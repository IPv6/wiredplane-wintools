// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_)
#define AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "_defines.h"
class CWPT;
class CSettings
{
public:
	// Common functions
	void LoadActiveWPT(const char* szPreset, CStringArray& aWPTs, BOOL bLDefs);
	void LoadActiveWPT();
	void SaveActiveWPT();
	void Finalize();
	void ApplySettings();
	void Load();
	void Save();
	void SaveAll();
	void LoadZones();
	void SaveZones();
	void OpenOptionsDialog(const char* szDefTopic=NULL, CWnd* pParent=NULL);
	BOOL CheckLiks(const char* szWho, const char* szOne, const char* szOther);
	CSettings();
	virtual ~CSettings();
	// Common vars
	CString sIniFile;
	CString sMainWindowTitle;
	CString sTrayTooltip;
	long bRunStatus;
	long bSaveAllowed;
	long bStartWithWindows;
	long bStartWithOptions;// Начинать работу проги с показа Options
	long lVeryFirstStart;
	char szSystemFolder[MAX_PATH+1];
	long iStickPix;
	long bDoModalInProcess;
	CString sLikUser;
	CString sLikKey;//20-длина ключа, 1-разделитель, 10-имя пользователя
	CString sLikFile;
	long isLight;//Облегченная ли версия
	long iLikStatus;// 0 - все ок, 14 дней, 1 - 14 закончились, 2 - есть ключ
	CIHotkey DefActionMapHK[MAX_HOTACTIONS];//горячие клавиши для действий
	// Для функций "горячего" меню эти данные должны быть уже установлены (как для кликнутости, так и гор. клавиши)
	HWND hFocusWnd;
	HWND hHotMenuWnd;
	CRect ActiveWndRect;
	HINSTANCE hUniquer;
	long bIntegrateWithWN;
	//////////////////////////
	//////////////////////////
	long lUseGlobalRandomWPTLevel;
	long lApplyingChangesWarn;
	long lDelThemeWarn;
	long lDelNoteWarn;
	long lDelImageWarn;
	long lDelClockWarn;
	HINSTANCE hEnliver;
	long bUnderWindowsNT;
	int iLang;// Для первоначальной загрузки
	int iLangsCount;// Для первоначальной загрузки
	CStringArray aLangBufferNames;
	long lOnExitFromMainWnd;
	long lOnWallExchange;
	// Часы
	CCriticalSection cNoteLock;
	CCriticalSection cAddWP;
	CArray<CNote,CNote&> aDesktopNotes;
	long lWPBGColor;
	long lWPTXColor;
	long lWPIBGColor;
	long lChangePriodically;
	long lHideConfDataInLock;
	CString sHideConfDataInLock2;
	long lSkipMorthOnGeneration;
	long lConfModeIsON;
	long lAdForPrevModeIsON;
	long lAutoAddExternalWP;
	long bNeedFirstTimeChange;
	long lChangePeriod;
	long lNextTimeChange;
	long lLastTimeChange;
	COleDateTime dtStartTime;
	long dwAutoStretchLim;
	long iColWChWidth;
	// Шаблоны
	CStringArray aLoadedWPTs;
	CMap<CString,const char*,CWPT*,CWPT*> aLoadedWPTsData;
	CStringArray TemplatesInWork;
	BOOL bTemplatesInWorkChanged;
	CString sWChangerFile;
	CString sReminderWav;
	long lLoopRemidersSound;
	long lShowRIconOnNotes;
	long bSupportHTMLWP;
	long bTrasparDeskIcons;
	long bChangeTrayIconOnWP;
	long bLoadDirsRecursively;
	long bSetLastWallpaperForDefUserToo;
	long bEnablePreviewDelay;
	long bEnablePreviewDelayMs;
	long bPreviewSize;
	long bShowTrayIconAsWP;
	long iHTMLTimeout;
	CString sActivePreset;
	CCriticalSection HtmlWP_CS;
	CString sPictInitDir;
	CString sIcoInitDir;
	CString sWallpaperFile;
	CString sBMPFileName;// Имя настоящего файла, куда будет врисовываться картинка
	CString sHTMLWrapperFile;
	long lDisableNotesBeforeExit;
	long noWallpaperChangesYet;
	long noDesktopHookProcessing;
	long lNoExitWarning;
	long bUsePreview;
	static BOOL bWPEngineInitialized;
	long bChangeWPaperOnStart;
	long bOnStartRegerate;
	WChanger m_WChanger;
	BOOL WPInProgress;
	CString sAddThisFileAfterOpenWPList;
	long bChangetWPIfScrSaver;
	long bSelectCurWallpaperAtOpen;
	long bPreviewQuality;
	long bUsePathEllipses;
	long bChangeWPaperCycleType;
	long bFineStretching;
	long bReportBrokenLinks;
	long bReportActiveDesktopChange;
	long bReportLongImageProcs;
	long bReportInformAlerts;
	long bExitAlert;
	long lReportAddWPNow;
	long bReportTraHit;
	long lCurPicture;// Текущий номер из aImagesInRotation (тек. обоя)
	long lChangeIfMemoE;
	long lChangeIfMemoP;
	long lChangeIfCPUE;
	long lChangeIfCPUP;
	long lAlertsOnTaskbar;
	long lSetWallaperLock;
	CString lSetWallaperLockDesc;
	long lMainWindowOpened;
	long lStartsCount;
	long lChangesCount;
	long lEnliverON;
	long bFirstJustEditNote;
	long bFirstMiniNote;
	long lDeskTooltips;
	//
	long lCheckForUpdate;
	long lDaysBetweenChecks;
	//
	long bBlockAlerts;
	long dwScrTimeout;
	CString sScrEffect;
	long iScrEffect_ML;
	long iScrEffect_TS;
	long lTempDisableAllWidgets;
	long lShowDupHelp;
	long lSyncCount;
	long lRemBrokenOnSync;
	long lShowPopupsOnErr;
	long bTileOrCenter;
	long bDotajkaImages;
	long bDotajkaRand;
	long bDotajkaRandImgBrd;
	long dwDefNoteOpacity;
	long lDefNoteModifier;
	long lDefautNoteType;
	long lAutoWrapNote;
	long lAutoSaveNote;
	CArray<CNoteSkin,CNoteSkin&> aNoteSkin;
	long lNoteSkin;
	long lNoteView;
	CFont* pNoteFont;
	CFont* pNoteTFont;
	CFont* pDicoFont;
	long lChangeDicoFont;
	long lShowAnimationItems;
	long lReuseBrowser;
	long lActOnDblClick;
	long lenableSimpleMoving;
	long lTrackWallpaperChanges;
	long lTrackWallpaperSaveJpg;
	long lTrackWallpaperSaveJpgQ;
	long lChangeInLowP;
	long lRestoreOnResChange;
	CSmartWBrowser* browser;
	CString sLastWallpaperFromUser;
	CString sLastWallpaperFromWC;
	long iLastStretchingFromUser;
	int iTile;
	long lDefaultNoteIcon;
	HANDLE hGlobalTimerEvent;
	CArray<CRect,CRect&> aActiveZonesRect;
	CStringArray aActiveZonesActions;
	long lDisableNotes;
	long iTreatUnwtRight;
	CCriticalSection csWPInChange;
	long lRefreshOnNewDay;
	CStringArray aCustomIcons;
	CStringArray aCustomFonts;
	CStringArray aCustomBg;
	long iCustomsBuf;
	long lHideIconsByDefault;
	CString sStopTitles;
	long lExcludeTaskbar;
	long lDblToRename;
	long lDefMWTab;
	HINSTANCE hRemin;
	HINSTANCE hHookLib;
	CString sNotesFolder;
	long lEnableDebug;
	long iCurrentMultyOrderStyle;
	long bDefViewCalendar;
	long bDefViewWNums;
	long lUseNoteTitleShadow;
	long lUseNoreDirectPositioning;
};
extern CSettings objSettings;
BOOL WriteSingleTemplateVar(CString wptName,CString sVar,CString sVal,const char* szPreset=0);
BOOL ReadSingleTemplateVar(CString wptName,CString sVar,CString& sVal,const char* szPreset=0,const char* szOverloadedDef=0);
class CWPT
{
public:
	CWPT()
	{
		bClonable=0;
		dwWeTmp=99;
		bWPTType=0;
		sADType="";
		sADTypeAlt="";
		sHints="";
		hIRow=0;
		bScrollBarsAware=0;
	}
	BOOL bWPTType;
	BOOL bClonable;
	BOOL bScrollBarsAware;
	CString sActionTransparency;
	CString sActionPosition;
	CString sHash;
	CString sADType;
	CString sADTypeAlt;
	CString sHints;
	CString sTitle;
	CString sCategs;
	CString sExcCateg;
	CString sDsc;
	CString sPreview;
	CString sWPFileName;
	CString sTemplName;
	CDWordArray aLoadedWPTsActionsFlags;
	CStringArray aLoadedWPTsActionsDsc;
	CStringArray aLoadedWPTsActionsFile;
	long dwWeTmp;
	HIROW hIRow;
	CWPT& operator=(CWPT& CopyObj)
	{
		int i=0;
		sHints=CopyObj.sHints;
		hIRow=CopyObj.hIRow;
		sHash=CopyObj.sHash; 
		sADType=CopyObj.sADType;
		sADTypeAlt=CopyObj.sADTypeAlt;
		dwWeTmp=CopyObj.dwWeTmp;
		sWPFileName=CopyObj.sWPFileName;
		sTemplName=CopyObj.sTemplName;
		bClonable=CopyObj.bClonable;
		sPreview=CopyObj.sPreview;
		sTitle=CopyObj.sTitle;
		sCategs=CopyObj.sCategs;
		sExcCateg=CopyObj.sExcCateg;
		sDsc=CopyObj.sDsc;
		sActionTransparency=CopyObj.sActionTransparency;
		sActionPosition=CopyObj.sActionPosition;
		for(i=0;i<CopyObj.aLoadedWPTsActionsFlags.GetSize();i++){
			aLoadedWPTsActionsFlags.SetAtGrow(i,CopyObj.aLoadedWPTsActionsFlags[i]);
		}
		for(i=0;i<CopyObj.aLoadedWPTsActionsDsc.GetSize();i++){
			aLoadedWPTsActionsDsc.SetAtGrow(i,CopyObj.aLoadedWPTsActionsDsc[i]);
		}
		for(i=0;i<CopyObj.aLoadedWPTsActionsFile.GetSize();i++){
			aLoadedWPTsActionsFile.SetAtGrow(i,CopyObj.aLoadedWPTsActionsFile[i]);
		}
		return *this;
	}
	BOOL isAD()
	{
		return (sADType=="")||(sADType=="ActiveDesktop")||(sADTypeAlt=="ActiveDesktop");
	}
	CString GetFTitle()
	{
		CString sRes=_l(sTitle);
		DWORD dwWe=GetWeight();
		if(dwWe<100){
			sRes+=Format(" (%i%%)", dwWe);
		}
		return sRes;
	}
	long GetWeight(const char* szPreset=0)
	{
		CString sP;
		if(szPreset==0){
			sP=objSettings.sActivePreset;
		}else{
			sP=szPreset;
		}
		CString sWeight="100";
		ReadSingleTemplateVar("actives-freq",sWPFileName,sWeight,sP,"100");
		return atol(sWeight);
	}

	void SetWeight(long lNew,const char* szPreset=0)
	{
		CString sP;
		if(szPreset==0){
			sP=objSettings.sActivePreset;
		}else{
			sP=szPreset;
		}
		WriteSingleTemplateVar("actives-freq",sWPFileName,Format(lNew),sP);
	}
};

// Вызов диалога со стилями
void ApplyDeskIconsOptions();
BOOL CallStylesDialogWnd(CWnd* pWnd);
CString GetNSkinBmp(int iSkin, const char* szBmp, int iNum=0);
BOOL CALLBACK DownloadWC(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK BuyWC(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK DownloadWP(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK DownloadWPT(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK OpenNoteSkins(HIROW hData, CDLG_Options* pDialog);
void PerformUnregActions();
void BackupAllSets();
void RestoreAllSets();
void UnregAlert(const char* szText);
BOOL CALLBACK OpenAnimProps(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK ChooseAnyFile(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK ChooseDir(HIROW hData, CDLG_Options* pDialog);
void LoadTemplates(CString sStartDir);
BOOL CloneTemplate(int iTemplNum);
BOOL RemoveTemplate(int iCurSel);
BOOL OpenFileDialog(const char* szExtension, LPVOID lpData, CDLG_Options* pDialog, BOOL bSkipFileCheck=FALSE, BOOL bWithCString=FALSE, CWnd* pParent=0, BOOL bOpenDialog=TRUE, CStringArray* pMultyOut=0);
BOOL SetScreensaver(BOOL bUI);
void RestartWC();
CWPT* GetWPT(const char* szWPT);
CString GetWPTTitle(const char* szWPT);
CString GetWPTTitle(int i);
BOOL GetWPTClonable(int i);
CWPT* GetWPT(int i);
int GetWPTIndex(const char* szWPT);
int GetTemplateMainIndex(const char* szTemplate);
BOOL CallTemplatePositionDialog(CWPT* wpt,CRect* pCurRect=0, BOOL bSilent=0);
DWORD WINAPI CallTemplatePositionDialog_InThread(LPVOID);
DWORD WINAPI CallTemplatePositionDialog_InThread2(LPVOID p);
CString GetNotesFolder();
CString GetPresetsFolder(CString sName);
BOOL isFolderEmpty(CString sDir, CString sMask);
BOOL DeleteFilesByMask(CString sDir, CString sMask, BOOL bDelRoot);
void GetPresetList(CStringArray& aPres);
CRect& GetMainMonitorRECTX();
#endif // !defined(AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_)
