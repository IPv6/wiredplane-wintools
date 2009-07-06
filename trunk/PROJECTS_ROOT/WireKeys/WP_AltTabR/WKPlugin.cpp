// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_AltTabR.h"
#include "HookCode.h"
#include "DLG_TaskHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef BOOL (WINAPI *_IsHungAppWindow)(HWND hw);
BOOL MyIsHungAppWindow(HWND hw)
{
	BOOL bRes=0;
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	if(hUser32){
		_IsHungAppWindow fp=(_IsHungAppWindow)GetProcAddress(hUser32,"IsHungAppWindow");
		if(fp){
			bRes=(*fp)(hw);
		}
		FreeLibrary(hUser32);
	}
	return bRes;
}

void GetWindowTopParent(HWND& hWnd)
{
	if(!hWnd){
		return;
	}
	//hWnd=::GetAncestor(hWnd,GA_ROOT);
	HWND hWnd2=hWnd;
	while(hWnd2!=NULL){
		hWnd=hWnd2;
		hWnd2=::GetWindow(hWnd,GW_OWNER);
		if(!hWnd2){
			hWnd2=::GetParent(hWnd);
		}
	}
	return;
}

long lGlobalLock=0;
extern long bOnCtrl;
extern int g_ButtonActive;
extern long g_LockCBT;
extern long g_ActionInProgres;
extern long bShowKill;
extern long bShowKillMC;
extern long bKillClose;
extern long bSuggestKill;
extern long bDragos;
extern long bMakeSq;
extern long iSqsize;
extern POINT g_pt;
extern HWND hWndControl;
extern HWND hWndButton;
extern DWORD g_NextMsg;
extern DWORD g_KillMsg;
extern POINT g_ptClient;
extern BOOL g_bToolBar;
extern HHOOK g_hhook;
extern HHOOK g_hhookM;
extern long	g_HookLock;
//extern HINSTANCE g_hinstDll;
extern DWORD dwHookerThread;
DWORD dwHookerThread2=0;
HANDLE hStopEvent=0;
HANDLE hHookerThread=NULL;
HANDLE hHookerThread2=NULL;
CRITICAL_SECTION csMainThread;
CString _l(const char* szText);
HWND MyFindWindow(const char* szClass, const char* szTitle,  BOOL bUseRemembered);
LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam);
WKCallbackInterface*& WKGetPluginContainer();
DWORD g_globalAction=0;
long lOneActionTime=0;
DWORD WINAPI ProcessWnd(LPVOID p)
{
	if(lOneActionTime>0){
		return 1;
	}
	SimpleTracker lc2(g_LockCBT);
	SimpleTracker lc(lOneActionTime);
	HWND hNewWnd=(HWND)p;
	if(g_globalAction==0 || hNewWnd==0){
		return 1;
	}
	if(g_globalAction==100){
		char szT[30]={0};
		GetWindowText(hNewWnd,szT,sizeof(szT));
		if(hNewWnd==0 || strlen(szT)==0){
#ifdef _DEBUG
			CString s;
			s.Format("Failed to detect %x ('%s', %i)",hNewWnd,szT,IsWindow(hNewWnd));
			WKGetPluginContainer()->ShowBaloon(s,"AltTabRepl",9000);
#endif
			WKGetPluginContainer()->Sound(0);
		}else{
			BOOL bProceed=bKillClose;
			if(bSuggestKill){
				SimpleTracker lc(g_HookLock);
				if(bKillClose){
					if(AfxMessageBox(((_l("Kill this application")+"?")+"\r\n")+szT,MB_YESNO|MB_ICONQUESTION)!=IDYES){
						bProceed=-1;
					}
				}else{
					if(MyIsHungAppWindow(hNewWnd)){
						if(AfxMessageBox(_l("Application is not responding")+". "+_l("Kill this application")+"?"+"\r\n"+szT,MB_YESNO|MB_ICONQUESTION)!=IDYES){
							bProceed=-1;
						}
						bProceed=1;
					}
				}
			}
			GetWindowTopParent(hNewWnd);
			if(bProceed==1){
				WKGetPluginContainer()->ProcessWindow(hNewWnd,12);
			}
			if(bProceed==0){
				static HWND hPrevHwnd=0;
				static long lPrevCoun=0;
				if(hNewWnd==hPrevHwnd){
					lPrevCoun++;
				}else{
					lPrevCoun=0;
				}
				BOOL bSendClose=1;
				if(lPrevCoun>3){
					if(AfxMessageBox(_l("Application is ignoring 'Close' command")+". "+_l("Kill this application")+"?"+"\r\n"+szT,MB_YESNO|MB_ICONQUESTION)==IDYES){
						WKGetPluginContainer()->ProcessWindow(hNewWnd,12);
						bSendClose=0;
						lPrevCoun=0;
					}
				}
				if(bSendClose){
					hPrevHwnd=hNewWnd;
					PostMessage(hNewWnd,WM_SYSCOMMAND,SC_CLOSE,0);
				}
			}
		}
	}else if(g_globalAction==1 || g_globalAction==3){
		WKGetPluginContainer()->ProcessWindow(hNewWnd,15);
	}else{
		WKGetPluginContainer()->ProcessWindow(hNewWnd,14);
	}
	g_globalAction=0;
	g_ButtonActive=-1;
	if(bMakeSq){
		SetSq();
	}
	return 0;
}

void SetSq()
{
	if(g_bToolBar){
		SendMessage(hWndControl,TB_SETBUTTONWIDTH,0,MAKELONG (bMakeSq, bMakeSq));
	}else{
		int iCount=TabCtrl_GetItemCount(hWndControl);
		TabCtrl_SetMinTabWidth(hWndControl,bMakeSq);
		for(int i=0;i<iCount;i++){
			RECT btRect;
			TabCtrl_GetItemRect(hWndControl,i,&btRect);
			TabCtrl_SetItemSize(hWndControl,bMakeSq,btRect.bottom-btRect.top);
			break;
		}
	}
}

DWORD WINAPI GlobalHooker_TBExtend(LPVOID pData)
{
	//TRACE1("[DEBUG THREAD: STARING %i]",GetCurrentThreadId());
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	hWndControl=0;
	g_bToolBar=FALSE;
	HWND hWnd1 = ::FindWindow("Shell_TrayWnd",NULL);
	if(hWnd1 == 0){
		return 0;
	}
	hWndControl = FindWindowEx(hWnd1,0,"SysTabControl32",NULL); 
	if(hWndControl==0){
		HWND hWnd2 = FindWindowEx(hWnd1,0,"ReBarWindow32",NULL); 
		if(hWnd2 == 0){
			return 0;
		}
		HWND hWnd3 = FindWindowEx(hWnd2,0,"MSTaskSwWClass",NULL); 
		if(hWnd3 == 0){
			return 0;
		}
		hWndControl = FindWindowEx(hWnd3,0,"SysTabControl32",NULL);
		if(hWndControl == 0){
			hWndControl = FindWindowEx(hWnd3,0,"ToolbarWindow32",NULL);
			g_bToolBar=TRUE;
		}
	}
	if(hWndControl == 0){
		return 0;
	}
	{// Область видимости окна
		CWnd dummy;
		dummy.Attach(hWndControl);
		CDLG_TaskHelper* infoWindow=new CDLG_TaskHelper();
		if(!infoWindow){
			return 99;
		}
		infoWindow->Create(CDLG_TaskHelper::IDD,0);//&dummy
		hWndButton=infoWindow->GetSafeHwnd();
		if(IsWindow(hWndButton)){
			infoWindow->EnableWindow(TRUE);
			infoWindow->ShowWindow(FALSE);
			//infoWindow->SetParent(&dummy);
		}
		DWORD pID = 0;
		DWORD dwThreadId=GetWindowThreadProcessId(hWndControl,&pID);
		char szDLLPath[MAX_PATH]={0};GetModuleFileName(WKGetPluginContainer()->GetPluginDllInstance(),szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
		g_hhook = SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndRetProc, hDLL, dwThreadId);
		DWORD pID2 = 0;
		DWORD dwThreadId2=GetWindowThreadProcessId(hWnd1,&pID2);
		g_hhookM = SetWindowsHookEx(WH_MOUSE, MouseProc, hDLL, dwThreadId);
		while(GetMessage(&msg,0,0,0)>0){
			if(msg.message==WM_CLOSE){
				break;
			}
			if(msg.message==WM_PAINT && msg.hwnd==hWndButton){
				::ValidateRect(hWndButton,0);
			}
			if(msg.message==WM_USER && lOneActionTime==0){
				HWND hOld=0,hNewWnd=0;
				g_globalAction=msg.wParam;
				hNewWnd=(HWND)msg.lParam;
				/*
				if(msg.wParam>1){
					hNewWnd=(HWND)msg.lParam;
				}else{
					hOld=(HWND)msg.lParam;
					DWORD dwCount=GetTickCount();
					while(GetTickCount()-dwCount<1500 && (hNewWnd==0 || hNewWnd==hOld)){
						hNewWnd=GetForegroundWindow();
					}
				}
				*/
				FORK(ProcessWnd,hNewWnd);
			}else{
				::DispatchMessage(&msg);
			}
		}
		infoWindow->SetParent(0);
		delete infoWindow;
		dummy.Detach();
	}
	UnhookWindowsHookEx(g_hhookM);
	UnhookWindowsHookEx(g_hhook);
	return 0;
}

int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(init){
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
	g_NextMsg=RegisterWindowMessage("WK_NEX_WND");
	g_KillMsg=RegisterWindowMessage("WK_KIL_WND");
	::InitializeCriticalSection(&csMainThread);
	hStopEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_TBExtend, NULL, 0, &dwHookerThread);
	PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
	return 1;
}

int	WINAPI WKPluginStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return 1;
}

int	WINAPI WKPluginStop()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(!WKGetPluginContainer()->getOption(WKOPT_ISSHUTDOWN)){
		PostThreadMessage(dwHookerThread,WM_CLOSE,0,0);
		//PostThreadMessage(dwHookerThread2,WM_CLOSE,0,0);
		::WaitForSingleObject(hHookerThread,2000);
		//::EnterCriticalSection(&csMainThread);
		//::LeaveCriticalSection(&csMainThread);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		//TerminateThread(hHookerThread,66);
		//SuspendThread(hHookerThread);
	}
	::DeleteCriticalSection(&csMainThread);
	::CloseHandle(hHookerThread);
	//::CloseHandle(hHookerThread2);
	return 1;
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction>=2 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Next window");
		stuff->hItemBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON1));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_MISC;
		stuff->dwDefaultHotkey=0;
	}else{
		strcpy(stuff->szItemName,"Previous window");
		stuff->hItemBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON2));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_MISC;
		stuff->dwDefaultHotkey=0;
	}
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction>2 || stuff==NULL){
		return 0;
	}
	return 1;
}

#define MPOINT_X(aa)	long(65535*aa.x/(rtDesktop.right-rtDesktop.left))
#define MPOINT_Y(aa)	long(65535*aa.y/(rtDesktop.bottom-rtDesktop.top))
typedef BOOL (WINAPI *_BlockInput)(BOOL);
BOOL BlockInputMy(BOOL bBlock)
{
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	_BlockInput fp=NULL;
	if(hUser32){
		fp=(_BlockInput)GetProcAddress(hUser32,"BlockInput");
	}
	if(hUser32 && fp){
		return (*fp)(bBlock);
	}
	return FALSE;
}

void SwitchThroughMouseClick(POINT& pt)
{
	HWND hGetForeg=GetForegroundWindow();
#ifdef _DEBUG
	{
		char s[100]={0};
		GetWindowText(hGetForeg,s,100);
		//FLOG2("Brefore drop wt:%i, %s",hGetForeg,s);
	}
#endif
	RECT rtDesktop;
	::GetWindowRect(::GetDesktopWindow(),&rtDesktop);
	POINT pt2;
	GetCursorPos(&pt2);
	::mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,MPOINT_X(pt),MPOINT_Y(pt),0,0);
	Sleep(300);
	::mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
	Sleep(10);
	::mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
	HWND hGetForeg2=GetForegroundWindow();
	int iCount=100;
	while(hGetForeg2==hGetForeg && iCount){
		Sleep(10);
		hGetForeg2=GetForegroundWindow();
		iCount--;
	}
	::mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,MPOINT_X(pt2),MPOINT_Y(pt2),0,0);
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(lGlobalLock>0){
		return 0;
	}
	SimpleTracker lc(lGlobalLock);
	WKGetPluginContainer()->WaitWhileAllKeysAreReleased();
	g_pt.x=0;
	g_pt.y=0;
	HWND hWnd1=::FindWindow("Shell_TrayWnd",NULL);
	SendMessage(hWndControl,g_NextMsg,0,iPluginFunction);
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hWnd1);
	if(g_pt.x!=0 && g_pt.y!=0){
		RECT rt1;
		RECT rt2;
		RECT rt3;
		DWORD bOnTop=(GetWindowLong(hWnd1, GWL_EXSTYLE)&WS_EX_TOPMOST)!=0;
		::GetWindowRect(hWnd1,&rt2);
		::GetWindowRect(::GetDesktopWindow(),&rt1);
		rt3.left=rt2.left;
		rt3.right=rt2.right;
		rt3.top=rt2.top;
		rt3.bottom=rt2.bottom;
		if(g_pt.x<0 || g_pt.y<0 || g_pt.x>rt1.right || g_pt.y>rt1.bottom){
			if(g_pt.x<0 || g_pt.y<0){
				if(g_pt.x<0){
					g_pt.x=(g_pt.x-rt2.left);
				}
				if(g_pt.y<0){
					g_pt.y=(g_pt.y-rt2.top);
				}
				rt3.left=0;
				rt3.top=0;
				rt3.right=rt2.right-rt2.left;
				rt3.bottom=rt2.bottom-rt2.top;
			}else if(g_pt.x>rt1.right || g_pt.y>rt1.bottom){
				if(g_pt.x>rt1.right){
					g_pt.x=(g_pt.x-(rt2.right-rt2.left));
					rt3.right=rt2.left;
					rt3.bottom=rt2.bottom;
					rt3.top=rt2.top;
					rt3.left=rt2.left-(rt2.right-rt2.left);
				}
				if(g_pt.y>rt1.bottom){
					g_pt.y=(g_pt.y-(rt2.bottom-rt2.top));
					rt3.left=rt2.left;
					rt3.right=rt2.right;
					rt3.bottom=rt2.top;
					rt3.top=rt2.top-(rt2.bottom-rt2.top);
				}
			}
		}
		::SetWindowPos(hWnd1,HWND_TOPMOST,rt3.left,rt3.top,rt3.right-rt3.left,rt3.bottom-rt3.top,SWP_SHOWWINDOW);
		SwitchThroughMouseClick(g_pt);
		::SetWindowPos(hWnd1,bOnTop?HWND_TOPMOST:HWND_NOTOPMOST,rt2.left,rt2.top,rt2.right-rt2.left,rt2.bottom-rt2.top,0);
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Taskbar Extender");
	strcpy(dsc->szDesk,"This plugin allows you to drag-n-drop buttons on taskbar or activate next/previous application in the TaskBar`s order, regardless of the previously active application");
	dsc->hPluginBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddBoolOption("ctrl","Activate plugin only when 'Control' key is down","",0,0);
		pOptionsCallback->AddBoolOption("dragos","Allow draging outside taskbar","",1,0);
		pOptionsCallback->AddBoolOption("killMC","Close window by middle-clicking taskbar button","",1,0);
		pOptionsCallback->AddBoolOption("kill","Add 'Close' icon to taskbar buttons","",1,0);
		pOptionsCallback->AddNumberOption("sqsize","Space between 'Close' icon and button borders","",4,-25,25,0);
		pOptionsCallback->AddBoolOption("close","Kill application on 'Close' icon instead of closing them","",0,0);
		pOptionsCallback->AddBoolOption("killsug","Always suggest to kill 'Not-responding' application","",1,0);
		//pOptionsCallback->AddNumberOption("tabsq","Set taskbar buttons width to ... pixels","",0,0,999,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		bShowKill=pOptionsCallback->GetBoolOption("kill");
		iSqsize=pOptionsCallback->GetNumberOption("sqsize");
		bShowKillMC=pOptionsCallback->GetBoolOption("killMC");
		bOnCtrl=pOptionsCallback->GetBoolOption("ctrl");
		bKillClose=pOptionsCallback->GetBoolOption("close");
		bSuggestKill=pOptionsCallback->GetBoolOption("killsug");
		bDragos=pOptionsCallback->GetBoolOption("dragos");
		//bMakeSq=pOptionsCallback->GetNumberOption("tabsq");
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("kill",bShowKill);
		pOptionsCallback->SetNumberOption("sqsize",iSqsize);
		pOptionsCallback->SetBoolOption("killMC",bShowKillMC);
		pOptionsCallback->SetBoolOption("ctrl",bOnCtrl);
		pOptionsCallback->SetBoolOption("close",bKillClose);
		pOptionsCallback->SetBoolOption("killsug",bSuggestKill);
		pOptionsCallback->SetBoolOption("dragos",bDragos);
		//pOptionsCallback->SetNumberOption("tabsq",bMakeSq);
	}
	return 1;
}

