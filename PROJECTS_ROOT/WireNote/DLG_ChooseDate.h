#if !defined(AFX_DLG_CHOOSEDATE_H__4E8F38BD_3FE7_45A7_8E65_3F8B192B4DAF__INCLUDED_)
#define AFX_DLG_CHOOSEDATE_H__4E8F38BD_3FE7_45A7_8E65_3F8B192B4DAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_ChooseDate.h : header file
//
#include "_common.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// DLG_ChooseDate dialog

class DLG_ChooseDate : public CDialog
{
// Construction
public:
	SimpleTracker Track;
	DLG_ChooseDate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DLG_ChooseDate)
	enum { IDD = IDD_REPORTFROM };
	CTime	m_Date;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DLG_ChooseDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DLG_ChooseDate)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CHOOSEDATE_H__4E8F38BD_3FE7_45A7_8E65_3F8B192B4DAF__INCLUDED_)
