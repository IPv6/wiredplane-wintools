#if !defined(AFX_DLG_TaskHelper_H__17101BF6_D941_4AF2_9421_B2C000ADB783__INCLUDED_)
#define AFX_DLG_TaskHelper_H__17101BF6_D941_4AF2_9421_B2C000ADB783__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_TaskHelper.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_TaskHelper dialog

class CDLG_TaskHelper : public CDialog
{
// Construction
public:
	BOOL bStarted;
	CDLG_TaskHelper(CWnd* pParent = NULL);   // standard constructor
	~CDLG_TaskHelper();
// Dialog Data
	//{{AFX_DATA(CDLG_TaskHelper)
	enum { IDD = IDD_TBAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_TaskHelper)
	public:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_TaskHelper)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_TaskHelper_H__17101BF6_D941_4AF2_9421_B2C000ADB783__INCLUDED_)
