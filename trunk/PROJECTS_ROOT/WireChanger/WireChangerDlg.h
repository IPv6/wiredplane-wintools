//

#if !defined(AFX_COSSORGANIZATORDLG_H__F91233BB_3784_4194_997B_F1278D90A9AF__INCLUDED_)
#define AFX_COSSORGANIZATORDLG_H__F91233BB_3784_4194_997B_F1278D90A9AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_common.h"
#include "..\SmartWires\BitmapUtils\MenuToolTip.h"
////////////////////////////////////////////////////////////////////////////
// AppMainDlg dialog
#define UNDEFVALUE	-99
class AppMainDlg: public CDialog, public CCallbacks, public CCalendarCallback
{
// Construction
public:
	BOOL GetNotesFile(char* szOut,int size);
	BOOL GetRemDefText(char* szText,int size);
	BOOL GetRemDefWav(char* szSoundPath,int size);
	BOOL GetRemDefKey(char* szKey,int size);
	BOOL GetDayNote(SYSTEMTIME Time,char szDsc[2048]);
	BOOL CallEditDayNote(SYSTEMTIME Time,char szFG[64], char szBG[64]);
	BOOL CallContextDayNote(SYSTEMTIME Time, HWND hCalendar);
	CWPT* FindWPTUnderPoint(CPoint pt);
	void OnShowHideIcons(BOOL bValue);
	void OnSystrayDownloadWpt();
	static BOOL GetHTMLForClock(const char* szFile,CString& sPreContent, CString sTemplFile="import_ouclock.templ", BOOL bSmallize=0);
	void OnSystrayImportClock();
	void OnBuyNow();
	void OnEmail() ;
	void OnWebsite2();
	void OnWebsite3();
	BOOL ShowDescTooltip(const char* szText);
	CMenuToolTip ttDesktop;
	CDLG_Options* pOpenedTemplate;
	BOOL GenerateFullFeaturedWallpaper(DWORD dwIndex, HBITMAP& pBmpOut, CRect sizeOut, BOOL bEffects=0);
	BOOL GetRndWallpaperPath(int iType,char*);
	long GetCallerOption(long lOptionID);
	char* GetWebPage(const char* szUrl);
	BOOL FreeChar(char* szChar);
	BOOL GetBaseURI(char* szPath, const char* szForWhat);
	void WalkHTML(HELEMENT he);
	BOOL RenderHTML(HDC dc,RECT rtArea,const char* szBlock, BOOL bAddActiveZones);
	BOOL AddActiveZone(CRect* rtZone, const char* szAction);
	int OrderModifierForNextWallpaper;//-2: без смены номера, -1: пред, 0: случайная, 1: след
	BOOL OnTemplParams(CWnd* pParent, CString sWPT, CString sStartTopic);
	BOOL OnTemplParamsFile(CWnd* pParent, CString sStartTopic, const char* szTitle, const char* szFile, int isWPTIndex=-1);
	void SetRegInfo();
	void OnSystraySendasuggestion();
	void OnSystrayDelClock();
	void OnSystrayDownloadWp();
	void OnSystrayDisableWpt();
	void OnSystrayRestart();
	void OnSystrayTellAFriend();
	void OnSystrayDownloadClocks();
	void OnSystrayDownloadSkins();
	void OnSystrayEnliveWp();
	void StartAnimation(int iIconType=0);
	void StopAnimation(HBITMAP hBmpIcon=NULL);
	void SetNextWallPaper();
	void SetTheSameWallPaper();
	void SetNextWallPaperTimely();
	void OnSystrayExchangeWp();
	void OnSystrayClanchMoney();
	CDLG_WChanger* dlgWChanger;
	afx_msg LRESULT ONWM_SSAVE(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ONWM_THIS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ONWM_WIRENOTE(WPARAM wParam, LPARAM lParam);
	BOOL IsIconMustBeHided(BOOL bSkipVisText=FALSE);
	afx_msg LRESULT OnFireAlert(WPARAM wParam, LPARAM lParam);
	void PerformTrayAction(int iAction,BOOL bHotKey=FALSE);
	int SetTrayTooltip(BOOL bDisableAuto=FALSE);
	BOOL bAllLoaded;
	~AppMainDlg();
	void Finalize();
	BOOL bAnyPopupIsActive;
	CSmartWndSizer Sizer;
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	void RegisterMyHotKeys();
	void UnRegisterMyHotKeys();
	int iRunStatus;
	AppMainDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL ShowBaloon(const char* szText,const char* szTitle,DWORD dwTimeout=0);
// Dialog Data
	//{{AFX_DATA(AppMainDlg)
	enum {IDD = IDD_ABOUTBOX};
	CBitmapStatic m_Logo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AppMainDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	CSystemTray m_STray;
	HICON m_hIcon;
	afx_msg void OnSystrayToMain(int iWL);
	// Generated message map functions
	//{{AFX_MSG(AppMainDlg)
	virtual void PostNcDestroy();
	afx_msg void OnEntcode();
	afx_msg void OnWebsite();
	virtual void OnOK();
	virtual void OnSystrayNewNote();
	virtual void OnSystrayNoteOptions();
	virtual void OnSystrayNoteDisable();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSystrayOptionsKey();
	afx_msg void OnSystrayOptions();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnSystrayAbout();
	afx_msg void OnSystrayExit();
	afx_msg void OnSystrayOpen();
	afx_msg void OnSystrayOpen2();
	afx_msg void OnHotmenuTakescreenshot();
	afx_msg void OnSystrayHelp();
	afx_msg void OnSystrayRegister();
	afx_msg void OnOrderNextWallPaper();
	afx_msg void OnOrderPrevWallPaper();
	afx_msg void OnOrderRandWallPaper();
	afx_msg void OnSetChangePeriodically();
	afx_msg void OnSystrayBackUpList();
	afx_msg void OnSystrayRestoreList();
	afx_msg void OnShowHideIcons();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif 
