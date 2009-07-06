// DLG_Chooser.cpp : implementation file
//

#include "stdafx.h"
#include "DLG_Chooser.h"
#include "..\SmartWires\SystemUtils\SupportClasses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLG_Chooser dialog


CDLG_Chooser::CDLG_Chooser(const char* szTitle,const char* szHelpString, CStringArray* pOptionList, int iStartElen, int iIconID, CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_Chooser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_Chooser)
	m_HelpString = szHelpString;
	//}}AFX_DATA_INIT
	m_pOptionList=pOptionList;
	m_sTitle=szTitle;
	m_iStartElen=iStartElen;
	m_iIconID=iIconID;
}


void CDLG_Chooser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Chooser)
	DDX_Control(pDX, IDC_COMBO, m_ChooseList);
	DDX_Text(pDX, IDC_TEXT, m_HelpString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_Chooser, CDialog)
	//{{AFX_MSG_MAP(CDLG_Chooser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_Chooser message handlers

void CDLG_Chooser::OnOK() 
{
	UpdateData();
	EndDialog(m_ChooseList.GetCurSel());
}

BOOL CDLG_Chooser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rtMe;
	GetWindowRect(&rtMe);
	SetIcon(::AfxGetApp()->LoadIcon(m_iIconID),TRUE);
	SetIcon(::AfxGetApp()->LoadIcon(m_iIconID),FALSE);
	int iSize=m_pOptionList->GetSize();
	if(m_pOptionList){
		for(int i=0;i<iSize;i++){
			m_ChooseList.AddString(m_pOptionList->GetAt(i));
		}
	}
	SetWindowText(m_sTitle);
	if(m_iStartElen>=iSize){
		m_iStartElen=0;
	}
	m_ChooseList.SetCurSel(m_iStartElen);
	SetTaskbarButton(this->GetSafeHwnd());
	SetWindowPos(&wndTopMost,0,0,rtMe.Width(),rtMe.Height(),SWP_NOMOVE|SWP_SHOWWINDOW);
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)GetSafeHwnd());
	return TRUE;
}
