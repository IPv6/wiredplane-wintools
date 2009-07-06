// HtmlWnd.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "HtmlWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CHtmlWnd
CHtmlWnd::CHtmlWnd()
{
	hHtmlLoadedEvent = CreateEvent(NULL,FALSE,FALSE,"");
	hExplorerContent=NULL;
}

CHtmlWnd::~CHtmlWnd()
{
	//afxOccManager->m_pOccManager->
	//COleControlSite
	//m_pCtrlCont->FreezeAllEvents()
	//::CoFreeUnusedLibraries();
	m_pFakeWnd->DestroyWindow();
	delete m_pFakeWnd;
	m_pFakeWnd=NULL;
	CloseHandle(hHtmlLoadedEvent);
	ClearBitmap(hExplorerContent);
}

BOOL CHtmlWnd::NavigateToUrl(const char* szStartUrl)
{
	CString sFullUrl=szStartUrl;//Format("file://%s",szStartUrl);
	COleVariant vURL(sFullUrl, VT_BSTR);
	COleVariant vFlags((long) (navNoHistory|navNoReadFromCache|navNoWriteToCache), VT_I4);
	COleVariant vTargetFrameName((LPCSTR)NULL, VT_BSTR);
	COleSafeArray vPostData;
	COleVariant vHeaders((LPCSTR)NULL, VT_BSTR);
	ResetEvent(hHtmlLoadedEvent);//pWebBrowserHost->PutSilent(-1);
	Navigate2(vURL, vFlags, vTargetFrameName, vPostData, vHeaders);
	return TRUE;
}

BOOL CHtmlWnd::CreateBrowser(const char* szStartUrl)
{
	CRect rDesktopRECT;
	GetDesktopWindow()->GetWindowRect(&rDesktopRECT);
	rDesktopRECT.OffsetRect(-30000,-30000);
	LPCTSTR szClass = AfxRegisterWndClass(NULL);
	m_pFakeWnd = new CWnd;
	BOOL bRes=m_pFakeWnd->CreateEx(WS_EX_TOOLWINDOW, szClass, "IPV6_BROWSER", WS_POPUP, rDesktopRECT, NULL, 0);
	CRect rWPLocalWndRect(CPoint(0,0),rDesktopRECT.Size());
	if(this->Create( "BROWSER", WS_CHILD | WS_VISIBLE, rWPLocalWndRect, m_pFakeWnd, 0)){//AFX_IDW_PANE_FIRST
		// ModifyStyle(WS_BORDER|WS_DLGFRAME|WS_THICKFRAME,0);
		// ModifyStyleEx(WS_EX_DLGMODALFRAME|WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE|WS_EX_STATICEDGE|WS_EX_APPWINDOW,0);
		// Перенаправляем...
		return NavigateToUrl(szStartUrl);
	}
	return FALSE;
}

void CHtmlWnd::PostNcDestroy()
{
	CWebBrowser2::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(CHtmlWnd, CWebBrowser2)
	//{{AFX_MSG_MAP(CHtmlWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// Manually added event sink map
BEGIN_EVENTSINK_MAP(CHtmlWnd, CWebBrowser2)
	ON_EVENT(CHtmlWnd, 0, DISPID_NEWWINDOW2, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CHtmlWnd, 0, DISPID_DOCUMENTCOMPLETE, DocumentComplete, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CHtmlWnd, 0, 271/*DISPID_NAVIGATEERROR*/, NavigateError, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

////////////////////////////////////////////////////////////////////////////
// CHtmlWnd message handlers
void CHtmlWnd::NavigateError(LPDISPATCH* ppDisp, VARIANT* URL,VARIANT* TargetFrameName, VARIANT* StatusCode, BOOL* bCancel)
{
	if(bCancel){
		*bCancel=TRUE;
	}
}

void CHtmlWnd::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
	if(bCancel){
		*bCancel=TRUE;
	}
}

void CHtmlWnd::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	static long lDocComplLock=0;
	if(lDocComplLock>0){
		return;
	}
	SimpleTracker Track(lDocComplLock);
	HRESULT hr=S_OK;
	if(pDisp!=NULL){
		IUnknown * punkFiredEvent = NULL; 
		HRESULT hr = pDisp->QueryInterface(IID_IUnknown, (LPVOID *) &punkFiredEvent);
		if(SUCCEEDED( hr ) && punkFiredEvent!=GetControlUnknown()){// Это фрейм или еще какая напасть...
			hr=S_FALSE;
		}
		punkFiredEvent->Release();
	}
	if(hr==S_OK){// Рисуем страницу в память...
		ClearBitmap(hExplorerContent);
		CDC dcCompatible;
		CDesktopDC dcDesk;
		BOOL bRes=dcCompatible.CreateCompatibleDC(&dcDesk);
		CRect rControlWnd;
		GetClientRect(&rControlWnd);
		hExplorerContent=CreateCompatibleBitmap(dcDesk, rControlWnd.Width(), rControlWnd.Height());
		HGDIOBJ hTmpBmp=dcCompatible.SelectObject(hExplorerContent);
		CComPtr<IUnknown> ptr=GetControlUnknown();
		if(ptr){
			hr=OleDraw(ptr,DVASPECT_CONTENT,dcCompatible.GetSafeHdc(),&rControlWnd);
			/*#pragma message("------------- IViewObject leak!!! ------")
			CComQIPtr<IViewObject> pHtmlRender(ptr);
			if(pHtmlRender!=NULL){
			hr=OleDraw(pHtmlRender,DVASPECT_CONTENT,dcCompatible.GetSafeHdc(),&rControlWnd);
			}*/
		}
	}
	if(SUCCEEDED(hr)){
		SetEvent(hHtmlLoadedEvent);
	}
}