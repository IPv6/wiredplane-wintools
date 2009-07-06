#if !defined(AFX_DLG_ADDLINK_H__730DE720_B128_42AB_99F1_C9542605E619__INCLUDED_)
#define AFX_DLG_ADDLINK_H__730DE720_B128_42AB_99F1_C9542605E619__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_AddLink.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_AddLink dialog

class CDLG_AddLink : public CDialog
{
// Construction
public:
	int iType;
	SimpleTracker Track;
	CString* pLink;
	CDLG_AddLink(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_AddLink)
	enum { IDD = IDD_WPLINK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_AddLink)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_AddLink)
	afx_msg void OnAdd();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_ADDLINK_H__730DE720_B128_42AB_99F1_C9542605E619__INCLUDED_)
