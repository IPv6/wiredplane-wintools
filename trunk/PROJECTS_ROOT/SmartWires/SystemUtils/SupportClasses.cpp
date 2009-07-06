// SupportClasses.cpp: implementation of the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SupportClasses.h"

int sgn(long l)
{_XLOG_
	if(l==0)
		return 0;
	if(l>0)
		return 1;
	return -1;
}

int randEx_old()
{_XLOG_
	static int iFirstValue=0;
	if(iFirstValue==0){_XLOG_
		srand(time(NULL));
		iFirstValue=rand();
	}
	int iResult=rand();
	if(iFirstValue==iResult){_XLOG_
		srand(time(NULL)+::GetCurrentThreadId()+iFirstValue);
		iResult=rand();
	}
	return iResult;
}

// Случайное число ВКЛЮЧАЯ границы
int randEx();
double rndDbl(double _min, double _max)
{_XLOG_
	double d=double(randEx());
	return _min + d / MYRAND_MAX * (_max - _min);
}

long getCharCount(const char* szString, char ch)
{_XLOG_
	char const* szPos=szString;
	long lOut=0;
	while(*szPos!=0){_XLOG_
		if(*szPos==ch){_XLOG_
			lOut++;
		}
		szPos++;
	}
	return lOut;
}

CString GetCOMError(int iErr)
{_XLOG_
	CString sOut;
	char szBuffer[1024]="";
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,iErr,0,szBuffer,sizeof(szBuffer),NULL);
	ScanAndReplace(szBuffer,"\t\r\n",' ');
	sOut.Format("%s (0x%X)",szBuffer,iErr);
	return sOut;
}

CString GetLastErrDsc()
{_XLOG_
	return GetCOMError(GetLastError());
}

int Validate(int iVal, int iMin, int iMax, int iDef)
{_XLOG_
	if(iVal>iMax || iVal<iMin)
		return iDef;
	else
		return iVal;
}

BOOL MakeSafeFileName(CString& sName, char sWithWhat)
{_XLOG_
	// Даже пробелы (для WN-в текстах мессаг)
	if(sWithWhat==0){
		sWithWhat='_';
	}
	ScanAndReplace(sName, "\\\"\'\t\r\n%:+!?/()<>*~`=; |", sWithWhat);
	sName.TrimLeft();
	sName.TrimRight();
	CString sChar(sWithWhat);
	CString sChar2=sChar+sChar;
	while(sName.Replace(sChar2,sChar)>0)
	{
		sName.Replace(sChar2,sChar);
	};
	sName.TrimLeft(sChar);
	sName.TrimRight(sChar);
	return TRUE;
}

long CtrlWidth(CWnd* wnd, UINT CtrlID)
{_XLOG_
	if(!wnd){_XLOG_
		return 0;
	}
	if(CtrlID==0){_XLOG_
		// Считаем неклиентские размеры
		CRect rect1;
		wnd->GetWindowRect(rect1);
		CRect rect2;
		wnd->GetClientRect(rect2);
		return rect1.Width()-rect2.Width();
	}
	CRect rect;
	CWnd* ctrlWnd=wnd->GetDlgItem(CtrlID);
	if(!ctrlWnd){_XLOG_
		return 0;
	}
	ctrlWnd->GetWindowRect(rect);
	return rect.Width();
}

long CtrlHeight(CWnd* wnd, UINT CtrlID)
{_XLOG_
	if(!wnd){_XLOG_
		return 0;
	}
	if(CtrlID==0){_XLOG_
		// Считаем неклиентские размеры
		CRect rect1;
		wnd->GetWindowRect(rect1);
		CRect rect2;
		wnd->GetClientRect(rect2);
		return rect1.Height()-rect2.Height();
	}
	CRect rect;
	CWnd* ctrlWnd=wnd->GetDlgItem(CtrlID);
	if(!ctrlWnd){_XLOG_
		return 0;
	}
	ctrlWnd->GetWindowRect(rect);
	return rect.Height();
}

int FindIDNumInNum2IDMap(aNum2IDMap& aM2I, DWORD dwCode)
{_XLOG_
	for(int i=0;i<aM2I.GetSize();i++){_XLOG_
		if(aM2I[i]==dwCode){_XLOG_
			return i;
		}
	}
	return -1;
}

// Если *acceptOwnerDrawMenus==-1, то OwnerDrawMenus будут пропускаться и не добавляться
// Если aM2I не NULL, то коды пунктов будут 1+offset, 2+offset и т.п., а реальные id будут в массиве
int AppendClonedMenuEx(CMenu* pMenu, HMENU hMenu, long lIDOffset, BOOL* acceptOwnerDrawMenus, aNum2IDMap* aM2I)
{_XLOG_
	int iRes=0;
	if(!pMenu || !hMenu || !IsMenu(hMenu)){_XLOG_
		return iRes;
	}
	int iCount=GetMenuItemCount(hMenu);
	int iBreakPosition=iCount/2;
	if(iBreakPosition>40){_XLOG_
		iBreakPosition=30;
	}
	if(iBreakPosition<15){_XLOG_
		iBreakPosition=-1;
	}
	for(int i=0;i<iCount;i++){_XLOG_
		char szTitle[256]="";
		MENUITEMINFO inf;
		memset(&inf,0,sizeof(inf));
		inf.fMask=MIIM_DATA|MIIM_TYPE|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_CHECKMARKS;
		inf.dwTypeData=(char*)&szTitle;
		inf.cch=sizeof(szTitle);
		inf.cbSize=sizeof(inf);
		if(GetMenuItemInfo(hMenu,i,TRUE,&inf)){_XLOG_
			char* szTab=strchr(szTitle,'\t');
			if(szTab){// Не показываем то что после табуляции (бывает '\t\t', горячие клавиши все равно недоступны)
				//ScanAndReplace(szTitle,"\t",' ');
				*szTab=0;
			}
			CString sItemTitle=szTitle;
			sItemTitle.TrimLeft();
			sItemTitle.TrimRight();
			BOOL bSeparator=0;
			if(inf.fType & MFT_SEPARATOR){_XLOG_
				bSeparator=1;
			}
			BOOL bOwnerDraw=0;
			if(inf.fType & MFT_OWNERDRAW){_XLOG_
				if(acceptOwnerDrawMenus && *acceptOwnerDrawMenus==-1){_XLOG_
					continue;
				}
				bOwnerDraw=1;
			}else{// Если не OwnerDraw, то иконки недоступны
				inf.fType&=~DWORD(MFT_BITMAP);
			}
			if(strlen(sItemTitle)==0 && !bOwnerDraw && !bSeparator){_XLOG_
				//sItemTitle=Format("Menu item #%i",i);
				continue;
			}
			BOOL bBreak=0;
			if(iBreakPosition>0 && i>1 && ((i-1)%iBreakPosition)==0){_XLOG_
				bBreak=MF_MENUBARBREAK;
			}
			if(inf.hSubMenu!=0){_XLOG_
				CMenu SubMenu;
				SubMenu.CreatePopupMenu();
				int iSubRes=AppendClonedMenuEx(&SubMenu,inf.hSubMenu,lIDOffset,acceptOwnerDrawMenus,aM2I);
				if(iSubRes>0){_XLOG_
					iRes+=iSubRes;
					int iItemID=(UINT)SubMenu.GetSafeHmenu();
					if(bOwnerDraw){_XLOG_
						if(aM2I){_XLOG_
							int iIndex=(*aM2I).Add((UINT)inf.wID);
							iItemID=iIndex+lIDOffset;
						}
						if(acceptOwnerDrawMenus){_XLOG_
							*acceptOwnerDrawMenus=1;
						}
						inf.wID=iItemID;
						inf.hSubMenu=SubMenu.GetSafeHmenu();
						inf.fState=MF_OWNERDRAW | MF_POPUP | bBreak;//inf.fState | 
						//// bug here!!!
						BOOL bRes=pMenu->AppendMenu(inf.fState, iItemID, (const char*)inf.dwItemData);
						// Восстанавливаем статус кво
						inf.cbSize=sizeof(inf);
						inf.fMask=MIIM_DATA|0x00000100|MIIM_SUBMENU|MIIM_ID;
						SetMenuItemInfo(pMenu->GetSafeHmenu(),iItemID,FALSE,&inf);
					}else{_XLOG_
						BOOL bRes=pMenu->AppendMenu(inf.fState | inf.fType | MF_POPUP | bBreak, iItemID, sItemTitle);
					}
					SubMenu.Detach();
					iRes++;
				}
			}else{_XLOG_
				int iItemID=inf.wID;
				if(aM2I){_XLOG_
					int iIndex=(*aM2I).Add(iItemID);
					iItemID=iIndex+lIDOffset;
				}else{_XLOG_
					iItemID=inf.wID+lIDOffset;
				}
				if(bOwnerDraw){_XLOG_
					BOOL bRes=pMenu->AppendMenu(inf.fState | MF_OWNERDRAW | bBreak, iItemID, (const char*)inf.dwItemData);
					if(acceptOwnerDrawMenus){_XLOG_
						*acceptOwnerDrawMenus=1;
					}
				}else{_XLOG_
					BOOL bRes=pMenu->AppendMenu(inf.fState | inf.fType | bBreak, iItemID, sItemTitle);
				}
				iRes++;
			}
		}else{_XLOG_
			DWORD dwRes=GetLastError();
		}
	}
	return iRes;
}

int AppendClonedMenuHalfEx(CMenu* pMenu, HMENU hMenu, long lIDOffset, BOOL* acceptOwnerDrawMenus, aNum2IDMap* aM2I)
{_XLOG_
	static long lLock=0;
	if(lLock>0){_XLOG_
		return 0;
	}
	SimpleTracker tr(lLock);
	return AppendClonedMenuEx(pMenu, hMenu, lIDOffset, acceptOwnerDrawMenus, aM2I);
}

int AppendClonedMenu(CMenu* pMenu, HMENU hMenu, long lIDOffset, BOOL* acceptOwnerDrawMenus, aNum2IDMap* aM2I)
{_XLOG_
	int iRes=0;
	__try{_XLOG_
		iRes=AppendClonedMenuHalfEx(pMenu, hMenu, lIDOffset, acceptOwnerDrawMenus, aM2I);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		iRes=0;
	}
	return iRes;

}

BOOL isWindowStationLocked()
{_XLOG_
	static BOOL bWin98=isWin9x();
	if(bWin98 || ::GetForegroundWindow()!=NULL){_XLOG_
		return FALSE;
	}
	SetLastError(0);
	HDESK desktop = OpenInputDesktop(0,FALSE,DESKTOP_READOBJECTS);
	DWORD dwLastError=GetLastError();
	if(desktop){_XLOG_
		char szData[256]="";
		DWORD dwLen=0;
		GetUserObjectInformation(desktop, UOI_NAME, szData, sizeof(szData), &dwLen);
		//AfxMessageBox(szData);
		CloseDesktop(desktop);
	}else if(dwLastError==0 || (IsVista() && dwLastError==ERROR_ACCESS_DENIED)){_XLOG_
		// Если залокирована станция - десктоп не открыть!
		return TRUE;
	}
	return FALSE;
}

BOOL isScreenSaverActive()
{_XLOG_
	BOOL srunning=FALSE;
	BOOL res=SystemParametersInfo(114/*SPI_GETSCREENSAVERRUNNING*/,0,&srunning,0);
	if (res) {_XLOG_
		if (srunning==0){_XLOG_
			return FALSE;
		}else{_XLOG_
			return TRUE;
		}
	}
	// That works fine under '95, '98 and NT5. But not older versions of NT.
	// Hence we need some magic.
	HDESK hDesk=OpenDesktop(TEXT("screen-saver"), 0, FALSE, MAXIMUM_ALLOWED);
	if (hDesk!=NULL){_XLOG_
		CloseDesktop(hDesk);
		return TRUE;
	}
	if (GetLastError()==ERROR_ACCESS_DENIED){_XLOG_
		return TRUE;
	}
	return FALSE;
}

BOOL isScreenSaverActiveOrCLocked()
{_XLOG_
	if(isScreenSaverActive()){_XLOG_
		return TRUE;
	}
	return isWindowStationLocked();
}

void AddMenuStringEx(CDWordArray* aIDsLog, CMenu* pMenu, int iID, const char* szString, CBitmap* pBmp, BOOL bCheck, CBitmap* pBmpC, BOOL bGray, BOOL bDisabled, BOOL bBreakMenu)
{_XLOG_
	if(aIDsLog){_XLOG_
		aIDsLog->Add(iID);
	}
	AddMenuString(pMenu, iID, szString, pBmp, bCheck, pBmpC, bGray, bDisabled, bBreakMenu);
}

char szHexes[]="0123456789ABCDEF";
BOOL MakeSafeForURL(CString& sText)
{_XLOG_
	CString sOut="";
	sText.Replace("\t","    ");
	char const* szPos=sText;
	char buf[128]="";
	while(*szPos){_XLOG_
		char c=(*szPos);
		if(c<=32 || c>127 || c=='%' || c=='?' || c=='&' || c=='+'){_XLOG_
			sOut+="%";//Format("%%%2X",(*szPos));//sOut+=itoa((*szPos),buf,16);
			sOut+=szHexes[(c>>4)&0x0F];
			sOut+=szHexes[c&0x0F];
		}else{_XLOG_
			sOut+=c;
		}
		szPos++;
	}
	sText=sOut;
	return TRUE;
}

CSize GetLargestLineSize(CDC* pDC, CString sText, int& iLines)
{_XLOG_
	iLines=0;
	CSize sRes(0,0);
	while(TRUE){_XLOG_
		CString sLine=sText.SpanExcluding("\n");
		CSize sProbe=pDC->GetTextExtent(sLine);
		if(sProbe.cx>sRes.cx){_XLOG_
			sRes=sProbe;
		}
		iLines++;
		if(sLine==sText){_XLOG_
			break;
		}
		if(strlen(sLine)<strlen(sText)){_XLOG_
			sText=sText.Mid(strlen(sLine)+1);
		}
	}
	return sRes;
}

CString GetLargestLine(CString sText, int& iLines)
{_XLOG_
	iLines=0;
	CString sRes="";
	//sText.Replace("\r\n","\n");
	while(TRUE){_XLOG_
		CString sLine=sText.SpanExcluding("\n");
		if(sRes.GetLength()<sLine.GetLength()){_XLOG_
			sRes=sLine;
		}
		iLines++;
		if(sLine==sText){_XLOG_
			break;
		}
		if(strlen(sLine)<strlen(sText)){_XLOG_
			sText=sText.Mid(strlen(sLine)+1);
		}
	}
	return sRes;
}

// iType==0 - обычное
// iType==1 - автоматическое
// iType==2 - ускоренное
// iType==3 - Suspend
// iType==4 - Hinernate
void ShutDownComputer(BOOL bRestart, int iType)
{_XLOG_
	FLOG2("ShutDownComputer %i ... %i", bRestart, iType);
	/*
	::SendMessage(HWND_BROADCAST,WM_SYSCOMMAND, SC_MONITORPOWER, 1);// Выключить монитор
	"rundll32 shell32,SHExitWindowsEx 1" - Выключение Компутера.
	"rundll32 shell32,SHExitWindowsEx 0" - Завершить Работу Текущего Пользователя
	"rundll32 shell32,SHExitWindowsEx 2" Windows-98-PC boot
	*/
	if(bRestart && iType>2){_XLOG_
		iType=0;
	}
	CString sVer;
	long lUnderNT=0;
	DWORD dwMajor=0;
	GetOSVersion(sVer, &dwMajor, &lUnderNT);
	if(!lUnderNT){_XLOG_
		if(bRestart){_XLOG_
			if(iType==2){_XLOG_
				WinExec("rundll32 shell32,SHExitWindowsEx 6",SW_HIDE);
			}else{_XLOG_
				WinExec("rundll32 shell32,SHExitWindowsEx 2",SW_HIDE);
			}
		}
		if(iType==2){_XLOG_
			//WinExec("rundll32 krnl386.exe,exitkernel",SW_HIDE); // моментальное выключение
			WinExec("rundll32 shell32,SHExitWindowsEx 13",SW_HIDE);
		}else{_XLOG_
			WinExec("rundll32 shell32,SHExitWindowsEx 9",SW_HIDE);
		}
		if(iType==3){_XLOG_
			SetSystemPowerState(1,1);
			return;
		}
		if(iType==4){_XLOG_
			SetSystemPowerState(0,1);
			return;
		}
		return;
	}
	// Берем привилегию...
	HANDLE hToken;              // handle to process token
	TOKEN_PRIVILEGES tkp;       // pointer to token structure 
	// Get the current process token handle so we can get shutdown privilege. 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return; 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
	if (GetLastError() != ERROR_SUCCESS){_XLOG_
		AfxMessageBox(_l("Can`t shutdown computer")+"!");
		return;
	}
	DWORD dwType=0;// No force at all
	if(iType==1 && !bRestart){_XLOG_
		dwType=0x00000010;//EWX_FORCEIFHUNG
	}else if(iType==2){_XLOG_
		dwType=EWX_FORCE;
	}else if(iType==3){_XLOG_
		SetSystemPowerState(1,1);
		return;
	}else if(iType==4){_XLOG_
		SetSystemPowerState(0,1);
		return;
	}
	ExitWindowsEx((bRestart?EWX_REBOOT:EWX_POWEROFF)|dwType, 0xffffffff);
}

void AppDisplayProperties() 
{_XLOG_
	// Start the display properties dialog
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	CreateProcess( NULL, // No module name (use command line). 
		"control.exe desk.cpl", // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent’s environment block. 
		NULL,             // Use parent’s starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi);             // Pointer to PROCESS_INFORMATION structure.
}

void AppAddRemPrograms() 
{_XLOG_
	// Start the display properties dialog
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	CreateProcess( NULL, // No module name (use command line). 
		"control.exe appwiz.cpl", // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent’s environment block. 
		NULL,             // Use parent’s starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi);             // Pointer to PROCESS_INFORMATION structure.
}

BOOL CheckMyMaskSubstr(CString sTitle, CString sTitleMask, BOOL bDef)
{_XLOG_
	CString sIn=CDataXMLSaver::GetInstringPart("[=","=]",sTitleMask);
	if(sIn!=""){_XLOG_
		if(stricmp(sTitle,sIn)==0){_XLOG_
			return TRUE;
		}
	}
	CString sNotMaybe=CDataXMLSaver::GetInstringPart("[!","!]",sTitleMask);
	if(sNotMaybe!="" && sTitle.Find(sNotMaybe)!=-1){_XLOG_
		return FALSE;
	}
	CString sMaybe=CDataXMLSaver::GetInstringPart("[?","?]",sTitleMask);
	if(sMaybe!="" && sTitle.Find(sMaybe)!=-1){_XLOG_
		return TRUE;
	}
	if(sTitleMask.Find("*")==-1){_XLOG_
		if(sTitle.Find(sTitleMask)!=-1){_XLOG_
			return TRUE;
		}
	}else if(PatternMatch(sTitle,sTitleMask)){_XLOG_
		return TRUE;
	}
	return bDef;
}

void SendSuggestion(const char* szProg, const char* szVer, const char* szSubj)
{_XLOG_
	CString sOperSystem;
	GetOSVersion(sOperSystem);
	CString sText="mailto:";
	sText+=szProg;
	sText+="@wiredplane.com?Subject=";
	sText+=szSubj;
	sText+="&body=%0d%0a***%0d%0a";
	sText+=szProg;
	sText+="%20";
	sText+=szVer;
	sText+="%20-%20";
	sOperSystem.Replace(" ","%20");
	sText+=sOperSystem;
	ShellExecute(AfxWPGetMainWnd()?AfxWPGetMainWnd()->GetSafeHwnd():NULL,NULL,sText,"",NULL,SW_SHOWNORMAL);
}

static int killerKilled=0;
CThreadKiller& GetThreadKiller()
{_XLOG_
	static CThreadKiller killer;
	return killer;
}

CThreadKillerHolder::CThreadKillerHolder()
{_XLOG_
	if(!killerKilled){_XLOG_
		GetThreadKiller().AddThis();
	}
}

CThreadKillerHolder::~CThreadKillerHolder()
{_XLOG_
	if(!killerKilled){_XLOG_
		GetThreadKiller().RemoveThis();
	}
}

CThreadKiller::CThreadKiller()
{_XLOG_
	iTCount=0;
}

CThreadKiller::~CThreadKiller()
{_XLOG_
	killerKilled=1;
	CSmartLock Lock(&cs,TRUE);
	for(int i=0;i<threads.GetSize();i++){_XLOG_
		if(threads[i]){_XLOG_
			TerminateThread(threads[i],57);
			CloseHandle(threads[i]);
		}
	}
	threads.RemoveAll();
}

void CThreadKiller::AddThis()
{_XLOG_
	CSmartLock Lock(&cs,TRUE);
	threads.Add(GetCurrentThread());
}

void CThreadKiller::RemoveThis()
{_XLOG_
	CSmartLock Lock(&cs,TRUE);
	HANDLE hID=0;//DuplicateHandle(GetCurrentThread(),);!!! не пашет иначе :(
	for(int i=0;i<threads.GetSize();i++){_XLOG_
		if(threads[i]==hID){_XLOG_
			threads.RemoveAt(i);
			return;
		}
	}
}

void GetWindowTopParent(HWND& hWnd, BOOL bStopOnInvisible)
{_XLOG_
	if(!hWnd){_XLOG_
		return;
	}
	//hWnd=::GetAncestor(hWnd,GA_ROOT);
	HWND hWnd2=hWnd;
	while(hWnd2!=NULL && (!bStopOnInvisible || IsWindowVisible(hWnd2))){_XLOG_
		hWnd=hWnd2;
		hWnd2=::GetWindow(hWnd,GW_OWNER);
		if(!hWnd2){_XLOG_
			hWnd2=::GetParent(hWnd);
		}
	}
	return;
}

#define DEF_LASTPRINTFILE	"Printed_note"
#define DEF_EXPLPRINTERSETS	"Software\\Microsoft\\Internet Explorer\\PageSetup"
#define DEF_IMPO_HEADER		"*** IPv6 - impossible header. Kirpi4i are cool! :>) ***"
#define DEF_IMPO_FOOTER		"*** IPv6 - impossible footer. Limp bizkit is cool! :^) ***"

BOOL PrintWindow_System(CWnd* pWnd, CString sText, CString sTitle)
{_XLOG_
	CString sTempFile=getFileFullName(DEF_LASTPRINTFILE);
	sTempFile+=".txt";
	SaveFile(sTempFile,sText);
	int iRes=(int)ShellExecute(pWnd?pWnd->GetSafeHwnd():NULL,"print",sTempFile,NULL,NULL, SW_SHOWMINIMIZED);
	return (iRes>32);
}

#ifdef _PRINT_CASES
BOOL PrintWindow_Explorer(CWnd* pWnd, CString sText, CString sTitle)
{_XLOG_
	CDataXMLSaver::Str2Xml(sText,0);
	sText.Replace("\n","<br>");
	sText=CString("<HTML><BODY>")+sText+"</BODY></HTML>";
	CString sTempFile=getFileFullName(DEF_LASTPRINTFILE);
	sTempFile+=".html";
	SaveFile(sTempFile,sText);
	// Готовим реестр
	BOOL bRestoreReg=TRUE;
	CString sFooter=DEF_IMPO_FOOTER,sHeader=DEF_IMPO_HEADER;
	CString sMyFooter="Page &p of &P",sMyHeader=TrimMessage(sTitle);
	{_XLOG_
		CRegKey key;
		if(key.Open(HKEY_CURRENT_USER, DEF_EXPLPRINTERSETS)!=ERROR_SUCCESS){_XLOG_
			key.Create(HKEY_CURRENT_USER, DEF_EXPLPRINTERSETS);
		}
		if(key.m_hKey!=NULL){_XLOG_
			DWORD dwType,dwSize,dwCount;
			dwType=0;
			dwSize=0;
			char szValue[1024]="";
			dwCount=sizeof(szValue);
			if(RegQueryValueEx(key.m_hKey,"footer",NULL, &dwType,(LPBYTE)szValue, &dwCount)==ERROR_SUCCESS){_XLOG_
				sFooter=szValue;
				RegSetValueEx(key.m_hKey,"footer",0,REG_SZ,(BYTE*)(LPCSTR)sMyFooter,lstrlen(sMyFooter)+1);
			}
			dwType=0;
			dwSize=0;
			szValue[0]=0;
			dwCount=sizeof(szValue);
			if(RegQueryValueEx(key.m_hKey,"header",NULL, &dwType,(LPBYTE)szValue, &dwCount)== ERROR_SUCCESS){_XLOG_
				sHeader=szValue;
				RegSetValueEx(key.m_hKey,"header",0,REG_SZ,(BYTE*)(LPCSTR)sMyHeader,lstrlen(sMyHeader)+1);
			}
		}
	}
	//
	BOOL bRes=(int(::ShellExecute(pWnd->GetSafeHwnd(),"print",sTempFile,NULL,NULL,SW_SHOWNORMAL))>32);
	// Восстанавливаем реестр
	{_XLOG_
		Sleep(1000);// Ждем чтобы все считалось
		CRegKey key;
		if(key.Open(HKEY_CURRENT_USER, DEF_EXPLPRINTERSETS)==ERROR_SUCCESS && key.m_hKey!=NULL){_XLOG_
			DWORD dwType=REG_SZ,dwSize=0;
			if(sFooter!=DEF_IMPO_FOOTER){_XLOG_
				RegSetValueEx(key.m_hKey,"footer",0,REG_SZ,(BYTE*)(LPCSTR)sFooter,lstrlen(sFooter)+1);
			}
			if(sHeader!=DEF_IMPO_HEADER){_XLOG_
				RegSetValueEx(key.m_hKey,"header",0,REG_SZ,(BYTE*)(LPCSTR)sHeader,lstrlen(sHeader)+1);
			}
		}
	}
	//Удалять нельзя, т.к. он должен существовать на момент печати
	//DeleteFile(sTempFile);
	return bRes;
}

#include "PrivatePrint.h"
BOOL PrintWindow_Raw(CWnd* pWnd, CString sText, CString sTitle)
{_XLOG_
	// А это если печатать напрямую...
	sText.Replace("\t","    ");
	//
	CDC dc; 
	DOCINFO di; 
	CPrintDialog dlg(FALSE); 
	
	dlg.GetDefaults(); 
	dc.Attach(dlg.GetPrinterDC()); 
	dc.SetMapMode(MM_TEXT);
	::ZeroMemory (&di, sizeof (DOCINFO)); 
	di.cbSize = sizeof (DOCINFO); 
	di.lpszDocName = sTitle;
	
	if(dc.StartDoc(&di)<0){_XLOG_
		return FALSE;
	}
	dc.StartPage();
	CPoint rtMargs(dc.GetDeviceCaps(PHYSICALOFFSETX), dc.GetDeviceCaps(PHYSICALOFFSETY));
	dc.DPtoLP(&rtMargs,1);
	CRect rt(0,0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));
	dc.DPtoLP(&rt);
	rt.DeflateRect(rtMargs.x,rtMargs.y);
	//SelectClipRgn
	//dc.SelectObject(pWnd->GetFont());
	dc.DrawText(sText,&rt,0);
	//rt.DeflateRect(50,50);dc.DrawFocusRect(rt);
	dc.EndPage(); 
	dc.EndDoc();
	return TRUE;
}

BOOL PrintWindow_Priv(CWnd* pWnd, CString sText, CString sTitle)
{_XLOG_
	// А это если печатать через сторонний класс...
	CPrivatePrint	prt;
	prt.Dialog();
	if(prt.StartPrint(sTitle)<0){_XLOG_
		return FALSE;
	}
	LOGFONT logFont;
	CFont* pFont=pWnd->GetFont();
	if(!pFont->GetLogFont(&logFont)){_XLOG_
		return FALSE;
	}
	HPRIVATEFONT hFont = prt.AddFontToEnvironment(logFont.lfFaceName);
	prt.SetMargins(50,130,50,10);
	prt.SetDistance(2);
	//prt.ActivateHF(HeaderFooter);
	prt.StartPage();
	int iFrom=0;
	int iFileLen=strlen(sText);
	while(iFrom>=0 && iFrom<iFileLen){_XLOG_
		int iPos=sText.Find("\n",iFrom);
		if(iPos==-1){_XLOG_
			iPos=strlen(sText);
		}else{_XLOG_
			iPos=iPos+1;
		}
		CString sLine=sText.Mid(iFrom,iPos-iFrom);
		sLine.TrimRight();
		prt.Print(hFont,sLine,FORMAT_NORMAL);
		iFrom=iPos;
	}
	prt.EndPage();
	prt.EndPrint();
	return TRUE;
}
#endif

BOOL PrintWindow(CWnd* pWnd, CString sText, CString sTitle)
{_XLOG_
	if(!pWnd){_XLOG_
		return FALSE;
	}
	if(sTitle==""){_XLOG_
		sTitle=TrimMessage(sText,10);
	}
	if(sTitle==""){_XLOG_
		sTitle="Unknown";
	}
	HCURSOR hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
	//BOOL bRes=PrintWindow_Explorer(pWnd,sText,sTitle);
	BOOL bRes=PrintWindow_System(pWnd,sText,sTitle);
	SetCursor(hCur);
	return bRes;
}

// dwFrom: 0 - bytes, 1-Kb, 2-Mb
const DWORD dwBytesInMB=1024*1024;
CString GetSizeStrFrom(DWORD dwSize, DWORD dwFrom)
{_XLOG_
	if(dwFrom==1){_XLOG_
		dwSize*=1024;
	}
	if(dwFrom==2){_XLOG_
		dwSize*=dwBytesInMB;
	}
	// Переводим в подходящий размер
	CString sOut=Format("%ib",dwSize);
	if(dwSize>=dwBytesInMB){_XLOG_
		sOut=Format("%.2fMb",double(dwSize)/dwBytesInMB);
	}else if(dwSize>=1024){_XLOG_
		sOut=Format("%.2fKb",double(dwSize)/1024);
	}
	return sOut;
}

/*
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
*/
CRect GetMainMonitorRECT()
{_XLOG_
	CRect rDesktopRECT(GetSystemMetrics(76),GetSystemMetrics(77),GetSystemMetrics(78),GetSystemMetrics(79));
	return rDesktopRECT;
}

void SetWindowSize(CWnd* pThis, CSize sx, UINT dwControlID)
{_XLOG_
	if(dwControlID!=0){_XLOG_
		pThis->GetDlgItem(dwControlID)->SetWindowPos(NULL,0,0,sx.cx,sx.cy,SWP_NOZORDER|SWP_NOMOVE);
	}else{_XLOG_
		pThis->SetWindowPos(NULL,0,0,sx.cx,sx.cy,SWP_NOZORDER|SWP_NOMOVE);
	}
}

void SetButtonSize(CWnd* pThis, UINT dwControlID, CSize sx)
{_XLOG_
	SetWindowSize(pThis, sx, dwControlID);
}

void SetButtonWidth(CWnd* pThis, UINT dwControlID, long sx)
{_XLOG_
	CRect rt;
	if(dwControlID!=0){_XLOG_
		pThis->GetDlgItem(dwControlID)->GetWindowRect(&rt);
	}else{_XLOG_
		pThis->GetWindowRect(&rt);
	}
	rt.right=rt.left+sx;
	SetWindowSize(pThis, rt.Size(), dwControlID);
}

void SetButtonHeight(CWnd* pThis, UINT dwControlID, long sy)
{_XLOG_
	CRect rt;
	if(dwControlID!=0){_XLOG_
		pThis->GetDlgItem(dwControlID)->GetWindowRect(&rt);
	}else{_XLOG_
		pThis->GetWindowRect(&rt);
	}
	rt.bottom=rt.top+sy;
	SetWindowSize(pThis, rt.Size(), dwControlID);
}

void SetButtonHeight(CWnd* pThis, UINT dwControlID, UINT iFrom)
{_XLOG_
	CRect rt;
	if(dwControlID!=0){_XLOG_
		pThis->GetDlgItem(dwControlID)->GetWindowRect(&rt);
	}else{_XLOG_
		pThis->GetWindowRect(&rt);
	}
	CRect rtFrom;
	if(iFrom!=0){_XLOG_
		pThis->GetDlgItem(iFrom)->GetWindowRect(&rtFrom);
	}else{_XLOG_
		pThis->GetWindowRect(&rtFrom);
	}
	rt.bottom=rt.top+rtFrom.Height();
	SetWindowSize(pThis, rt.Size(), dwControlID);
}

void SetButtonWidth(CWnd* pThis, UINT dwControlID, UINT iFrom)
{_XLOG_
	CRect rt;
	if(dwControlID!=0){_XLOG_
		pThis->GetDlgItem(dwControlID)->GetWindowRect(&rt);
	}else{_XLOG_
		pThis->GetWindowRect(&rt);
	}
	CRect rtFrom;
	if(iFrom!=0){_XLOG_
		pThis->GetDlgItem(iFrom)->GetWindowRect(&rtFrom);
	}else{_XLOG_
		pThis->GetWindowRect(&rtFrom);
	}
	rt.right=rt.left+rtFrom.Width();
	SetWindowSize(pThis, rt.Size(), dwControlID);
}

CFont* CreateFontFromStr(CString sSchValue, CString* pName, DWORD* dwBgColorOut, DWORD* dwTxColorOut, DWORD* dwAlphaOut, DWORD* dwAutoTransOut)
{_XLOG_
	CDataXMLSaver StyleData(sSchValue);
	CString sName="";
	StyleData.getValue("name",sName,pName?(*pName):"font-type");
	DWORD dwAlpha=0;
	StyleData.getValue("alpha",dwAlpha,dwAlphaOut?(*dwAlphaOut):100);
	DWORD dwAutoTrans=0;
	StyleData.getValue("autotrans",dwAutoTrans,dwAutoTransOut?(*dwAutoTransOut):0);
	DWORD dwTxtColor=0;
	StyleData.getValue("txtcolor",dwTxtColor,dwTxColorOut?(*dwTxColorOut):RGB(0,0,0));
	DWORD dwBgColor=0;
	StyleData.getValue("bgcolor",dwBgColor,dwBgColorOut?(*dwBgColorOut):RGB(255,255,255));
	CString sFontName="";
	StyleData.getValue("fontname",sFontName,"Arial");
	int iSize=0;
	StyleData.getValue("size",iSize,-27);
	DWORD dwWeight=0;
	StyleData.getValue("weight",dwWeight,400);
	int iCharset=0;
	StyleData.getValue("charset",iCharset,GetUserDefaultLangID()==0x0419?RUSSIAN_CHARSET:DEFAULT_CHARSET);
	CString sEffects;
	StyleData.getValue("effects",sEffects);
	BOOL bItalic=(sEffects.Find("i")!=-1);
	BOOL bUnderL=(sEffects.Find("u")!=-1);
	BOOL bStrike=(sEffects.Find("s")!=-1);
	CFont* font=new CFont();
	font->CreateFont(iSize,0,0,0,dwWeight,bItalic,bUnderL,bStrike,iCharset,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,sFontName);
	if(pName)
		*pName=sName;
	if(dwBgColorOut)
		*dwBgColorOut=dwBgColor;
	if(dwTxColorOut)
		*dwTxColorOut=dwTxtColor;
	if(dwAlphaOut)
		*dwAlphaOut=dwAlpha;
	if(dwAutoTransOut)
		*dwAutoTransOut=dwAutoTrans;
	return font;
}

CString CreateStrFromFont(CFont* pFont, CString sName, DWORD dwBgColorOut, DWORD dwTxColorOut, DWORD dwAlphaOut, DWORD dwAutoDisp, BOOL bInCuttedFormat)
{_XLOG_
	if(pFont==NULL){_XLOG_
		return "";
	}
	CString sSchValue;
	CDataXMLSaver StyleData(sSchValue,"",FALSE);
	//
	LOGFONT font;
	pFont->GetLogFont(&font);
	int PointSize=font.lfHeight;
	DWORD dwWeight=font.lfWeight;
	CString sEffects="";
	if(font.lfItalic>0){_XLOG_
		sEffects+="i";
	}
	if(font.lfUnderline>0){_XLOG_
		sEffects+="u";
	}
	if(font.lfStrikeOut>0){_XLOG_
		sEffects+="s";
	}
	if(sName!=""){_XLOG_
		StyleData.putValue("name",sName);
	}
	if(!bInCuttedFormat){_XLOG_
		StyleData.putValue("alpha",dwAlphaOut);
		StyleData.putValue("autotrans",dwAutoDisp);
		StyleData.putValue("txtcolor",dwTxColorOut);
		StyleData.putValue("bgcolor",dwBgColorOut);
	}
	StyleData.putValue("fontname",font.lfFaceName);
	StyleData.putValue("size",PointSize);
	StyleData.putValue("weight",dwWeight);
	StyleData.putValue("charset",font.lfCharSet);
	if(sEffects!=""){_XLOG_
		StyleData.putValue("effects",sEffects);
	}
	return StyleData.GetResult();
}

BOOL GetTextBoundingRect(HDC    hDC,   // Reference DC
                           int    x,     // X-Coordinate
                           int    y,     // Y-Coordinate
                           LPSTR  lpStr, // The text string to evaluate
                           DWORD  dwLen, // The length of the string 
                           LPRECT lprc)  // Holds bounding rectangle
{_XLOG_
	LPPOINT lpPoints; 
	LPBYTE lpTypes;
	int i, iNumPts;
	
	// Draw the text into a path
	BeginPath(hDC);
	i = SetBkMode(hDC, TRANSPARENT);
	TextOut(hDC, x, y, lpStr, dwLen);
	SetBkMode(hDC, i);
	EndPath(hDC);
	
	// How many points are in the path
	iNumPts = GetPath(hDC, NULL, NULL, 0);
	if (iNumPts == -1) return FALSE;
	
	// Allocate room for the points
	lpPoints = (LPPOINT)GlobalAlloc(GPTR, sizeof(POINT) * iNumPts);
	if (!lpPoints) return FALSE;
	
	// Allocate room for the point types
	lpTypes = (LPBYTE)(GlobalAlloc(GPTR, iNumPts));
	if (!lpTypes) {_XLOG_
		GlobalFree(lpPoints);
		return FALSE;
	}
	
	// Get the points and types from the current path
	iNumPts = GetPath(hDC, lpPoints, lpTypes, iNumPts);
	
	// Even more error checking
	if (iNumPts == -1) {_XLOG_
		GlobalFree(lpTypes);
		GlobalFree(lpPoints);
		return FALSE;
	}
	
	// Initialize the rectangle
	SetRect(lprc, 0xFFFFF, 0xFFFFF, 0, 0);
	
	// Get the maximum/minimum extents from the path data
	for (i=0; i<iNumPts; i++) {_XLOG_
		if (lpPoints[i].x > lprc->right)  lprc->right  = lpPoints[i].x;
		if (lpPoints[i].y > lprc->bottom) lprc->bottom = lpPoints[i].y;
		if (lpPoints[i].x < lprc->left)   lprc->left   = lpPoints[i].x;
		if (lpPoints[i].y < lprc->top)    lprc->top    = lpPoints[i].y;
	}
	
	GlobalFree(lpTypes);
	GlobalFree(lpPoints);
	
	return TRUE;
}

typedef BOOL (STDAPICALLTYPE * _MakeSureDirectoryPathExists)(PCSTR);
class CDbgHelp
{
public:
	HINSTANCE hDll;
	_MakeSureDirectoryPathExists fpMakeSureDirectoryPathExists;
	CDbgHelp()
	{_XLOG_
		fpMakeSureDirectoryPathExists=0;
		hDll=LoadLibrary("dbghelp.dll");
		if(hDll){_XLOG_
			fpMakeSureDirectoryPathExists=(_MakeSureDirectoryPathExists)DBG_GETPROC(hDll,"MakeSureDirectoryPathExists");
		}
	}
	~CDbgHelp()
	{_XLOG_
		if(hDll){_XLOG_
			FreeLibrary(hDll);
		}
	}
};

CDbgHelp& _DbgHelp()
{_XLOG_
	static CDbgHelp obj;
	return obj;
}

BOOL CreateDirIfNotExist(const char* szFile)
{_XLOG_
	CString sKatalog=GetDirectoryFromPath(szFile);
	if(!isFileExist(sKatalog)){_XLOG_
		if(_DbgHelp().fpMakeSureDirectoryPathExists){_XLOG_
			return _DbgHelp().fpMakeSureDirectoryPathExists(sKatalog);
		}
		return ::CreateDirectory(sKatalog,NULL);
	}
	return TRUE;
}

CString GetDirectoryFromPath(const char* szFile)
{_XLOG_
	char szDisk[MAX_PATH]="",szPath[MAX_PATH]="";
	_splitpath(szFile,szDisk,szPath,NULL,NULL);
	CString sKatalog=szDisk;
	sKatalog+=szPath;
	return sKatalog;
}

CString GetFileFromPath(const char* szFile)
{_XLOG_
	char szName[MAX_PATH]="",szExt[MAX_PATH]="";
	_splitpath(szFile,NULL,NULL,szName,szExt);
	CString sFileName=szName;
	sFileName+=szExt;
	return sFileName;
}

BOOL AdoptSizeToScreen(CSize& size)
{_XLOG_
	static CRect rDesktopRECT(0,0,0,0);
	if(rDesktopRECT.IsRectEmpty()){_XLOG_
		SystemParametersInfo(SPI_GETWORKAREA,0,&rDesktopRECT,0);
	}
	if(size.cx>rDesktopRECT.Width()*0.8){_XLOG_
		size.cx=long(rDesktopRECT.Width()*0.8);
	}
	if(size.cy>rDesktopRECT.Height()*0.8){_XLOG_
		size.cy=long(rDesktopRECT.Height()*0.8);
	}
	return TRUE;
}

long SimpleCalc(const char* szExpr)
{_XLOG_
	CString sExpression=szExpr;
	sExpression.TrimLeft();
	sExpression.TrimRight();
	CString sNumber=sExpression.SpanIncluding("0123456789");
	sExpression=sExpression.Mid(strlen(sNumber));
	double dRes=atol(sNumber);
	while(strlen(sExpression)>0){_XLOG_
		char cOperation=sExpression.GetAt(0);
		sExpression=sExpression.Mid(1);
		sNumber=sExpression.SpanIncluding("0123456789");
		sExpression=sExpression.Mid(strlen(sNumber));
		switch(cOperation){_XLOG_
		case '+':
			dRes+=atol(sNumber);
			break;
		case '-':
			dRes-=atol(sNumber);
			break;
		case '*':
			dRes*=atol(sNumber);
			break;
		case '/':
			dRes=dRes/atol(sNumber);
			break;
		}
	}
	return long(dRes);
}

#define FORMENU_LEN	25
CString trimTextForMenu(CString sText)
{_XLOG_
	if(sText==""){_XLOG_
		return _l("<empty>");
	}
	CString sOut;
	int iStopPrefix=sText.Find(BMP_SPECPREFIX);
	if(iStopPrefix!=-1){_XLOG_
		sOut=sText.Left(iStopPrefix);
	}else{_XLOG_
		sOut=sText.Left(FORMENU_LEN*2);
	}
	sOut.TrimLeft();
	sOut.Replace("&",".");
	sOut.Replace("\t",".");
	sOut.Replace("\r\n",".");
	sOut.Replace("\n",".");
	return TrimMessage(sOut,FORMENU_LEN,1);
}

/*CSIDL_COMMON_DOCUMENTS (0x002e)
CSIDL_COMMON_FAVORITES (0x001f)
CSIDL_DESKTOP 
CSIDL_MYDOCUMENTS*/

CString getDesktopPath()
{_XLOG_
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_DESKTOPDIRECTORY);
	return szFolderBuffer;
}

CString getMyDocPath()
{_XLOG_
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_PERSONAL);
	if(strlen(szFolderBuffer)==0){_XLOG_
		return "::{450D8FBA-AD25-11D0-98A8-0800361B1103}";
	}
	return szFolderBuffer;
}


CString getMyPicPath()
{_XLOG_
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,0x0036);// CSIDL_COMMON_PICTURES
	if(strlen(szFolderBuffer)==0){_XLOG_
		return getMyDocPath()+"\\Pictures";
	}
	return szFolderBuffer;
}

CString getEmailExePath()
{_XLOG_
	static CString sDefaultEmail="";
	if(sDefaultEmail==":("){_XLOG_
		return "";
	}
	if(sDefaultEmail==""){_XLOG_
		CRegKey key;
		sDefaultEmail=":(";
		key.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Clients\\Mail\\");
		char szTemp[MAX_PATH]="";
		DWORD lSize = MAX_PATH,dwType=0;
		if(RegQueryValueEx(key.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){_XLOG_
			if(strlen(szTemp)>0){_XLOG_
				CString sNewKeysVal="Software\\Clients\\Mail\\";
				sNewKeysVal+=szTemp;
				sNewKeysVal+="\\Shell\\open\\command\\";
				CRegKey key2;
				key2.Open(HKEY_LOCAL_MACHINE, sNewKeysVal);
				lSize = MAX_PATH,dwType=0;
				if(RegQueryValueEx(key2.m_hKey,"",NULL, &dwType,(LPBYTE)szTemp, &lSize)== ERROR_SUCCESS){_XLOG_
					if(szTemp[0]!=0){_XLOG_
						sDefaultEmail=szTemp;
						if(sDefaultEmail.Left(1)=="\"" && sDefaultEmail.Right(1)=="\""){_XLOG_
							sDefaultEmail=sDefaultEmail.Mid(1,sDefaultEmail.GetLength()-2);
						}
					}
				}
			}
		}
		if(sDefaultEmail==":("){_XLOG_
			HKEY hkey;
			LONG lres = ::RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("mailto\\shell\\open\\command"),0, KEY_READ, &hkey );
			if( lres == ERROR_SUCCESS ){_XLOG_
				DWORD dwType = 0, dwnBytes = MAX_PATH;
				_TCHAR buff[MAX_PATH] = {0};
				lres = ::RegQueryValueEx( hkey, NULL, 0, &dwType, (LPBYTE)buff, &dwnBytes );
				if( lres == ERROR_SUCCESS ){_XLOG_
					::RegCloseKey( hkey );
					if(buff[0]!=0){_XLOG_
						sDefaultEmail=buff;
					}
					int iLastp=sDefaultEmail.Find("%");
					if(iLastp!=-1){_XLOG_
						sDefaultEmail=sDefaultEmail.Left(iLastp);
					}
					sDefaultEmail.TrimLeft();
					sDefaultEmail.TrimRight();
				}
			}
		}
	}
	char szPathOut[MAX_PATH]="";
	strcpy(szPathOut,sDefaultEmail);
	DWORD dwDoEnv=DoEnvironmentSubst(szPathOut,sizeof(szPathOut));
	sDefaultEmail=szPathOut;
	return sDefaultEmail;
}

DWORD WINAPI RunDefaultEmailClient_InThread(LPVOID pData)
{_XLOG_
	long bRes=0;
	CString sDefaultEmail=getEmailExePath();
	if(strlen(sDefaultEmail)>0){_XLOG_
		DWORD dwStRes=(DWORD)::ShellExecute(NULL,"open",sDefaultEmail,NULL,NULL,SW_SHOWNORMAL);
		bRes=(dwStRes>32);
	}
	return bRes!=0;
}

BOOL RunDefaultEmailClient()
{_XLOG_
#ifdef FORK
	FORK(RunDefaultEmailClient_InThread,0);
#endif
	return TRUE;
}

CString MakeStandartLen(const char* szStr, int iLen, char c)
{_XLOG_
	CString sRes=szStr;
	if(sRes.GetLength()<iLen){_XLOG_
		sRes+=CString(c,iLen-sRes.GetLength());
	}else{_XLOG_
		sRes+=" ";
	}
	return sRes;
}

CString ConvertArrayToString(CStringArray& aStrings, char c)
{_XLOG_
	CString sRes;
	int iLen=aStrings.GetSize();
	if(iLen){_XLOG_
		sRes=aStrings[0];
		for(int i=1;i<iLen;i++){_XLOG_
			sRes+=c;
			sRes+=aStrings[i];
		}
	}
	return sRes;
}

int GetIndexInArray(const char* szItem, CStringArray* pArray,BOOL bInstring)
{_XLOG_
	for(int i=0;i<pArray->GetSize();i++){_XLOG_
		if((!bInstring && pArray->GetAt(i)==szItem) || (bInstring && pArray->GetAt(i).Find(szItem)!=-1)){_XLOG_
			return i;
		}
	}
	return -1;
}

#ifdef __AFXCMN_H__
int AddComboBoxExItem(CComboBoxEx* pCombo, int iCount, const char* szText, int iIcon)
{_XLOG_
	if(pCombo==NULL){_XLOG_
		return -1;
	}
	int iComboCount=pCombo->GetCount();
	if(iComboCount==CB_ERR){_XLOG_
		return -1;
	}
	BOOL bSet=TRUE;
	if(iCount>=iComboCount){_XLOG_
		bSet=FALSE;// Такого элемента еще нет
	}
	// Запихиваем в комбобокс
	COMBOBOXEXITEM pItem;
	pItem.mask=0;
	pItem.iItem=iCount;
	pItem.iIndent=0;
	pItem.lParam=0;
	if(szText!=NULL){_XLOG_
		pItem.pszText=(char*)szText;
		pItem.cchTextMax=strlen(szText);
		pItem.mask|=CBEIF_TEXT;
	}
	if(iIcon!=-1){_XLOG_
		pItem.mask|=CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_OVERLAY;
		pItem.iOverlay=pItem.iSelectedImage=pItem.iImage=iIcon;
	}
	if(bSet){_XLOG_
		return pCombo->SetItem(&pItem);
	}else{_XLOG_
		return pCombo->InsertItem(&pItem);// Здесь иногда падает!!! :(
	}
}
#endif

DWORD hexCode(const char* szRawValue)
{_XLOG_
	DWORD cRes;
	char szValue[3]="12";
	szValue[0]=szRawValue[0]>='a'?(szRawValue[0]-'a'+'A'):szRawValue[0];
	szValue[1]=szRawValue[1]>='a'?(szRawValue[1]-'a'+'A'):szRawValue[1];
	if (szValue[0]<='9')
		cRes=szValue[0]-'0';
	else
		cRes=10+szValue[0]-'A';
	cRes=cRes*16;
	if (szValue[1]<='9')
		cRes+=szValue[1]-'0';
	else
		cRes+=10+szValue[1]-'A';
	return cRes;
}

DWORD GetRGBFromHTMLString(CString sRGB)
{_XLOG_
	if(sRGB==""){_XLOG_
		return 0;
	}
	if(sRGB[0]!='#' || sRGB.GetLength()!=7){_XLOG_
		return atol(sRGB);
	}
	DWORD dwRes=RGB(hexCode(sRGB.Mid(1,2)),hexCode(sRGB.Mid(3,2)),hexCode(sRGB.Mid(5,2)));
	return dwRes;
}

void AddToDebugLog(const char* szText)
{_XLOG_
	COleDateTime dtNow=COleDateTime::GetCurrentTime();
	CString sTime=Format("%02lu.%02lu.%04lu&nbsp;%02lu:%02lu.%02lu (%lu)",dtNow.GetDay(),dtNow.GetMonth(),dtNow.GetYear(),dtNow.GetHour(),dtNow.GetMinute(),dtNow.GetSecond(),GetTickCount());
	CString sLog=sTime+"\t"+szText+"\r\n";
	CString sLogFile=Format("c:\\%s_debug.log",GetApplicationName());
	FILE* fLog=fopen(sLogFile,"a+");
	if(fLog){_XLOG_
		fprintf(fLog,"%s",sLog);
		fclose(fLog);
	}
}

CString GetRandomString(int iLen)
{_XLOG_
	char* szBuffer=new char[iLen+1];
	szBuffer[iLen]=0;
	for(int i=0;i<iLen;i++){_XLOG_
		szBuffer[i]=rnd('A','Z');
	}
	CString sRes=szBuffer;
	delete[] szBuffer;
	return sRes;
}

CString SetTextForUpdateTipText(CString sText, int iMaxLines, BOOL bTrimEnters)
{_XLOG_
	CString sOut;
	CString sOutFull=TrimMessage(sText,1000,0);
	sOutFull.Replace("\r","");
	// Убиваем повторяющиеся пустые строки
	int iLines=0;
	while(1){_XLOG_
		BOOL bNeedAdd=0;
		CString sLine=sOutFull.SpanExcluding("\n");
		if(sLine.GetLength()>0){_XLOG_
			sOutFull=sOutFull.Mid(sLine.GetLength()+((sLine.GetLength()<sOutFull.GetLength())?1:0));
		}else if(sOutFull.GetLength()>0 && sOutFull.GetAt(0)=='\n'){_XLOG_
			sOutFull=sOutFull.Mid(1);
		}
		if(bTrimEnters){_XLOG_
			sLine.TrimLeft();
			sLine.TrimRight();
		}
		if(iMaxLines>0 && iLines<iMaxLines){_XLOG_
			if(!bTrimEnters){_XLOG_
				bNeedAdd=TRUE;
			}else if(sLine.GetLength()>0){_XLOG_
				bNeedAdd=TRUE;
			}
		}else{_XLOG_
			sOutFull="";
			sLine="< "+_l("skipped")+" >";
			bNeedAdd=TRUE;
		}
		if(bNeedAdd){_XLOG_
			if(sOut.GetLength()>0){_XLOG_
				sOut+="\n";
			}
			sOut+=sLine;
			iLines++;
		}
		if(sOutFull==""){_XLOG_
			break;
		}
	}
	return sOut;
}

CString GetReadableStringFromMinutes(DWORD dwMinutes)
{_XLOG_
	CString sOut;
	DWORD dwHour=dwMinutes/60;
	DWORD dwMin=dwMinutes%60;
	if(dwHour>0){_XLOG_
		sOut+=Format("%lu%s ",dwHour,_l("h."));
	}
	if(dwMin>0){_XLOG_
		sOut+=Format("%lu%s",dwMin,_l("m."));
	}
	return sOut;
}

BOOL IsProgidInstalled(const char* szClsid)
{_XLOG_
	CString sKeyName="\\";
	sKeyName+=szClsid;
	CRegKey key;
	key.Open(HKEY_CLASSES_ROOT, sKeyName);
	if(key!=NULL){_XLOG_
		return TRUE;
	}
	return FALSE;
}


BOOL IsClsidInstalled(const char* szClsid)
{_XLOG_
	CString sKeyName="\\CLSID\\{";
	sKeyName+=szClsid;
	sKeyName+="}";
	CRegKey key;
	key.Open(HKEY_CLASSES_ROOT, sKeyName);
	if(key!=NULL){_XLOG_
		return TRUE;
	}
	return FALSE;
}

#include <mmsystem.h>
typedef WINMMAPI BOOL (WINAPI *_PlaySoundA)(IN LPCSTR pszSound, IN HMODULE hmod, IN DWORD fdwSound);
typedef WINMMAPI DWORD (WINAPI *_mciSendStringA)(IN LPCSTR lpstrCommand, OUT LPSTR lpstrReturnString, IN UINT uReturnLength, IN HWND hwndCallback);
typedef WINMMAPI DWORD (WINAPI *_mciSendCommandA)( IN UINT mciId, IN UINT uMsg, IN DWORD_PTR dwParam1, IN DWORD_PTR dwParam2);
CWinMM::CWinMM()
{_XLOG_
	hLib=LoadLibrary("winmm.dll");
}

CWinMM::~CWinMM()
{_XLOG_
	if(hLib){_XLOG_
		FreeLibrary(hLib);
	}
}

DWORD CWinMM::myMciSendCommand( IN UINT mciId, IN UINT uMsg, IN DWORD_PTR dwParam1, IN DWORD_PTR dwParam2)
{_XLOG_
	static _mciSendCommandA fp=0;
	if(fp==0 && hLib){_XLOG_
		fp=(_mciSendCommandA)DBG_GETPROC(hLib,"mciSendCommandA");
	}
	if(fp!=0){_XLOG_
		return (*fp)(mciId, uMsg, dwParam1, dwParam2);
	}
	return 0;
}


DWORD CWinMM::myMciSendString( IN LPCSTR lpstrCommand, OUT LPSTR lpstrReturnString, IN UINT uReturnLength, IN HWND hwndCallback)
{_XLOG_
	static _mciSendStringA fp=0;
	if(fp==0 && hLib){_XLOG_
		fp=(_mciSendStringA)DBG_GETPROC(hLib,"mciSendStringA");
	}
	if(fp!=0){_XLOG_
		return (*fp)(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
	}
	return 0;
}

BOOL CWinMM::myPlaySound(IN LPCSTR pszSound, IN HMODULE hmod, IN DWORD fdwSound)
{_XLOG_
	static _PlaySoundA fp=0;
	if(fp==0 && hLib){_XLOG_
		fp=(_PlaySoundA)DBG_GETPROC(hLib,"PlaySoundA");
	}
	if(fp!=0){_XLOG_
		return (*fp)(pszSound, hmod, fdwSound);
	}
	return 0;
}

CWinMM& getWinmm()
{_XLOG_
	static CWinMM wm;
	return wm;
}

void AsyncPlaySoundSys(const char* szSoundName)
{_XLOG_
	getWinmm().myPlaySound(szSoundName, NULL, SND_ALIAS | SND_NODEFAULT | SND_ASYNC | SND_NOWAIT);
	//::PlaySound(szSoundName, NULL, SND_ALIAS | SND_NODEFAULT );
}

HWND PlayMusicX(CString szSoundPath,BOOL bLoopSound, CString sAlias)
{_XLOG_
	CString sLow=szSoundPath;
	sLow.MakeLower();
	if(sLow.Find(".wav")!=-1){_XLOG_
		if(bLoopSound>=0){_XLOG_
			getWinmm().myPlaySound(szSoundPath,NULL,SND_FILENAME|SND_ASYNC|(bLoopSound?SND_LOOP:0));//SND_FILENAME|SND_NODEFAULT|SND_NOSTOP|SND_NOWAIT|SND_ASYNC - падает! :(
		}else{_XLOG_
			getWinmm().myPlaySound(NULL,NULL,SND_FILENAME|SND_ASYNC);
		}
	}else{_XLOG_
		CString sCommand;
		MCIERROR iErr=0;
		if(bLoopSound>=0){_XLOG_
			szSoundPath=CString("\"")+szSoundPath+CString("\"");
			sCommand=CString("open ")+szSoundPath+" alias "+sAlias;
			iErr=getWinmm().myMciSendString(sCommand, 0, 0, 0);
			if(iErr==0){_XLOG_
				sCommand=CString("play ")+sAlias+(bLoopSound?" repeat":"");
				iErr=getWinmm().myMciSendString(sCommand, 0, 0, 0);
				TRACE1("Playing %s...\n",szSoundPath);
			}
			//mciExecute(CString("play ")+szSoundPath);
		}else{_XLOG_
			MCIERROR iErr=getWinmm().myMciSendString(CString("close ")+sAlias, 0, 0, 0);
			TRACE1("Stopping %s...\n",szSoundPath);
		}
		/*
		HWND m_Video = hPrevInstance;
		if(m_Video != NULL)
		{_XLOG_
			//MCIWndHome(m_Video);
			MCIWndStop(m_Video);
			MCIWndDestroy(m_Video);
		}
		if(szSoundPath!=""){_XLOG_
			m_Video = MCIWndCreate(hParent,
				theApp.m_hInstance,
				(hParent?WS_CHILD:WS_POPUP)|MCIWNDF_NOMENU,szSoundPath);
			MCIWndPlay(m_Video);
		}
		return m_Video;
		*/
	}
	return 0;
}

#define ID_SIZE 30
#include <sys/timeb.h>
CString GenerateNewId(const char* szPrefix,const char* szPostfix)
{_XLOG_
	CString sRes;
	struct _timeb tstruct;
	_ftime(&tstruct);
	sRes.Format("%s%sI%02lxR%02x",szPrefix,szPostfix,WORD(rnd(0,0xFF)),(WORD)tstruct.millitm);
	if(strlen(sRes)>ID_SIZE){_XLOG_
		sRes=sRes.Left(ID_SIZE);
	}
	sRes.MakeUpper();
	return sRes;
};

BOOL SetRegString(HKEY& hOpenedKey, const char* szKeyName, CString sValue)
{_XLOG_
    return RegSetValueEx(hOpenedKey,szKeyName,0,REG_SZ,(BYTE*)((const char*) sValue),sValue.GetLength()+1)==ERROR_SUCCESS;
}

CString GetRegString(HKEY& hOpenedKey, const char* szKeyName, DWORD dwMaxSize)
{_XLOG_
	DWORD lSize=dwMaxSize+1;
	char szValue[3*MAX_PATH]={0};
	DWORD dwType=REG_SZ;
	if(RegQueryValueEx(hOpenedKey,szKeyName,0,&dwType,(LPBYTE)szValue,&lSize)!=ERROR_SUCCESS){_XLOG_
		dwType=REG_EXPAND_SZ;
		szValue[0]=0;
		RegQueryValueEx(hOpenedKey,szKeyName,0,&dwType,(LPBYTE)szValue,&lSize);
	}
	CString sOut=szValue;
	return sOut;
}


CSysCurBlinker::CSysCurBlinker(UINT hCursorFromRes, DWORD dwMinTime)
{_XLOG_
	hBackup=NULL;
	hBackup2=NULL;
	m_dwMinTime=dwMinTime;
	m_dwCurTime=GetTickCount();
	if(hCursorFromRes!=NULL){_XLOG_
		HCURSOR hCur=LoadCursor(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(hCursorFromRes));
		if(hCur){_XLOG_
			hBackup=CopyCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
			hBackup2=CopyCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_IBEAM)));
			SetSystemCursor(CopyCursor(hCur), 32512/*IDC_ARROW*/);
			SetSystemCursor(CopyCursor(hCur), 32513/*IDC_IBEAM*/);
		}
		DestroyCursor(hCur);
	}
}

CSysCurBlinker::~CSysCurBlinker()
{_XLOG_
	BOOL bRes=0;
	if(hBackup && hBackup2){_XLOG_
		if(m_dwMinTime>0 && GetTickCount()-m_dwCurTime<m_dwMinTime){_XLOG_
			Sleep(m_dwMinTime-(GetTickCount()-m_dwCurTime));
		}
		bRes=SetSystemCursor(CopyCursor(hBackup), 32512);
		bRes=SetSystemCursor(CopyCursor(hBackup2), 32513);
		DestroyCursor(hBackup);
		DestroyCursor(hBackup2);
	}
}

void AttachToWindowQueue(HWND hWin, BOOL bType)
{_XLOG_
	DWORD dwCurWinProcID=0;
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(hWin,&dwCurWinProcID);
	DWORD dwThisThread=GetCurrentThreadId();
	DWORD dwThisProcess=GetCurrentProcessId();
	AttachThreadInput(dwThisThread,dwCurWinThreadID,bType);
}

void ShowFileProps(const char* szFile)
{_XLOG_
	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei,sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = szFile;
	sei.lpVerb = "properties";
	sei.fMask = SEE_MASK_INVOKEIDLIST;
	BOOL bRes=::ShellExecuteEx(&sei);
}

#include <shlobj.h>
void ShowContextMenu(HWND hWnd, LPCTSTR pszPath, int x, int y)
{_XLOG_
   USES_CONVERSION;

   // Строим полное имя файла/каталога
   TCHAR tchFullPath[MAX_PATH];
   GetFullPathName(pszPath, sizeof(tchFullPath)/sizeof(TCHAR), tchFullPath, NULL);

   // Получаем интерфейс IShellFolder рабочего стола
   IShellFolder *pDesktopFolder;
   SHGetDesktopFolder(&pDesktopFolder);

   // Преобразуем заданный путь в LPITEMIDLIST
   LPITEMIDLIST pidl;
   pDesktopFolder->ParseDisplayName(hWnd, NULL, T2OLE(tchFullPath), NULL, &pidl, NULL);

   // Ищем последний идентификатор в полученном списке pidl
   LPITEMIDLIST pLastId = pidl;
   USHORT temp;
   while(1)
   {_XLOG_
       int offset = pLastId->mkid.cb;
       temp = *(USHORT*)((BYTE*)pLastId + offset);

       if(temp == 0)
           break;

       pLastId = (LPITEMIDLIST)((BYTE*)pLastId + offset);
   }
   
   // Получаем интерфейс IShellFolder родительского объекта для заданного файла/каталога
   // Примечание: родительский каталог идентифицируется списком pidl за вычетом последнего
   //             элемента, поэтому мы временно зануляем pLastId->mkid.cb, отрезая его от списка
   temp = pLastId->mkid.cb;
   pLastId->mkid.cb = 0;
   IShellFolder *pFolder;
   pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (void**)&pFolder);

   // Получаем интерфейс IContextMenu для заданного файла/каталога
   // Примечание: относительно родительского объекта заданный файл/каталог идентифицируется
   //             единственным элементом pLastId
   pLastId->mkid.cb = temp;
   IContextMenu *pContextMenu;
   pFolder->GetUIObjectOf(
      hWnd, 1, (LPCITEMIDLIST *)&pLastId, IID_IContextMenu, NULL, (void**)&pContextMenu);

   // Создаём меню
   HMENU hPopupMenu = CreatePopupMenu();

   // Заполняем меню
   pContextMenu->QueryContextMenu(hPopupMenu, 0, 1, 0x7FFF, 0);

   // Отображаем меню
   UINT nCmd = TrackPopupMenu(hPopupMenu,
      TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD|0x0001L/*TPM_RECURSE*/, x, y, 0, hWnd, 0);

   // Выполняем команду (если она была выбрана)
   if(nCmd)
   {_XLOG_
      CMINVOKECOMMANDINFO ici;
      ZeroMemory(&ici, sizeof(CMINVOKECOMMANDINFO));
      ici.cbSize = sizeof(CMINVOKECOMMANDINFO);

      ici.hwnd = hWnd;
      ici.lpVerb = MAKEINTRESOURCE(nCmd-1);
      ici.nShow = SW_SHOWNORMAL;

      pContextMenu->InvokeCommand(&ici);
   }

   // Получаем интерфейс IMalloc
   IMalloc *pMalloc;
   SHGetMalloc(&pMalloc);

   // Освобождаем память, выделенную для pidl
   pMalloc->Free(pidl);

   // Освобождаем все полученные интерфейсы
   pDesktopFolder->Release();
   pFolder->Release();
   pContextMenu->Release();
   pMalloc->Release();
   return;
}

DWORD GetFileChecksum(CString m_strFilePath)
{_XLOG_
	HANDLE hf;
	DWORD dwBuffer[4096];
	DWORD dwRead;
	ULONGLONG uhCount;
	
	uhCount = 3 & 0xFFFFFFFFFFFFFFFC; // Make 4 multiple and get ceil
	DWORD m_uhFileSize=GetFileSize(m_strFilePath);
	DWORD m_uhChecksumBytes = 0;
	DWORD m_dwChecksum=0;
	
	
	// Not all bytes summed
	hf = CreateFile(m_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	SetFilePointer(hf, m_uhChecksumBytes, (PLONG) &dwRead, FILE_BEGIN);
	while((m_uhChecksumBytes<uhCount) &&  // Bytes pending
		(ReadFile(hf, dwBuffer, MIN(sizeof(dwBuffer), (DWORD) uhCount), &dwRead, NULL)) &&
		(dwRead>0))
	{_XLOG_
		// Calc checksum for buffer
		for (UINT i=0;i<(dwRead >> 2);i++) {_XLOG_
            m_dwChecksum += dwBuffer[i];
		}
		if (dwRead & 0x3){_XLOG_
            // Calc Checksum of last dword padding with zeroes
            m_dwChecksum += dwBuffer[(dwRead >> 2)] & (0xFFFFFFFF >> ((4-(dwRead & 0x3)) << 3));
		}
		m_uhChecksumBytes += dwRead;
	}
	CloseHandle(hf);
	return m_dwChecksum;
}

#include <Cderr.h>
BOOL SaveFileFromHGlob(CString& sSavingPath,CString& szLastPathToSaveTo,HGLOBAL& hGlob,DWORD dwSize,BOOL bNeedDialog,DWORD dwOffset, CWnd* pParent)
{_XLOG_
	if(!pParent){_XLOG_
		pParent=GetAppWnd();
	}
	if(bNeedDialog){_XLOG_
		MakeSafeFileName(sSavingPath);
		sSavingPath=szLastPathToSaveTo+sSavingPath;
		CFileDialog dlg(FALSE, NULL, sSavingPath, 0, "All files (*.*)|*.*||", pParent);
		DWORD dwRes=dlg.DoModal();
		CFileDialog* pResDlg=&dlg;
		if(dwRes!=IDOK){_XLOG_
			DWORD dwErr=CommDlgExtendedError();
			if(dwErr==FNERR_INVALIDFILENAME){_XLOG_
				CFileDialog dlg2(FALSE, NULL, NULL, 0, "All files (*.*)|*.*||", pParent);
				dwRes=dlg2.DoModal();
				if(dwRes==IDOK){_XLOG_
					sSavingPath=dlg2.GetPathName();
				}
			}
		}else{_XLOG_
			sSavingPath=dlg.GetPathName();
		}
		if(dwRes!=IDOK){_XLOG_
			// Смотрим CANCEL или ошибка
			DWORD dwErr=CommDlgExtendedError();
			return dwErr==0?TRUE:FALSE;
		}
		char szDrive[MAX_PATH]="C:\\",szDir[MAX_PATH]="";
		_splitpath(sSavingPath, szDrive, szDir, NULL, NULL);
		szLastPathToSaveTo.Format("%s%s",szDrive,szDir);
	}
	DWORD dwAttribs=GetFileAttributes(sSavingPath);
	if(dwAttribs!=0xffffffff){_XLOG_
		// Файл существует!
		if(bNeedDialog && pParent->MessageBox(_l("Overwrite existing file")+"?",NULL,MB_YESNO)!=IDYES){_XLOG_
			return TRUE;
		}
		// Удаляем...
		DeleteFile(sSavingPath);
	}
	HANDLE hf = CreateFile(sSavingPath, GENERIC_WRITE, (DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE){_XLOG_
		return FALSE;
	}
	BOOL bRes=TRUE;
	DWORD dwWritten=0;
	char* pData=(char*)::GlobalLock(hGlob)+dwOffset;
	if (!WriteFile(hf, pData, dwSize, (LPDWORD) &dwWritten,  NULL)){_XLOG_
		bRes=FALSE;
	}
	if (!CloseHandle(hf)){_XLOG_
		bRes=FALSE;
	}
	if(dwWritten!=dwSize){_XLOG_
		bRes=FALSE;
	}
	::GlobalUnlock(hGlob);
	return bRes;
}

size_t mymemcpy(char* pTo, const char* pFrom, size_t howMuch)
{_XLOG_
	if(howMuch==0){_XLOG_
		pTo[0]=0;
		return 0;
	}
	ZeroMemory(pTo,howMuch);
	if(strlen(pFrom)<howMuch){_XLOG_
		memcpy(pTo,pFrom,strlen(pFrom));
	}else{_XLOG_
		memcpy(pTo,pFrom,howMuch);
	}
	return howMuch;
}

int GetWindowMenuHeight(CWnd* m_pMain)
{_XLOG_
	int iMenuH=0;
	if(m_pMain->GetMenu()){_XLOG_
		_MENUBARINFO pmbi;
		memset(&pmbi,0,sizeof(pmbi));
		pmbi.cbSize=sizeof(pmbi);
		static HINSTANCE hUser32=HINSTANCE(-1);
		if(hUser32==HINSTANCE(-1)){_XLOG_
			hUser32=GetModuleHandle("user32.dll");
		}
		if(hUser32){_XLOG_
			static _GetMenuBarInfo fp=_GetMenuBarInfo(-1);
			if(fp==_GetMenuBarInfo(-1)){_XLOG_
				fp=(_GetMenuBarInfo)DBG_GETPROC(hUser32,"GetMenuBarInfo");
			}
			if(fp){_XLOG_
				(*fp)(m_pMain->m_hWnd,0xFFFFFFFD,0,&pmbi);//OBJID_MENU
				iMenuH=(pmbi.rcBar.bottom-pmbi.rcBar.top);
			}
		}
	}
	return iMenuH;
}


int GetNearestStr(const char* szFrom, int iStartFrom, const char* szParts[], int* iIndex)
{_XLOG_
	int iRes=-1, i=0;
	const char* szSearchBase=szFrom+iStartFrom;
	while(szParts[i]!=0){_XLOG_
		const char* szFinded=strstr(szSearchBase,szParts[i++]);
		if(szFinded && szFinded-szSearchBase>iRes){_XLOG_
			iRes=szFinded-szSearchBase;
			if(iIndex){_XLOG_
				*iIndex=i;
			}
		}
	}
	return iRes>=0?(iRes+iStartFrom):iRes;
}

CString AddPathExtension(const char* szPath, const char* szExt)
{_XLOG_
	CString sRes=szPath;
	if(szExt!=0){_XLOG_
		if(size_t(sRes.GetLength())>strlen(szExt)){_XLOG_
			if(sRes.Right(strlen(szExt)).CompareNoCase(szExt)!=0){_XLOG_
				sRes+=szExt;
			}
		}
	}
	return sRes;
}

CString AppendPaths(CString sPath1, CString sPath2)
{_XLOG_
	sPath2.TrimLeft("\\/");
	return AddPathExtension(sPath1,"\\")+sPath2;
}


CString MakeRelativePath(const char* szFullPath,const char* szBasePath)
{_XLOG_
	char szOut[MAX_PATH] = "";
	if(_Shlwapi().fpMakeRelPath){_XLOG_
		if(_Shlwapi().fpMakeRelPath(szOut,szBasePath,FILE_ATTRIBUTE_DIRECTORY,szFullPath,FILE_ATTRIBUTE_NORMAL)){_XLOG_
			return szOut;
		}
	}
	return szFullPath;
}

//dd.mm.yy hh:24.ss
COleDateTime StringToTime(CString sTime)
{_XLOG_
	COleDateTime dtRes;
	if(sTime.GetLength()==17){_XLOG_
		int iY=0,iM=0,iD=0,ih=0,im=0,is=0;
		iD=atol(sTime.Mid(0,2));
		iM=atol(sTime.Mid(3,2));
		iY=2000+atol(sTime.Mid(6,2));
		ih=atol(sTime.Mid(9,2));
		im=atol(sTime.Mid(12,2));
		is=atol(sTime.Mid(15,2));
		dtRes.SetDateTime(iY,iM,iD,ih,im,is);
	}
	return dtRes;
}

BOOL DeleteFileUsingRBin(const char* sPath)
{_XLOG_
	SHFILEOPSTRUCT str;
	memset(&str,0,sizeof(str));
	str.hwnd=0;
	str.wFunc=FO_DELETE;
	char szPath[MAX_PATH+2]="";
	memset(&szPath,0,sizeof(szPath));
	strcpy(szPath,sPath);
	str.pFrom=szPath;
	str.pTo=NULL;
	str.fFlags=FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT;
	if(SHFileOperation(&str)!=0){_XLOG_
		return 0;
	}
	return 1;
}

BOOL IsFileInZip(const char* szFile)
{_XLOG_
	CString sURI=szFile;
	sURI.MakeLower();
	if(strstr(sURI,".zip\\")!=NULL
		|| strstr(sURI,".wsc\\")!=NULL){_XLOG_
		return TRUE;
	}else{_XLOG_
		return FALSE;
	}
}

BOOL IsFileURL(const char* szFile)
{_XLOG_
	CString sURI=szFile;
	sURI.MakeLower();
	if(strstr(sURI,".htm")!=NULL
		|| strstr(sURI,"http://")!=NULL
		|| strstr(sURI,"https://")!=NULL
		|| strstr(sURI,"file://")!=NULL
		|| strstr(sURI,"ftp://")!=NULL){_XLOG_
		return TRUE;
	}else{_XLOG_
		return FALSE;
	}
}

BOOL IsFileHTML(const char* szFile)
{_XLOG_
	CString sURI=szFile;
	sURI.MakeLower();
	if(strstr(sURI,".jpg")!=NULL
		|| strstr(sURI,".gif")!=NULL
		|| strstr(sURI,".bmp")!=NULL){_XLOG_
		return FALSE;
	}
	return IsFileURL(sURI);
}

BOOL& IsAppStopping()
{_XLOG_
	static BOOL bStopping=0;
	return bStopping;
}

CString GetNextPrefixedId(CString sKey,int iIndex)
{_XLOG_
	int iCount=iIndex;
	if(iIndex==-1){_XLOG_
		if(sKey.GetLength()>0 && sKey[0]=='['){_XLOG_
			iCount=atol(sKey.Mid(1))+1;
		}else{_XLOG_
			iCount=1;
		}
	}
	if(iCount==0){_XLOG_
		return sKey;
	}
	int iPos=sKey.Find("]");
	if(iPos!=-1){_XLOG_
		sKey=sKey.Mid(iPos+1);
	}
	return Format("[%i]%s",iCount,sKey);
}

BOOL IsWindowsSystemWnd(HWND hWin)
{_XLOG_
	char szClass[128]="";
	BOOL bClassRes=::GetClassName(hWin,szClass,sizeof(szClass));
	if(bClassRes>0){_XLOG_
		if(stricmp(szClass,"SHELLDLL_DefView")==0 
			|| stricmp(szClass,"WK_MAIN")==0 
			|| stricmp(szClass,"Progman")==0 
			|| stricmp(szClass,"SysTabControl32")==0 
			|| stricmp(szClass,"TrayNotifyWnd")==0 
			|| stricmp(szClass,"Shell_TrayWnd")==0 
			|| stricmp(szClass,"WorkerW")==0 || stricmp(szClass,"WorkerA")==0
			|| stricmp(szClass,"FakeDesktopWClass")==0 || stricmp(szClass,"FakeDesktopAClass")==0
			|| stricmp(szClass,"tooltips_class32")==0
			|| stricmp(szClass,"GLBSWizard")==0
			|| stricmp(szClass,"SysFader")==0){_XLOG_
			return TRUE;
		}
		char szTitle[128]="";
		BOOL bTitleRes=::GetWindowText(hWin,szTitle,sizeof(szTitle));
		if(bTitleRes>0){_XLOG_
			if(stricmp(szTitle,"MCI command handling window")==0 ||
				stricmp(szTitle,"MM Notify Callback")==0 ||
				stricmp(szTitle,"DDE Server Window")==0 ||
				stricmp(szTitle,"Program Manager")==0 ||
				strstr(szTitle,"OleMainThread")!=0){_XLOG_
				return TRUE;
			}
		}
	}
	HWND hParent=GetParent(hWin);
	return hParent?IsWindowsSystemWnd(hParent):FALSE;
}

BOOL isPressed(UINT iKey)
{_XLOG_
	if(iKey==VK_SHIFT && ::GetAsyncKeyState(VK_SHIFT)>=0)
		return 0;
	if(iKey!=VK_SHIFT && ::GetAsyncKeyState(VK_SHIFT)<0)
		return 0;
	if(iKey==VK_MENU && ::GetAsyncKeyState(VK_MENU)>=0)
		return 0;
	if(iKey!=VK_MENU && ::GetAsyncKeyState(VK_MENU)<0)
		return 0;
	if(iKey==VK_CONTROL && ::GetAsyncKeyState(VK_CONTROL)>=0)
		return 0;
	if(iKey!=VK_CONTROL && ::GetAsyncKeyState(VK_CONTROL)<0)
		return 0;
	if(::GetAsyncKeyState(iKey)>=0)
		return 0;
	return 1;
}

char* CUITranslationImpl::uit(const char* szUIText,const char* szUITextDefault)
{_XLOG_
	return strdup(_l(szUIText,szUITextDefault));
};

void CUITranslationImpl::freestr(char* sz)
{_XLOG_
	free(sz);
}

CUITranslationImpl* CUITranslationImpl::getInstance()
{_XLOG_
	static CUITranslationImpl lng;
	return &lng;
}

BOOL RegisterExtension(CString sExt, CString sDescription, CString sExeParam)
{_XLOG_
    // sDescription is the description key for you application
    sDescription.Replace(" ","");            
    sExt.Replace(" ","");                // sExt – extension to register
    if(sExt.IsEmpty()) return FALSE;
    if(sDescription.IsEmpty()) return FALSE;

    sExt.Replace(".","");
    sExt="."+sExt;

    CString str=GetCommandLine(); // Getting the application name
    CString app;
    int index=str.Find(":",0);
    index=str.Find(":",index+1);
    if(index==-1) app=str;
    else app=str.Mid(0,index-1);

    app.Replace("\"",""); 
    app+=" ";
	app+=sExeParam;
	app+="\"%1\"";
    HKEY hkey;
    RegOpenKeyEx(HKEY_CLASSES_ROOT,"",0,KEY_QUERY_VALUE,&hkey);
    DWORD dw;
    RegCreateKeyEx(hkey, sExt.operator LPCTSTR() , 0L, NULL,
            REG_OPTION_NON_VOLATILE, 
            KEY_ALL_ACCESS, 
            NULL,&hkey, &dw );
    CString key=sDescription;
    RegSetValueEx(hkey,"",0,REG_SZ,(BYTE *)key.operator LPCTSTR(),
               key.GetLength());
    RegCloseKey(hkey);

    RegOpenKeyEx(HKEY_CLASSES_ROOT,"",0,KEY_QUERY_VALUE,&hkey);
    RegCreateKeyEx (hkey, key, 0L, NULL,
            REG_OPTION_NON_VOLATILE, 
            KEY_ALL_ACCESS, 
            NULL, &hkey, &dw);
    RegCreateKeyEx (hkey, "shell",  0L, NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS, 
            NULL,&hkey,  &dw);
    RegCreateKeyEx (hkey, "open", 0L, NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS, 
            NULL,&hkey, &dw);
    RegCreateKeyEx (hkey, "command", 0L, NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS, 
            NULL,&hkey, &dw);
    RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)(LPCTSTR)app,app.GetLength());
    RegCloseKey(hkey);

    RegOpenKeyEx(HKEY_CLASSES_ROOT,key,0,KEY_QUERY_VALUE,&hkey);
    RegCreateKeyEx (hkey, "DefaultIcon", 0L, NULL,
            REG_OPTION_NON_VOLATILE, 
            KEY_ALL_ACCESS,
            NULL,&hkey, &dw);
    app.Replace(" \"%1\"",",0");
    RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)(LPCTSTR)app,app.GetLength());
    RegCloseKey(hkey);
    return TRUE;
}

BOOL ParseForTag(CString sBodyText,CString sTag,CString sAttr,CString sMask,CStringArray& aRes, CString sSourceURL)
{_XLOG_
	int iFrom=0;
	int iFrom2=0;
	CString sBodyTextCopy=sBodyText;
	sBodyText.MakeLower();
	sTag.MakeLower();
	sAttr.MakeLower();
	while(iFrom>=0 && iFrom<sBodyText.GetLength()){_XLOG_
		CString sTextLine=CDataXMLSaver::GetInstringPart(Format("<%s",sTag),">",sBodyText,iFrom,XMLNoConversion,sBodyTextCopy);
		if(iFrom<0){_XLOG_
			break;
		}
		if(sAttr!=""){_XLOG_
			CString sTextLineCopy=sTextLine;
			sTextLine.MakeLower();
			iFrom2=0;
			CString sTextLine2=CDataXMLSaver::GetInstringPart(Format("%s=\"",sAttr),"\"",sTextLine,iFrom2,XMLNoConversion,sTextLineCopy);
			if(sTextLine2==""){_XLOG_
				iFrom2=0;
				sTextLine2=CDataXMLSaver::GetInstringPart(Format("%s='",sAttr),"'",sTextLine,iFrom2,XMLNoConversion,sTextLineCopy);
			}
			if(sTextLine2==""){_XLOG_
				iFrom2=0;
				sTextLine2=CDataXMLSaver::GetInstringPart(Format("%s=",sAttr)," ",sTextLine,iFrom2,XMLNoConversion,sTextLineCopy);
			}
			if(sTextLine2!=""){_XLOG_
				sTextLine=sTextLine2;
			}
		}
		if(sSourceURL!=""){_XLOG_
			sTextLine=MakeBaseURL(sSourceURL,sTextLine);
		}
		sTextLine.TrimLeft();
		sTextLine.TrimRight();
		if(PatternMatchList(sTextLine,sMask,"|?|")){_XLOG_
			aRes.Add(sTextLine);
		}
	}
	return 1;
}

typedef BOOL (STDAPICALLTYPE* _InternetCombineUrl)(LPCTSTR lpszBaseUrl,LPCTSTR lpszRelativeUrl,LPTSTR lpszBuffer,LPDWORD lpdwBufferLength,DWORD dwFlags);
class WinInetWrapper
{
public:
	HINSTANCE hDll;
	_InternetCombineUrl fpComb;
	WinInetWrapper()
	{_XLOG_
		fpComb=0;
		hDll=LoadLibrary("wininet.dll");
		if(hDll){_XLOG_
			fpComb=(_InternetCombineUrl)DBG_GETPROC(hDll,"InternetCombineUrlA");
		}
	}
	~WinInetWrapper()
	{_XLOG_
		if(hDll){_XLOG_
			FreeLibrary(hDll);
		}
	}
};

WinInetWrapper& getWinInetWrapper()
{_XLOG_
	static WinInetWrapper instance;
	return instance;
}

CString MakeBaseURL(CString sSrc, CString sAdditional)
{_XLOG_
	if(sAdditional.Find("://")==-1){_XLOG_
		// Относительный!!!
		if(getWinInetWrapper().fpComb){_XLOG_
			char szVal[MAX_PATH*10]={0};
			char szBase[MAX_PATH*10]={0};
			strcpy(szBase,getURLHost(sSrc,1));
			char szRel[MAX_PATH*10]={0};
			strcpy(szRel,sAdditional);
			DWORD dwLen=sizeof(szVal);
			if((*getWinInetWrapper().fpComb)(szBase,szRel,szVal,&dwLen,0x20000000|0x02000000)){//ICU_NO_ENCODE|ICU_BROWSER_MODE
				sAdditional=szVal;
			}
		}
	}
	return sAdditional;
}

CString DumpCAr(CStringArray& a,CStringArray* b)
{_XLOG_
	CString res="";
	for(int i=0;i<a.GetSize();i++){_XLOG_
		res+=a[i];
		if(b){_XLOG_
			res+=" (";
			res+=(*b)[i];
			res+=")";
		}
		res+="\n";
	}
	return res;
}

CString DateFormat(COleDateTime tm,BOOL bShort)
{_XLOG_
	SYSTEMTIME EventTime;
	tm.GetAsSystemTime(EventTime);
	char szTmp[128]={0};
	GetDateFormat(LOCALE_USER_DEFAULT,bShort?DATE_SHORTDATE:DATE_LONGDATE,&EventTime,0,szTmp,sizeof(szTmp));
	return szTmp;
}

CString TimeFormat(COleDateTime tm,BOOL bDef, BOOL bNoSecs)
{_XLOG_
	SYSTEMTIME EventTime;
	tm.GetAsSystemTime(EventTime);
	char szTmp[128]={0};
	GetTimeFormat(LOCALE_USER_DEFAULT,(bDef?0:TIME_FORCE24HOURFORMAT)|(bNoSecs?TIME_NOSECONDS:0),&EventTime,0,szTmp,sizeof(szTmp));
	CString sRes=szTmp;
	/*if(sRes.Find("0)==3){_XLOG_
		sRes=CString("0")+sRes;
	}*/
	return sRes;
}

BOOL IsInOneHier(HWND hNewWindow,HWND hWin)
{_XLOG_
	GetWindowTopParent(hNewWindow);
	GetWindowTopParent(hWin);
	if(hWin==hNewWindow){_XLOG_
		return TRUE;
	}
	return FALSE;
}

int SelectFromMenu(CStringArray& aItems, HWND hParent)
{_XLOG_
	if(aItems.GetSize()==0){_XLOG_
		return -1;
	}
	HMENU mn=CreatePopupMenu();
	for(int i=0;i<aItems.GetSize();i++){_XLOG_
		char szTmp[35]={0};
		if(strlen(aItems[i])>30){_XLOG_
			memcpy(szTmp,aItems[i],30);
			strcat(szTmp,"...");
		}else{_XLOG_
			strcpy(szTmp,aItems[i]);
		}
		AppendMenu(mn,MF_STRING, i+1, szTmp);
	}
	POINT pos;
	GetCursorPos(&pos);
	CRect rDesktopRECT;
	rDesktopRECT.left=rDesktopRECT.top=-20;
	rDesktopRECT.right=rDesktopRECT.bottom=-10;
	HWND wndShotPhase2 = hParent;
	BOOL bDestroy=0;
	if(wndShotPhase2==0){_XLOG_
		bDestroy=1;
		wndShotPhase2 = ::CreateWindowEx(WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_POPUP, -200, -200, 50, 50, 0, 0, AfxGetApp()->m_hInstance, 0);
		::SetForegroundWindow(wndShotPhase2);
		PostMessage(wndShotPhase2,WM_NULL,0,0);
	}
	DWORD dwRetCode=TrackPopupMenu(mn, TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pos.x, pos.y, 0, wndShotPhase2 , NULL);
	if(bDestroy){_XLOG_
		DestroyWindow(wndShotPhase2);
	}
	if(dwRetCode){_XLOG_
		return dwRetCode-1;
	}
	return -1;
}

#define         MASKBITS                0x3F
#define         MASKBYTE                0x80
#define         MASK2BYTES              0xC0
#define         MASK3BYTES              0xE0
#define         MASK4BYTES              0xF0
#define         MASK5BYTES              0xF8
#define         MASK6BYTES              0xFC
void UTF8Encode2BytesUnicode(const WCHAR* input, CString& output)
{_XLOG_
	int i=0;
	while(input[i])
	{_XLOG_
		// 0xxxxxxx
		DWORD dw=(DWORD)input[i];
		if(dw < 0x80)
		{_XLOG_
			output+=(char)((byte)input[i]);
		}
		// 110xxxxx 10xxxxxx
		else if(dw < 0x800)
		{_XLOG_
			output+=(char)((byte)(MASK2BYTES | dw >> 6));
			output+=(char)((byte)(MASKBYTE | dw & MASKBITS));
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if(dw < 0x10000)
		{_XLOG_
			output+=(char)((byte)(MASK3BYTES | dw >> 12));
			output+=(char)((byte)(MASKBYTE | dw >> 6 & MASKBITS));
			output+=(char)((byte)(MASKBYTE | dw & MASKBITS));
		}
		i++;
	}
}

CString DelQuotesFromPath(const CString sPath, char symbol)
{
	if(sPath.GetLength()>0){
		if(sPath[0]==symbol && sPath[sPath.GetLength()-1]==symbol){
			return sPath.Mid(1,sPath.GetLength()-2);
		}
	}
	return sPath;
}

void AddSlashToPath(CString &sPath)
{_XLOG_
	if(sPath=="::{450D8FBA-AD25-11D0-98A8-0800361B1103}"){_XLOG_
		sPath=getMyDocPath();
	}
	if(sPath.Right(1)!="\\"){_XLOG_
		sPath+="\\";
	}
}

BOOL ParseLineForCookies(CString& sText)
{_XLOG_
	int iFrom=0;
	while(iFrom>=0){_XLOG_
		CString sLine=CDataXMLSaver::GetInstringPartGreedly("%SET:","%",sText,iFrom);
		sText.Replace(Format("%%SET:%s%%",sLine),"");
		if(sLine!=""){_XLOG_
			CString sKey=sLine;
			CString sVal="";
			int iPos=sLine.Find("=");
			if(iPos!=-1){_XLOG_
				sKey=sLine.Left(iPos);
				sVal=sLine.Mid(iPos+1);
			}
			getMCookies().SetAt(sKey,sVal);
		}
	}
	iFrom=0;
	while(iFrom>=0){_XLOG_
		CString sLine=CDataXMLSaver::GetInstringPartGreedly("%RESET:","%",sText,iFrom);
		sText.Replace(Format("%%RESET:%s%%",sLine),"");
		if(sLine!=""){_XLOG_
			getMCookies().RemoveKey(sLine);
		}
	}
	return TRUE;
}

BOOL IsVista()
{
	static BOOL bIsWinV=-1;
	if(bIsWinV==-1){
		bIsWinV=0;
		OSVERSIONINFOEX winfo; 
		ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
		winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
		if (GetVersionEx((OSVERSIONINFO *)&winfo)){ 
			bIsWinV=(winfo.dwMajorVersion>=6);
		} 
	}
	return bIsWinV;
}