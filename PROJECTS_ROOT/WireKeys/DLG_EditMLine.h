#if !defined(AFX_DLG_EDITMLINE_H__016690C3_9FA2_4CBB_A336_805A5663631C__INCLUDED_)
#define AFX_DLG_EDITMLINE_H__016690C3_9FA2_4CBB_A336_805A5663631C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_EditMLine.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_EditMLine dialog

class CDLG_EditMLine : public CDialog
{
// Construction
public:
	CString sTitle;
	CDLG_EditMLine(CWnd* pParent = NULL);   // standard constructor
	CString sText;
	int iMode;
// Dialog Data
	//{{AFX_DATA(CDLG_EditMLine)
	enum { IDD = IDD_DIALOG_EDIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_EditMLine)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_EditMLine)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_EDITMLINE_H__016690C3_9FA2_4CBB_A336_805A5663631C__INCLUDED_)
