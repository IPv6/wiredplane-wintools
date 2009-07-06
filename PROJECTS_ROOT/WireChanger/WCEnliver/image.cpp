// Copyright (c) 2002 Andreas Jцnsson, www.AngelCode.com

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "image.h"
#include "IEnliver.h"
#include <memory.h>  // memset()
#include <stdlib.h>  // rand()

// Global variables
extern CIEnliver objSettings;
void InitImage(BOOL bFully)
{
	// Clear image to black
	if(bFully){
		objSettings.dwStart=0;
	}
	objSettings.a=double(rand())/RAND_MAX+1;
	objSettings.b=double(rand())/RAND_MAX+3;
	objSettings.c=double(rand())/RAND_MAX+1;
	objSettings.a1=double(rand())/RAND_MAX+1;
	objSettings.b1=double(rand())/RAND_MAX+2;
	objSettings.c1=double(rand())/RAND_MAX+2;
	objSettings.t1=double(rand())/RAND_MAX/20;
	objSettings.t=3;
	if(double(rand())/RAND_MAX>0.5){
		objSettings.dwYS=0;
		objSettings.dwYS2=objSettings.szBitmapSize.cy;
		objSettings.dwXS=DWORD(double(rand())/RAND_MAX*double(objSettings.szBitmapSize.cx));
		objSettings.dwXS2=DWORD(double(rand())/RAND_MAX*double(objSettings.szBitmapSize.cx));
		if(objSettings.dwXS>objSettings.dwXS2){
			DWORD dwTmp=objSettings.dwXS;
			objSettings.dwXS=objSettings.dwXS2;
			objSettings.dwXS2=dwTmp;
		}
	}else{
		objSettings.dwXS=0;
		objSettings.dwXS2=objSettings.szBitmapSize.cx;
		objSettings.dwYS=DWORD(double(rand())/RAND_MAX*double(objSettings.szBitmapSize.cy));
		objSettings.dwYS2=DWORD(double(rand())/RAND_MAX*double(objSettings.szBitmapSize.cy));
		if(objSettings.dwYS>objSettings.dwYS2){
			DWORD dwTmp=objSettings.dwYS;
			objSettings.dwYS=objSettings.dwYS2;
			objSettings.dwYS2=dwTmp;
		}
	}
	objSettings.dwStepType=DWORD(double(rand())/RAND_MAX*4);
}

#define TSIZE	360
#define PI		3.141592
const double dDivivder=TSIZE/2/PI;
double xcos(double d)
{
	return cos(d);
	static BOOL bInitOK=0;
	static double cos_t[TSIZE]={0};
	if(bInitOK==0){
		bInitOK=1;
		for(int i=0;i<TSIZE;i++){
			cos_t[i]=double(cos(i*(2*PI/TSIZE)));
		}
	}
	return cos_t[int(d*dDivivder)%TSIZE];
}

double xsin(double d)
{
	return sin(d);
	static BOOL bInitOK=0;
	static double sin_t[TSIZE]={0};
	if(bInitOK==0){
		bInitOK=1;
		for(int i=0;i<TSIZE;i++){
			sin_t[i]=double(sin(i*(2*PI/TSIZE)));
		}
	}
	return sin_t[int(d*dDivivder)%TSIZE];
}

DWORD dwMyRndSeed=0;
#define MYRAND_MAX 0x7fff
DWORD myrand()
{
	return( ((dwMyRndSeed = dwMyRndSeed * 214013L + 2531011L) >> 16) & MYRAND_MAX );
}

int randEx()
{
	static DWORD iFirstValue=0;
	if(iFirstValue==0){
		iFirstValue=1;
		dwMyRndSeed=(unsigned)(time(NULL)-GetCurrentThreadId()*7);
	}
	return myrand();
}

// —лучайное число ¬ Ћё„јя границы
int rnd(int _min, int _max)
{
	int iRndVal=randEx();
	double d1=double(iRndVal)/double(MYRAND_MAX+1);
	int iRes = _min + (int)floor(double(_max - _min + 1)*d1);// RAND_MAX+1 - чтобы небыло единицы, а floor округлит в меньшую сторону
	return iRes;
}

double rndDbl(double _min, double _max)
{
	double d=double(randEx());
	return _min + d / MYRAND_MAX * (_max - _min);
}

void UpdateImage()
{
	// Very simple noise effect
	if(objSettings.dbBgImage.m_Bits==NULL){
		return;
	}
	double iRealImageXStep=double(objSettings.dbBgImage.m_Size.cx)/objSettings.dwImgW;
	double iRealImageYStep=double(objSettings.dbBgImage.m_Size.cy)/objSettings.dwImgH;
	DWORD dwQMask=((objSettings.dwQuality==1)?0x3:0xF);
	if(objSettings.dwEffect==4){
		static double 	d1=rndDbl(10,30);
		static double 	d2=d1-rndDbl(10,30);
		static double 	d3=rndDbl(10,30);
		static double 	d4=d3-rndDbl(10,30);
		static double 	d5=rndDbl(10,30);
		static double 	d6=d5-rndDbl(10,30);
		static double 	dXt=rndDbl(10,30);
		static double 	dYt=rndDbl(10,30);
		static double 	dTt=rndDbl(100,1000);
		static double 	dt=rndDbl(5,10);
		static double  t=rndDbl(0.0001,0.2);
		t+=dt;
		static int		iX1=rnd(0,objSettings.dwImgW);
		static int		iY1=rnd(0,objSettings.dwImgH);
		static int		iX2=rnd(0,objSettings.dwImgW);
		static int		iY2=rnd(0,objSettings.dwImgH);
		static int		iX3=rnd(0,objSettings.dwImgW);
		static int		iY3=rnd(0,objSettings.dwImgH);
		static int		iX4=rnd(0,objSettings.dwImgW);
		static int		iY4=rnd(0,objSettings.dwImgH);
		static double	dNormoLen=DISTANCE(0,0,objSettings.dwImgW,objSettings.dwImgH)+rnd(-300,300);
		for( long xPos = 0; xPos < objSettings.dwImgW; xPos=xPos+((objSettings.dwQuality<2)?(rand()&dwQMask):0)+1){
			for( long yPos = objSettings.dwImgH-1; yPos >= 0; yPos=yPos-((objSettings.dwQuality<2)?(rand()&dwQMask):0)-1){
				//t+=dt;
				double dXVklad=0;
				double dYVklad=0;
				dXVklad=DISTANCE(xPos,yPos,iX1,iY1)/dNormoLen*(dXt);
				dYVklad=DISTANCE(xPos,yPos,iX2,iY2)/dNormoLen*(dYt);
				//-------------
				double dRes=0;
				dRes+=sin(d1*dXVklad+d2)*cos(d3*dXVklad+d4);
				dRes+=cos(d2*dYVklad+d1)*sin(d4*dYVklad+d3);
				dXVklad=DISTANCE(xPos,yPos,iX3,iY3)/dNormoLen*(dXt+t/dTt);
				dYVklad=DISTANCE(xPos,yPos,int(iX4+t/dTt),iY4)/dNormoLen*(dYt);
				dRes+=cos(d1*dXVklad+d2)*cos(d3*dXVklad+d4);
				dRes+=sin(d2*dYVklad+d1)*sin(d4*dYVklad+d3);
				DWORD dwColor=objSettings.image[yPos*objSettings.dwImgW+xPos];
				double dNorma=fabs(dRes)/4;
				int iYPosNew=int(yPos+dNorma*50);
				if(1){
					double dNormaMin=dNorma*0.6;
					dwColor=RGB(int(GetRValue(dwColor)*(1-dNormaMin)+255*dNormaMin),int(GetGValue(dwColor)*(1-dNormaMin)+255*dNormaMin),int(GetBValue(dwColor)*(1-dNormaMin)+255*dNormaMin));
				}
				int kk;
				for(kk=yPos;kk<iYPosNew;kk++){
					if(xPos>=0 && xPos<objSettings.dwImgW && kk>=0 && kk<objSettings.dwImgH){
						objSettings.image[kk*objSettings.dwImgW+xPos]=dwColor;
						//dibSrc.SetPixel(xPos,kk,dwColor,bMixMode);
					}
				}
				if(1){
					double dwPercentage=.9;
					if(xPos+1>=0 && xPos+1<objSettings.dwImgW && kk+1>=0 && kk+1<objSettings.dwImgH){
						DWORD dwRGB=objSettings.image[kk*objSettings.dwImgW+xPos];
						dwRGB=RGB(GetRValue(dwRGB)*dwPercentage,
							GetGValue(dwRGB)*dwPercentage,
							GetBValue(dwRGB)*dwPercentage);
						objSettings.image[kk*objSettings.dwImgW+xPos]=dwRGB;
						//dibSrc.DarkenPixel(xPos+1,kk+1,0.9);
					}
				}
			}
		}
	}
	if(objSettings.dwEffect==3){
		if(objSettings.dwStart==0){
			memcpy(objSettings.image,objSettings.dbBgImage.m_Bits,sizeof(DWORD)*objSettings.dwImgW*objSettings.dwImgH);
			objSettings.dwStart=1;
		}
		if(objSettings.dwStepType==0)
		{
			for( long x = objSettings.dwXS; x < objSettings.dwXS2; x=x+1){
				for( long y = objSettings.dwYS; y < objSettings.dwYS2; y=y+1){
					if(x+1<objSettings.dwXS2){
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[y*objSettings.dwImgW+x+1];
					}else{
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[y*objSettings.dwImgW+objSettings.dwXS];
					}
				}
			}
		}
		if(objSettings.dwStepType==1)
		{
			for( long x = objSettings.dwXS; x < objSettings.dwXS2; x=x+1){
				for( long y = objSettings.dwYS; y < objSettings.dwYS2; y=y+1){
					if(y+1<objSettings.dwYS2){
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[(y+1)*objSettings.dwImgW+x];
					}else{
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[objSettings.dwYS*objSettings.dwImgW+x];
					}
				}
			}
		}
		if(objSettings.dwStepType==2)
		{
			for( long x = objSettings.dwXS2; x >= objSettings.dwXS; x=x-1){
				for( long y = objSettings.dwYS2; y >= objSettings.dwYS; y=y-1){
					if(x-1>objSettings.dwXS){
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[y*objSettings.dwImgW+x-1];
					}else{
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[y*objSettings.dwImgW+objSettings.dwXS2];
					}
				}
			}
		}
		if(objSettings.dwStepType==3)
		{
			for( long x = objSettings.dwXS2; x >= objSettings.dwXS; x=x-1){
				for( long y = objSettings.dwYS2; y >= objSettings.dwYS; y=y-1){
					if(y-1>objSettings.dwYS){
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[(y-1)*objSettings.dwImgW+x];
					}else{
						objSettings.image[y*objSettings.dwImgW+x]=objSettings.image[objSettings.dwYS2*objSettings.dwImgW+x];
					}
				}
			}
		}
		objSettings.t++;
		if(objSettings.t>40){
			InitImage(FALSE);
			objSettings.t=100-double(rand())/RAND_MAX*20;
		}
	}else if(objSettings.dwEffect==0){
		for( DWORD x = 0; x < objSettings.dwImgW; x=x+((objSettings.dwQuality<2)?(rand()&dwQMask):0)+1){
			double dXc=double(x)/objSettings.dwImgW;
			for( DWORD y = 0; y < objSettings.dwImgH; y=y+((objSettings.dwQuality<2)?(rand()&dwQMask):0)+1){
				double dYc=double(y)/objSettings.dwImgH;
				int dx=(int)(xsin((objSettings.a+dYc)*objSettings.t+objSettings.b1)*xcos((objSettings.c+dXc)*objSettings.t+objSettings.b)*20);
				int dy=(int)(xcos((objSettings.a1+dXc)*objSettings.t+objSettings.b1)*xsin((objSettings.c1+dYc)*objSettings.t+objSettings.b)*20);
				COLORREF pix2=0;
				COLORREF pix=objSettings.image[y*objSettings.dwImgW+x];//objSettings.dbBgImage.GetPixel(x*iRealImageXStep,objSettings.dbBgImage.m_Size.cy-y*iRealImageYStep-1);
				long lOffsetX=x+dx;
				long lOffsetY=y+dy;
				if(lOffsetX<0){
					lOffsetX=-lOffsetX;
				}
				if(lOffsetX>=long(objSettings.dwImgW)){
					lOffsetX=objSettings.dwImgW-(lOffsetX-objSettings.dwImgW)-1;
				}
				if(lOffsetY<0){
					lOffsetY=-lOffsetY;
				}
				if(lOffsetY>=long(objSettings.dwImgH)){
					lOffsetY=objSettings.dwImgH-(lOffsetY-objSettings.dwImgH)-1;
				}
				pix2=objSettings.dbBgImage.GetPixel(int((lOffsetX)*iRealImageXStep),int(objSettings.dbBgImage.m_Size.cy-(lOffsetY)*iRealImageYStep-1));
				objSettings.image[y*objSettings.dwImgW+x]=RGB(int(GetRValue(pix)+GetRValue(pix2))/2,int(GetGValue(pix)+GetGValue(pix2))/2,int(GetBValue(pix)+GetBValue(pix2))/2);
				//objSettings.image[y*objSettings.dwImgW+x]=pix2;
			}
		}
		objSettings.t+=objSettings.t1;
		if(objSettings.t>7 && objSettings.t1>0){
			objSettings.t=7;
			objSettings.t1=-objSettings.t1;
			objSettings.a+=((double(rand())/RAND_MAX)-0.5)/4;
			objSettings.b+=((double(rand())/RAND_MAX)-0.5)/4;
			objSettings.c+=((double(rand())/RAND_MAX)-0.5)/4;
		}else if(objSettings.t<1 && objSettings.t1<0){
			objSettings.t=1;
			objSettings.t1=-objSettings.t1;
			objSettings.a1+=((double(rand())/RAND_MAX)-0.5)/4;
			objSettings.b1+=((double(rand())/RAND_MAX)-0.5)/4;
			objSettings.c1+=((double(rand())/RAND_MAX)-0.5)/4;
		}
		//objSettings.t1+=0.02;
		//objSettings.c+=objSettings.t1;
		//objSettings.a+=objSettings.t1;
		/*if(objSettings.c>5 || objSettings.c<0){
			objSettings.t1=-objSettings.t1;
		}else if(objSettings.a>5 || objSettings.a<0){
				objSettings.t1=-objSettings.t1;
		}*/
	}else if(objSettings.dwEffect==1){
		for( DWORD x = 0; x < objSettings.dwImgW; x=x+((objSettings.dwQuality<2)?(rand()&dwQMask):0)+1){
			for( DWORD y = 0; y < objSettings.dwImgH; y=y+((objSettings.dwQuality<2)?(rand()&dwQMask):0)+1){
				//double rnd=double(rand())/RAND_MAX;
				double rnd=0.5*(1+(xsin((objSettings.a+double(x)/objSettings.dwImgW*2)*objSettings.t+objSettings.b)*xcos((objSettings.c+double(y)/objSettings.dwImgH*4)*objSettings.t+objSettings.a)));
				double rnd2=0.5*(1+(xsin((objSettings.a1+double(x)/objSettings.dwImgW*3)*objSettings.t1+objSettings.b1)*xcos((objSettings.c1+double(y)/objSettings.dwImgH*3)*objSettings.t1+objSettings.a)));
				double rnd3=0.5*(1+(xsin((objSettings.b1+double(x)/objSettings.dwImgW*4)*objSettings.t+objSettings.a)*xcos((objSettings.c+double(y)/objSettings.dwImgH*2)*objSettings.t1+objSettings.c1)));
				COLORREF pix2=objSettings.dbBgImage.GetPixel(int(x*iRealImageXStep),int(objSettings.dbBgImage.m_Size.cy-y*iRealImageYStep-1));
				objSettings.image[y*objSettings.dwImgW+x]=RGB((int)(rnd*GetRValue(pix2)),(int)(rnd2*GetGValue(pix2)),(int)(rnd3*GetBValue(pix2)));
			}
		}
		objSettings.t+=0.04;
		objSettings.t1+=0.03;
	}else{
		for( DWORD x = 0; x < objSettings.dwImgW; x=x+((objSettings.dwQuality<2)?(rand()&dwQMask):0)+1){
			for( DWORD y = 0; y < objSettings.dwImgH; y=y+((objSettings.dwQuality<2)?(rand()&dwQMask):0)+1){
				double rnd=0.5+(double(rand())/RAND_MAX/2);
				COLORREF pix2=objSettings.dbBgImage.GetPixel(int(x*iRealImageXStep),int(objSettings.dbBgImage.m_Size.cy-y*iRealImageYStep-1));
				objSettings.image[y*objSettings.dwImgW+x]=RGB((int)(rnd*GetRValue(pix2)),(int)(rnd*GetGValue(pix2)),(int)(rnd*GetBValue(pix2)));
			}
		}
	}
}

BOOL ClearBitmap(HBITMAP& hBitmap)
{
	if(hBitmap){
		DeleteObject(hBitmap);
		hBitmap=NULL;
	}
	return TRUE;
}

CSize GetBitmapSize(HBITMAP pBmp)
{
	if(pBmp==NULL){
		return CSize(0,0);
	}
	BITMAP pBitMap;
	::GetObject(pBmp, sizeof(BITMAP), &pBitMap);
	CSize size(pBitMap.bmWidth,pBitMap.bmHeight);
	return size;
}
