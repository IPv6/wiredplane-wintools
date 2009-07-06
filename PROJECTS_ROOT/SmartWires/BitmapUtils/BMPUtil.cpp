// SupportClasses.cpp: implementation of the SupportClasses class.
// !!!
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BMPUtil.h"
#include "..\SystemUtils\SupportClasses.h"
#include "..\SystemUtils\InternetUtils.h"
#include "..\SystemUtils\Debug.h"
#include <math.h>
#define GDI_KEEPER
//#define GDI_NOICONS

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void Warn(const char* szT, const char* szTitle)
{_XLOG_
#ifdef _DEBUG
	MessageBox(NULL,szT,szTitle,MB_ICONINFORMATION|MB_OK|MB_TOPMOST); 
#endif
}

void GetExeFileIcon(const char* sProcExePath, HICON& hIcon, BOOL bRawGet, BOOL bUseSH)
{_XLOG_
	#ifdef GDI_NOICONS
		return;
	#endif
	#ifdef GDI_KEEPER
	if(isWin98() && !bRawGet){_XLOG_
		return;
	}
	#endif
	if(strlen(sProcExePath)==0){_XLOG_
		ClearIcon(hIcon);
		return;
	}
	CString sFileIcon=sProcExePath;
	SubstEnvVariables(sFileIcon);
	if(ExtractInlineScript(sFileIcon)!=""){_XLOG_
		sFileIcon=GetAppExe();
	}

	if(bUseSH){_XLOG_
		if(sFileIcon.Find("http://")==0){_XLOG_
			sFileIcon=getBrowserExePath();
		}
		SHFILEINFO FInfo;
		::SHGetFileInfo(sFileIcon,0,&FInfo,sizeof(FInfo),SHGFI_ICON);
		hIcon=FInfo.hIcon;
	}else{_XLOG_
		CString sExt=GetPathPart(sFileIcon,0,0,0,1);
		sExt.MakeLower();
		if(sExt.Find(".exe")!=0 && sExt.Find(".dll")!=0){// Бывает .dll,31
			sFileIcon=::GetExtensionAssociatedAction(sExt,"");
		}
		int iIconNum=0;
		int iPos=sFileIcon.ReverseFind(',');
		if(iPos>sFileIcon.ReverseFind('\\')){_XLOG_
			iIconNum=atol(sFileIcon.Mid(iPos+1));
			sFileIcon=sFileIcon.Left(iPos);
		}
		::ExtractIconEx(sFileIcon,iIconNum,0,&hIcon,1);
	}
	return;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
//
// Functions for smooth bitmap resize
//
// Improvement: float calculations changed to int.
//
// Ivaylo Byalkov, January 24, 2000
// e-mail: ivob@i-n.net
//
///////////////////////////////////////////////////////////
// helper function prototypes
static BITMAPINFO *PrepareRGBBitmapInfo(WORD wWidth, 
                                        WORD wHeight);

static void ShrinkDataInt(BYTE *pInBuff, 
                          WORD wWidth, 
                          WORD wHeight,
                          BYTE *pOutBuff, 
                          WORD wNewWidth, 
                          WORD wNewHeight,
						  DWORD dwEffects);

static void EnlargeDataInt(BYTE *pInBuff, 
                           WORD wWidth, 
                           WORD wHeight,
                           BYTE *pOutBuff, 
                           WORD wNewWidth, 
                           WORD wNewHeight,
						   DWORD dwEffects);

///////////////////////////////////////////////////////////
// Main resize function

HBITMAP ScaleBitmapInt(HBITMAP hBmp, 
                       WORD wNewWidth, 
                       WORD wNewHeight,DWORD dwEffects)
{_XLOG_
	BITMAP bmp;
	::GetObject(hBmp, sizeof(BITMAP), &bmp);
	
	// check for valid size
	if((bmp.bmWidth > wNewWidth 
		&& bmp.bmHeight < wNewHeight) 
		|| bmp.bmWidth < wNewWidth 
		&& bmp.bmHeight > wNewHeight)
		return NULL;
	// Проверяем ебанутые пропорции
	if(wNewHeight==0 || wNewWidth==0 || wNewWidth/wNewHeight>10 || wNewHeight/wNewWidth>10){
		return NULL;
	}
	
	CDesktopDC dcDesk;
	HDC hDC = (HDC)dcDesk;
	BITMAPINFO *pbi = PrepareRGBBitmapInfo((WORD)bmp.bmWidth, 
		(WORD)bmp.bmHeight);
	BYTE *pData = new BYTE[bmp.bmWidth*bmp.bmHeight*4+1024];//pbi->bmiHeader.biSizeImage];
	
	::GetDIBits(hDC, hBmp, 0, bmp.bmHeight, pData, pbi, DIB_RGB_COLORS);
	
	delete pbi;
	pbi = PrepareRGBBitmapInfo(wNewWidth, wNewHeight);
	BYTE *pData2 = new BYTE[wNewWidth*wNewHeight*4+1024];//pbi->bmiHeader.biSizeImage
	{
		if(bmp.bmWidth >= wNewWidth 
			&& bmp.bmHeight >= wNewHeight)
			ShrinkDataInt(pData, 
			(WORD)bmp.bmWidth, 
			(WORD)bmp.bmHeight,
			pData2, 
			wNewWidth, 
			wNewHeight,
			dwEffects);
		else
			EnlargeDataInt(pData, 
			(WORD)bmp.bmWidth, 
			(WORD)bmp.bmHeight,
			pData2, 
			wNewWidth, 
			wNewHeight,
			dwEffects);
	}
	delete pData;
	HBITMAP hResBmp = ::CreateCompatibleBitmap(hDC, 
		wNewWidth, 
		wNewHeight);
	
	::SetDIBits(hDC, 
		hResBmp, 
		0, 
		wNewHeight, 
		pData2, 
		pbi, 
		DIB_RGB_COLORS);
	
	::ReleaseDC(NULL, hDC);
	
	delete pbi;
	delete pData2;
	
	return hResBmp;
}

///////////////////////////////////////////////////////////

BITMAPINFO *PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight)
{_XLOG_
	BITMAPINFO *pRes = new BITMAPINFO;
	::ZeroMemory(pRes, sizeof(BITMAPINFO));
	pRes->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pRes->bmiHeader.biWidth = wWidth;
	pRes->bmiHeader.biHeight = wHeight;
	pRes->bmiHeader.biPlanes = 1;
	pRes->bmiHeader.biBitCount = 24;
	
	pRes->bmiHeader.biSizeImage = 
		((3 * wWidth + 3) & ~3) * wHeight;
	
	return pRes;
}

///////////////////////////////////////////////////////////

static int *CreateCoeffInt(int nLen, int nNewLen, BOOL bShrink)
{
	int nSum = 0, nSum2;
	int *pRes = new int[2 * nLen];
	int *pCoeff = pRes;
	int nNorm = (bShrink) 
		? (nNewLen << 12) / nLen : 0x1000;
	int	nDenom = (bShrink)? nLen : nNewLen;
	
	::ZeroMemory(pRes, 2 * nLen * sizeof(int));
	for(int i = 0; i < nLen; i++, pCoeff += 2)
	{
		nSum2 = nSum + nNewLen;
		if(nSum2 > nLen)
		{
			*pCoeff = ((nLen - nSum) << 12) / nDenom;
			pCoeff[1] = ((nSum2 - nLen) << 12) / nDenom;
			nSum2 -= nLen;
		}
		else
		{
			*pCoeff = nNorm;
			if(nSum2 == nLen)
			{
				pCoeff[1] = -1;
				nSum2 = 0;
			}
		}
		nSum = nSum2;
	}
	
	return pRes;
}

///////////////////////////////////////////////////////////

void ShrinkDataInt(BYTE *pInBuff, 
                   WORD wWidth, 
                   WORD wHeight,
                   BYTE *pOutBuff, 
                   WORD wNewWidth, 
                   WORD wNewHeight,
				   DWORD dwEffects)
{
	BYTE  *pLine = pInBuff, *pPix;
	BYTE  *pOutLine = pOutBuff;
	DWORD dwInLn = (3 * wWidth + 3) & ~3;
	DWORD dwOutLn = (3 * wNewWidth + 3) & ~3;
	int   x, y, i, ii;
	BOOL  bCrossRow, bCrossCol;
	int   *pRowCoeff = CreateCoeffInt(wWidth, 
		wNewWidth, 
		TRUE);
	int   *pColCoeff = CreateCoeffInt(wHeight, 
		wNewHeight, 
		TRUE);
	int   *pXCoeff, *pYCoeff = pColCoeff;
	DWORD dwBuffLn = 3 * wNewWidth * sizeof(DWORD);
	DWORD *pdwBuff = new DWORD[6 * wNewWidth];
	DWORD *pdwCurrLn = pdwBuff, 
		*pdwCurrPix, 
		*pdwNextLn = pdwBuff + 3 * wNewWidth;
	DWORD dwTmp, *pdwNextPix;
	
	::ZeroMemory(pdwBuff, 2 * dwBuffLn);
	
	y = 0;
	while(y < wNewHeight)
	{
		pPix = pLine;
		pLine += dwInLn;
		
		pdwCurrPix = pdwCurrLn;
		pdwNextPix = pdwNextLn;
		
		x = 0;
		pXCoeff = pRowCoeff;
		bCrossRow = pYCoeff[1] > 0;
		while(x < wNewWidth)
		{
			dwTmp = *pXCoeff * *pYCoeff;
			for(i = 0; i < 3; i++){
				DWORD adda = dwTmp * pPix[i];
				pdwCurrPix[i] += adda;
			}
			bCrossCol = pXCoeff[1] > 0;
			if(bCrossCol)
			{
				dwTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0, ii = 3; i < 3; i++, ii++)
					pdwCurrPix[ii] += dwTmp * pPix[i];
			}
			if(bCrossRow)
			{
				dwTmp = *pXCoeff * pYCoeff[1];
				for(i = 0; i < 3; i++)
					pdwNextPix[i] += dwTmp * pPix[i];
				if(bCrossCol)
				{
					dwTmp = pXCoeff[1] * pYCoeff[1];
					for(i = 0, ii = 3; i < 3; i++, ii++)
						pdwNextPix[ii] += dwTmp * pPix[i];
				}
			}
			if(pXCoeff[1])
			{
				x++;
				pdwCurrPix += 3;
				pdwNextPix += 3;
			}
			pXCoeff += 2;
			pPix += 3;
		}
		if(pYCoeff[1])
		{
			// set result line
			pdwCurrPix = pdwCurrLn;
			pPix = pOutLine;
			for(i = 3 * wNewWidth; i > 0; i--, pdwCurrPix++, pPix++){
				*pPix = ((LPBYTE)pdwCurrPix)[3];
			}
			// prepare line buffers
			pdwCurrPix = pdwNextLn;
			pdwNextLn = pdwCurrLn;
			pdwCurrLn = pdwCurrPix;
			::ZeroMemory(pdwNextLn, dwBuffLn);
			y++;
			pOutLine += dwOutLn;
		}
		pYCoeff += 2;
	}
	
	delete [] pRowCoeff;
	delete [] pColCoeff;
	delete [] pdwBuff;
	if((dwEffects & 1)!=0){
		int iSize=wNewWidth*wNewHeight;
		BYTE* pCur=pOutBuff;
		BYTE b1=pCur[0];
		BYTE b2=pCur[1];
		BYTE b3=pCur[2];
		while(iSize>0){
			int iNewR=(b1+pCur[0])>>1;
			int iNewG=(b2+pCur[1])>>1;
			int iNewB=(b3+pCur[2])>>1;
			pCur[0]=iNewR;
			pCur++;
			pCur[0]=iNewG;
			pCur++;
			pCur[0]=iNewB;
			pCur++;
			iSize--;
		}
	}
}

void EnlargeDataInt(BYTE *pInBuff, 
					WORD wWidth, 
					WORD wHeight,
					BYTE *pOutBuff, 
					WORD wNewWidth, 
					WORD wNewHeight,
					DWORD dwEffects)
{
	BYTE  *pLine = pInBuff, 
		*pPix = pLine, 
		*pPixOld, 
		*pUpPix, 
		*pUpPixOld;
	BYTE  *pOutLine = pOutBuff, *pOutPix;
	DWORD dwInLn = (3 * wWidth + 3) & ~3;
	DWORD dwOutLn = (3 * wNewWidth + 3) & ~3;
	int   x, y, i;
	BOOL  bCrossRow, bCrossCol;
	int   *pRowCoeff = CreateCoeffInt(wNewWidth, 
		wWidth, 
		FALSE);
	int   *pColCoeff = CreateCoeffInt(wNewHeight, 
		wHeight, 
		FALSE);
	int   *pXCoeff, *pYCoeff = pColCoeff;
	DWORD dwTmp, dwPtTmp[3];
	
	y = 0;
	while(y < wHeight)
	{
		bCrossRow = pYCoeff[1] > 0;
		x = 0;
		pXCoeff = pRowCoeff;
		pOutPix = pOutLine;
		pOutLine += dwOutLn;
		pUpPix = pLine;
		if(pYCoeff[1])
		{
			y++;
			pLine += dwInLn;
			pPix = pLine;
		}
		
		while(x < wWidth)
		{
			bCrossCol = pXCoeff[1] > 0;
			pUpPixOld = pUpPix;
			pPixOld = pPix;
			if(pXCoeff[1])
			{
				x++;
				pUpPix += 3;
				pPix += 3;
			}
			
			dwTmp = *pXCoeff * *pYCoeff;
			
			for(i = 0; i < 3; i++)
				dwPtTmp[i] = dwTmp * pUpPixOld[i];
			
			if(bCrossCol)
			{
				dwTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0; i < 3; i++)
					dwPtTmp[i] += dwTmp * pUpPix[i];
			}
			
			if(bCrossRow)
			{
				dwTmp = *pXCoeff * pYCoeff[1];
				for(i = 0; i < 3; i++)
					dwPtTmp[i] += dwTmp * pPixOld[i];
				if(bCrossCol)
				{
					dwTmp = pXCoeff[1] * pYCoeff[1];
					for(i = 0; i < 3; i++)
						dwPtTmp[i] += dwTmp * pPix[i];
				}
			}
			for(i = 0; i < 3; i++, pOutPix++){
				*pOutPix = ((LPBYTE)(dwPtTmp + i))[3];
			}
			
			pXCoeff += 2;
		}
		pYCoeff += 2;
	}
	
	delete [] pRowCoeff;
	delete [] pColCoeff;
	if((dwEffects & 1)!=0){
		int iSize=wNewWidth*wNewHeight;
		BYTE* pCur=pOutBuff;
		BYTE b1=pCur[0];
		BYTE b2=pCur[1];
		BYTE b3=pCur[2];
		while(iSize>0){
			int iNewR=(b1+pCur[0])>>2;
			int iNewG=(b2+pCur[1])>>2;
			int iNewB=(b3+pCur[2])>>2;
			pCur[0]=iNewR;
			pCur++;
			pCur[0]=iNewG;
			pCur++;
			pCur[0]=iNewB;
			pCur++;
			iSize--;
		}
	}
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CIconPreview, CButton)
CIconPreview::CIconPreview(DWORD* dwIconNum)
{_XLOG_
	m_IconNum=dwIconNum;
}

CIconPreview::~CIconPreview()
{_XLOG_
}

BEGIN_MESSAGE_MAP(CIconPreview, CButton)
END_MESSAGE_MAP()

CBrush IconBr(RGB(255,255,255));
void CIconPreview::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{_XLOG_
	CRect rcBounds;
	GetClientRect(&rcBounds);
	HICON hIcon=AfxGetApp()->LoadIcon(*m_IconNum);
	::DrawIconEx(lpDrawItemStruct->hDC,0,0,hIcon,rcBounds.Width(),rcBounds.Height(),0,IconBr,DI_NORMAL);
}

//
IMPLEMENT_DYNAMIC(CBitmapStatic, CButton)
BOOL CBitmapStatic::DeleteBitmap(int iType)
{_XLOG_
	if(m_hIcon[iType]){_XLOG_
		ClearIcon(m_hIcon[iType]);
		return TRUE;
	}
	return FALSE;
}

BOOL CBitmapStatic::SetBitmap(CBitmap* bmp, int iType, int iScale, int iDxMove, int iDyMove, CSize* pOutSize)
{_XLOG_
	BOOL bRes=DeleteBitmap(iType);
	BITMAP bmpInfo;
	bmpInfo.bmBits=NULL;
	bmp->GetBitmap(&bmpInfo);
	CBitmap bmpNew;
	bmpNew.CreateBitmapIndirect(&bmpInfo);
	DWORD dwCount=bmpInfo.bmHeight*bmpInfo.bmWidth*bmpInfo.bmPlanes*bmpInfo.bmBitsPixel;
	BYTE* lpBits=new BYTE[dwCount];
	bmp->GetBitmapBits(dwCount, lpBits);
	bmpNew.SetBitmapBits(dwCount, lpBits);
	CSize size(pOutSize?pOutSize->cx:bmpInfo.bmWidth,pOutSize?pOutSize->cy:bmpInfo.bmHeight);
	if(iScale!=100){_XLOG_
		long lNewCx=long(size.cx*double(iScale)/100);
		long lNewCy=long(size.cy*double(iScale)/100);
		dwWOffset=(size.cx-lNewCx)/2;
		dwHOffset=(size.cy-lNewCy)/2;
	}
	if(iDxMove!=0){_XLOG_
		dwXMove=long(size.cx*double(iDxMove)/100);
	}
	if(iDyMove!=0){_XLOG_
		dwYMove=long(size.cy*double(iDyMove)/100);
	}
	m_hIcon[iType]=CreateIconFromBitmap(&bmpNew,&size,((m_dwStyles & CBS_HIQUAL)?TRUE:FALSE));
	delete[] lpBits;
	if(bRes){_XLOG_
		Invalidate();
	}
	return TRUE;
}

BOOL CBitmapStatic::SetText(const char* szText, DWORD dwTx, DWORD dwBg)
{_XLOG_
	m_sText=szText;
	if(dwTx!=0){_XLOG_
		dwTxCol=dwTx;
	}
	if(dwBg!=0){_XLOG_
		dwBgCol=dwBg;
	}
	if(IsWindow(this->GetSafeHwnd())){_XLOG_
		Invalidate();
	}
	return TRUE;
}

BOOL CBitmapStatic::SetBitmap(DWORD dwCode, int iType, int iScale, int iDxMove, int iDyMove)
{_XLOG_
	CBitmap hBmp;
	hBmp.LoadBitmap(dwCode);
	SetBitmap(&hBmp,iType,iScale,iDxMove,iDyMove);
	return TRUE;
}

BOOL CBitmapStatic::SetBitmap(HICON hIcon, int iType)
{_XLOG_
	BOOL bRes=DeleteBitmap(iType);
	if(hIcon){_XLOG_
		m_hIcon[iType]=CopyIcon(hIcon);
		if(bRes){_XLOG_
			Invalidate();
		}
	}
	return TRUE;
}

void CBitmapStatic::SetSysBrush(int iIndex)
{_XLOG_
	BOOL bNeedRedraw=FALSE;
	if(IconBtBr){_XLOG_
		::DeleteObject(IconBtBr);
		bNeedRedraw=TRUE;
	}
	IconBtBr=::GetSysColorBrush(iIndex);
	if(bNeedRedraw){_XLOG_
		Invalidate();
	}
}

void CBitmapStatic::SetBrush(HBITMAP br)
{_XLOG_
	BOOL bNeedRedraw=FALSE;
	if(IconBtBr){_XLOG_
		::DeleteObject(IconBtBr);
		bNeedRedraw=TRUE;
	}
	IconBtBr=::CreatePatternBrush(br);
	if(bNeedRedraw){_XLOG_
		Invalidate();
	}
}

void CBitmapStatic::SetBrush(COLORREF rb)
{_XLOG_
	BOOL bNeedRedraw=FALSE;
	if(IconBtBr){_XLOG_
		::DeleteObject(IconBtBr);
		bNeedRedraw=TRUE;
	}
	IconBtBr=::CreateSolidBrush(rb);
	if(bNeedRedraw){_XLOG_
		Invalidate();
	}
}

void CBitmapStatic::Init()
{_XLOG_
	IconBtBr=NULL;
	bHoverMode=0;
	m_hIcon[CBS_BCACHE]=m_hIcon[0]=m_hIcon[1]=m_hIcon[2]=NULL;
	dwXMove=dwYMove=0;
	SetStyles(CBS_NORMAL);
	SetSysBrush(COLOR_BTNFACE);
}

CBitmapStatic::CBitmapStatic()
{_XLOG_
	Init();
}

CBitmapStatic::CBitmapStatic(CBitmap* bmp)
{_XLOG_
	Init();
	SetBitmap(bmp);
}

CBitmapStatic::CBitmapStatic(DWORD dwCode)
{_XLOG_
	Init();
	SetBitmap(dwCode);
	
}

CBitmapStatic::~CBitmapStatic()
{_XLOG_
	DeleteBitmap(0);
	DeleteBitmap(1);
	DeleteBitmap(2);
	DeleteBitmap(3);
	if(IconBtBr){_XLOG_
		::DeleteObject(IconBtBr);
		IconBtBr=NULL;
	}
}

BOOL CBitmapStatic::SetStyles(DWORD dwStyles)
{_XLOG_
	m_dwStyles=dwStyles;
	if(m_dwStyles & CBS_NORMAL){_XLOG_
		dwWOffset=dwHOffset=2;
	}else{_XLOG_
		dwWOffset=dwHOffset=0;
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CBitmapStatic, CButton)
//{{AFX_MSG_MAP(CBitmapStatic)
	ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
// Добавляем сами. в 7ой студии иначе не компилит
//{WM_MOUSEMOVE, 0, 0, 0, AfxSig_vwp, (AFX_PMSG)(AFX_PMSGW) (static_cast< void (AFX_MSG_CALL CBitmapStatic::*)(UINT, CPoint) > (OnMouseMove)) },
END_MESSAGE_MAP()
DWORD WINAPI TrackMouseOut(LPVOID pDat)
{_XLOG_
	CBitmapStatic* pTarg=(CBitmapStatic*)pDat;
	if(!pTarg || !IsWindow(pTarg->GetSafeHwnd())){_XLOG_
		return 0;
	}
	CRect rt;
	pTarg->GetWindowRect(&rt);
	CPoint pt(rt.left+1,rt.top+1);
	while(rt.PtInRect(pt)){_XLOG_
		Sleep(500);
		GetCursorPos(&pt);
	}
	if(!pTarg || !IsWindow(pTarg->GetSafeHwnd())){_XLOG_
		return 0;
	}
	pTarg->m_dwStyles|=CBS_UNHOOV;
	pTarg->Invalidate();
	return 0;
}

void CBitmapStatic::OnMouseMove(UINT nFlags, CPoint pt)
{_XLOG_
	if(bHoverMode && (m_dwStyles & CBS_UNHOOV)!=0){_XLOG_
		m_dwStyles&=~CBS_UNHOOV;
		DWORD dwTID=0;
		HANDLE h=CreateThread(0,0,TrackMouseOut,(LPVOID)this,0,&dwTID);
		CloseHandle(h);
		Invalidate();
	}
	return;
}

void CBitmapStatic::SetHooverMode(BOOL bNewMode)
{_XLOG_
	bHoverMode=bNewMode;
	if(bHoverMode){_XLOG_
		m_dwStyles|=CBS_UNHOOV;
	}else{_XLOG_
		m_dwStyles&=~CBS_UNHOOV;
	}
	Invalidate();
}

void CBitmapStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{_XLOG_
	CRect rcBounds;
	GetClientRect(&rcBounds);
	CRect rcBoundsUnCorrected(rcBounds);
	if((m_dwStyles & CBS_NONSQR)==0){_XLOG_
		int i=min(rcBounds.right,rcBounds.bottom);
		rcBounds.right=rcBounds.bottom=i;
	}
	if(m_hIcon){_XLOG_
		BOOL bNeedBG=TRUE;
		if((m_dwStyles & CBS_NOBG)==0){_XLOG_
			::FillRect(lpDrawItemStruct->hDC,&rcBounds,IconBtBr);
		}else{_XLOG_
			bNeedBG=FALSE;
		}
		if(m_dwStyles & CBS_BACKG){_XLOG_
			if(m_hIcon[CBS_BACKG_N]){_XLOG_
				::DrawIconEx(lpDrawItemStruct->hDC, rcBounds.left, rcBounds.top, m_hIcon[CBS_BACKG_N], rcBounds.Width(), rcBounds.Height(), 0, IconBtBr, DI_NORMAL);
				bNeedBG=FALSE;
			}
		}
		if(m_hIcon[CBS_NORML_N]){_XLOG_
			DWORD dImmediateWOffset=dwWOffset;
			if(m_dwStyles & CBS_ZOOMX){_XLOG_
				dImmediateWOffset=(rcBounds.Width()-rcBounds.Height())+1;
			}
			
			CRect rtImmediate(rcBounds);
			if(m_dwStyles & CBS_NOSTR){_XLOG_
				if(rtImmediate.Width()>rtImmediate.Height()){_XLOG_
					rtImmediate.right=rtImmediate.left+rtImmediate.Height();
				}
			}
			int x=rtImmediate.left+dImmediateWOffset+dwXMove;
			int y=rtImmediate.top+dwHOffset+dwYMove;
			int w=rtImmediate.Width()-2*dImmediateWOffset;
			int h=rtImmediate.Height()-2*dwHOffset;
			if((m_dwStyles & CBS_CENTLES)!=0){_XLOG_
				CSize zIco=GetIconSize(m_hIcon[0]);
				if(w>zIco.cx){_XLOG_
					int iDif=w-zIco.cx;
					w-=iDif;
					x+=iDif/2;
				}
				if(h>zIco.cy){_XLOG_
					int iDif=h-zIco.cy;
					h-=iDif;
					y+=iDif/2;
				}
			}
			::DrawIconEx(lpDrawItemStruct->hDC, x, y, m_hIcon[0], w, h, 0, bNeedBG?IconBtBr:NULL, DI_NORMAL);
			if((m_dwStyles & CBS_UNHOOV)!=0){_XLOG_
				if(m_hIcon[CBS_BCACHE]==0)
				{_XLOG_
					CDC dc;
					CBitmap bmp;
					CDesktopDC dcDesk;
					dc.CreateCompatibleDC(&dcDesk);
					bmp.CreateCompatibleBitmap(&dcDesk,w,h);
					CBitmap* bmpTmp=dc.SelectObject(&bmp);
					for(int i=x;i<x+w;i++){_XLOG_
						for(int j=y;j<y+h;j++){_XLOG_
							DWORD dwCol=GetPixel(lpDrawItemStruct->hDC,i,j);
							if(i<x+w-1 && j<y+h-1){_XLOG_
								DWORD dwColOrig2=GetPixel(lpDrawItemStruct->hDC,i+1,j);
								DWORD dwColOrig3=GetPixel(lpDrawItemStruct->hDC,i,j+1);
								dwCol=RGB((GetRValue(dwCol)+GetRValue(dwColOrig2)+GetRValue(dwColOrig3))/3,(GetGValue(dwCol)+GetGValue(dwColOrig2)+GetGValue(dwColOrig3))/3,(GetBValue(dwCol)+GetBValue(dwColOrig2)+GetBValue(dwColOrig3))/3);
							}
							dc.SetPixel(i-x,j-y,dwCol);
						}
					}
					dc.SelectObject(bmpTmp);
					m_hIcon[CBS_BCACHE]=CreateIconFromBitmap(&bmp);
				}
				::DrawIconEx(lpDrawItemStruct->hDC, x, y, m_hIcon[CBS_BCACHE], w, h, 0, bNeedBG?IconBtBr:NULL, DI_NORMAL);
				/*
				::DrawIconEx(, 1+rtImmediate.left+dImmediateWOffset+dwXMove, rtImmediate.top+dwHOffset+dwYMove, m_hIcon[0], rtImmediate.Width()-2*dImmediateWOffset, rtImmediate.Height()-2*dwHOffset, 0, NULL, DI_NORMAL);
				::DrawIconEx(lpDrawItemStruct->hDC, rtImmediate.left+dImmediateWOffset+dwXMove, 1+rtImmediate.top+dwHOffset+dwYMove, m_hIcon[0], rtImmediate.Width()-2*dImmediateWOffset, rtImmediate.Height()-2*dwHOffset, 0, NULL, DI_NORMAL);
				::DrawIconEx(lpDrawItemStruct->hDC, -1+rtImmediate.left+dImmediateWOffset+dwXMove, rtImmediate.top+dwHOffset+dwYMove, m_hIcon[0], rtImmediate.Width()-2*dImmediateWOffset, rtImmediate.Height()-2*dwHOffset, 0, NULL, DI_NORMAL);
				::DrawIconEx(lpDrawItemStruct->hDC, rtImmediate.left+dImmediateWOffset+dwXMove, -1+rtImmediate.top+dwHOffset+dwYMove, m_hIcon[0], rtImmediate.Width()-2*dImmediateWOffset, rtImmediate.Height()-2*dwHOffset, 0, NULL, DI_NORMAL);
				::DrawIconEx(lpDrawItemStruct->hDC, 1+rtImmediate.left+dImmediateWOffset+dwXMove, 1+rtImmediate.top+dwHOffset+dwYMove, m_hIcon[0], rtImmediate.Width()-2*dImmediateWOffset, rtImmediate.Height()-2*dwHOffset, 0, NULL, DI_NORMAL);
				::DrawIconEx(lpDrawItemStruct->hDC, -1+rtImmediate.left+dImmediateWOffset+dwXMove, -1+rtImmediate.top+dwHOffset+dwYMove, m_hIcon[0], rtImmediate.Width()-2*dImmediateWOffset, rtImmediate.Height()-2*dwHOffset, 0, NULL, DI_NORMAL);
				*/
			}
		}
		if(m_sText!=""){_XLOG_
			DWORD dwStyles=TXSHADOW_GLOW;
			if(m_dwStyles & CBS_NOTEF){_XLOG_
				dwStyles=0;
			}
			SmartDrawText(lpDrawItemStruct->hDC, m_sText, rcBounds, NULL, dwTxCol, dwBgCol, TXSHADOW_MIXBG|TXSHADOW_VCENT|TXSHADOW_HCENT);
		}
		if(m_dwStyles & CBS_FOREG){_XLOG_
			if(m_hIcon[CBS_FOREG_N]){_XLOG_
				::DrawIconEx(lpDrawItemStruct->hDC, rcBounds.left, rcBounds.top, m_hIcon[CBS_FOREG_N], rcBounds.Width(), rcBounds.Height(), 0, NULL, DI_NORMAL);
			}
		}
	}
	if((m_dwStyles & CBS_DDOWN) || (m_dwStyles & CBS_DDOWN2)){_XLOG_
		BOOL bHalf=0;
		CSize pSize(12,12);
		if(m_dwStyles & CBS_DDOWN2){_XLOG_
			bHalf=1;
			pSize.cx=8;
			pSize.cy=8;
		}
		HICON hh=CreateIconFromBitmap(_bmp().GetRaw("DDOWN"),bHalf?(&pSize):0);
		if(hh!=0){_XLOG_
			CRect rcBoundsOrig(rcBounds);
			rcBoundsOrig.left=rcBoundsOrig.right-pSize.cx-1;
			rcBoundsOrig.top=rcBoundsOrig.bottom-pSize.cy-1;
			::DrawIconEx(lpDrawItemStruct->hDC, rcBoundsOrig.left, rcBoundsOrig.top, hh, pSize.cx, pSize.cy, 0, NULL, DI_NORMAL);
			ClearIcon(hh);
		}
	}
	if((m_dwStyles & CBS_NORMAL)){_XLOG_
		BOOL bChecked=(lpDrawItemStruct->itemState & ODS_SELECTED);
		::DrawEdge(lpDrawItemStruct->hDC,rcBounds,bChecked?EDGE_SUNKEN:EDGE_RAISED,BF_BOTTOMLEFT|BF_TOPRIGHT);
	}
	if((m_dwStyles & CBS_BORDER)){_XLOG_
		CBrush br;
		br.CreateSolidBrush(dwBgCol);
		::FrameRect(lpDrawItemStruct->hDC,rcBounds,br);
	}
}

CBitmap* AddBackgroundToBMP(CBitmap* pBMPFrom,DWORD dwBGIcon, CSize size3)
{_XLOG_
	CBrush pBr;
	pBr.CreateSolidBrush(RGB(0,0,0));
	HICON hBG=AfxGetApp()->LoadIcon(dwBGIcon);
	CSize size=GetIconSize(hBG);
	CSize size2=GetBitmapSize(pBMPFrom);
	//
	CDC dc,dc2;
	CDesktopDC dcDesk;
	dc.CreateCompatibleDC(&dcDesk);
	dc2.CreateCompatibleDC(&dcDesk);
	CBitmap* pBMP=new CBitmap();
	pBMP->CreateCompatibleBitmap(&dcDesk, size.cx, size.cy);
	CBitmap* pOld=dc.SelectObject(pBMP);
	CBitmap* pOld2=dc2.SelectObject(pBMPFrom);
	dc.SetStretchBltMode(HALFTONE);
	dc.DrawState(CPoint(0,0),size,hBG,DST_ICON|DSS_NORMAL,&pBr);
	dc.StretchBlt(0,0,size.cx,size.cy,&dc2,0,0,size2.cx,size2.cy,SRCCOPY);
	dc2.SelectObject(pOld2);
	dc.SelectObject(pOld);
	return pBMP;
}

BOOL CopyBitmapToBitmap(HBITMAP hFrom, HBITMAP& hTo, CSize size, CSize* sizeTo, BOOL bQualitySamplyng)
{_XLOG_
	ClearBitmap(hTo);
	if(hFrom==NULL){_XLOG_
		return FALSE;
	}
	WORD xSize=(WORD)(sizeTo?sizeTo->cx:size.cx);
	WORD ySize=(WORD)(sizeTo?sizeTo->cy:size.cy);
	if((xSize!=size.cx) || (ySize!=size.cy)){_XLOG_
		if(bQualitySamplyng>0){_XLOG_
			hTo=ScaleBitmapInt(hFrom, xSize, ySize,(bQualitySamplyng==2));
		}
	}
	if(hTo==0){_XLOG_
		hTo=(HBITMAP)CopyImage(hFrom,IMAGE_BITMAP, xSize, ySize,0);//bQualitySamplyng==0?0:LR_MONOCHROME
		if(hTo==0){_XLOG_
			DWORD dwErr=GetLastError();
			TRACE("Bitmap screen error!");
		}
	}
	return TRUE;
}

HBITMAP CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor)
{_XLOG_
	HBITMAP		hMask		= NULL;
	HDC			hdcSrc		= NULL;
	HDC			hdcDest		= NULL;
	HBITMAP		hbmSrcT		= NULL;
	HBITMAP		hbmDestT	= NULL;
	COLORREF	crSaveBk;
	COLORREF	crSaveDestText;

	hMask = ::CreateBitmap(dwWidth, dwHeight, 1, 1, NULL);// Нижняя половина - XOR - будет пуста
	if (hMask == NULL)	return NULL;

	hdcSrc	= ::CreateCompatibleDC(NULL);
	hdcDest	= ::CreateCompatibleDC(NULL);

	hbmSrcT = (HBITMAP)::SelectObject(hdcSrc, hSourceBitmap);
	hbmDestT = (HBITMAP)::SelectObject(hdcDest, hMask);
	crSaveBk = ::SetBkColor(hdcSrc, crTransColor);
	::BitBlt(hdcDest, 0, 0, dwWidth, dwHeight, hdcSrc, 0, 0, SRCCOPY);
	crSaveDestText = ::SetTextColor(hdcSrc, RGB(255, 255, 255));
	::SetBkColor(hdcSrc,RGB(0, 0, 0));
	::BitBlt(hdcSrc, 0, 0, dwWidth, dwHeight, hdcDest, 0, 0, SRCAND);
	//::SetTextColor(hdcDest, crSaveDestText);
	//::SetBkColor(hdcSrc, crSaveBk);
	::SelectObject(hdcSrc, hbmSrcT);
	::SelectObject(hdcDest, hbmDestT);
	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);
	return hMask;
}

void PngTransp2White(CBitmap* pOrigBitmap)
{
	BITMAP bmpInfo;
	bmpInfo.bmBits=NULL;
	pOrigBitmap->GetBitmap(&bmpInfo);
	DWORD dwCount=bmpInfo.bmHeight*bmpInfo.bmWidth*bmpInfo.bmPlanes*bmpInfo.bmBitsPixel;
	BYTE* lpBits=new BYTE[dwCount];
	pOrigBitmap->GetBitmapBits(dwCount, lpBits);
	memset(lpBits,128,dwCount);
	pOrigBitmap->SetBitmapBits(dwCount, lpBits);
};

// Белый цвет - прозрачный!
HICON CreateIconFromBitmap(CBitmap* pOrigBitmap, CSize* pSize, BOOL bQualitySamplyng, DWORD dwEmpColor)
{_XLOG_
	if(!pOrigBitmap){_XLOG_
		return 0;
	}
	CSize sizeOrig=GetBitmapSize(pOrigBitmap);
	CSize size=sizeOrig;
	if(pSize){_XLOG_
		size=*pSize;
	}
	// Перегоняем
	HICON hIconOut=NULL;
	HBITMAP hBmpSized=NULL;
	// Ресайзим картинку
	CopyBitmapToBitmap(*pOrigBitmap,hBmpSized,size,NULL,bQualitySamplyng);
	if(hBmpSized!=NULL){_XLOG_
		// Создаем маску из оригинала и ресайзим
		HBITMAP hBmpMask=CreateBitmapMask(hBmpSized, size.cx, size.cy, dwEmpColor);//RGB(255,255,255)
		ICONINFO iconinfo;
		iconinfo.fIcon=TRUE;
		iconinfo.hbmMask=hBmpMask;
		iconinfo.hbmColor=hBmpSized;
		hIconOut=CreateIconIndirect(&iconinfo);
		ClearBitmap(hBmpMask);
		ClearBitmap(hBmpSized);
	}
	return hIconOut;
}

////////////////////
LPBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP& hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 
	
    // Retrieve the bitmap color format, width, and height. 
	int iOutBytes=GetObject(hBmp, sizeof(BITMAP), 0);
	int iBitmapSize=sizeof(BITMAP);
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)){_XLOG_
		DWORD dwErr=GetLastError();
        Warn("Error creating wallpaper (GetObject)");
		return NULL;
	}
	
    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 
	
    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 
	
	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * (1<< cClrBits)); 
	
	// There is no RGBQUAD array for the 24-bit-per-pixel format. 
	
	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER)); 
	
    // Initialize the fields in the BITMAPINFO structure. 
	
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
	
    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 
	
    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98/Me, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 
	return pbmi; 
}

BOOL CreateBMPFile(HWND hwnd, const char* pszFile, PBITMAPINFO pbi, HBITMAP hBMP) 
{ 
	if(pszFile==NULL || strlen(pszFile)==0){_XLOG_
		Warn("Error creating wallpaper (Empty file name)");
		return FALSE;
	}
	//
	HANDLE hf=NULL;
    BITMAPFILEHEADER hdr;
    PBITMAPINFOHEADER pbih=NULL;
    LPBYTE lpBits=NULL;
    DWORD dwTotal=0;
    DWORD cb=0;
    BYTE *hp=NULL;
    DWORD dwTmp=0;
	BOOL bRes=TRUE;
	try{_XLOG_
		pbih = (PBITMAPINFOHEADER) pbi; 
		lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, pbih->biSizeImage);
		if (!lpBits){_XLOG_
			Warn("Error creating wallpaper (GlobalAlloc failed)");
			throw 0;
		}
		// Retrieve the color table (RGBQUAD array) and the bits 
		// (array of palette indices) from the DIB. 
		CDC pDC;
		CDesktopDC dcDesk;
		pDC.CreateCompatibleDC(&dcDesk);
		if (!GetDIBits(pDC.GetSafeHdc(), hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
			DIB_RGB_COLORS)){_XLOG_
			Warn("Error creating wallpaper (GetDIBBitsFailed)");
			throw 0;
		}
		// Create the .BMP file. 
		hf = CreateFile(pszFile, 
			/*GENERIC_READ | */GENERIC_WRITE, 
			(DWORD) 0, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL); 
		if (hf == INVALID_HANDLE_VALUE){_XLOG_
			CString sErr;
			sErr.Format("Error creating wallpaper! File: %s (0x%X)",pszFile,GetLastError());
			Warn(sErr);
			throw 0;
		}
		hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
		// Compute the size of the entire file. 
		hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
			pbih->biSize + pbih->biClrUsed 
			* sizeof(RGBQUAD) + pbih->biSizeImage); 
		hdr.bfReserved1 = 0; 
		hdr.bfReserved2 = 0; 
		// Compute the offset to the array of color indices. 
		hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
			pbih->biSize + pbih->biClrUsed 
			* sizeof (RGBQUAD); 
		// Copy the BITMAPFILEHEADER into the .BMP file. 
		if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
			(LPDWORD) &dwTmp,  NULL)){_XLOG_
			Warn("Error creating wallpaper (WriteFile of header failed)");
			throw 0;
		}
		// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
		if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
			+ pbih->biClrUsed * sizeof (RGBQUAD), 
			(LPDWORD) &dwTmp, ( NULL))){_XLOG_
			Warn("Error creating wallpaper (WriteFile of rgb quad failed)");
			throw 0;
		}
		// Copy the array of color indices into the .BMP file. 
		dwTotal = cb = pbih->biSizeImage; 
		hp = lpBits; 
		if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)){_XLOG_
			CString sErr;
			sErr.Format("Error creating wallpaper (WriteFile of color indices failed - 0x%X)",GetLastError());
			Warn(sErr);
			throw 0;
		}
		// Close the .BMP file. 
		if (!CloseHandle(hf)){_XLOG_
			Warn("Error creating wallpaper (Close handle failed)");
			throw 0;
		}
	}catch(...){_XLOG_
		bRes=FALSE;
	}
    // Free memory. 
	if(lpBits){_XLOG_
		VERIFY(::GlobalFree((HGLOBAL)lpBits)==NULL);
	}
	return bRes;
}


HBITMAP CopyDesktop()
{
	HWND hwnd=::GetDesktopWindow(); //здесь может быть любое окно, которое мы хотим поместить в HBITMAP

	CRect r;
	::GetWindowRect(hwnd,&r);

	int xScreen,yScreen;
	int xshift = 0, yshift = 0;
	xScreen = GetSystemMetrics(SM_CXSCREEN);
	yScreen = GetSystemMetrics(SM_CYSCREEN);
	if(r.right > xScreen)
		r.right = xScreen;
	if(r.bottom > yScreen)
		r.bottom = yScreen;
	if(r.left < 0){
		xshift = -r.left;
		r.left = 0;
	}
	if(r.top < 0){
		yshift = -r.top;
		r.top = 0;
	}

	CSize sz(r.Width(), r.Height());
	if(sz.cx <= 0 || sz.cy <= 0)
		return 0;

	// bring the window at the top most level
	//::SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	// prepare the DCs
	HDC srcDC = ::GetWindowDC(hwnd);

	CDesktopDC dcScreen;
	HDC memDC = ::CreateCompatibleDC(dcScreen.dc->GetSafeHdc());
	HBITMAP bm =::CreateCompatibleBitmap(dcScreen.dc->GetSafeHdc(), sz.cx, sz.cy);
	HBITMAP oldbm = (HBITMAP)::SelectObject(memDC,bm);
	::BitBlt(memDC, 0, 0, sz.cx, sz.cy, srcDC, xshift, yshift, SRCCOPY);

	// restore the position
	//::SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	//::SetWindowPos(m_hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SelectObject(memDC,oldbm);
	DeleteObject(memDC);
	ReleaseDC(hwnd,srcDC);
	return bm;
}

BOOL SaveBitmapToDisk(HBITMAP& hbmOutImage, const char* szFile, HWND hParent)
{_XLOG_
	if(!hbmOutImage){_XLOG_
		return FALSE;
	}
	PreparePathForWrite(szFile);
	hParent=hParent?hParent:(::GetDesktopWindow());
	PBITMAPINFO pBMPInfo=CreateBitmapInfoStruct(hParent, hbmOutImage);
	if(pBMPInfo==NULL){_XLOG_
		Warn("Error creating wallpaper bitmap info struct!");
		return FALSE;
	}
	CreateBMPFile(hParent, szFile, pBMPInfo, hbmOutImage);
	LocalFree(pBMPInfo);
	return TRUE;
}

void SaveScreenshotToFile(const char* szPath, int iType)
{
	HBITMAP hbmOutImage=CopyDesktop();
	if(hbmOutImage){
		if(iType){
			SaveBitmapToJpg(szPath, hbmOutImage,80);
		}else{
			SaveBitmapToDisk(hbmOutImage, szPath);
		}
	}
}

//
//      DitherBlt :     Draw a bitmap dithered (3D grayed effect like disabled buttons in toolbars) into a destination DC
//      Author :        Jean-Edouard Lachand-Robert (iamwired@geocities.com), June 1997.
//
//      hdcDest :       destination DC
//      nXDest :        x coordinate of the upper left corner of the destination rectangle into the DC
//      nYDest :        y coordinate of the upper left corner of the destination rectangle into the DC
//      nWidth :        width of the destination rectangle into the DC
//      nHeight :       height of the destination rectangle into the DC
//      hbm :           the bitmap to draw (as a part or as a whole)
//      nXSrc :         x coordinates of the upper left corner of the source rectangle into the bitmap
//      nYSrc :         y coordinates of the upper left corner of the source rectangle into the bitmap
//
void DitherBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth, 
				int nHeight, HBITMAP hbm, int nXSrc, int nYSrc)
{_XLOG_
	ASSERT(hdcDest && hbm);
	ASSERT(nWidth > 0 && nHeight > 0);
	
	// Create a generic DC for all BitBlts
	HDC hDC = CreateCompatibleDC(hdcDest);
	ASSERT(hDC);
	
	if (hDC)
	{_XLOG_
		// Create a DC for the monochrome DIB section
		HDC bwDC = CreateCompatibleDC(hDC);
		ASSERT(bwDC);
		
		if (bwDC)
		{_XLOG_
			// Create the monochrome DIB section with a black and white palette
			struct {
				BITMAPINFOHEADER bmiHeader; 
				RGBQUAD 		 bmiColors[2]; 
			} RGBBWBITMAPINFO = {
				
				{		// a BITMAPINFOHEADER
					sizeof(BITMAPINFOHEADER),	// biSize 
						nWidth, 				// biWidth; 
						nHeight,				// biHeight; 
						1,						// biPlanes; 
						1,						// biBitCount 
						BI_RGB, 				// biCompression; 
						0,						// biSizeImage; 
						0,						// biXPelsPerMeter; 
						0,						// biYPelsPerMeter; 
						0,						// biClrUsed; 
						0						// biClrImportant; 
				},
				
				{
					{ 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }
					} 
			};
			VOID *pbitsBW;
			HBITMAP hbmBW = CreateDIBSection(bwDC,
				(LPBITMAPINFO)&RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
			ASSERT(hbmBW);
			
			if (hbmBW)
			{_XLOG_
				// Attach the monochrome DIB section and the bitmap to the DCs
				SelectObject(bwDC, hbmBW);
				SelectObject(hDC, hbm);
				
				// BitBlt the bitmap into the monochrome DIB section
				BitBlt(bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);
				
				// Paint the destination rectangle in gray
				FillRect(hdcDest, CRect(nXDest, nYDest, nXDest + nWidth, nYDest +
					nHeight), GetSysColorBrush(COLOR_3DFACE));
				
				// BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT bits in the destination DC
				// The magic ROP comes from the Charles Petzold's book
				HBRUSH hb = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdcDest, hb);
				BitBlt(hdcDest, nXDest + 1, nYDest + 1, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
				
				// BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW bits in the destination DC
				hb = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
				DeleteObject(SelectObject(hdcDest, hb));
				BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
				DeleteObject(SelectObject(hdcDest, oldBrush));
			}
			
			VERIFY(DeleteDC(bwDC));
		}
		
		VERIFY(DeleteDC(hDC));
	}
//
//	-----------------------
//	bitmap created by the CreateDIBSection call needs to be deleted with a DeleteObject(hbmBW) after all of the drawing is finished. 
//	
}


/*
// This function creates a grayscale icon starting
// from a given icon. The resulting icon will have
// the same size of the original one.
//
// Parameters:
//      [IN]    hIcon
//              Handle to the original icon.
//
// Return value:
//      If the function succeeds, the return value is 
//      the handle to the newly created grayscale icon.
//      If the function fails, the return value is NULL.
//
HICON CreateGrayscaleIcon(HICON hIcon)
{_XLOG_
  HICON       hGrayIcon = NULL;
  HDC         hMainDC = NULL, 
              hMemDC1 = NULL, 
              hMemDC2 = NULL;
  BITMAP      bmp;
  HBITMAP     hOldBmp1 = NULL,
              hOldBmp2 = NULL;
  ICONINFO    csII, csGrayII;
  BOOL        bRetValue = FALSE;

  bRetValue = ::GetIconInfo(hIcon, &csII);
  if (bRetValue == FALSE) return NULL;

  hMainDC = ::GetDC(m_hWnd);
  hMemDC1 = ::CreateCompatibleDC(hMainDC);
  hMemDC2 = ::CreateCompatibleDC(hMainDC);
  if (hMainDC == NULL || 
    hMemDC1 == NULL ||
    hMemDC2 == NULL) 
      return NULL;

  if (::GetObject(csII.hbmColor, 
                sizeof(BITMAP), &
                amp;bmp))
  {_XLOG_
    csGrayII.hbmColor = 
         ::CreateBitmap(csII.xHotspot*2,
                        csII.yHotspot*2, 
                        bmp.bmPlanes, 
                        bmp.bmBitsPixel, 
                        NULL);
    if (csGrayII.hbmColor)
    {_XLOG_
      hOldBmp1 =
         (HBITMAP)::SelectObject(hMemDC1,
                                 csII.hbmColor);
      hOldBmp2 = 
         (HBITMAP)::SelectObject(hMemDC2,
                                 csGrayII.hbmColor);

      ::BitBlt(hMemDC2, 0, 0, csII.xHotspot*2,
               csII.yHotspot*2, hMemDC1, 0, 0,
               SRCCOPY);

      DWORD    dwLoopY = 0, dwLoopX = 0;
      COLORREF crPixel = 0;
      BYTE     byNewPixel = 0;

      for (dwLoopY = 0; dwLoopY < csII.yHotspot*2; dwLoopY++)
      {_XLOG_
        for (dwLoopX = 0; dwLoopX < csII.xHotspot*2; dwLoopX++)
        {_XLOG_
          crPixel = ::GetPixel(hMemDC2, dwLoopX, dwLoopY);

          byNewPixel = (BYTE)((GetRValue(crPixel) * 0.299) +
               (GetGValue(crPixel) * 0.587) +
               (GetBValue(crPixel) * 0.114));
          if (crPixel) ::SetPixel(hMemDC2,
                                  dwLoopX,
                                  dwLoopY,
                                  RGB(byNewPixel,
                                  byNewPixel,
                                  byNewPixel));
        } // for
      } // for

      ::SelectObject(hMemDC1, hOldBmp1);
      ::SelectObject(hMemDC2, hOldBmp2);

      csGrayII.hbmMask = csII.hbmMask;

      csGrayII.fIcon = TRUE;
      hGrayIcon = ::CreateIconIndirect(&csGrayII);
    } // if

    ::DeleteObject(csGrayII.hbmColor);
    //::DeleteObject(csGrayII.hbmMask);
  } // if

  ::DeleteObject(csII.hbmColor);
  ::DeleteObject(csII.hbmMask);
  ::DeleteDC(hMemDC1);
  ::DeleteDC(hMemDC2);
  ::ReleaseDC(m_hWnd, hMainDC);

  return hGrayIcon;
} // End of CreateGrayscaleIcon
*/

// DrawGrayScale	- Draws a bitmap in gray scale
void DrawGrayScale( CDC *pDC, HANDLE hDIB)
{_XLOG_
	//A few bug
	//-------------------------------------------------------------------------------
	//Instead of 
	//UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors); 
	//it's better to write 
	//UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * (nColors-)); 
	//One PALETTEENTRY included in LOGPALETTE, thus you allocates extra space. 
	CPalette pal;
	CPalette *pOldPalette=NULL;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 1 << bmInfo.bmiHeader.biBitCount;
	// Create the palette if needed
	if( pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE && nColors <= 256 ){_XLOG_
		// The device supports a palette and bitmap has color table
		// Allocate memory for a palette
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;
		for( int i=0; i < nColors; i++)
		{_XLOG_
			long lSquareSum = bmInfo.bmiColors[i].rgbRed 
				* bmInfo.bmiColors[i].rgbRed
				+ bmInfo.bmiColors[i].rgbGreen 
				* bmInfo.bmiColors[i].rgbGreen
				+ bmInfo.bmiColors[i].rgbBlue 
				* bmInfo.bmiColors[i].rgbBlue;
			//The formula grey=sqrt(r*r+g*g+b*b) looks really suspicious - where did you get it from? All the graphics books I read say that RGB maps to brightness (=grayscale) like gray=0.299*r+0.587*g+0.114*b. 
			int nGray = (int)sqrt(((double)lSquareSum)/3);
			pLP->palPalEntry[i].peRed = nGray;
			pLP->palPalEntry[i].peGreen = nGray;
			pLP->palPalEntry[i].peBlue = nGray;
			pLP->palPalEntry[i].peFlags = 0;
		}
		pal.CreatePalette( pLP );
		delete[] pLP;
		// Select the palette
		pOldPalette = pDC->SelectPalette(&pal, FALSE);
		pDC->RealizePalette();
	}else if((pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) == 0 && nColors <= 256 ){_XLOG_
		// Device does not supports palettes but bitmap has a color table
		// Modify the bitmaps color table directly
		// Note : This ends up changing the DIB. If that is not acceptable then
		// copy the DIB and then change the copy rather than the original
		for( int i=0; i < nColors; i++)
		{_XLOG_
			long lSquareSum = bmInfo.bmiColors[i].rgbRed 
				* bmInfo.bmiColors[i].rgbRed
				+ bmInfo.bmiColors[i].rgbGreen 
				* bmInfo.bmiColors[i].rgbGreen
				+ bmInfo.bmiColors[i].rgbBlue 
				* bmInfo.bmiColors[i].rgbBlue;
			int nGray = (int)sqrt(((double)lSquareSum)/3);
			bmInfo.bmiColors[i].rgbRed = nGray;
			bmInfo.bmiColors[i].rgbGreen = nGray;
			bmInfo.bmiColors[i].rgbBlue = nGray;
		}
	}
	int nWidth = bmInfo.bmiHeader.biWidth;
	int nHeight = bmInfo.bmiHeader.biHeight;
	// Draw the image 
	LPVOID lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	if(::SetDIBitsToDevice(pDC->m_hDC,	// hDC
		0,				// XDest
		0,				// YDest
		nWidth,				// nDestWidth
		nHeight,			// nDestHeight
		0,				// XSrc
		0,				// YSrc
		0,				// nStartScan
		nHeight,			// nNumScans
		lpDIBBits,			// lpBits
		(LPBITMAPINFO)hDIB,		// lpBitsInfo
		DIB_RGB_COLORS)==0){_XLOG_
		CString sErr;
		sErr.Format("0x%X",GetLastError());
		Warn(sErr);
	}
	if(pOldPalette){_XLOG_
		pDC->SelectPalette(pOldPalette, FALSE);
	}
	LocalFree(hDIB);
}

CBmpHolder::CBmpHolder()
{_XLOG_
	LONG lDims=::GetMenuCheckMarkDimensions();
	MenuIconSize.cx=LOWORD(lDims);
	if(MenuIconSize.cx<=13){_XLOG_
		MenuIconSize.cx++;
	}
	MenuIconSize.cy=HIWORD(lDims);
	if(MenuIconSize.cy<=13){_XLOG_
		MenuIconSize.cy++;
	}
};

CBmpHolder::~CBmpHolder()
{_XLOG_
	CString sKey;
	void* ptr=NULL;
	CBitmap* pBmp=NULL;
	POSITION pos=bmpHolder.GetStartPosition();
	while(pos!=NULL){_XLOG_
		bmpHolder.GetNextAssoc(pos,sKey,ptr);
		pBmp=(CBitmap*)ptr;
		if(pBmp!=NULL){_XLOG_
			delete pBmp;
		}
	}
	bmpHolder.RemoveAll();
}

#ifdef ZIP_SUPPORT
// Для вытаскивания файлов из кеша
#include "..\SystemUtils\Zip\ZipArchive.h"
#endif

#ifdef CXIMG_SUPPORT

CXIMG::CXIMG()
{_XLOG_
	bml=0;
	hDLL=0;
}
CXIMG::~CXIMG()
{_XLOG_
	if(hDLL){_XLOG_
		FreeLibrary(hDLL);
		hDLL=0;
	}
}
BOOL CXIMG::ConvertToJpg(const char* szFrom, const char* szTo, DWORD dwQ)
{_XLOG_
	if(!hDLL){_XLOG_
		hDLL=LoadLibrary("iload.cml");
	}
	if(hDLL && !cjp){_XLOG_
		cjp=(_ConvertToJpg)DBG_GETPROC(hDLL,"ConvertToJpg");
	}
	if(cjp){_XLOG_
		return (*cjp)(szFrom,szTo,dwQ);
	}
	return FALSE;
};
HBITMAP CXIMG::Load(const char* szPath)
{_XLOG_
	CString sExt="";
	CString sTempFile;
	HBITMAP hRes=0;
	// Если загрузка с интернета или сайта...
	if(strstr(szPath,"://")!=0){_XLOG_
		sExt=szPath;
		sExt=sExt.SpanExcluding("?&");
		int iRP=sExt.ReverseFind('.');
		if(iRP>0){_XLOG_
			sExt=sExt.Mid(iRP);
		}
	}else{_XLOG_
		sExt=GetPathPart(szPath,0,0,0,1);
	}
	sExt.MakeLower();
	DWORD dwType=0;
	if(sExt==".gif"){_XLOG_
		dwType=CXIMAGE_FORMAT_GIF;
	}
	if(sExt==".tif" || sExt==".tiff"){_XLOG_
		dwType=CXIMAGE_FORMAT_TIF;
	}
	if(sExt==".pcx"){_XLOG_
		dwType=CXIMAGE_FORMAT_PCX;
	}
	if(sExt==".tga"){_XLOG_
		dwType=CXIMAGE_FORMAT_TGA;
	}
	if(sExt==".png"){_XLOG_
		dwType=CXIMAGE_FORMAT_PNG;
	}
	if(sExt==".jpg" || sExt==".jpeg"){_XLOG_
		dwType=CXIMAGE_FORMAT_JPG;
	}
	if(dwType!=0){_XLOG_
		if(!hDLL){_XLOG_
			hDLL=LoadLibrary("iload.cml");
		}
		if(hDLL && !bml){_XLOG_
			bml=(_LoadBmp)DBG_GETPROC(hDLL,"LoadBmp");
		}
		if(bml){_XLOG_
			if(strstr(szPath,"://")!=0){_XLOG_
				CWebWorld ww;
				char sTempFolder[MAX_PATH]={0};
				GetTempPath(sizeof(sTempFolder),sTempFolder);
				static int iCacheNum=1;
				sTempFile.Format("%swc_temp%lu",sTempFolder,iCacheNum++);
				ww.GetWebFile(szPath,sTempFile);
				szPath=sTempFile;
				iCacheNum++;
			}
			hRes=bml(szPath, dwType);
		}
	}
	if(sTempFile!=""){_XLOG_
		DeleteFile(sTempFile);
	}
	return hRes;
}

CXIMG& getILoad()
{_XLOG_
	static CXIMG imgLoader;
	return imgLoader;
}
#endif

BOOL CBmpHolder::ReleaseBmp(CBitmap*& bmp)
{_XLOG_
	if(bmp){_XLOG_
		delete bmp;
	}
	bmp=0;
	return TRUE;
};

CCriticalSection& GetBmpLock()
{_XLOG_
	static CCriticalSection cl;
	return cl;
}
 
CBitmap* CBmpHolder::LoadBmpFromPath(CString szPath)
{_XLOG_
	#ifdef GDI_NOICONS
		return 0;
	#endif
	DEBUG_INFO3("Loading BMP: %s",szPath,0,0);
#ifdef _DEBUG
	if(isFileExist("c:\\wallpapers.log")){_XLOG_
		if(szPath.Find("Profile")==-1 && szPath.Find("Templ")==-1 && szPath.Find("Skin")==-1){_XLOG_
			CString sLog;
			sLog.Format("%s:%x loading %s\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
			FILE* f=fopen("c:\\wallpapers.log","a+");fprintf(f,sLog);fprintf(f,"\n");fclose(f);
		}
	}
	TRACE3("%s:%x loading %s\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
#endif
	CSmartLock lN(&GetBmpLock(),TRUE);
	CString sPathLow=szPath;
#ifdef ZIP_SUPPORT
	sPathLow.MakeLower();
	if(sPathLow.Find(".zip\\")!=-1 || sPathLow.Find(".wsc\\")!=-1){_XLOG_
		TRACE3("%s:%x loading %s - zip\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
		DEBUG_INFO3("Loading BMP-zip: %s",szPath,0,0);
		return LoadBmpFromPath(CZipCacher::zipCache(szPath));
	}
#endif
	//	GDI_KEEPER - MUST NOT be there!!!
	if(szPath.GetLength()==0){_XLOG_
		return NULL;
	}
	// Если это BMP файл - грузим упрощенно
	if(stricmp(((const char*)szPath)+strlen(szPath)-4,".bmp")==0){_XLOG_
		TRACE3("%s:%x loading %s - bmp\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
		DEBUG_INFO3("Loading BMP-bmp: %s",szPath,0,0);
		HBITMAP hBmp=(HBITMAP)LoadImage(NULL,szPath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if(hBmp!=NULL){_XLOG_
			CBitmap* pResBmp=new CBitmap();
			pResBmp->Attach(hBmp);
			return pResBmp;
		}
	}
#ifdef CXIMG_SUPPORT
	// Если это Tiff/Tga или png...
	if(sPathLow.Find(".gif")!=-1 || sPathLow.Find(".tif")!=-1 || sPathLow.Find(".tga")!=-1  || sPathLow.Find(".png")!=-1  || sPathLow.Find(".jpg")!=-1  || sPathLow.Find(".jpeg")!=-1){_XLOG_
		TRACE3("%s:%x loading %s - png/tif i t.p.\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
		DEBUG_INFO3("Loading BMP-png/tga: %s",szPath,0,0);
		HBITMAP hBmp=getILoad().Load(szPath);
		if(hBmp!=NULL){_XLOG_
			CBitmap* pResBmp=new CBitmap();
			pResBmp->Attach(hBmp);
			return pResBmp;
		}
	}
#endif
	BOOL bRes=TRUE;
	HBITMAP hbmRealImage=NULL;
	CSmartOleInitialize cInit;
	try{_XLOG_
		TRACE3("%s:%x loading %s - ipicture\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
		DEBUG_INFO3("Loading BMP-ole: %s",szPath,0,0);
		HRESULT hr=-1;
		USES_CONVERSION;
		CComPtr<IPicture> m_pPicture=NULL;
		WCHAR szWPath[MAX_PATH*3]={0};
		wcscpy(szWPath,A2W(szPath));
		//FLOG3("%lu: %lu Calling OleLoad for '%s'",GetTickCount(),GetCurrentThreadId(),szPath);
		hr = OleLoadPicturePath(szWPath,NULL,0,0,IID_IPicture,(void**)&m_pPicture);
		//FLOG3("%lu: %lu OleLoad for '%s' finished!",GetTickCount(),GetCurrentThreadId(),szPath);
		if(m_pPicture==NULL){_XLOG_
			//TRACE2("%s:%x loading %s - error 1!\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
			DEBUG_INFO3("Loading BMP-err1: %s",szPath,0,0);
			throw hr;
		}
		CDesktopDC dcDesk;
		HDC hdcCompatibleOrig = CreateCompatibleDC(dcDesk); 
		long hmWidth = 0, hmHeight = 0;
		m_pPicture->get_Width(&hmWidth);
		m_pPicture->get_Height(&hmHeight);
		CSize sizeOfRealImage;
		sizeOfRealImage.cx = MAP_LOGHIM_TO_PIX(hmWidth,::GetDeviceCaps(hdcCompatibleOrig, LOGPIXELSX));
		sizeOfRealImage.cy = MAP_LOGHIM_TO_PIX(hmHeight,::GetDeviceCaps(hdcCompatibleOrig, LOGPIXELSY));
		TRACE3("%s loaded %i:%i\n",COleDateTime::GetCurrentTime().Format("%x"),sizeOfRealImage.cx,sizeOfRealImage.cy);
		DEBUG_INFO3("Loading BMP: %s - ok!",szPath,0,0);
		// рисуем в битмап с оригинальными размерами
		CRect rcPictOrigSize(CPoint(0,0),sizeOfRealImage);
		hbmRealImage = CreateCompatibleBitmap(dcDesk, rcPictOrigSize.Width(), rcPictOrigSize.Height()); 
		if(hbmRealImage==0){_XLOG_
			Warn(CString("Error loading image:")+Format(GetLastError()));
			throw 0;
		}
		HGDIOBJ hTmpBmp=SelectObject(hdcCompatibleOrig, hbmRealImage);
		// Рисуем в него
		hr=m_pPicture->Render(
			hdcCompatibleOrig,			// [in] Handle of device context on which to render the image
			rcPictOrigSize.left,		// [in] Horizontal position of image in hdc
			rcPictOrigSize.top,			// [in] Vertical position of image in hdc
			rcPictOrigSize.Width(),		// [in] Horizontal dimension of destination rectangle
			rcPictOrigSize.Height(),	// [in] Vertical dimension of destination rectangle
			0,hmHeight,hmWidth,-hmHeight,
			&rcPictOrigSize				// [in, optional] Pointer to position of destination for a metafile hdc
			);
		SelectObject(hdcCompatibleOrig,hTmpBmp);
		DeleteObject(hdcCompatibleOrig);
		if(!SUCCEEDED(hr)){_XLOG_
			throw hr;
		}
	}catch(HRESULT){_XLOG_
		DEBUG_INFO3("Loading BMP-ole exeptionA: %s",szPath,0,0);
		TRACE3("%s:%x loading %s - ipicture err A\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
	}catch(...){_XLOG_
		DEBUG_INFO3("Loading BMP-ole exeptionB: %s",szPath,0,0);
		TRACE3("%s:%x loading %s - ipicture err B\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
	}
	if(hbmRealImage){_XLOG_
		DEBUG_INFO3("Loading BMP-ole ok: %s",szPath,0,0);
		TRACE3("%s:%x loading %s - ipicture OK\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
		CBitmap* pResBmp=new CBitmap();
		pResBmp->Attach(hbmRealImage);
		return pResBmp;
	}
	DEBUG_INFO3("Loading BMP-ole exeptionC: %s",szPath,0,0);
	TRACE3("%s:%x loading %s - ipicture err C\n",COleDateTime::GetCurrentTime().Format("%x"),GetCurrentThreadId(),szPath);
	return NULL;
}

CBitmap* CBmpHolder::LoadBmpCached(const char* szPath)
{_XLOG_
	CString sKey=CString("PATH:")+szPath;
	CBitmap* pRes=Get(sKey);
	if(pRes){_XLOG_
		return pRes;
	}
	pRes=LoadBmpFromPathExt(szPath);
	bmpHolder.SetAt(sKey,(void*)pRes);
	return pRes;

}

CBitmap* CBmpHolder::LoadBmpFromPathExt(const char* szPath)
{_XLOG_
	CBitmap* bmp=0;
	if(isFileExist(szPath)){_XLOG_
		bmp=LoadBmpFromPath(szPath);
	}
	if(!bmp){_XLOG_
		bmp=LoadBmpFromPath(Format("%s.jpg",szPath));
	}
	if(!bmp){_XLOG_
		bmp=LoadBmpFromPath(Format("%s.gif",szPath));
	}
	if(!bmp){_XLOG_
		bmp=LoadBmpFromPath(Format("%s.bmp",szPath));
	}
	if(!bmp){_XLOG_
		bmp=LoadBmpFromPath(Format("%s.ico",szPath));
	}
	if(!bmp){_XLOG_
		bmp=LoadBmpFromPath(Format("%s.png",szPath));
	}
	return bmp;
}

CBitmap* CBmpHolder::AddBmpRaw(const char* szName, CBitmap* bmp, CSize size)
{_XLOG_
	#ifdef GDI_NOICONS
		return 0;
	#endif
	if(!bmp){_XLOG_
		return NULL;
	}
	if(IsPresent(szName)){_XLOG_
		DelBmp(szName);
	}
	CSize zNew=(size.cx<0?size:(size.cx==0?MenuIconSize:size));
	CBitmap* bmpSized=Clone(bmp,zNew);
	bmpHolder.SetAt(szName,(void*)bmpSized);
	return bmpSized;
}

CBitmap* CBmpHolder::AddBmp(const char* szName, CBitmap* bmp, CSize size)
{_XLOG_
	#ifdef GDI_NOICONS
		return 0;
	#endif
	#ifdef GDI_KEEPER
	if(isWin98()){_XLOG_
		return NULL;
	}
	#endif
	return AddBmpRaw(szName, bmp, size);
}

CBitmap* CBmpHolder::AddBmpRaw(DWORD dwID, CSize size, const char* szPrefix)
{_XLOG_
	CString sKey=GetResName(dwID, szPrefix);
	CBitmap* bmp=GetRaw(sKey);
	if(bmp){_XLOG_
		DelBmp(sKey);
	}
	CBitmap bmpTemp;
	if(bmpTemp.LoadBitmap(dwID)){_XLOG_
		bmp=AddBmpRaw(sKey,&bmpTemp,size);
	}
	return bmp;
}

CBitmap* CBmpHolder::AddBmp(DWORD dwID, CSize size, const char* szPrefix)
{_XLOG_
	#ifdef GDI_NOICONS
		return 0;
	#endif
	#ifdef GDI_KEEPER
	if(isWin98()){_XLOG_
		return NULL;
	}
	#endif
	return AddBmpRaw(dwID, size, szPrefix);
}

CBitmap* CBmpHolder::AddBmpRaw(const char* szName, HICON icon, CSize sizeIn, CBrush* br)
{_XLOG_
	static CBrush brWhite(RGB(255,255,255));
	if(br==NULL){_XLOG_
		br=&brWhite;
	}
	CSize size;
	if(sizeIn.cx==0 || sizeIn.cy==0){_XLOG_
		size=GetIconSize(icon);
	}else{_XLOG_
		size.cx=sizeIn.cx;
		size.cy=sizeIn.cy;
	}
	CDC pDC;
	CBitmap pBM;
	CDesktopDC dcDesk;
	pDC.CreateCompatibleDC(&dcDesk);
	BOOL bRes=pBM.CreateCompatibleBitmap(&dcDesk,size.cx,size.cy);
	CBitmap* pTmpBmp=pDC.SelectObject(&pBM);
	DrawIconEx(pDC,0,0,icon,size.cx,size.cy,0,*br,DI_NORMAL);
	pDC.SelectObject(pTmpBmp);
	return AddBmpRaw(szName,&pBM,MenuIconSize);
}

CBitmap* CBmpHolder::AddBmp(const char* szName, HICON icon, CSize sizeIn, CBrush* br)
{_XLOG_
	#ifdef GDI_NOICONS
		return 0;
	#endif
	#ifdef GDI_KEEPER
	if(isWin98()){_XLOG_
		return NULL;
	}
	#endif
	return AddBmpRaw(szName, icon, sizeIn, br);
}

CBitmap* CBmpHolder::AddBmpFromDiskIfNotPresent(const char* szFile, const char* szName, CSize size)
{_XLOG_
	void* ptr=NULL;
	if(bmpHolder.Lookup(szName,ptr)==TRUE){_XLOG_
		return (CBitmap*)ptr;
	}
	return AddBmpFromDisk(szFile, size, szName);
}

CBitmap* CBmpHolder::AddBmpFromDisk(const char* szFile, CSize size, const char* szName)
{_XLOG_
	CBitmap* pSavedInMapBmp=NULL;
	CBitmap* bmp=LoadBmpFromPath(szFile);
	if(bmp){_XLOG_
		if(size.cx==0){_XLOG_
			size=GetBitmapSize(bmp);
		}
		pSavedInMapBmp=AddBmpRaw(szName?szName:szFile,bmp,size);
		delete bmp;
	}
	return pSavedInMapBmp;
}

BOOL CBmpHolder::DelBmp(DWORD dwID, const char* szPrefix)
{_XLOG_
	return DelBmp(GetResName(dwID,szPrefix));
}

BOOL CBmpHolder::DelBmp(const char* szName)
{_XLOG_
	BOOL bRes=FALSE;
	void* ptr=NULL;
	if(bmpHolder.Lookup(szName,ptr)==TRUE){_XLOG_
		if(ptr!=NULL){_XLOG_
			CBitmap* bmp=(CBitmap*)ptr;
			delete bmp;
			bRes=TRUE;
		}
		bmpHolder.RemoveKey(szName);
	}
	return bRes;
}
//
CString CBmpHolder::GetResName(DWORD dwID, const char* szNamePrefx)
{_XLOG_
	CString sKey;
	sKey.Format("ID_%lu",dwID);
	if(szNamePrefx){_XLOG_
		sKey+=szNamePrefx;
	}
	return sKey;
}
//
BOOL CBmpHolder::IsPresent(const char* szBmpName)
{_XLOG_
	void* ptr=NULL;
	return bmpHolder.Lookup(szBmpName,ptr);
}

CBitmap* CBmpHolder::GetIcon(const char* szIconName)
{_XLOG_
	CString sBmpName=GetResName(LOWORD(szIconName),"CICON");
	CBitmap* bmp=Get(sBmpName);
	if(bmp==NULL){_XLOG_
		bmp=AddBmp(sBmpName,::AfxGetApp()->LoadStandardIcon(szIconName));
	}
	return bmp;
}

CBitmap* CBmpHolder::GetIcon(DWORD dwIcon)
{_XLOG_
	CString sBmpName=GetResName(dwIcon,"ICON");
	CBitmap* bmp=Get(sBmpName);
	if(bmp==NULL){_XLOG_
		bmp=AddBmp(sBmpName,::AfxGetApp()->LoadIcon(dwIcon));
	}
	return bmp;
}

CBitmap* CBmpHolder::GetRaw(DWORD dwID, CSize size, const char* szNamePrefx)
{_XLOG_
	CBitmap* bmp=GetRaw(GetResName(dwID,szNamePrefx));
	if(bmp==NULL){_XLOG_
		bmp=AddBmpRaw(dwID,size,szNamePrefx);
	}
	return bmp;
}

CBitmap* CBmpHolder::Get(DWORD dwID, CSize size, const char* szNamePrefx)
{_XLOG_
	#ifdef GDI_NOICONS
		return 0;
	#endif
	#ifdef GDI_KEEPER
	if(isWin98()){_XLOG_
		return NULL;
	}
	#endif
	return GetRaw(dwID, size, szNamePrefx);
}

CBitmap* CBmpHolder::GetRaw(const char* szName, CSize size)
{_XLOG_
	if(szName==0 || *szName==0){_XLOG_
		return 0;
	}
	BOOL bRes=FALSE;
	void* ptr=NULL;
	CBitmap* bmp=NULL;
	if(bmpHolder.Lookup(szName,ptr)==TRUE){_XLOG_
		if(ptr!=NULL){_XLOG_
			bmp=(CBitmap*)ptr;
		}
	}
	return bmp;
}

CBitmap* CBmpHolder::Get(const char* szName, CSize size)
{_XLOG_
	#ifdef GDI_NOICONS
		return 0;
	#endif
	#ifdef GDI_KEEPER
	if(isWin98()){_XLOG_
		return NULL;
	}
	#endif
	return GetRaw(szName, size);
}

CBitmap* CBmpHolder::Clone(CBitmap* bmp, CSize size)
{_XLOG_
	if(!bmp){_XLOG_
		return NULL;
	}
	CSize sizeOrig(GetBitmapSize(bmp));
	CSize sizeTo(sizeOrig);
	if(size.cx>0 && size.cy>0){_XLOG_
		sizeTo.cx=size.cx;
		sizeTo.cy=size.cy;
	}
	HBITMAP hTo=NULL;
	CopyBitmapToBitmap(*bmp,hTo,sizeOrig,&sizeTo);
	if(!hTo){_XLOG_
		return NULL;
	}
	CBitmap* newBmp= new CBitmap();
	newBmp->Attach(hTo);
	return newBmp;
}

CBitmap* CBmpHolder::Clone(DWORD dwID, CSize sizeTo)
{_XLOG_
	return Clone(GetResName(dwID),sizeTo);
}

CBitmap* CBmpHolder::Clone(const char* szName, CSize size)
{_XLOG_
	CBitmap* bmp=GetRaw(szName);
	return Clone(bmp,size);
}

HICON CBmpHolder::CloneAsIcon(DWORD dwID)
{_XLOG_
	return CloneAsIcon(GetResName(dwID));
}

HICON CBmpHolder::CloneAsIcon(const char* szName)
{_XLOG_
	return CreateIconFromBitmap(GetRaw(szName));
}

CBmpHolder& _bmp()
{_XLOG_
	static CBmpHolder GlobalHolder;
	return GlobalHolder;
}

HICON PutOverlayInIconSz(HICON hIcon,HICON hIcon2, int iX0, int iY0, int iX, int iY, BOOL bFromDown, BOOL bQualitySampling)
{_XLOG_
	DWORD dwXBmpSize=iX0;
	DWORD dwYBmpSize=iY0;
	DWORD dwXBmpSize2=dwXBmpSize;
	if(iX>0){_XLOG_
		dwXBmpSize2=iX;
	}
	DWORD dwYBmpSize2=dwYBmpSize;
	if(iY>0){_XLOG_
		dwYBmpSize2=iY;
	}
	DWORD dwXBmpPos2=0,dwYBmpPos2=0;
	if(bFromDown){_XLOG_
		dwYBmpPos2=dwYBmpSize-dwYBmpSize2;
	}
	CBrush IconBr(RGB(255,255,255));
	CDC dc;
	CBitmap bmp;
	CDesktopDC dcDesk;
	dc.CreateCompatibleDC(&dcDesk);
	bmp.CreateCompatibleBitmap(&dcDesk, dwXBmpSize, dwYBmpSize);
	CBitmap* pOld=dc.SelectObject(&bmp);
	::DrawIconEx(dc,0,0,hIcon,dwXBmpSize,dwYBmpSize,0,IconBr,DI_NORMAL);
	::DrawIconEx(dc,dwXBmpPos2,dwYBmpPos2,hIcon2,dwXBmpSize2,dwYBmpSize2,0,NULL,DI_NORMAL);
	HICON hOut=CreateIconFromBitmap(&bmp,NULL,FALSE);
	dc.SelectObject(pOld);
	return hOut;
}

HICON PutOverlayInIcon(HICON hIcon,HICON hIcon2, int iX, int iY, BOOL bFromDown, BOOL bQualitySampling)
{_XLOG_
	DWORD dwXBmpSize=GetIconSize(hIcon).cx;
	DWORD dwYBmpSize=GetIconSize(hIcon).cy;
	return PutOverlayInIconSz(hIcon,hIcon2, dwXBmpSize, dwYBmpSize, iX, iY, bFromDown, bQualitySampling);
}

HICON PutTextInIcon(HICON hIcon,const char* szText, CFont* pFont, COLORREF dwCol, COLORREF dwBgCol, DWORD dwFlags)
{_XLOG_
	DWORD dwBmpSize=50;
	BOOL bRes=0;
	CBrush IconBr(RGB(255,255,255));
	CDC dc;
	CDesktopDC dcDesk;
	dc.CreateCompatibleDC(&dcDesk);
	if(!dc.GetSafeHdc()){_XLOG_
		return hIcon;
	}
	CFont pFontNew;
	CFont* pFontOld=NULL;
	BOOL bFonSelect=FALSE;
	if(pFont){_XLOG_
		bFonSelect=TRUE;
		if((dwFlags&TINI_FONTEXACT)==0){_XLOG_
			LOGFONT lg;
			pFont->GetLogFont(&lg);
			lg.lfWeight=800;
			lg.lfHeight=-30;
			lg.lfWidth=0;
			pFontNew.CreateFontIndirect(&lg);
			pFontOld=dc.SelectObject(&pFontNew);
		}else{_XLOG_
			pFontOld=dc.SelectObject(pFont);
		}
	}
	//
	CBitmap bmp;
	CSize size=GetIconSize(hIcon);
	CSize sizeT=dc.GetOutputTextExtent(szText,strlen(szText));
	dwBmpSize=(DWORD)((size.cx>size.cy?size.cx:size.cy)*1);
	bmp.CreateCompatibleBitmap(&dcDesk, dwBmpSize, dwBmpSize);
	CBitmap* pOld=dc.SelectObject(&bmp);
	bRes=::DrawIconEx(dc,0,0,hIcon,dwBmpSize,dwBmpSize,0,IconBr,DI_NORMAL);
	CRect bounds(0,0,dwBmpSize,dwBmpSize);
	SmartDrawText(dc, szText, bounds, NULL, dwCol, dwBgCol, TXSHADOW_WIDE|TXSHADOW_GLOW|TXSHADOW_VCENT|TXSHADOW_HCENT);
	if(bFonSelect){_XLOG_
		dc.SelectObject(pFontOld);
	}
	HICON hOut=CreateIconFromBitmap(&bmp,FALSE);
	dc.SelectObject(pOld);
	return hOut;
}


BOOL SmartDrawTextX(HDC pDC, const char* szText, CRect& bounds, CFont* pFont, COLORREF txColor, COLORREF bgColor, DWORD dwShadowMode, DWORD dwTextParams, COLORREF shColor, long lTextOp)
{_XLOG_
	if(lTextOp==0){_XLOG_
		return 0;
	}
	if(lTextOp==100){_XLOG_
		return SmartDrawText(pDC, szText, bounds, pFont, txColor, bgColor, dwShadowMode, dwTextParams, shColor);
	}
	CDesktopDC dcDesk;
	CDC dcT;
	dcT.CreateCompatibleDC(&dcDesk);
	CBitmap bmpOffscreen;
	bmpOffscreen.CreateCompatibleBitmap(&dcDesk,bounds.Width(),bounds.Height());
	CBitmap* pOld0=dcT.SelectObject(&bmpOffscreen);
	CFont pTmpFon;
	if(pFont==0){_XLOG_
		pTmpFon.Attach(GetCurrentObject(pDC,OBJ_FONT));
		dcT.SelectObject(&pTmpFon);
	}
	::BitBlt(dcT,0,0,bounds.Width(),bounds.Height(),pDC,bounds.left,bounds.top,SRCCOPY);
	CRect boundsT(0,0,bounds.Width(),bounds.Height());
	SmartDrawText(dcT, szText, boundsT, pFont, txColor, bgColor, dwShadowMode, dwTextParams, shColor);
	BLENDFUNCTION blendFunction;
	blendFunction.BlendFlags=0;
	blendFunction.AlphaFormat=0;
	blendFunction.BlendOp=AC_SRC_OVER;
	blendFunction.SourceConstantAlpha=(int)(double(255)*double(lTextOp)/double(100));
	AlphaBlendMy(pDC,bounds.left,bounds.top, bounds.Width(),bounds.Height(),dcT, 0, 0, blendFunction);
	dcT.SelectObject(pOld0);
	if(pFont==0){_XLOG_
		pTmpFon.Detach();
		//dcT.SelectObject(pTmpFon);
	}
	bounds.left+=boundsT.left;
	bounds.top+=boundsT.top;
	bounds.right=bounds.left+boundsT.Width();
	bounds.bottom=bounds.top+boundsT.Height();
	return TRUE;
}

BOOL SmartDrawText(HDC pDC, const char* szText, CRect& bounds, CFont* pFont, COLORREF txColor, COLORREF bgColor, DWORD dwShadowMode, DWORD dwTextParams, COLORREF shColor)
{_XLOG_
	BOOL bRes=0;
	if(szText==NULL){_XLOG_
		szText="";
	}
	CRect boundsOrig=bounds;
	DWORD dwTextFlags=dwTextParams;
	dwTextFlags|=((dwShadowMode & TXSHADOW_RIGHT)?DT_RIGHT:0)|((dwShadowMode & TXSHADOW_VCENT)?DT_VCENTER:0)|((dwShadowMode&TXSHADOW_HCENT)?DT_CENTER:0);
	LPVOID pOldFont=0;
	if(pFont){_XLOG_
		// Так рушит многое
		//pOldFont=::SelectObject(pDC, pFont);
	}
	if((dwShadowMode & TXSHADOW_CALCRC)!=0 || (dwShadowMode & TXTEXT_VCENT)!=0 || (dwShadowMode & TXTEXT_VBOTTOM)!=0 || (dwShadowMode & TXTEXT_CALCDR)!=0){_XLOG_
		if(strlen(szText)==0){_XLOG_
			bounds.bottom=bounds.top;
		}else{_XLOG_
			int iH=0;
			CRect rNewBounds;
			rNewBounds.CopyRect(bounds);
			if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
				iH=::DrawTextW(pDC, (LPCWSTR)szText, -1, &rNewBounds, dwTextFlags|DT_CALCRECT);
			}else{_XLOG_
				iH=::DrawText(pDC, szText, -1, &rNewBounds, dwTextFlags|DT_CALCRECT);
			}
			if((dwShadowMode & TXTEXT_VCENT)!=0 && bounds.Height()>rNewBounds.Height()){_XLOG_
				// Вертикально!!!
				bounds.top+=int((bounds.Height()-rNewBounds.Height())/2);
				boundsOrig.CopyRect(bounds);
				//::DrawFocusRect(pDC,&bounds);
			}
			if((dwShadowMode & TXTEXT_VBOTTOM)!=0 && bounds.Height()>rNewBounds.Height()){_XLOG_
				bounds.top+=bounds.Height()-rNewBounds.Height();
				boundsOrig.CopyRect(bounds);
			}
			if((dwShadowMode & TXTEXT_CALCDR)!=0){_XLOG_
				bounds.bottom=bounds.top+rNewBounds.Height();
				boundsOrig.CopyRect(bounds);
			}
			if((dwShadowMode & TXSHADOW_CALCRC)!=0){_XLOG_
				bounds.CopyRect(rNewBounds);
				//bounds.bottom=bounds.top+iH;
				return bRes;
			}
		}
	}
	if((dwShadowMode & TXTEXT_UNICODE) && wcslen((LPCWSTR)szText)==0){_XLOG_
		return bRes;
	}else if(strlen(szText)==0){_XLOG_
		return bRes;
	}
	::SetBkColor(pDC, bgColor);
	::SetBkMode(pDC, TRANSPARENT);
	COLORREF cTextCol2=bgColor;
	if(dwShadowMode & TXSHADOW_SHCOL){_XLOG_
		cTextCol2=shColor;
	}
	if(dwShadowMode & TXSHADOW_MIXBG){_XLOG_
		cTextCol2=RGB(
			GetRValue(bgColor)+int(GetRValue(txColor)-GetRValue(bgColor))/3,
			GetGValue(bgColor)+int(GetGValue(txColor)-GetGValue(bgColor))/3,
			GetBValue(bgColor)+int(GetBValue(txColor)-GetBValue(bgColor))/3);
	}
	/*
#ifdef _DEBUG
	CBrush br(RGB(255,0,0));
	::FrameRect(pDC,bounds,br);
#endif
	*/
	BOOL bRegionUsed=0;
	if(dwShadowMode & TXSHADOW_FILLBG){_XLOG_
		CBrush br(bgColor);
		::FillRect(pDC, &bounds, br);
	}
	if((dwShadowMode & TXSHADOW_GLOW) && (dwShadowMode & TXSHADOW_USERGN)){//
		bRegionUsed=1;
		BeginPath(pDC);
	}
	if(!bRegionUsed && ((dwShadowMode & TXSHADOW_SIMPLE) || (dwShadowMode & TXSHADOW_GLOW))){_XLOG_
		::SetTextColor(pDC, cTextCol2);
		if(dwShadowMode & TXSHADOW_SIMPLE){_XLOG_
			bounds.OffsetRect(1,0);
			if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
				::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
			}else{_XLOG_
				::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
			}
			bounds.OffsetRect(0,1);
			if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
				::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
			}else{_XLOG_
				::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
			}
			if(dwShadowMode & TXSHADOW_WIDE){_XLOG_
				bounds.OffsetRect(1,1);
				if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
					::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
				}else{_XLOG_
					::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
				}
			}
		}else if(dwShadowMode & TXSHADOW_GLOW){_XLOG_
			bounds.OffsetRect(CPoint(1,1));
			if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
				::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
			}else{_XLOG_
				::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
			}
			bounds.OffsetRect(CPoint(-2,-2));
			if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
				::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
			}else{_XLOG_
				::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
			}
			bounds.OffsetRect(CPoint(2,0));
			if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
				::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
			}else{_XLOG_
				::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
			}
			bounds.OffsetRect(CPoint(-2,2));
			if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
				::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
			}else{_XLOG_
				::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
			}
			if(dwShadowMode & TXSHADOW_WIDE){_XLOG_
				bounds=boundsOrig;
				bounds.OffsetRect(CPoint(2,2));
				if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
					::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
				}else{_XLOG_
					::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
				}
				bounds.OffsetRect(CPoint(-4,-4));
				if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
					::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
				}else{_XLOG_
					::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
				}
				bounds.OffsetRect(CPoint(4,0));
				if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
					::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
				}else{_XLOG_
					::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
				}
				bounds.OffsetRect(CPoint(-4,4));
				if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
					::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
				}else{_XLOG_
					::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
				}
			}
		}
		//
	}
	long lTW=0;
	bounds=boundsOrig;
	::SetTextColor(pDC,txColor);
	if(bRegionUsed){_XLOG_
		// Рисуем для тени...
		if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
			lTW=::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
		}else{_XLOG_
			lTW=::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
		}
		EndPath(pDC);
	}
	if(bRegionUsed){_XLOG_
		//CBrush br(txColor);
		//HGDIOBJ hTemp=::SelectObject(pDC,br);
		CPen pc(PS_SOLID,(dwShadowMode & TXSHADOW_WIDE)?6:0,cTextCol2);
		HGDIOBJ hTemp2=::SelectObject(pDC,pc);
		//WidenPath(pDC);
		//WidenPath(pDC);
		//HRGN hRgn = ::PathToRegion(pDC);
		//FillRgn(pDC,hRgn,br);
		StrokePath(pDC);//AndFill
		//StrokePath(pDC);
		::SelectObject(pDC,hTemp2);
		//::SelectObject(pDC,hTemp);
		//CloseHandle(hRgn);
	}
	if(dwShadowMode & TXTEXT_UNICODE){_XLOG_
		lTW=::DrawTextW(pDC, (LPCWSTR)szText, -1, &bounds, dwTextFlags);
	}else{_XLOG_
		lTW=::DrawText(pDC, szText, -1, &bounds, dwTextFlags);
	}
	if(pOldFont){_XLOG_
		::SelectObject(pDC, pOldFont);
	}
	if(lTW>bounds.Height()){_XLOG_
		bRes=1;
	}
	return bRes;
}

CSplashParams::CSplashParams()
{_XLOG_
	bHidden=0;
	pFont=0;
	bFade=1;
	dwSplashAlign=0;
	bounds=CRect(20,20,100,30);
	bmp=NULL;
	szText="";
	sTextPrefix=_l("Starting")+" '";
	sTextPosfix="'...";
	txColor=RGB(250,250,250);
	bgColor=0;
	dwTextParams=0;
}

HRGN CreateRgnFromBitmap(HBITMAP hBitmap,COLORREF crTransparent,BOOL bUSELR)
{_XLOG_
	//create a memory DC
	BITMAP bmpInfo;
	HDC hdcMem = CreateCompatibleDC(NULL);
	HRGN hRgn = CreateRectRgn(0,0,0,0);
	GetObject(hBitmap, sizeof(bmpInfo), &bmpInfo);
	//Select the bitmap into the dc
	HGDIOBJ hGdiObj = SelectObject(hdcMem, hBitmap);
	int iX = 0;
	int iRet = 0;
	if(bUSELR){_XLOG_
		crTransparent=GetPixel(hdcMem, 0, 0);
	}
	for (int iY = 0; iY < bmpInfo.bmHeight; iY++)
	{_XLOG_
		do
		{_XLOG_
			DWORD dwPix=0;
			//skip over transparent pixels at start of lines.
			while (iX < bmpInfo.bmWidth && (dwPix=GetPixel(hdcMem, iX, iY)) == crTransparent){_XLOG_
				iX++;
			}
			//remember this pixel
			int iLeftX = iX;
			//now find first non-transparent pixel
			while (iX < bmpInfo.bmWidth && (dwPix=GetPixel(hdcMem, iX, iY)) != crTransparent){_XLOG_
				++iX;
			}
			//create a temp region on this info
			HRGN hRgnTemp = CreateRectRgn(iLeftX, iY, iX, iY+1);
			//combine into main region
			iRet = CombineRgn(hRgn, hRgn, hRgnTemp, RGN_OR);
			if(iRet == ERROR)
			{_XLOG_
				return 0;
			}
			//delete the temp region for next pass
			DeleteObject(hRgnTemp);
		}while(iX < bmpInfo.bmWidth);
		iX = 0;
	}
	SelectObject(hdcMem, hGdiObj);
	DeleteDC(hdcMem);
	return hRgn;
}

// Начинаем с -1 nfr rfr byrhtvtynbncz chfpe///
long CSplashWindow::lSplashWindowsCount=-1;
DWORD WINAPI SplashWndProc(void* pData)
{_XLOG_
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	CSplashWindow* pSplash=(CSplashWindow*)pData;
	if(!pSplash){_XLOG_
		InterlockedDecrement(&CSplashWindow::lSplashWindowsCount);
		return 98;
	}
	BOOL bCreated=pSplash->CreateWnd();
	if(!bCreated){_XLOG_
		InterlockedDecrement(&CSplashWindow::lSplashWindowsCount);
		return 97;
	}
	SetEvent(pSplash->Event);
	BOOL bRet=0;
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0){ 
		if (msg.message==WM_QUIT || bRet == -1){_XLOG_
			break;
		}
		if(pSplash->wndH!=NULL && msg.message==WM_PAINT){_XLOG_
			pSplash->OnPaint();
			continue;
		}
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	pSplash->bClosing=1;
	if(pSplash->par.bFade && !isWin98() && pSplash->wnd && pSplash->bmpOffscreen){_XLOG_
		// Фэйдим...
		HWND h=pSplash->wnd->GetSafeHwnd();
		/*
		//SetLayeredWindowStyle(h,1);
		HDC hdcSrc=GetWindowDC(h);;//CreateCompatibleDC(h);
		for(int i=240;i>=10 && IsWindow(h);i=i-15){_XLOG_
			BLENDFUNCTION blendFunction;
			blendFunction.BlendFlags=0;
			blendFunction.AlphaFormat=0;
			blendFunction.BlendOp=AC_SRC_OVER;
			blendFunction.SourceConstantAlpha=i;
			UpdateLayeredWindowMy(h, NULL, 0, 0, hdcSrc, 0, pSplash->par.bgColor, &blendFunction, ULW_ALPHA|ULW_COLORKEY);
			//SetWndAlpha(h,pSplash->par.bgColor,FALSE);
			//SetWndAlpha(h,i,TRUE);
			DWORD dw=GetLastError();
			Sleep(100);
		}
		ReleaseDC(h,hdcSrc);//DeleteDC(hdcSrc);
		*/
		HRGN hRgn=CreateRgnFromBitmap(*pSplash->bmpOffscreen,pSplash->par.bgColor,1);
		if(hRgn){_XLOG_
			if(SetWindowRgn(h, hRgn, TRUE)){_XLOG_
				for(int i=90;i>=10 && IsWindow(h);i=i-15){_XLOG_
					SetWndAlpha(h,i,TRUE);
					Sleep(20);
				}
			}else{_XLOG_
				DeleteObject(hRgn); //- не нужно, окно само им уже владеет
			}
		}
	}
	pSplash->wndH=NULL;
	if(pSplash->wnd){_XLOG_
		//SimpleLocker l(&pSplash->cs);
		delete pSplash->wnd;
		pSplash->wnd=NULL;
	}
	InterlockedDecrement(&CSplashWindow::lSplashWindowsCount);
	SetEvent(pSplash->Event);
	return 0;
}

CSplashWindow::CSplashWindow(CSplashParams* SplParam)
{_XLOG_
	InterlockedIncrement(&CSplashWindow::lSplashWindowsCount);
	TRACETHREAD;
	bClosing=0;
	bNeedUpadteIntBmp=1;
	bmpOffscreen=0;
	bBeforeExit=0;
	dwThread=0;
	pFont=0;
	wnd=NULL;
	wndH=NULL;
	hWndProc=NULL;
	lDYCounter=-1;
	par=*SplParam;
	SetFont(par.pFont);
	dwAutoCloserNumber=0;
	bVisible=!(par.dwSplashAlign & SPLASH_HIDDEN);
	Event=::CreateEvent(NULL,TRUE,FALSE,NULL);
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rDesktopRECT,0);
	hWndProc=::CreateThread(NULL, 0, SplashWndProc, LPVOID(this), 0, &dwThread);
	WaitForSingleObject(Event,10000);
	ResetEvent(Event);
}

CSplashWindow::~CSplashWindow()
{_XLOG_
	wndH=NULL;
	if(hWndProc){_XLOG_
		PostThreadMessage(dwThread, WM_QUIT,0,0);
		WaitForSingleObject(Event,30000);
		if(WaitForSingleObject(hWndProc,30000)!=WAIT_OBJECT_0){_XLOG_
			TerminateThread(hWndProc,99);
		}
		::CloseHandle(hWndProc);
	}
	SimpleLocker l(&cs);
	CloseHandle(Event);
	if(pFont!=NULL){_XLOG_
		delete pFont;
		pFont=NULL;
	}
	if(bmpOffscreen){_XLOG_
		delete bmpOffscreen;
		bmpOffscreen=0;
	}
}

class CSplashAutoCloser
{
public:
	CSplashAutoCloser()
	{_XLOG_
		pSplash=0;
		pSplashToSetToNULL=0;
		dwTimeOut=0;
	}
	CSplashWindow* pSplash;
	CSplashWindow** pSplashToSetToNULL;
	DWORD dwTimeOut;
};

DWORD WINAPI AutoCloseSplash_inThread(LPVOID pData)
{_XLOG_
	CSplashAutoCloser* pSAC=(CSplashAutoCloser*)pData;
	if(pSAC){_XLOG_
		if(!pSAC->pSplash->bBeforeExit){_XLOG_
			DWORD dwBefore=pSAC->pSplash->dwAutoCloserNumber;
			Sleep(pSAC->dwTimeOut);
			DWORD dwAfter=pSAC->pSplash->dwAutoCloserNumber;
			if(dwBefore==dwAfter){_XLOG_
				pSAC->pSplash->bBeforeExit=1;
				if(pSAC->pSplashToSetToNULL){_XLOG_
					*pSAC->pSplashToSetToNULL=NULL;
				}
				PostThreadMessage(pSAC->pSplash->dwThread, WM_QUIT,0,0);
			}
		}
		delete pSAC;
	}
	return 0;
}

void CSplashWindow::AutoClose(DWORD dwTimeX,CSplashWindow** pSetToNULL)
{_XLOG_
	if(bBeforeExit){_XLOG_
		return;
	}
	static DWORD dwAutoCloseID=0;
	dwAutoCloserNumber=dwAutoCloseID;
	dwAutoCloseID++;
	CSplashAutoCloser* pSAC=new CSplashAutoCloser();
	pSAC->pSplash=this;
	pSAC->dwTimeOut=dwTimeX;
	pSAC->pSplashToSetToNULL=pSetToNULL;
	FORK(AutoCloseSplash_inThread,pSAC);
	return;
}

void CSplashWindow::OnPaint()
{_XLOG_
	SimpleLocker l(&cs);
	static long iPaintInProgress=0;
	if(iPaintInProgress>0 || !IsWindow(wndH)){_XLOG_
		return;
	}
	SimpleTracker Lock(iPaintInProgress);
	BOOL bDB=(par.bFade && ((par.dwSplashAlign & SPLASH_OPACITY)!=0));
	CRect rt;
	wnd->GetClientRect(&rt);
	CPaintDC pDCOrig(wnd);
	CDC *pDC=0,pBMDc;
	CBitmap* pOld=NULL;
	CBitmap* pOld0=NULL;
	BOOL bCanDelPDC=0;
	if(bDB && bNeedUpadteIntBmp){_XLOG_
		bNeedUpadteIntBmp=0;
		bCanDelPDC=1;
		pDC=new CDC();
		CDesktopDC dcDesk;
		pDC->CreateCompatibleDC(&dcDesk);
		if(bmpOffscreen){_XLOG_
			delete bmpOffscreen;
		}
		bmpOffscreen=new CBitmap();
		bmpOffscreen->CreateCompatibleBitmap(&dcDesk,rt.Width(),rt.Height());
		pOld0=pDC->SelectObject(bmpOffscreen);
		pBMDc.CreateCompatibleDC(&dcDesk);
	}else{_XLOG_
		pDC=&pDCOrig;
	}
	if(par.bmp){_XLOG_
		pOld=pBMDc.SelectObject(par.bmp);
		pDC->SetStretchBltMode(HALFTONE);
		pDC->StretchBlt(0,0,rt.Width(),rt.Height(),&pBMDc,0,0,par.bounds.Width(),par.bounds.Height(),SRCCOPY);
	}else{_XLOG_
		pDC->FillSolidRect(&rt,par.bgColor);
	}
	if(par.szText!=""){_XLOG_
		CFont* pTmp=NULL;
		if(pFont){_XLOG_
			pTmp=pDC->SelectObject(pFont);
		}
		// Двойной буффер!
		SmartDrawText(*pDC,CString(" ")+par.szText,rt,NULL,par.txColor,par.bgColor,par.dwTextParams|TXSHADOW_USERGN);
		if(pTmp){_XLOG_
			pDC->SelectObject(pTmp);
		}
	}
	if(bDB && pDC){_XLOG_
		BitBlt(pDCOrig,rt.left,rt.top,rt.Width(),rt.Height(),*pDC,rt.left,rt.top,SRCCOPY);
		/*
		if(par.dwSplashAlign & SPLASH_OPACITY){_XLOG_
		TransparentBltMy(pDCOrig,rt.left,rt.top,rt.Width(),rt.Height(),pDC,rt.left,rt.top,par.bgColor);
		}else{_XLOG_
		BitBlt(pDCOrig,rt.left,rt.top,rt.Width(),rt.Height(),pDC,rt.left,rt.top,SRCCOPY);
		}*/
		if(pOld0){_XLOG_
			pDC->SelectObject(pOld0);
		}
		if(bCanDelPDC){_XLOG_
			delete pDC;
		}
	}
	if(pOld){_XLOG_
		pBMDc.SelectObject(pOld);
	}
	//wnd->ReleaseDC(pDCOrig);
}

void CSplashWindow::SetBitmap(CBitmap* bmp)
{_XLOG_
	par.bmp=bmp;
	UpdateSize();
}

void CSplashWindow::SetTextOnly(const char* szText)
{_XLOG_
	par.szText=szText;
	UpdateSize();
}

void CSplashWindow::SetText(const char* szText, COLORREF txColor, COLORREF bgColor, DWORD dwTextParams)
{_XLOG_
	par.szText=szText;
	par.txColor=txColor;
	par.bgColor=bgColor;
	par.dwTextParams=dwTextParams;
	UpdateSize();
}

void CSplashWindow::SetFont(CFont* pFontFrom)
{_XLOG_
	if(!pFontFrom){_XLOG_
		return;
	}
	LOGFONT log;
	pFontFrom->GetLogFont(&log);
	if(pFont!=NULL){_XLOG_
		delete pFont;
		pFont=NULL;
	}
	pFont=new CFont();
	pFont->CreateFontIndirect(&log);
	UpdateSize();
}

BOOL CSplashWindow::CreateWnd()
{_XLOG_
	SimpleLocker l(&cs);
	wnd=new CWnd();
	BOOL bRes=FALSE;
	BOOL bOpaq=(par.dwSplashAlign & SPLASH_OPACITY);
	{// Аттачим к ьрею в качестве парента
		CWnd parW;
		parW.Attach(FindWindow("Shell_TrayWnd",NULL));
		LPCTSTR szClass = AfxRegisterWndClass(NULL);
		bRes=wnd->CreateEx((bOpaq?WS_EX_LAYERED:0)|WS_EX_TOOLWINDOW|WS_EX_TOPMOST, szClass, _T("Splash string"), ((par.dwSplashAlign & SPLASH_OPACITY)?0:WS_BORDER)|WS_POPUP|WS_DISABLED, par.bounds, &parW, 0);
		wnd->ShowWindow(SW_SHOWNOACTIVATE);
		parW.Detach();
	}
	BOOL bNeedBorder=1;
	if(bRes){_XLOG_
		wndH=wnd->GetSafeHwnd();
		if(bOpaq){_XLOG_
			bNeedBorder=0;
			if(SetLayeredWindowStyle(wndH,1)){_XLOG_
				if(SetWndAlpha(wndH,par.bgColor,FALSE))
				{_XLOG_
					bNeedBorder=0;
				}
			}
		}else{_XLOG_
			par.bFade=0;
		}
		if(bNeedBorder){_XLOG_
			wnd->ModifyStyle(0,WS_DLGFRAME,SWP_DRAWFRAME|SWP_NOACTIVATE);
		}
		UpdateSize();
	}
	return bRes;
}

BOOL CSplashWindow::UpdateSize()
{_XLOG_
	if(bClosing){_XLOG_
		return 0;
	}
	SimpleLocker l(&cs);
	bNeedUpadteIntBmp=1;
	if(lDYCounter==-1){_XLOG_
		lDYCounter=CSplashWindow::lSplashWindowsCount;
		//par.szText=Format("%i@%s",lDYCounter,par.szText);
	}

	CSize sz(100,20);
	if(IsWindow(wndH)){_XLOG_
		CRect rt=par.bounds;
		CDC* pDC=wnd->GetDC();
		CFont* pTmp=0;
		if(pFont){_XLOG_
			pTmp=pDC->SelectObject(pFont);
		}
		SmartDrawText(*pDC,CString(" ")+par.szText,rt,NULL,0,0,TXSHADOW_CALCRC|TXSHADOW_HCENT|TXSHADOW_VCENT);
		if(pTmp){_XLOG_
			pDC->SelectObject(pTmp);
		}
		sz.cx=long(rt.Width()*1.2);
		sz.cy=long(rt.Height()*1.2);
	}
	if(par.bmp){_XLOG_
		CSize sizeBmp=GetBitmapSize(par.bmp);
		if(sizeBmp.cx>sz.cx){_XLOG_
			sz.cx=sizeBmp.cx;
		}
		if(sizeBmp.cy>sz.cy){_XLOG_
			sz.cy=sizeBmp.cy;
		}
	}
	par.bounds=rDesktopRECT;
	if(par.dwSplashAlign & SPLASH_VTOP){_XLOG_
		par.bounds.top=rDesktopRECT.top+10+lDYCounter*sz.cy;
	}
	if(par.dwSplashAlign & SPLASH_HLEFT){_XLOG_
		par.bounds.left=rDesktopRECT.left+10;
	}
	if(par.dwSplashAlign & SPLASH_VBOTTOM){_XLOG_
		par.bounds.top=rDesktopRECT.bottom-sz.cy-10-lDYCounter*sz.cy;
	}
	if(par.dwSplashAlign & SPLASH_HRIGHT){_XLOG_
		par.bounds.left=rDesktopRECT.right-sz.cx-10;
	}
	if(par.dwSplashAlign & SPLASH_VMIDDLE){_XLOG_
		par.bounds.top=long((rDesktopRECT.bottom-sz.cy)/2)+lDYCounter*sz.cy;
	}
	if(par.dwSplashAlign & SPLASH_HCENTER){_XLOG_
		par.bounds.left=long((rDesktopRECT.right-sz.cx)/2);
	}
	par.bounds.right=par.bounds.left+sz.cx;
	par.bounds.bottom=par.bounds.top+sz.cy;
	BOOL bRes=FALSE;
	if(bClosing){_XLOG_
		return 0;
	}
	if(IsWindow(wndH)){_XLOG_
		bRes=wnd->SetWindowPos(&CWnd::wndTopMost,par.bounds.left,par.bounds.top,par.bounds.Width(),par.bounds.Height(),(SWP_ASYNCWINDOWPOS|SWP_NOACTIVATE|(bVisible?SWP_SHOWWINDOW:SWP_HIDEWINDOW)));
	}
	return bRes;
}

CString sLastPreviewFile;
void CPreviewImage::SetPreviewFile(const CString& csFileName)
{_XLOG_
	if (sLastPreviewFile!=""){_XLOG_
		_bmp().DelBmp(sLastPreviewFile);
	}
	sLastPreviewFile=csFileName;
	// Если множественный выбор - выделяем последнюю
	int iPos1=sLastPreviewFile.Find("\" \"");
	if(iPos1>0){_XLOG_
		CString sFile1=sLastPreviewFile.SpanExcluding("\"");
		CString sFile2=sLastPreviewFile.Mid(sFile1.GetLength()+1);
		sFile2=sFile2.SpanExcluding("\"");
		sLastPreviewFile=sFile1+sFile2;
	}
	cBmpSize=GetBitmapSize(_bmp().AddBmpFromDisk(sLastPreviewFile));
	sInfo=Format("%i x %i",cBmpSize.cx,cBmpSize.cy);
}

const char* CPreviewImage::GetImageInfo()
{_XLOG_
	if(sLastPreviewFile=="" || cBmpSize.cx==0 || cBmpSize.cy==0){_XLOG_
		return "";
	}
	return sInfo;
}

void CPreviewImage::DrawPreview(CDC *pDC,int x,int y,int width,int height)
{_XLOG_
	CRect rt(x ,y, x+width, y+height);
	CDC dc;
	CDesktopDC dcDesk;
	dc.CreateCompatibleDC(&dcDesk);
	CBitmap* pNew=_bmp().GetRaw(sLastPreviewFile);
	CBitmap* pBMPOld=NULL;
	if (pNew!=NULL){_XLOG_
		pBMPOld=dc.SelectObject(pNew);
		pDC->SetStretchBltMode(COLORONCOLOR);
		pDC->FillRect(rt, &bg);
		// Подгон размера...
		CRect rtOut(rt);
		FitImageToRect(rtOut,cBmpSize);
		pDC->StretchBlt(rtOut.left, rtOut.top, rtOut.Width(), rtOut.Height(), &dc, 0, 0, cBmpSize.cx, cBmpSize.cy, SRCCOPY);
	}
	if(pBMPOld){_XLOG_
		dc.SelectObject(pBMPOld);
	}
}

CPreviewImage::~CPreviewImage()
{_XLOG_
	if (sLastPreviewFile!=""){_XLOG_
		_bmp().DelBmp(sLastPreviewFile);
	}
};

CPreviewImage::CPreviewImage()
{_XLOG_
	sLastPreviewFile="";
	bg.CreateSysColorBrush(COLOR_BTNFACE);
};

BOOL CALLBACK FindDeskMover(HWND hwnd,LPARAM lParam)
{_XLOG_
	ListView_SetTextColor(hwnd, RGB(255,0,0));
	HWND* hOut=(HWND*)lParam;
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	strupr(szClass);
	if(strcmp(szClass,"DeskMover")==0){_XLOG_
		*hOut=hwnd;
		return FALSE;
	}
	return TRUE;
}


BOOL CALLBACK FindExplorer_Server(HWND hwnd,LPARAM lParam)
{_XLOG_
	ListView_SetTextColor(hwnd, RGB(255,0,0));
	HWND* hOut=(HWND*)lParam;
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	strupr(szClass);
	if(strcmp(szClass,"Internet Explorer_Server")==0){_XLOG_
		*hOut=hwnd;
		return FALSE;
	}else{_XLOG_
		HWND hMaybeChildShell=FindWindowEx(hwnd,0,"SHELLDLL_DefView",NULL);
		if(hMaybeChildShell){_XLOG_
			HWND hMaybeChild=FindWindowEx(hMaybeChildShell,0,"Internet Explorer_Server",NULL);
			if(hMaybeChild!=0){_XLOG_
				*hOut=hMaybeChild;
				return FALSE;
			}
		}
	}
	return TRUE;
}

void AdjustToSceenCoords(CRect& rt,BOOL bDirect)
{_XLOG_
	static HWND hADEnable=0;
	BOOL bActiveDeskEnabled=(hADEnable!=0);
	if(!hADEnable || !IsWindow(hADEnable)){_XLOG_
		hADEnable=0;
		//Progman->SHELLDLL_DefView->syslist32(FolderView)
		//Internet_ExplorerServer->DeskMover
		bActiveDeskEnabled=0;
		DWORD dwProgmanID=0;
		HWND hDesk=::FindWindow("Progman",NULL);
		DWORD dwProgmanThread=GetWindowThreadProcessId(hDesk,&dwProgmanID);
		EnumThreadWindows(dwProgmanThread,FindExplorer_Server,(LPARAM)&hADEnable);
		if(hADEnable!=0){_XLOG_
			bActiveDeskEnabled=1;
		}
	}
	if(bActiveDeskEnabled!=0){_XLOG_
		CRect DesktopDialogRect;
		::SystemParametersInfo(SPI_GETWORKAREA,0,&DesktopDialogRect,0);
		if(bDirect){_XLOG_
			rt.OffsetRect(DesktopDialogRect.left,DesktopDialogRect.top);
		}else{_XLOG_
			rt.OffsetRect(-DesktopDialogRect.left,-DesktopDialogRect.top);
		}
	}
	if(rt.Height()==0){_XLOG_
		rt.bottom=rt.top+rt.Width();
	}
	return;
}

BOOL CALLBACK FindSysListView32(HWND hwnd,LPARAM lParam)
{_XLOG_
	HWND* hOut=(HWND*)lParam;
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	strupr(szClass);
	if(strcmp(szClass,"SYSLISTVIEW32")==0){_XLOG_
		*hOut=hwnd;
		return FALSE;
	}else{_XLOG_
		HWND hMaybeChildShell=FindWindowEx(hwnd,0,"SHELLDLL_DefView",NULL);
		if(hMaybeChildShell){_XLOG_
			HWND hMaybeChild=FindWindowEx(hMaybeChildShell,0,"SysListView32",NULL);
			if(hMaybeChild!=0){_XLOG_
				*hOut=hMaybeChild;
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CALLBACK FindSHELLDLL32(HWND hwnd,LPARAM lParam)
{_XLOG_
	HWND* hOut=(HWND*)lParam;
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	strupr(szClass);
	if(strcmp(szClass,"SHELLDLL_DefView")==0){//DeskMover
		*hOut=hwnd;
		return FALSE;
	}
	return TRUE;
}

HWND GetSysListView()
{_XLOG_
	static HWND hTargetWindow=NULL;
	if(hTargetWindow!=NULL && !IsWindow(hTargetWindow)){_XLOG_
		hTargetWindow=NULL;
	}
	if(hTargetWindow==NULL){_XLOG_
		HWND hWnd1 = ::FindWindow("Progman","Program Manager");
		if(hWnd1 == 0){_XLOG_
			hWnd1 = ::FindWindow("Progman",NULL);
		}
		if(hWnd1 == 0){_XLOG_
			return 0;
		}
		HWND hWnd2 = FindWindowEx(hWnd1,0,"SHELLDLL_DefView",NULL); 
		if(hWnd2 == 0){_XLOG_
			DWORD dwProgmanID=0;
			DWORD dwProgmanThread=GetWindowThreadProcessId(hWnd1,&dwProgmanID);
			EnumThreadWindows(dwProgmanThread,FindSysListView32,(LPARAM)&hTargetWindow);
		}else{_XLOG_
			hTargetWindow = FindWindowEx(hWnd2,0,"SysListView32",NULL); 
		}
		if(hTargetWindow == 0){_XLOG_
#ifdef _DEBUG
			Warn("No desktop found!!!");
#endif
		}
	}
	return hTargetWindow;
}

BOOL CALLBACK RefreshWnd(HWND h, LPARAM)
{_XLOG_
	::RedrawWindow(h,0,0,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
	return TRUE;
}

// В XP не работает - см. KB305117, http://support.microsoft.com/default.aspx?scid=kb%3Ben-us%3B305117
BOOL SetDesktopIconsBg(BOOL bTransparent, COLORREF& cBgCol, COLORREF cTxCol)
{_XLOG_
	static HWND hTargetWindow=NULL;
 	if(hTargetWindow!=NULL && !IsWindow(hTargetWindow)){_XLOG_
		hTargetWindow=NULL;
	}
	if(hTargetWindow==NULL){_XLOG_
		hTargetWindow=GetSysListView();
		if(!hTargetWindow){_XLOG_
			return FALSE;
		}
	}
	static HWND hDesk=NULL;
	if(hDesk!=NULL && !IsWindow(hDesk)){_XLOG_
		hDesk=NULL;
	}
	if(hDesk==NULL){_XLOG_
		hDesk=::FindWindow("Progman",NULL);
	}
	static HWND hDeskMover=NULL;
	if(hDeskMover!=NULL && !IsWindow(hDeskMover)){_XLOG_
		hDeskMover=NULL;
	}
	if(hDeskMover==NULL){_XLOG_
		DWORD dwProgmanID=0;
		DWORD dwProgmanThread=GetWindowThreadProcessId(hDesk,&dwProgmanID);
		EnumThreadWindows(dwProgmanThread,FindDeskMover,(LPARAM)&hDeskMover);
	}
	static HWND hADEnable=0;
	if(hADEnable!=NULL && !IsWindow(hADEnable)){_XLOG_
		hADEnable=NULL;
	}
	RECT rtScreenSize;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rtScreenSize,0);
	BOOL bNeedLayered=0;
	if(bTransparent){_XLOG_
		DWORD dwProgmanID=0;
		DWORD dwProgmanThread=GetWindowThreadProcessId(hDesk,&dwProgmanID);
		EnumThreadWindows(dwProgmanThread,FindExplorer_Server,(LPARAM)&hADEnable);
		if(hADEnable!=0){_XLOG_
			cBgCol=RGB(120,119,126);
			bNeedLayered=1;
			if((GetWindowLong(hDesk,GWL_EXSTYLE) & 0x00080000L)==0){//WS_EX_LAYERED
				::ShowWindow(hDesk,SW_HIDE);
				if(SetLayeredWindowStyle(hDesk,1)){_XLOG_
					SetWndAlpha(hDesk,cBgCol,FALSE);
				}
				::ShowWindow(hDesk,SW_SHOW);
				EnumWindows(RefreshWnd,0);
				::RedrawWindow(::FindWindow("Shell_TrayWnd",0),0,0,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
				//DWORD dwTarget=BSM_APPLICATIONS;
				//BroadcastSystemMessage(BSF_QUERY | BSF_FORCEIFHUNG | BSF_FLUSHDISK, &dwTarget, WM_PAINT, 0, 0);
			}
		}else{_XLOG_
			cBgCol=CLR_NONE;
		}
	}
	if(!bNeedLayered && (GetWindowLong(hDesk,GWL_EXSTYLE) & 0x00080000L)!=0){_XLOG_
		::ShowWindow(hDesk,SW_HIDE);
		SetLayeredWindowStyle(hDesk,0);
		::ShowWindow(hDesk,SW_SHOW);
		EnumWindows(RefreshWnd,0);
		::RedrawWindow(::FindWindow("Shell_TrayWnd",0),0,0,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
		//DWORD dwTarget=BSM_APPLICATIONS;
		//BroadcastSystemMessage(BSF_QUERY | BSF_FORCEIFHUNG | BSF_FLUSHDISK, &dwTarget, WM_PAINT, 0, 0);
	}
	//if(hADEnable==0)
	{_XLOG_
		ListView_SetTextBkColor(hTargetWindow, cBgCol);
		ListView_SetTextColor(hTargetWindow, cTxCol);
		ListView_RedrawItems(hTargetWindow, 0, ListView_GetItemCount(hTargetWindow) - 1);
	}
	return TRUE;
}

BOOL FitImageToRect(CRect& rt, CSize cBmpSize,BOOL bAllowCenter)
{_XLOG_
	int width=rt.Width();
	int height=rt.Height();
	if(bAllowCenter){_XLOG_
		if(rt.Width()>cBmpSize.cx && rt.Height()>cBmpSize.cy){_XLOG_
			rt.left=rt.left+(width-cBmpSize.cx)/2;
			rt.top=rt.top+(height-cBmpSize.cy)/2;
			rt.right=rt.left+cBmpSize.cx;
			rt.bottom=rt.top+cBmpSize.cy;
			return 0;
		}
	}
	int iNewwidth=width;
	int iNewheight=height;
	double dx=double(cBmpSize.cx)/double(width);
	double dy=double(cBmpSize.cy)/double(height);
	if(dx>dy){_XLOG_
		iNewwidth=int(cBmpSize.cx/dx);
		iNewheight=int(cBmpSize.cy/dx);
	}else{_XLOG_
		iNewwidth=int(cBmpSize.cx/dy);
		iNewheight=int(cBmpSize.cy/dy);
	}
	rt.left=rt.left+(width-iNewwidth)/2;
	rt.top=rt.top+(height-iNewheight)/2;
	rt.right=rt.left+iNewwidth;
	rt.bottom=rt.top+iNewheight;
	return 1;
}

// CopyBitmapToClipboard	- Copies a device-dependent bitmap to clipboard
// pWnd				- Pointer to window that opens the clipboard
// bitmap			- The device-dependent bitmap
// pPal				- Pointer to logical palette - Can be NULL
// NOTE				- GDI objects are detached from bitmap & pPal
//				  as the clipboard owns them after the copy
void CopyBitmapToClipboard( const CWnd *pWnd, CBitmap& bitmap, CPalette* pPal )
{_XLOG_
	::OpenClipboard(pWnd->GetSafeHwnd());
	::EmptyClipboard() ;
	if( pPal ){_XLOG_
		::SetClipboardData (CF_PALETTE, pPal->GetSafeHandle() ) ;
	}
	::SetClipboardData (CF_BITMAP, bitmap.GetSafeHandle() ) ;
	::CloseClipboard () ;
	bitmap.Detach();
	if( pPal ){_XLOG_
		pPal->Detach();
	}
}

//---------------------------------------------------------------------------
// Функция получения ширины/высоты графического файла формата BMP (*.bmp) :
bool GetBMPDim(const char* FileName, int& Width, int& Height)
{_XLOG_
	Width = Height = -1;
	FILE* hFile = fopen(FileName,"rb");
	if(!hFile) return false;
	BITMAPFILEHEADER   bmfh;
	BITMAPINFOHEADER   bmih;
	memset(&bmfh,0,sizeof(BITMAPFILEHEADER));
	memset(&bmih,0,sizeof(BITMAPINFOHEADER));
	if(!fread(&bmfh,sizeof(BITMAPFILEHEADER),1,hFile)) { //read header
		fclose(hFile);
		return false;
	}
	char type[3] = {0};
	memcpy (&type,&bmfh.bfType,2);
	type[2]='\0';
	if(strcmpi(type,"BM")!=0) { // bad file format
		fclose(hFile);
		return false;
	}
	if(!fread(&bmih,sizeof(BITMAPINFOHEADER),1,hFile)) { //read header
		fclose(hFile);
		return false;
	}
	Width = bmih.biWidth;
	Height = bmih.biHeight;
	fclose(hFile);
	return true;
}

//---------------------------------------------------------------------------
// Функция для получения ширины/высоты графического файла формата PNG (Portable Network Graphic) - *.png:
bool GetPNGDim(const char* FileName, int& Width, int& Height)
{_XLOG_
	Width = Height = -1;
	FILE* hFile = fopen(FileName,"rb");
	if(!hFile) return false;
	unsigned char sign[8]={0};
	if(!fread(sign,8,1,hFile)) {_XLOG_
		fclose(hFile);
		return false;
    }
	if(sign[0]==137 && sign[1]==80 && sign[2]==78 && sign[3]==71 &&
		sign[4]==13 && sign[5]==10 &&   sign[6]==26 && sign[7]==10) {_XLOG_
		// good signature
		unsigned int data_length; // chunk's data field length
		char  chunk_type[5] = {0};// A 4-byte chunk type code
		// - пятый байт добавлен как окончание строки
		if(!fread(&data_length,sizeof(DWORD),1,hFile)) {_XLOG_
			fclose(hFile);
			return false;
		}
		if(!fread(chunk_type,4,1,hFile)) {_XLOG_
			fclose(hFile);
			return false;
		}
		if(strcmpi(chunk_type,"IHDR")!=0) { // bad file format
			fclose(hFile);
			return false;
		}
		unsigned char buff[8]={0};
		if(!fread(&buff,8,1,hFile)) {_XLOG_
			fclose(hFile);
			return false;
		}
		Width = MAKELONG(MAKEWORD(buff[3],buff[2]),MAKEWORD(buff[1],buff[0]));
		Height = MAKELONG(MAKEWORD(buff[7],buff[6]),MAKEWORD(buff[5],buff[4]));
	} else {  // bad signature...
		fclose(hFile);
		return false;
    }
	fclose(hFile);
	return true;
}

//---------------------------------------------------------------------------
// Функция получения ширины/высоты графического файла формата JPEG (*.jpg, *.jpeg):
//---------------------------------------------------------------------------
long Mult(BYTE lsb, BYTE msb){_XLOG_
    return (lsb + (msb * 256) );
}

bool GetJpegDim(const char* FileName, int& Width, int& Height)
{_XLOG_
	Width = Height = -1;
	FILE* hFile = fopen(FileName,"rb");
	if(!hFile) 
		return false;
	long lPos=0;
	unsigned char bBuf[3]={0};
	bool bMarkerFound=false;
	bool bExitFlag=false;
	while(1) {_XLOG_
		if(feof(hFile)) {_XLOG_
			fclose(hFile); return false;
		}
		if(!fread(bBuf,3,1,hFile)) {_XLOG_
			fclose(hFile); return false;
		}
		if(bBuf[0] == 0xFF && bBuf[1] == 0xD8 && bBuf[2] == 0xFF) {_XLOG_
			bMarkerFound=true; break; // нашли нужный маркер
		}
		if(fseek(hFile,lPos+1,SEEK_SET)!=0) {_XLOG_
			fclose(hFile);return false;
		}
		lPos = ftell(hFile);
		if(lPos==-1L) {_XLOG_
			fclose(hFile); return false;
        }
	}//while
	if(!bMarkerFound) { // маркер не найден - возможно, это вовсе не JPEG
		fclose(hFile);
		return false;
    }
	if(fseek(hFile,lPos+2,SEEK_SET)!=0) {_XLOG_
		fclose(hFile);return false;
	}
	lPos = ftell(hFile);
	if(lPos==-1L) {_XLOG_
		fclose(hFile); return false;
	}
	while(1) {_XLOG_
		if(feof(hFile)) {_XLOG_
			fclose(hFile); return false;
		}
		while(1) {_XLOG_
			if(feof(hFile)) {_XLOG_
				fclose(hFile); return false;
			}
			if(!fread(bBuf,2,1,hFile)) {_XLOG_
				fclose(hFile); return false;
			}
			if( bBuf[0] == 0xFF && bBuf[1] != 0xFF) break;
			if(fseek(hFile,lPos+1,SEEK_SET)!=0) {_XLOG_
				fclose(hFile);return false;
			}
			lPos = ftell(hFile);
			if(lPos==-1L) {_XLOG_
				fclose(hFile); return false;
			}
		} //while
		if(fseek(hFile,lPos+1,SEEK_SET)!=0) {_XLOG_
			fclose(hFile);return false;
        }
		lPos = ftell(hFile);
		if(lPos==-1L) {_XLOG_
			fclose(hFile); return false;
        }
		if(!fread(bBuf,3,1,hFile)) {_XLOG_
			fclose(hFile); return false;
		}
		switch (bBuf[0]) {_XLOG_
		case 0xC0:
		case 0xC1:
		case 0xC2:
		case 0xC3:
		case 0xC5:
		case 0xC6:
		case 0xC7:
		case 0xC9:
		case 0xCA:
		case 0xCB:
		case 0xCD:
		case 0xCE:
		case 0xCF:
			if ((bBuf[1]==0)&&(bBuf[2]==17)){//нужно добавить
				bExitFlag=true;
			}
		}
		if(bExitFlag)break;
		if(fseek(hFile,lPos + Mult(bBuf[2], bBuf[1]),SEEK_SET)!=0) {_XLOG_
			fclose(hFile);return false;
		}
		lPos = ftell(hFile);
		if(lPos == -1L) {_XLOG_
			fclose(hFile);
			return false;
		}
	} //while
	if(fseek(hFile,lPos + 4,SEEK_SET)!=0) {_XLOG_
		fclose(hFile);return false;
	}
	if(fread(bBuf,2,1,hFile)) {_XLOG_
        //получаем высоту
        Height = Mult(bBuf[1], bBuf[0]);
		if(fread(bBuf,2,1,hFile)) {_XLOG_
			//получаем ширину
			Width = Mult(bBuf[1], bBuf[0]);
		}
	}
	fclose(hFile);
	return true;
}

//---------------------------------------------------------------------------
// Вот какой я спаммер  В общем, опять функция, на этот раз - получение Width/Height файла *.gif:
typedef struct _GifHeader {
	// Header
	BYTE Signature[3];     /* Header Signature (always "GIF") */
	BYTE Version[3];       /* GIF format version("87a" or "89a") */
	// Logical Screen Descriptor
	WORD ScreenWidth;      /* Width of Display Screen in Pixels */
	WORD ScreenHeight;     /* Height of Display Screen in Pixels */
	BYTE Packed;           /* Screen and Color Map Information */
	BYTE BackgroundColor;  /* Background Color Index */
	BYTE AspectRatio;      /* Pixel Aspect Ratio */
} GIFHEAD;

//---------------------------------------------------------------------------
bool GetGifDim(const char* FileName, int& Width, int& Height)
{_XLOG_
	Width = Height = -1;
	GIFHEAD   GifHead;
	memset(&GifHead,0,sizeof(GifHead));
	FILE* hFile = fopen(FileName,"rb");
	if(!hFile) return false;
	if(!fread(&GifHead,13,1,hFile)) { //read header
		fclose(hFile);
		return false;
	}
	char Sign[4] = {0};
	memcpy (&Sign,&GifHead.Signature,3);
	if(strcmpi(Sign,"GIF")!=0) { // bad signature
		fclose(hFile);
		return false;
	}
	memcpy (&Sign,&GifHead.Version,3);
	if(strcmpi(Sign,"87a")==0) {}
	else if(strcmpi(Sign,"89a")==0) {}
	else { // not a gif file
		fclose(hFile);
		return false;
	}
	Width = GifHead.ScreenWidth;
	Height = GifHead.ScreenHeight;
	fclose(hFile);
	return true;
}

// вспомогательные функции и структуры
//---------------------------------------------------------------------------
typedef struct {
    int xMin;
    int xMax;
    int yMin;
    int yMax;
	
}SWFtagRect;
//---------------------------------------------------------------------------
void SWFInitBits(unsigned int& SWFbitBuf , int& SWFbitPos)
{_XLOG_
	SWFbitBuf=0;
	SWFbitPos=0;
}
//---------------------------------------------------------------------------
unsigned char SWFGetByte(unsigned int& SWFbitBuf , int& SWFbitPos,
						 unsigned char* SWFfileBuf, unsigned int& SWFfilePos)
{_XLOG_
    SWFInitBits(SWFbitBuf , SWFbitPos);
    return SWFfileBuf[SWFfilePos++];
}
//---------------------------------------------------------------------------
unsigned int SWFGetBits(int n, unsigned int& SWFbitBuf , int& SWFbitPos,
                        unsigned char* SWFfileBuf, unsigned int& SWFfilePos)
						// Get n bits from the stream.
{_XLOG_
	
    unsigned int v = 0;
	
    while (true)
    {_XLOG_
		
        int s = n - SWFbitPos;
        if (s > 0)
        {_XLOG_
            // Consume the entire buffer
            v |= SWFbitBuf << s;
            n -= SWFbitPos;
			
            // Get the next buffer
            SWFbitBuf = SWFGetByte(SWFbitBuf, SWFbitPos, SWFfileBuf, SWFfilePos);
            SWFbitPos = 8;
        }
        else
        {_XLOG_
            // Consume a portion of the buffer
            v |= SWFbitBuf >> -s;
            SWFbitPos -= n;
            SWFbitBuf &= 0xff >> (8 - SWFbitPos); // mask off the consumed bits
            return v;
        }
    }
}
//---------------------------------------------------------------------------
int SWFGetSBits(int n, unsigned int& SWFbitBuf , int& SWFbitPos,
				unsigned char* SWFfileBuf, unsigned int& SWFfilePos)
				// Get n bits from the string with sign extension.
{_XLOG_
    // Get the number as an unsigned value.
    int v = (int) SWFGetBits(n, SWFbitBuf,SWFbitPos, SWFfileBuf,SWFfilePos);
	
    // Is the number negative?
    if (v & (1L << (n - 1)))
    {_XLOG_
        // Yes. Extend the sign.
        v |= -1L << n;
    }
	
    return v;
}
//---------------------------------------------------------------------------
void SWFGetRect (SWFtagRect * r, unsigned int& SWFbitBuf , int& SWFbitPos,
				 unsigned char* SWFfileBuf, unsigned int& SWFfilePos)
{_XLOG_
    SWFInitBits(SWFbitBuf,SWFbitPos);
    int nBits = (int) SWFGetBits(5, SWFbitBuf,SWFbitPos,SWFfileBuf,SWFfilePos);
    r->xMin = SWFGetSBits(nBits, SWFbitBuf,SWFbitPos,SWFfileBuf,SWFfilePos);
    r->xMax = SWFGetSBits(nBits, SWFbitBuf,SWFbitPos,SWFfileBuf,SWFfilePos);
    r->yMin = SWFGetSBits(nBits, SWFbitBuf,SWFbitPos,SWFfileBuf,SWFfilePos);
    r->yMax = SWFGetSBits(nBits, SWFbitBuf,SWFbitPos,SWFfileBuf,SWFfilePos);
}
//---------------------------------------------------------------------------
// собственно функция
//---------------------------------------------------------------------------
bool GetSWFDim(const char* FileName, int& Width, int& Height)
{_XLOG_
	Width = Height = -1;
	
	FILE* hFile = fopen(FileName,"rb");
	if(!hFile) return false;
	
	
	unsigned char fHdr[8]={0}; // file header
	
	if(!fread(fHdr,8,1,hFile)) {_XLOG_
		fclose(hFile);
		return false;
    }
	
	if((fHdr[0]!='F' && fHdr[0]!='C') || fHdr[1]!='W' || fHdr[2] !='S') {_XLOG_
		// bad header
		fclose(hFile);
		return false;
    }
	
	int FFileSize = fHdr[4] | (fHdr[5] << 8) | (fHdr[6] << 16) | (fHdr[7] << 24);
	
	
	if(FFileSize<29) { // размер Flash-ролика не может быть меньше 29 байт
		fclose(hFile);
		return false;
    }
	
	DWORD c=40;
	if(fHdr[0]=='C'){_XLOG_
		c=FFileSize;
	}else{_XLOG_
		if(FFileSize<40)
			c = FFileSize - 8;
	}
	unsigned char* SWFfileBuf = new unsigned char[c];
	memset(SWFfileBuf,0,c);
	unsigned int SWFfilePos = 0;
	unsigned int SWFbitBuf = 0;
	int SWFbitPos = 0;
	int iAvailIn=(int)fread(SWFfileBuf,1,c,hFile);
	if(iAvailIn!=int(c) && fHdr[0]!='C') {_XLOG_
		delete [] SWFfileBuf;
		fclose(hFile);
		return false;
    }
#ifdef ZIP_SUPPORT
	if(fHdr[0]=='C'){_XLOG_
		// Распаковываем
		LPBYTE pTarget;
		ATLTRY(pTarget = new BYTE[FFileSize+100]);
		_ASSERTE(pTarget);
		if( pTarget == NULL ) return false;
		memset(pTarget,0,FFileSize+100);
		z_stream stream = { 0 };
		stream.next_in = (Bytef*) SWFfileBuf;
		stream.avail_in = (uInt) iAvailIn;
		stream.next_out = (Bytef*) pTarget;
		stream.avail_out = FFileSize+100;
		stream.zalloc = (alloc_func) NULL;
		stream.zfree = (free_func) NULL;
		// Perform inflation; wbits < 0 indicates no zlib header inside the data.
		int err = inflateInit2(&stream, MAX_WBITS);
		if( err == Z_OK ) {_XLOG_
			err = inflate(&stream, Z_NO_FLUSH);
			if( err == Z_STREAM_END) err = Z_OK;
			inflateEnd(&stream);
		}
		delete[] SWFfileBuf;
		SWFfileBuf = pTarget;
	}
#endif
	SWFtagRect rect;
	
	SWFGetRect(&rect, SWFbitBuf, SWFbitPos,SWFfileBuf,SWFfilePos);
	
	Width = (rect.xMax - rect.xMin)/20;
	Height = (rect.yMax - rect.yMin)/20;
	
	
	delete [] SWFfileBuf;
	
	fclose(hFile);
	return true;
}
//---------------------------------------------------------------------------


bool GetImageSizeFromFile(const char* FileName, int& Width, int& Height)
{_XLOG_
	Width=Height=0;
	if(FileName==NULL || *FileName==0){_XLOG_
		return false;
	}
	const char* szDot=strrchr(FileName,'.');
	if(szDot==NULL || *szDot==0){_XLOG_
		return false;
	}
	szDot++;
	if(stricmp(szDot,"bmp")==0){_XLOG_
		return GetBMPDim(FileName,Width, Height);
	}else if(stricmp(szDot,"png")==0){_XLOG_
		return GetPNGDim(FileName,Width, Height);
	}else if(stricmp(szDot,"jpg")==0 || stricmp(szDot,"jpeg")==0 || stricmp(szDot,"jpe")==0){_XLOG_
		return GetJpegDim(FileName,Width, Height);
	}else if(stricmp(szDot,"gif")==0){_XLOG_
		return GetGifDim(FileName,Width, Height);
	}else if(stricmp(szDot,"swf")==0){_XLOG_
		return GetSWFDim(FileName,Width, Height);
	}else if(stricmp(szDot,"wcclock")==0){_XLOG_
		return GetSWFDim(FileName,Width, Height);		
	}
	return false;	
}

typedef BOOL (WINAPI *_SaveToJpg)(const char*,HBITMAP,BYTE);
BOOL WINAPI SaveBitmapToJpg(const char* sPictureFile, HBITMAP bmp, BYTE bQuality)
{_XLOG_
	BOOL bRes=FALSE;
	HINSTANCE hUtils=LoadLibrary("WKUtils.dll");
	if(hUtils){_XLOG_
		_SaveToJpg fp=(_SaveToJpg)DBG_GETPROC(hUtils,"SaveBitmapToJpg");
		if(fp){_XLOG_
			bRes=(*fp)(sPictureFile, bmp, bQuality);
		}
	}
	FreeLibrary(hUtils);
	return bRes;
}

//
//    BitmapToRegion :    Create a region from the "non-transparent" pixels of a bitmap
//    Author :            Jean-Edouard Lachand-Robert (http://www.geocities.com/Paris/LeftBank/1160/resume.htm), June 1998.
//
//    hBmp :                Source bitmap
//    cTransparentColor :    Color base for the "transparent" pixels (default is black)
//    cTolerance :        Color tolerance for the "transparent" pixels.
//
//    A pixel is assumed to be transparent if the value of each of its 3 components (blue, green and red) is 
//    greater or equal to the corresponding value in cTransparentColor and is lower or equal to the 
//    corresponding value in cTransparentColor + cTolerance.
//
HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{_XLOG_
    HRGN hRgn = NULL;
	
    if (hBmp)
    {_XLOG_
        // Create a memory DC inside which we will scan the bitmap content
        HDC hMemDC = CreateCompatibleDC(NULL);
        if (hMemDC)
        {_XLOG_
            // Get bitmap size
            BITMAP bm;
            GetObject(hBmp, sizeof(bm), &bm);
			
            // Create a 32 bits depth bitmap and select it into the memory DC 
            BITMAPINFOHEADER RGB32BITSBITMAPINFO = {    
				sizeof(BITMAPINFOHEADER),    // biSize 
                    bm.bmWidth,                    // biWidth; 
                    bm.bmHeight,                // biHeight; 
                    1,                            // biPlanes; 
                    32,                            // biBitCount 
                    BI_RGB,                        // biCompression; 
                    0,                            // biSizeImage; 
                    0,                            // biXPelsPerMeter; 
                    0,                            // biYPelsPerMeter; 
                    0,                            // biClrUsed; 
                    0                            // biClrImportant; 
            };
            VOID * pbits32; 
            HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
            if (hbm32)
            {_XLOG_
                HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

                // Create a DC just to copy the bitmap into the memory DC
                HDC hDC = CreateCompatibleDC(hMemDC);
                if (hDC)
                {_XLOG_
                    // Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
                    BITMAP bm32;
                    GetObject(hbm32, sizeof(bm32), &bm32);
                    while (bm32.bmWidthBytes % 4)
                        bm32.bmWidthBytes++;

                    // Copy the bitmap into the memory DC
                    HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
                    BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

                    // For better performances, we will use the ExtCreateRegion() function to create the
                    // region. This function take a RGNDATA structure on entry. We will add rectangles by
                    // amount of ALLOC_UNIT number in this structure.
                    #define ALLOC_UNIT    100
                    DWORD maxRects = ALLOC_UNIT;
                    HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
                    RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
                    pData->rdh.dwSize = sizeof(RGNDATAHEADER);
                    pData->rdh.iType = RDH_RECTANGLES;
                    pData->rdh.nCount = pData->rdh.nRgnSize = 0;
                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

                    // Keep on hand highest and lowest values for the "transparent" pixels
                    BYTE lr = GetRValue(cTransparentColor);
                    BYTE lg = GetGValue(cTransparentColor);
                    BYTE lb = GetBValue(cTransparentColor);
                    BYTE hr = min(0xff, lr + GetRValue(cTolerance));
                    BYTE hg = min(0xff, lg + GetGValue(cTolerance));
                    BYTE hb = min(0xff, lb + GetBValue(cTolerance));

                    // Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
                    BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
                    for (int y = 0; y < bm.bmHeight; y++)
                    {_XLOG_
                        // Scan each bitmap pixel from left to right
                        for (int x = 0; x < bm.bmWidth; x++)
                        {_XLOG_
                            // Search for a continuous range of "non transparent pixels"
                            int x0 = x;
                            LONG *p = (LONG *)p32 + x;
                            while (x < bm.bmWidth)
                            {_XLOG_
                                BYTE b = GetRValue(*p);
                                if (b >= lr && b <= hr)
                                {_XLOG_
                                    b = GetGValue(*p);
                                    if (b >= lg && b <= hg)
                                    {_XLOG_
                                        b = GetBValue(*p);
                                        if (b >= lb && b <= hb)
                                            // This pixel is "transparent"
                                            break;
                                    }
                                }
                                p++;
                                x++;
                            }

                            if (x > x0)
                            {_XLOG_
                                // Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
                                if (pData->rdh.nCount >= maxRects)
                                {_XLOG_
                                    GlobalUnlock(hData);
                                    maxRects += ALLOC_UNIT;
                                    hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
                                    pData = (RGNDATA *)GlobalLock(hData);
                                }
                                RECT *pr = (RECT *)&pData->Buffer;
                                SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
                                if (x0 < pData->rdh.rcBound.left)
                                    pData->rdh.rcBound.left = x0;
                                if (y < pData->rdh.rcBound.top)
                                    pData->rdh.rcBound.top = y;
                                if (x > pData->rdh.rcBound.right)
                                    pData->rdh.rcBound.right = x;
                                if (y+1 > pData->rdh.rcBound.bottom)
                                    pData->rdh.rcBound.bottom = y+1;
                                pData->rdh.nCount++;

                                // On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
                                // large (ie: > 4000). Therefore, we have to create the region by multiple steps.
                                if (pData->rdh.nCount == 2000)
                                {_XLOG_
                                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
                                    if (hRgn)
                                    {_XLOG_
                                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                                        DeleteObject(h);
                                    }
                                    else
                                        hRgn = h;
                                    pData->rdh.nCount = 0;
                                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
                                }
                            }
                        }

                        // Go to next row (remember, the bitmap is inverted vertically)
                        p32 -= bm32.bmWidthBytes;
                    }

                    // Create or extend the region with the remaining rectangles
                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
                    if (hRgn)
                    {_XLOG_
                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                        DeleteObject(h);
                    }
                    else
                        hRgn = h;

                    // Clean up
                    GlobalFree(hData);
                    SelectObject(hDC, holdBmp);
                    DeleteDC(hDC);
                }

                DeleteObject(SelectObject(hMemDC, holdBmp));
            }

            DeleteDC(hMemDC);
        }    
    }

    return hRgn;
}

void DrawTransparent9x(CDC * pDC, int x, int y, int nWidth, int nHeight, int x2, int y2, int cx2, int cy2, CBitmap* bmp, COLORREF crColour)
{_XLOG_
	COLORREF crOldBack = pDC->SetBkColor(RGB(255,255,255));
	COLORREF crOldText = pDC->SetTextColor(0);
	CDC dcImage, dcTrans;
	
	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);
	
	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage = dcImage.SelectObject(bmp);
	
	// Create the mask bitmap
	CSize sz=GetBitmapSize(bmp);
	CBitmap bitmapTrans;
	bitmapTrans.CreateBitmap(sz.cx, sz.cy, 1, 1, NULL);
	
	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);
	
	// Build mask based on transparent colour
	dcImage.SetBkColor(crColour);
	dcTrans.BitBlt(0, 0, sz.cx, sz.cy, &dcImage, 0, 0, SRCCOPY);
	
	// Do the work - True Mask method - cool if not actual display
	pDC->BitBlt(x, y, cx2, cy2, &dcImage, x2, y2, SRCINVERT);
	pDC->BitBlt(x, y, cx2, cy2, &dcTrans, x2, y2, SRCAND);
	pDC->BitBlt(x, y, cx2, cy2, &dcImage, x2, y2, SRCINVERT);
	
	// Restore settings
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);
}


class CMsImg
{
public:
	BOOL bInit;
	HINSTANCE h;
	CMsImg()
	{_XLOG_
		bInit=0;
	};
	~CMsImg()
	{_XLOG_
		if(h){_XLOG_
			FreeLibrary(h);
		}
	};
	void Init()
	{_XLOG_
		bInit=1;
		h=LoadLibrary("Msimg32.dll");
	}
};
CMsImg msWrapper;

class CMsImgU32
{
public:
	BOOL bInit;
	HINSTANCE h;
	CMsImgU32()
	{_XLOG_
		bInit=0;
	};
	~CMsImgU32()
	{_XLOG_
		if(h){_XLOG_
			FreeLibrary(h);
		}
	};
	void Init()
	{_XLOG_
		bInit=1;
		h=LoadLibrary("user32.dll");
	}
};
CMsImgU32 msWrapper32;

typedef BOOL (WINAPI *_AlphaBlend)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
typedef BOOL (WINAPI *_TransparentBlt)(HDC,int,int,int,int,HDC,int,int,int,int,UINT);
typedef BOOL (WINAPI *_UpdateLayeredWindow)(HWND hwnd,HDC hdcDst,POINT *pptDst,SIZE *psize,HDC hdcSrc,POINT *pptSrc,COLORREF crKey,BLENDFUNCTION *pblend,DWORD dwFlags);
BOOL WINAPI TransparentBltMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,int cx2,int cy2,UINT iTransCol)
{_XLOG_
	if(isWin9x())
	{_XLOG_
		// Делаем imagelis
		HBITMAP hTemp=::CreateBitmap(1, 1, 1, 1, NULL);
		HBITMAP bmp=(HBITMAP)::SelectObject(hFrom,hTemp);
		CDC dc2;
		dc2.Attach(hTo);
		CBitmap bmp2;
		bmp2.Attach(bmp);
		DrawTransparent9x(&dc2,x,y,cx,cy,x2,y2,cx2,cy2,&bmp2,RGB(255,255,255));
		bmp2.Detach();
		dc2.Detach();
		::SelectObject(hFrom,bmp);
		ClearBitmap(hTemp);
		return TRUE;
	}
	BOOL bRes=FALSE;
	if(!msWrapper.bInit){_XLOG_
		msWrapper.Init();
	}
	if(msWrapper.h){_XLOG_
		_TransparentBlt fp=(_TransparentBlt)DBG_GETPROC(msWrapper.h,"TransparentBlt");
		if(fp){_XLOG_
			bRes=(*fp)(hTo,x,y,cx,cy,hFrom,x2,y2,cx2,cy2,iTransCol);
		}
		//FreeLibrary(h);
	}
	if(!bRes){_XLOG_
		// Без прозрачности
		if(cx==cx2 && cy==cy2){_XLOG_
			::BitBlt(hTo,x, y, cx, cy, hFrom, x2, y2, SRCCOPY);
		}else{_XLOG_
			::StretchBlt(hTo,x, y, cx, cy, hFrom, x2, y2, cx2, cy2, SRCCOPY);
		}
		return TRUE;
	}
	return bRes;
}

BOOL WINAPI TransparentBltMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,UINT iTransCol)
{_XLOG_
	return TransparentBltMy(hTo,x,y,cx,cy,hFrom,x2,y2,cx,cy,iTransCol);
}

void DrawSemiTransparentBitmap9x(CDC *pDstDC, int x, int y, int nWidth, int nHeight,
    CDC* pSrcDC, int xSrc, int ySrc)
{_XLOG_
    CDC dcCompatible;
    CBitmap *pBitmapOld;
    CBitmap bm;

    dcCompatible.CreateCompatibleDC(pDstDC);
    bm.CreateCompatibleBitmap(pDstDC, nWidth, nHeight);
    pBitmapOld = dcCompatible.SelectObject(&bm);
    dcCompatible.FillSolidRect(CRect(0, 0, nWidth, nHeight), RGB(0x7F, 0x7F, 0x7F));
    pDstDC->BitBlt(x, y, nWidth, nHeight, &dcCompatible, 0, 0, SRCAND);
    dcCompatible.SelectObject(pBitmapOld);

    pDstDC->BitBlt(x, y, nWidth, nHeight, pSrcDC, 0, 0, SRCPAINT);
}

BOOL WINAPI AlphaBlendMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,int cx2,int cy2,BLENDFUNCTION bl)
{_XLOG_
	if(bl.SourceConstantAlpha==0){_XLOG_
		return 1;
	}
	BOOL bRes=FALSE;
	if(bl.SourceConstantAlpha<255){_XLOG_
		if(isWin9x()){_XLOG_
			for(int i=0;i<cx;i++){_XLOG_
				for(int j=0;j<cy;j++){_XLOG_
					DWORD dwC1=GetPixel(hFrom,x2+i,y2+j);
					DWORD dwC2=GetPixel(hTo,x+i,y+j);
					BYTE bR=(GetRValue(dwC1)*bl.SourceConstantAlpha+GetRValue(dwC2)*(255-bl.SourceConstantAlpha))>>8;
					BYTE bG=(GetGValue(dwC1)*bl.SourceConstantAlpha+GetGValue(dwC2)*(255-bl.SourceConstantAlpha))>>8;
					BYTE bB=(GetBValue(dwC1)*bl.SourceConstantAlpha+GetBValue(dwC2)*(255-bl.SourceConstantAlpha))>>8;
					::SetPixel(hTo,x+i,y+j,RGB(bR,bG,bB));
				}
			}
			return TRUE;
		}
		if(!msWrapper.bInit){_XLOG_
			msWrapper.Init();
		}
		if(msWrapper.h){_XLOG_
			_AlphaBlend fp=(_AlphaBlend)DBG_GETPROC(msWrapper.h,"AlphaBlend");
			if(fp){_XLOG_
				bRes=(*fp)(hTo,x,y,cx,cy,hFrom,x2,y2,cx2,cy2,bl);
			}
		}
	}
	if(!bRes){_XLOG_
		// Без прозрачности
		if(cx==cx2 && cy==cy2){_XLOG_
			::BitBlt(hTo,x, y, cx, cy, hFrom, x2, y2, SRCCOPY);
		}else{_XLOG_
			::StretchBlt(hTo,x, y, cx, cy, hFrom, x2, y2, cx2, cy2, SRCCOPY);
		}
		return TRUE;
	}
	return bRes;
}

BOOL WINAPI UpdateLayeredWindowMy(HWND hwnd,HDC hdcDst,POINT *pptDst,SIZE *psize,HDC hdcSrc,POINT *pptSrc,COLORREF crKey,BLENDFUNCTION *pblend,DWORD dwFlags)
{_XLOG_
	BOOL bRes=0;
	if(!msWrapper32.bInit){_XLOG_
		msWrapper32.Init();
	}
	if(msWrapper32.h){_XLOG_
		_UpdateLayeredWindow fp=(_UpdateLayeredWindow)DBG_GETPROC(msWrapper32.h,"UpdateLayeredWindow");
		if(fp){_XLOG_
			bRes=(*fp)(hwnd,hdcDst,pptDst,psize,hdcSrc,pptSrc,crKey,pblend,dwFlags);
		}
	}
	return bRes;
}

BOOL WINAPI AlphaBlendMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,BLENDFUNCTION bl)
{_XLOG_
	return AlphaBlendMy(hTo,x,y,cx,cy,hFrom,x2,y2,cx,cy,bl);
}

BOOL WINAPI AlphaBlendMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,int iA)
{_XLOG_
	BLENDFUNCTION blendFunction;
	blendFunction.BlendFlags=0;
	blendFunction.AlphaFormat=0;
	blendFunction.BlendOp=AC_SRC_OVER;
	blendFunction.SourceConstantAlpha=iA;
	return AlphaBlendMy(hTo,x,y,cx,cy,hFrom,x2,y2,cx,cy,blendFunction);
}

#ifdef _DEBUG
bool fakemultix=false;
#endif
BOOL MakeImageCobble(HBITMAP*& hbmOutImage,CSize scrSize,int iTile)
{_XLOG_
#ifdef _DEBUG
	if(fakemultix){_XLOG_
		return 1;
	}
#endif
	CDC dc;
	CDesktopDC dcDesk;
	dc.CreateCompatibleDC(&dcDesk);
	HBITMAP hNew=CreateCompatibleBitmap(dcDesk,scrSize.cx,scrSize.cy);
	CBitmap bmpNewTmp;
	bmpNewTmp.Attach(hNew);
	CBitmap* bmOld=dc.SelectObject(&bmpNewTmp);
	bmpNewTmp.Detach();
	CRect rt(0,0,scrSize.cx,scrSize.cy);
	if(iTile==2){_XLOG_
		CDC dc2;
		dc2.CreateCompatibleDC(&dcDesk);
		CBitmap bmOut;
		bmOut.Attach(*hbmOutImage);
		CBitmap* bmOld2=dc2.SelectObject(&bmOut);
		CSize bsz=GetBitmapSize(&bmOut);
		dc.SetStretchBltMode(COLORONCOLOR);
		long lXSign=1;
		long lYSign=1;
		int iXCount=scrSize.cx/bsz.cx+1;
		int iYCount=scrSize.cy/bsz.cy+1;
		for(int i=0;i<iXCount;i++){_XLOG_
			for(int j=0;j<iYCount;j++){_XLOG_
				dc.StretchBlt(i*bsz.cx+((lXSign>0)?0:bsz.cx), j*bsz.cy+((lYSign>0)?0:bsz.cy), lXSign*(bsz.cx+1), lYSign*(bsz.cy+1), &dc2, 0, 0, bsz.cx, bsz.cy, SRCCOPY);
				lYSign=-lYSign;
			}
			lXSign=-lXSign;
			lYSign=1;
		}
		dc2.SelectObject(bmOld2);
		bmOut.Detach();
	}else if(iTile==1){_XLOG_
		CBrush br;
		CBitmap bmpT;
		bmpT.Attach(*hbmOutImage);
		br.CreatePatternBrush(&bmpT);
		dc.FillRect(&rt,&br);
		bmpT.Detach();
	}else if(iTile==3){_XLOG_
		CDC dc2;
		dc2.CreateCompatibleDC(&dcDesk);
		CBitmap bmpT;
		bmpT.Attach(*hbmOutImage);
		CBitmap* bmOut=&bmpT;
		CBitmap* bmOld2=dc2.SelectObject(bmOut);
		CSize bsz=GetBitmapSize(bmOut);
		dc.SetStretchBltMode(COLORONCOLOR);
		dc.StretchBlt((scrSize.cx-bsz.cx)/2, (scrSize.cy-bsz.cy)/2, bsz.cx, bsz.cy, &dc2, 0, 0, bsz.cx, bsz.cy, SRCCOPY);
		dc2.SelectObject(bmOld2);
		bmpT.Detach();
	}else{_XLOG_
		CDC dc2;
		dc2.CreateCompatibleDC(&dcDesk);
		CBitmap bmpT;
		bmpT.Attach(*hbmOutImage);
		CBitmap* bmOut=&bmpT;
		CBitmap* bmOld2=dc2.SelectObject(bmOut);
		CSize bsz=GetBitmapSize(bmOut);
		dc.SetStretchBltMode(COLORONCOLOR);
		dc.StretchBlt(0, 0, scrSize.cx, scrSize.cy, &dc2, 0, 0, bsz.cx, bsz.cy, SRCCOPY);
		dc2.SelectObject(bmOld2);
		bmpT.Detach();
	}
	dc.SelectObject(bmOld);
	ClearBitmap(*hbmOutImage);
	*hbmOutImage=hNew;
	return TRUE;
}

BOOL MakeImageCobble(CBitmap& bmOutImage,CSize scrSize,int iTile)
{_XLOG_
#ifdef _DEBUG
	if(fakemultix){_XLOG_
		return 1;
	}
#endif
	HBITMAP bmp=(HBITMAP)bmOutImage.Detach();
	HBITMAP* pbmp=&bmp;
	BOOL bRes=MakeImageCobble(pbmp,scrSize,iTile);
	bmOutImage.Attach(*pbmp);
	return bRes;
}

/*
//#include "hookpaint.h"
BOOL GetScreenShotInBuffer(HWND hWnd, CBitmap*& bmScreenshot, CSize sz)
{_XLOG_
	// Вытаскиваем скриншот!
	/ *
	UINT msg=RegisterWindowMessage("WK_GETSNAP");
	DWORD dwResults=0;
	LRESULT lRes=::SendMessageTimeout(hWnd,msg, 0, 0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 1000, &dwResults);
	* /
	int lRes=0;
	CDC dc;
	CRect rtW;
	CRect rtO(CPoint(0,0),sz);
	::GetWindowRect(hWnd,&rtW);
	FitImageToRect(rtO,CSize(rtW.Width(),rtW.Height()));
	sz.cx=rtO.Width();
	sz.cy=rtO.Height();
	//CPaintHook hPrintClient;
	//hPrintClient.SubClass(hWnd);
	CWnd* pWnd=CWnd::FromHandle(hWnd);
	CDC* pDC=pWnd->GetWindowDC();
	if(pDC){// Шлем WM_PRINT
		CRect rt,rtWin;
		CDC dcMem;
		CBitmap bmWindow;
		dcMem.CreateCompatibleDC(pDC);
		bmWindow.CreateCompatibleBitmap(pDC,rtW.Width(),rtW.Height());
		CBitmap* bmpOld=dcMem.SelectObject(&bmWindow);
		SendMessage(hWnd,WM_PRINT,WPARAM(dcMem.GetSafeHdc()),LPARAM(PRF_OWNED|PRF_CHILDREN|PRF_CLIENT|PRF_ERASEBKGND|PRF_NONCLIENT));
		SendMessage(hWnd,WM_PRINTCLIENT,WPARAM(dcMem.GetSafeHdc()),LPARAM(PRF_OWNED|PRF_CHILDREN|PRF_CLIENT|PRF_ERASEBKGND|PRF_NONCLIENT));
		// Маштабим битмап по размеру
		CDC dcMem2;
		if(bmScreenshot){_XLOG_
			delete bmScreenshot;
			bmScreenshot=NULL;
		}
		bmScreenshot=new CBitmap();
		bmScreenshot->CreateCompatibleBitmap(pDC,sz.cx,sz.cy);
		dcMem2.CreateCompatibleDC(pDC);
		CBitmap* bmOld2=dcMem2.SelectObject(bmScreenshot);
		dcMem2.SetStretchBltMode(HALFTONE);
		dcMem2.StretchBlt(0,0,sz.cx,sz.cy,&dcMem,0,0,rtW.Width(),rtW.Height(),SRCCOPY);
		dcMem2.SelectObject(bmOld2);
		dcMem.SelectObject(bmpOld);
		pWnd->ReleaseDC(pDC);
		lRes=1;
	}
	if(lRes==0){// Вариант 3
		HRESULT hr=0;
		bmScreenshot=new CBitmap();
		CDesktopDC dcDesk;
		dc.CreateCompatibleDC(&dcDesk);
		bmScreenshot->CreateCompatibleBitmap(&dcDesk,rtW.Width(),rtW.Height());
		CBitmap* bmOld=dc.SelectObject(bmScreenshot);
		CComPtr<IUnknown> ptr=CWnd::FromHandle(hWnd)->GetControlUnknown();
		if(ptr){_XLOG_
			hr=OleDraw(ptr,DVASPECT_DOCPRINT,dc.GetSafeHdc(),&rtW);
		}
		dc.SelectObject(bmOld);
	}
	return TRUE;
}*/

CRect rtScreenRect;
CRect rtPrevScrRect;
CString sPrevText;
BOOL StartScreenshotRect(CRect& Rectangle, CRect& Screen)
{_XLOG_
	rtScreenRect.CopyRect(Screen);
	rtPrevScrRect.CopyRect(Rectangle);
	DrawScreenshotRect(rtPrevScrRect,"");
	return TRUE;
}

BOOL ContScreenshotRect(CRect& Rectangle, const char* szTxt)
{_XLOG_
	DrawScreenshotRect(rtPrevScrRect,sPrevText);
	rtPrevScrRect.CopyRect(Rectangle);
	sPrevText=szTxt;
	DrawScreenshotRect(rtPrevScrRect,sPrevText);
	return TRUE;
}

BOOL StopScreenshotRect()
{_XLOG_
	if(!rtPrevScrRect.IsRectEmpty()){_XLOG_
		DrawScreenshotRect(rtPrevScrRect,sPrevText);
	}
	rtPrevScrRect.left=rtPrevScrRect.right=rtPrevScrRect.top=rtPrevScrRect.bottom=0;
	sPrevText="";
	return TRUE;
}

BOOL DrawScreenshotRect(CRect& Rectangle, const char* szTxt)
{_XLOG_
	CDC dc;
	dc.CreateDC("DISPLAY", NULL, NULL, NULL);
	dc.SetROP2(R2_NOT);
	//dc.DrawFocusRect(&Rectangle);
	CPen pn(PS_SOLID,1,RGB(0,0,0));
	CPen* pn2=dc.SelectObject(&pn);
	CRect rtLines(Rectangle);
	rtLines.NormalizeRect();
	dc.MoveTo(rtLines.right,rtScreenRect.top);
	dc.LineTo(rtLines.right,rtScreenRect.bottom);
	dc.MoveTo(rtLines.left,rtScreenRect.top);
	dc.LineTo(rtLines.left,rtScreenRect.bottom);
	dc.MoveTo(rtScreenRect.left,rtLines.top);
	dc.LineTo(rtScreenRect.right,rtLines.top);
	dc.MoveTo(rtScreenRect.left,rtLines.bottom);
	dc.LineTo(rtScreenRect.right,rtLines.bottom);
	if(szTxt && strlen(szTxt)>0){_XLOG_
		CRect rtText(0,0,0,100);
		CDC dc2;
		CBitmap bmp;
		CDesktopDC dcDesk;
		dc2.CreateCompatibleDC(&dcDesk);
		dc2.SetROP2(R2_NOT);
		CFont* pFont=CWnd::GetDesktopWindow()->GetFont();
		SmartDrawText(dc2, szTxt, rtText, pFont, RGB(255,255,255), RGB(0,0,0), TXSHADOW_CALCRC);//TXSHADOW_VCENT|TXSHADOW_HCENT
		rtText.OffsetRect(-rtText.left,-rtText.top);
		rtText.right+=10;
		rtText.bottom+=10;
		bmp.CreateCompatibleBitmap(&dcDesk,rtText.Width(),rtText.Height());
		rtText.left+=5;
		rtText.top+=5;
		CBitmap* bmTmp=dc2.SelectObject(&bmp);
		SmartDrawText(dc2, szTxt, rtText, pFont, RGB(255,255,255), RGB(0,0,0), TXSHADOW_MIXBG|TXSHADOW_GLOW);//TXSHADOW_VCENT|TXSHADOW_HCENT
		dc.BitBlt(rtLines.right,rtLines.bottom,rtText.Width(),rtText.Height(),&dc2,0,0,SRCINVERT);
		dc2.SelectObject(bmTmp);
	}
	dc.SelectObject(pn2);
	return TRUE;
}

BYTE limit2Byte(int iVal)
{_XLOG_
	if(iVal>255){_XLOG_
		iVal=255;
	}
	if(iVal<0){_XLOG_
		iVal=0;
	}
	return iVal;
}

#ifndef CSafeDC
CSafeDC::CSafeDC():CDC()
{_XLOG_
};

CSafeDC::~CSafeDC()
{_XLOG_
	//HDC dc=Detach();
	TRACE3("\n%s(%lu): CSafeDC delete =%08X\n\n",__FILE__,__LINE__,m_hDC);
	//::DeleteObject(m_hDC);
	::DeleteDC(m_hDC);
	m_hAttribDC=NULL;
	m_hDC=NULL;
	/*SelectObject((CBitmap*)NULL);
	HDC dc=Detach();
	::DeleteDC(dc);*/
};

BOOL CSafeDC::CreateCompatibleDC(CDC* dc,const char* sz)
{_XLOG_
	m_hDC=::CreateCompatibleDC(*dc);
	m_hAttribDC=m_hDC;
	//BOOL bRes=CDC::CreateCompatibleDC(dc);
	TRACE3("\n%s(%lu): CSafeDC create =%08X",__FILE__,__LINE__,m_hDC);
	TRACE2(" CSafeDC create =%s %i\n\n",sz,m_hDC!=NULL);
	return m_hDC!=NULL;
}

CBitmap* CSafeDC::SelectObject(CBitmap* bmp)
{_XLOG_
	CBitmap* bmpRes=CDC::SelectObject(bmp);
	TRACE3("\n%s(%lu): CSafeDC bmp dc =%08X\n",__FILE__,__LINE__,m_hDC);
	TRACE3("\n%s(%lu): CSafeDC bmp in =%08X\n",__FILE__,__LINE__,HBITMAP(*bmp));
	TRACE3("\n%s(%lu): CSafeDC bmp ou =%08X\n\n",__FILE__,__LINE__,HBITMAP(*bmpRes));
	return bmpRes;
}

CFont* CSafeDC::SelectObject(CFont* bmp)
{_XLOG_
	CFont* bmpRes=CDC::SelectObject(bmp);
	TRACE3("\n%s(%lu): CSafeDC fnt dc =%08X\n",__FILE__,__LINE__,m_hDC);
	TRACE3("\n%s(%lu): CSafeDC fnt in =%08X\n",__FILE__,__LINE__,HFONT(*bmp));
	TRACE3("\n%s(%lu): CSafeDC fnt ou =%08X\n\n",__FILE__,__LINE__,HFONT(*bmpRes));
	return bmpRes;
}
#endif

BYTE RGB2BW_Fx(BYTE r, BYTE g, BYTE b)
{_XLOG_
        return BYTE((r+g+b+255)>>2);
}

BYTE RGB2BW_Fx(COLORREF rgb)
{_XLOG_
        return RGB2BW_Fx(GetRValue(rgb),GetGValue(rgb),GetBValue(rgb));
}

void DrawBitmapAlpha(HDC hdc, HBITMAP* hBmp, int x, int y, CSize& sz, HBITMAP* hBmpMask) 
{_XLOG_
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP hTmp=(HBITMAP)SelectObject(memDC, *hBmp);
	
	HDC memDC2 = CreateCompatibleDC(hdc);
	HBITMAP hTmp2=(HBITMAP)SelectObject(memDC2, *hBmp);

	// Get size of the bitmap
	BITMAP bmp;
	GetObject( *hBmp, sizeof(bmp), &bmp );
	
	BITMAP bmpM;
	GetObject( *hBmpMask, sizeof(bmpM), &bmpM );
	
	for (int yb=0; yb<sz.cy; yb++) {_XLOG_
		for (int xb=0; xb<sz.cx; xb++) {_XLOG_
			long lBMPX=long((double(xb)/double(sz.cx))*double(bmp.bmWidth));
			long lBMPY=long((double(yb)/double(sz.cy))*double(bmp.bmHeight));
			//BYTE bt1ABit=(BYTE)GetPixel(memDC2, lBMPX, lBMPY);
			COLORREF pto = GetPixel(hdc, x+xb, y+yb);
			COLORREF pfrom = GetPixel(memDC, lBMPX, lBMPY);
			BYTE alpha=limit2Byte(RGB2BW_Fx(pfrom));
			if((DWORD)pfrom==0){_XLOG_
				alpha=0;
			}
			BYTE bRed = ((255-alpha)*GetRValue(pto)+alpha*GetRValue(pfrom))/255;
			BYTE bGreen = ((255-alpha)*GetGValue(pto)+alpha*GetGValue(pfrom))/255;
			BYTE bBlue = ((255-alpha)*GetBValue(pto)+alpha*GetBValue(pfrom))/255;
			COLORREF pres = RGB(bRed, bGreen, bBlue);
			SetPixel(hdc, x+xb, y+yb, pres);
		}
	}
	SelectObject(memDC2, hTmp2);
	SelectObject(memDC, hTmp);
	DeleteDC(memDC2);
	DeleteDC(memDC);
}

BOOL bCCI=0;
HICON SmartLoadIcon(HINSTANCE h, const char* szImageName)
{_XLOG_
	CString sNameL=szImageName;
	sNameL.MakeLower();
	if(sNameL.Find(".icl")!=-1 || sNameL.Find(".dll")!=-1 || sNameL.Find(".exe")!=-1){_XLOG_
		int nIconIndex=sNameL.ReverseFind(',');
		sNameL=szImageName;
		if(nIconIndex!=-1){_XLOG_
			int iNumber=atol(sNameL.Mid(nIconIndex+1));
			sNameL=sNameL.Left(nIconIndex);
			nIconIndex=iNumber;
		}else{_XLOG_
			nIconIndex=0;
		}
		return ExtractIcon(h,sNameL,nIconIndex);
	}
	return (HICON)LoadImage(h,szImageName,IMAGE_ICON,0,0,LR_LOADFROMFILE|LR_VGACOLOR);
}

BOOL Draw32Icon(CDC* dc, HICON hIco, CPoint pt, CSize sz, DWORD dwOpacity, DWORD dwBlendColor)
{_XLOG_
	if(isWinXP()){_XLOG_
		/*
		if(bCCI==0){_XLOG_
			bCCI=1;
			InitCommonControls();
		}
		HIMAGELIST il=ImageList_Create(sz.cx,sz.cy,ILC_COLOR32|ILC_MASK,1,1);
		ImageList_AddIcon(il,hIco);
		ImageList_DrawEx(il, 0, dc->GetSafeHdc(), pt.x, pt.y, sz.cx, sz.cy, CLR_NONE, CLR_DEFAULT, ILD_NORMAL);
		il.Draw(&dc,0,pt.TopLeft(),ILD_TRANSPARENT);
		*/
		::DrawIconEx(dc->GetSafeHdc(), pt.x, pt.y, hIco, sz.cx, sz.cy, 0, NULL, DI_NORMAL);
	}else{_XLOG_
		ICONINFO iconinfo;
		GetIconInfo(hIco,&iconinfo);
		DrawBitmapAlpha(dc->GetSafeHdc(), &iconinfo.hbmColor, pt.x, pt.y, sz, &iconinfo.hbmMask);
		ClearBitmap(iconinfo.hbmColor);
		ClearBitmap(iconinfo.hbmMask);
	}
	if(dwBlendColor!=0xFFFFFFFF){_XLOG_
		int x=pt.x;
		int y=pt.y;
		HDC hdc=dc->GetSafeHdc();
		BYTE alpha=30;
		DWORD pfrom=dwBlendColor;
		for (int yb=0; yb<sz.cy; yb++) {_XLOG_
		for (int xb=0; xb<sz.cx; xb++) {_XLOG_
			COLORREF pto = GetPixel(hdc, x+xb, y+yb);
			BYTE bRed = ((255-alpha)*GetRValue(pto)+alpha*GetRValue(pfrom))/255;
			BYTE bGreen = ((255-alpha)*GetGValue(pto)+alpha*GetGValue(pfrom))/255;
			BYTE bBlue = ((255-alpha)*GetBValue(pto)+alpha*GetBValue(pfrom))/255;
			COLORREF pres = RGB(bRed, bGreen, bBlue);
			SetPixel(hdc, x+xb, y+yb, pres);
		}
		}
	}
	return TRUE;
}

long bDragWidgetMode=0;
DWORD WINAPI EmutalewWMovw(LPVOID p)
{_XLOG_
	HWND h=(HWND)p;
	SimpleTracker t(bDragWidgetMode);
	CRect rt;
	::GetWindowRect(h,&rt);
	CPoint pt;
	GetCursorPos(&pt);
	int ix0=rt.left-pt.x;
	int iy0=rt.top-pt.y;
	if(ix0>0){_XLOG_
		ix0=-10;
	}
	if(iy0>0){_XLOG_
		iy0=-10;
	}
	BOOL bRAlign=0;
	BOOL bBAlign=0;
	if(ix0<-rt.Width()){_XLOG_
		bRAlign=1;
		ix0=10;
	}
	if(iy0<-rt.Height()){_XLOG_
		bBAlign=1;
		iy0=10;
	}
	while(GetAsyncKeyState(VK_LBUTTON)<0){_XLOG_
		SetCursor(LoadCursor(NULL,IDC_SIZEALL));
		Sleep(40);
		GetCursorPos(&pt);
		int ix=ix0+pt.x+(bRAlign?(-rt.Width()):0);
		int iy=iy0+pt.y+(bBAlign?(-rt.Height()):0);
		::MoveWindow(h,ix,iy,rt.Width(),rt.Height(),1);
	}
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	return 0;
}

////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    DrawWireRects
//
// DESCRIPTION: Creates exploding wire rectanges
//
// INPUTS:  LPRECT lprcFrom      Source Rectangle
//          LPRECT lprcTo        Destination Rectangle
//          UINT nMilliSecSpeed  Speed in millisecs for animation
//
// RETURN:    None
// NOTES:    None
//
//  Maintenance Log
//  Author      Date    Version     Notes
//  NT Almond   011199  1.0         Origin
//  CJ Maunder  010899  1.1         Modified rectangle transition code
//
/////////////////////////////////////////////////////////////////////////
void WINAPI DrawWireRects(LPRECT lprcFrom, LPRECT lprcTo, UINT nMilliSecSpeed)
{_XLOG_
	const int nNumSteps = 10;

	GdiFlush();
	Sleep(10);  // Let the desktop window sort itself out

	// if hwnd is null - "you have the CON".
	HDC hDC = ::GetDC(NULL);

	// Pen size, urmmm not too thick
	HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(0,0,0));

	int nMode = ::SetROP2(hDC, R2_NOT);
	HPEN hOldPen = (HPEN) ::SelectObject(hDC, hPen);

	for (int i = 3; i < nNumSteps-1; i++)
	{_XLOG_
		double dFraction = (double) i / (double) nNumSteps;

		RECT transition;
		transition.left   = lprcFrom->left + 
			(int)((lprcTo->left - lprcFrom->left) * dFraction);
		transition.right  = lprcFrom->right + 
			(int)((lprcTo->right - lprcFrom->right) * dFraction);
		transition.top    = lprcFrom->top + 
			(int)((lprcTo->top - lprcFrom->top) * dFraction);
		transition.bottom = lprcFrom->bottom + 
			(int)((lprcTo->bottom - lprcFrom->bottom) * dFraction);

		POINT pt[5];
		pt[0] = CPoint(transition.left, transition.top);
		pt[1] = CPoint(transition.right,transition.top);
		pt[2] = CPoint(transition.right,transition.bottom);
		pt[3] = CPoint(transition.left, transition.bottom);
		pt[4] = CPoint(transition.left, transition.top);

		// We use Polyline because we can determine our own pen size
		// Draw Sides
		::Polyline(hDC,pt,5);

		GdiFlush();

		Sleep(nMilliSecSpeed);

		// UnDraw Sides
		::Polyline(hDC,pt,5);

		GdiFlush();
	}

	::SetROP2(hDC, nMode);
	::SelectObject(hDC, hOldPen);

	::ReleaseDC(NULL,hDC);
}

void DrawAnimateRect(CRect rtBegin,CRect rtEnd)
{_XLOG_
	//BOOL iRes=DrawAnimatedRects(0,IDANI_CAPTION,&rtBegin,&rtEnd);
	DrawWireRects(&rtBegin,&rtEnd,40);
}

void MakeHole(HWND hw,RECT rtH)
{_XLOG_
	CRect rt,rtR;
	GetWindowRect(hw,rt);
	HRGN hRgn=CreateRectRgn(0,0,0,0);
	int iRes=::GetWindowRgn(hw, hRgn);
	GetRgnBox(hRgn,&rtR);
	if(iRes==NULLREGION || rtR.Width()==0 || rtR.Height()==0){_XLOG_
		hRgn=CreateRectRgn(0,0,rt.Width(),rt.Height());
	}
	HRGN hRgn2=CreateRectRgn(rtH.left,rtH.right,rtH.right,rtH.bottom);
	::CombineRgn(hRgn,hRgn,hRgn2,RGN_DIFF);
	SetWindowRgn(hw,hRgn,TRUE);
	DeleteObject(hRgn2);
}

BYTE l2Bl(int iVal)
{_XLOG_
	if(iVal>255){_XLOG_
		iVal=255;
	}
	if(iVal<0){_XLOG_
		iVal=0;
	}
	return iVal;
}

DWORD Color2Color(DWORD dwBegin, DWORD dwEnd, double dwTonicLevel, BOOL bStsr)
{_XLOG_
	if(bStsr && dwBegin>dwEnd){_XLOG_
		return Color2Color(dwEnd, dwBegin, dwTonicLevel,0);
	}
	BYTE R=l2Bl(int(GetRValue(dwBegin)*dwTonicLevel+GetRValue(dwEnd)*(1-dwTonicLevel)));
	BYTE G=l2Bl(int(GetGValue(dwBegin)*dwTonicLevel+GetGValue(dwEnd)*(1-dwTonicLevel)));
	BYTE B=l2Bl(int(GetBValue(dwBegin)*dwTonicLevel+GetBValue(dwEnd)*(1-dwTonicLevel)));
	return RGB(R,G,B);
}

void DrawBmp(CDC* dc,CRect rt,CBitmap* bmpIn,DWORD* pdwBGColor,DWORD* pdwTColor, BOOL bUpResize, BOOL bVAlign)
{_XLOG_
	if(pdwBGColor){_XLOG_
		dc->FillSolidRect(rt.left,rt.top,rt.Width(),rt.Height(),*pdwBGColor);
	}
	CDC pDC;
	pDC.CreateCompatibleDC(dc);
	CSize sz=GetBitmapSize(bmpIn);
	if((bVAlign&DT_BOTTOM) && (rt.Width()>sz.cx)){// Если надо выравнивать вниз и есть куда
		bUpResize=0;
		int iW=(rt.left+rt.right);
		rt.left=(iW-sz.cx)/2;
		rt.right=(iW+sz.cx)/2;
		rt.top=rt.bottom-sz.cy;
	}else{_XLOG_
		bUpResize=FitImageToRect(rt, sz, bUpResize?0:1)?1:0;
	}
	CBitmap* bmp=bUpResize?_bmp().Clone(bmpIn,CSize(rt.Width(),rt.Height())):bmpIn;
	CBitmap* pOld=pDC.SelectObject(bmp);
	DWORD dwPT=RGB(255,255,255);
	if(pdwTColor){_XLOG_
		dwPT=*pdwTColor;
	}else{_XLOG_
		dwPT=pDC.GetPixel(CPoint(0,0));
	}
	dc->SetStretchBltMode(HALFTONE);//COLORONCOLOR
	TransparentBltMy(*dc,rt.left,rt.top,rt.Width(),rt.Height(),pDC,0,0,dwPT);
	pDC.SelectObject(pOld);
	if(bUpResize){_XLOG_
		delete bmp;
	}
}

CString _IL(int i, const char* szPref)
{_XLOG_
	if(szPref==NULL){_XLOG_
		szPref="IML_";
	}
	CString sRes=Format("%s%i",szPref,i);
	return sRes;
}

CSize GetBitmapSize(HBITMAP pBmp)
{_XLOG_
	if(pBmp==NULL){_XLOG_
		return CSize(0,0);
	}
	BITMAP pBitMap;
	::GetObject(pBmp, sizeof(BITMAP), &pBitMap);
	CSize size(pBitMap.bmWidth,pBitMap.bmHeight);
	return size;
}

CSize GetBitmapSize(CBitmap* pBmp)
{_XLOG_
	if(pBmp==NULL || pBmp->m_hObject==NULL){_XLOG_
		return CSize(0,0);
	}
	BITMAP pBitMap;
	pBmp->GetBitmap( &pBitMap);
	CSize size(pBitMap.bmWidth,pBitMap.bmHeight);
	return size;
}

void ClearBitmap(HBITMAP& hBmp)
{_XLOG_
	if(hBmp!=NULL){_XLOG_
		if(DeleteObject(hBmp)==0){_XLOG_
#ifdef _DEBUG
			TRACE("Error clearing bitmap!");
#endif
		}
		hBmp=NULL;
	}
}

void ClearIcon(HICON& hIcon)
{_XLOG_
	if(hIcon!=NULL){_XLOG_
		DestroyIcon(hIcon);
		hIcon=NULL;
	}
}

CSize GetIconSize(HICON hIcon)
{_XLOG_
	ICONINFO iconinfo;
	GetIconInfo(hIcon,&iconinfo);
	CBitmap bmpT;
	bmpT.Attach(iconinfo.hbmColor);
	CSize size=GetBitmapSize(bmpT);
	ClearBitmap(iconinfo.hbmColor);
	ClearBitmap(iconinfo.hbmMask);
	bmpT.Detach();
	return size;
}

CBitmap* BitmapFromIcon(HICON icon, CSize sizeIn, CBrush* br)
{_XLOG_
	if(icon==0){_XLOG_
		return 0;
	}
	static CBrush brWhite(RGB(255,255,255));
	if(br==NULL){_XLOG_
		br=&brWhite;
	}
	CSize size;
	if(sizeIn.cx==0 || sizeIn.cy==0){_XLOG_
		size=GetIconSize(icon);
	}else{_XLOG_
		size.cx=sizeIn.cx;
		size.cy=sizeIn.cy;
	}
	CBitmap* pBM=new CBitmap;
	CDesktopDC dcScreen;
	BOOL bRes=pBM->CreateCompatibleBitmap(&dcScreen,size.cx,size.cy);
	CDC pDc;
	pDc.CreateCompatibleDC(&dcScreen);
	CBitmap* pTmpBmp=pDc.SelectObject(pBM);
	DrawIconEx(pDc.GetSafeHdc(),0,0,icon,size.cx,size.cy,0,*br,DI_NORMAL);
	pDc.SelectObject(pTmpBmp);
	return pBM;
}
