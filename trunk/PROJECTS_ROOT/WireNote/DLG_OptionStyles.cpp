// DLG_OptionStyles.cpp: implementation of the CDLG_OptionStyles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionPage_Styles property page

COptionPage_Styles::COptionPage_Styles(CWnd* pParent /*=NULL*/) 
	: CDialog(COptionPage_Styles::IDD, pParent), Track(objSettings.bDoModalInProcess)
{
	//{{AFX_DATA_INIT(COptionPage_Styles)
	m_CurStyleString = _T("");
	//}}AFX_DATA_INIT
	hTxt=NULL;
	hBg=NULL;
	font=NULL;
	bModified=FALSE;
}

COptionPage_Styles::~COptionPage_Styles()
{
	if(hTxt){
		delete hTxt;
	}
	if(hBg){
		delete hBg;
	}
}

void COptionPage_Styles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionPage_Styles)
	DDX_Control(pDX, IDC_ALPHA_SLIDER, m_Alpha);
	DDX_CBString(pDX, IDC_CUR_STYLE, m_CurStyleString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionPage_Styles, CDialog)
	//{{AFX_MSG_MAP(COptionPage_Styles)
	ON_BN_CLICKED(IDC_BUTTON_SETTXTCOL, OnButtonSettxtcol)
	ON_BN_CLICKED(IDC_BUTTON_SETBGCOL, OnButtonSetbgcol)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_CUR_STYLE, OnSelchangeCurStyle)
	ON_CBN_EDITCHANGE(IDC_CUR_STYLE, OnEditchangeCurStyle)
	ON_BN_CLICKED(IDC_BUTTON_REN, OnButtonRen)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_RND, OnButtonRnd)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SETFONT, OnButtonSetfont)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_ALPHA_SLIDER, OnReleasedcaptureAlphaSlider)
	ON_BN_CLICKED(IDC_OK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////

void COptionPage_Styles::OnDestroy() 
{
	CDialog::OnDestroy();
	UpdateData(TRUE);
}

void COptionPage_Styles::OnButtonSettxtcol() 
{
	CColorDialog dlg(0,0,this);
	SimpleTracker Track(objSettings.bDoModalInProcess);
	if(dlg.DoModal()==IDOK){
		m_TxtColor=dlg.GetColor();
		SetTxColor(m_TxtColor);
		bModified=TRUE;
	}
	Invalidate();
}

void COptionPage_Styles::OnButtonSetbgcol() 
{
	CColorDialog dlg(0,0,this);
	SimpleTracker Track(objSettings.bDoModalInProcess);
	if(dlg.DoModal()==IDOK){
		m_BgColor=dlg.GetColor();
		SetBgColor(m_BgColor);
		bModified=TRUE;
	}
	Invalidate();
}

HBRUSH COptionPage_Styles::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr;
	if(pWnd->GetDlgCtrlID()==IDC_STATIC_TXT){
		pDC->SetBkColor(m_TxtColor);
		pDC->SetTextColor(m_TxtColor);
		return HBRUSH(*hTxt);
	}else if(pWnd->GetDlgCtrlID()==IDC_STATIC_BG){
		pDC->SetBkColor(m_BgColor);
		pDC->SetTextColor(m_BgColor);
		return HBRUSH(*hBg);
	}else if(pWnd->GetDlgCtrlID()==IDC_STATIC_FONT){
		pDC->SetTextColor(m_TxtColor);
		pDC->SetBkColor(m_BgColor);
		return HBRUSH(*hBg);
	}else{
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	return hbr;
}

void COptionPage_Styles::SetTxColor(DWORD dwCol)
{
	m_TxtColor=dwCol;
	long iSc=objSettings.iLastSchemeUsed+1;
	objSettings.objSchemes.getTxtColor(iSc)=m_TxtColor;
	deleteBrush(hTxt);
	hTxt=new CBrush(m_TxtColor);
}

void COptionPage_Styles::SetBgColor(DWORD dwCol)
{
	m_BgColor=dwCol;
	long iSc=objSettings.iLastSchemeUsed+1;
	objSettings.objSchemes.getBgColor(iSc)=m_BgColor;
	deleteBrush(hBg);
	hBg=new CBrush(m_BgColor);
}

void COptionPage_Styles::SetNewFont()
{
	font=new CFont();
	font->CreateFontIndirect(&logfont);
	objSettings.objSchemes.setFont(objSettings.iLastSchemeUsed+1, font);
	GetDlgItem(IDC_STATIC_FONT)->SetFont(font);
}

void COptionPage_Styles::InitFields()
{
	long iSc=objSettings.iLastSchemeUsed+1;
	SetTxColor(objSettings.objSchemes.getTxtColor(iSc));
	SetBgColor(objSettings.objSchemes.getBgColor(iSc));
	font=objSettings.objSchemes.getFont(iSc);
	SetCurStyleName();
	memset(&logfont,0,sizeof(LOGFONT));
	font->GetLogFont(&logfont);
	SetNewFont();
	m_Alpha.SetPos(objSettings.objSchemes.getAlpha(iSc));
	OnReleasedcaptureAlphaSlider(0,0);
	Invalidate();
	((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->SetCurSel(objSettings.iLastSchemeUsed);
	GetDlgItem(IDC_BUTTON_REN)->EnableWindow(FALSE);
}

BOOL COptionPage_Styles::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_Styles");
	Sizer.InitSizer(this,objSettings.iStickPix);
	Sizer.SetOptions(STICKABLE|CENTERUNDERMOUSE);
	SetWindowText(_l("Visual styles"));
	GetDlgItem(IDC_STATIC_TEXTCOL)->SetWindowText(_l("Text color"));
	GetDlgItem(IDC_STATIC_BGCOL)->SetWindowText(_l("Background"));
	GetDlgItem(IDC_BUTTON_ADD)->SetWindowText(_l("Add new style"));
	GetDlgItem(IDC_BUTTON_DEL)->SetWindowText(_l("Delete style"));
	GetDlgItem(IDC_BUTTON_RND)->SetWindowText(_l("Randomize"));
	GetDlgItem(IDC_BUTTON_REN)->SetWindowText(_l("Rename"));
	GetDlgItem(IDC_STATIC_TSTFONT)->SetWindowText(_l("Text font"));
	GetDlgItem(IDC_STATIC_FONT)->SetWindowText(_l("Sample Text"));
	CString sOS;
	long lUnderNT=0;
	GetOSVersion(sOS,NULL,&lUnderNT);
	if(lUnderNT){
		GetDlgItem(IDC_STATIC_ALPHA)->SetWindowText(_l("Transparency"));
	}else{
		GetDlgItem(IDC_STATIC_ALPHA)->EnableWindow(FALSE);
		GetDlgItem(IDC_ALPHA_SLIDER)->EnableWindow(FALSE);
	}
	//GetDlgItem(IDC_AUTODISTRAN)->SetWindowText(_l("Disable transparency when mouse over window"));
	GetDlgItem(IDC_OK)->SetWindowText(_l("OK"));
	//
	FillComboWithColorschemes(((CComboBox*)GetDlgItem(IDC_CUR_STYLE)),1);
	m_Alpha.SetRangeMin(0);
	m_Alpha.SetRangeMax(100);
	m_Alpha.SetTicFreq(10);
	InitFields();
	SetIcon(::AfxGetApp()->LoadIcon(theApp.aAppIcons[objSettings.iTrayIcon]),TRUE);
	SetIcon(::AfxGetApp()->LoadIcon(theApp.aAppIcons[objSettings.iTrayIcon]),FALSE);
	SetTaskbarButton(this->GetSafeHwnd());
	return FALSE;
}

void COptionPage_Styles::SetCurStyleName()
{
	long iSc=objSettings.iLastSchemeUsed+1;
	GetDlgItem(IDC_CURSTYLE)->SetWindowText(_l("Current style")+": "+objSettings.objSchemes.getName(iSc));
}

void COptionPage_Styles::OnSelchangeCurStyle() 
{
	objSettings.iLastSchemeUsed=((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->GetCurSel();
	InitFields();
}

void COptionPage_Styles::OnEditchangeCurStyle() 
{
	((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->GetWindowText(sNewName);
	GetDlgItem(IDC_BUTTON_REN)->EnableWindow(TRUE);
}

void COptionPage_Styles::OnButtonRen()
{
	if(sNewName.GetLength()>0){
		long iSc=objSettings.iLastSchemeUsed+1;
		objSettings.objSchemes.getName(iSc)=sNewName;
		FillComboWithColorschemes(((CComboBox*)GetDlgItem(IDC_CUR_STYLE)),1);
		((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->SelectString(-1,sNewName);
		objSettings.iLastSchemeUsed=((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->GetCurSel();
		GetCombosState();
		InitFields();
	}
}

long iStyleCounter=1;
void COptionPage_Styles::OnButtonAdd() 
{
	sNewName=GetRandomName();//_l("New style")+Format(" %i",iStyleCounter++);
	sNewName.TrimLeft();
	sNewName.TrimRight();
	if(sNewName.GetLength()>0){
		objSettings.objSchemes.Add(sNewName,m_BgColor,m_TxtColor,m_Alpha.GetPos());
		FillComboWithColorschemes(((CComboBox*)GetDlgItem(IDC_CUR_STYLE)),1);
		((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->SelectString(-1,sNewName);
		objSettings.iLastSchemeUsed=((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->GetCurSel();
		GetCombosState();
		InitFields();
	}
}

void COptionPage_Styles::OnButtonDel() 
{
	((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->GetWindowText(sNewName);
	objSettings.objSchemes.Delete(sNewName);
	FillComboWithColorschemes(((CComboBox*)GetDlgItem(IDC_CUR_STYLE)),1);
	((CComboBox*)GetDlgItem(IDC_CUR_STYLE))->SetCurSel(0);
	objSettings.iLastSchemeUsed=0;
	bModified=TRUE;
	GetCombosState();
	InitFields();
}

void COptionPage_Styles::OnButtonRnd() 
{
	SetTxColor(RGB(rand(),rand(),rand()));
	SetBgColor(RGB(rand(),rand(),rand()));
	bModified=TRUE;
	Invalidate();
}

void COptionPage_Styles::OnButtonSetfont() 
{
	DWORD dwEffects=CF_EFFECTS|CF_SCREENFONTS;
	CFontDialog dlg(&logfont,dwEffects,NULL,this);
	dlg.m_cf.Flags|=dwEffects|CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST|CF_NOVERTFONTS;
	dlg.m_cf.lpLogFont=&logfont;
	dlg.m_cf.rgbColors=m_TxtColor;
	SimpleTracker Track(objSettings.bDoModalInProcess);
	if(dlg.DoModal()==IDOK){
		memcpy(&logfont, dlg.m_cf.lpLogFont, sizeof(LOGFONT));
		SetNewFont();
		SetTxColor(dlg.m_cf.rgbColors);
		bModified=TRUE;
		Invalidate();
	};
}

void COptionPage_Styles::GetCombosState()
{
}

void COptionPage_Styles::OnOk() 
{
	GetCombosState();
	UpdateData(TRUE);
	CDialog::OnOK();
}

LRESULT COptionPage_Styles::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void COptionPage_Styles::OnReleasedcaptureAlphaSlider(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD dwNewPos=m_Alpha.GetPos();
	if(pNMHDR!=NULL && pResult!=NULL){
		long iSc=objSettings.iLastSchemeUsed+1;
		objSettings.objSchemes.getAlpha(iSc)=dwNewPos;
		bModified=TRUE;
	}
}
