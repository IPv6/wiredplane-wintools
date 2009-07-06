
сделать клянч смски! и минилендарь

// DLG_AddLink.cpp : implementation file
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
// CDLG_AddLink dialog


CDLG_AddLink::CDLG_AddLink(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_AddLink::IDD, pParent), Track(objSettings.bDoModalInProcess)
{
	//{{AFX_DATA_INIT(CDLG_AddLink)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pLink=NULL;
	iType=0;
}


void CDLG_AddLink::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_AddLink)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_AddLink, CDialog)
	//{{AFX_MSG_MAP(CDLG_AddLink)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_AddLink message handlers

void CDLG_AddLink::OnAdd() 
{
	if(pLink){
		GetDlgItem(IDC_EDIT_WEIGHT)->GetWindowText(*pLink);
	}
	CDialog::OnOK();
}

BOOL CDLG_AddLink::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetTaskbarButton(this->GetSafeHwnd());
	if(pLink){
		GetDlgItem(IDC_EDIT_WEIGHT)->SetWindowText(*pLink);
		GetDlgItem(IDC_EDIT_WEIGHT)->SetFocus();
	}
	if(iType==99){
		//Alert(_l("Activate by sending SMS"),_l("SMS activation"));
		SetWindowText(_l("SMS activation"));
		GetDlgItem(IDC_STATIC)->SetWindowText(_l("Activate WireChanger via SMS"));
		GetDlgItem(IDC_ADD)->SetWindowText(_l("Activate"));
	}else if(iType==0){
		SetWindowText(_l("Add URL"));
		GetDlgItem(IDC_STATIC)->SetWindowText(_l("Enter URL and press 'Add' button"));
		GetDlgItem(IDC_ADD)->SetWindowText(_l("Add"));
	}else{
		SetWindowText(_l("Create new preset"));
		GetDlgItem(IDC_STATIC)->SetWindowText(_l("Enter preset title and press 'Create' button"));
		GetDlgItem(IDC_ADD)->SetWindowText(_l("Create"));
	}
	return TRUE;
}

BOOL CDLG_AddLink::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN){
		OnAdd();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
