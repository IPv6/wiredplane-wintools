// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_IconsFont.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CIconsFontInfo hookData;
extern CIconsFontOptions plgOptions;
extern HINSTANCE g_hinstDll;
extern BOOL	g_bFirstTime;
extern HWND	g_hTargetWin;
extern DWORD g_hTargetThread;
extern HANDLE hHookerEventDone;
#define SAVELOGFONT_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\FontMaster"

HWND GetSysListView()
{
	static HWND hTargetWindow=NULL;
	if(hTargetWindow!=NULL && !IsWindow(hTargetWindow)){
		hTargetWindow=NULL;
	}
	if(hTargetWindow==NULL){
		HWND hWnd1 = ::FindWindow("Progman","Program Manager");
		if(hWnd1 == 0){
			hWnd1 = FindWindow("ProgMan",NULL);
		}
		if(hWnd1 == 0){
			return 0;
		}
		HWND hWnd2 = FindWindowEx(hWnd1,0,"SHELLDLL_DefView",NULL); 
		if(hWnd2 == 0){
			hWnd2 = GetWindow(hWnd1,GW_CHILD);
		}
		if(hWnd2 == 0){
			return 0;
		}
		hTargetWindow = FindWindowEx(hWnd2,0,"SysListView32",NULL); 
		if(hTargetWindow == 0){
			hTargetWindow = GetWindow(hWnd2,GW_CHILD); // Desktop Window
		}
	}
	return hTargetWindow;
}

// It is very important to set and remove hook from one thread
// For Windows 2000 and higher this is required 
DWORD WINAPI GlobalHooker(LPVOID pData)
{
	MSG msg;
	// Создаем очередь сообщений
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	while(GetMessage(&msg,0,0,0)>0){
		if(msg.message==WM_ENABLE && msg.wParam!=0){
			SetHook(GetSysListView());
		}
		if(msg.message==WM_CLOSE || (msg.message==WM_ENABLE && msg.wParam==0)){
			SetHook(NULL);
		}
		SetEvent(hHookerEventDone);
		if(msg.message==WM_CLOSE || msg.message==WM_QUIT){
			break;
		}
	}
	return 0;
}

int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		::SendMessage(GetDlgItem(hwndDlg,IDC_SETFONT), BM_SETCHECK, plgOptions.bSetFont, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_SETTXCOL), BM_SETCHECK, plgOptions.bSetTxColor, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_SETBGCOL), BM_SETCHECK, plgOptions.bSetBgColor, 0);
		::SendMessage(GetDlgItem(hwndDlg,IDC_SETBGTRANS), BM_SETCHECK, plgOptions.bSetTransp, 0);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		plgOptions.bSetFont=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_SETFONT), BM_GETCHECK, 0, 0);
		plgOptions.bSetTxColor=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_SETTXCOL), BM_GETCHECK, 0, 0);
		plgOptions.bSetBgColor=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_SETBGCOL), BM_GETCHECK, 0, 0);
		plgOptions.bSetTransp=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_SETBGTRANS), BM_GETCHECK, 0, 0);
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	if(uMsg==WM_DRAWITEM && wParam==IDC_BT_BG){
		static char* szExText="Example";
		DRAWITEMSTRUCT* pDr=(DRAWITEMSTRUCT*)lParam;
		HBRUSH hBr=CreateSolidBrush(hookData.g_FontBgColor);
		::FillRect(pDr->hDC,&pDr->rcItem,hBr);
		DeleteObject(hBr);
		HFONT hFont = ::CreateFontIndirect(&hookData.g_Font);
		HFONT hPrevFont=(HFONT)::SelectObject(pDr->hDC,hFont);
		::SetTextColor(pDr->hDC,hookData.g_FontColor);
		::SetBkColor(pDr->hDC,hookData.g_FontBgColor);
		::DrawText(pDr->hDC,szExText,strlen(szExText),&pDr->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		::SelectObject(pDr->hDC,hPrevFont);
		HBRUSH hBr2=CreateSolidBrush(0);
		::FrameRect(pDr->hDC,&pDr->rcItem,hBr2);
		DeleteObject(hBr2);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BGCOL){
		CColorDialog dlg(hookData.g_FontBgColor,0,CWnd::FromHandle(hwndDlg));
		if(dlg.DoModal()==IDOK){
			hookData.g_FontBgColor=dlg.GetColor();
		}
		::InvalidateRect(hwndDlg,NULL,TRUE);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==ID_SETFONT){
		DWORD dwEffects=CF_EFFECTS|CF_SCREENFONTS;
		CFontDialog dlg((hookData.g_Font.lfHeight)?&hookData.g_Font:NULL,dwEffects,NULL,CWnd::FromHandle(hwndDlg));
		dlg.m_cf.rgbColors=hookData.g_FontColor;
		dlg.m_cf.Flags|=dwEffects|CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST|CF_NOVERTFONTS;
		dlg.m_cf.lpLogFont=&hookData.g_Font;
		if(dlg.DoModal()==IDOK){
			hookData.g_FontColor=dlg.GetColor();
		}
		::InvalidateRect(hwndDlg,NULL,TRUE);
		return TRUE;
	}
	return FALSE;
}

