#include "stdafx.h"
#include <atlbase.h>
#include "resource.h"
#include "..\wkplugin.h"
#include "..\..\SmartWires\SystemUtils\SystemInfo.h"

long g_bShowMS=2000;
long g_bShowCPU=0;
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

#define HIST_CHAR	'!'
HWND hWinBT=NULL;
HWND hWinRB=NULL;
CRect rWindBT;
CRect rWindRB;
void ShowUsage()
{
	double dCPUUsage=0;
	CString sTextPrefix="";
	if(g_bShowCPU==0){
		dCPUUsage=GetCPUTimesPercents();
		sTextPrefix="C";
	}else{
		dCPUUsage=GetMemoryPercents();
		sTextPrefix="M";
	}
	if(dCPUUsage<0){
		dCPUUsage=0;
	}
	static long lWidthInChars=0;
	static CRect rtChar,rPref,DesktopDialogRect;
	if(hWinBT==NULL || !IsWindow(hWinBT)){
		hWinBT=NULL;
		HWND hWnd1 = ::FindWindow("Shell_TrayWnd",0);
		if(hWnd1){
			hWinBT = ::FindWindowEx(hWnd1,0,"Button",NULL);
			::GetWindowRect(hWinBT,&rWindBT);
			lWidthInChars=0;
		}
	}
	CString sText;
	if(hWinBT){
		if(::GetWindowLong(hWinBT,GWL_STYLE) & BS_BITMAP){
			::SetWindowLong(hWinBT,GWL_STYLE,(::GetWindowLong(hWinBT,GWL_STYLE)&(~BS_BITMAP)&(~BS_LEFT))|BS_CENTER);//|BS_FLAT//&(~BS_PUSHLIKE)
		}
		if(lWidthInChars==0){
			sText.Format("%lu%%",int(dCPUUsage));
			::SetWindowText(hWinBT,sText);
		}else{
			int iPerc=int(double(lWidthInChars)*dCPUUsage/100);
			sText=sTextPrefix+":";
			if(iPerc>0){
				sText+=CString(HIST_CHAR,iPerc);
			}else{
				sText+="0";
			}
			::SetWindowText(hWinBT,sText);
		}
		/*
		#pragma TODO(Fix here sva start position)
		CRect rt;
		GetWindowRect(hWinBT,&rt);
		if(rt.Width()!=rWindBT.Width() || rt.Height()!=rWindBT.Height()){
		}
		*/
		::InvalidateRect(hWinBT,NULL,TRUE);
	}
}

bool stopped=0;
HANDLE hHookerThread=0;
HANDLE hThreadWorking=0;
HANDLE hThreadStopped=0;
DWORD WINAPI MainThread(LPVOID)
{
	hThreadStopped=::CreateEvent(NULL,FALSE,FALSE,NULL);
	DWORD dwLastSetTime=GetTickCount();
	while(!stopped){
		WaitForSingleObject(hThreadWorking,1000);
		if(long(GetTickCount()-dwLastSetTime)>g_bShowMS){
			dwLastSetTime=GetTickCount();
			ShowUsage();
		}
	}
	SetEvent(hThreadStopped);
	return 0;
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
	DWORD dwID=0;
	stopped = false;
	hThreadWorking=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(0,0,MainThread,0,0,&dwID);
	return 1;
}

int	WINAPI WKPluginStart()
{
	stopped=0;
	return 1;
}

int	WINAPI WKPluginStop()
{
	stopped=1;
	if(hHookerThread){
		if(hThreadStopped){
			if(!WKGetPluginContainer()->getOption(WKOPT_ISSHUTDOWN)){
				SetEvent(hThreadWorking);
				WaitForSingleObject(hThreadStopped,1000);
			}
			DWORD dwTRes=0;
			if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
				//TerminateThread(hThread,66);
				//SuspendThread(hHookerThread);
			}
			CloseHandle(hThreadWorking);
			CloseHandle(hThreadStopped);
		}
		CloseHandle(hHookerThread);
		hHookerThread=0;
	}
	if(hWinBT){
		::SetWindowText(hWinBT,"Start");
	}
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
	return 0;
	/*
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
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
	stuff->dwDefaultHotkey=0;
	return 1;
	*/
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	return 0;
	/*
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	return 1;
	*/
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	return 0;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Useless 'Start'");
	strcpy(dsc->szDesk,"Replaces text on 'Start' button with CPU/Memory usage");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		CString sRes=CString(_l("CPU usage"))+CString("\t")+CString(_l("Memory usage"));
		pOptionsCallback->AddListOption("CPU","Replace text on 'Start' button with","",sRes,0);
		pOptionsCallback->AddNumberOption("Freq","Refresh every ... milliseconds","",2000,0,10000,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		g_bShowCPU=pOptionsCallback->GetListOption("CPU");
		g_bShowMS=pOptionsCallback->GetNumberOption("Freq");
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetListOption("CPU",g_bShowCPU);
		pOptionsCallback->SetNumberOption("Freq",g_bShowMS);
	}
	return 1;
}

BOOL isFileExist(const char* szFile)
{
	return 0;
}

BOOL SaveFile(const char* sStartDir, const char* sFileContent)
{
	return 0;
}

CString GetUserFolder()
{
	return "";
}