// DLG_AntiSpam.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "AntiSpamAccount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDLG_AntiSpam dialog
class CGetMsgInfo
{
public:
	CGetMsgInfo(CDLG_AntiSpam* pDialog,CEmailHeader* msg, int iMsgLin)
	{
		_this=pDialog;
		item=msg;
		iLines=iMsgLin;
	};
	CDLG_AntiSpam* _this;
	CEmailHeader* item;
	int iLines;
};

BOOL g_bDownloadInProgress=FALSE;
DWORD WINAPI GetMessageAgain(LPVOID pData)
{
	if(g_bDownloadInProgress){
		AfxMessageBox(_l("Download in progress, please wait")+"!");
		return 0;
	}
	g_bDownloadInProgress=TRUE;
	CGetMsgInfo* pOb=(CGetMsgInfo*)pData;
	pOb->_this->m_pAccount->RetrieveSingleMessage(pOb->item->iIndex,pOb->item,pOb->iLines);
	pOb->_this->RefreshSingleMessage(pOb->item);
	g_bDownloadInProgress=FALSE;
	delete pOb;
	return 0;
}

CDLG_AntiSpam::CDLG_AntiSpam(CAntiSpamAccount* pAccount, CWnd* pParent /*=NULL*/)
	: CDialog()
{
	font=NULL;
	font2=NULL;
	m_pAccount=pAccount;
	//{{AFX_DATA_INIT(CDLG_AntiSpam)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDLG_AntiSpam::~CDLG_AntiSpam()
{
	if(font){
		deleteFont(font);
	}
	if(font2){
		deleteFont(font2);
	}
}

void CDLG_AntiSpam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_AntiSpam)
	DDX_Control(pDX, IDC_CHECK_RUNEMAILC, m_AutoRunEm);
	DDX_Control(pDX, IDC_SPAMMSG, m_MsgList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_AntiSpam, CDialog)
	//{{AFX_MSG_MAP(CDLG_AntiSpam)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_SELALL, OnSelall)
	ON_BN_CLICKED(IDC_SELNOTHING, OnSelnothing)
	ON_BN_CLICKED(IDC_REFILTER, OnRefilter)
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_BUTTON_MULTY, OnButtonMulty)
	ON_BN_CLICKED(IDC_RUNEMAILCLIENT, OnRunemailclient)
	ON_BN_CLICKED(IDC_BUTTON_ADDSTR, OnButtonAddstr)
	ON_BN_CLICKED(IDC_BUTTON_EDITSTR, OnButtonEditstr)
	ON_BN_CLICKED(IDC_VIEWLOG, OnViewlog)
	ON_WM_DESTROY()
	ON_COMMAND(ID_RCLICK_TOGGLEMARK,OnRCLICK_TOGGLEMARK)
	ON_COMMAND(ID_RCLICK_REREAD,OnRCLICK_REREAD)
	ON_COMMAND(ID_RCLICK_DELETEFROMSERVER,OnRCLICK_DELETEFROMSERVER)
	ON_COMMAND(ID_RCLICK_SHOWBODY,OnRCLICK_SHOWBODY)
	ON_COMMAND(ID_RCLICK_SHOWRAWBODY,OnRCLICK_SHOWRAWBODY)
	ON_COMMAND(ID_RCLICK_SHOWALLHEADERS,OnRCLICK_SHOWALLHEADERS)
	ON_NOTIFY(FLMN_CHECKBOX_TRIGGER, IDC_SPAMMSG, OnCheckboxClick)
	ON_NOTIFY(FLNM_SELCHANGED, IDC_SPAMMSG, OnMsgSel)
	ON_NOTIFY(NM_DBLCLK, IDC_SPAMMSG, OnDblclkMsglist)
	ON_BN_CLICKED(IDC_CHECK_RUNEMAILC, OnCheckRunemailc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_AntiSpam message handlers

BOOL CDLG_AntiSpam::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(ParseAdvert(wParam)){
		return TRUE;
	}
	if(wParam==ID_RCLICK_DOWNLOAD20LINESOFBODY){
		OnRCLICK_DOWNLOADNEXTLINES(20);
		return TRUE;
	}
	if(wParam==ID_RCLICK_DOWNLOAD30LINESOFBODY){
		OnRCLICK_DOWNLOADNEXTLINES(30);
		return TRUE;
	}
	if(wParam==ID_RCLICK_DOWNLOAD40LINESOFBODY){
		OnRCLICK_DOWNLOADNEXTLINES(40);
		return TRUE;
	}
	if(wParam==ID_RCLICK_DOWNLOAD50LINESOFBODY){
		OnRCLICK_DOWNLOADNEXTLINES(50);
		return TRUE;
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CDLG_AntiSpam::PostNcDestroy() 
{
	m_pAccount->wnd=NULL;
	CDialog::PostNcDestroy();
	delete this;
}

CString CDLG_AntiSpam::GetEmailDsc(CEmailHeader* mail)
{
	CString sBody,sInfo;
	if(mail){
		sInfo+=Format("%s #%lu%s\n",_l("Message"),mail->iIndex+1,(mail->bBroken)?(CString(", ")+_l("Broken message")):"");
		if(mail->lMarkedForDeletion){
			sInfo=_l("This is a SPAM message");
			if(objSettings.lShowWhySpam){
				mail->sWhyMarked.TrimLeft();
				mail->sWhyMarked.TrimRight();
				if(mail->sWhyMarked.GetLength()>0){
					sInfo+=":\n";
					sInfo+=mail->sWhyMarked;
					sInfo+="\n";
				}
			}
			sInfo+="\n";
		}else if(mail->sWhyNotMarked!=""){
			sInfo=_l("This is a GOOD message");
			if(objSettings.lShowWhySpam){
				sInfo+=":\n";
				sInfo+=mail->sWhyNotMarked;
				sInfo+="\n";
			}
		}
		{// Стандратный заголовок описания сообщения
			if(mail->sFields[MAIL_FROMFIELD].GetLength()>0){
				sInfo+=Format("%s %s\n",_l("Message from"),mail->sFields[MAIL_FROMFIELD]);
			}
			if(mail->sFields[MAIL_TOFIELD].GetLength()>0){
				sInfo+=Format("%s: %s\n",_l("Recipient"),mail->sFields[MAIL_TOFIELD]);
			}
			if(mail->sFields[MAIL_SUBJFIELD].GetLength()>0){
				sInfo+=Format("%s: %s\n",_l("Subject"),mail->sFields[MAIL_SUBJFIELD]);
			}
			if(mail->dwSize>0){
				sInfo+=Format("%s: %lu\n",_l("Size"),mail->dwSize);
			}
		}
		sBody=SetTextForUpdateTipText(mail->sFields[MAIL_BDFLD_PR],30,TRUE);
		if(sBody.GetLength()!=0){
			sInfo+="\n";
			sInfo+=Format("%s %lu %s",_l("First"),mail->dwDownloadedLines,_l("line(s) of message"));
			sInfo+=":\n";
			sInfo+=sBody;
		}
	}
	return sInfo;
}

static DWORD dwLastServerLogChangeTime=0;
static CEmailHeader* iLastServerLogItem=NULL;
BOOL CDLG_AntiSpam::PreTranslateMessage(MSG* pMsg) 
{
	UINT iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	{
		m_tooltip.RelayEvent(pMsg);
	}
	BOOL bMouseOverServeLog=0;
	CRect rt;
	CPoint pt;
	GetCursorPos(&pt);
	GetDlgItem(IDC_SERVERLOG)->GetWindowRect(&rt);
	if(rt.PtInRect(pt)){
		bMouseOverServeLog=1;
	}
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==VK_RETURN && ::isPressed(VK_CONTROL)){
			PostMessage(WM_COMMAND,IDC_CLOSE,0);
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN && iCtrlID==IDC_EDIT_SPAMSTR){
			PostMessage(WM_COMMAND,IDC_BUTTON_ADDSTR,0);
			return TRUE;
		}
		if(pMsg->wParam==VK_SPACE && isPressed(VK_CONTROL)){
			PostMessage(WM_COMMAND,IDC_BUTTON_MULTY,0);
			return TRUE;
		}
	}
	if(isPressed(VK_CONTROL)){
		if(pMsg->wParam=='O'){
			OnConfig();
			return TRUE;
		}
	}
	if(iCtrlID==IDC_SERVERLOG && pMsg->message==WM_LBUTTONDBLCLK && iLastServerLogItem){
		ShowItemBody(iLastServerLogItem);
	}
	if(iCtrlID==IDC_SPAMMSG && pMsg->message==WM_LBUTTONDBLCLK){
		LRESULT lRes=0;
		OnDblclkMsglist(NULL,&lRes);
		if(lRes!=-1){
			return TRUE;
		}
	}
	if(iCtrlID==IDC_SPAMMSG && pMsg->message==WM_RBUTTONUP){
		OnRClick();
		return TRUE;
	}
	if(pMsg->message==WM_MOUSEWHEEL){
		CWnd* pWnd=&m_MsgList;
		if(bMouseOverServeLog){
			pWnd=GetDlgItem(IDC_SERVERLOG);
		}
		if(pWnd){
			if(pMsg->wParam&0x80000000){
				pWnd->PostMessage(WM_VSCROLL,SB_LINEDOWN,0);
			}else{
				pWnd->PostMessage(WM_VSCROLL,SB_LINEUP,0);
			}
		}
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONDOWN){
		if(iCtrlID==IDC_SPAMMSG){
			int iUnderCursorCol=-1;
			CEmailHeader* mail=GetItemUnderCursor(&iUnderCursorCol);
			ShowMsgInServerLog(mail);
		}
	}
	if(pMsg->message==WM_MOUSEMOVE){
		if(iCtrlID==IDC_SPAMMSG){
			static CEmailHeader* sLastUnderCursorItem=NULL;
			static int iLastUnderCursorCol=0;
			if(m_MsgList.GetIRowCount()>0){
				CString sInfo="";
				int iUnderCursorCol=-1;
				CEmailHeader* mail=GetItemUnderCursor(&iUnderCursorCol);
				if(mail!=sLastUnderCursorItem || iLastUnderCursorCol!=iUnderCursorCol){
					ttMail.ShowToolTip(TRUE);// Всегда ДО, иначе ширина тултипа не вычисляется иногда :(
					sInfo=GetEmailDsc(mail);
					if(sInfo.GetLength()==0){
						sInfo=_l("Received email(s), double click to see message header");
					}
					CRect rtMsg;
					GetDlgItem(IDC_SPAMMSG)->GetWindowRect(&rtMsg);
					ttMail.UpdateToolTipText(SetTextForUpdateTipText(sInfo,30,FALSE));
					CRect rtTooltip;
					::GetWindowRect(HWND(ttMail),&rtTooltip);
					int iTooltipX=rtMsg.right;
					if(objSettings.rDesktopRectangle.right-rtMsg.right<rtMsg.left-objSettings.rDesktopRectangle.left){
						iTooltipX=rtMsg.left-rtTooltip.Width();
					}
					ttMail.SetToolTipPosition(iTooltipX,rtMsg.top);
					sLastUnderCursorItem=mail;
					iLastUnderCursorCol=iUnderCursorCol;
				}
			}
			return TRUE;
		}else{
			ttMail.ShowToolTip(FALSE);
		}
		CWnd* wndFocus=GetFocus();
		UINT iFocusCtrlID=0;
		if(wndFocus){
			iFocusCtrlID=::GetDlgCtrlID(wndFocus->GetSafeHwnd());
		}
		if(dwLastServerLogChangeTime!=0){
			DWORD dwCheckTime=5000;
			if(iFocusCtrlID==IDC_SERVERLOG){
				dwCheckTime=20000;
			}
			if(GetTickCount()-dwLastServerLogChangeTime>dwCheckTime && !bMouseOverServeLog){
				dwLastServerLogChangeTime=0;
				iLastServerLogItem=NULL;
				GetDlgItem(IDC_SERVERLOG)->SetWindowText(sLog);
				GetDlgItem(IDC_SERVERLOG)->SetFont(font2?font2:this->GetFont(),TRUE);
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CDLG_AntiSpam::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_KILLFOCUS || message==0x036E/*WM_ACTIVATETOPLEVEL*/ || message==WM_ACTIVATE  || message==WM_ACTIVATEAPP || message==WM_MOUSEACTIVATE){
		ttMail.ShowToolTip(FALSE);
	}
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		if(message==WM_SIZE){
			CRect rt;
			m_MsgList.GetClientRect(&rt);
			objSettings.iSpamCol0Width=m_MsgList.GetColumnWidth(0);
			objSettings.iSpamCol1Width=m_MsgList.GetColumnWidth(1);
			objSettings.iSpamCol2Width=m_MsgList.GetColumnWidth(2);
			objSettings.iSpamCol1Width=rt.Width()-objSettings.iSpamCol0Width-objSettings.iSpamCol2Width-1;
			m_MsgList.SetColumnWidth(1,objSettings.iSpamCol1Width);
		}
		return DispatchResultFromSizer;
	}
	if(message==WM_REFRESHANTISPAM){
		CSmartLock l(m_pAccount->csMails);
		RefreshCurrentInfo();
		static int iLastMessagesCount=-1;
		int iMessagesCount=m_pAccount->messages.GetSize();
		if(iMessagesCount!=iLastMessagesCount || lParam==1){
			RefreshMessagesTable(FALSE);
			iMessagesCount=iLastMessagesCount;
		}
		return TRUE;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDLG_AntiSpam::OnRefilter() 
{
	if(m_pAccount->CheckRetrievedMessages(NULL,FALSE)){
		RefreshMessagesTable(FALSE);
	}
}

void CDLG_AntiSpam::OnSelall() 
{
	CSmartLock l(m_pAccount->csMails);
	for(int i=0;i<m_pAccount->messages.GetSize();i++){
		CEmailHeader& head=m_pAccount->messages[i];
		head.lMarkedForDeletion=1;
	}
	RefreshMessagesTable(FALSE);
}

long CDLG_AntiSpam::GetMarkedCount() 
{
	CSmartLock l(m_pAccount->csMails);
	long iCount=0;
	for(int i=0;i<m_pAccount->messages.GetSize();i++){
		CEmailHeader& head=m_pAccount->messages[i];
		if(head.lMarkedForDeletion){
			iCount++;
		}
	}
	return iCount;
}

void CDLG_AntiSpam::OnSelnothing() 
{
	CSmartLock l(m_pAccount->csMails);
	for(int i=0;i<m_pAccount->messages.GetSize();i++){
		CEmailHeader& head=m_pAccount->messages[i];
		head.lMarkedForDeletion=0;
	}
	RefreshMessagesTable(FALSE);
}

void CDLG_AntiSpam::OnInvert() 
{
	CSmartLock l(m_pAccount->csMails);
	for(int i=0;i<m_pAccount->messages.GetSize();i++){
		CEmailHeader& head=m_pAccount->messages[i];
		head.lMarkedForDeletion=1-head.lMarkedForDeletion;
	}
	RefreshMessagesTable(FALSE);
}

void CDLG_AntiSpam::OnConfig()
{
	static long lLock=0;
	if(lLock>0){
		// Процесс уже идет
		return;
	}
	SimpleTracker Track(lLock);
	objSettings.OpenOptionsDialog(_l(DEF_ANTISPAMTOPIC),this);
	m_AutoRunEm.SetCheck(m_pAccount->lRunEmailAfterUserCheck);
	UpdateData(FALSE);
}

void CDLG_AntiSpam::OnClose()
{
	if(m_pAccount->lCurrentStatus==2){
		if(GetMarkedCount()>0){
			DWORD dwReply=AskYesNo(_l("Finish check","Do you want to kill marked messages")+"?",&objSettings.lOnCloseSpamWindow,this);
			if(dwReply!=IDYES){
				objSettings.lOnCloseSpamWindow=0;
				OnSelnothing();
			}
		}
		SetEvent(m_pAccount->hWaitForDelete);
	}
	ttMail.ShowToolTip(FALSE);
	ShowWindowAsync(GetSafeHwnd(),SW_HIDE);
}

void CDLG_AntiSpam::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID==SC_CLOSE){
		PostMessage(WM_COMMAND,IDC_CLOSE,0);
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

BOOL CDLG_AntiSpam::SetTitle(const char* szPrefix)
{
	CString sTitle=m_pAccount->GetAccountName()+" - "+_l("Anti-SPAM mail checker window");
	if(szPrefix && strlen(szPrefix)>0){
		SetWindowText(CString(szPrefix)+" - "+sTitle);
	}else{
		SetWindowText(sTitle);
	}
	return TRUE;
}

BOOL CDLG_AntiSpam::OnInitDialog() 
{
	//m_tooltip.SetAutoTrack(1);
	if(objSettings.bShadows && !IsVista()){
		SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW//ModifyStyleEx(0,WS_EX_COMPOSITED,SWP_FRAMECHANGED);
	}
	CDialog::OnInitDialog();
	SetTitle("");
	SetIcon(::AfxGetApp()->LoadIcon(IDI_ICON_MAIL),TRUE);
	SetIcon(::AfxGetApp()->LoadIcon(IDI_ICON_MAIL),FALSE);
	COleDateTime dt=COleDateTime::GetCurrentTime();
	{
		Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_AntiSpam_"+m_pAccount->sAccount);
		CRect minRect(0,0,(int)(DIALOG_MINXSIZE*1.5),(int)(DIALOG_MINYSIZE*1.6));
		Sizer.SetMinRect(minRect);
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
		Sizer.AddDialogElement(IDC_SPAMMSG);
		Sizer.AddDialogElement(IDC_CURRENTSTEP);
		Sizer.AddDialogElement(IDC_CLOSE,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_REFILTER,widthSize|heightSize,IDC_CLOSE);
		Sizer.AddDialogElement(IDC_BUTTON_MULTY,widthSize|heightSize,IDC_CLOSE);
		Sizer.AddDialogElement(IDC_SELALL,widthSize|heightSize|hidable,IDC_CLOSE);
		Sizer.AddDialogElement(IDC_SELNOTHING,widthSize|heightSize|hidable,IDC_CLOSE);
		Sizer.AddDialogElement(IDC_CONFIG,widthSize|heightSize|hidable,IDC_CLOSE);
		Sizer.AddDialogElement(IDC_RUNEMAILCLIENT,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_VIEWLOG,widthSize|heightSize|hidable,IDC_CLOSE);
		Sizer.AddDialogElement(IDC_BUTTON_ADDSTR,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_BUTTON_EDITSTR,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_EDIT_SPAMSTR,heightSize);
		Sizer.AddDialogElement(IDC_SPAMTYPE,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_SPAMTYPE2,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_ADDTOSPAM,heightSize|transparent);
		Sizer.AddDialogElement(IDC_CHECK_RUNEMAILC,heightSize);
		Sizer.AddDialogElement(IDC_SERVERLOG,bottomResize);
		//
		Sizer.AddLeftTopAlign(IDC_CURRENTSTEP,0,10);
		Sizer.AddRightTopAlign(IDC_BUTTON_MULTY,0,10);
		Sizer.AddLeftAlign(IDC_SERVERLOG,0,10);
		Sizer.AddLeftAlign(IDC_SPAMMSG,0,10);
		Sizer.AddLeftAlign(IDC_ADDTOSPAM,0,10);
		Sizer.AddLeftAlign(IDC_RUNEMAILCLIENT,0,10);
		Sizer.AddTopAlign(IDC_SERVERLOG,IDC_BUTTON_MULTY,bottomPos,4);
		Sizer.AddRightAlign(IDC_SERVERLOG,IDC_BUTTON_MULTY,leftPos,-4);
		Sizer.AddRightAlign(IDC_CURRENTSTEP,IDC_BUTTON_MULTY,leftPos,-4);
		Sizer.AddTopAlign(IDC_VIEWLOG,IDC_BUTTON_MULTY,bottomPos,4);
		Sizer.AddTopAlign(IDC_REFILTER,IDC_VIEWLOG,bottomPos,4);
		Sizer.AddTopAlign(IDC_SELALL,IDC_REFILTER,bottomPos,4);
		Sizer.AddTopAlign(IDC_SELNOTHING,IDC_SELALL,bottomPos,4);
		Sizer.AddTopAlign(IDC_SPAMMSG,IDC_SERVERLOG,bottomPos,4);

		Sizer.AddRightAlign(IDC_ADDTOSPAM,0,-10);
		Sizer.AddRightAlign(IDC_SPAMMSG,0,-10);
		Sizer.AddRightAlign(IDC_REFILTER,0,-10);
		Sizer.AddRightAlign(IDC_VIEWLOG,0,-10);
		Sizer.AddRightAlign(IDC_CLOSE,0,-10);
		Sizer.AddRightAlign(IDC_SELALL,0,-10);
		Sizer.AddRightAlign(IDC_SELNOTHING,0,-10);

		Sizer.AddBottomAlign(IDC_CONFIG,0,-10);
		Sizer.AddBottomAlign(IDC_CLOSE,0,-10);
		Sizer.AddBottomAlign(IDC_RUNEMAILCLIENT,0,-10);
		Sizer.AddBottomAlign(IDC_CHECK_RUNEMAILC,0,-10);
		Sizer.AddLeftAlign(IDC_CONFIG,IDC_RUNEMAILCLIENT,rightPos,4);
		Sizer.AddLeftAlign(IDC_CHECK_RUNEMAILC,IDC_CONFIG,rightPos,4);
		Sizer.AddRightAlign(IDC_CHECK_RUNEMAILC,IDC_CLOSE,leftPos,-4);

		Sizer.AddBottomAlign(IDC_ADDTOSPAM,IDC_CLOSE,topPos,-4);
		Sizer.AddSideCenterAlign(IDC_SPAMTYPE,IDC_ADDTOSPAM,sideCenter,6);
		Sizer.AddSideCenterAlign(IDC_SPAMTYPE2,IDC_ADDTOSPAM,sideCenter,6);
		Sizer.AddSideCenterAlign(IDC_BUTTON_EDITSTR,IDC_ADDTOSPAM,sideCenter,6);
		Sizer.AddSideCenterAlign(IDC_BUTTON_ADDSTR,IDC_ADDTOSPAM,sideCenter,6);
		Sizer.AddSideCenterAlign(IDC_EDIT_SPAMSTR,IDC_ADDTOSPAM,sideCenter,6);
		Sizer.AddLeftAlign(IDC_EDIT_SPAMSTR,IDC_ADDTOSPAM,leftPos,4);
		Sizer.AddRightAlign(IDC_BUTTON_EDITSTR,IDC_ADDTOSPAM,rightPos,-4);
		Sizer.AddRightAlign(IDC_BUTTON_ADDSTR,IDC_BUTTON_EDITSTR,leftPos,-4);
		Sizer.AddRightAlign(IDC_SPAMTYPE2,IDC_BUTTON_ADDSTR,leftPos,-4);
		Sizer.AddRightAlign(IDC_SPAMTYPE,IDC_SPAMTYPE2,leftPos,-4);
		Sizer.AddRightAlign(IDC_EDIT_SPAMSTR,IDC_SPAMTYPE,leftPos,-4);
		Sizer.AddBottomAlign(IDC_SPAMMSG,IDC_ADDTOSPAM,topPos,-4);
		Sizer.AddBottomAlign(IDC_SERVERLOG,IDC_SELNOTHING,topPos|ifLess,0);
		Sizer.AddBottomAlign(IDC_SERVERLOG,IDC_SPAMMSG,bottomPos|ifBigger,-50);

	}
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->AddString(_l("'Subject' field"));
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->AddString(_l("'From' field"));
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->AddString(_l("Message text"));
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->AddString(_l("RFC Headers"));
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE2))->AddString(_l("Black"));
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE2))->AddString(_l("White"));
	((CComboBox*)GetDlgItem(IDC_SPAMTYPE2))->SetCurSel(0);
	GetDlgItem(IDC_VIEWLOG)->SetWindowText(_l("View log"));
	GetDlgItem(IDC_REFILTER)->SetWindowText(_l("Re-filter"));
	GetDlgItem(IDC_CLOSE)->SetWindowText(_l("Close"));
	GetDlgItem(IDC_CONFIG)->SetWindowText(_l("Configure"));
	GetDlgItem(IDC_SELNOTHING)->SetWindowText(_l("Mark none"));
	GetDlgItem(IDC_ADDTOSPAM)->SetWindowText(_l("New rule: If the message contains text in the field specified, treat email as stated below")+":");
	GetDlgItem(IDC_BUTTON_ADDSTR)->SetWindowText(_l("Add"));
	GetDlgItem(IDC_BUTTON_EDITSTR)->SetWindowText("...");
	GetDlgItem(IDC_SELALL)->SetWindowText(_l("Mark all"));
	GetDlgItem(IDC_RUNEMAILCLIENT)->SetWindowText(_l("Run email client"));
	GetDlgItem(IDC_CHECK_RUNEMAILC)->SetWindowText(_l("Run email client after check"));
	m_AutoRunEm.SetCheck(m_pAccount->lRunEmailAfterUserCheck);
	SetTaskbarButton(this->GetSafeHwnd());
	{
		m_tooltip.Create(this);
		m_tooltip.SetMaxTipWidth(objSettings.rDesktopRectangle.Width()/2);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDC_CHECK_RUNEMAILC), _l("Run email client after check"));
		m_tooltip.AddTool(GetDlgItem(IDC_REFILTER), _l("Re-filter received messages"));
		m_tooltip.AddTool(GetDlgItem(IDC_CLOSE), _l("Close this window"));
		m_tooltip.AddTool(GetDlgItem(IDC_SELNOTHING), _l("Deselect all messages"));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_ADDSTR), _l("Add string to file with spam-strings"));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_EDITSTR), _l("Edit file with spam-strings"));
		m_tooltip.AddTool(GetDlgItem(IDC_SELALL), _l("Select all messages"));
		m_tooltip.AddTool(GetDlgItem(IDC_RUNEMAILCLIENT), _l("Run default email client"));
		//m_tooltip.AddTool(GetDlgItem(IDC_SPAMMSG), _l("Received messages"));
		m_tooltip.AddTool(GetDlgItem(IDC_CURRENTSTEP), _l("Current step"));
		m_tooltip.AddTool(GetDlgItem(IDC_SERVERLOG), _l("Log window"));
		m_tooltip.AddTool(GetDlgItem(IDC_EDIT_SPAMSTR), _l("String to be added to spam-strings file"));
		m_tooltip.AddTool(GetDlgItem(IDC_VIEWLOG), _l("View anti-spam log file"));
		m_tooltip.AddTool(GetDlgItem(IDC_SPAMTYPE), _l("Type of the search string file"));
		m_tooltip.AddTool(GetDlgItem(IDC_SPAMTYPE2), _l("Type of search string, black - to mark email as a SPAM, white - as a good message"));
		m_tooltip.AddTool(GetDlgItem(IDC_CONFIG), _l(OPTIONS_TPM)+" (Ctrl-O)");
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_MULTY), _l("Check mailbox NOW")+" (Ctrl-Space)");
		ttMail.Create(GetSafeHwnd(), "...", AfxGetResourceHandle(), TTS_NOPREFIX, NULL);//TTS_BALLOON
	}
	InitMsgList();
	RefreshCurrentInfo();
	RefreshMessagesTable();
	return TRUE;
}

void CDLG_AntiSpam::InitMsgList()
{
	m_MsgList.SetReadOnly(FALSE);
	m_MsgList.SetImageList(&theApp.MainImageList, LVSIL_SMALL);
	ListView_SetExtendedListViewStyleEx(m_MsgList.m_hWnd, 0, LVS_EX_FULLROWSELECT);
	objSettings.iSpamCol0Width=Validate(objSettings.iSpamCol0Width, 0, 300, 100);
	objSettings.iSpamCol1Width=Validate(objSettings.iSpamCol1Width, 20, 500, 100);
	objSettings.iSpamCol2Width=Validate(objSettings.iSpamCol2Width, 0, 300, 100);
	m_MsgList.InsertFColumn( 0, _l("Kill"), FALSE, LVCFMT_LEFT, objSettings.iSpamCol0Width);
	m_MsgList.SetIItemControlType(NULL, 0);
	m_MsgList.InsertFColumn( 1, _l("'Subject' field"), FALSE, LVCFMT_LEFT, objSettings.iSpamCol1Width);
	m_MsgList.SetIItemControlType(NULL, 1);
	m_MsgList.InsertFColumn( 2, _l("'From' field"), FALSE, LVCFMT_LEFT, objSettings.iSpamCol2Width);
	m_MsgList.SetIItemControlType(NULL, 2);
	m_MsgList.DeleteAllIRows();
	SetColorStyle();
}

void CDLG_AntiSpam::SetColorStyle() 
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
	m_MsgList.m_crIItemText=objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	m_MsgList.m_crIRowBackground=objSettings.objSchemes.getBgColor(objSettings.iMainStyle);
	m_MsgList.SetBkColor(objSettings.objSchemes.getBgColor(objSettings.iMainStyle));
	m_MsgList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);//objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	m_MsgList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);//objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	m_MsgList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);//objSettings.objSchemes.getBgColor(objSettings.iMainStyle);

	// Шрифт
	objSettings.objSchemes.getFont(objSettings.iMainStyle)->GetLogFont(&logfont);
	deleteFont(font);
	font=new CFont();
	font->CreateFontIndirect(&logfont);
	m_MsgList.SetFont(font,FALSE);
	deleteFont(font2);
	font2=new CFont();
	logfont.lfPitchAndFamily=FIXED_PITCH|FF_MODERN;
	strcpy(logfont.lfFaceName,"");//"Courier"
	font2->CreateFontIndirect(&logfont);
	GetDlgItem(IDC_SERVERLOG)->SetFont(font2,FALSE);
	return;
}

void CDLG_AntiSpam::RefreshCurrentInfo()
{
	CString sCurStatInfo=_l("Unknown");
	CString sMulty=_l("Abort");
	switch(m_pAccount->lCurrentStatus)
	{
	case 0:
		{
			sCurStatInfo=_l(" Waiting for next check time")+"...";
			sMulty=_l("Check now");
			GetDlgItem(IDC_BUTTON_MULTY)->EnableWindow(TRUE);
			m_tooltip.UpdateTipText(_l("Check mailbox NOW")+" (Ctrl-Space)",GetDlgItem(IDC_BUTTON_MULTY));
			//--------------------------------------------
			GetDlgItem(IDC_REFILTER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELNOTHING)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELALL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RUNEMAILCLIENT)->EnableWindow(TRUE);
			break;
		}
	case 1:
		{
			sCurStatInfo=_l(" Retrieving messages")+"...";
			sMulty=_l("Abort");
			GetDlgItem(IDC_BUTTON_MULTY)->EnableWindow(TRUE);
			m_tooltip.UpdateTipText(_l("Abort message retrieving")+" (Ctrl-Space)",GetDlgItem(IDC_BUTTON_MULTY));
			//--------------------------------------------
			GetDlgItem(IDC_REFILTER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELNOTHING)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELALL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RUNEMAILCLIENT)->EnableWindow(FALSE);
			break;
		}
	case 2:
		{
			if(!m_pAccount->lCurrentSubStatus){
				sCurStatInfo=_l(" Checking retrieved messages")+"...";
			}else{
				sCurStatInfo=CString(" ")+_l("Mark SPAM messages and press 'Finish'");
			}
			sMulty=_l("Finish");
			GetDlgItem(IDC_BUTTON_MULTY)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_MULTY)->SetFocus();
			m_tooltip.UpdateTipText(_l("Delete marked email(s) and finish check")+" (Ctrl-Space)",GetDlgItem(IDC_BUTTON_MULTY));
			//--------------------------------------------
			GetDlgItem(IDC_REFILTER)->EnableWindow(TRUE);
			GetDlgItem(IDC_SELNOTHING)->EnableWindow(TRUE);
			GetDlgItem(IDC_SELALL)->EnableWindow(TRUE);
			GetDlgItem(IDC_RUNEMAILCLIENT)->EnableWindow(FALSE);
			break;
		}
	case 3:
		{
			sCurStatInfo=_l(" Deleting spam messages")+"...";
			sMulty=_l("Abort");
			GetDlgItem(IDC_BUTTON_MULTY)->EnableWindow(TRUE);
			m_tooltip.UpdateTipText(_l("Abort deleting")+" (Ctrl-Space)",GetDlgItem(IDC_BUTTON_MULTY));
			//--------------------------------------------
			GetDlgItem(IDC_REFILTER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELNOTHING)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELALL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RUNEMAILCLIENT)->EnableWindow(FALSE);
			break;
		}
	case -1:
		{
			sCurStatInfo=_l(" Aborting check")+"...";
			sMulty=_l("Abort");
			GetDlgItem(IDC_BUTTON_MULTY)->EnableWindow(FALSE);
			//--------------------------------------------
			GetDlgItem(IDC_REFILTER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELNOTHING)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELALL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RUNEMAILCLIENT)->EnableWindow(FALSE);
			break;
		}
	}
	GetDlgItem(IDC_BUTTON_MULTY)->SetWindowText(sMulty);
	GetDlgItem(IDC_CURRENTSTEP)->SetWindowText(sCurStatInfo);
	sLog=m_pAccount->sLogString;
	int iLines=sLog.Replace("\n","\r\n");
	GetDlgItem(IDC_SERVERLOG)->SetWindowText(sLog);
	GetDlgItem(IDC_SERVERLOG)->SetFont(font2?font2:this->GetFont(),TRUE);
	return;
}

void CDLG_AntiSpam::OnButtonMulty() 
{
	switch(m_pAccount->lCurrentStatus)
	{
	case 0:
		{
			FORK(CheckMailInTread,m_pAccount);
			break;
		}
	case 1:
		{
			m_pAccount->lCurrentStatus=-1;
			break;
		}
	case 2:
		{
			SetEvent(m_pAccount->hWaitForDelete);
			break;
		}
	case 3:
		{
			PostMessage(WM_COMMAND,IDC_CLOSE,0);
			break;
		}
	}
	return;
}

HANDLEINTHREAD(CDLG_AntiSpam,OnRunemailclient) 
{
	if(objSettings.sUserDefinedEmailB!=""){
		::ShellExecute(NULL,"open",objSettings.sUserDefinedEmailB,NULL,NULL,SW_SHOWNORMAL);
	}else{
		RunDefaultEmailClient();
	}
	return 0;
}

const char* CDLG_AntiSpam::GetSpamTargetFile()
{
	int iSelType=((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->GetCurSel();
	const char* szTargetFile=objSettings.sSpamBodyFile;
	if(iSelType==0){
		szTargetFile=objSettings.sSpamSubjectsFile;
	}else if(iSelType==1){
		szTargetFile=objSettings.sSpamAddressesFile;
	}else if(iSelType==3){
		szTargetFile=objSettings.sSpamHeadFile;
	}
	return szTargetFile;
}

void CDLG_AntiSpam::OnButtonAddstr() 
{
	static long lLock=0;
	if(lLock>0){
		// Процесс уже идет
		return;
	}
	SimpleTracker Track(lLock);
	//
	CString sText;
	GetDlgItem(IDC_EDIT_SPAMSTR)->GetWindowText(sText);
	GetDlgItem(IDC_EDIT_SPAMSTR)->SetWindowText("");
	const char* szTargetFile=GetSpamTargetFile();
	if(!szTargetFile || sText.GetLength()==0){
		return;
	}
	int iListType=((CComboBox*)GetDlgItem(IDC_SPAMTYPE2))->GetCurSel();
	CString sWordPrefix="";
	if(iListType!=0){
		sText+="\t"MSG_WHITE_PART;
	}
	PrepareSpamFiles();
	CString sContent="";
	ReadFile(szTargetFile,sContent);
	sContent.Replace("\r\n","\n");
	sText.TrimLeft();
	sText.TrimRight();
	CString sNewContent="\n";
	sNewContent+=sContent;
	sNewContent+="\n";
	CString sNewText="\n";
	sNewText+=sText;
	sNewText+="\n";
	if(sNewContent.Find(sNewText)!=-1){
		Alert(_l("Such string already defined")+"!",_l("Spam-strings file"),3000);
		return;
	}
	sContent=sText;
	sContent+=sNewContent;
	sContent.TrimLeft();
	sContent.TrimRight();
	while(sContent.Replace("\n\n","\n")!=0);
	sContent.Replace("\n","\r\n");
	if(SaveFile(szTargetFile,sContent)){
		Alert(_l("String added successfully")+"!",_l("Spam-strings file"),3000);
	}
}

HANDLEINTHREAD2(CDLG_AntiSpam,OnButtonEditstr) 
{
	CDLG_AntiSpam* pThisO=(CDLG_AntiSpam*) pThis;
	CString szTargetFile=pThisO->GetSpamTargetFile();
	if(szTargetFile!=""){
		::ShellExecute(0,"open",szTargetFile,NULL,NULL,SW_SHOWMAXIMIZED);
	}
	return 0;
}

void CDLG_AntiSpam::OnViewlog() 
{
	if(pMainDialogWindow){
		pMainDialogWindow->OpenMsgLogOnNick(DEFAULT_ANTISPAMLOG);
	}
}

void CDLG_AntiSpam::RefreshSingleMessage(CEmailHeader* head)
{
	CSmartLock l(m_pAccount->csMails);
	if(head==NULL){
		return;
	}
	if(head->hRow==NULL){
		head->hRow=m_MsgList.InsertIRow(FL_ROOT, FL_LAST, "");
	}
	if(m_pAccount->lCurrentStatus==0 || head->bDeleted!=0){
		m_MsgList.SetIItemControlType(head->hRow, 0, FLC_READONLY);
		m_MsgList.SetIItemText(head->hRow, 0, head->bDeleted==1?_l("Killed"):"");
	}else{
		m_MsgList.SetIItemControlType(head->hRow, 0, FLC_CHECKBOX);
		m_MsgList.SetIItemCheck(head->hRow, 0, head->lMarkedForDeletion, TRUE, FALSE);
		m_MsgList.SetIItemText(head->hRow, 0, Format("%i.",head->iIndex+1));
	}
	if(head->sFields.GetSize()<MAIL_HEADERS){
		// Бред, не должно быть!
		#ifdef _DEBUG
		//DbgBreak();
		#endif
		return;
	}
	{// Если subject пуст, начинаем извращаться
		CString sHeadSubj=head->sFields[MAIL_SUBJFIELD];
		if(sHeadSubj.GetLength()==0){
			CString sLinedBody=head->sFields[MAIL_BDFLD_PR];
			sLinedBody.Replace("\n"," ");
			sLinedBody.Replace("\r"," ");
			sLinedBody.Replace("\t"," ");
			sHeadSubj=Format("<%s>",_l("No subject"));
			if(sLinedBody.GetLength()>0){
				sHeadSubj+=Format(", %s",sLinedBody);
			}else if(head->sFields[MAIL_TOFIELD]!=""){
				sHeadSubj+=Format(", %s: %s",_l("Message recipient"),head->sFields[MAIL_TOFIELD]);
			}
		}
		if(head->bBroken){
			sHeadSubj+=Format(", %s. %s: %lu",_l("Was not loaded successfully"),_l("Size"),head->dwSize);
		}
		//sHeadSubj+=Format(", %s: %lu",_l("Size"),head.dwSize);
		m_MsgList.SetIItemText(head->hRow, 1, sHeadSubj);
	}
	m_MsgList.SetIItemText(head->hRow, 2, head->sFields[MAIL_FROMFIELD]);
	if(head->bBroken){
		m_MsgList.SetIItemImage(head->hRow, 1, 42);
	}else if(head->bAttachment){
		m_MsgList.SetIItemImage(head->hRow, 1, 21);
	}else{
		m_MsgList.SetIItemImage(head->hRow, 1, 44);
	}
	m_MsgList.SetIItemData(head->hRow,0,head->iIndex);
	m_MsgList.SetIItemData(head->hRow,1,head->iIndex);
	m_MsgList.SetIItemData(head->hRow,2,head->iIndex);
	m_MsgList.SetIRowData(head->hRow,head->iIndex);
	m_MsgList.UpdateIRow(head->hRow);
}

void CDLG_AntiSpam::RefreshMessagesTable(BOOL bClearFirst)
{
	CSmartLock l(m_pAccount->csMails);
	if(bClearFirst || m_pAccount->messages.GetSize()==0){
		for(int i=0;i<m_pAccount->messages.GetSize();i++){
			CEmailHeader& head=m_pAccount->messages[i];
			head.hRow=NULL;
		}
		m_MsgList.DeleteAllIRows();
		m_tooltip.UpdateTipText(_l("Received messages"),&m_MsgList);
	}
	if(m_pAccount->messages.GetSize()>0){
		for(int i=0;i<m_pAccount->messages.GetSize();i++){
			CEmailHeader& head=m_pAccount->messages[i];
			RefreshSingleMessage(&head);
		}
	}
}

void CDLG_AntiSpam::OnDestroy() 
{
	objSettings.iSpamCol0Width=m_MsgList.GetFColumnWidth(0,0);
	objSettings.iSpamCol1Width=m_MsgList.GetFColumnWidth(1,0);
	objSettings.iSpamCol2Width=m_MsgList.GetFColumnWidth(2,0);
	//Sizer.SaveSizeAndPosToRegistry();
	CDialog::OnDestroy();
}

CEmailHeader* CDLG_AntiSpam::GetSelectedMessage(int* iSelectedCol)
{
	CSmartLock l(m_pAccount->csMails);
	HIROW hRow=m_MsgList.GetSelectedIRow();
	if(hRow==NULL){
		return NULL;
	}
	int iCol=m_MsgList.GetSelectedIItem();
	if(iCol!=1 && iCol!=2){
		return NULL;
	}
	int iMessageIndex=m_MsgList.GetIRowData(hRow);
	if(iMessageIndex<0 || iMessageIndex>=m_pAccount->messages.GetSize()){
		return NULL;
	}
	if(iSelectedCol){
		*iSelectedCol=iCol;
	}
	return &m_pAccount->messages[iMessageIndex];
}

void CDLG_AntiSpam::OnRClick()
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item){
		return;
	}
	CMenu menu;
	menu.CreatePopupMenu();
	menu.SetDefaultItem(0,TRUE);
	if(m_pAccount->lCurrentStatus==2){
		AddMenuString(&menu,ID_RCLICK_TOGGLEMARK,item->lMarkedForDeletion?_l("Clear SPAM mark"):_l("Mark as SPAM"));
		if(item->bBroken){
			AddMenuString(&menu,ID_RCLICK_REREAD,_l("Re-read from server"),_bmp().Get(IDB_BM_REFRESH));
		}
		if(!item->bDeleted){
			AddMenuString(&menu,ID_RCLICK_DELETEFROMSERVER,_l("Delete from server"),_bmp().Get(IDB_BM_DEL));
		}
		{
			CMenu linesMenu;
			linesMenu.CreatePopupMenu();
			AddMenuString(&linesMenu,ID_RCLICK_DOWNLOAD20LINESOFBODY,_l("Download 20 lines of body"));
			AddMenuString(&linesMenu,ID_RCLICK_DOWNLOAD30LINESOFBODY,_l("Download 30 lines of body"));
			AddMenuString(&linesMenu,ID_RCLICK_DOWNLOAD40LINESOFBODY,_l("Download 40 lines of body"));
			AddMenuString(&linesMenu,ID_RCLICK_DOWNLOAD50LINESOFBODY,_l("Download 50 lines of body"));
			AddMenuSubmenu(&menu,&linesMenu,_l("Download more lines")+"\t...",_bmp().Get(_IL(22)));
		}
	}
	if(!item->bBroken){
		AddMenuString(&menu,ID_RCLICK_SHOWBODY,_l("Show message body"),_bmp().Get(_IL(6)));
		AppendAdverts(&menu);
		if(item->sFields[MAIL_BDFLD_PR]!=item->sFields[MAIL_BODYFIELD]){
			AddMenuString(&menu,ID_RCLICK_SHOWRAWBODY,_l("Show raw body"),_bmp().Get(_IL(6)));
		}
		AddMenuString(&menu,ID_RCLICK_SHOWALLHEADERS,_l("Show all headers"),_bmp().Get(_IL(6)));
	}
	if(menu.GetMenuItemCount()>0){
		CPoint pos;
		GetCursorPos(&pos);
		::TrackPopupMenu(menu.GetSafeHmenu(), 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	}
}

void CDLG_AntiSpam::ShowMsgInServerLog(CEmailHeader* item)
{
	if(!item){
		return;
	}
	CString sServerLogInfo=GetEmailDsc(item);
	sServerLogInfo.Replace("\n","\r\n");
	GetDlgItem(IDC_SERVERLOG)->SetWindowText(sServerLogInfo);
	GetDlgItem(IDC_SERVERLOG)->SetFont(this->GetFont(),TRUE);
	dwLastServerLogChangeTime=GetTickCount();
	iLastServerLogItem=item;
}

void CDLG_AntiSpam::OnMsgSel(NMHDR * pNotifyStruct, LRESULT * result)
{
	int iCol=0;
	CEmailHeader* item=GetSelectedMessage(&iCol);
	if(!item){
		*result=-1;
		return;
	}
	if(iCol==1){
		GetDlgItem(IDC_EDIT_SPAMSTR)->SetWindowText(item->sFields[MAIL_SUBJFIELD]);
		((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->SetCurSel(0);
	}else{
		GetDlgItem(IDC_EDIT_SPAMSTR)->SetWindowText(item->sFields[MAIL_FROMFIELD]);
		((CComboBox*)GetDlgItem(IDC_SPAMTYPE))->SetCurSel(1);
	}
	ShowMsgInServerLog(item);
	return;
}

void CDLG_AntiSpam::OnCheckboxClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	CSmartLock l(m_pAccount->csMails);
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(!pRowState || pRowState->iIItem!=0){
		*result=-1;
		return;
	}
	int iMessageIndex=pRowState->dwData;
	if(iMessageIndex<0 || iMessageIndex>=m_pAccount->messages.GetSize()){
		*result=-1;
		return;
	}
	CEmailHeader& item=m_pAccount->messages[iMessageIndex];
	if(pRowState->strText==""){
		item.lMarkedForDeletion=pRowState->dwNotifyData;
	}
	*result=0;
}

CEmailHeader* CDLG_AntiSpam::GetItemUnderCursor(int* iUnderCursorCol)
{
	CSmartLock l(m_pAccount->csMails);
	CPoint p;
	int iCol=-1;
	GetCursorPos(&p);
	m_MsgList.ScreenToClient(&p);
	int iRow=m_MsgList.HitTestEx(p, &iCol);
	if(iUnderCursorCol){
		*iUnderCursorCol=iCol;
	}
	if(iRow!=-1){
		HIROW hIRow=m_MsgList.GetIRowFromIndex(iRow);
		if(hIRow){
			long dwItemData=m_MsgList.GetIItemData(hIRow,1);
			if(dwItemData>=0 && dwItemData<m_pAccount->messages.GetSize()){
				return &m_pAccount->messages[dwItemData];
			}
		}
	}
	return NULL;
}

void CDLG_AntiSpam::OnRCLICK_TOGGLEMARK()
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item){
		return;
	}
	item->lMarkedForDeletion=!item->lMarkedForDeletion;
	RefreshSingleMessage(item);
}

void CDLG_AntiSpam::OnRCLICK_REREAD()
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item || !m_pAccount){
		return;
	}
	FORK(GetMessageAgain,new CGetMsgInfo(this,item,objSettings.lAdditionalHeaderLines));
}

void CDLG_AntiSpam::OnRCLICK_DELETEFROMSERVER()
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item || !m_pAccount){
		return;
	}
	item->lMarkedForDeletion=TRUE;
	m_pAccount->DeleteMessage(item);
	RefreshSingleMessage(item);
}

void CDLG_AntiSpam::OnRCLICK_DOWNLOADNEXTLINES(int iLines)
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item || !m_pAccount){
		return;
	}
	FORK(GetMessageAgain,new CGetMsgInfo(this,item,iLines));
}

CNoteSettings* CDLG_AntiSpam::PrepareNoteForMessage(CEmailHeader* item)
{
	// Бывает что окно уже TOPMOST, снимаем это
	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
	// Готовим заметку...
	CRect rt;
	GetWindowRect(&rt);
	CNoteSettings* NewNote=new CNoteSettings(objSettings.NoteSettings);
	NewNote->SetTitle(_l("Email from")+" '"+item->sFields[MAIL_FROMFIELD]+"'");
	NewNote->bTopMost=TRUE;
	NewNote->SetReadOnly(TRUE);
	NewNote->bWordWrap=FALSE;
	NewNote->bNeedFocusOnStart=FALSE;
	NewNote->iViewMode=1;
	NewNote->bSaveOnExit=FALSE;
	NewNote->iXPos=rt.left;
	NewNote->iYPos=rt.top;
	CSize size;
	size.cx=rt.Width();
	size.cy=rt.Height();
	AdoptSizeToScreen(size);
	NewNote->iXSize=size.cx;
	NewNote->iYSize=size.cy;
	NewNote->bDisableTransparency=TRUE;
	return NewNote;
}

void CDLG_AntiSpam::ShowItemBody(CEmailHeader* item)
{
	CNoteSettings* NewNote=PrepareNoteForMessage(item);
	NewNote->sText=_l("Body of the email from")+" '"+item->sFields[MAIL_FROMFIELD]+"':\n\n";
	NewNote->sText+="\n\n\n";
	if(item->sFields[MAIL_BDFLD_PR].GetLength()>0){
		NewNote->sText+=_l("Received message body lines")+":\n\n";
		NewNote->sText+=item->sFields[MAIL_BDFLD_PR];
	}else{
		NewNote->sText+=_l("Message body not available");
	}
	OpenNote(NewNote);
}

void CDLG_AntiSpam::OnRCLICK_SHOWBODY()
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item){
		return;
	}
	ShowItemBody(item);
}

void CDLG_AntiSpam::OnRCLICK_SHOWRAWBODY()
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item){
		return;
	}
	CNoteSettings* NewNote=PrepareNoteForMessage(item);
	NewNote->sText=_l("Raw body of the email from")+" '"+item->sFields[MAIL_FROMFIELD]+"':\n\n";
	NewNote->sText+="\n\n\n";
	if(item->sFields[MAIL_BODYFIELD].GetLength()>0){
		NewNote->sText+=_l("Received message body lines")+":\n\n";
		NewNote->sText+=item->sFields[MAIL_BODYFIELD];
	}else{
		NewNote->sText+=_l("Message body not available");
	}
	OpenNote(NewNote);
}

void CDLG_AntiSpam::OnRCLICK_SHOWALLHEADERS()
{
	LRESULT lRes=0;
	OnDblclkMsglist(NULL,&lRes);
}

void CDLG_AntiSpam::OnDblclkMsglist(NMHDR * pNotifyStruct, LRESULT * result)
{
	CEmailHeader* item=GetSelectedMessage();
	if(!item){
		*result=-1;
		return;
	}
	CNoteSettings* NewNote=PrepareNoteForMessage(item);
	NewNote->sText=_l("Header of the email from")+" '"+item->sFields[MAIL_FROMFIELD]+"':\n\n";
	NewNote->sText+=item->sHeader;
	if(item->sFields[MAIL_BDFLD_PR].GetLength()>0){
		NewNote->sText+="\n\n\n";
		NewNote->sText+=_l("Received message body lines")+":\n\n";
		NewNote->sText+=item->sFields[MAIL_BDFLD_PR];
	}
	OpenNote(NewNote);
}

void CDLG_AntiSpam::OnCheckRunemailc() 
{
	UpdateData(TRUE);
	m_pAccount->lRunEmailAfterUserCheck=m_AutoRunEm.GetCheck();
}
