// WPOSSOrganizatorDlg.h : header file
//

#if !defined(AFX_WPOSSORGANIZATORDLG_H__F91233BB_3784_4194_997B_F1278D90A9AF__INCLUDED_)
#define AFX_WPOSSORGANIZATORDLG_H__F91233BB_3784_4194_997B_F1278D90A9AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_common.h"

////////////////////////////////////////////////////////////////////////////
// AppMainDlg dialog
#define TYPEMASK_TODO	1
#define TYPEMASK_REMD	2
class AppMainDlg : public CDialog
{
// Construction
public:
	void OnSystrayLinkCh();
	void SetLPText(CString sText);
	void OnTodoSaveToFile();
	BOOL SwitchToFEdit();
	int iIgnoreChangerFEdit;
	CString sLastFEditID;
	void ShowBaloon(const char* szTitle, const char* szText);
	DWORD dwLastEditedLastType;
	void DragItem(CItem* iStartItem,CItem* iStopItem);
	BOOL IsMainMessage(WPARAM wParam);
	void SetWindowIcon();
	afx_msg void LinkFromClipboard();
	BOOL LinkFromClipboardX();
	afx_msg void OnCopyLinkToClip();
	afx_msg void OnEditLink();
	afx_msg void OnSendItemAsMessage();
	afx_msg void OnRemdsOptions();
	afx_msg void OnShareItem(int iSec);
	afx_msg LRESULT OnCreateNewToDo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ONWM_WIREKEYS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ONWM_WIRECHANGER(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ONWM_THIS(WPARAM wParam, LPARAM lParam);
	void Renamefolder(HIROW hRow);
	afx_msg void AWndManager();
	void OpenMsgLogOnNick(const char* szNick);
	CFont* font;
	LOGFONT logfont;
	void SetColorStyle();
	static void ConvertSelected(HKL& hCurrentLayout, CString& sText, CString& sCopy);
	afx_msg LRESULT OnFireAlert(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFireOptions(WPARAM wParam, LPARAM lParam);
	int MainRulePos;
	void Modifytodo(CItem* item);
	int CreateMenuTree(CMenu& menu,const char* szFirstID, BOOL bFoldersOnly=0, DWORD dwDepth=100, DWORD dwTypeMask=0);
	void PerformTrayAction(int iAction,BOOL bHotKey=FALSE);
	BOOL bLastHalfer;
	BOOL bMainWindowInitialized;
	BOOL bAllLoaded;
	int SetTrayTooltip(int* iLastMessageNum=NULL);
	void StopAnimation(int ForPerson=-1);
	void StartAnimation(int ForPerson=-1);
	void UpdateBaloonTooltip(int iIncMessages, int iLastMessageNum, BOOL bShowTooltip);
	void SetWindowPinState(BOOL bShow=TRUE);
	void StopMouseTracker();
	void StartMouseTracker();
	BOOL bPinnedUp;
	BOOL LoadAll();
	void DeleteFileLink();
	void TrackFileLinkPopupMenu(CItem *item);
	void ActivateFileLink(CItem* item=NULL);
	CItemFileLink* AddFileLink(const char* szFilePath, const char* szPath=NULL,const char* szTitle=0);
	void NewTaskPath(const char* szPath=NULL, int i=0);
	void NewReminderPath(const char* szPath=NULL);
	void NewNotePath(const char* szPath=NULL, BOOL bFromCursor=TRUE);
	void OperationUnderCursor(int iOperation, CItem* item=NULL);
	~AppMainDlg();
	CString sLastSelectedItemPath;
	void TraceItemList(const char* szTitle);
	afx_msg LRESULT OnNeedOrganizatorReaction(WPARAM wParam, LPARAM lParam);
	DLG_Addrbook* dlgAddBook;
	HIROW hRowForDrag;
	int nRowForDragDir;
	BOOL CheckItemInFilters(CItem* item);
	void ReattachAccordingFilter();
	void SortList(int iColumn);
	void TrackPriorityPopupMenu(CItem* item);
	void SetDefaultsListParams(BOOL bbOnly=0);
	void SaveLinks();
	void LoadLinks();
	void SaveFolders();
	void LoadFolders();
	CItem* GetItemByListIndex(int iIndex);
	void DragItem(DWORD dwItemOnStartDrag,DWORD dwItemOnStopDrag);
	int GetItemUnderCursor(int *iCol=NULL, HIROW* hRow=0, BOOL* bLabel=0);
	DWORD dwDragLock;
	DWORD dwItemOnStartDrag;
	DWORD dwItemOnStopDrag;
	DWORD dwItemOnStopDragLast;
	void ModifyReminder(const char* szID);
	void OnMainTreeListComEx(NMHDR * pNotifyStruct, LRESULT * result);
	void OnMainTreeListSel(NMHDR * pNotifyStruct, LRESULT * result);
	void OnEndEditMainTreeList(NMHDR * pNotifyStruct, LRESULT * result);
	void OnEndEditMainTreeListC(NMHDR * pNotifyStruct, LRESULT * result);
	void OnEndEditMainTreeListI(NMHDR * pNotifyStruct, LRESULT * result);
	void OnEndEditMainTreeListM(NMHDR * pNotifyStruct, LRESULT * result, int iType);
	void OnCustomDialog(NMHDR * pNotifyStruct, LRESULT * result);
	void UpdateFedit(CItem* item);
	void UpdateFeditIfNeeded(CItem* itemC);
	CItem* GetSelectedRow();
	void TrackFolderPopupMenu(CItem* item);
	void TrackReminderPopupMenu(CItem* item);
	void TrackTaskPopupMenu(CItem* item,DWORD dwLims=0);
	void OnRclickTasklist();
	void TrackItemMenu(CItem* item);
	void RedrawItem(CItem* pItem);
	BOOL InsertOrRefreshItem(CItem* pItem, BOOL bStopOnNoParent=FALSE);
	void Finalize();
	void RefreshTaskList();
	MouseTrackerRuler MainTracker;
	MouseTrackerRuler MainTracker2;
	HANDLE MouseTracker;
	HANDLE MouseTracker2;
	CString sActiveAppTitle;
	long iTimerCheckCounter;
	DWORD dwAutoSaveCounter;
	long iUpdateTreeCounter;
	long iScrSaverCheckCounter;
	int iCurrentOnLineMode;
	BOOL bAnyPopupIsActive;
	int iLastBoldItem;
	int iCurrentItem;
	int iCurrentItemLast;
	DWORD dwMouseTrackerThread;
	CSmartWndSizer Sizer;
	void OnProperties();
	void OnFilelinkmenuRename(HIROW hSelIRow,int iSelIItem);
	afx_msg LRESULT OnCreateNewNote(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	void RegisterMyHotKeys();
	void UnRegisterMyHotKeys();
	void ConstructNewNote(CNoteSettings* pStartParams);
	void InitTaskList();
	int iRunStatus;
	AppMainDlg(CWnd* pParent = NULL);	// standard constructor
	HWND hLastActiveControl;
// Dialog Data
	//{{AFX_DATA(AppMainDlg)
	enum { IDD = IDD_WPOSSORGANIZATOR_DIALOG };
	CBitmapStatic	m_BtFind;
	CBitmapStatic	m_BtNewTd;
	CBitmapStatic	m_BtOnTop;
	CBitmapStatic	m_BtSaveAll;
	CBitmapStatic	m_BtOpt;
	CBitmapStatic	m_BtEform;
	CBitmapStatic	m_BtNewRem;
	CBitmapStatic	m_BtNewNote;
	CBitmapStatic	m_BtAdrBook;
	CBitmapStatic	m_BtNewLink;
	CBitmapStatic	m_BtNewFolder;
	CBitmapStatic	m_BtEditItem;
	CBitmapStatic	m_BtDelItem;
	CIListCtrl		m_TaskList;
	CNoteEditCtrl	m_FEdit;
	CSliderCtrl		m_Slider;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AppMainDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	CSystemTray m_STray;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(AppMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSystrayReport();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMinimaze();
	afx_msg void OnSystrayToDoAddOptions();
	afx_msg void OnSystrayExit();
	afx_msg void OnSystrayAntiSpamOptions();
	afx_msg void OnSystrayOpen();
	afx_msg void OnDestroy();
	afx_msg void OnSystrayNewtask();
	afx_msg void OnSystrayLink();
	afx_msg void OnDeleteTask();
	afx_msg void OnNotemenuFZFolder();
	afx_msg void OnSystrayCheckAllAcounts();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSystrayNewnote();
	afx_msg void OnSystrayOptions();
	afx_msg void OnEform();
	afx_msg void OnChangeAutopopupOrg();
	afx_msg void OnSystrayAddressbook();
	afx_msg void OnSystrayShowallnotes();
	afx_msg void OnSystrayShowNextNote();
	afx_msg void OnSystrayHideallnotes();
	afx_msg void OnSystrayDefaction();
	afx_msg void OnSystrayNewreminder();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTodoNewtodo();
	afx_msg void OnTodoSetascurrent();
	afx_msg void OnTodoModifytodo();
	afx_msg void OnTodoMarkasundone();
	afx_msg void OnTodoMarkasdone();
	afx_msg void OnTodoTodoreport();
	afx_msg void OnSystrayNewmessage();
	afx_msg void OnClose();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnRemindermenuActivatereminder();
	afx_msg void OnRemindermenuDeactivatereminder();
	afx_msg void OnRemindermenuDeletereminder();
	afx_msg void OnRemindermenuModifyreminder();
	afx_msg void OnFolderCreatenew();
	afx_msg void OnFolderDelete();
	afx_msg void OnPriorityHigh();
	afx_msg void OnPriorityHighest();
	afx_msg void OnPriorityLow();
	afx_msg void OnPriorityLowerest();
	afx_msg void OnPriorityNormal();
	afx_msg void OnBnDel();
	afx_msg void OnBnEdit();
	afx_msg void OnNewBnFolder();
	afx_msg void OnNewBnLink();
	afx_msg void OnColumnclickTasklist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewNote();
	afx_msg void OnNewTodo();
	afx_msg void OnNewReminder();
	afx_msg void OnNewMessage();
	afx_msg void OnAddrbook();
	afx_msg void OnOptionsBt();
	afx_msg void OnSaveallBt();
	afx_msg void OnFolderAddnote();
	afx_msg void OnFolderAddreminder();
	afx_msg void OnFolderAddtask();
	afx_msg void OnFilelinkmenuActivate();
	afx_msg void OnFilelinkmenuDel();
	afx_msg void OnFilelinkmenuRename();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnTodoHideshowdone();
	afx_msg void OnRemindermenuShowhideinactive();
	afx_msg void OnWndontop();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTodoSetasnotcurrent();
	afx_msg void OnHotmenuPopupmainwindow();
	afx_msg void OnFolderSetastrayfolder();
	afx_msg void OnWchangerBt();
	afx_msg void OnSystraySwitchmenu();
	afx_msg void OnFolderRenamefolder();
	afx_msg void OnSystrayAbout();
	afx_msg void OnSystrayRegister();
	afx_msg void OnTodoSettings();
	afx_msg void OnTodoRemindabout();
	afx_msg void OnSystrayWchanger();
	afx_msg void OnSystrayIncEmails();
	afx_msg void OnSystrayCBillb();
	afx_msg void OnSystraySendasuggestion();
	afx_msg void OnSystrayDonate();
	afx_msg void OnNotemenuNewstyle();
	afx_msg void OnFindBt();
	afx_msg void OnRclickRichedit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeRichedit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CToolTipCtrl m_tooltip;
public:
	afx_msg void OnSystrayHelp();
	afx_msg void OnSystrayOpenwirekeys();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_WPOSSORGANIZATORDLG_H__F91233BB_3784_4194_997B_F1278D90A9AF__INCLUDED_)
