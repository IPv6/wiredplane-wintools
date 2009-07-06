// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_IconsFont.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HHOOK g_hhook;
extern CIconsFontInfo hookData;
extern CIconsFontOptions plgOptions;
extern HINSTANCE g_hinstDll;
extern BOOL	g_bFirstTime;
extern HWND	g_hTargetWin;
extern DWORD g_hTargetThread;
DWORD dwHookerThread=NULL;
HANDLE hHookerEventDone=NULL;
DWORD WINAPI GlobalHooker(LPVOID pData);
WKCallbackInterface* pInter=NULL;
int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define SAVELOGFONT_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\FontMaster"

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}


int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
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
	pInter=init;
	hHookerEventDone=::CreateEvent(NULL,FALSE,FALSE,NULL);
	HANDLE hHookerThread=::CreateThread(NULL, 0, GlobalHooker, NULL, 0, &dwHookerThread);
	WaitForSingleObject(hHookerThread,500);
	CloseHandle(hHookerThread);
	PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	WaitForSingleObject(hHookerEventDone,500);
	return 1;
}

int	WINAPI WKPluginStart()
{
	g_hhook=NULL;
	g_bFirstTime = 0;
	memset(&hookData,0,sizeof(hookData));
	plgOptions.bSetTransp=1;
	hookData.g_FontColor=RGB(255,255,255);
	// Reading from registry...
	CRegKey key;
	if(key.Open(HKEY_CURRENT_USER, SAVELOGFONT_REGKEY)==ERROR_SUCCESS && key.m_hKey!=NULL){
		DWORD lSize = sizeof(hookData),dwType=0;
		RegQueryValueEx(key.m_hKey,"Data",NULL, &dwType,(LPBYTE)(&hookData), &lSize);
		if(hookData.g_Font.lfHeight==0){
			HFONT pFont=(HFONT)::SendMessage(::GetDesktopWindow(), WM_GETFONT, 0, 0);
			::GetObject(pFont,sizeof(hookData.g_Font),&hookData.g_Font);
		}
	}
	return 1;
}

int	WINAPI _WKPluginPause(BOOL bPaused)
{
	if(bPaused){
		PostThreadMessage(dwHookerThread, WM_ENABLE, 0, 0);
	}else{
		PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	}
	::WaitForSingleObject(hHookerEventDone,INFINITE);
	return 1;
}

int	WINAPI WKPluginStop()
{
	if(!isMemEmpty(&hookData.g_Font,sizeof(hookData.g_Font))){
		// Saving to registry...
		CRegKey key;
		if(key.Open(HKEY_CURRENT_USER, SAVELOGFONT_REGKEY)!=ERROR_SUCCESS){
			key.Create(HKEY_CURRENT_USER, SAVELOGFONT_REGKEY);
		}
		if(key.m_hKey!=NULL){
			RegSetValueEx(key.m_hKey,"Data",0,REG_BINARY,(BYTE*)(&hookData),sizeof(hookData));
		}
	}
	//----------------------
	PostThreadMessage(dwHookerThread, WM_CLOSE, 0, 0);
	::WaitForSingleObject(hHookerEventDone,500);
	::CloseHandle(hHookerEventDone);
	return 1;
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction!=0 || stuff==NULL){
		return 0;
	}
	strcpy(stuff->szItemName,"Set desktop items font");
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
	stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	stuff->dwDefaultHotkey=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction!=0 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	WKPluginOptions();
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	OSVERSIONINFOEX winfo; 
	ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
	winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if (winfo.dwPlatformId == VER_PLATFORM_WIN32_NT){
		dsc->bLoadPluginByDefault=TRUE;
	}else{
		dsc->bLoadPluginByDefault=FALSE;
	}
	dsc->bResidentPlugin=TRUE;
	//strcpy(dsc->szTitle,"Desktop font master");
	strcpy(dsc->szDesk,"This plugin allows you to set desktop icons font");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	// Installing fonts and colors into explorer
	PostThreadMessage(dwHookerThread, WM_ENABLE, 0, 0);
	::WaitForSingleObject(hHookerEventDone,500);
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPTIONS),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	// Installing fonts and colors into explorer
	PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	::WaitForSingleObject(hHookerEventDone,500);
	// Pushing changed options back into WireKeys
	if(pInter){
		pInter->PushMyOptions(0);
	}
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddBoolOption("SetFont","Change font of the desktop icons","",TRUE,0);
		pOptionsCallback->AddBoolOption("SetTxColor","Change text color of the desktop icons","",TRUE,0);
		pOptionsCallback->AddBoolOption("SetBgColor","Change background color of the desktop icons","",TRUE,0);
		pOptionsCallback->AddBoolOption("SetIcoTrans","Make desktop icons transparent","",TRUE,OL_DESKICONS);
		pOptionsCallback->AddActionOption("SetFontAction","Choose desktop icons font and color",-1,OL_DESKICONS);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
			WaitForSingleObject(hHookerEventDone,500);
		}
		plgOptions.bSetFont=pOptionsCallback->GetBoolOption("SetFont");
		plgOptions.bSetTransp=pOptionsCallback->GetBoolOption("SetIcoTrans");
		plgOptions.bSetTxColor=pOptionsCallback->GetBoolOption("SetTxColor");
		plgOptions.bSetBgColor=pOptionsCallback->GetBoolOption("SetBgColor");
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
			WaitForSingleObject(hHookerEventDone,500);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("SetFont",plgOptions.bSetFont);
		pOptionsCallback->SetBoolOption("SetIcoTrans",plgOptions.bSetTransp);
		pOptionsCallback->SetBoolOption("SetTxColor",plgOptions.bSetTxColor);
		pOptionsCallback->SetBoolOption("SetBgColor",plgOptions.bSetBgColor);
	}
	return 1;
}