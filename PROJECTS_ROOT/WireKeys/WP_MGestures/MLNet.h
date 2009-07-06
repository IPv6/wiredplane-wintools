// MLNet.h: interface for the CMLNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MLNET_H__7D0C5C28_4F13_4B2C_83C8_E7D3CE2A5B12__INCLUDED_)
#define AFX_MLNET_H__7D0C5C28_4F13_4B2C_83C8_E7D3CE2A5B12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(push, 3)
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>
#pragma warning(pop)
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <process.h>
#include <intshcut.h>
typedef std::list<POINT>	path_t;
typedef std::vector<float>	vector_real_t;
struct winner
{
	unsigned m_id;				// index of pattern in pattern_data array
	float    m_probability;
};
const double pi = 3.1415926535;
const double kk = pi / 180.;


#define RANGE_SIZE				16
#define NET_INPUT_SIZE			(RANGE_SIZE*2)
#define NET_OUTPUT_SIZE			29
#define NUMBER_OF_PATTERNS		NET_OUTPUT_SIZE
#define NUMBER_OF_ANCHOR_POINTS	RANGE_SIZE
#define NUMBER_OF_ANGLES		RANGE_SIZE

class MLNet
{
public:
	typedef float value_t;
	typedef std::vector<value_t> array_t;
	typedef value_t (*fn_transfer)(value_t);
	
	class Learn
	{
	public:
		typedef value_t (*fn_derivative)(value_t);

	protected:
		MLNet&			m_net;
		array_t			m_errors;
		array_t			m_deltas;		
		fn_derivative	m_fd; 
		
	public:
		Learn(MLNet&, fn_derivative);
		virtual ~Learn() {}

		void epoch(array_t& v_in, array_t& v_ideal, double N, double M);
		void run  (unsigned cycles, double N0, double N1, double M);
		
	protected:
		virtual bool get(unsigned, unsigned, array_t& v_in, array_t& v_ideal) = 0;

		void compute_errors(array_t& v_out, array_t& v_ideal);
		void compute_deltas(array_t& v_out, double, double);
		void update();	
		void randomize_weigths(float, float);
		bool check_convergence(double);
	//	void check_paralysis(array_t&, double);

		void propagate(array_t& in_synapsis, array_t& out_axons);

		float delta (double N, double M, double d, double e , double y) 
		{		
			return  float( N * ((M * d) + ((1. - M) * e * y)));
		//	return  float(- N * ((M * d) + ((1. - M) * e * y)));
		}		
	};

	friend class MLNet::Learn;
	
	MLNet(unsigned number_of_layers, ...);

	fn_transfer set_transfer_function(fn_transfer fn)	{fn_transfer t = m_fn; m_fn = fn; return t;}
	void set_minmax(value_t min_p, value_t max_p)		{m_min = min_p; m_max = max_p; m_r = float(1. / m_max);}
	void set_bias(value_t bias)							{m_bias = bias;}
	value_t get_min() const								{return m_min;}
	value_t get_max() const								{return m_max;}
	unsigned get_input_size() const						{return m_layers.front();}
	unsigned get_output_size() const					{return m_layers.back();}

	// warning: will change a parameters of in_synapsys and out_axons vectors (size, capacity, values) 	
	void propagate(array_t& in_synapsis, array_t& out_axons);

	friend 
	std::ostream& operator << (std::ostream&, MLNet&);

	array_t::iterator begin_weight()			{return m_weights.begin();}
	array_t::iterator end_weight()				{return m_weights.end();}
	array_t::size_type size_weight()			{return m_weights.size();}	
	
 private:

	void propagate_layer(array_t::iterator i_begin, array_t::iterator i_end,
		array_t::iterator o_begin, array_t::iterator o_end,	array_t::iterator& w);

	typedef std::vector<unsigned> layers_t;

	array_t			m_weights;
	layers_t		m_layers;
	fn_transfer		m_fn;
	value_t			m_min;
	value_t			m_max;	
	value_t			m_r;		// reciprocal value : 1 / m_max		
	value_t			m_bias;
};

class CMLNet  
{
	MLNet			m_net;
	vector_real_t m_angles;
	vector_real_t m_cosines;
	vector_real_t m_sinuses;
	winner		m_winners[3];
public:
	path_t		m_path;
	path_t		m_path2;
	CMLNet();
	virtual ~CMLNet();
	bool LoadNet(const TCHAR* filename);
	bool TransfromPath();
	int RecognizeBoard();
	static const char* getPatternName(int i);
};

#endif // !defined(AFX_MLNET_H__7D0C5C28_4F13_4B2C_83C8_E7D3CE2A5B12__INCLUDED_)
