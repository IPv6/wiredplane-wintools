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
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Make hole in the window under cursor");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=0;
		stuff->dwDefaultHotkey=0;
	}
	if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Restore window under cursor");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=0;
		stuff->dwDefaultHotkey=0;
	}
	return 1;
}

char szHotkeyStrPresentation[32]={0};
int	WINAPI WKGetPluginFunctionHints(long iPluginFunction, long lHintCode, void*& pOut)
{
	strcpy(szHotkeyStrPresentation,"%0~-27,486");
	pOut=szHotkeyStrPresentation;
	return 1;
};

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
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

int g_iRadi=150;
void MakeHole(POINT pt, HWND hw)
{
	CRect rt,rtR;
	GetWindowRect(hw,rt);
	int iX=pt.x-rt.left;
	int iY=pt.y-rt.top;
	HRGN hRgn=CreateRectRgn(0,0,0,0);
	int iRes=::GetWindowRgn(hw, hRgn);
	GetRgnBox(hRgn,&rtR);
	if(iRes==NULLREGION || rtR.Width()==0 || rtR.Height()==0){
		hRgn=CreateRectRgn(0,0,rt.Width(),rt.Height());
	}
	int iRadius=g_iRadi;
	CRgn hRgn2;
	hRgn2.CreateEllipticRgn(iX-iRadius/2,iY-iRadius/2,iX+iRadius/2,iY+iRadius/2);
	::CombineRgn(hRgn,hRgn,hRgn2,RGN_DIFF);
	SetWindowRgn(hw,hRgn,TRUE);
	//DeleteObject(hRgn);
}

void GetWindowTopParent(HWND& hWnd,POINT pt,BOOL bType)
{
	if(!hWnd){
		return;
	}
	//hWnd=::GetAncestor(hWnd,GA_ROOT);
	HWND hWnd2=hWnd;
	while(hWnd2!=NULL && IsWindowVisible(hWnd2)){
		hWnd=hWnd2;
		if(bType){
			MakeHole(pt,hWnd);
		}else{
			SetWindowRgn(hWnd,0,TRUE);
		}
		hWnd2=::GetWindow(hWnd,GW_OWNER);
		if(!hWnd2){
			hWnd2=::GetParent(hWnd);
		}
	}
	return;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	static POINT ptPrev;
	POINT pt;
	GetCursorPos(&pt);
	if(ptPrev.x==pt.x && ptPrev.y==pt.y){
		return 0;
	}
	ptPrev.x=pt.x;
	ptPrev.y=pt.y;
	HWND hw=::WindowFromPoint(pt);
	HWND hw0=hw;
	if(iPluginFunction==0){
		int iRadiusSave=g_iRadi;
		for(g_iRadi=10;g_iRadi<iRadiusSave;g_iRadi+=30){
			GetWindowTopParent(hw,pt,1);
			Sleep((iRadiusSave-g_iRadi)/10);
			hw=hw0;
		}
	}
	if(iPluginFunction==1){
		GetWindowTopParent(hw,pt,0);
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Window puncher");
	strcpy(dsc->szDesk,"Make a hole in the window under cursor");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddNumberOption("radius","Punch radius (pixels)","",150,10,9999,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(pOptionsCallback){
			g_iRadi=pOptionsCallback->GetNumberOption("radius");
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetNumberOption("radius",g_iRadi);
	}
	return 1;
}
