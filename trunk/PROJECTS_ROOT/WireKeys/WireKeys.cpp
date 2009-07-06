//
#include <crtdbg.h>
#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "DLG_Chooser.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma comment(exestr,"Copyright ©2002-2007 WiredPlane.com. All rights reserved. Build time:"__TIMESTAMP__)
const char* szBuildDate=__TIMESTAMP__;
CSettings objSettings;
AppMainDlg* pMainDialogWindow=NULL;
extern int iGlobalState;
extern CCriticalSection pCahLock;
//
/////////////////////////////////////////////////////////////////////////////
// AppMainApp
BEGIN_MESSAGE_MAP(AppMainApp, CWinApp)
	//{{AFX_MSG_MAP(AppMainApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

int GetWinAmpPlStatus(HWND hWin=0);
void FillSplashParams(const char* szTitle, CSplashParams& par);
void ShowWinampTitle(BOOL bOnlyOSD)
{_XLOG_
	static HWND hwndWinamp=NULL;
	if(hwndWinamp==NULL || !IsWindow(hwndWinamp)){_XLOG_
		hwndWinamp=::FindWindow("Winamp v1.x",NULL);
	}
	if(hwndWinamp==NULL){_XLOG_
		return;
	}
	char szNewTitle[256]="";
	::GetWindowText(hwndWinamp,szNewTitle,sizeof(szNewTitle));
	if(strlen(szNewTitle)==0){_XLOG_
		return;
	}
	CString sTitle=szNewTitle;
	// Дальше...
	ReparseWinampTitle(sTitle,hwndWinamp,!bOnlyOSD);
	if(objSettings.lShowWTitInTooltip && !bOnlyOSD){_XLOG_
		ShowBaloon(sTitle,objSettings.lStripAllWinampTitle?"":CString("WinAmp: ")+_l("Song"),objSettings.lWinampOSDTime*1000);
	}else{_XLOG_
		CSplashParams* par=new CSplashParams;
		FillSplashParams(_l("WinAmp controls"),*par);
		if(objSettings.lStripAllWinampTitle){_XLOG_
			par->szText=sTitle;
		}else{_XLOG_
			par->szText=Format("%s: %s",_l("Song"),sTitle);
		}
		FORK(ShowWTitleSplash,par);
	}
}

CNewVersionChecker& GetVersionChecker();
DWORD WINAPI CheckNewVersionInThread(LPVOID p)
{_XLOG_
	//Sleep(1000*60*120);// Через 2 часа работы проверяем
	GetVersionChecker().SetTraits();
	StartCheckNewVersion(objSettings.lDaysBetweenChecks);
	return 0;
}

//----------------------------
extern DWORD dwClipAttachTime;
BOOL bOnScreenSaverDone=FALSE;
DWORD PriorityClasses[]=	{NORMAL_PRIORITY_CLASS,			HIGH_PRIORITY_CLASS,	REALTIME_PRIORITY_CLASS,	NORMAL_PRIORITY_CLASS,	IDLE_PRIORITY_CLASS};
DWORD PriorityClassesNT[]=	{ABOVE_NORMAL_PRIORITY_CLASS,	HIGH_PRIORITY_CLASS,	REALTIME_PRIORITY_CLASS,	NORMAL_PRIORITY_CLASS,	IDLE_PRIORITY_CLASS};
#define TIMER_SETTIMER_TICK			60*30*1000
#define TIMER_BOOST_TICK			3000
#define TIMER_CPUUSAGE_TICK			1000
#define TIMER_WAMPT_TICK			5000
#define TIMER_WINAMP_FAST			900
#define TIMER_LANGINDIC				500
void OnAfterScreensaver(BOOL);
BOOL OnBeforeScreensaver(BOOL);
DWORD WINAPI GlobalOnTimer(LPVOID pValue)
{_XLOG_
	TRACETHREAD;
	DWORD dwLastActivePPR=0;
	DWORD dwLastActivePID=0;
	DWORD dwSleepTime=TIMER_SETTIMER_TICK;
	HWND hLastActiveApp=NULL;
	// Ставим случ. дату чтобы в первый раз сработала проверка
	COleDateTime dtLast=COleDateTime(1900,01,01,00,00,00);
	int dtLastDay=-1;
	BOOL bNTFeatures=IsRealNT();
	DWORD dwStartTickCount=GetTickCount();
	while(pMainDialogWindow){_XLOG_
		if(pMainDialogWindow->iRunStatus==-2){_XLOG_
			return 0;
		}
		if(pMainDialogWindow->iRunStatus!=1){_XLOG_
			Sleep(500);
			continue;
		}
		static long lScrSaveInit=0;
		if(lScrSaveInit==0){_XLOG_
			::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			lScrSaveInit=1;
			if(objSettings.lSetRandomScreensaver){_XLOG_
				wk.InitScreenSaverPaths();
			}
			objSettings.sJpgEditorDef=GetExtensionAssociatedAction("jpg","");
			if(objSettings.sJpgEditorDef==""){_XLOG_
				objSettings.sJpgEditorDef=GetExtensionAssociatedAction("bmp","");
			}
			// Переаттач спая буфера
			if(dwClipAttachTime && GetTickCount()-dwClipAttachTime>5*60000){
				pMainDialogWindow->OnReattachClips();
			}
			//борьба с видеорежимами
			CVideoModes::GetAvailableVideoModes(objSettings.videoModes);
			{// Сокращенный список разрешений
				CVideoMode mdTmp;
				objSettings.videoModesShort.RemoveAll();
				if(objSettings.lStartupDesktop==0){_XLOG_
					CVideoModes::GetCurrentVideoMode(objSettings.lResolutions[0]);
				}
				for(int i=objSettings.videoModes.GetSize()-1;i>=0;i--){_XLOG_
					if(objSettings.lUseAllVModes){_XLOG_
						objSettings.videoModes[i].dwAdditionalData=i;
						objSettings.videoModesShort.Add(objSettings.videoModes[i]);
					}else{_XLOG_
						CVideoMode& mdCur=objSettings.videoModes[i];
						if(mdTmp.m_dwWidth!=mdCur.m_dwWidth || mdTmp.m_dwHeight!=mdCur.m_dwHeight){_XLOG_
							mdTmp.m_dwWidth=mdCur.m_dwWidth;
							mdTmp.m_dwHeight=mdCur.m_dwHeight;
							mdCur.dwAdditionalData=i;
							objSettings.videoModesShort.Add(mdCur);
						}
					}
				}
				// Сортируем videoModesShort чтобы с более высокой частотой шли первыми
				for(int iSort1=0;iSort1<objSettings.videoModesShort.GetSize()-1;iSort1++){_XLOG_
					for(int iSort2=iSort1+1;iSort2<objSettings.videoModesShort.GetSize()-1;iSort2++){_XLOG_
						if(objSettings.videoModesShort[iSort1].m_dwHeight==objSettings.videoModesShort[iSort2].m_dwHeight
							&& objSettings.videoModesShort[iSort1].m_dwWidth==objSettings.videoModesShort[iSort2].m_dwWidth){_XLOG_
							if(objSettings.videoModesShort[iSort1].m_dwFrequency*1000+objSettings.videoModesShort[iSort1].m_dwBitsPerPixel>objSettings.videoModesShort[iSort2].m_dwFrequency*1000+objSettings.videoModesShort[iSort2].m_dwBitsPerPixel){_XLOG_
								CVideoMode mT=objSettings.videoModesShort[iSort1];
								objSettings.videoModesShort[iSort1]=objSettings.videoModesShort[iSort2];
								objSettings.videoModesShort[iSort2]=mT;
							}
						}
					}
				}
			}
			::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_NORMAL);
		}
		////////////////////////////////////////////////
		// Проверяем 
		dwSleepTime=TIMER_SETTIMER_TICK;
		DWORD dwCurTick=GetTickCount();
		////////////////////////////////////////////////
		// Обновление загрузки проца
		if(objSettings.lShowCPUInTrayIcon>0){_XLOG_
			static DWORD dwPrevTime=0;
			// Так как сюда можем и чаще попадать...
			if(long(dwCurTick-dwPrevTime)>=1000*objSettings.lShowCPUInTraySecs){_XLOG_
				dwPrevTime=dwCurTick;
				pMainDialogWindow->SetWindowIcon();
				if(objSettings.lShowCPUInTraySecs==0){_XLOG_
					objSettings.lShowCPUInTraySecs=5;
				}
			}
			dwSleepTime=1000*objSettings.lShowCPUInTraySecs;
		}
		if(objSettings.lShowDateInTrayIcon==3 && pMainDialogWindow){_XLOG_
			if(dwSleepTime>TIMER_LANGINDIC){_XLOG_
				dwSleepTime=TIMER_LANGINDIC;
			}
			pMainDialogWindow->PostMessage(LANG_CHANGE,99,99);
		}
		if(objSettings.lSetRandomScreensaver){_XLOG_
			if(dwSleepTime>TIMER_WAMPT_TICK){_XLOG_
				dwSleepTime=TIMER_WAMPT_TICK;
			}
			if(isScreenSaverActive() && objSettings.lIsScreensaverRandomized==0){_XLOG_
				RandomizeScreensaver();
			}else{_XLOG_
				objSettings.lIsScreensaverRandomized=0;
			}
		}
		if(objSettings.lStopWinAmpOnLock || objSettings.sAppToRWS!="" || objSettings.lSetRandomScreensaver){_XLOG_
			if(dwSleepTime>TIMER_WAMPT_TICK){_XLOG_
				dwSleepTime=TIMER_WAMPT_TICK;
			}
			if((isScreenSaverActive() || isWindowStationLocked()) && !bOnScreenSaverDone){_XLOG_
				OnBeforeScreensaver(TRUE);
			}
		}
		if(objSettings.lLogCPUHungers){
			static DWORD dwLast=GetTickCount();
			if(GetTickCount()-dwLast>10000){
				__FLOWLOG;
				CCSLock lpc(&pCahLock,1);
				dwLast=GetTickCount();
				int d=int(GetCPUTimesPercents());
				if(d>80){
					__FLOWLOG;
					// Кто жрет наши ресурсы???
					CString s;
					s+="\n============ ";
					s+=DateFormat(COleDateTime::GetCurrentTime(),FALSE)+" "+TimeFormat(COleDateTime::GetCurrentTime(),TRUE,FALSE);
					s+="============\n";
					EnumProcesses(1);
					long iInd=-1;
					LONGLONG lMax=-1;
					for(int i=0; i<GetProccessCash().GetSize();i++){_XLOG_
						if(GetProccessCash()[i].sProcExePath!="" && GetProccessCash()[i].sProcName!="Idle" && GetProccessCash()[i].lProcent>0){_XLOG_
							s+=Format("%s, CPU=%02f Kernel=%02f User=%02f\n",GetProccessCash()[i].sProcExePath,
								double(GetProccessCash()[i].lProcent)/100000.0f,double(GetProccessCash()[i].KernelTime)/100000.0f,double(GetProccessCash()[i].UserTime)/100000.0f);
						}
					}
					AppendFile(objSettings.sLogCPUHungers,s);
				}
			}
		}
		// Сброс клавиатурного буфера
		// Раз в 2 секунды
		{_XLOG_
			static DWORD dwPrevTime=0;
			if(long(dwCurTick-dwPrevTime)>=1000*2){_XLOG_
				dwPrevTime=dwCurTick;
				// Восстановление после скринсейвера
				if(bOnScreenSaverDone){_XLOG_
					if(dwSleepTime>TIMER_WAMPT_TICK){_XLOG_
						dwSleepTime=TIMER_WAMPT_TICK;
					}
					if(!isScreenSaverActive() && !isWindowStationLocked()){_XLOG_
						OnAfterScreensaver(TRUE);
					}
				}
				if(!isWin98()){_XLOG_
					BOOL bNeedDrop=0;
					static CString sLastDesk="???";
					HDESK desktop = OpenInputDesktop(0,FALSE,DESKTOP_READOBJECTS);
					if(desktop){_XLOG_
						char szData[256]="";
						DWORD dwLen=0;
						GetUserObjectInformation(desktop, UOI_NAME, szData, sizeof(szData), &dwLen);
						CString sDesk=szData;
						CloseDesktop(desktop);
						if(sLastDesk!=sDesk){_XLOG_
							TRACE2("New desktop!! %s->%s\n",sLastDesk,sDesk);
							sLastDesk=sDesk;
							bNeedDrop=1;
						}
					}else if(sLastDesk!=""){_XLOG_
						TRACE1("New desktop!! %s->''\n",sLastDesk);
						sLastDesk="";
						bNeedDrop=1;
					}
					if(bNeedDrop){_XLOG_
						WKIHOTKEYS_ResetKB();
					}
				}
			}
		}
		////////////////////////////////////////////////
		// Смена заголовка винампа
		if(pMainDialogWindow && objSettings.lCatchWinampTitle){_XLOG_
			static HWND hwndWinamp=NULL;
			if(hwndWinamp==NULL || !IsWindow(hwndWinamp)){_XLOG_
				hwndWinamp=::FindWindow("Winamp v1.x",NULL);
			}
			if(hwndWinamp){_XLOG_
				static CString sPrevTitle="_WINAMP отдыхает_"; 
				if(dwSleepTime>TIMER_WAMPT_TICK){_XLOG_
					dwSleepTime=TIMER_WAMPT_TICK;
				}
				CString sNewTitle;
				char szNewTitle[256]="";
				::GetWindowText(hwndWinamp,szNewTitle,sizeof(szNewTitle));
				sNewTitle=szNewTitle;
				if(GetWinAmpPlStatus()==0){_XLOG_
					// Если заголовок скроллится - отсекаем это!
					if(sPrevTitle.Find(sNewTitle.Left(20))==-1){_XLOG_
						ShowWinampTitle(FALSE);
					}else{_XLOG_
						dwSleepTime=TIMER_WINAMP_FAST;
					}
				}
				sPrevTitle=sNewTitle;
			}
		}
		////////////////////////////////////////////////
		// Наступление нового дня?
		COleDateTime dt=COleDateTime::GetCurrentTime();
		if(dtLastDay==-1 || ((dt.GetHour()>11) && (dt.GetDay()!=dtLast.GetDay() || dt.GetMonth()!=dtLast.GetMonth() || dt.GetYear()!=dtLast.GetYear()))){_XLOG_
			if(objSettings.iLikStatus!=2){_XLOG_
				UpdateTrialityStatus(objSettings.sLikUser,objSettings.sLikKey);
			}
			if(objSettings.lCheckForUpdate){_XLOG_
				CheckNewVersionInThread(0);
			}
			dtLast=dt;
			if(dtLastDay==-1){_XLOG_
				dtLastDay=dt.GetDay();
			}
			// Обновляем иконку, anyway
			if(pMainDialogWindow){_XLOG_
				pMainDialogWindow->PostMessage(UPDATE_ICON,0,0);
			}
		}
		if(dt.GetDay()!=dtLastDay){_XLOG_
			if(objSettings.lShowDateInTrayIcon>0){_XLOG_
				pMainDialogWindow->PostMessage(UPDATE_ICON,0,0);
			}
			dtLastDay=dt.GetDay();
		}
		CheckAndDisablePayFeaturesIfNeeded();
		////////////////////////////////////////////////
		// Ждем до следующей проверки
		WaitForSingleObject(objSettings.hTimerEvent,dwSleepTime);
	};
	if(dwLastActivePID!=0){_XLOG_
		HANDLE hLastActiveProc=::OpenProcess(PROCESS_SET_INFORMATION,FALSE,dwLastActivePID);
		if (!( (hLastActiveProc) == NULL || (hLastActiveProc) == INVALID_HANDLE_VALUE )){_XLOG_
			SetPriorityClass(hLastActiveProc,dwLastActivePPR);
			CloseHandle(hLastActiveProc);
		}
	}
	iGlobalState=4;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// AppMainApp construction
AppMainApp::AppMainApp()
{_XLOG_
}

int AddBitmapToIList(CImageList& list, UINT bmpID)
{_XLOG_
#ifdef GDI_NOICONS
	return 0;
#endif
	CBitmap* bmp=new CBitmap;
	bmp->LoadBitmap(bmpID);
	int iRes=list.Add(bmp, RGB(255, 255, 255)); 
	delete bmp;
	return iRes;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only AppMainApp object
AppMainApp theApp;
/////////////////////////////////////////////////////////////////////////////
// AppMainApp initialization
CRect AppMainApp::rDesktopRECT;
CRect AppMainApp::rFakedRect;
CWnd* AppMainApp::m_pFakeWnd=NULL;
UINT AppMainApp::iWM_THIS=GetProgramWMMessage();
UINT AppMainApp::iWM_WIRENOTE=GetProgramWMMessage(PARENT_PROG);
UINT AppMainApp::iWM_EXTERN=RegisterWindowMessage("WK_EXTERN_STUFF");

BOOL AppMainApp::InitInstance()
{_XLOG_
	CDebugWPHelper::isDebug()=0;
	if(CLogFlow::isEnabled()){
		GlobalAddAtom("WPLABSDEBUG-WK");
		CDebugWPHelper::isDebug()=1;
	}
	CString sExe;
	GetCommandLineParameter("execute",sExe,0);
	if(sExe!=""){_XLOG_
		ExecuteMacroByString(sExe);
		return FALSE;
	}
	GetCommandLineParameter("add",sExe,0);
	if(sExe!=""){_XLOG_
		ExecuteMacroByString(CString(ADD_MACRO_PREF)+sExe);
		return FALSE;
	}
	pMainDialogWindow=NULL;
	CString sWait;
	GetCommandLineParameter("wait",sWait);
	DWORD dwStartWait=GetTickCount();
	if(sWait=="yes"){_XLOG_
		while(CheckProgrammRunState(NULL, UNIQUE_TO_TRUSTEE, false) && GetTickCount()-dwStartWait<180000){_XLOG_
			Sleep(1000);
		}
	}
	// Проверяем спайрежим
	CString sMode;
	GetCommandLineParameter("spymode",sMode);
	if(sMode.GetLength()){
		spyMode()=1;
		if(IsThisProgrammAlreadyRunning()){
			return FALSE;
		}
	}
	if(!spyMode() && isFileExist(GetAppFolder()+SPY_MODE_APP)){
		underSpyMode()=1;
		::ShellExecute(NULL,"",GetAppFolder()+SPY_MODE_APP,"-spymode=yes",GetAppFolder(),SW_HIDE);
	}
	if(!spyMode() && !underSpyMode()){
		if(IsThisProgrammAlreadyRunning()){
			DWORD dwTarget=BSM_APPLICATIONS;
			BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, iWM_THIS, WPARAM(99), LPARAM(99));
			return FALSE;
		}
	}
	CString sNoPlugin;
	GetCommandLineParameter("plugins",sNoPlugin);
	if(sNoPlugin!=""){_XLOG_
		if(sNoPlugin.CompareNoCase("no")==0 || sNoPlugin.CompareNoCase("false")==0 || sNoPlugin.CompareNoCase("0")==0){_XLOG_
			objSettings.lDoNotLoadMacros=1;
		}
	}
	objSettings.hActiveApplicationAtStart=GetForegroundWindow();
	AfxEnableControlContainer();
	//AfxInitRichEdit();
#if WINVER<=0x0050
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
	OleInitialize(NULL);
	//
	objSettings.sLikUser="";
	objSettings.sLikKey="";
	WKIHOTKEYS_CheckForExternalDLL()=TRUE;
	HANDLE hLicenseFile=::CreateFile(GetUserFolder()+LICENSE_KEY_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (!hLicenseFile || hLicenseFile==INVALID_HANDLE_VALUE){_XLOG_
		// Из локального каталога!
		hLicenseFile=::CreateFile(CString(GetApplicationDir())+LICENSE_KEY_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	}
	if (hLicenseFile && hLicenseFile!=INVALID_HANDLE_VALUE){_XLOG_
		DWORD dwRead=0;
		char szKey[2048]="";
		objSettings.sLikKey="";
		::ReadFile(hLicenseFile, szKey, sizeof(szKey), &dwRead, NULL);
		objSettings.sLikUser=CDataXMLSaver::GetInstringPart("user:[","]",szKey);
		objSettings.sLikKey=CDataXMLSaver::GetInstringPart("key:[","]",szKey);
		::CloseHandle(hLicenseFile);
	}
	dwTID=::GetCurrentThreadId();
	UpdateTrialityStatus(objSettings.sLikUser,objSettings.sLikKey);
	if(objSettings.iLikStatus<0){_XLOG_
#ifdef _DEBUG
		AfxMessageBox(Format("Anti hack! %s",GetCOMError(GetLastError())));
#endif
		CSettings* objAntiDebug=0;
		objAntiDebug->ApplySettings();
		return FALSE;
	}
	// Основные иконки
	theApp.MainImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 2);
	AddBitmapToIList(theApp.MainImageList,IDB_IMAGELIST);
	// Иконки из ImageList-а
	for(int i=0;i<theApp.MainImageList.GetImageCount();i++){_XLOG_
		HICON hIcon=theApp.MainImageList.ExtractIcon(i);
		_bmp().AddBmp(_IL(i),hIcon);
		ClearIcon(hIcon);
	}
	CString sConfFile=getFileFullName(CString(PROGNAME)+".conf");
	CString sIniFileInfo;
	ReadFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
	if(sIniFileInfo!=""){_XLOG_
		sIniFileInfo+="\r\n";
	}
	if(sIniFileInfo.Find("UserData")==-1 || sIniFileInfo.Find("ConfigFile")==-1){_XLOG_
		sIniFileInfo+="ConfigFile=";
		sIniFileInfo+=sConfFile;
		sIniFileInfo+="\r\n";
		sIniFileInfo+="UserData=";
		sIniFileInfo+=GetPathPart(sConfFile,1,1,0,0);
		sIniFileInfo+="\r\n";
		SaveFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
	}
	if(!isFileExist(sConfFile)){// если первый запуск - выбор языка
		objSettings.bStartWithOptions=TRUE;
#ifdef _DEBUG
		int iLang=0;
#else
		int iLang=(GetUserDefaultLangID()==0x0419?1:0);
#endif

		CStringArray aLangBufferNames;
		int iLangsCount=wk.GetBuffersLang(aLangBufferNames,iLang);
		// Нет главной программы. спрашиваем стартовый язык пользователя!!!
		if(iLangsCount>1){_XLOG_
			CDLG_Chooser dlgChooser(CString(PROGNAME" - ")+_l("Select language"),_l("Choose your language"),&aLangBufferNames, GetUserDefaultLangID()==0x0419?1:0, IDR_MAINICO,NULL);
			iLang=dlgChooser.DoModal();
		}

		if(iLang>=0){_XLOG_
			GetApplicationLang()=iLang;
		}
	}
	CString sConsoleMode;
	GetCommandLineParameter("console",sConsoleMode);
	if(sConsoleMode=="help"){_XLOG_
		ShowHelp("Overview");
		return 0;
	}
	::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
	// Пытаемся к пустому окну захимичится
	rFakedRect.SetRect(-10,0,0,0);
	LPCTSTR szClass = AfxRegisterWndClass(NULL);
	WNDCLASS pClassInfo;
	memset(&pClassInfo,0,sizeof(pClassInfo));
	if(GetClassInfo(AfxGetApp()->m_hInstance,szClass,&pClassInfo)){_XLOG_
		pClassInfo.lpszClassName="WK_MAIN";
		if(RegisterClass(&pClassInfo)!=0){_XLOG_
			szClass=pClassInfo.lpszClassName;
		};
	}
	m_pFakeWnd = new CWnd;
	m_pFakeWnd->CreateEx(0, szClass, "WK_ROOT", 0, rFakedRect, NULL, 0);
	m_pFakeWnd->ShowWindow(SW_HIDE);
	m_pFakeWnd->EnableWindow(FALSE);
	// Грузим настройки-обязательно после создания окна, на окно вешаются провертя
	objSettings.Load();
	// **********
	CI18N::GetTranslationIFace()=CUITranslationImpl::getInstance();
	AppRunStateFile(NULL, 0, NULL);
	InstallDefaultMacros();
	if(objSettings.bStartWithOptions){_XLOG_
		if(IsStartupWithWindows()==FALSE){_XLOG_
			StartupApplicationWithWindows(TRUE);
		}
	}
	// Создаем
	pMainDialogWindow = new AppMainDlg();
	if(!pMainDialogWindow){_XLOG_
		AfxMessageBox(Format("Failed to create main window! %s",GetCOMError(GetLastError())));
		return FALSE;
	}
	theApp.m_pMainWnd=pMainDialogWindow;// Сначала задаем главное окно, потом создаем его
	pMainDialogWindow->Create(AppMainDlg::IDD,m_pFakeWnd);
	{// Запускаем поток, следящий за всем
		DWORD dwTimerTrackerId=0;
		HANDLE TimerTracker=::CreateThread(NULL, 0, GlobalOnTimer, NULL, 0, &dwTimerTrackerId);
		CloseHandle(TimerTracker);
	}
	if(objSettings.bStartWithOptions){_XLOG_
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_ABOUT,0);
		objSettings.bApplicationFirstRun=TRUE;
	}
	objSettings.bStartWithOptions=FALSE;
	// Все!
	return TRUE;
}

int AppMainApp::ExitInstance() 
{_XLOG_
	AppRunStateFile(NULL, -1, NULL);
	if(m_pFakeWnd){_XLOG_
		m_pFakeWnd->DestroyWindow();
		delete m_pFakeWnd;
	}
	//Тут на выходе зависает...
	//Возможно слишком поздно. не будем деинициализировать
	//OleUninitialize();
	return CWinApp::ExitInstance();
}

LRESULT AppMainApp::ProcessWndProcException(CException* e, const MSG* pMsg) 
{_XLOG_
	return CWinApp::ProcessWndProcException(e, pMsg);
}

BOOL AppMainApp::PreTranslateMessage(MSG* pMsg) 
{_XLOG_
	return CWinApp::PreTranslateMessage(pMsg);
}

CWnd* AfxWPGetMainWnd()
{_XLOG_
	return pMainDialogWindow;
}

BOOL& spyMode()
{
	static BOOL b=0;
	return b;
}

BOOL& underSpyMode()
{
	static BOOL b=0;
	return b;
}

#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif

#pragma message("Long perspective - минимизировать вызовы EscapeStringUTF8, EscapeString, DeEntitize")
#pragma message("Long perspective - макросы в проигрывателе записывать для WireKeys. без плагина")
 
 
 