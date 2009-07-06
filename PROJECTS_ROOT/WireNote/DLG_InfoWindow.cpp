// DLG_InfoWindow.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "dlg_infowindow.h"
CString Format(const char* szFormat,...);

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
	lBlockOKForAwhile=0;
	pParent=NULL;
	bAsModal=FALSE;
	dwDontAskHolder=NULL;
	sText="";
	sTitle=_l("WireNote: Warning")+"!";
	sOkButtonText=_l("OK");
	sCancelButtonText="";
	sAskAgainButtonText=_l("Don`t show again");
	iIcon=0;//IDI_ICON_INFO_AST;
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
	CRect boundsEx,bounds;
	GetClientRect(&boundsEx);
	boundsEx.DeflateRect(4,4);
	bounds=boundsEx;
	bounds.DeflateRect(4,4);
	bounds.top+=4;
	if(pDC){
		COLORREF cTextCol=objSettings.objSchemes.getTxtColor(m_pDialog->Sets->iStyle);
		COLORREF cBgCol=objSettings.objSchemes.getBgColor(m_pDialog->Sets->iStyle);
		DWORD dwShadowStyle=((objSettings.lAlertTextStyle==1)?TXSHADOW_SIMPLE:0)|((objSettings.lAlertTextStyle==2)?TXSHADOW_GLOW:0)|TXSHADOW_MIXBG|TXSHADOW_FILLBG;
		SmartDrawText(*pDC,m_pDialog->Sets->sText,bounds,objSettings.objSchemes.getFont(m_pDialog->Sets->iStyle),cTextCol,cBgCol,dwShadowStyle,m_pDialog->Sets->dwStaticPars);
		pDC->Draw3dRect(&boundsEx,RGB(100,100,100),RGB(100,100,100));
		//
		if(objSettings.iLikStatus<=1){
			CString sText=_l("Unregistered");
			pDC->SetTextColor(RGB(250,0,0));
			long iFont=0;
			pDC->SelectObject(objSettings.objSchemes.getFont(iFont));
			pDC->DrawText( sText, &bounds, DT_SINGLELINE|DT_BOTTOM|DT_RIGHT);
		}
	}
}

void InfoWnd::GetInfoWndSize(CSize& size)
{
	size.cx=200;
	size.cy=100;
	if(!IsWindow(m_Text.GetSafeHwnd())){
		return;
	}
	CDC* pDC=m_Text.GetDC();
	if(pDC){
		CRect bounds(0,0,0,0);
		CFont* pPrev=pDC->SelectObject(objSettings.objSchemes.getFont(Sets->iStyle));
		size.cy=::DrawText(*pDC, Sets->sText, -1, &bounds, DT_CALCRECT);//Sets->dwStaticPars|
		size.cx=bounds.Width();
		pDC->LPtoDP(&size);
		size.cy+=20;
		pDC->SelectObject(pPrev);
		ReleaseDC(pDC);
	}
	size.cx+=CtrlWidth(this,0)+30;
	DWORD dwUnregSize=(objSettings.iLikStatus<=1?CtrlHeight(this,IDOK):0);
	size.cy+=CtrlHeight(this,0)+(Sets->sOkButtonText!=""?CtrlHeight(this,IDOK):0)+CtrlHeight(this,IDC_WICON)+20+dwUnregSize;
	if(Sets->dwDontAskHolder!=NULL){
		//size.cy+=CtrlHeight(this,IDC_ASKAGAIN);
		if(size.cx<200){
			size.cx=200;
		}
	}else if(size.cx<140){
		size.cx=140;
	}
	if(size.cy<70){
		size.cy=70;
	}
	AdoptSizeToScreen(size);
	return;
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
	dwMillisecondsStart=GetTickCount();
}

InfoWnd::InfoWnd(InfoWndStruct* pInfo, CWnd* pParent)
	: CDialog(IDD, pParent), m_Text(this)
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
void InfoWnd::GetInfoIcon()
{
	m_Icon.SetStyles(CBS_FLAT);
	ClearIcon(hIcon);
	if(Sets->iIcon>0){
		hIcon=_bmp().CloneAsIcon(_bmp().GetResName(Sets->iIcon,BIGLIST_ID));
		m_Icon.SetBitmap(hIcon);
	}else{
		hIcon=theApp.MainImageList.ExtractIcon(-int(Sets->iIcon));
		m_Icon.SetBitmap(AfxGetApp()->LoadIcon(IDI_ICON_BG),CBS_BACKG_N);
		m_Icon.SetStyles(CBS_FLAT | CBS_BACKG);
		CBitmap* pBMP=_bmp().GetRaw(_IL(-Sets->iIcon,BIGLIST_ID));
		m_Icon.SetBitmap(pBMP,CBS_NORML_N,50,0,-10);
	}
}

#define INFOBUTT_WIDTH	60
BOOL InfoWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(objSettings.bShadows && !IsVista()){
		SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW
	}
	Sizer.InitSizer(this,objSettings.iStickPix,Sets->bCenterUnderMouse?0:HKEY_CURRENT_USER,Sets->bCenterUnderMouse?NULL:PROG_REGKEY"\\Wnd_Info");
	// Центруем под мышой
	if(Sets->bCenterUnderMouse){
		Sizer.SetOptions(STICKABLE|SIZEABLE|CENTERUNDERMOUSE|SETCLIPSIBL);
	}else{
		Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
	}
	SetButtonSize(this,IDCANCEL,CSize(INFOBUTT_WIDTH,20));
	Sizer.AddDialogElement(IDCANCEL,heightSize);
	Sizer.AddDialogElement(IDOK,heightSize,IDCANCEL);
	Sizer.AddDialogElement(IDC_ASKAGAIN,heightSize,IDCANCEL);
	Sizer.AddDialogElement(IDC_TITLE,heightSize);
	Sizer.AddDialogElement(IDC_TEXT,0);
	Sizer.AddDialogElement(IDC_WICON,widthSize|heightSize|fixedDimensions,20);
	Sizer.AddDialogElement(IDMINIMIZE,widthSize|heightSize|fixedDimensions,20);
	//
	Sizer.AddLeftAlign(IDC_WICON,0,0);
	Sizer.AddRightAlign(IDMINIMIZE,0,0);
	if(objSettings.lAlertsOnTaskbar){
		Sizer.AddBottomAlign(IDC_TITLE,0,topPos,0);
		Sizer.AddHideRule(IDC_TITLE);
		Sizer.AddBottomAlign(IDC_WICON,0,topPos,0);
		Sizer.AddHideRule(IDC_WICON);
		Sizer.AddBottomAlign(IDMINIMIZE,0,topPos,0);
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
	Sizer.AddGotoRule(100);
	Sizer.AddGotoLabel(100);
	Sizer.AddTopAlign(IDCANCEL,IDOK,0);
	Sizer.AddLeftAlign(IDC_ASKAGAIN,0,leftPos,10);
	if(Sets->dwDontAskHolder==NULL){
		Sizer.AddBottomAlign(IDC_TEXT,IDOK,topPos,-2);
		Sizer.AddTopAlign(IDC_ASKAGAIN,IDOK,bottomPos,-2);
	}else{
		if(Sets->sCancelButtonText==""){
			Sizer.AddBottomAlign(IDC_TEXT,IDOK,topPos,-2);
			Sizer.AddTopAlign(IDC_ASKAGAIN,IDOK,topPos,0);
		}else{
			Sizer.AddBottomAlign(IDC_TEXT,IDC_ASKAGAIN,topPos,-2);
			Sizer.AddBottomAlign(IDC_ASKAGAIN,IDOK,topPos,-2);
		}
	}
	if(Sets->sCancelButtonText==""){
		Sizer.AddLeftAlign(IDCANCEL,0,rightPos,0);
		Sizer.AddRightAlign(IDCANCEL,0,rightPos,0);
		Sizer.AddRightAlign(IDOK,0,-4);
		if(Sets->dwDontAskHolder==NULL){
			Sizer.AddLeftAlign(IDOK,0,4);
		}else{
			Sizer.AddLeftAlign(IDOK,0,rightPos,-INFOBUTT_WIDTH-4);
		}
		Sizer.AddRightAlign(IDC_ASKAGAIN,IDOK,leftPos,-4);
	}else{
		Sizer.AddRightAlign(IDC_ASKAGAIN,0,rightPos,-10);
		Sizer.AddLeftAlign(IDOK,0,4);
		Sizer.AddRightAlign(IDOK,0,topCenter,-4);
		Sizer.AddLeftAlign(IDCANCEL,0,topCenter,4);
		Sizer.AddRightAlign(IDCANCEL,0,-4);
	}
	//
	ClearIcon(hIcon);
	if(Sets->hWnd){
		GetWndIcon(Sets->hWnd,hIcon);
		SafeGetWindowText(Sets->hWnd,Sets->sTitle);
	}
	if(!hIcon){
		GetInfoIcon();
	}
	if(Sets->sIconTooltip==""){
		Sets->sIconTooltip=Sets->sTitle;
	}
	m_STray.Create(NULL, WM_USER, Sets->sIconTooltip, hIcon,IDR_INFOWND_MENU,this,objSettings.lAlertsOnTaskbar?TRUE:FALSE);
	m_STray.SetMenuDefaultItem(0, TRUE);
	m_STray.SetUserMenu(&RefreshInfoMenu,NULL,this);
	if(objSettings.lAlertsOnTaskbar){
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
	m_hBr = new CBrush(objSettings.objSchemes.getBgColor(Sets->iStyle));
	this->SetFont(objSettings.objSchemes.getFont(Sets->iStyle));
	m_Text.SetFont(objSettings.objSchemes.getFont(Sets->iStyle));
	m_Icon.SetBrush(objSettings.objSchemes.getBgColor(Sets->iStyle));
	m_BtMinim.SetBitmap(theApp.LoadIcon(IDI_ICON_MINIM));
	m_BtMinim.SetStyles(CBS_FLAT);
	m_BtMinim.SetBrush(objSettings.objSchemes.getBgColor(Sets->iStyle));
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
		if(objSettings.lAlertsOnTaskbar){
			ShowWindow(SW_SHOWMINIMIZED);
		}else{
			ShowWindow(SW_HIDE);
		}
	}else{
		ShowWindow(SW_SHOW);
	}
	BOOL bNeedFocus=TRUE;
	if(Sets->lBlockOKForAwhile>0){
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	SetDefID(IDOK);
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
	if(Sets && (Sets->lBlockOKForAwhile)>0 && long(GetTickCount()-dwMillisecondsStart)>(Sets->lBlockOKForAwhile)){
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		Sets->lBlockOKForAwhile=0;
	}
	if(dwMilliseconds==0){
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
		GetDlgItem(IDOK)->SetWindowText(sNewTitle);
	}
	CDialog::OnTimer(nIDEvent);
}

void InfoWnd::RunTimer(DWORD dwTimeout)
{
	dwMilliseconds=dwTimeout;
}

BOOL InfoWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE){
		if(strlen(Sets->sOkButtonText)>0 && Sets->lBlockOKForAwhile==0){
			PostMessage(WM_SYSCOMMAND,SC_CLOSE,0);
			return TRUE;
		}
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

void InfoWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID==SC_CLOSE){
		if(strlen(Sets->sOkButtonText)==0){
			return;
		}
	}
	if(nID==SC_MINIMIZE && !objSettings.lAlertsOnTaskbar){
		ShowWindow(SW_HIDE);
		return;
	}	
	CDialog::OnSysCommand(nID, lParam);
}

BOOL InfoWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(ParseAdvert(wParam)){
		return TRUE;
	}
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

void InfoWnd::SetAutoCloseTime(DWORD dwMilliseconds, BOOL bResetStart)
{
	if(bResetStart){
		dwMillisecondsStart=GetTickCount();
	}
	RunTimer(dwMilliseconds);
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
		GetInfoWndSize(size);
		CRect minRect(0,0,size.cx,size.cy);
		Sizer.SetMinRect(minRect);
		Sizer.TestStickness(Sizer.m_rMain,FALSE);
		SetWindowPos(&wndTopMost,0,0,size.cx,size.cy,SWP_NOMOVE);
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
	pDC->SetTextColor(objSettings.objSchemes.getTxtColor(Sets->iStyle));
	pDC->SetBkColor(objSettings.objSchemes.getBgColor(Sets->iStyle));
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
	GetInfoWndSize(size);
	CRect minRect(0,0,size.cx,size.cy);
	Sizer.SetMinRect(minRect);
	Sizer.TestStickness(Sizer.m_rMain,FALSE);
	SetWindowPos(&wndTopMost,0,0,size.cx,size.cy,SWP_NOMOVE);
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
		bRes=callback(CBParam,iButton);
		if(bRes){
			callback=NULL;
		}
		EnableWindow(TRUE);
		bCallbackInProgress=FALSE;
	}
	return bRes;
}

void InfoWnd::OnCancel() 
{
	if(CheckCallback(2)){
		ExitWindow(2);
	}
}

void InfoWnd::OnOK() 
{
	if(CheckCallback(1)){
		ExitWindow(1);
	}
}

void InfoWnd::ExitWindow(int iRes)
{
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
