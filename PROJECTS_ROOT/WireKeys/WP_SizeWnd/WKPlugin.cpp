// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_SizeWnd.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

CString _l(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

COptions plgOptions;
int	WINAPI WKPluginStart()
{
	if(WKGetPluginContainer()){
		// Version...
		char szVer[32]="";
		WKGetPluginContainer()->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			WKGetPluginContainer()->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
	return 1;
}

int	WINAPI _WKPluginPause(BOOL bPaused)
{
	return 1;
}

int	WINAPI WKPluginStop()
{
	return 1;
}

//extern HINSTANCE g_hinstDll;
int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>2 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Resize active window");
		stuff->hItemBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
	}else if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Make window larger");
	}else if(iPluginFunction==2){
		strcpy(stuff->szItemName,"Make window smaller");
	}
	stuff->dwDefaultHotkey=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>2 || stuff==NULL){
		return 0;
	}
	return 1;
}

BOOL StartWindProcessing(HWND hStartFrom);
int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	HWND hWin=stuff->hForegroundWindow;
	if(hWin==NULL){
		AfxMessageBox("Please, activate window that you want to resize first!");
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
	char szTitle[1000]="";
	GetWindowText(hWin,szTitle,sizeof(szTitle));
	if(strlen(szTitle)>20){
		szTitle[17]=szTitle[18]=szTitle[19]='.';
		szTitle[20]=0;
	}
	WKGetPluginContainer()->ShowPopup((CString(_l("Drag new rectangle for window"))+"\n")+szTitle,_l("Window`s resizer"),5000);
	BOOL bRes=StartWindProcessing(hWin);
	WKGetPluginContainer()->ShowPopup((CString(bRes?_l("Window resized succesfully"):_l("Window is not resized"))+"\n")+szTitle,_l("Window`s resizer"),5000);
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=FALSE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Window`s resizer");
	strcpy(dsc->szDesk,"This plugin allows you to resize any window regardless of window type");
	dsc->hPluginBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	return 1;
}
