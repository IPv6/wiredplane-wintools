// WKInstaScroll.cpp : Defines the entry point for the DLL application.
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <atlbase.h>
#include "WP_InstaScroll.h"
#include "Resource.h"
//#include "..\..\SmartWires\SystemUtils\Macros.h"
#include "../WKplugin.h"
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

#define GHOOKACTION	"WIREKEYS_GHOOKACTIONEVENT"

class SimpleTracker
{
public:
	long* bValue;
	SimpleTracker(long& b){
		bValue=&b;
		InterlockedIncrement(bValue);
	};
	~SimpleTracker(){
		InterlockedDecrement(bValue);
	};
};
#define RETURN_IFRECURSE(x)	static long l##__LINE__=0;if(l##__LINE__>0) return x;SimpleTracker lc##__LINE__(l##__LINE__);


WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

const char* _l(const char* szText)
{
	static char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

class CRect : public tagRECT
{
public:

// Constructors

	// uninitialized rectangle
	CRect()
	{
		left=0;
		top=0;
		right=0;
		bottom=0;
	};

	// from left, top, right, and bottom
	CRect(int l, int t, int r, int b)
	{
		left=l;
		top=t;
		right=r;
		bottom=b;
	}

	// retrieves the width
	int Width() {return right-left;};
	// returns the height
	int Height() {return bottom-top;};
	// returns the size
	// reference to the top-left point
	POINT& TopLeft(){ return *((POINT*)this+1); };
	// reference to the bottom-right point
	POINT& BottomRight(){ return *((POINT*)this); };

	// convert between CRect and LPRECT/LPCRECT (no need for &)
	operator LPRECT() {return (LPRECT)this;}

	// copy from another rectangle
	void CopyRect(LPCRECT lpSrcRect){
		left=lpSrcRect->left;
		top=lpSrcRect->left;
		right=lpSrcRect->left;
		bottom=lpSrcRect->left;
	};
};


HINSTANCE g_hinstDll = NULL;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hinstDll=(HINSTANCE)hModule;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// required funcs
int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>0 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Scrolling options");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
	}else{
		strcpy(stuff->szItemName,"Switch to Full screen");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
	}
	return 1;
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
/*
		RECT rt,rtBackup,rtScreen;
		::GetWindowRect(SpanWindowHwnd,&rt);
		rtBackup.left=rt.left;
		rtBackup.top=rt.top;
		rtBackup.right=rt.right;
		rtBackup.bottom=rt.bottom;
		HWND hParent=GetParent(SpanWindowHwnd);
		if(hParent){
			::GetWindowRect(hParent,&rtScreen);
			// Переводим в клиентские координаты
			POINT pt;
			pt.x=rt.left;
			pt.y=rt.top;
			ScreenToClient(hParent,&pt);
			rt.left=pt.x;
			rt.top=pt.y;
			pt.x=rt.right;
			pt.y=rt.bottom;
			ScreenToClient(hParent,&pt);
			rt.right=pt.x;
			rt.bottom=pt.y;
			rtScreen.right-=rtScreen.left;
			rtScreen.bottom-=rtScreen.top;
			rtScreen.top=0;
			rtScreen.left=0;
		}else{
			rtScreen=GetScreenRect(&rt,NULL);
		}
*/
int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(iPluginFunction==0){
		WKPluginOptions();
	}else{
		POINT pt;
		GetCursorPos(&pt);
		HWND hCurWin=WindowFromPoint(pt);
		if(!hCurWin){
			return 0;
		}
		HWND hRootWnd=GetForegroundWindow();
		DWORD dwFullscr=(DWORD)::GetProp(hRootWnd,"WKP_FULLSCR");
		if(dwFullscr){
			CRect rt2((DWORD)::GetProp(hRootWnd,"WKP_FULLSCRX2"),(DWORD)::GetProp(hRootWnd,"WKP_FULLSCRY2"),(DWORD)::GetProp(hRootWnd,"WKP_FULLSCRX2R"),(DWORD)::GetProp(hRootWnd,"WKP_FULLSCRY2B"));
			MoveWindow(hRootWnd,rt2.left,rt2.top,rt2.Width(),rt2.Height(),TRUE);
			CRect rt((DWORD)::GetProp(hRootWnd,"WKP_FULLSCRX"),(DWORD)::GetProp(hRootWnd,"WKP_FULLSCRY"),(DWORD)::GetProp(hRootWnd,"WKP_FULLSCRXR"),(DWORD)::GetProp(hRootWnd,"WKP_FULLSCRYB"));
			::ScreenToClient(hRootWnd,&rt.TopLeft());
			::ScreenToClient(hRootWnd,&rt.BottomRight());
			SetWindowPos(hCurWin,0,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);
			::RemoveProp(hRootWnd,"WKP_INVIS");
			::RemoveProp(hRootWnd,"WKP_FULLSCR");
		}else{
			DWORD dwStyle=::GetWindowLong(hCurWin,GWL_STYLE);
			if((dwStyle & WS_SIZEBOX)==0){
				return 0;
			}
			CRect rt,rt2,rt3;
			GetClientRect(hCurWin,&rt);
			GetWindowRect(hCurWin,&rt3);
			GetWindowRect(hRootWnd,&rt2);
			::SetProp(hRootWnd,"WKP_FULLSCRX",(LPVOID)rt3.left);
			::SetProp(hRootWnd,"WKP_FULLSCRY",(LPVOID)rt3.top);
			::SetProp(hRootWnd,"WKP_FULLSCRXR",(LPVOID)rt3.right);
			::SetProp(hRootWnd,"WKP_FULLSCRYB",(LPVOID)rt3.bottom);
			::SetProp(hRootWnd,"WKP_FULLSCRX2",(LPVOID)rt2.left);
			::SetProp(hRootWnd,"WKP_FULLSCRY2",(LPVOID)rt2.top);
			::SetProp(hRootWnd,"WKP_FULLSCRX2R",(LPVOID)rt2.right);
			::SetProp(hRootWnd,"WKP_FULLSCRY2B",(LPVOID)rt2.bottom);
			::SetProp(hRootWnd,"WKP_INVIS",(LPVOID)1);
			::SetProp(hRootWnd,"WKP_FULLSCR",(LPVOID)1);
			::ClientToScreen(hCurWin,&rt.TopLeft());
			::ClientToScreen(hCurWin,&rt.BottomRight());
			CRect rtDesktop;
			GetWindowRect(GetDesktopWindow(),&rtDesktop);
			DWORD dwlLDif=rt.left-rtDesktop.left;
			DWORD dwlRDif=rtDesktop.right-rt.right;
			DWORD dwlTDif=rt.top-rtDesktop.top;
			DWORD dwlBDif=rtDesktop.bottom-rt.bottom;
			MoveWindow(hRootWnd,rt2.left-dwlLDif,rt2.top-dwlTDif,rt2.Width()+dwlLDif+dwlRDif,rt2.Height()+dwlTDif+dwlBDif,TRUE);
			rt.CopyRect(rtDesktop);
			SetWindowPos(hCurWin,0,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);
		}
	}
	return 1;
}

CIScrollData hookData;
HINSTANCE hHookDLL=NULL;
typedef BOOL (WINAPI *_InstallHook)(BOOL);
typedef BOOL (WINAPI *_SetOptions)(BOOL,DWORD,DWORD,DWORD);
_InstallHook hProc=NULL;
_SetOptions hOpt=NULL;
HANDLE hHookerEventDone=NULL;
HANDLE hHookerEventStop=0;
DWORD dwHookerThread=0;
WKCallbackInterface* pInter=NULL;
char szHookLibPath[MAX_PATH]="";
CRITICAL_SECTION csMainThread;
DWORD WINAPI GlobalHooker_IScroll(LPVOID pData)
{
	::EnterCriticalSection(&csMainThread);
	// Создаем очередь сообщений
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetEvent(hHookerEventDone);
	while(GetMessage(&msg,0,0,0)>0){
		if(msg.message==WM_ENABLE && msg.wParam!=0){
			hHookDLL = LoadLibrary(szHookLibPath);
			BOOL bRes=FALSE;
			if(hHookDLL){                                                        
				hProc = (_InstallHook)GetProcAddress( hHookDLL,"InstallHook");
				if(hProc){
					bRes=(*hProc)(TRUE);
				}
				hOpt = (_SetOptions)GetProcAddress( hHookDLL, "SetOptions");
				if(hOpt){
					(*hOpt)(TRUE,hookData.dwKeys,hookData.lSmoothness,hookData.lSpeedLimit);
				}
				if(pInter && !bRes){
					pInter->ShowPopup("Failed to initialize Instant Scrolling plugin!","Plugin error",3000);
				}
			}
		}
		if(msg.message==WM_CLOSE || (msg.message==WM_ENABLE && msg.wParam==0)){
			if(hHookDLL){                                                        
				if(hOpt){
					hOpt(FALSE,0,0,0);
				}
				if(hProc){
					hProc(FALSE);
				}
				FreeLibrary(hHookDLL);
				hHookDLL=NULL;
			}
		}
		SetEvent(hHookerEventDone);
		if(msg.message==WM_CLOSE || msg.message==WM_QUIT){
			break;
		}
	}
	::LeaveCriticalSection(&csMainThread);
	SetEvent(hHookerEventStop);
	return 0;
}

#define SAVE_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\WKInstaScroll"
int	WINAPI WKPluginStart()
{
	::InitializeCriticalSection(&csMainThread);
	// Reading from registry...
	hookData.dwKeys=0x10;
	hookData.lSmoothness=20;
	hookData.lSpeedLimit=3;
	CRegKey key;
	if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)==ERROR_SUCCESS && key.m_hKey!=NULL){
		DWORD lSize = sizeof(hookData),dwType=0;
		RegQueryValueEx(key.m_hKey,"ScrollingFlags",NULL, &dwType,(LPBYTE)(&hookData), &lSize);
	}
	if(hookData.dwKeys==0){
		hookData.dwKeys=0x10;
	}
	return 1;
}

HANDLE hHookerThread=0;
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	if(init){
		pInter=init;
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
		pInter->GetWKPluginsDirectory(szHookLibPath,MAX_PATH);
	}
	BOOL bRes=TRUE;
	strcat(szHookLibPath,"lscrhook.dll");
	hHookerEventDone=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerEventStop=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_IScroll, NULL, 0, &dwHookerThread);
	WaitForSingleObject(hHookerEventDone,INFINITE);
	PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
#ifdef _DEBUG
	char s[100]={0};
	sprintf(s,"%s:%i %08x Debug thread point",__FILE__,__LINE__,dwHookerThread);
	init->ShowPopup(s,"Debug thread",0);
#endif
	return 1;
}

int	WINAPI _WKPluginPause(BOOL bPaused)
{
	if(bPaused){
		PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
	}else{
		PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
	}
	::WaitForSingleObject(hHookerEventDone,500);
	return 1;
}

int	WINAPI WKPluginStop()
{
	CRegKey key;
	if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)!=ERROR_SUCCESS){
		key.Create(HKEY_CURRENT_USER, SAVE_REGKEY);
	}
	if(key.m_hKey!=NULL){
		RegSetValueEx(key.m_hKey,"ScrollingFlags",0,REG_BINARY,(BYTE*)(&hookData),sizeof(hookData));
	}
	//--------------
	if(!pInter->getOption(WKOPT_ISSHUTDOWN)){
		PostThreadMessage(dwHookerThread,WM_CLOSE,0,0);
		WaitForSingleObject(hHookerEventStop,2000);
		//::EnterCriticalSection(&csMainThread);
		//::LeaveCriticalSection(&csMainThread);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		TerminateThread(hHookerThread,66);
	}
	::CloseHandle(hHookerThread);
	::CloseHandle(hHookerEventDone);
	::CloseHandle(hHookerEventStop);
	::DeleteCriticalSection(&csMainThread);
	return 1;
}

//----------------------------------------------------------------------
int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{	
	if(iPluginFunction!=0 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bLoadPluginByDefault=TRUE;
	dsc->bResidentPlugin=TRUE;
	//strcpy(dsc->szTitle,"Instant scrolling and fullscreen");
	strcpy(dsc->szDesk,"This plugin allows you to scroll windows using mouse and send any window into fullscreen mode");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LS), BM_SETCHECK, (hookData.dwKeys&H_LS)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LA), BM_SETCHECK, (hookData.dwKeys&H_LA)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LC), BM_SETCHECK, (hookData.dwKeys&H_LC)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LW), BM_SETCHECK, (hookData.dwKeys&H_LW)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RS), BM_SETCHECK, (hookData.dwKeys&H_RS)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RA), BM_SETCHECK, (hookData.dwKeys&H_RA)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RC), BM_SETCHECK, (hookData.dwKeys&H_RC)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RW), BM_SETCHECK, (hookData.dwKeys&H_RW)!=0, 0);
		SetWindowText(GetDlgItem(hwndDlg,IDC_LS),_l("Left shift"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_LA),_l("Left alt"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_LC),_l("Left control"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_LW),_l("Left win"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_RS),_l("Right shift"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_RA),_l("Right alt"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_RC),_l("Right control"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_RW),_l("Right win"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC1),_l("To scroll window click Right mouse and one of this keys"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_STATICS),_l("Smoothness"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_STATICSL),_l("Speed limit"));
		SetWindowText(GetDlgItem(hwndDlg,IDC_STATICDIS),"(c)2005 WiredPlane.com");
		char szTmp[100]="";
		sprintf(szTmp,"%lu",hookData.lSmoothness);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_SMOOTH),szTmp);
		sprintf(szTmp,"%lu",hookData.lSpeedLimit);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_SPEED),szTmp);
	}
	if(uMsg==WM_KEYDOWN){
		if(GetKeyState(VK_CONTROL)<0){
			if(wParam==VK_RETURN){
				uMsg=WM_COMMAND;
				wParam=IDOK;
			}
		}
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		EndDialog(hwndDlg,1);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		hookData.dwKeys=0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LS), BM_GETCHECK, 0, 0)?H_LS:0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LA), BM_GETCHECK, 0, 0)?H_LA:0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LC), BM_GETCHECK, 0, 0)?H_LC:0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LW), BM_GETCHECK, 0, 0)?H_LW:0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RS), BM_GETCHECK, 0, 0)?H_RS:0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RA), BM_GETCHECK, 0, 0)?H_RA:0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RC), BM_GETCHECK, 0, 0)?H_RC:0;
		hookData.dwKeys|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RW), BM_GETCHECK, 0, 0)?H_RW:0;
		char szTmp[100]="";
		::GetWindowText(GetDlgItem(hwndDlg,IDC_SMOOTH),szTmp,sizeof(IDC_SMOOTH));
		hookData.lSmoothness=atol(szTmp);
		if(hookData.lSmoothness<1){
			hookData.lSmoothness=1;
		}
		::GetWindowText(GetDlgItem(hwndDlg,IDC_SPEED),szTmp,sizeof(IDC_SMOOTH));
		hookData.lSpeedLimit=atol(szTmp);
		if(hookData.lSpeedLimit<1){
			hookData.lSpeedLimit=1;
		}
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	RETURN_IFRECURSE(0);
	PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
	WaitForSingleObject(hHookerEventDone,500);
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPTIONS),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
	WaitForSingleObject(hHookerEventDone,500);
	if(pInter){
		pInter->PushMyOptions(0);
	}
	return 1;
}
/*
int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddBoolOption("RestMax","Restore maximized windows before 'LINUX-Like' movements","",TRUE,0);
		pOptionsCallback->AddBoolOption("RClickBack","Move back window when right-click detected during 'LINUX-Like' movement","",TRUE,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
			WaitForSingleObject(hHookerEventDone,500);
		}
		bRestoreMaximized=pOptionsCallback->GetBoolOption("RestMax");
		bMoveBackOnRClick=pOptionsCallback->GetBoolOption("RClickBack");
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
			WaitForSingleObject(hHookerEventDone,500);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("RestMax",bRestoreMaximized);
		pOptionsCallback->SetBoolOption("RClickBack",bMoveBackOnRClick);
	}
	return 1;
}
*/