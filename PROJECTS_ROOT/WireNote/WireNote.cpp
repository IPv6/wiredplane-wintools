#include "stdafx.h"
// WPOSSOrganizator.cpp : Defines the class behaviors for the application.
//
#include <crtdbg.h>
#include "_common.h"
#include "wirenote.h"
#include "..\SmartWires\Atl\filemap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma comment(exestr,"WireNote. Copyright ©2002-2003 WiredPlane.com. All rights reserved. Build time:"__TIMESTAMP__)
const char* szBuildDate=__TIMESTAMP__;
CLogActions objLog;
CSettings objSettings;
CNewVersionChecker& GetVersionChecker();
AppMainDlg* pMainDialogWindow=NULL;
DWORD CWPOSSOrganizatorApp::aAppIcons[TRAY_ICONS]={IDR_MAINICO,IDI_ICON_MAIN2,IDI_ICON_MAIN3,IDI_ICON_MAIN4,IDI_ICON_MAIN5,IDI_ICON_MAIN6,IDI_ICON_MAIN7};
CWPOSSOrganizatorApp theApp;// The one and only CWPOSSOrganizatorApp object
BOOL& IsEmbedded();
void StartCheckNewVersionX(DWORD dwDays)
{
	if(IsEmbedded()){
		return;
	}
	StartCheckNewVersion(dwDays);
}

int WNTrialityStatus(CString& szUserName, CString& szUKey)
{
	int iRes=TrialityStatus(szUserName, szUKey);
	if(iRes!=2){
		szUserName="WiredPlane.com";
		szUKey="If you like this program, you can donate authors by registering WireNote";
	}
	return 2;
}
//
BOOL bStartingStatusRefreshener=FALSE;
DWORD WINAPI GlobalOnTimer(LPVOID pValue)
{
	COleDateTime dtLast=COleDateTime::GetCurrentTime();
	while(pMainDialogWindow){
		if(pMainDialogWindow->iRunStatus==-2){
			return 0;
		}
		if(pMainDialogWindow->iRunStatus!=1){
			Sleep(500);
			continue;
		}
		////////////////////////////////////////////////
		// Вызываем обработчик в диалоге...
		PostMessage(pMainDialogWindow->GetSafeHwnd(),WM_TIMER,ID_TIMER,NULL);
		////////////////////////////////////////////////
		// 2. Обновление статуса сетевый пользователей
		DWORD dwMilSecsTillLastRefresh=GetTickCount()-objSettings.iStatusUpdateTimeLastTCount;
		// нужно ли обновить статус...
		if(objSettings.iStatusUpdateTime!=0 &&
			(dwMilSecsTillLastRefresh>DWORD(objSettings.iStatusUpdateTime*60*1000)
			||(bStartingStatusRefreshener==FALSE && objSettings.bNetSubsystemInitialized && dwMilSecsTillLastRefresh>3000))){
			bStartingStatusRefreshener=TRUE;
			ExchangeOnlineStatuses();
		}
		////////////////////////////////////////////////
		// 3. Автоматическое сохранение всех данных
		dwMilSecsTillLastRefresh=GetTickCount()-pMainDialogWindow->dwAutoSaveCounter;
		if(objSettings.dwAutoSaveTime!=0 && dwMilSecsTillLastRefresh>DWORD(objSettings.dwAutoSaveTime*60*1000)){
			pMainDialogWindow->OnSaveallBt();
		}
		////////////////////////////////////////////////
		// 4. Наступление нового дня/часа/месяца/года?
		COleDateTime dt=COleDateTime::GetCurrentTime();
		if((dt.GetHour()!=dtLast.GetHour() || dt.GetDay()!=dtLast.GetDay() || dt.GetMonth()!=dtLast.GetMonth() || dt.GetYear()!=dtLast.GetYear()) && pMainDialogWindow){
			pMainDialogWindow->SetWindowIcon();
			objSettings.iLikStatus=WNTrialityStatus(objSettings.sLikUser,objSettings.sLikKey);
			CItem::SetModify(TRUE);
			if(objSettings.lCheckForUpdate){
				GetVersionChecker().SetTraits();
				StartCheckNewVersionX(objSettings.lDaysBetweenChecks);
			}
		}
		dtLast=dt;
		////////////////////////////////////////////////
		// 8. Проверяем почтовые ящики
		for(int si=0;si<objSettings.lAntiSpamAccounts;si++){
			if(!objSettings.antiSpamAccounts[si].lActive || objSettings.antiSpamAccounts[si].dwCheckingPeriod<=0 || objSettings.antiSpamAccounts[si].lCurrentStatus!=0){
				continue;
			}
			COleDateTime dt=objSettings.dStartTime;
			if(objSettings.antiSpamAccounts[si].dtLastCheck.GetStatus()!=COleDateTime::invalid){
				dt=objSettings.antiSpamAccounts[si].dtLastCheck;
			}
			dt+=COleDateTimeSpan(0,0,objSettings.antiSpamAccounts[si].dwCheckingPeriod,0);
			if(dt<COleDateTime::GetCurrentTime()){
				FORK(CheckMailInTread,&objSettings.antiSpamAccounts[si]);
			}else{
				objSettings.antiSpamAccounts[si].dtNextCheck=dt;
			}
		}
		////////////////////////////////////////////////
		// 9. Обновляем заметки после лока
		BOOL bInLock=(isScreenSaverActive() || isWindowStationLocked());
		static BOOL bInLockPrev=0;
		if(bInLockPrev){
			if(!bInLock){
				for(int i=0;i<objSettings.m_Notes.GetSize();i++){
					CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
					if(!pNote){
						continue;
					}
					objSettings.m_Notes.GetNote(i)->ActivateNoteParams(pNote->IsWindowVisible());
					objSettings.m_Notes.GetNote(i)->Invalidate();
				}
			}
		}
		bInLockPrev=bInLock;
		////////////////////////////////////////////////
		// 10. Ждем до следующей проверки
		Sleep(TIMER_SETTIMER_TICK);
	};
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CWPOSSOrganizatorApp

BEGIN_MESSAGE_MAP(CWPOSSOrganizatorApp, CWinApp)
	//{{AFX_MSG_MAP(CWPOSSOrganizatorApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWPOSSOrganizatorApp construction

CWPOSSOrganizatorApp::CWPOSSOrganizatorApp()
{
}

int AddBitmapToIList(CImageList& list, UINT bmpID, BOOL bWithHeap)
{
	CBitmap* bmp=NULL;
	BOOL bNeedFree=FALSE;
	if(bWithHeap){
		bmp=_bmp().GetRaw(bmpID,CSize(16,16));
	}else{
		bmp=new CBitmap;
		bmp->LoadBitmap(bmpID);
		bNeedFree=TRUE;
	}
	int iRes=list.Add(bmp, RGB(255, 255, 255)); 
	if(bNeedFree){
		delete bmp;
	}
	return iRes;
}


BOOL IsWindows2000()
{
	static BOOL bIsWin2000=-1;
	if(bIsWin2000==-1){
		bIsWin2000=0;
		OSVERSIONINFOEX winfo; 
		ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
		winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
		if (GetVersionEx((OSVERSIONINFO *)&winfo)){ 
			bIsWin2000=(winfo.dwPlatformId==VER_PLATFORM_WIN32_NT) && (winfo.dwMajorVersion>=5);
		} 
	}
	return bIsWin2000;
}

/*BOOL IsVista()
{
	static BOOL bIsWinV=-1;
	if(bIsWinV==-1){
		bIsWinV=0;
		OSVERSIONINFOEX winfo; 
		ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
		winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
		if (GetVersionEx((OSVERSIONINFO *)&winfo)){ 
			bIsWinV=(winfo.dwMajorVersion>=6);
		} 
	}
	return bIsWinV;
}*/

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

#pragma comment(lib, "advapi32")
BOOL IsUserAdmin( PBOOL pbAdmin )
{
	if(IsWindows98()){
		*pbAdmin=TRUE;
		return TRUE;
	}
    HANDLE hAccessToken       = NULL;
    PBYTE  pInfoBuffer        = NULL;
    DWORD  dwInfoBufferSize   = 1024;    // начальный размер буфера
    PTOKEN_GROUPS ptgGroups   = NULL;
    PSID   psidAdministrators = NULL;
    SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
    BOOL   bResult = FALSE;
    
    __try
    {
        // инициализация идентификатора безопасности
        if( !AllocateAndInitializeSid( 
            &siaNtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS, 0,0,0,0,0,0, &psidAdministrators ) )
			__leave;
        
		//if(!IsWindows2000())
		{
			// код для предшественников Windows2000
			
			// откроем токен процесса
			if( !OpenProcessToken( GetCurrentProcess(),TOKEN_QUERY,&hAccessToken ) )
				__leave;
			
			do  // выделение буфера для запрошенной из токена информации
			{
				if( pInfoBuffer )
					delete pInfoBuffer;
				pInfoBuffer = new BYTE[dwInfoBufferSize];
				if( !pInfoBuffer )
					__leave;
				SetLastError( 0 );
				if( !GetTokenInformation( 
                    hAccessToken, 
                    TokenGroups, pInfoBuffer,
                    dwInfoBufferSize, &dwInfoBufferSize ) &&
					( ERROR_INSUFFICIENT_BUFFER != GetLastError() )
					)
					__leave;
				else
					ptgGroups = (PTOKEN_GROUPS)pInfoBuffer;
			}
			while( GetLastError() ); // если была ошибка, значит начального размера недостаточно
			
			// переберем идентификаторы безопасности процесса в поисках необходимого нам
			for( UINT i = 0; i < ptgGroups->GroupCount; i++ )
			{
				if( EqualSid(psidAdministrators,ptgGroups->Groups[i].Sid) )
				{
					*pbAdmin = TRUE;
					bResult  = TRUE;
					__leave;
				}
			} 
		}
		/*
		else{
			// код для Windows2000
			bResult = CheckTokenMembership( NULL, psidAdministrators, pbAdmin );            
		}
		*/
    }
    __finally
    {
		//if(!IsWindows2000())
		{
			if( hAccessToken )
				CloseHandle( hAccessToken );
			if( pInfoBuffer )
				delete[] pInfoBuffer;
		}
        if( psidAdministrators )
            FreeSid( psidAdministrators );
    }
    
    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CWPOSSOrganizatorApp initialization
CRect CWPOSSOrganizatorApp::rDesktopRECT;
CRect CWPOSSOrganizatorApp::rFakedRect;
CWnd* CWPOSSOrganizatorApp::m_pFakeWnd=NULL;
UINT CWPOSSOrganizatorApp::iWM_THIS=GetProgramWMMessage();
UINT CWPOSSOrganizatorApp::iWM_WIREKEYS=GetProgramWMMessage("WireKeys");
UINT CWPOSSOrganizatorApp::iWM_WIRECHANGER=GetProgramWMMessage("WireChanger");
BOOL& IsEmbedded()
{
	static BOOL isEmbed=0;
	return isEmbed;
}

BOOL CWPOSSOrganizatorApp::InitInstance()
{
	pMainDialogWindow=NULL;
	CString sWait;
	GetCommandLineParameter("wait",sWait);
	DWORD dwStartWait=GetTickCount();
	if(sWait=="yes"){
		while(IsThisProgrammAlreadyRunning() && GetTickCount()-dwStartWait<180000){
			Sleep(1000);
		}
	}
	//AfxMessageBox(AppCommandLine());
	CString sEmbed;
	GetCommandLineParameter("embed",sEmbed);
	if(sEmbed=="yes"){
		IsEmbedded()=1;
		if(IsThisProgrammAlreadyRunning()){
			return FALSE;
		}
	}
	if(!IsEmbedded() && IsThisProgrammAlreadyRunning()){
		DWORD dwTarget=BSM_APPLICATIONS;
		BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, iWM_THIS, WPARAM(99), LPARAM(99));
		return FALSE;
	}
	AfxEnableControlContainer();
	//if(!AfxInitRichEdit2())
	AfxInitRichEdit();
#if WINVER<=0x0050
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
	OleInitialize(NULL);
	dwTID=::GetCurrentThreadId();
	if(!IsEmbedded()){
		CString sAbThisFile=getFileFullName(CString(PROGNAME)+".conf");
		CString sIniFileInfo;
		ReadFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
		if(sIniFileInfo!=""){
			sIniFileInfo+="\r\n";
		}
		if(sIniFileInfo.Find("UserData")==-1 || sIniFileInfo.Find("ConfigFile")==-1){
			sIniFileInfo+="ConfigFile=";
			sIniFileInfo+=sAbThisFile;
			sIniFileInfo+="\r\n";
			sIniFileInfo+="UserData=";
			sIniFileInfo+=GetPathPart(sAbThisFile,1,1,0,0);
			sIniFileInfo+="\r\n";
			SaveFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
		}
		CString sMainIniFile;
		GetCommandLineParameter("import",sMainIniFile);
		if(sMainIniFile==""){
			GetCommandLineParameter("io",sMainIniFile);
		}
		CString sAbIniFile;
		GetCommandLineParameter("importab",sAbIniFile);
		if(sMainIniFile==""){
			GetCommandLineParameter("ia",sAbIniFile);
		}
		if(sMainIniFile!=""){
			if(isFileExist(sMainIniFile)){
				objSettings.Load();
				objSettings.Load(sMainIniFile,1);
				objSettings.Save();
				AfxMessageBox(sMainIniFile+" - import finished");
			}else{
				AfxMessageBox(sMainIniFile+" - file not found!");
			}
		}
		if(sAbIniFile!=""){
			if(isFileExist(sAbIniFile)){
				CString sAbThisFile=getFileFullName(objSettings.sAddrFile);
				DeleteFile(sAbThisFile+".old");
				MoveFile(sAbThisFile,sAbThisFile+".old");
				CopyFile(sAbIniFile,sAbThisFile,FALSE);
				AfxMessageBox(sAbIniFile+" - import finished");
			}else{
				AfxMessageBox(sAbIniFile+" - file not found!");
			}
		}
		if((sAbIniFile!="" ||sMainIniFile!="") && sWait!="yes"){
			return FALSE;
		}
		//
		objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Initiaizing")+" "+PROGNAME+"...");
		HANDLE hLicenseFile=::CreateFile(GetUserFolder()+LICENSE_KEY_FILE, GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
		if (hLicenseFile && hLicenseFile!=INVALID_HANDLE_VALUE){
			DWORD dwRead=0;
			char szKey[2048]="";
			objSettings.sLikKey="";
			::ReadFile(hLicenseFile, szKey, sizeof(szKey), &dwRead, NULL);
			objSettings.sLikUser=CDataXMLSaver::GetInstringPart("user:[","]",szKey);
			objSettings.sLikKey=CDataXMLSaver::GetInstringPart("key:[","]",szKey);
			::CloseHandle(hLicenseFile);
		}
		objSettings.iLikStatus=WNTrialityStatus(objSettings.sLikUser,objSettings.sLikKey);
	}else{
		objSettings.iLikStatus=2;
		//objSettings.bNoSend=TRUE;//email все равно работает
	}
#ifdef GPL
	objSettings.iLikStatus=2;
#endif
	if(objSettings.iLikStatus<0){
		CSettings* objAntiDebug=0;
		objAntiDebug->ApplySettings();
		return FALSE;
	}
#ifdef SEND_ADMINS_ONLY
	//BOOL bAdmin=0;
	//IsUserAdmin(&bAdmin);
	//objSettings.bNoSend=!bAdmin;
	objSettings.bNoSend=TRUE;
#endif
	objSettings.Load();
	{
		aPriorityStrings.Add("Lowest");
		aPriorityStrings.Add("Low");
		aPriorityStrings.Add("Normal");
		aPriorityStrings.Add("High");
		aPriorityStrings.Add("Highest");
		//
		CBitmap* pBM=NULL;
		theApp.MainImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 2);
		// Основные иконки
		AddBitmapToIList(theApp.MainImageList,IDB_IMAGELIST);
		// Иконки для статусов
		dwOnlineStatusStartBitmap=AddBitmapToIList(theApp.MainImageList,IDB_BM_PONLINE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_PAWAY);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_PBUSY);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_PDND);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_POFFLINE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_PGROUP);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_PUNKNOWN);
		// Для мессаг грузим некоторые иконки сразу
		dwMsgBitmap=AddBitmapToIList(theApp.MainImageList,IDB_BM_NEWMESSAGE,TRUE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_INMESSAGE,TRUE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_INMESSAGE_F1,TRUE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_INMESSAGE_F2,TRUE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_INMESSAGE_F3,TRUE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_INMESSAGE_F4,TRUE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_INMESSAGE_F5,TRUE);
		AddBitmapToIList(theApp.MainImageList,IDB_BM_INMESSAGE_F6,TRUE);
		// Иконки из ImageList-а
		for(int i=0;i<theApp.MainImageList.GetImageCount();i++){
			HICON hIcon=theApp.MainImageList.ExtractIcon(i);
			_bmp().AddBmpRaw(_IL(i),hIcon);
			ClearIcon(hIcon);
		}
		// Иконки из ImageList-а в оригинальном размере
		for(int j=0;j<theApp.MainImageList.GetImageCount();j++){
			HICON hIcon=theApp.MainImageList.ExtractIcon(j);
			_bmp().AddBmpRaw(_IL(j,BIGLIST_ID),hIcon,CSize(16,16));
			ClearIcon(hIcon);
		}
		//Иконка заметки - в большом варианте
		_bmp().AddBmpRaw(_IL(6,BIGLIST_ID),_bmp().GetRaw(IDB_BM_NEWNOTE2,CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE),BIGLIST_ID),CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE));
		// Для мессаг Перегружаем большие варианты картинок
		_bmp().AddBmpRaw(_IL(dwMsgBitmap,BIGLIST_ID),_bmp().GetRaw(IDB_BM_NEWMESSAGE,CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE),BIGLIST_ID),CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE));
		_bmp().AddBmpRaw(_IL(dwMsgBitmap+1,BIGLIST_ID),_bmp().GetRaw(IDB_BM_INMESSAGE,CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE),BIGLIST_ID),CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE));

		CSize zBig=GetBitmapSize(_bmp().GetRaw(IDB_BM_INMESSAGE_F1));
		_bmp().AddBmpRaw(_IL(dwMsgBitmap+2,BIGLIST_ID),_bmp().GetRaw(IDB_BM_INMESSAGE_F1,CSize(-1,-1),"TMP_OR"),CSize(-1,-1));
		_bmp().AddBmpRaw(_IL(dwMsgBitmap+3,BIGLIST_ID),_bmp().GetRaw(IDB_BM_INMESSAGE_F2,CSize(-1,-1),"TMP_OR"),CSize(-1,-1));
		_bmp().AddBmpRaw(_IL(dwMsgBitmap+4,BIGLIST_ID),_bmp().GetRaw(IDB_BM_INMESSAGE_F3,CSize(-1,-1),"TMP_OR"),CSize(-1,-1));
		_bmp().AddBmpRaw(_IL(dwMsgBitmap+5,BIGLIST_ID),_bmp().GetRaw(IDB_BM_INMESSAGE_F4,CSize(-1,-1),"TMP_OR"),CSize(-1,-1));
		_bmp().AddBmpRaw(_IL(dwMsgBitmap+6,BIGLIST_ID),_bmp().GetRaw(IDB_BM_INMESSAGE_F5,CSize(-1,-1),"TMP_OR"),CSize(-1,-1));
		_bmp().AddBmpRaw(_IL(dwMsgBitmap+7,BIGLIST_ID),_bmp().GetRaw(IDB_BM_INMESSAGE_F6,CSize(-1,-1),"TMP_OR"),CSize(-1,-1));
		//
		_bmp().GetRaw(IDB_PINNED_UP,CSize(16,16));
		_bmp().GetRaw(IDB_PINNED_DOWN,CSize(16,16));
		_bmp().GetRaw(IDB_PINNED_UP_APP,CSize(16,16));
		_bmp().GetRaw(IDB_BM_MSG_OK,CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE),BIGLIST_ID);
		_bmp().GetRaw(IDB_BM_MSG_ERR,CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE),BIGLIST_ID);
		_bmp().GetRaw(IDB_BM_MSG_IN,CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE),BIGLIST_ID);
		objSettings.MessageNoteSettings.SetIconNum(dwMsgBitmap);
		//
		_bmp().AddBmpRaw("DDOWN",_bmp().GetRaw(IDB_BM_SKREPKA,CSize(12,12)));
		_bmp().AddBmp("WireKeys",LoadIcon(IDI_ICON_WIREKEYS));
		_bmp().AddBmp("WireChanger",LoadIcon(IDI_CHWALLPICON));
		_bmp().AddBmpRaw(_bmp().GetResName(IDI_ICON_INFO_AST,BIGLIST_ID),LoadIcon(IDI_ICON_INFO_AST));
		_bmp().AddBmpRaw(_bmp().GetResName(IDI_ICON_ASK,BIGLIST_ID),LoadIcon(IDI_ICON_ASK));
		//
	}
	::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
	// Пытаемся к пустому окну захимичится
	rFakedRect.SetRect(-10,0,0,0);
	LPCTSTR szClass = AfxRegisterWndClass(NULL);
	m_pFakeWnd = new CWnd;
	m_pFakeWnd->CreateEx(0, szClass, "WN_ROOT", 0, rFakedRect, NULL, 0);
	m_pFakeWnd->ShowWindow(SW_HIDE);
	m_pFakeWnd->EnableWindow(FALSE);
	HotkeysSkipDD()=1;
	// Если при запуске небыло найдено ini-файла, показываем опции...
	if(objSettings.bStartWithOptions){
		if(!IsEmbedded()){
			if(IsStartupWithWindows()==FALSE){
				StartupApplicationWithWindows(TRUE);
			}
			//
			objSettings.OpenOptionsDialog("");
		}
	}
	// Создаем
	pMainDialogWindow = new AppMainDlg();
	if(!pMainDialogWindow){
		return FALSE;
	}
	theApp.m_pMainWnd=pMainDialogWindow;// Сначала задаем главное окно, потом создаем его
	pMainDialogWindow->Create(AppMainDlg::IDD,m_pFakeWnd);
	// Запускаем таймер...
	DWORD dwTimerTrackerId=0;
	HANDLE TimerTracker=::CreateThread(NULL, 0, GlobalOnTimer, NULL, 0, &dwTimerTrackerId);
	CloseHandle(TimerTracker);
	if(IsEmbedded()){
		CString sWnd;
		sWnd.Format("%i",pMainDialogWindow->GetSafeHwnd());
		//SaveFile(GetUserFolder()+"HID.txt",sWnd);
		CFileMap fMap;
		fMap.Open("WK_WN_WND",true,256);
		char* sz=(char*)fMap.Base();
		strcpy(sz,sWnd);
		//
		HWND hProgman=::FindWindow("Progman",NULL);
		if(hProgman){
			::SetProp(hProgman,"WKP_NOTW",(LPVOID)pMainDialogWindow->GetSafeHwnd());
		}
	}
	// Все!
	return TRUE;
}

int CWPOSSOrganizatorApp::ExitInstance() 
{
	if(m_pFakeWnd){
		m_pFakeWnd->DestroyWindow();
		delete m_pFakeWnd;
	}
	CoUninitialize();
	return CWinApp::ExitInstance();
}

LRESULT CWPOSSOrganizatorApp::ProcessWndProcException(CException* e, const MSG* pMsg) 
{
	objLog.AddMsgLogLine(DEFAULT_GENERALLOG,"Error: Unhandled exception!");
	if(e){
		char szBuffer[1024];
		e->GetErrorMessage(szBuffer,sizeof(szBuffer));
		objLog.AddMsgLogLine(DEFAULT_GENERALLOG,szBuffer);
	}
	//assertWrapperObject.WriteAssert("WndProc Unhandled exception");
	return CWinApp::ProcessWndProcException(e, pMsg);
}

BOOL CWPOSSOrganizatorApp::PreTranslateMessage(MSG* pMsg) 
{
	return CWinApp::PreTranslateMessage(pMsg);
}


//Есть предел заметки меньше которого её уменьшить нельзя. Надо бы это убрать.  ) По Автоматической подгонке размера подгоняется с запасом снизу. Хотелось бы что было впритык, как и со всех других сторон.
//не шифровать заметки, по опции