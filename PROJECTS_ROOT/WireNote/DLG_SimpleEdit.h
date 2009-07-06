#if !defined(AFX_DLGSIMPLEEDIT_H__52F17A01_8B8B_455A_B2CE_653E95FFBBFA__INCLUDED_)
#define AFX_DLGSIMPLEEDIT_H__52F17A01_8B8B_455A_B2CE_653E95FFBBFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSimpleEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSimpleEdit dialog

class CDlgSimpleEdit : public CDialog
{
// Construction
public:
	int iStyle;
	CDlgSimpleEdit(CWnd* pParent = NULL);   // standard constructor
	afx_msg void OnBrowse();
	CSmartWndSizer Sizer;
// Dialog Data
	//{{AFX_DATA(CDlgSimpleEdit)
	enum { IDD = IDD_SIMPLEEDIT };
	CString	m_Edit;
	CString	m_EditTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSimpleEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSimpleEdit)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSIMPLEEDIT_H__52F17A01_8B8B_455A_B2CE_653E95FFBBFA__INCLUDED_)
