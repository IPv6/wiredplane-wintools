// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "WP_MegaMouse.h"
#include "MouseHelper.h"
#include <string>

int WINAPI HandleSystemEvent(CWKSYSEvent* pEvent);
COptions plgOptions;
MouseHelper* g_helperWnd;
WKCallbackInterface* pInter=NULL;

WKCallbackInterface*& WKGetPluginContainer()
{
    static WKCallbackInterface* pCallback=0;
    return pCallback;
}

int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	pInter=init;
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
	return 1;
}


int	WINAPI WKPluginStart()
{
    plgOptions.bColorVisible = true;
    plgOptions.bPositionVisible = true;
    plgOptions.bZoomVisible = true;
    plgOptions.iZoomOrder = ZS_NORMAL;
    POINT pt;
    pt.x = ZS_NORMAL;
    pt.y = ZS_NORMAL;
    plgOptions.pZoomWindowSize = pt;
    WKGetPluginContainer()->SubmitToSystemEventNotification(WKSYSEVENTTYPE_MOUSEMOVE, (FPONSYSTEMEVENT)HandleSystemEvent);
    g_helperWnd = new MouseHelper(&plgOptions);

	return 1;
}

int	WINAPI _WKPluginPause(BOOL bPaused)
{
	return 1;
}

int	WINAPI WKPluginStop()
{
    WKGetPluginContainer()->UnSubmitToSystemEventNotification(WKSYSEVENTTYPE_MOUSEMOVE, (FPONSYSTEMEVENT)HandleSystemEvent);
    delete g_helperWnd;
	return 1;
}

char* _l(const char* szText, char* szOut, int size)
{
	WKGetPluginContainer()->GetTranslation(szText,szOut,size);
    return szOut;
}

#define MAX_HK	2
int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>=MAX_HK || stuff==NULL){
		return 0;
	}
	stuff->bVisible=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>=MAX_HK || stuff==NULL)
    {
		return 0;
	}
	stuff->dwDefaultHotkey=0;
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY;
	switch(iPluginFunction)
    {
	case 0:
		strcpy(stuff->szItemName, "ShowWindowDesc");
        break;
	case 1:
		strcpy(stuff->szItemName, "CopyColorDesc");
		break;
	default:
		return 0;
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	static long lNoConcurrency=0;
	if(lNoConcurrency>0)
    {
		return 0;
	}
	lNoConcurrency++;
	BOOL bRes=0;
    TCHAR szBuff[256]={0};
	switch(iPluginFunction)
	{
	case 0:
        if (g_helperWnd->IsWindowVisible())
            g_helperWnd->HidePluginWindow();
        else
            g_helperWnd->ShowPluginWindow();
		bRes=1;
		break;
	case 1:
        MouseHelper::CopyCurrentPixelColor();
        WKGetPluginContainer()->ShowBaloon(_l("ColorBaloonText", szBuff, 256), _l("ÑolorBaloonTitle", szBuff, 256), 2000);
		bRes=1;
		break;
	default:
		bRes=0;
	}
	lNoConcurrency--;
	return bRes;
}

extern HINSTANCE g_hinstDll;
int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle, "Color picker");
	strcpy(dsc->szDesk, "Convinient color picker and magnifier");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD)
    {
        CHAR szBuff[256];
        CHAR szBuff2[256];
        CHAR szBuff3[256];
        CHAR szBigBuff[1024];
		pOptionsCallback->AddTip("keys","DscKeys",0);
        pOptionsCallback->AddBoolOption("bPositionVisible",
                                        "PosOptTitle",
                                        "PosOptDesc", 1, 0);
        pOptionsCallback->AddBoolOption("bZoomVisible",
                                        "ZoomVisOptTitle",
                                        "ZoomVisOptDesc", 1, 0);
        sprintf(szBigBuff, "%s\t%s\t%s\t", _l("ZoomOrder0", szBuff, 256), _l("ZoomOrder1", szBuff2, 256), _l("ZoomOrder2", szBuff3, 256));
        pOptionsCallback->AddListOption("iZoomOrder",
                                        "ZoomOrderTitle",
                                        "ZoomOrderDesc",
                                        szBigBuff, 1, 0);
        sprintf(szBigBuff, "%s\t%s\t", _l("pZoomWindowSize0", szBuff, 256), _l("pZoomWindowSize1", szBuff2, 256));
        pOptionsCallback->AddListOption("pZoomWindowSize",
                                        "pZoomWindowSizeTitle",
                                        "pZoomWindowSizeDesc",
                                        szBigBuff, 0, 0);
        pOptionsCallback->AddBoolOption("bColorVisible",
                                        "ColorOptTitle",
                                        "ColorOptDesc", 1, 0);
        pOptionsCallback->AddBoolOption("bStickToMouse",
                                        "bStickToMouseTitle",
                                        "bStickToMouseDesc", 0, 0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET)
    {
        plgOptions.bPositionVisible = (pOptionsCallback->GetBoolOption("bPositionVisible") != 0);

        plgOptions.bZoomVisible = (pOptionsCallback->GetBoolOption("bZoomVisible") != 0);

        if (pOptionsCallback->GetListOption("iZoomOrder") == 0)
            plgOptions.iZoomOrder = ZO_NOZOOM;
        else if (pOptionsCallback->GetListOption("iZoomOrder") == 1)
            plgOptions.iZoomOrder = ZO_NORMAL;
        else
            plgOptions.iZoomOrder = ZO_HIGH;

        POINT pt;
        if (pOptionsCallback->GetListOption("pZoomWindowSize") == 0)
            pt.x = pt.y = ZS_NORMAL;
        else
            pt.x = pt.y = ZS_LARGE;

        if (!plgOptions.bZoomVisible)
            pt.x = pt.y = 0;

        plgOptions.pZoomWindowSize = pt;

        plgOptions.bColorVisible = (pOptionsCallback->GetBoolOption("bColorVisible") != 0);

        plgOptions.bStickToMouse = (pOptionsCallback->GetBoolOption("bStickToMouse") != 0);
        g_helperWnd->ApplySettings(&plgOptions);
	}
	if(iAction==OM_OPTIONS_GET)
    {
        pOptionsCallback->SetBoolOption("bPositionVisible", ((plgOptions.bPositionVisible)?(1):(0)));
        pOptionsCallback->SetBoolOption("bZoomVisible", ((plgOptions.bZoomVisible)?(1):(0)));
        if (plgOptions.iZoomOrder > ZO_NORMAL)
            pOptionsCallback->SetListOption("iZoomOrder", (plgOptions.iZoomOrder - 1));
        else
            pOptionsCallback->SetListOption("iZoomOrder", 3);

        pOptionsCallback->SetListOption("iZoomWindowSize", ((plgOptions.pZoomWindowSize.x == ZS_NORMAL)?(0):(1)));
        pOptionsCallback->SetBoolOption("bColorVisible", ((plgOptions.bColorVisible)?(1):(0)));
        pOptionsCallback->SetBoolOption("bStickToMouse", ((plgOptions.bStickToMouse)?(1):(0)));
	}
	return 1;
}

int WINAPI HandleSystemEvent(CWKSYSEvent* pEvent)
{
    SendMessage(g_helperWnd->GetWindow(), WM_MOUSEMOVE, 0, 0);

    return 1;
}