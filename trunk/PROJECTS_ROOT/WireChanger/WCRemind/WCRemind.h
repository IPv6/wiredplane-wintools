// WCRemind.h : main header file for the WCRemind DLL
//

#if !defined(AFX_WCRemind_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_)
#define AFX_WCRemind_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "..\..\SmartWires\SystemUtils\WCRenders.h"
#include "..\..\SmartWires\SystemUtils\SupportClasses.h"
#include "..\..\SmartWires\SystemUtils\DataXMLSaver.h"
#include "..\..\SmartWires\SystemUtils\FileInfo.h"
#include "..\..\SmartWires\SystemUtils\RemindClasses.h"
#include "..\..\SmartWires\GUIClasses\Cal\WMDateCtrl.h"
#include "..\..\SmartWires\GUIClasses\Cal\WVCellData.h"
#include "..\..\SmartWires\GUIClasses\Cal\WVCellDataItem.h"
#include "..\..\SmartWires\SmartWndSizer\ResizeableDialog.h"
#include "..\..\SmartWires\BitmapUtils\BMPUtil.h"
/////////////////////////////////////////////////////////////////////////////
// CWCRemindApp
// See WCRemind.cpp for the implementation of this class
//

class CWCRemindApp : public CWinApp
{
public:
	CWCRemindApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWCRemindApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWCRemindApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
BOOL WINAPI SortReminders();
int WINAPI GetReminderIndex(const char* szKey);
BOOL WINAPI PutReminderRaw(char szKey[64], CWPReminder& pIn, BOOL bAdjustOld);
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
CString _lx(const char* szText,const char* szDef=0);
#endif // !defined(AFX_WCRemind_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_)
