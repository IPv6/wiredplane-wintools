#if !defined(AFX_ALERTDIALOG_H__A6A62042_7D20_402C_B34A_5795688CD9C8__INCLUDED_)
#define AFX_ALERTDIALOG_H__A6A62042_7D20_402C_B34A_5795688CD9C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlertDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlertDialog dialog

class CAlertDialog : public CDialog
{
// Construction
public:
	CString sText;
	long iRes;
	long lDoAutoclose;
	CAlertDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlertDialog)
	enum { IDD = IDD_DIALOG_PP };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlertDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlertDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnOk2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALERTDIALOG_H__A6A62042_7D20_402C_B34A_5795688CD9C8__INCLUDED_)
