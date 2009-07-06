// WKKeyMaster.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "WP_KeyMaster.h"
#include "DLG_Options.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CWKKeyMasterApp

BEGIN_MESSAGE_MAP(CWKKeyMasterApp, CWinApp)
	//{{AFX_MSG_MAP(CWKKeyMasterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWKKeyMasterApp construction
WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}


CString _l(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}


CWKKeyMasterApp::CWKKeyMasterApp()
{
	int a=0;
}

BOOL UnloadDLLRawGuarded(HINSTANCE hInst, const char* szFile)
{
	int iRes=0;
	__try
	{
		iRes=FreeLibrary(hInst);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){
		//LOGERROR1("UnloadPluginRawGuarded failed on %s!",szFile);
		iRes=0;
	}
	return iRes;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CWKKeyMasterApp object
CWKKeyMasterApp theApp;
CHidMaster hidmanager;
BOOL g_bDialogActive=0;
GPLUGINIni g_hookData;
char szHookLibPath[MAX_PATH]="";
DWORD dwHookerThread=0;
HANDLE hHookerEvent=NULL;
HANDLE hHookerEventUnhook=NULL;
HANDLE hHookerEventStop=NULL;
WKCallbackInterface* pInter=NULL;
BOOL bCurrentHookState=0;
CRITICAL_SECTION csMainThread;
HWND g_hKeyboardTrapWindow=NULL;
BOOL bPluginPaused=0;
CDLG_KMaster* infoWindow=NULL;//Do not use!!!
_ResetKBBuffer hReset=NULL;
DWORD WINAPI GlobalHooker_KMaster(LPVOID pData)
{
	TRACETHREAD;
	::EnterCriticalSection(&csMainThread);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// Создаем очередь сообщений
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetEvent(hHookerEvent);
	{// Пошло-поехало!
		_SetOptions hOpt=NULL;
		_InstallHook hProc=NULL;
		_SetActivity hActi=NULL;
		HINSTANCE hHookDLL=NULL;
		{// Создаем окошко
			infoWindow=new CDLG_KMaster();
			if(!infoWindow){
				return 99;
			}
			infoWindow->Create(CDLG_KMaster::IDD,NULL);//CWnd::GetDesktopWindow()
			// Отпускаем ждущий поток...
			SetEvent(hHookerEvent);
			while(GetMessage(&msg,0,0,0)>0){
				if(bPluginPaused && msg.message!=WM_KILLFOCUS){
					continue;
				}
				if(msg.message==WM_USER+99){
					if(hHookDLL!=NULL){
						if(hOpt){
							hOpt = (_SetOptions)GetProcAddress( hHookDLL,"SetOptions");
							if(hOpt){
								DWORD dwOpt=(g_hookData.bHideCursor?1:0)|(g_hookData.bScanOSD?2:0);
								(*hOpt)(TRUE,dwOpt,GetDoubleClickTime(),g_hookData.lPHTimeout);
							}
							if(g_hookData.lTranspLevel<100){
								SetLayeredWindowStyle(g_hKeyboardTrapWindow,TRUE);
								SetWndAlpha(g_hKeyboardTrapWindow,g_hookData.lTranspLevel,TRUE);
							}else{
								SetLayeredWindowStyle(g_hKeyboardTrapWindow,FALSE);
							}
						}
					}
				}
				if((msg.message==WM_ENABLE && msg.wParam!=0) || msg.message==WM_SETFOCUS){
					// Ставим хук
					BOOL bRes=FALSE;
					hHookDLL = NULL;
					//hHookDLL = GetModuleHandle("wkkeyh.dll");
					if(hHookDLL == NULL){
						hHookDLL = LoadLibrary(szHookLibPath);
					}
					if(hHookDLL!=NULL){
						hActi=(_SetActivity)GetProcAddress( hHookDLL,"SetActivity");
						if(hActi){
							hReset = (_ResetKBBuffer)GetProcAddress( hHookDLL,"ResetKBBuffer");
							if(hReset){
								(*hReset)(0);
							}
							hOpt = (_SetOptions)GetProcAddress( hHookDLL,"SetOptions");
							if(hOpt){
								DWORD dwOpt=(g_hookData.bHideCursor?1:0)|(g_hookData.bScanOSD?2:0);
								(*hOpt)(TRUE,dwOpt,GetDoubleClickTime(),g_hookData.lPHTimeout);
							}
							hProc = (_InstallHook)GetProcAddress( hHookDLL,"InstallHook");
							if(hProc){
								bRes=(*hProc)(TRUE,g_hookData.bUseMouseKeys,g_hKeyboardTrapWindow,bPluginPaused);
							}
							if(pInter && !bRes){
								pInter->ShowPopup("Failed to initialize Keyboard Master plugin!","Plugin error",3000);
							}
							if(g_hookData.lTranspLevel<100){
								SetLayeredWindowStyle(g_hKeyboardTrapWindow,TRUE);
								SetWndAlpha(g_hKeyboardTrapWindow,g_hookData.lTranspLevel,TRUE);
							}else{
								SetLayeredWindowStyle(g_hKeyboardTrapWindow,FALSE);
							}
						}
					}
					if(bRes){
						bCurrentHookState=1;
					}
				}
				if((msg.message==WM_ENABLE && msg.wParam==0) || msg.message==MSGKB_EXIT  || msg.message==WM_KILLFOCUS){
					// Снимаем хук
					if(hHookDLL){
						BOOL bRes=FALSE;
						if(hProc){
							bRes=(*hProc)(FALSE,FALSE,NULL,bPluginPaused);
						}
						if(hReset){
							(*hReset)(0);
						}
						if(bRes){
							bCurrentHookState=0;
						}
						UnloadDLLRawGuarded(hHookDLL,szHookLibPath);
						hHookDLL=NULL;
						SetEvent(hHookerEventUnhook);
					}
				}else if(msg.message==MSGKB_ACTIVHGUID){
					PostMessage(g_hKeyboardTrapWindow,MSGKB_ACTIVHGUID_RW,msg.wParam,msg.lParam);
				}else{
					// Распределяем сообщение
					::DispatchMessage(&msg);
				}
				if(msg.message==MSGKB_EXIT){
					// Выходим если нужно
					if(g_hKeyboardTrapWindow){
						PostMessage(g_hKeyboardTrapWindow,MSGKB_EXIT_RW,msg.wParam,msg.lParam);
					}else{
						break;
					}
				}
				SetEvent(hHookerEvent);
			}// end while
			SetEvent(hHookerEvent);
		}
	}
	::LeaveCriticalSection(&csMainThread);
	SetEvent(hHookerEventStop);
	return 0;
}

#define SAVE_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\WKKeyMast"
int	WINAPI WKPluginStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	::InitializeCriticalSection(&csMainThread);
	WKIHOTKEYS_DistingLRKeys()=g_hookData.bDistingLR;
	return 1;
}

HANDLE hHookerThread=0;
int	WINAPI WKPluginStop()
{
	if(!pInter->getOption(WKOPT_ISSHUTDOWN)){
		PostThreadMessage(dwHookerThread,MSGKB_EXIT,0,0);
		WaitForSingleObject(hHookerEventStop,2000);
		//::EnterCriticalSection(&csMainThread);
		//::LeaveCriticalSection(&csMainThread);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		//TerminateThread(hHookerThread,66);
		//SuspendThread(hHookerThread);
	}
	::DeleteCriticalSection(&csMainThread);
	::CloseHandle(hHookerEvent);
	::CloseHandle(hHookerEventUnhook);
	::CloseHandle(hHookerEventStop);
	// Похоже слипом получается запустить OnIdleProcessing
	// Без него просто падает!
	Sleep(300);
	return 1;
}

int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	szHookLibPath[0]=0;
	if(init){
		pInter=init;
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
		init->GetWKPluginsDirectory(szHookLibPath,MAX_PATH);
	}
	strcat(szHookLibPath,"wkkeyh.dll");
	hHookerEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerEventUnhook=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerEventStop=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_KMaster, NULL, 0, &dwHookerThread);
	WaitForSingleObject(hHookerEvent,INFINITE);
	PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
#ifdef _DEBUG
	char s[100]={0};
	sprintf(s,"%s:%i %08x Debug thread point",__FILE__,__LINE__,dwHookerThread);
	init->ShowPopup(s,"Debug thread",0);
#endif
	return 1;
}

int	WINAPI WKPluginPause(BOOL bPaused)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	bPluginPaused=bPaused;
	if(bPaused){
		PostThreadMessage(dwHookerThread,WM_KILLFOCUS,0,0);
		// Тут это важно!
		::WaitForSingleObject(hHookerEventUnhook,500);
	}else{
		PostThreadMessage(dwHookerThread,WM_SETFOCUS,1,0);
	}
	return 1;
}

HWND g_hCathcerWnd=0;
CString szCatcher;
typedef BOOL (WINAPI *_SetPromiscMode)(HWND,RECT*);
int CALLBACK CatcherDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		g_hCathcerWnd=hwndDlg;
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC1),_l("Learn special devices"));
		::SetWindowText(GetDlgItem(hwndDlg,ID_CLEAR),_l("Clear"));
		::SetWindowText(GetDlgItem(hwndDlg,ID_CLEAR2),_l("Clipboard"));
		
		::SetWindowText(hwndDlg,_l("Learn special devices"));
		PostMessage(hwndDlg,WM_COMMAND,IDD_KOPTIONS,0);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_SHDROCA){
		char* sz=(char*)lParam;
		CString sLine=sz;
		free(sz);
		if(szCatcher.GetLength()){
			szCatcher+="\r\n";
		}
		szCatcher+=sLine;
		PostMessage(hwndDlg,WM_COMMAND,IDD_KOPTIONS,0);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDD_KOPTIONS){
		HWND hEdit=GetDlgItem(hwndDlg,IDC_EDIT);
		::SetWindowText(hEdit,szCatcher);
		::SendMessage(hEdit,EM_LINESCROLL,0,0xFF);
		// Переустанавливаем прямоугольник
		HINSTANCE hLLHookInst=GetModuleHandle("wkkeyh.dll");
		if(hLLHookInst){
			_SetPromiscMode fp=(_SetPromiscMode)GetProcAddress(hLLHookInst,"SetPromiscMode");
			if(fp){
				HWND hDesc=GetDesktopWindow();
				CRect rt;
				::GetWindowRect(hEdit,&rt);
				(*fp)(hwndDlg,&rt);
			}
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==ID_CLEAR2){
		WKGetPluginContainer()->SetClipContent(-1,szCatcher);
		/*g_hCathcerWnd=0;
		::OpenClipboard(hwndDlg);
		int iLen=szCatcher.GetLength()+1;
		HGLOBAL hCliptextA=::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,iLen);
		if(hCliptextA!=NULL){
			
			char* szCliptextA=(char*)::GlobalLock(hCliptextA);
			strcpy(szCliptextA,((const char*)szCatcher));
			::SetClipboardData(CF_TEXT,hCliptextA);//???WTF??? szCliptextW
			//CF_OEMTEXT
			::GlobalUnlock(hCliptextA);
#ifdef _DEBUG
			//AfxMessageBox(szCatcher);
#endif
		}
		::CloseClipboard();
		g_hCathcerWnd=hwndDlg;*/
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==ID_CLEAR){
		szCatcher="";
		PostMessage(hwndDlg,WM_COMMAND,IDD_KOPTIONS,0);
		return TRUE;
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		HINSTANCE hLLHookInst=GetModuleHandle("wkkeyh.dll");
		if(hLLHookInst){
			_SetPromiscMode fp=(_SetPromiscMode)GetProcAddress(hLLHookInst,"SetPromiscMode");
			if(fp){
				(*fp)(NULL,NULL);
			}
		}
		EndDialog(hwndDlg,0);
		hwndDlg=0;
		g_hCathcerWnd=0;
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		PostMessage(hwndDlg,WM_SYSCOMMAND,SC_CLOSE,0);
		return TRUE;
	}
	return FALSE;
}

// required funcs
int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction>5 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==5){
		strcpy(stuff->szItemName,"Learn special devices");
		stuff->hItemBmp=::LoadBitmap(theApp.m_hInstance,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=0;
		stuff->dwDefaultHotkey=-1;
		return 1;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Show all registered hotkeys");
		stuff->hItemBmp=::LoadBitmap(theApp.m_hInstance,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
	}else if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Show hotkeys guide");
		stuff->hItemBmp=::LoadBitmap(theApp.m_hInstance,MAKEINTRESOURCE(IDB_BM_HGUIDE));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=145;
	}else if(iPluginFunction==2 || iPluginFunction==3 || iPluginFunction==4){
		if(iPluginFunction==2){
			strcpy(stuff->szItemName,"Temporarily block hotkeys");
		}
		if(iPluginFunction==3){
			strcpy(stuff->szItemName,"Temporarily block keyboard");
		}
		if(iPluginFunction==4){
			strcpy(stuff->szItemName,"Pause keyboard (but not hotkeys)\tUseful for using in 'sequence' hotkeys. Just start every sequence hotkey from this one to prevent active application to hanlde hotkey keys");
			//Useful for using in 'sequence' hotkeys. Just start every sequence hotkey from this one to prevent active application to hanlde hotkey keys
		}
		stuff->hItemBmp=::LoadBitmap(theApp.m_hInstance,MAKEINTRESOURCE(IDB_BM_ICON));
		if(iPluginFunction==2){
			stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_MISC|WKITEMPOSITION_OPTFLOATS;
		}else{
			stuff->dwItemMenuPosition=0;
		}
		/*
		if(iPluginFunction==2){
			stuff->dwItemMenuPosition=WKITEMPOSITION_OPTFLOATS;
		}
		*/
		stuff->dwDefaultHotkey=0;
	}
	return 1;
}

CIHotkey g_cantBeLockedHotkey;
BOOL g_bTeporalyBlockHotkeys=FALSE;
BOOL g_bTeporalyBlockAll=FALSE;
BOOL g_bTeporalyBlockAllButHK=FALSE;
int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction==5){
		RETURN_IFRECURSE(0);
		DialogBox(theApp.m_hInstance,MAKEINTRESOURCE(IDD_JTT),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),CatcherDialogProc);
		if(!WKUtils::isWKUpAndRunning()){
			return 0;
		}
	}else if(iPluginFunction==1){
		PostThreadMessage(dwHookerThread,MSGKB_ACTIVHGUID,0,!g_bDialogActive);
	}else if(iPluginFunction==2 || iPluginFunction==3  || iPluginFunction==4){
		static int c=0;
		c++;
		BOOL bOsd=1;
		CRegisteredHotkeyData* pDsc=0;
		if(stuff){
			if(stuff->pSpecialHotkeyDsc){
				pDsc=(CRegisteredHotkeyData*)stuff->pSpecialHotkeyDsc;
				if(pDsc){
					g_cantBeLockedHotkey=pDsc->oHotkey;
				}else{
					// Это вызов из макроса...
					bOsd=0;
					g_cantBeLockedHotkey.Reset();
				}
			}
		}else{
			g_cantBeLockedHotkey.Reset();
		}
		CString sText;
		if(iPluginFunction==2){
			g_bTeporalyBlockHotkeys=!g_bTeporalyBlockHotkeys;
			sText=g_bTeporalyBlockHotkeys?"All hotkeys are disabled":"All hotkeys are enabled";
		}
		if(iPluginFunction==3){
			if(pDsc && (pDsc->oRealTriggeredHotkey.Options()&ADDOPT_ON_UNPRESS)!=0){
				bOsd=0;
				AfxMessageBox("Sorry, OnUnpress hotkeys are not supported\nfor this function! Change hotkey properties");
			}else{
				g_bTeporalyBlockAll=!g_bTeporalyBlockAll;
				sText=g_bTeporalyBlockAll?"Keyboard blocked":"Keyboard unblocked";
			}
		}
		if(iPluginFunction==4){
			g_bTeporalyBlockAllButHK=!g_bTeporalyBlockAllButHK;
			sText=g_bTeporalyBlockAllButHK?"Keyboard paused":"Keyboard unpaused";
			//TRACE2("[%i] %i\n",c,g_bTeporalyBlockAllButHK);
			//bOsd=0;
		}
		if(bOsd && pInter && sText!=""){
			char szTranslation[128]="";
			pInter->GetTranslation(sText,szTranslation,sizeof(szTranslation));
			//FLOG1("OSD = %s",szTranslation);
			int iCountLimit=20;
			while(pInter->ShowOSD(szTranslation,3000)==FALSE && iCountLimit){
				Sleep(200);
				iCountLimit--;
			}
			//TRACE2("[%i]: %s\n",c,sText);
		}
		// Для правильной обработки подгоняем массив запретов
		GetListOfCurrentlyRegisteredHotkeys(TRUE);
	}else{
		WKPluginOptions();
	}
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction>4 || stuff==NULL || iPluginFunction==0){
		return 0;
	}
	if(iPluginFunction==1){
		stuff->bChecked=g_bDialogActive;
	}
	if(iPluginFunction==2){
		stuff->bChecked=g_bTeporalyBlockHotkeys;
	}
	if(iPluginFunction==3){
		stuff->bChecked=g_bTeporalyBlockAll;
	}
	if(iPluginFunction==4){
		stuff->bChecked=g_bTeporalyBlockAllButHK;
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=isWin9x()?FALSE:TRUE;
	//strcpy(dsc->szTitle,"Extended hotkey processing");
	strcpy(dsc->szDesk,"This plugin allows you to create hotkeys from unusual key combinations");
	dsc->hPluginBmp=::LoadBitmap(theApp.m_hInstance,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
	WaitForSingleObject(hHookerEvent,1000);
	CDLG_Options dlg;
	dlg.DoModal();
	PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
	WaitForSingleObject(hHookerEvent,1000);
	if(pInter){
		pInter->PushMyOptions(0);
	}
	return 1;
}
BOOL IsWindows98()
{
	static BOOL bIsWin98=-1;
	if(bIsWin98==-1){
		bIsWin98=0;
		OSVERSIONINFOEX winfo; 
		ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
		winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
		if (GetVersionEx((OSVERSIONINFO *)&winfo)){ 
			bIsWin98=!(winfo.dwPlatformId==VER_PLATFORM_WIN32_NT);
		} 
	}
	return bIsWin98;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iAction==OM_STARTUP_ADD){
		//pOptionsCallback->AddActionHotkey("pauseAction","Pause keyboard (but not hotkeys)","Useful for using in 'sequence' hotkeys. Just start every sequence hotkey from this one to prevent active application to hanlde hotkey keys",4,OL_KEYBOARD);
		pOptionsCallback->AddActionHotkey("blockAction","Temporarily block hotkeys","",2);
		pOptionsCallback->AddBoolOption("hideCur","Hide mouse cursor while typing","",false,OL_KEYBOARD);
		pOptionsCallback->AddBoolOption("BlockApps","Block keypresses on 'Apps' key","Check this option if you don`t want to use 'Apps' key alone. This can make use of 'Apps' key for wired key combinations more comfortable",FALSE,OL_KEYBOARD);
		pOptionsCallback->AddListOption("BlockWins","Block keypresses on 'Win' key","Check this option if you don`t want to use 'Win' key alone. This can make use of 'Win' key for wired key combinations more comfortable",_l("After hotkey only")+'\t'+_l("Block 'Win' key")+'\t'+_l("Block only left 'Win' key")+'\t'+_l("Block only right 'Win' key")+'\t'+_l("Do not block 'Win' key"),0,OL_KEYBOARD);
		pOptionsCallback->AddStringOptionEx("WinBlockMask","Block only if one of this applications is active","Use '*' as a wildcard for partial matches, ';' as delimiter. Leave empty to disable application check","",0,OL_KEYBOARD,"BlockWins");
		pOptionsCallback->AddListOption("ShiftDrop","Do special processing for 'Caps Lock' key","Here you can allow 'Shift' key to drop 'Caps lock' state or disable Caps Lock at all (recommended. With blocked Caps Lock you will be able to use it in hotkeys. In this case Caps lock state can be triggered by pressing 'Shift-Caps Lock')",_l("Do not affect Caps Lock")+"\t"+_l("Drop Caps Lock state when I press Shift key"),0,OL_KEYBOARD);//+"\t"+_l("Block keypresses on 'Caps Lock', trigger caps state by 'Shift-Caps lock' key")
		pOptionsCallback->AddBoolOptionEx("CLIcon","Show special tray icon when CapsLock is ON","",FALSE,OL_KEYBOARD);
		pOptionsCallback->AddBoolOptionEx("NLIcon","Show special tray icon when NumLock is ON","",FALSE,OL_KEYBOARD);
		pOptionsCallback->AddBoolOptionEx("SLIcon","Show special tray icon when ScrollLock is ON","",FALSE,OL_KEYBOARD);
		if(!IsWindows98()){
			pOptionsCallback->AddNumberOption("TLevel","Hotkey guide - window`s opacity level (percents)","",100,0,100,0);
		}
		pOptionsCallback->AddNumberOption("PH_time","Timeout for 'Press and Hold' modifier","How long you have to hold hotkey before 'Press and Hold' modifier would be added to hotkey, in seconds. Set it to '0' to disable this feature",0,0);
		pOptionsCallback->AddTip("add_p","Keyboard pause and 'Sequence' hotkeys",0);
		pOptionsCallback->AddBoolOptionEx("unpauseOnHK","Pause keyboard: Unpause keyboard when hotkey is triggered","",true,0,"add_p");
		pOptionsCallback->AddBoolOptionEx("unpauseOnHKAdd","Pause keyboard: Add pause key to all new sequence hotkeys","",true,0,"add_p");
		pOptionsCallback->AddBoolOptionEx("unpauseOnHKSwitch","Pause keyboard: Begin hotkey with 'Pause' key to make it 'Sequence' hotkey automatically","",true,0,"add_p");
		pOptionsCallback->AddTip("add1","Extended options",0);
		pOptionsCallback->AddBoolOptionEx("AutoRepl","Auto-repeat hotkey when pressed for awhile","This is very similar to Window`s auto-repeat feature, but for hotkeys, not for single keys",TRUE,0,"add1");
		pOptionsCallback->AddBoolOptionEx("LRDist","Distinguish between Left and Right keys","This option allows you to use in hotkeys left and right shift/control/alt/win keys",TRUE,0,"add1");
		pOptionsCallback->AddBoolOptionEx("UseMouse","Allow mouse buttons in hotkey combinations","This option allows you to use in hotkeys mouse buttons",TRUE,0,"add1");
		pOptionsCallback->AddBoolOptionEx("CornersDistinct","Allow monitor corners int hotkey combinations","This option allows you to use monitor corners in hotkeys, for example you can set screen-saver to be started when you move your mouse into right-top corner of the screen",TRUE,0,"add1");
		//pOptionsCallback->AddBoolOptionEx("blockM","When keyboard is blocked: Block mouse keys too","",TRUE,0,"add1");
		//pOptionsCallback->AddBoolOptionEx("ScanOSD","Show keyboard scancodes in OSD when choosing hotkey","",FALSE,0,"add1");
		pOptionsCallback->AddBoolOption("enableHID","Use special devices","Enable this option to use attached HID devices (such as gamepads) in hotkeys",true,0);
		pOptionsCallback->AddActionOption("catcher","Learn special devices",5);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		g_hookData.bUnpauseOnHK=pOptionsCallback->GetBoolOption("unpauseOnHK");
		g_hookData.bAddUnpause=pOptionsCallback->GetBoolOption("unpauseOnHKAdd");
		g_hookData.unpauseOnHKSwitch=pOptionsCallback->GetBoolOption("unpauseOnHKSwitch");
		g_hookData.bScanOSD=0;//pOptionsCallback->GetBoolOption("ScanOSD");
		g_hookData.bCLIcon=pOptionsCallback->GetBoolOption("CLIcon");
		g_hookData.bNLIcon=pOptionsCallback->GetBoolOption("NLIcon");
		g_hookData.bSLIcon=pOptionsCallback->GetBoolOption("SLIcon");
		g_hookData.bBlockMouse=1;//По другому не работает pOptionsCallback->GetBoolOption("blockM");
		g_hookData.enableHID=pOptionsCallback->GetBoolOption("enableHID");
		g_hookData.bHideCursor=pOptionsCallback->GetBoolOption("hideCur");
		g_hookData.bAutoRepeat=pOptionsCallback->GetBoolOption("AutoRepl");
		g_hookData.bDistingLR=pOptionsCallback->GetBoolOption("LRDist");
		g_hookData.bBlockApps=pOptionsCallback->GetBoolOption("BlockApps");
		g_hookData.bBlockWin=pOptionsCallback->GetBoolOption("BlockWins");
		g_hookData.bUseMouseKeys=pOptionsCallback->GetBoolOption("UseMouse");
		pOptionsCallback->GetStringOption("WinBlockMask",g_hookData.szWinBlockMask,sizeof(g_hookData.szWinBlockMask));
		g_hookData.bDistingCorners=pOptionsCallback->GetBoolOption("CornersDistinct");
		g_hookData.lPHTimeout=pOptionsCallback->GetNumberOption("PH_time");
		if(!g_hookData.bUseMouseKeys){
			g_hookData.bDistingCorners=0;
		}
		g_hookData.bShiftDropsCapslock=pOptionsCallback->GetListOption("ShiftDrop");
		if(!IsWindows98()){
			g_hookData.lTranspLevel=pOptionsCallback->GetNumberOption("TLevel");
		}
		WKIHOTKEYS_DistingLRKeys()=g_hookData.bDistingLR;
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_USER+99,0,0);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("unpauseOnHK",g_hookData.bUnpauseOnHK);
		pOptionsCallback->SetBoolOption("unpauseOnHKAdd",g_hookData.bAddUnpause);
		pOptionsCallback->SetBoolOption("unpauseOnHKSwitch",g_hookData.unpauseOnHKSwitch);
		//pOptionsCallback->SetBoolOption("ScanOSD",g_hookData.bScanOSD);
		pOptionsCallback->SetBoolOption("CLIcon",g_hookData.bCLIcon);
		pOptionsCallback->SetBoolOption("NLIcon",g_hookData.bNLIcon);
		pOptionsCallback->SetBoolOption("SLIcon",g_hookData.bSLIcon);
		//pOptionsCallback->SetBoolOption("blockM",g_hookData.bBlockMouse);
		pOptionsCallback->SetBoolOption("enableHID",g_hookData.enableHID);
		pOptionsCallback->SetBoolOption("hideCur",g_hookData.bHideCursor);
		pOptionsCallback->SetBoolOption("AutoRepl",g_hookData.bAutoRepeat);
		pOptionsCallback->SetBoolOption("CornersDistinct",g_hookData.bDistingCorners);
		pOptionsCallback->SetBoolOption("LRDist",g_hookData.bDistingLR);
		pOptionsCallback->SetBoolOption("BlockApps",g_hookData.bBlockApps);
		pOptionsCallback->SetBoolOption("BlockWins",g_hookData.bBlockWin);
		pOptionsCallback->SetBoolOption("UseMouse",g_hookData.bUseMouseKeys);
		pOptionsCallback->SetStringOption("WinBlockMask",g_hookData.szWinBlockMask);
		pOptionsCallback->SetNumberOption("PH_time",g_hookData.lPHTimeout);
		pOptionsCallback->SetListOption("ShiftDrop",g_hookData.bShiftDropsCapslock);
		if(!IsWindows98()){
			pOptionsCallback->SetNumberOption("TLevel",g_hookData.lTranspLevel);
		}
	}
	return 1;
}

BOOL WINAPI RebuildKeyboardStates(DWORD* baBlockKeys)
{
	CWKSYSEvent pEvent;
	pEvent.pData=baBlockKeys;
	pEvent.iEventType=WKSYSEVENTTYPE_REBUILDHK;
	WKGetPluginContainer()->NotifyAboutSystemEvent(&pEvent);
	// Ескли включено блокирование клавы...
	if(g_hookData.bBlockWin!=4){
		baBlockKeys[VK_RWIN]|=BLOCKSTATE_PROCESS;
		baBlockKeys[VK_LWIN]|=BLOCKSTATE_PROCESS;
	}
	if(g_hookData.bShiftDropsCapslock){
		//WK-GetPluginContainer()->ShowBaloon("!!!","???",1000);
		baBlockKeys[VK_SHIFT]|=BLOCKSTATE_PROCESS;
		baBlockKeys[VK_LSHIFT]|=BLOCKSTATE_PROCESS;
		baBlockKeys[VK_RSHIFT]|=BLOCKSTATE_PROCESS;
	}else{
		//WKGetPluginContainer()->ShowBaloon(":(((","???",1000);
	}
	if(g_hookData.bBlockApps){
		baBlockKeys[VK_APPS]|=BLOCKSTATE_PROCESS;
	}
	if(g_bTeporalyBlockAll || g_bTeporalyBlockAllButHK){
		//int iSz=sizeof(baBlockKeys[0]);
		//memset(baBlockKeys,0,KBBUFFER_DSIZE*iSz);
		// Блокируем все что не запрещено
		for(int i=0;i<KBBUFFER_DSIZE;i++){
			if(g_bTeporalyBlockAllButHK){
				baBlockKeys[i]&=~BLOCKSTATE_BLOCK;
				baBlockKeys[i]|=BLOCKSTATE_PROCESS;
			}else{
				baBlockKeys[i]=BLOCKSTATE_BLOCK;
			}
		}
		if(g_cantBeLockedHotkey.Present()){
			MapHotkeyToBlockBuffer(baBlockKeys,g_cantBeLockedHotkey,TRUE);
		}
	}
	return TRUE;
}

typedef void (WINAPI *_GetDefaultPauseKey)(void*);
void WINAPI GetDefaultPauseKey(void* p)
{
	CIHotkey* pHK=(CIHotkey*)p;
	if(p && g_hookData.bAddUnpause){
		CIHotkey* pPause=(CIHotkey*)WKGetPluginContainer()->getOptionEx(1,4);
		if(pPause && (pPause->m_dwBaseKey[0]!=0 || pPause->m_dwModifiers!=0)){
			pHK->AddOneModifier(pPause->m_dwModifiers);
			pHK->CopyKeysOnly(*pPause);
		}
	}
}

typedef BOOL (WINAPI *_PostProcessHotkey)(void*);
BOOL WINAPI PostProcessHotkey(void* p)
{
	CIHotkey* pHK=(CIHotkey*)p;
	//CString s=GetIHotkeyName(*pHK);
	if(pHK && g_hookData.unpauseOnHKSwitch){
		if(!pHK->isSeq() && !pHK->isEvent()){
			CIHotkey* pPause=(CIHotkey*)WKGetPluginContainer()->getOptionEx(1,4);
			if(pPause && pPause->Present()){
				//CString s2=GetIHotkey Name(*pPause);
				BOOL bTheSame=pPause->isEqual(*pHK,0);
				if(pPause->pNextHK){
					bTheSame|=pPause->pNextHK->isEqual(*pHK,0);
					if(pPause->pNextHK->pNextHK){
						bTheSame|=pPause->pNextHK->pNextHK->isEqual(*pHK,0);
					}
				}
				if(bTheSame){
					// Все сбрасываем, делаем секю
					pHK->DuplicateHotkey(*pPause);
					//CString s=GetIHotkeyName(*pHK);
					pHK->Options()=ADDOPT_SEQUENCE|ADDOPT_TRANSPARENT;
				}
			}
		}
		return 1;
	}
	return 0;
}

CWnd* AfxWPGetMainWnd()
{
	static CWnd wnd;
	static BOOL bInit=0;
	if(bInit==0){
		bInit=1;
		wnd.Attach(WKGetPluginContainer()->GetWKMainWindow());
	}
	return &wnd;
}

CString GetUserFolder()
{
	return "";
};

BOOL isFileExist(const char* szFile)
{
	return 0;
}

#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif