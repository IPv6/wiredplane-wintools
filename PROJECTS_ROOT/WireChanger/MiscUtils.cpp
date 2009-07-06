#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SmartWarning(const char* szText, const char* szTitle, long* bAnswerHolder, DWORD dwTimer)
{
	if(bAnswerHolder && *bAnswerHolder!=0){
		return;
	}
	InfoWndStruct* info=new InfoWndStruct();
	info->iIcon=IDI_ICON_INFO_AST;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	info->dwStaticPars=DT_CENTER|DT_VCENTER;
	info->sOkButtonText=_l("Ok");
	info->dwDontAskHolder=bAnswerHolder;
	info->dwTimer=dwTimer;
	//info->pParent=CWnd::GetDesktopWindow();
	Alert(info);
}

int AskYesNo(const char* szText, const char* szTitle, long* bAnswerHolder, CWnd* pParent)
{
	if(bAnswerHolder){
		if(*bAnswerHolder==1){
			return IDYES;
		}
		if(*bAnswerHolder==2){
			return IDNO;
		}
	}
	AsyncPlaySoundSys("SystemQuestion");
	InfoWndStruct* info=new InfoWndStruct();
	info->bAsModal=TRUE;
	info->iIcon=IDI_ICON_ASK;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	info->dwStaticPars=DT_CENTER|DT_VCENTER;
	info->sOkButtonText=_l("Yes");
	info->sCancelButtonText=_l("No");
	info->dwDontAskHolder=bAnswerHolder;
	info->pParent=pParent;
	int iRes=(LRESULT)Alert(info);
	if(iRes==1){
		return IDYES;
	}
	return IDNO;
}

int AskYesNo(const char* szText, long* bAnswerHolder, CWnd* pParent)
{
	return AskYesNo(szText, AppName(), bAnswerHolder, pParent);
}

void AddError(const char* szError, BOOL bSystem)
{
	Alert(szError,3000);
}

InfoWnd* Alert(InfoWndStruct* pInfo)
{
	if(objSettings.bBlockAlerts){
		return 0;
	}
	if(pMainDialogWindow){
		return (InfoWnd*)(pMainDialogWindow->SendMessage(FIREALERT,NULL,LPARAM(pInfo)));
	}
	if(!IsAppStopping()){
		AfxMessageBox(pInfo->sText);
	}
	return NULL;
}

void AlertBox(const char* szTitle, const char* szText, DWORD dwTime)
{
	InfoWndStruct* info=new InfoWndStruct();
	info->pParent=0;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	info->dwStaticPars=DT_CENTER;
	info->dwTimer=dwTime;
	Alert(info);
}

InfoWnd* Alert(const char* szText, const char* szTitle, CWnd* pParent, BOOL bScrCentered)
{
	InfoWndStruct* info=new InfoWndStruct();
	info->pParent=pParent;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=bScrCentered?FALSE:TRUE;
	info->dwStaticPars=DT_CENTER|DT_VCENTER;
	return Alert(info);
}

InfoWnd* Alert(const char* szText, DWORD dwTime, BOOL bStartHided, BOOL bStaticPars)
{
	BOOL bScrCentered=0;
	InfoWndStruct* info=new InfoWndStruct();
	info->sText=szText;
	info->dwTimer=dwTime;
	info->bCenterUnderMouse=bScrCentered?FALSE:TRUE;
	info->bStartHided=bStartHided;
	if(bStaticPars==-1){
		info->dwStaticPars=DT_CENTER|DT_VCENTER|DT_SINGLELINE;
	}else{
		info->dwStaticPars=bStaticPars;
	}
	return Alert(info);
}

InfoWnd* Alert(const char* szText)
{
	return Alert(szText,DWORD(0));
}

void deleteFont(CFont* font)
{
	if(font==NULL){
		return;
	}
	if(font!=NULL){
		VERIFY(font->DeleteObject()!=FALSE);
		delete font;
	}
}

void deleteBrush(CBrush* br)
{
	if(br==NULL){
		return;
	}
	if(br!=NULL){
		VERIFY(br->DeleteObject()!=FALSE);
		delete br;
	}
}

void GetActiveWindowTitle(CString& sNewTitle, CRect& ActiveWndRect, HWND* pActivWnd)
{
	sNewTitle="";
	ActiveWndRect.SetRect(0,0,0,0);
	HWND pCurActivWnd=::GetForegroundWindow();
	if(!pCurActivWnd || !IsWindow(pCurActivWnd)){
		return;
	}
	char szTitle[1024]="";
	if(::GetWindowText(pCurActivWnd,szTitle,sizeof(szTitle))==0){
		return;
	};
	::GetWindowRect(pCurActivWnd,ActiveWndRect);
	sNewTitle=szTitle;
	if(pActivWnd){
		*pActivWnd=pCurActivWnd;
	}else{
		objSettings.hHotMenuWnd=pCurActivWnd;
	}
	objSettings.hFocusWnd=::GetFocus();
	return;
}

BOOL PtInWindowRect(POINT pt, CWnd* wnd)
{
	CRect rect;
	wnd->GetWindowRect(rect);
	return rect.PtInRect(pt);
}

BOOL MouseInWndRect(CWnd* wnd)
{
	POINT p;
	GetCursorPos(&p);
	return PtInWindowRect(p,wnd);
}

HRESULT FileToStream(LPCTSTR szFile, IStream* *ppStream)
{
	HRESULT hr;
	HGLOBAL hGlobal = NULL;

	// open file
	HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return HRESULT_FROM_WIN32(::GetLastError());

	// get file size
	DWORD dwBytesRead = 0, dwFileSize = ::GetFileSize(hFile, NULL);
	if (-1 != dwFileSize)
	{
		hGlobal = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, dwFileSize);
		if (NULL != hGlobal)
		{
			PVOID pvData = ::GlobalLock(hGlobal);
			ATLASSERT(NULL != pvData);

			// read file and store in global memory
			::ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
			::GlobalUnlock(hGlobal);

			if (dwFileSize != dwBytesRead)
			{
				VERIFY(::GlobalFree(hGlobal)==NULL);
				hr = E_FAIL;
			}
			else
				hr = S_OK;
		}
		else
			hr = E_OUTOFMEMORY;
	}
	else
		hr = HRESULT_FROM_WIN32(::GetLastError());

	::CloseHandle(hFile);

	// create IStream* from global memory
	if (SUCCEEDED(hr))
		hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, ppStream);
	return hr;
}


// Plays a specified waveform-audio file using MCI_OPEN and MCI_PLAY. 
// Returns when playback begins. Returns 0L on success, otherwise 
// returns an MCI error code.
DWORD playWAVEFile(const char* lpszWAVEFileName)
{
	UINT wDeviceID=0;
	DWORD dwReturn=0;
	MCI_OPEN_PARMS mciOpenParms;
	MCI_PLAY_PARMS mciPlayParms;

	// Open the device by specifying the device and filename.
	// MCI will choose a device capable of playing the specified file.
	mciOpenParms.lpstrAlias=NULL;
	mciOpenParms.dwCallback=0;
	mciOpenParms.lpstrDeviceType = "waveaudio";
	mciOpenParms.lpstrElementName = lpszWAVEFileName;
	if (dwReturn = mciSendCommand(0, MCI_OPEN,
		MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, 
		(DWORD)(LPVOID) &mciOpenParms))
	{
		// Failed to open device. Don't close it; just return error.
		return (dwReturn);
	}

	// The device opened successfully; get the device ID.
	wDeviceID = mciOpenParms.wDeviceID;

	// Begin playback. The window procedure function for the parent 
	// window will be notified with an MM_MCINOTIFY message when 
	// playback is complete. At this time, the window procedure closes 
	// the device.

	mciPlayParms.dwCallback = 0;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_WAIT, (DWORD)(LPVOID) &mciPlayParms);
	mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
	return dwReturn;
}

DWORD WINAPI PlaySoundInThread(LPVOID lpdwstatus)
{
	static long iSoundTracker=0;
	SimpleTracker Track(iSoundTracker);
	char* szSName=(char*)lpdwstatus;
	if(szSName==NULL){
		return 0;
	}
	CString sWav=szSName;
	delete szSName;
	if(sWav.GetLength()>0){
		int iCount=1;
		int iAlarmPos=sWav.Find("alarm=");
		if(iAlarmPos>-1){
			CString sAlarmCount=sWav.Mid(iAlarmPos+strlen("alarm="));
			if(sAlarmCount.SpanExcluding("?&")=="h12"){
				iCount=COleDateTime::GetCurrentTime().GetHour();
				iCount=iCount%12;
				if(iCount==0){
					iCount=12;
				}
			}else{
				iCount=atol(sAlarmCount);
			}
		}
		int iSleep=2000;
		int iSleepPos=sWav.Find("sleep=");
		if(iSleepPos>-1){
			iSleep=atol(sWav.Mid(iSleepPos+strlen("sleep=")));
		}
		sWav=sWav.SpanExcluding("?&");
		CString sWavFile;
		if(sWav.Find(SOUND_CATALOG)==0){
			sWavFile=CString(GetApplicationDir())+sWav;
		}else{
			sWavFile=sWav;
		}
		for(int k=0;k<iCount;k++){
			PlaySound(sWavFile,NULL,SND_FILENAME|SND_ASYNC);//SND_FILENAME|SND_NODEFAULT|SND_NOSTOP|SND_NOWAIT|SND_ASYNC - падает! :(
			if(k<iCount-1){
				Sleep(iSleep);
			}
		}
	}
	return 0;
}

void AsyncPlaySound(const char* szSoundName)
{
	if(!szSoundName){
		return;
	}
	char* szStr=new char[strlen(szSoundName)+1];
	strcpy(szStr,szSoundName);
	DWORD dwThreadID=0;
	HANDLE hThread = CreateThread( NULL, 0, PlaySoundInThread, (void*)szStr, 0, &dwThreadID);
	CloseHandle(hThread);
}

CString getLinkedID(const char* szLinkID)
{
	CString sOut=szLinkID;
	sOut+="L";
	return sOut;
}

void InitTRIconList(CComboBoxEx& list)
{
	int iCount=0,iCount2=TR_ICONS_OFFSET;
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
}

CString getActionHotKeyDsc(int iActionNum,const char* szBefore, const char* szAfter)
{
	CString sRes=GetIHotkeyName(objSettings.DefActionMapHK[iActionNum]);
	if(szBefore){
		if(sRes!=""){
			sRes=CString(szBefore)+sRes;
		};
	};
	if(szAfter){
		if(sRes!=""){
			sRes+=szAfter;
		};
	};
	return sRes;
};

/*
Поддержка компрессии в NT          Оценить        
От:  Lonely Dog   

- RtlDecompressBuffer
- RtlCompressBuffer

RtlCompressBuffer имеет следующий прототип: 
NTSYSAPI 
ULONG 
NTAPI 
RtlCompressBuffer ( 
IN USHORT CompressionFormatAndEngine, 
IN PUCHAR UncompressedBuffer, 
IN ULONG UncompressedBufferSize, 
OUT PUCHAR CompressedBuffer, 
IN ULONG CompressedBufferSize, 
IN ULONG UncompressedChunkSize, 
OUT PULONG FinalCompressedSize, 
IN PVOID WorkSpace 
); 

Если ты разобрался с этой функцией, 
то может подскажежешь, что такое WorkSpace? 
и каким должен быть его размер 

Кстати в ntifs.h определено 

#define COMPRESSION_FORMAT_LZNT1 (0x0002) 
#define COMPRESSION_ENGINE_MAXIMUM (0x0100) 
#define COMPRESSION_ENGINE_HIBER (0x0200) 

но у меня работает только COMPRESSION_FORMAT_LZNT1 (WinXP) 

Вот пример работы с RtlCompressBuffer. (Из этого можно сделать простейший архиватор, 
если усекать файл до сжатого размера) 

#include <windows.h> 
#include <stdio.h> 

#define NTSYSAPI extern "C" 

#define COMPRESSION_FORMAT_NONE (0x0000) // winnt 
#define COMPRESSION_FORMAT_DEFAULT (0x0001) // winnt 
#define COMPRESSION_FORMAT_LZNT1 (0x0002) // winnt 

#define COMPRESSION_ENGINE_STANDART (0x0000) // winnt 
#define COMPRESSION_ENGINE_MAXIMUM (0x0100) // winnt 
#define COMPRESSION_ENGINE_HIBER (0x0200) // winnt 

NTSYSAPI 
ULONG 
NTAPI 
RtlGetCompressionWorkSpaceSize ( 
IN USHORT CompressionFormatAndEngine, 
OUT PULONG CompressBufferWorkSpaceSize, 
OUT PULONG CompressFragmentWorkSpaceSize 
); 

NTSYSAPI 
ULONG 
NTAPI 
RtlCompressBuffer ( 
IN USHORT CompressionFormatAndEngine, 

IN PUCHAR UncompressedBuffer, 
IN ULONG UncompressedBufferSize, 

OUT PUCHAR CompressedBuffer, 
IN ULONG CompressedBufferSize, 

IN ULONG UncompressedChunkSize, 

OUT PULONG FinalCompressedSize, 

IN PVOID WorkSpace 
); 

int main(int argc, char *argv[]) 
{ 
if (argc != 3) return 0; 

HANDLE hFile1 = CreateFile(argv[1], GENERIC_READ, 
FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0); 
HANDLE hFile2 = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, 
FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0); 

ULONG n = GetFileSize(hFile1, 0); 

HANDLE hMapping1 = CreateFileMapping(hFile1, 0, PAGE_READONLY, 0, 0, 0); 
HANDLE hMapping2 = CreateFileMapping(hFile2, 0, PAGE_READWRITE, 0, n, 0); 

PUCHAR p = PUCHAR(MapViewOfFileEx(hMapping1, FILE_MAP_READ, 0, 0, 0, 0)); 
PUCHAR q = PUCHAR(MapViewOfFileEx(hMapping2, FILE_MAP_WRITE, 0, 0, 0, 0)); 

ULONG ulBufferWsSize, ulFragmentWsSize; 
ULONG st; 
ULONG ulFormat = COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_STANDART; 
st = RtlGetCompressionWorkSpaceSize(ulFormat, &ulBufferWsSize, &ulFragmentWsSize); 

DWORD dwSize = 0; 

PVOID pv = GlobalAlloc(GPTR, ulBufferWsSize); 

DWORD dwSize1; 
st = RtlCompressBuffer(ulFormat, 
p, n, 
q, n, 
n, 
&dwSize, 
pv); 
printf("mem: %d\nSize = %d\n", ulBufferWsSize, dwSize); 
getchar(); 
return 0; 
} 

PS: Эту фигню надо линковать с ntdll.lib из DDK. 
PPS: COMPRESSION_FORMAT_LZNT1 - Алгоритм сжатия. (При проектировании NT, ребята из MS хотели добавить возможность расширения системы новыми 
алгоритмами сжатия, но сделали ли они это или нет, я не знаю.) 
COMPRESSION_ENGINE_XXX - Степень сжатия. 
Реально Hiber от standart отличается процентоа на 10 по степени сжатия, 
  но standart намного быстрее.
*/
DWORD GetDirectorySize(const char* szDirPath)
{
	HANDLE hFind;
	DWORD dwAllSize=0;
	WIN32_FIND_DATA FindData;
	CString sFileMask=szDirPath;
	sFileMask+="\\*.*";
	hFind=FindFirstFile(sFileMask,&FindData);
	int isFound=1;
	DWORD SavedRep=0;
	while(hFind!=INVALID_HANDLE_VALUE && isFound!=0){
		if (FindData.cFileName[0]!=TEXT('.') && FindData.cFileName[0] != 0)
			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0 )
				dwAllSize+=FindData.nFileSizeLow;
		isFound=FindNextFile(hFind,&FindData);
	};
	if (hFind!=INVALID_HANDLE_VALUE )
		FindClose(hFind);
	return dwAllSize;
}

//
int RemoveFirstFile(const char* szDirPath)
{
	HANDLE hFind;
	DWORD dwAllSize=0;
	WIN32_FIND_DATA FindData;
	DWORD dwFileSize=0;
	char szFileName[1024]="";
	FILETIME ftFirst;
	CString sFileMask=szDirPath;
	sFileMask+="\\*.*";
	hFind=FindFirstFile(sFileMask,&FindData);
	int isFound=1;
	DWORD SavedRep=0;
	while(hFind!=INVALID_HANDLE_VALUE && isFound!=0){
		if (FindData.cFileName[0]!=TEXT('.') && FindData.cFileName[0] != 0)
			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0 ){
				if (!strcmp(szFileName,"") || CompareFileTime(&ftFirst,&FindData.ftCreationTime)>0){
					strcpy(szFileName,FindData.cFileName);
					ftFirst=FindData.ftCreationTime;
					dwFileSize=FindData.nFileSizeLow;
				}
			}
		isFound=FindNextFile(hFind,&FindData);
	};
	if (hFind!=INVALID_HANDLE_VALUE)
		FindClose(hFind);
	if (szFileName[0]!=0){
		CString sFileToDelete=(CString(szDirPath)+"\\")+szFileName;
		SetFileAttributes(sFileToDelete,FILE_ATTRIBUTE_NORMAL);
		if(DeleteFile(sFileToDelete)==0){
			Alert(Format("Can`t delete file '%s', %s",sFileToDelete,GetCOMError(GetLastError())));
		}
	}
	return dwFileSize;
}

/*void TimetToFileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}*/

BOOL GetFileDateTime(const char* szFile, FILETIME* tOut, BOOL bModTime, BOOL bFold)
{
	if(!tOut){
		return 0;
	}
	HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (bFold?FILE_FLAG_BACKUP_SEMANTICS:0), NULL);
	if (hFile==INVALID_HANDLE_VALUE ){
		return 0;
	}
	FILETIME CreationTime,LastAccessTime,ModTime;
	memset(&CreationTime,0,sizeof(FILETIME));
	memset(&LastAccessTime,0,sizeof(FILETIME));
	memset(&ModTime,0,sizeof(FILETIME));
	BOOL bRes=::GetFileTime(hFile,&CreationTime,&LastAccessTime,&ModTime);
	::CloseHandle(hFile);
  	if(!bRes){
		return 0;
	}
	memcpy(tOut,&CreationTime,sizeof(FILETIME));
	if(bModTime && (ModTime.dwHighDateTime!=0 || ModTime.dwLowDateTime!=0)){
		memcpy(tOut,&ModTime,sizeof(FILETIME));
	}
	return TRUE;
}

void SetWallpaperStyle(DWORD dwTile, DWORD dwStretch)
{
	const char* szZero="0";
	CString sTile=Format("%i",dwTile);
	CString sStretch=Format("%i",dwStretch);
	CRegKey key;
	key.Open(HKEY_CURRENT_USER, "Control Panel\\Desktop");
	if(key!=NULL){
		RegSetValueEx(key.m_hKey,"WallpaperStyle",0,REG_SZ,(BYTE*)(LPCSTR)sStretch,strlen(sStretch)+1);
		RegSetValueEx(key.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)(LPCSTR)sTile,strlen(sTile)+1);
		RegSetValueEx(key.m_hKey,"WallpaperOriginX",0,REG_SZ,(BYTE*)(LPCSTR)szZero,strlen(szZero)+1);
		RegSetValueEx(key.m_hKey,"WallpaperOriginY",0,REG_SZ,(BYTE*)(LPCSTR)szZero,strlen(szZero)+1);
	}
	//
	CRegKey key2;
	key2.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\Desktop\\General\\TileWallpaper");
	if(key2!=NULL){
		RegSetValueEx(key2.m_hKey,"WallpaperStyle",0,REG_SZ,(BYTE*)(LPCSTR)sStretch,strlen(sStretch)+1);
		RegSetValueEx(key2.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)(LPCSTR)sTile,strlen(sTile)+1);
	}
	if(objSettings.bSetLastWallpaperForDefUserToo){
		// Дефолтный юзер
		CRegKey key3;
		key3.Open(HKEY_USERS, ".DEFAULT\\Control Panel\\Desktop");
		if(key3!=NULL){
			RegSetValueEx(key3.m_hKey,"WallpaperStyle",0,REG_SZ,(BYTE*)(LPCSTR)sStretch,strlen(sStretch)+1);
			RegSetValueEx(key3.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)(LPCSTR)sTile,strlen(sTile)+1);
			RegSetValueEx(key3.m_hKey,"WallpaperOriginX",0,REG_SZ,(BYTE*)(LPCSTR)szZero,strlen(szZero)+1);
			RegSetValueEx(key3.m_hKey,"WallpaperOriginY",0,REG_SZ,(BYTE*)(LPCSTR)szZero,strlen(szZero)+1);
		}
	}
}

void SetWallpaperFileInReg(const char* szWallpaperPath, BOOL bForDefUserOnly)
{
	char* szEmptyWalue="";
	CString sZero="0";
	CString sTile=(objSettings.bTileOrCenter==1)?"1":"0";
	if(!bForDefUserOnly){
		CRegKey key;
		key.Open(HKEY_CURRENT_USER, "Control Panel\\Desktop");
		if(key!=NULL){
			RegSetValueEx(key.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)((LPCSTR)sTile),strlen(sTile)+1);
			RegSetValueEx(key.m_hKey,"Wallpaper",0,REG_SZ,(BYTE*)(LPCSTR)szWallpaperPath,strlen(szWallpaperPath)+1);
			RegSetValueEx(key.m_hKey,"WallpaperStyle",0,REG_SZ,(BYTE*)((LPCSTR)sZero),strlen(sZero)+1);
			RegSetValueEx(key.m_hKey,"WallpaperOriginX",0,REG_SZ,(BYTE*)((LPCSTR)sZero),strlen(sZero)+1);
			RegSetValueEx(key.m_hKey,"WallpaperOriginY",0,REG_SZ,(BYTE*)((LPCSTR)sZero),strlen(sZero)+1);
		}
		CRegKey key2;
		key2.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\Desktop\\General");
		if(key2!=NULL){
			RegSetValueEx(key2.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)((LPCSTR)sTile),strlen(sTile)+1);
			RegSetValueEx(key2.m_hKey,"Wallpaper",0,REG_SZ,(BYTE*)(LPCSTR)szWallpaperPath,strlen(szWallpaperPath)+1);
			RegSetValueEx(key2.m_hKey,"WallpaperStyle",0,REG_SZ,(BYTE*)((LPCSTR)sZero),strlen(sZero)+1);
			RegSetValueEx(key2.m_hKey,"WallpaperOriginX",0,REG_SZ,(BYTE*)((LPCSTR)sZero),strlen(sZero)+1);
			RegSetValueEx(key2.m_hKey,"WallpaperOriginY",0,REG_SZ,(BYTE*)((LPCSTR)sZero),strlen(sZero)+1);
		}
	}
	if(objSettings.bSetLastWallpaperForDefUserToo){
		// Дефолтный юзер
		CRegKey key2;
		key2.Open(HKEY_USERS, ".DEFAULT\\Control Panel\\Desktop");
		if(key2!=NULL){
			RegSetValueEx(key2.m_hKey,"TileWallpaper",0,REG_SZ,(BYTE*)((LPCSTR)sTile),strlen(sTile)+1);
			RegSetValueEx(key2.m_hKey,"Wallpaper",0,REG_SZ,(BYTE*)(LPCSTR)szWallpaperPath,strlen(szWallpaperPath)+1);
		}
	}
}

typedef int (WINAPI *_GetUserInfo)(const char* szU, const char* szN,int* iDays, int* iDaysFI, const char* szHash, int* , int*, COleDateTime* );

int UpdateTrialityStatus(CString sUser, CString sKeys)
{
	objSettings.isLight=0;
#ifdef USE_LITE
	objSettings.sLikUser="Light";
	objSettings.sLikKey="<none>";
	objSettings.iLikStatus=2;
	return objSettings.iLikStatus;
#endif
#ifdef GPL
	objSettings.iLikStatus=2;
	return objSettings.iLikStatus;
#endif
	objSettings.iLikStatus--;
	objSettings.iLikStatus--;
	objSettings.iLikStatus--;
	objSettings.iLikStatus--;
	CRYPT_START
	if(objSettings.iLikStatus>0){
		objSettings.iLikStatus=0;
		return objSettings.iLikStatus;
	}
	sKeys.Replace("-","");
	sKeys.Replace(" ","");
	{
		long lPrevRegStatus=objSettings.iLikStatus;
		CString sStuFile=GetAppFolder()+"Stuff.cml";
		HINSTANCE hUtils=LoadLibrary(sStuFile);
		DWORD dwErr=GetLastError();
		BOOL bExecuted=0;
		if(hUtils){_XLOG_
			_GetUserInfo fp=(_GetUserInfo)DBG_GETPROC(hUtils,"GetUserInfo");
			if(fp){_XLOG_
				bExecuted=1;
				objSettings.iLikStatus=(*fp)(sUser,sKeys,&GetWindowDays(),&GetWindowDaysFromInstall(),"Note2crackers: you can freely patch this app, we don`t care anymore :)",&isTempKeyInvalid(),&isTempKey(),&isTempKeyDate());
			}
		}
		if(bExecuted==0){_XLOG_
			AfxMessageBox(Format("%s, error=%i\n %s=%i",_l("ERROR: Important files are missing. PLease reinstall application"),dwErr,sStuFile,hUtils));
			isTempKey()=1;
			objSettings.iLikStatus=0;
		}
		FreeLibrary(hUtils);
#ifndef _DEBUG
		if(long(GetWindowDays()-WC_TRIAL)>=0 || !isFileExist(objSettings.sIniFile)){
			if(objSettings.iLikStatus<2){
				//isKeyStat()=
				objSettings.iLikStatus=2;
				isTempKey()=1;
				isTempKeyInvalid()=0;
				isTempKeyDate().SetStatus(COleDateTime::invalid);
			}
		}
#endif
		if(lPrevRegStatus!=objSettings.iLikStatus && pMainDialogWindow){
			pMainDialogWindowCrypt->SetRegInfo();
		}
	}
	// Проверяем не лайтли версия МЫ
	CString isLig;
	ReadFile(CString(GetApplicationDir())+"install2.ini",isLig);
	if(isLig.Find("1")!=-1){
		objSettings.isLight=1;
		objSettings.iLikStatus=2;
		isTempKey()=0;
		isTempKeyInvalid()=0;
	}
	CRYPT_END
	return objSettings.iLikStatus;
}

void CheckFlashPlayer()
{
	// Слишком много народа жалуется что мешает сообщение... плеер фактически у всех есть
	return;
	//Flash check!
	//if(!IsClsidInstalled("D27CDB6E-AE6D-11cf-96B8-444553540000"))
	if(!IsProgidInstalled("ShockwaveFlash.ShockwaveFlash"))
	{
		Alert(_l("To use some objects you must have Flash Player","To use some objects you must have Flash Player.\nYou can download it from 'http://www.macromedia.com/go/getflashplayer'"),_l(AppName()+": Flash player not found"));
	}
}


BOOL GetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut, size_t dwOutSize)
{
	CRegKey key;
	CString sKeyPath=CString(PROG_REGKEY);
	if(szFName && szFName[0]){
		sKeyPath+="\\";
		sKeyPath+=szFName;
	}
	if(key.Open(HKEY_CURRENT_USER, sKeyPath)!=ERROR_SUCCESS){
		key.Create(HKEY_CURRENT_USER, sKeyPath);
	}
	if(key.m_hKey!=NULL){
		DWORD dwType=REG_SZ;
		DWORD dwSize=dwOutSize;
		RegQueryValueEx(key.m_hKey, szRegSettingsName, NULL, &dwType, (LPBYTE)szOut, &dwSize);
	}
	return TRUE;
}

BOOL SetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut)
{
	CRegKey key;
	CString sKeyPath=CString(PROG_REGKEY);
	if(szFName && szFName[0]){
		sKeyPath+="\\";
		sKeyPath+=szFName;
	}
	if(key.Open(HKEY_CURRENT_USER, sKeyPath)!=ERROR_SUCCESS){
		key.Create(HKEY_CURRENT_USER, sKeyPath);
	}
	if(key.m_hKey!=NULL){
		DWORD dwType=REG_SZ,dwSize=0;
		RegSetValueEx(key.m_hKey,szRegSettingsName,0,REG_SZ,(BYTE*)(LPCSTR)szOut,lstrlen(szOut)+1);
	}
	return TRUE;
}

#pragma comment(lib,"../SmartWires/TI/tiscript.lib")
static VOID CALLBACK WC_getWPTVar( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{
	if( argc == 2 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		json::string strf = argv[1].get(L"?");
		//json::string strt = argv[2].get(L"?");
		USES_CONVERSION;
		CString sWPT=W2A(str.chars());
		CString sVar=W2A(strf.chars());
		CString sVal;
		ReadSingleTemplateVar(sWPT,sVar,sVal);
		if(retval){
			(*retval)=json::string(A2W(sVal));
		}
	}
}

static VOID CALLBACK WC_setWPTVar( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{
	if( argc == 3 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		json::string strf = argv[1].get(L"?");
		json::string strt = argv[2].get(L"?");
		USES_CONVERSION;
		CString sWPT=W2A(str.chars());
		CString sVar=W2A(strf.chars());
		CString sVal=W2A(strt.chars());
		WriteSingleTemplateVar(sWPT,sVar,sVal);
	}
}

static VOID CALLBACK WC_lowerString( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		USES_CONVERSION;
		CString sPageContent=W2A(str.chars());
		sPageContent.MakeLower();
		if(retval){
			(*retval)=json::string(A2W(sPageContent));
		}
	}
}

static VOID CALLBACK WC_cutString( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{
	if( argc == 3 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		json::string strf = argv[1].get(L"?");
		json::string strt = argv[2].get(L"?");
		USES_CONVERSION;
		CString sPageContent=W2A(str.chars());
		CString sFrom=W2A(strf.chars());
		CString sTo=W2A(strt.chars());
		sPageContent=CDataXMLSaver::GetInstringPart(sFrom,sTo,sPageContent);
		if(retval){
			(*retval)=json::string(A2W(sPageContent));
		}
	}
}

static VOID CALLBACK WC_isFileExist( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		USES_CONVERSION;
		CString sPageContent=W2A(str.chars());
		BOOL bIs=isFileExist(sPageContent);
		if(retval){
			(*retval)=bIs?true:false;
		}
	}
}

static VOID CALLBACK WC_GetWebPage( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		CWebWorld url;
		USES_CONVERSION;
		CString sPageContent=url.GetWebPage(W2A(str.chars()));
		if(retval){
			(*retval)=json::string(A2W(sPageContent));
		}
	}
}

static VOID CALLBACK WC_alert( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		::MessageBoxW(NULL,str,L"script alert", MB_OK | MB_ICONEXCLAMATION);
	}
	else
		tiscript::engine(hse).throw_error(L"bad parameters!");  
}

static VOID CALLBACK WC_getUserFolder( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	if(retval){
		USES_CONVERSION;
		(*retval)=json::string(A2W(GetUserFolder()));
	}
}

static VOID CALLBACK WC_getFilePath( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	CString sFiile;
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		USES_CONVERSION;
		sFiile=W2A(str.chars());
	}
	if(retval){
		USES_CONVERSION;
		(*retval)=json::string(A2W(GetPathPart(sFiile,1,1,0,0)));
	}
}

CString GetRemindersOnDay(COleDateTime dtCurrent)
{
	CString sTooltipPostfix;
	int iCount=0;
	char szDate[64]={0};
	strcpy(szDate,CDataXMLSaver::OleDate2Str(dtCurrent));
	HINSTANCE hRemin=GetModuleHandle("WCRemind.cml");
	if(hRemin){
		_GetCountOnDate fp2=(_GetCountOnDate)GetProcAddress(hRemin,"GetCountOnDate");
		if(fp2){
			iCount=(*fp2)(szDate);
		}
		if(iCount>0){
			_GetReminderOnDate fp3=(_GetReminderOnDate)GetProcAddress(hRemin,"GetReminderOnDate");
			if(fp3){
				sTooltipPostfix+="\r\n";
				/*if(iCount>1){
				sTooltipPostfix+=_l("Alerts");
				sTooltipPostfix+=":";
				}*/
				for(int i=0;i<iCount;i++){
					CWPReminder rem;
					if(!(*fp3)(szDate,i,rem)){
						break;
					}
					sTooltipPostfix+="\r\n";
					CString sDsc=TrimMessage(rem.szText,30,1);
					if(sDsc==""){
						sDsc=_l("No description");
					}
					sTooltipPostfix+=Format("%02i:%02i %s",rem.EventTime.wHour,rem.EventTime.wMinute,sDsc);
				}
			}
		}
	}
	//sOnDatePostfix=Format("ONDATE:(%i)",iCount);
	return sTooltipPostfix;
}

static VOID CALLBACK WC_getRemindersDsc( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	CString sRes;
	int day=argv[0].get(0);
	int mon=argv[1].get(0);
	int year=argv[2].get(0);
	sRes=GetRemindersOnDay(COleDateTime(year,mon,day,1,1,1));
	sRes.TrimLeft();
	sRes.TrimRight();
	USES_CONVERSION;
	(*retval)=json::string(A2W(sRes));
}

static VOID CALLBACK WC_readFile( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	CString sFiile;
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		USES_CONVERSION;
		sFiile=W2A(str.chars());
	}
	if(retval){
		USES_CONVERSION;
		CString sRes;
		ReadFile(sFiile,sRes);
		(*retval)=json::string(A2W(sRes));
	}
}

static VOID CALLBACK WC_Str2HTMLColor( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	CString sColor;
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		USES_CONVERSION;
		sColor=W2A(str.chars());
	}
	if(retval){
		DWORD dwBgCol=atol(sColor);
		sColor=Format("#%02X%02X%02X",GetRValue(dwBgCol),GetGValue(dwBgCol),GetBValue(dwBgCol));
		USES_CONVERSION;
		(*retval)=json::string(A2W(sColor));
	}
}

static VOID CALLBACK WC_escape( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	CString sFiile;
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		USES_CONVERSION;
		sFiile=W2A(str.chars());
	}
	if(retval){
		USES_CONVERSION;
		(*retval)=json::string(A2W(EscapeString(sFiile)));
	}
}

static VOID CALLBACK WC_unescape( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	CString sFiile;
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		USES_CONVERSION;
		sFiile=W2A(str.chars());
	}
	if(retval){
		USES_CONVERSION;
		(*retval)=json::string(A2W(UnescapeString(sFiile)));
	}
}

static VOID CALLBACK WC_saveImageToFile( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	if( argc == 2 && argv[0].is_string() && argv[1].is_string())
	{
		json::string str_from = argv[0].get(L"?");
		json::string str_to = argv[1].get(L"?");
		CWebWorld url;
		USES_CONVERSION;
		//const CString Url, CString sOutFileName, DWORD* dwStatusCode
		DWORD dwStatusCode=0;
		url.GetWebFile(W2A(str_from.chars()),W2A(str_to.chars()),&dwStatusCode);
		//::MessageBoxW(NULL,str,L"script alert", MB_OK | MB_ICONEXCLAMATION);
	}
}


CString sFont2HTMLConvert(CString& sMyFont)
{
	CDataXMLSaver::Xml2Str(sMyFont,XMLJScriptConv);
	
	CString sName=CDataXMLSaver::GetInstringPart("[fontname]","[/fontname]",sMyFont);
	if(sName.GetLength()==0){
		sName=CDataXMLSaver::GetInstringPart("<fontname>","</fontname>",sMyFont);
	}
	CString sEffects=CDataXMLSaver::GetInstringPart("[effects]","[/effects]",sMyFont);
	if(sEffects.GetLength()==0){
		sEffects=CDataXMLSaver::GetInstringPart("<effects>","</effects>",sMyFont);
	}
	CString sS=CDataXMLSaver::GetInstringPart("[size value=\"","\"/]",sMyFont);
	if(sS.GetLength()==0){
		sS=CDataXMLSaver::GetInstringPart("<size value=\"","\"/>",sMyFont);
	}
	CString sW=CDataXMLSaver::GetInstringPart("[weight value=\"","\"/]",sMyFont);
	if(sW.GetLength()==0){
		sW=CDataXMLSaver::GetInstringPart("<weight value=\"","\"/>",sMyFont);
	}
	
	int lfHeight=atol(sS);
	CDesktopDC dcScreen;
	if(lfHeight<0){
		//---------------------------
		lfHeight=-lfHeight;
		CDC dc;
		dc.CreateCompatibleDC(&dcScreen);
		dc.SetMapMode(MM_TEXT);
		lfHeight = MulDiv(lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	}
	
	CString sCssFont="";
	sCssFont+=Format(" %ipx ",lfHeight);
	if(atol(sW)>=800){
		sCssFont+=" bold ";
	}
	if(sEffects.Find("i")!=-1){_XLOG_
		sCssFont+=" italic ";
	}
	if(sEffects.Find("u")!=-1){_XLOG_
		sCssFont+=" oblique ";
	}
	if(sEffects.Find("s")!=-1){_XLOG_
		sCssFont+=" italic ";
	}
	sCssFont+=sName;
	return sCssFont;
}

static VOID CALLBACK WC_font2css( HSCRIPT hse, void**, tiscript::value* argv, int argc, tiscript::value* retval) 
{ 
	if( argc == 1 && argv[0].is_string())
	{
		json::string str = argv[0].get(L"?");
		CString sCssFont=str;
		sCssFont=sFont2HTMLConvert(sCssFont);
		USES_CONVERSION;
		(*retval)=json::string(A2W(sCssFont));
	}
	else
		tiscript::engine(hse).throw_error(L"bad parameters!");  
}

static TIS_method_def WCAutClass_methods[] =
{
	{ "getWebPage", &WC_GetWebPage },
	{ "alert", &WC_alert },
	{ "getUserFolder", &WC_getUserFolder },
	{ "saveImageToFile", &WC_saveImageToFile },
	{ "convertFont2css", &WC_font2css },
	{ "getFilePath", &WC_getFilePath },
	{ "readFile", &WC_readFile },
	{ "escape", &WC_escape },
	{ "unescape", &WC_unescape },
	{ "getRemindersDsc", &WC_getRemindersDsc },
	{ "str2HTMLColor", &WC_Str2HTMLColor },
	{ "lowerString", &WC_lowerString },
	{ "cutString", &WC_cutString },
	{ "isFileExist", &WC_isFileExist },
	{ "getWPTVar", &WC_getWPTVar },
	{ "setWPTVar", &WC_setWPTVar },
	{ 0, 0 } // zero terminated, sic!
};

// Utils package definition
TIS_class_def WCAutClass = 
{
  "WireChanger",
  WCAutClass_methods,
  0, // no props
  0 // no destructor
};


class std_stream2: public tiscript::stream
{
	BOOL bSkipAdvance;
	CString* in;
	const tiscript::wchar* str; 
public:
	std_stream2(const tiscript::wchar* _str,CString* _in)
	{
		str=_str;
		in=_in;
		bSkipAdvance=0;
	}
	virtual int get()
	{
		if(str && (*str)){
			int szw=str[0];
			/*if(str[0]==L'\\' && !bSkipAdvance){
				//szw=L'/';
				bSkipAdvance=1;
			}else{
				bSkipAdvance=0;
				str++;
			}
#ifdef _DEBUG
			USES_CONVERSION;
			CString debug=W2A(str);
#endif
			*/
			str++;
			return szw;
		}
		return -1;
	}
	virtual bool put(int v)
    {
		if(!in){
			return false;
		}
		USES_CONVERSION;
		WCHAR szw[10]={0};
		szw[0]=(WCHAR)v;
		(*in)+=W2A(szw);
		//return fputc( v, stdout ) != EOF; 
		return true;
	}
	virtual const wchar_t* stream_name()
	{
		return L"string_input";
	}
};

const char* ProcessWithTiScript_0(const char* sBodyText,BOOL asScript, CSetWallPaper* wpCommonData)
{
	if(!wpCommonData){
		return "err!";
	}
	USES_CONVERSION;
	const tiscript::wchar* str=A2W((const char*)sBodyText);//aux::a2w
	CString sRes,sResErr;

	std_stream2 myConsole(str,&sRes);
	//tiscript::engine tis(&myConsole);
	if(!wpCommonData->tiscript){
		wpCommonData->tiscript=new tiscript::engine();
	}
	::TIS_set_std_streams(wpCommonData->tiscript->_hengine, 
        &myConsole, &tiscript::stream::input, // stdin in script
        &myConsole, &tiscript::stream::output, // stdout in script
        &myConsole, &tiscript::stream::output); // stderr in script
	wpCommonData->tiscript->add(&WCAutClass);
	BOOL bRes=wpCommonData->tiscript->load(&myConsole, asScript?false:true);
	if(!bRes){
		DUMPFILE("tiscript_error",Format("Error in TISCRIPT:\n%s\n\n==================\nReturned:\n==================\n\n%s\n\n\n\n\n==================\nOriginal:\n==================\n\n%s",sResErr,sRes,sBodyText));
	}
	return strdup(sRes);
}

const char* ProcessWithTiScript_1(const char* sBodyText,BOOL asScript, CSetWallPaper* wpCommonData)
{
  const char* sz=0;
  __try{
    sz=ProcessWithTiScript_0( sBodyText, asScript, wpCommonData);
  }
	__except(EXCEPTION_EXECUTE_HANDLER){
	}
  return sz;
}

CString ProcessWithTiScript(const char* sBodyText,BOOL asScript, CSetWallPaper* wpCommonData)
{
  const char* sz=ProcessWithTiScript_1( sBodyText, asScript, wpCommonData);
  return sz;
}

void ClanchimDengi()
{
//Отправьте SMS "+wpc" (без кавычек) на номер 1151 (Россия) или 5012 (Украина) (Стоимость - $0.99)
}