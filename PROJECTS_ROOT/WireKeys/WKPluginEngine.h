// WKPluginEngine.h: interface for the CWKPluginEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WKPLUGINENGINE_H__5049937F_1955_446E_8509_EDDB6814461F__INCLUDED_)
#define AFX_WKPLUGINENGINE_H__5049937F_1955_446E_8509_EDDB6814461F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WKPlugin.h"
#include "..\SmartWires\SystemUtils\hotkeys.h"
#include "..\SmartWires\GUIClasses\DLG_Options.h"
class WKPluginFunctionDscEx: public WKPluginFunctionDsc
{
public:
	static DWORD dwCommandCodeCount;
	WKPluginFunctionDscEx()
	{
		hRealItemBmp=NULL;
		dwCommandCode=dwCommandCodeCount;
	};
	WKPluginFunctionDscEx& operator=(WKPluginFunctionDscEx& CopyObj)
	{
		hRealItemBmp=CopyObj.hRealItemBmp;
		dwCommandCode=CopyObj.dwCommandCode;
		oRealHotkey=CopyObj.oRealHotkey;
		oRealDefHotkey=CopyObj.oRealDefHotkey;
		memcpy((WKPluginFunctionDsc*)this,(WKPluginFunctionDsc*)&CopyObj,sizeof(WKPluginFunctionDsc));
		return *this;
	}
	HBITMAP	hRealItemBmp;
	DWORD dwCommandCode;
	CIHotkey oRealHotkey;
	CIHotkey oRealDefHotkey;
};

class CPluginOption
{
public:
	CPluginOption()
	{
		m_lValue=0;
		m_sValue="";
		m_pOption=0;
		m_pParent="";
		m_hThis=0;
	};
	~CPluginOption()
	{
		if(m_pOption){
			delete m_pOption;
		}
	}
	CString sName;
	// Внутреннее значение
	COption* m_pOption;// Пока это равно NULL считается что опуия не добавлена плагином -> ниоткуда не видна
	CString m_sValue;// Для списка - сам список
	long m_lValue;// Для строки - хинт, для горклавиши - номер функции
	// Представление в настройках
	CString m_sOptionTitle;
	DWORD m_dwOptionPosition;
	HIROW m_hThis;
	CString m_pParent;
};

class CSplashWindow;
class CPlugin: public WKOptionsCallbackInterface, public WKCallbackInterface
{
public:
	virtual CString _l(const char* szText,const char* szDef=NULL);
	virtual CPluginOption* GetOptionByName(const char* szName);
	CPlugin()
	{
		hInst=NULL;
		bIsActive=FALSE;
		fCallFunction=NULL;
		fActualParamsFunction=NULL;
		bOptionsWasChanged=0;
		fOptionsFunction=NULL;
		fOptionsManager=NULL;
		fInitFunction=NULL;
		fStopFunction=NULL;
		fPauseFunction=NULL;
		hRealPluginBmp=NULL;
		fTrayMenuFunction=NULL;
		iPluginIconNum=-1;
		bResident=FALSE;
		pOSD=NULL;
		lIndex=-1;
		dwVersionHi=0;
		dwVersionLo=0;
		bIsActiveOverrideForOption=-1;
		lTrayPopupItemID_Start=0;
		lTrayPopupItemID_End=0;
	};
	~CPlugin();
	long lIndex;
	long bIsActive;
	long bIsActiveOverrideForOption;
	long lTrayPopupItemID_Start;
	long lTrayPopupItemID_End;
	CString sFile;
	CString sTitle;
	CString sDescription;
	CString sAuthors;
	CString sFileName;
	DWORD dwVersionHi;
	DWORD dwVersionLo;
	BOOL bResident;
	int iPluginIconNum;
	HBITMAP hRealPluginBmp;
	HINSTANCE hInst;
	BOOL bOptionsWasChanged;
	FPLUGININITFUNCTION fInitFunction;
	FPLUGINCALLFUNCTION	fCallFunction;
	FPLUGINDPOPUPMENUFUNCTION fTrayMenuFunction;
	FPLUGINACTDSCFUNCTION fActualParamsFunction;
	FPLUGINOPTIONSFUNCTION fOptionsFunction;
	FPLUGINOPTMANSFUNCTION fOptionsManager;
	FPLUGINSTARTFUNCTION fStartFunction;
	FPLUGINSTOPFUNCTION fStopFunction;
	FPLUGINPAUSEFUNCTION fPauseFunction;
	FPLUGINDSWITCHFUNCTION fDSwitchFunction;
	CArray<WKPluginFunctionDscEx,WKPluginFunctionDscEx&> funcs;
	BOOL triggerTweak(DWORD dwOption);
	// Настройки
public:
	CCriticalSection csOptions;
	CCriticalSection csOTExecution;
	CArray<CPluginOption*,CPluginOption*> options;
	BOOL Lookup(const char* szName,CPluginOption*& pOption);
	BOOL AddBoolOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long bDefaultValue, DWORD dwOptionPosition);
	BOOL AddStringOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szDefaultValue, DWORD dwHint, DWORD dwOptionPosition);
	BOOL AddNumberOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long lDefaultValue, long lMinValue, long lMaxValue, DWORD dwOptionPosition);
	BOOL AddListOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szTabDelimitedList, long lDefaultPosition, DWORD dwOptionPosition);
	BOOL AddActionOption(const char* szOptionName, const char* szOptionTitle, long iPluginFunction, DWORD dwOptionPosition);
	BOOL AddBinOption(const char* szOptionName);
	long GetBoolOption(const char* szOptionName);
	size_t GetStringOption(const char* szOptionName, char* szOut, size_t dwOutLen);
	long GetNumberOption(const char* szOptionName);
	long GetListOption(const char* szOptionName);
	long GetBinOption(const char* szOptionName, LPVOID pData, DWORD dwDataSize);
	BOOL SetBoolOption(const char* szOptionName, long lNewValue);
	BOOL SetStringOption(const char* szOptionName, const char* szNewValue);
	BOOL SetNumberOption(const char* szOptionName, long lNewValue);
	BOOL SetListOption(const char* szOptionName, long lNewValue);
	BOOL SetBinOption(const char* szOptionName, LPVOID pData, DWORD dwDataSize);
	BOOL RemoveOption(const char* szOptionName);
	BOOL AddTip(const char* szOptionName, const char* szTip, DWORD dwOptionPosition);
	size_t TranslateText(const char* szToTranslate,char* szText, size_t dwTextSize);
public:
	// Операции для плагинов
	int GetWKState();
	size_t GetSelectedText(char* szText, size_t size);
	BOOL PutText(const char* szText);
	DWORD GetClipCount();
	size_t GetClipContent(DWORD dwClipNum, char* szText, size_t size);
	BOOL SetClipContent(DWORD dwClipNum, const char* szContent);
	BOOL ShowAlert(const char* szText, const char* szTitle);
	size_t GetPreferencesFolder(char* szText, size_t size);
	size_t GetTranslation(const char* szToTranslate,char* szText, size_t size);
	BOOL ProcessWindow(HWND hWnd, int iAction);
	BOOL ShowWKMenu(HWND hWnd);
	BOOL ShowAppInfo(HWND hWnd);
	BOOL GetWKVersion(char szVersion[32]);
	HWND GetWKMainWindow();
	DWORD GetWKActionHKCode(int iAction);
	BOOL GetWKActionHKDesc(int iAction, char szAsString[32]);
	BOOL CallWKAction(int iAction);
	BOOL ShowPopup(const char* szText, const char* szTitle, DWORD dwTimeOut);
	size_t GetWKPluginsDirectory(char* szPath, size_t dwPathSize);
	BOOL AddQrunFromHWND(HWND hwnd);
	BOOL PushMyOptions(DWORD dwParameter);
	size_t SubstPatterns(char* szValue, size_t dwValueSize);
	BOOL ShowOSD(const char* szText,DWORD dwTimeToShow);
	BOOL StartQuickRunByName(const char* szQRunTitle, const char* szAdditionalParameter);
	BOOL StartQuickRunByNameX(const char* szQRunTitle, const char* szAdditionalParameter, const char* szSelectionSubstitution, int iEntryNum);
    BOOL CallWKActionX(int iAction, const char* szParameter);
	BOOL AddQrunFromFile(const char* szQRunFile);
	BOOL RegisterEvent(char szEventID[5], char szShortDescription[64], char szEventClassName[64]);
	BOOL UnRegisterEvent(char szEventID[5]);
	BOOL NotifyEventStarted(char szEventID[5]);
	BOOL NotifyEventFinished(char szEventID[5]);
	BOOL RegisterOnAddRemoveEventCallback(FPONEVENTADD fpAdd,FPONEVENTREMOVE fpRemove);
	BOOL SubmitToSystemEventNotification(int iEventType, FPONSYSTEMEVENT fp);
	BOOL UnSubmitToSystemEventNotification(int iEventType, FPONSYSTEMEVENT fp);
	BOOL NotifyAboutSystemEvent(CWKSYSEvent* pEvent);
	BOOL WaitWhileAllKeysAreReleased();
	void ReturnKeyboardFocusToActiveApp(WKPluginFunctionStuff*);
	BOOL GetRegSetting(const char* szRegSettingsName, char* szOut, size_t dwOutSize);
	BOOL SetRegSetting(const char* szRegSettingsName, char* szOut);
	BOOL PlayWavSound(const char* szWav);
	BOOL getOption(DWORD dwOption);
	LPVOID getOptionEx(DWORD dwOption,DWORD dwParameter);
	LPVOID setOptionEx(DWORD dwOption,DWORD dwParameter);
	CSplashWindow* pOSD;
	void SetPluginDsc(CPluginOption* pPlug, const char* szOptionTitle, const char* szOptionDescription,DWORD dwOptionPosition);
	BOOL ChangeItemIcon(const char* szItemPath);
	BOOL ShowBaloon(const char* szText,const char* szTitle,DWORD dwTimeToShow);
    int AddBoolOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long bDefaultValue=TRUE, DWORD dwOptionPosition=0, const char* szParent=0);
    int AddStringOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szDefaultValue="", DWORD dwHint=0, DWORD dwOptionPosition=0, const char* szParent=0);
    int AddNumberOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long lDefaultValue=0, long lMinValue=-999999, long lMaxValue=999999, DWORD dwOptionPosition=0, const char* szParent=0);
    int AddListOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szTabDelimitedList="", long lDefaultPosition=0, DWORD dwOptionPosition=0, const char* szParent=0);
    int AddActionOptionEx(const char* szOptionName, const char* szOptionTitle, long iPluginFunction, DWORD dwOptionPosition=0, const char* szParent=0);
	int AddTipEx(const char* szOptionName, const char* szTip, DWORD dwOptionPosition, const char* szParent=0);
	int AddActionHotkey(const char* szOptionName, const char* szActionTitle, const char* szOptionDescription, DWORD dwActionNumber, DWORD dwOptionPosition, const char* szParent=0);
	int AddColorOption(const char* szOptionName, const char* szActionTitle, const char* szOptionDescription, DWORD dwDefaultColor, DWORD dwOptionPosition, const char* szParent=0);
	BOOL Sound(int iType);
	BOOL ShowPluginPrefernces();
	BOOL TestRunningApplicationsByMask(const char* szMask);
	BOOL PauseWKCompletely(BOOL bPause);
	BOOL ShowPreferencesTopic(const char* szTitle_EN);
	HINSTANCE GetPluginDllInstance();
	void SaveScreenshotTo(char* szPath, int iType);
	void getMD5(char* szOut, BYTE* pFrom, size_t size);
	BOOL ExecuteScript(const char* szText);
	size_t GetBaseTranslation(const char* szToTranslate,char* szText, size_t dwTextSize);
};

struct CIntData
{
	CIntData(long l,LPVOID p)
	{
		lType=l;
		pData=p;
	}
	long lType;
	LPVOID pData;
};

void LoadPlugins();
int CallPluginStartFunction(FPLUGINSTARTFUNCTION plugFunc);
int CallPluginStopFunction(FPLUGINSTOPFUNCTION plugFunc);
int CallPluginPauseFunction(CPlugin* oPlug, BOOL bPause);
int CallPluginTPFunction(CPlugin* oPlug,int iActionType, int& iItemId, HMENU hMenu);
HINSTANCE SafeLoadLibrary(const char* szFile);
BOOL LoadPluginRaw(CPlugin*& oPlug);
BOOL UnloadPluginRaw(CPlugin*& oPlug);
BOOL LoadPlugin(CPlugin*& oPlug, BOOL bForceLoad=FALSE);
BOOL UnloadPlugin(CPlugin* oPlug);
BOOL SavePluginOptions(CPlugin* oPlug);
int UnLoadPlugins(BOOL bAndDisableForFuture=FALSE);
BOOL isPluginLoaded(const char* szFileName, CPlugin** pOut=0, BOOL bResident=TRUE);
int PrepareDesktopSwitch(BOOL bPause);
typedef int (WINAPI *FPLUGINDSCFUNCTIONHINTS)(long iPluginFunction, long lHintCode, void*& pOut);
BOOL GlobalNotifyAboutSystemEvent(CWKSYSEvent* pEvent);
#endif // !defined(AFX_WKPLUGINENGINE_H__5049937F_1955_446E_8509_EDDB6814461F__INCLUDED_)
