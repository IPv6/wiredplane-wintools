/**
* Copyright (C) 2002-2005
* W3L GmbH
* Technologiezentrum Ruhr
* Universitätsstraße 142
* D-44799 Bochum
* 
* Author: Dipl.Ing. Doga Arinir
* E-Mail: doga.arinir@w3l.de
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the author or the company be held liable 
* for any damages arising from the use of this software. EXPECT BUGS!
* 
* You may use this software in compiled form in any way you desire PROVIDING it is
* not sold for profit without the authors written permission, and providing that this
* notice and the authors name is included. If the source code in this file is used in 
* any commercial application then acknowledgement must be made to the author of this file.
*/

#include "stdafx.h"

#include "floatingBT.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
void DrawMe(HWND hwnd, HDC hDC)
{
	RECT rect;
	::GetClientRect(hwnd, &rect);
	/*
	HFONT font =(HFONT)::SendMessage(hl->hParent, WM_GETFONT, 0, 0);
	::SelectObject(hDC, font);
	::SetTextColor(hDC, RGB(0,0,200));
	::SetBkMode(hDC, TRANSPARENT);
	char* sz=hl->m_UrlVisible[0]!=0?hl->m_UrlVisible:hl->m_Url;
	::DrawText(hDC, sz, strlen(sz), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	//::DeleteObject(font);
	*/
	DWORD aColors[]={RGB(254,232,199)
		,RGB(253,208,142)
		,RGB(232,182,75)
		,RGB(200,132,4)};
	DWORD dwColIndex=(DWORD)::GetProp(hwnd,"WK_SHELL_COL");
	if(dwColIndex>3){
		dwColIndex=0;
	}
	HBRUSH hBr=::CreateSolidBrush(aColors[dwColIndex]);
	HBRUSH hBr2=::CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
	::FillRect(hDC,&rect,hBr);
	::FrameRect(hDC,&rect,hBr2);
	/*::MoveToEx(hDC,0,0,0);
	::LineTo(hDC,rect.right-rect.left,rect.bottom-rect.top);
	::MoveToEx(hDC,0,rect.bottom-rect.top-1,0);
	::LineTo(hDC,rect.right-rect.left,-1);*/
}


int WndProc(HWND hwnd,WORD wMsg,WPARAM wParam,LPARAM lParam)
{
	switch (wMsg)  
	{
	case WM_LBUTTONDOWN:
		{
			DWORD dwThread=(DWORD)::GetProp(hwnd,"WK_SHELL_THR");
			DWORD dwMsg=(DWORD)::GetProp(hwnd,"WK_SHELL_MSG");
			WPARAM hParent=(WPARAM)::GetProp(hwnd,"WK_SHELL_WND");
			PostThreadMessage(dwThread,dwMsg,hParent,0);
			break;
		}
	case WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			DrawMe(hwnd,hdc);
			return TRUE;
		}
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = ::BeginPaint(hwnd, &ps);
			DrawMe(hwnd,hdc);
			::EndPaint(hwnd, &ps);
			return TRUE;
		}
	default:
		DefWindowProc(hwnd, wMsg, wParam, lParam);
	}
	if(wMsg==WM_NCDESTROY){
		// ×èñòèì êëàññ...
		ATOM dwCNu=(ATOM)::GetProp(hwnd,"WK_SHELL_CNU");
		//UnregisterClass(MAKELONG(dwCNu,0));
	}else{
		// Ñ÷èòàåì îôôñåòû
		/*RECT rtP;
		GetWindowRect(GetParent(hwnd),&rtP);
		RECT rtT;
		GetWindowRect(hwnd,&rtT);
		long lOkXR=(long)::GetProp(hwnd,"WK_SHELL_XR")-1000;//(rtP.right-rtT.left)
		long lOkYB=(long)::GetProp(hwnd,"WK_SHELL_YB")-1000;//(rtP.bottom-rtT.top)
		int iX=0,iY=0;
		if(lOkXR!=(rtP.right-rtT.right)){
			iX=lOkXR-(rtP.right-rtT.right);
		}
		if(lOkYB!=(rtP.bottom-rtT.bottom)){
			iY=lOkYB-(rtP.bottom-rtT.bottom);
		}
		if(iY!=0 || iX!=0){
			//::SetWindowPos(hwnd,HWND_TOPMOST,rtT.left+iX,rtT.top+iY,rtT.right-rtT.left,rtT.bottom-rtT.top,SWP_ASYNCWINDOWPOS|SWP_SHOWWINDOW);
		}*/
	}
	return TRUE;
}

ATOM InitFloClass(char sz[256])
{
	//First, let's register the window class
	WNDCLASS hc;

	hc.style = 0;
	hc.lpfnWndProc = (WNDPROC)WndProc;
	hc.cbClsExtra = 0;
	hc.cbWndExtra =  0;//sizeof(CFloBT*);
	hc.hInstance = NULL;
	hc.hIcon = NULL;
	hc.hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));
	hc.hbrBackground = NULL;
	hc.lpszMenuName = NULL;
	hc.lpszClassName = sz;
	return RegisterClass(&hc);
}

HWND CreateFloWnd(RECT rect, HWND parent, char sz[256], BOOL bForSnake)
{
	//WS_EX_TOPMOST|WS_EX_TRANSPARENT
	DWORD dwEx=WS_EX_TOPMOST| WS_EX_NOPARENTNOTIFY;
	DWORD dwNo=WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	HWND hWnd=::CreateWindowEx(dwEx, sz, "", dwNo, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, parent, (HMENU)0xF3FF, NULL, NULL);
	/*
	RECT rtP;
	GetWindowRect(GetParent(hWnd),&rtP);
	RECT rtT;
	GetWindowRect(hWnd,&rtT);
	::SetProp(hWnd,"WK_SHELL_XR",(HANDLE)(rtP.right-rtT.right+1000));
	::SetProp(hWnd,"WK_SHELL_YB",(HANDLE)(rtP.bottom-rtT.bottom+1000));
	*/
	return hWnd;
}
