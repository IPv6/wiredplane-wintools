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

#if !defined(AFX_HYPERLINK_H__7544C8BB_6031_422B_BA0E_820CCBD5DC25__INCLUDED_)
#define AFX_HYPERLINK_H__7544C8BB_6031_422B_BA0E_820CCBD5DC25__INCLUDED_

#include <atlbase.h>
#include <windows.h>

class CHlink_autoinitializer
{
public:
	CHlink_autoinitializer();
	~CHlink_autoinitializer();
protected:
	HMODULE hModule;
};

class CHyperlink  
{
public:
	static HCURSOR handcursor;
	static CHlink_autoinitializer autoinitializer;
	static int WndProc(HWND hwnd,WORD wMsg,WPARAM wParam,LPARAM lParam);

	int bIsCreated;
	CHyperlink();
	virtual ~CHyperlink();

	bool create(int resourceid, HWND parent);
	bool create(RECT rect, const char *url, HWND parent);
	bool create(int x1, int y1, int x2, int y2, const char *url, HWND parent);
	char m_Url[1024];
	char m_UrlVisible[1024];
	HWND m_hWnd;
	HWND hParent;
};
#endif // !defined(AFX_HYPERLINK_H__7544C8BB_6031_422B_BA0E_820CCBD5DC25__INCLUDED_)
