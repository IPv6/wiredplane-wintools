#include "fractal"
#include <fstream>

using namespace fractals;

template<class _Ty>
		class myfractal : public mandelbrot<_Ty> {
public:
		myfractal(unsigned short _W, unsigned short _H)
				: mandelbrot<_Ty>(_W, _H) {}
protected:
		virtual void color(unsigned _N, unsigned char& _R,
				unsigned char& _G, unsigned char& _B)
				{_R = _N, _G = _N * 2, _B = _N * 3; }
		};

int main()
{
	{
		// myfractal
		myfractal<double> m(400, 400);
		m.render(-1.250, 0.048, 0.005, 0.005);

		std::ofstream os0("myfractal.tga", std::ios_base::binary);
		os0 << m;
	}

	{
		// mandelbrot set
		mandelbrot<double> m0(400, 400);
		mandelbrot<double> m1 = m0;			// test copy constructor
		mandelbrot<double> m(200, 200);
		m = m1;								// test assignment operator
	
		m.render(-1.5, 1.5, 3, 3);			// 400x400
	
		std::ofstream os("mandelbrot.tga", std::ios_base::binary);
		os << m;
	}

	{
		// sea horse valley
		mandelbrot<double> m(400, 400);
		m.render(-1.250, 0.048, 0.005, 0.005);

		std::ofstream os("seahorsevalley.tga", std::ios_base::binary);
		os << m;
	}

	{
		// san marco fractal
		julia<float> j0(400, 400);
		julia<float> j1 = j0;				// test copy constructor
		julia<float> j(200, 200);
		j = j1;								// test assignment operator
	
		j.c = std::complex<float>(-3.0f / 4.0f, 0);
		j.render(-1.5, 1.5, 3, 3);			// 400x400
	
		std::ofstream os("sanmarco.tga", std::ios_base::binary);
		os << j;
	}

	{
		// cactus fractal
		cactus<float> c0(400, 400);
		cactus<float> c1 = c0;				// test copy constructor
		cactus<float> c(200, 200);
		c = c1;								// test assignment operator
	
		c.render(-1.5, 1.5, 3, 3);			// 400x400
	
		std::ofstream os("cactus.tga", std::ios_base::binary);
		os << c;
	}
}

