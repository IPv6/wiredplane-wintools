// fractal header
#ifndef _FRACTAL_
#define _FRACTAL_
#include <complex>
#include <ostream>


// TEMPLATE CLASS fractal
template<class _Ty>
class fractal {
public:
	void render(_Ty _X0, _Ty _Y0, _Ty _W, _Ty _H, _Ty _AspRation=0){
		if(_AspRation>0.1){
			_H=_W*_AspRation;
		}
		_X0 -= _W/_Ty(2.0);
		_Y0 -= _H/_Ty(2.0);
		_Ty xAspect=_Ty(_W) / _Ty(_Width);
		_Ty yAspect=_Ty(_H) / _Ty(_Height);
		_Ty _Cx = 0;
		_Ty _Cy = 0;
		unsigned _N = (iterations <= 256) ?iterations : 256;
		for (unsigned long _Y = 0; _Y < _Height; _Y++){
			for (unsigned long _X = 0; _X < _Width; _X++){
				_Cx = _X0 + (_X * xAspect);
				_Cy = _Y0 + (_Y * yAspect);
				init(_Cx, _Cy);
				unsigned _I = 0;
				for (; _I < _N; _I++)
				{
					if (test())
						break;
					next(); 
				}
				if(_I>Max){
					Max=_I;
				}
				_M[_Y * _Width + _X] = _I;
			}
		}
	}
	
	fractal(unsigned long * _newM,unsigned short _W, unsigned short _H) :
	_Width(_W), _Height(_H), iterations(256),Min(0),Max(0),_M(_newM)
	{
		//_M = new unsigned char[_W * _H * 4];
		//::memset(_M, 0, sizeof(unsigned char) * _W * _H * 4); 
	}
	~fractal()
	{
		//delete[] _M; 
	}
	void setField(unsigned long * _newM)
	{
		_M=_newM;
	};
	unsigned short width() const
	{
		return _Width; 
	}
	unsigned short height() const
	{
		return _Height; 
	}
	unsigned char *data() const
	{
		return _M; 
	}
	unsigned iterations;
	long Min,Max;
protected:
	_Ty distance(const std::complex<_Ty>& _C)
	{
		return ::sqrt(_C.real() * _C.real() + _C.imag() * _C.imag()); 
	}
	virtual void init(_Ty _X, _Ty _Y) = 0;
	virtual bool test() = 0;
	virtual void next() = 0;
	unsigned short _Width, _Height;
	unsigned long *_M;
};

// TEMPLATE FUNCTION operator<<
template<class _Ty>
std::ostream& operator<<(std::ostream& _O, const fractal<_Ty>& _F)
{
	unsigned short _W = _F.width(), _H = _F.height();
	_O.write("\0\0\2\0\0\0\0\0\0\0\0\0", sizeof(char) * 12);
	_O.write((const char *)&_W, sizeof(unsigned short));
	_O.write((const char *)&_H, sizeof(unsigned short));
	_O.write("\x18\x20", sizeof(char) * 2);
	unsigned char *_P = _F.data(), _C[3];
	for (unsigned _I = 0; _I < _W * _H * 3U; _I += 3)
	{_C[0] = _P[_I + 2], _C[1] = _P[_I + 1], _C[2] = _P[_I];
	_O.write((const char *)_C, 3 * sizeof(unsigned char));
	}
	return _O; 
}

// TEMPLATE CLASS mandelbrot
template<class _Ty>
class mandelbrot : public fractal<_Ty> {
public:
	mandelbrot(unsigned long * _newM,unsigned short _W, unsigned short _H) :
	  fractal<_Ty>(_newM, _W, _H),c(0,0) {}
protected:
	virtual void init(_Ty _X, _Ty _Y)
	{
		_C = std::complex<_Ty>(_X, _Y);
		_Z = std::complex<_Ty>(0.0, 0.0); 
	}
	virtual bool test()
	{
		return (distance(_Z) > 2.0); 
		//return (distance(_Z) > (distance(std::complex<_Ty>(2, 0)/_C))); 
	}
	virtual void next()
	{
		std::complex<_Ty> _T = _Z * (_Z+c);			// keep borland happy
		_T += _C; _Z = _T; 
	}
	std::complex<_Ty> _Z, _C;
public:
	std::complex<_Ty> c;
};

// TEMPLATE CLASS julia
template<class _Ty>
class julia : public fractal<_Ty> {
public:
	std::complex<_Ty> c;
	julia(unsigned long * _newM,unsigned short _W, unsigned short _H) :
				fractal<_Ty>(_newM,_W, _H), c(0.5, 0.5) {}
protected:
	virtual void init(_Ty _X, _Ty _Y)
	{
		_Z = std::complex<_Ty>(_X, _Y); 
	}
	virtual bool test()
	{
		return (distance(_Z) > 2.0); 
	}
	virtual void next()
	{
		std::complex<_Ty> _T = _Z * _Z;			// keep borland happy
		_T += c; _Z = _T; 
	}
	std::complex<_Ty> _Z;
};

// TEMPLATE CLASS cactus
template<class _Ty>
class cactus : public fractal<_Ty> {
public:
	cactus(unsigned long * _newM,unsigned short _W, unsigned short _H) :
	  fractal<_Ty>(_newM,_W, _H),c(1,0) {}
protected:
	virtual void init(_Ty _X, _Ty _Y)
	{
		_Z = _Z0 = std::complex<_Ty>(_X, _Y); 
	}
	virtual bool test()
	{
		return (distance(_Z) > 2.0); 
	}
	virtual void next()
	{
		std::complex<_Ty> _T = _Z * _Z * _Z;		// keep borland happy
		_T += (_Z0 - c) * _Z;
		_Z = _T - _Z0; 
	}
	std::complex<_Ty> _Z, _Z0;
public:
	std::complex<_Ty> c;
};

template<class _Ty>
class lemon : public fractal<_Ty> {
public:
	lemon(unsigned long * _newM,unsigned short _W, unsigned short _H) :
	  fractal<_Ty>(_newM,_W, _H),c(1,0) {}
protected:
	virtual void init(_Ty _X, _Ty _Y)
	{
		_Zptest = _Z = _Z0 = std::complex<_Ty>(_X, _Y); 
		samecounter=0;
	}
	virtual bool test()
	{
		if (ABS(_Zptest.real()-_Z.real()) < 0.00001 && ABS(_Zptest.imag()-_Z.imag()) < 0.00001) 
			samecounter++;
		else
			samecounter = 0;
		_Zptest=_Z;
		if (samecounter > 4)
			return true;
		return false;
	}
	virtual void next()
	{
		std::complex<_Ty> _T = _Z0 * _Z * _Z * (_Z * _Z+c);
		std::complex<_Ty> _T2 = (_Z * _Z-c); 
		_T2=_T2*_T2;
		_Z=_T/_T2;
		
	}
	long samecounter;
	std::complex<_Ty> _Z, _Z0;
	std::complex<_Ty> _Zptest;
public:
	std::complex<_Ty> c;
};

template<class _Ty>
class circlesq : public fractal<_Ty> {
public:
	circlesq(unsigned long * _newM,unsigned short _W, unsigned short _H) :
	  fractal<_Ty>(_newM,_W, _H),c(1,0) {}
protected:
	virtual void init(_Ty _X, _Ty _Y)
	{
		_Z = _Z0 = std::complex<_Ty>(_X, _Y); 
		samecounter=0;
		counter=0;
	}
	virtual bool test()
	{
		return (distance(_Z) > 2.0); 
		/*
		if (ABS(_Zptest.real()-_Z.real()) < 0.00001 && ABS(_Zptest.imag()-_Z.imag()) < 0.00001) 
		samecounter++;
		else
		samecounter = 0;
		_Zptest=_Z;
		if (samecounter > 4)
		return true;
		return false;
		*/
	}
	virtual void next()
	{
		counter++;
		_Z = std::complex<_Ty>(long(distance(_Z * _Z))%(counter),0);// на самом деле - z*z mod counter;???
	}
	long samecounter;
	long counter;
	std::complex<_Ty> _Z, _Z0;
public:
	std::complex<_Ty> c;
};
#endif /* _FRACTAL_ */

/*
* http://home.tiscali.be/zoetrope
*/
