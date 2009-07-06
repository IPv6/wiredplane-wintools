// DLGOptions.cpp : implementation file
//
#include "stdafx.h"
#include "HookCode.h"
#include "WP_WTraits.h"
#include "DLGOptions.h"
extern CWKHekperInfo hookData;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HWND hOpenedOptions=0;
const char* aWays[]={"Tray", "Minimize", "Hide", "Rollup", "Floater", "Close"};
static BOOL bAutoHideTextEmptyOnDialogOpen=0;
char g_szStartUpWindowName[256]={0};
WKCallbackInterface*& WKGetPluginContainer();
int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		hOpenedOptions=hwndDlg;
		// Локализация
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC99),_pl("WiredPlane.com @2008"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_MIN2),_pl("Hide to tray inactive windows"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC4),_pl("Description: Place one title per line (Use '*' as a wildcard for partial matches). Prefix title with action type to modify plugin behaviour. Valid prefixes are: 'Minimize', 'Hide', 'Rollup', 'Tray', 'Floater' and 'Close'. Default is 'Tray' - application will be hidden to tray. Example: Minimize: *Bat!"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC5),_pl("sec"));
		::SetWindowText(GetDlgItem(hwndDlg,IDOPTIONS),_pl("Options"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATICP),_pl("Inactivity period"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_ADDT),_pl("Window title"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BUTTON_ADD),_pl("Add"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_RDSC),_pl("To remove application entry, simply remove its title from text boxes above"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_RDSC2),_pl("Tips: You can use '*' for partial match. Delimit titles with ';'"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_ADDF),_pl("Add new window title"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_AH_TRAY),_pl("Enter titles of applications that should minimize to the tray (not taskbar)"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_AH_FL),_pl("Enter titles of applications that should minimize to the floater (not taskbar)"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_SBLOCK),_pl("Manual minimization"));
		::SetWindowText(hwndDlg,_pl("Window`s traits plugin"));
		// Инициализация
		if(strlen(g_szStartUpWindowName)==0){
			strcpy(g_szStartUpWindowName,"*");
		}
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_NEWT),g_szStartUpWindowName);
		g_szStartUpWindowName[0]=0;
		::SendMessage(GetDlgItem(hwndDlg,IDC_MIN2), BM_SETCHECK, hookData.bCatchAutoMinimize, 0);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT2),hookData.szAutoMinimizeTitle);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT3),hookData.szAutoMinToTray);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT4),hookData.szAutoMinToFloat);
		char szNumber[128]={0};
		sprintf(szNumber,"%lu",hookData.iINACTSEC);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_INACTSEC),szNumber);
		if(strlen(hookData.szAutoMinimizeTitle)==0){
			bAutoHideTextEmptyOnDialogOpen=TRUE;
		}else{
			bAutoHideTextEmptyOnDialogOpen=FALSE;
		}
		for(int i2=5;i2>=0;i2--){
			::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_WAYS),CB_ADDSTRING,0,(LPARAM)(const char*)_pl(aWays[i2]));
		}
		::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_WAYS), CB_SETCURSEL, 5, 0);
	}
	if(uMsg==WM_KEYDOWN){
		if(GetKeyState(VK_CONTROL)<0){
			if(wParam==VK_RETURN){
				uMsg=WM_COMMAND;
				wParam=IDOK;
			}
		}
	}
	/*
	if(uMsg==WM_NOTIFY && wParam==IDC_TABS && lParam){
		NMHDR* hNm=(NMHDR*)lParam;
		if(hNm->code==TCN_SELCHANGE){
			ShowPage(TabCtrl_GetCurSel(GetDlgItem(hwndDlg,IDC_TABS)),hwndDlg);
		}
	}
	*/
	if(uMsg==WM_COMMAND && wParam==IDOPTIONS){
		WKGetPluginContainer()->ShowPluginPrefernces();
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BUTTON_ADD){
		char szNumber[128]={0};
		GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_NEWT),szNumber,sizeof(szNumber));
		if(strcmp(szNumber,"*")==0){
			MessageBox(0,_pl("Enter window title first"),"Windows helper",0);
		}else{
			int iType=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_COMBO_WAYS), CB_GETCURSEL, 0, 0);
			if(strlen(szNumber)>0 && iType!=CB_ERR){
				char szAddText[sizeof(hookData.szAutoMinimizeTitle)]={0};
				strcpy(szAddText,aWays[5-iType]);
				strcat(szAddText,": ");
				strcat(szAddText,szNumber);
				strcat(szAddText,"\r\n");
				memset(hookData.szAutoMinimizeTitle,0,sizeof(hookData.szAutoMinimizeTitle));
				GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT2),hookData.szAutoMinimizeTitle,sizeof(hookData.szAutoMinimizeTitle)-1);

				if(strlen(hookData.szAutoMinimizeTitle)+strlen(szAddText)<sizeof(hookData.szAutoMinimizeTitle)){
					strcat(szAddText,hookData.szAutoMinimizeTitle);
					SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT2),szAddText);
				}
				if(!hookData.bCatchAutoMinimize){
					hookData.bCatchAutoMinimize=1;
					::SendMessage(GetDlgItem(hwndDlg,IDC_MIN2), BM_SETCHECK, hookData.bCatchAutoMinimize, 0);
					hookData.iINACTSEC=2;
					::SetWindowText(GetDlgItem(hwndDlg,IDC_INACTSEC),"2");
				}
			}else{
				MessageBox(0,_pl("Enter window title first"),"Windows helper",0);
			}
		}
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		EndDialog(hwndDlg,1);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		char szNumber[128]={0};
		hookData.bCatchAutoMinimize=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_MIN2), BM_GETCHECK, 0, 0);
		GetWindowText(GetDlgItem(hwndDlg,IDC_INACTSEC),szNumber,sizeof(szNumber));
		hookData.iINACTSEC=atol(szNumber);
		memset(hookData.szAutoMinimizeTitle,0,sizeof(hookData.szAutoMinimizeTitle));
		GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT2),hookData.szAutoMinimizeTitle,sizeof(hookData.szAutoMinimizeTitle)-1);
		GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT3),hookData.szAutoMinToTray,sizeof(hookData.szAutoMinToTray)-1);
		GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT4),hookData.szAutoMinToFloat,sizeof(hookData.szAutoMinToFloat)-1);
		if(hookData.iStickness<3){
			hookData.bSnapMovements=0;
			hookData.iStickness=0;
		}
		if(strlen(hookData.szAutoMinimizeTitle)>0 && bAutoHideTextEmptyOnDialogOpen){
			hookData.bCatchAutoMinimize=TRUE;
			bAutoHideTextEmptyOnDialogOpen=0;
		}
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}