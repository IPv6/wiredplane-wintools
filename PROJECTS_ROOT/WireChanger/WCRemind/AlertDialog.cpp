// AlertDialog.cpp : implementation file
//

#include "stdafx.h"
#include "wcremind.h"
#include "AlertDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlertDialog dialog


CAlertDialog::CAlertDialog(CWnd* pParent /*=NULL*/)
: CDialog(CAlertDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlertDialog)
	//}}AFX_DATA_INIT
	static BOOL bInit=0;
	if(bInit==0){
		bInit=1;
		AfxEnableControlContainer();
		AfxInitRichEdit();
	}
	lDoAutoclose=0;
}


void CAlertDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlertDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertDialog, CDialog)
	//{{AFX_MSG_MAP(CAlertDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_PPREMINDER, OnOk2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlertDialog message handlers
struct CCheckAlertTimeData
{
	long lTime;
	HWND h;
	BOOL bForceWindowShow;
};

DWORD WINAPI CheckAlertTime(LPVOID p)
{
	CCheckAlertTimeData* data=(CCheckAlertTimeData*)p;
	HWND hWin=data->h;
	int iTime=data->lTime;
	if(data->bForceWindowShow)
	{
		SwitchToWindow(hWin,FALSE);
	}
	if(iTime>0)
	{
		DWORD dwTime=GetTickCount();
		while(true)
		{
			if(!IsWindow(hWin))
			{
				break;
			}
			if(int(GetTickCount()-dwTime)>iTime)
			{
				// Закрываем окно
				PostMessage(hWin,WM_COMMAND,IDOK,0);
				break;
			}
			CString sTime=Format("%s [%i]",_lx("RMX.UI.ATTENTION","Attention")+"!",(iTime-(GetTickCount()-dwTime))/1000+1);
			SetWindowText(hWin,sTime);
		}
	}
	delete data;
	return 0;
};

BOOL CAlertDialog::OnInitDialog() 
{
	iRes=0;
	CDialog::OnInitDialog();
	SetWindowText(_lx("RMX.UI.ATTENTION","Attention")+"!");
	GetDlgItem(IDOK)->SetWindowText(_lx("RMX.UI.DISMISS","Dismiss"));
	GetDlgItem(ID_PPREMINDER)->SetWindowText(_lx("RMX.UI.POSTPONE","Postpone ..."));
	sText.Replace("\r\n","\n");
	sText.Replace("\n","\r\n");
	GetDlgItem(IDC_EDIT)->SetWindowText(sText);
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// Открываем окно
	CCheckAlertTimeData* alertData=new CCheckAlertTimeData();
	alertData->h=this->GetSafeHwnd();
	alertData->lTime=0;
	alertData->bForceWindowShow=1;
	if(lDoAutoclose)
	{
		alertData->lTime=15000;
	}
	FORK(CheckAlertTime,alertData);
	return FALSE;
}

void CAlertDialog::OnOK() 
{
	iRes=0;
	CDialog::OnOK();
}

void CAlertDialog::OnClose() 
{
	iRes=0;	
	CDialog::OnClose();
}

void CAlertDialog::OnOk2() 
{
	CMenu menu;
	menu.CreatePopupMenu();
	CPoint pt;
	CRect rt;
	GetDlgItem(ID_PPREMINDER)->GetWindowRect(&rt);
	pt.x=rt.right;
	pt.y=rt.top;
	AddMenuString(&menu,101,_lx("RMX.UI.POSTPONE5","Postpone for 5 min"));
	AddMenuString(&menu,102,_lx("RMX.UI.POSTPONE15","Postpone for 15 min"));
	AddMenuString(&menu,103,_lx("RMX.UI.POSTPONE30","Postpone for 30 min"));
	AddMenuString(&menu,104,_lx("RMX.UI.POSTPONE1H","Postpone for 1 Hour"));
	AddMenuString(&menu,105,_lx("RMX.UI.POSTPONE1D","Postpone for 1 Day"));
	AddMenuString(&menu,106,_lx("RMX.UI.POSTPONE1W","Postpone for 1 Week"));
	AddMenuString(&menu,107,_lx("RMX.UI.POSTPONENEW","Set new time"));

	int iNum=::TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
	iRes=0;
	if(iNum!=0){
		iRes=iNum;
		CDialog::OnOK();
	}
}
