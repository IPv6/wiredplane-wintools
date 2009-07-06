// DlgSimpleEdit.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "DLG_SimpleEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSimpleEdit dialog


CDlgSimpleEdit::CDlgSimpleEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSimpleEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSimpleEdit)
	m_Edit = _T("");
	m_EditTitle = _T("");
	//}}AFX_DATA_INIT
	iStyle=0;
}


void CDlgSimpleEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSimpleEdit)
	DDX_Text(pDX, IDC_EDIT, m_Edit);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_EditTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSimpleEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgSimpleEdit)
	ON_BN_CLICKED(ID_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSimpleEdit message handlers

BOOL CDlgSimpleEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_SYSCOMMAND && pMsg->wParam==SC_CLOSE){
		EndDialog(IDCANCEL);
		return TRUE;
	}
	if(pMsg->message==WM_DROPFILES){
		UpdateData(TRUE);
		HDROP hDropInfo=(HDROP)pMsg->wParam;
		char szFilePath[256];
		UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
		for (UINT u = 0; u < cFiles; u++){
			DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
			m_Edit+="\r\n";
			m_Edit+=szFilePath;
		}
		DragFinish(hDropInfo);
		UpdateData(FALSE);
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CDlgSimpleEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CDlgSimpleEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_EditLink");
	if(iStyle==1){
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL|FIXEDYSIZE);
	}else{
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
	}
	Sizer.SetMinRect(CRect(0,0,150,90));
	Sizer.AddDialogElement(IDC_EDIT,0);
	Sizer.AddDialogElement(IDC_EDIT_TITLE,heightSize);
	Sizer.AddDialogElement(IDOK,heightSize|widthSize);
	Sizer.AddDialogElement(ID_BROWSE,heightSize|widthSize);
	//----------------------------------------
	if(iStyle==1){
		Sizer.AddLeftTopAlign(IDC_EDIT_TITLE,0,4);
		Sizer.AddRightTopAlign(IDC_EDIT_TITLE,0,4);
		Sizer.AddTopAlign(IDC_EDIT,0,bottomPos,10);
		Sizer.AddTopAlign(ID_BROWSE,0,bottomPos,10);
		Sizer.AddTopCenterAlign(IDOK,0,0);
		Sizer.AddBottomAlign(IDOK,0,-4);
	}else{
		Sizer.AddLeftTopAlign(IDC_EDIT_TITLE,0,4);
		Sizer.AddLeftAlign(IDC_EDIT,0,4);
		Sizer.AddTopAlign(IDC_EDIT,IDC_EDIT_TITLE,bottomPos,4);
		Sizer.AddLeftBottomAlign(IDC_EDIT,0,4);
		Sizer.AddRightBottomAlign(IDOK,0,-4);
		Sizer.AddRightAlign(IDC_EDIT,IDOK,leftPos,-4);
		Sizer.AddRightAlign(IDC_EDIT_TITLE,IDOK,leftPos,-4);
		Sizer.AddRightAlign(ID_BROWSE,IDOK,0);
		Sizer.AddBottomAlign(ID_BROWSE,IDOK,topPos,-4);
	}
	GetDlgItem(IDOK)->SetWindowText(_l("Save"));
	GetDlgItem(IDC_EDIT_TITLE)->SetFocus();
	if(iStyle==2 || iStyle==3 || iStyle==4){
		GetDlgItem(ID_BROWSE)->ShowWindow(SW_HIDE);
	}
	if(iStyle==3 || iStyle==4){
		GetDlgItem(IDOK)->SetWindowText(_l("Ok"));
		if(iStyle==4){
			GetDlgItem(IDC_EDIT_TITLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT)->SetFocus();
		}else{
			GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_TITLE)->SetFocus();
		}
	}
	SetWindowText(_l("Edit"));
	GetDlgItem(ID_BROWSE)->SetWindowText(_l("Browse"));
	m_Edit.Replace("\r","");
	m_Edit.Replace("\n","\r\n");
	UpdateData(FALSE);
	SetTaskbarButton(this->GetSafeHwnd());
	if(GetDlgItem(IDC_EDIT)->IsWindowEnabled()){
		GetDlgItem(IDC_EDIT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT))->SetSel(0,m_Edit.GetLength(),1);
	}
	return FALSE;
}

BOOL OpenFileDialog(const char* szExtension, HIROW hData, CDLG_Options* pDialog, BOOL bSkipFileCheck, BOOL bSaveDialog);
void CDlgSimpleEdit::OnBrowse()
{
	// Конвертим, так как нужно лишь CWnd*
	UpdateData(TRUE);
	objSettings.sLastGetAttachPath=m_Edit;
	OpenFileDialog("All files (*.*)|*.*||",0,(CDLG_Options *)this,FALSE,0);
	m_Edit=objSettings.sLastGetAttachPath;
	UpdateData(FALSE);
}