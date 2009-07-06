// HookCode.cpp: implementation of the CHookCode class.
//
//////////////////////////////////////////////////////////////////////
#include "HookCode.h"
#include "..\..\..\SmartWires\SystemUtils\Macros.h"
#include "..\..\..\SmartWires\SystemUtils\Debug.h"
// Instruct the compiler to put the g_hhook data variable in 
// its own data section called Shared. We then instruct the 
// linker that we want to share the data in this section 
// with all instances of this application.
#pragma data_seg("Shared")
long	lHookActive = 0;
HHOOK	g_hhook = NULL;
DWORD	g_dwTargetThread=0;
DWORD	g_LastChar=0;
CWKKeyLoggerInfo
		hookData = {0};
DWORD	g_TimeStopper=0;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")
// Nonshared variables
HINSTANCE g_hinstDll = NULL;
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


char lwrchr(char c)
{
	if(c>='A' && c<='Z'){
		c=c-('A'-'a');
	}
	return c;
}

char uprchr(char c)
{
	if(c>='a' && c<='z'){
		c=c-('a'-'A');
	}
	return c;
}

#include "../../WKPlugin.h"
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(!g_hhook){
		return 0;
	}
	if(WKUtils::isWKUpAndRunning() && g_dwTargetThread!=0){
		SimpleTracker lock(lHookActive);
		if (nCode>=0 && lParam!=0){
			if(wParam==PM_REMOVE){
				MSG* pStruct=(MSG*)(lParam);
				if(hookData.bLogChars || hookData.bATChars){
					if(hookData.bWatchUps && pStruct->message==WM_KEYUP){
						::PostThreadMessage(g_dwTargetThread,WM_USER+1,(WPARAM)pStruct->hwnd,(LPARAM)pStruct->wParam);
					}
				}
				/*if(pStruct->message>=0xC000 && pStruct->message<=0xFFFF){
					if(CDebugWPHelper::isDebug(2)){
						if(GetTickCount()-g_TimeStopper>1000){
							char szName[1024]={0};
							GetClipboardFormatName(pStruct->message,szName,sizeof(szName));
							if(strcmp(szName,"SHELLHOOK")!=0){
								DEBUG_INFO4("Registered message detected: %i '%s' (%08x:%08x)",pStruct->message, szName, pStruct->wParam, pStruct->lParam);
							}
						}
						g_TimeStopper=GetTickCount();
					}
				}*/
				if(pStruct->message==WM_KEYDOWN){
					g_LastChar=pStruct->wParam;
				}
				if(pStruct->message==WM_CHAR){
					BOOL bSend=TRUE;
					DWORD dwVirtuaCode=pStruct->wParam;
					DWORD dwVirtuaCodeOriginal=dwVirtuaCode;
					if(dwVirtuaCode<32){
						bSend=FALSE;
					}
					if(dwVirtuaCode=='\n'  || dwVirtuaCode=='\r' || dwVirtuaCode=='\t' || dwVirtuaCode==VK_BACK){
						bSend=TRUE;
					}
					if(bSend && g_dwTargetThread){
						////FLOG3("char: %08X, %08X, last virt=%08X",pStruct->wParam, pStruct->lParam, g_LastChar);
						DWORD dwScanCode=0;
						if(g_LastChar>0){
							if(dwVirtuaCode<127){
								dwScanCode=((pStruct->lParam)>>16)&0xFF;
								BYTE lKeyState[256]={0};
								lKeyState[VK_SHIFT]=(GetKeyState(VK_SHIFT)<0?1:0)<<7;
								lKeyState[VK_CONTROL]=(GetKeyState(VK_CONTROL)<0?1:0)<<7;
								lKeyState[VK_MENU]=(GetKeyState(VK_MENU)<0?1:0)<<7;
								WORD wBuffer=0;
								if(ToAsciiEx(g_LastChar, dwScanCode, (CONST PBYTE)lKeyState, &wBuffer, 0, GetKeyboardLayout(GetCurrentThreadId()))>0){
									dwVirtuaCode=LOBYTE(wBuffer);
								}
							}
							g_LastChar=0;
						}
						::PostThreadMessage(g_dwTargetThread,WM_USER,(WPARAM)dwVirtuaCodeOriginal,(LPARAM)dwVirtuaCode);
					}
				}
			}
		}
	}
	return(CallNextHookEx(g_hhook, nCode, wParam, lParam));
}

BOOL WINAPI SetHook(DWORD dwTargetThread)
{
	BOOL fOk = FALSE;
	g_dwTargetThread=dwTargetThread;
	if (dwTargetThread!=0) {
		if(g_hhook != NULL){
			return FALSE;
		};
		char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hDLL, 0);
		fOk = (g_hhook != NULL);
		if(g_hhook==NULL){
			//FLOG3("SetWindowsHookEx failed! File=%s, line=%lu, err=%08X",__FILE__,__LINE__,GetLastError());
		}
	} else {
		if(g_hhook == NULL){
			return FALSE;
		};
		BOOL bNoNeedToWait=FALSE;
		fOk=SmartUnhookWindowsHookEx(g_hhook,&bNoNeedToWait);
		g_hhook=NULL;
		if(!bNoNeedToWait){
			long lCount=0;
			while(lHookActive>0){
				lCount++;
				Sleep(20);
				if(lCount>200){
					//FLOG3("Hook wait failed! File=%s, line=%lu",__FILE__,__LINE__,0);
					break;
				}
			}
		}
	}
	return(fOk);
}

void WINAPI SetHookInfoObject(CWKKeyLoggerInfo* in)
{
	memcpy(&hookData,in,sizeof(hookData));
	return;
}

void WINAPI TweakTriggers(int iType, BOOL bVal)
{
	if(iType==0){
		hookData.bWatchUps=bVal;
	}
}