#if !defined(AFX_DLG_ANTISPAM_H__35E469EB_96CD_48E5_A712_A370006E72D7__INCLUDED_)
#define AFX_DLG_ANTISPAM_H__35E469EB_96CD_48E5_A712_A370006E72D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_AntiSpam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_AntiSpam dialog

#define WM_REFRESHANTISPAM	WM_USER+1
class CEmailHeader;
class CAntiSpamAccount;
class CDLG_AntiSpam : public CDialog
{
// Construction
public:
	BOOL SetTitle(const char* szPrefix);
	CMenuToolTip ttMail;
	void ShowItemBody(CEmailHeader* item);
	void RefreshSingleMessage(CEmailHeader* head);
	CNoteSettings* PrepareNoteForMessage(CEmailHeader* item);
	afx_msg void OnRCLICK_TOGGLEMARK();
	afx_msg void OnRCLICK_REREAD();
	afx_msg void OnRCLICK_DELETEFROMSERVER();
	afx_msg void OnRCLICK_DOWNLOADNEXTLINES(int iLines);
	afx_msg void OnRCLICK_SHOWBODY();
	afx_msg void OnRCLICK_SHOWRAWBODY();
	afx_msg void OnRCLICK_SHOWALLHEADERS();
	CEmailHeader* GetSelectedMessage(int* iSelectedCol=NULL);
	void OnRClick();
	CString sLog;
	CFont* font;
	CFont* font2;
	LOGFONT logfont;
	CIListCtrl m_MsgList;
	CToolTipCtrl m_tooltip;
	void SetColorStyle();
	void InitMsgList();
	CSmartWndSizer Sizer;
	CAntiSpamAccount* m_pAccount;
	CString GetEmailDsc(CEmailHeader* mail);
	CEmailHeader* GetItemUnderCursor(int* iUnderCursorCol=NULL);
	void OnMsgSel(NMHDR * pNotifyStruct, LRESULT * result);
	void OnCheckboxClick(NMHDR * pNotifyStruct, LRESULT * result);
	void OnDblclkMsglist(NMHDR * pNotifyStruct, LRESULT * result);
	void ShowMsgInServerLog(CEmailHeader* item);
	CDLG_AntiSpam(CAntiSpamAccount* pAccount, CWnd* pParent=NULL);
	~CDLG_AntiSpam();
	long GetMarkedCount();
	const char* GetSpamTargetFile();
	void RefreshCurrentInfo();
	void RefreshMessagesTable(BOOL bClearFirst=TRUE);
	// Dialog Data
	//{{AFX_DATA(CDLG_AntiSpam)
	enum { IDD = IDD_ANTISPAM };
	CButton	m_AutoRunEm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_AntiSpam)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_AntiSpam)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSelall();
	afx_msg void OnSelnothing();
	afx_msg void OnRefilter();
	afx_msg void OnInvert();
	afx_msg void OnConfig();
	afx_msg void OnClose();
	afx_msg void OnButtonMulty();
	afx_msg void OnRunemailclient();
	afx_msg void OnButtonAddstr();
	afx_msg void OnButtonEditstr();
	afx_msg void OnViewlog();
	afx_msg void OnDestroy();
	afx_msg void OnCheckRunemailc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_ANTISPAM_H__35E469EB_96CD_48E5_A712_A370006E72D7__INCLUDED_)
