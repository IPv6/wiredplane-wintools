// WKKeyLogger.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "WP_KeyLogger.h"
#include "WP_KeyLoggerHook\HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

char szRetranslationBuffer[1024]="";
char* _pl(const char* szText)
{
	WKGetPluginContainer()->GetTranslation(szText,szRetranslationBuffer,sizeof(szRetranslationBuffer));
	return szRetranslationBuffer;
}

CWKATInfo ATData;
extern HINSTANCE g_hinstDll;
long g_OptionsInProgress=0;
DWORD dwHookerThread=0;
HANDLE hHookerEventDone=NULL;
HANDLE hHookerEventStop=0;
DWORD WINAPI GlobalHooker_Autotext(LPVOID pData);
int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK OptionsDialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define SAVELOGFONT_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\KeyLogger"

char szAutoTextFile[MAX_PATH]="";
extern char szATBuffer[MAX_KEY_LEN];
char szHookLibPath[MAX_PATH]="";
extern CRITICAL_SECTION csMainThread;
HINSTANCE& GetHookInstance()
{
	static HINSTANCE hHookDLL = NULL;
	if(hHookDLL==NULL){
		hHookDLL = LoadLibrary(szHookLibPath);
	}
	return hHookDLL;
}

void ReplaceChar(char* szSrc, char cFrom, char cTo)
{
	int iPos=0;
	while(szSrc[iPos]!=0){
		if(szSrc[iPos]==cFrom){
			szSrc[iPos]=cTo;
		}
		iPos++;
	}
}

void ReplaceChar(char* szSrc, char cFrom, const char* szTo)
{
	int iPos=0;
	long iAmount=0;
	if(szTo){
		iAmount=strlen(szTo);
	}
	while(szSrc[iPos]!=0){
		if(szSrc[iPos]==cFrom){
			// Overlap-aware fuction
			if(iAmount>0){
				long lLen=strlen(szSrc+iPos);
				memmove(szSrc+iPos+iAmount,szSrc+iPos+1,lLen);
				memcpy(szSrc+iPos,szTo,iAmount);
				iPos+=iAmount;
			}else{
				// Overlap-aware fuction
				long lLen=strlen(szSrc+(iPos+1));
				memmove(szSrc+iPos,szSrc+(iPos+1),lLen);
				szSrc[iPos+lLen]=0;
			}
		}
		iPos++;
	}
}

void LoadAutotext()
{// Читаем ключи из файла
	WKGetPluginContainer()->GetPreferencesFolder(szAutoTextFile,sizeof(szAutoTextFile));
	strcat(szAutoTextFile,"autotext.conf");
	// Читаем 
	for(int i=0;i<MAX_AT;i++){
		ATData.szATKeys[i]=0;
		ATData.szATValues[i]=0;
	}
	HANDLE hFile = ::CreateFile(szAutoTextFile, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if (INVALID_HANDLE_VALUE != hFile){
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);
		DWORD dwRead=0;
		if(dwFileSize>0){
			char* szBuffer=new char[dwFileSize+1];
			ZeroMemory(szBuffer,dwFileSize+1);
			::ReadFile(hFile, szBuffer, dwFileSize, &dwRead, NULL);
			szBuffer[dwRead]=0;
			// Парсим ключи
			int iATKeyNum=0;
			char* szIntroduction=strtok(szBuffer,"\n");
			while(1 && iATKeyNum<MAX_AT){
				char* szKey=strtok(NULL,"\t\n");
				if(szKey==NULL || strlen(szKey)==0){
					break;
				}
				ATData.szATKeys[iATKeyNum]=strdup(szKey);
				char* szValue=strtok(NULL,"\n");
				if(szValue==NULL || strlen(szValue)==0){
					break;
				}
				ATData.szATValues[iATKeyNum]=strdup(szValue);
				ReplaceChar(ATData.szATValues[iATKeyNum],ENTER_CHAR,'\n');
				iATKeyNum++;
			}
			delete[] szBuffer;
		}
		CloseHandle(hFile);
	}
}

BOOL bInitDone=0;
HANDLE hHookerThread=0;
CWKKeyLoggerInfo hookData;
int WINAPI HandleSystemEvent_CopyCli(CWKSYSEvent* pEvent);
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	// Version...
	char szVer[32]="";
	init->GetWKVersion(szVer);
	DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
	if(dwBuild<348){
		init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
		return 0;
	}
	WKGetPluginContainer()->GetWKPluginsDirectory(szHookLibPath,MAX_PATH);
	strcat(szHookLibPath,"wkatexth.dll");
	LoadAutotext();
	hookData.iSaveScrLast=GetTickCount();
	hookData.bmfocus=1;
	hookData.bATChars=(ATData.szATKeys[0]!=0);
	hHookerEventDone=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerEventStop=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_Autotext, NULL, 0, &dwHookerThread);
	WaitForSingleObject(hHookerEventDone,INFINITE);
	PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	WKGetPluginContainer()->SubmitToSystemEventNotification(WKSYSEVENTTYPE_COPYTOCLIPB, (FPONSYSTEMEVENT)HandleSystemEvent_CopyCli);
#ifdef _DEBUG
	char s[100]={0};
	sprintf(s,"%s:%i %08x Debug thread point",__FILE__,__LINE__,dwHookerThread);
	init->ShowPopup(s,"Debug thread",0);
#endif
	bInitDone=1;
	return 1;
}

int	WINAPI WKPluginStart()
{
	::InitializeCriticalSection(&csMainThread);
	hookData.bATChars=FALSE;
	ATData.bATWasChanged=FALSE;
	memset(&hookData,0,sizeof(hookData));
	memset(&szATBuffer,0,sizeof(szATBuffer));
	hookData.bWatchUps=0;
	hookData.bLogChars=0;
	hookData.dwMaxSize=75;
	hookData.dwMinSize=50;
	hookData.bEnableSound=0;
	strcpy(hookData.szEnableSound,"");
	strcpy(hookData.szTargetFile,"");
	strcpy(hookData.szValidWndTitles,"");
	strcpy(hookData.szValidWndClasses,"");
	return 1;
}

int	WINAPI WKDesktopSwitching(BOOL bPaused)
{
	if(bPaused){
		PostThreadMessage(dwHookerThread, WM_ENABLE, 0, 0);
	}else{
		PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	}
	::WaitForSingleObject(hHookerEventDone,500);
	return 1;
}

int	WINAPI _WKPluginPause(BOOL bPaused)
{
	if(bPaused){
		PostThreadMessage(dwHookerThread, WM_ENABLE, 0, 0);
	}else{
		PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	}
	::WaitForSingleObject(hHookerEventDone,500);
	return 1;
}

extern long lHookActive;
#define INTRODUC_TEXT	"#WireKeys autotext module configuration file\n"
void SaveAutotextStrings()
{
	if(ATData.bATWasChanged){// Сохраняем ключи в файл
		HANDLE hFile = ::CreateFile(szAutoTextFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (INVALID_HANDLE_VALUE != hFile){
			DWORD dwWritten=0;
			::WriteFile(hFile,INTRODUC_TEXT,strlen(INTRODUC_TEXT),&dwWritten,NULL);
			for(int i=0;i<MAX_AT;i++){
				if(ATData.szATKeys[i] && ATData.szATValues[i] && strlen(ATData.szATKeys[i])>0 && strlen(ATData.szATValues[i])>0){
					::WriteFile(hFile,ATData.szATKeys[i],strlen(ATData.szATKeys[i]),&dwWritten,NULL);
					::WriteFile(hFile,"\t",1,&dwWritten,NULL);
					DWORD dwSize=strlen(ATData.szATValues[i]);
					char* szBuffer=new char[dwSize+1];
					ZeroMemory(szBuffer,dwSize+1);
					strcpy(szBuffer,ATData.szATValues[i]);
					ReplaceChar(szBuffer,'\n',ENTER_CHAR);
					::WriteFile(hFile,szBuffer,dwSize,&dwWritten,NULL);
					::WriteFile(hFile,"\n",1,&dwWritten,NULL);
				}else{
					break;
				}
			}
			CloseHandle(hFile);
		}
	}
	ATData.bATWasChanged=FALSE;
}

int	WINAPI WKPluginStop()
{
	if(!WKGetPluginContainer()->getOption(WKOPT_ISSHUTDOWN)){
		PostThreadMessage(dwHookerThread, WM_CLOSE, 0, 0);
		WaitForSingleObject(hHookerEventStop,2000);
		//::EnterCriticalSection(&csMainThread);
		//::LeaveCriticalSection(&csMainThread);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		//TerminateThread(hHookerThread,66);
		//SuspendThread(hHookerThread);
	}
	::CloseHandle(hHookerEventDone);
	::CloseHandle(hHookerEventStop);
	::CloseHandle(hHookerThread);
	SaveAutotextStrings();
	for(int i=0;i<MAX_AT;i++){
		if(ATData.szATKeys[i]){
			delete[] ATData.szATKeys[i];
			ATData.szATKeys[i]=0;
		}
		if(ATData.szATValues[i]){
			delete[] ATData.szATValues[i];
			ATData.szATValues[i]=0;
		}
	}
	::DeleteCriticalSection(&csMainThread);
	//FreeLibrary(GetHookInstance());
	GetHookInstance()=NULL;
	return 1;
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Define Autotext");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON2));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
	}else if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Just typed text");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON3));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
	}else{
		strcpy(stuff->szItemName,"Change layout of the last word");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON3));
		stuff->dwItemMenuPosition=0;
		stuff->dwDefaultHotkey=67108873;
	}
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>2 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI WKPluginOptions2()
{
	RETURN_IFRECURSE(0);
	while(bInitDone!=1){
		Sleep(500);
	}
	SimpleTracker lc(g_OptionsInProgress);
	PostThreadMessage(dwHookerThread, WM_ENABLE, 0, 0);
	WaitForSingleObject(hHookerEventDone,500);
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPTIONS2),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc2);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	hookData.bATChars=ATData.szATKeys[0]!=0;
	PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	WaitForSingleObject(hHookerEventDone,500);
	ATData.bATWasChanged=TRUE;
	SaveAutotextStrings();
	return 1;
}

extern int g_iDefactionType;
extern char g_sDefactionText[256];
extern int g_iAlertUser;
int WINAPI AddTypingAlias(char const* szEventID,int iType)
{
	g_iAlertUser=1;
	strcpy(g_sDefactionText,szEventID);
	g_iDefactionType=iType;
	WKPluginOptions2();
	return 1;
}

extern char szJustTypeTextBuffer[];
int CALLBACK JTTDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		char szBuffer[JTT_BUFFER*2+1]={0};
		memcpy(szBuffer,szJustTypeTextBuffer,sizeof(szBuffer));
		strrev(szBuffer);
		ReplaceChar(szBuffer,'\r',' ');
		ReplaceChar(szBuffer,'\n',"\r\n");
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),szBuffer);
		::SendMessage(GetDlgItem(hwndDlg,IDC_EDIT),EM_LINESCROLL,0,0xFF);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC1),_pl("This is the most recently typed text"));
		::SetWindowText(GetDlgItem(hwndDlg,ID_CLEAR),_pl("Clear"));
		::SetWindowText(hwndDlg,_pl("Just typed text"));
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==ID_CLEAR){
		memset(szJustTypeTextBuffer,0,JTT_BUFFER);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),"");
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	RETURN_IFRECURSE(0);
	SimpleTracker lc(g_OptionsInProgress);
	PostThreadMessage(dwHookerThread, WM_ENABLE, 0, 0);
	WaitForSingleObject(hHookerEventDone,500);
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPTIONS),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	PostThreadMessage(dwHookerThread, WM_ENABLE, 1, 0);
	WaitForSingleObject(hHookerEventDone,500);
	if(WKGetPluginContainer()){
		WKGetPluginContainer()->PushMyOptions(0);
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(iPluginFunction==0){
		WKPluginOptions2();
	}
	if(iPluginFunction==1){
		//Just typed text!!!
		RETURN_IFRECURSE(0);
		DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_JTT),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),JTTDialogProc);
		if(!WKUtils::isWKUpAndRunning()){
			return 0;
		}
	}
	if(iPluginFunction==2){
		WKGetPluginContainer()->WaitWhileAllKeysAreReleased();
		PostThreadMessage(dwHookerThread, WM_USER+2, 0, 0);
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Simple Autotext (replace as you type)");
	strcpy(dsc->szDesk,"This plugin allows you to define autotext snippets. It also can be used to log all keypresses from defined applications");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddBoolOption("ignoreCase","Treat all aliases as case insensitive","",TRUE,0);
		//pOptionsCallback->AddBoolOption("ByPaste","Use alternative method of text insertion","",1,0);
		char szCases[1024]={0};
		strcat(szCases,_pl("Direct"));
		strcat(szCases,"\t");
		strcat(szCases,_pl("Paste"));
		strcat(szCases,"\t");
		strcat(szCases,_pl("SendKeys"));
		pOptionsCallback->AddListOption("ByPaste","Method of text insertion","",szCases,1);
		pOptionsCallback->AddStringOption("atWindows","Use autotext in safe mode with this windows","If you have problems with autotext in some windows, type their titles here. Use '*' as a wildcard for partial matches","",0,0);
		pOptionsCallback->AddBoolOption("onSpace","Expand alias followed by 'Space' key only, Suggest partial matches","With this option enabled, aliases will be expanded only when followed by 'Space' key. If partial match with other aliases will be found, popup menu with corresponding suggestions will be opened for you. Without this option, aliases will be automatically expanded as you type them",FALSE,0);
		pOptionsCallback->AddBoolOption("menuFocus","Suggest partial matches: Move keyboard focus into menu","",TRUE,0);
		pOptionsCallback->AddActionOption("SetAT","Edit autotext pieces",0,0);
		pOptionsCallback->AddBoolOption("logOn","Enable logging keypresses into log file","",FALSE,0);
		pOptionsCallback->AddStringOptionEx("logFile","Log file name","","c:\\WireKeys_KeyLogger.txt",1,0,"logOn");
		pOptionsCallback->AddBoolOptionEx("logFileDt","Automatically add date to file name","",FALSE,0,"logOn");
		pOptionsCallback->AddNumberOptionEx("logMin","Recommended log file size (Kb)","",50,10,99999,0,"logOn");
		pOptionsCallback->AddNumberOptionEx("logMax","Max log file size (Kb)","",75,10,99999,0,"logOn");
		pOptionsCallback->AddStringOptionEx("logWnd","Gather keypresses from windows with matching titles","Use '*' as a wildcard for partial matches and ';' as delimiter between different masks","*",0,0,"logOn");
		pOptionsCallback->AddStringOptionEx("logWndClass","Gather keypresses from windows with matching class","Use '*' as a wildcard for partial matches and ';' as delimiter between different masks","*",0,0,"logOn");
		pOptionsCallback->AddNumberOptionEx("saveScr","Save screenshots every ... min (0 to disable)","",0,0,99999,0,"logOn");
		szCases[0]=0;
		strcat(szCases,_pl("Disable"));
		strcat(szCases,"\t");
		strcat(szCases,_pl("Enable, random"));
		strcat(szCases,"\t");
		strcat(szCases,_pl("Enable, key depended"));
		strcat(szCases,"\t");
		strcat(szCases,_pl("Enable, key unique"));
		pOptionsCallback->AddListOption("typeSound","Typing sound","",szCases,0,OL_KEYBOARD);
		//pOptionsCallback->AddStringOptionEx("Wav","Folder with sound files","","Sound\\typings",2,"typeSound");
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
			WaitForSingleObject(hHookerEventDone,1000);
		}
		hookData.iSaveScr=pOptionsCallback->GetNumberOption("saveScr");
		hookData.bIgnoreCase=pOptionsCallback->GetBoolOption("ignoreCase");
		hookData.bonSpace=pOptionsCallback->GetBoolOption("onSpace");
		hookData.bmfocus=pOptionsCallback->GetBoolOption("menuFocus");
		hookData.bLogChars=pOptionsCallback->GetBoolOption("logOn");
		hookData.bEnableSound=pOptionsCallback->GetBoolOption("typeSound");
		strcpy(hookData.szEnableSound,"Sound\\typings\\");//pOptionsCallback->GetStringOption("Wav",hookData.szEnableSound,sizeof(hookData.szEnableSound));
		hookData.dwMaxSize=pOptionsCallback->GetNumberOption("logMax");
		hookData.dwMinSize=pOptionsCallback->GetNumberOption("logMin");
		hookData.bInsByWK=pOptionsCallback->GetListOption("ByPaste");
		hookData.logFileDt=pOptionsCallback->GetBoolOption("logFileDt");
		memset(hookData.szTargetFile,0,sizeof(hookData.szTargetFile));
		memset(hookData.szValidWndTitles,0,sizeof(hookData.szValidWndTitles));
		memset(hookData.szValidWndClasses,0,sizeof(hookData.szValidWndClasses));
		memset(hookData.szWndTitlesForAT,0,sizeof(hookData.szWndTitlesForAT));
		pOptionsCallback->GetStringOption("logFile",hookData.szTargetFile,sizeof(hookData.szTargetFile));
		pOptionsCallback->GetStringOption("logWnd",hookData.szValidWndTitles,sizeof(hookData.szValidWndTitles));
		pOptionsCallback->GetStringOption("logWndClass",hookData.szValidWndClasses,sizeof(hookData.szValidWndClasses));
		pOptionsCallback->GetStringOption("atWindows",hookData.szWndTitlesForAT,sizeof(hookData.szWndTitlesForAT));
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,1,0);
			WaitForSingleObject(hHookerEventDone,1000);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("saveScr",hookData.iSaveScr);
		pOptionsCallback->SetBoolOption("ignoreCase",hookData.bIgnoreCase);
		pOptionsCallback->SetBoolOption("logOn",hookData.bLogChars);
		pOptionsCallback->SetBoolOption("onSpace",hookData.bonSpace);
		pOptionsCallback->SetBoolOption("menuFocus",hookData.bmfocus);
		pOptionsCallback->SetListOption("ByPaste",hookData.bInsByWK);
		pOptionsCallback->SetNumberOption("logMax",hookData.dwMaxSize);
		pOptionsCallback->SetNumberOption("logMin",hookData.dwMinSize);
		pOptionsCallback->SetStringOption("logFile",hookData.szTargetFile);
		pOptionsCallback->SetStringOption("logWnd",hookData.szValidWndTitles);
		pOptionsCallback->SetStringOption("logWndClass",hookData.szValidWndClasses);
		pOptionsCallback->SetStringOption("atWindows",hookData.szWndTitlesForAT);
		pOptionsCallback->SetBoolOption("typeSound",hookData.bEnableSound);
		pOptionsCallback->SetBoolOption("logFileDt",hookData.logFileDt);
		//pOptionsCallback->SetStringOption("Wav",hookData.szEnableSound);
	}
	return 1;
}


int WINAPI LastTypedText(const char* szParameters_in,char* szResult_out, size_t dwResultSize_in)
{
	if(dwResultSize_in<JTT_BUFFER){
		AfxMessageBox("Autotext scripting Error: buffer too small");
	}else{
		strcpy(szResult_out,szJustTypeTextBuffer);
	}
	return 1;
}