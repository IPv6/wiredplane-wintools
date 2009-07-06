// SmallXmlRpcClientServer.h : main header file for the SMALLXMLRPCCLIENTSERVER DLL
//

#if !defined(AFX_SMALLXMLRPCCLIENTSERVER_H__D5C46ADD_D8DB_4C33_89A4_978C10D87360__INCLUDED_)
#define AFX_SMALLXMLRPCCLIENTSERVER_H__D5C46ADD_D8DB_4C33_89A4_978C10D87360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#define RET_IF_FAULT(x)	if((x)==FALSE) return FALSE;
#include "exports.h"
#include "XmlRpcClient.h"
#include "XmlRpcServer.h"

/////////////////////////////////////////////////////////////////////////////
// CSmallXmlRpcClientServerApp
// See SmallXmlRpcClientServer.cpp for the implementation of this class
//

class CSmallXmlRpcClientServerApp : public CWinApp
{
public:
	CSmallXmlRpcClientServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallXmlRpcClientServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSmallXmlRpcClientServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMALLXMLRPCCLIENTSERVER_H__D5C46ADD_D8DB_4C33_89A4_978C10D87360__INCLUDED_)
extern CSmallXmlRpcClientServerApp theApp;
CString Format(const char* szFormat,...);