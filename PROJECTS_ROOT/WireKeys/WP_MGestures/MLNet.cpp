// MLNet.cpp: implementation of the CMLNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MLNet.h"


MLNet::MLNet(unsigned l_size, ...)
{
	set_transfer_function(0);
	set_minmax(.0, 1.);
	set_bias(1.);

	_ASSERTE(l_size >= 2); 
	
	m_layers.resize(l_size);

	unsigned number_of_weights = 0;
	unsigned t = 0;

	va_list arg;
	va_start(arg, l_size);
	for (unsigned i = 0; i < m_layers.size(); ++i)
	{
		m_layers[i] = va_arg(arg, unsigned);
		number_of_weights +=  (t * m_layers[i]);
		t = m_layers[i] + 1;	// add bias	
	}		
	va_end(arg);

	m_weights.resize(number_of_weights);
}

void MLNet::propagate(array_t& in_synapsis, array_t& out_axons)
{
	array_t::iterator  w = m_weights.begin();
	layers_t::iterator l = m_layers.begin();
	
	// an input layer
	_ASSERTE(in_synapsis.size() == *l);
	++l;

	// a second layer
	out_axons.resize(*l);
	propagate_layer(in_synapsis.begin(), in_synapsis.end(), out_axons.begin(), out_axons.end(), w);
	++l;
		
	// a next layers if any
	for (; l != m_layers.end(); ++l)
	{
		in_synapsis.swap(out_axons);
		out_axons.resize(*l);
		propagate_layer(in_synapsis.begin(), in_synapsis.end(), out_axons.begin(), out_axons.end(), w);
	}
		
	_ASSERTE(w == m_weights.end());
}

void MLNet::propagate_layer(array_t::iterator i_begin, array_t::iterator i_end,
		array_t::iterator o_begin, array_t::iterator o_end,	array_t::iterator& w)
{
	typedef array_t::iterator iterator;	
	array_t::value_type v;
	
	for (iterator o = o_begin; o != o_end; ++o)
	{				
		// bias		
		v = ((*w) * m_r * m_bias) / m_r;
		++w;
		
		for (iterator i = i_begin; i != i_end; ++i, ++w)		
		{		
			v += (((*i) * m_r) * ((*w) * m_r)) / m_r;					
		}		

		if (m_fn) 
		{		
			*o = m_fn(v);			// transfer function		
			_ASSERTE(*o >=  m_min);
			_ASSERTE(*o <=  m_max);
		}
		else *o = v;	
	}	
}

/////////////////////////////////////////////////////////


MLNet::Learn::Learn(MLNet& net, fn_derivative fd)
	: m_net(net), m_fd(fd)
{
	unsigned number_of_neurons = 0;
	for (layers_t::iterator l = (m_net.m_layers.begin() + 1); l != m_net.m_layers.end(); ++l)
		number_of_neurons += (*l);

	m_errors.resize(number_of_neurons);
	m_deltas.resize(m_net.m_weights.size());
}

void MLNet::Learn::propagate(array_t& in_synapsis, array_t& out_axons)
{
	// learn mode
	array_t::iterator  w = m_net.m_weights.begin();
	layers_t::iterator l = m_net.m_layers.begin();
	
	 // an input layer
	_ASSERTE(in_synapsis.size() == *l);
	++l;

	// a second layer
	out_axons.resize(*l);
	m_net.propagate_layer(in_synapsis.begin(), in_synapsis.end(), out_axons.begin(), out_axons.end(), w);
	++l;
	
	// a next layers if any
	array_t::iterator o_begin = out_axons.begin();
	array_t::iterator o_end   = out_axons.end();
	array_t::iterator i_begin, i_end;
	unsigned n_begin, n_end;
			
	for (; l != m_net.m_layers.end(); ++l)
	{	
		n_begin = o_begin  - out_axons.begin();
		n_end   = o_end	   - out_axons.begin();		
				
		out_axons.resize(out_axons.size() + *l);
					
		i_begin = out_axons.begin() + n_begin;
		i_end   = out_axons.begin() + n_end;
		o_begin = i_end;
		o_end   = out_axons.end();

		_ASSERTE(o_begin == (out_axons.begin() + out_axons.size() - *l));
				
		m_net.propagate_layer(i_begin, i_end, o_begin, o_end, w);
	}
	
	_ASSERTE(w == m_net.m_weights.end());
}

void MLNet::Learn::compute_errors(array_t& v_out, array_t& v_ideal)
{
	typedef array_t::reverse_iterator reverse_iterator;	

	reverse_iterator o = v_out.rbegin(); 
	reverse_iterator e = m_errors.rbegin(); 
	reverse_iterator w = m_net.m_weights.rbegin();

	// output layer
	for (reverse_iterator x = v_ideal.rbegin(); x != v_ideal.rend(); ++x, ++o, ++e)
	{	
		*e = ((*x) - (*o)) * m_fd(*o);
	//	*e = ((*o) - (*x)) * m_fd(*o);
	}

	// hidden layers
	
	typedef layers_t::reverse_iterator reverse_layer;
	reverse_layer l		= m_net.m_layers.rbegin();
	reverse_layer l_end = m_net.m_layers.rend();
		
	--l_end; // skips a input layer
	#ifdef _DEBUG
	unsigned l_input_size = *l_end;
	#endif // _DEBUG
			
	reverse_iterator ne_begin = m_errors.rbegin();
	reverse_iterator ne_end   = ne_begin + *l;
	
	unsigned nl_size = *l;
	++l;	// skips the output's layer too (already computed)

	for (; l != l_end; ++l)
	{	
		reverse_iterator te = e;		// remember the start of this layer
				
		// for every neuron into layer
		for (unsigned n = 0; n < *l; ++n, ++e, ++o, ++w)
		{			
			double dsum = .0;
			reverse_iterator nw = w;			
			for (reverse_iterator ne = ne_begin; ne != ne_end; ++ne)
			{
				dsum += ((*ne) * (*nw));
				nw += (*l + 1); // skip bias 
			}
			
			*e = value_t(dsum * m_fd(*o));
		}		
									
		ne_begin = te;
		ne_end   = ne_begin + *l;		

		// shift w		
		w += ((*l) + 1) * (nl_size - 1) + 1;
		nl_size = *l;
	}
	
	_ASSERTE(e == m_errors.rend());	
	_ASSERTE(o == v_out.rend());
	_ASSERTE(w == (m_net.m_weights.rend() - (l_input_size * nl_size + nl_size)));
}

void MLNet::Learn::compute_deltas(array_t& v_out, double M, double N)
{	
	typedef array_t::iterator iterator;	
	iterator e = m_errors.begin();
	iterator o = v_out.begin();
	iterator d = m_deltas.begin();

	typedef layers_t::iterator layer;
	layer l = m_net.m_layers.begin();
	unsigned pl_size = *l;
	++l; // skips a input layer
	
	for (; l != m_net.m_layers.end(); ++l)
	{					
		// for every neuron into layer 
		for (unsigned n = 0; n < *l; ++n, ++e)
		{
			// bias
			*d = delta(N, M, *d, *e, m_net.m_bias);			
			++d;

			iterator po = o;
			for (unsigned k = 0; k < pl_size; ++k, ++d, ++po)
			{			
				*d = delta(N, M, *d, *e, *po);
			}
		}		

		o += pl_size;
		pl_size = *l;		
	}

	_ASSERTE(e == m_errors.end());	
	_ASSERTE(o == v_out.end());
	_ASSERTE(d == m_deltas.end());
}

void MLNet::Learn::update()
{
	typedef array_t::iterator iterator;		
	iterator d = m_deltas.begin();

	for (iterator w = m_net.m_weights.begin(); w != m_net.m_weights.end(); ++w, ++d)
	{
		*w += *d;
	}

	_ASSERTE(d == m_deltas.end());
}

void MLNet::Learn::epoch(array_t& v_in, array_t& v_ideal, double N, double M)
{
	// in fact back-propagation	

	array_t v_out;	
	propagate(v_in, v_out);				
	compute_errors(v_out, v_ideal);
	v_out.insert(v_out.begin(), v_in.begin(), v_in.end());
	v_out.resize(v_out.size() - m_net.m_layers.back());
	compute_deltas(v_out, M, N);
	update();	
}

void MLNet::Learn::run(unsigned cycles, double N0, double N1, double M)
{
	randomize_weigths(- .25, .25);
	
	array_t v_in(m_net.m_layers.front());
	array_t v_ideal(m_net.m_layers.back());

	double dN = (N0 - N1) / cycles;	 
//	double tN = N0;

	for (unsigned i = 0; i < cycles; ++i)
	{		
		if (!get(0, i, v_in, v_ideal))
			return; // stop learning
		epoch(v_in, v_ideal, N0, M);

		_ASSERTE(v_in.size() == m_net.m_layers.front());
		_ASSERTE(v_ideal.size() == m_net.m_layers.back());
		
		#if 0		
		// is-c0nverged
		// change N
		if (N0 < tN  && 
			check_convergence(0.0001))
		{				
			tN = N0 * N0;
			N0 = sqrt(N0);
			if (cycles != i)
				dN = (N0 - N1) / (cycles - i);
		}
		#endif

		N0 -= dN;	
	}
}

void MLNet::Learn::randomize_weigths(float lo, float hi)
{	
	for (array_t::iterator i = m_net.m_weights.begin(); i != m_net.m_weights.end(); ++i)
	{
		*i = lo + ((float)rand() / RAND_MAX) * (hi - lo);
		_ASSERTE(*i >= lo);
		_ASSERTE(*i <= hi);
	}		
}

bool MLNet::Learn::check_convergence(double limit)
{		
	for (array_t::iterator d = m_deltas.begin(); d != m_deltas.end(); ++d)
	{
		if (fabs(*d) > limit) return false;
	}
	return true;
}

float sigmoid(float f)	
{
	return float(1. / (1. + exp(-f)));
}

float d_sigmoid(float f)
{
	return float(f *  (1. - f));
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMLNet::CMLNet():
m_net(3, NET_INPUT_SIZE, NET_INPUT_SIZE, NET_OUTPUT_SIZE),
m_angles (NUMBER_OF_ANGLES),
m_cosines(NUMBER_OF_ANGLES),
m_sinuses(NUMBER_OF_ANGLES)
{
	m_net.set_transfer_function(sigmoid);
	m_net.set_bias(0.5);
	m_net.set_minmax(.0, 1.);
}

CMLNet::~CMLNet()
{

}

bool CMLNet::LoadNet(const TCHAR* filename)
{
	HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (file == INVALID_HANDLE_VALUE) return false;

	DWORD data_size = GetFileSize(file, 0);

	if (data_size != m_net.size_weight() * sizeof(MLNet::array_t::value_type))
		return false; // invalid size

	MLNet::array_t::value_type* p = (MLNet::array_t::value_type*)_alloca(data_size);
		
	DWORD number_of_read = 0;	
	BOOL r = ReadFile(file, p, data_size, &number_of_read, 0);
	_ASSERTE(r); r;
	VERIFY(CloseHandle(file));

	if (data_size != number_of_read)
		return false; // invalid size

	typedef MLNet::array_t::iterator iterator;
	MLNet::array_t::value_type* t = p;
	for (iterator i = m_net.begin_weight(); i != m_net.end_weight(); ++i, ++t)
		*i = *t;
	return true;
}

#include <limits>
bool CMLNet::TransfromPath()
{
	typedef path_t::iterator iterator;

	m_path2.assign(m_path.begin(), m_path.end());
		
	if (m_path.size()<NUMBER_OF_ANCHOR_POINTS)
	{
		// todo : stretch path or warning message;		
		//	m_path.clear();
		// m_path2.clear();m_mode = recognizing_mode;
		return false;
	}	
	else if (NUMBER_OF_ANCHOR_POINTS < m_path.size())
	{
		// smooth path		
		// finds smallest interval and replaces two points on median point

		while (m_path2.size() > NUMBER_OF_ANCHOR_POINTS)
		{
			double d;
			double d_min = DBL_MAX;//std::numeric_limits<double>::max();
			
			iterator p_min  = m_path2.begin();
			++p_min;

			iterator p		= p_min;
			iterator i		= p_min;
			++i;

			iterator last	= m_path2.end();
			--last;

			for (; i != last; ++i)
			{
				d = sqrt(pow(double((*p).x - (*i).x), 2) + pow(double((*p).y - (*i).y), 2));
				if (d < d_min)
				{
					d_min = d;
					p_min = p;
				}
				p = i;
			}

			p = p_min;
			i = ++p_min;

			POINT pt = {((*p).x + (*i).x) / 2, ((*p).y + (*i).y) / 2};
			*i = pt;				// changes coord of a base point
			m_path2.erase(p);		// erases an odd point 
		}
	}		
	else
	{
		// nothing		
	}		
	
	_ASSERTE(m_path2.size() == NUMBER_OF_ANCHOR_POINTS);

	// computes angles, cosines and sines

	iterator i = m_path2.begin();
	iterator p = i++;
	unsigned n = 0;

	for (; i != m_path2.end(); ++i, ++n)
	{
		POINT pt2 = (*i);		
		POINT pt1 = (*p);		

		pt2.x -= pt1.x;
		pt2.y -= pt1.y;

		if (pt2.x || pt2.y)
		{
			m_cosines[n] = float(pt2.y / sqrt(double(pt2.x * pt2.x + pt2.y * pt2.y)));
			m_sinuses[n] = (float)sqrt(1. - m_cosines[n] * m_cosines[n]);		
			if (pt2.x < 0) m_sinuses[n] = - m_sinuses[n];		
			m_angles[n] = float(acos(m_cosines[n]) * 180. / pi);
			if (pt2.x < 0) m_angles[n] = 360.f - m_angles[n];
		}
		else
		{
			m_cosines[n] = 1;
			m_sinuses[n] = 0;
			m_angles[n]  = 0;
		}
		
		p = i;

		_ASSERTE(m_cosines[n] <=  1.);
		_ASSERTE(m_cosines[n] >= -1.);
		_ASSERTE(m_sinuses[n] <=  1.);
		_ASSERTE(m_sinuses[n] >= -1.);
		_ASSERTE(m_angles [n] <= 360.);
		_ASSERTE(m_angles [n] >= 0.);		
	}	

	return true;
}

int CMLNet::RecognizeBoard()
{	
	if(!TransfromPath()){
		return -1;
	};
	MLNet::array_t v_in (NET_INPUT_SIZE);
	MLNet::array_t v_out(NET_OUTPUT_SIZE);

	std::copy(m_cosines.begin(), m_cosines.end(), v_in.begin());
	std::copy(m_sinuses.begin(), m_sinuses.end(), v_in.begin() + RANGE_SIZE);

	m_net.propagate(v_in, v_out);

	typedef MLNet::array_t::iterator iterator;

	// apply softmax to a net output vector and find winner
	
	double sum = std::accumulate(v_out.begin(), v_out.end(), 0.);

	for (unsigned n = 0; n < sizeof(m_winners)/sizeof(m_winners[0]); ++n)
	{
		iterator i = std::max_element(v_out.begin(), v_out.end());
		m_winners[n].m_id = i - v_out.begin();
		m_winners[n].m_probability = (float)(double(*i) / sum);
		*i = 0;
	}	

	// verify winner 

	#define MIN_PROBABILITY 0.25
	#define MIN_DIFFERENCE	0.25

	if (m_winners[0].m_probability > MIN_PROBABILITY && 
	   (m_winners[0].m_probability - m_winners[1].m_probability) > MIN_DIFFERENCE)
	{
		return m_winners[0].m_id;
	}

	return -1;
}

const char* const pattern_names[NUMBER_OF_PATTERNS] = 
{
	"up",
	"down",
	"up-down",
	"down-up",	
	"right",
	"left",
	"right-left",
	"left-right",
	"down-right",
	"up-left",
	"rectangle",
	"down-left",
	"up-right",
	"left arrow",
	"right arrow",
	"zigzag",	
	"down arrow",
	"up arrow",
	"down-left cross",
	"down-right cross",
	"helix",
	"triangle",	
	"hour-glass",
	"flag",
	"circle",
	"wave",
	"N",
	"M",	
	"W",
};

const char* CMLNet::getPatternName(int i)
{
	return pattern_names[i];
}