#if !defined(AFX_DLG_ABOUT_H__EFDDB15B_D307_4905_AF3D_24CD935F299A__INCLUDED_)
#define AFX_DLG_ABOUT_H__EFDDB15B_D307_4905_AF3D_24CD935F299A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_About.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_About dialog
#include "_common.h"
class CDLG_About : public CDialog
{
// Construction
public:
	CSmartWndSizer Sizer;
	CDLG_About(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_About)
	enum { IDD = IDD_ABOUTBOX };
	CBitmapStatic m_Logo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_About)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_About)
	virtual BOOL OnInitDialog();
	afx_msg void OnEntcode();
	afx_msg void OnWebsite();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_ABOUT_H__EFDDB15B_D307_4905_AF3D_24CD935F299A__INCLUDED_)
