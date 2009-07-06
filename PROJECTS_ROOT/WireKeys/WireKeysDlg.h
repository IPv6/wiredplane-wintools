//

#if !defined(AFX_SORGANIZATORDLG_H__F91233BB_3784_4194_997B_F1278D90A9AF__INCLUDED_)
#define AFX_SORGANIZATORDLG_H__F91233BB_3784_4194_997B_F1278D90A9AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_common.h"
#include "DLGClip.h"
////////////////////////////////////////////////////////////////////////////
// AppMainDlg dialog
class AppMainDlg : public CDialog
{
public:
	CDLGClip* clipWnd;
	void ShowBaloonAtPosition(CString sText, CString sTitle, int iX, int iY, DWORD dwTimeout);
	CHyperlink wpLabsLink;
	BOOL ChangeItemIcon(const char* szItemPath);
	void KillWndProcess(HWND hUnderCur);
	CMenuToolTip ttMenu;
	CMenuToolTip mcMenu;
	int GetWKState();
	long lHotkeysState;
	void ScrollAction(int iAction);
	CFont* pIconFont;
	BOOL PopupAppWindow(DWORD dwProcID,BOOL bAction=0);
	CStringArray aExpandedFolderPaths;
	void SetRegInfo();
	void SetWindowIcon(int iIconType=0, BOOL bForce=0);
	void GenerateBindingsFile();
	BOOL RegisterMyHotkey(CIHotkey& oHotkey,DWORD dwCode,const char* szId,const char* szDsc, CString& sFailed, int& iCount);
	void AWndManagerMain(HWND forWnd, BOOL bTryToGetCaret=TRUE, int iMenu=0);
	void HideApplication(HWND hWin, DWORD dwHideType);
	void OnBossKey();
	void PerformOnShutdown();
	DWORD dwVolSplashTimer;
	DWORD dwWinAmpSplashTimer;
	CSplashWindow* VolumeSplWnd;
	CSplashWindow* WinAmpSplWnd;
	CSimpleMixer mixer;
	long ChangeVolumeControl(long lDx, int iMuter=-1);
	BOOL WinAmpControl(int iAction, BOOL bTryStart=1, BOOL bSplashEnabled=1, int iAdditionalPar=0);
	afx_msg void OnMinimizeActiveWindow();
	afx_msg void OnShutDownComputer();
	afx_msg void OnLockStation();
	afx_msg LRESULT ONWM_THIS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRunQRunApplication(WPARAM wParam, LPARAM lParam);
	LRESULT RunQRunApp(int iWndNum, BOOL bSwitchDesk, int iEntryNum=0);
	afx_msg LRESULT ONWM_WIRENOTE(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ONWM_EXTERN(WPARAM wParam, LPARAM lParam);
	BOOL IsIconMustBeHided();
	DWORD dwThreadId;

	void AttachToClipboard(BOOL bAr);
	//static void ConvertSelected(HKL& hCurrentLayout, CString& sText, CString& sCopy);
	void SwitchResolution(int iMode);
	afx_msg LRESULT OnFireAlert(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEDITTEXT(WPARAM wParam, LPARAM lParam);
	BOOL PerformTrayAction(int iAction,BOOL bHotKey=FALSE);
	BOOL bLastHalfer;
	BOOL bMainWindowInitialized;
	BOOL bAllLoaded;
	CString SetTrayTooltip(BOOL bUpdation=0);
	~AppMainDlg();
	void Finalize();
	BOOL bAnyPopupIsActive;
	CSmartWndSizer Sizer;
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	void RegisterMyHotKeys(BOOL bSilently=FALSE);
	void UnRegisterMyHotKeys(BOOL bNeedRecreate=TRUE);
	int iRunStatus;
	AppMainDlg(CWnd* pParent = NULL);	// standard constructor
	void TakeScreenshot();
	BOOL TuneUpItem(const char* szTopic);
	BOOL TuneUpItemX(const char* szTopic);
	afx_msg void SetKBLayout(HKL hWhat);
	// Dialog Data
	//{{AFX_DATA(AppMainDlg)
	enum { IDD = IDD_ABOUTBOX };
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
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	CSystemTray m_STray;

	// Generated message map functions
	//{{AFX_MSG(AppMainDlg)
	virtual void PostNcDestroy();
	afx_msg void OnDonate();
	afx_msg void OnEnterKey();
	afx_msg void OnHotmenuShowexepath();
	afx_msg void OnHotmenuAddToQRun();
	afx_msg void OnHotmenuAddToDesktop();
	afx_msg void OnHotmenuAddToQLaunch();
	afx_msg void OnHotmenuAddToStartMenu();
	afx_msg void OnHotmenuAddToAHide();
	afx_msg void OnHotmenuAddToUDFolder();
	afx_msg void OnSystraySetMute();
	afx_msg void OnSystraySetVol0();
	afx_msg void OnSystraySetVol30();
	afx_msg void OnSystraySetVol50();
	afx_msg void OnSystraySetVol70();
	afx_msg void OnSystraySetVol100();
	afx_msg void OnSystraySendasuggestion();
	afx_msg void OnSystrayDisplayProps();
	afx_msg void OnSystrayAddRemProgs();
	afx_msg void OnSystrayProcListPars();
	afx_msg void OnHotmenuTxtPars();
	afx_msg void OnSystrayScrShot();
	afx_msg void OnSystrayRestoreIcons();
	afx_msg void OnSystraySaveIcons();
	afx_msg void OnEntcode();
	afx_msg void OnWebsite();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSystrayOptions();
	afx_msg void OnSystrayOptions2();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void StartDefBrowser();
	afx_msg void StartDefEmail();
	afx_msg void OnSwapCD();
	afx_msg void ShowHotMenu();
	afx_msg void ShowKeyBindings();
	afx_msg void OnHotmenuBringinview();
	afx_msg void OnHotmenuSemitransparent();
	afx_msg void OnHotmenuExitmenu();
	afx_msg void OnHotmenuKillprocess();
	afx_msg void OnHotmenuKillprocessUnderCur();
	afx_msg void OnHotmenuTransparency100();
	afx_msg void OnHotmenuTransparency20();
	afx_msg void OnHotmenuTransparency40();
	afx_msg void OnHotmenuTransparency60();
	afx_msg void OnHotmenuTransparency80();
	afx_msg void OnHotmenuSwitchontop();
	afx_msg void OnHotmenuPopupmainwindow();
	afx_msg void OnHotmenuHidetotrayDetached();
	afx_msg void OnHotmenuHidetotray();
	afx_msg void OnHotmenuUnHideFromtray();
	afx_msg void OnHotmenuFreememory();
	afx_msg void OnHotmenuConvertselected();
	afx_msg void OnHotmenuConvertselectedLW();
	afx_msg void OnHotmenuUppercase();
	afx_msg void OnHotmenuLowercase();
	afx_msg void OnHotmenuSwitchcase();
	afx_msg void OnHotmenuCountcharacters();
	afx_msg void OnSystrayAbout();
	afx_msg void OnReattachClips();
	afx_msg void OnSystrayExit();
	afx_msg void OnSystrayOpen();
	afx_msg void OnSystrayShowAllApps();
	afx_msg void OnSystrayBossPanic();
	afx_msg void OnHotmenuTakescreenshot();
	afx_msg void OnSystrayHelp();
	afx_msg void OnHotmenuRunascommand();
	afx_msg void OnHotmenuCalculate();
	afx_msg void OnHotmenuHidecompletely();
	afx_msg void OnHotmenuRunscreensaver();
	afx_msg void OnHotmenuShutdown();
	afx_msg void OnHotmenuRestart();
	afx_msg void OnHotmenuSuspend();
	afx_msg void OnHotmenuScreensaverenabled();
	afx_msg void OnSystrayAddqrunapplication();
	afx_msg void OnSystrayAddqrunappwnd();
	afx_msg void OnSystrayAddqrunmacros();
	afx_msg void OnSystrayReqMacro();
	afx_msg void OnHotmenuClearclips();
	afx_msg void OnSystrayPlgOptions();
	afx_msg void OnSystrayPlgDownload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
int AppendQRunSingleToMenu(CMenu* pMenuRoot, CMenu* pMenu, DWORD dwMacrosMask, int* iCountTotal=NULL, const char* szCategID=NULL);
#endif 
