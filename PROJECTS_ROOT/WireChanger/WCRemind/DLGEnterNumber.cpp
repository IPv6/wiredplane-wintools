// DLGEnterNumber.cpp : implementation file
//

#include "stdafx.h"
#include "wcremind.h"
#include "DLGEnterNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLGEnterNumber dialog


CDLGEnterNumber::CDLGEnterNumber(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGEnterNumber::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLGEnterNumber)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pdwNumber=0;
}


void CDLGEnterNumber::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLGEnterNumber)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLGEnterNumber, CDialog)
	//{{AFX_MSG_MAP(CDLGEnterNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLGEnterNumber message handlers

BOOL CDLGEnterNumber::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDOK)->SetWindowText(_lx("RMX.UI.SAVECLOSE","Save and close"));
	GetDlgItem(IDCANCEL)->SetWindowText(_lx("RMX.UI.CANCEL","Cancel"));
	SetWindowText(_lx("RMX.UI.XMANUAL","Specify period"));
	GetDlgItem(IDC_STATIC1)->SetWindowText(_lx("RMX.UI.XMANUAL_DSC","Enter number to specify period"));
	CString sNumber="0";
	if(pdwNumber){
		sNumber.Format("%i",*pdwNumber);
	}
	GetDlgItem(IDC_EDIT1)->SetWindowText(sNumber);
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(0,1000,0);
	GetDlgItem(IDC_EDIT1)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDLGEnterNumber::OnOK() 
{
	char sNumber[128]={0};
	GetDlgItem(IDC_EDIT1)->GetWindowText(sNumber,sizeof(sNumber));
	if(pdwNumber){
		*pdwNumber=atol(sNumber);
	}
	CDialog::OnOK();
}

BOOL CDLGEnterNumber::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==VK_RETURN){
			OnOK();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}