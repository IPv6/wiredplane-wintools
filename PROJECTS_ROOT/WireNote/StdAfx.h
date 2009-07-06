// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D72E9E3A_9A5D_4F07_8851_35D5EABBEBC2__INCLUDED_)
#define AFX_STDAFX_H__D72E9E3A_9A5D_4F07_8851_35D5EABBEBC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define _WIN32_WINNT 0x0400
#define _WIN32_IE 0x0500
//#define WINVER 0x0500
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxwin.h>         // MFC core and standard components

#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <Winsock2.h>
#include <afxsock.h>		// MFC socket extensions
#include <atlconv.h>
#include <atlbase.h>
#include <afxtempl.h>
#include <afxmt.h>
#include <Lm.h>
#include <Nb30.h>
#include <Wininet.h>
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "resource.h"
#include "..\SmartWires\SystemUtils\Macros.h"
#define ALERTBOX
#ifdef ALERTBOX
void AlertBox(const char* szTitle, const char* szText, DWORD dwTime);
#endif
#define EXECRYPTOR_SALT "WN1"
#define _NOADVERT
#endif // !defined(AFX_STDAFX_H__D72E9E3A_9A5D_4F07_8851_35D5EABBEBC2__INCLUDED_)


