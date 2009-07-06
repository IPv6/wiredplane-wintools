#if !defined(AFX_DLG_ADDRBOOK_H__645484EC_72EB_4970_BDCC_87EDA6B7C2FB__INCLUDED_)
#define AFX_DLG_ADDRBOOK_H__645484EC_72EB_4970_BDCC_87EDA6B7C2FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Addrbook.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DLG_Addrbook dialog
#include "..\SmartWires\SmartWndSizer\SmartWndSizer.h"

class DLG_Addrbook : public CDialog
{
// Construction
public:
	void OnTrayAPopup();
	void SortList(int iColumn);
	void DelCurTab();
	void OnDelTab();
	void OnNewTab();
	void OnRenTab();
	CCriticalSection csView;
	HCURSOR hDragCursor;
	int iLastUnderCursorItem;
	void SetWindowIcon();
	void OnTrayOpenwindowFull(BOOL bShowTr, BOOL bShowWnd);
	BOOL IsAddrBookMessage(WPARAM wParam);
	LOGFONT logfont;
	CFont* font;
	void SetColorStyle();
	DWORD dwMouseTrackerThread;
	HANDLE MouseTracker;
	HANDLE MouseTracker2;
	MouseTrackerRuler MainTracker;
	MouseTrackerRuler MainTracker2;
	void StopMouseTracker();
	void StartMouseTracker();
	void PopupAdrBook();
	DWORD MainRulePos;
	int GetItemUnderCursor(int* iCol=NULL);
	int GetSelectedRow();
	BOOL OnRclick(BOOL bPerformDefAction=FALSE);
	void RefreshOnLineStatusIcon(int iPerson);
	void RefreshOnLineStatusIcons();
	virtual ~DLG_Addrbook();
	void OnMinimaze();
	void OpenNoteOnPerson(int iPerson);
	CSystemTray m_STray;
	int iStartDrug;
	void OnTrayStatusSet(int iStatus);
	void Refreshlist();
	void Initlist();
	DLG_Addrbook(CWnd* pParent = NULL);   // standard constructor
	CSmartWndSizer Sizer;
// Dialog Data
	//{{AFX_DATA(DLG_Addrbook)
	enum { IDD = IDD_ADDRESSBOOK };
	CTabCtrl	m_Tab;
	CBitmapStatic	m_BtOpt;
	CBitmapStatic	m_QMenu;
	CBitmapStatic	m_RefreshStatus;
	CBitmapStatic	m_BtMsgLog;
	CBitmapStatic	m_BtSendMessage;
	CBitmapStatic	m_BtSendMessage2;
	CBitmapStatic	m_BtNewPerson;
	CBitmapStatic	m_BtModPerson;
	CBitmapStatic	m_BtFindPerson;
	CBitmapStatic	m_BtDelPerson;
	CBitmapStatic	m_BtTabDel;
	CIListCtrl		m_PersonList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DLG_Addrbook)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(DLG_Addrbook)
	afx_msg void OnColumnclickTasklist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNewperson();
	afx_msg void OnNotemenuFZFolder();
	afx_msg void OnTrayReconnect();
	afx_msg void OnDeleteperson();
	virtual BOOL OnInitDialog();
	afx_msg void OnModperson();
	afx_msg void OnFindperson();
	afx_msg void OnDblclkPersonlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSendmes();
	afx_msg void OnSendmes2();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnTrayExitwindow();
	afx_msg void OnTrayExportAb();
	afx_msg void OnTrayImportAb();
	afx_msg void OnTrayFindUsers();
	afx_msg void OnTrayStatusaway();
	afx_msg void OnTrayStatusbusy();
	afx_msg void OnTrayStatusdnd();
	afx_msg void OnTrayStatusonline();
	afx_msg void OnTrayAutoanswering();
	afx_msg void OnTrayNewmessage();
	afx_msg void OnMsglog();
	afx_msg void OnRefreshstat();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTrayOpenwindow();
	afx_msg void OnTrayRefreshstatus();
	afx_msg void OnTrayDelperson();
	afx_msg void OnTrayMessagelog();
	afx_msg void OnTrayModperson();
	afx_msg void OnTrayNewmessageonperson();
	afx_msg void OnTrayNewperson();
	afx_msg void OnTrayUpdatepersonstatus();
	afx_msg void OnTrayOpencomputer();
	afx_msg void OnTrayUsermsglog();
	afx_msg void OnTrayOptions();
	afx_msg void OnQmenu();
	afx_msg void OnAdoptions();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_DLG_ADDRBOOK_H__645484EC_72EB_4970_BDCC_87EDA6B7C2FB__INCLUDED_)
