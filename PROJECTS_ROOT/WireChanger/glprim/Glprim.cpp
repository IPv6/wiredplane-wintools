bNoBGAtAll// glprim.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "glprim.h"
#include "GlGener.h"
#include <afxcmn.h>
#include <string>
#include "..\..\SmartWires\SystemUtils\RemindClasses.h"

bool UTF2U(const std::string &src,std::wstring &out);
BOOL ApplyImageEffect(const char* szEffect,const char* szBlock, CCallbacks* pCallback, CDC& dc, CBitmap& bmpDst, long iL, long iTop, long iWidth, long iHeight, CDC& dc2, CBitmap& bmp);
int getRndBmpFormat();
/////////////////////////////////////////////////////////////////////////////
// CGlprimApp

BEGIN_MESSAGE_MAP(CGlprimApp, CWinApp)
//{{AFX_MSG_MAP(CGlprimApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlprimApp construction

CGlprimApp::CGlprimApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGlprimApp object
CGlprimApp theApp;

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

CRect rPreviousBlock;
BOOL PrepareCommon(const char* sBlock, long& lOP, CRect& rtFull, CCallbacks* pCallback, const char* szDefTTText, BOOL bFull=1)
{
	if(bFull==1 || bFull==0){
		CString sX=CDataXMLSaver::GetInstringPart("X:(",")",sBlock);
		long lX=atol(sX);
		if(sX.Find("wp:previous-block")!=-1){
			lX=rPreviousBlock.left;
		}
		CString sY=CDataXMLSaver::GetInstringPart("Y:(",")",sBlock);
		long lY=atol(sY);
		if(sY.Find("wp:previous-block")!=-1){
			lY=rPreviousBlock.top;
		}
		CString sW=CDataXMLSaver::GetInstringPart("W:(",")",sBlock);
		long lW=atol(sW);
		if(sW.Find("wp:previous-block")!=-1){
			lW=rPreviousBlock.Width();
		}
		CString sH=CDataXMLSaver::GetInstringPart("H:(",")",sBlock);
		long lH=atol(sH);
		if(sH.Find("wp:previous-block")!=-1){
			lH=rPreviousBlock.Height();
		}
		lOP=atol(CDataXMLSaver::GetInstringPart("OPACITY:(",")",sBlock));
		if(lOP<0){
			lOP=0;
		}
		if(lOP>100){
			lOP=100;
		}
		// Пустой - пропускаем...
		if(!(lW==0 && lH==0 && lX==0 && lY==0)){
			rtFull.SetRect(lX,lY,lX+lW,lY+lH);
		}
	}
	if(bFull==1 || bFull==-1){
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
	}
	return TRUE;
}

#include <wininet.h>
CFont* getFontFromStr(CString& sFont)
{
	sFont.Replace("[","<");
	sFont.Replace("]",">");
	return CreateFontFromStr(sFont,0,0,0,0);
}

void DrawOverlay(CRect& rtFull, CString& sImages, CString& sBlock,const char* dzP,CDC* dc, DWORD dwOp=255, DWORD dwBlendColor=0xFFFFFFFF)
{// Первый оверлей
	CString sIcoOverlay=CDataXMLSaver::GetInstringPart(Format("%s:(",dzP),")",sBlock);
	CString sIcoOverlayHint=CDataXMLSaver::GetInstringPart(Format("%s-HINT:(",dzP),")",sBlock);
	if(sIcoOverlay!=""){
		BOOL bResize2=(sIcoOverlayHint.Find("SQARE")!=-1);
		BOOL bResize=bResize2||(sIcoOverlayHint.Find("RESIZE")!=-1);
		CString sImageName=sImages+sIcoOverlay;
		HICON hIco = SmartLoadIcon(theApp.m_hInstance,sImageName);
		if(hIco){
			CSize sDD=bResize?rtFull.Size():GetIconSize(hIco);
			if(bResize2){
				if(sDD.cx>sDD.cy){
					sDD.cx=sDD.cy;
				}else{
					sDD.cy=sDD.cx;
				}
			}
			Draw32Icon(dc, hIco, rtFull.TopLeft(), sDD, dwOp, dwBlendColor);
			ClearIcon(hIco);
		}
	}
}

BOOL DrawCell(CDC& dc, CString& sSkinData, const char* szSkinPart, CRect& rtFull, CString szText, DWORD dwOptions=0,DWORD dwOptionsEx=0, DWORD* dwBGOut=0, DWORD* dwFGOut=0, CRect* rtOffsetsOut=0, const char* szCustomIcon=0)
{
	// Пишем текст
	DEBUG_INFO5("Drawing cell %s %i:%i (%i:%i)",szSkinPart,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height());
	AFX_MANAGE_STATE(AfxGetAppModuleState());//AfxGetStaticModuleState()
	BOOL bDisableBGAtAll=FALSE;//isWin9x();
	CString sOnDate=CDataXMLSaver::GetInstringPart("ONDATE:(",")",sSkinData);
	CString sImages=CDataXMLSaver::GetInstringPart("IMAGES:(",")",sSkinData);
	sImages.Replace("\\","/");
	if(sImages!="" && sImages.Right(1)!='/'){
		int iRSlash=sImages.ReverseFind('/');
		if(iRSlash!=-1){
			sImages=sImages.Left(iRSlash+1);
		}
	}
	CString sBlock="";
	if(szSkinPart && *szSkinPart!=0){
		sBlock=CDataXMLSaver::GetInstringPart(Format("<%s>",szSkinPart),Format("</%s>",szSkinPart),sSkinData);
		CString sAddon=CDataXMLSaver::GetInstringPart("<SPECIAL-ADDON>","</SPECIAL-ADDON>",sSkinData);
		if(sAddon!=""){
			sBlock+=sAddon;
		}	
	}else{
		sBlock=sSkinData;
	}
	BOOL bNoBGAtAll=atol(CDataXMLSaver::GetInstringPart("NO-BACKGROUND:(",")",sBlock));
	int dwMaxWidth=atol(CDataXMLSaver::GetInstringPart("TEXT-WIDTH:(",")",sBlock));
	if(dwMaxWidth>0){
		rtFull.right=rtFull.left+dwMaxWidth;
	}
	CRect rtFullOriginal(rtFull);
	COLORREF lBG=0;
	COLORREF lTX=0xFFFFFF;
	COLORREF lBGTmp=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BG-COLOR:(",")",sBlock));
	COLORREF lTXTmp=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("TEXT-COLOR:(",")",sBlock));
	if(dwBGOut!=NULL && dwFGOut!=NULL && *dwFGOut!=*dwBGOut){
		lTX=*dwFGOut;
		lBG=*dwBGOut;
	}else{
		lBG=lBGTmp;
		lTX=lTXTmp;
	}
	if(dwBGOut){
		*dwBGOut=lBG;
	}
	if(dwFGOut){
		*dwFGOut=lTX;
	}
	long lTMargins=0,lBMargins=0,lRMargins=0,lLMargins=0;
	lTMargins=lBMargins=lRMargins=lLMargins=atol(CDataXMLSaver::GetInstringPart("IMAGE-BORDERS:(",")",sBlock));
	CString sTBord=CDataXMLSaver::GetInstringPart("IMAGE-TBORDER:(",")",sBlock);
	if(sTBord!=""){
		lTMargins=atol(sTBord);
	}
	CString sBBord=CDataXMLSaver::GetInstringPart("IMAGE-BBORDER:(",")",sBlock);
	if(sBBord!=""){
		lBMargins=atol(sBBord);
	}
	CString sLBord=CDataXMLSaver::GetInstringPart("IMAGE-LBORDER:(",")",sBlock);
	if(sLBord!=""){
		lLMargins=atol(sLBord);
	}
	CString sRBord=CDataXMLSaver::GetInstringPart("IMAGE-RBORDER:(",")",sBlock);
	if(sRBord!=""){
		lRMargins=atol(sRBord);
	}
	if(rtOffsetsOut){
		*rtOffsetsOut=CRect(lLMargins,lTMargins,lRMargins,lBMargins);
	}
	long lOP=atol(CDataXMLSaver::GetInstringPart("OPACITY:(",")",sBlock));
	if(lOP<0){
		lOP=0;
	}
	if(lOP>100){
		lOP=100;
	}
	BOOL bWarnIfClip=atol(CDataXMLSaver::GetInstringPart("WARN-CLIP:(",")",sBlock));
	CString sHints=CDataXMLSaver::GetInstringPart("HINTS:(",")",sBlock);
	CString sFont=CDataXMLSaver::GetInstringPart("FONT:(",")",sBlock);
	long lTextOp=100;
	CString sTextOpacity=CDataXMLSaver::GetInstringPart("OPACITY-TEXT:(",")",sBlock);
	if(sTextOpacity!=""){
		lTextOp=atol(sTextOpacity);
		if(lTextOp<0){
			lTextOp=0;
		}
		if(lTextOp>100){
			lTextOp=100;
		}
	}
	BOOL bFHBg=(sHints.Find("FULL-HEIGHT-BG")!=-1);
	if(sHints.Find("NO-TEXT")!=-1){
		szText="";
	}else{
		CString sTextSubst=CDataXMLSaver::GetInstringPart("TEXT-SUBSTITUTE:(",")\n",sBlock);
		if(sTextSubst!=""){
			szText=sTextSubst;
		}
		CString sTextConc=CDataXMLSaver::GetInstringPart("TEXT-CONCATE:(",")\n",sBlock);
		if(sTextConc!=""){
			szText+=sTextConc;
		}
	}
	// Шрифт
	CFont* pFont=0;
	CFont* pOldFont=0;
	if(sFont!=""){
		pFont=getFontFromStr(sFont);
		pOldFont=dc.SelectObject(pFont);
	}
	// Для szCustomIcon
	BOOL bIconSize=atol(CDataXMLSaver::GetInstringPart("ICON-SIZE:(",")",sBlock));
	BOOL bIconXOff=atol(CDataXMLSaver::GetInstringPart("ICON-X-OFFSET:(",")",sBlock));
	BOOL bIconYOff=atol(CDataXMLSaver::GetInstringPart("ICON-Y-OFFSET:(",")",sBlock));
	// Параметры рисования
	CString sShadow=CDataXMLSaver::GetInstringPart("SHADOW:(",")",sBlock);
	BOOL bShadow=atol(sShadow);
	CString sBorder=CDataXMLSaver::GetInstringPart("BORDER:(",")",sBlock);
	BOOL bBorder=atol(sBorder);
	CString sHAlign=CDataXMLSaver::GetInstringPart("HALIGN:(",")",sBlock);
	CString sVAlign=CDataXMLSaver::GetInstringPart("VALIGN:(",")",sBlock);
	DWORD dwAddHAlign=dwOptions;
	if(sHAlign.CompareNoCase("AUTO")==0){
		if(abs(rtFull.left-0)<abs(GetDesktopRect().right-rtFull.right)){
			sHAlign="LEFT";
		}else{
			sHAlign="RIGHT";
		}
	}
	if(sVAlign.CompareNoCase("AUTO")==0){
		if(abs(rtFull.top-0)<abs(GetDesktopRect().bottom-rtFull.bottom)){
			sVAlign="TOP";
		}else{
			sVAlign="BOTTOM";
		}
	}
	if(sHAlign.CompareNoCase("CENTER")==0){
		dwAddHAlign&=~(DT_RIGHT|DT_LEFT);
		dwAddHAlign|=DT_CENTER;
	}
	if(sHAlign.CompareNoCase("LEFT")==0){
		dwAddHAlign&=~(DT_RIGHT|DT_CENTER);
		dwAddHAlign|=DT_LEFT;
	}
	if(sHAlign.CompareNoCase("RIGHT")==0){
		dwAddHAlign&=~(DT_LEFT|DT_CENTER);
		dwAddHAlign|=DT_RIGHT;
	}
	DWORD dwAddVAlign=dwOptionsEx;
	if(sVAlign.CompareNoCase("TOP")==0){
		dwAddVAlign=0;
	}
	if(sVAlign.CompareNoCase("BOTTOM")==0){
		dwAddVAlign=TXTEXT_VBOTTOM;
	}
	DWORD dwShadow=(bShadow?TXSHADOW_GLOW:0);
	if(bShadow){
		if(sShadow.Find("S")!=-1){
			dwShadow=TXSHADOW_SIMPLE;
		}
		if(sShadow.Find("B")!=-1 || sShadow.Find("W")!=-1){
			dwAddVAlign|=TXSHADOW_WIDE;
		}
		if(sShadow.Find("M")!=-1){
			dwAddVAlign|=TXSHADOW_MIXBG;
		}
		if(sShadow.Find("R")!=-1){
			dwAddVAlign|=TXSHADOW_USERGN;
		}
	}
	if(sSkinData.Find("TEXT-WRAP:(1)")!=-1 || sHints.Find("WORD-WRAP")!=-1){
		dwAddHAlign|=DT_WORDBREAK;
	}
	CBitmap* bmpBG=NULL;
	
	long lStrBgImage=(sHints.Find("STRETCH-BGIMAGE")==-1?0:1);
	long lStrBgImagePart=(sHints.Find("STRETCH-BGPART")==-1?0:1);
	if(sHints.Find("NO-BG")==-1 && !bNoBGAtAll){// Рисуем картинку-фон
		if(!bDisableBGAtAll){
			CString sImageName=CDataXMLSaver::GetInstringPart("BG-IMAGE:(",")",sBlock);
			if(sImageName=="" && szSkinPart && *szSkinPart!=0){
				sImageName=sImages+szSkinPart+"-bg";
				sImageName.MakeLower();
			}else if(sImageName.Find(":\\")==-1){
				sImageName=sImages+sImageName;
			}
			if(sImageName!=""){
				bmpBG=LoadBmpFromPathExt(sImageName);
			}
		}
	}
	CSafeDC dc2;
	CBitmap bmp;
	dc2.CreateCompatibleDC(&dc);
	BLENDFUNCTION blendFunction;
	blendFunction.BlendFlags=0;
	blendFunction.AlphaFormat=0;
	blendFunction.BlendOp=AC_SRC_OVER;
	CRect rtDrawField(0,0,rtFullOriginal.Width(), rtFullOriginal.Height());
	long lBG_XExp=atol(CDataXMLSaver::GetInstringPart("BG-X-EXPAND:(",")",sBlock));
	long lBG_YExp=atol(CDataXMLSaver::GetInstringPart("BG-Y-EXPAND:(",")",sBlock));
	long lBG_XOffs=atol(CDataXMLSaver::GetInstringPart("BG-X-OFFSET:(",")",sBlock));
	long lBG_YOffs=atol(CDataXMLSaver::GetInstringPart("BG-Y-OFFSET:(",")",sBlock));
	long lTHei=atol(CDataXMLSaver::GetInstringPart("TEXT-RECT-HEIGHT:(",")",sBlock));
	BOOL bFitw=atol(CDataXMLSaver::GetInstringPart("FITW-TO-TEXT:(",")",sBlock));
	rtDrawField.right+=lBG_XExp;
	rtDrawField.bottom+=lBG_YExp;
	rtDrawField.left+=lBG_XOffs;
	rtDrawField.top+=lBG_YOffs;
	if((!bFHBg && lTHei==0) || bFitw){
		CFont* pTmpFont=dc.SelectObject(pOldFont);
		CFont* pTmpFont2=dc2.SelectObject(pTmpFont);
		CRect rtDrawFieldOriginal(rtDrawField);
		SmartDrawText(dc2,szText,rtDrawField,0,lTX,lBG,TXSHADOW_CALCRC|dwAddVAlign|dwShadow|(bBorder?TXSHADOW_MIXBG:0),dwAddHAlign|DT_NOPREFIX);
		dc2.SelectObject(pTmpFont2);
		pOldFont=dc.SelectObject(pTmpFont);
		if((dwAddVAlign & TXTEXT_VBOTTOM)!=0){
			long dwAddition=rtDrawField.bottom-rtDrawFieldOriginal.bottom;
			rtDrawField.top+=dwAddition;
			rtDrawField.bottom+=dwAddition;
		}
		rtDrawField.bottom+=lTMargins;
		rtDrawField.bottom+=lBMargins;
		if(sHints.Find("FITW-TO-TEXT")!=-1 || bFitw){
			rtDrawField.left=rtDrawFieldOriginal.left;
			rtDrawField.right=rtDrawFieldOriginal.right;
			rtDrawField.bottom=rtDrawFieldOriginal.top+rtDrawField.Height();
			if(bFitw){
				rtFull.bottom=rtFull.top+rtDrawField.Height();
			}
		}else if(sHints.Find("FIT-TO-TEXT")==-1){
			rtDrawField.left=0;
			rtDrawField.right=rtFullOriginal.Width();
		}else{
			rtDrawField.right+=lRMargins+lLMargins;
		}
	}
	bmp.CreateCompatibleBitmap(&dc, rtDrawField.Width(), rtDrawField.Height());
	CBitmap* bmOld2=dc2.SelectObject(&bmp);
	BOOL bBGMixer=FALSE;
	// Копируем фон в текущий битмап
	dc2.BitBlt(0, 0, rtDrawField.Width(), rtDrawField.Height(), &dc, rtFull.left, rtFull.top, SRCCOPY);
	blendFunction.SourceConstantAlpha=(int)(double(255)*double(lOP)/double(100));
	if(lOP && (sHints.Find("BG-NORECT")==-1 && bNoBGAtAll==0)){
		bBGMixer=TRUE;
		CRect rtInsideBG(rtDrawField);
		DWORD dwRectOffsets=atol(CDataXMLSaver::GetInstringPart("BG-RECT-OFFSETS:(",")",sBlock));
		rtInsideBG.DeflateRect(dwRectOffsets,dwRectOffsets);
		dc2.FillSolidRect(&rtInsideBG,lBG);
	}
	// Забираем часть картинка с оригинала...
	if(bBGMixer && lOP){
		blendFunction.SourceConstantAlpha=(int)(double(255)*double(lOP)/double(100));
		// Подрисовываем фон...
		AlphaBlendMy(dc, rtFull.left, rtFull.top, rtDrawField.Width(), rtDrawField.Height(), dc2, 0, 0, blendFunction);
		blendFunction.SourceConstantAlpha=(int)(double(255)*double(lOP)*0.7/double(100));
	}
	if(lOP && bmpBG!=NULL){
		CSafeDC dc3;
		int i=0;
		COLORREF cWhite=RGB(255,255,255);
		dc3.CreateCompatibleDC(&dc);
		CSize bgSize=GetBitmapSize(bmpBG);
		CSize bgSizeInt=CSize(bgSize.cx-lLMargins-lRMargins,bgSize.cy-lTMargins-lBMargins);
		CBitmap* bmpOld3=dc3.SelectObject(bmpBG);
		CRect rtBG(rtDrawField);
		CSize szBG(rtBG.Width(),rtBG.Height());
		int iXCount=0;
		if(bgSizeInt.cx>0){
			iXCount=int(double(szBG.cx)/bgSizeInt.cx)+1;
		}
		int iYCount=0;
		if(bgSizeInt.cy>0){
			iYCount=int(double(szBG.cy)/bgSizeInt.cy)+1;
		}
		if(lStrBgImage){
			TransparentBltMy(dc2,0, 0, rtDrawField.Width(), rtDrawField.Height(), dc3, 0, 0, bgSize.cx, bgSize.cy, cWhite);
		}else if(lStrBgImagePart){
			TransparentBltMy(dc2,lLMargins, lTMargins, rtDrawField.Width()-lLMargins-lRMargins, rtDrawField.Height()-lTMargins-lBMargins, 
				dc3, lLMargins, lTMargins, bgSize.cx-lLMargins-lRMargins,bgSize.cy-lTMargins-lBMargins,cWhite);
		}else{
			for(int ix=0;ix<iXCount;ix++){
				for(int iy=0;iy<iYCount;iy++){
					TransparentBltMy(dc2,bgSizeInt.cx*ix, bgSizeInt.cy*iy, bgSizeInt.cx, bgSizeInt.cy, dc3, lLMargins, lTMargins, cWhite);
				}
			}
		}
		if(lTMargins!=0){
			long dwWidthCounter=szBG.cx-lLMargins-lRMargins;
			for(i=0;i<=iXCount && dwWidthCounter>0;i++){
				TransparentBltMy(dc2,lLMargins+bgSizeInt.cx*i, 0, dwWidthCounter<bgSizeInt.cx?dwWidthCounter:bgSizeInt.cx, lTMargins	,dc3, lLMargins, 0, cWhite);
				dwWidthCounter-=bgSizeInt.cx;
			}
		}
		if(lBMargins!=0){
			long dwWidthCounter=szBG.cx-lLMargins-lRMargins;
			for(i=0;i<=iXCount && dwWidthCounter>0;i++){
				TransparentBltMy(dc2,lLMargins+bgSizeInt.cx*i, szBG.cy-lBMargins, dwWidthCounter<bgSizeInt.cx?dwWidthCounter:bgSizeInt.cx, lBMargins, dc3, lLMargins, bgSize.cy-lBMargins, cWhite);
				dwWidthCounter-=bgSizeInt.cx;
			}
		}
		if(lLMargins!=0){
			long dwHeightCounter=szBG.cy-lTMargins-lBMargins;
			for(i=0;i<=iYCount && dwHeightCounter>0;i++){
				TransparentBltMy(dc2,0, lTMargins+bgSizeInt.cy*i, lLMargins, dwHeightCounter<bgSizeInt.cy?dwHeightCounter:bgSizeInt.cy, dc3, 0, lTMargins, cWhite);
				dwHeightCounter-=bgSizeInt.cy;
			}
		}
		if(lRMargins!=0){
			long dwHeightCounter=szBG.cy-lTMargins-lBMargins;
			for(i=0;i<=iYCount && dwHeightCounter>0;i++){
				TransparentBltMy(dc2,szBG.cx-lRMargins, lTMargins+bgSizeInt.cy*i, lRMargins, dwHeightCounter<bgSizeInt.cy?dwHeightCounter:bgSizeInt.cy, dc3, bgSize.cx-lRMargins, lTMargins, cWhite);
				dwHeightCounter-=bgSizeInt.cy;
			}
		}
		if(lLMargins!=0 && lTMargins!=0){
			TransparentBltMy(dc2,0, 0, lLMargins, lTMargins, dc3, 0, 0, cWhite);
		}
		if(lRMargins!=0 && lTMargins!=0){
			TransparentBltMy(dc2,szBG.cx-lRMargins, 0, lRMargins, lTMargins, dc3, bgSize.cx-lRMargins, 0, cWhite);
		}
		if(lLMargins!=0 && lBMargins!=0){
			TransparentBltMy(dc2,0, szBG.cy-lBMargins, lLMargins, lBMargins, dc3, 0, bgSize.cy-lBMargins, cWhite);
		}
		if(lRMargins!=0 && lBMargins!=0){
			TransparentBltMy(dc2,szBG.cx-lRMargins, szBG.cy-lBMargins, lRMargins, lBMargins, dc3, bgSize.cx-lRMargins, bgSize.cy-lBMargins, cWhite);
		}
		CBitmap* bmTest1=dc3.SelectObject(bmpOld3);
	}
	if(lOP){
		if(bDisableBGAtAll || ((bmpBG==NULL || sHints.Find("BG-RECT")!=-1) && sHints.Find("BG-NORECT")==-1 && !bNoBGAtAll)){
			dc2.Draw3dRect(&rtDrawField,lTX,lTX);
		}
	}
	delete bmpBG;
	if(lOP){
		DrawOverlay(rtDrawField, sImages, sBlock,"ICO-OVERLAY",&dc2,255, (bNoBGAtAll==2)?lBG:0xFFFFFFFF);
		DrawOverlay(rtDrawField, sImages, sBlock,"ICO-OVERLAY2",&dc2,255, (bNoBGAtAll==2)?lBG:0xFFFFFFFF);
	}
	// Рисуем в оригинал с полупрозрачностью
	AlphaBlendMy(dc,rtFull.left, rtFull.top, rtDrawField.Width(), rtDrawField.Height(), dc2, 0, 0, blendFunction);
	CBitmap* bmTest2=dc2.SelectObject(bmOld2);
	// CustomIcon
	if(lOP && bIconSize>0 && szCustomIcon && isFileExist(szCustomIcon)){
		CSize sIco(50,50);
		CSize sIcoM(sIco);
		sIcoM.cx=int(sIcoM.cx*(double(bIconSize)/100));
		sIcoM.cy=int(sIcoM.cy*(double(bIconSize)/100));
		CPoint pt(rtFull.left-sIcoM.cx/2,rtFull.top-sIcoM.cy/2);
		pt.x+=int(rtFull.Width()*(double(bIconXOff)/100));
		pt.y+=int(rtFull.Height()*(double(bIconYOff)/100));
		CString sIconPath=szCustomIcon;
		CBitmap* bmpIco=_bmpX()->LoadBmpFromPath(sIconPath);
		HICON m_hIcon=CreateIconFromBitmap(bmpIco,&sIcoM,TRUE);
		::DrawIconEx(dc.GetSafeHdc(), pt.x, pt.y, m_hIcon, sIcoM.cx, sIcoM.cy, 0, NULL, DI_NORMAL);
		ClearIcon(m_hIcon);
		_bmpX()->ReleaseBmp(bmpIco);
	}
	CRect rtForText(rtFull);
	rtForText.DeflateRect(lLMargins,lTMargins,lRMargins,lBMargins);
	long lXOffset=atol(CDataXMLSaver::GetInstringPart("TEXT-X-OFFSET:(",")",sBlock));
	long lYOffset=atol(CDataXMLSaver::GetInstringPart("TEXT-Y-OFFSET:(",")",sBlock));
	long lXROffset=atol(CDataXMLSaver::GetInstringPart("TEXT-CLIPX-OFFSET:(",")",sBlock));
	long lYROffset=atol(CDataXMLSaver::GetInstringPart("TEXT-CLIPY-OFFSET:(",")",sBlock));
	rtForText.left+=lXOffset;
	rtForText.top+=lYOffset;
	rtForText.right+=lXROffset;
	rtForText.bottom+=lYROffset;
	if(lTHei>0){
		rtForText.bottom=rtForText.top+lTHei;
	}
	if(sHints.Find("CLIP-TEXT")==-1 && lTHei==0){
		dwAddVAlign|=TXTEXT_CALCDR;
	}
	if(lOP){
		CString s=szText;
		CRect rtForTextFree(rtForText);
		long lXOffsetFloat=atol(CDataXMLSaver::GetInstringPart("TEXT-X-FLOATOFFSET:(",")",sBlock));
		long lYOffsetFloat=atol(CDataXMLSaver::GetInstringPart("TEXT-Y-FLOATOFFSET:(",")",sBlock));
		rtForText.OffsetRect(lXOffsetFloat,lYOffsetFloat);
		BOOL bClipped=SmartDrawTextX(dc,s,rtForText,0,lTX,lBG,dwAddVAlign|dwShadow|(bBorder?TXSHADOW_MIXBG:0),dwAddHAlign|DT_NOPREFIX,0,lTextOp);
		rtForText.OffsetRect(-lXOffsetFloat,-lYOffsetFloat);
		if(bWarnIfClip && bClipped){
			CBitmap* bmpIco=_bmpX()->Get("DDOWN");
			CSize sDD(12,12);
			HICON m_hIcon=CreateIconFromBitmap(bmpIco,&sDD,TRUE);
			::DrawIconEx(dc.GetSafeHdc(), rtForText.right-12, rtForText.bottom-12, m_hIcon, sDD.cx, sDD.cy, 0, NULL, DI_NORMAL);
			ClearIcon(m_hIcon);
		}
		DrawOverlay(rtFull, sImages, sBlock,"ICO-OVERLAY3",&dc2, 255, (bNoBGAtAll==2)?lBG:0xFFFFFFFF);
		DrawOverlay(rtFull, sImages, sBlock,"ICO-OVERLAY-EX",&dc, lOP, (bNoBGAtAll==2)?lBG:0xFFFFFFFF);
		if(atol(sOnDate)>0){
			if(sBlock.Find("ICO-OVERLAY-REM")==-1){
				CBitmap* bmpIco=_bmpX()->Get("REMINDER");
				CSize sDD(12,12);
				HICON m_hIcon=CreateIconFromBitmap(bmpIco,&sDD,TRUE);
				::DrawIconEx(dc.GetSafeHdc(), rtForText.right-sDD.cx-1, rtForText.top+rtForText.Height()/2-sDD.cy/2+2, m_hIcon, sDD.cx, sDD.cy, 0, NULL, DI_NORMAL);
				ClearIcon(m_hIcon);
			}else{
				DrawOverlay(rtFull, sImages, sBlock,"ICO-OVERLAY-REM",&dc,lOP, (bNoBGAtAll==2)?lBG:0xFFFFFFFF);
			}
		}
	}
	if(pOldFont){
		dc.SelectObject(pOldFont);
	}
	if(pFont){
		delete pFont;
	}
	if(!bFitw){
		// Подгонка как для календаря
		rtForText.left-=lXOffset;
		rtForText.top-=lYOffset;
		rtForText.right-=lXROffset;
		rtForText.bottom-=lYROffset;
		BOOL bLimitRectToText=atol(CDataXMLSaver::GetInstringPart("LIMIT-BOTTOM-TOTEXT:(",")",sSkinData));
		rtFull.top=rtForText.bottom+(bLimitRectToText?0:lBMargins);
		rtFull.bottom=rtFullOriginal.bottom;
		rtFull.left=rtFullOriginal.left;
		rtFull.right=rtFullOriginal.right;
	}
	DEBUG_INFO5("End of Drawing cell %s %i:%i (%i:%i)",szSkinPart,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height());
	return TRUE;
}

BOOL DrawCellChex(CDC& dc, CString& sSkinData, const char* szSkinPart, CRect& rtFull, CString szText, DWORD dwOptions=0,DWORD dwOptionsEx=0, DWORD* dwBGOut=0, DWORD* dwFGOut=0, CRect* rtOffsetsOut=0, const char* szCustomIcon=0)
{
	if(szSkinPart && *szSkinPart!=0){
		CString sBlock=CDataXMLSaver::GetInstringPart(Format("<%s>",szSkinPart),Format("</%s>",szSkinPart),sSkinData);
		if(sBlock==""){
			return FALSE;
		}
	}
	return DrawCell(dc, sSkinData, szSkinPart, rtFull, szText, dwOptions,dwOptionsEx, dwBGOut, dwFGOut, rtOffsetsOut, szCustomIcon);
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
	CBitmap bmpScreenImage;
	bmpScreenImage.Attach(hbmOutImage);
	if(iFrom==-1){
		iFrom=0;
	}
	CSafeDC dcScreen;
	dcScreen.CreateDC("DISPLAY", NULL, NULL, NULL);
	while(iFrom>=0){
		sBlock=CDataXMLSaver::GetInstringPart("<calendar-in-rect>","</calendar-in-rect>",sScript,iFrom);
		if(sBlock!=""){
			sBlock.Replace("\r\n","\n");
			// Календарь в прямоугольнике
			int i=0;
			long lOP=100;
			CRect rtFull;
			CFont* pFont=0;
			PrepareCommon(sBlock,lOP,rtFull,pCallback,0);
			long dwWeeksToShow=atol(CDataXMLSaver::GetInstringPart("WEEKS_TO_SHOW:(",")",sBlock));
			long dwMonthsToShow=atol(CDataXMLSaver::GetInstringPart("MONTHS_TO_SHOW:(",")",sBlock));
			DWORD dwFirstDay=atol(CDataXMLSaver::GetInstringPart("FIRST-DAY:(",")",sBlock));
			DWORD dwMonOffset=atol(CDataXMLSaver::GetInstringPart("MONTH-OFFSET:(",")",sBlock));
			DWORD dwStartFromMon=atol(CDataXMLSaver::GetInstringPart("START-FROM-MONTH:(",")",sBlock));
			BOOL bInteract=atol(CDataXMLSaver::GetInstringPart("INTERACTIVITY:(",")",sBlock));
			BOOL bAllowOOI=atol(CDataXMLSaver::GetInstringPart("USE-OOI-CELLS:(",")",sBlock));
			BOOL bUseWrapNotes=pCallback->GetCallerOption(CALLER_OPTION_WRAPTEXT);
			CString sFont=CDataXMLSaver::GetInstringPart("TEXT_FONT:(",")",sBlock);
			CString sSubTitle=CDataXMLSaver::GetInstringPart("SUBTITLE:(",")\n",sBlock);
			DWORD dwOffset=atol(CDataXMLSaver::GetInstringPart("CELL-SPACING:(",")",sBlock));
			CString sCalBgHints=CDataXMLSaver::GetInstringPart("CALENDAR-BACKGROUND:(",")",sBlock);
			CString sCalPastOverlay=CDataXMLSaver::GetInstringPart("PAST-DAYS-OVERLAY:(",")",sBlock);
			CString sCalPastOverlayHint=CDataXMLSaver::GetInstringPart("PAST-DAYS-OVERLAY-HINT:(",")",sBlock);
			CString sCalPastOverlayOpacity=CDataXMLSaver::GetInstringPart("PAST-DAYS-OVERLAY-OPACITY:(",")",sBlock);
			//CString sHolidays=CString(",")+CDataXMLSaver::GetInstringPart("<holidays>","</holidays>",sBlock)+",";
			//CString sHolidaysF=CString(",")+CDataXMLSaver::GetInstringPart("<holidays-float>","</holidays-float>",sBlock)+",";
			CString sHolidaysFile=CDataXMLSaver::GetInstringPart("RDAYS-FILE:(",")\n",sBlock);
			CString sHolidaysContent;
			ReadFile(sHolidaysFile,sHolidaysContent);
			sHolidaysContent.Replace("\r\n","\n");
			sHolidaysContent=CString("\n")+sHolidaysContent+"\n";
			// Дни рождения
			DWORD dwCellsNumOnly=atol(CDataXMLSaver::GetInstringPart("CELL-NUMSONLY:(",")",sBlock));
			DWORD dwCellsNoNums=atol(CDataXMLSaver::GetInstringPart("CELL-NONUM:(",")",sBlock));
			DWORD dwBirthInCells=atol(CDataXMLSaver::GetInstringPart("CELL-BIRTHDAYS:(",")",sBlock));
			CString sBirthFile=CDataXMLSaver::GetInstringPart("BIRTHDAY-FILE:(",")",sBlock);
			CString sBirthContent;
			ReadFile(sBirthFile,sBirthContent);
			sBirthContent.Replace("\r\n","\n");
			sBirthContent=CString("\n")+sBirthContent+"\n";
			// Заметки
			CString sNotesFile=CDataXMLSaver::GetInstringPart("NOTES-FILE:(",")",sBlock);
			CString sNotesContent;
			ReadFile(sNotesFile,sNotesContent);
			sNotesContent.Replace("\r\n","\n");
			sNotesContent+="\n";
			// Скин
			CString sSkinOriginal=CDataXMLSaver::GetInstringPart("<calendar-skin>","</calendar-skin>",sBlock);
			CString sSkinDsk=sSkinOriginal;
			// Переводы
			CStringArray aWDays,aWDaysS,aMonths;
			for(i=0;i<7;i++){
				CString sWDay=CDataXMLSaver::GetInstringPart(Format("wdays[%i]='",i),"';",sBlock);
				aWDays.SetAtGrow(i,sWDay);
				CString sWDayS=CDataXMLSaver::GetInstringPart(Format("wdays_short[%i]='",i),"';",sBlock);
				aWDaysS.SetAtGrow(i,sWDayS);
			}
			for(i=0;i<12;i++){
				CString sMonth=CDataXMLSaver::GetInstringPart(Format("month[%i]='",i),"';",sBlock);
				aMonths.SetAtGrow(i,sMonth);
			}
			COleDateTime dt=GETCURTIME;//COleDateTime::GetCurrentTime();
			COleDateTime dtToday=dt;
			if(dwMonOffset>0){
				int nYear=dt.GetYear();
				int nMonth=dt.GetMonth();
				int nDay=dt.GetDay();
				nMonth+=dwMonOffset;
				if(nMonth>12){
					nMonth=1;
					nYear++;
				}
				dt.SetDate(nYear, nMonth, nDay);
			}
			COleDateTime dtStart=dt;
			COleDateTime dtYesterday=dt-COleDateTimeSpan(1,0,0,0);
			// Ищем первый день для показа...
			int iUncounted=0;
			BOOL bContinueSearch=TRUE, bStartOfMonthWasAlready=(dwStartFromMon?FALSE:TRUE);
			while(bContinueSearch && iUncounted<60){
				if(dtStart.GetDay()==1){
					bStartOfMonthWasAlready=TRUE;
				}
				if(!dwStartFromMon || (dwStartFromMon && bStartOfMonthWasAlready)){
					if(iUncounted%7==1 && dwFirstDay==2){
						bContinueSearch=FALSE;
						break;
					}
					if(dtStart.GetDayOfWeek()==1 && dwFirstDay==1){
						bContinueSearch=FALSE;
						break;
					}
					if(dtStart.GetDayOfWeek()==2 && dwFirstDay==0){
						bContinueSearch=FALSE;
						break;
					}
				}
				// На день назад...
				dtStart-=COleDateTimeSpan(1,0,0,0);
				iUncounted++;
			}
			if(dwMonthsToShow>0){
#ifdef _DEBUG
				CString sDbg=dtStart.Format("%x %X");
#endif 
				COleDateTime dtStart2(dtStart);
				COleDateTime dtStartEFWeek(dtStart);
				dtStartEFWeek+=COleDateTimeSpan(7,0,0,0);
				for(int i=0;i<dwWeeksToShow;i++){
					dtStart2+=COleDateTimeSpan(7,0,0,0);
#ifdef _DEBUG
					CString sDbg2=dtStart2.Format("%x %X");
#endif

					int iMDif=dtStart2.GetMonth()-dtStartEFWeek.GetMonth();
					iMDif=(12+iMDif)%12;
					if(iMDif>=dwMonthsToShow){
						dwWeeksToShow=i+((dwMonthsToShow==2)?2:1);
						break;
					}
				}
			}
			DWORD iWeekName=dtStart.GetDayOfWeek()-1;
			{// Подготавливаем канву
				CSafeDC dc;
				dc.CreateCompatibleDC(&dcScreen);
				CBitmap* bmOld=dc.SelectObject(&bmpScreenImage);
				CRect rtFlowRect(rtFull);
				{// Заголовок календаря
					CRect rtFlow2(rtFlowRect);
					BOOL bRootRectDeflated=FALSE;
					if(sCalBgHints.Find("WIDTH-BALANCE")!=-1){
						rtFlow2.left+=dwOffset;
						rtFlow2.right-=dwOffset;
						bRootRectDeflated=TRUE;
					}
					CString sTitleOpts=CDataXMLSaver::GetInstringPart("CALENDAR-TITLE:(",")",sBlock);
					if(sTitleOpts!="SKIP"){// Рисуем заголовок 
						CRect rtOffsets;
						char szTmp[128]={0};
						DWORD dwBG=0,dwFG=0;
						if(sTitleOpts=="MONTH-ONLY"){
							strcpy(szTmp,aMonths[dt.GetMonth()-1]);
						}else if(sTitleOpts==""){
							GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,0,0,szTmp,sizeof(szTmp));
						}else{
							GetDateFormat(LOCALE_USER_DEFAULT,0,0,sTitleOpts,szTmp,sizeof(szTmp));
						}
						if(DrawCellChex(dc,sSkinDsk,"TITLE",rtFlow2,szTmp,DT_CENTER,0,&dwBG,&dwFG,&rtOffsets)){
							if(bInteract){
								CRect rtAZone(rtFlow2);
								rtAZone.top=rtFlowRect.top;
								rtAZone.bottom=rtFlow2.top;
								CString sAction="WP:OPEN-CAL";
								/*"HINTS:(USE-NOTE-AS-EDITOR,TITLE-DISABLED,NOSIZE,NOMOVE,REDRAW-AFTER-EDIT,FULL-WND-EDIT)";
								sAction+=Format("X:(%i),Y:(%i),W:(%i),H:(%i)",rtAZone.left,rtAZone.top,rtAZone.Width(),rtAZone.Height());
								sAction+=Format("FG:(%i),BG:(%i),",dwFG,dwBG);
								sAction+=Format("EDIT-KEY:(CallX[Pickers.dll:GetDateX])");
								sAction+=Format("EDIT-FILE:(%s)",sNotesFile);*/
								sAction+=Format("<TOOLTIP>%s</TOOLTIP>",_l("Double-click to popup calendar for 4 months and add note to future date"));
								pCallback->AddActiveZone(&rtAZone, sAction);
							}
							rtFlow2.top+=2*dwOffset;
							rtFlowRect.top=rtFlow2.top;
							if(sCalBgHints.Find("WIDTH-LEFT-OFFSET")!=-1){
								rtFlow2.left+=rtOffsets.left;
								rtFlowRect.left=rtFlow2.left;
							}
						}
					}
					if(!bRootRectDeflated){
						rtFlow2.left+=dwOffset;
						rtFlow2.right-=dwOffset;
						bRootRectDeflated=TRUE;
					}
					if(sSubTitle!=""){// Рисуем дни рождения
						CPoint ptTL=rtFlow2.TopLeft();
						if(DrawCellChex(dc,sSkinDsk,"SUBTITLE",rtFlow2,sSubTitle,DT_CENTER|DT_WORDBREAK)){
							CRect rtTitleRect(ptTL.x,ptTL.y,rtFlow2.right,rtFlow2.top);
							rtFlowRect.top=rtFlow2.top;
							rtFlowRect.top+=2*dwOffset;
							pCallback->AddActiveZone(&rtTitleRect, Format("<TOOLTIP>%s</TOOLTIP>",sSubTitle));
						}
					}
				}
				CRect rtWholeCalendarRectPost(rtFlowRect);
				CRect rtWholeCalendarRectPre(rtFlowRect);
				DrawCellChex(dc,sSkinDsk,"PRE-WATERMARK",rtWholeCalendarRectPre,"",DT_CENTER);				
				double iCellXS=double(rtFlowRect.Width())/7;
				{// Рисуем названия дней недели
					CStringArray* pDayNames=&aWDays;
					// Пробуем на первом дне
					for(i=0;i<7;i++){
						int iLen=dc.GetTextExtent((*pDayNames)[i]).cx;
						if(long(iLen+dwOffset)>long(iCellXS)){
							pDayNames=&aWDaysS;
							break;
						}
					}
					DWORD dwBG=0,dwFG=0;
					for(i=0;i<7;i++){
						CRect rtCell(rtFlowRect);
						rtCell.left=long(rtFlowRect.left+i*iCellXS+dwOffset);
						rtCell.right=long(rtFlowRect.left+i*iCellXS+iCellXS-dwOffset);
						CRect rtAZone(rtCell);
						DrawCell(dc,sSkinDsk,(iWeekName==0 || iWeekName==6)?"DAYWEEK-NAME-HL":"DAYWEEK-NAME",rtCell,(*pDayNames)[iWeekName],DT_CENTER,0,&dwBG,&dwFG);
						if(bInteract){
							rtAZone.bottom=rtCell.top;
							CString sAction="WP:OPEN-CAL";
							/*"HINTS:(USE-NOTE-AS-EDITOR,TITLE-DISABLED,NOSIZE,NOMOVE,REDRAW-AFTER-EDIT,FULL-WND-EDIT)";
							sAction+=Format("X:(%i),Y:(%i),W:(%i),H:(%i)",rtAZone.left,rtAZone.top,rtAZone.Width(),rtAZone.Height());
							sAction+=Format("FG:(%i),BG:(%i),",dwFG,dwBG);
							sAction+=Format("EDIT-KEY:(CallX[Pickers.dll:GetDateX])");
							sAction+=Format("EDIT-FILE:(%s)",sNotesFile);*/
							sAction+=Format("<TOOLTIP>%s</TOOLTIP>",_l("Double-click to popup calendar for 4 months and add note to future date"));
							pCallback->AddActiveZone(&rtAZone, sAction);
						}
						if(i==6){
							rtFlowRect.top=rtCell.top;
							rtFlowRect.top+=2*dwOffset;
						}
						iWeekName=(iWeekName+1)%7;
					}
				}
				CRect rtWholeCalendarRectPrePost(rtFlowRect);
				int iOOIMonth=1;
				if(dtStart.GetMonth()!=dt.GetMonth()){
					iOOIMonth=0;
				}
				char szTmp[128]={0};
				COleDateTime dtCurrent=dtStart;
				double iCellYS=double(rtFlowRect.Height())/dwWeeksToShow;
				int iWeeks=0;
				DWORD dwVerticalAddition=0;
				while(iWeeks<dwWeeksToShow){
					for(int iDays=0;iDays<7;iDays++){
						sSkinDsk=sSkinOriginal;
						ArithReplace(sSkinDsk,"<DATE>",Format("%i",dtCurrent.GetDay()));
						ArithReplace(sSkinDsk,"<MONTH>",Format("%i",dtCurrent.GetMonth()));
						ArithReplace(sSkinDsk,"<WEEKDAY>",Format("%i",dtCurrent.GetDayOfWeek()));
						CRect rtCell(rtFlowRect);
						rtCell.left=long(rtFlowRect.left+iDays*iCellXS+dwOffset);
						rtCell.right=long(rtFlowRect.left+iDays*iCellXS+iCellXS-dwOffset);
						rtCell.top=long(rtFlowRect.top+iWeeks*iCellYS+dwOffset)+dwVerticalAddition;
						rtCell.bottom=long(rtFlowRect.top+iWeeks*iCellYS+iCellYS-dwOffset)+dwVerticalAddition;
						CRect rtCellOriginal(rtCell);
						CString sType="CELL";
						SYSTEMTIME sysTime;
						CString sCellTitle=dwCellsNoNums?"":Format("%02lu",dtCurrent.GetDay())+" ";
						CString sCellTitleEx;
						dtCurrent.GetAsSystemTime(sysTime);
						GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&sysTime,0,szTmp,sizeof(szTmp));
						sCellTitleEx=szTmp;
						sCellTitleEx+=" ";
						CString sTooltipPostfix="";
						CString sOnDatePostfix="";
						{// Работаем с напоминаниями...
							int iCount=0;
							char szDate[64]={0};
							strcpy(szDate,CDataXMLSaver::OleDate2Str(dtCurrent));
							HINSTANCE hRemin=GetModuleHandle("WCRemind.cml");
							if(hRemin){
								_GetCountOnDate fp2=(_GetCountOnDate)GetProcAddress(hRemin,"GetCountOnDate");
								if(fp2){
									iCount=(*fp2)(szDate);
								}
								if(iCount>0){
									_GetReminderOnDate fp3=(_GetReminderOnDate)GetProcAddress(hRemin,"GetReminderOnDate");
									if(fp3){
										sTooltipPostfix+="\r\n";
										/*if(iCount>1){
											sTooltipPostfix+=_l("Alerts");
											sTooltipPostfix+=":";
										}*/
										for(int i=0;i<iCount;i++){
											CWPReminder rem;
											if(!(*fp3)(szDate,i,rem)){
												break;
											}
											sTooltipPostfix+="\r\n";
											CString sDsc=TrimMessage(rem.szText,30,1);
											if(sDsc==""){
												sDsc=_l("No description");
											}
											sTooltipPostfix+=Format("%02i:%02i %s",rem.EventTime.wHour,rem.EventTime.wMinute,sDsc);
										}
									}
								}
							}
							sOnDatePostfix=Format("ONDATE:(%i)",iCount);
						}
						CString sThisDayHKey=Format("\n%i/%i*",dtCurrent.GetDay(),dtCurrent.GetMonth());
						CString sHolidayDsc=CDataXMLSaver::GetInstringPart(sThisDayHKey,"\n",sHolidaysContent);
						CString sThisDayHFKey=Format("\n%i_%i/%i*",dtCurrent.GetDayOfWeek()-1,int(dtCurrent.GetDay()/7+1),dtCurrent.GetMonth());
						CString sHolidayDscF=CDataXMLSaver::GetInstringPart(sThisDayHFKey,"\n",sHolidaysContent);
						CString sThisDayNKey=Format("%02i/%i/%i ",dtCurrent.GetDay(),dtCurrent.GetMonth(),dtCurrent.GetYear());
						CString sDayNote=CDataXMLSaver::GetInstringPart(sThisDayNKey,"\n",sNotesContent);
						if(dwBirthInCells){
							CString sThisDayBKey=Format("\n%i/%i ",dtCurrent.GetDay(),dtCurrent.GetMonth());
							CString sBirthNote=CDataXMLSaver::GetInstringPart(sThisDayBKey,"\n",sBirthContent);
							if(sBirthNote!=""){
								if(sDayNote!=""){
									sDayNote+="\n";
								}
								sDayNote+=sBirthNote;
							}
						}
						if(sHolidayDsc!=""){
							if(!dwCellsNumOnly){
								sCellTitle+=sHolidayDsc;
							}
							sCellTitleEx+=sHolidayDsc;
							sCellTitleEx+=" ";
						}
						if(sHolidayDscF!=""){
							if(sHolidayDsc!=""){
								if(!dwCellsNumOnly){
									sCellTitle+=", ";
								}
								sCellTitleEx+=", ";
							}
							if(!dwCellsNumOnly){
								sCellTitle+=sHolidayDscF;
							}
							sCellTitleEx+=sHolidayDscF;
							sCellTitleEx+=" ";
						}
						if(sTooltipPostfix!=""){
							if(sDayNote!=""){
								sTooltipPostfix.TrimLeft();
								sDayNote+="\r\n";
							}
							sDayNote+=sTooltipPostfix;
						}
						if(bAllowOOI && (iOOIMonth==0 || (iOOIMonth==2 && dwWeeksToShow<=6) || (iOOIMonth>=3 && dwWeeksToShow<=10)) || (iOOIMonth>dwMonthsToShow && dwMonthsToShow>0)
							|| (iOOIMonth==2 && dwMonthsToShow==1) || (iOOIMonth==3 && dwMonthsToShow==2)){
							sType="OOI-CELL";
						}else if(dtCurrent.GetDayOfWeek()==1 || dtCurrent.GetDayOfWeek()==7){
							sType="CELL-HL";
						}else if(sHolidayDsc!="" || sHolidayDscF!=""){
							sType="CELL-CB";
						}
						DWORD dwBG=0,dwFG=0;
						DWORD dwCellBG=0, dwCellFG=0;
						CString sCustomIcon;
						if(sDayNote!=""){
							dwBG=0;
							dwFG=0;
							sDayNote.Replace("<br>","\r\n");
							CString sSysInfo=CDataXMLSaver::GetInstringPart("[?CDATA{","}?DATAC:)]",sDayNote);
							if(sSysInfo!=""){
								dwFG=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("FG:(",")",sSysInfo));
								dwBG=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BG:(",")",sSysInfo));
								sCustomIcon=CDataXMLSaver::GetInstringPart("ICON:(",")",sSysInfo);
								sDayNote.Replace(Format("[?CDATA{%s}?DATAC:)]",sSysInfo),"");
							}
							if(sDayNote==""){
								// Сбрасываем цвета нафиг...
								dwBG=0;
								dwFG=0;
							}
						}
						BOOL bTodayCell=(dtCurrent==dt);
						CRect rtBeforeCellDraw(rtCell);
						dwCellBG=dwBG;
						dwCellFG=dwFG;
						BOOL bNoGetCurColor=0;
						if(bTodayCell && dwBG==dwFG){
							bNoGetCurColor=1;
							CString sBlock=CDataXMLSaver::GetInstringPart("<CELL-TODAY>","</CELL-TODAY>",sSkinDsk);
							dwCellBG=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BG-COLOR:(",")",sBlock));
							dwCellFG=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("TEXT-COLOR:(",")",sBlock));
						}
						if(dwCellsNumOnly){
							sCellTitle.TrimLeft();
							sCellTitle.TrimRight();
						}
						{// Рисуем основную ячейку
							CString sCellsAddon;
							if(dtCurrent<dtToday && sCalPastOverlay!=""){
								sCellsAddon+="<SPECIAL-ADDON>ICO-OVERLAY-EX:("+sCalPastOverlay+");ICO-OVERLAY-EX-HINT:("+sCalPastOverlayHint+");</SPECIAL-ADDON>";
							}
							DrawCell(dc,sSkinDsk+"LIMIT-BOTTOM-TOTEXT:(1)"+sCellsAddon+sOnDatePostfix,sType,rtCell,sCellTitle,DT_LEFT|(sDayNote==""?DT_WORDBREAK:0),0,&dwCellBG,&dwCellFG,0,bTodayCell?(const char*)0:(const char*)sCustomIcon);
						}
						if(bNoGetCurColor){
							dwBG=dwCellBG;
							dwFG=dwCellFG;
						}
						if(bTodayCell){
							dwCellBG=dwBG;
							dwCellFG=dwFG;
							DrawCell(dc,sSkinDsk+"LIMIT-BOTTOM-TOTEXT:(1)","CELL-TODAY",rtBeforeCellDraw,sCellTitle,DT_LEFT|(sDayNote==""?DT_WORDBREAK:0),0,&dwCellBG,&dwCellFG,0,sCustomIcon);
						}
						if(sDayNote!=""){
							sDayNote.TrimLeft();
							if(dwBG!=dwFG){
								dwCellBG=dwBG;
								dwCellFG=dwFG;
							}
							DrawCell(dc,sSkinDsk+CString(bUseWrapNotes?"TEXT-WRAP:(1)":"")+"<SPECIAL-ADDON>WARN-CLIP:(1)</SPECIAL-ADDON>","NOTE",rtCell,sDayNote,DT_LEFT,0,&dwCellBG,&dwCellFG);
						}
						if(bInteract){
							if(!bNoGetCurColor){
								dwFG=dwCellFG;
								dwBG=dwCellBG;
							}
							CString sAction="HINTS:(USE-NOTE-AS-EDITOR,TITLE-DISABLED,NOSIZE,NOMOVE,REDRAW-AFTER-EDIT,FULL-WND-EDIT)";
							sAction+=Format("X:(%i),Y:(%i),W:(%i),H:(%i)",rtCellOriginal.left,rtCellOriginal.top,rtCellOriginal.Width(),rtCellOriginal.Height());
							szTmp[0]=0;
							SYSTEMTIME sysTime;
							dtCurrent.GetAsSystemTime(sysTime);
							GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&sysTime,0,szTmp,sizeof(szTmp));
							sAction+=Format("FG:(%i),BG:(%i),",dwFG,dwBG);
							sAction+=Format("TITLE:(%s: %s)",_l("Date"),szTmp);
							sAction+=Format("EDIT-FILE:(%s)",sNotesFile);
							sAction+=Format("EDIT-KEY:(%s)",sThisDayNKey);
							sAction+=Format("EDIT-ON-DATE:(%s)",CDataXMLSaver::OleDate2Str(dtCurrent));
							sDayNote.TrimLeft();
							sDayNote.TrimRight();
							sCellTitleEx.TrimLeft();
							sCellTitleEx.TrimRight();
							if(sDayNote!="" || sCellTitleEx!=""){
								if(bTodayCell){
									sCellTitleEx+="; ";
									sCellTitleEx+=_l("Today");
								}
								sAction+=Format("<TOOLTIP>%s</TOOLTIP>",sCellTitleEx+"\r\n"+sDayNote);
							}
							pCallback->AddActiveZone(&rtCellOriginal, sAction);
						}
						COleDateTime dtCurrentNext=dtCurrent+COleDateTimeSpan(1,0,0,0);
						if(dtCurrent.GetMonth()!=dtCurrentNext.GetMonth()){
							iOOIMonth++;
							if(iWeeks!=dwWeeksToShow-1){
								if(iOOIMonth>1 && dwWeeksToShow>2 && !((iOOIMonth==2 && dwWeeksToShow<=6) || (iOOIMonth>=3 && dwWeeksToShow<=10))){
									if(dwMonthsToShow>0 && iOOIMonth>dwMonthsToShow){
										// Делаем эту нежелю последней
										iWeeks=dwWeeksToShow;
									}else{
										// Подрисовываем название месяца!
										CRect rtCell(rtFlowRect);
										rtCell.top=long(rtFlowRect.top+(iWeeks+1)*iCellYS);
										CRect rtCellOrig(rtCell);
										char szTmp[128]={0};
										SYSTEMTIME st;
										dtCurrentNext.GetAsSystemTime(st);
										GetDateFormat(LOCALE_USER_DEFAULT,0,&st,"MMMM yyyy",szTmp,sizeof(szTmp));
										DrawCellChex(dc,sSkinDsk,"SUBTITLE",rtCell,szTmp,DT_CENTER);
										dwVerticalAddition+=2*(rtCell.top-rtCellOrig.top)+dwOffset*4;
										if(iDays!=6){
											iWeeks++;
										}
										// Пересчет...
										iCellYS=double(rtFlowRect.Height()-dwVerticalAddition)/dwWeeksToShow;
									}
								}
							}
						}
						dtCurrent=dtCurrentNext;
					}
					iWeeks++;
				}
				DrawCellChex(dc,sSkinDsk,"PRE-POST-WATERMARK",rtWholeCalendarRectPrePost,"",DT_CENTER);
				DrawCellChex(dc,sSkinDsk,"POST-WATERMARK",rtWholeCalendarRectPost,"",DT_CENTER);
				CBitmap* bmpTest3=dc.SelectObject(bmOld);
			}
		}
	}
	if(iFrom==-1){
		iFrom=0;
	}
	while(iFrom>=0){
		sBlock=CDataXMLSaver::GetInstringPart("<text-in-rect>","</text-in-rect>",sScript,iFrom);
		if(sBlock!=""){
			// Текст в прямоугольнике
			long lOP=100;
			CRect rtFull;
			CString sSubTitle=CDataXMLSaver::GetInstringPart("<SUBTITLE>","</SUBTITLE>",sBlock);
			CString sText=CDataXMLSaver::GetInstringPart("<text-to-draw>","</text-to-draw>",sBlock);
			wchar_t* wszText=0;
			wchar_t* wszTextOrig=0;
			if(sText==""){
				sText=CDataXMLSaver::GetInstringPart("<textline-from-ufile>","</textline-from-ufile>",sBlock);
				if(sText!=""){
					USES_CONVERSION;
					// Юникодный файл!!!
					wchar_t* wszTextAll=0;
					ReadFile(sText,wszTextAll);
					sText="";
					if(wszTextAll!=0){
						int iRandomPos=rnd(-20,wcslen(wszTextAll));
						if(iRandomPos<2){
							iRandomPos=2;
						}
						wchar_t* wszTextB=wcsstr(wszTextAll+iRandomPos,A2W("\r\n\r\n"));
						if(wszTextB){
							wszTextB+=4;
							wchar_t* wszTextE=wcsstr(wszTextB,A2W("\r\n\r\n"));
							if(wszTextE){
								int iNewLineSize=wszTextE-wszTextB;
								wszText=new wchar_t[iNewLineSize+1];
								memset(wszText,0,(iNewLineSize+1)*sizeof(wchar_t));
								memcpy(wszText,wszTextB,iNewLineSize*sizeof(wchar_t));
								wszTextOrig=wszText;
							}
						}
						delete[] wszTextAll;
					}
				}
			}
			int iFrom2=0;
			while(iFrom2>=0){
				CString sReplaceWhat=CDataXMLSaver::GetInstringPart("<text-replace-what>","</text-replace-what>",sBlock,iFrom2);
				CString sReplaceWith=CDataXMLSaver::GetInstringPart("<text-replace-with>","</text-replace-with>",sBlock,iFrom2);
				if(sReplaceWhat!=""){
					sText.Replace(sReplaceWhat,sReplaceWith);
				}
			}
			iFrom2=0;
			while(iFrom2>=0){
				CString sReplaceFrom=CDataXMLSaver::GetInstringPart("<text-extract-from>","</text-extract-from>",sBlock,iFrom2);
				CString sReplaceTo=CDataXMLSaver::GetInstringPart("<text-extract-to>","</text-extract-to>",sBlock,iFrom2);
				if(sReplaceFrom!=""){
					sText=CDataXMLSaver::GetInstringPart(sReplaceFrom,sReplaceTo,sText);
				}
			}
			iFrom2=0;
			while(iFrom2>=0){
				CString sReplaceFrom=CDataXMLSaver::GetInstringPart("<text-skip-from>","</text-skip-from>",sBlock,iFrom2);
				CString sReplaceTo=CDataXMLSaver::GetInstringPart("<text-skip-to>","</text-skip-to>",sBlock,iFrom2);
				if(sReplaceFrom!=""){
					CString sSkipText=CDataXMLSaver::GetInstringPart(sReplaceFrom,sReplaceTo,sText);
					CString sTextToReplace=Format("%s%s%s",sReplaceFrom,sSkipText,sReplaceTo);
					sText.Replace(sTextToReplace,"");
				}
			}
			if(sBlock.Find("<text-transform-tags-to-text/>")!=-1){
				sText.Replace("<br>","\n");
				sText.Replace("<br/>","\n");
				sText.Replace("<br />","\n");
				sText.Replace("&#39;","'");
				sText.Replace("&39;","'");
				sText.Replace("&lt;","<");
				sText.Replace("&gt;",">");
				sText.Replace("&amp;","&");
			}
			// Убить теги?
			if(sBlock.Find("<text-transform-skiptags/>")!=-1){
				iFrom2=0;
				while(iFrom2>=0 && iFrom2<sText.GetLength()){
					CString sReplaceWhat=CDataXMLSaver::GetInstringPart("<",">",sText,iFrom2);
					if(sReplaceWhat!=""){
						CString sTextToReplace=Format("<%s>",sReplaceWhat);
						sText.Replace(sTextToReplace,"");
						iFrom2-=strlen(sTextToReplace);
					}
				}
			}
			if(sBlock.Find("<text-transform-trimwhitespaces/>")!=-1){
				sText.TrimLeft();
				sText.TrimRight();
			}
			CSafeDC dc;
			CFont* pFont=0;
			dc.CreateCompatibleDC(&dcScreen);
			PrepareCommon(sBlock,lOP,rtFull,pCallback,sText);
			CRect rtOriginalFull(rtFull);
			CBitmap* bmOld=dc.SelectObject(&bmpScreenImage);
			if(sBlock.Find("<TEXT-STYLE>")==-1)
			{// Подготавливаем канву
				COLORREF lBG=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BG-COLOR:(",")",sBlock));
				COLORREF lTX=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("TEXT-COLOR:(",")",sBlock));
				CString sVAlign=CDataXMLSaver::GetInstringPart("VALIGN:(",")",sBlock);
				CString sHAlign=CDataXMLSaver::GetInstringPart("HALIGN:(",")",sBlock);
				BOOL bW2T=atol(CDataXMLSaver::GetInstringPart("W-TO-TEXT:(",")",sBlock));
				CString sShadow=CDataXMLSaver::GetInstringPart("SHADOW:(",")",sBlock);
				BOOL bShadow=atol(sShadow);
				CString sBorder=CDataXMLSaver::GetInstringPart("BORDER:(",")",sBlock);
				BOOL bBorder=atol(sBorder);
				CString sMargins=CDataXMLSaver::GetInstringPart("MARGINS:(",")",sBlock);
				long lLMargins=7,lTMargins=7,lBMargins=7,lRMargins=7;
				if(sMargins!=""){
					lLMargins=lTMargins=lBMargins=lRMargins=atol(sMargins);
				}
				CString sLMargin=CDataXMLSaver::GetInstringPart("LMARGIN:(",")",sBlock);
				if(sLMargin!=""){
					lLMargins=atol(sLMargin);
				}
				CString sRMargin=CDataXMLSaver::GetInstringPart("RMARGIN:(",")",sBlock);
				if(sRMargin!=""){
					lRMargins=atol(sRMargin);
				}
				CString sTMargin=CDataXMLSaver::GetInstringPart("TMARGIN:(",")",sBlock);
				if(sTMargin!=""){
					lTMargins=atol(sTMargin);
				}
				CString sBMargin=CDataXMLSaver::GetInstringPart("BMARGIN:(",")",sBlock);
				if(sBMargin!=""){
					lBMargins=atol(sBMargin);
				}
				// Подготавливаем все для текста
				CFont* pOldFont=0;
				CString sFont=CDataXMLSaver::GetInstringPart("TEXT_FONT:(",")",sBlock);
				if(sFont!=""){
					sFont.Replace("[","<");
					sFont.Replace("]",">");
					pFont=CreateFontFromStr(sFont,0,0,0,0);
					pOldFont=dc.SelectObject(pFont);
				}
				if(sHAlign.CompareNoCase("AUTO")==0){
					if(abs(rtFull.left-0)<abs(GetDesktopRect().right-rtFull.right)){
						sHAlign="LEFT";
					}else{
						sHAlign="RIGHT";
					}
				}
				if(sVAlign.CompareNoCase("AUTO")==0){
					if(abs(rtFull.top-0)<abs(GetDesktopRect().bottom-rtFull.bottom)){
						sVAlign="TOP";
					}else{
						sVAlign="BOTTOM";
					}
				}
				DWORD dwAddHAlign=DT_CENTER;
				if(sHAlign.CompareNoCase("LEFT")==0){
					dwAddHAlign=DT_LEFT;
				}
				if(sHAlign.CompareNoCase("RIGHT")==0){
					dwAddHAlign=DT_RIGHT;
				}
				DWORD dwAddVAlign=TXTEXT_VCENT;
				if(sVAlign.CompareNoCase("TOP")==0){
					dwAddVAlign=0;
				}
				if(sVAlign.CompareNoCase("BOTTOM")==0){
					dwAddVAlign=TXTEXT_VBOTTOM;
				}
				DWORD dwShadow=(bShadow?TXSHADOW_GLOW:0);
				if(bShadow){
					if(sShadow.Find("S")!=-1){
						dwShadow=TXSHADOW_SIMPLE;
					}
					if(sShadow.Find("B")!=-1){
						dwAddVAlign|=TXSHADOW_WIDE;
					}
					if(sShadow.Find("M")!=-1){
						dwAddVAlign|=TXSHADOW_MIXBG;
					}
				}
				if(bW2T){
					// Если надо - ужимаем прямоугольник!!!
					rtFull.DeflateRect(lLMargins,lTMargins,lRMargins,lBMargins);
					CRect rtRightFull(rtFull);
					SmartDrawText(dc,wszText?(const char*)wszText:sText,rtRightFull,0,lTX,lBG,dwAddVAlign|dwShadow|(bBorder?TXSHADOW_MIXBG:0)|(wszText?TXTEXT_UNICODE:0)|TXSHADOW_CALCRC,dwAddHAlign|DT_NOPREFIX|DT_WORDBREAK);
					if((dwAddVAlign & TXTEXT_VBOTTOM)!=0){
						long dwAddition=rtFull.bottom-rtRightFull.bottom;
						rtFull.CopyRect(rtRightFull);
						rtFull.top+=dwAddition;
						rtFull.bottom+=dwAddition;
					}else{
						rtFull.CopyRect(rtRightFull);
					}
					rtFull.InflateRect(lLMargins,lTMargins,lRMargins,lBMargins);
				}
				// рисуем фон...
				BLENDFUNCTION blendFunction;
				blendFunction.BlendFlags=0;
				blendFunction.AlphaFormat=0;
				blendFunction.BlendOp=AC_SRC_OVER;
				blendFunction.SourceConstantAlpha=(int)(double(255)*double(lOP)/double(100));
				if(bBorder){
					CSafeDC dc2;
					CBitmap bmp;
					dc2.CreateCompatibleDC(&dc);
					bmp.CreateCompatibleBitmap(&dc, rtFull.Width(), rtFull.Height());
					CBitmap* bmOld2=dc2.SelectObject(&bmp);
					CRect rtDrawField(0,0,rtFull.Width(), rtFull.Height());
					if(sBorder.Find("NOBG")==-1){
						dc2.FillSolidRect(&rtDrawField,lBG);
					}
					rtDrawField.DeflateRect(1,1,1,1);
					dc2.Draw3dRect(&rtDrawField,lTX,lTX);
					// Рисуем в оригинал с полупрозрачностью
					AlphaBlendMy(dc,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(),dc2,0,0,blendFunction);
					dc2.SelectObject(bmOld2);
				}
				// Пишем текст
				rtFull.DeflateRect(lLMargins,lTMargins,lRMargins,lBMargins);
				CString sTitleFont=CDataXMLSaver::GetInstringPart("REDLINE_FONT:(",")",sBlock);
				if(sTitleFont!="" && wszText){
					COLORREF lTX2=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("REDLINE-COLOR:(",")",sBlock));
					sTitleFont.Replace("[","<");
					sTitleFont.Replace("]",">");
					CFont* pFont2=CreateFontFromStr(sTitleFont,0,0,0,0);
					if(pFont2){
						USES_CONVERSION;
						wchar_t* wszNewLine=A2W("\r\n");
						wchar_t* wszTextTitle=wcsstr(wszText,wszNewLine);
						wchar_t wc=wszTextTitle[0];
						wszTextTitle[0]=0;
						DWORD dwTitleVAlign=dwAddVAlign;
						dwTitleVAlign&=~TXTEXT_VCENT;
						dwTitleVAlign&=~TXTEXT_VBOTTOM;
						CRect rtFullTitle(rtFull);
						CFont* pOldFont2=dc.SelectObject(pFont2);
						SmartDrawText(dc,(const char*)wszText,rtFullTitle,0,lTX2,lBG,dwTitleVAlign|dwShadow|(bBorder?TXSHADOW_MIXBG:0)|(wszText?TXTEXT_UNICODE:0)|TXTEXT_CALCDR,dwAddHAlign|DT_NOPREFIX|DT_WORDBREAK);
						dc.SelectObject(pOldFont2);
						delete pFont2;
						rtFull.top=rtFullTitle.bottom;
						wszText=wszTextTitle+wcslen(wszNewLine);
					}
				}
				SmartDrawText(dc,wszText?(const char*)wszText:sText,rtFull,0,lTX,lBG,dwAddVAlign|dwShadow|(bBorder?TXSHADOW_MIXBG:0)|(wszText?TXTEXT_UNICODE:0),dwAddHAlign|DT_NOPREFIX|DT_WORDBREAK);
				if(pOldFont){
					dc.SelectObject(pOldFont);
				}
			}else{
				DrawCell(dc,sBlock,"TEXT-STYLE",rtFull,sText);
			}
			if(pFont){
				delete pFont;
			}
			if(wszTextOrig){
				delete[] wszTextOrig;
			}
			CRect rtATop(rtOriginalFull);
			if(sSubTitle!=""){
				CString sText=sSubTitle;
				rtATop.OffsetRect(7,-15);
				rtATop.right-=7*2;
				if(sSubTitle.Find("TEXT:(")!=-1){
					sText=CDataXMLSaver::GetInstringPart("TEXT:(",")",sSubTitle);
				}
				CString sTitleStyle="";
				if(sBlock.Find("<SUBTITLE-STYLE>")!=-1){
					sTitleStyle="SUBTITLE-STYLE";
				}
				DrawCell(dc,sSubTitle+sBlock,sTitleStyle,rtATop,sText,DT_LEFT);
			}
			dc.SelectObject(bmOld);
			rPreviousBlock.CopyRect(rtOriginalFull);
		}
	}
	if(iFrom==-1){
		iFrom=0;
	}
	while(iFrom>=0){
		sBlock=CDataXMLSaver::GetInstringPart("<image-in-rect>","</image-in-rect>",sScript,iFrom);
		if(sBlock!=""){
			// Картинка в прямоугольнике!!!
			long lOP=100;
			CRect rtFull;
			PrepareCommon(sBlock,lOP,rtFull,pCallback,0,0);
			long lCols=atol(CDataXMLSaver::GetInstringPart("MOSAIC-COLS:(",")",sBlock));
			long lRows=atol(CDataXMLSaver::GetInstringPart("MOSAIC-ROWS:(",")",sBlock));
			if(lCols==0 || lRows==0){
				lRows=lCols=1;
			}
			CString sAntiCache=CDataXMLSaver::GetInstringPart("ANTI-CACHE:(",")",sBlock);
			BOOL bNoFit=atol(CDataXMLSaver::GetInstringPart("NO-FIT:(",")",sBlock)) || atol(CDataXMLSaver::GetInstringPart("NO_FIT:(",")",sBlock));
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
			CString sEffects2=CDataXMLSaver::GetInstringPart("EFFECT2:(",")",sBlock);
			sEffect+=" ";
			sEffect+=sEffects2;
			// Какую картинку....
			CString sSrc=CDataXMLSaver::GetInstringPart("<image-src>","</image-src>",sBlock);
			CString sSrcFiltered=CDataXMLSaver::GetInstringPart("<image-src-web-filter>","</image-src-web-filter>",sBlock);
			if(sSrcFiltered!=""){
				// Получаем коллекцию
				CString sSrcFilteredTag=CDataXMLSaver::GetInstringPart("<image-src-web-filter-tag>","</image-src-web-filter-tag>",sBlock);
				CString sSrcFilteredAttr=CDataXMLSaver::GetInstringPart("<image-src-web-filter-attr>","</image-src-web-filter-attr>",sBlock);
				CString sSrcFilteredMask=CDataXMLSaver::GetInstringPart("<image-src-web-filter-mask>","</image-src-web-filter-mask>",sBlock);
				int iSrcFilteredNum=atol(CDataXMLSaver::GetInstringPart("<image-src-web-filter-num>","</image-src-web-filter-num>",sBlock));
				CString sContent=pCallback->GetWebPage(sSrcFiltered);
				CStringArray aRes;
				DEBUG_INFO3("Loaded page %s: %s",sSrcFiltered,sContent,0);
				ParseForTag(sContent,sSrcFilteredTag,sSrcFilteredAttr,sSrcFilteredMask,aRes,sSrcFiltered);
				if(aRes.GetSize()>0 && iSrcFilteredNum<aRes.GetSize()){
					sSrc=aRes[iSrcFilteredNum];
				}
			}
			if(sSrc.Find("://")!=-1){
				if(sSrc.Find("wc-id://")==-1){
					DWORD dwPing1=GetTickCount();
					DWORD dwMaxPing=atol(CDataXMLSaver::GetInstringPart("MAX-PING:(",")",sBlock));
					if(dwMaxPing>0){
						CWebWorld wb;
						if(!wb.CheckConnection(sSrc,FLAG_ICC_FORCE_CONNECTION,0)){
							DWORD dwErr=GetLastError();
							continue;
						}
					}
					DWORD dwPing2=GetTickCount();
					if(dwMaxPing>0 && dwPing2-dwPing1>dwMaxPing){
						continue;
					}
					if(sAntiCache!=""){
						if(sSrc.Find("?")!=-1){
							sSrc+="&";
						}else{
							sSrc+="?";
						}
						sSrc+=sAntiCache;
					}
				}
			}
			sBlock.Replace("{image-in-rect-src}",sSrc);
			PrepareCommon(sBlock,lOP,rtFull,pCallback,0,-1);
			CSafeDC dcGlobal;
			dcGlobal.CreateCompatibleDC(&dcScreen);
			CBitmap* bmpToWorkOver=NULL;
			BOOL bNeedToEraseDCtoWorkOver=FALSE;
			if(sSrc!=""){
				bNeedToEraseDCtoWorkOver=TRUE;
				if(sSrc.Find("wc-id://")==-1){
					CBitmap* bmpExtern=_bmpX()->LoadBmpFromPath(sSrc);
					if(bmpExtern!=0){
						bmpToWorkOver=_bmp().Clone(bmpExtern);
						_bmpX()->ReleaseBmp(bmpExtern);
					}
				}else{
					HBITMAP hbmp=0;
					pCallback->GenerateFullFeaturedWallpaper(atol(sSrc.Mid(strlen("wc-id://"))),hbmp,CRect(CPoint(0,0),rtFull.Size()));
					if(bmpToWorkOver==0){
						bmpToWorkOver=new CBitmap();
					}
					bmpToWorkOver->Attach(hbmp);
				}
				if(bTile)// todo proverit
				{
					MakeImageCobble(*bmpToWorkOver,rtFull.Size(),bTile);
				}
			}else{
				bmpToWorkOver=&bmpScreenImage;
			}
			CBitmap* bmOld=dcGlobal.SelectObject(&bmpScreenImage);
			if(bmpToWorkOver){
				CRect rtFullOriginal(rtFull);
				CSize bmpSize=GetBitmapSize(bmpToWorkOver);
				if(!bNoFit){
					FitImageToRect(rtFull,bmpSize);
				}
				CSize dimFullCell(rtFull.Size());
				// Проверка на мозаичность
				CBitmap bmpTemp2;
				if(lRows>1 || lCols>1)
				{
					HBITMAP hBmpScaled=0;
					dimFullCell.cx=dimFullCell.cx/lRows;
					dimFullCell.cy=dimFullCell.cy/lCols;
					CopyBitmapToBitmap(HBITMAP(*bmpToWorkOver), hBmpScaled, bmpSize, NULL, TRUE);
					bmpTemp2.Attach(hBmpScaled);
					if(bNeedToEraseDCtoWorkOver){
						delete bmpToWorkOver;
					}
					bmpToWorkOver=&bmpTemp2;
					// Очистится при выходе из if
					bNeedToEraseDCtoWorkOver=FALSE;
				}
				
				CBitmap bmpPreSrc;
				CSafeDC dcPreSrc;
				CSafeDC dcToWorkOver;
				dcPreSrc.CreateCompatibleDC(&dcGlobal);
				dcToWorkOver.CreateCompatibleDC(&dcGlobal);
				bmpPreSrc.CreateCompatibleBitmap(&dcGlobal, rtFull.Width(), rtFull.Height());
				CBitmap* bmOld2=dcPreSrc.SelectObject(&bmpPreSrc);
				dcPreSrc.SetStretchBltMode(COLORONCOLOR);
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
				// Копируем нужный кусок оригинального битмапа
				CBitmap* bmOld3=dcToWorkOver.SelectObject(bmpToWorkOver);
				bmpSize=GetBitmapSize(bmpToWorkOver);
				for(int iX=iXStart;iX<lRows;iX++){
					for(int iY=iYStart;iY<lCols;iY++){
						CRect cell;
						cell.left=lRealXOffset+iX*dimFullCell.cx+(iInvertX?dimFullCell.cx:0);
						cell.top=lRealYOffset+iY*dimFullCell.cy+(iInvertY?dimFullCell.cy:0);
						cell.right=cell.left+(iInvertX?-1:1)*(dimFullCell.cx+1);
						cell.bottom=cell.top+(iInvertY?-1:1)*(dimFullCell.cy+1);
						int iRes=dcPreSrc.StretchBlt(cell.left, cell.top, cell.Width(), cell.Height(), &dcToWorkOver, 0, 0, bmpSize.cx, bmpSize.cy, SRCCOPY);
						if(bMirror){
							iInvertY=1-iInvertY;
						}
						if(bRndImages){
							CBitmap* bmpToDel=dcToWorkOver.SelectObject(bmOld3);
							char szPath[MAX_PATH]={0};
							pCallback->GetRndWallpaperPath(0,szPath);
							CString sPath=szPath;
							CBitmap* bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
							CBitmap* bNewBmp=_bmp().Clone(bmpExtern);
							_bmpX()->ReleaseBmp(bmpExtern);
							if(bNewBmp){
								if(bmpToDel && bNeedToEraseDCtoWorkOver){
									delete bmpToDel;
								}
								bmpToDel=bNewBmp;
								bNeedToEraseDCtoWorkOver=TRUE;
							}
							bmpSize=GetBitmapSize(bmpToDel);
							bmOld3=dcToWorkOver.SelectObject(bmpToDel);
						}
					}
					if(bMirror){
						iInvertX=1-iInvertX;
						iInvertY=iInvertYStart;
					}
				}
				CBitmap* bmpToDel=dcToWorkOver.SelectObject(bmOld3);
				if(bmpToDel && bNeedToEraseDCtoWorkOver){
					delete bmpToDel;
				};
				// Так как мы добалвяли пробел, он может испортить все....
				sEffect.TrimLeft();
				if(sEffect!=""){
					//CBitmap* bmpN=dcGlobal.SelectObject(bmpToWorkOver);
					//*bmpToWorkOver
					ApplyImageEffect(sEffect,sBlock,pCallback,dcGlobal,bmpScreenImage,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(), dcPreSrc, bmpPreSrc);
					//dcGlobal.SelectObject(bmpN);
				}else{
					DEBUG_INFO5("Coping image %i:%i (%i:%i)",rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(),0);
					// Копируем на основной канвас
					BLENDFUNCTION blendFunction;
					blendFunction.BlendFlags=0;
					blendFunction.AlphaFormat=0;
					blendFunction.BlendOp=AC_SRC_OVER;
					blendFunction.SourceConstantAlpha=(int)(double(255)*double(lOP)/double(100));
					AlphaBlendMy(dcGlobal,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(), dcPreSrc, 0, 0, blendFunction);
				}
				if(bBorder){
					dcGlobal.Draw3dRect(&rtFull,lBG,lBG);
				}
				dcPreSrc.SelectObject(bmOld2);
			}
			dcGlobal.SelectObject(bmOld);
		}
	}
	if(iFrom==-1){
		iFrom=0;
	}
	while(iFrom>=0){
		sBlock=CDataXMLSaver::GetInstringPart("<rss-in-rect>","</rss-in-rect>",sScript,iFrom);
		if(sBlock!=""){
			long lX=atol(CDataXMLSaver::GetInstringPart("X:(",")",sBlock));
			long lY=atol(CDataXMLSaver::GetInstringPart("Y:(",")",sBlock));
			long lW=atol(CDataXMLSaver::GetInstringPart("W:(",")",sBlock));
			long lH=atol(CDataXMLSaver::GetInstringPart("H:(",")",sBlock));
			long lOP=atol(CDataXMLSaver::GetInstringPart("OPACITY:(",")",sBlock));
			CRect rtFull(CPoint(lX,lY),CSize(lW,lH));
			long lHReal=lH;
			CString sRSS=CDataXMLSaver::GetInstringPart("SRC:(",")",sBlock);
			char* szPage=pCallback->GetWebPage(sRSS);
			sRSS=szPage;
			pCallback->FreeChar(szPage);
			if(sRSS==""){
				continue;
			}
			XDoc* feedDoc=parseXML(sRSS);
			XNode* feed=0;
			if(feedDoc){
				feed=feedDoc->GetRoot();
			}
			if(feed){
				CString sGlobalTitle=feed->GetChild("channel",1)->GetChild("title",1)->value;
				sGlobalTitle.TrimLeft();
				sGlobalTitle.TrimRight();
				CXMLNodes feedItems;
				feed->FindByTagName("item",TRUE,feedItems);
				CString sStyle=CDataXMLSaver::GetInstringPart("<STYLE>","</STYLE>",sBlock);
				CRect rtCell(rtFull);
				int iCount=0;
				while(rtCell.top<rtFull.bottom && iCount<feedItems.GetSize()){
					XNode* item=feedItems[iCount];
					CString sText=item->GetChild("title",1)->value;
					sText=UnescapeString(sText);
					StripTags(sText);
					DeEntitize(sText);
					sText.TrimLeft();
					sText.TrimRight();
					CString sDesk=item->GetChild("description",1)->value;
					sDesk=UnescapeString(sDesk);
					StripTags(sDesk);
					DeEntitize(sDesk);
					sDesk.TrimLeft();
					sDesk.TrimRight();
					CString sLink=item->GetChild("link",1)->value;
					sLink=UnescapeString(sLink);
					StripTags(sLink);
					DeEntitize(sLink);
					sLink.TrimLeft();
					sLink.TrimRight();
					if(sText==""){
						sText=sDesk;
					}
					if(sText!=""){
						CSafeDC dc;
						dc.CreateCompatibleDC(&dcScreen);
						CBitmap* bmOld=dc.SelectObject(&bmpScreenImage);
						if(iCount==0){
							CRect rtATop(rtFull);
							long lSX=atol(CDataXMLSaver::GetInstringPart("SUBTITLE-X-OFFSET:(",")",sBlock));
							long lSY=atol(CDataXMLSaver::GetInstringPart("SUBTITLE-Y-OFFSET:(",")",sBlock));
							rtATop.OffsetRect(lSX,lSY);
							long lSR=atol(CDataXMLSaver::GetInstringPart("SUBTITLE-R-OFFSET:(",")",sBlock));
							rtATop.right=rtFull.right+lSR;
							CString sTitleStyle="SUBTITLE-STYLE";
							DrawCell(dc,sBlock,sTitleStyle,rtATop,sGlobalTitle,DT_LEFT);
						}
						CString sMStyle=sStyle+"FITW-TO-TEXT:(1)";
						DrawCell(dc,sMStyle,0,rtCell,sText);
						// Готовим тултип
						sMStyle+="INTERACTIVITY:(1)";
						sMStyle+=CString("INTERACTIVITY_ACTION:(")+sLink+")";
						sMStyle+="INTERACTIVITY_TOOLTIP:<![CDATA["+sText+((sDesk=="")?"":(CString("\n")+sDesk))+"\n\n\n"+TrimMessage(sLink,50)+"\n"+_l("Double-click to open")+"]]>";
						PrepareCommon(sMStyle,lOP,rtCell,pCallback,sText,-1);
						rtCell.top=rtCell.bottom;
						rtCell.bottom=rtFull.bottom;
						lHReal=rtCell.top-rtFull.top;
						dc.SelectObject(bmOld);
					}
					iCount++;
				}
				if(feedDoc){
					delete feedDoc;
				}
			}
			rPreviousBlock.SetRect(lX,lY,lX+lW,lY+lHReal);
		}
	}
	if(iFrom==-1){
		iFrom=0;
	}
	while(iFrom>=0){
		sBlock=CDataXMLSaver::GetInstringPart("<hlayout>","</hlayout>",sScript,iFrom);
		if(sBlock!=""){
			CSize sizeArea=GetBitmapSize(bmpScreenImage);
			CRect rtArea(CPoint(0,0),CPoint(sizeArea.cx,sizeArea.cy));
			CRect rtAreaFull(rtArea);
			long lOP=0;
			PrepareCommon(sBlock,lOP,rtArea,pCallback,0,1);
			CSafeDC dc;
			dc.CreateCompatibleDC(&dcScreen);
			CBitmap* bmOld=dc.SelectObject(&bmpScreenImage);
			{
				// рендерим через htmlite
				int ik=sBlock.Replace("<body>",Format("<body><div style='position: absolute; left: %i; top: %i; width: %i; height: %i; overflow: none;'>",rtArea.left,rtArea.top,rtArea.Width(),rtArea.Height()));
				int ik2=sBlock.Replace("</body>","</div></body>");
				if(ik+ik2<2){
					AfxMessageBox("Can`t patch <body>...</body>!");
				}
				pCallback->RenderHTML(dc.GetSafeHdc(),rtAreaFull,sBlock,1);
			}
			dc.SelectObject(bmOld);
			// Накладываем с альфой
			/*
			// Копируем на основной канвас
			BLENDFUNCTION blendFunction;
			blendFunction.BlendFlags=0;
			blendFunction.AlphaFormat=0;
			blendFunction.BlendOp=AC_SRC_OVER;
			blendFunction.SourceConstantAlpha=(int)(double(255)*double(lOP)/double(100));
			AlphaBlendMy(dcGlobal,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(), dcPreSrc, 0, 0, blendFunction);
			*/
		}
	}
	if(iFrom==-1){
		iFrom=0;
	}
	while(iFrom>=0){
		sBlock=CDataXMLSaver::GetInstringPart("<styled-rect>","</styled-rect>",sScript,iFrom);
		if(sBlock!=""){
			long lOP=100;
			CRect rtFull;
			CString sText=CDataXMLSaver::GetInstringPart("<TEXT>","</TEXT>",sBlock);
			CSafeDC dc;
			dc.CreateCompatibleDC(&dcScreen);
			PrepareCommon(sBlock,lOP,rtFull,pCallback,sText);
			if(!rtFull.IsRectEmpty()){
				CBitmap* bmOld=dc.SelectObject(&bmpScreenImage);
				DrawCell(dc,sBlock,0,rtFull,sText);
				dc.SelectObject(bmOld);
			}
		}
	}
	bmpScreenImage.Detach();
	return TRUE;
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

static DWORD dwTempForPreserve=0;
BOOL NOPRESERVE(BYTE* X, DWORD dwPreserveBrMin, DWORD dwPreserveBrMax)
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

BOOL NOPRESERVE(CDib& dibDst,int x,int y,DWORD dwPreserveBrMin,DWORD dwPreserveBrMax)
{
	if((dwPreserveBrMin==0 && dwPreserveBrMax==0) || x<0 || y<0){
		return TRUE;
	}
	BYTE* dst=(BYTE*)(dibDst.m_Bits+(y*dibDst.m_Size.cx)+x);
	return NOPRESERVE(dst,dwPreserveBrMin,dwPreserveBrMax);
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