#include "stdafx.h"
#include "WindowDescriber.h"
#include "SplashScreenEx.h"

long lOptHelp=0;

/*
CMenuToolTip ttMenu;
DWORD WINAPI ShowOptHelpT(LPVOID p)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWnd* pWnd=(CWnd*)p;
	if(!p){
		return 0;
	}
	MSG msg;
	BOOL bRet=0;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	ttMenu.Create(pWnd->GetSafeHwnd(), "...", AfxGetResourceHandle(), WS_CHILD | TTS_NOPREFIX | TTS_BALLOON, 0, TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT);
	while(lOptHelp && (bRet = GetMessage(&msg, NULL, 0, 0)) != 0){ 
		if (msg.message==WM_QUIT || bRet == -1){
			break;
		}
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	ttMenu.ShowToolTip(FALSE);
	return 0;

}

*/

class COptWinds
{
public:
	COptWinds(CWnd* p,CWnd* pT,CStringArray* d)
	{
		pW=p;
		pWT=pT;
		pD=d;
	};
	CWnd* pW;
	CWnd* pWT;
	CStringArray* pD;
};

#include "../SystemUtils/DataXMLSaver.h"
DWORD WINAPI ShowOptHelp(LPVOID p)
{
	if(lOptHelp){
		// ”же идет!
		return 0;
	}
	SimpleTracker lc(lOptHelp);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COptWinds* pParams=(COptWinds*)p;
	HWND hTestWnd=pParams->pWT->GetSafeHwnd();
	CWnd* pWnd=pParams->pW;
	if(!p){
		return 0;
	}
	HWND hParentWnd=pWnd->GetSafeHwnd();
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hTestWnd);
	CStringArray* pInfos=pParams->pD;
	//pWnd->EnableWindow(FALSE);
	BOOL bStop=0;
	for(int i=0;i<pInfos->GetSize();i++){
		CRect rtDesktop;
		CWnd::GetDesktopWindow()->GetWindowRect(&rtDesktop);
		CRect rtTipInClientXY(rtDesktop);
		rtTipInClientXY.DeflateRect(rtDesktop.Width()/3,rtDesktop.Height()/4,rtDesktop.Width()/3,rtDesktop.Height()/4);
		
	
		CSplashScreenEx *pSplash=new CSplashScreenEx();
		pSplash->Create(pWnd,"CSplashScreenEx dynamic text:",0, CSS_FADE | CSS_CENTERAPP | CSS_SHADOW);//CSS_CHILD
		//pSplash->SetBitmap(IDB_SPLASH,255,0,255);
		//pSplash->SetTextFont("Impact",100,CSS_TEXT_NORMAL);
		rtTipInClientXY.OffsetRect(-rtTipInClientXY.left,-rtTipInClientXY.top);
		pSplash->SetTextRect(rtTipInClientXY);
		pSplash->SetTextColor(GetSysColor(COLOR_INFOTEXT));
		pSplash->SetBgColor(GetSysColor(COLOR_INFOBK));
		pSplash->SetTextFormat(DT_LEFT|DT_WORDBREAK|DT_NOPREFIX|DT_TOP);
		pSplash->SetTextDefaultFont(pWnd->GetFont());

		CString sLine=pInfos->GetAt(i);
		CString sText=CDataXMLSaver::GetInstringPart("TEXT[[","]]",sLine);
		CString sTitle=CDataXMLSaver::GetInstringPart("TITLE[[","]]",sLine);
		CString sFooter=CDataXMLSaver::GetInstringPart("FOOTER[[","]]",sLine);
		int lWait=atol(CDataXMLSaver::GetInstringPart("WAIT[[","]]",sLine));
		
		CPoint pt;
		pt.x=atol(CDataXMLSaver::GetInstringPart("X[[","]]",sLine));
		pt.y=atol(CDataXMLSaver::GetInstringPart("Y[[","]]",sLine));

		CString sH=CDataXMLSaver::GetInstringPart("H[[","]]",sLine);
		if(atol(sH)>0){
			rtTipInClientXY.bottom=rtTipInClientXY.top+atol(sH);
		}
		

		BOOL bClient=1;
		CString sID=CDataXMLSaver::GetInstringPart("ID[[","]]",sLine);
		if(sID!=""){
			if(sID=="SCREEN"){
				bClient=0;
				CRect rtFull;
				//GetWindowRect(GetDesktopWindow(),&rtFull);
				::SystemParametersInfo(SPI_GETWORKAREA,0,&rtFull,0);
				if(pt.x<0){
					pt.x+=rtFull.Width()-rtTipInClientXY.Width();
				}else{
					pt.x+=rtFull.left;
				}
				if(pt.y<0){
					pt.y+=rtFull.Height()-rtTipInClientXY.Height();
				}else{
					pt.y+=rtFull.top;
				}
			}else{
				int iID=atol(sID);
				if(iID!=0){
					CWnd* pID=pWnd->GetDlgItem(iID);
					CRect rtID;
					pID->GetWindowRect(&rtID);
					pWnd->ScreenToClient(&rtID);
					pt.x+=rtID.left;
					pt.y+=rtID.top;
				}
			}
		}
		if(sTitle!=""){
			pSplash->SetTitle(sTitle);
		}
		if(sFooter!=""){
			pSplash->SetFooter(sFooter);
		}
		sText.Replace("\\n","\n");
		pSplash->SetText(sText);
		if(pt.x && pt.y){
			if(bClient){
				pWnd->ClientToScreen(&pt);
			}
			pSplash->SetPosition(pt.x,pt.y,rtTipInClientXY.Width(),sH=="AUTO"?0:rtTipInClientXY.Height());
		}
		pSplash->Show();
		BOOL bRet=0;
		DWORD dwTime=GetTickCount();
		while(lOptHelp && (bRet = GetMessage(&msg, NULL, 0, 0)) != 0){
			if(!IsWindow(hTestWnd) || !IsWindow(hParentWnd)){
				bStop=1;
				break;
			}
			if((lWait && int(GetTickCount()-dwTime)>lWait)){
				break;
			}
			if(msg.hwnd==pSplash->GetSafeHwnd()){
				pSplash->PreTranslateMessage(&msg);
			}
			if(pSplash->IsWasLclick()){
				break;
			}
			if (msg.message==WM_QUIT || bRet == -1){
				break;
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hTestWnd);
		pSplash->Hide();
		if(bStop==1){
			break;
		}
	}
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hTestWnd);
	delete pParams;
	return 0;
}

BOOL StartWndDescriber(CWnd* p,CWnd* pT, CStringArray* aDescriptions)
{
	COptWinds* pParams=new COptWinds(p,pT,aDescriptions);
	FORK(ShowOptHelp,pParams);
	return TRUE;
}
