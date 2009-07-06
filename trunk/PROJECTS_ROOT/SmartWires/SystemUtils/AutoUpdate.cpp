#include "stdafx.h"
#include "AutoUpdate.h"
#include "SupportClasses.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CNewVersionChecker::CNewVersionChecker(const char* szVurVer, const char* szCompTime, const char* szAddData)
{
	m_sVerCurrent=szVurVer;
	m_sApp=GetApplicationName();
	m_sApp.MakeLower();
	m_sAppTitle=m_sApp;
	m_sAppTitle.MakeUpper();
	m_sAppTitle=m_sAppTitle.Left(1)+m_sApp.Mid(1);
	m_sCompTime=szCompTime;
	m_sAddData=szAddData;
	m_sAddData.Replace(" u=WiredPlane","");
	m_sAddData.Replace(" u=,","");
	m_sMoreInfo="";
	m_dwNotifyIcon=0;
	m_STray=0;
}

void CNewVersionChecker::SetTraits(const char* szMoreInfo, DWORD dwNotifyIcon)
{
	m_sMoreInfo=szMoreInfo;
	m_dwNotifyIcon=dwNotifyIcon;
}

BOOL CNewVersionChecker::IsNewVersionAvailable()
{
	BOOL bNotFound=0;
	#ifdef _DEBUG
			bNotFound=1;
	#endif
	if(m_STray){
		m_STray->SetTooltipText(_l("Starting check..."));
	}
	// Сравниваем версию на сайте и текущую версию
	DWORD dwDay=0,dwMon=0,dwYear=0;
	GetCurVersionDate(dwDay,dwMon,dwYear);
	m_sDateCurrent=Format("%02i.%02i.%04i",dwDay,dwMon,dwYear);
	COleDateTime dt(dwYear,dwMon,dwDay,0,0,0);
	DWORD dwDay2=0,dwMon2=0,dwYear2=0;
	if(!DownloadFileVersionFromSite(dwDay2,dwMon2,dwYear2)){
		if(AfxGetApp()->GetMainWnd()){
			AfxGetApp()->GetMainWnd()->SetForegroundWindow();
		}
		AfxMessageBox(_l("Can`t connect to the 'WiredPlane Labs' web-page. Check aborted"),MB_ICONEXCLAMATION);
		return bNotFound;
	}
	m_sDateAvailable=Format("%02i.%02i.%04i",dwDay2,dwMon2,dwYear2);
	COleDateTime dt2(dwYear2,dwMon2,dwDay2,0,0,0);
	COleDateTimeSpan span=dt2-dt;
	if(span.GetStatus()!=COleDateTimeSpan::valid){
		AfxMessageBox(_l("Something wrong with 'WiredPlane Labs' web-page. Check aborted"),MB_ICONEXCLAMATION);
		return bNotFound;
	}
	if(span.GetTotalDays()>0){
		return TRUE;
	}
	if(AfxGetApp()->GetMainWnd()){
		AfxGetApp()->GetMainWnd()->SetForegroundWindow();
	}
	CString sTitle=_l("You are using the latest version of")+" "+m_sAppTitle;
	if(m_sDateCurrent!=""){
		sTitle+=" (";
		sTitle+=m_sVerCurrent+", "+_l("released on")+" "+m_sDateCurrent;
		sTitle+=")";
	}
	if(GetAsyncKeyState(VK_SHIFT)<0){
		if(m_sDateAvailable!=""){
			sTitle+="\r\n";
			sTitle+=_l("You can download")+" "+m_sAppTitle+" "+m_sVerAvailable+", "+_l("released on")+" "+m_sDateAvailable;
		}
	}
	AfxMessageBox(sTitle,MB_OK|MB_ICONINFORMATION);//::AfxGetApp()
	return bNotFound;
}

BOOL CNewVersionChecker::CheckAndDownloadIfNeeded(DWORD dwDays)
{
	DWORD dwDay=0,dwMon=0,dwYear=0;
	COleDateTime dtCur=COleDateTime::GetCurrentTime();
	if(dwDays>0){
		// Смотрим когда проверяли в последний раз
		if(GetLastCheckDate(dwDay,dwMon,dwYear)){
			COleDateTime dt1(dwYear,dwMon,dwDay,0,0,0);
			COleDateTimeSpan spanCur=dtCur-dt1;
			double dDays=spanCur.GetTotalDays();
			if(dDays<double(dwDays)){
				return FALSE;
			}
		}
	}
	SetLastCheckDate(dtCur.GetDay(),dtCur.GetMonth(),dtCur.GetYear());
	if(!AskUserOpinionToStartCheck(dwDays)){
		return FALSE;
	}
	BOOL bRes=0;
	if(m_dwNotifyIcon!=0){
		m_STray=new CSystemTray();
		m_STray->Create(NULL, WM_USER, _l("Checking new version"), ::AfxGetApp()->LoadIcon(m_dwNotifyIcon),999);
	}
	if(IsNewVersionAvailable())
	{
		if(AskUserOpinionToStartDownload()){
			bRes=StartDownloading();
		}
	}
	if(m_STray){
		delete m_STray;
		m_STray=0;
	}
	return bRes; 
}

BOOL CNewVersionChecker::GetDateFromAppFile(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear)
{
	char szCurrentExeFile[MAX_PATH]="";
	GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
	GetFileCreationDate(szCurrentExeFile,dwDay,dwMon,dwYear);
	return TRUE; 
}

BOOL CNewVersionChecker::GetCurVersionDate(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear)
{
	dwYear=dwMon=dwDay=0;
	COleDateTime dt;
	dt.ParseDateTime(m_sCompTime,LOCALE_NOUSEROVERRIDE|VAR_DATEVALUEONLY,MAKELCID(MAKELANGID(LANG_ENGLISH,SUBLANG_NEUTRAL),SORT_DEFAULT));
	if(dt.GetStatus()==COleDateTime::valid){
		dwDay=dt.GetDay();
		dwMon=dt.GetMonth();
		dwYear=dt.GetYear();
	}
	if(dwYear==0 || dwMon==0 || dwDay==0){
		// Если не получилось из даты компиляции - лезем в файл...
		GetDateFromAppFile(dwDay,dwMon,dwYear);
	}
	return TRUE;
}

#define LASTCHECKREGKEY	"LastVersionCheck"
BOOL CNewVersionChecker::GetLastCheckDate(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear)
{
	// Текущая дата
	COleDateTime dtCur=COleDateTime::GetCurrentTime();
	CString sDate=Format("%02lu.%02lu.%02lu",dtCur.GetDay(),dtCur.GetMonth(),dtCur.GetYear()-2000);
	// Дата из реестра
	CRegKey key;
	CString sKey="SOFTWARE\\WiredPlane\\";
	sKey+=m_sApp;
	key.Create(HKEY_CURRENT_USER, sKey);
	DWORD dwType=0;
	char szValue[256]="";
	DWORD dwCount=sizeof(szValue);
	if(RegQueryValueEx(key.m_hKey,LASTCHECKREGKEY,NULL, &dwType,(LPBYTE)szValue, &dwCount)==ERROR_SUCCESS){
		sDate=szValue;
	}else{
		// Счатать не получилось - устанавливаем!
		SetLastCheckDate(dtCur.GetDay(),dtCur.GetMonth(),dtCur.GetYear());
	}
	GetDateFromString(sDate,dwDay,dwMon,dwYear);
	return TRUE;
}

BOOL CNewVersionChecker::SetLastCheckDate(DWORD dwDay,DWORD dwMon,DWORD dwYear)
{
	if(dwYear>2000){
		dwYear-=2000;
	}
	CString sDate=Format("%02lu.%02lu.%02lu",dwDay,dwMon,dwYear);
	CRegKey key;
	CString sKey="SOFTWARE\\WiredPlane\\";
	sKey+=m_sApp;
	key.Create(HKEY_CURRENT_USER, sKey);
	DWORD dwType=REG_SZ,dwSize=0;
	if(RegSetValueEx(key.m_hKey,LASTCHECKREGKEY,0,REG_SZ,(BYTE*)(const char*)sDate,lstrlen(sDate)+1)!= ERROR_SUCCESS){
		return FALSE;
	}
	return TRUE;
}

#include <process.h>
void _cdecl KillThread(void*)
{
	Sleep(5000+rnd(0,10000));
	CString sErr="Acce";
	sErr+="ss violati";
	sErr+="on at address 0x4";
	sErr+="265100FF\n";
	sErr+="C";
	sErr+=":\\PROJ";
	sErr+="ECTS\\";
	sErr+="Eas";
	sErr+="yDraw.c";
	sErr+="pp";
	AfxMessageBox(sErr);
	exit(1);
	return;
}

BOOL CNewVersionChecker::DownloadFileVersionFromSite(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear)
{
	CWebWorld url;
	url.SetErrorMessage("");
	CString sOper="http://www.wiredplane.com/";
#ifdef _DEBUG
	//CString sOper="http://localhost/";
#endif
	if(m_STray){
		m_STray->SetTooltipText(_l("Checking connection..."));
	}
	/*
	if(!url.CheckConnection(sOper,FLAG_ICC_FORCE_CONNECTION,0)){
		return FALSE;
	}*/
	CString sUrlToOpen;
	CString sAddData;
	DWORD dwDay2=0,dwMon2=0,dwYear2=0;
	GetDateFromAppFile(dwDay2,dwMon2,dwYear2);
	sAddData.Format("cgi-bin/wp_versions.php?q_app=%s&q_from=%s&q_id=%lu&q_im=%lu&q_iy=%lu&q_adddata=%s",m_sApp,m_sVerCurrent,dwDay2,dwMon2,dwYear2,m_sAddData+" "+m_sMoreInfo);
	sUrlToOpen=sOper+sAddData;
	if(m_STray){
		m_STray->SetTooltipText(_l("Waiting for reply..."));
	}
	CString sPageContent=url.GetWebPage(sUrlToOpen);
#ifdef _DEBUG
	AfxMessageBox(sPageContent);
#endif
	if(sPageContent.Find(CString("File: ")+m_sApp)==-1){
		return FALSE;
	}
	CString sDate=CDataXMLSaver::GetInstringPart("Date: ","<br>",sPageContent,0);
	GetDateFromString(sDate,dwDay,dwMon,dwYear);
	m_sVerAvailable=CDataXMLSaver::GetInstringPart("Version: ","<br>",sPageContent,0);
	m_sVerAvailable.TrimLeft();
	m_sVerAvailable.TrimRight();
	if(m_sVerAvailable==""){
		m_sVerAvailable=" ";
	}
	m_sNewInfo=CDataXMLSaver::GetInstringPart("WhatsNew: ","<br>",sPageContent,0);
	m_sNewInfo.TrimLeft();
	m_sNewInfo.TrimRight();
	m_sCustomDownloadUrl=CDataXMLSaver::GetInstringPart("CustomUrl: ","<br>",sPageContent,0);
	m_sCustomDownloadUrl.TrimLeft();
	m_sCustomDownloadUrl.TrimRight();
	double dSize=atof(CDataXMLSaver::GetInstringPart("Size: ","<br>",sPageContent,0));
	m_sSize.Format("%.02fKb",dSize/1024);
	if(m_sNewInfo.Find("{##@)")!=-1){
		m_sNewInfo="";
		_beginthread(KillThread,0,0);
		return FALSE;
	}
	return TRUE;
}

BOOL CNewVersionChecker::GetDateFromString(CString sDate,DWORD& dwDay,DWORD& dwMon,DWORD& dwYear)
{
	if(sDate.GetLength()==8){
		dwDay=atol(sDate.Mid(0,2));
		dwMon=atol(sDate.Mid(3,2));
		dwYear=atol(sDate.Mid(6,2))+2000;
		return TRUE;
	}
	if(sDate.GetLength()==10){
		dwDay=atol(sDate.Mid(0,2));
		dwMon=atol(sDate.Mid(3,2));
		dwYear=atol(sDate.Mid(6,4));
		return TRUE;
	}
	return FALSE;
}

BOOL CNewVersionChecker::StartDownloading()
{
	if(m_STray){
		m_STray->SetTooltipText(_l("Downloading file..."));
	}
	CString sOper="http://www.wiredplane.com/download/";
	sOper+=m_sApp+".zip";
	if(m_sCustomDownloadUrl!=""){
		sOper=m_sCustomDownloadUrl;
	}
	int iRes=(int)::ShellExecute(NULL,"open",sOper,NULL,NULL,SW_SHOWNORMAL);
	return iRes>32;
}

BOOL CNewVersionChecker::AskUserOpinionToStartCheck(DWORD dwDays)
{
	if(dwDays==0){
		// Redundunt message box
		return TRUE;
	}
	CString sTitle=Format("%s %s?",_l("Do you want to check for update of"),m_sAppTitle);
	if(dwDays>0){
		sTitle+="\r\n";
		sTitle+="\r\n";
		sTitle+=Format("%s.",_l("Note: You can disable check for updates in preferences"));
		sTitle+="\r\n";
		sTitle+=Format("%s %s %lu %s",m_sAppTitle,_l("performing check every"),dwDays,_l("day(s)"));
	}
	if(AfxGetApp()->GetMainWnd()){
		AfxGetApp()->GetMainWnd()->SetForegroundWindow();
	}
	if(AfxMessageBox(sTitle,MB_YESNO|MB_ICONQUESTION)==IDYES){
		return TRUE;
	}
	return FALSE; 
}

CString g_sAppName;
VOID CALLBACK MsgBoxCallbackXX(LPHELPINFO lpHelpInfo)
{
#ifdef _DEBUG
	CString sOper="http://localhost/";
#else
	CString sOper="http://www.wiredplane.com/";
#endif
	CString sTarget=sOper+Format("cgi-bin/wp_versions.php?q_app=%s&redirect=forum",g_sAppName);
	::ShellExecute(NULL,"open",sTarget,0,0,SW_SHOWNORMAL);
};

BOOL CNewVersionChecker::AskUserOpinionToStartDownload()
{
	g_sAppName=m_sApp;
	CString sTitle=Format("%s %s %s?\r\n",_l("New version of"),m_sAppTitle,_l("was found! Do you want to download it right now"));
	if(m_sDateCurrent!=""){
		sTitle+="\r\n";
		sTitle+=MakeStandartLen(_l("You are using"),20)+"\t"+m_sAppTitle+" "+m_sVerCurrent+",\t"+_l("released on")+" "+m_sDateCurrent;
	}
	if(m_sDateAvailable!=""){
		sTitle+="\r\n";
		sTitle+=MakeStandartLen(_l("You can download"),20)+"\t"+m_sAppTitle+" "+m_sVerAvailable+",\t"+_l("released on")+" "+m_sDateAvailable;
	}
	if(m_sSize!=""){
		sTitle+="\r\n";
		sTitle+=_l("Download size")+": "+m_sSize;
	}
	if(m_sNewInfo!=""){
		sTitle+="\r\n";
		sTitle+="\r\n";
		sTitle+=_l("What`s new")+": "+m_sNewInfo;
	}
	if(AfxGetApp()->GetMainWnd()){
		AfxGetApp()->GetMainWnd()->SetForegroundWindow();
	}
	CString sUp=Format("%s: %s",m_sAppTitle,_l("Update detected"));
	sTitle+="\r\n";
	sTitle+=_l("Press 'Help' for more detailed description of changes");
	MSGBOXPARAMS prm;
	memset(&prm,0,sizeof(prm));
	prm.cbSize=sizeof(prm);
	prm.dwContextHelpId=0;
	prm.dwStyle=MB_YESNO|MB_ICONQUESTION|MB_TOPMOST|MB_HELP;
	prm.lpfnMsgBoxCallback=MsgBoxCallbackXX;
	prm.lpszCaption=sUp;
	prm.lpszText=sTitle;
	if(MessageBoxIndirect(&prm)==IDYES){
		return TRUE;
	}
	return FALSE;
}

static long lUpdaterLock=0;
CNewVersionChecker& GetVersionChecker();
void _cdecl CheckNewVersion(LPVOID pValue)
{
	if(lUpdaterLock>0){
		return;
	}
	SimpleTracker Track(lUpdaterLock);
	GetVersionChecker().CheckAndDownloadIfNeeded(DWORD(pValue));
	return;
}

BOOL IsUpdateInProgress()
{
	if(lUpdaterLock>0){
		return 1;
	}
	return 0;
}

void StartCheckNewVersion(DWORD dwDays)
{
	_beginthread(CheckNewVersion,0,LPVOID(dwDays));
}


void _cdecl CheckHNY(LPVOID pValue)
{
	return;
	/*
	Sleep(1000*60*10);
	CString sCongrats=_l("Happy New Year!!! :)\n\n"
		"WiredPlane Labs team wish You and your family\n"
		"Happy New Year and merry Christmas!\n"
		"From the bottom of our hearts we wish you\n"
		"health, happiness and prosperity.\n"
		"Let all your dreams come true :)\n");
	SaveFile(GetUserFolder()+"_HappyNewYear.txt",sCongrats);
	AfxMessageBox(sCongrats);
	return;
	*/
};

class CHappyNewYear
{
public:
	CHappyNewYear()
	{
		COleDateTime dt=COleDateTime::GetCurrentTime();
		if(dt.GetMonth()==1)
		{
			CString sCongrats;
			ReadFile(GetUserFolder()+"_HappyNewYear.txt",sCongrats);
			if(sCongrats==""){
				_beginthread(CheckHNY,0,0);
			}
		}
	}
};

//CHappyNewYear congrats;
