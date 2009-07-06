#include "stdafx.h"
#include <atlbase.h>
#include "WP_AverTVRem.h"
#include "HookCode.h"

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

CString _l(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

#define MAXLEN	1024
COptions plgOptions;
HANDLE hStopEvent=0;
//HINSTANCE g_hinstDll=NULL;
HANDLE hHookerThread=NULL;
CRITICAL_SECTION csMainThread;
WKCallbackInterface* pInter=NULL;
DWORD WINAPI GlobalHooker_Aver(LPVOID pData);
int WINAPI onAddEvent(char const* szEventID);
HANDLE hSocketThread=0;
SOCKET currentWLSocket=0;
long lWLConnectionStatus=0;
BOOL ConnectWinLIRC()
{
	int rVal=0;
	SOCKET theSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	currentWLSocket=theSocket;
	if(theSocket == SOCKET_ERROR){
		return 0;
	}
	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	LPSTR lpszAscii = T2A((LPTSTR)plgOptions.szWinLircIP);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(plgOptions.dwWinLircPort);
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszAscii);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL);
			return 0;
		}
	}
	
	rVal=connect(theSocket,(LPSOCKADDR)&sockAddr, sizeof(sockAddr));
	if(rVal==SOCKET_ERROR){
		return 0;
	}
	// Xbnftv lfyyst
	char buffer=' ';
	char szFromLirc[1024]={0};
	lWLConnectionStatus=1;
	WKGetPluginContainer()->PushMyOptions(-1);
	while(1){
        rVal = recv(theSocket, &buffer, 1, 0);
		if(rVal == 0){
			//int errorVal = WSAGetLastError();
			rVal = SOCKET_ERROR;
		}
        if(rVal == SOCKET_ERROR)
        {
            /*int errorVal = WSAGetLastError();
            if(errorVal == WSAENOTCONN)
            {
                socketError("Socket not connected!");
            }*/
			// Отсюда 1 так как сами выкидываем
			memset(szFromLirc,0,sizeof(szFromLirc));
			return 1;
        }
        
        if(buffer == '\n'){
            //AfxMessageBox(szFromLirc);
			unsigned __int64 keycode;
			unsigned int repeat;
			char keyname[MAXLEN];
			char remotename[MAXLEN];
			sscanf(szFromLirc,"%I64x %x %s %[^\n]",&keycode,&repeat,keyname,remotename);
			// send it to the app
			HandleButton(keyname, repeat);
			memset(szFromLirc,0,sizeof(szFromLirc));
        }else if(rVal > 0){
			char szTmp[2]={0};
			szTmp[0]=buffer;
            strcat(szFromLirc,szTmp);
        }
    }
	closesocket(theSocket);
	return 1;
}

DWORD WINAPI GlobalHooker_WinLIRC(LPVOID)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	lWLConnectionStatus=0;
	currentWLSocket=0;
	WKGetPluginContainer()->PushMyOptions(-1);
	BOOL bRes=1;
	if(plgOptions.szWinLircIP[0]!=0 && plgOptions.dwWinLircPort>0){
		WORD version=MAKEWORD(1,1);
		WSADATA wsaData;
		WSAStartup(version,(LPWSADATA)&wsaData);
		bRes=ConnectWinLIRC();
		WSACleanup();
	}
	currentWLSocket=0;
	lWLConnectionStatus=0;
	WKGetPluginContainer()->PushMyOptions(-1);
	if(!bRes){
		WKGetPluginContainer()->ShowAlert(_l("WinLIRC not found\r\nCheck WinLIRC settings"),"Plugin error");
	}
	CloseHandle(hSocketThread);
	hSocketThread=0;
	return 0;
}

int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// Version...
	if(init){
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
	pInter=init;
	DWORD dwHookerThread=0;
	::InitializeCriticalSection(&csMainThread);
	hStopEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_Aver, NULL, 0, &dwHookerThread);
	WKGetPluginContainer()->RegisterOnAddRemoveEventCallback(onAddEvent,0);
	return 1;
}

int	WINAPI WKPluginStart()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return 1;
}

int	WINAPI WKPluginStop()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(!pInter->getOption(WKOPT_ISSHUTDOWN)){
		SetEvent(hStopEvent);
		::WaitForSingleObject(hHookerThread,2000);
		closesocket(currentWLSocket);
		::WaitForSingleObject(hSocketThread,2000);
		//::EnterCriticalSection(&csMainThread);
		//::LeaveCriticalSection(&csMainThread);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		TerminateThread(hHookerThread,66);
	}
	::DeleteCriticalSection(&csMainThread);
	::CloseHandle(hHookerThread);
	::CloseHandle(hStopEvent);
	return 1;
}

#define MODE_TITLE	"Mode #"
int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction>4 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==4){
		strcpy(stuff->szItemName,"Normal mode\tReturn remote into default state");
	}else{
		memset(stuff->szItemName,0,strlen(MODE_TITLE));
		strcpy(stuff->szItemName,MODE_TITLE);
		stuff->szItemName[strlen(MODE_TITLE)]=iPluginFunction+'1';
		stuff->szItemName[strlen(MODE_TITLE)+1]=0;
		strcat(stuff->szItemName,"\tUse remote mode to switch buttons meaning for different actions. For example 'Power' button can start TV tuner in normal mode and shutdown computer in mode #1");
	}
	stuff->hItemBmp=0;
	stuff->dwItemMenuPosition=0;
	stuff->dwDefaultHotkey=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction>4 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iPluginFunction>4 || stuff==NULL){
		return 0;
	}
	char szID[5]="IRM1";
	szID[3]=char('1'+iPluginFunction);
	char szOSD[128]={0};
	strcpy(szOSD,_l("No mode"));
	if(iPluginFunction<4 && plgOptions.dwModeStates[iPluginFunction]){
		WKGetPluginContainer()->NotifyEventFinished(szID);
		plgOptions.dwModeStates[iPluginFunction]=0;
		plgOptions.dwPrevMode=-1;
		//WKGetPluginContainer()->NotifyEventStarted("IRM0");
	}else{
		if(plgOptions.dwPrevMode!=-1){
			// Выходим из предыдущего режима...
			WKCallPluginFunction(plgOptions.dwPrevMode,stuff);
		}
		if(iPluginFunction<4){
			//WKGetPluginContainer()->NotifyEventFinished("IRM0");
			WKGetPluginContainer()->NotifyEventStarted(szID);
			plgOptions.dwModeStates[iPluginFunction]=1;
			plgOptions.dwPrevMode=iPluginFunction;
			strcpy(szOSD,_l("Entering mode"));
			strcat(szOSD," #");
			strcat(szOSD,szID+3);
		}
	}
	WKGetPluginContainer()->ShowOSD(szOSD,2000);
	return 1;
}

bool IsAverPresent(void);
DWORD lDefWinLIRCPort=0;
bool IsWinLIRCPresent()
{
	CRegKey key;
	/* First try HKCU, then HKLM */
	if(key.Open(HKEY_CURRENT_USER,"Software\\LIRC")!=ERROR_SUCCESS){
		if(key.Open(HKEY_LOCAL_MACHINE,"Software\\LIRC")
			!=ERROR_SUCCESS){
			return false;
		}
	}
	char s[512]={0};
	DWORD len=sizeof(s);
	if(key.QueryValue(s,"port",&len)!=ERROR_SUCCESS)
		return false;
	lDefWinLIRCPort=8765;
	if(key.QueryValue(s,"tcp_port",&len)==ERROR_SUCCESS){
		lDefWinLIRCPort=atol(s);
	}
	return true;
};

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	dsc->bResidentPlugin=TRUE;
	BOOL bIsWPresNoOptimize=IsWinLIRCPresent();
	dsc->bLoadPluginByDefault=(IsAverPresent()||bIsWPresNoOptimize)?1:0;
	//strcpy(dsc->szTitle,"WinLIRC Remote controls");
	strcpy(dsc->szDesk,"Use Remote controls in hotkeys (supports WinLIRC and AverTV cards)");
	strcpy(dsc->szAuthors,"WiredPlane.com, @2008");
	dsc->hPluginBmp=::LoadBitmap(GetModuleHandle("WP_WinLirc.wkp"),MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(!plgOptions.bNoAver){
		::WinExec(plgOptions.szIniFilePath,1);
	}else{
		AfxMessageBox(_l("Use WinLIRC options to learn buttons.\r\nWireKeys will automatically add new buttons\r\nto hotkeys contex menu"));
	}
	return 1;
}

BOOL LoadConfig(BOOL bForWinLIRC);
BOOL UnLoadConfig(BOOL bForWinLIRC);
BOOL GetIniFile(BOOL bForWinLIRC, char* szInit);
int WINAPI onAddEvent(char const* szEventID)
{
	if(szEventID && strcmp(szEventID,"IRM5")==0){
		UnLoadConfig(1);
		char szInit[MAX_PATH]={0};
		GetIniFile(1,szInit);
		::DeleteFile(szInit);
		WKGetPluginContainer()->ShowAlert(_l("To add remote buttons again, press them in any order"),"Ok");
		return 1;
	}
	return 0;
}

void AddWinLircevent(const char * sEvent,unsigned long dwRepeat, char* szID)
{
	int iEventNum=0;
	// Смотрим есть ли такой ивент в карте...
	for(iEventNum=0;iEventNum<plgOptions.szWinLIRCKeys.GetSize();iEventNum++){
		if(strcmp(sEvent,plgOptions.szWinLIRCKeys[iEventNum])==0){
			// !!! Present
			sprintf(szID,"WR%02X",iEventNum);
			return;
		}
	}
	// Нет - регистрим...
	char szClass[16]={0};
	char szKeyNames[32]={0};
	strcpy(szClass,_l("Remote buttons"));
	iEventNum=plgOptions.szWinLIRCKeys.GetSize();
	sprintf(szID,"WR%02X",iEventNum);
	plgOptions.szWinLIRCKeys.Add(sEvent);
	char szShortDescription[64]={0};
	strcpy(szShortDescription,sEvent);
	WKGetPluginContainer()->RegisterEvent(szID,szShortDescription,szClass);
	sprintf(szKeyNames,"WL_Button_%lu=",iEventNum);
	{
		char szInit[MAX_PATH]={0};
		GetIniFile(1,szInit);
		FILE* f=fopen(szInit,"a+");
		if(f){
			fprintf(f,"%s%s",szKeyNames,sEvent);
			fprintf(f,"\n");
			fclose(f);
		};
	};
}

void HandleButton(const char * sEvent,unsigned long dwRepeat)
{
	if(!sEvent){
		return;
	}
	char szEventOK[16]={0};
	if(strlen(sEvent)>15){
		memcpy(szEventOK,sEvent,15);
	}else{
		strcpy(szEventOK,sEvent);
	}
	char szID[5]={0};
	AddWinLircevent(szEventOK,dwRepeat, szID);
	if(szID[0]!=0){
		WKGetPluginContainer()->NotifyEventStarted(szID);
		WKGetPluginContainer()->PlayWavSound(plgOptions.szWav);
		WKGetPluginContainer()->NotifyEventFinished(szID);
		if(plgOptions.bDebugMode){
			char szOSDText[64]={0};
			strcpy(szOSDText,szEventOK);
			if(plgOptions.dwPrevMode!=-1 && szOSDText[0]!=0){
				char szModeNum[]=" ";
				szModeNum[0]=char('1'+plgOptions.dwPrevMode);
				strcat(szOSDText," (");
				strcat(szOSDText,_l("mode"));
				strcat(szOSDText," #");
				strcat(szOSDText,szModeNum);
				strcat(szOSDText,")");
			}
			if(szOSDText[0]!=0){
				WKGetPluginContainer()->ShowOSD(szOSDText,5000);
			}
		}
	}
};

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(iAction==OM_STARTUP_ADD){
		plgOptions.bNoAver=IsAverPresent()?0:1;
		if(plgOptions.szIniFilePath[0]==0){
			WKGetPluginContainer()->GetWKPluginsDirectory(plgOptions.szIniFilePath,MAX_PATH);
			strcat(plgOptions.szIniFilePath,"gpio_avertv.txt");
		}
		pOptionsCallback->AddTip("tip1","To add remote button to hotkey, open context menu and choose desired button from drop-down submenu. You can add more than one remote button. Such hotkey can be 'triggered' by quickly pressing corresponding buttons one after another without delays",0);
		if(!plgOptions.bNoAver){
			pOptionsCallback->AddStringOption("IniFile","AverTV Remote configuration","",plgOptions.szIniFilePath,1);
		}
		pOptionsCallback->AddStringOption("WinLIRCip","WinLirc ip address","Leave empty if you don`t want to use WinLIRC","127.0.0.1");
		pOptionsCallback->AddNumberOption("WinLIRCport","WinLirc port","Leave empty if you don`t want to use WinLIRC",lDefWinLIRCPort,0,65535);//8765
		pOptionsCallback->AddListOption("wlStatus","WinLIRC connection status","To reconnect to WinLIRC change this option and press Apply button",_l("Not connected")+"\t"+_l("Connected"),0);
		pOptionsCallback->AddTip("tip2","Other options",0);
		pOptionsCallback->AddStringOption("Wav","Play sound on button press","Leave empty to disable","Sound\\remote.wav",1);
		pOptionsCallback->AddBoolOption("KeyOSD","Show pressed keys in OSD","",TRUE);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		//if(iAction==OM_OPTIONS_SET)
		{
			UnLoadConfig(0);
			UnLoadConfig(1);
			if(hSocketThread){
				closesocket(currentWLSocket);
				while(hSocketThread!=0){
					Sleep(20);
				}
			}
		}
		if(!plgOptions.bNoAver){
			pOptionsCallback->GetStringOption("IniFile",plgOptions.szIniFilePath,sizeof(plgOptions.szIniFilePath));
		}
		pOptionsCallback->GetStringOption("WinLIRCip",plgOptions.szWinLircIP,sizeof(plgOptions.szWinLircIP));
		pOptionsCallback->GetStringOption("Wav",plgOptions.szWav,sizeof(plgOptions.szWav));
		plgOptions.dwWinLircPort=pOptionsCallback->GetNumberOption("WinLIRCport");
		//lWLConnectionStatus=pOptionsCallback->GetListOption("wlStatus");
		plgOptions.bDebugMode=pOptionsCallback->GetBoolOption("KeyOSD");
		//if(iAction==OM_OPTIONS_SET)
		{
			BOOL bAnyEnabled=0;
			if(!plgOptions.bNoAver){
				LoadConfig(0);
				bAnyEnabled++;
			}
			if(plgOptions.szWinLircIP[0]!=0 && plgOptions.dwWinLircPort!=0){
				LoadConfig(1);
				bAnyEnabled++;
				DWORD dwHookerThread=0;
				hSocketThread=::CreateThread(NULL, 0, GlobalHooker_WinLIRC, NULL, 0, &dwHookerThread);
			}
			if(bAnyEnabled){
				// Регистрируем общие режимы
				char szClass2[16]={0};
				strcpy(szClass2,_l("Remote modes"));
				// Режимы
				strcat(szClass2,"\t@LONGPLAY");
				WKGetPluginContainer()->RegisterEvent("IRM0","Normal mode",szClass2);
				WKGetPluginContainer()->RegisterEvent("IRM1","Mode #1",szClass2);
				WKGetPluginContainer()->RegisterEvent("IRM2","Mode #2",szClass2);
				WKGetPluginContainer()->RegisterEvent("IRM3","Mode #3",szClass2);
				WKGetPluginContainer()->RegisterEvent("IRM4","Mode #4",szClass2);
			}
		}
	}
	if(iAction==OM_OPTIONS_GET){
		if(!plgOptions.bNoAver){
			pOptionsCallback->SetStringOption("IniFile",plgOptions.szIniFilePath);
		}
		pOptionsCallback->SetStringOption("WinLIRCip",plgOptions.szWinLircIP);
		pOptionsCallback->SetStringOption("Wav",plgOptions.szWav);
		pOptionsCallback->SetNumberOption("WinLIRCport",plgOptions.dwWinLircPort);
		pOptionsCallback->SetListOption("wlStatus",lWLConnectionStatus);
		pOptionsCallback->SetBoolOption("KeyOSD",plgOptions.bDebugMode);
	}
	return 1;
}

int WINAPI RenderScript(const char* szParameters_in, char* szResult_out, size_t dwResultSize_in)
{
	if(szResult_out){
		*szResult_out=0;
	}
	int iFrom=0;
	CString sBody=szParameters_in,sRes;
	CString sCommand=sBody.SpanExcluding(" \t\r\n");
	if(sCommand.CompareNoCase("Emulate")==0){
		CString sFile=sBody.Mid(strlen(sCommand));
		sFile.TrimLeft();
		sFile.TrimRight();
		HandleButton(sFile,1);
	}
	return 0;
}