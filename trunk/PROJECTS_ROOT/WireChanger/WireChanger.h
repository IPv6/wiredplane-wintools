//

#if !defined(AFX_COSSORGANIZATOR_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_)
#define AFX_COSSORGANIZATOR_H__9F8FC164_ED4F_4167_93C4_C85611144B33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include <afxtempl.h>
#include "HtmlWallpaper.h"
#include "WallpaperUtils.h"
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// AppMainApp:
//

class AppMainApp : public CWinApp
{
public:
	//
	static UINT iWM_THIS;
	static UINT iWM_SSAVE;
	static UINT iWM_WIRENOTE;
	static CRect rFakedRect;
	static CWnd* m_pFakeWnd;
	CRect GetDesktopRECT();
	CImageList MainImageList;
	AppMainApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AppMainApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
private:
	BOOL m_bATLInited;
	BOOL InitATL();
	//{{AFX_MSG(AppMainApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
int AddBitmapToIList(CImageList& list, UINT bmpID);
void _stdcall RefreshRecipientMenu(CDWordArray* pTrack, CMenu*& pRecMenu, DWORD dwCommOffset);
#endif 
