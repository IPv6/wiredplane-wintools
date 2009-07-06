#include "stdafx.h"
#include <process.h>
#include "_common.h"
#include "_externs.h"
#include "wirenotedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ClanchimDengi();
CStringArray aAdverts;
int iAdvCount=0;
int iAdvCountRaw=0;
BOOL bAdvertLoaded=0;

BOOL& IsEmbedded();
void StartCheckNewVersionX(DWORD dwDays);

void AppendAdverts(CMenu* m_pMenu)
{
#ifdef _NOADVERT
	return;
#endif
	if(IsEmbedded()){
		return;
	}
	if(bAdvertLoaded==0){
		bAdvertLoaded=1;
		CString sAdvertF=Format("%sadv_l%i.txt",GetApplicationDir(),GetApplicationLang());
		if(isFileExist(sAdvertF)){
			CString sText;
			ReadFile(sAdvertF,sText);
			sText.Replace("\r","\n");
			aAdverts.RemoveAll();
			ConvertToArray(sText,aAdverts);
			for(int i=0;i<aAdverts.GetSize()*3;i++){
				int iRnd=rnd(0,aAdverts.GetSize()-1);
				int iRnd2=rnd(0,aAdverts.GetSize()-1);
				if(iRnd!=iRnd2){
					CString s=aAdverts[iRnd];
					aAdverts[iRnd]=aAdverts[iRnd2];
					aAdverts[iRnd2]=s;
				}
			}
		}
	}
	iAdvCount++;
	m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");// Это - всегда
	if(aAdverts.GetSize()>0 && DaysFromInstall_Reg()<60){//(iAdvCount%3)==0//rnd(0,10)<8
		int i=iAdvCountRaw++;//rnd(0,aAdverts.GetSize()-1);
		if(iAdvCountRaw>=aAdverts.GetSize()){
			iAdvCountRaw=0;
		}
		CString sAdvert=aAdverts[i];
		if(sAdvert.GetLength()>0 && sAdvert[0]!=';'){
			CStringArray aAdvertItem;
			ConvertComboDataToArray(sAdvert,aAdvertItem,'|');
			for(int j=0;j<aAdvertItem.GetSize()-1;j++){
				if(j==0){
					AddMenuString(m_pMenu,WM_USER+ADV_BASE+i,aAdvertItem[j],_bmp().Get(IDB_BM_HELP));
				}else{
					AddMenuString(m_pMenu,WM_USER+ADV_BASE+i,aAdvertItem[j],0);
				}
			}
			m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		}
	}
}

DWORD WINAPI StartAdvert(LPVOID p)
{
	int i=(int)p;
	CString sAdvert=aAdverts[i];
	CStringArray aAdvertItem;
	ConvertComboDataToArray(sAdvert,aAdvertItem,'|');
	CString sAction=aAdvertItem[aAdvertItem.GetSize()-1];
	WNSubstMyVariables(sAction,"web-adv");
	ShellExecute(0,"open",sAction,NULL,NULL,SW_SHOWNORMAL);
	return 0;
}

BOOL ParseAdvert(long i)
{
	if(i>=WM_USER+ADV_BASE){
		i-=(WM_USER+ADV_BASE);
		if(i<aAdverts.GetSize()){
			FORK(StartAdvert,i);
			iAdvCount=1;
			return TRUE;
		}
	}
	return FALSE;
}


CNewVersionChecker& GetVersionChecker()
{
	static CNewVersionChecker verChecker(PROG_VERSION,__DATE__,EscapeString(Format("ls=%i, u=%s, days=%i",objSettings.iLikStatus,objSettings.sLikUser,GetWindowDaysFromInstall())));
	return verChecker; 
}
/////////////////////////////////////////////////////////////////////////////
// AppMainDlg dialog
void AppMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AppMainDlg)
	DDX_Control(pDX, IDC_FIND_BT, m_BtFind);
	DDX_Control(pDX, IDC_NEW_TODO, m_BtNewTd);
	DDX_Control(pDX, IDC_WNDONTOP, m_BtOnTop);
	DDX_Control(pDX, IDC_SAVEALL_BT, m_BtSaveAll);
	DDX_Control(pDX, IDC_OPTIONS_BT, m_BtOpt);
	DDX_Control(pDX, IDC_EDITFORM, m_BtEform);
	DDX_Control(pDX, IDC_NEW_REMINDER, m_BtNewRem);
	DDX_Control(pDX, IDC_NEW_LINK, m_BtNewLink);
	DDX_Control(pDX, IDC_NEW_FOLDER2, m_BtNewFolder);
	DDX_Control(pDX, IDC_EDIT_ITEM, m_BtEditItem);
	DDX_Control(pDX, IDC_DELETE_ITEM, m_BtDelItem);
	DDX_Control(pDX, IDC_NEW_NOTE, m_BtNewNote);
	DDX_Control(pDX, IDC_ADDRBOOK, m_BtAdrBook);
	DDX_Control(pDX, IDC_TASKLIST, m_TaskList);
	DDX_Control(pDX, IDC_RICHEDIT, m_FEdit);
	DDX_Control(pDX, IDC_SLIDER_TRANS, m_Slider);
	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AppMainDlg, CDialog)
	//{{AFX_MSG_MAP(AppMainDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDMINIMAZE, OnMinimaze)
	ON_COMMAND(ID_TODO_ADDOPTIONS, OnSystrayToDoAddOptions)
	ON_COMMAND(ID_SYSTRAY_EXIT, OnSystrayExit)
	ON_COMMAND(ID_SYSTRAY_ANTISPAMOPTIONS, OnSystrayAntiSpamOptions)
	ON_COMMAND(ID_SYSTRAY_OPEN, OnSystrayOpen)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SYSTRAY_NEWTASK, OnSystrayNewtask)
	ON_COMMAND(ID_SYSTRAY_NEWLINK, OnSystrayLink)
	ON_COMMAND(ID_TODO_DELETETODO, OnDeleteTask)
	ON_COMMAND(ID_FZFOLDER, OnNotemenuFZFolder)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_SYSTRAY_NEWNOTE, OnSystrayNewnote)
	ON_COMMAND(ID_SYSTRAY_OPTIONS, OnSystrayOptions)
	ON_BN_CLICKED(IDC_EDITFORM, OnEform)
	ON_COMMAND(ID_SYSTRAY_AUTOPOPUPORGANIZER, OnChangeAutopopupOrg)
	ON_COMMAND(ID_SYSTRAY_ADDRESSBOOK, OnSystrayAddressbook)
	ON_COMMAND(ID_SYSTRAY_SHOWALLNOTES, OnSystrayShowallnotes)
	ON_COMMAND(ID_SYSTRAY_REPORTALLITEMS, OnSystrayReport)
	ON_COMMAND(ID_SYSTRAY_SHOWNEXT, OnSystrayShowNextNote)
	ON_COMMAND(ID_SYSTRAY_HIDEALLNOTES, OnSystrayHideallnotes)
	ON_COMMAND(ID_SYSTRAY_DEFACTION, OnSystrayDefaction)
	ON_COMMAND(ID_SYSTRAY_NEWREMINDER, OnSystrayNewreminder)
	ON_WM_TIMER()
	ON_COMMAND(ID_TODO_NEWTODO, OnTodoNewtodo)
	ON_COMMAND(ID_TODO_SETASCURRENT, OnTodoSetascurrent)
	ON_COMMAND(ID_TODO_MODIFYTODO, OnTodoModifytodo)
	ON_COMMAND(ID_TODO_MARKASUNDONE, OnTodoMarkasundone)
	ON_COMMAND(ID_TODO_MARKASDONE, OnTodoMarkasdone)
	ON_COMMAND(ID_TODO_TODOREPORT, OnTodoTodoreport)
	ON_COMMAND(ID_SYSTRAY_NEWMESSAGE, OnSystrayNewmessage)
	ON_WM_CLOSE()
	ON_WM_ENDSESSION()
	ON_COMMAND(ID_REMINDERMENU_ACTIVATEREMINDER, OnRemindermenuActivatereminder)
	ON_COMMAND(ID_REMINDERMENU_DEACTIVATEREMINDER, OnRemindermenuDeactivatereminder)
	ON_COMMAND(ID_REMINDERMENU_DELETEREMINDER, OnRemindermenuDeletereminder)
	ON_COMMAND(ID_REMINDERMENU_MODIFYREMINDER, OnRemindermenuModifyreminder)
	ON_COMMAND(ID_FOLDER_CREATENEW, OnFolderCreatenew)
	ON_COMMAND(ID_FOLDER_DELETE, OnFolderDelete)
	ON_COMMAND(ID_PRIORITY_HIGH, OnPriorityHigh)
	ON_COMMAND(ID_PRIORITY_HIGHEST, OnPriorityHighest)
	ON_COMMAND(ID_PRIORITY_LOW, OnPriorityLow)
	ON_COMMAND(ID_PRIORITY_LOWEREST, OnPriorityLowerest)
	ON_COMMAND(ID_PRIORITY_NORMAL, OnPriorityNormal)
	ON_BN_CLICKED(IDC_DELETE_ITEM, OnBnDel)
	ON_BN_CLICKED(IDC_EDIT_ITEM, OnBnEdit)
	ON_BN_CLICKED(IDC_NEW_FOLDER2, OnNewBnFolder)
	ON_BN_CLICKED(IDC_NEW_LINK, OnNewBnLink)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_TASKLIST, OnColumnclickTasklist)
	ON_BN_CLICKED(IDC_NEW_NOTE, OnNewNote)
	ON_BN_CLICKED(IDC_NEW_TODO, OnNewTodo)
	ON_BN_CLICKED(IDC_NEW_REMINDER, OnNewReminder)
	ON_BN_CLICKED(IDC_NEW_MESSAGE, OnNewMessage)
	ON_BN_CLICKED(IDC_ADDRBOOK, OnAddrbook)
	ON_BN_CLICKED(IDC_OPTIONS_BT, OnOptionsBt)
	ON_BN_CLICKED(IDC_SAVEALL_BT, OnSaveallBt)
	ON_COMMAND(ID_FOLDER_ADDNOTE, OnFolderAddnote)
	ON_COMMAND(ID_FOLDER_ADDREMINDER, OnFolderAddreminder)
	ON_COMMAND(ID_FOLDER_ADDTASK, OnFolderAddtask)
	ON_COMMAND(IDR_FILELINKMENU_ACTIVATE, OnFilelinkmenuActivate)
	ON_COMMAND(IDR_FILELINKMENU_DEL, OnFilelinkmenuDel)
	ON_COMMAND(ID_FILELINKMENU_RENAME, OnFilelinkmenuRename)
	ON_WM_MOUSEACTIVATE()
	ON_COMMAND(ID_TODO_HIDESHOWDONE, OnTodoHideshowdone)
	ON_COMMAND(ID_REMINDERMENU_SHOWHIDEINACTIVE, OnRemindermenuShowhideinactive)
	ON_BN_CLICKED(IDC_WNDONTOP, OnWndontop)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_TODO_SETASNOTCURRENT, OnTodoSetasnotcurrent)
	ON_COMMAND(ID_HOTMENU_POPUPMAINWINDOW, OnHotmenuPopupmainwindow)
	ON_COMMAND(ID_FOLDER_SETASTRAYFOLDER, OnFolderSetastrayfolder)
	ON_COMMAND(ID_SYSTRAY_SWITCHMENU, OnSystraySwitchmenu)
	ON_COMMAND(ID_FOLDER_RENAMEFOLDER, OnFolderRenamefolder)
	ON_COMMAND(ID_SYSTRAY_ABOUT, OnSystrayAbout)
	ON_COMMAND(ID_SYSTRAY_REGISTER, OnSystrayRegister)
	ON_COMMAND(ID_TODO_TODOSETTINGS, OnTodoSettings)
	ON_COMMAND(ID_TODO_SAVETOFILE, OnTodoSaveToFile)
	
	ON_COMMAND(ID_TODO_REMINDABOUT, OnTodoRemindabout)
	ON_COMMAND(ID_SYSTRAY_WCHANGER, OnSystrayWchanger)
	ON_COMMAND(ID_SYSTRAY_CHECKINCOMINGEMAILS, OnSystrayIncEmails)
	ON_COMMAND(ID_SYSTRAY_CBILLB, OnSystrayCBillb)
	ON_COMMAND(ID_SYSTRAY_LINKCH, OnSystrayLinkCh)
	ON_COMMAND(ID_SYSTRAY_SENDASUGGESTION, OnSystraySendasuggestion)
	ON_COMMAND(ID_SYSTRAY_DONATE, OnSystrayDonate)
	ON_COMMAND(ID_NOTEMENU_NEWSTYLE, OnNotemenuNewstyle)
	ON_BN_CLICKED(IDC_FIND_BT, OnFindBt)
	ON_COMMAND(ID_SYSTRAY_LINKFROMCLIPBOARD, LinkFromClipboard)
	ON_COMMAND(ID_FILELINKMENU_COPYTOCLIPBOARD, OnCopyLinkToClip)
	ON_COMMAND(ID_FILELINKMENU_EDITLINK, OnEditLink)
	ON_COMMAND(ID_NOTEMENU_NEWFNT, OnNotemenuNewstyle)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(CREATE_NEWNOTE, OnCreateNewNote)
	ON_MESSAGE(CREATE_NEWTODO, OnCreateNewToDo)
	ON_NOTIFY(FLNM_ENDEDIT, IDC_TASKLIST, OnEndEditMainTreeList)
	ON_NOTIFY(FLNM_END_INPLACE_COMBO, IDC_TASKLIST, OnEndEditMainTreeListI)
	ON_NOTIFY(FLMN_CHECKBOX_TRIGGER, IDC_TASKLIST, OnEndEditMainTreeListC)
	ON_NOTIFY(FLNM_CUSTOMDIALOG, IDC_TASKLIST, OnCustomDialog)
	ON_NOTIFY(FLNM_SELCHANGED, IDC_TASKLIST, OnMainTreeListSel)
	ON_NOTIFY(FLNM_EXPAND, IDC_TASKLIST, OnMainTreeListComEx)
	ON_NOTIFY(FLNM_COLLAPSE, IDC_TASKLIST, OnMainTreeListComEx)
	
	ON_MESSAGE(NEEDORGREACTION, OnNeedOrganizatorReaction)
	ON_MESSAGE(FIREALERT, OnFireAlert)
	ON_MESSAGE(OPENPREFS, OnFireOptions)
	ON_COMMAND(ID_SYSTRAY_HELP, OnSystrayHelp)
	ON_COMMAND(ID_SYSTRAY_OPENWIREKEYS, OnSystrayOpenwirekeys)
	ON_COMMAND(ID_ITEM_SENDASMESSAGE, OnSendItemAsMessage)
	ON_COMMAND(ID_SYSTRAY_REMINDERSETTINGS, OnRemdsOptions)
	ON_NOTIFY(NM_RCLICK, IDC_RICHEDIT, OnRclickRichedit)
	ON_EN_CHANGE(IDC_RICHEDIT, OnChangeRichedit)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(theApp.iWM_THIS, ONWM_THIS)
	ON_REGISTERED_MESSAGE(theApp.iWM_WIRECHANGER, ONWM_WIRECHANGER)
	ON_REGISTERED_MESSAGE(theApp.iWM_WIREKEYS, ONWM_WIREKEYS)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppMainDlg message handlers
CDWordArray& MMenuIDLog()
{
	static CDWordArray aMMenuIDLog;
	return aMMenuIDLog;
}
CDWordArray& AMenuIDLog();

void AppendFeatsMenu(CMenu* m_pMenu,BOOL bExtended, BOOL bAdb)
{
	if(IsEmbedded()){
		return;
	}
	CMenu pAddFeatures;
	pAddFeatures.CreatePopupMenu();
	if(bAdb==0){
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_OPENWINDOW,_l("Open address book")+getActionHotKeyDsc(OPEN_ADDRBOOK),_bmp().Get(IDB_BM_ADRBOOK));
	}else{
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_OPEN,_l("Open organizer")+getActionHotKeyDsc(OPEN_TODOLIST),_bmp().Get(IDB_BM_ORGAICON));
	}
	if(bAdb==1 && (objSettings.sFBDir!="" || objSettings.bProtocolML_Active)){
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_FINDONLINEUSERS,_l("Find online users"),_bmp().Get(IDB_BM_FIND));
	}
	if(bAdb==0){
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,IDC_FIND_BT,_l("Find"),_bmp().Get(IDB_BM_FIND));
	}
	AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,IDC_SAVEALL_BT,_l("Save all"),_bmp().Get(IDB_BM_SAVE));
	if(bExtended!=2){
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_SHOWALLNOTES,_l("Show all notes")+getActionHotKeyDsc(SHOW_ALLNOTES),_bmp().Get(_IL(6)));
		pAddFeatures.AppendMenu(MF_SEPARATOR, 0, "");
	}
	{// Antispam
		int iActiveCount=0;
		for(int i=0;i<objSettings.lAntiSpamAccounts;i++){
			if(objSettings.antiSpamAccounts[i].sPOP3Server.GetLength()>0 && objSettings.antiSpamAccounts[i].sPOP3Server!=DEFPOP3SERVER){//objSettings.antiSpamAccounts[i].lActive
				AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,i+WM_USER+SHOW_ANTISPAM,objSettings.antiSpamAccounts[i].GetAccountName()+" - "+_l("Check")+"\t"+GetIHotkeyName(objSettings.antiSpamAccounts[i].dwHotkey),_bmp().Get(_IL(44)));
				iActiveCount++;
			}
		}
		if(iActiveCount>0){
			if(iActiveCount>1){
				AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,i+WM_USER+SHOW_ANTISPAM+1,_l("Check all accounts"),_bmp().Get(_IL(44)));
			}
		}
		if(objSettings.lAntiSpamAccountsFromNextStart>0){
			AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_ANTISPAMOPTIONS,_l("Anti-SPAM options"),_bmp().Get(IDB_BM_OPTIONS));
		}
		if(iActiveCount>0){
			pAddFeatures.AppendMenu(MF_SEPARATOR, 0, "");
		}
	}
	if(bExtended!=2){
		if(objSettings.bProtocolEM_Active){
			AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_CHECKINCOMINGEMAILS,_l("Check email messages"),_bmp().Get(_IL(44)),0,0,(bExtended!=2 && (!objSettings.objMessageWrapper || !objSettings.objMessageWrapper->pEMNet || objSettings.objMessageWrapper->pEMNet->lInCheck>0)));
		}
		if(objSettings.sFBDir!=""){
			AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_CBILLB,_l("Check billboard"),_bmp().Get(_IL(36)));
		}
	}
	if(objSettings.sFBDir!=""){
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_FZFOLDER,_l("Shared folder"),_bmp().Get(_IL(5)));
	}
	if(bExtended==2){
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_LINKCH,_l("Link checker"),_bmp().Get(_IL(22)));
	}
	/*
	if(IsOtherProgrammAlreadyRunning("WireChanger")){
	AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_WCHANGER,_l("WireChanger"),_bmp().Get("WireChanger"));
	}
	if(IsOtherProgrammAlreadyRunning("WireKeys")){
	AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_OPENWIREKEYS,_l("WireKeys"),_bmp().Get("WireKeys"));
	}
	*/
	pAddFeatures.AppendMenu(MF_SEPARATOR, 0, "");
	if(bAdb==0){
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_AUTOPOPUPORGANIZER,objSettings.APToDoList?_l("Disable Auto-popup"):_l("Enable Auto-popup"),_bmp().Get(_IL(102)));
	}else{
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_SYSTRAY_AUTOPOPUPAB,objSettings.bAutoPopupAdrBook?_l("Disable Auto-popup"):_l("Enable Auto-popup"),_bmp().Get(_IL(102)));
	}
	if(pAddFeatures.GetMenuItemCount()>0){
		if(bExtended!=2){
			AddMenuSubmenu(m_pMenu,&pAddFeatures,_l("Miscellaneous")+"\t...",_bmp().Get(_IL(54)));
		}else{
			AddMenuSubmenu(m_pMenu,&pAddFeatures,_l("Miscellaneous"),0);
		}
	}
}

void AppendHelpMenu(CMenu* m_pMenu,BOOL bExtended)
{
	CMenu pHelp;
	pHelp.CreatePopupMenu();
	AddMenuStringEx(&MMenuIDLog(),&pHelp,ID_SYSTRAY_HELP,_l("Contents"),_bmp().Get(IDB_BM_HELP));
	if(objSettings.iLikStatus<=1){
		AddMenuStringEx(&MMenuIDLog(),&pHelp,ID_SYSTRAY_REGISTER,_l("Enter key"),_bmp().Get(IDB_BM_LICKEY));
	}
	AddMenuStringEx(&MMenuIDLog(),&pHelp,ID_SYSTRAY_ABOUT,_l("About program"),_bmp().Get(IDB_BM_ABPROG));
	AddMenuStringEx(&MMenuIDLog(),&pHelp,ID_SYSTRAY_CHECKFORUPDATES,_l("Check for update"),_bmp().Get(_IL(22)),FALSE,NULL,IsUpdateInProgress());
	AddMenuStringEx(&MMenuIDLog(),&pHelp,ID_SYSTRAY_SENDASUGGESTION,_l("Support by Email"),_bmp().Get(_IL(44)));
	AddMenuStringEx(&MMenuIDLog(),&pHelp,ID_SYSTRAY_DONATE,_l("Donate")+"!",_bmp().Get(_IL(59)));
	if(bExtended!=2){
		AddMenuSubmenu(m_pMenu,&pHelp,_l("Help")+"\t...",_bmp().Get(IDB_BM_HELP));
	}else{
		AddMenuSubmenu(m_pMenu,&pHelp,_l("Help"),0);
	}
}

CMenu* _stdcall RefreshAdrBookMenuEx(void* pUser, BOOL bExtended);
CMenu* _stdcall RefreshMainMenuEx(void* pData, BOOL bExtended)
{
	if(bExtended){
		AMenuIDLog().RemoveAll();
		MMenuIDLog().RemoveAll();
	}
	CMenu* m_pMenu=new CMenu();
	if(bExtended==2){
		m_pMenu->CreateMenu();
		CMenu pAddFeatures;
		pAddFeatures.CreatePopupMenu();
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_REPORTALLITEMS,_l("Generate report"),_bmp().Get(IDB_BM_TODOINFO));
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_SHOWALLNOTES,_l("Show all notes")+getActionHotKeyDsc(SHOW_ALLNOTES),_bmp().Get(_IL(6)));
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_HIDEALLNOTES,_l("Hide all notes")+getActionHotKeyDsc(HIDE_ALLNOTES),_bmp().Get(_IL(6)));
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,IDC_SAVEALL_BT,_l("Save all"),_bmp().Get(IDB_BM_SAVE));
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,IDC_FIND_BT,_l("Find"),_bmp().Get(IDB_BM_FIND));
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_OPTIONS,_l("Preferences"),_bmp().Get(IDB_BM_OPTIONS));
		pAddFeatures.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_EXIT,_l("Exit"),_bmp().Get(_IL(40)),FALSE,NULL,0);
		AddMenuSubmenu(m_pMenu,&pAddFeatures,_l("File"));
	}else{
		m_pMenu->CreatePopupMenu();
	}
	if(bExtended!=2){
		if(pMainDialogWindow && !pMainDialogWindow->IsWindowVisible()){
			AddMenuStringEx(&MMenuIDLog(),m_pMenu,ID_SYSTRAY_OPEN,_l("Open organizer")+getActionHotKeyDsc(OPEN_TODOLIST),_bmp().Get(IDB_BM_ORGAICON));
		}
	}
	CMenu pNewMenu;
	pNewMenu.CreatePopupMenu();
	AddMenuStringEx(&MMenuIDLog(),&pNewMenu,ID_SYSTRAY_NEWTASK,_l("ToDo")+getActionHotKeyDsc(NEW_TODO),_bmp().Get(IDB_BM_NEWTODO));
	AddMenuStringEx(&MMenuIDLog(),&pNewMenu,ID_SYSTRAY_NEWNOTE,_l("Note")+getActionHotKeyDsc(NEW_NOTE),_bmp().Get(IDB_BM_NEWNOTE));
	AddMenuStringEx(&MMenuIDLog(),&pNewMenu,ID_SYSTRAY_NEWREMINDER,_l("Reminder")+getActionHotKeyDsc(NEW_REMD),_bmp().Get(IDB_BM_NEWREMAINDER));
	if(!objSettings.bNoSend){
		AddMenuStringEx(&MMenuIDLog(),&pNewMenu,ID_SYSTRAY_NEWMESSAGE,_l("Message")+getActionHotKeyDsc(NEW_MSG),_bmp().Get(IDB_BM_NEWMESSAGESM));
	}
	AddMenuString(&pNewMenu,ID_SYSTRAY_NEWLINK,_l("File/folder link"),_bmp().Get(IDB_BM_NLINK));
	if(bExtended!=2){
		AddMenuSubmenu(m_pMenu,&pNewMenu,_l("Create new")+"\t...",_bmp().Get(IDB_BM_NEW));
	}else{
		AddMenuStringEx(&MMenuIDLog(),&pNewMenu,ID_SYSTRAY_LINKFROMCLIPBOARD,_l("Link from clipboard"),_bmp().Get(_IL(22)));
		AddMenuSubmenu(m_pMenu,&pNewMenu,_l("New"),0);
	}
	if(bExtended!=2){
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		if(pMainDialogWindow){
			if(pMainDialogWindow->CreateMenuTree(*m_pMenu,objSettings.sRootMenuId, 0)==0){
				AddMenuStringEx(&MMenuIDLog(),m_pMenu,0,_l("None"),NULL,FALSE,NULL,TRUE,TRUE);
			}
			if(pMainDialogWindow){
				CMenu pSwitchMenu;
				pSwitchMenu.CreatePopupMenu();
				AddMenuStringEx(&MMenuIDLog(),&pSwitchMenu,ID_SYSTRAY_SWITCHMENU,_l("All folders"));
				pMainDialogWindow->CreateMenuTree(pSwitchMenu, "", 2);
				AddMenuSubmenu(m_pMenu,&pSwitchMenu,_l("Switch to")+"\t...",_bmp().Get(_IL(54)));
			}
			m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
		}
		/*if(pMainDialogWindow && pMainDialogWindow->dlgAddBook && objSettings.lTrayIconsMaskO){
			CMenu* pAddrBookMenu=RefreshAdrBookMenuEx(NULL,0);
			if(pAddrBookMenu){
				AddMenuSubmenu(m_pMenu,pAddrBookMenu,_l("Message center")+"\t...",_bmp().Get(IDB_BM_MESCNTR));
				delete pAddrBookMenu;
			}
		}*/
	}
	//
	AppendHelpMenu(m_pMenu,bExtended);
	AppendFeatsMenu(m_pMenu,bExtended,0);
	if(bExtended!=2){
		AddMenuStringEx(&MMenuIDLog(),m_pMenu,ID_SYSTRAY_OPTIONS,_l("Preferences"),_bmp().Get(IDB_BM_OPTIONS));
		AppendAdverts(m_pMenu);
		AddMenuStringEx(&MMenuIDLog(),m_pMenu,ID_SYSTRAY_EXIT,_l("Exit"),_bmp().Get(_IL(40)),FALSE,NULL,objSettings.bDoModalInProcess!=0);
	}
	return m_pMenu;
}

CMenu* _stdcall RefreshMenu(void* pData)
{
	return RefreshMainMenuEx(pData,1);
}

void FAR WINAPI OnTaskWndLeave(MouseTrackerRuler* pRuler)
{
	if(!pRuler->bStopWatching && pMainDialogWindow){
		pMainDialogWindow->OnMinimaze();
	}else{
		pRuler->bStopWatching=FALSE;
	}
}

void _stdcall OnTrayMove()
{
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus!=1){
		return;
	}
	if(objSettings.APToDoList){
		/*// Ждем пока положение не перестанет меняться!
		CRect rtStart(pMainDialogWindow->Sizer.m_rMain);
		pMainDialogWindow->Sizer.GetScreenRect(0,0)=CRect(0,0,0,0);
		pMainDialogWindow->Sizer.TestStickness(pMainDialogWindow->Sizer.m_rMain,FALSE);
		while(!rtStart.EqualRect(pMainDialogWindow->Sizer.m_rMain)){
			rtStart=pMainDialogWindow->Sizer.m_rMain;
			Sleep(200);
			pMainDialogWindow->Sizer.GetScreenRect(0,0)=CRect(0,0,0,0);
			pMainDialogWindow->Sizer.TestStickness(pMainDialogWindow->Sizer.m_rMain,FALSE);
		}*/
		pMainDialogWindow->OnHotmenuPopupmainwindow();
	}
}

AppMainDlg::AppMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(/*AppMainDlg::IDD, pParent*/)
{
	//{{AFX_DATA_INIT(AppMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	sLastFEditID="";
	hLastActiveControl=0;
	m_hIcon = ::AfxGetApp()->LoadIcon(theApp.aAppIcons[objSettings.iTrayIcon]);
	iRunStatus=-1;
	iIgnoreChangerFEdit=0;
	dwLastEditedLastType=DWORD(-1);
	MainTracker.pWnd=this;
	MainTracker.MouseOutProc=OnTaskWndLeave;
	MainTracker.dwTaskbarFriendly=1;
	MainTracker.bStopWatching=FALSE;
	MainTracker2.pWnd=this;
	MainTracker2.pSlider=&m_Slider;
	MainTracker2.MouseOutProc=OnNoteLeave2;
	MainTracker2.dwTaskbarFriendly=0;
	MainTracker2.bStopWatching=FALSE;
	MouseTracker=NULL;
	MouseTracker2=NULL;
	iCurrentItem=-1;
	iCurrentItemLast=-2;
	bAnyPopupIsActive=FALSE;
	iTimerCheckCounter=0;
	dwAutoSaveCounter=GetTickCount();
	iUpdateTreeCounter=0;
	iScrSaverCheckCounter=0;
	iCurrentOnLineMode=-1;
	sActiveAppTitle="";
	iLastBoldItem=-1;
	dwItemOnStopDragLast=dwItemOnStartDrag=dwItemOnStopDrag=-1;
	dwDragLock=0;
	hRowForDrag=NULL;
	nRowForDragDir=0;
	dlgAddBook=NULL;
	bPinnedUp=FALSE;
	bAllLoaded=FALSE;
	bMainWindowInitialized=FALSE;
	bLastHalfer=FALSE;
	MainRulePos=0;
	font=NULL;
}

AppMainDlg::~AppMainDlg()
{
	if(MouseTracker!=NULL){
		TerminateThread(MouseTracker,0);
		CloseHandle(MouseTracker);
	}
	if(MouseTracker2!=NULL){
		TerminateThread(MouseTracker2,0);
		CloseHandle(MouseTracker2);
	}
	if(font){
		deleteFont(font);
	}
}

BOOL AppMainDlg::LoadAll()
{
	objSettings.AddrBook.LoadFromFile();
	objSettings.m_Notes.LoadNotes();
	objSettings.m_Reminders.LoadReminders();
	objSettings.m_Tasks.LoadTasks();
	LoadLinks();
	LoadFolders();
	bAllLoaded=TRUE;
	return TRUE;
}

BOOL AppMainDlg::OnInitDialog()
{
	if(objSettings.bShadows&& !IsVista()){
		SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW//ModifyStyleEx(0,WS_EX_COMPOSITED,SWP_FRAMECHANGED);
	}
	CDialog::OnInitDialog();
	SetWindowText(IsEmbedded()?"WK_NOTES":objSettings.sMainWindowTitle);
	COleDateTime dt=COleDateTime::GetCurrentTime();
	GetDlgItem(IDC_STATUS)->SetWindowText(dt.Format("%x")+". "+_l("Welcome to WireNote","Welcome to WireNote!"));
	{
		Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_MainTasks");
		CRect minRect(0,0,200,DIALOG_MINYSIZE);
		SetButtonSize(this,IDC_NEW_TODO,CSize(24,24));
		Sizer.SetMinRect(minRect);
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
		Sizer.AddDialogElement(IDC_TASKLIST,bottomResize);
		Sizer.AddDialogElement(IDC_RICHEDIT);
		Sizer.AddDialogElement(IDC_NEW_TODO,widthSize|heightSize|fixedDimensions,objSettings.dwButtonSize);
		Sizer.AddDialogElement(IDC_NEW_NOTE,widthSize|heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_NEW_REMINDER,widthSize|heightSize,IDC_NEW_TODO);
        Sizer.AddDialogElement(IDC_ADDRBOOK,widthSize|heightSize|hidable,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_OPTIONS_BT,widthSize|heightSize|hidable,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_EDITFORM,widthSize|heightSize|hidable,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_SAVEALL_BT,widthSize|heightSize|hidable,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_FIND_BT,widthSize|heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_WNDONTOP,widthSize|heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_NEW_LINK,widthSize|heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_NEW_FOLDER2,widthSize|heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_EDIT_ITEM,widthSize|heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_DELETE_ITEM,widthSize|heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_STATUS,heightSize,IDC_NEW_TODO);
		Sizer.AddDialogElement(IDC_SLIDER_TRANS,widthSize|hidable,IDC_WNDONTOP);
		//
		Sizer.AddLeftTopAlign(IDC_NEW_NOTE,0,2);
		Sizer.AddTopAlign(IDC_NEW_TODO,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_NEW_REMINDER,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_FIND_BT,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_NEW_LINK,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_NEW_FOLDER2,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_ADDRBOOK,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_OPTIONS_BT,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_EDIT_ITEM,IDC_NEW_NOTE,0);
		Sizer.AddTopAlign(IDC_DELETE_ITEM,IDC_NEW_NOTE,0);
		Sizer.AddLeftAlign(IDC_NEW_TODO,IDC_NEW_NOTE,rightPos,2);
		Sizer.AddLeftAlign(IDC_NEW_REMINDER,IDC_NEW_TODO,rightPos,2);
		Sizer.AddLeftAlign(IDC_NEW_LINK,IDC_NEW_REMINDER,rightPos,2);
		Sizer.AddLeftAlign(IDC_NEW_FOLDER2,IDC_NEW_LINK,rightPos,2);
		Sizer.AddLeftAlign(IDC_FIND_BT,IDC_NEW_FOLDER2,rightPos,2);

		Sizer.AddRightTopAlign(IDC_WNDONTOP,0,2);
		Sizer.AddTopAlign(IDC_SLIDER_TRANS,IDC_WNDONTOP,bottomPos,2);
		Sizer.AddRightAlign(IDC_OPTIONS_BT,IDC_WNDONTOP,leftPos,-2);
		Sizer.AddRightAlign(IDC_DELETE_ITEM,IDC_OPTIONS_BT,leftPos,-2);
		Sizer.AddRightAlign(IDC_EDIT_ITEM,IDC_DELETE_ITEM,leftPos,-2);
		Sizer.AddRightAlign(IDC_ADDRBOOK,IDC_EDIT_ITEM,leftPos,-2);

		Sizer.AddRightAlign(IDC_SAVEALL_BT,0,2);
		Sizer.AddRightAlign(IDC_EDITFORM,0,2);
		Sizer.AddRightAlign(IDC_SLIDER_TRANS,IDC_WNDONTOP,rightPos,10);
		Sizer.AddHideRule(IDC_SLIDER_TRANS);

		Sizer.AddLeftAlign(IDC_TASKLIST,0,2);
		Sizer.AddLeftAlign(IDC_RICHEDIT,0,2);
		Sizer.AddLeftAlign(IDC_STATUS,0,2);
		Sizer.AddTopAlign(IDC_TASKLIST,IDC_NEW_TODO,bottomPos,2);
		Sizer.AddTopAlign(IDC_RICHEDIT,IDC_TASKLIST,bottomPos,2);
		Sizer.AddBottomAlign(IDC_STATUS,0,-2);
		Sizer.AddBottomAlign(IDC_RICHEDIT,IDC_STATUS,topPos,-2);
		Sizer.AddBottomAlign(IDC_TASKLIST,IDC_RICHEDIT,topPos,-2);
		Sizer.AddBottomAlign(IDC_EDITFORM,0,-2);
		Sizer.AddBottomAlign(IDC_SAVEALL_BT,0,-2);
		Sizer.AddRightAlign(IDC_EDITFORM,0,-2);
		Sizer.AddRightAlign(IDC_SAVEALL_BT,IDC_EDITFORM,leftPos,-2);
		Sizer.AddRightAlign(IDC_STATUS,IDC_SAVEALL_BT,leftPos,-2);
		
		Sizer.AddRightAlign(IDC_TASKLIST,0,-2);
		Sizer.AddRightAlign(IDC_RICHEDIT,0,-2);

		Sizer.AddBottomAlign(IDC_SLIDER_TRANS,IDC_SAVEALL_BT,topPos,-2);
		Sizer.AddBottomAlign(IDC_TASKLIST,IDC_STATUS, topPos|ifBigger,-2);
		Sizer.AddBottomAlign(IDC_TASKLIST,IDC_NEW_TODO, bottomPos|ifLess,65);
	}
	{
		m_BtOnTop.SetBitmap(bPinnedUp?IDB_PINNED_UP:IDB_PINNED_DOWN);
		m_BtSaveAll.SetBitmap(IDB_BM_SAVE);
		m_BtSaveAll.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtOpt.SetBitmap(IDB_BM_OPTIONS);
		m_BtOpt.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtEform.SetBitmap(IDB_BM_EFORM);
		m_BtEform.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtNewTd.SetBitmap(IDB_BM_NEWTODO);
		m_BtNewTd.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtNewRem.SetBitmap(IDB_BM_NEWREMAINDER);
		m_BtNewRem.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtNewNote.SetBitmap(IDB_BM_NEWNOTE);
		m_BtNewNote.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtAdrBook.SetBitmap(IDB_BM_MESCNTR);
		m_BtAdrBook.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtFind.SetBitmap(IDB_BM_FIND);
		m_BtFind.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtNewLink.SetBitmap(IDB_BM_NLINK);
		m_BtNewLink.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtNewFolder.SetBitmap(IDB_BM_NFOLDER);
		m_BtNewFolder.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtEditItem.SetBitmap(IDB_BM_EDIT);
		m_BtEditItem.SetStyles(CBS_NORMAL|CBS_FLAT);
		m_BtDelItem.SetBitmap(IDB_BM_DEL);
		m_BtDelItem.SetStyles(CBS_NORMAL|CBS_FLAT);
	}
	{
		m_tooltip.Create(this);
		m_tooltip.SetMaxTipWidth(objSettings.rDesktopRectangle.Width()/2);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDC_NEW_TODO), _l("New ToDo")+getActionHotKeyDsc(NEW_TODO," (",")"));
		m_tooltip.AddTool(GetDlgItem(IDC_NEW_NOTE), _l("New Note")+getActionHotKeyDsc(NEW_NOTE," (",")"));
		m_tooltip.AddTool(GetDlgItem(IDC_NEW_REMINDER), _l("New Reminder")+getActionHotKeyDsc(NEW_REMD," (",")"));
		m_tooltip.AddTool(GetDlgItem(IDC_ADDRBOOK), _l("Message center")+getActionHotKeyDsc(OPEN_ADDRBOOK," (",")"));
		m_tooltip.AddTool(GetDlgItem(IDC_OPTIONS_BT), _l(OPTIONS_TPM)+" (Ctrl-O)");
		m_tooltip.AddTool(GetDlgItem(IDC_SAVEALL_BT), _l("Save All")+" (Ctrl-S)");
		m_tooltip.AddTool(GetDlgItem(IDC_FIND_BT), _l("Find")+" (Ctrl-F)");
		m_tooltip.AddTool(GetDlgItem(IDC_WNDONTOP), _l("Change on-top state")+" (Ctrl-T)");
		m_tooltip.AddTool(GetDlgItem(IDC_TASKLIST), _l("Main view"));
		m_tooltip.AddTool(GetDlgItem(IDC_NEW_LINK), _l("New link"));
		m_tooltip.AddTool(GetDlgItem(IDC_NEW_FOLDER2), _l("New folder"));
		m_tooltip.AddTool(GetDlgItem(IDC_EDIT_ITEM), _l("Edit selected"));
		m_tooltip.AddTool(GetDlgItem(IDC_DELETE_ITEM), _l("Delete selected"));
	}
	m_Slider.SetRange(0,100,TRUE);
	m_Slider.SetTicFreq(30);
	m_STray.Create(NULL, WM_USER, objSettings.sTrayTooltip, ::AfxGetApp()->LoadIcon(theApp.aAppIcons[objSettings.iTrayIcon]),IDR_SYSTRAY,this,(!objSettings.lTrayIconsMaskO)?TRUE:FALSE);
	HICON aIList[2]={0, _bmp().CloneAsIcon(_bmp().GetResName(IDB_BM_INMESSAGE,BIGLIST_ID))};
	m_STray.SetIconList(aIList,sizeof(aIList)/sizeof(HICON));
	m_STray.SetAnimationLimits(0,1);
	//m_STray.SetMenuFlags(0x0001L);//TPM_RECURSE(0x0001L)
	SetWindowIcon();
	m_STray.SetMenuDefaultItem(ID_SYSTRAY_DEFACTION, FALSE);
	m_STray.pdwDefTICONAction=&objSettings.UseBblClick;
	m_STray.SetUserMenu(&RefreshMenu,&OnTrayMove);
	SetTaskbarButton(this->GetSafeHwnd());
	InitTaskList();
	{//Главное меню
		SetMenu(RefreshMainMenuEx(0,2));
	}
	//
	LoadAll();
	objSettings.objMessageWrapper->ConnectAll();
	OnSystrayAddressbook();

	SetTrayTooltip();
	SetWindowPinState(FALSE);
	RegisterMyHotKeys();
	iRunStatus=1;
	// Говорим всем что прога запустилась
	DWORD dwTarget=BSM_APPLICATIONS;
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(0), LPARAM(0));
	if(objSettings.lCheckForUpdate && !IsEmbedded()){
		GetVersionChecker().SetTraits();
		StartCheckNewVersionX(objSettings.lDaysBetweenChecks);
	}
	objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("WireNote started")+"...");
	return FALSE;
}

void AppMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID==SC_CLOSE){
		DWORD dwReply=AskYesNo(_l("Do you want to quit WireNote","Do you want to quit WireNote?\r\nPress 'NO' to minimize window"),&objSettings.lOnExitFromMainWnd,this);
		if(pMainDialogWindow && dwReply==IDYES){
			PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
		}else{
			OnMinimaze();
		}
		return;
	}
	if(nID==SC_MINIMIZE){
		OnMinimaze();
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void AppMainDlg::SetWindowPinState(BOOL bShow)
{
	if(bShow!=-1){
		SetWindowPos(bPinnedUp?&wndTopMost:&wndNoTopMost,0,0,0,0,(bShow?SWP_SHOWWINDOW:SWP_HIDEWINDOW)|SWP_NOSIZE|SWP_NOMOVE);
	}
	m_BtOnTop.SetBitmap(bPinnedUp?IDB_PINNED_UP:IDB_PINNED_DOWN);
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR AppMainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void AppMainDlg::OnMinimaze() 
{
	if(::GetFocus() && ::GetFocus()!=GetSafeHwnd()){
		hLastActiveControl=::GetFocus();
		char s[100]={0};
		::GetWindowText(hLastActiveControl,s,99);
		TRACE("<<<\n");
		TRACE(s);
		TRACE("\n>>>\n");
	}
	ShowWindowAsync(GetSafeHwnd(),SW_HIDE);
}

void AppMainDlg::OnSystrayExit() 
{
	Finalize();
}

void AppMainDlg::OnSystrayAntiSpamOptions()
{
	objSettings.OpenOptionsDialog(_l(DEF_ANTISPAMTOPIC),this);
}

void AppMainDlg::OnSystrayOpen() 
{
	if(IsEmbedded()){
		return;
	}
	SetWindowIcon();// Показать если надо, не показывать иначе
	OnHotmenuPopupmainwindow();
	SwitchToWindow(this->GetSafeHwnd());
}

void AppMainDlg::OnDestroy()
{
	// Говорим всем что прога останавливается
	DWORD dwTarget=BSM_APPLICATIONS;
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(1), LPARAM(0));
	//
	objSettings.Finalize();
	CDialog::OnDestroy();
}

long lWaitableWait=0;
DWORD WINAPI WaitForWaitableTimer(LPVOID pData)
{
	if(lWaitableWait){
		return 0;
	}
	lWaitableWait=1;
	while(1){
		if(pData==0){
			lWaitableWait=0;
			return 1;
		}
		DWORD dwRes=WaitForSingleObject((HANDLE) pData, INFINITE);
		if(!pMainDialogWindow || pMainDialogWindow->iRunStatus!=1){
			break;
		}
		if(dwRes==WAIT_OBJECT_0){
			SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
		}
		Sleep(5000);
	}
	return 0;
}

#include "..\SmartWires\Atl\filemap.h"
BOOL AppMainDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(lParam==0 && wParam==2){
		OnMinimaze();
		return TRUE;
	}
	if(wParam==WM_USER+EXTERNS_BASE+0){
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
		return TRUE;
	}
	if(wParam==WM_USER+EXTERNS_BASE+1){
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_NEWNOTE,0);
		return TRUE;
	}
	if(wParam==WM_USER+EXTERNS_BASE+2){
		//Список заметок!
		CString sListing;
		int iID=0;
		for(int i=0;i<objSettings.Items.GetSize();i++){
			if(objSettings.Items[i] && objSettings.Items[i]->GetType()==2){
				sListing+=Format("{Line(%i)_#$?...%s=*88#22#$}",iID++,TrimMessage(objSettings.Items[i]->GetTitle(),20));
			}
		}
		sListing=Format("{ALL_#$?...%i=*88#22#$ALL}",iID)+sListing;
		{
			CFileMap fMap;
			DWORD dwLen=1024;//sListing.GetLength()+1;
			if(int(sListing.GetLength()) > int(dwLen-1)){
				sListing=sListing.Left(dwLen-1);
			}
			fMap.Create("WK_NOST",dwLen);
			char* sz=(char*)fMap.Base();
			memset(sz,0,dwLen);
			strcpy(sz,sListing);
			fMap.Close();
		}
		//AfxMessageBox(sListing);
		return TRUE;
	}
	if(wParam>=WM_USER+EXTERNS_BASE+10 && wParam<=WM_USER+EXTERNS_BASE+10+USERMSG_STEP){
		int iId=wParam-(WM_USER+EXTERNS_BASE+10);
		int iCnt=0;
		for(int i=0;i<objSettings.Items.GetSize();i++){
			if(objSettings.Items[i] && objSettings.Items[i]->GetType()==2){
				if(iCnt==iId){
					CItem* pItem=CItem::GetItemByIndex(i);
					if(pItem){
						if(lParam==0){
							OperationUnderCursor(5, pItem);
						}else{
							CDLG_Note* note=objSettings.m_Notes.FindNoteByID(pItem->GetID());
							if(note){
								//note->OnNotemenuHidenote();
								note->PostMessage(WM_COMMAND,ID_NOTEMENU_HIDENOTE,0);
							}
						}
					}
					return TRUE;
				}
				iCnt++;
			}
		}
		return TRUE;
	}
	if(wParam==ID_SYSTRAY_CHECKFORUPDATES){
		GetVersionChecker().SetTraits("(Manual)", IDI_UPNOTIFY);
		StartCheckNewVersionX(0);
		return TRUE;
	}
	if(ParseAdvert(wParam)){
		return TRUE;
	}
	if(wParam>=WM_USER+SHOW_ANTISPAM && wParam<(DWORD)(WM_USER+SHOW_ANTISPAM+USERMSG_STEP)){
		int iIndex=wParam-WM_USER-SHOW_ANTISPAM;
		if(iIndex>=0 && iIndex<objSettings.lAntiSpamAccounts){
			objSettings.antiSpamAccounts[iIndex].ShowAntiSpamWindow();
			return TRUE;
		}else{
			OnSystrayCheckAllAcounts();
		}
	}
	if(wParam>=WM_USER+SWITCH_TO && wParam<(DWORD)(WM_USER+SWITCH_TO+USERMSG_STEP)){
		int iIndex=wParam-WM_USER-SWITCH_TO;
		CItem* pItem=CItem::GetItemByIndex(iIndex);
		if(pItem){
			if(pItem->GetType()==0){
				objSettings.sRootMenuId=pItem->GetID();
			}else{
				OperationUnderCursor(1, pItem);
			}
			return TRUE;
		}
	}
	if(wParam>=WM_USER+BB_SECTION && wParam<WM_USER+BB_SECTION+USERMSG_STEP){
		int iIndex=wParam-WM_USER-BB_SECTION;
		OnShareItem(iIndex);
		return TRUE;
	}
	if(wParam>=WM_USER+CHANGE_NOTE_STYLE && wParam<(DWORD)(WM_USER+CHANGE_NOTE_STYLE+USERMSG_STEP)){
		int iIndex=wParam-WM_USER-CHANGE_NOTE_STYLE;
		CItem* item=CItem::GetItemByIndex(iCurrentItem);
		if(item){
			item->SetColorCode(iIndex);
		}
		return TRUE;
	}
	if(wParam>=WM_USER+CHANGE_NOTE_STYLE && wParam<(DWORD)(WM_USER+CHANGE_NOTE_STYLE+USERMSG_STEP)){
		int iIndex=wParam-WM_USER-CHANGE_NOTE_STYLE;
		CItem* item=CItem::GetItemByIndex(iCurrentItem);
		if(item){
			item->SetColorCode(iIndex);
		}
		return TRUE;
	}
	if(wParam==REMOVE_REMD){
		CString sID=(const char*)lParam;
		objSettings.m_Reminders.Delete(sID);
		return TRUE;
	}
	if(wParam==SETWAITT){
		HANDLE hASleepTimer=OpenWaitableTimer(TIMER_ALL_ACCESS|TIMER_MODIFY_STATE,FALSE,"WN_ASleepTimer");
		if(!hASleepTimer){
			hASleepTimer=CreateWaitableTimer(0,TRUE,"WN_ASleepTimer");
		}
		if(hASleepTimer){
			if(SetWaitableTimer(hASleepTimer, (LARGE_INTEGER *)lParam, 0, 0, 0, TRUE)){
				FORK(WaitForWaitableTimer,hASleepTimer);
			}else{
				//???
				objSettings.lRemindFromSleep=0;
			}
			delete (LARGE_INTEGER *)lParam;
		}
		return TRUE;
	}
	if(dlgAddBook && dlgAddBook->IsAddrBookMessage(wParam)){
		dlgAddBook->PostMessage(WM_COMMAND,wParam,lParam);
	}
	return CDialog::OnCommand(wParam, lParam);
}

void AppMainDlg::OnDeleteTask()
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		objSettings.m_Tasks.DeleteTask(item->GetID());
	}
}

void AppMainDlg::RedrawItem(CItem* pItem)
{
	if(pItem==NULL){
		return;
	}
	HIROW hRow=pItem->hIRow;
	if(hRow==NULL){
		return;
	}
	if(pItem->iListIndex==-1){// Чисто для поддержки целостности
		pItem->iListIndex=m_TaskList.GetListIndex(hRow);
	}
	//
	m_TaskList.SetIRowData(hRow,pItem->index);
	DWORD dwFgColor=-1,dwBgColor=-1;
	if(pItem->GetColorCode()>=0){
		dwFgColor=objSettings.objSchemes.getTxtColor(pItem->GetColorCode());
		dwBgColor=objSettings.objSchemes.getBgColor(pItem->GetColorCode());
	}
	m_TaskList.SetIItemColors(hRow,0,dwFgColor,dwBgColor);
	if(pItem->GetType()==0){
		m_TaskList.SetIItemControlType(hRow,1,FLC_READONLY);
	}else if(pItem->GetType()<=4){
		DWORD dwCType=0;
		if(objSettings.bUseDblClickToStart){
			dwCType=FLC_READONLY;
		}else{
			dwCType=FLC_EDITWITHCUSTOM;
		}
		if(pItem->GetState()!=-1){
			if(pItem->GetType()==1){
				m_TaskList.SetIItemCheckboxImages(hRow, 1, 14, 15);
			}else{
				m_TaskList.SetIItemCheckboxImages(hRow, 1, 16, 17);
			}
			dwCType=dwCType|FLC_CHECKBOX;
		}
		m_TaskList.SetIItemControlType(hRow,1,dwCType);
	}
	//
	BOOL bBoldSettled=FALSE;
	BOOL bHIROWBold=m_TaskList.GetIRowTextStyle(hRow,FALSE);
	m_TaskList.SetIItemText(hRow, 1, pItem->GetTitle());
	m_TaskList.SetIItemImage(hRow, 0, pItem->GetPriority()+PRIOR_ICONS_OFFSET);
	if(pItem->GetType()>=1 && pItem->GetType()<=4){
		m_TaskList.SetIItemImage(hRow, 1, pItem->GetIconNum());
		if(!objSettings.bHideDateFld){
			m_TaskList.SetIItemText(hRow, 2, GetDateInCurFormat(pItem->GetCrTime()));
		}
		if(pItem->GetState()!=-1){
			if(m_TaskList.IsIItemChecked(hRow,1)!=pItem->GetState()){
				m_TaskList.SetIItemCheck(hRow, 1, pItem->GetState(), TRUE,FALSE);
			}
		}
		if(pItem->GetType()==1){// Задачи
			Task* pTask=objSettings.m_Tasks.GetTask(pItem->GetID());
			if(pTask){
				BOOL bState=pItem->GetState();
				bHIROWBold=bState?(FL_PALE | (objSettings.bShowDone==1?FL_STRIKE:0)):0;
				if(pTask->IsActive()){
					bHIROWBold|=FL_BOLD;
				}
				m_TaskList.SetIRowTextStyle(hRow, bHIROWBold|FL_SELBORD);
				bBoldSettled=TRUE;
			}
		}
		if(pItem->GetType()==3){// Напоминания
			BOOL bState=!pItem->GetState();
			bHIROWBold=bState?(FL_PALE | (objSettings.bShowNonActiveReminders==1?FL_STRIKE:0)):0;
			m_TaskList.SetIRowTextStyle(hRow, bHIROWBold|FL_SELBORD);
			bBoldSettled=TRUE;
		}
	}
	if(!bBoldSettled){
		m_TaskList.SetIRowTextStyle(hRow, FL_SELBORD);
	}
	pItem->ClearChanged();
}

BOOL AppMainDlg::InsertOrRefreshItem(CItem* pItem, BOOL bStopOnNoParent)
{
	if(pItem==NULL){
		return FALSE;
	}
	if(CheckItemInFilters(pItem)==FALSE){
		pItem->DeleteFromList();
		pItem->ClearChanged();
		return TRUE;
	}
	if(pItem->hIRow!=NULL){
		RedrawItem(pItem);
		return TRUE;
	}
	CString sParentID,sParentPath;
	CItem* pRoot=pItem->GetItemParent(&sParentID,&sParentPath);
	if(sParentID.GetLength()>0 && !pRoot){
		if(bStopOnNoParent){
			return FALSE;
		}
		// Несуществующая папка!
		// Надо создать
		pRoot=new CItemFolder(sParentID);
		pRoot->SetID(sParentID);
		pRoot->SetPath(sParentPath);
		if(sParentID==BB_PATH){
			pRoot->SetTitle(_l("Billboard"));
		}
		pRoot->Attach();
	}
	if(pRoot && pRoot->hIRow==NULL){
		InsertOrRefreshItem(pRoot);
	}
	if(pRoot){
		BOOL bInsertAfter=(pItem->GetType()==0);
		HIROW hInsertIn=bInsertAfter?FL_LAST:FL_FIRST;// Правда так новые элементы будут вперед папок, а папки в конце
		if(pItem->IsChanged()==2 && hRowForDrag!=NULL){
			if(nRowForDragDir<0){
				hInsertIn=m_TaskList.GetPrevIRow(hRowForDrag);
				if(hInsertIn==NULL){
					hInsertIn=FL_FIRST;
				}
			}else{
				hInsertIn=hRowForDrag;
			}
		}
		pItem->hIRow=m_TaskList.InsertIRow(pRoot->hIRow, hInsertIn, pItem->GetTitle(), pItem->GetIconNum());
		if(pItem->GetType()==0){
			m_TaskList.SetIItemColSpan(pItem->hIRow,1,1);
		}
		//
		if(pRoot->GetState()==0){
			m_TaskList.Expand(pRoot->hIRow);
			pRoot->SetState(-1);
		}
		RedrawItem(pRoot);// Т.к. содержимое изменилось, надо обновить
		if(bInsertAfter){// Надо перерисовать предыдущий элемент - а то подвод останется висеть в воздухе
			int iIndex=m_TaskList.GetIRowIndex(pItem->hIRow)-1;
			if(iIndex>=0){
				HIROW hPrevItem=m_TaskList.GetIRowFromIndex(iIndex);
				if(hPrevItem){
					m_TaskList.UpdateIRow(hPrevItem);
				}
			}
		}
	}else{
		pItem->hIRow=m_TaskList.InsertIRow(FL_ROOT, FL_LAST, pItem->GetTitle(), pItem->GetIconNum());
		if(pItem->GetType()==0){
			m_TaskList.SetIItemColSpan(pItem->hIRow,1,1);
			DWORD crRealBgColor=0,crTxColor=0,crBackground=0;
			m_TaskList.GetIRowColors(crTxColor,crBackground);
			crRealBgColor=RGB(GetRValue(crBackground)*0.95,GetGValue(crBackground)*0.95,GetBValue(crBackground)*0.95);
			m_TaskList.SetIItemColors(pItem->hIRow,1,-1,crRealBgColor);
		}
	}
	RedrawItem(pItem);
	return TRUE;
}

void AppMainDlg::RefreshTaskList()
{
	BOOL bForcedly=FALSE;
	if(objSettings.bTreeNeedUpdate==2){
		bForcedly=TRUE;
	}
	objSettings.bTreeNeedUpdate=FALSE;
	EnterCriticalSection(&objSettings.bTreeNeedUpdateLock);
	int size=objSettings.Items.GetSize();
	for(int i=0;i<size;i++){
		CItem* pItem=objSettings.Items.GetAt(i);
		if(pItem){
			pItem->index=i;
			if(pItem->hIRow!=NULL){
				m_TaskList.SetIRowData(pItem->hIRow,pItem->index);
			}
			if(pItem->IsChanged() || bForcedly){
				InsertOrRefreshItem(pItem);
			}
		}
	}
	LeaveCriticalSection(&objSettings.bTreeNeedUpdateLock);
	if(!bMainWindowInitialized && bAllLoaded){
		bMainWindowInitialized=TRUE;
		SortList(0);
	}
	if(objSettings.sTreeNeedMoveFocus!=""){
		CItem* item=CItem::GetItemByID(objSettings.sTreeNeedMoveFocus);
		if(item && item->hIRow){
			Task* pTask=objSettings.m_Tasks.GetTask(item->GetID());
			if(pTask){
				m_FEdit.GetWindowText(pTask->sDsc);
			}
			m_TaskList.SelectIItem(item->hIRow,1);
			m_TaskList.EnsureVisible(item->hIRow,1,1);
			objSettings.sTreeNeedMoveFocus="";
			SwitchToFEdit();
		}
	}
}

void AppMainDlg::InitTaskList()
{
	/*CRect rt;
	m_TaskList.GetClientRect(&rt);
	objSettings.iCol1Width=rt.Width()-objSettings.iCol0Width-objSettings.iCol2Width;*/
	m_TaskList.SetReadOnly(FALSE);
	m_TaskList.SetImageList(&theApp.MainImageList, LVSIL_SMALL);
	if(objSettings.bUseDblClickToStart){
		m_TaskList.bExcludeLabelFromCheckboxes=TRUE;
	}
	ListView_SetExtendedListViewStyleEx(m_TaskList.m_hWnd, 0, LVS_EX_FULLROWSELECT);
	objSettings.iCol0Width=Validate(objSettings.iCol0Width, 0, 300, 100);
	objSettings.iCol1Width=Validate(objSettings.iCol1Width, 20, 500, 100);
	objSettings.iCol2Width=Validate(objSettings.iCol2Width, 0, 300, 100);
	m_TaskList.InsertFColumn( 0, "!!!", FALSE, LVCFMT_LEFT, objSettings.iCol0Width);
	m_TaskList.SetIItemControlType(NULL, 0);
	m_TaskList.InsertFColumn( 1, _l("Title"), TRUE, LVCFMT_LEFT, objSettings.iCol1Width);
	m_TaskList.SetFColumnImageOffset(1, 1);
	m_TaskList.SetIItemControlType(NULL, 1);
	if(!objSettings.bHideDateFld){
		m_TaskList.InsertFColumn( 2, _l("Modification date"), FALSE, LVCFMT_LEFT, objSettings.iCol2Width);
		m_TaskList.SetIItemControlType(NULL, 2);
	}
	m_TaskList.DeleteAllIRows();
	if(objSettings.iFeditH>0){
		CRect rt;
		GetDlgItem(IDC_TASKLIST)->GetWindowRect(&rt);
		rt.bottom=rt.top+objSettings.iFeditH;
		ScreenToClient(rt);
		GetDlgItem(IDC_TASKLIST)->MoveWindow(rt);
	}
	m_FEdit.bIgnoreFocus=1;
	m_FEdit.SetEventMask(m_FEdit.GetEventMask()|ENM_CHANGE);
	SetColorStyle();
}

void AppMainDlg::SetDefaultsListParams(BOOL bbOnly)
{
	CItem* pItem=NULL;
	if(!bbOnly){
		if(CItem::GetItemByID(TODO_PATH)==NULL){
			pItem=new CItemFolder(_l("ToDo"));
			pItem->SetID(TODO_PATH);
			pItem->Attach();
			InsertOrRefreshItem(pItem);
		}
		if(CItem::GetItemByID(NOTE_PATH)==NULL){
			pItem=new CItemFolder(_l("Notes"));
			pItem->SetID(NOTE_PATH);
			pItem->Attach();
			InsertOrRefreshItem(pItem);
		}
		if(CItem::GetItemByID(REMD_PATH)==NULL){
			pItem=new CItemFolder(_l("Reminders"));
			pItem->SetID(REMD_PATH);
			pItem->Attach();
			InsertOrRefreshItem(pItem);
		}
		pItem=CItem::GetItemByID(REMS_PATH);
		if(pItem==NULL){
			pItem=new CItemFolder(_l("System"));
			pItem->SetID(REMS_PATH);
			pItem->SetPath(REMD_PATH);
			pItem->Attach();
			InsertOrRefreshItem(pItem);
		}else{
			pItem->SetTitle(_l("System"));
		}
		if(CItem::GetItemByID(LINK_PATH)==NULL){
			pItem=new CItemFolder(_l("Links"));
			pItem->SetID(LINK_PATH);
			pItem->Attach();
			InsertOrRefreshItem(pItem);
		}
		if(CItem::GetItemByID(MESS_PATH)==NULL){
			pItem=new CItemFolder(_l("Messages"));
			pItem->SetID(MESS_PATH);
			pItem->Attach();
			InsertOrRefreshItem(pItem);
		}

		if(objSettings.bStartWithOptions && !IsEmbedded())
		{
			//
			CNoteSettings* NewNote=new CNoteSettings(objSettings.NoteSettings);
			NewNote->SetTitle(_l("First steps"));
			NewNote->bNeedFocusOnStart=TRUE;
			NewNote->bTopMost=TRUE;
			NewNote->bWordWrap=TRUE;
			NewNote->sText=_l("WireNote: LAN messenger with stickies and reminders")+". "+_l("Free for non-commersial use")+":cool\n\n"
				+_l("Thanks for using WireNote","Thanks for using WireNote!\nYou can do a lot of things with this program, see help for details.\n\nNow you can delete this note by pressing 'Ctrl-D' or choosing 'Delete' from context menu")+"!";
			NewNote->sText+="\n";
			NewNote->sText+=_l("Right-click this note to see more actions: hide note (Ctrl-H), attach note to other windows or documents, search selection in Google, send note over LAN or email")+"/sms :smile";
			OpenNote(NewNote);
			//
			Task* pTask=new Task(_l(WNUSAGE_TITLE),DEFAULT_WNUSAGE);
			pTask->SetID(USAGE_ID);
			pTask->SetPriority(1);
			objSettings.m_Tasks.AddTask(pTask);
			//
			AddFileLink("http://www.wiredplane.com/wirekeys/","",_l("Automation tool for power users"));
			AddFileLink("http://www.wiredplane.com/wirechanger/","",_l("Wallpaper manager with widgets and PIM"));
			AddFileLink("http://outer-station.m6.net/","",_l("Free and fresh wallpapers every day"));
			AddFileLink("http://www.wiredplane.com/?who=WireNote","",_l("WiredPlane.com homepage"));
			objSettings.AddrBook.AddModPerson("WireNote Support","support@wiredplane.com/em","WiredPlane.com support team",0,0,0);
		}

	}
	if(objSettings.sFBDir!=""){
		pItem=CItem::GetItemByID(BB_PATH);
		if(pItem==NULL){
			pItem=new CItemFolder(_l("Billboard"));
			pItem->SetID(BB_PATH);
			pItem->Attach();
			InsertOrRefreshItem(pItem);
		}else{
			pItem->SetTitle(_l("Billboard"));
		}
	}
}

void AppMainDlg::ConstructNewNote(CNoteSettings* pStartParams)
{
	CRect rRect;
	CString sTitle;
	GetActiveWindowTitle(sTitle,rRect);
	int iIndex=objSettings.m_Notes.Add(new CDLG_Note(pStartParams));
	CDLG_Note* pNote=objSettings.m_Notes.GetNote(iIndex);
	if(pNote){
		pNote->Create(CDLG_Note::IDD,objSettings.bPopupNotesOnActivation?theApp.m_pFakeWnd:GetDesktopWindow());//GetDesktopWindow() - появляются в Task Managerе, this - при клике в трее все заметки всплывают
		if(pNote->Sets.bIncomingMessage){
			// Говорим что появилось новое сообщение
			StartAnimation(objSettings.AddrBook.IsPersonKnown(pNote->Sets.sSenderPerson));
		}else{
			if(pNote->Sets.bNeedFocusOnStart && IsWindow(pNote->GetSafeHwnd())){
				::SetForegroundWindow(this->GetSafeHwnd());
				::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)pNote->GetSafeHwnd());
				pNote->m_EditField.SetFocus();
			}else{
				::SetForegroundWindow(objSettings.hHotMenuWnd);
			}
		}
	}
}

BOOL AppMainDlg::CheckItemInFilters(CItem* item)
{
	if(!item){
		return FALSE;
	}
	BOOL bRes=TRUE;
	for(int i=0;i<objSettings.mainViewFilter.GetSize();i++){
		CString sPath=item->GetPath();
		if(sPath==""){
			sPath=item->GetID();
		}
		if(sPath.Right(objSettings.mainViewFilter[i].GetLength())==objSettings.mainViewFilter[i]){
			bRes=FALSE;
			break;
		}
	}
	if(bRes && item->GetType()==1){
		if(!objSettings.bShowDone){
			bRes=!item->GetState();
		}
	}
	if(bRes && item->GetType()==3){
		if(!objSettings.bShowNonActiveReminders){
			bRes=item->GetState();
		}
	}
	return bRes;
}

void AppMainDlg::ReattachAccordingFilter()
{
	// Смотрим фильтры
	EnterCriticalSection(&objSettings.bTreeNeedUpdateLock);
	for(int i=objSettings.Items.GetSize()-1;i>=0;i--){
		CItem* item=objSettings.Items[i];
		if(!item){
			continue;
		}
		if(CheckItemInFilters(item)){
			if(item->hIRow==NULL){
				item->Attach();
			}
		}else{
			if(item->hIRow!=NULL){
				item->DeleteFromList();
			}
		}
	}
	CItem::SetModify();
	LeaveCriticalSection(&objSettings.bTreeNeedUpdateLock);
}

void AppMainDlg::OnChangeAutopopupOrg() 
{
	objSettings.APToDoList=!objSettings.APToDoList;
}

void AppMainDlg::OnSystrayOptions() 
{
	objSettings.OpenOptionsDialog("",this);//_l(DEF_COMMON)
}

afx_msg LRESULT AppMainDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	CString sNewTitle;
	GetActiveWindowTitle(sNewTitle, objSettings.ActiveWndRect);
	int iHotKey=((DWORD)wParam)&CODE_NOPREFIX;
	wParam=iHotKey;
	if(iHotKey>=FIRST_HKEYNUM && iHotKey<=(FIRST_HKEYNUM+MAX_HOTACTIONS)){
		PerformTrayAction(iHotKey-FIRST_HKEYNUM, TRUE);
		return 0;
	}
	if(iHotKey>=MAIL_HKEYNUM && iHotKey<(MAIL_HKEYNUM+objSettings.lAntiSpamAccounts)){
		objSettings.antiSpamAccounts[iHotKey-MAIL_HKEYNUM].ShowAntiSpamWindow();
		return 0;
	}
	Alert("Error hotkey code!");
	return 0;
}

void AppMainDlg::RegisterMyHotKeys()
{
	if(IsEmbedded()){
		return;
	}
	//Регистрируем горячие клавиши
	int i=0;
	BOOL bRes=TRUE;
	CString sFailed="";
	// Default action map
	for(i=1;i<(sizeof(objSettings.DefActionMapHK)/sizeof(objSettings.DefActionMapHK[0]));i++){
		if(objSettings.DefActionMapHK[i].Present()){
			BOOL bReg=RegisterIHotKey(this->m_hWnd,FIRST_HKEYNUM+i,objSettings.DefActionMapHK[i],Format("WNK_%i",i),"None");
			if(!bReg){
				sFailed+=GetIHotkeyName(objSettings.DefActionMapHK[i]);
				sFailed+=", ";
			}
			bRes&=bReg;
		}
	}
	for(int si=0;si<objSettings.lAntiSpamAccounts;si++){
		if(objSettings.antiSpamAccounts[si].dwHotkey.Present()){
			BOOL bReg=RegisterIHotKey(this->m_hWnd,MAIL_HKEYNUM+si,objSettings.antiSpamAccounts[si].dwHotkey,Format("WNK2_%i",i),"None");
			if(!bReg){
				sFailed+=GetIHotkeyName(objSettings.antiSpamAccounts[si].dwHotkey);
				sFailed+=", ";
			}
			bRes&=bReg;
		}
	}
	if(bRes!=TRUE){
		sFailed.TrimRight(", ");
		Alert(_l("Some hotkeys can`t be registered!\nMaybe they are used by other application\nFailed to register:")+" "+sFailed,_l("WireNote: Hotkeys"));
	}
}

void AppMainDlg::UnRegisterMyHotKeys()
{
	int i=0;
	//Разрегистрируем горячие клавиши
	for(i=0;i<10;i++){
		UnregisterIHotKey(this->m_hWnd,i);
	}
	for(i=FIRST_HKEYNUM;i<=(FIRST_HKEYNUM+(sizeof(objSettings.DefActionMapHK)/sizeof(objSettings.DefActionMapHK[0]))+1);i++){
		UnregisterIHotKey(this->m_hWnd,i);
	}
	for(i=MAIL_HKEYNUM;i<=(MAIL_HKEYNUM+objSettings.lAntiSpamAccounts);i++){
		UnregisterIHotKey(this->m_hWnd,i);
	}
}

BOOL checkFirstStr(const char* szIn, const char* szCharSet){
	int len=strlen(szIn)-1,k=strlen(szCharSet);
	if(len<=0 || len<k){
		return FALSE;
	}
	BOOL bFlag=TRUE;
	for(int j=0;j<k;j++){
		if(*(szIn+j)!=*(szCharSet+j)){
			bFlag=FALSE;
			break;
		}
	}
	return bFlag;
};

BOOL checkLastStr(const char* szIn, const char* szCharSet){
	int len=strlen(szIn)-1,k=strlen(szCharSet);
	if(len<=0 || len<k){
		return FALSE;
	}
	BOOL bFlag=TRUE;
	for(int j=0;j<k;j++){
		if(*(szIn+len-k+j+1)!=*(szCharSet+j)){
			bFlag=FALSE;
			break;
		}
	}
	return bFlag;
};

BOOL checkStrByDir(const char* szIn, const char* szCharSet,BOOL bForward)
{
	if(bForward){
		return checkLastStr(szIn,szCharSet);
	}else{
		return checkFirstStr(szIn,szCharSet);
	}
}

Task* CreateNewTask(const char* szPath);
afx_msg LRESULT AppMainDlg::OnCreateNewToDo(WPARAM wParam, LPARAM lParam)
{
	if(lParam==2){
		Task* pTask=CreateNewTask((const char*)wParam);
		if(pTask){
			objSettings.sTreeNeedMoveFocus=pTask->GetID();
		}
		return 0;
	}
	CDLG_Newtask* dlgNewTask=NULL;
	if(lParam==1){
		Task* objTask=(Task*)wParam;
		if(!objTask){
			return 1;
		}
		if(objTask->pCorDialog){
			SwitchToWindow(objTask->pCorDialog->GetSafeHwnd());
			return 0;
		}
		dlgNewTask=new CDLG_Newtask();
		dlgNewTask->pTask=objTask;
	}else{
		dlgNewTask=new CDLG_Newtask();
		dlgNewTask->szPath=(const char*)wParam;
	}
	dlgNewTask->Create(CDLG_Newtask::IDD,NULL);
	return 0;
}

afx_msg LRESULT AppMainDlg::OnCreateNewNote(WPARAM wParam, LPARAM lParam)
{
	ConstructNewNote((CNoteSettings*)lParam);
	return 0;
}

afx_msg LRESULT AppMainDlg::OnNeedOrganizatorReaction(WPARAM wParam, LPARAM lParam)
{
	if(wParam==0 && dlgAddBook!=NULL){
		dlgAddBook->DestroyWindow();
		dlgAddBook=NULL;
	}
	return 0;
}

void AppMainDlg::OnSystrayAddressbook() 
{
	if(dlgAddBook==NULL){
		dlgAddBook=new DLG_Addrbook;
		dlgAddBook->Create(DLG_Addrbook::IDD,theApp.m_pFakeWnd);
	}else{
		dlgAddBook->OnTrayOpenwindowFull(TRUE,TRUE);
	}
}

void AppMainDlg::OnSystrayShowNextNote()
{
	BOOL bIDWasOk=0;
	for(int k=0;k<2;k++){
		for(int i=0;i<objSettings.m_Notes.GetSize();i++){
			CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
			if(!pNote || pNote->IsMessage()){
				continue;
			}
			BOOL bShow=pNote->CanThisNoteBeShown();
			if(bShow && bIDWasOk || (i==objSettings.m_Notes.GetSize()-1 && objSettings.sLastIndexOfOpenedNote=="")){
				SwitchToWindow(pNote->GetSafeHwnd(),FALSE);
				pNote->ActivateWindow(pNote->Sets.bTopMost, TRUE);
				return;
			}
			if(objSettings.sLastIndexOfOpenedNote==pNote->Sets.GetID()){
				bIDWasOk=1;
			}
		}
	}
	objSettings.sLastIndexOfOpenedNote="";
}

void AppMainDlg::OnSystrayShowallnotes() 
{
	for(int i=0;i<objSettings.m_Notes.GetSize();i++){
		CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
		if(!pNote || pNote->IsMessage()){
			continue;
		}
		BOOL bShow=pNote->CanThisNoteBeShown();
		if(bShow && !SwitchToWindow(pNote->GetSafeHwnd(),FALSE)){
			pNote->ActivateWindow(TRUE, TRUE);
		}
	}
}

void AppMainDlg::OnSystrayHideallnotes() 
{
	for(int i=0;i<objSettings.m_Notes.GetSize();i++){
		CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
		if(!pNote || pNote->IsMessage()){
			continue;
		}
		pNote->PostMessage(WM_COMMAND,ID_NOTEMENU_HIDENOTE,0);
	}
}

BOOL bTrayActionLock=FALSE;
void AppMainDlg::PerformTrayAction(int iAction,BOOL bHotKey)
{
	if(bTrayActionLock){
		return;
	}
	bTrayActionLock=TRUE;
	switch(iAction){
	case NEW_NOTE:
		this->SetForegroundWindow();
		OnSystrayNewnote();
		break;
	case CHECKALLACC:
		OnSystrayCheckAllAcounts();
		break;
	case NEW_TODO:
		this->SetForegroundWindow();
		NewTaskPath(objSettings.sLastNewToDoPath);
		break;
	case NEW_REMD:
		this->SetForegroundWindow();
		OnSystrayNewreminder();
		break;
	case NEW_MSG:
		this->SetForegroundWindow();
		OnSystrayNewmessage();
		break;
	case OPEN_TODOLIST:
		this->SetForegroundWindow();
		OnSystrayOpen();
		break;
	case SHOW_ALLNOTES:
		PostMessage(WM_COMMAND,ID_SYSTRAY_SHOWALLNOTES,0);
		break;
	case HIDE_ALLNOTES:
		PostMessage(WM_COMMAND,ID_SYSTRAY_HIDEALLNOTES,0);
		break;
	case OPEN_ADDRBOOK:
		this->SetForegroundWindow();
		OnSystrayAddressbook();
		break;
	case OPEN_LASTUNREAD:
		if(dlgAddBook){
			int iLastUnreadMessageNum=-1;
			objSettings.m_Notes.GetMessageCount(TRUE,NULL,NULL,NULL,&iLastUnreadMessageNum);
			if(iLastUnreadMessageNum>=0){
				dlgAddBook->SendMessage(WM_COMMAND,WM_USER+TASK_NOTE_OFFSET+iLastUnreadMessageNum,0);
			}
		}
		break;
	case NEXT_NOTE:
		PostMessage(WM_COMMAND,ID_SYSTRAY_SHOWNEXT,0);
	default:
		AddError(Format("No tray action (%i)!",iAction),TRUE);
	}
	bTrayActionLock=FALSE;
}

void AppMainDlg::OnSystrayDefaction() 
{
	int bAlt=0,bShift=0,bCtrl=0;
	if(::GetKeyState(VK_CONTROL)<0){
		bCtrl=1;
	}
	if(::GetKeyState(VK_SHIFT)<0){
		bShift=1;
	}
	if(::GetKeyState(VK_MENU)<0){
		bAlt=1;
	}
	int KState=bShift+(bCtrl*2)+(bAlt*4);
	int Action=objSettings.DefActionMap[0];
	if(KState!=0){
		for(int i=1;i<=MAX_HOTACTIONS;i++){
			if(objSettings.DefActionMap[i]==KState){
				Action=i;
				break;
			}
		}
	}
	if(Action==0){
		// Действия по умолчанию не найдено - ничего не делаем
		return;
	}
	PerformTrayAction(Action);
}

BOOL bPrevScrSaverStatus=0;
void AppMainDlg::OnTimer(UINT nIDEvent) 
{
	if(iRunStatus<0){
		return;
	}
	if(nIDEvent==ID_TIMER){
		// Проверяем изменилось ли активное приложение...
		BOOL bFocusAppChanged=FALSE;
		BOOL bFocusMoved=FALSE;
		static HWND pForeWnd=NULL;
		CString sCurTitle;
		CRect ActiveWndRect,rTmp;
		if(pForeWnd && (!IsWindow(pForeWnd) || ::IsIconic(pForeWnd)))
		{
			sCurTitle=Format("%i_%i",GetTickCount(),rand());
			pForeWnd=0;
		}else{
			GetActiveWindowTitle(sCurTitle,ActiveWndRect,&pForeWnd);
			BOOL bIco=::IsIconic(pForeWnd);
			BOOL bInv=!::IsWindowVisible(pForeWnd);
			if(bIco || bInv){
				// Такие окна за людей не считаем!
				sCurTitle=Format("%i_%i",GetTickCount(),rand());
				pForeWnd=0;
			}
		}
		//TRACE(sCurTitle+"\n");
		//if(pForeWnd!=NULL)
		{
			if(sActiveAppTitle!=sCurTitle/* && sCurTitle.GetLength()>0*/){
				// Изменилось!
				sActiveAppTitle=sCurTitle;
				bFocusAppChanged=TRUE;
			}
		}
		//
		for(int i=0;i<objSettings.m_Notes.GetSize();i++){
			CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
			if(!pNote){
				continue;
			}
			// Проверяем,нужно ли сохранить заметку
			if(pNote->dwLastChangeTime!=0 && long(GetTickCount()-(pNote->dwLastChangeTime))>long(objSettings.bAutoGetNoteText)){
				pNote->SaveNoteToFile();
			}
			if(bFocusAppChanged){
				BOOL bShow=pNote->TestTitleMatchCondition();
				BOOL bNoteState=pNote->IsWindowVisible();
				// Если новое окно не пересекает записку, то ниче не делаем
				if(objSettings.bCheckPinnedNoteForRect && !ActiveWndRect.IsRectEmpty() && CSmartWndSizer::TestRectInWindows(ActiveWndRect)!=2 &&
					rTmp.IntersectRect(ActiveWndRect,pNote->Sizer.m_rMain)==0){
					continue;
				}
				if(pNote->Sets.bTopMost && pNote->Sets.GetApplicationsList()!=""){
					if(bNoteState!=bShow){
						pNote->ActivateWindow(pNote->Sets.bTopMost, bShow);
						bFocusMoved=TRUE;
					}
				}
			}
		}
		if(bFocusMoved==TRUE && pForeWnd){
			::SetForegroundWindow(pForeWnd);
		}
		// Активация ToDo
		for(int j=0;j<objSettings.m_Tasks.GetSize();j++){
			Task* pTask=objSettings.m_Tasks.GetTask(j);
			if(!pTask){
				continue;
			}
			if(bFocusAppChanged && pTask->GetApplicationsList()!=""){
				BOOL bShow=pTask->TestTitleMatchCondition_Raw(sActiveAppTitle);
				if(bShow){
					objSettings.m_Tasks.TaskEnter(pTask->GetID());
				}else{
					objSettings.m_Tasks.TaskLeave(pTask->GetID());
				}
			}
		}
		// Нужно ли проверять ремаиндеры...
		iTimerCheckCounter++;
		if(iTimerCheckCounter==TIMER_CHECKREMINDERS){
			iTimerCheckCounter=0;
			// проверяем ремаиндеры...
			int iTestRemCount=0;
			while((iTestRemCount++)<10){
				int iNum=-1;
				COleDateTime CheckTimeMustBe=objSettings.m_Reminders.GetClosestTriggerTime(iNum);
				// Если CheckTimeMustBe.GetStatus()==COleDateTime::invalid, то iNum==-1
				Reminder* rem=objSettings.m_Reminders.GetReminder(iNum);
				if(rem){
					//проверяем!
					COleDateTime CurTime=COleDateTime::GetCurrentTime();
					COleDateTimeSpan span=(CheckTimeMustBe-CurTime);
					double dSecToEvent=span.GetTotalSeconds();
					if(dSecToEvent<0){//Ремаиндер сработал!
						objSettings.m_Reminders.OnModify();
						// эта функция удалит этот ремаиндер если надо
						rem->PerformReminderActions();
					}else{
						break;
					}
				}else{
					break;
				}
			}
		}
		// Нужно ли перерисовывать главное окно...
		iUpdateTreeCounter++;
		if(iUpdateTreeCounter==TIMER_CHECKTREEUPDATE){
			if(objSettings.bTreeNeedUpdate){
				RefreshTaskList();
			}
			iUpdateTreeCounter=0;
		}
		// Нужно ли проверять не запустился ли screen saver...
		if(objSettings.bSetAwayIfScrSaver){
			iScrSaverCheckCounter++;
			if(iScrSaverCheckCounter==TIMER_CHECKAUTOAWAY){
				iScrSaverCheckCounter=0;
				BOOL bSaver=isScreenSaverActiveOrCLocked();
				// Изменилось ли состояние screensaverа с прошлого раза
				if(bPrevScrSaverStatus!=bSaver){
					bPrevScrSaverStatus=bSaver;
					BOOL bExchange=objSettings.iOnlineStatus;
					if(bSaver){
						iCurrentOnLineMode=objSettings.iOnlineStatus;
						objSettings.iOnlineStatus=1;// Away
						objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Screen Saver activated, going to Away mode"));
					}else if(iCurrentOnLineMode!=-1){
						objSettings.iOnlineStatus=iCurrentOnLineMode;
						objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Screen Saver deactivated, returning from Away mode"));
						iCurrentOnLineMode=-1;
					}
					if(bExchange!=objSettings.iOnlineStatus){
						ExchangeOnlineStatuses();
					}
				}
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

int AppMainDlg::GetItemUnderCursor(int* iCol, HIROW* hRow, BOOL* bLabel)
{
	CPoint p;
	::GetCursorPos(&p);
	m_TaskList.ScreenToClient(&p);
	CIListCtrl::CIListRow* pIRow=NULL;
	int res=m_TaskList.HitTestEx(p, iCol, pIRow);
	if(bLabel || hRow){
		BOOL bL=m_TaskList.HitTestLabel(pIRow,p);
		if(bLabel){
			*bLabel=bL;
		}
		if(hRow){
			*hRow=pIRow->m_hIRow;
		}
	}
	return res;
}

BOOL AppMainDlg::SwitchToFEdit()
{
	CRect rt;
	GetDlgItem(IDC_RICHEDIT)->GetWindowRect(&rt);
	if(rt.Height()<10){
		OnEform();
	}
	SwitchToWindow(GetSafeHwnd(),1);
	m_FEdit.SetFocus();
	m_FEdit.SetSel(m_FEdit.GetTextLength(),m_FEdit.GetTextLength()+1);
	//SetForegroundWindow();
	return 1;
}

HCURSOR hDragCursor=NULL;
int iLastUnderCursorItem=-2;
int iLastUnderCursorItemCol=-2;
BOOL AppMainDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Пока все не закружено и не проинициализировано, блокируем работу клавы+мыши
	if(iRunStatus!=1 || !bMainWindowInitialized){
		if((pMsg->message>=WM_KEYFIRST && pMsg->message<=WM_KEYLAST)
			|| (pMsg->message>=WM_MOUSEFIRST && pMsg->message<=WM_MOUSELAST)){
			return TRUE;
		}
	}
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	UINT iCtrlID=0;
	if(pMsg->hwnd!=NULL){
		iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	}
	//if(pMsg->message==WM_ACTIVATE && pMsg->wParam==WA_ACTIVE)
	//if(pMsg->message==WM_SETFOCUS){
	//if(pMsg->message==WM_ACTIVATEAPP)
	/*{
		TRACE("???\n");
		if(hLastActiveControl){
			char s[100]={0};
			::GetWindowText(hLastActiveControl,s,99);
			TRACE(">>>\n");
			TRACE(s);
			TRACE("\n>>>\n");
			::SetFocus(hLastActiveControl);
			//SwitchToWindow(hLastActiveControl);
		}
	}*/
	if(pMsg->message==WM_CTLCOLOREDIT || pMsg->message==WM_CTLCOLORSCROLLBAR){
		CDC* pDC=CDC::FromHandle(HDC(pMsg->wParam));
		DWORD cTXColor=objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
		DWORD cBGColor=objSettings.objSchemes.getBgColor(objSettings.iMainStyle);
		pDC->SetTextColor(cTXColor);
		pDC->SetBkColor(cBGColor);
		return (LRESULT)OnCtlColor(pDC,CWnd::FromHandle(HWND(pMsg->lParam)),0);
	}
	if((pMsg->message==WM_KEYDOWN || pMsg->message==WM_KEYUP)&& iCtrlID==IDC_TASKLIST){
		if(pMsg->message==WM_KEYUP){
			return TRUE;
		}
		if(pMsg->wParam==VK_LEFT || pMsg->wParam==VK_RIGHT){
			CItem* item=GetSelectedRow();
			if(item){
				HIROW hSelItem=item->hIRow;
				int iCount=m_TaskList.GetChildIRowCount(item->hIRow);
				int iCollapsed=m_TaskList.IsCollapsed(item->hIRow);
				if(pMsg->wParam==VK_RIGHT){
					if(iCollapsed){
						m_TaskList.Expand(item->hIRow);
					}
					if(iCount>0){
						hSelItem=m_TaskList.GetChildIRowAt(item->hIRow,0);
					}
				}
				if(pMsg->wParam==VK_LEFT){
					if(iCount>0 && !iCollapsed){
						m_TaskList.Collapse(item->hIRow);
					}
					if(iCount==0 || iCollapsed){
						hSelItem=m_TaskList.GetParentIRow(item->hIRow);
					}
				}
				if(hSelItem==0 || hSelItem==FL_ROOT){
					hSelItem=item->hIRow;
				}
				m_TaskList.SelectIItem(hSelItem,1);
				return TRUE;
			}
		}
	}
	if(pMsg->message==WM_CHAR && ::GetFocus()!=m_FEdit.GetSafeHwnd() && iCtrlID!=IDC_INPLACE_EDIT){
		SwitchToFEdit();
		pMsg->hwnd=m_FEdit.GetSafeHwnd();
		//iCtrlID=IDC_RICHEDIT;
	}
	BOOL bBaseRes=TRUE;
	if(pMsg->message==WM_KEYDOWN){
		if(((pMsg->wParam==VK_INSERT && isPressed(VK_SHIFT))|| (pMsg->wParam=='V' && isPressed(VK_CONTROL)))){
			if( iCtrlID!=IDC_INPLACE_EDIT){
				if(iCtrlID==IDC_RICHEDIT || hLastActiveControl==m_FEdit.GetSafeHwnd()){
					m_FEdit.PasteSpecial(CF_TEXT);
					if(::GetFocus()!=m_FEdit.GetSafeHwnd()){
						SwitchToFEdit();
					}
					return TRUE;
				}else if(iCtrlID==IDC_TASKLIST){
					if(!LinkFromClipboardX()){
						m_FEdit.PasteSpecial(CF_TEXT);
						SwitchToFEdit();
					}
					return TRUE;
				}
			}
		}
		if(iCtrlID==IDC_TASKLIST || iCtrlID==IDC_INPLACE_BUTTON){
			if(pMsg->wParam==VK_DELETE){
				OperationUnderCursor(0);
				return TRUE;
			}
			if(pMsg->wParam==VK_RETURN){
				OperationUnderCursor(1);
				return TRUE;
			}
			if(pMsg->wParam==VK_INSERT){
				OperationUnderCursor(2);
				return TRUE;
			}
			if(pMsg->wParam==VK_SPACE){
				OperationUnderCursor(3);
				return TRUE;
			}
			if(pMsg->wParam==VK_F2){
				OperationUnderCursor(4);
				return TRUE;
			}
		}
		if(pMsg->wParam==VK_ESCAPE){
			OnMinimaze();
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN && iCtrlID!=IDC_RICHEDIT){
			return TRUE;
		}
		if(isPressed(VK_CONTROL)){
			if(pMsg->wParam=='S'){
				OnSaveallBt();
				return TRUE;
			}
			if(pMsg->wParam=='T'){
				OnWndontop();
				return TRUE;
			}
			if(pMsg->wParam=='O'){
				OnOptionsBt();
				return TRUE;
			}
			if(pMsg->wParam=='F'){
				OnFindBt();
				return TRUE;
			}
		}
	}
	if(pMsg->message==WM_LBUTTONUP){
		if(dwDragLock){
			dwItemOnStopDrag=GetItemUnderCursor();
			dwDragLock=0;
			SetCursor(hDragCursor);
			ReleaseCapture();
			if(dwItemOnStopDrag!=dwItemOnStartDrag && dwItemOnStopDrag!=-1 && dwItemOnStartDrag!=-1){
				DragItem(dwItemOnStartDrag,dwItemOnStopDrag);
			}
			dwItemOnStartDrag=-1;
			dwItemOnStopDrag=-1;
			return TRUE;
		}
	}else if(pMsg->message==WM_LBUTTONDOWN  &&	iCtrlID==IDC_TASKLIST){
		dwItemOnStopDrag=dwItemOnStartDrag=GetItemUnderCursor();
	}else if(pMsg->message==WM_MOUSEMOVE){
		dwItemOnStopDragLast=dwItemOnStopDrag;
		dwItemOnStopDrag=GetItemUnderCursor();
		if(iCtrlID==IDC_TASKLIST && dwItemOnStopDrag!=dwItemOnStartDrag && dwItemOnStopDrag!=-1 && dwItemOnStartDrag!=-1){
			if(dwDragLock){
				SetCapture();
				HCURSOR hHand=::LoadCursor(NULL,MAKEINTRESOURCE(32649));
				hDragCursor=SetCursor(hHand);
			}else if(GetAsyncKeyState(VK_LBUTTON)<0){
				dwDragLock=1;
			}
		}
		if(dwDragLock && dwItemOnStopDragLast!=-1 && dwItemOnStopDrag!=-1 && dwItemOnStopDragLast!=dwItemOnStopDrag){
			m_TaskList.UpdateIRow(m_TaskList.GetIRowFromIndex(dwItemOnStopDragLast));
			m_TaskList.SelectIRow(m_TaskList.GetIRowFromIndex(dwItemOnStopDrag),TRUE);
		}
	}
	if(pMsg->message==WM_MOUSEMOVE){
		CItem* item=NULL;
		if(iCtrlID==IDC_TASKLIST){
			int iColumn=0;
			int iUnderCursorItem=GetItemUnderCursor(&iColumn);
			if(iUnderCursorItem!=-1 && ((iLastUnderCursorItem!=iUnderCursorItem) || (iLastUnderCursorItemCol!=iColumn))){
				iLastUnderCursorItem=iUnderCursorItem;
				iLastUnderCursorItemCol=iColumn;
				HIROW hIRow=m_TaskList.GetIRowFromIndex(iUnderCursorItem);
				if(hIRow){
					iCurrentItem=m_TaskList.GetIRowData(hIRow);
					item=CItem::GetItemByIndex(iCurrentItem);
					if(item){
						CString sText;
						if(iColumn==1){
							sText=item->GetItemDsc()+"\n\n"+item->GetTextContent();
						}else{
							sText=_l("Created")+": ";
							sText+=item->GetCrTime().Format(LOGTIMEFRMT);
							sText+="\n";
							sText+=_l("Priority");
							sText+=": ";
							sText+=_l(theApp.aPriorityStrings[item->GetPriority()+2]);
						}
						if(sText.GetLength()>0){
							m_tooltip.Pop();
							m_tooltip.UpdateTipText(SetTextForUpdateTipText(sText,10,FALSE),GetDlgItem(IDC_TASKLIST));
						}
					}
				}
			}
		}
		//
		static UINT iLastStatId=0;
		if((iLastStatId!=iCtrlID && iCtrlID!=0) || (iCtrlID==IDC_TASKLIST && item) || iCtrlID==IDC_RICHEDIT){
			CString sText;
			iLastStatId=iCtrlID;
			if(iCtrlID==IDC_RICHEDIT){
				CItem* item2=CItem::GetItemByID(sLastFEditID);
				if(item2){
					sText=item2->GetTitle()+". "+_l("Right-click to open context menu");
				}else{
					sText=_l("Select item in the tree");
				}
			}else if(iCtrlID==IDC_TASKLIST && item){
				sText=item->GetTitle();
			}else{
				CWnd* pCtrlWnd=GetDlgItem(iCtrlID);
				if(pCtrlWnd){
					//m_tooltip.GetText(sText,pCtrlWnd,0);
					sText=GetTooltipText(&m_tooltip, pCtrlWnd);
				}
			}
			GetDlgItem(IDC_STATUS)->SetWindowText(sText);
		}
		// Вторая часть - смена прозрачности при наводе мышки
		BOOL bCanBeAutoTranspared=0;//(objSettings.objSchemes.getAutoDisp(objSettings.iMainStyle) && objSettings.objSchemes.getAlpha(objSettings.iMainStyle)<100);
		if(bCanBeAutoTranspared){
			DWORD dwExitCode=0;
			if(MouseTracker2!=NULL){
				::GetExitCodeThread(MouseTracker2,&dwExitCode);
			}
			if(dwExitCode!=STILL_ACTIVE){
				SetWndAlpha(this->m_hWnd, 100, TRUE);
				m_Slider.SetPos(100-100);
				MainTracker2.bStopWatching=FALSE;
				MainTracker2.dwAddFlag1=bCanBeAutoTranspared?objSettings.objSchemes.getAlpha(objSettings.iMainStyle):0;
				CloseHandle(MouseTracker2);
				DWORD dwMouseTrackerThread2=0;
				MouseTracker2=CreateThread( NULL, 0, MouseTrackerThread, (LPVOID)&MainTracker2, 0, &dwMouseTrackerThread2);
				return TRUE;
			}
		}
	}
	if(pMsg->message==WM_DROPFILES){
		int iUnderCursorItem=GetItemUnderCursor();
		CItem* item=0;
		CString sListPath="";
		if(iCtrlID!=IDC_RICHEDIT){
			item=GetItemByListIndex(iUnderCursorItem);
			if(!item){
				item=CItem::GetItemByID(LINK_PATH);
			}
			if(item){
				sListPath=item->GetPath();
				if(item->GetType()==0){
					sListPath=(CString)item->GetID()+"/"+sListPath;
					item->SetState(0);
				}
			}
		}else{
			m_FEdit.GetWindowText(sListPath);
			sListPath.TrimRight();
		}
		HDROP hDropInfo=(HDROP)pMsg->wParam;
		char szFilePath[256];
		UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
		for (UINT u = 0; u < cFiles; u++){
			DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
			if(iCtrlID!=IDC_RICHEDIT){
				AddFileLink(szFilePath,sListPath);
			}else{
				if(sListPath!=""){
					sListPath+="\r\n";
				}
				sListPath+=szFilePath;
			}
		}
		DragFinish(hDropInfo);
		if(iCtrlID==IDC_RICHEDIT){
			iIgnoreChangerFEdit++;
			SetLPText(sListPath);
			iIgnoreChangerFEdit--;
			OnChangeRichedit();
		}
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONDBLCLK && iCtrlID==IDC_TASKLIST){
		int iCol=-1;
		BOOL bLabel=0;
		HIROW hh=0;
		GetItemUnderCursor(&iCol, &hh, &bLabel);
		if(hh!=0){
			if(iCol==0 || iCol==2){
				OperationUnderCursor(1);
				return TRUE;
			}
			if(objSettings.bUseDblClickToStart){
				int iCCount=m_TaskList.GetChildIRowCount(hh);
				if(iCCount==0 || bLabel){
					// ТОлько если не коллапс!
					OperationUnderCursor(1);
					return TRUE;
				}
			}
		}
	}
	bBaseRes=CDialog::PreTranslateMessage(pMsg);
	if(pMsg->message==WM_RBUTTONDOWN){
		OnRclickTasklist();
		return TRUE;
	}
	return bBaseRes;
}

CItem* AppMainDlg::GetSelectedRow()
{
	CString sText="";
	HIROW hRow=m_TaskList.GetSelectedIRow();
	if(hRow==NULL){
		iCurrentItem=-1;
		return NULL;
	}
	iCurrentItem=m_TaskList.GetIRowData(hRow);
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(!item){
		iCurrentItem=-1;
		sLastSelectedItemPath="";
		return NULL;
	}
	sLastSelectedItemPath=((CString)item->GetID()+"/")+item->GetPath();
	return item;
}

DWORD WINAPI SetMainWndLayout(LPVOID p)
{
	int iLay=(int)p;
	static BOOL bInit=0;
	//LockSetF
	SetLayeredWindowStyle(pMainDialogWindow->m_hWnd,1);
	SetWndAlpha(pMainDialogWindow->m_hWnd, iLay, TRUE);
	if(bInit==0){
		bInit=1;
		SwitchToWindow(pMainDialogWindow->m_Slider,1);
	}
	return 0;
}

LRESULT AppMainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	/*
	if(message==WM_MOUSEWHEEL){
		if(wParam&0x80000000){
			m_TaskList.LineScroll(1);
		}else{
			m_TaskList.LineScroll(-1);
		}
		return 0;
	}
	*/
	if(message==WM_VSCROLL){
		int iCtrlID=::GetDlgCtrlID(::GetFocus());
		if(iCtrlID==IDC_SLIDER_TRANS)
		{
			int iLay=100-m_Slider.GetPos();
			if(iLay<30){
				iLay=30;
				m_Slider.SetPos(100-iLay);
			}
			FORK(SetMainWndLayout,iLay);
			//m_Slider.SetFocus();
			static int i=0;
			TRACE("Here %i\n",i++);
			return TRUE;
		}
	}
	if(message==WM_QUERYENDSESSION){
		return TRUE;
	}
	if(message==WM_DISPLAYCHANGE){
		CSmartWndSizer::GetScreenRect(0,0)=CRect(0,0,0,0);
		if(objSettings.bAdjustNotesPosOnScr){
			for(int i=0;i<objSettings.m_Notes.GetSize();i++){
				CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
				if(!pNote){
					continue;
				}
				objSettings.m_Notes.GetNote(i)->Sizer.ApplyLayout();
			}
		}
	}
	LRESULT DispatchResultFromSizer=0;
	BOOL bDispRes=Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer);
	if((bDispRes && message==WM_SIZE) || message==SETCOLW){
		CRect rt;
		m_TaskList.GetClientRect(&rt);
		int iCol0w=m_TaskList.GetColumnWidth(0);
		int iCol1w=m_TaskList.GetColumnWidth(1);
		objSettings.iCol0Width=iCol0w;
		objSettings.iCol1Width=iCol1w;
		DWORD dwCol2Width=0;
		if(!objSettings.bHideDateFld){
			objSettings.iCol2Width=m_TaskList.GetColumnWidth(2);
			dwCol2Width=objSettings.iCol2Width;
		}
		objSettings.iCol1Width=rt.Width()-objSettings.iCol0Width-dwCol2Width-1;
		if(iCol1w!=objSettings.iCol1Width){
			m_TaskList.SetColumnWidth(1,objSettings.iCol1Width);
		}
	}
	if(bDispRes==TRUE){
		return DispatchResultFromSizer;
	}
	if(message==WM_HELP){
		ShowHelp("Overview");
		return 0;
	}
	if(message==WM_INITMENUPOPUP){
		bAnyPopupIsActive=TRUE;
	}
	if(message==WM_ACTIVATEAPP){
		if(wParam==FALSE){
			bAnyPopupIsActive=FALSE;
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void AppMainDlg::StopMouseTracker()
{
	MainTracker.bStopWatching=TRUE;
}

void AppMainDlg::StartMouseTracker()
{
	DWORD dwExitCode=STILL_ACTIVE;
	if(MouseTracker!=NULL){
		::GetExitCodeThread(MouseTracker,&dwExitCode);
	}
	if(dwExitCode!=STILL_ACTIVE){
		CloseHandle(MouseTracker);
	}
	if(MouseTracker==NULL || dwExitCode!=STILL_ACTIVE){
		MainTracker.bStopWatching=FALSE;
		MouseTracker=CreateThread( NULL, 0, MouseTrackerThread, (LPVOID)&MainTracker, 0, &dwMouseTrackerThread);
	}
}

void AppMainDlg::TrackTaskPopupMenu(CItem* item,DWORD dwLims)
{
	BOOL bDone=FALSE,bCur=FALSE;
	Task* pTask=objSettings.m_Tasks.GetTask(item->GetID());
	bDone=pTask->IsDone();
	bCur=pTask->IsActive();
	CMenu menu;
	menu.CreatePopupMenu();
	AddMenuString(&menu,0,item->GetTitle(DEF_MENUTITLELEN),_bmp().Get(_IL(item->GetIconNum())),FALSE,NULL,FALSE,TRUE);
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(&menu,ID_TODO_SETASCURRENT,_l("Mark as 'In progress'"),_bmp().Get(IDB_BM_TODOCURRENT),FALSE,NULL,bCur);
	AddMenuString(&menu,ID_TODO_SETASNOTCURRENT,_l("Mark as 'Paused'"),_bmp().Get(IDB_BM_TODODONE),FALSE,NULL,!bCur);
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(&menu,ID_TODO_MARKASDONE,_l("Mark as done"),_bmp().Get(_IL(14)),FALSE,NULL,bDone);
	AddMenuString(&menu,ID_TODO_MARKASUNDONE,_l("Mark as not done"),_bmp().Get(_IL(15)),FALSE,NULL,!bDone);
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	if(!dwLims){
		AddMenuString(&menu,ID_TODO_MODIFYTODO,_l("Modify ToDo"),_bmp().Get(IDB_BM_TODOMOD));
		AddMenuString(&menu,ID_TODO_NEWTODO,_l("New ToDo")+getActionHotKeyDsc(NEW_TODO),_bmp().Get(IDB_BM_NEWTODO));
		menu.AppendMenu(MF_SEPARATOR, 0, "");
	}
	{
		CMenu pOptions;
		pOptions.CreatePopupMenu();
		AddMenuString(&pOptions,ID_TODO_ADDOPTIONS,_l("Pin to window/document")+(dwLims?("\tCtrl-O"):("")),_bmp().Get(IDB_PINNED_UP_APP));
		AppendStylesMenu(&pOptions,item);
		AddMenuString(&pOptions,ID_TODO_TODOSETTINGS,_l("ToDo settings"),_bmp().Get(_IL(88)));
		AddMenuSubmenu(&menu,&pOptions,_l("Options")+"\t...",_bmp().Get(IDB_BM_OPTIONS));
	}
	AddMenuString(&menu,ID_TODO_SAVETOFILE,_l("Save to file"),_bmp().Get(IDB_BM_SAVE));
	AddMenuString(&menu,ID_TODO_REMINDABOUT,_l("Remind later"),_bmp().Get(IDB_BM_NEWREMAINDER));
	if(!objSettings.bNoSend){
		if(objSettings.AddrBook.GetSize()>0){
			menu.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&menu,ID_ITEM_SENDASMESSAGE,_l("Send with message"),_bmp().Get(IDB_BM_NEWMESSAGE));
		}
	}
	AppendBBMenu(&menu,pTask->IsShared());
	//AddMenuString(&menu,ID_TODO_HIDESHOWDONE,_l("Hide/Show done"));
	AppendAdverts(&menu);
	AddMenuString(&menu,ID_TODO_DELETETODO,_l("Delete"),_bmp().Get(IDB_BM_DEL));
	::SetMenuDefaultItem(menu, 0, TRUE);
	CPoint pos;
	GetCursorPos(&pos);
	iCurrentItem=item->index;// Для правильного вызова из "Modify ToDo"
	::TrackPopupMenu(menu.GetSafeHmenu(), 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	menu.DestroyMenu();
}

void AppMainDlg::TrackPriorityPopupMenu(CItem* item)
{
	if(item==0){
		return;
	}
	int iPrior=item->GetPriority();
	CMenu menu;
	menu.CreatePopupMenu();
	AddMenuString(&menu,0,item->GetTitle(DEF_MENUTITLELEN),_bmp().Get(_IL(item->GetIconNum())),FALSE,NULL,FALSE,TRUE);
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(&menu,ID_PRIORITY_LOWEREST,_l("Lowest"),_bmp().Get(_IL(PRIOR_ICONS_OFFSET-2)),FALSE,NULL,iPrior==-2);
	AddMenuString(&menu,ID_PRIORITY_LOW,_l("Low"),_bmp().Get(_IL(PRIOR_ICONS_OFFSET-1)),FALSE,NULL,iPrior==-1);
	AddMenuString(&menu,ID_PRIORITY_NORMAL,_l("Normal"),_bmp().Get(_IL(PRIOR_ICONS_OFFSET)),FALSE,NULL,iPrior==0);
	AddMenuString(&menu,ID_PRIORITY_HIGH,_l("High"),_bmp().Get(_IL(PRIOR_ICONS_OFFSET+1)),FALSE,NULL,iPrior==1);
	AddMenuString(&menu,ID_PRIORITY_HIGHEST,_l("Highest"),_bmp().Get(_IL(PRIOR_ICONS_OFFSET+2)),FALSE,NULL,iPrior==2);
	AppendAdverts(&menu);
	AppendStylesMenu(&menu,item);
	CPoint pos;
	GetCursorPos(&pos);
	::TrackPopupMenu(menu.GetSafeHmenu(), 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	menu.DestroyMenu();
}

void AppMainDlg::TrackFolderPopupMenu(CItem *item)
{
	CMenu* pSubMenu;
	pSubMenu=new CMenu();
	pSubMenu->CreatePopupMenu();
	AddMenuString(pSubMenu,0,item->GetTitle(DEF_MENUTITLELEN),_bmp().Get(_IL(item->GetIconNum())),FALSE,NULL,FALSE,TRUE);
	pSubMenu->AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(pSubMenu,ID_FOLDER_RENAMEFOLDER,_l("Rename"),_bmp().Get(_IL(56)));
	AddMenuString(pSubMenu,ID_FOLDER_CREATENEW,_l("New folder"),_bmp().Get(_IL(5)));
	CString sID=item->GetID();
	if(sLastSelectedItemPath.Find(TODO_PATH)!=-1 || sID==TODO_PATH){
		AddMenuString(pSubMenu,ID_FOLDER_ADDTASK,_l("New ToDo")+getActionHotKeyDsc(NEW_TODO),_bmp().Get(IDB_BM_NEWTODO));
		AddMenuString(pSubMenu,ID_TODO_TODOREPORT,_l("ToDo report"),_bmp().Get(IDB_BM_TODOINFO));
		/*if(objSettings.bShowDone){
			AddMenuString(pSubMenu,ID_TODO_HIDESHOWDONE,_l("Hide done"),theApp.dwImagesFromBitmapHolder+14);
		}else{
			AddMenuString(pSubMenu,ID_TODO_HIDESHOWDONE,_l("Show done"),theApp.dwImagesFromBitmapHolder+14);
		}*/
	}
	if(sLastSelectedItemPath.Find(NOTE_PATH)!=-1 || sID==NOTE_PATH){
		AddMenuString(pSubMenu,ID_FOLDER_ADDNOTE,_l("New Note")+getActionHotKeyDsc(NEW_NOTE),_bmp().Get(IDB_BM_NEWNOTE));
	}
	if(!objSettings.bNoSend){
		if(sLastSelectedItemPath.Find(MESS_PATH)!=-1 || sID==MESS_PATH){
			AddMenuString(pSubMenu,ID_SYSTRAY_NEWMESSAGE,_l("New Message")+getActionHotKeyDsc(NEW_MSG),_bmp().Get(IDB_BM_NEWMESSAGESM));
		}
	}
	if(sLastSelectedItemPath.Find(REMD_PATH)!=-1 || sID==REMD_PATH){
		AddMenuString(pSubMenu,ID_FOLDER_ADDREMINDER,_l("New Reminder")+getActionHotKeyDsc(NEW_REMD),_bmp().Get(IDB_BM_NEWREMAINDER));
		/*if(objSettings.bShowNonActiveReminders){
			AddMenuString(pSubMenu,ID_REMINDERMENU_SHOWHIDEINACTIVE,_l("Hide Inactive"));
		}else{
			AddMenuString(pSubMenu,ID_REMINDERMENU_SHOWHIDEINACTIVE,_l("Show Inactive"));
		}*/
	}
	AppendAdverts(pSubMenu);
	AddMenuString(pSubMenu,ID_FOLDER_DELETE,_l("Delete folder"),_bmp().Get(IDB_BM_DEL));
	AddMenuString(pSubMenu,ID_FOLDER_SETASTRAYFOLDER,_l("Show in tray"));
	if(strlen(item->GetPath())==0){
		pSubMenu->EnableMenuItem(ID_FOLDER_DELETE,TRUE);
	}else{
		pSubMenu->EnableMenuItem(ID_FOLDER_DELETE,FALSE);
	}
	::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);
	CPoint pos;
	GetCursorPos(&pos);
	::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	delete pSubMenu;
}

void AppMainDlg::TrackReminderPopupMenu(CItem *item)
{
	CMenu menu;
	menu.CreatePopupMenu();
	AddMenuString(&menu,0,item->GetTitle(DEF_MENUTITLELEN),_bmp().Get(_IL(item->GetIconNum())),FALSE,NULL,FALSE,TRUE);
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	Reminder* rem=objSettings.m_Reminders.GetReminder(item->GetID());
	AddMenuString(&menu,ID_REMINDERMENU_MODIFYREMINDER,_l("Modify"),_bmp().Get(IDB_BM_TODOINFO));
	AddMenuString(&menu,ID_REMINDERMENU_ACTIVATEREMINDER,_l("Activate"),_bmp().Get(_IL(16)),FALSE,NULL,rem->GetState());
	AddMenuString(&menu,ID_REMINDERMENU_DEACTIVATEREMINDER,_l("Deactivate"),_bmp().Get(_IL(17)),FALSE,NULL,!rem->GetState());
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	{
		CMenu menuAdd;
		menuAdd.CreatePopupMenu();
		if(!objSettings.bNoSend){
			if(objSettings.AddrBook.GetSize()>0){
				AddMenuString(&menuAdd,ID_ITEM_SENDASMESSAGE,_l("Send with message"),_bmp().Get(IDB_BM_NEWMESSAGE));
				menuAdd.AppendMenu(MF_SEPARATOR, 0, "");
			}
		}
		AddMenuString(&menuAdd,ID_REMINDERMENU_SHOWHIDEINACTIVE,_l("Show/hide inactive"));
		AddMenuString(&menuAdd,ID_SYSTRAY_REMINDERSETTINGS,_l("Reminder settings"),_bmp().Get(IDB_BM_OPTIONS));
		AddMenuSubmenu(&menu,&menuAdd,_l("Additional")+"\t...",_bmp().Get(_IL(54)));
	}
	AppendStylesMenu(&menu,item);
	AppendBBMenu(&menu,rem->IsShared());
	AppendAdverts(&menu);
	AddMenuString(&menu,ID_REMINDERMENU_DELETEREMINDER,_l("Delete"),_bmp().Get(IDB_BM_DEL),FALSE,NULL,rem->bUndeletable);
	::SetMenuDefaultItem(menu, 0, TRUE);
	CPoint pos;
	GetCursorPos(&pos);
	::TrackPopupMenu(menu.GetSafeHmenu(), 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
}

void AppMainDlg::TrackFileLinkPopupMenu(CItem *item)
{
	CMenu menu;
	menu.CreatePopupMenu();
	AddMenuString(&menu,IDR_FILELINKMENU_ACTIVATE,item->GetTitle(DEF_MENUTITLELEN),_bmp().Get(_IL(item->GetIconNum())));
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(&menu,ID_FILELINKMENU_RENAME,_l("Rename"),_bmp().Get(_IL(56)));
	AddMenuString(&menu,ID_FILELINKMENU_EDITLINK,_l("Edit link"),_bmp().Get(_IL(56)));
	AddMenuString(&menu,ID_FILELINKMENU_COPYTOCLIPBOARD,_l("Copy to clipboard"),_bmp().Get(_IL(43)));
	AppendStylesMenu(&menu,item);
	AppendAdverts(&menu);
	AddMenuString(&menu,IDR_FILELINKMENU_DEL,_l("Delete"),_bmp().Get(IDB_BM_DEL));
	::SetMenuDefaultItem(menu, 0, TRUE);
	CPoint pos;
	GetCursorPos(&pos);
	::TrackPopupMenu(menu.GetSafeHmenu(), 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	menu.DestroyMenu();
}

void AppMainDlg::OnRclickTasklist()
{
	CItem* item=GetSelectedRow();
	if(item==0){
		return;
	}
	if(m_TaskList.GetSelectedIItem()==0){
		TrackPriorityPopupMenu(item);
		return;
	}
	TrackItemMenu(item);
}

void AppMainDlg::TrackItemMenu(CItem* item)
{
	if(item){
		switch(item->GetType()){
		case 0:
			TrackFolderPopupMenu(item);
			break;
		case 1://todo
			TrackTaskPopupMenu(item);
			break;
		case 2://notes
			{
				CDLG_Note* note=objSettings.m_Notes.FindNoteByID(item->GetID());
				if(note){
					note->PopupMenu(TRUE);
				}
				break;
			}
		case 3://reminders
			TrackReminderPopupMenu(item);
			break;
		case 4://filelink
			TrackFileLinkPopupMenu(item);
			break;
		}
	}
}

void AppMainDlg::OnTodoNewtodo() 
{
	NewTaskPath(objSettings.sLastNewToDoPath);
}

void AppMainDlg::OnTodoSetascurrent() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		objSettings.m_Tasks.TaskEnter(item->GetID(),TRUE);
	}
}

void AppMainDlg::Modifytodo(CItem* item)
{
	OnCreateNewToDo(WPARAM(objSettings.m_Tasks.LoadTaskByID(item->GetID())),1);
	return;
}

void AppMainDlg::OnTodoModifytodo()
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		Modifytodo(item);
	}
}

void AppMainDlg::OnTodoMarkasundone()
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		objSettings.m_Tasks.MarkAsNotDone(item->GetID());
	}
}

void AppMainDlg::OnTodoMarkasdone()
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		objSettings.m_Tasks.MarkAsDone(item->GetID());
	}
}

void AppMainDlg::OnTodoTodoreport()
{
	if(objSettings.iLikStatus<=1){
		Alert(_l(UNREG_FUNC_TEXT),_l(UNREG_FUNC_TITLE));
		return;
	}
	DLG_ChooseDate dlgDate(this);
	if(dlgDate.DoModal()==IDOK){
		COleDateTime refDateTime;
		refDateTime.SetDateTime(dlgDate.m_Date.GetYear(), dlgDate.m_Date.GetMonth(), dlgDate.m_Date.GetDay(), 0,0,0);
		objSettings.m_Tasks.Analyze(refDateTime,sLastSelectedItemPath);
		sLastSelectedItemPath="";
	}	
}

void AppMainDlg::OnClose() 
{
	CDialog::OnClose();
}

void AppMainDlg::Finalize()
{
	IsAppStopping()=1;
	iRunStatus=-2;
	HANDLE hASleepTimer=OpenWaitableTimer(TIMER_ALL_ACCESS|TIMER_MODIFY_STATE,FALSE,"WN_ASleepTimer");
	if(hASleepTimer){
		SetEvent(hASleepTimer);
		CloseHandle(hASleepTimer);
	}
	// Сохраняем все...
	objSettings.iCol0Width=m_TaskList.GetFColumnWidth(0,0);
	objSettings.iCol1Width=m_TaskList.GetFColumnWidth(1,0);
	if(!objSettings.bHideDateFld){
		objSettings.iCol2Width=m_TaskList.GetFColumnWidth(2,0);
	}

	CRect rt;
	GetDlgItem(IDC_TASKLIST)->GetWindowRect(&rt);
	objSettings.iFeditH=rt.Height();
	objSettings.m_Tasks.LeaveAllTasks();
	if(dlgAddBook){
		dlgAddBook->DestroyWindow();
		dlgAddBook=NULL;
	}
	// Убиваем spam-окна
	for(int i=0;i<objSettings.antiSpamAccounts.GetSize();i++){
		if(objSettings.antiSpamAccounts[i].wnd!=NULL){
			objSettings.antiSpamAccounts[i].wnd->DestroyWindow();
			objSettings.antiSpamAccounts[i].wnd=NULL;
		}
	}
	objSettings.SaveAll();
	// До SaveAll pMainDialogWindow должен быть нормальным!
	pMainDialogWindow=NULL;
	objSettings.bSaveAllowed=0;
	CItem::DeleteFolders();
	if(::IsWindow(theApp.m_pMainWnd->GetSafeHwnd())){
		theApp.m_pMainWnd->DestroyWindow();
	}
}

void AppMainDlg::OnEndSession(BOOL bEnding) 
{
	objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Windows station shutting down")+"...");
	CDialog::OnEndSession(bEnding);
	if(bEnding){
		Finalize();
	}	
}

void AppMainDlg::OnCustomDialog(NMHDR * pNotifyStruct, LRESULT * result)
{
	static long isDialogInProcess=0;
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(pRowState==NULL || isDialogInProcess>0){
		*result=-1;
		return;
	}
	SimpleTracker Track(isDialogInProcess);
	*result=0;//принять измененное значение
	CItem* item=CItem::GetItemByIndex(pRowState->dwData);
	if(!item){
		*result=-1;
		return;
	}
	OperationUnderCursor(1,item);
	return;
}

void AppMainDlg::OnEndEditMainTreeList(NMHDR * pNotifyStruct, LRESULT * result)
{
	OnEndEditMainTreeListM(pNotifyStruct, result,0);
}

void AppMainDlg::OnEndEditMainTreeListC(NMHDR * pNotifyStruct, LRESULT * result)
{
	OnEndEditMainTreeListM(pNotifyStruct, result,1);
}

void AppMainDlg::OnEndEditMainTreeListI(NMHDR * pNotifyStruct, LRESULT * result)
{
	OnEndEditMainTreeListM(pNotifyStruct, result,2);
}

void AppMainDlg::OnEndEditMainTreeListM(NMHDR * pNotifyStruct, LRESULT * result, int iSt)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(!pRowState || pRowState->iIItem==-1){
		*result=-1;
		return;
	}
	const char* szTitle=pRowState->strText;
	DWORD dwCheck=-1;
	if(iSt==1){
		dwCheck=pRowState->dwNotifyData;
		szTitle=NULL;
	}else if(strchr(szTitle,'{')!=NULL || strchr(szTitle,'}')!=NULL || strchr(szTitle,'/')!=NULL){
		*result=-1;
		return;
	}
	CItem* item=CItem::GetItemByIndex(pRowState->dwData);
	if(!item){
		*result=-1;
		return;
	}
	switch(item->GetType()){
	case 1:
		((Task*)item)->ActivateChanges(szTitle,NULL,dwCheck==-1?NULL:&dwCheck);
		break;
	case 2:
		((CNoteSettings*)item)->ActivateChanges(szTitle,NULL,dwCheck==-1?NULL:&dwCheck);
		break;
	case 3:
		((Reminder*)item)->ActivateChanges(szTitle,NULL,dwCheck==-1?NULL:&dwCheck);
		break;
	default:
		item->ActivateChanges(szTitle,NULL,dwCheck==-1?NULL:&dwCheck);
		break;
	}
	if(item->GetType()==0){
		SaveFolders();
	}
	if(item->GetType()==4){
		SaveLinks();
	}
	*result=0;
	if(dwLastEditedLastType!=DWORD(-1) && pRowState->hIRow){
		m_TaskList.SetIItemControlType(pRowState->hIRow,1,dwLastEditedLastType);
	}
	dwLastEditedLastType=DWORD(-1);
}

void AppMainDlg::OnRemindermenuActivatereminder() 
{
	CItem* item=GetSelectedRow();
	if(item){
		item->SetState(1);
	}
}

void AppMainDlg::OnRemindermenuDeactivatereminder() 
{
	CItem* item=GetSelectedRow();
	if(item){
		item->SetState(0);
	}
}

void AppMainDlg::OnRemindermenuDeletereminder() 
{
	CItem* item=GetSelectedRow();
	if(item){
		objSettings.m_Reminders.Delete(item->GetID());
	}
}

void AppMainDlg::OnRemindermenuModifyreminder() 
{
	CItem* item=GetSelectedRow();
	if(item){
		ModifyReminder(item->GetID());
	}
}

void AppMainDlg::ModifyReminder(const char* szID)
{
	Reminder* rem=objSettings.m_Reminders.GetReminder(szID);
	if(rem){
		rem->CreateRemDialog();
	}
}

CItem* AppMainDlg::GetItemByListIndex(int iIndex)
{
	if(iIndex==-1){
		return NULL;
	}
	HIROW hRow=m_TaskList.GetIRowFromIndex(iIndex);
	if(hRow==NULL){
		return NULL;
	}
	return CItem::GetItemByIndex(m_TaskList.GetIRowData(hRow));
}

void AppMainDlg::DragItem(CItem* iStartItem,CItem* iStopItem)
{
	iStopItem->ItemChanged();
	CString sPath;
	if(iStopItem->GetType()==0){
		sPath=(CString)iStopItem->GetID()+"/"+(CString)iStopItem->GetPath();
	}else{
		sPath=iStopItem->GetPath();
	}
	hRowForDrag=iStopItem->hIRow;
	nRowForDragDir=(m_TaskList.GetIRowIndex(iStopItem->hIRow)>m_TaskList.GetIRowIndex(iStartItem->hIRow))?1:-1;
	iStartItem->Detach();
	iStartItem->SetPath(sPath);
	iStartItem->Attach(iStopItem->GetType()==0?1:2);
}

void AppMainDlg::DragItem(DWORD dwItemOnStartDrag,DWORD dwItemOnStopDrag)
{
	if(dwItemOnStartDrag==-1 || dwItemOnStopDrag==-1 || dwItemOnStartDrag==dwItemOnStopDrag){
		return;
	}
	CItem* iStartItem=GetItemByListIndex(dwItemOnStartDrag);
	if(!iStartItem){
		return;
	}
	CItem* iStopItem=GetItemByListIndex(dwItemOnStopDrag);
	if(!iStopItem){
		return;
	}
	if(iStopItem->GetType()>6 || iStartItem->GetType()>4 || iStartItem->GetType()==0){
		return;
	}
	if(iStopItem->hIRow==NULL || iStartItem->hIRow==NULL){
		return;
	}
	DragItem(iStartItem,iStopItem);
}

void AppMainDlg::OnFolderDelete()
{
	CItem* itemUnder=GetSelectedRow();
	if(strlen(itemUnder->GetPath())==0 || itemUnder->GetType()!=0){
		return;
	}
	EnterCriticalSection(&objSettings.bTreeNeedUpdateLock);
	CString sID=itemUnder->GetID();
	CItem::DeleteItem(sID);
	// Делетим путь через эту хрень
	int size=objSettings.Items.GetSize();
	const char* szPathPart=NULL;
	const char* szPath=NULL;
	for(int i=0;i<size;i++){
		if(!objSettings.Items[i]){
			continue;
		}
		szPath=objSettings.Items[i]->GetPath();
		szPathPart=strstr(szPath,sID);
		if(szPathPart!=NULL){
			CString sNewPath=szPath;
			sNewPath=sNewPath.Left(szPathPart-szPath)+(CString)(szPathPart+strlen(sID)+1);
			objSettings.Items[i]->SetPath(sNewPath);
			objSettings.Items[i]->hIRow=NULL;
		}
	}
	SaveFolders();
	LeaveCriticalSection(&objSettings.bTreeNeedUpdateLock);
}

void AppMainDlg::SaveFolders()
{
	static long lLock=0;
	if(lLock>0){// Процесс уже идет
		return;
	}
	SimpleTracker Track(lLock);
	CDataXMLSaver xmlFile(getFileFullName(objSettings.sFolderFile),"FOLDERS",FALSE);
	int iCount=0;
	for(int i=0;i<objSettings.Items.GetSize();i++){
		if(!objSettings.Items[i] || objSettings.Items[i]->GetType()!=0){
			continue;
		}
		CDataXMLSaver xmlData("");
		xmlData.putValue("item",objSettings.Items[i]->ConvertToString(),XMLNoConversion);
		if(objSettings.Items[i]->hIRow){
			xmlData.putValue("collapsed",m_TaskList.IsCollapsed(objSettings.Items[i]->hIRow));
		}
		xmlFile.putValue(Format("folder%i",iCount++),xmlData.GetResult(),XMLNoConversion);
	}
}

void AppMainDlg::SaveLinks()
{
	static long lLock=0;
	if(lLock>0){// Процесс уже идет
		return;
	}
	SimpleTracker Track(lLock);
	CDataXMLSaver xmlFile(getFileFullName(objSettings.sLinksFile),"LINKS",FALSE);
	int iCount=0;
	for(int i=0;i<objSettings.Items.GetSize();i++){
		if(!objSettings.Items[i] || objSettings.Items[i]->GetType()!=4){
			continue;
		}
		CDataXMLSaver xmlData("");
		xmlData.putValue("item",objSettings.Items[i]->ConvertToString(),XMLNoConversion);
		xmlData.putValue("link-path",((CItemFileLink*)objSettings.Items[i])->sFilePath);
		xmlFile.putValue(Format("link%i",iCount++),xmlData.GetResult(),XMLNoConversion);
	}
}

void AppMainDlg::LoadFolders()
{
	SetDefaultsListParams();
	CDataXMLSaver xmlFile(getFileFullName(objSettings.sFolderFile),"FOLDERS",TRUE);
	int iCount=0,iCollapsed=0;
	CString sData,sTmp;
	while(xmlFile.getValue(Format("folder%i",iCount++),sData)){
		CDataXMLSaver xmlFolder(sData);
		xmlFolder.getValue("item",sTmp);
		CItemFolder* item=new CItemFolder();
		item->ConvertFromString(sTmp,"");
		if(item->isItemCorrupted()){
			delete item;
			continue;
		}
		CItemFolder* itemold=(CItemFolder*)CItem::GetItemByID(item->GetID());
		if(itemold!=NULL){// Такая папка уже есть, меняем ее параметры на сохранявшиеся
			itemold->ConvertFromString(sTmp,"");
			delete item;
			item=itemold;
		}
		xmlFolder.getValue("collapsed",iCollapsed);
		item->SetState(iCollapsed);
		item->Attach();
	}
}

void AppMainDlg::LoadLinks()
{
	CDataXMLSaver xmlFile(getFileFullName(objSettings.sLinksFile),"LINKS",TRUE);
	int iCount=0;
	CString sData,sTmp;
	while(xmlFile.getValue(Format("link%i",iCount++),sData)){
		CDataXMLSaver xmlData(sData);
		xmlData.getValue("item",sTmp);
		CItemFileLink* item=new CItemFileLink();
		item->ConvertFromString(sTmp,"");
		if(item->isItemCorrupted()){
			delete item;
			continue;
		}
		CItemFolder* itemold=(CItemFolder*)CItem::GetItemByID(item->GetID());
		if(itemold!=NULL){
			delete item;
			continue;
		}
		xmlData.getValue("link-path",item->sFilePath);
		item->Attach();
	}
}

void AppMainDlg::OnPriorityHigh() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		item->SetPriority(1);
	}
}

void AppMainDlg::OnPriorityHighest() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		item->SetPriority(2);
	}
}

void AppMainDlg::OnPriorityLow() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		item->SetPriority(-1);
	}
}

void AppMainDlg::OnPriorityLowerest() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		item->SetPriority(-2);
	}	
}

void AppMainDlg::OnPriorityNormal() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		item->SetPriority(0);
	}
}

void AppMainDlg::TraceItemList(const char* szTitle="Trace")
{
	CString sres=szTitle;
	sres+=":\n";
	int size=objSettings.Items.GetSize();
	for(int i=0;i<size;i++){
		CString sLine="NULL";
		CItem* pItem=objSettings.Items.GetAt(i);
		if(pItem){
			int iListIndex=m_TaskList.GetListIndex(pItem->hIRow);
			sLine=Format("(p%i,li%i):%s/(%s)/st%i;",pItem->GetPriority(),iListIndex,pItem->GetTitle(),pItem->GetPath(),pItem->GetState());
			if(pItem->hIRow==NULL){
				sLine+=" hIRow=NULL";
			}else{
				sLine+=" hIRow="+Format("%i",m_TaskList.GetIRowData(pItem->hIRow));
			}
		}
		sres+=Format("%i. %s\n",i+1,sLine);
	}
	Alert(sres);
}

void AppMainDlg::SortList(int iColumn)
{
	EnterCriticalSection(&objSettings.bTreeNeedUpdateLock);
	BOOL bDir=1;
	if(iColumn<0){
		bDir=-1;
		iColumn=-iColumn;
	}
	int i=0,j=0;
	int size=objSettings.Items.GetSize();
	for(i=0;i<size-1;i++){
		if(objSettings.Items.GetAt(i)==NULL){
			continue;
		}
		if(objSettings.Items.GetAt(i)->GetType()==0){
			continue;
		}
		for(j=i+1;j<size;j++){
			if(objSettings.Items.GetAt(j)==NULL){
				continue;
			}
			if(objSettings.Items.GetAt(j)->GetType()==0){
				continue;
			}
			if(strcmp(objSettings.Items.GetAt(i)->GetPath(),objSettings.Items.GetAt(j)->GetPath())!=0){
				continue;
			}
			BOOL bNeedSwap=FALSE;
			switch(iColumn){
			case 1:
				bNeedSwap=((bDir>0 && (objSettings.Items.GetAt(i)->GetPriority()>objSettings.Items.GetAt(j)->GetPriority()))||(bDir<0 && (objSettings.Items.GetAt(i)->GetPriority()<objSettings.Items.GetAt(j)->GetPriority())));
				break;
			case 2:
				bNeedSwap=((strcmp(objSettings.Items.GetAt(i)->GetTitle(),objSettings.Items.GetAt(j)->GetTitle())*bDir)>0);
				break;
			case 3:
				bNeedSwap=((bDir>0 && (objSettings.Items.GetAt(i)->GetCrTime()>objSettings.Items.GetAt(j)->GetCrTime()))||(bDir<0 && (objSettings.Items.GetAt(i)->GetCrTime()<objSettings.Items.GetAt(j)->GetCrTime())));
				break;
			default:
				{
					int iInd1=objSettings.Items.GetAt(i)->iListIndex;
					int iInd2=objSettings.Items.GetAt(j)->iListIndex;
					if(iInd1!=-1 && iInd2!=-1){
						if(iInd1>iInd2){
							bNeedSwap=TRUE;
						}
					}
					break;
				}
			}
			if(bNeedSwap){
				CItem::SwapItems(i,j);
			}
		}
	}
	// Сортим положение на экране в списке
	for(i=0;i<size-1;i++){
		CItem* pItem=objSettings.Items.GetAt(i);
		if(pItem==NULL || pItem->hIRow==NULL){
			continue;
		}
		if(pItem->GetType()==0){
			continue;
		}
		for(j=i+1;j<size;j++){
			CItem* pItem2=objSettings.Items.GetAt(j);
			if(pItem2==NULL || pItem2->hIRow==NULL){
				continue;
			}
			if(pItem2->GetType()==0){
				continue;
			}
			if(strcmp(pItem->GetPath(),pItem2->GetPath())!=0){
				continue;
			}
			int iListIndexI=m_TaskList.GetListIndex(objSettings.Items[i]->hIRow);
			int iListIndexJ=m_TaskList.GetListIndex(objSettings.Items[j]->hIRow);
			if(iListIndexI>iListIndexJ){
				HIROW hTmp=objSettings.Items[i]->hIRow;
				objSettings.Items[i]->hIRow=objSettings.Items[j]->hIRow;
				objSettings.Items[j]->hIRow=hTmp;
			}
			objSettings.Items[i]->iListIndex=m_TaskList.GetListIndex(objSettings.Items[i]->hIRow);
			objSettings.Items[j]->iListIndex=m_TaskList.GetListIndex(objSettings.Items[j]->hIRow);
			objSettings.Items[i]->ItemChanged();
			objSettings.Items[j]->ItemChanged();
		}
	}
	LeaveCriticalSection(&objSettings.bTreeNeedUpdateLock);
	RefreshTaskList();
}

void AppMainDlg::OnColumnclickTasklist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iSubItem!=abs(objSettings.iSortColumn)-1){
		if(objSettings.iSortColumn>0){
			objSettings.iSortColumn=pNMListView->iSubItem+1;
		}else{
			objSettings.iSortColumn=-(pNMListView->iSubItem+1);
		}
	}else{
		objSettings.iSortColumn=-objSettings.iSortColumn;
	}
	SortList(objSettings.iSortColumn);
	*pResult = 0;
}

void AppMainDlg::OnNewNote()
{
	OnSystrayNewnote();
}

void AppMainDlg::OnNewTodo()
{
	NewTaskPath(objSettings.sLastNewToDoPath,objSettings.bAutoCreateTodoFromMW?2:0);
}

void AppMainDlg::OnNewReminder()
{
	OnSystrayNewreminder();
}

void AppMainDlg::OnNewMessage()
{
	OnSystrayNewmessage();
}

void AppMainDlg::OnAddrbook()
{
	OnSystrayAddressbook();
}

void AppMainDlg::OnOptionsBt() 
{
	OnSystrayOptions();
}

void AppMainDlg::OnSaveallBt()
{
	dwAutoSaveCounter=GetTickCount();
	objSettings.SaveAll();
}

void AppMainDlg::OnFolderAddnote() 
{
	NewNotePath(sLastSelectedItemPath);
	sLastSelectedItemPath="";
}

void AppMainDlg::OnFolderAddreminder() 
{
	NewReminderPath(sLastSelectedItemPath);
	sLastSelectedItemPath="";
}

void AppMainDlg::OnFolderAddtask() 
{
	NewTaskPath(sLastSelectedItemPath);
	sLastSelectedItemPath="";
}

void AppMainDlg::OnSystrayNewtask()
{
	NewTaskPath(objSettings.sLastNewToDoPath);
}

void AppMainDlg::NewTaskPath(const char* szPath, int iType)
{
	OnCreateNewToDo(WPARAM(szPath),iType);
	return;
}

void AppMainDlg::OnSystrayNewmessage()
{
	if(dlgAddBook){
		dlgAddBook->OpenNoteOnPerson(-1);
		return;
	}
	CNoteSettings* pNoteSets=new CNoteSettings(objSettings.MessageNoteSettings);
	pNoteSets->iIconMode=0;
	OpenNote(pNoteSets);
}

void AppMainDlg::OnSystrayNewnote()
{
	NewNotePath();
	return;
}

void AppMainDlg::NewNotePath(const char* szPath,BOOL bFromCursor)
{
	ClanchimDengi();
	CNoteSettings* pNewNoteSets=new CNoteSettings(objSettings.NoteSettings, objSettings.sNewNote_TitleM, objSettings.sNewNote_BodyM);
	if(szPath && strlen(szPath)>0){
		pNewNoteSets->SetPath(szPath);
	}
	if(bFromCursor){
		SetNotePosToCursor(pNewNoteSets);
	}
	OpenNote(pNewNoteSets);
	return;
}

void AppMainDlg::OnSystrayNewreminder()
{
	NewReminderPath(NULL);
	return;
}

void AppMainDlg::NewReminderPath(const char* szPath)
{
	ClanchimDengi();
	Reminder* rem=new Reminder();
	rem->ActionUse[1]=TRUE;
	CString sTitle=objSettings.sNewRemd_TitleM;
	SubstMyVariables(sTitle);
	rem->SetTitle(sTitle);
	CString sBody=objSettings.sNewRemd_BodyM+REGNAG_TEXT;
	SubstMyVariables(sBody);
	rem->ActionPars[1]=sBody;
	if(szPath && strlen(szPath)>0){
		rem->SetPath(szPath);
	}
	objSettings.m_Reminders.Add(rem);
	if(rem){
		rem->CreateRemDialog(TRUE);
	}
	return;
}

//0-удаление
//1-модификация
//2-создание
//3-смена статуса
//4-переименовать
//5-активация
void AppMainDlg::OperationUnderCursor(int iOperation, CItem* item)
{
	static long lLock=0;
	if(lLock>0){// Процесс уже идет
		return;
	}
	SimpleTracker Track(lLock);
	// Смотрим что надо делать
	if(!item){
		item=GetSelectedRow();
	}
	if(!item){
		Alert(_l("Select item first")+"!");
		return;
	}
	CString sID=item->GetID();
	if(iOperation==4){
		m_TaskList.SelectIRow(item->hIRow,FALSE);
		OnFilelinkmenuRename();
		return;
	}
	switch(item->GetType())
	{
	case 0:
		if(iOperation==0){
			OnFolderDelete();
		}else if(iOperation==1){
			Renamefolder(item->hIRow);
		}else if(iOperation==2){
			item->SetState(0);// Expand on attach
			if(sLastSelectedItemPath.Find(TODO_PATH)!=-1 || sID==TODO_PATH){
				OnFolderAddtask();
			}
			if(sLastSelectedItemPath.Find(NOTE_PATH)!=-1 || sID==NOTE_PATH){
				OnFolderAddnote();
			}
			if(sLastSelectedItemPath.Find(REMD_PATH)!=-1 || sID==REMD_PATH){
				OnFolderAddreminder();
			}
			if(sLastSelectedItemPath.Find(MESS_PATH)!=-1 || sID==MESS_PATH){
				OnSystrayNewmessage();
			}
		}else if(iOperation==3){
			if(item->hIRow){
				if(m_TaskList.IsCollapsed(item->hIRow)){
					m_TaskList.Expand(item->hIRow);
				}else{
					m_TaskList.Collapse(item->hIRow);
				}
			}
		}
		break;
	case 1:
		if(iOperation==0){
			OnDeleteTask();
		}else if(iOperation==1){
			Modifytodo(item);
		}else if(iOperation==3){
			item->SetState(!item->GetState());
		}
		break;
	case 2:
		{
			CDLG_Note* note=objSettings.m_Notes.FindNoteByID(item->GetID());
			if(note){
				if(iOperation==0){
					note->SendMessage(DELNOTE,0,0);
				}else{
					note->ActivateThisNote();
				}
			}
		}
		break;
	case 3:
		if(iOperation==0){
			OnRemindermenuDeletereminder();
		}else if(iOperation==1){
			ModifyReminder(item->GetID());
		}else if(iOperation==3){
			item->SetState(!item->GetState());
		}
		break;
	case 4:
		if(iOperation==0){
			DeleteFileLink();
		}else if(iOperation==1){
			ActivateFileLink(item);
		}else if(iOperation==5){
			OnFilelinkmenuRename(item->hIRow,1);
		}else if(iOperation==2){
			OnSystrayLink();
		}
		break;
	}
}

CItemFileLink* AppMainDlg::AddFileLink(const char* szFilePath, const char* szPath,const char* szTitle)
{
	CItemFileLink* pItem=new CItemFileLink(szFilePath);
	if(szPath && strlen(szPath)>0){
		pItem->SetPath(szPath);
	}
	pItem->Attach();
	if(szTitle){
		pItem->SetTitle(szTitle);
	}
	InsertOrRefreshItem(pItem);
	m_TaskList.EnsureVisible(pItem->hIRow,0,TRUE);
	SaveLinks();
	return pItem;
}

void AppMainDlg::ActivateFileLink(CItem* item)
{
	static long lLock=0;
	if(lLock>0){// Процесс уже идет
		return;
	}
	SimpleTracker Track(lLock);
	// Активируем
	CItem* pItem=item;
	if(!item){
		pItem=GetSelectedRow();
	}
	if(pItem && pItem->GetType()==4){
		CString sAll=((CItemFileLink*)pItem)->sFilePath;
		sAll.TrimLeft();
		sAll.TrimRight();
		sAll.Replace("\r\n","|");
		sAll.Replace("\n","|");
		while(sAll.Replace("||","|")){
			// Ничего не делаем...
		}
		CString sAction;
		while(sAll!="" && (sAction=sAll.SpanExcluding("|"))!=""){
			CString sOrgiAction=sAction;
			sAll.Replace(sOrgiAction,"");
			sAll.TrimLeft();
			sAll.TrimRight();
			sAll.TrimLeft("|");
			sAll.TrimRight("|");
			WNSubstMyVariables(sAction,"web-flnk");
			CString sCommand, sParameter;
			ParseForShellExecute(sAction, sCommand, sParameter);
			int iRes=int(::ShellExecute(this->GetSafeHwnd(),"open",sCommand, sParameter, NULL, SW_SHOWNORMAL));
			if(iRes<=32){
				AsyncPlaySound(ERROR_SOUND);
#ifdef _DEBUG
				Alert(sAction+":\n"+_l("Failed to activate link")+"! Error:"+Format("%lu",iRes),_l("Link error"));
#else
				Alert(sAction+":\n"+_l("Failed to activate link")+"!",_l("Link error"));
#endif
			}
		}
	}
	return;
}

void AppMainDlg::DeleteFileLink()
{
	CItem* itemUnder=GetSelectedRow();
	if(itemUnder){
		CItem::DeleteItem(itemUnder->GetID());
	}
	SaveLinks();
}

void AppMainDlg::OnFilelinkmenuActivate()
{
	ActivateFileLink();
}

void AppMainDlg::OnFilelinkmenuDel() 
{
	DeleteFileLink();
}

void AppMainDlg::OnFilelinkmenuRename(HIROW hSelIRow,int iSelIItem)
{
	dwLastEditedLastType=m_TaskList.GetIItemControlType(hSelIRow,1);
	DWORD dwNewType=dwLastEditedLastType|FLC_EDIT;
	m_TaskList.SetIItemControlType(hSelIRow,1,dwNewType);
	m_TaskList.SelectAndEditIItem(hSelIRow,1);
}

void AppMainDlg::OnFilelinkmenuRename()
{
	HIROW hSelIRow=m_TaskList.GetSelectedIRow();
	int iSelIItem=m_TaskList.GetSelectedIItem();
	if(iSelIItem!=1){
		return;
	}
	OnFilelinkmenuRename(hSelIRow,iSelIItem);
}

void AppMainDlg::OnTodoHideshowdone() 
{
	objSettings.bShowDone=!objSettings.bShowDone;
	ReattachAccordingFilter();
}

void AppMainDlg::OnRemindermenuShowhideinactive() 
{
	objSettings.bShowNonActiveReminders=!objSettings.bShowNonActiveReminders;
	ReattachAccordingFilter();
}

void AppMainDlg::OnWndontop()
{
	bPinnedUp=!bPinnedUp;
	SetWindowPinState();
	if(objSettings.APToDoList){
		if(!bPinnedUp){
			StartMouseTracker();
		}else{
			StopMouseTracker();
		}
	}
}

void AppMainDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

int AppMainDlg::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	if(!MouseInWndRect(&m_BtOnTop)){
		bPinnedUp=TRUE;
		SetWindowPinState();
		if(objSettings.APToDoList){
			StopMouseTracker();
		}
	}
	return CDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void AppMainDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);//SW_PARENTOPENING
}

int AppMainDlg::SetTrayTooltip(int* iLastMessageNum)
{
	if(iRunStatus!=1){
		return 0;
	}
	CString sText;
	int iUnread=0;
	int iCount=objSettings.m_Notes.GetMessageCount(TRUE,&iUnread,NULL,iLastMessageNum);
	if(objSettings.objMessageWrapper->isAnyProtocolEnabled()){
		sText=" (";
		if(iCount>0){
			sText+=_l("Incoming");
			sText+=Format(": %i, ",iCount);
			sText+=_l("Unread");
			sText+=Format(": %i",iUnread);
		}else{
			sText+=_l("no incoming messages");
		}
		sText+=")";
	}
	if(!objSettings.bShowActiveTDInTray){
		objSettings.sTrayTooltip=objSettings.sMainWindowTitle;
	}
	if(dlgAddBook){
		objSettings.sMCTrayTooltip=_l("Message center")+"-"+objSettings.sProtocolNB_Alias+sText;
		dlgAddBook->m_STray.SetTooltipText(objSettings.sMCTrayTooltip);
		dlgAddBook->SetWindowText(objSettings.sMCTrayTooltip);
		if(objSettings.lShowMsgInAddrbookIcon>0){
			dlgAddBook->SetWindowIcon();
		}
		//
		m_STray.SetTooltipText(objSettings.sTrayTooltip);
	}else{
		m_STray.SetTooltipText(objSettings.sTrayTooltip+sText);
	}
	return iUnread;
}

void AppMainDlg::ShowBaloon(const char* szTitle, const char* szText)
{
	if(dlgAddBook && dlgAddBook->m_STray.Visible()){
		dlgAddBook->m_STray.PopupBaloonTooltip(szTitle,szText);
	}else{
		m_STray.PopupBaloonTooltip(szTitle,szText);
	}
}

void AppMainDlg::UpdateBaloonTooltip(int iIncMessages, int iLastMessageNum, BOOL bShowTooltip)
{
	if(objSettings.lUseBloonOnNewMsg){
		//Сбрасываем
		ShowBaloon("","");
		//Ставим если нужно
		if(iIncMessages>0 && bShowTooltip){
			CString sBTitle,sBText;
			sBTitle=_l("Incoming message");
			sBText=objSettings.sTrayTooltip;
			CDLG_Note* pNote=objSettings.m_Notes.GetNote(iLastMessageNum);
			if(pNote){
				sBTitle=pNote->Sets.GetTitle();
				sBText=TrimMessage(pNote->Sets.sText);
			}
			if(dlgAddBook && dlgAddBook->m_STray.Visible()){
				dlgAddBook->m_STray.PopupBaloonTooltip(sBTitle,sBText);
			}else{
				m_STray.PopupBaloonTooltip(sBTitle,sBText);
			}
		}
	}
}

void AppMainDlg::StopAnimation(int ForPerson)
{
	int iLastMessageNum=-1;
	int iIncMessages=SetTrayTooltip(&iLastMessageNum);
	UpdateBaloonTooltip(iIncMessages,iLastMessageNum,0);
	if(iIncMessages==0){
		if(dlgAddBook && dlgAddBook->m_STray.Visible()){
			dlgAddBook->m_STray.StopAnimation();
			dlgAddBook->SetWindowIcon();
		}
		m_STray.StopAnimation();
		SetWindowIcon();
	}
	if(dlgAddBook && ForPerson>=0){
		dlgAddBook->RefreshOnLineStatusIcon(ForPerson);
	}
}

void AppMainDlg::StartAnimation(int ForPerson)
{
	int iLastMessageNum=-1;
	int iIncMessages=SetTrayTooltip(&iLastMessageNum);
	UpdateBaloonTooltip(iIncMessages,iLastMessageNum,1);
	if(iIncMessages>0){
		//
		if(objSettings.bOnNewMessage!=1){
			if(dlgAddBook && dlgAddBook->m_STray.Visible()){
				dlgAddBook->m_STray.Animate(500,(objSettings.bOnNewMessage==0?-1:5));
			}else{
				m_STray.Animate(500,(objSettings.bOnNewMessage==0?-1:5));
			}
		}
	}
	if(dlgAddBook && ForPerson>=0){
		dlgAddBook->RefreshOnLineStatusIcon(ForPerson);
	}
}

void AppMainDlg::PostNcDestroy() 
{
	pMainDialogWindow=NULL;
	CDialog::PostNcDestroy();
	delete this;
}

void AppMainDlg::OnTodoSetasnotcurrent() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		objSettings.m_Tasks.TaskLeave(item->GetID());
	}
}

void AppMainDlg::OnHotmenuPopupmainwindow() 
{
	if(IsEmbedded()){
		return;
	}
	if(IsWindowVisible()==FALSE || (IsWindowVisible()==TRUE && GetActiveWindow()->GetSafeHwnd()!=this->GetSafeHwnd())){
		CRect rect;
		GetWindowRect(&rect);
		APPBARDATA pData;
		pData.cbSize=sizeof(pData);
		memset(&pData,0,sizeof(pData));
		if((SHAppBarMessage(ABM_GETSTATE,&pData) & ABS_AUTOHIDE)==0){//Auto hide is Off
			POINT p;
			GetCursorPos(&p);
			rect.OffsetRect(-rect.left,-rect.top);
			rect.OffsetRect(p);
		}
		Sizer.TestOnScreenOut(rect,objSettings.iStickPix,1,1);
		SetWindowPos(&wndTopMost,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
		Sizer.ApplyStickness();
		if(objSettings.APToDoList){
			StartMouseTracker();
			bPinnedUp=FALSE;
		}
		SetWindowPinState(1);
	}
}

// bFoldersOnly==0 - все
// bFoldersOnly==1 - только папки непустые
// bFoldersOnly==2 - только папки, пустые тоже
int AppMainDlg::CreateMenuTree(CMenu& menu,const char* szFirstID, BOOL bFoldersOnly, DWORD dwDepth, DWORD dwTypeMask)
{
	// Берем элемент...
	int iRes=0;
	CItem* pItem=CItem::GetItemByID(szFirstID);
	HIROW hRow=NULL;
	if(pItem){
		if(bFoldersOnly){
			AddMenuString(&menu,WM_USER+SWITCH_TO+pItem->index,".."/*pItem->GetTitle(DEF_MENUTITLELEN)*/,_bmp().Get(_IL(pItem->GetIconNum())));
		}
		// Берем детей...
		hRow=pItem->hIRow;
		if(hRow){
			hRow=m_TaskList.GetChildIRowAt(hRow,0);
			if(hRow){
				DWORD dwChildIndex=m_TaskList.GetIRowData(hRow);
				pItem=CItem::GetItemByIndex(dwChildIndex);
			}
		}
	}else{
		hRow=m_TaskList.GetIRowFromIndex(0);
		DWORD dwIndex=m_TaskList.GetIRowData(hRow);
		pItem=CItem::GetItemByIndex(dwIndex);
	}
	// Вытаскиваем...
	while(hRow && pItem){
		DWORD dwChilds=m_TaskList.GetChildIRowCount(hRow);
		if(dwChilds==0 || m_TaskList.GetDeep(hRow)>=int(dwDepth)){
			BOOL bNeedToAdd=TRUE;
			if(pItem->GetType()==0){
				if(dwChilds==0 && bFoldersOnly!=2){
					bNeedToAdd=FALSE;
				}
			}else{
				if(bFoldersOnly){
					bNeedToAdd=FALSE;
				}
			}
			if(dwTypeMask!=0 && bNeedToAdd){
				if(((dwTypeMask & TYPEMASK_TODO) && pItem->GetType()!=1)
					&& ((dwTypeMask & TYPEMASK_REMD) && pItem->GetType()!=3)){
					bNeedToAdd=FALSE;
				}
			}
			if(bNeedToAdd){
				AddMenuString(&menu,WM_USER+SWITCH_TO+pItem->index,pItem->GetTitle(DEF_MENUTITLELEN),_bmp().Get(_IL(pItem->GetIconNum())));
				iRes++;
			}
		}else{
			HIROW hChild=m_TaskList.GetChildIRowAt(hRow,0);
			if(hChild){
				CMenu subMenu;
				subMenu.CreatePopupMenu();
				CreateMenuTree(subMenu,pItem->GetID(), bFoldersOnly, dwDepth, dwTypeMask);
				AddMenuSubmenu(&menu,&subMenu,pItem->GetTitle(DEF_MENUTITLELEN),_bmp().Get(_IL(pItem->GetIconNum())));
				iRes++;
			}
		}
		hRow=m_TaskList.GetNextIRow(hRow);
		if(hRow){
			DWORD dwIndex=m_TaskList.GetIRowData(hRow);
			pItem=CItem::GetItemByIndex(dwIndex);
		}
	}
	return iRes;
}

void AppMainDlg::OnFolderSetastrayfolder() 
{
	if(objSettings.iLikStatus<=1){
		Alert(_l(UNREG_FUNC_TEXT),_l(UNREG_FUNC_TITLE));
		return;
	}
	CItem* item=GetSelectedRow();//CItem::GetItemByPath(sLastSelectedItemPath);
	if(item){
		objSettings.sRootMenuId=item->GetID();
	}
	sLastSelectedItemPath="";
}

void AppMainDlg::OnSystraySwitchmenu() 
{
	objSettings.sRootMenuId="";
}

afx_msg LRESULT AppMainDlg::OnFireOptions(WPARAM wParam, LPARAM lParam)
{
	char* name=(char*)lParam;
	objSettings.OpenOptionsDialog(_l(name), this);
	free (name);
	return 0;
}


afx_msg LRESULT AppMainDlg::OnFireAlert(WPARAM wParam, LPARAM lParam)
{
	InfoWndStruct* info=(InfoWndStruct*)lParam;
	if(info->bAsModal){
		InfoWnd infoWnd(info,info->pParent?info->pParent:this);
		return infoWnd.DoModal();
	}else{
		InfoWnd* infoWnd=new InfoWnd(info);
		infoWnd->Create(InfoWnd::IDD,this);
		return LRESULT(infoWnd);
	}
}

void AppMainDlg::SetColorStyle() 
{
	// Прозрачность
	static BOOL bWasSemiTrans=0;
	if(objSettings.objSchemes.getAlpha(objSettings.iMainStyle)==100){
		m_Slider.SetPos(0);
		if(bWasSemiTrans){
			SetWndAlpha(this->m_hWnd, 100, TRUE);
		}else{
			SetLayeredWindowStyle(this->m_hWnd,FALSE);
		}
	}else{
		bWasSemiTrans=1;
		m_Slider.SetPos(100-objSettings.objSchemes.getAlpha(objSettings.iMainStyle));
		SetLayeredWindowStyle(this->m_hWnd,TRUE);
		SetWndAlpha(this->m_hWnd, objSettings.objSchemes.getAlpha(objSettings.iMainStyle),TRUE);
	}
	DWORD cTXColor=objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	DWORD cBGColor=objSettings.objSchemes.getBgColor(objSettings.iMainStyle);
	m_TaskList.m_crIItemText=cTXColor;
	m_TaskList.m_crIRowBackground=cBGColor;
	m_TaskList.SetBkColor(cBGColor);
	m_TaskList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);//objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	m_TaskList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);//objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	m_TaskList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);//objSettings.objSchemes.getBgColor(objSettings.iMainStyle);
	// Шрифт
	objSettings.objSchemes.getFont(objSettings.iMainStyle)->GetLogFont(&logfont);
	deleteFont(font);
	font=new CFont();
	font->CreateFontIndirect(&logfont);
	m_TaskList.SetFont(font,FALSE);
	m_FEdit.SetFont(font,FALSE);
	// Цвет текста...
	m_FEdit.SetColor(cTXColor,cBGColor);
	return;
}

void AppMainDlg::OpenMsgLogOnNick(const char* szNick)
{
	objLog.FlushAllToDisk();
	CString sLogF=objLog.GetLogFileName(szNick)+DEFAULT_LOGEXT;
	if(isFileExist(sLogF)){
		CString sFileName="\"";
		sFileName+=sLogF;
		sFileName+="\"";
		ShellExecute(this->GetSafeHwnd(),NULL,"notepad.exe",sFileName,NULL,SW_SHOWNORMAL);
	}else{
		AfxMessageBox(_l("No history available"));
	}
}

void AppMainDlg::Renamefolder(HIROW hRow)
{
	if(hRow){
		dwLastEditedLastType=m_TaskList.GetIItemControlType(hRow,1);
		m_TaskList.SetIItemControlType(hRow,1,FLC_EDIT);
		m_TaskList.SelectAndEditIItem(hRow,1);
	}
}

void AppMainDlg::OnFolderRenamefolder()
{
	HIROW hSelIRow=m_TaskList.GetSelectedIRow();
	int iSelIItem=m_TaskList.GetSelectedIItem();
	if(iSelIItem!=1){
		return;
	}
	Renamefolder(hSelIRow);
}

void AppMainDlg::OnSystrayAbout() 
{
	SimpleTracker Track(objSettings.bDoModalInProcess);
	CDLG_About dlg(this);
	dlg.DoModal();
}

void AppMainDlg::OnSystrayRegister()
{
	objSettings.OpenOptionsDialog(_l(REGISTRATION_TOPIC), this);
}

void AppMainDlg::OnTodoSaveToFile()
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		CString sText=item->GetTextContent();
		SimpleTracker Track2(objSettings.bDoModalInProcess);
		DWORD dwDataSize=strlen(sText);
		HGLOBAL hGlobal=::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, dwDataSize+1);
		if (hGlobal == NULL){
			return;
		};
		char* sFileContent = (char*)hGlobal;
		strcpy(sFileContent,sText);
		CString sSavePath=item->GetTitle()+".txt";
		BOOL bSaveRes=SaveFileFromHGlob(sSavePath,objSettings.sLastSaveNoteTextDir,hGlobal,dwDataSize,TRUE,0,this);
		VERIFY(::GlobalFree(hGlobal)==NULL);
		hGlobal=NULL;
	}
}

void AppMainDlg::OnTodoSettings()
{
	objSettings.OpenOptionsDialog(_l(TODOSETS_TOPIC),this);
}

void AppMainDlg::OnTodoRemindabout() 
{
	static long lToDoReminderDlgActive=0;
	if(lToDoReminderDlgActive){
		return;
	}
	SimpleTracker Track(lToDoReminderDlgActive);
	//
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	Reminder* rem=AddReminder(24*60,item->GetTitle());
	if(rem){
		rem->CreateRemDialog();
	}
}

void AppMainDlg::OnSystrayHelp()
{
	ShowHelp("Overview");
}

afx_msg LRESULT AppMainDlg::ONWM_WIREKEYS(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void TranslateTemplateAliases(CString& sBodyText)
{
	int iFrom=0;
	COleDateTime dt=COleDateTime::GetCurrentTime();
	// Список напоминаний
	if(sBodyText.Find("$=<DECLARE_REMINDERS>")!=-1){
		CString sReminders;
		// Инициализируем
		sReminders+="R_Title= new Array();";
		sReminders+="R_Active= new Array();";
		sReminders+="R_DateTime= new Array();";
		sReminders+="R_DayType= new Array();";
		sReminders+="R_WDayType= new Array();";
		sReminders+="R_MonthType= new Array();";
		sReminders+="R_YearType= new Array();";
		sReminders+="R_ActuationTime= new Array();";
		sReminders+="\n";
		int k=0;
		// Выводим все напоминания
		for(int i=0;i<objSettings.m_Reminders.m_aReminders.GetSize();i++){
			Reminder* rem=objSettings.m_Reminders.m_aReminders[i];
			if(!rem){
				continue;
			}
			if(rem->bTimeType==1){// Слишком частое напоминаниае
				continue;
			}
			if(strstr(rem->GetPath(),REMS_PATH)!=NULL){// Только в подпапке с напоминаниями
				continue;
			}
			//if(rem->IsStrictReminder() && rem->dwMon!=dt.GetMonth()){
			//	continue;
			//}
			CString sTitle=rem->GetTitle();
			CDataXMLSaver::Str2Xml(sTitle,XMLJScriptConv);
			sReminders+=Format("R_Title[%i]='%s';",k,sTitle);
			sReminders+=Format("R_DateTime[%i]=new Date(%i,%i,%i,%i,%i);",k,rem->dwYear,rem->dwMon-1,rem->dwDay,rem->dwTime.GetHour(),rem->dwTime.GetMinute(),rem->dwTime.GetSecond());
			if(!rem->CalulateClosestTime(dt) || dt.GetStatus()!=COleDateTime::valid){
				sReminders+=Format("R_ActuationTime[%i]='';",k);
			}else{
				sReminders+=Format("R_ActuationTime[%i]=new Date(%i,%i,%i,%i,%i);",k,dt.GetYear(),dt.GetMonth()-1,dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
			}
			sReminders+=Format("R_Active[%i]=%i;",k,rem->GetState());
			sReminders+=Format("R_DayType[%i]=%i;",k,rem->bDayType);
			sReminders+=Format("R_WDayType[%i]=%i;",k,rem->dwWDay);
			sReminders+=Format("R_MonthType[%i]=%i;",k,rem->bMonType);
			sReminders+=Format("R_YearType[%i]=%i;",k,rem->bYearType);
			k++;
			sReminders+="\n";
		}
		//
		sBodyText.Replace("$=<DECLARE_REMINDERS>",sReminders);
	}
	// Список задач
	if(sBodyText.Find("$=<DECLARE_TODOS>")!=-1){
		CString sToDos;
		// Инициализируем
		sToDos+="T_Title= new Array();";
		sToDos+="T_DateTime= new Array();";
		sToDos+="T_State= new Array();";
		sToDos+="T_Prior= new Array();";
		sToDos+="\n";
		int k=0;
		// Выводим все напоминания
		for(int i=0;i<objSettings.m_Tasks.GetSize();i++){
			Task* todo=objSettings.m_Tasks.GetTask(i);
			if(!todo){
				continue;
			}
			if(strstr(todo->GetPath(),TODO_PATH)!=NULL){// Только в подпапке с ToDo
				continue;
			}
			CString sTitle=todo->GetTitle();
			CDataXMLSaver::Str2Xml(sTitle,XMLJScriptConv);
			sToDos+=Format("T_Title[%i]='%s';",k,sTitle);
			COleDateTime dt=todo->GetCrTime();
			sToDos+=Format("T_DateTime[%i]=new Date(%i,%i,%i,%i,%i);",k,dt.GetYear(),dt.GetMonth()-1,dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
			sToDos+=Format("T_State[%i]=%i;",k,todo->GetState());
			sToDos+=Format("T_Prior[%i]=%i;",k,todo->GetPriority());
			k++;
			sToDos+="\n";
		}
		//
		sBodyText.Replace("$=<DECLARE_TODOS>",sToDos);
	}
}

afx_msg LRESULT AppMainDlg::ONWM_THIS(WPARAM wParam, LPARAM lParam)
{
	if(wParam==99 && lParam==99){
		PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN,0);
		return 1;
	}
	return 0;
}

afx_msg LRESULT AppMainDlg::ONWM_WIRECHANGER(WPARAM wParam, LPARAM lParam)
{
	if(wParam==2){
		// Нужно обработать файл с обоями
		CRegKey key;
		key.Open(HKEY_CURRENT_USER, PROG_REGKEYWP);
		if(key!=NULL){
			DWORD dwType=REG_SZ;
			char szPathValue[256]="";
			DWORD dwCount=sizeof(szPathValue);
			if(RegQueryValueEx(key.m_hKey,REGWPPATH,NULL, &dwType,(LPBYTE)szPathValue, &dwCount)== ERROR_SUCCESS && strlen(szPathValue)>0){
				CString sFileContent;
				if(!ReadFile(szPathValue,sFileContent)){
					return 0;
				}
				TranslateTemplateAliases(sFileContent);
				if(!SaveFile(szPathValue,sFileContent)){
					return 0;
				}
			}
		}
	}
	return 1;
}

void AppMainDlg::OnSystrayOpenwirekeys()
{
	// Говорим что надо активироваться WireKeys-ам
	DWORD dwTarget=BSM_APPLICATIONS;
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(2), LPARAM(0));
}

void AppMainDlg::OnSystrayWchanger() 
{
	// Говорим что надо активироваться WireChanger-у
	DWORD dwTarget=BSM_APPLICATIONS;
	BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &dwTarget, theApp.iWM_THIS, WPARAM(3), LPARAM(0));
}

HANDLEINTHREAD(AppMainDlg,OnSystrayDonate)
{
	if(GetApplicationLang()==1){
		(int)ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?who=WireNote&target=donation-ru",NULL,NULL,SW_SHOWNORMAL);
	}else{
		(int)ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?who=WireNote&target=donation",NULL,NULL,SW_SHOWNORMAL);
	}
	return 0;
}

void AppMainDlg::OnSystraySendasuggestion() 
{
	SendSuggestion(PROGNAME,PROG_VERSION);
}

void AppMainDlg::OnNotemenuNewstyle() 
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		CString sID=item->GetID();
		CallStylesDialogWnd(this,item->GetColorCode()-1);
		item=CItem::GetItemByID(sID);
		if(item){
			// Так как итем может умереть за это время...
			item->SetColorCode(objSettings.iLastSchemeUsed+1);
		}
	}
}

void AppMainDlg::SetWindowIcon()
{
	if(IsEmbedded()){
		m_STray.HideIcon();
		return;
	}
	HICON hIcon=::AfxGetApp()->LoadIcon(theApp.aAppIcons[objSettings.iTrayIcon]);
	COleDateTime dt=COleDateTime::GetCurrentTime();
	HICON hIcoText=hIcon;
	if(objSettings.lShowDateInTrayIcon>0){
		CString sText=Format("%02i",dt.GetDay());
		if(objSettings.lShowDateInTrayIcon==2){
			sText=GetWeekDay(dt.GetDayOfWeek(),TRUE);
		}
		hIcoText=PutTextInIcon(hIcon,sText,objSettings.objSchemes.getFont(objSettings.iMainStyle),RGB(250,250,250));
		ClearIcon(hIcon);
	}
	m_STray.SetIcon(hIcoText,TRUE);
	SetIcon(hIcoText,TRUE);
	SetIcon(hIcoText,FALSE);
	theApp.m_pFakeWnd->SetIcon(hIcoText,TRUE);
	theApp.m_pFakeWnd->SetIcon(hIcoText,FALSE);
	// Показывать ли ее или скрывать
	if(!objSettings.lTrayIconsMaskO){
		m_STray.HideIcon();
	}else{
		m_STray.ShowIcon();
	}
}

BOOL AppMainDlg::IsMainMessage(WPARAM wParam)
{
	if(wParam>=WM_USER+SWITCH_TO && wParam<(DWORD)(WM_USER+SWITCH_TO+USERMSG_STEP)){
		return TRUE;
	}
	if(wParam>=WM_USER+SHOW_ANTISPAM && wParam<(DWORD)(WM_USER+SHOW_ANTISPAM+USERMSG_STEP)){
		return TRUE;
	}
	for(int i=0;i<MMenuIDLog().GetSize();i++){
		if(MMenuIDLog()[i]==DWORD(wParam)){
			return TRUE;
		}
	}
	return FALSE;
}

void AppMainDlg::OnFindBt()
{
	SimpleTracker Track(objSettings.bDoModalInProcess);
	if(objSettings.iLikStatus<=1){
		Alert(_l(UNREG_FUNC_TEXT),_l(UNREG_FUNC_TITLE));
		return;
	}
	CDLG_Find dlg(NULL);
	if(!dlg.bAlreadyExist){
		dlg.DoModal();
	}
	return;
}

void AppMainDlg::OnSystrayToDoAddOptions()
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item && item->GetType()==1){
		Task* pTask=(Task*)item;
		CDLG_NoteSettings dlg((pTask->pCorDialog!=NULL)?((CWnd*)pTask->pCorDialog):((CWnd*)this));
		dlg.pItem=(CActiItem*)item;
		dlg.DoModal();
	}
}

void AppMainDlg::OnEditLink()
{
	static long lAlread=0;
	if(lAlread){
		return;
	}
	SimpleTracker Lock(lAlread);
	CItem* pItem=GetSelectedRow();
	if(pItem && pItem->GetType()==4){
		CDlgSimpleEdit ed;
		ed.m_Edit=((CItemFileLink*)pItem)->sFilePath;
		ed.m_EditTitle=((CItemFileLink*)pItem)->GetTitle();
		ed.m_Edit.Replace("\n","\r\n");
		if(ed.DoModal()==IDOK){
			ed.m_Edit.Replace("\r\n","\n");
			((CItemFileLink*)pItem)->sFilePath=ed.m_Edit;
			((CItemFileLink*)pItem)->SetTitle(ed.m_EditTitle);
			pItem->SetModify();
			UpdateFedit(pItem);
		}
	}
}

void AppMainDlg::OnCopyLinkToClip()
{
	CItem* pItem=GetSelectedRow();
	if(pItem && pItem->GetType()==4){
		USES_CONVERSION;
		BOOL bUseGlobalMemory=0;
		SetClipboardText(A2W(((CItemFileLink*)pItem)->sFilePath),bUseGlobalMemory);
	}
}

void AppMainDlg::OnSendItemAsMessage()
{
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return;
	}
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		CNoteSettings* pNoteSets=new CNoteSettings(objSettings.MessageNoteSettings);
		pNoteSets->iIconMode=0;
		pNoteSets->sItemIDToAttachAfterStart=item->GetID();
		pNoteSets->sText=_l("This message contains attached item");
		pNoteSets->sText+=" '";
		pNoteSets->sText+=item->GetTitle();
		pNoteSets->sText+="'";
		OpenNote(pNoteSets);
	}
}

void AppMainDlg::OnRemdsOptions()
{
	objSettings.OpenOptionsDialog(_l(REMDSETS_TOPIC),this);
}

BOOL AppMainDlg::LinkFromClipboardX()
{
	CString sText=GetClipboardText();
	sText.TrimLeft();
	sText.TrimRight();
	if(sText.Find("\n")!=-1){
		return FALSE;
	}
	AddFileLink(sText);
	return TRUE;
}

void AppMainDlg::LinkFromClipboard()
{
	LinkFromClipboardX();
}

void AppMainDlg::OnBnDel()
{
	OperationUnderCursor(0,0);
}

void AppMainDlg::OnBnEdit()
{
	CItem* item=GetSelectedRow();
	if(item && item->GetType()==4){
		OperationUnderCursor(4,0);
		return;
	}
	OperationUnderCursor(1,0);
}

void AppMainDlg::OnNewBnFolder()
{
	OnFolderCreatenew();
}

void AppMainDlg::OnNewBnLink()
{
	OnSystrayLink();
}

void AppMainDlg::OnSystrayLink()
{
	CDlgSimpleEdit ed;
	ed.m_Edit=GetClipboardText(0,0);
	ed.m_EditTitle=_l("Link")+" "+GetRandomName();
	if(ed.DoModal()==IDOK){
		CItem* item=GetSelectedRow();
		CString sListPath="";
		if(!item){
			item=CItem::GetItemByID(LINK_PATH);
		}
		if(item){
			sListPath=item->GetPath();
			if(item->GetType()==0){
				sListPath=(CString)item->GetID()+"/"+sListPath;
				item->SetState(0);
			}
		}
		CString szFilePath=GetClipboardText(0,0);
		CItemFileLink* pItem=AddFileLink(szFilePath,sListPath);
		pItem->SetTitle(_l("Link")+" "+GetRandomName());
		if(pItem){
			((CItemFileLink*)pItem)->sFilePath=ed.m_Edit;
			((CItemFileLink*)pItem)->SetTitle(ed.m_EditTitle);
			pItem->SetModify();
		}
	}
}

void AppMainDlg::OnFolderCreatenew() 
{
	CItem* itemUnder=GetSelectedRow();
	if(!itemUnder){
		return;
	}
	if(itemUnder->GetType()!=0){
		itemUnder=itemUnder->GetItemParent();
	}
	if(!itemUnder || itemUnder->GetType()!=0){
		return;
	}
	CItem* pItem=new CItemFolder(objSettings.bGenerateRandomNames?(_l("New folder")+" "+GetRandomName()):_l("New folder"));
	CString sPath=(CString)itemUnder->GetID()+"/"+(CString)itemUnder->GetPath();
	pItem->SetPath(sPath);
	pItem->Attach();
	InsertOrRefreshItem(pItem);
	SaveFolders();
}

void AppMainDlg::OnSystrayIncEmails()
{
	if(objSettings.objMessageWrapper && objSettings.objMessageWrapper->pEMNet && objSettings.objMessageWrapper->pEMNet->lInCheck==0){
		SetEvent(objSettings.objMessageWrapper->pEMNet->hWaitEvent);
	}
}

void AppMainDlg::OnSystrayCBillb()
{
	if(objSettings.sFBDir!=""){
		if(GetAsyncKeyState(VK_SHIFT)<0){
			if(AfxMessageBox(_l("Do you want to re-download all latest versions from billboard")+"?",MB_YESNO)==IDYES){
				objSettings.sExcludeBillboardItems="";
				objSettings.bExclChanged=1;
			}
		}
		SetEvent(objSettings.objMessageWrapper->pFBNet->hWaitEvent);
	}
}

void AppMainDlg::OnShareItem(int iSec)
{
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		item->ShareItem(!item->IsShared(),iSec);
	}
}

void AppMainDlg::OnNotemenuFZFolder()
{
	CString sFFF=GetFBFZ();
	if(sFFF!=""){
		::ShellExecute(NULL,"open",sFFF,NULL,NULL,SW_SHOWNORMAL);
	}
}

void AppMainDlg::OnSystrayCheckAllAcounts()
{
	for(int iIndex=0; iIndex<objSettings.antiSpamAccounts.GetSize();iIndex++){
		objSettings.antiSpamAccounts[iIndex].ShowAntiSpamWindow();
	}
	return;
}

void AppMainDlg::OnEform()
{
	CRect rt;
	GetDlgItem(IDC_RICHEDIT)->GetWindowRect(&rt);
	CRect rect;
	GetWindowRect(&rect);
	if(rt.Height()<10){
		rect.bottom+=objSettings.lFEditHR>0?objSettings.lFEditHR:50;
		SetWindowPos(bPinnedUp?&wndTopMost:&wndNoTopMost,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
		m_BtEform.SetBitmap(IDB_BM_EFORM);
	}else{
		objSettings.lFEditHR=rt.Height();
		rect.bottom-=rt.Height();
		SetWindowPos(bPinnedUp?&wndTopMost:&wndNoTopMost,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
		m_BtEform.SetBitmap(IDB_BM_EFORM1);
	}
	Sizer.ApplyLayout();
}

void AppMainDlg::OnMainTreeListComEx(NMHDR * pNotifyStruct, LRESULT * result)
{
	PostMessage(SETCOLW,0,0);
	*result=0;
	return;
}

void AppMainDlg::OnMainTreeListSel(NMHDR * pNotifyStruct, LRESULT * result)
{
	CItem* item=GetSelectedRow();
	if(item){
		UpdateFedit(item);
	}
	PostMessage(SETCOLW,0,0);
	*result=0;
	return;
}

void AppMainDlg::UpdateFeditIfNeeded(CItem* itemC)
{
	CItem* item=GetSelectedRow();
	if(item && itemC && strcmp(item->GetID(),itemC->GetID())==0){
		UpdateFedit(item);
	}
}

void AppMainDlg::UpdateFedit(CItem* item)
{
	if(item==0){
		return;
	}
	iIgnoreChangerFEdit=1;
	sLastFEditID=item->GetID();
	switch(item->GetType())
	{
		case 1://todo
			{
				Task* pTask=objSettings.m_Tasks.GetTask(sLastFEditID);
				if(pTask){
					SetLPText(pTask->sDsc);
				}
				break;
			}
		case 2://notes
			{
				CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(sLastFEditID);
				if(pNote){
					SetLPText(pNote->Sets.sText);
				}
				break;
			}
		case 3://reminders
			{
				Reminder* pRem=objSettings.m_Reminders.GetReminder(sLastFEditID);
				if(pRem){
					SetLPText(pRem->ActionPars[1]);
				}
				break;
			}
			break;
		case 4://filelink
			{
				CItemFileLink* pFItem=((CItemFileLink*)item);
				if(pFItem){
					SetLPText(pFItem->sFilePath);
				}
				break;
			}
			break;
		default:
			SetLPText("");
			break;
	}
	iIgnoreChangerFEdit=0;
}

void AppMainDlg::OnRclickRichedit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CItem* item=CItem::GetItemByID(sLastFEditID);
	TrackItemMenu(item);
	*pResult = 0;
}

void AppMainDlg::OnChangeRichedit()
{
	if(iIgnoreChangerFEdit || iRunStatus!=1){
		return;
	}
	if(objSettings.sTreeNeedMoveFocus!=""){
		return;
	}
	long dwType=-1;
	static BOOL sPrevTextInit=0;
	static CString sPrevText="";
	CString sNewText;
	m_FEdit.GetWindowText(sNewText);
	if(sPrevTextInit==0){
		sPrevTextInit=1;
		sPrevText=sNewText;
	}
	CItem* item=CItem::GetItemByID(sLastFEditID);
	if(item){
		dwType=item->GetType();
		item->ItemChanged();
	}
	switch(dwType)
	{
		case 1://todo
			{
				Task* pTask=objSettings.m_Tasks.GetTask(sLastFEditID);
				if(pTask){
					pTask->sDsc=sNewText;
					if(pTask->pCorDialog){
						pTask->pCorDialog->SetDscText(sNewText);
					}
				}
				break;
			}
		case 2://notes
			{
				CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(sLastFEditID);
				if(pNote){
					pNote->Sets.sText=sNewText;
					pNote->SetNoteText();
				}
				break;
			}
		case 3://reminders
			{
				Reminder* pRem=objSettings.m_Reminders.GetReminder(sLastFEditID);
				if(pRem){
					pRem->ActionPars[1]=sNewText;
					if(pRem->dlg){
						pRem->dlg->SetRemText(sNewText);
					}
				}
				break;
			}
			break;
		case 4://filelink
			{
				CItemFileLink* pFItem=((CItemFileLink*)item);
				if(pFItem){
					pFItem->sFilePath=sNewText;
				}
				break;
			}
			break;
		default:
			{
				if(sNewText==sPrevText){
					break;
				}
				Task* pTask=CreateNewTask(objSettings.sLastNewToDoPath);
				if(pTask){
					objSettings.sTreeNeedMoveFocus=pTask->GetID();
				}
			}
			break;
	}	
	sPrevText=sNewText;
}

HANDLEINTHREAD(AppMainDlg,OnSystrayReport)
{
	static long l=0;
	if(l>0){
		return 0;
	}
	SimpleTracker lc(l);
	CString sResult="<html><head><title>";
	sResult+=_l("List of all items");
	sResult+="</title></head><body>\r\n<h3>";
	sResult+=_l("List of all items");
	sResult+="</h3>";
	CString s;
	for(int index=0; index<objSettings.Items.GetSize();index++){
		CItem* pItem=objSettings.Items[index];
		if(pItem && pItem->GetType()!=0){
			sResult+="<hr>";
			sResult+="<table border=0 width=100%>";
			sResult+="<tr><td>";
			s=pItem->GetItemDsc();
			s.Replace("\n","<br>");
			sResult+=s;
			sResult+="</td></tr>\r\n";
			if(pItem->GetTextContent()!="" && pItem->GetType()!=4){
				sResult+="<tr><td>";
				s=pItem->GetTextContent();
				s.Replace("\n","<br>");
				sResult+=s;
				sResult+="</td></tr>\r\n";
			}
			sResult+="</table>\r\n";
		}
	}
	sResult+="\r\n</body></html>";
	CString sFile=getFileFullName("report.html");
	SaveFile(sFile,sResult);
	::ShellExecute(NULL,"open",sFile,NULL,NULL,SW_SHOWNORMAL);
	return 0;
}

CWnd* AfxWPGetMainWnd()
{
	return pMainDialogWindow;
}

void AppMainDlg::SetLPText(CString sText)
{
	m_FEdit.SetText(sText);
}

static long lLockLC=0;
DWORD WINAPI DoLinkTest(LPVOID p)
{
	SimpleTracker Track(lLockLC);
	char* sz=(char*)p;
	CString sLinks=sz;
	free(sz);
	HWND hStatus=0;
	if(pMainDialogWindow){
		hStatus=pMainDialogWindow->GetDlgItem(IDC_STATUS)->GetSafeHwnd();
	}
	CString sResult;
	int iBadCount=0;
	int iGudCount=0;
	CStringArray aLinks;
	ConvertComboDataToArray(sLinks,aLinks,'\n');
	for(int j=0;j<aLinks.GetSize();j++){
		iGudCount++;
		CString sLink=aLinks[j];
		if(!isFileExist(sLink)){// Не файл
			CWebWorld url;
			DWORD dwStatus=0;
			if(hStatus){
				::SetWindowText(hStatus,Format("%s: %s",_l("Link checker"),sLink));
			}
			if(!url.GetWebFile(sLink,"",&dwStatus) || dwStatus>=400){
				iBadCount++;
				sResult+=Format("\n%i. %s",iBadCount,sLink);
				if(dwStatus!=0){
					sResult+=Format(", %s: %i",_l("Error code"),dwStatus);
				}
			}
		}
	}
	if(iBadCount){
		sResult+="\n";
		sResult+=Format("%s: %i/%i",_l("Bad links"),iBadCount,iGudCount);
		sResult.TrimLeft();
		sResult.TrimRight();
		CNoteSettings* NewNote=new CNoteSettings(objSettings.NoteSettings);
		NewNote->SetTitle(_l("Link check results"));
		NewNote->sText=sResult;
		OpenNote(NewNote);
	}else{
		AfxMessageBox(_l("Link Checker: all links are valid")+"!");
	}
	return 0;
}

void AppMainDlg::OnSystrayLinkCh()
{
	if(lLockLC>0){// Процесс уже идет
		AfxMessageBox(_l("Link checker is already in use, please wait")+"!");
		return;
	}
	SimpleTracker Track(lLockLC);
	CString sLinks;
	for(int i=0;i<objSettings.Items.GetSize();i++){
		if(!objSettings.Items[i] || objSettings.Items[i]->GetType()!=4){
			continue;
		}
		sLinks+=((CItemFileLink*)objSettings.Items[i])->sFilePath;
		sLinks+="\n";
	}
	sLinks.Replace("|","\n");
	if(sLinks!=""){
		CDlgSimpleEdit ed;
		ed.iStyle=4;
		ed.m_Edit=sLinks;
		ed.m_EditTitle=_l("Click 'OK' to start check");
		ed.m_Edit.Replace("\n","\r\n");
		if(ed.DoModal()==IDOK){
			ed.m_Edit.Replace("\r\n","\n");
			sLinks=ed.m_Edit;
			sLinks.TrimLeft();
			sLinks.TrimRight();
			FORK(DoLinkTest,strdup(sLinks));
		}
	}
	return;
}

void ClanchimDengi()
{
	static BOOL bStop=0;
	if(IsEmbedded() || bStop){
		return;
	}
	if(GetApplicationLang()==1){
		if(rnd(0,100)>70){
			if(!isFileExist(GetAppFolder()+"stopsms.txt")){
				Alert(_l(SMS_TEXT),"Donation");
				bStop=1;
			}
		}
	}
}