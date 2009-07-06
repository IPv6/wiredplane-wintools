// HookCode.cpp: implementation of the CHookCode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HookCode.h"
#include "../WKPlugin.h"

#define MAX_INTEROPQUEUE	100
struct EVENTMSG2 {
    UINT    message;
    UINT    paramL;
    UINT    paramH;
    DWORD   time;
    HWND    hwnd;
	// Уже наше
	HKL		tLayout;
};

#pragma data_seg("Shared")
HHOOK g_hrechook=NULL;
HWND hMacroRecControlWindow=0;
int iRecordingFormat=0;
int interopQueue=0;
EVENTMSG2 interop[MAX_INTEROPQUEUE]={0};
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")

// Nonshared variables
void ForceDebugBreak()
{
   __try { _asm { int 3 }; }
   __except(UnhandledExceptionFilter(GetExceptionInformation())) { }
}

extern HHOOK g_hhook;
extern COptions plgOptions;
extern BOOL bPaused;

WKCallbackInterface*& WKGetPluginContainer();
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain_(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		//g_hinstDll = hinstDll;
		break;
	case DLL_PROCESS_DETACH:
		// The calling process is detaching the DLL from its address space.
		break;
	case DLL_THREAD_ATTACH:
		// A new thread is being created in the current process.
		break;
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly.
		break;
	}
	return(TRUE);
}

BOOL isMemEmpty(LPVOID pData, long count)
{
	BYTE* p=(BYTE*)pData;
	for(long i=0;i<count;i++){
		if(*(p+i)!=0){
			return FALSE;
		}
	}
	return TRUE;
}

struct CCharToName
{
	DWORD dwChar;
	const char* szName;
};

CCharToName g_C2NCodesHandHelp[]=
{
	{VK_BACK,	"{BACKSPACE}"},
	{VK_PAUSE,	"{BREAK}"},
	{VK_CAPITAL,"{CAPSLOCK}"},
	{VK_DELETE,	"{DELETE}"},
	{VK_DOWN,	"{DOWN}"},
	{VK_END,	"{END}"},
	{VK_RETURN,	"{ENTER}"},
	{VK_ESCAPE,	"{ESC}"},
	{VK_HELP,	"{HELP}"},
	{VK_HOME,	"{HOME}"},
	{VK_INSERT,	"{INSERT}"},
	{VK_LEFT,	"{LEFT}"},
	{VK_NUMLOCK,"{NUMLOCK}"},
	{VK_NEXT,	"{PGDN}"},
	{VK_PRIOR,	"{PGUP}"},
	{VK_SNAPSHOT,"{PRTSC}"},
	{VK_RIGHT,	"{RIGHT}"},
	{VK_SCROLL,	"{SCROLLLOCK}"},
	{VK_TAB,	"{TAB}"},
	{VK_UP,	"{UP}"},
	{VK_F1,	"{F1}"},
	{VK_F2,	"{F2}"},
	{VK_F3,	"{F3}"},
	{VK_F4,	"{F4}"},
	{VK_F5,	"{F5}"},
	{VK_F6,	"{F6}"},
	{VK_F7,	"{F7}"},
	{VK_F8,	"{F8}"},
	{VK_F9,	"{F9}"},
	{VK_F10,"{F10}"},
	{VK_F11,"{F11}"},
	{VK_F12,"{F12}"},
	{VK_F13,"{F13}"},
	{VK_F14,"{F14}"},
	{VK_F15,"{F15}"},
	{VK_F16,"{F16}"},
	{'+',	"{+}"},
	{'^',	"{^}"},
	{'%',	"{%}"},
	{'~',	"{~}"},
	{'[',	"{[}"},
	{']',	"{]}"},
	{'{',	"{{}"},
	{'}',	"{}}"},
	{0,""}
};

CString GetKeyName(DWORD dwVkCode)
{
	int iCPos=0;
	while(g_C2NCodesHandHelp[iCPos].dwChar>0){
		if(g_C2NCodesHandHelp[iCPos].dwChar==dwVkCode){
			return g_C2NCodesHandHelp[iCPos].szName;
		}
		iCPos++;
	}
	if(dwVkCode<32){
		CString sTmp;
		sTmp.Format("0x%02x",dwVkCode);
		return sTmp;
	}
	char szBuffer[64]={0};
	szBuffer[0]=char(dwVkCode);
	DWORD dwChar=MapVirtualKey(dwVkCode,0);
	if(GetKeyNameText((dwChar<<16),szBuffer,sizeof(szBuffer))==0){
		return CString(char(dwVkCode));
	}else{
		return szBuffer;
	}
}

DWORD g_dwTimer=0;
BOOL bBlockRecord=0;
char szTitle[1024]="";
CArray<CMacroRecord,CMacroRecord&> aMRecords;
void AddRecord(CMacroRecord& pRecord)
{
	static CCriticalSection csS;
	SimpleLocker cs(&csS);
	if(hMacroRecControlWindow && pRecord.hActWnd!=hMacroRecControlWindow){
		aMRecords.Add(pRecord);
	}
	if(hMacroRecControlWindow){
		CString sText=pRecord.sActWndTitle;
		if(pRecord.dwMessage==WM_KEYDOWN || pRecord.dwMessage==WM_SYSKEYDOWN){
			sText.Format("%s: '%s'",_pl("Key down"),GetKeyName(pRecord.dwVkCode));
		}else if(pRecord.dwMessage==WM_KEYUP || pRecord.dwMessage==WM_SYSKEYUP){
			sText.Format("%s: '%s'",_pl("Key up"),GetKeyName(pRecord.dwVkCode));
		}else if(pRecord.dwMessage==WM_LBUTTONUP && pRecord.dwMessage<=WM_RBUTTONUP){
			sText=_pl("Mouse button up");
		}else if(pRecord.dwMessage==WM_LBUTTONDOWN && pRecord.dwMessage<=WM_RBUTTONDOWN){
			sText=_pl("Mouse button down");
		}else if(pRecord.dwMessage==WM_LBUTTONDBLCLK && pRecord.dwMessage<=WM_RBUTTONDBLCLK){
			sText=_pl("Mouse button double click");
		}else if(pRecord.dwMessage>=WM_MOUSEFIRST && pRecord.dwMessage<=WM_MOUSELAST){
			sText=_pl("Mouse action");
		}else if(pRecord.dwMessage==WM_COMMAND || pRecord.dwMessage==WM_SYSCOMMAND){
			sText=_pl("Menu command");
		}
		CString sOut;
		sOut.Format(" %s #%lu\r\n%s",_pl("Step"),aMRecords.GetSize()+1,sText);
		::SetWindowText(GetDlgItem(hMacroRecControlWindow,IDC_EDIT),sOut);
	}
}

#include "../WKPlugin.h"
void HandleInputBlock(EVENTMSG2* pMsg, bool BfROMmSGpROC)
{
	CMacroRecord objRecord;
	HWND hActiveWnd=GetForegroundWindow();//pMsg->hwnd
#ifdef _DEBUG
	if(objRecord.dwMessage==WM_KEYDOWN){
		int a=0;
	}
#endif
	CPoint pt;
	GetCursorPos(&pt);
	objRecord.hActWnd=hActiveWnd;
	objRecord.dwMessage=pMsg->message;
	objRecord.dwLParam=pMsg->paramL;
	objRecord.dwHParam=pMsg->paramH;
	TRACE2("<<<Adding record: %x, %x",pMsg->paramL,pMsg->paramH);
	objRecord.dwXRel=pt.x;
	objRecord.dwYRel=pt.y;
	objRecord.bUp=(objRecord.dwMessage==WM_KEYUP?TRUE:FALSE)||(objRecord.dwMessage==WM_SYSKEYUP?TRUE:FALSE)||(objRecord.dwMessage==WM_LBUTTONUP)||(objRecord.dwMessage==WM_RBUTTONUP)||(objRecord.dwMessage==WM_MBUTTONUP);
	objRecord.bDown=(objRecord.dwMessage==WM_KEYDOWN?TRUE:FALSE)||(objRecord.dwMessage==WM_SYSKEYDOWN?TRUE:FALSE)||(objRecord.dwMessage==WM_LBUTTONDOWN)||(objRecord.dwMessage==WM_RBUTTONDOWN)||(objRecord.dwMessage==WM_MBUTTONDOWN);
	if(hActiveWnd==hMacroRecControlWindow && !objRecord.bUp){
		return;
	}
	if(hActiveWnd){
		char szTitle2[1024]="";
		GetWindowText(hActiveWnd,szTitle2,sizeof(szTitle2));
		objRecord.sActWndExe=GetExeFromHwnd(hActiveWnd);
		objRecord.sActWndTitle=szTitle2;
	}
	if(hActiveWnd){
		CRect rt;
		GetWindowRect(hActiveWnd,&rt);
		if(rt.PtInRect(pt)){
			objRecord.bRelativePos=1;
			pt.x-=rt.left;
			pt.y-=rt.top;
		}
	}
	if(objRecord.dwMessage>=WM_KEYFIRST && objRecord.dwMessage<=WM_KEYLAST){
		if(BfROMmSGpROC){
			// Меняем значения как под Journal хуком!!!
				DWORD dwVK=pMsg->paramH;
				DWORD dwScan=(pMsg->paramL>>16)&0xFF;
				DWORD dwExt=(pMsg->paramL&(1<<24)?1:0);
				pMsg->paramL=MAKEWORD(dwVK,dwScan);
				pMsg->paramH=dwExt?0xFFFFFFFF:0;
				objRecord.dwLParam=pMsg->paramL;
				objRecord.dwHParam=pMsg->paramH;
				/*
				objRecord.dwVkCode=pMsg->paramH;
				objRecord.bExtended=(pMsg->paramL&(1<<24)?1:0);

				DWORD dwVK=LOBYTE(lPar);
				DWORD dwScan=HIBYTE(lPar);
				*/
				/*DWORD dwScanCode=(pMsg->paramL>>16)&0xFF;
				BYTE lKeyState[256]={0};
				lKeyState[VK_SHIFT]=(GetKeyState(VK_SHIFT)<0?1:0)<<7;
				lKeyState[VK_CONTROL]=(GetKeyState(VK_CONTROL)<0?1:0)<<7;
				lKeyState[VK_MENU]=(GetKeyState(VK_MENU)<0?1:0)<<7;
				WORD wBuffer=0;
				if(ToAsciiEx(g_LastChar, dwScanCode, (CONST PBYTE)lKeyState, &wBuffer, 0, pMsg->tLayout)>0){
					objRecord.dwVkCode=LOBYTE(wBuffer);
				}*/
		}
		objRecord.dwVkCode=LOBYTE(pMsg->paramL);
		objRecord.bExtended=(pMsg->paramH&(1<<15)?1:0);
	}
	objRecord.dwTimeOut=GetTickCount();
	if(aMRecords.GetSize()>0){
		objRecord.dwTimeOutDelta=objRecord.dwTimeOut-aMRecords[aMRecords.GetSize()-1].dwTimeOut;
	}
	AddRecord(objRecord);
}

LRESULT CALLBACK JournalRecordProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning())
	{
		SHORT val=GetKeyState(VK_PAUSE);
		if (val & 0x80000000){
			PostMessage(hMacroRecControlWindow,WM_STOPREC,0,0);
		}else if(!bBlockRecord){
			switch(nCode){
			case HC_ACTION:
				{
					EVENTMSG* pMsg=(EVENTMSG*)lParam;
					if(pMsg && !bPaused){
						if(iRecordingFormat!=0 && !(pMsg->message==WM_KEYUP || pMsg->message==WM_KEYDOWN || pMsg->message==WM_SYSKEYUP || pMsg->message==WM_SYSKEYDOWN)){
							// Нам не сюда...
							break;
						}
						if(pMsg->message==WM_MOUSEMOVE){
							// Нам не сюда...
							break;
						}
						EVENTMSG2 msg2;
						memset(&msg2,0,sizeof(EVENTMSG2));
						memcpy(&msg2,pMsg,sizeof(EVENTMSG));
						HandleInputBlock(&msg2,false);
					}
					break;
				}
			case HC_SYSMODALON:
				{
					bBlockRecord=1;
					break;
				}
			case HC_SYSMODALOFF:
				{
					bBlockRecord=0;
					break;
				}
			}
		}
	}
	return(CallNextHookEx(g_hrechook, nCode, wParam, lParam));
}


#define DO_WAIT_CLEAR(X) {int i=0;while(i<10 && X.message>0){Sleep(100);i++;}};
LRESULT CALLBACK GetMessageRecordProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(g_hrechook==0){
		return 0;
	}
	if(WKUtils::isWKUpAndRunning() && !bBlockRecord){
		if (nCode>=0 && wParam==PM_REMOVE && lParam!=0) {
			MSG* msg=((MSG*)lParam);
			if(msg->hwnd!=hMacroRecControlWindow){
				if(msg->message==WM_KEYUP || msg->message==WM_KEYDOWN || msg->message==WM_SYSKEYUP || msg->message==WM_SYSKEYDOWN){
					if(iRecordingFormat<3){//0,1,2(js)-все с клавишами
						// Нам не сюда...
						interop[interopQueue].message=msg->message;
						interop[interopQueue].hwnd=msg->hwnd;
						interop[interopQueue].paramH=msg->wParam;
						interop[interopQueue].paramL=msg->lParam;
						interop[interopQueue].time=msg->time;
						interop[interopQueue].tLayout=GetKeyboardLayout(GetCurrentThreadId());
						PostMessage(hMacroRecControlWindow,WM_REC_KEYK,(WPARAM)msg->wParam,interopQueue);
						//DO_WAIT_CLEAR(interop)
						interopQueue++;
						if(interopQueue>=MAX_INTEROPQUEUE){
							interopQueue=0;
						}
					}
				}
				if(msg->message>=WM_MOUSEFIRST && msg->message<=WM_MOUSELAST){
					if(iRecordingFormat==0){//0-с клавишами
						if(msg->message!=WM_MOUSEMOVE){
							interop[interopQueue].message=msg->message;
							interop[interopQueue].hwnd=msg->hwnd;
							interop[interopQueue].paramH=msg->wParam;
							interop[interopQueue].paramL=msg->lParam;
							interop[interopQueue].time=msg->time;
							interop[interopQueue].tLayout=GetKeyboardLayout(GetCurrentThreadId());
							PostMessage(hMacroRecControlWindow,WM_REC_MOUSK,(WPARAM)msg->wParam,interopQueue);
							//DO_WAIT_CLEAR(interop)
							interopQueue++;
							if(interopQueue>=MAX_INTEROPQUEUE){
								interopQueue=0;
							}
						}
					}
				}
				if(msg->message==WM_COMMAND){
					if(iRecordingFormat==3){
						PostMessage(hMacroRecControlWindow,WM_REC_COMMAND,(WPARAM)msg->wParam,(LPARAM)msg->lParam);
					}
				}
			}
		}
	}
	return(CallNextHookEx(g_hrechook, nCode, wParam, lParam));
}

BOOL g_bFirstTimeSwitch=TRUE;
CString g_sAfterAllClosingPostfix="";
CString g_sTabs="";
BOOL g_bSendKeysCount=0;
BOOL InjectAppSwitching(CString& sActiveAppExe,CString& sActiveAppWnd,CString& sLastMacroContent)
{
	//if(plgOptions.bAppSwitchesRTyp==0)
	{// Всегда пишем комментарий об активном приложении...
		CString sActiveApp;
		sActiveApp.Format("// Active application: '%s' (%s)\r\n",sActiveAppWnd,sActiveAppExe);
		sLastMacroContent+=g_sTabs+sActiveApp;
	}
	if(plgOptions.bAppSwitchesRTyp==1 || plgOptions.bAppSwitchesRTyp==0){
		CString sActiveApp,sActiveAppWndSafe=sActiveAppWnd;
		MakeJScriptSafe(sActiveAppWndSafe);
		if(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3){
			sActiveApp.Format(WPPREF"'ActivateWindow(%s)');\r\n",sActiveAppWndSafe);
		}else{
			sActiveApp.Format("WshShell.AppActivate(\"%s\");\r\n",sActiveAppWndSafe);
		}
		if(plgOptions.bAppSwitchesRTyp==0){
			sActiveApp=CString("// ")+sActiveApp;
		}
		sLastMacroContent+=g_sTabs+sActiveApp;
	}
	if(plgOptions.bAppSwitchesRTyp==2){
		g_sTabs="\t";
		g_sAfterAllClosingPostfix="};\r\n";
		if(!g_bFirstTimeSwitch){
			sLastMacroContent+=g_sAfterAllClosingPostfix+"\r\n";
		}
		CString sActiveApp;
		char szFileName[MAX_PATH]="";
		_splitpath(sActiveAppExe,0,0,szFileName,0);
		CString sFileNameOnly=szFileName;
		sFileNameOnly.MakeUpper();
		sActiveApp.Format("if(unescape(\"%%E_ACTIVEAPP\")==\"%s\")\r\n{\r\n",sFileNameOnly);
		sLastMacroContent+=sActiveApp;
	}
	g_bFirstTimeSwitch=FALSE;
	return TRUE;
}

BOOL InjectKeyPress(DWORD& dwTimeout,CString& sSingleSendKeyPrefix,CString& sSingleSendKey,CString sComment,CString& sLastMacroContent)
{
	DWORD dwTimeoutSecs=int(dwTimeout/1000);
	if(plgOptions.bRecPauses && dwTimeoutSecs>0 && g_bSendKeysCount>0 && dwTimeoutSecs>=DWORD(plgOptions.lPauseLevel)){
		CString sTimeout;
		sTimeout.Format("WireKeys.Wait(%04f*iMacroSpeed);",double(dwTimeout)/1000);
		sLastMacroContent+=g_sTabs+sTimeout+"\r\n";
	}
	CString sAddkeyCommand="WshShell.SendKeys(\"";
	CString sAddkeyCommandEnd="\");";
	if(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3){
		sAddkeyCommand=WPPREF"'Command(";
		sAddkeyCommandEnd=")');";
	}
	if(sComment!=""){
		sAddkeyCommandEnd+="// ";
		sAddkeyCommandEnd+=sComment;
	}
	sAddkeyCommandEnd+="\r\n";
	if(sSingleSendKey!=""){// Завершаем...
		plgOptions.sLastMacroContent+=g_sTabs+sAddkeyCommand;
		if(sSingleSendKeyPrefix!=""){
			sSingleSendKey=sSingleSendKeyPrefix+"("+sSingleSendKey;
			sSingleSendKey+=")";
		}
		sLastMacroContent+=sSingleSendKey;
		sLastMacroContent+=sAddkeyCommandEnd;
		g_bSendKeysCount++;
	}else if(sSingleSendKeyPrefix!=""){
		// Просто нажатие на Alt/Shift/Control
		sLastMacroContent+=g_sTabs+sAddkeyCommand+sSingleSendKeyPrefix;
		sLastMacroContent+=sAddkeyCommandEnd;
		g_bSendKeysCount++;
	}
	// Тут нам это больше не понадобится
	sSingleSendKeyPrefix="";
	sSingleSendKey="";
	dwTimeout=0;
	return TRUE;
}

DWORD dwStopReason=0;
DWORD dwStopReasonW=0;
CString InstallHook()
{
	__FLOWLOG;
	bBlockRecord=1;
	iRecordingFormat=plgOptions.lMFormat;
	TRACE1(">>> Recformat: %i\n",iRecordingFormat);
	aMRecords.RemoveAll();
	g_dwTimer=GetTickCount();
	HINSTANCE hThis=LoadLibrary("WP_MacroRec.wkp");
	dwStopReason=0;
	dwStopReasonW=0;
	CString sError;
	TRACE(">>>>3\n");
	BOOL bUseMsgRecPrc=1;//Используем всегда!!! //(plgOptions.lMFormat==3)?1:0;
	if(bUseMsgRecPrc)
	{
		TRACE(">>>>3 GetMessageHook\n");
		char szDLLPath[MAX_PATH]={0};GetModuleFileName(WKGetPluginContainer()->GetPluginDllInstance(),szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
		HOOKPROC myMsgProc=(HOOKPROC)GetProcAddress(hDLL,"GetMessageRecordProc");
		g_hrechook = SetWindowsHookEx(WH_GETMESSAGE, myMsgProc, hDLL, 0);
		//g_hrechook = SetWindowsHookEx(WH_GETMESSAGE, GetMessageRecordProc, WKGetPluginContainer()->GetPluginDllInstance(), 0);
	}else{
		TRACE(">>>>3 JournalRecord\n");
		char szDLLPath[MAX_PATH]={0};GetModuleFileName(WKGetPluginContainer()->GetPluginDllInstance(),szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
		g_hrechook = SetWindowsHookEx(WH_JOURNALRECORD, JournalRecordProc, WKGetPluginContainer()->GetPluginDllInstance(), 0);
		//g_hrechook = SetWindowsHookEx(WH_JOURNALRECORD, JournalRecordProc, hDLL, 0);
		//HOOKPROC jpc=(HOOKPROC)GetProcAddress(hDLL,"JournalRecordProc");g_hrechook = SetWindowsHookEx(WH_JOURNALRECORD, jpc, hDLL, 0);
	}
	if(g_hrechook==NULL){
		sError.Format("%s, 0x%08X",_pl("Failed to set hook"),GetLastError());
		PostQuitMessage(0);
	}
	bBlockRecord=0;
	TRACE2(">>>>4 %i, %08X Hooks added\n",g_hrechook,GetLastError());
	return sError;
}

CString DeinstallHookAndAnalizeData()
{
	__FLOWLOG
	CString sError;
	UnhookWindowsHookEx(g_hrechook);
	g_hrechook=NULL;
	if(dwStopReasonW==1){
		return Format("%s",_pl("Macro discarded"));
	}
	// Анализируем данные
	int iRecSize=aMRecords.GetSize();
	if(iRecSize==0){
		sError.Format("%s",_pl("Nothing to record"));
		return sError;
	}
	aMRecords.SetSize(iRecSize);
	g_bSendKeysCount=0;
	g_bFirstTimeSwitch=TRUE;
	g_sAfterAllClosingPostfix="";
	int iCurrentStep=0;
	CString sActiveAppExe;
	CString sActiveAppWnd;
	if(plgOptions.bAppSwitchesRTyp==2){
		// Предварительно сортируем так, чтобы нажатия относящиеся к одному приложению/окну шли подряд
		for(int i=0;i<iRecSize-1;i++){
			CString sKey1=aMRecords[i].sActWndExe;//+"!"+aMRecords[i].sActWndTitle;
			CString sKey2=aMRecords[i+1].sActWndExe;//+"!"+aMRecords[i+1].sActWndTitle;
			if(sKey1!=sKey2){
				for(int k=i+2;k<iRecSize;k++){
					CString sKey3=aMRecords[k].sActWndExe;//+"!"+aMRecords[k].sActWndTitle
					if(sKey1==sKey3){
						CMacroRecord tmp=aMRecords[k];
						aMRecords.RemoveAt(k);
						aMRecords.InsertAt(i+1,tmp);
						break;
					}
				}
			}
		}
	}
	while(iCurrentStep<iRecSize){
		DWORD dwTimeout=0;
		// Обрабатываем строку
		CString sSingleSendKey="";
		CString sSingleSendKeyPrefix="";
		while(iCurrentStep<aMRecords.GetSize()){
			CMacroRecord& mr=aMRecords[iCurrentStep];
			if(mr.dwMessage==0){
				// Это майлстоун
				plgOptions.sLastMacroContent+=mr.sActWndTitle;
				plgOptions.sLastMacroContent+="\r\n";
				iCurrentStep++;
				continue;
			}
			// Если эта клавиша - уже в другом приложении
			// Сbрасываем все нафиг
			if(sActiveAppExe=="" && sActiveAppWnd==""){
				sActiveAppExe=mr.sActWndExe;
				sActiveAppWnd=mr.sActWndTitle;
				InjectAppSwitching(sActiveAppExe,sActiveAppWnd,plgOptions.sLastMacroContent);
			}else if((mr.sActWndExe!="" && sActiveAppExe!=mr.sActWndExe)){//(mr.sActWndTitle!="" && sActiveAppWnd!=mr.sActWndTitle)
				sActiveAppExe=mr.sActWndExe;
				sActiveAppWnd=mr.sActWndTitle;
				InjectAppSwitching(sActiveAppExe,sActiveAppWnd,plgOptions.sLastMacroContent);
				break;
			}
			iCurrentStep++;
			if(plgOptions.lMFormat==3){
				CString sText="",sTmp;
				sText=Format("cmd::msg=[0x%04X] h=[0x%08X] l=[0x%08X]",mr.dwMessage,mr.dwHParam,mr.dwLParam);
				InjectKeyPress(mr.dwTimeOutDelta,sTmp,sText,"",plgOptions.sLastMacroContent);
				break;
			}else if(plgOptions.lMFormat==0 || plgOptions.lMFormat==1){
				// Вписываем команду и успокаиваемся!
				CString sText="",sTmp,sDesc;
				if(mr.dwMessage>=WM_KEYFIRST && mr.dwMessage<=WM_KEYLAST){
					sText=Format("key::msg=[%lu] l=[0x04%X] keydown=[%lu] ext=[%lu]",mr.dwMessage,mr.dwLParam,mr.bDown,mr.bExtended);
					sDesc=Format("%s: %s",mr.bDown?_l("Key down"):_l("Key up"),GetKeyName(mr.dwVkCode));
				}
				if(mr.dwMessage>=WM_MOUSEFIRST && mr.dwMessage<=WM_MOUSELAST){
					sText=Format("mou::msg=[%lu] h=[%lu] l=[%lu] rel=[%lu] x=[%i] y=[%i]",mr.dwMessage,mr.dwHParam,mr.dwLParam,mr.bRelativePos,mr.dwXRel,mr.dwYRel);
				}
				InjectKeyPress(mr.dwTimeOutDelta,sTmp,sText,sDesc,plgOptions.sLastMacroContent);
				break;
			}else if(mr.bDown){
				BOOL bModPressed=0;
				if(mr.dwVkCode==VK_SHIFT || mr.dwVkCode==VK_MENU || mr.dwVkCode==VK_CONTROL){
					bModPressed=1;
				}
				if(bModPressed && sSingleSendKey!=""){
					InjectKeyPress(dwTimeout,sSingleSendKeyPrefix,sSingleSendKey,"",plgOptions.sLastMacroContent);
				}
				if(sSingleSendKey=="" && iCurrentStep>1){
					// Запоминаем величину ожидания...
					dwTimeout=mr.dwTimeOut-aMRecords[iCurrentStep-2].dwTimeOut;
				}
				if(mr.dwVkCode==VK_SHIFT){
					if(sSingleSendKeyPrefix.Find("+")==-1){
						sSingleSendKeyPrefix+="+";
					}
					dwTimeout=0;
				}else if(mr.dwVkCode==VK_MENU){
					if(sSingleSendKeyPrefix.Find("%")==-1){
						sSingleSendKeyPrefix+="%";
					}
					dwTimeout=0;
				}else if(mr.dwVkCode==VK_CONTROL){
					if(sSingleSendKeyPrefix.Find("^")==-1){
						sSingleSendKeyPrefix+="^";
					}
					dwTimeout=0;
				};
			}else if(mr.bUp){
				if(mr.dwVkCode==VK_MENU || mr.dwVkCode==VK_SHIFT || mr.dwVkCode==VK_CONTROL){
					// Такие клавиши - на один SendKeys
					break;
				}
				// Ищем символ в карте
				sSingleSendKey+=GetKeyName(mr.dwVkCode);
				if(sSingleSendKeyPrefix=="" && iCurrentStep<aMRecords.GetSize() && (aMRecords[iCurrentStep].dwTimeOut-mr.dwTimeOut)>=DWORD(plgOptions.lPauseLevel*1000)){
					// Слишком долго... cбрасываем
					break;
				}
			}
		}
		InjectKeyPress(dwTimeout,sSingleSendKeyPrefix,sSingleSendKey,"",plgOptions.sLastMacroContent);
	}
	plgOptions.sLastMacroContent+=g_sAfterAllClosingPostfix;
	return "";
}

void HandleMacroRecorderMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	TRACE1(">>>>2a (%x)\n",message);
	if(message==WM_CANCELJOURNAL){
		bBlockRecord=1;
		TRACE(">>>>CJ\n");
		//PostQuitMessage(1);
		PostMessage(hMacroRecControlWindow,WM_CLOSERECORDER,0,0);
	}else if(message==WM_STARTREC){
		InstallHook();
	}else if(message==WM_STOPREC){
		__FLOWLOG
		bBlockRecord=1;
		dwStopReason=lParam;
		dwStopReasonW=wParam;
		TRACE1(">>>>SR %i\n",message==WM_CLOSE);
		//PostQuitMessage(0);
		PostMessage(hMacroRecControlWindow,WM_CLOSERECORDER,0,0);
	}else if(message==WM_REC_KEYK){
		TRACE(">>>KEY DETECTED!!!\n");
		HandleInputBlock(&interop[lParam],true);
		interop[lParam].message=0;
	}else if(message==WM_REC_MOUSK){
		TRACE(">>>MOUS DETECTED!!!\n");
		HandleInputBlock(&interop[lParam],true);
		interop[lParam].message=0;
	}else if((plgOptions.lMFormat==3) && (message==WM_REC_COMMAND)){
		__FLOWLOG;
		CMacroRecord objRecord;
		HWND hActiveWnd=GetForegroundWindow();//pMsg->hwnd
		if(hActiveWnd!=hMacroRecControlWindow && !bPaused){
			if(hActiveWnd){
				GetWindowText(hActiveWnd,szTitle,sizeof(szTitle));
				objRecord.sActWndExe=GetExeFromHwnd(hActiveWnd);
				objRecord.sActWndTitle=szTitle;
			}
			CPoint pt;
			GetCursorPos(&pt);
			objRecord.dwMessage=WM_COMMAND;
			objRecord.dwLParam=lParam;
			objRecord.dwHParam=wParam;
			TRACE2("WM_COMMAND detected: %i, %i",wParam,lParam);
			objRecord.dwXRel=pt.x;
			objRecord.dwYRel=pt.y;
			objRecord.bUp=FALSE;
			objRecord.bDown=FALSE;
			objRecord.dwTimeOut=GetTickCount();
			if(aMRecords.GetSize()>0){
				objRecord.dwTimeOutDelta=objRecord.dwTimeOut-aMRecords[aMRecords.GetSize()-1].dwTimeOut;
			}
			AddRecord(objRecord);
		}
	}
}

/*
съедается одно нажатие мышью
пна русский под макросом

  sdfsdfываыва
*/