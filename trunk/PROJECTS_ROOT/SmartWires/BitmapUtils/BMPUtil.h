// SupportClasses.h: interface for the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPUTIL_H__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_)
#define AFX_BMPUTIL_H__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxmt.h>
#include <afxtempl.h>
#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH   2540
#endif
#ifndef MAP_PIX_TO_LOGHIM
#define MAP_PIX_TO_LOGHIM(x,ppli)   ( (HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli) )
#endif
#ifndef MAP_LOGHIM_TO_PIX
#define MAP_LOGHIM_TO_PIX(x,ppli)   ( ((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH )
#endif
#include "DelayLoadFuncs.h"
class CIconPreview : public CButton
{
	DECLARE_DYNAMIC(CIconPreview)
	DWORD* m_IconNum;
public:
	CIconPreview(DWORD* dwIconNum);
	virtual ~CIconPreview();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};

#define CBS_NORMAL	0x00000001
#define CBS_FOREG	0x00000002
#define CBS_BACKG	0x00000004
#define CBS_FLAT	0x00000008
#define CBS_NOBG	0x00000010
#define CBS_HIQUAL	0x00000020
#define CBS_ZOOMX	0x00000040
#define CBS_UNHOOV	0x00000080
#define CBS_DDOWN	0x00000100
#define CBS_NOSTR	0x00000200
#define CBS_NOTEF	0x00000400
#define CBS_NONSQR	0x00000800
#define CBS_DDOWN2	0x00001000
#define CBS_BORDER	0x00002000
#define CBS_LTTRANS	0x00004000
#define CBS_CENTLES	0x00008000

#define CBS_NORML_N	0
#define CBS_FOREG_N	1
#define CBS_BACKG_N	2
#define CBS_BCACHE	3
class CBitmapStatic : public CButton
{
	long dwXMove;
	long dwYMove;
	long dwWOffset;
	long dwHOffset;
	DECLARE_DYNAMIC(CBitmapStatic)
	HBRUSH IconBtBr;
	HICON m_hIcon[4];
	DWORD m_dwStyles;
	CString m_sText;
	DWORD dwTxCol;
	DWORD dwBgCol;
	BOOL bHoverMode;
public:
	void SetHooverMode(BOOL bNewMode);
	void Init();
	BOOL SetText(const char* szText, DWORD dwTx=0, DWORD dwBg=0);
	void SetSysBrush(int iIndex);
	void SetBrush(HBITMAP br);
	void SetBrush(COLORREF rb);
	BOOL SetStyles(DWORD dwStyles);
	BOOL DeleteBitmap(int iType=0);
	BOOL SetBitmap(CBitmap* bmp, int iType=0, int iScale=100, int iDxMove=0, int iDyMove=0, CSize* pOutSize=NULL);
	BOOL SetBitmap(DWORD dwCode, int iType=0, int iScale=100, int iDxMove=0, int iDyMove=0);
	BOOL SetBitmap(HICON hIcon, int iType=0);
	CBitmapStatic();
	CBitmapStatic(CBitmap* bmp);
	CBitmapStatic(DWORD dwCode);
	virtual ~CBitmapStatic();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnMouseMove(UINT nFlags, CPoint pt);
	afx_msg BOOL OnEraseBkgnd( CDC * );
protected:
	DECLARE_MESSAGE_MAP()
};
void PngTransp2White(CBitmap* pOrigBitmap);
HBITMAP ScaleBitmapInt(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight,DWORD dwEffects=0);
HICON CreateIconFromBitmap(CBitmap* pBitmap, CSize* pSize=NULL, BOOL bQualitySamplyng=TRUE, DWORD dwEmpColor=RGB(255,255,255));
CBitmap* AddBackgroundToBMP(CBitmap* pBMPFrom,DWORD dwBGIcon, CSize size);
BOOL CopyBitmapToBitmap(HBITMAP hFrom, HBITMAP& hTo, CSize size, CSize* sizeTo=NULL, BOOL bQualitySamplyng=TRUE);
void GetExeFileIcon(const char* sProcExePath, HICON& hIcon, BOOL bRawGet=FALSE, BOOL bUseSH=FALSE);
BOOL SaveBitmapToDisk(HBITMAP& hbmOutImage, const char* szFile, HWND hParent=NULL);
void SaveScreenshotToFile(const char* szPath, int iType);
BOOL CreateBMPFile(HWND hwnd, const char* pszFile, PBITMAPINFO pbi, HBITMAP hBMP);
LPBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP& hBmp);
void DitherBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HBITMAP hbm, int nXSrc, int nYSrc);
void DrawGrayScale( CDC *pDC, HANDLE hDIB);

class CImgLoader
{
public:
	virtual CBitmap* LoadBmpFromPath(CString szPath)=0;
	virtual BOOL ReleaseBmp(CBitmap*&)=0;
	virtual CBitmap* Get(const char*, CSize=CSize(0,0))=0;
};

class CBmpHolder:public CImgLoader
{
public:
	BOOL ReleaseBmp(CBitmap*&);
	CString sInstance;
	CMapStringToPtr bmpHolder;
	CBmpHolder();
	~CBmpHolder();
	CSize MenuIconSize;
	CString GetResName(DWORD dwID, const char* szNamePrefx=NULL);
	BOOL IsPresent(const char* szBmpName);
	//
	CBitmap* AddBmpRaw(DWORD dwID, CSize size=CSize(0,0), const char* szPrefix=NULL);
	CBitmap* AddBmpRaw(const char* szName, CBitmap* bmp, CSize size=CSize(0,0));
	CBitmap* AddBmpRaw(const char* szName, HICON icon, CSize size=CSize(0,0), CBrush* br=NULL);
	CBitmap* AddBmp(DWORD dwID, CSize size=CSize(0,0), const char* szPrefix=NULL);
	CBitmap* AddBmp(const char* szName, CBitmap* bmp, CSize size=CSize(0,0));
	CBitmap* AddBmp(const char* szName, HICON icon, CSize size=CSize(0,0), CBrush* br=NULL);
	CBitmap* AddBmpFromDisk(const char* szFile, CSize size=CSize(0,0), const char* szName=0);
	CBitmap* AddBmpFromDiskIfNotPresent(const char* szFile, const char* szName, CSize size=CSize(0,0));
	BOOL DelBmp(DWORD dwID, const char* szPrefix=NULL);
	BOOL DelBmp(const char* szName);
	//
	CBitmap* GetIcon(DWORD dwIcon);
	CBitmap* GetIcon(const char* szIconName);
	CBitmap* GetRaw(DWORD dwID, CSize size=CSize(0,0), const char* szNamePrefx=NULL);// Взятие с загрузкой из ресурсов
	CBitmap* Get(DWORD dwID, CSize size=CSize(0,0), const char* szNamePrefx=NULL);// Взятие с загрузкой из ресурсов
	CBitmap* GetRaw(const char* szName, CSize size=CSize(0,0));// Взятие с загрузкой с диска
	CBitmap* Get(const char* szName, CSize size=CSize(0,0));// Взятие с загрузкой с диска
	CBitmap* Clone(CBitmap* bmp, CSize sizeTo=CSize(0,0));
	CBitmap* Clone(DWORD dwID, CSize size);
	CBitmap* Clone(const char* szName, CSize size);
	CBitmap* LoadBmpFromPath(CString szPath);
	CBitmap* LoadBmpFromPathExt(const char* szPath);
	CBitmap* LoadBmpCached(const char* szPath);
	HICON CloneAsIcon(DWORD dwID);
	HICON CloneAsIcon(const char* szName);
};

#include "CPreviewFileDialog.h"
class CPreviewImage: public CAbstractPreview
{
	CBrush bg;
	CSize cBmpSize;
	CString sLastPreviewFile;
	CString sInfo;
public:
	CPreviewImage();
	~CPreviewImage();
	virtual void SetPreviewFile(const CString& csFileName);
	virtual void DrawPreview(CDC *pDC,int x,int y,int width,int height);
	virtual const char* GetImageInfo();
};

#define SPLASH_VTOP		0x0001
#define SPLASH_VMIDDLE	0x0002
#define SPLASH_VBOTTOM	0x0004
#define SPLASH_OPACITY	0x0008
#define SPLASH_HLEFT	0x0010
#define SPLASH_HCENTER	0x0020
#define SPLASH_HRIGHT	0x0040
#define SPLASH_HIDDEN	0x0080
class CSplashParams
{
public:
	BOOL bHidden;
	BOOL bFade;
	CFont* pFont;
	CRect bounds;
	CBitmap* bmp;
	CString szText;
	CString sTextPrefix;
	CString sTextPosfix;
	COLORREF txColor;
	COLORREF bgColor;
	DWORD dwTextParams;
	DWORD dwSplashAlign;
	CSplashParams();
	CSplashParams& operator=(CSplashParams& or)
	{
		bHidden=or.bHidden;
		bounds=or.bounds;
		bmp=or.bmp;
		szText=or.szText;
		txColor=or.txColor;
		bgColor=or.bgColor;
		dwTextParams=or.dwTextParams;
		dwSplashAlign=or.dwSplashAlign;
		pFont=or.pFont;
		bFade=or.bFade;
		return *this;
	}
};

class CSplashWindow
{
	BOOL bNeedUpadteIntBmp;
	HANDLE hWndProc;
	long lDYCounter;
public:
	BOOL bClosing;
	DWORD dwThread;
	CCriticalSection cs;
	CBitmap* bmpOffscreen;
	BOOL bBeforeExit;
	CFont* pFont;
	HANDLE Event;
	BOOL bVisible;
	CRect rDesktopRECT;
	CWnd* wnd;
	HWND wndH;
	CSplashParams par;
	void AutoClose(DWORD dwTimeX,CSplashWindow** pSetToNULL=NULL);
	static long lSplashWindowsCount;
	CSplashWindow(CSplashParams* param);
	void SetBitmap(CBitmap* bmp);
	void SetText(const char* szText, COLORREF txColor=RGB(250,250,250), COLORREF bgColor=RGB(0,0,0), DWORD dwTextParams=0);
	void SetTextOnly(const char* szText);
	void SetFont(CFont* pFontFrom);
	BOOL UpdateSize();
	BOOL CreateWnd();
	void OnPaint();
	~CSplashWindow();
	DWORD dwAutoCloserNumber;
};

#define CSafeDC	CDC
/*class CSafeDC: public CDC
{
public:
	CSafeDC();
	virtual ~CSafeDC();
	BOOL CreateCompatibleDC(CDC*,const char*);
	CBitmap* SelectObject(CBitmap*);
	CFont* SelectObject(CFont*);
};*/

#define ULW_COLORKEY            0x00000001
#define ULW_ALPHA               0x00000002
#define ULW_OPAQUE              0x00000004
#define WS_EX_LAYERED           0x00080000

CBmpHolder& _bmp();
#define TXSHADOW_SIMPLE	0x0001//Простая тень
#define TXSHADOW_GLOW	0x0002//Тень во все стороны
#define TXSHADOW_MIXBG	0x0004//Цвет тени - среднее между фоном и текстом
#define TXSHADOW_FILLBG	0x0008//Заливать фон
#define TXSHADOW_VCENT	0x0010//Центровать по верт.
#define TXSHADOW_HCENT	0x0020//Центровать по гориз.
#define TXSHADOW_SHCOL	0x0040//Взять указанный цвет тени
#define TXSHADOW_WIDE	0x0080//Удлиненная тень
#define TXSHADOW_CALCRC	0x0100//Просто вычислить прямоугольник для текста
#define TXSHADOW_RIGHT	0x0200//Выравнивание направо
#define TXTEXT_VCENT	0x0400//Принудительно текст по вертикали в центр
#define TXTEXT_VBOTTOM	0x0800//Принудительно текст по вертикали в центр
#define TXTEXT_CALCDR	0x1000//Нарисовать и Просто вычислить прямоугольник для текста
#define TXTEXT_UNICODE	0x2000//Текст - в Юникоде
#define TXSHADOW_USERGN	0x4000//Текст - в Юникоде
#define TINI_FONTEXACT	0x0001
HICON PutTextInIcon(HICON hIcon,const char* szText, CFont* pFont=NULL, COLORREF col=RGB(254,254,254), COLORREF dwBgCol=RGB(0,0,0), DWORD dwFlags=0);
BOOL SmartDrawText(HDC pDC, const char* szText, CRect& bounds, CFont* pFont, COLORREF txColor, COLORREF bgColor, DWORD dwShadowMode, DWORD dwTextParams=0, COLORREF shColor=0);
BOOL SmartDrawTextX(HDC pDC, const char* szText, CRect& bounds, CFont* pFont, COLORREF txColor, COLORREF bgColor, DWORD dwShadowMode, DWORD dwTextParams=0, COLORREF shColor=0, long lTextOp=100);
HICON PutOverlayInIcon(HICON hIcon,HICON hIcon2, int iX=-1, int iY=-1, BOOL bFromDown=FALSE, BOOL bQualitySampling=FALSE);
HICON PutOverlayInIconSz(HICON hIcon,HICON hIcon2, int iX0, int iY0, int iX=-1, int iY=-1, BOOL bFromDown=FALSE, BOOL bQualitySampling=FALSE);
BOOL SetDesktopIconsBg(BOOL bTransparent, COLORREF& cBgCol, COLORREF cTxCol=RGB(255,255,255));
HWND GetSysListView();
BOOL FitImageToRect(CRect& rt, CSize cBmpSize,BOOL bAllowCenter=0);
void CopyBitmapToClipboard( const CWnd *pWnd, CBitmap& bitmap, CPalette* pPal );
bool GetImageSizeFromFile(const char* FileName, int& Width, int& Height);
BOOL WINAPI SaveBitmapToJpg(const char* sPictureFile,HBITMAP bmp, BYTE bQuality);
HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor = 0, COLORREF cTolerance = 0x101010);
BOOL WINAPI AlphaBlendMy(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
BOOL WINAPI AlphaBlendMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,BLENDFUNCTION bl);
BOOL WINAPI AlphaBlendMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,int bl);
BOOL WINAPI TransparentBltMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,int cx2,int cy2,UINT iTransCol);
BOOL WINAPI TransparentBltMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,UINT iTransCol);
BOOL WINAPI UpdateLayeredWindowMy(HWND hwnd,HDC hdcDst,POINT *pptDst,SIZE *psize,HDC hdcSrc,POINT *pptSrc,COLORREF crKey,BLENDFUNCTION *pblend,DWORD dwFlags);
BOOL MakeImageCobble(HBITMAP*& hbmOutImage,CSize scrSize,int iTile=1);
BOOL MakeImageCobble(CBitmap& bmOutImage,CSize scrSize,int iTile=1);
BOOL GetScreenShotInBuffer(HWND hWnd, CBitmap*& bmScreenshot, CSize sz);
BOOL StartScreenshotRect(CRect& Rectangle, CRect& Screen);
BOOL ContScreenshotRect(CRect& Rectangle, const char* szTxt);
BOOL DrawScreenshotRect(CRect&, const char* szTxt);
BOOL StopScreenshotRect();
BYTE limit2Byte(int iVal);
void AdjustToSceenCoords(CRect& rt,BOOL bDirect=TRUE);
HICON SmartLoadIcon(HINSTANCE h, const char* szImageName);
BOOL Draw32Icon(CDC* cd, HICON hIco, CPoint pt, CSize sz, DWORD dwOpacity=255, DWORD dwBlendColor=0xFFFFFFFF);
#ifdef CXIMG_SUPPORT

#define CXIMAGE_SUPPORT_BMP 1
#define CXIMAGE_SUPPORT_GIF 1
#define CXIMAGE_SUPPORT_JPG 1
#define CXIMAGE_SUPPORT_PNG 1
#define CXIMAGE_SUPPORT_MNG 0
#define CXIMAGE_SUPPORT_ICO 1
#define CXIMAGE_SUPPORT_TIF 1
#define CXIMAGE_SUPPORT_TGA 1
#define CXIMAGE_SUPPORT_PCX 1
#define CXIMAGE_SUPPORT_WBMP 0
#define CXIMAGE_SUPPORT_WMF 0
#define CXIMAGE_SUPPORT_J2K 0		// Beta, use JP2
#define CXIMAGE_SUPPORT_JBG 0		// GPL'd see ../jbig/copying.txt & ../jbig/patents.htm

#define CXIMAGE_SUPPORT_JP2 0
#define CXIMAGE_SUPPORT_JPC 0
#define CXIMAGE_SUPPORT_PGX 0
#define CXIMAGE_SUPPORT_PNM 0
#define CXIMAGE_SUPPORT_RAS 0
/////////////////////////////////////////////////////////////////////////////
// CxImage formats enumerator
enum ENUM_CXIMAGE_FORMATS{
CXIMAGE_FORMAT_UNKNOWN,
#if CXIMAGE_SUPPORT_BMP
CXIMAGE_FORMAT_BMP,
#endif
#if CXIMAGE_SUPPORT_GIF
CXIMAGE_FORMAT_GIF,
#endif
#if CXIMAGE_SUPPORT_JPG
CXIMAGE_FORMAT_JPG,
#endif
#if CXIMAGE_SUPPORT_PNG
CXIMAGE_FORMAT_PNG,
#endif
#if CXIMAGE_SUPPORT_MNG
CXIMAGE_FORMAT_MNG,
#endif
#if CXIMAGE_SUPPORT_ICO
CXIMAGE_FORMAT_ICO,
#endif
#if CXIMAGE_SUPPORT_TIF
CXIMAGE_FORMAT_TIF,
#endif
#if CXIMAGE_SUPPORT_TGA
CXIMAGE_FORMAT_TGA,
#endif
#if CXIMAGE_SUPPORT_PCX
CXIMAGE_FORMAT_PCX,
#endif
#if CXIMAGE_SUPPORT_WBMP
CXIMAGE_FORMAT_WBMP,
#endif
#if CXIMAGE_SUPPORT_WMF
CXIMAGE_FORMAT_WMF,
#endif
#if CXIMAGE_SUPPORT_J2K
CXIMAGE_FORMAT_J2K,
#endif
#if CXIMAGE_SUPPORT_JBG
CXIMAGE_FORMAT_JBG,
#endif
#if CXIMAGE_SUPPORT_JP2
CXIMAGE_FORMAT_JP2,
#endif
#if CXIMAGE_SUPPORT_JPC
CXIMAGE_FORMAT_JPC,
#endif
#if CXIMAGE_SUPPORT_PGX
CXIMAGE_FORMAT_PGX,
#endif
#if CXIMAGE_SUPPORT_PNM
CXIMAGE_FORMAT_PNM,
#endif
#if CXIMAGE_SUPPORT_RAS
CXIMAGE_FORMAT_RAS,
#endif
CMAX_IMAGE_FORMATS
};


typedef HBITMAP (WINAPI *_LoadBmp)(const char* szPath, DWORD dwType);
typedef BOOL (WINAPI *_ConvertToJpg)(const char* szFullPath,const char* szPath, DWORD dwQ);
class CXIMG
{
public:
	_LoadBmp bml;
	_ConvertToJpg cjp;
	HINSTANCE hDLL;
	CXIMG();
	~CXIMG();
	BOOL ConvertToJpg(const char* szFrom, const char* szTo, DWORD dwQ);
	HBITMAP Load(const char* szPath);
};

CXIMG& getILoad();
#endif


class CSmartCoInitialize
{
public:
	CSmartCoInitialize()
	{
		CoInitialize(0);
	}
	~CSmartCoInitialize()
	{
		CoUninitialize();
	}
};

class CSmartOleInitialize
{
public:
	CSmartOleInitialize()
	{
		OleInitialize(0);
	}
	~CSmartOleInitialize()
	{
		OleUninitialize();
	}
};
void Warn(const char* szT, const char* szTitle="Warning");
DWORD WINAPI EmutalewWMovw(LPVOID p);

class CDMark
{
public:
	int iX;
	int iY;
	int iX0;
	int iY0;
	CDC dcScreen;
	CPen pn;
	CDMark(int x,int y,int x0,int y0)
	{
		iX=x;
		iY=y;
		iX0=x0;
		iY0=y0;
		dcScreen.CreateDC("DISPLAY", NULL, NULL, NULL); 
		pn.CreatePen(0,4,RGB(0,0,0));
		dcScreen.SelectObject(&pn);
		dcScreen.SetROP2(R2_NOT);
		//dcScreen.FillSolidRect(iX-5,iY-5,10,10,0);
		dcScreen.MoveTo(iX0,iY0);
		dcScreen.LineTo(iX,iY);
	}
	~CDMark()
	{
		dcScreen.SetROP2(R2_NOT);
		//dcScreen.FillSolidRect(iX-5,iY-5,10,10,0);
		dcScreen.MoveTo(iX0,iY0);
		dcScreen.LineTo(iX,iY);
	}
};

CString _IL(int i, const char* szPref=NULL);
void MakeHole(HWND hw,RECT rtH);
void DrawAnimateRect(CRect rtBegin,CRect rtEnd);
void WINAPI DrawWireRects(LPRECT lprcFrom, LPRECT lprcTo, UINT nMilliSecSpeed);
DWORD Color2Color(DWORD dwBegin, DWORD dwEnd, double dwTonicLevel, BOOL bStsr);
void DrawBmp(CDC* dc,CRect rt,CBitmap* bmp,DWORD* dwBGColor=0,DWORD* pdwTColor=0, BOOL bUpResize=1, BOOL bVAlign=0);
CSize GetBitmapSize(CBitmap* pBmp);
CSize GetBitmapSize(HBITMAP pBmp);
void ClearBitmap(HBITMAP& hBmp);
void ClearIcon(HICON& hIcon);
CBitmap* BitmapFromIcon(HICON icon, CSize sizeIn=CSize(0,0), CBrush* br=0);
CSize GetIconSize(HICON hIcon);
#endif // !defined(AFX_SUPPORTCLASSES_H__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_)
