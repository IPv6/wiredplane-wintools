// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "resource.h"
#include "..\wkplugin.h"
#include "..\..\SmartWires\SystemUtils\Macros.h"
#include "..\..\SmartWires\SystemUtils\hotkeys.h"

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
		memset(szIniFilePath,0,sizeof(szIniFilePath));
		memset(szIniFilePath2,0,sizeof(szIniFilePath2));
	}
 char szIniFilePath[MAX_PATH];
 char szIniFilePath2[MAX_PATH];
	~COptions(){
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
	if(iPluginFunction>0 || stuff==NULL){
		return 0;
	}
	strcpy(stuff->szItemName,"Close console");
	stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	stuff->dwItemMenuPosition=0;
	stuff->dwDefaultHotkey=0;
	return 1;
}

char szHotkeyStrPresentation[32]={0};
int	WINAPI WKGetPluginFunctionHints(long iPluginFunction, long lHintCode, void*& pOut)
{
	if(iPluginFunction==0){
		strcpy(szHotkeyStrPresentation,"%0~-27,486");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	return 0;
};

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>0 || stuff==NULL){
		return 0;
	}
	return 1;
}



bool PatternMatch(const char* s, const char* mask)
{
	const   char* cp=0;
	const   char* mp=0;
	for (; *s&&*mask!='*'; mask++,s++){
		if (*mask!=*s && *mask!='?'){
			return 0;
		}
	}
	for (;;) {
		if (!*s){
			while (*mask=='*'){
				mask++;
			}
			return !*mask;
		}
		if (*mask=='*'){ 
			if (!*++mask){
				return 1;
			}
			mp=mask;
			cp=s+1;
			continue;
		}
		if (*mask==*s||*mask=='?'){
			mask++;
			s++;
			continue;
		}
		mask=mp;
		s=cp++;
	}
	return true;
}

void SkipBlanks(const char*& szPos)
{
	while(*szPos==' ' || *szPos=='\t' || *szPos=='\r' || *szPos=='\n'){
		szPos++;
	}
}

bool PatternMatchList(const char* s, const char* mask)
{
	if(mask==NULL || *mask==NULL){
		return false;
	}
	if(s==NULL || *s==NULL){
		return false;
	}
	const char* szPos=mask;
	SkipBlanks(szPos);
	if(szPos==NULL || *szPos==NULL){
		return false;
	}
	while(szPos!=NULL && *szPos!=0){
		SkipBlanks(szPos);
		const char* szPos2=strpbrk(szPos,";");
		if(szPos2==NULL){
			szPos2=szPos+(strlen(szPos));
		}
		if(szPos2<=szPos){
			break;
		}
		char szMask[256];
		memset(szMask,0,sizeof(szMask));
		memcpy(szMask,szPos,size_t(szPos2-szPos));
		if(szMask[0]!=0){
			const char* szPattern=szMask;
			SkipBlanks(szPattern);
			if(PatternMatch(s,szPattern)){
				return true;
			}
		}
		szPos=szPos2;
		if(*szPos==';'){
			szPos++;
		}
	}
	return false;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(stuff && stuff->hForegroundWindow){
		char szWindowClass[128]={0};
		char szWindowTitle[128]={0};
		::GetClassName(stuff->hForegroundWindow,szWindowClass,sizeof(szWindowClass)-1);
		::GetWindowText(stuff->hForegroundWindow,szWindowTitle,sizeof(szWindowTitle)-1);
		if(plgOptions.szIniFilePath2[0]!=0){
			if(PatternMatchList(szWindowTitle,plgOptions.szIniFilePath2)){
   				return 1;
			}
		}
		if(stricmp(szWindowClass,"consolewindowclass")==0){
			PostMessage(stuff->hForegroundWindow, WM_SYSCOMMAND, (WPARAM) SC_CLOSE, 0);
		}else if(plgOptions.szIniFilePath[0]!=0){
			if(PatternMatchList(szWindowTitle,plgOptions.szIniFilePath)){
				PostMessage(stuff->hForegroundWindow, WM_SYSCOMMAND, (WPARAM) SC_CLOSE, 0);
			}
		}
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=FALSE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Close Console windows by ESC");
	strcpy(dsc->szDesk,"This plugin allows you to close console windows by pressing ESC key");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		//pOptionsCallback->AddTip("tip1","Spelling is based on free component from OpenOffice suite",0);
		if(!WKGetPluginContainer()->getOption(WKOPT_EXTHKPROC)){
			pOptionsCallback->AddTipEx("warn_exh","Warning: extended hotkey processing is not enabled. Some features of this plugin are disabled",0);
		}
		pOptionsCallback->AddStringOption("masktoo","Handle this windows too","Type windows title to be closed by ESC key too. Use wildcards, delimit titles with ';' character",_l("*Notepad*"),1);
		pOptionsCallback->AddStringOption("maskno","Do not handle this windows","Windows with matched titles will be ignored. Use wildcards, delimit titles with ';' character",_l("*Far*"),1);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(pOptionsCallback){
			pOptionsCallback->GetStringOption("masktoo",plgOptions.szIniFilePath,sizeof(plgOptions.szIniFilePath));
			pOptionsCallback->GetStringOption("maskno",plgOptions.szIniFilePath2,sizeof(plgOptions.szIniFilePath2));
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetStringOption("masktoo",plgOptions.szIniFilePath);
		pOptionsCallback->SetStringOption("maskno",plgOptions.szIniFilePath2);
	}
	return 1;
}
