// HookCode.h: interface for the CHookCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
#define AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
BOOL WINAPI SetHook(HWND hWin);
BOOL isMemEmpty(LPVOID pData, long count);
struct CIconsFontInfo
{
	LOGFONT	g_Font;
	DWORD	g_FontColor;
	DWORD	g_FontBgColor;
};

struct CIconsFontOptions
{
	BOOL	bSetFont;
	BOOL	bSetTxColor;
	BOOL	bSetBgColor;
	BOOL	bSetTransp;
};
#endif // !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
