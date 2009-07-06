// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D72E9E3A_9A5D_4F07_8851_35D5EABBEBC2__INCLUDED_)
#define AFX_STDAFX_H__D72E9E3A_9A5D_4F07_8851_35D5EABBEBC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define _WIN32_WINNT	0x0501
#define WINVER			0x0500
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _NEW_
#include <afxwin.h>         // MFC core and standard components

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define _WIREKEYS
//#define _RICHEDIT_VER	0x0400
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
#include "resource.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "..\SmartWires\SystemUtils\Macros.h"
#define EXECRYPTOR_SALT	"WK999"
#define OPTIONS_TOOLBAR

//#define _USECASH
//#define _NOHOTKEYS
//#define GDI_NOICONS

//#pragma message("WARNING!!!! No licensing option is enabled!!!!!!!")
//#define FOR_FREE
#endif // !defined(AFX_STDAFX_H__D72E9E3A_9A5D_4F07_8851_35D5EABBEBC2__INCLUDED_)


