#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HHOOK g_hhook=NULL;
HANDLE g_hEvent=NULL;
long g_lCurrentElement=0;
EVENTMSG* g_aEvents=NULL;
HWND g_hTargetWnd=NULL;
LRESULT CALLBACK JournalPlaybackProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(g_hhook==0){
		SetEvent(g_hEvent);
		return 0;
	}
	if (nCode>=0 && g_aEvents!=NULL){
		switch(nCode){
		case HC_GETNEXT:
			{
				EVENTMSG* pMsg=(EVENTMSG*)lParam;
				memcpy(pMsg,&g_aEvents[g_lCurrentElement],sizeof(EVENTMSG));
				pMsg->hwnd=g_hTargetWnd;
				pMsg->time=GetTickCount();
				Sleep(10);
				return 0;
			}
		case HC_SKIP:
			{
				g_lCurrentElement++;
				if(g_aEvents[g_lCurrentElement].message==0){
					PostThreadMessage(GetCurrentThreadId(),WM_CANCELJOURNAL,0,0);
					SetEvent(g_hEvent);
				}
				break;
			}
		}
		return 0;
	}
	return(CallNextHookEx(g_hhook, nCode, wParam, lParam));
}

EVENTMSG g_aNumLockEvent[]=
{
	{WM_KEYDOWN		,VK_NUMLOCK	,0x45							,0,0},
	{WM_KEYUP		,VK_NUMLOCK	,0x45							,0,0},
	{0,0,0,0,0}
};

EVENTMSG g_aCtrlInsEvent[]=
{
	{WM_KEYDOWN		,VK_CONTROL	,::MapVirtualKey(VK_CONTROL,0)	,0,0},
	{WM_KEYDOWN		,VK_INSERT	,::MapVirtualKey(VK_INSERT,0)	,0,0},
	{WM_KEYUP		,VK_INSERT	,::MapVirtualKey(VK_INSERT,0)	,0,0},
	{WM_KEYUP		,VK_CONTROL	,::MapVirtualKey(VK_CONTROL,0)	,0,0},
	{0,0,0,0,0}
};

EVENTMSG g_aCtrlCEvent[]=
{
	{WM_KEYDOWN		,VK_CONTROL	,::MapVirtualKey(VK_CONTROL,0)	,0,0},
	{WM_KEYDOWN		,'C'		,::MapVirtualKey('C',0)			,0,0},
	{WM_KEYUP		,'C'		,::MapVirtualKey('C',0)			,0,0},
	{WM_KEYUP		,VK_CONTROL	,::MapVirtualKey(VK_CONTROL,0)	,0,0},
	{0,0,0,0,0}
};

EVENTMSG g_aShiftInsEvent[]=
{
	{WM_KEYDOWN		,VK_SHIFT	,::MapVirtualKey(VK_SHIFT,0)	,0,0},
	{WM_KEYDOWN		,VK_INSERT	,::MapVirtualKey(VK_INSERT,0)	,0,0},
	{WM_KEYUP		,VK_INSERT	,::MapVirtualKey(VK_INSERT,0)	,0,0},
	{WM_KEYUP		,VK_SHIFT	,::MapVirtualKey(VK_SHIFT,0)	,0,0},
	{0,0,0,0,0}
};

EVENTMSG g_aCtrlVEvent[]=
{
	{WM_KEYDOWN		,VK_CONTROL	,::MapVirtualKey(VK_CONTROL,0)	,0,0},
	{WM_KEYDOWN		,'V'		,::MapVirtualKey('V',0)			,0,0},
	{WM_KEYUP		,'V'		,::MapVirtualKey('V',0)			,0,0},
	{WM_KEYUP		,VK_CONTROL	,::MapVirtualKey(VK_CONTROL,0)	,0,0},
	{0,0,0,0,0}
};


BOOL SendSequenceViaHook(HWND hTargetWnd,int iSeqNumber)
{
	return FALSE;
	BOOL fOk=FALSE;
	g_hTargetWnd=hTargetWnd;
	g_lCurrentElement=0;
	switch(iSeqNumber){
	case 0:
		g_aEvents=g_aNumLockEvent;
		break;
	case 1:
		g_aEvents=g_aCtrlCEvent;//g_aCtrlInsEvent;
		break;
	case 2:
		g_aEvents=g_aCtrlCEvent;
		break;
	case 3:
		g_aEvents=g_aCtrlVEvent;//g_aShiftInsEvent;
		break;
	case 4:
		g_aEvents=g_aCtrlVEvent;
		break;
	}
	MSG msg;
	HINSTANCE hThis=AfxGetInstanceHandle();
	g_hEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	if(g_hhook==0){
		g_hhook = SetWindowsHookEx(WH_JOURNALPLAYBACK, JournalPlaybackProc, hThis, 0);
	}
	if(g_hhook==NULL){
		DWORD dwErr=GetLastError();
		return FALSE;
	}
	while(GetMessage(&msg,0,0,0)>0){
		if(msg.message==WM_CANCELJOURNAL){
			UnhookWindowsHookEx(g_hhook);
			g_hhook=NULL;
			break;
		}
		::DispatchMessage(&msg);
	}
	WaitForSingleObject(g_hEvent,10000);
	CloseHandle(g_hEvent);
	g_aEvents=NULL;
	return TRUE;
}

