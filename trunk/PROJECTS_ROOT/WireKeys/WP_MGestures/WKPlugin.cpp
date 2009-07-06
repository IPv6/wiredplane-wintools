// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_MGestures.h"
#include "HookCode.h"

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

CString _pl(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	char szPath[MAX_PATH]={0};
	WKGetPluginContainer()->GetWKPluginsDirectory(szPath,sizeof(szPath));
	strcat(szPath,"wp_mgestures\\gestures_help.htm");		
	WinExec(CString("explorer.exe \"")+szPath+"\"",1);
	return 1;
}

int WINAPI onAddEvent(char const* szEventID)
{
	if(szEventID && strcmp(szEventID,"MOG0")==0){
		WKPluginOptions(0);
		return 1;
	}
	return 0;
}

CMLNet Recognizer;
HINSTANCE g_hinstDll=NULL;
HANDLE hHookerThread=NULL;
CRITICAL_SECTION csMainThread;
WKCallbackInterface* pInter=NULL;
DWORD WINAPI GlobalHooker_Gestures(LPVOID pData);
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	// Version...
	char szVer[32]="";
	init->GetWKVersion(szVer);
	DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
	if(dwBuild<348){
		init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
		return 0;
	}
	pInter=init;
	char szPath[MAX_PATH]={0};
	pInter->GetWKPluginsDirectory(szPath,sizeof(szPath));
	strcat(szPath,"wp_mgestures\\wp_mgestures.cfg");
	if(!Recognizer.LoadNet(szPath)){
		char szError[MAX_PATH+200]={0};
		sprintf(szError,"Failed to find file '%s'",szPath);
		init->ShowAlert(szError,"Plugin error");
		return 0;
	}
	char szClass2[16]={0},szHelp[64]="";
	strcpy(szClass2,_pl("Mouse gestures"));
	strcpy(szHelp,_pl("Show help"));
	// Режимы
	WKGetPluginContainer()->RegisterEvent("MOG0",szHelp,szClass2);
	for(int i='A';i<=BYTE('\\')+1;i++){
		char szEvent[5]="MOGA";
		szEvent[3]=i;
		strcpy(szHelp,"@");
		strcat(szHelp,_pl(CMLNet::getPatternName(i-'A')));
		WKGetPluginContainer()->RegisterEvent(szEvent,szHelp,szClass2);
	}
	WKGetPluginContainer()->RegisterOnAddRemoveEventCallback(onAddEvent,0);
	return 1;
}

int	WINAPI WKPluginStart()
{
	::InitializeCriticalSection(&csMainThread);
	return 1;
}

int	WINAPI WKPluginStop()
{
	WKGetPluginContainer()->UnRegisterEvent("MOG0");
	for(int i='A';i<='\\';i++){
		char szEvent[5]="MOGA";
		szEvent[3]=i;
		WKGetPluginContainer()->UnRegisterEvent(szEvent);
	}
	::DeleteCriticalSection(&csMainThread);
	return 1;
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>0 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Start/Stop gesture");
	}
	stuff->hItemBmp=0;
	stuff->dwItemMenuPosition=0;
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

extern BOOL bNotStop;
extern BOOL bGestureInAction;
int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(iPluginFunction>0 || stuff==NULL){
		return 0;
	}
	bNotStop++;
	BOOL bUnPress=stuff?stuff->bOnUnpress:0;
	if(iPluginFunction==0){
		if(bGestureInAction==0 && !bUnPress){
			bGestureInAction=1;
			DWORD dwHookerThread=0;
			hHookerThread=::CreateThread(NULL, 0, GlobalHooker_Gestures, NULL, 0, &dwHookerThread);
			::CloseHandle(hHookerThread);
			while(bGestureInAction!=2){
				Sleep(20);
			}
		}
	}
	return 1;
}

BOOL bShowInOSD=1;
BOOL bShowTrail=1;
BOOL bStepTime=2;
int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Mouse Gesture");
	strcpy(dsc->szDesk,"Adds mouse gestures. Uses recognition engine by Konstantin Boukreev");
	strcpy(dsc->szAuthors,"Ilja Razinkov, @2008");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	dsc->dwVersionHi=1;
	dsc->dwVersionLo=1;
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddTip("tip1","To add gesture to hotkey, start edit hotkey, open context menu and choose desired gesture from submenu",0);
		pOptionsCallback->AddTip("tip2","You can add more than one gesture. To activate such hotkey draw one gesture after another without delay",0);
		pOptionsCallback->AddBoolOption("showOsd","Show detected gesture in OSD","",1,0);
		pOptionsCallback->AddBoolOption("showTrail","Show trail while drawing","",1,0);
		//pOptionsCallback->AddNumberOption("PauseLevel","Maximum delay between gestures to combine them into 'gesture word', seconds","",2,0,100,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		bShowInOSD=pOptionsCallback->GetBoolOption("showOsd");
		bShowTrail=pOptionsCallback->GetBoolOption("showTrail");
		//bStepTime=pOptionsCallback->GetNumberOption("PauseLevel");
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("showOsd",bShowInOSD);
		pOptionsCallback->SetBoolOption("showTrail",bShowTrail);
		//pOptionsCallback->SetNumberOption("PauseLevel",bStepTime);
	}
	return 1;
}

char szHotkeyStrPresentation[32]={0};
int	WINAPI WKGetPluginFunctionHints(long iPluginFunction, long lHintCode, void*& pOut)
{
	if(iPluginFunction==0){
		strcpy(szHotkeyStrPresentation,"%4P-2");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	return 0;
};
