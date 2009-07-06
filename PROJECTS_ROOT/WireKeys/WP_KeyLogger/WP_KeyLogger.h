// WKKeyLogger.h : main header file for the CLOCKFONT DLL
//

#if !defined(AFX_CLOCKFONT_H__C80658DE_F831_48C1_8AC8_57C3C48BBC71__INCLUDED_)
#define AFX_CLOCKFONT_H__C80658DE_F831_48C1_8AC8_57C3C48BBC71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "../WKPlugin.h"
#include "WP_KeyLoggerHook/HookCode.h"
#define LAUNCH_CHAR	0x01
#define ENTER_CHAR	0x02
#define JTT_BUFFER	1020
void ReplaceChar(char* szSrc, char cFrom, char cTo);
void ReplaceChar(char* szSrc, char cFrom, const char* szTo);
char* _pl(const char* szText);
HINSTANCE& GetHookInstance();
struct CWKATInfo
{
	BOOL bATWasChanged;
	char* szATKeys[MAX_AT];
	char* szATValues[MAX_AT];
};
typedef BOOL (WINAPI *_SetHook)(DWORD);
typedef void (WINAPI *_SetHookInfoObject)(CWKKeyLoggerInfo*);
typedef void (WINAPI *_TweakTriggers)(int iType, BOOL bVal);
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOCKFONT_H__C80658DE_F831_48C1_8AC8_57C3C48BBC71__INCLUDED_)
