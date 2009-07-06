//#include "stdafx.h"
#include <afxdisp.h>
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <atlconv.h>
#include <atlbase.h>
#include <afxtempl.h>
#include <afxmt.h>
#include <afxdlgs.h>
#include "_common.h"
#include "_externs.h"
BOOL ExecutePluginScript(const char* szText);
/*
malloc( FIELD_OFFSET(struct MyPointersArray, m_arr) +
        5 * sizeof(PVOID) );
  (int *)((size_t)(&object) + offsetof(TFoo, value)); // OK
выделять память с запасом +4!!! выравнивание!!!
*/

DWORD dwLastCopyDelay=0;
CCOMStringW GetSelectedTextInFocusWndW(BOOL bDoNotTolerateFailure);
CCOMStringW wsz=L"";
CCriticalSection csWsz;

void GetLastClipDataSet(WCHAR* w)
{// Может подергаться из разных потоков!
	SimpleLocker l(&csWsz);
	if(!w){
		wsz=L"";
	}else{
		wsz=w;
	}
	return;
}
CCOMStringW& GetLastClipData()
{
	return wsz;
}


CCOMStringW wszCC=L"";
void SavedClipContentSet(WCHAR* w)
{// Может подергаться из разных потоков!
	SimpleLocker l(&csWsz);
	if(!w){
		wszCC=L"";
	}else{
		wszCC=w;
	}
	return;
}
CCOMStringW& SavedClipContent()
{
	return wszCC;
}

void WaitWhileAllKeysAreFree(CIHotkey hk)
{_XLOG_
	__FLOWLOG;
	int iCount=0;
	BOOL bIgnoreDownOnly=0;
#ifdef _DEBUG
	CString sDebugTest;
#endif
	while(1){_XLOG_
		if(hk.Present()){_XLOG_
			CIHotkey hKur=getCurrentlyPressedKeys(FALSE);
			hKur.DropKeyClassFromHotkey(DOWNONLY_KEYS);
			if((hKur.Options() & ADDOPT_LOWLEVEL)==0){_XLOG_
				hk.Reset();
			}else if(!hKur.Present() || hKur.GetHKSimilarity(hk,FALSE,FALSE,TRUE)<=0){_XLOG_
				break;
			}
		}
		if(!hk.Present()){_XLOG_
			BOOL bC=0;
			BOOL bS=0;
			BOOL bM=0;
			BOOL bLW=0;
			BOOL bRW=0;
			BOOL bI=0;
			BOOL bI2=0;
			BOOL bcC=0;
			BOOL bcV=0;
			bC= (GetAsyncKeyState(VK_CONTROL)<0);
			bS= (GetAsyncKeyState(VK_SHIFT)<0);
			bM= (GetAsyncKeyState(VK_MENU)<0);
			bLW=(GetAsyncKeyState(VK_LWIN)<0);
			bRW=(GetAsyncKeyState(VK_RWIN)<0);
			bcC=(GetAsyncKeyState('C')<0);
			bcV=(GetAsyncKeyState('V')<0);
			bI=0;//(GetAsyncKeyState(VK_INSERT)<0);
			if(!bC && !bS && !bM && !bLW && !bRW && !bI && !bcC && !bcV){_XLOG_
				break;
			}
#ifdef _DEBUG
			sDebugTest=Format("Simpe=%i,%i,%i,%i,%i,%i,%i,%i",bC,bS,bM,bLW,bRW,bI,bcC,bcV);
#endif
		}
		if(iCount>500){_XLOG_
			break;
		}
#ifdef _DEBUG
		if(iCount>100 && hk.Present()){_XLOG_
			//DebugBreak();
			break;
		}
#endif
		iCount++;
		Sleep(50);
	}
}

void WaitWhileNoKeysArePressedAtAll()
{_XLOG_
	WaitWhileAllKeysAreFree();
/*
	int iCount=0;
	while(1){_XLOG_
		CIHotkey hk=getCurrentlyPressedKeys(FALSE);
		if(!hk.Present()){_XLOG_
			break;
		}
#ifndef _DEBUG
		if(iCount>500){_XLOG_
			break;
		}
#else
		CString hkn=GetIHotkeyName(hk);
		if(iCount>100){_XLOG_
			//DebugBreak();
			break;
		}
#endif
		iCount++;
		Sleep(50);
	}
*/
}

void PerformKBInit(HWND hTargetWnd)
{_XLOG_
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	//AfxMessageBox("FirstTime!");
	if(pMainDialogWindow!=NULL){_XLOG_
		::SetForegroundWindow(pMainDialogWindow->GetSafeHwnd());
		::SetActiveWindow(pMainDialogWindow->GetSafeHwnd());
		::SetFocus(pMainDialogWindow->GetSafeHwnd());
	}
	//if(hTargetWnd!=NULL)
	{_XLOG_
		::SetForegroundWindow(hTargetWnd);
		::SetActiveWindow(hTargetWnd);
		::SetFocus(hTargetWnd);
	}
}

//BOOL SendSequenceViaHook(HWND hTargetWnd,int iSeqNumber);
BOOL SendSequenceViaHook(HWND hTargetWnd,int iSeqNumber)
{_XLOG_
	return 0;
}

void SendNumlock(HWND hTargetWnd)
{_XLOG_
	if(objSettings.lDefaultSelQuality || 
		!SendSequenceViaHook(hTargetWnd,0)){_XLOG_
		::keybd_event(VK_NUMLOCK, 0x45,KEYEVENTF_EXTENDEDKEY|0,0);//::MapVirtualKey(VK_NUMLOCK,0)
		Sleep(20);
		::keybd_event(VK_NUMLOCK, 0x45,KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
		Sleep(20);
	}
}

long lCopyCountDistig;
BOOL SendCtrlC(BOOL m_bUseWMCOPY, HWND hTargetWnd=NULL)
{_XLOG_
	__FLOWLOG;
	LOGSTAMP
	CSmartLock lock(&csClipRecreation);
	if(pMainDialogWindow){_XLOG_
		LOGSTAMP
		CSmartLock lock(&csClipAttach);
		if(!pMainDialogWindow->clipWnd){
			return FALSE;
		}
		::ResetEvent(pMainDialogWindow->clipWnd->hClipChanged);
		lCopyCountDistig=pMainDialogWindow->clipWnd->lCopyCount;
	}
	if(hTargetWnd==NULL){_XLOG_
		hTargetWnd=objSettings.hFocusWnd;
	}
	SimpleTracker tr(objSettings.lWaitForAddclipInput);
	char szClass[128]="", szWnName[128]="";
	::GetClassName(hTargetWnd,szClass,sizeof(szClass));
	::GetWindowText(hTargetWnd,szWnName,sizeof(szWnName));
	strupr(szClass);
	BOOL bStopWmCopy=1;
	/*if(!bStopWmCopy &&
		(m_bUseWMCOPY || objSettings.bGetTextSelectionStyle==2 || strcmp(szClass,"EDIT")==0 || strstr(szClass,"RICHED")!=NULL))
	{_XLOG_
		FLOG2("Sending WM_COPY to '%s'/'%s'",szClass,szWnName);
		::SendMessage(hTargetWnd,WM_COPY,0,0);
		lCopyCountDistig++;//Принудительно
	}else*/
	{_XLOG_
		LOGSTAMP
		WaitWhileNoKeysArePressedAtAll();
		Sleep(20);
		FLOG("Sending Ctrl-C");
		BOOL bNumlockBlinck=0;
		if(objSettings.bGetTextSelectionStyle){//Ctrl-insert
			if(objSettings.lDefaultSelQuality || 
				!SendSequenceViaHook(hTargetWnd,1)){_XLOG_
				Sleep(5);
				::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),0,0);
				Sleep(5);
				::keybd_event(VK_INSERT, ::MapVirtualKey(VK_INSERT,0),KEYEVENTF_EXTENDEDKEY|0,0);
				Sleep(5);
				::keybd_event(VK_INSERT, ::MapVirtualKey(VK_INSERT,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
				Sleep(5);
				::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
				Sleep(5);
			}
			LOGSTAMP
		}else{//Ctrl-C
			if(objSettings.lDefaultSelQuality || 
				!SendSequenceViaHook(hTargetWnd,2)){_XLOG_
				Sleep(5);
				::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),0,0);
				Sleep(5);
				::keybd_event('C', ::MapVirtualKey('C',0),0,0);
				Sleep(5);
				::keybd_event('C', ::MapVirtualKey('C',0),KEYEVENTF_KEYUP,0);
				Sleep(5);
				::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP,0);
				Sleep(5);
			}
			LOGSTAMP
		}
	}
#ifdef _DEBUG
	LOGSTAMP1("Wait started")
#endif
	static long lDelay=700;
	long dwCopyDelay=GetTickCount();
	DWORD dwWaitRes=WAIT_OBJECT_0;
	if(lCopyCountDistig==pMainDialogWindow->clipWnd->lCopyCount)
	{
#ifdef _DEBUG
		//ShowBaloon("Waiting main thread...","WireKeys debug",10000);
#endif
		dwWaitRes=WaitForSingleObject(pMainDialogWindow->clipWnd->hClipChanged,objSettings.lDefaultSelQuality==0?lDelay+500:lDelay);
	}
#ifdef _DEBUG
	else{
		//ShowBaloon("Already get ok!!!....","WireKeys debug",9000);
		//Alert(Format("AlreadyOk %i=%i",lCopyCountDistig,pMainDialogWindow->lCopyCount));
	}
#endif
	if(dwWaitRes!=WAIT_OBJECT_0){_XLOG_
		if(lDelay<1500){_XLOG_
			lDelay+=200;
		}
		dwLastCopyDelay=0;
		LOGSTAMP1("Wait failed")
#ifdef _DEBUG
		ShowBaloon("Wait failed :(","WireKeys debug",10000);
#endif
		return FALSE;
	}
	dwLastCopyDelay=GetTickCount()-dwCopyDelay;
	LOGSTAMP1("Wait finished")
	return TRUE;
}

void SendCtrlV(BOOL m_bUseWMCOPY, HWND hTargetWnd=NULL)
{_XLOG_
	__FLOWLOG;
	LOGSTAMP
	CSmartLock lock(&csClipRecreation);
	if(hTargetWnd==NULL){_XLOG_
		hTargetWnd=objSettings.hFocusWnd;
	}
	DWORD dwTim1=0,dwTim2=0,dwTim3=0,dwTim4=0,dwTim5=0,dwTim6=0,dwTim7=0,dwTim8=0,dwTim9=0,dwTim10=0,dwTim11=0;
	dwTim7=GetTickCount();
	char szClass[128]="";
	::GetClassName(hTargetWnd,szClass,sizeof(szClass));
	strupr(szClass);
	//Bad: strcmp(szClass,"INTERNET EXPLORER_SERVER")==0 || 
	/*if(m_bUseWMCOPY || objSettings.bGetTextSelectionStyle==2 || strcmp(szClass,"EDIT")==0 || strstr(szClass,"RICHED")!=NULL){_XLOG_
		dwTim1=GetTickCount();
		::PostMessage(hTargetWnd,WM_PASTE,0,0);
		dwTim1=GetTickCount()-dwTim1;
	}else*/
	{_XLOG_
		dwTim2=GetTickCount();
		WaitWhileNoKeysArePressedAtAll();
		dwTim2=GetTickCount()-dwTim2;
		if(objSettings.bGetTextSelectionStyle){//Shift-insert
			BOOL bNumlockBlinck=0;
			if(//objSettings.lDefaultSelQuality==2 ||
				!SendSequenceViaHook(hTargetWnd,3)){_XLOG_
				Sleep(5);
				::keybd_event(VK_SHIFT, ::MapVirtualKey(VK_SHIFT,0),KEYEVENTF_EXTENDEDKEY|0,0);//KEYEVENTF_EXTENDEDKEY|
				Sleep(5);
				::keybd_event(VK_INSERT, ::MapVirtualKey(VK_INSERT,0),KEYEVENTF_EXTENDEDKEY|0,0);
				Sleep(5);
				::keybd_event(VK_INSERT, ::MapVirtualKey(VK_INSERT,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
				Sleep(5);
				::keybd_event(VK_SHIFT, ::MapVirtualKey(VK_SHIFT,0),KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
			}
			if(bNumlockBlinck==1){// Включаем NUMLOCK, если мы его же и выключили
			}
		}else{//Ctrl-V
			if(objSettings.lDefaultSelQuality || 
				!SendSequenceViaHook(hTargetWnd,4)){_XLOG_
				dwTim3=GetTickCount();
				Sleep(5);
				::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),0,0);
				Sleep(5);
				::keybd_event('V', ::MapVirtualKey('V',0),0,0);
				Sleep(5);
				::keybd_event('V', ::MapVirtualKey('V',0),KEYEVENTF_KEYUP,0);
				Sleep(5);
				::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
				Sleep(5);
				dwTim3=GetTickCount()-dwTim3;
			}
		}
	}
	dwTim7=GetTickCount()-dwTim7;
}

HWND GetLastFocusWindowByHook(HWND hWin)
{_XLOG_
	__FLOWLOG;
	HMODULE hPl=GetModuleHandle("wp_wtraits.wkp");
	if(hPl!=0){_XLOG_
		typedef HWND& (WINAPI *_GetLastFocusWnd)();
		_GetLastFocusWnd fp=(_GetLastFocusWnd)DBG_GETPROC(hPl,"GetLastFocusWnd");
		if(fp){_XLOG_
			HWND h=(*fp)();
			if(h!=0){_XLOG_
				return h;
			}
		}
	}
	if(::GetFocus()!=NULL){//Получаем окно с фокусом
		return ::GetFocus();
	}
	if(hWin){//Получаем окно с фокусом
		// Получаем элемент управления с фокусом
		MYGUITHREADINFO gui;
		DWORD dwCurWinProcID=0;
		DWORD dwCurWinThreadID=GetWindowThreadProcessId(hWin,&dwCurWinProcID);
		if(GetGUIThreadInfoMy(dwCurWinThreadID,&gui)){_XLOG_
			objSettings.hFocusWnd=gui.hwndFocus;
			if(gui.hwndCaret){_XLOG_
				return gui.hwndCaret;
			}
			if(gui.hwndActive){_XLOG_
				return gui.hwndActive;
			}
		}
	}
	return objSettings.hHotMenuWnd;
}

BOOL PrepareClipProcesses(BOOL& bUseWMCOPY, HWND hWin)
{_XLOG_
	__FLOWLOG;
	CSmartLock lock(&csClipRecreation);
	if(hWin==NULL){_XLOG_
		hWin=objSettings.hHotMenuWnd;
	}
	if(hWin==NULL){_XLOG_
		hWin=GetForegroundWindow();
	}
	LOGSTAMP
	DWORD dwCurWinProcID=0;
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(hWin,&dwCurWinProcID);
	BOOL bRes=TRUE;
	/*
	// Приаттачиваемся к процессу
	DWORD dwThisThread=GetCurrentThreadId();
	DWORD dwThisProcess=GetCurrentProcessId();
	BOOL bRes=FALSE;
	if(objSettings.lDefaultSelQuality<3){_XLOG_
		if(dwThisProcess!=dwCurWinProcID){_XLOG_
			LOGSTAMP
			!!!//bRes=AttachThreadInput(dwThisThread,dwCurWinThreadID,TRUE);
			LOGSTAMP
		}else{_XLOG_
			bRes=TRUE;
		}
	}else{_XLOG_
		bRes=TRUE;
	}*/
	LOGSTAMP
	objSettings.hFocusWnd=GetLastFocusWindowByHook(hWin);
	bUseWMCOPY=FALSE;
	LOGSTAMP
	return bRes;
}

BOOL FinishClipProcesses(HWND hWin)
{_XLOG_
	__FLOWLOG;
	if(hWin==NULL){_XLOG_
		hWin=objSettings.hFocusWnd;
	}
	BOOL bRes=TRUE;
	/*DWORD dwCurWinProcID=0;
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(hWin,&dwCurWinProcID);
	DWORD dwThisThread=GetCurrentThreadId();
	DWORD dwThisProcess=GetCurrentProcessId();
	if(dwThisProcess!=dwCurWinProcID){_XLOG_
		if(objSettings.lDefaultSelQuality<3){_XLOG_
			if(dwThisThread!=dwCurWinThreadID){_XLOG_
				!!!//bRes=AttachThreadInput(dwThisThread,dwCurWinThreadID,FALSE);
			}else{_XLOG_
				bRes=TRUE;
			}
		}else{_XLOG_
			bRes=TRUE;
		}
	}*/
	return bRes;
}

long lRestoreInProgress=0;
CCriticalSection csClipOperation;
DWORD WINAPI RestoreClipboard(LPVOID)
{_XLOG_
	__FLOWLOG;
	FLOG("Restoring clipboard");
	LOGSTAMP
	if(objSettings.lDefaultSelQuality<2){_XLOG_
		TRACETHREAD;
		if(SavedClipContent().GetLength()==0){_XLOG_
			return 0;
		}
		LOGSTAMP
		// Так как тут будут установки буфера
		// нам самим они совершенно ни к чему!
		SimpleTracker lNoGetBac0(lRestoreInProgress);
		SimpleTracker lNoGetBack(objSettings.lDisableClipboard);
		CSmartLock SL(&csClipOperation,TRUE);
		// Бывает что программа не сразу отпускает буфер обмена
		// подождем пока он освободится чтобы впихнуть старое значение
		BOOL bTGlob=0;
#ifdef _DEBUG
		ShowBaloon("","",0);
#endif
		if(!SetClipboardText(SavedClipContent().c_str(),bTGlob)){_XLOG_
#ifdef _DEBUG
				ShowBaloon("Restore clipboard - failed!","WireKeys debug",3000);
#endif
			Sleep(500);
			if(!SetClipboardText(SavedClipContent().c_str(),bTGlob)){_XLOG_
			}
		}
	}
	return 0;
}

long lPasteClipInSyncON=0;
long g_lChainedClipboardOperations=0;
DWORD WINAPI BackIconToNormal(LPVOID pData);
BOOL PasteClipInSync(const char* szText, BOOL bUseWMCOPY, HWND hTargetWnd=NULL, BOOL bViaGlobals=0)
{_XLOG_
	__FLOWLOG;
	if(!pMainDialogWindow || !pMainDialogWindow->clipWnd){_XLOG_
		return FALSE;
	}
	CSmartLock lock2(&csClipRecreation);
	BOOL bUseGlobals=((objSettings.lDefaultSelQuality==0) || bViaGlobals);
	HANDLE hMainT=0;
	ResetEvent(pMainDialogWindow->clipWnd->hClipChangedOut);
	BOOL bDelWChar=0;
	WCHAR* wszTextToSet=0;
	if(IsBMP(szText)){_XLOG_
		wszTextToSet=(WCHAR*)szText;
	}else{_XLOG_
		bDelWChar=1;
		int len=strlen(szText);
		wszTextToSet = new WCHAR[len+1];
		memset(wszTextToSet,0,(len+1)*sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP, 0, szText, -1, wszTextToSet, len);
	}
	LOGSTAMP
	BOOL bRes=TRUE;
	if(!SetClipboardText(wszTextToSet,bUseGlobals)){_XLOG_
		bRes=FALSE;
	}
	LOGSTAMP
	if(bRes){_XLOG_
		DWORD dwWaitRes=WAIT_OBJECT_0;
		SimpleTracker lLock(lPasteClipInSyncON);
		LOGSTAMP
		SendCtrlV(bUseWMCOPY,objSettings.hFocusWnd);
		LOGSTAMP
		if(objSettings.lDefaultSelQuality<2){_XLOG_
			__FLOWLOG
			// Ждем чтобы целевое приложение успело зацепить буфер
			if(!bUseGlobals){_XLOG_
				if(dwLastCopyDelay>500){_XLOG_
					dwLastCopyDelay=500;
				}
				FLOG1("Waiting %i...",dwLastCopyDelay);
				Sleep(dwLastCopyDelay+20);
				dwWaitRes=WAIT_OBJECT_0;
			}else{_XLOG_
				dwWaitRes=::WaitForSingleObject(pMainDialogWindow->clipWnd->hClipChangedOut,5000);
			}
		}
		LOGSTAMP
		if(dwWaitRes==WAIT_TIMEOUT){_XLOG_
			if(pMainDialogWindow){_XLOG_
				pMainDialogWindow->SetWindowIcon(2);
				FORK(BackIconToNormal,500);
			}
			bRes=FALSE;
		}
		LOGSTAMP
	}
	if(bDelWChar){_XLOG_
		delete[] wszTextToSet;
	}
	return bRes;
}

CString GetSelectedTextInFocusWnd(BOOL bDoNotTolerateFailure)
{_XLOG_
	__FLOWLOG;
	// Вытаскиваем текст...
	if(objSettings.lChTrayOnClips){_XLOG_
		pMainDialogWindow->PostMessage(SETTRAYICON,3,0);
	}
	CCOMStringW wszStr=GetSelectedTextInFocusWndW(bDoNotTolerateFailure);
	CString sRes="";
	if(wszStr.length()){_XLOG_
		if(IsBMP(wszStr.c_str())){_XLOG_
			sRes=(const char*)(wszStr.c_str());
		}else{_XLOG_
			sRes=wszStr.c_str();
		}
	}
	if(objSettings.lChTrayOnClips){_XLOG_
		pMainDialogWindow->PostMessage(SETTRAYICON,0,0);
	}
	return sRes;
}

long lCopyInProgress=0;
CCOMStringW GetSelectedTextInFocusWndW(BOOL bDoNotTolerateFailure)
{_XLOG_
	LOGSTAMP
	if(objSettings.lDisableClipboard){_XLOG_
		return L"";
	}
#ifdef _DEBUG
	ShowBaloon("Get selection...","WireKeys debug",10000);
#endif
	FLOG("Getting selection");
	SimpleTracker lNoGetBac0(lCopyInProgress);
	LOGSTAMP
	CCOMStringW wszText=L"";
	if(pMainDialogWindow){_XLOG_
		DWORD dwCurWinProcID=0;
		DWORD dwCurWinThreadID=GetWindowThreadProcessId(pMainDialogWindow->GetSafeHwnd(),&dwCurWinProcID);
		LOGSTAMP
		if(dwCurWinThreadID==GetCurrentThreadId()){_XLOG_
			AfxMessageBox("Big Error: Clip operations can`t be performed in the main window thread!!!");
#ifdef _DEBUG
			DebugBreak();
#endif
			return wszText;
		}
	}
	LOGSTAMP
	{// Делаем работу, защищенную крит. секцией
		CSmartLock SL(&csClipOperation,TRUE);
		// Запоминаем буфер...
		if(objSettings.lDefaultSelQuality<2){_XLOG_
			WCHAR* ws=GetClipboardText(TRUE,TRUE);
			SavedClipContentSet(ws);
			if(ws){
				delete[] ws;
			}
		}
		// Вытаскиваем все...
		BOOL bUseWMCOPY=FALSE;
		LOGSTAMP
		if(PrepareClipProcesses(bUseWMCOPY)){_XLOG_
			if(pMainDialogWindow){_XLOG_
				::ResetEvent(pMainDialogWindow->clipWnd->hClipChanged);
			}
			//::OpenClipboard(AfxWPGetMainWnd()->GetSafeHwnd()/*objSettings.hFocusWnd*/);
			//::EmptyClipboard();
			//::CloseClipboard();
			GetLastClipDataSet(L"");
			BOOL bSuccess=SendCtrlC(bUseWMCOPY);
			if(bSuccess || !bDoNotTolerateFailure){
				wszText=GetLastClipData();
				if(wszText.length()==0){_XLOG_
					WCHAR* wsz=GetClipboardText(FALSE,TRUE);
					if(wsz){_XLOG_
						wszText=wsz;
						delete[] wsz;
					}
				}
			}
			FLOG1("Cliptext: %s",CString(wszText.c_str()));
			FinishClipProcesses();
		}else{_XLOG_
			AsyncPlaySound(SND_ERROR);
		}
	}
	LOGSTAMP
	// Восстанавливаем буфер...
	if(objSettings.lDefaultSelQuality<2 && g_lChainedClipboardOperations==0){_XLOG_
		FORK_WAIT(RestoreClipboard,0,300);
	}
	return wszText;
}

HGLOBAL& GetGlobalClipText(BOOL);
void CheckAndDisablePayFeaturesIfNeeded();
long lPasteInProgress=0;
BOOL PutSelectedTextInFocusWnd(CString sText, HKL hSendLayout, BOOL lLeaveInClipOnPaste)
{_XLOG_
	__FLOWLOG;
	if(objSettings.lDisableClipboard){_XLOG_
		#ifdef _DEBUG
		ShowBaloon(Format("Put text in foreground - skipped [%i/%i/%i]!",lRestoreInProgress,lPasteInProgress,lCopyInProgress),"WireKeys debug",3000);
		#endif
		return FALSE;
	}
#ifdef _DEBUG
		ShowBaloon("Pasting...","WireKeys debug",10000);
#endif
	BOOL bRes=TRUE;
	FLOG("Putting selected text");
	LOGSTAMP
	{_XLOG_
		// Так как тут будут установки буфера
		// нам самим они совершенно ни к чему!
		SimpleTracker lNoGetBac0(lPasteInProgress);
		SimpleTracker lNoGetBack(objSettings.lDisableClipboard);
		// Начинаем операцию
		if(objSettings.lChTrayOnClips){_XLOG_
			pMainDialogWindow->PostMessage(SETTRAYICON,3,0);
		}
		if(pMainDialogWindow){_XLOG_
			DWORD dwCurWinProcID=0;
			DWORD dwCurWinThreadID=GetWindowThreadProcessId(pMainDialogWindow->GetSafeHwnd(),&dwCurWinProcID);
			if(dwCurWinThreadID==GetCurrentThreadId()){_XLOG_
				AfxMessageBox("Big Error: Clip operations can`t be performed in the main window thread!!!");
#ifdef _DEBUG
				DebugBreak();
#endif
				if(objSettings.lChTrayOnClips){_XLOG_
					pMainDialogWindow->PostMessage(SETTRAYICON,0,0);
				}
				return FALSE;
			}
		}
		LOGSTAMP
		BOOL bContinue=TRUE;
		if((objSettings.hFocusWnd && GetWindowClass(objSettings.hFocusWnd)=="ConsoleWindowClass")
		  || (objSettings.hHotMenuWnd && GetWindowClass(objSettings.hHotMenuWnd)=="ConsoleWindowClass")){
		  bContinue=FALSE;
		  SwitchToWindow(objSettings.hHotMenuWnd?objSettings.hHotMenuWnd:objSettings.hFocusWnd,1);
		  CString s="";
		  if(IsBMP(sText)){
			s="[Image]";
		  }else{
			s=sText;
		  }
		  s+="##SENDKEY_USE#!#";
		  ExecutePluginScript(s);
		}
		if(bContinue){// Делаем работу, защищенную крит. секцией
			CSmartLock SL(&csClipOperation,TRUE);
			// Запоминаем буфер...
			SimpleTracker lNoGetBac1(lPasteInProgress);
			if(objSettings.lDefaultSelQuality<2 && g_lChainedClipboardOperations==0){_XLOG_
				WCHAR* ws=GetClipboardText(TRUE,TRUE);
				SavedClipContentSet(ws);
				if(ws){
					delete[] ws;
				}
			}
			// Подготавливаем операцию...
			BOOL bUseWMCOPY=FALSE;
			LOGSTAMP
				if(PrepareClipProcesses(bUseWMCOPY)){_XLOG_
					SimpleTracker lNoGetBac2(lPasteInProgress);
					LOGSTAMP;
					// Вставляем текст...
					PasteClipInSync(sText, bUseWMCOPY, objSettings.hFocusWnd, 0);
					// Очищаем данные (если они успели передаться в буфер, GetGlobalClipText() вернет 0)
					if(GetGlobalClipText(0)){_XLOG_
						::GlobalFree(GetGlobalClipText(0));
						GetGlobalClipText(0)=NULL;
					}
					if(GetGlobalClipText(1)){_XLOG_
						::GlobalFree(GetGlobalClipText(1));
						GetGlobalClipText(1)=NULL;
					}
					LOGSTAMP;
					// Шлем раскладку...
					if(hSendLayout!=NULL){_XLOG_
						HWND hFocusParent=GetForegroundWindow();//objSettings.hFocusWnd;
						PostMessage(hFocusParent,WM_INPUTLANGCHANGEREQUEST,0,LPARAM(hSendLayout));
					}
					LOGSTAMP;
					FinishClipProcesses();
				}else{_XLOG_
					AsyncPlaySound(SND_ERROR);
					bRes=FALSE;
				}
		}
	}
	// Восстанавливаем буфер...
#ifdef _DEBUG
		ShowBaloon("","",0);
#endif
	if(objSettings.lDefaultSelQuality<2 && lLeaveInClipOnPaste==0){_XLOG_
		FORK_WAIT(RestoreClipboard,0,300);
	}
	CheckAndDisablePayFeaturesIfNeeded2();
	if(objSettings.lChTrayOnClips){_XLOG_
		pMainDialogWindow->PostMessage(SETTRAYICON,0,0);
	}
	LOGSTAMP
	return bRes;
}

char* szSkipChars="\r\n\t ";
BOOL ConvertSelected(HKL& hFrom, HKL& hTo, CString& sText, CString& sCopy)
{_XLOG_
	__FLOWLOG;
	if(hFrom==hTo || hFrom==NULL || hTo==NULL){_XLOG_
		return FALSE;
	}
	sCopy="";
	BYTE lKeyState[256];
	const char* pPos=sText;
	while(*pPos){_XLOG_
		if(strchr(szSkipChars,(*pPos))!=NULL){_XLOG_
			sCopy+=CHAR(*pPos);
		}else{_XLOG_
			WORD wBuffer=0;
			memset(lKeyState,0,sizeof(lKeyState));
			SHORT iVK=VkKeyScanEx(CHAR(*pPos),hFrom);
			int iVkeyCode=LOBYTE(iVK);
			int iKeysPress=HIBYTE(iVK);
			lKeyState[VK_SHIFT]=(iKeysPress & 1)<<7;
			lKeyState[VK_CONTROL]=(iKeysPress & 2)<<7;
			lKeyState[VK_MENU]=(iKeysPress & 4)<<7;
			if(iVkeyCode!=-1){_XLOG_
				if(ToAsciiEx(iVkeyCode, 0, (CONST PBYTE)lKeyState, &wBuffer, 0, hTo)>0){_XLOG_
					sCopy+=char(LOBYTE(wBuffer));
				}else{_XLOG_
					sCopy+=CHAR(*pPos);
				}
			}
		}
		pPos++;
	}
	return TRUE;
}

void SubstMyVariablesForQRun(CString& sParams, CQuickRunItem* pW, BOOL bJScriptSafe, BOOL bNoGetSelection, const char* szParAdditions, const char* szSelectionSubst)
{_XLOG_
	static DWORD dwC1=0;
	if(dwC1==0){_XLOG_
		dwC1=GetTickCount();
	}
	BOOL bNeedSelected=bNoGetSelection?0:((sParams.Find("%SELECTED")!=-1));
	bNeedSelected|=bNoGetSelection?0:((sParams.Find("%1")!=-1));
	BOOL bNeedEscaped=bNoGetSelection?0:((sParams.Find("%ESCAPED")!=-1)||(sParams.Find("%URL_SELECTED")!=-1));
	bNeedEscaped|=bNoGetSelection?0:(sParams.Find("%E_SELECTED")!=-1);
	BOOL bNeedParams=(sParams.Find("%PARAMETER")!=-1);
	if(bNeedSelected || bNeedEscaped){_XLOG_
		CString sNewTitle;
		GetActiveWindowTitle(sNewTitle, objSettings.ActiveWndRect);
		CCOMStringW wszText=L"";
		if(szSelectionSubst){_XLOG_
			WCHAR* wsz=MyA2W(szSelectionSubst);
			if(wsz){_XLOG_
				wszText=wsz;
				delete[] wsz;
			}
		}else{_XLOG_
			wszText=GetSelectedTextInFocusWndW(TRUE);
		}
		CString sText;
		CString sSel;
		if(IsBMP(wszText.c_str())){_XLOG_
			sText=(const char*)(wszText.c_str());
		}else{_XLOG_
			sText=wszText.c_str();
			sSel=sText.SpanExcluding("\r\n");
		}
		if(bNeedSelected){_XLOG_
			sParams.Replace("%SELECTED_FILE",GetPathPart(sSel,0,0,1,1));
			sParams.Replace("%SELECTED_FOLDER",GetPathPart(sSel,1,1,0,0));
			sParams.Replace("%SELECTED",sSel);
			sParams.Replace("%1",sSel);
		}
		if(bNeedEscaped){_XLOG_
			CString sEscaped=EscapeString(wszText.c_str());
			sParams.Replace("%ESCAPED_FILE",EscapeString(GetPathPart(sSel,0,0,1,1)));
			sParams.Replace("%ESCAPED_FOLDER",EscapeString(GetPathPart(sSel,1,1,0,0)));
			sParams.Replace("%ESCAPED",sEscaped);
			sParams.Replace("%E_SELECTED",sEscaped);
			if(sParams.Find("%URL_SELECTED")!=-1){_XLOG_
				CString sEscapedUTF8=EscapeStringUTF8(wszText.c_str());
				sParams.Replace("%URL_SELECTED",sEscapedUTF8);
			}
		}
	}
	if(bNeedParams){_XLOG_
		if(pW){_XLOG_
			CString sQRunParam=pW->sItemParams;
			if(szParAdditions){_XLOG_
				sQRunParam+=szParAdditions;
			}
			sParams.Replace("%PARAMETER",EscapeString(sQRunParam));
		}else{_XLOG_
			sParams.Replace("%PARAMETER","");
		}
	}
	SubstMyVariablesWK(sParams,bJScriptSafe);
}

//#define TEST_CLIP
extern DWORD dwClipAttachTime;
extern CCriticalSection csClipAttach;
extern CCriticalSection csClipRecreation;
DWORD WINAPI DoWM_DRAWCLIPBOARDProcessing_InThread(LPVOID p)
{_XLOG_
	__FLOWLOG;
	if(spyMode()){
		return 0;
	}
	CSmartLock lock(&csClipAttach);
	CSmartLock lock2(&csClipRecreation);
	LOGSTAMP1("receiving WM_DRAWCLIPBOARD");
	Sleep(200);// дадим другим приложениям прочухаться...
	DWORD dwLen=0;
#ifdef TEST_CLIP
	WCHAR* wszStrt=L"TEST";//GetClipboardText(FALSE,TRUE,TRUE);
#else
	WCHAR* wszStrt=GetClipboardText(FALSE,TRUE,TRUE,&dwLen);
#endif
	//ShowBaloon(Format("Clip DRAW: %s",wszStrt==0?"null!":CStringA(CStringW(wszStrt))),"WireKeys debug",9000);
	if(wszStrt){
		if(dwLen){
			dwClipAttachTime=GetTickCount();
			CString sText;
			if(SavedClipContent()!=wszStrt){_XLOG_
				GetLastClipDataSet(wszStrt);
				if(pMainDialogWindow->clipWnd->lCopyCount>0 && objSettings.lWaitForAddclipInput==0){_XLOG_
					if(objSettings.bIgnoreNextClipAction){_XLOG_
						DEBUG_INFO2("-ignoring WM_DRAWCLIPBOARD",0,0);
						objSettings.bIgnoreNextClipAction=0;
					}else{_XLOG_
						DEBUG_INFO2("-saving WM_DRAWCLIPBOARD in history",0,0);
					if(IsBMP(GetLastClipData().c_str())){_XLOG_
						sText=(const char*)(GetLastClipData().c_str());
					}else{_XLOG_
						sText=GetLastClipData().c_str();
					}
					DEBUG_INFO2("-clipdata: '%s'",TrimMessage(sText),0);
					if(sText!=""){_XLOG_
						if(objSettings.lSaveBMPSinHist || (!objSettings.lSaveBMPSinHist && !IsBMP(sText))){_XLOG_
							SetTextToClip(0,sText);
						}
					}
					}
				}
			}
		}
#ifdef TEST_CLIP
		//delete[] wszStrt;
#else
		delete[] wszStrt;
#endif
	}else{_XLOG_
#ifdef TEST_CLIP
		LOGSTAMP1("Clip change was handled by main thread - NULL IN!")
		AsyncPlaySound(CString(GetApplicationDir())+"clip_3.wav");
		ShowBaloon("Null In!!!","WireKeys debug",10000);
#endif
	}
	pMainDialogWindow->clipWnd->lCopyCount++;// Увеличиваем счетчик даже для 0!
	// Так как приложение может положить кусок в HTML и прочая и прочая, тогда в wszStrt будет 0 хотя буфер изменился
	SetEvent(pMainDialogWindow->clipWnd->hClipChanged);
	DEBUG_INFO2("-clipdata handled",0,0);
	return 0;
}

BOOL bShowWarning=1;
DWORD WINAPI DoWM_DRAWCLIPBOARDProcessing_InThread_Protected(LPVOID p)
{
	__try{
		DoWM_DRAWCLIPBOARDProcessing_InThread(p);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
#ifdef _DEBUG
		if(bShowWarning==1){
			bShowWarning=0;
			AfxMessageBox("Failed to process clipboard content!");
		}
#endif
	}
	return 0;
}

