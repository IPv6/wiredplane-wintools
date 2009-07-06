// SupportClasses.h: interface for the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUPPORTCLASSES_H__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_)
#define AFX_SUPPORTCLASSES_H__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Macros.h"
#include "Debug.h"
#include <atlbase.h>
#include <afxdlgs.h>
#include "DataXMLSaver.h"
#include <afxmt.h>
#include <math.h>

#ifdef __AFXCMN_H__
#include "../GUIClasses/IListCtrl.h"
// Фукнции, работающие с контролами...
int AddComboBoxExItem(CComboBoxEx* pCombo, int iCount, const char* szText, int iIcon);
#endif
// PathMatchSpec - проверка маски из АПИ
//
#define UNIQUE_TO_SYSTEM  0
#define UNIQUE_TO_DESKTOP 1
#define UNIQUE_TO_SESSION 2
#define UNIQUE_TO_TRUSTEE 3
#define UNIQUE_TO_COMPUTER 4
//
#define W95 "Windows 95" 
#define W98 "Windows 98" 
#define WME "Windows ME" 
#define WNT4 "Windows NT 4.0" 
#define W2K "Windows 2000" 
#define WXP "Windows XP" 
#define WNE "Windows .Net" 
#define HED " Home Edition" 
#define PED " Professional Edtion" 
#define DCS " DataCenter Server" 
#define ADS " Advanced Server" 
#define ENS " Enterprise Server" 
#define WES " Web Server" 
#define SER " Server"

#define BMP_SPECPREFIX	"?\?F0LLOW1NG*IS*PACK3D*DATA!"
#define MYRAND_MAX 0x7fff
class CSmartLock:public CSingleLock
{
	DWORD dwStartTime;
	DWORD dwMinWaitTime;
	BOOL bLocked;
	CSyncObject* m_pObject;
	HCURSOR hCur;
public:
	void SetMinWait(DWORD dwOff){dwMinWaitTime=dwOff;};

	BOOL Init(CSyncObject* pObject, BOOL bInitialLock=0, int iCursor=0)
	{
		dwMinWaitTime=0;
		dwStartTime=GetTickCount();
		hCur=NULL;
		m_pObject=NULL;
		if(iCursor>=0){
			if(iCursor){
				hCur=SetCursor(AfxGetApp()->LoadCursor(iCursor));
			}else{
				hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
			}
		}
		bLocked=bInitialLock;
		if(pObject){
			m_pObject=pObject;
			if(bInitialLock){
				bLocked=m_pObject->Lock();
			}
		}
		return TRUE;
	}

	CSmartLock::CSmartLock(CSyncObject& pObject, BOOL bInitialLock=0, int iCursor=0):CSingleLock(&pObject, bInitialLock)
	{
		Init(&pObject, bInitialLock, iCursor);
	}

	CSmartLock::CSmartLock(CSyncObject* pObject, BOOL bInitialLock=0, int iCursor=0):CSingleLock(pObject, bInitialLock)
	{
		Init(pObject, bInitialLock, iCursor);
	};

	CSmartLock::~CSmartLock()
	{
		if(dwMinWaitTime){
			DWORD dwStopTime=GetTickCount();
			long lW=dwStopTime-dwStartTime;
			if(lW>0 && lW<long(dwMinWaitTime)){
				Sleep(dwMinWaitTime-lW);
			}
		}
		if(hCur){
			SetCursor(hCur);
		}
		if(bLocked){
			m_pObject->Unlock();
		}
	}

};

void AddToDebugLog(const char* szText);
#ifdef _DEBUG
#define LOG(X)	AddToDebugLog(X)
#else
#define LOG(X)
#endif

typedef CArray<const char*,const char*> CStringSet;
class CIniFileParser
{
public:
	CMapStringToOb aFiles;
	CMapStringToString aFilesCont;
	CIniFileParser(){};
	~CIniFileParser();
	void ClearCashes();
	CStringArray* loadFileString(const char* szRawFileName);
	const char* getValueRandom(const char* szFile,const char* szKey, BOOL bStripKey=TRUE);
	BOOL getAllValues(const char* szRawFileName,const char* szKey, CStringSet& aStrings, BOOL bStripKey=TRUE);
	CString getByKey(const char* szFile,const char* szKey);
};

class CThreadKillerHolder
{
public:
	CThreadKillerHolder();
	~CThreadKillerHolder();
};

class CThreadKiller
{
	int iTCount;
	CArray<HANDLE,HANDLE> threads;
	CCriticalSection cs;
public:
	CThreadKiller();
	~CThreadKiller();
	void AddThis();
	void RemoveThis();
};

class CPairItem
{
public:
	CString sClipItemDsc;
	CString sItem;
	CString sApp;
	CPairItem(const char* szItem, const char* szDsc=NULL, const char* szApp=NULL)
	{
		sClipItemDsc=szDsc;
		sItem=szItem;
		sApp=szApp;
	};
	CPairItem()
	{
		sClipItemDsc="";
		sItem="";
		sApp="";
	}
	CPairItem& operator=(CPairItem& CopyObj)
	{
		sClipItemDsc=CopyObj.sClipItemDsc;
		sItem=CopyObj.sItem;
		sApp=CopyObj.sApp;
		return *this;
	}
};

class CSysCurBlinker
{
public:
	HCURSOR hBackup;
	HCURSOR hBackup2;
	DWORD m_dwMinTime;
	DWORD m_dwCurTime;
	CSysCurBlinker(UINT hCursorFromRes, DWORD dwMinTime);
	~CSysCurBlinker();
};

typedef struct tag_MENUBARINFO
{
    DWORD cbSize;
    RECT  rcBar;          // rect of bar, popup, item
    HMENU hMenu;          // real menu handle of bar, popup
    HWND  hwndMenu;       // hwnd of item submenu if one
    BOOL  fBarFocused:1;  // bar, popup has the focus
    BOOL  fFocused:1;     // item has the focus
} _MENUBARINFO;
typedef BOOL (WINAPI *_GetMenuBarInfo)(HWND hwnd,LONG idObject,LONG idItem,_MENUBARINFO* pmbi);

#define LANG_WRONGDLMS " \t\r\n\\.%:+-=*/!?,;()[]{}^`'"""
#define _L_NODEFAULT "???"
CString _getLngString(const char* szText,const char* szDef, const char* szDir, const char* szFile, int iLang=-1);
CString _l(const char* sText,const char* szDef=NULL);
CString GetLangName(int iLangNum);
BOOL IsHelpEnabled();
void ShowHelp(const char* szTopicName);
CString createExclusionName(LPCTSTR GUID, UINT kind = UNIQUE_TO_SYSTEM);

BOOL AppRunStateFile(const char* szProgName, BOOL bReadSaveDel, const char* szDesktop=NULL);

BOOL CheckProgrammRunState(const char* szProgName, UINT kind, bool bLeaveHandled, const char* szDesktop=NULL);
BOOL IsThisProgrammAlreadyRunning(UINT kind = UNIQUE_TO_TRUSTEE);
BOOL IsOtherProgrammAlreadyRunning(const char* szProgName, UINT kind = UNIQUE_TO_TRUSTEE);
UINT GetProgramWMMessage(const char* szProgName = NULL);
CWnd* GetAppWnd();
CString GetLastErrDsc();
CString GetCOMError(int iErr);
CIniFileParser& objFileParser();
long& GetApplicationLang();
const char* GetApplicationDir();
void SetAppDir(const char* sz);
const char* GetApplicationName();
void SetAppName(const char* sz);
CString GetAppName();
CString GetAppExe(BOOL bCheckSP=1);
//
int sgn(long l);
int rnd(int _min, int _max);
double rndDbl(double _min, double _max);
CString Format(long l);
CString Format(const char* szFormat,...);
bool PatternMatch(const char* s, const char* mask);
bool PatternMatchIC(const char* s, const char* mask);
bool PatternMatchList(const char* s, const char* mask, const char* szDelimiter=";");
long getCharCount(const char* szString, char ch);
int ScanAndReplace(CString& sBuffer, const char* szWrongChars, char szOkChar);
int ScanAndReplace(char* szBuffer, const char* szWrongChars, char szOkChar);
void CapitalizeAndShrink(CString& sStr,const char* szDelim);
CString TrimMessage(CString sMessage, int iSize=100, int bPrepareType=0);//0-ничего, 1-переводы строк, 2-обрезать середину
int Validate(int iVal, int iMin, int iMax, int iDef);
BOOL MakeSafeFileName(CString& sName, char sWithWhat='_');
BOOL MakeSafeForURL(CString& sText);
CString GetLargestLine(CString sText, int& iLines);
CSize GetLargestLineSize(CDC* pDC, CString sText, int& iLines);
BOOL CheckMyMaskSubstr(CString sTitle, CString sTitleMask, BOOL bDef=FALSE);
void SubstMyVariables(CString& sText,BOOL bJScriptSafe=FALSE);
void SubstEnvVariables(CString& sText);
CString ExtractInlineScript(CString sText);
BOOL ParseLineForCookies(CString& sText);
CString getCookie(const char* szKey);
CMapStringToString& getMCookies();
//
void ShutDownComputer(BOOL bRestart=FALSE, int iType=2);
BOOL StartupApplicationWithWindows (BOOL bStartup);
BOOL IsStartupWithWindows();
long CtrlWidth(CWnd* wnd, UINT CtrlID);
long CtrlHeight(CWnd* wnd, UINT CtrlID);
BOOL SetClipboardText(const WCHAR* szText, BOOL& bThroughGlobal, CWnd* pWndForOpen=0);
CString GetClipboardLinksAsText(BOOL bOpenCloseClip=1);
BOOL IsBMP(const WCHAR* wStr);
BOOL IsBMP(const char* szStr);
WCHAR* GetClipboardText(BOOL bAndClear=FALSE, BOOL bWithBitmaps=FALSE, BOOL bAndOpen=TRUE, DWORD* dwLen=0);// Вызывающая сторона должна удалить строку!
BOOL isScreenSaverActive();
BOOL isWindowStationLocked();
BOOL isScreenSaverActiveOrCLocked();
BOOL GetOSVersion(CString &WinVer, DWORD* dwMajor=NULL, long* bNT=NULL);
void AppDisplayProperties();
void AppAddRemPrograms();
void GetWindowTopParent(HWND& hWnd, BOOL bStopOnInvisible=1);
//
FILE* getFile(const char* szFName, const char* szOpenType="w", CString* sFileNameOut=NULL);
CString getFileFullName(const char* szFName, BOOL bDir=FALSE);
BOOL GetFolder(LPCTSTR szTitle,LPTSTR szPath,LPCTSTR szRoot=NULL,HWND hWndOwner=NULL);
BOOL GetFolderComputer(LPCTSTR szTitle, LPTSTR szPath, HWND hWndOwner=NULL);
CString GetUserFolder();
CString GetUserFolderRaw();
CString GetAppFolder();
//
typedef CArray<DWORD,DWORD> aNum2IDMap;
int FindIDNumInNum2IDMap(aNum2IDMap& aM2I, DWORD dwCode);
int AppendClonedMenu(CMenu* pMenu,HMENU hMenu,long lIDOffset=0, BOOL* bOwnerDraws=NULL, aNum2IDMap* aM2I=NULL);
int AddMenuSubmenu(CMenu* pMenuTo, CMenu* pMenuFrom, const char* szString, CBitmap* pBmp=NULL, DWORD dwID=0, int iIndexTo=-1);
void AddMenuStringEx(CDWordArray* aIDsLog, CMenu* pMenu, int iID, const char* szString, CBitmap* pBmp=NULL, BOOL bCheck=FALSE, CBitmap* pBmpC=NULL, BOOL bGray=FALSE, BOOL bDisabled=FALSE, BOOL bBreakMenu=FALSE);
void AddMenuString(CMenu* pMenu, int iID, const char* szString, CBitmap* pBmp=NULL, BOOL bCheck=FALSE, CBitmap* pBmpC=NULL, BOOL bGray=FALSE, BOOL bDisabled=FALSE, BOOL bBreakMenu=FALSE);
void AddMenuString(CMenu* pMenu, int iID, const char* szString, DWORD dwFlags, CBitmap* pBmp, CBitmap* pBmpC);
void SendSuggestion(const char* szProg, const char* szVer, const char* szSubj="Suggestion");
BOOL PrintWindow(CWnd* pWnd, CString sText, CString sTitle="");
CString GetSizeStrFrom(DWORD dwSize, DWORD dwFrom);
#define CUT_LF_AT_THE_END(x) if(x[strlen(x)-1]==10)x[strlen(x)-1]='\0'
BOOL GetSpecialFolderPath(char* szOutPath, int nFolder);
CRect GetDesktopRect();
CRect GetMainMonitorRECT();
void SetWindowSize(CWnd* pThis, CSize sx, UINT dwControlID=0);
void SetButtonSize(CWnd* pThis, UINT dwControlID, CSize sx=CSize(60,20));
void SetTaskbarButton(HWND hWnd);
void SetButtonWidth(CWnd* pThis, UINT dwControlID, long sx);
void SetButtonHeight(CWnd* pThis, UINT dwControlID, long sy);
void SetButtonHeight(CWnd* pThis, UINT dwControlID, UINT iFrom);
void SetButtonWidth(CWnd* pThis, UINT dwControlID, UINT iFrom);
CFont* CreateFontFromStr(CString sSchValue, CString* pName=NULL, DWORD* dwBgColor=NULL, DWORD* dwTxColor=NULL, DWORD* dwAlpha=NULL, DWORD* dwAutoTrans=NULL);
CString CreateStrFromFont(CFont* pFont, CString sName, DWORD dwBgColorOut, DWORD dwTxColorOut, DWORD dwAlphaOut, DWORD dwAutoDisp=0, BOOL bInCuttedFormat=FALSE);
CString GetFontTextDsc(CString sFont);
CString GetFontTextDsc(CFont* pFont);
BOOL CreateDirIfNotExist(const char* szFile);
CString GetDirectoryFromPath(const char* szFile);
CString GetFileFromPath(const char* szFile);
BOOL AdoptSizeToScreen(CSize& size);
long SimpleCalc(const char* szExpr);
CString trimTextForMenu(CString sText);
CString getBrowserExePath();
CString getEmailExePath();
CString getMyDocPath();
CString getMyPicPath();
CString getDesktopPath();
BOOL RunDefaultEmailClient();
DWORD WINAPI RunDefaultEmailClient_InThread(LPVOID pData);
CString MakeStandartLen(const char* szStr, int iLen, char c=' ');
void ConvertToArray(const char* szData, CStringArray& aStrings, char c='\n', char cComment=';');
void ConvertComboDataToArray(const char* szData, CStringArray& aStrings, char c='\t', char cComment=0);
int GetIndexInArray(const char* szItem, CStringArray* pArray,BOOL bInstring=0);
CString ConvertArrayToString(CStringArray& aStrings, char c='\t');
DWORD GetRGBFromHTMLString(CString sRGB);
DWORD hexCode(const char* szRawValue);
CString GetRandomString(int iLen);
CString GetExtensionAssociatedAction(const char* szExt="html", const char* szAction="open");
BOOL ParseForShellExecute(CString sFrom, CString& sCommand, CString& sParameter, BOOL bPartialFindIsOK=FALSE);
BOOL FindFullPathToExecutable(CString& sCommand);
BOOL FindFullPathToFile(CString& sCommand);
BOOL IsBatFile(const char* szCommand);
BOOL IsConsoleApplication(const char* szFile);
CString SetTextForUpdateTipText(CString sText, int iMaxLines=30, BOOL bTrimEnters=FALSE);
CString GetReadableStringFromMinutes(DWORD dwMinutes);
BOOL IsClsidInstalled(const char* szClsid);
BOOL IsProgidInstalled(const char* szClsid);
BOOL IsDefaultDesktop(CString* sDeskName=NULL);
void AsyncPlaySoundSys(const char* szSoundName);
CString GenerateNewId(const char* szPrefix="",const char* szPostfix="");
BOOL SetRegString(HKEY& hOpenedKey, const char* szKeyName, CString sValue);
CString GetRegString(HKEY& hOpenedKey, const char* szKeyName, DWORD dwMaxSize=MAX_PATH);
void AttachToWindowQueue(HWND hWin, BOOL bType);
BOOL BlockInputMy(BOOL bBlock);
void ShowContextMenu(HWND hWnd, LPCTSTR pszPath, int x, int y);
void ShowFileProps(const char* szFile);
CMapStringToString& GetIniFileCash();
CString _getIni(const char* szText,const char* szDef="");
DWORD GetFileChecksum(CString m_strFilePath);
void _clearIni();
BOOL isWin98();
BOOL isWinXP();
BOOL SaveFileFromHGlob(CString& sSavingPath,CString& szLastPathToSaveTo,HGLOBAL& hGlob,DWORD dwSize, BOOL bNeedDialog=TRUE, DWORD dwOffset=0, CWnd* pParent=NULL);
size_t mymemcpy(char* pTo, const char* pFrom, size_t howMuch);
HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc, LPCSTR lpszAppParams=NULL);
int GetWindowMenuHeight(CWnd* m_pMain);
CString GetRandomName();
int GetNearestStr(const char* szFrom, int iStartFrom, const char* szParts[], int* iIndex=0);
CString& AppCommandLine();
CString AddPathExtension(const char* szPath, const char* szExt);
BOOL GetCommandLineParameter(CString sName, CString& sValue, BOOL bAllowExpandFromFile=TRUE);
CString MakeRelativePath(const char* szFullPath,const char* szBasePath);
COleDateTime StringToTime(CString sTime);
int FindSubstrWithVolatilePP(CString& sCML,const char* sPML,const char* szPrefixes,const char* szPostFixes);
CString AppendPaths(CString sPath1, CString sPath2);
BOOL DeleteFileUsingRBin(const char* sPath);
BOOL IsFileInZip(const char* szFile);
BOOL IsFileURL(const char* szFile);
BOOL IsFileHTML(const char* szFile);
CMapStringToString& GetLangTermCash();
CString GetPathPart(const char* szFile, BOOL bD,BOOL bP,BOOL bN,BOOL bE);
BOOL& IsAppStopping();
DWORD& MaxBmpSize();
#include <math.h>
class CSinusoid
{
public:
	double m_dAmp;
	double p1;
	double o2;
	double o3;
	double p4;
	double o5;
	double o6;
	double p7;
	double o8;
	double o9;
	double o10;
	double o11;
	double p12;
	double o13;
	double o14;
	double o15;
	double o16;
	double p17;
	double o18;
	double o19;
	double o20;
	double o21;
	CSinusoid(double dAmp,double dAmpRnd=1)
	{
		m_dAmp=dAmp/2.0;
		p1=rndDbl(0,dAmpRnd);
		o2=rndDbl(-dAmpRnd,dAmpRnd);
		o3=rndDbl(-dAmpRnd,dAmpRnd);
		p4=rndDbl(0,dAmpRnd);
		o5=rndDbl(-dAmpRnd,dAmpRnd);
		o6=rndDbl(-dAmpRnd,dAmpRnd);
		p7=rndDbl(0,dAmpRnd);
		o8=rndDbl(-dAmpRnd,dAmpRnd);
		o9=rndDbl(-dAmpRnd,dAmpRnd);
		o10=rndDbl(-dAmpRnd,dAmpRnd);
		o11=rndDbl(-dAmpRnd,dAmpRnd);
		p12=rndDbl(0,dAmpRnd);
		o13=rndDbl(-dAmpRnd,dAmpRnd);
		o14=rndDbl(-dAmpRnd,dAmpRnd);
		o15=rndDbl(-dAmpRnd,dAmpRnd);
		o16=rndDbl(-dAmpRnd,dAmpRnd);
		p17=rndDbl(0,dAmpRnd);
		o18=rndDbl(-dAmpRnd,dAmpRnd);
		o19=rndDbl(-dAmpRnd,dAmpRnd);
		o20=rndDbl(-dAmpRnd,dAmpRnd);
		o21=rndDbl(-dAmpRnd,dAmpRnd);
	};
	double Get1D(double t)
	{
		double dUnNormed=p1*sin(o2*t+o3)
			+p4*cos(o5*t+o6)
			+p7*sin(o8*t+o9)*sin(o10*t+o11)
			+p12*cos(o13*t+o14)*cos(o15*t+o16)
			+p17*sin(o18*t+o19)*cos(o20*t+o21);
		return (dUnNormed/(p1+p4+p7+p12+p17))*m_dAmp+m_dAmp;
	}
};

class IUITranslation
{
public:
	virtual char* uit(const char* szUIText,const char* szUITextDefault)=0;
	virtual void freestr(char* sz)=0;
};


class CI18N
{
public:
	static IUITranslation*& GetTranslationIFace()
	{
		static IUITranslation* res=0;
		return res;
	}
	
	static CString _l(const char* szWhat)
	{
		if(!GetTranslationIFace()){
			return szWhat;
		}
		char* sz=GetTranslationIFace()->uit(szWhat,szWhat);
		CString sRes=sz;
		GetTranslationIFace()->freestr(sz);
		return sRes;
	}
};

class CUITranslationImpl:public IUITranslation
{
public:
	char* uit(const char* szUIText,const char* szUITextDefault);
	void freestr(char* sz);
	static CUITranslationImpl* getInstance();
};

class CCriticalSectionGuard
{
	CCriticalSection* m_lock;
public:
	CCriticalSectionGuard(CCriticalSection* lock)
	{
		m_lock=lock;
		if(m_lock){
			m_lock->Lock();
		}
	}
	~CCriticalSectionGuard()
	{
		if(m_lock){
			m_lock->Unlock();
		}
	}
};

CString GetNextPrefixedId(CString sKey,int iIndex=-1);

BOOL IsWindowsSystemWnd(HWND hWin);
BOOL isPressed(UINT);
CString UnifyString(CString sKey);
CString regGetBuyURL(CString publisher, CString appName, CString appVer );

class CFakeWindow
{
public:
	CWnd wnd;
	HWND hWin;
	HWND GetHwnd(){return hWin;};
	CWnd* GetWnd(){return &wnd;};
	CFakeWindow::CFakeWindow(const char* szTitle, DWORD dwStyles=WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT|WS_EX_APPWINDOW)
	{
		CRect rDesktopRECT(0,0,0,0);
		hWin = ::CreateWindowEx(dwStyles, "Static", szTitle?szTitle:"MENU_WND", WS_VISIBLE|WS_POPUP, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.right-rDesktopRECT.left, rDesktopRECT.bottom-rDesktopRECT.top, 0, 0, AfxGetApp()->m_hInstance, 0);//WS_DISABLED|WS_VISIBLE
		//SetTaskbarButton(hWin);
		wnd.Attach(hWin);
	}

	CFakeWindow::~CFakeWindow()
	{
		wnd.Detach();
		DestroyWindow(hWin);
	}
};

void MakeJScriptSafe(CString& sFile);
HWND PlayMusicX(CString szSoundPath,BOOL bLoopSound, CString sAlias);
BOOL RegisterExtension(CString sExt, CString sDescription, CString sExeParam);

class CDesktopDC
{
public:
	CDC* dc;
	CDesktopDC()
	{
		dc=new CDC();
		dc->CreateDC("DISPLAY", NULL, NULL, NULL);
	}
	CDC* operator&()
	{
		return dc;
	}
	CDC* operator->()
	{
		return dc;
	}
	operator HDC()
	{
		return dc->GetSafeHdc();
	}
	~CDesktopDC()
	{
		delete dc;
	}
};

CWnd* AfxWPGetMainWnd();
BOOL ParseForTag(CString sContent,CString sTag,CString sAttr,CString sMask,CStringArray& aRes, CString sSourceURL="");
CString MakeBaseURL(CString sSrc, CString sAdditional);
BOOL GetFileCreationDate(CString sStartDir, DWORD& dwDay,DWORD& dwMon,DWORD& dwYear);
CString DumpCAr(CStringArray& a,CStringArray* b=0);
CString DateFormat(COleDateTime tm,BOOL bShort=1);
CString TimeFormat(COleDateTime tm,BOOL bDef=1, BOOL bNoSecs=1);
BOOL IsInOneHier(HWND hNewWindow,HWND hWin);
int SelectFromMenu(CStringArray& aItems, HWND hParent);
void UTF8Encode2BytesUnicode(const WCHAR* input, CString& output);
BYTE GetFirstChar(CString s);
void AddSlashToPath(CString &sPath);
CString DelQuotesFromPath(const CString sPath, char symbol='\"');
void ConcateDir(CString& sDir,const char* szAdded="");
BOOL IsVista();
CString GetWindowClass(HWND h);
typedef BOOL (STDAPICALLTYPE * _PathFindOnPath)(LPSTR pszPath, LPCSTR * ppszOtherDirs);
typedef BOOL (STDAPICALLTYPE * _PathCompactPathEx)(LPSTR pszOut, LPCSTR pszSrc, UINT cchMax, DWORD dwFlags);
typedef BOOL (STDAPICALLTYPE * _PathRelativePathTo)(LPTSTR pszPath,LPCTSTR pszFrom,DWORD dwAttrFrom,LPCTSTR pszTo,DWORD dwAttrTo);
class CShlwapi
{
public:
	HINSTANCE hDll;
	_PathFindOnPath fpFind;
	_PathCompactPathEx fpCompact;
	_PathRelativePathTo fpMakeRelPath;
	CShlwapi()
	{
		fpFind=0;
		fpCompact=0;
		hDll=LoadLibrary("shlwapi.dll");
		fpFind=0;
		fpCompact=0;
		fpMakeRelPath=0;
		if(hDll){
			fpFind=(_PathFindOnPath)DBG_GETPROC(hDll,"PathFindOnPathA");
			fpCompact=(_PathCompactPathEx)DBG_GETPROC(hDll,"PathCompactPathExA");
			fpMakeRelPath=(_PathRelativePathTo)DBG_GETPROC(hDll,"PathRelativePathToA");
		}
	}
	~CShlwapi()
	{
		if(hDll){
			FreeLibrary(hDll);
		}
	}
};

CShlwapi& _Shlwapi();
WCHAR* MyA2W(const char* szText);
#ifndef DWORD_PTR
#define DWORD_PTR DWORD
#endif
class CWinMM
{
public:
	HMODULE hLib;
	CWinMM();
	~CWinMM();
	DWORD myMciSendCommand( IN UINT mciId, IN UINT uMsg, IN DWORD_PTR dwParam1, IN DWORD_PTR dwParam2);
	DWORD myMciSendString( IN LPCSTR lpstrCommand, OUT LPSTR lpstrReturnString, IN UINT uReturnLength, IN HWND hwndCallback);
	BOOL myPlaySound(IN LPCSTR pszSound, IN HMODULE hmod, IN DWORD fdwSound);
};

CWinMM& getWinmm();


#define __FLOWLOG		CLogFlow lf(__FILE__,__LINE__);
#define __FLOWLOG1(x)	CLogFlow lf(__FILE__,__LINE__,x);
#define __FLOWLOG2(x,y)	CLogFlow lf(__FILE__,__LINE__,x,y);
CString GetUserFolder();
BOOL isFileExist(const char* szFile);
BOOL SaveFile(const char* sStartDir, const char* sFileContent);
WINBASEAPI DWORD WINAPI GetCurrentThreadId();
#define WARNTHREAD_LIMIT	10000
class CLogFlow
{
public:
	#define	__FLOWLOG_BASEFOLDER	GetUserFolder()+"_DEBUG\\"
#ifdef _DEBUG
	#define	__FLOWLOG_BASEFOLDER_POST	""
#else
	#define	__FLOWLOG_BASEFOLDER_POST	"rls_"
#endif
	static CString GetAppRunID()
	{
		static CString sID;
		if(sID.GetLength()==0){
			SYSTEMTIME lSystemTime;
			memset(&lSystemTime,0,sizeof(lSystemTime));
			GetLocalTime(&lSystemTime);
			char szCurTime[MAX_PATH]={0};
			sprintf(szCurTime,__FLOWLOG_BASEFOLDER_POST"%04i%02i%02i#%02i%02i%02i_%i",lSystemTime.wYear,lSystemTime.wMonth,lSystemTime.wDay,lSystemTime.wHour,lSystemTime.wMinute,lSystemTime.wSecond,lSystemTime.wMilliseconds);
			sID=szCurTime;
		}
		return sID;
	}
	static DWORD GetRunID()
	{
		static DWORD iID=0;
		iID++;
		return iID;
	}
	static BOOL isEnabled()
	{
		static BOOL b=-1;
		if(b==-1){
			CString s=__FLOWLOG_BASEFOLDER;
			if(isFileExist(s)){
				b=1;
			}else{
				b=0;
			}
			SaveFile(GetUserFolder()+"dbg.file",s+(b?":enabled":":disabled"));
		}
		return b;
	}

	DWORD dwTime;
	CString file;
	CString fileContent;
	CLogFlow(const char* szFile, int l, const char* szParam="", const char* szParam2="")
	{
		if(!isEnabled()){
			return;
		}
		dwTime=GetTickCount();
		char sz[MAX_PATH*4]={0};
		sprintf(sz,"%s%s_%04i_t%i.log",__FLOWLOG_BASEFOLDER,GetAppRunID(),GetRunID(),GetCurrentThreadId());
		file=sz;

		SYSTEMTIME lSystemTime;
		memset(&lSystemTime,0,sizeof(lSystemTime));
		GetLocalTime(&lSystemTime);
		char szCurTime[MAX_PATH]={0};
		sprintf(szCurTime,"%02i.%02i.%04i %02i:%02i:%02i.%i",lSystemTime.wDay,lSystemTime.wMonth,lSystemTime.wYear,lSystemTime.wHour,lSystemTime.wMinute,lSystemTime.wSecond,lSystemTime.wMilliseconds);
		sprintf(sz,"[%s] file: %s; line=%i; thread: %i, param: %s %s",szCurTime,szFile,l,GetCurrentThreadId(),szParam,szParam2);
		fileContent=sz;
		SaveFile(file,fileContent);
	}
	~CLogFlow()
	{
		if(!isEnabled()){
			return;
		}
		DWORD dwt=GetTickCount()-dwTime;
		if(dwt>WARNTHREAD_LIMIT){
			char szWarning[2048]={0};
			sprintf(szWarning,"Warning, thread execution=%i sec!\n%s",dwt/1000,fileContent);
			AddLog(szWarning);
		}
		DeleteFile(file);
	}

	static void AddLogFile(const char* szfilepref,const char* logline)
	{
		SaveFile(__FLOWLOG_BASEFOLDER+Format("%s_%04i_%s.dbg",GetAppRunID(),GetRunID(),szfilepref),logline);
	}

	static void AddLog(const char* logline)
	{
		if(!isEnabled()){
			return;
		}
		static char sz[MAX_PATH*2]={0};
		if(sz[0]==0){
			sprintf(sz,"%s%s_messages.log",__FLOWLOG_BASEFOLDER,GetAppRunID());
		}
		FILE* f=fopen(sz,"a+");
		if(f){
			SYSTEMTIME lSystemTime;
			memset(&lSystemTime,0,sizeof(lSystemTime));
			GetLocalTime(&lSystemTime);
			char szCurTime[MAX_PATH]={0};
			sprintf(szCurTime,"%02i.%02i.%04i %02i:%02i:%02i.%i",lSystemTime.wDay,lSystemTime.wMonth,lSystemTime.wYear,lSystemTime.wHour,lSystemTime.wMinute,lSystemTime.wSecond,lSystemTime.wMilliseconds);
			fprintf(f,"[%s] ",szCurTime);
			fprintf(f,logline);
			fprintf(f,"\n");
			fclose(f);
		};
	}
};


// Макрос для логирования независимо от конфигурации
#define LOGERROR1(x,y)		if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2.Format(x,y);CLogFlow::AddLog(s+s2);};
#define LOGERROR2(x,y,z)	if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2.Format(x,y,z);CLogFlow::AddLog(s+s2);};
#define FLOG5(x,a,b,c,d,e)	if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2.Format(x,a,b,c,d,e);CLogFlow::AddLog(s+s2);};
#define FLOG4(x,a,b,c,d)	if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2.Format(x,a,b,c,d);CLogFlow::AddLog(s+s2);};
#define FLOG3(x,a,b,c)		if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2.Format(x,a,b,c);CLogFlow::AddLog(s+s2);};
#define FLOG2(x,a,b)		if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2.Format(x,a,b);CLogFlow::AddLog(s+s2);};
#define FLOG1(x,a)			if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2.Format(x,a);CLogFlow::AddLog(s+s2);};
#define FLOG(x)				if(CLogFlow::isEnabled()){CString s;s.Format("%s(%lu): ",__FILE__,__LINE__);CString s2;s2=x;CLogFlow::AddLog(s+s2);};
#define FLOGWND(SS,H)		if(CLogFlow::isEnabled()){char szT[256]={0};char szC[256]={0};::GetWindowText(H,szT,sizeof(szT));::GetClassName(H,szC,sizeof(szC));FLOG3(SS"%08x, '%s', '%s'",H,szT,szC);};
#define FLOGWND2(SS,H,X)	if(CLogFlow::isEnabled()){char szT[256]={0};char szC[256]={0};::GetWindowText(H,szT,sizeof(szT));::GetClassName(H,szC,sizeof(szC));FLOG4(SS"%08x, '%s', '%s'",X,H,szT,szC);};
#define	_XLOG_

#define DUMPFILE(x,y)		if(CLogFlow::isEnabled()){CLogFlow::AddLogFile((x),(y));};
#define LOGSTAMP			FLOG("timestamping");
#define LOGSTAMP1(x)		FLOG(x);


class CStringSubstr
{
public:
	
	static CString getTag(const char* szStart, const char* szEnd, const char* szString, int& iFrom, BOOL bRecursive)
	{
		CString sRes="";
		const char* szSearchFrom=szString+iFrom;
		char* szFrom=strstr(szSearchFrom,szStart);
		if(szFrom){
			sRes=(const char*)(szFrom+strlen(szStart));
			iFrom=(szFrom-szString)+strlen(szStart);
			const char* szSearchFromAfterStart=szString+iFrom;
		}else{
			iFrom=-1;
			return "";
		}
		if(szEnd!=NULL){
			int iEnd=sRes.Find(szEnd);
			if(iEnd!=-1){
				// Смотрим вложенности
				int iRecuFind=sRes.Find(szStart);
				if(bRecursive && iRecuFind!=-1 && iRecuFind<iEnd){
					//int iStCount=sRes.Replace(szStart,szStart);
					int iStCount=0,iStOffset=0;
					while((iStOffset=sRes.Find(szStart,iStOffset+strlen(szStart)))!=-1){
						iStCount++;
					}
					while(iStCount>0){
						iEnd=sRes.Find(szEnd,iEnd+strlen(szEnd));
						iStCount--;
					}
				}
				sRes=sRes.Left(iEnd);
				iFrom+=strlen(sRes)+strlen(szEnd);
				const char* szSearchFromEnd=szString+iFrom;
			}else{
				sRes="";
			}
		}else{
			sRes.TrimRight();
			iFrom+=strlen(sRes);
		}
		return sRes;
	}
	
	
	static CString getTag(const char* szStart, const char* szString)
	{
		int i=0;
		return getTag(szStart, NULL, szString,i, FALSE);
	}
	
	static CString getTag(const char* szStart, const char* szEnd, const char* szString)
	{
		int i=0;
		return getTag(szStart, szEnd, szString,i, FALSE);
	}
};

#endif // !defined(AFX_SUPPORTCLASSES_H__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_)
