#if !defined(AFX_DLG_CHOOSER_H__CBF77C8B_3E60_49F9_9CDA_B286DC12C7FE__INCLUDED_)
#define AFX_DLG_CHOOSER_H__CBF77C8B_3E60_49F9_9CDA_B286DC12C7FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include "_common.h"

/////////////////////////////////////////////////////////////////////////////
// CDLG_Chooser dialog
class CDLG_Chooser : public CDialog
{
// Construction
public:
	int m_iIconID;
	int m_iStartElen;
	CString m_sTitle;
	CStringArray* m_pOptionList;
	CDLG_Chooser(const char* szTitle,const char* szHelpString, CStringArray* pOptionList, int iStartElen, int iIconID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_Chooser)
	enum { IDD = IDD_SIMPLECHOOSER };
	CComboBox	m_ChooseList;
	CString	m_HelpString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Chooser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_Chooser)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CHOOSER_H__CBF77C8B_3E60_49F9_9CDA_B286DC12C7FE__INCLUDED_)
