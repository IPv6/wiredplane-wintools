#if !defined(AFX_HTMLWALLPAPER_H__89D3A115_1AE5_495F_B77C_02A6CCCF4BFA__INCLUDED_)
#define AFX_HTMLWALLPAPER_H__89D3A115_1AE5_495F_B77C_02A6CCCF4BFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlWallpaper.h : header file
//

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

/////////////////////////////////////////////////////////////////////////////
class CSmartWBrowserThread;
class CSmartWBrowser
{
public:
	CSmartWBrowserThread* pBr;
	CSmartWBrowser();
	~CSmartWBrowser();
	BOOL LoadPage(const char* szURL, DWORD dwTimeout, BOOL* bWasTimedOut=NULL);
	BOOL CopyPageImage(HBITMAP& hTo, CSize size);
	//BOOL InitWebBrowser(const char* szURL);
	//BOOL DeinitWebBrowser();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLWALLPAPER_H__89D3A115_1AE5_495F_B77C_02A6CCCF4BFA__INCLUDED_)
