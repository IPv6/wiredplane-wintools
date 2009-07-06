// WireNote.h : main header file for the WPOSSORGANIZATOR application
//

#if !defined(AFX_WPOSSORGANIZATOR_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_)
#define AFX_WPOSSORGANIZATOR_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include <afxtempl.h>
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWPOSSOrganizatorApp:
// See WPOSSOrganizator.cpp for the implementation of this class
//

class CWPOSSOrganizatorApp : public CWinApp
{
public:
	DWORD dwTID;
	static UINT iWM_WIRECHANGER;
	static UINT iWM_WIREKEYS;
	static UINT iWM_THIS;
	static CRect rDesktopRECT;
	static CRect rFakedRect;
	static DWORD aAppIcons[];
	static CWnd* m_pFakeWnd;
	CStringArray aPriorityStrings;
	DWORD dwOnlineStatusStartBitmap;
	DWORD dwMsgBitmap;
	CImageList MainImageList;
	CWPOSSOrganizatorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWPOSSOrganizatorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWPOSSOrganizatorApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
int AddBitmapToIList(CImageList& list, UINT bmpID, BOOL bWithHeap=FALSE);
void _stdcall RefreshRecipientMenu(CDWordArray* pTrack, CMenu*& pRecMenu);
BOOL IsVista();
#endif // !defined(AFX_WPOSSORGANIZATOR_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_)
