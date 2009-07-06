// WebBrowserDlg.h : main header file for the WEBBROWSERDLG application
//

#if !defined(AFX_WEBBROWSERDLG_H__9CEAB404_C41E_42BD_A3F2_C10E6286C230__INCLUDED_)
#define AFX_WEBBROWSERDLG_H__9CEAB404_C41E_42BD_A3F2_C10E6286C230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#define CRT_WITHCSTR
#include "..\..\SmartWires\Atl\crt_common.h"
#include "..\..\SmartWires\SmartWndSizer\ResizeableDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CWebBrowserDlgApp:
// See WebBrowserDlg.cpp for the implementation of this class
//

class CWebBrowserDlgApp : public CWinApp
{
public:
	CWebBrowserDlgApp();
	virtual ~CWebBrowserDlgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebBrowserDlgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	class CImpIDispatch* m_pDispOM;

protected:

#ifdef _DEBUG
	CMemoryState	*m_memState;
#endif

	//{{AFX_MSG(CWebBrowserDlgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CWebBrowserDlgApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

char UniCode(const char* szValue);
char HexCode(const char* szRawValue);
CString UnescapeString(const char* szString);
CRect GetDesktopRect();
CString TimeFormat(COleDateTime tm,BOOL bDef, BOOL bNoSecs);
CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString);
void ConvertToArray(const char* szData, CStringArray& aStrings, CString c, char cComment=0);


#endif // !defined(AFX_WEBBROWSERDLG_H__9CEAB404_C41E_42BD_A3F2_C10E6286C230__INCLUDED_)
