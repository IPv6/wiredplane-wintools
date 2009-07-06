// WKUtils2.h : main header file for the WKUtils2 DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class CWKUtils2App : public CWinApp
{
public:
	CWKUtils2App();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
