// Dib.h: interface for the CDib class.
//
//	Creator : El Barto (ef00@luc.ac.be)
//	Location : http://www.luc.ac.be/~ef00/ebgfx
//	Date : 09-04-98
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__4AC8F494_CB24_11D1_91CA_0020AFF82585__INCLUDED_)
#define AFX_DIB_H__4AC8F494_CB24_11D1_91CA_0020AFF82585__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define CDIBPASTE_TRANS	 0x01
#define CDIBPASTE_RECOL	 0x02
#define CDIBPASTE_DSTALL 0x04
class CDib  
{
public:
	COLORREF *m_Bits;
	BITMAPINFO m_Info;
	HBITMAP m_Bitmap;
	CSize m_Size;

	BOOL Create ( int Width, int Height );
	BOOL Destroy ();

	void Render ( CDC *pDC, int iX, int iY, int Width, int Height);
	void Render2 ( CDC *pDC, int iX, int iY, int Width, int Height);
	void Render ( CDC *pDC, CBitmap *pBitmap , UINT iScanLine=0);
	void SetBitmap ( CDC *pDC, CBitmap *pBitmap , UINT iScanLine=0);
	void SetBitmap ( CDC *pDC, HBITMAP pBitmap , UINT iScanLine=0);
	
	void Fill ( int R, int G, int B );
	void FillGlass ( int R, int G, int B, int A );
	void FillStippledGlass ( int R, int G, int B );

	void Copy ( CDib *Dib );
	void Paste ( CDib *Dib );

	void Blend ( CDib *Dib, int A );
	void Darken ( CDib *Dib );
	void Difference ( CDib *Dib );
	double CalcAffinity ( CDib *Dib, long dwLim=10);
	double CalcDistance ( CDib *Dib);
	void Lighten ( CDib *Dib );
	void Multiply ( CDib *Dib );
	void Screen ( CDib *Dib );

	void CopyRect ( CDib *Dib, int x, int y );
	void PasteRect ( CDib *Dib, int x, int y, DWORD dwOpts, DWORD dwTransKey=0, DWORD dwReColor=0);

	void FillRect ( int x, int y, int w, int h, int R, int G, int B );
	void FillGlassRect ( int x, int y, int w, int h, int R, int G, int B, int A );
	void FillStippledGlassRect ( int x, int y, int w, int h, int R, int G, int B );
	void DrawGradientRect ( long bLTColor, long bRTColor, long bRBColor, long bLBColor, int iLayerOffset);

	void BlendRect ( CDib *Dib, int x, int y, int A );
	void DarkenRect ( CDib *Dib, int x, int y );
	void DifferenceRect ( CDib *Dib, int x, int y );
	void LightenRect ( CDib *Dib, int x, int y );
	void MultiplyRect ( CDib *Dib, int x, int y );
	void ScreenRect ( CDib *Dib, int x, int y );

	void Line ( int x1, int y1, int x2, int y2, int R, int G, int B );
	void LineGlass ( int x1, int y1, int x2, int y2, int R, int G, int B, int A );
	COLORREF& GetPixel(int x, int y);
	BOOL DarkenPixel(int x, int y, double dwPercentage);
	BOOL SetPixel(int x, int y, DWORD RGB, BOOL bMixMode=0);
	BOOL GetPixel(int x, int y, BYTE& R, BYTE& G, BYTE& B, BOOL bInvertEdges=FALSE, BOOL bmIXmODE=0);
	BOOL GetRectMiddleValue(long xPos,DWORD dwWidth,long yPos,DWORD dwHeight,BYTE& R, BYTE& G, BYTE& B);
	BOOL GetRectMedianValue(long xPos,DWORD dwWidth,long yPos,DWORD dwHeight,DWORD& dwColor,DWORD dwStep=1);
	BOOL GetPixelBrightness( int x, int y, int& iBr);
	CDib ();
	virtual ~CDib ();
};
BYTE l2B(int iVal);
BYTE l2B(double iVal);
BYTE RGB2BW(COLORREF rgb);
BYTE RGB2BW(BYTE r, BYTE g, BYTE b);
BYTE RGB2BW_F(COLORREF rgb);
BYTE RGB2BW_F(BYTE r, BYTE g, BYTE b);
double DISTANCE(int x, int y, int x1, int y1);
DWORD GetColorBlend(DWORD dwBegin, DWORD dwEnd, double dwTonicLevel, BOOL bStsr=1);
#endif // !defined(AFX_DIB_H__4AC8F494_CB24_11D1_91CA_0020AFF82585__INCLUDED_)
