// IconsFont.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include <atlbase.h>
#include "WP_OPE.h"
#include "..\..\SmartWires\Atl\file\SADirRead.h"
#pragma data_seg("Shared")
long		bOnNT=-1;
COptions	plgOptions = {0};
char		g_CurrentFile[MAX_PATH]={0};
char		szAddHere[128]={0};
char		szManageMenu[128]={0};
char		szSaveicons[128]={0};
char		szRestoreIcons[128]={0};
char		szFindLinkTarget[128]={0};
char		szRegisterDll[128]={0};
char		szUnregisterDll[128]={0};
char		szAssignHotkey[128]={0};
char		szCopyPaths[128]={0};
char		szCopyNames[128]={0};
char		szStartConsole[128]={0};
char		szFolderListing[128]={0};
char		szChangeIcon[128]={0};
char		szMountAsDrive[128]={0};
char		szPutInSendTo[128]={0};
char		szWKSpecials[128]={0};
char		szJumpToFolder[128]={0};
char		szAddPBar[128]={0};
char		szRemPBar[128]={0};
char		szConcat[128]={0};
char		szAssignOpen[128]={0};
char		szShowDiff[128]={0};
char		szCopyNFol[128]={0};
char		szExit[128]={0};
char		szAdd2Path[128]={0};
char		szOpts[128]={0};
char		szFDelete[128]={0};
char		szFPack[128]={0};
char		szFUNPack[128]={0};
HWND		hWKMainWnd=0;
DWORD		dwFMapSize=0;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")
extern HINSTANCE g_hinstDll;
BOOL isFileExist(const char* szFile);
BOOL isFolder(const char* szFile);
BOOL isDLL(const char* szFile);
BOOL isLNK(const char* szFile);
BOOL isOnRemovable(const char* szFile);
LONG QueryBinaryValue(CRegKey& key, LPCTSTR pszValueName, void* pValue, ULONG* pnBytes);
WINOLEAPI  CoInitializeEx(IN LPVOID pvReserved, IN DWORD dwCoInit);
STDAPI DllRegisterServerRaw(BOOL bForce);
WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

CCOMString _l(const char* szText)
{
	char szOut[1024]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

static int CALLBACK BrowseCallbackProc (HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    TCHAR szPath[_MAX_PATH];
    switch (uMsg) {
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

BOOL GetFolderRaw (LPCTSTR szTitle, LPTSTR szPath, LPCTSTR szRoot, HWND hWndOwner, DWORD dwFlags)
{
    if (szPath == NULL)
        return false;

    bool result = false;
	CoInitializeEx(0,COINIT_APARTMENTTHREADED);
    LPMALLOC pMalloc;
    if (::SHGetMalloc(&pMalloc) == NOERROR) {
        BROWSEINFO bi;
        ::ZeroMemory(&bi,sizeof bi);
        bi.ulFlags   = BIF_RETURNONLYFSDIRS;

        // дескриптор окна-владельца диалога
        bi.hwndOwner = hWndOwner;

        // добавление заголовка к диалогу
        bi.lpszTitle = szTitle;
		// 
		char strDisplayName[MAX_PATH+1]={0};
		bi.pszDisplayName=strDisplayName;

        // отображение текущего каталога
        bi.lpfn      = BrowseCallbackProc;
        bi.ulFlags  |= BIF_STATUSTEXT|0x0040;//|BIF_NEWDIALOGSTYLE;
		bi.ulFlags  |= dwFlags;

        // установка каталога по умолчанию
        bi.lParam    = LPARAM(szPath);

        // установка корневого каталога
		BOOL bSearchForComp=FALSE;
		if(dwFlags & BIF_BROWSEFORCOMPUTER){
			bSearchForComp=TRUE;
		}
        if (szRoot != NULL) {
            IShellFolder *pDF;
            if (SHGetDesktopFolder(&pDF) == NOERROR) {
                LPITEMIDLIST pIdl = NULL;
                ULONG        chEaten;
                ULONG        dwAttributes;

                USES_CONVERSION;
                LPOLESTR oleStr = T2OLE(szRoot);

                pDF->ParseDisplayName(NULL,NULL,oleStr,&chEaten,&pIdl,&dwAttributes);
                pDF->Release();
                bi.pidlRoot = pIdl;
            }
        }else{
			if(bSearchForComp){
				LPITEMIDLIST pIdl = NULL;
				if(MyGetSpecialFLoc(NULL,CSIDL_NETWORK,&pIdl)){
					bi.pidlRoot=pIdl;
				}
			}
		}
        LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
        if (pidl != NULL) {
			if (::SHGetPathFromIDList(pidl,szPath)){
				result = true;
			}else{
				strcpy(szPath,strDisplayName);
				if(strlen(szPath)>0){
					result = true;
				}
			}
            pMalloc->Free(pidl);
        }
        if (bi.pidlRoot != NULL)
            pMalloc->Free((void*)bi.pidlRoot);
        pMalloc->Release();
    }
	CoUninitialize();
    return result;
}

HWND hOpenedOptions=0;
int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		hOpenedOptions=hwndDlg;
		// Локализация
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC99),_l("Written by Ilja Razinkov, @2008"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC4),_l("Description: Edit favourite folders order and add new favourites. You can drag-and-drop files/folders from Explorer window on this dialog"));
		::SetWindowText(hwndDlg,_l("Shell extender plugin"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),"");
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_UP),_l("Move up"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_DOWN),_l("Move down"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_REM),_l("Remove"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_ADD),_l("Add"));
		::SetWindowText(GetDlgItem(hwndDlg,IDOPTIONS),_l("Options"));
		int iCI=0;
		while(plgOptions.szCustomItems[iCI][0]!=0 && iCI<MAX_CITEMS){
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_ADDSTRING,0,(LPARAM)plgOptions.szCustomItems[iCI]);
			iCI++;
		}
	}
	if(uMsg==WM_KEYDOWN){
		if(GetKeyState(VK_CONTROL)<0){
			if(wParam==VK_RETURN){
				uMsg=WM_COMMAND;
				wParam=IDOK;
			}
		}
	}
	if(uMsg==WM_DROPFILES){
		HDROP hDropInfo=(HDROP)wParam;
		char szFilePath[MAX_PATH]="",szD[MAX_PATH]={0},szP[MAX_PATH]={0};;
		DragQueryFile(hDropInfo, 0, szFilePath, sizeof(szFilePath));
		if(!isFolder(szFilePath)){
			_splitpath(szFilePath,szD,szP,0,0);
			strcpy(szFilePath,szD);
			strcat(szFilePath,szP);
		}else{
			strcat(szFilePath,"\\");
		}
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),szFilePath);
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		EndDialog(hwndDlg,1);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && LOWORD(wParam)==IDC_LIST && HIWORD(wParam)==LBN_SELCHANGE){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR){
			char szCur[MAX_PATH]={0};
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel,(LPARAM)szCur);
			::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),szCur);
		}
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_UP){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR && iCurSel>0){
			char szCur[MAX_PATH]={0};
			char szNext[MAX_PATH]={0};
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel,(LPARAM)szCur);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_DELETESTRING,iCurSel,0);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_INSERTSTRING,iCurSel-1,(LPARAM)szCur);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETCURSEL,iCurSel-1,0);
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_DOWN){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR){
			char szCur[MAX_PATH]={0};
			char szNext[MAX_PATH]={0};
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel,(LPARAM)szCur);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel+1,(LPARAM)szNext);
			if(szNext[0]!=0){
				SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_DELETESTRING,iCurSel+1,0);
				SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_INSERTSTRING,iCurSel,(LPARAM)szNext);
			}
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOPTIONS){
		WKGetPluginContainer()->ShowPluginPrefernces();
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_REM){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR){
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_DELETESTRING,iCurSel,0);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETCURSEL,iCurSel>1?iCurSel-1:0,0);
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_ADD){
		char szCur[MAX_PATH]={0};
		::GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),szCur,sizeof(szCur));
		if(szCur[0]!=0){
			if(!isFolder(szCur)){
				strcat(szCur,"\\");
			}
			BOOL bNew=1;
			int iCI=0;
			while(plgOptions.szCustomItems[iCI][0]!=0 && iCI<MAX_CITEMS){
				if(strcmp(plgOptions.szCustomItems[iCI],szCur)==0){
					bNew=0;
				};
				iCI++;
			}
			if(bNew){
				SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_ADDSTRING,0,(LPARAM)szCur);
				::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),"");
			}
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		int iCI=0;
		while(iCI<MAX_CITEMS){
			plgOptions.szCustomItems[iCI][0]=0;
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCI,(LPARAM)plgOptions.szCustomItems[iCI]);
			iCI++;
		}
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}

int	WINAPI WKPluginStart()
{
	return 1;
}

HWND g_invisEdits=0;
HWND g_hMSWordButton=0;
BOOL g_bOkCancelFound=0;
BOOL CALLBACK EnumFindEditBox(HWND hwnd,LPARAM lParam)
{
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	if(IsWindowVisible(hwnd)){
		if(stricmp(szClass,"OfficeTooltip")==0){
			g_hMSWordButton=hwnd;
		}
		if(stricmp(szClass,"Button")==0){
			if(GetDlgCtrlID(hwnd)==1){
				g_bOkCancelFound++;
			}
			if(GetDlgCtrlID(hwnd)==2){
				g_bOkCancelFound++;
			}
		}
		if(strcmp(szClass,"Edit")==0 || strstr(szClass,"RichEdit")!=0){
			if((GetWindowLong(hwnd,GWL_STYLE) & ES_READONLY)==0)
			{
				HWND* pOut=(HWND*)lParam;
				*pOut=hwnd;
			}
		}
	}else{
		if(strcmp(szClass,"Edit")==0 || strstr(szClass,"RichEdit")!=0){
			g_invisEdits=hwnd;
		}
	}
	return TRUE;
}

BOOL isWinNT()
{
	if(bOnNT!=-1){
		return bOnNT;
	}
	OSVERSIONINFO vi = { sizeof(vi) };
	GetVersionEx(&vi);
	if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		bOnNT=0;
	}else{
		bOnNT=1;
	}
	return bOnNT;
}

WCHAR* GetClipboardText(HWND hEditBox,BOOL bEmpty)
{
	USES_CONVERSION;
	OpenClipboard(hEditBox);
	// Получаем в UniCode
	WCHAR* strW=NULL;
	BOOL bSkipRest=0;
	if(!bSkipRest){
		HANDLE hCliptextW=::GetClipboardData(CF_UNICODETEXT);
		if(hCliptextW){
			WCHAR* cliptxt=(WCHAR*)::GlobalLock(hCliptextW);
			size_t len=wcslen(cliptxt);
			if(len>0){
				strW = new WCHAR[len+1];
				memset(strW,0,(len+1)*sizeof(WCHAR));
				wcscpy(strW,cliptxt);
				bSkipRest=1;
			}
			GlobalUnlock(hCliptextW);
		}
	}
	if(!bSkipRest){
		HANDLE hCliptext=::GetClipboardData(CF_TEXT);
		if(hCliptext){
			char* cliptxt = (char*)GlobalLock(hCliptext); 
			size_t len=strlen(cliptxt);
			if(len>0){
				USES_CONVERSION;
				strW = new WCHAR[len+1];
				memset(strW,0,(len+1)*sizeof(WCHAR));
				wcscpy(strW,A2W(cliptxt));
			}
			GlobalUnlock(hCliptext);
		}
	}
	if(bEmpty){
		::EmptyClipboard();
	}
	::CloseClipboard();
	return strW;
}

BOOL SetClipText(HWND hFore,const char* sTextToSetIntoClipA, WCHAR* sTextToSetIntoClipW, BOOL bWTrick)
{
	USES_CONVERSION;
	if(sTextToSetIntoClipW==0){
		sTextToSetIntoClipW=L"";
	}
	if(sTextToSetIntoClipA==0){
		sTextToSetIntoClipA="";
	}
	OpenClipboard(hFore);
	::EmptyClipboard();
	char* szCliptextA=NULL;
	int iLen=max((wcslen(sTextToSetIntoClipW)+1)*2,(strlen(sTextToSetIntoClipA)+1)*2);
	HGLOBAL hCliptextA=::GlobalAlloc((isWinNT()?GMEM_MOVEABLE:GMEM_FIXED)|GMEM_ZEROINIT,iLen);
	if(hCliptextA!=NULL){
		szCliptextA=(char*)::GlobalLock(hCliptextA);
		strcpy(szCliptextA,sTextToSetIntoClipA?sTextToSetIntoClipA:W2A(sTextToSetIntoClipW));
		if(bWTrick){
			WKGetPluginContainer()->triggerTweak(1);
		}
		::SetClipboardData(CF_TEXT,hCliptextA);//???WTF??? szCliptextA
		::GlobalUnlock(hCliptextA);
	}
	WCHAR* szCliptextW=NULL;
	HGLOBAL hCliptextW=::GlobalAlloc((isWinNT()?GMEM_MOVEABLE:GMEM_FIXED)|GMEM_ZEROINIT,iLen);
	if(hCliptextW!=NULL){
		szCliptextW=(WCHAR*)::GlobalLock(hCliptextW);
		wcscpy(szCliptextW,sTextToSetIntoClipA?A2W(sTextToSetIntoClipA):sTextToSetIntoClipW);
		if(bWTrick){
			WKGetPluginContainer()->triggerTweak(1);
		}
		::SetClipboardData(CF_UNICODETEXT,hCliptextW);//???WTF??? szCliptextW
		::GlobalUnlock(szCliptextW);
	}
	CloseClipboard();
	return 1;
}

typedef BOOL (WINAPI *_BlockInput)(BOOL);
BOOL BlockInputMy(BOOL bBlock)
{
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	_BlockInput fp=NULL;
	if(hUser32){
		fp=(_BlockInput)GetProcAddress(hUser32,"BlockInput");
	}
	if(hUser32 && fp){
		return (*fp)(bBlock);
	}
	return FALSE;
}

WCHAR* g_wszSavedInfo=0;
WCHAR* g_wszSavedChars=0;
BOOL PasteText(HWND hEditBox,const char* sPath,BOOL bType=0)
{
	g_wszSavedChars=GetClipboardText(hEditBox,1);
	SendMessage(hEditBox,EM_SETSEL,0,0x0FF0);
	WKGetPluginContainer()->triggerTweak(1);
	SendMessage(hEditBox,WM_CUT,0,0);
	g_wszSavedInfo=GetClipboardText(hEditBox,1);
	SetClipText(hEditBox,sPath,0,1);
	if(bType==0){
		SendMessage(hEditBox,WM_PASTE,0,0);
	}else{
		Sleep(5);
		::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),0,0);
		Sleep(5);
		::keybd_event('V', ::MapVirtualKey('V',0),0,0);
		Sleep(5);
		::keybd_event('V', ::MapVirtualKey('V',0),KEYEVENTF_KEYUP,0);
		Sleep(5);
		::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
		Sleep(5);
	}
	return 1;
}

void PasteRestoredText(HWND hEditBox, BOOL bRestore)
{
	if(bRestore){
		SendMessage(hEditBox,EM_SETSEL,0,0x0FF0);
		SendMessage(hEditBox,WM_CLEAR,0,0);
		if(g_wszSavedInfo){
			SetClipText(hEditBox,0,g_wszSavedInfo,1);
			SendMessage(hEditBox,WM_PASTE,0,0);
		}
	}
	if(g_wszSavedChars){
		SetClipText(hEditBox,0,g_wszSavedChars,1);
	}
	delete[] g_wszSavedInfo;
	delete[] g_wszSavedChars;
	g_wszSavedInfo=0;
	g_wszSavedChars=0;
}

#define MPOINT_X(aa)	long(65535*aa.x/(rtDesktop.right-rtDesktop.left))
#define MPOINT_Y(aa)	long(65535*aa.y/(rtDesktop.bottom-rtDesktop.top))
void SwitchSelectedExpView(HWND hFore,const char* szFile, BOOL bForceOpen=0)
{
	CCOMString sPath=szFile;
	if(szFile[strlen(szFile)-1]!='\\'){
		sPath+="\\";
	}
	if(bForceOpen){
		::ShellExecute(NULL,NULL,sPath,0,0,SW_SHOWNORMAL);
		return;
	}
	BOOL bOK=0;
	char szClass[128]="";
	::GetClassName(hFore,szClass,sizeof(szClass));
	//LOGSTAMP1(szClass)
	DWORD dwCurWinProcID=0;
	DWORD dwCurWinThreadID=GetWindowThreadProcessId(hFore,&dwCurWinProcID);
	DWORD dwThisThread=GetCurrentThreadId();
	DWORD dwThisProcess=GetCurrentProcessId();
	sPath.TrimLeft();
	sPath.TrimRight();
	//LOGSTAMP
	HWND hSubFolder=FindWindowEx(hFore,0,"SHELLDLL_DefView",0);
	if(hSubFolder){
		//LOGSTAMP
		HWND hEditBox=0;
		g_bOkCancelFound=0;
		EnumChildWindows(hFore,EnumFindEditBox,(LPARAM)&hEditBox);
		if(hEditBox){
			//LOGSTAMP
			::PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hEditBox);
			BOOL bRestoree=1;
			PasteText(hEditBox,sPath);
			Sleep(100);
			if(g_bOkCancelFound>=1){
				//LOGSTAMP
				SendMessage(hFore,WM_COMMAND,IDOK,0);
			}else{
				//LOGSTAMP
				bRestoree=0;
				SendMessage(hEditBox,WM_KEYDOWN,0xD,0x001C0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)
				SendMessage(hEditBox,WM_KEYUP,0xD,0xC01C0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)|(1<<30)|(1<<31)
				::PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hSubFolder);
				::SetFocus(hSubFolder);
			}
			Sleep(100);
			PasteRestoredText(hEditBox,bRestoree);
			bOK=1;
		}
	}else{
		//LOGSTAMP
		hSubFolder=FindWindowEx(hFore,0,"Snake List",0);
		if(hSubFolder){
			//LOGSTAMP
			HWND hEditBox=0;
			g_hMSWordButton=0;
			EnumChildWindows(hFore,EnumFindEditBox,(LPARAM)&hEditBox);
			if(hEditBox){
				//LOGSTAMP
				::PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hEditBox);
				Sleep(100);
				Sleep(100);
				RECT rEdit;
				::GetWindowRect(hEditBox,&rEdit);
				POINT pt,pt2;
				pt.x=(rEdit.left+rEdit.right)/2;
				pt.y=(rEdit.top+rEdit.bottom)/2;
				RECT rtDesktop;
				::GetWindowRect(::GetDesktopWindow(),&rtDesktop);
				GetCursorPos(&pt2);
				BlockInputMy(TRUE);
				::mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,MPOINT_X(pt),MPOINT_Y(pt),0,0);
				Sleep(100);
				::mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
				Sleep(10);
				::mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
				Sleep(100);
				::mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,MPOINT_X(pt2),MPOINT_Y(pt2),0,0);
				PasteText(hEditBox,sPath,1);
				Sleep(100);
				::keybd_event(VK_END, ::MapVirtualKey(VK_END,0),0,0);
				::keybd_event(VK_END, ::MapVirtualKey(VK_END,0),KEYEVENTF_KEYUP,0);
				Sleep(100);
				::keybd_event(VK_RIGHT, ::MapVirtualKey(VK_RIGHT,0),0,0);
				::keybd_event(VK_RIGHT, ::MapVirtualKey(VK_RIGHT,0),KEYEVENTF_KEYUP,0);
				Sleep(100);
				::keybd_event(VK_RETURN, ::MapVirtualKey(VK_RETURN,0),0,0);
				::keybd_event(VK_RETURN, ::MapVirtualKey(VK_RETURN,0),KEYEVENTF_KEYUP,0);
				BlockInputMy(FALSE);
				Sleep(1500);
				PasteRestoredText(hEditBox,1);
				bOK=1;
			}
		}else{
			//LOGSTAMP
			if(stricmp(szClass,"TTOTAL_CMD")==0){
				//LOGSTAMP
				HWND hEditBox=0;
				g_invisEdits=0;
				EnumChildWindows(hFore,EnumFindEditBox,(LPARAM)&hEditBox);
				//AttachThreadInput(dwThisThread,dwCurWinThreadID,TRUE);
				if(hEditBox==0){
					hEditBox=g_invisEdits;
				}
				if(hEditBox){
					//LOGSTAMP
					PasteText(hEditBox,CCOMString("cd \"")+sPath+"\"");
					SendMessage(hEditBox,WM_KEYDOWN,0xD,0x001C0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)
					SendMessage(hEditBox,WM_KEYUP,0xD,0xC01C0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)|(1<<30)|(1<<31)
					PasteRestoredText(hEditBox,1);
					bOK=1;
				}
				//AttachThreadInput(dwThisThread,dwCurWinThreadID,FALSE);
			}
			hSubFolder=FindWindowEx(hFore,0,"SysListView32",0);
			if(stricmp(szClass,"WinRarWindow")==0 || hSubFolder){
				//LOGSTAMP
				HWND hEditBox=0;
				g_invisEdits=0;
				EnumChildWindows(hFore,EnumFindEditBox,(LPARAM)&hEditBox);
				if(hEditBox==0){
					hEditBox=g_invisEdits;
				}
				if(hEditBox){
					//LOGSTAMP
					PasteText(hEditBox,sPath);
					SendMessage(hEditBox,WM_KEYDOWN,0xD,0x001C0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)
					SendMessage(hEditBox,WM_KEYUP,0xD,0xC01C0001);//(1)|(MapVirtualKey(VK_BACK,0)<<16)|(1<<30)|(1<<31)
					//PasteRestoredText(hEditBox,1);
					bOK=1;
				}
			}
		}
	}
	if(!bOK){
		::ShellExecute(NULL,NULL,sPath,0,0,SW_SHOWNORMAL);
	}
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hFore);
}

void SaveFavFolders();
DWORD WINAPI CallOptions(LPVOID pData)
{
	static BOOL bInProc=0;
	if(bInProc){
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hOpenedOptions);
		return 0;
	}
	bInProc++;
	HWND hParent=(HWND)pData;
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPTIONS),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	// Pushing changed options back into WireKeys
	SaveFavFolders();
	WKGetPluginContainer()->PushMyOptions(0);
	bInProc--;
	return 1;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	CallOptions(hParent);
	return 1;
}

BOOL isUnique(string_list& sl,const char* szItem)
{
	const char* szItemKey=strchr(szItem,'\t');
	if(!szItemKey){
		szItemKey=szItem;
	}
	for(int f=0;f<sl.size();f++){
		const char* szVal=(sl[f].c_str());
		const char* szValKey=strchr(szVal,'\t');
		if(!szValKey){
			szValKey=szVal;
		}
		int iRes=stricmp(szValKey,szItemKey);
		if(iRes==0){
			return FALSE;
		}
	}
	return TRUE;
}

long RecentsInProgress=0;

void ExtractRecentFolder(char* szFileItemOut)
{
	char szD[MAX_PATH*2]={0};
	char szP[MAX_PATH*2]={0};
	if(!isFolder(szFileItemOut)){
		_splitpath(szFileItemOut,szD,szP,0,0);
		strcpy(szFileItemOut,szD);
		strcat(szFileItemOut,szP);
	}
	if(szFileItemOut[strlen(szFileItemOut)-1]=='/'){
		szFileItemOut[strlen(szFileItemOut)-1]='\\';
	}
	if(szFileItemOut[strlen(szFileItemOut)-1]!='\\'){
		strcat(szFileItemOut,"\\");
	}
}

#ifdef _DEBUG
bool ccSort(const char& c1, const char& c2)
{
	return c1<c2;
}
#endif

string_list m_szRecentFolders;
DWORD WINAPI InitRecents(LPVOID p)
{
/*
#ifdef _DEBUG
	CCOMList<char> aaa;
	aaa.push_back('b');
	aaa.push_back('n');
	aaa.push_back('d');
	aaa.push_back('l');
	aaa.push_back('h');
	aaa.push_back('a');
	CCOMString res1=aaa.toString(",");
	aaa.sort(ccSort);
	CCOMString res2=aaa.toString(",");
#endif
*/
	if(RecentsInProgress>0){
		return 0;
	}
	//string_list* mp_szRecentFolders=&m_szRecentFolders;//(string_list*)p;
	// Иницим хип
	if(GetProcessHeap()==0){
#ifdef _DEBUG
		if(WKGetPluginContainer()){
			WKGetPluginContainer()->ShowOSD("Updating recents - failed...",15000);
		}
#endif
		return 0;
	}
#ifdef _DEBUG
	if(WKGetPluginContainer()){
		WKGetPluginContainer()->ShowOSD("Updating recents...",15000);
	}
#endif
	CSimpleTracker cl(RecentsInProgress);
	OleInitialize(0);
	// Готовим список...
	{
		string_list m_szRecentFoldersTmp;
		int nCount=0;
		// Из папки с рисентами...
		char strRecentFolder[MAX_PATH*2]={0};
		GetSpecialFolderPath(strRecentFolder,CSIDL_RECENT);
		CCOMString strDirName=strRecentFolder;
		strDirName+="\\";
		CSADirRead dc;
		dc.GetDirs(strDirName,FALSE);
		dc.GetFiles("*.*");
		dc.SortFiles(CSADirRead::eSortWriteDate,TRUE);
		CSADirRead::SAFileVector &fileVector = dc.Files();
		char szFileItem[MAX_PATH*2]={0};
		char szFileItemOut[MAX_PATH*2]={0};
		for (int fit = 0; fit!=fileVector.size(); fit++)
		{
			if(m_szRecentFoldersTmp.size()>=plgOptions.dwRItemsToGet){
				break;
			}
			strcpy(szFileItem,fileVector[fit].m_sName.c_str());
			strcpy(szFileItemOut,szFileItem);
			if ((!isLNK(szFileItemOut) || SUCCEEDED(UnpackLinkTM(szFileItem,szFileItemOut,200)))){
				if(isOnRemovable(szFileItemOut) || isFileExist(szFileItemOut)){
					ExtractRecentFolder(szFileItemOut);
					if(isUnique(m_szRecentFoldersTmp,szFileItemOut)){
						CCOMString sTmp(szFileItemOut);
						m_szRecentFoldersTmp.push_back(sTmp);
						nCount++;
					}
				}
			}
		}
		// Из реестра...
		// HKEY_CURRENT_USER\Software\Microsoft\Windows\
		// CurrentVersion\Explorer\ComDlg32\LastVisitedMRU
		//FLOG("Extrracting recent1 - finished");ANDFLUSH;
		CRegKey reg;
		LONG lRet = reg.Open(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\LastVisitedMRU"),KEY_QUERY_VALUE);
		if ( ERROR_SUCCESS == lRet ){
			USES_CONVERSION;
			DWORD dwCopied=0;
			wchar_t szMRUValueW[MAX_PATH*2]={0};
			char szMRUValue[MAX_PATH*2]={0}, szMRUFolder[MAX_PATH*2]={0}, szC[20]={0};
			dwCopied=sizeof(szMRUValue)-2;
			reg.QueryValue(szMRUValue,"MRUList",&dwCopied);
			char* szPos=szMRUValue;
			while(*szPos){
				if(m_szRecentFoldersTmp.size()>=plgOptions.dwRItemsToGet){
					break;
				}
				dwCopied=0;
				szC[0]=*szPos;
				dwCopied=sizeof(szMRUValueW)-2;
				if(QueryBinaryValue(reg,szC,szMRUValueW,&dwCopied)==ERROR_SUCCESS){
					wchar_t* pos=szMRUValueW;
					while(pos[0]!=0){
						pos++;
					}
					if(pos[1]!=0){
						strcpy(szMRUFolder,W2A(pos+1));
						//FLOG1("Extrracting recent2 '%s'",szFileItemOut);ANDFLUSH
						ExtractRecentFolder(szMRUFolder);
						if(isUnique(m_szRecentFoldersTmp,szMRUFolder)){
							CCOMString sTmp(szMRUFolder);
							m_szRecentFoldersTmp.push_back(sTmp);
						}
					}
				}
				szPos++;
			}
		}
		m_szRecentFolders.clear();
		m_szRecentFolders.merge(m_szRecentFoldersTmp);
#ifdef _DEBUG
		if(WKGetPluginContainer())
			WKGetPluginContainer()->ShowOSD("",0);
#endif	
	}
	OleUninitialize();
	return 0;
}

DWORD WINAPI InitRecents0(LPVOID p)
{
	Sleep(10000);
	return InitRecents(p);
}

HANDLE hStopEvent=0;
WKCallbackInterface* pInter=0;
DWORD WINAPI GlobalHooker_SHELLEX(LPVOID pData)
{
	CoInitializeEx(0,COINIT_APARTMENTTHREADED);
	if(szExit[0]==0){
		strcpy(szExit,_l("Close menu"));
	}
	if(szAddHere[0]==0){
		strcpy(szAddHere,_l("Add current..."));
	}
	if(szManageMenu[0]==0){
		strcpy(szManageMenu,_l("Manage folders"));
	}
	if(szSaveicons[0]==0){
		strcpy(szSaveicons,_l("Save icons"));
	}
	if(szRestoreIcons[0]==0){
		strcpy(szRestoreIcons,_l("Restore icons"));
	}
	if(szFindLinkTarget[0]==0){
		strcpy(szFindLinkTarget,_l("Find link target"));
	}
	if(szRegisterDll[0]==0){
		strcpy(szRegisterDll,_l("Register Dll"));
	}
	if(szUnregisterDll[0]==0){
		strcpy(szUnregisterDll,_l("Unregister Dll"));
	}
	if(szAssignHotkey[0]==0){
		strcpy(szAssignHotkey,_l("Assign hotkey"));
	}
	if(szCopyPaths[0]==0){
		strcpy(szCopyPaths,_l("Copy path(s)"));
	}
	if(szCopyNames[0]==0){
		strcpy(szCopyNames,_l("Copy name(s)"));
	}
	if(szStartConsole[0]==0){
		strcpy(szStartConsole,_l("Start console"));
	}
	if(szFolderListing[0]==0){
		strcpy(szFolderListing,_l("Folder listing"));
	}
	if(szChangeIcon[0]==0){
		strcpy(szChangeIcon,_l("Change icon"));
	}
	if(szMountAsDrive[0]==0){
		strcpy(szMountAsDrive,_l("Mount as drive"));
	}
	if(szPutInSendTo[0]==0){
		strcpy(szPutInSendTo,_l("Put in 'Send to'"));
	}
	if(szJumpToFolder[0]==0){
		strcpy(szJumpToFolder,_l("Jump to folder"));
	}
	if(szAddPBar[0]==0){
		strcpy(szAddPBar,_l("Add to Places"));
	}
	if(szRemPBar[0]==0){
		strcpy(szRemPBar,_l("Reset Places"));
	}
	if(szWKSpecials[0]==0){
		strcpy(szWKSpecials,_l("WK Specials"));
	}
	if(szConcat[0]==0){
		strcpy(szConcat,_l("Concatenate"));
	}
	if(szAssignOpen[0]==0){
		strcpy(szAssignOpen,_l("Set 'Open' handler"));
	}
	if(szShowDiff[0]==0){
		strcpy(szShowDiff,_l("Show difference"));
	}
	if(szCopyNFol[0]==0){
		strcpy(szCopyNFol,_l("Copy files - no tree"));
	}
	if(szFDelete[0]==0){
		strcpy(szFDelete,_l("Delete NOW"));
	}
	if(szFPack[0]==0){
		strcpy(szFPack,_l("Pack"));
	}
	if(szFUNPack[0]==0){
		strcpy(szFUNPack,_l("UnPack"));
	}
	if(szAdd2Path[0]==0){
		strcpy(szAdd2Path,_l("Add dir to PATH"));
	}
	if(szOpts[0]==0){
		strcpy(szOpts,_l("Options"));
	}
	if(DllRegisterServer()!=S_OK){
		char szError[256]={0};
		sprintf(szError,"Failed to initialize Shell extension (#%lu)\nThis plugin requires admin rights",GetLastError());
		WKGetPluginContainer()->ShowAlert(szError,"WK Shell extension");
		return 0;
	}
	MSG msg;
	DWORD dwLastUpdateTime=0;
	dwLastUpdateTime=GetTickCount();
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	plgOptions.dwWKThread=GetCurrentThreadId();
	FORK(InitRecents0,&m_szRecentFolders);
	WKGetPluginContainer()->setOptionEx(1346,plgOptions.dwWKThread);
	while(GetMessage(&msg,0,0,0)>0){
		//FLOG("GlobalHooker_SHELLEX");ANDFLUSH
		if(msg.message==WM_CLOSE){
			break;
		}
		if(msg.message==WM_ENABLE){
			if(msg.wParam){
				InterlockedDecrement(&plgOptions.bMenuBlocked);
			}else{
				InterlockedIncrement(&plgOptions.bMenuBlocked);
			}
		}
		if(msg.message==WM_FONTCHANGE){
			int iIndex=msg.lParam;
			HANDLE hRecents=CreateEvent(NULL,FALSE,FALSE,"WK_RECENTS_EVENT");
			g_CurrentFile[0]=0;
			if(iIndex<m_szRecentFolders.size()){
				for(int i=0;i<m_szRecentFolders.size();i++,iIndex--){
					if(iIndex==0){
						strcpy(g_CurrentFile,m_szRecentFolders[i].c_str());
						break;
					}
				}
			}
			SetEvent(hRecents);
			CloseHandle(hRecents);
			if(g_CurrentFile[0]==0){
				// Чтобы рисенты считались заново
				msg.message=WM_NULL;
			}
		}
		if(msg.message==WM_TIMECHANGE || msg.message!=WM_FONTCHANGE){// Обновляем? возможно после получения последнего!
			if(msg.message==WM_TIMECHANGE || dwLastUpdateTime==0 || (GetTickCount()-dwLastUpdateTime)>5*60*1000){
				FORK(InitRecents,&m_szRecentFolders);
				dwLastUpdateTime=GetTickCount();
			}
		}
		if(msg.message==WM_COMMAND){
			CCOMString sFile=g_CurrentFile;
			int iAction=msg.lParam;
			if(iAction==1){
				WKGetPluginContainer()->CallWKAction(ICONS_SAVE);
			}
			if(iAction==2){
				WKGetPluginContainer()->CallWKAction(ICONS_RESTORE);
			}
			if(iAction==3){
				WKGetPluginContainer()->AddQrunFromFile(sFile.GetString());
			}
			if(iAction==4){
				WKGetPluginContainer()->ChangeItemIcon(sFile.GetString());
			}
			if(iAction==5){
				WKGetPluginContainer()->PutText(sFile.GetString());
				::keybd_event(VK_RETURN, ::MapVirtualKey(VK_RETURN,0),0,0);
				::keybd_event(VK_RETURN, ::MapVirtualKey(VK_RETURN,0),KEYEVENTF_KEYUP,0);
			}
			if(iAction==6){
				SwitchSelectedExpView(HWND(msg.wParam),sFile.GetString(),(GetAsyncKeyState(VK_SHIFT)<0 || GetAsyncKeyState(VK_CONTROL)<0)?1:0);
			}
			if(iAction==7){
				try{
					CRegKey reg;
					LONG lRet=0;
					lRet = reg.Open ( HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),KEY_CREATE_SUB_KEY|KEY_QUERY_VALUE|KEY_SET_VALUE);
					if ( ERROR_SUCCESS != lRet ){
						throw 0;
					}
					CRegKey reg2;
					lRet = reg2.Open ( HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Comdlg32"),KEY_CREATE_SUB_KEY|KEY_QUERY_VALUE|KEY_SET_VALUE);
					if ( ERROR_SUCCESS != lRet ){
						lRet = reg2.Create(reg.m_hKey,"Comdlg32");
					}
					if ( ERROR_SUCCESS != lRet ){
						throw 0;
					}
					CRegKey reg3;
					lRet = reg3.Open ( HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Comdlg32\\Placesbar"),KEY_CREATE_SUB_KEY|KEY_QUERY_VALUE|KEY_SET_VALUE);
					if ( ERROR_SUCCESS != lRet ){
						lRet = reg3.Create(reg2.m_hKey,"Placesbar");
					}
					if ( ERROR_SUCCESS != lRet ){
						throw 0;
					}
					int iFreeBar=0;
					char szP[32]={0},szValue[MAX_PATH]={0};
					while(iFreeBar<5){
						sprintf(szP,"Place%i",iFreeBar);
						szValue[0]=0;
						DWORD dwCopied=0;
						reg3.QueryValue(szValue,szP,&dwCopied);
						if(dwCopied==0){
							break;
						}
						iFreeBar++;
					}
					if(iFreeBar>=5){
						throw 1;
					}
					lRet = reg3.SetValue ( sFile.GetString(),szP);
					WKGetPluginContainer()->ShowAlert(_l("Please, reopen dialog to apply changes"),"");
				}catch(int e){
					if(e==0){
						WKGetPluginContainer()->ShowAlert(_l("Not enough rights to work with Places"),_l("Plugin error"));
					}else if(e==1){
						WKGetPluginContainer()->ShowAlert(_l("There are no free bars left"),_l("Plugin error"));
					}
				}
			}
			if(iAction==8){
				try{
					LONG lRet=0;
					CRegKey reg3;
					lRet = reg3.Open ( HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies"),KEY_ALL_ACCESS);
					if ( ERROR_SUCCESS != lRet ){
						throw 1;
					}
					lRet = reg3.RecurseDeleteKey("Comdlg32");
					if ( ERROR_SUCCESS != lRet ){
						throw 1;
					}
					WKGetPluginContainer()->ShowAlert(_l("Please, reopen dialog to apply changes"),"");
				}catch(int){
					WKGetPluginContainer()->ShowAlert(_l("Not enough rights to work with Places"),_l("Plugin error"));
				}
			}
			if(iAction==9){
				CallOptions(0);
			}
			if(iAction==10){
				char szFolder[MAX_PATH+1]={0};
				char szFolderO[MAX_PATH+1]={0};
				strcpy(szFolderO,sFile.GetString());
				if(szFolderO[strlen(szFolderO)-1]!='\\'){
					strcat(szFolderO,"\\");
				}
				strcpy(szFolder,szFolderO);
				HWND hPar=WKGetPluginContainer()->GetWKMainWindow();
				::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hPar);
				BOOL bRes=GetFolderRaw(_l("Choose target folder"),szFolder,0,hPar,0);
				if(szFolder[strlen(szFolder)-1]!='\\'){
					strcat(szFolder,"\\");
				}
				if(bRes){
					WKGetPluginContainer()->ShowBaloon(_l("File transfer in progress"),_l("Operation status"),5000);
					//char szFolderI[MAX_PATH+1]={0};
					//strcpy(szFolderI,szFolderO);
					//strcat(szFolderI,"*.*");
					CSADirRead dc;
					dc.GetDirs(szFolderO,TRUE);
					dc.GetFiles("*.*");
					CSADirRead::SAFileVector &fileVector = dc.Files();
					for (int fit = 0; fit!=fileVector.size(); fit++)
					{
						CCOMString sNewFile=fileVector[fit].m_sName;
						const char* szNP=sNewFile.GetString();
						szNP+=strlen(szFolderO);
						sNewFile=CCOMString(szNP);
						sNewFile.Replace('\\','_');
						sNewFile.Replace('/','_');
						CCOMString sTargFile=CCOMString(szFolder)+sNewFile;
						int iCount=1;
						while(isFileExist(sTargFile)){
							char s[20]={0};
							sprintf(s,"Copy(%i)_",iCount);
							sTargFile=CCOMString(CCOMString(CCOMString(szFolder)+CCOMString(s))+sNewFile);
							iCount++;
						}
						CopyFile(fileVector[fit].m_sName,sTargFile,TRUE);
					}
					WKGetPluginContainer()->ShowBaloon(_l("File transfer completed")+"!",_l("Operation status"),5000);
				}
			}
			if(iAction==11){
				WKGetPluginContainer()->ShowBaloon(_l("Operation started")+"!",_l("Operation status"),5000);
			}
			if(iAction==12){
				WKGetPluginContainer()->ShowBaloon(_l("Operation completed")+"!",_l("Operation status"),5000);
			}
			if(iAction==13){
				const char* evName="PATH";
				long lres = 0;
				HKEY hregkey;
				lres = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, KEY_ALL_ACCESS, &hregkey);
				char szOrigname[33000]={0};
				DWORD dwnBytes=sizeof(szOrigname), dwType=0;
				lres = ::RegQueryValueEx( hregkey, evName, 0, &dwType, (LPBYTE)szOrigname, &dwnBytes );
				if( lres == ERROR_SUCCESS ){
					char* szBegin=0;
					if((szBegin=strstr(szOrigname,sFile.GetString()))!=0){
						if(MessageBox(0,_l("Environment already contain this path.\nDo you want to remove it")+"?",_l("Modifying Environment strings"),MB_YESNO|MB_ICONQUESTION)==IDYES){
							char* szEnd=strstr(szBegin,";");
							if(szEnd && *szEnd==';'){
								szEnd++;
							}
							char szOrigname2[33000]={0};
							memcpy(szOrigname2,szOrigname,(szBegin-szOrigname));
							if(szEnd){
								strcat(szOrigname2,szEnd);
							}
							strcpy(szOrigname,szOrigname2);
						}
					}else{
						if(strlen(szOrigname)>0){
							strcat(szOrigname,";");
						}
						strcat(szOrigname,sFile.GetString());
					}
				}
				//SetEnvironmentVariable("PATH",szOrigname);
				if (lres == ERROR_SUCCESS && strlen(szOrigname)<1023) {
					RegSetValueEx(hregkey, evName, 0, REG_SZ, (const unsigned char *)szOrigname, strlen(szOrigname));
					// propagate changes so that environment variables takes immediate effect!
					DWORD dwReturnValue=0;
					SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment", SMTO_ABORTIFHUNG, 5000, &dwReturnValue);
					WKGetPluginContainer()->ShowBaloon(_l("Operation completed")+"!",_l("Operation status"),5000);
				}
				else{
					WKGetPluginContainer()->ShowBaloon(_l("Operation failed")+"!",_l("Operation status"),5000);
				}
				RegCloseKey(hregkey);
			}
			if(iAction==14){
				CFileMap fMap;
				fMap.Open("WK_SHELL_DOMACROS",true,dwFMapSize);
				char* sz=(char*)fMap.Base();
				WKGetPluginContainer()->StartQuickRunByNameX(sFile,NULL,sz,0);
				sz[0]=0;
			}
			if(iAction==15){
				WKGetPluginContainer()->CallWKActionX(-1,sFile);
				WKGetPluginContainer()->ShowBaloon(_l("Operation completed")+"!",_l("Operation status"),5000);
			}
			if(iAction==16){
				WKGetPluginContainer()->ShowPreferencesTopic(0);
			}
		}
	}
	CoUninitialize();
	plgOptions.dwWKThread=0;
	/* Хватит! анрег при деинсталле!
	if(!pInter->getOption(WKOPT_ISSHUTDOWN)){
		if(DllUnregisterServer()!=S_OK){
			char szError[256]={0};
			sprintf(szError,"Failed to unregister Shell extension (#%lu)",GetLastError());
			WKGetPluginContainer()->ShowAlert(szError,"WK Shell extension");
		}
	}
	*/
	SetEvent(hStopEvent);
	return 0;
}


int	WINAPI _WKPluginPause(BOOL bPaused)
{
	// Специально ниче не делаем
	return 1;
}

HANDLE hHookerThread=0;
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	// Version...
	pInter=init;
	// Version...
	char szVer[32]="";
	init->GetWKVersion(szVer);
	DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
	if(dwBuild<348){
		init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
		return 0;
	}
	hWKMainWnd=pInter->GetWKMainWindow();
	DWORD dwID=0;
	hStopEvent=CreateEvent(0,0,0,0);
	hHookerThread=CreateThread(0,0,GlobalHooker_SHELLEX,0,0,&dwID);
#ifdef _DEBUG
	char s[100]={0};
	sprintf(s,"%s:%i %08x Debug thread point",__FILE__,__LINE__,dwID);
	init->ShowPopup(s,"Debug thread",0);
#endif
	return 1;
}

int	WINAPI WKPluginStop()
{
	if(!pInter->getOption(WKOPT_ISSHUTDOWN)){
		PostThreadMessage(plgOptions.dwWKThread,WM_CLOSE,0,0);
		WaitForSingleObject(hStopEvent,2000);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		//TerminateThread(hThread,66);
		//SuspendThread(hHookerThread);
	}
	CloseHandle(hStopEvent);
	CloseHandle(hHookerThread);
	hStopEvent=0;
	return 1;
}

extern HINSTANCE g_hinstDll;
char szHotkeyStrPresentation[32]={0};
int	WINAPI WKGetPluginFunctionHints(long iPluginFunction, long lHintCode, void*& pOut)
{
	if(iPluginFunction==0){
		strcpy(szHotkeyStrPresentation,"%0-4,491");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	return 0;
};

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>9 || stuff==NULL){
		return 0;
	}
	stuff->dwDefaultHotkey=0;
	if(iPluginFunction==0){
		stuff->dwDefaultHotkey=0;
		strcpy(stuff->szItemName,("Popup list of favourite folders"));
	}
	if(iPluginFunction==1){
		strcpy(stuff->szItemName,("Jump to favourite folder #1"));
	}
	if(iPluginFunction==2){
		strcpy(stuff->szItemName,("Jump to favourite folder #2"));
	}
	if(iPluginFunction==3){
		strcpy(stuff->szItemName,("Jump to favourite folder #3"));
	}
	if(iPluginFunction==4){
		strcpy(stuff->szItemName,("Switch explorer into view #1"));
	}
	if(iPluginFunction==5){
		strcpy(stuff->szItemName,("Switch explorer into view #2"));
	}
	if(iPluginFunction==6){
		strcpy(stuff->szItemName,("Switch explorer into view #3"));
	}
	if(iPluginFunction==7){
		strcpy(stuff->szItemName,("Arrange explorer by type #1"));
	}
	if(iPluginFunction==8){
		strcpy(stuff->szItemName,("Arrange explorer by type #2"));
	}
	if(iPluginFunction==9){
		strcpy(stuff->szItemName,("Arrange explorer by type #3"));
	}
	stuff->hItemBmp=0;//::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	stuff->dwItemMenuPosition=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>9 || stuff==NULL){
		return 0;
	}
	return 1;
}

BOOL SetExplorerView(HWND hFore, int lDefView, BOOL bArrange=0)
{
	HWND hSubFolder=FindWindowEx(hFore,0,"SHELLDLL_DefView",0);
	if(hSubFolder){
		if(bArrange){
			if(isWinXP()){
				switch(lDefView)
				{
				case 0:
					PostMessage(hFore,0x0111,0x7606,0);
					break;
				case 1:
					PostMessage(hFore,0x0111,0x7604,0);
					break;
				case 2:
					PostMessage(hFore,0x0111,0x7603,0);
					break;
				case 3:
					PostMessage(hFore,0x0111,0x7605,0);
					break;
				}
			}else{
				switch(lDefView)
				{
				case 0:
					PostMessage(hFore,0x0111,0x7230,0);
					break;
				case 1:
					PostMessage(hFore,0x0111,0x7232,0);
					break;
				case 2:
					PostMessage(hFore,0x0111,0x7231,0);
					break;
				case 3:
					PostMessage(hFore,0x0111,0x7233,0);
					break;
				}
			}
		}else{
			switch(lDefView)
			{
			case 0:
				PostMessage(hSubFolder,0x0111,0x702E,0);
				break;
			case 1:
				PostMessage(hSubFolder,0x0111,0x7029,0);
				break;
			case 2:
				PostMessage(hSubFolder,0x0111,0x702b,0);
				break;
			case 3:
				PostMessage(hSubFolder,0x0111,0x702c,0);
				break;
			case 4:
				PostMessage(hSubFolder,0x0111,0x702D,0);
				break;
			}
		}
		return 1;
	}
	return 0;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	RETURN_IFRECURSE(0);
	//FLOG("WKCallPluginFunction-SHELLEX");

	WKGetPluginContainer()->WaitWhileAllKeysAreReleased();
	if(iPluginFunction==0){
		CoInitializeEx(0,COINIT_APARTMENTTHREADED);
		// Делаем окно до плучения списка чтобы курсор заменялся ожидающим....
		//FLOG("Opening Recents list");
		RECT rDesktopRECT;
		rDesktopRECT.left=rDesktopRECT.top=-20;
		rDesktopRECT.right=rDesktopRECT.bottom=-10;
		HWND wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_DISABLED, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.right-rDesktopRECT.left, rDesktopRECT.bottom-rDesktopRECT.top, 0, 0, g_hinstDll, 0);
		HCURSOR hWait=SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
		HMENU mn=::CreatePopupMenu();
		CWKShellExtender2 se;
		se.hFore=stuff?stuff->hForegroundWindow:GetForegroundWindow();
		se.QueryContextMenu(mn,0,1,1000,CMF_RESERVED);
		POINT pos;
		GetCursorPos(&pos);
		SetCursor(hWait);
		//FLOG("SetForegr!!!!");ANDFLUSH;
		SetForegroundWindow(wndShotPhase2);
		DWORD dwRetCode=TrackPopupMenu(mn, TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pos.x, pos.y, 0, wndShotPhase2 , NULL);
		DestroyWindow(wndShotPhase2);
		if(dwRetCode!=0){
			::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)se.hFore);
			CMINVOKECOMMANDINFO pCmdInfo;
			memset(&pCmdInfo,0,sizeof(pCmdInfo));
			//так как генерация шла с нуля, используем код==порядковый номер
			pCmdInfo.lpVerb=MAKEINTRESOURCE(dwRetCode-1);
			se.InvokeCommand ( &pCmdInfo);
		}
		//FLOG("Opening Recents list end");
		CoUninitialize();
	}	
	if(iPluginFunction==1){
		SwitchSelectedExpView(stuff->hForegroundWindow,plgOptions.szPath1);
	}
	if(iPluginFunction==2){
		SwitchSelectedExpView(stuff->hForegroundWindow,plgOptions.szPath2);
	}
	if(iPluginFunction==3){
		SwitchSelectedExpView(stuff->hForegroundWindow,plgOptions.szPath3);
	}
	// Первые 4е должны быть именно в таком порядке!
	if(iPluginFunction==4){
		SetExplorerView(stuff->hForegroundWindow,plgOptions.lDefView1);
	}
	if(iPluginFunction==5){
		SetExplorerView(stuff->hForegroundWindow,plgOptions.lDefView2);
	}
	if(iPluginFunction==6){
		SetExplorerView(stuff->hForegroundWindow,plgOptions.lDefView3);
	}
	if(iPluginFunction==7){
		SetExplorerView(stuff->hForegroundWindow,plgOptions.lDefViewT1,1);
	}
	if(iPluginFunction==8){
		SetExplorerView(stuff->hForegroundWindow,plgOptions.lDefViewT2,1);
	}
	if(iPluginFunction==9){
		SetExplorerView(stuff->hForegroundWindow,plgOptions.lDefViewT3,1);
	}
	return 0;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	dsc->dwVersionHi=1;
	dsc->dwVersionLo=1;
	//strcpy(dsc->szTitle,"Shell extender");
	strcpy(dsc->szDesk,"This plugin extends Save/Load dialogs and adds usefull items to Explorer context menu");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


const char* getDesktopPathX()
{
	static char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_DESKTOPDIRECTORY);
	return szFolderBuffer;
}

const char* getMyDocPathX()
{
	static char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_PERSONAL);
	if(strlen(szFolderBuffer)==0){
		return "::{450D8FBA-AD25-11D0-98A8-0800361B1103}";
	}
	return szFolderBuffer;
}


const char* getMyPicPathX()
{
	static char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,0x0036);// CSIDL_COMMON_PICTURES
	if(strlen(szFolderBuffer)==0){
		strcpy(szFolderBuffer,getMyDocPathX());
		strcat(szFolderBuffer,"\\Pictures");
	}
	return szFolderBuffer;
}

void SaveFavFolders()
{
	//Сохраняем папки фавориты
	char szPrefFolder[MAX_PATH]={0};
	WKGetPluginContainer()->GetPreferencesFolder(szPrefFolder, sizeof(szPrefFolder));
	//pOptionsCallback->GetBinOption("items",plgOptions.szCustomItems,MAX_CITEMS*MAX_PATH);
	strcat(szPrefFolder,"explFavFolders.ini");
	CCOMString sContent;
	for(int i=0;i<MAX_CITEMS;i++){
		CCOMString s=plgOptions.szCustomItems[i];
		s.TrimLeft();
		s.TrimRight();
		if(s.GetLength()>0){
			sContent+=plgOptions.szCustomItems[i];
			sContent+="\n";
		}
	}
	SaveFile(szPrefFolder,sContent);
}

void LoadFavFolders()
{
	//Грузим папки фавориты
	char szPrefFolder[MAX_PATH]={0};
	WKGetPluginContainer()->GetPreferencesFolder(szPrefFolder, sizeof(szPrefFolder));
	//pOptionsCallback->GetBinOption("items",plgOptions.szCustomItems,MAX_CITEMS*MAX_PATH);
	strcat(szPrefFolder,"explFavFolders.ini");
	CCOMString sContent;
	ReadFile(szPrefFolder,sContent);
	sContent+="\n";
	for(int i=0;i<MAX_CITEMS;i++){
		plgOptions.szCustomItems[i][0]=0;
	}
	int posa=0;
	int iP=0;
	int iN=sContent.Find('\n');
	while(iN!=-1 && iN!=iP && iN<sContent.GetLength()){
		CCOMString sLine=sContent.Mid(iP,iN-iP);
		sLine.TrimLeft();
		sLine.TrimRight();
		if(sLine.GetLength()){
			strcpy(plgOptions.szCustomItems[posa],sLine);
			posa++;
		}
		iP=iN;
		iN=sContent.Find('\n',iP+1);
	}
}

#define _TAB_ CCOMString("\t")
int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddTipEx("folder1","Folders and views for quick jump by hotkeys",0);
		pOptionsCallback->AddStringOptionEx("path1","Favourite folder #1","",getDesktopPathX(),2,0,"folder1");
		pOptionsCallback->AddStringOptionEx("path2","Favourite folder #2","",getMyDocPathX(),2,0,"folder1");
		pOptionsCallback->AddStringOptionEx("path3","Favourite folder #3","",getMyPicPathX(),2,0,"folder1");
		pOptionsCallback->AddListOptionEx("defview1","Explorer view #1","",_l("Large icons")+_TAB_+_l("Small icons")+_TAB_+_l("List")+_TAB_+_l("Details")+_TAB_+_l("Thumbnails"),2,0,"folder1");
		pOptionsCallback->AddListOptionEx("defview2","Explorer view #2","",_l("Large icons")+_TAB_+_l("Small icons")+_TAB_+_l("List")+_TAB_+_l("Details")+_TAB_+_l("Thumbnails"),3,0,"folder1");
		pOptionsCallback->AddListOptionEx("defview3","Explorer view #3","",_l("Large icons")+_TAB_+_l("Small icons")+_TAB_+_l("List")+_TAB_+_l("Details")+_TAB_+_l("Thumbnails"),4,0,"folder1");
		pOptionsCallback->AddListOptionEx("defviewT1","Arrange by #1","",_l("Name")+_TAB_+_l("Type")+_TAB_+_l("Size")+_TAB_+_l("Date"),0,0,"folder1");
		pOptionsCallback->AddListOptionEx("defviewT2","Arrange by #2","",_l("Name")+_TAB_+_l("Type")+_TAB_+_l("Size")+_TAB_+_l("Date"),1,0,"folder1");
		pOptionsCallback->AddListOptionEx("defviewT3","Arrange by #3","",_l("Name")+_TAB_+_l("Type")+_TAB_+_l("Size")+_TAB_+_l("Date"),2,0,"folder1");
		//-----------
		pOptionsCallback->AddNumberOption("recitems","How many recent folders to retrieve","",10,0,1000,0);
		if(!WKGetPluginContainer()->getOption(WKOPT_EXTHKPROC)){
			pOptionsCallback->AddTipEx("warn_exh","Warning: extended hotkey processing is not enabled. Some features of this plugin are disabled",0);
		}
		pOptionsCallback->AddBoolOption("bAddsysmen","'Save/Open' dialogs: Add Fav. item to system menu","This feature depends on 'Window helper' plugin",TRUE,0);
		pOptionsCallback->AddBoolOption("bAddbutton","'Save/Open' dialogs: Add clickable mark to call Fav. menu","This feature depends on 'Window helper' plugin",TRUE,0);
		//!!!!!!!!!
		//-----------
		pOptionsCallback->AddTipEx("folder2","Options for Explorer context menu extension",0);
		//pOptionsCallback->AddBoolOptionEx("favout","Show favourite folders ","",FALSE,0,"folder2");
		pOptionsCallback->AddBoolOptionEx("extrastuff","Show advanced items","",TRUE,0,"folder2");
		pOptionsCallback->AddBoolOptionEx("OnShift","Add WireKeys menu only when Shift key is pressed","",FALSE,0,"folder2");
		pOptionsCallback->AddBoolOptionEx("OnShift2","Add list of recent folders only when Shift keys is pressed","Retrieving list of recent folders can take long time in some cases. Using this option you can control where you need this list at all",FALSE,0,"folder2");
		//pOptionsCallback->AddBoolOptionEx("infirst","Move WireKeys menu to the bottom","",FALSE,0,"folder2");
		pOptionsCallback->AddBoolOptionEx("dobyone","Execute macro on group of files in one call","",FALSE,0,"folder2");
		pOptionsCallback->AddStringOptionEx("leditor","Default text editor","","notepad",1,0,"folder2");
		pOptionsCallback->AddStringOptionEx("ldiff","Default diff application","We recommend to use freeware 'WinMerge'","fc.exe",1,0,"folder2");
		//pOptionsCallback->AddBinOption("items");
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		plgOptions.bAddbutton=pOptionsCallback->GetBoolOption("bAddbutton");
		plgOptions.bAddsysmen=pOptionsCallback->GetBoolOption("bAddsysmen");
		plgOptions.bShowOnlyIfShift=pOptionsCallback->GetBoolOption("OnShift");
		plgOptions.bShowOnlyIfShift2=pOptionsCallback->GetBoolOption("OnShift2");
		plgOptions.lDefView1=pOptionsCallback->GetListOption("defview1");
		plgOptions.lDefView2=pOptionsCallback->GetListOption("defview2");
		plgOptions.lDefView3=pOptionsCallback->GetListOption("defview3");
		plgOptions.lDefViewT1=pOptionsCallback->GetListOption("defviewT1");
		plgOptions.lDefViewT2=pOptionsCallback->GetListOption("defviewT2");
		plgOptions.lDefViewT3=pOptionsCallback->GetListOption("defviewT3");
		plgOptions.dwRItemsToGet=pOptionsCallback->GetNumberOption("recitems");
		//plgOptions.bSetInFirst=pOptionsCallback->GetBoolOption("infirst");
		//plgOptions.lAddToRoot=pOptionsCallback->GetBoolOption("favout");
		plgOptions.bDobyone=pOptionsCallback->GetBoolOption("dobyone");
		plgOptions.bExtrastuff=pOptionsCallback->GetBoolOption("extrastuff");
		pOptionsCallback->GetStringOption("leditor",plgOptions.szListingEditor,sizeof(plgOptions.szListingEditor));
		pOptionsCallback->GetStringOption("ldiff",plgOptions.szDiffEditor,sizeof(plgOptions.szDiffEditor));
		pOptionsCallback->GetStringOption("path1",plgOptions.szPath1,sizeof(plgOptions.szPath1));
		pOptionsCallback->GetStringOption("path2",plgOptions.szPath2,sizeof(plgOptions.szPath2));
		pOptionsCallback->GetStringOption("path3",plgOptions.szPath3,sizeof(plgOptions.szPath3));
		LoadFavFolders();
		if(plgOptions.szDiffEditor[0]==0){
			strcpy(plgOptions.szDiffEditor,"fc.exe");
		}
		if(plgOptions.szListingEditor[0]==0){
			if(isWinXP()){
				strcpy(plgOptions.szListingEditor,"noteadt");
			}else{
				strcpy(plgOptions.szListingEditor,"notepad");
			}
		}
		if(iAction==OM_OPTIONS_SET){
			DllRegisterServerRaw(TRUE);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("bAddbutton",plgOptions.bAddbutton);
		pOptionsCallback->SetBoolOption("bAddsysmen",plgOptions.bAddsysmen);
		pOptionsCallback->SetBoolOption("OnShift",plgOptions.bShowOnlyIfShift);
		pOptionsCallback->SetBoolOption("OnShift2",plgOptions.bShowOnlyIfShift2);
		//pOptionsCallback->SetBoolOption("favout",plgOptions.lAddToRoot);
		pOptionsCallback->SetBoolOption("dobyone",plgOptions.bDobyone);
		pOptionsCallback->SetBoolOption("extrastuff",plgOptions.bExtrastuff);
		pOptionsCallback->SetListOption("defview1",plgOptions.lDefView1);
		pOptionsCallback->SetListOption("defview2",plgOptions.lDefView2);
		pOptionsCallback->SetListOption("defview3",plgOptions.lDefView3);
		pOptionsCallback->SetListOption("defviewT1",plgOptions.lDefViewT1);
		pOptionsCallback->SetListOption("defviewT2",plgOptions.lDefViewT2);
		pOptionsCallback->SetListOption("defviewT3",plgOptions.lDefViewT3);
		pOptionsCallback->SetNumberOption("recitems",plgOptions.dwRItemsToGet);
		//pOptionsCallback->SetBoolOption("infirst",plgOptions.bSetInFirst);
		pOptionsCallback->SetStringOption("leditor",plgOptions.szListingEditor);
		pOptionsCallback->SetStringOption("ldiff",plgOptions.szDiffEditor);
		pOptionsCallback->SetStringOption("path1",plgOptions.szPath1);
		pOptionsCallback->SetStringOption("path2",plgOptions.szPath2);
		pOptionsCallback->SetStringOption("path3",plgOptions.szPath3);
		//pOptionsCallback->SetBinOption("items",plgOptions.szCustomItems,MAX_CITEMS*MAX_PATH);
		SaveFavFolders();
	}
	GlobalRemoveAddAtom(plgOptions.bAddsysmen,"WK_SHELL_SR");
	GlobalRemoveAddAtom(plgOptions.bAddbutton,"WK_SHELL_BT");
	if(plgOptions.bAddbutton){
		GlobalRemoveAddAtom(plgOptions.szPath1[0],"WK_SHELL_BT1");
		GlobalRemoveAddAtom(plgOptions.szPath2[0],"WK_SHELL_BT2");
		GlobalRemoveAddAtom(plgOptions.szPath3[0],"WK_SHELL_BT3");
	}
	return 1;
}
