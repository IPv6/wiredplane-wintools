// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "resource.h"
#include "..\wkplugin.h"
#include "..\..\SmartWires\SystemUtils\Macros.h"
#include "sntp.h"

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

const char* _l(const char* szText)
{
	static char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

class COptions
{
public:
	char sTimeServer[256];
	DWORD dwServerPort;
	DWORD dwTimeOut;
	BOOL bNotify;
	COptions(){
		strcpy(sTimeServer,"ntp.nasa.gov");
		bNotify=1;
		dwTimeOut=360;
		dwServerPort=123;
	}
	~COptions(){
	}
};
COptions plgOptions;

BOOL SyncTime()
{
	static long lc=0;
	if(lc>0){
		return 0;
	}
	SimpleTracker c(lc);
	CSNTPClient snc;
	snc.SetTimeout(20000);
	//
	NtpServerResponse resp;
	BOOL bRes=snc.GetServerTime(plgOptions.sTimeServer,resp,plgOptions.dwServerPort);
	if(bRes){
		if(resp.m_LocalClockOffset>1000){
			if(plgOptions.bNotify){
				CString s=CString(_l("Your clocks are out of sync!\nSynchronizing your clocks with"))+CString(" ")+plgOptions.sTimeServer;
				WKGetPluginContainer()->ShowBaloon(s,_l("Warning: Time sync plugin"),9000);
			}
			/*
			SYSTEMTIME st1 = resp.m_OriginateTime;
			SYSTEMTIME st2 = resp.m_ReceiveTime;
			SYSTEMTIME st3 = resp.m_TransmitTime;
			SYSTEMTIME st4 = resp.m_DestinationTime;
			*/
			CNtpTime newTime(CNtpTime::GetCurrentTime() + resp.m_LocalClockOffset);
			snc.SetClientTime(newTime);
		}
	}else{
		if(plgOptions.bNotify){
			CString s=CString(_l("Can`t connect to clock server!\nServer"))+CString(": ")+CString(plgOptions.sTimeServer);
			WKGetPluginContainer()->ShowBaloon(s,_l("Error: Time sync plugin"),9000);
		}
	}
	return bRes;
}

BOOL bStop=0;
HANDLE hHookerThread=0;
HANDLE hHookerEventStop=0;
DWORD WINAPI GlobalHooker_TSync(LPVOID pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	WSADATA WSAData;
	int rc=WSAStartup(MAKEWORD(1,1),&WSAData);
	while(!bStop){
		SyncTime();
		WaitForSingleObject(hHookerEventStop,plgOptions.dwTimeOut*1000*60);
	}
	WSACleanup();
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
	DWORD dwHookerThread=0;
	hHookerEventStop=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_TSync, NULL, 0, &dwHookerThread);
	return 1;
}

int	WINAPI WKPluginStart()
{
	return 1;
}

int	WINAPI WKPluginStop()
{
	bStop=1;
	SetEvent(hHookerEventStop);
	::CloseHandle(hHookerEventStop);
	::CloseHandle(hHookerThread);
	return 1;
}


//HINSTANCE g_hinstDll=NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain_(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		//g_hinstDll = hinstDll;
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
	strcpy(stuff->szItemName,"Sync time now");
	stuff->hItemBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON));
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
	stuff->dwDefaultHotkey=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>0 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(iPluginFunction==0){
		SyncTime();
		return 1;
	}
	return 0;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Time sync");
	strcpy(dsc->szDesk,"This plugin allows you to sync your time with atomic clock from internet");
	dsc->hPluginBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddStringOption("Server","SNTP Server","You can find free servers at Google by using 'Free SNTP servers' query","ntp.nasa.gov");
		pOptionsCallback->AddNumberOption("Port","Server Port","",123,0,99999,0);
		pOptionsCallback->AddNumberOption("Tout","Timeout (minutes)","How frequently check time at time server",60*3,0,99999,0);
		pOptionsCallback->AddBoolOption("Notify","Notify about sync issues","",TRUE,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(pOptionsCallback){
			pOptionsCallback->GetStringOption("Server",plgOptions.sTimeServer,sizeof(plgOptions.sTimeServer));
			plgOptions.dwServerPort=pOptionsCallback->GetNumberOption("Port");
			plgOptions.dwTimeOut=pOptionsCallback->GetNumberOption("Tout");
			plgOptions.bNotify=pOptionsCallback->GetBoolOption("Notify");
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetStringOption("Server",plgOptions.sTimeServer);
		pOptionsCallback->SetNumberOption("Port",plgOptions.dwServerPort);
		pOptionsCallback->SetNumberOption("Tout",plgOptions.dwTimeOut);
		pOptionsCallback->SetBoolOption("Notify",plgOptions.bNotify);
	}
	return 1;
}
/*
This article describes a time client which uses RFC 868 protocol. The program uses TCP port 37 for connection with a time server. This is a MFC based console application.

The following block calculates current date and time.

lnTime = (long)((sBuffer - (unsigned long)(DATE_1970)));
Time_Int64 = Int32x32To64(lnTime, 10000000) + 116444736000000000;
FileTime.dwLowDateTime = (DWORD)Time_Int64;
FileTime.dwHighDateTime = Time_Int64 >> 32;
FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
FileTimeToSystemTime(&LocalFileTime, &SystemTime);
SetLocalTime( &SystemTime );

The time is calculated and converted to 64-bit in the first and second lines and then converted to local time zone before being converted to a SYSTEMTIME structure. Last of all the system time is set.

The following is the procedure for communicating with the time server. (These notes are copied from http://www.faqs.org/rfcs/rfc868.html )

   1. S: Listen on port 37 (45 octal).
   2. U: Connect to port 37.
   3. S: Send the time as a 32 bit binary number.
   4. U: Receive the time.
   5. U: Close the connection.
   6. S: Close the connection.

The server listens for a connection on port 37. When the connection is established, the server returns a 32-bit time value and closes the connection. If the server is unable to determine the time at its site, it should either refuse the connection or close it without sending anything. The most important thing is, the time is the number of seconds since 00:00 (midnight) 1 January 1900 GMT, such that the time 1 is 12:00:01 am on 1 January 1900 GMT; this base will serve until the year 2036.
*/