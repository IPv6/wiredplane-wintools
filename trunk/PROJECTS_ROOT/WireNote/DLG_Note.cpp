// DLG_Note.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include ".\dlg_note.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define BG1C	0.2f
#define BG2C	0.1f
#define ID_RESENDTIMER 100
BOOL& IsEmbedded();
CNoteSettings::CNoteSettings():CActiItem(2)
{
	sAttachment="";
	bDeleteAfterSend=FALSE;
	CreationTime=COleDateTime::GetCurrentTime();
	bNeedFocusOnStart=FALSE;
	sText="";
	m_sTitle="";
	bSaveOnExit=FALSE;
	bTopMost=FALSE;
	iViewMode=0;
	ReadOnly=FALSE;
	AutoResize=FALSE;
	ReplyButton=FALSE;
	sSenderPerson="";
	sReplyAddressName="";
	iXPos=iYPos=iYSize=iXSize=-1;
	iViewMode=0;
	SetApplicationsList("");
	bAutoHide=FALSE;
	bWordWrap=TRUE;
	bSelectTextOnStart=FALSE;
	bIncomingMessage=0;
	bIncMsgOpenType=0;
	b_systemCopySID=FALSE;
	bFixedSize=FALSE;
	bMouseTrans=0;
	iIconMode=0;
	bUndeletable=0;
	bDisableTransparency=0;
	bHidden=0;
}



CNoteSettings::~CNoteSettings()
{
};

CNoteSettings& CNoteSettings::operator=(CNoteSettings& CopyObj)
{
	if(CopyObj.b_systemCopySID){
		//Копируется только при создании из файла
		m_sID=CopyObj.m_sID;
	}
	SetColorCode(CopyObj.GetColorCode());
	SetIconNum(CopyObj.GetIconNum());
	m_sTitle=CopyObj.m_sTitle;
	m_sPath=CopyObj.m_sPath;
	lShared=CopyObj.lShared;
	m_iRevision=CopyObj.m_iRevision;
	sAuthor=CopyObj.sAuthor;
	iIconMode=CopyObj.iIconMode;
	bFixedSize=CopyObj.bFixedSize;
	bMouseTrans=CopyObj.bMouseTrans;
	bDeleteAfterSend=CopyObj.bDeleteAfterSend;
	bIncomingMessage=CopyObj.bIncomingMessage;
	bIncMsgOpenType=CopyObj.bIncMsgOpenType;
	bSelectTextOnStart=CopyObj.bSelectTextOnStart;
	bNeedFocusOnStart=CopyObj.bNeedFocusOnStart;
	sText=CopyObj.sText;
	bSaveOnExit=CopyObj.bSaveOnExit;
	bTopMost=CopyObj.bTopMost;
	iViewMode=CopyObj.iViewMode;
	ReadOnly=CopyObj.ReadOnly;
	AutoResize=CopyObj.AutoResize;
	hHotkey=CopyObj.hHotkey;
	ReplyButton=CopyObj.ReplyButton;
	sReplyAddressName=CopyObj.sReplyAddressName;
	sSenderPerson=CopyObj.sSenderPerson;
	iXSize=CopyObj.iXSize;
	iXPos=CopyObj.iXPos;
	iYSize=CopyObj.iYSize;
	iYPos=CopyObj.iYPos;
	iViewMode=CopyObj.iViewMode;
	SetApplicationsList(CopyObj.sPinnedToApp);
	bAutoHide=CopyObj.bAutoHide;
	bWordWrap=CopyObj.bWordWrap;
	bUndeletable=CopyObj.bUndeletable;
	sAttachment=CopyObj.sAttachment;
	sItemIDToAttachAfterStart=CopyObj.sItemIDToAttachAfterStart;
	bDisableTransparency=CopyObj.bDisableTransparency;
	bHidden=CopyObj.bHidden;
	return *this;
}

CNoteSettings::CNoteSettings(CNoteSettings& CopyObj, const char* szTitlePattern, const char* szBodyPattern):CActiItem(2)
{
	*this=CopyObj;
	if(szTitlePattern){
		CString sTitle=szTitlePattern;
		SubstMyVariables(sTitle);
		SetTitle(sTitle);
	}
	if(szBodyPattern){
		CString sBody=(CString)szBodyPattern;
		SubstMyVariables(sBody);
		sText=sBody;
	}
	sText+=REGNAG_TEXT;
}

void CNoteSettings::ActivateChanges(const char* szTitle, DWORD* pdwPrir, DWORD* pdwState)
{
	if(szTitle){
		SetTitle(szTitle);
	}
	if(pdwPrir){
		SetPriority(*pdwPrir);
	}
	if(GetState()!=-1 && pdwState){
		SetState(*pdwState);
	}
	CDLG_Note* note=objSettings.m_Notes.FindNoteByID(GetID());
	note->ActivateNoteParams(1,0,IsShared());
	ItemContentChanged();
}

CString CNoteSettings::GetFileName(BOOL bTrash)
{
	if(bTrash){
		return getFileFullName(objSettings.sTrashDirectory,TRUE)+GetID()+"."NOTE_EXTENSION;
	}else{
		return getFileFullName(objSettings.sNotesDirectory,TRUE)+GetID()+"."NOTE_EXTENSION;
	}
}

CString CNoteSettings::GetTextContent()
{
	return sText;
}

CString CNoteSettings::GetItemDsc()
{
	CString sOut=GetTitle();
	if(!objSettings.bHideDateFld){
		sOut+=Format(", %s: %s",_l("Modified"),GetDateInCurFormat(GetCrTime()));
	}
	if(IsShared()){
		sOut+=Format("\n %s %s",_l("Published at billboard by"), GetAuthor());
	}
	if(GetRevision()){
		sOut+=Format("\n %s: %i",_l("Revision"), GetRevision());
	}
	return sOut;
};

BOOL CNoteSettings::SaveToFile(const char* szFile)
{
	if(!Saveable()){
		return FALSE;
	}
	CString sFile=szFile?szFile:GetFileName();
	CDataXMLSaver xmlFile(sFile,"NOTE",FALSE);
	CString sItem=ConvertToString();
	xmlFile.putValue("item",sItem,XMLNoConversion);
	xmlFile.putValue("undeletable",bUndeletable);
	xmlFile.putValue("hidden",bHidden);
	xmlFile.putValue("word-wrap",bWordWrap);
	xmlFile.putValue("fixed-size",bFixedSize);
	xmlFile.putValue("mouse-trans",bMouseTrans);
	xmlFile.putValue("icon-mode",iIconMode);
	xmlFile.putValue("stay-ontop",bTopMost);
	xmlFile.putValue("auto-hide",bAutoHide);
	xmlFile.putValue("app-pinned",sPinnedToApp, XMLWithStartCR|XMLWithEndCR);
	xmlFile.putValue("view-mode",iViewMode);
	xmlFile.putValue("reply-button",ReplyButton);
	xmlFile.putValue("reply-to",sReplyAddressName);
	xmlFile.putValue("xpos",iXPos);
	xmlFile.putValue("ypos",iYPos);
	xmlFile.putValue("xsize",iXSize);
	xmlFile.putValue("ysize",iYSize);
	xmlFile.putValue("activate-hk",hHotkey);
	xmlFile.putValue("note-content",sText,XMLWithStartCR|XMLWithEndCR);
	if(objSettings.bSaveNoteAttachments){
		xmlFile.putValue("note-attachment",sAttachment,XMLWithStartCR|XMLWithEndCR|XMLNoConversion);
	}
	if(objSettings.lFBCrypt){
		xmlFile.CryptBody();
	}
	return xmlFile.SaveDataIntoSource();
}

BOOL CNoteSettings::LoadFromFile(const char* szFileName)
{
	if(!szFileName){
		return 0;
	}
	CDataXMLSaver xmlFile(szFileName,"NOTE",TRUE);
	xmlFile.UncryptBody();
	CString sItem;
	xmlFile.getValue("item",sItem);
	ConvertFromString(sItem,szFileName);
	if(isItemCorrupted()){
		return FALSE;
	}
	bSaveOnExit=TRUE;
	xmlFile.getValue("undeletable",bUndeletable);
	xmlFile.getValue("hidden",bHidden);
	xmlFile.getValue("icon-mode",iIconMode);
	xmlFile.getValue("view-mode",iViewMode);
	xmlFile.getValue("stay-ontop",bTopMost);
	xmlFile.getValue("auto-hide",bAutoHide);
	xmlFile.getValue("app-pinned",sPinnedToApp, "", XMLTrimSpaces);
	xmlFile.getValue("word-wrap",bWordWrap);
	xmlFile.getValue("xpos",iXPos);
	xmlFile.getValue("xsize",iXSize);
	xmlFile.getValue("ypos",iYPos);
	xmlFile.getValue("ysize",iYSize);
	xmlFile.getValue("fixed-size",bFixedSize);
	xmlFile.getValue("mouse-trans",bMouseTrans);
	xmlFile.getValue("reply-button",ReplyButton);
	xmlFile.getValue("reply-to",sReplyAddressName);
	xmlFile.getValue("activate-hk",hHotkey);
	xmlFile.getValue("note-content",sText,"",XMLTrimSpaces);
	xmlFile.getValue("note-attachment",sAttachment,"",XMLTrimSpaces|XMLNoConversion);
	if(strstr(szFileName,DEF_BB_NDIR)!=0){
		lShared=1;
	}
	if(Saveable()){
		b_systemCopySID=TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CNoteSettings::Saveable()
{
	return bSaveOnExit;
}

BOOL CNoteSettings::DelFile()
{
	CString sFile=GetFileName();
	if(objSettings.bDelToTrash && Saveable()){
		CString sBakFile=GetFileName(TRUE);
		SetFileAttributes(sBakFile,FILE_ATTRIBUTE_NORMAL);
		DeleteFile(sBakFile);
		PartlyEmptyTrash();
		BOOL bRes=MoveFile(sFile,sBakFile);
		if(bRes==0){
			DWORD dwErr=GetLastError();
			Alert(Format("Failed to trash note! From '%s' to '%s', err 0x%08X",sFile,sBakFile,dwErr),"Error");
		}
		return bRes;
	}else{
		return DeleteFile(sFile);
	}
}

BOOL CNoteSettings::ShareItem(BOOL bNewSharingData, int iSectionNum)
{
	CSmartLock SL(&objSettings.csSharing,TRUE,IDC_CURSORSAVE);
	SL.SetMinWait(500);
	CString sFile=GetValidFileName(this,DEF_BB_NDIR,NOTE_EXTENSION,iSectionNum);
	if(sFile==""){
		return FALSE;
	}
	CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(GetID());
	pNote->GetNoteText();
	pNote->GetTitleToSets();
	pNote->RememberWindowPos();
	SetAuthor(objSettings.sDefAuthor);
	if(!SaveToFile(sFile)){
		Alert(_l("Failed to publish item at Billboard\nBillboard is temporarily down"),_l("Billboard error"),9000);
		return FALSE;
	}
	if(!IsShared()){
		SetPath(BB_PATH);//NOTE_PATH"/"
		CItem* pItem=CItem::GetItemByID(BB_PATH);
		if(pItem){
			pMainDialogWindow->DragItem(this,pItem);
		}
	}
	SetShared(1,sFile);
	SmartWarning(_l("Item successfully published at Billboard\nAfter some minutes it will be delivered to others\nTo change this item at billboard, you have to republish it\n"),_l("Item published"),&objSettings.bSWarning_ShareOK);
	return TRUE;
}

CString CNoteSettings::GetMessageTitle(const char* szAddr, BOOL bFrom, const char* szFromR)
{
	int iUserNum=0;
	CString sIPAddr;
	if(szFromR){
		sIPAddr=szFromR;
	}else{
		sIPAddr=objSettings.AddrBook.TranslateNickToIP(szAddr,&iUserNum);
	}
	CString sTitle=_l("Message");
	if(bFrom){
		sTitle="%HOUR:%MINUTE";
		sTitle+=" ";
		sTitle+=_l("From");
		sTitle+=" ";
		sTitle+="%FROM";
		if(sIPAddr.Find(szAddr)==-1){
			sTitle+=" (";
			sTitle+="%IP";
			sTitle+=")";
		}
	}else{
		sTitle=objSettings.sNewMess_TitleM;
	}
	SubstMyVariables(sTitle);
	sTitle.Replace("%RECP",szAddr);
	sTitle.Replace("%FROM",szAddr);
	sTitle.Replace("%IP",sIPAddr);
	return sTitle;
}

void CNoteSettings::SetColorCode(long iCode)
{
	CItem::SetColorCode(iCode);
	CDLG_Note* note=objSettings.m_Notes.FindNoteByID(GetID());
	if(note){
		note->SetColorCode(iCode);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDLG_Note dialog
void CDLG_Note::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Note)
	DDX_Control(pDX, IDC_RECENTA, m_RecentList);
	DDX_Control(pDX, IDC_TITLE, m_Title);
	DDX_Control(pDX, IDC_NOTE_ICON, m_NoteIcon);
	DDX_Control(pDX, IDC_ABOOK, m_ABook);
	DDX_Control(pDX, IDC_SENDMESSAGE, m_Send);
	DDX_Control(pDX, IDC_NOTE_PRINTBT, m_PrintNote);
	DDX_Control(pDX, IDC_NOTE_FIXSIZE, m_RemindLater);
	DDX_Control(pDX, IDC_NOTE_SAFETOFILE, m_SaveToFile);
	DDX_Control(pDX, IDC_NOTE_DEL, m_DestroyNote);
	DDX_Control(pDX, IDC_NOTE_REPLY, m_Reply);
	DDX_Control(pDX, IDC_NOTE_OPTMENU, m_OptionsDialog);
	DDX_Control(pDX, IDC_COMBOBOXEX, m_Recipients);
	DDX_Control(pDX, IDC_TEXTFIELD, m_EditField);
	DDX_Control(pDX, IDC_NOTE_ONTOP, m_Ontop);
	DDX_Control(pDX, IDC_NOTE_RED, m_ColorRed);
	DDX_Control(pDX, IDC_NOTE_OPTION, m_Option);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_Note, CDialog)
	//{{AFX_MSG_MAP(CDLG_Note)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_NOTE_RED, OnNoteColorChange)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_NOTEMENU_SWITCHTRANSPARENCY, OnNotemenuSwitchTransp)
	ON_COMMAND(ID_NOTEMENU_MESSAGEFILTER, OnNotemenuMsgFilter)
	ON_COMMAND(ID_NOTEMENU_NOTEOPTIONS, OnNotemenuNoteSettings)
	ON_COMMAND(ID_NOTEMENU_MESSAGEOPTIONS, OnNotemenuMsgSettings)
	ON_COMMAND(ID_NOTEMENU_NETWORKSETTINGS, OnNotemenuNetSettings)
	ON_BN_CLICKED(IDC_NOTE_OPTION, OnNoteOption)
	ON_COMMAND(IDC_NOTE_OPTDIALOG, OnNoteOptdialog)
	ON_BN_CLICKED(IDC_NOTE_REPLY, OnNoteQuote)
	ON_COMMAND(IDC_ADDTOADRBOOK, OnAddtoadrbook)
	ON_BN_CLICKED(IDC_NOTE_COPYTOCL, OnNoteCopytocl)
	ON_BN_CLICKED(IDC_NOTE_PRINTBT, OnNotemenuPrint)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_NOTEMENU_BUTTONVIEW, OnNotemenuView)
	ON_COMMAND(ID_NOTEMENU_EXTENDEDVIEW, OnNotemenuExtendedview)
	ON_COMMAND(ID_NOTEMENU_SMALLVIEW, OnNotemenuSmallview)
	ON_COMMAND(ID_NOTEMENU_HIDENOTE, OnNotemenuHidenote)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SENDMESSAGE, OnSendmessage)
	ON_COMMAND(ID_NOTEMENU_CLIPSPY, OnNotemenuClipSpy)
	ON_COMMAND(ID_NOTEMENU_AUTOHIDE, OnNotemenuAutohide)
	ON_COMMAND(ID_NOTEMENU_ASSIGNHOTKEY, OnNotemenuAssignHK)
	ON_COMMAND(ID_NOTEMENU_PINMENU, ActivatePinupPopupMenu)
	ON_COMMAND(ID_NOTEMENU_SEARCH, OnGSearch)
	ON_COMMAND(ID_NOTEMENU_EXEASSCR, OnExeAsScr)
	ON_COMMAND(ID_NOTEMENU_PREVENTFROMEDIT, OnNotemenuPreventfromedit)
	ON_COMMAND(ID_NOTEMENU_REMINDIN15MIN, OnNotemenuRemindin15min)
	ON_COMMAND(ID_NOTEMENU_REMINDAFTER3H, OnNotemenuRemindafter3h)
	ON_COMMAND(ID_NOTEMENU_REMINDIN1H, OnNotemenuRemindin1h)
	ON_COMMAND(ID_NOTEMENU_REMINDIN30MIN, OnNotemenuRemindin30min)
	ON_COMMAND(ID_NOTEMENU_REMINDTOMORROW, OnNotemenuRemindtomorrow)
	ON_COMMAND(ID_NOTEMENU_BRINGALLONTOP, OnNotemenuBringallontop)
	ON_BN_CLICKED(IDC_NOTE_FIXSIZE, OnNotemenuRemindtomorrow)
	ON_COMMAND(ID_NOTEMENU_SHOWNOTE, OnNotemenuShownote)
	ON_COMMAND(ID_NOTEMENU_WORDWRAP, OnNotemenuWordwrap)
	ON_EN_KILLFOCUS(IDC_COMBOBOXEX, OnKillfocusComboboxex)
	ON_COMMAND(ID_INFOWND_DELETE, OnInfowndDelete)
	ON_COMMAND(ID_INFOWND_OPEN, OnInfowndOpen)
	ON_COMMAND(ID_NOTEMENU_ICONVIEW, OnNotemenuIconview)
	ON_COMMAND(ID_NOTEMENU_FIXEDSIZE, OnNotemenuFixedsize)
	ON_COMMAND(ID_NOTEMENU_AUTODISP, OnNotemenuMTrans)
	ON_COMMAND(ID_NOTEMENU_SMILEY1, OnNotemenuSmiley1)
	ON_COMMAND(ID_NOTEMENU_SMILEY2, OnNotemenuSmiley2)
	ON_COMMAND(ID_NOTEMENU_SMILEY3, OnNotemenuSmiley3)
	ON_COMMAND(ID_NOTEMENU_SMILEY4, OnNotemenuSmiley4)
	ON_COMMAND(ID_NOTEMENU_SMILEY5, OnNotemenuSmiley5)
	ON_COMMAND(ID_NOTEMENU_SMILEY6, OnNotemenuSmiley6)
	ON_COMMAND(ID_NOTEMENU_POS1, OnNotemenuPos1)
	ON_COMMAND(ID_NOTEMENU_POS2, OnNotemenuPos2)
	ON_COMMAND(ID_NOTEMENU_POS3, OnNotemenuPos3)
	ON_COMMAND(ID_NOTEMENU_SMILEYOFF, OnNotemenuSmileyOff)
	ON_COMMAND(ID_NOTEMENU_ATTACHFILE, OnNotemenuAttachfile)
	ON_COMMAND(ID_FZFOLDER, OnNotemenuFZFolder)
	ON_COMMAND(ID_NOTEMENU_MSGLOG, OnNotemenuMsglog)
	ON_COMMAND(ID_NOTEMENU_NEWSTYLE, OnNotemenuNewStyle)
	ON_COMMAND(ID_NOTEMENU_NEWFNT, OnNotemenuNewStyle)
	ON_COMMAND(ID_NOTEMENU_COPY, OnNotemenuCopy)
	ON_COMMAND(ID_NOTEMENU_CUT, OnNotemenuCut)
	ON_COMMAND(ID_NOTEMENU_PASTE, OnNotemenuPaste)
	ON_COMMAND(ID_NOTEMENU_LOADFROMFILE, OnNotemenuLoadfromfile)
	ON_BN_CLICKED(IDC_NOTE_SAFETOFILE, OnNotemenuSavetofile)
	ON_BN_CLICKED(IDC_ABOOK, OnAbook)
	ON_BN_CLICKED(IDC_NOTE_OPTMENU, OnNoteOptmenu)
	ON_EN_KILLFOCUS(IDC_TITLE, OnKillfocusTitle)
	ON_EN_CHANGE(IDC_TITLE, OnChangedTitle)
	ON_COMMAND(ID_NOTEMENU_PRINT, OnNotemenuPrint)
	ON_COMMAND(ID_NOTEMENU_SENDASEMAIL, OnNotemenuSendasemail)
	ON_COMMAND(ID_NOTEMENU_SNAPTOCONTENT, OnNotemenuSnaptocontent)
	ON_COMMAND(ID_NOTEMENU_ATTACH2WND, OnNotemenuA2W)
	ON_COMMAND(ID_NOTEMENU_SNAPTODERF, OnNotemenuRemembAsDef)
	ON_COMMAND(ID_NOTEMENU_SAVETOFILE, OnNotemenuSavetofile)
	ON_COMMAND(ID_NOTEMENU_SAVENOTES, OnNotemenuSavenote)
	ON_COMMAND(ID_NOTEMENU_REALLYDELETE, OnReallyDeleteNote)
	ON_BN_CLICKED(IDC_NOTE_DEL, OnDeleteNote)
	ON_WM_DROPFILES()
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(DELNOTE, DeleteNote)
	ON_MESSAGE(UPDATERECLIST, UpdateRecipientList)
	ON_EN_CHANGE(IDC_COMBOBOXEX, OnChangeComboboxex)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

CDLG_Note::CDLG_Note(CNoteSettings* pNoteParams)
	: CDialog()
{
	//{{AFX_DATA_INIT(CDLG_Note)
	//}}AFX_DATA_INIT
	bNoteTitleChanged=0;
	m_EditField.pLinkHandler=this;
	dwASTimer=0;
	bWasSemi=0;
	bStatusCacheUpdater=1;
	hNextClipboardChainWnd=0;
	iClipCount=0;
	lClipSpy=0;
	iSendLock=0;
	pNoteDlg=NULL;
	iNoteWndStatus=0;
	iRModeSwitcher=0;
	if(pNoteParams){
		Sets=*pNoteParams;
		delete pNoteParams;
	}else{
		Sets=objSettings.MessageNoteSettings;
	}
	m_pMenu=NULL;
	m_hBr = NULL;
	m_hBrTitle = NULL;
	m_hBrInfo = NULL;
	font=NULL;
	isMove=FALSE;
	MouseTracker=NULL;
	MouseTracker2=NULL;
	bStartHided=FALSE;
	bAnyDialogInProcess=0;
	dwLastChangeTime=0;
}

void CDLG_Note::RegHotkey()
{
	if(Sets.hHotkey.Present()){
		RegisterIHotKey(this->m_hWnd,1,Sets.hHotkey,Format("WNK3_%s",Sets.GetID()),"None");
	}
}

CDLG_Note::~CDLG_Note()
{
	deleteBrush(m_hBr);
	deleteBrush(m_hBrTitle);
	deleteBrush(m_hBrInfo);
	deleteFont(font);
	if(m_pMenu!=NULL){
		delete m_pMenu;
		m_pMenu=NULL;
	}
	if(MouseTracker!=NULL){
		TerminateThread(MouseTracker,0);
		CloseHandle(MouseTracker);
	}
	if(MouseTracker2!=NULL){
		TerminateThread(MouseTracker2,0);
		CloseHandle(MouseTracker2);
	}
	UnregisterIHotKey(this->m_hWnd,1);
	iNoteWndStatus=-1;
}

void FAR WINAPI OnNoteLeave(MouseTrackerRuler* pRuler)
{
	if(pRuler && pRuler->pWnd){
		((CDLG_Note*)pRuler->pWnd)->ShowWindow(SW_SHOW);
		pRuler->bStopWatching=FALSE;
	}
}

#define ALPHA_STEP	7
#define ALPHA_TIME	200
void FAR WINAPI OnNoteLeave2(MouseTrackerRuler* pRuler)
{
	// Из-за этого начинается мерцание если флаг не был установлен ранее
	// но при большой площади это лучше чем тормоза
	SetLayeredWindowStyle(pRuler->pWnd->GetSafeHwnd(),TRUE);
	if(pRuler && pRuler->pWnd){
		if(pRuler->dwAddFlag1<100 && pRuler->dwAddFlag1>0){
			DWORD dwTick=GetTickCount();
			double dDx=(100-pRuler->dwAddFlag1)/ALPHA_STEP;
			if(dDx>0){
				for(int i=ALPHA_STEP-1;i>0;i--){
					int iVal=pRuler->dwAddFlag1+int(dDx*i);
					if(pRuler->pSlider){
						pRuler->pSlider->SetPos(100-iVal);
					}
					SetWndAlpha(pRuler->pWnd->GetSafeHwnd(), iVal,TRUE);
					if(GetTickCount()-dwTick>ALPHA_TIME){
						break;
					}
					Sleep(20);
				}
			}
		}
		pRuler->bStopWatching=FALSE;
		if(pRuler->dwAddFlag1<100 && pRuler->dwAddFlag1>0){
			SetWndAlpha(pRuler->pWnd->GetSafeHwnd(), pRuler->dwAddFlag1,TRUE);
		}else{
			SetWndAlpha(pRuler->pWnd->GetSafeHwnd(), 100, FALSE);
		}
	}
}

void FAR WINAPI OnNoteLeave2Pre(MouseTrackerRuler* pRuler)
{
	if(pRuler && pRuler->pWnd){
		if(pRuler->dwAddFlag1){
			DWORD dwTick=GetTickCount();
			double dDx=(100-pRuler->dwAddFlag1)/ALPHA_STEP;
			for(int i=1;i<ALPHA_STEP;i++){
				SetWndAlpha(pRuler->pWnd->GetSafeHwnd(), pRuler->dwAddFlag1+int(dDx*i),TRUE);
				if(GetTickCount()-dwTick>ALPHA_TIME){
					break;
				}
				Sleep(20);
			}
		}
		pRuler->bStopWatching=FALSE;
		SetWndAlpha(pRuler->pWnd->GetSafeHwnd(), 100, TRUE);
	}
}

BOOL CDLG_Note::ExtractDataFromAttachment()
{
	if(Sets.sAttachment!=""){
		int iFrom=0;
		// Ищем иконку...
		while(iFrom>=0){
			CString sTextLineParams=CDataXMLSaver::GetInstringPart("["ICONPREFIX,"]",Sets.sAttachment,iFrom,XMLNoConversion);
			if(sTextLineParams!=""){
				if(sTextLineParams==SMILE1){
					Sets.SetIconNum(theApp.dwMsgBitmap+2);
					break;
				}
				if(sTextLineParams==SMILE2){
					Sets.SetIconNum(theApp.dwMsgBitmap+3);
					break;
				}
				if(sTextLineParams==SMILE3){
					Sets.SetIconNum(theApp.dwMsgBitmap+4);
					break;
				}
				if(sTextLineParams==SMILE4){
					Sets.SetIconNum(theApp.dwMsgBitmap+5);
					break;
				}
				if(sTextLineParams==SMILE5){
					Sets.SetIconNum(theApp.dwMsgBitmap+6);
					break;
				}
				if(sTextLineParams==SMILE6){
					Sets.SetIconNum(theApp.dwMsgBitmap+7);
					break;
				}
			}
		}
		// Если прислали напоминания/ToDo - поступаем как велит совесть...
		int iAttPosition=0;
		if(!(objSettings.bReceiveRemdsAction==1 && objSettings.bReceiveToDosAction==1)){
			DWORD dwAttSize=0;
			CString sAttName,sAttribs;
			while(GetAttachByIndex(iAttPosition,sAttName,dwAttSize,NULL,&sAttribs)){
				BOOL bResult=0;
				if(sAttribs=="wntype1"){
					if(objSettings.bReceiveToDosAction==3){
						bResult=1;
					}else if(objSettings.bReceiveToDosAction==0){
						long lAskRes=0;
						if(AskYesNo(_l("Do you want to accept ToDo,\nreceived from")+" '"+Sets.sReplyAddressName+"'?",&lAskRes,this)==IDYES){
							bResult=1;
						}
						if(lAskRes>0){
							if(lAskRes==1){
								objSettings.bReceiveToDosAction=2;
							}else{
								objSettings.bReceiveToDosAction=1;
							}
						}
					}else if(objSettings.bReceiveToDosAction==2){
						if(objSettings.AddrBook.IsPersonKnown(Sets.sReplyAddressName)!=-1){
							bResult=1;
						}
					}
				}
				if(sAttribs=="wntype3"){
					if(objSettings.bReceiveRemdsAction==3){
						bResult=1;
					}else if(objSettings.bReceiveRemdsAction==0){
						long lAskRes=0;
						if(AskYesNo(_l("Do you want to accept Reminder,\nreceived from")+" '"+Sets.sReplyAddressName+"'?",&lAskRes,this)==IDYES){
							bResult=1;
						}
						if(lAskRes>0){
							if(lAskRes==1){
								objSettings.bReceiveRemdsAction=2;
							}else{
								objSettings.bReceiveRemdsAction=1;
							}
						}
					}else if(objSettings.bReceiveRemdsAction==2){
						if(objSettings.AddrBook.IsPersonKnown(Sets.sReplyAddressName)!=-1){
							bResult=1;
						}
					}
				}
				if(bResult){
					SaveAttachToFile(iAttPosition,sAttribs);
					if(RemoveFileFromAttach(sAttName)){
						iAttPosition=0;//Все сначала, так как могло сбросится
					}
				}else{
					iAttPosition++;
				}
				sAttName=sAttribs="";
			}
			
		}
	}
	if(Sets.sItemIDToAttachAfterStart!=""){
		AddItemToAttachment(Sets.sItemIDToAttachAfterStart);
		Sets.sItemIDToAttachAfterStart="";
	}
	// Ищем приаттаченные файлы (могли уже появится)...
	RefreshAttachedFileList();
	return TRUE;
}

BOOL CALLBACK EditFocusHandler(LPVOID pControl)
{
	CDLG_Note* pNote=(CDLG_Note*)pControl;
	if(pNote){
		pNote->MarkAsRead();
	}
	return TRUE;
}

BOOL CDLG_Note::OnInitDialog()
{
	// Initializing note...
	if(objSettings.bShadows && !IsVista()){
		SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW//ModifyStyleEx(0,WS_EX_COMPOSITED,SWP_FRAMECHANGED);
	}
	CDialog::OnInitDialog();
	// CG: The following block was added by the ToolTips component.
	{
		GetDlgItem(IDC_COMBOBOXEX)->ModifyStyle(0,WS_BORDER);
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.SetMaxTipWidth(objSettings.rDesktopRectangle.Width()/2);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_ONTOP), "...");
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_RED), _l("Choose note color")+"\n"+_l("Right-click to cycle color"));
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_OPTMENU), _l("Note options"));
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_OPTION), _l("Change view"));
		m_tooltip.AddTool(GetDlgItem(IDC_COMBOBOXEX), _l("Recipient`s computer name or nick from address book")+"\n"+_l("Press 'Down' to popup")+" '"+_l("List of previous recipients")+"'");
		m_tooltip.AddTool(GetDlgItem(IDC_RECENTA), _l("List of previous recipients"));
		m_tooltip.AddTool(GetDlgItem(IDC_SENDMESSAGE), _l("Send message")+" (Ctrl-Enter)"+"\n"+_l("Press")+" Ctrl-Shift-Enter "+_l("to delete after delivery"));
		if(objSettings.bNoteDelButtonWay==1){
			m_tooltip.AddTool(GetDlgItem(IDC_NOTE_DEL), _l("Destroy this note")+" (Ctrl-D)");
		}else{
			m_tooltip.AddTool(GetDlgItem(IDC_NOTE_DEL), _l("Hide note")+" (Ctrl-H)");
		}
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_PRINTBT), _l("Print note")+" (Ctrl-P)");
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_FIXSIZE), _l("Remind later"));
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_SAFETOFILE), _l("Save content to file")+" (Ctrl-Shift-S)");
		m_tooltip.AddTool(GetDlgItem(IDC_ABOOK), _l("Message recipient")+"\n"+_l("Hold")+" Shift "+_l("to add multirecipients"));
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_ICON), _l("Icon and attachment"));
		//m_tooltip.AddTool(GetDlgItem(IDC_NOTE_HISTORY), _l("Messages history"));
		m_tooltip.AddTool(GetDlgItem(IDC_TITLE), Sets.GetTitle());
		m_tooltip.AddTool(GetDlgItem(IDC_NOTE_REPLY), _l("Quote text/Quick answer")+" (Ctrl-Q,1,2,3...)");
	}
	NoteTracker.pWnd=this;
	NoteTracker.MouseOutProc=OnNoteLeave;
	NoteTracker.dwTaskbarFriendly=0;
	NoteTracker.bStopWatching=FALSE;
	NoteTracker2.pWnd=this;
	NoteTracker2.MouseOutProc=OnNoteLeave2;
	NoteTracker2.MousePreProc=OnNoteLeave2Pre;
	NoteTracker2.dwTaskbarFriendly=0;
	NoteTracker2.bStopWatching=FALSE;
	NoteTracker2.bFocusFriendly=1;//objSettings.lNoteTranspFocuFriendly;
	BOOL bed=IsEmbedded();
	//
	ExtractDataFromAttachment();
	if(Sets.sText!=""){
		SetNoteText();
		if(!Sets.bSelectTextOnStart){
			m_EditField.SetSel(0,0);
		}else{
			m_EditField.SetSel(0,-1);
		}
	}
	{
		Sizer.InitSizer(this,objSettings.iStickPix);
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
		// Элементы
		Sizer.AddDialogElement(IDC_TEXTFIELD,0);
		Sizer.AddDialogElement(IDC_NOTE_ONTOP);//,widthSize|heightSize,IDC_NOTE_OPTION
		Sizer.AddDialogElement(IDC_NOTE_ICON,widthSize|heightSize,IDC_SENDMESSAGE);
		//Sizer.AddDialogElement(IDC_NOTE_HISTORY,widthSize|heightSize,IDC_SENDMESSAGE);
		Sizer.AddDialogElement(IDC_ABOOK,widthSize|heightSize|hidable,IDC_SENDMESSAGE);
		Sizer.AddDialogElement(IDC_NOTE_OPTION,widthSize|heightSize|fixedDimensions,objSettings.dwNoteButtonSize);
		Sizer.AddDialogElement(IDC_NOTE_RED,widthSize|heightSize|hidable,IDC_NOTE_OPTION);
		Sizer.AddDialogElement(IDC_NOTE_OPTMENU,widthSize|heightSize|hidable,IDC_NOTE_OPTION);
		Sizer.AddDialogElement(IDC_NOTE_REPLY,widthSize|heightSize,IDC_NOTE_OPTION);
		Sizer.AddDialogElement(IDC_NOTE_DEL,widthSize|heightSize,IDC_NOTE_OPTION);
		Sizer.AddDialogElement(IDC_NOTE_PRINTBT,widthSize|heightSize|hidable,IDC_NOTE_OPTION);
		Sizer.AddDialogElement(IDC_NOTE_FIXSIZE,widthSize|heightSize,IDC_NOTE_OPTION);
		Sizer.AddDialogElement(IDC_NOTE_SAFETOFILE,widthSize|heightSize|hidable,IDC_NOTE_OPTION);
		Sizer.AddDialogElement(IDC_SENDMESSAGE,widthSize|heightSize|fixedDimensions,int(objSettings.dwNoteButtonSize*1.5));
		Sizer.AddDialogElement(IDC_RECENTA,widthSize|heightSize,IDC_SENDMESSAGE);//|hidable
		//Sizer.AddDialogElement(IDC_FRAME,0);
		Sizer.AddDialogElement(IDC_COMBOBOXEX,0);
		Sizer.AddDialogElement(IDC_INFO,0);
		Sizer.AddDialogElement(IDC_TITLE,0);
		if(objSettings.bNoSend){
			GetDlgItem(IDC_SENDMESSAGE)->EnableWindow(FALSE);
		}
		////////////
		// Раскладка
		// Common
		MainRulePos=Sizer.AddGotoRule(1);
		//Sizer0
		Sizer.AddGotoLabel(0);
		Sizer.AddBottomAlign(IDC_TEXTFIELD,0,0);
		//Sizer.AddHideRule(IDC_FRAME);
		Sizer.AddHideRule(IDC_ABOOK);
		Sizer.AddHideRule(IDC_SENDMESSAGE);
		//Sizer.AddHideRule(IDC_NOTE_HISTORY);
		Sizer.AddHideRule(IDC_COMBOBOXEX);
		Sizer.AddHideRule(IDC_RECENTA);
		Sizer.AddHideRule(IDC_NOTE_REPLY);
		Sizer.AddHideRule(IDC_NOTE_OPTION);
		Sizer.AddHideRule(IDC_NOTE_ICON);
		Sizer.AddHideRule(IDC_INFO);
		if(!objSettings.bNoteOptButtonPos){
			Sizer.AddHideRule(IDC_NOTE_RED);
			Sizer.AddHideRule(IDC_NOTE_OPTMENU);
			Sizer.AddHideRule(IDC_NOTE_PRINTBT);
			Sizer.AddHideRule(IDC_NOTE_FIXSIZE);
			Sizer.AddHideRule(IDC_NOTE_SAFETOFILE);
		}else{
			Sizer.AddShowRule(IDC_NOTE_RED);
			Sizer.AddShowRule(IDC_NOTE_OPTMENU);
			Sizer.AddShowRule(IDC_NOTE_PRINTBT);
			Sizer.AddShowRule(IDC_NOTE_FIXSIZE);
			Sizer.AddShowRule(IDC_NOTE_SAFETOFILE);
		}
		if(objSettings.bNoteDelButtonPos){
			Sizer.AddShowRule(IDC_NOTE_DEL);
		}else{
			Sizer.AddHideRule(IDC_NOTE_DEL);
		}
		Sizer.AddGotoRule(100);
		//Sizer1
		Sizer.AddGotoLabel(1);
		Sizer.AddRightBottomAlign(IDC_NOTE_OPTION,0,0);
		Sizer.AddBottomAlign(IDC_TEXTFIELD,IDC_NOTE_OPTION,topPos,-1);
		Sizer.AddShowRule(IDC_NOTE_DEL);
		Sizer.AddShowRule(IDC_NOTE_RED);
		Sizer.AddShowRule(IDC_NOTE_OPTMENU);
		Sizer.AddShowRule(IDC_NOTE_PRINTBT);
		Sizer.AddShowRule(IDC_NOTE_FIXSIZE);
		Sizer.AddShowRule(IDC_NOTE_SAFETOFILE);
		Sizer.AddShowRule(IDC_NOTE_OPTION);
		Sizer.AddShowRule(IDC_INFO);
		if(IsMessage()){// Кпопка быстрого ответа видна всегда если сообщение
			Sizer.AddShowRule(IDC_NOTE_REPLY);
		}else{
			Sizer.AddHideRule(IDC_NOTE_REPLY);
		}
		Sizer.AddHideRule(IDC_NOTE_ICON);
		//Sizer.AddHideRule(IDC_FRAME);
		Sizer.AddHideRule(IDC_ABOOK);
		Sizer.AddHideRule(IDC_SENDMESSAGE);
		//Sizer.AddHideRule(IDC_NOTE_HISTORY);
		Sizer.AddHideRule(IDC_RECENTA);
		Sizer.AddHideRule(IDC_COMBOBOXEX);
		Sizer.AddGotoRule(100);
		//Sizer2
		Sizer.AddGotoLabel(2);
		//Sizer.AddShowRule(IDC_FRAME);
		if(!bed){
			Sizer.AddShowRule(IDC_ABOOK);
			Sizer.AddShowRule(IDC_NOTE_ICON);
		}else{
			Sizer.AddHideRule(IDC_ABOOK);
			Sizer.AddHideRule(IDC_NOTE_ICON);
		}
		Sizer.AddShowRule(IDC_SENDMESSAGE);
		Sizer.AddShowRule(IDC_COMBOBOXEX);
		Sizer.AddShowRule(IDC_RECENTA);
		Sizer.AddRightBottomAlign(IDC_SENDMESSAGE,0,0);
		Sizer.AddBottomAlign(IDC_NOTE_OPTION,IDC_SENDMESSAGE,topPos,-1);
		Sizer.AddBottomAlign(IDC_TEXTFIELD,IDC_NOTE_OPTION,topPos,-1);
		Sizer.AddShowRule(IDC_NOTE_DEL);
		Sizer.AddShowRule(IDC_NOTE_RED);
		Sizer.AddShowRule(IDC_NOTE_OPTMENU);
		Sizer.AddShowRule(IDC_NOTE_PRINTBT);
		Sizer.AddShowRule(IDC_NOTE_FIXSIZE);
		Sizer.AddShowRule(IDC_NOTE_SAFETOFILE);
		Sizer.AddShowRule(IDC_NOTE_OPTION);
		Sizer.AddShowRule(IDC_INFO);
		if(IsMessage()){// Кпопка быстрого ответа видна всегда если сообщение
			Sizer.AddShowRule(IDC_NOTE_REPLY);
		}else{
			Sizer.AddHideRule(IDC_NOTE_REPLY);
		}
		Sizer.AddGotoRule(100);
		//Sizer3
		Sizer.AddGotoLabel(3);
		if(objSettings.iSHowTitleInIconMode){
			Sizer.AddBottomAlign(IDC_NOTE_ONTOP,0,-1);
			Sizer.AddHeight2WidthAlign(IDC_NOTE_ONTOP,IDC_NOTE_ONTOP,0);
			Sizer.AddRightAlign(IDC_TITLE,0,0);
			Sizer.AddBottomAlign(IDC_TITLE,IDC_NOTE_ONTOP,bottomPos,0);
			Sizer.AddShowRule(IDC_NOTE_DEL);
		}else{
			Sizer.AddRightBottomAlign(IDC_NOTE_ONTOP,0,-1);
			Sizer.AddHideRule(IDC_NOTE_DEL);
		}
		Sizer.AddHideRule(IDC_INFO);
		Sizer.AddHideRule(IDC_NOTE_RED);
		Sizer.AddHideRule(IDC_NOTE_OPTMENU);
		Sizer.AddHideRule(IDC_NOTE_PRINTBT);
		Sizer.AddHideRule(IDC_NOTE_FIXSIZE);
		Sizer.AddHideRule(IDC_NOTE_SAFETOFILE);
		Sizer.AddBottomAlign(IDC_TEXTFIELD,0,0);
		Sizer.AddGotoRule(101);
		//Общее для не-Icon
		Sizer.AddGotoLabel(100);
		Sizer.AddHeightAlign(IDC_NOTE_ONTOP,IDC_NOTE_OPTION,0);
		Sizer.AddWidthAlign(IDC_NOTE_ONTOP,IDC_NOTE_OPTION,0);
		//Общее для всех
		Sizer.AddGotoLabel(101);
		int iRightElem=IDC_NOTE_SAFETOFILE, iLeftElem=IDC_NOTE_REPLY;
		Sizer.AddLeftTopAlign(IDC_NOTE_ONTOP,0,1);
		Sizer.AddLeftAlign(IDC_TEXTFIELD,0,0);
		Sizer.AddRightAlign	(IDC_TEXTFIELD,0,0);
		Sizer.AddRightTopAlign(IDC_NOTE_DEL,0,1);
		Sizer.AddTopAlign(IDC_TEXTFIELD,IDC_NOTE_ONTOP,bottomPos,3);
		if(IsMessage()){// Кпопка быстрого ответа видна всегда если сообщение
			Sizer.AddLeftAlign(IDC_NOTE_REPLY,0,0);
		}else{
			Sizer.AddRightAlign(IDC_NOTE_REPLY,0,leftPos,-1);
		}
		Sizer.AddRightAlign	(IDC_NOTE_OPTION,0,rightPos,0);
		Sizer.AddRightAlign	(IDC_SENDMESSAGE,0,rightPos,0);
		int iRightTitleElem=0;
		if(objSettings.bNoteDelButtonPos){
			Sizer.AddRightTopAlign(IDC_NOTE_DEL,0,1);
			iRightTitleElem=IDC_NOTE_DEL;
		}else{
			Sizer.AddTopAlign(IDC_NOTE_DEL,IDC_TEXTFIELD,bottomPos,1);
			Sizer.AddLeftAlign(IDC_NOTE_DEL,IDC_NOTE_REPLY,rightPos,1);
			iLeftElem=IDC_NOTE_DEL;
		}
		if(objSettings.bNoteOptButtonPos){
			// New Style
			iRightElem=IDC_NOTE_OPTION;
			Sizer.AddTopAlign	(IDC_NOTE_RED,0,1);
			Sizer.AddTopAlign	(IDC_NOTE_OPTMENU,0,1);
			Sizer.AddTopAlign	(IDC_NOTE_PRINTBT,0,1);
			Sizer.AddTopAlign	(IDC_NOTE_FIXSIZE,0,1);
			Sizer.AddTopAlign	(IDC_NOTE_SAFETOFILE,0,1);
			if(iRightTitleElem!=0){
				Sizer.AddRightAlign	(IDC_NOTE_RED,iRightTitleElem,leftPos,-1);
			}else{
				Sizer.AddRightAlign	(IDC_NOTE_RED,0,0);
			}
			Sizer.AddRightAlign	(IDC_NOTE_OPTMENU,IDC_NOTE_RED,leftPos,-1);
			Sizer.AddRightAlign	(IDC_NOTE_PRINTBT,IDC_NOTE_OPTMENU,leftPos,-1);
			Sizer.AddRightAlign	(IDC_NOTE_FIXSIZE,IDC_NOTE_PRINTBT,leftPos,-1);
			Sizer.AddRightAlign	(IDC_NOTE_SAFETOFILE,IDC_NOTE_FIXSIZE,leftPos,-1);
			iRightTitleElem=IDC_NOTE_SAFETOFILE;
		}else{
			// Old style
			Sizer.AddTopAlign	(IDC_NOTE_RED,IDC_TEXTFIELD,bottomPos,1);
			Sizer.AddTopAlign	(IDC_NOTE_OPTMENU,IDC_TEXTFIELD,bottomPos,1);
			Sizer.AddTopAlign	(IDC_NOTE_PRINTBT,IDC_TEXTFIELD,bottomPos,1);
			Sizer.AddTopAlign	(IDC_NOTE_FIXSIZE,IDC_TEXTFIELD,bottomPos,1);
			Sizer.AddTopAlign	(IDC_NOTE_SAFETOFILE,IDC_TEXTFIELD,bottomPos,1);
			Sizer.AddRightAlign	(IDC_NOTE_RED,IDC_NOTE_OPTION,leftPos,-1);
			Sizer.AddRightAlign	(IDC_NOTE_OPTMENU,IDC_NOTE_RED,leftPos,-1);
			Sizer.AddRightAlign	(IDC_NOTE_PRINTBT,IDC_NOTE_OPTMENU,leftPos,-1);
			Sizer.AddRightAlign	(IDC_NOTE_FIXSIZE,IDC_NOTE_PRINTBT,leftPos,-1);
			Sizer.AddRightAlign	(IDC_NOTE_SAFETOFILE,IDC_NOTE_FIXSIZE,leftPos,-1);
		}
		if(iRightTitleElem!=0){
			Sizer.AddRightAlign(IDC_TITLE,iRightTitleElem,leftPos,-1);
		}else{
			Sizer.AddRightAlign(IDC_TITLE,0,0);
		}
		Sizer.AddTopAlign	(IDC_NOTE_OPTION,IDC_TEXTFIELD,bottomPos,1);
		Sizer.AddTopAlign	(IDC_NOTE_REPLY,IDC_TEXTFIELD,bottomPos,1);
		Sizer.AddTopAlign	(IDC_INFO,IDC_NOTE_OPTION,topPos,0);
		Sizer.AddBottomAlign(IDC_INFO,IDC_NOTE_OPTION,bottomPos,0);
		Sizer.AddTopAlign	(IDC_SENDMESSAGE,IDC_NOTE_OPTION,bottomPos,1);
		Sizer.AddTopAlign	(IDC_ABOOK,IDC_SENDMESSAGE,0);
		Sizer.AddTopAlign	(IDC_NOTE_ICON,IDC_SENDMESSAGE,0);
		//Sizer.AddTopAlign	(IDC_NOTE_HISTORY,IDC_SENDMESSAGE,0);
		Sizer.AddTopAlign	(IDC_RECENTA,IDC_SENDMESSAGE,topPos,0);
		
		if(!bed){
			Sizer.AddLeftAlign	(IDC_NOTE_ICON,0,0);
			Sizer.AddLeftAlign	(IDC_ABOOK,IDC_NOTE_ICON,rightPos,1);
			Sizer.AddLeftAlign	(IDC_RECENTA,IDC_ABOOK,rightPos,1);
		}else{
			Sizer.AddLeftAlign	(IDC_RECENTA,0,0);
			//Sizer.AddLeftAlign	(IDC_ABOOK,0,rightPos,1);
			Sizer.AddHideRule(IDC_ABOOK);
			Sizer.AddHideRule(IDC_NOTE_ICON);
			Sizer.AddHideRule(IDC_NOTE_REPLY);
		}
		Sizer.AddLeftAlign	(IDC_COMBOBOXEX,IDC_RECENTA,rightPos,4);
		Sizer.AddRightAlign	(IDC_COMBOBOXEX,IDC_SENDMESSAGE,leftPos,-4);

		Sizer.AddLeftAlign	(IDC_TITLE,IDC_NOTE_ONTOP,rightPos,2);
		Sizer.AddTopAlign	(IDC_TITLE,IDC_NOTE_ONTOP,topPos,1);
		Sizer.AddBottomAlign(IDC_TITLE,IDC_NOTE_ONTOP,bottomPos,-1);
		Sizer.AddTopAlign	(IDC_COMBOBOXEX,IDC_SENDMESSAGE,topPos,4);
		Sizer.AddBottomAlign(IDC_COMBOBOXEX,IDC_SENDMESSAGE,bottomPos,-4);
		Sizer.AddTopAlign	(IDC_RECENTA,IDC_SENDMESSAGE,topPos,0);
		Sizer.AddBottomAlign(IDC_RECENTA,IDC_SENDMESSAGE,bottomPos,0);

		Sizer.AddLeftAlign	(IDC_INFO,iLeftElem,rightPos,3);
		Sizer.AddRightAlign	(IDC_INFO,iRightElem,leftPos,-3);
	}
	if(strlen(Sets.GetTitle())==0){
		COleDateTime dtNow=COleDateTime::GetCurrentTime();
		SetTitle(dtNow.Format("["LOGTIMEFRMT"]"));
	};
	if(IsMessage()){
		if(objSettings.lRecentUserByDef && Sets.sReplyAddressName.GetLength()==0 && objSettings.lMsgRecentList.GetSize()>0){
			Sets.sReplyAddressName=objSettings.lMsgRecentList[0];
		}
	}else{
		objSettings.lStatNotesCreated++;
	}
	UpdateRecipientList();
	//
	m_Ontop.SetStyles(CBS_FLAT);
	m_Ontop.SetBrush(RGB(255,255,255));
	if(objSettings.bNoSend){
		m_Send.SetBitmap(IDB_BM_NEWNOTE);
	}else{
		m_Send.SetBitmap(IDB_BM_MSG_SEND);
	}
	m_Send.SetStyles(CBS_FLAT);
	//m_NoteHistory.SetBitmap(IDB_BM_MSGLOG);
	//m_NoteHistory.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_ABook.SetBitmap(IDB_BM_ADRBOOK);
	m_ABook.SetStyles(CBS_FLAT);
	m_NoteIcon.SetStyles(CBS_FLAT | CBS_HIQUAL | CBS_LTTRANS | CBS_BORDER | CBS_CENTLES);
	m_PrintNote.SetBitmap(IDB_BM_PRINT);
	m_PrintNote.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_RemindLater.SetBitmap(IDB_BM_NEWREMAINDER);
	m_RemindLater.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_SaveToFile.SetBitmap(IDB_BM_SAVE);
	m_SaveToFile.SetStyles(CBS_FLAT|CBS_HIQUAL);
	if(objSettings.bUnderWindows98){
		m_DestroyNote.SetBitmap(IDB_BM_DEL);
		m_Reply.SetBitmap(IDB_BM_NOTEREPL);
	}else{
		m_DestroyNote.SetBitmap(IDB_BM_DELPERSON);
		m_Reply.SetBitmap(IDB_BM_NOTEREPL);
	}
	m_DestroyNote.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_Reply.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_OptionsDialog.SetBitmap(IDB_BM_OPTIONS);
	m_OptionsDialog.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_ColorRed.SetBitmap(IDB_BM_CHANGECOL);
	m_ColorRed.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_Option.SetStyles(CBS_FLAT|CBS_HIQUAL);
	//m_NoteIcon.SetBitmap(_bmp().GetRaw(IDB_BM_SKREPKA),CBS_FOREG_N);
	//m_Ontop.SetBitmap(_bmp().GetRaw(IDB_BM_SKREPKA),CBS_FOREG_N);//,CSize(DEF_BIGBMPSIZE,DEF_BIGBMPSIZE)
	m_RecentList.SetBitmap(IDB_BM_RECENT);
	m_RecentList.SetStyles(CBS_FLAT);
	BOOL bTrayIconHided=TRUE;
	// При открытии не показываем записки пришпиленные к приложениям
	bStartHided=!CanThisNoteBeShown();
	if(Sets.bHidden){
		bStartHided=TRUE;
	}
	// Если это входящая мессага, то сообщаем об этом Message centerу...
	BOOL bIncomingMessage=(IsMessage() && Sets.bIncomingMessage>0);
	if(bIncomingMessage){
		BOOL bOpenType=Sets.bIncMsgOpenType;
		if(bOpenType==0){
			bOpenType=objSettings.bOnNewMessage;
		}
		if(bOpenType==1){
			bTrayIconHided=FALSE;
		}
		if(bOpenType<2){
			bStartHided=TRUE;
		}
		if(bOpenType==3){
			Sets.iIconMode=1;
		}else{
			Sets.iIconMode=0;
		}
	}
	// Иконка в трее
	HICON hIcon=CreateIconFromBitmap(_bmp().GetRaw(GetNoteIcon()));
	m_STray.Create(NULL, WM_USER, Sets.GetTitle(), hIcon,IDR_INFOWND_MENU,this,bTrayIconHided);
	ClearIcon(hIcon);
	m_STray.SetMenuDefaultItem(0, TRUE);
	m_STray.pdwDefTICONAction=&objSettings.UseBblClick;
	if(Sets.iIconMode){
		Sets.bNeedFocusOnStart=FALSE;
	}
	Sets.Attach();
	SetParent(NULL);
	SetOwner(NULL);
	iNoteWndStatus=1;
	InitSizeAndPos();
	BOOL bNeedAnimation=(!bStartHided && bIncomingMessage);
	SetColorCode(Sets.GetColorCode());
	ActivateNoteParams(!bStartHided,bNeedAnimation,1);
	Sets.SaveToFile();
	m_EditField.pFocusHandler=EditFocusHandler;
	m_EditField.pFocusHandlerData=LPVOID(this);
	::SetProp(GetSafeHwnd(),"WKP_INVIS",(HANDLE)1);
	RegHotkey();
	return Sets.bNeedFocusOnStart;
}

void CDLG_Note::AddEmotionsMenu(CMenu& pSmiles)
{
	AddMenuString(&pSmiles,ID_NOTEMENU_SMILEY1,SMILE1,0,Sets.sAttachment.Find(Format("["ICONPREFIX"%s]",SMILE1))!=-1);
	AddMenuString(&pSmiles,ID_NOTEMENU_SMILEY2,SMILE2,0,Sets.sAttachment.Find(Format("["ICONPREFIX"%s]",SMILE2))!=-1);
	AddMenuString(&pSmiles,ID_NOTEMENU_SMILEY3,SMILE3,0,Sets.sAttachment.Find(Format("["ICONPREFIX"%s]",SMILE3))!=-1);
	AddMenuString(&pSmiles,ID_NOTEMENU_SMILEY4,SMILE4,0,Sets.sAttachment.Find(Format("["ICONPREFIX"%s]",SMILE4))!=-1);
	AddMenuString(&pSmiles,ID_NOTEMENU_SMILEY5,SMILE5,0,Sets.sAttachment.Find(Format("["ICONPREFIX"%s]",SMILE5))!=-1);
	AddMenuString(&pSmiles,ID_NOTEMENU_SMILEY6,SMILE6,0,Sets.sAttachment.Find(Format("["ICONPREFIX"%s]",SMILE6))!=-1);
	pSmiles.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(&pSmiles,ID_NOTEMENU_POS1,_l("Send as 'Full'"),0,Sets.sAttachment.Find(Format("["POSPREFIX"%s]",POS1))!=-1);
	AddMenuString(&pSmiles,ID_NOTEMENU_POS2,_l("Send as 'Iconic'"),0,Sets.sAttachment.Find(Format("["POSPREFIX"%s]",POS2))!=-1);
	AddMenuString(&pSmiles,ID_NOTEMENU_POS3,_l("Send to 'Tray'"),0,Sets.sAttachment.Find(Format("["POSPREFIX"%s]",POS3))!=-1);
	pSmiles.AppendMenu(MF_SEPARATOR, 0, "");
	AddMenuString(&pSmiles,ID_NOTEMENU_SMILEYOFF,_l("Remove all"));
	return;
}

void CDLG_Note::AddAttachmentsMenu(CMenu& pAttachments, BOOL bFullView)
{
	// Смотрим, есть ли аттачи...
	int dwAttFiles=attachmentFileNames.GetSize();
	if(dwAttFiles>0){
		// Сохранить приаттаченное...
		for(int i=0;i<dwAttFiles;i++){
			AddMenuString(&pAttachments,WM_USER+ATTACHMENT_SAVE+i,Format("&%i. %s",i+1,attachmentFileNames[i]));
		}
		// Удалить приаттаченное...
		CMenu pAttachmentsRemove;
		pAttachmentsRemove.CreatePopupMenu();
		for(i=0;i<dwAttFiles;i++){
			AddMenuString(&pAttachmentsRemove,WM_USER+ATTACHMENT_REM+i,Format("%i. %s",i+1,attachmentFileNames[i]));
		}
		AddMenuString(&pAttachmentsRemove,ID_NOTEMENU_REMOVEALLATTACHMENTS,_l("Remove all attachments"));
		AddMenuSubmenu(&pAttachments,&pAttachmentsRemove,_l("Remove attachment")+"\t...",_bmp().Get(IDB_BM_DEL));
	}
	if(bFullView && pMainDialogWindow){
		CMenu NewMenu;
		NewMenu.CreatePopupMenu();
		AddMenuString(&NewMenu,ID_NOTEMENU_ATTACHFILE,_l("Add file")+"\t...",_bmp().Get(_IL(5)));
		pMainDialogWindow->CreateMenuTree(NewMenu, "", 0,100,TYPEMASK_REMD|TYPEMASK_TODO);
		AddMenuSubmenu(&pAttachments,&NewMenu,_l("Add attach")+"\t...");
	}
	return;
}

void CDLG_Note::RefreshMenu(BOOL bExtended)
{
	if(bExtended!=-1){
		if(m_pMenu!=NULL){
			delete m_pMenu;
			m_pMenu=NULL;
		}
		m_pMenu=new CMenu();
		m_pMenu->CreatePopupMenu();
		AddMenuString(m_pMenu,0,Sets.GetTitle(DEF_MENUTITLELEN)+(Sets.sText.GetLength()>10?Format("\t%i%s",Sets.sText.GetLength(),_l("Chars_short","ch.")):""),_bmp().Get(GetNoteIcon(FALSE)),FALSE,NULL,FALSE,TRUE);
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	}
	if(bExtended==TRUE){
		AddMenuString(m_pMenu,ID_NOTEMENU_SHOWNOTE,_l("Show note"),_bmp().Get(_IL(6)));
		m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	}
	if(Sets.iIconMode){
		AddMenuString(m_pMenu,ID_NOTEMENU_BACKNORMALVIEW,_l("Return to full view"),_bmp().Get(_IL(6)));
	}
	AddMenuString(m_pMenu,ID_NOTEMENU_COPY,_l("Copy")+"\tCtrl-C",_bmp().Get(_IL(43)));
	AddMenuString(m_pMenu,ID_NOTEMENU_CUT,_l("Cut")+"\tCtrl-X",_bmp().Get(_IL(46)));
	AddMenuString(m_pMenu,ID_NOTEMENU_PASTE,_l("Paste")+"\tCtrl-V",_bmp().Get(_IL(31)));
	AddMenuString(m_pMenu,IDC_NOTE_COPYTOCL,_l("Copy all to clipboard")+"\tCtrl-A",_bmp().Get(_IL(43)));
	AddMenuString(m_pMenu,ID_NOTEMENU_PINMENU,_l("Pin to window/document"),_bmp().Get(IDB_PINNED_UP_APP));
	AddMenuString(m_pMenu,ID_NOTEMENU_SEARCH,_l("Search in Google")+"\tCtrl-G",_bmp().Get(_IL(106)));
	// Save note
	AddMenuString(m_pMenu,ID_NOTEMENU_SAVENOTES,_l("Save note")+"\tCtrl-S",_bmp().Get(IDB_BM_SAVE));
	m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	{// Actions
		CMenu pActions;
		pActions.CreatePopupMenu();
		AddMenuString(&pActions,ID_NOTEMENU_HIDENOTE,_l("Hide note")+"\tCtrl-H",_bmp().Get(_IL(51)));
		AddMenuString(&pActions,ID_NOTEMENU_CLIPSPY,_l("Spy clipboard")+"\tCtrl-K",0,lClipSpy);
		AddMenuString(&pActions,ID_NOTEMENU_EXEASSCR,_l("Execute as Script")+"\tCtrl-J");
		AddMenuString(&pActions,ID_NOTEMENU_SNAPTOCONTENT,_l("Snap size to content")+"\tCtrl-U");
		if(!Sets.iIconMode){
			AddMenuString(&pActions,ID_NOTEMENU_SNAPTODERF,_l("Set this size as default"));
		}
		//AddMenuString(&pActions,ID_NOTEMENU_BRINGALLONTOP,_l("Bring all notes to top")+getActionHotKeyDsc(SHOW_ALLNOTES));//theApp.dwImagesFromBitmapHolder+50
		AddMenuString(&pActions,ID_NOTEMENU_ATTACH2WND,_l("Attach to window"));
		AddMenuString(&pActions,ID_NOTEMENU_PRINT,_l("Print note")+"\tCtrl-P",_bmp().Get((IDB_BM_PRINT)));
		if(!IsEmbedded()){
			AddMenuString(&pActions,ID_NOTEMENU_ASSIGNHOTKEY,_l("Assign hotkey"),_bmp().Get(_IL(55)));
			{// Remind
				CMenu pRemind;
				pRemind.CreatePopupMenu();
				AddMenuString(&pRemind, ID_NOTEMENU_REMINDIN15MIN, _l("Remind in 15 min"));
				AddMenuString(&pRemind, ID_NOTEMENU_REMINDIN30MIN, _l("Remind in 30 min"));
				AddMenuString(&pRemind, ID_NOTEMENU_REMINDIN1H, _l("Remind in 1h"));
				AddMenuString(&pRemind, ID_NOTEMENU_REMINDAFTER3H, _l("Remind in 3h"));
				AddMenuString(&pRemind, ID_NOTEMENU_REMINDTOMORROW, _l("Remind in")+"\t...");
				AddMenuSubmenu(&pActions,&pRemind,_l("Remind later")+"\t...",_bmp().Get(IDB_BM_NEWREMAINDER));
			}
		}
		pActions.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&pActions,ID_NOTEMENU_LOADFROMFILE,_l("Load text from file")+"\tCtrl-Shift-L",_bmp().Get(_IL(5)));
		AddMenuString(&pActions,ID_NOTEMENU_SAVETOFILE,_l("Save text to file")+"\tCtrl-Shift-S",_bmp().Get((IDB_BM_SAVE)));
		if(Sets.bIncomingMessage){
			pActions.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&pActions,ID_NOTEMENU_MESSAGEFILTER,_l("Add message filter"));
		}
		AddMenuSubmenu(m_pMenu,&pActions,_l("Actions")+"\t...",_bmp().Get(_IL(54)));
	}
	if(!IsEmbedded()){//Messaging
		CMenu pMessaging;
		pMessaging.CreatePopupMenu();
		// Сообщения и адресная книга
		int iPersonNumber=GetRecipientPerson();
		if(Sets.sReplyAddressName.GetLength()>0){
			if(iPersonNumber==-1){
				AddMenuString(&pMessaging,IDC_ADDTOADRBOOK,_l("Add to address book"),_bmp().Get(IDB_BM_ADRBOOK));
			}else{
				AddMenuString(&pMessaging,IDC_ADDTOADRBOOK,_l("View recipient info"),_bmp().Get(IDB_BM_ADRBOOK));
			}
			if(objSettings.SaveMesNotesByDef){
				AddMenuString(&pMessaging,ID_NOTEMENU_MSGLOG,_l("Message history")+"\tCtrl-L",_bmp().Get(IDB_BM_MSGLOG));
			}
		}
		if(objSettings.sFBDir!=""){
			AddMenuString(&pMessaging,ID_FZFOLDER,_l("Shared folder"),_bmp().Get(_IL(5)));
			AppendBBMenu(&pMessaging,Sets.IsShared());
		}
		if(!objSettings.bNoSend){
			if(objSettings.AddrBook.GetSize()>0){
				CMenu* pSendToMenu=new CMenu;
				pSendToMenu->CreatePopupMenu();
				RefreshRecipientMenu(NULL, pSendToMenu);
				AddMenuSubmenu(&pMessaging,pSendToMenu,_l("Send to")+"\t...",_bmp().Get(IDB_BM_NEWMESSAGE));
				delete pSendToMenu;
			}
		}
		if(Sets.sReplyAddressName.GetLength()>0 || IsMessage() || Sets.sAttachment!=""){
			// Attachments
			CMenu pAttachments;
			pAttachments.CreatePopupMenu();
			AddAttachmentsMenu(pAttachments,FALSE);
			AddMenuSubmenu(&pMessaging,&pAttachments,_l("Attachments")+"\t...",_bmp().Get("DDOWN"));
			// Это может быть сообщение, есть смысл добавить Смайлики
			CMenu pSmiles;
			pSmiles.CreatePopupMenu();
			AddEmotionsMenu(pSmiles);
			int iIcon=theApp.dwMsgBitmap+2;
			AddMenuSubmenu(&pMessaging,&pSmiles,_l("Ext.options")+"\t...",_bmp().Get(GetNoteIcon(0,&iIcon)));
		}
		//AddMenuString(&pMessaging,ID_NOTEMENU_SENDASEMAIL,_l("Send as email"),_bmp().Get(_IL(44)));
		if(Sets.bIncomingMessage){
			pMessaging.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&pMessaging,ID_NOTEMENU_MESSAGEFILTER,_l("Add message filter"));
		}
		if(pMessaging.GetMenuItemCount()>0){
			AddMenuSubmenu(m_pMenu,&pMessaging,_l("Messaging")+"\t...",_bmp().Get(IDB_BM_NEWMESSAGESM));
		}
	}
	// Режимы просмотра...
	//m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	{// Options
		CMenu pOptions;
		pOptions.CreatePopupMenu();
		AddOptionsSubMenu(&pOptions);
		AddMenuSubmenu(m_pMenu,&pOptions,_l("Note options")+"\t...",_bmp().Get(IDB_BM_OPTIONS));
	}
	CMenu pViewMenu;
	pViewMenu.CreatePopupMenu();
	AddMenuString(&pViewMenu,ID_NOTEMENU_ICONVIEW,_l("Iconic")+"\tCtrl-I",NULL,Sets.iIconMode);
	AddMenuString(&pViewMenu,ID_NOTEMENU_SMALLVIEW,_l("Minimal"),NULL,(Sets.iViewMode==0)&&!Sets.iIconMode);
	AddMenuString(&pViewMenu,ID_NOTEMENU_BUTTONVIEW,_l("Buttons"),NULL,(Sets.iViewMode==1)&&!Sets.iIconMode);
	//if(!IsEmbedded())
	{
		AddMenuString(&pViewMenu,ID_NOTEMENU_EXTENDEDVIEW,_l("Extended")+"\tCtrl-M",NULL,(Sets.iViewMode==2)&&!Sets.iIconMode);
	}
	AddMenuSubmenu(m_pMenu,&pViewMenu,_l("Change view")+"\t...",_bmp().Get(_IL(61)));
	AppendStylesMenu(m_pMenu,&Sets);
	AppendSmiles(*m_pMenu);
	// Del note
	AppendAdverts(m_pMenu);
	AddMenuString(m_pMenu,ID_NOTEMENU_REALLYDELETE,_l("Delete")+"\tCtrl-D",_bmp().Get(IDB_BM_DEL));
}

CStringArray sTopLevelWindowsTitles;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	HWND hPar=GetParent(hwnd);
	DWORD dwStyleEx=GetWindowLong(hwnd,GWL_EXSTYLE);
	DWORD dwStyle=GetWindowLong(hwnd,GWL_STYLE);
	if(!hPar && ::IsWindowEnabled(hwnd) && ::IsWindowVisible(hwnd) && (dwStyleEx & WS_EX_TOOLWINDOW)==0 && (dwStyle & WS_POPUP)==0){
		char szBuffer[1024]="";
		SendMessage(hwnd,WM_GETTEXT,(WPARAM)sizeof(szBuffer),(LPARAM)szBuffer);
		if(strlen(szBuffer)>0){
			sTopLevelWindowsTitles.Add(szBuffer);
		}
	}
	return TRUE;
}

// Иконка из списка сохраненных Bitmap-ов!
CString CDLG_Note::GetNoteIcon(BOOL bBig,int* iPrevaleIcon)
{
	CString sIcon=_IL(iPrevaleIcon?(*iPrevaleIcon):Sets.GetIconNum(),bBig?BIGLIST_ID:NULL);
	return sIcon;
}

void CDLG_Note::RefreshOnTopMenu()
{
	if(m_pMenu!=NULL){
		delete m_pMenu;
		m_pMenu=NULL;
	}
	m_pMenu=new CMenu();
	m_pMenu->CreatePopupMenu();
	AddMenuString(m_pMenu,WM_USER,Sets.GetTitle(DEF_MENUTITLELEN),_bmp().Get(GetNoteIcon(FALSE)),FALSE,NULL,FALSE,!Sets.iIconMode);
	m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
	if(Sets.iIconMode){
		RefreshMenu(-1);
	}else{
		if(Sets.bTopMost){
			AddMenuString(m_pMenu,ID_NOTEMENU_UNPINNOTE,_l("Detach note"),_bmp().Get(IDB_PINNED_DOWN),FALSE,_bmp().Get(IDB_PINNED_DOWN));
		}else{
			AddMenuString(m_pMenu,ID_NOTEMENU_PINNOTE,_l("Attach note"),_bmp().Get(IDB_PINNED_UP),FALSE,_bmp().Get(IDB_PINNED_UP));
		}
		CMenu pPinToWnd;
		pPinToWnd.CreatePopupMenu();
		AddMenuString(&pPinToWnd,ID_NOTEMENU_PINTOUNDERLAINGWINDOW,_l("Attach to current window"),_bmp().Get(IDB_PINNED_UP_APP));
		// Двигаемся по активным приложениям
		sTopLevelWindowsTitles.RemoveAll();
		if(::EnumWindows(EnumWindowsProc,0)){
			pPinToWnd.AppendMenu(MF_SEPARATOR, 0, "");
			for(int i=0;i<sTopLevelWindowsTitles.GetSize();i++){
				AddMenuString(&pPinToWnd,WM_USER+PINTOWINDOW+i,sTopLevelWindowsTitles[i],NULL,Sets.TestTitleMatchCondition_Raw(sTopLevelWindowsTitles[i]));
			}
		}
		AddMenuSubmenu(m_pMenu,&pPinToWnd,_l("Attach to window")+"\t...",_bmp().Get(IDB_PINNED_UP_APP));
		AddMenuString(m_pMenu,IDC_NOTE_OPTDIALOG,_l("Edit manually")+"\tCtrl-O",_bmp().Get(IDB_BM_OPTIONS));
	}
}

void CDLG_Note::InitAutoSend(BOOL bSimpleStop)
{
	BOOL bForceResend=0;
	if(dwASTimer){
		if(bSimpleStop==0){
			// Не доставлено сообщение которое уже автоотправлялось. не спрашиваем повторно
			bForceResend=1;
		}
		KillTimer(dwASTimer);
		dwASTimer=0;
	}
	if(bSimpleStop==0){
		if(bForceResend==0){
			if(AskYesNo(_l("Your message has not been sent.\nDo you wish to resend message\nafter a while (automatically)")+"?",_l("Warning"),&objSettings.bAskToResendMsg,this)!=IDYES){
				return;
			}
		}
		dwASTimer=SetTimer(ID_RESENDTIMER,30000,0);
		dwASTimerStart=GetTickCount();
		PostMessage(WM_TIMER,ID_RESENDTIMER,0);
	}
}

void CDLG_Note::SetNoteInfo(DWORD* dwColor, const char* szText)
{
	if(dwColor==0 && szText==0){
		if(bStatusCacheUpdater==0){
			return;
		}
		// Убиваем и таймер
		InitAutoSend(1);
		bStatusCacheUpdater=0;
	}else{
		bStatusCacheUpdater=1;
	}
	if(dwColor){
		dwInfoBgColor=*dwColor;
	}else{
		dwInfoBgColor=m_dwBrTitle;
	}
	deleteBrush(m_hBrInfo);
	m_hBrInfo=new CBrush(dwInfoBgColor);
	if(!szText){
		szText="";
	}
	GetDlgItem(IDC_INFO)->SetWindowText(szText);
	GetDlgItem(IDC_INFO)->Invalidate();
}

HBRUSH CDLG_Note::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	DWORD dwCol=objSettings.objSchemes.getTxtColor(Sets.GetColorCode());
	pDC->SetTextColor(dwCol);
	if(Sets.iIconMode){
		pDC->SetBkColor(objSettings.objSchemes.getBgColor(Sets.GetColorCode()));
		return HBRUSH(*m_hBr);
	}
	DWORD dwID=pWnd->GetDlgCtrlID();
	if(dwID==IDC_INFO){
		if(m_hBrInfo!=NULL){
			pDC->SetTextColor(RGB(255,255,255));
			pDC->SetBkColor(dwInfoBgColor);
			return HBRUSH(*m_hBrInfo);
		}
	}
	if(dwID==IDC_TITLE || dwID==IDC_COMBOBOXEX || dwID==IDC_INFO){
		pDC->SetBkColor(m_dwBrTitle);
		return HBRUSH(*m_hBrTitle);
	}else{
		pDC->SetBkColor(objSettings.objSchemes.getBgColor(Sets.GetColorCode()));
		return HBRUSH(*m_hBr);
	}
}

// Без учета параметров заметки!
BOOL CDLG_Note::ActivateWindow(BOOL bTop, BOOL bShow)
{
	if(iNoteWndStatus<=0){
		return FALSE;
	}
	if(bTop==-1){
		bTop=Sets.bTopMost;
	}
	if(Sets.iIconMode){
		m_Title.ModifyStyle(0,ES_MULTILINE);
		m_Title.EnableWindow(FALSE);
		m_Title.ShowWindow(SW_HIDE);
	}else{
		m_Title.ModifyStyle(ES_MULTILINE,0);
		m_Title.EnableWindow(TRUE);
		m_Title.ShowWindow(SW_SHOW);
	}
	BOOL bBorderChanged=0;
	if(Sets.iIconMode || Sets.bFixedSize){
		if((GetStyle()&WS_SIZEBOX)!=0){
			bBorderChanged=TRUE;
			Sizer.lClientOffsetL=GetSystemMetrics(SM_CXBORDER);
			Sizer.lClientOffsetR=GetSystemMetrics(SM_CXBORDER);
			Sizer.lClientOffsetT=GetSystemMetrics(SM_CYBORDER);
			Sizer.lClientOffsetB=GetSystemMetrics(SM_CYBORDER);
			ModifyStyle(WS_SIZEBOX,WS_BORDER);
		}
	}else{
		if((GetStyle()&WS_BORDER)!=0){
			bBorderChanged=TRUE;
			Sizer.lClientOffsetL=GetSystemMetrics(SM_CXDLGFRAME);
			Sizer.lClientOffsetR=GetSystemMetrics(SM_CXDLGFRAME);
			Sizer.lClientOffsetT=GetSystemMetrics(SM_CYDLGFRAME);
			Sizer.lClientOffsetB=GetSystemMetrics(SM_CYDLGFRAME);
			ModifyStyle(WS_BORDER,WS_SIZEBOX);
		}
	}
	Sizer.bSkipResize=TRUE;
	BOOL bRes=SetWindowPos(bTop?&wndTopMost:&wndNoTopMost,Sizer.m_rMain.left,Sizer.m_rMain.top,Sizer.m_rMain.Width(),Sizer.m_rMain.Height(),SWP_NOSENDCHANGING);
	if(bShow!=-1){
		::ShowWindow(GetSafeHwnd(),bShow?(SW_SHOWNORMAL):(SW_HIDE));
		if(bShow>0){
			Sets.bHidden=FALSE;
			Invalidate();
		}
	}
	Sizer.bSkipResize=FALSE;
	// Туултипы и иконки...
	UpdateTooltips();
	CString sIcon=GetNoteIcon(TRUE);
	m_NoteIcon.SetBitmap(_bmp().GetRaw(sIcon));
	if(attachmentFileNames.GetSize()>0){
		m_NoteIcon.SetStyles(CBS_DDOWN | CBS_FLAT | CBS_HIQUAL | CBS_LTTRANS | CBS_CENTLES);
		if(Sets.iIconMode){
			m_Ontop.SetStyles(CBS_DDOWN | CBS_LTTRANS | CBS_NOBG);
		}else{
			m_Ontop.SetStyles(CBS_FLAT);
		}
	}else{
		m_NoteIcon.SetStyles(CBS_FLAT | CBS_HIQUAL | CBS_LTTRANS | CBS_CENTLES);
		if(Sets.iIconMode){
			m_Ontop.SetStyles(CBS_LTTRANS | CBS_NOBG);
		}else{
			m_Ontop.SetStyles(CBS_FLAT);
		}
	}
	if(!Sets.iIconMode){
		if(bTop && bRes){
			if(Sets.GetApplicationsList()==""){
				sIcon=_bmp().GetResName(IDB_PINNED_UP);
			}else{
				sIcon=_bmp().GetResName(IDB_PINNED_UP_APP);
			}
		}else{
			sIcon=_bmp().GetResName(IDB_PINNED_DOWN);
		}
	}
	m_Ontop.SetBitmap(_bmp().GetRaw(sIcon));
	if(Sets.iIconMode){
		m_DestroyNote.ShowWindow(SW_HIDE);
	}
	if(bBorderChanged){
		Sizer.ApplyLayout();
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_ERASENOW | RDW_FRAME);
	}
	objSettings.sLastIndexOfOpenedNote=Sets.GetID();
	return bRes;
}

BOOL CDLG_Note::SwitchOntopState(BOOL bCheckAsMouse)
{
	dwLastChangeTime=GetTickCount();
	if(!(bCheckAsMouse && isPressed(VK_CONTROL))){
		Sets.bTopMost=!Sets.bTopMost;
	}else{
		PinToUnderlaingWnd();
	}
	ActivateNoteParams();
	return TRUE;
}

BOOL CDLG_Note::OnNoteOntop()
{
	if(iNoteWndStatus<=0)
		return FALSE;
	if(Sets.iIconMode){
		return FALSE;
	}
	if(isPressed(VK_SHIFT)){
		OnNotemenuIconview();
		return TRUE;
	}
	return SwitchOntopState();
}

void CDLG_Note::OnNoteOption() 
{
	if(Sets.iViewMode==1){
		Sets.iViewMode=2;
	}else{
		Sets.iViewMode=1;
	}
	ActivateNoteParams();
}

DWORD WINAPI OpenAnimated(LPVOID p)
{
	CDLG_Note* pNote=(CDLG_Note*)p;
	HWND h=pNote->GetSafeHwnd();
	IAnimateWindow(h,1);
	if(IsWindow(h)){
		pNote->ApplyAllLayouts(TRUE);
	}
	return 0;
}

void CDLG_Note::ActivateNoteParams(BOOL bShow, BOOL bWithAnimation, BOOL bFully)
{
	if(iNoteWndStatus!=1){
		return;
	}
	SetTitle();
	if(::IsWindow(m_EditField.GetSafeHwnd())){
		if(Sets.GetReadOnly()){
			m_EditField.SetReadOnly();
		}else{
			m_EditField.SetReadOnly(FALSE);
		}
		m_EditField.bWordWrap=Sets.bWordWrap;
		if(Sets.bWordWrap){
			m_EditField.SetTargetDevice(NULL, 0);
		}else{
			m_EditField.SetTargetDevice(NULL, 1);
		}
	}
	if(bFully){
		SetNoteText(0);
	}
	if(::IsWindow(m_Option.GetSafeHwnd())){
		m_Option.SetBitmap(Sets.iViewMode==2?IDB_BM_EFORM1:IDB_BM_EFORM);
		//m_Option.SetText(Sets.iViewMode==2?"<":">");
	}
	if(Sets.bMouseTrans){
		::SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_TRANSPARENT);
	}else if((::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)&WS_EX_TRANSPARENT)){
		::SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)&~WS_EX_TRANSPARENT);
	}
	// Показываем/скрываем ...
	if(bWithAnimation){
		ApplyAllLayouts(FALSE);
		if(bShow){
			//bStartHided=TRUE;//Открываться будет после анимации!
			FORK(OpenAnimated,this)
			m_EditField.Invalidate(TRUE);
		}
	}else{
		ApplyAllLayouts(bShow);
	}
}

void CDLG_Note::OnNoteOptdialog()
{
	if(pNoteDlg!=NULL){
		return;
	}
	//
	if(bAnyDialogInProcess){
		return;
	}
	SimpleTracker Track(bAnyDialogInProcess);
	//
	CDLG_NoteSettings dlg(this);
	dlg.pItem=&Sets;
	pNoteDlg=&dlg;
	dlg.DoModal();
	if(this!=NULL){// :^)) Прикол - при закрытии записки может быть и такое...
		pNoteDlg=NULL;
		if(Sets.GetApplicationsList()!="" && !Sets.bTopMost){
			Sets.bTopMost=TRUE;
			ActivateNoteParams();
		}
	}
}

void CDLG_Note::RemindThisNote()
{
	HICON hIcon=CreateIconFromBitmap(_bmp().GetRaw(GetNoteIcon()));
	m_STray.SetIcon(hIcon);
	m_STray.ShowIcon();
	ClearIcon(hIcon);
}

void CDLG_Note::MarkAsRead(BOOL bRefreshInfo)
{
	if(Sets.bIncomingMessage==2){
		// Помечаем как прочитанное
		Sets.bIncomingMessage=1;
		bRefreshInfo=TRUE;
		CString sSender=(Sets.sSenderPerson==""?Sets.sReplyAddressName:Sets.sSenderPerson);
		if(sSender!="" && Sets.sAttachment.Find("["DEF_READREQUEST"]")!=-1 && Sets.sAttachment.Find("["DEF_DELIVERYNOTIFY"]")==-1){
			CString sAutoanswerReply=Format("%s\n%s",_l("Your message has been read"),TrimMessage(Sets.sText,200,4));
			SendNetMessage(sSender,sAutoanswerReply,"",
				CString("["DEF_NOTID)+CDataXMLSaver::GetInstringPart("["DEF_NOTID,"]",Sets.sAttachment)+CString("]")+
				CString("["DEF_NOTTIMESTAMP)+CDataXMLSaver::GetInstringPart("["DEF_NOTTIMESTAMP,"]",Sets.sAttachment)+CString("]")+
				"["DEF_DELIVERYNOTIFY"]"+"["DEF_READREQUEST"]");
			
		}
	}
	if(pMainDialogWindow && bRefreshInfo){
		pMainDialogWindow->StopAnimation(objSettings.AddrBook.IsPersonKnown(Sets.sSenderPerson));
	}
}

void CDLG_Note::ActivateThisNote()
{
	if(iNoteWndStatus<=0){
		return;
	}
	if(bStartHided==TRUE && !Sets.bHidden){
		Sets.iXPos=-1;
		Sets.iYPos=-1;
		InitSizeAndPos();
	}
	bStartHided=FALSE;
	if(!SwitchToWindow(GetSafeHwnd())){
		ActivateWindow(TRUE, TRUE);
	}
	BackToNormalView();
}	

void CDLG_Note::OnAddtoadrbook() 
{
	//
	if(bAnyDialogInProcess){
		return;
	}
	SimpleTracker Track(bAnyDialogInProcess);
	//
	CNewPerson dlgNewPerson(this);
	dlgNewPerson.m_CompName=Sets.sReplyAddressName=GetRecipient();
	dlgNewPerson.m_iTab=objSettings.iLastViewTab;
	int iPersonNum=GetRecipientPerson();
	if(iPersonNum!=-1){
		dlgNewPerson.m_Nick=objSettings.AddrBook.aPersons[iPersonNum]->GetTitle();
		dlgNewPerson.m_CompName=objSettings.AddrBook.aPersons[iPersonNum]->sCompName;
		dlgNewPerson.m_Dsc=objSettings.AddrBook.aPersons[iPersonNum]->sDsc;
		dlgNewPerson.m_bGroup=objSettings.AddrBook.aPersons[iPersonNum]->IsGroup();
		dlgNewPerson.m_iTab=objSettings.AddrBook.aPersons[iPersonNum]->iTabNumber;
	}
	dlgNewPerson.DoModal();
	if(dlgNewPerson.m_Nick.GetLength()>0){
		int iP=objSettings.AddrBook.AddModPerson(dlgNewPerson.m_Nick,dlgNewPerson.m_CompName,dlgNewPerson.m_Dsc,dlgNewPerson.m_bGroup,dlgNewPerson.m_iTab);
		PutRecipientPerson(iP);
		if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
			pMainDialogWindow->dlgAddBook->Refreshlist();
		}
	}
}

afx_msg LRESULT CDLG_Note::UpdateRecipientList(WPARAM wParam, LPARAM lParam)
{
	//Sets.sReplyAddressName=GetRecipient();//Sets.sReplyAddressName уже содержит верное значение!! а edit может быть пустой
	//
	/*CComboBox* pCombo=m_Recipients.GetComboBoxCtrl();
	if(pCombo && ::IsWindow(pCombo->GetSafeHwnd())){
		pCombo->ResetContent();
		for(int i=0;i<objSettings.lMsgRecentList.GetSize();i++){
			CString sName=objSettings.lMsgRecentList[i];
			if(sName==""){
				continue;
			}
			// Ищем его...
			int iPersonNumber=0;
			objSettings.AddrBook.TranslateNickToIP(sName,&iPersonNumber);
			if(iPersonNumber==-1){
				objSettings.AddrBook.TranslateIPToNick(sName,&iPersonNumber);
			}
			// Добавляем
			AddComboBoxExItem(&m_Recipients,i,sName,iPersonNumber==-1?-1:GetStatusIcon(iPersonNumber));
		};
	}*/
	PutRecipient(Sets.sReplyAddressName);
	return 0;
}

void CDLG_Note::GetStartPos1(long& X, long& Y, long iWidth, long iHeight)
{
	double dPos=rndDbl(0,1);
	double dRand=rndDbl(0,1);
	X=objSettings.rDesktopRectangle.right-iWidth;
	Y=objSettings.rDesktopRectangle.bottom-iHeight;
	if(dPos>0.75){
		X=long(X*dRand);
	}else if(dPos>0.5){
		Y=long(Y*dRand);
	}else if(dPos>0.25){
		X=long(X*dRand);
		Y=0;
	}else{
		X=0;
		Y=long(Y*dRand);
	}
	if(Y<objSettings.rDesktopRectangle.top){
		Y=objSettings.rDesktopRectangle.top;
	}
	return;
}

void CDLG_Note::GetStartPos2(long& X, long& Y, long iWidth, long iHeight,int iDirX,int iDirY)
{
	int iCount=0,bPositionOk=FALSE;
	CRect rt;
	rt.left=X;
	rt.top=Y;
	rt.right=rt.left+iWidth;
	rt.bottom=rt.top+iHeight;
	CSmartWndSizer::TestOnScreenOut(rt,objSettings.iStickPix);
	// Дальше...
	X=rt.left;
	Y=rt.top;
	int iXStart=X;
	int iYStart=Y;
	while((iCount++)<500){
		CRect rt(CPoint(X,Y),CPoint(X+iWidth-1,Y+iHeight-1));
		int iRes=CSmartWndSizer::TestRectInWindows(rt,1,0,"WN_NOTEPROP");
		if(iRes==0){
			bPositionOk=TRUE;
			break;
		}
		Y+=iDirY*iHeight;
		if(Y+iHeight>=objSettings.rDesktopRectangle.bottom
			|| Y<=objSettings.rDesktopRectangle.top){
				X+=iDirX*iWidth+1;
				Y=iYStart;
				if(X+iWidth>=objSettings.rDesktopRectangle.right
					|| X<=objSettings.rDesktopRectangle.left){
						break;
					}
			}
	}
	if(!bPositionOk){
		GetStartPos1(X, Y, iWidth, iHeight);
	}
	// Указываем что это окно входящей заметки, 
	// Для будущих входящих проверки пересеченности будут лишь на этих заметках
	SetProp(GetSafeHwnd(),"WN_NOTEPROP",(HANDLE)1);
	return;
}

/*
void CDLG_Note::GetStartPos2(long& X, long& Y, long iWidth, long iHeight)
{
	int iDirX=1,iDirY=1,iCount=0,iTurnCount=0,bPositionOk=FALSE;
	CRect rt;
	rt.left=X;
	rt.top=Y;
	rt.right=rt.left+iWidth;
	rt.bottom=rt.top+iHeight;
	CSmartWndSizer::TestOnScreenOut(rt,objSettings.iStickPix);
	// Задаем ход по часовой стрелке
	iDirX=sgn((rt.left+rt.Width()/2)-(objSettings.rDesktopRectangle.left+objSettings.rDesktopRectangle.Width()/2));
	iDirY=sgn((rt.top+rt.Height()/2)-(objSettings.rDesktopRectangle.top+objSettings.rDesktopRectangle.Height()/2));
	if(iDirX<0 && iDirY>0){
		iDirX=0;
		iDirY=-1;
	}else if(iDirX>0 && iDirY>0){
		iDirX=-1;
		iDirY=0;
	}else if(iDirX>0 && iDirY<0){
		iDirX=0;
		iDirY=1;
	}else{
		iDirX=1;
		iDirY=0;
	}
	// Дальше...
	X=rt.left;
	Y=rt.top;
	CArray<CRect,CRect> aRectStack;
	while((iCount++)<500 && iTurnCount<2){
		CRect rt(CPoint(X,Y),CPoint(X+iWidth-1,Y+iHeight-1));
		int iRes=CSmartWndSizer::TestRectInWindows(rt,0,0,"WN_NOTEPROP");
		if(iRes==0){
			bPositionOk=TRUE;
			break;
		}
		BOOL bSecondRes=0;
		for(int i=0;i<aRectStack.GetSize();i++){
			CRect inter(rt);
			inter.IntersectRect(rt,aRectStack[i]);
			if(!inter.IsRectEmpty()){
				bSecondRes=1;
				break;
			}
		}
		if(iRes==1){
			aRectStack.Add(rt);
		}
		if(iRes==2 || bSecondRes){
			X-=iDirX*iWidth;
			Y-=iDirY*iHeight;
			// Поворачиваем...
			int iTmp=iDirX;
			iDirX=iDirY;
			iDirY=iTmp;
			iDirX=-iDirX;
			iTurnCount++;
		}else{
			iTurnCount=0;
		}
		X+=iDirX*iWidth;
		Y+=iDirY*iHeight;
	}
	if(!bPositionOk){
		GetStartPos1(X, Y, iWidth, iHeight);
	}
	return;
}
*/
void CDLG_Note::InitSizeAndPos()
{
	BOOL bNotSavedPos=FALSE;
	int iNormW=(objSettings.rDesktopRectangle.Width()/4-1);
	if(objSettings.iDefNoteW>0){
		iNormW=objSettings.iDefNoteW;
	}
	int iNormH=(objSettings.rDesktopRectangle.Height()/5-1);
	if(objSettings.iDefNoteH>0){
		iNormH=objSettings.iDefNoteH;
	}
	if(Sets.iXSize<=0){
		Sets.iXSize=iNormW;
	}
	if(Sets.iYSize<=0){
		Sets.iYSize=iNormH;
	}
	int iWidth=Sets.iXSize;
	int iHeight=Sets.iYSize;
	TRACE3("Notepos %s: %i-%i\n",Sets.GetTitle(),iWidth,iHeight);
	int iDirX=1;
	int iDirY=1;
	if(Sets.iXPos==-1 && Sets.iYPos==-1){
		bNotSavedPos=TRUE;
		Sets.iXSize=iNormW;
		Sets.iYSize=iNormH;
		iWidth=Sets.iIconMode?(objSettings.iSHowTitleInIconMode?iNormW:objSettings.lNOTEICON_DIM):iNormW;
		iHeight=Sets.iIconMode?objSettings.lNOTEICON_DIM:iNormH;
		// Вытаскиваем позицию
		CRect rt;
		APPBARDATA pData;
		pData.cbSize=sizeof(pData);
		pData.hWnd=this->GetSafeHwnd();
		SHAppBarMessage(ABM_GETTASKBARPOS, &pData);
		CRect rtBar(pData.rc);
		rt.left=rtBar.left;
		rt.top=rtBar.top;
		if((rtBar.right-rtBar.left)>(rtBar.bottom-rtBar.top)){
			if(rtBar.top<theApp.rDesktopRECT.Height()/2){
				// Вверху
				rt.left=rtBar.left;
				rt.top=rtBar.bottom;
				iDirX=1;
				iDirY=1;
			}else{
				// Внизу
				rt.left=rtBar.left;
				rt.top=rtBar.top-iHeight;
				iDirX=1;
				iDirY=-1;
			}
		}else{
			if(rtBar.left<theApp.rDesktopRECT.Width()/2){
				// Слева
				rt.left=rtBar.right;
				rt.top=rtBar.top;
				iDirX=1;
				iDirY=1;
			}else{
				// Справа
				rt.left=rtBar.left-iWidth;
				rt.top=rtBar.top;
				iDirX=-1;
				iDirY=1;
			}
		}
		Sets.iXPos=rt.left;
		Sets.iYPos=rt.top;
		pData.cbSize=sizeof(pData);
		memset(&pData,0,sizeof(pData));
		if((SHAppBarMessage(ABM_GETSTATE,&pData) & ABS_AUTOHIDE)!=0){//Auto hide is On
			CRect rtTest(Sets.iXPos,Sets.iYPos,Sets.iXPos+iWidth,Sets.iYPos+iHeight);
			CSmartWndSizer::TestOnScreenOut(rtTest,objSettings.iStickPix);
			Sets.iXPos=rtTest.left;
			Sets.iYPos=rtTest.top;
			iWidth=rtTest.Width();
			iHeight=rtTest.Height();
			if(rtBar.Width()>rtBar.Height()){
				if(rtBar.top<theApp.rDesktopRECT.Height()/2){
					Sets.iYPos+=rtBar.Height();
				}else{
					Sets.iYPos-=rtBar.Height();
				}
			}else{
				if(rtBar.left<theApp.rDesktopRECT.Width()/2){
					Sets.iXPos+=rtBar.Width();
				}else{
					Sets.iXPos-=rtBar.Width();
				}
			}
		}
	}
	if(bNotSavedPos){
		GetStartPos2(Sets.iXPos,Sets.iYPos,iWidth,iHeight, iDirX, iDirY);
	}
	Sizer.m_rMain.SetRect(Sets.iXPos,Sets.iYPos,Sets.iXPos+iWidth,Sets.iYPos+iHeight);
	MoveWindow(Sizer.m_rMain,FALSE);
}

void CDLG_Note::RememberWindowPos()
{
	Sets.iXPos=Sizer.m_rMain.left;
	Sets.iYPos=Sizer.m_rMain.top;
	if(!Sets.iIconMode){
		Sets.iXSize=Sizer.m_rMain.Width();
		Sets.iYSize=Sizer.m_rMain.Height();
	}
}

void CDLG_Note::ApplyAllLayouts(BOOL bShow)
{
	// Вызывается отовсюду из-за того что иконки не ресайзятся автоматом :(
	if(iNoteWndStatus<=0)
		return;
	if(Sets.iIconMode){
		CRect minRect(Sizer.m_rMainMin);
		minRect.bottom=minRect.top+objSettings.lNOTEICON_DIM;
		Sizer.m_rMain.bottom=Sizer.m_rMain.top+minRect.Height();
		if(!objSettings.iSHowTitleInIconMode){
			minRect=CRect(0,0,objSettings.lNOTEICON_DIM,objSettings.lNOTEICON_DIM);
			Sizer.m_rMain.right=Sizer.m_rMain.left+minRect.Width();
		}
		Sizer.SetMinRect(minRect);
		Sizer.SetGotoRule(MainRulePos,3);
	}else{
		CRect minRect(0,0,CtrlWidth(this,IDC_NOTE_OPTION)*5+15,CtrlHeight(this,IDC_NOTE_OPTION)*5+5);
		Sizer.SetMinRect(minRect);
		Sizer.SetGotoRule(MainRulePos,Sets.iViewMode);
	}
	ActivateWindow(Sets.bTopMost, bShow);
	Sizer.ApplyLayout();
	return;
}

// wParam==0 - обычный, безоговорочный
// wParam==1 - удаление сообщения после доставки
afx_msg LRESULT CDLG_Note::DeleteNote(WPARAM wParam, LPARAM lParam)
{
	static long lLock=0;
	if(lLock>0){// Процесс уже идет
		return 0;
	}
	SimpleTracker Track(lLock);
	//-------------------------
	if(Sets.bUndeletable){
		return 0;
	}
	InitAutoSend(1);// Останавливаем автоссенд
	if(wParam==0 && !IsMessage() && AskYesNo(_l("Do you want delete this note","Do you really want\nto delete this note")+"?",Sets.GetTitle(25),&objSettings.bAskDlg_DelNote,this)!=IDYES){
		if(objSettings.bAskDlg_DelNote==2){
			objSettings.bAskDlg_DelNote=0;
		}
		return 0;
	}
	if(pNoteDlg){
		pNoteDlg->EndDialog(IDCANCEL);
	}
	iNoteWndStatus=-1;
	Sets.DelFile();
	objSettings.m_Notes.UnregisterNoteById(Sets.GetID());
	IAnimateWindow(this,wParam==0?0:2);
	// Помечаем как прочитанное
	MarkAsRead(TRUE);
	// Ищем следующее непрочитанное
	HWND hToSwitchIn=0;
	if(::GetForegroundWindow()==GetSafeHwnd()){
		// Ищем следующий входящий...
		for(int i=0;i<objSettings.m_Notes.GetSize();i++){
			CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
			if(!pNote || !pNote->IsMessage() || pNote->Sets.bIncomingMessage!=2){
				continue;
			}
			hToSwitchIn=pNote->GetSafeHwnd();
		}
	}	
	Sets.DeleteItem();
	if(hToSwitchIn){
		::SetWindowPos(GetSafeHwnd(),HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hToSwitchIn);
		::SetForegroundWindow(hToSwitchIn);
	}
	DestroyWindow();
	return 0;
}

void CDLG_Note::OnReallyDeleteNote()
{
	DeleteNote();
}

void CDLG_Note::OnDeleteNote()
{
	if(objSettings.bNoteDelButtonWay==0){
		OnNotemenuHidenote();
		return;
	}
	DeleteNote();
}

BOOL CDLG_Note::IsMessage()
{
	if(Sets.bIncomingMessage>0){
		return TRUE;
	}
	if(strstr(Sets.GetPath(),MESS_PATH)==NULL){
		return FALSE;
	}
	return TRUE;
}

void CDLG_Note::OnNoteCopytocl() 
{
	GetNoteText();
	USES_CONVERSION;
	BOOL bThroughtGlobal=0;
	m_EditField.SetSel(0,-1);
	SetClipboardText(A2W(Sets.sText),bThroughtGlobal);
}

void CDLG_Note::PinToWndTitle(const char* szTitle)
{
	CString sTitle=szTitle;
	sTitle.TrimLeft();
	sTitle.TrimRight();
	if(!sTitle.IsEmpty()){
		CString sApps=CString("\n")+Sets.GetApplicationsList()+CString("\n");
		int iCount=sApps.Replace(CString("\n")+sTitle+CString("\n"),"\n");
		if(iCount==0){
			sApps+=sTitle;
		}
		Sets.SetApplicationsList(sApps);
		Sets.bTopMost=TRUE;
	}
}

void CDLG_Note::PinToUnderlaingWnd()
{
	Sets.bTopMost=1;
	// Pinning to app
	RECT MainRect;
	GetWindowRect(&MainRect);
	POINT p;
	p.x=MainRect.left-5;
	p.y=MainRect.top-5;
	HWND hWnd=::GetDesktopWindow();
	if(hWnd){
		hWnd=::WindowFromPoint(p);
	}
	GetWindowTopParent(hWnd);
	if(hWnd){
		CString sBuffer;
		SafeGetWindowText(hWnd,sBuffer);
		PinToWndTitle(sBuffer);
	}
}

void CDLG_Note::ActivateEmotionsMenu()
{
	CMenu IconMenu;
	IconMenu.CreatePopupMenu();
	// Attachments
	//CMenu pAttachments;
	//pAttachments.CreatePopupMenu();
	AddAttachmentsMenu(IconMenu, TRUE);
	//AddMenuSubmenu(&IconMenu,&pAttachments,_l("Attachments")+"\t...");
	// Это может быть сообщение, есть смысл добавить Смайлики
	CMenu pSmiles;
	pSmiles.CreatePopupMenu();
	AddEmotionsMenu(pSmiles);
	int iIcon=theApp.dwMsgBitmap+2;
	AddMenuSubmenu(&IconMenu,&pSmiles,_l("Ext.options")+"\t...",_bmp().Get(GetNoteIcon(0,&iIcon)));
	AppendAdverts(&IconMenu);
	if(objSettings.sFBDir!=""){
		AddMenuString(&IconMenu,ID_FZFOLDER,_l("Shared folder"),_bmp().Get(_IL(5)));
	}
	//
	CRect rt;
	m_NoteIcon.GetWindowRect(&rt);
	IconMenu.SetDefaultItem(0,TRUE);
	int iCommand=::TrackPopupMenu(IconMenu.GetSafeHmenu(), TPM_RETURNCMD, rt.left, rt.top, 0, this->GetSafeHwnd(), NULL);
	int iItem=iCommand-WM_USER-SWITCH_TO;
	CItem* pItem=CItem::GetItemByIndex(iItem);
	if(pItem){
		AddItemToAttachment(pItem->GetID());
	}else{
		PostMessage(WM_COMMAND,iCommand,0);
	}
}

void CDLG_Note::ActivatePinupPopupMenuX(BOOL bAtCursor)
{
	RefreshOnTopMenu();
	CPoint pos;
	GetCursorPos(&pos);
	if(!bAtCursor){
		if(GetDlgItem(IDC_NOTE_ONTOP)->IsWindowVisible()){
			CRect pos2;
			GetDlgItem(IDC_NOTE_ONTOP)->GetWindowRect(&pos2);
			pos.x=pos2.left;
			pos.y=pos2.top;
		}
	}
	m_pMenu->SetDefaultItem(0,TRUE);
	int iCommand=::TrackPopupMenu(m_pMenu->GetSafeHmenu(), TPM_RETURNCMD, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	switch(iCommand)
	{
	case IDC_NOTE_OPTDIALOG:
		OnNoteOptdialog();
		break;
	case ID_NOTEMENU_UNPINNOTE:
	case ID_NOTEMENU_PINNOTE:
		SwitchOntopState();
		break;
	case ID_NOTEMENU_REALLYDELETE:
	case IDC_NOTE_DEL:
		{
			OnReallyDeleteNote();
			return;
		}
	case WM_USER:
	case ID_NOTEMENU_BACKNORMALVIEW:
		BackToNormalView();
		break;
	case ID_NOTEMENU_PINTOUNDERLAINGWINDOW:
		PinToUnderlaingWnd();
		break;
	default:
		{
			int iIndex=iCommand-WM_USER-PINTOWINDOW;
			if(iIndex>=0 && iIndex<sTopLevelWindowsTitles.GetSize()){
				PinToWndTitle(sTopLevelWindowsTitles[iIndex]);
			}else{
				PostMessage(WM_COMMAND,iCommand,0);
			}
		}
	}
	ActivateNoteParams();
}

void CDLG_Note::ActivatePinupPopupMenu()
{
	ActivatePinupPopupMenuX(1);
}

void CDLG_Note::OnRButtonUp(UINT nFlags, CPoint point) 
{
	RefreshMenu(nFlags);
	CPoint pos;
	GetCursorPos(&pos);
	m_pMenu->SetDefaultItem(0,TRUE);
	::TrackPopupMenu(m_pMenu->GetSafeHmenu(), 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
}

void CDLG_Note::BackToNormalView()
{
	MarkAsRead();
	if(Sets.iIconMode){
		m_tooltip.Pop();
		Sets.iIconMode=0;
		Sizer.m_rMain.left=Sets.iXPos;
		Sizer.m_rMain.top=Sets.iYPos;
		Sizer.m_rMain.right=Sizer.m_rMain.left+Sets.iXSize;
		Sizer.m_rMain.bottom=Sizer.m_rMain.top+Sets.iYSize;
		CSmartWndSizer::TestOnScreenOut(Sizer.m_rMain,objSettings.iStickPix);
		ApplyAllLayouts(TRUE);
		SetActiveWindow();
		m_EditField.SetFocus();
	}else{
		ApplyAllLayouts(TRUE);
	}
}

void CDLG_Note::OnNotemenuIconview() 
{
	m_tooltip.Pop();
	LockWindowUpdate();
	GetWindowRect(&Sizer.m_rMain);
	RememberWindowPos();
	Sets.iIconMode=1;
	GetNoteText();
	OnKillfocusTitle();
	ApplyAllLayouts(TRUE);
	RememberWindowPos();
	UnlockWindowUpdate();
	ActivateNoteParams(TRUE);
}

void CDLG_Note::OnNotemenuSmallview() 
{
	Sets.iViewMode=0;
	BackToNormalView();
}
void CDLG_Note::OnNotemenuView() 
{
	Sets.iViewMode=1;
	BackToNormalView();
}

void CDLG_Note::OnNotemenuExtendedview() 
{
	Sets.iViewMode=2;
	BackToNormalView();
}

void CDLG_Note::OnNotemenuSavenote() 
{
	HCURSOR hCur=SetCursor(theApp.LoadCursor(IDC_CURSORSAVE));
	Sets.bSaveOnExit=TRUE;
	SaveNoteToFile();
	SetCursor(hCur);
}

void CDLG_Note::OnNotemenuHidenote()
{
	if(Sets.bUndeletable)
		return;
	ShowWindow(SW_HIDE);
	Sets.bHidden=TRUE;
}

BOOL CDLG_Note::SaveAttachToFile(int iAttachIndex, const char* szAttribs)
{
	DWORD dwSize=0;
	CString sFileName,sFileData;
	CString sErrorDscPrefix="";
	BOOL bSaveRes=FALSE;
	if(GetAttachByIndex(iAttachIndex, sFileName, dwSize, &sFileData)){
		HGLOBAL hGlob=NULL;
		BOOL bSaveRes, bCorrupted=FALSE;
		char szFileDefName[MAX_PATH]="file not found";
		char szDisk[MAX_PATH]="C://", szName[MAX_PATH]="Untitled",szExt[MAX_PATH]=".txt";
		_splitpath(sFileName, NULL, NULL, szName, szExt);
		sprintf(szFileDefName,"%s%s",szName,szExt);
		if(PackFromString(sFileData,dwSize,hGlob) && hGlob!=NULL){
			SimpleTracker Track(objSettings.bDoModalInProcess);
			CString sSavePath=szFileDefName;
			if(szAttribs){
				if(strcmp(szAttribs,"wntype1")==0){
					//TODO
					Task* pTask=new Task();
					sSavePath=pTask->GetItemFile();
					bSaveRes=SaveFileFromHGlob(sSavePath,objSettings.sLastSaveAttachDir,hGlob,dwSize,FALSE,0,this);
					if(bSaveRes){
						bCorrupted=!pTask->LoadFromFile(sSavePath);
						//CItem* pOldItem=CItem::GetItemByID(pTask->GetID());//-нельзя, так как аттачится к дереву в конструкторе
						Task* pOldItem=objSettings.m_Tasks.LoadTaskByID(pTask->GetID());
						if(!bCorrupted && pOldItem==NULL){
							pTask->SetPath(TODO_PATH);
							objSettings.m_Tasks.m_aTasks.Add(pTask);
						}else{
							if(bCorrupted){
								sErrorDscPrefix=_l("Message with corrupted ToDo was received");
							}else{
								sErrorDscPrefix=_l("Message with dupicated ToDo was received");
							}
							bSaveRes=FALSE;
							delete pTask;
						}
						DeleteFile(sSavePath);
					}
				}
				if(strcmp(szAttribs,"wntype3")==0){
					//Reminder
					Reminder* pRemd=new Reminder();
					sSavePath=pRemd->GetItemFile();
					bSaveRes=SaveFileFromHGlob(sSavePath,objSettings.sLastSaveAttachDir,hGlob,dwSize,FALSE,0,this);
					if(bSaveRes){
						bCorrupted=!pRemd->LoadFromFile(sSavePath);
						CItem* pOldItem=CItem::GetItemByID(pRemd->GetID());
						if(!bCorrupted && pOldItem==NULL){
							pRemd->SetPath(REMD_PATH);
							objSettings.m_Reminders.Add(pRemd);
						}else{
							if(bCorrupted){
								sErrorDscPrefix=_l("Message with corrupted reminder was received");
							}else{
								sErrorDscPrefix=_l("Message with dupicated reminder was received");
							}
							bSaveRes=FALSE;
							delete pRemd;
						}
						DeleteFile(sSavePath);
					}
				}
			}else{
				bSaveRes=SaveFileFromHGlob(sSavePath,objSettings.sLastSaveAttachDir,hGlob,dwSize,TRUE,0,this);
			}
		}else{
			bCorrupted=TRUE;
		}
		if(hGlob){
			VERIFY(::GlobalFree(hGlob)==NULL);
		}
		if(bSaveRes==FALSE){
			CString sError;
			if(bCorrupted){
				sError=_l("File corrupted and can`t be saved correctly!");
			}else{
				sError=_l("Error saving file, file not saved!");
			}
			if(sErrorDscPrefix!=""){
				sError=sErrorDscPrefix+",\n"+sError;
			}
			Alert(sError,_l("Save file error"));
			
		}
	}
	return bSaveRes;
}


BOOL CDLG_Note::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// ловим ескейп...
	if(lParam==0 && wParam==2){
		OnNotemenuHidenote();
		return TRUE;
	}
	//
	if(ParseAdvert(wParam)){
		return TRUE;
	}
	int iFromMenuNum=wParam-WM_USER;
	if(iFromMenuNum>=SEND_TO_OFFSET && iFromMenuNum<SEND_TO_OFFSET+USERMSG_STEP){
		if(iFromMenuNum-SEND_TO_OFFSET<objSettings.AddrBook.GetSize()){
			GetDlgItem(IDC_COMBOBOXEX)->SetWindowText(objSettings.AddrBook.aPersons[iFromMenuNum-SEND_TO_OFFSET]->GetTitle());
		}else{
			GetDlgItem(IDC_COMBOBOXEX)->SetWindowText(objSettings.AddrBook.GetTabComps(iFromMenuNum-SEND_TO_OFFSET));
		}
		OnSendmessage();
		return TRUE;
	}else if(iFromMenuNum>=CHANGE_NOTE_STYLE && iFromMenuNum<CHANGE_NOTE_STYLE+USERMSG_STEP){
		Sets.SetColorCode(iFromMenuNum-CHANGE_NOTE_STYLE);
		return TRUE;
	}else if(iFromMenuNum>=BB_SECTION && iFromMenuNum<BB_SECTION+USERMSG_STEP){
		OnItemShare(iFromMenuNum-BB_SECTION);
		return TRUE;
	}else if(iFromMenuNum>=SMILS_BASE && iFromMenuNum<SMILS_BASE+USERMSG_STEP){
		CString sText=":";
		sText+=GetSmilesEnum()[iFromMenuNum-SMILS_BASE].GetFileTitle();
		USES_CONVERSION;
		BOOL bThroughtGlobal=0;
		SetClipboardText(A2W(sText),bThroughtGlobal);
		m_EditField.PasteSpecial(CF_TEXT);
		return TRUE;
	}else if(iFromMenuNum>=ATTACHMENT_SAVE && iFromMenuNum<ATTACHMENT_SAVE+USERMSG_STEP){
		int iAttachIndex=iFromMenuNum-ATTACHMENT_SAVE;
		SaveAttachToFile(iAttachIndex);
		return TRUE;
	}else if(wParam==ID_NOTEMENU_REMOVEALLATTACHMENTS){
		Sets.sAttachment="";
		attachmentFileNames.RemoveAll();
		RefreshAttachedFileList();
	}else if(iFromMenuNum>=ATTACHMENT_REM && iFromMenuNum<ATTACHMENT_REM+USERMSG_STEP){
		int iAttachIndex=iFromMenuNum-ATTACHMENT_REM;
		DWORD dwSize=0;
		CString sFileName;
		if(GetAttachByIndex(iAttachIndex, sFileName, dwSize)){
			RemoveFileFromAttach(sFileName);
			RefreshAttachedFileList();
		}else{
			AfxMessageBox("Can`t find attachment!!!");
		}
		return TRUE;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

void CDLG_Note::UpdateTooltips()
{
	if(!::IsWindow(m_tooltip.GetSafeHwnd())){
		return;
	}
	CString sTitleTooltip=Sets.GetTitle();
	CString sToolTipText=sTitleTooltip;
	if(Sets.iIconMode){
		sTitleTooltip=SetTextForUpdateTipText(Sets.GetTitle()+(Sets.sText!=""?"\n":"")+Sets.sText,30,TRUE);
		sToolTipText=sTitleTooltip;
	}else if(Sets.bTopMost){
		if(Sets.GetApplicationsList()==""){
			sToolTipText=_l("Attached")+" ";
			sToolTipText+="("+_l("Ctrl/Shift/Right click - specials")+")";
		}else{
			sToolTipText=Format(_l("Attached to")+":\n%s",Sets.GetApplicationsList());
		}
	}else{
		Sets.SetApplicationsList("");
		sToolTipText="";
		sToolTipText+="("+_l("Ctrl/Shift/Right click - specials")+")";
	}
	//
	CWnd* pOnTop=GetDlgItem(IDC_NOTE_ONTOP);
	if(pOnTop){
		m_tooltip.UpdateTipText(SetTextForUpdateTipText(sToolTipText), pOnTop);
	}
	m_tooltip.UpdateTipText(TrimMessage(Sets.GetTitle(),2000),&m_EditField);
	m_tooltip.UpdateTipText(TrimMessage(sTitleTooltip,2000), &m_Title);
}

void CDLG_Note::SetNoteText(BOOL bDoNotCheckText)
{
	dwLastChangeTime=GetTickCount();
	m_EditField.SetText(Sets.sText);
	Sets.ItemContentChanged();
	UpdateTooltips();
}

void CDLG_Note::GetNoteText()
{
	if(::IsWindow(m_EditField.m_hWnd)){
		m_EditField.GetWindowText(Sets.sText);
		Sets.sText.Replace("\r\n","\n");
		if(pMainDialogWindow){
			pMainDialogWindow->UpdateFeditIfNeeded(&Sets);
		}
	}
}

BOOL CDLG_Note::IsAutoTransparencyEnabled()
{
	return (!Sets.bMouseTrans) && (Sets.bDisableTransparency==0) && (objSettings.bAutoDisp && objSettings.objSchemes.getAlpha(Sets.GetColorCode())<100);
}

/*DWORD WINAPI AskWhatToDoWithDD(LPVOID p)
{
	CStringArray* pOut=(CStringArray*)p;
	CString sInfoText="\n";
	for (UINT u = 0; u < pOut->GetSize(); u++){
		CString szFilePath=pOut->GetAt[u];
		if(bAttachAction){
			CString sFile=szFilePath;
			if(AddFileToAttachment(sFile)){
				sInfoText+=Format("\n- file://%s",sFile);
				iCounter++;
			}
		}else{
			sInfoText+=szFilePath;
			sInfoText+=":\n\n";
			CString sFileContent;
			ReadFile(szFilePath,sFileContent);
			sInfoText+=sFileContent;
			sInfoText+="\n\n";
			iCounter++;
		}
	}
	if(iCounter>0){
		GetNoteText();
		Sets.sText+="\n"+sInfoText;
		Sets.sText.TrimLeft();
		SetNoteText();
		RefreshAttachedFileList();
	}
	return 0;
}*/

BOOL CDLG_Note::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	if(Sets.bMouseTrans && ::GetAsyncKeyState(VK_MENU)<0){
		OnNotemenuMTrans();
	}
	BOOL bNeedFlashTransparency=FALSE;
	DWORD dwID=::GetDlgCtrlID(pMsg->hwnd);
	if(pMsg->message==WM_TIMER && pMsg->wParam==ID_RESENDTIMER){
		DWORD dwMaxTime=5*60*1000+999;
		int iSecsLeft=(dwMaxTime-(GetTickCount()-dwASTimerStart))/1000;
		if(iSecsLeft<60){
			iSecsLeft=0;
		}
		if(iSecsLeft==0){
			OnSendmessage();
		}else{
			CString sText=Format("%s. %s: %i min",_l("Waiting"),_l("Time left"),iSecsLeft/60,_l("MIN_short","min."));
			DWORD dwResendInfo=RGB(128,0,0);
			SetNoteInfo(&dwResendInfo,sText);
		}
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONUP){
		if(dwID==IDC_RECENTA){
			OnSelchangeComboboxex();
			return TRUE;
		}
	}
	if(pMsg->message==WM_LBUTTONDBLCLK){
		if(dwID==IDC_COMBOBOXEX || dwID==IDC_RECENTA){
			OnSelchangeComboboxex();
			return TRUE;
		}
	}
	if(pMsg->message==WM_KEYDOWN){
		if((dwID==IDC_COMBOBOXEX || isPressed(VK_CONTROL)) && (pMsg->wParam==VK_DOWN || pMsg->wParam==VK_UP)){
			OnSelchangeComboboxex();
			return TRUE;
		}
		if(pMsg->wParam==VK_ESCAPE){
			if(objSettings.bEscDelIncomingMsg && IsMessage() && Sets.bIncomingMessage){
				DeleteNote();
			}
			if(objSettings.iEnableAutoHideWithEsc){
				OnNotemenuHidenote();
			}
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN){
			if(!objSettings.bNoSend){
				if(dwID==IDC_COMBOBOXEX || ::GetAsyncKeyState(VK_CONTROL)<0){
					if(::GetAsyncKeyState(VK_SHIFT)<0){
						Sets.bDeleteAfterSend=TRUE;
					}
					OnSendmessage();
					return TRUE;
				}
			}
			if(dwID==IDC_TITLE){
				m_EditField.SetFocus();
				return TRUE;
			}
		}
		if(isPressed(VK_CONTROL)){
			if(pMsg->wParam=='M'){
				OnNotemenuExtendedview();
				return TRUE;
			}
			if(pMsg->wParam=='I'){
				if(Sets.iIconMode){
					BackToNormalView();
				}else{
					OnNotemenuIconview();
				}
				return TRUE;
			}
			if(pMsg->wParam=='B'){
				OnItemShare(0);
				return TRUE;
			}
			if(pMsg->wParam=='L'){
				OnNotemenuMsglog();
				return TRUE;
			}
			if(pMsg->wParam=='P'){
				OnNotemenuPrint();
				return TRUE;
			}
			if(pMsg->wParam=='D'){
				DeleteNote();
				return TRUE;
			}
			if(pMsg->wParam=='H'){
				OnNotemenuHidenote();
				return TRUE;
			}
			if(pMsg->wParam=='G'){
				OnGSearch();
				return TRUE;
			}
			if(pMsg->wParam=='J'){
				OnExeAsScr();
				return TRUE;
			}
			if(pMsg->wParam=='K'){
				OnNotemenuClipSpy();
				return TRUE;
			}
			if(pMsg->wParam=='S'){
				OnNotemenuSavenote();
				return TRUE;
			}
			if(pMsg->wParam=='E'){
				OnNotemenuPreventfromedit();
				return TRUE;
			}
			if(pMsg->wParam=='O'){
				OnNoteOptdialog();
				return TRUE;
			}
			if(pMsg->wParam=='A'){
				m_EditField.SetSel(0,-1);
				return TRUE;
			}
			if(pMsg->wParam=='U'){
				OnNotemenuSnaptocontent();
				return TRUE;
			}
			if(pMsg->wParam=='N'){
				OnNotemenuAutohide();
				return TRUE;
			}
			if(pMsg->wParam=='Q'){
				SetQuote(-1);
				return TRUE;
			}
			if(pMsg->wParam=='W'){
				OnNotemenuWordwrap();
				return TRUE;
			}
			if(pMsg->wParam=='T'){
				SwitchOntopState(FALSE);
				return TRUE;
			}
			if(dwID==IDC_TEXTFIELD && pMsg->wParam=='V'){
				dwLastChangeTime=GetTickCount();
				m_EditField.PasteSpecial(CF_TEXT);
				GetNoteText();
				return TRUE;
			}
			if(pMsg->wParam=='X'){
				dwLastChangeTime=GetTickCount();
			}
		}
		if(::GetAsyncKeyState(VK_CONTROL)<0){
			if(pMsg->wParam>='0' && pMsg->wParam<='9'){
				if(::GetAsyncKeyState(VK_SHIFT)<0){
					CString sSelText=m_EditField.GetSelText();
					if(sSelText==""){
						GetNoteText();
						sSelText=Sets.sText;
					}
					if(sSelText!=""){
						objSettings.lMsgQAnswers.SetAtGrow(pMsg->wParam-'0',sSelText);
					}
				}else{
					SetQuote(pMsg->wParam-'0');
				}
				return TRUE;
			}
		}
		if(isPressed(VK_SHIFT)){
			if(dwID==IDC_TEXTFIELD && pMsg->wParam==VK_INSERT){
				dwLastChangeTime=GetTickCount();
				m_EditField.PasteSpecial(CF_TEXT);
				GetNoteText();
				return TRUE;
			}
		}
	}
	if(pMsg->message==WM_CUT || pMsg->message==WM_PASTE){
		dwLastChangeTime=GetTickCount();
	}
	if(pMsg->message==WM_LBUTTONUP && dwID!=IDC_NOTE_DEL){
		if (isMove){
			//m_Ontop.SetBrush(RGB(250,250,250));
			ReleaseCapture();
			isMove=0;
		}
	}
	if(pMsg->message==WM_LBUTTONDOWN && dwID!=IDC_NOTE_DEL){
		if(dwID==IDC_NOTE_ONTOP && !Sets.iIconMode){
			OnNoteOntop();
			return TRUE;
		}
		if (Sets.iIconMode || dwID==IDC_TITLE || dwID==0){
			SetCapture();
			isMove=1;
			//
			CPoint point;
			GetCursorPos(&point);
			RECT rect;
			GetWindowRect( (LPRECT) &rect );
			point.x=point.x-rect.left;
			point.y=point.y-rect.top;
			fromPoint=point;
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			if(dwID==IDC_TITLE || MouseInWndRect(&m_Title)){
				m_Title.SetFocus();
			}else{
				return TRUE;
			}
		}
	}
	// Если Always on top и либо вклчен автохайд и не нажат Ctrl либо автохайд не включен и нажат Ctrl с Shift, то скрываем
	if(pMsg->message==WM_MOUSEMOVE){
		if(isMove){
			CPoint point;
			GetCursorPos(&point);
			RECT rect;
			GetWindowRect( (LPRECT) &rect );
			point.x=point.x-rect.left;
			point.y=point.y-rect.top;
			if(fromPoint.x!=point.x || fromPoint.y!=point.y){
				isMove=2;
				rect.right=rect.right-rect.left;
				rect.bottom=rect.bottom-rect.top;
				rect.left-=fromPoint.x-point.x;
				rect.top-=fromPoint.y-point.y;
				MoveWindow(rect.left,rect.top,rect.right,rect.bottom);
			}
			return TRUE;
		}else {
			// Первая часть - автозаурытие при наводе мышки
			BOOL bNeedStartTracker=FALSE;
			bNeedStartTracker|=(Sets.bTopMost && Sets.bAutoHide && !m_EditField.InFocus && !NoteTracker.bStopWatching);
			bNeedStartTracker|=(!Sets.bAutoHide && isPressed(VK_MENU) && objSettings.iEnableAutoHideWithAlt);
			if(bNeedStartTracker){
				if(Sets.bAutoHide && isPressed(VK_MENU)){
					NoteTracker.bStopWatching=TRUE;
				}else{
					NoteTracker.bStopWatching=FALSE;
					//ActivateWindow(Sets.bTopMost,FALSE);
					ShowWindow(SW_HIDE);
				}
				DWORD dwExitCode=STILL_ACTIVE;
				if(MouseTracker!=NULL){
					::GetExitCodeThread(MouseTracker,&dwExitCode);
				}
				if(MouseTracker==NULL || dwExitCode!=STILL_ACTIVE){
					if(MouseTracker!=NULL){
						CloseHandle(MouseTracker);
					}
					MouseTracker=CreateThread( NULL, 0, MouseTrackerThread, (LPVOID)&NoteTracker, 0, &dwMouseTrackerThread);
				}
				return TRUE;
			}
			// Вторая часть - смена прозрачности при наводе мышки
			if(IsAutoTransparencyEnabled()){
				bNeedFlashTransparency=TRUE;
			}
		}
	}
	if(pMsg->message==WM_SETFOCUS){
		if(IsAutoTransparencyEnabled()){
			bNeedFlashTransparency=TRUE;
		}
	}
	if(pMsg->message==WM_LBUTTONDBLCLK){
		if(MouseInWndRect(&m_Title)){
			if(Sets.iIconMode){
				BackToNormalView();
			}else{
				OnNotemenuIconview();
			}
			return TRUE;
		}
		if(MouseInWndRect(GetDlgItem(IDC_NOTE_ONTOP))){
			if(Sets.bIncomingMessage==2 && objSettings.bAutoQuote){
				OnNoteQuote();
			}else{
				BackToNormalView();
			}
			return TRUE;
		}
	}
	if(pMsg->message==WM_COMMAND && dwID==IDC_NOTE_HISTORY){
		OnNotemenuMsglog();
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONUP && dwID==IDC_NOTE_ICON){
		ActivateEmotionsMenu();
		return TRUE;
	}
	if(pMsg->message==WM_RBUTTONUP){
		if(dwID==IDC_NOTE_RED){
			OnRightclickedNoteRed();
			return TRUE;
		}
		if(dwID==IDC_NOTE_ICON){
			ActivateEmotionsMenu();
			return TRUE;
		}
		if(dwID==IDC_NOTE_ONTOP || dwID==IDC_TITLE){
			ActivatePinupPopupMenu();
			return TRUE;
		}
	}
	if(pMsg->message==WM_DROPFILES){
		static CCriticalSection cs;
		CSmartLock SL(&cs,TRUE);
		CStringArray aOptions;
		aOptions.Add("Attach file(s) to note");
		aOptions.Add("Insert link(s) file(s)");
		aOptions.Add("Insert file(s) as text");
		BOOL bAttachAction=SelectFromMenu(aOptions,GetSafeHwnd());
		if(bAttachAction>=0){
			//-----------
			HDROP hDropInfo=(HDROP)pMsg->wParam;
			int iCounter=0;
			char szFilePath[MAX_PATH]="",szFileName[MAX_PATH]="",szExtension[MAX_PATH]="";
			UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
			CStringArray* pOut=new CStringArray();
			CString sInfoText="\n";
			for (UINT u = 0; u < cFiles; u++){
				DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
				pOut->Add(szFilePath);
				if(bAttachAction==0){
					CString sFile=szFilePath;
					if(AddFileToAttachment(sFile)){
						sInfoText+=Format("\n- file://%s",sFile);
						iCounter++;
					}
				}
				if(bAttachAction==1){
					sInfoText+=Format("\n- file://%s",szFilePath);
					iCounter++;
				}
				if(bAttachAction==2){
					sInfoText+=szFilePath;
					sInfoText+=":\n\n";
					CString sFileContent;
					ReadFile(szFilePath,sFileContent);
					sInfoText+=sFileContent;
					sInfoText+="\n\n";
					iCounter++;
				}
			}
			DragFinish(hDropInfo);
			//FORK(AskWhatToDoWithDD,pOut);
			if(iCounter>0){
				GetNoteText();
				Sets.sText+="\n"+sInfoText;
				Sets.sText.TrimLeft();
				SetNoteText();
				if(bAttachAction==0){
					RefreshAttachedFileList();
				}
			}
		}
		return TRUE;
	}
	if(bNeedFlashTransparency){
		DWORD dwExitCode=0;
		if(MouseTracker2!=NULL){
			::GetExitCodeThread(MouseTracker2,&dwExitCode);
		}
		if(dwExitCode!=STILL_ACTIVE){
			NoteTracker2.bStopWatching=FALSE;
			NoteTracker2.dwAddFlag1=IsAutoTransparencyEnabled()?(objSettings.objSchemes.getAlpha(Sets.GetColorCode())):0;
			CloseHandle(MouseTracker2);
			DWORD dwMouseTrackerThread2=0;
			MouseTracker2=CreateThread( NULL, 0, MouseTrackerThread, (LPVOID)&NoteTracker2, 0, &dwMouseTrackerThread2);
			return TRUE;
		}
	}
	BOOL bDRes=CDialog::PreTranslateMessage(pMsg);
	if(pMsg->message==WM_CHAR && (dwID==IDC_TITLE || dwID==IDC_TEXTFIELD || dwID==IDC_COMBOBOXEX)){
		if(dwID==IDC_TITLE){
			bNoteTitleChanged=1;
		}
		DWORD dwCurTime=GetTickCount();
		SetNoteInfo(NULL,NULL);
		dwLastChangeTime=dwCurTime;
		GetNoteText();
	}
	return bDRes;
}

LRESULT CDLG_Note::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_CHANGECBCHAIN){
		if(hNextClipboardChainWnd!=HWND(wParam)){
			::SendMessage(hNextClipboardChainWnd,WM_CHANGECBCHAIN,wParam,lParam);
		}else{
			hNextClipboardChainWnd=HWND(lParam);
		}
		return 0;
	}
	if(message==WM_DRAWCLIPBOARD && lClipSpy && hNextClipboardChainWnd){
		USES_CONVERSION;
		GetNoteText();
		WCHAR* wszStr=GetClipboardText(FALSE,FALSE);
		if(wszStr){
			CString sClipPiece=W2A(wszStr);
			static CString sPrev="";
			if(sClipPiece!="" && sClipPiece!=sPrev){
				CString sAddition;
				sAddition.Format("[%s #%i]\n%s\n\n",_l("Item"),++iClipCount,sClipPiece);
				Sets.sText=sAddition+Sets.sText;
				SetNoteText();
				sClipPiece=sPrev;
			}
		}
		::SendMessage(hNextClipboardChainWnd,WM_DRAWCLIPBOARD,0,0);
		return 0;
	}
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		if(iNoteWndStatus>=1){
			RememberWindowPos();
		}
		return DispatchResultFromSizer;
	}
	if(message==WM_CTLCOLOREDIT || message==WM_CTLCOLORSCROLLBAR){
		return (LRESULT)OnCtlColor(CDC::FromHandle(HDC(wParam)),CWnd::FromHandle(HWND(lParam)),0);
	}
	if(message==WM_DESTROY){
		if(iNoteWndStatus>=1){
			RememberWindowPos();
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDLG_Note::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CDLG_Note::OnSendmessage()
{
	if(iSendLock>0){
		return;
	}
	SimpleTracker lock(iSendLock);
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return;
	}
	GetNoteText();
	//InitAutoSend(1);// Не! Останавливаем автоссенд. Остановится если есть либо по доставке либо никак!!!
	if(strlen(Sets.sReplyAddressName)>0 && strlen(Sets.sText)>0){
		AddRecipientToRecentList(Sets.sReplyAddressName);
		if(objSettings.bAutoMoveNoteToMsg && !IsMessage()){
			// Переносим в папку "Messages"
			Sets.Detach();
			Sets.SetPath(MESS_PATH);
			Sets.Attach(1);
		}
		Sets.bIncomingMessage=0;// Теперь это в любом случае не Incoming message...
		MarkAsRead();
		CString sTitle=Sets.GetTitle();
		if(bNoteTitleChanged==0){
			Sets.SetTitle(CNoteSettings::GetMessageTitle(Sets.sReplyAddressName,FALSE));
			sTitle="";
		}
		ActivateNoteParams();
		SendNetMessage(Sets.sReplyAddressName,Sets.sText,sTitle,Sets.sAttachment,Sets.GetTitle(),Sets.GetID(),Sets.GetColorCode(),Sets.bDeleteAfterSend);
	}else{
		if(strlen(Sets.sReplyAddressName)==0){
			Alert(_l("Choose message recipient first")+"!");
		}else{
			Alert(_l("Type message text first")+"!");
		}
	}
}

BOOL CDLG_Note::CanThisNoteBeShown()
{
	if(Sets.bTopMost==0) {
		return TRUE;
	}
	return TestTitleMatchCondition();
}

BOOL CDLG_Note::TestTitleMatchCondition(const char* szTitle)
{
	if(!pMainDialogWindow){
		return FALSE;
	}
	if(Sets.bTopMost==0){
		return FALSE;
	}
	if(Sets.GetApplicationsList().GetLength()==0){
		return TRUE;
	};
	CString sTitle=szTitle;
	if(pMainDialogWindow && sTitle==""){
		sTitle=pMainDialogWindow->sActiveAppTitle;
	}
	BOOL bVisible=(iNoteWndStatus==1) && IsWindowVisible();
	if(sTitle==objSettings.sMainWindowTitle || sTitle==objSettings.sTrayTooltip || sTitle==objSettings.sMCTrayTooltip){
		return bVisible;
	};
	// Если активным стала сама записка, то выключаться нет смысла
	if(bTitleChanged){
		OnKillfocusTitle();
	}
	if(bVisible!=0 && sTitle==Sets.GetTitle()){//|| CSmartWndSizer::TestRectInWindows(Sizer.m_rMain,TRUE)==1
		return bVisible;
	}
	if(pMainDialogWindow){
		return Sets.TestTitleMatchCondition_Raw(sTitle);
	}
	return FALSE;
}

void CDLG_Note::OnNotemenuAutohide() 
{
	Sets.bAutoHide=!Sets.bAutoHide;
	if(Sets.bAutoHide){
		Sets.bTopMost=TRUE;
		SmartWarning(_l("To disable auto-hiding you can hold ALT key\nwhile moving mouse over note window"),_l("Note auto-hide mode"),&objSettings.bNoteAHideMode,DEFAULT_READTIME*3,DT_CENTER|DT_VCENTER,this);
	}
	ActivateNoteParams();
}

void CDLG_Note::OnNotemenuPreventfromedit() 
{
	Sets.SetReadOnly(!Sets.GetReadOnly());
	ActivateNoteParams();
}

void CDLG_Note::OnNotemenuRemindin15min() 
{
	AddReminder(15,Sets.GetTitle());
}

void CDLG_Note::OnNotemenuRemindafter3h() 
{
	AddReminder(3*60,Sets.GetTitle());
}

void CDLG_Note::OnNotemenuRemindin1h() 
{
	AddReminder(60,Sets.GetTitle());
}

void CDLG_Note::OnNotemenuRemindin30min() 
{
	AddReminder(30,Sets.GetTitle());
}

void CDLG_Note::OnNotemenuRemindtomorrow()
{
	//
	if(bAnyDialogInProcess){
		return;
	}
	SimpleTracker Track(bAnyDialogInProcess);
	//
	Reminder* rem=AddReminder(24*60,Sets.GetTitle());
	if(rem){
		rem->CreateRemDialog();
	}
}

void CDLG_Note::OnNotemenuBringallontop() 
{
	if(pMainDialogWindow){
		pMainDialogWindow->OnSystrayShowallnotes();
	}
}

void CDLG_Note::PopupMenu(BOOL bExtended)
{
	CPoint point(0,0);
	OnRButtonUp(bExtended, point);
}

void CDLG_Note::OnNotemenuShownote() 
{
	ActivateThisNote();
}

void CDLG_Note::OnNotemenuWordwrap() 
{
	Sets.bWordWrap=!Sets.bWordWrap;
	ActivateNoteParams();
}

void CDLG_Note::OnNotemenuA2W()
{
	ActivatePinupPopupMenuX(0);
}

void CDLG_Note::OnNotemenuSnaptocontent() 
{
	GetNoteText();
	CSize size(200,100);
	CDC* pDC=m_EditField.GetDC();
	if(pDC){
		int iLines=0;
		pDC->SelectObject(font);
		size=GetLargestLineSize(pDC,Sets.sText,iLines);
		pDC->LPtoDP(&size);
		size.cy=size.cy*iLines;
		ReleaseDC(pDC);
	}
	AdoptSizeToScreen(size);
	Sizer.m_rMain.right=Sizer.m_rMain.left+size.cx+10+CtrlWidth(this,0);
	Sizer.m_rMain.bottom=Sizer.m_rMain.top+size.cy+10+CtrlHeight(this,0)+CtrlHeight(this,IDC_TITLE)+(Sets.iViewMode>0?CtrlHeight(this,IDC_NOTE_OPTION):0)+(Sets.iViewMode>1?CtrlHeight(this,IDC_COMBOBOXEX):0);
	MoveWindow(Sizer.m_rMain);
	ApplyAllLayouts();
}

void CDLG_Note::OnNotemenuSnaptocontent_old() 
{
	GetNoteText();
	long len=strlen(Sets.sText);
	m_EditField.LineScroll(-m_EditField.GetFirstVisibleLine(), 0);
	char szText[1024]="";
	long i=0,maxx=0,maxy=0;
	for(i=0; i<len;i++){
		CPoint p=m_EditField.GetCharPos(i);
		if(p.x>maxx){
			maxx=p.x;
		}
		if(p.y>maxy){
			maxy=p.y;
		}
	}
	long lLines=m_EditField.GetLineCount();
	if(lLines>1){
		maxy+=maxy/(lLines-1);
	}else{
		maxy+=15;
	}
	CRect rOldEditSize;
	m_EditField.GetClientRect(&rOldEditSize);
	CRect rOldNoteSize;
	GetWindowRect(&rOldNoteSize);
	Sizer.m_rMain.right=Sizer.m_rMain.left+(rOldNoteSize.Width()-rOldEditSize.Width())+maxx;
	Sizer.m_rMain.bottom=Sizer.m_rMain.top+(rOldNoteSize.Height()-rOldEditSize.Height())+maxy;
	MoveWindow(Sizer.m_rMain);
	ApplyAllLayouts();
}

void CDLG_Note::PostNcDestroy() 
{
	if(lClipSpy){
		OnNotemenuClipSpy();
	}
	CDialog::PostNcDestroy();
	delete this;
}


void CDLG_Note::OnInfowndDelete() 
{
	DeleteNote();
}

void CDLG_Note::OnInfowndOpen() 
{
	ActivateThisNote();
	m_STray.RemoveIcon();
	m_EditField.SetFocus();
}

void CDLG_Note::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID==SC_MINIMIZE || nID==SC_CLOSE){
		// Не минимизируем
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CDLG_Note::SaveNoteToFile()
{
	GetNoteText();
	OnKillfocusTitle();
	RememberWindowPos();
	dwLastChangeTime=0;
	Sets.SaveToFile();
}

void CDLG_Note::OnNotemenuMTrans()
{
	Sets.bMouseTrans=!Sets.bMouseTrans;
	ActivateNoteParams();
	if(Sets.bMouseTrans){
		SmartWarning(_l("To disable click-through mode\nyou should edit note`s parameters\nfrom organizer window"),_l("Click-through mode"),&objSettings.bNoteCTMode,DEFAULT_READTIME*3,DT_CENTER|DT_VCENTER,this);
	}
}

void CDLG_Note::OnNotemenuFixedsize()
{
	Sets.bFixedSize=!Sets.bFixedSize;
	ActivateNoteParams();
}

BOOL CDLG_Note::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CDLG_Note::OnNotemenuSmiley1() 
{
	OnNotemenuSmileyOff();
	Sets.sAttachment+=Format("["ICONPREFIX"%s]\n",SMILE1);
	Sets.SetIconNum(theApp.dwMsgBitmap+2);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuSmiley2() 
{
	OnNotemenuSmileyOff();
	Sets.sAttachment+=Format("["ICONPREFIX"%s]\n",SMILE2);
	Sets.SetIconNum(theApp.dwMsgBitmap+3);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuSmiley3() 
{
	OnNotemenuSmileyOff();
	Sets.sAttachment+=Format("["ICONPREFIX"%s]\n",SMILE3);
	Sets.SetIconNum(theApp.dwMsgBitmap+4);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuSmiley4() 
{
	OnNotemenuSmileyOff();
	Sets.sAttachment+=Format("["ICONPREFIX"%s]\n",SMILE4);
	Sets.SetIconNum(theApp.dwMsgBitmap+5);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuSmiley5() 
{
	OnNotemenuSmileyOff();
	Sets.sAttachment+=Format("["ICONPREFIX"%s]\n",SMILE5);
	Sets.SetIconNum(theApp.dwMsgBitmap+6);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuSmiley6() 
{
	OnNotemenuSmileyOff();
	Sets.sAttachment+=Format("["ICONPREFIX"%s]\n",SMILE6);
	Sets.SetIconNum(theApp.dwMsgBitmap+7);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuPos1() 
{
	CDataXMLSaver::StripInstringPart("["POSPREFIX,"]",Sets.sAttachment);
	Sets.sAttachment+=Format("["POSPREFIX"%s]\n",POS1);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuPos2() 
{
	CDataXMLSaver::StripInstringPart("["POSPREFIX,"]",Sets.sAttachment);
	Sets.sAttachment+=Format("["POSPREFIX"%s]\n",POS2);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuPos3() 
{
	CDataXMLSaver::StripInstringPart("["POSPREFIX,"]",Sets.sAttachment);
	Sets.sAttachment+=Format("["POSPREFIX"%s]\n",POS3);
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuSmileyOff() 
{
	CDataXMLSaver::StripInstringPart("["ICONPREFIX,"]",Sets.sAttachment);
	CDataXMLSaver::StripInstringPart("["POSPREFIX,"]",Sets.sAttachment);
	if(IsMessage()){
		if(Sets.bIncomingMessage){
			Sets.SetIconNum(theApp.dwMsgBitmap+1);
		}else{
			Sets.SetIconNum(theApp.dwMsgBitmap);
		}
	}else{
		Sets.SetIconNum(NOTE_ICON_NUM);
	}
	ActivateWindow(Sets.bTopMost);
}

BOOL CDLG_Note::RemoveFileFromAttach(const char* szFileName)
{
	if(szFileName==NULL || szFileName==""){
		return FALSE;
	}
	int iFileStart=Sets.sAttachment.Find(Format("[file name='%s'",GetPathPart(szFileName,0,0,1,1)));
	if(iFileStart!=-1){
		int iFileEnd=Sets.sAttachment.Find("']",iFileStart);
		if(iFileEnd!=-1){
			const char* szEnd=(const char*)(Sets.sAttachment)+iFileEnd+2;
			while(*szEnd=='\n'){
				szEnd++;
			}
			Sets.sAttachment=Sets.sAttachment.Left(iFileStart)+szEnd;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDLG_Note::AddFileToAttachment(CString& sFileName, const char* szAttribs, const char* szSendWithName)
{
	if(sFileName=="" || !isFileExist(sFileName)){
		return FALSE;
	}
	// Смотрим, есть ли уже такой файл в аттаче
	RemoveFileFromAttach(sFileName);
	// Загружаем
	DWORD dwSize=GetFileSize(sFileName);
	if(dwSize==0){
		return FALSE;
	}
	HGLOBAL hGlob=::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, dwSize+1);
	if(!ReadFileToBuffer(sFileName, hGlob, dwSize)){
		return FALSE;
	}
	DWORD dwFutureTotalAttSize=strlen(Sets.sAttachment)+dwSize;
	if(dwFutureTotalAttSize>DWORD(objSettings.dwMaxAttachSizeKB*1024)){
		Alert(_l("Sorry, this file can`t be attached; Max attachment size")+" "+GetSizeStrFrom(objSettings.dwMaxAttachSizeKB,1));
		return FALSE;
	}
	if(hGlob){
		CString sOut = PackToString((char*)hGlob,dwSize);
		char szName[MAX_PATH]="Untitled",szExt[MAX_PATH]=".txt";
		_splitpath(sFileName, NULL, NULL, szName, szExt);
		CString sSafeFileName=szName;
		sSafeFileName+=szExt;
		MakeSafeFileName(sSafeFileName);
		sSafeFileName=szSendWithName?szSendWithName:sSafeFileName;
		sSafeFileName.Replace("'","\"");// Так как ' используется для разделения...
		sFileName=sSafeFileName;
		Sets.sAttachment+=Format("[file name='%s', len='%lu', attribs='%s', dat64='%s']\n",sFileName,dwSize,szAttribs,sOut);
		VERIFY(::GlobalFree(hGlob)==NULL);
	}
	return TRUE;
}

BOOL CDLG_Note::GetAttachByIndex(int iIndex, CString& sFileName, DWORD& dwSize, CString* pData, CString* pAttribs)
{
	BOOL bFinded=FALSE;
	int iCurrentIndex=0;
	int iFrom=0, iPrevFrom=0;
	CString sFileData="";
	while(iFrom>=0){
		BOOL bHandled=0;
		iPrevFrom=iFrom;
		sFileData=CDataXMLSaver::GetInstringPart("[file","dat64",Sets.sAttachment,iFrom,XMLNoConversion);
		if(iPrevFrom!=iFrom){
			if(iCurrentIndex==iIndex){
				bFinded=TRUE;
				break;
			}
			iCurrentIndex++;
		}
	}
	if(bFinded){
		if(pData){
			// Если нужно с данными - вытаскиваем полную версию
			sFileData=CDataXMLSaver::GetInstringPart("[file","]",Sets.sAttachment,iPrevFrom,XMLNoConversion);
		}
		if(*((const char*)sFileData)!=0){
			// Файл найден!
			sFileName=CDataXMLSaver::GetInstringPart("name='","'",sFileData);
			dwSize=atol(CDataXMLSaver::GetInstringPart("len='","'",sFileData));
			if(pData){
				*pData=CDataXMLSaver::GetInstringPart("dat64='","'",sFileData,XMLNoConversion);
			}
			if(pAttribs){
				*pAttribs=CDataXMLSaver::GetInstringPart("attribs='","'",sFileData,XMLNoConversion);
			}
			return TRUE;
		}
	}
	return FALSE;
}


void CDLG_Note::OnNotemenuAttachfile() 
{
	//
	if(bAnyDialogInProcess){
		return;
	}
	SimpleTracker Track(bAnyDialogInProcess);
	SimpleTracker Track2(objSettings.bDoModalInProcess);
	//
	CFileDialog dlg(TRUE, "txt", NULL, OFN_ALLOWMULTISELECT, "All files (*.*)|*.*||", this);
	char szFileNameBuffer[102400]="";
	dlg.m_ofn.lpstrFile=szFileNameBuffer;
	dlg.m_ofn.lpstrInitialDir=objSettings.sLastGetAttachPath;
	dlg.m_ofn.nMaxFile=sizeof(szFileNameBuffer);
	int iCounter=0;
	CString sInfoText="";
	if(dlg.DoModal()!=IDOK){
		return;
	}else{
		static CCriticalSection cs;
		CSmartLock SL(&cs,TRUE);
		//
		POSITION p=dlg.GetStartPosition();
		CString sFile;
		while(p){
			sFile=dlg.GetNextPathName(p);
			if(sFile!=""){
				if(AddFileToAttachment(sFile)){
					sInfoText+=Format("\n- file://%s",sFile);
					iCounter++;
				}
			}
		}
		if(sFile!=""){
			objSettings.sLastGetAttachPath=sFile;
		}
	}
	if(iCounter>0){
		GetNoteText();
		CString sInfoTextTotal="\n";
		Sets.sText+=sInfoTextTotal+sInfoText;
		SetNoteText();
		RefreshAttachedFileList();
	}
}

void CDLG_Note::RefreshAttachedFileList()
{
	attachmentFileNames.RemoveAll();
	if(Sets.sAttachment==""){
		ActivateWindow(Sets.bTopMost);
		return;
	}
	CString sAttName;
	DWORD dwAttSize;
	int iAttPosition=0;
	while(GetAttachByIndex(iAttPosition,sAttName,dwAttSize)){
		char szName[MAX_PATH]="Untitled",szExt[MAX_PATH]=".txt";
		_splitpath(sAttName, NULL, NULL, szName, szExt);
		CString sAttName=Format("%s%s",szName,szExt);
		if(sAttName==""){
			sAttName="Unknown";
		}
		attachmentFileNames.SetAtGrow(iAttPosition,Format("%s\t%s",sAttName,GetSizeStrFrom(dwAttSize,0)));
		iAttPosition++;
	}
	ActivateWindow(Sets.bTopMost);
}

void CDLG_Note::OnNotemenuMsglog() 
{
	if(pMainDialogWindow){
		int iPersonNumber=0;
		CString sNick=Sets.sReplyAddressName;
		if(!objSettings.bUseNickOnSend){
			sNick=objSettings.AddrBook.TranslateIPToNick(Sets.sReplyAddressName,NULL);
		}
		pMainDialogWindow->OpenMsgLogOnNick(sNick);
	}
}

afx_msg void CDLG_Note::OnNotemenuNewStyle()
{
	//Sets.bUndeletable++;
	HWND h=GetSafeHwnd();
	CString sID=Sets.GetID();
	CallStylesDialogWnd(pMainDialogWindow,Sets.GetColorCode()-1);
	CItem* item=CItem::GetItemByID(sID);
	if(item){
		// Так как итем может умереть за это время...
		item->SetColorCode(objSettings.iLastSchemeUsed+1);
		if(IsWindow(h)){
			SetColorCode(objSettings.iLastSchemeUsed+1);
		}
	}
	//Sets.bUndeletable--;
}

void CDLG_Note::OnNotemenuCopy() 
{
	m_EditField.Copy();
}

void CDLG_Note::OnNotemenuCut() 
{
	m_EditField.Cut();
}

void CDLG_Note::OnNotemenuPaste() 
{
	m_EditField.PasteSpecial(CF_TEXT);
	GetNoteText();
}

void CDLG_Note::OnNotemenuLoadfromfile() 
{
	if(bAnyDialogInProcess){
		return;
	}
	SimpleTracker Track(bAnyDialogInProcess);
	SimpleTracker Track2(objSettings.bDoModalInProcess);	
	CFileDialog dlg(TRUE, "txt", NULL, 0, "All files (*.*)|*.*||", this);
	char szFileNameBuffer[102400]="";
	dlg.m_ofn.lpstrFile=szFileNameBuffer;
	dlg.m_ofn.lpstrInitialDir=objSettings.sLastGetAttachPath;
	dlg.m_ofn.nMaxFile=sizeof(szFileNameBuffer);
	if(dlg.DoModal()!=IDOK){
		return;
	}else{
		POSITION p=dlg.GetStartPosition();
		CString sFile=dlg.GetNextPathName(p);
		CString sFileContent;
		if(ReadFile(sFile, sFileContent)){
			Sets.sText=sFileContent;
			SetNoteText();
		}else{
			Alert(_l("Can`t read file"));
		}
	}
}

void CDLG_Note::OnNotemenuSavetofile() 
{
	if(bAnyDialogInProcess){
		return;
	}
	SimpleTracker Track(bAnyDialogInProcess);
	SimpleTracker Track2(objSettings.bDoModalInProcess);
	GetNoteText();
	DWORD dwDataSize=strlen(Sets.sText);
	HGLOBAL hGlobal=::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, dwDataSize+1);
	if (hGlobal == NULL){
		return;
	};
	char* sFileContent = (char*)hGlobal;
	strcpy(sFileContent,Sets.sText);
	CString sSavePath=Sets.GetTitle()+".txt";
	BOOL bSaveRes=SaveFileFromHGlob(sSavePath,objSettings.sLastSaveNoteTextDir,hGlobal,dwDataSize,TRUE,0,this);
	VERIFY(::GlobalFree(hGlobal)==NULL);
	hGlobal=NULL;
}

void CDLG_Note::OnAbook() 
{
	CRect rABook;
	m_ABook.GetWindowRect(&rABook);
	CMenu AddrbookMenu;
	AddrbookMenu.CreatePopupMenu();
	int iPersonNumber=GetRecipientPerson();
	CMenu* pAddrbookMenuPointer=&AddrbookMenu;
	RefreshRecipientMenu(NULL, pAddrbookMenuPointer);
	if(AddrbookMenu.GetMenuItemCount()>0){
		AppendAdverts(&AddrbookMenu);
	}else{
		Alert(_l("Your address book is empty")+"!",DEFAULT_READTIME);
		return;
	}
	if(iPersonNumber==-1){
		AddMenuString(&AddrbookMenu,IDC_ADDTOADRBOOK,_l("Add to address book"),_bmp().Get(IDB_BM_ADRBOOK));
	}
	AddMenuString(&AddrbookMenu,ID_NOTEMENU_OPENADDRESSBOOK,_l("Show address book"),_bmp().Get(IDB_BM_ADRBOOK));
	int wm=::TrackPopupMenu(AddrbookMenu.GetSafeHmenu(), TPM_RETURNCMD, rABook.left, rABook.top, 0, this->GetSafeHwnd(), NULL);
	if(ParseAdvert(wm)){
		return;
	}
	int iRecipient=wm-WM_USER-SEND_TO_OFFSET;
	if(iRecipient>=0){
		PutRecipientPerson(iRecipient);
	}
	if(wm==IDC_ADDTOADRBOOK){
		PostMessage(WM_COMMAND,IDC_ADDTOADRBOOK,0);
	}
	if(pMainDialogWindow && wm==ID_NOTEMENU_OPENADDRESSBOOK){
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_ADDRESSBOOK,0);
	}
}

CString CDLG_Note::GetRecipient()
{
	CString sRes;
	m_Recipients.GetWindowText(sRes);
	return sRes;
}

int CDLG_Note::GetRecipientPerson()
{
	Sets.sReplyAddressName=GetRecipient();
	int iPersonNumber=0;
	objSettings.AddrBook.TranslateNickToIP(Sets.sReplyAddressName,&iPersonNumber);
	if(iPersonNumber==-1){
		objSettings.AddrBook.TranslateIPToNick(Sets.sReplyAddressName,&iPersonNumber);
	}
	return iPersonNumber;
}

void CDLG_Note::PutRecipientPerson(int iRecipient)
{
	if(iRecipient<0){
		return;
	}
	if(iRecipient<objSettings.AddrBook.GetSize()){
		Person* pPerson=objSettings.AddrBook.aPersons[iRecipient];
		if(pPerson){
			CString sPersonNm;
			if(objSettings.bUseNickOnSend){
				sPersonNm=pPerson->GetTitle();
			}else{
				sPersonNm=pPerson->sCompName;
			}
			if(isPressed(VK_SHIFT) && Sets.sReplyAddressName!=""){
				sPersonNm=Sets.sReplyAddressName+"; "+sPersonNm;
			}
			PutRecipient(sPersonNm);
		}
	}else{
		PutRecipient(objSettings.AddrBook.GetTabComps(iRecipient));
	}
}

void CDLG_Note::PutRecipient(const char* szRecipient)
{
	if(szRecipient==NULL || strlen(szRecipient)==0){
		return;
	}
	dwLastChangeTime=GetTickCount();
	Sets.sReplyAddressName=szRecipient;
	Sets.sReplyAddressName.TrimLeft();
	Sets.sReplyAddressName.TrimRight();
	m_Recipients.SetWindowText(Sets.sReplyAddressName);
}

void CDLG_Note::SetColorCode(long iCode)
{
	dwLastChangeTime=GetTickCount();
	//Шрифт...
	objSettings.objSchemes.getFont(iCode)->GetLogFont(&logfont);
	deleteFont(font);
	font=new CFont();
	font->CreateFontIndirect(&logfont);
	m_EditField.SetFont(font,FALSE);
	// Цвет фона...
	DWORD cBGColor=objSettings.objSchemes.getBgColor(iCode);
	DWORD cTXColor=objSettings.objSchemes.getTxtColor(iCode);
	deleteBrush(m_hBr);
	deleteBrush(m_hBrTitle);
	DWORD cBG2=Color2Color(cTXColor, cBGColor, BG1C, 0);
	m_dwBrTitle=Color2Color(cTXColor, cBGColor, BG2C, 0);
	m_hBr = new CBrush(cBGColor);
	m_hBrTitle = new CBrush(m_dwBrTitle);
	m_Send.SetBrush(cBG2);
	m_ABook.SetBrush(cBG2);
	m_NoteIcon.SetBrush(cBG2);
	//m_NoteHistory.SetBrush(cBG2);
	m_RecentList.SetBrush(cBG2);
	m_PrintNote.SetBrush(m_dwBrTitle);
	m_RemindLater.SetBrush(m_dwBrTitle);
	m_SaveToFile.SetBrush(m_dwBrTitle);
	m_DestroyNote.SetBrush(m_dwBrTitle);
	m_Reply.SetBrush(m_dwBrTitle);
	m_OptionsDialog.SetBrush(m_dwBrTitle);
	m_ColorRed.SetBrush(m_dwBrTitle);
	m_Option.SetBrush(m_dwBrTitle);
	m_Option.SetStyles(CBS_FLAT|CBS_HIQUAL);
	// Цвет текста...
	m_EditField.SetColor(cTXColor,cBGColor);
	// Прозрачность...
	if(objSettings.objSchemes.getAlpha(iCode)==100 || Sets.bDisableTransparency!=0){
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
		SetWndAlpha(this->m_hWnd, objSettings.objSchemes.getAlpha(iCode),TRUE);
	}
	// Автопрозрачность сбрасывам...
	NoteTracker2.dwAddFlag1=IsAutoTransparencyEnabled()?(objSettings.objSchemes.getAlpha(iCode)):0;
	SetNoteInfo(0,0);// Убиваем таймер
	SetNoteInfo(&m_dwBrTitle,0);
	m_Title.Invalidate();
	m_Recipients.Invalidate();
	m_EditField.Invalidate(TRUE);
	Invalidate();
}

void CDLG_Note::AddOptionsSubMenu(CMenu* pOptions)
{
	AddMenuString(pOptions,ID_NOTEMENU_WORDWRAP,_l("Word-wrap")+"\tCtrl-W",NULL,Sets.bWordWrap);
	AddMenuString(pOptions,ID_NOTEMENU_AUTOHIDE,_l("Auto hide")+"\tCtrl-N",NULL,Sets.bAutoHide==1);
	AddMenuString(pOptions,ID_NOTEMENU_PREVENTFROMEDIT,_l("Prevent from edit")+"\tCtrl-E",NULL,Sets.GetReadOnly()==1);
	AddMenuString(pOptions,ID_NOTEMENU_FIXEDSIZE,_l("Fixed note size"),NULL,Sets.bFixedSize==1);
	AddMenuString(pOptions,ID_NOTEMENU_AUTODISP,_l("Click-through"),NULL,Sets.bMouseTrans==1);
	//AddMenuString(pOptions,IDC_NOTE_OPTDIALOG,_l("Edit list of a")+"\tCtrl-O",_bmp().Get(IDB_BM_OPTIONS));
	///////////////////////////////////////////////////////////////////////
	pOptions->AppendMenu(MF_SEPARATOR, 0, "");
	if(IsMessage()){
		AddMenuString(pOptions,ID_NOTEMENU_MESSAGEOPTIONS,_l("Message settings"));
		AddMenuString(pOptions,ID_NOTEMENU_NETWORKSETTINGS,_l("Network settings"));
	}
	AddMenuString(pOptions,ID_NOTEMENU_NOTEOPTIONS,_l("Note settings"));
}

void CDLG_Note::OnNoteOptmenu() 
{
	CMenu pOptions;
	pOptions.CreatePopupMenu();
	AddOptionsSubMenu(&pOptions);
	CRect pos;
	GetDlgItem(IDC_NOTE_OPTMENU)->GetWindowRect(&pos);
	pOptions.TrackPopupMenu(0, pos.left, pos.top, this);
}

void CDLG_Note::SetTitle(const char* szTitle) 
{
	CString sNewTitle=szTitle;
	if(sNewTitle.IsEmpty()){
		sNewTitle=Sets.GetTitle();
	}else{
		Sets.SetTitle(sNewTitle);
	}
	m_Title.SetWindowText(Sets.GetTitle());
	SetWindowText(Sets.GetTitle());
	bTitleChanged=0;
}

void CDLG_Note::GetTitleToSets()
{
	if(::IsWindow(m_Title.m_hWnd)){
		CString sTitleStr;
		m_Title.GetWindowText(sTitleStr);
		SetTitle(sTitleStr);
	}
}

void CDLG_Note::OnKillfocusTitle() 
{
	if(::IsWindow(m_Title.m_hWnd)){
		GetTitleToSets();
		UpdateTooltips();
	}
}

void CDLG_Note::OnPaint()
{
	CPaintDC dc(this);
	if(Sets.iIconMode){
		CRect rt;
		m_Title.GetWindowRect(&rt);
		ScreenToClient(&rt);
		CString sTitleStr;
		m_Title.GetWindowText(sTitleStr);
		if(objSettings.lShowTextInIcon && Sets.sText.GetLength()>0){
			sTitleStr+=": "+TrimMessage(Sets.sText,200,1);
			sTitleStr.Replace("\r","");
			sTitleStr.Replace("\n"," ");
			sTitleStr.Replace("\t"," ");
			while(sTitleStr.Replace("  "," ")){};
		}
		CFont* fn=dc.SelectObject(GetFont());
		DWORD dwCol=objSettings.objSchemes.getTxtColor(Sets.GetColorCode());
		dc.SetTextColor(dwCol);
		dc.SetBkColor(objSettings.objSchemes.getBgColor(Sets.GetColorCode()));
		dc.DrawText(sTitleStr,-1,&rt,DT_LEFT|DT_TOP|DT_WORDBREAK|DT_NOPREFIX|DT_WORD_ELLIPSIS);
		ValidateRect(&rt);
		dc.SelectObject(fn);
	}
}

void CDLG_Note::OnChangedTitle()
{
	bTitleChanged=1;
}

void CDLG_Note::OnNotemenuPrint() 
{
	if(objSettings.iLikStatus<=1){
		Alert(_l(UNREG_FUNC_TEXT),_l(UNREG_FUNC_TITLE));
		return;
	}
	GetNoteText();
	if(!PrintWindow(this,Sets.sText,Sets.GetTitle())){
		Alert(_l("Failed to print ")+" '"+Sets.GetTitle(10)+"'!");
	}
}

void CDLG_Note::OnNotemenuSendasemail() 
{
	if(objSettings.iLikStatus<=1){
		Alert(_l(UNREG_FUNC_TEXT),_l(UNREG_FUNC_TITLE));
		return;
	}
	CString sTitle=Sets.GetTitle();
	MakeSafeForURL(sTitle);
	GetNoteText();
	CString sText=Sets.sText;
	if(objSettings.iLikStatus==1){
		sText=_l("Please register WireNote")+"\n"+sText;
	}
	MakeSafeForURL(sText);
	int iMax=INTERNET_MAX_URL_LENGTH>MAX_PATH?MAX_PATH:INTERNET_MAX_URL_LENGTH;
	int iMaxLen=iMax-strlen(sTitle)-40;
	if(int(strlen(sText))>iMaxLen){
		int iPos=sText.Find("%",iMaxLen-3);
		if(iPos!=-1){
			sText=sText.Left(iPos);
		}
		sText=TrimMessage(sText,iMaxLen);
	}
	//ShellExecute(this->GetSafeHwnd(),NULL,(CString)"mailto:?Subject="+sTitle+"&body="+sText,"",NULL,SW_SHOWNORMAL);
	CString sSubj=(CString)"?Subject="+sTitle+"&body="+sText;
	CString sCom=(CString)"mailto:"+sSubj;
	SHELLEXECUTEINFO ExecInfo;
	memset(&ExecInfo,0,sizeof(ExecInfo));
	ExecInfo.cbSize=sizeof(ExecInfo);
	ExecInfo.lpFile=(const char*)sCom;
	//ExecInfo.lpParameters=(const char*)sSubj;
	ExecInfo.nShow=SW_MAXIMIZE;
	::ShellExecuteEx(&ExecInfo);
	return;
}


void CDLG_Note::OnNotemenuMsgFilter()
{
	objSettings.OpenOptionsDialog(_l(MSGFILTER_TOPIC),this);
}

void CDLG_Note::OnNotemenuNoteSettings()
{
	objSettings.OpenOptionsDialog(_l(NOTESETS_TOPIC),this);
}

void CDLG_Note::OnNotemenuMsgSettings()
{
	objSettings.OpenOptionsDialog(_l(MSGSETS_TOPIC),this);
}

void CDLG_Note::OnNotemenuNetSettings()
{
	objSettings.OpenOptionsDialog(_l(DEF_ACTIVEPROTS),this);
}

BOOL CDLG_Note::AddItemToAttachment(const char* szID)
{
	CItem* pItem=CItem::GetItemByID(szID);
	if(pItem){
		pItem->SaveToFile();
		CString sFile=pItem->GetItemFile();
		BOOL bRes=AddFileToAttachment(sFile, Format("wntype%lu",pItem->GetType()), pItem->GetTitle());
		RefreshAttachedFileList();
		return bRes;
	}
	return FALSE;
}

void CDLG_Note::OnRightclickedNoteRed() 
{
	Sets.SetColorCode(Sets.GetColorCode()+1);
	Invalidate();
}

void CDLG_Note::AppendSmiles(CMenu& Menu)
{
	CFileInfoArray& dir=GetSmilesEnum();
	if(dir.GetSize()){
		CMenu MenuSm;
		MenuSm.CreatePopupMenu();
		for(int i=0;i<dir.GetSize();i++){
			AddMenuString(&MenuSm,WM_USER+SMILS_BASE+i,_l(CString("SMILE_")+dir[i].GetFileTitle(),dir[i].GetFileTitle()),0);//_bmp().LoadBmpCached(dir[i].GetFilePath()));
		}
		int iIcon=theApp.dwMsgBitmap+3;
		AddMenuSubmenu(&Menu,&MenuSm,_l("Smiles")+"\t...",_bmp().Get(GetNoteIcon(0,&iIcon)));
	}
}

void CDLG_Note::OnNoteColorChange() 
{
	CMenu Menu;
	Menu.CreatePopupMenu();
	if(objSettings.objSchemes.getAlpha(Sets.GetColorCode())<100){
		AddMenuString(&Menu,ID_NOTEMENU_SWITCHTRANSPARENCY, (Sets.bDisableTransparency?_l("Enable opacity"):_l("Disable opacity")),_bmp().Get(_IL(51)));
		Menu.AppendMenu(MF_SEPARATOR, 0, "");
	}
	AppendStylesMenu(&Menu,&Sets,FALSE);
	Menu.AppendMenu(MF_SEPARATOR, 0, "");
	AppendSmiles(Menu);
	CRect rect;
	m_ColorRed.GetWindowRect(&rect);
	::TrackPopupMenu(Menu.GetSafeHmenu(), 0, rect.left, rect.top, 0, this->GetSafeHwnd(), NULL);
}

void CDLG_Note::OnKillfocusComboboxex() 
{
	Sets.sReplyAddressName=GetRecipient();
}

void CDLG_Note::OnSelchangeComboboxex()
{
	CMenu pRecRec;
	pRecRec.CreatePopupMenu();
	if(Sets.sReplyAddressName.GetLength()>0){
		if(objSettings.SaveMesNotesByDef){
			AddMenuString(&pRecRec, IDC_NOTE_HISTORY, _l("Message history")+"\tCtrl-L", _bmp().Get(IDB_BM_MSGLOG));
			pRecRec.AppendMenu(MF_SEPARATOR, 0, "");
		}
	}
	if(objSettings.lMsgRecentList.GetSize()>0){
		for(int i=0;i<objSettings.lMsgRecentList.GetSize();i++){
			CString sUser=TrimMessage(objSettings.lMsgRecentList[i],30,3);
			int iPerson=objSettings.AddrBook.IsPersonKnown(sUser);
			CBitmap* bmp=NULL;
			if(iPerson!=-1){
				Person* p=objSettings.AddrBook.aPersons[iPerson];
				bmp=_bmp().Get(IDB_BM_PONLINE+p->iOnlineStatus);
			}
			AddMenuString(&pRecRec, i+1, sUser, bmp);
		}
		AppendAdverts(&pRecRec);
		AddMenuString(&pRecRec, 1000, _l("Clear history"), _bmp().Get(IDB_BM_DEL));
	}else{
		AddMenuString(&pRecRec, 0, _l("History empty"));
	}
	CRect pos;
	DWORD iNum=0;
	if(GetDlgItem(IDC_COMBOBOXEX)->IsWindowVisible()){
		GetDlgItem(IDC_RECENTA)->GetWindowRect(&pos);
		iNum=pRecRec.TrackPopupMenu(TPM_RETURNCMD, pos.right, pos.top, this);
	}else{
		GetDlgItem(IDC_TEXTFIELD)->GetWindowRect(&pos);
		iNum=pRecRec.TrackPopupMenu(TPM_RETURNCMD, pos.left, pos.bottom, this);
	}
	if(iNum>0){
		if(ParseAdvert(iNum)){
			return;
		}
		if(iNum==IDC_NOTE_HISTORY){
			OnNotemenuMsglog();
		}else if(iNum==1000){
			objSettings.lMsgRecentList.RemoveAll();
		}else{
			dwLastChangeTime=GetTickCount();
			PutRecipient(objSettings.lMsgRecentList[iNum-1]);
			if(!objSettings.bUseNickOnSend){
				PutRecipient(objSettings.AddrBook.TranslateNickToIP(Sets.sReplyAddressName,NULL));
			}
			if(bNoteTitleChanged==0){
				Sets.SetTitle(Sets.GetMessageTitle(Sets.sReplyAddressName,FALSE));
			}
			ActivateNoteParams();
		}
	}
}

void CDLG_Note::OnChangeComboboxex() 
{
	dwLastChangeTime=GetTickCount();
	Sets.sReplyAddressName=GetRecipient();
}

void CDLG_Note::SetQuote(int iCode)
{
	BackToNormalView();
	GetNoteText();
	CString sOriginal=Sets.sText,sNewText;
	if(iCode>=0 && iCode<objSettings.lMsgQAnswers.GetSize()){
		Sets.sText=objSettings.lMsgQAnswers[iCode];
	}else{
		if(Sets.bIncomingMessage>0){
			Sets.sText="";
		}
	}
	Sets.sText+="\n";
	sNewText+="> ";
	sNewText+=TrimMessage(sOriginal,objSettings.sQuotationLen);
	sNewText.Replace("\n","\n>");
	Sets.sText+=sNewText;
	SetNoteText();
	// сбрасываем автоинформинг
	// и вообще весь аттачмент потому 
	// как обратно его слать нельзя!!!
	Sets.sAttachment="";
	if(!objSettings.bNoSend && Sets.bIncomingMessage>0 && iCode>=0){
		Sets.bDeleteAfterSend=1;
		OnSendmessage();
	}else{
		UpdateData(FALSE);
		m_EditField.SetFocus();
	}
}

void CDLG_Note::OnNoteQuote()
{
	CMenu pQAnswer;
	pQAnswer.CreatePopupMenu();
	if(Sets.bIncomingMessage>0){
		AddMenuString(&pQAnswer, 1000, _l("Quote message text")+"\t"+"Ctrl-Q");
		pQAnswer.AppendMenu(MF_SEPARATOR, 0, "");
	}
	if(objSettings.lMsgQAnswers.GetSize()>0){
		for(int i=0;i<objSettings.lMsgQAnswers.GetSize();i++){
			if(objSettings.lMsgQAnswers[i]!=""){
				AddMenuString(&pQAnswer, i+1, trimTextForMenu(objSettings.lMsgQAnswers[i])+((i<10)?(Format("\tCtrl-%c",'0'+i)):CString("")));
			}
		}
		pQAnswer.AppendMenu(MF_SEPARATOR, 0, "");
	}
	if(objSettings.lMsgQAnswers.GetSize()>0){
		AddMenuString(&pQAnswer, 2000, _l("Clear quick answers"));
		pQAnswer.AppendMenu(MF_SEPARATOR, 0, "");
	}
	for(int i=0;i<10;i++){
		AddMenuString(&pQAnswer, i+1001, Format("%s #%lu\tCtrl-Shift-%c",_l("Set as answer"),i+1,'0'+i));
	}
	CRect pos;
	GetDlgItem(IDC_NOTE_REPLY)->GetWindowRect(&pos);
	DWORD iNum=pQAnswer.TrackPopupMenu(TPM_RETURNCMD, pos.right, pos.top, this);
	if(iNum>0){
		if(iNum==1000){
			SetQuote(-1);
		}else if(iNum==2000){
			objSettings.lMsgQAnswers.RemoveAll();
		}else if(iNum>1000){
			CString sSelText=m_EditField.GetSelText();
			if(sSelText==""){
				GetNoteText();
				sSelText=Sets.sText;
			}
			if(sSelText!=""){
				objSettings.lMsgQAnswers.SetAtGrow(iNum-1001,sSelText);
			}
		}else{
			SetQuote(iNum-1);
		}
	}
}

void CDLG_Note::OnNotemenuSwitchTransp()
{
	Sets.bDisableTransparency=!Sets.bDisableTransparency;
	SetColorCode(Sets.GetColorCode());
}

void CDLG_Note::OnNotemenuClipSpy()
{
	if(lClipSpy){
		if(hNextClipboardChainWnd!=0){
			ChangeClipboardChain(hNextClipboardChainWnd);
		}
		hNextClipboardChainWnd=0;
		iClipCount=0;
		lClipSpy=0;
	}else{
		lClipSpy=1;
		iClipCount=0;
		OpenClipboard();
		EmptyClipboard();
		CloseClipboard();
		hNextClipboardChainWnd=SetClipboardViewer();
	}
}

void CDLG_Note::OnNotemenuAssignHK()
{
	objSettings.OpenOptionsDialog(_l(DEF_NAH), this,1,&Sets.hHotkey);
	RegHotkey();
}

afx_msg LRESULT CDLG_Note::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	ActivateNoteParams(1,0);
	SwitchToWindow(GetSafeHwnd());
	SetActiveWindow();
	m_EditField.SetFocus();
	return 0;
}

void CDLG_Note::OnItemShare(int iSection)
{
	Sets.ShareItem(!Sets.IsShared(),iSection);
}

void CDLG_Note::OnNotemenuFZFolder()
{
	CString sFFF=GetFBFZ();
	if(sFFF!=""){
		::ShellExecute(NULL,"open",sFFF,NULL,NULL,SW_SHOWNORMAL);
	}
}

BOOL CDLG_Note::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;
	GetWindowRect(&rt);
	ScreenToClient(&rt);
	long iCode=Sets.GetColorCode();
	DWORD cBGColor=objSettings.objSchemes.getBgColor(iCode);
	DWORD cTXColor=objSettings.objSchemes.getTxtColor(iCode);
	pDC->FillSolidRect(&rt,cBGColor);
	if(!Sets.iIconMode){
		CRect rt3;
		GetDlgItem(IDC_NOTE_ONTOP)->GetWindowRect(&rt3);
		ScreenToClient(&rt3);
		rt.top=rt3.top;
		rt.bottom=rt3.bottom+1;
		pDC->FillSolidRect(&rt,m_dwBrTitle);
		if(Sets.iViewMode>0){
			CRect rt2;
			GetDlgItem(IDC_NOTE_OPTMENU)->GetWindowRect(&rt2);
			ScreenToClient(&rt2);
			rt.top=rt2.top;
			rt.bottom=rt2.bottom+1;
			pDC->FillSolidRect(&rt,m_dwBrTitle);
			if(Sets.iViewMode==2){
				DWORD cBG2=Color2Color(cTXColor,cBGColor,BG1C,0);
				CRect rt1;
				GetDlgItem(IDC_SENDMESSAGE)->GetWindowRect(&rt1);
				ScreenToClient(&rt1);
				rt.top=rt1.top;
				rt.bottom=rt1.bottom;
				pDC->FillSolidRect(&rt,cBG2);
			}
		}
	}
	return 1;
}

HANDLEINTHREAD2(CDLG_Note,OnGSearch)
{
	CDLG_Note* pThisO=(CDLG_Note*)pThis;
	CString sSelText=pThisO->m_EditField.GetSelText();
	if(sSelText==""){
		pThisO->GetNoteText();
		sSelText=pThisO->Sets.sText;
	}
	sSelText.Replace("\r","");
	sSelText.Replace("\t","");
	sSelText.Replace("\n"," ");
	sSelText.TrimLeft();
	sSelText.TrimRight();
	if(sSelText!=""){
		//http://www.google.com/custom?domains=http%3A%2F%2Fwww.wiredplane.com&client=pub-5186671154037974&forid=1&q=
		CString sUrl="http://www.wiredplane.com/cgi-bin/wp_engine.php?who=WiteNote&target=search&text="+EscapeString2(sSelText);
		int iRes=(int)ShellExecute(0,"open",sUrl,NULL,NULL,SW_SHOWNORMAL);
	}
	return 0;
}

CString sBegin=Format("** %s **",_l("Script output"));
CString sEnd=Format("** %s **",_l("End of script output"));
void DeparseString(CString& s)
{
	CDataXMLSaver::StripInstringPart(sBegin,sEnd,s,0,1);
	s.TrimLeft();
	s.TrimRight();
}

HANDLEINTHREAD2(CDLG_Note,OnExeAsScr)
{
	CDLG_Note* pThisO=(CDLG_Note*)pThis;
	pThisO->GetNoteText();// Полюбому так как вставлять обратно
	DeparseString(pThisO->Sets.sText);
	CString sSelText=pThisO->m_EditField.GetSelText();
	if(sSelText==""){
		sSelText=pThisO->Sets.sText;
	}
	DeparseString(sSelText);
	if(sSelText!=""){
		if(CalculateExpression_Ole(sSelText)){
			if(sSelText!=""){
				//Alert(sSelText,_l("Script result"));
				sSelText.TrimLeft();
				sSelText.TrimRight();
				pThisO->Sets.sText=pThisO->Sets.sText+"\n"+sBegin+"\n"+sSelText+"\n"+sEnd;
				pThisO->SetNoteText();
			}
		}/*else{
			Alert(sSelText,_l("Script result"));
		}*/
	}
	return 0;
}

BOOL CDLG_Note::HandleLink(const char* sz)
{
	CString sAttach=sz;
	if(sAttach.Find("file://")==0){
		sAttach=sAttach.Mid(7);
		CString sAttName;
		DWORD dwAttSize;
		int iAttPosition=0;
		while(GetAttachByIndex(iAttPosition,sAttName,dwAttSize)){
			if(sAttName.CompareNoCase(sAttach)==0){
				PostMessage(WM_COMMAND,WM_USER+ATTACHMENT_SAVE+iAttPosition,0);
				return TRUE;
			}
			iAttPosition++;
		}
	}
	return FALSE;
}

void CDLG_Note::OnNotemenuRemembAsDef()
{
	objSettings.iDefNoteW=Sizer.m_rMain.Width();
	objSettings.iDefNoteH=Sizer.m_rMain.Height();
}