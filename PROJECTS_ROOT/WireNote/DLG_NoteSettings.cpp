// DLG_NoteSettings.cpp : implementation file
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
// CDLG_NoteSettings dialog


CDLG_NoteSettings::CDLG_NoteSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_NoteSettings::IDD, pParent), Track(objSettings.bDoModalInProcess)
{
	//{{AFX_DATA_INIT(CDLG_NoteSettings)
	m_PinupTitle = _T("");
	//}}AFX_DATA_INIT
	pItem=NULL;
}


void CDLG_NoteSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_NoteSettings)
	DDX_Text(pDX, IDC_PINUPTITLE, m_PinupTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_NoteSettings, CDialog)
	//{{AFX_MSG_MAP(CDLG_NoteSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_NoteSettings message handlers

void CDLG_NoteSettings::OnOK() 
{
	UpdateData(TRUE);
	if(pItem){
		pItem->SetApplicationsList(m_PinupTitle);
	}
	CDialog::OnOK();
}

BOOL CDLG_NoteSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,NULL,NULL);//HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_NoteOptions"
	Sizer.SetOptions(STICKABLE|SETCLIPSIBL);
	if(pItem){
		m_PinupTitle=pItem->GetApplicationsList();
		m_PinupTitle.Replace("\n","\r\n");
	}
	UpdateData(FALSE);
	SetWindowText(_l("Advanced options"));
	GetDlgItem(IDOK)->SetWindowText(_l("OK"));
	GetDlgItem(IDC_PINUP_STATIC)->SetWindowText(_l("Attach to specified applications"));
	GetDlgItem(IDC_STATIC_DSC)->SetWindowText(_l("Put one title per line","Put one title per line\nUse '*' as a wildcard for partial matches between mask and active window title"));
	return TRUE;
}

BOOL CDLG_NoteSettings::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==VK_RETURN){
			if(isPressed(VK_CONTROL)){
				OnOK();
				EndDialog(0);
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CDLG_NoteSettings::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
