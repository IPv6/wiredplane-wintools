/////////////////////////////////////////////////////////////
// hooklib.c
// серверная часть

#define __GLOBAL_HOOK
#include <windows.h>
#include <stdio.h>
#include <memory.h> 
#include "..\..\..\SmartWires\SystemUtils\Macros.h"

#pragma data_seg("Shared")
HHOOK	g_hhook	= NULL;
int		shMem[2]={0,0};
BOOL	bActive=0;
DWORD	dwFlags=1;
DWORD	dwWasMoved=0;
RECT	rtLastDownPoint;
long	lHookActive=0;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")
#define		H_LA	0x001
#define		H_LC	0x002
#define		H_LW	0x004
#define		H_LS	0x008
#define		H_RA	0x010
#define		H_RC	0x020
#define		H_RW	0x040
#define		H_RS	0x080
#define		H_ANY	0x400
//--
#define		H_RESTORE	0x0100
#define		H_ROLLBACK	0x0200
#define		H_RESTPROH	0x0400

HINSTANCE g_hinstDll = NULL;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	BOOL bSwitchingOff=FALSE;
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hinstDll = (HINSTANCE)hModule;
			bSwitchingOff=FALSE;
			break;
	
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			bSwitchingOff=TRUE;
			break;
    }
    return TRUE;
}

#include "../../WKPlugin.h"
LRESULT WINAPI MsgHookProc( int code, WPARAM wParam, LPARAM lParam )
{
	if (WKUtils::isWKUpAndRunning() && g_hhook && bActive && code==HC_ACTION && lParam!=0){
		SimpleTracker lock(lHookActive);
		LPMSG msg = (LPMSG)lParam;
		if(dwFlags&H_ROLLBACK)
		{
			if(msg && (msg->message == WM_LBUTTONDOWN || msg->message == WM_NCLBUTTONDOWN)){
				dwWasMoved=0;
				HWND hFgw= ::GetForegroundWindow();
				GetWindowRect(hFgw,&rtLastDownPoint);
			}
			if(msg && (msg->message == WM_RBUTTONDOWN || msg->message == WM_NCRBUTTONDOWN)){
				if(dwWasMoved && GetAsyncKeyState(VK_LBUTTON)<0 && !(rtLastDownPoint.left==0 && rtLastDownPoint.top==0 && rtLastDownPoint.right==0 && rtLastDownPoint.bottom==0)){
					dwWasMoved=0;
					HWND hFgw= ::GetForegroundWindow();
					::MoveWindow( hFgw, rtLastDownPoint.left, rtLastDownPoint.top, rtLastDownPoint.right-rtLastDownPoint.left, rtLastDownPoint.bottom-rtLastDownPoint.top, TRUE );
				}
			}
		}
		if(msg && (msg->message == WM_MOUSEMOVE || msg->message == WM_NCMOUSEMOVE)){
			int oldX= shMem[0];
			int oldY= shMem[1];
			shMem[0]= (int)(msg->pt.x);
			shMem[1]= (int)(msg->pt.y);
			if( (msg->wParam & MK_LBUTTON)!=0 ){
				BOOL bOK=TRUE;
				if(dwFlags & H_ANY){
					if((dwFlags & H_LA) || (dwFlags & H_RA)){
						if(GetAsyncKeyState(VK_LMENU)>=0 && GetAsyncKeyState(VK_RMENU)>=0){
							bOK=FALSE;
						}
					}
					if((dwFlags & H_LS) || (dwFlags & H_RS)){
						if(GetAsyncKeyState(VK_LSHIFT)>=0 && GetAsyncKeyState(VK_RSHIFT)>=0){
							bOK=FALSE;
						}
					}
					if((dwFlags & H_LC) || (dwFlags & H_RC)){
						if(GetAsyncKeyState(VK_LCONTROL)>=0 && GetAsyncKeyState(VK_RCONTROL)>=0){
							bOK=FALSE;
						}
					}
					if((dwFlags & H_LW) || (dwFlags & H_RW)){
						if(GetAsyncKeyState(VK_LWIN)>=0 && GetAsyncKeyState(VK_RWIN)>=0){
							bOK=FALSE;
						}
					}
				}else{
					if((dwFlags & H_LA) && GetAsyncKeyState(VK_LMENU)>=0){
						bOK=FALSE;
					}
					if((dwFlags & H_RA) && GetAsyncKeyState(VK_RMENU)>=0){
						bOK=FALSE;
					}
					if((dwFlags & H_LS) && GetAsyncKeyState(VK_LSHIFT)>=0){
						bOK=FALSE;
					}
					if((dwFlags & H_RS) && GetAsyncKeyState(VK_RSHIFT)>=0){
						bOK=FALSE;
					}
					if((dwFlags & H_LC) && GetAsyncKeyState(VK_LCONTROL)>=0){
						bOK=FALSE;
					}
					if((dwFlags & H_RC) && GetAsyncKeyState(VK_RCONTROL)>=0){
						bOK=FALSE;
					}
					if((dwFlags & H_LW) && GetAsyncKeyState(VK_LWIN)>=0){
						bOK=FALSE;
					}
					if((dwFlags & H_RW) && GetAsyncKeyState(VK_RWIN)>=0){
						bOK=FALSE;
					}
				}
				if(bOK){
					RECT rct;
					HWND hFgw= ::GetForegroundWindow();
					if(dwFlags&H_RESTPROH){
						if(::IsZoomed(hFgw)){
							bOK=FALSE;
						}
					}
					if(bOK){// Двинулось!
						dwWasMoved=1;
						::GetWindowRect( hFgw, &rct );
						if(dwFlags&H_RESTORE){
							if(::IsZoomed(hFgw)){
								WINDOWPLACEMENT pl;
								memset(&pl,0,sizeof(WINDOWPLACEMENT));
								pl.length=sizeof(WINDOWPLACEMENT);
								::GetWindowPlacement(hFgw,&pl);
								pl.showCmd=SW_RESTORE;
								memcpy(&pl.rcNormalPosition,&pl.ptMaxPosition,sizeof(pl.rcNormalPosition));
								::SetWindowPlacement(hFgw,&pl);
							}
						}
						::MoveWindow( hFgw, rct.left+msg->pt.x-oldX, rct.top+msg->pt.y-oldY, rct.right-rct.left, rct.bottom-rct.top, TRUE );
					}
				}
			}
		}
	}
	return CallNextHookEx( g_hhook, code, wParam, lParam );   
}

BOOL WINAPI SetOptions(BOOL bEnable, DWORD dwInOutFlags)
{
	bActive=bEnable;
	dwFlags=dwInOutFlags;
	return TRUE;
}

BOOL WINAPI InstallHook(BOOL bSet)
{
	// Сбрасываем данные...
	rtLastDownPoint.left=rtLastDownPoint.right=rtLastDownPoint.top=rtLastDownPoint.bottom=0;
	// Работаем с хуком
	BOOL fOk=FALSE;
	if (bSet) {
		g_hhook = NULL;
		char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, MsgHookProc, hDLL, 0);
		fOk = (g_hhook != NULL);
		if(g_hhook==NULL){
			//FLOG3("SetWindowsHookEx failed! File=%s, line=%lu, err=%08X",__FILE__,__LINE__,GetLastError());
		}
	}else if(g_hhook!=NULL){
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
	return fOk;
}
