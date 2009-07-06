#include "stdafx.h"
#include <process.h>
#include "_common.h"
#include "_externs.h"


/*
убрать опции в квикранах
хранилище опций по имени
джойстик
перевод макросов - проверка на непустое имя
xml-формат макросов
*/


BOOL bHideIcon=0;
CCriticalSection csClipAttach;
CCriticalSection csClipRecreation;
extern int iGlobalState;
/////////////////////////////////////////////////////////////////////////////
// AppMainDlg dialog
//test
#define MAX_MIXER_VOL	65535
#define QRUN_ONWKSTART	0x0001
#define QRUN_NOFOREGR	0x0002
#define QRUN_BYHOTKEY	0x0004
#define QRUN_DETACHTO	0x0008
#define QRUN_DETACHFL	0x0010
#define QRUN_SHOWMENU	0x0020
#define QRUN_WAITCDEL	0x0040
DWORD dwClipAttachTime=0;
BOOL HandleTuneUpItem2(DWORD dwId)
{
	if(dwId==RCLICKHLP){
		AfxMessageBox(_l("Right-click on menu item to popup additional options"));
		return 1;
	}
	return 0;
}

CNewVersionChecker& GetVersionChecker()
{_XLOG_
	static CNewVersionChecker verChecker(PROG_VERSION,__DATE__,EscapeString(Format("ls=%i, u=%s, days=%i",objSettings.iLikStatus,objSettings.sLikUser,GetWindowDaysFromInstall())));
	return verChecker; 
}

DWORD WINAPI SendCommand_InThread(LPVOID pData)
{_XLOG_
	DWORD dwCommand=(DWORD)pData;
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->PostMessage(WM_COMMAND,dwCommand,0);
	}
	return 0;
}

static long lHotkeyActionInProgress=0;
BOOL AppMainDlg::TuneUpItem(const char* szTopic)
{_XLOG_
	if(lHotkeyActionInProgress && objSettings.lHotMenuOpened==0){//Вызов по активации горячей клавишей - и не из-за hotmenu
		return FALSE;
	}
	if(GetAsyncKeyState(VK_SHIFT)<0){_XLOG_
		objSettings.OpenOptionsDialog(0,szTopic,this);
		return TRUE;
	}
	return FALSE;
}

BOOL AppMainDlg::TuneUpItemX(const char* szTopic)
{_XLOG_
	if(lHotkeyActionInProgress && objSettings.lHotMenuOpened==0){//Вызов по активации горячей клавишей - и не из-за hotmenu
		return FALSE;
	}
	if(GetAsyncKeyState(VK_SHIFT)<0){_XLOG_
		objSettings.OpenOptionsDialog(1,szTopic,this);
		return TRUE;
	}
	return FALSE;
}

BOOL AppMainDlg::IsIconMustBeHided()
{_XLOG_
	/*if(objSettings.bIntegrateWithWN && IsOtherProgrammAlreadyRunning(PARENT_PROG)){_XLOG_
		return TRUE;
	}*/
	return !objSettings.bShowIconInTray;
}

void AppMainDlg::DoDataExchange(CDataExchange* pDX)
{_XLOG_
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AppMainDlg)
	//DDX_Control(pDX, IDC_LOGO, m_Logo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AppMainDlg, CDialog)
	//{{AFX_MSG_MAP(AppMainDlg)
	ON_BN_CLICKED(ID_ENTCODE, OnEntcode)
	ON_BN_CLICKED(ID_WEBSITE, OnWebsite)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SYSTRAY_DONATE, OnDonate)
	ON_COMMAND(ID_SYSTRAY_ENTERKEY, OnEnterKey)
	ON_COMMAND(ID_HOTMENU_SHOWEXEPATH, OnHotmenuShowexepath)
	ON_COMMAND(ID_HOTMENU_ADDTOQUICKRUNS, OnHotmenuAddToQRun)
	ON_COMMAND(ID_HOTMENU_SHORTCUTONDESKTOP, OnHotmenuAddToDesktop)
	ON_COMMAND(ID_HOTMENU_SHORTCUTONQLAUNCH, OnHotmenuAddToQLaunch)
	ON_COMMAND(ID_HOTMENU_ADDTOAHIDE, OnHotmenuAddToAHide)
	ON_COMMAND(ID_HOTMENU_SHORTCUTONSTARTMENU, OnHotmenuAddToStartMenu)
	ON_COMMAND(ID_HOTMENU_SHORTCUTONUDLOCATION, OnHotmenuAddToUDFolder)
	ON_COMMAND(ID_SYSTRAY_OPTIONS, OnSystrayOptions)
	ON_COMMAND(ID_SYSTRAY_OPTIONS2, OnSystrayOptions2)
	ON_COMMAND(ID_SYSTRAY_PLUGINOPTIONS, OnSystrayPlgOptions)
	ON_COMMAND(ID_SYSTRAY_MOREPLUGINSANDMACROS, OnSystrayPlgDownload)
	ON_COMMAND(ID_SYSTRAY_VLUME0, OnSystraySetVol0)
	ON_COMMAND(ID_SYSTRAY_VLUME30, OnSystraySetVol30)
	ON_COMMAND(ID_SYSTRAY_VLUME50, OnSystraySetVol50)
	ON_COMMAND(ID_SYSTRAY_VLUME70, OnSystraySetVol70)
	ON_COMMAND(ID_SYSTRAY_VLUME100, OnSystraySetVol100)
	ON_COMMAND(ID_SYSTRAY_MUTESOUND, OnSystraySetMute)
	ON_WM_ENDSESSION()
	ON_COMMAND(ID_SYSTRAY_RUNEMAILCLIENT, StartDefEmail)
	ON_COMMAND(ID_SYSTRAY_RUNBROWSER, StartDefBrowser)
	ON_COMMAND(ID_SYSTRAY_EJECTCLOSECD, OnSwapCD)
	ON_COMMAND(ID_SYSTRAY_SHOWWKMENU, ShowHotMenu)
	ON_COMMAND(ID_SYSTRAY_CURRENTKEYBINDINGS, ShowKeyBindings)
	ON_COMMAND(ID_HOTMENU_BRINGINVIEW, OnHotmenuBringinview)
	ON_COMMAND(ID_HOTMENU_SEMITRANSPARENT, OnHotmenuSemitransparent)
	ON_COMMAND(ID_HOTMENU_EXITMENU, OnHotmenuExitmenu)
	ON_COMMAND(ID_HOTMENU_KILLPROCESS, OnHotmenuKillprocess)
	ON_COMMAND(ID_SYSTRAY_KILLACTWND, OnHotmenuKillprocessUnderCur)
	ON_COMMAND(ID_HOTMENU_TRANSPARENCY100, OnHotmenuTransparency100)
	ON_COMMAND(ID_HOTMENU_TRANSPARENCY20, OnHotmenuTransparency20)
	ON_COMMAND(ID_HOTMENU_TRANSPARENCY40, OnHotmenuTransparency40)
	ON_COMMAND(ID_HOTMENU_TRANSPARENCY60, OnHotmenuTransparency60)
	ON_COMMAND(ID_HOTMENU_TRANSPARENCY80, OnHotmenuTransparency80)
	ON_COMMAND(ID_HOTMENU_SWITCHONTOP, OnHotmenuSwitchontop)
	ON_COMMAND(ID_HOTMENU_HIDETOTRAY, OnHotmenuHidetotray)
	ON_COMMAND(ID_SYSTRAY_SHOWALLAPPS, OnSystrayShowAllApps)
	ON_COMMAND(ID_SYSTRAY_BOSSPANIC, OnSystrayBossPanic)
	ON_COMMAND(ID_HOTMENU_FREEMEMORY, OnHotmenuFreememory)
	ON_COMMAND(ID_SYSTRAY_ABOUT, OnSystrayAbout)
	ON_COMMAND(ID_REATTACHCLIP, OnReattachClips)
	ON_COMMAND(ID_SYSTRAY_EXIT, OnSystrayExit)
	ON_COMMAND(ID_SYSTRAY_OPEN, OnSystrayOpen)
	ON_COMMAND(ID_HOTMENU_TAKESCREENSHOT, OnHotmenuTakescreenshot)
	ON_COMMAND(ID_SYSTRAY_HELP, OnSystrayHelp)
	ON_COMMAND(ID_HOTMENU_HIDECOMPLETELY, OnHotmenuHidecompletely)
	ON_COMMAND(ID_HOTMENU_RUNSCREENSAVER, OnHotmenuRunscreensaver)
	ON_COMMAND(ID_SYSTRAY_SHUTDOWN, OnHotmenuShutdown)
	ON_COMMAND(ID_SYSTRAY_RESTART, OnHotmenuRestart)
	ON_COMMAND(ID_SYSTRAY_SUSPEND, OnHotmenuSuspend)
	ON_COMMAND(ID_HOTMENU_SCREENSAVERENABLED, OnHotmenuScreensaverenabled)
	ON_COMMAND(ID_SYSTRAY_SENDASUGGESTION, OnSystraySendasuggestion)
	ON_COMMAND(ID_SYSTRAY_ADDQRUNAPPLICATION, OnSystrayAddqrunapplication)
	ON_COMMAND(ID_SYSTRAY_ADDQRUNAPPWND, OnSystrayAddqrunappwnd)
	
	ON_COMMAND(ID_SYSTRAY_ADDMACROS, OnSystrayAddqrunmacros)
	ON_COMMAND(ID_SYSTRAY_RECMACRO, OnSystrayReqMacro)
	
	ON_COMMAND(ID_SYSTRAY_RESTOREDESKTOPICONS, OnSystrayRestoreIcons)
	ON_COMMAND(ID_SYSTRAY_SAVEDESKTOPICONS, OnSystraySaveIcons)
	ON_COMMAND(ID_SYSTRAY_PROCLISTPARAMS, OnSystrayProcListPars)
	ON_COMMAND(ID_HOTMENU_TUNEUPMENU, OnHotmenuTxtPars)
	ON_COMMAND(ID_SYSTRAY_MAKESCREENSHOT, OnSystrayScrShot)
	ON_COMMAND(ID_SYSTRAY_DISPLAYPROPERTIES, OnSystrayDisplayProps)
	ON_COMMAND(ID_SYSTRAY_ADDREMOVEPROGS, OnSystrayAddRemProgs)
	ON_COMMAND(ID_SYSTRAY_LOCKWORKSTATION, OnLockStation)

	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(FIREALERT, OnFireAlert)
	ON_MESSAGE(EDITTEXT, OnEDITTEXT)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(ID_SYSTRAY_RUNQRUNAPP, OnRunQRunApplication)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(theApp.iWM_EXTERN, ONWM_EXTERN)
	ON_REGISTERED_MESSAGE(theApp.iWM_WIRENOTE, ONWM_WIRENOTE)
	ON_REGISTERED_MESSAGE(theApp.iWM_THIS, ONWM_THIS)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppMainDlg message handlers
void AppendPluginsToMenu(CMenu* pMenu, DWORD dwMenuMask, DWORD dwSubMenuMask)
{_XLOG_
	if(pMainDialogWindow){_XLOG_
		// Из трея - вытаскиваем из запаски
		objSettings.hLastForegroundWndForPlugins=pMainDialogWindow->m_STray.hLastActiveWindow;
	}
	for(int i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
		CPlugin* plg=objSettings.plugins[i];
		if(!plg){_XLOG_
			continue;
		}
		if(!plg->bIsActive){_XLOG_
			continue;
		}
		//TRACE(plg->sTitle);		TRACE("\n");
		for(int j=0;j<plg->funcs.GetSize();j++){_XLOG_
			if(((plg->funcs[j].dwItemMenuPosition) & dwMenuMask) == 0){_XLOG_
				continue;
			}
			if(((plg->funcs[j].dwItemMenuPosition) & dwSubMenuMask) == 0){_XLOG_
				continue;
			}
			WKPluginFunctionActualDsc dsc;
			if(plg->fActualParamsFunction){_XLOG_
				(*plg->fActualParamsFunction)(j,&dsc); 
			}
			if(dsc.bVisible){_XLOG_
				CString sTitle;
				if(strlen(dsc.szNonDefaultItemText)!=0){_XLOG_
					sTitle=dsc.szNonDefaultItemText;
				}else{_XLOG_
					sTitle=plg->_l(plg->funcs[j].szItemName);
				}
				CIHotkey lHotKey(plg->funcs[j].oRealHotkey);
				if(lHotKey.Present()){_XLOG_
					sTitle+="\t";
					sTitle+=GetIHotkeyName(lHotKey,FALSE,FALSE);
				}
				CBitmap bmpT;
				BOOL bPlugIcon=0;
				if(plg->funcs[j].hRealItemBmp){_XLOG_
					bmpT.Attach(plg->funcs[j].hRealItemBmp);
					bPlugIcon=1;
				}
				AddMenuString(pMenu,plg->funcs[j].dwCommandCode,sTitle,(bPlugIcon?&bmpT:NULL),dsc.bChecked,NULL,dsc.bGrayedout,dsc.bDisabled,FALSE);
				if(bPlugIcon){_XLOG_
					bmpT.Detach();
				}
			}
		}
	}
}

#define PROC_MENU_ID	"*** PROCESSES ***"
_ProcEnum TrayMenuProcesses;
extern CCriticalSection pCahLock;
void AppendProcessesToMenu(CMenu& ProcMenu, CString& sProcessIcon, DWORD& dwProcID, BOOL bFully=FALSE)
{_XLOG_
	__FLOWLOG;
	if(bFully==FALSE){_XLOG_
		AddMenuString(&ProcMenu,0,PROC_MENU_ID);
		return;
	}
	// Список процессов
	CAppBooster objThis;
	TrayMenuProcesses.RemoveAll();
	if(!EnumProcesses(0,objSettings.bSortForSysMenu)){_XLOG_
		AddMenuString(&ProcMenu,0,_l("Failed to get processes list"),NULL,FALSE,NULL,TRUE);
		return;
	}
	if(objSettings.bExtractIconsForSysMenu){_XLOG_
		GetProcessesIcons();
	}
	CCSLock lpc(&pCahLock,1);
	TrayMenuProcesses.Copy(GetProccessCash());//TODO Тут падает!!! на копировании CProcInfo
	// Свободная память
	int iCountDop=0;//3+(objSettings.bShowMemUsage?1:0)+(objSettings.lDisableSCHelp?0:1);

	//ProcMenu.AppendMenu(MF_SEPARATOR, 0, "");
	if(objSettings.sListToKillSim!=""){_XLOG_
		//AddMenuString(&ProcMenu,ID_SYSTRAY_KILLPROCESSESFROMLIST,_l("Kill processes from list")+getActionHotKeyDsc(KILL_PROCLIST),_bmp().Get(IDB_BM_DEL));
		CProcInfo p;
		p.dwCustomMessageID=ID_SYSTRAY_KILLPROCESSESFROMLIST;
		p.sProcName=_l("Kill processes from list")+getActionHotKeyDsc(KILL_PROCLIST);
		p.dwCstomIconIndex=_bmp().Get(IDB_BM_DEL);
		p.bTerminable=1;
		TrayMenuProcesses.Add(p);
	}
	if(objSettings.bShowMemUsage){_XLOG_
		MEMORYSTATUS memstat;
		memset(&memstat,0,sizeof(memstat));
		GlobalMemoryStatus(&memstat);
		double dPercent=100*double(memstat.dwAvailPhys)/double(memstat.dwTotalPhys);
		//AddMenuString(&ProcMenu,WM_USER+MAX_USERMSG+__LINE__,_l("Memory")+Format(": %lu%% (%luMb)\t%luMb",int(dPercent),int(memstat.dwAvailPhys/1024/1024),int(memstat.dwTotalPhys/1024/1024)),_bmp().Get(_IL(21)),FALSE,NULL,TRUE,TRUE);
		CProcInfo p;
		p.dwCustomMessageID=WM_USER+MAX_USERMSG+__LINE__;
		p.sProcName=_l("Memory")+Format(": %lu%% (%luMb)\t%luMb",int(dPercent),int(memstat.dwAvailPhys/1024/1024),int(memstat.dwTotalPhys/1024/1024));
		p.dwCstomIconIndex=_bmp().Get(_IL(21));
		p.bTerminable=0;
		TrayMenuProcesses.Add(p);
	}
	if(objSettings.bUnderWindowsNT){_XLOG_
		//AddMenuString(&ProcMenu,ID_HOTMENU_FREEMEMORY,_l("Free physical memory")+getActionHotKeyDsc(FREE_MEMORY),_bmp().Get(_IL(23)));
		CProcInfo p;
		p.dwCustomMessageID=ID_HOTMENU_FREEMEMORY;
		p.sProcName=_l("Free physical memory")+getActionHotKeyDsc(FREE_MEMORY);
		p.dwCstomIconIndex=_bmp().Get(_IL(23));
		p.bTerminable=1;
		TrayMenuProcesses.Add(p);
	}
	{
		//AddMenuString(&ProcMenu,ID_SYSTRAY_PROCLISTPARAMS," ["+_l("Customize this menu")+"]",_bmp().Get(IDB_BM_OPTIONS));
		CProcInfo p;
		p.dwCustomMessageID=ID_SYSTRAY_PROCLISTPARAMS;
		p.sProcName=" ["+_l("Customize this menu")+"]";
		p.dwCstomIconIndex=_bmp().Get(IDB_BM_OPTIONS);
		p.bTerminable=1;
		TrayMenuProcesses.Add(p);
	}
	if(!objSettings.lDisableSCHelp){_XLOG_
		//AddMenuString(&ProcMenu,RCLICKHLP,_l("Right-click: more options"),0,FALSE,NULL,FALSE);
		CProcInfo p;
		p.dwCustomMessageID=RCLICKHLP;
		p.sProcName=_l("Right-click: more options");
		p.dwCstomIconIndex=0;
		p.bTerminable=0;
		TrayMenuProcesses.Add(p);
	}

	int iTotalMenuItemsCount=TrayMenuProcesses.GetSize()+iCountDop;
	int iNumberOfColumns=objSettings.bPlistUse2Columns;
	while(iNumberOfColumns>1 && (iTotalMenuItemsCount/iNumberOfColumns)<13){
		iNumberOfColumns--;
	}
	if(iNumberOfColumns<1){
		iNumberOfColumns=1;
	}
	//А также сбалансировать по остатку в последнем столбце!
	int iml=0;
	if(iCountDop){
		//iml=-iCountDop/iNumberOfColumns-1;
	}
	int iEachBreak=iTotalMenuItemsCount/iNumberOfColumns;
	while(iNumberOfColumns*iEachBreak<iTotalMenuItemsCount){
		iEachBreak++;
	}
	TRACE3("=== Breaker %i %i-%i\n",iEachBreak,iTotalMenuItemsCount,iCountDop);
	BOOL bShift=(GetAsyncKeyState(VK_SHIFT)<0);
	for(int iPrCount=0;iPrCount<TrayMenuProcesses.GetSize();iPrCount++){_XLOG_
		BOOL bBreakMenu=FALSE;
		DWORD dwPosProcId=(TrayMenuProcesses[iPrCount]).dwProcId;
		CProcInfo& pInfo=TrayMenuProcesses[iPrCount];
		if(underSpyMode()){
			if(!bShift){
				if(pInfo.sProcName=="WireKeys.exe" || pInfo.sProcName==SPY_MODE_APP){
					continue;
				}
			}
		}
		if(objSettings.sListToHideSim.GetLength()>0 && !bShift && PatternMatchList(pInfo.sProcName,objSettings.sListToHideSim)){
			continue;
		}
		if(!objSettings.bUnderWindows98){_XLOG_
			// Только под NT, под 98ыми во втором столбце не пишется после /t ничего
			if(iNumberOfColumns>1){
				if(iml>2 && (iml % iEachBreak)==0){
					bBreakMenu=TRUE;
					TRACE1("=== Breakerin in %i\n",iml);
				}
			}
		}
		if(pInfo.dwCustomMessageID){
			AddMenuString(&ProcMenu,pInfo.dwCustomMessageID,pInfo.sProcName,pInfo.dwCstomIconIndex,FALSE,NULL,!pInfo.bTerminable,FALSE,bBreakMenu);
		}else{
			CString sProc="";
			/*if(pInfo.lProcent>0){_XLOG_
				sProc=Format(",%i%%",pInfo.lProcent);
			}*/
			CString sPart1=pInfo.sProcName+sProc;
			if(objSettings.bPlistUseFullPaths && pInfo.sProcExePath!=""){
				sPart1=pInfo.sProcExePath;
			}
			CString sTitle=Format("%s\t%s",sPart1,pInfo.lWorkingSet>0?Format("%ik",int(pInfo.lWorkingSet/1024)):"");
			if(objSettings.bExtractIconsForSysMenu && dwProcID==dwPosProcId){_XLOG_
				sProcessIcon=pInfo.sIconIndex;
			}
			TRACE1("=== adding %i\n",iml);
			AddMenuString(&ProcMenu,WM_USER+KILLPROCESS+iPrCount,sTitle/*Format("%i. %s",iPrCount,sTitle)*/,_bmp().GetRaw(pInfo.sIconIndex),FALSE,NULL,!pInfo.bTerminable,FALSE,bBreakMenu);
		}
		iml++;
	}
}


void AppendDeskactionsToMenu(CMenu* pMenu, DWORD dwMenuMask)
{_XLOG_
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		if(objSettings.bMiscSaverMenu==3 || (objSettings.bMiscSaverMenu==1 && (dwMenuMask & WKITEMPOSITION_TRAY)!=0)
			|| (objSettings.bMiscSaverMenu==2 && (dwMenuMask & WKITEMPOSITION_HOTMENU)!=0)){_XLOG_
			return;
		}
	}
	CMenu pMiscSaver;
	CMenu* pOut=&pMiscSaver;
	if(dwMenuMask & WKITEMPOSITION_DBLHOTMENU){_XLOG_
		pOut=pMenu;
	}else{_XLOG_
		pMiscSaver.CreatePopupMenu();
	}
	// Десктопные фукнции
	// Разрешения...
	if(objSettings.videoModes.GetSize()>0){_XLOG_
		CMenu DModesMenu;
		DModesMenu.CreatePopupMenu();
		CVideoMode md,mdTmp,mdCor;
		CVideoModes::GetCurrentVideoMode(md);
		mdCor.m_dwHeight=123;
		mdCor.m_dwWidth=123;
		for(int i=objSettings.videoModesShort.GetSize()-1;i>=0;i--){_XLOG_
			CVideoMode& mdCur=objSettings.videoModesShort[i];
			if(mdCur.m_dwHeight!=mdCor.m_dwHeight || mdCur.m_dwWidth!=mdCor.m_dwWidth){_XLOG_
				BOOL bCurrent=FALSE;
				if(md.m_dwWidth==mdCur.m_dwWidth && md.m_dwHeight==mdCur.m_dwHeight){_XLOG_
					bCurrent=TRUE;
				}
				AddMenuString(&DModesMenu,WM_USER+SET_DMODE+mdCur.dwAdditionalData,mdCur.Name(),0,bCurrent);
				mdCor=mdCur;
			}
		}
		TUNE_UP_ITEM2(DModesMenu,"Right-click: more options");
		AddMenuSubmenu(pOut,&DModesMenu,_l("Set resolution")+"\t...",_bmp().Get(_IL(92)));
	}
	AddMenuString(pOut,ID_SYSTRAY_DISPLAYPROPERTIES,_l("Display properties"),_bmp().Get(_IL(42)));
	if(!objSettings.bUnderWindows98){_XLOG_
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
		// Дополнительные десктопы
		if(objSettings.lEnableAddDesktops){_XLOG_
			CMenu DesktopsMenu;
			DesktopsMenu.CreatePopupMenu();
			for(int i=0;i<objSettings.lDesktopSwitches;i++){_XLOG_
				AddMenuString(&DesktopsMenu,WM_USER+SWITCH_DESKTOP+i,wk.GetDesktopName(i)+getActionHotKeyDsc(objSettings.DefDSwitchMapHK[i]),NULL,(objSettings.lLastActiveDesktop==i)?TRUE:FALSE);
			}
			AddMenuSubmenu(pOut,&DesktopsMenu,_l("Desktops")+"\t...",_bmp().Get(_IL(27)));
		}
		AddMenuString(pOut,ID_SYSTRAY_RESTOREDESKTOPICONS,_l("Restore desktop icons")+getActionHotKeyDsc(ICONS_RESTORE),_bmp().Get(_IL(24)));
		AddMenuString(pOut,ID_SYSTRAY_SAVEDESKTOPICONS,_l("Save desktop icons")+getActionHotKeyDsc(ICONS_SAVE),_bmp().Get(_IL(89)));
	}
	{_XLOG_
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(pOut,ID_HOTMENU_RUNSCREENSAVER,_l("Launch screen saver")+getActionHotKeyDsc(RUN_SCREENSAVER),_bmp().Get(_IL(82)));
		BOOL bRes=FALSE;
		CString sSSText="";
		::SystemParametersInfo(16/*SPI_GETSCREENSAVEACTIVE*/,0,&bRes,0);
		int iIcon=0;
		if(bRes){_XLOG_
			iIcon=16;
			sSSText=_l("Screen saver")+": "+_l("Enabled")+"\t("+_l("Disable")+")";
		}else{_XLOG_
			iIcon=17;
			sSSText=_l("Screen saver")+": "+_l("Disabled")+"\t("+_l("Enable")+")";
		}
		AddMenuString(pOut,ID_HOTMENU_SCREENSAVERENABLED,sSSText,_bmp().Get(_IL(iIcon)));
	}
	{_XLOG_
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
		AppendPluginsToMenu(pOut,dwMenuMask,WKITEMPOSITION_DESK);
		AddMenuString(pOut,ID_SYSTRAY_MAKESCREENSHOT,_l("Make screenshot")+getActionHotKeyDsc(TAKE_SCREENSHOT),_bmp().Get(_IL(72)));
	}
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		TUNE_UP_ITEM2(*pOut,"Right-click: more options");
		AddMenuSubmenu(pMenu,pOut,_l("Desktop related")+"\t...",_bmp().Get(_IL(30)));
	}
}

void AppendMiscSaverToMenu(CMenu* pMenu, DWORD dwMenuMask)
{_XLOG_
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		if(objSettings.bMiscSaverMenu==3 || (objSettings.bMiscSaverMenu==1 && (dwMenuMask & WKITEMPOSITION_TRAY)!=0)
			|| (objSettings.bMiscSaverMenu==2 && (dwMenuMask & WKITEMPOSITION_HOTMENU)!=0)){_XLOG_
			return;
		}
	}
	CMenu pMiscSaver2;
	CMenu* pOut=&pMiscSaver2;
	if(dwMenuMask & WKITEMPOSITION_DBLHOTMENU){_XLOG_
		pOut=pMenu;
	}else{_XLOG_
		pMiscSaver2.CreatePopupMenu();
	}
	if(objSettings.DefActionMapHK[BOSS_KEY].Present()){_XLOG_
		AddMenuString(pOut,ID_SYSTRAY_BOSSPANIC,_l("Boss key")+getActionHotKeyDsc(BOSS_KEY),_bmp().Get(_IL(77)));
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
	}
	AddMenuString(pOut,ID_SYSTRAY_EJECTCLOSECD,_l("Eject/Load CD Drive")+getActionHotKeyDsc(OPEN_CD),_bmp().Get(IDB_BM_CDDRIVE));
	AddMenuString(pOut,ID_SYSTRAY_ADDREMOVEPROGS,_l("Add/Remove programs"),_bmp().Get(IDB_BM_ADDREMPRG));
	if((dwMenuMask & WKITEMPOSITION_TRAY)!=0){_XLOG_
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(pOut,ID_SYSTRAY_SHOWWKMENU,_l("Popup hot menu")+getActionHotKeyDsc(HOT_MENU),_bmp().Get(_IL(25)));
	}
	AppendPluginsToMenu(pOut,dwMenuMask,WKITEMPOSITION_MISC);
	{_XLOG_
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
		if(objSettings.bUnderWindowsNT){_XLOG_
			AddMenuString(pOut,ID_SYSTRAY_LOCKWORKSTATION,_l("Lock computer")+getActionHotKeyDsc(LOCK_STATION),_bmp().Get(_IL(10)));
		}
		AddMenuString(pOut,ID_SYSTRAY_SHUTDOWN,_l("Shutdown computer")+getActionHotKeyDsc(FAST_SHUTDOWN),_bmp().Get(IDB_BM_SHUTDOWN));
		AddMenuString(pOut,ID_SYSTRAY_RESTART,_l("Restart computer")+CString(objSettings.DefActionMapHK[FAST_SHUTDOWN].Present()?getActionHotKeyDsc(FAST_SHUTDOWN,"\t2*"):""),_bmp().Get(IDB_BM_RESTART));
	}
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		AddMenuSubmenu(pMenu,pOut,_l("Miscellaneous")+"\t...",_bmp().Get(_IL(26)));
	}
}

void AppendHiddenToMenu(CMenu* m_pMenu)
{_XLOG_
	CMenu WndMenu;
	WndMenu.CreatePopupMenu();
	CSmartLock SL(&csInfoWnds,TRUE);
	for(int i=0; i<objSettings.aHidedWindowsMap.GetSize();i++){_XLOG_
		InfoWnd* pW=objSettings.aHidedWindowsMap[i];
		if(pW && pW->Sets){_XLOG_
			CString sHidIcon=Format("HIDDEN_ICON%x08",pW->Sets->hWnd);
			if(_bmp().Get(sHidIcon)==NULL){_XLOG_
				_bmp().AddBmp(sHidIcon,pW->Sets->hIcon);
			}
			AddMenuString(&WndMenu,WM_USER+HIDEDWINDOWS+i,TrimMessage(pW->GetTitle(),30)+(i==(objSettings.aHidedWindowsMap.GetSize()-1)?getActionHotKeyDsc(LAST_HIDDEN):""),_bmp().Get(sHidIcon));
		}
	}
	if(objSettings.aHidedWindowsMap.GetSize()>1){_XLOG_
		WndMenu.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&WndMenu,ID_SYSTRAY_SHOWALLAPPS,_l("Show all applications")+getActionHotKeyDsc(LAST_HIDDEN,"\t2*"),(CBitmap*)NULL);
	}
	AddMenuSubmenu(m_pMenu,&WndMenu,_l("Hidden windows")+"\t...",_bmp().Get(IDB_BM_HIDDEN));
}

int AppendQRunSingleToMenu(CMenu* pMenuRoot, CMenu* pMenu, DWORD dwMacrosMask, int* iCountTotal, const char* szCategID)
{_XLOG_
	int iCount=0;
	if(szCategID==NULL){_XLOG_
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			CQuickRunItem* pItem=objSettings.aQuickRunApps[i];
			if(pItem){_XLOG_
				if(pItem->lOptionClass==-1){_XLOG_
					pItem->lOptionClass=i;
				}
				if(pItem->lHowToShowInTray==0){_XLOG_
					continue;
				}
				if(pItem->lMacros==CATEG_MACR){_XLOG_
					CMenu pCategMenu;
					pCategMenu.CreatePopupMenu();
					int iCategCount=AppendQRunSingleToMenu(pMenuRoot, &pCategMenu, dwMacrosMask, iCountTotal, pItem->sUniqueID);
					if(iCategCount>0){_XLOG_
						AddMenuSubmenu(pMenu,&pCategMenu,pItem->sItemName+getActionHotKeyDsc(pItem->qrHotkeys[0]->oHotKey),_bmp().Get(_IL(100)));
						iCount+=iCategCount;
					}
				}
			}
		}
	}
	for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
		CQuickRunItem* pItem=objSettings.aQuickRunApps[i];
		if(pItem){_XLOG_
			if(pItem->lOptionClass==-1){_XLOG_
				pItem->lOptionClass=i;
			}
			if(DWORD(pItem->lMacros)==dwMacrosMask || (dwMacrosMask==ANY_MACRO)){_XLOG_
				if((szCategID!=NULL && pItem->sCategoryID!=szCategID) 
					|| (szCategID==NULL && pItem->sCategoryID!="")){_XLOG_
					continue;
				}
				if(iCountTotal){_XLOG_
					(*iCountTotal)++;
				}
				if(pItem->lHowToShowInTray==0){_XLOG_
					continue;
				}
				CBitmap* qrBmp=NULL;
				BOOL bAdded=FALSE;
				if(pItem->lMacros==0){_XLOG_
					CString sTargetFile=pItem->sItemPath;
					SubstMyVariablesForQRun(sTargetFile,0,0,TRUE);
					CString sQRunIcon=GetExeIconIndex(sTargetFile);
					if(sQRunIcon!=""){_XLOG_
						qrBmp=_bmp().Get(sQRunIcon);
					}
					if(pItem->lParamType==2 && pItem->lExpandFolderOrCheckInstance){_XLOG_
						bAdded=TRUE;
						CMenu sub;
						sub.CreatePopupMenu();
						AddSlashToPath(sTargetFile);
						AddMenuString(&sub,0,Format("%s%s%s%s",EXPAND_FOLDER_ID,sTargetFile,EXPAND_FOLDER_MASK,pItem->sAutoExpandMask));
						AddMenuSubmenu(pMenu,&sub,pItem->sItemName+getActionHotKeyDsc(pItem->qrHotkeys[0]->oHotKey),qrBmp);
					}
				}else{_XLOG_
					qrBmp=_bmp().Get(_IL(wk.GetIconNumByMacro(pItem->lMacros)));
				}
				if(!bAdded){_XLOG_
					/*if(pItem->lExpandFolderOrCheckInstance && pItem->hWnd!=NULL && pItem->hWnd!=HWND(-2) && pItem->hWnd!=HWND(-1) && IsWindow(pItem->hWnd)){_XLOG_
					continue;
					}*/
					if(pItem->lShowInUpMenu && pMenuRoot && pMenuRoot!=pMenu){_XLOG_
						AddMenuString(pMenuRoot,WM_USER+QRUNAPPLICATION+i,pItem->sItemName+getActionHotKeyDsc(pItem->qrHotkeys[0]->oHotKey),qrBmp);
					}
					AddMenuString(pMenu,WM_USER+QRUNAPPLICATION+i,pItem->sItemName+getActionHotKeyDsc(pItem->qrHotkeys[0]->oHotKey),qrBmp);
				}
				iCount++;
			}
		}
	}
	return iCount;
}

void AppendVolumeToMenu(CMenu* pMenu, DWORD dwMenuMask)
{_XLOG_
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		if(objSettings.bVolumeMenu==3 || (objSettings.bVolumeMenu==1 && (dwMenuMask & WKITEMPOSITION_TRAY)!=0)
			|| (objSettings.bVolumeMenu==2 && (dwMenuMask & WKITEMPOSITION_HOTMENU)!=0)){_XLOG_
			return;
		}
	}
	if(!IsVistaSoundControl()){
		if(pMainDialogWindow){// Volume
			CMenu pMiscVol;
			pMiscVol.CreatePopupMenu();
			BOOL bMuted=0;
			UINT iCurPos=0;
			if(pMainDialogWindow->mixer.isOpened()){_XLOG_
				iCurPos=pMainDialogWindow->mixer.GetVolumeCtrlValue();
			bMuted=pMainDialogWindow->mixer.GetMuteCtrlValue();
			}
			AddMenuString(&pMiscVol,ID_SYSTRAY_VLUME0,_l("Volume 0%"),NULL,iCurPos>0&& iCurPos<((UINT)(MAX_MIXER_VOL*0.1)));
			AddMenuString(&pMiscVol,ID_SYSTRAY_VLUME30,_l("Volume 30%"),NULL,iCurPos>0&& iCurPos>=((UINT)(MAX_MIXER_VOL*0.1))&&iCurPos<((UINT)(MAX_MIXER_VOL*0.4)));
			AddMenuString(&pMiscVol,ID_SYSTRAY_VLUME50,_l("Volume 50%"),NULL,iCurPos>0&& iCurPos>=((UINT)(MAX_MIXER_VOL*0.4))&&iCurPos<((UINT)(MAX_MIXER_VOL*0.6)));
			AddMenuString(&pMiscVol,ID_SYSTRAY_VLUME70,_l("Volume 70%"),NULL,iCurPos>0&& iCurPos>=((UINT)(MAX_MIXER_VOL*0.6))&&iCurPos<((UINT)(MAX_MIXER_VOL*0.8)));
			AddMenuString(&pMiscVol,ID_SYSTRAY_VLUME100,_l("Volume 100%"),NULL,iCurPos>0&& iCurPos>=((UINT)(MAX_MIXER_VOL*0.9)));
			pMiscVol.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&pMiscVol,ID_SYSTRAY_MUTESOUND,_l("Mute sound")+getActionHotKeyDsc(SOUND_MUTE),NULL,bMuted);
			AddMenuSubmenu(pMenu,&pMiscVol,_l("Sound volume")+"\t...",_bmp().Get(_IL(53)));
		}
	}
}

void AppendMacrosToMenu(CMenu* pMenu, DWORD dwMenuMask);
void AppendSelTextToMenu(CMenu* pMenu,BOOL bActiveWindowPresent, DWORD dwMenuMask)
{_XLOG_
	CMenu* pMenuR=0;
	CMenu ExtraClipMenu;
	if(dwMenuMask & WKITEMPOSITION_DBLHOTMENU){_XLOG_
		pMenuR=pMenu;
	}else{_XLOG_
		ExtraClipMenu.CreatePopupMenu();
		pMenuR=&ExtraClipMenu;
	}
	CMenu PasteMenu;
	if(objSettings.bAddClipboard){_XLOG_
		int a=0;
		PasteMenu.CreatePopupMenu();
		for(a=1;a<=objSettings.lClipCount;a++){_XLOG_
			SimpleLocker l(&csClipBuffer);
			AddMenuString(&PasteMenu,WM_USER+ID_PASTECLIP+a,trimTextForMenu(objSettings.sClipBuffer[a])+"\t"+getClipHotKeyDsc(a+1,FALSE),(CBitmap*)NULL);
		}
		PasteMenu.AppendMenu(MF_SEPARATOR, 0, "");
		CString sTitle;
		for(a=1;a<=objSettings.lClipCount;a++){_XLOG_
			sTitle=_l("Clip")+Format(" #%i - ",a)+_l("history");
			AddMenuString(&PasteMenu,WM_USER+ID_CLIPHISTORY+a,sTitle+getActionHotKeyDsc(objSettings.DefClipMapHK_H[a]),_bmp().Get(_IL(18)));
		}
		AddMenuString(&PasteMenu,WM_USER+ID_CLIPHISTORY+0,_l("History of clipboard")+getActionHotKeyDsc(objSettings.DefClipMapHK_H[0]),_bmp().Get(_IL(18)));
		AddMenuString(&PasteMenu,ID_HOTMENU_CLEARCLIPS,_l("Clear clipboards"),_bmp().Get(_IL(37)));
	}
	AddMenuString(pMenuR,ID_HOTMENU_SWITCHCASE,_l("Switch text case")+getActionHotKeyDsc(CHANGE_CASE));
	AddMenuString(pMenuR,ID_HOTMENU_UPPERCASE,_l("Convert to UPPER case"));
	AddMenuString(pMenuR,ID_HOTMENU_LOWERCASE,_l("Convert to lower case"));
	if(objSettings.lHKLCount>1){_XLOG_
		AddMenuString(pMenuR,ID_HOTMENU_CONVERTSELECTED,_l("Convert layout")+getActionHotKeyDsc(CONVERT_SELECTED));
	}
	AppendPluginsToMenu(pMenuR,WKITEMPOSITION_HOTMENU,WKITEMPOSITION_SELTEXT);
	if(objSettings.bAddClipboard){_XLOG_
		int a=0;
		CMenu CopyMenu;
		CopyMenu.CreatePopupMenu();
		for(a=1;a<=objSettings.lClipCount;a++){_XLOG_
			SimpleLocker l(&csClipBuffer);
			AddMenuString(&CopyMenu,WM_USER+ID_COPYTOCLIP+a,trimTextForMenu(objSettings.sClipBuffer[a])+"\t"+getClipHotKeyDsc(a+1,TRUE),(CBitmap*)NULL);
		}
		AddMenuSubmenu(pMenuR,&CopyMenu,_l("Copy selection to clip")+"\t...",_bmp().Get(_IL(19)));
	}
	if(dwMenuMask & WKITEMPOSITION_DBLHOTMENU){_XLOG_
		if(objSettings.bAddClipboard){_XLOG_
			AddMenuSubmenu(pMenu,&PasteMenu,_l("Additional clips")+"\t...",_bmp().Get(_IL(31)));
		}
		AppendMacrosToMenu(pMenu,WKITEMPOSITION_HOTMENU);
	}
	pMenuR->AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(pMenuR,ID_HOTMENU_CALCULATE,_l("Calculate")+getActionHotKeyDsc(RUN_ASCOMMAND));
	AddMenuString(pMenuR,ID_HOTMENU_RUNASCOMMAND,_l("Execute as command")+getActionHotKeyDsc(RUN_ASCOMMAND));
	{_XLOG_
		CMenu sortMenu;
		sortMenu.CreatePopupMenu();
		AddMenuString(&sortMenu,ID_HOTMENU_SORTLINES,_l("Sort lines automatically"));
		sortMenu.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&sortMenu,ID_HOTMENU_SORTLINES_AZ,_l("Sort in A..Z order")+" ("+_l("As string")+")");
		AddMenuString(&sortMenu,ID_HOTMENU_SORTLINES_ZA,_l("Sort in Z..A order")+" ("+_l("As string")+")");
		AddMenuString(&sortMenu,ID_HOTMENU_SORTLINES_09,_l("Sort in 0..9 order")+" ("+_l("As number")+")");
		AddMenuString(&sortMenu,ID_HOTMENU_SORTLINES_90,_l("Sort in 9..0 order")+" ("+_l("As number")+")");
		AddMenuSubmenu(pMenuR,&sortMenu,_l("Sort text lines")+"\t...");
	}
	AddMenuString(pMenuR,ID_HOTMENU_COUNTCHARACTERS,_l("Characters count"));
	if(objSettings.lEnableTransmitMode && (bActiveWindowPresent || objSettings.lClipTransmitMode)){_XLOG_
		AddMenuString(pMenuR,ID_HOTMENU_SETASCLIPTARGET,objSettings.lClipTransmitMode?_l("Disable transmit"):_l("Enable transmit"));
	}
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		pMenuR->AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(pMenuR,ID_HOTMENU_TUNEUPMENU,_l("Customize this menu"),_bmp().Get(IDB_BM_OPTIONS));
	}
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		AddMenuSubmenu(pMenu,pMenuR,_l("Selected text")+"\t...",_bmp().Get(_IL(15)));
	}
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		if(objSettings.bAddClipboard){_XLOG_
			AddMenuSubmenu(pMenu,&PasteMenu,_l("Additional clips")+"\t...",_bmp().Get(_IL(31)));
		}
	}
}

void AppendMacrosToMenu(CMenu* pMenu, DWORD dwMenuMask)
{_XLOG_
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		if(objSettings.bMacroMenu==3 || (objSettings.bMacroMenu==1 && (dwMenuMask & WKITEMPOSITION_TRAY)!=0)
			|| (objSettings.bMacroMenu==2 && (dwMenuMask & WKITEMPOSITION_HOTMENU)!=0)){_XLOG_
			return;
		}
	}
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	int iCount2=0,iCountTotal=0;
	CMenu pQRun;
	CMenu* pOut=&pQRun;
	if(dwMenuMask & WKITEMPOSITION_DBLHOTMENU){_XLOG_
		pOut=pMenu;
	}else{_XLOG_
		pQRun.CreatePopupMenu();
	}
	iCount2+=AppendQRunSingleToMenu(pMenu,pOut,1,&iCountTotal);
	iCount2+=AppendQRunSingleToMenu(pMenu,pOut,2,&iCountTotal);
	iCount2+=AppendQRunSingleToMenu(pMenu,pOut,3,&iCountTotal);
	if(iCount2==0 && iCountTotal>0){_XLOG_
		// Все закрыто
		return;
	}
	if(iCount2>0){_XLOG_
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
	}
	AppendPluginsToMenu(pOut,0xFFFFFFFF,WKITEMPOSITION_MACRO);
	AddMenuString(pOut,ID_SYSTRAY_ADDMACROS,_l("Edit macro"),_bmp().Get(_IL(70)));
	AddMenuString(pOut,ID_SYSTRAY_RECMACRO,_l("Record a macro"),_bmp().Get(_IL(83)));
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		TUNE_UP_ITEM2(*pOut,"Right-click: more options");
		AddMenuSubmenu(pMenu,pOut,_l("Macros")+"\t...",_bmp().Get(_IL(70)));//11
	}
}

//#include <winuser.h>
void AppendQRunToMenu(CMenu* pMenu, DWORD dwMenuMask)
{_XLOG_
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		if(objSettings.bQRunMenu==3 || (objSettings.bQRunMenu==1 && (dwMenuMask & WKITEMPOSITION_TRAY)!=0)
			|| (objSettings.bQRunMenu==2 && (dwMenuMask & WKITEMPOSITION_HOTMENU)!=0)){_XLOG_
			return;
		}
	}
	// Очищаем пути для экспандов
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->aExpandedFolderPaths.RemoveAll();
	}
	// Создаем меню
	CMenu pQRun;
	CMenu* pOut=&pQRun;
	if(dwMenuMask & WKITEMPOSITION_DBLHOTMENU){_XLOG_
		pOut=pMenu;
	}else{_XLOG_
		pQRun.CreatePopupMenu();
	}
	MYMENUINFO cmi;
	cmi.cbSize=sizeof(cmi);
	cmi.fMask=MIM_STYLE|MIM_APPLYTOSUBMENUS;
	cmi.dwStyle=MNS_DRAGDROP;
	MySetMenuInfo(pOut->GetSafeHmenu(), &cmi);
	int iCount=0,iCount1=0,iCountTotal=0;
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	iCount1=AppendQRunSingleToMenu(pMenu,pOut,0,&iCountTotal);
	iCount+=iCount1;
	/*
	if(objSettings.DefActionMapHK[RUN_DEFBROWSER]!=0){_XLOG_
		CString sBrowsePath=getBrowserExePath();
		if(sBrowsePath!=""){_XLOG_
			iCount++;
			CString sBrIcon=GetExeIconIndex(sBrowsePath);
			AddMenuString(&pQRun,ID_SYSTRAY_RUNBROWSER,_l("Browser")+getActionHotKeyDsc(RUN_DEFBROWSER),sBrIcon!=""?_bmp().Get(sBrIcon):NULL);
		}
	}
	if(objSettings.DefActionMapHK[RUN_DEFEMAILCL]!=0){_XLOG_
		CString sEmailPath=getEmailExePath();
		if(sEmailPath!=""){_XLOG_
			iCount++;
			CString sMlIcon=GetExeIconIndex(sEmailPath);
			AddMenuString(&pQRun,ID_SYSTRAY_RUNEMAILCLIENT,_l("Email client")+getActionHotKeyDsc(RUN_DEFEMAILCL),sMlIcon!=""?_bmp().Get(sMlIcon):NULL);
		}
	}
	*/
	AppendPluginsToMenu(pOut,dwMenuMask,WKITEMPOSITION_QRUN);
	if(pOut->GetMenuItemCount()==0 && iCountTotal>0){_XLOG_
		// Все закрыто
		return;
	}
	if(iCount>0){_XLOG_
		pOut->AppendMenu(MF_SEPARATOR, 0, "");
	}
	AddMenuString(pOut,ID_SYSTRAY_ADDQRUNAPPWND,_l("Add application"),_bmp().Get(_IL(32)));
	AddMenuString(pOut,ID_SYSTRAY_RECMACRO,_l("Record a macro"),_bmp().Get(_IL(83)));
	if(!(dwMenuMask & WKITEMPOSITION_DBLHOTMENU)){_XLOG_
		TUNE_UP_ITEM2(*pOut,"Right-click: more options");
		AddMenuSubmenu(pMenu,pOut,_l("Quick run")+"\t...",_bmp().Get(_IL(32)));
	}
}

BOOL AppendPluginSubmenu(CMenu* pMenu, DWORD dwMenuMask)
{_XLOG_
	BOOL bRes=0;
	CMenu pPlugRun;
	pPlugRun.CreatePopupMenu();
	AppendPluginsToMenu(&pPlugRun,dwMenuMask,WKITEMPOSITION_PLUGIN);
	if(pPlugRun.GetMenuItemCount()>0){_XLOG_
		//pPlugRun.AppendMenu(MF_SEPARATOR, 0, "");
		//AddMenuString(&pPlugRun,ID_SYSTRAY_PLUGINOPTIONS,_l("Plugins options"),_bmp().Get(IDB_BM_OPTIONS));
		AddMenuSubmenu(pMenu,&pPlugRun,_l("Plugins")+"\t...",_bmp().Get(_IL(35)));
		bRes=TRUE;
	}
	//Кастом подпункты
	{//int WINAPI WKTrayMenuPopup(int iActionType, int iItemId, HMENU hMenu)
		int iItemId=WM_USER+PLUGINTP_OFFSET;
		for(int i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
			CPlugin* oPlug=objSettings.plugins[i];
			if(oPlug){_XLOG_
				if(oPlug && oPlug->bIsActive){_XLOG_
					oPlug->lTrayPopupItemID_Start=iItemId;
					CallPluginTPFunction(oPlug, TM_ADDCUSTOMITEM, iItemId, pMenu->GetSafeHmenu());
					oPlug->lTrayPopupItemID_End=iItemId;
				}
			}
		}
	}
	return bRes;
}

CCriticalSection csRefresher;
CMenu* _stdcall RefreshMenu(void* pData)
{_XLOG_
	__FLOWLOG;
	if(objSettings.lPreventTrayMenu>0){_XLOG_
		return 0;
	}
	// Повышем приоритет
	HANDLE hThisProc=::OpenProcess(PROCESS_SET_INFORMATION,FALSE,GetCurrentProcessId());
	if (!( (hThisProc) == NULL || (hThisProc) == INVALID_HANDLE_VALUE )){_XLOG_
		SetPriorityClass(hThisProc,HIGH_PRIORITY_CLASS);
	}
	CSmartLock lc(&csRefresher,TRUE);
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->m_STray.iTrayMenuOpened++;
	}
	CMenu* m_pMenu=new CMenu();
	m_pMenu->CreatePopupMenu();
	{_XLOG_
		SetWaitTrayIcon trWait(pMainDialogWindow?&(pMainDialogWindow->m_STray):NULL);
		if(objSettings.aHidedWindowsMap.GetSize()>0){// Hidden windows
			AppendHiddenToMenu(m_pMenu);
		}
		if(objSettings.bSysActionsInHotMenu==0 || objSettings.bSysActionsInHotMenu==2){_XLOG_
			CString sProcessIcon;
			DWORD dwProcID=-1;
			CMenu ProcMenu;
			ProcMenu.CreatePopupMenu();
			AppendProcessesToMenu(ProcMenu,sProcessIcon,dwProcID);
			AddMenuSubmenu(m_pMenu,&ProcMenu,_l("Processes")+"\t...",_bmp().Get(_IL(33)));
		}
		AppendMacrosToMenu(m_pMenu,WKITEMPOSITION_TRAY);
		AppendMiscSaverToMenu(m_pMenu,WKITEMPOSITION_TRAY);
		AppendDeskactionsToMenu(m_pMenu,WKITEMPOSITION_TRAY);
		AppendVolumeToMenu(m_pMenu,WKITEMPOSITION_TRAY);
		{
			CMenu pHelp;
			pHelp.CreatePopupMenu();
			AddMenuString(&pHelp,ID_SYSTRAY_HELP,_l("Help topics"),_bmp().Get(IDB_BM_HELP));
			AddMenuString(&pHelp,ID_SYSTRAY_ABOUT,_l("About program"),_bmp().Get(IDB_BM_ABPRG));
			AddMenuString(&pHelp,ID_SYSTRAY_CURRENTKEYBINDINGS,_l("List of all hotkeys"),_bmp().Get(_IL(55)));
			pHelp.AppendMenu(MF_SEPARATOR, 0, "");
			//if(!bPluginMenu){AddMenuString(&pHelp,ID_SYSTRAY_PLUGINOPTIONS,_l("Installed plugins"),_bmp().Get(_IL(35)));}
			AddMenuString(&pHelp,ID_SYSTRAY_MOREPLUGINSANDMACROS,_l("More plugins and macros"),_bmp().Get(_IL(35)));
			AddMenuString(&pHelp,ID_SYSTRAY_CHECKFORUPDATES,_l("Check for update"),_bmp().Get(_IL(22)),FALSE,NULL,IsUpdateInProgress());
			AddMenuString(&pHelp,ID_SYSTRAY_SENDASUGGESTION,_l("Support by Email"),_bmp().Get(_IL(44)));
			AddMenuString(&pHelp,ID_SYSTRAY_RESTARTWK,_l("Restart "PROGNAME),_bmp().Get(_IL(40)));
			AddMenuSubmenu(m_pMenu,&pHelp,_l("Help")+"\t...",_bmp().Get(IDB_BM_HELP));
		}
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		AppendQRunToMenu(m_pMenu,WKITEMPOSITION_TRAY);
		AddMenuString(m_pMenu,ID_SYSTRAY_OPTIONS2,_l("Quick-runs/Macros"),_bmp().Get(_IL(54)));
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		BOOL bPluginMenu=AppendPluginSubmenu(m_pMenu, WKITEMPOSITION_TRAY);
		AddMenuString(m_pMenu,ID_SYSTRAY_PLUGINOPTIONS,_l("Plugins options"),_bmp().Get(_IL(54)));
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(m_pMenu,ID_SYSTRAY_OPTIONS,_l("Open "PROGNAME)+getActionHotKeyDsc(SHOW_OPTIONS),_bmp().Get(IDB_BM_OPTIONS));//_IL(54)
#ifndef GPL
		if(objSettings.iLikStatus<2)
		{_XLOG_
#if VERSION_MAJOR>1
			AddMenuString(m_pMenu,ID_SYSTRAY_DONATE,_l("Order now")+"!",_bmp().Get(_IL(9)));
			AddMenuString(m_pMenu,ID_SYSTRAY_ENTERKEY,_l("Enter key"),_bmp().Get(_IL(12)));
#else
			AddMenuString(m_pMenu,ID_SYSTRAY_DONATE,_l("Donate")+"!",_bmp().Get(_IL(9)));
#endif
		}
#endif
		if(!WKUtils::isWKUpAndRunning()){_XLOG_
			AddMenuString(m_pMenu,0,_l("Warning: "PROGNAME" is paused"),0,0,0,1);
		}
		m_pMenu->SetDefaultItem(ID_SYSTRAY_OPTIONS,0);
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(m_pMenu,ID_SYSTRAY_EXIT,_l("Exit"),_bmp().Get(_IL(40)),FALSE,NULL,objSettings.bDoModalInProcess!=0);
	}
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->m_STray.iTrayMenuOpened--;
	}
	// Повышем приоритет
	if (!( (hThisProc) == NULL || (hThisProc) == INVALID_HANDLE_VALUE )){_XLOG_
		if(!objSettings.bAllowHighPriority){_XLOG_
			SetPriorityClass(hThisProc,NORMAL_PRIORITY_CLASS);
		}
		CloseHandle(hThisProc);
	}
	return m_pMenu;
}

AppMainDlg::AppMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(/*AppMainDlg::IDD, pParent*/), mixer(1)
{_XLOG_
	//{{AFX_DATA_INIT(AppMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	clipWnd=0;
	dwThreadId=0;
	iRunStatus=-1;
	lHotkeysState=0;
	pIconFont=NULL;
	bAnyPopupIsActive=FALSE;
	bAllLoaded=FALSE;
	bMainWindowInitialized=FALSE;
	bLastHalfer=FALSE;
	VolumeSplWnd=NULL;
	WinAmpSplWnd=NULL;
	dwVolSplashTimer=0;
	dwWinAmpSplashTimer=0;
}

AppMainDlg::~AppMainDlg()
{_XLOG_
	if(VolumeSplWnd){_XLOG_
		delete VolumeSplWnd;
		VolumeSplWnd=NULL;
	}
	if(WinAmpSplWnd){_XLOG_
		delete WinAmpSplWnd;
		WinAmpSplWnd=NULL;
	}
	if(pIconFont){_XLOG_
		delete pIconFont;
	}
}

HWND hTT=0;
DWORD dwLastTrayCPUTime=0;
DWORD WINAPI UpdateTrayCPU(LPVOID)
{_XLOG_
	static long lPreventer=0;
	if(lPreventer>0){_XLOG_
		return 0;
	}
	CPoint pt;
	GetCursorPos(&pt);
	SimpleTracker lc(lPreventer);
	while(pMainDialogWindow){_XLOG_
		Sleep(250);
		CString sNewT=pMainDialogWindow->SetTrayTooltip(1);
		if(hTT==0 || !IsWindow(hTT)){_XLOG_
			HWND hWnd1 = ::FindWindow("Shell_TrayWnd",NULL);
			if(hWnd1 == 0){_XLOG_
				return 0;
			}
			HWND hWnd3 = FindWindowEx(hWnd1,0,"TrayNotifyWnd",NULL); 
			if(hWnd3!=0){_XLOG_
				HWND hWnd4 = FindWindowEx(hWnd3,0,"SysPager",NULL); 
				HWND hWnd5 = FindWindowEx((hWnd4 !=0)?hWnd4:hWnd3,0,"ToolbarWindow32",NULL); 
				if(hWnd5 !=0){_XLOG_
					hTT=(HWND)SendMessage(hWnd5,TB_GETTOOLTIPS,0,0);
				}
			}
		}
		if(IsWindowVisible(hTT)){_XLOG_
			//static int iii=0;
			//SetWindowText(hTT,Format("%i %s",iii++,sNewT));
			RedrawWindow(hTT,0,0,RDW_ERASE|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
		}
		Sleep(250);
		CPoint pt2;
		GetCursorPos(&pt2);
		if(abs(pt.x-pt2.x)>40 || abs(pt.y-pt2.y)>40){_XLOG_
			return 0;
		}
	}
	return 0;
}

void WINAPI MouseOverTray()
{_XLOG_
	if(!objSettings.bShowCPUinTT || !pMainDialogWindow){_XLOG_
		return;
	}
	if(dwLastTrayCPUTime!=0 && GetTickCount()-dwLastTrayCPUTime<500){_XLOG_
		return;
	}
	dwLastTrayCPUTime=GetTickCount();
	FORK(UpdateTrayCPU,0);
}

BOOL AppMainDlg::OnInitDialog()
{_XLOG_
	DEBUG_INFO2("WK Enabled",0,0);
	dwThreadId=GetCurrentThreadId();
	CSystemTray::bShowErrors=FALSE;
	srand(time(NULL)+::GetCurrentThreadId());
	CDialog::OnInitDialog();
	mixer.SetWndForOpen(this->GetSafeHwnd());
	if(objSettings.lVolumeAfterStart){_XLOG_
		mixer.SetVolumeCtrlValue((UINT)(double(MAX_MIXER_VOL)*double(objSettings.lVolumeAfterStartLevel)/double(100)));
	}
	//------------------------------------------
	SetWindowText(Format("%s %s",PROGNAME, PROG_VERSION));// Не менять! Для шорткатов
	Sizer.InitSizer(this,objSettings.iStickPix,0,0);//HKEY_CURRENT_USER,PROG_REGKEY"\\MainWindowPos"
	CRect minRect(0,0,DIALOG_MINXSIZE,DIALOG_MINYSIZE);
	Sizer.SetMinRect(minRect);
	Sizer.SetOptions(STICKABLE);
	HICON hTrayIcon=::AfxGetApp()->LoadIcon(IDR_MAINICOX);
	m_STray.Create(NULL, WM_USER, objSettings.sMainWindowTitle, hTrayIcon,IDR_SYSTRAY,this, IsIconMustBeHided());
	m_STray.SetMenuDefaultItem(ID_SYSTRAY_OPTIONS, FALSE);
	m_STray.SetMenuDefaultItem2(TRAYICO_DBCL);
	m_STray.SetMenuFlags(0,ADDFLAGS_DLBHAND);
	m_STray.SetUserMenu(&RefreshMenu,MouseOverTray);
	m_STray.hActiveWndsExceptions=GetSafeHwnd();
	//m_Logo.SetBitmap(IDB_BM_LOGO);
	//m_Logo.SetStyles(CBS_NONSQR);
	SetWindowIcon(2);
	SetTaskbarButton(this->GetSafeHwnd());
	RegisterMyHotKeys(TRUE);
	if(objSettings.lSaveClipContent){_XLOG_
		BOOL bThroughGlobal=0;
		BOOL bDel=0;
		WCHAR* sOut=0;
		if(IsBMP(objSettings.sSavedClipContent)){_XLOG_
			sOut=((WCHAR*)(const char*)objSettings.sSavedClipContent);
		}else{_XLOG_
			int len=strlen(objSettings.sSavedClipContent);
			sOut = new WCHAR[len+1];
			memset(sOut,0,(len+1)*sizeof(WCHAR));
			MultiByteToWideChar(CP_ACP, 0, objSettings.sSavedClipContent, -1, sOut, len);
			bDel=1;
		}
		SetClipboardText(sOut,bThroughGlobal);
		if(bDel){_XLOG_
			delete[] sOut;
		}
		// Сбрасываем
		objSettings.sSavedClipContent="";
	}
	AttachToClipboard(TRUE);
	{// Запускаем автозапускаемые программы - до iRunStatus, чтобы не автоподнимались они...
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			if(objSettings.aQuickRunApps[i]){//&& DWORD(objSettings.aQuickRunApps[i]->lMacros)==0 && objSettings.aQuickRunApps[i]->lUseAdditionalOptions
				if(objSettings.aQuickRunApps[i]->lUseDetachedPresentation){_XLOG_
					PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+i,QRUN_DETACHTO);
				}
				if(objSettings.aQuickRunApps[i]->lStartAtStartUp){_XLOG_
					PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+i,QRUN_ONWKSTART|QRUN_NOFOREGR|QRUN_BYHOTKEY);
				}
			}
		}
	}
	// Шрифт для текста на иконке
	pIconFont=GetFont();
	LOGFONT logf;
	pIconFont->GetLogFont(&logf);
	strcpy(logf.lfFaceName,"Fixedsys");
	logf.lfPitchAndFamily=0;//FIXED_PITCH|FF_MODERN;
	logf.lfWeight=200;
	logf.lfHeight=-30;
	logf.lfWidth=0;
	pIconFont=new CFont();
	pIconFont->CreateFontIndirect(&logf);
	// Обновляем информацию...
	SetRegInfo();
	// Говорим всем что прога запустилась
	iRunStatus=1;
	SetTrayTooltip();
	DWORD dwTarget=BSM_APPLICATIONS;
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(0), LPARAM(0));
	if(objSettings.bUnderWindowsNT){_XLOG_
		HANDLE hProcess=::OpenProcess(PROCESS_SET_QUOTA,FALSE,GetCurrentProcessId());
		if(hProcess){_XLOG_
			//::SetProcessWorkingSetSize(hProcess, -1, -1);
			::SetProcessWorkingSetSize(hProcess, 1*1024*1024, -1);
			CloseHandle(hProcess);
		}
	}
	if(0){_XLOG_
		// Обновляем информацию загодя
		CMenu* pTemp=RefreshMenu(NULL);
		delete pTemp;
	}
	// Ставим слепую метку!
	::SetProp(GetSafeHwnd(),"WKP_INVIS",(LPVOID)1);
	//::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)objSettings.hActiveApplicationAtStart);
	if(objSettings.lShowDateInTrayIcon==3){_XLOG_
		PostMessage(LANG_CHANGE,0,0);
	}
	if(objSettings.sLikKey.GetLength()>20){_XLOG_
		Alert(_l("Warning: You should upgrade your registration key in order to use "PROGNAME"!\n\nPlease, write an email to support@wiredplane.com and ask for new key.\nThis is absoluetly FREE for registered users.\nWe are forced to reissue keys because of hackers activity")+"\n"+_l("Sorry for inconvinience"), _l("New registrations keys."));
	}
	SetWindowIcon();
	char szWkmRegistered[32]={0};
	GetRegSetting("", "wkmRegistered", szWkmRegistered, sizeof(szWkmRegistered));
	if(szWkmRegistered[0]==0){_XLOG_
		strcpy(szWkmRegistered,"yes");
		SetRegSetting("", "wkmRegistered", szWkmRegistered);
		RegisterExtension("wkm","WireKeys Macros","-add=");
	}
	HWND hMain=::FindWindow("WK_MAIN",0);
	if(hMain){_XLOG_
		SetProp(hMain,"MAIN_WND",GetSafeHwnd());
	}
	return FALSE;
}

#define HIST_CHAR	'!'
void AppMainDlg::SetWindowIcon(int iIconType, BOOL bForce)
{_XLOG_
	if(bHideIcon==1){_XLOG_
		objSettings.lDisableSounds=1;
		objSettings.lEverythingIsPaused=1;
		m_STray.HideIcon();
		return;
	}
	if(bForce==0 && (m_STray.iTrayMenuOpened>0 || !lHotkeysState)){_XLOG_
		return;
	}
	HICON hIcon=0;
	if(iIconType==0){_XLOG_
		if(objSettings.lEverythingIsPaused){_XLOG_
			iIconType=2;
		}else{_XLOG_
			if(objSettings.sUDIcon!=""){_XLOG_
				if(objSettings.hUDIcon==0){_XLOG_
					objSettings.hUDIcon=(HICON)::LoadImage(NULL,objSettings.sUDIcon,IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
				}
				hIcon=::CopyIcon(objSettings.hUDIcon);
			}
			if(hIcon==0){_XLOG_
				hIcon=::AfxGetApp()->LoadIcon(IDR_MAINICO);
				objSettings.sUDIcon="";
			}
		}
	}
	if(iIconType==1){_XLOG_
		hIcon=::AfxGetApp()->LoadIcon(IDI_PASTE);
	}
	if(iIconType==2){_XLOG_
		hIcon=::AfxGetApp()->LoadIcon(IDR_MAINICOX);
	}
	if(iIconType==3){_XLOG_
		hIcon=::AfxGetApp()->LoadIcon(IDI_PASTE);
	}
	HICON hIcoText=NULL;
	// CPU Usage
	int iPrc=0;
	if(objSettings.lShowCPUInTrayIcon){_XLOG_
		double dCPUUsage=0;
		if(objSettings.lShowCPUInTrayIcon==1){_XLOG_
			dCPUUsage=GetCPUTimesPercents();
		}
		if(objSettings.lShowCPUInTrayIcon==2){_XLOG_
			dCPUUsage=GetMemoryPercents();
		}
		iPrc=int(double(GetIconSize(hIcon).cy)*dCPUUsage/100);
		if(iPrc>0 || objSettings.lShowCPUHistInTray){_XLOG_
			HICON hIcon2=::AfxGetApp()->LoadIcon(IDI_ICON_CPU), hIcon3=NULL;
			/*if(!objSettings.lShowCPUHistInTray){_XLOG_
				hIcon3=PutOverlayInIconSz(hIcoText?hIcoText:hIcon,hIcon2,_bmp().MenuIconSize.cx,_bmp().MenuIconSize.cy,-1,iPrc,TRUE);
			}else
			*/
			{_XLOG_
				DWORD dwXBmpSize=GetIconSize(hIcon).cx;
				DWORD dwYBmpSize=GetIconSize(hIcon).cy;
				// Рисуем гистограмму
				static CArray<int,int&> aHistogrammData;
				aHistogrammData.Add(iPrc);
				if(aHistogrammData.GetSize()>objSettings.lShowCPUHistInTrayC){_XLOG_
					aHistogrammData.RemoveAt(0);
				}
				CDC dc;
				CBitmap bmp;
				CBrush IconBr(RGB(255,255,255));
				CDesktopDC dcDesk;
				dc.CreateCompatibleDC(&dcDesk);
				bmp.CreateCompatibleBitmap(&dcDesk, dwXBmpSize, dwYBmpSize);
				CBitmap* pOld=dc.SelectObject(&bmp);
				::DrawIconEx(dc,0,0,hIcoText?hIcoText:hIcon,dwXBmpSize,dwYBmpSize,0,IconBr,DI_NORMAL);
				DWORD dXSize=(dwXBmpSize/aHistogrammData.GetSize());
				if(!objSettings.lShowCPUHistInTray){_XLOG_
					::DrawIconEx(dc,0,dwYBmpSize-iPrc,hIcon2,dwXBmpSize,iPrc,0,NULL,DI_NORMAL);
				}else{_XLOG_
					for(int i=0;i<aHistogrammData.GetSize();i++){_XLOG_
						iPrc=aHistogrammData[i];
						::DrawIconEx(dc,i*dXSize,dwYBmpSize-iPrc,hIcon2,dXSize,iPrc,0,NULL,DI_NORMAL);
					}
				}
				hIcon3=CreateIconFromBitmap(&bmp,NULL,FALSE);
				if(pOld){_XLOG_
					dc.SelectObject(pOld);
				}
			}
			ClearIcon(hIcon2);
			ClearIcon(hIcon);
			hIcon=hIcon3;
		}
	}
	if(objSettings.lShowDateInTrayIcon>0){_XLOG_
		// Date
		CString sText="";
		static const char* sDays[]={"Su","Mn","Tu","We","Th","Fr","Sa"};
		COleDateTime dt=COleDateTime::GetCurrentTime();
		if(objSettings.lShowDateInTrayIcon==1){_XLOG_
			sText=Format("%02i",dt.GetDay());
			hIcoText=PutTextInIcon(hIcon,sText,pIconFont,RGB(250,250,250),RGB(0,0,0),TINI_FONTEXACT);
		}
		if(objSettings.lShowDateInTrayIcon==2){_XLOG_
			DWORD dwDay=dt.GetDayOfWeek();
			sText=_l(Format("Day%iShort",dwDay),sDays[dwDay-1]);
			hIcoText=PutTextInIcon(hIcon,sText,pIconFont,RGB(250,250,250),RGB(0,0,0),TINI_FONTEXACT);
		}
		if(objSettings.lShowDateInTrayIcon==3){_XLOG_
			sText=objSettings.sLastStringForTrayIcons;
			hIcoText=PutTextInIcon(hIcon,sText,pIconFont,RGB(250,250,250),RGB(0,0,0),TINI_FONTEXACT);
		}
	}
	if(hIcoText==NULL){_XLOG_
		hIcoText=hIcon;
	}else{_XLOG_
		ClearIcon(hIcon);
	}
	m_STray.SetIcon(hIcoText,TRUE);
	SetIcon(hIcoText,TRUE);
	SetIcon(hIcoText,FALSE);
	theApp.m_pFakeWnd->SetIcon(hIcoText,TRUE);
	theApp.m_pFakeWnd->SetIcon(hIcoText,FALSE);
}

void AppMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{_XLOG_
	if(nID==SC_MINIMIZE || nID==SC_CLOSE){_XLOG_
		ShowWindow(SW_HIDE);
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void AppMainDlg::OnSystrayExit() 
{_XLOG_
	if(spyMode()){
		bHideIcon=1;
		SetWindowIcon();
		return;
	}
	Finalize();
}

void AppMainDlg::OnSystrayOpen() 
{_XLOG_
	if(spyMode()){
		bHideIcon=0;
		objSettings.lDisableSounds=0;
		objSettings.lEverythingIsPaused=0;
		SetWindowIcon();
		//m_STray.HideIcon();
	}
	SwitchToWindow(this->GetSafeHwnd());
	SetForegroundWindow();
	ShowWindow(SW_SHOW);
}

void AppMainDlg::OnDestroy() 
{_XLOG_
	// Говорим всем что прога останавливается
	DWORD dwTarget=BSM_APPLICATIONS;
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(1), LPARAM(0));
	objSettings.Finalize();
	AttachToClipboard(FALSE);
	CDialog::OnDestroy();
}

CPoint GetCaretScreenPos(BOOL bUseCashe)
{_XLOG_
	static POINT pt;
	if(bUseCashe){_XLOG_
		static DWORD dwTime=0; 
		if(dwTime!=0 && GetTickCount()-dwTime<5000){_XLOG_
			return pt;
		}
		dwTime=GetTickCount();
	}
	memset(&pt,0,sizeof(pt));
	BOOL bUseWMCOPY=FALSE;
	PrepareClipProcesses(bUseWMCOPY);
	if(objSettings.hFocusWnd==NULL || !GetCaretPos(&pt) || (pt.x==0 && pt.y==0)){_XLOG_
		::GetCursorPos(&pt);
	}else{_XLOG_
		::ClientToScreen(objSettings.hFocusWnd,&pt);
	}
	FinishClipProcesses();
	return pt;
}

DWORD WINAPI Blinker(LPVOID pData)
{_XLOG_
	TRACETHREAD;
	static long lLock=0;
	if(lLock>0){// Уже что-то считается...
		return -1;
	}
	SimpleTracker tr(lLock);
	CSysCurBlinker bl(pData==NULL?0:(UINT)pData,200);
	return 0;
}

CString InsertAndPinOrBubbleHistory(const char* szHistName, long iClipNumber, BOOL bPaste=TRUE, BOOL bForcePin=0)
{_XLOG_
	CPairItem sClipPart;
	BOOL bNeedAddToPinned=FALSE;
	if(bForcePin || GetAsyncKeyState(VK_SHIFT)<0){// Добавляем item к pinned up
		bNeedAddToPinned=TRUE;
	}
	if(bForcePin==-1){
		objSettings.sClipHistory[szHistName].RemoveAt(iClipNumber);
		if(iClipNumber<objSettings.lClipHistoryDepth && objSettings.sClipHistory[szHistName].GetSize()>=objSettings.lClipHistoryDepth){
			CPairItem item("","","");
			objSettings.sClipHistory[szHistName].InsertAt(objSettings.lClipHistoryDepth-1,item);
		}
		return "";
	}
	if(bNeedAddToPinned==TRUE && iClipNumber>=objSettings.lClipHistoryDepth){_XLOG_
		// Удаляем со старого места
		objSettings.sClipHistory[szHistName].RemoveAt(iClipNumber);
	}else{_XLOG_
		sClipPart=objSettings.sClipHistory[szHistName][iClipNumber];
		if(bPaste){_XLOG_
			pMainDialogWindow->PostMessage(WM_COMMAND,PASTETEXT,(LPARAM)strdup(sClipPart.sItem));
		}
		if(bNeedAddToPinned==TRUE){_XLOG_
			ClipAddTextToPinned(sClipPart.sItem,szHistName,TRUE, sClipPart.sClipItemDsc,sClipPart.sApp);
		}else if(iClipNumber<objSettings.lClipHistoryDepth){_XLOG_
			// Всплывание
			objSettings.sClipHistory[szHistName].RemoveAt(iClipNumber);
			objSettings.sClipHistory[szHistName].InsertAt(0,sClipPart);
		}
	}
	return sClipPart.sItem;
}

DWORD WINAPI ClipboardOperations(LPVOID pData)
{_XLOG_
	TRACETHREAD;
	static long lLock=0;
	if(lLock>0){// Уже что-то делается...
		return -1;
	}
	SimpleTracker tr(lLock);
	SetCurrentDesktop();
	//------------------------
	WORD iAction=HIWORD(pData);
	WORD iClip=LOWORD(pData);
	if(iAction==0){_XLOG_
		FORK(Blinker,objSettings.lNoNotifyByCursor?0:IDC_CPASTE);
		SimpleLocker l(&csClipBuffer);
		if(iClip>=0 && iClip<objSettings.sClipBuffer.GetSize() && objSettings.sClipBuffer[iClip].GetLength()!=0){_XLOG_
			pMainDialogWindow->PostMessage(WM_COMMAND,PASTETEXT,(LPARAM)strdup(objSettings.sClipBuffer[iClip]));
		}
	}else if(iAction==WORD(-1)){_XLOG_
		FORK(Blinker,objSettings.lNoNotifyByCursor?0:IDC_CCOPY);
		CString sText=GetSelectedTextInFocusWnd();
		if(SetTextToClip(iClip,sText)){_XLOG_
			AsyncPlaySound(CLIP_COPY);
		}
		if(objSettings.lClipTransmitMode){_XLOG_
			CString sTextToAdd;
			if(objSettings.lAppendTextToTransmits){_XLOG_
				CString sAppTxt=objSettings.sTransmitAppendText;
				SubstMyVariablesWK(sAppTxt);
				sTextToAdd+="\r\n";
				sTextToAdd+=sAppTxt;
				sTextToAdd+="\r\n";
			}
			sTextToAdd+=sText;
			sTextToAdd+="\r\n";
			FORK(TransmitTextToTarget, strdup(sTextToAdd));
		}
	}
	::SetForegroundWindow(objSettings.hHotMenuWnd);
	return 0;
}

void SetMainWindowForeground(HWND hWin, HWND hWindowToSetForeground, HWND* hWindowToSetFocus)
{_XLOG_
	if(pMainDialogWindow){_XLOG_
		BOOL bNeedToGetFocus=FALSE;
		if(hWindowToSetForeground==NULL){_XLOG_
			bNeedToGetFocus=TRUE;
			hWindowToSetForeground=pMainDialogWindow->GetSafeHwnd();
		}
		if(!hWindowToSetForeground || !IsWindow(hWindowToSetForeground)){_XLOG_
			return;
		}
		DWORD dwCurWinProcID=0;
		DWORD dwCurWinThreadID=0;
		DWORD dwThisThread=GetCurrentThreadId();
		DWORD dwThisProcess=GetCurrentProcessId();
		if(hWin && IsWindow(hWin)){_XLOG_
			// Чтобы меню закрывалось по клику на свободном месте десктопа...
			dwCurWinThreadID=GetWindowThreadProcessId(hWin,&dwCurWinProcID);
			AttachThreadInput(dwThisThread,dwCurWinThreadID,TRUE);
		}
		if(hWindowToSetFocus!=NULL){_XLOG_
			if(bNeedToGetFocus){_XLOG_
				*hWindowToSetFocus=::GetFocus();
				::SetForegroundWindow(hWindowToSetForeground);
			}else{_XLOG_
				//::SetForegroundWindow(hWindowToSetForeground);
				::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hWindowToSetForeground);
				::SetFocus(*hWindowToSetFocus);
			}
		}else{_XLOG_
			::SetForegroundWindow(hWindowToSetForeground);
		}
		if(dwCurWinThreadID!=0){_XLOG_
			AttachThreadInput(dwThisThread,dwCurWinThreadID,FALSE);
		}
	}
}

HMENU g_hClipHistoryMenu=NULL;
HMENU g_hClipHistoryMenu2=NULL;
CString g_sOpenedHistoryMenuName="";
void EditClip(CString szHistName,int lEditClipNumber);
BOOL AddHistItemToMenu(CMenu& menu,int i, const char* szHistName,int& iPinnedCount,int& iSimpleCount, int iCodeOffset, CString sFilter)
{_XLOG_
	if(sFilter!=""){_XLOG_
		if(objSettings.sClipHistory[szHistName][i].sApp.Find(sFilter)==-1){_XLOG_
			return FALSE;
		}
	}
	BOOL bRes=1;
	BOOL bPinnedPieces=FALSE;
	CString sQAccesItem="";
	if(i>=objSettings.lClipHistoryDepth){_XLOG_
		bPinnedPieces=TRUE;
		iPinnedCount++;
		if(iPinnedCount<=9 && iCodeOffset==0){_XLOG_
			sQAccesItem=Format("\t&%i",iPinnedCount);
		}
		
	}else{_XLOG_
		if(iCodeOffset==0 && 'A'+iSimpleCount<='Z'){_XLOG_
			sQAccesItem=Format("\t&%c",'A'+iSimpleCount);
		}
		iSimpleCount++;
	}
	if(objSettings.sClipHistory[szHistName][i].sItem!=""){_XLOG_
		CString sText="";
		if(objSettings.bUnderWindows98){_XLOG_
			sQAccesItem.TrimLeft("\t");
			sText=sQAccesItem+") "+trimTextForMenu(objSettings.sClipHistory[szHistName][i].sItem);
		}else{_XLOG_
			sText=trimTextForMenu(objSettings.sClipHistory[szHistName][i].sItem)+sQAccesItem;
		}
		CString sDscicon=GetExeIconIndex(objSettings.sClipHistory[szHistName][i].sApp);
		CBitmap* bmp=bPinnedPieces?_bmp().Get(IDB_PINNED_UP):_bmp().Get(sDscicon);
		if(bmp==0){_XLOG_
			bmp=_bmp().Get(IDB_PINNED_DOWN);
		}
		if(IsBMP(objSettings.sClipHistory[szHistName][i].sItem)){_XLOG_
			bRes=1;// Разрешаем картинки для редактирования...
			const char* szBMP=objSettings.sClipHistory[szHistName][i].sItem;
			CString sBMPHash=CDataXMLSaver::GetInstringPart(BMP_SPECPREFIX"[","]",szBMP);
			CString sBMPTName=CString("CLIPDIB")+sBMPHash;
			bmp=_bmp().Get(sBMPTName);
			if(bmp==0){_XLOG_
				// Добавляем
				DWORD dwSize=atol(sBMPHash);
				if(dwSize>0){_XLOG_
					const char* szBMPData=strchr(szBMP,']');
					if(szBMPData){_XLOG_
						HGLOBAL hClipBMP=0;
						PackFromString(szBMPData+1,dwSize,hClipBMP,(!isWin9x()?GMEM_MOVEABLE:GMEM_FIXED)|GMEM_ZEROINIT);
						LPBITMAPINFO lpBI = (LPBITMAPINFO)GlobalLock(hClipBMP);
						if(lpBI){_XLOG_
							CDesktopDC dc;
							CBitmap bmpThumb;
							void* pDIBBits=(void*)(lpBI + 1); 
							HBITMAP hBitmap = CreateDIBitmap(dc, &lpBI->bmiHeader, CBM_INIT, pDIBBits, lpBI, DIB_RGB_COLORS);
							bmpThumb.Attach(hBitmap);
							_bmp().AddBmp(sBMPTName,&bmpThumb,_bmp().MenuIconSize);
							GlobalUnlock(hClipBMP);
							GlobalFree(hClipBMP);
							bmp=_bmp().Get(sBMPTName);
						}
					}
				}
			}
		}
		AddMenuString(&menu,i+1+iCodeOffset,sText,bmp);
	}
	return bRes;
}

#include "DLG_EditMLine.h"
BOOL CallBMPEdit(CString sPictureFile);
BOOL SaveBitmapToFile(CString& sPictureFile, CBitmap* bmpResult);
DWORD WINAPI ShowClipHistoryMenu(const char* szHistName,const char* szHistTitle, CString* sOut, BOOL bFilterApp)
{_XLOG_
	static long lLock=0;
	if(lLock>0){// Уже что-то считается...
		return -1;
	}
	g_sOpenedHistoryMenuName=szHistName;
	SetCurrentDesktop();
	SimpleTracker tr(lLock);
	SimpleTracker tr2(objSettings.lHotMenuOpened);
	CStringArrayX tmp;
	if(!objSettings.sClipHistory.Lookup(szHistName,tmp)){_XLOG_
		objSettings.sClipHistory.SetAt(szHistName,tmp);
	}
	CString sFilter="";
	if(bFilterApp){_XLOG_
		sFilter=GetPathPart(GetExeFromHwnd(objSettings.hHotMenuWnd),0,0,1,1);
	}
	CMenu menu;
	menu.CreatePopupMenu();
	//CMenu menuEdit;
	//menuEdit.CreatePopupMenu();
	AddMenuString(&menu,0,szHistTitle,_bmp().Get(_IL(18)));
	BOOL bPinnedPieces=FALSE;
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	long iHistLen=objSettings.sClipHistory[szHistName].GetSize();
	int iEditOffset=iHistLen+100,iDummy=0;
	if(iHistLen>0){_XLOG_
		int iPinnedCount=0,iSimpleCount=0;
		int iRealMenuCounts=0;
		if((iHistLen-1)>=objSettings.lClipHistoryDepth){_XLOG_
			for(long i=objSettings.lClipHistoryDepth;i<iHistLen;i++){_XLOG_
				if(AddHistItemToMenu(menu,i,szHistName,iPinnedCount,iSimpleCount,0,sFilter)){_XLOG_
					//AddHistItemToMenu(menuEdit,i,szHistName,iDummy,iDummy,iEditOffset,sFilter);
					iRealMenuCounts++;
				}
			}
			if(iPinnedCount>0){_XLOG_
				menu.AppendMenu(MF_SEPARATOR, 0, "");
			}
		}
		CMenu menuExtended;
		menuExtended.CreatePopupMenu();
		BOOL bUseExtended=0;
		int iMenuCounts=min(iHistLen,objSettings.lClipHistoryDepth);
		for(long i=0;i<iMenuCounts;i++){_XLOG_
			if(AddHistItemToMenu(bUseExtended?menuExtended:menu,i,szHistName,iPinnedCount,iSimpleCount,0,sFilter)){_XLOG_
				//AddHistItemToMenu(menuEdit,i,szHistName,iDummy,iDummy,iEditOffset,sFilter);
				iRealMenuCounts++;
			}
			if(iRealMenuCounts>20 && bUseExtended==0){_XLOG_
				bUseExtended=1;
			}
		}
		if(bUseExtended){_XLOG_
			g_hClipHistoryMenu2=menuExtended.GetSafeHmenu();
			AddMenuSubmenu(&menu,&menuExtended,_l("More items")+"\t...",_bmp().Get(_IL(18)));
		}
		if(iRealMenuCounts>0){_XLOG_
			menu.AppendMenu(MF_SEPARATOR, 0, "");
			/*if(menuEdit.GetMenuItemCount()>0){_XLOG_
				AddMenuSubmenu(&menu,&menuEdit,_l("Edit items")+"\t...",_bmp().Get(_IL(12)));
			}*/
			AddMenuString(&menu,ID_HOTMENU_CLEARCLIPS,_l("Clear this history"),_bmp().Get(_IL(37)));
		}else{_XLOG_
			AddMenuString(&menu,0,_l("History is empty"),NULL,FALSE,NULL,TRUE);
			menu.AppendMenu(MF_SEPARATOR, 0, "");
		}
		AddMenuString(&menu,ID_HOTMENU_EXITMENU,_l("Close menu"),_bmp().Get(_IL(40)));
		TUNE_UP_ITEM2(menu,"Right-click: more options");
	}else{_XLOG_
		AddMenuString(&menu,0,_l("History is empty"),NULL,FALSE,NULL,TRUE);
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&menu,ID_HOTMENU_EXITMENU,_l("Close menu"),_bmp().Get(_IL(40)));
	}
	CPoint pos=GetCaretScreenPos(0);
	menu.SetDefaultItem(0,TRUE);
	// Запоминаем активное окно...
	HWND hRealFocus=0;
	if(GetModuleHandle("wp_wtraits.wkp")){_XLOG_
		hRealFocus=GetLastFocusWindowByHook(objSettings.hHotMenuWnd);
/*#ifdef _DEBUG
		char s[100]={0};
		::GetWindowText(hRealFocus,s,sizeof(s));
		char szClass[128]="";
		::GetClassName(hRealFocus,szClass,sizeof(szClass));
		ShowBaloon(Format("Lost focus: 0x%08X %s",hRealFocus,szClass),"WireKeys debug",3000);
#endif*/
	}
	// И делаем активным текущее окно
	CString sNewTitle;
	GetActiveWindowTitle(sNewTitle, objSettings.ActiveWndRect);
	if(pMainDialogWindow && !objSettings.lDisableFocusMove){_XLOG_
		SetMainWindowForeground(objSettings.hHotMenuWnd,NULL,&objSettings.hFocusWnd);
	}
	g_hClipHistoryMenu=menu.GetSafeHmenu();
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->ttMenu.UpdateToolTipTitle(szHistTitle);
	}
	BOOL pAnim=FALSE;
	SystemParametersInfo(0x1002/*SPI_GETMENUANIMATION*/,0,&pAnim,0);
	if(pAnim){
		BOOL pNoAnim=0;
		SystemParametersInfo(0x1003/*SPI_SETMENUANIMATION*/,0,NULL,0);
	}
	DWORD dwRetCode=TrackPopupMenu(g_hClipHistoryMenu, TPM_RETURNCMD|TPM_RECURSE, pos.x, pos.y, 0, pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL, NULL);//TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RECURSE
	if(pAnim){//Включаем анимацию обратно
		SystemParametersInfo(0x1003/*SPI_SETMENUANIMATION*/,0,&pAnim,0);
	}
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->ttMenu.ShowToolTip(FALSE);
	}
	if(HandleTuneUpItem2(dwRetCode)){
		return 0;
	}
	if(hRealFocus && objSettings.hHotMenuWnd==0){_XLOG_
		objSettings.hHotMenuWnd=hRealFocus;
		objSettings.hFocusWnd=hRealFocus;
	}
	if(!sOut && objSettings.hHotMenuWnd){_XLOG_
		if(!objSettings.lDisableFocusMove || hRealFocus!=0){_XLOG_
			SetMainWindowForeground(objSettings.hHotMenuWnd,objSettings.hHotMenuWnd,&objSettings.hFocusWnd);
		}
	}
	if(pMainDialogWindow && dwRetCode==ID_HOTMENU_CLEARCLIPS){_XLOG_
		wk.ClearClipHistory(szHistName);
	}else{_XLOG_
		long iClipNumber=dwRetCode-1;
		if(dwRetCode!=0 && dwRetCode!=ID_HOTMENU_EXITMENU && iClipNumber>=0 && long(iClipNumber)<iHistLen){_XLOG_
			if(iClipNumber>=0 && iClipNumber<objSettings.sClipHistory[szHistName].GetSize()){_XLOG_
				if(sOut){_XLOG_
					*sOut=InsertAndPinOrBubbleHistory(szHistName,iClipNumber,FALSE);
				}else{_XLOG_
					InsertAndPinOrBubbleHistory(szHistName,iClipNumber,TRUE);
				}
			}
		}
		EditClip(szHistName,iClipNumber-iEditOffset);
	}
	return 0;
}

void EditClip(CString szHistName,int lEditClipNumber)
{
	long iHistLen=objSettings.sClipHistory[szHistName].GetSize();
	if(lEditClipNumber>=0 && long(lEditClipNumber)<iHistLen){_XLOG_
		if(IsBMP(objSettings.sClipHistory[szHistName][lEditClipNumber].sItem)){_XLOG_
			const char* szBMP=objSettings.sClipHistory[szHistName][lEditClipNumber].sItem;
			CString sBMPHash=CDataXMLSaver::GetInstringPart(BMP_SPECPREFIX"[","]",szBMP);
			// Получаем битмап
			DWORD dwSize=atol(sBMPHash);
			if(dwSize>0){_XLOG_
				const char* szBMPData=strchr(szBMP,']');
				if(szBMPData){_XLOG_
					HGLOBAL hClipBMP=0;
					PackFromString(szBMPData+1,dwSize,hClipBMP,(!isWin9x()?GMEM_MOVEABLE:GMEM_FIXED)|GMEM_ZEROINIT);
					LPBITMAPINFO lpBI = (LPBITMAPINFO)GlobalLock(hClipBMP);
					if(lpBI){_XLOG_
						CBitmap bmpThumb;
					void* pDIBBits=(void*)(lpBI + 1); 
					CDesktopDC dc;
					HBITMAP hBitmap = CreateDIBitmap(dc, &lpBI->bmiHeader, CBM_INIT, pDIBBits, lpBI, DIB_RGB_COLORS);
					bmpThumb.Attach(hBitmap);
					GlobalUnlock(hClipBMP);
					GlobalFree(hClipBMP);
					CString sPictureFile;
					CFileDialog dlg(FALSE, NULL, sPictureFile, OFN_NODEREFERENCELINKS, "All files (*.*)|*.*||", GetAppWnd());
						if(dlg.DoModal()==IDOK){_XLOG_
							sPictureFile=dlg.GetPathName();
							if(SaveBitmapToFile(sPictureFile, &bmpThumb)){_XLOG_
								// Вызываем редактирование
								CallBMPEdit(sPictureFile);
							}
						}
					}
				}
			}
		}else{
			//AfxMessageBox(objSettings.sClipHistory[szHistName][lEditClipNumber].sItem);
			CDLG_EditMLine dlg;
			dlg.sText=objSettings.sClipHistory[szHistName][lEditClipNumber].sItem;
			if(dlg.DoModal()==IDOK){_XLOG_
				objSettings.sClipHistory[szHistName][lEditClipNumber].sItem=dlg.sText;
			}
		}
	}
}

int CallPluginFunction(CPlugin* plg, int iNum, WKPluginFunctionStuff* stuff)
{_XLOG_
	if(!plg || !plg->fCallFunction){_XLOG_
		return 0;
	}
	int iRes=0;
	__try
	{_XLOG_
		iRes=(*plg->fCallFunction)(iNum, stuff);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		if(stuff){_XLOG_
			strcpy(stuff->szError,"Error: Exception occured!");
			strcat(stuff->szError," (");
			strcat(stuff->szError,plg->sTitle);
			strcat(stuff->szError,")");
		}
	}
	return iRes;
}

class CMsgData
{
public:
	CMsgData()
	{_XLOG_
		wParam=0;
		lParam=0;
	}
	WPARAM wParam;
	LPARAM lParam;
};

DWORD WINAPI PluginOperation2(LPVOID pData)
{_XLOG_
	CMsgData* msg=(CMsgData*)pData;
	DWORD dwOrigCode=msg->wParam;
	DWORD iHotKey=msg->lParam;
	delete msg;
	return PluginOperationX(dwOrigCode,iHotKey);
}

DWORD WINAPI PluginOperation(LPVOID pData)
{_XLOG_
	int iHotKey=(int)pData;
	return PluginOperationX(iHotKey,0);
}

DWORD WINAPI PluginOperationX(DWORD wParam,DWORD dwOrigCode)
{_XLOG_
	TRACETHREAD;
	SimpleTracker lc(objSettings.lInPlugAction);
	if(pMainDialogWindow!=NULL && pMainDialogWindow->iRunStatus==-2){_XLOG_
		return 0;
	}
	__FLOWLOG1("PluginOperationX");
	if(wParam<WM_USER){_XLOG_
		// Init operation - Do it whatever happens
		if(wParam<(DWORD)objSettings.plugins.GetSize() && objSettings.plugins[wParam]){_XLOG_
			while(pMainDialogWindow==0){_XLOG_
				// Ждем пока появится главное окно...
				Sleep(200);
			}
			if(pMainDialogWindow!=NULL && pMainDialogWindow->iRunStatus==-2){_XLOG_
				return 0;
			}
			CPlugin* plg=objSettings.plugins[wParam];
			if(plg->fInitFunction!=NULL){_XLOG_
#ifdef _DEBUG
				CString sPluginName=plg->sFile;
#endif
				(*plg->fInitFunction)((WKCallbackInterface*)plg);
			}
		}
		return 0;
	}
	static long lLock=0;
	SetCurrentDesktop();
	SimpleTracker tr(lLock);
	for(int i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
		CPlugin* plg=objSettings.plugins[i];
		if(!plg){_XLOG_
			continue;
		}
		if(!plg->bIsActive){_XLOG_
			continue;
		}
		CSmartLock* pSL=0;
		for(int j=0;j<(plg->funcs.GetSize());j++){_XLOG_
			if(plg->funcs[j].dwCommandCode == wParam){_XLOG_
				BOOL bStopOperation=0;
				if(!plg->bResident){_XLOG_
					pSL=new CSmartLock(&plg->csOTExecution,TRUE);
					// Грузим...
					if(LoadPluginRaw(plg)){_XLOG_
						// Задаем настройки...
						if(plg->fOptionsManager){_XLOG_
							(*plg->fOptionsManager)(OM_OPTIONS_SET,plg,0);
						}
					}else{_XLOG_
						bStopOperation=1;
					}
				}
				WKPluginFunctionActualDsc dsc;
				if(plg->fActualParamsFunction){_XLOG_
					(*plg->fActualParamsFunction)(j,&dsc);
				}
				if(!dsc.bDisabled && !dsc.bGrayedout && !bStopOperation){// но не !dsc.bVisible
					WKPluginFunctionStuff stuff;
					stuff.hCurrentFocusWnd=GetLastFocusWindowByHook(objSettings.hLastForegroundWndForPlugins);//objSettings.hFocusWnd
					stuff.hForegroundWindow=objSettings.hLastForegroundWndForPlugins;
					if(stuff.hForegroundWindow==0){_XLOG_
						stuff.hForegroundWindow=GetForegroundWindow();
					}
					if(stuff.hCurrentFocusWnd==0){_XLOG_
						stuff.hCurrentFocusWnd=stuff.hForegroundWindow;
					}
					stuff.wireKeysInt=(WKCallbackInterface*)(objSettings.plugins[i]);
					//DWORD dwHotkeyCode=HK_PLUGBASE+(wParam-(WM_USER+PLUGIN_FUNCTION));
					CRegisteredHotkeyData psHotkeyDsc;
					if(pMainDialogWindow){_XLOG_
						GetRegisteredHotkeyDsc(dwOrigCode,pMainDialogWindow->GetSafeHwnd(),0,&psHotkeyDsc);
						stuff.pSpecialHotkeyDsc=&psHotkeyDsc;
						stuff.bOnUnpress=psHotkeyDsc.bOnUnpress;
					}
					int iRes=CallPluginFunction(plg,j,&stuff);
					if(iRes==0){_XLOG_
						if(stuff.szError[0]!=0){_XLOG_
							Alert(stuff.szError,5000);
							AsyncPlaySound(SND_ERROR);
						}
					}
				}
				if(!plg->bResident){_XLOG_
					UnloadPluginRaw(plg);
				}
				if(pSL){_XLOG_
					delete pSL;
					pSL=0;
				}
			}
		}
	}
	return 0;
}

DWORD WINAPI ShowAppInfoEx(HWND hWnd,DWORD dwProcID)
{_XLOG_
	static long lLock=0;
	if(lLock>0){_XLOG_
		return 0;
	}
	SimpleTracker lc(lLock);
	if(!hWnd && !dwProcID){_XLOG_
		return 0;
	}
	if(hWnd!=NULL){_XLOG_
		GetWindowThreadProcessId(hWnd,&dwProcID);
	}
	CProcInfo* info=0;
	RefreshProcessListIfNeeded();
	GetProcessInfo(dwProcID,info);
	if(info!=NULL){_XLOG_
		CDlg_AppInfo dlg;
		dlg.dwProcId=dwProcID;
		dlg.sAppPath=info->sProcExePath;
		dlg.m_Info=_l("Info for application")+": "+info->sProcName+"\r\n";
		//HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils.dll");
		//if(hUtils)
		{_XLOG_
			//_GetFileDesc fp=(_GetFileDesc)DBG_GETPROC(hUtils,"GetFileDescription");
			//if(fp)
			{_XLOG_
				char sz[256]={0};
				GetFileDescription(info->sProcExePath,sz);
				dlg.m_Info+=_l("File description")+": "+CString(sz)+"\r\n";
			}
			//FreeLibrary(hUtils);
		}
		dlg.m_Info+=_l("Memory usage")+": "+Format(info->lWorkingSet)+"\r\n";
		dlg.m_Info+="\r\n";
		dlg.m_Info+=_l("Path to exe-file")+": "+info->sProcExePath+"\r\n";
		CString sCommandLine;
		GetProcessComLine(info->dwProcId,sCommandLine);
		if(sCommandLine!=""){_XLOG_
			dlg.m_Info+=_l("Command line")+": "+sCommandLine+"\r\n";
		}
		dlg.m_Info+=_l("Process ID")+": "+Format(info->dwProcId)+"; ";
		dlg.m_Info+=_l("Uptime")+": "+Format("%i",(info->KernelTime+info->UserTime)/1000);
		dlg.m_Info+="\r\n\r\n";
		if(hWnd!=NULL){_XLOG_
			dlg.hSlWnd=hWnd;
			// Информация об окне
			char szTitle[128]="";
			::GetWindowText(hWnd,szTitle,sizeof(szTitle));
			dlg.m_Info+=_l("Window title")+": "+szTitle+"\r\n";
			char szClass[128]="";
			::GetClassName(hWnd,szClass,sizeof(szClass));
			dlg.m_Info+=_l("Window class")+": "+szClass+"\r\n";
			RECT rect;
			RECT rectC;
			::GetWindowRect(hWnd,&rect);
			::GetClientRect(hWnd,&rectC);
			dlg.m_Info+=Format("%s: (%i,%i)-(%i,%i); %s: (%i,%i)\r\n",_l("Window position"),rect.left,rect.top,rect.right,rect.bottom,_l("Size"),rect.right-rect.left,rect.bottom-rect.top);
			dlg.m_Info+=Format("%s: (%i,%i)-(%i,%i); %s: (%i,%i)\r\n",_l("Window client part position"),rectC.left,rectC.top,rectC.right,rectC.bottom,_l("Size"),rectC.right-rectC.left,rectC.bottom-rectC.top);
			dlg.m_Info+="\r\n";
		}
		CString sFiles;
		SystemGetOpenedHandles(dwProcID,sFiles);
		if(sFiles!=""){_XLOG_
			dlg.m_Info+=_l("Opened files")+":\r\n";
			dlg.m_Info+=sFiles;
		}
		dlg.DoModal();
	}
	return 0;
}

DWORD WINAPI ShowAppInfo_InThread(LPVOID pData)
{_XLOG_
	return ShowAppInfoEx(HWND(pData),0);
}

DWORD WINAPI ShowAppInfo2_InThread(LPVOID pData)
{_XLOG_
	return ShowAppInfoEx(0,DWORD(pData));
}

void KillProcessesFromList(CString sKillList)
{_XLOG_
__FLOWLOG;
	// Убиваем случайные пробелы
	CCSLock lpc(&pCahLock,1);
	while(sKillList.Replace("* ","*")>0){};
	while(sKillList.Replace(" *","*")>0){};
	sKillList.Replace("*,","*;");
	sKillList.Replace(",*",";*");
	CString sList=sKillList;
	if(sList==""){_XLOG_
		return;
	}
	sList.TrimLeft();
	sList.TrimRight();
	sList.MakeLower();
	if(!EnumProcesses()){_XLOG_
		return;
	}
	for(int iProcNum=0; iProcNum<GetProccessCash().GetSize();iProcNum++){_XLOG_
		CProcInfo& info=GetProccessCash()[iProcNum];
		CString sTitle=info.sProcName;
		if(sTitle!=""){_XLOG_
			sTitle.MakeLower();
			if(PatternMatchList(sTitle,sList)){_XLOG_
				KillProcess(info.dwProcId);
			}
		}
	}
}

BOOL ParsedInternetMem(CString sText)
{
	// Файл читаем даже когда пуст - используется чтобы подготовить перед открытием настроек
	CString sIMemFile=GetUserFolder()+"smartkeywords.ini";
	if(!isFileExist(sIMemFile)){
		CString sDef=CString(GetApplicationDir())+"smartkeywords.ini";
		if(isFileExist(sDef)){
			CString s;
			ReadFile(sDef,s);
			SaveFile(sIMemFile,s);
		}
	}
	if(sText==""){
		return 0;
	}
	CStringArray aInputParams;
	ConvertToArray(sText, aInputParams, ' ', 0);
	if(aInputParams.GetSize()==0){
		return 0;
	}
	BOOL bRes=0;
	CString sData;
	ReadFile(sIMemFile,sData);
	sData+="\n";
	CStringArray aStrings;
	ConvertToArray(sData, aStrings, '\n', ';');
	CString sCustomBrowser=CDataXMLSaver::GetInstringPart("\nCustomBrowser=","\n",sData);
	sCustomBrowser.TrimLeft();
	sCustomBrowser.TrimRight();
	for(int i=0;i<aStrings.GetSize();i++){
		CString s=aStrings[i];
		s.TrimLeft();
		s.TrimRight();
		CStringArray params;
		ConvertToArray(s, params, ' ', 0);
		if(params.GetSize()>1){
			if(params[0].CompareNoCase(aInputParams[0])==0){
				//Совпало!!!!
				CString sStringToRun=params[1];
				//sStringToRun.Replace("%s",EscapeString(aInputParams[1]));
				if(aInputParams.GetSize()>3){
					sStringToRun.Format(sStringToRun,EscapeString2(aInputParams[1]),EscapeString2(aInputParams[2]),EscapeString2(aInputParams[3]));
				}else if(aInputParams.GetSize()>2){
					sStringToRun.Format(sStringToRun,EscapeString2(aInputParams[1]),EscapeString2(aInputParams[2]));
				}else if(aInputParams.GetSize()>1){
					sStringToRun.Format(sStringToRun,EscapeString2(aInputParams[1]));
				}
				if(sCustomBrowser==""){
					::ShellExecute(NULL,"open",sStringToRun,"",NULL,SW_SHOWNORMAL);
				}else{
					::ShellExecute(NULL,"open",sCustomBrowser,sStringToRun,NULL,SW_SHOWNORMAL);
				}
				if(!bRes){
					//ShowOSD(_l("Activating smart keyword"),3000);
					CSplashParams par;
					FillSplashParams("",par);
					par.szText=_l("Activating smart keyword")+"...";
					CSplashWindow* pOSD=new CSplashWindow(&par);
					pOSD->AutoClose(3000);
				}
				bRes=1;
			}
		}
	}
	return bRes;
}

BOOL RunAsCommand(CString sText)
{_XLOG_
	sText.TrimLeft();
	sText.TrimRight();
	if(ParsedInternetMem(sText)){
		return 0;
	}
	BOOL bRes=ParsedAsArithmeticalExpr(sText);
	if(bRes!=0){_XLOG_
		if(bRes==1){_XLOG_
			AsyncPlaySound(CALC_TEXT);
			PutSelectedTextInFocusWnd(sText);
		}
		return 0;
	}
	if(sText.GetLength()!=0){_XLOG_
		sText.TrimLeft("=;,");
		sText.TrimRight("=;,");
		//
		CSplashParams par;
		FillSplashParams(sText,par);
		CSplashWindow SplWnd(&par);
		CString sCommand, sParameter;
		// Смотрим - может здесь программа+параметр
		ParseForShellExecute(sText, sCommand, sParameter);
		// Запускаем
		const char* szParameter=sParameter;
		if(*szParameter==0){_XLOG_
			szParameter=NULL;
		}
		int iRes=(int)::ShellExecute(NULL,NULL,sCommand,szParameter,NULL,SW_SHOWNORMAL);
		if(iRes<=32){_XLOG_
			//ERROR_FILE_NOT_FOUND
			Alert(Format("%s '%s'",_l("Failed to run"),TrimMessage(sText,30,2)));
		}else{_XLOG_
			AsyncPlaySound(CALC_TEXT);
		}
	}
	::SetForegroundWindow(objSettings.hHotMenuWnd);
	return 0;
}

DWORD WINAPI RunAsCommand_inThread(LPVOID pText)
{_XLOG_
	CString sCommand=(const char*)pText;
	free(pText);
	RunAsCommand(sCommand);
	return 0;
}

extern DWORD PriorityClasses[];
extern DWORD PriorityClassesNT[];
BOOL CALLBACK WK_EnumWindowsProc(HWND hwnd, LPARAM lParam);
BOOL AppMainDlg::PopupAppWindow(DWORD dwProcID,BOOL bAction) 
{_XLOG_
	BOOL bRes=0;
	objSettings.sTopLevelWindowsTitles.RemoveAll();
	objSettings.sTopLevelWindowsHWnds.RemoveAll();
	if(::EnumWindows(WK_EnumWindowsProc,1)){_XLOG_
		for(int i=0;i<objSettings.sTopLevelWindowsHWnds.GetSize();i++){_XLOG_
			DWORD dwProc=0;
			WINDOWPLACEMENT pl;
			HWND hWin=objSettings.sTopLevelWindowsHWnds[i];
			::GetWindowThreadProcessId(hWin,&dwProc);
			pl.length=sizeof(WINDOWPLACEMENT);
			::GetWindowPlacement(hWin,&pl);
			if(dwProc==dwProcID){_XLOG_
				GetWindowTopParent(hWin);
				if(bAction){_XLOG_
					::PostMessage(hWin,WM_SYSCOMMAND,SC_CLOSE,0);
				}else{_XLOG_
					::RedrawWindow(hWin,NULL,NULL,RDW_ERASE|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
					SwitchToWindow(hWin);
				}
				bRes=1;
				break;
			}
		}
	}
	return bRes;
}

DWORD WINAPI SwitchCurrentDesktop(LPVOID pData)
{_XLOG_
	TRACETHREAD;
	static long lLock=0;
	if(lLock>0 || !objSettings.lEnableAddDesktops || objSettings.bUnderWindows98){// Уже идет переключение...
		return 0;
	}
	SimpleTracker tr(lLock);
	// Меняем десктоп и если нужно - разрешение
	long lCurrentDesktop=objSettings.lLastActiveDesktop;
	int iDesk=(int)pData;
	HDESK hDesktop=NULL;
	BOOL bDesktopAlreadyExist=TRUE;
	char szDesktopID[DEF_DESKNAMESIZE]="";
	GetDesktopInternalName(iDesk,szDesktopID);
	hDesktop=::OpenDesktop(szDesktopID,0,FALSE,MAXIMUM_ALLOWED);//DF_ALLOWOTHERACCOUNTHOOK
	if(hDesktop==NULL){_XLOG_
		hDesktop=::CreateDesktop(szDesktopID,NULL,NULL,0,MAXIMUM_ALLOWED,NULL);//DF_ALLOWOTHERACCOUNTHOOK
		bDesktopAlreadyExist=FALSE;// Иначе десктоп тутже убъется! :)
	}
	if(hDesktop==NULL){_XLOG_
		AsyncPlaySound(SND_ERROR);
		return 0;
	}
	// Выключаем лишнее...
	if(pMainDialogWindow){_XLOG_
		WaitWhileAllKeysAreFreeX();
		// Отключаем плагины
		PrepareDesktopSwitch(TRUE);
		//---------------
		if(objSettings.lRemeberResolutInDesktops){_XLOG_
			// Сбрасываем границы...
			pMainDialogWindow->Sizer.GetScreenRect()=CRect(0,0,0,0);
		}
	}
	CString sNoSwitchReason;
	BOOL bSwitchedOK=TRUE;
	if(bSwitchedOK && !AppRunStateFile(NULL, 1, szDesktopID)){// WireKeys&explorer
		// Стартуем необходимые программы
		{// Shell process
			STARTUPINFO su;
			PROCESS_INFORMATION pi;
			ZeroMemory(&su, sizeof(su));
			ZeroMemory(&pi, sizeof(pi));
			GetStartupInfo(&su);
			su.cb = sizeof(su);
			su.dwFlags = STARTF_USESHOWWINDOW;
			su.wShowWindow = SW_SHOWDEFAULT;
			su.lpDesktop = szDesktopID;
			CString sShell="explorer.exe";
			if(objSettings.lRemeberShellInDesktops){_XLOG_
				sShell=objSettings.lDeskShells[iDesk];
			if(sShell==""){_XLOG_
				sShell="explorer.exe";
			}
			}
			char szShellApp[MAX_PATH]="";
			strcpy(szShellApp,sShell);
			if(CreateProcess(NULL, szShellApp, NULL,NULL, false, NULL, NULL, NULL, &su, &pi)){
				bSwitchedOK=TRUE;
				Sleep(1500);
			}else{
				bSwitchedOK=FALSE;
				sNoSwitchReason=Format("%s: %i",szShellApp,GetLastError());
			}
		}
		{
			STARTUPINFO su;
			PROCESS_INFORMATION pi;
			ZeroMemory(&su, sizeof(su));
			ZeroMemory(&pi, sizeof(pi));
			GetStartupInfo(&su);
			su.cb = sizeof(su);
			su.dwFlags = STARTF_USESHOWWINDOW;
			su.wShowWindow = SW_HIDE;
			su.lpDesktop = szDesktopID;
			char szCLine[MAX_PATH]="";
			strcpy(szCLine,GetAppExe()+" -plugins=no");
			if(CreateProcess(GetAppExe(),szCLine, NULL,NULL, false, NULL, NULL, NULL, &su, &pi)){
				bSwitchedOK=TRUE;
				Sleep(1500);
			}else{
				bSwitchedOK=FALSE;
				sNoSwitchReason=Format("%s: %i",GetAppExe(),GetLastError());
			}
		}
	}
	if(bSwitchedOK){
		SetLastError(0);
		bSwitchedOK=::SwitchDesktop(hDesktop);
		if(bSwitchedOK){
			// По новой "технологии" активный десктоп всегда один - тот, с которого запускали
			objSettings.lLastActiveDesktop=lCurrentDesktop;
			SetThreadDesktop(hDesktop);
			// Говорим включить гор. клавиши... (уже после установки нового десктопа)
			DWORD dwTarget=BSM_APPLICATIONS|BSM_ALLDESKTOPS;
			BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, AppMainApp::iWM_THIS, WPARAM(3), LPARAM(iDesk));
			// Устанавливаем новый десктоп
			if(objSettings.lRemeberResolutInDesktops){_XLOG_
				long lRes=CVideoModes::ChangeVideoModePermanentlyEx(objSettings.lResolutions[iDesk]);
				if(lRes!=0){
					CString sErr=Format("Failed to set '%s' mode! Error - %X",objSettings.lResolutions[iDesk].Name(),lRes);
					Alert(sErr);
					FLOG(sErr);
				}
			}
		}else
		{
			sNoSwitchReason=Format("%s: 0x%08X",_l("Error code"),GetLastError());
		}
	}
	if(!bSwitchedOK){
		Alert(_l("Failed to switch to another desktop")+"\n"+sNoSwitchReason);
	}
	CloseDesktop(hDesktop);
	return 0;
}

DWORD WINAPI TrackLaunchSetDblPress(LPVOID pData)
{_XLOG_
	TRACETHREAD;
	int iWndNum=(int)pData;
	CQuickRunItem* pItem=objSettings.aQuickRunApps[iWndNum];
	long lHMAction=0;
	if(objSettings.lAllowDblCategStart){_XLOG_
		IsThreadDoubleCalled(lHMAction);
	}
	if(lHMAction){_XLOG_
		pMainDialogWindow->OnRunQRunApplication(iWndNum,0);
	}else{_XLOG_
		pMainDialogWindow->PostMessage(ID_SYSTRAY_RUNQRUNAPP,iWndNum,QRUN_SHOWMENU);
	}
	return 0;
}

DWORD WINAPI PerformTrayAction_InThread(LPVOID pData)
{_XLOG_
	if(!pMainDialogWindow){_XLOG_
		return 0;
	}
	return pMainDialogWindow->PerformTrayAction((int)pData,FALSE);
}

void AppMainDlg::SwitchResolution(int iMode)
{
	CVideoModes::ChangeVideoModePermanently(objSettings.videoModes[iMode]);
	// Сбрасываем границы...
	Sizer.GetScreenRect()=CRect(0,0,0,0);
}

DWORD WINAPI OnHotmenuSortLines(LPVOID pType);
BOOL AppMainDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{_XLOG_
	if(HandleTuneUpItem2(wParam)){
		return TRUE;
	}
	if(lParam==0 && wParam==2){_XLOG_
		ShowWindow(SW_HIDE);
		return TRUE;
	}
	if(wParam==ID_REATTACHCLIP_REALLY){
		AttachToClipboard(TRUE);
		return TRUE;
	}
	if(wParam==ID_SYSTRAY_RESTARTWK){_XLOG_
		wk.RestartWK();
		return TRUE;
	}
	if(wParam==ID_SYSTRAY_CHECKFORUPDATES){_XLOG_
		GetVersionChecker().SetTraits("(Manual)", IDI_ICON_ASK);
		StartCheckNewVersion(0);
		return TRUE;
	}
	if(wParam==TRAYICO_DBCL){_XLOG_
		objSettings.hHotMenuWnd=m_STray.hLastActiveWindow;
		int iAction=-1;
		if(lParam==WM_LBUTTONDBLCLK){_XLOG_
			iAction=objSettings.lLBTray-1;
		}
		if(lParam==WM_MBUTTONDBLCLK){_XLOG_
			iAction=objSettings.lMBTray-1;
		}
		if(lParam==WM_RBUTTONDBLCLK){_XLOG_
			iAction=objSettings.lRBTray-1;
		}
		if(iAction>=0){_XLOG_
			PerformTrayAction(iAction,FALSE);
			//FORK(PerformTrayAction_InThread,iAction);
		}
		return TRUE;
	}
	if(wParam==CHANGEICON){_XLOG_
		char* szFile=(char*)lParam;
		if(szFile){_XLOG_
			ChangeItemIcon(szFile);
		}
		delete szFile;
		return TRUE;
	}
	if(wParam==ADDTOQUICKRUNS){_XLOG_
		char* szFile=(char*)lParam;
		if(szFile){_XLOG_
			CString sTopic=DEFQRUN_TOPIC;
			if(strstr(szFile,".wkm")!=NULL){_XLOG_
				sTopic=DEFQMAC_TOPIC;
			}
			objSettings.OpenOptionsDialog(1,sTopic,this,0,szFile);
		}
		return TRUE;
	}
	if(wParam==FIREHOTMENU){_XLOG_
		AWndManagerMain(NULL,TRUE,lParam);
		return TRUE;
	}
	if(wParam==OPT_TOPIC){_XLOG_
		char* pt=(char*)lParam;
		objSettings.OpenOptionsDialog(0,pt,this);
		free(pt);
		return TRUE;
	}
	if(wParam==PASTETEXT){_XLOG_
		// Вставлять текст можно только в отдельном потоке!!!
		FORK(PasteClipText,lParam);
		return TRUE;
	}
	if(wParam==APPINFO){_XLOG_
		ShowAppInfo_InThread((LPVOID)lParam);
		return TRUE;
	}
	if(wParam==APPINFO2){_XLOG_
		ShowAppInfo2_InThread((LPVOID)lParam);
		return TRUE;
	}
	if(wParam==ID_SYSTRAY_KILLPROCESSESFROMLIST){_XLOG_
		KillProcessesFromList(objSettings.sListToKillSim);
		return TRUE;
	}
	if(wParam>=WM_USER+PLUGINTP_OFFSET && wParam<(DWORD)(WM_USER+PLUGINTP_OFFSET+USERMSG_STEP)){
		for(int i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
			CPlugin* oPlug=objSettings.plugins[i];
			if(oPlug){_XLOG_
				if(oPlug && oPlug->bIsActive){_XLOG_
					if(wParam>=oPlug->lTrayPopupItemID_Start && wParam<oPlug->lTrayPopupItemID_End){
						int ID=wParam;//-oPlug->lTrayPopupItemID_Start;
						CallPluginTPFunction(oPlug, TM_HANDLECUSTOMITEM, ID, 0);
					}
				}
			}
		}
	}
	if(wParam>=WM_USER+KILLPROCESS && wParam<(DWORD)(WM_USER+KILLPROCESS+USERMSG_STEP)){_XLOG_
		int iProcNum=wParam-(WM_USER+KILLPROCESS);
		if(iProcNum>=0 && iProcNum<TrayMenuProcesses.GetSize()){_XLOG_
			CProcInfo& info=TrayMenuProcesses[iProcNum];
			CString sTitle=info.sProcName;
			if(info.sProcExePath!=""){_XLOG_
				sTitle+=" (";
				info.sProcExePath.TrimRight();
				sTitle+=TrimMessage(info.sProcExePath,30,4);
				sTitle+=")";
			}
			if(!KillProcess(info.dwProcId,sTitle)){_XLOG_
				AddError(_l("Unable to kill process"));
			}
		}
		return TRUE;
	}
	//
	if(wParam>=WM_USER+HIDEDWINDOWS && wParam<(DWORD)(WM_USER+HIDEDWINDOWS+USERMSG_STEP)){_XLOG_
		CSmartLock SL(&csInfoWnds,TRUE);
		int iWndNum=wParam-(WM_USER+HIDEDWINDOWS);
		if(iWndNum>=0 && iWndNum<objSettings.aHidedWindowsMap.GetSize()){_XLOG_
			InfoWnd* pW=objSettings.aHidedWindowsMap[iWndNum];
			if(pW){_XLOG_
				pW->PostMessage(WM_COMMAND,ID_INFOWND_OPENPR,0);
			}
		}
		return TRUE;
	}
	if(wParam>=WM_USER+EXPAND_FILES && wParam<(DWORD)(WM_USER+EXPAND_FILES+EXPAND_FILES_MAX)){_XLOG_
		int iItem=wParam-(WM_USER+EXPAND_FILES);
		if(iItem>=0 && iItem<aExpandedFolderPaths.GetSize()){_XLOG_
			CString sItemPath=aExpandedFolderPaths[iItem];
			if(sItemPath!=""){_XLOG_
				int iRes=(int)::ShellExecute(this->GetSafeHwnd(),"",sItemPath,NULL,NULL,SW_SHOWNORMAL);
				if(iRes<=32){_XLOG_
					Alert(_l("Failed to start")+"\n'"+TrimMessage(sItemPath,30,3)+"' !",_l(PROGNAME": Quick-run"));
				}
			}
		}
		return TRUE;
	}
	if(wParam>=WM_USER+QRUNAPPLICATION && wParam<(DWORD)(WM_USER+QRUNAPPLICATION+USERMSG_STEP_HALF)){_XLOG_
		int iWndNum=wParam-(WM_USER+QRUNAPPLICATION);
		CQuickRunItem* pItem=objSettings.aQuickRunApps[iWndNum];
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		if(lParam & QRUN_DETACHTO){_XLOG_
			InfoWndStruct* info=new InfoWndStruct();
			info->bStartHided=2;
			info->bDetached=(lParam & QRUN_DETACHFL)!=0;
			info->hWnd=HWND(-objSettings.aQuickRunApps[iWndNum]->lUniqueCount);
			Alert(info);
		}else{_XLOG_
			BOOL bAddOptions=0;
			if((lParam & QRUN_ONWKSTART)==0){_XLOG_
				bAddOptions=1;
			}
			if((lParam & QRUN_NOFOREGR)==0){_XLOG_
				::SetForegroundWindow(m_STray.hLastActiveWindow);
				objSettings.hHotMenuWnd=m_STray.hLastActiveWindow;
			}
			if((lParam & QRUN_BYHOTKEY)==0){_XLOG_
				if(TuneUpItemX(wk.GetQRunOptTitle(objSettings.aQuickRunApps[iWndNum]))){_XLOG_
					return TRUE;
				}
			}
			// Special case - double pressing on launch set button
			if(pItem && (lParam & QRUN_BYHOTKEY) && pItem->lMacros==CATEG_MACR){_XLOG_
				FORK(TrackLaunchSetDblPress,iWndNum);
				return TRUE;
			}
			OnRunQRunApplication(iWndNum,lParam);
		}
		return TRUE;
	}
	if(wParam>=WM_USER+QRUNAPP_EXTRA && wParam<(DWORD)(WM_USER+QRUNAPP_EXTRA+USERMSG_STEP_HALF)){_XLOG_
		DWORD iWndCode=(wParam-(WM_USER+QRUNAPP_EXTRA))+HK_EXTRAMACRO;
		int iWndNum=-1,iWndEntry=-1;
		//Поиск...
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			if(objSettings.aQuickRunApps[i]){_XLOG_
				for(int j=0;j<objSettings.aQuickRunApps[i]->qrHotkeys.GetSize();j++){_XLOG_
					if(objSettings.aQuickRunApps[i]->qrHotkeys[j]->dwHotkeyID==iWndCode){_XLOG_
						iWndNum=i;
						iWndEntry=j;
						break;
					}
				}
			}
		}
		if(iWndNum>=0 && iWndEntry>=0){_XLOG_
			CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
			RunQRunApp(iWndNum,0,iWndEntry);
		}
		return TRUE;
	}
	if(wParam>=WM_USER+QRUNAPP_EDIT && wParam<(DWORD)(WM_USER+QRUNAPP_EDIT+USERMSG_STEP_HALF)){_XLOG_
		int iWndNum=wParam-(WM_USER+QRUNAPP_EDIT);
		objSettings.OpenOptionsDialog(1,wk.GetQRunOptTitle(objSettings.aQuickRunApps[iWndNum]),NULL);
		return TRUE;
	}
	if(wParam>=WM_USER+PLUGIN_FUNCTION && wParam<(DWORD)(WM_USER+PLUGIN_FUNCTION+USERMSG_STEP)){_XLOG_
		// wParam-причесанный код, lParam-сырой
		CMsgData* msg=new CMsgData();
		msg->wParam=wParam;
		msg->lParam=lParam;
		FORK(PluginOperation2,msg);
		return TRUE;
	}
	if(wParam>=WM_USER+SET_DMODE && wParam<(DWORD)(WM_USER+SET_DMODE+USERMSG_STEP)){_XLOG_
		int iMode=wParam-(WM_USER+SET_DMODE);
		// Спрашиваем разрешения...
		DWORD dwRes=AskYesNo(_l("Do you want to change resolution")+"?\r\n"+objSettings.videoModes[iMode].Name(),_l("Confirmation"),0,GetAppWnd());
		if(dwRes==IDYES){_XLOG_
			SwitchResolution(iMode);
		}
		return TRUE;
	}
	if(wParam>=WM_USER+SWITCH_DESKTOP && wParam<(DWORD)(WM_USER+SWITCH_DESKTOP+USERMSG_STEP)){_XLOG_
		if(lParam==0 && TuneUpItem(ADDDESK_TOPIC)){_XLOG_
			return TRUE;
		}
		int iDesk=wParam-(WM_USER+SWITCH_DESKTOP);
		FLOG2("Switching to desktop #%i %s",iDesk, wk.GetDesktopName(iDesk));
		FORK(SwitchCurrentDesktop,iDesk);
		return TRUE;
	}
	if(wParam>=WM_USER+ID_COPYTOCLIP && wParam<(DWORD)(WM_USER+ID_COPYTOCLIP+USERMSG_STEP)){_XLOG_
		int iClip=wParam-(WM_USER+ID_COPYTOCLIP);
		FORK(ClipboardOperations,MAKELONG(iClip,WORD(-1)))
		return TRUE;
	}
	if(wParam>=WM_USER+ID_PASTECLIP && wParam<(DWORD)(WM_USER+ID_PASTECLIP+USERMSG_STEP)){_XLOG_
		int iClip=wParam-(WM_USER+ID_PASTECLIP);
		FORK(ClipboardOperations,MAKELONG(iClip,WORD(0)))
		return TRUE;
	}
	if(wParam>=WM_USER+ACTIVEAPP_PRIOR && wParam<(DWORD)(WM_USER+ACTIVEAPP_PRIOR+USERMSG_STEP)){_XLOG_
		int iPrior=wParam-(WM_USER+ACTIVEAPP_PRIOR);
		DWORD dwProcID=0;
		GetWindowThreadProcessId(objSettings.hHotMenuWnd,&dwProcID);
		if(dwProcID && iPrior>=0 && iPrior<=4){_XLOG_
			HANDLE hActiveProc=::OpenProcess(PROCESS_SET_INFORMATION,FALSE,dwProcID);
			if (!( (hActiveProc) == NULL || (hActiveProc) == INVALID_HANDLE_VALUE )){_XLOG_
				SetPriorityClass(hActiveProc,objSettings.bUnderWindows98?PriorityClasses[iPrior]:PriorityClassesNT[iPrior]);
				CloseHandle(hActiveProc);
			}
		}
		return TRUE;
	}
	if(wParam>=WM_USER+ID_CLIPHISTORY && wParam<(DWORD)(WM_USER+ID_CLIPHISTORY+USERMSG_STEP)){_XLOG_
		if(lParam==1 || lParam==3){_XLOG_
			CString sText;
			ShowClipHistoryMenu(RUNHIST_NAME,_l("History of runs"),&sText,(lParam==3));
			if(sText!=""){_XLOG_
				FORK(RunAsCommand_inThread,strdup((const char*)sText));
			}
			return TRUE;
		}
		int iClip=wParam-(WM_USER+ID_CLIPHISTORY);
		SimpleLocker l(&csClipBuffer);
		if(iClip>=0 && iClip<objSettings.sClipBuffer.GetSize()){_XLOG_
			CString sTitle;
			if(iClip>0){_XLOG_
				sTitle=_l("Clip")+Format(" #%i - ",iClip)+_l("history");
			}else{_XLOG_
				sTitle=_l("Standard clipboard - history");
			}
			ShowClipHistoryMenu(GetClipHistoryName(iClip),sTitle,0,(lParam==2));
		}
		return TRUE;
	}
	// Чтобы не вызывать переключения активного приложения в WireKeys
	// Вызываем прямо здесь, а не через ON_COMMAND(...)
	if(wParam==ID_HOTMENU_CONVERTSELECTED){_XLOG_
		OnHotmenuConvertselected();
		return TRUE;
	}
	if(wParam==ID_HOTMENU_UPPERCASE){_XLOG_
		OnHotmenuUppercase();
		return TRUE;
	}
	if(wParam==ID_HOTMENU_LOWERCASE){_XLOG_
		OnHotmenuLowercase();
		return TRUE;
	}
	if(wParam==ID_HOTMENU_SWITCHCASE){_XLOG_
		OnHotmenuSwitchcase();
		return TRUE;
	}
	if(wParam==ID_HOTMENU_COUNTCHARACTERS){_XLOG_
		OnHotmenuCountcharacters();
		return TRUE;
	}
	if(wParam==ID_HOTMENU_SORTLINES){_XLOG_
		FORK(OnHotmenuSortLines,0);
		return TRUE;
	}
	if(wParam==ID_HOTMENU_SORTLINES_AZ){_XLOG_
		FORK(OnHotmenuSortLines,1);
		return TRUE;
	}
	if(wParam==ID_HOTMENU_SORTLINES_ZA){_XLOG_
		FORK(OnHotmenuSortLines,2);
		return TRUE;
	}
	if(wParam==ID_HOTMENU_SORTLINES_09){_XLOG_
		FORK(OnHotmenuSortLines,3);
		return TRUE;
	}
	if(wParam==ID_HOTMENU_SORTLINES_90){_XLOG_
		FORK(OnHotmenuSortLines,4);
		return TRUE;
	}
	if(wParam==ID_HOTMENU_CLEARCLIPS){_XLOG_
		OnHotmenuClearclips();
		return TRUE;
	}
	if(wParam==ID_HOTMENU_CALCULATE){_XLOG_
		OnHotmenuCalculate();
		return TRUE;
	}
	if(wParam==ID_HOTMENU_RUNASCOMMAND){_XLOG_
		OnHotmenuRunascommand();
		return TRUE;
	}
	return CDialog::OnCommand(wParam, lParam);
}

void AppMainDlg::OnSystrayPlgDownload() 
{_XLOG_
	ShellExecute(this->GetSafeHwnd(),"open",(CString)"http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wkplugins&who="PROGNAME+PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
}

void AppMainDlg::OnSystrayPlgOptions() 
{_XLOG_
	objSettings.OpenOptionsDialog(2,_l(PLUGINS_TOPIC),this);
}

void AppMainDlg::OnSystrayOptions2() 
{
	m_STray.ShowIcon();
	objSettings.OpenOptionsDialog(1,/*objSettings.sDefOptionsTopic==""?GSTARTED_TOPIC:objSettings.sDefOptionsTopic*/DEFAPPLACTIONS_TOPIC,this);
}

void AppMainDlg::OnSystrayOptions() 
{_XLOG_
	m_STray.ShowIcon();
	if(!objSettings.OpenOptionsDialog(0,objSettings.sDefOptionsTopic==""?GSTARTED_TOPIC:objSettings.sDefOptionsTopic,this) && objSettings.pOpenedOptions){_XLOG_
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)objSettings.pOpenedOptions->GetSafeHwnd());
		return;
	}
	if(objSettings.sLastOpenedOptionsTopic!=""){_XLOG_
		objSettings.sDefOptionsTopic=objSettings.sLastOpenedOptionsTopic;
	}
	if(IsIconMustBeHided()){_XLOG_
		m_STray.HideIcon();
	}
}

void AppMainDlg::OnSystraySetMute()
{_XLOG_
	if(TuneUpItem(VOL_TOPIC)){_XLOG_
		return;
	}
	ChangeVolumeControl(0,!mixer.GetMuteCtrlValue());
}

void AppMainDlg::OnSystraySetVol0() 
{_XLOG_
	if(TuneUpItem(VOL_TOPIC)){_XLOG_
		return;
	}
	mixer.SetVolumeCtrlValue(0);
}

void AppMainDlg::OnSystraySetVol30() 
{_XLOG_
	if(TuneUpItem(VOL_TOPIC)){_XLOG_
		return;
	}
	mixer.SetVolumeCtrlValue((UINT)(MAX_MIXER_VOL*0.3));
}

void AppMainDlg::OnSystraySetVol50() 
{_XLOG_
	if(TuneUpItem(VOL_TOPIC)){_XLOG_
		return;
	}
	mixer.SetVolumeCtrlValue((UINT)(MAX_MIXER_VOL*0.5));
}

void AppMainDlg::OnSystraySetVol70() 
{_XLOG_
	if(TuneUpItem(VOL_TOPIC)){_XLOG_
		return;
	}
	mixer.SetVolumeCtrlValue((UINT)(MAX_MIXER_VOL*0.7));
}

void AppMainDlg::OnSystraySetVol100() 
{_XLOG_
	if(TuneUpItem(VOL_TOPIC)){_XLOG_
		return;
	}
	mixer.SetVolumeCtrlValue(MAX_MIXER_VOL);
}

DWORD WINAPI RunHotkeyInThread(LPVOID lpData)
{_XLOG_
	TRACETHREAD;
	LOGSTAMP
	__FLOWLOG;
	SimpleTracker trG(lHotkeyActionInProgress);
	if(!pMainDialogWindow){_XLOG_
		return 2;
	}
	//---------------
	SetCurrentDesktop();
	//Выполняем операции, которые могут работать в паралельном режиме
	DWORD dwOrigCode=(DWORD)lpData;
	int iHotKey=(dwOrigCode&CODE_NOPREFIX);
	if(iHotKey>=100 && iHotKey<=(100+LAST_ACTION_CODE)){_XLOG_
		LOGSTAMP
		pMainDialogWindow->PerformTrayAction(iHotKey-100, TRUE);
		return 0;
	}
	//Дальше паралельного вычисления уже не будет
	static long lLock=0;
	if(lLock>0){_XLOG_
		return 2;
	}
	SimpleTracker tr(lLock); 
	if(iHotKey>=300 && iHotKey<=(300+MAX_CLIPS)){_XLOG_
		pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+ID_COPYTOCLIP+(iHotKey-300),0);
		return 0;
	}
	if(iHotKey>=400 && iHotKey<=(400+MAX_CLIPS)){_XLOG_
		pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+ID_PASTECLIP+(iHotKey-400),0);
		return 0;
	}
	if(iHotKey>=HK_PLUGBASE && iHotKey<=(HK_PLUGBASE+MAX_PLUGFUNCS)){_XLOG_
		objSettings.hLastForegroundWndForPlugins=GetForegroundWindow();
		pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+PLUGIN_FUNCTION+(iHotKey-HK_PLUGBASE),dwOrigCode);
		return 0;
	}
	return 1;
}

CIHotkey hLastHotkeyPressed,hLastHotkeyPressedPrev;
afx_msg LRESULT AppMainDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{_XLOG_
	__FLOWLOG;
	FLOGWND("OnHotkey wnd: ",::GetForegroundWindow());
	if(!objSettings.lEverythingIsPaused){_XLOG_
		AsyncPlaySound(HK_TEXT);
		LOGSTAMP
		CString sHotKeyName;
		CRegisteredHotkeyData sHotKeyDsc;
		GetRegisteredHotkeyDsc(int(wParam),GetSafeHwnd(),&hLastHotkeyPressed,&sHotKeyDsc);
		if(wk.GetHotkeyInputInProgress()>0){_XLOG_
			if(!objSettings.bSkipHKWarning){_XLOG_
				sHotKeyName=GetIHotkeyName(hLastHotkeyPressed);
				if(!hLastHotkeyPressed.isSeq() && !hLastHotkeyPressed.isOnUnp()){_XLOG_
					hLastHotkeyPressedPrev=hLastHotkeyPressed;
					if(sHotKeyName!=wk.GetHotkeyInputInProgress()->sStartupValue){_XLOG_
						ShowBaloon("","");
						CString sText=_l("Please note: This hotkey is already used")+"!\n";
						if(!wk.isPluginLoaded("WP_KeyMaster")){_XLOG_
							sText+=_l("Single hotkey can be assigned to one action only");
						}
						sText+="'"+sHotKeyDsc.GetDsc()+"'";
						ShowBaloon(sText,_l("Used hotkey")+": "+sHotKeyName);
					}
					if(sHotKeyDsc.bOnUnpress==0){_XLOG_
						if(!wk.GetHotkeyInputInProgress()->m_bPromiscON){_XLOG_
							wk.GetHotkeyInputInProgress()->RevertStartupValue(hLastHotkeyPressed);
						}else{_XLOG_
							//TRACE2("``````````onhk:%i, %i\n",sHotKeyDsc.bOnUnpress,sHotKeyDsc.dwLastKeyThatActivatedHK);
							wk.GetHotkeyInputInProgress()->PostMessage(WM_HOTKEY,sHotKeyDsc.dwLastKeyThatActivatedHK,1);
						}
					}
				}
			}
			return 1;
		}
		objSettings.lStat_hotkCount++;
		SimpleTracker tr(lHotkeyActionInProgress);
		CString sNewTitle;
		int iHotKey=((DWORD)wParam)&CODE_NOPREFIX;
		GetActiveWindowTitle(sNewTitle, objSettings.ActiveWndRect,NULL,objSettings.lDirectMouseToUnderC && hLastHotkeyPressed.ContainMouseClicks(), 0);
		m_STray.hLastActiveWindow=objSettings.hHotMenuWnd;
		// Здесь нельзя ставить Lock на паралельное вычисление!
		// Сначала проверяем можно ли сделать что-то, что не будет работать в другом десктопе...
		if(iHotKey>=100 && iHotKey<(100+LAST_ACTION_CODE)){_XLOG_
			if((iHotKey-100)==TAKE_SCREENSHOT){_XLOG_
				pMainDialogWindow->PerformTrayAction(iHotKey-100, TRUE);
				return 0;
			}
		}
		if(iHotKey>=HK_MACRO && iHotKey<=(HK_MACRO+MAX_QRUNAPPLICATION)){_XLOG_
			PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+(iHotKey-HK_MACRO),QRUN_NOFOREGR|QRUN_BYHOTKEY);
			return 0;
		}
		if(iHotKey>=HK_EXTRAMACRO && iHotKey<=(HK_EXTRAMACRO+MAX_EXTRAMAC)){_XLOG_
			PostMessage(WM_COMMAND,WM_USER+QRUNAPP_EXTRA+(iHotKey-HK_EXTRAMACRO),QRUN_NOFOREGR|QRUN_BYHOTKEY);
			return 0;
		}
		if(iHotKey>=HK_SWITBASE && iHotKey<=(HK_SWITBASE+MAX_DESKTOPS)){_XLOG_
			PostMessage(WM_COMMAND,WM_USER+SWITCH_DESKTOP+(iHotKey-HK_SWITBASE),1);
			return 0;
		}
		if(iHotKey>=HK_HISTCL && iHotKey<=(HK_HISTCL+MAX_CLIPS)){_XLOG_
			PostMessage(WM_COMMAND,WM_USER+ID_CLIPHISTORY+(iHotKey-HK_HISTCL),0);
			return 0;
		}
		if(iHotKey>=HK_HISTCLAPP && iHotKey<=(HK_HISTCLAPP+MAX_CLIPS)){_XLOG_
			PostMessage(WM_COMMAND,WM_USER+ID_CLIPHISTORY+(iHotKey-HK_HISTCLAPP),2);
			return 0;
		}
		if(iHotKey>=HK_LAYOBASE && iHotKey<=(HK_LAYOBASE+MAX_LAYOUTS)){_XLOG_
			SetKBLayout(objSettings.aKBLayouts[iHotKey-HK_LAYOBASE]);
			return 0;
		}
		LOGSTAMP
		// Все остальное должно работать в других десктопах as well
		FORK(RunHotkeyInThread,wParam);
	}
	return 0;
}

BOOL AppMainDlg::RegisterMyHotkey(CIHotkey& oHotkey,DWORD dwCode,const char* szId,const char* szDsc, CString& sFailed, int& iCount)
{_XLOG_
	__FLOWLOG
	{// Регистрируем всегда...
		BOOL bReg=RegisterIHotKey(this->m_hWnd,dwCode,oHotkey,szId,szDsc,FALSE);
		if(oHotkey.Present()){_XLOG_
			if(!bReg){_XLOG_
				sFailed+=GetIHotkeyName(oHotkey);
				sFailed+=", ";
				return FALSE;
			}
			iCount++;
		}
	}
	return TRUE;
}

void AppMainDlg::RegisterMyHotKeys(BOOL bSilently)
{_XLOG_
if(spyMode()){
	return;
}
	__FLOWLOG
	//Регистрируем горячие клавиши
	int i=0;
	int iCount=0;
	BOOL bRes=TRUE;
	CString sFailed="";
	// Default action map
	for(i=0;i<(sizeof(objSettings.DefActionMapHK)/sizeof(objSettings.DefActionMapHK[0]));i++){_XLOG_
		CString sDsc=wk.GetBasicHotkeyDsc(i);
		if((i>=WS_SHOWSTART && i<=WS_SHOWUTILM) && GetModuleHandle("wp_wtraits.wkp")==0){_XLOG_
			continue;
		}
		if(RegisterMyHotkey(objSettings.DefActionMapHK[i],100+i,Format("@MAIN%i",i),sDsc,sFailed,iCount)){_XLOG_
			bRes&=TRUE;
		}else{_XLOG_
			bRes=FALSE;
		}
	}
	{// Quick run
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		for(i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			if(objSettings.aQuickRunApps[i]){_XLOG_
				objSettings.aQuickRunApps[i]->qrHotkeys[0]->dwHotkeyID=HK_MACRO+i;
				if(RegisterMyHotkey(objSettings.aQuickRunApps[i]->qrHotkeys[0]->oHotKey,objSettings.aQuickRunApps[i]->qrHotkeys[0]->dwHotkeyID,wk.GetQrunKeyDsc(objSettings.aQuickRunApps[i],0),objSettings.aQuickRunApps[i]->sItemName,sFailed,iCount)){_XLOG_
					bRes&=TRUE;
				}else{_XLOG_
					bRes=FALSE;
				}
				// Дополнительные клавиши макроса...
				for(int j=1;j<objSettings.aQuickRunApps[i]->qrHotkeys.GetSize();j++){_XLOG_
					objSettings.aQuickRunApps[i]->qrHotkeys[j]->dwHotkeyID=CQuickRunItem::lExtraMscroHotkeysIDs++;
					if(RegisterMyHotkey(objSettings.aQuickRunApps[i]->qrHotkeys[j]->oHotKey,objSettings.aQuickRunApps[i]->qrHotkeys[j]->dwHotkeyID,wk.GetQrunKeyDsc(objSettings.aQuickRunApps[i],j),objSettings.aQuickRunApps[i]->sItemName,sFailed,iCount)){_XLOG_
						bRes&=TRUE;
					}else{_XLOG_
						bRes=FALSE;
					}
				}
			}
		}
	}
	// Additional clipboards
	if(objSettings.bAddClipboard){_XLOG_
		for(i=1;i<=objSettings.lClipCount;i++){_XLOG_
			if(RegisterMyHotkey(objSettings.DefClipMapHK[i*2],300+i,Format("@CLIPIN%i",i),Format("%s #%i - %s",_l("Clipboard"),i,_l("Copy")),sFailed,iCount)){_XLOG_
				bRes&=TRUE;
			}else{_XLOG_
				bRes=FALSE;
			}
		}
		for(i=1;i<=objSettings.lClipCount;i++){_XLOG_
			if(RegisterMyHotkey(objSettings.DefClipMapHK[i*2+1],400+i,Format("@CLIPOUT%i",i),Format("%s #%i - %s",_l("Clipboard"),i,_l("Paste")),sFailed,iCount)){_XLOG_
				bRes&=TRUE;
			}else{_XLOG_
				bRes=FALSE;
			}
		}
		for(i=0;i<=objSettings.lClipCount;i++){_XLOG_
			CString sName=Format("%s #%i - %s",_l("Clipboard"),i,_l("History"));
			if(i==0){_XLOG_
				sName=_l("Standard clipboard - history");
			}
			if(RegisterMyHotkey(objSettings.DefClipMapHK_H[i],HK_HISTCL+i,Format("@CLIPHST%i",i),sName,sFailed,iCount)){_XLOG_
				bRes&=TRUE;
			}else{_XLOG_
				bRes=FALSE;
			}
			if(RegisterMyHotkey(objSettings.DefClipMapHK_HA[i],HK_HISTCLAPP+i,Format("@CLIPHSTA%i",i),sName,sFailed,iCount)){_XLOG_
				bRes&=TRUE;
			}else{_XLOG_
				bRes=FALSE;
			}
		}
	}
	// Additional desktops
	if(objSettings.lEnableAddDesktops){_XLOG_
		for(i=0;i<=objSettings.lDesktopSwitches;i++){_XLOG_
			if(RegisterMyHotkey(objSettings.DefDSwitchMapHK[i],HK_SWITBASE+i,Format("@DESKT%i",i),wk.GetDesktopName(i),sFailed,iCount)){_XLOG_
				bRes&=TRUE;
			}else{_XLOG_
				bRes=FALSE;
			}
		}
	}
	// Layouts
	for(i=0;i<=MAX_HKL;i++){_XLOG_
		if(objSettings.aKBLayouts[i]>0){_XLOG_
			if(RegisterMyHotkey(objSettings.aKBLayoutKeys[i],HK_LAYOBASE+i,Format("@LAYOUT%i",i),wk.GetHKLName(i),sFailed,iCount)){_XLOG_
				bRes&=TRUE;
			}else{_XLOG_
				bRes=FALSE;
			}
		}else{_XLOG_
			break;
		}
	}
	// Plugins
	for(i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
		if(!objSettings.plugins[i]){_XLOG_
			continue;
		}
		if(!objSettings.plugins[i]->bIsActive){_XLOG_
			continue;
		}
		for(int j=0;j<objSettings.plugins[i]->funcs.GetSize();j++){_XLOG_
			CString sActDsc=objSettings.plugins[i]->funcs[j].szItemName;
			if(sActDsc.Find("HINT:SOFT")!=-1){_XLOG_
				objSettings.plugins[i]->funcs[j].oRealHotkey.Options()|=ADDOPT_SOFT_EVENTS;
			}
			if(sActDsc.Find("HINT:UNPRESS")!=-1){_XLOG_
				objSettings.plugins[i]->funcs[j].oRealHotkey.Options()|=ADDOPT_ON_UNPRESS;
			}
			if(sActDsc.Find("HINT:PASST")!=-1){_XLOG_
				objSettings.plugins[i]->funcs[j].oRealHotkey.Options()|=ADDOPT_TRANSPARENT;
			}
			if(sActDsc.Find("HINT:SEQ")!=-1){_XLOG_
				objSettings.plugins[i]->funcs[j].oRealHotkey.Options()|=ADDOPT_SEQUENCE;
			}
			if(sActDsc.Find("HINT:PUP")!=-1){_XLOG_
				objSettings.plugins[i]->funcs[j].oRealHotkey.Options()|=ADDOPT_ON_WPRESSED;
			}
			sActDsc=sActDsc.SpanExcluding("\t");
			sActDsc=objSettings.plugins[i]->_l(sActDsc);
			if(RegisterMyHotkey(objSettings.plugins[i]->funcs[j].oRealHotkey,HK_PLUGBASE+(objSettings.plugins[i]->funcs[j].dwCommandCode-WM_USER-PLUGIN_FUNCTION),Format("@PLUGIN-%s-%i",objSettings.plugins[i]->sFileName,j),sActDsc,sFailed,iCount)){_XLOG_
				bRes&=TRUE;
			}else{_XLOG_
				bRes=FALSE;
			}
		}
	}
	if(!bSilently){_XLOG_
		// Окончание
		if(!objSettings.bSkipHKWarning){_XLOG_
			if(bRes!=TRUE){_XLOG_
				sFailed.TrimRight(", ");
				Alert(_l("Some hotkeys can not be registered","Some hotkeys can not be registered! Probably they are used\nby another application.\nList of the conflicting hotkeys")+":\n"+TrimMessage(sFailed,70,4),_l(PROGNAME": Hotkeys"));
			}
		}
	}
	GetListOfCurrentlyRegisteredHotkeys(TRUE);
	lHotkeysState=1;
	SetWindowIcon();
}

void AppMainDlg::UnRegisterMyHotKeys(BOOL bNeedRecreate)
{_XLOG_
if(spyMode()){
	return;
}
	__FLOWLOG
	// Показываем что клавиши не работают...
	m_STray.SetIcon(IDR_MAINICOX);
	lHotkeysState=0;
	int i=0;
	//Разрегистрируем горячие клавиши
	for(i=0;i<=objSettings.lClipCount;i++){_XLOG_
		UnregisterIHotKey(this->m_hWnd,300+i);
		UnregisterIHotKey(this->m_hWnd,400+i);
		UnregisterIHotKey(this->m_hWnd,HK_HISTCL+i);
		UnregisterIHotKey(this->m_hWnd,HK_HISTCLAPP+i);
	}
	for(i=0;i<=objSettings.lDesktopSwitches;i++){_XLOG_
		UnregisterIHotKey(this->m_hWnd,HK_SWITBASE+i);
	}
	for(i=0;i<=MAX_HKL;i++){_XLOG_
		if(objSettings.aKBLayouts[i]>0){_XLOG_
			UnregisterIHotKey(this->m_hWnd,HK_LAYOBASE+i);
		}else{_XLOG_
			break;
		}
	}
	for(i=100;i<=(100+(sizeof(objSettings.DefActionMapHK)/sizeof(objSettings.DefActionMapHK[0])));i++){_XLOG_
		UnregisterIHotKey(this->m_hWnd,i);
	}
	{_XLOG_
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		for(i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			//if(objSettings.aQuickRunApps[i] && objSettings.aQuickRunApps[i]->dwHotKey!=0){_XLOG_
			//Удаляем все - после сдвигов могут появится "лакуны":
			//dwHotKey!=0 не на тех приложениях, на которых это регилось сначала
			UnregisterIHotKey(this->m_hWnd,HK_MACRO+i);
		}
		for(i=HK_EXTRAMACRO;i<CQuickRunItem::lExtraMscroHotkeysIDs;i++){_XLOG_
			//if(objSettings.aQuickRunApps[i] && objSettings.aQuickRunApps[i]->dwHotKey!=0){_XLOG_
			//Удаляем все - после сдвигов могут появится "лакуны":
			//dwHotKey!=0 не на тех приложениях, на которых это регилось сначала
			UnregisterIHotKey(this->m_hWnd,i);
		}
	}
	DWORD dwMaxPluginsKeys=WKPluginFunctionDscEx::dwCommandCodeCount-WM_USER+PLUGIN_FUNCTION;
	for(i=0; DWORD(i)<dwMaxPluginsKeys; i++){_XLOG_
		UnregisterIHotKey(this->m_hWnd,HK_PLUGBASE+i);
	}
	// Сбрасываем счетчик для доп.клавиш макросов...
	CQuickRunItem::lExtraMscroHotkeysIDs=HK_EXTRAMACRO;
	if(bNeedRecreate){_XLOG_
		GetListOfCurrentlyRegisteredHotkeys(TRUE);
	}
}

BOOL checkFirstStr(const char* szIn, const char* szCharSet){_XLOG_
	int len=strlen(szIn)-1,k=strlen(szCharSet);
	if(len<=0 || len<k){_XLOG_
		return FALSE;
	}
	BOOL bFlag=TRUE;
	for(int j=0;j<k;j++){_XLOG_
		if(*(szIn+j)!=*(szCharSet+j)){_XLOG_
			bFlag=FALSE;
			break;
		}
	}
	return bFlag;
};

void AppMainDlg::AWndManagerMain(HWND forWnd, BOOL bTryToGetCaret, int iMenu)
{_XLOG_
	static HMENU hLastHotMenu=0;
	if(objSettings.lHotMenuOpened>0){_XLOG_
		if(iMenu==99){
			// Список процессов - скрываем если уже открыт
			if(IsMenu(hLastHotMenu)){
				EndMenu();
			}
		}
		return;
	}
	SimpleTracker tr(objSettings.lHotMenuOpened);
	DWORD dwProcID=0;
	CString sProcessIcon;
	CMenu menu;
	menu.CreatePopupMenu();
	aNum2IDMap aSysWndMenuMap;
	if(iMenu==99){_XLOG_
		AppendProcessesToMenu(menu,sProcessIcon,dwProcID);
	}else{_XLOG_
		// Получаем данные активного окна
		CString sNewTitle;
		if(forWnd==NULL){_XLOG_
			forWnd=objSettings.hHotMenuWnd;
		}else{_XLOG_
			objSettings.hHotMenuWnd=forWnd;
		}
		GetWindowTitleAndData(forWnd,sNewTitle,objSettings.ActiveWndRect);
		m_STray.hLastActiveWindow=forWnd;
		// Звучим и делаем вещи	
		//AsyncPlaySoundSys("Menu popup");
		BOOL bActiveWindowPresent=TRUE;
		if(forWnd==NULL || sNewTitle==""){_XLOG_
			bActiveWindowPresent=FALSE;
		}
		if(bActiveWindowPresent){_XLOG_
			DWORD dwThread=GetWindowThreadProcessId(forWnd,&dwProcID);
		}
		if(iMenu==1){_XLOG_
			AppendQRunToMenu(&menu,WKITEMPOSITION_DBLHOTMENU|WKITEMPOSITION_HOTMENU);
		}else if(iMenu==2){_XLOG_
			AppendMacrosToMenu(&menu,WKITEMPOSITION_DBLHOTMENU|WKITEMPOSITION_HOTMENU);
		}else if(iMenu==3){_XLOG_
			AppendDeskactionsToMenu(&menu,WKITEMPOSITION_DBLHOTMENU|WKITEMPOSITION_HOTMENU);
		}else if(iMenu==4){_XLOG_
			AppendMiscSaverToMenu(&menu,WKITEMPOSITION_DBLHOTMENU|WKITEMPOSITION_HOTMENU);
		}else if(iMenu==5){_XLOG_
			AppendSelTextToMenu(&menu,bActiveWindowPresent,WKITEMPOSITION_DBLHOTMENU|WKITEMPOSITION_HOTMENU);
		}else{_XLOG_
			CMenu SysMenu;
			SysMenu.CreatePopupMenu();
			CMenu SysWndMenu;
			SysWndMenu.CreatePopupMenu();
			if(objSettings.bSysActionsInHotMenu!=3){_XLOG_
				if(bActiveWindowPresent){_XLOG_
					AddMenuString(&SysMenu,ID_HOTMENU_KILLPROCESS,_l("Kill application"),_bmp().Get(IDB_BM_DEL));
					if(!objSettings.bUnderWindows98){_XLOG_
						// Trans
						CMenu TransMenu;
						TransMenu.CreatePopupMenu();
						AddMenuString(&TransMenu,ID_HOTMENU_TRANSPARENCY20,"20%",(CBitmap*)NULL);
						AddMenuString(&TransMenu,ID_HOTMENU_TRANSPARENCY40,"40%",(CBitmap*)NULL);
						AddMenuString(&TransMenu,ID_HOTMENU_TRANSPARENCY60,"60%",(CBitmap*)NULL);
						AddMenuString(&TransMenu,ID_HOTMENU_TRANSPARENCY80,"80%",(CBitmap*)NULL);
						AddMenuString(&TransMenu,ID_HOTMENU_TRANSPARENCY100,"100%",(CBitmap*)NULL);
						AddMenuSubmenu(&SysMenu,&TransMenu,_l("Set opacity")+"\t...",_bmp().Get(_IL(51)));
					}
					CMenu PrSysMenu;
					PrSysMenu.CreatePopupMenu();
					HANDLE hActiveProc=::OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,dwProcID);
					DWORD dwPrior=NORMAL_PRIORITY_CLASS;
					if (!( (hActiveProc) == NULL || (hActiveProc) == INVALID_HANDLE_VALUE )){_XLOG_
						dwPrior=GetPriorityClass(hActiveProc);
						CloseHandle(hActiveProc);
					}
					AddMenuString(&PrSysMenu,WM_USER+ACTIVEAPP_PRIOR+4,_l("Low"),NULL,dwPrior==IDLE_PRIORITY_CLASS);
					AddMenuString(&PrSysMenu,WM_USER+ACTIVEAPP_PRIOR+3,_l("Normal"),NULL,dwPrior==NORMAL_PRIORITY_CLASS);
					AddMenuString(&PrSysMenu,WM_USER+ACTIVEAPP_PRIOR+1,_l("High"),NULL,dwPrior==HIGH_PRIORITY_CLASS);
					AddMenuSubmenu(&SysMenu,&PrSysMenu,_l("Application priority")+"\t...",_bmp().Get(_IL(33)));
				}
				if(objSettings.bSysActionsInHotMenu==0 || objSettings.bSysActionsInHotMenu==1){_XLOG_
					CMenu ProcMenu;
					ProcMenu.CreatePopupMenu();
					AppendProcessesToMenu(ProcMenu,sProcessIcon,dwProcID);
					AddMenuSubmenu(&SysMenu,&ProcMenu,_l("Processes")+"\t...",_bmp().Get(_IL(33)));
				}
				if(bActiveWindowPresent){_XLOG_
					// Show active app exe-path
					AddMenuString(&SysMenu,ID_HOTMENU_SHOWEXEPATH,_l("Show application info"),_bmp().Get(_IL(13)));
				}
				// Free memory
				if(objSettings.bUnderWindowsNT){_XLOG_
					AddMenuString(&SysMenu,ID_HOTMENU_FREEMEMORY,_l("Free physical memory")+getActionHotKeyDsc(FREE_MEMORY),_bmp().Get(_IL(23)));
				}
			}
			if(sProcessIcon==""){_XLOG_
				sProcessIcon="HOTMENU_ICON";
				HICON hIcon=NULL;
				GetWndIcon(forWnd,hIcon);
				if(hIcon){_XLOG_
					_bmp().AddBmp(sProcessIcon,hIcon);
				}else{_XLOG_
					CProcInfo* pInfo=NULL;
					GetProcessInfo(dwProcID,pInfo);
					if(pInfo && pInfo->sIconIndex!=""){_XLOG_
						sProcessIcon=pInfo->sIconIndex;
					}else{_XLOG_
						sProcessIcon=_IL(APP_ICONS_OFFSET);
					}
				}
			}
			if(bActiveWindowPresent){_XLOG_
				BOOL bTitleAppended=FALSE;
				if(objSettings.bRetrieveSysMenu){_XLOG_
					HMENU hWndSysMenu=::GetSystemMenu(forWnd,FALSE);
					if(::IsMenu(hWndSysMenu)){_XLOG_
						BOOL bOwnerDraws=0;
						::SendMessage(forWnd,WM_INITMENU,WPARAM(hWndSysMenu),0);
						AppendClonedMenu(&SysWndMenu,hWndSysMenu,WM_USER+SYSMENU_FUNCTION,&bOwnerDraws,&aSysWndMenuMap);
						if(SysWndMenu.GetMenuItemCount()>0){_XLOG_
							AddMenuSubmenu(&menu,&SysWndMenu,TrimMessage(sNewTitle,25),_bmp().Get(sProcessIcon));
							bTitleAppended=TRUE;
						}
					}else{_XLOG_
						DWORD dwErr=GetLastError();
					}
				}
				if(!bTitleAppended){_XLOG_
					AddMenuString(&menu,0,TrimMessage(sNewTitle,25),_bmp().Get(sProcessIcon));
				}
				menu.AppendMenu(MF_SEPARATOR, 0, "");
				if(!::IsZoomed(forWnd) && CSmartWndSizer::TestOnScreenOut(objSettings.ActiveWndRect,objSettings.iStickPix)){_XLOG_
					AddMenuString(&menu,ID_HOTMENU_BRINGINVIEW,_l("Move window into view")+getActionHotKeyDsc(BRING_INTOVIEW),_bmp().Get(_IL(14)));
				}
				// To tray
				AddMenuString(&menu,ID_HOTMENU_HIDETOTRAY,_l("Hide application to tray")+getActionHotKeyDsc(HIDE_TOTRAY),_bmp().Get(_IL(59)));
				AddMenuString(&menu,ID_HOTMENU_HIDECOMPLETELY,_l("Hide application completely")+getActionHotKeyDsc(HIDE_COMPLIT),_bmp().Get(_IL(60)));
				// Pin/unpin
				BOOL bTopmost=(GetWindowLong(forWnd, GWL_EXSTYLE) & WS_EX_TOPMOST);
				AddMenuString(&menu,ID_HOTMENU_SWITCHONTOP,(bTopmost?_l("Unpin window"):_l("Pin window"))+getActionHotKeyDsc(SWITCH_ONTOP),bTopmost?_bmp().Get(IDB_PINNED_UP):_bmp().Get(IDB_PINNED_DOWN));
				if(objSettings.bAppShortcInHotMenu){// Быстрый доступ
					CMenu SCutMenu;
					SCutMenu.CreatePopupMenu();
					AddMenuString(&SCutMenu,ID_HOTMENU_ADDTOQUICKRUNS,		_l("Add to quick-runs"),_bmp().Get(_IL(32)));
					AddMenuString(&SCutMenu,ID_HOTMENU_ADDTOAHIDE,			_l("Add to auto-hide"));
					AddMenuString(&SCutMenu,ID_HOTMENU_SHORTCUTONDESKTOP,	_l("Add to Desktop"));
					AddMenuString(&SCutMenu,ID_HOTMENU_SHORTCUTONSTARTMENU,	_l("Add to Start menu"));
					AddMenuString(&SCutMenu,ID_HOTMENU_SHORTCUTONQLAUNCH,	_l("Add to Quick Launch"));
					AddMenuString(&SCutMenu,ID_HOTMENU_SHORTCUTONUDLOCATION,_l("Choose shortcut location")+"\t...",_bmp().Get(_IL(5)));
					AddMenuSubmenu(&menu,&SCutMenu,_l("Application shortcut")+"\t...",_bmp().Get(_IL(22)));
				}
			}
			// Системные действия
			if(objSettings.bSysActionsInHotMenu!=3){_XLOG_
				AddMenuSubmenu(&menu,&SysMenu,_l("System actions")+"\t...",_bmp().Get(_IL(20)));
			}
			// Операции над выделенным текстом
			if(objSettings.bTextActionsInHotMenu){_XLOG_
				if(bActiveWindowPresent){_XLOG_
					AppendSelTextToMenu(&menu,bActiveWindowPresent,WKITEMPOSITION_HOTMENU);
				}
			}
			menu.AppendMenu(MF_SEPARATOR, 0, "");
			if(objSettings.aHidedWindowsMap.GetSize()>0){// Hidden windows
				AppendHiddenToMenu(&menu);
			}
			AppendMacrosToMenu(&menu,WKITEMPOSITION_HOTMENU);
			AppendQRunToMenu(&menu,WKITEMPOSITION_HOTMENU);
			AppendMiscSaverToMenu(&menu,WKITEMPOSITION_HOTMENU);
			AppendDeskactionsToMenu(&menu,WKITEMPOSITION_HOTMENU);
			AppendVolumeToMenu(&menu,WKITEMPOSITION_HOTMENU);
		}
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		AppendPluginSubmenu(&menu, WKITEMPOSITION_HOTMENU);
		//
		AddMenuString(&menu,ID_SYSTRAY_OPTIONS,_l("Open "PROGNAME)+getActionHotKeyDsc(SHOW_OPTIONS),_bmp().Get(IDB_BM_OPTIONS));
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		menu.SetDefaultItem(0,TRUE);
	}
	AddMenuString(&menu,ID_HOTMENU_EXITMENU,_l("Close menu"),_bmp().Get(_IL(40)));
	CPoint pos;
	if(bTryToGetCaret && iMenu==0){_XLOG_
		pos=GetCaretScreenPos(0);
	}else{_XLOG_
		::GetCursorPos(&pos);
	}
	SimpleTracker Track(objSettings.bDoModalInProcess);
	if(pMainDialogWindow){_XLOG_
		// Делаем активным текущее окно
			HWND hRealFocus=0;
			if(GetModuleHandle("wp_wtraits.wkp")){_XLOG_
				hRealFocus=GetLastFocusWindowByHook(objSettings.hHotMenuWnd);
				/*#ifdef _DEBUG
				char s[100]={0};
				::GetWindowText(hRealFocus,s,sizeof(s));
				char szClass[128]="";
				::GetClassName(hRealFocus,szClass,sizeof(szClass));
				ShowBaloon(Format("Lost focus: 0x%08X %s",hRealFocus,szClass),"WireKeys debug",3000);
				#endif*/
			}
		if(pMainDialogWindow && !objSettings.lDisableFocusMove){_XLOG_
			SetMainWindowForeground(forWnd,NULL,&objSettings.hFocusWnd);
		}
		hLastHotMenu=menu.GetSafeHmenu();
		DWORD dwMessage=::TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD, pos.x, pos.y, 0, pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL, NULL);//|TPM_NONOTIFY
		if(dwMessage>=WM_USER+SYSMENU_FUNCTION && (dwMessage-(WM_USER+SYSMENU_FUNCTION))<DWORD(aSysWndMenuMap.GetSize())){_XLOG_
			DWORD dwID=aSysWndMenuMap[dwMessage-(WM_USER+SYSMENU_FUNCTION)];
			HWND hWin2=forWnd;
			GetWindowTopParent(hWin2);
			POINT p;
			::GetCursorPos(&p);
			::PostMessage(hWin2,WM_SYSCOMMAND,dwID,MAKELONG(p.x,p.y));
			return;
		}
		if(hRealFocus && objSettings.hHotMenuWnd==0){_XLOG_
			objSettings.hHotMenuWnd=hRealFocus;
			objSettings.hFocusWnd=hRealFocus;
		}
		if(forWnd){_XLOG_
			objSettings.hHotMenuWnd=forWnd;
		}
		if(!objSettings.lDisableFocusMove || hRealFocus!=0){_XLOG_
			SetMainWindowForeground(objSettings.hHotMenuWnd,objSettings.hHotMenuWnd,&objSettings.hFocusWnd);
		}
		if(dwMessage==ID_HOTMENU_SETASCLIPTARGET){_XLOG_
			objSettings.lClipTransmitMode=!objSettings.lClipTransmitMode;
			objSettings.hWndToTransmitTo=NULL;
			if(objSettings.lClipTransmitMode){_XLOG_
				BOOL bUseWMCOPY=FALSE;
				if(PrepareClipProcesses(bUseWMCOPY)){_XLOG_
					objSettings.hWndToTransmitTo=objSettings.hFocusWnd;
					if(objSettings.hWndToTransmitTo==NULL){_XLOG_
						objSettings.hWndToTransmitTo=forWnd;
					}
					FinishClipProcesses();
					CString sOutText=CString("\r\n*** "PROGNAME" - ")+_l("Transmitted text")+": ***\r\n";
					FORK(TransmitTextToTarget,strdup(sOutText));
					Alert(_l("Copy anything to additional clipboard\r\nto transmit it to this window"),9000,FALSE,DT_CENTER|DT_VCENTER);
				}
			}
		}else if(dwMessage>0){_XLOG_
			pMainDialogWindow->OnCommand(dwMessage,0);
		}
	}
	return;
}

void AppMainDlg::ShowKeyBindings()
{_XLOG_
	GenerateBindingsFile();
	CString sFile=getFileFullName("KeyBindings.txt");
	int iRes=(int)::ShellExecute(NULL,"open",sFile,NULL,NULL,SW_SHOWNORMAL);
	if(iRes<=32){_XLOG_
		::ShellExecute(NULL,"","notepad.exe",sFile,NULL,SW_SHOWNORMAL);
	}
}

CString MakeDoublePressKey(CString sText)
{_XLOG_
	int iLen=sText.GetLength();
	if(iLen==0){_XLOG_
		return "";
	}
	//CString sTextM1=sText.Left(iLen-1);
	int iDPos=sText.ReverseFind('-');
	if(iDPos!=-1){_XLOG_
		sText+=sText.Mid(iDPos);
	}
	return sText;
}

void AppMainDlg::GenerateBindingsFile()
{_XLOG_
	CoInitialize(NULL);
	int iCount=1;
	CString sOut;
	sOut+="**************************************\r\n";
	sOut+=Format(PROGNAME" %i.%i.%i: ",VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD);
	sOut+=_l("List of all hotkeys");
	sOut+="\r\n";
	sOut+="**************************************\r\n";
	sOut+="\r\n";
	sOut+=_l("Common operations")+"\r\n";
	sOut+="--------------------------------------\r\n";
	sOut+="\r\n";
	int i=0;
	for(i=0;i<=LAST_ACTION_CODE;i++){_XLOG_
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefActionMapHK[i]),25);
		sOut+=wk.GetBasicHotkeyDsc(i)+getHotKeyStrAlt(objSettings.DefActionMapHK[i],CString("; ")+_l("Alternative hotkey(s)")+": ");
		sOut+="\r\n";
	}
	sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
	sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefClipMapHK_H[0]),25);
	sOut+=_l("Show history for standard clipboard")+getHotKeyStrAlt(objSettings.DefClipMapHK_H[0],CString("; ")+_l("Alternative hotkey(s)")+": ");
	sOut+="\r\n";
	sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
	sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefClipMapHK_HA[0]),25);
	sOut+=_l("Show application-specific history for standard clipboard")+getHotKeyStrAlt(objSettings.DefClipMapHK_H[0],CString("; ")+_l("Alternative hotkey(s)")+": ");
	if(!objSettings.lDisableDblPressExtensions){_XLOG_
		sOut+="\r\n";
		sOut+="\r\n";
		sOut+=_l("Operations on hotkey`s double-press")+"\r\n";
		sOut+="--------------------------------------\r\n";
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[WINDOW_MINIMIZE])),25);
		sOut+=_l("Restore last minimized");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(WINDOW_MINIMIZE);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[OPEN_CD])),25);
		sOut+=_l("Load/Eject second CD-Drive");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(OPEN_CD);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[RUN_ASCOMMAND])),25);
		sOut+=_l("History of runned strings");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(RUN_ASCOMMAND);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[TAKE_SCREENSHOT])),25);
		sOut+=_l("Alternative screenshot");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(TAKE_SCREENSHOT);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[BOSS_KEY])),25);
		sOut+=_l("Alternative boss-key action");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(BOSS_KEY);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[HIDE_TOTRAY])),25);
		sOut+=_l("Unhide last hidden window");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(HIDE_TOTRAY);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[HIDE_TOFLOAT])),25);
		sOut+=_l("Unhide last hidden window");//Hide active window into floater
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(HIDE_TOFLOAT);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[HIDE_COMPLIT])),25);
		sOut+=_l("Unhide last hidden window");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(HIDE_COMPLIT);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[KILL_ACTWND])),25);
		sOut+=_l("Terminate active application");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(KILL_ACTWND);
		sOut+="\r\n";
		//------------------
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(MakeDoublePressKey(getHotKeyStr(objSettings.DefActionMapHK[HOT_MENU])),25);
		sOut+=_l("Alternative hot menu");
		sOut+=", ";
		sOut+=_l("Base action")+": ";
		sOut+=wk.GetBasicHotkeyDsc(HOT_MENU);
		sOut+="\r\n";
	}
	sOut+="\r\n";
	sOut+="\r\n";
	if(objSettings.lClipCount>0){_XLOG_
		sOut+="\r\n";
		sOut+=_l("Additional clipboards");
		if(!objSettings.bAddClipboard){_XLOG_
			sOut+="\r\n";
			sOut+="(";
			sOut+=_l("Disabled, check 'Preferences' to enable");
			sOut+=")";
		}
		sOut+="\r\n";
		sOut+="--------------------------------------\r\n";
		sOut+="\r\n";
		for(i=1;i<=objSettings.lClipCount;i++){_XLOG_
			CString sClipTitle=_l("Clip")+Format(" #%i - ",i);
			sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
			sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefClipMapHK[i*2]),25);
			sOut+=sClipTitle+_l("'Copy' hot key");
			sOut+="\r\n";
			sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
			sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefClipMapHK[i*2+1]),25);
			sOut+=sClipTitle+_l("'Paste' hot key");
			sOut+="\r\n";
			sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
			sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefClipMapHK_H[i]),25);
			sOut+=sClipTitle+_l("Show history");
			sOut+="\r\n";
			sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
			sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefClipMapHK_HA[i]),25);
			sOut+=sClipTitle+_l("Show application-specific history");
			sOut+="\r\n";
		}
	}
	if(objSettings.aQuickRunApps.GetSize()>0){_XLOG_
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		sOut+="\r\n";
		sOut+=_l("Quick run applications")+"/"+_l("Macros")+"\r\n";
		sOut+="--------------------------------------\r\n";
		sOut+="\r\n";
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			if(objSettings.aQuickRunApps[i]){_XLOG_
				sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
				sOut+=MakeStandartLen(getHotKeyStr(objSettings.aQuickRunApps[i]->qrHotkeys[0]->oHotKey),25);
				sOut+=objSettings.aQuickRunApps[i]->sItemName+getHotKeyStrAlt(objSettings.aQuickRunApps[i]->qrHotkeys[0]->oHotKey,CString("; ")+_l("Alternative hotkey(s)")+": ");
				if(objSettings.aQuickRunApps[i]->qrHotkeys.GetSize()>1){_XLOG_
					sOut+="; ";
					sOut+=_l("More hotkeys")+": ";
					for(int j=1;j<objSettings.aQuickRunApps[i]->qrHotkeys.GetSize();j++){_XLOG_
						if(j>1){_XLOG_
							sOut+=", ";
						}
						sOut+=getHotKeyStr(objSettings.aQuickRunApps[i]->qrHotkeys[j]->oHotKey);
					}
				}
				sOut+="\r\n";
			}
		}
	}
	if(objSettings.lDesktopSwitches>0){_XLOG_
		sOut+="\r\n";
		sOut+=_l("Additional desktops");
		if(!objSettings.lEnableAddDesktops){_XLOG_
			sOut+="\r\n";
			sOut+="(";
			sOut+=_l("Disabled, check 'Preferences' to enable");
			sOut+=")";
		}
		sOut+="\r\n";
		sOut+="--------------------------------------\r\n";
		sOut+="\r\n";
		for(i=0;i<objSettings.lDesktopSwitches;i++){_XLOG_
			sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
			sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefDSwitchMapHK[i]),25);
			sOut+=wk.GetDesktopName(i);
			sOut+="\r\n";
		}
		// Prev desktop
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefActionMapHK[ADDDESK_PREV]),25);
		sOut+=wk.GetBasicHotkeyDsc(ADDDESK_PREV);
		sOut+="\r\n";
		// Next desktop
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefActionMapHK[ADDDESK_NEXT]),25);
		sOut+=wk.GetBasicHotkeyDsc(ADDDESK_NEXT);
		sOut+="\r\n";
	}
	{// Ракладки клавиатуры
		sOut+="\r\n";
		sOut+=_l("Keyboard layout")+"\r\n";
		sOut+="--------------------------------------\r\n";
		sOut+="\r\n";
		// Next layout
		sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
		sOut+=MakeStandartLen(getHotKeyStr(objSettings.DefActionMapHK[SWITCHLAYOUT]),25);
		sOut+=wk.GetBasicHotkeyDsc(SWITCHLAYOUT);
		sOut+="\r\n";
		for(int i=0;i<MAX_HKL;i++){_XLOG_
			if(objSettings.aKBLayouts[i]){_XLOG_
				sOut+=MakeStandartLen(Format("%lu. ",iCount++),4);
				sOut+=MakeStandartLen(getHotKeyStr(objSettings.aKBLayoutKeys[i]),25);
				sOut+=_l("Layout")+": "+wk.GetHKLName(i);
				sOut+="\r\n";
			}else{_XLOG_
				break;
			}
		}
	}
	{// Шорткаты с рабочего стола
		CString sDeskCuts="";
		CString sDeskpath=getDesktopPath();
		CFileInfoArray dir;
		dir.AddDir(sDeskpath+"\\","*.lnk",TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
		int iSize=dir.GetSize();
		for(int i=0;i<iSize;i++){ 
			HRESULT hres; 
			IShellLink* psl; 
			const char* lpszLinkFile=dir[i].GetFilePath();
			// Get a pointer to the IShellLink interface. 
			hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
			if (SUCCEEDED(hres)){ 
				IPersistFile* ppf; 
				// Get a pointer to the IPersistFile interface. 
				hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
				if (SUCCEEDED(hres)){ 
					WCHAR wsz[MAX_PATH];
					// Ensure that the string is Unicode. 
					MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH); 
					// Load the shortcut. 
					hres = ppf->Load(wsz, STGM_READ); 
					if (SUCCEEDED(hres)){ 
						// Resolve the link. 
						WORD wHotkey=0;
						hres = psl->GetHotkey(&wHotkey); 
						if (SUCCEEDED(hres) && wHotkey>0){ 
							sDeskCuts+=MakeStandartLen(Format("%lu. ",iCount++),4);
							CIHotkey hk;
							hk.InitFromWinWORD(wHotkey);
							sDeskCuts+=MakeStandartLen(getHotKeyStr(hk),25);
							char szFileName[MAX_PATH]="";
							_splitpath(lpszLinkFile, NULL, NULL, szFileName, NULL);
							sDeskCuts+=szFileName;
							sDeskCuts+="\r\n";
						}
					}
					// Release the pointer to the IPersistFile interface. 
					ppf->Release(); 
				} 
				// Release the pointer to the IShellLink interface. 
				psl->Release(); 
			} 
		}
		if(sDeskCuts!=""){_XLOG_
			sOut+="\r\n";
			sOut+=_l("Desktop icon`s shortcuts");
			sOut+="\r\n";
			sOut+="--------------------------------------\r\n";
			sOut+="\r\n";
			sOut+=sDeskCuts;
			sOut+="\r\n";
		}
	}
	// Стандартные шорткаты
	CString sWSFileName=getFileFullName(Format("%sWinShortcuts_l%lu.txt",GetApplicationDir(),GetApplicationLang()));
	if(!isFileExist(sWSFileName)){_XLOG_
		sWSFileName=getFileFullName(Format("%sWinShortcuts_l0.txt",GetApplicationDir()));
	}
	CString sShortCutsStand;
	ReadFile(sWSFileName,sShortCutsStand);
	if(sShortCutsStand!=""){_XLOG_
		sOut+="\r\n";
		sOut+=_l("Standard MS Windows shortcuts");
		sOut+="\r\n";
		sOut+="--------------------------------------\r\n";
		sOut+="\r\n";
		sOut+=sShortCutsStand;
		sOut+="\r\n";
	}
	// Сохраняем
	CString sFile=getFileFullName("KeyBindings.txt");
	SaveFile(sFile,sOut);
	CoUninitialize();
	return;
}

void UnhideLastHidden()
{_XLOG_
	CSmartLock SL(&csInfoWnds,TRUE);
	if(pMainDialogWindow && objSettings.aHidedWindowsMap.GetSize()>0){_XLOG_
		int iPos=objSettings.aHidedWindowsMap.GetSize()-1;
		InfoWnd* pW=objSettings.aHidedWindowsMap[iPos];
		if(pW){_XLOG_
			pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+HIDEDWINDOWS+iPos,0);
		}
	}
	return;
}

DWORD WINAPI ShowOSDClocks()
{_XLOG_
	static DWORD dwStopTimerClocks=0;
	BOOL bInCreation=0;
	if(dwStopTimerClocks==0){_XLOG_
		bInCreation=1;
	}
	dwStopTimerClocks=GetTickCount()+3500;
	if(!bInCreation){_XLOG_
		return 0;
	}
	CSplashParams par;
	char szTmp[256]="",szTmp2[256]="";
	FillSplashParams("",par);
	CSplashWindow OSD(&par);
	while(dwStopTimerClocks>0 && GetTickCount()<dwStopTimerClocks){_XLOG_
		if(objSettings.sODSTimeFormat!=""){_XLOG_
			GetTimeFormat(LOCALE_USER_DEFAULT,0,0,objSettings.sODSTimeFormat,szTmp2,sizeof(szTmp2));
		}else{_XLOG_
			GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,0,0,szTmp2,sizeof(szTmp2));
		}
		if(objSettings.sODSDateFormat!=""){_XLOG_
			GetDateFormat(LOCALE_USER_DEFAULT,0,0,objSettings.sODSDateFormat,szTmp,sizeof(szTmp));
		}else{_XLOG_
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,0,0,szTmp,sizeof(szTmp));
		}
		OSD.SetTextOnly(Format("%s %s", szTmp, szTmp2));//Format("%s %s", _strdate(szTmp), _strtime(szTmp2))
		OSD.wnd->Invalidate();
		Sleep(500);
	}
	dwStopTimerClocks=0;
	return 0;
};

int FindSimilarApps(_ProcEnum& procList,CString sName)
{
	int iRes=0;
	for(int i=0;i<procList.GetSize();i++){
		if(procList[i].sProcName==sName){
			iRes++;
		}
	}
	return iRes;
}


int g_iATPos=-1;
HWND g_hNextWindow=0;
/*
BOOL CALLBACK EnumWindowsProcNXT(HWND hwnd, LPARAM lParam)
{_XLOG_

	ALTTABINFO ti;
	memset(&ti,0,sizeof(ti));
	ti.cbSize=sizeof(ti);
	if(g_hNextWindow==0){_XLOG_
		int iPos=ti.iColFocus+ti.iRowFocus*ti.cColumns;
		AfxMessageBox(Format("%i,%i,%i,%i,%i",iPos,ti.iColFocus,ti.iRowFocus,ti.cColumns));
		if(hwnd==(HWND)lParam){_XLOG_
			g_iATPos=iPos;
		}else if(g_iATPos>=0 && iPos==g_iATPos+1){_XLOG_
			g_hNextWindow=hwnd;
		}
	}
	return TRUE;
}
*/
void ShowWinampTitle(BOOL);
long bTrayActionLock=FALSE;
void OnBeforeShutdown();
long g_ShutType=objSettings.lShutdownStrength;
const char* g_sShutType[]={("Normal"),("Auto"),("Extreme"),("Suspend"),("Hibernate")};
BOOL AppMainDlg::PerformTrayAction(int iAction,BOOL bHotKey)
{_XLOG_
	SimpleTracker tr(lHotkeyActionInProgress);// Не зависит от bTrayActionLock, так как локируется паралельно в разных функциях
	// Спец. hotkeys для двойного нажатия
	switch(iAction){_XLOG_
	case CLEARHISTORIES:
		{_XLOG_
			PostMessage(WM_COMMAND,ID_HOTMENU_CLEARCLIPS,0);
			return 0;
		}
	case SHOWPROCLIST:
		{_XLOG_
			PostMessage(WM_COMMAND,FIREHOTMENU,99);
			return 0;
		}
	case HOT_MENU:
		{_XLOG_
			long lHMAction=0;
			IsThreadDoubleCalled(lHMAction);
			if(lHMAction){_XLOG_
				PostMessage(WM_COMMAND,FIREHOTMENU,objSettings.bMenuOnDblHotMenu+1);
			}else{_XLOG_
				PostMessage(WM_COMMAND,FIREHOTMENU,0);
			}
			return 0;
		}
	case KILL_ACTWND:
		{_XLOG_
			long lAction=0;
			IsThreadDoubleCalled(lAction);
			if(lAction){_XLOG_
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_HOTMENU_KILLPROCESS,0);
			}else{_XLOG_
				::PostMessage(objSettings.hHotMenuWnd,WM_CLOSE,0,0);
				//::PostMessage(objSettings.hHotMenuWnd,WM_SYSCOMMAND,SC_CLOSE,0);
				/*if(::SendMessage(objSettings.hHotMenuWnd,WM_SYSCOMMAND,SC_CLOSE,0)!=0){
					::PostMessage(objSettings.hHotMenuWnd,WM_CLOSE,0,0);
				}*/
			}
			return 0;
		}
	case HIDE_TOTRAY:
		{_XLOG_
			long lAction=0;
			IsThreadDoubleCalled(lAction);
			if(lAction){_XLOG_
				UnhideLastHidden();
			}else{_XLOG_
				OnHotmenuHidetotray();
			}
			return 0;
		}
	case HIDE_TOFLOAT:
		{_XLOG_
			long lAction=0;
			IsThreadDoubleCalled(lAction);
			if(lAction){_XLOG_
				UnhideLastHidden();
			}else{_XLOG_
				OnHotmenuHidetotrayDetached();
			}
			return 0;
		}
	case HIDE_COMPLIT:
		{_XLOG_
			long lAction=0;
			IsThreadDoubleCalled(lAction);
			if(lAction){_XLOG_
				UnhideLastHidden();
			}else{_XLOG_
				OnHotmenuHidecompletely();
			}
			return 0;
		}
	case SHOWCLOCKS:
		ShowOSDClocks();
		return 0;
	}
	// Остальные гор. клавиши
	if(bTrayActionLock){_XLOG_
		return 1;
	}
	SimpleTracker lc(bTrayActionLock);
	switch(iAction){_XLOG_
	case WS_SHOWSTART:
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_SYSCOMMAND, SC_TASKLIST, 1);
		break;
	case WS_SYSPROPS:
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)::FindWindow("Shell_TrayWnd",NULL));
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01FD,0);
		break;
	case WS_SHOWDESK:
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01FE,0);
		break;
	case WS_MINIMALL:
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01F5,0);
		break;
	case WS_SHOWMYCOMP:
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)::FindWindow("Shell_TrayWnd",NULL));
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01F8,0);
		break;
	case WS_SEARCHFILE:
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)::FindWindow("Shell_TrayWnd",NULL));
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01F9,0);
		break;
	case WS_SEARCHCOMP:
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)::FindWindow("Shell_TrayWnd",NULL));
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01FA,0);
		break;
	case WS_SHOWWHELP:
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)::FindWindow("Shell_TrayWnd",NULL));
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01F7,0);
		break;
	case WS_LOCKKEYB:
		OnLockStation();
		//::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01F,0);
		break;
	case WS_SHOWRUND:
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)::FindWindow("Shell_TrayWnd",NULL));
		::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01F4,0);
		break;
	case WS_SHOWUTILM:
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)::FindWindow("Shell_TrayWnd",NULL));
		//::PostMessage(::FindWindow("Shell_TrayWnd",NULL),WM_HOTKEY,0x01F,0);
		break;		
	case KILL_PROCLIST:
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_KILLPROCESSESFROMLIST,0);
		break;
	case FREE_MEMORY:
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_HOTMENU_FREEMEMORY,0);
		break;
	case SHOW_OPTIONS:
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_OPTIONS,0);
		break;
	case LAST_HIDDEN:
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_SHOWALLAPPS,0);
		break;
	case BRING_INTOVIEW:
		OnHotmenuBringinview();
		break;
	case SWITCH_ONTOP:
		OnHotmenuSwitchontop();
		break;
	case ROLLUP_TOTITLE:
		HideApplication(objSettings.hHotMenuWnd,objSettings.bPreserveTitle?7:8);
		break;
	case WINDOW_RESTORE:
		HideApplication(objSettings.hHotMenuWnd,4);
		break;
	case WINDOW_TRANS:
		HideApplication(objSettings.hHotMenuWnd,13);
		break;
	case WINDOW_MINIMIZE:
		OnMinimizeActiveWindow();
		break;
	case SOUND_MUTE:
		OnSystraySetMute();
		break;
	case CONVERT_SELECTED:
		OnHotmenuConvertselected();
		break;
	case CONVERT_LASTWORD:
		OnHotmenuConvertselectedLW();
		break;
	case TAKE_SCREENSHOT:
		OnHotmenuTakescreenshot();
		break;
	case RUN_ASCOMMAND:
		OnHotmenuRunascommand();
		break;
	case RUN_SCREENSAVER:
		OnHotmenuRunscreensaver();
		break;
	case LOCK_STATION:
		OnLockStation();
		break;
	case EXTREM_SHUTD:
		g_ShutType=2;
		OnShutDownComputer();
		break;
	case DIALOG_SHUTDOWN:
		{_XLOG_
			static long lLock=0;
			if(lLock==0){_XLOG_
				SimpleTracker tr(lLock);
				typedef int (WINAPI *_Shutdown)(DWORD);
				HINSTANCE hShell32=LoadLibrary("shell32.dll");
				if(hShell32){_XLOG_
					_Shutdown fp=(_Shutdown)DBG_GETPROC(hShell32,(LPCSTR)0x3c);
					if(fp){_XLOG_
						(*fp)(0);
					}
				}
			}
		}
		break;
	case FAST_SHUTDOWN:
		g_ShutType=objSettings.lShutdownStrength;
		OnShutDownComputer();
		break;
	case VOLUME_UP:
		ChangeVolumeControl(objSettings.lVolumeCStep);
		break;
	case VOLUME_DOWN:
		ChangeVolumeControl(-objSettings.lVolumeCStep);
		break;
	case RUN_DEFBROWSER:
		StartDefBrowser();
		break;
	case RUN_DEFEMAILCL:
		StartDefEmail();
		break;
	case SWITCHLAYOUT:
		SetKBLayout(0);
		break;
	case BOSS_KEY:
		OnBossKey();
		break;
	case OPEN_CD:
		OnSwapCD();
		break;
	case CHANGE_CASE:
		OnHotmenuSwitchcase();
		break;
	case WINAMP_SHOWTITL:
		ShowWinampTitle(TRUE);
		break;
	case ADDDESK_NEXT:
		{_XLOG_
			int iNewDesk=objSettings.lLastActiveDesktop+1;
			if(iNewDesk>=objSettings.lDesktopSwitches){_XLOG_
				iNewDesk=0;
			}
			FORK(SwitchCurrentDesktop,iNewDesk);
			break;
		}
	case ADDDESK_PREV:
		{_XLOG_
			int iNewDesk=objSettings.lLastActiveDesktop-1;
			if(iNewDesk<0){_XLOG_
				iNewDesk=objSettings.lDesktopSwitches-1;
			}
			FORK(SwitchCurrentDesktop,iNewDesk);
			break;
		}
	case WINAMP_VISPLUGIN:
	case WINAMP_SHUFFLING:
	case WINAMP_PLAY:
	case WINAMP_STOP:
	case WINAMP_PREVS:
	case WINAMP_NEXTS:
	case WINAMP_NEXT5S:
	case WINAMP_PREV5S:
	case WINAMP_VOLUP:
	case WINAMP_VOLDOWN:
	case WINAMP_SHOWHIDE:
		WinAmpControl(iAction,TRUE,objSettings.lWinAmpNeedOSD);
		break;
	case SCROLL_UP:
	case SCROLL_DOWN:
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
	case SCROLL_PAGEUP:
	case SCROLL_PAGEDN:
		ScrollAction(iAction);
		break;
	case ICONS_SAVE:
		wk.SaveIconsPos(0,0);
		break;
	case ICONS_RESTORE:
		wk.RestoreIconsPos(0,0);
		break;
	case MONITOR_STANDBY:
		WaitWhileAllKeysAreFreeX();
		Sleep(500);
		::SendMessage(HWND_BROADCAST,WM_SYSCOMMAND, SC_MONITORPOWER, 1);
		break;
	case HOTSEL_MENU:
		PostMessage(WM_COMMAND,FIREHOTMENU,5);
		break;
	default:
		AddError("No tray action!",TRUE);
	}
	return 0;
}

char FirstDriveFromMask (ULONG unitmask)
{_XLOG_
	char i;
	for (i = 0; i < 26; ++i)
	{_XLOG_
		if (unitmask & 0x1)
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}

#include <dbt.h>
#define SSHOTCOLOR1	RGB(120,120,120)
#define SSHOTCOLOR2	RGB(200,200,200)
BOOL AppMainDlg::PreTranslateMessage(MSG* pMsg) 
{_XLOG_

	if(pMsg->message==273 && pMsg->wParam==32983 && pMsg->lParam==1){_XLOG_
		wk.RestartWK();
		return TRUE;
	}
	if(pMsg->hwnd==HWND(mcMenu)){_XLOG_
		if(GetAsyncKeyState(VK_LBUTTON)<0 || GetAsyncKeyState(VK_RBUTTON)<0){_XLOG_
			mcMenu.ShowToolTip(FALSE);
		}
	}
	if(pMsg->hwnd==HWND(ttMenu)){_XLOG_
		if(GetAsyncKeyState(VK_LBUTTON)<0 || GetAsyncKeyState(VK_RBUTTON)<0){_XLOG_
			ttMenu.ShowToolTip(FALSE);
		}
	}
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE){_XLOG_
		if(pMainDialogWindow){_XLOG_
			pMainDialogWindow->ShowWindow(FALSE);
		}
		return TRUE;
	}
	/*
	DoDragDrop - for future
	if(pMsg->message==WM_MENUGETOBJECT){_XLOG_
		return MNGO_NOERROR;
	}
	if(pMsg->message==WM_MENUDRAG){_XLOG_
		return MND_CONTINUE;
	}
	*/
	return CDialog::PreTranslateMessage(pMsg);
}

DWORD WINAPI CloseCDOnShutdown(LPVOID pData)
{_XLOG_
	char c=(char)pData;
	Sleep(1500);
	CloseCD(c,FALSE);
	return 0;
}

void AppMainDlg::PerformOnShutdown()
{_XLOG_
if(spyMode()){
	return;
}
	static BOOL bPerformed=0;
	if(bPerformed!=0){_XLOG_
		return;
	}
	bPerformed=1;
	if(objSettings.lStartupDesktop!=0){_XLOG_
		// А то дисководы по два раза скачут
		return;
	}
	// Volume - set before shutdown
	if(objSettings.lVolumeBeforeShutdown){_XLOG_
		mixer.SetVolumeCtrlValue((UINT)(double(MAX_MIXER_VOL)*double(objSettings.lVolumeBeforeShutdownLevel)/double(100)));
	}
	// CD drives - check for disk
	if(objSettings.bEjectCDOnShut){_XLOG_
		BOOL bSleepYep=0;
		if(objSettings.sCDDisk!=""){_XLOG_
			objSettings.sCDDisk.MakeUpper();
			EjectCDIfDiskPresent(objSettings.sCDDisk[0]);
			// Closing after 2 seconds
			FORK(CloseCDOnShutdown,objSettings.sCDDisk[0])
			bSleepYep=1;
		}
		if(objSettings.sCDDisk2!=""){_XLOG_
			objSettings.sCDDisk2.MakeUpper();
			EjectCDIfDiskPresent(objSettings.sCDDisk2[0]);
			FORK(CloseCDOnShutdown,objSettings.sCDDisk2[0])
			bSleepYep=1;
		}
		if(bSleepYep){_XLOG_
			// Ждем а не то дисководы не успевают закрываться!!!
			Sleep(3500);
		}
	}
}

void AddHistoryItemToReg(int i, int& iRawCount, DWORD dwHistNum, HKEY hReadkey)
{_XLOG_
	CString sMsg=objSettings.sClipHistory[GetClipHistoryName(0)][i].sItem;
	if(dwHistNum!=0){_XLOG_
		sMsg.TrimLeft();
		sMsg.TrimRight();
		sMsg=TrimMessage(sMsg,dwHistNum);
		sMsg.Replace("\t"," ");
		sMsg.Replace("\r"," ");
		sMsg.Replace("\n"," ");
		while(sMsg.Replace("  "," ")>0){};
	}
	char szText[32]={0};
	strcpy(szText,"PasteHistory");
	szText[sizeof("PasteHistory")-1]='0'+iRawCount;
	szText[sizeof("PasteHistory")]=0;
	DWORD dwRes=RegSetValueEx(hReadkey,szText,0,REG_SZ,(BYTE*)((const char*)sMsg),strlen(sMsg));
	iRawCount++;
	return;
}

BOOL bHelpShown=0;
DWORD WINAPI ShowProcTutorial(LPVOID p)
{_XLOG_
	HMENU h=(HMENU)p;
	Sleep(1500);
	if(IsMenu(h) && bHelpShown==0){_XLOG_
		bHelpShown=1;
		//RECT rt;
		//GetMenuItemRect(pMainDialogWindow->GetSafeHwnd(),h,0,&rt);
		CPoint pt;
		GetCursorPos(&pt);
		pMainDialogWindow->ShowBaloonAtPosition(_l("You just opened list of all active processes")+". "+
			_l("Click on process to close it")+". "+
			_l("Right-click on process to popup context menu with options to set process priority or view additional information about process")+"."
			,_l(PROGNAME" Hint"),pt.x,pt.y,20000);
	}
	return 0;
}

DWORD WINAPI BackIconToNormal(LPVOID pData)
{_XLOG_
	Sleep((DWORD)pData);
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->SetWindowIcon();
	}
	return 0;
}

extern long lPasteClipInSyncON;
HGLOBAL& GetGlobalClipText(BOOL);
#ifdef _DEBUG
CString& GetGlobalClipTextD();
#endif
HWND g_hRecursiveWndBase=NULL;
extern long g_lOverlayState;
CMap<DWORD,DWORD,DWORD,DWORD&> sCDMap;
/*
Ukrainian 00000422 
Russian 00000419 
English 00000409 
German 00000407 
Spanish 00000403 
polish 00000415 
Romainan 00000418 
French 0000040C 
*/
void KillActiveMacro(HANDLE h, BOOL bHard)
{_XLOG_
	// Смерть иконке...
	NOTIFYICONDATA m_tnd;
	memset(&m_tnd,0,sizeof(m_tnd));
	m_tnd.cbSize = sizeof(m_tnd);
	m_tnd.hWnd   = pMainDialogWindow->GetSafeHwnd();
	m_tnd.uID    = UINT(h);
	m_tnd.uFlags = 0;
	Shell_NotifyIcon(NIM_DELETE, (PNOTIFYICONDATA)&m_tnd);
	ShowBaloon("","");
	// И потоку
	TerminateThread(h,99);
	if(!bHard)
	{_XLOG_
		objSettings.aQRThreads.RemoveKey(h);
		/*if(p){_XLOG_
		MSScriptControl::IScriptControlPtr* script=(MSScriptControl::IScriptControlPtr*)p;
		(*script)->Reset();
		}else{_XLOG_
		TerminateThread(h,99);
		objSettings.aQRThreads.RemoveKey(h);
		CloseHandle(h);
		}*/
	}
	CloseHandle(h);
}

DWORD WINAPI ClosettMenuBaloon(LPVOID p)
{_XLOG_
	static int iCount=0;
	iCount++;
	int iStartCount=iCount;
	Sleep((long)p);
	if(iStartCount==iCount){_XLOG_
		pMainDialogWindow->PostMessage(SHOWCURBALOON,0,0);
	}
	return 0;
}

void AppMainDlg::ShowBaloonAtPosition(CString sText, CString sTitle, int iX, int iY, DWORD dwTimeout)
{_XLOG_
	if(!mcMenu.bCreated){_XLOG_
		mcMenu.Create(GetSafeHwnd(), "...", AfxGetResourceHandle(), TTS_NOPREFIX | TTS_BALLOON, NULL);
	}
	if(sText==""){_XLOG_
		mcMenu.ShowToolTip(FALSE);
	}
	if(sText!=""){_XLOG_
		mcMenu.SetToolTipPosition(iX,iY);
		mcMenu.UpdateToolTipTitle(sTitle);
		mcMenu.UpdateToolTipText(sText);
		mcMenu.ShowToolTip(TRUE);
		FORK(ClosettMenuBaloon,dwTimeout);
	}
}

DWORD WINAPI DoWM_DRAWCLIPBOARDProcessing_InThread(LPVOID p);
LRESULT AppMainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{_XLOG_
	if(message==LOADPLUGIN){_XLOG_
		// Грузим плагин!
		CString sName=(const char*)wParam;
		CPlugin* pOut=0;
		wk.isPluginLoaded(sName,&pOut);
		if(pOut && !pOut->bIsActive){_XLOG_
			LoadPlugin(pOut,TRUE);
			pOut->bIsActiveOverrideForOption=1;
			wk.bForcePluginLoadUpdate=1;
		}
		return TRUE;
	}
	if(message==SETTRAYICON){_XLOG_
		SetWindowIcon(wParam);
		return TRUE;
	}
	if(message==SHOWCURBALOON){_XLOG_
		CStringArray* aParams=(CStringArray*)lParam;
		if(aParams){_XLOG_
			CString sP1;
			CString sP2;
			CString sP3;
			CString sP4;
			CString sP5;
			if(aParams->GetSize()>0){_XLOG_
				sP1=aParams->GetAt(0);
			}
			if(aParams->GetSize()>1){_XLOG_
				sP2=aParams->GetAt(1);
			}
			if(sP2==""){_XLOG_
				sP2=_l("Warning");
			}
			if(aParams->GetSize()>2){_XLOG_
				sP3=aParams->GetAt(2);
			}
			if(aParams->GetSize()>3){_XLOG_
				sP4=aParams->GetAt(3);
			}
			if(sP3=="" && sP4==""){_XLOG_
				CPoint pt;
				GetCursorPos(&pt);
				sP3=Format(pt.x);
				sP4=Format(pt.y);
			}
			if(aParams->GetSize()>4){_XLOG_
				sP5=aParams->GetAt(4);
			}
			if(sP5==""){_XLOG_
				sP5="5000";
			}
			ShowBaloonAtPosition(sP1,sP2,atol(sP3),atol(sP4),atol(sP5));
			delete aParams;
		}else{_XLOG_
			ShowBaloonAtPosition("","",0,0,0);
		}
		return TRUE;
	}
	if(message==STOPMACRO && (LOWORD(lParam) == WM_RBUTTONUP || LOWORD(lParam) == WM_LBUTTONUP)){_XLOG_
		LPVOID p=0;
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		static long lNoRec=0;
		if(lNoRec>0){_XLOG_
			return TRUE;
		}
		SimpleTracker lc(lNoRec);
		HANDLE h=(HANDLE)wParam;
		if(objSettings.aQRThreads.Lookup(h,p)){_XLOG_
			DWORD dwRes=AskYesNo(_l("Do you want to\r\nstop macro execution")+"?",_l("Confirmation"),0,GetAppWnd());
			if(dwRes==IDYES){_XLOG_
				KillActiveMacro(h,0);
			}
		}
		return TRUE;
	}
	if(message==UPDATE_ICON){_XLOG_
		SetWindowIcon();
		return TRUE;
	}
	if(objSettings.lShowDateInTrayIcon==3){_XLOG_
		if(message==LANG_CHANGE){_XLOG_
			if(lParam==0){_XLOG_
				lParam=(LPARAM)GetKeyboardLayout(0);
			}
			if(lParam==99 && wParam==99){_XLOG_
				HWND hWn=::GetForegroundWindow();
				DWORD dwProc=0;
				DWORD dwThread=GetWindowThreadProcessId(hWn,&dwProc);
				lParam=(LPARAM)GetKeyboardLayout(dwThread);
				static LPARAM lPrev=0;
				if(lParam!=lPrev){_XLOG_
					lPrev=lParam;
				}else{_XLOG_
					lParam=0;
				}
			}
			if(lParam!=0){_XLOG_
				objSettings.sLastStringForTrayIcons=_l(Format("LANG_ID%04Xs",LOWORD(lParam)));
				SetWindowIcon();
			}
			return TRUE;
		}
	}
	if(message==WM_DEVICECHANGE){_XLOG_
		DWORD dwStatus=0;
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
		char szMsg[80]={0};
		switch(wParam)
		{_XLOG_
		case DBT_DEVICEARRIVAL:
			// Check whether a CD or DVD was inserted into a drive.
			if (lpdb -> dbch_devicetype == DBT_DEVTYP_VOLUME){_XLOG_
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv -> dbcv_flags & DBTF_MEDIA){_XLOG_
					char drive=FirstDriveFromMask(lpdbv ->dbcv_unitmask);
					dwStatus=0;
					sCDMap.SetAt(drive,dwStatus);
				}
			}
			break;
			
		case DBT_DEVICEREMOVECOMPLETE:
			// Check whether a CD or DVD was removed from a drive.
			if (lpdb -> dbch_devicetype == DBT_DEVTYP_VOLUME){_XLOG_
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv -> dbcv_flags & DBTF_MEDIA){_XLOG_
					char drive=FirstDriveFromMask(lpdbv ->dbcv_unitmask);
					dwStatus=1;
					sCDMap.SetAt(drive,dwStatus);
				}
			}
			break;
		}
	}
	if(message==WM_PASTE){_XLOG_
		if(LOWORD(lParam)<objSettings.sClipHistory[GetClipHistoryName(HIWORD(lParam))].GetSize()){_XLOG_
			CString sMsg=objSettings.sClipHistory[GetClipHistoryName(HIWORD(lParam))][LOWORD(lParam)].sItem;
			pMainDialogWindow->PostMessage(WM_COMMAND,PASTETEXT,(LPARAM)strdup(sMsg));
		}
		return TRUE;
	}
	if(message==WM_COPYDATA){_XLOG_
		COPYDATASTRUCT* cs=(COPYDATASTRUCT*)lParam;
		if(cs){_XLOG_
			DWORD dwMessage=HIWORD(cs->dwData);
			if(dwMessage==WM_CREATE){_XLOG_
				CString sCommand=(char*)cs->lpData;
				StartQRunByName(sCommand,"");
				return 1;
			}
			if(dwMessage==WM_SETTEXT){_XLOG_
				CString sCommand=(char*)cs->lpData;
				ShowBaloon(sCommand,"",3000);
			}
			if(dwMessage==WM_PASTE){_XLOG_
				CString sMsg;
				long dwHistNum=LOWORD(cs->dwData);
				CString sCommand=(char*)cs->lpData;
				if(sCommand.Find("PASTE")==0 && dwHistNum<objSettings.sClipHistory[GetClipHistoryName(0)].GetSize()){_XLOG_
					sMsg=objSettings.sClipHistory[GetClipHistoryName(0)][dwHistNum].sItem;
					pMainDialogWindow->PostMessage(WM_COMMAND,PASTETEXT,(LPARAM)strdup(sMsg));
				}else if(sCommand.Find("INIT_")==0){//Заполняем реестр
					CString sBranch=sCommand.Mid(5);
					HKEY hReadkey=0;
					if(RegOpenKeyEx(HKEY_CURRENT_USER,(CString)"SOFTWARE\\WiredPlane\\"PROGNAME"\\"+sBranch,0,KEY_WRITE,&hReadkey)!=ERROR_SUCCESS){_XLOG_
						DWORD dwDispos=0;
						RegCreateKeyEx(HKEY_CURRENT_USER,(CString)"SOFTWARE\\WiredPlane\\"PROGNAME"\\"+sBranch,0,NULL,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&hReadkey,&dwDispos);
					}
					if(hReadkey){_XLOG_
						int i=0,iRawCount=0;
						char szText[32]={0};
						// С objSettings.lClipHistoryDepth поиграт не выйдет, так как в менюшках важен порядок =ый текущему
						for(i=0;i<objSettings.sClipHistory[GetClipHistoryName(0)].GetSize();i++){_XLOG_
							AddHistoryItemToReg(i,iRawCount,dwHistNum,hReadkey);
						}
						// Пустая строчка после последнего элемента...
						sMsg="";
						strcpy(szText,"PasteHistory");
						szText[sizeof("PasteHistory")-1]='0'+i;
						szText[sizeof("PasteHistory")]=0;
						DWORD dwRes=RegSetValueEx(hReadkey,szText,0,REG_SZ,(BYTE*)((const char*)sMsg),strlen(sMsg));
						RegCloseKey(hReadkey);
					}
				}
				return TRUE;
			}
		}
		return FALSE;
	}
	if(message==WM_MENUSELECT){_XLOG_
		DWORD dwFlags=HIWORD(wParam);
		DWORD dwItemID=LOWORD(wParam);
		HMENU hMenu=HMENU(lParam);
		if(dwFlags != 0xFFFF && hMenu != NULL){_XLOG_
			if(objSettings.lShowHPMTooltip && (hMenu==g_hClipHistoryMenu || hMenu==g_hClipHistoryMenu2)){_XLOG_
				if(!ttMenu.bCreated){_XLOG_
					ttMenu.Create(GetSafeHwnd(), "...", AfxGetResourceHandle(), TTS_NOPREFIX | TTS_BALLOON, 0, TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE);
				}
				if((dwItemID-1)>=0 && (dwItemID-1)<DWORD(objSettings.sClipHistory[g_sOpenedHistoryMenuName].GetSize())){_XLOG_
					CString sText=TrimMessage(objSettings.sClipHistory[g_sOpenedHistoryMenuName][dwItemID-1].sItem,100);
					if(objSettings.sClipHistory[g_sOpenedHistoryMenuName][dwItemID-1].sClipItemDsc!=""){_XLOG_
						sText+=Format("\n --- %s: ---\n",_l("Taken from"));
						sText+=TrimMessage(objSettings.sClipHistory[g_sOpenedHistoryMenuName][dwItemID-1].sClipItemDsc,200);
					}
					ttMenu.OnMenuSelect(sText, dwItemID, dwFlags|(hMenu==g_hClipHistoryMenu2?MF_SHOWATCURSOR:0), hMenu);
				}else{_XLOG_
					ttMenu.ShowToolTip(FALSE);
				}
			}
		}
	}
	if(message==WM_INITMENUPOPUP){_XLOG_
		bAnyPopupIsActive=TRUE;
		HMENU h=(HMENU)wParam;
		// Дополняем пункты если надо (процессы, подкаталоги)
		if(h && IsMenu(h)){_XLOG_
			char szItem[MAX_PATH+30]="";
			memset(szItem,0,sizeof(szItem));
			if(GetMenuString(h,0,szItem,sizeof(szItem)-1,MF_BYPOSITION)){_XLOG_
				if(strstr(szItem,EXPAND_FOLDER_ID)!=NULL){_XLOG_
					// Раскрываем каталог
					HCURSOR hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
					::DeleteMenu(h,0,MF_BYPOSITION);
					CString sFileTitle;
					char szFileName[MAX_PATH]="",szFileExt[MAX_PATH]="";
					CString sFolder=(const char*)(szItem+strlen(EXPAND_FOLDER_ID));
					if(sFolder!=""){_XLOG_
						// Вытаскиваем маску (если есть)
						CString sMask;
						int iMaskPos=sFolder.Find(EXPAND_FOLDER_MASK);
						if(iMaskPos>0){_XLOG_
							sMask=sFolder.Mid(iMaskPos+strlen(EXPAND_FOLDER_MASK));
							sFolder=sFolder.Left(iMaskPos);
							sMask.Replace(",",";");
						}
						CMenu aFiles;
						aFiles.Attach(h);
						if(!isFileExist(sFolder)){_XLOG_
							AddMenuString(&aFiles,0,_l("Folder not found"),NULL,FALSE,NULL,TRUE);
						}else{_XLOG_
							CFileInfoArray dir;
							dir.AddDir(sFolder,sMask!=""?sMask:"*.*",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,TRUE);
							long lDirSize=dir.GetSize();
							if(lDirSize==0){_XLOG_
								AddMenuString(&aFiles,0,_l("Folder is empty"),NULL,FALSE,NULL,TRUE);
							}else{_XLOG_
								long lBreakCount=0;
								long lBreakCountLim=lDirSize/3;
								if(lBreakCountLim<20){_XLOG_
									lBreakCountLim=lDirSize/2;
								}
								if(lBreakCountLim<20){_XLOG_
									lBreakCountLim=lDirSize+999;
								}
								dir.BubbleUpDirectories();
								for(int i=0;i<lDirSize;i++){_XLOG_
									CString sFile=dir[i].GetFilePath();
									if(sFile!=""){_XLOG_
										if(aExpandedFolderPaths.GetSize()==EXPAND_FILES_MAX){_XLOG_
											AddMenuString(&aFiles,0,CString("*** ")+_l("Too much files to load")+" ***");
											return 0;
										}
										int iIndex=aExpandedFolderPaths.Add(sFile);
										_splitpath(sFile,NULL,NULL,szFileName,szFileExt);
										sFileTitle=TrimMessage(szFileName,20);
										sFileTitle+="\t";
										sFileTitle+=szFileExt;
										CString sQRunIcon=GetExeIconIndex(sFile);
										if(dir[i].IsDirectory()){_XLOG_
											CMenu sub;
											sub.CreatePopupMenu();
											AddSlashToPath(sFile);
											CString sFirstItem=Format("%s%s%s%s",EXPAND_FOLDER_ID,sFile,EXPAND_FOLDER_MASK,sMask);
											AddMenuString(&sub,0,sFirstItem);
											AddMenuSubmenu(&aFiles,&sub,sFileTitle,_bmp().Get(sQRunIcon),WM_USER+EXPAND_FILES+iIndex);
											lBreakCount++;
										}else{_XLOG_
											BOOL bBreak=FALSE;
											if(lBreakCount>lBreakCountLim){_XLOG_
												lBreakCount=0;
												bBreak=TRUE;
											}
											AddMenuString(&aFiles,WM_USER+EXPAND_FILES+iIndex,sFileTitle,_bmp().Get(sQRunIcon),FALSE,0,FALSE,FALSE,bBreak);
											lBreakCount++;
										}
									}
								}
							}
						}
						TUNE_UP_ITEM2(aFiles,"Right-click: more options");
						aFiles.Detach();
					}
					SetCursor(hCur);
					return 0;
				}
				if(strcmp(szItem,PROC_MENU_ID)==0){_XLOG_
					HCURSOR hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
					::DeleteMenu(h,0,MF_BYPOSITION);
					{_XLOG_
						CMenu aProcesses;
						aProcesses.Attach(h);
						DWORD dwProcID=0;
						CString sProcessIcon;
						AppendProcessesToMenu(aProcesses, sProcessIcon, dwProcID, TRUE);
						aProcesses.Detach();
					}
					SetCursor(hCur);
					/*
					if(objSettings.bApplicationFirstRun && bHelpShown==0)
					{_XLOG_
						FORK(ShowProcTutorial,h);
					}
					*/
					return 0;
				}
			}
		}
	}
	if(message==WM_MENURBUTTONUP){_XLOG_
		HMENU h=(HMENU)lParam;
		if(IsMenu(h)){_XLOG_
			DWORD dwID=(DWORD)wParam;
			MENUITEMINFO mii;
			memset(&mii,0,sizeof(mii));
			mii.cbSize=sizeof(mii);
			mii.fMask=MIIM_ID;
			GetMenuItemInfo(h,dwID,TRUE,&mii);
			dwID=mii.wID;
			if(h==g_hClipHistoryMenu || h==g_hClipHistoryMenu2){
				ttMenu.ShowToolTip(FALSE);
				CMenu scrMenu;
				scrMenu.CreatePopupMenu();
				AddMenuString(&scrMenu,2,_l("Pin/Unpin")+"\tShift-click",_bmp().Get(_IL(12)));
				AddMenuString(&scrMenu,1,_l("Edit"),_bmp().Get(_IL(93)));
				AddMenuString(&scrMenu,3,_l("Delete"),_bmp().Get(IDB_BM_DEL));
				CPoint pt;
				GetCursorPos(&pt);
				DWORD dwRetCode=TrackPopupMenu(scrMenu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RECURSE, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
				if(dwRetCode!=0){_XLOG_
					switch(dwRetCode)
					{_XLOG_
					case 1:
						EditClip(g_sOpenedHistoryMenuName,dwID-1);
						break;
					case 2:
						InsertAndPinOrBubbleHistory(g_sOpenedHistoryMenuName, dwID-1, FALSE, 1);
						break;
					case 3:
						InsertAndPinOrBubbleHistory(g_sOpenedHistoryMenuName, dwID-1, FALSE, -1);
						break;
					}
				}
				//CancelMenu(g_sOpenedHistoryMenuName);
				//DestroyMenu
				EndMenu();
				return TRUE;
			}else if(dwID==ID_SYSTRAY_MAKESCREENSHOT){_XLOG_
				CMenu scrMenu;
				scrMenu.CreatePopupMenu();
				AddMenuString(&scrMenu,18,_l("Save to")+" "+TrimMessage(objSettings.sScrshotDir,20,3),_bmp().Get(_IL(5)));
				AddMenuString(&scrMenu,10,_l("Save as JPG image"),0,objSettings.bUseJpg);
				AddMenuString(&scrMenu,11,_l("Put into clipboard"),0,objSettings.bPutScreshotInClipboard);
				AddMenuString(&scrMenu,15,_l("Add cursor to screenshot"),0,objSettings.lAddCursorToScreenshot);
				//AddMenuString(&scrMenu,24,_l("Video windows support"),0,(g_lOverlayState!=0));
				AddMenuString(&scrMenu,12,_l("Sign screenshot"),0,objSettings.bSignSShot);
				AddMenuString(&scrMenu,13,_l("Fix X size")+Format("\t%lupx",objSettings.lFixXSizeTo),0,objSettings.bFixXSize);
				AddMenuString(&scrMenu,14,_l("Fix Y size")+Format("\t%lupx",objSettings.lFixYSizeTo),0,objSettings.bFixYSize);
				AddMenuString(&scrMenu,16,_l("Make screenshots on timer"),0,objSettings.lScreenshotByTimer);
				CMenu scrTarget;
				scrTarget.CreatePopupMenu();
				AddMenuString(&scrTarget,1,_l("Whole screen"),0,objSettings.bShotActiveWindow==0);
				AddMenuString(&scrTarget,2,_l("Active window"),0,objSettings.bShotActiveWindow==1);
				AddMenuString(&scrTarget,3,_l("Draw rectangle"),0,objSettings.bShotActiveWindow==2);
				AddMenuSubmenu(&scrMenu,&scrTarget,_l("Make screenshot from")+"\t...");
				CMenu scrOpen;
				scrOpen.CreatePopupMenu();
				AddMenuString(&scrOpen,20,_l("Do not open"),0,objSettings.bAutoEditOnShot==0);
				AddMenuString(&scrOpen,21,_l("Open for single-press"),0,objSettings.bAutoEditOnShot==1);
				AddMenuString(&scrOpen,22,_l("Open for double-press"),0,objSettings.bAutoEditOnShot==2);
				AddMenuString(&scrOpen,23,_l("Open always"),0,objSettings.bAutoEditOnShot==3);
				AddMenuSubmenu(&scrMenu,&scrOpen,_l("Open image editor")+"\t...");
				scrMenu.AppendMenu(MF_SEPARATOR, 0, "");
				AddMenuString(&scrMenu,17,_l("Open preferences"),_bmp().Get(_IL(93)));
				CPoint pt;
				GetCursorPos(&pt);
				DWORD dwRetCode=TrackPopupMenu(scrMenu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RECURSE, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
				if(dwRetCode!=0){_XLOG_
					switch(dwRetCode)
					{_XLOG_
					case 1:
						objSettings.bShotActiveWindow=0;
						break;
					case 2:
						objSettings.bShotActiveWindow=1;
						break;
					case 3:
						objSettings.bShotActiveWindow=2;
						break;
					case 20:
						objSettings.bAutoEditOnShot=0;
						break;
					case 21:
						objSettings.bAutoEditOnShot=1;
						break;
					case 22:
						objSettings.bAutoEditOnShot=2;
						break;
					case 23:
						objSettings.bAutoEditOnShot=3;
						break;
					case 24:
						wk.SwitchOverlaySupport(0,0);
						break;
					case 10:
						objSettings.bUseJpg=objSettings.bUseJpg?0:1;
						break;
					case 11:
						objSettings.bPutScreshotInClipboard=objSettings.bPutScreshotInClipboard?0:1;
						break;
					case 12:
						objSettings.bSignSShot=objSettings.bSignSShot?0:1;
						break;
					case 13:
						objSettings.bFixXSize=objSettings.bFixXSize?0:1;
						break;
					case 14:
						objSettings.bFixYSize=objSettings.bFixYSize?0:1;
						break;
					case 15:
						objSettings.lAddCursorToScreenshot=objSettings.lAddCursorToScreenshot?0:1;
						break;
					case 16:
						objSettings.lScreenshotByTimer=objSettings.lScreenshotByTimer?0:1;
						if(objSettings.lScreenshotByTimer){_XLOG_
							Alert(SCRSEQCAUTION);
						}
						break;
					case 17:
						objSettings.OpenOptionsDialog(0,_l(SCRSHOT_TOPIC),this);
						break;
					case 18:
						{_XLOG_
							char szDir[MAX_PATH]="";
							strcpy(szDir,objSettings.sScrshotDir);
							if(GetFolder (_l("Choose directory"), szDir, NULL, GetSafeHwnd())){_XLOG_
								objSettings.sScrshotDir=szDir;
								AddSlashToPath(objSettings.sScrshotDir);
							}
							break;
						}
					};
				}
				return TRUE;
			}else if(dwID>=WM_USER+QRUNAPPLICATION && dwID<(DWORD)(WM_USER+QRUNAPPLICATION+USERMSG_STEP_HALF)){_XLOG_
				CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
				int iMode=dwID-(WM_USER+QRUNAPPLICATION);
				CMenu QItemMenu;
				QItemMenu.CreatePopupMenu();
				CQuickRunItem* pQRunItem=objSettings.aQuickRunApps[iMode];
				if(pQRunItem && pQRunItem->lMacros==2){_XLOG_
					AddMenuString(&QItemMenu,1,_l("Play macro"),_bmp().Get(_IL(103)));
				}else{_XLOG_
					AddMenuString(&QItemMenu,1,_l("Activate this item"),_bmp().Get(_IL(22)));
				}
				AddMenuString(&QItemMenu,2,_l("Edit item properties"),_bmp().Get(_IL(93)));
				if(pQRunItem && pQRunItem->lMacros!=2 && pQRunItem->lParamType==2){_XLOG_
					if(!pQRunItem->lExpandFolderOrCheckInstance){_XLOG_
						AddMenuString(&QItemMenu,6,_l("Expand folder`s content"),_bmp().Get(_IL(5)));
					}
				}
				if(pQRunItem && pQRunItem->lMacros==2){_XLOG_
					AddMenuString(&QItemMenu,7,_l("Edit macro file"),_bmp().Get(_IL(56)));
				}
				QItemMenu.AppendMenu(MF_SEPARATOR, 0, "");
				AddMenuString(&QItemMenu,4,_l("Show item in menus"),0,TRUE);
				AddMenuString(&QItemMenu,3,_l("Detach item into icon"),_bmp().Get(_IL(54)));
				AddMenuString(&QItemMenu,8,_l("Detach item to floater"),_bmp().Get(_IL(54)));
				QItemMenu.AppendMenu(MF_SEPARATOR, 0, "");
				AddMenuString(&QItemMenu,5,_l("Delete this item"),_bmp().Get(IDB_BM_DEL));
				CPoint pt;
				GetCursorPos(&pt);
				DWORD dwRetCode=TrackPopupMenu(QItemMenu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RECURSE, pt.x, pt.y, 0, (g_hRecursiveWndBase!=NULL)?g_hRecursiveWndBase:this->GetSafeHwnd(), NULL);
				if(dwRetCode==1){_XLOG_
					PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+iMode,0);
				}else if(dwRetCode==2){_XLOG_
					FORK(SendCommand_InThread,(WM_USER+QRUNAPP_EDIT+iMode));
				}else if(dwRetCode==3){_XLOG_
					PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+iMode,QRUN_DETACHTO);
				}else if(dwRetCode==4){_XLOG_
					objSettings.aQuickRunApps[iMode]->lHowToShowInTray=0;
				}else if(dwRetCode==5){_XLOG_
					wk.RemoveQRun((HIROW)pQRunItem->lUniqueCount,NULL);
				}else if(dwRetCode==6){_XLOG_
					pQRunItem->lExpandFolderOrCheckInstance=1;
				}else if(dwRetCode==7){_XLOG_
					wk.EditMacroFile(pQRunItem->sItemPath);
				}else if(dwRetCode==8){_XLOG_
					PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+iMode,QRUN_DETACHTO|QRUN_DETACHFL);
				}
				if(dwRetCode!=0){_XLOG_
					EndMenu();
				}
				return TRUE;
			}else if(dwID>=WM_USER+SET_DMODE && dwID<(DWORD)(WM_USER+SET_DMODE+USERMSG_STEP)){_XLOG_
				int iMode=dwID-(WM_USER+SET_DMODE);
				CVideoMode md=objSettings.videoModes[iMode];
				CMenu DModesMenu;
				DModesMenu.CreatePopupMenu();
				for(int i=objSettings.videoModes.GetSize()-1;i>=0;i--){_XLOG_
					CVideoMode& mdCur=objSettings.videoModes[i];
					if(mdCur.m_dwWidth==md.m_dwWidth && mdCur.m_dwHeight==md.m_dwHeight){_XLOG_
						AddMenuString(&DModesMenu,WM_USER+SET_DMODE+i,Format("%lu*%lu, %lubpp @%lu",mdCur.m_dwWidth,mdCur.m_dwHeight,mdCur.m_dwBitsPerPixel,mdCur.m_dwFrequency));
					}
				}
				CPoint pt;
				GetCursorPos(&pt);
				DWORD dwRetCode=TrackPopupMenu(DModesMenu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RECURSE, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
				if(dwRetCode!=0){_XLOG_
					PostMessage(WM_COMMAND,dwRetCode,0);
				}
			}else if(dwID>=WM_USER+EXPAND_FILES && dwID<(DWORD)(WM_USER+EXPAND_FILES+EXPAND_FILES_MAX)){_XLOG_
				dwID=dwID-(WM_USER+EXPAND_FILES);
				if(dwID>=0 && dwID<DWORD(aExpandedFolderPaths.GetSize())){_XLOG_
					CPoint pt;
					GetCursorPos(&pt);
					ShowContextMenu(this->GetSafeHwnd(), aExpandedFolderPaths[dwID], pt.x, pt.y);
				}
				return TRUE;
			}else if(dwID>=WM_USER+KILLPROCESS && dwID<(DWORD)(WM_USER+KILLPROCESS+USERMSG_STEP)){_XLOG_
				dwID=dwID-(WM_USER+KILLPROCESS);
				// Показываем меню с приоритетом и пр.
				if(dwID>=0 && dwID<DWORD(TrayMenuProcesses.GetSize())){_XLOG_
					CProcInfo* pInf=&TrayMenuProcesses[dwID];
					if(pInf->bTerminable){_XLOG_
						CMenu aProcMenu;
						aProcMenu.CreatePopupMenu();
						HANDLE hActiveProc=::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_SET_INFORMATION,FALSE,pInf->dwProcId);
						if (hActiveProc != NULL && hActiveProc != INVALID_HANDLE_VALUE){_XLOG_
							CPoint pos;
							GetCursorPos(&pos);
							DWORD dwPrior=GetPriorityClass(hActiveProc);
							AddMenuString(&aProcMenu,1,pInf->sProcName,_bmp().Get(GetExeIconIndex(pInf->sProcExePath)),FALSE,NULL,FALSE);
							aProcMenu.AppendMenu(MF_SEPARATOR, 0, "");
							AddMenuString(&aProcMenu,ID_HOTMENU_BRINGINVIEW,_l("Show main window"),_bmp().Get(_IL(14)));
							AddMenuString(&aProcMenu,ID_HOTMENU_SHOWEXEPATH,_l("Application info"),_bmp().Get(_IL(13)));
							aProcMenu.AppendMenu(MF_SEPARATOR, 0, "");
							AddMenuString(&aProcMenu,WM_USER+ACTIVEAPP_PRIOR+4,_l("Low"),NULL,dwPrior==IDLE_PRIORITY_CLASS);
							AddMenuString(&aProcMenu,WM_USER+ACTIVEAPP_PRIOR+3,_l("Normal"),NULL,dwPrior==NORMAL_PRIORITY_CLASS);
							AddMenuString(&aProcMenu,WM_USER+ACTIVEAPP_PRIOR+1,_l("High"),NULL,dwPrior==HIGH_PRIORITY_CLASS);
							aProcMenu.AppendMenu(MF_SEPARATOR, 0, "");
							AddMenuString(&aProcMenu,ID_HOTMENU_KILLPROCESS,_l("Kill application"),_bmp().Get(IDB_BM_DEL));
							int iCount=FindSimilarApps(TrayMenuProcesses,pInf->sProcName);
							if(iCount>1){
								AddMenuString(&aProcMenu,ID_HOTMENU_KILLPROCESS2,_l("Kill all")+" "+pInf->sProcName,_bmp().Get(IDB_BM_DEL));
							}
							DWORD dwRetCode=TrackPopupMenu(aProcMenu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RECURSE, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
							if(dwRetCode>=WM_USER+ACTIVEAPP_PRIOR && dwRetCode<(DWORD)(WM_USER+ACTIVEAPP_PRIOR+USERMSG_STEP)){_XLOG_
								int iPrior=dwRetCode-(WM_USER+ACTIVEAPP_PRIOR);
								if(iPrior>=0 && iPrior<=4){_XLOG_
									SetPriorityClass(hActiveProc,objSettings.bUnderWindows98?PriorityClasses[iPrior]:PriorityClassesNT[iPrior]);
								}
							}
							if(dwRetCode==ID_HOTMENU_KILLPROCESS2){
								CString sTargetName=pInf->sProcName;
								for(int i=0;i<TrayMenuProcesses.GetSize();i++){
									CProcInfo* pInf2=&TrayMenuProcesses[i];
									if(pInf2->sProcName==sTargetName){
										CString sTitle=pInf2->sProcName;
										if(pInf2->sProcExePath!=""){_XLOG_
											sTitle+=" (";
										pInf2->sProcExePath.TrimRight();
										sTitle+=TrimMessage(pInf2->sProcExePath,30,4);
										sTitle+=")";
										}
										if(!KillProcess(pInf2->dwProcId,sTitle)){_XLOG_
											AddError(_l("Unable to kill process"));
										}
									}
								}
							}
							if(dwRetCode==ID_HOTMENU_KILLPROCESS){_XLOG_
								CString sTitle=pInf->sProcName;
								if(pInf->sProcExePath!=""){_XLOG_
									sTitle+=" (";
									pInf->sProcExePath.TrimRight();
									sTitle+=TrimMessage(pInf->sProcExePath,30,4);
									sTitle+=")";
								}
								if(!KillProcess(pInf->dwProcId,sTitle)){_XLOG_
									AddError(_l("Unable to kill process"));
								}
							}
							if(dwRetCode==ID_HOTMENU_BRINGINVIEW){_XLOG_
								PopupAppWindow(pInf->dwProcId);
							}
							if(dwRetCode==ID_HOTMENU_SHOWEXEPATH || dwRetCode==1){_XLOG_
								EndMenu();
								PostMessage(WM_COMMAND,APPINFO2,pInf->dwProcId);
							}
							CloseHandle(hActiveProc);
						}
					}
				}
				return TRUE;
			}
		}
	}
	if(message==WM_QUERYENDSESSION){_XLOG_
		objSettings.bShutIP=1;
		OnBeforeShutdown();
		return TRUE;
	}
	if(message==WM_DISPLAYCHANGE){_XLOG_
		if(objSettings.lRestorePreservRes>1){_XLOG_
			wk.RestoreIconsPos(0,0);
		}
	}
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){_XLOG_
		return DispatchResultFromSizer;
	}
	if(message==WM_HELP){_XLOG_
		ShowHelp("Overview");
		return 0;
	}
	if(message==WM_ACTIVATEAPP){_XLOG_
		if(wParam==FALSE){_XLOG_
			bAnyPopupIsActive=FALSE;
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

extern "C" __declspec(dllexport)
LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{_XLOG_
	// Пишем в лог!!
	LOGERROR2("Hard exception, dwId=%i/%i",GetCurrentThreadId(),iGlobalState);
	return EXCEPTION_EXECUTE_HANDLER;
}

void AppMainDlg::Finalize()
{_XLOG_
	__FLOWLOG
	iGlobalState=0;
	SetUnhandledExceptionFilter(TopLevelExceptionFilter);
#ifndef DEBUG
	// В целях борьбы с хуками...
	#pragma TODO(Don`t forget to overcome SEM_NOGPFAULTERRORBOX)
	DWORD dwMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
	SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);
#endif
	iGlobalState=1;
	//-------------------------- Закрываем мьютекс активности WK ---------------
    if (objSettings.hUpAndRunningMutex != NULL){_XLOG_
		__FLOWLOG
		CloseHandle(objSettings.hUpAndRunningMutex);
        ::ReleaseMutex(objSettings.hUpAndRunningMutex);
		objSettings.hUpAndRunningMutex=0;
    }
	// В том числе и на теекущем десктопе
	OpenCurrentDeskAtom(0);
	//-----------------------------------------
	iRunStatus=-2;
	{
		// Убиваем макросы
		__FLOWLOG
			iGlobalState=2;
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		POSITION pos=objSettings.aQRThreads.GetStartPosition();
		while(pos){_XLOG_
			HANDLE h;
		LPVOID p;
		objSettings.aQRThreads.GetNextAssoc(pos,h,p);
		if(h){_XLOG_
			KillActiveMacro(h,1);
		}
		}
		objSettings.aQRThreads.RemoveAll();
	}
	// Сохраняем все...
	//-----------------------------------------------------
	// До SaveAll pMainDialogWindow должен быть нормальным!
	//-----------------------------------------------------
	objSettings.SaveAll();
	//-----------------------------------------------------
	if(objSettings.lOptionsDialogOpened && objSettings.pOpenedOptions){_XLOG_
		objSettings.pOpenedOptions->SendMessage(WM_COMMAND,IDCANCEL,0);
	}
	{
		__FLOWLOG
			pMainDialogWindow=NULL;
		objSettings.bSaveAllowed=0;
		iGlobalState=3;
		if(::IsWindow(theApp.m_pMainWnd->GetSafeHwnd())){_XLOG_
			theApp.m_pMainWnd->DestroyWindow();
		}
		theApp.m_pMainWnd=NULL;
	}
	// выключаем глобально следящий поток...
	SetEvent(objSettings.hTimerEvent);
	DEBUG_INFO2("WK Disabled",0,0);
	while(GlobalFindAtom("WPLABSDEBUG-WK")){_XLOG_
		GlobalDeleteAtom(GlobalFindAtom("WPLABSDEBUG-WK"));
	}
	CDebugWPHelper::isDebug()=0;
	TRACETHREAD;
}

void EjectCDIfDiskPresent(TCHAR c)
{_XLOG_
	CString sPath=Format("%c://",c);
	if(isFileExist(sPath)){_XLOG_
		OpenCD(c,FALSE);
	}
}

void AppMainDlg::OnEndSession(BOOL bEnding) 
{_XLOG_
	// Говорим что системы выключается
    ::CreateMutex( NULL, TRUE, SHUTDOWN_NAME);
	OnBeforeShutdown();
	CDialog::OnEndSession(bEnding);
	if(bEnding){_XLOG_
		Finalize();
	}	
}

void AppMainDlg::PostNcDestroy() 
{_XLOG_
	pMainDialogWindow=NULL;
	CDialog::PostNcDestroy();
	delete this;
}

void AppMainDlg::OnHotmenuExitmenu() 
{_XLOG_
	::SetForegroundWindow(objSettings.hHotMenuWnd);
}

void AppMainDlg::KillWndProcess(HWND hUnderCur)
{_XLOG_
	if(hUnderCur){_XLOG_
		DWORD dwProcID=0;
		GetWindowThreadProcessId(hUnderCur,&dwProcID);
		if(!KillProcess(dwProcID)){_XLOG_
			AddError(_l("Unable to kill process"));
		}
	}
}

void AppMainDlg::OnHotmenuKillprocessUnderCur()
{_XLOG_
	CPoint pt;
	GetCursorPos(&pt);
	KillWndProcess(::WindowFromPoint(pt));
}

void AppMainDlg::OnHotmenuKillprocess() 
{_XLOG_
	CString szTitle;
	SafeGetWindowText(objSettings.hHotMenuWnd,szTitle);
	DWORD dwProcID=0;
	GetWindowThreadProcessId(objSettings.hHotMenuWnd,&dwProcID);
	CString sTitle=TrimMessage(szTitle,30,4);
	if(!KillProcess(dwProcID,sTitle)){_XLOG_
		AddError(_l("Unable to kill process"));
	}
}

void AppMainDlg::OnHotmenuSemitransparent() 
{_XLOG_
	bLastHalfer=!bLastHalfer;
	// Ставим параметр окна
	if(bLastHalfer){_XLOG_
		SetLayeredWindowStyle(objSettings.hHotMenuWnd,TRUE);
		SetWndAlpha(objSettings.hHotMenuWnd,50,TRUE);
	}else{_XLOG_
		SetWndAlpha(objSettings.hHotMenuWnd,100,TRUE);
		SetLayeredWindowStyle(objSettings.hHotMenuWnd,FALSE);
	}
}

void AppMainDlg::OnHotmenuTransparency100() 
{_XLOG_
	SetWndAlpha(objSettings.hHotMenuWnd,100,TRUE);
	SetLayeredWindowStyle(objSettings.hHotMenuWnd,FALSE);
	//SetLayeredWindowStyle(objSettings.hHotMenuWnd,TRUE);
}

void AppMainDlg::OnHotmenuTransparency20() 
{_XLOG_
	SetLayeredWindowStyle(objSettings.hHotMenuWnd,TRUE);
	SetWndAlpha(objSettings.hHotMenuWnd,20,TRUE);
}

void AppMainDlg::OnHotmenuTransparency40() 
{_XLOG_
	SetLayeredWindowStyle(objSettings.hHotMenuWnd,TRUE);
	SetWndAlpha(objSettings.hHotMenuWnd,40,TRUE);
}

void AppMainDlg::OnHotmenuTransparency60() 
{_XLOG_
	SetLayeredWindowStyle(objSettings.hHotMenuWnd,TRUE);
	SetWndAlpha(objSettings.hHotMenuWnd,60,TRUE);
}

void AppMainDlg::OnHotmenuTransparency80() 
{_XLOG_
	SetLayeredWindowStyle(objSettings.hHotMenuWnd,TRUE);
	SetWndAlpha(objSettings.hHotMenuWnd,80,TRUE);
}

void AppMainDlg::OnHotmenuFreememory() 
{_XLOG_
	static long lLock=0;
	if(lLock>0){_XLOG_
		return;
	}
	__FLOWLOG;
	DWORD dwStart=GetTickCount();
	SimpleTracker tr(lLock);
	CSplashParams par;
	FillSplashParams("",par);
	par.szText=_l("Optimizing memory")+"...";
	CSplashWindow* SplWnd=0;
	if(objSettings.lDeskMemOptOSD){_XLOG_
		SplWnd=new CSplashWindow(&par);
	}
	if(objSettings.lOptimizeRegistryON){_XLOG_
		RegFlushKey(HKEY_CLASSES_ROOT);
		RegFlushKey(HKEY_CURRENT_CONFIG);
		RegFlushKey(HKEY_CURRENT_USER);
		RegFlushKey(HKEY_LOCAL_MACHINE);
		RegFlushKey(HKEY_PERFORMANCE_DATA);
		RegFlushKey(HKEY_USERS);
	}
	if(objSettings.bUnderWindowsNT){_XLOG_
		BOOL bBreakMenu=FALSE;
		CCSLock lpc(&pCahLock,1);
		EnumProcesses();
		for(int i=0;i<GetProccessCash().GetSize();i++){_XLOG_
			HANDLE hProcess=OpenProcess(PROCESS_SET_QUOTA,FALSE,(GetProccessCash()[i]).dwProcId);
			if(hProcess){_XLOG_
				SetProcessWorkingSetSize(hProcess, -1, -1);
			}
		}
	}
	DWORD dwStop=GetTickCount();
	long lDifference=dwStop-dwStart;
	if(lDifference<3000){_XLOG_
		Sleep(3000-lDifference);
	}
	if(SplWnd){_XLOG_
		delete SplWnd;
	}
}

afx_msg LRESULT AppMainDlg::OnEDITTEXT(WPARAM wParam, LPARAM lParam)
{_XLOG_
	CString* sTitle=(CString*)wParam;
	CString* sValue=(CString*)lParam;
	CDLG_EditMLine dlg;
	dlg.sTitle=*sTitle;
	dlg.sText=*sValue;
	if(dlg.DoModal()==IDOK){_XLOG_
		*sValue=dlg.sText;
	}
	return 0;
}

afx_msg LRESULT AppMainDlg::OnFireAlert(WPARAM wParam, LPARAM lParam)
{_XLOG_
if(spyMode()){
	return 0;
}
 
	InfoWndStruct* info=(InfoWndStruct*)lParam;
	if(info->bAsModal){_XLOG_
		InfoWnd infoWnd(info,info->pParent?info->pParent:this);
		return infoWnd.DoModal();
	}else{_XLOG_
		InfoWnd* infoWnd=new InfoWnd(info);
		infoWnd->Create(InfoWnd::IDD,this);
		return LRESULT(infoWnd);
	}
} 

void KillSpecChars(CString& sStr)
{_XLOG_
	char* szBuffer=sStr.GetBuffer(0);
	while(*szBuffer){_XLOG_
		switch(*szBuffer){_XLOG_
		case ';':
		case ':':
		case '\'':
		case '\"':
		case '/':
		case '?':
		case '.':
		case ',':
		case '<':
		case '>':
		case '!':
		case '@':
		case '#':
		case '$':
		case '%':
		case '^':
		case '`':
		case '&':
		case '*':
		case '(':
		case ')':
		case '-':
		case '=':
		case '+':
		case '[':
		case ']':
		case '{':
		case '}':
		case '\\':
		case '|':
		case '~':
		case '№':
			*szBuffer='_';
		default:
			break;
		}
		szBuffer++;
	}
}

BOOL CheckEqualNoSpecChars(CString sOne,CString sTwo)
{_XLOG_
	KillSpecChars(sOne);
	KillSpecChars(sTwo);
	return (sOne==sTwo);
}

DWORD WINAPI FlipScreen(LPVOID p)
{_XLOG_
	HWND h=::GetForegroundWindow();
	if(h){_XLOG_
		CDesktopDC dcScreen;
		CRect rRECT;
		::GetWindowRect(h,&rRECT);
		dcScreen->InvertRect(&rRECT);
		Sleep(4000);
		dcScreen->InvertRect(&rRECT);
	}
/*
	HWND h=::GetDesktopWindow();
	if(h){_XLOG_
		CRect rRECT;
		::GetWindowRect(h,&rRECT);
		HWND wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_DISABLED|WS_VISIBLE, rRECT.left, rRECT.top, rRECT.Width(), rRECT.Height(), 0, 0, GetModuleHandle(0), 0);
		HDC dc=::GetDC(wndShotPhase2);
		::InvertRect(dc,&rRECT);
		Sleep(500);
		::InvertRect(dc,&rRECT);
		::ReleaseDC(wndShotPhase2,dc);
		DestroyWindow(wndShotPhase2);
	}
*/
	return 0;
}

void AppMainDlg::SetKBLayout(HKL hWhat)
{_XLOG_
	if(hWhat!=NULL){_XLOG_
		::PostMessage(::GetForegroundWindow(),WM_INPUTLANGCHANGEREQUEST,0,LPARAM(hWhat));//objSettings.hFocusWnd
	}else{_XLOG_
		::PostMessage(::GetForegroundWindow(),WM_INPUTLANGCHANGEREQUEST,0x0002,0);//INPUTLANGCHANGE_FORWARD
	}
	if(objSettings.lLayEffectType==1){_XLOG_
		AsyncPlaySound(CONV_TEXT);
	}
	if(objSettings.lLayEffectType==2){_XLOG_
		FORK(FlipScreen,0);
	}
}

void ParseLayout12(CString& s)
{_XLOG_
	for(int i=0;i<s.GetLength();i++){_XLOG_
		int iL1Pos=objSettings.sLayoutP1.Find(s[i]);
		if(iL1Pos!=-1){_XLOG_
			s.SetAt(i,objSettings.sLayoutP2[iL1Pos]);
		}
	}
}

extern long g_lChainedClipboardOperations;
long lConSelInProgress=0;
HANDLEINTHREAD(AppMainDlg,OnHotmenuConvertselected)
{_XLOG_
	__FLOWLOG;
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);
	BOOL bLastWord=0;
	if(pValue!=0){_XLOG_
		bLastWord=1;
	}
	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	SimpleTracker trCO(g_lChainedClipboardOperations);
	BOOL bRes=FALSE;
	SetCurrentDesktop();
	FLOGWND("ConvSelW 2: ",GetForegroundWindow());
#ifdef _DEBUG
	AsyncPlaySound(CString(GetApplicationDir())+"clip_1.wav");
#endif
	CString sText=GetSelectedTextInFocusWnd();
	FLOGWND("ConvSelW 3: ",GetForegroundWindow());
	if(sText==""){_XLOG_
		return 0;
	}
	if(IsBMP(sText)){_XLOG_
		return 0;
	}
	CString sPrefix;
	if(bLastWord){_XLOG_
		int iWord=-1;
		CString sWordDelims=" \t\r\n";
		BOOL bWhiteSpaceSkipped=0;
		for(int i=sText.GetLength()-2;i>=0;i--){_XLOG_
			if(sWordDelims.Find(sText[i])!=-1){_XLOG_
				if(bWhiteSpaceSkipped==0){_XLOG_
					continue;
				}
				iWord=i;
				break;
			}
			bWhiteSpaceSkipped=1;
		}
		if(iWord!=-1){_XLOG_
			sPrefix=sText.Left(iWord+1);
			sText=sText.Mid(iWord+1);
		}
	}
#ifdef _DEBUG
		CString sOrigText=sText;
#endif
	DWORD dwProcID=0;
	FLOGWND("ConvSelW 4: ",GetForegroundWindow());
	DWORD dwThreadId=GetWindowThreadProcessId(objSettings.hFocusWnd,&dwProcID);
	FLOGWND("ConvSelW 5: ",GetForegroundWindow());
	HKL hStartLayout=GetKeyboardLayout(dwThreadId);
	FLOGWND("ConvSelW 6: ",GetForegroundWindow());
	HKL hLayoutToSetTo=0;
	long l1=objSettings.lHKL1;
	long l2=objSettings.lHKL2;
	//GetWindowThread
	if(sText!=""){_XLOG_
		CString sCopy;
		if(sCopy=="" || CheckEqualNoSpecChars(sText,sCopy)){_XLOG_
			FLOGWND("ConvSelW 7: ",GetForegroundWindow());
			if(ConvertSelected(hStartLayout, objSettings.aKBLayouts[l1], sText, sCopy)){_XLOG_
				hLayoutToSetTo=objSettings.aKBLayouts[l1];
			}
		}
		if(sCopy=="" || CheckEqualNoSpecChars(sText,sCopy)){_XLOG_
			FLOGWND("ConvSelW 8: ",GetForegroundWindow());
			if(ConvertSelected(hStartLayout, objSettings.aKBLayouts[l2], sText, sCopy)){_XLOG_
				hLayoutToSetTo=objSettings.aKBLayouts[l2];
			}
		}
		// А теперь пробуем в тойже раскладке!
		// т.е. без одной проверки
		if(sCopy=="" || CheckEqualNoSpecChars(sText,sCopy)){_XLOG_
			FLOGWND("ConvSelW 9: ",GetForegroundWindow());
			if(ConvertSelected(objSettings.aKBLayouts[l2], objSettings.aKBLayouts[l1], sText, sCopy)){_XLOG_
				hLayoutToSetTo=objSettings.aKBLayouts[l1];
			}
		}
		if(sCopy=="" || CheckEqualNoSpecChars(sText,sCopy)){_XLOG_
			FLOGWND("ConvSelW A: ",GetForegroundWindow());
			if(ConvertSelected(objSettings.aKBLayouts[l1], objSettings.aKBLayouts[l2], sText, sCopy)){_XLOG_
				hLayoutToSetTo=objSettings.aKBLayouts[l2];
			}
		}
		if(sCopy!="" && sText!=sCopy){_XLOG_
			sCopy=sPrefix+sCopy;
			ParseLayout12(sCopy);
			FLOGWND("ConvSelW B: ",GetForegroundWindow());
#ifdef _DEBUG
	AsyncPlaySound(CString(GetApplicationDir())+"clip_2.wav");
#endif
			if(PutSelectedTextInFocusWnd(sCopy,hLayoutToSetTo)){_XLOG_
				bRes=TRUE;
			}
			FLOGWND("ConvSelW C: ",GetForegroundWindow());
		}
		//ActivateKeyboardLayout(hStartLayout, 0);
	}
	if(bRes){_XLOG_
		AsyncPlaySound(CONV_TEXT);
	}else{_XLOG_
		AsyncPlaySound(SND_ERROR);
	}
	// В фотошопе и другших прогах где текст выдирается не из отдельного окна а из нарисованного
	// прогой, это сбрасывает фокус!
	//::SetForegroundWindow(objSettings.hHotMenuWnd);
	return 1;
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuConvertselectedLW)
{_XLOG_
/*#ifdef _DEBUG
	char szDsc[256]="";
	char szDsc2[256]="";
	HWND hFore=objSettings.hHotMenuWnd;
	if(hFore){_XLOG_
		::GetWindowText(hFore,szDsc,sizeof(szDsc));
	}
	HWND hWin=0;
	hWin=GetLastFocusWindowByHook(hWin);
	::GetWindowText(hWin,szDsc2,sizeof(szDsc2));
	ShowBaloon(Format("%08x Switching in %s, foreg=%08x\n LastByHook=%08x %s",hFore,szDsc,GetForegroundWindow(),hWin,szDsc2),"SwitchLD");
#endif*/
	/*
	// Здесь не надо так как дергается OnHotmenuConvertselected_InThread
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);
	*/
	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	WaitWhileAllKeysAreFreeX();
//	BlockInputMy(TRUE);
	::keybd_event(VK_SHIFT, 1,KEYEVENTF_EXTENDEDKEY|0,0);//KEYEVENTF_EXTENDEDKEY|
	Sleep(10);
	::keybd_event(VK_HOME, ::MapVirtualKey(VK_HOME,0),KEYEVENTF_EXTENDEDKEY|0,0);
	Sleep(10);
	::keybd_event(VK_HOME, ::MapVirtualKey(VK_HOME,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
	Sleep(10);
	::keybd_event(VK_SHIFT, 1,KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);//KEYEVENTF_EXTENDEDKEY|
	Sleep(100);
	if(OnHotmenuConvertselected_InThread((LPVOID)objSettings.lLastWordNotLine)==0){_XLOG_
		::keybd_event(VK_END, ::MapVirtualKey(VK_END,0),KEYEVENTF_EXTENDEDKEY|0,0);
		::keybd_event(VK_END, ::MapVirtualKey(VK_END,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
	}
//	BlockInputMy(FALSE);
	return 0;
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuUppercase)
{_XLOG_
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);

	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	SimpleTracker trCO(g_lChainedClipboardOperations);
	CString sText=GetSelectedTextInFocusWnd();
	sText.MakeUpper();
	PutSelectedTextInFocusWnd(sText);
	return 0;
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuLowercase)
{_XLOG_
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);

	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	SimpleTracker trCO(g_lChainedClipboardOperations);
	CString sText=GetSelectedTextInFocusWnd();
	sText.MakeLower();
	PutSelectedTextInFocusWnd(sText);	
	return 0;
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuSwitchcase)
{_XLOG_
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);

	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	SimpleTracker trCO(g_lChainedClipboardOperations);
	SetCurrentDesktop();
	CString sText=GetSelectedTextInFocusWnd();
	int iCount=0;
	for(int i=0;i<sText.GetLength();i++){_XLOG_
		CString sChar=sText.GetAt(i);
		CString sUpChar=sChar;
		sUpChar.MakeUpper();
		if(sChar==sUpChar){_XLOG_
			iCount++;
		}
	}
	if(iCount>int(sText.GetLength()/2)){_XLOG_
		sText.MakeLower();
	}else{_XLOG_
		sText.MakeUpper();
	}
	PutSelectedTextInFocusWnd(sText);
	AsyncPlaySound(CONV_TEXT);
	return 0;
}

DWORD WINAPI OnHotmenuSortLines(LPVOID pType)
{_XLOG_
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);

	int iCase=(int)pType;
	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	SimpleTracker trCO(g_lChainedClipboardOperations);
	SetCurrentDesktop();
	CString sText=GetSelectedTextInFocusWnd();
	if(sText==""){_XLOG_
		return 0;
	}
	CStringArray aLines;
	int iType=sText.Replace("\r\n","\n");
	ConvertComboDataToArray(sText,aLines,'\n');
	bool bAscend=((iCase==1)||(iCase==3));
	bool bAsNumber=((iCase==3)||(iCase==4));
	if(pType==0){_XLOG_
		if(aLines.GetSize()>0){_XLOG_
			if(aLines[0].GetLength()>0){_XLOG_
				bAsNumber=atol(aLines[0])>0;
			}
			if(aLines.GetSize()>2){_XLOG_
				if(aLines[0]>aLines[1]){_XLOG_
					bAscend=true;
				}
			}
		}
	}
	for(int i=0;i<aLines.GetSize()-1;i++){_XLOG_
		for(int j=i+1;j<aLines.GetSize();j++){_XLOG_
			bool bSwap=false;
			if(bAscend){_XLOG_
				if(bAsNumber?(atof(aLines[i])>atof(aLines[j])):(aLines[i]>aLines[j])){_XLOG_
					bSwap=true;
				}
			}else{_XLOG_
				if(bAsNumber?(atof(aLines[i])<atof(aLines[j])):(aLines[i]<aLines[j])){_XLOG_
					bSwap=true;
				}
			}
			if(bSwap){_XLOG_
				CString sTmp=aLines[j];
				aLines[j]=aLines[i];
				aLines[i]=sTmp;
			}
		}
	}
	sText="";
	for(int k=0;k<aLines.GetSize();k++){_XLOG_
		sText+=aLines[k];
		sText+="\n";
	}
	if(iType>0){_XLOG_
		sText.Replace("\n","\r\n");
	}
	if(PutSelectedTextInFocusWnd(sText)){_XLOG_
		AsyncPlaySound(CONV_TEXT);
	}
	return 0;
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuCountcharacters)
{_XLOG_
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);

	SetCurrentDesktop();
	CString sText=GetSelectedTextInFocusWnd();
	Alert(_l("Characters in selection")+Format(": %i",strlen(sText)));
	return 0;
}

void AppMainDlg::OnReattachClips()
{
	//AttachToClipboard(TRUE);
	PostMessage(WM_COMMAND,ID_REATTACHCLIP_REALLY,0);
}

void AppMainDlg::OnSystrayAbout() 
{_XLOG_
#ifdef _DEBUG
	ShowRegNotice(DEFAULT_REGNOTICE2);
	ShowRegNotice(DEFAULT_REGNOTICE3);
#endif
	if(wpLabsLink.bIsCreated==0){_XLOG_
		wpLabsLink.create(IDC_STATIC_URL,GetSafeHwnd());
		strcpy(wpLabsLink.m_Url,CString("http://www.wiredplane.com/cgi-bin/wp_engine.php?target=none&who="PROGNAME)+PROG_VERSION);
		strcpy(wpLabsLink.m_UrlVisible,"http://www.wiredplane.com/");
	}
	SetRegInfo();
	SetTaskbarButton(this->GetSafeHwnd());
	m_STray.ShowIcon();
	::EnableWindow(GetSafeHwnd(),TRUE);
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	ShowWindow(SW_SHOW);
	SwitchToWindow(GetSafeHwnd(),TRUE);
	if(objSettings.bApplicationFirstRun){_XLOG_
		ShowBaloon(_l("Right-click the system tray icon to access all features. Choose Help from the menu for an overview of the features"),PROGNAME,10000,0);
	}	
}

void AppMainDlg::OnOK() 
{_XLOG_
	ShowWindow(SW_HIDE);
	if(IsIconMustBeHided()){_XLOG_
		m_STray.HideIcon();
	}
	//::EnableWindow(GetSafeHwnd(),FALSE);
}

void AppMainDlg::OnEntcode() 
{_XLOG_
	OnOK();//Скрываем...
	PostMessage(WM_COMMAND,ID_SYSTRAY_OPTIONS,0);
}

DWORD WINAPI Tutorial(LPVOID data)
{_XLOG_
	ShowHelp("tutorial");
	return 0;
}

void AppMainDlg::OnWebsite() 
{_XLOG_
	FORK(Tutorial,NULL);
}

void SubstMyVariablesForScreenshot(CString& sWhat, CString sTitle)
{_XLOG_
	sWhat.Replace("%TITLE",sTitle);
	sWhat.Replace("%COUNT",Format("%lu",objSettings.iGlobalScrShotCount));
}

void AppMainDlg::ScrollAction(int iAction)
{_XLOG_
	HWND hFgw=::GetForegroundWindow();
	DWORD dwCurWinProcID=0;
	objSettings.hFocusWnd=NULL;
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(hFgw,&dwCurWinProcID);
	// Получаем элемент управления с фокусом
	MYGUITHREADINFO gui;
	if(GetGUIThreadInfoMy(dwCurWinThreadID,&gui)){_XLOG_
		hFgw=gui.hwndFocus;
		if(hFgw==NULL){_XLOG_
			hFgw=gui.hwndCaret;
		}
		if(hFgw==NULL){_XLOG_
			hFgw=gui.hwndActive;
		}
	}
	int iLines=objSettings.lNumberOfLinesToScroll;
	if(hFgw){_XLOG_
		DWORD dwMsg=0,dwPar=0;
		switch(iAction)
		{_XLOG_
		case SCROLL_UP:
			dwMsg=WM_VSCROLL;
			dwPar=SB_LINEUP;
			break;
		case SCROLL_DOWN:
			dwMsg=WM_VSCROLL;
			dwPar=SB_LINEDOWN;
			break;
		case SCROLL_LEFT:
			dwMsg=WM_HSCROLL;
			dwPar=SB_LINELEFT;
			break;
		case SCROLL_RIGHT:
			dwMsg=WM_HSCROLL;
			dwPar=SB_LINERIGHT;
			break;
		case SCROLL_PAGEUP:
			dwMsg=WM_VSCROLL;
			dwPar=SB_PAGEUP;
			iLines=1;
			break;
		case SCROLL_PAGEDN:
			dwMsg=WM_VSCROLL;
			dwPar=SB_PAGEDOWN;
			iLines=1;
			break;
		}
		for(int i=0;i<iLines;i++){_XLOG_
			::SendMessage(hFgw,dwMsg,dwPar,NULL);
		}
	}
	return;
}

static long lMinimizeAction=objSettings.bShotActiveWindow;
HANDLEINTHREAD(AppMainDlg,OnMinimizeActiveWindow)
{_XLOG_
	long lAction=0;
	IsThreadDoubleCalled(lAction);
	static CArray<HWND,HWND> aMinWindws;
	if(lAction==0){_XLOG_
		if(pMainDialogWindow){_XLOG_
			aMinWindws.Add(objSettings.hHotMenuWnd);
			pMainDialogWindow->HideApplication(objSettings.hHotMenuWnd,5);
		}
	}else{ 
		int iLen=aMinWindws.GetSize();
		if(iLen>0){_XLOG_
			HWND hLast=aMinWindws[iLen-1];
			aMinWindws.RemoveAt(iLen-1);
			::PostMessage(hLast,WM_SYSCOMMAND,SC_RESTORE,0);
			::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hLast);
		}
	}
	return 0;
}

static long lScrshotAction=0;
static long lSeqScreenshowInProgress=0;
void AppMainDlg::OnSystrayScrShot()
{_XLOG_
	if(TuneUpItem(_l(SCRSHOT_TOPIC))){_XLOG_
		return;
	}
	lScrshotAction=0;
	lSeqScreenshowInProgress=0;
	TakeScreenshot();
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuTakescreenshot)
{_XLOG_
	lSeqScreenshowInProgress=0;
	IsThreadDoubleCalled(lScrshotAction);
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->TakeScreenshot();
	}
	return 0;
}

BOOL SaveBitmapToFile(CString& sPictureFile, CBitmap* bmpResult)
{_XLOG_
	BOOL bSavedOk=0;
	if(objSettings.bUseJpg && bmpResult){_XLOG_
		sPictureFile+=".jpg";
		PreparePathForWrite(sPictureFile);
		bSavedOk=SaveBitmapToJpg(sPictureFile,*bmpResult,(BYTE)objSettings.bUseJpgQuality);
	}else{_XLOG_
		sPictureFile+=".bmp";
		PreparePathForWrite(sPictureFile);
		HBITMAP hBmp=*bmpResult;
		if(SaveBitmapToDisk(hBmp,sPictureFile,pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():(::GetDesktopWindow()))){_XLOG_
			bSavedOk=TRUE;
		}
	}
	return bSavedOk;
}

BOOL CallBMPEdit(CString sPictureFile)
{_XLOG_
	CString sCommand=objSettings.sJpgEditor;
	CString sParameter="\""+sPictureFile+"\"";
	if(sCommand==""){_XLOG_
		sCommand=sPictureFile;
		sParameter="";
	}else{_XLOG_
		FindFullPathToExecutable(sCommand);
	}
	if((int)::ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():0,"open",sCommand,sParameter,NULL,SW_SHOWNORMAL)<=32){_XLOG_
		Alert(_l("Associated editor not found")+"!");
#ifdef _DEBUG
		Alert(sCommand+"::"+sParameter);
#endif
		return 0;
	}
	return 1;
}

#define xCAPTUREBLT          (DWORD)0x40000000 /* Include layered windows */ 
void AppMainDlg::TakeScreenshot()
{_XLOG_
	static long lLock=0;
	if(lLock>0){_XLOG_
		return;
	}
	SimpleTracker tr(lLock);
	long lCurrentScreenshotAction=lScrshotAction?objSettings.bShotActiveWindow2:objSettings.bShotActiveWindow;
	static CString sLastActiveWindowTitle;
	CWnd* wndShotPhase2=0;
	if(lCurrentScreenshotAction==2){_XLOG_
		SetCurrentDesktop();
		CRect rDesktopRECT;
		GetActiveWindowTitle(sLastActiveWindowTitle, rDesktopRECT);
		objSettings.rUserRect.left=objSettings.rUserRect.right=objSettings.rUserRect.top=objSettings.rUserRect.bottom=-99;
		BlockInputMy(TRUE);
		CRect rDesktopResolution;
		::GetWindowRect(::GetDesktopWindow(),&rDesktopResolution);
		wndShotPhase2 = new CWnd;
		CString szClass = ::AfxRegisterWndClass(0,theApp.LoadStandardCursor(IDC_CROSS),0,0);
		wndShotPhase2->CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, szClass, "WK_SCREENSHOT", 0, rDesktopResolution, NULL, 0);
		if(wndShotPhase2){_XLOG_
			wndShotPhase2->ModifyStyle(WS_CAPTION|WS_BORDER|WS_DLGFRAME|WS_OVERLAPPEDWINDOW,0);
			wndShotPhase2->ShowWindow(SW_SHOWNOACTIVATE);
			wndShotPhase2->EnableWindow(TRUE);
			::SetCapture(wndShotPhase2->GetSafeHwnd());
			SetCursor(theApp.LoadStandardCursor(IDC_CROSS));
		}
		BlockInputMy(FALSE);
		// Ждем пока нажмут гденить
		while(GetAsyncKeyState(VK_LBUTTON)>=0){_XLOG_
			Sleep(100);
		}
		//---------------------------------------------
		GetCursorPos(&objSettings.rUserRect.TopLeft());
		// Ждем пока отпустят гденить
		BOOL bEscaped=FALSE;
		GetCursorPos(&objSettings.rUserRect.BottomRight());
		StartScreenshotRect(objSettings.rUserRect,rDesktopResolution);
		while(GetAsyncKeyState(VK_LBUTTON)<0){_XLOG_
			if(GetAsyncKeyState(VK_ESCAPE)<0){_XLOG_
				bEscaped=TRUE;
				break;
			}
			CPoint pt,pt2(objSettings.rUserRect.BottomRight());
			GetCursorPos(&pt);
			objSettings.rUserRect.BottomRight()=pt;
			static BOOL bPrevState=FALSE;
			if(GetAsyncKeyState(VK_RBUTTON)<0){_XLOG_
				if(bPrevState==FALSE){_XLOG_
					bPrevState=TRUE;
					long l=objSettings.rUserRect.left;
					objSettings.rUserRect.left=objSettings.rUserRect.right;
					objSettings.rUserRect.right=l;
					long l2=objSettings.rUserRect.top;
					objSettings.rUserRect.top=objSettings.rUserRect.bottom;
					objSettings.rUserRect.bottom=l2;
					SetCursorPos(objSettings.rUserRect.right,objSettings.rUserRect.bottom);
				}
			}else{_XLOG_
				bPrevState=FALSE;
			}
			if(pt.x==pt2.x && pt.y==pt2.y){_XLOG_
				Sleep(50);
			}else{_XLOG_
				CString sTxt="";
				if(objSettings.lShowScrData){_XLOG_
					CRect rtPopup(objSettings.rUserRect);
					rtPopup.NormalizeRect();
					sTxt=Format("%s: %lu*%lu\n%s: (%lu,%lu)-(%lu,%lu)",_l("Area size"),rtPopup.Width(),rtPopup.Height(),_l("Position"),rtPopup.left,rtPopup.top,rtPopup.right,rtPopup.bottom);
				}
				ContScreenshotRect(objSettings.rUserRect,sTxt);
			}
		}
		// Стираем последний прямоугольник
		StopScreenshotRect();
		// Запоминаем последнюю позицию и убиваем окошко
		GetCursorPos(&objSettings.rUserRect.BottomRight());
		ReleaseCapture();
		if(wndShotPhase2 && objSettings.lScreenshotByTimer){_XLOG_
			delete wndShotPhase2;
			wndShotPhase2=0;
		}
		if(bEscaped){_XLOG_
			// Спаслись ескейпом :)
			return;
		}
	}
	//-------------------------
	CString sNewTitle;
	CRect rDesktopRECT;
	if(lCurrentScreenshotAction==2){_XLOG_
		sNewTitle=sLastActiveWindowTitle;
		rDesktopRECT=objSettings.rUserRect;
		objSettings.rUserRect.SetRectEmpty();
	}else{_XLOG_
		GetActiveWindowTitle(sNewTitle, rDesktopRECT);
	}
	CWnd* h_DeskTop=NULL;
	if(lCurrentScreenshotAction==1){_XLOG_
		if(::IsWindow(objSettings.hHotMenuWnd)){_XLOG_
			h_DeskTop=CWnd::FromHandle(objSettings.hHotMenuWnd);
			h_DeskTop->GetWindowRect(&rDesktopRECT);
		}else{_XLOG_
			// На всякий случай
			lSeqScreenshowInProgress=0;
		}
	}else {_XLOG_
		h_DeskTop=GetDesktopWindow();
		if(lCurrentScreenshotAction!=2){_XLOG_//==0
			//h_DeskTop->GetWindowRect(&rDesktopRECT);
			rDesktopRECT=GetMainMonitorRECT();
		}
	}
	//
	BOOL bNeedQualityResizing=FALSE;
	rDesktopRECT.NormalizeRect();
	CRect rScrSize(CPoint(0,0),rDesktopRECT.Size());
	if(objSettings.bFixXSize){_XLOG_
		bNeedQualityResizing=TRUE;
		rScrSize.right=rScrSize.left+objSettings.lFixXSizeTo;
		if(!objSettings.bFixYSize){// Если заданы оба размера, не подгоняем
			rScrSize.bottom=rScrSize.top+long(double(rScrSize.Height())*(double(rScrSize.Width())/rDesktopRECT.Width()));
		}
	}
	if(objSettings.bFixYSize){_XLOG_
		bNeedQualityResizing=TRUE;
		rScrSize.bottom=rScrSize.top+objSettings.lFixYSizeTo;
		if(!objSettings.bFixXSize){// Если заданы оба размера, не подгоняем
			rScrSize.right=rScrSize.left+long(double(rScrSize.Width())*(double(rScrSize.Height())/rDesktopRECT.Height()));
		}
	}
	if(rScrSize.IsRectEmpty()){_XLOG_
		lSeqScreenshowInProgress=0;
		AsyncPlaySound(SND_ERROR);
		return;
	}
	// Экран, с которого вытаскивается screenshot
	CDesktopDC dcScreen;
	lSeqScreenshowInProgress=1;
	while(lSeqScreenshowInProgress){_XLOG_
		// Выходное изображение
		CBitmap* bmpResult=NULL;
		CBitmap* pOld=NULL;
		CBitmap bmp;
		// Вспомогательный DC
		CDC pSHDC;
		pSHDC.CreateCompatibleDC(&dcScreen);
		pSHDC.SetStretchBltMode(COLORONCOLOR);
		BOOL bResized=FALSE;
		{// Маштабируем качественно или не маштабируем
			CBitmap bmp2;
			bmp2.CreateCompatibleBitmap(&dcScreen,rDesktopRECT.Width(),rDesktopRECT.Height());
			CDC pSHDC2;
			pSHDC2.CreateCompatibleDC(&dcScreen);
			CBitmap* pOld2=pSHDC2.SelectObject(&bmp2);
			pSHDC2.BitBlt(0, 0, rDesktopRECT.Width(),rDesktopRECT.Height(),&dcScreen, rDesktopRECT.left, rDesktopRECT.top, SRCCOPY|xCAPTUREBLT);
			if(wndShotPhase2){_XLOG_
				delete wndShotPhase2;
				wndShotPhase2=0;
			}
			if(objSettings.lAddCursorToScreenshot){_XLOG_
				CPoint pt;
				::GetCursorPos(&pt);
				HCURSOR hCur=::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
				ICONINFO inf;
				GetIconInfo(hCur,&inf);
				::DrawIconEx(pSHDC2,pt.x-rDesktopRECT.left-inf.xHotspot,pt.y-rDesktopRECT.top-inf.yHotspot,hCur,0,0,0,NULL,DI_DEFAULTSIZE|DI_NORMAL);
			}
			HBITMAP pOutBmp=NULL;
			bResized=CopyBitmapToBitmap(bmp2,pOutBmp,rDesktopRECT.Size(),&(rScrSize.Size()),bNeedQualityResizing?TRUE:FALSE);
			pSHDC2.SelectObject(pOld2);
			// Подготавливаем дальше...
			bmpResult=new CBitmap();
			bmpResult->Attach(pOutBmp);
			pOld=pSHDC.SelectObject(bmpResult);
		}
		// Подготавливаем к сохранению
		CString sPath=(lScrshotAction==0?objSettings.sScrshotDir:(objSettings.sScrshotDir2!=""?objSettings.sScrshotDir2:objSettings.sScrshotDir));
		AddSlashToPath(sPath);
		CString sFile=objSettings.sScrshotFile;
		CString sSign=objSettings.sSignString;
		SubstMyVariablesWK(sPath);
		SubstMyVariablesWK(sFile);
		SubstMyVariablesWK(sSign);
		CapitalizeAndShrink(sNewTitle," \t\r\n\\.%:+-*/!?,()[]{}^");
		SubstMyVariablesForScreenshot(sFile,sNewTitle);
		SubstMyVariablesForScreenshot(sSign,sNewTitle);
		SubstMyVariablesForScreenshot(sPath,sNewTitle);
		if(bResized && bmpResult){_XLOG_
			if(objSettings.bBlackAndWhiteConfirm==1){_XLOG_
				dcScreen->InvertRect(&rDesktopRECT);
			}
			DWORD dwStart=GetTickCount();
			MakeSafeFileName(sFile,objSettings.sSafeChar.GetLength()?objSettings.sSafeChar[0]:0);
			// Добавляем мерзкую подпись для незарегистрированных чайников...
			if(objSettings.iLikStatus!=2 && GetWindowDaysFromInstall()>UNREGDAY_SCR){_XLOG_
				objSettings.bSignSShot=TRUE;
				sSign=CString("["PROGNAME": ")+_l("Unregistered version")+". "+_l("Please register")+"!]";
			}
			if(objSettings.bSignSShot){_XLOG_
				CRect rt;
				rt=rScrSize;
				rt.InflateRect(-20,-20,-20,-20);
				SmartDrawText(pSHDC, sSign, rt, NULL, RGB(0,0,0), RGB(250,250,250), TXSHADOW_GLOW);
			}
			//
			CString sPictureFile=sPath+sFile;
			BOOL bSavedOk=SaveBitmapToFile(sPictureFile,bmpResult);
			if(!objSettings.lScreenshotByTimer){_XLOG_
				if(objSettings.bPutScreshotInClipboard && bmpResult){_XLOG_
					BITMAP bmpInfo;
					bmpInfo.bmBits=NULL;
					bmpResult->GetBitmap(&bmpInfo);
					CBitmap bmpNew;
					bmpNew.CreateBitmapIndirect(&bmpInfo);
					DWORD dwCount=bmpInfo.bmHeight*bmpInfo.bmWidth*bmpInfo.bmPlanes*bmpInfo.bmBitsPixel;//bmpInfo.bmWidthBytes
					BYTE* lpBits=new BYTE[dwCount];
					bmpResult->GetBitmapBits(dwCount, lpBits);
					bmpNew.SetBitmapBits(dwCount, lpBits);
					CopyBitmapToClipboard(this,bmpNew,NULL);
					delete[] lpBits;
				}
			}
			if(objSettings.bBlackAndWhiteConfirm==1){_XLOG_
				DWORD dwStop=GetTickCount();
				if(dwStop-dwStart<150){_XLOG_
					Sleep((dwStart+150)-dwStop);
				}
				dcScreen->InvertRect(&rDesktopRECT);
			}
			if(bSavedOk){_XLOG_
				if(!objSettings.lScreenshotByTimer){_XLOG_
					if(objSettings.bBlackAndWhiteConfirm==2){_XLOG_
						Alert(_l("Screenshot successfully saved to")+"\n"+TrimMessage(sPictureFile,60,3),5000,FALSE,DT_CENTER|DT_VCENTER);
					}
				}
				objSettings.sLastScreenshot=sPictureFile;
				objSettings.iGlobalScrShotCount++;
				AsyncPlaySound(TAKE_SSHOT);
				if(!objSettings.lScreenshotByTimer){_XLOG_
					if(objSettings.bAutoEditOnShot==3 || (objSettings.bAutoEditOnShot==1 && lScrshotAction==0) || (objSettings.bAutoEditOnShot==2 && lScrshotAction!=0)){_XLOG_
						CallBMPEdit(sPictureFile);
					}
				}
			}
		}
		if(pOld){_XLOG_
			pSHDC.SelectObject(pOld);
		}
		if(!objSettings.lScreenshotByTimer){_XLOG_
			lSeqScreenshowInProgress=0;
		}
		if(lSeqScreenshowInProgress){_XLOG_
			if(objSettings.lScreenshotByTimerSec<5){_XLOG_
				objSettings.lScreenshotByTimerSec=5;
			}
			Sleep(objSettings.lScreenshotByTimerSec*1000);
		}
		delete bmpResult;
	}
	return;
}

afx_msg LRESULT AppMainDlg::ONWM_THIS(WPARAM wParam, LPARAM lParam)
{_XLOG_
	// wParam==
	// 0 - прога запускается
	// 1 - останавливается
	// 3 - нужно зарегисть клавиши на десктопе lParam
	// 99 - активируется
	if(wParam==3 && lParam==objSettings.lLastActiveDesktop){_XLOG_
		// Включаем плагины
		PrepareDesktopSwitch(FALSE);
		// Восстанавливаем иконка на раб.столе
		if(objSettings.lRemeberIconsPosInDesktops){_XLOG_
			RestoreDesktopIcons();
		}
		if(objSettings.lDeskNeedOSD){_XLOG_
			CSplashParams par;
			par.sTextPrefix="";
			par.sTextPosfix="";
			FillSplashParams(wk.GetDesktopName(objSettings.lLastActiveDesktop),par);
			CSplashWindow* SplWnd=new CSplashWindow(&par);
			SplWnd->AutoClose(objSettings.lOSDSecs*1000);
		}
		SetWindowIcon(0,1);
		// Боремся с глюком что на ноовм десктопе не показывается иконка ВК
		m_STray.HideIcon();
		m_STray.ShowIcon();
		return 1;
	}
	if(wParam==99 && lParam==99){_XLOG_
		PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN,0);
		return 1;
	}
	return 0;
}

// wParam=0 - старт родительской проги
// wParam=1 - останов родительской проги
// wParam=2 - активация программы WireKeys
afx_msg LRESULT AppMainDlg::ONWM_WIRENOTE(WPARAM wParam, LPARAM lParam)
{_XLOG_
if(spyMode()){
	return 0;
}
	if(wParam>2){// Это не к нам...
		return 0;
	}
	if(wParam==2){_XLOG_
		OnSystrayOptions();
		return 0;
	}
	if(IsIconMustBeHided() && wParam!=1){_XLOG_
		m_STray.HideIcon();
	}else{_XLOG_
		m_STray.ShowIcon();
	}	
	return 0;
}

void AppMainDlg::OnSystrayHelp() 
{_XLOG_
	ShowHelp("Overview");	
}

void AppMainDlg::OnSystraySendasuggestion()
{_XLOG_
	SendSuggestion(PROGNAME,PROG_VERSION);
}

void AppMainDlg::OnSystrayDisplayProps()
{_XLOG_
	AppDisplayProperties();
}

void AppMainDlg::OnSystrayAddRemProgs()
{_XLOG_
	AppAddRemPrograms();
}

void AppMainDlg::OnHotmenuTxtPars()
{_XLOG_
	objSettings.OpenOptionsDialog(0,SELTXT_TOPIC,this);
}

void AppMainDlg::OnSystrayProcListPars()
{_XLOG_
	objSettings.OpenOptionsDialog(0,PROCLIST_TOPIC,this);
}

HANDLEINTHREAD3(AppMainDlg,OnSystraySaveIcons,if(TuneUpItem(SAVEICONS_TOPIC)){return;})
{_XLOG_
	wk.SaveIconsPos(NULL,NULL);
	return 0;
}

HANDLEINTHREAD3(AppMainDlg,OnSystrayRestoreIcons,if(TuneUpItem(SAVEICONS_TOPIC)){return;})
{_XLOG_
	wk.RestoreIconsPos(NULL,NULL);
	return 0;
}

// 0: Bring into view
// 1: Switch On-top state
// 2: Hide to tray
// 3: Hide completely
// 4: Maximize/Restore
// 5: Minimize
// 6: Maximize
// 7: Rollup (title bar and app. menu)/Unroll
// 8: Rollup (title bar only)/Unroll
// 9: Hide detached
// 10: Close window
// 11: Создать ярлык
// 12: Убить приложение
// 13: 100%ая прозарчность и обратно
// 14: Floater anyway
// 15: Tray anyway

#pragma message("---------------------------- проверять WS_EX_NOACTIVATE/disabled при переводах фокуса!")
void AppMainDlg::HideApplication(HWND hWin, DWORD dwHideType) 
{_XLOG_
	if(IsWindowsSystemWnd(hWin)){_XLOG_
		return;
	}
	if(dwHideType==11){_XLOG_
		::CoInitialize(NULL);
		GetWindowTopParent(hWin);
		CreateLinkInFolder(getDesktopPath(),hWin);
		::CoUninitialize();
		return;
	}
	if(dwHideType==12){_XLOG_
		GetWindowTopParent(hWin);
		KillWndProcess(hWin);
		return;
	}
	if(dwHideType==10){_XLOG_
		GetWindowTopParent(hWin);
		::PostMessage(hWin,WM_CLOSE,0,0);
		return;
	}
	if(dwHideType==13){_XLOG_
		GetWindowTopParent(hWin);
		if((GetWindowLong(hWin, GWL_EXSTYLE) & (0x00080000L))==0){_XLOG_
			SetLayeredWindowStyle(hWin,TRUE);
			SetWndAlpha(objSettings.hHotMenuWnd,0,TRUE);
		}else{_XLOG_
			SetLayeredWindowStyle(hWin,FALSE);
			::RedrawWindow(hWin,NULL,NULL,RDW_ERASE|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
		}
		return;
	}
	// Определяем, заrollupлено ли окно
	CRect rt;
	WINDOWPLACEMENT pl;
	static CMap<HWND,HWND&,WINDOWPLACEMENT,WINDOWPLACEMENT&> aWindowPositions;
	BOOL bWndRolledUp=aWindowPositions.Lookup(hWin,pl);
	// Устанавливаем текущий десктоп - и поехали
	SetCurrentDesktop();
	if(dwHideType==0){_XLOG_
		CRect rt(0,0,0,0);
		::GetWindowRect(hWin,&rt);
		CSmartWndSizer::TestOnScreenOut(rt,objSettings.iStickPix);
		::MoveWindow(hWin,rt.left,rt.top,rt.Width(),rt.Height(),TRUE);
	}
	if(dwHideType==1){_XLOG_
		CRect rt(0,0,0,0);
		::GetWindowRect(hWin,&rt);
		DWORD bOnTop=GetWindowLong(hWin, GWL_EXSTYLE)&WS_EX_TOPMOST;
		::SetWindowPos(hWin,bOnTop?HWND_NOTOPMOST:HWND_TOPMOST,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOSENDCHANGING);
	}
	if(dwHideType==2 || dwHideType==3 || dwHideType==9 || dwHideType==14 || dwHideType==15){_XLOG_
		// Смотрим, не десктоп/таскбар ли это?
		if(IsWindowsSystemWnd(hWin)){_XLOG_
			return;
		}
		BOOL bForceMinType=0;
		if(dwHideType==14){_XLOG_
			bForceMinType=1;
			dwHideType=2;
		}
		if(dwHideType==15){_XLOG_
			bForceMinType=1;
			dwHideType=9;
		}
		HWND hWin2=hWin;
		GetWindowTopParent(hWin2);
		if(IsWindowsSystemWnd(hWin2)){_XLOG_
			return;
		}
		DWORD dwVisibility=::IsWindowVisible(hWin);// Видимость определяем по самому окну, а не паренту!!!
		BOOL bIsZoomed1=::IsZoomed(hWin2);
		BOOL bIsZoomed2=::IsZoomed(hWin);
		if(objSettings.bFreeMemOfHidApp){_XLOG_
			::PostMessage(hWin2,WM_SYSCOMMAND,SC_MINIMIZE,0);
			if(objSettings.bUnderWindowsNT){_XLOG_
				DWORD dwProcID=0;
				DWORD dwThreadId=GetWindowThreadProcessId(hWin2,&dwProcID);
				HANDLE hProcess=::OpenProcess(PROCESS_SET_QUOTA,FALSE,dwProcID);
				if(hProcess){_XLOG_
					::SetProcessWorkingSetSize(hProcess, -1, -1);
					CloseHandle(hProcess);
				}
			}
		}
		if(dwVisibility>0){_XLOG_
			// Смотрим, если такое уже спрятано - не делаем ничего!
			BOOL bJustHide=FALSE;
			{_XLOG_
				CSmartLock SL(&csInfoWnds,TRUE);
				for(int i=0; i<objSettings.aHidedWindowsMap.GetSize();i++){_XLOG_
					InfoWnd* pW=objSettings.aHidedWindowsMap[i];
					if(pW && pW->Sets){_XLOG_
						if(pW->Sets->hWnd==hWin || pW->Sets->hWnd==hWin2){_XLOG_
							// Уже все спрятано
							bJustHide=TRUE;
							break;
						}
					}
				}
			}
			if(!bJustHide){_XLOG_
				InfoWndStruct* info=new InfoWndStruct();
				info->bStartHided=(dwHideType==9)?2:dwHideType;
				if(dwHideType==9){_XLOG_
					info->bDetached=TRUE;
				}
				info->hWnd=hWin;
				SetProp(info->hWnd,"WAS_MAXIMIZED1",HANDLE(bIsZoomed1));
				SetProp(info->hWnd,"WAS_MAXIMIZED2",HANDLE(bIsZoomed2));
				info->bForceMinType=bForceMinType;
				// Делаем скриншот - оставляем на будущее. может в лонгхорне буду способы...
				//GetScreenShotInBuffer(info->hWnd, info->bmScreenshot, CSize(objSettings.sDetachedSize,objSettings.sDetachedSize));
				Alert(info);
			}
			// Скрываем самое топовое окно
			::SetForegroundWindow(NULL);
			::ShowWindow(hWin2,SW_HIDE);
			::ShowOwnedPopups(hWin2, FALSE);
			// Дезактивируем окно
			HWND hFore=::GetForegroundWindow();
			FLOGWND("Hide app: ",::GetForegroundWindow());
			//if(hFore==0 || hWin==hFore || hWin2==hFore)
			{_XLOG_
				CPoint pt;
				::GetCursorPos(&pt);
				HWND hNewWindow=::GetTopWindow(0);
				if(hNewWindow==0 || IsInOneHier(hNewWindow,hWin)){_XLOG_
					pt.x=theApp.rDesktopRECT.Width()/2;
					pt.y=theApp.rDesktopRECT.Height()/2;
					hNewWindow=::WindowFromPoint(pt);
				}
				if(hNewWindow==0 || IsInOneHier(hNewWindow,hWin)){_XLOG_
					hNewWindow=::WindowFromPoint(pt);
				}
				//::PostMessage(hWin,WM_ACTIVATEAPP,0,0);
				/*
				g_iATPos=0;
				g_hNextWindow=objSettings.hHotMenuWnd;
				while(g_iATPos<35){_XLOG_
				g_iATPos++;
				GetWindowTopParent(g_hNextWindow);
				g_hNextWindow=::GetNextWindow(g_hNextWindow,GW_HWNDNEXT);
				if(::IsWindowVisible(g_hNextWindow) || g_hNextWindow==0){_XLOG_
				break;
				}
				}
				HRESULT hres; 
				ITaskbarList* psl;
				hres = CoCreateInstance(CLSID_TaskbarList , NULL, CLSCTX_INPROC_SERVER, IID_TaskbarList , (LPVOID*)&psl);
				if (SUCCEEDED(hres)){_XLOG_
				psl->HrInit();
				psl->Release();
				}
				if(g_hNextWindow){_XLOG_
				SwitchToWindow(g_hNextWindow);
				}
				*/
				//::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_NEXTWINDOW, 0);
				//::SendMessage(::FindWindow("Shell_TrayWnd",NULL), WM_SYSCOMMAND, (WPARAM) SC_NEXTWINDOW, 0);
				if(hNewWindow){_XLOG_
					::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hNewWindow);
				}
			}
			// Типа заминимизировали
			AsyncPlaySoundSys("Minimize");
		}
	}
	if(dwHideType==4){_XLOG_
		if(bWndRolledUp){_XLOG_
			dwHideType=7;
		}else{_XLOG_
			DWORD dwCommand=SC_MAXIMIZE;
			if(::IsZoomed(hWin)){_XLOG_
				dwCommand=SC_RESTORE;
			}
			::PostMessage(hWin,WM_SYSCOMMAND,dwCommand,0);
		}
	}
	if(dwHideType==5){_XLOG_
		if(bWndRolledUp){_XLOG_
			dwHideType=7;
		}else{_XLOG_
			::PostMessage(hWin,WM_SYSCOMMAND,SC_MINIMIZE,0);
		}
	}
	if(dwHideType==6){_XLOG_
		::PostMessage(hWin,WM_SYSCOMMAND,SC_MAXIMIZE,0);
	}
	if(dwHideType==7 || dwHideType==8){_XLOG_
		if(bWndRolledUp){_XLOG_
			if(objSettings.bUnderWindows98){//Это было раньше - по честному:
				if(pl.showCmd==SW_SHOWMAXIMIZED){_XLOG_
					::GetWindowRect(GetDesktopWindow()->GetSafeHwnd(),&rt);
				}else{_XLOG_
					::GetWindowRect(hWin,&rt);
					rt.bottom=rt.top+(pl.rcNormalPosition.bottom-pl.rcNormalPosition.top);
				}
				::SetWindowPos(hWin,NULL,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOZORDER);
			}else{//Это теперь - регионом
				::SetWindowRgn(hWin,NULL,TRUE);
			}
			aWindowPositions.RemoveKey(hWin);
		}else{_XLOG_
			pl.length=sizeof(WINDOWPLACEMENT);
			::GetWindowPlacement(hWin,&pl);
			aWindowPositions.SetAt(hWin,pl);
			// Считаем координаты
			::GetWindowRect(hWin,&rt);
			if(dwHideType==7){_XLOG_
				CRect rtClient;
				::GetClientRect(hWin,&rtClient);
				::ClientToScreen(hWin,&rtClient.TopLeft());
				::ClientToScreen(hWin,&rtClient.BottomRight());
				rt.bottom=rt.top+(rtClient.top-rt.top)+GetSystemMetrics(SM_CYFRAME);
				//Поправка на толщину границы
				rt.bottom-=GetSystemMetrics(SM_CYFRAME)/2+1;
			}else{_XLOG_
				rt.bottom=rt.top+GetSystemMetrics(SM_CYCAPTION);
				//Поправка на толщину границы
				rt.bottom+=GetSystemMetrics(SM_CYFRAME);
			}
			if(objSettings.bUnderWindows98){//Это было раньше - по честному:
				::SetWindowPos(hWin,NULL,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOZORDER);//SWP_NOACTIVATE|
			}else{//Это теперь - регионом
				CRect rDesktopRECT;
				::GetWindowRect(::GetDesktopWindow(),&rDesktopRECT);
				rt.OffsetRect(-rt.left,-rt.top);
				rt.right=rDesktopRECT.Width()*10;// С запасом
				HRGN hRegion = CreateRectRgn(rt.left,rt.top,rt.right,rt.bottom);
				int iRes=::SetWindowRgn(hWin,hRegion,TRUE);
			}
			// Типа заминимизировали
			AsyncPlaySoundSys("Minimize");
		}
	}
	return;
}

void AppMainDlg::OnHotmenuBringinview() 
{_XLOG_
	HideApplication(objSettings.hHotMenuWnd,0);
}

void AppMainDlg::OnHotmenuSwitchontop() 
{_XLOG_
	HideApplication(objSettings.hHotMenuWnd,1);
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuUnHideFromtray)
{_XLOG_
	if(objSettings.aHidedWindowsMap.GetSize()>0){_XLOG_
		long lUnhideAction=0;
		IsThreadDoubleCalled(lUnhideAction);
		// Делаем дело и уходим
		if(pMainDialogWindow){_XLOG_
			if(lUnhideAction){_XLOG_
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_SHOWALLAPPS,0);
			}else{_XLOG_
				UnhideLastHidden();
			}
		}
	}
	return 0;
}

void AppMainDlg::OnHotmenuHidetotrayDetached()
{_XLOG_
	HideApplication(objSettings.hHotMenuWnd,9);
}

void AppMainDlg::OnHotmenuHidetotray() 
{_XLOG_
	HideApplication(objSettings.hHotMenuWnd,2);
}

void AppMainDlg::OnHotmenuHidecompletely() 
{_XLOG_
	HideApplication(objSettings.hHotMenuWnd,3);
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuCalculate)
{_XLOG_
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);

	SetCurrentDesktop();
	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	SimpleTracker trCO(g_lChainedClipboardOperations);
	CString sText=GetSelectedTextInFocusWnd();
	sText.TrimLeft();
	sText.TrimRight();
	if(sText.Right(1)!="="){_XLOG_
		sText+="=";
	}
	BOOL bRes=ParsedAsArithmeticalExpr(sText);
	if(bRes!=0){_XLOG_
		if(bRes==1){_XLOG_
			AsyncPlaySound(CALC_TEXT);
			PutSelectedTextInFocusWnd(sText);
		}
	}
	return 0;
}

HANDLEINTHREAD(AppMainDlg,OnHotmenuRunascommand)
{_XLOG_
	// Ждем второго нажатия - история
	long lHistory=0;
	IsThreadDoubleCalled(lHistory);
	//-------------------------
	SetCurrentDesktop();
	if(lHistory){_XLOG_
		pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+ID_CLIPHISTORY,1);
		return 0;
	}
	if(lConSelInProgress){
		return 0;
	}
	SimpleTracker trInP(lConSelInProgress);

	CString sText=GetSelectedTextInFocusWnd(TRUE);
	if(sText==""){_XLOG_
		// Считаем от начала строки!!!
		BlockInputMy(TRUE);
		Sleep(10);
		::keybd_event(VK_SHIFT, 1,KEYEVENTF_EXTENDEDKEY|0,0);//KEYEVENTF_EXTENDEDKEY|
		Sleep(10);
		::keybd_event(VK_HOME, ::MapVirtualKey(VK_HOME,0),KEYEVENTF_EXTENDEDKEY|0,0);
		Sleep(10);
		::keybd_event(VK_HOME, ::MapVirtualKey(VK_HOME,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
		Sleep(10);
		::keybd_event(VK_SHIFT, 1,KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);//KEYEVENTF_EXTENDEDKEY|
		BlockInputMy(FALSE);
		Sleep(10);
		sText=GetSelectedTextInFocusWnd(FALSE);
	}
	// Активное в данный момент окно
	char szDsc[256]="";
	HWND hFore=::GetForegroundWindow();
	if(hFore){_XLOG_
		::GetWindowText(hFore,szDsc,sizeof(szDsc));
	}
	ClipAddTextToPinned(sText, RUNHIST_NAME, FALSE, szDsc);
	RunAsCommand(sText);
	return 0;
}

void AppMainDlg::OnHotmenuScreensaverenabled() 
{_XLOG_
	if(TuneUpItem(SCRSAVE_TOPIC)){_XLOG_
		return;
	}
	BOOL bRes=FALSE;
	::SystemParametersInfo(16/*SPI_GETSCREENSAVEACTIVE*/,0,&bRes,0);
	bRes=!bRes;
	::SystemParametersInfo(17/*SPI_SETSCREENSAVEACTIVE*/,bRes,0,SPIF_SENDCHANGE);
}

void AppMainDlg::OnSystrayAddqrunapplication() 
{
	objSettings.OpenOptionsDialog(1,DEFQRUN_TOPIC,this);
}

void AppMainDlg::OnSystrayAddqrunappwnd() 
{
	objSettings.OpenOptionsDialog(1,DEFQRUN_TOPIC,this,1);
}

void AppMainDlg::OnSystrayReqMacro() 
{_XLOG_
	CPlugin* pRecorder=0;
	wk.isPluginLoaded("WP_MacroRec",&pRecorder,FALSE);
	if(pRecorder && pMainDialogWindow){_XLOG_
		if(!pRecorder->bIsActive){_XLOG_
			::SendMessage(pMainDialogWindow->GetSafeHwnd(),WM_USER+16/*LOADPLUGIN*/,(WPARAM)"WP_MacroRec",0);
		}
		if(pRecorder->funcs.GetSize()>0){_XLOG_
			pMainDialogWindow->PostMessage(WM_COMMAND,(pRecorder->funcs[0].dwCommandCode),0);
		}
	}
}

void AppMainDlg::OnSystrayAddqrunmacros() 
{_XLOG_
	objSettings.OpenOptionsDialog(1,DEFQMAC_TOPIC,this);
}

#define DEF_OPENBRACKET	"::{"
BOOL ExecuteMacroByContent(CString& sMacrosContent,CString& sMacrosFile, int iEntryPoint, CString sAddPAram, BOOL bSelectionSubst, CString sSelectionSubst, CQuickRunItem* pW, HANDLE hThis)
{
	CString sMacrosFileLo=sMacrosFile;
	sMacrosFileLo.MakeLower();
	sMacrosContent.TrimLeft();
	sMacrosContent.TrimRight();
	CString sMacrosContentLo=sMacrosContent;
	sMacrosContentLo.MakeLower();
	CString sHints=CDataXMLSaver::GetInstringPart("hint:","\n",sMacrosContentLo);
	if(sMacrosContentLo.Find("sendkeys")!=-1 || sHints.Find("wait_for_key_release")!=-1){
		// Ждем пока отпустят все клавиши
		// иначе взаимодействует со скриптом
		WaitWhileAllKeysAreFreeX(TRUE);
	}
	BOOL bShowIcon=objSettings.lShowMacroIcon;
	if(sHints.Find("no_icon")!=-1){
		bShowIcon=FALSE;
	}
	BOOL bSound=TRUE;
	if(sHints.Find("no_sound")!=-1){
		bSound=FALSE;
	}
	if(sHints.Find("osd")!=-1 && pW){
		CString sOSD=CDataXMLSaver::GetInstringPart(iEntryPoint>0?Format("wkMain%i description:",iEntryPoint+1):"wkMain description:","\n",sMacrosContent);
		if(sOSD!=""){
			CSplashParams par;
			FillSplashParams(pW->sItemName,par);
			par.szText=sOSD;
			static CSplashWindow* pOSD=0;
			if(pOSD==0){
				pOSD=new CSplashWindow(&par);
				pOSD->AutoClose(1000,&pOSD);
			}else{
				pOSD->AutoClose(1000,&pOSD);
				pOSD->SetTextOnly(sOSD);
			}
		}
	}
	NOTIFYICONDATA m_tnd;
	if(pMainDialogWindow && bShowIcon && pW){// Adding trai icon...
		memset(&m_tnd,0,sizeof(m_tnd));
		m_tnd.cbSize = sizeof(m_tnd);
		m_tnd.hWnd   = pMainDialogWindow->GetSafeHwnd();
		m_tnd.hIcon  = ::AfxGetApp()->LoadIcon(IDI_IC_MACRO);
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;//| NIF_STATE
		m_tnd.uID    = UINT(hThis);
		m_tnd.uCallbackMessage = STOPMACRO;
		_tcscpy(m_tnd.szTip, _l("Executing")+" "+pW->sItemName);
		Shell_NotifyIcon(NIM_ADD, (PNOTIFYICONDATA)&m_tnd);
	}
	CString sMacroLang=CDataXMLSaver::GetInstringPart("Language:","\n",sMacrosContent);
	sMacroLang.TrimLeft();
	sMacroLang.TrimRight();
	BOOL bUseJavaScript=(sMacroLang.Find("JScript")!=-1) || (sMacroLang=="");
	if(sMacrosFileLo.Find(".vbs")!=-1){
		bUseJavaScript=0;
	}else if(sMacrosFileLo.Find(".js")!=-1){
		bUseJavaScript=1;
	}else {
		CString sMacroLangLo=sMacroLang;
		sMacroLangLo.MakeLower();
		if(sMacroLangLo.Find("vbscript")!=-1 || sMacrosContentLo.Find("end function")!=-1){
			bUseJavaScript=0;
		}
	}
	if(sMacrosContent.Find(" wkMain")!=-1 || sMacrosContent.Find("\twkMain")!=-1){
		sMacrosContent+=Format("\r\nOUTPUT=wkMain%s(",(iEntryPoint>0)?Format("%i",iEntryPoint+1):"");
		CString sHotkeyTitleEx;
		CString sDeclaration;
		wk.GetEntryDesc(sMacrosContent, iEntryPoint, sHotkeyTitleEx, sDeclaration);
		if(sDeclaration!=""){
			sMacrosContent+=bUseJavaScript?"unescape('":"Unescape(\"";
			sMacrosContent+="%ESCAPED";
			sMacrosContent+=bUseJavaScript?"')":"\")";
		}
		//if(sMacrosContent.Find("macroParam")!=-1)
		if(sDeclaration.Find(",")!=-1){// Добавляем параметр
			sMacrosContent+=",";
			sMacrosContent+=bUseJavaScript?"unescape('":"Unescape(\"";
			sMacrosContent+="%PARAMETER";
			sMacrosContent+=bUseJavaScript?"')":"\")";
		}
		sMacrosContent+=")";
		if(bUseJavaScript){
			sMacrosContent+=";";
		}
	}
	BOOL bRes=0;
	if(sMacrosContent!=""){
		BOOL bSilence=0;
		if(sMacrosContent.Find("/*#SILENCE*/")!=-1){
			bSilence=1;
		}
		SubstMyVariablesForQRun(sMacrosContent, pW, TRUE, FALSE, sAddPAram, bSelectionSubst?sSelectionSubst.GetBuffer(0):0);
		bRes=(CalculateExpression_Ole(sMacrosContent,sMacroLang,bSilence,iEntryPoint,hThis)==1);
		if(bRes){
			if(sMacrosContent!=""){
				ParseLineForCookies(sMacrosContent);
				PutSelectedTextInFocusWnd(sMacrosContent);
				if(bSound){
					AsyncPlaySound(CLIP_PASTE);
				}
			}else{
				if(bSound){
					AsyncPlaySound(CALC_TEXT);
				}
			}
		}
	}else{
		Alert(_l("Macro file is empty")+"!",_l(PROGNAME": Quick-run"));
	}
	if(pMainDialogWindow && bShowIcon && pW){// Removing tray icon...
		m_tnd.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, (PNOTIFYICONDATA)&m_tnd);
	}
	return bRes;
}


DWORD WINAPI RunQRunInThreadX(LPVOID param)
{_XLOG_
	RunQRunInThreadParam* pParam=(RunQRunInThreadParam*)param;
	if(!pParam){_XLOG_
		return 0;
	}
	HANDLE hThis=0;
	while((hThis=pParam->hInThread)==0){_XLOG_
		Sleep(10);
	}
	CheckAndDisablePayFeaturesIfNeeded2();
	TRACETHREAD;
	BOOL bRes=FALSE;
	SetCurrentDesktop();
	SimpleTracker trCO(g_lChainedClipboardOperations);
	if(pParam){_XLOG_
		HWND hQRunHotMenuWnd=NULL;
		int iWndNum=pParam->iWndNum;
		BOOL bPopupItem=pParam->bPopupItem;
		BOOL bCanSwapDesktop=FALSE;//pParam->bSwapCurDesk;// Со сменой десктопа на лету одни проблемы... надо - переключаться потом руками!!!
		int iEntryPoint=pParam->iEntryPoint;
		CString sAddPAram=pParam->szAdditionalParameters;
		CString sSelectionSubst=pParam->szSelection;
		BOOL bSelectionSubst=pParam->bSubstituteSelection;
		delete pParam;
		pParam=NULL;
		//Без лока, так как внутри идет опрос окон
		//CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		if(iWndNum>=0 && iWndNum<objSettings.aQuickRunApps.GetSize()){_XLOG_
			CQuickRunItem* pW=objSettings.aQuickRunApps[iWndNum];
			if(pW){_XLOG_
				if(pW->lMacros==CATEG_MACR){_XLOG_
					if(pMainDialogWindow){_XLOG_
						// Запускаем все пункты из данной категории
						DWORD dwLastCategDelay=pW->lStartupTime;
						int iCount=0;
						for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
							CQuickRunItem* pItem=objSettings.aQuickRunApps[i];
							if(pItem){_XLOG_
								if(pItem->sCategoryID==pW->sUniqueID){_XLOG_
									if((dwLastCategDelay>0)){_XLOG_
										if(iCount>0){_XLOG_
											Sleep(1000*dwLastCategDelay);
										}
										pMainDialogWindow->SendMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+i,QRUN_NOFOREGR|QRUN_BYHOTKEY);
									}else{_XLOG_
										pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+i,QRUN_NOFOREGR|QRUN_BYHOTKEY);
									}
									iCount++;
								}
							}
						}
						dwLastCategDelay=0;
					}
				}else if(pW->lMacros==3){_XLOG_
					CString sMacrosContent=pW->sItemPath;
					if(pW->sItemParams!=""){_XLOG_
						if(pW->lExpandFolderOrCheckInstance){_XLOG_
							srand((unsigned)time(NULL)+GetCurrentThreadId());
							sMacrosContent+=objFileParser().getValueRandom(pW->sItemParams,"");
						}else{_XLOG_
							CString sContent;
							ReadFile(pW->sItemParams,sContent);
							sMacrosContent+=sContent;
						}
					}
					if(sMacrosContent!=""){_XLOG_
						SubstMyVariablesForQRun(sMacrosContent, pW);
						PutSelectedTextInFocusWnd(sMacrosContent);
						AsyncPlaySound(CLIP_PASTE);
					}else{_XLOG_
						AsyncPlaySound(CALC_TEXT);
					}
				}else if(strlen(pW->sItemPath)){_XLOG_
					CString sMacrosContent;
					CString sMacrosFile;
					int bIsMacro=(pW->lMacros==2);
					CString sInlineScript=ExtractInlineScript(pW->sItemPath);
					if(sInlineScript!=""){_XLOG_
						bIsMacro=1;
						sMacrosContent=sInlineScript;
					}
					if(bIsMacro){_XLOG_
						// Выполняем с использование JScript/VBScript
						if(sMacrosContent==""){_XLOG_
							sMacrosFile=wk.GetMacrosFileName(pW->sItemPath,FALSE);
							ReadFile(sMacrosFile,sMacrosContent);
						}
						bRes=ExecuteMacroByContent(sMacrosContent,sMacrosFile, iEntryPoint, sAddPAram, bSelectionSubst, sSelectionSubst, pW, hThis);
					}else{_XLOG_
						CSplashWindow* SplWnd=NULL;
						if(objSettings.lShowSplashForQuickRun && pW->sItemName.Find("!")!=0){
							CSplashParams par;
							FillSplashParams(pW->sItemName,par);
							SplWnd=new CSplashWindow(&par);
						}
						DWORD dwSleepTime=0;
						DWORD dwStartTime=GetTickCount();
						dwSleepTime=pW->lStartupTime*1000+100;
						BOOL bUseCreateProcess=FALSE;
						BOOL bPostProcessWindow=(pW->lFixPostion || pW->lFixSize || (pW->lOpacity<100));
						long lAfterStart=pW->lAfterStart;
						if(pMainDialogWindow && objSettings.lEnableAddDesktops && pW->lStartInDesktop>0){_XLOG_
							if((pW->lStartInDesktop-1)!=objSettings.lLastActiveDesktop){_XLOG_
								if(bCanSwapDesktop){_XLOG_
									SwitchCurrentDesktop((LPVOID)(pW->lStartInDesktop-1));
								}
								bUseCreateProcess=TRUE;
							}
						}
						// Параметры того, что нужно делать с окном
						int iShow=SW_SHOWNORMAL;
						if(lAfterStart==3){_XLOG_
							iShow=SW_SHOWMINNOACTIVE;
							//SW_SHOWMINNOACTIVE;
							//SW_FORCEMINIMIZE;
						}
						if(lAfterStart==4){_XLOG_
							iShow=SW_SHOWMAXIMIZED;
						}
						if(bPostProcessWindow){_XLOG_
							iShow=SW_SHOWNORMAL;
						}
						BOOL bPostProcessApplication=(lAfterStart>0 && lAfterStart<=2);
						// Стартовый каталог
						CString sStartDir;
						CString sPath=pW->sItemPath;
						if(sPath.Find(DEF_OPENBRACKET)==-1){_XLOG_
							if(pW->sStartupDir=="" || !isFileExist(pW->sStartupDir)){_XLOG_
								char szDisk[MAX_PATH]="C://", szPath[MAX_PATH]="";
								_splitpath(DelQuotesFromPath(sPath), szDisk, szPath, NULL, NULL);
								sStartDir=szDisk;
								sStartDir+=szPath;
							}else{_XLOG_
								AddSlashToPath(pW->sStartupDir);
								sStartDir=pW->sStartupDir;
							}
						}
						CString sParams=pW->sItemParams+sAddPAram;
						// Смотрим ole..
						CString sOleCommand=CDataXMLSaver::GetInstringPart("[ole:","]",sParams);
						CDataXMLSaver::StripInstringPart("[ole:","]",sParams);
						if(sOleCommand!=""){_XLOG_
							bUseCreateProcess=FALSE;
						}
						SubstMyVariablesForQRun(sPath, pW, 0,0, sAddPAram, bSelectionSubst?sSelectionSubst.GetBuffer(0):0);
						SubstMyVariablesForQRun(sParams, pW, 0,0, sAddPAram, bSelectionSubst?sSelectionSubst.GetBuffer(0):0);
						// Если это макрос-каталог, то...
						if(pW->lParamType==2){_XLOG_
							AddSlashToPath(sPath);
							if(pW->lAutoCreate){_XLOG_
								if(!isFileExist(sPath)){_XLOG_
									CreateDirIfNotExist(sPath);
								}
							}
						}
						// Если потом окошко нужно будет найти...
						if(pW->lParamType==2 && bUseCreateProcess && sParams==""){_XLOG_
							// Для запуска в другом десктопе - подправляем...
							ParseForShellExecute(Format("explorer.exe \"%s\"",sPath), sPath, sParams);
						}else if(pW->lParamType!=2 && pW->lExpandFolderOrCheckInstance){_XLOG_
							bUseCreateProcess=FALSE;
							bPostProcessWindow=TRUE;
						}
						BOOL bAlreadyStarted=FALSE;
						BOOL bNeedToSearchForInstance=pW->lSearchInstanceFirst;
						if(bNeedToSearchForInstance && pW->hWnd!=HWND(-2)){// Если -2, то точно не запускалось еще
							if(pW->hWnd!=NULL && pW->hWnd!=HWND(-1)){_XLOG_
								if(::IsWindow(pW->hWnd)){_XLOG_
									// Окно еще существует!!!
									LOGSTAMP;
									hQRunHotMenuWnd=pW->hWnd;
									bAlreadyStarted=TRUE;
								}else{_XLOG_
									FLOG1("Found old: %i",pW->hWnd);
									// запускаем заново - окно было да сплыло...
									pW->hWnd=HWND(-2);
								}
							}
							if(pW->hWnd!=HWND(-2) && (pW->hWnd==NULL || pW->hWnd==HWND(-1))){_XLOG_
								CString sTargetPath=sPath;
								sTargetPath.MakeLower();
								char szFile[MAX_PATH]="", szExt[MAX_PATH]="";
								_splitpath(sTargetPath, NULL, NULL, szFile, szExt);
								CString sTargetExe=szFile;
								sTargetExe+=szExt;
								// Ищем уже запущенное приложение в hQRunHotMenuWnd...
								objSettings.sTopLevelWindowsTitles.RemoveAll();
								objSettings.sTopLevelWindowsHWnds.RemoveAll();
								if(::EnumWindows(WK_EnumWindowsProc,1)){_XLOG_
									for(int i=0;i<objSettings.sTopLevelWindowsHWnds.GetSize();i++){_XLOG_
										CString sWindowExe=GetExeFromHwnd(objSettings.sTopLevelWindowsHWnds[i]);
										if(sWindowExe!=""){_XLOG_
											sWindowExe.MakeLower();
											char szFile2[MAX_PATH]="", szExt2[MAX_PATH]="";
											_splitpath(sWindowExe, NULL, NULL, szFile2, szExt2);
											CString sCheckExe=szFile2;
											sCheckExe+=szExt2;
											if(strcmp(sCheckExe,sTargetExe)==0){_XLOG_
												// Проверка по командной строке
												if(sParams!=""){_XLOG_
													DWORD dwProcID=0;
													CString sCommandLine;
													GetWindowThreadProcessId(objSettings.sTopLevelWindowsHWnds[i],&dwProcID);
													GetProcessComLine(dwProcID,sCommandLine);
													if(sCommandLine!="" && sCommandLine.Find(sParams)==-1){_XLOG_
														// Пропускаем - командная строка не подходит
														continue;
													}
												}
												pW->hWnd=objSettings.sTopLevelWindowsHWnds[i];
												hQRunHotMenuWnd=pW->hWnd;
												bAlreadyStarted=TRUE;
												LOGSTAMP;
												break;
											}
										}
									}
								}
							}
						}
						BOOL bAlreadyStartedAndWndOK=(bAlreadyStarted && bNeedToSearchForInstance && hQRunHotMenuWnd!=NULL);
						if(bAlreadyStartedAndWndOK){_XLOG_
							bAlreadyStartedAndWndOK=0;
							if(bNeedToSearchForInstance==1){_XLOG_
								FLOG("found old app");
								// Активируем окно...
								SwitchToWindow(hQRunHotMenuWnd,TRUE);
								bAlreadyStartedAndWndOK=1;
							}
							if(pMainDialogWindow){_XLOG_
								if(bNeedToSearchForInstance==2){_XLOG_
									pMainDialogWindow->HideApplication(hQRunHotMenuWnd,10);
									bAlreadyStartedAndWndOK=0;
								}
								if(bNeedToSearchForInstance==3){_XLOG_
									pMainDialogWindow->HideApplication(hQRunHotMenuWnd,12);
									bAlreadyStartedAndWndOK=0;
								}
								if(bNeedToSearchForInstance==4){_XLOG_
									pMainDialogWindow->HideApplication(hQRunHotMenuWnd,10);
									bAlreadyStartedAndWndOK=1;
								}
								if(bNeedToSearchForInstance==5){_XLOG_
									pMainDialogWindow->HideApplication(hQRunHotMenuWnd,12);
									bAlreadyStartedAndWndOK=1;
								}
							}
							bRes=TRUE;
						}
						DWORD dwErr=0;
						if(!bAlreadyStartedAndWndOK){_XLOG_
							FLOG("starting new app");
							if(bPostProcessApplication || bPostProcessWindow){_XLOG_
								if(objSettings.bUnderWindows98){_XLOG_
									//Иначе никак не получить окно для скрытия в трей :((
									bUseCreateProcess=TRUE;
								}
							}
							if(pW->lMacros==1){_XLOG_
								// Для подстановочного макроса невозможно 
								bUseCreateProcess=FALSE;
							}
							if(bUseCreateProcess){_XLOG_
								FLOG("starting new app-createproc");
								STARTUPINFO su;
								PROCESS_INFORMATION pi;
								ZeroMemory(&su, sizeof(su));
								ZeroMemory(&pi, sizeof(pi));
								GetStartupInfo(&su);
								su.cb = sizeof(su);
								su.dwFlags = STARTF_USESHOWWINDOW;
								su.wShowWindow = iShow;
								if(pW->lStartInDesktop>0){_XLOG_
									char szDeskName[DEF_DESKNAMESIZE]="Default";
									GetDesktopInternalName((pW->lStartInDesktop-1),szDeskName);
									su.lpDesktop = szDeskName;
								}else{_XLOG_
									su.lpDesktop = NULL;
								}
								char szProcessPArameters[MAX_PATH]="";
								strcpy(szProcessPArameters,sPath);
								if(sParams.GetLength()>0){
									strcat(szProcessPArameters," ");
									strcat(szProcessPArameters,sParams);
								}
								//szProcessPArameters
								bRes=::CreateProcessA(DelQuotesFromPath(sPath), szProcessPArameters, NULL,NULL, false, NULL, NULL, DelQuotesFromPath(sStartDir), &su, &pi);
								dwErr=GetLastError();
								#ifdef _DEBUG
								if(!bRes)
								{
									CString sText=sPath;
									sText+="\n(";
									sText+=Format("0x%X",dwErr);
									sText+=")";
									sText+="\n";
									sText+=sStartDir;
									AfxMessageBox(sText);
								}
								#endif
								if(dwSleepTime>0){_XLOG_
									Sleep(dwSleepTime);
								}
								hQRunHotMenuWnd=GetMainWindowFromProcessId(pi.dwProcessId);
								if(pi.hProcess){_XLOG_
									CloseHandle(pi.hProcess);
								}
							}else{_XLOG_
								sPath.TrimLeft();
								sPath.TrimRight();							
								BOOL bSkipAdvanced=0;
								FLOG("starting new app-shellex");
								SHELLEXECUTEINFO ExecInfo;
								memset(&ExecInfo,0,sizeof(ExecInfo));
								ExecInfo.cbSize=sizeof(ExecInfo);
								CString sDefaultAction="";
								if(sOleCommand!=""){_XLOG_
									sDefaultAction=sOleCommand;
								}else{_XLOG_
									sDefaultAction="open";
									CString sItemType=GetPathPart(sPath,0,0,0,1);
									sItemType.MakeLower();
									if(sItemType.Find("exe")!=-1){_XLOG_
										sDefaultAction="";
									}
									if(sItemType.Find("lnk")!=-1){_XLOG_
										if(objSettings.lUseOldStartStyle){_XLOG_
											bSkipAdvanced=1;
										}
									}
								}
								if(!bSkipAdvanced){_XLOG_
									FLOG("starting new app-shellex advanced");
									ExecInfo.lpVerb=sDefaultAction;
									ExecInfo.lpFile=sPath;
									if(sParams==""){_XLOG_
										ExecInfo.lpParameters=NULL;
									}else{_XLOG_
										ExecInfo.lpParameters=sParams;
									}
									ExecInfo.nShow=iShow;
									ExecInfo.fMask=SEE_MASK_NOCLOSEPROCESS;
									ExecInfo.lpDirectory=sStartDir;
									bRes=::ShellExecuteEx(&ExecInfo);
									dwErr=GetLastError();
								}
								DWORD dwLastErr1=GetLastError();
								DWORD dwLastErr2=0;
								DEBUG_INFO4("Executing ShellExecuteEx %s %s (%s), res=%i",sDefaultAction,sPath,ExecInfo.lpParameters,bRes);
								if(!bRes){_XLOG_
									// Пробуем по-умолчанию
									/*ExecInfo.lpVerb=NULL;
									bRes=::ShellExecuteEx(&ExecInfo);*/
									int iRes=(int)::ShellExecute(NULL,sDefaultAction,sPath,(sDefaultAction=="")?(const char*)sParams:NULL,sStartDir,iShow);
									dwLastErr2=GetLastError();
									dwErr=GetLastError();
									hQRunHotMenuWnd=HWND(-1);
									bRes=(iRes>32)?1:0;
									DEBUG_INFO4("Executing ShellExecute %s %s (%s), res=%i",sOleCommand,sPath,ExecInfo.lpParameters,iRes);
								}
								DWORD dwLastErr3=0;
								if(!bRes){_XLOG_
									int iRes=WinExec(sPath+" "+sParams,iShow);
									dwErr=GetLastError();
									dwLastErr3=GetLastError();
									hQRunHotMenuWnd=HWND(-1);
									bRes=(iRes>31)?1:0;
									FLOG1("starting new app-trying WINEXEC, res=%i",iRes);
								}
								DEBUG_INFO3("Executing ShellExecute/Ex GetLastError=%08X (%08X,%08X)",dwLastErr1,dwLastErr2,dwLastErr3);
								if(dwSleepTime>0){_XLOG_
									Sleep(dwSleepTime);
								}
								if(ExecInfo.hProcess){_XLOG_
									hQRunHotMenuWnd=GetMainWindowFromHandle(ExecInfo.hProcess);
									if(ExecInfo.hProcess){_XLOG_
										CloseHandle(ExecInfo.hProcess);
									}
								}
							}
							// Запоминаем окно
							pW->hWnd=hQRunHotMenuWnd;
						}
						if(!bRes){
							CString sText=_l("Failed to start")+" '"+pW->sItemName+"'";
							if(dwErr){
								sText+=" (";
								sText+=Format("0x%X",dwErr);
								sText+=")";
							}
							Alert(sText);
						}else{_XLOG_
							if(bPostProcessWindow && pW->hWnd && IsWindow(pW->hWnd)){_XLOG_
								CRect rt;
								BOOL bMoved=0;
								::GetWindowRect(pW->hWnd,&rt);
								if(pW->lParamType!=2 && pW->lExpandFolderOrCheckInstance){_XLOG_
									// Делаем "Поверх всех"
									::SetWindowPos(pW->hWnd,HWND_TOPMOST,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW|SWP_ASYNCWINDOWPOS);
								}
								if(pW->lFixSize){_XLOG_
									rt.right=rt.left+pW->lFixedWidth;
									rt.bottom=rt.top+pW->lFixedHeight;
									bMoved=TRUE;
								}
								if(pW->lFixPostion){_XLOG_
									rt.right=pW->lFixedXPos+rt.Width();
									rt.left=pW->lFixedXPos;
									rt.bottom=pW->lFixedYPos+rt.Height();
									rt.top=pW->lFixedYPos;
									bMoved=TRUE;
								}
								if(bMoved){_XLOG_
									BOOL bRes=::SetWindowPos(pW->hWnd,0,rt.left,rt.top,rt.Width(),rt.Height(),SWP_NOZORDER|SWP_ASYNCWINDOWPOS);
									FLOG2("error setwpos: %i, %s",bRes,GetCOMError(GetLastError()));
								}
								if(pW->lOpacity<100){_XLOG_
									SetLayeredWindowStyle(pW->hWnd,1);
									SetWndAlpha(pW->hWnd,pW->lOpacity,1);
								}
							}
							//Скрываем если надо
							if(bPostProcessApplication){_XLOG_
								if(pMainDialogWindow && pMainDialogWindow->iRunStatus==1 && hQRunHotMenuWnd != NULL && hQRunHotMenuWnd != HWND(-1)){_XLOG_
									if(lAfterStart==2){_XLOG_
										pMainDialogWindow->HideApplication(hQRunHotMenuWnd,3);
									}else{_XLOG_
										pMainDialogWindow->HideApplication(hQRunHotMenuWnd,2);
									}
								}
							}else if(IsWindow(pW->hWnd)){_XLOG_
								if(lAfterStart==0 || lAfterStart>=4){_XLOG_
									// Принудительно свитчимся на него
									PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)pW->hWnd);
								}
								if(lAfterStart==3 && !IsIconic(pW->hWnd)){_XLOG_
									PostMessage(pW->hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
								}
								if(lAfterStart==4){_XLOG_
									PostMessage(pW->hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
								}
							}
						}
						if(SplWnd){_XLOG_
							// Ждем еще время, чтобы сплэш висел нужное количество секунд
							DWORD dwCurTime=GetTickCount();
							if(dwSleepTime>0 && (dwCurTime-dwStartTime)<dwSleepTime){_XLOG_
								Sleep(dwSleepTime-(dwCurTime-dwStartTime));
							}
							delete SplWnd;
							SplWnd=NULL;
						}
					}
				}else{_XLOG_
					Alert(_l("Missed file name, please define\nfile name for this action"),_l(PROGNAME": Quick-run"));
				}
			}
			// Popup if needed
			if(bPopupItem){_XLOG_
				CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
				objSettings.aQuickRunApps.RemoveAt(iWndNum);
				objSettings.aQuickRunApps.InsertAt(0,pW);
			}
		}
	}
	if(hThis){_XLOG_
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		objSettings.aQRThreads.RemoveKey(hThis);
		CloseHandle(hThis);
	}
	return bRes;
}

void StartQRun(RunQRunInThreadParam* p)
{_XLOG_
	DWORD dwID=0;
	HANDLE h=CreateThread(0,0,RunQRunInThreadX,(LPVOID)p,0,&dwID);
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	objSettings.aQRThreads.SetAt(h,0);
	p->hInThread=h;
}

LRESULT AppMainDlg::RunQRunApp(int iWndNum, BOOL bSwitchDesk, int iEntryNum)
{_XLOG_
	RunQRunInThreadParam* pParam=new RunQRunInThreadParam;
	pParam->bPopupItem=FALSE;//objSettings.bPopupRecentlyUsed && ((lParam & QRUN_ONWKSTART)==0);
	pParam->iWndNum=iWndNum;
	pParam->bSwapCurDesk=(bSwitchDesk==0)?TRUE:FALSE;
	pParam->iEntryPoint=iEntryNum;
	StartQRun(pParam);
	return 0;
}

afx_msg LRESULT AppMainDlg::OnRunQRunApplication(WPARAM wParam, LPARAM lParam)
{_XLOG_
	if(lParam & QRUN_SHOWMENU){_XLOG_
		// Меню с выбором!!!
		if(!pMainDialogWindow){_XLOG_
			return 0;
		}
		CQuickRunItem* pQRun=objSettings.aQuickRunApps[wParam];
		CMenu menu;
		menu.CreatePopupMenu();
		CString sIcon=_IL(wk.GetIconNumByMacro(pQRun->lMacros,pQRun->lParamType));
		AddMenuString(&menu,WM_USER+MAX_USERMSG+__LINE__,TrimMessage(pQRun->sItemName,20)+getActionHotKeyDsc(pQRun->qrHotkeys[0]->oHotKey),_bmp().Get(sIcon),FALSE,NULL,FALSE,TRUE);
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		AppendQRunSingleToMenu(0, &menu, ANY_MACRO, NULL, pQRun->sUniqueID);
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&menu,WM_USER+QRUNAPPLICATION+wParam,_l("Start all items")+"\t"+MakeDoublePressKey(GetIHotkeyName(pQRun->qrHotkeys[0]->oHotKey)),_bmp().Get(sIcon));
		AddMenuString(&menu,ID_HOTMENU_EXITMENU,_l("Close menu"),_bmp().Get(_IL(40)));
		CPoint pos;
		GetCursorPos(&pos);
		if(pMainDialogWindow){_XLOG_
			SetMainWindowForeground(objSettings.hHotMenuWnd,NULL,NULL);
		}
		TrackPopupMenu(menu.GetSafeHmenu(), TPM_LEFTBUTTON, pos.x, pos.y, 0, pMainDialogWindow->GetSafeHwnd(), NULL);
	}else{_XLOG_
		RunQRunApp(wParam,lParam,0);
	}
	return 0;
}

long lStopOnBeforeShutdown=0;
void OnBeforeShutdown()
{_XLOG_
	if(lStopOnBeforeShutdown){_XLOG_
		return;
	}
	lStopOnBeforeShutdown=1;
	KillProcessesFromList(objSettings.sKillBeforeShutdown);
	objSettings.SaveAll();
	objSettings.bSaveAllowed=0;
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->PerformOnShutdown();
	}
	// Посылаем наше сообщение о выключении
	//DWORD dwTarget=BSM_APPLICATIONS;
	//BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(0), LPARAM(0));
}

void AppMainDlg::OnHotmenuShutdown()
{_XLOG_
	if(TuneUpItem(SHUTDOWN_TOPIC)){_XLOG_
		return;
	}
	OnBeforeShutdown();
	MyShutDownComputer(FALSE,objSettings.lShutdownStrength);
}

void AppMainDlg::OnHotmenuSuspend()
{_XLOG_
	if(TuneUpItem(SHUTDOWN_TOPIC)){_XLOG_
		return;
	}
	MyShutDownComputer(FALSE,3);
}

void AppMainDlg::OnHotmenuRestart()
{_XLOG_
	if(TuneUpItem(SHUTDOWN_TOPIC)){_XLOG_
		return;
	}
	OnBeforeShutdown();
	MyShutDownComputer(TRUE,objSettings.lShutdownStrength);
}

#define SHUT_WAIT_TIME	2
long lCancelShutdown=0;
BOOL CALLBACK CancelShutdown(void* Param, int iButtonNum)
{_XLOG_
	lCancelShutdown=1;
	return FALSE;
}

CString GetShutdownmsg(long lType,CString sStep="Initialization", BOOL bFull=1)
{_XLOG_
	if(objSettings.lExitWndOnDbl){_XLOG_
		lType=!lType;
	}
	CString sShutMsg=(lType?_l("Restart"):_l("Shutdown"))+": "+_l(sStep)+"\n";
	if(bFull){_XLOG_
		sShutMsg+=_l("Press")+getActionHotKeyDsc(FAST_SHUTDOWN," '","' ")+(lType?_l("to shutdown"):_l("to restart"))+"\n"+_l("Press 'ESC' to cancel");
		sShutMsg+="\n";
	}
	sShutMsg+=_l("Shutdown type")+": "+_l(g_sShutType[g_ShutType]);
	return sShutMsg;
}

CCriticalSection csShutdown;
HANDLEINTHREAD(AppMainDlg,OnShutDownComputer)
{_XLOG_
	SetCurrentDesktop();
	static long lShutdownType=-1;
	if(lShutdownType>1){_XLOG_
		// тут больше нечего делать
		return 0;
	}
	lShutdownType++;
	if(lShutdownType>0){_XLOG_
		// тут больше нечего делать
		return 0;
	}
	CSmartLock cs(csShutdown);
	InfoWnd* pWnd=NULL;
	if(pWnd!=0){_XLOG_
		delete pWnd;
		pWnd=0;
	}
	if(pWnd==0){_XLOG_
		pWnd=Alert(GetShutdownmsg(lShutdownType),0,FALSE,DT_CENTER|DT_VCENTER);
	}
	pWnd->SetCallback(CancelShutdown,0);
	pWnd->SetOkText(_l("Cancel shutdown"));
	DWORD dwStartTime=GetTickCount();
	DWORD dwCurTime=dwStartTime;
	long dwLastShutdownType=lShutdownType;
	while(dwCurTime<(dwStartTime+SHUT_WAIT_TIME*1000)){_XLOG_
		if((GetAsyncKeyState(VK_ESCAPE)<0 || lCancelShutdown==1)){_XLOG_
			pWnd->SetText(GetShutdownmsg(lShutdownType,"Cancelled",0));
			pWnd->SetOkText(_l("Ok"));
			pWnd->SetCallback(0,0);
			pWnd=NULL;
			lShutdownType=-1;
			lCancelShutdown=0;
			return 0;
		}
		if(dwLastShutdownType!=lShutdownType){_XLOG_
			dwLastShutdownType=lShutdownType;
			pWnd->SetText(GetShutdownmsg(lShutdownType));
			break;
		}
		dwCurTime=GetTickCount();
		Sleep(100);
	}
	pWnd->SetText(GetShutdownmsg(lShutdownType,"In progress",0));
	pWnd->SetOkText(_l("Ok"));
	pWnd->SetCallback(0,0);
	pWnd=NULL;
	long lType=lShutdownType?1:0;
	if(objSettings.lExitWndOnDbl){_XLOG_
		lType=lType?0:1;
	}
	lShutdownType=-1;
	lCancelShutdown=0;
	OnBeforeShutdown();
	MyShutDownComputer(lType?TRUE:FALSE,g_ShutType);
	return 0;
}

/*
HANDLEINTHREAD(AppMainDlg,OnShutDownComputer)
{_XLOG_
	static DWORD dwShutDownStart=0;
	SetCurrentDesktop();
	static long lLock=0;
	static InfoWnd* pWnd=NULL;
	lLock++;
	if(lLock>1){_XLOG_
		if(lLock>2){_XLOG_
			if(GetTickCount()-dwShutDownStart<10000){_XLOG_
				Alert(_l("Shutdown sequence can`t be cancelled")+"!",5);
				return 1;
			}
			// Слишком долго ничего не происходит...
			lLock=1;
		}
		if(lLock!=1){_XLOG_
			dwShutDownStart=GetTickCount();
			if(pWnd && ::IsWindow(pWnd->GetSafeHwnd())){_XLOG_
				pWnd->SetText((objSettings.lExitWndOnDbl?_l("Shutdown"):_l("Restart"))+": "+_l("Initialization"));
			}
			OnBeforeShutdown();
			MyShutDownComputer((objSettings.lExitWndOnDbl?FALSE:TRUE),g_ShutType);
			pWnd->SetOkText("");
			pWnd->SetText((objSettings.lExitWndOnDbl?_l("Shutdown"):_l("Restart"))+": "+_l("In progress"));
			return 1;
		}
	}
	CString sShutMsg=(objSettings.lExitWndOnDbl?_l("Restart"):_l("Shutdown"))+": "+_l("initialization")+"\n"+_l("Press")+getActionHotKeyDsc(FAST_SHUTDOWN," '","' ")+(objSettings.lExitWndOnDbl?_l("to shutdown"):_l("to restart"))+"\n"+_l("Press 'ESC' to cancel");
	sShutMsg+="\n";
	sShutMsg+=_l("Shutdown type")+": "+_l(g_sShutType[g_ShutType]);
	if(!pWnd){_XLOG_
		pWnd=Alert(sShutMsg,0,FALSE,DT_CENTER|DT_VCENTER);
	}else{_XLOG_
		pWnd->SetText(sShutMsg);
	}
	lCancelShutdown=0;
	pWnd->SetCallback(CancelShutdown,0);
	pWnd->SetOkText(_l("Cancel shutdown"));
	DWORD dwStartTime=GetTickCount();
	DWORD dwCurTime=dwStartTime;
	while(dwCurTime<(dwStartTime+SHUT_WAIT_TIME*1000)){_XLOG_
		if(lLock==1 && (GetAsyncKeyState(VK_ESCAPE)<0 || lCancelShutdown==1)){_XLOG_
			pWnd->SetText((objSettings.lExitWndOnDbl?_l("Restart"):_l("Shutdown"))+": "+_l("Cancelled")+"!");
			pWnd->SetOkText(_l("Ok"));//pWnd->SetButtonsVisibility(TRUE);
			pWnd=NULL;
			lLock=0;
			return 0;
		}
		dwCurTime=GetTickCount();
		Sleep(100);
	}
	if(lLock==1){// Если еще не начался restart...
		OnBeforeShutdown();
		dwShutDownStart=GetTickCount();
		MyShutDownComputer(objSettings.lExitWndOnDbl?TRUE:FALSE,g_ShutType);
		pWnd->SetOkText("");
		CString sText=(objSettings.lExitWndOnDbl?_l("Restart"):_l("Shutdown"))+": "+_l("In progress");
		sText+="\n";
		sText+=_l("Shutdown type")+": "+_l(g_sShutType[g_ShutType]);
		pWnd->SetText(sText);
	}
	return 1;
}*/

long lVolSplashCounter=0;
DWORD WINAPI AutoCloseVolumeFlash(LPVOID pData)
{_XLOG_
	InterlockedIncrement(&lVolSplashCounter);
	long lCurCounter=lVolSplashCounter;
	////////////////////////////////////////////////
	// Проверяем регулятор громкости
	Sleep(objSettings.lOSDSecs*1000);
	if(lCurCounter==lVolSplashCounter && pMainDialogWindow && pMainDialogWindow->VolumeSplWnd){//(GetTickCount()-pMainDialogWindow->dwVolSplashTimer)>VOLSPLASH_SLEEPTIME
		CSplashWindow* pTmp=pMainDialogWindow->VolumeSplWnd;
		pMainDialogWindow->VolumeSplWnd=NULL;
		delete pTmp;
	}
	return 0;
}

long AppMainDlg::ChangeVolumeControl(long lDx, int iMuter)
{_XLOG_
	static long lLock=0;
	if(lLock>0){_XLOG_
		return -1;
	}
	CString sSplashText="...";
	SimpleTracker tr(lLock);
	if(iMuter==-1){_XLOG_
		/*const double d10=double(MAX_MIXER_VOL)/double(objSettings.lVolumeCStep);
		double dRounded=double(mixer.GetVolumeCtrlValue())/d10;
		double iCurPos=ceil(d10*dRounded);
		long lRawDx=long(double(MAX_MIXER_VOL)*(double(lDx)/double(100)));
		if(long(iCurPos)+lRawDx>0 && long(iCurPos)+lRawDx<MAX_MIXER_VOL){_XLOG_
			iCurPos+=lRawDx;
		}else{_XLOG_
			if(long(iCurPos)+lRawDx<=0){_XLOG_
				iCurPos=0;
			}
			if(long(iCurPos)+lRawDx>MAX_MIXER_VOL){_XLOG_
				iCurPos=MAX_MIXER_VOL;
			}
		}
		mixer.SetVolumeCtrlValue(iCurPos);
		int iPerc=int(double(iCurPos+1)/double(MAX_MIXER_VOL)*100);
		*/
		#define INT_MUL_VOL	10000
		if(lDx==0){_XLOG_
			lDx=1;
		}
		lDx*=INT_MUL_VOL;
		__int64 lDxS=lDx;
		if(lDxS<0){_XLOG_
			lDxS=-lDxS;
		}
		__int64 iMixerValue=mixer.GetVolumeCtrlValue()*100*INT_MUL_VOL/MAX_MIXER_VOL;
		// Округляем до ближайшего порога
		__int64 iMixerValueTmp=lDxS*(iMixerValue/lDxS);
		if(iMixerValueTmp!=iMixerValue){_XLOG_
			// Ищем минимальный...
			__int64 iMixerValueNew=0;
			while(iMixerValueNew<iMixerValue && (iMixerValue-iMixerValueNew>lDxS/2)){_XLOG_
				iMixerValueNew+=lDxS;
			}
			iMixerValue=iMixerValueNew;
		}
		if(iMixerValue+lDx<=0){_XLOG_
			iMixerValue=0;
		}else if(iMixerValue+lDx>100*INT_MUL_VOL){_XLOG_
			iMixerValue=100*INT_MUL_VOL;
		}else{_XLOG_
			iMixerValue+=lDx;
		}
		mixer.SetVolumeCtrlValue(iMixerValue*MAX_MIXER_VOL/(100*INT_MUL_VOL));
		__int64 iPerc=iMixerValue/INT_MUL_VOL;
		sSplashText=Format("%s: %i%%",_l("Volume"),iPerc);
	}else{_XLOG_
		BOOL bMuted=mixer.GetMuteCtrlValue();
		mixer.SetMuteCtrlValue(!bMuted);
		if(bMuted){_XLOG_
			sSplashText=_l("Sound ON");
		}else{_XLOG_
			sSplashText=_l("Sound OFF");
		}
	}
	if(objSettings.lVolumeNeedOSD){_XLOG_
		static CSplashParams par;
		if(!VolumeSplWnd){_XLOG_
			FillSplashParams(_l("Volume control"),par);
			par.szText=sSplashText;
			VolumeSplWnd=new CSplashWindow(&par);
		}else{_XLOG_
			VolumeSplWnd->SetText(sSplashText,par.txColor,par.bgColor,par.dwTextParams);
			VolumeSplWnd->wnd->Invalidate();
		}
		dwVolSplashTimer=GetTickCount();
		FORK(AutoCloseVolumeFlash,0);
	}
	return TRUE;
}

HANDLEINTHREAD(AppMainDlg,StartDefBrowser)
{_XLOG_
	BOOL bRes=0;
	CSplashParams par;
	FillSplashParams(_l("Browser"),par);
	CSplashWindow SplWnd(&par);
	CString sDefaultBrowser=getBrowserExePath();
	bRes=(sDefaultBrowser!="") && (32<(int)ShellExecute(NULL,"open",sDefaultBrowser,"",NULL,SW_SHOWNORMAL));
	if(bRes==0){_XLOG_
		Alert(_l("Can`t start default web-browser")+"!");
	}
	return 0;
}

HANDLEINTHREAD(AppMainDlg,StartDefEmail)
{_XLOG_
	CSplashParams par;
	FillSplashParams(_l("Email client"),par);
	CSplashWindow SplWnd(&par);
	//
	long bRes=RunDefaultEmailClient_InThread(0);
	if(bRes==0){_XLOG_
		Alert(_l("Can`t start default e-mail client")+"!");
	}
	return 0;
}

void AppMainDlg::OnHotmenuClearclips() 
{_XLOG_
	wk.ClearAllClips(NULL,NULL);
}

BOOL CALLBACK WK_EnumWindowsProc(HWND hwnd, LPARAM lParam)
{_XLOG_
	BOOL bNeedToAdd=FALSE;
	//--------------------
	HWND hPar=hwnd;
	if(!IsWindowsSystemWnd(hPar)){_XLOG_
		GetWindowTopParent(hPar);
		char szBuffer[1024]="";
		if(hPar==NULL || hPar==hwnd){_XLOG_
			DWORD dwStyleEx=GetWindowLong(hwnd,GWL_EXSTYLE);
			DWORD dwStyle=GetWindowLong(hwnd,GWL_STYLE);
			DWORD dwResult=0;
			if(SendMessageTimeout(hwnd,WM_GETTEXT,(WPARAM)sizeof(szBuffer),(LPARAM)szBuffer,SMTO_ABORTIFHUNG,500,&dwResult)){_XLOG_
				if (lParam==1){_XLOG_
					if((dwStyleEx & WS_EX_TOOLWINDOW)==0){//&& GetWindow(hwnd,GW_CHILD)!=NULL
						// Это должно быть не временным окошком и должны быть подчиненные окна
						bNeedToAdd=TRUE;
					}
				}else if (::IsWindowEnabled(hwnd) && ::IsWindowVisible(hwnd) && (dwStyleEx & WS_EX_TOOLWINDOW)==0){//&& (dwStyle & WS_POPUP)==0
					bNeedToAdd=TRUE;
				}
			}
		}
		if(bNeedToAdd){_XLOG_
			if(strlen(szBuffer)>0){_XLOG_
				// Убираем лишние окна
				objSettings.sTopLevelWindowsHWnds.Add(hwnd);
				objSettings.sTopLevelWindowsTitles.Add(szBuffer);
			}
		}
	}
	return TRUE;
}

#include <winioctl.h>
void CD_OpenCloseDrive98(BOOL bOpenDrive, TCHAR cDrive)
{_XLOG_
	MCI_OPEN_PARMS op;
	MCI_STATUS_PARMS st;
	DWORD flags;
	
	TCHAR szDriveName[4];
	strcpy(szDriveName, "X:");
	
	::ZeroMemory(&op, sizeof(MCI_OPEN_PARMS));
	op.lpstrDeviceType = (LPCSTR) MCI_DEVTYPE_CD_AUDIO;
	
	if(cDrive > 1)
	{_XLOG_
		szDriveName[0] = cDrive;
		op.lpstrElementName = szDriveName;
		flags = MCI_OPEN_TYPE 
			| MCI_OPEN_TYPE_ID 
			| MCI_OPEN_ELEMENT 
			| MCI_OPEN_SHAREABLE;
	}
	else flags = MCI_OPEN_TYPE 
		| MCI_OPEN_TYPE_ID 
		| MCI_OPEN_SHAREABLE;
	
	if (!getWinmm().myMciSendCommand(0,MCI_OPEN,flags,(unsigned long)&op)) 
	{_XLOG_
		st.dwItem = MCI_STATUS_READY;
		
		if(bOpenDrive)
			getWinmm().myMciSendCommand(op.wDeviceID,MCI_SET,MCI_SET_DOOR_OPEN,0);
		else
			getWinmm().myMciSendCommand(op.wDeviceID,MCI_SET,MCI_SET_DOOR_CLOSED,0);
		
		getWinmm().myMciSendCommand(op.wDeviceID,MCI_CLOSE,MCI_WAIT,0);
	}
}

void OpenCD(TCHAR cCDDiskLetter, BOOL bWithOSD)
{_XLOG_
	CSplashWindow* pOSD=0;
	if(bWithOSD){_XLOG_
		CSplashParams par;
		FillSplashParams("",par);
		par.szText=Format("%s '%c:'...",_l("Ejecting disk"),cCDDiskLetter);
		pOSD=new CSplashWindow(&par);
	}
	if(objSettings.bUnderWindows98){_XLOG_
		CD_OpenCloseDrive98(TRUE,cCDDiskLetter);
	}else{_XLOG_
		HANDLE H=NULL;
		CString sCDHandlePath=Format("\\\\.\\%c:",cCDDiskLetter);
		H=::CreateFile(sCDHandlePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
		if(H!=INVALID_HANDLE_VALUE && H!=NULL){_XLOG_
			DWORD BytesReturned=0;
			BOOL bSuccess=::DeviceIoControl(H, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &BytesReturned, NULL);
			CloseHandle(H);
		}
	}
	if(pOSD){_XLOG_
		delete pOSD;
	}
	DWORD dwState=1;
	sCDMap.SetAt(cCDDiskLetter,dwState);
}

void CloseCD(TCHAR cCDDiskLetter, BOOL bWithOSD)
{_XLOG_
	CSplashWindow* pOSD=0;
	if(bWithOSD){_XLOG_
		CSplashParams par;
		FillSplashParams("",par);
		par.szText=Format("%s '%c:'...",_l("Loading disk"),cCDDiskLetter);
		pOSD=new CSplashWindow(&par);
	}
	if(objSettings.bUnderWindows98){_XLOG_
		CD_OpenCloseDrive98(FALSE,cCDDiskLetter);
	}else{_XLOG_
		HANDLE H=NULL;
		CString sCDHandlePath=Format("\\\\.\\%c:",cCDDiskLetter);
		H=::CreateFile(sCDHandlePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
		if(H!=INVALID_HANDLE_VALUE && H!=0){_XLOG_
			
			DWORD BytesReturned=0;
			BOOL bSuccess=::DeviceIoControl(H, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0, &BytesReturned, NULL);
			CloseHandle(H);
		}
	}
	if(pOSD){_XLOG_
		delete pOSD;
	}
	DWORD dwState=0;
	sCDMap.SetAt(cCDDiskLetter,dwState);
}

long lCDOperationInAction=0;
DWORD WINAPI CDCloseThread(LPVOID param)
{_XLOG_
	char cCD=(char)param;
	if(objSettings.lAutoCloseCDTime==0){_XLOG_
		return 2;
	}
	DWORD dwState=0;
	sCDMap.Lookup(DWORD(cCD),dwState);
	// Итак уже закрыт
	if(dwState==0){_XLOG_
		return 1;
	}
	Sleep(objSettings.lAutoCloseCDTime*1000);
	{// в отдельной секции...
		SimpleTracker tr(lCDOperationInAction);
		sCDMap.Lookup(DWORD(cCD),dwState);
		// Уже закрыт
		if(dwState==0){_XLOG_
			return 1;
		}
		CloseCD(cCD);
	}
	return 0;
}

HANDLEINTHREAD3(AppMainDlg,OnSwapCD,if(TuneUpItem(CDEJECT_TOPIC)){return;})
{_XLOG_
	static long lLock=0;
	static TCHAR cCDDiskLetter='-';
	if(lLock==0){_XLOG_
		if(!objSettings.sCDDisk.IsEmpty()){_XLOG_
			objSettings.sCDDisk.MakeUpper();
			cCDDiskLetter=objSettings.sCDDisk[0];
		}
	}
	if(lLock==1){_XLOG_
		if(!objSettings.sCDDisk2.IsEmpty()){_XLOG_
			objSettings.sCDDisk2.MakeUpper();
			cCDDiskLetter=objSettings.sCDDisk2[0];
		}
	}
	if(lLock>0 || cCDDiskLetter=='-'){_XLOG_
		return 0;
	}
	SimpleTracker tr(lLock);
	WaitWhileAllKeysAreFreeX();
	{_XLOG_
		if(lCDOperationInAction>0){_XLOG_
			Alert(_l("CD Drive is busy now")+" ("+_l("Disk")+": "+CString(cCDDiskLetter)+")...\n"+_l("Try later")+"!",_l("CD Drive"),3000);
			return 0;
		}
		SimpleTracker tr(lCDOperationInAction);
		DWORD bOpen=FALSE;
		// Думаем что же делать теперь...
		DWORD bDiskNotOpened=0;
		sCDMap.Lookup(cCDDiskLetter,bDiskNotOpened);
		// Проводим реальное действие
		if(!bDiskNotOpened){_XLOG_
			OpenCD(cCDDiskLetter);
			if(objSettings.bAutoCloseCD){_XLOG_
				// Команда на закрытие...
				FORK(CDCloseThread,cCDDiskLetter);
			}
		}else{_XLOG_
			CloseCD(cCDDiskLetter);
		}
	}
	return 0;
}

HANDLEINTHREAD(AppMainDlg,OnBossKey)
{_XLOG_
	static CString sMask=objSettings.sBossAppMask;
	static long lBossAction=objSettings.lBosskeyAction;
	long lAction=0;
	IsThreadDoubleCalled(lAction);
	if(lAction==1){_XLOG_
		// Один раз уже было нажато
		sMask=objSettings.sBossAppMask2;
		lBossAction=objSettings.lBosskeyAction2;
	}else{_XLOG_
		sMask=objSettings.sBossAppMask;
		lBossAction=objSettings.lBosskeyAction;
	}
	// Двигаемся по активным приложениям
	objSettings.sTopLevelWindowsTitles.RemoveAll();
	objSettings.sTopLevelWindowsHWnds.RemoveAll();
	if(sMask!=""){_XLOG_
		if(sMask.Find("*")==-1 && sMask.Find("?")==-1){_XLOG_
			sMask=CString("*")+sMask+"*";
		}
		// Убиваем случайные пробелы
		while(sMask.Replace("* ","*")>0){};
		while(sMask.Replace(" *","*")>0){};
		sMask.Replace("*,","*;");
		sMask.Replace(",*",";*");
	}
	if(::EnumWindows(WK_EnumWindowsProc,0)){_XLOG_
		for(int i=0;i<objSettings.sTopLevelWindowsTitles.GetSize();i++){_XLOG_
			if(sMask!=""){_XLOG_
				CString sWndTitle=objSettings.sTopLevelWindowsTitles[i];
				if(PatternMatchList(sWndTitle,sMask)!=TRUE){_XLOG_
					continue;
				}
			}
			HWND hWind=objSettings.sTopLevelWindowsHWnds[i];
			if(hWind!=NULL && IsWindow(hWind) && pMainDialogWindow){_XLOG_
				switch(lBossAction){_XLOG_
				case 1:
					pMainDialogWindow->HideApplication(hWind,2);
					break;
				case 2:
					{_XLOG_
						DWORD dwProcId=0;
						DWORD dwThread=GetWindowThreadProcessId(hWind,&dwProcId);
						KillProcess(dwProcId,NULL);
						break;
					}
				default:
					pMainDialogWindow->HideApplication(hWind,3);
					break;
				}
			}
		}
	}
	if(objSettings.lBossShotSwap && objSettings.sLastScreenshot!=""){_XLOG_
		CBitmap* bmpLoaded=_bmp().LoadBmpFromPath(objSettings.sLastScreenshot);
		if(bmpLoaded){_XLOG_
			HWND wDesc=GetDesktopWindow();
			if(wDesc){_XLOG_
				CRect rt;
				::GetWindowRect(wDesc,&rt);
				HDC pDC=::GetWindowDC(wDesc);
				if(pDC){_XLOG_
					CDC pSHDC;
					pSHDC.CreateCompatibleDC(CDC::FromHandle(pDC));
					CBitmap* pOld=pSHDC.SelectObject(bmpLoaded);
					StretchBlt(pDC,0,0,rt.Width(),rt.Height(),pSHDC,0,0,rt.Width(),rt.Height(),SRCCOPY);
					pSHDC.SelectObject(pOld);
					::ReleaseDC(wDesc,pDC);
				}
			}
			delete bmpLoaded;
		}
	}
	return 0;
}

void AppMainDlg::OnSystrayShowAllApps()
{_XLOG_
	CSmartLock SL(&csInfoWnds,TRUE);
	for(int i=0; i<objSettings.aHidedWindowsMap.GetSize();i++){_XLOG_
		InfoWnd* pW=objSettings.aHidedWindowsMap[i];
		if(pW){_XLOG_
			PostMessage(WM_COMMAND,WM_USER+HIDEDWINDOWS+i);
		}
	}
}

void AppMainDlg::OnSystrayBossPanic()
{_XLOG_
	if(TuneUpItem(BOSS_TOPIC)){_XLOG_
		return;
	}
	OnBossKey();
}

long lWinAmpSplashCounter=0;
CCriticalSection csWinAmpAction;
DWORD WINAPI AutoCloseWinAmpFlash(LPVOID pData)
{_XLOG_
	InterlockedIncrement(&lWinAmpSplashCounter);
	long lCurCounter=lWinAmpSplashCounter;
	////////////////////////////////////////////////
	Sleep(objSettings.lWinampOSDTime*1000);
	if(lCurCounter==lWinAmpSplashCounter && pMainDialogWindow && pMainDialogWindow->WinAmpSplWnd){_XLOG_
		CSplashWindow* pTmp=pMainDialogWindow->WinAmpSplWnd;
		csWinAmpAction.Lock();
		pMainDialogWindow->WinAmpSplWnd=NULL;
		csWinAmpAction.Unlock();
		delete pTmp;
	}
	return 0;
}
/*
Raise volume by 1% 40058 
Lower volume by 1% 40059 
*/

int GetWinAmpPlStatus(HWND hWin=0)
{// 0 - playing, -1 - failed to get result, 1 - paused, 2 - stopped
	if(hWin==0){_XLOG_
		static HWND hwndWinamp=NULL;
		if(hwndWinamp==NULL || !IsWindow(hwndWinamp)){_XLOG_
			hwndWinamp=::FindWindow("Winamp v1.x",NULL);
		}
		hWin=hwndWinamp;
	}
	if(hWin && ::IsWindow(hWin)){_XLOG_
		int iRes=::SendMessage(hWin,WM_USER,104,104);
		if(iRes==1){_XLOG_
			return 0;
		}
		if(iRes==3){_XLOG_
			return 1;
		}
		return 2;
	}
	return -1;
}

BOOL AppMainDlg::WinAmpControl(int iAction, BOOL bTryStart, BOOL bSplashEnabled, int iAdditionalPar)
{_XLOG_
	static long lLock=0;
	if(lLock>0 && !(iAction==0 && lLock==1)){_XLOG_
		return 0;
	}
	SimpleTracker tr(lLock);
	//
	CString sSplashText=_l("No WinAmp found");
	CString sWinAmpTitle="";
	BOOL bWinAmp3=FALSE;
	HWND hwndWinamp=::FindWindow("Winamp v1.x",NULL);
	if(hwndWinamp==NULL){_XLOG_
		if(bTryStart){_XLOG_
			int iCount=0;
			BOOL bRes=WinAmpControl(0,FALSE,bSplashEnabled,iAdditionalPar);
			if(!bRes){_XLOG_
				return 0;
			}
			while((hwndWinamp=::FindWindow("Winamp v1.x",NULL))==0 && iCount<30){_XLOG_
				Sleep(1000);
				iCount++;
			}
			if(iAction==WINAMP_STOP){_XLOG_
				return 1;
			}
		}
	}
	//
	int iWAStatus=GetWinAmpPlStatus(hwndWinamp);
	if(iAdditionalPar==0 && iAction==WINAMP_PLAY && iWAStatus==2){_XLOG_
		// Если винамп остановлен то распаузивание превращается в старт
		iAction=WINAMP_STOP;
	}
	switch(iAction){_XLOG_
	case 0:
		{_XLOG_
			// Пробуем запустить
			BOOL bStarted=0;
			CString sWinAmpExe=objSettings.sWinAmpStartPas;
			if(sWinAmpExe==""){_XLOG_
				// Сначала 3ий
				CRegKey key;
				key.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Winamp3");
				char szTemp[MAX_PATH]="";
				DWORD lSize = MAX_PATH,dwType=0;
				if(RegQueryValueEx(key.m_hKey,"UninstallString",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){_XLOG_
					CString sUninstPath=szTemp;
					sUninstPath.TrimLeft("\" ");
					sUninstPath.TrimRight("\" ");
					char szDisk[MAX_PATH]="C://", szPath[MAX_PATH]="";
					_splitpath(sUninstPath, szDisk, szPath, NULL, NULL);
					sWinAmpExe=szDisk;
					sWinAmpExe+=szPath;
					sWinAmpExe+="studio.exe";
					sSplashText=_l("Starting WinAmp3");
				}
				if(!bStarted){_XLOG_
					// Если нет - второй
					CRegKey key;
					key.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Winamp");
					char szTemp[MAX_PATH]="";
					DWORD lSize = MAX_PATH,dwType=0;
					if(RegQueryValueEx(key.m_hKey,"UninstallString",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){_XLOG_
						CString sUninstPath=szTemp;
						sUninstPath.TrimLeft("\" ");
						sUninstPath.TrimRight("\" ");
						char szDisk[MAX_PATH]="C://", szPath[MAX_PATH]="";
						_splitpath(sUninstPath, szDisk, szPath, NULL, NULL);
						sWinAmpExe=szDisk;
						sWinAmpExe+=szPath;
						sWinAmpExe+="winamp.exe";
						sSplashText=_l("Starting WinAmp");
					}
				}
			}
			if(sWinAmpExe!=""){_XLOG_
				int iRes=(int)::ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open",sWinAmpExe,NULL,NULL,SW_SHOWNORMAL);
				if(iRes>32){_XLOG_
					bStarted=TRUE;
					sSplashText=_l("Starting WinAmp");
				}
			}
			if(!bStarted){_XLOG_
				return 0;
			}
			break;
		}
	case WINAMP_SHOWHIDE:
		{_XLOG_
			sSplashText=_l("Show/Hide WinAmp");
			if(::IsWindowVisible(hwndWinamp)){_XLOG_
				/*
				DWORD bOnTop=(GetWindowLong(hwndWinamp, GWL_EXSTYLE)&WS_EX_TOPMOST)!=0;
				::SetWindowPos(hwndWinamp,HWND_TOPMOST,0,0,0,0,SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);//|SWP_NOACTIVATE::GetForegroundWindow()
				if(!bOnTop){_XLOG_
					::SetWindowPos(hwndWinamp,HWND_NOTOPMOST,0,0,0,0,SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);//|SWP_NOACTIVATE
				}*/
				//::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndWinamp);
				::SendMessage(hwndWinamp, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			}else{_XLOG_
				//::SendMessage(hwndWinamp, WM_COMMAND, 40258, 0);
				::SendMessage(hwndWinamp, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				::SwitchToWindow(hwndWinamp,0);
			}
			break;
		}
	case WINAMP_VISPLUGIN:
		{_XLOG_
			sSplashText=_l("Start/Stop visualization plugin");
			::PostMessage(hwndWinamp, WM_COMMAND, 40192, 0);
			break;
		}
	case WINAMP_SHUFFLING:
		{_XLOG_
			sSplashText=_l("Shuffling changed");
			::PostMessage(hwndWinamp, WM_COMMAND, 40023, 0);
			break;
		}
	case WINAMP_NEXT5S:
		{_XLOG_
			sSplashText=_l("Forward 5 secs");
			if(bSplashEnabled && objSettings.lShowWinampTitle){_XLOG_
				::SendMessage(hwndWinamp, WM_COMMAND, 40148, 0);
			}else{_XLOG_
				::PostMessage(hwndWinamp, WM_COMMAND, 40148, 0);
			}
			break;
		}
	case WINAMP_PREV5S:
		{_XLOG_
			sSplashText=_l("Back 5 secs");
			if(bSplashEnabled && objSettings.lShowWinampTitle){_XLOG_
				::SendMessage(hwndWinamp, WM_COMMAND, 40144, 0);
			}else{_XLOG_
				::PostMessage(hwndWinamp, WM_COMMAND, 40144, 0);
			}
			break;
		}
	case WINAMP_NEXTS:
		{_XLOG_
			sSplashText=_l("Next song");
			if(bSplashEnabled && objSettings.lShowWinampTitle){_XLOG_
				::SendMessage(hwndWinamp, WM_COMMAND, 40048, 0);
			}else{_XLOG_
				::PostMessage(hwndWinamp, WM_COMMAND, 40048, 0);
			}
			break;
		}
	case WINAMP_PREVS:
		{_XLOG_
			sSplashText=_l("Prev song");
			if(bSplashEnabled && objSettings.lShowWinampTitle){_XLOG_
				::SendMessage(hwndWinamp, WM_COMMAND, 40044, 0);
			}else{_XLOG_
				::PostMessage(hwndWinamp, WM_COMMAND, 40044, 0);
			}
			break;
		}
	case WINAMP_STOP:
		{_XLOG_
			if(iWAStatus==0){_XLOG_
				sSplashText=_l("Stop");
				if(bSplashEnabled && objSettings.lShowWinampTitle){_XLOG_
					::SendMessage(hwndWinamp, WM_COMMAND, 40047, 0);
				}else{_XLOG_
					::PostMessage(hwndWinamp, WM_COMMAND, 40047, 0);
				}
			}else{_XLOG_
				sSplashText=_l("Play");
				iAction=WINAMP_PLAY;
				// Отличается от паузы, просто Play
				if(bSplashEnabled && objSettings.lShowWinampTitle){_XLOG_
					::SendMessage(hwndWinamp, WM_COMMAND, 40045, 0);
				}else{_XLOG_
					::PostMessage(hwndWinamp, WM_COMMAND, 40045, 0);
				}
			}
			break;
		}
	case WINAMP_PLAY:
		{_XLOG_
			if(iAdditionalPar>0){_XLOG_
				// Паузим если запущен, ничего в противном случае
				BOOL bStopped=(iWAStatus!=0);
				if(iAdditionalPar==1 && bStopped){_XLOG_
					break;
				}
				if(iAdditionalPar==2 && !bStopped){_XLOG_
					break;
				}
			}
			{_XLOG_
				sSplashText=_l("Play/Pause");
				if(bSplashEnabled && objSettings.lShowWinampTitle){_XLOG_
					::SendMessage(hwndWinamp, WM_COMMAND, 40046, 0);
				}else{_XLOG_
					::PostMessage(hwndWinamp, WM_COMMAND, 40046, 0);
				}
			}
			break;
		}
	case WINAMP_VOLUP:
		{_XLOG_
			{_XLOG_
				sSplashText=_l("Volume")+"+";
				//int iRes=::SendMessage(hWin,WM_USER,122,122);
				//sSplashText=_l("Increasing WinAmp volume by")+Format(" %lu%%",objSettings.lVolumeCStep);
				for(int i=0;i<objSettings.lVolumeWStep;i++){_XLOG_
					::SendMessage(hwndWinamp, WM_COMMAND, 40058, 0);
				}
			}
			break;
		}
	case WINAMP_VOLDOWN:
		{_XLOG_
			{_XLOG_
				sSplashText=_l("Volume")+"-";
				//int iRes=::SendMessage(hWin,WM_USER,123,123);
				//sSplashText=_l("Decreasing WinAmp volume by")+Format(" %lu%%",objSettings.lVolumeCStep);
				for(int i=0;i<objSettings.lVolumeWStep;i++){_XLOG_
					::SendMessage(hwndWinamp, WM_COMMAND, 40059, 0);
				}
			}
			break;
		}
	}
	if(bSplashEnabled && objSettings.lShowWinampTitle && hwndWinamp && iAction!=0){_XLOG_
		char szTitle[256]="";
		::GetWindowText(hwndWinamp,szTitle,sizeof(szTitle));
		if(szTitle[0]!=0){_XLOG_
			sWinAmpTitle=szTitle;
			ReparseWinampTitle(sWinAmpTitle,hwndWinamp,FALSE);
			sSplashText+=" - ";
			sSplashText+=TrimMessage(sWinAmpTitle,objSettings.lOSDChars,1);
		}
	}
	if(bSplashEnabled){_XLOG_
		csWinAmpAction.Lock();
		static CSplashParams par;
		if(!WinAmpSplWnd){_XLOG_
			FillSplashParams(_l("WinAmp controls"),par);
			par.szText=sSplashText;
			WinAmpSplWnd=new CSplashWindow(&par);
		}else{_XLOG_
			WinAmpSplWnd->SetText(sSplashText,par.txColor,par.bgColor,par.dwTextParams);
			WinAmpSplWnd->wnd->Invalidate();
		}
		dwWinAmpSplashTimer=GetTickCount();
		FORK(AutoCloseWinAmpFlash,0);
		csWinAmpAction.Unlock();
	}
	return 1;
}

typedef BOOL (WINAPI *_Lockworkstation)();
HANDLEINTHREAD3(AppMainDlg,OnLockStation,if(TuneUpItem(SHUTDOWN_TOPIC)){return;})
{_XLOG_
	HINSTANCE hUser32=LoadLibrary("User32.dll");
	if(hUser32){_XLOG_
		_Lockworkstation fp=(_Lockworkstation)DBG_GETPROC(hUser32,"LockWorkStation");
		if(fp){_XLOG_
			WaitWhileAllKeysAreFree();
			Sleep(200);
			//SetKeyboardState();
			(*fp)();
		}
	}
	FreeLibrary(hUser32);
	return 0;
}

void RandomizeScreensaver()
{_XLOG_
	objSettings.lIsScreensaverRandomized=1;
	if(!objSettings.lSetRandomScreensaver){_XLOG_
		return;
	}
	// Считываем скриинсейверы из каталога с виндами
	CStringArray aValidScreenSavers;
	CString sData=objSettings.sValidScreenSavers;
	sData.Replace("|","\t");
	sData.Replace("\t\t","\t");
	ConvertComboDataToArray(sData,aValidScreenSavers);
	int iSize=aValidScreenSavers.GetSize();
	int iRandom=0;
	CString sScrFile;
	if(iSize>1){_XLOG_
		int iRandom=rnd(0,iSize-1);
		for(int i=0;i<objSettings.dirScreenSavers.GetSize();i++){_XLOG_
			CString sPath=objSettings.dirScreenSavers[i].sPath;
			if(aValidScreenSavers[iRandom].CompareNoCase(sPath)==0){_XLOG_
				sScrFile=sPath;
				break;
			}
		}
	}else{_XLOG_
		iSize=objSettings.dirScreenSavers.GetSize();
		if(iSize>0){_XLOG_
			sScrFile=objSettings.dirScreenSavers[rnd(0,iSize-1)].sPath;
		}
	}
	if(sScrFile!=""){_XLOG_
		CRegKey key;
		if(key.Open(HKEY_CURRENT_USER, "Control Panel\\Desktop")==ERROR_SUCCESS){_XLOG_
			if(key.m_hKey!=NULL){_XLOG_
				DWORD dwType=REG_SZ,dwSize=0;
				char szShortPath[MAX_PATH]={0};
				GetShortPathName(sScrFile,szShortPath,sizeof(szShortPath));
				RegSetValueEx(key.m_hKey,"SCRNSAVE.EXE",0,REG_SZ,(BYTE*)(LPCSTR)szShortPath,lstrlen(szShortPath)+1);
				//Возможно нужен еще и "HKEY_CURRENT_USER\Control Panel\Screen Saver. screen saver name"
			}
		}
	}
}

void AppMainDlg::AttachToClipboard(BOOL bAr)
{
	CSmartLock lock(&csClipAttach);
	CSmartLock lock2(&csClipRecreation);
	if(clipWnd){// Убиваем всегда...
		// Убиваем следящее окно
		::PostMessage(clipWnd->GetSafeHwnd(),WM_CLOSE,0,0);
		clipWnd=0;
	}
	if(bAr){
		dwClipAttachTime=GetTickCount();
		if(!clipWnd){
			if(pMainDialogWindow && !objSettings.lDisableClipboard){_XLOG_
				clipWnd=new CDLGClip();
				clipWnd->Create(CDLGClip::IDD,0);
			}
		}
	}
}


HANDLE hSSJob=0;
extern BOOL bOnScreenSaverDone;
BOOL bWinampWaspausedByUS=0;
void OnAfterScreensaver(BOOL bCheckForWinAmpOnly)
{_XLOG_
	if(objSettings.lStartupDesktop!=objSettings.lLastActiveDesktop){_XLOG_
		return;
	}
	if(pMainDialogWindow){
		pMainDialogWindow->OnReattachClips();
	}
	if(objSettings.lStartWinAmpAfterLock && pMainDialogWindow && bWinampWaspausedByUS){_XLOG_
		int iWAStatus=GetWinAmpPlStatus();
		if(iWAStatus>=1){_XLOG_
			pMainDialogWindow->WinAmpControl(WINAMP_PLAY,FALSE,FALSE,2);
		}
	}
	if(hSSJob){_XLOG_
		HWND hJobWnd=GetMainWindowFromHandle(hSSJob);
		if(hJobWnd){_XLOG_
			::PostMessage(hJobWnd,WM_CLOSE,0,0);
		}else{_XLOG_
			::TerminateProcess(hSSJob,99);
		}
		CloseHandle(hSSJob);
		hSSJob=0;
	}
	bOnScreenSaverDone=FALSE;
	bWinampWaspausedByUS=0;
}

BOOL OnBeforeScreensaver(BOOL bCheckForWinAmpOnly)
{_XLOG_
	if(objSettings.lStartupDesktop!=objSettings.lLastActiveDesktop){_XLOG_
		return 1;
	}
	BOOL bRes=FALSE;
	if(objSettings.sAppToRWS!=""){_XLOG_
		char szPath[MAX_PATH]={0};
		char szDir[MAX_PATH]={0};
		SHELLEXECUTEINFO ExecInfo;
		memset(&ExecInfo,0,sizeof(ExecInfo));
		ExecInfo.cbSize=sizeof(ExecInfo);
		ExecInfo.lpVerb="open";
		strcpy(szPath,objSettings.sAppToRWS);
		ExecInfo.lpFile=szPath;
		ExecInfo.lpParameters=NULL;
		ExecInfo.nShow=SW_SHOWMINNOACTIVE;
		ExecInfo.fMask=SEE_MASK_NOCLOSEPROCESS;
		strcpy(szDir,GetPathPart(objSettings.sAppToRWS,1,1,0,0));
		ExecInfo.lpDirectory=szDir;
		bRes=::ShellExecuteEx(&ExecInfo);
		if(ExecInfo.hProcess){_XLOG_
			if(objSettings.lStopAppToRWS){_XLOG_
				hSSJob=ExecInfo.hProcess;
			}else{_XLOG_
				CloseHandle(ExecInfo.hProcess);
			}
		}
	}
	if((objSettings.lStopWinAmpOnScrsave || objSettings.lStopWinAmpOnLock)&& pMainDialogWindow){_XLOG_
		int iWAStatus=GetWinAmpPlStatus();
		if(iWAStatus==0){_XLOG_
			pMainDialogWindow->WinAmpControl(WINAMP_PLAY,FALSE,FALSE,1);
			bWinampWaspausedByUS=1;
			bRes=TRUE;
		}
	}
	if(!bCheckForWinAmpOnly){_XLOG_
		RandomizeScreensaver();
	}
	bOnScreenSaverDone=TRUE;
	return bRes;
}

HANDLEINTHREAD3(AppMainDlg,OnHotmenuRunscreensaver,if(TuneUpItem(SCRSAVE_TOPIC)){return;})
{_XLOG_
	CSplashParams par;
	FillSplashParams(_l("Screen saver"),par);
	CSplashWindow SplWnd(&par);
	while(GetAsyncKeyState(VK_LWIN)<0 || GetAsyncKeyState(VK_RWIN)<0 ||
		GetAsyncKeyState(VK_SHIFT)<0 || GetAsyncKeyState(VK_MENU)<0 || GetAsyncKeyState(VK_CONTROL)<0){_XLOG_
		Sleep(500);// Задержка чтобы человек отпустил клавишу, перестал шевелить мышью и пр.
	}
	OnBeforeScreensaver(FALSE);
	::DefWindowProc(::GetDesktopWindow(), WM_SYSCOMMAND, SC_SCREENSAVE, 0);
	return 0;
}

void AppMainDlg::ShowHotMenu()
{_XLOG_
	if(TuneUpItem(WINCONT_TOPIC)){_XLOG_
		return;
	}
	objSettings.hHotMenuWnd=m_STray.hLastActiveWindow;
	AWndManagerMain(m_STray.hLastActiveWindow,FALSE);
	return;
}

void AppMainDlg::OnHotmenuShowexepath() 
{_XLOG_
	PostMessage(WM_COMMAND,APPINFO,LPARAM(objSettings.hHotMenuWnd));
}

void AppMainDlg::OnHotmenuAddToQRun() 
{_XLOG_
	objSettings.OpenOptionsDialog(1,DEFQRUN_TOPIC,this,1,objSettings.hHotMenuWnd);
}

// 1 - tray menu opened
// 2 - hot menu opened
// 3 - preferences opened
int AppMainDlg::GetWKState()
{_XLOG_
	if(m_STray.iTrayMenuOpened){_XLOG_
		return 1;
	}
	if(objSettings.lHotMenuOpened){_XLOG_
		return 2;
	};
	if(objSettings.lOptionsDialogOpened){_XLOG_
		return 3;
	}
	return 0;
}

void AppMainDlg::OnEnterKey()
{
	objSettings.OpenOptionsDialog(0,_l(REGISTRATION_TOPIC),this);
}


HANDLEINTHREAD(AppMainDlg,OnDonate)
{_XLOG_
	int iRes=0;
	CString sURL=regGetBuyURL("WiredPlane",PROGNAME,"3");
	if(sURL==""){_XLOG_
		if(GetApplicationLang()==1){_XLOG_
			// Наши!!!
			sURL=CString("http://www.wiredplane.com/cgi-bin/wp_engine.php?target=vendor-ru&who="PROGNAME)+PROG_VERSION;
		}else{_XLOG_
			sURL=CString("http://www.wiredplane.com/cgi-bin/wp_engine.php?target=vendor&who="PROGNAME)+PROG_VERSION;
		}
	}
	iRes=(int)ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open",sURL,NULL,NULL,SW_SHOWNORMAL);
	return iRes;
}

const char* GetAppVersion()
{_XLOG_
	static char szVersion[16]="";
	if(szVersion[0]==0){_XLOG_
		sprintf(szVersion,"%lu.%lu.%lu%s",VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,MINOR_VALUE);
	}
	return szVersion;
}

CString AppMainDlg::SetTrayTooltip(BOOL bUpdation)
{_XLOG_
	if(iRunStatus!=1){_XLOG_
		return "";
	}
	CString sTooltip="";
#if VERSION_MAJOR<2
	sTooltip=(CString)PROGNAME+" Light";
#else
	int iCPU=0;
	static CString sTooltipAddition;
	CString sRegInfo;
	if(objSettings.iLikStatus<2){_XLOG_
		sRegInfo+=_l("Unregistered");
	}else{_XLOG_
		sRegInfo=PROGNAME" v.";
		sRegInfo+=GetAppVersion();
	}
	if(objSettings.bShowCPUinTT){_XLOG_//psk->lLogCPUHungers
		iCPU=int(GetCPUTimesPercents());
		if(iCPU<0){_XLOG_
			iCPU=0;
		}
		int iMem=int(GetMemoryPercents());
		if(iMem<0){_XLOG_
			iMem=0;
		}
		if(bUpdation && iCPU>40){_XLOG_
			__FLOWLOG;
			static DWORD dwTime=0;
			//if(dwTime==0 || GetTickCount()-dwTime>5000)
			{_XLOG_
				CCSLock lpc(&pCahLock,1);
				dwTime=GetTickCount();
				// Кто жрет наши ресурсы???
				EnumProcesses(1);
				long iInd=-1;
				LONGLONG lMax=-1;
				for(int i=0; i<GetProccessCash().GetSize();i++){_XLOG_
					if(GetProccessCash()[i].sProcName!="Idle" && GetProccessCash()[i].lProcent>lMax){_XLOG_
						lMax=GetProccessCash()[i].lProcent;
						iInd=i;
					}
				}
				if(iInd!=-1){_XLOG_
					sTooltipAddition=GetProccessCash()[iInd].sProcExePath;
					sTooltipAddition.TrimLeft();
					sTooltipAddition.TrimRight();
				}
			}
		}else if(iCPU<5 || !bUpdation){_XLOG_
			sTooltipAddition="";
		}
		CString sCPU=Format("%s:%i%%",_l("CPU"),iCPU);
		CString sMem=Format("%s:%i%%",_l("Memory"),iMem);
		sTooltip=sCPU;
		if(sTooltipAddition!=""){_XLOG_
			sTooltip+="; ";
			sTooltip+=sMem;
			sTooltip+="\n";
			sTooltip+=TrimMessage(sTooltipAddition,21,3);
		}else{_XLOG_
			sTooltip+="\n";
			sTooltip+=sMem;
		}
		sTooltip+="\n";
		sTooltip+=MakeStandartLen(sRegInfo,25);//Format("%s; %s",sRegInfo,_l("Right-click for more"));
	}else{_XLOG_
		sTooltip=sRegInfo;
	}
	/*
	if(objSettings.lLBTray>0){_XLOG_
		if(sTooltip!=""){_XLOG_
			sTooltip+="\n";
		}
		sTooltip+="2*LMB: ";
		sTooltip+=TrimMessage(wk.GetBasicHotkeyDsc(objSettings.lLBTray-1),15);
	}
	if(objSettings.lMBTray>0){_XLOG_
		if(sTooltip!=""){_XLOG_
			sTooltip+="\n";
		}
		sTooltip+="2*MMB: ";
		sTooltip+=TrimMessage(wk.GetBasicHotkeyDsc(objSettings.lMBTray-1),15);
	}
	if(objSettings.lRBTray>0){_XLOG_
		if(sTooltip!=""){_XLOG_
			sTooltip+="\n";
		}
		sTooltip+="2*RMB: ";
		sTooltip+=TrimMessage(wk.GetBasicHotkeyDsc(objSettings.lRBTray-1),15);
	}
	*/
#endif
	m_STray.SetTooltipText(sTooltip);
	return sTooltip;
}

BOOL CALLBACK RegNow(void* Param, int iButtonNum);
void AppMainDlg::SetRegInfo()
{_XLOG_
	CString sVersion,sStatus;
#if VERSION_MAJOR<2
	sVersion=Format("%s %s (%s)",PROGNAME,"Light",__DATE__);
	sStatus=_l("Freeware");
#else
	sVersion=Format("%s %s (%s)",PROGNAME,PROG_VERSION,__DATE__);
#ifndef GPL
	if(objSettings.iLikStatus<2){_XLOG_
		CString sText=_l("Evaluation copy");
		if(objSettings.iLikStatus==0){_XLOG_
			sText+=Format("\r\n(%s: %i)",_l("Days left"),GetWindowDays());
		}
		sStatus=sText;
	}else{_XLOG_
		sStatus=_l("Registered to")+" "+objSettings.sLikUser;
	}
#endif
#endif
	CString sAddInfo;
	sAddInfo+=sVersion;
	sAddInfo+="\r\n";
	sAddInfo+=_l("Automation tool for power users");
	sAddInfo+="\r\n";
	sAddInfo+="\r\n";
	if(objSettings.bApplicationFirstRun){_XLOG_
		//sAddInfo+=_l("Welcome! To start to use this application,\nplease read 'Help'-'Tutorial' and look at\nhotkey bindings in 'Help' submenu");
		sAddInfo+=_l("Welcome to "PROGNAME"! Notice new icon in the tray area, right-click it to popup menu with possible actions");
		sAddInfo+=". ";
		sAddInfo+=_l("This application is a real all-in-one tool and can do a lot of useful things. Click on 'Tutorial' button for quick overview of main possibilities");
		//+"\n"+_l("Right-click tray icon to find more"));
	}else{_XLOG_
		sAddInfo+=_l("For comments, bugs reports, and any suggestions please write to")+" "+SUPPORT_EMAIL;
	}
	sAddInfo+=". ";
	sAddInfo+=_l("Feel free to ask questions at our forum");
	sAddInfo+=".\r\n";
	//sAddInfo+=_l("Visit http://www.wiredplane.com for additional information");
	//sAddInfo+=_l("Notice: this computer program is protected by copyright laws","Notice: this computer program is protected by copyright laws and international treaties. Unauthorized reproduction or distribution of this program, or any portion of it, may result in severe civil and criminal penalties, and will be prosecuted to the maximum extend possible under the law");
	sAddInfo+="\r\n";
	sAddInfo+="\r\n";
	sAddInfo+=sStatus;
	sAddInfo+="\r\n";
	sAddInfo+="(C)2002-2007. WiredPlane.com";
	GetDlgItem(IDC_INFO)->SetWindowText(sAddInfo);
	GetDlgItem(ID_ENTCODE)->SetWindowText(_l(OPTIONS_TP));
	GetDlgItem(ID_WEBSITE)->SetWindowText(_l("Tutorial"));
	GetDlgItem(IDOK)->SetWindowText(_l("Ok"));
	GetDlgItem(IDC_LEGALTITLE)->SetWindowText(_l("List of registered hotkeys"));
	CString sInfo2=GetListOfCurrentlyRegisteredHotkeys();
	sInfo2+=Format("\r\n****\r\n%s:\r\n%s: %i;\r\n%s: %i;\r\n%s: %ipx",_l("Statistics"),_l("Keypresses"),objSettings.lStat_keybCount,_l("Hotkeys"),objSettings.lStat_hotkCount,_l("Mouse"),objSettings.lStat_mouseLen);
	sInfo2+=Format("\r\n%s: %i/%i",_l("In use"),GetWindowDays(),GetWindowDaysFromInstall());
	GetDlgItem(IDC_LEGALNOTICE)->SetWindowText(sInfo2);
	SetTrayTooltip();
}

BOOL WaitWhileAllKeysAreFreeX(BOOL bCheckExplicit)
{_XLOG_
	CIHotkey hCheckHK=hLastHotkeyPressed;
	hLastHotkeyPressed.Reset();
	if(!objSettings.lDisableDblPressExtensions || bCheckExplicit){_XLOG_
		WaitWhileAllKeysAreFree(hCheckHK);
	}
	return TRUE;
}

void NoPlugError(const char* sPName)
{_XLOG_
	Alert(_l(Format("In order to use thus feature,\n'%s' plugin must be enabled. Operation aborted",sPName)),_l("	Plugin not found"));
}

typedef DWORD (WINAPI *_GetThreadHandle)();
BOOL AddToAutoHide(HWND hWin)
{_XLOG_
	HMODULE hPl=GetModuleHandle("wp_wtraits.wkp");
	if(hPl==0){_XLOG_
		NoPlugError("Windows helper");
		return 0;
	}
	_GetThreadHandle fp=(_GetThreadHandle)DBG_GETPROC(hPl,"GetThreadHandle");
	if(fp==0){_XLOG_
		NoPlugError("Windows helper");
		return 0;
	}
	DWORD dwThreadID=(*fp)();
	if(dwThreadID==0){_XLOG_
		NoPlugError("Windows helper");
		return 0;
	}
	PostThreadMessage(dwThreadID,WM_COMMAND,(WPARAM)hWin,0);
	return 0;
}

BOOL CreateLinkInFolder(CString sTargetFolder, HWND hWin)
{_XLOG_
	BOOL bRes=FALSE;
	AddSlashToPath(sTargetFolder);
	if(isFileExist(sTargetFolder)){_XLOG_
		DWORD dwProcID=0;
		CProcInfo* info=NULL;
		CString sCommand;
		CString sParameter;
		if(wk.GetFullInfoFromHWND(hWin,dwProcID,info,sCommand,sParameter)){_XLOG_
			char szFile[MAX_PATH]="";
			_splitpath(info->sProcExePath,0,0,szFile,0);
			sTargetFolder=Format("%sLink to %s.lnk",sTargetFolder,szFile);
			int iCount=1;
			while(isFileExist(sTargetFolder) && iCount<10){_XLOG_
				sTargetFolder=Format("%sLink to %s[%lu].lnk",sTargetFolder,szFile,iCount++);
			}
			bRes=(CreateLink(info->sProcExePath, sTargetFolder, Format("%s %s",_l("Shortcut to"),info->sProcName),sParameter)==S_OK);
		}
	}
	Alert(Format("%s %s %s!",_l("Shortcut to"),_l("this application was"),bRes?_l("created successfully"):_l("not created (Error occured)")));
	return bRes;
}

void AppMainDlg::OnHotmenuAddToDesktop() 
{_XLOG_
	HWND hLinkTo=objSettings.hHotMenuWnd;
	CreateLinkInFolder(getDesktopPath(),hLinkTo);
}

void AppMainDlg::OnHotmenuAddToQLaunch() 
{_XLOG_
	HWND hLinkTo=objSettings.hHotMenuWnd;
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_APPDATA);
	CString sTargetFolder=szFolderBuffer;
	AddSlashToPath(sTargetFolder);
	sTargetFolder+="Microsoft\\Internet Explorer\\Quick Launch\\";
	CreateLinkInFolder(sTargetFolder,hLinkTo);
}

void AppMainDlg::OnHotmenuAddToAHide()
{_XLOG_
	AddToAutoHide(objSettings.hHotMenuWnd);
}

void AppMainDlg::OnHotmenuAddToStartMenu() 
{_XLOG_
	HWND hLinkTo=objSettings.hHotMenuWnd;
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_STARTMENU);
	CreateLinkInFolder(szFolderBuffer,hLinkTo);
	//CSIDL_MYDOCUMENTS
}

void AppMainDlg::OnHotmenuAddToUDFolder() 
{_XLOG_
	HWND hLinkTo=objSettings.hHotMenuWnd;
	SetForegroundWindow();
	char szTargetFolder[MAX_PATH]="";
	strcpy(szTargetFolder,getMyDocPath());
	if(GetFolder(_l("Choose directory"), szTargetFolder, NULL, GetSafeHwnd())){_XLOG_
		CreateLinkInFolder(szTargetFolder,hLinkTo);
	}
}

BOOL AppMainDlg::ChangeItemIcon(const char* szItemPath)
{_XLOG_
	CString sTargetFolder=szItemPath;
	AddSlashToPath(sTargetFolder);
	if(!isFolder(sTargetFolder)){_XLOG_
		return FALSE;
	}
	CIconDialog dlg;
	CString sDesktopIni;
	ReadFile(sTargetFolder+"desktop.ini",sDesktopIni);
	if(sDesktopIni==""){_XLOG_
		sDesktopIni="[.ShellClassInfo]\r\nIconFile=???\r\nIconIndex=???\r\n";
	}
	sDesktopIni+="\r\n";
	CString sIconFile=CDataXMLSaver::GetInstringPart("IconFile=","\r\n",sDesktopIni);
	CString sIconIndex=CDataXMLSaver::GetInstringPart("IconIndex=","\r\n",sDesktopIni);
	dlg.SetIcon(sIconFile,atol(sIconIndex));
	dlg.sItemName=sTargetFolder;
	dlg.DoModal();
	int iNewIcon=0;
	CString sNewIcon;
	dlg.GetIcon(sNewIcon,iNewIcon);
	sDesktopIni.Replace("IconFile="+sIconFile+"\r\n","IconFile="+sNewIcon+"\r\n");
	sDesktopIni.Replace("IconIndex="+sIconIndex+"\r\n","IconIndex="+Format("%i",iNewIcon)+"\r\n");
	SetFileAttributes(sTargetFolder+"desktop.ini",FILE_ATTRIBUTE_NORMAL);
	SaveFile(sTargetFolder+"desktop.ini",sDesktopIni);
	SetFileAttributes(sTargetFolder+"desktop.ini",FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY);
	SetFileAttributes(sTargetFolder,FILE_ATTRIBUTE_READONLY);
	return TRUE;
}

#include "..\SmartWires\Atl\filemap.h"
afx_msg LRESULT AppMainDlg::ONWM_EXTERN(WPARAM wParam, LPARAM lParam)
{_XLOG_
	if((wParam&0x40000000)!=0){_XLOG_
		int iCounter=lParam;
		CFileMap fMap;
		fMap.Open("WK_EXTERN_STUFF_CLIPHISTORY",true,MAX_PATH);
		char* sz=(char*)fMap.Base();
		if(sz && iCounter>=0){_XLOG_
			int iFound=iCounter;
			int iRecord=iCounter;
			/*
			// Этого нельзя делать, так как из плагина приходит номер в истории :-()
			for(iRecord=0;iRecord<objSettings.sClipHistory[GetClipHistoryName(0)].GetSize();iRecord++){
				if(IsBMP(objSettings.sClipHistory[GetClipHistoryName(0)][iRecord].sItem)){
					continue;
				}
				iFound++;
				if(iFound==iCounter){
					break;
				}
			}*/
			if(iFound==iCounter && iFound<objSettings.sClipHistory[GetClipHistoryName(0)].GetSize()){
				CString sMsg=objSettings.sClipHistory[GetClipHistoryName(0)][iRecord].sItem;
				strcpy(sz,Format("%i. %s",iCounter+1,TrimMessage(sMsg,30,1)));
				return 1;
			}
		}
		return 0;
	}
	if((wParam&0x10000000)!=0){//&(~0x10000000))
		int iCounter=lParam;
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			CQuickRunItem* pQRun=objSettings.aQuickRunApps[i];
			if(pQRun && pQRun->lMacros==CATEG_MACR && pQRun->lShowInUpMenu){_XLOG_
				if(iCounter==0){_XLOG_
					CFileMap fMap;
					fMap.Open("WK_EXTERN_STUFF_CATEGS",true,MAX_PATH);
					char* sz=(char*)fMap.Base();
					if(sz){_XLOG_
						strcpy(sz,pQRun->sItemName);
						return 1;
					}
					else{_XLOG_
						return 0;
					}
				}
				iCounter--;
			}
		}
	}
	if((wParam&0x20000000)!=0){_XLOG_
		int iCounter=(wParam&(~0x20000000));
		int iInCategNum=lParam;
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			CQuickRunItem* pQRun=objSettings.aQuickRunApps[i];
			if(pQRun && pQRun->lMacros==CATEG_MACR && pQRun->lShowInUpMenu){_XLOG_
				if(iCounter==0){_XLOG_
					// Ищем подходящий
					for(int iX=0;iX<objSettings.aQuickRunApps.GetSize();iX++){_XLOG_
						CQuickRunItem* pQRunX=objSettings.aQuickRunApps[iX];
						if(pQRunX && pQRunX->lMacros!=CATEG_MACR && pQRunX->sCategoryID==pQRun->sUniqueID){_XLOG_
							if(iInCategNum==0){_XLOG_
								CFileMap fMap;
								fMap.Open("WK_EXTERN_STUFF_CATEGITEMS",true,MAX_PATH);
								char* sz=(char*)fMap.Base();
								if(sz){_XLOG_
									strcpy(sz,pQRunX->sItemName);
									strcat(sz,"\t");
									strcat(sz,pQRunX->sUniqueID);
									return 1;
								}else{_XLOG_
									return 0;
								}
							}
							iInCategNum--;
						}
					}
					return 0;
				}
				iCounter--;
			}
		}
	}
	return 0;
}

BOOL IsVistaSoundControl()
{
	return IsVista();
}
