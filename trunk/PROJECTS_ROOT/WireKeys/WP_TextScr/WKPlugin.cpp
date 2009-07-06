// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_TextScr.h"
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

extern HINSTANCE g_hinstDll;
int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>=1 || stuff==NULL){
		return 0;
	}
	strcpy(stuff->szItemName,"Grab text");
	stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
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


BOOL SaveFile(const char* sStartDir, const char* sFileContent)
{
	FILE* m_pFile=fopen(sStartDir,"w+b");
	if(!m_pFile){
		return FALSE;
	}
	DWORD nRead=fwrite(sFileContent,sizeof(char),strlen(sFileContent),m_pFile);
	fclose(m_pFile);
	m_pFile=NULL;
	return (nRead==strlen(sFileContent));
}

CString g_sResult;
CArray<HWND,HWND> g_Windows;
BOOL StartWindProcessing();
int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	HWND hWin=stuff->hCurrentFocusWnd;
	if(hWin==NULL){
		AfxMessageBox("Please, activate window that you want to grab first!");
		return 0;
	}
	g_Windows.RemoveAll();
	g_sResult="Grabbing text:\r\n";
	g_Windows.Add(hWin);
	StartWindProcessing();
	char szPrefFolder[MAX_PATH]="";
	WKGetPluginContainer()->GetPreferencesFolder(szPrefFolder,sizeof(szPrefFolder));
	CString sOurFile=szPrefFolder;
	sOurFile+="text_grab.txt";
	if(g_sResult==""){
		g_sResult="Sorry, no text info found";
	}
	SaveFile(sOurFile,g_sResult);
	::ShellExecute(NULL,"open","notepad.exe",sOurFile,NULL,SW_SHOWNORMAL);
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=FALSE;
	dsc->bLoadPluginByDefault=FALSE;
	//strcpy(dsc->szTitle,"Text grabber");
	strcpy(dsc->szDesk,"This plugin allows you grab all text from the active dialog or window (even when this text can`t be copied)");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	return 1;
}
