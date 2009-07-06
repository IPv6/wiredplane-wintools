// HookCode.cpp: implementation of the CHookCode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HookCode.h"
// Instruct the compiler to put the g_hhook data variable in 
// its own data section called Shared. We then instruct the 
// linker that we want to share the data in this section 
// with all instances of this application.
#pragma data_seg("Shared")
HHOOK	g_hhook = NULL;
HWND	g_hTargetWin = NULL;
BOOL	g_bFirstTime = 0;
HFONT	g_hFont = NULL;
CIconsFontInfo
		hookData = {0};
CIconsFontOptions 
		plgOptions = {0};
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")

// Nonshared variables
HINSTANCE g_hinstDll = NULL;
void ForceDebugBreak()
{
   __try { _asm { int 3 }; }
   __except(UnhandledExceptionFilter(GetExceptionInformation())) { }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		g_hinstDll = hinstDll;
		break;
	case DLL_PROCESS_DETACH:
		// The calling process is detaching the DLL from its address space.
		break;
	case DLL_THREAD_ATTACH:
		// A new thread is being created in the current process.
		break;
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly.
		break;
	}
	return(TRUE);
}

#include "../WKPlugin.h"
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (WKUtils::isWKUpAndRunning() && nCode>=0 && lParam!=0) {
		MSG* msg=(MSG*)(lParam);
		if(g_bFirstTime==1){
			g_bFirstTime = 0;
			if(!isMemEmpty(&hookData.g_Font,sizeof(hookData.g_Font))){
				if(plgOptions.bSetFont){
					if(g_hFont){
						::DeleteObject(g_hFont);
					}
					g_hFont = ::CreateFontIndirect(&hookData.g_Font);
					::PostMessage(g_hTargetWin,WM_SETFONT,WPARAM(g_hFont),1);
				}
				if(plgOptions.bSetTxColor){
					ListView_SetTextColor(g_hTargetWin, hookData.g_FontColor);
				}
				if(plgOptions.bSetTransp){
					ListView_SetTextBkColor(g_hTargetWin, CLR_NONE);
				}else if(plgOptions.bSetBgColor){
					ListView_SetTextBkColor(g_hTargetWin, hookData.g_FontBgColor);
				}
				::RedrawWindow(g_hTargetWin,NULL,NULL,RDW_ERASE|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
				::PostMessage(g_hTargetWin,LVM_REDRAWITEMS, 0, (LPARAM)(int)ListView_GetItemCount(g_hTargetWin)-1);
			}
		}
	}
	return(CallNextHookEx(g_hhook, nCode, wParam, lParam));
}

BOOL WINAPI SetHook(HWND hTargetWin)
{
	BOOL fOk = FALSE;
	if (hTargetWin != 0) {
		g_bFirstTime = 1;
		g_hTargetWin=hTargetWin;
		DWORD dwThreadId=GetWindowThreadProcessId(hTargetWin,NULL);
		char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hDLL, dwThreadId);
		fOk = (g_hhook != NULL);
		PostMessage(g_hTargetWin,WM_NULL,0,0);
	} else {
		g_bFirstTime = 0;
		fOk = UnhookWindowsHookEx(g_hhook);
		PostMessage(g_hTargetWin,WM_SETFONT,0,1);
		g_bFirstTime = 0;
		g_hhook = 0;
	}
	return(fOk);
}

BOOL isMemEmpty(LPVOID pData, long count)
{
	BYTE* p=(BYTE*)pData;
	for(long i=0;i<count;i++){
		if(*(p+i)!=0){
			return FALSE;
		}
	}
	return TRUE;
}