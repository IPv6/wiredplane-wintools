// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_)
#define AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_defines.h"
#include "WireKeys.h"
#include "WKPlugin.h"
#include "WKPluginEngine.h"
#include "DLG_InfoWindow.h"
#include "DLG_EditMLine.h"
#include "WireKeysDlg.h"

#define ANY_MACRO	101
#define CATEG_MACR	100

#define OC_LANG		1
#define OC_NETMS	2
#define OC_NETNB	3
#define OC_NETXPC	4
#define OC_NETONLINESTATUS 5
#define OC_SORT		6
#define OC_ICON		7
#define OC_FILTER	8
#define OC_LICDATA	9
#define OC_STARTUP	10
#define OC_SHOWICON	11
#define OC_SPLASHC	12
#define OC_QRUNFROM	13
#define OC_CLREGEXP	14
#define OC_PLUGACT	15
#define OC_BOOST	16
#define OC_DESKRES	17
#define OC_BACKUP	18
#define OC_HKL		19
#define OC_ICONTYPE	20
#define OC_SCRSAVE	21
#define OC_STARQAPP	50


#define APP_DISCLAMER0	_l("This option may not work with some applications","This option may not work with some applications or certain application parameters")
#define APP_DISCLAMER	"\t"+APP_DISCLAMER0
class CQRunHotkeyItem
{
public:
	CIHotkey oHotKey;
	DWORD dwHotkeyID;
	CQRunHotkeyItem()
	{
		dwHotkeyID=0;
	}
	CQRunHotkeyItem(CIHotkey hk)
	{
		dwHotkeyID=0;
		oHotKey=hk;
	}
	CQRunHotkeyItem(CQRunHotkeyItem& hk)
	{
		dwHotkeyID=hk.dwHotkeyID;
		oHotKey=hk.oHotKey;
	}
	CQRunHotkeyItem& operator=(CQRunHotkeyItem& CopyObj)
	{
		dwHotkeyID=CopyObj.dwHotkeyID;
		oHotKey=CopyObj.oHotKey;
		return *this;
	}

};

class CScrItem
{
public:
	char sName[MAX_PATH];
	char sPath[MAX_PATH];
	long bUse;
	CScrItem(const char* s=0,const char* p=0, BOOL b=1)
	{
		sName[0]=0;
		if(s){
			strcpy(sName,s);
		}
		sPath[0]=0;
		if(p){
			strcpy(sPath,p);
		}
		bUse=b;
	}
};

class CQuickRunItem
{
public:
	CQuickRunItem();
	~CQuickRunItem();
	// lMacros - тип элемента
	// 0 - просто приложение с параметром
	// 1 - приложение с параметризумыми параметрами (как 0, но уже макрос)
	// 2 - макрос для вставки. sItemPath - путь к файлу с макросом
	// 3 - Insertion macro. lExpandFolderOrCheckInstance>0 - random string from file
	// CATEG_MACR - категория для всего
	long lMacros;
	// Всякое разное
	CString sUniqueID;
	CString sItemName;
	CString sItemPath;
	CString sItemParams;
	CString sStartupDir;
	CString sCategoryID;
	CString sDsc;
	long lStartInDesktop;
	long lUniqueCount;
	long lOptionClass;
	long lHowToShowInTray;
	long lExpandFolderOrCheckInstance;
	long lUseDetachedPresentation;
	long lUseAdditionalOptions;
	CString sAutoExpandMask;
	long lAutoCreate;// Создавать каталог если его нету
	long lStartupTime;//Время ожидания появления главного окна после запуска проги
	long lAfterStart;//0-ничего, 1-спрятать в трей, 2-спрятать совсем, 3-run minimized, 4-run maximized
	long lStartAtStartUp;//Запускать ли вместе с WireKeys
	long lShowInUpMenu;//Выносить ли в меню на уровень вверх
	long lSearchInstanceFirst;//Запускать ли приложение даже если такое уже запущено (0) или активировать запущенное (1)
	long lParamType;//Тип параметра 0=строка,1=file,2=directory
	long lFixPostion;
	long lFixedXPos;
	long lFixedYPos;
	long lFixSize;
	long lFixedWidth;
	long lFixedHeight;
	long lOpacity;
	CString sParamTypeName;
	//CIHotkey oHotKey;
	CArray<CQRunHotkeyItem*,CQRunHotkeyItem*> qrHotkeys;
	BOOL SetHotkey(int iPos,CIHotkey oHotKey);
	BOOL RemoveAllQRunHotkeys();
	HWND hWnd;//Хэнд окна, если приложение уже было запущено
	HIROW hLeftPanePosition;
	HIROW hThisItemParent;
	HIROW hThisItemRoot;
	long lTempCategNumber;
	static long lExtraMscroHotkeysIDs;
	void Copy(CQuickRunItem* p)
	{
		lMacros=p->lMacros;
		sItemName=p->sItemName;
		sItemPath=p->sItemPath;
		sItemParams=p->sItemParams;
		sStartupDir=p->sStartupDir;
		sCategoryID=p->sCategoryID;
		sDsc=p->sDsc;
		lStartInDesktop=p->lStartInDesktop;
		lHowToShowInTray=p->lHowToShowInTray;
		lExpandFolderOrCheckInstance=p->lExpandFolderOrCheckInstance;
		lUseDetachedPresentation=p->lUseDetachedPresentation;
		lUseAdditionalOptions=p->lUseAdditionalOptions;
		sAutoExpandMask=p->sAutoExpandMask;
		lAutoCreate=p->lAutoCreate;
		lStartupTime=p->lStartupTime;
		lAfterStart=p->lAfterStart;
		lStartAtStartUp=p->lStartAtStartUp;
		lShowInUpMenu=p->lShowInUpMenu;
		lSearchInstanceFirst=p->lSearchInstanceFirst;
		lParamType=p->lParamType;
		lOpacity=p->lOpacity;
		lFixPostion=p->lFixPostion;
		lFixedXPos=p->lFixedXPos;
		lFixedYPos=p->lFixedYPos;
		lFixSize=p->lFixSize;
		lFixedWidth=p->lFixedWidth;
		lFixedHeight=p->lFixedHeight;
		sParamTypeName=p->sParamTypeName;
		lTempCategNumber=p->lTempCategNumber;
	}
};

struct RunQRunInThreadParam
{
	int iWndNum;
	BOOL bPopupItem;
	BOOL bSwapCurDesk;
	CString szAdditionalParameters;
	CString szSelection;
	BOOL bSubstituteSelection;
	int iEntryPoint;
	HANDLE hInThread;
	RunQRunInThreadParam()
	{
		hInThread=0;
		iWndNum=-1;
		bPopupItem=FALSE;
		bSwapCurDesk=TRUE;
		szAdditionalParameters="";
		iEntryPoint=0;
		szSelection="";
		bSubstituteSelection=0;
	};
};

class CStringArrayX:public CArray<CPairItem,CPairItem&>
{
public:
	CStringArrayX& operator=(CStringArrayX& CopyObj)
	{
		RemoveAll();
		for(int i=0;i<CopyObj.GetSize();i++){
			SetAtGrow(i,CopyObj[i]);
		}
		return *this;
	}
};

class CSettings
{
public:
	virtual void Save(const char* szConfigFile=NULL);
	virtual void Load(const char* szConfigFile=NULL);
		
	void DeleteQRun();
	void Finalize();
	void ApplySettings();
	void SaveQRuns(const char* szOutFile);
	void LoadQRuns(const char* szInFile,BOOL bAppendMode=0);
	void SaveHistories(const char* szOutFile);
	void LoadHistories(const char* szInFile);
	void LoadDeskLayout(BOOL bDoNotRestore=FALSE);
	void SaveAll();
	BOOL OpenOptionsDialog(int iTab=0,const char* szDefTopic=NULL, CWnd* pParent=NULL, int fpPostActionType=-1, LPVOID pPostActionParam=0);
	CSettings();
	virtual ~CSettings();
	// Common
	long bRunStatus;
	long bSaveAllowed;
	long bStartWithWindows;
	long bStartWithOptions;// Начинать работу проги с показа Options
	long bApplicationFirstRun;
	long lTempLang;
	long iStickPix;
	long bDoModalInProcess;
	CString sMainWindowTitle;
	CString sPrevVersion;
	//////////////////////////
	// WireKeys
	//////////////////////////
	// Плагины
	CArray<CPlugin*,CPlugin*> plugins;
	//
	long lHKL1;
	long lHKL2;
	long lHKLCount;
	CString sHKLList;
	HKL aKBLayouts[MAX_HKL];
	CIHotkey aKBLayoutKeys[MAX_HKL];
	long lLayEffectType;
	CString sCDDiskDefault;
	CString sCDDisk;
	CString sCDDisk2;
	CString sKillBeforeShutdown;
	long lStartupMsgType;
	long bUnderWindowsNT;
	long bUnderWindows98;
	long lDefaultSelMethod;
	long lDefaultSelQuality;
	CArray<InfoWnd*,InfoWnd*> aHidedWindowsMap;
	CArray<CQuickRunItem*,CQuickRunItem*> aQuickRunApps;
	int AddQrunT(CQuickRunItem* p){
		CSmartLock ql(aQRunLock,TRUE,-1);
		return aQuickRunApps.Add(p);
	}
	CMap<HANDLE,HANDLE,LPVOID,LPVOID> aQRThreads;
	long bSupportOwnerDraw;
	long lHTrayRecentItems;
	long bAskForKill;
	long bAskForRemQ;
	long bIntegrateWithWN;
	long bFixXSize;
	long lFixXSizeTo;
	long bFixYSize;
	long lFixYSizeTo;
	long bSignSShot;
	long bAutoEditOnShot;
	CString sJpgEditor;
	CString sJpgEditorDef;
	long lSplashTextColor;
	long lSplashBgColor;
	CFont* pSplashFont;
	CString sSignString;
	long iGlobalScrShotCount;
	long lShowScrData;
	CString sScrshotFile;
	CString sScrshotDir;
	CString sScrshotDir2;
	CRect rUserRect;
	long bShotActiveWindow;
	long bShotActiveWindow2;
	long bUseJpg;
	long bUseJpgQuality;
	long bPutScreshotInClipboard;
	long bBlackAndWhiteConfirm;
	long bHidedAppShowMenu;
	long lShutdownStrength;
	long lBosskeyAction;
	long lBosskeyAction2;
	long lBoosShotSwap;
	long lBossShotSwap;
	CString	sLastScreenshot;
	CString sBossAppMask;
	CString sBossAppMask2;
	CString sDefaultFind;
	//
	long bAddClipboard;
	long bUseRegexps;
	CString sLayoutP1;
	CString sLayoutP2;
	CString sRegexpWhat;
	CString sRegexpTo;
	long lEnableTransmitMode;
	long lClipTransmitMode;
	long lAppendTextToTransmits;
	CString sTransmitAppendText;
	HWND hWndToTransmitTo;
	long lClipCount;
	long lClipCountAfterNextStart;
	long lUseMaxClipSizeLimit;
	long lMaxClipSizeLimit;
	long lClipHistoryDepth;
	CStringArray sClipBuffer;
	//CStringArray sClipHistory[MAX_CLIPS];
	CMap<CString, const char*, CStringArrayX, CStringArrayX&> sClipHistory;
	CIHotkey DefClipMapHK[MAX_CLIPS*2+2+1];//горячие клавиши для буферов
	CIHotkey DefClipMapHK_H[MAX_CLIPS+1];//горячие клавиши для буферов - история
	CIHotkey DefClipMapHK_HA[MAX_CLIPS+1];//горячие клавиши для буферов - история по приложению
	CIHotkey DefActionMapHK[LAST_ACTION_CODE+1];//горячие клавиши для действий
	long lDesktopSwitches;
	long lEnableAddDesktops;
	long lRestoreDeskIconsPos;
	long lDisableDeskicoAlert;
	long lRestorePreservRes;
	long lStartupDesktop;
	long lLastActiveDesktop;
	long lDesktopSwitchesAfterNextStart;
	CIHotkey DefDSwitchMapHK[MAX_DESKTOPS];//горячие клавиши для смены десктопов
	long lShowSplashForQuickRun;
	//
	CMapStringToString aIconsMap;
	long bGetTextSelectionStyle;
	long bSysActionsInHotMenu;
	long bAppShortcInHotMenu;
	long bMenuOnDblHotMenu;
	long bMiscSaverMenu;
	long bQRunMenu;
	long bMacroMenu;
	long bVolumeMenu;
	long bTextActionsInHotMenu;
	long bExtractIconsForSysMenu;
	long bSortForSysMenu;
	long bPlistUse2Columns;
	long bPlistUseFullPaths;
	long bShowCPUinTT;
	long lLogCPUHungers;
	CString sLogCPUHungers;
	CString sListToKillSim;
	CString sListToHideSim;
	CString sAppToRWS;
	long lStopAppToRWS;
	long bShowMemUsage;
	long bErrorSound;
	long bClipCopySound;
	long bClipPasteSound;
	long bOnLConv;
	long bSShot;
	long bSCalc;
	long bSHk;
	long bAutoCloseCD;
	long bEjectCDOnShut;
	long lShowHPMTooltip;
	long lAutoCloseCDTime;
	long lVolumeCStep;
	long lVolumeWStep;
	long lVolumeBeforeShutdown;
	long lVolumeBeforeShutdownLevel;
	long lVolumeAfterStart;
	long lVolumeAfterStartLevel;
	long lVolumeNeedOSD;
	long lWinAmpNeedOSD;
	long lNoNeedOSD;
	CString sWinAmpStartPas;
	long lDeskNeedOSD;
	long lDeskMemOptOSD;
	long lDeskfadeOSD;
	long lOSDBorder;
	long lOSDSecs;
	long lOSDChars;
	long bShowIconInTray;
	long bSkipHKWarning;
	long lShowDateInTrayIcon;
	long lIndiTypeInTrayIcon;
	long lShowCPUHistInTray;
	long lShowCPUHistInTrayC;
	long lWaitForAddclipInput;
	CString	sErrorSound;
	CString sClipCopyWav;
	CString sClipPasteWav;
	CString sOnLConvWav;
	CString sSShotWav;
	CString sSCalcWav;
	CString sSHkWav;
	// Для функций "горячего" меню эти данные должны быть уже установлены (как для кликнутости, так и гор. клавиши)
	HWND hFocusWnd;
	HWND hHotMenuWnd;
	CRect ActiveWndRect;
	CArray<HWND,HWND> sTopLevelWindowsHWnds;
	CStringArray sTopLevelWindowsTitles;
	// Проверка версии
	long lCheckForUpdate;
	long lDaysBetweenChecks;
	long lAlertsOnTaskbar;
	long lOptionsDialogOpened;
	long lDefaultMacros;
	CString sBackupFile;
	long lSkipSavings;
	long lShowWinampTitle;
	long lStripNumberWinampTitle;
	long lAddTTWinampTitle;
	long lAddTLWinampTitle;
	long lStripAllWinampTitle;
	long lShowWTitInTooltip;
	long lCatchWinampTitle;
	long lWinampOSDTime;
	long bPopupRecentlyUsed;
	long lStopWinAmpOnScrsave;
	long lSetRandomScreensaver;
	long lIsScreensaverRandomized;
	CArray<CScrItem,CScrItem&> dirScreenSavers;
	CString sValidScreenSavers;
	long lStopWinAmpOnLock;
	long lStartWinAmpAfterLock;
	long lDisableDblPressExtensions;
	long lHotMenuOpened;
	long bOverlayTrayAppIcon;
	long bAnimateFloaters;
	long bFreeMemOfHidApp;
	long bTakeComLineForFloater;
	long lFloatUseSingleClick;
	long lFloatUseLBDir;
	long bPreserveTitle;
	long bRetrieveSysMenu;
	long bKillProcessOnSelect;
	CString sLikFile;
	CString sLikUser;
	CString sLikKey;
	long iLikStatus;// 0 - все ок, 30 дней, 1 - 30 закончились, 2 - есть ключ
	long lPrevLik;
	long lDisableSCHelp;
	long lDisableFocusMove;
	long sDetachedSize;
	long lNoNotifyByCursor;
	long lOptimizeRegistryON;
	long lFloaterBG;
	long lFloaterTransp;
	long lShowCPUInTrayIcon;
	long lShowCPUInTraySecs;
	long lExitWndOnDbl;
	long lAddCursorToScreenshot;
	HANDLE hTimerEvent;
	long lScreenshotByTimer;
	long lScreenshotByTimerSec;
	CAvailableVideoModes videoModes;
	CAvailableVideoModes videoModesShort;
	long lDisableSounds;
	CCriticalSection aQRunLock;
	long lRemeberIconsPosInDesktops;
	long lRemeberResolutInDesktops;
	long lRemeberShellInDesktops;
	long lChoosenResolutions[MAX_DESKTOPS];
	CVideoMode lResolutions[MAX_DESKTOPS];
	CString lDeskShells[MAX_DESKTOPS];
	long lNumberOfLinesToScroll;
	long lPreventTrayMenu;
	long lLBTray;
	long lMBTray;
	long lRBTray;
	long lShowMacroIcon;
	CString sODSTimeFormat;
	CString sODSDateFormat;
	long lUseAllVModes;
	CString sUDIcon;
	HICON hUDIcon;
	long lDirectMouseToUnderC;
	CDLG_Options* pOpenedOptions;
	HWND hActiveApplicationAtStart;
	long lSaveClipContent;
	long lSaveClipContentPer;
	CString sSavedClipContent;
	long lSaveBMPSinHist;
	long lLeaveInClipOnPaste;
	long lMaxBMPHistSize;
	long lMaxBMPHistSizeCatch;
	HWND hLastForegroundWndForPlugins;
	CString sDefOptionsTopic;
	CString sLastOpenedOptionsTopic;
	CString sFindOptionMask;
	long lCreateQRunFrom;
	long bIgnoreNextClipAction;
	CString sLastStringForTrayIcons;
	long bShutIP;
	long lInPlugAction;
	long lDoNotLoadMacros;
	long lLastWordNotLine;
	long lDisableClipboard;
	long lChTrayOnClips;
	long lStat_mouseLen;
	long lStat_keybCount;
	long lStat_hotkCount;
	HANDLE hUpAndRunningMutex;
	CString sCodeWord;
	long bAllowHighPriority;
	long lAllowDblCategStart;
	long lEverythingIsPaused;
	long lUseOldStartStyle;
	CString sActivePlugins;
	long bDskTutorial;
	CString sSafeChar;
};

class CStub
{
public:
	CStub()
	{
		g_INFOWNDHK_TOPIC_InfoWnd=0;
		lLikKeysPasted=0;
		pMainWnd=0;
		g_lOverlayState=0;
		HIROW hQRunType0=NULL;
		hQRunType3=NULL;
		hQRunType2=NULL;
		hQRunType0Right=NULL;
		hQRunType2Right=NULL;
		hQRunTypeCCRight=NULL;
		bCategsWereChanged=0;
		_ShowCPUUsageDef=1;
		sLangsNames="English";
		afxModuleStateX=AfxGetStaticModuleState();
		bHotkeyChangedMode=0;
		bForcePluginLoadUpdate=0;
	}
	AFX_MODULE_STATE* afxModuleStateX;
	CString sLangsNames;
	InfoWnd* g_INFOWNDHK_TOPIC_InfoWnd;
	CIHotkey g_INFOWNDHK_TOPIC_Hk;
	long lLikKeysPasted;
	CStringArray g_aListOfCategs;
	AppMainDlg* pMainWnd;
	long g_lOverlayState;
	HIROW hQRunType0;
	HIROW hQRunType3;
	HIROW hQRunType2;
	HIROW hQRunType0Right;
	HIROW hQRunType2Right;
	HIROW hQRunTypeCCRight;
	AppMainApp* theApp;
	BOOL bCategsWereChanged;
	long _ShowCPUUsageDef;
	BOOL bHotkeyChangedMode;
	BOOL bForcePluginLoadUpdate;

	virtual BOOL CALLBACK ClearAllClips(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL CALLBACK CheckLikKeys(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL CALLBACK BuyWK(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL CALLBACK AddDefMacros(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL CALLBACK RestoreIconsPos(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL CALLBACK SaveIconsPos(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL CALLBACK SwitchOverlaySupport(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL CALLBACK RemoveQRun(HIROW hData, CDLG_Options* pDialog);

	virtual BOOL CALLBACK ClearClipHistory(const char* szClipHistName);
	virtual CString GetQRunOptTitle(CQuickRunItem* pTemplate);
	virtual void RestartWK();
	virtual CString GetBasicHotkeyDsc(int iHK);
	virtual BOOL CALLBACK EditMacroFile(CString sFile);
	virtual BOOL GetEntryDesc(CString& sContentNP, int iEntryCount, CString& sHotkeyTitleEx, CString& sDeclaration);
	virtual CString GetMacrosFileName(CString sFile, BOOL bAutoGetModel=TRUE);
	virtual BOOL isUndefinedName(CString sName);
	virtual int FindQRunItemByID(const char* szID, CStringArray* aList=NULL);
	virtual BOOL CALLBACK ApplyOptionsFromDialog(BOOL bSaveAndClose,DWORD dwRes,CDLG_Options* dlgOpt);
	virtual long ChooseDlg(const char* szTitle,const char* szHelpString, CStringArray* pOptionList, int iStartElen, int iIconID, CWnd* pParent=0);
	virtual int CreateMlineEdit(CWnd* pDialog, int iMode, CString& sText, CString sTitle="");
	virtual BOOL CALLBACK ClearClipHistory(int iClip);
	virtual BOOL isDefaultName(const char* szName);
	virtual BOOL WINAPI ExternalEnterFind(const char* szTitle, CString& sText,CWnd* pDialog);
	virtual int GetBuffersLang(CStringArray& aLangBufferNames, int& iDefLang);
	virtual void vAlert(const char* szText, const char* szTitle=_l(PROGNAME));
	virtual BOOL CALLBACK CallPlgFunction(HIROW hData, CDLG_Options* pDialog);
	virtual BOOL GetFullInfoFromHWND(HWND hwnd,DWORD& dwProcID,CProcInfo*& info,CString& sCommand, CString& sParameter);
	virtual int GetIconNumByMacro(int lMacros, int iMacroParType=-1,const char* szName=0);
	virtual CString GetQrunKeyDsc(CQuickRunItem* p,int j);
	virtual CString GetDesktopName(int i);
	virtual CString GetHKLName(int i);
	virtual const char* GetAppVers();
	virtual void vAsyncPlaySound(const char* szSoundName="");
	virtual BOOL vEnumWindowsProc(HWND hwnd, LPARAM lParam);
	virtual BOOL isPluginLoaded(const char* szFileName, CPlugin** pOut=0, BOOL bResident=TRUE);
	virtual void InitScreenSaverPaths();
	virtual CQuickRunItem* CALLBACK AddQRunFromHWnd(HIROW hData, CDLG_Options* pDialog);
	virtual CQuickRunItem* AddQRunTemplate(long lMacrosMask=0, const char* szFile=NULL,CQuickRunItem* pItem=0, BOOL bAdd=1);
	virtual BOOL FillTemplateFromHWND(CQuickRunItem* pTemplate, HWND hwnd);
	virtual CString vGetApplicationDir();
	virtual CString vGetApplicationName();
	virtual long& vGetApplicationLang();
	virtual CIHotkey* getDefActionMapHK_Preinit();
	virtual CIHotkey* getDefClipMapHK_Preinit();
	virtual CIHotkey* getDefClipMapHK_H_Preinit();
	virtual CIHotkey* getDefClipMapHK_HA_Preinit();
	virtual CIHotkey* getDefDsSwMapHK_Preinit();
	virtual HICON vLoadIcon(int ID);
	virtual CInPlaceHotKey*& GetHotkeyInputInProgress();
};
#endif // !defined(AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_)
