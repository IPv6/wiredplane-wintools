// WP_RClickcp.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "resource.h"
#include "../WKPlugin.h"
#include "..\..\SmartWires\Atl\filemap.h"
#include <stdio.h>
#pragma data_seg("Shared")
BOOL stopped=TRUE;
HINSTANCE g_hinstDll=0;
HHOOK g_hhook1 = NULL;
HHOOK g_hhook2 = NULL;
HWND g_MainWnd=0;
long g_ActiveCounter=0;
BYTE g_lItemsCount=0;
long g_dwLastSelectedItem=-1;
HMENU g_hLastSelectedMenu=0;
BOOL g_MenuActivated=0;
long g_ItemsIn=10;
char g_szTweakMenuItems[1024]={0};
long g_lIteration=0;
HMENU g_hMyAttachedMenu=0;
BOOL g_InContextMenu=0;
BOOL g_ReplacePaste=0;
BOOL g_FastIns=0;
DWORD g_FastInsCode=0;
char g_ClipitemName[32]="Clipboard";
char szListOfExclisions[1024]="ifrun60.exe";
long g_Patcher=0;
char szClipHost[128]={0};
//DWORD g_iCurrentItemID=0;
long aCurCharDebug=0;
long aCurCharDebugLast=1;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")
#define FLOG2(x,z1,z2)		{FILE* f=fopen("c:\\wkp_rclick.debug","a+");fprintf(f,x,z1,z2);fprintf(f,"\n");fclose(f);};
WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

char szTempBuffer[1024]={0};
char* _l(const char* szText)
{
	WKGetPluginContainer()->GetTranslation(szText,szTempBuffer,sizeof(szTempBuffer));
	return szTempBuffer;
}

//----- Hook code -------
void LowerString(char* s)
{
	if(!s){
		return;
	}
	while(s[0]){
		if(s[0]>='A' && s[0]<='Z'){
			s[0]+='a'-'A';
		}
		s++;
	}
}

int SCmp(char* s1, char* s2, char* szSkip=0)
{
	int iRes=0;
	while(s1[0] && s2[0]){
		if(szSkip){
			char* sTmp=szSkip;
			while(*sTmp){
				if(s1[0]==sTmp[0]){
					s1++;
				}
				if(s2[0]==sTmp[0]){
					s2++;
				}
				sTmp++;
			}
		}
		if(s1[0]==s2[0]){
			iRes++;
		}else{
			break;
		}
		s1++;
		s2++;
	}
	return iRes;
}

BOOL isCharIn(char c, char* sz)
{
	if(c==0){
		return FALSE;
	}
	while(sz[0]){
		if(sz[0]==c){
			return TRUE;
		}
		sz++;
	}
	return FALSE;
}

int GetSLen(char* s1, char* szSkip=0)
{
	int iLen1=0,iLenTmp=0;
	while(s1[iLenTmp]!=0){
		if(!szSkip || !isCharIn(s1[iLenTmp],szSkip)){
			iLen1++;
		}
		iLenTmp++;
	}
	return iLen1;
}

void DropSkips(char* s1, char* szSkip)
{
	while(s1[0]){
		if(isCharIn(s1[0],szSkip)){
			char* s2=s1;
			do{
				s2[0]=s2[1];
				s2++;
			}while(s2[0]);
		}else{
			s1++;
		}
	}
}

int SCmpInList(char* s1, char* s2, char* szSkip, char* szDelims)
{
	DropSkips(s1,szSkip);
	DropSkips(s2,szSkip);
	int iLen1=GetSLen(s1);
	if(iLen1==0){
		return FALSE;
	}
	while(s2[0]){
		int iTest=SCmp(s2,s1);
		if(iTest==iLen1){
			return TRUE;
		}
		while(s2[0] && !isCharIn(s2[0],szDelims)){
			s2++;
		}
		if(s2[0]==0){
			return FALSE;
		}
		s2++;
	}
	return FALSE;
}

void SCpy(char* s1, const char* s2, long lMaxLen)
{
	while(s2[0] && lMaxLen>1){
		s1[1]=0;
		s1[0]=s2[0];
		s1++;
		s2++;
		lMaxLen--;
	}
	return;
}

void SRev(char* s)
{
	int iLen=GetSLen(s);
	for(int i=0;i<iLen/2;i++){
		char t=s[i];
		s[i]=s[iLen-i-1];
		s[iLen-i-1]=t;
	}
}

void SI2A(int i,char* s)
{
	if(i<0){
		i=-i;
		s[0]='-';
		s++;
	}
	char* sOrig=s;
#ifdef _DEBUG
	int iOrig=i;
#endif
	if(i==0){
		s[0]='0';
	}else{
		int iD=10,iC=0;
		while(i>0 && iC<30){
			int iOstatok=i%iD;
			s[0]='0'+iOstatok;
			s++;
			i=(i-iOstatok)/iD;
			iC++;
		}
	}
	SRev(sOrig);
#ifdef _DEBUG
	char szTest[100]={0};
	sprintf(szTest,"%i",iOrig);
	if(SCmp(szTest,sOrig)==0){
		MessageBox(0,sOrig,szTest,0);
	}
#endif
	return;
}

char* SFindLastC(char* s,char c)
{
	int iLen=GetSLen(s);
	while(iLen>=0){
		if(s[iLen]==c){
			return s+(iLen+1);
		}
		iLen--;
	}
	return 0;
}

#define UNIQUE_BASE	0xFA00
#define UNIQUE_DATA	0xABCDEF66
#define FLOG3(x,z1,z2,z3)	{FILE* f=fopen("c:\\rc_debug.txt","a+");fprintf(f,x,z1,z2,z3);fprintf(f,"\n");fclose(f);};
DWORD WINAPI SendPaste(LPVOID pData)
{
	g_dwLastSelectedItem=-1;
	long dwCommand=(long)pData;
	if(dwCommand>0 && long(dwCommand-1)<g_lItemsCount && aCurCharDebug!=aCurCharDebugLast){
		aCurCharDebugLast=aCurCharDebug;
		// Наше!!!
		//FLOG3("testing %c,%i=%i",aCurCharDebug,long(dwCommand-1),g_lItemsCount);
		::PostMessage(g_MainWnd,WM_PASTE,g_FastInsCode,MAKELONG(dwCommand-1,0));
		g_hLastSelectedMenu=0;
		g_MenuActivated=0;
		g_FastInsCode=0;
	}
	return 0;
}

//#include <Psapi.h>
BOOL PatchMenu(HMENU hMenu, int bSource)
{
	BOOL bRes=FALSE,bPathced=0;
	DWORD dwCount=::GetMenuItemCount(hMenu),bAlready=0;
	for(DWORD iPos=0;iPos<dwCount;iPos++){
		char szText[128]={0};
		char g_szItemText[32]={0};
		MENUITEMINFO mi,mi2;
		memset(&mi,0,sizeof(mi));
		mi.cbSize=sizeof(mi);
		mi.fMask=0x00000040|MIIM_ID|MIIM_STATE|MIIM_DATA|0x00000100;//MIIM_STRING|MIIM_FTYPE
		mi.fType=MFT_STRING;
		mi.dwTypeData=szText;
		mi.cch=sizeof(szText)-1;
		::GetMenuItemInfo(hMenu,iPos,TRUE,&mi);
		{// Отрезаем до табуляции...
			int iC=0;
			while(szText[iC]){
				if(szText[iC]=='\t'){
					szText[iC]=0;
					break;
				}
				iC++;
			}
		}
		char szItemTextOriginalBackup[128]={0};
		SCpy(szItemTextOriginalBackup,szText,sizeof(szItemTextOriginalBackup));
		LowerString(szText);
		LowerString(g_szTweakMenuItems);
		BOOL bOk=SCmpInList(szText,g_szTweakMenuItems,"& \t.:",";,");
		int iOkType=0;
		{// Удаляем себя если нашли!
			int iTest=SCmp(szClipHost,szItemTextOriginalBackup);
			if(iTest==GetSLen(szClipHost)){
				if(g_ReplacePaste){
					// Если мы перезатираем и нашли сами себя, то значит реальный пасте удален!!!
					//g_FastInsCode=mi.wID;
					bOk=TRUE;
					iOkType=1;
				}else{
					DeleteMenu(hMenu,iPos,MF_BYPOSITION);
					dwCount--;
					continue;
				}
			}
		}
		if(!bOk){
			char szNumber[32]={0};
			SI2A(mi.wID,szNumber);
			bOk=SCmpInList(szNumber,g_szTweakMenuItems,"& \t.:",";,");
			iOkType=1;
		}
		if(!bOk){
			char szNumber[32]={0};
			szNumber[0]='@';
			SI2A(mi.dwItemData,szNumber+1);
			bOk=SCmpInList(szNumber,g_szTweakMenuItems,"& \t.:",";,");
			iOkType=2;
		}
		if(!bOk){
			char szTemp[sizeof(g_ClipitemName)]={0};
			SCpy(szTemp,g_ClipitemName,sizeof(szTemp));
			LowerString(szTemp);
			DropSkips(szTemp,"& \t.:");
			bOk=(SCmp(szText,szTemp,"& \t.:")==GetSLen(szTemp));
			iOkType=3;
		}
		BOOL bToContextLast=FALSE;
		if(dwCount==iPos+1 && !bPathced && !bOk && g_InContextMenu){
			bToContextLast=TRUE;
			bOk=TRUE;
		}
		DWORD g_iCurrentItemID=0xFFFF;//mi.wID;
		char szListOfExclisionsTmp[sizeof(szListOfExclisions)]={0};
		SCpy(szListOfExclisionsTmp,szListOfExclisions,sizeof(szListOfExclisionsTmp));
		char szThisExe[MAX_PATH]={0};
		GetModuleFileName(NULL,szThisExe,sizeof(szThisExe));
		LowerString(szThisExe);
		char* szLast=SFindLastC(szThisExe,'\\');
		if(SCmpInList(szLast,szListOfExclisionsTmp,"& \t.:",";,")){
			g_iCurrentItemID=0;
		}
		if(bOk && bAlready==0){// && (bToContextLast || (mi.fType & MF_POPUP)==0)
			bPathced=TRUE;
			int iOrigPos=iPos;
			char szItemTextOriginal[128]={0};
			if(bToContextLast){
				SCpy(szItemTextOriginalBackup,g_ClipitemName,sizeof(szItemTextOriginalBackup));
			}else{
				g_FastInsCode=mi.wID;
				if(g_ReplacePaste){
					DeleteMenu(hMenu,iPos,MF_BYPOSITION);
				}
			}
			SCpy(szItemTextOriginal,szItemTextOriginalBackup,sizeof(szItemTextOriginal));
			if(g_hMyAttachedMenu){
				::DestroyMenu(g_hMyAttachedMenu);
			}
			g_hMyAttachedMenu=CreatePopupMenu();
			// Втыкаем оригинальный пункт
			SCpy(szText,"...",sizeof(szText));
			aCurCharDebug++;
			InsertMenuItem(g_hMyAttachedMenu,::GetMenuItemCount(g_hMyAttachedMenu),TRUE,&mi);
			// Паста! втыкаем наше все сюда
			{
				bRes=TRUE;
				g_lItemsCount=0;
				g_MenuActivated=1;
				g_lIteration++;
				CFileMap fMap;
				fMap.Create("WK_EXTERN_STUFF_CLIPHISTORY",DWORD(MAX_PATH));
				UINT iWM_EXTERN=RegisterWindowMessage("WK_EXTERN_STUFF");//WM_USER+14;//EXTERNSTUFF;//
				while(g_lItemsCount<g_ItemsIn){
					DWORD dwResult=0;
					DWORD dwSendRes=::SendMessageTimeout(g_MainWnd,iWM_EXTERN,(WPARAM)(0x40000000),(LPARAM)(g_lItemsCount),SMTO_ABORTIFHUNG,500,&dwResult);
					if(dwResult!=1 || dwSendRes==0){
						break;
					}
					CFileMap fMap2("WK_EXTERN_STUFF_CLIPHISTORY",MAX_PATH);
					char* szTextItem=(char*)fMap2.Base();
					if(szTextItem[0]!=0){
						if(GetSLen(szTextItem)>30){
							szTextItem[25]='.';
							szTextItem[26]='.';
							szTextItem[27]='.';
							szTextItem[28]=0;
						}
						memset(&mi2,0,sizeof(mi2));
						mi2.cbSize=sizeof(mi2);
						mi2.fMask=MIIM_ID|MIIM_STATE|MIIM_DATA|MIIM_TYPE;
						mi2.fType=MFT_STRING;
						mi2.fState=MFS_ENABLED;
						mi2.dwItemData=UNIQUE_DATA+g_lItemsCount;
						mi2.wID=g_iCurrentItemID;
						mi2.dwTypeData=szTextItem;
						mi2.cch=GetSLen((char*)mi2.dwTypeData)+1;
						bRes=InsertMenuItem(g_hMyAttachedMenu,::GetMenuItemCount(g_hMyAttachedMenu),TRUE,&mi2);//iPos
						g_lItemsCount++;
					}else{
						break;
					}
				}
			}
			if(g_lItemsCount>0){
				InsertMenu(hMenu,iOrigPos+((bToContextLast || iOkType==3)?1:(g_ReplacePaste?0:1)),MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT)g_hMyAttachedMenu,szClipHost);//szItemTextOriginal[0]==0?">>>>":szItemTextOriginal
				bAlready=1;
				iPos++;
				dwCount++;
			}
			//break;
		}
	}
	return bRes;
}

BOOL IsMouseOnMenu(HMENU h, POINT* pt=0, int* iOut=0)
{
	if(iOut){
		*iOut=-1;
	}
	if(!h || !IsMenu(h)){
		return FALSE;
	}
	POINT pt2;
	GetCursorPos(&pt2);
	POINT pt1;
	pt1.x=pt2.x;
	pt1.y=pt2.y;
	if(pt){
		pt1.x=pt->x;
		pt1.y=pt->y;
	}else{
		HWND hWin=GetForegroundWindow();
		if(hWin){
			::ScreenToClient(hWin,&pt1);
		}
	}
	int iPos=-1;
	// Нажат ли пункт меню?
	iPos=MenuItemFromPoint(NULL,h,pt2);
	if(iPos>=0){
		if(iOut){
			*iOut=iPos;
		}
		return 2;
	}else{
		iPos=MenuItemFromPoint(NULL,h,pt1);
		if(iPos>=0){
			if(iOut){
				*iOut=iPos;
			}
			return 1;
		}
	}
	return 0;
}

void EnableMenu(HMENU hMenu)
{
	if(!hMenu || !IsMenu(hMenu)){
		return;
	}
	DWORD dwCount=::GetMenuItemCount(hMenu);
	for(DWORD i=0;i<dwCount;i++){
		::EnableMenuItem(hMenu,i,MF_BYPOSITION);
	}
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning()){
		::InterlockedIncrement(&g_ActiveCounter);
		if (nCode>=0 && lParam!=0 && !stopped){
			CWPSTRUCT* msg=((CWPSTRUCT*)lParam);
			if(g_MainWnd){
				if(msg->message==WM_ENTERMENULOOP || msg->message==WM_CONTEXTMENU){
					g_dwLastSelectedItem=-1;
					g_hLastSelectedMenu=0;
					g_MenuActivated=1;
				}
			}
		}
		::InterlockedDecrement(&g_ActiveCounter);
	}
	return(CallNextHookEx(g_hhook1, nCode, wParam, lParam));
}

class SimpleTracker
{
public:
	long* bValue;
	SimpleTracker(long& b){
		bValue=&b;
		InterlockedIncrement(bValue);
	};
	~SimpleTracker(){
		InterlockedDecrement(bValue);
	};
};

LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning()){
		::InterlockedIncrement(&g_ActiveCounter);
		if (nCode>=0 && lParam!=0 && !stopped){
			CWPRETSTRUCT* msg=((CWPRETSTRUCT*)lParam);
			if(g_MainWnd){
				if(msg->message==WM_EXITMENULOOP || msg->message==WM_CONTEXTMENU){
					if(g_MenuActivated && g_dwLastSelectedItem>0){
						DWORD dwID=0;
						//::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)(msg->hwnd)));
						HANDLE h=::CreateThread(0,0,SendPaste,LPVOID(g_dwLastSelectedItem),0,&dwID);
						CloseHandle(h);
					}
				}
				if(msg->message==WM_INITMENUPOPUP && g_MenuActivated){
					HMENU h=(HMENU)msg->wParam;
					if(h==g_hMyAttachedMenu){
						EnableMenu(g_hMyAttachedMenu);
					}
				}
				if(msg->message==WM_MENUSELECT){
					int iPos=LOWORD(msg->wParam);
					DWORD dwType=HIWORD(msg->wParam);
					if(!(msg->wParam==0xFFFF0000 && msg->lParam==0)){
						if(g_hLastSelectedMenu==0){
							if(g_Patcher==0){
								SimpleTracker lc(g_Patcher);
								// Ищем все активные менюшки?
								int iCount=0;
								for(int i=0;i<0xFFFF;i++){
									HMENU h=(HMENU)i;
									if(IsMouseOnMenu(h)){
										if(PatchMenu(h,0)){
											g_hLastSelectedMenu=h;
											break;
										}
									}
								}
							}
						}
						if((dwType & MF_POPUP)==0 && g_hMyAttachedMenu){
							iPos=-1;
							int iRes=IsMouseOnMenu(g_hMyAttachedMenu,0,&iPos);
							g_dwLastSelectedItem=iPos;
						}
					}
				}
			}
		}
		::InterlockedDecrement(&g_ActiveCounter);
	}
	return(CallNextHookEx(g_hhook2, nCode, wParam, lParam));
}


/*BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hinstDll=hModule;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}*/

//--------------------------------------------
// Functions definitions. 
// All required functions MUST be exported (throught .DEF file)
//--------------------------------------------
/*
	Function must fill szTitle with Plugin name
	and some other options. Result is ignored
	-------------------------------------------------
	This is not required function
*/
typedef int (WINAPI *FPLUGINCOMDSCFUNCTION)(WKPluginDsc*);
int	WINAPI GetPluginDsc(WKPluginDsc* stuff)
{
	stuff->bResidentPlugin=TRUE;
	stuff->bLoadPluginByDefault=TRUE;
	strcpy(stuff->szTitle,"Right-click Paste Enhancer");
	strcpy(stuff->szDesk,"This plugin will add history of clipboard to all popup menus with Paste item");
	g_hinstDll=WKGetPluginContainer()->GetPluginDllInstance();
	stuff->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BITMAP));
	if(stuff->cbsize>=sizeof(WKPluginDsc)){
		strcpy(stuff->szAuthors,"WiredPlane.com");
	}
	return 1;
}

/*
	This function will be called when for resident plugin
	when it is loaded, but in secondary thread. 
	(WKCallbackInterface*) pointer can be saved for later use
	This method may involve long-duration operations
	Result must be 1 if everithing is OK
	-------------------------------------------------
	This is not required function
*/
HANDLE hHookerThread=0;
HANDLE hThreadStopped=0;
HANDLE hThreadWorking=0;
char szHookLibPath[MAX_PATH]="";
//typedef WINUSERAPI long  (WINAPI  *BroadcastSystemMessageA)(DWORD, LPDWORD, UINT, WPARAM, LPARAM);
DWORD WINAPI RCLCP_MainThread(LPVOID)
{
	// Loading copy of this DLL, because
	// Windows will unload this module on UnhookWindowsHookEx anyway!!!!
	HINSTANCE hCopy=LoadLibrary(szHookLibPath);
	hThreadStopped=::CreateEvent(NULL,FALSE,FALSE,NULL);
	g_hhook1 = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, hCopy, 0);
	g_hhook2 = SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndRetProc, hCopy, 0);
	if(g_hhook1==NULL || g_hhook2==NULL){
		char szErr[100]={0};
		sprintf(szErr,"Failed to start menus enhancer, error 0x%08X",GetLastError());
		MessageBox(0, szErr, "Enhancer Message", 0);
	}
	while(!stopped){
		WaitForSingleObject(hThreadWorking,1000);
	}
	UnhookWindowsHookEx(g_hhook1);
	UnhookWindowsHookEx(g_hhook2);
	DWORD dwTick=GetTickCount();
	while(g_ActiveCounter>0 && GetTickCount()-dwTick<5000){
		Sleep(100);
	}
	g_hhook1=0;
	g_hhook2=0;
	SetEvent(hThreadStopped);
	return 0;
}

WKCallbackInterface* pInter=0;
typedef int	(WINAPI *FPLUGININITFUNCTION)(WKCallbackInterface*);
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	if(init==0){
		init=WKGetPluginContainer();
	}
	pInter=init;
	if(init){
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
		szHookLibPath[0]=0;
		init->GetWKPluginsDirectory(szHookLibPath,MAX_PATH);
		strcat(szHookLibPath,"WP_RClickcp.wkp");
		g_MainWnd=init->GetWKMainWindow();
	}
	DWORD dwID=0;
	stopped = false;
	hThreadWorking=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(0,0,RCLCP_MainThread,0,0,&dwID);
#ifdef _DEBUG
	char s[100]={0};
	sprintf(s,"%s:%i %08x Debug thread point",__FILE__,__LINE__,dwID);
	init->ShowPopup(s,"Debug thread",0);
#endif
	return 1;
}


/*
	This function will be called when plugin is unloaded
	MUST NOT invlolve any oprations with long duration
	Result must be 1 if everithing is OK
	-------------------------------------------------
	This is not required function
*/
typedef int	(WINAPI *FPLUGINSTOPFUNCTION)();
int	WINAPI WKPluginStop()
{
	stopped = true;
	if(hHookerThread){
		if(hThreadStopped){
			if(!pInter->getOption(WKOPT_ISSHUTDOWN)){
				SetEvent(hThreadWorking);
				WaitForSingleObject(hThreadStopped,4000);
			}
			TerminateThread(hHookerThread,66);
			//SuspendThread(hHookerThread);
			CloseHandle(hThreadWorking);
			CloseHandle(hThreadStopped);
		}
		CloseHandle(hHookerThread);
		hHookerThread=0;
	}
	return 1;
}

/*
	WireKeys detect plugins by presence of this function
	At start up WireKeys will call this function 
	with iPluginFunction from 0 to '...' while WKGetPluginFunctionDescription(..)!=0
	Plugin must fill WKPluginFunctionDsc for every supported function
	Result must be:
	-------------------------------------------------
	0	Function with this iPluginFunction is not supported by plugin
	1	Function with this iPluginFunction supported by plugin
	-------------------------------------------------
	This is required function
*/
int	WINAPI WKGetPluginFunctionCommonDesc(DWORD iPluginFunction, WKPluginFunctionDsc* stuff)
{
	return 0;
}

int	WINAPI WKCallPluginFunction(DWORD iPluginFunction, WKPluginFunctionStuff* stuff)
{
	return 0;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddNumberOption("items","How many history items add to menus","",10,1,100);
		pOptionsCallback->AddStringOption("pastes","Tweak menus that contain following items","Delimit items with ';' character, for example 'Paste;Copy;Ctrl-V'. You can also type ID`s of the items",_l("Paste"));
		pOptionsCallback->AddStringOption("aApps","Applications that must be tweaked in the alternative way","Some applications must be handled in special ways, if you have problems with 'Paste' menu, type application name here. Delimit items with ';' character","ifrun60.exe");
		pOptionsCallback->AddBoolOption("replace","Replace Paste item instead of adding submenu","",0,0);
		pOptionsCallback->AddBoolOption("others","Tweak *all* menus (Ignoring 'Paste' item presence)","Tweak all menus even that can`t be tweaked in normal way",0,0);
		//pOptionsCallback->AddBoolOption("fast","Use fast pastes","Faster, but can be incompatible with some applications",1,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		/*
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,0,0);
			DWORD dwRes=WaitForSingleObject(hHookerEventDone,500);
		}
		*/
		g_ItemsIn=pOptionsCallback->GetNumberOption("items");
		pOptionsCallback->GetStringOption("pastes",g_szTweakMenuItems,sizeof(g_szTweakMenuItems));
		pOptionsCallback->GetStringOption("aApps",szListOfExclisions,sizeof(szListOfExclisions));
		g_InContextMenu=pOptionsCallback->GetBoolOption("others");
		g_ReplacePaste=pOptionsCallback->GetBoolOption("replace");
		//g_FastIns=pOptionsCallback->GetBoolOption("fast");
		/*
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,1,0);
			DWORD dwRes=WaitForSingleObject(hHookerEventDone,500);
		}
		*/
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetNumberOption("items",g_ItemsIn);
		pOptionsCallback->SetStringOption("pastes",g_szTweakMenuItems);
		pOptionsCallback->SetStringOption("aApps",szListOfExclisions);
		pOptionsCallback->SetBoolOption("others",g_InContextMenu);
		pOptionsCallback->SetBoolOption("replace",g_ReplacePaste);
		//pOptionsCallback->SetBoolOption("fast",g_FastIns);
	}
	return 1;
}

/*
	This function will be called when plugin is loaded
	MUST NOT invlolve any operations with long duration
	(use WKPluginInit for such initializations)
	Result must be 1 if everithing is OK
	-------------------------------------------------
	This is not required function
*/
typedef int	(WINAPI *FPLUGINSTARTFUNCTION)();
int	WINAPI WKPluginStart()
{
	SCpy(g_szTweakMenuItems,"paste",sizeof(g_szTweakMenuItems));
	SCpy(g_ClipitemName,_l("Clipboard"),sizeof(g_ClipitemName));
	SCpy(szClipHost,_l("Clipboard history"),sizeof(szClipHost));
	return 1;
}

int	WINAPI WKDesktopSwitching(BOOL bPaused)
{
	if(bPaused){
		WKPluginStop();
	}else{
		WKPluginInit(0);
	}
	return 1;
}
