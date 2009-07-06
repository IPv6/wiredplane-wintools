#if !defined(AFX_UTILS_H__7CD51405_6EC9_48A4_BBD1_CAFAD8F26FC4__INCLUDED_)
#define AFX_UTILS_H__7CD51405_6EC9_48A4_BBD1_CAFAD8F26FC4__INCLUDED_

#include <tlhelp32.h>
#include "_common.h"
typedef BOOL (CALLBACK *FARPROCL) (HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
typedef BOOL (CALLBACK *FARPROCW) (HWND hwnd, BOOL bExpand);
typedef BOOL (CALLBACK *FARPROCA) (HWND hwnd, DWORD, DWORD);
typedef BOOL (CALLBACK *FARPROCLOCKFOREG) (BOOL bLock);
#if _WIN32_WINNT == 0x0400
#define AW_HOR_POSITIVE             0x00000001
#define AW_HOR_NEGATIVE             0x00000002
#define AW_VER_POSITIVE             0x00000004
#define AW_VER_NEGATIVE             0x00000008
#define AW_CENTER                   0x00000010
#define AW_HIDE                     0x00010000
#define AW_ACTIVATE                 0x00020000
#define AW_SLIDE                    0x00040000
#define AW_BLEND                    0x00080000
#endif
//
struct MouseTrackerRuler;
typedef void	(FAR WINAPI *OnMouseOut)(MouseTrackerRuler*);
struct MouseTrackerRuler
{
	OnMouseOut MouseOutProc;
	CWnd*	pWnd;
	DWORD	dwTaskbarFriendly;
	BOOL	bStopWatching;
};
//
int FindColorSchemNumByName(const char* szName);
void FillComboWithColorschemes(CComboBox* ID,int iStart=0);
void deleteFont(CFont* font);
void deleteBrush(CBrush* br);
void GetActiveWindowTitle(CString& sNewTitle, CRect& ActiveWndRect, HWND* pActivWnd=NULL);
DWORD WINAPI MouseTrackerThread(LPVOID lpdwstatus);
BOOL PtInWindowRect(POINT pt, CWnd* wnd);
BOOL MouseInWndRect(CWnd* wnd);
int GetStatusIcon(int iPerson);
CSize GetLargestLineSize(CDC* pDC, CString sText, int& iLines);
void IAnimateWindow(CWnd* pWnd, DWORD dwStyle=0, DWORD dwParam=0);
void AsyncPlaySound(const char* szSoundName="");
CString getLinkedID(const char* szLinkID);
void InitTRIconList(CComboBoxEx& list);
void EnableDebugPriv();
void GetProcessInfo(DWORD dwProcId, CProcInfo*& pInfo);
CString getActionHotKeyDsc(int iActionNum,const char* szBefore="\t", const char* szAfter="");
CString getClipHotKeyDsc(int iClip,BOOL bCopy);
void PartlyEmptyTrash();
void TimetToFileTime( time_t t, LPFILETIME pft );
BOOL GetFileDateTime(const char* szFile, FILETIME* tOut, BOOL bModTime=0, BOOL bFold=0);
void SetWallpaperStyle(DWORD dwTile, DWORD dwStrtch);
void SetWallpaperFileInReg(const char* szWallpaperPath, BOOL bForDefUserOnly=FALSE);
int UpdateTrialityStatus(CString sUser, CString sKeys);
void CheckFlashPlayer();
BOOL GetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut, size_t dwOutSize);
BOOL SetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut);
//
#include "DLG_InfoWindow.h"
int AskYesNo(const char* szText, const char* szTitle, long* bAnswerHolder=NULL, CWnd* pParent=NULL);
int AskYesNo(const char* szText, long* bAnswerHolder, CWnd* pParent);
void AddError(const char* szError, BOOL bSystem=FALSE);
InfoWnd* Alert(InfoWndStruct* pInfo);
InfoWnd* Alert(const char* szText);
InfoWnd* Alert(const char* szText,DWORD dwTime, BOOL bStartHided=FALSE, BOOL bStaticPars=-1);
InfoWnd* Alert(const char* szText, const char* szTitle, CWnd* pParent=NULL, BOOL bScrCentered=0);
void SmartWarning(const char* szText, const char* szTitle, long* bAnswerHolder, DWORD dwTimer=0);
#endif // !defined(AFX_UTILS_H__7CD51405_6EC9_48A4_BBD1_CAFAD8F26FC4__INCLUDED_)
