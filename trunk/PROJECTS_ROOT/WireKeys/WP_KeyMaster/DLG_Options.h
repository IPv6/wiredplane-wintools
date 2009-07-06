#if !defined(AFX_DLG_OPTIONS_H__DE4A09A2_E8C1_43FB_BF29_55B089F8B0BA__INCLUDED_)
#define AFX_DLG_OPTIONS_H__DE4A09A2_E8C1_43FB_BF29_55B089F8B0BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Options.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_Options dialog

class CDLG_Options : public CDialog
{
// Construction
public:
	CSmartWndSizer Sizer;
	CDLG_Options(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_Options)
	enum { IDD = IDD_KOPTIONS };
	CString	m_AKeys;
	BOOL	m_bGuide;
	//BOOL	m_bLRDisting;
	BOOL	m_bTempBlockHotkeys;
	BOOL	m_bBlockApps;
	BOOL	m_bBlockWin;
	//BOOL	m_bUseMouseBt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Options)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_Options)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_OPTIONS_H__DE4A09A2_E8C1_43FB_BF29_55B089F8B0BA__INCLUDED_)
