#include "stdafx.h"
#include <process.h>
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL IsVista();
BOOL IsHelpEnabled2();
/////////////////////////////////////////////////////////////////////////////
// AppMainDlg dialog
CNewVersionChecker& GetVersionChecker()
{
	static CNewVersionChecker verChecker(PROG_VERSION,__DATE__,EscapeString(Format("ls=%i, u=%s, days=%i",objSettings.iLikStatus,objSettings.sLikUser,GetWindowDaysFromInstall())));
	return verChecker; 
}

BOOL AppMainDlg::IsIconMustBeHided(BOOL bSkipVisText)
{
	if((COleDateTime::GetCurrentTime()-objSettings.dtStartTime).GetTotalSeconds()<3){
		return FALSE;
	}
	if(objSettings.bShowTrayIconAsWP==2){
		if(!bSkipVisText && dlgWChanger && ::IsWindow(dlgWChanger->GetSafeHwnd()) && dlgWChanger->IsWindowVisible()){// Не закрываем если открыто
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK FindDeskMover(HWND hwnd,LPARAM lParam);
BOOL CALLBACK FindExplorer_Server(HWND hwnd,LPARAM lParam);
HWND GetProgman()
{
	static HWND hDesk=NULL;
/*
	DWORD dwProgmanID=0;
	DWORD dwProgmanThread=GetWindowThreadProcessId(hDesk,&dwProgmanID);
	EnumThreadWindows(dwProgmanThread,FindExplorer_Server,(LPARAM)&hDesk);
*/
	if(hDesk!=NULL && !IsWindow(hDesk)){
		hDesk=NULL;
	}


	if(hDesk==NULL){
		hDesk=::FindWindow("Progman",NULL);
	}
	return hDesk;
}

HWND GetShellDLL()
{
	static HWND hDesk=NULL;
/*
	DWORD dwProgmanID=0;
	DWORD dwProgmanThread=GetWindowThreadProcessId(hDesk,&dwProgmanID);
	EnumThreadWindows(dwProgmanThread,FindExplorer_Server,(LPARAM)&hDesk);
*/
	if(hDesk!=NULL && !IsWindow(hDesk)){
		hDesk=NULL;
	}
	if(hDesk==NULL){
		hDesk=::FindWindow("Progman",NULL);
		if(hDesk){
			hDesk = FindWindowEx(hDesk,0,"SHELLDLL_DefView",NULL); 
		}else{
			return GetSysListView();
		}
	}
	if(hDesk==0){
		return GetSysListView();
	}
	return hDesk;
}

HINSTANCE hHookLib=NULL;
BOOL bHookInstalled=FALSE;
typedef BOOL (WINAPI *_SetHook)(HWND,HWND); 
DWORD WINAPI SetDesktopHook(HWND hThisWnd)
{
	int iCount=0;
	bHookInstalled=FALSE;
	HWND hSysListView=GetShellDLL();//GetSysListView();
	if(hHookLib==NULL){
		CString sFile=CString(GetApplicationDir())+"WCDesknote.cml";
		hHookLib=LoadLibrary(sFile);
		DWORD dwErr=GetLastError();
	}
	if(hHookLib){
		_SetHook fp=(_SetHook)GetProcAddress(hHookLib,"SetHook");
		if(fp){
			while(iCount<100 && (!hSysListView || !IsWindow(hSysListView))){//::GetDesktopWindow()
				iCount++;
				Sleep(3000);
				hSysListView=GetShellDLL();//GetSysListView();
				if(!pMainDialogWindow || pMainDialogWindow->iRunStatus!=1){
					return 0;
				}
			}
			if(iCount<100){
				if(fp(hSysListView,hThisWnd)){
					bHookInstalled=TRUE;
				}
			}
		}
	}
	if(bHookInstalled==FALSE && pMainDialogWindow && !objSettings.bReportInformAlerts){
		pMainDialogWindow->m_STray.PopupBaloonTooltip(_l(AppName()+": Warning"),_l("Failed to initialize desktop notes module")+"!\n"+_l("Restart "+AppName()+" to fix the problem"));
	}
	return 0;
}

long g_lOnceScrPerventer=0;
#define TIMER_SETTIMER_TICK		(60*1000)
#define TIMER_SETTIMERH_TICK	(10*1000)
//60*1000
DWORD WINAPI GlobalOnTimer(LPVOID pValue)
{
	COleDateTime dtLast=GETCURTIME;//COleDateTime::GetCurrentTime();
	while(pMainDialogWindow){
		if(pMainDialogWindow->iRunStatus!=1){
			Sleep(500);
			continue;
		}
		DWORD dwTimeOut=(objSettings.lHideConfDataInLock || objSettings.sHideConfDataInLock2!="")?TIMER_SETTIMERH_TICK:TIMER_SETTIMER_TICK;
		////////////////////////////////////////////////
		// Нужно ли скрыть иконку...
		if(pMainDialogWindow && pMainDialogWindow->IsIconMustBeHided()){
				pMainDialogWindow->m_STray.HideIcon();
		}
		////////////////////////////////////////////////
		// Нужно ли поменять Wallpaper периодически...
		if(pMainDialogWindow && !objSettings.lMainWindowOpened){
			if((objSettings.lChangePriodically || objSettings.bNeedFirstTimeChange) && (objSettings.m_WChanger.GetLastActivePictureNum()>0)){
				if(objSettings.lChangePeriod==0){
					objSettings.lChangePeriod=30;
				}
				long lCurTick=GetTickCount();
				objSettings.lNextTimeChange=objSettings.lLastTimeChange+(objSettings.lChangePeriod*60*1000);
				if(lCurTick>=objSettings.lNextTimeChange){
					BOOL bChangeTimely=1;
					if(objSettings.bNeedFirstTimeChange){
						objSettings.bNeedFirstTimeChange=0;
						if(objSettings.bOnStartRegerate){
							bChangeTimely=0;
							pMainDialogWindow->PostMessage(WM_COMMAND,ID_WCHANGER_REFRESHWP,0);
						}
					}
					if(bChangeTimely){
						pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER_TIMELY,0);
					}
				}
			}
			if(objSettings.lHideConfDataInLock || objSettings.sHideConfDataInLock2!=""){
				static BOOL bPrevState=0;
				objSettings.lConfModeIsON=isScreenSaverActiveOrCLocked()?1:0;
				if(objSettings.lConfModeIsON!=bPrevState){
					if(objSettings.bChangetWPIfScrSaver!=2){
						pMainDialogWindow->OrderModifierForNextWallpaper=-3;
						if(objSettings.sHideConfDataInLock2!=""){
							objSettings.sLastWallpaperFromUser=objSettings.sHideConfDataInLock2;
						}
					}
					pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
				}
				bPrevState=objSettings.lConfModeIsON;
			}else if(objSettings.bChangetWPIfScrSaver==2){// Может вошли в скринсейвер?
				dwTimeOut=20*1000;
				if(isScreenSaverActiveOrCLocked()){
					if(g_lOnceScrPerventer==0){
						pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
					}
				}else{
					g_lOnceScrPerventer=0;
				}
			}
		}
		////////////////////////////////////////////////
		// Наступление нового дня?
		static DWORD dwLastAsk=0;
		if(objSettings.iLikStatus<2){
			if(dwLastAsk==0 || (GetTickCount()-dwLastAsk)>(7*60*60*1000)){//7 часов
				dwLastAsk=GetTickCount();
				PerformUnregActions();
			}
		}
		COleDateTime dt=GETCURTIME;//COleDateTime::GetCurrentTime();
		if((dt.GetDay()!=dtLast.GetDay() || dt.GetMonth()!=dtLast.GetMonth() || dt.GetYear()!=dtLast.GetYear())){
			UpdateTrialityStatus(objSettings.sLikUser,objSettings.sLikKey);
			if(!USE_ELEFUN){
				if(objSettings.lCheckForUpdate){
					GetVersionChecker().SetTraits(Format("chs=%i, strs=%i",objSettings.lStartsCount,objSettings.lChangesCount), 0);
					StartCheckNewVersion(objSettings.lDaysBetweenChecks);
				}
			}
			if(objSettings.lRefreshOnNewDay && pMainDialogWindow){
				pMainDialogWindow->OrderModifierForNextWallpaper=-3;
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
			}
			dwLastAsk=0;
		}
		dtLast=dt;
		if(pMainDialogWindow){
			pMainDialogWindow->SetTrayTooltip();
		}
		////////////////////////////////////////////////
		// Нужно ли установить хук...
		if(!bHookInstalled && pMainDialogWindow && objSettings.bSaveAllowed){// Все уже считано, а хук не установлен
			SetDesktopHook(pMainDialogWindow->GetSafeHwnd());
		}
		////////////////////////////////////////////////
		// Ждем до следующей проверки
		WaitForSingleObject(objSettings.hGlobalTimerEvent, dwTimeOut);
		if(!pMainDialogWindow || pMainDialogWindow->iRunStatus==-1){
			break;
		}
	};
	// Вырубаем хук
	if(hHookLib){
		if(bHookInstalled){
			_SetHook fp=(_SetHook)GetProcAddress(hHookLib,"SetHook");
			if(fp){
				fp(NULL,NULL);
			}
		}
		FreeLibrary(hHookLib);
	}
	return 0;
}

void AppMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AppMainDlg)
	DDX_Control(pDX, IDC_LOGO, m_Logo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AppMainDlg, CDialog)
	//{{AFX_MSG_MAP(AppMainDlg)
	ON_BN_CLICKED(ID_ENTCODE, OnEntcode)
	ON_BN_CLICKED(ID_WEBSITE2, OnWebsite2)
	ON_BN_CLICKED(ID_WEBSITE3, OnWebsite3)
	ON_BN_CLICKED(ID_WEBSITE, OnWebsite)
	ON_BN_CLICKED(ID_EMAIL, OnEmail)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SYSTRAY_ENTERKEY, OnSystrayOptionsKey)
	ON_COMMAND(ID_SYSTRAY_OPTIONS, OnSystrayOptions)
	ON_COMMAND(ID_SYSTRAY_IMPORTCLOCK, OnSystrayImportClock)
	ON_WM_ENDSESSION()
 	ON_COMMAND(ID_SYSTRAY_BACKUPLIST, OnSystrayBackUpList)
	ON_COMMAND(ID_SYSTRAY_RESTORELIST, OnSystrayRestoreList)
	ON_COMMAND(ID_SYSTRAY_NEWDESKTOPNOTE, OnSystrayNewNote)
	ON_COMMAND(ID_SYSTRAY_NOTEOPTIONS, OnSystrayNoteOptions)
	ON_COMMAND(ID_SYSTRAY_DISABLENOTES, OnSystrayNoteDisable)
	ON_COMMAND(ID_SYSTRAY_ABOUT, OnSystrayAbout)
	ON_COMMAND(ID_SYSTRAY_EXIT, OnSystrayExit)
	ON_COMMAND(ID_SYSTRAY_OPEN, OnSystrayOpen)
	ON_COMMAND(ID_SYSTRAY_OPEN2, OnSystrayOpen2)
	ON_COMMAND(ID_SYSTRAY_HELP, OnSystrayHelp)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(FIREALERT, OnFireAlert)
	ON_WM_CTLCOLOR()
	ON_REGISTERED_MESSAGE(theApp.iWM_WIRENOTE, ONWM_WIRENOTE)
	ON_REGISTERED_MESSAGE(theApp.iWM_THIS, ONWM_THIS)
	ON_REGISTERED_MESSAGE(theApp.iWM_SSAVE, ONWM_SSAVE)
	ON_COMMAND(ID_SYSTRAY_REGISTER, OnSystrayRegister)
	ON_COMMAND(ID_SYSTRAY_CHANGEWALLPAPER, SetNextWallPaper)
	ON_COMMAND(ID_WCHANGER_REFRESHWP, SetTheSameWallPaper)
	ON_COMMAND(ID_SYSTRAY_CHANGEWALLPAPER_TIMELY, SetNextWallPaperTimely)
	ON_COMMAND(ID_SYSTRAY_NEXTWALLP, OnOrderNextWallPaper)
	ON_COMMAND(ID_SYSTRAY_PREVWALLP, OnOrderPrevWallPaper)
	ON_COMMAND(ID_SYSTRAY_HIDESHOWICONS, OnShowHideIcons)
	ON_COMMAND(ID_SYSTRAY_RANDWALLP, OnOrderRandWallPaper)
	ON_COMMAND(ID_SYSTRAY_CHANGEPERIODICALLY, OnSetChangePeriodically)
	ON_COMMAND(ID_SYSTRAY_SENDASUGGESTION, OnSystraySendasuggestion)
	ON_COMMAND(ID_SYSTRAY_DOWNLOADWALLPAPERS, OnSystrayDownloadWp)
	ON_COMMAND(ID_SYSTRAY_DOWNLOADTEMPLATES, OnSystrayDownloadWpt)
	ON_COMMAND(ID_SYSTRAY_DISABLEWIDGETST, OnSystrayDisableWpt)
	ON_COMMAND(ID_SYSTRAY_RESTART, OnSystrayRestart)
	ON_COMMAND(ID_SYSTRAY_DOWNLOADSKINS, OnSystrayDownloadSkins)
	ON_COMMAND(ID_SYSTRAY_DOWNLOADCLOCKS, OnSystrayDownloadClocks)
	ON_COMMAND(ID_SYSTRAY_ENLIVEIMAGE, OnSystrayEnliveWp)
	ON_COMMAND(ID_SYSTRAY_EXCHANGEWALLPAPER, OnSystrayExchangeWp)
	ON_COMMAND(ID_SYSTRAY_EXCHANGEWALLPAPER2, OnSystrayExchangeWp)
	ON_COMMAND(ID_SYSTRAY_CLANCHMONEY, OnSystrayClanchMoney)
	ON_COMMAND(ID_SYSTRAY_TELLAFRIEND, OnSystrayTellAFriend)
	ON_COMMAND(ID_SYSTRAY_GOTOWEBSITE, OnBuyNow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppMainDlg message handlers
int GetTemplateMainIndex(const char* szTemplate)
{
	return GetIndexInArray(szTemplate, &objSettings.aLoadedWPTs);
}

void AppendNotes(CMenu& pNotes, BOOL bMain, BOOL bWnd)
{
	AddMenuString(&pNotes,ID_SYSTRAY_NEWDESKTOPNOTE,_l("New note")+getActionHotKeyDsc(NEW_NOTE),_bmp().Get(_IL(6)));
	CSmartLock lN(&objSettings.cNoteLock,TRUE,-1);
	int i;
	for(i=0;i<objSettings.aDesktopNotes.GetSize();i++){
		CNote& nt=objSettings.aDesktopNotes[i];
		CBitmap *bmp=0;
		if(nt.iIcon>=0){
			bmp=_bmp().Get(GetNSkinBmp(objSettings.lNoteSkin,"ICO_TN",nt.iIcon));
		}else{
			bmp=_bmp().Get(nt.sCustomIcon+DEFNOTEPRFM);
		}
		if(!bmp){
			bmp=_bmp().GetRaw(IDB_BM_NOICON);
		}
		AddMenuString(&pNotes,WM_USER+OPEN_NOTE+i,TrimMessage(nt.sTitle,30,2),bmp);
	}
	if(!bWnd && objSettings.aDesktopNotes.GetSize()>0){
		pNotes.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&pNotes,ID_SYSTRAY_DISABLENOTES,objSettings.lDisableNotes?_l("Enable notes"):_l("Disable notes"),objSettings.lDisableNotes?_bmp().Get(_IL(6)):_bmp().Get(_IL(78)));
	}
	pNotes.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(&pNotes,ID_SYSTRAY_NOTEOPTIONS,_l("Notes options"),_bmp().Get(IDB_BM_OPTIONS));
	if(!bHookInstalled && !bMain){
		pNotes.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&pNotes,WM_USER+OPEN_NOTE+i+1,_l("No desktop found")+"!",_bmp().Get(_IL(60)),0,0,0,1);
	}
}

CMenu* _stdcall RefreshMenu(void* pData)
{
	// создаем меню в зависимости от настроек
	CMenu* m_pMenu=new CMenu();
	m_pMenu->CreatePopupMenu();
	BOOL bLockChange=(objSettings.lSetWallaperLock>0)?TRUE:FALSE;
	if(objSettings.m_WChanger.GetLastActivePictureNum()>0){
		if(objSettings.bChangeWPaperCycleType==0){
			AddMenuString(m_pMenu,ID_SYSTRAY_CHANGEWALLPAPER,_l("Change now")+getActionHotKeyDsc(NEXT_WALLPAPER),_bmp().Get(_IL(49)),FALSE,NULL,bLockChange);
		}else{
			AddMenuString(m_pMenu,ID_SYSTRAY_NEXTWALLP,_l("Next image")+getActionHotKeyDsc(ORDER_NEXT),_bmp().Get(_IL(13)),FALSE,NULL,bLockChange);
			AddMenuString(m_pMenu,ID_SYSTRAY_PREVWALLP,_l("Previous image")+getActionHotKeyDsc(ORDER_PREV),_bmp().Get(_IL(12)),FALSE,NULL,bLockChange);
		}
	}
	m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(m_pMenu,ID_SYSTRAY_OPEN,_l("Wallpapers")+"/"+_l("Widgets")+getActionHotKeyDsc(BRING_INTOVIEW),_bmp().Get(IDB_BM_EDIT));
#ifndef ART_VERSION
	//AddMenuString(m_pMenu,ID_SYSTRAY_OPEN2,_l("Widgets list"),_bmp().Get(_IL(74)));
	AddMenuString(m_pMenu,ID_SYSTRAY_OPEN2,_l("Calendar")+getActionHotKeyDsc(OPEN_CALEN),_bmp().Get(_IL(80)));
#endif
	m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
#ifdef ART_VERSION
	AddMenuString(m_pMenu,ID_SYSTRAY_OPEN2,_l("Art options"),_bmp().Get(_IL(74)));
#endif
#ifndef ART_VERSION
	// Эффекты и часы
	BOOL bTemplatesPresent=FALSE;
	CMenu pTemplAct;
	if(objSettings.aLoadedWPTs.GetSize()>0){
		// Настройки активных шаблонов
		pTemplAct.CreatePopupMenu();
		CStringArray aPresentLines;
		if(objSettings.TemplatesInWork.GetSize()>0){
			for(int i=0;i<objSettings.TemplatesInWork.GetSize();i++){
				int iActiveWpt=GetTemplateMainIndex(objSettings.TemplatesInWork[i]);
				if(iActiveWpt>=0){
					CWPT* wpt=GetWPT(iActiveWpt);
					if(wpt && wpt->sCategs==CHIDDEN){
						continue;
					}
					if(wpt && IsVista() && wpt->isAD()){
						continue;
					}
					for (int j=0;j<wpt->aLoadedWPTsActionsDsc.GetSize();j++){
						CString sTemplParTitle=wpt->aLoadedWPTsActionsDsc[j];
						if(GetIndexInArray(sTemplParTitle,&aPresentLines)==-1){
							if(wpt->aLoadedWPTsActionsFlags[j]==2){
								continue;
							}
							CMenu* pTargetMenu=&pTemplAct;
							if(wpt->aLoadedWPTsActionsFlags[j]){
								pTargetMenu=m_pMenu;
							}
							AddMenuString(pTargetMenu,WM_USER+CHANGE_TEMPL_PAR+MAXROOTS_PERTEMPL*iActiveWpt+j,_l(sTemplParTitle));
							aPresentLines.Add(sTemplParTitle);
						}
					}
				}
			}
		}
		CMenu pTemplRemove;
		pTemplRemove.CreatePopupMenu();
		CMenu& pTempl=pTemplAct;
		//pTempl.CreatePopupMenu();
		{// Списки шаблонов
			CMap<CString,const char*,CMenu*,CMenu*> aCategs;
			CStringArray aCategsOrder;
			for (int i=0;i<objSettings.aLoadedWPTs.GetSize();i++){
				BOOL bActive=IsWPTActive(objSettings.aLoadedWPTs[i]);
				CMenu* pPutTo=&pTemplRemove;
				CWPT* pWPT=GetWPT(objSettings.aLoadedWPTs[i]);
				if(!pWPT){
					continue;
				}
				CString sCateg=pWPT->sCategs;
				if(sCateg==CHIDDEN){
					continue;
				}
				if(IsVista() && pWPT->isAD()){
					continue;
				}
				CString sTitle=_l(pWPT->sTitle);
				BOOL bAdded=0;
				if(!bActive){
					pPutTo=&pTempl;
					if(sCateg!=""){
						CMenu* pCategMenu=0;
						if(!aCategs.Lookup(sCateg,pCategMenu)){
							pCategMenu=new CMenu();
							pCategMenu->CreatePopupMenu();
							aCategs.SetAt(sCateg,pCategMenu);
							aCategsOrder.Add(sCateg);
							bAdded=1;
						}
						pPutTo=pCategMenu;
					}
				}else{
					long dwWe=pWPT->GetWeight();
					if(dwWe<100 && dwWe>0){
						sTitle+="\t(";
						sTitle+=Format("%i",dwWe);
						sTitle+="%)";
					}
				}
				AddMenuString(pPutTo,WM_USER+CHANGE_TEMPLATE+i,sTitle,NULL,bActive);
			}
			{// Заносим допменюшки
				CMenu* pMenu=0;
				for(int i=0;i<aCategsOrder.GetSize();i++){
					if(pMenu==0){
						pTemplAct.AppendMenu(MF_SEPARATOR, 0, "");
					}
					CString sCategName=aCategsOrder[i];
					aCategs.Lookup(sCategName,pMenu);
					AddMenuSubmenu(&pTempl,pMenu,_l(sCategName));
					delete pMenu;
				}
			}
		}
		AddMenuString(&pTemplAct,ID_SYSTRAY_DOWNLOADTEMPLATES,_l("Download widgets"),_bmp().Get(_IL(22)));
		if(pTemplAct.GetMenuItemCount()>0){
			pTemplAct.AppendMenu(MF_SEPARATOR, 0, "");
		}
		if(objSettings.TemplatesInWork.GetSize()>0){
			bTemplatesPresent=TRUE;
			pTemplRemove.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&pTemplRemove,WM_USER+CHANGE_TEMPLATE+objSettings.aLoadedWPTs.GetSize(),_l("Disable all"),_bmp().Get(_IL(78)),objSettings.TemplatesInWork.GetSize()==0?TRUE:FALSE);
			AddMenuSubmenu(&pTemplAct,&pTemplRemove,_l("Disable"),_bmp().Get(IDB_BM_OPTIONS));
			AddMenuString(&pTemplAct,ID_SYSTRAY_DISABLEWIDGETST,_l("Disable widgets"),_bmp().Get(_IL(74)));
		}
		//AddMenuSubmenu(&pTemplAct,&pTempl,_l("Activate widget"),_bmp().Get(_IL(74)));
	}
#endif
	{// Ручная смена
		if(objSettings.m_WChanger.GetLastActivePictureNum()>0){
			CMenu pRotation;
			pRotation.CreatePopupMenu();
			{// Активные темы
				if(objSettings.m_WChanger.aUsedThemes.GetSize()>1){
					CMenu pThemes;
					pThemes.CreatePopupMenu();
					int iSuize=objSettings.m_WChanger.aUsedThemes.GetSize();
					for(int i=0;i<iSuize;i++){
						WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[objSettings.m_WChanger.aUsedThemes[i]];
						if(wp){
							BOOL bBreak=(iSuize>30 && (i>0) && (i%20==0));
							AddMenuString(&pThemes,WM_USER+CHANGE_THEME+i,wp->sTitle,NULL,wp->dwStatus,0,0,0,bBreak);
						}
					}
					AddMenuSubmenu(&pRotation,&pThemes,_l("Themes"),_bmp().Get(_IL(9)));
				}
			}
			AddMenuString(&pRotation,ID_SYSTRAY_CHANGEPERIODICALLY,Format("%s %s",_l("Change every"),GetReadableStringFromMinutes(objSettings.lChangePeriod)),_bmp().Get(_IL(17)),objSettings.lChangePriodically,_bmp().Get(_IL(16)));
			AddMenuString(&pRotation,ID_WCHANGER_REFRESHWP,_l("Refresh current")+getActionHotKeyDsc(ORDER_SAME),_bmp().Get(_IL(49)),FALSE,NULL,bLockChange);
			if(objSettings.bChangeWPaperCycleType==0){
				AddMenuString(&pRotation,ID_SYSTRAY_NEXTWALLP,_l("Next image")+getActionHotKeyDsc(ORDER_NEXT),_bmp().Get(_IL(13)),FALSE,NULL,bLockChange);
				AddMenuString(&pRotation,ID_SYSTRAY_PREVWALLP,_l("Prev image")+getActionHotKeyDsc(ORDER_PREV),_bmp().Get(_IL(12)),FALSE,NULL,bLockChange);
			}
			AddMenuString(&pRotation,ID_SYSTRAY_RANDWALLP,_l("Random image"),_bmp().Get(_IL(11)),FALSE,NULL,bLockChange);
			if(!isWin9x()){
				if(::IsWindowVisible(GetSysListView())){
					AddMenuString(&pRotation,ID_SYSTRAY_HIDESHOWICONS,_l("Hide desktop icons")+getActionHotKeyDsc(HIDE_ICONS),_bmp().Get(_IL(23)));
				}else{
					AddMenuString(&pRotation,ID_SYSTRAY_HIDESHOWICONS,_l("Show desktop icons")+getActionHotKeyDsc(HIDE_ICONS),_bmp().Get(_IL(23)));
				}
			}
			if(!objSettings.isLight){
				if(objSettings.lShowAnimationItems && objSettings.hEnliver){
					pRotation.AppendMenu(MF_SEPARATOR, 0, "");
					AddMenuString(&pRotation,ID_SYSTRAY_ENLIVEIMAGE,_l("Enlive wallpaper"),NULL,objSettings.lEnliverON);
				}
			}
			AddMenuSubmenu(m_pMenu,&pRotation,_l("Wallpapers"),_bmp().Get(_IL(10)));
		}
	}
#ifndef ART_VERSION
	if(objSettings.lTempDisableAllWidgets){
		AddMenuString(m_pMenu,ID_SYSTRAY_DISABLEWIDGETST,_l("Enable widgets"),_bmp().Get(_IL(74)));
	}else{
		if(objSettings.TemplatesInWork.GetSize()>0){
			AddMenuSubmenu(m_pMenu,&pTemplAct,_l("Widgets"),_bmp().Get(_IL(74)));
		}
		{// Пресеты
			CStringArray aPres;
			GetPresetList(aPres);
			if(aPres.GetSize()>1){
				CMenu mPres;
				mPres.CreatePopupMenu();
				for(int i=0;i<aPres.GetSize();i++){
					AddMenuString(&mPres,WM_USER+ACTIVATE_PRESET+i,aPres[i]==""?_l("Default"):aPres[i],0,(aPres[i]==objSettings.sActivePreset)?1:0);
				}
				AddMenuSubmenu(m_pMenu,&mPres,_l("Presets"),_bmp().Get(_IL(61)));
			}
		}
	}
#endif
#if	!defined(USE_LITE) && !defined(ART_VERSION)
	{// Desktop notes
		CMenu pNotes;
		pNotes.CreatePopupMenu();
		AppendNotes(pNotes,0,0);
		AddMenuSubmenu(m_pMenu,&pNotes,_l("Notes"),_bmp().Get(_IL(50)));
	}
#endif

	CMenu pHelp;
	pHelp.CreatePopupMenu();
	if(IsHelpEnabled2()){
		AddMenuString(&pHelp,ID_SYSTRAY_HELP,_l("Help topics"),_bmp().Get(IDB_BM_HELP));
	}
	AddMenuString(&pHelp,ID_SYSTRAY_ABOUT,_l("About program"),_bmp().Get(IDB_BM_ABPROG));
#ifndef ART_VERSION
	if(!USE_ELEFUN){
		AddMenuString(&pHelp,ID_SYSTRAY_CHECKFORUPDATES,_l("Check for update"),_bmp().Get(_IL(22)),FALSE,NULL,IsUpdateInProgress());
	}
#endif
	AddMenuString(&pHelp,ID_SYSTRAY_TELLAFRIEND,_l("Tell a friend"),_bmp().Get(_IL(36)));
	pHelp.AppendMenu(MF_SEPARATOR, 0, "");
	if(AddDefsInteract()){
		AddMenuString(&pHelp,ID_SYSTRAY_EXCHANGEWALLPAPER,_l("Exchange wallpaper"),_bmp().Get(_IL(72)));
	}
	AddMenuString(&pHelp,ID_SYSTRAY_DOWNLOADWALLPAPERS,_l("Download wallpapers"),_bmp().Get(_IL(73)));
#ifndef ART_VERSION
	AddMenuString(&pHelp,ID_SYSTRAY_DOWNLOADSKINS,_l("Download skins"),_bmp().Get(_IL(73)));
	AddMenuString(&pHelp,ID_SYSTRAY_SENDASUGGESTION,_l("Support by Email"),_bmp().Get(_IL(44)));
	AddMenuString(&pHelp,ID_SYSTRAY_RESTART,_l("Restart "+AppName()),_bmp().Get(_IL(40)));
#endif
	AddMenuSubmenu(m_pMenu,&pHelp,_l("Help"),_bmp().Get(IDB_BM_HELP));
#ifndef GPL
	BOOL bEnterKeyPresent=0;
	if(objSettings.iLikStatus<2 || isTempKey()){
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		bEnterKeyPresent=1;
		AddMenuString(m_pMenu,ID_SYSTRAY_REGISTER,_l("Enter key"),_bmp().Get(IDB_BM_LICKEY));
		AddMenuString(m_pMenu,ID_SYSTRAY_GOTOWEBSITE,_l("Unlock all features"),_bmp().Get(_IL(15)));
		if(!isTempKey() || isTempKeyInvalid()){
			AddMenuString(m_pMenu,ID_SYSTRAY_EXCHANGEWALLPAPER2,_l("Register for one week")+"\t"+_l("Free"),_bmp().Get(_IL(72)));
		}
	}
#endif
	m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(m_pMenu,ID_SYSTRAY_OPTIONS,_l(OPTIONS_TP),_bmp().Get(IDB_BM_OPTIONS));
#ifndef GPL
	if(objSettings.iLikStatus<2 && bEnterKeyPresent==0)
	{
		AddMenuString(m_pMenu,ID_SYSTRAY_ENTERKEY,_l("Enter key")+"...",NULL);
	}
#endif
	//-
	m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(m_pMenu,ID_SYSTRAY_EXIT,_l("Exit"),_bmp().Get(_IL(40)),FALSE,NULL,objSettings.bDoModalInProcess!=0);
	return m_pMenu;
}

AppMainDlg::AppMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(/*AppMainDlg::IDD, pParent*/)
{
	//{{AFX_DATA_INIT(AppMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = ::AfxGetApp()->LoadIcon(MAIN_ICON);
	iRunStatus=-1;
	pOpenedTemplate=0;
	bAnyPopupIsActive=FALSE;
	bAllLoaded=FALSE;
	dlgWChanger=NULL;
	OrderModifierForNextWallpaper=UNDEFVALUE;
}

AppMainDlg::~AppMainDlg()
{
}

void AppMainDlg::SetRegInfo()
{
	CString sText=_l("Evaluation copy");
#ifndef GPL
	if(isTempKey() && isTempKeyDate().GetStatus()!=COleDateTime::invalid){
		sText=_l("Time-limited registration")+": "+objSettings.sLikUser;
		sText+=Format(", %s: %s",_l("Expiration date"),isTempKeyDate().Format("%x"));
	}else if(objSettings.iLikStatus<2){
		sText=_l("Not registered");
	}else if(objSettings.sLikUser!=""){
		sText=_l("Registered to")+": "+objSettings.sLikUser;
	}
#endif
	GetDlgItem(IDC_REGSTATUS)->SetWindowText(sText);
	GetDlgItem(ID_ENTCODE)->SetWindowText(_l("Main window"));
	objSettings.sTrayTooltip=CString(AppName())+" - v."+PROG_VERSION;
	if(isTempKey()){
		objSettings.sTrayTooltip=objSettings.sMainWindowTitle+" - "+_l("Time-limited key");
	}
#ifndef GPL
	if(objSettings.iLikStatus<2){
		GetDlgItem(ID_ENTCODE)->SetWindowText(_l("Enter key"));
		objSettings.sTrayTooltip=objSettings.sMainWindowTitle+" - "+_l("Unregistered");
	}
#endif
	Invalidate();
	GetDlgItem(ID_WEBSITE2)->SetWindowText(_l("Support forum"));
	GetDlgItem(ID_WEBSITE3)->SetWindowText(_l("Web site"));
	GetDlgItem(ID_EMAIL)->SetWindowText(_l("Compose new mail message"));
	if(objSettings.iLikStatus==2 && !isTempKey()){
		GetDlgItem(ID_WEBSITE)->SetWindowText(_l("Tell a friend")+"!");
	}
#ifndef GPL
	else{
		GetDlgItem(ID_WEBSITE)->SetWindowText(_l("Buy online now")+"!");
	}
#endif
#ifdef ART_VERSION
	GetDlgItem(IDC_REGSTATUS)->SetWindowText(_l("Art edition"));
#endif
	if(objSettings.isLight){
		GetDlgItem(IDC_REGSTATUS)->SetWindowText(_l("Light version"));
	}
	SetTrayTooltip();
	GetDlgItem(IDC_RUNSTATS)->SetWindowText(Format("%s: %s %lu %s, %s %lu %s",_l("Statistics"),_l("Started"),objSettings.lStartsCount,_l("time(s)"),_l("Wallpapers processed"),objSettings.lChangesCount,_l("time(s)")));
	GetDlgItem(IDOK)->SetWindowText(_l("Close"));
}


CString g_sPathToSaveBmp;
DWORD WINAPI GenerateWPInThread(LPVOID q)
{
	CString sBMP=g_sPathToSaveBmp;
	static long l=0;
	if(l>0){
		return 0;
	}
	SimpleTracker lc(l);
	AppMainDlg* p=(AppMainDlg*)q;
	// Генерим картинку!!!
	if(sBMP==""){
		sBMP=GetUserFolderRaw();
		sBMP+="Screensaver\\";
		CreateDirIfNotExist(sBMP);
		sBMP+="Wired_Screensaver.bmp";
	}
	HBITMAP hbmp=0;
	int iNextWpNum=objSettings.m_WChanger.GetNextWPNumber(0);
	if(iNextWpNum<0 || iNextWpNum>=objSettings.m_WChanger.GetLastActivePictureNum()){
		iNextWpNum=rnd(0,objSettings.m_WChanger.GetLastActivePictureNum()-1);
	}
	if(iNextWpNum>=0 && iNextWpNum<objSettings.m_WChanger.GetLastActivePictureNum()){
		CString sX,sY;
		ReadFile(sBMP+".cx",sX);
		ReadFile(sBMP+".cy",sY);
		CRect outSize=theApp.GetDesktopRECT();
		if(sX!="" && sY!=""){
			outSize.top=0;
			outSize.left=0;
			outSize.right=atol(sX);
			outSize.bottom=atol(sY);
		}
		p->GenerateFullFeaturedWallpaper(objSettings.m_WChanger.aImagesInRotation[iNextWpNum],hbmp,outSize,1);
		SaveBitmapToDisk(hbmp,sBMP+"_");
		ClearBitmap(hbmp);
		flushall();
		DeleteFile(sBMP);
		MoveFile(sBMP+"_",sBMP);
		CString sTmp;
		ReadFile(sBMP+".cnt",sTmp);
		SaveFile(sBMP+".cnt",Format(atol(sTmp)+1));
	}else{
		Alert(_l("No images found")+"!",3000);
	}
	return 0;
}

HANDLE hGlo_Copy=0;
DWORD WINAPI WaitForGenWallRequest(LPVOID q)
{
	AppMainDlg* p=(AppMainDlg*)q;
	HANDLE hGlo=CreateEvent(0,0,0,"WC_GENERATE_FILE");
	hGlo_Copy=hGlo;
	while(1){
		WaitForSingleObject(hGlo,INFINITE);
		if(p->iRunStatus!=1){
			break;
		}
		g_sPathToSaveBmp="";
		//FORK(GenerateWPInThread,q);
		GenerateWPInThread(q);
		ResetEvent(hGlo);
	}
	CloseHandle(hGlo);
	return 0;
}

DWORD WINAPI TrackWallpaperChange(LPVOID pType);
BOOL AppMainDlg::OnInitDialog()
{
	::SetProp(::FindWindow("Progman",NULL),"WC_MAIN",(HANDLE)GetSafeHwnd());
	{// Еще и в реестр...
		LONG res; HKEY skey; DWORD val;
		res=RegCreateKeyEx(HKEY_CURRENT_USER,PROG_REGKEY,0,0,0,KEY_ALL_ACCESS,0,&skey,0);
		if (res==ERROR_SUCCESS){
			val=(DWORD)GetSafeHwnd();
			RegSetValueEx(skey,_T("MainWindow"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
		}
	}
	srand(time(NULL)+::GetCurrentThreadId());
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,0,0);//HKEY_CURRENT_USER,PROG_REGKEY"\\MainWindowPos"
	CRect minRect(0,0,DIALOG_MINXSIZE,DIALOG_MINYSIZE);
	Sizer.SetMinRect(minRect);
	Sizer.SetOptions(STICKABLE);
	m_STray.Create(NULL, WM_USER, objSettings.sTrayTooltip, ::AfxGetApp()->LoadIcon(MAIN_ICON),IDR_SYSTRAY, this, IsIconMustBeHided());
	m_STray.SetMenuDefaultItem(0, TRUE);
	m_STray.SetUserMenu(&RefreshMenu,NULL);
	//
	m_Logo.SetStyles(CBS_FLAT|CBS_NONSQR|CBS_HIQUAL);
	SetWindowText(_l("About"));
	SetRegInfo();
	GetDlgItem(IDOK)->SetWindowText(_l("Ok"));
	CString sVersion=Format("%s %s (%s)",AppName(),PROG_VERSION,__DATE__);
	GetDlgItem(IDC_PRVERSION)->SetWindowText(sVersion);
	GetDlgItem(IDC_COPYRIGHT)->SetWindowText("(C)2002-2006. WiredPlane.com");
	GetDlgItem(IDC_EMDSC)->SetWindowText(_l("For comments, bugs reports, and any suggestions please write to")+" "+SUPPORT_EMAIL);
	GetDlgItem(IDC_DOWNLOADDSC)->SetWindowText(_l("You can download latest version from")+" "+DOWNLOAD_URL);
	GetDlgItem(IDC_LEGALNOTICE)->SetWindowText(_l("Notice: this computer program is protected by copyright laws","Notice: this computer program is protected by copyright laws and international treaties. Unauthorized reproduction or distribution of this program, or any portion of it, may result in severe civil and criminal penalties, and will be prosecuted to the maximum extend possible under the law"));
	// Запускаем таймер...
	objSettings.m_WChanger.Load();
	FORK(GlobalOnTimer,0);
#ifndef ART_VERSION
	DWORD dwRegTrackerId=0;
	HANDLE RegTracker1=::CreateThread(NULL, 0, TrackWallpaperChange, LPVOID(0), 0, &dwRegTrackerId);
	CloseHandle(RegTracker1);
	HANDLE RegTracker2=::CreateThread(NULL, 0, TrackWallpaperChange, LPVOID(1), 0, &dwRegTrackerId);
	CloseHandle(RegTracker2);
#endif
	//
	SetTrayTooltip();
	ttDesktop.Create(GetSafeHwnd(), "...", AfxGetResourceHandle(), TTS_NOPREFIX , NULL, TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT);
	SetTaskbarButton(this->GetSafeHwnd());
	RegisterMyHotKeys();
	iRunStatus=1;
	FORK(WaitForGenWallRequest,this);
	// Говорим всем что прога запустилась
	DWORD dwTarget=BSM_APPLICATIONS;
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(0), LPARAM(0));
	if(objSettings.m_WChanger.GetLastActivePictureNum()==0){
		PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN,0);
	}
	if(!USE_ELEFUN){
		if(objSettings.lCheckForUpdate){
			GetVersionChecker().SetTraits(Format("chs=%i, strs=%i",objSettings.lStartsCount,objSettings.lChangesCount), 0);
			StartCheckNewVersion(objSettings.lDaysBetweenChecks);
		}
	}
	//PerformUnregActions();// Это в глобалонтаймер теперь
	ttDesktop.SetAutoTrack(1);
	ApplyDeskIconsOptions();
	if(objSettings.lVeryFirstStart && isWin98()){
		Alert(_l("Warning: This program were not tested in Windows 98 and may work incorrect"));
	}
	if(GlobalFindAtom("WPLABSDEBUG-WC")){
		GlobalDeleteAtom(GlobalFindAtom("WPLABSDEBUG-WC"));
	}
	if(objSettings.lEnableDebug){
		GlobalAddAtom("WPLABSDEBUG-WC");
	}
	char szWptRegistered[32]={0};
	GetRegSetting("", "WptRegistered", szWptRegistered, sizeof(szWptRegistered));
	if(szWptRegistered[0]==0){_XLOG_
		strcpy(szWptRegistered,"yes");
		SetRegSetting("", "WptRegistered", szWptRegistered);
		RegisterExtension("Wpt","WireChanger Widget","-add=");
	}
	DEBUG_INFO3("App initialized ok...",0,0,0);
	return FALSE;
}

void AppMainDlg::Finalize()
{
	DEBUG_INFO3("Finalizing app...",0,0,0);
	static BOOL bFinalized=0;
	if(bFinalized){
		return;
	}
	bFinalized=1;
	ttDesktop.ShowToolTip(FALSE);
	SimpleLocker lc(&objSettings.csWPInChange);
	// Говорим всем что прога останавливается
	IsAppStopping()=1;
	DWORD dwTarget=BSM_APPLICATIONS;
	pMainDialogWindow->m_STray.SetTooltipText("Broadcasting...");
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(1), LPARAM(0));
	HideDeskIcons(0);
	::SetProp(::FindWindow("Progman",NULL),"WC_MAIN",(HANDLE)0);
	SetEvent(objSettings.hGlobalTimerEvent);
	iRunStatus=-1;
	// Сбрасываем поток, чтобы завершилось ожидание скринсейвера...
	SetEvent(hGlo_Copy);
	// Иконка...
	StartAnimation(-1);
	pMainDialogWindow->m_STray.SetTooltipText(_l("Closing options")+"...");
	// Закрываем окна
	if(pOpenedTemplate){
		CDLG_Options* pTmp=pOpenedTemplate;
		pOpenedTemplate=0;
		pTmp->SendMessage(WM_COMMAND,IDCANCEL,0);
	}
	if(dlgWChanger){
		pMainDialogWindow->m_STray.SetTooltipText("Closing main window...");
		CDLG_WChanger* pTmp=dlgWChanger;
		dlgWChanger=NULL;
		pTmp->Apply(TRUE);
		pTmp->DestroyWindow();
	}
	// Сохраняем все...
	pMainDialogWindow->m_STray.SetTooltipText(_l("Saving")+"...");
	objSettings.SaveAll();
	pMainDialogWindow->m_STray.SetTooltipText("Finalizing...");
	objSettings.Finalize();
	objSettings.bSaveAllowed=0;
	// До SaveAll pMainDialogWindow должен быть нормальным!
	pMainDialogWindow->m_STray.SetTooltipText(_l("All done")+"!");
	pMainDialogWindow=NULL;
	DestroyWindow();
	if(GlobalFindAtom("WPLABSDEBUG-WC")){
		GlobalDeleteAtom(GlobalFindAtom("WPLABSDEBUG-WC"));
	}
}

void AppMainDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	return;
}


void AppMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID==SC_MINIMIZE || nID==SC_CLOSE){
		ShowWindow(SW_HIDE);
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void RestoreWP(CString& sIniFileInfo,BOOL bWithScr);
DWORD WINAPI SetNextWallPaper_InThread(LPVOID);
void DisableUnsecured(BOOL bWithClocks)
{
	objSettings.lNoExitWarning=TRUE;
	BOOL bQSetWP=0;
	if(objSettings.lDisableNotesBeforeExit){
		bQSetWP=1;
	}
	if(bQSetWP){
		if(objSettings.lDisableNotesBeforeExit==2){
			pMainDialogWindow->m_STray.SetTooltipText("Restoring wallpaper (full)...");
			CString sIniFileInfo;
			ReadFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
			RestoreWP(sIniFileInfo,0);
			//SetWallPaper(objSettings.sBMPFileName, objSettings.iLastStretchingFromUser, "User defined");
		}else{
			objSettings.lConfModeIsON=2;
			pMainDialogWindow->OrderModifierForNextWallpaper=-3;
			pMainDialogWindow->m_STray.SetTooltipText("Restoring wallpaper...");
			SetNextWallPaper_InThread(0);
			objSettings.lConfModeIsON=0;
		}
	}
}

static long lLockExit=0;
static long lLockExit2=0;
void AppMainDlg::OnSystrayExit() 
{
	if(lLockExit>0){
		return;
	}
	if(lLockExit2>0){
		AfxMessageBox(_l("Close Calendar first!"));
		return;
	}

	SimpleTracker Track(lLockExit);
#ifndef _DEBUG
	if(!objSettings.lNoExitWarning && objSettings.bExitAlert==0 && AfxMessageBox(
#ifdef ART_VERSION
			_l("If you close this application, several features will not work")+":"+
			"\n- "+_l("Your wallpaper will not cycle automatically")+"."+
			"\n- "+_l("You will not be able to edit your desktop notes")+"."+
			"\n"+
			"\n"+_l("Are you sure you wish to close application")+"?",MB_YESNO|MB_ICONQUESTION|MB_TOPMOST)!=IDYES){
#else
			_l("If the "+AppName()+" tray application is closed, several features will not work")+":"+
			"\n"+
			"\n- "+_l("Your wallpaper will not cycle automatically (and the calendar will not change)")+"."+
			"\n- "+_l("You will not be able to edit your desktop notes")+"."+
			"\n- "+_l("The quick start keys will not be active")+"."+
			"\n"+
			"\n"+_l("Are you sure you wish to close the "+AppName()+" tray application")+"?",MB_YESNO|MB_ICONQUESTION|MB_TOPMOST)!=IDYES){
#endif
		return;
	}
#endif
	objSettings.bBlockAlerts=1;
	pMainDialogWindow->m_STray.SetTooltipText("Main window: close...");
	// Закрываем окна
	if(dlgWChanger){
		// Для пущей безопасности...
		CDLG_WChanger* pTmp=dlgWChanger;
		int iRes=pTmp->Apply(TRUE);
		if(iRes==0){
			AfxMessageBox(_l("Main window can`t be closed now, please wait"));
			return;
		}
		pMainDialogWindow->m_STray.SetTooltipText("Main window: destroy...");
		pTmp->DestroyWindow();
		dlgWChanger=NULL;
	}
	pMainDialogWindow->m_STray.SetTooltipText("Disabling usecured...");
	DisableUnsecured(1);
	pMainDialogWindow->m_STray.SetTooltipText("Finalizing...");
	Finalize();
}

void AppMainDlg::OnSystrayToMain(int iWL)
{
	if(iRunStatus!=1){
		return;
	}
	objSettings.lDefMWTab=iWL;
	if(dlgWChanger){
		dlgWChanger->ShowWindow(SW_SHOW);
		SwitchToWindow(dlgWChanger->GetSafeHwnd());
		if(iWL){
			dlgWChanger->On1Tab();
		}else{
			dlgWChanger->On0Tab();
		}
		return;
	};
	SetForegroundWindow();
	int iCount=100;
	while(objSettings.lMainWindowOpened && iCount>0){
		Sleep(500);
		iCount--;
	}
	dlgWChanger=new CDLG_WChanger(GetDesktopWindow());//this
	dlgWChanger->Create(CDLG_WChanger::IDD,GetDesktopWindow());//this
}

void AppMainDlg::OnSystrayOpen2()
{
	SimpleTracker Track(lLockExit2);
#ifdef ART_VERSION
	OnTemplParams(0,"ArtInfo","");
	return;
#endif
	//OnSystrayToMain(1);
	if(objSettings.hRemin){
		_OpenCalendar fp=(_OpenCalendar)GetProcAddress(objSettings.hRemin,"OpenCalendar");
		if(fp){
			(*fp)(this,(objSettings.bDefViewCalendar?1:0)|(objSettings.bDefViewWNums?2:0));
		}
	}
}

void AppMainDlg::OnSystrayOpen()
{
	OnSystrayToMain(0);
}

void AppMainDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

BOOL AppMainDlg::ShowDescTooltip(const char* szText)
{
	if(!szText){
		ttDesktop.ShowToolTip(FALSE);
		return FALSE;
	}
	CPoint pt;
	CString sTooltip=szText;
	sTooltip.TrimLeft();
	sTooltip.TrimRight();
	sTooltip.Replace("\r\n","\n");
	while(sTooltip.Replace("\n\n","\n")){};
	sTooltip.Replace("\n","\r\n");
	sTooltip.Replace(DEF_HIDBR,"\r\n");
	static CPoint pPrevPosition(0,0);
	if(!::IsWindowVisible(HWND(ttDesktop)) && (abs(pPrevPosition.x-pt.x)>5 || abs(pPrevPosition.y-pt.y)>5)){
		GetCursorPos(&pt);
		ttDesktop.SetToolTipPosition(pt.x,pt.y);
		ttDesktop.UpdateToolTipText(sTooltip);
		HWND hCurActiveWindow=::WindowFromPoint(pt);
		static char z[100]={0};
		GetClassName(hCurActiveWindow,z,100);
		BOOL bOkByClass=(stricmp(z,"SHELLDLL_DefView")==0 || stricmp(z,"internet explorer_server")==0 || stricmp(z,"syslistview32")==0 || stricmp(z,"deskmover")==0);
		if(!bOkByClass){
			return 0;
		}
		ttDesktop.ShowToolTip(TRUE);
		pPrevPosition=pt;
		//static long iii=1;
		//Alert(Format("!!! %i",iii++));
		BOOL bNeedToMB=0;
		CRect rt;
		::GetWindowRect(HWND(ttDesktop),&rt);
		CRect rDesktopRECT=GetAllMonitorsRECT();
		if(rt.Width()>rDesktopRECT.Width()-50){
			rt.right=rDesktopRECT.right-50;
			bNeedToMB=1;
		}
		if(rt.Height()>rDesktopRECT.Height()-50){
			rt.bottom=rDesktopRECT.bottom-50;
			bNeedToMB=1;
		}
		if(!bNeedToMB){
			if(rt.right>rDesktopRECT.right){
				rt.OffsetRect(-(rt.right-rDesktopRECT.right+5),0);
				bNeedToMB=1;
			}
			if(rt.bottom>rDesktopRECT.bottom){
				rt.OffsetRect(0,-(rt.bottom-rDesktopRECT.bottom+5));
				bNeedToMB=1;
			}
		}
		::MoveWindow(HWND(ttDesktop),rt.left+1,rt.top-1,rt.Width(),rt.Height(),TRUE);
	}
	return TRUE;
}

DWORD WINAPI AskTutorial(LPVOID);
BOOL AppMainDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(lParam==0 && wParam==2){
		ShowWindow(SW_HIDE);
		return TRUE;
	}
	if(wParam==ID_SYSTRAY_TUTO){
		FORK(AskTutorial,0);
		return TRUE;
	}
	if(wParam==ID_SYSTRAY_CHECKFORUPDATES){
		GetVersionChecker().SetTraits(Format("(Manual), chs=%i, strs=%i",objSettings.lStartsCount,objSettings.lChangesCount), IDI_UPNOTIFY);
		StartCheckNewVersion(0);
		return TRUE;
	}
	if(wParam>=WM_USER+CHANGE_TEMPLATE && wParam<(DWORD)(WM_USER+CHANGE_TEMPLATE+USERMSG_STEP)){
		int iIndex=wParam-WM_USER-CHANGE_TEMPLATE;
		objSettings.bTemplatesInWorkChanged=1;
		if(iIndex>=0 && iIndex<objSettings.aLoadedWPTs.GetSize()){
			SwitchSingleTemplate(objSettings.aLoadedWPTs[iIndex]);
		}else{
			objSettings.TemplatesInWork.RemoveAll();
		}
		// Обновляем обоину
		OrderModifierForNextWallpaper=-2;
		PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
		return TRUE;
	}
	if(wParam>=WM_USER+ACTIVATE_PRESET && wParam<(DWORD)(WM_USER+ACTIVATE_PRESET+USERMSG_STEP)){
		objSettings.SaveActiveWPT();
		CStringArray aPres;
		GetPresetList(aPres);
		objSettings.sActivePreset="";
		int iIndex=wParam-WM_USER-ACTIVATE_PRESET;
		if(iIndex>0 && iIndex<aPres.GetSize()){
			objSettings.sActivePreset=aPres[iIndex];
		}
		if(iIndex==0){
			objSettings.sActivePreset="";
		}
		objSettings.LoadActiveWPT();
		if(pMainDialogWindow->dlgWChanger){
			pMainDialogWindow->dlgWChanger->PostMessage(WM_COMMAND,WM_USER+REFRTEMPL,LPARAM(-1));
			pMainDialogWindow->dlgWChanger->RefreshNotesMenu();
		}
		return TRUE;
	}
	if(wParam>=WM_USER+OPEN_NOTE && wParam<(DWORD)(WM_USER+OPEN_NOTE+USERMSG_STEP)){
		int iIndex=wParam-WM_USER-OPEN_NOTE;
		if(iIndex<0 || iIndex>=objSettings.aDesktopNotes.GetSize()){
			return TRUE;
		}
		CNote& nt=objSettings.aDesktopNotes[iIndex];
		if(lParam){
			CString sPreFix,sPostFix;
			if(objSettings.hRemin){
				CString sRMKey=nt.GetReminderKey();
				_GetReminder fp=(_GetReminder)GetProcAddress(objSettings.hRemin,"GetReminder");
				if(fp){
					CWPReminder rem;
					strcpy(rem.szKey,sRMKey);
					if((*fp)(rem.szKey,rem)){
						char szTmp[128]={0};
						GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&rem.EventTime,0,szTmp,sizeof(szTmp));
						char szTmp2[128]={0};
						GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,&rem.EventTime,0,szTmp2,sizeof(szTmp2));
						sPreFix+=Format("%s %s"DEF_HIDBR,szTmp,szTmp2);
					}
				}
			}
			sPostFix+=_l("Double-click to edit");
			ShowDescTooltip(sPreFix+nt.sTitle+DEF_HIDBR+DEF_HIDBR+nt.GetRealTextFromNote()+DEF_HIDBR+sPostFix);
		}else{
			long lNoteUC=nt.lUniqueCounter;
			if(nt.bUnderEdit==0){
				CSmartLock lN(&objSettings.cNoteLock,TRUE,-1);
				CDLG_Note dlg;
				dlg.bForceFocus=TRUE;
				dlg.pNote=&nt;
				dlg.bNoBGImage=objSettings.aNoteSkin[objSettings.lNoteSkin].bIsHTML?TRUE:FALSE;
				int iRes=dlg.DoModal();
				BOOL bNeedRefresh=FALSE;
				{// Сохраняем/удаляем
					if(iRes==IDOK){
						nt.SaveToFile();
						bNeedRefresh=TRUE;
					}
					if(iRes==IDC_DEL){
						DWORD dwReply=AskYesNo(_l("Do you really want to delete this note")+"?",_l("Confirmation"),&objSettings.lDelNoteWarn,this);
						if(dwReply!=IDYES){
							objSettings.lDelNoteWarn=0;
						}else{
							for(int i=0;i<objSettings.aDesktopNotes.GetSize();i++){
								if(objSettings.aDesktopNotes[i].lUniqueCounter==lNoteUC){
									char szPath[MAX_PATH+2]="";
									memset(&szPath,0,sizeof(szPath));
									strcpy(szPath,GetNotesFolder()+nt.sNoteFile);
									if(isFileExist(szPath)){
										//DeleteFile(GetNotesFolder()+nt.sNoteFile);
										SHFILEOPSTRUCT str;
										memset(&str,0,sizeof(str));
										str.hwnd=this->GetSafeHwnd();
										str.wFunc=FO_DELETE;
										str.pFrom=szPath;
										str.pTo=NULL;
										str.fFlags=FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT;
										if(SHFileOperation(&str)!=0){
											Alert(_l("Can`t delete")+" '"+TrimMessage(GetNotesFolder()+nt.sNoteFile,20,3)+"'");
										}
									}
									objSettings.aDesktopNotes.RemoveAt(i);
									break;
								}
							}
							bNeedRefresh=TRUE;
						}
					}
				}
				if(pMainDialogWindow && bNeedRefresh){
					OrderModifierForNextWallpaper=-2;
					PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
					if(pMainDialogWindow->dlgWChanger){
						pMainDialogWindow->dlgWChanger->RefreshNotesMenu();
					}
				}
			}
		}
		return TRUE;
	}
	if(wParam>=WM_USER+CHANGE_THEME && wParam<(DWORD)(WM_USER+CHANGE_THEME+USERMSG_STEP)){
		static long lChangeThemeLock=0;
		if(lChangeThemeLock==0){
			SimpleTracker Track(lChangeThemeLock);
			int iIndex=wParam-WM_USER-CHANGE_THEME;
			if(iIndex<objSettings.m_WChanger.aUsedThemes.GetSize()){
				WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[objSettings.m_WChanger.aUsedThemes[iIndex]];
				if(wp){
					wp->SetStatus(!wp->dwStatus);
					objSettings.m_WChanger.ReinitWPaperShuffle();
					objSettings.m_WChanger.Save();
				}
			}
		}
		return TRUE;
	}
	if(wParam>=WM_USER+CHANGE_TEMPL_PAR && wParam<(DWORD)(WM_USER+CHANGE_TEMPL_PAR+USERMSG_STEP)){
		static long lThemeParLock=0;
		if(lThemeParLock==0){
			SimpleTracker Track(lThemeParLock);
			CString sStartTopic="",sFile="";
			int iIndexRaw=wParam-WM_USER-CHANGE_TEMPL_PAR;
			int iIndex=0;
			int iActiveWpt=0;
			if(lParam==255){
				m_STray.ShowIcon();
				OnTemplParams(0,objSettings.aLoadedWPTs[iIndexRaw],"");
				return TRUE;
			}else if(lParam==254){
				FORK(CallTemplatePositionDialog_InThread2,GetWPT(iIndexRaw));
				return TRUE;
			}else{
				iIndex=iIndexRaw%MAXROOTS_PERTEMPL;
				iActiveWpt=(iIndexRaw-iIndex)/MAXROOTS_PERTEMPL;
			}
			CWPT* wpt=GetWPT(iActiveWpt);
			if(wpt && iActiveWpt>=0 && iIndex>=0 && iIndex<wpt->aLoadedWPTsActionsDsc.GetSize()){
				sStartTopic=wpt->aLoadedWPTsActionsDsc[iIndex];
				sFile=wpt->aLoadedWPTsActionsFile[iIndex];
			}
			if(sFile!=""){
				CallFileForOpenOrEdit(sFile);
			}else{
				m_STray.ShowIcon();
				OnTemplParams(0,objSettings.aLoadedWPTs[iActiveWpt],_l(sStartTopic));
			}
		}
		return TRUE;
	}
	return CDialog::OnCommand(wParam, lParam);
}

void AppMainDlg::OnSystrayOptionsKey() 
{
	m_STray.ShowIcon();
	objSettings.OpenOptionsDialog(_l(REGISTRATION_TOPIC));
}

void AppMainDlg::OnSystrayOptions() 
{
	m_STray.ShowIcon();
	objSettings.OpenOptionsDialog();
}

void AppMainDlg::RegisterMyHotKeys()
{
	//Регистрируем горячие клавиши
	int i=0;
	BOOL bRes=TRUE;
	CString sFailed="";
	// Default action map
	for(i=0;i<(sizeof(objSettings.DefActionMapHK)/sizeof(objSettings.DefActionMapHK[0]));i++){
		if(objSettings.DefActionMapHK[i].Present()){
			BOOL bReg=RegisterIHotKey(this->m_hWnd,100+i,objSettings.DefActionMapHK[i],Format("MAIN%i",i));
			if(!bReg){
				sFailed+=GetIHotkeyName(objSettings.DefActionMapHK[i]);
				sFailed+=", ";
			}
			bRes&=bReg;
		}
	}
	if(bRes!=TRUE){
		sFailed.TrimRight(", ");
		Alert(_l("Some hotkeys can`t be registered!\nMaybe they are used by other application\nFailed to register:")+" "+sFailed,_l(AppName()+": Hotkeys"));
	}

}

void AppMainDlg::UnRegisterMyHotKeys()
{
	int i=0;
	//Разрегистрируем горячие клавиши
	for(i=0;i<10;i++){
		UnregisterHotKey(this->m_hWnd,i);
	}
	for(i=100;i<=(100+(sizeof(objSettings.DefActionMapHK)/sizeof(objSettings.DefActionMapHK[0])));i++){
		UnregisterHotKey(this->m_hWnd,i);
	}
}

BOOL checkFirstStr(const char* szIn, const char* szCharSet){
	int len=strlen(szIn)-1,k=strlen(szCharSet);
	if(len<=0 || len<k){
		return FALSE;
	}
	BOOL bFlag=TRUE;
	for(int j=0;j<k;j++){
		if(*(szIn+j)!=*(szCharSet+j)){
			bFlag=FALSE;
			break;
		}
	}
	return bFlag;
};

afx_msg LRESULT AppMainDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	CString sNewTitle;
	GetActiveWindowTitle(sNewTitle, objSettings.ActiveWndRect);
	int iHotKey=((DWORD)wParam)&CODE_NOPREFIX;
	wParam=iHotKey;
	if(iHotKey>=100 && iHotKey<=(100+MAX_HOTACTIONS)){
		PerformTrayAction(iHotKey-100, TRUE);
		return 0;
	}
	Alert("Error hotkey code!");
	return 0;
}

BOOL bTrayActionLock=FALSE;
void AppMainDlg::PerformTrayAction(int iAction,BOOL bHotKey)
{
	if(bTrayActionLock){
		return;
	}
	bTrayActionLock=TRUE;
	switch(iAction){
	case NEW_NOTE:
		PostMessage(WM_COMMAND,ID_SYSTRAY_NEWDESKTOPNOTE,0);
		break;
	case BRING_INTOVIEW:
		PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN,0);
		break;
	case NEXT_WALLPAPER:
		PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
		break;
	case ORDER_SAME:
		PostMessage(WM_COMMAND,ID_WCHANGER_REFRESHWP,0);
		break;
	case OPEN_CALEN:
		PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN2,0);
		break;
	case ORDER_NEXT:
		PostMessage(WM_COMMAND,ID_SYSTRAY_NEXTWALLP,0);
		break;
	case ORDER_PREV:
		PostMessage(WM_COMMAND,ID_SYSTRAY_PREVWALLP,0);
		break;
	case HIDE_ICONS:
		PostMessage(WM_COMMAND,ID_SYSTRAY_HIDESHOWICONS,0);
		break;
	case HIDE_WIDGETS:
		PostMessage(WM_COMMAND,ID_SYSTRAY_DISABLEWIDGETST,0);
		break;
	default:
		AddError("No tray action!",TRUE);
	}
	bTrayActionLock=FALSE;
}

void DisableElefun()
{
	HWND progman_window = FindWindow("Progman", 0);
	HWND view_window = FindWindowEx(progman_window, 0, "SHELLDLL_DefView", 0);
	HWND wallpaper_window = FindWindowEx(view_window, 0, "EleFunAnimatedWallpaper", 0);
	if (wallpaper_window){
		CRegKey key;
		if(key.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")==ERROR_SUCCESS){
			key.DeleteValue("EleFunAnimatedWallpaper");
		}
		for (int attempts_count = 10; attempts_count > 0; attempts_count--)
		{
			PostMessage(wallpaper_window, WM_CLOSE, 0, 0);
			Sleep(30);
			if (!(wallpaper_window = FindWindowEx(view_window, 0, "EleFunAnimatedWallpaper", 0))){
				break;
			}
			
		}
		// Пусть ресурсы все выгрузит
		Sleep(1500);
	}
}

//#define _LOGWSET
#ifdef _LOGWSET
long lNumber=0;
CString sWCatchDebug;
#endif
//HKEY_CURRENT_USER
const char* szKey1="Control Panel\\Desktop";
const char* szKey2="Software\\Microsoft\\Internet Explorer\\Desktop\\General";
DWORD WINAPI TrackWallpaperChange(LPVOID pType)
{
	CoInitialize(NULL);
	const char* sKey=szKey1;
	DWORD dwMessage=DESKTOP_WALLC;
	if(pType!=0){
		sKey=szKey2;
		dwMessage=DESKTOP_WALLC2;
	}
	CRegKey key;
	DWORD lSize = MAX_PATH,dwType=0;
	key.Open(HKEY_CURRENT_USER, sKey);
	while(pMainDialogWindow!=NULL && key.m_hKey!=NULL){
		if(pMainDialogWindow->iRunStatus!=1){
			Sleep(500);
			continue;
		}
		char szWallpaperPathBefore[MAX_PATH]="";
		strcpy(szWallpaperPathBefore,GetRegString(key.m_hKey,"Wallpaper"));
		DoEnvironmentSubst(szWallpaperPathBefore,sizeof(szWallpaperPathBefore));
		CFileInfo wpInfo;
		wpInfo.Create(szWallpaperPathBefore);
		DWORD dwWallpaperPathBeforeCRC=wpInfo.GetChecksum();
#ifdef _DEBUG
		time_t t1=time(0);
		CString s=Format("%s %i:BeforeChange %s - %s:%lu",asctime(gmtime(&t1)),GetCurrentThreadId(),sKey,szWallpaperPathBefore,dwWallpaperPathBeforeCRC);
		FLOG(s);
#endif
		LONG lRes=RegNotifyChangeKeyValue(key.m_hKey,FALSE,REG_NOTIFY_CHANGE_LAST_SET,NULL,FALSE);
		// Если это изменилась обычная обоина, ждем полсекунды и если к тому моменту поменялась и active-desktop
		// то пропускаем только active desktop
		BOOL bSkipThisSignal=FALSE;
		static long lParallelTrigger=0;
		if(dwMessage==DESKTOP_WALLC){
			long lRemember=lParallelTrigger;
			Sleep(1500);
			if(lRemember!=lParallelTrigger){
				bSkipThisSignal=TRUE;
			}
		}else{
			::InterlockedIncrement(&lParallelTrigger);
		}
		if(pMainDialogWindow && !bSkipThisSignal){
			CFileInfo wpInfoAfter;
			RefreshWallpaperFileName();
			char szWallpaperPath[MAX_PATH]="";
			strcpy(szWallpaperPath,GetRegString(key.m_hKey,"Wallpaper"));
			DoEnvironmentSubst(szWallpaperPath,sizeof(szWallpaperPath));
			wpInfoAfter.Create(szWallpaperPath);
			DWORD dwWallpaperPathAfterCRC=wpInfoAfter.GetChecksum();
#ifdef _DEBUG
		time_t t2=time(0);
		CString s=Format("%s %i:AfterChange %s - %s:%lu",asctime(gmtime(&t2)),GetCurrentThreadId(),sKey,szWallpaperPath,dwWallpaperPathAfterCRC);
		FLOG(s);
#endif
			if(strlen(szWallpaperPath)>0 && (strcmp(szWallpaperPathBefore,szWallpaperPath)!=0 || dwWallpaperPathBeforeCRC!=dwWallpaperPathAfterCRC)){
				CString szTile=GetRegString(key.m_hKey,"TileWallpaper");
				// Если обоина из системного каталога - вычисляем прямой путь
				BOOL bThisIsNewWallpaper=FALSE;
				RefreshWallpaperFileName();
				if(objSettings.sBMPFileName!=szWallpaperPath && objSettings.sHTMLWrapperFile!=szWallpaperPath){
					// Смотрим, не элефан ли тут постарался...
					CRegKey key;
					CString sWPName=GetPathPart(szWallpaperPath,0,0,1,0);
					key.Open(HKEY_LOCAL_MACHINE, CString("Software\\EleFun Multimedia\\")+sWPName+" Wallpaper");
					if(key!=NULL)
					//CString sWPPath=GetPathPart(szWallpaperPath,1,1,0,0);
					//if(isFileExist(sWPPath+"mute.swf"))
					{
						// Элефант!
						static DWORD dwLastWaarn=0;
						if(dwLastWaarn==0 || GetTickCount()-dwLastWaarn>10000){
							dwLastWaarn=GetTickCount();
							// Включена ли шаблона?
							DWORD dwReply=IDNO;
							if(!IsWPTActive("AnimatedWp")){
								dwReply=AskYesNo(_l("Elefun animated wallpaper detected\nIt is recommended to activate 'Elefun wallpapers' template\nto utilize all "+AppName()+"`s features with animated background")+"\n"+_l("Do you wish to enable animated wallpaper support right now")+"?",_l("Elefun wallpaper"),0,pMainDialogWindow?(pMainDialogWindow->dlgWChanger?(CWnd*)pMainDialogWindow->dlgWChanger:(CWnd*)pMainDialogWindow):NULL);
								if(dwReply==IDYES){
									SwitchSingleTemplate("AnimatedWp");
								}
							}else{
								dwReply=AskYesNo(_l("Elefun animated wallpaper detected\nIt is recommended to activate 'Elefun wallpapers' template\nto utilize all "+AppName()+"`s features with animated background")+"\n"+_l("Do you wish to set animated wallpaper parameters right now")+"?",_l("Elefun wallpaper"),0,pMainDialogWindow?(pMainDialogWindow->dlgWChanger?(CWnd*)pMainDialogWindow->dlgWChanger:(CWnd*)pMainDialogWindow):NULL);
							}
							if(pMainDialogWindow && dwReply==IDYES){
								// Выключаем его...
								DisableElefun();
								/*
								if(IsWPTActive("Special Effects")){
									SwitchSingleTemplate("Special Effects");
								}
								if(IsWPTActive("Picture in Picture")){
									SwitchSingleTemplate("Picture in Picture");
								}
								if(IsWPTActive("Image morthing")){
									SwitchSingleTemplate("Image morthing");
								}
								Alert(_l("Warning: You should restart computer to apply changes"));
								*/
								int iWptIndex=GetTemplateMainIndex("AnimatedWp");
								if(iWptIndex!=-1){
									pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+CHANGE_TEMPL_PAR+iWptIndex,255);
								}
							}
						}
						dwLastWaarn=GetTickCount();
						objSettings.noDesktopHookProcessing++;
					}else{
						objSettings.iTile=atol(szTile);
						objSettings.iLastStretchingFromUser=objSettings.iTile?-1:-2;
						objSettings.sLastWallpaperFromUser=szWallpaperPath;
						bThisIsNewWallpaper=TRUE;
					}
				}
				::PostMessage(pMainDialogWindow->GetSafeHwnd(),dwMessage,0,bThisIsNewWallpaper);
			}
		}else if(!pMainDialogWindow){
			#ifdef _DEBUG
			time_t t2=time(0);
			CString s=Format("%s %i:Break!",asctime(gmtime(&t2)),GetCurrentThreadId());
			FLOG(s);
			#endif
			break;
		}
	}
	#ifdef _DEBUG
			time_t t2=time(0);
			CString s=Format("%s %i:Exit!",asctime(gmtime(&t2)),GetCurrentThreadId());
			FLOG(s);
	#endif
	CoUninitialize();
	return 0;
}

BOOL AppMainDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==273 && pMsg->wParam==32983 && pMsg->lParam==1){
		RestartWC();
		return TRUE;
	}
	BOOL bNeedSetTransIcons=FALSE;
	if(pMsg->message==DESKTOP_WALLC || pMsg->message==DESKTOP_WALLC2){
		static long lDWPLock=0;
		SimpleTracker Track(lDWPLock);
		if(lDWPLock==1){
			bNeedSetTransIcons=TRUE;
			CString sNewWPFile=objSettings.sLastWallpaperFromUser;//GetCurrentWallpaper(&bActiveDesktopOn);
			// Смотрим что за обои установили
			if(sNewWPFile!=""){
				if(objSettings.lTrackWallpaperChanges){
					RefreshWallpaperFileName();
					if(sNewWPFile!=objSettings.sBMPFileName && objSettings.sBMPFileName!="" && pMsg->lParam!=0){
						// Фас эту обоину!!!
						bNeedSetTransIcons=FALSE;
						if(objSettings.lAutoAddExternalWP<2){
							CString sNewPath=getMyPicPath();
							if(sNewPath!="" && sNewPath.Right(1)!="\\"){
								sNewPath+="\\";
							}
							CreateDirIfNotExist(sNewPath);
							sNewPath+=WP_WALLP;
							sNewPath+="\\";
							CreateDirIfNotExist(sNewPath);
							static int iNum=1;
							sNewPath+=Format("Wallpaper_%lu_%i",objSettings.lStartsCount,iNum++);
							if(!objSettings.lTrackWallpaperSaveJpg){
								sNewPath+=GetPathPart(sNewWPFile,0,0,0,1);
							}else{
								sNewPath+=".jpg";
							}
							DWORD dwReply=AskYesNo(_l("Do you want to add this image to your collection")+"?",_l("New wallpaper"),&objSettings.lAutoAddExternalWP,NULL);
							if(pMainDialogWindow && dwReply==IDYES){
								if(!objSettings.lTrackWallpaperSaveJpg || !getILoad().ConvertToJpg(sNewWPFile, sNewPath, objSettings.lTrackWallpaperSaveJpgQ)){
									CopyFile(sNewWPFile,sNewPath,FALSE);
								}
								objSettings.sAddThisFileAfterOpenWPList=sNewPath;
								::PostMessage(pMainDialogWindow->GetSafeHwnd(),WM_COMMAND,ID_SYSTRAY_OPEN,0);
							}
						}
						SimpleTracker Track(objSettings.lSkipMorthOnGeneration);
						if(ReKolbassWallpaper(sNewWPFile,objSettings.iTile)){
							bNeedSetTransIcons=TRUE;
						}
					}else{
						bNeedSetTransIcons=TRUE;
					}
				}
			}
		}
		return TRUE;
	}
	if(pMsg->message==WM_SETTINGCHANGE || pMsg->message==WM_WININICHANGE || pMsg->message==DESKTOP_REFRESH){
		bNeedSetTransIcons=TRUE;
	}
	if(bNeedSetTransIcons){
		ApplyDeskIconsOptions();
	}
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE){
		ShowWindow(SW_HIDE);
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

DWORD dwLookForStop=0;
CPoint pStopPoint(0,0);
long lMouseStopTracker=0;
DWORD WINAPI TrackMoustStopped(LPVOID p)
{
	if(lMouseStopTracker>0){
		return 0;
	}
	SimpleTracker rt(lMouseStopTracker);
	HWND hTarg=(HWND)p;
	if(!hTarg){
		return 0;
	}
	DWORD dwMinTime=1500;
	SystemParametersInfo(SPI_GETMOUSEHOVERTIME,0,&dwMinTime,0);
	while(pMainDialogWindow){
		Sleep(1000);
		DWORD dwElapsed=GetTickCount()-dwLookForStop;
		if(dwElapsed>3000){
			dwLookForStop=0;
		}
		if(dwLookForStop!=0 && dwElapsed>dwMinTime){
			CPoint ptThis;
			GetCursorPos(&ptThis);
			if(abs(ptThis.x-pStopPoint.x)<5 && abs(ptThis.y-pStopPoint.y)<5){
				::PostMessage(hTarg,DESKTOP_MOVEEX,0,0);
				dwLookForStop=0;
			}
		}
	}
	return 0;
}

BOOL bEnableDragWidgetMode=0;
CPoint pPrevEnableDragWidgetMode;

void DropDragWidgetMode()
{
	bEnableDragWidgetMode=0;
	pPrevEnableDragWidgetMode.x=pPrevEnableDragWidgetMode.y=0;
}

extern long bDragWidgetMode;
extern CCriticalSection csActZones;
CWPT* AppMainDlg::FindWPTUnderPoint(CPoint pt)
{
	// Ищем Widget для смены позиции/настроек
	for(int i=0;i<objSettings.aLoadedWPTs.GetSize();i++){
		if(!IsWPTActive(objSettings.aLoadedWPTs[i])){
			continue;
		}
		CWPT* wpt=GetWPT(i);
		if(wpt){
			CRect rtZone;
			if(CallTemplatePositionDialog(wpt,&rtZone,1)==0){
				// Уже в процессе, не даем ничего делать пока не закроют окошко!
				return 0;
			}
			if(bEnableDragWidgetMode>0 && wpt->bScrollBarsAware){
				rtZone.right-=GetSystemMetrics(SM_CXVSCROLL)-5;
				rtZone.left+=GetSystemMetrics(SM_CXVSCROLL)+5;
				rtZone.bottom-=GetSystemMetrics(SM_CYHSCROLL)-5;
				rtZone.top+=GetSystemMetrics(SM_CYHSCROLL)+5;
			}
			AdjustToSceenCoords(rtZone);
			if(pt.x>rtZone.left && pt.y>rtZone.top && pt.x<rtZone.right && pt.y<rtZone.bottom){
				return wpt;
			}
		}
	}
	return 0;
}

LRESULT AppMainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_COPYDATA){
		if(wParam==1){
			g_sPathToSaveBmp=(const char*)(((COPYDATASTRUCT*)lParam)->lpData);
			FORK(GenerateWPInThread,this);
			return TRUE;
		}
	}
	if(message==OPEN_TEMPL_PARS){
		char* sAlert=(char*)wParam;
		pMainDialogWindow->OnTemplParams(0,sAlert,"");
		free(sAlert);
		return TRUE;
	}
	if(message==WM_DISPLAYCHANGE){
		if(objSettings.lRestoreOnResChange){
			CSmartWndSizer::GetScreenRect(0,0)=CRect(0,0,0,0);
			pMainDialogWindow->OrderModifierForNextWallpaper=-3;
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
		}
	}
	if(message==DESKTOP_CALLONDT){
		const char* sz=(const char*)wParam;
		CString sExecuteLine=sz;
		free(LPVOID(sz));
		CallFileForOpenOrEdit(sExecuteLine,0);
		//::SendMessage(pMainDialogWindow->GetSafeHwnd(),,strdup(sExecuteLine),0);
		return TRUE;
	}
	if(bDragWidgetMode==0 && !objSettings.noDesktopHookProcessing){
		CPoint pt;
		::GetCursorPos(&pt);
		if(message==DESKTOP_SCLICKU){
			LOGSTAMP1("DESKTOP_SCLICKU");
			if(bEnableDragWidgetMode){
				DropDragWidgetMode();
				message=0;
			}
			// Далековато уехали...
			if(abs(pt.x-pPrevEnableDragWidgetMode.x)>=25 || abs(pt.y-pPrevEnableDragWidgetMode.y)>=25){
				message=0;
			}
		}
		static DWORD dwClickTime=GetTickCount()-100000;
		if(message==DESKTOP_SCLICKD){
			LOGSTAMP1("DESKTOP_SCLICKD");
			dwClickTime=GetTickCount();
			DropDragWidgetMode();
			GetCursorPos(&pPrevEnableDragWidgetMode);
		}
		if(message==DESKTOP_DBLC){
			DropDragWidgetMode();
			GetCursorPos(&pPrevEnableDragWidgetMode);
			dwClickTime=GetTickCount();
			LOGSTAMP1("DESKTOP_DBLC");
		}
		UINT iRightMessage=(objSettings.lActOnDblClick==0?0:(objSettings.lActOnDblClick==2?DESKTOP_DBLC:DESKTOP_SCLICKU));
		if(pPrevEnableDragWidgetMode.x!=0 && pPrevEnableDragWidgetMode.y!=0){
			if(message==DESKTOP_MOVE && objSettings.lenableSimpleMoving==1 && (GetAsyncKeyState(VK_LBUTTON)<0)){
				CPoint ptThis;
				GetCursorPos(&ptThis);
				if((abs(ptThis.x-pPrevEnableDragWidgetMode.x)>=25|| abs(ptThis.y-pPrevEnableDragWidgetMode.y)>=25)){
					if(GetTickCount()-dwClickTime<2000){
						CWPT* wptUnderCur=FindWPTUnderPoint(ptThis);
						if(wptUnderCur==0/*Заметка?*/ || wptUnderCur->sHints.Find("IGNOREDBL")==-1){// Над таким драггить нельзя!!!
							dwClickTime=GetTickCount()-100000;
							// Пытаемся начать двигать!
							message=DESKTOP_SCMOVE;
							bEnableDragWidgetMode++;
							pt.x=pPrevEnableDragWidgetMode.x;
							pt.y=pPrevEnableDragWidgetMode.y;
						}
					}
				}
			}
		}
		if(objSettings.lDeskTooltips && message==DESKTOP_MOVE){
			BOOL bLimitedMode=0;
			if(objSettings.lDisableNotesBeforeExit && objSettings.noWallpaperChangesYet){
				bLimitedMode=1;
			}
			if(!bLimitedMode){
				CPoint ptThis;
				GetCursorPos(&ptThis);
				if(abs(ptThis.x-pStopPoint.x)>=5|| abs(ptThis.y-pStopPoint.y)>=5){
					ShowDescTooltip(NULL);
				}
				if(lMouseStopTracker==0){
					FORK(TrackMoustStopped,this->GetSafeHwnd());
				}
				pStopPoint.x=ptThis.x;
				pStopPoint.y=ptThis.y;
				dwLookForStop=GetTickCount();
			}
			return TRUE;
		}
		if((iRightMessage && message==iRightMessage) || message==DESKTOP_MOVEEX || message==DESKTOP_SCMOVE){
			CWPT* wptUnderCur=FindWPTUnderPoint(pt);
			if(message==iRightMessage && wptUnderCur && wptUnderCur->sHints.Find("IGNOREDBL")!=-1){
				// Ничего нельзя!!!
				message=0;
				lParam=1;
			}
			BOOL bDblClick=(message==DESKTOP_DBLC);
			BOOL bTooltip=0;
			BOOL bLimitedMode=0;
			if(objSettings.lDisableNotesBeforeExit && objSettings.noWallpaperChangesYet){
				bLimitedMode=1;
				LOGSTAMP;
			}
			if(message==DESKTOP_MOVEEX){
				bTooltip=1;
				LOGSTAMP("MOVEEX");
			}
			if(message==iRightMessage || message==DESKTOP_SCMOVE){
				ShowDescTooltip(NULL);
			}
			BOOL bValidDblClick=1;
			if(objSettings.lDefNoteModifier!=0 && !bTooltip){
				if(objSettings.lDefNoteModifier==1 && GetKeyState(VK_SHIFT)>=0){
					bValidDblClick=0;
				}
				if(objSettings.lDefNoteModifier==2 && GetKeyState(VK_CONTROL)>=0){
					bValidDblClick=0;
				}
				if(objSettings.lDefNoteModifier==3 && GetKeyState(VK_MENU)>=0){
					bValidDblClick=0;
				}
			}
			if(lParam==0){
				int i=0;
				BOOL bFindOK=FALSE;
				CSmartLock lN(&objSettings.cNoteLock,TRUE,-1);
#if	!defined(USE_LITE) && !defined(ART_VERSION)
				if(bValidDblClick && objSettings.aDesktopNotes.GetSize()>0 && !objSettings.lDisableNotes && !bLimitedMode){
					CSize sizeIcon=DEFNOTEICONSIZE;
					CSize sizeTitle=GetBitmapSize(_bmp().GetRaw(GetNSkinBmp(objSettings.lNoteSkin,"TITLE")));
					if(sizeTitle.cy==0 || sizeTitle.cx==0){
						sizeTitle=sizeIcon;
					}
					for(i=0;i<objSettings.aDesktopNotes.GetSize();i++){
						if(objSettings.aDesktopNotes[i].lShowMode==4){
							continue;
						}
						CRect rNoteRt;
						rNoteRt.left=objSettings.aDesktopNotes[i].left;
						rNoteRt.right=objSettings.aDesktopNotes[i].right;
						rNoteRt.top=objSettings.aDesktopNotes[i].top;
						rNoteRt.bottom=objSettings.aDesktopNotes[i].bottom;
						if(objSettings.aDesktopNotes[i].lShowMode==1){
							rNoteRt.bottom=rNoteRt.top+sizeTitle.cy;
						}
						if(objSettings.aDesktopNotes[i].lShowMode==2){
							rNoteRt.right=rNoteRt.left+sizeIcon.cx;
							rNoteRt.bottom=rNoteRt.top+sizeIcon.cy;
						}
						AdjustToSceenCoords(rNoteRt);
						if(pt.x>rNoteRt.left && pt.y>rNoteRt.top && pt.x<rNoteRt.right && pt.y<rNoteRt.bottom){
							PostMessage(WM_COMMAND,WM_USER+OPEN_NOTE+i,bTooltip);
							bFindOK=TRUE;
							LOGSTAMP;
							break;
						}
					}
				}
#endif
				if(!bLimitedMode && !bFindOK){
					CSmartLock lActZones(&csActZones,TRUE);
					BOOL bS=(GetAsyncKeyState(VK_SHIFT)<0) || (bEnableDragWidgetMode>0);
					BOOL bC=(GetAsyncKeyState(VK_CONTROL)<0);
					if(!bTooltip && (bS || bC)){
						if(wptUnderCur){
							int i=GetWPTIndex(wptUnderCur->sTemplName);
							if(bC){
								PostMessage(WM_COMMAND,WM_USER+CHANGE_TEMPL_PAR+i,255);
							}else if(bS){
								PostMessage(WM_COMMAND,WM_USER+CHANGE_TEMPL_PAR+i,254);
							}
							bFindOK=TRUE;
						}
					}else{
						// Сверяем активные зоны...
						for(i=objSettings.aActiveZonesRect.GetSize()-1;i>=0;i--){
							CRect rtZone=objSettings.aActiveZonesRect[i];
							AdjustToSceenCoords(rtZone);
							if(pt.x>rtZone.left && pt.y>rtZone.top && pt.x<rtZone.right && pt.y<rtZone.bottom){
								CallFileForOpenOrEdit(objSettings.aActiveZonesActions[i],bTooltip);
								bFindOK=TRUE;
								LOGSTAMP;
								break;
							}
						}
					}
				}
				LOGSTAMP;
				if(!bTooltip && message!=DESKTOP_SCMOVE){
					if(bFindOK==FALSE && !objSettings.lDisableNotes){
						BOOL bLockChange=(objSettings.lSetWallaperLock>0)?TRUE:FALSE;
						if(bLockChange){
							ShowBaloon(_l("Please wait: wallpaper image is changing"),_l("Warning"));
						}else if(bValidDblClick){
							LOGSTAMP1("NEW note");
							PostMessage(WM_COMMAND,ID_SYSTRAY_NEWDESKTOPNOTE,0);
						}
					}
				}
				if(!bFindOK){
					DropDragWidgetMode();
				}
			}
			return TRUE;
		}
	}
	if(message==WM_QUERYENDSESSION){
		DisableUnsecured(1);
		return TRUE;
	}
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	if(message==WM_HELP){
		ShowHelp("Overview");
		return 0;
	}
	if(message==WM_INITMENUPOPUP){
		bAnyPopupIsActive=TRUE;
	}
	if(message==WM_ACTIVATEAPP){
		if(wParam==FALSE){
			bAnyPopupIsActive=FALSE;
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


void AppMainDlg::OnEndSession(BOOL bEnding) 
{
	CDialog::OnEndSession(bEnding);
	if(bEnding){
		Finalize();
	}
}

int AppMainDlg::SetTrayTooltip(BOOL bDisableAuto)
{
	if(iRunStatus!=1){
		return 0;
	}
	CString sTooltip=objSettings.sTrayTooltip;
	if(objSettings.lSetWallaperLock && !bDisableAuto){
		sTooltip+="\n";
		if(objSettings.lSetWallaperLockDesc!=""){
			sTooltip+=objSettings.lSetWallaperLockDesc;
		}else{
			sTooltip+=_l("Generating new wallpaper");
		}
	}else if(objSettings.lChangePriodically){
		sTooltip+="\n";
		DWORD dwCurTick=GetTickCount();
		CString sInfoText="";
		long lPeriod=(objSettings.lChangePeriod*60*1000);
		long lTimeToNextChange=(objSettings.lLastTimeChange+lPeriod)-dwCurTick;
		if(lTimeToNextChange>=0){
			sInfoText=GetReadableStringFromMinutes(lTimeToNextChange/1000/60);
		}else{
			sInfoText="";
		}
		if(sInfoText==""){
			sTooltip+=_l("New wallpaper: coming soon")+"...";
		}else{
			sTooltip+=Format("%s %s",sInfoText,_l("till next change"));
		}
	}
	m_STray.SetTooltipText(sTooltip);
	return 0;
}

void AppMainDlg::StartAnimation(int iIconType)
{
	SetTrayTooltip();
	if(iIconType<0){
		m_STray.SetIcon(IDI_NOCHWALL);
		m_STray.ShowIcon();
		return;
	}
	if(objSettings.bChangeTrayIconOnWP){
		m_STray.SetIcon(iIconType?iIconType:IDI_CHWALLPICON);
		m_STray.ShowIcon();
	}
}

void AppMainDlg::StopAnimation(HBITMAP hBmpIcon)
{
	SetTrayTooltip(TRUE);
	if(hBmpIcon){
		CSize size(16,16);
		CBitmap bmpT;
		bmpT.Attach(hBmpIcon);
		HICON hIcon=CreateIconFromBitmap(&bmpT,&size);
		m_STray.SetIcon(hIcon);
		//ClearIcon(hIcon);
		bmpT.Detach();
	}else{
		m_STray.SetIcon(MAIN_ICON);
	}
}

void AppMainDlg::PostNcDestroy() 
{
	Finalize();
	CDialog::PostNcDestroy();
	delete this;
}

afx_msg LRESULT AppMainDlg::OnFireAlert(WPARAM wParam, LPARAM lParam)
{
	InfoWndStruct* info=(InfoWndStruct*)lParam;
	if(info->bAsModal){
		InfoWnd infoWnd(info,info->pParent?info->pParent:this);
		return infoWnd.DoModal();
	}else{
		InfoWnd* infoWnd=new InfoWnd(info);
		infoWnd->Create(InfoWnd::IDD,info->pParent?info->pParent:this);
		return LRESULT(infoWnd);
	}
}

void AppMainDlg::OnSystrayAbout() 
{	
#ifdef ART_VERSION
	ShowHelp("Overview");
	return;
#endif
	static CBitmap* bmLogo=_bmp().AddBmpFromDisk(CString(GetApplicationDir())+"logo.jpg",CSize(0,0),"LOGO");
	m_Logo.SetBitmap(bmLogo);
	SetTaskbarButton(this->GetSafeHwnd());
	m_STray.ShowIcon();
	SetRegInfo();
	ShowWindow(SW_SHOW);
}

void AppMainDlg::OnOK() 
{
	ShowWindow(SW_HIDE);
}

void AppMainDlg::OnEntcode() 
{
	if(objSettings.iLikStatus<2 || isTempKey()){
		PostMessage(WM_COMMAND,ID_SYSTRAY_REGISTER,0);
	}else{
		PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN,0);
	}
}

HANDLEINTHREAD(AppMainDlg,OnWebsite)
{
	if(pMainDialogWindow){
		if(objSettings.iLikStatus<2 || isTempKey()){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_GOTOWEBSITE,0);
		}else{
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_TELLAFRIEND,0);
		}
	}
	return 0;
}

afx_msg LRESULT AppMainDlg::ONWM_SSAVE(WPARAM wParam, LPARAM lParam)
{
	m_STray.ShowIcon();
	objSettings.OpenOptionsDialog(_l(SSAVE_TOPIC));
	return 0;
}

afx_msg LRESULT AppMainDlg::ONWM_THIS(WPARAM wParam, LPARAM lParam)
{
	if(wParam==99 && lParam==99){
		PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN,0);
		return 1;
	}
	return 0;
}

// wParam=0 - старт родительской проги
// wParam=1 - останов родительской проги
// wParam=2 - активация программы WireKeys
// wParam=3 - активация программы WireChanger
afx_msg LRESULT AppMainDlg::ONWM_WIRENOTE(WPARAM wParam, LPARAM lParam)
{
	if(wParam>3 || wParam==2){// Это не к нам...
		return 0;
	}
	if(wParam==3){
		OnSystrayOpen();
		return 0;
	}
	if(IsIconMustBeHided() && wParam!=1){
		m_STray.HideIcon();
	}else{
		m_STray.ShowIcon();
	}	
	return 0;
}

void AppMainDlg::OnSystrayHelp() 
{
	ShowHelp("Overview");
}

void AppMainDlg::SetTheSameWallPaper()
{
	OrderModifierForNextWallpaper=-2;
	PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
}

void AppMainDlg::OnOrderNextWallPaper()
{
	OrderModifierForNextWallpaper=1;
	PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
}

void AppMainDlg::OnOrderRandWallPaper()
{
	OrderModifierForNextWallpaper=0;
	PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
}

void AppMainDlg::OnOrderPrevWallPaper()
{
	OrderModifierForNextWallpaper=-1;
	PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
}

void AppMainDlg::OnSetChangePeriodically()
{
	objSettings.lChangePriodically=!objSettings.lChangePriodically;
	if(dlgWChanger){
		dlgWChanger->PostMessage(WM_COMMAND,ID_WCHANGER_REFRESHCYCLESTATUS,0);
	}
	SetTrayTooltip();
}

BOOL g_bMatchedWndFound=0;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char g_szSearchTitle2[1024]={0};
	::GetWindowText(hwnd,g_szSearchTitle2,sizeof(g_szSearchTitle2));
	if(!IsWindowVisible(hwnd)){
		return 1;
	}
	if(PatternMatchList(g_szSearchTitle2,objSettings.sStopTitles)){
		g_bMatchedWndFound=1;
		return 0;
	}
	return 1;
}

DWORD WINAPI SetNextWallPaper_InThread(LPVOID p)
{
	if(objSettings.lSetWallaperLock>0){
		return 0;
	}
	BOOL bTimelyChange=(BOOL)p;
	BOOL bBaloonWasOpened=0;
	static BOOL bWasDscShow=0;
	static long lNoTwice=0;
	if(lNoTwice>0){
		return 0;
	}
	CSmartLock lslock(objSettings.m_WChanger.cShufflingLock,TRUE);
	SimpleTracker lcl(lNoTwice);
	SimpleLocker lc(&objSettings.csWPInChange);
	DisableElefun();
	int iInitialDesiredOrder=pMainDialogWindow->OrderModifierForNextWallpaper;
	if(pMainDialogWindow && (!objSettings.lVeryFirstStart || bWasDscShow!=0)){
		if(pMainDialogWindow->OrderModifierForNextWallpaper==-2 && !objSettings.lApplyingChangesWarn){
			static DWORD dwFreqStart=0;
			if(GetTickCount()-dwFreqStart>(60*1000*5)){
				dwFreqStart=GetTickCount();
				pMainDialogWindow->ShowBaloon(_l("Applying changes and generating wallpaper"),_l("Please wait"));
				bBaloonWasOpened=1;
			}
		}
	}
	srand(time(NULL)+GetCurrentThreadId());
	objSettings.lSetWallaperLockDesc="";
	SimpleTracker Track(objSettings.lSetWallaperLock);
	BOOL bSkipChange=FALSE;
	if(!objSettings.lHideConfDataInLock && !(objSettings.sHideConfDataInLock2!="")){
		if(isScreenSaverActiveOrCLocked()){
			if(objSettings.bChangetWPIfScrSaver==0){
				bSkipChange=TRUE;
			}else if(objSettings.bChangetWPIfScrSaver==2 && g_lOnceScrPerventer>0){
				bSkipChange=TRUE;
			}
			g_lOnceScrPerventer++;
		}else{
			g_lOnceScrPerventer=0;
		}
	}
	if(bTimelyChange){
		if(!bSkipChange){
			if(objSettings.lChangeIfMemoE){
				MEMORYSTATUS memstat;
				memset(&memstat,0,sizeof(memstat));
				GlobalMemoryStatus(&memstat);
				double dPercent=100*double(memstat.dwAvailPhys)/double(memstat.dwTotalPhys);
				if(dPercent>objSettings.lChangeIfMemoP){
					bSkipChange=TRUE;
					if(pMainDialogWindow && objSettings.lShowPopupsOnErr){
						pMainDialogWindow->m_STray.PopupBaloonTooltip(_l(AppName()+": Warning"),Format("%s\n%s: %.2f%%",_l("Skipping wallpaper generation due"),_l("High Memory usage"),dPercent));
					}
				}
			}
		}
		if(!bSkipChange){
			if(objSettings.lChangeIfCPUE && !IsAppStopping()){
				double dPercent=GetCPUTimesPercents();
				if(dPercent>objSettings.lChangeIfCPUP){
					bSkipChange=TRUE;
					if(pMainDialogWindow && objSettings.lShowPopupsOnErr){
						pMainDialogWindow->m_STray.PopupBaloonTooltip(_l(AppName()+": Warning"),Format("%s\n%s: %.2f%%",_l("Skipping wallpaper generation due"),_l("High CPU usage"),dPercent));
					}
				}
			}
		}
	}
	if(!bSkipChange){
		if(objSettings.sStopTitles!=""){
			g_bMatchedWndFound=0;
			::EnumWindows(EnumWindowsProc,0);
			if(g_bMatchedWndFound){
				bSkipChange=TRUE;
				if(pMainDialogWindow && objSettings.lShowPopupsOnErr){
					pMainDialogWindow->m_STray.PopupBaloonTooltip(_l(AppName()+": Warning"),_l("Skipping wallpaper generation due\npresence of specific window"));
				}
			}
		}
	}
	//
	if(bSkipChange){
		// Повторная проверка не раньше чем через 10 минут
		objSettings.lLastTimeChange=(GetTickCount()+60*10*1000)-(objSettings.lChangePeriod*60*1000);
		// Мигаем красным глазом
		if(pMainDialogWindow){
			pMainDialogWindow->StartAnimation(-1);
			Sleep(700);
			pMainDialogWindow->StopAnimation();
		}
		return 0;
	}
	int iCurrentOrderStyle=objSettings.bChangeWPaperCycleType;
	if(pMainDialogWindow){
		if(pMainDialogWindow->OrderModifierForNextWallpaper!=UNDEFVALUE){
			iCurrentOrderStyle=pMainDialogWindow->OrderModifierForNextWallpaper;
			pMainDialogWindow->OrderModifierForNextWallpaper=UNDEFVALUE;
		}
	}
	if(iCurrentOrderStyle==-3){
		// Это тоже самое, только без балуна
		iCurrentOrderStyle=-2;
	}
	if(iCurrentOrderStyle==-2 && objSettings.sLastWallpaperFromUser==""){
		// Хоть и нужна последняя, но последней нету - ставим случайную
		iCurrentOrderStyle=objSettings.bChangeWPaperCycleType;
	}
	if(iCurrentOrderStyle!=-2){
		// По номеру...
		// getnextwallpaper
		objSettings.lCurPicture=objSettings.m_WChanger.GetNextWPNumber(iCurrentOrderStyle);
		if(objSettings.lCurPicture<0 || objSettings.lCurPicture>=objSettings.m_WChanger.GetLastActivePictureNum()){
			if(objSettings.lCurPicture==-1){
				return 0;
			}else{
				// Пока нельзя сказать что нужно использовать
				// Делаем "Показать текущую"
				iCurrentOrderStyle=-2;
			}
		}
		objSettings.lLastTimeChange=GetTickCount();
		if(iCurrentOrderStyle!=-2){// Устанавливаем обою (по номеру)
			if(objSettings.m_WChanger.SetWallPaperByIndex(objSettings.m_WChanger.aImagesInRotation[objSettings.lCurPicture])){
				objSettings.bNeedFirstTimeChange=0;// Обнуляем необходимость первоначальной смены
			}
		}
	}
	if(iCurrentOrderStyle==-2){
		CString sLastWP=objSettings.sLastWallpaperFromUser;
		if(sLastWP==""){
			sLastWP=Format("%sSkins\\NoteSkin0\\textbg.jpg",GetApplicationDir());
		}
		// Последняя из установленных...
		SetWallPaper(sLastWP, objSettings.iLastStretchingFromUser, "User defined");
	}
	// Выходим если нужно
	if(pMainDialogWindow && (objSettings.bChangeWPaperOnStart==2||objSettings.bChangeWPaperOnStart==4) && GetKeyState(VK_CONTROL)>=0 && GetKeyState(VK_MENU)>=0 && GetKeyState(VK_SHIFT)>=0 && !objSettings.bDoModalInProcess){
		objSettings.lNoExitWarning=1;
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
	}
#ifndef ART_VERSION
	if(objSettings.lVeryFirstStart && bWasDscShow==0){
		pMainDialogWindow->m_STray.PopupBaloonTooltip(_l(AppName()+": First wallpaper"),_l(DEF_FIRSTPOPUP));
	}
#endif
	if(objSettings.bUnderWindowsNT && !objSettings.lReuseBrowser){
		HANDLE hProcess=::OpenProcess(PROCESS_SET_QUOTA,FALSE,GetCurrentProcessId());
		if(hProcess){
			SetProcessWorkingSetSize(hProcess, -1, -1);
			//SetProcessWorkingSetSize(hProcess, 3*1024*1024, 20*1024*1024);
		}
	}
	if(pMainDialogWindow && (!objSettings.lVeryFirstStart || bWasDscShow!=0)){
		if(iInitialDesiredOrder==-2 && !objSettings.lApplyingChangesWarn && bBaloonWasOpened){
			// Убираем предупреждения
			pMainDialogWindow->ShowBaloon("","");
		}
	}
	bWasDscShow=1;
	return 1;
}

void AppMainDlg::SetNextWallPaperTimely()
{
	FORK(SetNextWallPaper_InThread,1);
}

void AppMainDlg::SetNextWallPaper()
{
	FORK(SetNextWallPaper_InThread,0);
}

void AppMainDlg::OnSystrayRegister()
{
	m_STray.ShowIcon();
	objSettings.OpenOptionsDialog(_l(REGISTRATION_TOPIC));
}

void AppMainDlg::OnSystraySendasuggestion()
{
	SendSuggestion(AppName(),PROG_VERSION);
}

void AppMainDlg::OnSystrayEnliveWp()
{
	objSettings.lEnliverON=1-objSettings.lEnliverON;
	UpdateEnviverState();
}

void AppMainDlg::OnSystrayDownloadClocks()
{
	DownloadWC(0,0);
}

void AppMainDlg::OnSystrayDownloadSkins()
{
	OpenNoteSkins(0,0);
}

void AppMainDlg::OnSystrayTellAFriend()
{
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=tellafriend&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
}

void AppMainDlg::OnSystrayDownloadWp()
{
	DownloadWP(0,0);
}

void AppMainDlg::OnSystrayDownloadWpt()
{
	DownloadWPT(0,0);
}

void AppMainDlg::OnSystrayNoteDisable()
{
	objSettings.lDisableNotes=objSettings.lDisableNotes?0:1;
	PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
}

void AppMainDlg::OnSystrayNoteOptions()
{
	objSettings.OpenOptionsDialog(_l(DESKNOTE_TOPIC));
}

void AppMainDlg::OnSystrayNewNote()
{
#if	defined(USE_LITE) || defined(ART_VERSION)
	return;
#endif
	CNote nt;
	CDLG_Note dlg;
	dlg.bForceFocus=TRUE;
	static long lLock=0;
	static CWnd* pNewNoteWnd=NULL;
	if(lLock>0){
		if(pNewNoteWnd){
			SwitchToWindow(pNewNoteWnd->GetSafeHwnd(),TRUE);
		}
		return;
	}
	if(objSettings.iLikStatus<2){
		if(objSettings.aDesktopNotes.GetSize()>2){
			UnregAlert(OPTION_NOTE_UNREG);
			return;
		}
	}
	SimpleTracker Track(lLock);
	if(nt.bUnderEdit==0){
		char szTmp[128]={0};
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,0,0,szTmp,sizeof(szTmp));
		nt.sTitle=szTmp;//%#c//_l("Created"),//_l("Note"),
		nt.sNoteFile=Format("%s.wnt",GenerateNewId("WDN"));
		dlg.pNote=&nt;
		dlg.bNewNote=TRUE;
		dlg.bNoBGImage=objSettings.aNoteSkin[objSettings.lNoteSkin].bIsHTML?TRUE:FALSE;
		nt.dwTColor=objSettings.aNoteSkin[objSettings.lNoteSkin].dwTXColor;
		pNewNoteWnd=&dlg;
		nt.dwBColor=objSettings.aNoteSkin[objSettings.lNoteSkin].dwBGColor;
		nt.iIcon=objSettings.lDefaultNoteIcon;
		if(nt.iIcon>=objSettings.aNoteSkin[objSettings.lNoteSkin].aIconsTitles.GetSize()){
			nt.iIcon=rnd(0,objSettings.aNoteSkin[objSettings.lNoteSkin].aIconsTitles.GetSize()-1);
		}
		int iRes=dlg.DoModal();
		pNewNoteWnd=NULL;
		if(iRes==IDOK){
			CSmartLock lN(&objSettings.cNoteLock,TRUE,-1);
			nt.SaveToFile();
			objSettings.aDesktopNotes.Add(nt);
			OrderModifierForNextWallpaper=-2;
			PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
			if(pMainDialogWindow->dlgWChanger){
				pMainDialogWindow->dlgWChanger->RefreshNotesMenu();
			}
		}
	}
}

void AppMainDlg::OnSystrayExchangeWp()
{
	//if(AddDefsInteract()){!!!
	CString sError=_l("Current wallpaper can`t be exchanged")+"!";
	if(objSettings.lCurPicture<0 || objSettings.lCurPicture>=objSettings.m_WChanger.aImagesInRotation.GetSize()){
		Alert(sError);
		return;
	}
	DWORD dwSampling=0;
	CString sPath="",sThemeName="";
	objSettings.m_WChanger.GetWPTitleAndSampling(objSettings.m_WChanger.aImagesInRotation[objSettings.lCurPicture], sPath, sThemeName, dwSampling);
	if(!isFileExist(sPath)){
		Alert(sError);
		return;
	}
	CString sContest="0";//sContest="&contest=no";
	if(objSettings.iLikStatus<2){
		//sContest="&contest=yes";
		sContest="1";
	}
	CString sRFile=Format("%s"WP_TEMPLATE"\\Upload.html",GetApplicationDir(),0);
	GenerateForm(sRFile,CString("ADDINFO:(")+sContest+")",0);
	/*
	//ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/guestbook/thumbview.php?action=upload_form&who="PROGNAME""PROG_VERSION+sContest,NULL,NULL,SW_SHOWNORMAL);
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=upload&who="PROGNAME""PROG_VERSION+sContest,NULL,NULL,SW_SHOWNORMAL);
	*/
}

char* AppMainDlg::GetWebPage(const char* sFile)
{
	CWebWorld url;
	CString sErr=_l("Failed to load");
	sErr+=" ";
	sErr+=sFile;
	url.SetErrorMessage(sErr);
	CString sPageContent=url.GetWebPage(sFile);
	return strdup(sPageContent);
}

BOOL GetElementCoords(HELEMENT he, RECT& recta)
{
	HTMLayoutGetElementLocation(he,&recta,CONTENT_BOX|ROOT_RELATIVE);
	return TRUE;
}

CString GetElementAttribute(HELEMENT he, const char* szName)
{
	LPCWSTR heId = 0;
	HLDOM_RESULT res = HTMLayoutGetAttributeByName(he, szName, &heId);
	if(!heId)
	{
		return "";
	}
	USES_CONVERSION;
	return W2A(heId);
}

void AppMainDlg::WalkHTML(HELEMENT he)
{
	if(!he){
		return;
	}
	const char* szTagName=0;
	HTMLayoutGetElementType(he, &szTagName);
	CString Attrib=GetElementAttribute(he,"interactivityTooltip");
	//Обрабатываем...
	if(Attrib!=""){
		RECT item;
		GetElementCoords(he, item);//Для расчета положения элементов на странице
		CRect critem=item;
		CString sAction=UnescapeString(GetElementAttribute(he,"interactivityAction"));
		if(sAction.Find("EDIT-CAL-NOTE")==0){
			// транслируем
			int id=atol(CDataXMLSaver::GetInstringPart(" d[","]",sAction));
			int im=atol(CDataXMLSaver::GetInstringPart(" m[","]",sAction));
			int iy=atol(CDataXMLSaver::GetInstringPart(" y[","]",sAction));
			DWORD dwFG=atol(CDataXMLSaver::GetInstringPart("fg[","]",sAction));
			DWORD dwBG=atol(CDataXMLSaver::GetInstringPart("bg[","]",sAction));
			CString sNotesFile=CDataXMLSaver::GetInstringPart("nf[","]",sAction);
			CString sThisDayNKey=CDataXMLSaver::GetInstringPart("nk[","]",sAction);
			COleDateTime dtCurrent(iy,im,id,0,0,0);
			sAction="HINTS:(USE-NOTE-AS-EDITOR,TITLE-DISABLED,NOSIZE,NOMOVE,REDRAW-AFTER-EDIT,FULL-WND-EDIT)";
			sAction+=Format("X:(%i),Y:(%i),W:(%i),H:(%i)",critem.left,critem.top,critem.Width(),critem.Height());
			char szTmp[128]={0};
			szTmp[0]=0;
			SYSTEMTIME sysTime;
			dtCurrent.GetAsSystemTime(sysTime);
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&sysTime,0,szTmp,sizeof(szTmp));
			sAction+=Format("FG:(%i),BG:(%i),",dwFG,dwBG);
			sAction+=Format("TITLE:(%s: %s)",_l("Date"),szTmp);
			sAction+=Format("EDIT-FILE:(%s)",sNotesFile);
			sAction+=Format("EDIT-KEY:(%s)",sThisDayNKey);
			sAction+=Format("EDIT-ON-DATE:(%s)",CDataXMLSaver::OleDate2Str(dtCurrent));
		}
		sAction+=Format("<TOOLTIP>%s</TOOLTIP>",UnescapeString(Attrib));
		AddActiveZone(&critem, sAction);
	}
	//Переходим к детям...
	UINT chCount=0;
	HTMLayoutGetChildrenCount(he,&chCount);
	for(UINT i=0;i<chCount;i++)
	{
		HELEMENT child=0;
		HTMLayoutGetNthChild(he,i,&child);
		WalkHTML(child);
	}
	return;
}


#pragma comment(lib,"..\\SmartWires\\HLayout\\HTMLayout.lib")
BOOL AppMainDlg::RenderHTML(HDC dc,RECT rtAreaIn,const char* sBlock, BOOL bAddActiveZones)
{
	char szBaseURI[MAX_PATH]={0};
	GetBaseURI(szBaseURI,CDataXMLSaver::GetInstringPart("<base url=\"","\">",sBlock));
	CRect rtArea(rtAreaIn);
#ifdef USE_HTMLITE
	HTMLite engine;
	USES_CONVERSION;
	bool rlod=engine.load((LPBYTE)((const char*)sBlock),strlen(sBlock),A2W(szBaseURI));
	bool rmea=engine.measure(rtArea.Width(), rtArea.Height());
	bool rren=engine.render(dc, rtArea.left, rtArea.top, rtArea.Width(), rtArea.Height());
	GdiFlush();
#else
	// рендери м через печать
	PrintEx engine;
	engine.load((LPBYTE)((const char*)sBlock),strlen(sBlock),szBaseURI);
	//Sleep(2000);
	//engine.load("c:\\aaa.html");
	int m_scaled_width=rtArea.Width();//Пикселы
	int num_pages = engine.measure(dc, m_scaled_width, rtArea.Width(), rtArea.Height());
	DWORD dwPageH=engine.getDocumentHeight();
	bool bRenderResult;
	for(int i=0;i<num_pages;i++){
		bRenderResult=engine.render(dc, rtArea.left, rtArea.top, i);
		// Только первую
		break;
	}
	//AfxMessageBox(Format("%i-%i: %i, %i",rtArea.Width(),rtArea.Height(),dwPageH,bRenderResult));
#endif
	if(bAddActiveZones){
		HELEMENT hRoot=engine.getRootElement();
		WalkHTML(hRoot);
	}
	return 1;
}

BOOL AppMainDlg::GetBaseURI(char* szPath, const char* szForWhat)
{
	strcpy(szPath,GetWPTemplateFilePath(szForWhat));
	return TRUE;
}

BOOL AppMainDlg::FreeChar(char* szChar)
{
	free(szChar);
	return 1;
}

long AppMainDlg::GetCallerOption(long lOption)
{
	if(lOption==CALLER_OPTION_WRAPTEXT){
		return objSettings.lAutoWrapNote;
	}
	return 0;
}


BOOL AppMainDlg::GetRndWallpaperPath(int iType,char* s)
{
	if(iType==-99){
		strcpy(s,"QWEASS");
		return 15;
	}
	DWORD dwSampling=0;
	CString sPath,sThemeName;
	int iNextWpNum=objSettings.m_WChanger.GetNextWPNumber(iType);
	if(iNextWpNum>=0 && iNextWpNum<objSettings.m_WChanger.GetLastActivePictureNum()){
		objSettings.m_WChanger.GetWPTitleAndSampling(objSettings.m_WChanger.aImagesInRotation[iNextWpNum], sPath, sThemeName, dwSampling);
	}
	strcpy(s,sPath);
	return 1;
}

BOOL AppMainDlg::AddActiveZone(CRect* rtZone, const char* szAction)
{
	CSmartLock lActZones(&csActZones,TRUE);
	if(rtZone!=NULL && szAction!=NULL){
		int iIndex=objSettings.aActiveZonesRect.Add(*rtZone);
		objSettings.aActiveZonesActions.SetAtGrow(iIndex,szAction);
	}
	return TRUE;
}

BOOL AppMainDlg::ShowBaloon(const char* szText,const char* szTitle,DWORD dwTimeout)
{
	m_STray.PopupBaloonTooltip(szTitle,szText,0,dwTimeout==0?9000:dwTimeout);
	return TRUE;
}

BOOL AppMainDlg::GenerateFullFeaturedWallpaper(DWORD dwIndex, HBITMAP& hBmpOut, CRect sizeOut, BOOL bEffects)
{
	SimpleLocker lc(&objSettings.csWPInChange);
	DWORD dwSampling=0;
	CString sPath="",sThemeName="";
	objSettings.m_WChanger.GetWPTitleAndSampling(dwIndex, sPath, sThemeName, dwSampling);
	if(sPath!=""){
		CSetWallPaper wpCommonData;
		wpCommonData.bRegardMM=0;
		wpCommonData.sWallpaperFile=sPath;
		wpCommonData.sWallpaperThemeName=sThemeName;
		wpCommonData.dwResamplingType=dwSampling;
		wpCommonData.bSkipWrapperStep=bEffects?0:1;
		wpCommonData.sizeOfOutImage=sizeOut.Size();
		wpCommonData.requiredSizeOfOutImage=sizeOut;
		wpCommonData.lScrMode=1;
		if(bEffects){
			wpCommonData.TemplatesInWork.RemoveAll();
			if(objSettings.sScrEffect!=""){
				CStringArray aPres;
				GetPresetList(aPres);
				for(int iIndex=0; iIndex<aPres.GetSize();iIndex++){
					if(objSettings.sScrEffect==aPres[iIndex]){
						wpCommonData.InitForPreset(aPres[iIndex]);
						break;
					}
				}
			}
			int iML=GetIndexInArray("Minilendar",&wpCommonData.TemplatesInWork,0);
			if(objSettings.iScrEffect_ML){
				if(iML==-1){
					wpCommonData.TemplatesInWork.Add("Minilendar");
				}
			}else{
				if(iML!=-1){
					wpCommonData.TemplatesInWork.RemoveAt(iML);
				}
			}
			int iTS=GetIndexInArray("TimeStamp",&wpCommonData.TemplatesInWork,0);
			if(objSettings.iScrEffect_TS){
				if(iTS==-1){
					wpCommonData.TemplatesInWork.Add("TimeStamp");
				}
			}else{
				if(iTS!=-1){
					wpCommonData.TemplatesInWork.RemoveAt(iTS);
				}
			}
		}
		PrepareWallPaper(&wpCommonData);
		CopyBitmapToBitmap(wpCommonData.hbmOutImage,hBmpOut,wpCommonData.sizeOfOutImage,0,FALSE);
	}
	return TRUE;
}

void AppMainDlg::OnSystrayBackUpList()
{
	CString sFile;
	if(!OpenFileDialog("Any files (*.*)|*.*||",&sFile,0,FALSE,TRUE,dlgWChanger?(CWnd*)dlgWChanger:(CWnd*)this)){
		return;
	}
	if(isFileExist(sFile)){
		if(AfxMessageBox(Format("%s ('%s')?",_l("Overwrite existing backup"),TrimMessage(sFile,40,3)),MB_YESNO|MB_ICONQUESTION)!=IDYES){
			return;
		}
	}
	objSettings.m_WChanger.Save(sFile);
	Alert(_l("Image list saved successfully")+"!",5000);
}

void AppMainDlg::OnSystrayRestoreList()
{
	CString sFile;
	if(!OpenFileDialog("Any files (*.*)|*.*||",&sFile,0,FALSE,TRUE,dlgWChanger?(CWnd*)dlgWChanger:(CWnd*)this)){
		return;
	}
	if(!isFileExist(sFile)){
		return;
	}
	SimpleLocker lc(&objSettings.csWPInChange);
	BOOL bNeedReopen=FALSE;
	if(dlgWChanger){
		dlgWChanger->SendMessage(WBT_EXIT,0,0);
		while(dlgWChanger!=0){
			Sleep(100);
		}
		bNeedReopen=TRUE;
	}
	objSettings.m_WChanger.Load(sFile);
	Alert(_l("Image list restored successfully")+"!",5000);
	if(bNeedReopen){
		OnSystrayOpen();
	}
}

#ifndef SHELLSTATE
typedef struct {
    BOOL fShowAllObjects:1;
    BOOL fShowExtensions:1;
    BOOL fNoConfirmRecycle:1;
    BOOL fShowSysFiles:1;
    BOOL fShowCompColor:1;
    BOOL fDoubleClickInWebView:1;
    BOOL fDesktopHTML:1;
    BOOL fWin95Classic:1;
    BOOL fDontPrettyPath:1;
    BOOL fShowAttribCol:1;
    BOOL fMapNetDrvBtn:1;
    BOOL fShowInfoTip:1;
    BOOL fHideIcons:1;
    BOOL fWebView:1;
    BOOL fFilter:1;
    BOOL fShowSuperHidden:1;
    BOOL fNoNetCrawling:1;
    DWORD dwWin95Unused;
    UINT uWin95Unused;
    LONG lParamSort;
    int iSortDirection;
    UINT version;
    UINT uNotUsed;
    BOOL fSepProcess:1;
    BOOL fStartPanelOn:1;
    BOOL fShowStartPage:1;
    UINT fSpareFlags:13;
} SHELLSTATE, *LPSHSHELLSTATE;
#define _SSF_HIDEICONS 16384
#endif
typedef void (WINAPI *_SHGetSetSettings)(SHELLSTATE *lpss,DWORD dwMask,BOOL bSet);
void AppMainDlg::OnShowHideIcons(BOOL bValue)
{
	HWND hLV=GetSysListView();
	if(hLV){
		if(bValue && ::IsWindowVisible(hLV)){
			::ShowWindowAsync(hLV,SW_HIDE);
		}
		if(!bValue){
			int iCount=4;
			::ShowWindowAsync(hLV,SW_SHOW);
			HWND Hp1=::GetParent(hLV);
			while(Hp1!=0 && iCount>0){
				::ShowWindowAsync(Hp1,SW_SHOW);
				Hp1=::GetParent(Hp1);
				iCount--;
			}
		}
	}
}

void AppMainDlg::OnShowHideIcons()
{
	static long lLock=0;
	if(lLock>0){
		return;
	}
	SimpleTracker Track(lLock);
	objSettings.lHideIconsByDefault=1-objSettings.lHideIconsByDefault;
	OnShowHideIcons(objSettings.lHideIconsByDefault);
	return;
	/*
	BOOL bRes=FALSE;
	HINSTANCE hShell32=LoadLibrary("Shell32.dll");
	if(hShell32){
		_SHGetSetSettings fp=(_SHGetSetSettings)GetProcAddress(hShell32,"SHGetSetSettings");
		SHELLSTATE st;
		memset(&st,0,sizeof(st));
		(*fp)(&st,_SSF_HIDEICONS,FALSE);
		st.fHideIcons=1-st.fHideIcons;
		(*fp)(&st,_SSF_HIDEICONS,TRUE);
		FreeLibrary(hShell32);
	}
	HWND hWnd = ::FindWindow("Progman",NULL);
	::ShowWindow(hWnd,SW_HIDE);
	//::SendMessage(hWnd , WM_SETTINGCHANGE, 0,0);
	DWORD dwRet=0;
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,(LPARAM)"Software\\Microsoft\\Internet Explorer\\Main", SMTO_ABORTIFHUNG, 3 * 1000,&dwRet);
	::ShowWindow(hWnd,SW_SHOW);
	//::InvalidateRect(hWnd,0,1);
	//::SendMessage(hWnd, WM_SETTINGCHANGE, 0, 0);
	return;
	*/
}

void AppMainDlg::OnEmail() 
{
	PostMessage(WM_COMMAND,ID_SYSTRAY_SENDASUGGESTION,0);
}

void AppMainDlg::OnWebsite3() 
{
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=none&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
}

void AppMainDlg::OnWebsite2() 
{
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wcboard&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
}

void AppMainDlg::OnBuyNow()
{
	BuyWC(0,0);
}

void AppMainDlg::OnSystrayImportClock()
{
	if(IsVista()){
		AlertBox(_l("Restriction"),_l("This feature is not supported in MS Vista"),6000);
		return;
	}
	if(objSettings.isLight){
		AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
		return;
	}
	if(objSettings.iLikStatus<2 || isTempKey()){
		UnregAlert(OPTION_UNREGF);
		return;
	}
	CStringArray sFileA;
	CString sFile=getMyDocPath()+"\\*.swf";
	if(!OpenFileDialog("Flash files (*.swf)|*.swf||",(LPVOID)&sFile,0,FALSE,TRUE,
		dlgWChanger?(CWnd*)dlgWChanger:(CWnd*)this,TRUE,&sFileA)){
		return;
	}
	int iCount=0;
	SimpleLocker lc(&objSettings.csWPInChange);
	for(int i=0;i<sFileA.GetSize();i++){
		sFile=sFileA[i];
		if(!isFileExist(sFile)){
			return;
		}
		CString sNewFileMainPart=GetApplicationDir();
		sNewFileMainPart+=WP_TEMPLATE"\\";
		CString sClockDir=sNewFileMainPart+WP_CLOCKS"\\";
		CreateDirIfNotExist(sClockDir);
		sClockDir+="Imported\\";
		CreateDirIfNotExist(sClockDir);
		CString sNewFileSwf=sClockDir+GetPathPart(sFile,0,0,1,0)+".swf";
		if(!CopyFile(sFile,sNewFileSwf,FALSE)){
			Alert(_l("Failed to copy clock file. Operation aborted"),5000);
			return;
		}
		CString sTemplate;
		GetHTMLForClock(sNewFileSwf,sTemplate);
		SaveFile(sNewFileMainPart+"Clock "+GetPathPart(sFile,0,0,1,0)+".wpt",sTemplate);
		iCount++;
	}
	if(iCount){
		//Alert(_l("Restart "PROGNAME" to see new clock")+"!",5000);
		RestartWC();
	}
}

void AppMainDlg::OnSystrayDisableWpt()
{
	objSettings.lTempDisableAllWidgets=objSettings.lTempDisableAllWidgets?0:1;
	OrderModifierForNextWallpaper=-2;
	PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
}

void AppMainDlg::OnSystrayRestart()
{
	RestartWC();
}

CWnd* AfxWPGetMainWnd()
{
	return pMainDialogWindow;
}

BOOL AppMainDlg::GetNotesFile(char* szOut,int size)
{
	strcpy(szOut,GetTextIncludeFilePath_forTempl(Format(PROFILE_PREFIX"daydesc_l%i.txt",GetApplicationLang())));
	return 1;
}

BOOL AppMainDlg::GetRemDefText(char* szText,int size)
{
	return 0;
}

BOOL AppMainDlg::GetRemDefWav(char* szSoundPath,int size)
{
	CString sReminder=objSettings.sReminderWav;
	if(sReminder.Find(":\\")==-1){
		sReminder=CString(GetApplicationDir())+objSettings.sReminderWav;
	}
	strcpy(szSoundPath,sReminder);
	return 0;
}

BOOL AppMainDlg::GetRemDefKey(char* szKey,int size)
{
	strcpy(szKey,GenerateNewId("CL",""));
	return 0;
}

BOOL AppMainDlg::CallContextDayNote(SYSTEMTIME stTime, HWND hCalendar)
{
	return TRUE;
}

BOOL AppMainDlg::CallEditDayNote(SYSTEMTIME stTime, char szFG[64], char szBG[64])
{
	CString sExecuteLine="HINTS:(USE-NOTE-AS-EDITOR,TITLE-DISABLED,NOSIZE,NOMOVE,REDRAW-AFTER-EDIT,FULL-WND-EDIT)";
	CPoint pt;
	GetCursorPos(&pt);
	sExecuteLine+=Format("X:(%i),Y:(%i),W:(70),H:(70)",pt.x,pt.y);
	char szMainFile[MAX_PATH]={0};
	GetNotesFile(szMainFile,sizeof(szMainFile));
	sExecuteLine+=Format("TITLE:(%s)EDIT-FILE:(%s)",_l("Edit note"),szMainFile);
	sExecuteLine+=Format("FG:(%s)BG:(%s)",szFG,szBG);
	sExecuteLine+=Format("EDIT-KEY:(%02i/%i/%i )",stTime.wDay,stTime.wMonth,stTime.wYear);
	sExecuteLine+=Format("EDIT-ON-DATE:(%02i/%i/%i 00:00:00)",stTime.wDay,stTime.wMonth,stTime.wYear);
	//CallFileForOpenOrEdit(sExecuteLine,0);
	::SendMessage(pMainDialogWindow->GetSafeHwnd(),DESKTOP_CALLONDT,WPARAM(strdup(sExecuteLine)),0);
	return 1;
}

BOOL AppMainDlg::GetDayNote(SYSTEMTIME Time,char szDsc[2048])
{
	return 0;
}


BOOL CALLBACK CheckLikKeys(HIROW hData, CDLG_Options* pDialog);
void AppMainDlg::OnSystrayClanchMoney()
{
	if(GetApplicationLang()!=1){
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXCHANGEWALLPAPER,0);
		return;
	}
	CString sSMS;
	CDLG_AddLink dlg(this);
	dlg.iType=99;
	dlg.pLink=&sSMS;
	dlg.DoModal();
	if(sSMS.GetLength()==0){
		return;
	}
	CWebWorld url;
	CString sPageContent=url.GetWebPage(Format("http://www.wiredplane.com/cgi-bin/smsproxy_check.php?code=%s",EscapeString3(sSMS)));
	int iTru=sPageContent.Find("true");
	if(sPageContent.GetLength()==0){
		Alert(_l("Sorry, failed to check code")+"!");
	}else
	if(sSMS.GetLength()==0 || iTru!=0){
		Alert(_l("Sorry, SMS code is wrong")+"!");
	}else{
		Alert(_l("Code is OK, Thanks for registering")+"!");
		objSettings.sLikUser="SMS";
		COleDateTime dt=COleDateTime::GetCurrentTime();
		dt+=7;
		objSettings.sLikKey=Format("WIRECHANGER%02i%02i%02iASTON-PISTON-DRUBEL%sMARTIN-CARTIN-GIGARTESK",dt.GetDay(),dt.GetMonth(),dt.GetYear()%1000,objSettings.sLikUser);
		objSettings.sLikKey=Format("WPK%02i%02i%02i%s",dt.GetDay(),dt.GetMonth(),dt.GetYear()%1000,MD5_HexString(objSettings.sLikKey));
		UpdateTrialityStatus(objSettings.sLikUser,objSettings.sLikKey);
		CheckLikKeys(0,0);// Запустить рестарт!
	}
}

#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif