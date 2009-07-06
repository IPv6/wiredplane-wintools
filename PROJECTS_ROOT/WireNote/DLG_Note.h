#if !defined(AFX_DLG_NOTE_H__5B0C41EA_B5AE_4002_B1CD_386404FB5CD0__INCLUDED_)
#define AFX_DLG_NOTE_H__5B0C41EA_B5AE_4002_B1CD_386404FB5CD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Note.h : header file
//
#include "..\SmartWires\SmartWndSizer\SmartWndSizer.h"
#include "resource.h"
#include "ItemModif.h"
#include "NoteEditCtrl.h"
#include "Reminders.h"
#include "MiscUtils.h"
#include "_common.h"


/////////////////////////////////////////////////////////////////////////////
// CDLG_Note dialog
class CDLG_Note;
class Task;
class CNoteSettings: public CActiItem
{
public:
	CIHotkey hHotkey;
	void SetColorCode(long iCode);
	CString sAttachment;
	CString sItemIDToAttachAfterStart;
	BOOL bUndeletable;
	BOOL bHidden;
	int iIconMode;
	BOOL b_systemCopySID;
	BOOL bDeleteAfterSend;
	BOOL bIncomingMessage;// 0 - не входящее, 1 - входящее прочитанное, 2- входящее непрочитанное
	BOOL bIncMsgOpenType;// Тип открытия заметки при входящем сообщении
	BOOL bSelectTextOnStart;
	BOOL bNeedFocusOnStart;
	BOOL bWordWrap;
	CString sText;
	BOOL bSaveOnExit;
	BOOL bTopMost;
	BOOL bFixedSize;
	BOOL bMouseTrans;
	BOOL bAutoHide;
	BOOL bDisableTransparency;
	int iViewMode;
	BOOL AutoResize;
	BOOL ReplyButton;
	CString sSenderPerson;
	CString sReplyAddressName;
	long iYPos;
	long iXPos;
	long iYSize;
	long iXSize;
	virtual void ActivateChanges(const char* szTitle=NULL, DWORD* pdwPrir=NULL, DWORD* pdwState=NULL);
	CNoteSettings& operator=(CNoteSettings& CopyObj);
	CNoteSettings(CNoteSettings& CopyObj, const char* szTitlePattern=NULL, const char* szBodyPattern=NULL);
	CNoteSettings();
	~CNoteSettings();
	CString GetItemDsc();
	CString GetTextContent();
	CString GetFindText(){return GetTitle()+"|"+sText;};
	BOOL SaveToFile(const char* szFile=0);
	CString GetFileName(BOOL bTrash=FALSE);
	BOOL LoadFromFile(const char* szFileName);
	long lClipSpy;
	BOOL DelFile();
	BOOL Saveable();
	static CString GetMessageTitle(const char* szAddr, BOOL bFrom, const char* szFromR=NULL);
	BOOL ShareItem(BOOL bNewSharingData, int iSectionNum);
};

#include "DLG_InfoWindow.h"
class CDLG_NoteSettings;
#define POSPREFIX	DEF_OPENTYPE
#define POS1 "2"
#define POS2 "3"
#define POS3 "1"
#define ICONPREFIX	"icon "
#define SMILE1 ":-)"
#define SMILE2 ":))"
#define SMILE3 ":-("
#define SMILE4 ":(("
#define SMILE5 "?-)"
#define SMILE6 "!-)"

class CDLG_Note : public CDialog, public CLinkHandler
{
// Construction
public:
	BOOL bNoteTitleChanged;
	BOOL HandleLink(const char* sz);
	DWORD dwASTimer;
	DWORD dwASTimerStart;
	BOOL bStatusCacheUpdater;
	void OnGSearch();
	void OnExeAsScr();
	BOOL bWasSemi;
	void GetTitleToSets();
	void RegHotkey();
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	DWORD dwHKNum;
	static DWORD dwHKNumTotal;
	void OnNotemenuAssignHK();
	long lClipSpy;
	long iSendLock;
	long iClipCount;
	HWND hNextClipboardChainWnd;
	void SetNoteInfo(DWORD* dwColor, const char* szText=NULL);
	void InitAutoSend(BOOL bSimpleStop);
	void SetQuote(int iCode);
	BOOL IsAutoTransparencyEnabled();
	DWORD dwLastChangeTime;
	afx_msg void OnNotemenuSavenote();
	void ActivateEmotionsMenu();
	void AddAttachmentsMenu(CMenu& pAttachments, BOOL bFullView=FALSE);
	void AddEmotionsMenu(CMenu& pSmiles);
	void AddOptionsSubMenu(CMenu* pOptions);
	void SetColorCode(long iCode);
	CString GetRecipient();
	void PutRecipient(const char* szRecipient);
	void PutRecipientPerson(int iRecipient);
	int GetRecipientPerson();
	afx_msg void OnDeleteNote();
	afx_msg void OnReallyDeleteNote();
	afx_msg LRESULT DeleteNote(WPARAM wParam=0, LPARAM lParam=0);
	afx_msg LRESULT UpdateRecipientList(WPARAM wParam=0, LPARAM lParam=0);
	long bAnyDialogInProcess;
	void RefreshAttachedFileList();
	CStringArray attachmentFileNames;
	BOOL GetAttachByIndex(int iIndex, CString& sFileName, DWORD& dwSize, CString* pData=NULL, CString* pAttribs=NULL);
	BOOL AddFileToAttachment(CString& szFileName, const char* szAttribs="", const char* szSendWithName=NULL);
	BOOL AddItemToAttachment(const char* szID);
	BOOL SaveAttachToFile(int iAttachIndex, const char* szAttribs=NULL);
	BOOL RemoveFileFromAttach(const char* szFileName);
	BOOL ExtractDataFromAttachment();
	CDLG_NoteSettings* pNoteDlg;
	void UpdateTooltips();
	void SetTitle(const char* szTitle=NULL);
	BOOL bStartHided;
	BOOL bTitleChanged;
	void GetStartPos2(long& X, long& Y, long iWidth, long iHeight,int iDirX,int iDirY);
	void GetStartPos1(long& X, long& Y, long iWidth, long iHeight);
	void ActivatePinupPopupMenu();
	void ActivatePinupPopupMenuX(BOOL bAtCursor=1);
	CString GetNoteIcon(BOOL bBig=TRUE,int* iPrevaleIcon=0);
	void PinToWndTitle(const char* szTitle);
	BOOL SwitchOntopState(BOOL bCheckAsMouse=TRUE);
	void PinToUnderlaingWnd();
	void BackToNormalView();
	void RefreshOnTopMenu();
	BOOL OnNoteOntop();
	void SaveNoteToFile();
	void SetNoteText(BOOL bDoNotCheckText=FALSE);
	void MarkAsRead(BOOL bRefreshInfo=FALSE);
	BOOL IsMessage();
	CSystemTray m_STray;
	int iWindowLong;
	BOOL bDoNotShowNote;
	void PopupMenu(BOOL bExtended=FALSE);
	void RemindThisNote();
	int MainRulePos;
	BOOL CanThisNoteBeShown();
	BOOL TestTitleMatchCondition(const char* szTitle=NULL);
	HANDLE MouseTracker;
	HANDLE MouseTracker2;
	DWORD dwMouseTrackerThread;
	MouseTrackerRuler NoteTracker;
	MouseTrackerRuler NoteTracker2;
	// Для драггинга
	POINT fromPoint;
	BOOL isMove;
	CFont* font;
	LOGFONT logfont;
	int iRModeSwitcher;
	CSmartWndSizer Sizer;
	CMenu* m_pMenu;
	void RefreshMenu(BOOL bExtended=FALSE);
	CNoteSettings Sets;
	void RememberWindowPos();
	void ApplyAllLayouts(BOOL bShow=TRUE);
	void InitSizeAndPos();
	void ActivateThisNote();
	afx_msg void GetNoteText();
	void SetNoteParams();
	void ActivateNoteParams(BOOL bShow=TRUE, BOOL bWithAnimation=FALSE, BOOL bFully=0);
	BOOL ActivateWindow(BOOL bTop=-1, BOOL bShow=-1);
	int iNoteWndStatus;
	afx_msg void OnNotemenuSnaptocontent_old();
	void AppendSmiles(CMenu& Menu);
	CDLG_Note(CNoteSettings* pNoteParams=NULL);   // standard constructor
	~CDLG_Note();
	CBrush* m_hBr;
	CBrush* m_hBrTitle;
	CBrush* m_hBrInfo;
	DWORD dwInfoBgColor;
	DWORD m_dwBrTitle;

// Dialog Data
	//{{AFX_DATA(CDLG_Note)
	enum { IDD = IDD_NOTE };
	CBitmapStatic m_RecentList;
	CEdit	m_Title;
	CBitmapStatic m_NoteIcon;
	CBitmapStatic m_ABook;
	CBitmapStatic m_Send;
	CBitmapStatic m_PrintNote;
	CBitmapStatic m_RemindLater;
	CBitmapStatic m_SaveToFile;
	CBitmapStatic m_DestroyNote;
	CBitmapStatic m_Reply;
	CBitmapStatic m_OptionsDialog;
	CEdit m_Recipients;
	CNoteEditCtrl m_EditField;
	CBitmapStatic m_Ontop;
	CBitmapStatic m_ColorRed;
	CBitmapStatic m_Option;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Note)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CDLG_Note)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNoteColorChange();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNotemenuSwitchTransp();
	afx_msg void OnNotemenuMsgFilter();
	afx_msg void OnNotemenuNoteSettings();
	afx_msg void OnNotemenuMsgSettings();
	afx_msg void OnNotemenuNetSettings();
	afx_msg void OnNoteOption();
	afx_msg void OnNoteOptdialog();
	afx_msg void OnNoteQuote();
	afx_msg void OnAddtoadrbook();
	afx_msg void OnNoteCopytocl();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNotemenuView();
	afx_msg void OnNotemenuExtendedview();
	afx_msg void OnNotemenuSmallview();
	afx_msg void OnNotemenuHidenote();
	afx_msg void OnDestroy();
	afx_msg void OnSendmessage();
	afx_msg void OnNotemenuAutohide();
	afx_msg void OnNotemenuClipSpy();
	afx_msg void OnItemShare(int iSection);
	afx_msg void OnNotemenuPreventfromedit();
	afx_msg void OnNotemenuRemindin15min();
	afx_msg void OnNotemenuRemindafter3h();
	afx_msg void OnNotemenuRemindin1h();
	afx_msg void OnNotemenuRemindin30min();
	afx_msg void OnNotemenuRemindtomorrow();
	afx_msg void OnNotemenuBringallontop();
	afx_msg void OnNotemenuSnaptocontent();
	afx_msg void OnNotemenuA2W();
	afx_msg void OnNotemenuRemembAsDef();
	afx_msg void OnNotemenuShownote();
	afx_msg void OnNotemenuWordwrap();
	afx_msg void OnKillfocusComboboxex();
	afx_msg void OnSelchangeComboboxex();
	afx_msg void OnInfowndDelete();
	afx_msg void OnInfowndOpen();
	afx_msg void OnNotemenuIconview();
	afx_msg void OnNotemenuFixedsize();
	afx_msg void OnNotemenuMTrans();
	afx_msg void OnNotemenuSmiley1();
	afx_msg void OnNotemenuSmiley2();
	afx_msg void OnNotemenuSmiley3();
	afx_msg void OnNotemenuSmiley4();
	afx_msg void OnNotemenuSmiley5();
	afx_msg void OnNotemenuSmiley6();
	afx_msg void OnNotemenuPos1();
	afx_msg void OnNotemenuPos2();
	afx_msg void OnNotemenuPos3();
	afx_msg void OnNotemenuSmileyOff();
	afx_msg void OnNotemenuAttachfile();
	afx_msg void OnNotemenuFZFolder();
	afx_msg void OnNotemenuMsglog();
	afx_msg void OnNotemenuNewStyle();
	afx_msg void OnNotemenuCopy();
	afx_msg void OnNotemenuCut();
	afx_msg void OnNotemenuPaste();
	afx_msg void OnNotemenuLoadfromfile();
	afx_msg void OnNotemenuSavetofile();
	afx_msg void OnAbook();
	afx_msg void OnNoteOptmenu();
	afx_msg void OnKillfocusTitle();
	afx_msg void OnChangedTitle();
	afx_msg void OnNotemenuPrint();
	afx_msg void OnNotemenuSendasemail();
	afx_msg void OnRightclickedNoteRed();
	afx_msg void OnChangeComboboxex();
	afx_msg BOOL OnEraseBkgnd( CDC * );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_NOTE_H__5B0C41EA_B5AE_4002_B1CD_386404FB5CD0__INCLUDED_)
