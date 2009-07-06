#if !defined(IPV6_HOTKEYS)
#define IPV6_HOTKEYS
#include "Hotkeys_def.h"
class CIHotkey
{
public:
	// Данные
	DWORD m_dwModifiers;
	DWORD m_dwBaseKey[MAX_KEYS];
	DWORD m_dwAdditionalOptions;
	CIHotkey* pNextHK;
	char m_szActionId[64];

	CString* sExclusiveInApps;
	CString* sExcludedInApps;
public:
	CIHotkey();
	CIHotkey(DWORD dwVal);
	CIHotkey(DWORD dwBaseKeys, DWORD dwModifiers);
	CIHotkey(CIHotkey& oVal);
	~CIHotkey();
	DWORD& Options();
	BOOL isSeq();
	BOOL isOnUnp();
	void Reset();// Удаляет все включая вторые третьи клавиши
	void Clear();// Очищает инфу с только этой клавиши
	BOOL isEqual(CIHotkey& oKey,BOOL bCheckNx);
	BOOL operator ==(CIHotkey& oKey);
	BOOL operator !=(CIHotkey& oKey);
	CIHotkey& operator=(const CIHotkey& CopyObj);
	CIHotkey& CopyHotkey(const CIHotkey& CopyObj);
	CIHotkey& CopyKeysOnly(const CIHotkey& CopyObj);
	CIHotkey& DuplicateHotkey(const CIHotkey& CopyObj);
	BOOL isBigger(CIHotkey& hkCheck);
	BOOL Present();
	BOOL isEvent();
	BOOL IsEventsInList(CStringArray& evMap, CIHotkey& hkWith);
	CIHotkey GetModifiers();
	void SetModifiers(CIHotkey oNewKeys);
	CIHotkey GetBaseKeys();
	void SetBaseKeys(CIHotkey oNewKeys);
	DWORD GetAsDWORD();
	CString GetForSave(BOOL bSingle=TRUE);
	BOOL InitFromSave(CString sData,BOOL bSingle=TRUE);
	BOOL InitFromWinWORD(WORD wd);
	long GetHKSimilarity(CIHotkey oNewKeys, BOOL bStricktByModifiers=TRUE, DWORD dwIgnoreDownOnlyKeys=FALSE, BOOL bAnyOrderInKeys=TRUE);
	BOOL DropKeyClassFromHotkey(DWORD dwHKClass);
	BOOL ContainMouseClicks();
	BOOL ContainKeyClicksToo(BOOL bCheckModifiers=TRUE);
	BOOL SkipMouseClicks();
	BOOL IsKeysInHotkey(CIHotkey& oNewKeys,BOOL bIgnoreNx=0);
	BOOL RemoveOneKey();
	BOOL AddOneKey(DWORD dwChar);
	BOOL AddOneKeySeq(DWORD dwChar,BOOL bIntoFirst);
	BOOL AddOneModifier(DWORD dwChar);
	BOOL AddOneEvent(const char* szEventID);
	BOOL NormalizeHotkeyChain();
	BOOL IsRawKeyInHotkey(DWORD dwKey);
	BOOL DropRawKeyInHotkey(DWORD dwKey);
	BOOL IsEventInHotkey(const char* szEventID);
	CString IsEventOfProhibitedClassInHotkey(const char* szEventID);
	void SetActionId(const char* szId);
	char* GetActionId(){return m_szActionId;};
	BOOL AppConditionsFail();
};

class CRegisteredHotkeyData
{
public:
	DWORD dwLastKeyThatActivatedHK;
	BOOL bAutoRepeat;
	BOOL bOnUnpress;
	CString sHotkeyId;
	CString sHotkeyDsc;
	CIHotkey oHotkey;
	CIHotkey oRealTriggeredHotkey;
	CRegisteredHotkeyData(){
		bOnUnpress=0;
		bAutoRepeat=0;
		dwLastKeyThatActivatedHK=0;
	}
	CRegisteredHotkeyData& operator=(const CRegisteredHotkeyData& CopyObj)
	{
		bOnUnpress=CopyObj.bOnUnpress;
		bAutoRepeat=CopyObj.bAutoRepeat;
		dwLastKeyThatActivatedHK=CopyObj.dwLastKeyThatActivatedHK;
		oRealTriggeredHotkey=CopyObj.oRealTriggeredHotkey;
		oHotkey=CopyObj.oHotkey;
		sHotkeyDsc=CopyObj.sHotkeyDsc;
		sHotkeyId=CopyObj.sHotkeyId;
		return *this;
	}
	CString GetDsc()
	{
		return sHotkeyDsc.SpanExcluding("\t");
	}
};

class CRegisteredHotkeyKey
{
public:
	int iID;
	HWND hWindow;
	CRegisteredHotkeyKey()
	{
		iID=0;
		hWindow=0;
	}
	CRegisteredHotkeyKey(int id, HWND wnd)
	{
		iID=id;
		hWindow=wnd;
	}
	CRegisteredHotkeyKey& operator=(const CRegisteredHotkeyKey& CopyObj)
	{
		iID=CopyObj.iID;
		hWindow=CopyObj.hWindow;
		return *this;
	}
	BOOL operator ==(const CRegisteredHotkeyKey& oKey) const
	{
		return (oKey.iID==iID && oKey.hWindow==hWindow);
	}
	operator DWORD()
	{
		return (DWORD)((DWORD(hWindow)<<16) & iID);
	}
};

class CRegisteredHotkeyPair
{
public:
	CRegisteredHotkeyKey key;
	CRegisteredHotkeyData value;
	CRegisteredHotkeyPair& operator=(const CRegisteredHotkeyPair& CopyObj)
	{
		key=CopyObj.key;
		value=CopyObj.value;
		return *this;
	}
};

// Extern events
class CExternEventClass
{
public:
	CExternEventClass(const char* in_szEventClassName)
	{
		pNext=0;
		memset(szEventClassName,0,sizeof(szEventClassName));
		if(strlen(in_szEventClassName)>sizeof(szEventClassName)-1){
			memcpy(szEventClassName,in_szEventClassName,sizeof(szEventClassName)-1);
		}else{
			strcpy(szEventClassName,in_szEventClassName);
		}
	}
	char szEventClassName[64];
	CExternEventClass* pNext;
};

class CExternEventClassList
{
public:
	CExternEventClass* g_pEventsClassesList;
	CExternEventClassList()
	{
		g_pEventsClassesList=0;
	}
	~CExternEventClassList()
	{
		CExternEventClass* pPos=g_pEventsClassesList;
		while(pPos){
			CExternEventClass* pPosNext=pPos->pNext;
			delete pPos;
			pPos=pPosNext;
		}
		g_pEventsClassesList=0;
	}
	CExternEventClass* AddEventClass(const char* szEventClassName);
	CExternEventClass* GetClass(int iIndex);
};

class CExternEvent
{
public:
	CExternEvent(const char* in_szEventID, const char* in_szShortDescription, const char* in_szEventClassName);
	char szEventID[EVENT_KEYS];
	char szEventName[16];
	CExternEvent* pNext;
	CExternEventClass* pEventClass;
};

class CExternEventList
{
public:
	CExternEvent* g_pEventsList;
	CExternEventList()
	{
		g_pEventsList=0;
	}
	~CExternEventList()
	{
		CExternEvent* pPos=g_pEventsList;
		while(pPos){
			CExternEvent* pPosNext=pPos->pNext;
			delete pPos;
			pPos=pPosNext;
		}
		g_pEventsList=0;
	}
	CExternEvent* GetEvent(int iIndex);
	CExternEvent* GetEvent(const char* szEventID);
	CExternEvent* AddEvent(const char* szEventID, const char* szShortDescription, const char* szEventClassName);
	BOOL RemEvent(const char* szEventID);
};

typedef int (WINAPI *FPONEVENTADD)(char const* szEventID);
typedef int (WINAPI *FPONEVENTREMOVE)(char const* szEventID);
typedef CMap<CRegisteredHotkeyKey,CRegisteredHotkeyKey&,CRegisteredHotkeyData,CRegisteredHotkeyData&> CRegisteredHotkeys;
const char* GetKeyNameByCode(WORD wLo, char szDefault[MAXHOTKEY_NAMELEN], WORD dwHiCode);
CString GetIHotkeyName(CIHotkey& oHotKey, BOOL bWithFollowers=FALSE, BOOL bShrinkIfBig=FALSE);
int MapHotkeyToBlockBuffer(DWORD* baBlockKeys, CIHotkey& oHotKey, BOOL bWithFollowers);
DWORD GetHotKeyForRegistration(DWORD dwHKey, WORD& wHi, WORD& wLo);
BOOL GetRegisteredHotkeyDsc(int iHotkeyNum, HWND hWin, CIHotkey* oHotkey, CRegisteredHotkeyData* sHotkeyDsc);
BOOL RegisterIHotKey(HWND hWnd , int id, CIHotkey& oHK, const char* szId, const char* szHotKeyDsc="",BOOL bRebuildList=TRUE);
BOOL UnregisterIHotKey(HWND hWnd, int id);
//Externs
CRegisteredHotkeys& WINAPI getRegisteredHotkeysInfo(int iLockStatus);
CString& WINAPI GetListOfCurrentlyRegisteredHotkeys(BOOL bRebuild=FALSE);
CIHotkey WINAPI getCurrentlyPressedKeys(BOOL bGetKeyboardState_WinAPI=FALSE);
BOOL WINAPI RegisterIHotKeyDsc(HWND hWnd , int id, CIHotkey& oHK, const char* szHotKeyDsc,BOOL bRebuildList=TRUE);
BOOL WINAPI UnregisterIHotKeyDsc(HWND hWnd, int id);
BOOL WINAPI WKIHOTKEYS_RegisterExternEvent(const char* szEventID, const char* szShortDescription, const char* szEventClassName);
BOOL WINAPI WKIHOTKEYS_UnRegisterExternEvent(const char* szEventID);
BOOL WINAPI WKIHOTKEYS_ExternEventStart(const char* szEventID);
BOOL WINAPI WKIHOTKEYS_ExternEventStop(const char* szEventID);
BOOL WINAPI WKIHOTKEYS_ResetKB();
CExternEvent* WINAPI WKIHOTKEYS_ExternGetEvent(int iIndex);
CExternEventClass* WINAPI WKIHOTKEYS_ExternGetClass(int iIndex);
CExternEvent* WINAPI WKIHOTKEYS_ExternGetEventByID(const char* szEventID);
CStringArray& WINAPI WKIHOTKEYS_GetExternEvents(int iLockStatus);
int WINAPI WKIHOTKEYS_RegisterOnAddRemoveEventCallback(FPONEVENTADD fpAdd,FPONEVENTREMOVE fpRemove);
int WINAPI WKIHOTKEYS_NotifyOnAddEvent(char const* szEventID);
int WINAPI WKIHOTKEYS_GetHotkeyDscByID(char const* szEventID,char* szOut,int iOut);
int WINAPI WKIHOTKEYS_ActHotkeyByID(char const* szEventID);
//Options
BOOL& WKIHOTKEYS_CheckForExternalDLL();
BOOL& WINAPI WKIHOTKEYS_DistingLRKeys();
HWND& WINAPI WKIHOTKEYS_EventsHandlerWindow();
HWND& WINAPI WKIHOTKEYS_EventsHandlerDuplicateWindow();
void GetBytesFromDword(char* szBytes, DWORD* dwDwords);
DWORD GetDwordFromBytes(const char* szBytes);
void MarkHKListAsChanged();
void MarkHKListAsNotChanged();
BOOL GetMarkHKListChangedStatus();
#endif // !defined(IPV6_HOTKEYS)
