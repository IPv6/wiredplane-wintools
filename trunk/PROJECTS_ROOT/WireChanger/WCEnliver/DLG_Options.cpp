// DLG_Options.cpp : implementation file
//

#include "stdafx.h"
#include "wcenliver.h"
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
	m_Quality = -1;
	m_Resolution = -1;
	m_Speed = _T("");
	m_Effect = -1;
	//}}AFX_DATA_INIT
}


void CDLG_Options::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Options)
	DDX_CBIndex(pDX, IDC_COMBO_Q, m_Quality);
	DDX_CBIndex(pDX, IDC_COMBO_R, m_Resolution);
	DDX_Text(pDX, IDC_SPEED, m_Speed);
	DDX_CBIndex(pDX, IDC_COMBO_EF, m_Effect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_Options, CDialog)
	//{{AFX_MSG_MAP(CDLG_Options)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_Options message handlers
extern CIEnliver objSettings;
BOOL CDLG_Options::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Speed.Format("%lu",objSettings.dwTimeout);
	m_Quality=objSettings.dwQuality;
	m_Effect=objSettings.dwEffect;
	m_Resolution=0;
	if(objSettings.dwImgW>=1280){
		m_Resolution=3;
	}else if(objSettings.dwImgW>=1024){
		m_Resolution=2;
	}else if(objSettings.dwImgW>=640){
		m_Resolution=1;
	}
	UpdateData(FALSE);
	return TRUE;
}

void CDLG_Options::OnOK() 
{
	UpdateData(TRUE);
	objSettings.dwQuality=m_Quality;
	switch(m_Resolution)
	{
		case 1:
			objSettings.dwImgW=640;
			objSettings.dwImgH=480;
			break;
		case 2:
			objSettings.dwImgW=1024;
			objSettings.dwImgH=768;
			break;
		case 3:
			objSettings.dwImgW=1280;
			objSettings.dwImgH=1024;
			break;
		default:
			objSettings.dwImgW=320;
			objSettings.dwImgH=200;
			break;
	}
	objSettings.dwEffect=m_Effect;
	objSettings.dwTimeout=atol(m_Speed);
	if(objSettings.dwTimeout<10){
		objSettings.dwTimeout=10;
	}
	CDialog::OnOK();
}
