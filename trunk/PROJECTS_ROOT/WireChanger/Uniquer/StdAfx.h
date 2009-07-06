// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FB4F88AF_543E_4816_ACF6_7067BF501673__INCLUDED_)
#define AFX_STDAFX_H__FB4F88AF_543E_4816_ACF6_7067BF501673__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _NO_XMLHOTKEYSUPPORT
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#include <atlbase.h>
#include <windows.h>
#include <afxtempl.h>
#include <afxmt.h>
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "..\..\SmartWires\SystemUtils\Macros.h"
#endif // !defined(AFX_STDAFX_H__FB4F88AF_543E_4816_ACF6_7067BF501673__INCLUDED_)
