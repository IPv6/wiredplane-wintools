// Pickers.h : main header file for the PICKERS DLL
//

#if !defined(AFX_PICKERS_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_)
#define AFX_PICKERS_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_

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
/////////////////////////////////////////////////////////////////////////////
// CPickersApp
// See Pickers.cpp for the implementation of this class
//

class CPickersApp : public CWinApp
{
public:
	CPickersApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickersApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPickersApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICKERS_H__ACF0B2BF_7856_4B64_A374_DF314048192F__INCLUDED_)
