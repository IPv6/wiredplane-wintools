// NewPerson.cpp : implementation file
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
// CNewPerson dialog


CNewPerson::CNewPerson(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPerson::IDD, pParent), Track(objSettings.bDoModalInProcess)
{
	//{{AFX_DATA_INIT(CNewPerson)
	m_CompName = _T("");
	m_Nick = _T("");
	m_Dsc = _T("");
	m_bGroup = FALSE;
	m_iTab = 0;
	//}}AFX_DATA_INIT
}


void CNewPerson::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewPerson)
	DDX_Text(pDX, IDC_EDIT_IP, m_CompName);
	DDX_Text(pDX, IDC_EDIT_NICK, m_Nick);
	DDX_Text(pDX, IDC_EDIT_DSC, m_Dsc);
	DDX_Check(pDX, IDC_CHECK_GROUP, m_bGroup);
	DDX_CBIndex(pDX, IDC_COMBO_TAB, m_iTab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewPerson, CDialog)
	//{{AFX_MSG_MAP(CNewPerson)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_COMPADDR, OnCompaddr)
	ON_BN_CLICKED(IDFIND, OnCompFind)
	ON_EN_CHANGE(IDC_EDIT_IP, OnChangeEditIp)
	ON_CBN_SELCHANGE(IDC_COMBO_PROT, OnSelchangeComboProt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPerson message handlers

BOOL CNewPerson::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_NewPerson");
	CRect minRect(0,0,DIALOG_MINXSIZE,DIALOG_MINYSIZE);
	Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
	Sizer.SetMinRect(minRect);
	Sizer.AddDialogElement(IDOK,widthSize|heightSize);
	Sizer.AddDialogElement(IDCANCEL,widthSize|heightSize);
	Sizer.AddDialogElement(IDFIND,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_STATIC1,heightSize,IDC_COMBO_TAB);
	Sizer.AddDialogElement(IDC_STATIC2,heightSize,IDC_COMBO_TAB);
	Sizer.AddDialogElement(IDC_STATIC3,heightSize,IDC_COMBO_TAB);
	Sizer.AddDialogElement(IDC_EDIT_NICK,heightSize,IDC_COMBO_TAB);
	Sizer.AddDialogElement(IDC_EDIT_IP,heightSize,IDC_COMBO_TAB);
	Sizer.AddDialogElement(IDC_COMBO_PROT,heightSize|widthSize);
	Sizer.AddDialogElement(IDC_COMBO_TAB,heightSize|widthSize);
	Sizer.AddDialogElement(IDC_COMPADDR,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_EDIT_DSC);
	Sizer.AddDialogElement(IDC_CHECK_GROUP,heightSize,IDOK);

	Sizer.AddLeftTopAlign(IDC_STATIC1,0,4);
	Sizer.AddLeftAlign(IDC_EDIT_NICK,0,4);
	Sizer.AddRightAlign(IDC_COMBO_TAB,0,-4);
	Sizer.AddRightAlign(IDC_EDIT_NICK,IDC_COMBO_TAB,leftPos,-2);
	Sizer.AddTopAlign(IDC_EDIT_NICK,IDC_STATIC1,bottomPos,1);
	Sizer.AddTopAlign(IDC_COMBO_TAB,IDC_STATIC1,bottomPos,1);
	Sizer.AddLeftAlign(IDC_STATIC2,0,4);
	Sizer.AddLeftAlign(IDC_EDIT_IP,0,4);
	Sizer.AddRightAlign(IDC_COMPADDR,0,-4);
	Sizer.AddRightAlign(IDC_COMBO_PROT,IDC_COMPADDR,leftPos,-1);
	Sizer.AddRightAlign(IDC_EDIT_IP,IDC_COMBO_PROT,leftPos,-1);
	Sizer.AddTopAlign(IDC_STATIC2,IDC_EDIT_NICK,bottomPos,1);
	Sizer.AddTopAlign(IDC_EDIT_IP,IDC_STATIC2,bottomPos,1);
	Sizer.AddTopAlign(IDC_COMPADDR,IDC_STATIC2,bottomPos,1);
	Sizer.AddTopAlign(IDC_COMBO_PROT,IDC_STATIC2,bottomPos,1);
	Sizer.AddLeftAlign(IDC_STATIC3,0,4);
	Sizer.AddLeftAlign(IDC_EDIT_DSC,0,4);
	Sizer.AddRightAlign(IDC_EDIT_DSC,0,-4);
	Sizer.AddTopAlign(IDC_STATIC3,IDC_EDIT_IP,bottomPos,1);
	Sizer.AddTopAlign(IDC_EDIT_DSC,IDC_STATIC3,bottomPos,1);
	Sizer.AddRightBottomAlign(IDCANCEL,0,-4);
	Sizer.AddBottomAlign(IDOK,0,-4);
	Sizer.AddBottomAlign(IDFIND,0,-4);
	Sizer.AddRightAlign(IDOK,IDCANCEL,leftPos,-2);
	Sizer.AddRightAlign(IDFIND,IDOK,leftPos,-2);
	Sizer.AddBottomAlign(IDC_EDIT_DSC,IDOK,topPos,-5);
	Sizer.AddLeftBottomAlign(IDC_CHECK_GROUP,0,4);
	Sizer.AddRightAlign(IDC_CHECK_GROUP,IDFIND,leftPos,-5);
	GetDlgItem(IDC_CHECK_GROUP)->SetWindowText(_l("Group of persons"));
	SetWindowText(_l("Person"));
	GetDlgItem(IDC_STATIC1)->SetWindowText(_l("Person nick"));
	GetDlgItem(IDC_STATIC2)->SetWindowText(_l("Computer name")+" "+_l("or names delimited with ';'"));
	GetDlgItem(IDC_STATIC3)->SetWindowText(_l("Additional info"));
	GetDlgItem(IDFIND)->SetWindowText(_l("Find"));
	GetDlgItem(IDCANCEL)->SetWindowText(_l("Cancel"));
	GetDlgItem(IDOK)->SetWindowText(_l("OK"));
	CComboBox* pProts=((CComboBox*)GetDlgItem(IDC_COMBO_PROT));
	if(pProts){
		pProts->ResetContent();
		pProts->AddString(_l("Default"));
		pProts->AddString(_l("Mailslot"));
		pProts->AddString(_l("WinPopup"));
		pProts->AddString(_l("WireNote"));
		pProts->AddString(_l("Billboard"));
		pProts->AddString(_l("Email"));
		pProts->AddString(_l("Sms"));
		pProts->AddString(_l("Rss"));
	}
	CComboBox* pTabs=((CComboBox*)GetDlgItem(IDC_COMBO_TAB));
	if(pTabs){
		pTabs->ResetContent();
		for(int i=0;i<objSettings.AddrBook.aAddrBookTabs.GetSize();i++){
			pTabs->AddString(objSettings.AddrBook.aAddrBookTabs[i]);
		}
		pTabs->SetCurSel(m_iTab);
	}
	//
	SetIcon(theApp.MainImageList.ExtractIcon(theApp.dwOnlineStatusStartBitmap),TRUE);
	SetIcon(theApp.MainImageList.ExtractIcon(theApp.dwOnlineStatusStartBitmap),FALSE);
	SetTaskbarButton(this->GetSafeHwnd());
	UpdateData(FALSE);
	OnChangeEditIp();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CNewPerson::OnOK() 
{
	UpdateData();
	CDialog::OnOK();
}

void CNewPerson::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	Sizer.ApplyLayout();
	GetDlgItem(IDC_EDIT_NICK)->SetFocus();
}

LRESULT CNewPerson::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CNewPerson::OnCompFind()
{
	if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
		pMainDialogWindow->dlgAddBook->PostMessage(WM_COMMAND,ID_TRAY_FINDONLINEUSERS,0);
		OnCancel();
	}
}

void CNewPerson::OnCompaddr() 
{
	//IDC_COMPADDR
	char szComp[MAX_PATH]="";
	if(GetFolderComputer(_l("Choose computer"), szComp, this->GetSafeHwnd())){
		UpdateData(TRUE);
		m_CompName=szComp;
		UpdateData(FALSE);
	}
}

BOOL CNewPerson::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN){
		if(isPressed(VK_CONTROL)){
			if(pMsg->wParam==VK_RETURN){
				OnOK();
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CNewPerson::OnChangeEditIp() 
{
	CComboBox* pProts=((CComboBox*)GetDlgItem(IDC_COMBO_PROT));
	if(pProts){
		UpdateData(TRUE);
		CString sRecipientNoProt;
		CString sProt=GetCompNameProtocol(m_CompName, sRecipientNoProt);
		int iIndex=FindProtocolIndex(sProt);
		if(iIndex<0){
			iIndex=0;
		}else{
			iIndex++;
		}
		if(pProts->GetCurSel()!=iIndex){
			pProts->SetCurSel(iIndex);
		}
	}
}

void CNewPerson::OnSelchangeComboProt() 
{
	CComboBox* pProts=((CComboBox*)GetDlgItem(IDC_COMBO_PROT));
	if(pProts){
		int iNewProt=pProts->GetCurSel();
		UpdateData(TRUE);
		CString sRecipientNoProt;
		CString sProt=GetCompNameProtocol(m_CompName, sRecipientNoProt);
		if(FindProtocolIndex(sProt)!=-1){
			m_CompName=sRecipientNoProt;
		}
		if(iNewProt>0){
			m_CompName+="/";
			m_CompName+=objSettings.aProtList[iNewProt-1];
		}
		UpdateData(FALSE);
	}
}
