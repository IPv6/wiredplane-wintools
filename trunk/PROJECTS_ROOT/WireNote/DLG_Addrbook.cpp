// DLG_Addrbook.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD WINAPI OnOpencomputer(LPVOID pd)
{
	long lCS=0;
	if(lCS>0){
		return 0;
	}
	int iPerson=(int) pd;
	SimpleTracker cc(lCS);
	// Открываем компутер...
	Person* pPerson=objSettings.AddrBook.aPersons[iPerson];
	if(pPerson){
		CString sNick=pPerson->GetTitle();
		CString sIP=objSettings.AddrBook.TranslateNickToIP(sNick).SpanExcluding(" ,;/:");
		if(sIP!=""){
			CString sCommand="\\\\";
			sCommand+=sIP;
			ShellExecute(0,NULL,sCommand,NULL,NULL,SW_SHOWNORMAL);
		}
	}
	return 0;
}

void FAR WINAPI OnAdrBWndLeave(MouseTrackerRuler* pRuler)
{
	if(!pRuler->bStopWatching && pMainDialogWindow && pMainDialogWindow->dlgAddBook){
		ShowWindowAsync(pMainDialogWindow->dlgAddBook->GetSafeHwnd(),SW_HIDE);
	}else{
		pRuler->bStopWatching=FALSE;
	}
}

void _stdcall OnAdrBookTrayMove()
{
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus!=1 || !pMainDialogWindow->dlgAddBook){
		return;
	}
	if(objSettings.bAutoPopupAdrBook){
		pMainDialogWindow->dlgAddBook->PopupAdrBook();
	}
}

/////////////////////////////////////////////////////////////////////////////
// DLG_Addrbook dialog
DLG_Addrbook::DLG_Addrbook(CWnd* pParent /*=NULL*/)
	: CDialog(/*DLG_Addrbook::IDD, pParent*/)
{
	//{{AFX_DATA_INIT(DLG_Addrbook)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	font=NULL;
	hDragCursor=0;
	iStartDrug=-1;
	MainRulePos=0;
	MainTracker.pWnd=this;
	MainTracker.MouseOutProc=OnAdrBWndLeave;
	MainTracker.dwTaskbarFriendly=1;
	MainTracker.bStopWatching=FALSE;
	MainTracker2.pWnd=this;
	MainTracker2.MouseOutProc=OnNoteLeave2;
	MainTracker2.dwTaskbarFriendly=0;
	MainTracker2.bStopWatching=FALSE;
	MouseTracker=NULL;
	MouseTracker2=NULL;
	iLastUnderCursorItem=-1;
}

DLG_Addrbook::~DLG_Addrbook()
{
	deleteFont(font);
	if(MouseTracker!=NULL){
		TerminateThread(MouseTracker,0);
		CloseHandle(MouseTracker);
	}
	if(MouseTracker2!=NULL){
		TerminateThread(MouseTracker2,0);
		CloseHandle(MouseTracker2);
	}
}

void DLG_Addrbook::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DLG_Addrbook)
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, ID_ADOPTIONS, m_BtOpt);
	DDX_Control(pDX, ID_QMENU, m_QMenu);
	DDX_Control(pDX, ID_REFRESHSTAT, m_RefreshStatus);
	DDX_Control(pDX, ID_MSGLOG, m_BtMsgLog);
	DDX_Control(pDX, IDC_SENDMES, m_BtSendMessage);
	DDX_Control(pDX, IDC_SENDMES2, m_BtSendMessage2);
	DDX_Control(pDX, IDC_NEWPERSON, m_BtNewPerson);
	DDX_Control(pDX, IDC_MODPERSON, m_BtModPerson);
	DDX_Control(pDX, IDC_FINDPERSON, m_BtFindPerson);
	DDX_Control(pDX, ID_DELETEPERSON, m_BtDelPerson);
	DDX_Control(pDX, IDC_TAB_DEL, m_BtTabDel);
	DDX_Control(pDX, IDC_PERSONLIST, m_PersonList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DLG_Addrbook, CDialog)
	//{{AFX_MSG_MAP(DLG_Addrbook)
	ON_WM_SYSCOMMAND()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PERSONLIST, OnColumnclickTasklist)
	ON_COMMAND(ID_FZFOLDER, OnNotemenuFZFolder)
	ON_BN_CLICKED(IDC_NEWPERSON, OnNewperson)
	ON_BN_CLICKED(ID_DELETEPERSON, OnDeleteperson)
	ON_BN_CLICKED(IDC_MODPERSON, OnModperson)
	ON_BN_CLICKED(IDC_FINDPERSON, OnFindperson)
	ON_NOTIFY(NM_DBLCLK, IDC_PERSONLIST, OnDblclkPersonlist)
	ON_BN_CLICKED(IDC_SENDMES, OnSendmes)
	ON_BN_CLICKED(IDC_SENDMES2, OnSendmes2)
	ON_BN_CLICKED(IDC_TAB_DEL, OnDelTab)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_TRAY_IMPORTAB, OnTrayImportAb)
	ON_COMMAND(ID_TRAY_EXPORTAB, OnTrayExportAb)
	ON_COMMAND(ID_TRAY_EXITWINDOW, OnTrayExitwindow)
	ON_COMMAND(ID_TRAY_FINDONLINEUSERS, OnTrayFindUsers)
	ON_COMMAND(ID_TRAY_STATUSAWAY, OnTrayStatusaway)
	ON_COMMAND(ID_TRAY_STATUSBUSY, OnTrayStatusbusy)
	ON_COMMAND(ID_TRAY_STATUSDND, OnTrayStatusdnd)
	ON_COMMAND(ID_TRAY_STATUSONLINE, OnTrayStatusonline)
	ON_COMMAND(ID_TRAY_AUTOANSWERING, OnTrayAutoanswering)
	ON_COMMAND(ID_TRAY_RECONNECT, OnTrayReconnect)
	ON_COMMAND(ID_TRAY_NEWMESSAGE, OnTrayNewmessage)
	ON_BN_CLICKED(ID_MSGLOG, OnMsglog)
	ON_BN_CLICKED(ID_REFRESHSTAT, OnRefreshstat)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_TRAY_OPENWINDOW, OnTrayOpenwindow)
	ON_COMMAND(ID_TRAY_REFRESHSTATUS, OnTrayRefreshstatus)
	ON_COMMAND(ID_TRAY_DELPERSON, OnTrayDelperson)
	ON_COMMAND(ID_TRAY_MESSAGELOG, OnTrayMessagelog)
	ON_COMMAND(ID_TRAY_MODPERSON, OnTrayModperson)
	ON_COMMAND(ID_TRAY_NEWMESSAGEONPERSON, OnTrayNewmessageonperson)
	ON_COMMAND(ID_TRAY_NEWPERSON, OnTrayNewperson)
	ON_COMMAND(ID_TRAY_UPDATEPERSONSTATUS, OnTrayUpdatepersonstatus)
	ON_COMMAND(ID_TRAY_OPENCOMPUTER, OnTrayOpencomputer)
	ON_COMMAND(ID_TRAY_USERMSGLOG, OnTrayUsermsglog)
	ON_COMMAND(ID_TRAY_OPTIONS, OnTrayOptions)
	ON_COMMAND(ID_SYSTRAY_AUTOPOPUPAB, OnTrayAPopup)
	ON_BN_CLICKED(ID_QMENU, OnQmenu)
	ON_BN_CLICKED(ID_ADOPTIONS, OnAdoptions)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLG_Addrbook message handlers

void DLG_Addrbook::OnNewperson() 
{
	CNewPerson dlgNewPerson(this);
	dlgNewPerson.m_iTab=objSettings.iLastViewTab;
	dlgNewPerson.DoModal();
	if(dlgNewPerson.m_Nick.GetLength()>0){
		objSettings.AddrBook.AddModPerson(dlgNewPerson.m_Nick,dlgNewPerson.m_CompName,dlgNewPerson.m_Dsc,dlgNewPerson.m_bGroup,dlgNewPerson.m_iTab);
		Refreshlist();
	}
}

int DLG_Addrbook::GetItemUnderCursor(int* iCol)
{
	int iPerson=-1;
	CPoint p;
	::GetCursorPos(&p);
	m_PersonList.ScreenToClient(&p);
	int iRow=m_PersonList.HitTestEx(p, iCol);
	if(iRow!=-1){
		HIROW hRow = m_PersonList.GetIRowFromIndex(iRow);
		if(hRow){
			iPerson = m_PersonList.GetIRowData(hRow);
		}
	}
	return iPerson;
}

int DLG_Addrbook::GetSelectedRow()
{
	HIROW hRow=m_PersonList.GetSelectedIRow();
	if(hRow==NULL){
		return -1;
	}
	return m_PersonList.GetIRowData(hRow);
}

void DLG_Addrbook::OnDeleteperson() 
{
	int iPerson=GetSelectedRow();
	if(iPerson==-1){
		Alert(_l("Select person first")+"!");
		return;
	}
	objSettings.AddrBook.DeletePerson(objSettings.AddrBook.aPersons[iPerson]->GetTitle());
	Refreshlist();
}

void _stdcall RefreshRecipientMenuByTab(CDWordArray* pTrack, CMenu* pUsersMenu, DWORD dwCommOffset, int iTab)
{
	int iOther=0;
	CMenu pAllMenu;
	pAllMenu.CreatePopupMenu();
	int iTabCounts=0;
	for(int i=0;i<objSettings.AddrBook.GetSize();i++){
		if(objSettings.AddrBook.aPersons[i]->sCompName=="" || objSettings.AddrBook.aPersons[i]->iTabNumber!=iTab){
			continue;
		}
		iTabCounts++;
		DWORD dwComID=WM_USER+dwCommOffset+i;
		AddMenuStringEx(pTrack,pUsersMenu,dwComID,TrimMessage(objSettings.AddrBook.aPersons[i]->GetTitle(),30,1),_bmp().Get(IDB_BM_PONLINE+GetStatusIcon(i)));
	}
	if(iTabCounts>0 && iTab!=0){
		AddMenuStringEx(pTrack,pUsersMenu,WM_USER+dwCommOffset+objSettings.AddrBook.GetSize()+iTab,_l("All in this menu"),_bmp().Get(IDB_BM_PGROUP));
	}
}


void _stdcall RefreshRecipientMenu(CDWordArray* pTrack, CMenu*& pUsersMenu)
{
	DWORD dwCommOffset=SEND_TO_OFFSET;
	int iOther=0;
	CMenu pAllMenu;
	pAllMenu.CreatePopupMenu();
	RefreshRecipientMenuByTab(pTrack, pUsersMenu, dwCommOffset, 0);
	if(objSettings.AddrBook.aAddrBookTabs.GetSize()>1){
		for(int i=1;i<objSettings.AddrBook.aAddrBookTabs.GetSize();i++){
			CMenu pTabMenu;
			pTabMenu.CreatePopupMenu();
			RefreshRecipientMenuByTab(pTrack, &pTabMenu, dwCommOffset, i);
			AddMenuSubmenu(pUsersMenu, &pTabMenu, objSettings.AddrBook.aAddrBookTabs[i], _bmp().Get(_IL(5)));
		}
	}
	if(pTrack!=NULL){
		pUsersMenu->AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuStringEx(pTrack,pUsersMenu,ID_TRAY_REFRESHSTATUS,_l("Refresh statuses"),_bmp().Get(IDB_BM_REFRESH));
	}
}

BOOL AddMessageToTrayMenu(CDWordArray* pTrack, CMenu* pMenu, int& iStart,int& iMessageCount, int& iFirstUnreadMessageCode, BOOL bUnreadOnly=FALSE, int* iPersonFilter=NULL)
{
	for(;iStart<objSettings.m_Notes.GetSize();iStart++){
		CDLG_Note* pNote=objSettings.m_Notes.GetNote(iStart);
		if(!pNote || !pNote->IsMessage()){
			continue;
		}
		if(iPersonFilter!=NULL){
			int iNotePerson=objSettings.AddrBook.IsPersonKnown(pNote->Sets.sSenderPerson);
			if((*iPersonFilter)!=iNotePerson){
				continue;
			}
		}
		if(bUnreadOnly && pNote->Sets.bIncomingMessage!=2){
			continue;
		}
		CString sNoteName=pNote->Sets.GetTitle(DEF_MENUTITLELEN);
		if(sNoteName!=""){
			DWORD dwMessageCode=WM_USER+TASK_NOTE_OFFSET+iStart;
			if(pNote->Sets.bIncomingMessage==2 && iFirstUnreadMessageCode==0){
				iFirstUnreadMessageCode=dwMessageCode;
			}
			AddMenuStringEx(pTrack,pMenu,dwMessageCode,sNoteName,_bmp().Get(pNote->GetNoteIcon(FALSE)));
			iMessageCount++;
			iStart++;
			return TRUE;
		}
	}
	return FALSE;
}


CDWordArray& AMenuIDLog()
{
	static CDWordArray aAMenuIDLog;
	return aAMenuIDLog;
}

CDWordArray& MMenuIDLog();
void AppendHelpMenu(CMenu* m_pMenu,BOOL bExtended=0);
void AppendFeatsMenu(CMenu* m_pMenu,BOOL bExtended, BOOL bAdb);
CMenu* _stdcall RefreshMainMenuEx(void* pData, BOOL bExtended);
CMenu* _stdcall RefreshAdrBookMenuEx(void* pUser, BOOL bExtended)
{
	if(bExtended){
		MMenuIDLog().RemoveAll();
		AMenuIDLog().RemoveAll();
	}
	int iCount=0,iMessageCount=0;
	CMenu* pMesCentreMenu=new CMenu();
	if(bExtended==2){
		pMesCentreMenu->CreateMenu();
		CMenu pAddFeatures;
		pAddFeatures.CreatePopupMenu();
		if(objSettings.SaveMesNotesByDef){
			AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_MESSAGELOG,_l("Message history"),_bmp().Get(IDB_BM_MSGLOG));
		}
		if(objSettings.sFBDir!="" || objSettings.bProtocolML_Active){
			AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_FINDONLINEUSERS,_l("Find online users"),_bmp().Get(IDB_BM_FIND));
		}
		if(objSettings.sFBDir!=""){
			AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_FZFOLDER,_l("Shared folder"),_bmp().Get(_IL(5)));
		}
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_IMPORTAB,_l("Import address book"),_bmp().Get(IDB_BM_ADRBOOK));
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_EXPORTAB,_l("Export address book"),_bmp().Get(IDB_BM_ADRBOOK));
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_OPTIONS,_l(OPTIONS_TP),_bmp().Get(IDB_BM_OPTIONS));
		pAddFeatures.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuStringEx(&MMenuIDLog(),&pAddFeatures,ID_SYSTRAY_EXIT,_l("Exit"),_bmp().Get(_IL(40)),FALSE,NULL,0);
		AddMenuSubmenu(pMesCentreMenu,&pAddFeatures,_l("File"));
	}else{
		pMesCentreMenu->CreatePopupMenu();
	}
	if(bExtended==2){
		CMenu pAddFeatures;
		pAddFeatures.CreatePopupMenu();
		if(!objSettings.bNoSend){
			AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_NEWMESSAGE,_l("New Message")+getActionHotKeyDsc(NEW_MSG),_bmp().Get(IDB_BM_NEWMESSAGESM));
		}
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,ID_TRAY_NEWPERSON,_l("New person"),_bmp().Get(IDB_BM_PONLINE));
		AddMenuStringEx(&AMenuIDLog(),&pAddFeatures,IDC_TAB_NEW,_l("New tab"),_bmp().Get(IDB_BM_NEWTAB));
		AddMenuSubmenu(pMesCentreMenu,&pAddFeatures,_l("New"),0);
	}
	if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
		if(objSettings.bMessCentreDefAction){
			pMainDialogWindow->dlgAddBook->m_STray.SetMenuDefaultItem(ID_TRAY_NEWMESSAGE, FALSE);
		}else{
			pMainDialogWindow->dlgAddBook->m_STray.SetMenuDefaultItem(ID_TRAY_OPENWINDOW, FALSE);
		}
	}
	if(bExtended!=2){
		AddMenuStringEx(&AMenuIDLog(),pMesCentreMenu,ID_TRAY_OPENWINDOW,_l("Open address book")+getActionHotKeyDsc(OPEN_ADDRBOOK),_bmp().Get(IDB_BM_ADRBOOK));
	}
	if(bExtended!=2){// Персональные Сообщения
		CMenu* pNewMenu=new CMenu;
		pNewMenu->CreatePopupMenu();
		if(!objSettings.bNoSend){
			AddMenuStringEx(&AMenuIDLog(),pNewMenu,ID_TRAY_NEWMESSAGE,_l("New Message")+getActionHotKeyDsc(NEW_MSG),_bmp().Get(IDB_BM_NEWMESSAGESM));
			if(objSettings.AddrBook.GetSize()>0){
				pNewMenu->AppendMenu(MF_SEPARATOR, 0, "");
				RefreshRecipientMenu(&AMenuIDLog(),pNewMenu);
				pNewMenu->AppendMenu(MF_SEPARATOR, 0, "");
			}
		}
		AddMenuStringEx(&AMenuIDLog(),pNewMenu,ID_TRAY_NEWPERSON,_l("New person"),_bmp().Get(IDB_BM_PONLINE));
		AddMenuSubmenu(pMesCentreMenu,pNewMenu,_l("Create new")+"\t...",_bmp().Get(IDB_BM_NEW));
		delete pNewMenu;
	}
	// Текущий статус...
	CMenu pStatusMenu;
	pStatusMenu.CreatePopupMenu();
	int iStatus=objSettings.iOnlineStatus;
	if(bExtended==2){
		// Так как верхнее меню не обновляется, не чекбоксим текущий
		iStatus=-1;
	}
	AddMenuStringEx(&AMenuIDLog(),&pStatusMenu,ID_TRAY_STATUSONLINE,_l("Online"),_bmp().Get(IDB_BM_PONLINE),iStatus==0);
	AddMenuStringEx(&AMenuIDLog(),&pStatusMenu,ID_TRAY_STATUSAWAY,_l("Away"),_bmp().Get(IDB_BM_PAWAY),iStatus==1);
	AddMenuStringEx(&AMenuIDLog(),&pStatusMenu,ID_TRAY_STATUSBUSY,_l("Busy"),_bmp().Get(IDB_BM_PBUSY),iStatus==2);
	AddMenuStringEx(&AMenuIDLog(),&pStatusMenu,ID_TRAY_STATUSDND,_l("DND"),_bmp().Get(IDB_BM_PDND),iStatus==3);
	if(objSettings.dwLastSendError || bExtended==2){
		AddMenuStringEx(&AMenuIDLog(),&pStatusMenu,ID_TRAY_RECONNECT,_l("Reconnect to LAN"),_bmp().Get(_IL(22)));
	}
	pStatusMenu.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuStringEx(&AMenuIDLog(),&pStatusMenu,ID_TRAY_AUTOANSWERING,_l("Auto answer"),NULL,objSettings.bAutoAnswer);
	if(bExtended!=2){
		AddMenuSubmenu(pMesCentreMenu,&pStatusMenu,_l("Online status")+"\t...",_bmp().Get(IDB_BM_PONLINE+objSettings.iOnlineStatus));
	}else{
		AddMenuSubmenu(pMesCentreMenu,&pStatusMenu,_l("Status"),0);
	}
	int iUnreadMessageCode=0;
	if(bExtended!=2){// Поступившие сообщения...
		pMesCentreMenu->AppendMenu(MF_SEPARATOR, 0, "");
		iCount=0;
		iMessageCount=0;
		while(1){
			if(AddMessageToTrayMenu(&AMenuIDLog(),pMesCentreMenu,iCount,iMessageCount,iUnreadMessageCode)==FALSE){
				break;
			}
		};
		if(iMessageCount==0){
			AddMenuString(pMesCentreMenu,0,_l("No messages"),0,FALSE,0,TRUE);
		}
		// Дополнительно...
		if(objSettings.SaveMesNotesByDef){
			AddMenuStringEx(&AMenuIDLog(),pMesCentreMenu,ID_TRAY_MESSAGELOG,_l("Message history"),_bmp().Get(IDB_BM_MSGLOG));
		}
		//
		if(pMainDialogWindow && pMainDialogWindow->dlgAddBook && iUnreadMessageCode!=0){
			pMainDialogWindow->dlgAddBook->m_STray.SetMenuDefaultItem(iUnreadMessageCode, FALSE);
		}
	}
	if(bExtended!=2){
		pMesCentreMenu->AppendMenu(MF_SEPARATOR, 0, "");
	}
	AppendHelpMenu(pMesCentreMenu,bExtended);
	AppendFeatsMenu(pMesCentreMenu,bExtended,1);
	if(bExtended!=2){
		AddMenuStringEx(&AMenuIDLog(),pMesCentreMenu,ID_TRAY_OPTIONS,_l(OPTIONS_TP),_bmp().Get(IDB_BM_OPTIONS));
		AppendAdverts(pMesCentreMenu);
		AddMenuStringEx(&AMenuIDLog(),pMesCentreMenu,ID_TRAY_EXITWINDOW,_l("Exit"),_bmp().Get(_IL(40)),FALSE,NULL,objSettings.bDoModalInProcess!=0);
	}
	return pMesCentreMenu;
}

CMenu* _stdcall RefreshAdrBookMenu(void* pUser)
{
	return RefreshAdrBookMenuEx(pUser,1);
}

BOOL DLG_Addrbook::OnInitDialog() 
{
	if(objSettings.bShadows && !IsVista()){
		SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW//ModifyStyleEx(0,WS_EX_COMPOSITED,SWP_FRAMECHANGED);
	}
	CDialog::OnInitDialog();
	{
		Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_AddrBook");
		CRect minRect(0,0,DIALOG_MINXSIZE,DIALOG_MINYSIZE);
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
		Sizer.SetMinRect(minRect);
		// Элементы
		Sizer.AddDialogElement(IDC_PERSONLIST);
		Sizer.AddDialogElement(IDC_NEWPERSON,widthSize|heightSize|fixedDimensions,objSettings.dwButtonSize);
		Sizer.AddDialogElement(ID_DELETEPERSON,widthSize|heightSize,IDC_NEWPERSON);
		Sizer.AddDialogElement(IDC_MODPERSON,widthSize|heightSize,IDC_NEWPERSON);
		Sizer.AddDialogElement(ID_QMENU,widthSize|heightSize,IDC_NEWPERSON);
		Sizer.AddDialogElement(ID_REFRESHSTAT,widthSize|heightSize,IDC_NEWPERSON);
		Sizer.AddDialogElement(ID_MSGLOG,widthSize|heightSize|hidable,IDC_NEWPERSON);
		Sizer.AddDialogElement(ID_ADOPTIONS,widthSize|heightSize,IDC_NEWPERSON);
		Sizer.AddDialogElement(IDC_FINDPERSON,widthSize|heightSize,IDC_NEWPERSON);
		Sizer.AddDialogElement(IDC_TAB_DEL,widthSize|heightSize,IDC_NEWPERSON);
		
		Sizer.AddDialogElement(IDC_TAB,0,0);
		if(objSettings.bNoSend){
			Sizer.AddDialogElement(IDC_SENDMES,widthSize|heightSize,0);
			Sizer.AddDialogElement(IDC_SENDMES2,widthSize|heightSize,0);
			Sizer.AddHideRule(IDC_SENDMES);
			Sizer.AddHideRule(IDC_SENDMES2);
			GetDlgItem(IDC_SENDMES)->EnableWindow(FALSE);
			GetDlgItem(IDC_SENDMES2)->EnableWindow(FALSE);
			CRect rt;
			GetDlgItem(IDC_SENDMES)->GetWindowRect(&rt);
			rt.bottom=rt.top+1;
			GetDlgItem(IDC_SENDMES)->MoveWindow(&rt,TRUE);
			GetDlgItem(IDC_SENDMES2)->GetWindowRect(&rt);
			GetDlgItem(IDC_SENDMES2)->MoveWindow(&rt,TRUE);
		}else{
			Sizer.AddDialogElement(IDC_SENDMES,widthSize|heightSize,IDC_NEWPERSON);
			Sizer.AddDialogElement(IDC_SENDMES2,widthSize|heightSize,IDC_NEWPERSON);
		}
		// Раскладка
		Sizer.AddLeftTopAlign(IDC_NEWPERSON,0,2);
		Sizer.AddTopAlign(ID_DELETEPERSON,0,2);
		Sizer.AddTopAlign(IDC_MODPERSON,0,2);
		Sizer.AddTopAlign(IDC_FINDPERSON,0,2);
		Sizer.AddTopAlign(IDC_TAB_DEL,0,2);
		
		Sizer.AddLeftAlign(ID_DELETEPERSON,IDC_NEWPERSON,rightPos,2);
		Sizer.AddLeftAlign(IDC_MODPERSON,ID_DELETEPERSON,rightPos,2);
		Sizer.AddLeftAlign(IDC_FINDPERSON,IDC_MODPERSON,rightPos,2);
		
		Sizer.AddLeftAlign(IDC_PERSONLIST,0,2);
		Sizer.AddTopAlign(IDC_PERSONLIST,IDC_NEWPERSON,bottomPos,2);
		Sizer.AddBottomAlign(IDC_PERSONLIST,0,-2);

		Sizer.AddRightAlign(ID_QMENU,0,-2);
		Sizer.AddRightAlign(IDC_PERSONLIST,ID_QMENU,leftPos,-2);
		Sizer.AddRightAlign(IDC_SENDMES,ID_QMENU,0);
		Sizer.AddRightAlign(IDC_SENDMES2,IDC_SENDMES,0);
		Sizer.AddRightAlign(ID_REFRESHSTAT,IDC_SENDMES,0);
		Sizer.AddRightAlign(ID_MSGLOG,IDC_SENDMES,0);
		Sizer.AddRightAlign(ID_ADOPTIONS,IDC_SENDMES,0);
		Sizer.AddRightAlign(ID_QMENU,IDC_SENDMES,0);
		Sizer.AddTopAlign(ID_QMENU,0,2);
		Sizer.AddTopAlign(IDC_SENDMES,ID_QMENU,bottomPos,2);
		Sizer.AddTopAlign(IDC_SENDMES2,IDC_SENDMES,bottomPos,2);
		Sizer.AddTopAlign(ID_REFRESHSTAT,IDC_SENDMES2,bottomPos,2);
		Sizer.AddTopAlign(ID_MSGLOG,ID_REFRESHSTAT,bottomPos,2);
		Sizer.AddRightBottomAlign(ID_ADOPTIONS,0,-2);
		Sizer.AddTopAlign(IDC_TAB,IDC_NEWPERSON,topPos,0);
		Sizer.AddBottomAlign(IDC_TAB,IDC_PERSONLIST,topPos,0);
		
		Sizer.AddRightAlign(IDC_TAB_DEL,ID_QMENU,leftPos,-2);
		Sizer.AddRightAlign(IDC_TAB,IDC_TAB_DEL,leftPos,-2);
		Sizer.AddLeftAlign(IDC_TAB,IDC_FINDPERSON,rightPos,2);
	}
	{
		m_tooltip.Create(this);
		m_tooltip.SetMaxTipWidth(objSettings.rDesktopRectangle.Width()/2);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDC_SENDMES), _l("New Message")+getActionHotKeyDsc(NEW_MSG," (",")"));
		m_tooltip.AddTool(GetDlgItem(IDC_SENDMES2), _l("Message to all in this tab"));
		m_tooltip.AddTool(GetDlgItem(IDC_NEWPERSON), _l("New entry"));
		m_tooltip.AddTool(GetDlgItem(ID_DELETEPERSON), _l("Delete"));
		m_tooltip.AddTool(GetDlgItem(IDC_MODPERSON), _l("Modify"));
		m_tooltip.AddTool(GetDlgItem(ID_MSGLOG), _l("General message history")+"(Ctrl-L)");
		m_tooltip.AddTool(GetDlgItem(ID_REFRESHSTAT), _l("Refresh statuses"));
		m_tooltip.AddTool(GetDlgItem(ID_QMENU), _l("Quick menu"));
		m_tooltip.AddTool(GetDlgItem(IDC_PERSONLIST), _l("Address book"));
		m_tooltip.AddTool(GetDlgItem(ID_ADOPTIONS), _l(OPTIONS_TP)+"(Ctrl-O)");
		m_tooltip.AddTool(GetDlgItem(IDC_TAB_DEL), _l("Delete current tab"));
	}
	{
		m_BtSendMessage.SetBitmap(IDB_BM_NEWMESSAGE);
		m_BtSendMessage2.SetBitmap(IDB_BM_NEWMESSAGE2);
		m_BtNewPerson.SetBitmap(IDB_BM_NEWPERSON);
		m_BtModPerson.SetBitmap(IDB_BM_MODPERSON);
		m_BtFindPerson.SetBitmap(IDB_BM_FIND);
		m_BtDelPerson.SetBitmap(IDB_BM_DELPERSON);
		m_RefreshStatus.SetBitmap(IDB_BM_REFRESH);
		m_BtMsgLog.SetBitmap(IDB_BM_MSGLOG);
		m_QMenu.SetBitmap(IDB_BM_MESCNTR);
		m_BtOpt.SetBitmap(IDB_BM_OPTIONS);
		m_BtTabDel.SetBitmap(IDB_BM_POPTAB);
	}
	{//Главное меню
		SetMenu(RefreshAdrBookMenuEx(0,2));
	}
	Initlist();
	Refreshlist();
	objSettings.sMCTrayTooltip=_l("Message center")+"-"+objSettings.sProtocolNB_Alias;
	m_STray.Create(NULL, WM_USER, objSettings.sMCTrayTooltip, ::AfxGetApp()->LoadIcon(objSettings.StatusIcons[objSettings.iOnlineStatus]),IDR_INFOWND_MENU, this, (!objSettings.lTrayIconsMaskM)?TRUE:FALSE);
	HICON aIList[2]={0,_bmp().CloneAsIcon(_bmp().GetResName(IDB_BM_INMESSAGE,BIGLIST_ID))};
	m_STray.SetIconList(aIList,sizeof(aIList)/sizeof(HICON));
	m_STray.SetAnimationLimits(0,1);
	m_STray.SetMenuDefaultItem(0, TRUE);
	SetWindowIcon();
	SetWindowText(objSettings.sMCTrayTooltip);
	SetTaskbarButton(this->GetSafeHwnd());
	m_STray.pdwDefTICONAction=&objSettings.UseBblClick;
	m_STray.SetUserMenu(&RefreshAdrBookMenu,OnAdrBookTrayMove);
	return FALSE;
}

void DLG_Addrbook::Initlist()
{
	m_PersonList.SetReadOnly(FALSE);
	m_PersonList.SetImageList(&theApp.MainImageList, LVSIL_SMALL);
	ListView_SetExtendedListViewStyleEx(m_PersonList.m_hWnd, 0, LVS_EX_FULLROWSELECT);
	// Устанавливаем столбцы диаграммы
	objSettings.iCol0WidthAdrb=Validate(objSettings.iCol0WidthAdrb, 20, 300, 100);
	objSettings.iCol1WidthAdrb=Validate(objSettings.iCol1WidthAdrb, 20, 300, 100);
	objSettings.iCol2WidthAdrb=Validate(objSettings.iCol2WidthAdrb, 20, 300, 100);
	m_PersonList.InsertFColumn( 0, _l("Nick"), FALSE, LVCFMT_LEFT, objSettings.iCol0WidthAdrb);
	m_PersonList.SetIItemControlType(NULL, 0);
	m_PersonList.InsertFColumn( 1, _l("Computer Name"), FALSE, LVCFMT_LEFT, objSettings.iCol1WidthAdrb);
	m_PersonList.SetIItemControlType(NULL, 1);
	m_PersonList.InsertFColumn( 2, _l("Additional Info"), FALSE, LVCFMT_LEFT, objSettings.iCol2WidthAdrb);
	m_PersonList.SetIItemControlType(NULL, 2);
	SetColorStyle();
	for(int j=0;j<objSettings.AddrBook.GetSize();j++){
		Person* p=objSettings.AddrBook.aPersons[j];
		if(p){
			p->hABIRow=NULL;
		}
	}
	m_PersonList.DeleteAllIRows();
	for(int i=0;i<objSettings.AddrBook.aAddrBookTabs.GetSize();i++){
		m_Tab.InsertItem(i,objSettings.AddrBook.aAddrBookTabs[i]);
	}
	m_Tab.SetCurSel(objSettings.iLastViewTab);
}

void DLG_Addrbook::Refreshlist()
{
	CSmartLock cs(&csView);
	int i=0,j=0;
	if(objSettings.AddrBook.bNeedResetVisibleContent==TRUE || m_Tab.GetCurSel()!=objSettings.iLastViewTab){
		m_PersonList.DeleteAllIRows();
		m_PersonList.SelectIRow(0,0,0);
		objSettings.AddrBook.bNeedResetVisibleContent=FALSE;
		for(j=0;j<objSettings.AddrBook.GetSize();j++){
			Person* p=objSettings.AddrBook.aPersons[j];
			if(p){
				p->hABIRow=0;
			}
		}
	}
	CString sTemp;
	for(j=0;j<objSettings.AddrBook.GetSize();j++){
		Person* p=objSettings.AddrBook.aPersons[j];
		if(p && p->iTabNumber==objSettings.iLastViewTab){
			if(p->hABIRow==NULL){
				p->hABIRow=m_PersonList.InsertIRow(FL_ROOT, FL_LAST, p->GetTitle(), GetStatusIcon(j)+theApp.dwOnlineStatusStartBitmap);
				m_PersonList.SetIRowData(p->hABIRow, i);
			}
		}
	}
	RefreshOnLineStatusIcons();
}

void DLG_Addrbook::RefreshOnLineStatusIcon(int iPerson)
{
	if(iPerson<0 || iPerson>=objSettings.AddrBook.aPersons.GetSize()){
		return;
	}
	CSmartLock cs(&csView);
	Person* p=objSettings.AddrBook.aPersons[iPerson];
	if(p){
		if(p->hABIRow==NULL){
			return;
		}
		CString sTitle=p->GetTitle();
		int iUnread=0;
		int iCount=objSettings.m_Notes.GetMessageCount(TRUE,&iUnread, &iPerson);
		if(iCount>0){
			if(objSettings.lShowUnreadInAddrb){
				sTitle+=Format(" (%i/%i)",iUnread,iCount);
			}else{
				sTitle+=Format(" (%i)",iCount);
			}
		}
		DWORD dwPersonStat=GetStatusIcon(iPerson);
		m_PersonList.SetIItemImage(p->hABIRow, 0, dwPersonStat+theApp.dwOnlineStatusStartBitmap);
		if(dwPersonStat==6){
			m_PersonList.SetIRowTextStyle(p->hABIRow,FL_PALE);
		}else if(dwPersonStat==4){
			m_PersonList.SetIRowTextStyle(p->hABIRow,FL_STRIKE);
		}else if(iCount>0){
			m_PersonList.SetIRowTextStyle(p->hABIRow,FL_BOLD);
		}else{
			m_PersonList.SetIRowTextStyle(p->hABIRow,FL_NORMAL);
		}
		m_PersonList.SetIItemText	(p->hABIRow, 0, sTitle);
		m_PersonList.SetIRowData	(p->hABIRow, iPerson);
		m_PersonList.SetIItemText	(p->hABIRow, 1, p->sCompName);
		m_PersonList.SetIItemText	(p->hABIRow, 2, p->sDsc);
	}
}

void DLG_Addrbook::RefreshOnLineStatusIcons()
{
	for(int j=0;j<objSettings.AddrBook.GetSize();j++){
		RefreshOnLineStatusIcon(j);
	}
}

void DLG_Addrbook::OnFindperson()
{
	OnTrayFindUsers();
}

void DLG_Addrbook::OnModperson()
{
	int iPerson=GetSelectedRow();
	if(iPerson==-1){
		Alert(_l("Select person first")+"!");
		return;
	}
	CNewPerson dlgNewPerson(this);
	dlgNewPerson.m_Nick=objSettings.AddrBook.aPersons[iPerson]->GetTitle();
	dlgNewPerson.m_CompName=objSettings.AddrBook.aPersons[iPerson]->sCompName;
	dlgNewPerson.m_Dsc=objSettings.AddrBook.aPersons[iPerson]->sDsc;
	dlgNewPerson.m_bGroup=objSettings.AddrBook.aPersons[iPerson]->IsGroup();
	dlgNewPerson.m_iTab=objSettings.AddrBook.aPersons[iPerson]->iTabNumber;
	dlgNewPerson.DoModal();
	if(dlgNewPerson.m_Nick.GetLength()>0){
		objSettings.AddrBook.aPersons[iPerson]->SetTitle(dlgNewPerson.m_Nick);
		objSettings.AddrBook.aPersons[iPerson]->sCompName=dlgNewPerson.m_CompName;
		objSettings.AddrBook.aPersons[iPerson]->sDsc=dlgNewPerson.m_Dsc;
		if(objSettings.AddrBook.aPersons[iPerson]->iTabNumber!=dlgNewPerson.m_iTab){
			objSettings.AddrBook.bNeedResetVisibleContent=TRUE;
		}
		objSettings.AddrBook.aPersons[iPerson]->iTabNumber=dlgNewPerson.m_iTab;
		objSettings.AddrBook.aPersons[iPerson]->SetGroup(dlgNewPerson.m_bGroup);
		// Сначала обновляем список!!!
		objSettings.AddrBook.bNeedResetVisibleContent=TRUE;
		Refreshlist();
		// А теперь можно и иконку подкрутить...
		ExchangeOnlineStatusWithPerson(iPerson,TRUE);
	}	
}

void DLG_Addrbook::OnDblclkPersonlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCol=0;
	int iRow=GetItemUnderCursor(&iCol);
	if(iRow==-1){
		OnNewperson();
	}else if(iCol==1){
		FORK(OnOpencomputer,iRow);
	}else if(iCol==0){
		OnRclick(TRUE);
		//OpenNoteOnPerson(iRow);
	}else{
		OnModperson();
	}
	*pResult = 0;
}

LRESULT DLG_Addrbook::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		if(message==WM_SIZE){
			CRect rt;
			m_PersonList.GetClientRect(&rt);
			objSettings.iCol0WidthAdrb=m_PersonList.GetColumnWidth(0);
			objSettings.iCol1WidthAdrb=m_PersonList.GetColumnWidth(1);
			objSettings.iCol2WidthAdrb=m_PersonList.GetColumnWidth(2);
			objSettings.iCol2WidthAdrb=rt.Width()-objSettings.iCol0WidthAdrb-objSettings.iCol1WidthAdrb-1;
			m_PersonList.SetColumnWidth(2,objSettings.iCol2WidthAdrb);
		}
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void DLG_Addrbook::OnSendmes2() 
{
	OpenNoteOnPerson(objSettings.AddrBook.GetSize()+objSettings.iLastViewTab);
}

void DLG_Addrbook::OnSendmes() 
{
	OpenNoteOnPerson(GetSelectedRow());
}

void DLG_Addrbook::OpenNoteOnPerson(int iPerson)
{
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return;
	}
	CNoteSettings* Sets=new CNoteSettings(objSettings.MessageNoteSettings);
	Sets->iIconMode=0;
	if(iPerson!=-1){
		if(iPerson<objSettings.AddrBook.GetSize()){
			if(objSettings.bUseNickOnSend){
				Sets->sReplyAddressName=objSettings.AddrBook.aPersons[iPerson]->GetTitle();
			}else{
				Sets->sReplyAddressName=objSettings.AddrBook.aPersons[iPerson]->sCompName;
			}
			Sets->SetTitle(Sets->GetMessageTitle(objSettings.AddrBook.aPersons[iPerson]->GetTitle(),FALSE));
		}else{
			Sets->sReplyAddressName=objSettings.AddrBook.GetTabComps(iPerson);
			Sets->SetTitle(TrimMessage(Sets->GetMessageTitle(Sets->sReplyAddressName,FALSE),40));
		}
	}
	Sets->bNeedFocusOnStart=TRUE;
	SetNotePosToCursor(Sets);
	//
	CString sBody=objSettings.sNewMess_BodyM;
	SubstMyVariables(sBody);
	Sets->sText=sBody+REGNAG_TEXT;
	Sets->bSelectTextOnStart=1;
	//
	OpenNote(Sets);
}

BOOL DLG_Addrbook::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	UINT iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	if(pMsg->message==WM_LBUTTONDOWN){
		if(iCtrlID==IDC_PERSONLIST){
			iStartDrug=GetItemUnderCursor();
			return 0;
		}
	}
	if(pMsg->message==WM_LBUTTONUP){
		BOOL bCanReturn=0;
		if(iCtrlID==IDC_PERSONLIST){
			int iStopDrug=GetItemUnderCursor();
			if(iStartDrug!=-1 && iStartDrug!=iStopDrug){
				if(iStopDrug==-1){
					objSettings.AddrBook.MovePerson(iStartDrug,objSettings.AddrBook.GetLastPersonInTab(objSettings.iLastViewTab));
				}else{
					objSettings.AddrBook.MovePerson(iStartDrug,iStopDrug);
				}
				if(hDragCursor){
					::SetCursor(hDragCursor);
					hDragCursor=0;
				}
				objSettings.AddrBook.bNeedResetVisibleContent=TRUE;
				Refreshlist();
				bCanReturn=1;
			}
		}
		iStartDrug=-1;
		if(bCanReturn){
			return 0;
		}
	}
	if(pMsg->message==WM_KEYDOWN && isPressed(VK_CONTROL)){
		if(pMsg->wParam=='S' && pMainDialogWindow){
			pMainDialogWindow->OnSaveallBt();
			return TRUE;
		}
		if(pMsg->wParam=='O'){
			OnAdoptions();
			return TRUE;
		}
		if(pMsg->wParam=='L'){
			OnTrayMessagelog();
			return TRUE;
		}
	}
	if(pMsg->message==WM_KEYDOWN &&	iCtrlID==IDC_PERSONLIST){
		if(pMsg->wParam==VK_DELETE){
			OnDeleteperson();
			return TRUE;
		}
		if(pMsg->wParam==VK_INSERT){
			OnNewperson();
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN){
			OnModperson();
			return TRUE;
		}
	}
	if(iCtrlID==IDC_PERSONLIST && pMsg->message==WM_RBUTTONUP){
		if(OnRclick()){
			return TRUE;
		}
	}
	if(iCtrlID==IDC_PERSONLIST && pMsg->message==WM_LBUTTONDBLCLK){
		LRESULT lRes=0;
		OnDblclkPersonlist(NULL,&lRes);
		return TRUE;
	}
	if(pMsg->message==WM_MOUSEMOVE){
		if(iCtrlID==IDC_PERSONLIST){
			int iColumn=0;
			int iUnderCursorItem=GetItemUnderCursor(&iColumn);
			if(iUnderCursorItem!=-1){
				if(iStartDrug!=-1){
					if(hDragCursor==0){
						hDragCursor=::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DRAG));
					}else{
						::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DRAG));
					}
				}
				if(iLastUnderCursorItem!=iUnderCursorItem && iUnderCursorItem!=-1){
					iLastUnderCursorItem=iUnderCursorItem;
					Person* pPerson=objSettings.AddrBook.aPersons[iUnderCursorItem];
					if(pPerson){
						CString sText=pPerson->GetItemDsc()+"\n\n"+pPerson->GetTextContent();
						m_tooltip.Pop();
						m_tooltip.UpdateTipText(SetTextForUpdateTipText(sText),GetDlgItem(IDC_PERSONLIST));
					}
				}
			}
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
				MainTracker2.bStopWatching=FALSE;
				MainTracker2.dwAddFlag1=bCanBeAutoTranspared?objSettings.objSchemes.getAlpha(objSettings.iMainStyle):0;
				CloseHandle(MouseTracker2);
				DWORD dwMouseTrackerThread2=0;
				MouseTracker2=CreateThread( NULL, 0, MouseTrackerThread, (LPVOID)&MainTracker2, 0, &dwMouseTrackerThread2);
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void DLG_Addrbook::OnDestroy()
{
	objSettings.iCol0WidthAdrb=m_PersonList.GetColumnWidth(0);
	objSettings.iCol1WidthAdrb=m_PersonList.GetColumnWidth(1);
	objSettings.iCol2WidthAdrb=m_PersonList.GetColumnWidth(2);
	CDialog::OnDestroy();
	if(pMainDialogWindow){
		pMainDialogWindow->PostMessage(NEEDORGREACTION,0,0);
	}
}

void DLG_Addrbook::OnClose()
{
	CDialog::OnClose();
	if(pMainDialogWindow){
		pMainDialogWindow->PostMessage(NEEDORGREACTION,0,0);
	}
}

void DLG_Addrbook::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
		pMainDialogWindow->dlgAddBook=NULL;
	}
	delete this;
}

void DLG_Addrbook::OnTrayExportAb()
{
	CallExportAB();
}

void DLG_Addrbook::OnTrayImportAb()
{
	CallImportAB();
}

void DLG_Addrbook::OnTrayExitwindow()
{
	if(pMainDialogWindow){
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
	}
	//ShowWindow(SW_HIDE);
}

void DLG_Addrbook::OnTrayOpenwindowFull(BOOL bShowTr, BOOL bShowWnd)
{
	if(bShowWnd){
		ShowWindow(SW_SHOW);
	}
}

void DLG_Addrbook::OnTrayOpenwindow()
{
	OnTrayOpenwindowFull(TRUE,TRUE);
}

void DLG_Addrbook::OnTrayStatusSet(int iStatus) 
{
	objSettings.aAutoAnswersRecepients.RemoveAll();
	objSettings.iOnlineStatus=iStatus;
	ExchangeOnlineStatuses();
}


void DLG_Addrbook::OnTrayStatusaway() 
{
	OnTrayStatusSet(1);
}

void DLG_Addrbook::OnTrayStatusbusy() 
{
	OnTrayStatusSet(2);
}

void DLG_Addrbook::OnTrayStatusdnd() 
{
	OnTrayStatusSet(3);
}

void DLG_Addrbook::OnTrayStatusonline() 
{
	OnTrayStatusSet(0);
}

void DLG_Addrbook::OnTrayAutoanswering() 
{
	objSettings.bAutoAnswer=!objSettings.bAutoAnswer;
	objSettings.aAutoAnswersRecepients.RemoveAll();
}

BOOL DLG_Addrbook::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(lParam==0 && wParam==2){
		OnMinimaze();
		return TRUE;
	}
	if(ParseAdvert(wParam)){
		return TRUE;
	}
	if(wParam>=WM_USER+SEND_TO_OFFSET && wParam<(DWORD)(WM_USER+SEND_TO_OFFSET+USERMSG_STEP)){
		OpenNoteOnPerson(wParam-(WM_USER+SEND_TO_OFFSET));
		return TRUE;
	}
	if(wParam>=WM_USER+TASK_NOTE_OFFSET && wParam<(DWORD)(WM_USER+TASK_NOTE_OFFSET+USERMSG_STEP)){
		int iNoteNum=wParam-WM_USER-TASK_NOTE_OFFSET;
		CDLG_Note* pNote=objSettings.m_Notes.GetNote(iNoteNum);
		if(pNote){
			pNote->ActivateThisNote();
			pNote->m_EditField.SetFocus();
		}
	}
	if(pMainDialogWindow && pMainDialogWindow->IsMainMessage(wParam)){
		pMainDialogWindow->PostMessage(WM_COMMAND,wParam,lParam);
	}
	return CDialog::OnCommand(wParam, lParam);
}

void DLG_Addrbook::OnMinimaze()
{
	ShowWindow(SW_HIDE);
}

void DLG_Addrbook::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID==SC_CLOSE){
		DWORD dwReply=AskYesNo(_l("Do you want to quit WireNote","Do you want to quit WireNote?\r\nPress 'NO' to minimize window"),&objSettings.lOnExitFromMainWnd,this);
		if(pMainDialogWindow && dwReply==IDYES){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
		}else{
			ShowWindow(SW_HIDE);
		}
		return;
	}
	if(nID==SC_MINIMIZE){
		OnMinimaze();
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void DLG_Addrbook::OnTrayNewmessage() 
{
	OpenNoteOnPerson(-1);
}

void DLG_Addrbook::OnRefreshstat() 
{
	ExchangeOnlineStatuses();
}

void DLG_Addrbook::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if(bShow){
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOSENDCHANGING);
	}else{
		CDialog::OnShowWindow(bShow, nStatus);
	}
}

void DLG_Addrbook::OnTrayRefreshstatus() 
{
	OnRefreshstat();
}

BOOL DLG_Addrbook::OnRclick(BOOL bPerformDefAction)
{
	CMenu menu;
	menu.CreatePopupMenu();
	int iPerson=GetSelectedRow();
	if(iPerson<0){
		return FALSE;
	}
	Person* pPerson=objSettings.AddrBook.aPersons[iPerson];
	if(!pPerson){
		return FALSE;
	}
	int dwDefAction=0;
	AddMenuString(&menu,0,pPerson->GetTitle(),_bmp().Get(IDB_BM_PONLINE+GetStatusIcon(iPerson)),FALSE,NULL,FALSE,TRUE);
	menu.AppendMenu(MF_SEPARATOR, 0, "");
	int iStart=0,iCount=0;
	while(1){
		if(AddMessageToTrayMenu(NULL,&menu,iStart,iCount,dwDefAction,FALSE,&iPerson)==FALSE){
			break;
		}
	}
	if(iCount>0){
		menu.AppendMenu(MF_SEPARATOR, 0, "");
	}
	if(dwDefAction==0){
		dwDefAction=ID_TRAY_NEWMESSAGEONPERSON;
	}
	if(!objSettings.bNoSend){
		AddMenuString(&menu,ID_TRAY_NEWMESSAGEONPERSON,_l("Send message"),_bmp().Get(IDB_BM_NEWMESSAGE));
	}
	AddMenuString(&menu,ID_TRAY_MODPERSON,_l("Modify person"),_bmp().Get(IDB_BM_MODPERSON));
	AddMenuString(&menu,ID_TRAY_USERMSGLOG,_l("Message history"),_bmp().Get(IDB_BM_MSGLOG));
	AddMenuString(&menu,ID_TRAY_UPDATEPERSONSTATUS,_l("Update status"),_bmp().Get(IDB_BM_REFRESH));
	AddMenuString(&menu,ID_TRAY_OPENCOMPUTER,_l("Open computer"),_bmp().Get(_IL(APP_ICONS_OFFSET)));
	AddMenuString(&menu,ID_TRAY_NEWPERSON,_l("New person"),_bmp().Get(IDB_BM_PONLINE));
	AppendAdverts(&menu);
	AddMenuString(&menu,ID_TRAY_DELPERSON,_l("Delete"),_bmp().Get(IDB_BM_DEL));
	if(bPerformDefAction){
		PostMessage(WM_COMMAND,dwDefAction,0);
	}else{
		::SetMenuDefaultItem(menu.m_hMenu, dwDefAction, FALSE);
		CPoint pos;
		GetCursorPos(&pos);
		::TrackPopupMenu(menu.m_hMenu, 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	}
	return TRUE;
}

void DLG_Addrbook::OnTrayDelperson() 
{
	OnDeleteperson();
}

void DLG_Addrbook::OnTrayMessagelog() 
{
	OnMsglog();
}

void DLG_Addrbook::OnTrayModperson() 
{
	OnModperson();
}

void DLG_Addrbook::OnTrayNewmessageonperson() 
{
	OnSendmes();
}

void DLG_Addrbook::OnTrayNewperson() 
{
	OnNewperson();
}

void DLG_Addrbook::PopupAdrBook() 
{
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
		SetWindowPos(&wndTopMost,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
		Sizer.ApplyStickness();
		StartMouseTracker();
	}
}

void DLG_Addrbook::StopMouseTracker()
{
	MainTracker.bStopWatching=TRUE;
}

void DLG_Addrbook::StartMouseTracker()
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

void DLG_Addrbook::OnTrayUpdatepersonstatus()
{
	int iPerson=GetSelectedRow();
	if(iPerson==-1){
		return;
	}
	ExchangeOnlineStatusWithPerson(iPerson);
}

void DLG_Addrbook::OnTrayOpencomputer() 
{
	int iPerson=GetSelectedRow();
	if(iPerson==-1){
		return;
	}
	FORK(OnOpencomputer,iPerson);
}

void DLG_Addrbook::SetColorStyle()
{
	// Прозрачность
	static BOOL bWasSemi=0;
	if(objSettings.objSchemes.getAlpha(objSettings.iMainStyle)==100){
		// Не выключаем больше никогда
		if(!bWasSemi){
			SetLayeredWindowStyle(this->m_hWnd,FALSE);
		}else{
			SetLayeredWindowStyle(this->m_hWnd,TRUE);
			SetWndAlpha(this->m_hWnd, 100,TRUE);
		}
	}else{
		bWasSemi=1;
		SetLayeredWindowStyle(this->m_hWnd,TRUE);
		SetWndAlpha(this->m_hWnd, objSettings.objSchemes.getAlpha(objSettings.iMainStyle),TRUE);
	}
	m_PersonList.m_crIItemText=objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	m_PersonList.m_crIRowBackground=objSettings.objSchemes.getBgColor(objSettings.iMainStyle);
	m_PersonList.SetBkColor(objSettings.objSchemes.getBgColor(objSettings.iMainStyle));
	m_PersonList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	m_PersonList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	m_PersonList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);

	// Шрифт
	objSettings.objSchemes.getFont(objSettings.iMainStyle)->GetLogFont(&logfont);
	deleteFont(font);
	font=new CFont();
	font->CreateFontIndirect(&logfont);
	m_PersonList.SetFont(font,FALSE);
	return;
}

void DLG_Addrbook::OnTrayUsermsglog() 
{
	if(pMainDialogWindow){
		int iPerson=GetSelectedRow();
		if(iPerson>=0){
			Person* pPerson=objSettings.AddrBook.aPersons[iPerson];
			if(pPerson){
				pMainDialogWindow->OpenMsgLogOnNick(pPerson->GetTitle());
			}
		}
	}
}

void DLG_Addrbook::OnMsglog() 
{
	if(pMainDialogWindow){
		pMainDialogWindow->OpenMsgLogOnNick(DEFAULT_GENERALLOG);
	}
}

void DLG_Addrbook::OnTrayAPopup()
{
	objSettings.bAutoPopupAdrBook=objSettings.bAutoPopupAdrBook?0:1;
}

void DLG_Addrbook::OnTrayOptions() 
{
	objSettings.OpenOptionsDialog("",this);//_l(MESSAGING_TOPIC)
}

void DLG_Addrbook::SetWindowIcon()
{
	HICON hIcon=::AfxGetApp()->LoadIcon(objSettings.StatusIcons[objSettings.iOnlineStatus]);
	HICON hIcoText=hIcon;
	if(objSettings.lShowMsgInAddrbookIcon>0){
		int iUnread=0;
		int iCount=objSettings.m_Notes.GetMessageCount(TRUE,&iUnread);
		CString sText=Format("%i",iUnread);
		if(objSettings.lShowMsgInAddrbookIcon==2){
			sText=Format("%i",iCount);
		}
		if(sText!="0"){
			hIcoText=PutTextInIcon(hIcon,sText,objSettings.objSchemes.getFont(objSettings.iMainStyle),RGB(254,0,0),RGB(250,250,250));
			ClearIcon(hIcon);
		}
	}
	m_QMenu.SetBitmap(hIcoText);
	if(pMainDialogWindow){
		pMainDialogWindow->m_BtAdrBook.SetBitmap(hIcoText);
	}
	m_STray.SetIcon(hIcoText,TRUE);
	SetIcon(hIcoText,TRUE);
	SetIcon(hIcoText,FALSE);
	// Показывать ли ее или скрывать
	if(!objSettings.lTrayIconsMaskM){
		m_STray.HideIcon();
	}else{
		m_STray.ShowIcon();
	}
}

BOOL DLG_Addrbook::IsAddrBookMessage(WPARAM wParam)
{
	if(wParam>=WM_USER+SEND_TO_OFFSET && wParam<(DWORD)(WM_USER+SEND_TO_OFFSET+USERMSG_STEP)){
		return TRUE;
	}
	for(int i=0;i<AMenuIDLog().GetSize();i++){
		if(AMenuIDLog()[i]==DWORD(wParam)){
			return TRUE;
		}
	}
	return FALSE;
}

void DLG_Addrbook::OnQmenu() 
{
	int iCount=0,iMessageCount=0;
	CMenu* pMesCentreMenu=new CMenu();
	pMesCentreMenu->CreatePopupMenu();
	// Поступившие сообщения...
	iCount=0;
	iMessageCount=0;
	int iUnreadMessageCode=0;
	while(1){
		if(AddMessageToTrayMenu(NULL,pMesCentreMenu,iCount,iMessageCount,iUnreadMessageCode)==FALSE){
			break;
		}
	};
	if(iMessageCount==0){
		AddMenuString(pMesCentreMenu,0,_l("No messages"),0,FALSE,0,TRUE);
	}
	pMesCentreMenu->AppendMenu(MF_SEPARATOR, 0, "");
	// Текущий статус...
	CMenu pStatusMenu;
	pStatusMenu.CreatePopupMenu();
	AddMenuString(&pStatusMenu,ID_TRAY_STATUSONLINE,_l("Online"),_bmp().Get(IDB_BM_PONLINE),objSettings.iOnlineStatus==0);
	AddMenuString(&pStatusMenu,ID_TRAY_STATUSAWAY,_l("Away"),_bmp().Get(IDB_BM_PAWAY),objSettings.iOnlineStatus==1);
	AddMenuString(&pStatusMenu,ID_TRAY_STATUSBUSY,_l("Busy"),_bmp().Get(IDB_BM_PBUSY),objSettings.iOnlineStatus==2);
	AddMenuString(&pStatusMenu,ID_TRAY_STATUSDND,_l("DND"),_bmp().Get(IDB_BM_PDND),objSettings.iOnlineStatus==3);
	AddMenuString(&pStatusMenu,ID_TRAY_AUTOANSWERING,_l("Auto answer"),NULL,objSettings.bAutoAnswer);
	AddMenuSubmenu(pMesCentreMenu,&pStatusMenu,_l("Online status")+"\t...",_bmp().Get(IDB_BM_PONLINE+objSettings.iOnlineStatus));
	//
	if(pMainDialogWindow && pMainDialogWindow->dlgAddBook && iUnreadMessageCode!=0){
		pMesCentreMenu->SetDefaultItem(iUnreadMessageCode, FALSE);
	}
	pMesCentreMenu->AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(pMesCentreMenu,ID_TRAY_OPTIONS,_l(OPTIONS_TP),_bmp().Get(IDB_BM_OPTIONS));
	AppendHelpMenu(pMesCentreMenu);
	CRect rt;
	m_QMenu.GetWindowRect(&rt);
	::TrackPopupMenu(pMesCentreMenu->GetSafeHmenu(), 0, rt.right, rt.top, 0, this->GetSafeHwnd(), NULL);
	return;
}

void DLG_Addrbook::OnAdoptions() 
{
	OnTrayOptions();
}

void DLG_Addrbook::OnTrayFindUsers()
{
	CDLG_Find dlg(NULL);
	if(!dlg.bAlreadyExist){
		dlg.bSearchForPersons=TRUE;
		dlg.DoModal();
	}
	return;
}

void DLG_Addrbook::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	objSettings.iLastViewTab=m_Tab.GetCurSel();
	objSettings.AddrBook.bNeedResetVisibleContent=TRUE;
	Refreshlist();
	*pResult = 0;
}

void DLG_Addrbook::OnDelTab()
{
	CMenu menu;
	menu.CreatePopupMenu();
	AddMenuString(&menu,IDC_TAB_NEW,_l("New tab"),_bmp().Get(IDB_BM_NEWTAB));
	AddMenuString(&menu,IDC_TAB_REN,_l("Rename tab"),_bmp().Get(IDB_BM_EDITAB));
	AddMenuString(&menu,IDC_TAB_DEL,_l("Delete tab"),_bmp().Get(IDB_BM_DELTAB));
	::SetMenuDefaultItem(menu.m_hMenu, 0, FALSE);
	CRect rt;
	m_BtTabDel.GetWindowRect(&rt);
	DWORD dwRes=::TrackPopupMenu(menu.m_hMenu, TPM_RETURNCMD, rt.right, rt.top, 0, this->GetSafeHwnd(), NULL);
	if(dwRes==IDC_TAB_NEW){
		OnNewTab();
	}
	if(dwRes==IDC_TAB_REN){
		OnRenTab();
	}
	if(dwRes==IDC_TAB_DEL){
		DelCurTab();
	}
}

void DLG_Addrbook::DelCurTab()
{
	int iCurSel=m_Tab.GetCurSel();
	if(iCurSel<1){
		Alert(_l("This tab can not be deleted")+"!");
		return;
	}
	if(AskYesNo(_l("Delete current tab and\nall address book entries in it")+"?", PROGNAME,0)!=IDYES){
		return;
	}
	CSmartLock cs(&csView);
	m_PersonList.DeleteAllIRows();
	objSettings.AddrBook.bNeedResetVisibleContent=FALSE;
	for(int j=objSettings.AddrBook.GetSize()-1;j>=0;j--){
		Person* p=objSettings.AddrBook.aPersons[j];
		if(p && p->iTabNumber==iCurSel){
			objSettings.AddrBook.DeletePerson(j);
		}else if(p && p->iTabNumber>iCurSel){
			p->iTabNumber--;
		}
	}
	objSettings.AddrBook.aAddrBookTabs.RemoveAt(iCurSel);
	m_Tab.DeleteItem(iCurSel);
	objSettings.iLastViewTab=0;
	m_Tab.SetCurSel(objSettings.iLastViewTab);
	objSettings.AddrBook.bNeedResetVisibleContent=TRUE;
	Refreshlist();
}

void DLG_Addrbook::OnNewTab()
{
	CSmartLock cs(&csView);
	CDlgSimpleEdit ed(this);
	ed.iStyle=1;
	ed.m_EditTitle=_l("New tab");
	if(ed.DoModal()==IDOK){
		int i=objSettings.AddrBook.aAddrBookTabs.Add(ed.m_EditTitle);
		m_Tab.InsertItem(i,objSettings.AddrBook.aAddrBookTabs[i]);		
		m_Tab.SetCurSel(i);
		objSettings.iLastViewTab=i;
		objSettings.AddrBook.bNeedResetVisibleContent=TRUE;
		Refreshlist();
	}
}

void DLG_Addrbook::OnRenTab()
{
	CSmartLock cs(&csView);
	int iCurSel=m_Tab.GetCurSel();
	if(iCurSel<0){
		return;
	}
	CDlgSimpleEdit ed(this);
	ed.iStyle=1;
	ed.m_EditTitle=objSettings.AddrBook.aAddrBookTabs[iCurSel];
	if(ed.DoModal()==IDOK){
		objSettings.AddrBook.aAddrBookTabs[iCurSel]=ed.m_EditTitle;
		TCITEM pTabCtrlItem;
		memset(&pTabCtrlItem,0,sizeof(pTabCtrlItem));
		pTabCtrlItem.mask=TCIF_TEXT;
		pTabCtrlItem.pszText=objSettings.AddrBook.aAddrBookTabs[iCurSel].GetBuffer(strlen(objSettings.AddrBook.aAddrBookTabs[iCurSel]));
		m_Tab.SetItem(iCurSel,&pTabCtrlItem);
		objSettings.AddrBook.aAddrBookTabs[iCurSel].ReleaseBuffer();
	}
}

void DLG_Addrbook::SortList(int iColumn)
{
	EnterCriticalSection(&objSettings.bTreeNeedUpdateLock);
	BOOL bDir=1;
	if(iColumn<0){
		bDir=-1;
		iColumn=-iColumn;
	}
	for(int i=0;i<objSettings.AddrBook.GetSize()-1;i++){
		for(int j=i+1;j<objSettings.AddrBook.GetSize();j++){
			Person* pi=objSettings.AddrBook.aPersons[i];
			Person* pj=objSettings.AddrBook.aPersons[j];
			if(pi->iTabNumber==objSettings.iLastViewTab && pj->iTabNumber==objSettings.iLastViewTab){
				BOOL bMove=0;
				if(iColumn==1 && ((pi->GetTitle()>pj->GetTitle() && bDir>0)||(pi->GetTitle()<pj->GetTitle() && bDir<0))){
					bMove=1;
				}
				if(iColumn==2 && ((pi->sCompName>pj->sCompName && bDir>0)||(pi->sCompName<pj->sCompName && bDir<0))){
					bMove=1;
				}
				if(iColumn==3 && ((pi->sDsc>pj->sDsc && bDir>0)||(pi->sDsc<pj->sDsc && bDir<0))){
					bMove=1;
				}
				if(bMove){
					objSettings.AddrBook.MovePerson(j,i);
				}
			}
		}
	}
	LeaveCriticalSection(&objSettings.bTreeNeedUpdateLock);
	objSettings.AddrBook.bNeedResetVisibleContent=TRUE;
	Refreshlist();
}

void DLG_Addrbook::OnColumnclickTasklist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iSubItem!=abs(objSettings.iSortAColumn)-1){
		if(objSettings.iSortAColumn>0){
			objSettings.iSortAColumn=pNMListView->iSubItem+1;
		}else{
			objSettings.iSortAColumn=-(pNMListView->iSubItem+1);
		}
	}else{
		objSettings.iSortAColumn=-objSettings.iSortAColumn;
	}
	SortList(objSettings.iSortAColumn);
	*pResult = 0;
}

void DLG_Addrbook::OnNotemenuFZFolder()
{
	CString sFFF=GetFBFZ();
	if(sFFF!=""){
		::ShellExecute(NULL,"open",sFFF,NULL,NULL,SW_SHOWNORMAL);
	}
}

void DLG_Addrbook::OnTrayReconnect()
{
	objSettings.dwLastSendError=0;
	objSettings.objMessageWrapper->ConnectAll();
}