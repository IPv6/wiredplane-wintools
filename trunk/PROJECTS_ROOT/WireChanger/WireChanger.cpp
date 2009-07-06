//
#include <crtdbg.h>
#include "stdafx.h"
#include "_common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma comment(exestr,"Copyright ©2002-2006 WiredPlane.com. All rights reserved. Build time:"__TIMESTAMP__)
const char* szBuildDate=__TIMESTAMP__;
CSettings objSettings;
AppMainDlg* pMainDialogWindow=NULL;
AppMainDlg* pMainDialogWindowCrypt=NULL;
extern BOOL bAddOnStartCal;
extern BOOL bAddOnStartClo;
/////////////////////////////////////////////////////////////////////////////
// AppMainApp
BEGIN_MESSAGE_MAP(AppMainApp, CWinApp)
	//{{AFX_MSG_MAP(AppMainApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppMainApp construction

AppMainApp::AppMainApp()
{
}

int AddBitmapToIList(CImageList& list, UINT bmpID)
{
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
BOOL AppMainApp::InitATL()
{
	m_bATLInited = TRUE;
	HRESULT hRes = OleInitialize(NULL);
/*#if _WIN32_WINNT >= 0x0400
		HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
		HRESULT hRes = CoInitialize(NULL);
#endif*/
	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
		return FALSE;
	}
	return TRUE;
}

BOOL IsActiveDesctopEnable();
BOOL SetActiveDState(BOOL bEnabled);
BOOL SetWallpaperActiveD(const char* szFile, BOOL bTile=-1, int iStyle=-1)
{
	BOOL bNeedActiveDesktop=!(strstr(szFile,".bmp")!=NULL);
	BOOL bActiveDEnabled=IsActiveDesctopEnable();
	if(!bNeedActiveDesktop){
		if(bActiveDEnabled){// Выключаем ActiveDesktop если он не нужен
			SetActiveDState(FALSE);
		}
		SystemParametersInfo(SPI_SETDESKWALLPAPER,0,(void*)szFile,SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);
		return TRUE;
	}
	HRESULT hr=S_FALSE;
	LPACTIVEDESKTOP pIAD=NULL;
	// Инициализируем интерфейс IActiveDesktop
	CoInitialize(NULL); // Вот почему не срабатывает CoCreateInstance
	hr = CoCreateInstance (CLSID_ActiveDesktop,NULL,CLSCTX_INPROC_SERVER,IID_IActiveDesktop,(void**) &pIAD );
	if (SUCCEEDED(hr)){
		if(pIAD){
			if(bNeedActiveDesktop && !bActiveDEnabled){// Включаем ActiveDesktop если он нужен
				//SetActiveDState(TRUE);
				COMPONENTSOPT opt;
				opt.dwSize = sizeof(opt);
				opt.fActiveDesktop = opt.fEnableComponents = TRUE;
				pIAD->SetDesktopItemOptions(&opt,0);
				pIAD->ApplyChanges(AD_APPLY_ALL);
			}
			USES_CONVERSION;
			WALLPAPEROPT wOpt;
			wOpt.dwSize=sizeof(wOpt);
			wOpt.dwStyle=(iStyle==-1)?WPSTYLE_TILE:iStyle;
			pIAD->SetWallpaperOptions(&wOpt,0);
			hr=pIAD->SetWallpaper(A2W(szFile),0);
			pIAD->ApplyChanges(AD_APPLY_ALL);//AD_APPLY_BUFFERED_REFRESH
			pIAD->Release();
		}else{
			hr=S_FALSE;
		}
	}
	CoUninitialize();
	return SUCCEEDED(hr) && (bActiveDEnabled==TRUE);
}

void RestoreWP(CString& sIniFileInfo,BOOL bWithScr)
{
	CString sWP=CDataXMLSaver::GetInstringPart("CPD-W=<",">",sIniFileInfo);
	if(sWP==""){
		return;
	}
	CString s,sStyle;
	{
		CRegKey key;
		key.Open(HKEY_CURRENT_USER, "Control Panel\\Desktop");
		if(key!=NULL){
			s=CDataXMLSaver::GetInstringPart("CPD-W=<",">",sIniFileInfo);
			if(s!=""){
				RegSetValueEx(key.m_hKey,"Wallpaper",0,REG_SZ,(BYTE*)(LPCSTR)s,strlen(s)+1);
			}
			s=CDataXMLSaver::GetInstringPart("CPD-WS=<",">",sIniFileInfo);
			sStyle=s;
			if(s!=""){
				RegSetValueEx(key.m_hKey,"WallpaperStyle",0,REG_SZ,(BYTE*)(LPCSTR)s,strlen(s)+1);
			}
			s=CDataXMLSaver::GetInstringPart("CPD-WT=<",">",sIniFileInfo);
			if(s!=""){
				RegSetValueEx(key.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)(LPCSTR)s,strlen(s)+1);
			}
			if(bWithScr){
				s=CDataXMLSaver::GetInstringPart("CPD-SS=<",">",sIniFileInfo);
				if(s!=""){
					RegSetValueEx(key.m_hKey,"SCRNSAVE.EXE",0,REG_SZ,(BYTE*)(LPCSTR)s,strlen(s)+1);
				}
			}
		}
	}
	if(!SetWallpaperActiveD(sWP,-1,atol(sStyle))){
		::SystemParametersInfo(SPI_SETDESKWALLPAPER,0,(void*)(const char*)sWP,SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);
	}
	{
		CRegKey key;
		key.Open(HKEY_USERS, ".DEFAULT\\Control Panel\\Desktop");
		if(key!=NULL){
			char szTemp[MAX_PATH]="";
			DWORD lSize = MAX_PATH,dwType=0;
			s=CDataXMLSaver::GetInstringPart("DCPD-W=<",">",sIniFileInfo);
			if(s!=""){
				RegSetValueEx(key.m_hKey,"Wallpaper",0,REG_SZ,(BYTE*)(LPCSTR)s,strlen(s)+1);
			}
			s=CDataXMLSaver::GetInstringPart("DCPD-WS=<",">",sIniFileInfo);
			if(s!=""){
				RegSetValueEx(key.m_hKey,"WallpaperStyle",0,REG_SZ,(BYTE*)(LPCSTR)s,strlen(s)+1);
			}
			s=CDataXMLSaver::GetInstringPart("DCPD-WT=<",">",sIniFileInfo);
			if(s!=""){
				RegSetValueEx(key.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)(LPCSTR)s,strlen(s)+1);
			}
		}
	}
}

int& AddDefsWallps()
{
	static int c=1;
	return c;
}

int& AddDefsInteract()
{
	static int c=1;
	return c;
}

CString& AppName()
{
    static CString s=PROGNAME;
    return s;
}

long USE_ELEFUN=0;
CRect AppMainApp::rFakedRect;
CWnd* AppMainApp::m_pFakeWnd=NULL;
UINT AppMainApp::iWM_THIS=GetProgramWMMessage();
UINT AppMainApp::iWM_SSAVE=RegisterWindowMessage(CString("WC_SHOWSSAVE_OPTS"));
UINT AppMainApp::iWM_WIRENOTE=GetProgramWMMessage(PARENT_PROG);
BOOL AppMainApp::InitInstance()
{
	pMainDialogWindowCrypt = NULL;
	pMainDialogWindow=NULL;
	if (!InitATL()){
		return FALSE;
	}
	{// На первый запуск...
		char sz1[128]={0};
		char sz2[128]="1";
		CString sAddCal;
		GetCommandLineParameter("addcal",sAddCal);
		if(sAddCal!=""){
			strcpy(sz1,sAddCal);
			SetRegSetting("", "FR_addCalendar", sz1);
			return FALSE;
		}
		CString sAddClo;
		GetCommandLineParameter("addclo",sAddClo);
		if(sAddClo!=""){
			strcpy(sz2,sAddClo);
			SetRegSetting("", "FR_addClock", sz2);
			return FALSE;
		}
		GetRegSetting("", "FR_addClock", sz1, sizeof(sz1));
		bAddOnStartClo=atol(sz1);
		GetRegSetting("", "FR_addCalendar", sz2, sizeof(sz2));
		bAddOnStartCal=atol(sz2);
	}
	{//setup_file
		// Регистрируем файл
		char szWkmRegistered[32]={0};
		GetRegSetting("", "wpc_setupRegistered", szWkmRegistered, sizeof(szWkmRegistered));
		if(szWkmRegistered[0]==0){
			strcpy(szWkmRegistered,"yes");
			SetRegSetting("", "wpc_setupRegistered", szWkmRegistered);
			RegisterExtension("wpc_setup","WireChanger setup file","-setup_file=");
		}
		CString sSetupFile;
		GetCommandLineParameter("setup_file",sSetupFile);
		if(sSetupFile!=""){
			if(isFileExist(sSetupFile)){
				CString sContent,sKeyPart;
				ReadFile(sSetupFile,sContent);
				CString sXML=CDataXMLSaver::GetInstringPart("<SETUP>","</SETUP>",sContent);
				if(sXML==""){
					sKeyPart=sContent;
				}
				// Делаем что сказано...
				if(sKeyPart!=""){
					SaveFile(GetUserFolder()+LICENSE_KEY_FILE,sKeyPart);
					AfxMessageBox(_l("Registration info installed successfully\nRestart WireChanger to see changes"));
				}
			}else{
				AfxMessageBox(Format("Reading setup file error: '%s' not found!",sSetupFile));
			}
			return FALSE;
		}
	}
	CString sExe;
	GetCommandLineParameter("add",sExe,0);
	if(sExe!=""){_XLOG_
		CString sNewFile=CString(GetApplicationDir())+WP_TEMPLATE+"\\"+GetPathPart(sExe,0,0,1,1);
		BOOL b=CopyFile(sExe,sNewFile,TRUE);
		if(!b){
			AfxMessageBox(_l("Error")+": "+_l("Widget already exist"));
		}
		return FALSE;
	}
    // Глобальные настройки
    CString sDats;
    ReadFile(CString(GetApplicationDir())+"inits.txt",sDats);
    AppName()=PROGNAME;
    AddDefsWallps()=1;
	AddDefsInteract()=1;
	if(sDats.GetLength()){
		CString sAppName=CDataXMLSaver::GetInstringPart("app:[","]",sDats);
		if(sAppName.GetLength()!=0){
			AppName()=sAppName;
		}
		AddDefsWallps()=atol(CDataXMLSaver::GetInstringPart("defs:[","]",sDats));
		AddDefsInteract()=atol(CDataXMLSaver::GetInstringPart("intr:[","]",sDats));
	}
    //===========
	CString sConsoleMode;
	GetCommandLineParameter("console",sConsoleMode);
	CString sIniFileInfo;
	ReadFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
	if(sIniFileInfo!=""){
		sIniFileInfo+="\r\n";
	}
	if(sConsoleMode=="yes"){
		CString sConsoleSave,sConsoleRest;
		GetCommandLineParameter("wpsave",sConsoleSave);
		GetCommandLineParameter("wprestore",sConsoleRest);
		if(sConsoleSave=="yes"){
			if(sIniFileInfo.Find("[Main]")==-1){
				sIniFileInfo+="[Main]\r\n";
			}
			
			CString sAID,sDte;
			GetCommandLineParameter("affid",sAID);
			GetCommandLineParameter("date",sDte);
			if(sIniFileInfo.Find("AffId=")==-1){
				sIniFileInfo+=CString("AffId=")+sAID+"\r\n";
			}
			if(sIniFileInfo.Find("Date=")==-1){
				sIniFileInfo+=CString("Date=")+sDte+"\r\n";
			}
			if(sIniFileInfo.Find("CPD-W")==-1){
				CRegKey key;
				key.Open(HKEY_CURRENT_USER, "Control Panel\\Desktop");
				if(key!=NULL){
					char szTemp[MAX_PATH]="";
					DWORD lSize,dwType=0;
					lSize = MAX_PATH;
					if(RegQueryValueEx(key.m_hKey,"Wallpaper",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){
						sIniFileInfo+="CPD-W=<";
						sIniFileInfo+=szTemp;
						sIniFileInfo+=">\r\n";
					}
					lSize = MAX_PATH;
					if(RegQueryValueEx(key.m_hKey,"WallpaperStyle",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){
						sIniFileInfo+="CPD-WS=<";
						sIniFileInfo+=szTemp;
						sIniFileInfo+=">\r\n";
					}
					lSize = MAX_PATH;
					if(RegQueryValueEx(key.m_hKey,"TileWallpaper",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){
						sIniFileInfo+="CPD-WT=<";
						sIniFileInfo+=szTemp;
						sIniFileInfo+=">\r\n";
					}
					lSize = MAX_PATH;
					if(RegQueryValueEx(key.m_hKey,"SCRNSAVE.EXE",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){
						sIniFileInfo+="CPD-SS=<";
						sIniFileInfo+=szTemp;
						sIniFileInfo+=">\r\n";
					}
				}
				{// На дефолтного
					CRegKey key;
					key.Open(HKEY_USERS, ".DEFAULT\\Control Panel\\Desktop");
					if(key!=NULL){
						char szTemp[MAX_PATH]="";
						DWORD lSize,dwType=0;
						lSize = MAX_PATH;
						if(RegQueryValueEx(key.m_hKey,"Wallpaper",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){
							sIniFileInfo+="DCPD-W=<";
							sIniFileInfo+=szTemp;
							sIniFileInfo+=">\r\n";
						}
						lSize = MAX_PATH;
						if(RegQueryValueEx(key.m_hKey,"WallpaperStyle",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){
							sIniFileInfo+="DCPD-WS=<";
							sIniFileInfo+=szTemp;
							sIniFileInfo+=">\r\n";
						}
						lSize = MAX_PATH;
						if(RegQueryValueEx(key.m_hKey,"TileWallpaper",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){
							sIniFileInfo+="DCPD-WT=<";
							sIniFileInfo+=szTemp;
							sIniFileInfo+=">\r\n";
						}
					}
				}
			}
			if(sIniFileInfo.Find("UserData")==-1 || sIniFileInfo.Find("ConfigFile")==-1){
				sIniFileInfo+="ConfigFile=";
				sIniFileInfo+=objSettings.sIniFile;
				sIniFileInfo+="\r\n";
				sIniFileInfo+="UserData=";
				sIniFileInfo+=GetPathPart(objSettings.sIniFile,1,1,0,0);
				sIniFileInfo+="\r\n";
			}
			SaveFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
		}else if(sConsoleRest=="yes"){
			if(sIniFileInfo==""){
				return 0;
			}
			RestoreWP(sIniFileInfo,1);
		}
		return FALSE;
	}
	CString sWait;
	GetCommandLineParameter("wait",sWait);
	DWORD dwStartWait=GetTickCount();
	if(sWait=="yes"){
		while(CheckProgrammRunState(NULL, UNIQUE_TO_TRUSTEE, false) && GetTickCount()-dwStartWait<180000){
			Sleep(1000);
		}
	}
	// Для нормально работы клонов
	if(IsThisProgrammAlreadyRunning()){
		// || IsOtherProgrammAlreadyRunning("WireChanger")
		DWORD dwTarget=BSM_APPLICATIONS;
		BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, iWM_THIS, WPARAM(99), LPARAM(99));
		return FALSE;
	}
	// Столбим уникальное за компьютером...
	CheckProgrammRunState("WC3",UNIQUE_TO_COMPUTER,1,"");
	CString sRestore;
	GetCommandLineParameter("restore",sRestore);
	if(sRestore!="" && isFileExist(sRestore)){
		CString sIniPath=GetPathPart(objSettings.sIniFile,1,1,0,0);
		{//Первый проход
			CZipArchive zipFile;
			if(zipFile.Open(sRestore)){
				ZIP_FIND_DATA pFind;
				HANDLE hSearch=zipFile.FindFirstFile("*.*",&pFind);
				while(zipFile.FindNextFile(hSearch, &pFind)){
					CString sFileName=pFind.szFileName;
					if(sFileName!=""){
						CZipFile zI;
						if(zipFile.GetFile(pFind.nDirIndex,&zI)){
							DeleteFile(sIniPath+sFileName);
							zI.SafeSaveToDosk(sIniPath+sFileName,0);
						}
					}
				}
				zipFile.FindClose(hSearch);
				zipFile.Close();
			}
		}
	}
	AfxEnableControlContainer();
	AfxInitRichEdit();
#if WINVER<=0x0050
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
	OleInitialize(NULL);
	//
	CRYPT_START
	objSettings.sLikUser="";
	objSettings.sLikKey="";
	HANDLE hLicenseFile=::CreateFile(GetUserFolder()+LICENSE_KEY_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (!hLicenseFile || hLicenseFile==INVALID_HANDLE_VALUE){
		// Из локального каталога!
		hLicenseFile=::CreateFile(CString(GetApplicationDir())+LICENSE_KEY_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	}
	if (hLicenseFile && hLicenseFile!=INVALID_HANDLE_VALUE){
		DWORD dwRead=0;
		char szKey[2048]="";
		objSettings.sLikKey="";
		::ReadFile(hLicenseFile, szKey, sizeof(szKey), &dwRead, NULL);
		objSettings.sLikUser=CDataXMLSaver::GetInstringPart("user:[","]",szKey);
		objSettings.sLikKey=CDataXMLSaver::GetInstringPart("key:[","]",szKey);
		::CloseHandle(hLicenseFile);
	}
#ifdef LIC_HARDCODED_U
	objSettings.sLikUser=LIC_HARDCODED_U;
#endif
#ifdef LIC_HARDCODED_K
	objSettings.sLikKey=LIC_HARDCODED_K;
#endif
	//if(sIniFileInfo.Find("WireChangerEF")!=-1){
		/*
		WC5Elefun	CCC2-HJ3S-88A6-C4TP
		*/
		/*
		WC5IPv6	4HVF-9XF6-DGQ2-94U2	
		*/
		/*
		WK999IPv6	EJCM-VNR7-GZ8F
		*/
		/*Mapi2Pop3
		IPv6	GT8N-6747-AGDM
		*/
		/*
		// Здесь поддержан элефан
		USE_ELEFUN=1;
		objSettings.sLikUser="Elefun";
		objSettings.sLikKey=Recrypt("\xd3\x4c\xc3\x24\x84\x53\x3e\x2a\x2c\x21\x9e\x24\x48\x34\xb5\xb4");//"CCC2-HJ3S-88A6-C4TP";// EXECryptor_DecryptStr()?
		*/
	//}
	UpdateTrialityStatus(objSettings.sLikUser,objSettings.sLikKey);
	if(objSettings.iLikStatus<0){
		CSettings* objAntiDebug=0;
		objAntiDebug->ApplySettings();
		return FALSE;
	}
	CRYPT_END
	objSettings.Load();
	if(sConsoleMode=="help"){
		ShowHelp("Overview");
		return 0;
	}
	{// грузим ddown
		CBitmap bpTmp;
		bpTmp.LoadBitmap(IDB_DDOWN_A);
		_bmp().AddBmpRaw("DDOWN",&bpTmp,GetBitmapSize(bpTmp));
	}
	{// грузим remd
		CBitmap bpTmp;
		bpTmp.LoadBitmap(IDB_REMIND_A);
		_bmp().AddBmpRaw("REMINDER",&bpTmp,GetBitmapSize(bpTmp));
	}
	theApp.MainImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 2);
	// Основные иконки
	AddBitmapToIList(theApp.MainImageList,IDB_IMAGELIST);
	for(int i=0;i<theApp.MainImageList.GetImageCount();i++){
		HICON hIcon=theApp.MainImageList.ExtractIcon(i);
		_bmp().AddBmp(_IL(i),hIcon);
		ClearIcon(hIcon);
	}
	_bmp().AddBmpRaw(IDB_BM_LOGO,CSize(LOGO_W,LOGO_H));
	// Пытаемся к пустому окну захимичится
	rFakedRect.SetRect(-10,0,0,0);
	LPCTSTR szClass = AfxRegisterWndClass(NULL);
	m_pFakeWnd = new CWnd;
	m_pFakeWnd->CreateEx(0, szClass, ROOT_WND_NAME, 0, rFakedRect, NULL, 0);
	m_pFakeWnd->ShowWindow(SW_HIDE);
	m_pFakeWnd->EnableWindow(FALSE);
	m_pFakeWnd->SetIcon(::AfxGetApp()->LoadIcon(MAIN_ICON),TRUE);
	m_pFakeWnd->SetIcon(::AfxGetApp()->LoadIcon(MAIN_ICON),FALSE);
	HotkeysSkipDD()=1;
	// Если при запуске небыло найдено ini-файла, показываем опции...
	if(objSettings.bStartWithOptions){
		objSettings.bStartWithOptions=FALSE;
		if(IsStartupWithWindows()==FALSE){
			StartupApplicationWithWindows(TRUE);
		}
		// точней больше не показываем
		// objSettings.OpenOptionsDialog();
	}
	
	// Создаем
	pMainDialogWindow = new AppMainDlg();
	pMainDialogWindowCrypt = pMainDialogWindow;
	if(!pMainDialogWindow){
		return FALSE;
	}
	theApp.m_pMainWnd=pMainDialogWindow;// Сначала задаем главное окно, потом создаем его
	pMainDialogWindow->Create(AppMainDlg::IDD,m_pFakeWnd);
	// Все!
	return TRUE;
}

int AppMainApp::ExitInstance() 
{
	if(m_pMainWnd){
		m_pMainWnd->DestroyWindow();
		delete m_pMainWnd;
	}
	if(m_pFakeWnd){
		m_pFakeWnd->DestroyWindow();
		delete m_pFakeWnd;
	}
	CoUninitialize();
	return CWinApp::ExitInstance();
}

CRect AppMainApp::GetDesktopRECT()
{
	CRect rDesktopRECT;
	if(GetNumberOfMons()>1){
		rDesktopRECT=GetAllMonitorsRECT();
	}else{
		::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
	}
		
	return rDesktopRECT;
}

