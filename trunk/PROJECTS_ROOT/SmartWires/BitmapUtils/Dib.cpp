// Dib.cpp: implementation of the CDib class.
//
//	Creator : El Barto (ef00@luc.ac.be)
//	Location : http://www.luc.ac.be/~ef00/ebgfx
//	Date : 09-04-98
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BYTE I2B(int iVal)
{
	if(iVal>255){
		iVal=255;
	}
	if(iVal<0){
		iVal=0;
	}
	return iVal;
}

CDib::CDib ()
{
	m_Bits=NULL;
	m_Bitmap=NULL;
	m_Size=CSize ( 0, 0 );
}

CDib::~CDib ()
{
	Destroy ();
}

BOOL CDib::Create ( int Width, int Height )
{
	Destroy ();

	ZeroMemory ( &m_Info, sizeof (BITMAPINFO) );

	m_Info.bmiHeader.biSize=sizeof (BITMAPINFOHEADER);
	m_Info.bmiHeader.biWidth=Width;
	m_Info.bmiHeader.biHeight=Height;
	m_Info.bmiHeader.biPlanes=1;
	m_Info.bmiHeader.biBitCount=32; 
	m_Info.bmiHeader.biCompression=BI_RGB;
	m_Info.bmiHeader.biSizeImage=Width*Height*4;
	
	m_Bitmap=CreateDIBSection ( NULL, &m_Info, DIB_RGB_COLORS, (void **)&m_Bits, NULL, NULL ); 

	if ( m_Bitmap )
	{
		m_Size=CSize ( Width, Height );
		return TRUE;
	}
	else
	{
		m_Size=CSize ( 0, 0 );
		return FALSE;
	}
}

BOOL CDib::Destroy ()
{
	if ( m_Bitmap )
		DeleteObject ( m_Bitmap );
	m_Bitmap=NULL;
	m_Size=CSize ( 0, 0 );
	
	return TRUE;
}

void CDib::Render ( CDC *pDC, int iX, int iY, int Width, int Height )
{
	SetDIBitsToDevice ( pDC->m_hDC, iX, iY, Width, Height, 0, 0, 0, Height, m_Bits, &m_Info, 0); 
}

void CDib::Render2 ( CDC *pDC, int iX, int iY, int Width, int Height )
{
	SetDIBitsToDevice ( pDC->m_hDC, iX, iY, Width, Height, iX, 0, 0, Height, m_Bits, &m_Info, 0); 
}

void CDib::Render ( CDC *pDC, CBitmap *pBitmap , UINT iScanLine )
{
	SetDIBits ( pDC->m_hDC, HBITMAP(*pBitmap), iScanLine, m_Size.cy, m_Bits, &(m_Info), DIB_RGB_COLORS); 
}

void CDib::SetBitmap ( CDC *pDC, HBITMAP pBitmap , UINT iScanLine)
{
	if ( m_Bitmap )
		GetDIBits ( pDC->m_hDC, pBitmap, iScanLine, m_Size.cy, m_Bits, &(m_Info), DIB_RGB_COLORS ); 
}

void CDib::SetBitmap ( CDC *pDC, CBitmap *pBitmap , UINT iScanLine)
{
	if ( m_Bitmap )
		GetDIBits ( pDC->m_hDC, HBITMAP(*pBitmap), iScanLine, m_Size.cy, m_Bits, &(m_Info), DIB_RGB_COLORS ); 
}

void CDib::Fill ( int R, int G, int B )
{
	COLORREF Color=RGB ( B, G, R );
	int Size=m_Size.cx*m_Size.cy;

	for ( int i=0; i<Size; i++ )
		m_Bits[i]=Color;
}

void CDib::FillGlass ( int R, int G, int B, int A )
{
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
		
	while ( Size-- )
	{
		dst[0]=(BYTE)(((B-dst[0])*A+(dst[0]<<8))>>8);
		dst[1]=(BYTE)(((G-dst[1])*A+(dst[1]<<8))>>8);
		dst[2]=(BYTE)(((R-dst[2])*A+(dst[2]<<8))>>8);	
		dst+=4;
	}
}

void CDib::FillStippledGlass ( int R, int G, int B )
{	
	COLORREF Color=RGB ( B, G, R );
	int w=m_Size.cx;
	int h=m_Size.cy;

	for ( int j=0; j<w; j++ )
	{
		for ( int i=0; i<h; i++ )
		{
			m_Bits[j*w+i]=((i+j)&0x1) ? m_Bits[j*w+i] : Color;
		}
	}
}

void CDib::Copy ( CDib *Dib )
{
	// If DibSize Wrong Re-Create Dib
	if ( (Dib->m_Size.cx!=m_Size.cx) || (Dib->m_Size.cy!=m_Size.cy) )
		Dib->Create ( m_Size.cx, m_Size.cy );
	// do Copy
	memcpy ( Dib->m_Bits, m_Bits, m_Size.cx*m_Size.cy*4 );
}

void CDib::Paste ( CDib *Dib )
{
	// If DibSize Wrong Re-Create Dib
	if ( (m_Size.cx!=Dib->m_Size.cx) || (m_Size.cy!=Dib->m_Size.cy) )
		Create ( Dib->m_Size.cx, Dib->m_Size.cx );
	// do Paste
	memcpy ( m_Bits, Dib->m_Bits, m_Size.cx*m_Size.cy*4 );
}

void CDib::Blend ( CDib *Dib, int A )
{
	if ( m_Size!=Dib->m_Size )
		return;

	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
		
	while ( Size-- )
	{
		dst[0]=(BYTE)(((src[0]-dst[0])*A+(dst[0]<<8))>>8);
		dst[1]=(BYTE)(((src[1]-dst[1])*A+(dst[1]<<8))>>8);
		dst[2]=(BYTE)(((src[2]-dst[2])*A+(dst[2]<<8))>>8);	
		dst+=4;
		src+=4;
	}
}

void CDib::Darken ( CDib *Dib )
{
	if ( m_Size!=Dib->m_Size )
		return;

	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
		
	while ( Size-- )
	{
		dst[0]=(BYTE)((src[0]<dst[0]) ? src[0] : dst[0]);
		dst[1]=(BYTE)((src[1]<dst[1]) ? src[1] : dst[1]);
		dst[2]=(BYTE)((src[2]<dst[2]) ? src[2] : dst[2]);	
		dst+=4;
		src+=4;
	}
}

double CDib::CalcAffinity ( CDib *Dib, long dwLim)
{
	if ( m_Size!=Dib->m_Size )
		return 0;
	int iTotal=0;
	int iAffTotal=0;
	int iOffTotal=0;
	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
	BYTE bO0=0,bO1=0,bO2=0;
	while ( Size-- )
	{
#ifdef _DEBUG
		DWORD dw1=RGB(src[0],src[1],src[2]);
		DWORD dw2=RGB(dst[0],dst[1],dst[2]);
#endif
		if(abs(src[0]-dst[0])<dwLim 
			&& abs(src[1]-dst[1])<dwLim
			&& abs(src[2]-dst[2])<dwLim){
			iAffTotal++;
		}
		if(iTotal==0){
			iOffTotal=1;
			bO0=(src[0]-dst[0]);
			bO1=(src[1]-dst[1]);
			bO2=(src[2]-dst[2]);
		}else{
			if(abs(src[0]-dst[0]-bO0)<dwLim
				&& abs(src[0]-dst[0]-bO1)<dwLim
				&& abs(src[0]-dst[0]-bO2)<dwLim){
				iOffTotal++;
			}
		}
		iTotal++;
		dst+=4;
		src+=4;
	}
	double dAffLimit=double(iAffTotal)/double(iTotal);
	double dOffsetLimit=double(iOffTotal)/double(iTotal);
	return dAffLimit>dOffsetLimit?dOffsetLimit:dAffLimit;
}

double CDib::CalcDistance ( CDib *Dib)
{
	if ( m_Size!=Dib->m_Size )
		return 0;
	int iTotal=0;
	int iAffTotal=0;
	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
	while ( Size-- )
	{
		iAffTotal+=(src[0]-dst[0])*(src[0]-dst[0]);
		iAffTotal+=(src[1]-dst[2])*(src[1]-dst[2]);
		iAffTotal+=(src[2]-dst[2])*(src[2]-dst[2]);
		iTotal++;
		dst+=4;
		src+=4;
	}
	return iAffTotal/iTotal;
}

void CDib::Difference ( CDib *Dib )
{
	if ( m_Size!=Dib->m_Size )
		return;

	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
		
	while ( Size-- )
	{
		int Difference;
		Difference=src[0]-dst[0];
		dst[0]=(BYTE)((Difference<0) ? -Difference : Difference);
		Difference=src[1]-dst[1];
		dst[1]=(BYTE)((Difference<0) ? -Difference : Difference);
		Difference=src[2]-dst[2];
		dst[2]=(BYTE)((Difference<0) ? -Difference : Difference);	
		dst+=4;
		src+=4;
	}
}

void CDib::Lighten ( CDib *Dib )
{
	if ( m_Size!=Dib->m_Size )
		return;

	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
		
	while ( Size-- )
	{
		dst[0]=(BYTE)((src[0]>dst[0]) ? src[0] : dst[0]);
		dst[1]=(BYTE)((src[1]>dst[1]) ? src[1] : dst[1]);
		dst[2]=(BYTE)((src[2]>dst[2]) ? src[2] : dst[2]);	
		dst+=4;
		src+=4;
	}
}

void CDib::Multiply ( CDib *Dib )
{
	if ( m_Size!=Dib->m_Size )
		return;

	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
		
	while ( Size-- )
	{
		dst[0]=(BYTE)(((src[0])*(dst[0]))>>8);
		dst[1]=(BYTE)(((src[1])*(dst[1]))>>8);
		dst[2]=(BYTE)(((src[2])*(dst[2]))>>8);	
		dst+=4;
		src+=4;
	}
}

void CDib::Screen ( CDib *Dib )
{
	if ( m_Size!=Dib->m_Size )
		return;

	BYTE *src=(BYTE*)Dib->m_Bits;
	BYTE *dst=(BYTE*)m_Bits;
	int Size=m_Size.cx*m_Size.cy;
		
	while ( Size-- )
	{
		dst[0]=(BYTE)(255-(((255-src[0])*(255-dst[0]))>>8));
		dst[1]=(BYTE)(255-(((255-src[1])*(255-dst[1]))>>8));
		dst[2]=(BYTE)(255-(((255-src[2])*(255-dst[2]))>>8));	
		dst+=4;
		src+=4;
	}
}

//////////////////////////////////////////////////////////////////////
// Rectangle Functions
//////////////////////////////////////////////////////////////////////

void CDib::CopyRect ( CDib *Dib, int x, int y )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Copy return
	if ( (dx<=0) || (dy<=0) )
		return;
	// If DibSize Wrong Re-Create Dib
	if ( (dx!=Dib->m_Size.cx) || (dy!=Dib->m_Size.cy) )
		Dib->Create ( dx, dy );

	// Prepare Buffer Addresses
	COLORREF *src=m_Bits+(py*m_Size.cx)+px;
	COLORREF *dst=Dib->m_Bits;

	// Do Copy
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
			dst[i]=src[i];
		src+=m_Size.cx;
		dst+=Dib->m_Size.cx;
	}
}

void CDib::PasteRect ( CDib *Dib, int x, int y, DWORD dwOpts, DWORD dwTransKey, DWORD dwReColor)
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;
	
	// If Nothing to Paste return
	if ( (dx<=0) || (dy<=0) )
		return;
	
	// Prepare Buffer Addresses
	COLORREF *src=0;//Dib->m_Bits+((py-y)*Dib->m_Size.cx)+px-x;
	COLORREF *dst=0;//m_Bits+(py*m_Size.cx)+px;
	if(dwOpts&CDIBPASTE_DSTALL){
		src=Dib->m_Bits;
		dst=m_Bits+px;
	}else{
		dst=m_Bits+(py*m_Size.cx)+px;
		src=Dib->m_Bits+((py-y)*Dib->m_Size.cx)+px-x;
	}
	
	// Do Paste
	if(dwOpts&CDIBPASTE_TRANS){
		while ( dy-- )
		{
			for ( int i=0; i<dx; i++ ){
				if(src[i]!=dwTransKey){
					dst[i]=(dwOpts&CDIBPASTE_RECOL)?dwReColor:src[i];
				}
			}
			src+=Dib->m_Size.cx;
			dst+=m_Size.cx;
		}
		
	}else{
		while ( dy-- )
		{
			for ( int i=0; i<dx; i++ ){
				dst[i]=src[i];
			}
			src+=Dib->m_Size.cx;
			dst+=m_Size.cx;
		}
	}
}

#include <math.h>
void CDib::DrawGradientRect ( long bLTColor, long bRTColor, long bRBColor, long bLBColor, int iLayerOffset)
{
	int iSize=m_Size.cx*m_Size.cy;
	int iSizeOrig=iSize;
	BYTE *dst=(BYTE*) m_Bits;
	double iDiagLen=m_Size.cy>m_Size.cx?m_Size.cy:m_Size.cx;
	//double iDiagLen=sqrt(m_Size.cx*m_Size.cx+m_Size.cy*m_Size.cy);
	while (iSize--){
		long xPos=(iSizeOrig-iSize)%m_Size.cx-1;
		long yPos=(iSizeOrig-iSize)/m_Size.cx;
		double iValLT=bLTColor*(iDiagLen*0.8-sqrt(double((xPos)*(xPos)+(yPos)*(yPos))))/(iDiagLen*0.8);
		double iValRT=bRTColor*(iDiagLen*0.8-sqrt(double((m_Size.cx-xPos)*(m_Size.cx-xPos)+(yPos)*(yPos))))/(iDiagLen*0.8);
		double iValRB=bRBColor*(iDiagLen*0.8-sqrt(double((m_Size.cx-xPos)*(m_Size.cx-xPos)+(m_Size.cy-yPos)*(m_Size.cy-yPos))))/(iDiagLen*0.8);
		double iValLB=bLBColor*(iDiagLen*0.8-sqrt(double((xPos)*(xPos)+(m_Size.cy-yPos)*(m_Size.cy-yPos))))/(iDiagLen*0.8);
		double iVal=((iValLT>0?iValLT:0)+(iValRT>0?iValRT:0)+(iValRB>0?iValRB:0)+(iValLB>0?iValLB:0));
		if(iVal<0){
			iVal=0;
		}else if(iVal>255){
			iVal=255;
		}
		dst[iLayerOffset]=BYTE(iVal);
		dst+=4;
	}
}

void CDib::FillRect ( int x, int y, int w, int h, int R, int G, int B )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+w)<m_Size.cx) ? w : m_Size.cx-x;
	int dy=((y+h)<m_Size.cy) ? h : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Fill return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Address
	COLORREF *dst=m_Bits+(py*m_Size.cx)+px;
	COLORREF Color=RGB ( R, G, B);

	// Do Fill
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[i]=Color;	
		}
		dst+=m_Size.cx;
	}
}

void CDib::FillGlassRect ( int x, int y, int w, int h, int R, int G, int B, int A )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+w)<m_Size.cx) ? w : m_Size.cx-x;
	int dy=((y+h)<m_Size.cy) ? h : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to FillGlass return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Address
	BYTE *dst=(BYTE *)m_Bits+((py*m_Size.cx)+px)*4;

	// Do FillGlass
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[0]=(BYTE)(((B-dst[0])*A+(dst[0]<<8))>>8);
			dst[1]=(BYTE)(((G-dst[1])*A+(dst[1]<<8))>>8);
			dst[2]=(BYTE)(((R-dst[2])*A+(dst[2]<<8))>>8);	
			dst+=4;
		}
		dst+=(m_Size.cx-dx)<<2;
	}
}

void CDib::FillStippledGlassRect ( int x, int y, int w, int h, int R, int G, int B )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+w)<m_Size.cx) ? w : m_Size.cx-x;
	int dy=((y+h)<m_Size.cy) ? h : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to FillStippledGlass return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Address
	COLORREF *dst=m_Bits+(py*m_Size.cx)+px;
	COLORREF Color=RGB ( B, G, R );

	// Do FillStippledGlass
	for ( int j=0; j<dy; j++ )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[i]=((i+j)&0x1) ? dst[i] : Color;	
		}
		dst+=m_Size.cx;
	}
}

void CDib::BlendRect ( CDib *Dib, int x, int y, int A )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Blend return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Addresses
	BYTE *src=(BYTE *)Dib->m_Bits+(((py-y)*Dib->m_Size.cx)+px-x)*4;
	BYTE *dst=(BYTE *)m_Bits+((py*m_Size.cx)+px)*4;

	// Do Blend
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[0]=(BYTE)(((src[0]-dst[0])*A+(dst[0]<<8))>>8);
			dst[1]=(BYTE)(((src[1]-dst[1])*A+(dst[1]<<8))>>8);
			dst[2]=(BYTE)(((src[2]-dst[2])*A+(dst[2]<<8))>>8);	
			dst+=4;
			src+=4;
		}
		dst+=(m_Size.cx-dx)<<2;
		src+=(Dib->m_Size.cx-dx)<<2;
	}
}

void CDib::DarkenRect ( CDib *Dib, int x, int y )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Darken return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Addresses
	BYTE *src=(BYTE *)Dib->m_Bits+(((py-y)*Dib->m_Size.cx)+px-x)*4;
	BYTE *dst=(BYTE *)m_Bits+((py*m_Size.cx)+px)*4;

	// Do Darken
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[0]=(BYTE)((src[0]<dst[0]) ? src[0] : dst[0]);
			dst[1]=(BYTE)((src[1]<dst[1]) ? src[1] : dst[1]);
			dst[2]=(BYTE)((src[2]<dst[2]) ? src[2] : dst[2]);	
			dst+=4;
			src+=4;
		}
		dst+=(m_Size.cx-dx)<<2;
		src+=(Dib->m_Size.cx-dx)<<2;
	}
}

void CDib::DifferenceRect ( CDib *Dib, int x, int y )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Difference return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Addresses
	BYTE *src=(BYTE *)Dib->m_Bits+(((py-y)*Dib->m_Size.cx)+px-x)*4;
	BYTE *dst=(BYTE *)m_Bits+((py*m_Size.cx)+px)*4;

	// Do Difference
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			int Difference;
			Difference=src[0]-dst[0];
			dst[0]=(BYTE)((Difference<0) ? -Difference : Difference);
			Difference=src[1]-dst[1];
			dst[1]=(BYTE)((Difference<0) ? -Difference : Difference);
			Difference=src[2]-dst[2];
			dst[2]=(BYTE)((Difference<0) ? -Difference : Difference);	
			dst+=4;
			src+=4;
		}
		dst+=(m_Size.cx-dx)<<2;
		src+=(Dib->m_Size.cx-dx)<<2;
	}
}

void CDib::LightenRect ( CDib *Dib, int x, int y )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Lighten return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Addresses
	BYTE *src=(BYTE *)Dib->m_Bits+(((py-y)*Dib->m_Size.cx)+px-x)*4;
	BYTE *dst=(BYTE *)m_Bits+((py*m_Size.cx)+px)*4;

	// Do Lighten
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[0]=(BYTE)((src[0]>dst[0]) ? src[0] : dst[0]);
			dst[1]=(BYTE)((src[1]>dst[1]) ? src[1] : dst[1]);
			dst[2]=(BYTE)((src[2]>dst[2]) ? src[2] : dst[2]);
			dst+=4;
			src+=4;
		}
		dst+=(m_Size.cx-dx)<<2;
		src+=(Dib->m_Size.cx-dx)<<2;
	}
}

void CDib::MultiplyRect ( CDib *Dib, int x, int y )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Multiply return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Addresses
	BYTE *src=(BYTE *)Dib->m_Bits+(((py-y)*Dib->m_Size.cx)+px-x)*4;
	BYTE *dst=(BYTE *)m_Bits+((py*m_Size.cx)+px)*4;

	// Do Multiply
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[0]=(BYTE)(((src[0])*(dst[0]))>>8);
			dst[1]=(BYTE)(((src[1])*(dst[1]))>>8);
			dst[2]=(BYTE)(((src[2])*(dst[2]))>>8);
			dst+=4;
			src+=4;
		}
		dst+=(m_Size.cx-dx)<<2;
		src+=(Dib->m_Size.cx-dx)<<2;
	}
}

void CDib::ScreenRect ( CDib *Dib, int x, int y )
{
	// Clip Rect
	int px=(x>=0) ? x : 0;
	int py=(y>=0) ? y : 0;
	int dx=((x+Dib->m_Size.cx)<m_Size.cx) ? Dib->m_Size.cx : m_Size.cx-x;
	int dy=((y+Dib->m_Size.cy)<m_Size.cy) ? Dib->m_Size.cy : m_Size.cy-y;
	dx=(x>=0) ? dx : dx + x;
	dy=(y>=0) ? dy : dy + y;

	// If Nothing to Screen return
	if ( (dx<=0) || (dy<=0) )
		return;

	// Prepare Buffer Addresses
	BYTE *src=(BYTE *)Dib->m_Bits+(((py-y)*Dib->m_Size.cx)+px-x)*4;
	BYTE *dst=(BYTE *)m_Bits+((py*m_Size.cx)+px)*4;

	// Do Screen
	while ( dy-- )
	{
		for ( int i=0; i<dx; i++ )
		{
			dst[0]=(BYTE)(255-(((255-src[0])*(255-dst[0]))>>8));
			dst[1]=(BYTE)(255-(((255-src[1])*(255-dst[1]))>>8));
			dst[2]=(BYTE)(255-(((255-src[2])*(255-dst[2]))>>8));
			dst+=4;
			src+=4;
		}
		dst+=(m_Size.cx-dx)<<2;
		src+=(Dib->m_Size.cx-dx)<<2;
	}
}

//////////////////////////////////////////////////////////////////////
// Line Functions
//////////////////////////////////////////////////////////////////////

/*void CDib::Line ( int x1, int y1, int x2, int y2, int R, int G, int B )
{
	int dx, dy, k1, k2, d, x, y;
	COLORREF Color=RGB ( B, G, R );

	dx=x2-x1;
	dy=y2-y1;
	d=(dy<<1)-dx;
	k1=dy<<1;
	k2=(dy-dx)<<1;
	x=x1;
	y=y1;

	m_Bits[y*m_Size.cx+x]=Color;
	while (x<dx) 
	{
		if (d<=0) 
		{
			d+=k1;
			x++;
		} 
		else 
		{
			d+=k2;
			x++;
			y++;
		}
		m_Bits[y*m_Size.cx+x]=Color;
	}
}*/

COLORREF& CDib::GetPixel(int x, int y)
{
	if(!m_Bits){
		static COLORREF cTmp=0;
		return cTmp;
	}
	return m_Bits[y*m_Size.cx+x];
}

void CDib::Line ( int x1, int y1, int x2, int y2, int R, int G, int B )
{
	int d, x, y, ax, ay, sx, sy, dx, dy;
	COLORREF Color=RGB ( B, G, R );
	
	dx=x2-x1;
	ax=abs ( dx )<<1;
	sx=(dx<0) ? -1 : 1;
	dy=y2-y1;
	ay=abs ( dy )<<1;
	sy=(dy<0) ? -1 : 1;
	x=x1;
	y=y1;
	
	if ( ax>ay )
	{
		d=ay-(ax>>1);
		while ( x!=x2 )
		{
			m_Bits[y*m_Size.cx+x]=Color;
			if ( d>=0 )
			{
				y+=sy;
				d-=ax;
			}
			x+=sx;
			d+=ay;
		}
	}
	else
	{
		d=ax-(ay>>1);
		while ( y!=y2 )
		{
			m_Bits[y*m_Size.cx+x]=Color;
			if ( d>=0 )
			{
				x+=sx;
				d-=ay;
			}
			y+=sy;
			d+=ax;
		}
	}
}

void CDib::LineGlass ( int x1, int y1, int x2, int y2, int R, int G, int B, int A )
{
	int d, x, y, ax, ay, sx, sy, dx, dy;
	COLORREF Color=RGB ( B, G, R );
	BYTE *dst=(BYTE *)m_Bits;
	
	dx=x2-x1;
	ax=abs ( dx )<<1;
	sx=(dx<0) ? -1 : 1;
	dy=y2-y1;
	ay=abs ( dy )<<1;
	sy=(dy<0) ? -1 : 1;
	x=x1;
	y=y1;
	
	if ( ax>ay )
	{
		d=ay-(ax>>1);
		while ( x!=x2 )
		{
			dst[(y*m_Size.cx+x)<<2]=(BYTE)(((B-dst[(y*m_Size.cx+x)<<2])*A+(dst[(y*m_Size.cx+x)<<2]<<8))>>8);
			dst[((y*m_Size.cx+x)<<2)+1]=(BYTE)(((G-dst[((y*m_Size.cx+x)<<2)+1])*A+(dst[((y*m_Size.cx+x)<<2)+1]<<8))>>8);
			dst[((y*m_Size.cx+x)<<2)+2]=(BYTE)(((R-dst[((y*m_Size.cx+x)<<2)+2])*A+(dst[((y*m_Size.cx+x)<<2)+2]<<8))>>8);
			if ( d>=0 )
			{
				y+=sy;
				d-=ax;
			}
			x+=sx;
			d+=ay;
		}
	}
	else
	{
		d=ax-(ay>>1);
		while ( y!=y2 )
		{
			dst[(y*m_Size.cx+x)<<2]=(BYTE)(((B-dst[(y*m_Size.cx+x)<<2])*A+(dst[(y*m_Size.cx+x)<<2]<<8))>>8);
			dst[((y*m_Size.cx+x)<<2)+1]=(BYTE)(((G-dst[((y*m_Size.cx+x)<<2)+1])*A+(dst[((y*m_Size.cx+x)<<2)+1]<<8))>>8);
			dst[((y*m_Size.cx+x)<<2)+2]=(BYTE)(((R-dst[((y*m_Size.cx+x)<<2)+2])*A+(dst[((y*m_Size.cx+x)<<2)+2]<<8))>>8);
			if ( d>=0 )
			{
				x+=sx;
				d-=ay;
			}
			y+=sy;
			d+=ax;
		}
	}
}

BOOL CDib::GetRectMiddleValue(long xPos,DWORD dwWidth,long yPos,DWORD dwHeight,BYTE& R, BYTE& G, BYTE& B)
{
	int iRMid=0;
	int iGMid=0;
	int iBMid=0;
	int iCount=0;
	BYTE Rt=0,Gt=0,Bt=0;
	for(long ix=xPos;ix<long(xPos+dwWidth);ix++){
		for(long iy=yPos;iy<long(yPos+dwHeight);iy++){
			GetPixel(ix,iy,Rt,Gt,Bt,TRUE);
			iRMid+=Rt;
			iGMid+=Gt;
			iBMid+=Bt;
			iCount++;
		}
	}
	if(iCount>0){
		R=iRMid/iCount;
		G=iGMid/iCount;
		B=iBMid/iCount;
	}
	return TRUE;
}

DWORD pColors[100]={0};
BOOL CDib::GetRectMedianValue(long xPos,DWORD dwWidth,long yPos,DWORD dwHeight,DWORD& dwColor,DWORD dwStep)
{
	int iCount=0;
	int iColCount=0;
	DWORD dwPrevColor=0,dwCurColor=0;
	for(long ix=xPos;ix<long(xPos+dwWidth);ix+=dwStep){
		for(long iy=yPos;iy<long(yPos+dwHeight);iy+=dwStep){
			if(iy<m_Size.cy && ix<m_Size.cx){
				dwCurColor=m_Bits[iy*m_Size.cx+ix];
				if(dwCurColor!=dwPrevColor){
					BOOL bNewCol=TRUE;
					/*for(int k=0;k<iColCount;k++){
						if(pColors[k]==dwCurColor){
							bNewCol=FALSE;
							break;
						}
					}*/
					dwPrevColor=dwCurColor;
					if(bNewCol){
						pColors[iColCount++]=dwCurColor;
						if(iColCount>100){
							return FALSE;
						}
					}
				}
			}
		}
	}
	if(iColCount>0){
		// Сортируем...
		for(int i=0;i<(iColCount-1)/2;i++){
			for(int j=i;j<iColCount;j++){
				DWORD dwC1=pColors[i];
				DWORD dwC2=pColors[j];
				if(GetRValue(dwC1)+GetGValue(dwC1)+GetBValue(dwC1)<
					GetRValue(dwC2)+GetGValue(dwC2)+GetBValue(dwC2)){
					DWORD dwTmp=pColors[i];
					pColors[i]=pColors[j];
					pColors[j]=dwTmp;
				}
			}
		}
		dwColor=pColors[int(iColCount/2)];
	}
	return TRUE;
}

BOOL CDib::GetPixel(int x, int y, BYTE& R, BYTE& G, BYTE& B, BOOL bInvertEdges, BOOL bmIXmODE)
{
	if(!m_Bits){
		return 0;
	}
	if(x<0 || x>=m_Size.cx || y<0 || y>=m_Size.cy){
		if(bInvertEdges){
			if(x<0){
				x=-x;
			}
			if(y<0){
				y=-y;
			}
			if(x>=m_Size.cx){
				x=m_Size.cx-(x+1-m_Size.cx);
			}
			if(y>=m_Size.cy){
				y=m_Size.cy-(y+1-m_Size.cy);
			}
		}
		if(x<0){
			x=m_Size.cx-(-x)%m_Size.cx-1;
		}
		if(x>=m_Size.cx){
			x=x%m_Size.cx;
		}
		if(y<0){
			y=m_Size.cy-(-y)%m_Size.cy-1;
		}
		if(y>=m_Size.cy){
			y=y%m_Size.cy;
		}
	}
	COLORREF RGB=m_Bits[y*m_Size.cx+x];
	if(bmIXmODE){
		R=(R+GetRValue(RGB))/2;
		G=(G+GetGValue(RGB))/2;
		B=(B+GetBValue(RGB))/2;
	}else{
		R=GetRValue(RGB);
		G=GetGValue(RGB);
		B=GetBValue(RGB);
	}
	return TRUE;
}

BOOL CDib::DarkenPixel(int x, int y, double dwPercentage)
{
	if(!m_Bits){
		return 0;
	}
	if(x<0 || x>=m_Size.cx || y<0 || y>=m_Size.cy){
		if(x<0){
			x=m_Size.cx-(-x)%m_Size.cx-1;
		}else if(x>=m_Size.cx){
			x=x%m_Size.cx;
		}
		if(y<0){
			y=m_Size.cy-(-y)%m_Size.cy-1;
		}else if(y>=m_Size.cy){
			y=y%m_Size.cy;
		}
	}
	DWORD dwRGB=m_Bits[y*m_Size.cx+x];
	dwRGB=RGB(GetRValue(dwRGB)*dwPercentage,
		GetGValue(dwRGB)*dwPercentage,
		GetBValue(dwRGB)*dwPercentage);
	m_Bits[y*m_Size.cx+x]=dwRGB;
	return TRUE;
}

BOOL CDib::SetPixel(int x, int y, DWORD RGB, BOOL bMixMode)
{
	if(!m_Bits){
		return 0;
	}
	if(x<0 || x>=m_Size.cx || y<0 || y>=m_Size.cy){
		if(x<0){
			x=m_Size.cx-(-x)%m_Size.cx-1;
		}else if(x>=m_Size.cx){
			x=x%m_Size.cx;
		}
		if(y<0){
			y=m_Size.cy-(-y)%m_Size.cy-1;
		}else if(y>=m_Size.cy){
			y=y%m_Size.cy;
		}
	}
	if(bMixMode){
		DWORD dwOffset=y*m_Size.cx+x;
		m_Bits[dwOffset]=(m_Bits[dwOffset]+RGB)/2;
	}else{
		m_Bits[y*m_Size.cx+x]=RGB;
	}
	return TRUE;
}

BOOL CDib::GetPixelBrightness( int x, int y, int& iBr)
{
	if(!m_Bits){
		return FALSE;
	}
	if(x<0 || y<0 || x>=m_Size.cx || y>=m_Size.cy){
		return FALSE;
	}
	COLORREF RGB=m_Bits[y*m_Size.cx+x];
	iBr=(GetRValue(RGB)+GetGValue(RGB)+GetBValue(RGB))/3;
	return TRUE;
}

BYTE RGB2BW(BYTE r, BYTE g, BYTE b)
{
	return BYTE(0.229*r+0.587*g+0.114*b);
}

BYTE RGB2BW(COLORREF rgb)
{
	return RGB2BW(GetRValue(rgb),GetGValue(rgb),GetBValue(rgb));
}


BYTE RGB2BW_F(BYTE r, BYTE g, BYTE b)
{
	return BYTE((r+g+b+255)>>2);
}

BYTE RGB2BW_F(COLORREF rgb)
{
	return RGB2BW_F(GetRValue(rgb),GetGValue(rgb),GetBValue(rgb));
}

double DISTANCE(int x, int y, int x1, int y1)
{
	return sqrt(double((x-x1)*(x-x1)+(y-y1)*(y-y1)));
}

BYTE l2B(int iVal)
{
	if(iVal>255){
		iVal=255;
	}
	if(iVal<0){
		iVal=0;
	}
	return iVal;
}

BYTE l2B(double iVal)
{
	return l2B(int(iVal));
}


DWORD GetColorBlend(DWORD dwBegin, DWORD dwEnd, double dwTonicLevel, BOOL bStsr)
{
	if(bStsr && dwBegin>dwEnd){
		return GetColorBlend(dwEnd, dwBegin, dwTonicLevel);
	}
	BYTE R=l2B(int(GetRValue(dwBegin)*dwTonicLevel+GetRValue(dwEnd)*(1-dwTonicLevel)));
	BYTE G=l2B(int(GetGValue(dwBegin)*dwTonicLevel+GetGValue(dwEnd)*(1-dwTonicLevel)));
	BYTE B=l2B(int(GetBValue(dwBegin)*dwTonicLevel+GetBValue(dwEnd)*(1-dwTonicLevel)));
	return RGB(R,G,B);
}

