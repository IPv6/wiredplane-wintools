// Dlg_AppInfo.cpp : implementation file
//

перенести текст на сайт из flashpaste...
сделать опцию чтобы не показывать процессы которые нельзя убить 
ACER.EMPOWERING.FRAMEWORK.SUPERVISOR.EXE так вот из-за длины его имени колонка становится 1/4 ширины монитора... можно какнить урезать имена


#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "Dlg_AppInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_AppInfo dialog


CDlg_AppInfo::CDlg_AppInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_AppInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_AppInfo)
	m_Info = _T("");
	//}}AFX_DATA_INIT
	dwProcId=0;
	hSlWnd=0;
}


void CDlg_AppInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_AppInfo)
	DDX_Text(pDX, IDC_EDIT, m_Info);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_AppInfo, CDialog)
	//{{AFX_MSG_MAP(CDlg_AppInfo)
	ON_BN_CLICKED(ID_OPEN, OnOpen)
	ON_BN_CLICKED(ID_WNDS, OnWnds)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_AppInfo message handlers

BOOL CDlg_AppInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_AppInfo");
	Sizer.SetOptions(STICKABLE|SIZEABLE);
	CRect minRect(0,0,260,100);
	Sizer.SetMinRect(minRect);
	Sizer.AddDialogElement(IDOK,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_EDIT,0);
	Sizer.AddDialogElement(ID_OPEN,widthSize|heightSize);
	Sizer.AddDialogElement(ID_WNDS,widthSize|heightSize|hidable);
	Sizer.AddLeftTopAlign(IDC_EDIT,0,4);
	Sizer.AddRightTopAlign(IDC_EDIT,0,4);
	Sizer.AddRightBottomAlign(IDOK,0,-4);
	Sizer.AddBottomAlign(ID_OPEN,0,-4);
	Sizer.AddBottomAlign(ID_WNDS,0,-4);
	Sizer.AddRightAlign(ID_OPEN,IDOK,leftPos,-4);
	Sizer.AddRightAlign(ID_WNDS,ID_OPEN,leftPos,-4);
	Sizer.AddBottomAlign(IDC_EDIT,IDOK,topPos,-4);
	SetWindowText(_l("Application info"));
	GetDlgItem(ID_OPEN)->SetWindowText(_l("Application folder"));
	GetDlgItem(ID_WNDS)->SetWindowText(_l("Actions")+"...");
	SetIcon(theApp.MainImageList.ExtractIcon(13),TRUE);
	SetIcon(theApp.MainImageList.ExtractIcon(13),FALSE);
	SetTaskbarButton(this->GetSafeHwnd());	
	SwitchToWindow(this->GetSafeHwnd());
	return TRUE;
}

LRESULT CDlg_AppInfo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlg_AppInfo::OnOpen() 
{
	if(sAppPath!=""){
		char szDir[MAX_PATH]="",szPath[MAX_PATH]="";
		_splitpath(sAppPath,szDir,szPath,0,0);
		CString sAppDir=szDir;
		sAppDir+=szPath;
		int iRes=(int)::ShellExecute(GetSafeHwnd(),"open",sAppDir,NULL,NULL,SW_SHOWNORMAL);
	}
}

void CDlg_AppInfo::OnWnds() 
{
	CMenu menu;
	menu.CreatePopupMenu();
	if(hSlWnd!=0){
		AddMenuString(&menu,2,_l("Put link on desktop"));
		AddMenuString(&menu,3,_l("Autohide window")+"...");
	}
	AddMenuString(&menu,1,_l("Kill application"));
	::SetMenuDefaultItem(menu.m_hMenu, 0, FALSE);
	CRect rt;
	GetDlgItem(ID_WNDS)->GetWindowRect(&rt);
	RECT rDesktopRECT;
	rDesktopRECT.left=rDesktopRECT.top=-20;
	rDesktopRECT.right=rDesktopRECT.bottom=-10;
	HWND wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_DISABLED, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.right-rDesktopRECT.left, rDesktopRECT.bottom-rDesktopRECT.top, 0, 0, AfxGetApp()->m_hInstance, 0);
	DWORD dwRes=::TrackPopupMenu(menu.m_hMenu, TPM_RETURNCMD, rt.right, rt.top, 0, wndShotPhase2 , NULL);
	::DestroyWindow(wndShotPhase2);
	if(dwRes==3){
		AddToAutoHide(hSlWnd);
	}
	if(dwRes==1){
		KillProcess(dwProcId,NULL);
	}
	if(dwRes==2){
		CreateLinkInFolder(getDesktopPath(),hSlWnd);
	}
}
