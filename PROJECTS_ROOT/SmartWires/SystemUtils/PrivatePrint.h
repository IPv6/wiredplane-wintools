// PrivatePrint.h: defination for class CPrivatePrint.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRIVATEPRINT_H__B83675D5_F325_4A55_B8A4_B22AB09A07AB__INCLUDED_)
#define AFX_PRIVATEPRINT_H__B83675D5_F325_4A55_B8A4_B22AB09A07AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef int	HPRIVATEFONT;

#define	CALCF(x)		(-MulDiv(x, GetDeviceCaps(m_DC.m_hDC, LOGPIXELSY), 72))
#define	CALCY(y)		(MulDiv(y, GetDeviceCaps(m_DC.m_hDC, LOGPIXELSY), 72))
#define	CALCX(x)		(MulDiv(x, GetDeviceCaps(m_DC.m_hDC, LOGPIXELSX), 72))

// each added font can have 4 attributes
#define		FACE_NORMAL			0	
#define		FACE_NORMALBOLD		1
#define		FACE_BIG			2
#define		FACE_BIGBOLD		3

// for Print() <-- how should the text be aligned within the line?
// these formats can be combined with the | operator
#define FORMAT_CENTER	1
#define FORMAT_NORMAL	2
#define FORMAT_RIGHT	4
#define FORMAT_NOLF		8	// no automatic linefeed

// Error-codes
#define	PRERR_OK					0
#define	PRERR_NOIMAGES				1
#define PRERR_LOADBITMAPFAILED		2
#define PRERR_NOGETOBJECT			3
#define	PRERR_NOCOMPATIBLEDC		4
#define PRERR_NOSELECTOBJECT		5
#define PRERR_STRETCHBLTFAILED		6
#define	PRERR_STARTPAGEFAILED		7
#define PRERR_CANTCREATEPRINTERDC	8
#define PRERR_NOBITBLT				9

class CPrivatePrint  
{
public:
	CPrivatePrint();
	~CPrivatePrint	();
	HPRIVATEFONT	GetActiveFont ();
	HPRIVATEFONT	AddFontToEnvironment (char *FontName, int normalHeight = 10, int bigHeight = 16);
	void	CreateDPICompatibleRect (CRect& rect, int top, int height, int left, int width);
	void	GetDrawDimension	(CSize& dim);
	int		GetNumCopies		() { return (m_numCopies); };
	void	GetMargins			(CRect& Margins);
	int		GetHPos				();
	CDC*	GetPrivateDC		() { return (&m_DC); };
	int		SetMargins			(int top, int bottom, int left, int right);
	int		SetMargins			(CRect rect);
	void	SetDistance			(int points);
	void	SetFace				(HPRIVATEFONT font, UINT face);
	void	SetHPos				(int Pos);
	void	SetActiveFont		(HPRIVATEFONT newfnt);
	void	lf					(HPRIVATEFONT font, UINT Anz=1);
	void	Print				(HPRIVATEFONT font, CString strText, UINT Format, int left=0);
	void	Line				(int Mode);
	int		Dialog				(char*  PrinterName = NULL, int Pages = 1);
	void	NewPage				();
	void	ActivateHF			(void(*HFFunc)(CPrivatePrint*,int,bool));
	int		StartPrint			(CString strTitle);
	void	EndPrint			();
	int		StartPage			();
	void	EndPage				();
	int		InsertBitmap		(int ResourceIDColor, int fmt, CSize* dest=NULL, int left=0);
	void	IncrementHPos		(int points);
	int		GetErrorCode		() { return (m_LastErrNo); };
	int		SetEscapement		(HPRIVATEFONT font, int Angle);
	HANDLE	DDBToDIB			(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal );
    void	SetDialog			(CPrintDialog* dlg) { m_printDlg = dlg; }

private:
	CDC		m_DC;			// device-context for printing
	int		m_numFonts;		// number of added fonts, max. 10 possible
	int		m_numCopies;	// number of copies, which should printed
	CSize	m_DimDraw;		// max width/height, which the printer allowes on the paper
	CFont*	m_fnt[10][4];	// 10 different fonts with 4 different apperances for each
	CRect	m_Margins;		// desired margins
	UINT	m_ActFace[10];	// actual adjusted apperance of a font
	int		m_y;			// cursor-position at page during the printing
	bool	m_HFPrinting;	// is a headline desired?
	int		m_Abstand;		// distance of lines in points
	int		m_Page;			// actual page-number
	int		m_normalHeight, m_bigHeight;	// size in points of normal font and height-font
	int		m_LastErrNo;	// contains in error-case the error-number. can get with GetErrorCode()
	HDC		m_hprinter;
	CDC		m_PrinterDC;	// is needed for printing without printer-dialog
	bool	m_IsHF;			// is needed for printing of head/footline
    CPrintDialog    *m_printDlg; // pointer to a various dialog-class, if desired
	HPRIVATEFONT	m_ActiveFont;	// actual active font
	void(*m_funcHeadline)(CPrivatePrint*,int,bool);	// pointer to function, which is handling the head/footline
};

#endif // !defined(AFX_PRIVATEPRINT_H__B83675D5_F325_4A55_B8A4_B22AB09A07AB__INCLUDED_)
