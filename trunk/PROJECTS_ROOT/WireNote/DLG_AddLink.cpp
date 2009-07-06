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
	if(pLink){
		GetDlgItem(IDC_EDIT_WEIGHT)->SetWindowText(*pLink);
		GetDlgItem(IDC_EDIT_WEIGHT)->SetFocus();
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDLG_AddLink::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->wParam==VK_RETURN){
		OnAdd();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
