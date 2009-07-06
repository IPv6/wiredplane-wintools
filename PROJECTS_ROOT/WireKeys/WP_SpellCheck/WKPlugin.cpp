// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "resource.h"
#include "..\wkplugin.h"
#include "..\..\SmartWires\SystemUtils\myspell\SpellChecker.h"

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

class COptions
{
public:
	COptions(){
		pChecker=0;
		pChecker2=0;
		memset(szIniFilePath,0,sizeof(szIniFilePath));
		memset(szIniFilePath2,0,sizeof(szIniFilePath2));
	}
	CSpellChecker* pChecker;
	char szIniFilePath[MAX_PATH];
	CSpellChecker* pChecker2;
	char szIniFilePath2[MAX_PATH];
	~COptions(){
		if(pChecker){
			delete pChecker;
			pChecker=0;
		}
		if(pChecker2){
			delete pChecker2;
			pChecker2=0;
		}
	}
};

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

int	WINAPI WKPluginStop()
{
	return 1;
}


HINSTANCE g_hinstDll=NULL;
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


int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Download dictionary");
		stuff->dwItemMenuPosition=0;
		stuff->dwDefaultHotkey=-1;
		return 1;
	}
	strcpy(stuff->szItemName,"Check word");
	stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_HOTMENU|WKITEMPOSITION_SELTEXT;
	stuff->dwDefaultHotkey=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(iPluginFunction==1){
		ShellExecute(NULL,"open","http://lingucomponent.openoffice.org/spell_dic.html",NULL,NULL,SW_SHOWNORMAL);
		return 1;
	}
	if(!plgOptions.pChecker){
		WKGetPluginContainer()->ShowAlert(_l("Dictionary not initialized"),_l("Plugin error"));
		return 0;
	}
	char szWord[256]={0};
	WKGetPluginContainer()->GetSelectedText(szWord,sizeof(szWord));
	BOOL bSpelling1=plgOptions.pChecker->CheckSpelling(szWord);
	BOOL bSpelling2=TRUE;
	if(plgOptions.pChecker2){
		bSpelling2=plgOptions.pChecker2->CheckSpelling(szWord);
	}
	char** szSuggestions=0;
	int iSugestionsCount=0;
	if(bSpelling1==FALSE){
		plgOptions.pChecker->CheckSpelling(szWord,szSuggestions,iSugestionsCount);
	}
	if(iSugestionsCount==0 && bSpelling2==FALSE && plgOptions.pChecker2){
		plgOptions.pChecker2->CheckSpelling(szWord,szSuggestions,iSugestionsCount);
	}
	if(iSugestionsCount==0){
		WKGetPluginContainer()->ShowAlert(_l("No suggestions available")+"\n"+_l("Word")+": '"+szWord+"'",_l("Spell checking"));
		return 1;
	}
	DWORD dwRetCode=1;
	if(iSugestionsCount>1){
		CMenu mn;
		mn.CreatePopupMenu();
		for(int i=0;i<iSugestionsCount;i++){
			mn.AppendMenu(MF_STRING, i+1, szSuggestions[i]);
		}
		CPoint pos;
		GetCursorPos(&pos);
		CRect rDesktopRECT;
		//::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
		rDesktopRECT.left=rDesktopRECT.top=-20;
		rDesktopRECT.right=rDesktopRECT.bottom=-10;
		HWND wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_DISABLED, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.Width(), rDesktopRECT.Height(), 0, 0, g_hinstDll, 0);
		//::ShowWindow(wndShotPhase2,SW_SHOWNOACTIVATE);
		dwRetCode=TrackPopupMenu(mn.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pos.x, pos.y, 0, wndShotPhase2 , NULL);
		DestroyWindow(wndShotPhase2);
	}
	if(dwRetCode!=0){
		/*
		if(stuff->hCurrentFocusWnd==0){
			stuff->hCurrentFocusWnd=stuff->hForegroundWindow;
		}*/
		if(stuff && stuff->hCurrentFocusWnd!=0){
			WKGetPluginContainer()->ReturnKeyboardFocusToActiveApp(stuff);
			::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)stuff->hCurrentFocusWnd);
			::SetFocus(stuff->hCurrentFocusWnd);
		}
		WKGetPluginContainer()->PutText(szSuggestions[dwRetCode-1]);
	}
	plgOptions.pChecker->FreeSuggestions(szSuggestions);
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=FALSE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Spell checker");
	strcpy(dsc->szDesk,"This plugin allows you to check spelling of the selected word");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		if(plgOptions.szIniFilePath[0]==0){
			WKGetPluginContainer()->GetWKPluginsDirectory(plgOptions.szIniFilePath,MAX_PATH);
			strcat(plgOptions.szIniFilePath,"dic\\en_US.dic");
		}
		pOptionsCallback->AddTip("tip1","Spelling is based on free component from OpenOffice suite",0);
		pOptionsCallback->AddActionOption("tip2","Click here to download more dictionaries",1,0);
		pOptionsCallback->AddStringOption("IniFile","Dictionary","",plgOptions.szIniFilePath,1);
		pOptionsCallback->AddStringOption("IniFile2","Alternative dictionary","",plgOptions.szIniFilePath2,1);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(iAction==OM_OPTIONS_SET){
			if(plgOptions.pChecker){
				delete plgOptions.pChecker;
				plgOptions.pChecker=0;
			}
			if(plgOptions.pChecker2){
				delete plgOptions.pChecker2;
				plgOptions.pChecker2=0;
			}
		}
		if(pOptionsCallback){
			pOptionsCallback->GetStringOption("IniFile",plgOptions.szIniFilePath,sizeof(plgOptions.szIniFilePath));
			pOptionsCallback->GetStringOption("IniFile2",plgOptions.szIniFilePath2,sizeof(plgOptions.szIniFilePath2));
		}
		if(!plgOptions.pChecker){
			if(plgOptions.szIniFilePath[0]==0){
				strcat(plgOptions.szIniFilePath,"dic\\en_US.dic");
			}
			CString sAff=plgOptions.szIniFilePath;
			sAff.Replace(".dic",".aff");
			plgOptions.pChecker=new CSpellChecker(sAff,plgOptions.szIniFilePath);
		}
		if(!plgOptions.pChecker2 && plgOptions.szIniFilePath2[0]!=0){
			CString sAff=plgOptions.szIniFilePath2;
			sAff.Replace(".dic",".aff");
			plgOptions.pChecker2=new CSpellChecker(sAff,plgOptions.szIniFilePath2);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetStringOption("IniFile",plgOptions.szIniFilePath);
		pOptionsCallback->SetStringOption("IniFile2",plgOptions.szIniFilePath2);
	}
	return 1;
}
