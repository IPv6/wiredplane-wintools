// HookCode.h: interface for the CHookCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
#define AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#define WM_STOPREC			WM_USER+1
#define WM_STARTREC			WM_USER+2
#define WM_REC_COMMAND		WM_USER+3
#define WM_REC_KEYK			WM_USER+4
#define WM_REC_MOUSK		WM_USER+5
#define WM_OPENRECORDER		WM_USER+100
#define WM_CLOSERECORDER	WM_USER+101
#include "..\..\SmartWires\SystemUtils\Macros.h"
#include "..\..\SmartWires\SystemUtils\SystemInfo.h"
#include "..\..\SmartWires\SystemUtils\SupportClasses.h"
CString _pl(const char* szText);
BOOL isMemEmpty(LPVOID pData, long count);
struct COptions
{
	BOOL bAP_Add2WK;
	BOOL bRecPauses;
	long lPauseLevel;
	BOOL bAP_OpenInEd;
	long bAppSwitchesRTyp;
	CString sLastMacroName;
	CString sLastMacroContent;
	long lMFormat;
};

class CMacroRecord
{
public:
	// Для клавиатуры
	BOOL bUp;
	BOOL bDown;
	DWORD dwVkCode;
	BOOL bExtended;
	// Для мыши
	long dwXRel;
	long dwYRel;
	BOOL bRelativePos;
	// Дубликат сообщения
	DWORD dwMessage;
	LPARAM dwLParam;
	WPARAM dwHParam;
	DWORD dwTimeOut;
	// Активное приложение
	CString sActWndExe;
	CString sActWndTitle;
	HWND hActWnd;
	// Агрегаты
	DWORD dwTimeOutDelta;
	// Функции
	CMacroRecord()
	{
		bUp=0;
		bDown=0;
		hActWnd=0;
		dwVkCode=0;
		bExtended=0;
		sActWndExe="";
		sActWndTitle="";
		dwXRel=-1;
		dwYRel=-1;
		dwTimeOut=0;
		dwMessage=0;
		bRelativePos=0;
		dwLParam=0;
		dwHParam=0;
		dwTimeOutDelta=0;
	}
	CMacroRecord& operator=(CMacroRecord& CopyObj)
	{
		hActWnd=CopyObj.hActWnd;
		bUp=CopyObj.bUp;
		bDown=CopyObj.bDown;
		dwXRel=CopyObj.dwXRel;
		dwYRel=CopyObj.dwYRel;
		dwVkCode=CopyObj.dwVkCode;
		sActWndExe=CopyObj.sActWndExe;
		sActWndTitle=CopyObj.sActWndTitle;
		dwTimeOut=CopyObj.dwTimeOut;
		dwMessage=CopyObj.dwMessage;
		bRelativePos=CopyObj.bRelativePos;
		dwLParam=CopyObj.dwLParam;
		dwHParam=CopyObj.dwHParam;
		dwTimeOutDelta=CopyObj.dwTimeOutDelta;
		bExtended=CopyObj.bExtended;
		return *this;
	}
};
#define WPPREF "WireKeys.PlayRecord("
#endif // !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
