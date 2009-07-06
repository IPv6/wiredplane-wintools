bNoBGAtAll// glprim.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "glprim.h"
#include "GlGener.h"
#include <afxcmn.h>
#include "..\..\SmartWires\SystemUtils\RemindClasses.h"
#include "..\..\SmartWires\SystemUtils\Debug.h"

#include <string>
int getRndBmpFormat();
BOOL NOPRESERVE(BYTE* X, DWORD dwPreserveBrMin, DWORD dwPreserveBrMax);
BOOL NOPRESERVE(CDib& dibDst,int x,int y,DWORD dwPreserveBrMin,DWORD dwPreserveBrMax);

BOOL ApplyImageEffect(const char* szEffect,const char* szBlock, CCallbacks* pCallback, CDC& dcDst, CBitmap& bmpDst, long iL, long iTop, long iWidth, long iHeight, CDC& dcSrc, CBitmap& bmpSrc)
{
	DEBUG_INFO5("Generating effect %s on %i:%i-%i:%i",szEffect, iL, iTop, iWidth, iHeight);
	if(iHeight==0 || iWidth==0){
		DEBUG_INFO5("End generating effect %s on %i:%i-%i:%i",szEffect, iL, iTop, iWidth, iHeight);
		return FALSE;
	}
	long iWF=iWidth+iL;
	long iHF=iHeight+iTop;
	// Супер быстрые и сверхпростые эффекты
	if(strstr(szEffect,"SHAKER")!=NULL){
		CRect rcBounds(iL,iTop,iWF,iHF);
		dcSrc.BitBlt(0, 0, rcBounds.Width(), rcBounds.Height(), &dcDst, rcBounds.left, rcBounds.top, SRCCOPY);
		dcDst.SetStretchBltMode(COLORONCOLOR);
		dcDst.StretchBlt(rcBounds.left+1, rcBounds.top, rcBounds.Width(), rcBounds.Height(), &dcSrc, 0, 0, rcBounds.Width(), rcBounds.Height(), SRCINVERT);
		dcDst.StretchBlt(rcBounds.left-2, rcBounds.top, rcBounds.Width(), rcBounds.Height(), &dcSrc, 0, 0, rcBounds.Width(), rcBounds.Height(), MERGEPAINT);
		dcDst.StretchBlt(rcBounds.left, rcBounds.top+1, rcBounds.Width(), rcBounds.Height(), &dcSrc, 0, 0, rcBounds.Width(), rcBounds.Height(), MERGEPAINT);
		dcDst.StretchBlt(rcBounds.left, rcBounds.top-2, rcBounds.Width(), rcBounds.Height(), &dcSrc, 0, 0, rcBounds.Width(), rcBounds.Height(), SRCINVERT);
		DEBUG_INFO5("End generating effect %s on %i:%i-%i:%i",szEffect, iL, iTop, iWidth, iHeight);
		return TRUE;
	}
	if(strstr(szEffect,"UNPRE")!=NULL){
		CRect rcBounds(iL,iTop,iWF,iHF);
		dcSrc.BitBlt(0, 0, rcBounds.Width(), rcBounds.Height(), &dcDst, rcBounds.left, rcBounds.top, SRCCOPY);
		// Случайная
		dcDst.SetStretchBltMode(COLORONCOLOR);
		dcDst.StretchBlt(rcBounds.left, rcBounds.top, rcBounds.Width(), rcBounds.Height(), &dcSrc, 0, 0, rcBounds.Width(), rcBounds.Height(), SRCCOPY);
		int count=rnd(2,6);
		for(int i=0;i<count;i++){
			dcDst.StretchBlt(rcBounds.left+rnd(1,8)-4, rcBounds.top+rnd(1,8)-4, rcBounds.Width()+rnd(1,8)-4, rcBounds.Height()+rnd(1,8)-4, &dcSrc, 0, 0, rcBounds.Width(), rcBounds.Height(), getRndBmpFormat());
		}
		DEBUG_INFO5("End generating effect %s on %i:%i-%i:%i",szEffect, iL, iTop, iWidth, iHeight);
		return TRUE;
	}
	CString sTemp;
	CDib dibSrc,dibDst;
	//iT!!! iH!!!
	dibDst.Create(iWF,iHF);
	dibSrc.Create(iWF,iHF);
	dibDst.SetBitmap(&dcDst,&bmpDst, 0);
	int iSizeOrig=dibSrc.m_Size.cx*dibSrc.m_Size.cy;
	{
		CDib dibSrcFlat;
		dibSrcFlat.Create(iWidth,iHeight);
		dibSrcFlat.SetBitmap(&dcSrc,&bmpSrc, 0);
		dibSrc.PasteRect(&dibSrcFlat,iL,iTop,CDIBPASTE_DSTALL,0,0);
	}
	// 600->10,10 freq!!!
	int iPerleMax=100;//max(min(iWF,600),min(iHF,600));
	int iPerleMaxF=7;
	int iPerleMaxO=10;
	BYTE *src=(BYTE*)dibSrc.m_Bits;
	BYTE *dst=(BYTE*)dibDst.m_Bits;
	BYTE *dstOrig=dst;
	BYTE *srcOrig=src;
	int iSize=iSizeOrig;
	BOOL bNeedOpacityProcessing=0;
	BOOL bNeedDP=atol(CDataXMLSaver::GetInstringPart("SMART-LAYMODE:(",")",szBlock));
	BOOL bNeedSP=atol(CDataXMLSaver::GetInstringPart("SMART-PRESERVE:(",")",szBlock));
	BOOL bRandom01=atol(CDataXMLSaver::GetInstringPart("SMART-PRESERVE-TYPE:(",")",szBlock));
	if(bRandom01==0){
		bRandom01=rnd(1,5);
	}
	BOOL bRandom01b=rnd(0,1);
	BOOL bRandom01c=((rnd(0,100)>80)?1:0);
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
	// Идем дальше...
	DWORD dwPreserveBrMin=0;
	DWORD dwPreserveBrMax=0;
	DWORD dwPreserveBrMinDst=0;
	DWORD dwPreserveBrMaxDst=0;
	int iRw=rnd(0,4);
	int iGw=rnd(0,4);
	int iBw=rnd(0,4);
	if(iRw+iGw+iBw==0){
		iRw=iGw=iBw=1;
	}
	if(bNeedSP || bNeedDP){
		// Восстанавливаем верхнюю картинку!
		memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibDst.m_Size.cx*dibDst.m_Size.cy*sizeof(dibDst.m_Bits[0]));
	}
	
	if(bNeedSP){
		// Нормируем яркость
		DWORD aBrights[64]={0};
		DWORD aBrightsDst[64]={0};
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};
			int iBr=(iRw*src[0]+iGw*src[1]+iBw*src[2])/(iRw+iGw+iBw);
			int iBrDst=(iRw*dst[0]+iGw*dst[1]+iBw*dst[2])/(iRw+iGw+iBw);
			BYTE iBr2=iBr>>2;
			if(iBr2>63){
				iBr2=63;
			}
			aBrights[iBr2]++;
			BYTE iBr2Dst=iBrDst>>2;
			if(iBr2Dst>63){
				iBr2Dst=63;
			}
			aBrightsDst[iBr2Dst]++;
		}
		// Считаем популярную яркость
		int ib=0;
		int iRand0=rnd(-20,20);
		DWORD iPopBr=0,iPopBrLev=0;
		for(ib=0;ib<64;ib++){
			aBrights[ib]+=iRand0;
			if(iPopBr<aBrights[ib]){
				iPopBr=aBrights[ib];
				iPopBrLev=ib;
			}
		}
		iPopBrLev=iPopBrLev<<2;
		int iRand=rnd(10,60);
		dwPreserveBrMin=(long(iPopBrLev-iRand)>=0?(iPopBrLev-iRand):0);
		dwPreserveBrMax=(long(iPopBrLev+iRand)<256?(iPopBrLev+iRand):255);
		// Считаем популярную яркость
		iRand0=rnd(-20,20);
		iPopBr=0,iPopBrLev=0;
		for(ib=0;ib<64;ib++){
			aBrightsDst[ib]+=iRand0;
			if(iPopBr<aBrightsDst[ib]){
				iPopBr=aBrightsDst[ib];
				iPopBrLev=ib;
			}
		}
		iPopBrLev=iPopBrLev<<2;
		iRand=rnd(10,60);
		dwPreserveBrMinDst=(long(iPopBrLev-iRand)>=0?(iPopBrLev-iRand):0);
		dwPreserveBrMaxDst=(long(iPopBrLev+iRand)<256?(iPopBrLev+iRand):255);
	}
	// Накладываем эффект...
	if(strstr(szEffect,"BW")!=NULL || strstr(szEffect,"XRAY")!=NULL || strstr(szEffect,"CGLASS")!=NULL){
		BOOL bInvert=(strstr(szEffect,"XRAY")!=NULL);
		DWORD dwRandomColor=rnd(0,0xFFFFFF);
		double dwTonicLevel=rndDbl(0.4,0.9);
		BOOL bTonic=(strstr(szEffect,"CGLASS")!=NULL);
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};
			if(NOPRESERVE(dst,dwPreserveBrMin,dwPreserveBrMax)){
				if(bTonic){
					src[0]=limit2Byte(int(dst[0]*dwTonicLevel+GetRValue(dwRandomColor)*(1-dwTonicLevel)));
					src[1]=limit2Byte(int(dst[1]*dwTonicLevel+GetGValue(dwRandomColor)*(1-dwTonicLevel)));
					src[2]=limit2Byte(int(dst[2]*dwTonicLevel+GetBValue(dwRandomColor)*(1-dwTonicLevel)));
				}else{
					int iNewBW=RGB2BW(dst[0],dst[1],dst[2]);
					src[0]=src[1]=src[2]=bInvert?(255-iNewBW):iNewBW;
				}
				//src[0]=src[1]=src[2]=100;
			}
		}
	}
	if(strstr(szEffect,"SETCOLOR")!=NULL){
		BOOL bLevel=atol(CDataXMLSaver::GetInstringPart("EFFECT-SETCOLOR-LEVEL:(",")",szBlock));
		BOOL bMir=(bLevel>0);
		DWORD dwC=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("EFFECT-SETCOLOR:(",")",szBlock));
		DWORD dwRandBW=(RGB2BW_F(dibDst.GetPixel(rnd(iL,dibDst.m_Size.cx),rnd(0,dibDst.m_Size.cy)))+
			RGB2BW_F(dibDst.GetPixel(rnd(iL,dibDst.m_Size.cx),rnd(0,dibDst.m_Size.cy)))+
			RGB2BW_F(dibDst.GetPixel(rnd(iL,dibDst.m_Size.cx),rnd(0,dibDst.m_Size.cy)))+
			RGB2BW_F(dibDst.GetPixel(rnd(iL,dibDst.m_Size.cx),rnd(0,dibDst.m_Size.cy))))/4;
		BOOL bInvert=(bLevel==1?rnd(2,5):bLevel);
		if(bInvert==5){
			bMir=0;
		}
		DWORD dwTopLevel=rnd(5,10);
		long dwCurLevel=dwTopLevel;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};			BOOL bSet=0;
			DWORD* srcDW=(DWORD*)src;
			if(bMir){
				if(bInvert==2){
					if((--dwCurLevel)>rnd(0,1)){
						bSet=1;
					}else{
						dwCurLevel=dwTopLevel;
					}
				}
				if(bInvert==3 && RGB2BW_F(*srcDW)>=dwRandBW){
					bSet=1;
				}
				if(bInvert==4 && RGB2BW_F(*srcDW)<=dwRandBW){
					bSet=1;
				}
			}else{
				bSet=1;
			}
			if(bSet){
				src[2]=GetRValue(dwC);
				src[1]=GetGValue(dwC);
				src[0]=GetBValue(dwC);
			}
		}
	}
	if(strstr(szEffect,"MIRROR")!=NULL){
		BOOL bXMir=rnd(0,1);
		BOOL bYMir=rnd(0,1);
		memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibDst.m_Size.cx*dibDst.m_Size.cy*sizeof(dibDst.m_Bits[0]));
		if(bXMir!=0 || bYMir!=0){
			for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
				long xPos=(iSizeOrig-iSize)%iWF;
				long yPos=(iSizeOrig-iSize)/iWF;
				if(xPos<iL){
					continue;
				};
				int iNewBW=RGB2BW(dst[0],dst[1],dst[2]);
				BOOL bMixMode=NOPRESERVE(dibDst,xPos,yPos-1,dwPreserveBrMin,dwPreserveBrMax);
				dibDst.GetPixel(bXMir?dibDst.m_Size.cx-(xPos-iL):xPos,bYMir?dibDst.m_Size.cy-(yPos+iTop):yPos,src[0],src[1],src[2],0,bMixMode);
			}
		}
	}
	if(strstr(szEffect,"RPART")!=NULL){
		double dwXL=rndDbl(iL,(iWF+iL)/2-10);
		double dwXR=rndDbl(dwXL+10,iWF);
		if(dwXL>dwXR){
			double dwTmp=dwXL;
			dwXL=dwXR;
			dwXR=dwTmp;
		}
		double dX=double(dwXR-dwXL)/iWidth;
		double dY=dX*(double(iHeight)/double(iWidth));
		double dwYL=rndDbl(iTop,iHF-iHeight*dY);
		memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibDst.m_Size.cx*dibDst.m_Size.cy*sizeof(dibDst.m_Bits[0]));
		for(int j=0;j<iHeight;j++){
			for(int i=0;i<iWidth;i++){
				//BOOL bMixMode=!NOPRESERVE(dibDst,iL+i,j,dwPreserveBrMin,dwPreserveBrMax);
				DWORD dwRgb=dibSrc.GetPixel(long(dwXL+dX*double(i)),long(dwYL+dY*double(j)));
				dibDst.SetPixel(iL+i,j,dwRgb,0);
			}
		}
		memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibDst.m_Size.cx*dibDst.m_Size.cy*sizeof(dibDst.m_Bits[0]));
		szEffect="BLUR";
		sTemp.Format("EFFECT-BLUR:(%0.02f)",2/dX);
		szBlock=sTemp;
	}
	if(strstr(szEffect,"BROMI")!=NULL){
		int iCountTC=rnd(2,30);
		memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibDst.m_Size.cx*dibDst.m_Size.cy*sizeof(dibDst.m_Bits[0]));
		for(int iTC=0;iTC<iCountTC;iTC++){
			int iX1=rnd(iL,iWF-100);
			int iX2=rnd(iX1+50,iWF);
			int iY1=rnd(0,iHF-100);
			int iY2=rnd(iY1+50,iHF-1);
			double w=abs(iX2-iX1);
			double h=abs(iY2-iY1);
			int iD=min(int(h*0.1),int(w*0.1));
			BYTE R,G,B;
			int iType=rnd(0,2);
			for(int j=iY1;j<iY2;j++){
				for(int i=iX1;i<iX2;i++){
					switch (iType){
					case 0:
						dibDst.GetPixel(iX2-i,j,R,G,B,0);
						break;
					case 1:
						dibDst.GetPixel(i,iY2-j,R,G,B,0);
						break;
					case 2:
						dibDst.GetPixel(iX2-i,iY2-j,R,G,B,0);
						break;
					case 3:
						dibDst.GetPixel(int(iX1+double(iX2-iX1)*double(j/double(iY2-iY1))),int(iY1+double(iY2-iY1)*double(i/double(iX2-iX1))),R,G,B,TRUE);
						break;
					case 4:
						dibDst.GetPixel(i,j,R,G,B,0);
						R=255-R;
						G=255-G;
						B=255-B;
						break;
					}
					BOOL bLDark=((i-iX1)<iD);
					BOOL bTDark=((j-iY1)<iD);
					if(bLDark && !((i-iX1)<(j-iY1) && (i-iX1)<(iY2-j))){// && j<(iX2-i)
						bLDark=0;
					}
					if(bTDark && !((iX2-i)>(j-iY1))){// && j<(iX2-i)
						bTDark=0;
					}
					if(bLDark || bTDark){// && 
						dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=GetColorBlend(0,RGB(R,G,B),.5);
					}else if((j-iY1)>(h-iD) || (i-iX1)>(w-iD)){
						dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=GetColorBlend(RGB(R,G,B),RGB(255,255,255),.5);
					}else{
						dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=RGB(R,G,B);
					}
				}
			}
			{// Копируем обратно
				for(int j=iY1;j<iY2;j++){
					for(int i=iX1;i<iX2;i++){
						dibSrc.GetPixel(i,j,R,G,B,0);
						BOOL bMixMode=!NOPRESERVE(dibDst,i,j-1,dwPreserveBrMin,dwPreserveBrMax);
						dibDst.SetPixel(i,j,RGB(R,G,B),0);
					}
				}
			}
		}
	}
	if(strstr(szEffect,"TOFBR")!=NULL || strstr(szEffect,"TOWFBR")!=NULL){
		BOOL bWind=(strstr(szEffect,"TOWFBR")!=NULL);
		Perlin pl1(rnd(1,10),rndDbl(10,40),.5,rnd(0,100),min(iWF,iHF));
		Perlin pl2(rnd(1,10),rndDbl(10,40),.5,rnd(0,100),min(iWF,iHF));
		Perlin pl3(rnd(1,10),rndDbl(10,40),.5,rnd(0,100),min(iWF,iHF));
		CSinusoid cs1(.5);
		CSinusoid cs2(.5);
		CSinusoid cs3(.5);
		BOOL bSinuser=rnd(0,1);
		DWORD dwM=rnd(0,4);
		DWORD dwRatio=rnd(1,100);
		double lB=0,dFactor=rndDbl(1,50);
		if(bSinuser){
			dFactor=rndDbl(100,400);
		}
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};
			if(NOPRESERVE(dst,dwPreserveBrMin,dwPreserveBrMax)){
				if(dwM==0){
					DWORD dwSingle=(src[0]+src[1]+src[2]+255)>>2;
					dwSingle=dwSingle-(dwSingle%dwRatio);
					lB=double(dwSingle)/double(255);
				}else{
					lB=double(src[dwM-1]-(src[dwM-1]%dwRatio))/double(255);
				}
				if(bSinuser){
					if(bWind){
						src[0]=limit2Byte((BYTE)(double(dst[0])*(cs1.Get1D(lB*dFactor)+.5)));
						src[1]=limit2Byte((BYTE)(double(dst[1])*(cs2.Get1D(lB*dFactor)+.5)));
						src[2]=limit2Byte((BYTE)(double(dst[2])*(cs3.Get1D(lB*dFactor)+.5)));
					}else{
						lB=cs1.Get1D(lB*dFactor)+.5;
						src[0]=limit2Byte((BYTE)(double(dst[0])*(lB)));
						src[1]=limit2Byte((BYTE)(double(dst[1])*(lB)));
						src[2]=limit2Byte((BYTE)(double(dst[2])*(lB)));
					}
				}else{
					if(bWind){
						src[0]=limit2Byte((BYTE)(double(dst[0])*(pl1.Get1D(lB*dFactor)+.5)));
						src[1]=limit2Byte((BYTE)(double(dst[1])*(pl2.Get1D(lB*dFactor)+.5)));
						src[2]=limit2Byte((BYTE)(double(dst[2])*(pl3.Get1D(lB*dFactor)+.5)));
					}else{
						lB=pl1.Get1D(lB*dFactor)+.5;
						src[0]=limit2Byte((BYTE)(double(dst[0])*(lB)));
						src[1]=limit2Byte((BYTE)(double(dst[1])*(lB)));
						src[2]=limit2Byte((BYTE)(double(dst[2])*(lB)));
					}
				}
			}
		}
	}
	if(strstr(szEffect,"BLUR")!=NULL){
		DWORD dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-BLUR:(",")",szBlock))/2;
		if(dwStreng>10){
			dwStreng=10;
		}
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			if(xPos<iL){
				continue;
			};
			if(NOPRESERVE(dst,dwPreserveBrMinDst,dwPreserveBrMaxDst) || NOPRESERVE(src,dwPreserveBrMin,dwPreserveBrMax)){
				dibDst.GetRectMiddleValue(xPos-dwStreng,2*dwStreng,yPos-dwStreng,2*dwStreng,src[0],src[1],src[2]);
			}
		}
	}
	if(strstr(szEffect,"PIXELATE")!=NULL || strstr(szEffect,"SYMBO")!=NULL){
		DWORD dwSymbo=(strstr(szEffect,"SYMBO")!=NULL);
		DWORD dwSStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-SYMBO-SQR:(",")",szBlock));
		DWORD dwBGColor=atolx(CDataXMLSaver::GetInstringPart("EFFECT-SYMBO-BG:(",")",szBlock));
		CString sChars=CDataXMLSaver::GetInstringPart("EFFECT-SYMBO-CHARS:(",")",szBlock);
		if(sChars==""){
			sChars="0123456789 ";
		}
		CString sFont=CDataXMLSaver::GetInstringPart("EFFECT-SYMBO-FN:(",")",szBlock);
		sFont.Replace("[","<");
		sFont.Replace("]",">");
		bNeedOpacityProcessing=-1;
		DWORD dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-PIXL-SQR:(",")",szBlock));
		if(dwSymbo){
			dwStreng=dwSStreng;
		}
		BYTE Ra,Ga,Ba;
		DWORD dwWhite=RGB(255,255,255);
		CDib* dbChars[256]={0};
		if(dwStreng>0){
			int iXCount=iWF/dwStreng;
			int iYCount=iHF/dwStreng;
			//BYTE R=0,G=0,B=0;
			for(int i=0;i<iXCount+1;i++){
				//if(long((i+1)*dwStreng)>=iL)
				{
					for(int j=0;j<iYCount+1;j++){
						// Считаем среднее
						if(NOPRESERVE(dibDst,i*dwStreng-1,j*dwStreng-1,dwPreserveBrMin,dwPreserveBrMax)){
							dibDst.GetRectMiddleValue(i*dwStreng,dwStreng,j*dwStreng,dwStreng,Ra,Ga,Ba);
							if(dwSymbo){
								// Рисуем случайный символ
								CString c(sChars[rnd(0,sChars.GetLength()-1)]);
								CDib* pC=dbChars[BYTE(c[0])];
								if(!pC){
									pC=new CDib();
									CDC dc;
									CDesktopDC dcDesk;
									dc.CreateCompatibleDC(&dcDesk);
									CBitmap bmp;
									bmp.CreateCompatibleBitmap(&dcDesk,dwStreng,dwStreng);
									CBitmap* bmp2=dc.SelectObject(&bmp);
									dc.FillSolidRect(0,0,dwStreng,dwStreng,RGB(255,255,255));
									dc.SetTextColor(0);
									//dc.SetBkColor(RGB(255,255,255));
									dc.SetBkMode(OPAQUE);
									CFont* pFont=0;
									CFont* pOldFont=0;
									if(sFont!=""){
										pFont=CreateFontFromStr(sFont,0,0,0,0);
										pOldFont=dc.SelectObject(pFont);
									}
									dc.DrawText(c, -1, CRect(0,0,dwStreng,dwStreng), DT_SINGLELINE | DT_NOPREFIX | DT_CENTER | DT_VCENTER);
									pC->Create(dwStreng,dwStreng);
									pC->SetBitmap(&dc,&bmp);
									dc.SelectObject(bmp2);
									if(pOldFont){
										delete dc.SelectObject(pOldFont);
									}
									dbChars[BYTE(c[0])]=pC;
								}
								if(pC){
									dibDst.FillRect(i*dwStreng,j*dwStreng,dwStreng,dwStreng,GetRValue(dwBGColor),GetGValue(dwBGColor),GetBValue(dwBGColor));
									dibDst.PasteRect(pC,i*dwStreng,j*dwStreng,CDIBPASTE_TRANS|CDIBPASTE_RECOL,dwWhite,RGB(Ra,Ga,Ba));
								}
							}else{
								dibDst.FillRect(i*dwStreng,j*dwStreng,dwStreng,dwStreng,Ra,Ga,Ba);
							}
						}
					}
				}
			}
		}
		//memcpy(dibDst.m_Bits,dibSrc.m_Bits,dibSrc.m_Size.cx*dibSrc.m_Size.cy*sizeof(dibSrc.m_Bits[0]));
		//memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibSrc.m_Size.cx*dibSrc.m_Size.cy*sizeof(dibSrc.m_Bits[0]));
		for(int i=0;i<256;i++){
			if(dbChars[i]){
				delete dbChars[i];
			}
		}
	}
	if(strstr(szEffect,"CUTCOLORS")!=NULL){
		bNeedOpacityProcessing=-1;
		double dwStreng=atol(CDataXMLSaver::GetInstringPart("CUTCOLORS-PLANES:(",")",szBlock))/2;
		if(dwStreng<1){
			dwStreng=1;
		}
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};
			BOOL bMixMode=!NOPRESERVE(dst,dwPreserveBrMin,dwPreserveBrMax);
			if(bMixMode){
				dst[0]=(dst[0]+limit2Byte(int((255/dwStreng)*int(dst[0]/double(255)*dwStreng))))/2;
				dst[1]=(dst[1]+limit2Byte(int((255/dwStreng)*int(dst[1]/double(255)*dwStreng))))/2;
				dst[2]=(dst[2]+limit2Byte(int((255/dwStreng)*int(dst[2]/double(255)*dwStreng))))/2;
			}else{
				dst[0]=limit2Byte(int((255/dwStreng)*int(dst[0]/double(255)*dwStreng)));
				dst[1]=limit2Byte(int((255/dwStreng)*int(dst[1]/double(255)*dwStreng)));
				dst[2]=limit2Byte(int((255/dwStreng)*int(dst[2]/double(255)*dwStreng)));
			}
		}
	}
	if(strstr(szEffect,"EMBOSS")!=NULL || strstr(szEffect,"BUMPMAP")!=NULL){
		long bPreserveC=0;
		long dwStreng=2;
		long lForBump=1;
		if(strstr(szEffect,"BUMPMAP")==NULL){
			dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-EMBOSS:(",")",szBlock));
			bPreserveC=atol(CDataXMLSaver::GetInstringPart("EFFECT-COLOR:(",")",szBlock));
			lForBump=0;
		}
		if(dwStreng<1){
			dwStreng=1;
		}
		long dwStep=255/dwStreng/2;
		src=srcOrig;
		if(dwStreng>0){
			int iXCount=iWF/dwStreng;
			int iYCount=iHF/dwStreng;
			BYTE R=0,G=0,B=0;
			long lMinBr=1024;
			long lMaxBr=-1024;
			{// Первый проход-запоминаем
				for(int j=0;j<iHF;j++){
					for(int i=iL;i<iWF;i++){
						BYTE R=0,G=0,B=0;
						int Rt=0,Gt=0,Bt=0;
						int k=-dwStreng;
						while(k<=dwStreng){
							dibDst.GetPixel(i+k,j+k,R,G,B,TRUE);
							if(k!=0){
								Rt+=int(R*(k/dwStreng));
								Gt+=int(G*(k/dwStreng));
								Bt+=int(B*(k/dwStreng));
							}
							k++;
						}
						int iNewBr=limit2Byte(128+(Rt+Gt+Bt)/(2*dwStreng-1)/3)/10;
						if(NOPRESERVE(dibSrc,i,j,dwPreserveBrMin,dwPreserveBrMax)){
							dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]|(iNewBr<<24);
						}else{
							dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]|(0xf);
						}
						if(lMinBr>iNewBr){
							lMinBr=iNewBr;
						}
						if(lMaxBr<iNewBr){
							lMaxBr=iNewBr;
						}
					}
				}
			}
			if(bPreserveC || lForBump){
				bNeedOpacityProcessing=1;
			}
			{// Второй проход-нормируем
				lMaxBr=lMaxBr-lMinBr;
				for(int j=0;j<iHF;j++){
					for(int i=iL;i<iWF;i++){
						int iNewBr=((dibSrc.m_Bits[j*dibSrc.m_Size.cx+i])>>24)-lMinBr;
						// Пересчитываем в 0...255
						iNewBr=int(double(iNewBr)*(255/double(lMaxBr)));
						if(lForBump){
							dibDst.m_Bits[j*dibSrc.m_Size.cx+i]=RGB(iNewBr,iNewBr,iNewBr)|((255-iNewBr)<<24);
						}else{
							dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=RGB(iNewBr,iNewBr,iNewBr);
							if(bPreserveC){
								dibDst.m_Bits[j*dibSrc.m_Size.cx+i]|=(200-int(iNewBr/4)*4/6)<<24;
							}
						}
					}
				}
			}
		}
		
	}
	if(strstr(szEffect,"NOISE")!=NULL){
		bNeedOpacityProcessing=-1;
		long dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-NOISE-STR:(",")",szBlock));
		if(dwStreng>0){
			for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
				long xPos=(iSizeOrig-iSize)%iWF;
				if(xPos<iL){
					continue;
				};
				double dBR=rndDbl(0,0.8);
				if(rnd(0,100)<dwStreng){
					if(NOPRESERVE(dst,dwPreserveBrMin,dwPreserveBrMax)){
						dst[0]=limit2Byte(int(dst[0]*dBR+255*(1-dBR)));
						dst[1]=limit2Byte(int(dst[1]*dBR+255*(1-dBR)));
						dst[2]=limit2Byte(int(dst[2]*dBR+255*(1-dBR)));
					}
				}
			}
		}
	}
	if(strstr(szEffect,"EXPLODE")!=NULL){
		bNeedOpacityProcessing=-1;
		long dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-EXPL-RAD:(",")",szBlock));
		if(dwStreng>0){
			BYTE r1,b1,g1;
			BYTE r2,b2,g2;
			for(long j=0;j<iHF;j++){
				for(long i=iL;i<iWF;i++){
					// Считаем среднее
					if(NOPRESERVE(dibDst,i,j,dwPreserveBrMin,dwPreserveBrMax)){
						int iNewX=i+rnd(1,min(dwStreng,iWF-i));
						int iNewY=j+rnd(1,min(dwStreng,iHF-j));
						dibDst.GetPixel(iNewX,iNewY,r1,g1,b1);
						dibDst.GetPixel(i,j,r2,g2,b2);
						dibDst.SetPixel(iNewX,iNewY,RGB(r2,g2,b2));
						dibDst.SetPixel(i,j,RGB(r1,g1,b1));
					}
				}
			}
		}
	}
	if(strstr(szEffect,"KALEIDOSCOPE")!=NULL){
		const double PI=3.1415926;
		const long PI100000=long(PI*100000);
		const long PI2100000=long(PI100000*2);
		long lXDIFF=atol(CDataXMLSaver::GetInstringPart("XKADIFF:(",")",szBlock));
		long lYDIFF=atol(CDataXMLSaver::GetInstringPart("XKADIFF:(",")",szBlock));
		long lNoXYMove=atol(CDataXMLSaver::GetInstringPart("NOKADIFF:(",")",szBlock));
		long lDIFFR=atol(CDataXMLSaver::GetInstringPart("DIFFR:(",")",szBlock));
		long lDIFFRR=atol(CDataXMLSaver::GetInstringPart("DIFFRR:(",")",szBlock));
		long lDIFFRZ=atol(CDataXMLSaver::GetInstringPart("DIFFRZ:(",")",szBlock));
		long lInv=atol(CDataXMLSaver::GetInstringPart("DINVERT:(",")",szBlock));
		long lDRANGE=atol(CDataXMLSaver::GetInstringPart("DRANGE:(",")",szBlock));
		long lMaxDist=-1;
		long lCurDist=long(sqrt(double((iWF-iL)*(iWF-iL)+iHF*iHF)));
		if(lDRANGE==1){
			lMaxDist=long(rndDbl(lCurDist*0.3,lCurDist+lCurDist*0.1));
		}else if(lDRANGE==2){
			lMaxDist=long(rndDbl(lCurDist*0.7,lCurDist+lCurDist*0.1));
		}else if(lDRANGE==3){
			lMaxDist=long(rndDbl(lCurDist*0.1,lCurDist*0.5));
		}
		long lXOff=0;
		long lYOff=0;
		if(lDIFFR && rnd(0,100)<lDIFFRR){
			if(lNoXYMove==0){
				lXDIFF+=rnd(-20,20);
				lYDIFF+=rnd(-20,20);
				if(lXDIFF<0 || lXDIFF>99){
					lXDIFF=rnd(10,90);
				}
				if(lYDIFF<0 || lYDIFF>99){
					lYDIFF=rnd(10,90);
				}
			}
			lXOff=rnd(-(iWF-iL)/2,(iWF-iL)/2);
			lYOff=rnd(-(iHF)/2,(iHF)/2);
		}
		double dZooming=1;
		if(lDIFFRZ){
			dZooming=rndDbl(0.8,3);
		}
		int iXC=iL+int((iWF-iL)*double(lXDIFF)/100.0);
		int iYC=int((iHF)*double(lYDIFF)/100.0);
		//memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibSrc.m_Size.cx*dibSrc.m_Size.cy*sizeof(dibSrc.m_Bits[0]));
		memcpy(dibDst.m_Bits,dibSrc.m_Bits,dibSrc.m_Size.cx*dibSrc.m_Size.cy*sizeof(dibSrc.m_Bits[0]));
		long lRMin=atol(CDataXMLSaver::GetInstringPart("RMIN:(",")",szBlock));
		if(lRMin<3){
			lRMin=3;
		}
		long lRMax=atol(CDataXMLSaver::GetInstringPart("RMAX:(",")",szBlock));
		if(lRMax<3){
			lRMax=3;
		}
		if(lRMax<lRMin){
			int i=lRMin;
			lRMin=lRMax;
			lRMax=i;
		}
		double dStep=rndDbl(0,2*PI);
		double dSector=PI/double(rnd(lRMin,lRMax));
		if(dSector<0.00001){
			dSector=PI/3.0;
		}
		long dSector100000=long(dSector*100000);
		for(long j=0;j<iHF;j++){
			for(long i=iL;i<iWF;i++){
				if(NOPRESERVE(dibDst,i,j,dwPreserveBrMin,dwPreserveBrMax)){
					DWORD dwRef=0;
					double dAngle=0;
					double dLen=sqrt(double((j-iYC)*(j-iYC)+(i-iXC)*(i-iXC)));
					if(lMaxDist!=-1 && dLen>=lMaxDist){
						dibSrc.SetPixel(i,j,dibDst.GetPixel(i,j));
					}else{
						if(i-iXC>0 && abs(j-iYC)<1){
							dAngle=PI/2.0;
						}else if(i-iXC<=0 && abs(j-iYC)<1){
							dAngle=PI/2.0+PI;//PI/2.0+PI
						}else{
							dAngle=atan(double(i-iXC)/double(j-iYC));
							if(j>iYC){
								dAngle+=PI;
							}
							if(i>iXC && j<iYC){
								dAngle+=2*PI;
							}
						}
						long dAngle100000=long(dAngle*100000);
						/*long lShift=0;
						if((dAngle100000%dSector100000)<100){
							//dAngle100000=0;//dSector100000-100;
							//dAngle=0;//dSector-100/100000;
							lShift=1;
						}*/
						if((long(dAngle100000/dSector100000)%2)==0){
							dAngle=(dAngle100000%dSector100000)/100000.0;
						}else{
							dAngle=(dSector100000-dAngle100000%dSector100000)/100000.0;
						}
						dAngle+=dStep;
						BYTE R,G,B;
						dLen=dZooming*dLen;
						dibDst.GetPixel(int(iXC+lXOff+dLen*sin(dAngle)),int(iYC+lYOff+dLen*cos(dAngle)),R,G,B,lInv,0);
						COLORREF col=dwRef?RGB(255,0,0):RGB(R,G,B);
						if(lMaxDist!=-1 && dLen>(lMaxDist/2)){
							double dLenh=dLen-(lMaxDist/2);
							COLORREF colO=dibSrc.GetPixel(i,j);
							double dA=(dLenh/(lMaxDist/2))*(dLenh/(lMaxDist/2));
							col=GetColorBlend(colO,col,dA,0);
						}
						//dibSrc.SetPixel(int(iXC+dLen*sin(dAngle)),int(iYC+dLen*cos(dAngle)),col);
						dibSrc.SetPixel(i,j,col);
					}
				}
			}
		}
		//memcpy(dibSrc.m_Bits,dibDst.m_Bits,dibSrc.m_Size.cx*dibSrc.m_Size.cy*sizeof(dibSrc.m_Bits[0]));
	}
	if(strstr(szEffect,"WCOLOR")!=NULL){
		DWORD dwColor=dst[0],dwPrevColor=dst[0];
		long dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-WATCOL-RAD:(",")",szBlock));
		if(dwStreng>0){
			long dwPeaks=atol(CDataXMLSaver::GetInstringPart("EFFECT-WATCOL-PEAKS:(",")",szBlock));
			if(dwPeaks<1){
				dwPeaks=1;
			}
			for(long j=0;j<iHF;j++){
				for(long i=iL;i<iWF;i++){
					if(NOPRESERVE(dibDst,i,j,dwPreserveBrMinDst,dwPreserveBrMaxDst) || NOPRESERVE(dibSrc,i,j,dwPreserveBrMin,dwPreserveBrMax)){
						dibDst.GetRectMedianValue(i,dwStreng*dwPeaks,j,dwStreng*dwPeaks,dwColor,dwPeaks);
						dibSrc.SetPixel(i,j,dwColor);
						dwPrevColor=dwColor;
					}
				}
			}
		}
	}
	if(strstr(szEffect,"EDGEDETECT")!=NULL){
		BYTE r,b,g;
		int iR,iG,iB;
		BOOL bXOR=FALSE;
		for(long j=0;j<iHF;j++){
			for(long i=iL;i<iWF;i++){
				iR=iG=iB=0;
				if(bXOR){
					// Считаем среднее
					dibDst.GetPixel(i,j,r,g,b);
					iR^=r;
					iG^=g;
					iB^=b;
					dibDst.GetPixel(i-1,j-1,r,g,b);
					iR^=r;
					iG^=g;
					iB^=b;
					dibDst.GetPixel(i+1,j-1,r,g,b);
					iR^=r;
					iG^=g;
					iB^=b;
					dibDst.GetPixel(i+1,j-1,r,g,b);
					iR^=r;
					iG^=g;
					iB^=b;
					dibDst.GetPixel(i+1,j+1,r,g,b);
					iR^=r;
					iG^=g;
					iB^=b;
				}
				{
					// Считаем среднее
					dibDst.GetPixel(i,j,r,g,b);
					iR-=int(3.5*r);
					iG-=int(3.5*g);
					iB-=int(3.5*b);
					dibDst.GetPixel(i-1,j-1,r,g,b);
					iR+=r;
					iG+=g;
					iB+=b;
					dibDst.GetPixel(i+1,j-1,r,g,b);
					iR+=r;
					iG+=g;
					iB+=b;
					dibDst.GetPixel(i+1,j-1,r,g,b);
					iR+=r;
					iG+=g;
					iB+=b;
					dibDst.GetPixel(i+1,j+1,r,g,b);
					iR+=r;
					iG+=g;
					iB+=b;
					iR=limit2Byte(iR);
					iG=limit2Byte(iG);
					iB=limit2Byte(iB);
				}
				if(NOPRESERVE(dibDst,i,j,dwPreserveBrMinDst,dwPreserveBrMaxDst)){
					dibSrc.SetPixel(i,j,RGB(iR,iG,iB));
				}
			}
		}
	}
	if(strstr(szEffect,"SHARPEN")!=NULL){
		BYTE r,b,g;
		int iR,iG,iB;
		for(long j=0;j<iHF;j++){
			for(long i=iL;i<iWF;i++){//iL
				// Считаем среднее
				iR=iG=iB=0;
				dibDst.GetPixel(i,j,r,g,b);
				iR+=5*r;
				iG+=5*g;
				iB+=5*b;
				dibDst.GetPixel(i-1,j,r,g,b);
				iR-=r;
				iG-=g;
				iB-=b;
				dibDst.GetPixel(i,j-1,r,g,b);
				iR-=r;
				iG-=g;
				iB-=b;
				dibDst.GetPixel(i+1,j,r,g,b);
				iR-=r;
				iG-=g;
				iB-=b;
				dibDst.GetPixel(i,j+1,r,g,b);
				iR-=r;
				iG-=g;
				iB-=b;
				iR=limit2Byte(iR);
				iG=limit2Byte(iG);
				iB=limit2Byte(iB);
				if(NOPRESERVE(dibDst,i,j,dwPreserveBrMinDst,dwPreserveBrMaxDst)){
					dibSrc.SetPixel(i,j,RGB(iR,iG,iB));
				}
			}
		}
	}
	if(strstr(szEffect,"CDISPOS")!=NULL){
		DWORD dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-CDISP-STR:(",")",szBlock))/2;
		dwStreng=rnd(dwStreng/2,dwStreng);
		double d1=rndDbl(10,50)/150;
		double d2=rndDbl(50,150)/150;
		double d3=rndDbl(10,50)/150;
		double d4=rndDbl(50,150)/150;
		double d5=rndDbl(10,100)/150;
		double d4a=rndDbl(50,150)/150;
		double d5a=rndDbl(10,100)/150;
		double d4b=rndDbl(50,150)/150;
		double d5b=rndDbl(10,100)/150;
		double d2b=rndDbl(50,150)/150;
		double d6=rndDbl(10,100)+rndDbl(0,200)/3;
		double d7=rndDbl(10,100)+rndDbl(0,200)/3;
		double d8=rndDbl(0,150)/150;
		if(d8>0.5){
			d8=0;
		}
		double d9=rndDbl(0,150)/150;
		if(d9>0.5){
			d9=0;
		}
		double t=rndDbl(50,155)/15000;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			if(xPos<iL){
				continue;
			};
			t+=d5;
			BOOL bMixMode=!NOPRESERVE(dibDst,xPos,yPos-1,dwPreserveBrMin,dwPreserveBrMax);
			double dXVklad=double(xPos)/iWF*d6+1;
			double dYVklad=double(yPos)/iHF*d7+1;
			double dRes=0;
			dRes+=sin(d1*dXVklad+d2+t*d8/1000)*cos(d3*dYVklad+d4);
			dRes+=cos(d2*dYVklad+d1+t*d9/1000)*sin(d4*dXVklad+d3);
			double dRes2=0;
			dRes2+=sin(d3*dXVklad+d2+t*d8/1000)*cos(d1*dYVklad+d5);
			dRes2+=cos(d1*dYVklad+d4+t*d9/1000)*sin(d3*dXVklad+d2);
			double dRes3=0;
			dRes3+=sin(d5*dXVklad+d3+t*d8/1000)*cos(d1*dYVklad+d2);
			dRes3+=cos(d4*dYVklad+d2+t*d9/1000)*sin(d3*dXVklad+d5);
			double dRes4=0;
			dRes4+=sin(d5a*dXVklad+d3+t*d8/1000)*cos(d1*dYVklad+d2);
			dRes4+=cos(d4a*dYVklad+d2+t*d9/1000)*sin(d3*dXVklad+d5a);
			double dRes5=0;
			dRes5+=sin(d5a*dXVklad+d3+t*d8/1000)*cos(d4b*dYVklad+d2);
			dRes5+=cos(d4a*dYVklad+d2+t*d9/1000)*sin(d3*dXVklad+d5b);
			double dRes6=0;
			dRes6+=sin(d5b*dXVklad+d3+t*d8/1000)*cos(d1*dYVklad+d2b);
			dRes6+=cos(d4b*dYVklad+d2+t*d9/1000)*sin(d3*dXVklad+d5b);
			BYTE bTmp=0;
			dibDst.GetPixel(int(xPos+dRes*dwStreng),int(yPos+dRes2*dwStreng),src[0],bTmp,bTmp,TRUE,bMixMode);
			dibDst.GetPixel(int(xPos+dRes3*dwStreng),int(yPos+dRes4*dwStreng),bTmp,src[1],bTmp,TRUE,bMixMode);
			dibDst.GetPixel(int(xPos+dRes5*dwStreng),int(yPos+dRes6*dwStreng),bTmp,bTmp,src[2],TRUE,bMixMode);
		}
	}
	if(strstr(szEffect,"GENE_QUILT")!=NULL){
		int width = iWF;
		int height = iHF;
		BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
		COLORREF *outPixels = dibSrc.m_Bits;
		{// Готовим почву
			int iSeed=rnd(0,100);
			CSinusoid cs1(255);
			CSinusoid cs2(255);
			CSinusoid cs3(255);
			CSinusoid csy(rnd(10,500));
			CSinusoid csx(rnd(10,500));
			int iWaves=rnd(10,100);
			BOOL bSemi=0;//(iSeed>50?1:0);
			double t=0,tSta=0;
			int iXL=bSemi?(iWF/2):(iWF-1);
			int iYL=bSemi?(iHF/2):(iHF-1);
			for(int iy=0;iy<= iYL;iy++){
				for (int ix = iL; ix <= iXL; ix++){
					t=csx.Get1D(iWaves*ix/double(iWF))+csy.Get1D(iWaves*iy/double(iHF));
					if(iSeed>80){
						tSta+=0.01;
						t+=tSta;
					}
					DWORD dwPxpel=RGB(cs1.Get1D(t),cs2.Get1D(t),cs3.Get1D(t));
					int xp=ix;//int(csx.Get1D(t)+iL);
					int yp=iy;//int(csy.Get1D(t));
					if(bNeedSP && RGB2BW_F(dibDst.m_Bits[width*yp+xp])>bMixModeLevel){
						BYTE R,G,B;
						dibDst.GetPixel(xp,yp,R,G,B);
						dibSrc.SetPixel(xp,yp,RGB(R,G,B),0);
					}else{
						outPixels[width*yp+xp] = dwPxpel;
						if(bSemi){
							outPixels[width*(iHF-yp-1)+xp] = dwPxpel;
							outPixels[width*(iHF-yp-1)+(iWF-xp-1)] = dwPxpel;
							outPixels[width*yp+(iWF-xp-1)] = dwPxpel;
						}
					}
				}
			}
		}
	}
	if(strstr(szEffect,"GENE_CRATERS")!=NULL || strstr(szEffect,"GENE_EARTH")!=NULL){
		BOOL bEarth=(strstr(szEffect,"GENE_EARTH")!=NULL);
		int i, x, y;
		i = 0;
		{// Готовим почву
			Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,bEarth?100:500),iPerleMax);
			//Perlin pl(10,10,1,10,600);
			for(y=iHF-1;y>=0; y--){
				for (x = iL; x < iWF; x++){
					if(bEarth){
						double iAmp=pl.Get2D( y/double(iHF), x/double(iWF))*64.0+64.0;
						int iSeed=int(pl.Get2D( x/double(iWF), y/double(iHF))*iAmp+iAmp);
						dibSrc.m_Bits[dibSrc.m_Size.cx*y+x] = RGB(iSeed,iSeed,iSeed);
					}else{
						double iAmp=pl.Get2D( y/double(iHF), x/double(iWF))*500.0+500.0;
						dibSrc.m_Bits[dibSrc.m_Size.cx*y+x] = int(pl.Get2D( x/double(iWF), y/double(iHF))*iAmp+iAmp);
					}
				}
			}
		}
		if(!bEarth){
			int iMaxCraters=100;
			double depthScaling=rndDbl(3,10);
			double depthPower=rndDbl(0.7,1.7);
			long numCraters=rnd(20,iMaxCraters);
			long numSmallCraters=rnd(30,100);
			for (i = 0; i < numCraters+numSmallCraters; i++) {
				int cx = (int)rnd(iL, dibSrc.m_Size.cx-1);
				int cy = (int)rnd(0, dibSrc.m_Size.cy-1);
				int gx = 0, gy = 0;
				int amptot = 0, axelev = 0;
				int npatch = 0;
				double g = (double)sqrt(1.0 / ((double)(3.141592) * (1.0 - rndDbl(0.0, 0.9999))));
				if (i>numCraters) {//g < 3 || 
					// A very small crater
					for (y = max(0, cy - 1); y <= min(dibSrc.m_Size.cy - 1, cy + 1); y++) {
						int sx = max(0, cx - 1);
						int a = y*dibSrc.m_Size.cx+sx;
						for (x = sx; x <= min(dibSrc.m_Size.cx - 1, cx + 1); x++) {
							amptot += dibSrc.m_Bits[a++];
							npatch++;
						}
					}
					axelev = amptot / npatch;
					// Perturb the mean elevation by a small random factor.
					x = rnd(1,8);
					dibSrc.m_Bits[dibSrc.m_Size.cx*cy+cx] += int(axelev + x*depthScaling);
				} else
				{
					int iWR=rnd(10,int(iWF/double(numCraters)*2.0+1));
					int iHR=iWR;//rnd(10,300);
					double depthBias=rndDbl(0.3,1.1);
					double rollmin = rndDbl(0.6,0.95);
					gx = (int)max(iWR, g / 3);
					gy = (int)max(iHR, g / 3);
					for (y = max(0, cy - gy); y <= min(dibSrc.m_Size.cy - 1, cy + gy); y++) {
						int sx = max(0, cx - gx);
						int a = y*dibSrc.m_Size.cx+sx;
						
						for (x = sx; x <= min(dibSrc.m_Size.cx - 1, cx + gx); x++) {
							amptot += dibSrc.m_Bits[a++];
							npatch++;
						}
					}
					axelev = amptot / npatch;
					
					gy = (int)max(iHR, g);
					g = double(gy);
					gx = (int)max(iWR, g);
					
					for (y = max(0, cy - gy); y <= min(dibSrc.m_Size.cy - 1, cy + gy); y++) {
						int sx = max(0, cx - gx);
						int ax = y*dibSrc.m_Size.cx+sx;
						double dy = (cy - y) / (double) gy;
						double dysq = dy * dy;
						
						for (x = sx; x <= min(dibSrc.m_Size.cx - 1, cx + gx); x++) {
							double dx = ((cx - x) / (double) gx),
								cd = (dx * dx) + dysq,
								cd2 = cd * 2.25f,
								tcz = depthBias - (double)sqrt(fabs(1 - cd2)),
								cz = max((cd2 > 1) ? 0.0 : -10, tcz),
								roll, iroll;
							int av;
							
							cz *= (double)pow(g, depthPower);
							if (dy == 0 && dx == 0 && ((int) cz) == 0){
								cz = cz < 0 ? -1 : 1;
							}
							cz *= depthScaling;
							
							roll = (((1 / (1 - min(rollmin, cd))) /
								(1 / (1 - rollmin))) - (1 - rollmin)) / rollmin;
							iroll = 1 - roll;
							av = (int)((axelev + cz) * iroll + (dibSrc.m_Bits[ax] + cz) * roll);
							av = min(64000, av);
							dibSrc.m_Bits[ax++] = av;
						}
					}
				}
			}
		}
		BOOL bMixModeLevel=(bRandom01==4)?rnd(dwPreserveBrMin,dwPreserveBrMax):rnd(50,250);
		double ImageGamma = 0.5f;
		double dgamma = 1.0f;
		int slopemin = -52, slopemax = 52;
		i = max((slopemax - slopemin) + 1, 1);
		double* slopemap = new double[i];
		double dMinValue=0;
		double dMaxValue=0;
		int iCounter=0;
		for (i = slopemin; i <= slopemax; i++) {
			slopemap[i - slopemin] = i > 0 ?
				(0.5f + 0.5f *
				(double)pow(sin((3.141592/2) * i / slopemax),
				dgamma * ImageGamma)) :
			(0.5f - 127.0f *
				(double)pow(sin((3.141592/2) * i / slopemin),
				dgamma * ImageGamma));
		}
		DWORD btPal[256]={0};
		DWORD btPal2[256]={0};
		DWORD dwRandomColor=RGB(rnd(0,120),rnd(0,120),rnd(0,120));
		for(i=0;i<256;i++){
			btPal[i]=GetColorBlend(dwRandomColor,RGB(255,255,255),i/double(256));
		}
		int iWaterCut=rnd(10,80);
		int iSnowCut=rnd(81,160);
		if(bEarth){
			for(i=0;i<256;i++){
				if(i<iWaterCut){
					double d=double(i)/double(iWaterCut);
					btPal2[i]=GetColorBlend(
						RGB(0,255-rnd(0,30),0),
						RGB(255-rnd(0,30),0,0),
						d,0);
				}else if(i>iSnowCut){
					double d=double(i-iSnowCut)/double(255-iSnowCut);
					btPal2[i]=GetColorBlend(
						RGB(255-rnd(0,30),255-rnd(0,30),255-rnd(0,30)),
						RGB(150-rnd(0,30),150-rnd(0,30),150-rnd(0,30)),
						d,0);
				}else{
					double d=double(i-iWaterCut)/double(iSnowCut-iWaterCut);
					btPal2[i]=GetColorBlend(
						RGB(150-rnd(0,30),150-rnd(0,30),150-rnd(0,30)),
						RGB(0,255-rnd(0,30),0),
						d,0);
				}
			}
		}
		int index = 0;
		DWORD dwRandom3D=atol(CDataXMLSaver::GetInstringPart("CRATERS-TYPE:(",")",szBlock));
		if(bEarth){
			dwRandom3D=atol(CDataXMLSaver::GetInstringPart("EARTH-TYPE:(",")",szBlock));
		}
		if(dwRandom3D>2){
			dwRandom3D=rnd(0,2);
		}
		long lSize=dibSrc.m_Size.cx*dibSrc.m_Size.cy*sizeof(dibSrc.m_Bits[0]);
		DWORD* dwTemp=new DWORD[lSize];
		memcpy(dwTemp,dibSrc.m_Bits,lSize);
		long dwMiddler=0,dwZeroer=0;
		for(y=iHF-1;y>=0;y--){
			int last = dwTemp[index];//dibSrc
			for (x = iL; x < iWF; x++){
				DWORD t = dwTemp[dibSrc.m_Size.cx*y+x];//dibSrc
				BYTE bLevelByte=GetRValue(t);//RGB2BW_F(t);
				long j = t-last;
				j = min(max(slopemin, j), slopemax);
				double iLevel=slopemap[j-slopemin];
				int iByte=limit2Byte(int(255*(iLevel+127)/256));
				GET_CHECK_BYTE(bLevelByte,dibDst.m_Bits[dibDst.m_Size.cx*y+x]);
				int iPH=(bLevelByte-dwZeroer)/10;
				if(bNeedSP && iCheckByte<bMixModeLevel){
					BYTE R,G,B;
					dibDst.GetPixel(x,y,R,G,B);
					dibSrc.SetPixel(x,y,RGB(R,G,B),0);
					if(dwZeroer==0){
						dwZeroer=bLevelByte;
					}
				}else{//btPal[iByte]
					DWORD colToSet=bEarth?(GetColorBlend(0,btPal2[bLevelByte],limit2Byte(250-GetRValue(btPal[iByte]))/double(255),0)):btPal[iByte];
					if(dwRandom3D){
						iPH=(iPH+dwMiddler)/2;
						dwMiddler=iPH;
						int iPHi;
						for(iPHi=0;iPHi<=iPH && y+iPHi<iHF;iPHi++){
							dibSrc.m_Bits[dibSrc.m_Size.cx*(y+iPHi)+x] = colToSet;
						}
						if(iPH>0 && dwRandom3D==2){
							dibSrc.DarkenPixel(x,(y+iPHi),0.9);
						}
					}else{
						dibSrc.m_Bits[dibSrc.m_Size.cx*y+x] = colToSet;
					}
				}
				last = t;
				index++;
			}
		} 
		delete[] slopemap;
		delete[] dwTemp;
	}
	if(strstr(szEffect,"GENE_WOOD")!=NULL){
		double dShift1=rndDbl(0.7,1.3);
		double dShift2=rndDbl(0.7,1.3);
		DWORD btPal[256]={0};
		long lDef=rnd(1,10);
		int i=0;
		{
			DWORD dwFr=RGB(rnd(200,255),rnd(200,255),rnd(200,255));
			DWORD dwTo=RGB(rnd(0,100),rnd(0,100),rnd(0,100));
			Perlin pl2(rnd(1,4),rndDbl(0.5,5),1,rnd(0,500),100);
			for(i=0;i<256;i++){
				double dI=i/double(256);
				btPal[i]=GetColorBlend(dwFr,dwTo,dI+pl2.Get1D(dI));
			}
		}
		Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
		BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
		for(long j=iHF-2;j>=0;j--){
			for(long i=iL;i<iWF-1;i++){
				double d1=(pl.Get2D( i/double(iWF)*dShift1, j/double(iHF)*dShift2))*lDef;
				d1=d1-int(d1);
				d1+=(1-d1)*pl.Get2D( j/double(iHF)*50, i/double(iWF))/3;
				BYTE bt=limit2Byte((BYTE)ceil(d1*255));
				GET_CHECK_BYTE(bt,dibDst.m_Bits[dibDst.m_Size.cx*j+i]);
				if(bNeedSP && iCheckByte<bMixModeLevel){
					BYTE R,G,B;
					dibDst.GetPixel(i,j,R,G,B);
					dibSrc.SetPixel(i,j,RGB(R,G,B),0);
				}else{
					dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=btPal[bt];
					//dibSrc.m_Bits[j*dibSrc.m_Size.cx+i+1]=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
					//dibSrc.m_Bits[(j+1)*dibSrc.m_Size.cx+i+1]=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
				}
			}
		}
	}
	if(strstr(szEffect,"GENE_PERLIN")!=NULL){
		DWORD dwPalette=atol(CDataXMLSaver::GetInstringPart("PERLIN-PALETTE:(",")",szBlock));
		DWORD dwMethod=atol(CDataXMLSaver::GetInstringPart("PERLIN-METHOD:(",")",szBlock));
		if(dwMethod==0){
			dwMethod=rnd(1,2);
		}
		DWORD dwShadow=rnd(0,0);
		double dShift1=rndDbl(0,6.3);
		double dShift2=rndDbl(0,6.3);
		double dShift3=rndDbl(0,6.3);
		BYTE btPalR[256]={0};
		BYTE btPalG[256]={0};
		BYTE btPalB[256]={0};
		double dWaves=rndDbl(.5,5);
		int i=0;
		for(i=0;i<256;i++){
			switch(dwPalette){
			case 1:
				btPalR[i]=i;
				break;
			case 2:
				btPalG[i]=i;
				break;
			case 3:
				btPalB[i]=i;
				break;
			case 4:
				btPalR[i]=i;
				btPalG[i]=i;
				btPalB[i]=i;
				break;
			default:
				{
					btPalR[i]=(BYTE)(fabs(255*sin(i/255.0*(3.14/dWaves)+dShift1)));
					btPalG[i]=(BYTE)(fabs(255*sin(i/255.0*(3.14/dWaves)+dShift2)));
					btPalB[i]=(BYTE)(fabs(255*sin(i/255.0*(3.14/dWaves)+dShift3)));
				}
				break;
			}
		}
		BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
		Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
		for(long j=iHF-1;j>=0;j--){
			for(long i=iL;i<iWF;i++){
				double d1=pl.Get2D( i/double(iWF), j/double(iHF));
				BYTE bt=BYTE(d1*127+128);
				GET_CHECK_BYTE(bt,dibDst.m_Bits[dibDst.m_Size.cx*j+i]);
				if(bNeedSP && iCheckByte<bMixModeLevel){
					BYTE R,G,B;
					dibDst.GetPixel(i,j,R,G,B);
					dibSrc.SetPixel(i,j,RGB(R,G,B),0);
				}else{
					if(dwMethod==2){
						dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
					}else{
						int iPH=(bt-bMixModeLevel)/5;
						int iPHi;
						for(iPHi=0;iPHi<iPH && iPHi+j<iHF;iPHi++){
							dibSrc.m_Bits[(j+iPHi)*dibSrc.m_Size.cx+i]=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
						}
						if(dwShadow){
							dibSrc.DarkenPixel(i,(j+iPHi),0.9);
						}
					}
				}
			}
		}
	}
	if(strstr(szEffect,"GENE_CLOUD")!=NULL){
		long j=0,i=0;
		BYTE btPal[255]={0};
		int iMaxL=rnd(3,10);
		for(int iLayer=0;iLayer<iMaxL;iLayer++){// Задник
			BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
			if(!bNeedSP){
				bMixModeLevel=255;
			}
			int iRB=10;
			int iGB=30;
			int iBB=rnd(0,255);
			for(i=0;i<bMixModeLevel;i++){
				btPal[i]=(BYTE)GetColorBlend(RGB(iGB,iBB,iRB),RGB(255,255,255),i/double(bMixModeLevel));
			}
			Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
			for(j=iHF-1;j>=0;j--){
				for(long i=iL;i<iWF;i++){
					double d1=pl.Get2D( i/double(iWF), j/double(iHF));
					BYTE bt=BYTE(d1*127+128);
					GET_CHECK_BYTE(bt,dibDst.m_Bits[dibDst.m_Size.cx*j+i]);
					if(iLayer==0 && bNeedSP && iCheckByte>bMixModeLevel){
						BYTE R,G,B;
						dibDst.GetPixel(i,j,R,G,B);
						dibSrc.SetPixel(i,j,RGB(R,G,B),0);
					}else{
						if(iLayer==0){
							dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=btPal[bt];
						}else{
							dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=GetColorBlend(dibSrc.m_Bits[j*dibSrc.m_Size.cx+i],btPal[bt],1-bt/255.0);
						}
					}
				}
			}
		}
	}
	if(strstr(szEffect,"GENE_2MIX")!=NULL){
		long j=0,i=0;
		BYTE btPalR[255]={0};
		BYTE btPalG[255]={0};
		BYTE btPalB[255]={0};
		{// Задник
			BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
			if(!bNeedSP){
				bMixModeLevel=255;
			}
			int iRB=0,iRE=rnd(0,255);
			int iGB=0,iGE=rnd(0,255);
			int iBB=255,iBE=rnd(0,255);
			for(i=0;i<bMixModeLevel;i++){
				btPalB[i]=(BYTE)GetColorBlend(iRB,iRE,i/double(bMixModeLevel));
				btPalG[i]=(BYTE)GetColorBlend(iGB,iGE,i/double(bMixModeLevel));
				btPalR[i]=(BYTE)GetColorBlend(iBB,iBE,i/double(bMixModeLevel));
			}
			Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
			for(j=iHF-1;j>=0;j--){
				for(long i=iL;i<iWF;i++){
					double d1=pl.Get2D( i/double(iWF), j/double(iHF));
					BYTE bt=BYTE(d1*127+128);
					GET_CHECK_BYTE(bt,dibDst.m_Bits[dibDst.m_Size.cx*j+i]);
					if(bNeedSP && iCheckByte<bMixModeLevel){
						BYTE R,G,B;
						dibDst.GetPixel(i,j,R,G,B);
						dibSrc.SetPixel(i,j,RGB(R,G,B),0);
					}else{
						dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
					}
				}
			}
		}
		{// Средник
			BOOL bMixModeLevel=rnd(150,220);
			int iRB=0,iRE=rnd(0,255);
			int iGB=255,iGE=rnd(0,255);
			int iBB=0,iBE=rnd(0,255);
			for(i=0;i<bMixModeLevel;i++){
				btPalB[i]=(BYTE)GetColorBlend(iRB,iRE,i/double(bMixModeLevel));
				btPalG[i]=(BYTE)GetColorBlend(iGB,iGE,i/double(bMixModeLevel));
				btPalR[i]=(BYTE)GetColorBlend(iBB,iBE,i/double(bMixModeLevel));
			}
			Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
			for(j=iHF-1;j>=0;j--){
				for(long i=iL;i<iWF;i++){
					double d1=pl.Get2D( i/double(iWF), j/double(iHF));
					BYTE bt=BYTE(d1*127+128);
					if(bt<bMixModeLevel){
						dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=GetColorBlend(dibSrc.m_Bits[j*dibSrc.m_Size.cx+i],RGB(btPalR[bt],btPalG[bt],btPalB[bt]),bt/255.0);
					}
				}
			}
		}
		{// Передник
			BOOL bMixModeLevel=rnd(50,150);
			int iRB=rnd(0,255),iRE=rnd(0,255);
			int iGB=rnd(0,255),iGE=rnd(0,255);
			int iBB=rnd(0,255),iBE=rnd(0,255);
			for(i=0;i<bMixModeLevel;i++){
				btPalB[i]=(BYTE)GetColorBlend(iRB,iRE,i/double(bMixModeLevel));
				btPalG[i]=(BYTE)GetColorBlend(iGB,iGE,i/double(bMixModeLevel));
				btPalR[i]=(BYTE)GetColorBlend(iBB,iBE,i/double(bMixModeLevel));
			}
			Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
			for(j=iHF-1;j>=0;j--){
				for(long i=iL;i<iWF;i++){
					double d1=pl.Get2D( i/double(iWF), j/double(iHF));
					BYTE bt=BYTE(d1*127+128);
					if(bt>bMixModeLevel){
						dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=GetColorBlend(dibSrc.m_Bits[j*dibSrc.m_Size.cx+i],RGB(btPalR[bt],btPalG[bt],btPalB[bt]),bt/255.0);
					}
				}
			}
		}
	}
	// сделать 3Д отдельно
	if(strstr(szEffect,"GENE_MARBLE")!=NULL){
		BYTE btPalR[256]={0};
		BYTE btPalG[256]={0};
		BYTE btPalB[256]={0};
		int i=0;
		for(i=0;i<256;i++){
			btPalB[i]=10*int(25*double(i)/255.0);
			btPalG[i]=10*int(25*double(i)/255.0);
			btPalR[i]=10*int(25*double(i)/255.0);
		}
		Perlin pl(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
		BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
		BOOL bStrMode=rnd(0,1);
		BOOL dwShadow=rnd(0,1);
		for(long j=0;j<iHF;j++){
			for(long i=iL;i<iWF;i++){
				double d1=0;
				if(bStrMode){
					d1=cos(3.14*i/double(iWF)*100.0+30*pl.Get2D( i/double(iWF), j/double(iHF)));
				}else{
					d1=cos(3.14*j/double(iHeight)*100.0+30*pl.Get2D( i/double(iWF), j/double(iHF)));
				}
				/*
#ifndef _DEBUG
				DWORD dwAdd=0;
				double dx=double(i)/double(iWF);
				double dy=double(j)/double(iHF);
				if(j<iHF/2){
					dx=floor(dx*700.0f)/700.0f;
					dy=floor(dy*700.0f)/700.0f;
					dwAdd=1;
				}
				d1=30*pl.Get2D(dx,dy);
#endif*/
				BYTE bt=limit2Byte(BYTE(d1*127+127));
				GET_CHECK_BYTE(bt,dibDst.m_Bits[dibDst.m_Size.cx*j+i]);
				if(bNeedSP && iCheckByte<bMixModeLevel){
					BYTE R,G,B;
					dibDst.GetPixel(i,j,R,G,B);
					dibSrc.SetPixel(i,j,RGB(R,G,B),0);
				}else{
					DWORD dwRgb=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
					/*if(dwAdd){
						((BYTE*)&dwRgb)[0]=0xAA;
					}*/
					dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=dwRgb;
				}
			}
		}
	}
	if(strstr(szEffect,"GENE_3DMAZE")!=NULL){
		//memcpy(dibDst.m_Bits,dibSrc.m_Bits,dibDst.m_Size.cx*dibDst.m_Size.cy*sizeof(dibDst.m_Bits[0]));
		BYTE btPalR[256]={0};
		BYTE btPalG[256]={0};
		BYTE btPalB[256]={0};
		int i=0;
		for(i=0;i<256;i++){
			btPalB[i]=10*int(25*double(i)/255.0);
			btPalG[i]=10*int(25*double(i)/255.0);
			btPalR[i]=10*int(25*double(i)/255.0);
		}
		BYTE array[100][100]={0};
		for(i=1;i<99;i=i+2){
			for(int j=1;j<99;j=j+2){
				array[i][j]=255;
				int ir=rnd(1,100);
				if(ir>55){
					array[i+1][j]=255;
				}else if(ir>10){
					array[i][j+1]=255;
				}
				if(rnd(0,50)>49 && array[i-1][j]==0 && array[i][j-1]==0){
					if(rnd(1,100)>50){
						array[i-1][j]=255;
					}else{
						array[i][j-1]=255;
					}
				}
			}
		}
		BOOL bMixModeLevel=rnd(dwPreserveBrMin,dwPreserveBrMax);
		BOOL bStrMode=rnd(0,1);
		for(long j=iHF-1;j>=0;j--){
			for(long i=iL;i<iWF;i++){
				BYTE bt=array[int(i/double(iWF)*99)+1][int(j/double(iHF)*99)+1];
				GET_CHECK_BYTE(bt,dibDst.m_Bits[dibDst.m_Size.cx*j+i]);
				if(bNeedSP && iCheckByte<bMixModeLevel){
					BYTE R,G,B;
					dibDst.GetPixel(i,j,R,G,B);
					dibSrc.SetPixel(i,j,RGB(R,G,B),0);
				}else{
					DWORD dwRgb=RGB(btPalR[bt],btPalG[bt],btPalB[bt]);
					int iPH=bt/20;
					dibSrc.m_Bits[j*dibSrc.m_Size.cx+i]=dwRgb;
					int iPHi;
					for(iPHi=0;iPHi<iPH && iPHi+j<iHF;iPHi++){
						dibSrc.m_Bits[(j+iPHi)*dibSrc.m_Size.cx+i]=dwRgb;
						dibSrc.DarkenPixel(i,(j+iPHi),iPHi/double(iPH));
					}
					dibSrc.DarkenPixel(i,(j+iPHi),0.9);
				}
			}
		}
	}
	if(strstr(szEffect,"GENE_FRACTAL")!=NULL){
		DWORD dwPalette=atol(CDataXMLSaver::GetInstringPart("FRACTAL-PALETTE:(",")",szBlock));
		BYTE btPalR[256]={0};
		BYTE btPalG[256]={0};
		BYTE btPalB[256]={0};
		double dShift1=rndDbl(0,6.3);
		double dShift2=rndDbl(0,6.3);
		double dShift3=rndDbl(0,6.3);
		double dFraction=rndDbl(100,255);
		int i,p,q;
		for(i=0;i<256;i++){
			switch(dwPalette){
			case 1:
				btPalR[i]=i;
				break;
			case 2:
				btPalG[i]=i;
				break;
			case 3:
				btPalB[i]=i;
				break;
			case 4:
				btPalR[i]=i;
				btPalG[i]=i;
				btPalB[i]=i;
				break;
			default:
				{
					btPalR[i]=(BYTE)fabs(255*sin(i/dFraction*(3.14/2)+dShift1));
					btPalG[i]=(BYTE)fabs(255*sin(i/dFraction*(3.14/2)+dShift2));
					btPalB[i]=(BYTE)fabs(255*sin(i/dFraction*(3.14/2)+dShift3));
					btPalR[i]=abs(btPalR[i]-127)+20;
					btPalG[i]=abs(btPalG[i]-127)+20;
					btPalB[i]=abs(btPalB[i]-127)+20;
				}
				break;
			}
		}
		BOOL bMixMode=0;
		double x,ox,y,a,b,c,d,e,f,g,h,rt1,it1,rb1,ib1,b1,gg,ss,uu;
		double j,k,l,m,n,o,b2,rt2,it2,rb2,ib2;
		double t = rndDbl(0,0.99);//0.5;// vary t (0<t<1) for different homotopys
		double aa,bb,cc,dd;
		double MMax_p = iWF;
		double MMax_q = iHF;
		double R = rndDbl(0.001,5);//0.001;
		double RR = rndDbl(100,300);//200;
		int its = 100;//160;
		a = ((double)rndDbl(0,550)/100.0)-1.0;//1.7;
		b = ((double)rndDbl(0,550)/100.0)-1.0;//0;//a from -1 to 4.5 for mag1 and mag2
		//aa = -6.0,bb = -4.5,cc = 6.0,dd = 4.5;
		//aa = -12.0,bb = -9.0,cc = 12.0,dd = 9.0;
		aa=-12.0+(t*6.0);cc=-aa;
		bb=-9.0+(t*4.5);dd=-bb;
		int iType=rnd(0,1);
		int iType2=rnd(0,1);
		int iRnd1=rnd(2,4);
		int iRnd2=rnd(2,4);
		int iRnd3=rnd(2,4);
		int iRnd4=rnd(2,4);
		int iRnd5=rnd(2,4);
		double iRnd6=rndDbl(0,.1);
		for (p=iL; p<MMax_p; p++)
		{
			ox = aa + ((cc-aa)*(p/MMax_p));
			for (q=0; q<MMax_q; q++)
			{
				bMixMode=0;//!NOPRESERVE(dibDst,p,q,dwPreserveBrMin,dwPreserveBrMax);
				x=ox;
				y = bb + ((dd-bb)*(q/MMax_q));
				for (i=1; i<its; i++)
				{
					if(iType){
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
					}else{
						c=x*x-y*y;d=2*x*y;    //start the math and the iterations.
						e=a*a-b*b;f=2*a*b;
						g=c*c-d*d;h=2*c*d;
						rt1=g+1+e+2*(c*a-b*d-c-a);
						it1= h+f+2*(c*b+d*a-d-b);
						rb1= e+4+4*(c+x*a-y*b-2*x-a);
						ib1= f+4*(d+x*b+a*y-2*y-b);
						b1=1/(rb1*rb1+ib1*ib1);
						j=x*c-y*d;k=x*d+y*c;
						l=j+2+e+iRnd1*(x*a-y*b-x-a);
						m= k+f+iRnd2*(x*b+a*y-y-b);
						n= e+iRnd4+iRnd3*(c+x*a-y*b-2*x-a);
						o= f+iRnd5*(d+x*b+a*y-2*y-b);
						rt2=l*l-m*m;it2=2*l*m;
						rb2=n*n-o*o;ib2=2*n*o;
						b2=1/(rb2*rb2+ib2*ib2);
						x = t*(rt1*rb1+it1*ib1)*b1+(1-t)*(rt2*rb2+it2*ib2)*b2; //new x
						y = t*(it1*rb1-rt1*ib1)*b1+(1-t)*(it2*rb2-rt2*ib2)*b2; //new y					
					}
					gg=x*x+y*y;ss=gg+1;uu=ss-2*x; //test distance from roots
					if(iType2){
						if(int(x+y)%2==0){
							t+=iRnd6;
						}else{
							t-=iRnd6;
						}
					}
					if (gg>RR) {
						BYTE bt=BYTE(int(uu+gg)%255);
						dibSrc.SetPixel(p,q,RGB(btPalR[bt],btPalG[bt],btPalB[bt]),bMixMode);//i+80
						break;
					}
					if (uu<R) {
						if(bNeedSP){
							BYTE R,G,B;
							dibDst.GetPixel(p,q,R,G,B);
							dibSrc.SetPixel(p,q,RGB(R,G,B),0);
						}else{
							BYTE bt=255-BYTE(int(uu+gg)%255);
							dibSrc.SetPixel(p,q,RGB(btPalR[bt],btPalG[bt],btPalB[bt]),bMixMode);//i+7
						}
						break;
					}
					
				}/*next i*/
				if(i==its){
					BYTE bt=BYTE(int(uu+gg+30)%255);
					dibSrc.SetPixel(p,q,RGB(0,0,0),bMixMode);
				}
			}/*next q*/
		}/*next p*/
	}
	if(strstr(szEffect,"GENE_LANDS")!=NULL){
		//???
		iSize=0;
		dst=0;//dstOrig+iSizeOrig*4-4;
		src=0;//srcOrig+iSizeOrig*4-4;
		BOOL bMixMode=0;
		double 	d1=rndDbl(1,3);
		double 	d2=d1-rndDbl(1,3);
		double 	d3=rndDbl(1,3);
		double 	d4=d3-rndDbl(1,3);
		double 	d5=rndDbl(1,3);
		double 	d6=d5-rndDbl(1,3);
		double 	dXt=rndDbl(1,30);
		double 	dYt=rndDbl(1,30);
		double 	dTt=rndDbl(100,1000);
		double 	dt=rndDbl(0,1);
		double  t=rndDbl(0.0001,0.2);
		int		iX1=rnd(0,iWF);
		int		iY1=rnd(0,iHF);
		int		iX2=rnd(0,iWF);
		int		iY2=rnd(0,iHF);
		int		iX3=rnd(0,iWF);
		int		iY3=rnd(0,iHF);
		int		iX4=rnd(0,iWF);
		int		iY4=rnd(0,iHF);
		CSinusoid cs1(255);
		CSinusoid cs2(255);
		CSinusoid cs3(255);
		double	dNormoLen=DISTANCE(0,0,iWF,iHF)+rnd(-300,300);
		double dDel=rndDbl(.5,5);
		while ((iSize++)<iSizeOrig){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			t+=dt;
			bMixMode=!NOPRESERVE(dibDst,xPos,yPos-1,dwPreserveBrMin,dwPreserveBrMax);
			if(!bMixMode){
				double dXVklad=0;
				double dYVklad=0;
				double dRes=0;
				double dRes2=0;
				double dRes3=0;
				//-------------
				dXVklad=DISTANCE(xPos,yPos,iX1,iY1)/dNormoLen*(dXt);
				dYVklad=DISTANCE(xPos,yPos,iX2,iY2)/dNormoLen*(dYt);
				dRes+=sin(d1*dXVklad+d2)*cos(d3*dXVklad+d4);
				dRes+=cos(d2*dYVklad+d1)*sin(d4*dYVklad+d3);
				dRes2+=sin(d3*dXVklad+d2)*cos(d3*dXVklad+d2);
				dRes2+=cos(d2*dYVklad+d4)*sin(d4*dYVklad+d1);
				dRes3+=cos(d4*dXVklad+d2)*sin(d3*dXVklad+d5);
				dRes3+=sin(d2*dYVklad+d1)*sin(d3*dYVklad+d1);
				dXVklad=DISTANCE(xPos,yPos,iX3,iY3)/dNormoLen*(dXt+t/dTt);
				dYVklad=DISTANCE(xPos,yPos,int(iX4+t/dTt),iY4)/dNormoLen*(dYt);
				dRes+=cos(d1*dXVklad+d2)*cos(d3*dXVklad+d4);
				dRes+=sin(d2*dYVklad+d1)*sin(d4*dYVklad+d3);
				dRes2+=sin(d3*dXVklad+d2)*cos(d3*dXVklad+d2);
				dRes2+=cos(d2*dYVklad+d4)*sin(d4*dYVklad+d1);
				dRes3+=cos(d4*dXVklad+d2)*sin(d3*dXVklad+d5);
				dRes3+=sin(d2*dYVklad+d1)*sin(d3*dYVklad+d1);
				double dNorma=fabs(dRes)*dDel;
				double dNorma2=fabs(dRes2)*dDel;
				double dNorma3=fabs(dRes3)*dDel;
				DWORD dwColor=RGB(cs1.Get1D(dNorma),cs2.Get1D(dNorma2),cs3.Get1D(dNorma3));
				dibSrc.SetPixel(xPos,yPos,dwColor);
			}
		}
	}
	if(strstr(szEffect,"WATER")!=NULL || strstr(szEffect,"3DLAND")!=NULL || strstr(szEffect,"SQGRAN")!=NULL){
		//???
		DWORD dwGran=(strstr(szEffect,"SQGRAN")!=NULL);
		DWORD dw3D=(strstr(szEffect,"3DLAND")!=NULL);
		DWORD dwStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-WAVE-STR:(",")",szBlock))/2;
		DWORD dwLStreng=atol(CDataXMLSaver::GetInstringPart("EFFECT-LAND-STR:(",")",szBlock));
		DWORD dwShadow=atol(CDataXMLSaver::GetInstringPart("EFFECT-LAND-SHD:(",")",szBlock));
		DWORD dwStrengGran=atol(CDataXMLSaver::GetInstringPart("EFFECT-GRAN-STR:(",")",szBlock));
		DWORD dw3dMet=atol(CDataXMLSaver::GetInstringPart("EFFECT-LAND-MET:(",")",szBlock));
		if(dw3dMet==0){
			dw3dMet=rnd(1,2);
		}
		if(dwStrengGran==0){
			dwStrengGran=30;
		}
		Perlin* pl=0;
		if(dw3dMet==2){
			pl=new Perlin(rnd(1,iPerleMaxO),rndDbl(0.1,iPerleMaxF),1,rnd(0,500),iPerleMax);
		}
		dwStreng=rnd(dwStreng/2,dwStreng);
		dst=0;//dstOrig+iSizeOrig*4;
		src=0;//srcOrig+iSizeOrig*4;
		iSize=0;
		BOOL bMixMode=0;
		double 	d1=rndDbl(1,3);
		double 	d2=d1-rndDbl(1,3);
		double 	d3=rndDbl(1,3);
		double 	d4=d3-rndDbl(1,3);
		double 	d5=rndDbl(1,3);
		double 	d6=d5-rndDbl(1,3);
		double 	dXt=rndDbl(1,30);
		double 	dYt=rndDbl(1,30);
		double 	dTt=rndDbl(100,1000);
		double 	dt=rndDbl(0,1);
		double  t=rndDbl(0.0001,0.2);
		int		iX1=rnd(0,iWF);
		int		iY1=rnd(0,iHF);
		int		iX2=rnd(0,iWF);
		int		iY2=rnd(0,iHF);
		int		iX3=rnd(0,iWF);
		int		iY3=rnd(0,iHF);
		int		iX4=rnd(0,iWF);
		int		iY4=rnd(0,iHF);
		double	dNormoLen=DISTANCE(0,0,iWF,iHF)+rnd(-300,300);
		BYTE R,G,B;
		while ((iSize++)<iSizeOrig){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			t+=dt;
			if(!dw3D){
				bMixMode=!NOPRESERVE(dibDst,xPos,yPos-1,dwPreserveBrMin,dwPreserveBrMax);
			}
			double dXVklad=0;
			double dYVklad=0;
			if(dw3D){
				if(dw3dMet==1){
					dXVklad=DISTANCE(xPos,yPos,iX1,iY1)/dNormoLen*(dXt);
					dYVklad=DISTANCE(xPos,yPos,iX2,iY2)/dNormoLen*(dYt);
				}
			}else{
				dXVklad=double(yPos)/iHF*dYt;
				dYVklad=double(xPos)/iWF*dXt;
			}
			//-------------
			double dRes=0;
			dRes+=sin(d1*dXVklad+d2)*cos(d3*dXVklad+d4);
			dRes+=cos(d2*dYVklad+d1)*sin(d4*dYVklad+d3);
			if(dw3D){
				dibDst.GetPixel(xPos,yPos,R,G,B,TRUE,bMixMode);
				DWORD dwColor=RGB(R,G,B);
				double dNorma=0;
				if(dw3dMet==1){
					dXVklad=DISTANCE(xPos,yPos,iX3,iY3)/dNormoLen*(dXt+t/dTt);
					dYVklad=DISTANCE(xPos,yPos,int(iX4+t/dTt),iY4)/dNormoLen*(dYt);
					dRes+=cos(d1*dXVklad+d2)*cos(d3*dXVklad+d4);
					dRes+=sin(d2*dYVklad+d1)*sin(d4*dYVklad+d3);
					dNorma=fabs(dRes)/4;
				}else{
					dNorma=(pl->Get2D(xPos/double(iWF),yPos/double(iHF))+1)/2;
				}
				int iYPosNew=int(yPos+dNorma*dwLStreng);
				if(dwShadow){
					double dNormaMin=dNorma*0.6;
					dwColor=RGB(int(GetRValue(dwColor)*(1-dNormaMin)+255*dNormaMin),int(GetGValue(dwColor)*(1-dNormaMin)+255*dNormaMin),int(GetBValue(dwColor)*(1-dNormaMin)+255*dNormaMin));
				}
				int kk;
				for(kk=yPos;kk<iYPosNew;kk++){
					dibSrc.SetPixel(xPos,kk,dwColor,bMixMode);
				}
				if(dwShadow){
					dibSrc.DarkenPixel(xPos+1,kk+1,0.9);
				}
			}else{
				double dRes2=0;
				dRes2+=sin(d3*dXVklad+d2)*cos(d1*dXVklad+d4);//*dTVklad
				dRes2+=cos(d1*dYVklad+d4)*sin(d3*dYVklad+d2);
				//double dTVklad2=(yPos*dYt+t/10)/100;
				//dRes2+=cos(d1*dTVklad+d4)*cos(d3*dTVklad2+d2);
				if(dwGran){
					if((yPos%dwStrengGran)<(xPos%dwStrengGran)){
						dibDst.GetPixel(int(xPos+dRes*dwStreng),int(yPos+dRes2*dwStreng),R,G,B,TRUE,bMixMode);
					}else{
						dibDst.GetPixel(int(xPos+dRes2*dwStreng),int(yPos+dRes*dwStreng),R,G,B,TRUE,bMixMode);
					}
				}else{
					dibDst.GetPixel(int(xPos+dRes*dwStreng),int(yPos+dRes2*dwStreng),R,G,B,TRUE,bMixMode);
					// Уменьшаем яркость...
					if(dRes>0){
						double d01=1;
						d01=1-dRes/9;
						R=int(d01*R);
						G=int(d01*G);
						B=int(d01*B);
					}
				}
				dibSrc.SetPixel(xPos,yPos,RGB(R,G,B),bMixMode);
			}
		}
		if(pl){
			delete pl;
			pl=0;
		}
	}
	if(strstr(szEffect,"GAPAREL")!=NULL){
		BOOL bByX=(rnd(1,100)>50)?1:0;
		double d1=rndDbl(10,50)/50;
		double d2=rndDbl(50,150)/150;
		double d3=rndDbl(10,50)/50;
		double d4=rndDbl(50,150)/150;
		double d5=rndDbl(50,150)/10;
		double t=rndDbl(50,155)/10;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			if(xPos<iL){
				continue;
			};
			BOOL bMixMode=!NOPRESERVE(dibDst,xPos,yPos-1,dwPreserveBrMin,dwPreserveBrMax);
			{
				double t2=t+d5*double(bByX?xPos:yPos)/iHF;
				double dRes=sin(d1*t2+d2)*cos(d3*t2+d4);
				if(bByX){
					dibDst.GetPixel(xPos,iHF/2+int((2+dRes)*double(yPos-iHF/2)),src[0],src[1],src[2],TRUE,bMixMode);
				}else{
					dibDst.GetPixel(iWF/2+int((2+dRes)*double(xPos-iWF/2)),yPos,src[0],src[1],src[2],TRUE,bMixMode);
				}
				// Уменьшаем яркость...
				if(dRes>0){
					double d01=1-dRes*0.7;
					src[0]=int(d01*src[0]);
					src[1]=int(d01*src[1]);
					src[2]=int(d01*src[2]);
				}
			}
		}
	}
	if(strstr(szEffect,"WAVE")!=NULL){
		bNeedOpacityProcessing=1;
		// Сливаем по случайности
		BOOL bInvert=atol(CDataXMLSaver::GetInstringPart("MASK-WAVE-INVERT:(",")",szBlock));
		double d1=rndDbl(10,50)/150;
		double d2=rndDbl(50,150)/150;
		double d3=rndDbl(10,50)/150;
		double d4=rndDbl(50,150)/150;
		double d5=rndDbl(10,100)/150;
		double d6=rndDbl(10,100)+rndDbl(0,200)/3;
		double d7=rndDbl(10,100)+rndDbl(0,200)/3;
		double d8=rndDbl(0,150)/150;
		if(d8>0.5){
			d8=0;
		}
		double d9=rndDbl(0,150)/150;
		if(d9>0.5){
			d9=0;
		}
		double t=rndDbl(50,155)/15000;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			if(xPos<iL){
				continue;
			};
			t+=d5;
			if(NOPRESERVE(dst,dwPreserveBrMin,dwPreserveBrMax)){
				double dXVklad=double(xPos)/iWF*d6+1;
				double dYVklad=double(yPos)/iHF*d7+1;
				double dRes=0;
				dRes+=sin(d1*dXVklad+d2+t*d8/1000)*cos(d3*dYVklad+d4);//*dTVklad
				dRes+=cos(d2*dYVklad+d1+t*d9/1000)*sin(d4*dXVklad+d3);
				//dRes*=0.3;
				if(dRes>0.45){
					dst[3]=bInvert?0:255;
				}else if(dRes<-0.45){
					dst[3]=bInvert?255:0;
				}else{
					dst[3]=BYTE(255*(0.5+dRes));
				}
			}
		}
	}
	if(strstr(szEffect,"IMAGE")!=NULL){
		bNeedOpacityProcessing=1;
		BOOL bInvertSt=atol(CDataXMLSaver::GetInstringPart("MASK-IMAGE-INVERT:(",")",szBlock));
		int bDropNotesUp=int(255*0.01*atol(CDataXMLSaver::GetInstringPart("MASK-IMAGE-TONES-UP:(",")",szBlock)));
		int bDropNotesDown=int(255*0.01*atol(CDataXMLSaver::GetInstringPart("MASK-IMAGE-TONES-DOWN:(",")",szBlock)));
		// Нормируем яркость
		DWORD aBrights[64]={0};
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};
			BYTE iBr2=((src[0]+src[1]+src[2])/3)>>2;
			if(iBr2>63){
				iBr2=63;
			}
			aBrights[iBr2]++;
		}
		// Считаем популярную яркость
		DWORD iPopBr=0,iPopBrLev=0;
		for(int ib=0;ib<64;ib++){
			if(iPopBr<aBrights[ib]){
				iPopBr=aBrights[ib];
				iPopBrLev=ib;
			}
		}
		// Коэффициенты...
		double dDenomin=rndDbl(10,40);
		// Считаем прозрачность...
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};
			int iBr=(dst[0]+dst[1]+dst[2])/3;
			BYTE iBr2=iBr>>2;
			iBr=int(abs(double(iPopBrLev-iBr2))*dDenomin);
			if(iBr<0){
				iBr=0;
			}
			if(iBr>255){
				iBr=255;
			}
			dst[3]=iBr;
			if(bDropNotesUp<255 && dst[3]>bDropNotesUp){
				dst[3]=255;
			}
			if(bDropNotesDown>0 && dst[3]<bDropNotesDown){
				dst[3]=0;
			}
			if(bInvertSt){
				dst[3]=(255-dst[3]);
			}
		}
	}
	if(strstr(szEffect,"STRIP")!=NULL){
		bNeedOpacityProcessing=1;
		dst=dstOrig;
		src=srcOrig;
		iSize=iSizeOrig;
		int iCnt=atol(CDataXMLSaver::GetInstringPart("MASK-IMAGE-STRIPES-COUNT:(",")",szBlock));
		if(iCnt==0){
			iCnt=rnd(2,20);
		}
		int iAng=atol(CDataXMLSaver::GetInstringPart("MASK-IMAGE-STRIPES-ANGLE:(",")",szBlock));
		if(iAng==0){
			iAng=rnd(-45,45);
		}
		int iSmooth=atol(CDataXMLSaver::GetInstringPart("MASK-IMAGE-STRIPES-SMOOTH:(",")",szBlock));
		if(iCnt<2){
			iCnt=2;
		}
		BOOL bA=0;
		{
			for(long j=-iHF;j<2*iHF;j++){
				for(long i=0;i<iWF;i++){
					int iNewX=i;
					if(iAng!=0){
						iNewX-=j/iAng;
					}
					if(i>=0 && i<iWF && j>=0 && j<iHF){
						int iNumLev=((abs(iNewX))%iWF)%(iWF/iCnt*2);
						if(iSmooth){
							double dU=iNumLev-double(iWF/iCnt);
							if(dU<0){
								dU=-dU;
							}
							bA=limit2Byte(int(dU/double(iWF/iCnt)*300));
							if(bA>128){
								bA=limit2Byte(256-bA);
							}
							bA=limit2Byte(2*bA);
						}else{
							bA=255;
						}
						DWORD dwB=dibDst.GetPixel(i,j);
						dwB=((dwB&0x00FFFFFF)|(bA<<8*3));//RGB(bA,bA,bA);//
						dibDst.SetPixel(i,j,dwB,0);
					}
				}
			}
		}
	}
	if(strstr(szEffect,"CHESS")!=NULL){
		bNeedOpacityProcessing=1;
		int iYCount=8;
		int iYSize=iHF/iYCount;
		int iXCount=iWF/iYSize;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			if(xPos<iL){
				continue;
			};
			int iXN=xPos/iYSize;
			int iYN=yPos/iYSize;
			int iCellVal=0;
			iCellVal=((iXN+iYN)%2)?255:0;
			int iB1=(dst[0]+dst[1]+dst[2])/3;
			int iB2=(src[0]+src[1]+src[2])/3;
			if(!iCellVal){
				dst[3]=(iB1>iB2)?(255-(iB1-iB2)):0;
			}else{
				dst[3]=(iB1>iB2)?0:(255-(iB2-iB1));
			}
		}
	}
	if(strstr(szEffect,"BRIGHT")!=NULL){
		bNeedOpacityProcessing=1;
		BOOL bInvert=atol(CDataXMLSaver::GetInstringPart("MASK-BRIGHT-INVERT:(",")",szBlock));
		BOOL bRab=atol(CDataXMLSaver::GetInstringPart("MASK-BRIGHT-RAB:(",")",szBlock));
		BOOL bAbs=atol(CDataXMLSaver::GetInstringPart("MASK-BRIGHT-ABS:(",")",szBlock));
		int iBumber=0;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			if(xPos<iL){
				continue;
			};
			BOOL bYarche=0;
			int iXN=(dst[0]+dst[1]+dst[2])/3;
			int iYN=(src[0]+src[1]+src[2])/3;
			if(iXN<iYN){
				bYarche=1;
				int iTmp=iYN;
				iYN=iXN;
				iXN=iTmp;
			}
			int iAlpha=0;
			if(bAbs){
				iAlpha=(bInvert?iXN:iYN)*2+(bRab?iBumber:0)%10;
			}else{
				iAlpha=(iXN-iYN)+(bRab?iBumber:0)%10;
			}
			if(iBumber>0){
				iBumber--;
			}
			long lRes=(iAlpha-40)*2;
			if(lRes<0){
				lRes=0;
				iBumber=40;
			}
			if(lRes>255){
				lRes=255;
				iBumber=40;
			}
			dst[3]=BYTE(lRes);
			if(bInvert && bYarche){
				dst[3]=255-dst[3];
			}
		}
	}
	if(strstr(szEffect,"LR")!=NULL){
		bNeedOpacityProcessing=1;
		dibDst.DrawGradientRect(400,0,0,400,3);
	}
	if(strstr(szEffect,"TB")!=NULL){
		bNeedOpacityProcessing=1;
		dibDst.DrawGradientRect(400,400,0,0,3);
	}
	if(strstr(szEffect,"LBRT")!=NULL){
		bNeedOpacityProcessing=1;
		dibDst.DrawGradientRect(127,0,127,400,3);
	}
	if(strstr(szEffect,"LTRB")!=NULL){
		bNeedOpacityProcessing=1;
		dibDst.DrawGradientRect(400,127,0,127,3);
	}
	if(strstr(szEffect,"RADIAL")!=NULL){
		bNeedOpacityProcessing=1;
		int iMaxR=iHF;
		if(iMaxR>iWidth){
			iMaxR=iWidth;
		}
		int iXC=iL+iWidth/2;
		int iYC=iHF/2;
		for(dst=dstOrig,src=srcOrig,iSize=iSizeOrig;iSize>0;iSize--,dst+=4,src+=4){
			long xPos=(iSizeOrig-iSize)%iWF;
			long yPos=(iSizeOrig-iSize)/iWF;
			if(xPos<iL){
				continue;
			};
			double iR=sqrt(double((xPos-iXC)*(xPos-iXC)+(yPos-iYC)*(yPos-iYC)))/iMaxR;
			if(iR>1){
				iR=1;
			}
			dst[3]=BYTE(255*iR);
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
				};
				int iDiff=0;
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
				};
				int iBr1=(src[0]+src[1]+src[2])/3;
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
				//*((DWORD*)dst)=*((DWORD*)src)+200;
				continue;
			};
			DWORD A=bNeedOpacityProcessing?dst[3]:255;
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
	//iT!!! iH!!!
	dibDst.Render2(&dcDst,iL,iTop,iWF,iHF-iTop);
	//dibDst.Render(&dcDst,0,iTop,iWF,iHF-iTop);
	DEBUG_INFO5("End generating effect %s on %i:%i-%i:%i",szEffect, iL, iTop, iWidth, iHeight);
	return TRUE;
}

//случ часть картинки