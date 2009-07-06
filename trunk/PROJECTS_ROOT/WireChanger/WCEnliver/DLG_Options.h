#if !defined(AFX_DLG_OPTIONS_H__19A36502_2E9D_4271_8515_2ACE6AB54A97__INCLUDED_)
#define AFX_DLG_OPTIONS_H__19A36502_2E9D_4271_8515_2ACE6AB54A97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Options.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_Options dialog
#include "resource.h"
class CDLG_Options : public CDialog
{
// Construction
public:
	CDLG_Options(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_Options)
	enum { IDD = IDD_EOPTIONS };
	int		m_Quality;
	int		m_Resolution;
	CString	m_Speed;
	int		m_Effect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Options)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

#endif // !defined(AFX_DLG_OPTIONS_H__19A36502_2E9D_4271_8515_2ACE6AB54A97__INCLUDED_)
