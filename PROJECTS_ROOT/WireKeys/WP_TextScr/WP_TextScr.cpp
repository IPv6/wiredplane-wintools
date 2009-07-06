// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_TextScr.h"
#include "HookCode.h"
#include "WindowHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern COptions plgOptions;
extern CString g_sResult;
extern CArray<HWND,HWND> g_Windows;
BOOL CALLBACK AddCWin(HWND hwnd,LPARAM lParam)
{
	g_Windows.Add(hwnd);
	return TRUE;
}

void GetWindowTopParent(HWND& hWnd)
{
	if(!hWnd){
		return;
	}
	//hWnd=::GetAncestor(hWnd,GA_ROOT);
	HWND hWnd2=hWnd;
	while(hWnd2!=NULL){
		hWnd=hWnd2;
		hWnd2=::GetWindow(hWnd,GW_OWNER);
		if(!hWnd2){
			hWnd2=::GetParent(hWnd);
		}
	}
	return;
}

WKCallbackInterface*& WKGetPluginContainer();
CString _l(const char* szText);
BOOL StartWindProcessing()
{
	if(g_Windows.GetSize()==0){
		return 0;
	}
	WKGetPluginContainer()->ShowBaloon(_l("Parsing window"),_l("Text grabber"),9000);
	EnumChildWindows(g_Windows[0],AddCWin,0);
	g_sResult="";
	
    HRESULT hRes = ::CoInitialize(NULL);
    CWindowHelper _WindowHelper;
    _WindowHelper.Attach(g_Windows[0]);
	g_sResult+=_WindowHelper.GetCombinedTextInfo();
    _WindowHelper.Release();

	if(g_sResult==""){
		//Default approach...
		char szBuffer[10000]="";
		for(int i=0;i<g_Windows.GetSize();i++){
			::GetWindowText(g_Windows[i],szBuffer,sizeof(szBuffer));
			if(strlen(szBuffer)>0){
				g_sResult+=szBuffer;
				g_sResult+="\r\n------------------\r\n";
			}
		}
	}
	WKGetPluginContainer()->ShowBaloon("","",0);
	return TRUE;
}