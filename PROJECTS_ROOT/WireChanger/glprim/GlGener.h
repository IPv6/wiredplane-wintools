// glprim.h : main header file for the GLPRIM DLL
//

#if !defined(AAFX_GLPRIM_H__D57A0563_4856_4DF0_BCDC_981C1E9DBE6F__INCLUDED_)
#define AAFX_GLPRIM_H__D57A0563_4856_4DF0_BCDC_981C1E9DBE6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class Perlin
{
public:  
	Perlin(int octaves,double freq,double amp,int seed, int iSample);
	~Perlin();
	double noise1(double arg);
	double noise2(double vec[2]);
	double noise3(double vec[3]);
	void normalize2(double v[2]);
	void normalize3(double v[3]);
	void init(void);
	int   mOctaves;
	double mFrequency;
	double mAmplitude;
	int   mSeed;
	int mMaxSample;
	int *p;
	double (*g3)[3];
	double (*g2)[2];
	double *g1;
	/*
	int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	double g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
	double g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
	double g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	*/
	bool  mStart;
	double Get1D(double x)
	{
		return perlin_noise_1D(x);
	};
	
	
	double Get2D(double x,double y)
	{
		//if(mMaxSample>100)
		/*{
			x=ceil(x*900.0f)/300.0f;
			y=ceil(y*900.0f)/300.0f;
		}*/
		double vec[2];
		vec[0] = x;
		vec[1] = y;
		return perlin_noise_2D(vec);
	};
	
	
	double Get3D(double x,double y,double z)
	{
		double vec[3];
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		return perlin_noise_3D(vec);
	};
	
	double perlin_noise_1D(double vec);
	double perlin_noise_2D(double vec[2]);
	double perlin_noise_3D(double vec[3]);
	
};

double rndDbl(double _min, double _max);
#endif // !defined(AAFX_GLPRIM_H__D57A0563_4856_4DF0_BCDC_981C1E9DBE6F__INCLUDED_)
