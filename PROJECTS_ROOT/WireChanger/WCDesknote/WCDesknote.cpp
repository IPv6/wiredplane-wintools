// WCDesknote.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "../../SmartWires/SystemUtils/Debug.h"
#pragma data_seg("Shared")
POINT pLastClick={0};
long g_DblCT=0;
DWORD dwBGFreshTime=0;
DWORD g_WM_SF=0;
long lHookerDesiredAction=0;
HHOOK g_hhook = NULL;
HHOOK g_hhook2 = NULL;
long lHookActive=0;
HWND g_hFromWin=NULL;
HWND g_hTargetWindow=NULL;
LOGFONT pDeskFont={0};
LOGFONT lLocalFn={0};
HFONT g_hFont=0;
DWORD g_TxCol=0;
DWORD g_BgCol=0;
DWORD g_DblDownTime=0;
long g_skipOneLButton=0;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")
int absx(int i)
{
	if(i<0){
		return -i;
	}
	return i;
}

// Nonshared variables
HINSTANCE g_hinstDll = NULL; 
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

BOOL CALLBACK FindSysListView32(HWND hwnd,LPARAM lParam)
{
	HWND* hOut=(HWND*)lParam;
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	strupr(szClass);
	if(strcmp(szClass,"SYSLISTVIEW32")==0){
		*hOut=hwnd;
		return FALSE;
	}else{
		HWND hMaybeChildShell=FindWindowEx(hwnd,0,"SHELLDLL_DefView",NULL);
		if(hMaybeChildShell){
			HWND hMaybeChild=FindWindowEx(hMaybeChildShell,0,"SysListView32",NULL);
			if(hMaybeChild!=0){
				*hOut=hMaybeChild;
				return FALSE;
			}
		}
	}
	return TRUE;
}

HWND GetSysListView()
{
	static HWND hTargetWindow=NULL;
	if(hTargetWindow!=NULL && !IsWindow(hTargetWindow)){
		hTargetWindow=NULL;
	}
	if(hTargetWindow==NULL){
		HWND hWnd1 = ::FindWindow("Progman","Program Manager");
		if(hWnd1 == 0){
			hWnd1 = ::FindWindow("Progman",NULL);
		}
		if(hWnd1 == 0){
			return 0;
		}
		HWND hWnd2 = FindWindowEx(hWnd1,0,"SHELLDLL_DefView",NULL); 
		if(hWnd2 == 0){
			DWORD dwProgmanID=0;
			DWORD dwProgmanThread=GetWindowThreadProcessId(hWnd1,&dwProgmanID);
			EnumThreadWindows(dwProgmanThread,FindSysListView32,(LPARAM)&hTargetWindow);
		}else{
			hTargetWindow = FindWindowEx(hWnd2,0,"SysListView32",NULL); 
		}
	}
	return hTargetWindow;
}

BOOL WINAPI SetDeskFont(LOGFONT& pFont, DWORD dwText, DWORD dwBG)
{
	memcpy(&pDeskFont,&pFont,sizeof(pDeskFont));
	g_BgCol=dwBG;
	g_TxCol=dwText;
	PostMessage(GetSysListView(),g_WM_SF,0,0);
	return 1;
}

#define GHOOKACTION	"WIREKEYS_GHOOKACTIONEVENT"
#define LVHT_NOWHERE            0x0001
#define LVHT_ONITEMICON         0x0002
#define LVHT_ONITEMLABEL        0x0004
#define LVHT_ONITEMSTATEICON    0x0008
#define LVHT_ONITEM             (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON)
#define LVHT_ABOVE              0x0008
#define LVHT_BELOW              0x0010
#define LVHT_TORIGHT            0x0020
#define LVHT_TOLEFT             0x0040
#define LVM_FIRST               0x1000      // ListView messages
#define LVM_HITTEST             (LVM_FIRST + 18)
#define LVM_SETBKIMAGEA         (LVM_FIRST + 68)
#define LVM_UPDATE              (LVM_FIRST + 42)
#define LVM_GETITEMCOUNT        (LVM_FIRST + 4)
#define MAKEPOINTS(l)			(*((POINTS FAR *)&(l)))
#include <commctrl.h>
#ifndef _INC_COMMCTRL
typedef struct tagLVHITTESTINFO
{
    POINT pt;
    UINT flags;
    int iItem;
#if (_WIN32_IE >= 0x0300)
    int iSubItem;    // this is was NOT in win95.  valid only for LVM_SUBITEMHITTEST
#endif
} LVHITTESTINFO, FAR* LPLVHITTESTINFO;
#endif
#include <stdio.h>

// Эта функция НЕ ищет по парентам!!! Так и должно быть!
BOOL IsWindowsDesktopWnd(HWND hWin)
{
	char szClass[128]="";
	BOOL bClassRes=::GetClassName(hWin,szClass,sizeof(szClass));
	if(bClassRes>0){
		if(stricmp(szClass,"SHELLDLL_DefView")==0 
			|| stricmp(szClass,"Progman")==0 
			|| stricmp(szClass,"SysListView32")==0 
			|| stricmp(szClass,"SysTabControl32")==0 
			|| stricmp(szClass,"TrayNotifyWnd")==0 
			|| stricmp(szClass,"Shell_TrayWnd")==0 
			|| stricmp(szClass,"WorkerW")==0 || stricmp(szClass,"WorkerA")==0
			|| stricmp(szClass,"FakeDesktopWClass")==0 || stricmp(szClass,"FakeDesktopAClass")==0
			|| stricmp(szClass,"SysFader")==0
			|| stricmp(szClass,"Internet Explorer_Server")==0
			){
			return TRUE;
		}
		char szTitle[128]="";
		BOOL bTitleRes=::GetWindowText(hWin,szTitle,sizeof(szTitle));
		if(bTitleRes>0){
			if(stricmp(szTitle,"MCI command handling window")==0 ||
				stricmp(szTitle,"MM Notify Callback")==0 ||
				stricmp(szTitle,"DDE Server Window")==0 ||
				stricmp(szTitle,"Program Manager")==0 ||
				strstr(szTitle,"OleMainThread")!=0){
				return TRUE;
			}
		}
	}
	return FALSE;
}


LRESULT CALLBACK MyWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode>=0 && lParam!=0 && wParam==PM_REMOVE) {
		if(lHookActive && g_hFromWin){
			// Наше ли окно?
			POINT ptCur;
			GetCursorPos(&ptCur);
			HWND hCur=WindowFromPoint(ptCur);
			if(IsWindowsDesktopWnd(hCur)){
				MSG* msg=(MSG*)lParam;
				DWORD iMsgNum=msg->message;
				if(iMsgNum==g_WM_SF){
					HWND hListView=GetSysListView();//g_hTargetWindow
					if(hListView==NULL){
						hListView=msg->hwnd;
					}
					BOOL bUpdated=0;
					if(pDeskFont.lfFaceName[0]!=0){
						memcpy(&lLocalFn,&pDeskFont,sizeof(lLocalFn));
						memset(&pDeskFont,0,sizeof(pDeskFont));
						if(g_hFont){
							::DeleteObject(g_hFont);
						}
						g_hFont = ::CreateFontIndirect(&lLocalFn);
						if(g_hFont){
							::SendMessage(hListView,WM_SETFONT,WPARAM(g_hFont),1);
						}
						bUpdated=-1;
					}
					if(g_BgCol!=g_TxCol){
						ListView_SetTextBkColor(hListView, g_BgCol);
						ListView_SetTextColor(hListView, g_TxCol);
						bUpdated=-1;
					}
					if(bUpdated==-1){
						ListView_RedrawItems(hListView, 0, ListView_GetItemCount(hListView) - 1);
						bUpdated=1;
					}
				}
				if(iMsgNum==WM_MOUSEMOVE && g_skipOneLButton==0){
					::PostMessage(g_hFromWin,WM_USER+8/*DESKTOP_MOVE*/,0,0);
				}
				
				if(iMsgNum==WM_LBUTTONDOWN || iMsgNum==WM_LBUTTONUP|| iMsgNum==WM_LBUTTONDBLCLK){
					if((iMsgNum==WM_LBUTTONDOWN || iMsgNum==WM_LBUTTONUP) && g_skipOneLButton>0){
						g_skipOneLButton--;
						g_DblDownTime=0;
					}else{
						POINT ptCurClick;
						GetCursorPos(&ptCurClick);
						long lDif=GetTickCount()-g_DblDownTime;
						if(iMsgNum==WM_LBUTTONDBLCLK){
							iMsgNum=WM_LBUTTONDOWN;
							/*lDif=0;
							ptCurClick.x=pLastClick.x;
							ptCurClick.y=pLastClick.y;*/
						}
						if(iMsgNum==WM_LBUTTONUP){
							lDif=-1;
						}
						if(iMsgNum==WM_LBUTTONDOWN){
							if(g_DblCT==0){
								g_DblCT=GetDoubleClickTime()+100;
							}
							if(lDif>g_DblCT){
								g_DblDownTime=GetTickCount();
								ptCurClick.x=pLastClick.x;
								ptCurClick.y=pLastClick.y;
							}
						}
						int iRes=-2;
						{// Определяем есть ли иконка под курсором
							HWND hListView=GetSysListView();
							if(hListView==NULL){
								hListView=msg->hwnd;
							}
							if(!IsWindowVisible(GetSysListView())){
								iRes=-1;
							}else if(hListView){
								LVHITTESTINFO info;
								memset(&info,0,sizeof(info));
								::GetCursorPos(&info.pt);
								::ScreenToClient(GetSysListView(),&info.pt);
								info.flags=LVHT_ONITEM;
								DWORD dwRes=0;
								if(::SendMessageTimeout(hListView, LVM_HITTEST, 0, (LPARAM)&info,SMTO_ABORTIFHUNG,300,&dwRes)==0){
									iRes=-2;
								}else{
									iRes=(int)dwRes;
								}
							}
						}
						if(iRes==-1){
							// Под курсором не нацдено ничего социально полезного
							if(lDif>=0 && lDif<g_DblCT && absx(ptCurClick.x-pLastClick.x)<10 && absx(ptCurClick.y-pLastClick.y)<10)
							{
								//::SetForegroundWindow(g_hFromWin);
								::PostMessage(g_hFromWin,WM_USER+5/*DESKTOP_DBLC*/,wParam,0);
								g_DblDownTime=0;
								DEBUG_INFO3("DBL!",0,0,0);
							}else if(iMsgNum==WM_LBUTTONDOWN){
								//::SetForegroundWindow(g_hFromWin);
								::PostMessage(g_hFromWin,WM_USER+13/*DESKTOP_SCLICKD*/,wParam,0);
								DEBUG_INFO3("DOWN!",0,0,0);
							}else if(iMsgNum==WM_LBUTTONUP){
								//::SetForegroundWindow(g_hFromWin);
								::PostMessage(g_hFromWin,WM_USER+12/*DESKTOP_SCLICKU*/,wParam,0);
								DEBUG_INFO3("UP!",0,0,0);
							}
						}
						GetCursorPos(&pLastClick);
					}
				}
			}
		}
	}
	return(CallNextHookEx(g_hhook, nCode, wParam, lParam));
}


LRESULT CALLBACK TestConnect(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (g_hhook2 && lHookActive && g_hFromWin && nCode>=0 && lParam!=0) {
		CWPRETSTRUCT* msg=(CWPRETSTRUCT*)(lParam);
		int iMsgNum=msg->message;
		if(iMsgNum==WM_EXITMENULOOP
			|| iMsgNum==WM_MENUSELECT
			|| iMsgNum==WM_CONTEXTMENU){//iMsgNum==WM_EXITMENULOOP || iMsgNum==WM_UNINITPOPUPMENU || iMsgNum==WM_MENUSELECT || iMsgNum==WM_RBUTTONDOWN || iMsgNum==WM_CONTEXTMENU
			g_skipOneLButton=2;
		}
		if(iMsgNum==LVM_SETEXTENDEDLISTVIEWSTYLE){// || msg->message==LVM_SETTEXTCOLOR//WM_ERASEBKGND
			if(GetTickCount()-dwBGFreshTime>1000){
				dwBGFreshTime=GetTickCount();
				::PostMessage(g_hFromWin,WM_USER+10/*DESKTOP_REFRESH*/,0,0);
			}
		}
	}
	return(CallNextHookEx(g_hhook2, nCode, wParam, lParam));
}

BOOL WINAPI SetHook(HWND hTargetWin, HWND hFromWin)
{
	BOOL fOk = FALSE;
	g_hFromWin=hFromWin;
	g_hTargetWindow=hTargetWin;
	if (hTargetWin != 0) {
		g_WM_SF=::RegisterWindowMessage("WC_SETDFONT");
		g_hhook = NULL;
		DWORD dwThreadId=GetWindowThreadProcessId(hTargetWin,NULL);
		if(dwThreadId){
			g_hhook = SetWindowsHookEx(WH_GETMESSAGE, MyWndProc, g_hinstDll, dwThreadId);//WH_MOUSE
			DWORD dwThreadId2=GetWindowThreadProcessId(GetSysListView(),NULL);
			g_hhook2 = SetWindowsHookEx(WH_CALLWNDPROCRET, TestConnect, g_hinstDll, dwThreadId2);
			lHookActive = 1;
		}
		fOk = (g_hhook != NULL) && (g_hhook2 != NULL);
	} else {
		lHookActive = 0;
		if(g_hhook != NULL){
			fOk = UnhookWindowsHookEx(g_hhook);
			if(fOk){
				g_hhook = 0;
			}
		};
		if(g_hhook2 != NULL){
			fOk = UnhookWindowsHookEx(g_hhook2);
			if(fOk){
				g_hhook2 = 0;
			}
		};
	}
	return(fOk);
}
