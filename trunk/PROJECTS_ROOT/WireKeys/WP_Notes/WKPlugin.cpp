#include "stdafx.h"
#include <atlbase.h>
#include "resource.h"
#include "..\wkplugin.h"
#include "..\..\SmartWires\Atl\filemap.h"
#define EXTERNS_BASE		29000
HWND hNotesWindow=0;
char szNotesFolder[MAX_PATH]={0};

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

const char* _l(const char* szText)
{
	static char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

CString _pl(const char* szText)
{
	return _l(szText);
}

bool stopped=0;
HANDLE hHookerThread=0;
HANDLE hThreadWorking=0;
HANDLE hThreadStopped=0;
DWORD WINAPI MainThread(LPVOID)
{
	hThreadStopped=::CreateEvent(NULL,FALSE,FALSE,NULL);
	DWORD dwLastSetTime=GetTickCount();
	while(!stopped){
		WaitForSingleObject(hThreadWorking,1000);
		/*if(long(GetTickCount()-dwLastSetTime)>g_bShowMS){
			dwLastSetTime=GetTickCount();
			ShowUsage();
		}*/
	}
	SetEvent(hThreadStopped);
	return 0;
}

BOOL isFileExist(const char* szFile)
{
	if(szFile==NULL || szFile[0]==0){
		return FALSE;
	}
	if(int(GetFileAttributes(szFile))==-1){
		return FALSE;
	}
	return TRUE;
}

BOOL bInitOK=0;
typedef HRESULT (WINAPI* _WNStartEmbedded)();
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	bInitOK=0;
	if(init){
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<364){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.6.4 and higher only","Plugin error");
			return 0;
		}
	}
	DWORD dwID=0;
	stopped = false;
	WKGetPluginContainer()->GetPreferencesFolder(szNotesFolder,sizeof(szNotesFolder));
	strcat(szNotesFolder,"notes\\");
	::CreateDirectory(szNotesFolder,NULL);
	hThreadWorking=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=0;//::CreateThread(0,0,MainThread,0,0,&dwID);
	char szDLLPath[MAX_PATH]={0},szDir[256]={0},szDisk[256]={0};
	GetModuleFileName(WKGetPluginContainer()->GetPluginDllInstance(),szDLLPath,sizeof(szDLLPath));
	_splitpath(szDLLPath,szDisk,szDir,0,0);
	CString sPluginPath=CString(szDisk)+CString(szDir)+"Notes\\";
	CString sWNPath=sPluginPath+"WireKeys_notes.exe";
	if(!isFileExist(sWNPath)){
		init->ShowAlert("Failed to find notes server application. Please reinstall notes plugin","Plugin error");
		return 0;
	}
	CString sRunLine;
	CString sLangName;
	sLangName.Format("LangName%i",WKGetPluginContainer()->triggerTweak(2));
	char szLangNameTr[256]={0};
	WKGetPluginContainer()->GetBaseTranslation(sLangName,szLangNameTr,sizeof(szLangNameTr));
	sRunLine.Format("-ul=\"%s\" -embed=yes -langByName=%s",szNotesFolder,szLangNameTr);

	CFileMap fMapHolder;
	fMapHolder.Create("WK_WN_WND",256);
	::ShellExecute(NULL,"open",sWNPath,sRunLine,sPluginPath,SW_HIDE);

	CString sHid;
	HWND hHidDir=0;
	CFileMap fMap;
	DWORD dwTime=GetTickCount();
	do{
		fMap.Open("WK_WN_WND",false,256);
		sHid=(char*)fMap.Base();
		if(sHid==""){
			Sleep(10);
			if(GetTickCount()-dwTime>25000){
				HWND hProgman=::FindWindow("Progman",NULL);
				if(hProgman){
					hHidDir=(HWND)::GetProp(hProgman,"WKP_NOTW");
					if(hHidDir){
						break;
					}
				}
				//init->ShowAlert("Failed to run notes server application. Please reinstall notes plugin","Plugin error");
				return 0;
			}
		}
	}while(sHid=="");
	hNotesWindow=(HWND)atol(sHid);
	if(hHidDir){
		hNotesWindow=hHidDir;
	}
	bInitOK=1;
	return 1;
}

int	WINAPI WKPluginStart()
{
	stopped=0;
	return 1;
}

int	WINAPI WKPluginStop()
{
	stopped=1;
	HWND hWkn=hNotesWindow;
	if(hWkn && IsWindow(hNotesWindow)){
		PostMessage(hWkn,WM_COMMAND,WM_USER+EXTERNS_BASE+0,0);
	}
	if(hHookerThread){
		if(hThreadStopped){
			if(!WKGetPluginContainer()->getOption(WKOPT_ISSHUTDOWN)){
				SetEvent(hThreadWorking);
				WaitForSingleObject(hThreadStopped,1000);
			}
			DWORD dwTRes=0;
			if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
				//TerminateThread(hThread,66);
				//SuspendThread(hHookerThread);
			}
			CloseHandle(hThreadWorking);
			CloseHandle(hThreadStopped);
		}
		CloseHandle(hHookerThread);
		hHookerThread=0;
	}
	return 1;
}

HINSTANCE g_hinstDll=NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
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

CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString, int& iFrom)
{
	if(iFrom>=(int)strlen(szString)){
		iFrom=-1;
		return "";
	}
	CString sRes="";
	const char* szSearchFrom=szString+iFrom;
	char const * szFrom=strstr(szString+iFrom,szStart);
	if(!szFrom){
		iFrom=-1;
		return "";
	}
	iFrom=(szFrom-szString)+strlen(szStart);
	const char* szSearchFromAfterStart=szString+iFrom;
	const char* szBeginTmp=(szFrom+strlen(szStart));
	if(szEnd!=NULL){
		const char* szEndPos=strstr(szBeginTmp,szEnd);
		if(szEndPos){
			sRes=CString(szBeginTmp,szEndPos-szBeginTmp);
			iFrom+=strlen(sRes)+strlen(szEnd);
			const char* szSearchFromEnd=szString+iFrom;
		}else{
			iFrom=strlen(szString);
			sRes="";
		}
	}else{
		sRes=szBeginTmp;
		sRes.TrimRight();
		iFrom+=strlen(sRes);
	}
	return sRes;
}

CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString)
{
	int iFrom=0;
	return GetInstringPart(szStart, szEnd, szString, iFrom);
}

CString GetNoteListing(int& iCount)
{
	CString sListing;
	HWND hWkn=hNotesWindow;
	if(hWkn && IsWindow(hNotesWindow)){
		DWORD dwResults=0;
		CFileMap fMapHandleHolder;
		fMapHandleHolder.Create("WK_NOST",1024);
		{// Спрашиваем список...
			SendMessageTimeout(hWkn,WM_COMMAND,WM_USER+EXTERNS_BASE+2,0,SMTO_ABORTIFHUNG|SMTO_NORMAL, 2000, &dwResults);
			//SendMessage(hWkn,WM_COMMAND,WM_USER+EXTERNS_BASE+2,0);
			CFileMap fMap;
			fMap.Open("WK_NOST",false,1024);
			char* sz=(char*)fMap.Base();
			sListing=sz;
		}
	}
	iCount=atol(GetInstringPart("{ALL_#$?...","=*88#22#$ALL}",sListing));
	return sListing;
}


void ShowAllNotes(BOOL bShow=1)
{
	int iCount=0;
	HWND hWkn=hNotesWindow;
	CString sListing=GetNoteListing(iCount);
	for(int i=0;i<iCount;i++){
		PostMessage(hWkn,WM_COMMAND,WM_USER+EXTERNS_BASE+10+(i),bShow?0:1);
	}
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Create new note");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
	}
	if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Show all notes");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
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

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(!bInitOK){
		WKGetPluginContainer()->ShowAlert("Failed to run notes server application. Please reinstall notes plugin","Plugin error");
		return 0;
	}
	if(iPluginFunction==0){
		HWND hWkn=hNotesWindow;
		if(hWkn && IsWindow(hNotesWindow)){
			PostMessage(hWkn,WM_COMMAND,WM_USER+EXTERNS_BASE+1,0);
		}
		return 1;
	}
	if(iPluginFunction==1){
		ShowAllNotes();
		return 1;
	}
	return 0;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	strcpy(dsc->szDesk,"Sticky notes");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
	}
	if(iAction==OM_OPTIONS_GET){
	}
	return 1;
}

int iAddNew=0;
int iShowAll=0;
int iHideAll=0;
int iIdBeginning=0;
int WINAPI WKTrayMenuPopup(int iActionType, int& iItemId, HMENU hMenu)
{
	if(!bInitOK){
		return 0;
	}
	if(iActionType==TM_ADDCUSTOMITEM){
		iIdBeginning=iItemId;
		int iCount=0;
		CString sListing=GetNoteListing(iCount);
		if(sListing!=""){
			HMENU mn=CreatePopupMenu();
			int iPos=0;
			CString sText;
			for(int i=0;i<iCount;i++){
				CString sLK;
				sLK.Format("{Line(%i)_#$?...",i);
				sText=GetInstringPart(sLK,"=*88#22#$}",sListing);
				if(sText!=""){
					AppendMenu(mn,MF_STRING,iItemId,sText);
					iPos++;
				}
				iItemId++;//Увеличиваем на все, даже на пустые!
			}
			if(iPos>0){
				iAddNew=iItemId;
				iItemId++;
				AppendMenu(mn,MF_STRING,iAddNew,_pl("Add new..."));
				if(iPos>1){
					iShowAll=iItemId;
					iItemId++;
					AppendMenu(mn,MF_STRING,iShowAll,_pl("Show All..."));
					{
						iHideAll=iItemId;
						iItemId++;
						AppendMenu(mn,MF_STRING,iHideAll,_pl("Hide All..."));
					}
				}
				AppendMenu(hMenu,MF_POPUP|MF_STRING,UINT(mn),_pl("Notes"));
			}
		}
	}else{
		HWND hWkn=hNotesWindow;
		if(hWkn && IsWindow(hNotesWindow)){
			if(iItemId==iHideAll){
				ShowAllNotes(0);
			}else if(iItemId==iShowAll){
				ShowAllNotes();
			}else if(iItemId==iAddNew){
				WKCallPluginFunction(0,0);
			}else{
				PostMessage(hWkn,WM_COMMAND,WM_USER+EXTERNS_BASE+10+(iItemId-iIdBeginning),0);
			}
		}
	}
	return 0;
}