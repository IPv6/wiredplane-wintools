#if !defined(AFX_DLG_SERVERTEST_H__0FB58CA5_931B_4830_BA3C_C980AE3D5710__INCLUDED_)
#define AFX_DLG_SERVERTEST_H__0FB58CA5_931B_4830_BA3C_C980AE3D5710__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_ServerTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DLG_ServerTest dialog
#include "\PROJECTS_ROOT\Tools\SmartWndSizer\SmartWndSizer.h"
class DLG_ServerTest : public CDialog
{
// Construction
public:
	DWORD dwTimer;
	CSmartWndSizer Sizer;
	int iServerIndex;
	DLG_ServerTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DLG_ServerTest)
	enum { IDD = IDD_TESTSERVER };
	CString	m_Port;
	CString	m_Statistics;
	CString	m_Status;
	CString	m_data;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DLG_ServerTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
	public:

	// Generated message map functions
	//{{AFX_MSG(DLG_ServerTest)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnGetstatistic();
	afx_msg void OnUpdate();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_SERVERTEST_H__0FB58CA5_931B_4830_BA3C_C980AE3D5710__INCLUDED_)
