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
	MouseTrackerRuler()
	{
		pWnd=NULL;
		dwTaskbarFriendly=TRUE;
		bFocusFriendly=FALSE;
		bStopWatching=FALSE;
		MousePreProc=NULL;
		MouseOutProc=NULL;
		dwAddFlag1=0;
		pSlider=0;
	}
	OnMouseOut MouseOutProc;
	OnMouseOut MousePreProc;
	CWnd*	pWnd;
	DWORD	dwTaskbarFriendly;
	BOOL	bFocusFriendly;
	BOOL	bStopWatching;
	DWORD	dwAddFlag1;
	CSliderCtrl* pSlider;
};
void FAR WINAPI OnNoteLeave(MouseTrackerRuler* pRuler);
void FAR WINAPI OnNoteLeave2(MouseTrackerRuler* pRuler);
//
int FindColorSchemNumByName(const char* szName);
void FillComboWithColorschemes(CComboBox* ID,int iStart=0);
void deleteFont(CFont* font);
void deleteBrush(CBrush* br);
void GetActiveWindowTitle(CString& sNewTitle, CRect& ActiveWndRect, HWND* pActivWnd=NULL, DWORD* dwWindowLong=NULL);
DWORD WINAPI MouseTrackerThread(LPVOID lpdwstatus);
BOOL PtInWindowRect(POINT pt, CWnd* wnd);
BOOL MouseInWndRect(CWnd* wnd);
int GetStatusIcon(int iPerson);
CString GetLargestLine(CString sText, int& iLines);
BOOL IAnimateWindow(CWnd* pWnd, long lStyle, DWORD dwParam=200);
BOOL IAnimateWindow(HWND pWnd, long lStyle, DWORD dwParam=200);
void AsyncPlaySound(const char* szSoundName="");
CString getLinkedID(const char* szLinkID);
void InitTRIconList(CComboBoxEx& list);
void GetWndIcon(HWND& hSlayerWnd,HICON& hIcon);
void GetProcessInfo(DWORD dwProcId, CProcInfo*& pInfo);
CString getActionHotKeyDsc(int iActionNum,const char* szBefore="\t", const char* szAfter="");
CString getClipHotKeyDsc(int iClip,BOOL bCopy);
void PartlyEmptyTrash();
void TimetToFileTime( time_t t, LPFILETIME pft );
BOOL GetFileDateTime(const char* szFile, FILETIME* tOut);
//
#include "DLG_Note.h"
void CreateNote(CNoteSettings* pNoteParams);
void OpenNote(CNoteSettings* pNoteParams);
#include "DLG_InfoWindow.h"
int AskYesNo(const char* szText, const char* szTitle, long* bAnswerHolder=NULL, CWnd* pParent=NULL);
int AskYesNo(const char* szText, long* bAnswerHolder, CWnd* pParent);
void AddError(const char* szError, BOOL bSystem=FALSE);
InfoWnd* Alert(InfoWndStruct* pInfo);
InfoWnd* Alert(const char* szText);
InfoWnd* Alert(const char* szText,DWORD dwTime, BOOL bStartHided=FALSE, BOOL bStaticPars=-1);
InfoWnd* Alert(const char* szText, const char* szTitle, DWORD dwTime=0);
void SmartWarning(const char* szText, const char* szTitle, long* bAnswerHolder, DWORD dwTimer=0, DWORD dwTextTraits=DT_CENTER|DT_VCENTER, CWnd* pParent=0);
void AddRecipientToRecentList(const char* szName);
void AppendBBMenu(CMenu* menu, int iSharedAlready, int iStart=WM_USER+BB_SECTION);
void AppendStylesMenu(CMenu* menu, CItem* pItem, BOOL bAsSubMenu=TRUE);
void SetNotePosToCursor(CNoteSettings* pNoteSets);
CString GetDateInCurFormat(COleDateTime dt);
CString GetWeekDay(long i, BOOL b=0);
void AlertBox(const char* szTitle, const char* szText, DWORD dwTime);
void WNSubstMyVariables(CString& sText, CString aWType);
BOOL CalculateExpression_Ole(CString& sExpr);
CFileInfoArray& GetSmilesEnum();
#endif // !defined(AFX_UTILS_H__7CD51405_6EC9_48A4_BBD1_CAFAD8F26FC4__INCLUDED_)
