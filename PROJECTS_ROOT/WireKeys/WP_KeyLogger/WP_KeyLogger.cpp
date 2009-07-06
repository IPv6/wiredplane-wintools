// WKKeyLogger.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_KeyLogger.h"
#include "WP_KeyLoggerHook\HookCode.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma TODO(move to CArray and add auto-learning feature to keylogger)

_TweakTriggers hTrigs=0;
bool PatternMatch(const char* s, const char* mask)
{
	const   char* cp=0;
	const   char* mp=0;
	for (; *s&&*mask!='*'; mask++,s++){
		if (*mask!=*s && *mask!='?'){
			return 0;
		}
	}
	for (;;) {
		if (!*s){
			while (*mask=='*'){
				mask++;
			}
			return !*mask;
		}
		if (*mask=='*'){ 
			if (!*++mask){
				return 1;
			}
			mp=mask;
			cp=s+1;
			continue;
		}
		if (*mask==*s||*mask=='?'){
			mask++;
			s++;
			continue;
		}
		mask=mp;
		s=cp++;
	}
	return true;
}

void SkipBlanks(const char*& szPos)
{
	while(*szPos==' ' || *szPos=='\t' || *szPos=='\r' || *szPos=='\n' || *szPos==';'){
		szPos++;
	}
}

bool PatternMatchList(const char* s, const char* mask)
{
	if(mask==NULL || *mask==NULL){
		return false;
	}
	if(s==NULL || *s==NULL){
		return false;
	}
	const char* szPos=mask;
	SkipBlanks(szPos);
	if(szPos==NULL || *szPos==NULL){
		return false;
	}
	while(szPos!=NULL && *szPos!=0){
		SkipBlanks(szPos);
		const char* szPos2=strpbrk(szPos,";");
		if(szPos2==NULL){
			szPos2=szPos+(strlen(szPos));
		}
		if(szPos2<=szPos){
			break;
		}
		char szMask[256];
		memset(szMask,0,sizeof(szMask));
		memcpy(szMask,szPos,size_t(szPos2-szPos));
		if(szMask[0]!=0){
			const char* szPattern=szMask;
			SkipBlanks(szPattern);
			if(PatternMatch(s,szPattern)){
				return true;
			}
		}
		szPos=szPos2;
		if(*szPos==';'){
			szPos++;
		}
	}
	return false;
}

typedef BOOL (WINAPI *_BlockInput)(BOOL);
BOOL BlockInputMy(BOOL bBlock)
{
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	_BlockInput fp=NULL;
	if(hUser32){
		fp=(_BlockInput)GetProcAddress(hUser32,"BlockInput");
	}
	if(hUser32 && fp){
		return (*fp)(bBlock);
	}
	return FALSE;
}

HINSTANCE g_hinstDll;
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		g_hinstDll = hinstDll;
		break;
	case DLL_PROCESS_DETACH:
		// The calling process is detaching the DLL from its address space.
		break;
	case DLL_THREAD_ATTACH:
		// A new thread is being created in the current process.
		break;
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly.
		break;
	}
	return(TRUE);
}

#define SAVELOGFONT_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\KeyLogger"
extern char szHookLibPath[MAX_PATH];
extern CWKKeyLoggerInfo hookData;
extern HANDLE hHookerEventDone;
extern HANDLE hHookerEventStop;
extern CWKATInfo ATData;
#define BUFER_LIMIT	100
DWORD dwBufferPosition=0;
char szWindowTitle[256]="";
char szWindowClass[256]="";
char szLogCharBuffer[BUFER_LIMIT+3]="";
char szATBuffer[MAX_KEY_LEN]="";
extern long g_OptionsInProgress;
HANDLE hKeyUpEvent=NULL;
HWND g_hActiveWindow=NULL;
#define WM_UNICHAR	0x0109

const char* itemType(const char* szValue, int* iType=0)
{
	if(!szValue){
		return 0;
	}
	if(iType){
		*iType=0;
	}
	if(szValue[0]==LAUNCH_CHAR){
		if(iType){
			*iType=1;
		}
		return szValue+1;
	}
	return szValue;
}

void GetWindowTopParent(HWND& hWnd)
{
	if(!hWnd){
		return;
	}
	//hWnd=::GetAncestor(hWnd,GA_ROOT);
	HWND hWnd2=hWnd;
	while(hWnd2!=NULL){
		hWnd=hWnd2;
		hWnd2=::GetWindow(hWnd,GW_OWNER);
		if(!hWnd2){
			hWnd2=::GetParent(hWnd);
		}
	}
	return;
}

WKCallbackInterface*& WKGetPluginContainer();
char szJustTypeTextBuffer[JTT_BUFFER]={0};
WKCallbackInterface*& WKGetPluginContainer();
DWORD WINAPI ProcessATBuffer(LPVOID pDat)
{
	static long lLock=0;
	if(lLock>0){
		return 0;
	}
	int iKeyNum=0;
	size_t iKeyLen=0;
	long lRawD=(long)pDat;
	if(lRawD!=-1){
		iKeyNum=LOWORD(pDat);
		iKeyLen=HIWORD(pDat);
	}
	SimpleTracker lock(lLock);
	HWND hTargetWin=g_hActiveWindow;
	char szWindowTitle[128]={0};
	char szWindowClass[128]={0};
	HWND hTopParent=hTargetWin;
	GetWindowTopParent(hTopParent);
	::GetWindowText(hTopParent,szWindowTitle,sizeof(szWindowTitle));
	::GetClassName(hTargetWin,szWindowClass,sizeof(szWindowClass));
	if(lRawD!=-1){
		if(iKeyLen==0){
			iKeyLen=strlen(ATData.szATKeys[iKeyNum]);
		}
		if(hookData.bonSpace){
			iKeyLen+=1;
		}
	}else{
		BlockInputMy(TRUE);
		::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),0,0);
		::keybd_event(VK_SHIFT, ::MapVirtualKey(VK_SHIFT,0),KEYEVENTF_EXTENDEDKEY|0,0);//KEYEVENTF_EXTENDEDKEY|0
		::keybd_event(VK_LEFT, ::MapVirtualKey(VK_LEFT,0),KEYEVENTF_EXTENDEDKEY|0,0);
		::keybd_event(VK_LEFT, ::MapVirtualKey(VK_LEFT,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
		::keybd_event(VK_SHIFT, ::MapVirtualKey(VK_SHIFT,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);//KEYEVENTF_EXTENDEDKEY|
		::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
		BlockInputMy(FALSE);
		WKGetPluginContainer()->CallWKAction(CONVERT_SELECTED);
		return 0;
	}
	if(hTrigs){
		hTrigs(0,1);
	}
	BlockInputMy(TRUE);
	for(size_t i2=0;i2<iKeyLen;i2++){
		if(isWin9x()){
			SendMessage(hTargetWin,WM_KEYDOWN,VK_BACK,0x000E0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)
			SendMessage(hTargetWin,WM_KEYUP,VK_BACK,0xC00E0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)|(1<<30)|(1<<31)
		}else{
			ResetEvent(hKeyUpEvent);
			::keybd_event(VK_BACK, ::MapVirtualKey(VK_BACK,0),0,0);
			::keybd_event(VK_BACK, ::MapVirtualKey(VK_BACK,0),KEYEVENTF_KEYUP,0);
			WaitForSingleObject(hKeyUpEvent,1000);
		}
	}
	Sleep(300);
	if(hookData.szWndTitlesForAT[0]!=0 && (PatternMatchList(szWindowTitle,hookData.szWndTitlesForAT) || PatternMatchList(szWindowClass,hookData.szWndTitlesForAT))){
		Sleep(300);
	}
	int iType=0;
	char szTextToReplace[50000]={0};
	const char* szTextToReplaceFrom=0;
	if(iKeyNum>=0){
		szTextToReplaceFrom=itemType(ATData.szATValues[iKeyNum],&iType);
		strcpy(szTextToReplace,szTextToReplaceFrom);
		WKGetPluginContainer()->SubstPatterns(szTextToReplace, sizeof(szTextToReplace));
	}
	if(iType==0 && szTextToReplace!=0){
		if(hookData.bInsByWK==2){
			// Вставляем через SendText!
			strcat(szTextToReplace,"##SENDKEY_USE#!#");
			WKGetPluginContainer()->ExecuteScript(szTextToReplace);
		}else if(hookData.bInsByWK==1){
			WKGetPluginContainer()->PutText(szTextToReplace);
		}else{
			size_t iLen=strlen(szTextToReplace);
			for(size_t i3=0;i3<iLen;i3++){
				if(szTextToReplace[i3]=='\r'){
					continue;
				}
				PostMessage(hTargetWin,WM_CHAR,szTextToReplace[i3],0);
				//WaitForSingleObject(hKeyUpEvent,1000);
				//SendMessage(hTargetWin,WM_UNICHAR,ATData.szATValues[iKeyNum][i3],0);
				//::keybd_event(VK_BACK, ::MapVirtualKey(VK_BACK,0),0,0);
				//::keybd_event(VK_BACK, ::MapVirtualKey(VK_BACK,0),KEYEVENTF_KEYUP,0);
			}
		}
	}
	BlockInputMy(FALSE);
	if(hTrigs){
		hTrigs(0,0);
	}
	if(iType>0 && szTextToReplace!=0){
		if(strstr(szTextToReplace,"[ID=")!=0 || strstr(szTextToReplace,"javascript:")!=0){
			// Это автодобавление из настроек
			BOOL bStarted=WKGetPluginContainer()->StartQuickRunByName(szTextToReplace,NULL);
		}else{
			char const* szPos=szTextToReplace;
			while(szPos!=NULL && *szPos!=0){
				SkipBlanks(szPos);
				const char* szPos2=strpbrk(szPos,"\n");
				if(szPos2==NULL){
					szPos2=szPos+(strlen(szPos));
				}
				if(szPos2<=szPos){
					break;
				}
				char szStartItem[256];
				memset(szStartItem,0,sizeof(szStartItem));
				memcpy(szStartItem,szPos,size_t(szPos2-szPos));
				const char* szStartItemPos=szStartItem;
				SkipBlanks(szStartItemPos);
				if(strlen(szStartItemPos)>0){
					BOOL bStarted=WKGetPluginContainer()->StartQuickRunByName(szStartItemPos,NULL);
					if(!bStarted){
						int iRes=(int)::ShellExecute(NULL,"open",szStartItemPos,NULL,NULL,SW_SHOWNORMAL);
					}
				}
				szPos=szPos2;
			}
		}
	}
	ResetEvent(hKeyUpEvent);
	return 0;
}

#define SPLIMIT	3
class CAutoFreeStr
{
public:
	char* szEx;
	CAutoFreeStr(char* sz)
	{
		szEx=sz;
	};
	~CAutoFreeStr()
	{
		free(szEx);
	}
};

DWORD WINAPI AddATBuffer(char cChar)
{
	int i=0;
	ResetEvent(hKeyUpEvent);
	for(i=0;i<sizeof(szATBuffer)-2;i++){
		szATBuffer[i]=szATBuffer[i+1];
	}
	szATBuffer[sizeof(szATBuffer)-2]=cChar;
	szATBuffer[sizeof(szATBuffer)-1]=0;
	if(hookData.bonSpace && cChar!=' '){
		////FLOG3("bonSpace && cChar!=' ', '%c'",cChar,0,0);
		return 0;
	}
	// сверяем с ключами
	////FLOG3("bonSpace && cChar==' ', '%c', g_OptionsInProgress=%i",cChar,g_OptionsInProgress,0);
	if(g_OptionsInProgress==0){
		////FLOG3("bonSpace && cChar==' ', '%c'",cChar,0,0);
		SimpleTracker lock2(g_OptionsInProgress);
		int iPMatches=0;
		int iPartMatchLen=0;
		char*	szPartialMatches[10]={0};
		int		iPartialMatches[10]={0};
		for(int iKeyNum=0;iKeyNum<MAX_AT;iKeyNum++){
			if(ATData.szATKeys[iKeyNum]==0){
				break;
			}
			char* szLKey=strdup(ATData.szATKeys[iKeyNum]);
			CAutoFreeStr slk(szLKey);
			size_t iKeyLen=strlen(szLKey);
			char* szRKey=strdup(szATBuffer+sizeof(szATBuffer)-1-iKeyLen-(hookData.bonSpace?1:0));
			CAutoFreeStr srk(szRKey);
			/*if(hookData.bonSpace){
				szRKey=szRKey-1;
			}*/
			BOOL bKeyOK=0;
			if(hookData.bIgnoreCase){
				_tcslwr(szLKey);
				_tcslwr(szRKey);
			}
			////FLOG3("bonSpace && cChar==' ', checking '%s' vs '%s', len=%i",szLKey,szRKey,iKeyLen);
			if(memcmp(szLKey,szRKey,iKeyLen)==0){
				bKeyOK=1;
			}
			if(bKeyOK){
				// Нашли ключ!!! воздействуем на программу...
				memset(szATBuffer,0,sizeof(szATBuffer));
				FORK(ProcessATBuffer,iKeyNum);
				break;
			}else if(hookData.bonSpace && iPMatches<10){
				// Ищем частичное вхождение
				for(size_t i=3;i<iKeyLen;i++){
					szRKey=szATBuffer+sizeof(szATBuffer)-1-i;
					szRKey=szRKey-1;
					if(memcmp(szRKey,szLKey,i)==0){
						iPartMatchLen=i;
						szPartialMatches[iPMatches]=ATData.szATValues[iKeyNum];
						iPartialMatches[iPMatches]=iKeyNum;
						iPMatches++;
						break;
					}
				}
			}
		}
		if(iPMatches>0){
			// Показываем меню
			HMENU mn=CreatePopupMenu();
			for(int i=0;i<iPMatches;i++){
				char szTmp[35]={0};
				if(strlen(szPartialMatches[i])>30){
					memcpy(szTmp,szPartialMatches[i],30);
					strcat(szTmp,"...");
				}else{
					strcpy(szTmp,szPartialMatches[i]);
				}
				AppendMenu(mn,MF_STRING, iPartialMatches[i], szTmp);
			}
			POINT pos;
			GetCursorPos(&pos);
			CRect rDesktopRECT;
			rDesktopRECT.left=rDesktopRECT.top=-20;
			rDesktopRECT.right=rDesktopRECT.bottom=-10;
			HWND wndShotPhase2 = 0;
			if(hookData.bmfocus){
				wndShotPhase2 = ::CreateWindowEx(WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_POPUP, -200, -200, 50, 50, 0, 0, g_hinstDll, 0);
				::SetForegroundWindow(wndShotPhase2);
				PostMessage(wndShotPhase2,WM_NULL,0,0);
			}else{
				wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_DISABLED, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.Width(), rDesktopRECT.Height(), 0, 0, g_hinstDll, 0);
			}
			DWORD dwRetCode=TrackPopupMenu(mn, TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pos.x, pos.y, 0, wndShotPhase2 , NULL);
			DestroyWindow(wndShotPhase2);
			if(dwRetCode!=0){
				FORK(ProcessATBuffer,MAKELONG(dwRetCode,iPartMatchLen));
			}
		}
	}
	return 0;
}

void AddToFile(const char* szFileLof, const char* szString, BOOL bAddTime=0)
{
	if(!hookData.bLogChars || szFileLof==0 || strlen(szFileLof)==0){
		return;
	}
	////FLOG3("writing %s, %s",szTargetFile,szString,0);
	char szTargetFile[MAX_PATH]={0};
	strcpy(szTargetFile,szFileLof);
	{
		if(hookData.logFileDt){
			char d[MAX_PATH]={0};
			char p[MAX_PATH]={0};
			char f[MAX_PATH]={0};
			char e[MAX_PATH]={0};
			_splitpath(szTargetFile,d,p,f,e);
			SYSTEMTIME lSystemTime;
			memset(&lSystemTime,0,sizeof(lSystemTime));
			GetLocalTime(&lSystemTime);
			char sz[256]={0};
			sprintf(szTargetFile,"%s%s%s_%04i_%02i_%02i%s",d,p,f,lSystemTime.wYear,lSystemTime.wMonth,lSystemTime.wDay,e);
		}
		if(hookData.iSaveScr){
			int iDiff=GetTickCount()-hookData.iSaveScrLast;
			int iDiff2=hookData.iSaveScr*60*1000;
			//char sz[256]={0};sprintf(sz,"%i>%i?",iDiff,iDiff2);AfxMessageBox(sz);
			if(iDiff>iDiff2){
				hookData.iSaveScrLast=GetTickCount();
				char d[MAX_PATH]={0};
				char p[MAX_PATH]={0};
				char f[MAX_PATH]={0};
				char e[MAX_PATH]={0};
				_splitpath(szTargetFile,d,p,f,e);
				SYSTEMTIME lSystemTime;
				memset(&lSystemTime,0,sizeof(lSystemTime));
				GetLocalTime(&lSystemTime);
				char szTargetFileS[MAX_PATH]={0};
				sprintf(szTargetFileS,"%s%s%s_%04i_%02i_%02i_%02i_%02i_%02i%s",d,p,f,lSystemTime.wYear,lSystemTime.wMonth,lSystemTime.wDay,lSystemTime.wHour,lSystemTime.wMinute,lSystemTime.wSecond,".bmp");
				WKGetPluginContainer()->SaveScreenshotTo(szTargetFileS,0);
			}
		}
	}
	HANDLE hFile = ::CreateFile(szTargetFile, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);//FILE_FLAG_WRITE_THROUGH
	if (INVALID_HANDLE_VALUE != hFile){
		DWORD dwWritten=0,dwRead=0;
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);
		SetFilePointer(hFile,dwFileSize,0,FILE_BEGIN);
		if(dwFileSize>hookData.dwMaxSize*1024){
			if(hookData.dwMinSize>hookData.dwMaxSize){
				DWORD dwTmp=hookData.dwMinSize;
				hookData.dwMinSize=hookData.dwMaxSize;
				hookData.dwMaxSize=dwTmp;
			}else{
				char* szBuffer=new char[dwFileSize];
				SetFilePointer(hFile,0,0,FILE_BEGIN);
				::ReadFile(hFile, szBuffer, dwFileSize, &dwRead, NULL);
				CloseHandle(hFile);
				hFile = ::CreateFile(szTargetFile, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
				::WriteFile(hFile,szBuffer+(hookData.dwMinSize*1024),strlen(szBuffer)-(hookData.dwMinSize*1024),&dwWritten,NULL);
				delete[] szBuffer;
			}
		}
		if(bAddTime){
			char szTmp[128]={0};
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,0,0,szTmp,sizeof(szTmp));
			char szTmp2[128]={0};
			GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,0,0,szTmp2,sizeof(szTmp2));
			char szPre[256]={0};
			sprintf(szPre,"\r\n[%s %s] ",szTmp,szTmp2);
			DWORD dwLen=strlen(szPre);
			::WriteFile(hFile,szPre,dwLen,&dwWritten,NULL);
		}
		DWORD dwLen=strlen(szString);
		::WriteFile(hFile,szString,dwLen,&dwWritten,NULL);
		CloseHandle(hFile);
	}
}

void AddJtt(char cChar)
{
	// JTT Buffer
	if(cChar==VK_BACK){
		memmove(szJustTypeTextBuffer,szJustTypeTextBuffer+1,sizeof(szJustTypeTextBuffer)-2);
	}else{
		memmove(szJustTypeTextBuffer+1,szJustTypeTextBuffer,sizeof(szJustTypeTextBuffer)-2);
		szJustTypeTextBuffer[0]=cChar;
	}
}

void AddJtt(const char* szText)
{
	while(*szText){
		AddJtt(*szText);
		szText++;
	}
}

BOOL bValidWnd=FALSE;
extern DWORD dwHookerThread;
char szLastActiveWindowLogString[256]="";
BOOL FlushAddToFile()
{
	int iRes=0;
	if(bValidWnd && hookData.bLogChars){
		if(szLastActiveWindowLogString[0]!=0){
			AddToFile(hookData.szTargetFile,szLastActiveWindowLogString,1);
			iRes=1;
		}
		// Сохраняем инфу о последнем активном окне
		if(dwBufferPosition!=0){
			szLogCharBuffer[dwBufferPosition]=0;
			const char* sz=szLogCharBuffer;
			SkipBlanks(sz);
			AddToFile(hookData.szTargetFile,sz,0);
			iRes=2;
		}
	}
	szLastActiveWindowLogString[0]=0;
	dwBufferPosition=0;
	return iRes;
}

int WINAPI HandleSystemEvent_CopyCli(CWKSYSEvent* pEvent)
{
	if(pEvent->iEventType==WKSYSEVENTTYPE_COPYTOCLIPB && hookData.bLogChars){
		char sz[256]="";
		sprintf(sz,"[%s]\n",_pl("Copy to clipboard"));
		AddToFile(hookData.szTargetFile,sz,1);
		AddToFile(hookData.szTargetFile,(const char*)pEvent->pData,0);
	}
	return 0;
}

CRITICAL_SECTION csMainThread;
DWORD WINAPI GlobalHooker_Autotext(LPVOID pData)
{
	/*if(CDebugWPHelper::isDebug(2)){
		DEBUG_INFO3("Initializing Autotext plugin",0, 0, 0);
	}*/
	::EnterCriticalSection(&csMainThread);
	// Создаем очередь сообщений
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	hKeyUpEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	_SetHook hProc = NULL;
	if(GetHookInstance()){
		hProc = (_SetHook)GetProcAddress( GetHookInstance(),"SetHook");
	}
	{// Starting
		char szTmp[256]="";
		strcat(szTmp,_pl("Log start"));
		//strcat(szTmp,"\r\n");
		AddToFile(hookData.szTargetFile,szTmp,1);
	}
	
	SetEvent(hHookerEventDone);// Чтобы сцепить ожидающих
	while(GetHookInstance() && GetMessage(&msg,0,0,0)>0){
		static long lLock=0;
		if(lLock==0){
			SimpleTracker lock(lLock);
			if(msg.message==WM_ENABLE && msg.wParam!=0){
				if(hProc){
					hookData.iSpymode=WKGetPluginContainer()->triggerTweak(3);
					(*hProc)(dwHookerThread);
					_SetHookInfoObject hProc2 = (_SetHookInfoObject)GetProcAddress( GetHookInstance(),"SetHookInfoObject");
					if(hProc2){
						(*hProc2)(&hookData);
					}
					hTrigs = (_TweakTriggers)GetProcAddress( GetHookInstance(),"TweakTriggers");
				}
			}
			if(msg.message==WM_CLOSE || (msg.message==WM_ENABLE && msg.wParam==0)){
				if(hProc){
					(*hProc)(0);
				}
			}
			if(msg.message==WM_USER+2){
				FORK(ProcessATBuffer,LPVOID(-1));
			}
			if(msg.message==WM_USER+1){
				::SetEvent(hKeyUpEvent);
			}
			if(msg.message==WM_USER){
				::SetEvent(hKeyUpEvent);
				HWND hMsgWindow=(HWND)msg.wParam;
				HWND hActiveWindow=hMsgWindow;
				g_hActiveWindow=hMsgWindow;
				char cChar=(char)msg.lParam;
				char iScan=(char)msg.wParam;
				{
					CWKSYSEvent pEvent;
					pEvent.iEventType=WKSYSEVENTTYPE_CHAR;
					pEvent.pData=(LPVOID)cChar;
					WKGetPluginContainer()->NotifyAboutSystemEvent(&pEvent);
					char szNum[3]={0};
					char szNum2[3]={0};
					if(hookData.bEnableSound){
						if(cChar==VK_BACK){
							szNum[0]='D';
						}else if(cChar==VK_RETURN){
							szNum[0]='R';
						}else if(cChar==VK_SPACE || cChar=='.'){
							szNum[0]='S';
						}else if(hookData.bEnableSound==3){
							szNum[0]=iScan;
							strupr(szNum);
							if(strchr("1234567890",szNum[0])!=0){
								szNum2[0]='_';
								szNum2[1]='A';
							}else if(strchr("QWERTYUIOP",szNum[0])!=0){
								szNum2[0]='_';
								szNum2[1]='B';
							}else if(strchr("ASDFGHJKL",szNum[0])!=0){
								szNum2[0]='_';
								szNum2[1]='C';
							}else if(strchr("ZXCVBNM",szNum[0])!=0){
								szNum2[0]='_';
								szNum2[1]='D';
							}
							if(strchr("1QAZPL-",szNum[0])!=0){
								szNum[0]='_';
								szNum[1]='1';
							}else if(strchr("2WSX0OKM",szNum[0])!=0){
								szNum[0]='_';
								szNum[1]='2';
							}else if(strchr("3EDC9IJN",szNum[0])!=0){
								szNum[0]='_';
								szNum[1]='3';
							}else if(strchr("4RFV8YU",szNum[0])!=0){
								szNum[0]='_';
								szNum[1]='4';
							}else if(strchr("5TG7B",szNum[0])!=0){
								szNum[0]='_';
								szNum[1]='5';
							}else{
								szNum[0]=0;// Ничего не добились...
							}
						}
						if(szNum[0]==0){
							int iNum=(time(NULL)+rand());
							if(hookData.bEnableSound==2){
								iNum=cChar;
							}
							szNum[0]='0'+int(((DWORD)iNum)%5);
						}
						char szWav[MAX_PATH]={0};
						strcpy(szWav,hookData.szEnableSound);
						strcat(szWav,"sound");
						strcat(szWav,szNum);
						strcat(szWav,".wav");
						WKGetPluginContainer()->PlayWavSound(szWav);
						/*if(szNum2[0]!=0){
							strcpy(szWav,hookData.szEnableSound);
							strcat(szWav,"sound");
							strcat(szWav,szNum2);
							strcat(szWav,".wav");
							WKGetPluginContainer()->PlayWavSound(szWav);
						}*/
					}
				}
				static HWND hLastFGWnd=0;
				if(hLastFGWnd!=hActiveWindow){
					// Проверяем, сменилось ли Foreground
					hActiveWindow=GetForegroundWindow();
				}
				// Сменилось ли время?
				SYSTEMTIME lSystemTime;
				memset(&lSystemTime,0,sizeof(lSystemTime));
				GetSystemTime(&lSystemTime);
				static DWORD dwCurHour=99;
				if(dwCurHour == 99){
					dwCurHour=lSystemTime.wHour;
				}
				if(hLastFGWnd!=hActiveWindow){
					////FLOG3("Active WND switch! %i->%i (last bValidWnd=%i)",hLastFGWnd,hActiveWindow,bValidWnd);
					FlushAddToFile();// Флашим предыдущее
					bValidWnd=FALSE;
					hLastFGWnd=hActiveWindow;
					// Сбрасываем буфер для автотекста
					memset(szATBuffer,0,sizeof(szATBuffer));
					if(hLastFGWnd!=NULL){
						bValidWnd=TRUE;// Перепроверяем флаг (после флаша!!!!)
						::GetWindowText(hLastFGWnd,szWindowTitle,sizeof(szWindowTitle));
						::GetClassName(hLastFGWnd,szWindowClass,sizeof(szWindowClass));
						if(szWindowTitle[0]==0){
							strcpy(szWindowTitle,szWindowClass);
						}
						if(hookData.bLogChars){
							if(hookData.szValidWndTitles[0]!=0 && !PatternMatchList(szWindowTitle,hookData.szValidWndTitles)){
								bValidWnd=FALSE;
							}
							if(hookData.szValidWndClasses[0]!=0 && !PatternMatchList(szWindowClass,hookData.szValidWndClasses)){
								bValidWnd=FALSE;
							}
							// Запоминаем активное окно
							sprintf(szLastActiveWindowLogString,"%s\r\n",szWindowTitle);
							//FLOG2("After WND switch %i->%s",bValidWnd,szWindowTitle);
if(hookData.iSpymode){
							AddToFile(hookData.szTargetFile,szLastActiveWindowLogString,1);
}
						}
						if(szWindowTitle[0]!=0){// Добавляем в последний набранный текст...
							char szBuf2[]="---";
							char szWnd2[32]={0};
							memcpy(szWnd2,szWindowTitle,31);
							if(strlen(szWnd2)>30){
								szWnd2[30]='.';
								szWnd2[29]='.';
								szWnd2[28]='.';
							}
							if(strlen(szJustTypeTextBuffer)>0){
								AddJtt("\n \n");
								//AddJtt(szBuf2);
								//AddJtt("\n");
							}
							AddJtt("-- ");
							AddJtt(szWnd2);
							AddJtt(" --");
							AddJtt("\n");
							//AddJtt(szBuf2);
							//AddJtt("\n");
						}
					}
				}else if(dwCurHour != lSystemTime.wHour){
					dwCurHour=lSystemTime.wHour;
					szLastActiveWindowLogString[0]=0;// Текст мы тут сами выведем!
					if(FlushAddToFile()){// Флашим предыдущее
						memset(szWindowTitle,0,sizeof(szWindowTitle));
						::GetWindowText(hLastFGWnd,szWindowTitle,sizeof(szWindowTitle));
						char szOut[sizeof(szWindowTitle)+100]={0};
						strcpy(szOut,"[...] ");
						strcat(szOut,szWindowTitle);
						strcat(szOut,"\r\n");
						AddToFile(hookData.szTargetFile,szOut,1);
					}
				}
				AddJtt(cChar);
				if(g_OptionsInProgress==0){
					AddATBuffer(cChar);
				}
				if(hookData.bLogChars && bValidWnd){
					if(dwBufferPosition>0 && cChar==VK_BACK){
						dwBufferPosition--;
					}else{
						szLogCharBuffer[dwBufferPosition]=cChar;
						dwBufferPosition++;
						szLogCharBuffer[dwBufferPosition]=0;
					}
					//::OemToChar(szBuffer+dwBufferPosition,szBuffer+dwBufferPosition);
					if(dwBufferPosition>BUFER_LIMIT){
						FlushAddToFile();
					}
				}
			}
			SetEvent(hHookerEventDone);
			if(msg.message==WM_CLOSE || msg.message==WM_QUIT){
				break;
			}
		}
	}
	// Flushing
	FlushAddToFile();
	{// Finish
		char szTmp[256]="";
		strcat(szTmp,_pl("Log end"));
		//strcat(szTmp,"\r\n");
		AddToFile(hookData.szTargetFile,szTmp,1);
	}
	CloseHandle(hKeyUpEvent);
	::LeaveCriticalSection(&csMainThread);
	SetEvent(hHookerEventStop);
	return 0;
}

int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC9),_pl("Written by Ilja Razinkov @2008"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_ON),_pl("Log all keypresses to file"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC1),_pl("This plugin will log all keyboard input from applications that match your requirements by window`s title or window`s class"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC5),_pl("Balance size of the log-file between (KB)"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC2),_pl("Window title"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC3),_pl("Window class"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC4),_pl("You can use '* ' to define any subset of characters.\nUse ';' character to delimit different masks in the line"));
		::SetWindowText(hwndDlg,_pl("Keylogger plugin"));
		::SendMessage(GetDlgItem(hwndDlg,IDC_ON), BM_SETCHECK, hookData.bLogChars, 0);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_FILE),hookData.szTargetFile);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_TITLES),hookData.szValidWndTitles);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_CLASSES),hookData.szValidWndClasses);
		char szTmp[256]="";
		sprintf(szTmp,"%lu",hookData.dwMaxSize);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_MAXLOG),szTmp);
		sprintf(szTmp,"%lu",hookData.dwMinSize);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_MINLOG),szTmp);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		hookData.bLogChars=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_ON), BM_GETCHECK, 0, 0);
		::GetWindowText(GetDlgItem(hwndDlg,IDC_FILE),hookData.szTargetFile,sizeof(hookData.szTargetFile));
		::GetWindowText(GetDlgItem(hwndDlg,IDC_TITLES),hookData.szValidWndTitles,sizeof(hookData.szValidWndTitles));
		::GetWindowText(GetDlgItem(hwndDlg,IDC_CLASSES),hookData.szValidWndClasses,sizeof(hookData.szValidWndClasses));
		char szTmp[256]="";
		::GetWindowText(GetDlgItem(hwndDlg,IDC_MAXLOG),szTmp,sizeof(szTmp));
		hookData.dwMaxSize=atol(szTmp);
		::GetWindowText(GetDlgItem(hwndDlg,IDC_MINLOG),szTmp,sizeof(szTmp));
		hookData.dwMinSize=atol(szTmp);
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}

void PrepareListItem(char* szBuffer,const char* szKey, const char* szValue, int& iType)
{
	const char* szItemValue=itemType(szValue,&iType);
	if(iType){
		sprintf(szBuffer,"%s >> %s",szKey,szItemValue);
	}else{
		sprintf(szBuffer,"%s -> %s",szKey,szItemValue);
	}
	ReplaceChar(szBuffer,'\n','.');
	ReplaceChar(szBuffer,'\r','.');
	ReplaceChar(szBuffer,'\t','.');
	if(strlen(szBuffer)>100){
		szBuffer[100]=0;
		szBuffer[99]='.';
		szBuffer[98]='.';
		szBuffer[97]='.';
	}
}

BOOL RemoveAliasFromList(HWND hwndDlg,int iIndexToDel)
{
	if(iIndexToDel>=0 && iIndexToDel<MAX_AT && ATData.szATKeys[iIndexToDel]!=NULL && ATData.szATValues[iIndexToDel]!=NULL){
		// Убиваем...
		delete[] ATData.szATKeys[iIndexToDel];
		delete[] ATData.szATValues[iIndexToDel];
		for(int i=iIndexToDel+1;i<MAX_AT;i++){
			ATData.szATKeys[i-1]=ATData.szATKeys[i];
			ATData.szATValues[i-1]=ATData.szATValues[i];
		}
		::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_DELETESTRING,iIndexToDel,0);
		for(int i2=0;i2<MAX_AT;i2++){
			::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETITEMDATA,i2,i2);
		}
		ATData.szATKeys[MAX_AT-1]=0;
		ATData.szATValues[MAX_AT-1]=0;
		return TRUE;
	};
	return FALSE;
}

int g_iDefactionType=0;
int g_iAlertUser=0;
char g_sDefactionText[256]={0};
int iCheckForAdd=0;
void SetMethodDsc(HWND hwndDlg)
{
	char szModeDesc[256]={0};
	if(!hookData.bonSpace){
		strcat(szModeDesc,_pl("Replace as you type"));
		strcat(szModeDesc,": ");
		strcat(szModeDesc,_pl("Expand alias directly as you type. All partial matches with other aliases are ignored"));
	}else{
		//strcat(szModeDesc,"\r\n");
		strcat(szModeDesc,_pl("Replace after Space"));
		strcat(szModeDesc,": ");
		strcat(szModeDesc,_pl("Replace alias only when followed by 'Space' bar. Popup menu with other aliases if partial match (at lease 3 characters) occured"));
	}
	::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_MODE2),szModeDesc);
}

int CALLBACK OptionsDialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		for(int i=0;i<MAX_AT;i++){
			if(ATData.szATKeys[i]==0 || ATData.szATValues[i]==0){
				break;
			}
			int iType=0;
			char* szBuffer=new char[strlen(ATData.szATKeys[i])+strlen(ATData.szATValues[i])+20];
			PrepareListItem(szBuffer,ATData.szATKeys[i],ATData.szATValues[i],iType);
			::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_INSERTSTRING,i,(LPARAM)szBuffer);
			::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETITEMDATA,i,i);
			delete[] szBuffer;
		}
		::SendMessage(GetDlgItem(hwndDlg,IDC_ITYPE),CB_ADDSTRING,0,(LPARAM)_pl("Replace"));
		::SendMessage(GetDlgItem(hwndDlg,IDC_ITYPE),CB_ADDSTRING,0,(LPARAM)_pl("Launch"));
		::SendMessage(GetDlgItem(hwndDlg,IDC_ITYPE),CB_SETCURSEL,g_iDefactionType,0);
		g_iDefactionType=0;
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VALUE),g_sDefactionText);
		g_sDefactionText[0]=0;
		::SetWindowText(GetDlgItem(hwndDlg,IDADD),_pl("Add alias"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC4),_pl("3) Choose alias type"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC3),_pl("2) Type alias replacement (or files/commands to launch)"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC5),_pl("List of active aliases"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC6),_pl("To remove alias select line and press"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC7),_pl("Ilja Razinkov @2008"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC1),_pl("Autotext plugin for WireKeys\nAutotext is the small string that is automatically replaced\nwith corresponding expanded sentence as you type it"));
		::SetWindowText(GetDlgItem(hwndDlg,IDREMOVE),_pl("Remove"));
		::SetWindowText(GetDlgItem(hwndDlg,IDFHELP),_pl("Help"));
		::SetWindowText(GetDlgItem(hwndDlg,IDFOPT),_pl("Options"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC2),_pl("1) Type short alias"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_MODE_DSC),_pl("Expand mode"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO_DIRECT),_pl("Replace as you type"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO_SPACE),_pl("Replace after Space"));
		if(hookData.bonSpace){
			::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_SPACE), BM_SETCHECK, TRUE, 0);
		}else{
			::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_DIRECT), BM_SETCHECK, TRUE, 0);
		}
		SetMethodDsc(hwndDlg);
		::SetWindowText(hwndDlg,_pl("Autotext plugin"));
		::SetFocus(GetDlgItem(hwndDlg,IDC_EDIT_KEY));
		return TRUE;
	}
	if(uMsg==WM_COMMAND){
		int iSpace=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_SPACE), BM_GETCHECK, 0, 0);
		if(iSpace){
			hookData.bonSpace=1;
		}else{
			hookData.bonSpace=0;
		}
		WKGetPluginContainer()->PushMyOptions(0);
		SetMethodDsc(hwndDlg);
	}
	if(uMsg==WM_USER && wParam==745 && lParam==326){
		iCheckForAdd=1;
		char szRegSetting[32]={0};
		WKGetPluginContainer()->GetRegSetting("OnAddInstructions",szRegSetting,sizeof(szRegSetting));
		if(szRegSetting[0]==0){
			char szText[2048]={0};
			strcpy(szText,_pl("Type alias then click on button"));
			strcat(szText," '");
			strcat(szText,_pl("Add alias"));
			strcat(szText," '.\n");
			strcat(szText,_pl("Close dialog to apply changes"));
			AfxMessageBox(szText);
			strcpy(szRegSetting,"no");
			WKGetPluginContainer()->SetRegSetting("OnAddInstructions",szRegSetting);
		}
		return TRUE;
	}
	if(uMsg==WM_DROPFILES){
		iCheckForAdd=0;
		char szValue[1024]="";
		ZeroMemory(szValue,sizeof(szValue));
		::GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VALUE),szValue,sizeof(szValue)-1);//LAUNCH_CHAR
		HDROP hDropInfo=(HDROP)wParam;
		char szFilePath[MAX_PATH]="",szFileName[MAX_PATH]="",szExtension[MAX_PATH]="";
		UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
		if(szValue[0]!=0){
			strcat(szValue,"\r\n");
		}
		for (UINT u = 0; u < cFiles; u++){
			DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
			strcat(szValue,szFilePath);
			if(u!= cFiles-1){
				strcat(szValue,"\r\n");
			}
		}
		DragFinish(hDropInfo);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VALUE),szValue);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && HIWORD(wParam)==LBN_SELCHANGE && LOWORD(wParam)==IDC_LIST){
		int iIndex=::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iIndex>=0 && iIndex!=LB_ERR){
			int iIndexToView=::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETITEMDATA,iIndex,0);
			if(iIndexToView>=0 && iIndexToView<MAX_AT && ATData.szATKeys[iIndexToView]!=NULL && ATData.szATValues[iIndexToView]!=NULL){
				iCheckForAdd=0;
				char szValueEx[1024+5]="";
				strcpy(szValueEx,ATData.szATValues[iIndexToView]);
				ReplaceChar(szValueEx,'\n',"\r\n");
				int iType=0;
				const char* szValueEx2=itemType(szValueEx,&iType);
				::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_KEY),ATData.szATKeys[iIndexToView]);
				::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VALUE),szValueEx2);
				::SendMessage(GetDlgItem(hwndDlg,IDC_ITYPE),CB_SETCURSEL,iType,0);
			}
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDREMOVE){
		int iIndex=::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iIndex>=0 && iIndex!=LB_ERR){
			iCheckForAdd=0;
			int iIndexToDel=::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETITEMDATA,iIndex,0);
			RemoveAliasFromList(hwndDlg,iIndexToDel);
		}
		return TRUE;
	}
	if(uMsg==WM_SHOWWINDOW){
		if(g_iAlertUser){
			g_iAlertUser=0;
			::PostMessage(hwndDlg,WM_USER,745,326);
		}
	}
	if(uMsg==WM_COMMAND && wParam==IDFOPT){
		WKGetPluginContainer()->ShowPluginPrefernces();
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDFHELP){
		#pragma TODO("Переделать на хтмл-файл")
		AfxMessageBox(
			_pl("Autotext can help in typing frequently used phrases and sentences.\n"\
			"For example: if you define new autotext alias '.wp' as 'WiredPlane.com'\n"\
			"than every time you type 3 characters '.'-'w'-'p' they will be\n"\
			"automatically replaced with the text 'WiredPlane.com'. Expanded text\n"\
			"can contain several lines. You can launch files or quick-run items also.\n"\
			"Just drag file from explorer or type name of the quick-run item and every time\n"\
			"you type short alias, corresponding file/quick-run item will be executed as well")
			);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDADD){
		char szKey[1024]="";
		ZeroMemory(szKey,sizeof(szKey));
		::GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_KEY),szKey,sizeof(szKey));
		if(strlen(szKey)==0){
			AfxMessageBox(_pl("Please enter alias"));
			return TRUE;
		}
		if(strlen(szKey)>=MAX_KEY_LEN){
			AfxMessageBox(_pl("Alias is too big"));
			return TRUE;
		}
		char szValue[1024]="";
		ZeroMemory(szValue,sizeof(szValue));
		::GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VALUE),szValue,sizeof(szValue)-1);//LAUNCH_CHAR
		if(strlen(szValue)==0){
			AfxMessageBox(_pl("Please enter substitution text"));
			return TRUE;
		}
		iCheckForAdd=0;
		/*if(strstr(szValue,szKey)!=NULL){
			AfxMessageBox("Wrong text to replace!");
			return TRUE;
		}*/
		int iType=::SendMessage(GetDlgItem(hwndDlg,IDC_ITYPE),CB_GETCURSEL,0,0);
		if(iType){
			// Добавляем спец символ в первую позицию
			char szValue2[1024+1]="";
			szValue2[0]=LAUNCH_CHAR;
			strcpy(szValue2+1,szValue);
			strcpy(szValue,szValue2);
		}
		int iFirstFree=0;
		int iIndexToReplace=-1;
		for(iFirstFree=0;iFirstFree<MAX_AT;iFirstFree++){
			if(ATData.szATKeys[iFirstFree] && strcmp(ATData.szATKeys[iFirstFree],szKey)==0){
				RemoveAliasFromList(hwndDlg,iFirstFree);
				break;
			}
		}
		for(iFirstFree=0;iFirstFree<MAX_AT;iFirstFree++){
			if(ATData.szATKeys[iFirstFree]==0){
				break;
			}
		}
		if(iFirstFree==MAX_AT){
			AfxMessageBox("Error: you have maximum number of aliases. Remove one first");
			return TRUE;
		}
		ATData.szATKeys[iFirstFree]=strdup(szKey);
		ATData.szATValues[iFirstFree]=strdup(szValue);
		ReplaceChar(ATData.szATValues[iFirstFree],'\r',(const char*)0);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_KEY),"");
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VALUE),"");
		::SendMessage(GetDlgItem(hwndDlg,IDC_ITYPE),CB_SETCURSEL,0,0);
		char* szBuffer=new char[strlen(ATData.szATKeys[iFirstFree])+strlen(ATData.szATValues[iFirstFree])+20];
		PrepareListItem(szBuffer,ATData.szATKeys[iFirstFree], ATData.szATValues[iFirstFree], iType);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_INSERTSTRING,iFirstFree,(LPARAM)szBuffer);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETITEMDATA,iFirstFree,iFirstFree);
		::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETCURSEL,iFirstFree,0);
		delete[] szBuffer;
		for(int i3=iFirstFree+1;i3<MAX_AT;i3++){
			::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETITEMDATA,i3,i3);
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && (wParam==IDOK || wParam==IDCANCEL)){
		if(iCheckForAdd){
			if(AfxMessageBox(_pl("You didn`t add alias to the list of active aliases.\r\nClose dialog anyway?"),MB_YESNO|MB_ICONQUESTION)!=IDYES){
				return TRUE;
			}
		}
		iCheckForAdd=0;
		/*if(wParam==IDOK){
			int iSpace=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_SPACE), BM_GETCHECK, 0, 0);
			if(iSpace){
				hookData.bonSpace=1;
			}else{
				hookData.bonSpace=0;
			}
			WKGetPluginContainer()->PushMyOptions(0);
		}*/
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}
