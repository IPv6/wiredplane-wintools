// WKUtils.h : main header file for the WKUTILS DLL
//

#if !defined(AFX_WKUTILS_H__93B39C93_F353_42C2_81BD_4C2F864EB196__INCLUDED_)
#define AFX_WKUTILS_H__93B39C93_F353_42C2_81BD_4C2F864EB196__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
//#define _HOGSUPPORT
#include "..\..\SmartWires\SystemUtils\DataXMLSaver.h"
#include "..\..\SmartWires\Atl\File\verinfo.h"
#ifdef _HOGSUPPORT
#include "..\..\SmartWires\BitmapUtils\HogVideo.h"
#endif

class CWKUtilsApp : public CWinApp
{
public:
	CWKUtilsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWKUtilsApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWKUtilsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_WKUTILS_H__93B39C93_F353_42C2_81BD_4C2F864EB196__INCLUDED_)
