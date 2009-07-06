// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_MacroRec.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL bBlockRecord; 
WKCallbackInterface*& WKGetPluginContainer();
BOOL RecordNewMacro(CString& sError);
extern COptions plgOptions;
BOOL bPaused=0;
class CMacroRecord;
extern CArray<CMacroRecord,CMacroRecord&> aMRecords;
void AddRecord(CMacroRecord& pRecord);
DWORD iMLCount=0;
HWND hwndDlgBeforeActive=0;
DWORD WINAPI RetFocus(LPVOID p)
{
	Sleep(500);
	HWND hwndDlg=(HWND)p;
	if(GetForegroundWindow()==hwndDlg)
	{
		SendMessage(hwndDlg,WM_USER+100,0,0);
		//PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlgBeforeActive);
	}
	return 0;
}

CString InstallHook();
CString DeinstallHookAndAnalizeData();
extern HWND hMacroRecControlWindow;
void HandleMacroRecorderMessage(UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK RecControlDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HandleMacroRecorderMessage(uMsg, wParam, lParam);
	if(uMsg==WM_INITDIALOG){
		iMLCount=0;
		hMacroRecControlWindow=hwndDlg;
		::SetWindowPos(hMacroRecControlWindow,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		::SetWindowText(GetDlgItem(hwndDlg,IDOK), _pl("Save and close"));
		::SetWindowText(GetDlgItem(hwndDlg,IDDISMISS), _pl("Dismiss macro"));
		::SetWindowText(GetDlgItem(hwndDlg,IDPAUSE), _pl("Pause macro"));
		::SetWindowText(GetDlgItem(hwndDlg,IDREM), _pl("Add milestone"));
		::SetWindowText(hwndDlg, _pl("Recording macro"));
		::SetFocus(GetDlgItem(hwndDlg,IDPAUSE));
		DWORD dwID=0;
		//HANDLE hControlWindow=CreateThread(0,0,RetFocus,hwndDlg,0,&dwID);CloseHandle(hControlWindow);
		PostMessage(hwndDlg,WM_OPENRECORDER,0,0);
		return FALSE;
	}
	if(uMsg==WM_CLOSERECORDER){
		CString sError=DeinstallHookAndAnalizeData();
		hMacroRecControlWindow=0;
		if(sError.GetLength()){
			AfxMessageBox(sError);
		}
		EndDialog(hwndDlg,1);
	}
	if(uMsg==WM_OPENRECORDER){
		::SetForegroundWindow(hwndDlgBeforeActive);
		::BringWindowToTop(hwndDlgBeforeActive);
		SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlgBeforeActive);
		//Устанавливаем хук
		CString sError=InstallHook();
		if(sError.GetLength()){
			AfxMessageBox(sError);
			HandleMacroRecorderMessage(WM_STOPREC,1,0);
		}
		return TRUE;
	}
	if(uMsg==WM_CLOSE || (uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE)){
		HandleMacroRecorderMessage(WM_STOPREC,1,0);
		return TRUE;
	}
	if(uMsg==WM_COMMAND){
		if(!bBlockRecord){
			if(wParam==IDDISMISS){
				HandleMacroRecorderMessage(WM_STOPREC,1,0);
			}
			if(wParam==IDREM){
				CMacroRecord objRecord;
				objRecord.dwMessage=0;
				objRecord.sActWndExe="";
				if(aMRecords.GetSize()>0){
					objRecord.sActWndExe=aMRecords[aMRecords.GetSize()-1].sActWndExe;
				}
				objRecord.sActWndTitle.Format("// %s #%lu",_pl("Milestone"),++iMLCount);
				AddRecord(objRecord);
			}
			if(wParam==IDOK){
				PostMessage(hMacroRecControlWindow,WM_STOPREC,0,0);
			}
			if(wParam==IDPAUSE){
				bPaused=!bPaused;
				if(!bPaused){
					::SetWindowText(GetDlgItem(hwndDlg,IDPAUSE), _pl("Pause macro"));
					::SetWindowText(hwndDlg, _pl("Recording macro"));
					::SetWindowText(GetDlgItem(hMacroRecControlWindow,IDC_EDIT),"");
				}else{
					::SetWindowText(GetDlgItem(hMacroRecControlWindow,IDC_EDIT),_pl("Paused"));
					::SetWindowText(GetDlgItem(hwndDlg,IDPAUSE), _pl("Continue"));
					::SetWindowText(hwndDlg, _pl("[Paused] Recording macro"));
				}
			}
		}
		return TRUE;
	}
	return FALSE;
};

DWORD WINAPI StartControlWindow(LPVOID)
{
	RETURN_IFRECURSE(0);
	hMacroRecControlWindow=0;
	DWORD dwRes=DialogBox(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDD_RECMAIN),0,RecControlDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	return 0;
}

DWORD WINAPI StartRecordThread(LPVOID)
{// Recording a new macro!!!
	RETURN_IFRECURSE(0);
	StartControlWindow(0);
	return 0;
}

BOOL bSkipPreparations=0;
int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_STOPREC && (LOWORD(lParam)==WM_LBUTTONDBLCLK || LOWORD(lParam)==WM_LBUTTONUP)){
		TRACE(">>>>> Posting WM_STOPREC-6\n");
		PostMessage(hMacroRecControlWindow,WM_STOPREC,0,2);
		return TRUE;
	}
	if(uMsg==WM_INITDIALOG){
		__FLOWLOG
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		if(bSkipPreparations){
			ShowWindow(hwndDlg,SW_HIDE);
			PostMessage(hwndDlg,WM_COMMAND,IDOK,0);
		}else{
			::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO1), _pl("Ignore application switches, record into plain sequence"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO2), _pl("Record 'as is' (replay will switch between windows)"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO3), _pl("Record keypresses for each application separately"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC1), _pl("Title of new Macro"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC3), _pl("How to record switches between applications"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC2), _pl("Set recording parameters and press OK to start recording")+". "+_pl("Use macro panel to control process of recording (start/stop/pause)"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_F), _pl("Recording options"));
			::SetWindowText(GetDlgItem(hwndDlg,IDOK), _pl("Start"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_CHECK_EDOPEN), _pl("Open macro in text editor after all"));
			//::SetWindowText(GetDlgItem(hwndDlg,IDC_CHECK_ADDWK), _pl("Add to WireKeys"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_CHECK_PAUSE), _pl("Record pauses larger than (seconds)"));
			//!!!::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_F),CB_ADDSTRING,0,(LPARAM)(const char*)(_pl("Internal")+" - "+_pl("keys and mouse")));
			//::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_F),CB_ADDSTRING,0,(LPARAM)(const char*)(_pl("Internal")+" - "+_pl("keystrokes only")));
			//::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_F),CB_ADDSTRING,0,(LPARAM)(const char*)(_pl("JScript-SendKeys")+", "+_pl("keys only")));
			//::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_F),CB_ADDSTRING,0,(LPARAM)(const char*)(_pl("Menu items, toolbar buttons")));
			//::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_F),CB_SETCURSEL,plgOptions.lMFormat,0);
			::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO_KEYB), _pl("Record keypresses only"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO_MOUSE), _pl("Record keypresses and mouse clicks"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO_JSCRIPT), _pl("Record keypresses in JavaScript-compatible way"));
			::SetWindowText(GetDlgItem(hwndDlg,IDC_RADIO_MENUS), _pl("Record menu items and buttons only"));
			switch(plgOptions.lMFormat){
			case 1:
				::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_KEYB), BM_SETCHECK, TRUE, 0);
				break;
			case 2:
				::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_JSCRIPT), BM_SETCHECK, TRUE, 0);
				break;
			case 3:
				::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_MENUS), BM_SETCHECK, TRUE, 0);
				break;
			default:
				::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_MOUSE), BM_SETCHECK, TRUE, 0);
				break;
			}
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_PAUSE), BM_SETCHECK, plgOptions.bRecPauses, 0);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_EDOPEN), BM_SETCHECK, plgOptions.bAP_OpenInEd, 0);
			//::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_ADDWK), BM_SETCHECK, plgOptions.bAP_Add2WK, 0);
			plgOptions.bAppSwitchesRTyp=0;// Сбрасываем, слишком опасная опция
			if(plgOptions.bAppSwitchesRTyp==0){
				::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO1), BM_SETCHECK, TRUE, 0);
			}
			if(plgOptions.bAppSwitchesRTyp==1){
				::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO2), BM_SETCHECK, TRUE, 0);
			}
			if(plgOptions.bAppSwitchesRTyp==2){
				::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO3), BM_SETCHECK, TRUE, 0);
			}
			char szNumber[64]="";
			sprintf(szNumber,"%lu",plgOptions.lPauseLevel);
			::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_PAUSEL),szNumber);
			::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_TITLE),plgOptions.sLastMacroName);
			::SetWindowText(hwndDlg,_pl("Record new macro"));
		}
		return TRUE;
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		EndDialog(hwndDlg,1);
		return TRUE;
	}
	if(uMsg==WM_KEYDOWN && wParam==VK_ESCAPE){
		EndDialog(hwndDlg,1);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_SEPHELP){
		AfxMessageBox(_pl("Check this option to record macro that will perform different actions in different applications. After start, switch to the first application and press keys that you want to be recorded for it. Then switch to the next application and press key sequence specific to that application and so on. When you will play such macro WireKeys will analyze active application and fire corresponding keystrokes only"));
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		if(!bSkipPreparations){
			plgOptions.bRecPauses=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_PAUSE), BM_GETCHECK, 0, 0);
			plgOptions.bAP_Add2WK=1;//(int)::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_ADDWK), BM_GETCHECK, 0, 0);
			plgOptions.bAP_OpenInEd=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_EDOPEN), BM_GETCHECK, 0, 0);
			//!!!plgOptions.lMFormat=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_F), CB_GETCURSEL, 0, 0);
			if((int)::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_KEYB), BM_GETCHECK, 0, 0)){
				plgOptions.lMFormat=1;
			}else if((int)::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_JSCRIPT), BM_GETCHECK, 0, 0)){
				plgOptions.lMFormat=2;
			}else if((int)::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO_MENUS), BM_GETCHECK, 0, 0)){
				plgOptions.lMFormat=3;
			}else{
				plgOptions.lMFormat=0;
			}
			char szNumber[1024]="";
			::GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_PAUSEL),szNumber,sizeof(szNumber));
			plgOptions.lPauseLevel=atol(szNumber);
			szNumber[0]=0;
			::GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_TITLE),szNumber,sizeof(szNumber));
			plgOptions.sLastMacroName=szNumber;
			plgOptions.bAppSwitchesRTyp=0;
			if((int)::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO2), BM_GETCHECK, 0, 0)){
				plgOptions.bAppSwitchesRTyp=1;
			}
			if((int)::SendMessage(GetDlgItem(hwndDlg,IDC_RADIO3), BM_GETCHECK, 0, 0)){
				plgOptions.bAppSwitchesRTyp=2;
			}
		}
		ShowWindow(hwndDlg,SW_HIDE);
		TRACE(">>>>Starting recthread\n");
		FORK_WAIT(StartRecordThread,0,INFINITE);
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}

