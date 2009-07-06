/////////////////////////////////////////////////////////////
// hooklib.c
// серверная часть

#define __GLOBAL_HOOK
#include <windows.h>
#include <stdio.h>
#include <memory.h> 
#include "..\..\..\SmartWires\SystemUtils\Macros.h"

#pragma data_seg("ScrollShared")
HHOOK	g_hhook	= NULL;
int		oldX=0;
int		oldScrX=0;
int		oldY=0;
int		oldScrY=0;
DWORD	dwFlags=0x10;
DWORD	dwWasMoved=0;
RECT	rtLastDownPoint;
long	lRecurseLock=0;
long	lHookActive=0;
long	bHookEnabled=0;
long	lSmooth=4;
long	lSpeedLim=10;
long	lNoRe=0;
#pragma data_seg()
#pragma comment(linker, "/section:ScrollShared,rws")
#define		H_LA	0x01
#define		H_LC	0x02
#define		H_LW	0x04
#define		H_LS	0x08
#define		H_RA	0x10
#define		H_RC	0x20
#define		H_RW	0x40
#define		H_RS	0x80
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
BOOL GetGUIThreadInfoMy(DWORD dwCurWinThreadID, MYGUITHREADINFO* gui)
{
	BOOL bRes=FALSE;
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	if(hUser32){
		_GetGUIThreadInfo fp=(_GetGUIThreadInfo)GetProcAddress(hUser32,"GetGUIThreadInfo");
		if(fp){
			memset(gui,0,sizeof(MYGUITHREADINFO));
			gui->cbSize=sizeof(MYGUITHREADINFO);
			bRes=(*fp)(dwCurWinThreadID, gui);
		}
		FreeLibrary(hUser32);
	}
	return bRes;
}

void SendMessageRec(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam)
{
	PostMessage(hWnd,Msg,wParam,lParam);
	Sleep(2);
	/*
	int iC=0;
	while(iC<3 && SendMessage(hWnd,Msg,wParam,lParam)==0){
		hWnd=GetParent(hWnd);
		iC++;
	}*/
}

DWORD WINAPI GOinThread(LPVOID p)
{
	if(lNoRe>0){
		return 0;
	}
	SimpleTracker lc(lNoRe);
	HWND hFgw=(HWND )p;
	while(GetAsyncKeyState(VK_RBUTTON)<0){
		BOOL bOK=TRUE;
		if((dwFlags & H_LA) && GetAsyncKeyState(VK_LMENU)>=0){
			bOK=FALSE;
		}else if((dwFlags & H_RA) && GetAsyncKeyState(VK_RMENU)>=0){
			bOK=FALSE;
		}else if((dwFlags & H_LS) && GetAsyncKeyState(VK_LSHIFT)>=0){
			bOK=FALSE;
		}else if((dwFlags & H_RS) && GetAsyncKeyState(VK_RSHIFT)>=0){
			bOK=FALSE;
		}else if((dwFlags & H_LC) && GetAsyncKeyState(VK_LCONTROL)>=0){
			bOK=FALSE;
		}else if((dwFlags & H_RC) && GetAsyncKeyState(VK_RCONTROL)>=0){
			bOK=FALSE;
		}else if((dwFlags & H_LW) && GetAsyncKeyState(VK_LWIN)>=0){
			bOK=FALSE;
		}else if((dwFlags & H_RW) && GetAsyncKeyState(VK_RWIN)>=0){
			bOK=FALSE;
		}
		if(bOK){
			if(hFgw!=NULL){
				if(lSmooth<1){
					lSmooth=1;
				}
				RECT rt;
				::GetWindowRect(hFgw,&rt);
				/*lRecurseLock++;
				POINT pt;
				pt.x=msg->pt.x;
				pt.y=msg->pt.y;
				pt.x=rt.right-::GetSystemMetrics(SM_CXVSCROLL)/2;
				::SetCursorPos(pt.x,pt.y);
				mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN,pt.x,pt.y,0,0);
				mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,pt.x,pt.y,0,0);
				::SetCursorPos(pt.x,pt.y);
				::SendMessage(hFgw,WM_NCLBUTTONDOWN,MK_LBUTTON,MAKEWORD(pt.x,pt.y));
				::SendMessage(hFgw,WM_NCMOUSEMOVE,MK_LBUTTON,MAKEWORD(pt.x,pt.y+10));
				::SetCursorPos(pt.x,pt.y+10);
				::SendMessage(hFgw,WM_NCLBUTTONUP,MK_LBUTTON,MAKEWORD(pt.x,pt.y+10));
				::SetCursorPos(msg->pt.x,msg->pt.y);
				lRecurseLock--;*/
				POINT pt;
				GetCursorPos(&pt);
				int idX=pt.x-oldX;
				int idY=pt.y-oldY;
				int iXLim=(ABS(idX)/lSmooth);
				int iYLim=(ABS(idY)/lSmooth);
				for(int i=0;i<max(iXLim,iYLim) && i<lSpeedLim;i++){
					if(i<iXLim){
						SendMessageRec(hFgw,WM_HSCROLL,(idX<0)?SB_LINELEFT:SB_LINERIGHT,NULL);
					}
					if(i<iYLim){
						SendMessageRec(hFgw,WM_VSCROLL,(idY<0)?SB_LINEUP:SB_LINEDOWN,NULL);
					}
				}
				/*
				 SCROLLINFO siX;
				 memset(&siX,0,sizeof(siX));
				 siX.cbSize=sizeof(siX);
				 siX.fMask=SIF_ALL;
				 if(GetScrollInfo(hFgw,SB_HORZ,&siX)){
				 siX.fMask=SIF_POS|SIF_TRACKPOS;
				 siX.nPos=int(siX.nMin+double(msg->pt.x-rt.left)/double(rt.right-rt.left)*double(siX.nMax-siX.nMin));
				 if(siX.nPos>=0){
				 siX.nTrackPos=siX.nPos;
				 SetScrollInfo(hFgw,SB_HORZ,&siX, TRUE);
				 ::PostMessage(hFgw,WM_HSCROLL,(siX.nPos<<16)|(SB_THUMBTRACK),NULL);
				 }
				 }
				 SCROLLINFO siY;
				 memset(&siY,0,sizeof(siY));
				 siY.cbSize=sizeof(siY);
				 siY.fMask=SIF_ALL;
				 if(GetScrollInfo(hFgw,SB_VERT,&siY)){
				 siY.fMask=SIF_POS|SIF_TRACKPOS;
				 siY.nPos=int(siY.nMin+double(msg->pt.y-rt.top)/double(rt.bottom-rt.top)*double(siY.nMax-siY.nMin));
				 if(siY.nPos>=0){
				 siY.nTrackPos=siY.nPos;
				 SetScrollInfo(hFgw,SB_VERT,&siY, TRUE);
				 ::PostMessage(hFgw,WM_VSCROLL,(siY.nPos<<16)|(SB_THUMBTRACK),NULL);
				 }
				 }
					*/
				Sleep(5);
			}
		}else{
			break;
		}
	}
	return 0;
}

#include "../../WKPlugin.h"
#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif
LRESULT WINAPI MouseProc( int code, WPARAM wParam, LPARAM lParam )
{
	if (WKUtils::isWKUpAndRunning() && bHookEnabled && g_hhook && code==HC_ACTION && lParam!=0 && lRecurseLock==0){
		SimpleTracker lock(lHookActive);
		MOUSEHOOKSTRUCT* msg = (MOUSEHOOKSTRUCT*)lParam;
		if(msg){
			if(wParam == WM_RBUTTONDOWN || wParam == WM_NCRBUTTONDOWN){
				POINT pt;
				GetCursorPos(&pt);
				oldX= (int)(pt.x);
				oldY= (int)(pt.y);
				HWND hFgw=::WindowFromPoint(msg->pt);//::GetForegroundWindow();//msg->hwnd;
				SCROLLINFO siX,siY;
				memset(&siX,0,sizeof(siX));
				siX.cbSize=sizeof(siX);
				siX.fMask=SIF_ALL;
				GetScrollInfo(hFgw,SB_HORZ,&siX);
				oldScrX=siX.nPos;
				memset(&siY,0,sizeof(siY));
				siY.cbSize=sizeof(siY);
				siY.fMask=SIF_ALL;
				GetScrollInfo(hFgw,SB_VERT,&siY);
				oldScrY=siY.nPos;
			}
			if(wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE){
				HWND hFgw=::WindowFromPoint(msg->pt);//::GetForegroundWindow();//msg->hwnd;
				FORK(GOinThread,hFgw);
			}
		}
	}
	return CallNextHookEx( g_hhook, code, wParam, lParam );   
}

BOOL WINAPI SetOptions(BOOL bEnable, DWORD dwInOutFlags, DWORD dwInSmooth, DWORD dwInSpLim)
{
	bHookEnabled=bEnable;
	if(bEnable){
		dwFlags=dwInOutFlags;
		lSmooth=dwInSmooth;
		lSpeedLim=dwInSpLim;
	}
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
		g_hhook = SetWindowsHookEx(WH_MOUSE, MouseProc, hDLL, 0);
		fOk = (g_hhook != NULL);
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
