// HookCode.cpp: implementation of the CHookCode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HookCode.h"

// Nonshared variables
void ForceDebugBreak()
{
   __try { _asm { int 3 }; }
   __except(UnhandledExceptionFilter(GetExceptionInformation())) { }
}

//extern HINSTANCE g_hinstDll;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
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
}*/

HINSTANCE g_AverTVLib=NULL;
FPAVER_AHWInit _AHWInit=NULL;
FPAVER_AHWFree _AHWFree=NULL;
FPAVER_IsRemoteDataReady _IsRemoteDataReady=NULL;
FPAVER_GetRemoteData _GetRemoteData=NULL;
FPAVER_SetRemoteFlag _SetRemoteFlag=NULL;
FPAVER_GetRemoteFlag _GetRemoteFlag=NULL;
BOOL UnLoadAverLib()
{
	if(g_AverTVLib){
		if(_AHWFree){
			(*_AHWFree)();
		}
		FreeLibrary(g_AverTVLib);
		g_AverTVLib=NULL;
	}
	return TRUE;
}

#include "../WKPlugin.h"
extern WKCallbackInterface* pInter;
BOOL LoadAverLib()
{
	BOOL bRes=FALSE;
	CString sAverPath="";
	// »щем ключ реестра
	CRegKey key;
	if(key.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\AVerMedia TECHNOLOGIES, Inc.")!=ERROR_SUCCESS){
		key.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\AVerMedia");
	}
	if(key.m_hKey){
		char szPath[MAX_PATH]="";
		DWORD lSize = sizeof(szPath),dwType=0;
		RegQueryValueEx(key.m_hKey,"Application Path",NULL, &dwType,(LPBYTE)(szPath), &lSize);
		sAverPath=szPath;
	}
	if(sAverPath.Right(1)!="\\"){
		sAverPath+="\\";
	}
	g_AverTVLib=LoadLibrary(sAverPath+"avertv2k.dll");
	if(!g_AverTVLib){
		g_AverTVLib=LoadLibrary(sAverPath+"avertv32.dll");
	}
	if(!g_AverTVLib){
		g_AverTVLib=LoadLibrary(sAverPath+"aver32.dll");
	}
	if(!g_AverTVLib){
		g_AverTVLib=LoadLibrary(sAverPath+"avertv.dll");
	}
	/*
	if(!g_AverTVLib){
		char szHookLibPath[MAX_PATH]="";
		pInter->GetWKPluginsDirectory(szHookLibPath,MAX_PATH);
		//sAverPath=szHookLibPath;
		g_AverTVLib=LoadLibrary(sAverPath+"averapi.dll");
		//g_AverTVLib=LoadLibrary(sAverPath+"avertv2k.dll");
	}
	*/
	if(!g_AverTVLib){
		return FALSE;
	}
	_AHWInit=(FPAVER_AHWInit)GetProcAddress(g_AverTVLib,"AVER_HWInit");
	_AHWFree=(FPAVER_AHWFree)GetProcAddress(g_AverTVLib,"AVER_Free");
	_GetRemoteData=(FPAVER_GetRemoteData)GetProcAddress(g_AverTVLib,"AVER_GetRemoteData");
	// For old cards
	_IsRemoteDataReady=(FPAVER_IsRemoteDataReady)GetProcAddress(g_AverTVLib,"AVER_IsRemoteDataReady");
	_SetRemoteFlag=(FPAVER_SetRemoteFlag)GetProcAddress(g_AverTVLib,"AVER_SetRemoteFlag");
	_GetRemoteFlag=(FPAVER_GetRemoteFlag)GetProcAddress(g_AverTVLib,"AVER_GetRemoteFlag");
	if(_AHWInit && _GetRemoteData){
		bRes=TRUE;
		(*_AHWInit)();
	}
	return bRes;
}