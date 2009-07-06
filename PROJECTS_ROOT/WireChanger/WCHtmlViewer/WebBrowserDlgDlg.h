// WebBrowserDlgDlg.h : header file
//
//{{AFX_INCLUDES()
#include "webbrowser2.h"
#include "afxwin.h"
//}}AFX_INCLUDES

#if !defined(AFX_WEBBROWSERDLGDLG_H__48BB8F83_CAEF_48BA_A181_8A76EF15DCC1__INCLUDED_)
#define AFX_WEBBROWSERDLGDLG_H__48BB8F83_CAEF_48BA_A181_8A76EF15DCC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserDlgDlg dialog
class CWebBrowserDlgDlg : public CResizeableDialog
{
// Construction
public:
	int iCounter;
	DWORD dwTimer;
	CString sBGText;
	HANDLE hLookingThread;
	CWebBrowserDlgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWebBrowserDlgDlg)
	enum { IDD = IDD_WEBBROWSERDLG_DIALOG };
	CWebBrowser2	m_WebBrowser;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebBrowserDlgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWebBrowserDlgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeMenuMode();
	afx_msg BOOL OnEraseBkgnd( CDC * );
	//}}AFX_MSG
	BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomControlSiteMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBBROWSERDLGDLG_H__48BB8F83_CAEF_48BA_A181_8A76EF15DCC1__INCLUDED_)
