// WKUtils.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "WKUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma data_seg("Shared")
HHOOK	g_hhook = NULL;
BYTE	g_GlobalData[500] = {0};
UINT	g_RestoreRegMessage = 0;
UINT	g_SaveRegMessage = 0;
HWND	g_TargetWnd = 0;
POINT	g_pt = {0,0};
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")


#include "..\..\SmartWires\SystemUtils\Lik\md5.h"

CString B2H2(const unsigned char* digest, int iLen)
{
	if(digest==0){
		return "";
	}
	unsigned char const* pc = digest;
	if(iLen<0){
		iLen=strlen((const char *)digest);
	}
	CString result;
	for(int i=0;i<iLen;i++) 
    { // convert md to hex-represented string (hex-letters in upper case!)
		CString sHex;
		sHex.Format("%02x",(*pc));
		result += sHex;
		pc++;
	}
	return result;
}


CString MD5_HexString2(const char* SourceString)
{
	unsigned char digest[16] = {0};    // message digest
	CString tmp = SourceString;
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, (unsigned char*)(const char*)tmp, tmp.GetLength());
	MD5Final(digest, &context);
	return B2H2((const unsigned char*)digest,16);
}

CString GetStringHash2(const char* sz)
{
	return MD5_HexString2(sz);
}

/////////////////////////////////////////////////////////////////////////////
// CWKUtilsApp

BEGIN_MESSAGE_MAP(CWKUtilsApp, CWinApp)
	//{{AFX_MSG_MAP(CWKUtilsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWKUtilsApp construction

CWKUtilsApp::CWKUtilsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWKUtilsApp object
CString g_sPWnds;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwCurWinProcID=0;
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(hwnd,&dwCurWinProcID);
	if(dwCurWinProcID==(DWORD)lParam){
		CString sRes;
		char szTitle[256]={0};
		char szClass[256]={0};
		::GetWindowText(hwnd,szTitle,sizeof(szTitle));
		::GetClassName(hwnd,szClass,sizeof(szClass));
		sRes.Format("Wnd 0x%08X '%s' [%s]\r\n", hwnd,szTitle,szClass);
		g_sPWnds+=sRes;
	}
	return TRUE;
}

CWKUtilsApp theApp;
#include "..\..\SmartWires\SystemUtils\NTSystemInfo.h"
char* WINAPI SystemGetOpenedHandles(DWORD dwProcID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//------------------------------------------
	CString sResult;
	{
		sResult="";
		CString name;
		CString processName;
		SystemHandleInformation hi;
		SystemProcessInformation pi;
		SystemProcessInformation::SYSTEM_PROCESS_INFORMATION* pPi;
		//Query every file handle (system wide)
		if ( !hi.SetFilter( _T("File"), TRUE ) )
		{
			sResult=_T("SystemHandleInformation::SetFilter() failed.\n");
			return NULL;
		}
		if ( !pi.Refresh() )
		{
			sResult=_T("SystemProcessInformation::Refresh() failed.\n");
			return NULL;
		}
		//Iterate through the found file handles
		for ( POSITION pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
		{
			SystemHandleInformation::SYSTEM_HANDLE& h = hi.m_HandleInfos.GetNext(pos);
			
			if(dwProcID!=0 && h.ProcessID != dwProcID)
				continue;
			
			if ( !pi.m_ProcessInfos.Lookup( h.ProcessID, pPi ) )
				continue;
			
			if ( pPi == NULL )
				continue;
			
			//Get the process name
			SystemInfoUtils::Unicode2CString( &pPi->usName, processName );
			
			//NT4 Stupid thing if I query the name of a file in services.exe
			//Messengr service brings up a message dialog ??? :(
			if ( INtDll::dwNTMajorVersion == 4 && _tcsicmp( processName, _T("services.exe") ) == 0 )
				continue;
			
			//what's the file name for this given handle?
			hi.GetName( (HANDLE)h.HandleNumber, name, h.ProcessID );
			if(name!=""){
				sResult+=name;
				sResult+="\r\n";
			}
		}
	}
	{// Открытые окна
		g_sPWnds="";
		EnumWindows(EnumWindowsProc,dwProcID);
		if(g_sPWnds!=""){
			sResult+="********************\r\n";
			sResult+=g_sPWnds;
			sResult+="\r\n";
		}
	}
	DWORD dwSize=strlen(sResult);
	char* hGlob=(char*)::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, dwSize+1);
	strcpy(hGlob,sResult);
	return hGlob;
}

BOOL IsWindows98()
{
	static BOOL bIsWin98=-1;
	if(bIsWin98==-1){
		bIsWin98=0;
		OSVERSIONINFOEX winfo; 
		ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
		winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
		if (GetVersionEx((OSVERSIONINFO *)&winfo)){ 
			bIsWin98=!(winfo.dwPlatformId==VER_PLATFORM_WIN32_NT);
		} 
	}
	return bIsWin98;
}

#include "..\..\SmartWires\BitmapUtils\ximage.h"
BOOL WINAPI SaveBitmapToJpg(const char* sPictureFile,HBITMAP bmp, BYTE bQuality)
{
	CxImage image;
	image.CreateFromHBITMAP(bmp);
	image.SetJpegQuality(bQuality);
	return image.Save(sPictureFile,CXIMAGE_FORMAT_JPG);
}

CString Format(const char* szFormat,...)
{
	va_list vaList;
	va_start(vaList,szFormat);
	CString sBuffer;
	sBuffer.FormatV(szFormat,vaList);
	va_end (vaList);
	return sBuffer;
}

BOOL CALLBACK FindSysListView32(HWND hwnd,LPARAM lParam)
{
	HWND* hOut=(HWND*)lParam;
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	strupr(szClass);
	if(strcmp(szClass,"SYSLISTVIEW32")==0){
		*hOut=hwnd;
		return FALSE;
	}else{
		HWND hMaybeChildShell=FindWindowEx(hwnd,0,"SHELLDLL_DefView",NULL);
		if(hMaybeChildShell){
			HWND hMaybeChild=FindWindowEx(hMaybeChildShell,0,"SysListView32",NULL);
			if(hMaybeChild!=0){
				*hOut=hMaybeChild;
				return FALSE;
			}
		}
	}
	return TRUE;
}

HWND GetSysListView()
{
	static HWND hTargetWindow=NULL;
	if(hTargetWindow!=NULL && !IsWindow(hTargetWindow)){
		hTargetWindow=NULL;
	}
	if(hTargetWindow==NULL){
		HWND hWnd1 = ::FindWindow("Progman","Program Manager");
		if(hWnd1 == 0){
			hWnd1 = ::FindWindow("Progman",NULL);
		}
		if(hWnd1 == 0){
			return 0;
		}
		HWND hWnd2 = FindWindowEx(hWnd1,0,"SHELLDLL_DefView",NULL); 
		if(hWnd2 == 0){
			DWORD dwProgmanID=0;
			DWORD dwProgmanThread=GetWindowThreadProcessId(hWnd1,&dwProgmanID);
			EnumThreadWindows(dwProgmanThread,FindSysListView32,(LPARAM)&hTargetWindow);
		}else{
			hTargetWindow = FindWindowEx(hWnd2,0,"SysListView32",NULL); 
		}
		if(hTargetWindow == 0){
#ifdef _DEBUG
			AfxMessageBox("No desktop found!!!");
#endif
		}
	}
	return hTargetWindow;
}

BOOL TransformPoint(POINT& pt, BOOL lRestorePreservRes, CRect& rDesktopRECT, long lResX, long lResY)
{
	if(pt.x!=0 && pt.y!=0){
		if(lRestorePreservRes>0){
			if(lResX!=rDesktopRECT.Width() || lResY!=rDesktopRECT.Height()){
				double dX=double(pt.x)/double(lResX);
				double dY=double(pt.y)/double(lResY);
				pt.x=(int)(double(rDesktopRECT.Width())*dX);
				pt.y=(int)(double(rDesktopRECT.Height())*dY);
			}
		}
		return TRUE;
	}
	return FALSE;
}

#include "../WKPlugin.h"
LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (WKUtils::isWKUpAndRunning() && g_hhook && g_TargetWnd && nCode>=0 && lParam!=0) {
		CWPRETSTRUCT* msg=(CWPRETSTRUCT*)(lParam);
		if(msg->message==g_RestoreRegMessage || msg->message==g_SaveRegMessage){
			LVITEMW lv;
			WCHAR pIconTitle[200];
			ZeroMemory(&lv,sizeof(lv));
			ZeroMemory(&g_pt,sizeof(g_pt));
			ZeroMemory(pIconTitle,sizeof(pIconTitle));
			ZeroMemory(g_GlobalData,sizeof(g_GlobalData));
			lv.mask = LVIF_TEXT;
			lv.iItem = msg->lParam;
			lv.pszText = (LPWSTR)pIconTitle;
			lv.cchTextMax = sizeof(pIconTitle);
			LRESULT lRes=SendMessage(g_TargetWnd,LVM_GETITEMW,0,(LPARAM)(&lv));
			memcpy(g_GlobalData,lv.pszText,sizeof(g_GlobalData));
			SendMessage(g_TargetWnd,LVM_GETITEMPOSITION,msg->lParam,(LPARAM)&g_pt);
		}
	}
	return(CallNextHookEx(g_hhook, nCode, wParam, lParam));
}


BOOL WINAPI RestoreDesktopIcons(const char* szFileName, BOOL lRestorePreservRes)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString sFrom;
	ReadFile(szFileName,sFrom);
	CString str=CDataXMLSaver::GetInstringPart("<ipos>","</ipos>",sFrom);
	if(str.GetLength()==0){
		return -1;
	}
	HWND hWnd = GetSysListView();
	if(hWnd == 0){
		return FALSE;
	}
	DWORD pID = 0;
	DWORD dwThreadId=GetWindowThreadProcessId(hWnd,&pID);
	if(pID == 0){
		return FALSE;
	}
	USES_CONVERSION;
	BOOL bRes=FALSE;
	{// Восстанавливаем
		CDataXMLSaver iconData(str);
		POINT pt;
		long lResX=0,lResY=0;
		iconData.getValue("res-x",lResX);
		iconData.getValue("res-y",lResY);
		CRect rDesktopRECT;
		g_TargetWnd=hWnd;
		::GetWindowRect(::GetDesktopWindow(),&rDesktopRECT);
		int iIconCount=SendMessage(hWnd,LVM_GETITEMCOUNT,0,0);
		if(!IsWindows98()){
			// По-старинке, через память
			HANDLE h = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,FALSE,pID);
			if(h != NULL){
				// Пошли восстанавливать!
				BOOL bRes=FALSE;
				LPVOID pBuf  = VirtualAllocEx(h,NULL,1024,MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
				LPVOID pText = VirtualAllocEx(h,NULL,1024,MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
				if(pBuf != NULL){
					for(int i=0;i<iIconCount;i++){
						// Первый этап - название иконки
						LVITEMW lv;
						ZeroMemory(&lv,sizeof(lv));
						lv.iItem = i;
						lv.mask = LVIF_TEXT;
						lv.pszText = (LPWSTR)pText;
						lv.cchTextMax = 1024;
						WriteProcessMemory(h,pBuf,&lv,sizeof(lv),NULL);
						SendMessage(hWnd,LVM_GETITEMW,0,(LPARAM)pBuf);
						ReadProcessMemory(h,pBuf,&lv,sizeof(lv),NULL);
						WCHAR szText[1024];
						ZeroMemory(&szText,sizeof(szText));
						ReadProcessMemory(h,pText,szText,1024,NULL);
						if(*szText!=0){
							CString sIconName=W2A(szText);
							CString sMiniHash=GetStringHash2(sIconName);
							ZeroMemory(&pt,sizeof(pt));
							iconData.getValue(Format("%s-x",sMiniHash),pt.x);
							iconData.getValue(Format("%s-y",sMiniHash),pt.y);
							if(TransformPoint(pt,lRestorePreservRes,rDesktopRECT,lResX,lResY)){
								SendMessage(hWnd,LVM_SETITEMPOSITION,i,(LPARAM) MAKELPARAM ((int) pt.x, (int) pt.y));
							}
						}
					}
					bRes=TRUE;
				}
				VirtualFreeEx(h,pBuf,0,MEM_RELEASE);
				VirtualFreeEx(h,pText,0,MEM_RELEASE);
				CloseHandle(h);
			}
		}
		if(!bRes){
			// Пробуем установить хук
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(AfxGetInstanceHandle(),szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook = SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndRetProc, hDLL, dwThreadId);
			if(g_hhook!=NULL){
				g_RestoreRegMessage=RegisterWindowMessage("WP_RESTORE_DICONS");
				// Через хук и GlobalMemory
				for(int i=0;i<iIconCount;i++){
					g_GlobalData[0]=g_GlobalData[1]=0;
					BOOL bRes=::SendMessage(hWnd,g_RestoreRegMessage,123,LPARAM(i));
					if(g_GlobalData[0]!=0 || g_GlobalData[1]!=0){
						CString sIconName=W2A((WCHAR*)g_GlobalData);
						CString sMiniHash=GetStringHash2(sIconName);
						iconData.getValue(Format("%s-x",sMiniHash),pt.x);
						iconData.getValue(Format("%s-y",sMiniHash),pt.y);
						if(TransformPoint(pt,lRestorePreservRes,rDesktopRECT,lResX,lResY)){
							SendMessage(hWnd,LVM_SETITEMPOSITION,i,(LPARAM) MAKELPARAM ((int) pt.x, (int) pt.y));
						}
					}
				}
				// Выключаемся...
				g_TargetWnd=NULL;
				SmartUnhookWindowsHookEx(g_hhook);
				g_hhook=NULL;
				bRes=TRUE;
			}
		}
	}
	return bRes;
}

BOOL WINAPI SaveDesktopIcons(const char* szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HWND hWnd = GetSysListView();
	if(hWnd == 0){
		return FALSE;
	}
	DWORD pID = 0;
	DWORD dwThreadId=GetWindowThreadProcessId(hWnd,&pID);
	if(pID == 0){
		return FALSE;
	}
	CString str;
	USES_CONVERSION;
	BOOL bRes=FALSE;
	{// Сохраняем иконки
		CString sSavedIcons;
		CDataXMLSaver iconData(sSavedIcons,"",FALSE);
		CRect rDesktopRECT;
		::GetWindowRect(GetDesktopWindow(),&rDesktopRECT);
		iconData.putValue("res-x",rDesktopRECT.Width());
		iconData.putValue("res-y",rDesktopRECT.Height());
		int iIconCount=SendMessage(hWnd,LVM_GETITEMCOUNT,0,0);
		if(!IsWindows98()){
			HANDLE h = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,FALSE,pID);
			if(h != NULL){
				// Пошли читать!
				str="";
				BOOL bRes=FALSE;
				LPVOID pBuf  = VirtualAllocEx(h,NULL,1024,MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
				LPVOID pText = VirtualAllocEx(h,NULL,1024,MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
				if(pBuf != NULL){
					for(int i=0;i<iIconCount;i++){
						// Первый этап - название иконки
						LVITEMW lv;
						ZeroMemory(&lv,sizeof(lv));
						lv.iItem = i;
						lv.mask = LVIF_TEXT;
						lv.pszText = (LPWSTR)pText;
						lv.cchTextMax = 1024;
						WriteProcessMemory(h,pBuf,&lv,sizeof(lv),NULL);
						SendMessage(hWnd,LVM_GETITEMW,0,(LPARAM)pBuf);
						ReadProcessMemory(h,pBuf,&lv,sizeof(lv),NULL);
						WCHAR szText[1024];
						ZeroMemory(&szText,sizeof(szText));
						ReadProcessMemory(h,pText,szText,1024,NULL);
						if(*szText!=0){
							CString sIconName=W2A(szText);
							CString sMiniHash=GetStringHash2(sIconName);
							// Второй этам - координаты
							POINT pt;
							ZeroMemory(&pt,sizeof(pt));
							WriteProcessMemory(h,pBuf,&pt,sizeof(pt),NULL);
							SendMessage(hWnd,LVM_GETITEMPOSITION,i,(LPARAM)pBuf);
							ReadProcessMemory(h,pBuf,&pt,sizeof(pt),NULL);
							//Записываем в строку
							iconData.putValue(Format("%s-x",sMiniHash),pt.x);
							iconData.putValue(Format("%s-y",sMiniHash),pt.y);
						}
					}
					str=iconData.GetResult();
					bRes=TRUE;
				}
				VirtualFreeEx(h,pBuf,0,MEM_RELEASE);
				VirtualFreeEx(h,pText,0,MEM_RELEASE);
				CloseHandle(h);
			}
		}
		if(!bRes){
			g_TargetWnd=hWnd;
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(AfxGetInstanceHandle(),szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook = SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndRetProc, hDLL, dwThreadId);
			if(g_hhook!=NULL){
				g_SaveRegMessage=RegisterWindowMessage("WP_SAVE_DICONS");
				// Через хук и GlobalMemory
				for(int i=0;i<iIconCount;i++){
					g_GlobalData[0]=g_GlobalData[1]=0;
					BOOL bRes=::SendMessage(hWnd,g_SaveRegMessage,123,LPARAM(i));
					if(g_GlobalData[0]!=0 || g_GlobalData[1]!=0){
						CString sIconName=W2A((WCHAR*)g_GlobalData);
						CString sMiniHash=GetStringHash2(sIconName);
						//Записываем в строку
						iconData.putValue(Format("%s-x",sMiniHash),g_pt.x);
						iconData.putValue(Format("%s-y",sMiniHash),g_pt.y);
					}
				}
				// Выключаемся...
				str=iconData.GetResult();
				g_TargetWnd=NULL;
				SmartUnhookWindowsHookEx(g_hhook);
				g_hhook=NULL;
				bRes=TRUE;
			}
		}
	}
	// Приписываем результат в нужный десктоп
	CString sResult="<ipos>\n";
	sResult+=str;
	sResult+="\n</ipos>";
	SaveFile(szFileName,sResult);
	return bRes;
};

//------------------------------------
#ifdef _HOGSUPPORT
#include "..\..\SmartWires\BitmapUtils\HogVideo.cpp"

#define HOGS_COUNT	5
CArray<CHogVideo*,CHogVideo*> aVHog;
BOOL WINAPI DXSwitchOverlaySupport(BOOL lState,const char* szDir)
{
	if(lState==0){
		CString sPath=szDir;
		for(int i=0;i<HOGS_COUNT;i++){
			CHogVideo* pVHog=new CHogVideo;
			pVHog->SetVideo(sPath+"logo.dat");
			pVHog->Hog();
			aVHog.Add(pVHog);
		}
	}else{
		for(int i=0;i<aVHog.GetSize();i++){
			if(aVHog[i]){
				delete aVHog[i];
			}
		}
		aVHog.RemoveAll();
	}
	/*
	CComPtr<IDirectDrawVideo> pDDV;
	HRESULT hr = ::CoCreateInstance(CLSID_VideoRenderer,NULL,CLSCTX_INPROC_SERVER,IID_IDirectDrawVideo,(void**)&pDDV);
	if(!SUCCEEDED(hr)){
		return FALSE;
	}
	DWORD Switches;
	hr = pDDV->GetSwitches(&Switches);
	if(lState==0){
		Switches &= ~(AMDDS_RGBOVR | AMDDS_YUVOVR);
	}else{
		Switches |= (AMDDS_RGBOVR | AMDDS_YUVOVR);
	}
	hr = pDDV->SetSwitches(Switches);
	*/
	return TRUE;
}
#else
BOOL WINAPI DXSwitchOverlaySupport(BOOL lState,const char* szDir)
{
	return 0;
}
#endif
/*
void ConvertComboDataToArray(const char* szData, CStringArray& aStrings, char c)
{
	CString sData=szData;
	CString sChar=c;
	sData.Replace(sChar,sChar+sChar);
	sData=sChar+sData+sChar;
	int iFrom=0;
	while(iFrom>=0){
		CString sLine=CDataXMLSaver::GetInstringPart(sChar,sChar,sData,iFrom);
		sLine.TrimLeft();
		sLine.TrimRight();
		if(sLine!=""){
			aStrings.Add(sLine);
		}
	}
}

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

bool PatternMatchList(const char* s, const char* mask,const char* szDelimiter)
{
	CString sList=mask;
	if(sList==""){
		return true;
	}
	int iC=sList.Replace(szDelimiter,"\t");
	if(iC==0 && sList.Find("*")==-1){
		sList=sList+"*";
	}
	CStringArray sAppList;
	ConvertComboDataToArray(sList,sAppList);
	for(int i=0;i<sAppList.GetSize();i++){
		if(PatternMatch(s,sAppList[i])){
			return true;
		}
	}
	return false;
}*/