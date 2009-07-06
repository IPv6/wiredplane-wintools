// WKKeyMaster.h : main header file for the WKKEYMASTER DLL
//

#if !defined(AFX_WKKEYMASTER_H__4CA75D36_5C7D_49AC_A574_3077D5E6B1D0__INCLUDED_)
#define AFX_WKKEYMASTER_H__4CA75D36_5C7D_49AC_A574_3077D5E6B1D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "..\..\SmartWires\SystemUtils\Macros.h"
#include "..\..\SmartWires\SystemUtils\hotkeys.h"
#include "..\..\SmartWires\SmartWndSizer\SmartWndSizer.h"
#include "..\..\SmartWires\BitmapUtils\DelayLoadFuncs.h"
#include "..\WKPlugin.h"
#include "DLG_KMaster.h"
/////////////////////////////////////////////////////////////////////////////
// CWKKeyMasterApp
// See WKKeyMaster.cpp for the implementation of this class
//
struct GPLUGINIni
{
	BOOL bAutoRepeat;
	BOOL bDistingCorners;
	BOOL bDistingLR;
	BOOL bShiftDropsCapslock;
	BOOL bBlockApps;
	BOOL bBlockWin;
	BOOL bUseMouseKeys;
	long lTranspLevel;
	long lPHTimeout;
	BOOL bHideCursor;
	BOOL bBlockMouse;
	BOOL bScanOSD;
	BOOL bUnpauseOnHK;
	BOOL bAddUnpause;
	BOOL unpauseOnHKSwitch;
	BOOL bCLIcon;
	BOOL bNLIcon;
	BOOL bSLIcon;
	BOOL enableHID;
	char szWinBlockMask[2048];
	GPLUGINIni()
	{
		enableHID=1;
		unpauseOnHKSwitch=1;
		bAddUnpause=1;
		bUnpauseOnHK=1;
		szWinBlockMask[0]=0;
		bCLIcon=0;
		bSLIcon=0;
		bNLIcon=0;
		bScanOSD=0;
		bHideCursor=1;
		bBlockApps=0;
		bBlockWin=0;
		bDistingLR=1;
		bUseMouseKeys=1;
		bShiftDropsCapslock=0;
		lTranspLevel=100;
		bDistingCorners=1;
		bBlockMouse=0;
		lPHTimeout=0;
	};
};

#define MSGKB_ACTIVHGUID	WM_USER+1
#define MSGKB_EXIT			WM_USER+2
#define MSGKB_EXIT_RW		WM_USER+3
#define MSGKB_ACTIVHGUID_RW	WM_USER+4
#define MSGKB_JUSTINFORM	WM_USER+99
#define MSGKB_CURSORHIDE	WM_USER+100
#define MSGKB_SHOWOSD		WM_USER+101
#define MSGKB_SHOWOSD2		WM_USER+102
#define MSGKB_SHOWOSD3		WM_USER+103
#define MSGKB_JUSTINFORM_MM WM_USER+104
#define MSGKB_JUSTINFORM_EV WM_USER+105
#define GHOOKACTION			"WIREKEYS_GHOOKACTIONEVENT"
class CWKKeyMasterApp : public CWinApp
{
public:
	CWKKeyMasterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWKKeyMasterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWKKeyMasterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

typedef BOOL (WINAPI *_InstallHook)(BOOL,BOOL,HWND,BOOL);
typedef BOOL (WINAPI *_SetOptions)(BOOL,DWORD&, DWORD, DWORD);
typedef BOOL (WINAPI *_SetActivity)(BOOL bSetActive);
typedef void (WINAPI *_ResetKBBuffer)(BOOL bADOnly);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_WKKEYMASTER_H__4CA75D36_5C7D_49AC_A574_3077D5E6B1D0__INCLUDED_)
