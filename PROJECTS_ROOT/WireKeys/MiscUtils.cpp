/*#pragma comment(lib, "Delayimp.lib")
#pragma comment(linker, "/DELAYLOAD:Winmm.dll")
#pragma comment(linker, "/DELAYLOAD:Wsock32.dll")
#pragma comment(linker, "/DELAYLOAD:Shell32.dll")
#pragma comment(linker, "/DELAYLOAD:Ole32.dll")
#pragma comment(linker, "/DELAYLOAD:Oleaut32.dll")
#pragma comment(linker, "/DELAYLOAD:Setupapi.dll")
#pragma comment(linker, "/DELAYLOAD:Rpcrt4.dll")
#pragma comment(linker, "/DELAYLOAD:Riched20.dll")
#pragma comment(linker, "/DELAYLOAD:Shlwapi.dll")
#pragma comment(linker, "/DELAYLOAD:Userenv.dll")*/

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SmartWarning(const char* szText, const char* szTitle, long* bAnswerHolder, DWORD dwTimer)
{_XLOG_
	if(bAnswerHolder && *bAnswerHolder!=0){_XLOG_
		return;
	}
	InfoWndStruct* info=new InfoWndStruct();
	info->iIcon=IDI_ICON_INFO_AST;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	info->dwStaticPars=DT_CENTER|DT_VCENTER;
	info->sOkButtonText=_l("OK");
	info->dwDontAskHolder=bAnswerHolder;
	info->dwTimer=dwTimer;
	Alert(info);
}

int AskYesNo(const char* szText, const char* szTitle, long* bAnswerHolder, CWnd* pParent)
{_XLOG_
	if(bAnswerHolder){_XLOG_
		if(*bAnswerHolder==1){_XLOG_
			return IDYES;
		}
		if(*bAnswerHolder==2){_XLOG_
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
	if(iRes==1){_XLOG_
		return IDYES;
	}
	return IDNO;
}

void AddError(const char* szError, BOOL bSystem)
{_XLOG_
#ifndef _DEBUG
	if(bSystem)
#endif
	{_XLOG_
		ShowBaloon(szError,"Error",3000,1);
#ifndef _DEBUG
		LOGERROR1("Syserror:%s",szError);
#endif
	}
	//CString sError=_l("Error occured")+": "+szError;
	//AsyncPlaySound(SND_ERROR);
}

InfoWnd* Alert(const char* szText, const char* szTitle, DWORD dwTimeOut)
{_XLOG_
	TRACE2(">>>>>>>>> Alert:%s %s\n",CString(szText,128), szTitle);
	InfoWndStruct* info=new InfoWndStruct();
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	if(strstr(szText,"\\n")==0){_XLOG_
		info->dwStaticPars=DT_CENTER|DT_VCENTER;
	}else{_XLOG_
		info->dwStaticPars=DT_CENTER;
	}
	if(dwTimeOut!=0){_XLOG_
		info->dwTimer=dwTimeOut;
	}
	return Alert(info);
}

InfoWnd* Alert(const char* szText, DWORD dwTime, BOOL bStartHided, BOOL bStaticPars)
{_XLOG_
	InfoWndStruct* info=new InfoWndStruct();
	info->sText=szText;
	info->dwTimer=dwTime;
	info->bCenterUnderMouse=TRUE;
	info->iIcon=IDI_ICON_INFO_AST;
	info->bStartHided=bStartHided;

	if(bStaticPars==-1){_XLOG_
		info->dwStaticPars=DT_CENTER|DT_VCENTER|DT_SINGLELINE;
	}else{_XLOG_
		info->dwStaticPars=bStaticPars;
	}
	return Alert(info);
}

InfoWnd* Alert(const char* szText)
{_XLOG_
	return Alert(szText,DWORD(0));
}



InfoWnd* Alert(InfoWndStruct* pInfo)
{_XLOG_
	if(pMainDialogWindow && pMainDialogWindow->GetSafeHwnd()){_XLOG_
		return (InfoWnd*)(pMainDialogWindow->SendMessage(FIREALERT,NULL,LPARAM(pInfo)));
	}
	if(WKUtils::isWKUpAndRunning()){_XLOG_
		AfxMessageBox(pInfo->sText);
	}
	return NULL;
}

void deleteFont(CFont* font)
{_XLOG_
	if(font==NULL){_XLOG_
		return;
	}
	if(font!=NULL){_XLOG_
		VERIFY(font->DeleteObject()!=FALSE);
		delete font;
	}
}

void deleteBrush(CBrush* br)
{_XLOG_
	if(br==NULL){_XLOG_
		return;
	}
	if(br!=NULL){_XLOG_
		VERIFY(br->DeleteObject()!=FALSE);
		delete br;
	}
}

void GetWindowTitleAndData(HWND pCurActivWnd,CString& sNewTitle,CRect& ActiveWndRect)
{_XLOG_
	sNewTitle="";
	ActiveWndRect.bottom=ActiveWndRect.top=0;
	ActiveWndRect.left=ActiveWndRect.right=0;
	SafeGetWindowText(pCurActivWnd,sNewTitle);
	::GetWindowRect(pCurActivWnd,ActiveWndRect);
	objSettings.hFocusWnd=GetLastFocusWindowByHook(pCurActivWnd);
	/*
	// Зачем??????????????????????
	DWORD dwCurWinProcID=0;
	DWORD dwThisThread=GetCurrentThreadId();
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(pCurActivWnd,&dwCurWinProcID);
	DWORD dwThisProcess=GetCurrentProcessId();
	BOOL bRes=FALSE;
	if(dwThisProcess!=dwCurWinProcID){_XLOG_
		bRes=AttachThreadInput(dwThisThread,dwCurWinThreadID,TRUE);
	}
	objSettings.hFocusWnd=::GetFocus();
	if(bRes){_XLOG_
		bRes=AttachThreadInput(dwThisThread,dwCurWinThreadID,FALSE);
	}
	*/
}

BOOL GetActiveWindowTitle(CString& sNewTitle, CRect& ActiveWndRect, HWND* pActivWnd, BOOL bUnderCursor, BOOL bCheckInvis)
{_XLOG_
	sNewTitle="NoActiveWindowFound";
	ActiveWndRect.SetRect(0,0,0,0);
	HWND pCurActivWnd=NULL;
	if(bUnderCursor){_XLOG_
		POINT pt;
		::GetCursorPos(&pt);
		pCurActivWnd=::WindowFromPoint(pt);
	}else{_XLOG_
		pCurActivWnd=::GetForegroundWindow();
	}
	if(!pCurActivWnd || !IsWindow(pCurActivWnd)){_XLOG_
		return 1;
	}
	DWORD dwPropT=(DWORD)::GetProp(pCurActivWnd,"WKP_INVIS");
	if(dwPropT==1 && bCheckInvis){_XLOG_
		// Это окно невидимо для WK
		//PerformKBInit(NULL);
		pCurActivWnd=0;
	}
	if(pMainDialogWindow && pCurActivWnd == pMainDialogWindow->GetSafeHwnd()){_XLOG_
		return 2;
	}
	if(pActivWnd){_XLOG_
		*pActivWnd=pCurActivWnd;
	}else{_XLOG_
		objSettings.hHotMenuWnd=pCurActivWnd;
	}
	if(pCurActivWnd){_XLOG_
		GetWindowTitleAndData(pCurActivWnd,sNewTitle,ActiveWndRect);
	}else{_XLOG_
		sNewTitle="";
		ActiveWndRect=CRect(0,0,0,0);
	}
#ifdef _DEBUG
	//ShowBaloon(sNewTitle,"Active window",1000);
#endif
	return 0;
}

BOOL PtInWindowRect(POINT pt, CWnd* wnd)
{_XLOG_
	CRect rect;
	wnd->GetWindowRect(rect);
	return rect.PtInRect(pt);
}

BOOL MouseInWndRect(CWnd* wnd)
{_XLOG_
	POINT p;
	GetCursorPos(&p);
	return PtInWindowRect(p,wnd);
}

HRESULT FileToStream(LPCTSTR szFile, IStream* *ppStream)
{_XLOG_
	HRESULT hr;
	HGLOBAL hGlobal = NULL;

	// open file
	HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return HRESULT_FROM_WIN32(::GetLastError());

	// get file size
	DWORD dwBytesRead = 0, dwFileSize = ::GetFileSize(hFile, NULL);
	if (-1 != dwFileSize)
	{_XLOG_
		hGlobal = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, dwFileSize);
		if (NULL != hGlobal)
		{_XLOG_
			PVOID pvData = ::GlobalLock(hGlobal);
			ATLASSERT(NULL != pvData);

			// read file and store in global memory
			::ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
			::GlobalUnlock(hGlobal);

			if (dwFileSize != dwBytesRead)
			{_XLOG_
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

DWORD WINAPI PlaySoundInThread(LPVOID lpdwstatus)
{_XLOG_
	long static iSoundTracker=0;
	SimpleTracker Track(iSoundTracker);
	char* szSName=(char*)lpdwstatus;
	if(szSName==NULL){_XLOG_
		return 0;
	}
	CString sWav=szSName;
	delete szSName;
	if(objSettings.lDisableSounds){_XLOG_
		return 0;
	}
	if(sWav==SND_ERROR){_XLOG_
		if(objSettings.bErrorSound){_XLOG_
			sWav=objSettings.sErrorSound;
		}else{_XLOG_
			return 0;
		}
	}
	if(sWav==CLIP_COPY){_XLOG_
		if(objSettings.bClipCopySound){_XLOG_
			sWav=objSettings.sClipCopyWav;
		}else{_XLOG_
			return 0;
		}
	}
	if(sWav==CLIP_PASTE){_XLOG_
		if(objSettings.bClipPasteSound){_XLOG_
			sWav=objSettings.sClipPasteWav;
		}else{_XLOG_
			return 0;
		}
	}
	if(sWav==CONV_TEXT){_XLOG_
		if(objSettings.bOnLConv){_XLOG_
			sWav=objSettings.sOnLConvWav;
		}else{_XLOG_
			return 0;
		}
	}
	if(sWav==TAKE_SSHOT){_XLOG_
		if(objSettings.bSShot){_XLOG_
			sWav=objSettings.sSShotWav;
		}else{_XLOG_
			return 0;
		}
	}
	if(sWav==CALC_TEXT){_XLOG_
		if(objSettings.bSCalc){_XLOG_
			sWav=objSettings.sSCalcWav;
		}else{_XLOG_
			return 0;
		}
	}
	if(sWav==HK_TEXT){_XLOG_
		if(objSettings.bSHk){_XLOG_
			sWav=objSettings.sSHkWav;
		}else{_XLOG_
			return 0;
		}
	}
	if(sWav.GetLength()>0){_XLOG_
		int iCount=1;
		int iAlarmPos=sWav.Find("alarm=");
		if(iAlarmPos>-1){_XLOG_
			CString sAlarmCount=sWav.Mid(iAlarmPos+strlen("alarm="));
			if(sAlarmCount.SpanExcluding("?&")=="h12"){_XLOG_
				iCount=COleDateTime::GetCurrentTime().GetHour();
				iCount=iCount%12;
				if(iCount==0){_XLOG_
					iCount=12;
				}
			}else{_XLOG_
				iCount=atol(sAlarmCount);
			}
		}
		int iSleep=2000;
		int iSleepPos=sWav.Find("sleep=");
		if(iSleepPos>-1){_XLOG_
			iSleep=atol(sWav.Mid(iSleepPos+strlen("sleep=")));
		}
		sWav=sWav.SpanExcluding("?&");
		CString sWavFile;
		if(sWav.Find(SOUND_CATALOG"\\")==0 || sWav.Find(SOUND_CATALOG"/")==0){_XLOG_
			sWavFile=CString(GetApplicationDir())+sWav;
		}else{_XLOG_
			sWavFile=sWav;
		}
		if(isFileExist(sWavFile)){_XLOG_
			for(int k=0;k<iCount;k++){_XLOG_
				getWinmm().myPlaySound(sWavFile,NULL,SND_FILENAME|SND_ASYNC);//SND_FILENAME|SND_NODEFAULT|SND_NOSTOP|SND_NOWAIT|SND_ASYNC - падает! :(
				if(k<iCount-1){_XLOG_
					Sleep(iSleep);
				}
			}
		}
	}
	return 0;
}

void AsyncPlaySound(const char* szSoundName)
{_XLOG_
	if(objSettings.lDisableSounds){_XLOG_
		return;
	}
	if(!szSoundName || szSoundName[0]==0){_XLOG_
		return;
	}
	char* szStr=new char[strlen(szSoundName)+1];
	strcpy(szStr,szSoundName);
	DWORD dwThreadID=0;
	HANDLE hThread = CreateThread( NULL, 0, PlaySoundInThread, (void*)szStr, 0, &dwThreadID);
	CloseHandle(hThread);
}

CString getLinkedID(const char* szLinkID)
{_XLOG_
	CString sOut=szLinkID;
	sOut+="L";
	return sOut;
}

void InitTRIconList(CComboBoxEx& list)
{_XLOG_
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

CString getClipHotKeyDsc(int iClip,BOOL bCopy)
{_XLOG_
	int i=(iClip-1)*2+(bCopy?0:1);
	return GetIHotkeyName(objSettings.DefClipMapHK[i]);
}

CString getActionHotKeyDsc(CIHotkey& hk,const char* szBefore, const char* szAfter, const char* szIfNull)
{_XLOG_
	BOOL bShrink=0;
	CString sRes=GetIHotkeyName(hk,0,bShrink);
	sRes=TrimMessage(sRes,25,1);
	if(szBefore[0]=='\t'){_XLOG_
		/*if(sRes.GetLength()>10){_XLOG_
			sRes=sRes.Left(10)+"...";
		}*/

	}
	if(szIfNull && sRes==""){_XLOG_
		sRes=szIfNull;
	}
	if(szBefore){_XLOG_
		if(sRes!=""){_XLOG_
			sRes=CString(szBefore)+sRes;
		};
	};
	if(szAfter){_XLOG_
		if(sRes!=""){_XLOG_
			sRes+=szAfter;
		};
	};
	return sRes;
}

CString getActionHotKeyDsc(int iActionNum,const char* szBefore, const char* szAfter, const char* szIfNull)
{_XLOG_
	return getActionHotKeyDsc(objSettings.DefActionMapHK[iActionNum],szBefore, szAfter, szIfNull);
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
{_XLOG_
	HANDLE hFind;
	DWORD dwAllSize=0;
	WIN32_FIND_DATA FindData;
	CString sFileMask=szDirPath;
	sFileMask+="\\*.*";
	hFind=FindFirstFile(sFileMask,&FindData);
	int isFound=1;
	DWORD SavedRep=0;
	while(hFind!=INVALID_HANDLE_VALUE && isFound!=0){_XLOG_
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
{_XLOG_
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
	while(hFind!=INVALID_HANDLE_VALUE && isFound!=0){_XLOG_
		if (FindData.cFileName[0]!=TEXT('.') && FindData.cFileName[0] != 0)
			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0 ){_XLOG_
				if (!strcmp(szFileName,"") || CompareFileTime(&ftFirst,&FindData.ftCreationTime)>0){_XLOG_
					strcpy(szFileName,FindData.cFileName);
					ftFirst=FindData.ftCreationTime;
					dwFileSize=FindData.nFileSizeLow;
				}
			}
		isFound=FindNextFile(hFind,&FindData);
	};
	if (hFind!=INVALID_HANDLE_VALUE)
		FindClose(hFind);
	if (strcmp(szFileName,"")){_XLOG_
		CString sFileToDelete=(CString(szDirPath)+"\\")+szFileName;
		SetFileAttributes(sFileToDelete,FILE_ATTRIBUTE_NORMAL);
		if(DeleteFile(sFileToDelete)==0){_XLOG_
			Alert(Format("Can`t delete file '%s', %s",sFileToDelete,GetCOMError(GetLastError())));
		}
	}
	return dwFileSize;
}

BOOL GetFileDateTime(const char* szFile, FILETIME* tOut)
{_XLOG_
	if(!tOut){_XLOG_
		return 0;
	}
	memset(tOut,0,sizeof(FILETIME));
	HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile==INVALID_HANDLE_VALUE ){_XLOG_
		return 0;
	}
	FILETIME CreationTime,LastAccessTime;
	memset(&CreationTime,0,sizeof(FILETIME));
	memset(&LastAccessTime,0,sizeof(FILETIME));
	BOOL bRes=::GetFileTime(hFile,&CreationTime,&LastAccessTime,NULL);
	::CloseHandle(hFile);
  	if(!bRes){_XLOG_
		return 0;
	}
	memcpy(tOut,&CreationTime,sizeof(FILETIME));
	return TRUE;
}

DWORD dwLastSysRefreshTime=0;
void RefreshProcessListIfNeeded()
{_XLOG_
__FLOWLOG;
	if((GetTickCount()-dwLastSysRefreshTime)>5000){_XLOG_
		EnumProcesses();
		dwLastSysRefreshTime=GetTickCount();
	}
}

void GetWndIcon(HWND& hSlayerWnd,HICON& hIcon, HICON* hBigOut)
{_XLOG_
	ClearIcon(hIcon);
	HICON hTmpIcon=NULL;
	HICON hTmpIconBig=NULL;
	if(hTmpIcon==NULL){_XLOG_
		hTmpIcon=(HICON)::SendMessage(hSlayerWnd,WM_GETICON,ICON_SMALL,0);
	}
	if(hTmpIcon==NULL){_XLOG_
		hTmpIcon=(HICON)GetClassLong(hSlayerWnd, GCL_HICONSM);
	}
	if(hTmpIconBig==NULL){_XLOG_
		hTmpIconBig=(HICON)::SendMessage(hSlayerWnd,WM_GETICON,ICON_BIG,0);
	}
	if (hTmpIconBig==NULL){_XLOG_
		hTmpIconBig=(HICON)GetClassLong(hSlayerWnd, GCL_HICON);
	}
	if(hTmpIconBig==NULL){_XLOG_
		hTmpIconBig=hTmpIcon;
	}else if(hTmpIcon==NULL){_XLOG_
		hTmpIcon=hTmpIconBig;
	}
	if(hBigOut){_XLOG_
		*hBigOut=CopyIcon(hTmpIconBig);
	}
	if(hTmpIcon!=NULL){_XLOG_
		hIcon=CopyIcon(hTmpIcon);
	}
	if(hIcon==NULL){_XLOG_
		CString sPath="";
		DWORD dwProcID=0;
		DWORD dwThread=GetWindowThreadProcessId(hSlayerWnd,&dwProcID);
		if(dwProcID!=0){_XLOG_
			RefreshProcessListIfNeeded();
			CProcInfo* info=NULL;
			GetProcessInfo(dwProcID,info);
			if(info){_XLOG_
				GetExeFileIcon(info->sProcExePath,hIcon);
			}
		}
	}
	if(hIcon==NULL){_XLOG_
		hIcon=theApp.MainImageList.ExtractIcon(APP_ICONS_OFFSET);
	}
}

CString GetOrAddPathIconIndex(CString sKey, HICON& hIcon)
{_XLOG_
	BOOL bDoNotCache=FALSE;
	CString sBmpID=":(";
	if(sKey!=""){_XLOG_
		CString sVal;
		if(objSettings.aIconsMap.Lookup(sKey,sVal)){_XLOG_
			bDoNotCache=TRUE;
			sBmpID=sVal;
		}else{_XLOG_
			if(hIcon!=NULL){_XLOG_
				// Устанавливаем
				sBmpID=CString("PROCIC_")+sKey;
				_bmp().AddBmpRaw(sBmpID,hIcon);
			}else{_XLOG_
				sBmpID="";
			}
		}
	}
	if(!bDoNotCache && sBmpID!=""){_XLOG_
		objSettings.aIconsMap.SetAt(sKey,sBmpID);
	}
	return sBmpID;
}

CString GetExeIconIndex(const char* szExePath, BOOL bRawGet,BOOL bAllowRecheck, BOOL bUseSH)
{_XLOG_
	if(szExePath==0 || *szExePath==0){_XLOG_
		return "";
	}
	HICON hIcon=NULL;
	CString sRes=GetOrAddPathIconIndex(szExePath, hIcon);
	if(sRes==":("){_XLOG_
		if(!bRawGet){_XLOG_
			return "";
		}else{// Даем второй шанс
			objSettings.aIconsMap.RemoveKey(szExePath);
			sRes="";
		}
	}
	if(sRes==""){_XLOG_
		ClearIcon(hIcon);
		if(bUseSH){_XLOG_
			GetExeFileIcon(szExePath, hIcon, bRawGet, 1);
		}else if(isFolder(szExePath)){_XLOG_
			hIcon=_bmp().CloneAsIcon(_IL(5));
		}else{_XLOG_
			CString sFile=szExePath;
			sFile.TrimLeft(" \"");
			sFile.TrimRight(" \"");
			CString sExt=GetPathPart(sFile,0,0,0,1);
			sExt.MakeLower();
			if(sExt==".lnk"){_XLOG_
				hIcon=_bmp().CloneAsIcon(_IL(22));
			}else{_XLOG_
				if(bAllowRecheck){_XLOG_
					CString sEAssoc=GetExtensionAssociatedAction(sExt,"");
					if(sEAssoc!="" && sEAssoc!=sExt && isFileExist(sEAssoc)){_XLOG_
						// Тащим реальный Exe-file
						return GetExeIconIndex(sEAssoc,bRawGet,0);
					}
				}
				GetExeFileIcon(sFile, hIcon, bRawGet);
			}
		}
		if(hIcon!=NULL){_XLOG_
			sRes=GetOrAddPathIconIndex(szExePath, hIcon);
		}else{_XLOG_
			hIcon=::LoadIcon(0,IDI_APPLICATION);
			sRes=GetOrAddPathIconIndex(szExePath, hIcon);
			//objSettings.aIconsMap.SetAt(szExePath,":(");
			//return "";
		}
	}
	ClearIcon(hIcon);
	return sRes;
}

CString GetProcIconIndex(CProcInfo& info)
{_XLOG_
	return GetExeIconIndex(info.sProcExePath, TRUE);
}

extern CCriticalSection pCahLock;
void GetProcessesIcons()
{_XLOG_
	__FLOWLOG;
	CCSLock lpc(&pCahLock,1);
	for(int iPrCount=0;iPrCount<GetProccessCash().GetSize();iPrCount++){_XLOG_
		CProcInfo& pInfo=GetProccessCash()[iPrCount];
		pInfo.sIconIndex=GetProcIconIndex(pInfo);
	}
}

// fkill forces a kill -- it will attempt to enable SeDebugPrivilege
// before opening its process handles, allowing it to kill processes
// running under builtin\system (LocalSystem, to the users out there).
int KillProcess(DWORD pid, const char* szFileTitle)
{_XLOG_
	if(szFileTitle){_XLOG_
		if(GetAppWnd()){_XLOG_
			SetForegroundWindow(GetAppWnd()->GetSafeHwnd());
		}
		CString sTitle=szFileTitle;
		DWORD dwRes=AskYesNo(_l("Do you want to kill")+"\r\n"+Format("'%s'?",TrimMessage(sTitle,40)),_l("Confirmation"),&objSettings.bAskForKill,GetAppWnd());
		if(dwRes!=IDYES){_XLOG_
			objSettings.bAskForKill=0;// Если NO, то в следующий раз спросим снова
			return 0;
		}
	}
	DWORD dwRes=0;
	EnableDebugPriv();
	HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, pid);
	if (!( (hProcess) == NULL || (hProcess) == INVALID_HANDLE_VALUE )){_XLOG_
		if (TerminateProcess( hProcess, (DWORD) -1 )){_XLOG_
			dwRes=1;
		}
		CloseHandle( hProcess );
	}
	if(!dwRes){_XLOG_
		AsyncPlaySound(SND_ERROR);
	}
	return dwRes;
}

CString GetCurrentDesktop()
{_XLOG_
	HDESK hCurDesk=NULL;
	char szDesktopID[DEF_DESKNAMESIZE]="";
	if(objSettings.bUnderWindows98 ||!(hCurDesk=GetThreadDesktop(GetCurrentThreadId()))){_XLOG_
		GetDesktopInternalName(0,szDesktopID);
		return szDesktopID;
	}
	DWORD dwRealLen=0;
	GetUserObjectInformation(hCurDesk,UOI_NAME,szDesktopID,sizeof(szDesktopID),&dwRealLen);
	return szDesktopID;
};

int GetCurrentDesktopNumber()
{_XLOG_
	CString sDName=GetCurrentDesktop();
	char szDesktopID[DEF_DESKNAMESIZE]="";
	GetDesktopInternalName(0,szDesktopID);
	if(sDName==szDesktopID){_XLOG_
		return 0;
	}
	int iPos=strcspn(sDName,"01234567890");
	return atol(sDName.Mid(iPos));
};

BOOL SetCurrentDesktop()
{_XLOG_
	if(!objSettings.bUnderWindows98){_XLOG_
		// Чтобы пахало на нужно десктопе...
		char szDesktopID[DEF_DESKNAMESIZE]="";
		GetDesktopInternalName(objSettings.lLastActiveDesktop,szDesktopID);
		HDESK hDesktop=::OpenDesktop(szDesktopID,DF_ALLOWOTHERACCOUNTHOOK,FALSE,MAXIMUM_ALLOWED);
		if(hDesktop){_XLOG_
			BOOL bRes=SetThreadDesktop(hDesktop);
			CloseDesktop(hDesktop);
			return bRes;
		}
	}
	return FALSE;
}

typedef BOOL (WINAPI *_SaveDesktopIcons)(const char*);
typedef BOOL (WINAPI *_RestoreDesktopIcons)(const char*,BOOL);
BOOL RestoreDesktopIcons()
{_XLOG_
	__FLOWLOG;
	BOOL bRes=FALSE;
	CString sFrom;
	char szDesktopID[DEF_DESKNAMESIZE]="";
	GetDesktopInternalName(objSettings.lLastActiveDesktop,szDesktopID);
	CreateDirIfNotExist(getFileFullName("Desktop\\",TRUE));
	CString sFile=getFileFullName(Format("Desktop\\icons-%s.conf",szDesktopID));
	HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils.dll");
	if(hUtils){_XLOG_
		_RestoreDesktopIcons fp=(_RestoreDesktopIcons)DBG_GETPROC(hUtils,"RestoreDesktopIcons");
		if(fp){_XLOG_
			bRes=(*fp)(sFile,objSettings.lRestorePreservRes);
		}
	}
	FreeLibrary(hUtils);
	return bRes;
}

BOOL SaveDesktopIcons()
{_XLOG_
	__FLOWLOG;
	BOOL bRes=FALSE;
	// Приписываем результат в нужный десктоп
	char szDesktopID[DEF_DESKNAMESIZE]="";
	GetDesktopInternalName(objSettings.lLastActiveDesktop,szDesktopID);
	CreateDirIfNotExist(getFileFullName("Desktop\\",TRUE));
	CString sFile=getFileFullName(Format("Desktop\\icons-%s.conf",szDesktopID));
	HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils.dll");
	if(hUtils){_XLOG_
		_SaveDesktopIcons fp=(_SaveDesktopIcons)DBG_GETPROC(hUtils,"SaveDesktopIcons");
		if(fp){_XLOG_
			bRes=(*fp)(sFile);
		}
	}
	FreeLibrary(hUtils);
	return bRes;
}

HWND GetSysTrayNotifyWnd()
{_XLOG_
	HWND hWnd1 = ::FindWindow("Shell_TrayWnd",NULL);
	if(hWnd1 == 0){_XLOG_
		return 0;
	}
	HWND hWnd2 = FindWindowEx(hWnd1,0,"TrayNotifyWnd",NULL); 
	if(hWnd2 == 0){_XLOG_
		return 0;
	}
	return hWnd2;
}

HWND GetSysClockWnd()
{_XLOG_
	HWND hWnd2 = GetSysTrayNotifyWnd();
	if(hWnd2 == 0){_XLOG_
		return 0;
	}
	HWND hWnd3 = FindWindowEx(hWnd2,0,"TrayClockWClass",NULL); 
	if(hWnd3 == 0){_XLOG_
		return 0;
	}
	return hWnd3;
}

HWND GetSysTrayWnd()
{_XLOG_
	HWND hWnd2 = GetSysTrayNotifyWnd();
	if(hWnd2 == 0){_XLOG_
		return 0;
	}
	HWND hWnd3 = FindWindowEx(hWnd2,0,"ToolbarWindow32",NULL); 
	if(hWnd3 == 0){_XLOG_
		return 0;
	}
	return hWnd3;
}

BOOL GetGUIThreadInfoMy(DWORD dwCurWinThreadID, MYGUITHREADINFO* gui)
{_XLOG_
	BOOL bRes=FALSE;
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	if(hUser32){_XLOG_
		_GetGUIThreadInfo fp=(_GetGUIThreadInfo)DBG_GETPROC(hUser32,"GetGUIThreadInfo");
		if(fp){_XLOG_
			memset(gui,0,sizeof(MYGUITHREADINFO));
			gui->cbSize=sizeof(MYGUITHREADINFO);
			bRes=(*fp)(dwCurWinThreadID, gui);
		}
		FreeLibrary(hUser32);
	}
	return bRes;
}

SetWaitTrayIcon::SetWaitTrayIcon(CSystemTray* pTray)
{_XLOG_
	hPrev=NULL;
	m_pTray=NULL;
	hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
	if(pTray && !objSettings.bUnderWindows98){_XLOG_
		m_pTray=pTray;
		hPrev=m_pTray->GetIcon();
		if(hPrev){_XLOG_
			HICON hWaitIcon=CopyIcon(::AfxGetApp()->LoadIcon(IDR_MAINICOW));
			HICON hIcon=PutOverlayInIconSz(hPrev,hWaitIcon,_bmp().MenuIconSize.cx,_bmp().MenuIconSize.cy,0,0,0,TRUE);
			m_pTray->SetIcon(hIcon);
		}
	} 
};

SetWaitTrayIcon::~SetWaitTrayIcon()
{_XLOG_
	if(hPrev){_XLOG_
		HICON hTemp=m_pTray->GetIcon();
		m_pTray->SetIcon(hPrev);
		ClearIcon(hTemp);
	}
	if(hCur){_XLOG_
		SetCursor(hCur);
	}
};

long getCharAsInt(char c)
{_XLOG_
	if(c>='0' && c<='9'){_XLOG_
		return int(c)-int('0');
	}
	if(c>='a' && c<='z'){_XLOG_
		return int(c)-int('a')+10;
	}
	if(c>='A' && c<='Z'){_XLOG_
		return int(c)-int('A')+10;
	}
	return -1;
}

BOOL ParsedAsArithmeticalExpr(CString& sExpr)
{_XLOG_
	static long lLock=0;
	if(lLock>0){// Уже что-то считается...
		return -1;
	}
	SimpleTracker tr(lLock);
	int iRes=0;
	CString sResPrefix="";
	// Возможно просто случай - число в 10 или 16
	if(sExpr.Right(1)!="="){_XLOG_
		sExpr.TrimRight("=");
		if(sExpr.SpanIncluding("0123456789")==sExpr){_XLOG_
			sExpr+="(10->16)=";
			sResPrefix="0x";
		}
		if(sExpr.GetLength()>2 && sExpr.Left(2)=="0x" && sExpr.SpanIncluding("0123456789xabcdefABCDEF")==sExpr){_XLOG_
			sExpr=sExpr.Mid(2);
			sExpr+="(16->10)=";
		}
		if(sExpr.SpanIncluding("0123456789+-*/().,")==sExpr){_XLOG_
			sExpr+="=";
		}
	}
	if(sExpr.Right(1)=="="){_XLOG_
		long lResultIn=0;
		long lResultTo=0;
		BOOL bConversion=PatternMatch(sExpr,"*(*>*)=");
		if(!bConversion){_XLOG_
			bConversion=PatternMatch(sExpr,"*(*to*)=");
		}
		if(!bConversion){_XLOG_
			bConversion=PatternMatch(sExpr,"*(*2*)=");
		}
		if(bConversion){_XLOG_
			int iPos=sExpr.ReverseFind('(');
			int iPos2=sExpr.ReverseFind('>');
			if(iPos2==-1){_XLOG_
				iPos2=sExpr.Find("to",iPos);
				if(iPos2!=-1){_XLOG_
					iPos2++;
				}
			}
			if(iPos2==-1){_XLOG_
				iPos2=sExpr.Find("2",iPos);
			}
			if(iPos2>0){_XLOG_
				CString sTp=sExpr.Mid(iPos2+1);
				sTp.MakeLower();
				sTp.TrimLeft();
				sTp.TrimRight();
				if(sTp!=""){_XLOG_
					if(sTp[0]=='b'){_XLOG_
						lResultTo=2;
					}else if(sTp[0]=='d'){_XLOG_
						lResultTo=10;
					}else if(sTp[0]=='o'){_XLOG_
						lResultTo=8;
					}else if(sTp[0]=='h'){_XLOG_
						lResultTo=16;
					}else{_XLOG_
						lResultTo=atol(sTp);
						if(lResultTo==0){_XLOG_
							lResultTo=16;
						}
					}
				}
			}
			if(iPos>0){_XLOG_
				bConversion=1;
				CString sTp=sExpr.Mid(iPos+1);
				sExpr=sExpr.Left(iPos);
				sTp.MakeLower();
				sTp.TrimLeft();
				sTp.TrimRight();
				if(sTp[0]=='b'){_XLOG_
					lResultIn=2;
				}else if(sTp[0]=='d'){_XLOG_
					lResultIn=10;
				}else if(sTp[0]=='o'){_XLOG_
					lResultIn=8;
				}else if(sTp[0]=='h'){_XLOG_
					lResultIn=16;
				}else{_XLOG_
					lResultIn=atol(sTp);
					if(lResultIn==0){_XLOG_
						lResultIn=10;
					}
				}
			}
		}
		if(!bConversion){_XLOG_
			if(sExpr.Find("(b)=")!=-1 || sExpr.Find("(B)=")!=-1){_XLOG_
				bConversion=1;
				lResultIn=10;
				lResultTo=2;
			}
			if(sExpr.Find("(h)=")!=-1 || sExpr.Find("(H)=")!=-1){_XLOG_
				bConversion=1;
				lResultIn=10;
				lResultTo=16;
			}
			if(sExpr.Find("(o)=")!=-1 || sExpr.Find("(O)=")!=-1){_XLOG_
				bConversion=1;
				lResultIn=10;
				lResultTo=8;
			}
			if(sExpr.Find("(d)=")!=-1 || sExpr.Find("(D)=")!=-1){_XLOG_
				bConversion=1;
				lResultIn=16;
				lResultTo=10;
			}
		}
		sExpr.TrimRight("=");
		CString sScript=Format("var RESULT=0;with(Math){RESULT=eval(unescape('%s'));};RESULT;",EscapeString(sExpr));
		if(bConversion && sExpr.SpanExcluding("0123456789abcdefABCDEF to")==""){_XLOG_
			iRes=1;
			sScript=sExpr;
		}else{_XLOG_
			iRes=CalculateExpression_Ole(sScript, "JScript", bConversion);
		}
		if(bConversion && lResultIn>0 && lResultTo>0){_XLOG_
			if(iRes!=1){_XLOG_
				// Это было невычисляемо, значит 
				// просто переводим текст
				sScript=sExpr;
			}
			long lValue=0;
			long lCharValue=0;
			sScript.MakeUpper();
			while(sScript!="" && (lCharValue=getCharAsInt(sScript[0]))>=0){_XLOG_
				lValue=lValue*lResultIn+lCharValue;
				sScript=sScript.Mid(1);
			}
			char szBuffer[128]="";
			_itoa(lValue,szBuffer,lResultTo);
			sScript=szBuffer;
			sScript.MakeUpper();
			iRes=1;
		}else if(iRes && sScript.Find(",")!=-1 && sScript.GetLength()>2){_XLOG_
			// Исправляем запятые на точки
			char* szScriptPos=sScript.GetBuffer(sScript.GetLength());
			for(long i=1;i<long(strlen(szScriptPos)-1);i++){_XLOG_
				if(szScriptPos[i]==',' && szScriptPos[i-1]>='0' && szScriptPos[i-1]<='9' && szScriptPos[i+1]>='0' && szScriptPos[i+1]<='9'){_XLOG_
					szScriptPos[i]='.';
				}
			}
			sScript.ReleaseBuffer();
			
		}
		sExpr=CString(iRes==1?sResPrefix:"")+sScript;
	}
	return iRes;
}

DWORD WINAPI PasteClipText(LPVOID pData)
{_XLOG_
	TRACETHREAD;
	char* szChar=(char*)pData;
	if(!szChar){_XLOG_
		return 0;
	}
	CString sText=szChar;
	delete[] szChar;
	CString sTextToSet=sText;
	if(objSettings.bUseRegexps && objSettings.sRegexpWhat.GetLength()>0){_XLOG_
		sText.Replace("\"","\\\"");
		CString sRegexp=CString("\"")+sText+"\".replace(/"+objSettings.sRegexpWhat+"/ig,\""+objSettings.sRegexpTo+"\")=";
		CString sRegexpOrig=sRegexp;
		if(ParsedAsArithmeticalExpr(sRegexp)!=1){_XLOG_
			return 0;
		}
		if(sRegexpOrig!=sRegexp){_XLOG_
			sTextToSet=sRegexp;
		}
	}
	AsyncPlaySound(CLIP_PASTE);
	PutSelectedTextInFocusWnd(sTextToSet,0,objSettings.lLeaveInClipOnPaste);
	return 0;
}

BOOL ClipAddTextToPinned(const char* szText, const char* szHistName,BOOL bToPinned, const char* szDsc, const char* szApp)
{_XLOG_
	static CCriticalSection cs;
	CLocker locker(&cs);
	CStringArrayX tmp;
	if(!objSettings.sClipHistory.Lookup(szHistName,tmp)){_XLOG_
		objSettings.sClipHistory.SetAt(szHistName,tmp);
	}
	if(bToPinned){_XLOG_
		while(objSettings.sClipHistory[szHistName].GetSize()<objSettings.lClipHistoryDepth){_XLOG_
			CPairItem item("",szDsc,szApp);
			objSettings.sClipHistory[szHistName].Add(item);
		}
		// переноcим на новое если это раньше был обычный кусок
		CPairItem item(szText,szDsc,szApp);
		objSettings.sClipHistory[szHistName].Add(item);
	}else{_XLOG_
		CPairItem item(szText,szDsc,szApp);
		objSettings.sClipHistory[szHistName].InsertAt(0,item);
		if(objSettings.sClipHistory[szHistName].GetSize()>objSettings.lClipHistoryDepth){_XLOG_
			objSettings.sClipHistory[szHistName].RemoveAt(objSettings.lClipHistoryDepth);
		}
	}
	static DWORD dwLastOperTime=GetTickCount();
	if(objSettings.lSaveClipContentPer && GetTickCount()-dwLastOperTime>5*60*1000){_XLOG_
		dwLastOperTime=GetTickCount();
		objSettings.SaveHistories("");
	}
	return TRUE;
}

DWORD WINAPI NotifyClip(LPVOID p)
{
	CWKSYSEvent pEvent;
	pEvent.iEventType=IsBMP((char*)p)?WKSYSEVENTTYPE_COPYCLIPIMG:WKSYSEVENTTYPE_COPYTOCLIPB;
	pEvent.pData=p;
	GlobalNotifyAboutSystemEvent(&pEvent);
	free(p);
	return 0;
}

BOOL SetTextToClip(int iClip, CString& sText, BOOL bCheckAllHistory)
{_XLOG_
	if(iClip==0 && !IsBMP(sText)){
		FORK(NotifyClip,strdup(sText));
	}
	if(iClip>=0 && iClip<objSettings.sClipBuffer.GetSize() && sText.GetLength()!=0){_XLOG_
		SimpleLocker l(&csClipBuffer);
		objSettings.sClipBuffer[iClip]=sText;
		if(objSettings.sClipHistory[GetClipHistoryName(iClip)].GetSize()>0){_XLOG_
			if(bCheckAllHistory){_XLOG_
				CString sTextToCheck;
				if(IsBMP(sText)){_XLOG_
					sTextToCheck=CDataXMLSaver::GetInstringPart(BMP_SPECPREFIX"[","]",sText);
					int iPos=sTextToCheck.Find(",");
					if(iPos!=-1){_XLOG_
						sTextToCheck=sTextToCheck.Mid(iPos);
					}
				}else{_XLOG_
					sTextToCheck=sText;
				}
				for(int i=0;i<objSettings.sClipHistory[GetClipHistoryName(iClip)].GetSize();i++){_XLOG_
					CString sItemToCheck;
					if(IsBMP(objSettings.sClipHistory[GetClipHistoryName(iClip)][i].sItem)){_XLOG_
						sItemToCheck=CDataXMLSaver::GetInstringPart(BMP_SPECPREFIX"[","]",objSettings.sClipHistory[GetClipHistoryName(iClip)][i].sItem);
						int iPos=sItemToCheck.Find(",");
						if(iPos!=-1){_XLOG_
							sItemToCheck=sItemToCheck.Mid(iPos);
						}
					}else{_XLOG_
						sItemToCheck=objSettings.sClipHistory[GetClipHistoryName(iClip)][i].sItem;
					}
					if(sTextToCheck==sItemToCheck){_XLOG_
						return FALSE;
					}
				}
			}else{_XLOG_
				if(sText==objSettings.sClipHistory[GetClipHistoryName(iClip)][0].sItem){_XLOG_
					return FALSE;
				}
			}
		}
		// Активное в данный момент окно
		char szDsc[256]="";
		CString szApp;
		HWND hFore=::GetForegroundWindow();
		CString sDsc;
		if(hFore){_XLOG_
			::GetWindowText(hFore,szDsc,sizeof(szDsc));
			sDsc=szDsc;
			sDsc+="\n["+_l("Time")+": "+COleDateTime::GetCurrentTime().Format(0,LANG_USER_DEFAULT)+"]";//Format("%x",LOCALE_NOUSEROVERRIDE);
			szApp=GetExeFromHwnd(hFore);
		}
		ClipAddTextToPinned(sText,GetClipHistoryName(iClip),(GetAsyncKeyState(VK_RWIN)<0),sDsc,szApp);
		return TRUE;
	}
	return FALSE;
}

void SendCtrlV(BOOL m_bUseWMCOPY, HWND hTargetWnd);
DWORD WINAPI TransmitTextToTarget(LPVOID pData)
{_XLOG_
	TRACETHREAD;
	char* szText=(char*)pData;
	CString sText=szText;
	free((LPVOID)szText);
	if(objSettings.hWndToTransmitTo!=NULL && ::IsWindow(objSettings.hWndToTransmitTo)){_XLOG_
		HWND hCurrent=GetForegroundWindow();
		SwitchToWindow(objSettings.hWndToTransmitTo,TRUE);
		PutSelectedTextInFocusWnd(sText,0,1);
		SwitchToWindow(hCurrent,TRUE);
		return TRUE;
	}
	objSettings.lClipTransmitMode=0;
	objSettings.hWndToTransmitTo=NULL;
	AsyncPlaySound(SND_ERROR);
	return FALSE;
}

DWORD WINAPI ShowBaloon_InThread(LPVOID pData)
{_XLOG_
	TRACETHREAD;
	static long lLock=0;
	/*if(lLock>0){_XLOG_
		return 0;
	}*/
	SimpleTracker tr(lLock);
	long lLockAtStart=lLock;
	BaloonInfo* bi=(BaloonInfo*)pData;
	if(!pMainDialogWindow || !bi){_XLOG_
		return 0;
	}
	if(pMainDialogWindow->m_STray.Visible()){_XLOG_
		pMainDialogWindow->m_STray.PopupBaloonTooltip("","");
		pMainDialogWindow->m_STray.PopupBaloonTooltip(bi->sTitle,bi->sText,0,10000);
		Sleep(bi->dwTimeout);
		if(lLockAtStart==lLock && pMainDialogWindow && pMainDialogWindow->iRunStatus!=-2){_XLOG_
			pMainDialogWindow->m_STray.PopupBaloonTooltip("","");
		}
	}else if(!bi->bBaloonOnly){_XLOG_
		Alert(bi->sText,bi->sTitle,bi->dwTimeout);
	}
	delete bi;
	return 1;
}

void ShowBaloon(const char* szText, const char* szTitle, DWORD dwTimeout, BOOL bBaloonOnly)
{_XLOG_
	BaloonInfo* bi=new BaloonInfo();
	bi->sText=szText;
	bi->sTitle=szTitle;
	bi->dwTimeout=dwTimeout;
	bi->bBaloonOnly=bBaloonOnly;
	FORK(ShowBaloon_InThread,bi);
}

void SubstMyVariablesWK(CString& sText, BOOL bJScriptSafe)
{_XLOG_
__FLOWLOG;
	BOOL bNeedActive=(sText.Find("%ACTIVE")!=-1);
	bNeedActive|=(sText.Find("%E_ACTIVE")!=-1);
	if(bNeedActive){_XLOG_
		char szTitle[1024]="";
		HWND hCurWnd=GetForegroundWindow();
		CString sExe="",sFile="";
		if(hCurWnd){_XLOG_
			sExe=GetExeFromHwnd(hCurWnd);
			::GetWindowText(hCurWnd,szTitle,sizeof(szTitle));
			char szFile[MAX_PATH]="";
			_splitpath(sExe,NULL,NULL,szFile,NULL);
			sFile=szFile;
		}
		CString sWnd=szTitle;
		sFile.MakeUpper();
		sText.Replace("%ACTIVEAPP",sFile);
		sText.Replace("%E_ACTIVEAPP",EscapeString(sFile));
		sText.Replace("%ACTIVEEXE",sExe);
		sText.Replace("%E_ACTIVEEXE",EscapeString(sExe));
		sText.Replace("%ACTIVEWND",sWnd);
		sText.Replace("%E_ACTIVEWND",EscapeString(sWnd));
	}
	int iPrECount=0;
	int iIsRunPos=sText.Find("%ISRUN:");
	CCSLock lpc(&pCahLock,1);
	while(iIsRunPos!=-1){_XLOG_
		if(iPrECount==0){_XLOG_
			EnumProcesses();
		}
		iPrECount++;
		CString sRes="0";
		CString sExeName=sText.Mid(iIsRunPos+strlen("%ISRUN:")).SpanExcluding(" ;\"',/*=![](){}~@#$%^&");
		for(int iProcNum=0; iProcNum<GetProccessCash().GetSize();iProcNum++){_XLOG_
			CProcInfo& info=GetProccessCash()[iProcNum];
			if(info.sProcName.CompareNoCase(sExeName)==0){_XLOG_
				sRes="1";
			}
		}
		sText.Replace("%ISRUN:"+sExeName,sRes);
		iIsRunPos=sText.Find("%ISRUN:");
	}
	SubstMyVariables(sText,bJScriptSafe);
}

//DEFINE_GUID(IID_IDirectDrawVideo, 0x36d39eb0, 0xdd75, 0x11ce, 0xbf, 0x0e, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a)
BOOL ReparseWinampTitle(CString& sTitle, HWND hWAWnd, BOOL bTotOnly)
{_XLOG_
	if(objSettings.lStripNumberWinampTitle){_XLOG_
		int iPos=sTitle.Find(" - Winamp");
		if(iPos!=-1){_XLOG_
			sTitle=sTitle.Left(iPos);
		}
		CString sSongNumber=Format("%lu. ",atol(sTitle));
		int iPos2=sTitle.Find(sSongNumber);
		if(iPos2==0){_XLOG_
			sTitle=sTitle.Mid(sSongNumber.GetLength());
		}
	}
	if(objSettings.lAddTLWinampTitle && !bTotOnly){_XLOG_
		int iCurMills=::SendMessage(hWAWnd,WM_USER,0,105);
		if(iCurMills<0){_XLOG_
			iCurMills=0;
		}
		sTitle+=Format(" [%02i:%02i]",int(iCurMills/1000/60),int(iCurMills/1000)%60);
		if(objSettings.lAddTTWinampTitle){_XLOG_
			sTitle+=" / ";
		}
	}
	if(objSettings.lAddTTWinampTitle){_XLOG_
		int iTSeconds=::SendMessage(hWAWnd,WM_USER,1,105);
		if(iTSeconds<0){_XLOG_
			iTSeconds=0;
		}
		sTitle+=Format(" [%02i:%02i]",int(iTSeconds/60),iTSeconds%60);
	}
	return TRUE;
};

void FillSplashParams(const char* szTitle, CSplashParams& par)
{_XLOG_
	par.szText=par.sTextPrefix+TrimMessage(szTitle,objSettings.lOSDChars,1)+par.sTextPosfix;
	par.bgColor=objSettings.lSplashBgColor;
	par.txColor=objSettings.lSplashTextColor;
	par.pFont=objSettings.pSplashFont;
	par.dwTextParams=0;
	par.bFade=objSettings.bUnderWindows98?0:objSettings.lDeskfadeOSD;
	if(objSettings.lOSDBorder){_XLOG_
		par.dwTextParams|=TXSHADOW_VCENT|TXSHADOW_FILLBG|TXSHADOW_MIXBG;////TXSHADOW_SHCOL
		if(objSettings.lOSDBorder==1 || objSettings.lOSDBorder==3){_XLOG_
			par.dwTextParams|=TXSHADOW_WIDE;
		}
		if(objSettings.lOSDBorder==1 || objSettings.lOSDBorder==2){_XLOG_
			par.dwTextParams|=TXSHADOW_GLOW;
		}else{_XLOG_
			par.dwTextParams|=TXSHADOW_SIMPLE;
		}
	}
	par.dwSplashAlign=SPLASH_OPACITY;
	DWORD dwTextParAlign=TXSHADOW_HCENT;
	if(objSettings.lStartupMsgType==0){_XLOG_
		par.dwSplashAlign|=SPLASH_HIDDEN;
	}
	if(objSettings.lNoNeedOSD){_XLOG_
		par.dwSplashAlign|=SPLASH_HIDDEN;
	}
	if(objSettings.lStartupMsgType==1 || objSettings.lStartupMsgType==4){_XLOG_
		par.dwSplashAlign|=SPLASH_HLEFT;
		dwTextParAlign=0;
	}
	if(objSettings.lStartupMsgType==2 || objSettings.lStartupMsgType==3){_XLOG_
		par.dwSplashAlign|=SPLASH_HRIGHT;
		dwTextParAlign=TXSHADOW_RIGHT;
	}
	if(objSettings.lStartupMsgType==1 || objSettings.lStartupMsgType==2 || objSettings.lStartupMsgType==6){_XLOG_
		par.dwSplashAlign|=SPLASH_VTOP;
	}
	if(objSettings.lStartupMsgType==3 || objSettings.lStartupMsgType==4 || objSettings.lStartupMsgType==7){_XLOG_
		par.dwSplashAlign|=SPLASH_VBOTTOM;
	}
	if(objSettings.lStartupMsgType==5 || objSettings.lStartupMsgType==6 || objSettings.lStartupMsgType==7){_XLOG_
		par.dwSplashAlign|=SPLASH_HCENTER;
		if(objSettings.lStartupMsgType==5){_XLOG_
			par.dwSplashAlign|=SPLASH_VMIDDLE;
		}
	}
	par.dwTextParams|=dwTextParAlign;
}

DWORD WINAPI ShowWTitleSplash(LPVOID pData)
{_XLOG_
	CSplashParams* par=(CSplashParams*)pData;
	static long lCount=0;
	if(lCount>0){_XLOG_
		delete par;
		return 0;
	}
	SimpleTracker tr(lCount);
	if(par){_XLOG_
		if(!objSettings.lNoNeedOSD){// Показываем сплэш-стринг
			CSplashWindow OSD(par);
			Sleep(objSettings.lWinampOSDTime*1000);
		}
		delete par;
	}
	return 0;
}

void ExecuteMacroByString(CString sExe)
{_XLOG_
	HWND hMainI=FindWindow("WK_MAIN",0);
	HWND hMain=(HWND)GetProp(hMainI,"MAIN_WND");
	if(hMain==0 || !IsWindow(hMain)){_XLOG_
		CString sErr=_l("Error")+": "+_l("To activate this action, "PROGNAME" must be started already")+"!";
		#ifdef _DEBUG
		sErr+=Format("\n%x/%x",hMainI,hMain);
		#endif
		AfxMessageBox(sErr);
	}else{_XLOG_
		COPYDATASTRUCT cpd;
		cpd.dwData = MAKELONG(0,WM_CREATE);
		cpd.cbData = strlen(sExe)+1;
		cpd.lpData = (void*)(sExe.GetBuffer(1));
		BOOL b=SendMessage( hMain, WM_COPYDATA, (WPARAM)AppMainApp::m_pFakeWnd->GetSafeHwnd(), (LPARAM)&cpd );
	}
}

BOOL GetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut, size_t dwOutSize)
{_XLOG_
	CRegKey key;
	CString sKeyPath=CString(PROG_REGKEY);
	if(szFName && szFName[0]){_XLOG_
		sKeyPath+="\\";
		sKeyPath+=szFName;
	}
	if(key.Open(HKEY_CURRENT_USER, sKeyPath)!=ERROR_SUCCESS){_XLOG_
		key.Create(HKEY_CURRENT_USER, sKeyPath);
	}
	if(key.m_hKey!=NULL){_XLOG_
		DWORD dwType=REG_SZ;
		DWORD dwSize=dwOutSize;
		RegQueryValueEx(key.m_hKey, szRegSettingsName, NULL, &dwType, (LPBYTE)szOut, &dwSize);
	}
	return TRUE;
}

BOOL SetRegSetting(const char* szFName, const char* szRegSettingsName, char* szOut)
{_XLOG_
	CRegKey key;
	CString sKeyPath=CString(PROG_REGKEY);
	if(szFName && szFName[0]){_XLOG_
		sKeyPath+="\\";
		sKeyPath+=szFName;
	}
	if(key.Open(HKEY_CURRENT_USER, sKeyPath)!=ERROR_SUCCESS){_XLOG_
		key.Create(HKEY_CURRENT_USER, sKeyPath);
	}
	if(key.m_hKey!=NULL){_XLOG_
		DWORD dwType=REG_SZ,dwSize=0;
		RegSetValueEx(key.m_hKey,szRegSettingsName,0,REG_SZ,(BYTE*)(LPCSTR)szOut,lstrlen(szOut)+1);
	}
	return TRUE;
}

typedef unsigned __int32 uint32_t;
bool UTF2U(const char* src,CString& outc)
{_XLOG_

    bool res = true;
    int mState = 0;
    int mUcs4  = 0;
    int mBytes = 1;
	int iLen=strlen(src);
	wchar_t* out=new wchar_t[iLen];
	memset(out,0,sizeof(wchar_t)*iLen);
	int iw=0;
    for(const char *in = src; in-src<iLen; in++)
    {_XLOG_
        if(0 == mState) {_XLOG_
            if( 0 == (0x80 & (*in))) {_XLOG_
                // ASCII
                out[iw++] = *in;
                mBytes =1;
            } else if( 0xC0 == (0xE0 & (*in))) {_XLOG_
                // 2-х байтный UTF8
                mUcs4 = (uint32_t)(*in);
                mUcs4 = (mUcs4 << 6) & 0x000007C0L;
                mState=1;
                mBytes =2;
            } else if( 0xE0 == (0xF0 & (*in))) {_XLOG_
                // 3-х байтный UTF8
                mUcs4 = (uint32_t)(*in);
                mUcs4 = (mUcs4 << 12) & 0x0000F000L;
                mState=2;
                mBytes =3;
            } else if( 0xF0 == (0xF8 & (*in))) {_XLOG_
                // 4-х байтный UTF8
                mUcs4 = (uint32_t)(*in);
                mUcs4 = (mUcs4 << 18) & 0x001F0000L;
                mState=3;
                mBytes =4;
            } else if( 0xF8 == (0xFC & (*in))) {_XLOG_
                // 5-х байтный UTF8
                mUcs4 = (uint32_t)(*in);
                mUcs4 = (mUcs4 << 24) & 0x03000000L;
                mState=4;
                mBytes =5;
            } else if( 0xFC == (0xFE & (*in))) {_XLOG_
                // 6-х байтный UTF8
                mUcs4 = (uint32_t)(*in);
                mUcs4 = (mUcs4 << 30) & 0x40000000L;
                mState=5;
                mBytes =6;
            } else {_XLOG_
                res = false;
                break;
            }
        } else {_XLOG_
            if(0x80 == (0xC0 & (*in))) {_XLOG_
                uint32_t tmp = (*in);
                uint32_t shift = (mState-1) * 6;
                tmp = (tmp << shift ) & ( 0x0000003FL << shift);
                mUcs4 |= tmp;
                if(0 == --mState) {_XLOG_
                    if(mUcs4 >= 0x00010000) {_XLOG_
                        if(mUcs4 >= 0x00110000) {_XLOG_
                            out[iw++] = (wchar_t)0xFFFD;
                        } else {_XLOG_
                            mUcs4 -= 0x00010000;
                            out[iw++] = (wchar_t)(0xD800 | (0x000003FF & (mUcs4 >> 10)));
                            out[iw++] = (wchar_t)(0xDC00 | (0x000003FF & mUcs4));
                        }
                    } else {_XLOG_
                        // from Unicode 3.1, non-shortest form is illegal 
                        if(((2==mBytes) && (mUcs4 < 0x0080)) ||
                            ((3==mBytes) && (mUcs4 < 0x0800)) ||
                            ((4==mBytes) && (mUcs4 < 0x1000)) ||
                            (5==mBytes) ||
                            (6==mBytes)) 
                        {_XLOG_
                            res = false;
                            break;
                        } 

                        if( 0xfeff != mUcs4 ) // ignore BOM
                        {  
                            out[iw++] = (wchar_t)mUcs4;
                        }
                    }
                    mState = 0;// cached expected number of bytes per UTF8 character sequence
                    mUcs4  = 0;// cached Unicode character
                    mBytes = 1;
                }
            } else {_XLOG_
                res = false;
                break;
            }
        }
    }
	outc=out;
	delete[] out;
    return(res);
}

CString getHotKeyStr(CIHotkey oHotKey, BOOL bWithFollowers)
{_XLOG_
	CString sRes=GetIHotkeyName(oHotKey,bWithFollowers);
	if(sRes==""){_XLOG_
		sRes="<";
		sRes+=_l("not defined");
		sRes+=">";
	}
	return sRes;
}

CString& GetPluginsDirectory()
{_XLOG_
	static CString sDir;
	return sDir;
}

CString getHotKeyStrAlt(CIHotkey oHotKey,CString sPrefix)
{_XLOG_
	CString sRes="";
	if(oHotKey.pNextHK){_XLOG_
		sRes=GetIHotkeyName(*oHotKey.pNextHK,TRUE);
	}
	if(sRes!=""){_XLOG_
		sRes=sPrefix+sRes;
	}
	return sRes;
}

BOOL WINAPI GetFileDescription(const char* szFileName, char szDescOut[256])
{_XLOG_
	memset(szDescOut,0,sizeof(szDescOut));
	CFileVersionInfo fVer;
	if(fVer.Open(szFileName)){_XLOG_
		fVer.QueryStringValue(VI_STR_FILEDESCRIPTION,szDescOut,256);
		return 1;
	}
	char szD[MAX_PATH]="";
	char szP[MAX_PATH]="";
	char szN[MAX_PATH]="";
	char szE[MAX_PATH]="";
	_splitpath(szFileName,szD,szP,szN,szE);
	strcpy(szDescOut,szN);
	return 0;
}
