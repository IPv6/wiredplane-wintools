// Проблема - под ХР убивается посленяя кнопа
// HookCode.cpp: implementation of the CHookCode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HookCode.h"
#include "..\..\SmartWires\SystemUtils\Macros.h"

// Nonshared variables
void ForceDebugBreak()
{
   __try { _asm { int 3 }; }
   __except(UnhandledExceptionFilter(GetExceptionInformation())) { }
}
#define MAX_BUFSIZE	2048
#pragma data_seg("Shared")
long	g_LockCBT=0;
long	g_LockCBT2=0;
HWND	hWndButton=0;
long	g_PaintLock=0;
DWORD	g_TimeForDrag=0;
BOOL	g_SkipOneUp=0;
HWND	g_hShellTray=0;
long	bShowKillMC=0;
long	bShowKill=0;
long	bOnCtrl=0;
long	bSuggestKill=0;
long	bDragos=1;
long	bKillClose=0;
long	bMakeSq=0;
HWND	g_hLastActive=0;
BOOL	g_CursorSet=0;
BOOL	g_bToolBar=0;
HHOOK	g_hhook = NULL;
HHOOK	g_hhookM = NULL;
DWORD	g_NextMsg=0;
DWORD	g_KillMsg=0;
HWND	hWndControl=0;
long	g_HookLock=0;
POINT	g_pt={0};
POINT	g_ptClient={0};
int		g_State=0;
int		g_ButtonBegin=-1;
int		g_ButtonActive=-1;
int		g_ButtonTmp=-1;
int		g_ButtonEnd=-1;
RECT	g_rButtonBegin={0};
RECT	g_rButtonEnd={0};
RECT	g_rButtonTmp={0};
POINT	g_LastCursorPos={0};
DWORD	dwHookerThread=0;
HWND	g_FindWnd=0;
char	g_szSearchTitle[MAX_BUFSIZE]={0};
char	g_szToolbarTitle[MAX_BUFSIZE]={0};
long	g_NoRec=0;
long	g_ActionInProgres=0;
long	iSqsize=4;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")

typedef struct _TBBUTTONXXX {
    int iBitmap;
    int idCommand;
    BYTE fsState;
    BYTE fsStyle;
#ifdef _WIN32
    BYTE bReserved[2];
#endif
    DWORD dwData;
    int iString;
	char szDummy[1024];
} TBBUTTONXXX;

typedef struct _TCITEMAXXX {
    UINT mask;
#if (_WIN32_IE >= 0x0300)
    DWORD dwState;
    DWORD dwStateMask;
#else
    UINT lpReserved1;
    UINT lpReserved2;
#endif
    LPSTR pszText;
    int cchTextMax;
    int iImage;

    LPARAM lParam;
	char szDummy[1024];
} TCITEMAXXX;

void ShowCBT(RECT* btRect)
{
	if(btRect){
		BOOL bSkip=g_LockCBT;
		bSkip=(bSkip || (GlobalFindAtom("WK_RECORDING")!=0));
		if(bSkip){
			btRect=0;
		}else{
			g_LockCBT2=1;
			BOOL bSkipUpdate=0;
			if(IsWindowVisible(hWndButton)){
				RECT rt;
				GetWindowRect(hWndButton,&rt);
				if(rt.bottom==btRect->bottom
					&& rt.top==btRect->top
					&& rt.left==btRect->left
					&& rt.right==btRect->right){
					bSkipUpdate=1;
				}
			}
			if(!bSkipUpdate){
				::SetWindowPos(hWndButton,HWND_TOPMOST,btRect->left,btRect->top,btRect->right-btRect->left,btRect->bottom-btRect->top,SWP_ASYNCWINDOWPOS|SWP_SHOWWINDOW|SWP_NOACTIVATE);
				::UpdateWindow(hWndButton);
			}
		}
	}
	if(!btRect){
		g_LockCBT2=0;
		if(IsWindowVisible(hWndButton)){
			::ShowWindowAsync(hWndButton,SW_HIDE);
		}
	}
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain_(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		/*g_hinstDll = hinstDll;
		AfxSetResourceHandle(g_hinstDll);
		afxCurrentInstanceHandle=g_hinstDll;*/
		break;
	case DLL_PROCESS_DETACH:
		// The calling process is detaching the DLL from its address space.
		break;
	case DLL_THREAD_ATTACH:
		// A new thread is being created in the current process.
		break;
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly.
		break;
	}
	return(TRUE);
}

#include <commctrl.h>
void SwitchThroughMouseClick(POINT& pt);
BOOL IsWindowsSystemWnd(HWND hWin)
{
	char szClass[128]="";
	BOOL bClassRes=::GetClassName(hWin,szClass,sizeof(szClass));
	if(bClassRes>0){
		if(stricmp(szClass,"SHELLDLL_DefView")==0 
			|| stricmp(szClass,"Progman")==0 
			|| stricmp(szClass,"Shell_TrayWnd")==0 
			|| stricmp(szClass,"WorkerW")==0 || stricmp(szClass,"WorkerA")==0
			|| stricmp(szClass,"FakeDesktopWClass")==0 || stricmp(szClass,"FakeDesktopAClass")==0
			|| stricmp(szClass,"tooltips_class32")==0
			|| stricmp(szClass,"GLBSWizard")==0
			|| stricmp(szClass,"SysFader")==0){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsWindowStylesOk(HWND hWind)
{
	return 1;
	DWORD dwStyle=::GetWindowLong(hWind,GWL_STYLE);
	DWORD dwStyleEx=::GetWindowLong(hWind,GWL_EXSTYLE);
	if((dwStyleEx & WS_EX_TOOLWINDOW)==0){
		if((dwStyle & WS_THICKFRAME)!=0 || (dwStyle & WS_BORDER)!=0 || (dwStyle & WS_DLGFRAME)!=0 || (dwStyleEx & WS_EX_WINDOWEDGE)!=0 || (dwStyleEx & WS_EX_STATICEDGE)!=0){
			if(!IsZoomed(hWind) && !IsIconic(hWind)){
				WINDOWPLACEMENT pl;
				pl.length=sizeof(WINDOWPLACEMENT);
				::GetWindowPlacement(hWind,&pl);
				if(pl.showCmd!=SW_SHOWMAXIMIZED && pl.showCmd!=SW_MAXIMIZE && pl.showCmd!=SW_MAX){
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void GetWindowTopParent(HWND& hWnd);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char g_szSearchTitle2[MAX_BUFSIZE]={0};
	::GetWindowText(hwnd,g_szSearchTitle2,sizeof(g_szSearchTitle2)-1);
	if(!IsWindowVisible(hwnd) || IsWindowsSystemWnd(hwnd) || !IsWindowStylesOk(hwnd)){
		return 1;
	}
	int iLen=0;
	char* szP=g_szSearchTitle;
	char* szP2=g_szSearchTitle2;
	while(*szP && *szP2){
		if(*szP!=*szP2){
			return 1;
		}
		szP++;
		szP2++;
		iLen++;
	}
	if(iLen==0){
		return 1;
	}
	if((*szP != *szP2) && (*szP==0 && *szP2!=0)){
		return 1;
	}
	if(g_FindWnd!=0){
		g_FindWnd=0;
		return 0;
	}
	if(g_FindWnd!=0 && g_FindWnd!=hwnd){
		HWND hParentL=g_FindWnd;
		GetWindowTopParent(hParentL);
		HWND hParentR=hwnd;
		GetWindowTopParent(hParentR);
		if(hParentR!=hParentL){
			g_FindWnd=0;
			return 0;
		}
		g_FindWnd=hParentL;
	}else{
		g_FindWnd=hwnd;
	}
	return 1;
}

HWND MyFindWindow(const char* szClass, const char* szTitle,  BOOL bUseRemembered)
{
	if(!szTitle || szTitle[0]==0){
		return 0;
	}
	if(g_NoRec!=0){
		return 0;
	}
	SimpleTracker lc(g_NoRec);
	g_FindWnd=0;
	memset(g_szSearchTitle,0,sizeof(g_szSearchTitle));
	memcpy(g_szSearchTitle,szTitle,sizeof(g_szSearchTitle));
	::EnumWindows(EnumWindowsProc,0);
	g_szSearchTitle[0]=0;
	return g_FindWnd;
};

class C_ABC
{
public:
	int iAction;
	HWND hFw;
};

DWORD WINAPI ActivateByClickInThread(LPVOID p)
{
	C_ABC* pD=(C_ABC*)p;
	// Активируем программу...
	POINT pt;
	POINT ptLT,ptRB;
	ptLT.x=g_rButtonBegin.left;
	ptLT.y=g_rButtonBegin.top;
	ptRB.x=g_rButtonBegin.right;
	ptRB.y=g_rButtonBegin.bottom;
	ClientToScreen(hWndControl,&ptLT);
	ClientToScreen(hWndControl,&ptRB);
	pt.x=(ptLT.x+ptRB.x)/2;
	pt.y=(ptLT.y+ptRB.y)/2;
	SwitchThroughMouseClick(pt);
	PostThreadMessage(dwHookerThread,WM_USER,pD->iAction,(LPARAM)GetForegroundWindow());
	delete pD;
	return 0;
}

void DropButton(int iB,int iE)
{
	if(bOnCtrl && GetAsyncKeyState(VK_CONTROL)>=0){
		return;
	}
	if(g_ActionInProgres!=0){
		return;
	}
	SimpleTracker lc(g_ActionInProgres);
	ShowCBT(0);
	char szTitle[MAX_BUFSIZE]={0};
	BOOL bMovingActive=0;
	if(g_bToolBar){
		TBBUTTONXXX tb;
		memset(&tb,0,sizeof(tb));
		if(SendMessage(hWndControl,TB_GETBUTTON,iB,(LPARAM)&tb) && ((tb.fsState & TBSTATE_CHECKED)!=0)){
			bMovingActive=TRUE;
		}
	}else{
		bMovingActive=(TabCtrl_GetCurSel(hWndControl)==iB);
	}
	HWND hWnd1 = ::FindWindow("Shell_TrayWnd",NULL);
	if(iE==-1){
		// Смотрим время...
		if(g_TimeForDrag==0 || GetTickCount()-g_TimeForDrag<100){
			return;
		}
		g_TimeForDrag=0;
		// в трей?
		if(hWnd1 == 0){
			return;
		}
		HWND hWnd2 = FindWindowEx(hWnd1,0,"TrayNotifyWnd",NULL); 
		if(hWnd2 == 0){
			return;
		}
		HWND hFw=g_hLastActive?g_hLastActive:GetForegroundWindow();

		// Пытаемся вытащить по имени окно
		if(g_bToolBar){
			TBBUTTONXXX tb;
			memset(&tb,0,sizeof(tb));
			SendMessage(hWndControl,TB_GETBUTTON,iB,(LPARAM)&tb);
			SendMessage(hWndControl,TB_GETBUTTONTEXTA,tb.idCommand,(LPARAM)szTitle);
		}else{
			TCITEMAXXX tc;
			memset(&tc,0, sizeof(tc));
			tc.mask=TCIF_PARAM|TCIF_IMAGE|TCIF_TEXT|0x0010;//TCIF_PARAM|
			tc.dwStateMask=0xFFFFFFFF;
			tc.pszText=g_szToolbarTitle;
			tc.cchTextMax=sizeof(g_szToolbarTitle)-1;
			//FLOG3("Requesting Tab 2000: %08x, %i,%i",hWndControl,iB,tc.cchTextMax);
			SendMessage(hWndControl, TCM_GETITEMA, (WPARAM)(int)(iB), (LPARAM)(TC_ITEMA *)(&tc));
			strcpy(szTitle,tc.pszText);
			//FLOG3("Requesting Tab 2000 done: %08x, %i,%i",hWndControl,iB,tc.cchTextMax);
		}

		HWND hWinByTitle=MyFindWindow(NULL,szTitle,0);
		if(strlen(szTitle)>0 && hWinByTitle){
			bMovingActive=1;
			hFw=hWinByTitle;
		}
		int iAction=3;//bMovingActive?3:1;
		RECT rt;
		GetWindowRect(hWnd2,&rt);
		if(rt.left<g_LastCursorPos.x && rt.right>g_LastCursorPos.x && rt.bottom>g_LastCursorPos.y && rt.top<g_LastCursorPos.y){
			iAction=2;//bMovingActive?2:0;
		}else if(!bDragos){
			// нельзя за пределы!
			return;
		}
		if(!bMovingActive){
			C_ABC* pD=new C_ABC();
			pD->hFw=hFw;
			pD->iAction=iAction;
			FORK(ActivateByClickInThread,pD);
		}else{
			PostThreadMessage(dwHookerThread,WM_USER,iAction,(LPARAM)hFw);
		}
		//FLOG("drop done, exit");
		return;
	}
	if(bMovingActive){
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hWndControl);
	}
	// Кидаем кноку назад
	int iCount=0;
	if(iE>iB && iE-iB==1){
		int iTmp=iE;
		iE=iB;
		iB=iTmp;
	}

	if(g_bToolBar){
		RECT rtScr,rtScr2;
		GetWindowRect(hWndControl,&rtScr);
		SystemParametersInfo(SPI_GETWORKAREA,0,&rtScr2,SPIF_SENDCHANGE);
		TBBUTTONXXX tb;
		char* szText=szTitle;//(char*)GlobalLock(hg);
		memset(&tb,0,sizeof(tb));
		tb.iString=(int)szText;
		DWORD dwWH=SendMessage(hWndControl,TB_GETBUTTONSIZE,0,0);
		long lW=LOWORD(dwWH);
		long lH=HIWORD(dwWH);
		int iCountW=(rtScr.right-rtScr.left)/lW;
		int iCountH=(rtScr.bottom-rtScr.top)/lH;
		SendMessage(hWndControl,TB_GETBUTTON,iB,(LPARAM)&tb);
		SendMessage(hWndControl,TB_GETBUTTONTEXTA,tb.idCommand,(LPARAM)szText);
		tb.iString=(int)szText;
		SendMessage(hWndControl,TB_DELETEBUTTON,iB,0);
		SendMessage(hWndControl,TB_SETBUTTONWIDTH,0,MAKELONG ((rtScr.right-rtScr.left)/(iCountW+1)-5, (rtScr.bottom-rtScr.top)/(iCountH+1)-5));
		SendMessage(hWndControl,TB_INSERTBUTTONA,iE,(LPARAM)&tb);//+((iE>iB)?-2:0)
		PostMessage(hWndControl,TB_AUTOSIZE,0,0);
		SystemParametersInfo(SPI_SETWORKAREA,0,&rtScr2,SPIF_SENDCHANGE);
		//SendMessage(hWndControl,WM_SETTINGCHANGE,,0);
	}else{
		TCITEMAXXX tc;
		memset(&tc,0, sizeof(tc));
		tc.mask=TCIF_PARAM|TCIF_IMAGE|TCIF_TEXT|0x0010;//
		tc.dwStateMask=0xFFFFFFFF;
		tc.pszText=g_szToolbarTitle;
		tc.cchTextMax=sizeof(g_szToolbarTitle)-1;
		//FLOG3("Moving Tab 2000: %08x, %i,%i",hWndControl,iB,tc.cchTextMax);
		SendMessage(hWndControl, TCM_GETITEMA, (WPARAM)(int)(iB), (LPARAM)(TC_ITEMA *)(&tc));
		strcpy(szTitle,tc.pszText);
		//FLOG3("Moving Tab 2000 step2: %08x, %i,%i",hWndControl,iB,tc.cchTextMax);
		TabCtrl_DeleteItem(hWndControl,iB);
		//FLOG3("Moving Tab 2000 step3: %08x, %i,%i",hWndControl,iB,tc.cchTextMax);
		TabCtrl_InsertItem(hWndControl,iE,&tc);
		//FLOG3("Moving Tab 2000 done: %08x, %i,%i",hWndControl,iB,tc.cchTextMax);
	}
	::InvalidateRect(hWnd1,0,0);
}

int GetUnderMBt(RECT& btRect)
{
	int iCount=0;
	if(g_bToolBar){
		iCount=SendMessage(hWndControl,TB_BUTTONCOUNT,0,0);
	}else{
		iCount=TabCtrl_GetItemCount(hWndControl);
	}
	if(iCount>0){
		int i=-1;
		POINT pt;
		GetCursorPos(&g_LastCursorPos);
		pt.x=g_LastCursorPos.x;
		pt.y=g_LastCursorPos.y;
		::ScreenToClient(hWndControl,&pt);
		TBBUTTONXXX tb;
		for(int j=0;j<iCount;j++){
			if(g_bToolBar){
				memset(&tb,0,sizeof(tb));
				if(SendMessage(hWndControl,TB_GETBUTTON,j,(LPARAM)&tb)){
					if((tb.fsState & TBSTATE_HIDDEN)!=0){
						continue;
					}
				}
				SendMessage(hWndControl,TB_GETITEMRECT,j,(LPARAM)&btRect);
			}else{
				TabCtrl_GetItemRect(hWndControl,j,&btRect);
			}
			if(pt.x<btRect.right && pt.x>btRect.left){
				if(pt.y<btRect.bottom && pt.y>btRect.top){
					return j;
				}
			}
		}
	}
	return -1;
}

DWORD WINAPI SwitchApp(LPVOID pData)
{
	if(g_ActionInProgres!=0){
		return 0;
	}
	SimpleTracker lc(g_ActionInProgres);
	DWORD dwRes=0;
	int iType=(int)pData;
	int iCount=0;
	if(g_bToolBar){
		iCount=SendMessage(hWndControl,TB_BUTTONCOUNT,0,0);
	}else{
		iCount=TabCtrl_GetItemCount(hWndControl);
	}
	if(iCount>0){
		int i=-1;
		if(g_bToolBar){
			TBBUTTONXXX tb;
			for(int j=0;j<iCount;j++){
				memset(&tb,0,sizeof(tb));
				if(SendMessage(hWndControl,TB_GETBUTTON,j,(LPARAM)&tb)){
					if((tb.fsState & TBSTATE_CHECKED)!=0){
						i=j;
						break;
					}
				}
			}
			if(i==-1){
				i=1;
			}else if(iType!=2){
				i=i+(iType?-2:2)+iCount;
				i=i%(iCount)+1;
			}
		}else{
			i=TabCtrl_GetCurSel(hWndControl);
			if(i==-1){
				i=0;
			}else if(iType!=2){
				i=i+(iType?-1:1)+iCount;
				i=i%iCount;
			}
		}
		{// Вычисляем прямоугольник соотв. кнопки
			// Ищем куда будем симулировать нажатие
			RECT btRect;
			BOOL bRectOK=0;
			if(g_bToolBar){
				bRectOK=SendMessage(hWndControl,TB_GETITEMRECT,i,(LPARAM)&btRect);
			}else{
				bRectOK=TabCtrl_GetItemRect(hWndControl,i,&btRect);
			}
			if(bRectOK){
				POINT pt;
				pt.x=(btRect.left+btRect.right)/2;
				pt.y=(btRect.top+btRect.bottom)/2;
				g_ptClient.x=pt.x;
				g_ptClient.y=pt.y;
				::ClientToScreen(hWndControl,&pt);
				dwRes=MAKELONG(pt.x,pt.y);
				g_pt.x=pt.x;
				g_pt.y=pt.y;
			}
		}
	}
	return dwRes;
}

void DumpMem(void* p, int i)
{
	BYTE* b=(BYTE*)p;
	int c=0;
	while(c<=i){
		//FLOG4("%02x %02x %02x %02x",b[c],b[c+1],b[c+2],b[c+3]);
		c+=4;
	}
}

DWORD WINAPI KillWndUnderC(LPVOID p)
{
	if(g_ActionInProgres!=0){
		return 0;
	}
	int iForce=(int)p;
	HWND hWinByTitle=0;
	char szTitle[MAX_BUFSIZE]={0};
	{// сужаем область трекера
		SimpleTracker lc(g_ActionInProgres);
		g_ButtonBegin=GetUnderMBt(g_rButtonBegin);
		if(bShowKill || iForce){
			RECT btRect;
			btRect.left=g_rButtonBegin.left;
			btRect.right=g_rButtonBegin.right;
			btRect.top=g_rButtonBegin.top;
			btRect.bottom=g_rButtonBegin.bottom;
			btRect.right-=iSqsize;
			btRect.top+=iSqsize;
			int iMH=min(btRect.right-btRect.left,btRect.bottom-btRect.top);
			btRect.left=btRect.right-iMH/2;
			btRect.bottom=btRect.top+iMH/2;
			POINT pt;
			GetCursorPos(&pt);
			::ScreenToClient(hWndControl,&pt);
			if(iForce || (pt.x<btRect.right && pt.x>btRect.left && pt.y<btRect.bottom && pt.y>btRect.top)){
				if(g_bToolBar){
					TBBUTTONXXX tb;
					memset(&tb,0,sizeof(tb));
					SendMessage(hWndControl,TB_GETBUTTON,g_ButtonBegin,(LPARAM)&tb);
					SendMessage(hWndControl,TB_GETBUTTONTEXTA,tb.idCommand,(LPARAM)szTitle);
					hWinByTitle=MyFindWindow(NULL,szTitle,0);
					//DumpMem(&tb, sizeof(tb));FLOG1("Search for=%08x",hWinByTitle);
				}else{
					// Только активное окно
					if(g_ButtonBegin==(g_ButtonActive!=-1?g_ButtonActive:TabCtrl_GetCurSel(hWndControl))){
						hWinByTitle=g_hLastActive?g_hLastActive:GetForegroundWindow();
						GetWindowText(hWinByTitle,szTitle,sizeof(szTitle));
					}else{
						szTitle[0]=0;
					}
				}
			}
		}
	}
	if(hWinByTitle!=0){
		g_SkipOneUp=1;
		PostThreadMessage(dwHookerThread,WM_USER,100,(LPARAM)hWinByTitle);
	}
	return TRUE;
}

#include "../WKPlugin.h"
LRESULT CALLBACK MouseProc( int code, WPARAM wParam, LPARAM lParam )
{
	if(WKUtils::isWKUpAndRunning() && g_HookLock==0){
		SimpleTracker lc(g_HookLock);
		if (code==HC_ACTION && lParam!=0){
			MOUSEHOOKSTRUCT* msg = (MOUSEHOOKSTRUCT*)lParam;
			if(wParam==WM_MOUSEMOVE){
				if(bOnCtrl && GetAsyncKeyState(VK_CONTROL)>=0){
					g_ButtonBegin=g_ButtonEnd=-1;
				}else{
					HWND hFw=GetForegroundWindow();
					if(g_hShellTray==0 || !IsWindow(g_hShellTray)){
						g_hShellTray=::FindWindow("Shell_TrayWnd",NULL);
					}
					if(hFw!=0 && hFw!=hWndControl && hFw!=g_hShellTray){
						g_hLastActive=hFw;
					}
					if(g_ButtonBegin!=-1 && g_ButtonTmp!=g_ButtonBegin){
						if(g_CursorSet!=1){
							g_CursorSet=1;
							SetCursor(::LoadCursor(0,IDC_CROSS));//IDC_SIZEWE
						}
					}
					g_ButtonTmp=GetUnderMBt(g_rButtonTmp);
					if(g_ButtonTmp!=-1 && bShowKill && g_PaintLock==0){
						InterlockedIncrement(&g_PaintLock);
						POINT pt;
						GetCursorPos(&pt);
						::ScreenToClient(hWndControl,&pt);
						RECT btClSize;
						GetClientRect(hWndControl,&btClSize);
						RECT btWnPos;
						GetWindowRect(hWndControl,&btWnPos);
						if(pt.x<btClSize.right && pt.x>btClSize.left){
							if(pt.y<btClSize.bottom && pt.y>btClSize.top){
								int iCount=0;
								TBBUTTONXXX tb;
								int iCurSelectionIfAny=0;
								if(g_bToolBar){
									iCount=SendMessage(hWndControl,TB_BUTTONCOUNT,0,0);
								}else{
									iCount=TabCtrl_GetItemCount(hWndControl);
									iCurSelectionIfAny=TabCtrl_GetCurSel(hWndControl);
								}
								int j=g_ButtonTmp;
								if(j<iCount){
									RECT btRect;
									BOOL bOk=FALSE;
									if(g_bToolBar){
										memset(&tb,0,sizeof(tb));
										if(SendMessage(hWndControl,TB_GETBUTTON,j,(LPARAM)&tb)){
											if((tb.fsState & TBSTATE_HIDDEN)==0){
												iCurSelectionIfAny=j;
												// Возможно тут падает
												SendMessage(hWndControl,TB_GETITEMRECT,j,(LPARAM)&btRect);
												bOk=TRUE;
											}
										}
									}else{
										if(j==iCurSelectionIfAny){
											TabCtrl_GetItemRect(hWndControl,j,&btRect);
											bOk=TRUE;
										}
									}
									if(bOk){// && !IsWindowVisible(hWndButton)
										g_ButtonActive=iCurSelectionIfAny;//TabCtrl_GetCurSel(hWndControl);
										// Чек дупс
										btRect.right-=iSqsize;
										btRect.top+=iSqsize;
										int iMH=min(btRect.right-btRect.left,btRect.bottom-btRect.top);
										btRect.left=btRect.right-iMH/2;
										btRect.bottom=btRect.top+iMH/2;
										// Переводим в скринкоординаты
										btRect.left+=btWnPos.left;
										btRect.right+=btWnPos.left;
										btRect.top+=btWnPos.top;
										btRect.bottom+=btWnPos.top;
										ShowCBT(&btRect);
									}else{
										ShowCBT(0);
									}
								}
							}
						}
						InterlockedDecrement(&g_PaintLock);
					}
				}
			}
			if(wParam==WM_MBUTTONDOWN && bShowKillMC){
				g_ButtonBegin=GetUnderMBt(g_rButtonBegin);
//				FLOG3("Middle-click detected!!! beginKnop=%i",g_ButtonBegin,0,0);
				FORK(KillWndUnderC,1);
			}
			if(wParam==WM_LBUTTONDOWN){
				//FLOG3("Left-click detected!!!",0,0,0);
				if(bOnCtrl && GetAsyncKeyState(VK_CONTROL)>=0){
					g_TimeForDrag=0;
					g_ButtonBegin=-1;
				}else{
					g_TimeForDrag=GetTickCount();
					g_ButtonBegin=GetUnderMBt(g_rButtonBegin);
				}
			}
			if(wParam==WM_LBUTTONUP){
				if(g_ButtonBegin==-1 || (bOnCtrl && GetAsyncKeyState(VK_CONTROL)>=0)){
					g_ButtonEnd=-1;
				}else{
					if(g_SkipOneUp){
						g_SkipOneUp--;
					}else{
						g_ButtonEnd=GetUnderMBt(g_rButtonEnd);
						if(g_ButtonBegin!=g_ButtonEnd){
							DropButton(g_ButtonBegin,g_ButtonEnd);
						}
					}
					g_ButtonEnd=g_ButtonBegin=-1;
				}
				if(g_CursorSet==1){
					SetCursor(::LoadCursor(0,IDC_ARROW));
					g_CursorSet=0;
				}
			}
		}
	}
	return (CallNextHookEx(g_hhookM, code, wParam, lParam));
}

LRESULT CALLBACK CallWndRetProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (WKUtils::isWKUpAndRunning() && g_hhook && code>=0 && lParam!=0 && g_HookLock==0) {
		SimpleTracker lc(g_HookLock);
		CWPRETSTRUCT* msg=(CWPRETSTRUCT*)(lParam);
		if(msg->message==g_NextMsg){
			SwitchApp(LPVOID(msg->lParam));
		}
		if(msg->message==g_KillMsg){
			FORK(KillWndUnderC,0);
		}
		if(msg->message==TB_ADDBUTTONS || msg->message==TB_INSERTBUTTONA || msg->message==TB_ADDBUTTONSA ||
			msg->message==TB_DELETEBUTTON || msg->message==TB_INSERTBUTTONW ||
			msg->message==TCM_INSERTITEMA || msg->message==TCM_INSERTITEMW || 
			msg->message==TCM_DELETEITEM || msg->message==TCM_DELETEALLITEMS
			|| msg->message==WM_WINDOWPOSCHANGED //|| msg->message==WM_SIZING || msg->message==WM_SIZE
		){
			ShowCBT(0);
		}
	}
	return (CallNextHookEx(g_hhook, code, wParam, lParam));
}
