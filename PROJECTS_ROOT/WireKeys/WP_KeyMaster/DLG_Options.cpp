// DLG_Options.cpp : implementation file
//

#include "stdafx.h"
#include "WP_KeyMaster.h"
#include "DLG_Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLG_Options dialog


CDLG_Options::CDLG_Options(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_Options::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_Options)
	m_AKeys = _T("");
	//m_bLRDisting = FALSE;
	m_bTempBlockHotkeys = FALSE;
	m_bBlockApps = FALSE;
	m_bBlockWin = FALSE;
	//m_bUseMouseBt = FALSE;
	//}}AFX_DATA_INIT
}


void CDLG_Options::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Options)
	DDX_Text(pDX, IDC_ACTIVE_KEYS, m_AKeys);
	DDX_Check(pDX, IDC_BLOCK, m_bTempBlockHotkeys);
	DDX_Check(pDX, IDC_BLOCKAPPS, m_bBlockApps);
	DDX_Check(pDX, IDC_BLOCKWIN, m_bBlockWin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_Options, CDialog)
	//{{AFX_MSG_MAP(CDLG_Options)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_Options message handlers

void CDLG_Options::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
}

BOOL CDLG_Options::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CDLG_Options::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

extern GPLUGINIni g_hookData;
extern BOOL g_bTeporalyBlockHotkeys;
extern BOOL g_bTeporalyBlockAll;
extern BOOL g_bTeporalyBlockAllButHK;
CString _l(const char* szText);
BOOL CDLG_Options::OnInitDialog() 
{
	// Тень
	SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);
	// Поверх всех
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	//-----------
	SetWindowText(_l("Hotkeys options"));
	Sizer.InitSizer(this,10,HKEY_CURRENT_USER,"SOFTWARE\\WiredPlane\\WireKeys\\WKKeyMaster\\OptionsPos2");
	Sizer.SetOptions(STICKABLE);
	CDialog::OnInitDialog();
	//GetDlgItem(IDC_RLDIST)->SetWindowText(_l("Distinguish between Left and Right keys"));
	//GetDlgItem(IDC_USEMOUSE)->SetWindowText(_l("Treat mouse buttons as regular keys. You must restart WireKeys in order to get effect from this option"));
	GetDlgItem(IDC_STATIC3)->SetWindowText(_l("Additional features"));
	GetDlgItem(IDC_BLOCKAPPS)->SetWindowText(_l("Block single 'Apps' key"));
	GetDlgItem(IDC_BLOCKWIN)->SetWindowText(_l("Block single 'Win' key"));
	GetDlgItem(IDC_BLOCK)->SetWindowText(_l("Temporarily block all hotkeys and combinations"));
	GetDlgItem(IDC_STATIC2)->SetWindowText(_l("Written by IPv6 @2008"));
	GetDlgItem(IDCANCEL)->SetWindowText(_l("Cancel"));
	GetDlgItem(IDOK)->SetWindowText(_l("OK"));

	m_AKeys = GetListOfCurrentlyRegisteredHotkeys();
	//m_bLRDisting = g_hookData.bDistingLR;
	m_bTempBlockHotkeys = g_bTeporalyBlockHotkeys;
	m_bBlockApps = g_hookData.bBlockApps;
	m_bBlockWin = g_hookData.bBlockWin;
	//m_bUseMouseBt = g_hookData.bUseMouseKeys;
	UpdateData(FALSE);
	return TRUE;
}

void CDLG_Options::OnOK() 
{
	UpdateData(TRUE);
	//g_hookData.bDistingLR=m_bLRDisting;WKIHOTKEYS_DistingLRKeys()=g_hookData.bDistingLR;
	g_bTeporalyBlockHotkeys = m_bTempBlockHotkeys;
	g_hookData.bBlockApps = m_bBlockApps;
	g_hookData.bBlockWin = m_bBlockWin;
	//g_hookData.bUseMouseKeys = m_bUseMouseBt;
	CDialog::OnOK();
}
