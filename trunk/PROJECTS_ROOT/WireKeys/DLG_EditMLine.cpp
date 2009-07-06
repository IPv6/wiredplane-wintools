// DLG_EditMLine.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "DLG_EditMLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLG_EditMLine dialog


CDLG_EditMLine::CDLG_EditMLine(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_EditMLine::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_EditMLine)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	iMode=0;
}


void CDLG_EditMLine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_EditMLine)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_EditMLine, CDialog)
	//{{AFX_MSG_MAP(CDLG_EditMLine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_EditMLine message handlers

BOOL CDLG_EditMLine::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetTaskbarButton(this->GetSafeHwnd());
	if(iMode){
		SetWindowText(_l("Enter item title"));
		GetDlgItem(IDC_EDIT2)->SetWindowText(sText);
		GetDlgItem(IDC_EDIT2)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT2))->SetSel(0,10000);
		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_HIDE);
		CRect rt,rt1,rt2,rtO;
		GetWindowRect(&rt);
		GetDlgItem(IDC_EDIT1)->GetWindowRect(&rt1);
		int iRawOffset=rt1.top-rt.top;
		ScreenToClient(rt1);
		int iOffs=rt1.top;
		GetDlgItem(IDC_EDIT2)->GetWindowRect(&rt2);
		rt2.OffsetRect(-rt.left,-rt.top);
		GetDlgItem(IDOK)->GetWindowRect(&rtO);
		rtO.OffsetRect(-rt.left,-rt.top);
		rt2.bottom=iOffs+rt2.Height();
		rt2.top=iOffs;
		rtO.bottom=iOffs+rtO.Height();
		rtO.top=iOffs;
		GetDlgItem(IDC_EDIT2)->MoveWindow(&rt2);
		GetDlgItem(IDOK)->MoveWindow(&rtO);
		rt.bottom-=(rt1.Height()-rt2.Height())+iRawOffset-iOffs;
		MoveWindow(&rt);
	}else{
		SetWindowText(_l("Edit items"));
		GetDlgItem(IDC_EDIT1)->SetWindowText(sText);
		GetDlgItem(IDC_EDIT1)->SetFocus();
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_HIDE);
	}
	if(sTitle!=""){
		SetWindowText(sTitle);
	}
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDLG_EditMLine::OnOK() 
{
	// TODO: Add extra validation here
	CString sOkText;
	if(iMode){
		GetDlgItem(IDC_EDIT2)->GetWindowText(sOkText);
	}else{
		GetDlgItem(IDC_EDIT1)->GetWindowText(sOkText);
	}
	if(sOkText!=""){
		sText=sOkText;
	}
	CDialog::OnOK();
}

LRESULT CDLG_EditMLine::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CDLG_EditMLine::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN){
		if(GetKeyState(VK_CONTROL)<0){
			if(pMsg->wParam==VK_RETURN){
				OnOK();
				return TRUE;
			}
			if(pMsg->wParam=='A'){
				((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(-1,-1);
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
