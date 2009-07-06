#if !defined(AFX_NOTEEDITCTRL_H__8B507423_ECB5_4120_9BDA_DA18CB45E515__INCLUDED_)
#define AFX_NOTEEDITCTRL_H__8B507423_ECB5_4120_9BDA_DA18CB45E515__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NoteEditCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNoteEditCtrl window

class CNoteEditCtrl;
typedef BOOL (CALLBACK *OnFocusHandler) (LPVOID pControl);
class CLinkHandler
{
	public:
	virtual BOOL HandleLink(const char* sz)=0;
};

class CNoteEditCtrl : public CRichEditCtrl
{
// Construction
public:
	CLinkHandler* pLinkHandler;
	BOOL bWBProc;
	BOOL bSmilesPresent;
	void SetFont(CFont* font, BOOL bRedraw);
	CFont* pFont;
	BOOL DrawSmile(CString sSmile,CString sSmileBmp, CDC* dc,int iFirstVisC,DWORD iLIneH);
	DWORD dwBGColor;
	void DrawContent(CDC* dc,BOOL bDislpay);
	CString sInternalStringCopy;
	void OnPaint();
	BOOL SetColor(COLORREF dwTX,COLORREF dwBG);
	BOOL SetText(const char* sz);
	BOOL bIgnoreFocus;
	BOOL bBorderSet;
	OnFocusHandler pFocusHandler;
	LPVOID pFocusHandlerData;
	BOOL bDisableStandartContextMenu;
	BOOL bWordWrap;

	int IsFlatInit;
	int InFocus;
	CNoteEditCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoteEditCtrl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNoteEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNoteEditCtrl)
	afx_msg void OnLinkEvent(NMHDR* pHDR, LRESULT* pRes);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

class CNoteComboBoxEx : public CComboBoxEx
{
// Construction
public:
	BOOL bIgnoreFocus;
	DWORD bgColor;
	DWORD txColor;
	HBRUSH hbr;
	CNoteComboBoxEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoteComboBoxEx)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNoteComboBoxEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNoteComboBoxEx)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_NOTEEDITCTRL_H__8B507423_ECB5_4120_9BDA_DA18CB45E515__INCLUDED_)
