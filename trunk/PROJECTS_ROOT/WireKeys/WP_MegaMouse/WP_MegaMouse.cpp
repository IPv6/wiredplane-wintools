// WP_MegaMouse.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "WP_MegaMouse.h"


HINSTANCE g_hinstDll=NULL;

BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        g_hinstDll = hModule;
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

CNTWnWrap& getCNTWnWrap()
{
	static CNTWnWrap wrap;
	return wrap;
}

BOOL SetLayeredWindowStyle(HWND m_hWnd, BOOL bSet)
{
	if(getCNTWnWrap().m_hUser32Instance){
		if(bSet){
			SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd, GWL_EXSTYLE) | (0x00080000L));//WS_EX_APPWINDOW         0x00040000L
		}else{
			SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~(0x00080000L));
		}
		return TRUE;
	}
	return FALSE;
}

BOOL SetWndAlpha(HWND m_hWnd, DWORD dwPercent, BOOL bAlpha)
{
	if(getCNTWnWrap().m_hUser32Instance){
		if(!getCNTWnWrap().SetLayerAttribs){
			getCNTWnWrap().SetLayerAttribs=(FARPROCL)GetProcAddress(getCNTWnWrap().m_hUser32Instance,"SetLayeredWindowAttributes");
		}
		if(getCNTWnWrap().SetLayerAttribs){
			if(bAlpha){
				return getCNTWnWrap().SetLayerAttribs(m_hWnd, 0, (255 * (BYTE)dwPercent) / 100, 2);
			}else{
				return getCNTWnWrap().SetLayerAttribs(m_hWnd, dwPercent, 255, 1);
			}
		}
	}
	return FALSE;
}

BOOL SwitchToWindow(HWND hWnd, BOOL bMoveFocus)
{
	if(!::IsWindow(hWnd)){
		return FALSE;
	}
	// Приаттачиваемся к процессу
	// Перевод фокуса с тфжелой артилеррией
	DWORD dwCurWinProcID=0;
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(hWnd,&dwCurWinProcID);
	DWORD dwThisThread=GetCurrentThreadId();
	AttachThreadInput(dwThisThread,dwCurWinThreadID,TRUE);

	BOOL bTopMost=::GetWindowLong(hWnd,GWL_EXSTYLE) & WS_EX_TOPMOST;
	BOOL bRes=FALSE;
	int iShowCmd=SW_SHOWNORMAL;
	if(::IsZoomed(hWnd)){
		iShowCmd=SW_SHOW;
	}
	HWND hFore=GetForegroundWindow();
	if(hFore){
		::SetWindowPos(hWnd,hFore,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|iShowCmd);
	}
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	::ShowWindow(hWnd,iShowCmd);
	bRes=::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|iShowCmd);
	bRes=::RedrawWindow(hWnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_FRAME) && bRes;
	if(!bTopMost){
		bRes=::SetWindowPos(hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW) && bRes;
	}
	if(GetForegroundWindow()!=hWnd){
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hWnd);
	}
	if(bMoveFocus){
		::SetFocus(hWnd);
	}
	AttachThreadInput(dwThisThread,dwCurWinThreadID,FALSE);
	return bRes;
	/*if(getCNTWnWrap().m_hUser32Instance){
		if(!getCNTWnWrap().SwitchToWnd){
			getCNTWnWrap().SwitchToWnd=(FARPROCW)GetProcAddress(getCNTWnWrap().m_hUser32Instance,"SwitchToThisWindow");
		}
		if(getCNTWnWrap().SwitchToWnd){
			getCNTWnWrap().SwitchToWnd(hWnd, TRUE);
			return TRUE;
		}
	}
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	//::ShowWindow(hWnd,SW_SHOW);*/
	return FALSE;
}