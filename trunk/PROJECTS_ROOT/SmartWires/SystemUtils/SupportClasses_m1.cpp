// SupportClasses.cpp: implementation of the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SupportClasses.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CShlwapi& _Shlwapi()
{_XLOG_
	static CShlwapi obj;
	return obj;
}

void ConcateDir(CString& sDir,const char* szAdded)
{_XLOG_
	if (sDir.GetLength()>0 && sDir[sDir.GetLength()-1]!='\\'){_XLOG_
		sDir+="\\";
	}
	sDir+=szAdded;
	return;
};

long& GetApplicationLang()
{_XLOG_
	static long dwInterfaceLang=0;// Язык, 0- английский, 1-русский
	return dwInterfaceLang;
}

CString GetUserFolderRaw()
{_XLOG_
	static char szUserFolder[MAX_PATH]="";
	if(szUserFolder[0]!=0){_XLOG_
		return szUserFolder;
	}
	CString sFolder;
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_APPDATA);
	if(szFolderBuffer[0]==0){_XLOG_
		strcpy(szFolderBuffer,GetApplicationDir());
	}
	sFolder=szFolderBuffer;
	ConcateDir(sFolder,GetApplicationName());
	if(int(GetFileAttributes(sFolder))==-1){_XLOG_
		CreateDirectory(sFolder,NULL);
	}
	sFolder+="\\";
	strcpy(szUserFolder,sFolder);
	return sFolder;
}

CString GetAppFolder()
{_XLOG_
	return GetApplicationDir();
}

CString GetUserFolder()
{_XLOG_
	static char szUserFolder[MAX_PATH]="";
	if(szUserFolder[0]!=0){_XLOG_
		return szUserFolder;
	}
	CString sFolder;
	if(GetCommandLineParameter("ul",sFolder)){_XLOG_
		if(sFolder==""){_XLOG_
			sFolder=GetApplicationDir();
			ConcateDir(sFolder,"UserData");
		}
	}else{_XLOG_
		sFolder=CString(GetApplicationDir())+"Profile";
		if(!isFileExist(sFolder)){_XLOG_
			char szFolderBuffer[MAX_PATH]="";
			GetSpecialFolderPath(szFolderBuffer,CSIDL_APPDATA);
			if(szFolderBuffer[0]==0){_XLOG_
				strcpy(szFolderBuffer,GetApplicationDir());
			}
			sFolder=szFolderBuffer;
			ConcateDir(sFolder,GetApplicationName());
		}
	}
	if(int(GetFileAttributes(sFolder))==-1){_XLOG_
		CreateDirectory(sFolder,NULL);
	}
	sFolder+="\\";
	strcpy(szUserFolder,sFolder);
	return sFolder;
}

char appDir[MAX_PATH]="";
void SetAppDir(const char* sz)
{_XLOG_
	strcpy(appDir,sz);
}

const char* GetApplicationDir()
{_XLOG_
	if(appDir[0]!=0){_XLOG_
		return appDir;
	}
	char szCurrentExeFile[MAX_PATH]="";
	GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
	char szDrive[MAX_PATH],szDir[MAX_PATH];
	_splitpath(szCurrentExeFile, szDrive, szDir, NULL, NULL);
	strcpy(appDir,szDrive);
	strcat(appDir,szDir);
	return appDir;
}

CString GetAppName()
{_XLOG_
	return GetApplicationName();
}

CString GetAppExe(BOOL bCheckSP)
{_XLOG_
	static char szCurrentExeFile[MAX_PATH]="";
	if(szCurrentExeFile[0]==0){_XLOG_
		GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
		#ifdef _DEBUG
		bCheckSP=1;
		#endif
		if(bCheckSP && strchr(szCurrentExeFile,' ')!=0){_XLOG_
			CString sRes="\"";
			sRes+=szCurrentExeFile;
			sRes+="\"";
			strcpy(szCurrentExeFile,sRes);
		}
	}
	return szCurrentExeFile;
}

char appName[MAX_PATH]="";
void SetAppName(const char* sz)
{_XLOG_
	strcpy(appName,sz);
}

const char* GetApplicationName()
{_XLOG_
	
	if(appName[0]!=0){_XLOG_
		return appName;
	}
	char szCurrentExeFile[MAX_PATH]="";
	/*int iParsedParams=0;
	USES_CONVERSION;
	LPCWSTR szEmp=A2W("");
	LPWSTR* szwExeFile=CommandLineToArgvW(szEmp,&iParsedParams);
	if(szwExeFile && *szwExeFile!=NULL){_XLOG_
		strcpy(szCurrentExeFile,W2A(*szwExeFile));
	}else*/
	CString sCommandLine=GetCommandLine(),sExeFile,sParams;
	if(ParseForShellExecute(sCommandLine,sExeFile,sParams)){_XLOG_
		strcpy(szCurrentExeFile,sExeFile);
	}else{_XLOG_
		GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
	}
	char szDrive[MAX_PATH],szDir[MAX_PATH], szExt[MAX_PATH];
	_splitpath(szCurrentExeFile, szDrive, szDir, appName, szExt);
	return appName;
}

CString Format(const char* szFormat,...)
{_XLOG_
	va_list vaList;
	va_start(vaList,szFormat);
	CString sBuffer;
	sBuffer.FormatV(szFormat,vaList);
	va_end (vaList);
	return sBuffer;
}

CString Format(long l)
{_XLOG_
	return Format("%i",l);
}

#define MAXKLEN		30
#define NEWLINE		"<br>"
#define NEWLINE2	"<brr>"
//#define _USECASH
CString _getLngString(const char* szText,const char* szDef, const char* szDir, const char* szFile, int iLang)
{_XLOG_
#ifdef NO_AUTOSAVESTRINGS
	#pragma message("Auto save missed strings: disabled")
	return szText;
#endif
	if(iLang==-1){_XLOG_
		iLang=GetApplicationLang();
	}
	/*
	if(CString(szText)=="Пустой"){_XLOG_
		int a=0;
	}
	*/
	if(szText==NULL || *szText==0){_XLOG_
		return "";
	}
	CString sKey=szText,sValue;
	CapitalizeAndShrink(sKey,LANG_WRONGDLMS);
	if(sKey.GetLength()>MAXKLEN){_XLOG_
		sKey=sKey.Left(MAXKLEN-11)+"_"+GetStringHash(sKey);
	}
	sKey+="=";
#ifdef _USECASH	
	CString sCashKey;
	// Смотрим в кэш
	sCashKey=Format("%s%i%s",szFile,iLang,sKey);
	if(GetLangTermCash().Lookup(sCashKey,sValue)){_XLOG_
		return sValue;
	}
#endif	
	char szLastFile[MAX_PATH]="";
	sprintf(szLastFile,"%s%s_l%i.lng",szDir,szFile,iLang);
#ifdef FNF_SUPPORT_LNG_AST
	char szLastFile2[MAX_PATH]="";
	sprintf(szLastFile2,"%s%s*_l%i.lng",szDir,szFile,iLang);
	sValue=objFileParser().getByKey(szLastFile2,sKey);
#else
	sValue=objFileParser().getByKey(szLastFile,sKey);
#endif
	if(sValue.GetLength()==0){_XLOG_
		// Дополняем кэш и файл недостающей строчкой
		BOOL bGetted=0;
		static CCriticalSection cs;
		CSmartLock lngl(cs,TRUE);
		if(!IsFileUnicode(szLastFile)){// Приписываем недостающий термин
			CString sSetValue="";
			if(iLang!=0){_XLOG_
				sSetValue=_getLngString(szText,szDef, szDir, szFile, 0);// Тут принудительно 0! вытаскиваем термин английского
			}else{_XLOG_
				sSetValue=szDef;
				if(!szDef){_XLOG_
					sSetValue=szText;
				}
			}
			sValue=sSetValue;
			CString sBeforeRead;
			ReadFile(szLastFile,sBeforeRead);
			if(sBeforeRead.Find(CString("\n")+sKey)==-1){_XLOG_
				sSetValue.Replace("\r\n",NEWLINE2);
				sSetValue.Replace("\n",NEWLINE);
				sSetValue.Replace("\r","");
				FILE* m_logFile=getFile(szLastFile,"a+");
				if(m_logFile!=NULL){_XLOG_
					fprintf(m_logFile,"\n%s%s",sKey,sSetValue);
					fclose(m_logFile);
				}
			}
		}
	}
	sValue.Replace(NEWLINE,"\n");
	sValue.Replace(NEWLINE2,"\r\n");
#ifdef _USECASH
	GetLangTermCash().SetAt(sCashKey,sValue);
#endif
	return sValue;
}

CString _l(const char* szText,const char* szDef)
{_XLOG_
	return _getLngString(szText,szDef,GetApplicationDir(),GetApplicationName());
}

CString GetLangName(int iLangNum)
{_XLOG_
	CString sLangTermKey=Format("LangName%i",iLangNum);
	CString sLangName=_l(sLangTermKey);
	if(strcmp(sLangName,sLangTermKey)==0){_XLOG_
		// По умолчанию
		if(iLangNum==0){_XLOG_
			sLangName="English";
		}
	}
	return sLangName;
}

CString TrimMessage(CString sMessage, int iSize, int bPrepare)
{_XLOG_
	BOOL bNeedPostProcessing=(bPrepare==0)?FALSE:TRUE;
	CString sOut;
	int iStopPrefix=sMessage.Find(BMP_SPECPREFIX);
	if(iStopPrefix!=-1){_XLOG_
		sOut=sMessage.Left(iStopPrefix);
	}else{_XLOG_
		sOut=sMessage;
	}
	int iLen=sOut.GetLength();
	if(iLen>iSize){_XLOG_
		if(bPrepare==2){_XLOG_
			sOut=sMessage.Left(iSize/2-3)+"..."+sMessage.Right(iSize/2);
		}else if(bPrepare==3){_XLOG_
			char szPath[MAX_PATH]="";
			if(_Shlwapi().fpCompact){_XLOG_
				_Shlwapi().fpCompact(szPath,sMessage,iSize,0);
				sOut=szPath;
			}else{_XLOG_
				bPrepare=1;
			}
		}else if(bPrepare==4){_XLOG_
			static CString sDelimiters=" ,./\r\n\\-=+";
			int iPos=iSize*int(iLen/iSize);
			bNeedPostProcessing=FALSE;
			while(iPos>=iSize){_XLOG_
				int iInsertPos=iPos;
				while(iPos-iInsertPos<iSize/2 && sDelimiters.Find(sOut[iInsertPos-1])==-1){_XLOG_
					iInsertPos--;
				}
				sOut.Insert(iInsertPos,'\n');
				iPos-=iSize;
			}
		}else if(bPrepare==5){_XLOG_
			sOut=CString((iLen>iSize?"...":""))+sMessage.Right(iSize);
		}
		if(bPrepare<2){_XLOG_
			sOut=sMessage.Left(iSize)+(iLen>iSize?"...":"");
		}
	}
	if(bNeedPostProcessing){_XLOG_
		sOut.Replace("\t"," ");
		sOut.Replace("\n"," ");
		sOut.Replace("\r"," ");
		sOut.TrimLeft();
		sOut.TrimRight();
	}
	return sOut;
}

#ifdef _DEBUG
CString& GetGlobalClipTextD()
{_XLOG_
	static CString sTest;
	return sTest;
}
#endif

HGLOBAL& GetGlobalClipText(BOOL bWide)
{_XLOG_
	static HGLOBAL hGlobalClipTextA=NULL;
	static HGLOBAL hGlobalClipTextW=NULL;
	if(bWide){_XLOG_
		return hGlobalClipTextW;
	}
	return hGlobalClipTextA;
}

CCriticalSection csGetClip;
BOOL SetClipboardText(const WCHAR* wszText, BOOL& bThroughGlobal, CWnd* pWndForOpen)
{_XLOG_
	SimpleLocker lc(&csGetClip);
	BOOL bRes=1;
	DWORD dwSleepOnExit=0;
	CWnd* pWnd=pWndForOpen;
	if(pWnd==0){_XLOG_
		pWnd=AfxWPGetMainWnd();
	}
	if(!pWnd || !pWnd->OpenClipboard()){_XLOG_
		if(pWnd->GetSafeHwnd()!=::GetOpenClipboardWindow()){_XLOG_
			bRes=0;
		}
	}
	::EmptyClipboard();
	if(wszText!=NULL){_XLOG_
		static long bWorkOnNT=-1;
		if(bWorkOnNT==-1){// на 98ых работает только если память - FIXED
			CString sVer;
			GetOSVersion(sVer, NULL, &bWorkOnNT);
		}
#ifdef _DEBUG
		GetGlobalClipTextD()=wszText;
#endif
		BOOL bSkipRest=0;
		if(!bSkipRest){// Для Win2000/NT/XP - CF_DIB
			const char* szClipBMP=(const char*)wszText;
			const char* szBMPStart=strstr(szClipBMP,BMP_SPECPREFIX);
			if(szBMPStart!=0){_XLOG_
				bSkipRest=1;
				int iFrom=0;
				HGLOBAL hClipDib=0;
				DWORD dwSize=atol(CDataXMLSaver::GetInstringPart(BMP_SPECPREFIX"[","]",szBMPStart,iFrom));
				PackFromString(szBMPStart+iFrom,dwSize,hClipDib,((bWorkOnNT)?GMEM_MOVEABLE:GMEM_FIXED)|GMEM_ZEROINIT);
				if(hClipDib!=NULL){_XLOG_
					bThroughGlobal=0;
					::SetClipboardData(CF_DIB,hClipDib);
					dwSleepOnExit=int(1000*double(dwSize/3000000));
				}
			}
		}
		if(!bSkipRest){// Для Win2000/NT/XP - CF_UNICODETEXT
			WCHAR* szCliptextW=NULL;
			HGLOBAL hCliptextW=::GlobalAlloc((bWorkOnNT?GMEM_MOVEABLE:GMEM_FIXED)|GMEM_ZEROINIT,(wcslen(wszText)+1)*2);
			if(hCliptextW!=NULL){_XLOG_
				szCliptextW=(WCHAR*)::GlobalLock(hCliptextW);
				wcscpy(szCliptextW,wszText);
				if(bThroughGlobal){_XLOG_
					GetGlobalClipText(1)=hCliptextW;
				}
				::GlobalUnlock(hCliptextW);
				::SetClipboardData(CF_UNICODETEXT,bThroughGlobal?NULL:hCliptextW);//???WTF??? szCliptextW
			}
		}
		if(!bSkipRest){// Для Win98/ME/не юникодовских программ - CF_TEXT
			char* szCliptextA=NULL;
			HGLOBAL hCliptextA=::GlobalAlloc((bWorkOnNT?GMEM_MOVEABLE:GMEM_FIXED)|GMEM_ZEROINIT,wcslen(wszText)+1);
			if(hCliptextA!=NULL){_XLOG_
				szCliptextA=(char*)::GlobalLock(hCliptextA);
				WideCharToMultiByte(CP_ACP, 0, wszText, -1, szCliptextA, wcslen(wszText), NULL, NULL);
				if(bThroughGlobal){_XLOG_
					GetGlobalClipText(0)=hCliptextA;
				}
				::GlobalUnlock(hCliptextA);
				::SetClipboardData(CF_TEXT,bThroughGlobal?NULL:hCliptextA);//???WTF??? szCliptextA
			}
		}
	}
	::CloseClipboard();
	if(dwSleepOnExit){_XLOG_
		Sleep(dwSleepOnExit);
	}
	return bRes;
}

CString GetClipboardLinksAsText(BOOL bOpenCloseClip)
{_XLOG_
	SimpleLocker lc(&csGetClip);
	if(bOpenCloseClip){_XLOG_
		CWnd* pWnd=AfxWPGetMainWnd();
		if(!pWnd || !pWnd->OpenClipboard()){_XLOG_
			return "";
		}
	}
	CString str="";
	HDROP hCliptext=(HDROP)::GetClipboardData(CF_HDROP);
	if(hCliptext){_XLOG_
		char szFilePath[256];
		UINT cFiles = DragQueryFile(hCliptext, (UINT)-1, NULL, 0);
		for (UINT u = 0; u < cFiles; u++){_XLOG_
			DragQueryFile(hCliptext, u, szFilePath, sizeof(szFilePath));
			str+=szFilePath;
			if(u!=cFiles-1){_XLOG_
				str+="\n";
			}
		}
	}
	if(bOpenCloseClip){_XLOG_
		::CloseClipboard();
	}
	return str;
}

BOOL IsBMP(const char* szStr)
{_XLOG_
	if(szStr){_XLOG_
		if(strstr(szStr,BMP_SPECPREFIX)!=0){_XLOG_
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsBMP(const WCHAR* wStr)
{_XLOG_
	if(wStr){_XLOG_
		const char* sz=(const char*)wStr;
		if(strstr(sz,BMP_SPECPREFIX)!=0){_XLOG_
			return TRUE;
		}
	}
	return FALSE;
}

DWORD& MaxBmpSize()
{_XLOG_
	static DWORD dw=0;
	return dw;
}

WCHAR* NewWCHAR(int size)
{
	WCHAR* res=new WCHAR[size+__alignof(WCHAR)];
	memset(res,0,(size+__alignof(WCHAR))*sizeof(WCHAR));
	return res;
}

// Переделать на w-string!
WCHAR* GetClipboardText(BOOL bAndClear, BOOL bWithBitmaps, BOOL bAndOpen, DWORD* dwLen)
{_XLOG_
	__FLOWLOG;
	SimpleLocker lc(&csGetClip);
	if(bAndOpen){
		CWnd* pWnd=AfxWPGetMainWnd();
		if(!pWnd || !pWnd->OpenClipboard()){_XLOG_
			return NULL;
		}
	}
	LOGSTAMP
	//bWithBitmaps=0;
	//MaxBmpSize()=100;
	// Получаем в UniCode
	WCHAR* strWOUT=NULL;
	BOOL bSkipRest=0;
	BOOL bWasLink=0;
	DWORD dwOutLen=0;
	LOGSTAMP
	if(!bSkipRest && IsClipboardFormatAvailable(CF_HDROP)){_XLOG_
		LOGSTAMP
		if(dwOutLen==0){// Может есть линки на файлы?
			LOGSTAMP
			CString str=GetClipboardLinksAsText(0);
			LOGSTAMP
			size_t len=strlen(str);
			if(len>0){_XLOG_
				LOGSTAMP
				if(strWOUT){_XLOG_
					LOGSTAMP
					delete[] strWOUT;
					strWOUT=0;
					dwOutLen=0;
				}
				USES_CONVERSION;
				dwOutLen=len+1;
				strWOUT = NewWCHAR(dwOutLen);
				wcscpy(strWOUT,A2W(str));
				bSkipRest=1;
				bWasLink=1;
				LOGSTAMP
			}
		}
	}
#ifdef _DEBUG
	if(bWasLink){_XLOG_
		FLOG("Getting links");
	}else{_XLOG_
		FLOG("Skipping links");
	}
#endif
	if(!bSkipRest && IsClipboardFormatAvailable(CF_UNICODETEXT)){_XLOG_
		LOGSTAMP
		HANDLE hCliptextW=::GetClipboardData(CF_UNICODETEXT);
		DWORD dwErr=GetLastError();
		if(hCliptextW){_XLOG_
			DWORD dwDataSize=GlobalSize(hCliptextW);
			LOGSTAMP
			if(MaxBmpSize()==0 || dwDataSize<MaxBmpSize()){_XLOG_
				WCHAR* cliptxt=(WCHAR*)::GlobalLock(hCliptextW);
				size_t len=wcslen(cliptxt);
				if(len>0){_XLOG_
					if(strWOUT){_XLOG_
						delete[] strWOUT;
						strWOUT=0;
						dwOutLen=0;
					}
					LOGSTAMP
					dwOutLen=len+1;
					strWOUT = NewWCHAR(dwOutLen);
					wcscpy(strWOUT,cliptxt);
					bSkipRest=1;
				}
				GlobalUnlock(hCliptextW);
				LOGSTAMP
			}else{_XLOG_
				USES_CONVERSION;
				strWOUT = 0;//_wcsdup(A2W(Format("Big piece of text (%i bytes)",dwDataSize)));
				dwOutLen=0;
			}
		}
	}
	LOGSTAMP
	if(!bSkipRest && IsClipboardFormatAvailable(CF_TEXT)){_XLOG_
		// Получаем в тексте
		HANDLE hCliptext=::GetClipboardData(CF_TEXT);
		DWORD dwErr=GetLastError();
		if(hCliptext){_XLOG_
			DWORD dwDataSize=GlobalSize(hCliptext);
			if(MaxBmpSize()==0 || dwDataSize<MaxBmpSize()){_XLOG_
				char* cliptxt = (char*)GlobalLock(hCliptext); 
				size_t len=strlen(cliptxt);
				if(len>0){_XLOG_
					if(strWOUT){_XLOG_
						delete[] strWOUT;
						strWOUT=0;
					}
					dwOutLen=len+1;
					strWOUT = NewWCHAR(dwOutLen);
					MultiByteToWideChar(CP_ACP, 0, cliptxt, -1, strWOUT, len);
					bSkipRest=1;
				}
				GlobalUnlock(hCliptext);
			}else{_XLOG_
				USES_CONVERSION;
				strWOUT = 0;//_wcsdup(A2W(Format("Big piece of text (%i bytes)",dwDataSize)));
				dwOutLen=0;
			}
		}
	}
	LOGSTAMP
	if(!bSkipRest && bWithBitmaps && IsClipboardFormatAvailable(CF_DIB)){_XLOG_
		// Получаем в BMP
		HANDLE hClipbmp=::GetClipboardData(CF_DIB);
		if(hClipbmp){_XLOG_
			DWORD dwDataSize=GlobalSize(hClipbmp);
			DWORD dwMaxSize=MaxBmpSize();
			if(dwMaxSize==0){
				dwMaxSize=10000000;//10 мегов
			}
			if(dwDataSize>0 && dwDataSize<dwMaxSize){_XLOG_
				BYTE* cliptxt=0;
				{
					BYTE* cliptxtbmp = (BYTE*)GlobalLock(hClipbmp);
					if(cliptxtbmp){_XLOG_
						cliptxt=new BYTE[dwDataSize];
						if(cliptxt){
							memcpy(cliptxt,cliptxtbmp,dwDataSize);
						}
						GlobalUnlock(hClipbmp);
					}
					hClipbmp=0;
				}
				if(cliptxt && dwDataSize>sizeof(BITMAPINFO)){_XLOG_
					BITMAPINFO* pBMPDsc=(BITMAPINFO*)cliptxt;
					DWORD dwImageDataSize=0;
					CString sBMPDsc=Format("%lux%lu ",abs(pBMPDsc->bmiHeader.biWidth),abs(pBMPDsc->bmiHeader.biHeight));
					if(pBMPDsc->bmiHeader.biBitCount==0){_XLOG_
						sBMPDsc+="JPG/PNG";
					}else if(pBMPDsc->bmiHeader.biBitCount==1){_XLOG_
						dwImageDataSize=(pBMPDsc->bmiHeader.biHeight*pBMPDsc->bmiHeader.biWidth)/8;
						sBMPDsc+="Mono";
					}else if(pBMPDsc->bmiHeader.biBitCount==4){_XLOG_
						dwImageDataSize=(pBMPDsc->bmiHeader.biHeight*pBMPDsc->bmiHeader.biWidth)/2;
						sBMPDsc+=Format("16 %s",_l("Colors"));
					}else if(pBMPDsc->bmiHeader.biBitCount==8){_XLOG_
						dwImageDataSize=(pBMPDsc->bmiHeader.biHeight*pBMPDsc->bmiHeader.biWidth);
						sBMPDsc+=Format("256 %s",_l("Colors"));
					}else if(pBMPDsc->bmiHeader.biBitCount==16){_XLOG_
						dwImageDataSize=(pBMPDsc->bmiHeader.biHeight*pBMPDsc->bmiHeader.biWidth)*2;
						sBMPDsc+=Format("16 %s",_l("Bits"));
					}else if(pBMPDsc->bmiHeader.biBitCount==24){_XLOG_
						dwImageDataSize=(pBMPDsc->bmiHeader.biHeight*pBMPDsc->bmiHeader.biWidth)*3;
						sBMPDsc+=Format("24 %s",_l("Bits"));
					}else if(pBMPDsc->bmiHeader.biBitCount==32){_XLOG_
						dwImageDataSize=(pBMPDsc->bmiHeader.biHeight*pBMPDsc->bmiHeader.biWidth)*4;
						sBMPDsc+=Format("32 %s",_l("Bits"));
					}
					if(sBMPDsc!=""){_XLOG_
						sBMPDsc=CString(", ")+sBMPDsc;
					}
					DWORD dwHeaderLen=sizeof(BITMAPINFO);
					dwImageDataSize+=dwHeaderLen;
					TRACE2("\n\nCoping Bitmap!!! Size=%lu(%lu)\n\n", dwDataSize, dwImageDataSize);
					DWORD dwHashSize=dwDataSize;
					if(dwImageDataSize<=dwHashSize){_XLOG_
						dwHashSize=dwImageDataSize;
					}
					if(dwDataSize>0){_XLOG_
						CString sPacked=PackToString((const char*)cliptxt, dwDataSize);
						const char* szBeg=sPacked;
						int iAddon=dwHeaderLen*3+1;
						if(iAddon<sPacked.GetLength()){
							szBeg+=iAddon;
						}
						CString sHash=GetStringHash(szBeg);
						CString sPreview=_l("Bitmap")+sBMPDsc+BMP_SPECPREFIX+Format("[%lu,%s]",dwDataSize,sHash);
						sPreview+=sPacked;
						if(strWOUT){_XLOG_
							delete[] strWOUT;
							strWOUT=0;
							dwOutLen=0;
						}
						dwOutLen=sPreview.GetLength();
						DWORD dwMemoLen=dwOutLen/sizeof(WCHAR)+10;
						strWOUT = NewWCHAR(dwMemoLen);
						const char* szz=sPreview;
						memcpy((BYTE*)strWOUT,(const BYTE*)szz,dwOutLen);
						bSkipRest=1;
					}
				}
				if(cliptxt){_XLOG_
					delete[] cliptxt;
				}				
			}else{_XLOG_
				USES_CONVERSION;
				strWOUT = 0;//_wcsdup(A2W(Format("Big image (%i bytes)",dwDataSize)));
				dwOutLen=0;
			}
		}
	}
	if(dwLen){
		(*dwLen)=dwOutLen;
	}
	LOGSTAMP
	if(bAndOpen){
		::CloseClipboard();
	}
	LOGSTAMP
	return strWOUT;
}

BOOL GetOSVersion(CString &WinVer, DWORD* dwMajor, long* bNT) 
{ 
	// Кешируем результаты
	static CString _sWinVer="";
	static long _dwMajor=-1;
	static long _bNT=-1;
	static long _bVersionGathered=-1;
	if(_bVersionGathered!=-1){_XLOG_
		WinVer=_sWinVer;
		if(dwMajor){_XLOG_
			*dwMajor=_dwMajor;
		}
		if(bNT){_XLOG_
			*bNT=_bNT;
		}
		return TRUE;
	}
	//----------------------
	OSVERSIONINFOEX winfo; 
	
	ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
	winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	
	if (GetVersionEx((OSVERSIONINFO *)&winfo) == 0) 
	{ 
		//Get Windows version failed 
		return FALSE; 
	} 
	if (winfo.dwPlatformId == VER_PLATFORM_WIN32_NT) 
	{_XLOG_
		/*if (winfo.wProductType == VER_NT_WORKSTATION)//Workingstation version 
		{ 
			if (winfo.dwMajorVersion >= 5) 
			{ 
				if (winfo.dwMinorVersion == 1) 
				{ 
					WinVer = WXP; //Windows XP 
					
					if (winfo.wSuiteMask & VER_SUITE_PERSONAL) 
						WinVer += HED; //Windows XP Home Edtion 
					else 
						WinVer += PED; //Windows XP Professional Edtion 
					
				} 
				else 
					WinVer = W2K; //Windows 2000 Professtiona 
			} 
			else 
				WinVer = WNT4; //Windows NT 4.0 Workstation 
		} 
		else if (winfo.wProductType == VER_NT_SERVER)*/
		{ 
			if (winfo.dwMajorVersion >= 5) 
			{ 
				if (winfo.dwMinorVersion == 1) 
					WinVer = WXP; //Windows XP 
				else 
					if (winfo.dwMinorVersion == 2) 
						WinVer = WNE; //Windows .Net 
					else 
						WinVer = W2K; //Windows 2000 
					
					/*if(winfo.wSuiteMask & VER_SUITE_DATACENTER ) 
						WinVer += DCS; //DataCenter Server 
					else if (winfo.wSuiteMask & VER_SUITE_ENTERPRISE ) 
					{ 
						if (winfo.dwMajorVersion == 4 ) 
							WinVer += ADS; //Advanced Server 
						else 
							WinVer += ENS; //Enterprise Server 
					} 
					else if (winfo.wSuiteMask == VER_SUITE_BLADE ) 
						WinVer += WES; //Web Server 
					else 
						WinVer += SER; //Server */
			} 
			else 
			{ 
				WinVer = WNT4; //Window NT 4.0 
				WinVer += SER; //Server 
			} 
		} 
	}
	else 
	{_XLOG_
		if (winfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
		{ 
			if(winfo.dwMinorVersion < 10) 
				WinVer = W95; //Windows 95 
			else if (winfo.dwMinorVersion < 90) 
				WinVer = W98; //Windows 98 
			else 
				WinVer = WME; //Windows ME 
		} 
	} 
	if(dwMajor){_XLOG_
		*dwMajor=winfo.dwMajorVersion;
	}
	//---------
	_bVersionGathered=1;
	_sWinVer=WinVer;
	_dwMajor=winfo.dwMajorVersion;
	_bNT=(winfo.dwPlatformId==VER_PLATFORM_WIN32_NT);
	return GetOSVersion(WinVer, dwMajor, bNT);
}

#include <shlobj.h>
typedef BOOL (WINAPI *_SHGetSpecialFolderLocation)(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl);
BOOL MyGetSpecialFLoc(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl)
{_XLOG_
	BOOL bRes=FALSE;
	/*HINSTANCE hShell32=LoadLibrary("Shell32.dll");
	if(hShell32){_XLOG_
		_SHGetSpecialFolderLocation fp=(_SHGetSpecialFolderLocation)DBG_GETPROC(hShell32,"SHGetSpecialFolderLocationA");
		if(!fp){_XLOG_
			fp=(_SHGetSpecialFolderLocation)DBG_GETPROC(hShell32,"SHGetSpecialFolderLocation");
		}
		if(fp){_XLOG_
			LPITEMIDLIST pIdl = NULL;
			bRes=(*fp)(hwndOwner,nFolder,ppidl);
			FreeLibrary(hShell32);
		}
	}*/
	bRes=SHGetSpecialFolderLocation(hwndOwner,nFolder,ppidl);
	return bRes;
}

typedef HRESULT (WINAPI* _SHGetFolderPath)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
BOOL GetSpecialFolderPath(char* szOutPath, int nFolder)
{_XLOG_
	strcpy(szOutPath,"c:\\temp\\");
	/*CString sQuickRes="";
	switch(nFolder){_XLOG_
	case CSIDL_PERSONAL:
		sQuickRes="%USERPROFILE%";
		break;
	case CSIDL_APPDATA:
		sQuickRes="%USERPROFILE%\\Application Data\\";
		break;
	}
	if(sQuickRes!=""){_XLOG_
		CString sEta=sQuickRes;
		SubstEnvVariables(sQuickRes);
		strcpy(szOutPath,sQuickRes);
		if(sQuickRes!="" && sEta!=sQuickRes){_XLOG_
			return TRUE;
		}
	}*/

	//SHGetFolderPath implementation in Shell32.dll/Shfolder.dll
	/*
	HINSTANCE hShell32=LoadLibrary("Shell32.dll");
	if(hShell32){_XLOG_
		_SHGetFolderPath fp=(_SHGetFolderPath)DBG_GETPROC(hShell32,"SHGetFolderPathA");
		if(!fp){_XLOG_
			FreeLibrary(hShell32);
			hShell32=LoadLibrary("Shfolder.dll");
			if(hShell32){_XLOG_
				fp=(_SHGetFolderPath)DBG_GETPROC(hShell32,"SHGetFolderPathA");
			}
		}
		if(fp){_XLOG_
			char szPath[MAX_PATH]="";
			(*fp)(0,nFolder,0,0,szPath);//SHGFP_TYPE_CURRENT
			strcpy(szOutPath,szPath);
		}
		FreeLibrary(hShell32);
	}
	*/

	char szPath[MAX_PATH]="";
	SHGetSpecialFolderPath(0,szPath,nFolder,0);
	strcpy(szOutPath,szPath);
	return TRUE;
}

void AddMenuString(CMenu* pMenu, int iID, const char* szString, CBitmap* pBmp, BOOL bCheck, CBitmap* pBmpC, BOOL bGray, BOOL bDisabled, BOOL bBreakMenu)
{_XLOG_
	DWORD dwFlags=MF_STRING|(bCheck?MF_CHECKED:MF_UNCHECKED)|(bGray?MF_GRAYED:0)|(bDisabled?MF_DISABLED:0)|(bBreakMenu?MF_MENUBARBREAK:0);
	pMenu->AppendMenu(dwFlags, iID, szString);
	if(pBmp!=NULL || pBmpC!=NULL){_XLOG_
		pMenu->SetMenuItemBitmaps(iID,MF_BYCOMMAND,pBmp,pBmpC);
	}
}

void AddMenuString(CMenu* pMenu, int iID, const char* szString, DWORD dwFlags, CBitmap* pBmp, CBitmap* pBmpC)
{_XLOG_
	pMenu->AppendMenu(dwFlags, iID, szString);
	if(pBmp!=NULL || pBmpC!=NULL){_XLOG_
		pMenu->SetMenuItemBitmaps(iID,MF_BYCOMMAND,pBmp,pBmpC);
	}
}

BOOL IsHelpEnabled()
{
	static BOOL b=-1;
	if(b==-1){
		b=0;
		CString sFile=Format("%s%s_l%i.chm", GetApplicationDir(), GetApplicationName(), 0);
		if(isFileExist(sFile)){
			b=1;
		}
	}
	return b;
}

void ShowHelp(const char* szTopicNameRaw)
{_XLOG_
	CString szTopicName=szTopicNameRaw;
	szTopicName.MakeLower();
	CString sFile=Format("%s%s_l%i.chm", GetApplicationDir(), GetApplicationName(), GetApplicationLang());
	if(!isFileExist(sFile)){_XLOG_
		sFile=Format("%s%s_l0.chm", GetApplicationDir(), GetApplicationName());
		if(!isFileExist(sFile)){_XLOG_
			sFile=Format("%s%s.chm", GetApplicationDir(), GetApplicationName());
		}
		if(!isFileExist(sFile)){_XLOG_
			::ShellExecute(GetAppWnd()?GetAppWnd()->GetSafeHwnd():NULL,"open","help.html",0,GetApplicationDir(),SW_SHOWNORMAL);
			return;
		}
	}
	::ShellExecute(GetAppWnd()?GetAppWnd()->GetSafeHwnd():NULL,NULL,"hh.exe",Format("%s::/%s.shtml",sFile,szTopicName),GetApplicationDir(),SW_SHOWNORMAL);
}

int AddMenuSubmenu(CMenu* pMenuTo, CMenu* pMenuFrom, const char* szString, CBitmap* pBmp, DWORD dwID, int iIndexTo)
{_XLOG_
	int iRes=-1;
	if(!pMenuFrom || !pMenuFrom->GetSafeHmenu() || pMenuFrom->GetMenuItemCount()==0){_XLOG_
		return iRes;
	}
	if(iIndexTo==-1){_XLOG_
		pMenuTo->AppendMenu(MF_STRING|MF_POPUP, (UINT)pMenuFrom->GetSafeHmenu(), szString);
		iRes=pMenuTo->GetMenuItemCount()-1;
	}else{_XLOG_
		pMenuTo->DeleteMenu(iIndexTo,MF_BYPOSITION);
		pMenuTo->InsertMenu(iIndexTo, MF_BYPOSITION|MF_STRING|MF_POPUP, (UINT)pMenuFrom->GetSafeHmenu(), szString);
		iRes=iIndexTo;
	}
	if(pBmp!=NULL){_XLOG_
		pMenuTo->SetMenuItemBitmaps(iRes,MF_BYPOSITION,pBmp,NULL);
	}
	if(dwID!=NULL){_XLOG_
		MENUITEMINFO cmi;
		memset(&cmi,0,sizeof(cmi));
		cmi.cbSize=sizeof(cmi);
		cmi.wID=dwID;
		cmi.fMask=MIIM_ID;
		SetMenuItemInfo(pMenuTo->GetSafeHmenu(), iRes,TRUE,&cmi);
	}
	pMenuFrom->Detach();
	return iRes;
}

CString getBrowserExePath()
{_XLOG_
	static CString sDefaultBrowser=GetExtensionAssociatedAction("html","open");
	if(sDefaultBrowser==":("){_XLOG_
		return "";
	}
	if(sDefaultBrowser==""){_XLOG_
		sDefaultBrowser=":(";
		char szTempFile[MAX_PATH]="";
		if(GetTempFileName(GetUserFolder(),"DBR",time(NULL),szTempFile)!=0){_XLOG_
			CString sHtmlFile=szTempFile;
			sHtmlFile=sHtmlFile.SpanExcluding(".")+".htm";
			SaveFile(sHtmlFile,"<HTML></HTML>");
			char szBrFile[MAX_PATH]="";
			if(int(::FindExecutable(sHtmlFile,sHtmlFile,szBrFile))>32){_XLOG_
				sDefaultBrowser=szBrFile;
			}
			DeleteFile(sHtmlFile);
		}
	}
	return sDefaultBrowser;
}

CString getFileFullName(const char* szFName, BOOL bDir)
{_XLOG_
	CString sFileToOpen=szFName;
	char szFolderBuffer[MAX_PATH+1]="";
	if(strlen(szFName)>3 && (szFName[0]!='\\' && !(szFName[1]==':' && szFName[2]=='\\'))){_XLOG_
		// Относительный путь
		sFileToOpen=GetUserFolder()+szFName;
	}else{_XLOG_
		// Абсолютный путь
		if(szFName[0]=='\\'){_XLOG_
			sFileToOpen=GetApplicationDir();
			sFileToOpen+="UserData\\";
			sFileToOpen+=szFName;
		}else{_XLOG_
			sFileToOpen=szFName;
		}
	}
	if(bDir==TRUE){_XLOG_
		if(sFileToOpen.Right(1)!="\\"){_XLOG_
			// Смотрим, кончается ли на имя файла
			int iDotPos=sFileToOpen.ReverseFind('.');
			int iSplashPos=sFileToOpen.ReverseFind('\\');
			if(iDotPos!=-1 && iDotPos>iSplashPos){_XLOG_
				char szDisk[MAX_PATH]="C://", szPath[MAX_PATH]="TEMP//";
				_splitpath(sFileToOpen, szDisk, szPath, NULL, NULL);
				sFileToOpen=szDisk;
				sFileToOpen+=szPath;
			}else{_XLOG_
				sFileToOpen+="\\";
			}
		}
	}
	return sFileToOpen;
}

CString GetExtensionAssociatedAction(const char* szExt, const char* szAction)
{_XLOG_
	CString sAction=szAction;
	CString sExtRaw=szExt;
	if(ExtractInlineScript(sExtRaw)!=""){_XLOG_
		return GetAppExe();
	}
	sAction.MakeLower();
	CString sExt="";
	if(szExt && szExt[0]!='.' && szExt[0]!=':'){//для случая exe но не ::{...}
		sExt=".";
	}
	sExt+=sExtRaw;
	if(sExt.Find("::{")==0){_XLOG_
		sExt.TrimLeft(":");
		sExt="CLSID\\"+sExt;
		char szTemp[MAX_PATH]="";
		DWORD lSize = MAX_PATH,dwType=0;
		{_XLOG_
			CRegKey key;
			key.Open(HKEY_CLASSES_ROOT, sExt);
			dwType=0;
			szTemp[0]=0;
			lSize = MAX_PATH;
			if(RegQueryValueEx(key.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS && isFileIExist(szTemp)){_XLOG_
				return szTemp;
			}
		}
		{_XLOG_
			CRegKey key2;
			key2.Open(HKEY_CLASSES_ROOT, sExt+"\\LocalServer32");
			dwType=0;
			szTemp[0]=0;
			lSize = MAX_PATH;
			if(RegQueryValueEx(key2.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS && isFileIExist(szTemp)){_XLOG_
				return szTemp;
			}
		}
		{_XLOG_
			CRegKey key3;
			key3.Open(HKEY_CLASSES_ROOT, sExt+"\\LocalServer");
			dwType=0;
			szTemp[0]=0;
			lSize = MAX_PATH;
			if(RegQueryValueEx(key3.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS && isFileIExist(szTemp)){_XLOG_
				return szTemp;
			}
		}
		{_XLOG_
			CRegKey key4;
			key4.Open(HKEY_CLASSES_ROOT, sExt+"\\InprocServer32");
			dwType=0;
			szTemp[0]=0;
			lSize = MAX_PATH;
			if(RegQueryValueEx(key4.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS && isFileIExist(szTemp)){_XLOG_
				return szTemp;
			}
		}
		{_XLOG_
			CRegKey key5;
			key5.Open(HKEY_CLASSES_ROOT, sExt+"\\DefaultIcon");
			dwType=0;
			szTemp[0]=0;
			lSize = MAX_PATH;
			if(RegQueryValueEx(key5.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS && isFileIExist(szTemp)){_XLOG_
				return szTemp;
			}
		}
	}
	sExt.MakeLower();
	if(sExt.Find("http://")==0){_XLOG_
		return getBrowserExePath();
	}
	if(!szAction || szAction[0]==0){_XLOG_
		CString sRes=GetExtensionAssociatedAction(szExt,"open");
		if(sRes==""){_XLOG_
			sRes=GetExtensionAssociatedAction(szExt,"edit");
		}
		return sRes;
	}
	CRegKey key;
	key.Open(HKEY_CLASSES_ROOT, sExt);
	char szTemp[MAX_PATH]="";
	DWORD lSize = MAX_PATH,dwType=0;
	if(RegQueryValueEx(key.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){_XLOG_
		CString sFileTypeName=szTemp;
		CRegKey key2;
		dwType=0;
		szTemp[0]=0;
		lSize = MAX_PATH;
		key2.Open(HKEY_CLASSES_ROOT, sFileTypeName+"\\shell\\"+sAction+"\\Command");
		if(RegQueryValueEx(key2.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){_XLOG_
			CString sRes=szTemp;
			if(sRes!=""){_XLOG_
				int iKPos=sRes.Find('\"',1);
				if(iKPos!=-1){_XLOG_
					sRes=sRes.Left(iKPos);
					if(sRes.Left(1)=="\""){_XLOG_
						sRes=sRes.Mid(1);
					}
				}else{_XLOG_
					sRes=sRes.SpanExcluding(" ");
				}
				sRes.TrimLeft();
				sRes.TrimRight();
			}
			CString sProhibitedHandlers=sRes;
			sProhibitedHandlers.MakeLower();
			if(sProhibitedHandlers=="rundll32.exe"){_XLOG_
				sRes="";
			}
			return sRes;
		}
	}
	return "";
}


CString regGetBuyURL(CString publisher, CString appName, CString appVer ) 
{_XLOG_
	// FromIniFile
	CString sIniFileInfo;
	ReadFile(CString(GetApplicationDir())+"install.ini",sIniFileInfo);
	CString CustomBuyUrl=CDataXMLSaver::GetInstringPart("CustomBuyUrl={","}",sIniFileInfo);
	if(CustomBuyUrl!=""){
		return CustomBuyUrl;
	}
	HKEY hKey = NULL;
	CString buyURL = "";

	// form the registry key path
	CString keyPath = "SOFTWARE\\Digital River\\SoftwarePassport\\" + publisher + "\\" + appName + "\\" + appVer;

	// read the "BuyURL" value from HKEY_LOCAL_MACHINE branch first
	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath,0,KEY_READ, &hKey);
	if (lRet != ERROR_SUCCESS) {_XLOG_
		// fail to read from HKEY_LOCAL_MACHINE branch, try HKEY_CURRENT_USER
		lRet = RegOpenKeyEx(HKEY_CURRENT_USER, keyPath,0,KEY_READ, &hKey);
	};

	if (lRet == ERROR_SUCCESS) {_XLOG_
        LPSTR   lpBuyURL   = buyURL.GetBuffer( 1024 );
        DWORD   dwSize   = 1024;

		// Read the "BuyURL" value from the registry key.
        RegQueryValueEx(hKey, "BuyURL",0, NULL, (LPBYTE)lpBuyURL, &dwSize);
		buyURL.ReleaseBuffer();

        RegCloseKey( hKey);
	}

	return buyURL;
}

CString UnifyString(CString sKey)
{_XLOG_
	CString sRes;
	sKey.MakeUpper();
	for(int i=0;i<sKey.GetLength();i++){_XLOG_
		unsigned iChar=(unsigned)sKey[i];
		if((iChar>='A' && iChar<='Z') || (iChar>='0' && iChar<='9') || iChar>=127){//|| sKey>127
			sRes+=sKey[i];
		}
	}
	return sRes;
}

/*
Пункт Дидо (французский пункт, пункт цицеро), который был принят у нас. 
Будем записывать его как 1 пт.
Пункт пика, или американский пункт. Будем записывать его 1 pt.
Большой пункт, используемый PostScript'ом. Он несколько округлен для
удобства вычислений. Будем записывать его как 1 bp.


1 пт = 0,3759 мм.
1 мм = 2,66 пт
1 cc (цицеро) = 12 пт.
1 квадрат = 48 пт = 4 цицеро
1 pt = 1/72.27 in
1 pc (пика) = 12 pt.
1 in = 2.54 см.
1167 пт = 1238 pt.
1 bp = 1/72 in
*/
CString GetFontTextDsc(CFont* pFont)
{_XLOG_
	CString sOut=_l("Normal");
	if(pFont==NULL){_XLOG_
		return sOut;
	}
	LOGFONT font;
	pFont->GetLogFont(&font);
	DWORD lfHeight=font.lfHeight;
	CDesktopDC dcScreen;
	if(font.lfHeight<0){_XLOG_
		//---------------------------
		lfHeight=-font.lfHeight;
		CDC dc;
		dc.CreateCompatibleDC(&dcScreen);
		dc.SetMapMode(MM_TEXT);
		lfHeight = MulDiv(lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	}
	//+++++++++++++++++++++++++++
	sOut=Format("%s, %ipt",font.lfFaceName,lfHeight);
	if(font.lfWeight>400){_XLOG_
		sOut+=", "+_l("Bold");
	}
	if(font.lfItalic>0){_XLOG_
		sOut+=", "+_l("Italic");
	}
	if(font.lfUnderline>0){_XLOG_
		sOut+=", "+_l("Underline");
	}
	if(font.lfStrikeOut>0){_XLOG_
		sOut+=", "+_l("Strike");
	}
	return sOut;
}

CString GetFontTextDsc(CString sFont)
{_XLOG_
	CString sOut=_l("Normal");
	if(sFont==""){_XLOG_
		return sOut;
	}
	CString sSchValue;
	CDataXMLSaver StyleData(sFont);
	long lfHeight=0;
	StyleData.getValue("size",lfHeight,-27);
	long dwWeight=0;
	StyleData.getValue("weight",dwWeight,400);
	CString sFaceName;
	StyleData.getValue("fontname",sFaceName);
	CString sEffects;
	StyleData.getValue("effects",sEffects);
	CDesktopDC dcScreen;
	if(lfHeight<0){_XLOG_
		//---------------------------
		lfHeight=-lfHeight;
		CDC dc;
		dc.CreateCompatibleDC(&dcScreen);
		dc.SetMapMode(MM_TEXT);
		lfHeight = MulDiv(lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
	}
	//+++++++++++++++++++++++++++
	sOut=Format("%s, %ipt",sFaceName,lfHeight);
	if(dwWeight>400){_XLOG_
		sOut+=", "+_l("Bold");
	}
	if(sEffects.Find("i")!=-1){_XLOG_
		sOut+=", "+_l("Italic");
	}
	if(sEffects.Find("u")!=-1){_XLOG_
		sOut+=", "+_l("Underline");
	}
	if(sEffects.Find("s")!=-1){_XLOG_
		sOut+=", "+_l("Strike");
	}
	return sOut;
}

CIniFileParser::~CIniFileParser()
{_XLOG_
	ClearCashes();
}

void CIniFileParser::ClearCashes()
{_XLOG_
	POSITION pos = aFiles.GetStartPosition();
	while( pos != NULL )
	{_XLOG_
		CObject* pFile;
		CString string;
		// Gets key (string) and value (pPerson)
		aFiles.GetNextAssoc( pos, string, pFile );
		CStringArray* pFileObj =(CStringArray*)pFile;
		delete pFileObj;
	}
	// RemoveAll deletes the keys
	aFiles.RemoveAll();
}

#ifdef FNF_SUPPORT
#include "FileInfo.h"
#endif
CStringArray* CIniFileParser::loadFileString(const char* szRawFileName)
{_XLOG_
	if(!szRawFileName){_XLOG_
		return NULL;
	}
	char szFile[MAX_PATH]="";
	strcpy(szFile,szRawFileName);
	CObject* pFileStrings=NULL;
	CStringArray* pFileStringsMap=NULL;
	if(!aFiles.Lookup(szFile,pFileStrings)){_XLOG_
		CString sContent;
		if(strchr(szFile,'*')==NULL){_XLOG_
			if(ReadFile(szFile,sContent,TRUE)==FALSE){_XLOG_
				return NULL;
			}
		}
#ifdef FNF_SUPPORT
		else{_XLOG_
			CString sContentTmp;
			CFileInfoArray dir;
			char szD[MAX_PATH]="",szP[MAX_PATH]="";
			char szF[MAX_PATH]="",szE[MAX_PATH]="";
			_splitpath(szFile,szD,szP,szF,szE);
			CString sFirstImagePath=szD;
			sFirstImagePath+=szP;
			strcat(szF,szE);
			dir.AddDir(sFirstImagePath,szF,TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
			for (int i=0;i<dir.GetSize();i++){_XLOG_
				CString sFile=dir[i].GetFilePath();
				ReadFile(sFile,sContentTmp,TRUE);
				sContent+=sContentTmp;
				sContent+="\r\n";
			}
		}
#endif
		CString sDelims=CDataXMLSaver::GetInstringPart(";#Delimiters:(",")",sContent);
		sDelims.MakeLower();
		CString sLinesDelims="\n";
		if(sDelims.Find("new line")!=-1){_XLOG_
			sLinesDelims="\n\n";
			if(sContent.Find("\r\n")!=-1){_XLOG_
				sLinesDelims="\r\n\r\n";
			}
		}
		sContent=sLinesDelims+sContent+sLinesDelims;
		pFileStringsMap=new CStringArray;
		int iFrom=0;
		int iFileLen=strlen(sContent);
		while(iFrom>=0){_XLOG_
			CString sLine=CDataXMLSaver::GetInstringPart(sLinesDelims,sLinesDelims,sContent,iFrom);
			if(sLine!=""){_XLOG_
				if(sLine[0]!=';'){_XLOG_
					sLine.TrimLeft();
					sLine.TrimRight();
					pFileStringsMap->Add(sLine);
				}
			}
			if(iFrom<=0 || iFrom>=iFileLen){_XLOG_
				break;
			}
			if(iFrom>0){_XLOG_
				iFrom-=sLinesDelims.GetLength();// Чтобы отлавливались одиночные переводы строки между строками
			}
		}
		aFiles.SetAt(szFile,(CObject*)pFileStringsMap);
	}else{_XLOG_
		pFileStringsMap=(CStringArray*)pFileStrings;
	}
	if(pFileStringsMap==NULL || pFileStringsMap->GetSize()==0){_XLOG_
		return NULL;
	}
	return pFileStringsMap;

}

BOOL CIniFileParser::getAllValues(const char* szRawFileName,const char* szKey, CStringSet& aStrings, BOOL bStripKey)
{_XLOG_
	CStringArray* pFileStringsMap=loadFileString(szRawFileName);
	if(!pFileStringsMap){_XLOG_
		return FALSE;
	}
	int iFileStringsSize=pFileStringsMap->GetSize();
	CString* pStringsFromFile=pFileStringsMap->GetData();
	if(!pStringsFromFile){_XLOG_
		return FALSE;
	}
	aStrings.RemoveAll();
	int iLen=0;
	if(szKey){_XLOG_
		iLen=strlen(szKey);
	}
	for(int i=0;i<iFileStringsSize;i++){_XLOG_
		const char* sLine=pStringsFromFile[i];
		const char* szNewString=(bStripKey && iLen>0)?(sLine+iLen):sLine;
		if(iLen==0){_XLOG_
			aStrings.Add(szNewString);
			continue;
		}
		if(strchr(szKey,'*')==NULL){_XLOG_
			if(memicmp(sLine,szKey,iLen)==0){_XLOG_
				aStrings.Add(szNewString);
			}
		}else{_XLOG_
			if(PatternMatch(sLine,CString(szKey)+"*")){_XLOG_
				aStrings.Add(szNewString);
			}
		}
	}
	return TRUE;
}

CString CIniFileParser::getByKey(const char* szFile,const char* szKey)
{_XLOG_
	if(!szFile || !szKey){_XLOG_
		return "";
	}
	CString sContent;
	CString sContentLow;
	aFilesCont.Lookup(CString(szFile)+"Low",sContentLow);
	BOOL bLookupOriginal=aFilesCont.Lookup(szFile,sContent);
	if(!bLookupOriginal){_XLOG_
		if(strchr(szFile,'*')==0){_XLOG_
			ReadFile(szFile,sContent);
			sContent="\n"+sContent+"\n";
		}
#ifdef FNF_SUPPORT		
		else{_XLOG_
			CString sContentTmp;
			CFileInfoArray dir;
			char szD[MAX_PATH]="",szP[MAX_PATH]="";
			char szF[MAX_PATH]="",szE[MAX_PATH]="";
			_splitpath(szFile,szD,szP,szF,szE);
			CString sFirstImagePath=szD;
			sFirstImagePath+=szP;
			strcat(szF,szE);
			dir.AddDir(sFirstImagePath,szF,TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
			for (int i=0;i<dir.GetSize();i++){_XLOG_
				CString sFile=dir[i].GetFilePath();
				ReadFile(sFile,sContentTmp,TRUE);
				sContent+="\n";
				sContent+=sContentTmp;
				sContent+="\n";
			}
		}
#endif		
		sContent.Replace("\r","\n");
		aFilesCont.SetAt(szFile,sContent);
		sContentLow=sContent;
		sContentLow.MakeLower();
		aFilesCont.SetAt(CString(szFile)+"Low",sContentLow);
	}
	CString sKey="\n";
	sKey+=szKey;
	sKey.MakeLower();
	int iPos0=sContentLow.Find(sKey);
	if(iPos0>=0){_XLOG_
		iPos0+=strlen(sKey);
		int iPos2=sContentLow.Find("\n",iPos0);
		if(iPos2>=0){_XLOG_
			CString sRes=sContent.Mid(iPos0,iPos2-iPos0);
			return sRes;
		}
	}
	return "";
}

const char* CIniFileParser::getValueRandom(const char* szRawFileName,const char* szKey, BOOL bStripKey)
{_XLOG_
	if(!szRawFileName || !szKey){_XLOG_
		return "";
	}
	// Пробегаемся по строчкам
	CStringSet aStrings;
	if(!getAllValues(szRawFileName, szKey, aStrings, bStripKey)){_XLOG_
		return "";
	}
	int iLen=aStrings.GetSize();
	if(iLen==0){_XLOG_
		return "";
	}
	if(iLen==1){_XLOG_
		return aStrings[0];
	}
	int iLineIndex=rnd(0,iLen-1);
	return aStrings[iLineIndex];
}

CIniFileParser& objFileParser()
{_XLOG_
	static CIniFileParser oFileParser;
	return oFileParser;
}


CMapStringToString& GetIniFileCash()
{_XLOG_
	static CMapStringToString objIniFile;
	static BOOL bHashSizeSet=0;
	if(bHashSizeSet==0){_XLOG_
		objIniFile.InitHashTable(101);
		bHashSizeSet=1;
	}
	return objIniFile;
}

void _clearIni()
{_XLOG_
	GetIniFileCash().RemoveAll();
	objFileParser().ClearCashes();
	char szLastFile[MAX_PATH]="";
	sprintf(szLastFile,"%sconfig.ini",GetApplicationDir());
	DeleteFile(szLastFile);
	return;
}

CString _getIni(const char* szText,const char* szDef)
{_XLOG_
	if(szText==NULL || *szText==0){_XLOG_
		return "";
	}
	CString sKey=szText;
	sKey+="=";
	CString sCashKey,sValue;
	// Смотрим в кэш
	sCashKey=sKey;
	if(GetIniFileCash().Lookup(sCashKey,sValue)){_XLOG_
		return sValue;
	}
	char szLastFile[MAX_PATH]="";
	sprintf(szLastFile,"%sconfig.ini",GetApplicationDir());
	sValue=objFileParser().getValueRandom(szLastFile,sKey,TRUE);
	if(((const char*)sValue)[0]==0){_XLOG_
		CString sSetValue=szDef;
		if(!szDef){_XLOG_
			sSetValue=szText;
		}
		sValue=sSetValue;
		FILE* m_logFile=getFile(szLastFile,"a+");
		if(m_logFile!=NULL){_XLOG_
			fprintf(m_logFile,"%s%s\n",sKey,sSetValue);
			fclose(m_logFile);
		}
	}
	GetIniFileCash().SetAt(sCashKey,sValue);
	return sValue;
}

/* Простые числа до 700
2 	3 	5 	7 	11 	13 	17 	19 	23 	29
31 	37 	41 	43 	47 	53 	59 	61 	67 	71
73 	79 	83 	89 	97 	101 	103 	107 	109 	113
127 	131 	137 	139 	149 	151 	157 	163 	167 	173
179 	181 	191 	193 	197 	199
211 223 227 229 233 239 241 251 257 263 269 271 277 281 283 293 
307 311 313 317 331 337 347 349 353 359 367 373 379 383 389 397 
401 409 419 421 431 433 439 443 449 457 461 463 467 479 487 491 499 
503 509 521 523 541 547 557 563 569 571 577 587 593 599 
601 607 613 617 619 631 641 643 647 653 659 661 673 677 683 691
*/
CMapStringToString& GetLangTermCash()
{_XLOG_
	static CMapStringToString objLanguageTermins;
	static BOOL bHashSizeSet=0;
	if(bHashSizeSet==0){_XLOG_
		objLanguageTermins.InitHashTable(101);
		bHashSizeSet=1;
	}
	return objLanguageTermins;
}

bool PatternMatchIC(const char* s, const char* mask)
{_XLOG_
	CString sS=s;
	sS.MakeLower();
	CString sM=mask;
	sM.MakeLower();
	return PatternMatch(sS,sM);
}

bool PatternMatch(const char* s, const char* mask)
{_XLOG_
	const   char* cp=0;
	const   char* mp=0;
	for (; *s&&*mask!='*'; mask++,s++){_XLOG_
		if (*mask!=*s && *mask!='?'){_XLOG_
			return 0;
		}
	}
	for (;;) {_XLOG_
		if (!*s){_XLOG_
			while (*mask=='*'){_XLOG_
				mask++;
			}
			return !*mask;
		}
		if (*mask=='*'){ 
			if (!*++mask){_XLOG_
				return 1;
			}
			mp=mask;
			cp=s+1;
			continue;
		}
		if (*mask==*s||*mask=='?'){_XLOG_
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
{_XLOG_
	CString sList=mask;
	if(sList==""){_XLOG_
		return true;
	}
	int iC=sList.Replace(szDelimiter/*";"*/,"\t");
	if(iC==0 && sList.Find("*")==-1){_XLOG_
		sList=sList+"*";
	}
	CStringArray sAppList;
	ConvertComboDataToArray(sList,sAppList);
	for(int i=0;i<sAppList.GetSize();i++){_XLOG_
		if(PatternMatch(s,sAppList[i])){_XLOG_
			return true;
		}
	}
	return false;
}

int ScanAndReplace(CString& sBuffer, const char* szWrongChars, char szOkChar)
{_XLOG_
	char* szBuffer=sBuffer.GetBuffer(1);
	int iRes=ScanAndReplace(szBuffer, szWrongChars, szOkChar);
	sBuffer.ReleaseBuffer();
	return iRes;
}

int ScanAndReplace(char* szBuffer, const char* szWrongChars, char szOkChar)
{_XLOG_
	int iRes=0;
	char* szPos=szBuffer;
	while(*szPos){_XLOG_
		if(strchr(szWrongChars,*szPos)!=NULL){_XLOG_
			*szPos=szOkChar;
			iRes++;
		}
		szPos++;
	}
	return iRes;
}

void CapitalizeAndShrink(CString& sStr,const char* szDelim)
{_XLOG_
	CString sRes="",sTerm;
	while(sStr!=""){_XLOG_
		sTerm=sStr.SpanExcluding(szDelim);
		sStr=sStr.Mid(strlen(sTerm));
		sStr.TrimLeft(szDelim);
		if(sTerm!=""){_XLOG_
			sTerm.MakeLower();
			char szSmalBuf[2]=" ";
			szSmalBuf[0]=sTerm[0];
			strupr(szSmalBuf);
			sTerm.SetAt(0,szSmalBuf[0]);
			sRes+=sTerm;
		}
	}
	sStr=sRes;
}

CRect GetDesktopRect()
{_XLOG_
	CRect rt;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);
	return rt;
}

// Обертка, которую можно вызывать из разных потоков
CWnd* GetAppWnd()
{_XLOG_
	static CWnd* pMainWnd=NULL;
	if(pMainWnd==NULL){_XLOG_
		CWinApp* pApp=AfxGetApp();
		if(pApp){_XLOG_
			pMainWnd=pApp->m_pMainWnd;
		}else{_XLOG_
			pMainWnd=AfxWPGetMainWnd();
		}
	}
	return pMainWnd;
}


BOOL GetCommandLineParameter(CString sName, CString& sValue, BOOL bAllowExpandFromFile)
{_XLOG_
	sValue="";
	CString sCM=AppCommandLine()+" ";
	CString sCML=sCM,sPML=sName;
	sCML.MakeLower();
	sPML.MakeLower();
	int iPos=FindSubstrWithVolatilePP(sCML,sPML,"-\\/#$@","= ");
	if(iPos==-1){_XLOG_
		return 0;
	}
	CString sRawParamValue=sCM.Mid(iPos+strlen(sName)+1);
	if(sRawParamValue==""){_XLOG_
		return 1;
	}
	if(sRawParamValue[0]=='='){_XLOG_
		sRawParamValue=sRawParamValue.Mid(1);
	}
	if(sRawParamValue==""){_XLOG_
		return 1;
	}
	if(sRawParamValue[0]=='\"'){_XLOG_
		sValue=sRawParamValue.Mid(1).SpanExcluding("\"");
	}else{_XLOG_
		sValue=sRawParamValue.SpanExcluding(" ");
	}
	if(bAllowExpandFromFile && sValue!="" && sValue[0]=='@'){_XLOG_
		CString sContent;
		CString sFile=sValue.Mid(1);
		if(sFile.Find("\\")==-1){_XLOG_
			sFile=CString(GetApplicationDir())+sFile;
		}
		ReadFile(sFile,sContent);
		sValue=sContent;
	}
	return 1;
}


CString& AppCommandLine()
{_XLOG_
	static CString s="";
	static BOOL bInit=0;
	if(bInit==0){_XLOG_
		bInit=1;
		s=GetCommandLine();
		// Дописываем реестр?
		HKEY hKey = NULL;
		// form the registry key path
		CString sAddCLine;
		CString keyPath = "SOFTWARE\\WiredPlane\\";
		keyPath+=GetApplicationName();
		LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, keyPath,0,KEY_READ, &hKey);
		if (lRet != ERROR_SUCCESS){_XLOG_
			lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath,0,KEY_READ, &hKey);
		};
		if (lRet == ERROR_SUCCESS) {_XLOG_
			LPSTR   lpCLine   = sAddCLine.GetBuffer( 1024 );
			DWORD   dwSize   = 1023;
			// Read the "BuyURL" value from the registry key.
			RegQueryValueEx(hKey, "CommandLine",0, NULL, (LPBYTE)lpCLine, &dwSize);
			sAddCLine.ReleaseBuffer();
			RegCloseKey( hKey);
			s+=" ";
			s+=lpCLine;
		}
	}
	return s;
}

BOOL GetFileCreationDate(CString sStartDir, DWORD& dwDay,DWORD& dwMon,DWORD& dwYear)
{_XLOG_
	HANDLE hFile = ::CreateFile(sStartDir, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile==INVALID_HANDLE_VALUE ){_XLOG_
		return FALSE;
	}
	FILETIME CreationTime;
	memset(&CreationTime,0,sizeof(CreationTime));
	if(!GetFileTime(hFile,NULL,NULL,&CreationTime)){_XLOG_
		return FALSE;
	}
	CloseHandle(hFile);
	SYSTEMTIME SystemTime;
	memset(&SystemTime,0,sizeof(SystemTime));
	if(!FileTimeToSystemTime(&CreationTime, &SystemTime)){_XLOG_
		return FALSE;
	}
	dwYear=SystemTime.wYear;
	dwMon=SystemTime.wMonth;
	dwDay=SystemTime.wDay;
	return TRUE; 
}

void SetTaskbarButton(HWND hWnd)
{_XLOG_
	BOOL bOk=0;
	/*CoInitialize(0);
	HRESULT hres; 
	ITaskbarList* psl;
	hres = CoCreateInstance(CLSID_TaskbarList , NULL, CLSCTX_INPROC_SERVER, IID_TaskbarList , (LPVOID*)&psl);
	if (SUCCEEDED(hres)){_XLOG_
		psl->HrInit();
		hres=psl->AddTab(hWnd);
		if (SUCCEEDED(hres)){_XLOG_
			bOk=1;
		}
		psl->Release();
	}*/
	if(bOk==0){_XLOG_
		::SetWindowLong(hWnd,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hWnd,GWL_EXSTYLE));
	}
}

#define MAX_LEN_ENVSUBST	10000
void SubstMyVariables(CString& sText,BOOL bJScriptSafe)
{_XLOG_
	if(sText.Find("%")==-1){_XLOG_
		return;
	}
	// Сначала смотрим куки...
	int iFrom=0;
	while(iFrom>=0){_XLOG_
		CString sLine=CDataXMLSaver::GetInstringPart("%COOKIE:","%",sText,iFrom);
		sText.Replace(Format("%%COOKIE:%s%%",sLine),getCookie(sLine));
	}
	if(sText.Find("%CLIP")!=-1 || sText.Find("%E_CLIP")!=-1 || sText.Find("%URL_CLIP")!=-1){_XLOG_
		WCHAR* wszClip=GetClipboardText();
		CString sClip;
		if(wszClip){_XLOG_
			sClip=wszClip;
		}
		sText.Replace("%CLIP",sClip);
		sText.Replace("%E_CLIP",EscapeString(sClip));
		sText.Replace("%URL_CLIP",EscapeStringUTF8(wszClip));
		if(wszClip){_XLOG_
			delete[] wszClip;
		}
	}
	BOOL bNeedAsk=(sText.Find("%ASK")!=-1) || (sText.Find("%E_ASK")!=-1);
	if(bNeedAsk){_XLOG_
		if(sText.Find("%ASKFILE")!=-1 || sText.Find("%E_ASKFILE")!=-1){_XLOG_
			static CString sLastFile="c:\\";
			CString sFile="";
			CFakeWindow fakeWnd(_l("Browse for file"));
			CFileDialog dlg(TRUE, NULL, sLastFile, OFN_NODEREFERENCELINKS, "All files (*.*)|*.*||", fakeWnd.GetWnd());
			if(dlg.DoModal()==IDOK){_XLOG_
				sLastFile=dlg.GetPathName();
				sFile=sLastFile;
				if(bJScriptSafe){_XLOG_
					MakeJScriptSafe(sFile);
				}
			}
			sText.Replace("%ASKFILE",sFile);
			sText.Replace("%E_ASKFILE",EscapeString(sFile));
		}
		if(sText.Find("%ASKFOLDER")!=-1 || sText.Find("%E_ASKFOLDER")!=-1){_XLOG_
			static CString sLastFolder="";
			CString sFolder="";
			char szDir[MAX_PATH]="c:\\";
			CFakeWindow fakeWnd(_l("Browse for folder"));
			if(GetFolder(_l("Choose directory"), szDir, NULL, fakeWnd.GetHwnd())){//GetAppWnd()->GetSafeHwnd()
				sLastFolder=szDir;
				sFolder=szDir;
				if(bJScriptSafe){_XLOG_
					MakeJScriptSafe(sFolder);
				}
			}
			sText.Replace("%ASKFOLDER",sFolder);
			sText.Replace("%E_ASKFOLDER",EscapeString(sFolder));
		}
	}
	// Environment variables
	SubstEnvVariables(sText);
	COleDateTime dt=COleDateTime::GetCurrentTime();
	sText.Replace("%DATE",Format("%02lu.%02lu.%04lu",dt.GetDay(),dt.GetMonth(),dt.GetYear()));
	char szTmp[256]={0};
	GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,0,0,szTmp,sizeof(szTmp));
	sText.Replace("%DATE_SHORT",szTmp);
	GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,0,0,szTmp,sizeof(szTmp));
	sText.Replace("%DATE_LONG",szTmp);
	sText.Replace("%TIME",Format("%02lu.%02lu.%02lu",dt.GetHour(),dt.GetMinute(),dt.GetSecond()));
	GetDateFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,0,0,szTmp,sizeof(szTmp));
	sText.Replace("%TIME_SHORT",szTmp);
	GetDateFormat(LOCALE_USER_DEFAULT,TIME_NOTIMEMARKER,0,0,szTmp,sizeof(szTmp));
	sText.Replace("%TIME_LONG",szTmp);
	sText.Replace("%YEAR",Format("%04lu",dt.GetYear()));
	sText.Replace("%MONTH",Format("%02lu",dt.GetMonth()));
	sText.Replace("%DAY",Format("%02lu",dt.GetDay()));
	sText.Replace("%HOUR",Format("%02lu",dt.GetHour()));
	sText.Replace("%MINUTE",Format("%02lu",dt.GetMinute()));
	sText.Replace("%SECOND",Format("%02lu",dt.GetSecond()));
	sText.Replace("%RNDNAME",GetRandomName());
	sText.Replace("%TICKCOUNT",Format("%lu",GetTickCount()));
	sText.Replace("%APPLICATIONNAME",GetApplicationName());
	sText.Replace("%APPLICATIONDIR",GetAppFolder());
	if(sText.Find("%PROFILEDIR")!=-1){_XLOG_
		sText.Replace("%PROFILEDIR",GetUserFolder());
	}
}

CString createExclusionName(LPCTSTR GUID, UINT kind)
{_XLOG_
    switch(kind)
    { // тип  
		
	case UNIQUE_TO_SYSTEM:
		return CString(GUID);
		
	case UNIQUE_TO_DESKTOP:
        { // рабочий стол  
            CString s = GUID;
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
            CString s = GUID;
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
                CString t;
                t.Format(_T("_%08x%08x"), uid.HighPart, uid.LowPart);
                return s + t;
            } // NT  
			
            else
            { // 16-битная OS  
				
                return s;
            } // 16-битная OS 
        } // сессия 
		
	case UNIQUE_TO_TRUSTEE:
        { // пользователь 
            CString s = GUID;
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
                CString t;
                t.Format(_T("_%s_%s"), domainName, userName);
                s += t;
            } // получаем сетевое имя 
            return s;
        } // пользователь 
	case UNIQUE_TO_COMPUTER:
		{ // пользователь 
            CString s = GUID;
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
                CString t;
                t.Format(_T("_%s_%s"), domainName, szCompName);
                s += t;
            } // получаем имя компа
            return s;
        } // пользователь 
	default:
		ASSERT(FALSE);
		break;
    } // тип 
	
    return CString(GUID);
}// createExclusionName

BOOL CheckProgrammRunState(const char* szProgName, UINT kind, bool bLeaveHandled, const char* szDesktop)
{_XLOG_
    bool AlreadyRunning=false;
    SetLastError(0);
	CString sDesk=szDesktop;
	if(sDesk==""){_XLOG_
		IsDefaultDesktop(&sDesk);
	}
	CString sMutexName=createExclusionName(CString("WIREDPLANE_")+(szProgName?szProgName:GetApplicationName())+sDesk, kind);
    HANDLE hMutexOneInstance = ::CreateMutex( NULL, TRUE, sMutexName);
    DWORD dwErr = GetLastError();
    AlreadyRunning = (dwErr == ERROR_ALREADY_EXISTS);
    if (hMutexOneInstance != NULL){_XLOG_
        ::ReleaseMutex(hMutexOneInstance);
    }
    if(!bLeaveHandled){_XLOG_
		CloseHandle(hMutexOneInstance);
    }
	return AlreadyRunning;
}

BOOL IsThisProgrammAlreadyRunning(UINT kind)
{_XLOG_
	return CheckProgrammRunState(NULL, kind, true);
}

BOOL IsOtherProgrammAlreadyRunning(const char* szProgName, UINT kind)
{_XLOG_
	return CheckProgrammRunState(szProgName, kind, false);
}

UINT GetProgramWMMessage(const char* szProgName)
{_XLOG_
	CString sMsg=CString("WM_WIREDPLANE_")+(szProgName?szProgName:GetApplicationName());
	return RegisterWindowMessage(sMsg);
}

BOOL AppRunStateFile(const char* szProgName, BOOL bReadSaveDel, const char* szDesktop)
{
	BOOL bRes=FALSE;
	SetLastError(0);
	CString sDesk=szDesktop;
	if(sDesk==""){
		// Получаем имя десктопа
		IsDefaultDesktop(&sDesk);
	}
	CString sMutexName=createExclusionName(CString("proc_")+(szProgName?szProgName:GetApplicationName())+"_"+sDesk, UNIQUE_TO_SYSTEM);
	CString sBaseFile=GetUserFolder()+sMutexName+".anc";
	CString sData;
	if(bReadSaveDel==1)
	{
		// Читаем файл
		ReadFile(sBaseFile,sData);
		DWORD dwProcId=atol(sData);
		HANDLE otherProc=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcId);
		bRes=otherProc?TRUE:FALSE;
		CloseHandle(otherProc);
	}
	if(bReadSaveDel==0)
	{
		// Создаем файл
		sData=Format(GetCurrentProcessId());
		SaveFile(sBaseFile,sData);
	}
	if(bReadSaveDel==-1)
	{
		DeleteFile(sBaseFile);
	}
	return bRes;
}

static int CALLBACK BrowseCallbackProc (HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{_XLOG_
    TCHAR szPath[_MAX_PATH];
    switch (uMsg) {_XLOG_
    case BFFM_INITIALIZED:
        if (lpData)
            SendMessage(hWnd,BFFM_SETSELECTION,TRUE,lpData);
        break;
    case BFFM_SELCHANGED:
        SHGetPathFromIDList(LPITEMIDLIST(lParam),szPath);
        SendMessage(hWnd, BFFM_SETSTATUSTEXT, NULL, LPARAM(szPath));
        break;
    }
    return 0;
}

#define BIF_EDITBOX            0x0010   // Add an editbox to the dialog
#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
// Caller needs to call OleInitialize() before using this API
#define BIF_USENEWUI           (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
BOOL GetFolderRaw (LPCTSTR szTitle, LPTSTR szPath, LPCTSTR szRoot, HWND hWndOwner, DWORD dwFlags)
{_XLOG_
    if (szPath == NULL)
        return false;

	CoInitialize(0);
    bool result = false;

    LPMALLOC pMalloc;
    if (::SHGetMalloc(&pMalloc) == NOERROR) {_XLOG_
        BROWSEINFO bi;
        ::ZeroMemory(&bi,sizeof bi);
        bi.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_USENEWUI;

        // дескриптор окна-владельца диалога
        bi.hwndOwner = hWndOwner;

        // добавление заголовка к диалогу
        bi.lpszTitle = szTitle;
		// 
		CString strDisplayName;
		bi.pszDisplayName=strDisplayName.GetBuffer(MAX_PATH + 1);

        // отображение текущего каталога
        bi.lpfn      = BrowseCallbackProc;
        bi.ulFlags  |= BIF_STATUSTEXT;
		bi.ulFlags  |= dwFlags;

        // установка каталога по умолчанию
        bi.lParam    = LPARAM(szPath);

        // установка корневого каталога
		BOOL bSearchForComp=FALSE;
		if(dwFlags & BIF_BROWSEFORCOMPUTER){_XLOG_
			bSearchForComp=TRUE;
		}
        if (szRoot != NULL) {_XLOG_
            IShellFolder *pDF;
            if (SHGetDesktopFolder(&pDF) == NOERROR) {_XLOG_
                LPITEMIDLIST pIdl = NULL;
                ULONG        chEaten;
                ULONG        dwAttributes;

                USES_CONVERSION;
                LPOLESTR oleStr = T2OLE(szRoot);

                pDF->ParseDisplayName(NULL,NULL,oleStr,&chEaten,&pIdl,&dwAttributes);
                pDF->Release();
                bi.pidlRoot = pIdl;
            }
        }else{_XLOG_
			if(bSearchForComp){_XLOG_
				LPITEMIDLIST pIdl = NULL;
				if(MyGetSpecialFLoc(NULL,CSIDL_NETWORK,&pIdl)){_XLOG_
					bi.pidlRoot=pIdl;
				}
			}
		}
        LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
		strDisplayName.ReleaseBuffer();
        if (pidl != NULL) {_XLOG_
			if (::SHGetPathFromIDList(pidl,szPath)){_XLOG_
				result = true;
			}else{_XLOG_
				strcpy(szPath,strDisplayName);
				if(strlen(szPath)>0){_XLOG_
					result = true;
				}
			}
            pMalloc->Free(pidl);
			//
			if(bSearchForComp){_XLOG_
				// Выдергиваем имя компа
				CString sCompOnly=CDataXMLSaver::GetInstringPart("\\\\","\\",szPath);
				if(strlen(sCompOnly)>0){_XLOG_
					strcpy(szPath,sCompOnly);
				}
			}
        }
        if (bi.pidlRoot != NULL)
            pMalloc->Free((void*)bi.pidlRoot);
        pMalloc->Release();
    }
	CoUninitialize();
    return result;
}

BOOL GetFolder(LPCTSTR szTitle, LPTSTR szPath, LPCTSTR szRoot, HWND hWndOwner)
{_XLOG_
	return GetFolderRaw (szTitle, szPath, szRoot, hWndOwner, 0);
}

BOOL GetFolderComputer(LPCTSTR szTitle, LPTSTR szPath, HWND hWndOwner)
{_XLOG_
	return GetFolderRaw (szTitle, szPath, NULL, hWndOwner, BIF_BROWSEFORCOMPUTER);
}

void ConvertComboDataToArray(const char* szData, CStringArray& aStrings, char c, char cComment)
{_XLOG_
	CString sData=szData;
	CString sChar(c);
	sData.Replace(sChar,sChar+sChar);
	sData=sChar+sData+sChar;
	int iFrom=0;
	while(iFrom>=0){_XLOG_
		CString sLine=CDataXMLSaver::GetInstringPart(sChar,sChar,sData,iFrom);
		sLine.TrimLeft();
		sLine.TrimRight();
		if(sLine!=""){_XLOG_
			if(cComment==0 || sLine[0]!=cComment){_XLOG_
				aStrings.Add(sLine);
			}
		}
	}
}

void ConvertToArray(const char* szData, CStringArray& aStrings, char c, char cComment)
{_XLOG_
	ConvertComboDataToArray(szData, aStrings, c, cComment);
}

void MakeJScriptSafe(CString& sFile)
{_XLOG_
	sFile.Replace("\\","\\\\");
	sFile.Replace("'","\\\'");
	sFile.Replace("\"","\\\"");
}


CMapStringToString& getMCookies()
{_XLOG_
	static CMapStringToString aCookies;
	return aCookies;
}

CString getCookie(const char* szKey)
{_XLOG_
	CString sVal;
	getMCookies().Lookup(szKey,sVal);
	return sVal;
}

// CreateLink - uses the Shell's IShellLink and IPersistFile interfaces 
//   to create and store a shortcut to the specified object. 
// Returns the result of calling the member functions of the interfaces. 
// lpszPathObj - address of a buffer containing the path of the object. 
// lpszPathLink - address of a buffer containing the path where the 
//   Shell link is to be stored. 
// lpszDesc - address of a buffer containing the description of the 
//   Shell link. 

HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc, LPCSTR lpszAppParams)
{_XLOG_
    HRESULT hres; 
    IShellLink* psl; 
	
	// Get a pointer to the IShellLink interface. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl);
	if (SUCCEEDED(hres)) { 
		IPersistFile* ppf; 
		
        // Set the path to the shortcut target and add the 
        // description. 
        psl->SetPath(lpszPathObj); 
        psl->SetDescription(lpszDesc); 
		if(lpszAppParams && strlen(lpszAppParams)>0){_XLOG_
			psl->SetArguments(lpszAppParams);
		}
		// Query IShellLink for the IPersistFile interface for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
		
		if (SUCCEEDED(hres)) {_XLOG_
            WCHAR wsz[MAX_PATH]; 
			
			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, 
				wsz, MAX_PATH); 
			
			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(wsz, TRUE); 
			ppf->Release(); 
		} 
		psl->Release(); 
	}
	return hres; 
}

// 0/1 - автозапуск через реестр или папку автозапуска
#define STARTUP_THROW_REGISTRY	0
BOOL IsStartupWithWindows()
{_XLOG_
#if STARTUP_THROW_REGISTRY != 1
	// через папку
	char szFolderBuffer[MAX_PATH]="";
	strcpy(szFolderBuffer,GetApplicationName());
	GetSpecialFolderPath(szFolderBuffer,CSIDL_STARTUP);
	CString sFilePath=szFolderBuffer;
	sFilePath+="\\";
	sFilePath+=CString(GetApplicationName())+".lnk";
	FILE* m_pFile=fopen(sFilePath,"r");
	if(!m_pFile){_XLOG_
		return FALSE;
	}
	fclose(m_pFile);
	return TRUE;
#else
	// Через реестр
	char szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, sizeof(szPath));
	char szName[MAX_PATH];
	_splitpath (szPath, NULL, NULL, szName, NULL);
	char szTemp[MAX_PATH];
	DWORD lSize = MAX_PATH,dwType=0;
	CRegKey key;
	key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run");//if (key.QueryValue (szTemp, NULL, szName, &lSize ) != ERROR_SUCCESS) return FALSE;
	if(RegQueryValueEx(key.m_hKey,szName,NULL, &dwType,(LPBYTE)szTemp, &lSize)!= ERROR_SUCCESS)
		return FALSE;
	return TRUE;
#endif
}

BOOL StartupApplicationWithWindows(BOOL bStartup)
{_XLOG_
	char szPath[MAX_PATH]="";
	GetModuleFileName(NULL, szPath, sizeof(szPath));
#if STARTUP_THROW_REGISTRY != 1
	char szFolderBuffer[MAX_PATH]="";
	strcpy(szFolderBuffer,GetApplicationName());
	GetSpecialFolderPath(szFolderBuffer,CSIDL_STARTUP);
	CString sFilePath=szFolderBuffer;
	sFilePath+="\\";
	sFilePath+=CString(GetApplicationName())+".lnk";
	if(!bStartup){_XLOG_
		return DeleteFile(sFilePath);
	}else{_XLOG_
		return (CreateLink(szPath, sFilePath, GetApplicationName())==S_OK);
	}
#else
	char szName[MAX_PATH];
	_splitpath (szPath, NULL, NULL, szName, NULL);

	CRegKey key;
	key.Open (HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	if(key!=NULL){_XLOG_
		if (bStartup){_XLOG_
			DWORD dwType=REG_SZ,dwSize=0;
			if(RegSetValueEx(key.m_hKey,szName,0,REG_SZ,(BYTE*)szPath,lstrlen(szPath)+1)!= ERROR_SUCCESS)
				return 0;
			/*if(key.SetValue(szPath,REG_SZ, szName, strlen(szName)) != ERROR_SUCCESS)
				return FALSE;*/
		}else{_XLOG_
			if(key.DeleteValue(szName) != ERROR_SUCCESS)
				return FALSE;
		}
	}
	return TRUE;
#endif
}

BOOL isWin98()
{_XLOG_
	//return 1;
	static long bWin98=-1;
	if(bWin98==-1){_XLOG_
		long bWinNT=0;
		CString sOS;
		GetOSVersion(sOS,NULL,&bWinNT);
		if(bWinNT){_XLOG_
			bWin98=FALSE;
		}else{_XLOG_
			bWin98=TRUE;
		}
	}
	return bWin98;
}

BOOL isWinXP()
{_XLOG_
	//return 1;
	static long bWinXP=-1;
	if(bWinXP==-1){_XLOG_
		long bWinNT=0;
		CString sOS;
		DWORD dwMaj=0;
		GetOSVersion(sOS,&dwMaj,&bWinNT);
		if(sOS.Find("XP")!=-1 || sOS.Find("Vista")!=-1 || sOS.Find("Server")!=-1 || dwMaj>5){_XLOG_
			bWinXP=TRUE;
		}else{_XLOG_
			bWinXP=FALSE;
		}
	}
	return bWinXP;
}

FILE* getFile(const char* szFName, const char* szOpenType, CString* sFileNameOut)
{_XLOG_
	CString sFileToOpen=getFileFullName(szFName);
	if(sFileNameOut){_XLOG_
		*sFileNameOut=sFileToOpen;
	}
	FILE* pFile=fopen(sFileToOpen,szOpenType);
	return pFile;
}

CString GetRandomName()
{_XLOG_
	static const char* szSogl="BCDEFGHKLMNPQRSTVWXYZ";
	static const char* szGl="EUIOA";
	const char* szCur=rnd(1,100)<75?szSogl:szGl;
	int iLen=rnd(3,7);
	CString sRes="";
	for(int i=0;i<iLen;i++){_XLOG_
		sRes+=szCur[rnd(0,strlen(szCur)-1)];
		if(rnd(0,100)<(szCur==szSogl?85:95)){_XLOG_
			if(szCur==szSogl){_XLOG_
				szCur=szGl;
			}else{_XLOG_
				szCur=szSogl;
			}
		}
	}
	CString sOut=sRes;
	sOut.MakeLower();
	sOut.SetAt(0,sRes[0]);
	return sOut;
}

int FindSubstrWithVolatilePP(CString& sCML,const char* sPML,const char* szPrefixes,const char* szPostFixes)
{_XLOG_
	char szTmp[2];
	szTmp[1]=0;
	char const* szPrefPos=szPrefixes;
	char const* szPostPos=szPostFixes;
	while(*szPrefPos){_XLOG_
		while(*szPostPos){_XLOG_
			szTmp[0]=*szPrefPos;
			CString sLookingFor=szTmp;
			sLookingFor+=sPML;
			szTmp[0]=*szPostPos;
			sLookingFor+=szTmp;
			int iPos=sCML.Find(sLookingFor);
			if(iPos!=-1){_XLOG_
				return iPos;
			}
			szPostPos++;
		}
		szPrefPos++;
	}
	return -1;
}

BOOL IsDefaultDesktop(CString* sDeskName)
{_XLOG_
	if(sDeskName){_XLOG_
		*sDeskName="";
	}
	CString WinVer;
	DWORD dwMajor=0;
	long bNT=0;
	if(GetOSVersion(WinVer, &dwMajor, &bNT) && bNT){_XLOG_
		HDESK hD=GetThreadDesktop(GetCurrentThreadId());
		char szDeskName[250]="";
		DWORD dwLen=sizeof(szDeskName);
		if(GetUserObjectInformation(hD,UOI_NAME,szDeskName,sizeof(szDeskName),&dwLen)){_XLOG_
			if(sDeskName){_XLOG_
				*sDeskName=szDeskName;
			}
			if(stricmp(szDeskName,"Default")!=0){_XLOG_
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL IsBatFile(const char* szCommand)
{_XLOG_
	CString sFile=szCommand;
	sFile.MakeLower();
	if(sFile.Find(".bat")!=-1){_XLOG_
		return TRUE;
	}
	if(sFile.Find(".cmd")!=-1){_XLOG_
		return TRUE;
	}
	return FALSE;
}

BOOL IsConsoleApplication(const char* szFile)
{_XLOG_
	if(IsBatFile(szFile)){_XLOG_
		return TRUE;
	}
	FILE* pFile=fopen(szFile,"rb");
	if(pFile){_XLOG_
		IMAGE_DOS_HEADER headDOS;
		memset(&headDOS,0,sizeof(headDOS));
		fread(&headDOS,1,sizeof(headDOS),pFile);
		fseek(pFile,headDOS.e_lfanew,SEEK_SET);
		IMAGE_NT_HEADERS headNT;
		memset(&headNT,0,sizeof(headNT));
		fread(&headNT,1,sizeof(headNT),pFile);
		fclose(pFile);
		if(headNT.OptionalHeader.Subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI){_XLOG_
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ParseForShellExecute(CString sFrom, CString& sCommand, CString& sParameter, BOOL bPartialFindIsOK)
{_XLOG_
	BOOL bRes=FALSE;
	sFrom.TrimLeft(" ");
	sFrom.TrimRight(" ");
	sCommand=sFrom;
	sParameter="";
	// Смотрим - может здесь программа+параметр
	int iKPos=sFrom.Find(' ',1);
	if(sFrom.Left(1)=='\"'){_XLOG_
		iKPos=sFrom.Find('\"',1);
	}
	if(iKPos!=-1){_XLOG_
		iKPos++;
		sCommand=sFrom.Left(iKPos);
		sParameter=sFrom.Mid(iKPos);
		sParameter.TrimLeft(" ");
		sParameter.TrimRight(" ");
		sCommand.TrimLeft(" \"");
		sCommand.TrimRight(" \"");
		if(FindFullPathToExecutable(sCommand)){_XLOG_
			bRes=TRUE;
		}else if(!bPartialFindIsOK){_XLOG_
			// Ничего не нашлось
			// Возвращаемся к началу
			sCommand=sFrom;
			sParameter="";
		}
	}
	if(IsConsoleApplication(sCommand) || IsBatFile(sCommand)){_XLOG_
		sParameter=Format("/T:0A /K \"%s %s\"",sCommand,sParameter);
		sCommand="cmd.exe";
		return TRUE;
	}
	return bRes;
}

DWORD dwMyRndSeed=0;
DWORD myrand()
{_XLOG_
	return( ((dwMyRndSeed = dwMyRndSeed * 214013L + 2531011L) >> 16) & MYRAND_MAX );
}

int randEx()
{_XLOG_
	static DWORD iFirstValue=0;
	if(iFirstValue==0){_XLOG_
		iFirstValue=1;
		dwMyRndSeed=(unsigned)(time(NULL)-GetCurrentThreadId()*7);
	}
	return myrand();
}

// Случайное число ВКЛЮЧАЯ границы
int rnd(int _min, int _max)
{_XLOG_
	int iRndVal=randEx();
	double d1=double(iRndVal)/double(MYRAND_MAX+1);
	int iRes = _min + (int)floor(double(_max - _min + 1)*d1);// RAND_MAX+1 - чтобы небыло единицы, а floor округлит в меньшую сторону
	return iRes;
}

BOOL FindFullPathToFile(CString& sCommand)
{_XLOG_
	if(isFileExist(sCommand)){_XLOG_
		return TRUE;
	}
	char szFullPath[MAX_PATH]="";
	strcpy(szFullPath,sCommand);
	if(_Shlwapi().fpFind!=NULL){_XLOG_
		if(_Shlwapi().fpFind(szFullPath,NULL)){_XLOG_
			sCommand=szFullPath;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL FindFullPathToExecutable(CString& sCommand)
{_XLOG_
	char szPathOut[MAX_PATH]="";
	strcpy(szPathOut,sCommand);
	DWORD dwDoEnv=DoEnvironmentSubst(szPathOut,sizeof(szPathOut));
	if(HIWORD(dwDoEnv)>0){_XLOG_
		sCommand=szPathOut;
	}
	if(FindFullPathToFile(sCommand)){_XLOG_
		return TRUE;
	}
	CString sRes;
	sRes=sCommand+".exe";
	if(FindFullPathToFile(sRes)){_XLOG_
		sCommand=sRes;
		return TRUE;
	}
	sRes=sCommand+".com";
	if(FindFullPathToFile(sRes)){_XLOG_
		sCommand=sRes;
		return TRUE;
	}
	sRes=sCommand+".bat";
	if(FindFullPathToFile(sRes)){_XLOG_
		sCommand=sRes;
		return TRUE;
	}
	sRes=sCommand+".cmd";
	if(FindFullPathToFile(sRes)){_XLOG_
		sCommand=sRes;
		return TRUE;
	}
	return FALSE;
}

WCHAR* MyA2W(const char* szText)
{_XLOG_
	int len=strlen(szText);
	WCHAR* wszTextToSet = NewWCHAR(len+1);
	MultiByteToWideChar(CP_ACP, 0, szText, -1, wszTextToSet, len);
	return wszTextToSet ;
}

CString GetPathPart(const char* szFile, BOOL bD,BOOL bP,BOOL bN,BOOL bE)
{_XLOG_
	if(!szFile){_XLOG_
		return "";
	}
	if(strlen(szFile)>7 && (memicmp(szFile,"http://",7)==0 || memicmp(szFile,"::{",3)==0)){_XLOG_
		return szFile;
	}
	if(ExtractInlineScript(szFile)!=""){_XLOG_
		return szFile;
	}
	char szD[MAX_PATH]={0},szP[MAX_PATH]={0},szN[MAX_PATH]={0},szE[MAX_PATH]={0};
	_splitpath(szFile,szD,szP,szN,szE);
	CString sRes;
	if(bD){_XLOG_
		sRes+=szD;
	}
	if(bP){_XLOG_
		sRes+=szP;
	}
	if(bN){_XLOG_
		sRes+=szN;
	}
	if(bE){_XLOG_
		sRes+=szE;
	}
	return sRes;
}

CString GetWindowClass(HWND h)
{
	char sz[256]={0};
	::GetClassName(h,sz,sizeof(sz));
	return sz;
}
