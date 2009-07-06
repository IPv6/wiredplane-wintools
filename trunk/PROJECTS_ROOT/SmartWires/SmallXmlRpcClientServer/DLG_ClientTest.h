#if !defined(AFX_DLG_CLIENTTEST_H__301E9B39_D301_4EEE_ABF6_96DD8CEC07C8__INCLUDED_)
#define AFX_DLG_CLIENTTEST_H__301E9B39_D301_4EEE_ABF6_96DD8CEC07C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_ClientTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DLG_ClientTest dialog
#include "\PROJECTS_ROOT\Tools\SmartWndSizer\SmartWndSizer.h"
class DLG_ClientTest : public CDialog
{
// Construction
public:
	DWORD dwTimer;
	CSmartWndSizer Sizer;
	DLG_ClientTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DLG_ClientTest)
	enum { IDD = IDD_TESTCLIENT };
	CButton	m_Stats;
	CString	m_Address;
	CString	m_Received;
	CString	m_Sended;
	CString	m_Method;
	CString	m_Statistica;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DLG_ClientTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DLG_ClientTest)
	afx_msg void OnStart();
	afx_msg void OnGetstatistic();
	virtual BOOL OnInitDialog();
	afx_msg void OnStartSync();
	afx_msg void OnStartAsync();
	afx_msg void OnUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CLIENTTEST_H__301E9B39_D301_4EEE_ABF6_96DD8CEC07C8__INCLUDED_)
