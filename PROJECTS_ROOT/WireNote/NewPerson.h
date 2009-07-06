#if !defined(AFX_NEWPERSON_H__45C6486C_1BD4_409D_BBA0_CECECA973EF4__INCLUDED_)
#define AFX_NEWPERSON_H__45C6486C_1BD4_409D_BBA0_CECECA973EF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewPerson.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewPerson dialog

class CNewPerson : public CDialog
{
// Construction
public:
	SimpleTracker Track;
	CSmartWndSizer Sizer;
	CNewPerson(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewPerson)
	enum { IDD = IDD_PERSON };
	CString	m_CompName;
	CString	m_Nick;
	CString	m_Dsc;
	BOOL	m_bGroup;
	int		m_iTab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewPerson)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewPerson)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCompaddr();
	afx_msg void OnCompFind();
	afx_msg void OnChangeEditIp();
	afx_msg void OnSelchangeComboProt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPERSON_H__45C6486C_1BD4_409D_BBA0_CECECA973EF4__INCLUDED_)
