// WCRemind.h : main header file for the WCRemind DLL
//

#if !defined(sAFX_WCRemind_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_)
#define sAFX_WCRemind_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_

#define RECURRENCY_NO		0
#define RECURRENCY_DAILY	1
#define RECURRENCY_WEEKLY	2
#define RECURRENCY_MONTHLY	3
#define RECURRENCY_ANNUALLY	4
#define RECURRENCY_XMINUTES	5
#define RECURRENCY_XHOURS	6

#define MODIFY_OPTS_NONE	0x0000
#define MODIFY_OPTS_NODAYS	0x0001

class CWPReminder
{
public:
	CWPReminder()
	{
		Clear();
	}
	void Clear()
	{
		memset(this,0,sizeof(CWPReminder));
	}
	char szKey[64];
	SYSTEMTIME EventTime;
	char szText[4048];
	int iRecurrency;
	BOOL bDisableRemider;
	BOOL bActPopup;
	BOOL bActLaunch;
	BOOL bActSound;
	BOOL bLoopSound;
	DWORD dXMinHourPeriod;
	char szLaunchPath[MAX_PATH];
	char szSoundPath[MAX_PATH];
	char szReserved[MAX_PATH*4];
	BOOL bActIgnoreOnSkip;
	BOOL bActAutoClosePopup;
};

class CCalendarCallback
{
public:
	virtual BOOL CallEditDayNote(SYSTEMTIME Time,char szFG[64], char szBG[64])=0;
	virtual BOOL GetDayNote(SYSTEMTIME Time,char szDsc[2048])=0;
	virtual BOOL GetRemDefText(char* szText,int size)=0;
	virtual BOOL GetRemDefWav(char* szSoundPath,int size)=0;
	virtual BOOL GetRemDefKey(char* szKey,int size)=0;
	virtual BOOL GetNotesFile(char* szOut,int size)=0;
	virtual BOOL CallContextDayNote(SYSTEMTIME Time, HWND hCalendar)=0;
};

#include "WCRenders.h"
typedef CImgLoader*& (WINAPI *__bmpX)();
typedef IUITranslation*& (WINAPI *__lngX)();
typedef long& (WINAPI *_GetApplicationLang)();
typedef BOOL (WINAPI *_ReminderCallback)(CWPReminder& rem);
typedef BOOL (WINAPI *_InitDLL)(int ihash, const char* szInitDir, _ReminderCallback pCallbackFunction);
typedef BOOL (WINAPI *_StopDLL)();
typedef int  (WINAPI *_GetCountOnDate)(char szDate[64]);
typedef BOOL (WINAPI *_SortReminders)();
typedef BOOL (WINAPI *_GetReminderOnDate)(char szDate[64], int iIndex, CWPReminder& pOut);
typedef BOOL (WINAPI *_GetReminder)(char szKey[64], CWPReminder& pOut);
typedef BOOL (WINAPI *_PutReminder)(char szKey[64], CWPReminder& pIn);
typedef BOOL (WINAPI *_CallModifyReminder)(char szKey[64], HWND hParent, DWORD dwOptions);
typedef BOOL (WINAPI *_RemoveReminder)(char szKey[64]);
typedef BOOL (WINAPI *_IsReminderFamilyPresent)(const char* szKey, int* iMaxSearchNumber);
typedef BOOL (WINAPI *_OpenCalendar)(CCalendarCallback* pCallback,BOOL bView);
typedef BOOL (WINAPI *_CreateNewReminder)(COleDateTime dt,CCalendarCallback* pCallback,CWnd* wParent);

BOOL WINAPI InitDLL(int ihash, CImgLoader*& bmpH, long& lngH, const char* szInitDir, _ReminderCallback pCallbackFunction);
int  WINAPI GetCountOnDate(char szDate[64]);
BOOL WINAPI GetReminderOnDate(char szDate[64], int iIndex, CWPReminder& pOut);
BOOL WINAPI GetReminder(char szKey[64], CWPReminder& pOut);
BOOL WINAPI PutReminder(char szKey[64], CWPReminder& pIn);
BOOL WINAPI RemoveReminder(char szKey[64]);
BOOL WINAPI CallModifyReminder(char szKey[64], HWND hParent, DWORD dwOptions);
BOOL WINAPI StopDLL();
BOOL WINAPI IsReminderFamilyPresent(const char* szKey, int* iMaxSearchNumber);
BOOL WINAPI OpenCalendar(CCalendarCallback* pCallback,BOOL bView);
#endif // !defined(sAFX_WCRemind_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_)



