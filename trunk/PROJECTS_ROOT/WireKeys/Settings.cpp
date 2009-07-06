// Settings.cpp: implementation of the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "DLG_Chooser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStub wk;
int iGlobalState=-1;
char szSystemFolder[MAX_PATH+1];
long CQuickRunItem::lExtraMscroHotkeysIDs=HK_EXTRAMACRO;
DWORD WINAPI GetHKL(LPVOID)
{_XLOG_
	// борьба с кодировками
	objSettings.lHKL1=0;
	objSettings.lHKL2=1;
	memset(objSettings.aKBLayouts,0,MAX_HKL*sizeof(objSettings.aKBLayouts[0]));
	objSettings.lHKLCount=GetKeyboardLayoutList(MAX_HKL,(HKL*)&objSettings.aKBLayouts);
	if(objSettings.lHKLCount>2){_XLOG_
		//HKL hCurLayout=GetKeyboardLayout(0);
		for(int i=0;i<objSettings.lHKLCount;i++){_XLOG_
			objSettings.sHKLList+=wk.GetHKLName(i);
			/* 
			char szBuffer[KL_NAMELENGTH]={0};
			/ActivateKeyboardLayout(aKBLayouts[i], 0);
			GetKeyboardLayoutName(szBuffer);
			sHKLList+=szBuffer;
			// or this
			WORD dwDevID=HIWORD(aKBLayouts[i]);
			if(dwDevID>0){_XLOG_
			sHKLList+=Format(" #%4X",dwDevID);
			}*/
			objSettings.sHKLList+="\t";
		}
		objSettings.sHKLList.TrimRight("\t");
		//ActivateKeyboardLayout(hCurLayout, 0);
	}
	return 0;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSettings::CSettings()
{_XLOG_
	// Оно глобальное поэтому сюда
	while(GlobalFindAtom("WPLABSDEBUG-WK")){
		GlobalDeleteAtom(GlobalFindAtom("WPLABSDEBUG-WK"));
	}
	CDebugWPHelper::isDebug()=0;
	lSkipSavings=0;
	lEverythingIsPaused=0;
	lIsScreensaverRandomized=0;
	lDoNotLoadMacros=0;
	bShutIP=0;
	bIgnoreNextClipAction=0;
	lInPlugAction=0;
	hLastForegroundWndForPlugins=0;
	CString sOS;
	iLikStatus=1;
	pSplashFont=0;
	pOpenedOptions=0;
	lCreateQRunFrom=0;
	hActiveApplicationAtStart=0;
	hTimerEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	lDefaultMacros=-999;
	lStartupMsgType=DEF_STARTUPMSGT;
	bUnderWindows98=bUnderWindowsNT=0;
	GetOSVersion(sOS,NULL,&bUnderWindowsNT);
	bUnderWindows98=!bUnderWindowsNT;
	if(bUnderWindows98){_XLOG_
		wk._ShowCPUUsageDef=0;
	}
	lHotMenuOpened=0;
	lOptionsDialogOpened=0;
	bSupportOwnerDraw=0;
	bAskForKill=0;
	bAskForRemQ=0;
	bIntegrateWithWN=0;
	lClipTransmitMode=0;
	hWndToTransmitTo=NULL;
	bFixXSize=bFixYSize=0;
	lFixXSizeTo=DEFAULT_XRES;
	lFixYSizeTo=DEFAULT_YRES;
	sDetachedSize=50;
	bSignSShot=0;
	hUDIcon=0;
	sSignString=DEFAULT_SCRSIGN;
	bHidedAppShowMenu=DEFAULT_SHOWHMENU;
	hHotMenuWnd=NULL;
	hFocusWnd=NULL;
	ActiveWndRect.SetRect(0,0,0,0);
	bRunStatus=0;
	bSaveAllowed=0;
	lWaitForAddclipInput=0;
	_XLOG_
	GetSpecialFolderPath(szSystemFolder,CSIDL_APPDATA);
	sScrshotDir=szSystemFolder;
	sScrshotDir2="";
	sScrshotFile=DEFAULT_SCRFILE;
	sMainWindowTitle=PROGNAME;
	iGlobalScrShotCount=1;
	bShotActiveWindow=0;
	bShotActiveWindow2=((objSettings.bUnderWindowsNT)?2:1);
	bBlackAndWhiteConfirm=1;
	bDoModalInProcess=0;
	lPreventTrayMenu=0;
	bStartWithOptions=FALSE;
	bApplicationFirstRun=0;
	rUserRect.left=rUserRect.right=rUserRect.top=rUserRect.bottom=0;
	lDefaultSelMethod=0;//bUnderWindows98?2:0;
	lDefaultSelQuality=0;
	lDesktopSwitchesAfterNextStart=0;
	_XLOG_
	lStartupDesktop=GetCurrentDesktopNumber();
	lLastActiveDesktop=lStartupDesktop;
	GetHKL(0);
	bRunStatus=1;{_XLOG_;COleDateTime dt=COleDateTime::GetCurrentTime();COleDateTimeSpan sp=dt-COleDateTimeSpan(3);};
	_XLOG_
}

DWORD WINAPI WhatsNew(LPVOID data)
{_XLOG_
	static long bWas=0;
	if(bWas){_XLOG_
		return 0;
	}
	bWas=1;
	if(AfxMessageBox(_l("Do you want to see 'What`s new' in this version of")+" "+PROGNAME+"?",MB_YESNO|MB_ICONQUESTION)==IDYES){_XLOG_
		ShowHelp("history");
	}
	return 0;
}

CString GetClipHistoryName(int iClipNumber)
{_XLOG_
	return Format("CLIP%lu",iClipNumber);
}

BOOL CQuickRunItem::SetHotkey(int iPos,CIHotkey oHotKey)
{_XLOG_
	if(iPos>=qrHotkeys.GetSize()){_XLOG_
		qrHotkeys.SetSize(iPos+1);
	}
	if(qrHotkeys[iPos]){_XLOG_
		delete qrHotkeys[iPos];
		qrHotkeys[iPos]=0;
	}
	for(int i=0;i<qrHotkeys.GetSize();i++){_XLOG_
		CQRunHotkeyItem* pItem=qrHotkeys[i];
		if(pItem==0){_XLOG_
			qrHotkeys[i]=new CQRunHotkeyItem();
		}
	}
	if(qrHotkeys[iPos]){_XLOG_
		delete qrHotkeys[iPos];
	}
	qrHotkeys[iPos]=new CQRunHotkeyItem(oHotKey);
	return TRUE;
}

BOOL CQuickRunItem::RemoveAllQRunHotkeys()
{_XLOG_
	for(int i=0;i<qrHotkeys.GetSize();i++){_XLOG_
		CQRunHotkeyItem* pItem=qrHotkeys[i];
		qrHotkeys[i]=0;
		delete pItem;
	}
	qrHotkeys.RemoveAll();
	return TRUE;
}

CQuickRunItem::~CQuickRunItem()
{_XLOG_
	//FLOG3("~CQuickRunItem %i: %s %s",this->lUniqueCount,this->sItemName,this->sUniqueID);
	RemoveAllQRunHotkeys();
};

CQuickRunItem::CQuickRunItem()
{_XLOG_
	static long lUniqueCountGlobal=1;
	lUniqueCount=lUniqueCountGlobal++;
	sUniqueID=GenerateNewId("QR");
	lExpandFolderOrCheckInstance=0;
	lFixedWidth=theApp.rDesktopRECT.Width();
	lFixedHeight=theApp.rDesktopRECT.Height();
	lStartupTime=DEF_STARTUPTIME;
	lUseDetachedPresentation=0;
	lUseAdditionalOptions=1;
	lSearchInstanceFirst=0;
	lTempCategNumber=0;
	lHowToShowInTray=1;
	lStartAtStartUp=0;
	lShowInUpMenu=0;
	lStartInDesktop=0;
	lOptionClass=-1;
	lParamType=-1;
	hWnd=HWND(-2);
	lAfterStart=0;
	lAutoCreate=0;
	lFixPostion=0;
	lFixedXPos=0;
	lFixedYPos=0;
	lFixSize=0;
	lMacros=0;
	lOpacity=100;
	//Создаем по-любому 0ой элемент
	qrHotkeys.SetAtGrow(0,new CQRunHotkeyItem());
};

void CSettings::Load(const char* szConfigFile)
{_XLOG_
	DefActionMapHK_Preinit[CONVERT_LASTWORD]=CIHotkey(VK_PAUSE);
	DefActionMapHK_Preinit[HOTSEL_MENU].InitFromSave("%2-2",TRUE);
	//DefActionMapHK_Preinit[WS_SHOWSTART].InitFromSave("%8@-0",TRUE);
	DefActionMapHK_Preinit[WS_SYSPROPS].InitFromSave("%8-19",TRUE);
	DefActionMapHK_Preinit[WS_SHOWDESK]=CIHotkey('D',HOTKEYF_EXT);
	DefActionMapHK_Preinit[WS_MINIMALL]=CIHotkey('M',HOTKEYF_EXT);
	DefActionMapHK_Preinit[WS_SHOWMYCOMP]=CIHotkey('E',HOTKEYF_EXT);
	DefActionMapHK_Preinit[WS_SEARCHFILE]=CIHotkey('F',HOTKEYF_EXT);
	DefActionMapHK_Preinit[WS_SEARCHCOMP]=CIHotkey('F',HOTKEYF_EXT|HOTKEYF_CONTROL);
	DefActionMapHK_Preinit[WS_SHOWWHELP]=CIHotkey(VK_F1,HOTKEYF_EXT);
	DefActionMapHK_Preinit[WS_LOCKKEYB]=CIHotkey('L',HOTKEYF_EXT);
	DefActionMapHK_Preinit[WS_SHOWRUND]=CIHotkey('R',HOTKEYF_EXT);
	//DefActionMapHK_Preinit[WS_SHOWUTILM]=CIHotkey('U',HOTKEYF_EXT);
	BOOL bRestoreMode=TRUE;
	CString sConfFile=szConfigFile;
	if(sConfFile==""){_XLOG_
		bRestoreMode=FALSE;
		sConfFile=getFileFullName(CString(PROGNAME)+".conf");
	}
	{_XLOG_
		//Читаем настройки
		CDataXMLSaver IniFile(sConfFile,"application-config",TRUE);
		if(!bRestoreMode && !IniFile.IsFileLoaded()){_XLOG_
			bStartWithOptions=TRUE;
		}
		// Записываем в реестр путь к инифайлу
		CRegKey key;
		if(key.Open(HKEY_CURRENT_USER, PROG_REGKEY)!=ERROR_SUCCESS){_XLOG_
			key.Create(HKEY_CURRENT_USER, PROG_REGKEY);
		}
		if(key.m_hKey!=NULL){_XLOG_
			CString sSource=IniFile.GetSourceFile();
			DWORD dwType=REG_SZ,dwSize=0;
			RegSetValueEx(key.m_hKey,"ConfigFile",0,REG_SZ,(BYTE*)(LPCSTR)sSource,lstrlen(sSource)+1);
			char szDisk[MAX_PATH]="C://", szPath[MAX_PATH]="";
			_splitpath(sSource, szDisk, szPath, NULL, NULL);
			CString sConfPath=szDisk;
			sConfPath+=szPath;
			RegSetValueEx(key.m_hKey,"UserData",0,REG_SZ,(BYTE*)(LPCSTR)sConfPath,lstrlen(sConfPath)+1);
			if(objSettings.bStartWithOptions){_XLOG_
				sConfPath=COleDateTime::GetCurrentTime().Format("%d.%m.%Y");
				RegSetValueEx(key.m_hKey,"InstallDate",0,REG_SZ,(BYTE*)(LPCSTR)sConfPath,lstrlen(sConfPath)+1);
				sConfPath="";
				ReadFile(CString(GetApplicationDir())+"affinfo.txt",sConfPath);
				RegSetValueEx(key.m_hKey,"AffData",0,REG_SZ,(BYTE*)(LPCSTR)sConfPath,lstrlen(sConfPath)+1);
			}
		}
		if(!objSettings.bStartWithOptions){_XLOG_
			IniFile.getValue("uint-language",				GetApplicationLang(),0);
		}
		IniFile.getValue("cur-version",					sPrevVersion);
		/*if(!bRestoreMode){_XLOG_
			if(objSettings.sPrevVersion!=PROG_VERSION && !objSettings.bStartWithOptions){_XLOG_
				FORK(WhatsNew,NULL);
			}
		}*/
		IniFile.getValue("uint-regStatus",				lPrevLik,-1);
		IniFile.getValue("uint-integrWN",				bIntegrateWithWN,0);
		IniFile.getValue("uint-checkForUpdates",		lCheckForUpdate,1);
		IniFile.getValue("uint-checkForUpdDays",		lDaysBetweenChecks,20);
		IniFile.getValue("uint-askForDefMacros",		lDefaultMacros,1);
		IniFile.getValue("uint-userTrayIcon",			sUDIcon);
		IniFile.getValue("uint-defOptionsTopic",		sDefOptionsTopic);
		//
		IniFile.getValue("uint-scrshotFile",			sScrshotFile,DEFAULT_SCRFILE);
		IniFile.getValue("uint-scrshotDir",				sScrshotDir,szSystemFolder);
		IniFile.getValue("uint-scrshotDir2",			sScrshotDir2,"%ASKFOLDER");
		IniFile.getValue("uint-scrshotActiveWnd",		bShotActiveWindow);
		IniFile.getValue("uint-scrshotActiveWnd2",		bShotActiveWindow2,((objSettings.bUnderWindowsNT)?2:1));
		IniFile.getValue("uint-scrshotBAWConfirm",		bBlackAndWhiteConfirm,1);
		IniFile.getValue("uint-scrshotSign",			bSignSShot);
		IniFile.getValue("uint-scrshotJpg",				bUseJpg);
		IniFile.getValue("uint-scrshotJpgQuality",		bUseJpgQuality,DEF_JPGQUALITY);
		IniFile.getValue("uint-scrshotPutInClip",		bPutScreshotInClipboard,1);
		IniFile.getValue("uint-scrshotShowIntInfo",		lShowScrData);
		IniFile.getValue("uint-autoEditShot",			bAutoEditOnShot);
		IniFile.getValue("uint-autoEditShotEd",			sJpgEditor,sJpgEditorDef);
		IniFile.getValue("uint-scrshotSignStr",			sSignString,DEFAULT_SCRSIGN);
		IniFile.getValue("uint-scrshotFixX",			bFixXSize);
		IniFile.getValue("uint-scrshotFixY",			bFixYSize);
		IniFile.getValue("uint-scrshotFixXTo",			lFixXSizeTo,DEFAULT_XRES);
		IniFile.getValue("uint-scrshotFixYTo",			lFixYSizeTo,DEFAULT_YRES);
		IniFile.getValue("uint-scrshotLastFile",		sLastScreenshot);
		IniFile.getValue("uint-scrshotByTimer",			lScreenshotByTimer);
		IniFile.getValue("uint-scrshotByTimerSec",		lScreenshotByTimerSec,5);
		IniFile.getValue("uint-scrshotAddCursor",		lAddCursorToScreenshot);
		IniFile.getValue("uint-startupMsgType",			lStartupMsgType,DEF_STARTUPMSGT);
		IniFile.getValue("uint-showIconInTray",			bShowIconInTray,1);
		IniFile.getValue("uint-showTextInTray",			lShowDateInTrayIcon);
		IniFile.getValue("uint-showCPUInTray",			lShowCPUInTrayIcon,wk._ShowCPUUsageDef);
		IniFile.getValue("uint-showCPUHistInTray",		lShowCPUHistInTray,wk._ShowCPUUsageDef);
		IniFile.getValue("uint-showCPUHistColumns",		lShowCPUHistInTrayC,4);
		IniFile.getValue("uint-showCPUInTrayRefr",		lShowCPUInTraySecs,2);
		IniFile.getValue("uint-exitWndsOnDbl",			lExitWndOnDbl);
		IniFile.getValue("uint-skipHKTrayWarning",		bSkipHKWarning);
		IniFile.getValue("uint-noNotifyByCursor",		lNoNotifyByCursor,1);
		IniFile.getValue("uint-skipTrayHelpItems",		lDisableSCHelp);
		IniFile.getValue("uint-skipFocusMove",			lDisableFocusMove);
		//IniFile.getValue("uint-alertToTaskbar",			lAlertsOnTaskbar,1);
		lAlertsOnTaskbar=1;
		IniFile.getValue("uint-noHotkeyDblPress",		lDisableDblPressExtensions);
		IniFile.getValue("uint-backupFile",				sBackupFile);
		IniFile.getValue("uint-showWinAmpTitle",		lShowWinampTitle,1);
		IniFile.getValue("uint-pathWinAmpFile",			sWinAmpStartPas);
		IniFile.getValue("uint-stripWinAmpStuffTitle",	lStripNumberWinampTitle,1);
		IniFile.getValue("uint-addWinAmpTTTitle",		lAddTTWinampTitle,1);
		IniFile.getValue("uint-addWinAmpTLTitle",		lAddTLWinampTitle,1);
		IniFile.getValue("uint-stripWinAmpStuffATitle",	lStripAllWinampTitle);
		IniFile.getValue("uint-showWTitleInTooltip",	lShowWTitInTooltip);
		IniFile.getValue("uint-catchWinAmpTitle",		lCatchWinampTitle);
		IniFile.getValue("uint-showWinAmpOSDSecs",		lWinampOSDTime,4);
		IniFile.getValue("uint-stopWinampOnScrsave",	lStopWinAmpOnScrsave);
		IniFile.getValue("uint-stopWinampOnLock",		lStopWinAmpOnLock);
		IniFile.getValue("uint-startWinampAfLock",		lStartWinAmpAfterLock);
		IniFile.getValue("uint-optimizeRegistry",		lOptimizeRegistryON,1);
		IniFile.getValue("uint-setRandomScreensaver",	lSetRandomScreensaver);
		IniFile.getValue("uint-setRandomScrSavValids",	sValidScreenSavers);
		IniFile.getValue("hkl-prefLayout1",				lHKL1,0);
		IniFile.getValue("hkl-prefLayout2",				lHKL2,1);
		IniFile.getValue("uint-osdDateFormat",			sODSDateFormat);
		IniFile.getValue("uint-osdTimeFormat",			sODSTimeFormat);
		IniFile.getValue("uint-statMous",				lStat_mouseLen);
		IniFile.getValue("uint-statKeyb",				lStat_keybCount);
		IniFile.getValue("uint-statHotk",				lStat_hotkCount);
		IniFile.getValue("uint-useOldStartStyle",		lUseOldStartStyle,1);
		//
		IniFile.getValue("uint-getSelectedType",		bGetTextSelectionStyle,lDefaultSelMethod);
		//IniFile.getValue("uint-getSelectedQual",		lDefaultSelQuality,1);
		lDefaultSelQuality=1;
		IniFile.getValue("uint-disableClipAtAll",		lDisableClipboard);
		IniFile.getValue("uint-chTrayOnClips",			lChTrayOnClips,0);
		IniFile.getValue("uint-lLastWordNotLine",		lLastWordNotLine,1);
		IniFile.getValue("uint-saveClipContent",		lSaveClipContent,1);
		IniFile.getValue("uint-saveClipContentPer",		lSaveClipContentPer,0);
		IniFile.getValue("uint-soundOnClipCopy",		bClipCopySound,1);
		IniFile.getValue("uint-soundOnClipPaste",		bClipPasteSound,1);
		IniFile.getValue("uint-soundOnClipCopyWav",		sClipCopyWav,DEFAULT_ONCCOPYSND);
		IniFile.getValue("uint-soundOnClipPasteWav",	sClipPasteWav,DEFAULT_ONCPASTESND);
		IniFile.getValue("uint-soundOnLConvWav",		sOnLConvWav,DEFAULT_ONLCONV);
		IniFile.getValue("uint-soundOnLConv",			bOnLConv,1);
		IniFile.getValue("uint-soundSShotWav",			sSShotWav,DEFAULT_SCRSHOT);
		IniFile.getValue("uint-soundSShot",				bSShot,1);
		IniFile.getValue("uint-soundCalc",				bSCalc,1);
		IniFile.getValue("uint-soundCalcWav",			sSCalcWav,DEFAULT_CALCSOUND);
		IniFile.getValue("uint-soundHk",				bSHk);
		IniFile.getValue("uint-soundHkWav",				sSHkWav,DEFAULT_HKSOUND);
		IniFile.getValue("uint-soundOnErr",				bErrorSound,1);
		IniFile.getValue("uint-soundOnErrWav",			sErrorSound,DEFAULT_ONERRSOUND);
		IniFile.getValue("uint-soundDisableAll",		lDisableSounds);
		IniFile.getValue("uint-directMouseToUnderC",	lDirectMouseToUnderC);
		
		IniFile.getValue("uint-menuOnDblHot",			bMenuOnDblHotMenu);
		IniFile.getValue("uint-sysActionsInHotMenu",	bSysActionsInHotMenu,DEFAULT_SYSSUBMENU);
		IniFile.getValue("uint-appShortcInHotMenu",		bAppShortcInHotMenu,1);
		IniFile.getValue("uint-screenSaverMenu",		bMiscSaverMenu,DEFAULT_SCRSUBMENU);
		IniFile.getValue("uint-qrunMenu",				bQRunMenu,DEFAULT_QRUNSUBMENU);
		IniFile.getValue("uint-macroMenu",				bMacroMenu, DEFAULT_MACRSUBMENU);
		IniFile.getValue("uint-categAllowDblStart",		lAllowDblCategStart,1);
		IniFile.getValue("uint-volumeMenu",				bVolumeMenu, DEFAULT_VOLUMEMENU);
		IniFile.getValue("uint-sysAskForKill",			bAskForKill);
		IniFile.getValue("uint-sysAskForRemQ",			bAskForRemQ);
		
		IniFile.getValue("uint-textActionsInHotMenu",	bTextActionsInHotMenu,DEFAULT_TEXTSUBMENU);
		IniFile.getValue("uint-showMenuInHidedApp",		bHidedAppShowMenu,DEFAULT_SHOWHMENU);
		IniFile.getValue("uint-supportOwnerDrawMenus",	bSupportOwnerDraw,SUPPORT_OWONDRDEF);
		IniFile.getValue("uint-linesToScroolByHK",		lNumberOfLinesToScroll,1);
		IniFile.getValue("uint-htrayMenuRecentItems",	lHTrayRecentItems,10);
		IniFile.getValue("uint-htrayFloaterBG",			lFloaterBG,BGCOLOR);
		IniFile.getValue("uint-htrayFloaterSize",		sDetachedSize,30);
		IniFile.getValue("uint-htrayFloaterTransp",		lFloaterTransp,70);
		IniFile.getValue("uint-trayIconLBDClick",		lLBTray,35);
		IniFile.getValue("uint-trayIconMBDClick",		lMBTray,16);
		IniFile.getValue("uint-trayIconRBDClick",		lRBTray,45);
		IniFile.getValue("uint-showIconWhenMacro",		lShowMacroIcon,1);
		IniFile.getValue("uint-overlayTrayHidIcons",	bOverlayTrayAppIcon);
		IniFile.getValue("uint-animateFloaters",		bAnimateFloaters,1);
		IniFile.getValue("uint-freeMemofhiddenApp",		bFreeMemOfHidApp);
		IniFile.getValue("uint-takeComLineForFloater",	bTakeComLineForFloater);
		IniFile.getValue("uint-floatUseSingleClick",	lFloatUseSingleClick,1);
		IniFile.getValue("uint-floatUseLBDir",			lFloatUseLBDir,1);
		IniFile.getValue("uint-preserveTitleOnRollup",	bPreserveTitle,1);
		IniFile.getValue("uint-extractIconsForSysMenu",	bExtractIconsForSysMenu,1);
		IniFile.getValue("uint-sortForSysMenu",			bSortForSysMenu,1);
		IniFile.getValue("uint-plistUseFullPaths",		bPlistUseFullPaths);
		IniFile.getValue("uint-plistUse2Columns",		bPlistUse2Columns,2);
		IniFile.getValue("uint-showCpuInTT",			bShowCPUinTT,1);
		IniFile.getValue("uint-logCPUHungers",			lLogCPUHungers);
		IniFile.getValue("uint-logCPUHungersFile",		sLogCPUHungers);
		IniFile.getValue("uint-listToKillSim",			sListToKillSim);
		IniFile.getValue("uint-sListToHideSim",			sListToHideSim);
		IniFile.getValue("uint-appToRWS",				sAppToRWS);
		IniFile.getValue("uint-appStopRWS",				lStopAppToRWS,1);
		IniFile.getValue("uint-showMemUsage",			bShowMemUsage,1);
		IniFile.getValue("uint-stickWindowsThreshold",	iStickPix,10);
		IniFile.getValue("uint-shutdownStrength",		lShutdownStrength,1);
		IniFile.getValue("uint-shutdownKillBefore",		sKillBeforeShutdown);
		IniFile.getValue("uint-popupRecentQRun",		bPopupRecentlyUsed,1);
		IniFile.getValue("uint-retrieveSysMenu",		bRetrieveSysMenu);
		IniFile.getValue("uint-bossKeyAction",			lBosskeyAction);
		IniFile.getValue("uint-bossKeyAction2",			lBosskeyAction2,1);
		IniFile.getValue("uint-bossKeyMask",			sBossAppMask);
		IniFile.getValue("uint-bossKeyMask2",			sBossAppMask2);
		IniFile.getValue("uint-defaultFind",			sDefaultFind);
		IniFile.getValue("uint-bossKeyScreenshot",		lBossShotSwap);
		IniFile.getValue("uint-layoutPart1",			sLayoutP1);
		IniFile.getValue("uint-layoutPart2",			sLayoutP2);
		IniFile.getValue("uint-cdDriveLetter",			sCDDisk);
		IniFile.getValue("uint-cdDriveLetter2",			sCDDisk2);
		IniFile.getValue("uint-cdDriveAutoClose",		bAutoCloseCD,1);
		IniFile.getValue("uint-cdEjectOnShutdown",		bEjectCDOnShut,1);
		IniFile.getValue("uint-cdDriveCloseTime",		lAutoCloseCDTime,DEF_CDCLOSETIME);
		IniFile.getValue("uint-volumeStep",				lVolumeCStep,10);
		IniFile.getValue("uint-volumeWStep",			lVolumeWStep,10);
		IniFile.getValue("uint-volumeBefShut",			lVolumeBeforeShutdown);
		IniFile.getValue("uint-volumeBefShutLevel",		lVolumeBeforeShutdownLevel,20);
		IniFile.getValue("uint-volumeAfterStart",		lVolumeAfterStart);
		IniFile.getValue("uint-volumeAfterStartLevel",	lVolumeAfterStartLevel,20);
		IniFile.getValue("uint-volumeOSDNeed",			lVolumeNeedOSD,1);
		IniFile.getValue("uint-winampOSDNeed",			lWinAmpNeedOSD,1);
		IniFile.getValue("uint-noNeedOSD",				lNoNeedOSD);
		IniFile.getValue("uint-swdeskOSDNeed",			lDeskNeedOSD,1);
		IniFile.getValue("uint-memoptOSDNeed",			lDeskMemOptOSD,1);
		IniFile.getValue("uint-OSDFade",				lDeskfadeOSD,objSettings.bUnderWindows98?0:1);
		IniFile.getValue("uint-OSDBorder",				lOSDBorder,1);
		IniFile.getValue("uint-secsOSD",				lOSDSecs,DEF_OSDSECS);
		IniFile.getValue("uint-charsOSD",				lOSDChars,DEF_OSDCHARS);

		IniFile.getValue("uint-layChangeEffect",		lLayEffectType);
		{// Грузим раскладки
			for(int i=0;i<=MAX_HKL;i++){_XLOG_
				if(objSettings.aKBLayouts[i]>0){_XLOG_
					IniFile.getValue(Format("hkl-hotkey%lu",i),	aKBLayoutKeys[i]);
				}else{_XLOG_
					break;
				}
			}
		}
		//
		CString sSplashInfo, sSplFN="splash";
		DWORD dwTxCol=DEF_SPLASHTEXTCOL,dwBgCol=DEF_SPLASHBGCOL;
		IniFile.getValue("uint-splashTextInfo",			sSplashInfo);
		if(pSplashFont){_XLOG_
			delete pSplashFont;
			pSplashFont=NULL;
		}
		pSplashFont=CreateFontFromStr(sSplashInfo, &sSplFN, &dwBgCol, &dwTxCol);
		lSplashTextColor=dwTxCol;
		lSplashBgColor=dwBgCol;
		//
		memset(DefActionMapHK,0,sizeof(DefActionMapHK));
		for(int j=0;j<(sizeof(DefActionMapHK)/sizeof(DefActionMapHK[0]));j++){_XLOG_
			IniFile.getValue(Format("uint-hotkey%i",j),	DefActionMapHK[j],DefActionMapHK_Preinit[j]);
		}
		DefActionMapHK[RUN_DEFBROWSER]=CIHotkey();
		DefActionMapHK[RUN_DEFEMAILCL]=CIHotkey();
		DefActionMapHK[WS_SHOWSTART]=CIHotkey();
		DefActionMapHK[WS_SHOWUTILM]=CIHotkey();
		// Anti-Vista
		if(IsVistaSoundControl()){
			DefActionMapHK[SOUND_MUTE]=CIHotkey();
			DefActionMapHK[VOLUME_UP]=CIHotkey();
			DefActionMapHK[VOLUME_DOWN]=CIHotkey();
		}
		//
		IniFile.getValue("uint-clipCount",				lClipCount,3);
		lClipCountAfterNextStart=lClipCount;
		IniFile.getValue("uint-clipUseMaxLimit",		lUseMaxClipSizeLimit,1);
		IniFile.getValue("uint-clipMaxLimitKb",			lMaxClipSizeLimit,DEF_MAXCLIPSIZE);
		IniFile.getValue("uint-additionalClips",		bAddClipboard,1);
		IniFile.getValue("uint-showHPMTooltip",			lShowHPMTooltip,1);
		IniFile.getValue("uint-clipHistoryDepth",		lClipHistoryDepth,DEF_CLIPHIST);
		IniFile.getValue("uint-clipEnableTransmit",		lEnableTransmitMode,1);
		IniFile.getValue("uint-useRegexpsForClip",		bUseRegexps);
		IniFile.getValue("uint-regexpsForClipWhat",		sRegexpWhat);
		IniFile.getValue("uint-regexpsForClipTo",		sRegexpTo);
#ifdef WK_DEPRECATED
		lEnableTransmitMode=1;
		bUseRegexps=0;
#endif
		IniFile.getValue("uint-clipTransAppend",		lAppendTextToTransmits,1);
		IniFile.getValue("uint-clipTransAppendText",	sTransmitAppendText,DEF_TRANSMITAPP);
		IniFile.getValue("uint-clipSaveBMPSinHist",		lSaveBMPSinHist,1);
		IniFile.getValue("uint-lLeaveInClipOnPaste",	lLeaveInClipOnPaste);
		IniFile.getValue("uint-clipMaxBMPHistSize",		lMaxBMPHistSize,MAX_BMPCLIP_SIZE_SAVE);
		IniFile.getValue("uint-clipMaxBMPHistSizeC",	lMaxBMPHistSizeCatch,MAX_BMPCLIP_SIZE);
		IniFile.getValue("uint-sCodeWord",				sCodeWord);
		IniFile.getValue("uint-safeFileNameChar",		sSafeChar);
		IniFile.getValue("uint-allowHPrior",			bAllowHighPriority);
		IniFile.getValue("uint-bDskTutorial",			bDskTutorial);
		
		//-----------------
		IniFile.getValue("uint-desktopIconsRestore",		lRestoreDeskIconsPos);
		IniFile.getValue("uint-desktopIconsDisAlert",		lDisableDeskicoAlert);
		IniFile.getValue("uint-desktopIconsPreserveRes",	lRestorePreservRes,1);
		IniFile.getValue("uint-addDeskEnable",				lEnableAddDesktops);
		IniFile.getValue("uint-addDeskRememIcons",			lRemeberIconsPosInDesktops,1);
		IniFile.getValue("uint-addDeskRememResol",			lRemeberResolutInDesktops);
		IniFile.getValue("uint-addDeskRememShell",			lRemeberShellInDesktops);
		IniFile.getValue("uint-desktopCount",				lDesktopSwitches,3);
		
		if(objSettings.bUnderWindows98){_XLOG_
			lEnableAddDesktops=0;
		}
		IniFile.getValue("uint-useAllVModes",				lUseAllVModes);
		lDesktopSwitchesAfterNextStart=lDesktopSwitches;
		memset(DefDSwitchMapHK,0,sizeof(DefDSwitchMapHK));
		memset(DefDsSwMapHK_Preinit+3,0,sizeof(DefDsSwMapHK_Preinit)-3*sizeof(DefDsSwMapHK_Preinit[0]));
		for(int o2=0;o2<lDesktopSwitches;o2++){_XLOG_
			IniFile.getValue(Format("uint-desktopHkey%i",o2),DefDSwitchMapHK[o2],DefDsSwMapHK_Preinit[o2]);
			IniFile.getValue(Format("uint-desktop%i-ResX",o2),lResolutions[o2].m_dwWidth,lResolutions[0].m_dwWidth);
			IniFile.getValue(Format("uint-desktop%i-ResY",o2),lResolutions[o2].m_dwHeight,lResolutions[0].m_dwHeight);
			IniFile.getValue(Format("uint-desktop%i-ResFQ",o2),lResolutions[o2].m_dwFrequency,lResolutions[0].m_dwFrequency);
			IniFile.getValue(Format("uint-desktop%i-ResBPP",o2),lResolutions[o2].m_dwBitsPerPixel,lResolutions[0].m_dwBitsPerPixel);
			IniFile.getValue(Format("uint-desktop%i-Shell",o2),lDeskShells[o2],"explorer.exe");
			lChoosenResolutions[o2]=0;
		}
		IniFile.getValue("uint-activePlugins",				sActivePlugins,DEF_PLUGINS);
	}
	LoadQRuns(szConfigFile);
	LoadHistories(szConfigFile);
	if(!bRestoreMode){_XLOG_
		LoadDeskLayout();
		// Грузим плагины
		GetPluginsDirectory()=CString(GetApplicationDir())+"Plugins\\";
		if(!isFileExist(GetPluginsDirectory())){_XLOG_
			GetPluginsDirectory()=GetApplicationDir();
		}
/*		
		// длл?
		//FILETIME fd1;
		//FILETIME fd2;
		//GetFileDateTime(GetPluginsDirectory()+"WKUtils.dll",&fd1);
		//GetFileDateTime(GetAppFolder()+"WKUtils.dll",&fd2);
		//if(memcmp(&fd1,&fd2,sizeof(fd1))!=0){_XLOG_
		if(GetFileSize(GetPluginsDirectory()+"WKUtils.dll")!=GetFileSize(GetAppFolder()+"WKUtils.dll")){_XLOG_
			CFileInfoArray dir;
			dir.AddDir(GetAppFolder(),"*.dll",FALSE,0,FALSE);
			for(int i=0;i<dir.GetSize();i++){_XLOG_
				CString sNewPath=GetPluginsDirectory()+dir[i].GetFileName();
				DeleteFile(sNewPath);
				CopyFile(dir[i].GetFilePath(),sNewPath,TRUE);
			}
		}
*/
		
		LoadPlugins();
		// Смотрим, может нужна буква CD-Disk-а по умолчанию
		if(objSettings.sCDDisk.IsEmpty()){_XLOG_
			char dr[32]="D:\\";
			for (; dr[0] <= 'Z'; dr[0]++){_XLOG_
				if(GetDriveType(dr) == DRIVE_CDROM){_XLOG_
					objSettings.sCDDiskDefault=dr[0];
					objSettings.sCDDisk=objSettings.sCDDiskDefault;
					break;
				}
			}
		}
	}
	//-------------------------- Открываем мьютекс активности WK ---------------
	OpenCurrentDeskAtom(1);
	objSettings.hUpAndRunningMutex=::CreateMutex( NULL, TRUE, "WIREKEYS_ACTIVE");
	//-----------------------------------------
	// Все считано
	bSaveAllowed=1;
	ApplySettings();
	return;
}

void CSettings::LoadDeskLayout(BOOL bDoNotRestore)
{_XLOG_
	if(!bDoNotRestore && (lRestoreDeskIconsPos || (objSettings.lStartupDesktop!=0 && objSettings.lRemeberIconsPosInDesktops && objSettings.lEnableAddDesktops))){_XLOG_
		if(RestoreDesktopIcons()==-1){_XLOG_
			if(objSettings.lStartupDesktop==0){_XLOG_
				SaveDesktopIcons();
			}
		}
	}
}

void CSettings::LoadHistories(const char* szInFile)
{_XLOG_
	CString sHeader="histories";
	CString sConfFile=szInFile;
	CString sClipDataFolder;
	if(sConfFile==""){_XLOG_
		sClipDataFolder=getFileFullName("Histories",TRUE);
		CreateDirIfNotExist(sClipDataFolder);
		sConfFile=sClipDataFolder+"histories.conf";
	}
	CDataXMLSaver IniFile(sConfFile,sHeader,TRUE);
	// История запусков
	{_XLOG_
		sClipBuffer.RemoveAll();
		sClipBuffer.SetSize(lClipCount+1);
		memset(DefClipMapHK_H,0,sizeof(DefClipMapHK_H));
		memset(DefClipMapHK_H_Preinit+5,0,sizeof(DefClipMapHK_H_Preinit)-5*sizeof(DefClipMapHK_H_Preinit[0]));
		memset(DefClipMapHK_HA,0,sizeof(DefClipMapHK_HA));
		memset(DefClipMapHK_HA_Preinit+5,0,sizeof(DefClipMapHK_HA_Preinit)-5*sizeof(DefClipMapHK_HA_Preinit[0]));
		for(int a=0;a<=lClipCount;a++){_XLOG_
			CString sClipContent;
			if(sClipDataFolder!=""){_XLOG_
				ReadFile(sClipDataFolder+Format("clip-hist-%i.clp",a),sClipContent);
			}
			IniFile.getValue(Format("clip-hist-hkey%i-main",a),DefClipMapHK_H[a], DefClipMapHK_H_Preinit[a]);
			IniFile.getValue(Format("clip-hist-hkey%i-main-app",a),DefClipMapHK_HA[a], DefClipMapHK_HA_Preinit[a]);
			sClipBuffer[a]=sClipContent;
		}
		memset(DefClipMapHK,0,sizeof(DefClipMapHK));
		memset(DefClipMapHK_Preinit+10,0,sizeof(DefClipMapHK_Preinit)-10*sizeof(DefClipMapHK_Preinit[0]));
		for(int o=0;o<(lClipCount+1)*2;o++){_XLOG_
			IniFile.getValue(Format("clip-hist-hkey%i",o),DefClipMapHK[o], DefClipMapHK_Preinit[o]);
		}
		if(sClipDataFolder!=""){_XLOG_
			ReadFile(sClipDataFolder+"clip-hist-default.clp",				sSavedClipContent);
		}
	}
	int iHistCount=1;
	while(1){_XLOG_
		CString sName;
		if(!IniFile.getValue(Format("clip%lu-name",iHistCount),sName)){_XLOG_
			break;
		}
		int iPieceCount=0;
		CStringArrayX hist;
		while(1){_XLOG_
			CString sValue,sValueDsc;
			if(!IniFile.getValue(Format("clip%lu-hist-%i-dsc",iHistCount,iPieceCount),sValueDsc)){_XLOG_
				break;
			}
			if(sClipDataFolder!=""){_XLOG_
				ReadFile(sClipDataFolder+Format("clip%lu-hist-%i.clp",iHistCount,iPieceCount),sValue);
			}
			CPairItem item;
			item.sItem=sValue;
			IniFile.getValue(Format("clip%lu-hist-%i-dsc",iHistCount,iPieceCount),item.sClipItemDsc);
			IniFile.getValue(Format("clip%lu-hist-%i-app",iHistCount,iPieceCount),item.sApp);
			hist.SetAtGrow(iPieceCount,item);
			iPieceCount++;
		}
		sClipHistory.SetAt(sName,hist);
		iHistCount++;
	}
	int iCookieCount=0;
	getMCookies().RemoveAll();
	while(1){_XLOG_
		CString sName;
		CString sValue;
		IniFile.getValue(Format("cookie%lu-name",iCookieCount),sName);
		IniFile.getValue(Format("cookie%lu-value",iCookieCount),sValue);
		if(sName=="" && sValue==""){_XLOG_
			break;
		}
		getMCookies().SetAt(sName,sValue);
		iCookieCount++;
	}
}

void CSettings::LoadQRuns(const char* szInFile,BOOL bAppendMode)
{_XLOG_
	CString sHeader="quick-runs";
	CString sConfFile=szInFile;
	if(sConfFile==""){_XLOG_
		sConfFile=getFileFullName("quickruns.conf");
	}
	if(!isFileExist(sConfFile)){_XLOG_
		sConfFile=getFileFullName(CString(PROGNAME)+".conf");
		if(atol(objSettings.sPrevVersion)<2){_XLOG_
			sHeader="application-config";
		}
	}
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	CDataXMLSaver IniFile(sConfFile,sHeader,TRUE);
	if(!bAppendMode){_XLOG_
		DeleteQRun();
		IniFile.getValue("uint-quickRunSplash",				lShowSplashForQuickRun,1);
	}
	int p=0;
	CString sQAppData="None";
	while(sQAppData!=""){_XLOG_
		if(!IniFile.getValue(Format("uint-quickrun%i",p), sQAppData) || sQAppData==""){_XLOG_
			break;
		}
		CDataXMLSaver appData(sQAppData);
		CQuickRunItem* appItem=new CQuickRunItem;
		if(!bAppendMode){_XLOG_
			CString sNewUniqueID;
			appData.getValue("id",sNewUniqueID);
			if(sNewUniqueID!=""){_XLOG_
				appItem->sUniqueID=sNewUniqueID;
			}
		}
		appData.getValue("name",appItem->sItemName);
		appData.getValue("category",appItem->sCategoryID);
		appData.getValue("isMacros",appItem->lMacros);
		appData.getValue("path",appItem->sItemPath);
		appData.getValue("startupdir",appItem->sStartupDir);
		appData.getValue("app-desk",appItem->lStartInDesktop);
		appData.getValue("params",appItem->sItemParams);
		appData.getValue("par-type",appItem->lParamType);
		appData.getValue("dsc",appItem->sDsc);
		appData.getValue("stype",appItem->lAfterStart);
		appData.getValue("run-atstart",appItem->lStartAtStartUp);
		appData.getValue("showin-upmenu",appItem->lShowInUpMenu);
		appData.getValue("search-instance",appItem->lSearchInstanceFirst);
		appData.getValue("auto-expand-mask",appItem->sAutoExpandMask);
		appData.getValue("auto-create",appItem->lAutoCreate);
		appData.getValue("howto-showintray",appItem->lHowToShowInTray,1);
		appData.getValue("howto-checkcontent",appItem->lExpandFolderOrCheckInstance);
		appData.getValue("use-detachedIcon",appItem->lUseDetachedPresentation);
		appData.getValue("sttime",appItem->lStartupTime,DEF_STARTUPTIME);
		appData.getValue("fix-opacity",appItem->lOpacity,100);
		appData.getValue("fix-position",appItem->lFixPostion);
		appData.getValue("x-postion",appItem->lFixedXPos);
		appData.getValue("y-postion",appItem->lFixedYPos);
		appData.getValue("fix-size",appItem->lFixSize);
		appData.getValue("fix-width",appItem->lFixedWidth,theApp.rDesktopRECT.Width());
		appData.getValue("fix-height",appItem->lFixedHeight,theApp.rDesktopRECT.Height());
		// Создаем по-любому 0ой элемент...
		appItem->RemoveAllQRunHotkeys();
		appItem->qrHotkeys.SetAtGrow(0,new CQRunHotkeyItem());
		appData.getValue("hotkey",appItem->qrHotkeys[0]->oHotKey);
		int ihkCount=1;
		while(1){_XLOG_
			CQRunHotkeyItem hkItem;
			if(appData.getValue(Format("hotkey%lu",ihkCount),hkItem.oHotKey)){_XLOG_
				appItem->qrHotkeys.Add(new CQRunHotkeyItem(hkItem));
			}else{_XLOG_
				break;
			}
			ihkCount++;
		}
		//appData.getValue("useaddpars",appItem->lUseAdditionalOptions);
		AddQrunT(appItem);
		p++;
	}
	return;
}

void CSettings::Save(const char* szConfigFile)
{_XLOG_
	TRACETHREAD;
	if(lSkipSavings){
		return;
	}
	BOOL bRestoreMode=TRUE;
	{_XLOG_
		// Тут можно потимизировать по идее...
		// Все это сохраняется каждый раз при выключении!
		CString sConfFile=szConfigFile;
		if(sConfFile==""){_XLOG_
			bRestoreMode=FALSE;
			sConfFile=getFileFullName(CString(PROGNAME)+".conf");
		}
		if(bSaveAllowed==-1 || lStartupDesktop!=0){_XLOG_
			return;
		}
#ifndef _DEBUG
		if(isFileExist(sConfFile) && objSettings.iLikStatus==1){_XLOG_
			return;
		}
#endif // _DEBUG
		CDataXMLSaver IniFile(sConfFile,"application-config",FALSE);
		// UINT
		IniFile.putValue("uint-language",				GetApplicationLang());
		IniFile.putValue("cur-version",					PROG_VERSION);
		IniFile.putValue("uint-regStatus",				iLikStatus);
		IniFile.putValue("uint-integrWN",				bIntegrateWithWN);
		IniFile.putValue("uint-checkForUpdates",		lCheckForUpdate);
		IniFile.putValue("uint-checkForUpdDays",		lDaysBetweenChecks);
		IniFile.putValue("uint-askForDefMacros",		lDefaultMacros);
		IniFile.putValue("uint-userTrayIcon",			sUDIcon);
		IniFile.putValue("uint-defOptionsTopic",		sDefOptionsTopic);
		//
		IniFile.putValue("uint-scrshotFile",			sScrshotFile);
		IniFile.putValue("uint-scrshotDir",				sScrshotDir);
		IniFile.putValue("uint-scrshotDir2",			sScrshotDir2);
		IniFile.putValue("uint-scrshotActiveWnd",		bShotActiveWindow);
		IniFile.putValue("uint-scrshotActiveWnd2",		bShotActiveWindow2);
		IniFile.putValue("uint-scrshotBAWConfirm",		bBlackAndWhiteConfirm);
		IniFile.putValue("uint-scrshotSign",			bSignSShot);
		IniFile.putValue("uint-scrshotJpg",				bUseJpg);
		IniFile.putValue("uint-scrshotJpgQuality",		bUseJpgQuality);
		IniFile.putValue("uint-scrshotPutInClip",		bPutScreshotInClipboard);
		IniFile.putValue("uint-autoEditShot",			bAutoEditOnShot);
		IniFile.putValue("uint-autoEditShotEd",			sJpgEditor);
		IniFile.putValue("uint-scrshotSignStr",			sSignString);
		IniFile.putValue("uint-scrshotFixX",			bFixXSize);
		IniFile.putValue("uint-scrshotFixY",			bFixYSize);
		IniFile.putValue("uint-scrshotFixXTo",			lFixXSizeTo);
		IniFile.putValue("uint-scrshotFixYTo",			lFixYSizeTo);
		IniFile.putValue("uint-scrshotLastFile",		sLastScreenshot);
		IniFile.putValue("uint-scrshotByTimer",			lScreenshotByTimer);
		IniFile.putValue("uint-scrshotByTimerSec",		lScreenshotByTimerSec);
		IniFile.putValue("uint-scrshotAddCursor",		lAddCursorToScreenshot);
		IniFile.putValue("uint-scrshotShowIntInfo",		lShowScrData);
		IniFile.putValue("uint-startupMsgType",			lStartupMsgType);
		IniFile.putValue("uint-showIconInTray",			bShowIconInTray);
		IniFile.putValue("uint-showTextInTray",			lShowDateInTrayIcon);
		IniFile.putValue("uint-showCPUHistInTray",		lShowCPUHistInTray);
		IniFile.putValue("uint-showCPUHistColumns",		lShowCPUHistInTrayC);
		IniFile.putValue("uint-showCPUInTray",			lShowCPUInTrayIcon);
		IniFile.putValue("uint-osdDateFormat",			sODSDateFormat);
		IniFile.putValue("uint-osdTimeFormat",			sODSTimeFormat);
		IniFile.putValue("uint-statMous",				lStat_mouseLen);
		IniFile.putValue("uint-statKeyb",				lStat_keybCount);
		IniFile.putValue("uint-statHotk",				lStat_hotkCount);
		IniFile.putValue("uint-useOldStartStyle",		lUseOldStartStyle);
		IniFile.putValue("uint-showCPUInTrayRefr",		lShowCPUInTraySecs);
		IniFile.putValue("uint-exitWndsOnDbl",			lExitWndOnDbl);
		IniFile.putValue("uint-skipHKTrayWarning",		bSkipHKWarning);
		IniFile.putValue("uint-noNotifyByCursor",		lNoNotifyByCursor);
		IniFile.putValue("uint-skipTrayHelpItems",		lDisableSCHelp);
		IniFile.putValue("uint-skipFocusMove",			lDisableFocusMove);
		//IniFile.putValue("uint-alertToTaskbar",			lAlertsOnTaskbar);
		IniFile.putValue("uint-noHotkeyDblPress",		lDisableDblPressExtensions);
		IniFile.putValue("uint-backupFile",				sBackupFile);
		IniFile.putValue("uint-showWinAmpTitle",		lShowWinampTitle);
		IniFile.putValue("uint-pathWinAmpFile",			sWinAmpStartPas);
		IniFile.putValue("uint-stripWinAmpStuffTitle",	lStripNumberWinampTitle);
		IniFile.putValue("uint-addWinAmpTTTitle",		lAddTTWinampTitle);
		IniFile.putValue("uint-addWinAmpTLTitle",		lAddTLWinampTitle);
		IniFile.putValue("uint-stripWinAmpStuffATitle",	lStripAllWinampTitle);
		IniFile.putValue("uint-showWTitleInTooltip",	lShowWTitInTooltip);
		IniFile.putValue("uint-catchWinAmpTitle",		lCatchWinampTitle);
		IniFile.putValue("uint-showWinAmpOSDSecs",		lWinampOSDTime);
		IniFile.putValue("uint-stopWinampOnScrsave",	lStopWinAmpOnScrsave);
		IniFile.putValue("uint-stopWinampOnLock",		lStopWinAmpOnLock);
		IniFile.putValue("uint-startWinampAfLock",		lStartWinAmpAfterLock);
		IniFile.putValue("uint-optimizeRegistry",		lOptimizeRegistryON);
		IniFile.putValue("uint-setRandomScreensaver",	lSetRandomScreensaver);
		IniFile.putValue("uint-setRandomScrSavValids",	sValidScreenSavers);
		IniFile.putValue("hkl-prefLayout1",				lHKL1);
		IniFile.putValue("hkl-prefLayout2",				lHKL2);
		//
		IniFile.putValue("uint-disableClipAtAll",		lDisableClipboard);
		IniFile.putValue("uint-chTrayOnClips",			lChTrayOnClips);
		IniFile.putValue("uint-getSelectedType",		bGetTextSelectionStyle);
		//IniFile.putValue("uint-getSelectedQual",		lDefaultSelQuality);
		IniFile.putValue("uint-saveClipContent",		lSaveClipContent);
		IniFile.putValue("uint-saveClipContentPer",		lSaveClipContentPer);
		IniFile.putValue("uint-lLastWordNotLine",		lLastWordNotLine);
		IniFile.putValue("uint-extractIconsForSysMenu",	bExtractIconsForSysMenu);
		IniFile.putValue("uint-sortForSysMenu",			bSortForSysMenu);
		IniFile.putValue("uint-plistUse2Columns",		bPlistUse2Columns);
		IniFile.putValue("uint-plistUseFullPaths",		bPlistUseFullPaths);
		IniFile.putValue("uint-showCpuInTT",			bShowCPUinTT);
		IniFile.putValue("uint-logCPUHungers",			lLogCPUHungers);
		IniFile.putValue("uint-logCPUHungersFile",		sLogCPUHungers);
		IniFile.putValue("uint-listToKillSim",			sListToKillSim);
		IniFile.putValue("uint-sListToHideSim",			sListToHideSim);
		IniFile.putValue("uint-appToRWS",				sAppToRWS);
		IniFile.putValue("uint-appStopRWS",				lStopAppToRWS);
		IniFile.putValue("uint-showMemUsage",			bShowMemUsage);
		IniFile.putValue("uint-menuOnDblHot",			bMenuOnDblHotMenu);
		IniFile.putValue("uint-sysActionsInHotMenu",	bSysActionsInHotMenu);
		IniFile.putValue("uint-appShortcInHotMenu",		bAppShortcInHotMenu);
		IniFile.putValue("uint-screenSaverMenu",		bMiscSaverMenu);
		IniFile.putValue("uint-qrunMenu",				bQRunMenu);
		IniFile.putValue("uint-macroMenu",				bMacroMenu);
		IniFile.putValue("uint-categAllowDblStart",		lAllowDblCategStart);
		IniFile.putValue("uint-volumeMenu",				bVolumeMenu);
		IniFile.putValue("uint-sysAskForKill",			bAskForKill);
		IniFile.putValue("uint-sysAskForRemQ",			bAskForRemQ);
		IniFile.putValue("uint-textActionsInHotMenu",	bTextActionsInHotMenu);
		IniFile.putValue("uint-showMenuInHidedApp",		bHidedAppShowMenu);
		IniFile.putValue("uint-supportOwnerDrawMenus",	bSupportOwnerDraw);
		IniFile.putValue("uint-linesToScroolByHK",		lNumberOfLinesToScroll);
		IniFile.putValue("uint-htrayMenuRecentItems",	lHTrayRecentItems);
		IniFile.putValue("uint-htrayFloaterBG",			lFloaterBG);
		IniFile.putValue("uint-htrayFloaterSize",		sDetachedSize);
		IniFile.putValue("uint-htrayFloaterTransp",		lFloaterTransp);
		IniFile.putValue("uint-trayIconLBDClick",		lLBTray);
		IniFile.putValue("uint-trayIconMBDClick",		lMBTray);
		IniFile.putValue("uint-trayIconRBDClick",		lRBTray);
		IniFile.putValue("uint-showIconWhenMacro",		lShowMacroIcon);
		IniFile.putValue("uint-overlayTrayHidIcons",	bOverlayTrayAppIcon);
		IniFile.putValue("uint-animateFloaters",		bAnimateFloaters);
		IniFile.putValue("uint-freeMemofhiddenApp",		bFreeMemOfHidApp);
		IniFile.putValue("uint-takeComLineForFloater",	bTakeComLineForFloater);
		IniFile.putValue("uint-floatUseSingleClick",	lFloatUseSingleClick);
		IniFile.putValue("uint-floatUseLBDir",			lFloatUseLBDir);
		IniFile.putValue("uint-preserveTitleOnRollup",	bPreserveTitle);
		IniFile.putValue("uint-soundOnClipCopy",		bClipCopySound);
		IniFile.putValue("uint-soundOnClipPaste",		bClipPasteSound);
		IniFile.putValue("uint-soundOnClipCopyWav",		sClipCopyWav);
		IniFile.putValue("uint-soundOnClipPasteWav",	sClipPasteWav);
		IniFile.putValue("uint-soundOnLConvWav",		sOnLConvWav);
		IniFile.putValue("uint-soundOnLConv",			bOnLConv);
		IniFile.putValue("uint-soundSShotWav",			sSShotWav);
		IniFile.putValue("uint-soundSShot",				bSShot);
		IniFile.putValue("uint-soundCalc",				bSCalc);
		IniFile.putValue("uint-soundCalcWav",			sSCalcWav);
		IniFile.putValue("uint-soundHk",				bSHk);
		IniFile.putValue("uint-soundHkWav",				sSHkWav);
		IniFile.putValue("uint-soundOnErr",				bErrorSound);
		IniFile.putValue("uint-soundOnErrWav",			sErrorSound);
		IniFile.putValue("uint-soundDisableAll",		lDisableSounds);
		IniFile.putValue("uint-directMouseToUnderC",	lDirectMouseToUnderC);
		IniFile.putValue("uint-stickWindowsThreshold",	iStickPix);
		IniFile.putValue("uint-shutdownStrength",		lShutdownStrength);
		IniFile.putValue("uint-shutdownKillBefore",		sKillBeforeShutdown);
		IniFile.putValue("uint-popupRecentQRun",		bPopupRecentlyUsed);
		IniFile.putValue("uint-retrieveSysMenu",		bRetrieveSysMenu);
		IniFile.putValue("uint-bossKeyAction",			lBosskeyAction);
		IniFile.putValue("uint-bossKeyAction2",			lBosskeyAction2);
		IniFile.putValue("uint-bossKeyMask",			sBossAppMask);
		IniFile.putValue("uint-bossKeyMask2",			sBossAppMask2);
		IniFile.putValue("uint-defaultFind",			sDefaultFind);
		IniFile.putValue("uint-bossKeyScreenshot",		lBossShotSwap);
		IniFile.putValue("uint-useRegexpsForClip",		bUseRegexps);
		IniFile.putValue("uint-layoutPart1",			sLayoutP1);
		IniFile.putValue("uint-layoutPart2",			sLayoutP2);
		IniFile.putValue("uint-regexpsForClipWhat",		sRegexpWhat);
		IniFile.putValue("uint-regexpsForClipTo",		sRegexpTo);
		IniFile.putValue("uint-cdDriveLetter",			sCDDisk);
		IniFile.putValue("uint-cdDriveLetter2",			sCDDisk2);
		IniFile.putValue("uint-cdDriveAutoClose",		bAutoCloseCD);
		IniFile.putValue("uint-cdEjectOnShutdown",		bEjectCDOnShut);
		IniFile.putValue("uint-cdDriveCloseTime",		lAutoCloseCDTime);
		IniFile.putValue("uint-volumeStep",				lVolumeCStep);
		IniFile.putValue("uint-volumeWStep",			lVolumeWStep);
		IniFile.putValue("uint-volumeBefShut",			lVolumeBeforeShutdown);
		IniFile.putValue("uint-volumeBefShutLevel",		lVolumeBeforeShutdownLevel);
		IniFile.putValue("uint-volumeAfterStart",		lVolumeAfterStart);
		IniFile.putValue("uint-volumeAfterStartLevel",	lVolumeAfterStartLevel);
		IniFile.putValue("uint-volumeOSDNeed",			lVolumeNeedOSD);
		IniFile.putValue("uint-winampOSDNeed",			lWinAmpNeedOSD);
		IniFile.putValue("uint-noNeedOSD",				lNoNeedOSD);
		IniFile.putValue("uint-swdeskOSDNeed",			lDeskNeedOSD);
		IniFile.putValue("uint-memoptOSDNeed",			lDeskMemOptOSD);
		IniFile.putValue("uint-OSDFade",				lDeskfadeOSD);
		IniFile.putValue("uint-OSDBorder",				lOSDBorder);
		IniFile.putValue("uint-secsOSD",				lOSDSecs);
		IniFile.putValue("uint-charsOSD",				lOSDChars);
		IniFile.putValue("uint-splashTextInfo",			CreateStrFromFont(pSplashFont,"splash",lSplashBgColor,lSplashTextColor,100),XMLNoConversion);
		
		IniFile.putValue("uint-layChangeEffect",		lLayEffectType);
		{// СОхраняем раскладки
			for(int i=0;i<=MAX_HKL;i++){_XLOG_
				if(objSettings.aKBLayouts[i]>0){_XLOG_
					IniFile.putValue(Format("hkl-hotkey%lu",i),	aKBLayoutKeys[i]);
				}else{_XLOG_
					break;
				}
			}
		}
		//
		for(int j=0;j<(sizeof(DefActionMapHK)/sizeof(DefActionMapHK[0]));j++){_XLOG_
			IniFile.putValue(Format("uint-hotkey%i",j),	DefActionMapHK[j]);
		}
		IniFile.putValue("uint-additionalClips",		bAddClipboard);
		IniFile.putValue("uint-showHPMTooltip",			lShowHPMTooltip);
		IniFile.putValue("uint-clipCount",				lClipCountAfterNextStart);
		IniFile.putValue("uint-clipUseMaxLimit",		lUseMaxClipSizeLimit);
		IniFile.putValue("uint-clipMaxLimitKb",			lMaxClipSizeLimit);
		IniFile.putValue("uint-clipHistoryDepth",		lClipHistoryDepth);
		IniFile.putValue("uint-clipEnableTransmit",		lEnableTransmitMode);
		IniFile.putValue("uint-clipTransAppend",		lAppendTextToTransmits);
		IniFile.putValue("uint-clipTransAppendText",	sTransmitAppendText);
		IniFile.putValue("uint-clipSaveBMPSinHist",		lSaveBMPSinHist);
		IniFile.putValue("uint-lLeaveInClipOnPaste",	lLeaveInClipOnPaste);
		IniFile.putValue("uint-clipMaxBMPHistSize",		lMaxBMPHistSize);
		IniFile.putValue("uint-clipMaxBMPHistSizeC",	lMaxBMPHistSizeCatch);
		IniFile.putValue("uint-sCodeWord",				sCodeWord);
		IniFile.putValue("uint-safeFileNameChar",		sSafeChar);
		IniFile.putValue("uint-allowHPrior",			bAllowHighPriority);
		IniFile.putValue("uint-bDskTutorial",			bDskTutorial);
		
		//-----------------
		IniFile.putValue("uint-desktopIconsRestore",		lRestoreDeskIconsPos);
		IniFile.putValue("uint-desktopIconsDisAlert",		lDisableDeskicoAlert);
		IniFile.putValue("uint-desktopIconsPreserveRes",	lRestorePreservRes);
		IniFile.putValue("uint-addDeskEnable",				lEnableAddDesktops);
		IniFile.putValue("uint-addDeskRememIcons",			lRemeberIconsPosInDesktops);
		IniFile.putValue("uint-addDeskRememResol",			lRemeberResolutInDesktops);
		IniFile.putValue("uint-addDeskRememShell",			lRemeberShellInDesktops);
		IniFile.putValue("uint-desktopCount",				lDesktopSwitchesAfterNextStart);
		for(int o2=0;o2<lDesktopSwitches;o2++){_XLOG_
			IniFile.putValue(Format("uint-desktopHkey%i",o2),DefDSwitchMapHK[o2]);
			IniFile.putValue(Format("uint-desktop%i-ResX",o2),lResolutions[o2].m_dwWidth);
			IniFile.putValue(Format("uint-desktop%i-ResY",o2),lResolutions[o2].m_dwHeight);
			IniFile.putValue(Format("uint-desktop%i-ResFQ",o2),lResolutions[o2].m_dwFrequency);
			IniFile.putValue(Format("uint-desktop%i-ResBPP",o2),lResolutions[o2].m_dwBitsPerPixel);
			IniFile.putValue(Format("uint-desktop%i-Shell",o2),lDeskShells[o2]);
		}
		IniFile.putValue("uint-useAllVModes",				lUseAllVModes);
		IniFile.putValue("uint-activePlugins",				sActivePlugins);
	}
	// а это должно сохраняться всегда
	SaveHistories(szConfigFile);
	SaveQRuns(szConfigFile);
	return;
}

void CSettings::SaveHistories(const char* szOutFile)
{_XLOG_
	CString sHeader="histories";
	CString sConfFile=szOutFile;
	CString sClipDataFolder;
	BOOL bAppendMode=TRUE;
	if(sConfFile==""){_XLOG_
		bAppendMode=FALSE;
		sClipDataFolder=getFileFullName("Histories",TRUE);
		CreateDirIfNotExist(sClipDataFolder);
		sConfFile=sClipDataFolder+"histories.conf";
	}
	CDataXMLSaver IniFile(sConfFile,sHeader,FALSE,bAppendMode);
	// История запусков
	{_XLOG_
		int iClipSaveCount=0;
		int iCountOfClipsToSave=min(lClipCount+1,sClipBuffer.GetSize());
		for(int a=0;a<iCountOfClipsToSave;a++){_XLOG_
			BOOL bSkipSave=0;
			BOOL blUseMaxClipSizeLimit=lUseMaxClipSizeLimit;
			if(sClipBuffer[a].Find(BMP_SPECPREFIX)!=-1){_XLOG_
				blUseMaxClipSizeLimit=0;
				if(!lSaveBMPSinHist || (long)strlen(sClipBuffer[a])>lMaxBMPHistSize){_XLOG_
					sClipBuffer[a]=_l("Big bitmap - not saved");
				}
			}
			if(sClipDataFolder!=""){_XLOG_
				SaveFile(sClipDataFolder+Format("clip-hist-%i.clp",a),blUseMaxClipSizeLimit?sClipBuffer[a].Left(lMaxClipSizeLimit*1024):sClipBuffer[a]);
			}
			IniFile.putValue(Format("clip-hist-hkey%i-main",a),DefClipMapHK_H[a]);
			IniFile.putValue(Format("clip-hist-hkey%i-main-app",a),DefClipMapHK_HA[a]);
		}
		for(int o=0;o<(iCountOfClipsToSave+1)*2;o++){_XLOG_
			IniFile.putValue(Format("clip-hist-hkey%i",o),DefClipMapHK[o]);
		}
		if(objSettings.lSaveClipContent){_XLOG_
			WCHAR* wszText=GetClipboardText(FALSE,TRUE);
			if(wszText){_XLOG_
				if(IsBMP(wszText)){_XLOG_
					if(!lSaveBMPSinHist || (long)strlen(objSettings.sSavedClipContent)>lMaxBMPHistSize){_XLOG_
						objSettings.sSavedClipContent=_l("Big bitmap - not saved");
					}else{_XLOG_
						objSettings.sSavedClipContent=(const char*)(wszText);
					}
				}else{_XLOG_
					if(wcslen(wszText)>size_t(lMaxClipSizeLimit*1024)){_XLOG_
						//objSettings.sSavedClipContent=_l("Big text block - not saved");
						WideCharToMultiByte(CP_ACP, 0, wszText, -1, objSettings.sSavedClipContent.GetBufferSetLength(lMaxClipSizeLimit*1024), lMaxClipSizeLimit*1024-1, NULL, NULL);
						objSettings.sSavedClipContent.ReleaseBuffer();
					}else{_XLOG_
						//USES_CONVERSION;
						//objSettings.sSavedClipContent=W2A(wszText);
						WideCharToMultiByte(CP_ACP, 0, wszText, -1, objSettings.sSavedClipContent.GetBufferSetLength(wcslen(wszText)), wcslen(wszText), NULL, NULL);
						objSettings.sSavedClipContent.ReleaseBuffer();
					}
				}
				delete[] wszText;
			}
		}
		if(sClipDataFolder!=""){_XLOG_
			SaveFile(sClipDataFolder+"clip-hist-default.clp",				sSavedClipContent);
		}
	}
	int iHistCount=1;
	POSITION pos=sClipHistory.GetStartPosition();
	while(pos){_XLOG_
		CString sName;
		CStringArrayX hist;
		sClipHistory.GetNextAssoc(pos,sName,hist);
		IniFile.putValue(Format("clip%lu-name",iHistCount),sName);
		for(int b=0;b<hist.GetSize();b++){_XLOG_
			BOOL bSkipSave=0;
			BOOL blUseMaxClipSizeLimit=lUseMaxClipSizeLimit;
			if(hist[b].sItem.Find(BMP_SPECPREFIX)!=-1){_XLOG_
				blUseMaxClipSizeLimit=0;
				if(!lSaveBMPSinHist || (long)strlen(hist[b].sItem)>lMaxBMPHistSize){_XLOG_
					hist[b].sItem=_l("Big bitmap - not saved");
				}
			}
			if(sClipDataFolder!=""){_XLOG_
				SaveFile(sClipDataFolder+Format("clip%lu-hist-%i.clp",iHistCount,b),blUseMaxClipSizeLimit?hist[b].sItem.Left(lMaxClipSizeLimit*1024):hist[b].sItem);
			}
			IniFile.putValue(Format("clip%lu-hist-%i-dsc",iHistCount,b),hist[b].sClipItemDsc);
			IniFile.putValue(Format("clip%lu-hist-%i-app",iHistCount,b),hist[b].sApp);
		}
		iHistCount++;
	}
	int iCookieCount=0;
	pos=getMCookies().GetStartPosition();
	while(pos){_XLOG_
		CString sName;
		CString sValue;
		getMCookies().GetNextAssoc(pos,sName,sValue);
		IniFile.putValue(Format("cookie%lu-name",iCookieCount),sName);
		IniFile.putValue(Format("cookie%lu-value",iCookieCount),sValue);
		iCookieCount++;
	}
}

void CSettings::SaveQRuns(const char* szOutFile)
{_XLOG_
	CString sHeader="quick-runs";
	CString sConfFile=szOutFile;
	BOOL bAppendMode=TRUE;
	if(sConfFile==""){_XLOG_
		bAppendMode=FALSE;
		sConfFile=getFileFullName("quickruns.conf");
	}
	CDataXMLSaver IniFile(sConfFile,sHeader,FALSE,bAppendMode);
	int iPC=0;
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	for(int p=0;p<aQuickRunApps.GetSize();p++){_XLOG_
		if(aQuickRunApps[p]){_XLOG_
			CString sQAppData;
			CDataXMLSaver appData(sQAppData,"",FALSE);
			CQuickRunItem* appItem=aQuickRunApps[p];
			appData.putValue("id",appItem->sUniqueID);
			appData.putValue("name",appItem->sItemName);
			appData.putValue("category",appItem->sCategoryID);
			appData.putValue("isMacros",appItem->lMacros);
			appData.putValue("path",appItem->sItemPath);
			appData.putValue("startupdir",appItem->sStartupDir);
			appData.putValue("app-desk",appItem->lStartInDesktop);
			appData.putValue("params",appItem->sItemParams);
			appData.putValue("par-type",appItem->lParamType);
			appData.putValue("run-atstart",appItem->lStartAtStartUp);
			appData.putValue("showin-upmenu",appItem->lShowInUpMenu);
			appData.putValue("search-instance",appItem->lSearchInstanceFirst);
			appData.putValue("auto-expand-mask",appItem->sAutoExpandMask);
			appData.putValue("auto-create",appItem->lAutoCreate);
			appData.putValue("howto-showintray",appItem->lHowToShowInTray);
			appData.putValue("howto-checkcontent",appItem->lExpandFolderOrCheckInstance);
			appData.putValue("use-detachedIcon",appItem->lUseDetachedPresentation);
			appData.putValue("dsc",appItem->sDsc);
			appData.putValue("stype",appItem->lAfterStart);
			appData.putValue("sttime",appItem->lStartupTime);
			appData.putValue("fix-opacity",appItem->lOpacity);
			appData.putValue("fix-position",appItem->lFixPostion);
			appData.putValue("x-postion",appItem->lFixedXPos);
			appData.putValue("y-postion",appItem->lFixedYPos);
			appData.putValue("fix-size",appItem->lFixSize);
			appData.putValue("fix-width",appItem->lFixedWidth);
			appData.putValue("fix-height",appItem->lFixedHeight);
			appData.putValue("hotkey",appItem->qrHotkeys[0]->oHotKey);
			for(int ihkCount=1;ihkCount<appItem->qrHotkeys.GetSize();ihkCount++){_XLOG_
				appData.putValue(Format("hotkey%lu",ihkCount),appItem->qrHotkeys[ihkCount]->oHotKey);
			}
			//appData.putValue("useaddpars",appItem->lUseAdditionalOptions);
			IniFile.putValue(Format("uint-quickrun%i",iPC), appData.GetResult(),XMLNoConversion);
			iPC++;
		}
	}
	IniFile.putValue("uint-quickRunSplash",				lShowSplashForQuickRun);
	return;
}

void CSettings::DeleteQRun()
{_XLOG_
	iGlobalState=6;
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	for(int p=0;p<aQuickRunApps.GetSize();p++){_XLOG_
		CQuickRunItem* pItem=aQuickRunApps[p];
		if(pItem){_XLOG_
			aQuickRunApps[p]=NULL;
			delete pItem;
		}
	}
	aQuickRunApps.RemoveAll();
}

void CSettings::Finalize()
{_XLOG_
	if(bRunStatus==-1){_XLOG_
		// Тут уже все было...
		return;
	}
	SaveAll();
	bRunStatus=-1;
	DeleteQRun();
	if(pSplashFont){_XLOG_
		delete pSplashFont;
		pSplashFont=0;
	}
	// Вырубаем плагины
	UnLoadPlugins();
	if(wk.g_lOverlayState!=0){_XLOG_
		wk.SwitchOverlaySupport(0, 0);
	}
	iGlobalState=7;
}

CSettings::~CSettings()
{_XLOG_
	Finalize();
	CloseHandle(hTimerEvent);
	TRACE("CSettings::~CSettings()\n");
};

void CSettings::ApplySettings()
{_XLOG_
	if(objSettings.bAllowHighPriority){_XLOG_
		HANDLE hThisProc=::OpenProcess(PROCESS_SET_INFORMATION,FALSE,GetCurrentProcessId());
		if (!( (hThisProc) == NULL || (hThisProc) == INVALID_HANDLE_VALUE )){_XLOG_
			SetPriorityClass(hThisProc,HIGH_PRIORITY_CLASS);
			CloseHandle(hThisProc);
		}
	}
	{// Заменяем неопределенные имена макросов/qrun
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			CQuickRunItem* pW=objSettings.aQuickRunApps[i];
			if(pW && wk.isUndefinedName(pW->sItemName)){_XLOG_
				char szFile[MAX_PATH]="";
				_splitpath(pW->sItemPath, NULL, NULL, szFile, NULL);
				pW->sItemName=szFile;
				if(pW->sItemName=="" || wk.isUndefinedName(pW->sItemName)){_XLOG_
					if(!wk.isUndefinedName(pW->sItemPath)){_XLOG_
						pW->sItemName=pW->sItemPath.SpanExcluding(" .,:;");
						pW->sItemName.Replace("\"","");
						pW->sItemName.Replace("'","");
					}else{_XLOG_
						pW->sItemName=_l("Quick-run item");
					}
				}
			}
		}
	}
	MaxBmpSize()=objSettings.lMaxBMPHistSizeCatch;
}

void CSettings::SaveAll()
{_XLOG_
	__FLOWLOG
	if(bSaveAllowed!=1){_XLOG_
		return;
	}
	iGlobalState=8;
	HCURSOR hCur=SetCursor(theApp.LoadCursor(IDC_CURSORSAVE));
	this->Save();
	SetCursor(hCur);
}

void GetDesktopInternalName(int iDesk, char szDesktopID[DEF_DESKNAMESIZE])
{_XLOG_
	if(iDesk==0){_XLOG_
		sprintf(szDesktopID,"Default");
	}else{_XLOG_
		sprintf(szDesktopID,"WIREKEYS_D%lu",iDesk);
	}
}

void InstallDefaultMacros()
{_XLOG_
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	if(objSettings.lDefaultMacros==2){_XLOG_
		return;
	}
	wk.AddDefMacros(HIROW(1),0);
	objSettings.lDefaultMacros=2;
}

BOOL CALLBACK RegNow(void* Param, int iButtonNum)
{_XLOG_
	if(iButtonNum==IDOK){_XLOG_
		wk.BuyWK(NULL,NULL);
	}
	return TRUE;
}

void ShowRegNotice(CString sLertText,BOOL bForce)
{_XLOG_
	static DWORD dwTime=0;
	if(bForce==0 && GetCurrentTime()-dwTime<120000){_XLOG_
		//показываем всегда
		//return;
	}
	dwTime=GetCurrentTime();
	InfoWndStruct* info=new InfoWndStruct();
	info->pParent=NULL;
	info->sText=sLertText;
	info->sTitle=_l(PROGNAME": Registration");
	info->bCenterUnderMouse=TRUE;
	info->dwStaticPars=DT_CENTER|DT_VCENTER;
	info->sCancelButtonText=_l("Cancel");
	info->sOkButtonText=_l("Order now")+"!";
	InfoWnd* RegAlert=Alert(info);
	if(pMainDialogWindow && pMainDialogWindow->GetSafeHwnd()){_XLOG_
		RegAlert->SetCallback(RegNow,0);
	}
}

void CheckAndDisablePayFeaturesIfNeeded2()
{_XLOG_
	if(rnd(0,100)>90){_XLOG_
		CheckAndDisablePayFeaturesIfNeeded();
	}
}

void CheckAndDisablePayFeaturesIfNeeded()
{_XLOG_
	if(objSettings.iLikStatus==1){_XLOG_
		BOOL bNeddAlert=0;
		static DWORD dwLastWarnTime=GetTickCount();
		if(GetTickCount()-dwLastWarnTime>(20*60*60*1000)){_XLOG_
			/* 20часов */
			dwLastWarnTime=GetTickCount();
			bNeddAlert=1;
		}
		static int iDay=-1;
		if(iDay==-1 || COleDateTime::GetCurrentTime().GetDay()!=iDay || bNeddAlert){_XLOG_
			// Предупреждаем пользователя
			ShowRegNotice(DEFAULT_REGNOTICE2);
		}
		iDay=COleDateTime::GetCurrentTime().GetDay();
	}
}

DWORD WINAPI RunQRunInThread(LPVOID param);
DWORD WINAPI PerformTrayAction_InThread(LPVOID pData);
BOOL StartQRunByName(const char* szQRunTitle, const char* szAdditionalParameter, const char* szSelection)
{_XLOG_
	if(strstr(szQRunTitle,ADD_MACRO_PREF)==szQRunTitle){_XLOG_
		CString sFile=(szQRunTitle+strlen(ADD_MACRO_PREF));
		if(sFile.Find("~")!=-1){_XLOG_
			// Короткое имя
			char lpszPath[MAX_PATH]={0};
			if(GetLongPathName(sFile,lpszPath,MAX_PATH)!=0 && lpszPath[0]!=0){_XLOG_
				sFile=lpszPath;
			}
		}
		DWORD dwContinue=AskYesNo(_l("Do you want to install")+"\r\n"+_l("macro")+" "+GetPathPart(sFile,0,0,1,1)+"?",_l("Confirmation"),0,GetAppWnd());
		if(dwContinue==IDYES){_XLOG_
			CString sContent;
			ReadFile(sFile,sContent);
			if(sContent.Find("<quick-runs>")!=-1){_XLOG_
				objSettings.LoadQRuns(sFile,1);
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_ADDQRUNAPPLICATION,0);
			}else{_XLOG_
				CString sFileNew=Format("%sMacros\\%s",GetUserFolder(),GetPathPart(sFile,0,0,1,1));
				int iCount=1;
				while(isFileExist(sFileNew)){_XLOG_
					sFileNew=Format("%sMacros\\%s[%i]%s",GetUserFolder(),GetPathPart(sFile,0,0,1,0),iCount++,GetPathPart(sFile,0,0,0,1));
				}
				CopyFile(sFile,sFileNew,1);
				pMainDialogWindow->PostMessage(WM_COMMAND,ADDTOQUICKRUNS,LPARAM(strdup(sFileNew)));
			}
		}
		return TRUE;
	}
	if(szQRunTitle && szQRunTitle[0]=='!'){_XLOG_
		FORK(PerformTrayAction_InThread,atol(szQRunTitle+1));
		return TRUE;
	}

	if(szQRunTitle && szQRunTitle[0]=='@'){_XLOG_
		// Активируем гор. клавишу по коду действия
		return WKIHOTKEYS_ActHotkeyByID(szQRunTitle);
	}

	int iNumber=-1;
	for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
		if(objSettings.aQuickRunApps[i] && (stricmp(objSettings.aQuickRunApps[i]->sItemName,szQRunTitle)==0
			|| stricmp(objSettings.aQuickRunApps[i]->sUniqueID,szQRunTitle)==0)){_XLOG_
			iNumber=i;
			break;
		}
	}
	if(iNumber==-1){_XLOG_
		return FALSE;
	}
	RunQRunInThreadParam* pParam=new RunQRunInThreadParam;
	pParam->bPopupItem=FALSE;
	pParam->iWndNum=iNumber;
	pParam->bSwapCurDesk=FALSE;
	pParam->szAdditionalParameters=szAdditionalParameter;
	pParam->szSelection=szSelection;
	pParam->bSubstituteSelection=(szSelection==0)?0:1;
	StartQRun(pParam);
	return TRUE;
}

void MyShutDownComputer(BOOL bRestart, int iType)
{_XLOG_
	if(iType>=3){_XLOG_
		WaitWhileAllKeysAreFree();
	}
	ShutDownComputer(bRestart, iType);
}

CArray<int,int> aEventId;
CArray<int,int> aEventCountByType;
CArray<FPONSYSTEMEVENT,FPONSYSTEMEVENT> aFpEvents;
BOOL RemSysEventSubmit(int iEventType, FPONSYSTEMEVENT fp)
{_XLOG_
	if(iEventType<0 || fp==0){_XLOG_
		return 0;
	}
	if(iEventType>=aEventCountByType.GetSize()){_XLOG_
		aEventCountByType.SetAtGrow(iEventType,0);
	}
	aEventCountByType[iEventType]=aEventCountByType[iEventType]-1;
	for(int i=0;i<aEventId.GetSize();i++){_XLOG_
		if(aEventId[i]==iEventType && aFpEvents[i]==fp){_XLOG_
			aEventId.SetAtGrow(i,0);
			aFpEvents.SetAtGrow(i,0);
		}
	}
	return aFpEvents.GetSize();
}

BOOL AddSysEventSubmit(int iEventType, FPONSYSTEMEVENT fp)
{_XLOG_
	if(iEventType<0 || fp==0){_XLOG_
		return 0;
	}
	if(iEventType>=aEventCountByType.GetSize()){_XLOG_
		aEventCountByType.SetAtGrow(iEventType,0);
	}
	aEventCountByType[iEventType]=aEventCountByType[iEventType]+1;
	int iIndex=aEventId.Add(iEventType);
	aFpEvents.SetAtGrow(iIndex,fp);
	return aFpEvents.GetSize();
}

DWORD WINAPI OnActivateEvent(LPVOID p)
{_XLOG_
	CWKSYSEvent* pEvent=(CWKSYSEvent*)p;
	int iC=0;
	BOOL bNeedDel=0;
	for(int i=0;i<aEventId.GetSize();i++){_XLOG_
		if(aEventId[i]==pEvent->iEventType && aFpEvents[i]!=0){_XLOG_
			if(!pEvent){_XLOG_
				bNeedDel=1;
				pEvent=new CWKSYSEvent;
				memset(pEvent,0,sizeof(CWKSYSEvent));
				CPoint pt;
				GetCursorPos(&pt);
				pEvent->iCurPosX=pt.x;
				pEvent->iCurPosY=pt.y;
			}
			pEvent->cbSize=sizeof(CWKSYSEvent);
			(*aFpEvents[i])(pEvent);
			iC++;
		}
	}
	if(bNeedDel){_XLOG_
		delete pEvent;
	}
	return iC;
}


CString GetCurrentDesktopAtom()
{_XLOG_
	char s[64]={0};
	WKUtils::GetCurrentDesktopAtom(s);
	return s;
}

void OpenCurrentDeskAtom(BOOL bAdd)
{_XLOG_
	ATOM hTest;
	while((hTest=GlobalFindAtom(GetCurrentDesktopAtom()))!=0){_XLOG_
		GlobalDeleteAtom(hTest);
	}
	if(bAdd){_XLOG_
		GlobalAddAtom(GetCurrentDesktopAtom());
	}
}

