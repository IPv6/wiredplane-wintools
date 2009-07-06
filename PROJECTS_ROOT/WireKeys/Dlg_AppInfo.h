#if !defined(AFX_DLG_APPINFO_H__409A34A3_D115_4E5D_B48F_E56107AF507C__INCLUDED_)
#define AFX_DLG_APPINFO_H__409A34A3_D115_4E5D_B48F_E56107AF507C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_AppInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_AppInfo dialog

class CDlg_AppInfo : public CDialog
{
// Construction
public:
	DWORD dwProcId;
	CSmartWndSizer Sizer;
	CDlg_AppInfo(CWnd* pParent = NULL);   // standard constructor
	CString sAppPath;
	HWND hSlWnd;
// Dialog Data
	//{{AFX_DATA(CDlg_AppInfo)
	enum { IDD = IDD_APP_INFO };
	CString	m_Info;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_AppInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_AppInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnOpen();
	afx_msg void OnWnds();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_APPINFO_H__409A34A3_D115_4E5D_B48F_E56107AF507C__INCLUDED_)
