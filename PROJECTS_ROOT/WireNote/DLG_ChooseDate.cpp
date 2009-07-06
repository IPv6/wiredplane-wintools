// DLG_ChooseDate.cpp : implementation file
//

#include "stdafx.h"
#include "dlg_choosedate.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DLG_ChooseDate dialog


DLG_ChooseDate::DLG_ChooseDate(CWnd* pParent /*=NULL*/)
	: CDialog(DLG_ChooseDate::IDD, pParent), Track(objSettings.bDoModalInProcess)
{
	//{{AFX_DATA_INIT(DLG_ChooseDate)
	m_Date = time(NULL)-(7*24*60*60);
	//}}AFX_DATA_INIT
}


void DLG_ChooseDate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DLG_ChooseDate)
	DDX_DateTimeCtrl(pDX, IDC_MONTHCALENDAR, m_Date);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DLG_ChooseDate, CDialog)
	//{{AFX_MSG_MAP(DLG_ChooseDate)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLG_ChooseDate message handlers

void DLG_ChooseDate::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

void DLG_ChooseDate::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL DLG_ChooseDate::OnInitDialog()
{
	SetWindowText(_l("Report: Start date"));
	GetDlgItem(IDOK)->SetWindowText(_l("OK"));
	GetDlgItem(IDCANCEL)->SetWindowText(_l("Cancel"));
	return TRUE;
}
void DLG_ChooseDate::OnBnClickedCancel()
{
	OnCancel();
}
