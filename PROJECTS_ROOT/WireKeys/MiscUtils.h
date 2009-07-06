#if !defined(AFX_UTILS_H__7CD51405_6EC9_48A4_BBD1_CAFAD8F26FC4__INCLUDED_)
#define AFX_UTILS_H__7CD51405_6EC9_48A4_BBD1_CAFAD8F26FC4__INCLUDED_

#include <tlhelp32.h>
#include "_common.h"

//
void deleteFont(CFont* font);
void deleteBrush(CBrush* br);
void GetWindowTitleAndData(HWND pCurActivWnd,CString& sNewTitle,CRect& ActiveWndRect);
BOOL GetActiveWindowTitle(CString& sNewTitle, CRect& ActiveWndRect, HWND* pActivWnd=NULL, BOOL bUnderCursor=FALSE, BOOL bCheckInvis=TRUE);
DWORD WINAPI MouseTrackerThread(LPVOID lpdwstatus);
BOOL PtInWindowRect(POINT pt, CWnd* wnd);
BOOL MouseInWndRect(CWnd* wnd);
int GetStatusIcon(int iPerson);
long CtrlWidth(CWnd* wnd, UINT CtrlID);
long CtrlHeight(CWnd* wnd, UINT CtrlID);
void AsyncPlaySound(const char* szSoundName="");
CString getLinkedID(const char* szLinkID);
BOOL ConvertSelected(HKL& hFrom, HKL& hTo, CString& sText, CString& sCopy);
BOOL PrepareClipProcesses(BOOL& bUseWMCOPY, HWND hWin=NULL);
BOOL FinishClipProcesses(HWND hWin=NULL);
void PerformKBInit(HWND hTargetWnd);
CString GetSelectedTextInFocusWnd(BOOL bDoNotTolerateFailure=FALSE);
BOOL PutSelectedTextInFocusWnd(CString sText, HKL hSendLayout=0, BOOL lLeaveInClipOnPaste=0);
void WaitWhileAllKeysAreFree(CIHotkey hk=CIHotkey(0,0));
void EnableDebugPriv();
void RefreshProcessListIfNeeded();
void GetWndIcon(HWND& hSlayerWnd,HICON& hIcon, HICON* hBig=NULL);
void GetProcessInfo(DWORD dwProcId, CProcInfo*& pInfo);
CString GetProcessComLine(HWND hWin);
BOOL GetProcessComLine(DWORD dwProcID, CString& sCmdLine);
BOOL GetProcessStartupInfo(DWORD dwProcID,STARTUPINFO* si, CString* sCmdLine);
int KillProcess(DWORD pid,const char* szFileTitle=NULL);
void GetProcessesIcons();
CString GetProcIconIndex(CProcInfo& info);
CString GetExeIconIndex(const char* szExePath, BOOL bRawGet=FALSE,BOOL bAllowRecheck=TRUE, BOOL bUseSH=FALSE);
CString getActionHotKeyDsc(CIHotkey& hk,const char* szBefore="\t", const char* szAfter="", const char* szIfNull=NULL);
CString getActionHotKeyDsc(int iActionNum,const char* szBefore="\t", const char* szAfter="", const char* szIfNull=NULL);
CString getClipHotKeyDsc(int iClip,BOOL bCopy);
CString GetSizeStrFrom(DWORD dwSize, DWORD dwFrom);
void TimetToFileTime( time_t t, LPFILETIME pft );
BOOL GetFileDateTime(const char* szFile, FILETIME* tOut);
//
int AskYesNo(const char* szText, const char* szTitle, long* bAnswerHolder=NULL, CWnd* pParent=NULL);
void AddError(const char* szError, BOOL bSystem=FALSE);
void SmartWarning(const char* szText, const char* szTitle, long* bAnswerHolder, DWORD dwTimer=0);
BOOL SetCurrentDesktop();
CString GetCurrentDesktop();
int GetCurrentDesktopNumber();
BOOL RestoreDesktopIcons();
BOOL SaveDesktopIcons();
CString GetStringHash(const char* szName);
void OpenCD(TCHAR cCDDiskLetter, BOOL bWithOSD=TRUE);
void CloseCD(TCHAR cCDDiskLetter, BOOL bWithOSD=TRUE);
void EjectCDIfDiskPresent(TCHAR c);
int UpdateTrialityStatus(CString sUser, CString sKeys);
void AddSlashToPath(CString &sPath);
typedef struct MytagGUITHREADINFO {
    DWORD cbSize;
    DWORD flags;
    HWND hwndActive;
    HWND hwndFocus;
    HWND hwndCapture;
    HWND hwndMenuOwner;
    HWND hwndMoveSize;
    HWND hwndCaret;
    RECT rcCaret;
} MYGUITHREADINFO;//=GUITHREADINFO
typedef BOOL (WINAPI *_GetGUIThreadInfo)(DWORD idThread,MYGUITHREADINFO* lpgui);
BOOL GetGUIThreadInfoMy(DWORD dwCurWinThreadID, MYGUITHREADINFO* gui);
class SetWaitTrayIcon
{
	HICON hPrev;
	CSystemTray *m_pTray;
	HCURSOR hCur;
public:
	SetWaitTrayIcon(CSystemTray* pTray);
	~SetWaitTrayIcon();
};

DWORD WINAPI PasteClipText(LPVOID pData);
BOOL ParsedAsArithmeticalExpr(CString& sExpr);
BOOL CalculateExpression_Cmd(CString& sExpr);
BOOL CalculateExpression_Ole(CString& sExpr, CString sLang="JScript", BOOL bInSilence=FALSE, int iEntryPoint=0, HANDLE h=0);
BOOL ClipAddTextToPinned(const char* szText, const char* szHistName,BOOL bToPinned=FALSE, const char* szDsc=NULL, const char* szApp=NULL);
BOOL SetTextToClip(int iClip, CString& sText, BOOL bCheckAllHistory=FALSE);
void SendCtrlV(BOOL m_bUseWMCOPY, HWND hTargetWnd);
DWORD WINAPI TransmitTextToTarget(LPVOID pData);
struct BaloonInfo
{
	CString sText;
	CString sTitle;
	DWORD dwTimeout;
	BOOL bBaloonOnly;
};
DWORD WINAPI ShowBaloon_InThread(LPVOID pData);
void ShowBaloon(const char* szText, const char* szTitle, DWORD dwTimeout=9000, BOOL bBaloonOnly=FALSE);
void SubstMyVariablesWK(CString& sText, BOOL bJScriptSafe=FALSE);
BOOL ReparseWinampTitle(CString& sTitle, HWND hWAWnd, BOOL bTotOnly);
void FillSplashParams(const char* szTitle, CSplashParams& par);
DWORD WINAPI ShowWTitleSplash(LPVOID pData);
BOOL WaitWhileAllKeysAreFreeX(BOOL bCheckExplicit=FALSE);
void ExecuteMacroByString(CString sExe);
#define IsThreadDoubleCalled(ACTION) {\
	static long lLock=0;static long lHitCount=0;\
	if(lLock==0){lHitCount=0;};\
	if(lLock==1){lHitCount=1;};\
	if(lLock>0){return 0;}\
	SimpleTracker tr(lLock);\
	WaitWhileAllKeysAreFreeX();\
	ACTION=lHitCount;\
};
HWND GetLastFocusWindowByHook(HWND hWin);
void SetMainWindowForeground(HWND hWin, HWND hWindowToSetForeground, HWND* hWindowToSetFocus);
BOOL GetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut, size_t dwOutSize);
BOOL SetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut);
bool UTF2U(const char* src,CString& out);
BOOL WINAPI GetFileDescription(const char* szFileName, char szDescOut[256]);
#endif // !defined(AFX_UTILS_H__7CD51405_6EC9_48A4_BBD1_CAFAD8F26FC4__INCLUDED_)
