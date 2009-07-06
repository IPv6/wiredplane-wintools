// HookCode.h: interface for the CHookCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
#define AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include <stdio.h>
#include <memory.h> 
#include "..\..\..\SmartWires\SystemUtils\Macros.h"
#define MAX_AT		200
#define MAX_KEY_LEN	100
struct CWKKeyLoggerInfo
{
	BOOL bLogChars;
	BOOL bATChars;
	DWORD dwMaxSize;
	char szTargetFile[MAX_PATH];
	char szValidWndTitles[1024];
	char szValidWndClasses[1024];
	char szWndTitlesForAT[1024];
	DWORD dwMinSize;
	BOOL bInsByWK;
	BOOL bmfocus;
	BOOL bonSpace;
	BOOL bWatchUps;
	BOOL bIgnoreCase;
	BOOL bEnableSound;
	char szEnableSound[MAX_PATH];
	BOOL logFileDt;
	int iSaveScr;
	int iSaveScrLast;
	int iSpymode;
};
#endif // !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
