// HookCode.h: interface for the CHookCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
#define AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"
#include "..\..\SmartWires\SystemUtils\Macros.h"
#define GHOOKACTION	"WIREKEYS_GHOOKACTIONEVENT"
BOOL WINAPI SetHook(BOOL bSet,char*);
BOOL isMemEmpty(LPVOID pData, long count);
BOOL IsWindowsSystemWnd(HWND hWin, BOOL);
BOOL IsWindowStylesOk(HWND hWind);
DWORD WINAPI SnapWindow_InThread(LPVOID pData);
DWORD WINAPI AutoHideWindow_InThread(LPVOID pData);
char* _pl(const char* szText);
struct CWKHekperInfo
{
	BOOL bSnapMovements;
	BOOL bCatchAutoMinimize;
	char szAutoMinimizeTitle[256];
	int iStickness;
	BOOL bSnapToScreen;
	int iSnapToScreenStickness;
	BOOL bSnapToScreenForce;
	BOOL bPreserveMenuOnRollup;
	int iINACTSEC;
	BOOL bInjectItems;
	BOOL bInjectIt1;
	BOOL bInjectIt2;
	BOOL bInjectIt3;
	BOOL bInjectIt4;
	BOOL bInjectIt5;
	BOOL bInjectIt6;
	BOOL bInjectIt7;
	BOOL bCatchRollDock;
	BOOL bCatchRollExpand;
	BOOL bBlockFocuStealing;
	BOOL bSnapToScreenForceFull;
	char szAutoMinToTray[1024];
	char szAutoMinToFloat[1024];
	BOOL bOntopicon;
	char szAutoDisalbe[256];
	BOOL bAllowReplaceExplorerKeys;
};

struct CWKHekperInfoLocal
{
	char szExclusions[1024];
};
#endif // !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
