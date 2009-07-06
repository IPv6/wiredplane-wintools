/**
* Copyright (C) 2002-2005
* W3L GmbH
* Technologiezentrum Ruhr
* Universit‰tsstraﬂe 142
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

#include "Hyperlink.h"

//#include <shellapi.h>

#define STATIC_HYPER_WINDOW_CLASS "STATIC_HYPER"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CHlink_autoinitializer::CHlink_autoinitializer() : hModule(NULL)
{
	//First, let's register the window class
	WNDCLASS hc;

	hc.style = 0;
	hc.lpfnWndProc = (WNDPROC)CHyperlink::WndProc;
	hc.cbClsExtra = 0;
	hc.cbWndExtra = sizeof(CHyperlink*);
	hc.hInstance = NULL;
	hc.hIcon = NULL;
	hc.hCursor = NULL;
	hc.hbrBackground = NULL;
	hc.lpszMenuName = NULL;
	hc.lpszClassName = STATIC_HYPER_WINDOW_CLASS;
	RegisterClass(&hc);


	//Now, try to find a hand icon...
	CHyperlink::handcursor = ::LoadCursor(NULL, MAKEINTRESOURCE(32649)); //32649 == IDC_HAND
	if (CHyperlink::handcursor == NULL)
	{
		TCHAR szWindowsDir[MAX_PATH];
		GetWindowsDirectory(szWindowsDir ,MAX_PATH);
		strcat(szWindowsDir,"\\winhlp32.exe");
		hModule = LoadLibrary(szWindowsDir);		
		if (hModule) 
			CHyperlink::handcursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
	}
}

CHlink_autoinitializer::~CHlink_autoinitializer()
{
	if (hModule != NULL)
		FreeLibrary(hModule);
}

CHlink_autoinitializer CHyperlink::autoinitializer;
HCURSOR CHyperlink::handcursor = NULL;

CHyperlink::CHyperlink() : m_hWnd(NULL)
{
	bIsCreated=0;
	m_Url[0]=0;
	m_UrlVisible[0]=0;
}

CHyperlink::~CHyperlink()
{
	if (m_hWnd)
		::DestroyWindow(m_hWnd);
}


bool CHyperlink::create(int resourceid, HWND parent)
{
	bIsCreated=1;
	HWND old = ::GetDlgItem(parent, resourceid);
	if (old != NULL)
	{
		RECT rect; char url[256];

		::GetWindowText(old, url, sizeof(url));
		::GetWindowRect(old, &rect);
		
		POINT pt;
		pt.x=rect.left;
		pt.y=rect.top;
		::ScreenToClient(parent, &pt);
		
		POINT pt2;
		pt2.x=rect.right;
		pt2.y=rect.bottom;
		::ScreenToClient(parent, &pt2);
		
		rect.left=pt.x;
		rect.top=pt.y;
		rect.right=pt2.x;
		rect.bottom=pt2.y;

		//finally, destroy the old label
		if (create(rect, url, parent))
			::DestroyWindow(old);
	}
	return m_hWnd != NULL;
}

bool CHyperlink::create(RECT rect, const char *url, HWND parent)
{
	bIsCreated=1;
	if (url != NULL){
		strcpy(m_Url, url);
	}

	m_hWnd = ::CreateWindow( STATIC_HYPER_WINDOW_CLASS, m_Url, WS_CHILD | WS_VISIBLE, 
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 
		parent, NULL, NULL, NULL);

	::SetWindowLong( m_hWnd, GWL_USERDATA, (LONG)this);
	hParent=parent;
	return m_hWnd != NULL;
}

bool CHyperlink::create(int x1, int y1, int x2, int y2, const char *url, HWND parent)
{
	bIsCreated=1;
	RECT rect; 
	rect.left = x1;
	rect.top = y1;
	rect.right = x2;
	rect.bottom = y2;
	return create(rect, url, parent);
}




int CHyperlink::WndProc(HWND hwnd,WORD wMsg,WPARAM wParam,LPARAM lParam)
{
	CHyperlink *hl = (CHyperlink*)GetWindowLong(hwnd, GWL_USERDATA);

	switch (wMsg)  
	{
	case WM_LBUTTONDOWN:
		if (((UINT)::ShellExecute(NULL, _T("open"), hl->m_Url, NULL, NULL, SW_SHOWNORMAL)) <= 32){
			MessageBeep(0);
		}
		break;
	case WM_MOUSEMOVE:

		break;
	case WM_PAINT:
		{
			HDC hDC; PAINTSTRUCT ps;
			hDC = ::BeginPaint(hwnd, &ps);
			if (hl == NULL)
				return 0;

			RECT rect;
			::GetClientRect(hwnd, &rect);

			/*HFONT font = ::CreateFont( 16, //height
				7, //average char width
				0, //angle of escapement
				0, //base-line orientation angle
				FW_NORMAL,	//font weight
				FALSE,		//italic
				TRUE,		//underline
				FALSE,		//strikeout
				ANSI_CHARSET,			//charset identifier
				OUT_DEFAULT_PRECIS,		//ouput precision
				CLIP_DEFAULT_PRECIS,	//clipping precision
				DEFAULT_QUALITY,	//output quality
				DEFAULT_PITCH,			//pitch and family
				"Tahoma");*/
			HFONT font =(HFONT)::SendMessage(hl->hParent, WM_GETFONT, 0, 0);

			::SelectObject(hDC, font);
			::SetTextColor(hDC, RGB(0,0,200));
			::SetBkMode(hDC, TRANSPARENT);
			char* sz=hl->m_UrlVisible[0]!=0?hl->m_UrlVisible:hl->m_Url;
			::DrawText(hDC, sz, strlen(sz), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			//::DeleteObject(font);

			::EndPaint(hwnd, &ps);

			return TRUE;
		}
	case WM_SETCURSOR:
		{
			if (CHyperlink::handcursor)
				::SetCursor(CHyperlink::handcursor);
		}
	default:
		DefWindowProc(hwnd, wMsg, wParam, lParam);
	}

	return TRUE;
}
