// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_SizeWnd.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString sPrevText;
CRect rtScreenRect;
CRect rtPrevScrRect;
extern COptions plgOptions;
WKCallbackInterface*& WKGetPluginContainer();

BOOL DrawScreenshotRect(CRect& Rectangle, const char* szTxt)
{
	CDC dc;
	dc.CreateDC("DISPLAY", NULL, NULL, NULL);
	dc.SetROP2(R2_NOT);
	//dc.DrawFocusRect(&Rectangle);
	CPen pn(PS_SOLID,1,RGB(0,0,0));
	CPen* pn2=dc.SelectObject(&pn);
	CRect rtLines(Rectangle);
	rtLines.NormalizeRect();
	dc.MoveTo(rtLines.right,rtScreenRect.top);
	dc.LineTo(rtLines.right,rtScreenRect.bottom);
	dc.MoveTo(rtLines.left,rtScreenRect.top);
	dc.LineTo(rtLines.left,rtScreenRect.bottom);
	dc.MoveTo(rtScreenRect.left,rtLines.top);
	dc.LineTo(rtScreenRect.right,rtLines.top);
	dc.MoveTo(rtScreenRect.left,rtLines.bottom);
	dc.LineTo(rtScreenRect.right,rtLines.bottom);
	dc.SelectObject(pn2);
	return TRUE;
}

BOOL StartScreenshotRect(CRect& Rectangle, CRect& Screen)
{
	rtScreenRect.CopyRect(Screen);
	rtPrevScrRect.CopyRect(Rectangle);
	DrawScreenshotRect(rtPrevScrRect,"");
	return TRUE;
}

BOOL ContScreenshotRect(CRect& Rectangle, const char* szTxt)
{
	DrawScreenshotRect(rtPrevScrRect,sPrevText);
	rtPrevScrRect.CopyRect(Rectangle);
	sPrevText=szTxt;
	DrawScreenshotRect(rtPrevScrRect,sPrevText);
	return TRUE;
}

BOOL StopScreenshotRect()
{
	if(!rtPrevScrRect.IsRectEmpty()){
		DrawScreenshotRect(rtPrevScrRect,sPrevText);
	}
	rtPrevScrRect.left=rtPrevScrRect.right=rtPrevScrRect.top=rtPrevScrRect.bottom=0;
	sPrevText="";
	return TRUE;
}

typedef BOOL (WINAPI *_BlockInput)(BOOL);
BOOL BlockInputMy(BOOL bBlock)
{
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	_BlockInput fp=NULL;
	if(hUser32){
		fp=(_BlockInput)GetProcAddress(hUser32,"BlockInput");
	}
	if(hUser32 && fp){
		return (*fp)(bBlock);
	}
	return FALSE;
}

BOOL GetNewPosition(CRect& rUserRect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//BlockInputMy(TRUE);
	CRect rDesktopRECT;
	::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
	HWND wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "CAPT_WND", 0, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.Width(), rDesktopRECT.Height(), 0, 0, WKGetPluginContainer()->GetPluginDllInstance(), 0);
	if(wndShotPhase2){
		::SetCursor(::LoadCursor(0,MAKEINTRESOURCE(IDC_CROSS)));
		::SetWindowLong(wndShotPhase2,GWL_STYLE,(::GetWindowLong(wndShotPhase2,GWL_STYLE)&(~(WS_CAPTION|WS_BORDER|WS_DLGFRAME|WS_OVERLAPPEDWINDOW))));
		::ShowWindow(wndShotPhase2,SW_SHOWNOACTIVATE);
		::EnableWindow(wndShotPhase2,TRUE);
		::SetCapture(wndShotPhase2);
	}
	//BlockInputMy(FALSE);
	// Ждем пока нажмут гденить
	BOOL bEscaped=FALSE;
	while(GetAsyncKeyState(VK_LBUTTON)>=0){
		Sleep(100);
		if(GetAsyncKeyState(VK_ESCAPE)<0){
			bEscaped=TRUE;
			break;
		}
	}
	if(!bEscaped){
		//---------------------------------------------
		GetCursorPos(&rUserRect.TopLeft());
		// Ждем пока отпустят гденить
		GetCursorPos(&rUserRect.BottomRight());
		StartScreenshotRect(rUserRect,rDesktopRECT);
		while(GetAsyncKeyState(VK_LBUTTON)<0){
			if(GetAsyncKeyState(VK_ESCAPE)<0){
				bEscaped=TRUE;
				break;
			}
			CPoint pt,pt2(rUserRect.BottomRight());
			GetCursorPos(&pt);
			rUserRect.BottomRight()=pt;
			static BOOL bPrevState=FALSE;
			if(GetAsyncKeyState(VK_RBUTTON)<0){
				if(bPrevState==FALSE){
					bPrevState=TRUE;
					long l=rUserRect.left;
					rUserRect.left=rUserRect.right;
					rUserRect.right=l;
					long l2=rUserRect.top;
					rUserRect.top=rUserRect.bottom;
					rUserRect.bottom=l2;
					SetCursorPos(rUserRect.right,rUserRect.bottom);
				}
			}else{
				bPrevState=FALSE;
			}
			if(pt.x==pt2.x && pt.y==pt2.y){
				Sleep(50);
			}else{
				ContScreenshotRect(rUserRect,NULL);
			}
		}
		// Стираем последний прямоугольник
		StopScreenshotRect();
		// Запоминаем последнюю позицию и убиваем окошко
		GetCursorPos(&rUserRect.BottomRight());
		ReleaseCapture();
		if(wndShotPhase2){
			DestroyWindow(wndShotPhase2);
		}
	}
	if(bEscaped){
		// Спаслись ескейпом :)
		return FALSE;
	}
	return TRUE;
}

CArray<HWND,HWND> g_Windows;
CArray<CRect, CRect> g_aWndRects;
CArray<CRect, CRect> g_aWndPRects;
BOOL CALLBACK AddCWin(HWND hwnd,LPARAM lParam)
{
	CRect rt,prt(0,0,0,0);
	::GetWindowRect(hwnd,&rt);
	g_Windows.Add(hwnd);
	g_aWndRects.Add(rt);
	HWND hPWnd=::GetParent(hwnd);
	if(hPWnd){
		::GetWindowRect(hPWnd,&prt);
	}
	g_aWndPRects.Add(prt);
	return TRUE;
}

BOOL StartWindProcessing(HWND hStartFrom)
{
	// Выясняем новую позицию окна
	CRect rtOld, rtOldClient;
	::GetWindowRect(hStartFrom,&rtOld);
	::GetClientRect(hStartFrom,&rtOldClient);
	if(rtOldClient.Width()==0 || rtOldClient.Height()==0){
		return FALSE;
	}
	CPoint pointClTL=rtOldClient.TopLeft();
	CPoint pointClBR=rtOldClient.BottomRight();
	::ClientToScreen(hStartFrom,&pointClTL);
	::ClientToScreen(hStartFrom,&pointClBR);
	g_Windows.RemoveAll();
	g_aWndRects.RemoveAll();
	::EnumChildWindows(hStartFrom,AddCWin,0);
	if(g_Windows.GetSize()==0){
		return FALSE;
	}
	CRect rt(rtOld);
	if(!GetNewPosition(rt)){
		return FALSE;
	}
	if(rt==rtOld){
		return FALSE;
	}
	::SetWindowPos(hStartFrom,0,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOZORDER);
	CRect rtClient;
	::GetClientRect(hStartFrom,&rtClient);
	if(rtClient.Width()==0 || rtClient.Height()==0){
		return FALSE;
	}
	double xd=double(rtClient.Width())/double(rtOldClient.Width());
	double yd=double(rtClient.Height())/double(rtOldClient.Height());
	for(int i=0;i<g_Windows.GetSize();i++){
		CRect rtWnd(g_aWndRects[i]),rtWndNew;
		if(::GetParent(g_Windows[i])==hStartFrom){
			rtWnd.OffsetRect(-pointClTL.x,-pointClTL.y);
		}else{
			rtWnd.OffsetRect(-g_aWndPRects[i].left,-g_aWndPRects[i].top);
		}
		rtWndNew.left=int(double(rtWnd.left)*xd);
		rtWndNew.right=int(double(rtWnd.right)*xd);
		rtWndNew.top=int(double(rtWnd.top)*yd);
		rtWndNew.bottom=int(double(rtWnd.bottom)*yd);
		//::SetWindowPos(g_Windows[i],0,rtWndNew.left,rtWndNew.top,rtWndNew.Width(),rtWndNew.Height(),SWP_NOZORDER);
		::MoveWindow(g_Windows[i],rtWndNew.left,rtWndNew.top,rtWndNew.Width(),rtWndNew.Height(),TRUE);
	}
	::RedrawWindow(hStartFrom,0,0,RDW_ERASE|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
	return TRUE;
}