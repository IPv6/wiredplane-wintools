// DLGClip.cpp : implementation file
//

#include "stdafx.h"
#include "WireKeys.h"
#include "DLGClip.h"
#include "_common.h"
#include "_externs.h"

// CDLGClip dialog
HGLOBAL& GetGlobalClipText(BOOL);
extern DWORD dwClipAttachTime;
extern long lPasteClipInSyncON;
IMPLEMENT_DYNAMIC(CDLGClip, CDialog)
DWORD WINAPI BackIconToNormal(LPVOID pData);
DWORD WINAPI DoWM_DRAWCLIPBOARDProcessing_InThread(LPVOID p);
DWORD WINAPI DoWM_DRAWCLIPBOARDProcessing_InThread_Protected(LPVOID p);
CDLGClip::CDLGClip(CWnd* pParent /*=NULL*/)
: CDialog(/*CDLGClip::IDD, pParent*/)
{
	hNextClipboardChainWnd=NULL;
	lCopyCount=0;
	hClipChanged=::CreateEvent(NULL,TRUE,FALSE,NULL);
	hClipChangedOut=::CreateEvent(NULL,TRUE,FALSE,NULL);
}

CDLGClip::~CDLGClip()
{
	CloseHandle(hClipChanged);
	CloseHandle(hClipChangedOut);
}

BEGIN_MESSAGE_MAP(CDLGClip, CDialog)
	ON_WM_NCDESTROY()
	ON_WM_NCCREATE()
END_MESSAGE_MAP()

// CDLGClip message handlers
LRESULT CDLGClip::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!objSettings.lEverythingIsPaused && pMainDialogWindow && pMainDialogWindow->clipWnd==this){_XLOG_
		if(message==WM_CHANGECBCHAIN){_XLOG_
			__FLOWLOG
			CSmartLock lock(&csClipAttach);
			if(hNextClipboardChainWnd!=HWND(wParam)){_XLOG_
				::SendMessage(hNextClipboardChainWnd,WM_CHANGECBCHAIN,wParam,lParam);
			}else{_XLOG_
				hNextClipboardChainWnd=HWND(lParam);
			}
			return 0;
		}
		if(message==WM_RENDERFORMAT){//|| message==WM_RENDERALLFORMATS
			__FLOWLOG
			CSmartLock lock(&csClipAttach);
			// Не должно зависеть от objSettings.lDisableClipboard
			// так как мы это тоже используем для временных блокировок
			// которые сюда не относятся
			if(lPasteClipInSyncON>0 && !objSettings.lEverythingIsPaused){_XLOG_
				//LOGSTAMP1("receiving WM_RENDERFORMAT");
				BOOL bRes=FALSE;
				if(message==WM_RENDERALLFORMATS){_XLOG_
					OpenClipboard();
				}
				if(wParam==CF_UNICODETEXT || message==WM_RENDERALLFORMATS){_XLOG_
					if(GetGlobalClipText(1)){_XLOG_
						__FLOWLOG
						::SetClipboardData(CF_UNICODETEXT,GetGlobalClipText(1));
						GetGlobalClipText(1)=NULL;
						bRes=TRUE;
					}
				}
				if(wParam==CF_TEXT || message==WM_RENDERALLFORMATS){_XLOG_
					if(GetGlobalClipText(0)){_XLOG_
						__FLOWLOG
						::SetClipboardData(CF_TEXT,GetGlobalClipText(0));
						GetGlobalClipText(0)=NULL;
						bRes=TRUE;
					}
				}
				if(message==WM_RENDERALLFORMATS){_XLOG_
					CloseClipboard();
				}
				if(bRes){_XLOG_
					pMainDialogWindow->SetWindowIcon(1);
					SetEvent(hClipChangedOut);
					FORK(BackIconToNormal,500);
					return 0;
				}
				//LOGSTAMP1("WM_RENDERFORMAT finished");
			}
		}
		if(message==WM_DRAWCLIPBOARD){_XLOG_
			__FLOWLOG
			CSmartLock lock(&csClipAttach);
			if(hNextClipboardChainWnd != NULL){_XLOG_
				::SendMessage(hNextClipboardChainWnd,WM_DRAWCLIPBOARD,0,0);
			}
			if(!objSettings.lDisableClipboard && !objSettings.lEverythingIsPaused){_XLOG_
				FORK(DoWM_DRAWCLIPBOARDProcessing_InThread_Protected,0);
			}
			return 0;
		}
	}
	return CDialog::WindowProc(message,wParam,lParam);
}
void CDLGClip::OnNcDestroy()
{
	CDialog::OnNcDestroy();
	// TODO: Add your message handler code here
	::ChangeClipboardChain(GetSafeHwnd(),hNextClipboardChainWnd);
}

BOOL CDLGClip::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CDialog::OnNcCreate(lpCreateStruct))
		return FALSE;
	// TODO:  Add your specialized creation code here
	hNextClipboardChainWnd=::SetClipboardViewer(GetSafeHwnd());
	return TRUE;
}
