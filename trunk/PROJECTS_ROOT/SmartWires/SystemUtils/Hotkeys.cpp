#include "stdafx.h"
#include "hotkeys.h"
#include "SupportClasses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define FLOG2a(x,y1,z1)			{FILE* f=fopen("c:\\wk_debug.txt","a+");fprintf(f,"[%lu] ",GetTickCount());fprintf(f,x,y1,z1);fprintf(f,"\n");fclose(f);};
#else
#define FLOG2a(x,y1,z1)
#endif
void GetBytesFromDword(char* szBytes, DWORD* dwDwords)
{
	szBytes[0]=BYTE((dwDwords[0] & 0x000000ff));
	szBytes[1]=BYTE((dwDwords[0] & 0x0000ff00)>>8);
	szBytes[2]=BYTE((dwDwords[0] & 0x00ff0000)>>16);
	szBytes[3]=BYTE((dwDwords[0] & 0xff000000)>>24);
	return;
}

DWORD GetDwordFromBytes(const char* szBytes)
{
	DWORD dwRes=szBytes[0]+(szBytes[1]<<8)+(szBytes[2]<<16)+(szBytes[3]<<24);
	return dwRes;
}

HINSTANCE& WKIHOTKEYS_GetExternalDLLInstance()
{
	static HINSTANCE hExternalDLLInstance=NULL;
	return hExternalDLLInstance;
}

BOOL& WKIHOTKEYS_CheckForExternalDLL()
{
	static BOOL bCheckForExternalDL=FALSE;
	return bCheckForExternalDL;
};

typedef HWND& (WINAPI *_WKIHOTKEYS_EventsHandlerWindow)();
HWND& WINAPI WKIHOTKEYS_EventsHandlerWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_EventsHandlerWindow fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_EventsHandlerWindow)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_EventsHandlerWindow");
			}
			return (*fp)();
		}else{
			fp=NULL;
		}
	}
	static HWND hEventsHandlerWindow=0;
	return hEventsHandlerWindow;
};

typedef HWND& (WINAPI *_WKIHOTKEYS_EventsHandlerDuplicateWindow)();
HWND& WINAPI WKIHOTKEYS_EventsHandlerDuplicateWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_EventsHandlerDuplicateWindow fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_EventsHandlerDuplicateWindow)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_EventsHandlerDuplicateWindow");
			}
			return (*fp)();
		}else{
			fp=NULL;
		}
	}
	static HWND hEventsHandlerWindow=0;
	return hEventsHandlerWindow;
};

// Работа с внешними ивентами
CExternEventClassList& GetExternEventClassList()
{
	static CExternEventClassList cl;
	return cl;
}

CExternEventList& GetExternEventList()
{
	static CExternEventList el;
	return el;
}

CExternEventClass* CExternEventClassList::AddEventClass(const char* szEventClassName)
{
	if(!szEventClassName){
		return 0;
	}
	if(g_pEventsClassesList==0){
		g_pEventsClassesList=new CExternEventClass(szEventClassName);
		return g_pEventsClassesList;
	}
	CExternEventClass* pCList=g_pEventsClassesList;
	while(pCList){
		if(strcmp(pCList->szEventClassName,szEventClassName)==0){
			return pCList;
		}
		pCList=pCList->pNext;
	}
	if(pCList==0){
		pCList=new CExternEventClass(szEventClassName);
		pCList->pNext=g_pEventsClassesList;
		g_pEventsClassesList=pCList;
	}
	return pCList;
}

CExternEventClass* CExternEventClassList::GetClass(int iIndex)
{
	CExternEventClass* pPos=g_pEventsClassesList;
	while(pPos){
		if(iIndex==0){
			return pPos;
		}
		pPos=pPos->pNext;
		iIndex--;
	}
	return 0;
}

CExternEvent* CExternEventList::AddEvent(const char* szEventID, const char* szShortDescription, const char* szEventClassName)
{
	if(!szEventID || !szShortDescription || !szEventClassName){
		return 0;
	}
	if(g_pEventsList==0){
		g_pEventsList=new CExternEvent(szEventID, szShortDescription, szEventClassName);
		return g_pEventsList;
	}
	CExternEvent* pCList=g_pEventsList;
	while(pCList){
		if(strcmp(pCList->szEventID,szEventID)==0){
			return pCList;
		}
		pCList=pCList->pNext;
	}
	if(pCList==0){
		pCList=new CExternEvent(szEventID, szShortDescription, szEventClassName);
		pCList->pNext=g_pEventsList;
		g_pEventsList=pCList;
	}
	return pCList;
}

BOOL CExternEventList::RemEvent(const char* szEventID)
{
	if(!szEventID){
		return 0;
	}
	CExternEvent* pCList=g_pEventsList;
	CExternEvent* pCListpPrev=0;
	while(pCList){
		if(strcmp(pCList->szEventID,szEventID)==0){
			if(pCListpPrev!=0){
				pCListpPrev->pNext=pCList->pNext;
			}else{
				g_pEventsList=pCList->pNext;
			}
			delete pCList;
			return 1;
		}
		pCListpPrev=pCList;
		pCList=pCList->pNext;
	}
	return 0;
}

CExternEvent* CExternEventList::GetEvent(int iIndex)
{
	CExternEvent* pPos=g_pEventsList;
	while(pPos){
		if(iIndex==0){
			return pPos;
		}
		pPos=pPos->pNext;
		iIndex--;
	}
	return 0;
}

CExternEvent* CExternEventList::GetEvent(const char* szEventID)
{
	if(!szEventID){
		return 0;
	}
	CExternEvent* pPos=g_pEventsList;
	while(pPos){
		if(strcmp(pPos->szEventID,szEventID)==0){
			return pPos;
		}
		pPos=pPos->pNext;
	}
	return 0;
}

CExternEvent::CExternEvent(const char* in_szEventID, const char* in_szShortDescription, const char* in_szEventClassName)
{
	pNext=0;
	strcpy(szEventID,in_szEventID);
	memset(szEventName,0,sizeof(szEventName));
	if(strlen(in_szShortDescription)>sizeof(szEventName)-1){
		memcpy(szEventName,in_szShortDescription,sizeof(szEventName)-1);
	}else{
		strcpy(szEventName,in_szShortDescription);
	}
	pEventClass=GetExternEventClassList().AddEventClass(in_szEventClassName);
}

typedef BOOL (WINAPI *_WKIHOTKEYS_ResetKB)();
typedef void (WINAPI *_ResetKBBuffer)(BOOL bADOnly);
BOOL WINAPI WKIHOTKEYS_ResetKB()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_ResetKB fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_ResetKB)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_ResetKB");
			}
			return (*fp)();
		}else{
			fp=NULL;
		}
	}
	_ResetKBBuffer hReset=NULL;
	HINSTANCE hHookDLL=GetModuleHandle("wkkeyh.dll");
	if(hHookDLL){
		hReset = (_ResetKBBuffer)DBG_GETPROC( hHookDLL,"ResetKBBuffer");
		if(hReset){
			(*hReset)(0);
		}
	}
	return TRUE;
};

typedef BOOL (WINAPI *_WKIHOTKEYS_RegisterExternEvent)(const char* szEventID, const char* szShortDescription, const char* szEventClassName);
BOOL WINAPI WKIHOTKEYS_RegisterExternEvent(const char* szEventID, const char* szShortDescription, const char* szEventClassName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_RegisterExternEvent fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_RegisterExternEvent)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_RegisterExternEvent");
			}
			return (*fp)(szEventID, szShortDescription, szEventClassName);
		}else{
			fp=NULL;
		}
	}
	GetExternEventList().AddEvent(szEventID, szShortDescription, szEventClassName);
	return TRUE;
};

typedef BOOL (WINAPI *_WKIHOTKEYS_UnRegisterExternEvent)(const char* szEventID);
BOOL WINAPI WKIHOTKEYS_UnRegisterExternEvent(const char* szEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_UnRegisterExternEvent fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_UnRegisterExternEvent)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_UnRegisterExternEvent");
			}
			return (*fp)(szEventID);
		}else{
			fp=NULL;
		}
	}
	GetExternEventList().RemEvent(szEventID);
	return TRUE;
};

// iLockStatus>0 - Lock
// iLockStatus<0 - Unlock
// iLockStatus==0 - just get
CStringArray& GetActiveEvents(int iLockStatus=0)
{
	static CCriticalSection csActiveEvents;
	if(iLockStatus>0){
		csActiveEvents.Lock();
	}
	if(iLockStatus<0){
		csActiveEvents.Unlock();
	}
	static CStringArray aActiveEvents;
	return aActiveEvents;
}
CArray<DWORD,DWORD> aEventSequence;

typedef CStringArray& (WINAPI *_WKIHOTKEYS_GetExternEvents)(int iLockStatus);
CStringArray& WINAPI WKIHOTKEYS_GetExternEvents(int iLockStatus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_GetExternEvents fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_GetExternEvents)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_GetExternEvents");
			}
			return (*fp)(iLockStatus);
		}else{
			fp=NULL;
		}
	}
	return GetActiveEvents(iLockStatus);
};

typedef BOOL (WINAPI *_WKIHOTKEYS_GetExternEventClass)(const char*, char*);
BOOL WINAPI WKIHOTKEYS_GetExternEventClass(const char* szEventID, char* szEventClassOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_GetExternEventClass fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_GetExternEventClass)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_GetExternEventClass");
			}
			return (*fp)(szEventID,szEventClassOut);
		}else{
			fp=NULL;
		}
	}
	BOOL bRes=0;
	CExternEvent* pEvent=GetExternEventList().GetEvent(szEventID);
	if(pEvent && pEvent->pEventClass){
		bRes=TRUE;
		strcpy(szEventClassOut,pEvent->pEventClass->szEventClassName);
	}
	return bRes;
};

typedef BOOL (WINAPI *_WKIHOTKEYS_ExternEventStart)(const char* szEventID);
BOOL WINAPI WKIHOTKEYS_ExternEventStart(const char* szEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_ExternEventStart fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_ExternEventStart)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_ExternEventStart");
			}
			return (*fp)(szEventID);
		}else{
			fp=NULL;
		}
	}
	char szEventClass[64]={0};
	WKIHOTKEYS_GetExternEventClass(szEventID,szEventClass);
	BOOL bAutoRepeat=FALSE;
	CStringArray& evMap=GetActiveEvents(1);
	// Проверяем, допускает ли неуникальность? и соотв. запрет автоповтора.
	if(strstr(szEventClass,"@NU")==0){
		for(int i=0;i<evMap.GetSize();i++){
			if(evMap[i]==szEventID){
				bAutoRepeat=TRUE;
				break;
			}
		}
	}
	if(!bAutoRepeat){
		evMap.Add(szEventID);
	}
	DWORD dwEventID=GetDwordFromBytes(szEventID);
	if(WKIHOTKEYS_EventsHandlerWindow()){
		::PostMessage(WKIHOTKEYS_EventsHandlerWindow(),WM_USER+105/*MSGKB_JUSTINFORM_EV*/,VK_EVENTKEY,dwEventID);
	}
	if(strstr(szEventClass,"@NS")==NULL && WKIHOTKEYS_EventsHandlerDuplicateWindow()){
		::PostMessage(WKIHOTKEYS_EventsHandlerDuplicateWindow(),WM_HOTKEY,VK_EVENTKEY,(LPARAM)szEventID);
	}
	if(WKIHOTKEYS_EventsHandlerDuplicateWindow()==0){// Регистрируем в цикловой карте и проверяем на совпадения гор.клавиш
		//SimpleLocker cs(&csKeySequence);
		static DWORD dwLastPress=0;
		if(dwLastPress!=0 && GetTickCount()-dwLastPress>5000){
			aEventSequence.RemoveAll();
		}
		dwLastPress=GetTickCount();
		aEventSequence.InsertAt(0,dwEventID);
		aEventSequence.SetSize(10);
		{// Пробегаемся по карте гор. клавиш
			CRegisteredHotkeys& hkMap=getRegisteredHotkeysInfo(1);
			POSITION pos=hkMap.GetStartPosition();
			CRegisteredHotkeyKey iInfo;
			BOOL bTheSameEvent=0;
			int iCounter=0;
			while(pos){
				CRegisteredHotkeyData hkTest;
				hkMap.GetNextAssoc(pos,iInfo,hkTest);
				iCounter++;
				if(hkTest.oHotkey.isEvent() && hkTest.oHotkey.Present()){// Проверяем совпадение
					int iPos=0;
					CIHotkey hkFull;
					hkFull.Options()|=ADDOPT_SEQUENCE;
					while(!bTheSameEvent && iPos<aEventSequence.GetSize()){
						hkFull.AddOneKeySeq(aEventSequence[iPos++],1);
						bTheSameEvent=hkTest.oHotkey.IsEventsInList(evMap,hkFull);
						if(bTheSameEvent){
							if(!hkTest.oHotkey.AppConditionsFail()){
								CRegisteredHotkeyData& hkTestOriginal=hkMap[iInfo];
								hkTestOriginal.bOnUnpress=0;
								hkTestOriginal.bAutoRepeat=bAutoRepeat;
								hkTestOriginal.dwLastKeyThatActivatedHK=VK_EVENTKEY;
								hkTestOriginal.oRealTriggeredHotkey=hkTest.oHotkey;
								//FLOG3("WM_HOTKEY File=%s, char=%lu, unp=%i",GetIHotkeyName(hk),wCode,hkTestOriginal.bOnUnpress);
								::PostMessage(iInfo.hWindow, WM_HOTKEY, iInfo.iID, hkTestOriginal.oHotkey.GetAsDWORD());
								// И дроппаем это
								aEventSequence.RemoveAll();
							}
						}
					}
				}
			}
			getRegisteredHotkeysInfo(-1);
		}
	}
	// Отпускаем лок на ивенты
	GetActiveEvents(-1);
	return TRUE;
};

typedef BOOL (WINAPI *_WKIHOTKEYS_ExternEventStop)(const char* szEventID);
BOOL WINAPI WKIHOTKEYS_ExternEventStop(const char* szEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_ExternEventStop fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_ExternEventStop)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_ExternEventStop");
			}
			return (*fp)(szEventID);
		}else{
			fp=NULL;
		}
	}
	BOOL bFindOK=FALSE;
	CStringArray& aActiveEvents=GetActiveEvents(1);
	for(int i=aActiveEvents.GetSize()-1;i>=0;i--){
		if(aActiveEvents[i]==szEventID){
			aActiveEvents.RemoveAt(i);
			if(WKIHOTKEYS_EventsHandlerWindow()){
				::PostMessage(WKIHOTKEYS_EventsHandlerWindow(),WM_USER+105/*MSGKB_JUSTINFORM_EV*/,VK_EVENTKEY,0);
			}
			bFindOK=TRUE;
			break;
		}
	}
	GetActiveEvents(-1);
	return TRUE;
};


typedef CExternEventClass* (WINAPI *_WKIHOTKEYS_ExternGetClass)(int);
CExternEventClass* WINAPI WKIHOTKEYS_ExternGetClass(int iIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_ExternGetClass fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_ExternGetClass)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_ExternGetClass");
			}
			return (*fp)(iIndex);
		}else{
			fp=NULL;
		}
	}
	if(iIndex<0){
		return 0;
	}
	return GetExternEventClassList().GetClass(iIndex);
}

typedef CExternEvent* (WINAPI *_WKIHOTKEYS_ExternGetEvent)(int);
CExternEvent* WINAPI WKIHOTKEYS_ExternGetEvent(int iIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_ExternGetEvent fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_ExternGetEvent)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_ExternGetEvent");
			}
			return (*fp)(iIndex);
		}else{
			fp=NULL;
		}
	}
	if(iIndex<0){
		return 0;
	}
	return GetExternEventList().GetEvent(iIndex);
}

typedef int (WINAPI *_WKIHOTKEYS_RegisterOnAddRemoveEventCallback)(FPONEVENTADD,FPONEVENTREMOVE);
CArray<FPONEVENTADD,FPONEVENTADD> aFPOnEventAdd;
CArray<FPONEVENTREMOVE,FPONEVENTREMOVE> aFPOnEventRemove;
int WINAPI WKIHOTKEYS_RegisterOnAddRemoveEventCallback(FPONEVENTADD fpAdd,FPONEVENTREMOVE fpRemove)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_RegisterOnAddRemoveEventCallback fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_RegisterOnAddRemoveEventCallback)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_RegisterOnAddRemoveEventCallback");
			}
			return (*fp)(fpAdd,fpRemove);
		}else{
			fp=NULL;
		}
	}
	if(fpAdd){
		aFPOnEventAdd.Add(fpAdd);
	}
	if(fpRemove){
		aFPOnEventRemove.Add(fpRemove);
	}
	return 0;
}

typedef int (WINAPI *_WKIHOTKEYS_NotifyOnAddEvent)(char const*);
int WINAPI WKIHOTKEYS_NotifyOnAddEvent(char const* szEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_NotifyOnAddEvent fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_NotifyOnAddEvent)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_NotifyOnAddEvent");
			}
			return (*fp)(szEventID);
		}else{
			fp=NULL;
		}
	}
	int iSum=0;
	for(int i=0;i<aFPOnEventAdd.GetSize();i++){
		if(aFPOnEventAdd[i]){
			iSum+=((*aFPOnEventAdd[i])(szEventID));
		}
	}
	return iSum;
}

typedef CExternEvent* (WINAPI *_WKIHOTKEYS_ExternGetEventByID)(const char* szEventID);
CExternEvent* WINAPI WKIHOTKEYS_ExternGetEventByID(const char* szEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_ExternGetEventByID fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_ExternGetEventByID)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_ExternGetEventByID");
			}
			return (*fp)(szEventID);
		}else{
			fp=NULL;
		}
	}
	return GetExternEventList().GetEvent(szEventID);
}

typedef BOOL& (WINAPI *_WKIHOTKEYS_DistingLRKeys)();
BOOL& WINAPI WKIHOTKEYS_DistingLRKeys()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_DistingLRKeys fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_DistingLRKeys)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_DistingLRKeys");
			}
			return (*fp)();
		}else{
			fp=NULL;
		}
	}
	static BOOL bDistingLRKeys=FALSE;
	return bDistingLRKeys;
};

struct CCharToName
{
	DWORD dwChar;
	const char* szName;
};

#define MMKEYPREFIX	"@"
CCharToName g_C2NCodesHandHelp[]=
{
	//-- Мышь --
	{VK_LBUTTON,	MMKEYPREFIX"LMB"},
	{VK_RBUTTON,	MMKEYPREFIX"RMB"},
	{VK_MBUTTON,	MMKEYPREFIX"MMB"},
	{VK_MBT_DBL,	MMKEYPREFIX"2xMMB"},
	{VK_LBT_DBL,	MMKEYPREFIX"2xLMB"},
	{VK_RBT_DBL,	MMKEYPREFIX"2xRMB"},
	{WHEEL_CL,		MMKEYPREFIX"Wheel Click"},
	{WHEEL_UP,		MMKEYPREFIX"Wheel Up"},
	{WHEEL_DO,		MMKEYPREFIX"Wheel Down"},
	{VK_XBTTNWK1,	MMKEYPREFIX"X1MB"},
	{VK_XBTTNWK2,	MMKEYPREFIX"X2MB"},
	{VK_XBTTNWK3,	MMKEYPREFIX"X3MB"},
	{VK_XBTTNWK4,	MMKEYPREFIX"X4MB"},
	{VK_MCORNER_RT,	MMKEYPREFIX"RT-Corner"},
	{VK_MCORNER_RB,	MMKEYPREFIX"RB-Corner"},
	{VK_MCORNER_LT,	MMKEYPREFIX"LT-Corner"},
	{VK_MCORNER_LB,	MMKEYPREFIX"LB-Corner"},
	{VK_MBORDER_T,	MMKEYPREFIX"T-Border"},
	{VK_MBORDER_B,	MMKEYPREFIX"B-Border"},
	{VK_MBORDER_R,	MMKEYPREFIX"R-Border"},
	{VK_MBORDER_L,	MMKEYPREFIX"L-Border"},
	{VK_MOB_ICO,	MMKEYPREFIX"MOverSys"},
	{VK_MOB_MAX,	MMKEYPREFIX"MOverMax"},
	{VK_MOB_MIN,	MMKEYPREFIX"MOverMin"},
	{VK_MOB_CLOSE,	MMKEYPREFIX"MOverClose"},
	{VK_MO_TITLE,	MMKEYPREFIX"MOverTitle"},
	{VK_MOB_HLP,	MMKEYPREFIX"MOverHelp"},
	{VK_MOB_MLES,	MMKEYPREFIX"Motionless"},
	//-- Разное --
	{VK_2X,			"2x"},
	{VK_3X,			"3x"},
	//-- Клавиатура --
	{VK_PRESSNHOLD,	"(P+H)"},
	{0xae,			"Volume Down"},
	{0xaf,			"Volume Up"},
	{0xad,			"Volume Mute"},
	{0xac,			MMKEYPREFIX"Browser"},
	{0xa6,			MMKEYPREFIX"Back"},
	{0xa7,			MMKEYPREFIX"Forw."},
	{0xa8,			MMKEYPREFIX"Refr."},
	{0xa9,			MMKEYPREFIX"Stop"},
	{0xaa,			MMKEYPREFIX"Find"},
	{0xab,			MMKEYPREFIX"Fav."},
	{0xb0,			MMKEYPREFIX"Next"},
	{0xb1,			MMKEYPREFIX"Prev."},
	{0xb2,			MMKEYPREFIX"Stop"},
	{0xb3,			MMKEYPREFIX"Play"},
	{0xb4,			MMKEYPREFIX"EMail"},
	{0xb5,			MMKEYPREFIX"Media"},
	{34,			"PageDown"},
	{33,			"PageUp"},
	{36,			"Home"},
	{VK_INSERT,		"Ins"},
	{VK_END,		"End"},
	{VK_DELETE,		"Delete"},
	{VK_BACK,		"Backspace"},
	{VK_UP,			"Up"},
	{VK_DOWN,		"Down"},
	{VK_LEFT,		"Left"},
	{VK_RIGHT,		"Right"},
	{VK_NUMLOCK,	"NumLock"},
	{VK_SHIFT,		"Shift"},
	{VK_RSHIFT,		"RShift"},
	{VK_LSHIFT,		"LShift"},
	{VK_CONTROL,	"Control"},
	{VK_RCONTROL,	"RControl"},
	{VK_LCONTROL,	"LControl"},
	{VK_MENU,		"Alt"},
	{VK_RMENU,		"RAlt"},
	{VK_LMENU,		"LAlt"},
	{VK_RWIN,		"RWin"},
	{VK_LWIN,		"LWin"},
	{VK_EVENTKEY,	"<???>"},
	{0,""}
};

CCharToName g_C2NCodes[]=
{
	{VK_SNAPSHOT,"Prt Scr"},
	{VK_SPACE,	"Space"},
	{VK_APPS,	"Apps"},
	{VK_PAUSE,	"Pause"},
	{VK_TAB,	"Tab"},
	{VK_ESCAPE,	"Esc"},
	{0,""}
};

#define IS_ADDKEY(wLo)		(wLo>KBBUFFER_SIZE)||(wLo>=0xA6 && wLo!=0xc0 && wLo!=0xdc && wLo!=0xbb && wLo!=0xbd && wLo!=0xdd && wLo!=0xdb && wLo!=0xba && wLo!=0xde && wLo!=0xbc && wLo!=0xbe && wLo!=0xbf)
const char* GetKeyNameByCode(BOOL bHandHeld, DWORD dwBaseKeys, char szDefault[MAXHOTKEY_NAMELEN])
{
	CCharToName* aMap=(CCharToName*)g_C2NCodes;
	if(bHandHeld){
		aMap=(CCharToName*)g_C2NCodesHandHelp;
	}
	int i=0;
	while(aMap[i].dwChar>0){
		if(aMap[i].dwChar==dwBaseKeys){
			strcpy(szDefault,aMap[i].szName);
			return szDefault;
		}
		i++;
	}
	if(IS_ADDKEY(dwBaseKeys)){
		if(dwBaseKeys>KBBUFFER_SIZE){
			sprintf(szDefault,"%s%c%c",MMKEYPREFIX,'A'+(((dwBaseKeys)&0xf0)>>4), 'A'+((dwBaseKeys)&0x0f));
		}else{
			sprintf(szDefault,"%s%2x",MMKEYPREFIX,dwBaseKeys);
		}
		return szDefault;
	}
	return NULL;
}

void AddHKChar(CString& sRes, const char* szWhat)
{
	if(sRes!="" && sRes.Right(1)!=":"){
		sRes+="-";
	}
	if(sRes!="" && sRes.Right(1)==":"){
		sRes.SetAt(sRes.GetLength()-1,',');
	}
	sRes+=szWhat;
}

int MapHotkeyToBlockBuffer(DWORD* baBlockKeys, CIHotkey& oHotKey, BOOL bWithFollowers)
{
	int iCount=0;
	if(!oHotKey.isEvent())
	{
		if(oHotKey.m_dwModifiers & HOTKEYF_CONTROL){
			iCount++;
			baBlockKeys[VK_CONTROL]=(baBlockKeys[VK_CONTROL] | BLOCKSTATE_PROCESS);
			baBlockKeys[VK_RCONTROL]=(baBlockKeys[VK_RCONTROL] | BLOCKSTATE_PROCESS);
			baBlockKeys[VK_LCONTROL]=(baBlockKeys[VK_LCONTROL] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_RCONTROL){
			iCount++;
			baBlockKeys[VK_RCONTROL]=(baBlockKeys[VK_RCONTROL] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LCONTROL){
			iCount++;
			baBlockKeys[VK_LCONTROL]=(baBlockKeys[VK_LCONTROL] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_SHIFT){
			iCount++;
			baBlockKeys[VK_SHIFT]=(baBlockKeys[VK_SHIFT] | BLOCKSTATE_PROCESS);
			baBlockKeys[VK_RSHIFT]=(baBlockKeys[VK_RSHIFT] | BLOCKSTATE_PROCESS);
			baBlockKeys[VK_LSHIFT]=(baBlockKeys[VK_LSHIFT] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_RSHIFT){
			iCount++;
			baBlockKeys[VK_RSHIFT]=(baBlockKeys[VK_RSHIFT] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LSHIFT){
			iCount++;
			baBlockKeys[VK_LSHIFT]=(baBlockKeys[VK_LSHIFT] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_ALT){
			iCount++;
			baBlockKeys[VK_MENU]=(baBlockKeys[VK_MENU] | BLOCKSTATE_PROCESS);
			baBlockKeys[VK_RMENU]=(baBlockKeys[VK_RMENU] | BLOCKSTATE_PROCESS);
			baBlockKeys[VK_LMENU]=(baBlockKeys[VK_LMENU] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_RALT){
			iCount++;
			baBlockKeys[VK_RMENU]=(baBlockKeys[VK_RMENU] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LALT){
			iCount++;
			baBlockKeys[VK_LMENU]=(baBlockKeys[VK_LMENU] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_EXT){
			iCount++;
			baBlockKeys[VK_LWIN]=(baBlockKeys[VK_LWIN] | BLOCKSTATE_PROCESS);
			baBlockKeys[VK_RWIN]=(baBlockKeys[VK_RWIN] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_REXT){
			iCount++;
			baBlockKeys[VK_RWIN]=(baBlockKeys[VK_RWIN] | BLOCKSTATE_PROCESS);
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LEXT){
			iCount++;
			baBlockKeys[VK_LWIN]=(baBlockKeys[VK_LWIN] | BLOCKSTATE_PROCESS);
		};
		for(int iC=0;iC<MAX_KEYS;iC++){
			if(oHotKey.m_dwBaseKey[iC]!=0){
				iCount++;
				DWORD dwCode=oHotKey.m_dwBaseKey[iC];
				if(dwCode<KBBUFFER_DSIZE){
					baBlockKeys[dwCode]|=BLOCKSTATE_PROCESS;
				}
			}
		}
	}
	if(bWithFollowers && oHotKey.pNextHK){
		iCount+=MapHotkeyToBlockBuffer(baBlockKeys,*oHotKey.pNextHK,bWithFollowers);
	}
	return iCount;
}

CString GetIHotkeyName(CIHotkey& oHotKey, BOOL bWithFollowers, BOOL BShrinkIfBig)
{
	CString sRes="";
	if(!oHotKey.Present() && oHotKey.m_dwModifiers==0){
		CIHotkey* pPos=oHotKey.pNextHK;
		if(pPos){
			return GetIHotkeyName(*pPos,bWithFollowers);
		}
		return sRes;
	}
	if(WKIHOTKEYS_DistingLRKeys()){
		if(oHotKey.m_dwModifiers & HOTKEYF_CONTROL){
			AddHKChar(sRes,"Ctrl");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_RCONTROL){
			AddHKChar(sRes,"RCtrl");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LCONTROL){
			AddHKChar(sRes,"LCtrl");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_SHIFT){
			AddHKChar(sRes,"Shift");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_RSHIFT){
			AddHKChar(sRes,"RShift");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LSHIFT){
			AddHKChar(sRes,"LShift");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_ALT){
			AddHKChar(sRes,"Alt");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_RALT){
			AddHKChar(sRes,"RAlt");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LALT){
			AddHKChar(sRes,"LAlt");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_EXT){
			AddHKChar(sRes,"Win");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_REXT){
			AddHKChar(sRes,"RWin");
		};
		if(oHotKey.m_dwModifiers & HOTKEYF_LEXT){
			AddHKChar(sRes,"LWin");
		};
	}else {
		if((oHotKey.m_dwModifiers & HOTKEYF_CONTROL) || (oHotKey.m_dwModifiers & HOTKEYF_LCONTROL) || (oHotKey.m_dwModifiers & HOTKEYF_RCONTROL)){
			AddHKChar(sRes,"Ctrl");
		};
		if((oHotKey.m_dwModifiers & HOTKEYF_SHIFT) || (oHotKey.m_dwModifiers & HOTKEYF_LSHIFT) || (oHotKey.m_dwModifiers & HOTKEYF_RSHIFT)){
			AddHKChar(sRes,"Shift");
		};
		if((oHotKey.m_dwModifiers & HOTKEYF_ALT) || (oHotKey.m_dwModifiers & HOTKEYF_LALT) || (oHotKey.m_dwModifiers & HOTKEYF_RALT)){
			AddHKChar(sRes,"Alt");
		};
		if((oHotKey.m_dwModifiers & HOTKEYF_EXT) || (oHotKey.m_dwModifiers & HOTKEYF_LEXT) || (oHotKey.m_dwModifiers & HOTKEYF_REXT)){
			AddHKChar(sRes,"Win");
		};
	}
	CString sChar;
	for(int iC=0;iC<MAX_KEYS;iC++){
		if(oHotKey.m_dwBaseKey[iC]!=0){
			char szBuffer[MAXHOTKEY_NAMELEN]="";
			if(oHotKey.Options()&ADDOPT_EVENT){
				// Спец клавиша - отдельно!!!
				char szEventID[EVENT_KEYS]="";
				GetBytesFromDword(szEventID,&oHotKey.m_dwBaseKey[iC]);
				szEventID[EVENT_KEYS-1]=0;
				CExternEvent* pEvent=WKIHOTKEYS_ExternGetEventByID(szEventID);
				if(pEvent){
					strcpy(szBuffer,pEvent->szEventName);
				}else{
					strcpy(szBuffer,"@Event:");
					strcat(szBuffer,szEventID);
				}
			}else{
				if(GetKeyNameByCode(TRUE,oHotKey.m_dwBaseKey[iC],szBuffer)==NULL){
					szBuffer[0]=char(oHotKey.m_dwBaseKey[iC]);
					DWORD dwChar=MapVirtualKey(oHotKey.m_dwBaseKey[iC],0);
					if(GetKeyNameText((dwChar<<16),szBuffer,sizeof(szBuffer))==0){
						GetKeyNameByCode(FALSE,oHotKey.m_dwBaseKey[iC],szBuffer);
					}
				}
			}
			AddHKChar(sRes,szBuffer);
			if(CDebugWPHelper::isDebug(2)){
				CString sDebug;
				sDebug.Format("[#%04X]",oHotKey.m_dwBaseKey[iC]);
				sRes+=sDebug;
			}
		}
	}
	if(!BShrinkIfBig)
	{
		if((oHotKey.Options()&ADDOPT_SEQUENCE)!=0){
			AddHKChar(sRes,CI18N::_l("Sequence"));
		}else if((oHotKey.Options()&ADDOPT_TRANSPARENT)!=0){
			AddHKChar(sRes,CI18N::_l("Pass-through"));
		}
		if((oHotKey.Options()&ADDOPT_ON_WPRESSED)!=0){
			AddHKChar(sRes,CI18N::_l("Press+Unpress"));
		}else if((oHotKey.Options()&ADDOPT_ON_UNPRESS)!=0){
			AddHKChar(sRes,CI18N::_l("Unpress"));
		}
	}
	if(oHotKey.pNextHK && bWithFollowers){
		CIHotkey* pPos=oHotKey.pNextHK;
		while(pPos){
			sRes+=", ";
			sRes+=GetIHotkeyName(*pPos,FALSE);
			pPos=pPos->pNextHK;
		}
	}
	if(BShrinkIfBig==1){
		//sRes=TrimMessage(sRes,25,1);
		sRes.Replace("Ctrl","Ct");
		sRes.Replace("Alt","Al");
		sRes.Replace("Shift","Sh");
		sRes.Replace("Num ","Nm");
		sRes.Replace("Win","Wn");
		sRes.Replace("Wheel","Wl");
		sRes.Replace("PageDown","PgD");
		sRes.Replace("PageUp","PgU");
		sRes.Replace("Down","Dn");
		sRes.Replace("Left","Lf");
		sRes.Replace("Right","Rt");
		sRes.Replace("Corner","Cn");
		sRes.Replace("Border","Br");
		sRes.Replace("Delete","Dl");
		sRes.Replace("Backspace","Bs");
		sRes.Replace("NumLock","NL");
		sRes.Replace("Volume","Vl");
		sRes.Replace("Home","Hm");
		sRes.Replace("Scroll Lock","ScLc");
		sRes.Replace("Caps Lock","Caps");
		sRes.Replace("Browser","Brw");
		sRes.Replace("Print","Prn");
		sRes.Replace("Screen","Scr");
		sRes.Replace("Pause","Ps");
		sRes.Replace("Apps","Ap");
		sRes.Replace("Return","Ret");
		sRes.Replace("Enter","En");
		sRes.Replace("Over","O");
		sRes.Replace("Title","T");
		sRes.Replace("Close","C");
	}
	return sRes;
}

DWORD GetHotKeyForRegistration(DWORD dwHKey, WORD& wHi, WORD& wLo)
{
	if(dwHKey==0){
		return 0;
	}
	wLo=LOWORD(dwHKey);
	wHi=HIWORD(dwHKey);
	wHi=((((wHi&HOTKEYF_ALT)||(wHi&HOTKEYF_RALT)||(wHi&HOTKEYF_LALT))?MOD_ALT:0)
		|(((wHi&HOTKEYF_CONTROL)||(wHi&HOTKEYF_RCONTROL)||(wHi&HOTKEYF_LCONTROL))?MOD_CONTROL:0)
		|(((wHi&HOTKEYF_SHIFT)||(wHi&HOTKEYF_RSHIFT)||(wHi&HOTKEYF_LSHIFT))?MOD_SHIFT:0)
		|(((wHi&HOTKEYF_EXT)||(wHi&HOTKEYF_REXT)||(wHi&HOTKEYF_LEXT))?MOD_WIN:0));
	return 1;
}

typedef CRegisteredHotkeys& (WINAPI *_getRegisteredHotkeysInfo)(int iLockStatus);
CRegisteredHotkeys& WINAPI getRegisteredHotkeysInfo(int iLockStatus)
{
	#pragma message("Add safe-threadness")
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _getRegisteredHotkeysInfo fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_getRegisteredHotkeysInfo)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"getRegisteredHotkeysInfo");
			}
			return (*fp)(iLockStatus);
		}else{
			fp=NULL;
		}
	}
	static CCriticalSection csHotkeysData;
	if(iLockStatus>0){
		csHotkeysData.Lock();
	}
	if(iLockStatus<0){
		csHotkeysData.Unlock();
	}
	static CRegisteredHotkeys aHotkeysData;
	return aHotkeysData;
}

BOOL GetRegisteredHotkeyDsc(int iHotkeyNum, HWND hWin, CIHotkey* oHotkey, CRegisteredHotkeyData* psHotkeyDsc)
{
	BOOL bRes=0;
	CRegisteredHotkeyData oHotkeyInfo;
	getRegisteredHotkeysInfo(1);
	if(getRegisteredHotkeysInfo(0).Lookup(CRegisteredHotkeyKey(iHotkeyNum,hWin),oHotkeyInfo)){
		if(oHotkey){
			*oHotkey=oHotkeyInfo.oHotkey;
		}
		if(psHotkeyDsc){
			*psHotkeyDsc=oHotkeyInfo;
		}
		bRes=TRUE;
	}
	getRegisteredHotkeysInfo(-1);
	return bRes;
}

typedef int (WINAPI *_WKIHOTKEYS_GetHotkeyDscByID)(char const* szEventID,char* szOut,int iOut);
int WINAPI WKIHOTKEYS_GetHotkeyDscByID(char const* szEventID,char* szOut,int iOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_GetHotkeyDscByID fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_GetHotkeyDscByID)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_GetHotkeyDscByID");
			}
			return (*fp)(szEventID,szOut,iOut);
		}else{
			fp=NULL;
		}
	}
	BOOL bRes=0;
	CRegisteredHotkeyData aHotkeyData;
	CRegisteredHotkeys& aaKeys=getRegisteredHotkeysInfo(1);
	POSITION pos=aaKeys.GetStartPosition();
	while(pos){
		CRegisteredHotkeyKey iInfo;
		aaKeys.GetNextAssoc(pos,iInfo,aHotkeyData);
		if(strcmp(aHotkeyData.sHotkeyId,szEventID)==0){
			strcpy(szOut,aHotkeyData.sHotkeyDsc);
			bRes=1;
			break;
		}
	}
	getRegisteredHotkeysInfo(-1);
	return bRes;
}

typedef int (WINAPI *_WKIHOTKEYS_ActHotkeyByID)(char const* szEventID);
int WINAPI WKIHOTKEYS_ActHotkeyByID(char const* szEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _WKIHOTKEYS_ActHotkeyByID fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_WKIHOTKEYS_ActHotkeyByID)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"WKIHOTKEYS_ActHotkeyByID");
			}
			return (*fp)(szEventID);
		}else{
			fp=NULL;
		}
	}
	int iRes=0;
	CRegisteredHotkeyData aHotkeyData;
	CRegisteredHotkeys& aaKeys=getRegisteredHotkeysInfo(1);
	POSITION pos=aaKeys.GetStartPosition();
	while(pos){
		CRegisteredHotkeyKey iInfo;
		aaKeys.GetNextAssoc(pos,iInfo,aHotkeyData);
		if(strcmp(aHotkeyData.sHotkeyId,szEventID)==0){
			iRes=1;
			::PostMessage(iInfo.hWindow,WM_HOTKEY,iInfo.iID,0);
			break;
		}
	}
	getRegisteredHotkeysInfo(-1);
	return iRes;
}

void MarkHKListAsChanged()
{
	HWND hW=FindWindow("WK_MAIN",0);
	if(hW){
		SetProp(hW,"HKCHANGED",(HANDLE)1);
	}
}

void MarkHKListAsNotChanged()
{
	HWND hW=FindWindow("WK_MAIN",0);
	if(hW){
		SetProp(hW,"HKCHANGED",(HANDLE)0);
	}
}

BOOL GetMarkHKListChangedStatus()
{
	HWND hW=FindWindow("WK_MAIN",0);
	if(hW){
		return (int)GetProp(hW,"HKCHANGED");
	}
	return 0;
}

typedef BOOL  (WINAPI *_UnregisterIHotKeyDsc)(HWND hWnd, int id);
BOOL WINAPI UnregisterIHotKeyDsc(HWND hWnd, int id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BOOL bRes=getRegisteredHotkeysInfo(1).RemoveKey(CRegisteredHotkeyKey(id,hWnd));
	if(bRes)
	{
		MarkHKListAsChanged();
	}
	GetListOfCurrentlyRegisteredHotkeys(FALSE);
	getRegisteredHotkeysInfo(-1);
	return bRes;
}

BOOL UnregisterIHotKey_Single(HWND hWnd, int id)
{
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _UnregisterIHotKeyDsc fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_UnregisterIHotKeyDsc)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"UnregisterIHotKeyDsc");
			}
			return (*fp)(hWnd, id);
		}else{
			fp=NULL;
		}
	}
	BOOL bRes=UnregisterHotKey(hWnd, id);
	if(bRes){
		UnregisterIHotKeyDsc(hWnd, id);
	}
	return bRes;
}

BOOL UnregisterIHotKey(HWND hWnd, int id)
{
	UnregisterIHotKey_Single(hWnd, CODE_2PREFIX|id);
	UnregisterIHotKey_Single(hWnd, CODE_3PREFIX|id);
	UnregisterIHotKey_Single(hWnd, CODE_4PREFIX|id);
	return UnregisterIHotKey_Single(hWnd, CODE_1PREFIX|id);
}

typedef BOOL  (WINAPI *_RegisterIHotKeyDsc)(HWND hWnd , int id, CIHotkey& oHK, const char* szId, const char* szHotKeyDsc,BOOL bRebuildList);
BOOL WINAPI RegisterIHotKeyDsc(HWND hWnd, int id, CIHotkey& oHK, const char* szId, const char* szHotKeyDsc,BOOL bRebuildList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// Запрещенные гор. клавиши НЕ регистрируем!!!
	if(oHK.Options()==0 && (oHK==CIHotkey(VK_DELETE) || oHK==CIHotkey(VK_SPACE) || oHK==CIHotkey(VK_LBUTTON) || oHK==CIHotkey(VK_RBUTTON))){//|| oHK==CIHotkey(VK_ESCAPE)
		oHK.Reset();
	}
	if((oHK.Options()&ADDOPT_EVENT)==0 && oHK.m_dwBaseKey[0]>=KBBUFFER_DSIZE){
		oHK.Reset();
	}
	// Смотрим есть ли уже такая горячая клавиша?
	CRegisteredHotkeyData aHotkeyData;
	CRegisteredHotkeys& aaKeys=getRegisteredHotkeysInfo(1);
	POSITION pos=aaKeys.GetStartPosition();
	/*BOOL bRes=TRUE;
	while(pos){
		CRegisteredHotkeyKey iInfo;
		aaKeys.GetNextAssoc(pos,iInfo,aHotkeyData);
		// Точней не смотрим, потому что если их несколько - то и ничего страшного
		if(iInfo.iID==id && iInfo.hWindow==hWnd){// || aHotkeyData.oHotkey==oHK
			bRes=0;
			break;
		}
	}
	if(bRes)*/
	{
		// Ни клавиши ни кода с окном нету - регистрим
		aHotkeyData.oHotkey=oHK;
		aHotkeyData.sHotkeyDsc=szHotKeyDsc;
		aHotkeyData.sHotkeyId=szId;
		CRegisteredHotkeyKey rHk(id,hWnd);
		aaKeys.RemoveKey(rHk);
		aaKeys.SetAt(rHk,aHotkeyData);
		MarkHKListAsChanged();
		if(bRebuildList){
			GetListOfCurrentlyRegisteredHotkeys(TRUE);
		}
	}
	getRegisteredHotkeysInfo(-1);
	return TRUE;
}

BOOL RegisterIHotKey_Single(HWND hWnd, int id, CIHotkey& oHK, const char* szId, const char* szHotKeyDsc,BOOL bRebuildList)
{
#ifdef _NOHOTKEYS
	return 0;
#endif
	oHK.SetActionId(szId);
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _RegisterIHotKeyDsc fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_RegisterIHotKeyDsc)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"RegisterIHotKeyDsc");
			}
			return (*fp)(hWnd , id, oHK, szId, szHotKeyDsc,bRebuildList);
		}else{
			fp=NULL;
		}
	}
	if(oHK.m_dwBaseKey[1]>0){
		// Такие не поддерживаются при обычной регистрации!!!
		return FALSE;
	}
	BOOL bRes=0;
	WORD wHi=0,wLo=0;
	if(GetHotKeyForRegistration(oHK.GetAsDWORD(),wHi,wLo)){
		// Регистрим описание...
		RegisterIHotKeyDsc(hWnd, id, oHK, szId, szHotKeyDsc,bRebuildList);
		bRes=RegisterHotKey(hWnd, id, wHi,wLo);
	}
	return bRes;
}

BOOL RegisterIHotKey(HWND hWnd, int id, CIHotkey& oHK, const char* szId, const char* szHotKeyDsc,BOOL bRebuildList)
{
	oHK.NormalizeHotkeyChain();
	if(oHK.pNextHK){
		RegisterIHotKey_Single(hWnd , CODE_2PREFIX|id, *(oHK.pNextHK), szId, szHotKeyDsc, FALSE);
		if(oHK.pNextHK->pNextHK){
			RegisterIHotKey_Single(hWnd , CODE_3PREFIX|id, *(oHK.pNextHK->pNextHK), szId, szHotKeyDsc, FALSE);
			if(oHK.pNextHK->pNextHK->pNextHK){
				RegisterIHotKey_Single(hWnd , CODE_4PREFIX|id, *(oHK.pNextHK->pNextHK->pNextHK), szId, szHotKeyDsc, FALSE);
			}
		}
	}
	return RegisterIHotKey_Single(hWnd, CODE_1PREFIX|id, oHK, szId, szHotKeyDsc, bRebuildList);
}

typedef BOOL (WINAPI *_GetKBBufferState)(BYTE*);
typedef BOOL (WINAPI *_SetKBBlockKeys)(DWORD* baBlockKeys);
typedef BOOL (WINAPI *_RebuildKeyboardStates)(DWORD* baBlockKeys);
BOOL GetKeyboardStateBuffer(BOOL bLowLevel, BYTE* btOut)
{
	if(bLowLevel){
		// Если подключена библиотека с хуком - берем буфер из нее
		HINSTANCE hLLHookInst=GetModuleHandle("wkkeyh.dll");
		static _GetKBBufferState fp=NULL;
		if(hLLHookInst){
			fp=(_GetKBBufferState)DBG_GETPROC(hLLHookInst,"GetKBBufferState");
			while((*fp)(btOut)==FALSE){
				Sleep(50);
			}
			return TRUE;
		}else{
			fp=NULL;
		}
	}
	memset(btOut,0,KBBUFFER_DSIZE*sizeof(btOut[0]));
	::GetKeyboardState(btOut);
	TRACE1("::GetKeyboardState(btOut); bLowLevel=%i",bLowLevel);
	return FALSE;
}

typedef CIHotkey (WINAPI *_getCurrentlyPressedKeys)(BOOL);
CIHotkey WINAPI getCurrentlyPressedKeys(BOOL bGetKeyboardState_WinAPI)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _getCurrentlyPressedKeys fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_getCurrentlyPressedKeys)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"getCurrentlyPressedKeys");
			}
			return (*fp)(FALSE);
		}else{
			fp=NULL;
		}
	}
	int iCounter=0;
	CIHotkey oHKRes;
	BYTE btKBBuffer[KBBUFFER_DSIZE];
	BOOL bReallyLL=GetKeyboardStateBuffer(!bGetKeyboardState_WinAPI,btKBBuffer);
	if(bReallyLL){
		oHKRes.Options()|=ADDOPT_LOWLEVEL;
	}
	// Определяем что это за комбинация
	for(int i=0;i<KBBUFFER_DSIZE;i++){
		if((btKBBuffer[i] & 0x80)!=0){
			if((i!=VK_MENU)&&(i!=VK_SHIFT)&&(i!=VK_CONTROL)&&(i!=VK_LCONTROL)&&(i!=VK_RCONTROL)&&(i!=VK_RSHIFT)&&(i!=VK_LSHIFT)&&(i!=VK_RMENU)&&(i!=VK_LMENU)&&(i!=VK_RWIN)&&(i!=VK_LWIN)){
				oHKRes.m_dwBaseKey[iCounter]=i;
				iCounter++;
#ifndef _USRDLL
				if(WKIHOTKEYS_CheckForExternalDLL() || WKIHOTKEYS_GetExternalDLLInstance()==NULL){
					break;
				}
#endif
				if(iCounter>=MAX_KEYS){
					break;
				}
			}
		}
	}
	// Модификаторы
	if(!WKIHOTKEYS_DistingLRKeys()){
		if((btKBBuffer[VK_MENU] & 0x80)!=0 || ((btKBBuffer[VK_RMENU] & 0x80)!=0) || ((btKBBuffer[VK_LMENU] & 0x80)!=0)){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_ALT);
		}
		if((btKBBuffer[VK_CONTROL] & 0x80)!=0 || ((btKBBuffer[VK_LCONTROL] & 0x80)!=0) || ((btKBBuffer[VK_RCONTROL] & 0x80)!=0)){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_CONTROL);
		}
		if((btKBBuffer[VK_SHIFT] & 0x80)!=0 || ((btKBBuffer[VK_LSHIFT] & 0x80)!=0) || ((btKBBuffer[VK_RSHIFT] & 0x80)!=0)){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_SHIFT);
		}
		if(((btKBBuffer[VK_LWIN] & 0x80)!=0) || ((btKBBuffer[VK_RWIN] & 0x80)!=0)){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_EXT);
		}
	}else{
		if((btKBBuffer[VK_RMENU] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_RALT);
		}
		if((btKBBuffer[VK_LMENU] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_LALT);
		}
		if((btKBBuffer[VK_RCONTROL] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_RCONTROL);
		}
		if((btKBBuffer[VK_LCONTROL] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_LCONTROL);
		}
		if((btKBBuffer[VK_RSHIFT] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_RSHIFT);
		}
		if((btKBBuffer[VK_LSHIFT] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_LSHIFT);
		}
		if((btKBBuffer[VK_RWIN] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_REXT);
		}
		if((btKBBuffer[VK_LWIN] & 0x80)!=0){
			oHKRes.m_dwModifiers=(oHKRes.m_dwModifiers | HOTKEYF_LEXT);
		}
	}
	return oHKRes;
}

CIHotkey::~CIHotkey()
{
	if(pNextHK){
		delete pNextHK;
		pNextHK=0;
	}
	delete sExcludedInApps;
	delete sExclusiveInApps;
}

CIHotkey::CIHotkey()
{
	sExcludedInApps=0;
	sExclusiveInApps=0;
	pNextHK=0;
	Reset();
};

CIHotkey::CIHotkey(DWORD dwVal)
{
	sExcludedInApps=0;
	sExclusiveInApps=0;
	pNextHK=0;
	Reset();
	m_dwBaseKey[0]=LOWORD(dwVal);
	m_dwModifiers=HIWORD(dwVal);
};

CIHotkey::CIHotkey(DWORD dwBaseKeys, DWORD dwModifiers)
{
	sExcludedInApps=0;
	sExclusiveInApps=0;
	pNextHK=0;
	Reset();
	m_dwBaseKey[0]=dwBaseKeys;
	m_dwModifiers=dwModifiers;
};

CIHotkey::CIHotkey(CIHotkey& oVal)
{
	sExcludedInApps=0;
	sExclusiveInApps=0;
	pNextHK=0;
	*this=oVal;
};

DWORD& CIHotkey::Options()
{
	return m_dwAdditionalOptions;
};


BOOL CIHotkey::isSeq()
{
	return (Options()&ADDOPT_SEQUENCE)!=0?1:0;
}

BOOL CIHotkey::isOnUnp()
{
	return (Options()&ADDOPT_ON_UNPRESS)!=0?1:0;
}

void CIHotkey::Clear()
{
	m_dwModifiers=0;
	m_dwAdditionalOptions=0;
	memset(m_dwBaseKey,0,sizeof(m_dwBaseKey[0])*MAX_KEYS);
}

void CIHotkey::Reset()
{
	if(!sExcludedInApps){
		delete sExcludedInApps;
	}
	if(!sExclusiveInApps){
		delete sExclusiveInApps;
	}
	sExcludedInApps=new CString();
	sExclusiveInApps=new CString();
	m_szActionId[0]=0;
	Clear();
	if(pNextHK){
		delete pNextHK;
		pNextHK=0;
	}
}

void MoveBitsByMask(DWORD& dwFrom, DWORD& dwTo, DWORD dwMask)
{
	dwTo=(dwTo & (!dwMask));
	dwTo=(dwTo | (dwFrom & dwMask));
	return;
}

void SetSameBitsByMask(DWORD& dwFrom, DWORD& dwTo, DWORD dwMask)
{
	DWORD dwCommonPart=((dwFrom & dwMask)|(dwTo & dwMask));
	dwFrom|=dwCommonPart;
	dwTo|=dwCommonPart;
	return;
}

void BalanceTwoModifiers(DWORD& dwThisModifiers,DWORD& dwThatModifiers)
{
	/*
	bool bWinPHL=(dwThisModifiers & HOTKEYF_PRNHOLD)?true:false;
	bool bWinPHR=(dwThatModifiers & HOTKEYF_PRNHOLD)?true:false;
	if(bWinPHL || bWinPHR){
		SetSameBitsByMask(dwThisModifiers,dwThatModifiers,HOTKEYF_PRNHOLD);
	}
	*/
	bool bWinEqL=(dwThisModifiers & HOTKEYF_EXT)&&((dwThatModifiers & HOTKEYF_REXT)||(dwThatModifiers & HOTKEYF_LEXT));
	bool bWinEqR=(dwThatModifiers & HOTKEYF_EXT)&&((dwThisModifiers & HOTKEYF_REXT)||(dwThisModifiers & HOTKEYF_LEXT));
	if(bWinEqL || bWinEqR){
		SetSameBitsByMask(dwThisModifiers,dwThatModifiers,HOTKEYF_EXT|HOTKEYF_REXT|HOTKEYF_LEXT);
	}
	
	bool bAltEqL=(dwThisModifiers & HOTKEYF_ALT)&&((dwThatModifiers & HOTKEYF_RALT)||(dwThatModifiers & HOTKEYF_LALT));
	bool bAltEqR=(dwThatModifiers & HOTKEYF_ALT)&&((dwThisModifiers & HOTKEYF_RALT)||(dwThisModifiers & HOTKEYF_LALT));
	if(bAltEqL || bAltEqR){
		SetSameBitsByMask(dwThisModifiers,dwThatModifiers,HOTKEYF_ALT|HOTKEYF_RALT|HOTKEYF_LALT);
	}
	
	bool bShiftEqL=(dwThisModifiers & HOTKEYF_SHIFT)&&((dwThatModifiers & HOTKEYF_RSHIFT)||(dwThatModifiers & HOTKEYF_LSHIFT));
	bool bShiftEqR=(dwThatModifiers & HOTKEYF_SHIFT)&&((dwThisModifiers & HOTKEYF_RSHIFT)||(dwThisModifiers & HOTKEYF_LSHIFT));
	if(bShiftEqL || bShiftEqR){
		SetSameBitsByMask(dwThisModifiers,dwThatModifiers,HOTKEYF_SHIFT|HOTKEYF_RSHIFT|HOTKEYF_LSHIFT);
	}
	
	bool bCtrlEqL=(dwThisModifiers & HOTKEYF_CONTROL)&&((dwThatModifiers & HOTKEYF_RCONTROL)||(dwThatModifiers & HOTKEYF_LCONTROL));
	bool bCtrlEqR=(dwThatModifiers & HOTKEYF_CONTROL)&&((dwThisModifiers & HOTKEYF_RCONTROL)||(dwThisModifiers & HOTKEYF_LCONTROL));
	if(bCtrlEqL || bCtrlEqR){
		SetSameBitsByMask(dwThisModifiers,dwThatModifiers,HOTKEYF_CONTROL|HOTKEYF_RCONTROL|HOTKEYF_LCONTROL);
	}
}

BOOL CIHotkey::isEqual(CIHotkey& oKey,BOOL bCheckNx)
{
	if(oKey.m_dwModifiers!=m_dwModifiers){
		// Может быть так что в одной клавише Win а в другой LWin/RWin
		// Такие гор. клавиши все равно совпадают
		DWORD dwThisModifiers=m_dwModifiers;
		DWORD dwThatModifiers=oKey.m_dwModifiers;
		BalanceTwoModifiers(dwThisModifiers,dwThatModifiers);
		if(dwThatModifiers!=dwThisModifiers){
			return FALSE;
		}
	}
	// Особый случай, когда модификаторы не пусты
	// а клавиш не задано....
	if(m_dwModifiers!=0 && oKey.m_dwModifiers!=0
		&& (m_dwBaseKey[0]==0) && (oKey.m_dwBaseKey[0]==0)){
		return TRUE;
	}
	if((Options() & ADDOPT_ON_WPRESSED)!=0 || (oKey.Options() & ADDOPT_ON_WPRESSED)!=0){
		// Для таких клавиш многонажатия не имею смысла
		if((m_dwBaseKey[0]==0 || m_dwBaseKey[0]==VK_2X || m_dwBaseKey[0]==VK_3X) 
			&& (oKey.m_dwBaseKey[0]==0 || oKey.m_dwBaseKey[0]==VK_2X || oKey.m_dwBaseKey[0]==VK_3X)){
		return TRUE;
	}
	}
	BOOL bSkipNx=0;
	if(bCheckNx){
		BOOL bIsNxLeft=((IsRawKeyInHotkey(VK_2X)||IsRawKeyInHotkey(VK_3X))?1:0);
		BOOL bIsNxRight=((oKey.IsRawKeyInHotkey(VK_2X)||oKey.IsRawKeyInHotkey(VK_3X))?1:0);
		if(bIsNxLeft && !bIsNxRight){
			return FALSE;
		}
		bSkipNx=(!bIsNxLeft && bIsNxRight);
	}
	int iRCount=0;
	for(int i=0;i<MAX_KEYS;i++){
		if(bSkipNx && (oKey.m_dwBaseKey[iRCount]==VK_2X || oKey.m_dwBaseKey[iRCount]==VK_3X)){
			iRCount++;
			if(iRCount==MAX_KEYS){
				break;
			}
		}
		if(m_dwBaseKey[i]!=oKey.m_dwBaseKey[iRCount]){
			return FALSE;
		}
		if(m_dwBaseKey[i]==0){
			break;
		}
		iRCount++;
	}
	return TRUE;
}

BOOL CIHotkey::operator==(CIHotkey& oKey)
{
	return isEqual(oKey,0);	
};

BOOL CIHotkey::operator!=(CIHotkey& oKey)
{
	return !(oKey==(*this));
};

void CIHotkey::SetActionId(const char* szId)
{
	memset(m_szActionId,0,sizeof(m_szActionId));
	if(strlen(szId)>sizeof(m_szActionId)){
		memcpy(m_szActionId,szId,sizeof(m_szActionId)-1);
	}else{
		strcpy(m_szActionId,szId);
	}
}

CIHotkey& CIHotkey::CopyKeysOnly(const CIHotkey& CopyObj)
{
	int iRCount=0;
	for(int i=0;i<MAX_KEYS && iRCount<MAX_KEYS;i++){
		if(m_dwBaseKey[i]==0){
			m_dwBaseKey[i]=CopyObj.m_dwBaseKey[iRCount++];
		}
	}
	return *this;
}

CIHotkey& CIHotkey::DuplicateHotkey(const CIHotkey& CopyObj)
{
	Clear();
	m_dwModifiers=CopyObj.m_dwModifiers;
	m_dwAdditionalOptions=CopyObj.m_dwAdditionalOptions;
	if(!sExcludedInApps){
		sExcludedInApps=new CString();
	}
	if(!sExclusiveInApps){
		sExclusiveInApps=new CString();
	}
	(*sExcludedInApps)=(CopyObj.sExcludedInApps?(*CopyObj.sExcludedInApps):"");
	(*sExclusiveInApps)=(CopyObj.sExclusiveInApps?(*CopyObj.sExclusiveInApps):"");
	CopyKeysOnly(CopyObj);
	return *this;
}

CIHotkey& CIHotkey::CopyHotkey(const CIHotkey& CopyObj)
{
	strcpy(m_szActionId,CopyObj.m_szActionId);
	DuplicateHotkey(CopyObj);
	return *this;
}

CIHotkey& CIHotkey::operator=(const CIHotkey& CopyObj)
{
	CopyHotkey(CopyObj);
	if(pNextHK){
		CIHotkey* pTmp=pNextHK;
		pNextHK=0;
		delete pTmp;
	}
	if(CopyObj.pNextHK){
		pNextHK=new CIHotkey();
		*pNextHK=*CopyObj.pNextHK;
	}
	return *this;
};

BOOL CIHotkey::Present()
{
	if(m_dwModifiers!=0){
		if(m_dwBaseKey[0]==0){
			WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
			if(WKIHOTKEYS_GetExternalDLLInstance()==0){
				return 0;
			}
			//m_dwBaseKey[0]=VK_NULL;
		}
		return TRUE;
	}
	for(int i=0;i<MAX_KEYS;i++){
		// Важно проверять ДО проверки на 0
		// в случае EVENTа (FAKEKEY) зануление не должно ничего затереть
		if((Options()&ADDOPT_EVENT)==0 && m_dwBaseKey[i]>=KBBUFFER_DSIZE){
			m_dwBaseKey[i]=0;
		}
		if(m_dwBaseKey[i]!=0){
			return TRUE;
		}
	}
	return FALSE;
};

BOOL CIHotkey::IsEventsInList(CStringArray& evMap, CIHotkey& hkWith)
{
	//1) Проверяем совпадения @LONGPLAY
	CIHotkey hkThisExplained;
	hkThisExplained.Options()|=ADDOPT_SEQUENCE;
	for(int i=0;i<MAX_KEYS;i++){
		char szEventID[EVENT_KEYS]={0},szClass[32]={0};
		GetBytesFromDword(szEventID,&m_dwBaseKey[i]);
		// Вытаскиваем его класс...
		BOOL bAddToExplained=1;
		if(WKIHOTKEYS_GetExternEventClass(szEventID,szClass)){
			if(strstr(szClass,"@LONGPLAY")!=0){
				bAddToExplained=0;
				// Есть ли оно в карте?
				BOOL bFound=0;
				for(int j=0;j<evMap.GetSize();j++){
					if(evMap[j]==szEventID){
						bFound=1;
						break;
					}
				}
				if(!bFound){
					// Нету!!!
					return 0;
				}
			}
		}
		if(bAddToExplained){
			hkThisExplained.AddOneKeySeq(m_dwBaseKey[i],0);
		}
	}
	//2) Равенство остальных ХК
	return hkThisExplained==hkWith;
	/*BOOL bRes=isEvent();
	int iLen=evMap.GetSize();
	if(iLen==0){
		return !bRes;
	}
	if(!bRes){
		return FALSE;
	}
	// Теперь можно сказать: Точно есть ивенты и в горячей клавише и в глобальном списке
	int i=0;
	int iCountOfTheSameClassGlobal=0;
	if(bHardScheme==-1){
		bHardScheme=((Options() & ADDOPT_SOFT_EVENTS)==0);
	}
	BOOL bCheckOrder=0;
	CStringArray evHotkeys;
	CStringArray evHotkeysClass;
	for(i=0;i<MAX_KEYS-EVENT_KEYS_LEN;i++){
		if(Options()&ADDOPT_EVENT){
			char szEventID[EVENT_KEYS]={0},szClass[32]={0};
			GetBytesFromDword(szEventID,&m_dwBaseKey[i+1]);
			// Вытаскиваем его класс...
			if(WKIHOTKEYS_GetExternEventClass(szEventID,szClass)){
				evHotkeys.Add(szEventID);
				evHotkeysClass.Add(szClass);
				if(strstr(szClass,"@CH")==0){
					bCheckOrder=1;
				}
			}
			i=i+EVENT_KEYS_LEN-1;
			continue;
		}
		if(m_dwBaseKey[i]==0){
			break;
		}
	}
	CStringArray evEventToRemoveFromGlobal;
	// Сравниваем. По жесткой схеме - когдав пересекающихся классах все элементы должны
	// совпадать. По мягкой - достаточно вхождения элементов гор. клавиши в карту общих событий
	// Если нету @CH - то важен также порядок, указанный в гор.клавише и общей карте
	CStringArray evMapTClass;
	char szClass[64]={0};
	int iLastFount=0;
	for(i=0;i<evMap.GetSize();i++){
		BOOL bThisIsUniqElem=TRUE;
		if(!bCheckOrder){
			iLastFount=0;
		}
		for(int j=iLastFount;j<evHotkeys.GetSize();j++){
			if(evHotkeys[j]==evMap[i]){
				iLastFount=j;
				if(evHotkeysClass[j].Find("@LP")==-1){
					evEventToRemoveFromGlobal.Add(evHotkeys[j]);
				}
				bThisIsUniqElem=FALSE;
				evHotkeys.RemoveAt(j);
				evHotkeysClass.RemoveAt(j);
				break;
			}
		}
		if(bThisIsUniqElem && WKIHOTKEYS_GetExternEventClass(evMap[i],szClass)){
			evMapTClass.Add(szClass);
		}
	}
	bRes=(evHotkeys.GetSize()==0);
	if(bHardScheme && bRes){
		// Нужно еще убедиться что в сухом остатке нету ивентов с классом, пересекающимся с ивентами хоткея
		for(int k=0;k<evMapTClass.GetSize();k++){
			for(int l=0;l<evHotkeysClass.GetSize();l++){
				if(evHotkeysClass[l]==evMapTClass[k]){
					bRes=FALSE;
					break;
				}
			}
		}
	}
	// Удаляем те ивенты, которые заставили сработать гор. клавишу
	if(bRes){
		for(int i=0;i<evEventToRemoveFromGlobal.GetSize();i++){
			for(int j=0;j<evMap.GetSize();j++){
				if(evEventToRemoveFromGlobal[i]==evMap[j]){
					evMap.RemoveAt(j);
					break;
				}
			}
		}
	}
	return bRes;*/
}

BOOL CIHotkey::isEvent()
{
	if(Options()&ADDOPT_EVENT){
		return 1;
	}
	return FALSE;
};

CIHotkey CIHotkey::GetModifiers()
{
	return CIHotkey(0,m_dwModifiers);
};

void CIHotkey::SetModifiers(CIHotkey oNewKeys)
{
	m_dwModifiers = oNewKeys.m_dwModifiers;
};

CIHotkey CIHotkey::GetBaseKeys()
{
	CIHotkey ht(*this);
	ht.SetModifiers(0);
	return ht;
};

void CIHotkey::SetBaseKeys(CIHotkey oNewKeys)
{
	for(int i=0;i<MAX_KEYS;i++){
		m_dwBaseKey[i]=oNewKeys.m_dwBaseKey[i];
	}
	return;
};

DWORD CIHotkey::GetAsDWORD()
{
	if(isEvent()){
		return VK_EVENTKEY;
	}
	return m_dwBaseKey[0] | (m_dwModifiers<<16);
}

#define MULTIHK_DELIM	"(or)"
#define UNPRESS_DELIM	"@"
#define TRANSP_DELIM	"~"
#define SEQUEN_DELIM	"S"
#define PUNP_DELIM		"P"
#define EVENT_DELIM		"E"
#define EXCUD_APPS		"excludapps"
#define EXCUS_APPS		"exclusapps"
CString CIHotkey::GetForSave(BOOL bSingle)
{
	CString sData;
	sData.Format("%%%lu%s-%lu",m_dwModifiers,CString(((Options()&ADDOPT_ON_UNPRESS)!=0)?UNPRESS_DELIM:"")
		+CString(((Options()&ADDOPT_TRANSPARENT)!=0)?TRANSP_DELIM:"")
		+CString(((Options()&ADDOPT_SEQUENCE)!=0)?SEQUEN_DELIM:"")
		+CString(((Options()&ADDOPT_ON_WPRESSED)!=0)?PUNP_DELIM:"")
		+CString(((Options()&ADDOPT_EVENT)!=0)?EVENT_DELIM:"")
		,m_dwBaseKey[0]);
	for(int i=1;i<MAX_KEYS;i++){
		if(m_dwBaseKey[i]>0){
			CString sKey;
			sKey.Format(",%lu",m_dwBaseKey[i]);
			sData+=sKey;
		}
	}
	if(!bSingle && pNextHK){
		CIHotkey* pPos=pNextHK;
		while(pPos){
			sData+=MULTIHK_DELIM;
			sData+=pPos->GetForSave(TRUE);
			pPos=pPos->pNextHK;
		}
	}
	if(sExclusiveInApps && sExclusiveInApps->GetLength()){
		sData=sData+"["+EXCUS_APPS+":"+(*sExclusiveInApps)+":"+EXCUS_APPS+"]";
	}
	if(sExcludedInApps && sExcludedInApps->GetLength()){
		sData=sData+"["+EXCUD_APPS+":"+(*sExcludedInApps)+":"+EXCUD_APPS+"]";
	}
	return sData;
}

BOOL CIHotkey::InitFromSave(CString sData,BOOL bSingle)
{
	Reset();
	int exApps=0;
	int exAppsMin=0;
	exAppsMin=sData.Find(EXCUS_APPS);
	if(exAppsMin>0 && sExclusiveInApps){
		if(exApps<exAppsMin){
			exApps=exAppsMin;
		}
		(*sExclusiveInApps)=(CStringSubstr::getTag(CString("[")+EXCUS_APPS+":",CString(":")+EXCUS_APPS+"]",sData));
	}
	exAppsMin=sData.Find(EXCUD_APPS);
	if(exAppsMin>0 && sExcludedInApps){
		if(exApps<exAppsMin){
			exApps=exAppsMin;
		}
		(*sExcludedInApps)=(CStringSubstr::getTag(CString("[")+EXCUD_APPS+":",CString(":")+EXCUD_APPS+"]",sData));
	}
	if(exApps>0){
		sData=sData.Left(exApps-1);
	}

	if(sData[0]=='%'){
		int iFrom=sData.Find("-");
		CString sModif="";
		CString sBaseKey="";
		if(iFrom>0){
			sModif=sData.Mid(1,iFrom-1);
			sBaseKey=sData.Mid(iFrom+1);
		}else{
			sModif=sData.Mid(1);
		}
		
		if(sModif.Find(EVENT_DELIM)!=-1){
			Options()|=ADDOPT_EVENT;
		}
		if(sModif.Find(UNPRESS_DELIM)!=-1){
			Options()|=ADDOPT_ON_UNPRESS;
		}
		if(sModif.Find(TRANSP_DELIM)!=-1){
			Options()|=ADDOPT_TRANSPARENT;
		}
		if(sModif.Find(SEQUEN_DELIM)!=-1){
			Options()|=ADDOPT_SEQUENCE;
		}
		if(sModif.Find(PUNP_DELIM)!=-1){
			Options()|=ADDOPT_ON_WPRESSED;
		}
		m_dwModifiers=atol(sModif);
		// Остальные буквы гор. комбинации
		int iCounter=0;
		while(sBaseKey!="" && iCounter<MAX_KEYS){
			if(sBaseKey=="" || sBaseKey.Find(MULTIHK_DELIM)==0){
				break;
			}
			m_dwBaseKey[iCounter]=atol(sBaseKey);
			// Убираем цифры...
			sBaseKey.TrimLeft("0123456789");
			// Убираем запятую...
			sBaseKey.TrimLeft(",");
			sBaseKey.TrimLeft();
			iCounter++;
		}
	}else{
		DWORD dwVal=atol(sData);
		m_dwBaseKey[0]=LOWORD(dwVal);
		m_dwModifiers=HIWORD(dwVal);
	}
	if(pNextHK){
		delete pNextHK;
		pNextHK=0;
	}
	if(!bSingle){
		int iSecondKeyPos=sData.Find(MULTIHK_DELIM);
		if(iSecondKeyPos!=-1){
			pNextHK=new CIHotkey();
			CString sNewDescription=sData.Mid(iSecondKeyPos+strlen(MULTIHK_DELIM));
			pNextHK->InitFromSave(sNewDescription,FALSE);
		}
	}
	return TRUE;
}

BOOL CIHotkey::InitFromWinWORD(WORD wd)
{
	Reset();
	m_dwBaseKey[0]=LOBYTE(wd);
	WORD wHi=HIBYTE(wd);
	//wHi=(wHi&MOD_ALT?HOTKEYF_ALT:0)|(wHi&MOD_CONTROL?HOTKEYF_CONTROL:0)|(wHi&MOD_SHIFT?HOTKEYF_SHIFT:0)|(wHi&MOD_WIN?HOTKEYF_EXT:0);
	m_dwModifiers=wHi;
	return  TRUE;
}


DWORD aDownOnlyKeys[]={WHEEL_UP,WHEEL_DO,WHEEL_CL
	,VK_MCORNER_RT,VK_MCORNER_RB,VK_MCORNER_LT,VK_MCORNER_LB,VK_MBORDER_T,VK_MBORDER_B,VK_MBORDER_R,VK_MBORDER_L,VK_LBT_DBL,VK_MBT_DBL,VK_RBT_DBL
	,VK_MOB_ICO,VK_MOB_MAX,VK_MOB_MIN,VK_MOB_CLOSE,VK_MO_TITLE,VK_MOB_HLP,VK_MOB_MLES
	,0};
BOOL CIHotkey::DropKeyClassFromHotkey(DWORD dwHKClass)
{
	BOOL bRes=0;
	/*if((dwHKClass & EXTEVENT_KEYS)!=0){
		for(int i=0;i<MAX_KEYS-EVENT_KEYS_LEN;i++){
			if(m_dwBaseKey[i]==0){
				break;
			}
			if(m_dwBaseKey[i]==VK_FAKEKEY){
				for(int j=i; j<MAX_KEYS-EVENT_KEYS_LEN; j++){
					m_dwBaseKey[j]=m_dwBaseKey[j+EVENT_KEYS_LEN];
					m_dwBaseKey[j+EVENT_KEYS_LEN]=0;
				}
				i--;
				continue;
			}
		}
	}*/
	if((dwHKClass & DOWNONLY_KEYS)!=0){
		for(int i=0;i<MAX_KEYS;i++){
			if(m_dwBaseKey[i]==0){
				break;
			}
			for(int j=0;aDownOnlyKeys[j]!=0;j++){
				if(m_dwBaseKey[i]==aDownOnlyKeys[j]){
					bRes=1;
					for(int k=i;k<MAX_KEYS-1;k++){
						m_dwBaseKey[k]=m_dwBaseKey[k+1];
					}
					break;
				}
			}
		}
	}
	return bRes;
}

long CIHotkey::GetHKSimilarity(CIHotkey oKey, BOOL bStricktByModifiers, DWORD dwIgnoreDownOnlyKeys, BOOL bAnyOrderInKeys)
{
	long lRes=0;
	// Modifiers
	DWORD dwThisModifiers=m_dwModifiers;
	DWORD dwThatModifiers=oKey.m_dwModifiers;
	BalanceTwoModifiers(dwThisModifiers,dwThatModifiers);
	if(dwThisModifiers&HOTKEYF_RALT){
		if(dwThatModifiers&HOTKEYF_RALT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_LALT){
		if(dwThatModifiers&HOTKEYF_LALT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_LEXT){
		if(dwThatModifiers&HOTKEYF_LEXT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_REXT){
		if(dwThatModifiers&HOTKEYF_REXT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_RSHIFT){
		if(dwThatModifiers&HOTKEYF_RSHIFT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_LSHIFT){
		if(dwThatModifiers&HOTKEYF_LSHIFT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_LCONTROL){
		if(dwThatModifiers&HOTKEYF_LCONTROL){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_RCONTROL){
		if(dwThatModifiers&HOTKEYF_RCONTROL){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_CONTROL){
		if(dwThatModifiers&HOTKEYF_RCONTROL || dwThatModifiers&HOTKEYF_LCONTROL || dwThatModifiers&HOTKEYF_CONTROL){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_SHIFT){
		if(dwThatModifiers&HOTKEYF_RSHIFT || dwThatModifiers&HOTKEYF_LSHIFT || dwThatModifiers&HOTKEYF_SHIFT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_ALT){
		if(dwThatModifiers&HOTKEYF_RALT || dwThatModifiers&HOTKEYF_LALT || dwThatModifiers&HOTKEYF_ALT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	if(dwThisModifiers&HOTKEYF_EXT){
		if(dwThatModifiers&HOTKEYF_REXT || dwThatModifiers&HOTKEYF_LEXT || dwThatModifiers&HOTKEYF_EXT){
			lRes++;
		}else if(bStricktByModifiers){
			return 0;
		}
	}
	// Проверяем базовые клавиши
	int iBasekeysInCommon=0, iBaseKeysPresent=0;
	for(int i=0;i<MAX_KEYS;i++){
		if(m_dwBaseKey[i]==0){
			break;
		}
		if(m_dwBaseKey[i]==VK_PRESSNHOLD){
			i=i+1;
			continue;
		}
		if(dwIgnoreDownOnlyKeys){
			if((dwIgnoreDownOnlyKeys & DOWNONLY_KEYS)!=0){
				for(int j=0;aDownOnlyKeys[j]!=0;j++){
					if(m_dwBaseKey[i]==aDownOnlyKeys[j]){
						continue;
					}
				}
			}
		}
		if(!bAnyOrderInKeys){
			if(oKey.m_dwBaseKey[i]==m_dwBaseKey[i]){
				iBasekeysInCommon++;
			}else{
				return 0;
			}
		}else{
			// Проверяем каждый с каждым
			for(int j=0;j<MAX_KEYS;j++){
				if(oKey.m_dwBaseKey[j]==0){
					break;
				}
				if(oKey.m_dwBaseKey[j]==m_dwBaseKey[i]){
					iBasekeysInCommon++;
				}
			}
		}
		iBaseKeysPresent++;
	}
	if(iBasekeysInCommon==0 && iBaseKeysPresent>0){
		lRes=0;
	}else{
		lRes+=iBasekeysInCommon;
	}
	return lRes;
}

CCriticalSection csRebuildLock;
typedef CString& (WINAPI *_GetListOfCurrentlyRegisteredHotkeys)(BOOL);
CString& WINAPI GetListOfCurrentlyRegisteredHotkeys(BOOL bRebuild)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(WKIHOTKEYS_CheckForExternalDLL()){
		WKIHOTKEYS_GetExternalDLLInstance()=GetModuleHandle("WP_KeyMaster.wkp");
		static _GetListOfCurrentlyRegisteredHotkeys fp=NULL;
		if(WKIHOTKEYS_GetExternalDLLInstance()){
			if(fp==NULL){
				fp=(_GetListOfCurrentlyRegisteredHotkeys)DBG_GETPROC(WKIHOTKEYS_GetExternalDLLInstance(),"GetListOfCurrentlyRegisteredHotkeys");
			}
			return (*fp)(bRebuild);
		}else{
			fp=NULL;
		}
	}
	csRebuildLock.Lock();
	// Буффер для стоп-клавиш..
	DWORD baBlockKeys[KBBUFFER_DSIZE]={0};
	static CString m_AKeys="";
	if(bRebuild || m_AKeys==""){
		FLOG("GetListOfCurrentlyRegisteredHotkeys-rebuild");
		m_AKeys = "";
		int iCount=0;
		CRegisteredHotkeyKey iInfo;
		CRegisteredHotkeyData hkTest;
		CRegisteredHotkeys& hkMap=getRegisteredHotkeysInfo(1);
		POSITION pos=hkMap.GetStartPosition();
		char szTmp[1024]="";
		CStringArray aHKList;
		while(pos){
			hkMap.GetNextAssoc(pos,iInfo,hkTest);
			if(hkTest.oHotkey.Present()){
				sprintf(szTmp,"%s: %s",GetIHotkeyName(hkTest.oHotkey,FALSE),hkTest.sHotkeyDsc.SpanExcluding("\t"));
				//m_AKeys+=szTmp;
				aHKList.Add(szTmp);
				iCount++;
			}
			MapHotkeyToBlockBuffer(baBlockKeys,hkTest.oHotkey,TRUE);
		}
		if(iCount==0){
			m_AKeys ="<none>";
		}else{
			int i=0,j=0;
			for(i=0;i<aHKList.GetSize()-1;i++){
				for(j=i+1;j<aHKList.GetSize();j++){
					if(aHKList[i]>aHKList[j]){
						CString sTmp=aHKList[i];
						aHKList[i]=aHKList[j];
						aHKList[j]=sTmp;
					}
				}
			}
			for(i=0;i<aHKList.GetSize();i++){
				CString sLine;
				sLine.Format("%lu. %s\r\n",i+1,aHKList[i]);
				m_AKeys +=sLine;
			}
		}
		getRegisteredHotkeysInfo(-1);
		// Устанавливаем блок-клавиши?
		BOOL bRebuildOK=0;
		HINSTANCE hLLHookInst=GetModuleHandle("wkkeyh.dll");
		if(!hLLHookInst && GetModuleHandle("WP_KeyMaster.wkp")){
			FLOG("GetListOfCurrentlyRegisteredHotkeys-rebuild almost failed");
			int i=50;
			while((i--)>0 && (hLLHookInst=GetModuleHandle("wkkeyh.dll"))==0){
				Sleep(200);
			}
		}
		static _SetKBBlockKeys fp=NULL;
		if(hLLHookInst){
			FLOG("GetListOfCurrentlyRegisteredHotkeys-rebuild2");
			// Даем плагинам обработать массив....
			HINSTANCE hKeyMaster=GetModuleHandle("WP_KeyMaster.wkp");
			if(hKeyMaster){
				static _RebuildKeyboardStates fprebuild=NULL;
				if(fprebuild==NULL){
					FLOG("GetListOfCurrentlyRegisteredHotkeys-prerebuild");
					fprebuild=(_RebuildKeyboardStates)DBG_GETPROC(hKeyMaster,"RebuildKeyboardStates");
				}
				if(fprebuild){
					FLOG("GetListOfCurrentlyRegisteredHotkeys-rebuild done");
					(*fprebuild)(baBlockKeys);
					bRebuildOK=1;
				}
			}
			fp=(_SetKBBlockKeys)DBG_GETPROC(hLLHookInst,"SetKBBlockKeys");
			if(fp){
				(*fp)(baBlockKeys);
			}
		}
		MarkHKListAsNotChanged();
#ifdef _DEBUG
		if(!bRebuildOK){
			FLOG("GetListOfCurrentlyRegisteredHotkeys-rebuild failed!!!");
		}
#endif
	}
	csRebuildLock.Unlock();
	return m_AKeys;
}

BOOL IsMouseKey(DWORD dwCode)
{
	//Это - не нажатия!!! -> dwCode==VK_MCORNER_RT || dwCode==VK_MCORNER_RB || dwCode==VK_MCORNER_LT || dwCode==VK_MCORNER_LB
	if(dwCode==VK_LBUTTON || dwCode==VK_RBUTTON || dwCode==VK_MBUTTON || 
		dwCode==VK_RBT_DBL || dwCode==VK_MBT_DBL || dwCode==VK_LBT_DBL || 
			dwCode==WHEEL_UP || dwCode==WHEEL_DO || dwCode==WHEEL_CL ||
			dwCode==VK_XBTTNWK1 || dwCode==VK_XBTTNWK2 || dwCode==VK_XBTTNWK3 || dwCode==VK_XBTTNWK4){
		return TRUE;
	}
	return FALSE;
}

BOOL CIHotkey::ContainMouseClicks()
{
	for(int i=0;i<MAX_KEYS;i++){
		if(IsMouseKey(m_dwBaseKey[i])){
			return TRUE;
		}
		if(m_dwBaseKey[i]==0){
			break;
		}
	}
	return FALSE;
}

BOOL CIHotkey::ContainKeyClicksToo(BOOL bCheckModifiers)
{
	if(bCheckModifiers && m_dwModifiers!=0){
		return TRUE;
	}
	int iKeyClicks=0;
	for(int i=0;i<MAX_KEYS;i++){
		if(m_dwBaseKey[i]==0){
			break;
		}
		if(!IsMouseKey(m_dwBaseKey[i])){
			iKeyClicks++;
		}
	}
	return (iKeyClicks>0);
}

BOOL CIHotkey::AddOneModifier(DWORD dwChar)
{
	if((Options()&ADDOPT_EVENT)!=0){
		Reset();
	}
	if(isSeq()){
		if(dwChar & HOTKEYF_RCONTROL){
			AddOneKey(VK_RCONTROL);
		}
		if(dwChar & HOTKEYF_LCONTROL){
			AddOneKey(VK_LCONTROL);
		}
		if(dwChar & HOTKEYF_CONTROL){
			AddOneKey(VK_CONTROL);
		}
		if(dwChar & HOTKEYF_RSHIFT){
			AddOneKey(VK_RSHIFT);
		}
		if(dwChar & HOTKEYF_LSHIFT){
			AddOneKey(VK_LSHIFT);
		}
		if(dwChar & HOTKEYF_SHIFT){
			AddOneKey(VK_SHIFT);
		}
		if(dwChar & HOTKEYF_RALT){
			AddOneKey(VK_RMENU);
		}
		if(dwChar & HOTKEYF_LALT){
			AddOneKey(VK_LMENU);
		}
		if(dwChar & HOTKEYF_ALT){
			AddOneKey(VK_MENU);
		}
		if(dwChar & HOTKEYF_REXT){
			AddOneKey(VK_RWIN);
		}
		if(dwChar & HOTKEYF_LEXT){
			AddOneKey(VK_LWIN);
		}
		if(dwChar & HOTKEYF_EXT){
			AddOneKey(VK_LWIN);
		}
		return 1;
	}
	// Check for  & HOTKEYF_VALUABLE_MASK?
	m_dwModifiers|=dwChar;
	if(dwChar==HOTKEYF_RCONTROL || dwChar==HOTKEYF_LCONTROL){
		m_dwModifiers&=~(HOTKEYF_CONTROL);
	}
	if(m_dwModifiers & HOTKEYF_CONTROL){
		m_dwModifiers&=~(HOTKEYF_RCONTROL | HOTKEYF_LCONTROL);
	}
	if(dwChar==HOTKEYF_RALT || dwChar==HOTKEYF_LALT){
		m_dwModifiers&=~(HOTKEYF_ALT);
	}
	if(m_dwModifiers & HOTKEYF_ALT){
		m_dwModifiers&=~(HOTKEYF_RALT | HOTKEYF_LALT);
	}
	if(dwChar==HOTKEYF_RSHIFT || dwChar==HOTKEYF_LSHIFT){
		m_dwModifiers&=~(HOTKEYF_SHIFT);
	}
	if(m_dwModifiers & HOTKEYF_SHIFT){
		m_dwModifiers&=~(HOTKEYF_RSHIFT | HOTKEYF_LSHIFT);
	}
	if(dwChar==HOTKEYF_REXT || dwChar==HOTKEYF_LEXT){
		m_dwModifiers&=~(HOTKEYF_EXT);
	}
	if(m_dwModifiers & HOTKEYF_EXT){
		m_dwModifiers&=~(HOTKEYF_REXT | HOTKEYF_LEXT);
	}
	return TRUE;
}

CString CIHotkey::IsEventOfProhibitedClassInHotkey(const char* szEventID)
{
	char szClass[64]={0},szClass2[64]={0},szEventId2[5]={0};
	WKIHOTKEYS_GetExternEventClass(szEventID,szClass);
	if(Options()&ADDOPT_EVENT){
		if(strstr(szClass,"@NSC")!=0){// Unique by class
			int iZeroPos=0;
			while(m_dwBaseKey[iZeroPos] && iZeroPos<=MAX_KEYS){
				GetBytesFromDword(szEventId2,&m_dwBaseKey[iZeroPos]);
				WKIHOTKEYS_GetExternEventClass(szEventId2,szClass2);
				if(strcmp(szClass2,szClass)==0){
					return szEventId2;
				}
				iZeroPos++;
			}
		}
	}
	return "";
}

BOOL CIHotkey::IsEventInHotkey(const char* szEventID)
{
	int iZeroPos=0;
	if(Options()&ADDOPT_EVENT){
	while(m_dwBaseKey[iZeroPos] && iZeroPos<=MAX_KEYS){
		if(m_dwBaseKey[iZeroPos]==GetDwordFromBytes(szEventID)){
			return TRUE;
		}
		iZeroPos++;
	}
	}
	return FALSE;
}

BOOL CIHotkey::AddOneEvent(const char* szEventID)
{
	if((Options()&ADDOPT_EVENT)==0){
		Reset();
	}
	Options()|=ADDOPT_EVENT;
	if(IsEventOfProhibitedClassInHotkey(szEventID).GetLength()>0){
		return 0;
	}
	int iZeroPos=0;
	char szClass[64]={0};
	WKIHOTKEYS_GetExternEventClass(szEventID,szClass);
	BOOL bCheckUniqity=(strstr(szClass,"@NU")==0);
	while(m_dwBaseKey[iZeroPos] && iZeroPos<=MAX_KEYS){
		if(bCheckUniqity && m_dwBaseKey[iZeroPos]==GetDwordFromBytes(szEventID)){
			return FALSE;
		}
		iZeroPos++;
	}
	m_dwBaseKey[iZeroPos]=GetDwordFromBytes(szEventID);
	return TRUE;
}

BOOL CIHotkey::NormalizeHotkeyChain()
{
	CIHotkey* pNext=this;//pNextHK;
	CIHotkey* pPrev=this;
	while(pNext!=0){
		//Вызов Present() важен - нормализует
		//клавиши с одними модификаторами
		if(pNext->Present()){
			pPrev=pNext;
			pNext=pNext->pNextHK;
		}else{
			if(pNext!=pPrev){
				// Так как начинаем сами с себя
				// возможно удалять и не нужно
				pPrev->pNextHK=pNext->pNextHK;
				pNext->pNextHK=0;
				delete pNext;
			}
			pNext=pPrev->pNextHK;
		}
	}
	return TRUE;
}

BOOL CIHotkey::AddOneKeySeq(DWORD dwChar,BOOL bIntoFirst)
{
	if(dwChar==VK_2X || dwChar==VK_3X){
		return 0;
	}
	if(!WKIHOTKEYS_DistingLRKeys()){
		if(dwChar==VK_LCONTROL || dwChar==VK_RCONTROL){
			dwChar=VK_CONTROL;
		}
		if(dwChar==VK_LSHIFT || dwChar==VK_RSHIFT){
			dwChar=VK_SHIFT;
		}
		if(dwChar==VK_LMENU || dwChar==VK_RMENU){
			dwChar=VK_MENU;
		}
	}
	if(bIntoFirst){
		for(int i=MAX_KEYS-1;i>=1;i--){
			m_dwBaseKey[i]=m_dwBaseKey[i-1];
		}
		m_dwBaseKey[0]=dwChar;
	}else{
		// Записываем в заданном порядке
		int iNewKeyCount=0;
		while(m_dwBaseKey[iNewKeyCount]!=0 && iNewKeyCount<MAX_KEYS){
			if(m_dwBaseKey[iNewKeyCount]==0){
				break;
			}
			iNewKeyCount++;
		}
		if(iNewKeyCount<MAX_KEYS){
			m_dwBaseKey[iNewKeyCount]=dwChar;
			return 1;
		}
	}
	return 0;
}

BOOL CIHotkey::AddOneKey(DWORD dwChar)
{
	if((Options()&ADDOPT_EVENT)!=0){
		Reset();
	}
	if(isSeq()){
		return AddOneKeySeq(dwChar,0);
	}
	if(dwChar==VK_LWIN || dwChar==VK_RWIN
		|| dwChar==VK_LSHIFT || dwChar==VK_RSHIFT || dwChar==VK_SHIFT
		|| dwChar==VK_LMENU || dwChar==VK_RMENU || dwChar==VK_MENU
		|| dwChar==VK_LCONTROL || dwChar==VK_RCONTROL || dwChar==VK_CONTROL){
		return 0;
	}
	int iNewKeyCount=0;
	while(m_dwBaseKey[iNewKeyCount]!=0 && iNewKeyCount<MAX_KEYS){
		if(m_dwBaseKey[iNewKeyCount]==dwChar){
			return FALSE;
		}
		if(m_dwBaseKey[iNewKeyCount]>dwChar){
			break;
		}
		iNewKeyCount++;
	}
	if(iNewKeyCount<MAX_KEYS){
		for(int i=MAX_KEYS-2;i>iNewKeyCount;i--){
			m_dwBaseKey[i]=m_dwBaseKey[i-1];
		}
		m_dwBaseKey[iNewKeyCount]=dwChar;
		return TRUE;
	}
	return FALSE;
}

BOOL CIHotkey::RemoveOneKey()
{
	for(int i=MAX_KEYS-1;i>=0;i--){
		if(m_dwBaseKey[i]!=0){
			m_dwBaseKey[i]=0;
			return TRUE;
		}
	}
	m_dwModifiers=0;
	return TRUE;
}

BOOL CIHotkey::isBigger(CIHotkey& hkCheck)
{
	BOOL bKeyBigger=IsKeysInHotkey(hkCheck,1);
	BOOL bModifBigger=1;
	if(!isSeq()){
		// Там где у нас нулики, не должно быть единиц у них
		DWORD dwUNasNuliki=!m_dwModifiers;
		bModifBigger=(dwUNasNuliki&hkCheck.m_dwModifiers)==0;
	}
	return bKeyBigger & bModifBigger;
}

// Проверяем что все кнопки чужой горклавиши есть в нашей (но в нашей может быть и больше)
BOOL CIHotkey::IsKeysInHotkey(CIHotkey& oNewKeys,BOOL bIgnoreNx)
{
	// Модификаторы не проверяются :(
	int iSameKeys=0;
	int iNewKeyCount=0,iRPosCount=0;
	while(oNewKeys.m_dwBaseKey[iRPosCount]!=0 && iRPosCount<MAX_KEYS){
		for(int i=0;i<MAX_KEYS;i++){
			if(m_dwBaseKey[i]==0){
				break;
			}
			if(bIgnoreNx){
				if(i<MAX_KEYS-1 && (m_dwBaseKey[i]==VK_2X || m_dwBaseKey[i]==VK_3X)){
					i++;
				}
				if(iRPosCount<MAX_KEYS-1 && (oNewKeys.m_dwBaseKey[iRPosCount]==VK_2X || oNewKeys.m_dwBaseKey[iRPosCount]==VK_3X)){
					iRPosCount++;
				}
			}
			if(m_dwBaseKey[i]==oNewKeys.m_dwBaseKey[iRPosCount]){
				iSameKeys++;
				break;
			}
		}
		if(oNewKeys.m_dwBaseKey[iRPosCount]==0){
			break;
		}
		iNewKeyCount++;
		iRPosCount++;
	}
	if(iSameKeys==iNewKeyCount && iSameKeys!=0){
		return TRUE;
	}
	return FALSE;
}

CString GetExeFromHwnd(HWND hWin);
BOOL CIHotkey::AppConditionsFail()
{
	BOOL bFail=FALSE;
	if(sExcludedInApps->GetLength()>0 || sExclusiveInApps->GetLength()>0){
		char szTitle[1024]="";
		HWND hCurWnd=::GetForegroundWindow();
		CString sExcl="";
		CString sExe="";
		if(hCurWnd){
			sExe=GetExeFromHwnd(hCurWnd);
			::GetWindowText(hCurWnd,szTitle,sizeof(szTitle));
			sExe+=" ";
			sExe+=szTitle;
		}
		sExe.MakeUpper();
		sExcl=(*sExclusiveInApps);
		if(sExcl.GetLength()){
			sExcl.MakeUpper();
			if(!PatternMatchList(sExe, sExcl,";")){
				bFail=TRUE;
			}
		}
		sExcl=(*sExcludedInApps);
		if(sExcl.GetLength()){
			sExcl.MakeUpper();
			if(PatternMatchList(sExe, sExcl,";")){
				bFail=TRUE;
			}
		}
	}
	return bFail;
}

BOOL CIHotkey::IsRawKeyInHotkey(DWORD dwKey)
{
	int iSameKeys=0;
	for(int i=0;i<MAX_KEYS;i++){
		if(m_dwBaseKey[i]==0){
			break;
		}
		if(m_dwBaseKey[i]==dwKey){
			iSameKeys++;
			break;
		}
	}
	if(iSameKeys!=0){
		return TRUE;
	}
	return FALSE;
}

BOOL CIHotkey::SkipMouseClicks()
{
	BOOL bRes=FALSE;
	for(int i=0;i<MAX_KEYS;i++){
		if(IsMouseKey(m_dwBaseKey[i])){
			// Скипаем клавишу!
			bRes=TRUE;
			for(int j=i;j<MAX_KEYS-1;j++){
				m_dwBaseKey[j]=m_dwBaseKey[j+1];
			}
			m_dwBaseKey[MAX_KEYS-1]=0;
		}
		if(m_dwBaseKey[i]==0){
			return bRes;
		}
	}
	return bRes;
}

BOOL CIHotkey::DropRawKeyInHotkey(DWORD dwKey)
{
	BOOL bRes=FALSE;
	for(int i=0;i<MAX_KEYS;i++){
		if(m_dwBaseKey[i]==dwKey){
			// Скипаем клавишу!
			bRes=TRUE;
			for(int j=i;j<MAX_KEYS-1;j++){
				m_dwBaseKey[j]=m_dwBaseKey[j+1];
			}
			m_dwBaseKey[MAX_KEYS-1]=0;
		}
		if(m_dwBaseKey[i]==0){
			return bRes;
		}
	}
	return bRes;
}