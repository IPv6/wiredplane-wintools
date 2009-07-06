// glfrac.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "glfrac.h"
#include "GlGener.h"
#include "fractal.h"
#include "../../SmartWires/SystemUtils/Debug.h"
#include <afxcmn.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define JLOG5(x,z1,z2,z3,z4,z5)	DEBUG_INFO5(x,z1,z2,z3,z4,z5);
#define JLOG4(x,z1,z2,z3,z4)	DEBUG_INFO5(x,z1,z2,z3,z4,0);
#define JLOG3(x,z1,z2,z3)		DEBUG_INFO5(x,z1,z2,z3,0,0);
#define JLOG2(x,z1,z2)			DEBUG_INFO5(x,z1,z2,0,0,0);
#define JLOG1(x,z1)				DEBUG_INFO5(x,z1,0,0,0,0);
#define JLOG(x)					DEBUG_INFO5(x,0,0,0,0,0);

/////////////////////////////////////////////////////////////////////////////
// CglfracApp

BEGIN_MESSAGE_MAP(CglfracApp, CWinApp)
//{{AFX_MSG_MAP(CglfracApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CglfracApp construction

CglfracApp::CglfracApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CglfracApp object
CglfracApp theApp;

CImgLoader*& WINAPI _bmpX()
{
	TRACE1("%x WINAPI _bmpX()!\n",GetCurrentThreadId());
	static CImgLoader* bmpThis=0;
	/*
	if(!bmpThis){
	bmpThis=&_bmp();
	}
	*/
	return bmpThis;
}

CBitmap* LoadBmpFromPathExt(const char* szPath)
{
	CString sPath=szPath;
	CBitmap* bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	if(!bmpExtern){
		sPath=Format("%s.jpg",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	if(!bmpExtern){
		sPath=Format("%s.gif",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	if(!bmpExtern){
		sPath=Format("%s.bmp",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	if(!bmpExtern){
		sPath=Format("%s.ico",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	CBitmap* bmp=0;
	if(bmpExtern){
		bmp=bmpExtern;//_bmp().Clone(bmpExtern);
		//_bmpX()->ReleaseBmp(bmpExtern);
	}
	return bmp;
}


BOOL PrepareCommon(const char* sBlock, long& lOP, CRect& rtFull, CCallbacks* pCallback, const char* szDefTTText)
{
	long lX=atol(CDataXMLSaver::GetInstringPart("X:(",")",sBlock));
	long lY=atol(CDataXMLSaver::GetInstringPart("Y:(",")",sBlock));
	long lW=atol(CDataXMLSaver::GetInstringPart("W:(",")",sBlock));
	long lH=atol(CDataXMLSaver::GetInstringPart("H:(",")",sBlock));
	lOP=atol(CDataXMLSaver::GetInstringPart("OPACITY:(",")",sBlock));
	if(lOP<0){
		lOP=0;
	}
	if(lOP>100){
		lOP=100;
	}
	rtFull.SetRect(lX,lY,lX+lW,lY+lH);
	BOOL bInteract=atol(CDataXMLSaver::GetInstringPart("INTERACTIVITY:(",")",sBlock));
	if(bInteract && pCallback){
		CString sIntAct=CDataXMLSaver::GetInstringPart("INTERACTIVITY_ACTION:(",")",sBlock);
		if(sIntAct==""){
			sIntAct=CDataXMLSaver::GetInstringPart("<INTERACTIVITY_ACTION>","</INTERACTIVITY_ACTION>",sBlock);
		}
		CString sTooltip=CDataXMLSaver::GetInstringPart("INTERACTIVITY_TOOLTIP:(",")",sBlock);
		if(sTooltip==""){
			sTooltip=CDataXMLSaver::GetInstringPart("INTERACTIVITY_TOOLTIP:<![CDATA[","]]>",sBlock);
		}
		if(sTooltip==""){
			sTooltip=szDefTTText;
		}
		if(sTooltip!=""){
			sIntAct+="<TOOLTIP>";
			sIntAct+=sTooltip;
			sIntAct+="</TOOLTIP>";
		}
		if(sIntAct!=""){
			pCallback->AddActiveZone(&rtFull, sIntAct);
		}
	}
	return TRUE;
}


static DWORD dwTempForPreserve=0;
inline BOOL NOPRESERVE(BYTE* X, DWORD dwPreserveBrMin, DWORD dwPreserveBrMax)
{
	if(dwPreserveBrMin==0 && dwPreserveBrMax==0){
		return TRUE;
	}
	DWORD dwTempForPreserve=(X[0]+X[1]+X[2])/3;
	if(dwTempForPreserve<dwPreserveBrMax && dwTempForPreserve>dwPreserveBrMin){
		return TRUE;
	}
	return FALSE;
}

int getRndBmpFormat()
{
	switch(rnd(1,8))
	{
	case 1:
		return SRCPAINT;
		break;
	case 2:
		return SRCAND;
		break;
	case 3:
		return SRCINVERT;
		break;
	case 4:
		return SRCERASE;
		break;
	case 5:
		return NOTSRCCOPY;
		break;
	case 6:
		return NOTSRCERASE;
		break;
	case 7:
		return MERGECOPY;
		break;
	default:
		return MERGEPAINT;
	}
	return SRCCOPY;
}

inline BOOL NOPRESERVE(CDib& dibDst,int x,int y,DWORD dwPreserveBrMin,DWORD dwPreserveBrMax)
{
	if((dwPreserveBrMin==0 && dwPreserveBrMax==0) || x<0 || y<0){
		return TRUE;
	}
	BYTE* dst=(BYTE*)(dibDst.m_Bits+(y*dibDst.m_Size.cx)+x);
	return NOPRESERVE(dst,dwPreserveBrMin,dwPreserveBrMax);
}

//		if(fB>_dwInfiniMax){_dwInfiniMax=fB;};
//		if(fB<_dwInfiniMin){_dwInfiniMin=fB;};
//		_dwInfiniMid=long(sqrt(fB*fB+_dwInfiniMid*_dwInfiniMid));
#define SETFRACPIXEL(x,y,fB){\
		if(_dwInfiniMin==-1){_dwInfiniMin=fB;}\
		if(_dwInfiniMax==-1){_dwInfiniMax=fB;}\
		if(_dwInfiniMid==-1){_dwInfiniMid=fB;}\
		if(fB>_dwInfiniMax){_dwInfiniMax=fB;};\
		if(fB<_dwInfiniMin){_dwInfiniMin=fB;};\
		dibSrc.SetPixel(x,y,DWORD(fB),0);\
	}

// Для определения типа
#define GET_CHECK_BYTE(aaa,bbb) long iCheckByte=(aaa);\
	if(bNeedSP){\
	BYTE iCheckByte2=RGB2BW_F(bbb);\
	switch(bRandom01){\
		case 1:iCheckByte=(iCheckByte2);break;\
		case 2:iCheckByte=(iCheckByte+iCheckByte2)>>1;break;\
		case 3:iCheckByte=(iCheckByte-iCheckByte2);break;\
		case 4:iCheckByte=(iCheckByte2-iCheckByte);break;};\
		if(bRandom01b){iCheckByte=255-iCheckByte;};\
		if(bRandom01c){iCheckByte=rnd(0,iCheckByte);};\
	};

BOOL ApplyImageEffect(const char* szEffect,const char* szBlock, CCallbacks* pCallback, CDC& dc, CBitmap& bmpDst, long iL, long iT, long iW, long iH, CDC& dc2, CBitmap& bmp)
{
	if(iH==0 || iW==0){
		return FALSE;
	}
	CString sTemp;
	long iWF=iW+iL;
	CDib dibSrc,dibDst;
	dibSrc.Create(iWF,iH);
	dibSrc.SetBitmap(&dc2,&bmp, 0);
	dibDst.Create(iWF,iH);
	dibDst.SetBitmap(&dc,&bmpDst, iT);
	BYTE *src=(BYTE*)dibSrc.m_Bits;
	BYTE *dst=(BYTE*)dibDst.m_Bits;
	BYTE *dstOrig=dst;
	BYTE *srcOrig=src;
	int iSizeOrig=dibSrc.m_Size.cx*dibSrc.m_Size.cy;
	int iSizeOrigDst=dibDst.m_Size.cx*dibDst.m_Size.cy;
	BOOL bFLATTEN=atol(CDataXMLSaver::GetInstringPart("FLATTEN-LAYERS-INT:(",")",szBlock));
	if(bFLATTEN){
		memcpy(dibDst.m_Bits,dibSrc.m_Bits,dibDst.m_Size.cx*dibDst.m_Size.cy*sizeof(dibDst.m_Bits[0]));
	}else{
		memset(dibSrc.m_Bits,0,dibSrc.m_Size.cx*dibSrc.m_Size.cy*sizeof(dibSrc.m_Bits[0]));
	}
	int iSize=iSizeOrig;
	if(iL>0){
		// Сдвигаем картинку на нужную величину
		src=srcOrig;
		for(int y=0;y<iH;y++){
			memmove(src+iL*4,src,iW*4);
			// Следующая линия...
			src+=iWF*4;
		}
	}
	BOOL bNeedOpacityProcessing=0;
	BOOL bNeedSP=atol(CDataXMLSaver::GetInstringPart("SMART-PRESERVE:(",")",szBlock));
	BOOL bRandom01=atol(CDataXMLSaver::GetInstringPart("SMART-PRESERVE-TYPE:(",")",szBlock));
	if(bRandom01==0){
		bRandom01=rnd(1,5);
	}
	BOOL bRandom01b=rnd(0,1);
	BOOL bRandom01c=((rnd(0,100)>80)?1:0);
	// Идем дальше...
	long iRand=rnd(10,60),lrx=rnd(0,dibDst.m_Size.cx),lry=rnd(0,dibDst.m_Size.cy);
	long iPopBrLev=RGB2BW_F(dibDst.GetPixel(lrx,lry));
	long dwPreserveBrMin=(long(iPopBrLev-iRand)>=0?(iPopBrLev-iRand):0);
	long dwPreserveBrMax=(long(iPopBrLev+iRand)<256?(iPopBrLev+iRand):255);
	long _dwInfiniMin=-1;
	long _dwInfiniMax=-1;
	long _dwInfiniMid=-1;
	long _dwInfiniLocalMin=0;
	long _dwInfiniLocalMax=100;
	long dwQuality=atol(CDataXMLSaver::GetInstringPart("QUALITY:(",")",szBlock));
	long dwRandomize=atol(CDataXMLSaver::GetInstringPart("RANDOMIZE:(",")",szBlock));
	float fParX=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAMX:(",")",szBlock));
	float fParY=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAMY:(",")",szBlock));
	float fParW=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAMW:(",")",szBlock));
	float fParH=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAMH:(",")",szBlock));
	float fPar1=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAM1:(",")",szBlock));
	float fPar2=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAM2:(",")",szBlock));
	float fPar3=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAM3:(",")",szBlock));
	float fPar4=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAM4:(",")",szBlock));
	float fPar5=atof(CDataXMLSaver::GetInstringPart("FRACTAL-PARAM5:(",")",szBlock));
	// Палитра...
	DWORD dwPalette=atol(CDataXMLSaver::GetInstringPart("PALETTE:(",")",szBlock));
	BYTE btPalR[256]={0};
	BYTE btPalG[256]={0};
	BYTE btPalB[256]={0};
	double dShift1=rndDbl(0,6.3);
	double dShift2=rndDbl(0,6.3);
	double dShift3=rndDbl(0,6.3);
	double dFraction1=rndDbl(10,400);
	double dFraction2=rndDbl(10,400);
	double dFraction3=rndDbl(10,400);
	long i,p,q;
	for(i=0;i<256;i++){
		switch(dwPalette){
		case 1:
			btPalR[i]=int(i);
			break;
		case 2:
			btPalG[i]=int(i);
			break;
		case 3:
			btPalB[i]=int(i);
			break;
		case 4:
			btPalR[i]=int(i);
			btPalG[i]=int(i);
			btPalB[i]=int(i);
			break;
		default:
			{
				btPalR[i]=l2B(fabs(127+127*sin(i/dFraction1*(3.14/2)+dShift1)));
				btPalG[i]=l2B(fabs(127+127*sin(i/dFraction2*(3.14/2)+dShift2)));
				btPalB[i]=l2B(fabs(127+127*sin(i/dFraction3*(3.14/2)+dShift3)));
			}
			break;
		}
	}
	_dwInfiniLocalMin=-3000;
	_dwInfiniLocalMax=3000;
	CRect rtDesk;
	GetWindowRect(GetDesktopWindow(),&rtDesk);
	float dAspRatio=float(rtDesk.Height())/float(rtDesk.Width());
	// Накладываем эффект...
	if(strstr(szEffect,"GENE_MANDEL")!=NULL){
		mandelbrot<float> j(dibSrc.m_Bits,iW, iH);
		j.iterations=dwQuality;
		//The Mandelbrot doesn't exist outside a circle of radius 2 on the complex plane, this gives scope for turning it inside out by mapping z0 to 1/z0. The result, center at (1.4,0), is as follows.
		//zn+1 = zn2.1 + zo
		//zn+1 = zn-3 + zo
		//j.render(-0.8, 0.0, 2.7, 0, dAspRatio);
		if(dwRandomize){
			fParX=rndDbl(-1.2,0.5);
			fParY=rndDbl(-0.2,0.2);
			fParW=rndDbl(.1,2.7);
			fPar4=rndDbl(-.9,.9);
			fPar5=rndDbl(-.9,.9);
		}
		j.c = std::complex<float>(fPar4, fPar5);
		j.render(fParX, fParY, fParW, fParH, dAspRatio);
		_dwInfiniMin=j.Min;
		_dwInfiniMax=j.Max;
	}
	// Еще:
	//http://astronomy.swin.edu.au/~pbourke/fractals/marek/
	//http://astronomy.swin.edu.au/~pbourke/fractals/juliaset/
	//http://astronomy.swin.edu.au/~pbourke/fractals/sinjulia/
	//http://astronomy.swin.edu.au/~pbourke/fractals/biomorph/
	//http://astronomy.swin.edu.au/~pbourke/fractals/newtonraphson/

	if(strstr(szEffect,"GENE_JULIA")!=NULL){
		julia<float> j(dibSrc.m_Bits,iW, iH);
		j.iterations=dwQuality;
		if(dwRandomize){
			fPar1=rndDbl(-0.5,0.5);
			fPar2=rndDbl(-0.5,0.5);
			fParX=rndDbl(0,2.9);
			fParW=3.0-fParX;
		}
		j.c = std::complex<float>(-3.0f / 4.0f+fPar1, fPar2);
		j.render(fParX, fParY, fParW, fParH, dAspRatio);
		_dwInfiniMin=j.Min;
		_dwInfiniMax=j.Max;
	}
	if(strstr(szEffect,"GENE_CACTUS")!=NULL){
		cactus<float> j(dibSrc.m_Bits,iW, iH);
		j.iterations=dwQuality;
		if(dwRandomize){
			fPar1=rndDbl(-1.2,0.5);
			fPar2=rndDbl(-0.2,0.2);
			fPar3=rndDbl(.1,2.7);
			fPar4=rndDbl(-.9,.9);
			fPar5=rndDbl(-.9,.9);
			fParW=2.0;
		}
		j.c = std::complex<float>(fPar4, fPar5);
		j.render(fParX, fParY, fParW, fParH, dAspRatio);
		_dwInfiniMin=j.Min;
		_dwInfiniMax=j.Max;
	}
	if(strstr(szEffect,"GENE_LEMON")!=NULL){
		lemon<float> j(dibSrc.m_Bits,iW, iH);
		j.iterations=dwQuality;
		if(dwRandomize){
			fPar1=rndDbl(-1.2,0.5);
			fPar2=rndDbl(-0.2,0.2);
			fPar3=rndDbl(.1,2.7);
			fPar4=rndDbl(-.9,.9);
			fPar5=rndDbl(-.9,.9);
			fParW=2.0;
		}
		j.c = std::complex<float>(fPar4, fPar5);
		j.render(fParX, fParY, fParW, fParH, dAspRatio);
		_dwInfiniMin=j.Min;
		_dwInfiniMax=j.Max;
	}
	if(strstr(szEffect,"GENE_CSQ")!=NULL){
		circlesq<float> j(dibSrc.m_Bits,iW, iH);
		j.iterations=dwQuality;
		if(dwRandomize){
			fPar1=rndDbl(-1.2,0.5);
			fPar2=rndDbl(-0.2,0.2);
			fPar3=rndDbl(.1,2.7);
			fPar4=rndDbl(-.9,.9);
			fPar5=rndDbl(-.9,.9);
			fParW=2.0;
		}
		j.c = std::complex<float>(fPar4, fPar5);
		j.render(fParX, fParY, fParW, fParH, dAspRatio);
		_dwInfiniMin=j.Min;
		_dwInfiniMax=j.Max;
	}
	if(strstr(szEffect,"GENE_TDRAG")!=NULL){
		int i,m,n,c,ix,iy;
		if(dwRandomize){
			fPar1=rndDbl(1,12);
		}
		m=int(fPar1);
		float x=1,y=1,x1=0,y1=0;
		float r,ra;
		float* a=new float[m];
		float* b=new float[m];
		long NX=iWF;
		long NY=iH;
		float TWOPI=float(2*3.1415926);
		//long N=100000000;
		long N=dwQuality*100000;
		N=rnd(N/3,N);
		float SCALE=(min(NX,NY) *0.3);
		float SQRT2 =sqrt(2.0);
		for (i=0;i<m;i++) {
			a[i] = cos(TWOPI * i / (float)m);
			b[i] = sin(TWOPI * i / (float)m);
		}
		r = sqrt(1.25 * m);
		for (n=0;n<N;n++) {
			c = rand() % m;
			ra = sqrt(3.0 * (x*x + y*y));
			x1 = - x / r + y / (ra * r) + a[c];
			y1 = - x / (ra * r) - y / r + b[c];
			x = x1;
			y = y1;
			if (n < 100)
				continue;
			ix = x * SCALE + NX/2;
			iy = y * SCALE + NY/2;
			SETFRACPIXEL(ix,iy,(n/1000))
		}
		delete[] a;
		delete[] b;
	}
	if(strstr(szEffect,"GENE_CIRCLES")!=NULL){
		int i,m,n,c,ix,iy;
		if(dwRandomize){
			fPar1=rndDbl(1,12);
		}
		m=int(fPar1);
		float x=1,y=1,x1=0,y1=0;
		float* a=new float[1+2*m];
		float* b=new float[1+2*m];
		long NX=iWF;
		long NY=iH;
		float TWOPI=2*3.1415926f;
		//long N=100000000;
		long N=dwQuality*100000;
		N=rnd(N/3,N);
		float SCALE=(min(NX,NY) *0.1 );
		float SQRT2 =sqrt(2.0);
		for (i=0;i<m;i++) {
			a[2*i] = cos(TWOPI * (i+1) / (float)m);
			b[2*i] = sin(TWOPI * (i+1) / (float)m);
		}
		for (i=0;i<m;i++) {
			a[2*i+1] = 0.5*(cos(TWOPI * (i+1) / (float)m) + cos(TWOPI * i / (float)m));
			b[2*i+1] = 0.5*(sin(TWOPI * (i+1) / (float)m) + sin(TWOPI * i / (float)m));
		}
		
		for (n=0;n<N;n++) {
			c = rand() % (2*m);
			x1 = (x/(x*x + y*y))/3 + a[c];
			y1 = (-y/(x*x + y*y))/3 + b[c];
			x = x1 / (x1*x1 + y1*y1);
			y = y1 / (x1*x1 + y1*y1);
			if (n < 100)
				continue;
			ix = x * SCALE + NX/2;
			iy = y * SCALE + NY/2;
			SETFRACPIXEL(ix,iy,(n/1000))
		}
		delete[] a;
		delete[] b;
	}
	if(strstr(szEffect,"GENE_PCORN")!=NULL){
		int m,n,ix,iy;
		if(dwRandomize){
			fPar1=rndDbl(1,25);
		}
		m=int(fPar1);
		float x=0,y=0,x1=0,y1=0,r;
		long NX=iWF;
		long NY=iH;
		float TWOPI=float(2*3.1415926);
		//long N=100000000;
		long N=dwQuality*10000;
		float SCALE=(min(NX,NY) *0.8 );
		float SQRT2 =sqrt(2.0);
		r = float(0.45);//rndDbl(0.9,1.1);
		x=rndDbl(-1,1);
		y=rndDbl(-1,1);
		for (n=0;n<N;n++) {
			x1 = r*sin(y + tan(3*y));
			y1 = r*sin(x + tan(3*x));
			x = x1;
			y = y1;
			if (n < 100)
				continue;
			ix = x * SCALE + NX/2;
			iy = y * SCALE + NY/2;
			SETFRACPIXEL(ix,iy,(n)%255)
		}
	}
	if(strstr(szEffect,"GENE_TREEROOT")!=NULL){
		int n,ix,iy;
		float x=0,y=0,x1=0,y1=0,denom;
		long NX=iWF;
		long NY=iH;
		//long N=100000000;
		long N=dwQuality*500000;
		float SCALE=(min(NX,NY) *0.5 );
		float SQRT2 =sqrt(2.0);
		for (n=0;n<N;n++) {
			//if ((n % (N/10)) == 0)break;
			switch (rnd(0,10)%2) {
			case 0:
				x1 = y / SQRT2 + 1.5;
				y1 = -x / SQRT2 + 1.5;
				break;
			case 1:
				x1 = -y / SQRT2 + 0.5;
				y1 = x / SQRT2 + 0.5;
				break;
			}
			switch (rnd(0,10)%2) {
			case 0:
				x = x1 - 2;
				y = y1;
				break;
			case 1:
				denom = (x1 - 2) * (x1 - 2) + y1 * y1;
				x = (x1 - 2) / denom;
				y = y1 / denom;
				break;
			}
			if (n < 100)
				continue;
			ix = (x+1.0) * SCALE + NX/2;
			iy = (y-0.75) * SCALE + NY/2;
			SETFRACPIXEL(ix,iy,(n/1000))
			//Draw_Pixel(image,NX,NY,ix,iy,black);
		}
	}
	if(strstr(szEffect,"GENE_FRACTAL")!=NULL){
		BOOL bMixMode=0;
		float x,ox,y,a,b,c,d,e,f,g,h,rt1,it1,rb1,ib1,b1,gg,ss,uu;
		float j,k,l,m,n,o,b2,rt2,it2,rb2,ib2;
		if(dwRandomize){
			fPar1=rndDbl(0.3,0.99);//0.5;// vary t (0<t<1) for different homotopys
			fPar2=rndDbl(-2,4);
			fPar3=rndDbl(-2,4);
			fPar4=rndDbl(-7,7);
			fPar5=rndDbl(-7,7);
		}
		if(fPar4==0){
			fPar4=0.1f;
		}
		if(fPar5==0){
			fPar5=0.1f;
		}
		float t = fPar1;
		float aa,bb,cc,dd;
		float RR = dwQuality*rndDbl(3,8);//200;
		a =fPar2;
		b =fPar3;
		aa=fPar4;cc=-aa;
		bb=fPar5;dd=-bb;
		float iRnd1=rndDbl(0.1,4);//rnd(2,4);
		float iRnd2=rndDbl(0.1,4);//rnd(2,4);
		float iRnd3=rndDbl(0.1,4);//rnd(2,4);
		float iRnd4=rndDbl(0.1,4);//rnd(2,4);
		float iRnd5=rndDbl(0.1,4);//rnd(2,4);
		float diWF=float(iWF);
		float diH=float(iH);
		for (p=0; p<iWF; p++)
		{
			ox = aa + ((cc-aa)*(p/diWF));
			for (q=0; q<iH; q++)
			{
				x=ox;
				y = bb + ((dd-bb)*(q/diH));
				for (i=1; i<dwQuality; i++)
				{
					c=x*x-y*y;d=2*x*y;    //start the math and the iterations.
					e=a*a-b*b;f=2*a*b;
					g=c*c-d*d;h=2*c*d;
					rt1=g+1+e+2*(c*a-b*d-c-a);
					it1= h+f+2*(c*b+d*a-d-b);
					rb1= e+4+4*(c+x*a-y*b-2*x-a);
					ib1= f+4*(d+x*b+a*y-2*y-b);
					b1=1/(rb1*rb1+ib1*ib1);
					j=x*c-y*d;k=x*d+y*c;
					l=j+2+e+3*(x*a-y*b-x-a);
					m= k+f+3*(x*b+a*y-y-b);
					n= e+3+3*(c+x*a-y*b-2*x-a);
					o= f+3*(d+x*b+a*y-2*y-b);
					rt2=l*l-m*m;it2=2*l*m;
					rb2=n*n-o*o;ib2=2*n*o;
					b2=1/(rb2*rb2+ib2*ib2);
					x = t*(rt1*rb1+it1*ib1)*b1+(1-t)*(rt2*rb2+it2*ib2)*b2; //new x
					y = t*(it1*rb1-rt1*ib1)*b1+(1-t)*(it2*rb2-rt2*ib2)*b2; //new y				
					gg=(x*x+y*y);ss=gg+1;uu=ss-2*x; //test distance from roots
					if (gg>RR) {
						SETFRACPIXEL(p,q,i)
						break;
					}
				}/*next i*/
				if(i>=dwQuality){
					SETFRACPIXEL(p,q,int(gg)%int(RR))
				}
			}/*next q*/
		}/*next p*/
	}
	JLOG1("Generated %s:",szEffect);
	JLOG4("XYWH: %f;%f;%f;%f",fParX,fParY,fParW,fParH);
	JLOG5("12345: %f;%f;%f;%f",fPar1,fPar2,fPar3,fPar4,fPar4);

	if(_dwInfiniMax>_dwInfiniLocalMax){
		_dwInfiniMax=_dwInfiniLocalMax;
	}
	if(_dwInfiniMin<_dwInfiniLocalMin){
		_dwInfiniMin=_dwInfiniLocalMin;
	}
	if(_dwInfiniMax<_dwInfiniMin){
		long l=_dwInfiniMax;
		_dwInfiniMax=_dwInfiniMin;
		_dwInfiniMin=l;
	}
	BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
	{// Нормируем палитру
		BYTE *srcTmp=srcOrig;
		BYTE *dstTmp=dstOrig;
		int iSizeTmp=iSizeOrig;
		DWORD aBrights[64]={0};
		while (iSizeTmp--){
			long xPos=(iSizeOrig-iSizeTmp)%iWF;
			long yPos=(iSizeOrig-iSizeTmp)/iWF;
			if(iL>0){
				if(xPos<iL && iSizeTmp>0){
					srcTmp+=4*(iL-xPos);
					dstTmp+=4*(iL-xPos);
					iSizeTmp-=(iL-xPos);
					xPos=iL;
				}
			}
			long lValue=*((long*)srcTmp);
			if(lValue<_dwInfiniMin){
				lValue=_dwInfiniMin;
			}
			if(lValue>_dwInfiniMax){
				lValue=_dwInfiniMax;
			}
			BYTE bt=BYTE(255.0*double(lValue-_dwInfiniMin)/double(_dwInfiniMax-_dwInfiniMin));
			DWORD dwColor=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
			GET_CHECK_BYTE(dwColor,*srcTmp);
			if(bNeedSP && iCheckByte<bMixModeLevel){
				*((DWORD*)srcTmp)=*((DWORD*)dstTmp);
			}else{
				*((DWORD*)srcTmp)=dwColor;
			}
			srcTmp+=4;
			dstTmp+=4;
		}
	}
	if(bNeedOpacityProcessing){
		BOOL bEffectMaskAdjust=atol(CDataXMLSaver::GetInstringPart("MASK-ADJUST:(",")",szBlock));
		if(bEffectMaskAdjust){
			// ------------------------
			// Подгоняем уровень
			// ------------------------
			long aBrights[64]={0};
			long aBrights2[64]={0};
			for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
				long xPos=(iSizeOrig-iSize)%iWF;
				if(xPos<iL){
					continue;
				};				int iDiff=0;
				int iBr1=(src[0]+src[1]+src[2])/3;
				int iBr2=(dst[0]+dst[1]+dst[2])/3;
				BYTE iBra=iBr1>>2;
				if(iBra>63){
					iBra=63;
				}
				aBrights[iBra]++;
				BYTE iBrb=iBr2>>2;
				if(iBrb>63){
					iBrb=63;
				}
				aBrights2[iBrb]++;
			}
			// Считаем популярную яркость
			long iPopBr=0,iPopBrLev=0;
			for(int ib=0;ib<64;ib++){
				if(iPopBr<aBrights[ib]){
					iPopBr=aBrights[ib];
					iPopBrLev=ib<<2;
				}
			}
			long iPopBr2=0,iPopBrLev2=0;
			for(int ib2=0;ib2<64;ib2++){
				if(iPopBr2<aBrights2[ib2]){
					iPopBr2=aBrights[ib2];
					iPopBrLev2=ib2<<2;
				}
			}
			int iCount=0;
			for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
				long xPos=(iSizeOrig-iSize)%iWF;
				if(xPos<iL){
					continue;
				};				int iBr1=(src[0]+src[1]+src[2])/3;
				int iBr2=(dst[0]+dst[1]+dst[2])/3;
				long A=dst[3];
				BOOL bCase1=(iBr1>iPopBrLev-50 && iBr1<iPopBrLev+50);
				BOOL bCase2=(iBr2>iPopBrLev2-50 && iBr2<iPopBrLev2+50);
				if(bCase1||bCase2){
					if(bCase2){
						A=(A+iBr2-iBr1)<<2;
					}else{
						A=(A+iBr2-iBr1)>>2;
					}
				}
				int AOrig=A;
				DWORD dwPrev=0;
				A=(A+dwPrev)/2;
				dst[3]=BYTE(A);
				dwPrev=AOrig;
			}
		}
	}
	// ------------------------
	// Все!!! считаем результат
	// ------------------------
	if(bNeedOpacityProcessing>=0){
		int iCount=0;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};			DWORD A=bNeedOpacityProcessing?dst[3]:255;
			if(A!=0){
				if(A==255){
					*((DWORD*)dst)=*((DWORD*)src);
				}else{
					dst[0]=(BYTE)(((src[0]-dst[0])*A+(dst[0]<<8))>>8);
					dst[1]=(BYTE)(((src[1]-dst[1])*A+(dst[1]<<8))>>8);
					dst[2]=(BYTE)(((src[2]-dst[2])*A+(dst[2]<<8))>>8);
				}
			}
		}
	}
	// Копируем обратно...
	dibDst.Render(&dc,0,iT,iWF,iH);
	return TRUE;
}

BOOL WINAPI RenderScript(HBITMAP& hbmOutImage,const char* sScript, CCallbacks* pCallback)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());//AfxGetStaticModuleState()
	if(pCallback==0){
		return 0;
	}
	char szTmp[100]={0};
	int irq=pCallback->GetRndWallpaperPath(-99,szTmp);
	if(!(irq==15 && strcmp(szTmp,"QWEASS")==0)){
		return 0;
	}
	// Прямоугольник?
	int iFrom=0;
	CString sBlock;
	if(iFrom==-1){
		iFrom=0;
	}
	CBitmap bmpTemp;
	bmpTemp.Attach(hbmOutImage);
	if(iFrom==-1){
		iFrom=0;
	}
	CSafeDC dcScreen;
	dcScreen.CreateDC("DISPLAY", NULL, NULL, NULL);
	if(iFrom==-1){
		iFrom=0;
	}
	while(iFrom>=0){
		sBlock=CDataXMLSaver::GetInstringPart("<frac-in-rect>","</frac-in-rect>",sScript,iFrom);
		if(sBlock!=""){
			// Картинка в прямоугольнике!!!
			long lOP=100;
			CRect rtFull;
			PrepareCommon(sBlock,lOP,rtFull,pCallback,0);
			long lCols=atol(CDataXMLSaver::GetInstringPart("MOSAIC-COLS:(",")",sBlock));
			long lRows=atol(CDataXMLSaver::GetInstringPart("MOSAIC-ROWS:(",")",sBlock));
			if(lCols==0 || lRows==0){
				lRows=lCols=1;
			}
			CString sAntiCache=CDataXMLSaver::GetInstringPart("ANTI-CACHE:(",")",sBlock);
			long lXOffsPrc=atol(CDataXMLSaver::GetInstringPart("XDIFF:(",")",sBlock));
			long lYOffsPrc=atol(CDataXMLSaver::GetInstringPart("YDIFF:(",")",sBlock));
			BOOL bBorder=atol(CDataXMLSaver::GetInstringPart("BORDER:(",")",sBlock));
			BOOL bMirror=atol(CDataXMLSaver::GetInstringPart("MOSAIC-MIRROR:(",")",sBlock));
			BOOL bMirror1X=atol(CDataXMLSaver::GetInstringPart("MOSAIC-MIRROR-1X:(",")",sBlock));
			BOOL bMirror1Y=atol(CDataXMLSaver::GetInstringPart("MOSAIC-MIRROR-1Y:(",")",sBlock));
			BOOL bRndImages=atol(CDataXMLSaver::GetInstringPart("MOSAIC-RANDOM:(",")",sBlock));
			BOOL bTile=atol(CDataXMLSaver::GetInstringPart("TILE-IMAGE:(",")",sBlock));
			COLORREF lBG=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BORDER-COLOR:(",")",sBlock));
			long lOPStart=0,lOPStop=100,lEffect=-1;
			// Эффекты прозрачности...
			CString sEffect=CDataXMLSaver::GetInstringPart("EFFECT:(",")",sBlock);
			// Какую картинку....
			CString sSrc=CDataXMLSaver::GetInstringPart("<image-src>","</image-src>",sBlock);
			CSafeDC dc;
			dc.CreateCompatibleDC(&dcScreen);
			CBitmap* bmp=NULL;
			BOOL bNeedToEraseDC3=FALSE;
			if(sSrc!=""){
				bNeedToEraseDC3=TRUE;
				if(sSrc.Find("wc-id://")==-1){
					CBitmap* bmpExtern=_bmpX()->LoadBmpFromPath(sSrc);
					if(bmpExtern!=0){
						bmp=_bmp().Clone(bmpExtern);
						_bmpX()->ReleaseBmp(bmpExtern);
					}
				}else{
					HBITMAP hbmp=0;
					pCallback->GenerateFullFeaturedWallpaper(atol(sSrc.Mid(strlen("wc-id://"))),hbmp,CRect(CPoint(0,0),rtFull.Size()));
					if(bmp==0){
						bmp=new CBitmap();
					}
					bmp->Attach(hbmp);
				}
			}else{
				bmp=&bmpTemp;
			}
			if(bTile){
				MakeImageCobble(*bmp,rtFull.Size(),bTile);
			}
			CBitmap* bmOld=dc.SelectObject(&bmpTemp);
			if(bmp){
				CBitmap bmpTemp2;
				CSize bmpSize=GetBitmapSize(bmp);
				FitImageToRect(rtFull,bmpSize);
				CSize dimFullCell(rtFull.Size());
				// Проверка на мозаичность
				if(lRows>1 || lCols>1){
					HBITMAP hBmpScaled=0;
					dimFullCell.cx=dimFullCell.cx/lRows;
					dimFullCell.cy=dimFullCell.cy/lCols;
					CopyBitmapToBitmap(HBITMAP(*bmp), hBmpScaled, bmpSize, NULL, TRUE);
					bmpTemp2.Attach(hBmpScaled);
					if(bNeedToEraseDC3){
						delete bmp;
					}
					bmp=&bmpTemp2;
					// Очистится при выходе из if
					bNeedToEraseDC3=FALSE;
				}
				CSafeDC dc2,dc3;
				CBitmap bmp2;
				dc2.CreateCompatibleDC(&dc);
				dc3.CreateCompatibleDC(&dc);
				bmp2.CreateCompatibleBitmap(&dc, rtFull.Width(), rtFull.Height());
				CBitmap* bmOld2=dc2.SelectObject(&bmp2);
				dc2.SetStretchBltMode(COLORONCOLOR);
				CBitmap* bmOld3=dc3.SelectObject(bmp);
				int iInvertXStart=bMirror1X?1:0;
				int iInvertYStart=bMirror1Y?1:0;
				int iXStart=0;
				int iYStart=0;
				long lRealXOffset=0;
				long lRealYOffset=0;
				if(lXOffsPrc>0){
					iXStart=-1;
					iInvertXStart=1-iInvertXStart;
					lRealXOffset=long(double(dimFullCell.cx)*double(lXOffsPrc)/100);
				}
				if(lYOffsPrc>0){
					iYStart=-1;
					iInvertYStart=1-iInvertYStart;
					lRealYOffset=long(double(dimFullCell.cy)*double(lYOffsPrc)/100);
				}
				int iInvertX=iInvertXStart;
				int iInvertY=iInvertYStart;
				for(int iX=iXStart;iX<lRows;iX++){
					for(int iY=iYStart;iY<lCols;iY++){
						CRect cell;
						cell.left=lRealXOffset+iX*dimFullCell.cx+(iInvertX?dimFullCell.cx:0);
						cell.top=lRealYOffset+iY*dimFullCell.cy+(iInvertY?dimFullCell.cy:0);
						cell.right=cell.left+(iInvertX?-1:1)*(dimFullCell.cx+1);
						cell.bottom=cell.top+(iInvertY?-1:1)*(dimFullCell.cy+1);
						dc2.StretchBlt(cell.left, cell.top, cell.Width(), cell.Height(), &dc3, 0, 0, bmpSize.cx, bmpSize.cy, SRCCOPY);
						if(bMirror){
							iInvertY=1-iInvertY;
						}
						if(bRndImages){
							CBitmap* bmpToDel=dc3.SelectObject(bmOld3);
							char szPath[MAX_PATH]={0};
							pCallback->GetRndWallpaperPath(0,szPath);
							CBitmap* bmpExtern=_bmpX()->LoadBmpFromPath(szPath);
							CBitmap* bNewBmp=_bmp().Clone(bmpExtern);
							_bmpX()->ReleaseBmp(bmpExtern);
							if(bNewBmp){
								if(bmpToDel && bNeedToEraseDC3){
									delete bmpToDel;
								}
								bmpToDel=bNewBmp;
								bNeedToEraseDC3=TRUE;
							}
							bmpSize=GetBitmapSize(bmpToDel);
							bmOld3=dc3.SelectObject(bmpToDel);
						}
					}
					if(bMirror){
						iInvertX=1-iInvertX;
						iInvertY=iInvertYStart;
					}
				}
				CBitmap* bmpToDel=dc3.SelectObject(bmOld3);
				if(bmpToDel && bNeedToEraseDC3){
					delete bmpToDel;
				};
				if(bBorder){
					CRect rt(0,0,rtFull.Width(), rtFull.Height());
					dc2.Draw3dRect(&rt,lBG,lBG);
				}
				if(sEffect!=""){
					ApplyImageEffect(sEffect,sBlock,pCallback,dc,bmpTemp,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(), dc2, bmp2);
				}else{
					BLENDFUNCTION blendFunction;
					blendFunction.BlendFlags=0;
					blendFunction.AlphaFormat=0;
					blendFunction.BlendOp=AC_SRC_OVER;
					blendFunction.SourceConstantAlpha=(int)(double(255)*double(lOP)/double(100));
					AlphaBlendMy(dc,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(), dc2, 0, 0, blendFunction);
				}
				dc2.SelectObject(bmOld2);
			}
		}
	}
	bmpTemp.Detach();
	return TRUE;
}

CWnd* AfxWPGetMainWnd()
{
	return 0;
}

bool fakemulti=false;

#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif