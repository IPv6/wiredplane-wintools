// DLG_InfoWindow.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "dlg_infowindow.h"
CString Format(const char* szFormat,...);
extern BOOL bEnableDragWidgetMode;
void DropDragWidgetMode();
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMenu* _stdcall RefreshInfoMenu(void* pData)
{
	InfoWnd* pInfo=(InfoWnd*)pData;
	CMenu* m_pMenu=new CMenu();
	m_pMenu->CreatePopupMenu();
	AddMenuString(m_pMenu,ID_INFOWND_OPEN,_l("Open"));
	if(pInfo->Sets->hWnd==NULL){
		AddMenuString(m_pMenu,ID_INFOWND_DELETE,_l("Close"));
	}
	return m_pMenu;
}

InfoWndStruct::InfoWndStruct()
{
	pParent=NULL;
	bAsModal=FALSE;
	dwBlockCancel=0;
	bPlaceOnTaskbarAnyway=FALSE;
	bHideInfoTitler=FALSE;
	dwDontAskHolder=NULL;
	sText="";
	rtStart.SetRect(0,0,0,0);
	sTitle=_l(AppName()+": Warning")+"!";
	sOkButtonText=_l("OK");
	sCancelButtonText="";
	sAskAgainButtonText=_l("Don`t show again");
	iIcon=IDI_ICON_INFO_AST;
	dwTimer=0;
	bStartHided=0;
	bCenterUnderMouse=TRUE;
	iStyle=0;
	hWnd=NULL;
	dwStaticPars=DT_LEFT|DT_WORDBREAK;
};

InfoWndStruct::~InfoWndStruct()
{
}

/////////////////////////////////////////////////////////////////////////////
// CInfoDsc
IMPLEMENT_DYNAMIC(CInfoDsc, CButton)
CInfoDsc::CInfoDsc(InfoWnd* pDialog)
{
	m_pDialog=pDialog;
	bFirstTime=1;
}

CInfoDsc::~CInfoDsc()
{
}


BEGIN_MESSAGE_MAP(CInfoDsc, CButton)
END_MESSAGE_MAP()

void CInfoDsc::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(bFirstTime){
		bFirstTime=0;
		m_pDialog->RedrawWindow(0,0);
	}
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect boundsOrig,boundsEx,bounds;
	GetClientRect(&boundsOrig);
	boundsEx=boundsOrig;
	boundsEx.DeflateRect(4,4);
	bounds=boundsEx;
	bounds.DeflateRect(4,4);
	if(pDC){
		if(m_pDialog->dwAdditionalTxtOpts){
			pDC->FillSolidRect(boundsOrig,RGB(0,0,240));
		}
		SmartDrawText(*pDC,m_pDialog->Sets->sText,bounds,NULL,m_pDialog->m_dwTXCOLOR,m_pDialog->m_dwBGCOLOR,TXSHADOW_FILLBG|TXSHADOW_VCENT|m_pDialog->dwAdditionalTxtOpts,m_pDialog->Sets->dwStaticPars);
		pDC->Draw3dRect(&boundsEx,RGB(100,100,100),RGB(100,100,100));
	}
}


/////////////////////////////////////////////////////////////////////////////
// InfoWnd dialog
void InfoWnd::Init(InfoWndStruct* pInfo)
{
	Sets=pInfo;
	fromPoint.x=fromPoint.y=0;
	isMove=0;
	dwTimer=0;
	RunTimer(pInfo->dwTimer);
	m_hBr = NULL;
	callback=NULL;
	hIcon=NULL;
	bCallbackInProgress=FALSE;
	m_dwBGCOLOR=GetSysColor(COLOR_INFOBK);
	m_dwTXCOLOR=GetSysColor(COLOR_WINDOWTEXT);
	dwAdditionalTxtOpts=0;
	bCloseReason=IDOK;
}

InfoWnd::InfoWnd(InfoWndStruct* pInfo, CWnd* pParWnd)
	: CDialog(IDD, pParWnd), m_Text(this)
{
	Init(pInfo);
	Sets->bAsModal=TRUE;
}

InfoWnd::InfoWnd(InfoWndStruct* pInfo)
	: CDialog(), m_Text(this)
{
	//{{AFX_DATA_INIT(InfoWnd)
	//}}AFX_DATA_INIT
	Init(pInfo);
	Sets->bAsModal=FALSE;
}

InfoWnd::~InfoWnd()
{
	delete Sets;
	deleteBrush(m_hBr);
	ClearIcon(hIcon);
}

void InfoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InfoWnd)
	DDX_Control(pDX, IDMINIMIZE, m_BtMinim);
	DDX_Control(pDX, IDC_ASKAGAIN, m_AskAgain);
	DDX_Control(pDX, IDC_TITLE, m_Title);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_WICON, m_Icon);
	DDX_Control(pDX, IDC_TEXT, m_Text);
	DDX_Control(pDX, IDOK, m_OkButton);
	//}}AFX_DATA_MAP
}

void InfoWnd::SetTextParams(DWORD dwFlags)
{
	Sets->dwStaticPars=dwFlags;
}

BEGIN_MESSAGE_MAP(InfoWnd, CDialog)
	//{{AFX_MSG_MAP(InfoWnd)
	ON_WM_TIMER()
	ON_COMMAND(ID_INFOWND_DELETE, OnInfowndDelete)
	ON_COMMAND(ID_INFOWND_OPEN, OnInfowndOpen)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_ASKAGAIN, OnAskagain)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDMINIMIZE, OnBnClickedMinimize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InfoWnd message handlers

BOOL InfoWnd::GetInfoWndSize(CSize& size)
{
	size.cx=0;
	size.cy=0;
	if(!Sets->rtStart.IsRectEmpty()){
		size.cx=80;
		size.cy=60;
		return FALSE;
	}
	CDC* pDC=m_Text.GetDC();
	if(pDC){
		int iLines=0;
		CFont* pParentFont=GetFont();
		CFont* pPrev=pDC->SelectObject(pParentFont);
		size=GetLargestLineSize(pDC,Sets->sText,iLines);
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->LPtoDP(&size);
		pDC->SelectObject(pPrev);
		size.cx=size.cx+15;
		size.cy=size.cy*(iLines-1)+5;
		ReleaseDC(pDC);
	}
	size.cx+=CtrlWidth(this,0)+10;
	size.cy+=CtrlHeight(this,0)+(Sets->sOkButtonText!=""?CtrlHeight(this,IDOK):0)+CtrlHeight(this,IDC_WICON)+20;
	if(Sets->dwDontAskHolder!=NULL){
		if(size.cx<300){
			size.cx=300;
		}
	}else if(size.cx<150){
		size.cx=150;
	}
	if(size.cy<70){
		size.cy=70;
	}
	AdoptSizeToScreen(size);
	return TRUE;
}

void InfoWnd::GetInfoIcon()
{
	m_Icon.SetStyles(CBS_FLAT);
	ClearIcon(hIcon);
	if(Sets->iIcon>0){
		hIcon=::AfxGetApp()->LoadIcon(Sets->iIcon);
		m_Icon.SetBitmap(hIcon);
	}else{
		hIcon=theApp.MainImageList.ExtractIcon(-int(Sets->iIcon));
		m_Icon.SetBitmap(AfxGetApp()->LoadIcon(IDI_ICON_BG),CBS_BACKG_N);
		m_Icon.SetStyles(CBS_FLAT | CBS_BACKG);
		CBitmap* pBMP=_bmp().GetRaw(_IL(-Sets->iIcon));
		m_Icon.SetBitmap(pBMP,CBS_NORML_N,50,0,-10);
	}
}

#define INFOBUTT_WIDTH	60
BOOL InfoWnd::OnInitDialog()
{
	//::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
	CDialog::OnInitDialog();
	SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW
	/*if(Sets->bHideInfoTitler){
		ModifyStyle(0,WS_CAPTION,SWP_FRAMECHANGED);
	}*/
	Sizer.InitSizer(this,objSettings.iStickPix,Sets->bCenterUnderMouse?0:(Sets->rtStart.IsRectEmpty()?HKEY_CURRENT_USER:0),Sets->bCenterUnderMouse?NULL:(Sets->rtStart.IsRectEmpty()?PROG_REGKEY"\\Wnd_Info":""));
	// Центруем под мышой
	if(Sets->bCenterUnderMouse){
		Sizer.SetOptions(STICKABLE|SIZEABLE|CENTERUNDERMOUSE|SETCLIPSIBL);
	}else{
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
	}
	SetButtonSize(this,IDCANCEL,CSize(INFOBUTT_WIDTH,20));
	Sizer.AddDialogElement(IDCANCEL,widthSize|heightSize);
	Sizer.AddDialogElement(IDOK,heightSize,IDCANCEL);
	Sizer.AddDialogElement(IDC_ASKAGAIN,heightSize,IDCANCEL);
	Sizer.AddDialogElement(IDC_TITLE,heightSize);
	Sizer.AddDialogElement(IDC_TEXT,0);
	Sizer.AddDialogElement(IDC_WICON,widthSize|heightSize|fixedDimensions,20);
	Sizer.AddDialogElement(IDMINIMIZE,widthSize|heightSize|fixedDimensions,20);
	//
	Sizer.AddLeftAlign(IDC_WICON,0,0);
	Sizer.AddRightAlign(IDMINIMIZE,0,0);
	if(objSettings.lAlertsOnTaskbar || Sets->bPlaceOnTaskbarAnyway){
		Sizer.AddBottomAlign(IDC_TITLE,0,topPos,0);
		Sizer.AddBottomAlign(IDC_WICON,0,topPos,0);
		Sizer.AddBottomAlign(IDMINIMIZE,0,topPos,0);
		Sizer.AddHideRule(IDC_TITLE);
		Sizer.AddHideRule(IDC_WICON);
		Sizer.AddHideRule(IDMINIMIZE);
	}else{
		Sizer.AddTopAlign(IDC_TITLE,0,0);
		Sizer.AddTopAlign(IDC_WICON,0,0);
		Sizer.AddTopAlign(IDMINIMIZE,0,0);
	}
	Sizer.AddRightAlign(IDC_TITLE,IDMINIMIZE,leftPos,-4);
	Sizer.AddLeftAlign(IDC_TITLE,IDC_WICON,rightPos,4);
	Sizer.AddTopAlign(IDC_TEXT,IDC_WICON,bottomPos,2);
	Sizer.AddLeftAlign(IDC_TEXT,0,2);
	Sizer.AddRightAlign(IDC_TEXT,0,-2);
	MainRulePos=Sizer.AddGotoRule(0);
	Sizer.AddGotoLabel(0);
	Sizer.AddBottomAlign(IDOK,0,-4);
	Sizer.AddGotoRule(100);
	Sizer.AddGotoLabel(1);
	Sizer.AddTopAlign(IDOK,0,bottomPos,0);
	Sizer.AddGotoLabel(100);
	Sizer.AddBottomAlign(IDC_TEXT,IDOK,topPos,-2);
	Sizer.AddLeftAlign(IDC_ASKAGAIN,0,4);
	Sizer.AddBottomAlign(IDC_ASKAGAIN,0,-4);
	Sizer.AddBottomAlign(IDCANCEL,0,-4);
	if(Sets->sCancelButtonText!=""){
		Sizer.AddRightAlign(IDCANCEL,0,-4);
	}else{
		Sizer.AddLeftAlign(IDCANCEL,0,rightPos,0);
		Sizer.AddHideRule(IDCANCEL);
	}
	Sizer.AddRightAlign(IDOK,IDCANCEL,leftPos,-4);
	if(Sets->dwDontAskHolder==NULL){
		Sizer.AddLeftAlign(IDOK,0,4);
		Sizer.AddHideRule(IDC_ASKAGAIN);
	}else{
		Sizer.AddLeftAlign(IDOK,IDCANCEL,leftPos,-INFOBUTT_WIDTH);
	}
	Sizer.AddRightAlign(IDC_ASKAGAIN,IDOK,leftPos,-4);
	Sizer.AddLeftAlign(IDC_ASKAGAIN,0,leftPos,4);
	//
	ClearIcon(hIcon);
	GetInfoIcon();
	m_STray.Create(NULL, WM_USER, Sets->sIconTooltip, hIcon,IDR_INFOWND_MENU,this,(objSettings.lAlertsOnTaskbar || Sets->bPlaceOnTaskbarAnyway)?TRUE:FALSE);
	m_STray.SetMenuDefaultItem(0, TRUE);
	m_STray.SetUserMenu(&RefreshInfoMenu,NULL,this);
	if(objSettings.lAlertsOnTaskbar || Sets->bPlaceOnTaskbarAnyway){
		SetIcon(hIcon,TRUE);
		SetIcon(hIcon,FALSE);
		SetWindowText(Sets->sIconTooltip);
		SetTaskbarButton(this->GetSafeHwnd());
	}else{
		::SetWindowLong(this->GetSafeHwnd(),GWL_STYLE,WS_THICKFRAME|(::GetWindowLong(this->GetSafeHwnd(),GWL_STYLE)&~WS_OVERLAPPEDWINDOW));
	}
	//
	dwTimer=SetTimer(1,1000,NULL);
	SetParams(Sets->iIcon, Sets->sTitle, Sets->sText);
	if(!Sets->rtStart.IsRectEmpty()){
		Sizer.m_rMain.CopyRect(Sets->rtStart);
		SetWindowPos(&wndTopMost,Sizer.m_rMain.left,Sizer.m_rMain.top,Sizer.m_rMain.Width(),Sizer.m_rMain.Height(),0);
	}
	m_hBr = new CBrush(m_dwBGCOLOR);
	m_Icon.SetBrush(m_dwBGCOLOR);
	m_BtMinim.SetBitmap(theApp.LoadIcon(IDI_ICON_MINIM));
	m_BtMinim.SetStyles(CBS_FLAT);
	m_BtMinim.SetBrush(m_dwBGCOLOR);
	//
	GetDlgItem(IDC_WICON)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT)->EnableWindow(FALSE);
	//
	SetOkText(Sets->sOkButtonText);
	m_Cancel.SetWindowText(Sets->sCancelButtonText);
	m_AskAgain.SetWindowText(Sets->sAskAgainButtonText);
	//
	if(Sets->dwDontAskHolder==NULL){
		m_AskAgain.ShowWindow(SW_HIDE);
	}else{
		m_AskAgain.SetCheck(*Sets->dwDontAskHolder>0?TRUE:FALSE);
		UpdateData(FALSE);
	}
	if(Sets->bStartHided){
		if(objSettings.lAlertsOnTaskbar || Sets->bPlaceOnTaskbarAnyway){
			ShowWindow(SW_SHOWMINIMIZED);
		}else{        
			ShowWindow(SW_HIDE);
		}
	}else{
		ShowWindow(SW_SHOW);
	}
	BOOL bNeedFocus=TRUE;
	SetDefID(IDOK);
	if(Sizer.m_rMain.Width()>600 || Sizer.m_rMain.Height()>400){
		if(!isWin9x()){
			if(SetLayeredWindowStyle(GetSafeHwnd(),1)){
				dwAdditionalTxtOpts=TXSHADOW_GLOW|TXSHADOW_MIXBG|TXSHADOW_WIDE;
				SetWndAlpha(GetSafeHwnd(),m_dwBGCOLOR,FALSE);
				Invalidate();
			}
		}
	}

	if(Sets->dwBlockCancel){
		m_Cancel.EnableWindow(FALSE);
	}
	if(bEnableDragWidgetMode>0){
		DropDragWidgetMode();
		FORK(EmutalewWMovw,GetSafeHwnd());
	}
	return bNeedFocus;
}

void InfoWnd::SetCallback(OnButtonCallback func, void* param)
{
	if(func && ::IsWindow(this->GetSafeHwnd())){
		SetDefID(IDC_TEXT);
		GetDlgItem(IDC_TEXT)->SetFocus();
	}
	callback=func;
	CBParam=param;
}

void InfoWnd::Cleanup()
{
	m_STray.RemoveIcon();
	if(dwTimer!=0){
		KillTimer(dwTimer);
	}
}

void InfoWnd::OnTimer(UINT nIDEvent) 
{
	if(dwMilliseconds==0){
		return;
	}
	if(Sets->dwBlockCancel){
		if(Sets->dwBlockCancel && GetTickCount()-dwMillisecondsStart>Sets->dwBlockCancel){
			dwMilliseconds=0;
			m_Cancel.EnableWindow(TRUE);
		}else{
			CString sNewTitle=Sets->sCancelButtonText;
			int iTimeLeft=int((dwMilliseconds-GetTickCount()+dwMillisecondsStart)/1000);
			if(iTimeLeft>0){
				sNewTitle=Format("%s (%i)",Sets->sCancelButtonText,iTimeLeft);
			}
			m_Cancel.SetWindowText(sNewTitle);
		}
		return;
	}
	if(GetTickCount()-dwMillisecondsStart>dwMilliseconds){
		ExitWindow(0);
		return;
	}
	if(Sets->sOkButtonText==""){
		CString sNewTitle=Format("%s - %i ",Sets->sTitle,int((dwMilliseconds-GetTickCount()+dwMillisecondsStart)/1000))+_l("sec to be closed");
		m_Title.SetWindowText(sNewTitle);
		SetWindowText(sNewTitle);
	}else{
		CString sNewTitle=Format("%s (%i)",Sets->sOkButtonText,int((dwMilliseconds-GetTickCount()+dwMillisecondsStart)/1000));
		m_OkButton.SetWindowText(sNewTitle);
	}
	CDialog::OnTimer(nIDEvent);
}

void InfoWnd::RunTimer(DWORD dwTimeout)
{
	dwMilliseconds=dwTimeout;
	dwMillisecondsStart=GetTickCount();
}

BOOL InfoWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE){
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT InfoWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL InfoWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(lParam==0 && wParam==2){
		if(strlen(Sets->sOkButtonText)==0){
			return TRUE;
		}else{
			ExitWindow(1);
			return TRUE;
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}

void InfoWnd::OnInfowndDelete()
{
	if(strlen(Sets->sOkButtonText)==0){
		return;
	}
	ExitWindow(0);
}

void InfoWnd::OpenSlayerWindow()
{
	if(Sets->hWnd){
		::ShowWindow(Sets->hWnd,SW_SHOW);
		::SetForegroundWindow(Sets->hWnd);
		Sets->hWnd=NULL;
	}
}

void InfoWnd::OnClose() 
{
	ExitWindow(0);
	return;
}

void InfoWnd::OnInfowndOpen() 
{
	if(Sets->hWnd){
		this->PostMessage(WM_CLOSE,0,0);
		return;
	}else{
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}
}

void InfoWnd::SetAutoCloseTime(DWORD dwMilliseconds)
{
	RunTimer(dwMilliseconds);
}

void InfoWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID==SC_MINIMIZE && !(objSettings.lAlertsOnTaskbar || Sets->bPlaceOnTaskbarAnyway)){
		ShowWindow(SW_HIDE);
		return;
	}
	if(nID==SC_CLOSE){
		bCloseReason=IDCANCEL;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void InfoWnd::SetParams(long iIcon, const char* szTitle, const char* szText)
{
	if(Sets->hWnd==NULL && Sets->iIcon!=iIcon){
		Sets->iIcon=iIcon;
		GetInfoIcon();
		m_STray.SetIcon(hIcon);
		SetIcon(hIcon,TRUE);
		SetIcon(hIcon,FALSE);
	}
	if(szTitle){
		m_Title.SetWindowText(szTitle);
		SetWindowText(szTitle);
		Sets->sTitle=szTitle;
	}
	if(szText){
		Sets->sText=szText;
		m_Text.SetWindowText(Sets->sText);
		CSize size;
		BOOL bNeedMove=GetInfoWndSize(size);
		CRect minRect(0,0,size.cx,size.cy);
		Sizer.SetMinRect(minRect);
		Sizer.TestStickness(Sizer.m_rMain,FALSE);
		if(bNeedMove){
			SetWindowPos(&wndTopMost,0,0,size.cx,size.cy,SWP_NOMOVE);
		}
	}
}

void InfoWnd::SetInfoIcon(long iIcon)
{
	SetParams(iIcon);
}

void InfoWnd::SetTitle(const char* szText)
{
	SetParams(Sets->iIcon, szText, NULL);
}

void InfoWnd::SetText(const char* szText)
{
	SetParams(Sets->iIcon, NULL, szText);
}

HBRUSH InfoWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(m_dwTXCOLOR);
	pDC->SetBkColor(m_dwBGCOLOR);
	return HBRUSH(*m_hBr);
}

void InfoWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (isMove)
		ReleaseCapture();
	isMove=FALSE;
}

void InfoWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fromPoint=point;
	isMove=TRUE;
	SetCapture();
}

void InfoWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (isMove){
		RECT rect;
		GetWindowRect( (LPRECT) &rect );
		rect.right=rect.right-rect.left;
		rect.bottom=rect.bottom-rect.top;
		rect.left-=fromPoint.x-point.x;
		rect.top-=fromPoint.y-point.y;
		MoveWindow(rect.left,rect.top,rect.right,rect.bottom);
	}
}

void InfoWnd::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	if(!Sets->bAsModal){// Удалиться там где создалось!
		delete this;
	}
}

void InfoWnd::SetOkText(const char* szOk)
{
	Sets->sOkButtonText=szOk;
	m_OkButton.SetWindowText(Sets->sOkButtonText);
	Sizer.SetGotoRule(MainRulePos,Sets->sOkButtonText.GetLength()>0?0:1);
	//
	CSize size;
	BOOL bNeedMove=GetInfoWndSize(size);
	CRect minRect(0,0,size.cx,size.cy);
	Sizer.SetMinRect(minRect);
	Sizer.TestStickness(Sizer.m_rMain,FALSE);
	if(bNeedMove){
		SetWindowPos(&wndTopMost,0,0,size.cx,size.cy,SWP_NOMOVE);
	}
}

void InfoWnd::OnAskagain() 
{
	if(Sets->dwDontAskHolder){
		UpdateData();
		*Sets->dwDontAskHolder=m_AskAgain.GetCheck()?1:0;
	}
}

BOOL InfoWnd::InCallback()
{
	return bCallbackInProgress;
}

BOOL InfoWnd::CheckCallback(int iButton) 
{
	BOOL bRes=TRUE;
	if(callback){
		bCallbackInProgress=TRUE;
		EnableWindow(FALSE);
		bRes=callback(CBParam, iButton, GetSafeHwnd());
		callback=NULL;
		EnableWindow(TRUE);
		bCallbackInProgress=FALSE;
	}
	return bRes;
}

void InfoWnd::OnCancel() 
{
	if(CheckCallback(IDCANCEL)){
		ExitWindow(2);
	}
}

void InfoWnd::OnOK() 
{
	if(CheckCallback(IDOK)){
		ExitWindow(1);
	}
}

void InfoWnd::ExitWindow(int iRes)
{
	// На всяк случай...
	CheckCallback((iRes==1)?IDOK:IDCANCEL);
	if(Sets->hWnd){
		OpenSlayerWindow();
	}
	if(Sets->dwDontAskHolder){
		UpdateData();
		*Sets->dwDontAskHolder=m_AskAgain.GetCheck()?1:0;
		if(*Sets->dwDontAskHolder){
			*Sets->dwDontAskHolder=iRes;
		}
	}
	Cleanup();
	if(!Sets->bAsModal){
		DestroyWindow();
	}else{
		EndDialog(iRes);
	}
}
void InfoWnd::OnBnClickedMinimize()
{
	ShowWindow(SW_HIDE);
}
