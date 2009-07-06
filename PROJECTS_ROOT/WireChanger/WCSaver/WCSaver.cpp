// Images Screen Saver
//
// This saver shows a sprite (.BMP) bouncing over a background (.JPG)
// The two images have been compressed in a ZIP file, and this zip
// is stored as an RT_RCDATA resource in the .scr file. At runtime,
// the zip/bmp/jpg are all extracted directly from resource into
// memory: no temporary files are used.
//
// It's important to keep file-size down. That's why we use zips.
// (actually, only about 10k was saved by compressing the JPG into
// the zip... but it's the thought that counts!)
//
// As for the sprites, this saver demonstrates several techniques:
// (1) How to load JPEGs from memory. If you want to add this to your own code,
// you must #include <ole2.h> and <olectl.h> and copy the LoadJpeg() function.
// (2) How to load BMPs from memory. The code is in EnsureBitmaps()
// (3) How to make transparent sprites. The sprite is actually stored as two
// bitmaps, hbmClip and hbmSprite. The code for loading/generating the two
// is in EnsureBitmaps(). The code for drawing them is in OnPaint()
// (4) How to use a double-buffering to avoid flicker. The bitmap hbmBuffer
// stores our back-buffer. It's created in TSaverWindow() and used in OnPaint().
// (5) How to read zip files. The code for this is in EnsureBitmaps. Also,
// it's in the separate module UNZIP.CPP/UNZIP.H, which consists largely
// of code from www.info-zip.org. Thanks!
//

#pragma warning( disable: 4127 4800 4702 )
#define _WIN32_WINNT 0x0500
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <shlobj.h>
//#include <afxmt.h>
#include <commctrl.h>
#include <regstr.h>
#include <ole2.h>
#include <olectl.h>
#include <stdlib.h>
#include <tchar.h>
typedef std::basic_string<TCHAR> tstring;
using namespace std;
#include "unzip.h"


#ifdef _DEBUG
bool fakemulti=false;
#else
bool fakemulti=false;
#endif

int GetNumberOfMons()
{
	if(fakemulti){
		return 2;
	}
	return GetSystemMetrics(80);
}

DWORD GetFileSize(const char* szFileName)
{
	DWORD dwFileSize=0;
	FILE* f=fopen(szFileName,"rb");
	if (f==NULL){
		return 0;
	}
	fseek(f,0,SEEK_END);
	dwFileSize = ftell(f);
	fclose(f);
	return dwFileSize;
}

BOOL SaveFile(const char* sStartDir, const char* sFileContent)
{
	FILE* m_pFile=fopen(sStartDir,"w+b");
	if(!m_pFile){
		return FALSE;
	}
	DWORD nRead=fwrite(sFileContent,sizeof(char),strlen(sFileContent),m_pFile);
	fclose(m_pFile);
	m_pFile=NULL;
	return (nRead==strlen(sFileContent));
}

BOOL ReadFile(const char* szFileName, std::string& sFileContent)
{
	if(szFileName==NULL || strlen(szFileName)==0){
		return FALSE;
	}
	DWORD dwSize=GetFileSize(szFileName);
	if(dwSize==0){
		return FALSE;
	}
	FILE* f=fopen(szFileName,"rb");
	if(f==NULL){
		return FALSE;
	}
	char* szData = new char[dwSize+1];
	memset(szData,0,dwSize+1);
	DWORD dwReadedBytes=fread(szData,1,dwSize,f);
	sFileContent=szData;
	delete[] szData;
	fclose(f);
	// Проверяем и выходим
	if(dwReadedBytes!=dwSize){
		return FALSE;
	}
	return TRUE;
}

//#define SHOW_DEBUG
const bool SCRDEBUG = false;//(DebugFile!=_T(""));
// If you want debugging output, set this to "OutputDebugString" or a filename. The second line controls the saver's "friendliness"
const tstring DebugFile="";//_T("OutputDebugString");
// These global variables are loaded at the start of WinMain
BOOL  MuteSound=0;
DWORD MouseThreshold=10;  // In pixels
DWORD PasswordDelay=10;   // In seconds. Doesn't apply to NT/XP/Win2k.

DWORD g_dwTimeOut=10;
DWORD g_dwEffects=0;

// also these, which are used by the General properties dialog
DWORD PasswordDelayIndex=0;  // 0=seconds, 1=minutes. Purely a visual thing.
DWORD MouseThresholdIndex=1; // 0=high, 1=normal, 2=low, 3=ignore. Purely visual
TCHAR Corners[5]="NNNN";          // "-YN-" or something similar
BOOL  HotServices=0;         // whether they're present or not
// and these are created when the dialog/saver starts
POINT InitCursorPos;
DWORD InitTime=0;        // in ms
bool  IsDialogActive=0;
bool  ReallyClose=0;     // for NT, so we know if a WM_CLOSE came from us or it.
// Some other minor global variables and prototypes
HINSTANCE hInstance=0;
HICON hiconsm=0,hiconbg=0;
HBITMAP hbmmonitor=0;  // bitmap for the monitor class
tstring SaverName;     // this is retrieved at the start of WinMain from String Resource 1
vector<RECT> monitors; // the rectangles of each monitor (smSaver) or of the preview window (smPreview)
vector<SIZE> monitorSizes;
class TSaverWindow;
const UINT SCRM_GETMONITORAREA=WM_APP; // gets the client rectangle. 0=all, 1=topleft, 2=topright, 3=botright, 4=botleft corner
inline void Debug(const tstring s);
tstring GetLastErrorString();
void SetDlgItemUrl(HWND hdlg,int id,const TCHAR *url);
void   RegSave(const tstring name,int val);
void   RegSave(const tstring name,bool val);
void   RegSave(const tstring name,tstring val);
int    RegLoad(const tstring name,int def);
bool   RegLoad(const tstring name,bool def);
tstring RegLoad(const tstring name,tstring def);

//
// IMPORTANT GLOBAL VARIABLES:
enum TScrMode {smNone,smConfig,smPassword,smPreview,smSaver,smInstall,smUninstall} ScrMode=smNone;
vector<TSaverWindow*> SaverWindow;   // the saver windows, one per monitor. In preview mode there's just one.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------




// LoadJpeg: given an HGLOBAL containing jpeg data, we load it.
HBITMAP LoadJpeg(HGLOBAL hglob)
{ 
	
	IStream *stream=0; HRESULT hr=CreateStreamOnHGlobal(hglob,FALSE,&stream);
	if (!SUCCEEDED(hr) || stream==0) return 0;
	IPicture *pic;  hr=OleLoadPicture(stream, 0, FALSE, IID_IPicture, (LPVOID*)&pic);
	stream->Release();
	if (!SUCCEEDED(hr) || pic==0) return 0;
	HBITMAP hbm0=0; hr=pic->get_Handle((OLE_HANDLE*)&hbm0);
	if (!SUCCEEDED(hr) || hbm0==0) {pic->Release(); return 0;}
	//
	// Now we make a copy of it into our own hbm
	DIBSECTION dibs; GetObject(hbm0,sizeof(dibs),&dibs);
	if (dibs.dsBm.bmBitsPixel!=24) {pic->Release(); return 0;}
	int w=dibs.dsBm.bmWidth, h=dibs.dsBm.bmHeight;
	dibs.dsBmih.biClrUsed=0; dibs.dsBmih.biClrImportant=0; void *bits;
	HDC sdc=GetDC(0);
	HBITMAP hbm1=CreateDIBSection(sdc,(BITMAPINFO*)&dibs.dsBmih,DIB_RGB_COLORS,&bits,0,0);
	//
	HDC hdc0=CreateCompatibleDC(sdc), hdc1=CreateCompatibleDC(sdc);
	HGDIOBJ hold0=SelectObject(hdc0,hbm0), hold1=SelectObject(hdc1,hbm1);
	BitBlt(hdc1,0,0,w,h,hdc0,0,0,SRCCOPY);
	SelectObject(hdc0,hold0); SelectObject(hdc1,hold1);
	DeleteDC(hdc0); DeleteDC(hdc1);
	ReleaseDC(0,sdc);
	pic->Release();
	return hbm1;
}

//SHGetSpecialFolderPathA
typedef BOOL (WINAPI *_SHGetSpecialFolderLocation)(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl);
BOOL MyGetSpecialFLoc(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl)
{
	BOOL bRes=FALSE;
	HINSTANCE hShell32=LoadLibrary("Shell32.dll");
	if(hShell32){
		_SHGetSpecialFolderLocation fp=(_SHGetSpecialFolderLocation)GetProcAddress(hShell32,"SHGetSpecialFolderLocationA");
		if(!fp){
			fp=(_SHGetSpecialFolderLocation)GetProcAddress(hShell32,"SHGetSpecialFolderLocation");
		}
		LPITEMIDLIST pIdl = NULL;
		bRes=(*fp)(hwndOwner,nFolder,ppidl);
		FreeLibrary(hShell32);
	}
	return bRes;
}

BOOL GetSpecialFolderPath(char* szOutPath, int nFolder)
{
	strcpy(szOutPath,"c:\\temp\\");
	TCHAR szPathName[MAX_PATH]="";
	LPITEMIDLIST pidl;
	HRESULT hRes=S_FALSE;
	IMalloc* pMalloc=NULL;
	hRes = SHGetMalloc(&pMalloc);
	if (SUCCEEDED(hRes)){
		hRes = MyGetSpecialFLoc(NULL, nFolder, &pidl);
		if (SUCCEEDED(hRes))
		{
			hRes = SHGetPathFromIDList(pidl, szPathName);
			pMalloc->Free(pidl);
		}
		pMalloc->Release();
	}
	// если SUCCEEDED(hRes), то szPathName содержит путь к папке 
	if(SUCCEEDED(hRes)){
		strcpy(szOutPath,szPathName);
		return TRUE;
	}else{
		return FALSE;
	}
}

const char* GetUserFolder()
{
	static char szUserFolder[MAX_PATH]="";
	if(szUserFolder[0]!=0){
		return szUserFolder;
	}
	szUserFolder[0]='!';
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_APPDATA);
	strcat(szFolderBuffer,"\\WireChanger\\");
	if(int(GetFileAttributes(szFolderBuffer))==-1){
		CreateDirectory(szFolderBuffer,NULL);
	}
	strcpy(szUserFolder,szFolderBuffer);
	return szUserFolder;
}

BOOL isFileExist(const char* szFile)
{
	if(szFile==NULL || szFile[0]==0){
		return FALSE;
	}
	char szTmp[MAX_PATH]={0};
	strcpy(szTmp,szFile);
	strlwr(szTmp);
	if(int(GetFileAttributes(szFile))==-1){
		return FALSE;
	}
	return TRUE;
}
#include "..\..\SmartWires\SystemUtils\macros.h"
class CMsImg
{
public:
	BOOL bInit;
	HINSTANCE h;
	CMsImg()
	{
		bInit=0;
	};
	~CMsImg()
	{
		if(h){
			FreeLibrary(h);
		}
	};
	void Init()
	{
		bInit=1;
		h=LoadLibrary("Msimg32.dll");
	}
};
CMsImg msWrapper;
typedef BOOL (WINAPI *_AlphaBlend)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
BOOL WINAPI AlphaBlendMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,int cx2,int cy2,BLENDFUNCTION bl)
{
	BOOL bRes=FALSE;
	if(bl.SourceConstantAlpha<255){
		if(!msWrapper.bInit){
			msWrapper.Init();
		}
		BOOL bOk=0;
		if(msWrapper.h){
			_AlphaBlend fp=(_AlphaBlend)GetProcAddress(msWrapper.h,"AlphaBlend");
			if(fp){
				bOk=1;
				bRes=(*fp)(hTo,x,y,cx,cy,hFrom,x2,y2,cx2,cy2,bl);
			}
		}
		/*
		if(!bOk){
			for(int i=0;i<cx;i++){
				for(int j=0;j<cy;j++){
					DWORD dwC1=GetPixel(hFrom,x2+i,y2+j);
					DWORD dwC2=GetPixel(hTo,x+i,y+j);
					BYTE bR=(GetRValue(dwC1)*bl.SourceConstantAlpha+GetRValue(dwC2)*(255-bl.SourceConstantAlpha))>>8;
					BYTE bG=(GetGValue(dwC1)*bl.SourceConstantAlpha+GetGValue(dwC2)*(255-bl.SourceConstantAlpha))>>8;
					BYTE bB=(GetBValue(dwC1)*bl.SourceConstantAlpha+GetBValue(dwC2)*(255-bl.SourceConstantAlpha))>>8;
					::SetPixel(hTo,x+i,y+j,RGB(bR,bG,bB));
				}
			}
			return TRUE;
		}
		*/
	}
	if(!bRes){
		// Без прозрачности
		if(cx==cx2 && cy==cy2){
			::BitBlt(hTo,x, y, cx, cy, hFrom, x2, y2, SRCCOPY);
		}else{
			::StretchBlt(hTo,x, y, cx, cy, hFrom, x2, y2, cx2, cy2, SRCCOPY);
		}
		return TRUE;
	}
	return bRes;
}

BOOL WINAPI AlphaBlendMy(HDC hTo,int x,int y,int cx,int cy,HDC hFrom,int x2,int y2,BLENDFUNCTION bl)
{
	return AlphaBlendMy(hTo,x,y,cx,cy,hFrom,x2,y2,cx,cy,bl);
}

#define UNIQUE_TO_SYSTEM  0
#define UNIQUE_TO_DESKTOP 1
#define UNIQUE_TO_SESSION 2
#define UNIQUE_TO_TRUSTEE 3
#define UNIQUE_TO_COMPUTER 4

std::string createExclusionName(LPCTSTR GUID, UINT kind)
{
    switch(kind)
    { // тип  
		
	case UNIQUE_TO_SYSTEM:
		return std::string(GUID);
		
	case UNIQUE_TO_DESKTOP:
        { // рабочий стол  
            std::string s = GUID;
            DWORD len;
            HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
            BOOL result = GetUserObjectInformation(desktop, UOI_NAME, NULL, 0, &len);
            DWORD err = ::GetLastError();
            if(!result && err == ERROR_INSUFFICIENT_BUFFER)
            { // NT/2000  
                LPBYTE data = new BYTE[len];
                result = GetUserObjectInformation(desktop, UOI_NAME, data, len, &len);
                s += _T("-");
                s += (LPCTSTR)data;
                delete [ ] data;
            } // NT/2000  
            else
            { // Win9x  
                s += _T("_Win9x");
            } // Win9x   
			
            return s;
        } // рабочий стол  
		
	case UNIQUE_TO_SESSION:
        { // сессия  
            std::string s = GUID;
            HANDLE token;
            DWORD len;
            BOOL result = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);
            if(result)
            { // NT  
                GetTokenInformation(token, TokenStatistics, NULL, 0, &len);
                LPBYTE data = new BYTE[len];
                GetTokenInformation(token, TokenStatistics, data, len, &len);
                LUID uid = ((PTOKEN_STATISTICS)data)->AuthenticationId;
                delete [ ] data;
                char t[128]={0};
                sprintf(t,_T("_%08x%08x"), uid.HighPart, uid.LowPart);
				s+=t;
                return s;
            } // NT  
			
            else
            { // 16-битная OS  
				
                return s;
            } // 16-битная OS 
        } // сессия 
		
	case UNIQUE_TO_TRUSTEE:
        { // пользователь 
            std::string s = GUID;
			#define NAMELENGTH 64
            TCHAR userName[NAMELENGTH];
            DWORD userNameLength = NAMELENGTH;
            TCHAR domainName[NAMELENGTH];
            DWORD domainNameLength = NAMELENGTH;
			
            if(GetUserName(userName, &userNameLength))
            { // получаем сетевое имя 
				
                // Вызовы NetApi очень прожорливы по времени
                // Этот метод получает доменное имя из
                // переменных окружения
                domainNameLength = ExpandEnvironmentStrings(_T("%USERDOMAIN%"),
					domainName,
					NAMELENGTH);
                char t[128]={0};
                sprintf(t,_T("_%s_%s"), domainName, userName);
                s += t;
            } // получаем сетевое имя 
            return s;
        } // пользователь 
	case UNIQUE_TO_COMPUTER:
		{ // пользователь 
            std::string s = GUID;
#define NAMELENGTH 64
            TCHAR domainName[NAMELENGTH];
            DWORD domainNameLength = NAMELENGTH;
			TCHAR szCompName[MAX_COMPUTERNAME_LENGTH + 1]="";
			DWORD szCompNameLength=sizeof(szCompName);
            if(GetComputerName(szCompName,&szCompNameLength))
            { // получаем имя компа
                // Вызовы NetApi очень прожорливы по времени
                // Этот метод получает доменное имя из
                // переменных окружения
                domainName[0] = 0;//ExpandEnvironmentStrings(_T("%USERDOMAIN%"),domainName,NAMELENGTH);
                char t[128]={0};
                sprintf(t,_T("_%s_%s"), domainName, szCompName);
                s += t;
            } // получаем имя компа
            return s;
        } // пользователь 
	default:
		break;
    } // тип 
	
    return std::string(GUID);
}// createExclusionName
/*
void AddToDebugLog(const char* szText)
{
	FILE* fLog=fopen("c:\\WCSaver_debug.log","a+");
	if(fLog){
		fprintf(fLog,"%s",szText);
		fclose(fLog);
	}
}
*/
BOOL CheckProgrammRunState(const char* szProgName, UINT kind, bool bLeaveHandled)
{
    bool AlreadyRunning=false;
    SetLastError(0);

	//{"WIREDPLANE_WireChangerDefault_WIREDPLANE_IPv6"}
	std::string sMutexName=createExclusionName((std::string("WIREDPLANE_")+szProgName+"Default").c_str(), kind);
    HANDLE hMutexOneInstance = ::CreateMutex( NULL, TRUE, sMutexName.c_str());
    AlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);
    if (hMutexOneInstance != NULL){
        ::ReleaseMutex(hMutexOneInstance);
    }
    if(!bLeaveHandled){
		CloseHandle(hMutexOneInstance);
    }
	//AddToDebugLog(sMutexName.c_str());
	return AlreadyRunning;
}

BOOL IsOtherProgrammAlreadyRunning()
{
	return CheckProgrammRunState("WC3",UNIQUE_TO_COMPUTER,1);
}


// TSaverWindow: one is created for each saver window (be it preview, or the
// preview in the config dialog, or one for each monitor when running full-screen)
#include <sys/types.h>
#include <sys/stat.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
//#include "..\..\SmartWires\BitmapUtils\Dib.h"
long lDrawInProgress=0;
DWORD WINAPI PaintFast(LPVOID pIn);
#define PROGNAME	"WireChanger"
#define PROG_REGKEY		"SOFTWARE\\WiredPlane\\"PROGNAME
#define XPIE	20
#define YPIE	15
class TSaverWindow
{ 
public:
	int imageSizeX;
	int imageSizeY;
	int iMMCounter;
	std::string sWarnings;
	TSaverWindow** pBack;
	HWND hwnd; int id;          // id=-1 for a preview, or 0..n for full-screen on the specified monitor
	int bw, bh, cw,ch;    // dimensions of the background and sprite and client-area
	int x,y, dirx,diry;         // location and direction of the sprite
	unsigned int time;          // last time when we drew anything
	HBITMAP hbmBackground;      // the background
	HBITMAP hbmPrev;
	HANDLE hControl;
	BOOL bFlick;
	HWND hTarget;
	void OtherWndProc(UINT,WPARAM,LPARAM) {};
	HANDLE hGlo;
	CRITICAL_SECTION lc;
	int iThisMonitor;
	std::string lastFStat;
	long lNoReccur;
	bool bWarn;
	int iEffect;
	TSaverWindow(HWND _hwnd,int _id) : hwnd(_hwnd),id(_id), hbmBackground(0)
	{ 
		imageSizeX=monitorSizes[id+1].cx;
		imageSizeY=monitorSizes[id+1].cy;
		iMMCounter=0;
		lNoReccur=0;
		bWarn=1;
		iEffect=0;
		::InitializeCriticalSection(&lc);
		hGlo=CreateEvent(0,0,0,"WC_GENERATE_FILE");
		lastFStat="";
		bFlick=0;
		pBack=0;
		hbmPrev=0;
		hbmBackground=0;
		GetUserFolder();
		sWarnings="Please wait...";
		if(fakemulti){
			char szDebug[64]={0};
			sprintf(szDebug," id=%i",id);
			sWarnings+=szDebug;
		}
		LONG res; HKEY skey;
		// Такой изврат так как мы в другом десктопе
		res=RegOpenKeyEx(HKEY_CURRENT_USER,PROG_REGKEY,0,KEY_READ,&skey);
		if (res==ERROR_SUCCESS){
			DWORD valtype, valsize, val;
			valsize=sizeof(val); 
			res=RegQueryValueEx(skey,_T("MainWindow"),0,&valtype,(LPBYTE)&val,&valsize);
			hTarget=(HWND)val;
		}
		/*
		if(hTarget==0){
			hTarget=(HWND)::GetProp(::FindWindow("Progman",NULL),"WC_MAIN");
		}
		*/
		if(hTarget==0){
			sWarnings="Error: WC not found!";
		}
		RECT rc; GetClientRect(hwnd,&rc); cw=rc.right; ch=rc.bottom;
		time=GetTickCount();
		//SetTimer(hwnd,1,g_dwTimeOut*1000,NULL);
		DWORD dwID=0;
		hControl=CreateThread(0,0,PaintFast,(LPVOID)this,0,&dwID);
		//::PlaySound("SystemQuestion", NULL, SND_ALIAS | SND_NODEFAULT | SND_ASYNC | SND_NOWAIT);
	}
	
	~TSaverWindow()
	{ 
		CloseHandle(hGlo);
		pBack=0;
		TerminateThread(hControl,0);
		CloseHandle(hControl);
		//KillTimer(hwnd,1);
		if (hbmBackground!=0) DeleteObject(hbmBackground); hbmBackground=0;
		if (hbmPrev!=0) DeleteObject(hbmPrev); hbmPrev=0;
		::DeleteCriticalSection(&lc);
	}

	void OnTimer(BOOL bInit)
	{
		if(lNoReccur>0){
			return;
		}
		SimpleTracker lc(lNoReccur);
		char szF[MAX_PATH]="";
		strcpy(szF,GetUserFolder());
		if(szF[0]=='!'){
			return;
		}
		strcat(szF,"Screensaver//");
		::CreateDirectory(szF,NULL);
		strcat(szF,"Wired_Screensaver.bmp");
		char szFC[MAX_PATH]="";
		strcpy(szFC,szF);
		strcat(szFC,".cnt");
		if(!IsOtherProgrammAlreadyRunning()){
			if(bWarn){
				bWarn=0;
				MessageBox(NULL,_T("Slideshow screensaver: WireChanger is not running. You should start WireChanger first"),_T("WCSaver Install"),MB_ICONINFORMATION|MB_OK|MB_TOPMOST); 
			}
			return;
		}
		std::string curFStat;
		BOOL bNeddRelocate=bInit;
		{// Запрашиваем файлик
			// Вызваем WC
			sWarnings="Generating...";
			ReadFile(szFC,curFStat);
			if(bInit!=1 || (bInit==1 && id<=0)){
				// Тут все честно
				// RECT rtWnd;
				lastFStat=curFStat;
				char szTmp[54]={0};
				char szFX[MAX_PATH]="";
				strcpy(szFX,szF);
				strcat(szFX,".cx");
				sprintf(szTmp,"%i",imageSizeX);
				SaveFile(szFX,szTmp);
				char szFY[MAX_PATH]="";
				strcpy(szFY,szF);
				strcat(szFY,".cy");
				sprintf(szTmp,"%i",imageSizeY);
				SaveFile(szFY,szTmp);
				SetEvent(hGlo);
			}
			// Ждем!!!
			int iCount=15;
			while((!isFileExist(szF) || curFStat==lastFStat) && iCount>0){
				iCount--;
				Sleep(1000);
				ReadFile(szFC,curFStat);
			}
			lastFStat=curFStat;
			sWarnings="Prerendering...";
			bNeddRelocate=1;
		}
		// Второй раз..
		if(bNeddRelocate){
			HBITMAP hbmBackground2=0;
			hbmBackground2=(HBITMAP)LoadImage(NULL,szF,IMAGE_BITMAP,cw,ch,LR_LOADFROMFILE);
			if(hbmBackground2!=0){
				bw=cw;
				bh=ch;
				if(hbmBackground){
					DeleteObject(hbmBackground);
				}
				hbmBackground=hbmBackground2;
				bFlick=1;
			}else{
				sWarnings="Error loading image!";
			}
		}
	}

	void OnPaint(HDC hdc,RECT &rt)
	{
		EnterCriticalSection(&lc);
		HBITMAP hForTick=hbmBackground;
		if(hForTick==0){
			hForTick=hbmPrev;
		}else{
			if (hbmPrev!=0){
				DeleteObject(hbmPrev);
			}
			hbmPrev=hbmBackground;
			hbmBackground=0;
		}
		//::PlaySound("SystemQuestion", NULL, SND_ALIAS | SND_NODEFAULT | SND_ASYNC | SND_NOWAIT);
		if(hForTick==0){
			//::FillRect(hdc,&rt,GetSysColorBrush(COLOR_DESKTOP));
			rt.top+=12;
			rt.left+=12;
			::SetBkMode(hdc,TRANSPARENT);
			SetTextColor(hdc,RGB(255,255,255));
			::DrawText(hdc,sWarnings.c_str(),-1,&rt,DT_LEFT);
			rt.top-=1;
			rt.left-=1;
			SetTextColor(hdc,RGB(0,0,0));
			::DrawText(hdc,sWarnings.c_str(),-1,&rt,DT_LEFT);
		}else{// Рисуем...
			HDC bufdc=CreateCompatibleDC(hdc);
			HGDIOBJ hTmp=SelectObject(bufdc,hForTick);
			SetStretchBltMode(hdc,COLORONCOLOR);
			BLENDFUNCTION blendFunction;
			blendFunction.BlendFlags=0;
			blendFunction.AlphaFormat=0;
			blendFunction.BlendOp=AC_SRC_OVER;
			blendFunction.SourceConstantAlpha=255;
			if(bFlick){
				::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
				bFlick=0;
				float icw10=cw/float(XPIE);
				float ich10=ch/float(YPIE);
				float ibw10=bw/float(XPIE);
				float ibh10=bh/float(YPIE);
				switch(iEffect){
				case 0:
					{
						/*
						for(int i=0;i<255*3;i+=150){
							for(int x=0;x<bw;x++){
								for(int y=0;y<bh;y++){
									DWORD dwCol=GetPixel(hdc,x,y);
									int iB=GetRValue(dwCol)+GetGValue(dwCol)+GetBValue(dwCol);
									if(iB<i){
										SetPixel(hdc,x,y,GetPixel(bufdc,x,y));
									}
								}
							}
						}*/
						int i=0;
						DWORD dwDelay=70;
						DWORD dwDLx=5;
						for(i=0;i<10;i++){
							int iX=int(double(rand())/double(RAND_MAX)*(XPIE-1));
							int iY=int(double(rand())/double(RAND_MAX)*(YPIE-1));
							blendFunction.SourceConstantAlpha=5*i+155;
							AlphaBlendMy(hdc,iX*icw10,iY*ich10,icw10+2,ich10+2
								,bufdc,iX*ibw10,iY*ibh10,ibw10+2,ibh10+2,blendFunction);
							Sleep(10);
							//Sleep((dwDelay>dwDLx)?dwDelay-dwDLx:4);
							//dwDLx=dwDLx*2;
						}
						for(i=0;i<10;i++){
							int iX=int(double(rand())/double(RAND_MAX)*(XPIE-2));
							int iY=int(double(rand())/double(RAND_MAX)*(YPIE-2));
							blendFunction.SourceConstantAlpha=5*i+155;
							AlphaBlendMy(hdc,iX*icw10,iY*ich10,icw10*2+2,ich10*2+2
								,bufdc,iX*ibw10,iY*ibh10,ibw10*2+2,ibh10*2+2,blendFunction);
							Sleep(10);
							//Sleep((dwDelay>dwDLx)?dwDelay-dwDLx:4);
							//dwDLx=dwDLx*2;
						}
						for(i=0;i<10;i++){
							int iX=int(double(rand())/double(RAND_MAX)*(XPIE-4));
							int iY=int(double(rand())/double(RAND_MAX)*(YPIE-4));
							blendFunction.SourceConstantAlpha=5*i+155;
							AlphaBlendMy(hdc,iX*icw10,iY*ich10,icw10*4+3,ich10*4+2
								,bufdc,iX*ibw10,iY*ibh10,ibw10*4+2,ibh10*4+2,blendFunction);
							Sleep(10);
							//Sleep((dwDelay>dwDLx)?dwDelay-dwDLx:4);
							//dwDLx=dwDLx*2;
						}
						break;
					}
				case 1:
					{
						for(int iX=0;iX<XPIE;iX++){
							blendFunction.SourceConstantAlpha=100;
							AlphaBlendMy(hdc,(iX+1)*icw10,0,icw10,ch
								,bufdc,(iX+1)*ibw10,0,ibw10,bh,blendFunction);
							blendFunction.SourceConstantAlpha=255;
							AlphaBlendMy(hdc,iX*icw10,0,icw10+2,ch
								,bufdc,iX*ibw10,0,ibw10+2,bh,blendFunction);
							Sleep(50);
						}
						break;
					}
				case 2:
					{
						for(int iY=0;iY<YPIE;iY++){
							blendFunction.SourceConstantAlpha=100;
							AlphaBlendMy(hdc,0,(1+iY)*ich10,cw
								,ich10,bufdc,0,(1+iY)*ibh10,bw,ibh10,blendFunction);
							blendFunction.SourceConstantAlpha=255;
							AlphaBlendMy(hdc,0,iY*ich10,cw,ich10+2
								,bufdc,0,iY*ibh10,bw,ibh10+2,blendFunction);
							Sleep(50);
						}
						break;
					}
				case 3:
					{
						for(int iX=XPIE;iX>0;iX--){
							blendFunction.SourceConstantAlpha=100;
							AlphaBlendMy(hdc,(iX-1)*icw10,0,icw10,ch
								,bufdc,(iX-1)*ibw10,0,ibw10,bh,blendFunction);
							blendFunction.SourceConstantAlpha=255;
							AlphaBlendMy(hdc,iX*icw10,0,icw10+2,ch
								,bufdc,iX*ibw10,0,ibw10+2,bh,blendFunction);
							Sleep(50);
						}
						break;
					}
				case 4:
					{
						for(int iY=YPIE;iY>0;iY--){
							blendFunction.SourceConstantAlpha=100;
							AlphaBlendMy(hdc,0,(iY-1)*ich10,cw,ich10
								,bufdc,0,(iY-1)*ibh10,bw,ibh10,blendFunction);
							blendFunction.SourceConstantAlpha=255;
							AlphaBlendMy(hdc,0,iY*ich10,cw,ich10+2
								,bufdc,0,iY*ibh10,bw,ibh10+2,blendFunction);
							Sleep(50);
						}
						break;
					}
				case 5:
					{
						for(int iX0=0;iX0<XPIE*1.5;iX0++){
							for(int iY=0;iY<YPIE;iY++){
								int iX=iX0-iY;
								blendFunction.SourceConstantAlpha=100;
								AlphaBlendMy(hdc,(iX+1)*icw10,iY*ich10,icw10,ich10,bufdc,(iX+1)*ibw10,iY*ibh10,ibw10,ibh10,blendFunction);
								blendFunction.SourceConstantAlpha=255;
								AlphaBlendMy(hdc,iX*icw10,iY*ich10,icw10+2,ich10+2,bufdc,iX*ibw10,iY*ibh10,ibw10+2,ibh10+2,blendFunction);
							}
							Sleep(40);
						}
						break;
					}
				case 6:
					{
						for(int iX0=-XPIE;iX0<XPIE*1.5;iX0++){
							for(int iY=0;iY<YPIE;iY++){
								int iX=iX0+iY;
								if(iX<-1){
									continue;
								}
								blendFunction.SourceConstantAlpha=100;
								AlphaBlendMy(hdc,(iX+1)*icw10,iY*ich10,icw10+1,ich10+1,bufdc,(iX+1)*ibw10,iY*ibh10,ibw10+1,ibh10+1,blendFunction);
								blendFunction.SourceConstantAlpha=255;
								AlphaBlendMy(hdc,iX*icw10,iY*ich10,icw10+2,ich10+2,bufdc,iX*ibw10,iY*ibh10,ibw10+2,ibh10+2,blendFunction);
							}
							Sleep(40);
						}
						break;
					}
				case 7:
					{
						for(int iX0=XPIE*2;iX0>0;iX0--){
							for(int iY=0;iY<YPIE;iY++){
								int iX=iX0-iY;
								blendFunction.SourceConstantAlpha=100;
								AlphaBlendMy(hdc,(iX-1)*icw10,iY*ich10,icw10+1,ich10+1,bufdc,(iX-1)*ibw10,iY*ibh10,ibw10+1,ibh10+1,blendFunction);
								blendFunction.SourceConstantAlpha=255;
								AlphaBlendMy(hdc,iX*icw10,iY*ich10,icw10+2,ich10+2,bufdc,iX*ibw10,iY*ibh10,ibw10+2,ibh10+2,blendFunction);
							}
							Sleep(40);
						}
						break;
					}
				case 8:
					{
						for(int iX0=XPIE;iX0>-XPIE;iX0--){
							for(int iY=YPIE;iY>=0;iY--){
								int iX=iX0+iY;
								blendFunction.SourceConstantAlpha=100;
								AlphaBlendMy(hdc,(iX-1)*icw10,iY*ich10,icw10+1,ich10+1,bufdc,(iX-1)*ibw10,iY*ibh10,ibw10+1,ibh10+1,blendFunction);
								blendFunction.SourceConstantAlpha=255;
								AlphaBlendMy(hdc,iX*icw10,iY*ich10,icw10+2,ich10+2,bufdc,iX*ibw10,iY*ibh10,ibw10+2,ibh10+2,blendFunction);
							}
							Sleep(40);
						}
						break;
					}
				}
				::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				iEffect++;
				if(iEffect>8){
					iEffect=0;
				}
			}
			blendFunction.SourceConstantAlpha=255;
			AlphaBlendMy(hdc,0,0,cw,ch,bufdc,0,0,bw,bh,blendFunction);
			SelectObject(bufdc,hTmp);
			DeleteDC(bufdc);
		}
		LeaveCriticalSection(&lc);
	}
};

DWORD WINAPI PaintFast(LPVOID pIn)
{
	TSaverWindow* p=(TSaverWindow*)pIn;
	if(!p){
		return 0;
	}
	p->pBack=&p;
	if(p){
		p->OnTimer(1);
	}
	int num_monitors=GetNumberOfMons();
	while(p && IsWindow(p->hwnd)){
		InvalidateRect(p->hwnd,NULL,TRUE);
		DWORD dwTime1=GetTickCount();
		EnterCriticalSection(&p->lc);
		LeaveCriticalSection(&p->lc);
		if(p){
			if(p->id==-1 || num_monitors<=1 || (p->iMMCounter)%num_monitors==p->id){
				p->OnTimer(0);
			}
			(p->iMMCounter)++;
		}
		DWORD dwTime2=GetTickCount();
		long lSleep=g_dwTimeOut*1000-(dwTime2-dwTime1);
		if(lSleep>0){
			Sleep(lSleep);
		}
	}
	return 0;
}

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

BOOL VerifyPassword(HWND hwnd)
{ // Under NT, we return TRUE immediately. This lets the saver quit, and the system manages passwords.
	// Under '95, we call VerifyScreenSavePwd. This checks the appropriate registry key and, if necessary, pops up a verify dialog
	OSVERSIONINFO osv; osv.dwOSVersionInfoSize=sizeof(osv); GetVersionEx(&osv);
	if (osv.dwPlatformId==VER_PLATFORM_WIN32_NT) return TRUE;
	HINSTANCE hpwdcpl=::LoadLibrary(_T("PASSWORD.CPL"));
	if (hpwdcpl==NULL) {Debug(_T("Unable to load PASSWORD.CPL. Aborting"));return TRUE;}
	typedef BOOL (WINAPI *VERIFYSCREENSAVEPWD)(HWND hwnd);
	VERIFYSCREENSAVEPWD VerifyScreenSavePwd;
	VerifyScreenSavePwd=(VERIFYSCREENSAVEPWD)GetProcAddress(hpwdcpl,"VerifyScreenSavePwd");
	if (VerifyScreenSavePwd==NULL) {Debug(_T("Unable to get VerifyPwProc address. Aborting"));FreeLibrary(hpwdcpl);return TRUE;}
	Debug(_T("About to call VerifyPwProc")); BOOL bres=VerifyScreenSavePwd(hwnd); FreeLibrary(hpwdcpl);
	return bres;
}
void ChangePassword(HWND hwnd)
{ // This only ever gets called under '95, when started with the /a option.
	HINSTANCE hmpr=::LoadLibrary(_T("MPR.DLL"));
	if (hmpr==NULL) {Debug(_T("MPR.DLL not found: cannot change password."));return;}
	typedef VOID (WINAPI *PWDCHANGEPASSWORD) (LPCSTR lpcRegkeyname,HWND hwnd,UINT uiReserved1,UINT uiReserved2);
	PWDCHANGEPASSWORD PwdChangePassword=(PWDCHANGEPASSWORD)::GetProcAddress(hmpr,"PwdChangePasswordA");
	if (PwdChangePassword==NULL) {FreeLibrary(hmpr); Debug(_T("PwdChangeProc not found: cannot change password"));return;}
	PwdChangePassword("SCRSAVE",hwnd,0,0); FreeLibrary(hmpr);
}



void ReadGeneralRegistry()
{ 
	PasswordDelay=15; PasswordDelayIndex=0; MouseThreshold=50; IsDialogActive=false; // default values in case they're not in registry
	LONG res; HKEY skey; DWORD valtype, valsize, val;
	res=RegOpenKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_SETUP _T("\\Screen Savers"),0,KEY_READ,&skey);
	if (res!=ERROR_SUCCESS) return;
	valsize=sizeof(val); res=RegQueryValueEx(skey,_T("Effects"),0,&valtype,(LPBYTE)&val,&valsize); if (res==ERROR_SUCCESS) g_dwEffects=val;
	valsize=sizeof(val); res=RegQueryValueEx(skey,_T("TimeOut"),0,&valtype,(LPBYTE)&val,&valsize); if (res==ERROR_SUCCESS) g_dwTimeOut=val;
	PasswordDelay=0;//valsize=sizeof(val); res=RegQueryValueEx(skey,_T("Password Delay"),0,&valtype,(LPBYTE)&val,&valsize); if (res==ERROR_SUCCESS) PasswordDelay=val;
	PasswordDelayIndex=0;//valsize=sizeof(val); res=RegQueryValueEx(skey,_T("Password Delay Index"),0,&valtype,(LPBYTE)&val,&valsize); if (res==ERROR_SUCCESS) PasswordDelayIndex=val;
	MouseThreshold=15;//valsize=sizeof(val); res=RegQueryValueEx(skey,_T("Mouse Threshold"),0,&valtype,(LPBYTE)&val,&valsize);if (res==ERROR_SUCCESS) MouseThreshold=val;
	MouseThresholdIndex=1;//valsize=sizeof(val); res=RegQueryValueEx(skey,_T("Mouse Threshold Index"),0,&valtype,(LPBYTE)&val,&valsize);if (res==ERROR_SUCCESS) MouseThresholdIndex=val;
	MuteSound=0;//valsize=sizeof(val); res=RegQueryValueEx(skey,_T("Mute Sound"),0,&valtype,(LPBYTE)&val,&valsize);     if (res==ERROR_SUCCESS) MuteSound=val;
	//valsize=5*sizeof(TCHAR); res=RegQueryValueEx(skey,_T("Mouse Corners"),0,&valtype,(LPBYTE)Corners,&valsize);
	//for (int i=0; i<4; i++) {if (Corners[i]!='Y' && Corners[i]!='N') Corners[i]='-';} Corners[4]=0;
	RegCloseKey(skey);
	HotServices=FALSE;
	HINSTANCE sagedll=LoadLibrary(_T("sage.dll"));
	if (sagedll==0) sagedll=LoadLibrary(_T("scrhots.dll"));
	if (sagedll!=0)
	{ 
		typedef BOOL (WINAPI *SYSTEMAGENTDETECT)();
		SYSTEMAGENTDETECT detectproc=(SYSTEMAGENTDETECT)GetProcAddress(sagedll,"System_Agent_Detect");
		if (detectproc!=NULL) HotServices=detectproc();
		FreeLibrary(sagedll);
	}
}

void WriteGeneralRegistry()
{ 
	LONG res; HKEY skey; DWORD val;
	res=RegCreateKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_SETUP _T("\\Screen Savers"),0,0,0,KEY_ALL_ACCESS,0,&skey,0);
	if (res!=ERROR_SUCCESS) return;
	val=g_dwEffects; RegSetValueEx(skey,_T("Effects"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
	val=g_dwTimeOut; RegSetValueEx(skey,_T("TimeOut"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
	/*
	val=PasswordDelay; RegSetValueEx(skey,_T("Password Delay"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
	val=PasswordDelayIndex; RegSetValueEx(skey,_T("Password Delay Index"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
	val=MouseThreshold; RegSetValueEx(skey,_T("Mouse Threshold"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
	val=MouseThresholdIndex; RegSetValueEx(skey,_T("Mouse Threshold Index"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
	val=MuteSound?1:0; RegSetValueEx(skey,_T("Mute Sound"),0,REG_DWORD,(const BYTE*)&val,sizeof(val));
	RegSetValueEx(skey,_T("Mouse Corners"),0,REG_SZ,(const BYTE*)Corners,5*sizeof(TCHAR));
	*/
	RegCloseKey(skey);
	//
	HINSTANCE sagedll=LoadLibrary(_T("sage.dll"));
	if (sagedll==0) sagedll=LoadLibrary(_T("scrhots.dll"));
	if (sagedll!=0)
	{ 
		typedef VOID (WINAPI *SCREENSAVERCHANGED)();
		SCREENSAVERCHANGED changedproc=(SCREENSAVERCHANGED)GetProcAddress(sagedll,"Screen_Saver_Changed");
		if (changedproc!=NULL) changedproc();
		FreeLibrary(sagedll);
	}
}



LRESULT CALLBACK SaverWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ 
	TSaverWindow *sav=0; int id=0; HWND hmain=0;
	#pragma warning( push )
	#pragma warning( disable: 4244 4312 )
	if (msg==WM_CREATE)
	{ 
		CREATESTRUCT *cs=(CREATESTRUCT*)lParam; id=*(int*)cs->lpCreateParams; 
		SetWindowLong(hwnd,0,id);
		sav = new TSaverWindow(hwnd,id); 
		SetWindowLong(hwnd,GWL_USERDATA,(LONG)sav);
		SaverWindow.push_back(sav);
	}
	else
	{ 
		sav=(TSaverWindow*)GetWindowLong(hwnd,GWL_USERDATA);
		id=GetWindowLong(hwnd,0);
	}
	#pragma warning( pop )
	if (id<=0){
		hmain=hwnd;
	}else {
		hmain=SaverWindow[0]->hwnd;
	}
	if (msg==WM_PAINT)
	{
		PAINTSTRUCT ps; BeginPaint(hwnd,&ps); RECT rc; 
		GetClientRect(hwnd,&rc); 
		if (sav!=0){
			sav->OnPaint(ps.hdc,rc); 
			//::PlaySound("SystemQuestion", NULL, SND_ALIAS | SND_NODEFAULT | SND_ASYNC | SND_NOWAIT);
		}else{
			//::PlaySound("SystemQuestion", NULL, SND_ALIAS | SND_NODEFAULT | SND_ASYNC | SND_NOWAIT);
		}
		EndPaint(hwnd,&ps);
	}
	else if (sav!=0) sav->OtherWndProc(msg,wParam,lParam);
	//
	switch (msg)
	{ 
	case WM_ACTIVATE: case WM_ACTIVATEAPP: case WM_NCACTIVATE:
		{ 
			CoInitialize(0);
			TCHAR pn[100]; GetClassName((HWND)lParam,pn,100); 
			bool ispeer = (_tcsncmp(pn,_T("ScrClass"),8)==0);
			if (ScrMode==smSaver && !IsDialogActive && LOWORD(wParam)==WA_INACTIVE && !ispeer && !SCRDEBUG)
			{ 
				Debug(_T("WM_ACTIVATE: about to inactive window, so sending close")); ReallyClose=true; PostMessage(hmain,WM_CLOSE,0,0); return 0;
			}
		} break;
	case WM_SETCURSOR:
		{ 
			if (ScrMode==smSaver && !IsDialogActive && !SCRDEBUG) SetCursor(NULL);
			else SetCursor(LoadCursor(NULL,IDC_ARROW));
		} return 0;
	case WM_LBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDOWN: case WM_KEYDOWN:
		{ 
			if (ScrMode==smSaver && !IsDialogActive) {Debug(_T("WM_BUTTONDOWN: sending close")); ReallyClose=true; PostMessage(hmain,WM_CLOSE,0,0); return 0;}
		} break;
	case WM_MOUSEMOVE:
		{ 
			if (ScrMode==smSaver && !IsDialogActive && !SCRDEBUG)
			{ 
				POINT pt; GetCursorPos(&pt);
				int dx=pt.x-InitCursorPos.x; if (dx<0) dx=-dx; int dy=pt.y-InitCursorPos.y; if (dy<0) dy=-dy;
				if (dx>(int)MouseThreshold || dy>(int)MouseThreshold)
				{ 
					Debug(_T("WM_MOUSEMOVE: gone beyond threshold, sending close")); ReallyClose=true; PostMessage(hmain,WM_CLOSE,0,0);
				}
			}
		} return 0;
	case (WM_SYSCOMMAND):
		{ 
			if (ScrMode==smSaver)
			{ 
				if (wParam==SC_SCREENSAVE) {Debug(_T("WM_SYSCOMMAND: gobbling up a SC_SCREENSAVE to stop a new saver from running."));return 0;}
				if (wParam==SC_CLOSE && !SCRDEBUG) {Debug(_T("WM_SYSCOMMAND: gobbling up a SC_CLOSE"));return 0;}
			}
		} break;
	case (WM_CLOSE):
		{ 
			CoUninitialize();
			if (id>0) return 0; // secondary windows ignore this message
			if (id==-1) {DestroyWindow(hwnd); return 0;} // preview windows close obediently
			if (ReallyClose && !IsDialogActive)
			{ 
				Debug(_T("WM_CLOSE: maybe we need a password"));
				BOOL CanClose=TRUE;
				if (GetTickCount()-InitTime > 1000*PasswordDelay)
				{ 
					IsDialogActive=true; SendMessage(hwnd,WM_SETCURSOR,0,0);
					CanClose=VerifyPassword(hwnd);
					IsDialogActive=false; SendMessage(hwnd,WM_SETCURSOR,0,0); GetCursorPos(&InitCursorPos);
				}
				// note: all secondary monitors are owned by the primary. And we're the primary (id==0)
				// therefore, when we destroy ourself, windows will destroy the others first
				if (CanClose) {Debug(_T("WM_CLOSE: doing a DestroyWindow")); DestroyWindow(hwnd);}
				else {Debug(_T("WM_CLOSE: but failed password, so doing nothing"));}
			}
		} return 0;
	case (WM_DESTROY):
		{ 
			Debug(_T("WM_DESTROY."));
			SetWindowLong(hwnd,0,0); SetWindowLong(hwnd,GWL_USERDATA,0);
			for (vector<TSaverWindow*>::iterator i=SaverWindow.begin(); i!=SaverWindow.end(); i++) 
			{
				if (sav==*i)
					*i=0;
			}
			delete sav;
			if ((id<=0 && ScrMode==smSaver) || ScrMode==smPreview) 
				PostQuitMessage(0);
		} break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}






// ENUM-MONITOR-CALLBACK is part of DoSaver. Its stuff is in windef.h but
// requires WINVER>=0x0500. Well, we're doing LoadLibrary, so we know it's
// safe...
#ifndef SM_CMONITORS
DECLARE_HANDLE(HMONITOR);
#endif
//
BOOL CALLBACK EnumMonitorCallback(HMONITOR,HDC,LPRECT rc,LPARAM)
{ 
	if (rc->left==0 && rc->top==0) monitors.insert(monitors.begin(),*rc);
	else monitors.push_back(*rc);
	return TRUE;
}

void DoSaver(HWND hparwnd, bool fakem)
{ 
	int num_monitors=GetNumberOfMons(); // 80=SM_CMONITORS
	RECT rcmainScre; rcmainScre.left=0; rcmainScre.top=0; rcmainScre.right=GetSystemMetrics(SM_CXSCREEN); rcmainScre.bottom=GetSystemMetrics(SM_CYSCREEN);
	if (ScrMode==smPreview)
	{ 
		RECT rc; GetWindowRect(hparwnd,&rc); monitors.push_back(rc);
	}
	else if (fakem)
	{ 
		int w=GetSystemMetrics(SM_CXSCREEN), x1=w/4, x2=w*2/3, h=x2-x1; RECT rc;
		rc.left=x1; rc.top=x1; rc.right=x1+h; rc.bottom=x1+h; monitors.push_back(rc);
		rc.left=0; rc.top=x1; rc.right=x1; rc.bottom=x1+x1; monitors.push_back(rc);
		rc.left=x2; rc.top=x1+h+x2-w; rc.right=w; rc.bottom=x1+h; monitors.push_back(rc);
	}
	else
	{ 
		if (num_monitors>1)
		{ 
			typedef BOOL (CALLBACK *LUMONITORENUMPROC)(HMONITOR,HDC,LPRECT,LPARAM);
			typedef BOOL (WINAPI *LUENUMDISPLAYMONITORS)(HDC,LPCRECT,LUMONITORENUMPROC,LPARAM);
			HINSTANCE husr=LoadLibrary(_T("user32.dll"));
			LUENUMDISPLAYMONITORS pEnumDisplayMonitors=0;
			if (husr!=NULL) pEnumDisplayMonitors=(LUENUMDISPLAYMONITORS)GetProcAddress(husr,"EnumDisplayMonitors");
			if (pEnumDisplayMonitors!=NULL) (*pEnumDisplayMonitors)(NULL,NULL,EnumMonitorCallback,NULL);
			if (husr!=NULL) FreeLibrary(husr);
		}
		if (monitors.size()==0)
		{ 
			monitors.push_back(rcmainScre);
		}
	}
	//
	HWND hwnd=0;
	monitorSizes.clear();
	SIZE sizeMon;
	sizeMon.cx=rcmainScre.right-rcmainScre.left;
	sizeMon.cy=rcmainScre.bottom-rcmainScre.top;
	monitorSizes.push_back(sizeMon);
	if (ScrMode==smPreview)
	{ 
		RECT rc; 
		GetWindowRect(hparwnd,&rc); int w=rc.right-rc.left, h=rc.bottom-rc.top;  
		int id=-1;
		hwnd=CreateWindowEx(0,_T("ScrClass"),_T(""),WS_CHILD|WS_VISIBLE,0,0,w,h,hparwnd,NULL,hInstance,&id);
	}
	else
	{ 
		GetCursorPos(&InitCursorPos); InitTime=GetTickCount();
		for (int i=0; i<(int)monitors.size(); i++)
		{ 
			const RECT &rc = monitors[i];
			DWORD exstyle=WS_EX_TOPMOST; if (SCRDEBUG) exstyle=0;
			int id=i;
			if(monitors.size()==1){
				// Только один монитор
				id=-1;
			}else{
				SIZE sizeMon2;
				sizeMon2.cx=rc.right-rc.left;
				sizeMon2.cy=rc.bottom-rc.top;
				monitorSizes.push_back(sizeMon2);
			}
			HWND hthis = CreateWindowEx(exstyle,_T("ScrClass"),_T(""),WS_POPUP|WS_VISIBLE,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,hwnd,NULL,hInstance,&id);
			if (i==0){
				hwnd=hthis;
			}
		}
	}
	if (hwnd!=NULL)
	{ 
		UINT oldval;
		if (ScrMode==smSaver) SystemParametersInfo(SPI_SETSCREENSAVERRUNNING,1,&oldval,0);
		MSG msg;
		while (GetMessage(&msg,NULL,0,0))
		{ 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (ScrMode==smSaver) SystemParametersInfo(SPI_SETSCREENSAVERRUNNING,0,&oldval,0);
	}
	//
	SaverWindow.clear();
	return;
}




BOOL CALLBACK GeneralDlgProc(HWND hwnd,UINT msg,WPARAM,LPARAM lParam)
{ 
	switch (msg)
	{ 
	case (WM_INITDIALOG):
		{ 
			ShowWindow(GetDlgItem(hwnd,HotServices?102:101),SW_HIDE);
			SetDlgItemText(hwnd,112,Corners);
			SendDlgItemMessage(hwnd,109,CB_ADDSTRING,0,(LPARAM)_T("seconds"));
			SendDlgItemMessage(hwnd,109,CB_ADDSTRING,0,(LPARAM)_T("minutes"));
			SendDlgItemMessage(hwnd,109,CB_SETCURSEL,PasswordDelayIndex,0);
			int n=PasswordDelay; if (PasswordDelayIndex==1) n/=60;
			TCHAR c[16]; wsprintf(c,_T("%i"),n); SetDlgItemText(hwnd,107,c);
			SendDlgItemMessage(hwnd,108,UDM_SETRANGE,0,MAKELONG(99,0));
			SendDlgItemMessage(hwnd,105,CB_ADDSTRING,0,(LPARAM)_T("High"));
			SendDlgItemMessage(hwnd,105,CB_ADDSTRING,0,(LPARAM)_T("Normal"));
			SendDlgItemMessage(hwnd,105,CB_ADDSTRING,0,(LPARAM)_T("Low"));
			SendDlgItemMessage(hwnd,105,CB_ADDSTRING,0,(LPARAM)_T("Keyboard only (ignore mouse movement)"));
			SendDlgItemMessage(hwnd,105,CB_SETCURSEL,MouseThresholdIndex,0);
			if (MuteSound) CheckDlgButton(hwnd,113,BST_CHECKED);
			OSVERSIONINFO ver; ZeroMemory(&ver,sizeof(ver)); ver.dwOSVersionInfoSize=sizeof(ver); GetVersionEx(&ver);
			for (int i=106; i<111 && ver.dwPlatformId!=VER_PLATFORM_WIN32_WINDOWS; i++) ShowWindow(GetDlgItem(hwnd,i),SW_HIDE); 
		} return TRUE;
    case (WM_NOTIFY):
		{ 
			LPNMHDR nmh=(LPNMHDR)lParam; UINT code=nmh->code;
			switch (code)
			{ 
			case (PSN_APPLY):
				{ 
					GetDlgItemText(hwnd,112,Corners,5);                   
					PasswordDelayIndex=SendDlgItemMessage(hwnd,109,CB_GETCURSEL,0,0);
					TCHAR c[16]; GetDlgItemText(hwnd,107,c,16); int n=_ttoi(c); if (PasswordDelayIndex==1) n*=60; PasswordDelay=n;
					MouseThresholdIndex=SendDlgItemMessage(hwnd,105,CB_GETCURSEL,0,0);
					if (MouseThresholdIndex==0) MouseThreshold=0;
					else if (MouseThresholdIndex==1) MouseThreshold=200;
					else if (MouseThresholdIndex==2) MouseThreshold=400;
					else MouseThreshold=999999;
					MuteSound = (IsDlgButtonChecked(hwnd,113)==BST_CHECKED);
					WriteGeneralRegistry();
					SetWindowLong(hwnd,DWL_MSGRESULT,PSNRET_NOERROR);
				} return TRUE;
			}
		} return FALSE;
	}
	return FALSE;
}



//
// MONITOR CONTROL -- either corners or a preview
//

LRESULT CALLBACK MonitorWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ 
	switch (msg)
	{ 
	case WM_CREATE:
		{ 
			TCHAR c[5]; GetWindowText(hwnd,c,5); if (*c!=0) return 0;
			int id=-1; RECT rc; SendMessage(hwnd,SCRM_GETMONITORAREA,0,(LPARAM)&rc);
			CreateWindow(_T("ScrClass"),_T(""),WS_CHILD|WS_VISIBLE,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,hwnd,NULL,hInstance,&id);
		} return 0;
    case WM_PAINT:
		{ 
			if (hbmmonitor==0) hbmmonitor=LoadBitmap(hInstance,_T("Monitor"));
			RECT rc; GetClientRect(hwnd,&rc);
			//
			PAINTSTRUCT ps; BeginPaint(hwnd,&ps);
			HBITMAP hback = (HBITMAP)GetWindowLong(hwnd,GWL_USERDATA);
			if (hback!=0)
			{ 
				BITMAP bmp; GetObject(hback,sizeof(bmp),&bmp);
				if (bmp.bmWidth!=rc.right || bmp.bmHeight!=rc.bottom) {DeleteObject(hback); hback=0;}
			}
			if (hback==0) {hback=CreateCompatibleBitmap(ps.hdc,rc.right,rc.bottom); SetWindowLong(hwnd,GWL_USERDATA,(LONG)hback);}
			HDC backdc=CreateCompatibleDC(ps.hdc);
			HGDIOBJ holdback=SelectObject(backdc,hback);
			BitBlt(backdc,0,0,rc.right,rc.bottom,ps.hdc,0,0,SRCCOPY);
			//
			TCHAR corners[5]; GetWindowText(hwnd,corners,5);
			HDC hdc=CreateCompatibleDC(ps.hdc);
			HGDIOBJ hold=SelectObject(hdc,hbmmonitor);
			StretchBlt(backdc,0,0,rc.right,rc.bottom,hdc,0,0,184,170,SRCAND);
			StretchBlt(backdc,0,0,rc.right,rc.bottom,hdc,184,0,184,170,SRCINVERT);
			RECT crc; SendMessage(hwnd,SCRM_GETMONITORAREA,0,(LPARAM)&crc);
			//
			if (*corners!=0) FillRect(backdc,&crc,GetSysColorBrush(COLOR_DESKTOP));
			for (int i=0; i<4 && *corners!=0; i++)
			{ 
				RECT crc; SendMessage(hwnd,SCRM_GETMONITORAREA,i+1,(LPARAM)&crc);
				int y=0; if (corners[i]=='Y') y=22; else if (corners[i]=='N') y=44;
				BitBlt(backdc,crc.left,crc.top,crc.right-crc.left,crc.bottom-crc.top,hdc,368,y,SRCCOPY);
				if (!HotServices) 
				{ 
					DWORD col=GetSysColor(COLOR_DESKTOP);
					for (int y=crc.top; y<crc.bottom; y++)
					{ 
						for (int x=crc.left+(y&1); x<crc.right; x+=2) SetPixel(backdc,x,y,col);
					}
				}
			}
			SelectObject(hdc,hold);
			DeleteDC(hdc);
			//
			BitBlt(ps.hdc,0,0,rc.right,rc.bottom,backdc,0,0,SRCCOPY);
			SelectObject(backdc,holdback);
			DeleteDC(backdc);
			EndPaint(hwnd,&ps);
		} return 0;
    case SCRM_GETMONITORAREA:
		{ 
			RECT *prc=(RECT*)lParam;
			if (hbmmonitor==0) hbmmonitor=LoadBitmap(hInstance,_T("Monitor"));
			// those are the client coordinates unscalled
			RECT wrc; GetClientRect(hwnd,&wrc); int ww=wrc.right, wh=wrc.bottom;
			RECT rc; rc.left=16*ww/184; rc.right=168*ww/184; rc.top=17*wh/170; rc.bottom=130*wh/170;
			*prc=rc; if (wParam==0) return 0;
			if (wParam==1) {prc->right=rc.left+24; prc->bottom=rc.top+22;}
			else if (wParam==2) {prc->left=rc.right-24; prc->bottom=rc.top+22;}
			else if (wParam==3) {prc->left=rc.right-24; prc->top=rc.bottom-22;}
			else if (wParam==4) {prc->right=rc.left+24; prc->top=rc.bottom-22;}
		} return 0;
    case WM_LBUTTONDOWN:
		{ 
			if (!HotServices) return 0;
			int x=LOWORD(lParam), y=HIWORD(lParam);
			TCHAR corners[5]; GetWindowText(hwnd,corners,5);
			if (corners[0]==0) return 0;
			int click=-1; for (int i=0; i<4; i++)
			{ 
				RECT rc; SendMessage(hwnd,SCRM_GETMONITORAREA,i+1,(LPARAM)&rc);
				if (x>=rc.left && y>=rc.top && x<rc.right && y<rc.bottom) {click=i; break;}
			}
			if (click==-1) return 0;
			for (int j=0; j<4; j++)
			{ 
				if (corners[j]!='-' && corners[j]!='Y' && corners[j]!='N') corners[j]='-';
			}
			corners[4]=0;
			//
			HMENU hmenu=CreatePopupMenu();
			MENUITEMINFO mi; ZeroMemory(&mi,sizeof(mi)); mi.cbSize=sizeof(MENUITEMINFO);
			mi.fMask=MIIM_TYPE|MIIM_ID|MIIM_STATE|MIIM_DATA;
			mi.fType=MFT_STRING|MFT_RADIOCHECK;
			mi.wID='N'; mi.fState=MFS_ENABLED; if (corners[click]=='N') mi.fState|=MFS_CHECKED;
			mi.dwTypeData=_T("Never"); mi.cch=sizeof(TCHAR)*_tcslen(mi.dwTypeData);
			InsertMenuItem(hmenu,0,TRUE,&mi);
			mi.wID='Y'; mi.fState=MFS_ENABLED; if (corners[click]=='Y') mi.fState|=MFS_CHECKED;
			mi.dwTypeData=_T("Now"); mi.cch=sizeof(TCHAR)*_tcslen(mi.dwTypeData);
			InsertMenuItem(hmenu,0,TRUE,&mi);
			mi.wID='-'; mi.fState=MFS_ENABLED; if (corners[click]!='Y' && corners[click]!='N') mi.fState|=MFS_CHECKED;
			mi.dwTypeData=_T("Default"); mi.cch=sizeof(TCHAR)*_tcslen(mi.dwTypeData);
			InsertMenuItem(hmenu,0,TRUE,&mi);
			POINT pt; pt.x=x; pt.y=y; ClientToScreen(hwnd,&pt);
			int cmd = TrackPopupMenuEx(hmenu,TPM_RETURNCMD|TPM_RIGHTBUTTON,pt.x,pt.y,hwnd,NULL);
			if (cmd!=0) corners[click]=(char)cmd;
			corners[4]=0; SetWindowText(hwnd,corners);
			InvalidateRect(hwnd,NULL,FALSE);
		} return 0;
    case WM_DESTROY:
		{ 
			HBITMAP hback = (HBITMAP)SetWindowLong(hwnd,GWL_USERDATA,0);
			if (hback!=0) DeleteObject(hback);
		} return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}


BOOL CALLBACK AboutDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM)
{ 
	if (msg==WM_INITDIALOG)
	{ 
		SetDlgItemText(hdlg,101,SaverName.c_str());
		SetDlgItemUrl(hdlg,102,_T("http://www.wiredplane.com/wirechanger/info.shtml"));
		SetDlgItemText(hdlg,102,_T("http://www.wiredplane.com/wirechanger/info.shtml"));
		return TRUE;
	}
	else if (msg==WM_COMMAND)
	{ 
		int id=LOWORD(wParam);
		if (id==IDOK || id==IDCANCEL) EndDialog(hdlg,id);
		return TRUE;
	} 
	else return FALSE;
}


//
// PROPERTY SHEET SUBCLASSING -- this is to stick an "About" option on the sysmenu.
//
WNDPROC OldSubclassProc=0;
LRESULT CALLBACK PropertysheetSubclassProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ 
	if (msg==WM_SYSCOMMAND && wParam==3500)
	{ 
		DialogBox(hInstance,_T("DLG_ABOUT"),hwnd,AboutDlgProc);
		return 0;
	} 
	if (OldSubclassProc!=NULL) return CallWindowProc(OldSubclassProc,hwnd,msg,wParam,lParam);
	else return DefWindowProc(hwnd,msg,wParam,lParam);
}

int CALLBACK PropertysheetCallback(HWND hwnd,UINT msg,LPARAM)
{ 
	if (msg!=PSCB_INITIALIZED) return 0;
	HMENU hsysmenu=GetSystemMenu(hwnd,FALSE);
	AppendMenu(hsysmenu,MF_SEPARATOR,1,_T("-"));
	AppendMenu(hsysmenu,MF_STRING,3500,_T("About..."));
	OldSubclassProc=(WNDPROC)SetWindowLong(hwnd,GWL_WNDPROC,(LONG)PropertysheetSubclassProc);
	return 0;
}


void DoConfig(HWND hpar)
{ 
	DWORD dwTarget=0;
	UINT iWM_SSAVE=RegisterWindowMessage("WC_SHOWSSAVE_OPTS");
	BroadcastSystemMessage(BSF_POSTMESSAGE | BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK, &dwTarget, iWM_SSAVE, WPARAM(0), LPARAM(0));
	return;
	/*
	hiconbg = (HICON)LoadImage(hInstance,MAKEINTRESOURCE(1),IMAGE_ICON,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),0);
	hiconsm = (HICON)LoadImage(hInstance,MAKEINTRESOURCE(1),IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),0);
	//  
	PROPSHEETHEADER psh; ZeroMemory(&psh,sizeof(psh));
	PROPSHEETPAGE psp[1]; ZeroMemory(psp,sizeof(PROPSHEETPAGE));
	psp[0].dwSize=sizeof(psp[0]);
	psp[0].dwFlags=PSP_DEFAULT;
	psp[0].hInstance=hInstance; 
	psp[0].pszTemplate=_T("DLG_GENERAL");
	psp[0].pfnDlgProc=GeneralDlgProc;
	psh.dwSize=sizeof(psh);
	psh.dwFlags=PSH_NOAPPLYNOW | PSH_PROPSHEETPAGE | PSH_USEHICON | PSH_USECALLBACK;
	psh.hwndParent=hpar;
	psh.hInstance=hInstance;
	psh.hIcon=hiconsm;
	tstring cap=_T("Options for ")+SaverName; psh.pszCaption=cap.c_str();
	psh.nPages=1;
	psh.nStartPage=0;
	psh.ppsp=psp;
	psh.pfnCallback=PropertysheetCallback;
	Debug(_T("Config..."));
	PropertySheet(&psh);
	Debug(_T("Config done."));
	if (hiconbg!=0) DestroyIcon(hiconbg); hiconbg=0;
	if (hiconsm!=0) DestroyIcon(hiconsm); hiconsm=0;
	if (hbmmonitor!=0) DeleteObject(hbmmonitor); hbmmonitor=0;
	*/
}


// This routine is for using ScrPrev. It's so that you can start the saver
// with the command line /p scrprev and it runs itself in a preview window.
// You must first copy ScrPrev somewhere in your search path
HWND CheckForScrprev()
{ 
	HWND hwnd=FindWindow(_T("Scrprev"),NULL); // looks for the Scrprev class
	if (hwnd==NULL) // try to load it
	{ 
		STARTUPINFO si; PROCESS_INFORMATION pi; ZeroMemory(&si,sizeof(si)); ZeroMemory(&pi,sizeof(pi));
		si.cb=sizeof(si);
		TCHAR cmd[MAX_PATH]; _tcscpy(cmd,_T("Scrprev")); // unicode CreateProcess requires it writeable
		BOOL cres=CreateProcess(NULL,cmd,0,0,FALSE,CREATE_NEW_PROCESS_GROUP | CREATE_DEFAULT_ERROR_MODE,
			0,0,&si,&pi);
		if (!cres) {Debug(_T("Error creating scrprev process")); return NULL;}
		DWORD wres=WaitForInputIdle(pi.hProcess,2000);
		if (wres==WAIT_TIMEOUT) {Debug(_T("Scrprev never becomes idle")); return NULL;}
		if (wres==0xFFFFFFFF) {Debug(_T("ScrPrev, misc error after ScrPrev execution"));return NULL;}
		hwnd=FindWindow(_T("Scrprev"),NULL);
	}
	if (hwnd==NULL) {Debug(_T("Unable to find Scrprev window")); return NULL;}
	::SetForegroundWindow(hwnd);
	hwnd=GetWindow(hwnd,GW_CHILD);
	if (hwnd==NULL) {Debug(_T("Couldn't find Scrprev child")); return NULL;}
	return hwnd;
}


void DoInstall(BOOL bUI=1)
{ 
	TCHAR windir[MAX_PATH]; GetWindowsDirectory(windir,MAX_PATH);
	TCHAR tfn[MAX_PATH]; UINT ures=GetTempFileName(windir,_T("pst"),0,tfn);
	if (ures==0) {
		if(bUI){
			MessageBox(NULL,_T("You must be logged on as system administrator to install screen savers"),_T("WCSaver Install"),MB_ICONINFORMATION|MB_OK); 
		}
		return;
	}
	DeleteFile(tfn);
	tstring fn=tstring(windir)+_T("\\")+SaverName+_T(".scr");
	DWORD attr = GetFileAttributes(fn.c_str());
	/*if(bUI){
	bool exists = (attr!=0xFFFFFFFF);
	tstring msg=_T("Do you want to install '")+SaverName+_T("' ?");
	if (exists) msg+=_T("\r\n\r\n(This will replace the version that you have currently)");
		int res=MessageBox(NULL,msg.c_str(),_T("Install this screensaver"),MB_YESNOCANCEL);
		if (res!=IDYES)
			return;
	}*/
	::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, TRUE);
	::WritePrivateProfileString(_T("boot"), _T("SCRNSAVE.EXE"), fn.c_str(), _T("system.ini"));

	TCHAR cfn[MAX_PATH];
	GetModuleFileName(hInstance,cfn,MAX_PATH);
	SetCursor(LoadCursor(NULL,IDC_WAIT));
	BOOL bres = CopyFile(cfn,fn.c_str(),FALSE);
	if (!bres)
	{ 
		if(bUI){
			tstring msg = _T("Screensaver is active or already installed");//+GetLastErrorString()
			MessageBox(NULL,msg.c_str(),_T("WCSaver Install"),MB_ICONERROR|MB_OK);
		}
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		return;
	}
	/*
	LONG lres; HKEY skey; DWORD disp; tstring val;
	tstring key=REGSTR_PATH_UNINSTALL _T("\\")+SaverName;
	lres=RegCreateKeyEx(HKEY_LOCAL_MACHINE,key.c_str(),0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
	if (lres==ERROR_SUCCESS)
	{ 
		val="WireChanger slide show"; RegSetValueEx(skey,_T("DisplayName"),0,REG_SZ,(const BYTE*)val.c_str(),sizeof(TCHAR)*(val.length()+1));
		val=_T("\"")+fn+_T("\" /u"); RegSetValueEx(skey,_T("UninstallString"),0,REG_SZ,(const BYTE*)val.c_str(),sizeof(TCHAR)*(val.length()+1));
		RegSetValueEx(skey,_T("UninstallPath"),0,REG_SZ,(const BYTE*)val.c_str(),sizeof(TCHAR)*(val.length()+1));
		val=_T("\"")+fn+_T("\""); RegSetValueEx(skey,_T("ModifyPath"),0,REG_SZ,(const BYTE*)val.c_str(),sizeof(TCHAR)*(val.length()+1));
		val=fn; RegSetValueEx(skey,_T("DisplayIcon"),0,REG_SZ,(const BYTE*)val.c_str(),sizeof(TCHAR)*(val.length()+1));
		TCHAR url[1024]="http://www.wiredplane.com/wirechanger/info.shtml";
		RegSetValueEx(skey,_T("HelpLink"),0,REG_SZ,(const BYTE*)url,sizeof(TCHAR)*(_tcslen(url)+1));
		RegCloseKey(skey);
	}
	*/
	if(bUI){
		SHELLEXECUTEINFO sex; ZeroMemory(&sex,sizeof(sex)); sex.cbSize=sizeof(sex);
		sex.fMask=SEE_MASK_NOCLOSEPROCESS;
		sex.lpVerb=_T("install");
		sex.lpFile=fn.c_str();
		sex.nShow=SW_SHOWNORMAL;
		bres = ShellExecuteEx(&sex);
		if (!bres) {
			SetCursor(LoadCursor(NULL,IDC_ARROW)); 
			if(bUI){
				MessageBox(NULL,_T("The saver has been installed"),SaverName.c_str(),MB_OK); 
			}
			return;
		}
		WaitForInputIdle(sex.hProcess,2000);
		CloseHandle(sex.hProcess);
	}else{
		LONG res; HKEY skey;
		res=RegCreateKeyEx(HKEY_CURRENT_USER,("Control Panel\\Desktop"),0,0,0,KEY_ALL_ACCESS,0,&skey,0);
		if (res!=ERROR_SUCCESS) return;
		char szPath[MAX_PATH]={0};
		GetShortPathName(fn.c_str(),szPath,sizeof(szPath));
		RegSetValueEx(skey,_T("SCRNSAVE.EXE"),0,REG_SZ,(const BYTE*)szPath,sizeof(szPath));
		RegCloseKey(skey);
	}
	SetCursor(LoadCursor(NULL,IDC_ARROW));
}


void DoUninstall()
{ 
	tstring key=REGSTR_PATH_UNINSTALL _T("\\")+SaverName;
	RegDeleteKey(HKEY_LOCAL_MACHINE,key.c_str());
	TCHAR fn[MAX_PATH]; GetModuleFileName(hInstance,fn,MAX_PATH);
	SetFileAttributes(fn,FILE_ATTRIBUTE_NORMAL); // remove readonly if necessary
	BOOL res = MoveFileEx(fn,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
	//
	const TCHAR *c=fn, *lastslash=c;
	while (*c!=0) {if (*c=='\\' || *c=='/') lastslash=c+1; c++;}
	tstring cap=SaverName+_T(" uninstaller");
	tstring msg;
	if (res) msg=_T("Uninstall completed. The saver will be removed next time you reboot.");
	else msg=_T("There was a problem uninstalling.\r\n")
		_T("To complete the uninstall manually, you should go into your Windows ")
		_T("directory and delete the file '")+tstring(lastslash)+_T("'");
	MessageBox(NULL,msg.c_str(),cap.c_str(),MB_OK);
}




// --------------------------------------------------------------------------------
// SetDlgItemUrl(hwnd,IDC_MYSTATIC,"http://www.wischik.com/lu");
//   This routine turns a dialog's static text control into an underlined hyperlink.
//   You can call it in your WM_INITDIALOG, or anywhere.
//   It will also set the text of the control... if you want to change the text
//   back, you can just call SetDlgItemText() afterwards.
// --------------------------------------------------------------------------------
void SetDlgItemUrl(HWND hdlg,int id,const TCHAR *url); 

// Implementation notes:
// We have to subclass both the static control (to set its cursor, to respond to click)
// and the dialog procedure (to set the font of the static control). Here are the two
// subclasses:
LRESULT CALLBACK UrlCtlProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK UrlDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
// When the user calls SetDlgItemUrl, then the static-control-subclass is added
// if it wasn't already there, and the dialog-subclass is added if it wasn't
// already there. Both subclasses are removed in response to their respective
// WM_DESTROY messages. Also, each subclass stores a property in its window,
// which is a HGLOBAL handle to a GlobalAlloc'd structure:
typedef struct {TCHAR *url; WNDPROC oldproc; HFONT hf; HBRUSH hb;} TUrlData;
// I'm a miser and only defined a single structure, which is used by both
// the control-subclass and the dialog-subclass. Both of them use 'oldproc' of course.
// The control-subclass only uses 'url' (in response to WM_LBUTTONDOWN),
// and the dialog-subclass only uses 'hf' and 'hb' (in response to WM_CTLCOLORSTATIC)
// There is one sneaky thing to note. We create our underlined font *lazily*.
// Initially, hf is just NULL. But the first time the subclassed dialog received
// WM_CTLCOLORSTATIC, we sneak a peak at the font that was going to be used for
// the control, and we create our own copy of it but including the underline style.
// This way our code works fine on dialogs of any font.

// SetDlgItemUrl: this is the routine that sets up the subclassing.
void SetDlgItemUrl(HWND hdlg,int id,const TCHAR *url) 
{ 
	// nb. vc7 has crummy warnings about 32/64bit. My code's perfect! That's why I hide the warnings.
#pragma warning( push )
#pragma warning( disable: 4312 4244 )
	// First we'll subclass the edit control
	HWND hctl = GetDlgItem(hdlg,id);
	SetWindowText(hctl,url);
	HGLOBAL hold = (HGLOBAL)GetProp(hctl,_T("href_dat"));
	if (hold!=NULL) // if it had been subclassed before, we merely need to tell it the new url
	{ 
		TUrlData *ud = (TUrlData*)GlobalLock(hold);
		delete[] ud->url;
		ud->url=new TCHAR[_tcslen(url)+1]; _tcscpy(ud->url,url);
	}
	else
	{ 
		HGLOBAL hglob = GlobalAlloc(GMEM_MOVEABLE,sizeof(TUrlData));
		TUrlData *ud = (TUrlData*)GlobalLock(hglob);
		ud->oldproc = (WNDPROC)GetWindowLong(hctl,GWL_WNDPROC);
		ud->url=new TCHAR[_tcslen(url)+1]; _tcscpy(ud->url,url);
		ud->hf=0; ud->hb=0;
		GlobalUnlock(hglob);
		SetProp(hctl,_T("href_dat"),hglob);
		SetWindowLong(hctl,GWL_WNDPROC,(LONG)UrlCtlProc);
	}
	//
	// Second we subclass the dialog
	hold = (HGLOBAL)GetProp(hdlg,_T("href_dlg"));
	if (hold==NULL)
	{ 
		HGLOBAL hglob = GlobalAlloc(GMEM_MOVEABLE,sizeof(TUrlData));
		TUrlData *ud = (TUrlData*)GlobalLock(hglob);
		ud->url=0;
		ud->oldproc = (WNDPROC)GetWindowLong(hdlg,GWL_WNDPROC);
		ud->hb=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		ud->hf=0; // the font will be created lazilly, the first time WM_CTLCOLORSTATIC gets called
		GlobalUnlock(hglob);
		SetProp(hdlg,_T("href_dlg"),hglob);
		SetWindowLong(hdlg,GWL_WNDPROC,(LONG)UrlDlgProc);
	}
#pragma warning( pop )
}

// UrlCtlProc: this is the subclass procedure for the static control
LRESULT CALLBACK UrlCtlProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ 
	HGLOBAL hglob = (HGLOBAL)GetProp(hwnd,_T("href_dat"));
	if (hglob==NULL) return DefWindowProc(hwnd,msg,wParam,lParam);
	TUrlData *oud=(TUrlData*)GlobalLock(hglob); TUrlData ud=*oud;
	GlobalUnlock(hglob); // I made a copy of the structure just so I could GlobalUnlock it now, to be more local in my code
	switch (msg)
	{ 
	case WM_DESTROY:
		{ 
			RemoveProp(hwnd,_T("href_dat")); GlobalFree(hglob);
			if (ud.url!=0) delete[] ud.url;
			// nb. remember that ud.url is just a pointer to a memory block. It might look weird
			// for us to delete ud.url instead of oud->url, but they're both equivalent.
		} break;
	case WM_LBUTTONDOWN:
		{ 
			HWND hdlg=GetParent(hwnd); if (hdlg==0) hdlg=hwnd;
			ShellExecute(hdlg,_T("open"),ud.url,NULL,NULL,SW_SHOWNORMAL);
		} break;
	case WM_SETCURSOR:
		{ 
			HCURSOR hc=LoadCursor(NULL,MAKEINTRESOURCE(32649)); // =IDC_HAND
			if (hc==0) hc=LoadCursor(NULL,IDC_ARROW); // systems before Win2k didn't have the hand
			SetCursor(hc);
			return TRUE;
		} 
	case WM_NCHITTEST:
		{ 
			return HTCLIENT; // because normally a static returns HTTRANSPARENT, so disabling WM_SETCURSOR
		} 
	}
	return CallWindowProc(ud.oldproc,hwnd,msg,wParam,lParam);
}

// UrlDlgProc: this is the subclass procedure for the dialog
LRESULT CALLBACK UrlDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ 
	HGLOBAL hglob = (HGLOBAL)GetProp(hwnd,_T("href_dlg"));
	if (hglob==NULL) return DefWindowProc(hwnd,msg,wParam,lParam);
	TUrlData *oud=(TUrlData*)GlobalLock(hglob); TUrlData ud=*oud;
	GlobalUnlock(hglob);
	switch (msg)
	{ 
	case WM_DESTROY:
		{ 
			RemoveProp(hwnd,_T("href_dlg")); GlobalFree(hglob);
			if (ud.hb!=0) DeleteObject(ud.hb);
			if (ud.hf!=0) DeleteObject(ud.hf);
		} break;
	case WM_CTLCOLORSTATIC:
		{ 
			HDC hdc=(HDC)wParam; HWND hctl=(HWND)lParam;
			// To check whether to handle this control, we look for its subclassed property!
			HANDLE hprop=GetProp(hctl,_T("href_dat")); if (hprop==NULL) return CallWindowProc(ud.oldproc,hwnd,msg,wParam,lParam);
			// There has been a lot of faulty discussion in the newsgroups about how to change
			// the text colour of a static control. Lots of people mess around with the
			// TRANSPARENT text mode. That is incorrect. The correct solution is here:
			// (1) Leave the text opaque. This will allow us to re-SetDlgItemText without it looking wrong.
			// (2) SetBkColor. This background colour will be used underneath each character cell.
			// (3) return HBRUSH. This background colour will be used where there's no text.
			SetTextColor(hdc,RGB(0,0,255));
			SetBkColor(hdc,GetSysColor(COLOR_BTNFACE));
			if (ud.hf==0)
			{ 
				// we use lazy creation of the font. That's so we can see font was currently being used.
				TEXTMETRIC tm; GetTextMetrics(hdc,&tm);
				LOGFONT lf;
				lf.lfHeight=tm.tmHeight;
				lf.lfWidth=0;
				lf.lfEscapement=0;
				lf.lfOrientation=0;
				lf.lfWeight=tm.tmWeight;
				lf.lfItalic=tm.tmItalic;
				lf.lfUnderline=TRUE;
				lf.lfStrikeOut=tm.tmStruckOut;
				lf.lfCharSet=tm.tmCharSet;
				lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
				lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
				lf.lfQuality=DEFAULT_QUALITY;
				lf.lfPitchAndFamily=tm.tmPitchAndFamily;
				GetTextFace(hdc,LF_FACESIZE,lf.lfFaceName);
				ud.hf=CreateFontIndirect(&lf);
				TUrlData *oud = (TUrlData*)GlobalLock(hglob); oud->hf=ud.hf; GlobalUnlock(hglob);
			}
			SelectObject(hdc,ud.hf);
			// Note: the win32 docs say to return an HBRUSH, typecast as a BOOL. But they
			// fail to explain how this will work in 64bit windows where an HBRUSH is 64bit.
			// I have supressed the warnings for now, because I hate them...
#pragma warning( push )
#pragma warning( disable: 4311 )
			return (BOOL)ud.hb;
#pragma warning( pop )
		}  
	}
	return CallWindowProc(ud.oldproc,hwnd,msg,wParam,lParam);
}



inline void Debug(const tstring s)
{ 
	if (DebugFile==_T("")) return;
	if (DebugFile==_T("OutputDebugString")) {tstring err=s+_T("\r\n"); OutputDebugString(err.c_str());}
	else {FILE *f = _tfopen(DebugFile.c_str(),_T("a+t")); _ftprintf(f,_T("%s\n"),s.c_str()); fclose(f);}
}

tstring GetLastErrorString()
{ 
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,
		GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
	tstring s((TCHAR*)lpMsgBuf);
	LocalFree( lpMsgBuf );
	return s;
}


void RegSave(const tstring name,DWORD type,void*buf,int size)
{ 
	tstring path = _T("Software\\WiredPlane\\")+SaverName;
	HKEY skey; LONG res=RegCreateKeyEx(HKEY_CURRENT_USER,path.c_str(),0,0,0,KEY_ALL_ACCESS,0,&skey,0);
	if (res!=ERROR_SUCCESS) return;
	RegSetValueEx(skey,name.c_str(),0,type,(const BYTE*)buf,size);
	RegCloseKey(skey);
}

bool RegLoadDword(const tstring name,DWORD *buf)
{ 
	tstring path = _T("Software\\WiredPlane\\")+SaverName;
	HKEY skey; LONG res=RegOpenKeyEx(HKEY_CURRENT_USER,path.c_str(),0,KEY_READ,&skey);
	if (res!=ERROR_SUCCESS) return false;
	DWORD size=sizeof(DWORD);
	res=RegQueryValueEx(skey,name.c_str(),0,0,(LPBYTE)buf,&size);
	RegCloseKey(skey);
	return (res==ERROR_SUCCESS);
}

void RegSave(const tstring name,int val)
{ 
	DWORD v=val; RegSave(name,REG_DWORD,&v,sizeof(v));
}
void RegSave(const tstring name,bool val)
{ 
	RegSave(name,val?1:0);
}
void RegSave(const tstring name,tstring val)
{ 
	RegSave(name,REG_SZ,(void*)val.c_str(),sizeof(TCHAR)*(val.length()+1));
}
int RegLoad(const tstring name,int def)
{
	DWORD val; bool res=RegLoadDword(name,&val);
	return res?val:def;
}
bool RegLoad(const tstring name,bool def)
{
	int b=RegLoad(name,def?1:0); return (b!=0);
}
tstring RegLoad(const tstring name,tstring def)
{
	tstring path = _T("Software\\WiredPlane\\")+SaverName;
	HKEY skey; LONG res=RegOpenKeyEx(HKEY_CURRENT_USER,path.c_str(),0,KEY_READ,&skey);
	if (res!=ERROR_SUCCESS) return def;
	DWORD size=0; res=RegQueryValueEx(skey,name.c_str(),0,0,0,&size);
	if (res!=ERROR_SUCCESS) {RegCloseKey(skey); return def;}
	TCHAR *buf = new TCHAR[size];
	RegQueryValueEx(skey,name.c_str(),0,0,(LPBYTE)buf,&size);
	tstring s(buf); delete[] buf;
	RegCloseKey(skey);
	return s;
}



int WINAPI WinMain(HINSTANCE h,HINSTANCE,LPSTR,int)
{
	srand(time(0));
	TCHAR *c=GetCommandLine();
	hInstance=h;
	TCHAR name[MAX_PATH]="Wired_Screensaver";
	SaverName=name;
	//
	TCHAR mod[MAX_PATH]; GetModuleFileName(hInstance,mod,MAX_PATH); tstring smod(mod);
	bool isexe = (smod.find(_T(".exe"))!=tstring::npos || smod.find(_T(".EXE"))!=tstring::npos);
	//
	BOOL bUI=1;
	if (*c=='\"') {c++; while (*c!=0 && *c!='\"') c++; if (*c=='\"') c++;} else {while (*c!=0 && *c!=' ') c++;}
	while (*c==' ') c++;
	HWND hwnd=NULL;
	if (*c==0) {if (isexe) ScrMode=smInstall; else ScrMode=smConfig; hwnd=NULL;}
	else
	{ 
		if (*c=='-' || *c=='/') c++;
		if (*c=='i' || *c=='I'){
			ScrMode=smInstall;
			while(*c){
				if (*c=='t' || *c=='T')
				{
					c++; while (*c==' ' || *c=='=') c++;
					g_dwTimeOut=_ttoi(c);
				}
				if (*c=='e' || *c=='E')
				{
					c++; while (*c==' ' || *c=='=') c++;
					g_dwEffects=_ttoi(c);
				}
				if (*c=='d' || *c=='D') bUI=0;
				c++;
			}
		}
		if (*c=='u' || *c=='U') ScrMode=smUninstall;
		if (*c=='p' || *c=='P' || *c=='l' || *c=='L')
		{ 
			c++; while (*c==' ' || *c==':') c++;
			if (_tcsicmp(c,_T("scrprev"))==0) hwnd=CheckForScrprev(); else hwnd=(HWND)_ttoi(c); 
			ScrMode=smPreview;
		}
		else if (*c=='s' || *c=='S') {ScrMode=smSaver; /*fakemulti=(c[1]=='m'||c[1]=='M');*/}
		else if (*c=='c' || *c=='C') {c++; while (*c==' ' || *c==':') c++; if (*c==0) hwnd=GetForegroundWindow(); else hwnd=(HWND)_ttoi(c); ScrMode=smConfig;}
		else if (*c=='a' || *c=='A') {c++; while (*c==' ' || *c==':') c++; hwnd=(HWND)_ttoi(c); ScrMode=smPassword;}
	}
#ifndef SHOW_DEBUG
	if (ScrMode==smInstall) {
		WriteGeneralRegistry();
		DoInstall(bUI);
		return 0;
	}
#else
	ScrMode=smSaver;
#endif
	if (ScrMode==smUninstall) {DoUninstall(); return 0;}
	if (ScrMode==smPassword) {ChangePassword(hwnd); return 0;}
	//
	ReadGeneralRegistry();
	INITCOMMONCONTROLSEX icx; ZeroMemory(&icx,sizeof(icx));
	icx.dwSize=sizeof(icx);
	icx.dwICC=ICC_UPDOWN_CLASS;
	InitCommonControlsEx(&icx);
	//
	WNDCLASS wc; ZeroMemory(&wc,sizeof(wc));
	wc.hInstance=hInstance;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.lpszClassName=_T("ScrMonitor");
	wc.lpfnWndProc=MonitorWndProc;
	RegisterClass(&wc);
	//
	wc.lpfnWndProc=SaverWndProc;
	wc.cbWndExtra=8;
	wc.lpszClassName=_T("ScrClass");
	RegisterClass(&wc);
	//
	if (ScrMode==smConfig) 
		DoConfig(hwnd);
	else 
		if (ScrMode==smSaver || ScrMode==smPreview) 
			DoSaver(hwnd,fakemulti);
	//
	return 0;
}
