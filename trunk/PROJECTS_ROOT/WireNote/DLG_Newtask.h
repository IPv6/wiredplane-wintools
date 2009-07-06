#if !defined(AFX_DLG_NEWTASK_H__8121D1AF_3888_4BA4_9A98_F3DA5A17ED97__INCLUDED_)
#define AFX_DLG_NEWTASK_H__8121D1AF_3888_4BA4_9A98_F3DA5A17ED97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Newtask.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_Newtask dialog

class CDLG_Newtask : public CDialog
{
// Construction
public:
	void SetDscText(CString sDesc);
	BOOL bWasSemi; 
	void OnFolderDblClick();
	CWnd* pParent;
	BOOL DepictToDoFolder();
	POINT fromPoint;
	BOOL isMove;
	CFont* font;
	LOGFONT logfont;
	CBrush* m_hBr;
	void SetColorCode(long iCode);
	void Finalize(int iRes=IDCANCEL);
	const char* szPath;
	SimpleTracker Track;
	CImageList m_ToDoIconImages;
	CImageList m_ToDoPrImages;
	void CommitData();
	BOOL bNewTask;
	int iLastPriority;
	Task* pTask;
	CSmartWndSizer Sizer;
	CDLG_Newtask(CWnd* pParent = NULL);   // standard constructor
	~CDLG_Newtask();

// Dialog Data
	//{{AFX_DATA(CDLG_Newtask)
	enum { IDD = IDD_NEWTASK };
	CEdit m_Folder;
	CBitmapStatic m_FolderSel;
	CBitmapStatic m_CycleBg;
	CBitmapStatic m_Delete;
	CBitmapStatic m_Actions;
	CComboBoxEx	m_ToDoIcon;
	CComboBoxEx	m_Priority;
	CNoteEditCtrl m_sDsc;
	CString	m_sName;
	BOOL	m_IncludeInRep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Newtask)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CDLG_Newtask)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnActions();
	afx_msg void OnDelete();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCyclebg();
	afx_msg void OnFoldersel();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNotemenuNewstyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_NEWTASK_H__8121D1AF_3888_4BA4_9A98_F3DA5A17ED97__INCLUDED_)
