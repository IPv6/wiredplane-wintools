// DLG_TaskHelper.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <atlbase.h>
#include "WP_AltTabR.h"
#include "HookCode.h"
#include "DLG_TaskHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern DWORD g_KillMsg;
extern HWND hWndControl;
CString _l(const char* szText);
WKCallbackInterface*& WKGetPluginContainer();
/////////////////////////////////////////////////////////////////////////////
// CDLG_TaskHelper dialog
CDLG_TaskHelper::CDLG_TaskHelper(CWnd* pParent /*=NULL*/)
: CDialog()//CDialog(CDLG_TaskHelper::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_TaskHelper)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	bStarted=0;
}

BEGIN_MESSAGE_MAP(CDLG_TaskHelper, CDialog)
//{{AFX_MSG_MAP(CDLG_TaskHelper)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern BOOL g_bToolBar;
extern int g_ButtonActive;
extern long g_LockCBT;
extern long g_LockCBT2;
LRESULT CDLG_TaskHelper::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_ERASEBKGND){
		HDC hdc=(HDC)wParam;
		::SetMapMode(hdc,MM_TEXT);
		CRect btRect(0,0,6,6);
		GetClientRect(&btRect);
		static HBRUSH hBr=0;
		if(hBr==0){
			hBr=::CreateSolidBrush(GetSysColor(COLOR_INFOBK));
		}
		::FillRect(hdc,&btRect,hBr);
		::MoveToEx(hdc,0,0,0);
		::LineTo(hdc,btRect.Width(),btRect.Height());
		::MoveToEx(hdc,0,btRect.Height()-1,0);
		::LineTo(hdc,btRect.Width(),-1);
		return 1;
	}
	if(message==WM_PAINT){
		ValidateRect(0);
		return 0;
	}
	/*
	if(!g_bToolBar){
		if(message==WM_ACTIVATEAPP || message==WM_ACTIVATE || message==WM_NCACTIVATE){
			g_ButtonActive=TabCtrl_GetCurSel(hWndControl);
		}
	}*/
	CPoint pt;
	CRect rt;
	GetCursorPos(&pt);
	GetWindowRect(&rt);
	if(rt.PtInRect(pt)){
		if(bStarted==1 && g_LockCBT==0 && g_LockCBT2==1){
			//if((message==WM_ACTIVATEAPP) || (message==WM_ACTIVATE)  || (message==WM_NCACTIVATE))
			if(message==WM_LBUTTONDOWN)
			{
				// Вообще не нужно, там все автоматом...
				g_LockCBT2=0;
				ShowWindow(SW_HIDE);
				::SendMessage(hWndControl,g_KillMsg,0,0);
				return 0;
			}
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CDLG_TaskHelper::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// Тень
	SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)&(~(0x00020000)));
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	ModifyStyleEx(0,WS_EX_TOOLWINDOW|WS_EX_TOPMOST);
	ShowWindow(SW_HIDE);
	bStarted=1;
	return FALSE;
}

void CDLG_TaskHelper::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

CDLG_TaskHelper::~CDLG_TaskHelper()
{
	// Just nothing todo
	Sleep(10);
}
