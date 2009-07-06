// HookCode.cpp: implementation of the CHookCode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HookCode.h"
#include "floatingBT.h"
// Instruct the compiler to put the g_hhook data variable in 
// its own data section called Shared. We then instruct the 
// linker that we want to share the data in this section 
// with all instances of this application.
#pragma data_seg("Shared")
long			bNoRecurseAddSelectionToRecents=0;
long			bNoRecurseAddSelectionToRecentsStep=0;
int				bCasheWKStatus=0;
HWND			hLastFocusWnd=0;
HWND			hLastActiveWnd=0;
DWORD			g_LKeyActionLast=0;
DWORD			g_LKeyActionLastTime=0;
DWORD			g_LKeyActionHookerDesiredActionDsc=0;
POINT			g_LKeyActionLastPos={0};
POINT			g_prevPT={0};
HWND			g_WKWnd=0;
DWORD			dwLastNewWndTick=0;
DWORD			dwLastWMCHARTick=0;
DWORD			dwLastWMMOUSETick=0;
DWORD			dwNewWinProcID=0;
DWORD			dwThisWinProcID=1;
HWND			hThisWinProcID=0;
DWORD			dwHookerThread=0;
long			lHookActive = 0;
HHOOK			g_hhook1 = NULL;
HHOOK			g_hhook2 = NULL;
HHOOK			g_hhook3 = NULL;
HHOOK			g_hhook4 = NULL;
RECT			rtScreenSize = {0,0,0,0};
CWKHekperInfo	hookData = {0};
UINT			msgGetScreenshot=0;
DWORD			dwSnapWindowTimeData=0;
HWND			hLastNCHittestWnd=0;
DWORD			dwLastNCHittestRes=0;
HWND			hCurNCHittestWnd=0;
DWORD			dwCurNCHittestRes=0;
char			szItem_ToTray[64]="Hide to tray";
char			szItem_Compl[64]="Hide completely";
char			szItem_AddMenu[64]="Additional menu\t...";
char			szItem_FavMenu[64]="Recent folders\t...";
char			szItem_OnTopState[64]="Change 'on top'";
char			szItem_ToTitle[64]="Rollup to title";
char			szItem_Info[64]="Application info";
char			szItem_QRun[64]="Add to quick-run";
char			szItem_SCut[64]="Create shortcut";
char			szItem_Kill[64]="Kill application";
HWND			g_MainWnd=0;
HWND			g_hLastWnd=0;
char			g_szLastWndT[256]={0};
char			g_szApplications[MAX_APPS][(10+MAX_APP_NAME)]={0};
int				g_iGlobalClassCount=0;
//UINT			g_ObjectSelMSG=0;//RegisterWindowMessage("WM_OBJECTSEL");
HWND			g_hLastObjectSel=0;
HWND			g_hLastObjectSelRoot=0;
char			g_szSelectedFile[MAX_PATH]={0};
BOOL			bShellSR=0;
BOOL			bShellBT=0;
long			g_IzWndAttachInProgress=0;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")
// Nonshared variables
void WINAPI SetLastFocusWnd(HWND);
HINSTANCE		g_hinstDll = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		g_hinstDll = hinstDll;
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

DWORD GetLastHitTest()
{
	DWORD dwRes=dwCurNCHittestRes;
	dwCurNCHittestRes=0;
	return dwRes;
}

BOOL IsWindowsSystemWnd(HWND hWin,BOOL bIncludingShell)
{
	char szClass[128]="";
	BOOL bClassRes=::GetClassName(hWin,szClass,sizeof(szClass));
	if(bClassRes>0){
		if((bIncludingShell && stricmp(szClass,"SHELLDLL_DefView")==0)
			|| (bIncludingShell && stricmp(szClass,"SysTabControl32")==0)
			|| (bIncludingShell && stricmp(szClass,"tooltips_class32")==0)
			|| stricmp(szClass,"WK_MAIN")==0 
			|| stricmp(szClass,"Progman")==0 
			|| stricmp(szClass,"TrayNotifyWnd")==0 
			|| stricmp(szClass,"Shell_TrayWnd")==0 
			|| stricmp(szClass,"WorkerW")==0 || stricmp(szClass,"WorkerA")==0
			|| stricmp(szClass,"FakeDesktopWClass")==0 || stricmp(szClass,"FakeDesktopAClass")==0
			|| stricmp(szClass,"GLBSWizard")==0
			|| stricmp(szClass,"DeskMover")==0
			|| stricmp(szClass,"Internet Explorer_Server")==0
			|| stricmp(szClass,"SysFader")==0){
			return TRUE;
		}
	}
	HWND hParent=GetParent(hWin);
	return hParent?IsWindowsSystemWnd(hParent,bIncludingShell):FALSE;
}

BOOL IsWindowStylesOk(HWND hWind)
{
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

void GetWndIcon(HWND& hSlayerWnd,HICON& hIcon)
{
	HICON hTmpIcon=NULL,hTmpIconBig=0;
	if(hTmpIcon==NULL){
		hTmpIcon=(HICON)::SendMessage(hSlayerWnd,WM_GETICON,ICON_SMALL,0);
	}
	if(hTmpIcon==NULL){
		hTmpIcon=(HICON)GetClassLong(hSlayerWnd, GCL_HICONSM);
	}
	if(hTmpIconBig==NULL){
		hTmpIconBig=(HICON)::SendMessage(hSlayerWnd,WM_GETICON,ICON_BIG,0);
	}
	if (hTmpIconBig==NULL){
		hTmpIconBig=(HICON)GetClassLong(hSlayerWnd, GCL_HICON);
	}
	if(hTmpIconBig==NULL){
		hTmpIconBig=hTmpIcon;
	}else if(hTmpIcon==NULL){
		hTmpIcon=hTmpIconBig;
	}
	if(hTmpIcon!=NULL){
		hIcon=CopyIcon(hTmpIcon);
	}
}


#define SYSMENU_WKFIRST	0xFF00FF00
#define SYSMENU_WKMENU	SYSMENU_WKFIRST
#define SYSMENU_HTRAY	SYSMENU_WKFIRST+0x0010
#define SYSMENU_HCOMP	SYSMENU_WKFIRST+0x0020
#define SYSMENU_HTITLE	SYSMENU_WKFIRST+0x0030
#define SYSMENU_HINFO	SYSMENU_WKFIRST+0x0040
#define SYSMENU_HQRUN	SYSMENU_WKFIRST+0x0050
#define SYSMENU_HSHOR	SYSMENU_WKFIRST+0x0060
#define SYSMENU_HKILL	SYSMENU_WKFIRST+0x0070
#define SYSMENU_ONTOP	SYSMENU_WKFIRST+0x0080
#define SYSMENU_FAVL	SYSMENU_WKFIRST+0x0090

void SkipBlanks(const char*& szPos);
bool PatternMatch(const char* s, const char* mask);
bool PatternMatchList0(const char* s, const char* mask)
{
	if(mask==NULL || *mask==NULL){
		return false;
	}
	if(s==NULL || *s==NULL){
		return false;
	}
	const char* szPos=mask;
	SkipBlanks(szPos);
	if(szPos==NULL || *szPos==NULL){
		return false;
	}
	while(szPos!=NULL && *szPos!=0){
		SkipBlanks(szPos);
		const char* szPos2=strpbrk(szPos,";");
		if(szPos2==NULL){
			szPos2=szPos+(strlen(szPos));
		}
		if(szPos2<=szPos){
			break;
		}
		char szMask[256];
		memset(szMask,0,sizeof(szMask));
		memcpy(szMask,szPos,size_t(szPos2-szPos));
		if(szMask[0]!=0){
			const char* szPattern=szMask;
			SkipBlanks(szPattern);
			if(PatternMatch(s,szPattern)){
				return true;
			}
		}
		szPos=szPos2;
		if(*szPos==';'){
			szPos++;
		}
	}
	return false;
}

void CheckWKPause()
{
	if(hookData.szAutoDisalbe[0]!=0){
		char szTitle[256]={0};
		HWND hWin=GetForegroundWindow();
		GetWindowText(hWin,szTitle,sizeof(szTitle));
		if(PatternMatchList0(szTitle,hookData.szAutoDisalbe)){
			if(bCasheWKStatus==0){
				PostThreadMessage(dwHookerThread,WM_USER+30,WPARAM(hWin),1);
			}
			bCasheWKStatus=1;
		}else{
			if(bCasheWKStatus==1)
			{
				PostThreadMessage(dwHookerThread,WM_USER+30,WPARAM(hWin),0);
			}
			bCasheWKStatus=0;
		}
	}
}

void CheckWindwOnTop(HWND hwndDlg)
{
	if(hookData.bOntopicon){
		BOOL bTopMost=(::GetWindowLong(hwndDlg,GWL_EXSTYLE) & WS_EX_TOPMOST)!=0;
		if(bTopMost){
			if(::GetProp(hwndDlg,"WKP_ORIGINALICON")==0){
				HICON hOriginal;
				GetWndIcon(hwndDlg,hOriginal);
				::SetProp(hwndDlg,"WKP_ORIGINALICON",(HANDLE)hOriginal);
				HICON hIco=LoadIcon(0,IDI_EXCLAMATION);
				::SendMessage(hwndDlg,WM_SETICON,ICON_SMALL,(LPARAM)hIco);
			}
		}else{
			if(::GetProp(hwndDlg,"WKP_ORIGINALICON")!=0){
				HICON hIco=(HICON)::GetProp(hwndDlg,"WKP_ORIGINALICON");
				::RemoveProp(hwndDlg,"WKP_ORIGINALICON");
				::SendMessage(hwndDlg,WM_SETICON,ICON_SMALL,(LPARAM)hIco);
			}
		}
	}
}

BOOL PatchSystemMenu(HWND hwndDlg)
{
	if(hookData.bInjectItems){
		// Патчим меню...
		HMENU hMenu=::GetSystemMenu(hwndDlg,FALSE);
		if(!::IsMenu(hMenu)){
			return FALSE;
		}
		BOOL bTopMost=(::GetWindowLong(hwndDlg,GWL_EXSTYLE) & WS_EX_TOPMOST)!=0;
		long bPatch=(DWORD(GetMenuItemID(hMenu,0))!=SYSMENU_WKMENU);
		if(bPatch){
			::InsertMenu(hMenu,0,MF_BYPOSITION|MF_SEPARATOR,0,"");
			if(hookData.bInjectIt1){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_HTRAY,szItem_ToTray);
			}
			if(hookData.bInjectIt3){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_HTITLE,szItem_ToTitle);
			}
			if(hookData.bInjectIt2){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_HCOMP,szItem_Compl);
			}
			if(hookData.bInjectIt6){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_HSHOR,szItem_SCut);
			}
			if(hookData.bInjectIt5){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_HQRUN,szItem_QRun);
			}
			if(hookData.bInjectIt4){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_HINFO,szItem_Info);
			}
			if(hookData.bInjectIt7){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_HKILL,szItem_Kill);
			}
			{// Это есть всегда...
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_ONTOP,szItem_OnTopState);
			}
			if((DWORD)::GetProp(hwndDlg,"WK_SHELL_SR_P")==1){
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_FAVL,szItem_FavMenu);
			}
			{// Это всегда должно идти последним - по ней определяется пропатчено ли меню...
				::InsertMenu(hMenu,0,MF_BYPOSITION|MF_STRING,SYSMENU_WKMENU,szItem_AddMenu);
			}
		}else{// Включаем все!
			::ModifyMenu(hMenu,SYSMENU_WKMENU,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_WKMENU,szItem_AddMenu);
			if(!::ModifyMenu(hMenu,SYSMENU_FAVL,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_FAVL,szItem_FavMenu)){
				// Если ее небыло - добавляем
				if((DWORD)::GetProp(hwndDlg,"WK_SHELL_SR_P")==1){
					::InsertMenu(hMenu,1,MF_BYPOSITION|MF_STRING,SYSMENU_FAVL,szItem_FavMenu);
				}
			}
			::ModifyMenu(hMenu,SYSMENU_HCOMP,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_HCOMP,szItem_Compl);
			::ModifyMenu(hMenu,SYSMENU_HTRAY,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_HTRAY,szItem_ToTray);
			::ModifyMenu(hMenu,SYSMENU_HINFO,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_HINFO,szItem_Info);
			::ModifyMenu(hMenu,SYSMENU_HTITLE,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_HTITLE,szItem_ToTitle);
			::ModifyMenu(hMenu,SYSMENU_HQRUN,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_HQRUN,szItem_QRun);
			::ModifyMenu(hMenu,SYSMENU_HSHOR,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_HSHOR,szItem_SCut);
			::ModifyMenu(hMenu,SYSMENU_HKILL,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_HKILL,szItem_Kill);
			::ModifyMenu(hMenu,SYSMENU_ONTOP,MF_BYCOMMAND|MF_STRING|MF_ENABLED,SYSMENU_ONTOP,szItem_OnTopState);
		}
		if(bTopMost){
			::CheckMenuItem(hMenu,SYSMENU_ONTOP,MF_BYCOMMAND | MF_CHECKED);
		}
	}
	return TRUE;
}
/*
BOOL CALLBACK MakeHoleIn(HWND hw, LPARAM l)
{
	RECT rect;
	GetWindowRect(hw,&rect);
	if(rect.right>l){
		rect.right=l;
		//SetWindowPos(hw,0,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,SWP_SHOWWINDOW|SWP_NOZORDER);
		//::MoveWindow(hw,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,TRUE);
		ShowWindow(hw,SW_HIDE);
	}
	return TRUE;
}
*/
void AddBT(HWND hFore, HWND fList, int iIndex)
{
	RECT rect;
	::GetWindowRect(fList,&rect);
	RECT rectFList;
	memcpy(&rectFList,&rect,sizeof(rectFList));
	
	POINT pt;
	pt.x=rect.left;
	pt.y=rect.top;
	::ScreenToClient(hFore, &pt);
	
	POINT pt2;
	pt2.x=rect.right;
	pt2.y=rect.bottom;
	::ScreenToClient(hFore, &pt2);
	
	int iW=GetSystemMetrics(SM_CYMENU)/2+1;
	//pt.x+3;// Именно от левого края
	rect.left=pt.x-iW/2;
	//pt.y+3+(iW+2)*iIndex;
	rect.top=pt.y+3+(iW+2)*iIndex;
	rect.right=rect.left+iW+1;//GetSystemMetrics(SM_CXSCREEN);
	rect.bottom=rect.top+iW;

	char sz[256]={0};
	int iClassNum=g_iGlobalClassCount++;
	sprintf(sz,"WPFLO_%i_%i",GetTickCount(),iClassNum);
	ATOM aCl=InitFloClass(sz);
	if(aCl){
		POINT ptTest;
		ptTest.x=pt.x-iW/2;//pt2.x+2;
		ptTest.y=pt.y+iW/2;//pt.y+3;
		HWND hPr=ChildWindowFromPoint(hFore,ptTest);
		if(hPr==0 || ptTest.x<=0){
			hPr=hFore;
		}
		HWND hRes=CreateFloWnd(rect, hPr, sz, 0);
		::SetProp(hRes,"WK_SHELL_COL",(HANDLE)iIndex);
		::SetProp(hRes,"WK_SHELL_THR",(HANDLE)dwHookerThread);
		::SetProp(hRes,"WK_SHELL_MSG",(HANDLE)(WM_USER+31+iIndex));
		::SetProp(hRes,"WK_SHELL_WND",(HANDLE)hFore);
		::SetProp(hRes,"WK_SHELL_CNU",(HANDLE)aCl);
		char szPW[128]={0};
		sprintf(szPW,"WK_SHELL_BT_PW%i",iIndex);
		::SetProp(fList,szPW,(HANDLE)hRes);
/*#ifdef _DEBUG
	if(iIndex==3){
		RECT rt;
		GetWindowRect(hRes,&rt);
		char s[100]="";
		sprintf(s,"Created BT: %X: %i.%i-%i.%i, isWnd=%i, isPWnd=%i, rt->%i.%i",fList,rect.left,rect.top,rect.right,rect.bottom,IsWindow(fList),IsWindow(hFore),rt.left,rt.top);
		SetWindowText(hFore,s);
	}
#endif*/
	}else{
#ifdef _DEBUG
	if(iIndex==3){
		DWORD dwErr=GetLastError();
		char s[100]="";
		sprintf(s,"Created BT: Error %x",dwErr);
		SetWindowText(hFore,s);
	}
#endif	
	}
}
/*
BOOL iSAddtionsInCharge()
{
	BOOL bShellSR=GlobalFindAtom("WK_SHELL_SR");
	BOOL bShellBT=GlobalFindAtom("WK_SHELL_BT");
	return bShellSR||bShellBT;
}
*/
BOOL ReAttachShellListener(HWND hWin)
{
	HWND hFore=hWin?hWin:GetForegroundWindow();
	if(hFore){
		HWND fListForRec=FindWindowEx(hFore,0,"SHELLDLL_DefView",0);
		if(fListForRec==0){
			fListForRec=FindWindowEx(hFore,0,"Snake List",0);
			if(fListForRec){
				// Опять уже внутри его!
				HWND fListForRec2=FindWindowEx(fListForRec,0,"SHELLDLL_DefView",0);
				if(fListForRec2){
					g_hLastObjectSel=GetParent(fListForRec2);
					g_hLastObjectSelRoot=hFore;
					g_szSelectedFile[0]=0;
					return 1;
				}else{
					g_hLastObjectSel=fListForRec;
					g_hLastObjectSelRoot=hFore;
					g_szSelectedFile[0]=0;
					return 1;
				}
			}else{
				//FLOG1("ReAttachShellListener %i",__LINE__);
			}
		}else{
			g_hLastObjectSel=GetParent(fListForRec);
			g_hLastObjectSelRoot=hFore;
			g_szSelectedFile[0]=0;
			return 1;
		}
	}else{
		//FLOG1("ReAttachShellListener %i",__LINE__);
	}
	return 0;
}

void ProcessWindowAdditions(HWND hwndDlg, BOOL bBtOnly, int iType)
{
	if(iType==0){
		bShellSR=GlobalFindAtom("WK_SHELL_SR");
		bShellBT=GlobalFindAtom("WK_SHELL_BT");
	}
	HWND fList=0;
	if(bShellSR || bShellBT){
		HWND hFore=GetForegroundWindow();
		if(hFore){
			if(::GetProp(hFore,"WK_SHELL_NOBT")==0){
				BOOL bBanIt=0;
				char szClass[128]="";
				::GetClassName(hFore,szClass,sizeof(szClass));
				if(!IsWindowsSystemWnd(hFore,0)){// Ищем
					if(g_hLastObjectSel==0 || !IsWindow(g_hLastObjectSel)){
						ReAttachShellListener(hFore);
					}
					fList=FindWindowEx(hFore,0,"Snake List",0);
					if(fList==0){
						fList=FindWindowEx(hFore,0,"SHELLDLL_DefView",0);
						if(fList==0){
							fList=FindWindowEx(hFore,0,"FolderView",0);
						}
					}
					if(bShellSR){
						if(fList || (stricmp(szClass,"WinRarWindow")==0) || (stricmp(szClass,"TTOTAL_CMD")==0)){
							// Метка для патчера системного меню
							::SetProp(hFore,"WK_SHELL_SR_P",(HANDLE)1);
							if(hFore!=hwndDlg){
								bBtOnly=0;
								PatchSystemMenu(hFore);
							}
						}
					}
					bBanIt=(fList==0);
					if(bBanIt && iType!=0){
						if(fList==0){
							//FLOGWND("banning dlg for FileList funcs:",hFore);
							::SetProp(hFore,"WK_SHELL_NOBT",(HANDLE)1);
						}
					}
					if(bShellBT && fList){
						// Порядок обратный иначе парент будет кривой
						{// В любом случае поднимаем окно! если есть
							if((HWND)::GetProp(fList,"WK_SHELL_BT_PW3") && !IsWindow((HWND)::GetProp(fList,"WK_SHELL_BT_PW3"))){
								::SetProp(fList,"WK_SHELL_BT_P3",(HANDLE)0);
								::SetProp(fList,"WK_SHELL_BT_PW3",(HANDLE)0);
							}
							if((DWORD)::GetProp(fList,"WK_SHELL_BT_P3")!=1 && GlobalFindAtom("WK_SHELL_BT3")){
								::SetProp(fList,"WK_SHELL_BT_P3",(HANDLE)1);
								// Добавляем в окно кнопочку
								AddBT(hFore,fList,3);
							}
							HWND hBt=(HWND)::GetProp(fList,"WK_SHELL_BT_PW3");
							if(IsWindow(hBt)){
								::ShowWindow(hBt,SW_SHOW);
								::InvalidateRect(hBt,0,1);
							}
						}
						{// В любом случае поднимаем окно! если есть
							if((HWND)::GetProp(fList,"WK_SHELL_BT_PW2") && !IsWindow((HWND)::GetProp(fList,"WK_SHELL_BT_PW2"))){
								::SetProp(fList,"WK_SHELL_BT_P2",(HANDLE)0);
								::SetProp(fList,"WK_SHELL_BT_PW2",(HANDLE)0);
							}
							if((DWORD)::GetProp(fList,"WK_SHELL_BT_P2")!=1 && GlobalFindAtom("WK_SHELL_BT2")){
								::SetProp(fList,"WK_SHELL_BT_P2",(HANDLE)1);
								// Добавляем в окно кнопочку
								AddBT(hFore,fList,2);
							}
							HWND hBt=(HWND)::GetProp(fList,"WK_SHELL_BT_PW2");
							if(IsWindow(hBt)){
								::ShowWindow(hBt,SW_SHOW);
								::InvalidateRect(hBt,0,1);
							}
						}
						{// В любом случае поднимаем окно! если есть
							if((HWND)::GetProp(fList,"WK_SHELL_BT_PW1") && !IsWindow((HWND)::GetProp(fList,"WK_SHELL_BT_PW1"))){
								::SetProp(fList,"WK_SHELL_BT_P1",(HANDLE)0);
								::SetProp(fList,"WK_SHELL_BT_PW1",(HANDLE)0);
							}					
							if((DWORD)::GetProp(fList,"WK_SHELL_BT_P1")!=1 && GlobalFindAtom("WK_SHELL_BT1")){
								::SetProp(fList,"WK_SHELL_BT_P1",(HANDLE)1);
								// Добавляем в окно кнопочку
								AddBT(hFore,fList,1);
							}
							HWND hBt=(HWND)::GetProp(fList,"WK_SHELL_BT_PW1");
							if(IsWindow(hBt)){
								::ShowWindow(hBt,SW_SHOW);
								::InvalidateRect(hBt,0,1);
							}
						}
						{// В любом случае поднимаем окно! если есть
							if((HWND)::GetProp(fList,"WK_SHELL_BT_PW0") && !IsWindow((HWND)::GetProp(fList,"WK_SHELL_BT_PW0"))){
								::SetProp(fList,"WK_SHELL_BT_P0",(HANDLE)0);
								::SetProp(fList,"WK_SHELL_BT_PW0",(HANDLE)0);
							}					
							if((DWORD)::GetProp(fList,"WK_SHELL_BT_P0")!=1){
								::SetProp(fList,"WK_SHELL_BT_P0",(HANDLE)1);
								// Добавляем в окно кнопочку
								AddBT(hFore,fList,0);
							}
							HWND hBt=(HWND)::GetProp(fList,"WK_SHELL_BT_PW0");
							if(IsWindow(hBt)){
								::ShowWindow(hBt,SW_SHOW);
								::InvalidateRect(hBt,0,1);
							}
						}
					}
				}else{
					//FLOGWND("checking dlg failed, fore is system= ",hFore);
				}
			}else{
				//FLOGWND("checking dlg for FileList funcs-banned!:",hFore);
			}
		}
	}
	if(bBtOnly==0 || fList==0){
		PatchSystemMenu(hwndDlg);
	}
}

void CheckWindowForFitToScreen(HWND hWind)
{
	if (hookData.bSnapToScreenForce){
		if(hWind && !IsWindowsSystemWnd(hWind,1)){
			if(GetAsyncKeyState(VK_SHIFT)>=0){
				if(hookData.bSnapToScreenForceFull || IsWindowStylesOk(hWind)){// Снаппим только окна с границами!!!
					PostThreadMessage(dwHookerThread,WM_USER+14,WPARAM(hWind),0);
				}
			}
		}
	}
}

#include <shlobj.h>
//#include <shobjidl.h>
#define WM_GETISHELLBROWSER (WM_USER + 7)
DWORD WINAPI SendSelectionToRecents(LPVOID p)
{
	bNoRecurseAddSelectionToRecentsStep=__LINE__;
	if(g_szSelectedFile[0]){
		bNoRecurseAddSelectionToRecentsStep=__LINE__;
		if((GetFileAttributes(g_szSelectedFile) & FILE_ATTRIBUTE_DIRECTORY)!=0){
			bNoRecurseAddSelectionToRecentsStep=__LINE__;
			if(g_szSelectedFile[strlen(g_szSelectedFile)-1]!='\\'){
				bNoRecurseAddSelectionToRecentsStep=__LINE__;
				strcat(g_szSelectedFile,"\\");
			}
		}
		bNoRecurseAddSelectionToRecentsStep=__LINE__;
		SHAddToRecentDocs(SHARD_PATH,g_szSelectedFile);
		Sleep(5000);// Ждем, иначе ссылка в рисентах не успеет объявиться
		// Шлем сообщение на апдейт рисен
		bNoRecurseAddSelectionToRecentsStep=__LINE__;
		int iRes=PostThreadMessage(dwHookerThread,WM_USER+35,WPARAM(GetDesktopWindow()),0);
		g_szSelectedFile[0]=0;
		bNoRecurseAddSelectionToRecentsStep=__LINE__;
	}
	return 0;
}

DWORD WINAPI AddSelectionToRecentsX(LPVOID p)
{
	if(bNoRecurseAddSelectionToRecents){
		return 0;
	}
	SimpleTracker lc(bNoRecurseAddSelectionToRecents);
	bNoRecurseAddSelectionToRecentsStep=__LINE__;
	CoInitialize(NULL);
	bNoRecurseAddSelectionToRecentsStep=__LINE__;
	HWND hOwner = (HWND)p;
	{ 
		bNoRecurseAddSelectionToRecentsStep=__LINE__;
		//Сейчас все в осноном потоке.... Sleep(1000);// Держим секунду так как а) слишком часто и б) нужно после обраобтки кликов состояние!!!
		if(IsWindow(hOwner)){
			// Смотрим что за окно под курсором.
			POINT pt;
			GetCursorPos(&pt);
			HWND hUnder=WindowFromPoint(pt);
			char szClassName[256]={0};
			GetClassName(hUnder,szClassName,sizeof(szClassName));
			if(IsWindow(hOwner) && IsWindow(hUnder) 
				&& (strcmp(szClassName,"SysListView32")==0 || strcmp(szClassName,"OpenListView")==0))
			{
				IShellView* psv = 0;
				bNoRecurseAddSelectionToRecentsStep=__LINE__;
				IShellBrowser* psb=(IShellBrowser*)::SendMessage(hOwner, WM_GETISHELLBROWSER, 0, 0 );
				bNoRecurseAddSelectionToRecentsStep=__LINE__;
				if (IsWindow(hOwner) && IsWindow(hUnder) && psb && SUCCEEDED(psb->QueryActiveShellView(&psv))){
					bNoRecurseAddSelectionToRecentsStep=__LINE__;
					IDataObject* pDataObj =0;
					bNoRecurseAddSelectionToRecentsStep=__LINE__;
					if (IsWindow(hOwner) && IsWindow(hUnder) && psv && SUCCEEDED(psv->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (LPVOID*) &pDataObj)))
					{
						bNoRecurseAddSelectionToRecentsStep=__LINE__;
						{
							FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
							STGMEDIUM stg = { TYMED_HGLOBAL };
							HDROP     hDrop;
							// Look for CF_HDROP data in the data object.
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
							if (IsWindow(hOwner) && IsWindow(hUnder) &&  FAILED( pDataObj->GetData ( &fmt, &stg )))
							{
								bNoRecurseAddSelectionToRecentsStep=__LINE__;
								// Nope! Return an "invalid argument" error back to Explorer.
								return E_INVALIDARG;
							}
							// Get a pointer to the actual data.
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
							hDrop = (HDROP) GlobalLock ( stg.hGlobal );
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
							// Make sure it worked.
							if ( NULL == hDrop ){
								return E_INVALIDARG;
							}
							// Sanity check - make sure there is at least one filename.
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
							UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
							if ( 0 == uNumFiles )
							{
								bNoRecurseAddSelectionToRecentsStep=__LINE__;
								GlobalUnlock ( stg.hGlobal );
								ReleaseStgMedium ( &stg );
								bNoRecurseAddSelectionToRecentsStep=__LINE__;
								//return E_INVALIDARG;
							}else{
								for(int i=0;i<int(uNumFiles);i++){
									bNoRecurseAddSelectionToRecentsStep=__LINE__;
									// Get the name of the first file and store it in our member variable m_szFile.
									if ( DragQueryFile ( hDrop, i, g_szSelectedFile, sizeof(g_szSelectedFile))){
										bNoRecurseAddSelectionToRecentsStep=__LINE__;
										if(g_szSelectedFile[0]){
											bNoRecurseAddSelectionToRecentsStep=__LINE__;
#ifdef _DEBUG
											SetWindowText(GetForegroundWindow(),g_szSelectedFile);
#endif
										}
									}
								}
							}
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
							GlobalUnlock ( stg.hGlobal );
							ReleaseStgMedium ( &stg );
							bNoRecurseAddSelectionToRecentsStep=__LINE__;
						}
					}
				}
			}
		}
	}
	bNoRecurseAddSelectionToRecentsStep=__LINE__;
	CoUninitialize();
	bNoRecurseAddSelectionToRecentsStep=__LINE__;
	return 0;
}

DWORD WINAPI AddSelectionToRecents(LPVOID p)
{
	__try{
		return AddSelectionToRecentsX(p);
	}__except(GetExceptionCode()!=0){
		//FLOG1("Exception in AddSelectionToRecents!!! beforeline=%i",bNoRecurseAddSelectionToRecentsStep);
	}
	return 0;
}

#include "../WKPlugin.h"
LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPRETSTRUCT* msg=((CWPRETSTRUCT*)lParam);
	if(!g_hhook1){
		return 0;
	}
	if(WKUtils::isWKUpAndRunning() && dwHookerThread){
		SimpleTracker lock(lHookActive);
		if (nCode>=0 && lParam!=0){
			if(msg->message==WM_CHAR){
				SetLastFocusWnd(msg->hwnd);
			}
			if(msg->message==WM_NCACTIVATE){
				ProcessWindowAdditions(msg->hwnd,1,0);
			}
			if(msg->message==WM_INITDIALOG){
				ProcessWindowAdditions(msg->hwnd,1,4);
			}
			if(msg->message==WM_ACTIVATEAPP){
				// Инициализация
				SetLastFocusWnd(msg->hwnd);
				ProcessWindowAdditions(msg->hwnd,1,1);
				CheckWindwOnTop(msg->hwnd);
			}
			if(msg->message==WM_ENTERMENULOOP){
				ProcessWindowAdditions(msg->hwnd,0,2);
			}
			if(msg->message==WM_NCHITTEST){
				hCurNCHittestWnd=msg->hwnd;
				if(IsWindowsSystemWnd(hCurNCHittestWnd,1)){
					dwCurNCHittestRes=0;
				}else{
					dwCurNCHittestRes=msg->lResult;
				}
				CheckWindwOnTop(msg->hwnd);
			}
			if(msg->message==WM_DISPLAYCHANGE){
				rtScreenSize.left=rtScreenSize.right=rtScreenSize.top=rtScreenSize.bottom=0;
			}else if(hookData.bCatchRollDock || hookData.bCatchRollExpand){
				// Главный поток свободен и ждет приказаний
				if(g_IzWndAttachInProgress==0 && msg->message==WM_NCHITTEST && dwCurNCHittestRes!=0 && dwCurNCHittestRes!=HTCLIENT){
					// GetAsyncKeyState нельзя!!! Outlook к примеру смотрит на меньший бит - была ли нажата кнопа с прошолго вызова GetAsyncKeyState
					BOOL bRKeyDown=(GetAsyncKeyState(VK_RBUTTON)<0);
					BOOL bLKeyDown=(GetAsyncKeyState(VK_LBUTTON)<0);
					BOOL bMKeyDown=(GetAsyncKeyState(VK_MBUTTON)<0);
					if(bRKeyDown || bLKeyDown || bMKeyDown){
						DWORD lHookerDesiredAction=0;
						DWORD lHookerDesiredActionDsc=dwCurNCHittestRes;
						if(hCurNCHittestWnd!=hLastNCHittestWnd || dwCurNCHittestRes!=dwLastNCHittestRes){
							if(hookData.bCatchRollExpand && (bMKeyDown || bRKeyDown)){
								if(dwCurNCHittestRes==HTRIGHT || dwCurNCHittestRes==HTLEFT){
									lHookerDesiredAction=10;
								}else if(dwCurNCHittestRes==HTBOTTOM || dwCurNCHittestRes==HTTOP){
									lHookerDesiredAction=11;
								}else if(dwCurNCHittestRes==HTTOPLEFT){
									lHookerDesiredAction=15;
								}else if(dwCurNCHittestRes==HTTOPRIGHT){
									lHookerDesiredAction=16;
								}else if(dwCurNCHittestRes==HTBOTTOMLEFT){
									lHookerDesiredAction=17;
								}else if(dwCurNCHittestRes==HTBOTTOMRIGHT){
									lHookerDesiredAction=18;
								}
							}
							if(bLKeyDown && hookData.bCatchRollDock){
								if((dwCurNCHittestRes==HTRIGHT || dwCurNCHittestRes==HTLEFT)){
									lHookerDesiredAction=12;
								}else if((dwCurNCHittestRes==HTBOTTOM || dwCurNCHittestRes==HTTOP)){
									lHookerDesiredAction=13;
								}else if(dwCurNCHittestRes==HTTOPLEFT){
									lHookerDesiredAction=19;
								}else if(dwCurNCHittestRes==HTTOPRIGHT){
									lHookerDesiredAction=20;
								}else if(dwCurNCHittestRes==HTBOTTOMLEFT){
									lHookerDesiredAction=21;
								}else if(dwCurNCHittestRes==HTBOTTOMRIGHT){
									lHookerDesiredAction=22;
								}
							}
							if(lHookerDesiredAction!=0){
								hLastNCHittestWnd=msg->hwnd;
								dwLastNCHittestRes=dwCurNCHittestRes;
								g_LKeyActionLast=lHookerDesiredAction;
								g_LKeyActionLastTime=GetTickCount();
								g_LKeyActionHookerDesiredActionDsc=lHookerDesiredActionDsc;
								GetCursorPos(&g_LKeyActionLastPos);
								if(!bLKeyDown){
									PostThreadMessage(dwHookerThread,WM_USER+lHookerDesiredAction,WPARAM(msg->hwnd),LPARAM(lHookerDesiredActionDsc));
									dwCurNCHittestRes=0;
								}
							}
						}
					}
				}
			}
			if(msg->message==WM_ACTIVATEAPP){
				if(msg->wParam==TRUE){
					CheckWindowForFitToScreen(msg->hwnd);
				}
				if(hookData.bCatchAutoMinimize && msg->wParam==FALSE){
					if(msg->hwnd!=NULL && IsWindowVisible(msg->hwnd)){
						DWORD dwStyle=::GetWindowLong(msg->hwnd,GWL_STYLE);
						DWORD dwStyleEx=::GetWindowLong(msg->hwnd,GWL_EXSTYLE);
						if(GetAsyncKeyState(VK_SHIFT)>=0 && ::GetParent(msg->hwnd)==NULL && (dwStyle & WS_CHILD)==0){//(dwStyleEx & WS_EX_APPWINDOW)!=0 && ::GetAncestor(msg->hwnd,GA_PARENT)==NULL && (dwStyleEx & WS_EX_TOOLWINDOW)==0 && 
							PostThreadMessage(dwHookerThread,WM_USER+4,WPARAM(msg->hwnd),0);
						}
					}
				}
			}
		}
	}
	return(CallNextHookEx(g_hhook1, nCode, wParam, lParam));
}

BOOL IsOurWindow(const char* szTitle, DWORD& dwActions)
{
	int i=0;
	dwActions=0;
	while(g_szApplications[i][0]!=0 && i<MAX_APPS){
		if(PatternMatchList0(szTitle,g_szApplications[i]+5)){
			if(g_szApplications[i][0]=='L'){
				dwActions|=ACT_L;
			}
			if(g_szApplications[i][1]=='R'){
				dwActions|=ACT_R;
			}
			if(g_szApplications[i][2]=='T'){
				dwActions|=ACT_T;
			}
			if(g_szApplications[i][3]=='B'){
				dwActions|=ACT_B;
			}

		}
		i++;
	}
	return dwActions!=0;
}

void ProcessWindow(HWND hWnd, DWORD dwActions, RECT& rt, BOOL bForceMove=0)
{
	RECT rtd;
	//GetWindowRect(GetDesktopWindow(),&rtd);
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rtd,0);
	DWORD dwStyle=::GetWindowLong(hWnd,GWL_STYLE);
	BOOL bResizable=0;
	/*
	if((dwStyle & WS_THICKFRAME)!=0 || (dwStyle & WS_BORDER)!=0 || (dwStyle & WS_DLGFRAME)!=0){
		bResizable=1;
	}
	*/
	RECT rtOrig;
	rtOrig.left=rt.left;
	rtOrig.right=rt.right;
	rtOrig.bottom=rt.bottom;
	rtOrig.top=rt.top;
	if(bForceMove){
		::GetWindowRect(hWnd,&rtOrig);
		memcpy(&rt,&rtOrig,sizeof(RECT));
	}
	if((dwActions&ACT_L)!=0){
		if(bResizable){
			rt.left=rtd.left;
		}else{
			rt.right=rtd.left+(rt.right-rt.left);
			rt.left=rtd.left;
		}
	}
	if((dwActions&ACT_R)!=0){
		if(bResizable){
			rt.right=rtd.right;
		}else{
			rt.left=rtd.right-(rt.right-rt.left);
			rt.right=rtd.right;
		}
	}
	if((dwActions&ACT_T)!=0){
		if(bResizable){
			rt.top=rtd.top;
		}else{
			rt.bottom=rtd.top+(rt.bottom-rt.top);
			rt.top=rtd.top;
		}
	}
	if((dwActions&ACT_B)!=0){
		if(bResizable){
			rt.bottom=rtd.bottom;
		}else{
			rt.top=rtd.bottom-(rt.bottom-rt.top);
			rt.bottom=rtd.bottom;
		}
	}
	if(bForceMove || (bResizable && (rtOrig.left!=rt.left || rtOrig.right!=rt.right ||
		rtOrig.top!=rt.top || rtOrig.bottom!=rt.bottom))){
		if(bForceMove){
			rt.right=rt.left+rtOrig.right-rtOrig.left;
			rt.bottom=rt.top+rtOrig.bottom-rtOrig.top;
		}
		::MoveWindow(hWnd,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,TRUE);
	}
}

LRESULT CALLBACK CBTMoveSizeProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode==HCBT_MOVESIZE || nCode==HCBT_ACTIVATE){
		HWND hWin=(HWND)wParam;
		RECT* pPos=(RECT*)lParam;
		if(pPos && !IsZoomed(hWin) && !IsIconic(hWin)){
			if(g_hLastWnd!=hWin){
				g_hLastWnd=hWin;
				GetWindowText(g_hLastWnd,g_szLastWndT,sizeof(g_szLastWndT));
			}
			DWORD dwAction=0;
			if(IsOurWindow(g_szLastWndT,dwAction) && dwAction>0){
				ProcessWindow(g_hLastWnd,dwAction,*pPos,(nCode==HCBT_ACTIVATE));
			}
		}
	}
	return 0;
}

void CheckWndForMin(HWND hWind)
{
	char szTitle[256]={0};
	GetWindowText(hWind,szTitle,sizeof(szTitle));
	if(PatternMatchList0(szTitle,hookData.szAutoMinToTray)){
		PostThreadMessage(dwHookerThread,WM_USER+25,WPARAM(hWind),0);
	}else if(PatternMatchList0(szTitle,hookData.szAutoMinToFloat)){
		PostThreadMessage(dwHookerThread,WM_USER+26,WPARAM(hWind),0);
	}
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning() && dwHookerThread){
		SimpleTracker lock(lHookActive);
		CBTMoveSizeProc(nCode, wParam, lParam);
		if (nCode==HCBT_MOVESIZE && ((hookData.bSnapMovements && hookData.iStickness>0) || (hookData.bSnapToScreen && hookData.iSnapToScreenStickness>0) || hookData.bSnapToScreenForce || hookData.bSnapToScreenForceFull)){
			HWND hWind=(HWND)wParam;
			if(hWind && GetAsyncKeyState(VK_SHIFT)>=0){
				if(hookData.bSnapToScreenForceFull || IsWindowStylesOk(hWind)){// Снаппим только окна с границами!!!
					PostThreadMessage(dwHookerThread,WM_USER+14,WPARAM(hWind),0);
				}
			}
		}else if (nCode==HCBT_MINMAX){
			if(lParam==SW_SHOWMINIMIZED || lParam==SW_MINIMIZE || lParam==SW_SHOWMINNOACTIVE || lParam==SW_FORCEMINIMIZE){
				HWND hWind=(HWND)wParam;
				CheckWndForMin(hWind);
			}
		}else if (nCode==HCBT_ACTIVATE){
			SetLastFocusWnd((HWND)wParam);
			CheckWindowForFitToScreen((HWND)wParam);
		}
		if (nCode==HCBT_CREATEWND){
			dwLastNewWndTick=GetTickCount();
			GetWindowThreadProcessId((HWND)wParam,&dwNewWinProcID);
		}
		if (nCode==HCBT_SETFOCUS /*|| nCode==HCBT_ACTIVATE*/ || nCode==HCBT_SYSCOMMAND){

			SetLastFocusWnd((HWND)wParam);
			if(hookData.bBlockFocuStealing){
				GetWindowThreadProcessId((HWND)wParam,&dwNewWinProcID);
				BOOL bNedavnaaKlavisha=(dwLastWMCHARTick!=0 && GetTickCount()-dwLastWMCHARTick<3000);
				BOOL bDavnoMishBila=(dwLastWMMOUSETick!=0 && GetTickCount()-dwLastWMMOUSETick>3000);
				BOOL bNovoeOknoIsDrugogoProc=(dwThisWinProcID!=dwNewWinProcID);
				if(dwLastNewWndTick!=0 && GetTickCount()-dwLastNewWndTick<3000 &&
					bNedavnaaKlavisha && bDavnoMishBila && bNovoeOknoIsDrugogoProc){
#ifdef _DEBUG
					PostThreadMessage(dwHookerThread,WM_USER+999,0,0);
#endif
					//::SetForegroundWindow(hThisWinProcID);
					//::SetFocus(hThisWinProcID);
					return 1;
				}
				dwLastWMCHARTick=0;
				dwLastWMMOUSETick=0;
				dwLastNewWndTick=0;
			}
		}
	}
	return CallNextHookEx(g_hhook2, nCode, wParam, lParam); 
}   

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(!g_hhook4){
		return 0;
	}
	if(WKUtils::isWKUpAndRunning() && dwHookerThread){
		SimpleTracker lock(lHookActive);
		if (code==HC_ACTION && lParam!=0 && wParam==PM_REMOVE){
			MSG* msg=((MSG*)lParam);
			if(g_hLastObjectSel){
				if(!IsWindow(g_hLastObjectSelRoot)){
					g_hLastObjectSelRoot=0;
					g_hLastObjectSel=0;
					//-----------------------
					if(g_szSelectedFile[0]){
						DWORD dwTick=GetTickCount();
						while(bNoRecurseAddSelectionToRecents && GetTickCount()-dwTick<10000){
							Sleep(100);
						}
						FORK(SendSelectionToRecents,0);
					}
				}else if(g_hLastObjectSelRoot && IsWindow(g_hLastObjectSelRoot) && g_hLastObjectSel!=0 && !IsWindow(g_hLastObjectSel)){
					// Нужно пересоздать аттач
					ReAttachShellListener(0);
				}
				// g_hLastObjectSel уже может занулится
				if(g_hLastObjectSel && (msg->message==WM_RBUTTONUP || msg->message==WM_LBUTTONUP)){
					if(!bNoRecurseAddSelectionToRecents){
#ifdef _DEBUG
/*
						char szCC[256]={0};
						static long lCount=0;
						sprintf(szCC,"OnClick event! %i (%i)",bNoRecurseAddSelectionToRecentsStep,lCount++);
						SetWindowText(GetForegroundWindow(),szCC);
*/
#endif
						//FORK(AddSelectionToRecents,g_hLastObjectSel);
						AddSelectionToRecents(g_hLastObjectSel);
					}
#ifdef _DEBUG
					else{
						char szCC[256]={0};
						static long lCount=0;
						sprintf(szCC,"OnClick event - blocked! %i (%i)",bNoRecurseAddSelectionToRecentsStep,lCount++);
						SetWindowText(GetForegroundWindow(),szCC);
					}
#endif
				}
			}
			if(msg->message==WM_HOTKEY && hookData.bAllowReplaceExplorerKeys){
				// Смотрим класс окна
				char sz[256]={0};
				::GetClassName(msg->hwnd,sz,sizeof(sz));
				if(strcmp(sz,"Shell_TrayWnd")==0){
					BOOL bAllowHK=0;
					bAllowHK|=(msg->wParam==0 && msg->lParam!=0);// клавиши от ярлыков если назначены
					bAllowHK|=(msg->wParam!=0 && msg->lParam==0);// от наших горклавиш, заменителей оригинальных
					if(!bAllowHK){
						//Эти гор. клавиши мы подавляем так как эмулируем их сами!!!
						msg->message=WM_NULL;
					}
				}

			}
			if(msg->message==WM_CHAR){
				if(GetKeyState(VK_CONTROL)<0 || GetKeyState(VK_MENU)<0){
					dwLastWMCHARTick=0;
				}else{
					hThisWinProcID=msg->hwnd;
					GetWindowThreadProcessId(msg->hwnd,&dwThisWinProcID);
					dwLastWMCHARTick=GetTickCount();
				}
			}
			if(msg->message==WM_LBUTTONUP){
				if(g_LKeyActionLast!=0){
					POINT pt2;
					GetCursorPos(&pt2);
					if(GetTickCount()-g_LKeyActionLastTime<800 && GetTickCount()-g_LKeyActionLastTime>20){
						if((pt2.x-g_LKeyActionLastPos.x>=0 && pt2.x-g_LKeyActionLastPos.x<3)||
							(g_LKeyActionLastPos.x-pt2.x>=0 && g_LKeyActionLastPos.x-pt2.x<3)){
							if((pt2.y-g_LKeyActionLastPos.y>=0 && pt2.y-g_LKeyActionLastPos.y<3)||
								(g_LKeyActionLastPos.y-pt2.y>=0 && g_LKeyActionLastPos.y-pt2.y<3)){
								PostThreadMessage(dwHookerThread,WM_USER+g_LKeyActionLast,WPARAM(msg->hwnd),LPARAM(g_LKeyActionHookerDesiredActionDsc));
							}
						}
					}
					dwLastNCHittestRes=0;
					g_LKeyActionLast=0;
				}
			}
			if(msg->message>=WM_MOUSEFIRST && msg->message<=WM_MOUSELAST && msg->message!=WM_MOUSEMOVE){
				hThisWinProcID=msg->hwnd;
				GetWindowThreadProcessId(msg->hwnd,&dwThisWinProcID);
				dwLastWMMOUSETick=GetTickCount();
			}
			if(msg->message==WM_MOUSEMOVE){
				POINT pt;
				GetCursorPos(&pt);
				if(abs(g_prevPT.x=pt.x)<10 && abs(g_prevPT.y=pt.y)<10){
					dwLastWMMOUSETick=GetTickCount();
				}
				g_prevPT.x=pt.x;
				g_prevPT.y=pt.y;
			}
			if(msg->message==WM_SYSCOMMAND && hookData.bInjectItems){
				DWORD dwCommand=msg->wParam;
				HWND hWinToSend=msg->hwnd;
				//F120==SC_RESTORE
				if(dwCommand>=SYSMENU_WKFIRST){// Это наша команда или нет?
					DWORD lHookerDesiredAction=2;
					if(dwCommand==SYSMENU_HCOMP){//SC_MOUSEMENU
						lHookerDesiredAction=7;
					}else if(dwCommand==SYSMENU_FAVL){
						hWinToSend=GetForegroundWindow();
						lHookerDesiredAction=31;
					}else if(dwCommand==SYSMENU_WKMENU){
						lHookerDesiredAction=3;
					}else if(dwCommand==SYSMENU_HTITLE){
						lHookerDesiredAction=6;
					}else if(dwCommand==SYSMENU_HINFO){
						lHookerDesiredAction=8;
					}else if(dwCommand==SYSMENU_HQRUN){
						lHookerDesiredAction=9;
					}else if(dwCommand==SYSMENU_HSHOR){
						lHookerDesiredAction=23;
					}else if(dwCommand==SYSMENU_HKILL){
						lHookerDesiredAction=24;
					}else if(dwCommand==SYSMENU_ONTOP){
						lHookerDesiredAction=27;
					}
					PostThreadMessage(dwHookerThread,WM_USER+lHookerDesiredAction,WPARAM(hWinToSend),0);
				}
			}
		}
	}
	return(CallNextHookEx(g_hhook4, code, wParam, lParam));
}

BOOL WINAPI SetHook(BOOL bSet,char* szLibPath)
{
	BOOL fOk = TRUE;
	if (bSet) {
		/*if(g_hhook1 != NULL){
			fOk=FALSE;
		}else*/
		{
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook1 = SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndRetProc, hDLL, 0);
			fOk &= (g_hhook1 != NULL);
			if(g_hhook1==NULL){
				//LOGERROR1("Failed to set hook1: '%08X'",GetLastError());
			}
		}
		/*if(g_hhook2 != NULL){
			fOk=FALSE;
		}else*/
		{
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook2 = SetWindowsHookEx(WH_CBT, CBTProc, hDLL, 0);
			fOk &= (g_hhook2 != NULL);
			if(g_hhook2==NULL){
				//LOGERROR1("Failed to set hook2: '%08X'",GetLastError());
			}
		}
		/*// Lkz crhbyijnjd- yt bcgjkmpetncz
		if(g_hhook3 != NULL){
			fOk=FALSE;
		}else{
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook3 = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, hDLL, 0);
			fOk &= (g_hhook3 != NULL);
			if(g_hhook3==NULL){
				LOGERROR1("Failed to set hook3: '%08X'",GetLastError());
			}
		}
		*/
		/*if(g_hhook4 != NULL){
			fOk=FALSE;
		}else*/
		{
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook4 = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hDLL, 0);
			fOk &= (g_hhook4 != NULL);
			if(g_hhook4==NULL){
				//LOGERROR1("Failed to set hook4: '%08X'",GetLastError());
			}
		}
	} else {
		BOOL bNoNeedToWait=FALSE;
		if(g_hhook1 != NULL){
			fOk &= SmartUnhookWindowsHookEx(g_hhook1,&bNoNeedToWait);
			g_hhook1 = NULL;
		};
		if(g_hhook2 != NULL){
			fOk &= SmartUnhookWindowsHookEx(g_hhook2,&bNoNeedToWait);
			g_hhook2 = NULL;
		};
		/*if(g_hhook3 != NULL){
			fOk &= SmartUnhookWindowsHookEx(g_hhook3,&bNoNeedToWait);
			g_hhook3 = NULL;
		};*/
		if(g_hhook4 != NULL){
			fOk &= SmartUnhookWindowsHookEx(g_hhook4,&bNoNeedToWait);
			g_hhook4 = NULL;
		};
		SmartUnhookWindowsHookEx(NULL,&bNoNeedToWait);
		if(!bNoNeedToWait){
			long lCount=0;
			while(lHookActive>0){
				lCount++;
				Sleep(20);
				if(lCount>200){
					break;
				}
			}
		}
		if(!fOk){
			//LOGERROR1("Failed to reset hook3: '%08X'",GetLastError());
		}
	}
	return(fOk);
}

BOOL isMemEmpty(LPVOID pData, long count)
{
	BYTE* p=(BYTE*)pData;
	for(long i=0;i<count;i++){
		if(*(p+i)!=0){
			return FALSE;
		}
	}
	return TRUE;
}

