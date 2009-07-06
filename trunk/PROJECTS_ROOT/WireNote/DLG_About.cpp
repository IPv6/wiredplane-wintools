// DLG_About.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLG_About dialog


CDLG_About::CDLG_About(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_About::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_About)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDLG_About::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_About)
	DDX_Control(pDX, IDC_LOGO, m_Logo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_About, CDialog)
	//{{AFX_MSG_MAP(CDLG_About)
	ON_BN_CLICKED(ID_ENTCODE, OnEntcode)
	ON_BN_CLICKED(ID_WEBSITE, OnWebsite)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_About message handlers

BOOL CDLG_About::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,0,0);//HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_About"
	Sizer.SetOptions(STICKABLE);
	m_Logo.SetBitmap(IDB_BM_LOGO);
	m_Logo.SetStyles(CBS_FLAT|CBS_NONSQR);
	//m_Logo.SetBrush(RGB(100,100,100));
	SetWindowText((CString)PROGNAME+" - "+_l("About"));
	GetDlgItem(ID_ENTCODE)->SetWindowText(_l("Enter key"));
	GetDlgItem(ID_WEBSITE)->SetWindowText(_l("Web site"));
	GetDlgItem(IDOK)->SetWindowText(_l("Ok"));
	if(objSettings.bNoSend){
		GetDlgItem(IDC_PRVERSION)->SetWindowText(Format("%s %s (%s)",PROGNAME,PROG_VERSION,_l("Receiver")));
	}else{
		GetDlgItem(IDC_PRVERSION)->SetWindowText(Format("%s %s (%s)",PROGNAME,PROG_VERSION,__DATE__));
	}
	GetDlgItem(IDC_COPYRIGHT)->SetWindowText("(C)2002-2006. WiredPlane.com");
	if(objSettings.iLikStatus<=1){
		CString sText=_l("Evaluation copy");
		if(objSettings.iLikStatus==0){
			sText+=Format(" (%s: %i)",_l("Days left"),GetWindowDays());
		}
		GetDlgItem(IDC_REGSTATUS)->SetWindowText(sText);
	}else{
		//GetDlgItem(IDC_REGSTATUS)->SetWindowText(_l("Registered to")+" "+objSettings.sLikUser);
		GetDlgItem(IDC_REGSTATUS)->SetWindowText("GPL edition");
		GetDlgItem(ID_ENTCODE)->ShowWindow(SW_HIDE);
	}
	GetDlgItem(IDC_EMDSC)->SetWindowText(_l("For comments, bugs reports, and any suggestions please write to")+" "+SUPPORT_EMAIL);
	GetDlgItem(IDC_DOWNLOADDSC)->SetWindowText(_l("You can download latest version from")+" "+DOWNLOAD_URL);
	GetDlgItem(IDC_LEGALNOTICE)->SetWindowText(_l("Notice: this computer program is protected by copyright laws","Notice: this computer program is protected by copyright laws and international treaties. Unauthorized reproduction or distribution of this program, or any portion of it, may result in severe civil and criminal penalties, and will be prosecuted to the maximum extend possible under the law"));
	CString sUsageStats;
	if(objSettings.bNoSend){
		sUsageStats=Format("%s: %lu, %s: R%lu, %s: %lu/%lu",_l("Notes"),objSettings.lStatNotesCreated,_l("Messages"),objSettings.lStatReceivedMessages,_l("Mails"),objSettings.lStatReceivedMails,objSettings.lStatReceivedSpamMails);
	}else{
		sUsageStats=Format("%s: %lu, %s: R%lu/S%lu, %s: %lu/%lu",_l("Notes"),objSettings.lStatNotesCreated,_l("Messages"),objSettings.lStatReceivedMessages,objSettings.lStatSendedMessages,_l("Mails"),objSettings.lStatReceivedMails,objSettings.lStatReceivedSpamMails);
	}
	GetDlgItem(IDC_STATS)->SetWindowText(sUsageStats);
	SetTaskbarButton(this->GetSafeHwnd());
	return TRUE;
}

LRESULT CDLG_About::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDLG_About::OnEntcode() 
{
	objSettings.OpenOptionsDialog(_l(REGISTRATION_TOPIC),this);
}

HANDLEINTHREAD(CDLG_About,OnWebsite) 
{
	ShellExecute(0,"open","http://www.wiredplane.com?who=WireNote"PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	return 0;
}

void CDLG_About::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID==SC_MINIMIZE || nID==SC_CLOSE){
		OnOK();
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CDLG_About::OnOK() 
{
	CDialog::OnOK();
}
