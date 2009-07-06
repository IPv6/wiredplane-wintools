// WKasLINUXalt.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_asLINUX.h"
#include "Resource.h"
#include "..\..\SmartWires\SystemUtils\Macros.h"
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
#define		H_ANY		0x400
#define GHOOKACTION	"WIREKEYS_GHOOKACTIONEVENT"
HINSTANCE g_hinstDll = NULL;

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

const char* _l(const char* szText)
{
	static char szOut[1024]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}
/*
BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hinstDll=hModule;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
*/
// required funcs
int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>2 || stuff==NULL){
		return 0;
	}
	stuff->dwItemMenuPosition=0;
	stuff->dwDefaultHotkey=0;
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Set new window size");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	}else if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Expand window");
	}else if(iPluginFunction==2){
		strcpy(stuff->szItemName,"Shrink window");
	}
	return 1;
}

BOOL bDragInProcess=0;
CString sPrevText;
CRect rtScreenRect;
CRect rtPrevScrRect;
BOOL DrawScreenshotRect(CRect& Rectangle, const char* szTxt)
{
	CDC dc;
	dc.CreateDC("DISPLAY", NULL, NULL, NULL);
	dc.SetROP2(R2_NOT);
	//dc.DrawFocusRect(&Rectangle);
	CPen pn(PS_SOLID,1,RGB(0,0,0));
	CPen* pn2=dc.SelectObject(&pn);
	CRect rtLines(Rectangle);
	rtLines.NormalizeRect();
	dc.MoveTo(rtLines.right,rtScreenRect.top);
	dc.LineTo(rtLines.right,rtScreenRect.bottom);
	dc.MoveTo(rtLines.left,rtScreenRect.top);
	dc.LineTo(rtLines.left,rtScreenRect.bottom);
	dc.MoveTo(rtScreenRect.left,rtLines.top);
	dc.LineTo(rtScreenRect.right,rtLines.top);
	dc.MoveTo(rtScreenRect.left,rtLines.bottom);
	dc.LineTo(rtScreenRect.right,rtLines.bottom);
	dc.SelectObject(pn2);
	return TRUE;
}

BOOL StartScreenshotRect(CRect& Rectangle, CRect& Screen)
{
	rtScreenRect.CopyRect(Screen);
	rtPrevScrRect.CopyRect(Rectangle);
	DrawScreenshotRect(rtPrevScrRect,"");
	return TRUE;
}

BOOL ContScreenshotRect(CRect& Rectangle, const char* szTxt)
{
	DrawScreenshotRect(rtPrevScrRect,sPrevText);
	rtPrevScrRect.CopyRect(Rectangle);
	sPrevText=szTxt;
	DrawScreenshotRect(rtPrevScrRect,sPrevText);
	return TRUE;
}

BOOL StopScreenshotRect()
{
	if(!rtPrevScrRect.IsRectEmpty()){
		DrawScreenshotRect(rtPrevScrRect,sPrevText);
	}
	rtPrevScrRect.left=rtPrevScrRect.right=rtPrevScrRect.top=rtPrevScrRect.bottom=0;
	sPrevText="";
	return TRUE;
}

BOOL GetNewPosition(CRect& rUserRect)
{
	//BlockInputMy(TRUE);
	CRect rDesktopRECT;
	::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
	HWND wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "CAPT_WND", 0, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.right-rDesktopRECT.left, rDesktopRECT.bottom-rDesktopRECT.top, 0, 0, g_hinstDll, 0);
	if(wndShotPhase2){
		::SetCursor(::LoadCursor(0,MAKEINTRESOURCE(IDC_CROSS)));
		::SetWindowLong(wndShotPhase2,GWL_STYLE,(::GetWindowLong(wndShotPhase2,GWL_STYLE)&(~(WS_CAPTION|WS_BORDER|WS_DLGFRAME|WS_OVERLAPPEDWINDOW))));
		::ShowWindow(wndShotPhase2,SW_SHOWNOACTIVATE);
		::EnableWindow(wndShotPhase2,TRUE);
		::SetCapture(wndShotPhase2);
	}
	//BlockInputMy(FALSE);
	// Ждем пока нажмут гденить
	BOOL bEscaped=FALSE;
	{
		//---------------------------------------------
		GetCursorPos(&rUserRect.TopLeft());
		// Ждем пока отпустят гденить
		GetCursorPos(&rUserRect.BottomRight());
		StartScreenshotRect(rUserRect,rDesktopRECT);
		while(bDragInProcess){
			if(GetAsyncKeyState(VK_ESCAPE)<0){
				bEscaped=TRUE;
				break;
			}
			CPoint pt,pt2(rUserRect.BottomRight());
			GetCursorPos(&pt);
			rUserRect.BottomRight()=pt;
			static BOOL bPrevState=FALSE;
			if(GetAsyncKeyState(VK_RBUTTON)<0){
				if(bPrevState==FALSE){
					bPrevState=TRUE;
					long l=rUserRect.left;
					rUserRect.left=rUserRect.right;
					rUserRect.right=l;
					long l2=rUserRect.top;
					rUserRect.top=rUserRect.bottom;
					rUserRect.bottom=l2;
					SetCursorPos(rUserRect.right,rUserRect.bottom);
				}
			}else{
				bPrevState=FALSE;
			}
			if(pt.x==pt2.x && pt.y==pt2.y){
				Sleep(50);
			}else{
				ContScreenshotRect(rUserRect,NULL);
			}
		}
		// Стираем последний прямоугольник
		StopScreenshotRect();
		// Запоминаем последнюю позицию и убиваем окошко
		GetCursorPos(&rUserRect.BottomRight());
		ReleaseCapture();
		if(wndShotPhase2){
			DestroyWindow(wndShotPhase2);
		}
	}
	if(bEscaped){
		// Спаслись ескейпом :)
		return FALSE;
	}
	return TRUE;
}

BOOL StartWindProcessing(HWND hStartFrom)
{
	// Выясняем новую позицию окна
	CRect rtOld, rtOldClient;
	::GetWindowRect(hStartFrom,&rtOld);
	::GetClientRect(hStartFrom,&rtOldClient);
	if(rtOldClient.Width()==0 || rtOldClient.Height()==0){
		return FALSE;
	}
	CRect rt(rtOld);
	if(!GetNewPosition(rt)){
		return FALSE;
	}
	if(rt.right<rt.left){
		int t=rt.right;
		rt.right=rt.left;
		rt.left=t;
	}
	if(rt.bottom<rt.top){
		int t=rt.bottom;
		rt.bottom=rt.top;
		rt.top=t;
	}
	if(rt==rtOld || rt.Width()<10 || rt.Height()<10){
		return FALSE;
	}
	if(IsZoomed(hStartFrom)){
		::SendMessage(hStartFrom,WM_SYSCOMMAND, (WPARAM) SC_RESTORE, 0);
	}
	::SetWindowPos(hStartFrom,0,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOZORDER);
	return TRUE;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	HWND hWin=stuff->hForegroundWindow;
	if(hWin==NULL){
		WKGetPluginContainer()->ShowAlert("Please, activate window that you want to resize first!",_l("Linux-like windows move"));
		return 0;
	}
	if(iPluginFunction!=0){
		CRect rt;
		GetWindowRect(hWin,&rt);
		if(IsZoomed(hWin)){
			::SendMessage(hWin,WM_SYSCOMMAND, (WPARAM) SC_RESTORE, 0);
		}
		if(rt.Width()!=0){
			#define STEP 40
			double d=rt.Height()/double(rt.Width());
			if(iPluginFunction==1){
				rt.left-=STEP/2;
				rt.right+=STEP/2;
				rt.bottom+=long(d*STEP/2);
				rt.top-=long(d*STEP/2);
			}
			if(iPluginFunction==2){
				rt.left+=STEP/2;
				rt.right-=STEP/2;
				rt.bottom-=long(d*STEP/2);
				rt.top+=long(d*STEP/2);
			}
			MoveWindow(hWin,rt.left,rt.top,rt.Width(),rt.Height(),1);
		}
		return 1;
	}
	if(bDragInProcess){
		bDragInProcess=0;
		return 1;
	}
	char szTitle[1000]="";
	GetWindowText(hWin,szTitle,sizeof(szTitle));
	if(strlen(szTitle)>20){
		szTitle[17]=szTitle[18]=szTitle[19]='.';
		szTitle[20]=0;
	}
	bDragInProcess=1;
	WKGetPluginContainer()->ShowPopup((CString(_l("Drag new rectangle for window"))+"\n")+szTitle,_l("Linux-like windows move"),5000);
	BOOL bRes=StartWindProcessing(hWin);
	bDragInProcess=0;
	return 1;
}

DWORD hookData=0;
long bMoveBackOnRClick=1;
long bRestoreMaximized=1;
HINSTANCE hHookDLL=NULL;
typedef BOOL (WINAPI *_InstallHook)(BOOL);
typedef BOOL (WINAPI *_SetOptions)(BOOL,DWORD);
_InstallHook hProc=NULL;
_SetOptions hOpt=NULL;
HANDLE hHookerEventDone=NULL;
HANDLE hHookerEventStop=NULL;
DWORD dwHookerThread=NULL;
WKCallbackInterface* pInter=NULL;
CRITICAL_SECTION csMainThread;
DWORD WINAPI GlobalHooker_AsLAlt(LPVOID pData)
{
	MSG msg;
	::EnterCriticalSection(&csMainThread);
	// Создаем очередь сообщений
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	::SetEvent(hHookerEventDone);
	while(GetMessage(&msg,0,0,0)>0){
		if(msg.message==WM_ENABLE && msg.wParam!=0){
			BOOL bRes=FALSE;
			if(hHookDLL){                                                        
				if(hProc == 0){
					hProc = (_InstallHook)GetProcAddress( hHookDLL,"InstallHook");
				}
				if(hProc){
					bRes=hProc(TRUE);
				}
				if(hOpt == 0){
					hOpt = (_SetOptions)GetProcAddress( hHookDLL,"SetOptions");
				}
				if(hOpt){
					hOpt(TRUE,hookData|(bMoveBackOnRClick?H_ROLLBACK:0)|((bRestoreMaximized==1)?H_RESTORE:0)|((bRestoreMaximized==2)?H_RESTPROH:0));
				}
				if(pInter && !bRes){
					pInter->ShowPopup("Failed to initialize AsLinuxMove plugin!","Plugin error",3000);
				}
			}
		}
		if(msg.message==WM_CLOSE || (msg.message==WM_ENABLE && msg.wParam==0)){
			if(hHookDLL){                                                        
				if(hOpt){
					hOpt(FALSE,hookData);
				}
				if(hProc){
					hProc(FALSE);
				}
			}
		}
		::SetEvent(hHookerEventDone);
		if(msg.message==WM_CLOSE || msg.message==WM_QUIT){
			break;
		}
	}
	::LeaveCriticalSection(&csMainThread);
	SetEvent(hHookerEventStop);
	return 0;
}

#define SAVE_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\WKasLINUXAlt"
int	WINAPI WKPluginStart()
{
	g_hinstDll=GetModuleHandle("WP_asLINUX.wkp");
	::InitializeCriticalSection(&csMainThread);
	// Reading from registry...
	CRegKey key;
	if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)==ERROR_SUCCESS && key.m_hKey!=NULL){
		DWORD lSize = sizeof(hookData),dwType=0;
		RegQueryValueEx(key.m_hKey,"AsLINUXMoveFlags",NULL, &dwType,(LPBYTE)(&hookData), &lSize);
	}
	if(hookData==0){
		hookData=1;
	}
	return 1;
}

HANDLE hHookerThread=0;
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	DWORD dwThread=0;
	char szLib[MAX_PATH]="";
	if(init){
		pInter=init;
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
		pInter->GetWKPluginsDirectory(szLib,MAX_PATH);
	}
	BOOL bRes=TRUE;
	strcat(szLib,"lalthook.dll");
	hHookDLL = LoadLibrary(szLib);
	hHookerEventDone=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerEventStop=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_AsLAlt, NULL, 0, &dwHookerThread);
	::WaitForSingleObject(hHookerEventDone,1000);
	ResetEvent(hHookerEventDone);
	PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
#ifdef _DEBUG
	char s[100]={0};
	sprintf(s,"%s:%i %08x Debug thread point",__FILE__,__LINE__,dwHookerThread);
	init->ShowPopup(s,"Debug thread",0);
#endif
	return 1;
}

int	WINAPI WKPluginStop()
{
	if(!pInter->getOption(WKOPT_ISSHUTDOWN)){
		PostThreadMessage(dwHookerThread,WM_CLOSE,0,0);
		::WaitForSingleObject(hHookerEventStop,1500);
		::EnterCriticalSection(&csMainThread);
		::LeaveCriticalSection(&csMainThread);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		TerminateThread(hHookerThread,66);
	}
	::CloseHandle(hHookerEventDone);
	::CloseHandle(hHookerEventStop);
	CRegKey key;
	if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)!=ERROR_SUCCESS){
		key.Create(HKEY_CURRENT_USER, SAVE_REGKEY);
	}
	if(key.m_hKey!=NULL){
		RegSetValueEx(key.m_hKey,"AsLINUXMoveFlags",0,REG_BINARY,(BYTE*)(&hookData),sizeof(hookData));
	}
	::DeleteCriticalSection(&csMainThread);
	//FreeLibrary(hHookDLL);
	hHookDLL=NULL;
	::CloseHandle(hHookerThread);
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{	
	if(iPluginFunction>2 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bLoadPluginByDefault=TRUE;
	dsc->bResidentPlugin=TRUE;
	//strcpy(dsc->szTitle,"Linux-like windows move");
	strcpy(dsc->szDesk,"This plugin allows you to move windows in Linux-like style");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	dsc->dwVersionLo=1;
	dsc->dwVersionHi=1;
	return 1;
}

int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LS), BM_SETCHECK, (hookData&H_LS)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LA), BM_SETCHECK, (hookData&H_LA)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LC), BM_SETCHECK, (hookData&H_LC)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LW), BM_SETCHECK, (hookData&H_LW)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RS), BM_SETCHECK, (hookData&H_RS)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RA), BM_SETCHECK, (hookData&H_RA)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RC), BM_SETCHECK, (hookData&H_RC)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_RW), BM_SETCHECK, (hookData&H_RW)!=0, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_IGNORLR), BM_SETCHECK, (hookData&H_ANY)!=0, 0);
		//::SendMessage(GetDlgItem(hwndDlg,IDC_RESTORE), BM_SETCHECK, bRestoreMaximized, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_ROLLBACK), BM_SETCHECK, bMoveBackOnRClick, 0);
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
		hookData=0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LS), BM_GETCHECK, 0, 0)?H_LS:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LA), BM_GETCHECK, 0, 0)?H_LA:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LC), BM_GETCHECK, 0, 0)?H_LC:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_LW), BM_GETCHECK, 0, 0)?H_LW:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RS), BM_GETCHECK, 0, 0)?H_RS:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RA), BM_GETCHECK, 0, 0)?H_RA:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RC), BM_GETCHECK, 0, 0)?H_RC:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RW), BM_GETCHECK, 0, 0)?H_RW:0;
		hookData|=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_IGNORLR), BM_GETCHECK, 0, 0)?H_ANY:0;
		//bRestoreMaximized=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RESTORE), BM_GETCHECK, 0, 0)?H_RESTORE:0;
		bMoveBackOnRClick=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_ROLLBACK), BM_GETCHECK, 0, 0)?H_ROLLBACK:0;
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	RETURN_IFRECURSE(0);
	PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
	::WaitForSingleObject(hHookerEventDone,INFINITE);
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPTIONS),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
	::WaitForSingleObject(hHookerEventDone,INFINITE);
	if(pInter){
		pInter->PushMyOptions(0);
	}
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		char szOpts[1024]={0};
		strcpy(szOpts,_l("Do nothing"));
		strcat(szOpts,"\t");
		strcat(szOpts,_l("Restore window"));
		strcat(szOpts,"\t");
		strcat(szOpts,_l("Do not move window"));
		pOptionsCallback->AddListOption("RestMax","What to do with maximized windows on 'LINUX-Like' movements","",szOpts,1,0);
		pOptionsCallback->AddBoolOption("RClickBack","Move back window when right-click detected during 'LINUX-Like' movement","",TRUE,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
			WaitForSingleObject(hHookerEventDone,1000);
		}
		bRestoreMaximized=pOptionsCallback->GetListOption("RestMax");
		bMoveBackOnRClick=pOptionsCallback->GetBoolOption("RClickBack");
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
			WaitForSingleObject(hHookerEventDone,1000);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetListOption("RestMax",bRestoreMaximized);
		pOptionsCallback->SetBoolOption("RClickBack",bMoveBackOnRClick);
	}
	return 1;
}

char szHotkeyStrPresentation[32]={0};
int	WINAPI WKGetPluginFunctionHints(long iPluginFunction, long lHintCode, void*& pOut)
{
	if(iPluginFunction==0){
		strcpy(szHotkeyStrPresentation,"%16384P-2");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	return 0;
};
