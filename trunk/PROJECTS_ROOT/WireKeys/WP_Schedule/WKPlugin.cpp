// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_Schedule.h"
#include "HookCode.h"

#include "..\..\SmartWires\BitmapUtils\BMPUtil.h"
#include "..\..\SmartWires\SystemUtils\SupportClasses.h"
#include "..\..\SmartWires\SystemUtils\RemindClasses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char szWavFilePath[MAX_PATH]={0};
char szDateFormat[MAX_PATH]={0};
WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

CString _l2(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

HINSTANCE hRemin=0;
COptions plgOptions;
HINSTANCE g_hinstDll=NULL;
HANDLE hHookerThread=NULL;
CRITICAL_SECTION csMainThread;
WKCallbackInterface* pInter=NULL;
DWORD WINAPI GlobalHooker_Gestures(LPVOID pData);
int	WINAPI WKPluginShowEventX(char szEvent[128],HWND pParent,COleDateTime dt,BOOL bAskMenu);
int WINAPI onAddEvent(char const* szEevnt)
{
	pInter->ShowAlert(szEevnt,"Test");
	return 0;
}

BOOL WINAPI ReminderActivate(CWPReminder& rem)
{
	CString szEvent=rem.szReserved;
	if(!pInter->StartQuickRunByName(szEvent,"")){
		rem.bActPopup=1;
	}
	if(strstr(rem.szText,"javascript:")==rem.szText){
		pInter->StartQuickRunByName(rem.szText,"");
		rem.bActPopup=0;
	}
	return FALSE;
}

int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	// Version...
	// Version...
	char szVer[32]="";
	init->GetWKVersion(szVer);
	DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
	if(dwBuild<348){
		init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
		return 0;
	}
	if(!init->getOption(WKOPT_EXTHKPROC)){
		init->ShowAlert("Sorry, you must enable extended hotkey processing\nin order to use this plugin. You can enable it\nin the preferences dialog","Scheduler");
		return 0;
	}
	pInter=init;
	
	if(!hRemin){
		init->ShowAlert("Sorry, something important is missing\nReminders plugin is not available","Scheduler");
		return 0;
	}else{
		/*__bmpX fp1=(__bmpX)GetProcAddress(hRemin,"_bmpX");
		if(fp1){
			(*fp1)()=&_bmp();
		}*/
		// Ждем пока WK полностью инициализируется
		while(!(init->getOption(WKOPT_WKINITIALIZED))){
			Sleep(100);
		}
		__lngX fp2=(__lngX)GetProcAddress(hRemin,"_lngX");
		if(fp2){
			(*fp2)()=(CUITranslationImpl*)init->getOptionEx(0,0);//CUITranslationImpl::getInstance();
		}
		_InitDLL fp0=(_InitDLL)GetProcAddress(hRemin,"InitDLL");
		if(fp0){
			char szPath[MAX_PATH]={0};
			pInter->GetPreferencesFolder(szPath,sizeof(szPath));
			CString sRemDir=szPath;
			sRemDir+="Schedule\\";
			//AfxMessageBox(sRemDir);
			CreateDirIfNotExist(sRemDir);
			int ihash=0;
			(*fp0)(ihash, sRemDir, ReminderActivate);
		}
	}
	return 1;
}
//WCRemind.cml
int	WINAPI WKPluginStart()
{
	char szDir[MAX_PATH]={0};
	WKGetPluginContainer()->GetWKPluginsDirectory(szDir,MAX_PATH);
	strcat(szDir,"WCRemind.cml");
	hRemin=LoadLibrary(szDir);
	::InitializeCriticalSection(&csMainThread);
	return 1;
}

BOOL bWasShow=0;
int	WINAPI WKPluginStop()
{
	if(hRemin){
		_StopDLL fp0=(_StopDLL)GetProcAddress(hRemin,"StopDLL");
		if(fp0){
			(*fp0)();
		}
		FreeLibrary(hRemin);
	}
	::DeleteCriticalSection(&csMainThread);
	if(bWasShow){
		WKGetPluginContainer()->PushMyOptions(0);
	}
	return 1;
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Add/Edit reminders");
	}
	if(iPluginFunction==1){
		strcpy(stuff->szItemName,"Show/Hide calendar");
	}
	stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_CAL));
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_HOTMENU|WKITEMPOSITION_MISC;
	stuff->dwDefaultHotkey=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	return 1;
}

#define DEF_ID "SCHEDULE_COMMON"
void CreateRemOnDate(HWND hParent,COleDateTime dt,BOOL bAsk)
{
	char szEvent[128]=DEF_ID;
	WKPluginShowEventX(szEvent,hParent,dt,bAsk);
}

#define ID_SIZE 30
#include <sys/timeb.h>
CString GenerateNewIdX(const char* szPrefix,const char* szPostfix)
{
	CString sRes;
	struct _timeb tstruct;
	_ftime(&tstruct);
	sRes.Format("%s%sI%02lxR%02x",szPrefix,szPostfix,WORD(rnd(0,0xFF)),(WORD)tstruct.millitm);
	if(strlen(sRes)>ID_SIZE){
		sRes=sRes.Left(ID_SIZE);
	}
	sRes.MakeUpper();
	return sRes;
};

long bDefViewCalendar=0;
class CalenHelper:public CCalendarCallback
{
public:
	BOOL CallEditDayNote(SYSTEMTIME Time,char szFG[64], char szBG[64]){return 0;};
	BOOL GetDayNote(SYSTEMTIME Time,char szDsc[2048]){return 0;};
	BOOL GetRemDefText(char* szText,int size){return 0;};
	BOOL GetRemDefWav(char* szSoundPath,int size)
	{
		strcpy(szSoundPath,szWavFilePath);
		return 1;
	};
	BOOL GetRemDefKey(char* szKey,int size)
	{
		strcpy(szKey,GenerateNewIdX("WKR_",0));
		return 1;
	};
	BOOL GetNotesFile(char* szOut,int size){return 0;};
	BOOL CallContextDayNote(SYSTEMTIME Time, HWND hCalendar)
	{
		CStringArray arr;
		arr.Add(_l("New reminder"));
		arr.Add(_l("Add date to clipboard"));
		int iSelection=SelectFromMenu(arr,0);
		if(iSelection<0){
			return 0;
		}
		if(iSelection==0){
			return -1;
		}
		CString s="";
		COleDateTime tm;//=COleDateTime(Time);
		tm.SetDate(Time.wYear,Time.wMonth,Time.wDay);
		if(strlen(szDateFormat)==0){
			s=DateFormat(tm,FALSE);
		}else{
			SYSTEMTIME EventTime;
			tm.GetAsSystemTime(EventTime);
			char szTmp[1020]={0};
			GetDateFormat(LOCALE_USER_DEFAULT,0,&EventTime,szDateFormat,szTmp,sizeof(szTmp));
			s=szTmp;
		}
		BOOL bThroughGlobal=0;
		USES_CONVERSION;
		SetClipboardText(A2W(s),bThroughGlobal,0);
		return 0;
	};
};

CalenHelper objCalenHelper;
long lDouble=0;
DWORD WINAPI ShowMainDialog(LPVOID p)
{
	if(lDouble){
		return 0;
	}
	lDouble++;
	_OpenCalendar fp=(_OpenCalendar)GetProcAddress(hRemin,"OpenCalendar");
	if(fp){
		(*fp)(&objCalenHelper,bDefViewCalendar);
	}
	lDouble--;
	//DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_CALENDAR_DIALOG),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),CalenDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	//hDialog=0;
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	if(iPluginFunction>1){
		return 0;
	}
	if(iPluginFunction==1){
		/*
		if(hDialog!=0){
			if(hDialog!=HWND(1)){
				HWND hTmp=hDialog;
				hDialog=0;
				EndDialog(hTmp,0);
			}
			return 1;
		}
		hDialog=HWND(1);*/
		FORK(ShowMainDialog,stuff?stuff->hMainWKWindow:0)
	}
	if(iPluginFunction==0){
		CRect rDesktopRECT;
		//::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
		rDesktopRECT.left=rDesktopRECT.top=-20;
		rDesktopRECT.right=rDesktopRECT.bottom=-10;
		HWND wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_DISABLED, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.Width(), rDesktopRECT.Height(), 0, 0, g_hinstDll, 0);
		CreateRemOnDate(wndShotPhase2,COleDateTime::GetCurrentTime(),1);
		DestroyWindow(wndShotPhase2); 
	}
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Scheduler");
	strcpy(dsc->szDesk,"Purpose: Schedule any action to be automatically activated at the specified time");
	strcpy(dsc->szAuthors,"WiredPlane.com, @2008");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

BOOL bAddTrayMenu=1;
BOOL bActSound=1;
BOOL bLoopSound=0;
int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddTip("tip1","To add/modify schedule to event/macro/etc expand action`s hotkey and click on 'Add schedule' link",0);
		pOptionsCallback->AddListOption("defView","Default tray calendar view","",_l2("Month")+"\t"+_l2("Week"),0);
		pOptionsCallback->AddBoolOption("AddTrayMenu","Add submenu with reminders into tray menu","",TRUE,0);
		// Дефаулт звук
		// Играть ли звук по умолчанию
		pOptionsCallback->AddBoolOption("Sound","Play sound on scheduled event","",TRUE,0);
		if(szWavFilePath[0]==0){
			WKGetPluginContainer()->GetWKPluginsDirectory(szWavFilePath,MAX_PATH);
			strcat(szWavFilePath,"onevent.wav");
		}
		pOptionsCallback->AddStringOption("Wav","Sound file","",szWavFilePath,1,0);
/*
				pDialog->Option_AddString(hDTF,_l("Time format"),&psk->sODSTimeFormat);
				pDialog->Option_AddString(hDTF,_l("Date format"),&psk->sODSDateFormat);

*/
		pOptionsCallback->AddStringOption("datef","Date format","Used for adding date to clipboard","",1,0);
		//pOptionsCallback->AddBinOption("wndpos");
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		pOptionsCallback->GetStringOption("Wav",szWavFilePath,sizeof(szWavFilePath));
		pOptionsCallback->GetStringOption("datef",szDateFormat,sizeof(szDateFormat));
		bActSound=pOptionsCallback->GetBoolOption("Sound");
		//pOptionsCallback->GetBinOption("wndpos",&wndRect,sizeof(wndRect));
		bDefViewCalendar=pOptionsCallback->GetListOption("defView");
		bAddTrayMenu=pOptionsCallback->GetBoolOption("AddTrayMenu");
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetStringOption("datef",szDateFormat);
		pOptionsCallback->SetStringOption("Wav",szWavFilePath);
		pOptionsCallback->SetBoolOption("Sound",bActSound);
		pOptionsCallback->SetBoolOption("AddTrayMenu",bAddTrayMenu);
		pOptionsCallback->SetListOption("defView",bDefViewCalendar);
		//pOptionsCallback->SetBinOption("wndpos",&wndRect,sizeof(wndRect));
	}
	return 1;
}

void AddMapItems(CStringArray& aKeys,CStringArray& aTitles, const char* szKey, const char* szDsc)
{
	for(int i=0;i<aKeys.GetSize();i++){
		if(aKeys[i]==szKey){
			return;
		}
	}
	aTitles.SetAtGrow(aKeys.Add(szKey),szDsc);
}

CString GetRemDsc(CWPReminder& rem,BOOL bAddDate=FALSE)
{
	CString sDsc=TrimMessage(rem.szText,30,1);
	if(sDsc==""){
		sDsc=_l2("No description");
	}
	COleDateTime tm(rem.EventTime);
	COleDateTime dtNow=COleDateTime::GetCurrentTime();
	if(bAddDate && (tm.GetDay()!=dtNow.GetDay() || tm.GetMonth()!=dtNow.GetMonth() || tm.GetYear()!=dtNow.GetYear())){
		return Format("%s %s %s",DateFormat(tm,TRUE),TimeFormat(tm),sDsc);
	}else{
		return Format("%s %s",TimeFormat(tm),sDsc);
	}
}

CString calcNextPrefixedId(const char* sRMKey)
{
	_IsReminderFamilyPresent fpPr=(_IsReminderFamilyPresent)GetProcAddress(hRemin,"IsReminderFamilyPresent");
	CString sSelectedKey=GetNextPrefixedId(sRMKey);
	while((fpPr && (*fpPr)(sSelectedKey,0))){
		sSelectedKey=GetNextPrefixedId(sSelectedKey);
	}
	return sSelectedKey;
}

typedef int (WINAPI *_WKPluginShowEvent)(char szEvent[128],HWND);
typedef int (WINAPI *_WKIHOTKEYS_GetHotkeyDscByID)(char const* szEventID,char* szOut,int iOut);
int	WINAPI WKPluginShowEventX(char szEvent[128],HWND pParent,COleDateTime dt,BOOL bAskMenu)
{
	static long l=0;
	if(l>0){
		return 0;
	}
	SimpleTracker lc(l);

	CString sEventName=_l2("Scheduled event");
	HINSTANCE hLLHookInst=GetModuleHandle("WP_KeyMaster.wkp");
	_WKIHOTKEYS_GetHotkeyDscByID fpHk=(_WKIHOTKEYS_GetHotkeyDscByID)GetProcAddress(hLLHookInst,"WKIHOTKEYS_GetHotkeyDscByID");
	BOOL bDef=(strcmp(szEvent,DEF_ID)==0);
	if(fpHk){
		char szTitle[256]={0};
		if((*fpHk)(szEvent,szTitle,sizeof(szTitle))){
			sEventName=szTitle;
		}else{
			sEventName=_l2("Attention! Time has come!");
		}
	}
	_GetReminder fp=(_GetReminder)GetProcAddress(hRemin,"GetReminder");
	_IsReminderFamilyPresent fpPr=(_IsReminderFamilyPresent)GetProcAddress(hRemin,"IsReminderFamilyPresent");
	CString sRMKey=szEvent;
	// Возможно нужен выбор?
	CStringArray aTitles;
	CStringArray aKeys;
	if(bDef){
		aTitles.SetAtGrow(0,_l2("Add new reminder"));
		aKeys.SetAtGrow(0,sRMKey);
	}else{
		aTitles.SetAtGrow(0,_l2("Add new schedule"));
		aKeys.SetAtGrow(0,sRMKey);
	}
	CString sSelectedKey="";
	if(bAskMenu){
		int iMaxIndex=0;
		if(fpPr && (*fpPr)(sRMKey,&iMaxIndex)){
			aKeys[0]="";
			for(int i=0;i<=iMaxIndex;i++){
				if(fp){
					CWPReminder rem;
					strcpy(rem.szKey,GetNextPrefixedId(sRMKey,i));
					if((*fp)(rem.szKey,rem)){
						AddMapItems(aKeys,aTitles, rem.szKey, GetRemDsc(rem,TRUE));
					}
				}
			}
		}
		if(aKeys.GetSize()==1){
			sRMKey=aKeys[0];
		}else{
			CMenu menu;
			menu.CreatePopupMenu();
			CPoint pt;
			GetCursorPos(&pt);
			for(int i=0;i<aKeys.GetSize();i++){
				AddMenuString(&menu,WM_USER+i,aTitles[i]);
			}
			AddMenuString(&menu,WM_USER+aKeys.GetSize()+1,_l2("Close menu"));
			int iNum=::TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, pParent, NULL);
			if(iNum==0 || iNum<WM_USER || iNum>=WM_USER+aKeys.GetSize()){
				return 0;
			}
			sSelectedKey=aKeys[iNum-WM_USER];
			if(sSelectedKey==""){
				sSelectedKey=calcNextPrefixedId(sRMKey);
			}
			sRMKey=sSelectedKey;
		}
	}
	BOOL bNewReminder=0;
	CWPReminder rem;
	memset(&rem,0,sizeof(CWPReminder));
	if(fp && sRMKey!=""){
		strcpy(rem.szKey,sRMKey);
		if(!(*fp)(rem.szKey,rem)){
			// Создаем новый!
			bNewReminder=1;
			//COleDateTime dt=COleDateTime::GetCurrentTime();
			dt=dt+COleDateTimeSpan(0,0,1,0);
			dt.GetAsSystemTime(rem.EventTime);
			rem.bActPopup=bDef;
			rem.bActSound=bActSound;
			rem.bLoopSound=bLoopSound;
			strcpy(rem.szText,sEventName);
			strcpy(rem.szSoundPath,szWavFilePath);
			if(!bDef){
				// Здесь именно оригинальный Id-Event!!!
				strcpy(rem.szReserved,szEvent);
			}
			_PutReminder fp2=(_PutReminder)GetProcAddress(hRemin,"PutReminder");
			if(fp2){
				(*fp2)(rem.szKey,rem);
			}
		}
	}
#ifdef _DEBUG
	WKGetPluginContainer()->ShowAlert(rem.szKey,"opened reminder");
#endif
	_CallModifyReminder fp1=(_CallModifyReminder)GetProcAddress(hRemin,"CallModifyReminder");
	if(fp1){
		int iRes=(*fp1)(rem.szKey,pParent,0);
		if(iRes==IDCANCEL && bNewReminder){
			_RemoveReminder fp3=(_RemoveReminder)GetProcAddress(hRemin,"RemoveReminder");
			if(fp3){
				(*fp3)(rem.szKey);
			}
		}
	}
	return 1;
}

int	WINAPI WKPluginShowEvent(char szEvent[128],HWND pParent)
{
	return WKPluginShowEventX(szEvent,pParent,COleDateTime::GetCurrentTime(),1);
}

int WINAPI WKPluginDelEvent(char szEvent[128])
{
	_RemoveReminder fp3=(_RemoveReminder)GetProcAddress(hRemin,"RemoveReminder");
	_IsReminderFamilyPresent fpPr=(_IsReminderFamilyPresent)GetProcAddress(hRemin,"IsReminderFamilyPresent");
	int iMaxIndex=0;
	if(fpPr && (*fpPr)(szEvent,&iMaxIndex)){
		for(int i=0;i<=iMaxIndex;i++){
			if(fp3){
				char szKey[64]={0};
				strcpy(szKey,GetNextPrefixedId(szEvent,i));
				(*fp3)(szKey);
			}
		}
	}
	return 0;
}

int WINAPI WKPluginIsEventPresent(char szEvent[128])
{
	_GetReminder fp=(_GetReminder)GetProcAddress(hRemin,"GetReminder");
	if(fp){
		CWPReminder rem;
		memset(&rem,0,sizeof(CWPReminder));
		strcpy(rem.szKey,szEvent);
		return (*fp)(rem.szKey,rem);
	}
	return 0;
}

CWnd* AfxWPGetMainWnd()
{
	static CWnd wnd;
	static BOOL bInit=0;
	if(bInit==0){
		bInit=1;
		wnd.Attach(WKGetPluginContainer()->GetWKMainWindow());
	}
	return &wnd;
}

int iAddNew=0;
int iShowAll=0;
int iIdBeginning=0;
CStringArray aRKeys;
int WINAPI WKTrayMenuPopup(int iActionType, int& iItemId, HMENU hMenu)
{
	if(!bAddTrayMenu){
		return 0;
	}
	if(iActionType==TM_ADDCUSTOMITEM){
		aRKeys.RemoveAll();
		iIdBeginning=iItemId;
		int iCount=0;
		_SortReminders srp=(_SortReminders)GetProcAddress(hRemin,"SortReminders");
		if(srp){
			(*srp)();
		}
		_GetReminder fp=(_GetReminder)GetProcAddress(hRemin,"GetReminder");
		COleDateTime dtNow=COleDateTime::GetCurrentTime();
		HMENU mn=CreatePopupMenu();
		int iPos=0;
		CString sText;
		char szKey[64]={0};
		sprintf(szKey,"#%i",iCount);
		CWPReminder rem;
		while((*fp)(szKey,rem)){
			COleDateTime tm(rem.EventTime);
			sText=GetRemDsc(rem,TRUE);
			if(sText!="" && abs(tm-dtNow)<=14 && !rem.bDisableRemider){
				AppendMenu(mn,MF_STRING,iItemId,sText);
				aRKeys.SetAtGrow(iPos,rem.szKey);
				iPos++;
			}
			iItemId++;//Увеличиваем на все, даже на пустые!
			iCount++;
			rem.Clear();
			sprintf(szKey,"#%i",iCount);
		}
		//if(iPos>0)//Всегда
		{
			{
				iAddNew=iItemId;
				iItemId++;
				AppendMenu(mn,MF_STRING,iAddNew,_l2("Add new reminder"));
			}
			{
				iShowAll=iItemId;
				iItemId++;
				AppendMenu(mn,MF_STRING,iShowAll,_l2("Show calendar"));
			}
			AppendMenu(hMenu,MF_POPUP|MF_STRING,UINT(mn),_l2("Reminders"));
		}
	}else{
		if(iItemId==iShowAll){
			ShowMainDialog(0);
		}else if(iItemId==iAddNew){
			//WKCallPluginFunction(0,0);
			//CreateRemOnDate(0,COleDateTime::GetCurrentTime(),FALSE);
			CString sSelectedKey=calcNextPrefixedId(DEF_ID);
			char szEvent[128]={0};
			strcpy(szEvent,sSelectedKey);
			WKPluginShowEventX(szEvent,0,COleDateTime::GetCurrentTime(),0);
		}else{
			char szKey[64]={0};
			strcpy(szKey,aRKeys[iItemId-iIdBeginning]);
			WKPluginShowEventX(szKey,0,COleDateTime::GetCurrentTime(),0);
		}
	}
	return 0;
}