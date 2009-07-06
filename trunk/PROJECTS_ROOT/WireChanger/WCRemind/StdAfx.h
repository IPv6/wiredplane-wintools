// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F864B7F2_4F7E_43D6_BC71_1B8D644E6519__INCLUDED_)
#define AFX_STDAFX_H__F864B7F2_4F7E_43D6_BC71_1B8D644E6519__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#include <afxwin.h>
#include <afxcmn.h>
#include <afxdtctl.h>
#include <afxtempl.h>
#include <math.h>
#include <atlconv.h>
#include <atlbase.h>
#include <afxmt.h>
#define _NO_XMLHOTKEYSUPPORT
#include "MonthCalCtrlEx.h"
#include "PrivateProfile.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
CString Format(long l);
CString _lx(const char* szText,const char* szDef);
#define _TRANM(X)  _lx(CString("RMX.UI.MONTH")+Format(X.GetMonth()),X.Format("%B"))
#define _TRANW(X)  _lx(CString("RMX.UI.WDAY")+Format(X.GetDayOfWeek()),X.Format("%A"))
#define _TRANWS(X)  _lx(CString("RMX.UI.WDAYS")+Format(X.GetDayOfWeek()),X.Format("%A"))

#endif // !defined(AFX_STDAFX_H__F864B7F2_4F7E_43D6_BC71_1B8D644E6519__INCLUDED_)
