// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_Schedule.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _DEBUG
#define TRACETHREAD	TRACE3("\nCheckpoint: Thread: 0x%04x, file=%s, line=%lu\n",::GetCurrentThreadId(),__FILE__,__LINE__);
#else
#define TRACETHREAD
#endif

#define FORK(func,param) \
	{\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func, LPVOID(param), 0, &dwThread);\
		::CloseHandle(hThread);\
	}

CString _l(const char* szText);
WKCallbackInterface*& WKGetPluginContainer();
extern HANDLE hStopEvent;
extern HINSTANCE g_hinstDll;
extern HANDLE hHookerThread;
extern COptions plgOptions;
extern CRITICAL_SECTION csMainThread;
/*
char* GetSubString(const char* szFrom, const char* szBegin, char cEnd, char* szTarget, const char* szDefaultValue="", int iOutBufLen=16)
{
	strcpy(szTarget,szDefaultValue);
	char* szPos=strstr(szFrom,szBegin);
	if(szPos){
		szPos=szPos+strlen(szBegin);
		char* szEnd=strchr(szPos,cEnd);
		if(szEnd){
			int iLen=szEnd-szPos;
			if(iLen>iOutBufLen){
				iLen=iOutBufLen-1;
			}
			memcpy(szTarget,szPos,iLen);
			szTarget[iLen]=0;
		}
	}
	return szTarget;
}

DWORD WINAPI GlobalHooker_Gestures(LPVOID pData)
{
	::EnterCriticalSection(&csMainThread);
	char c=0;
	CPoint pt0;
	GetCursorPos(&pt0);
	BOOL bNotStopStart=bNotStop;
	while(bNotStopStart==bNotStop){
		Sleep(20);
	}
	CPoint pt1;
	GetCursorPos(&pt1);
	long lX=pt1.x-pt0.x;
	long lY=pt1.y-pt0.y;
	CRect rt;
	GetWindowRect(GetDesktopWindow(),&rt);
	if(max(abs(lX),abs(lY))>max(rt.Width(),rt.Height())/5 && min(abs(lX),abs(lY))<max(rt.Width(),rt.Height())/5){
		if(abs(lX)>abs(lY)){
			// X
			if(pt1.x>pt0.x){
				c='0';
			}else{
				c='1';
			}
		}else{
			// Y
			if(pt1.y>pt0.y){
				c='2';
			}else{
				c='3';
			}
		}
	}
	// Собрали информацию!!! Обрабатываем
	if(c!=0){
		char sz[]="MOG0";
		sz[3]=c;
		WKGetPluginContainer()->NotifyEventStarted(sz);
		FORK(DepressKey,c);
	}
	::LeaveCriticalSection(&csMainThread);
	return 0;
}
*/
/*char szHotkeyStrPresentation[32]={0};
int	WINAPI WKGetPluginFunctionHints(long iPluginFunction, long lHintCode, void*& pOut)
{
	if(iPluginFunction==0){
		strcpy(szHotkeyStrPresentation,"%0~-4");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	if(iPluginFunction==1){
		strcpy(szHotkeyStrPresentation,"%0@~-4");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	return 0;
};*/

#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif