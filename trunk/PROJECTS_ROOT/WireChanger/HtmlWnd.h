#if !defined( _ITREESURF__HTMLWND_H__INCLUDED_ )
#define _ITREESURF__HTMLWND_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// HtmlWnd.h : header file     
//     

#include "WebBrowser2.h"


////////////////////////////////////////////////////////////////////////////
// CHtmlWnd window

class CHtmlWnd : public CWebBrowser2
{
// Construction
public:
	CWnd* m_pFakeWnd;
	HBITMAP hExplorerContent;
	HANDLE hHtmlLoadedEvent;
	CHtmlWnd();
	BOOL CreateBrowser(const char* szStartUrl);
	BOOL NavigateToUrl(const char* szStartUrl);
	DECLARE_EVENTSINK_MAP()

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHtmlWnd();
	virtual void PostNcDestroy();
	void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlWnd)
	//}}AFX_MSG
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel);
	virtual void NavigateError(LPDISPATCH* ppDisp, VARIANT* URL,VARIANT* TargetFrameName, VARIANT* StatusCode, BOOL* bCancel);
	DECLARE_MESSAGE_MAP()

  // Event map declaration for WebBrowser control events.

};

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__HTMLWND_H__INCLUDED_ )


