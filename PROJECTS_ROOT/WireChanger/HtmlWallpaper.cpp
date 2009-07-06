// HtmlWallpaper.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "HtmlWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CSmartWBrowserThread:public CWinThread
{
	DECLARE_DYNCREATE(CSmartWBrowserThread)
public:
	int bBrowserEnabled;
	CHtmlWnd* wnd;
	//
	CSmartWBrowserThread()
	{
		wnd=NULL;
		bBrowserEnabled=-1;
	};
	//
	~CSmartWBrowserThread()
	{
		bBrowserEnabled=FALSE;
		if(wnd){
			wnd->DestroyWindow();
			delete wnd;
			wnd=NULL;
		}
		CoUninitialize();
	};

	virtual BOOL InitInstance()
	{
		HRESULT hRes = OleInitialize(NULL);
		m_bAutoDelete=TRUE;
		return TRUE;
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(pMsg->message==WBT_EXIT){
			::PostQuitMessage(0);
			return TRUE;
		}
		if(pMsg->message==WBT_LOAD){
			if (pMsg->wParam!=NULL){
				char* szUrl=(char*)pMsg->wParam;
				if(szUrl){
					if(wnd==NULL){
						wnd=new CHtmlWnd();
						bBrowserEnabled=wnd->CreateBrowser(szUrl);
					}else{
						wnd->NavigateToUrl(szUrl);
					}
					free(szUrl);
					if(bBrowserEnabled){
						// Лупим мессаги пока не кончатся
						MSG msg;
						BOOL bRet=0;
						while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0){ 
							if (msg.message==WBT_STOP || msg.message==WBT_EXIT || bRet == -1){
								break;
							}
							TranslateMessage(&msg); 
							DispatchMessage(&msg); 
						}
						wnd->Stop();
						//::PostQuitMessage(0);
					}
				}
			};
			return TRUE;
		}
		return CWinThread::PreTranslateMessage(pMsg);
	}
};

IMPLEMENT_DYNCREATE(CSmartWBrowserThread, CWinThread)
CSmartWBrowser::CSmartWBrowser()
{
	pBr=(CSmartWBrowserThread*)::AfxBeginThread(RUNTIME_CLASS(CSmartWBrowserThread));
}

CSmartWBrowser::~CSmartWBrowser()
{
	if(pBr){
		pBr->PostThreadMessage(WBT_EXIT,0,0);// Удалится самостоятельно - m_bAutoDelete==TRUE
	}
}

BOOL CSmartWBrowser::LoadPage(const char* szURL, DWORD dwTimeout, BOOL* bWasTimedOut)
{
	if(!objSettings.bSupportHTMLWP){
		return FALSE;
	}
	if(dwTimeout<1000){
		dwTimeout=5000;
	}
	char* pStr=strdup(szURL);
	if(pBr && !pBr->PostThreadMessage(WBT_LOAD,WPARAM(pStr),0)){
		free(pStr);
		return FALSE;
	}
	DWORD dwStart=GetTickCount();
	while(pBr && pBr->bBrowserEnabled!=TRUE && (GetTickCount()-dwStart)<dwTimeout){// Ждем пока не инициализируется
		if(pBr->bBrowserEnabled==-1){
			Sleep(100);
		}else{
			return FALSE;
		}
	}
	BOOL bRes=TRUE;
	if(pBr && pBr->bBrowserEnabled==TRUE && pBr->wnd!=NULL){
		DWORD dwRes=WaitForSingleObject(pBr->wnd->hHtmlLoadedEvent,dwTimeout);
		::ResetEvent(pBr->wnd->hHtmlLoadedEvent);
		if(bWasTimedOut){
			if(dwRes==WAIT_TIMEOUT){
				*bWasTimedOut=TRUE;
			}else{
				*bWasTimedOut=FALSE;
			}
		}
		bRes=TRUE;
	}else{
		bRes=FALSE;
	}
	if(pBr){
		pBr->PostThreadMessage(WBT_STOP,0,0);
	}
	return bRes;
}

BOOL CSmartWBrowser::CopyPageImage(HBITMAP& hTo, CSize size)
{
	if(!pBr){
		return FALSE;
	}
	if(pBr->wnd->hExplorerContent==NULL){
		pBr->wnd->DocumentComplete(NULL,NULL);
	}
	return CopyBitmapToBitmap(pBr->wnd->hExplorerContent, hTo, size);
}
