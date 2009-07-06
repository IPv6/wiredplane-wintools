//

#if !defined(AFX_SORGANIZATORDLG_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_)
#define AFX_SORGANIZATORDLG_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include <afxtempl.h>
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// AppMainApp:
// See SORGANIZATORDLG.cpp for the implementation of this class
//

class AppMainApp : public CWinApp
{
public:
	DWORD dwTID;
	static UINT iWM_THIS;
	static UINT iWM_WIRENOTE;
	static UINT iWM_EXTERN;
	static CRect rDesktopRECT;
	static CRect rFakedRect;
	static CWnd* m_pFakeWnd;
	CImageList MainImageList;
	AppMainApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AppMainApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(AppMainApp)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#define SPY_MODE_APP	"soundman.exe"
BOOL& spyMode();
BOOL& underSpyMode();
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
BOOL IsVista();
BOOL IsVistaSoundControl();
int AddBitmapToIList(CImageList& list, UINT bmpID);
void _stdcall RefreshRecipientMenu(CDWordArray* pTrack, CMenu*& pRecMenu, DWORD dwCommOffset);
#endif // !defined(AFX_SORGANIZATORDLG_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_)
