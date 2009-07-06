// DLG_Options.cpp : implementation file
#include "stdafx.h"
#include "DLG_Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SPECINVERTCOL(X)	0xFFFFFF^(DWORD(X)+0x7B7B7B)
#define YELLOWR	0xFF
#define YELLOWG	0xFF
#define YELLOWB	0xE0
CString sSafeChar; 
void CDLG_Options::InitExtraData()
{
	hDefaultTopic=NULL;
	hLPaneRoot=FL_ROOT;
	hTopLPaneItem=NULL;
	hCurButtonsBarIRow=NULL;
	bAnyHotkeyWasChanged=0;
	hLastOpenedRPane=0;
	isDialogInProcess=0;
}

CString GetTooltipText(CToolTipCtrl* tl, CWnd* pWnd)
{
	if(pWnd != NULL && ::IsWindow(pWnd->GetSafeHwnd())){
		TOOLINFO ti;
		ti.hwnd = ::GetParent(pWnd->GetSafeHwnd());
		ti.uFlags = TTF_IDISHWND;
		ti.uId = (UINT_PTR)(pWnd->GetSafeHwnd());
		char sz[32000]={0};
		ti.lpszText = sz;
		::SendMessage(tl->GetSafeHwnd(), TTM_GETTEXT, 0, (LPARAM)&ti);
		return sz;
	}
	return "";
}


/////////////////////////////////////////////////////////////////////////////
// CDLG_Options dialog
HINSTANCE CheckPlugin(const char* sz, BOOL bAllowLoad=0)
{
	HINSTANCE h=GetModuleHandle(CString(sz)+".wkp");
	if(h){
		return h;
	}
	CString sP=CString("WKPLUG_")+sz;
	sP.MakeUpper();
	HWND hW=FindWindow("WK_MAIN",0);
	if(bAllowLoad && hW!=0){
		HWND hMain=(HWND)GetProp(hW,"MAIN_WND");
		if(hMain){
			::SendMessage(hMain,WM_USER+16/*LOADPLUGIN*/,(WPARAM)sz,0);
			HINSTANCE h=GetModuleHandle(CString(sz)+".wkp");
			if(h){
				return h;
			}
		}
	}
	if(hW!=0){
		return (HINSTANCE)GetProp(hW,sP);
	}
	return 0;
}

void CDLG_Options::InitAll(CDLG_Options_Startup& pStartupInfo)
{
//{{AFX_DATA_INIT(CDLG_Options)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	iTab=0;
	bNoGridOnRPanel=0;
	bAutoSkipEmptyPanes=0;
	bHHideAddit=0;
	bHHideHelp=0;
	lSipleActionIconNum=0;
	m_bSelInBold=1;
	bManualPreventCommit=0;
	bBlockCancel=0;
	bShowFind=1;
	bRPaneOnly=0;
	InitExtraData();
	//-----------
	MainRulePos=0;
	bDoNotSavePanePos=0;
	bShowColorForColorOptions=FALSE;
	m_pStartupInfo=&pStartupInfo;
	dwOptionCount=0;
	bParentOnTop=FALSE;
	dwFlg=0;
#ifdef _WIREKEYS
	lSipleRemindIconNum=34;//Теперь всегда CheckPlugin("wp_schedule")?34:0;
#else
	lSipleRemindIconNum=0;
#endif
}

CDLG_Options::CDLG_Options(CDLG_Options_Startup& pStartupInfo, CWnd* pParent, DWORD dwIFlg)
: CDialog(IDD_OPTIONS_DIALOG, pParent)
{
	InitAll(pStartupInfo);
	pParentWnd=0;
	dwFlg=dwIFlg;
}

CDLG_Options::CDLG_Options(CDLG_Options_Startup& pStartupInfo, CWnd* pParent /*=NULL*/)
: CDialog(IDD_OPTIONS_DIALOG, NULL)//CWnd::GetDesktopWindow()-убивает мышь на NT!
{
	InitAll(pStartupInfo);
	pParentWnd=pParent;
}

void CDLG_Options::DestroyOptions(BOOL bTemp)
{
	m_OptionsList.SetSelectionToZero();
	m_LList.SetSelectionToZero();
	m_ChangedClasses.RemoveAll();
	m_LList.DeleteAllIRows();
	m_OptionsList.DeleteAllIRows();
	int i=0;
	for(i=0;i<aOptions.GetSize();i++){
		COption* pOption=aOptions[i];
		if(!pOption){
			continue;
		}
		delete pOption;
	}
	aOptions.RemoveAll();
	for(i=0;i<aMenuOptions.GetSize();i++){
		COption* pOption=aMenuOptions[i];
		if(!pOption){
			continue;
		}
		delete pOption;
	}
	aMenuOptions.RemoveAll();
	if(!bTemp && pParentWnd && IsWindow(pParentWnd->GetSafeHwnd())){
		pParentWnd->EnableWindow(TRUE);
		pParentWnd->SetForegroundWindow();
		if(bParentOnTop){
			pParentWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
	}
	aFromRtoL.RemoveAll();
}

CDLG_Options::~CDLG_Options()
{
	DestroyOptions(0);
}

CString getHotKeyStr(CIHotkey oHotKey, BOOL bWithFollowers)
{
	CString sRes=GetIHotkeyName(oHotKey,bWithFollowers);
	if(sRes==""){
		sRes="<";
		sRes+=_l("not defined");
		sRes+=">";
	}
	return sRes;
}

CString ConvertToLocaleDate(COleDateTime& dt)
{
	SYSTEMTIME st;
	dt.GetAsSystemTime(st);
	char szTmp[128]={0};
	GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,0,szTmp,sizeof(szTmp));
	return szTmp;
}

CString getHotKeyStrAlt(CIHotkey oHotKey,CString sPrefix)
{
	CString sRes="";
	if(oHotKey.pNextHK){
		sRes=GetIHotkeyName(*oHotKey.pNextHK,TRUE);
	}
	if(sRes!=""){
		sRes=sPrefix+sRes;
	}
	return sRes;
}

void CDLG_Options::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Options)
	DDX_Control(pDX, IDC_OPTIONSLIST, m_OptionsList);
	DDX_Control(pDX, IDC_LPANE, m_LList);
	//}}AFX_DATA_MAP
#ifdef OPTIONS_TOOLBAR
	DDX_Control(pDX, ID_TOOLBAR1, m_Toolbar1);
	DDX_Control(pDX, ID_TOOLBAR2, m_Toolbar2);
	DDX_Control(pDX, ID_TOOLBAR3, m_Toolbar3);
	DDX_Control(pDX, ID_TOOLBAR4, m_Toolbar4);
	DDX_Control(pDX, ID_TOOLBAR5, m_Toolbar5);
	DDX_Control(pDX, ID_TOOLBAR6, m_Toolbar6);
#endif
}


BEGIN_MESSAGE_MAP(CDLG_Options, CDialog)
	//{{AFX_MSG_MAP(CDLG_Options)
	ON_NOTIFY(FLNM_ENDEDIT, IDC_OPTIONSLIST, OnEndEdit)
	ON_NOTIFY(FLNM_STARTEDIT, IDC_OPTIONSLIST, OnStartEdit)
	ON_NOTIFY(FLNM_END_INPLACE_COMBO, IDC_OPTIONSLIST, OnEndEdit)
	ON_NOTIFY(FLMN_CHECKBOX_TRIGGER, IDC_OPTIONSLIST, OnEndEdit)
	ON_NOTIFY(FLNM_END_INPLACE_DATE, IDC_OPTIONSLIST, OnEndEdit)
	ON_NOTIFY(FLNM_CUSTOMDIALOG, IDC_OPTIONSLIST, OnCustomDialog)
	ON_NOTIFY(FLNM_COMBOEXPAND, IDC_OPTIONSLIST, OnComboExpand)
	ON_BN_CLICKED(ID_FOLDERACTIONS1, OnAction1)
	ON_BN_CLICKED(ID_FOLDERACTIONS2, OnAction2)
	ON_BN_CLICKED(IDRESET, OnAdditional)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnFind)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
#ifdef OPTIONS_TOOLBAR
	ON_BN_CLICKED(ID_TOOLBAR1, OnTOOLBAR1)
	ON_BN_CLICKED(ID_TOOLBAR2, OnTOOLBAR2)
	ON_BN_CLICKED(ID_TOOLBAR3, OnTOOLBAR3)
	ON_BN_CLICKED(ID_TOOLBAR4, OnTOOLBAR4)
	ON_BN_CLICKED(ID_TOOLBAR5, OnTOOLBAR5)
	ON_BN_CLICKED(ID_TOOLBAR6, OnTOOLBAR6)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_Options message handlers
BOOL CDLG_Options::SetButtons()
{
	static BOOL bWas=FALSE;
	if(bWas==FALSE){
		bWas=TRUE;
		m_tooltip.AddTool(GetDlgItem(IDC_OPTIONSLIST), _l(OPTIONS_TP));
		m_tooltip.AddTool(GetDlgItem(IDRESET), _l("Click here to popup menu with additional actions"));
		m_tooltip.AddTool(GetDlgItem(IDOK), _l("Apply changes and exit dialog")+" (Ctrl-Enter)");
		m_tooltip.AddTool(GetDlgItem(IDCANCEL), _l("Exit dialog without saving changes")+" (Esc)");
		m_tooltip.AddTool(GetDlgItem(IDHELP), _l("Show help page"));
		m_tooltip.AddTool(GetDlgItem(ID_FOLDERACTIONS1), _l("Context-specific action"));
		m_tooltip.AddTool(GetDlgItem(ID_FOLDERACTIONS2), _l("Menu with possible actions"));
		m_tooltip.AddTool(GetDlgItem(IDAPPLY), _l("Apply options"));
		m_tooltip.AddTool(GetDlgItem(IDC_EDIT_FIND), _l("Find Option"));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_FIND), _l("Find Option"));
#ifdef OPTIONS_TOOLBAR
		m_tooltip.AddTool(GetDlgItem(ID_TOOLBAR1), "");
		m_tooltip.AddTool(GetDlgItem(ID_TOOLBAR2), "");
		m_tooltip.AddTool(GetDlgItem(ID_TOOLBAR3), "");
		m_tooltip.AddTool(GetDlgItem(ID_TOOLBAR4), "");
		m_tooltip.AddTool(GetDlgItem(ID_TOOLBAR5), "");
		m_tooltip.AddTool(GetDlgItem(ID_TOOLBAR6), "");
#endif
	}else{
		m_tooltip.UpdateTipText(_l(OPTIONS_TP),GetDlgItem(IDC_OPTIONSLIST));
		m_tooltip.UpdateTipText(_l("Click here to popup menu with additional actions"),GetDlgItem(IDRESET));
		m_tooltip.UpdateTipText(_l("Apply changes and exit dialog")+" (Ctrl-Enter)",GetDlgItem(IDOK));
		m_tooltip.UpdateTipText(_l("Exit dialog without saving changes")+" (Esc)",GetDlgItem(IDCANCEL));
		m_tooltip.UpdateTipText(_l("Show help page"),GetDlgItem(IDHELP));
		m_tooltip.UpdateTipText(_l("Context-specific action"), GetDlgItem(ID_FOLDERACTIONS1));
		m_tooltip.UpdateTipText(_l("Menu with possible actions"),GetDlgItem(ID_FOLDERACTIONS2));
		m_tooltip.UpdateTipText(_l("Apply options"), GetDlgItem(IDAPPLY));
		m_tooltip.UpdateTipText(_l("Find Option"), GetDlgItem(IDC_EDIT_FIND));
		m_tooltip.UpdateTipText(_l("Find Option"), GetDlgItem(IDC_BUTTON_FIND));
	}
	GetDlgItem(IDAPPLY)->SetWindowText(_l("Apply"));
	GetDlgItem(IDC_QHELP)->SetWindowText("");//_l("Description")
	GetDlgItem(IDOK)->SetWindowText(_l(m_pStartupInfo->szOkButtonText));
	GetDlgItem(IDCANCEL)->SetWindowText(_l(m_pStartupInfo->szCancelButtonText));
	GetDlgItem(IDRESET)->SetWindowText(_l(m_pStartupInfo->szByDefButtonText)+"...");
	GetDlgItem(IDHELP)->SetWindowText(_l(m_pStartupInfo->szHelpButtonText));
	GetDlgItem(IDC_BUTTON_FIND)->SetWindowText(_l("Find"));
	return TRUE;
}

#define OPT_VALUECOLUMN_WIDTH	0.4
#define OPT_FREEBORDER	7
#define OPT_BTBORDER	4
BOOL CDLG_Options::IniIList()
{
	m_OptionsListImages.DeleteImageList();
	m_OptionsListImages.Create(m_pStartupInfo->pImageList);
	m_OptionsListImages2.DeleteImageList();
	m_OptionsListImages2.Create(m_pStartupInfo->pImageList);
	m_OptionsList.SetImageList(&m_OptionsListImages, LVSIL_SMALL);
	m_LList.SetImageList(&m_OptionsListImages2, LVSIL_SMALL);
	return TRUE;
}

BOOL CDLG_Options::OnInitDialog() 
{
	ShowWindow(SW_HIDE);
	CDialog::OnInitDialog();
	m_OptionsList.SetReadOnly(FALSE);
	BOOL bROpts=(bNoGridOnRPanel?VIEW_NOGRD2:0);
	if(!m_pStartupInfo->bTreeOnRight){
		bROpts=bROpts|VIEW_NOTREE;
	}
	m_OptionsList.SetViewOptions(bROpts);
	
	IniIList();
	
	ListView_SetExtendedListViewStyleEx(m_OptionsList.m_hWnd, 0, LVS_EX_FULLROWSELECT);//LVS_EX_FLATSB
	m_OptionsList.InsertFColumn( 0, _l("Option"), TRUE, LVCFMT_LEFT, -1);
	m_OptionsList.InsertFColumn( 1, _l("Value") , FALSE, LVCFMT_LEFT, -1);
	m_OptionsList.SetIItemControlType(NULL, 0);
	m_OptionsList.SetIItemControlType(NULL, 1);
	strcpy(m_OptionsList.szName,"m_OptionsList");
	m_OptionsList.m_crIItemText=RGB(70,70,70);
	m_OptionsList.m_crSelectedIItemBackground=-1;//RGB(70,100,250);
	m_OptionsList.m_crSelectedIRowBackground=-1;
	m_OptionsList.m_crSelectedIItemText=RGB(0,0,0);
	m_OptionsList.bIgnoreTabKey=TRUE;
	m_OptionsList.m_crSelectedBold=1;
	m_OptionsList.bShowButtonsAsLinks=TRUE;
	m_OptionsList.bSkipTreeLines=TRUE;
	//
	m_LList.SetReadOnly(FALSE);
	m_LList.SetViewOptions(VIEW_NOGRID|VIEW_TREE0);//VIEW_NOTREE
	SetWindowLong(m_LList,GWL_STYLE,GetWindowLong(m_LList,GWL_STYLE)|LVS_NOCOLUMNHEADER);
	ListView_SetExtendedListViewStyleEx(m_LList.m_hWnd, 0, LVS_EX_FULLROWSELECT);//LVS_EX_FLATSB
	m_LList.InsertFColumn( 0, _l("Settings"), TRUE, LVCFMT_LEFT, -1);
	m_LList.SetIItemControlType(NULL, 0);
	strcpy(m_LList.szName,"m_LList");
	m_LList.m_crIItemText=RGB(70,70,70);
	m_LList.m_crSelectedIItemBackground=-1;
	m_LList.m_crSelectedIRowBackground=-1;
	m_LList.m_crSelectedIItemText=RGB(0,0,0);
	m_LList.m_crSelectedBold=m_bSelInBold?1:-1;
	m_LList.bIgnoreTabKey=TRUE;
	Sizer.InitSizer(this,m_pStartupInfo->iStickPix,HKEY_CURRENT_USER,m_pStartupInfo->szRegSaveKey);
	CRect minRect(0,0,400,300);
	Sizer.SetMinRect(minRect);
	Sizer.SetOptions(STICKABLE|SIZEABLE);
	Sizer.AddDialogElement(IDC_OPTIONSLIST,bottomResize);
	Sizer.AddDialogElement(IDOK,widthSize|heightSize);
	Sizer.AddDialogElement(IDCANCEL,widthSize|heightSize);
	if(bHHideHelp){
		Sizer.AddDialogElement(IDHELP,widthSize|heightSize|alwaysInvisible);
	}else{
		Sizer.AddDialogElement(IDHELP,widthSize|heightSize);
	}
	if(bHHideAddit){
		GetDlgItem(IDRESET)->ShowWindow(SW_HIDE);
		Sizer.AddDialogElement(IDRESET,widthSize|heightSize|alwaysInvisible);
	}else{
		Sizer.AddDialogElement(IDRESET,widthSize|heightSize|hidable);
	}
	Sizer.AddDialogElement(IDAPPLY,widthSize|heightSize);//|hidable-низя
	if(!bRPaneOnly){
		Sizer.AddDialogElement(IDC_LPANE,rightResize);
	}else{
		Sizer.AddDialogElement(IDC_LPANE,0);
	}
	Sizer.AddDialogElement(ID_FOLDERACTIONS1,heightSize);
	Sizer.AddDialogElement(ID_FOLDERACTIONS2,widthSize|heightSize);
#ifdef OPTIONS_TOOLBAR
	Sizer.AddDialogElement(ID_TOOLBAR1,widthSize|heightSize);
	Sizer.AddDialogElement(ID_TOOLBAR2,widthSize|heightSize);
	Sizer.AddDialogElement(ID_TOOLBAR3,widthSize|heightSize);
	Sizer.AddDialogElement(ID_TOOLBAR4,widthSize|heightSize);
	Sizer.AddDialogElement(ID_TOOLBAR5,widthSize|heightSize);
	Sizer.AddDialogElement(ID_TOOLBAR6,widthSize|heightSize);
#endif
	Sizer.AddDialogElement(IDC_QHELP,0);//|hidable-низя
	Sizer.AddDialogElement(IDC_QHELP_TXT,0);//|hidable-низя
	Sizer.AddDialogElement(IDC_EDIT_FIND,heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_FIND,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_SLIDER_HORZ,widthSize|heightSize);
	if(!bRPaneOnly){
		Sizer.AddDialogElement(IDC_SLIDER_VERT,widthSize|heightSize);
	}else{
		GetDlgItem(IDC_SLIDER_VERT)->ShowWindow(SW_HIDE);
	}

	//
	Sizer.AddLeftAlign(IDC_LPANE,0,OPT_FREEBORDER);
	Sizer.AddTopAlign(IDC_LPANE,0,OPT_BTBORDER);
	Sizer.AddRightAlign(IDC_OPTIONSLIST,0,-OPT_FREEBORDER);
	Sizer.AddTopAlign(IDC_OPTIONSLIST,0,OPT_BTBORDER);

	Sizer.AddTopAlign(ID_FOLDERACTIONS1,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(ID_FOLDERACTIONS2,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
#ifdef OPTIONS_TOOLBAR
	Sizer.AddTopAlign(ID_TOOLBAR1,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(ID_TOOLBAR2,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(ID_TOOLBAR3,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(ID_TOOLBAR4,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(ID_TOOLBAR5,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(ID_TOOLBAR6,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddLeftAlign(ID_TOOLBAR1,IDC_LPANE,rightPos,OPT_BTBORDER+2);
	Sizer.AddLeftAlign(ID_TOOLBAR2,ID_TOOLBAR1,rightPos,OPT_BTBORDER);
	Sizer.AddLeftAlign(ID_TOOLBAR3,ID_TOOLBAR2,rightPos,OPT_BTBORDER);
	Sizer.AddLeftAlign(ID_TOOLBAR4,ID_TOOLBAR3,rightPos,OPT_BTBORDER);
	Sizer.AddLeftAlign(ID_TOOLBAR5,ID_TOOLBAR4,rightPos,OPT_BTBORDER);
	Sizer.AddLeftAlign(ID_TOOLBAR6,ID_TOOLBAR5,rightPos,OPT_BTBORDER);
#endif	
	MainRulePos=Sizer.AddGotoRule(0);//
	Sizer.AddGotoLabel(0);
	Sizer.AddTopAlign(IDC_QHELP,IDC_OPTIONSLIST,bottomPos,OPT_BTBORDER+2);
	Sizer.AddGotoRule(100);

	Sizer.AddGotoLabel(1);
	Sizer.AddLeftAlign(ID_FOLDERACTIONS1,IDC_LPANE,rightPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(IDC_QHELP,ID_FOLDERACTIONS1,bottomPos,OPT_BTBORDER);
	Sizer.AddGotoRule(100);
#ifdef OPTIONS_TOOLBAR
	Sizer.AddGotoLabel(2);
	Sizer.AddLeftAlign(ID_FOLDERACTIONS1,ID_TOOLBAR1,rightPos,OPT_BTBORDER);
	Sizer.AddTopAlign(IDC_QHELP,ID_FOLDERACTIONS1,bottomPos,OPT_BTBORDER);
	Sizer.AddGotoRule(100);

	Sizer.AddGotoLabel(3);
	Sizer.AddLeftAlign(ID_FOLDERACTIONS1,ID_TOOLBAR2,rightPos,OPT_BTBORDER);
	Sizer.AddTopAlign(IDC_QHELP,ID_FOLDERACTIONS1,bottomPos,OPT_BTBORDER);
	Sizer.AddGotoRule(100);

	Sizer.AddGotoLabel(4);
	Sizer.AddLeftAlign(ID_FOLDERACTIONS1,ID_TOOLBAR3,rightPos,OPT_BTBORDER);
	Sizer.AddTopAlign(IDC_QHELP,ID_FOLDERACTIONS1,bottomPos,OPT_BTBORDER);
	Sizer.AddGotoRule(100);

	Sizer.AddGotoLabel(5);
	Sizer.AddLeftAlign(ID_FOLDERACTIONS1,ID_TOOLBAR4,rightPos,OPT_BTBORDER);
	Sizer.AddTopAlign(IDC_QHELP,ID_FOLDERACTIONS1,bottomPos,OPT_BTBORDER);
	Sizer.AddGotoRule(100);

	Sizer.AddGotoLabel(6);
	Sizer.AddLeftAlign(ID_FOLDERACTIONS1,ID_TOOLBAR5,rightPos,OPT_BTBORDER);
	Sizer.AddTopAlign(IDC_QHELP,ID_FOLDERACTIONS1,bottomPos,OPT_BTBORDER);
	Sizer.AddGotoRule(100);

	Sizer.AddGotoLabel(7);
	Sizer.AddLeftAlign(ID_FOLDERACTIONS1,ID_TOOLBAR6,rightPos,OPT_BTBORDER);
	Sizer.AddTopAlign(IDC_QHELP,ID_FOLDERACTIONS1,bottomPos,OPT_BTBORDER);
	Sizer.AddGotoRule(100);
#endif
	Sizer.AddGotoLabel(100);
	Sizer.AddLeftAlign(IDC_OPTIONSLIST,IDC_LPANE,rightPos,OPT_BTBORDER+2);
	Sizer.AddRightAlign(ID_FOLDERACTIONS2,0,-OPT_FREEBORDER);
	Sizer.AddRightAlign(ID_FOLDERACTIONS1,ID_FOLDERACTIONS2,leftPos,-OPT_BTBORDER);
	Sizer.AddBottomAlign(IDOK,0,-OPT_FREEBORDER);
	Sizer.AddBottomAlign(IDHELP,0,-OPT_FREEBORDER);
	Sizer.AddBottomAlign(IDRESET,0,-OPT_FREEBORDER);
	Sizer.AddBottomAlign(IDCANCEL,0,-OPT_FREEBORDER);
	Sizer.AddBottomAlign(IDAPPLY,0,-OPT_FREEBORDER);
	Sizer.AddBottomAlign(IDC_LPANE,IDOK,topPos, -OPT_BTBORDER);
	Sizer.AddBottomAlign(IDC_QHELP,IDOK, topPos, -OPT_BTBORDER);
	Sizer.AddRightAlign(IDC_QHELP,0,-OPT_FREEBORDER);
	Sizer.AddLeftAlign(IDC_QHELP,IDC_LPANE,rightPos,OPT_BTBORDER+2);
	Sizer.AddTopAlign(IDC_QHELP_TXT,IDC_QHELP,topPos,OPT_BTBORDER*4);
	Sizer.AddLeftAlign(IDC_QHELP_TXT,IDC_QHELP,leftPos,OPT_BTBORDER*4);
	Sizer.AddBottomAlign(IDC_QHELP_TXT,IDC_QHELP,bottomPos,-OPT_BTBORDER*4);
	Sizer.AddRightAlign(IDC_QHELP_TXT,IDC_QHELP,rightPos,-OPT_BTBORDER*4);
	Sizer.AddLeftAlign(IDHELP,0,OPT_FREEBORDER);
	Sizer.AddLeftAlign(IDRESET,IDHELP,rightPos,OPT_BTBORDER);
	Sizer.AddRightAlign(IDAPPLY,0,-OPT_FREEBORDER);
	Sizer.AddRightAlign(IDCANCEL,IDAPPLY,leftPos,-OPT_BTBORDER);
	Sizer.AddRightAlign(IDOK,IDCANCEL,leftPos,-OPT_BTBORDER);
	if(!bRPaneOnly){
		Sizer.AddRightAlign(IDC_LPANE,IDCANCEL,leftPos|ifBigger,0);
		Sizer.AddRightAlign(IDC_LPANE,IDC_LPANE,leftPos|ifLess,0);
	}
	Sizer.AddBottomAlign(IDC_OPTIONSLIST,IDC_LPANE, bottomPos|ifBigger,-50);
	Sizer.AddBottomAlign(IDC_OPTIONSLIST,IDC_OPTIONSLIST, topPos|ifLess,0);
	if(bShowFind==2 || bShowFind==0){
		GetDlgItem(IDC_BUTTON_FIND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_FIND)->ShowWindow(SW_HIDE);
	}
	if(bShowFind==3){
		GetDlgItem(IDC_EDIT_FIND)->ShowWindow(SW_HIDE);
	}
	Sizer.AddRightAlign(IDC_BUTTON_FIND,IDOK,leftPos,-OPT_BTBORDER);
	Sizer.AddRightAlign(IDC_EDIT_FIND,IDC_BUTTON_FIND,leftPos,-OPT_BTBORDER);
	Sizer.AddLeftAlign(IDC_EDIT_FIND,IDRESET,rightPos,OPT_BTBORDER);
	Sizer.AddBottomAlign(IDC_EDIT_FIND,0,-OPT_FREEBORDER);
	Sizer.AddBottomAlign(IDC_BUTTON_FIND,0,-OPT_FREEBORDER);
	if(!bRPaneOnly){
		Sizer.AddRightAlign(IDC_SLIDER_VERT,IDC_LPANE,rightPos,2);
		Sizer.AddLeftAlign(IDC_SLIDER_VERT,IDC_LPANE,rightPos,1);
		Sizer.AddSideCenterAlign(IDC_SLIDER_VERT,IDC_LPANE,sideCenter,0);
	}
	Sizer.AddBottomAlign(IDC_SLIDER_HORZ,IDC_OPTIONSLIST,bottomPos,4);
	Sizer.AddTopAlign(IDC_SLIDER_HORZ,IDC_OPTIONSLIST,bottomPos,3);
	Sizer.AddTopCenterAlign(IDC_SLIDER_HORZ,IDC_OPTIONSLIST,topCenter,0);

	/*
	//GetDlgItem(IDC_BUTTON_FIND)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_FIND)->ShowWindow(SW_HIDE);
	SetButtonSize(this,IDC_BUTTON_FIND,CSize(GetSystemMetrics(SM_CYMENU)-3,GetSystemMetrics(SM_CYMENU)-3));
	SetButtonSize(this,IDC_EDIT_FIND,CSize(GetSystemMetrics(SM_CYMENU),GetSystemMetrics(SM_CYMENU)-3));
	Sizer.AddTopAlign(IDC_BUTTON_FIND,0,-GetSystemMetrics(SM_CYMENU)+1);
	Sizer.AddTopAlign(IDC_EDIT_FIND,0,-GetSystemMetrics(SM_CYMENU)+1);
	Sizer.AddRightAlign(IDC_BUTTON_FIND,0,-1);
	Sizer.AddRightAlign(IDC_EDIT_FIND,IDC_BUTTON_FIND,leftPos,-OPT_BTBORDER);
	Sizer.AddLeftAlign(IDC_EDIT_FIND,IDAPPLY,leftPos,1);
	*/
	{// CG: The following block was added by the ToolTips component.
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);
		m_tooltip.SetMaxTipWidth(int(GetDesktopRect().Width()*0.4));
		SetButtons();
	}
	//Остальная инициализация
	FillOptionListWithData();
	SetWindowText(m_pStartupInfo->szWindowTitle);
	FinishFillOptionListWithData();

	if(bBlockCancel){
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	}
	// Показываем наконец окно
	SetTaskbarButton(this->GetSafeHwnd());
	HICON hIMain=m_pStartupInfo->hWindowIcon;
	if(hIMain==0){
		hIMain=::AfxGetApp()->LoadIcon(m_pStartupInfo->iWindowIcon);
	}
	SetIcon(hIMain,TRUE);
	SetIcon(hIMain,FALSE);
	ShowWindow(SW_SHOW);
	SwitchToWindow(GetSafeHwnd(),TRUE);
	// При первом запуске окно может просто не показаться, поэтому свитчим приложение
	if(pParentWnd){
		if(GetWindowLong(pParentWnd->GetSafeHwnd(),GWL_EXSTYLE)&WS_EX_TOPMOST){
			bParentOnTop=TRUE;
			pParentWnd->SetWindowPos(this,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
		::EnableWindow(pParentWnd->GetSafeHwnd(),FALSE);
	}
	GetDlgItem(IDAPPLY)->EnableWindow(FALSE);
	// Настройки выравнивания заданы, продолжаем
	//SetForegroundWindow();
	//::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)this->GetSafeHwnd());
	return TRUE;
}

LRESULT CDLG_Options::SaveColumnWidths()
{
	long lColumn1=m_OptionsList.GetColumnWidth(0);
	long lColumn2=m_OptionsList.GetColumnWidth(1);
	CRect rt1,rt2;
	m_LList.GetWindowRect(&rt1);
	m_OptionsList.GetWindowRect(&rt2);
	long lH1=rt2.Height();
	long lW1=rt1.Width();
	CString sValue=Format("col1={%i};col2={%i};w={%i};h={%i}",lColumn1,lColumn2,lW1,lH1);
	CString sKeyName=Sizer.m_szRegKey;
	int iPos=sKeyName.ReverseFind('\\');
	if(iPos==-1){
		return 0;
	}
	CString sValueName=sKeyName.Mid(iPos+1);
	sKeyName=sKeyName.Left(iPos);
	CRegKey key;
	key.Create(Sizer.m_hKeyRoot, sKeyName);
	sValueName+="cW";
	DWORD dwType=REG_SZ,dwSize=0;
	if(RegSetValueEx(key.m_hKey,sValueName,0,REG_SZ,(BYTE*)(const char*)sValue,lstrlen(sValue)+1)!= ERROR_SUCCESS){
		return 0;
	}
	return 1;
}

LRESULT CDLG_Options::LoadColumnWidths() 
{
	CString sKeyName=Sizer.m_szRegKey;
	int iPos=sKeyName.ReverseFind('\\');
	if(iPos==-1){
		return 0;
	}
	CString sValueName=sKeyName.Mid(iPos+1);
	sKeyName=sKeyName.Left(iPos);
	char szValue[256];
	CRegKey key;
	key.Create(Sizer.m_hKeyRoot, sKeyName);
	DWORD dwCount=sizeof(szValue);
	DWORD dwType=0;
	sValueName+="cW";
	CRect rect;
	long lColumn1=0,lColumn2=0;
	long lW1=200,lH1=270;
	m_OptionsList.GetWindowRect(&rect);
	if(RegQueryValueEx(key.m_hKey,sValueName,NULL, &dwType,(LPBYTE)szValue, &dwCount)== ERROR_SUCCESS){
		lColumn1=atol(CDataXMLSaver::GetInstringPart("col1={","}",szValue));
		lColumn2=atol(CDataXMLSaver::GetInstringPart("col2={","}",szValue));
		lW1=atol(CDataXMLSaver::GetInstringPart("w={","}",szValue));
		lH1=atol(CDataXMLSaver::GetInstringPart("h={","}",szValue));
	}
	// Положения окон
	CRect rtFull;
	GetWindowRect(&rtFull);
	m_LList.GetWindowRect(&rect);
	if(bDoNotSavePanePos){
		rect.right=rect.left+3;
	}else{
		if(lW1>rtFull.Width()){
			lW1=rtFull.Width()-200;
		}
		if(lW1<30){
			lW1=30;
		}
		rect.right=rect.left+lW1;
	}
	ScreenToClient(&rect);
	m_LList.MoveWindow(&rect);
	m_OptionsList.GetWindowRect(&rect);
	if(lH1>rtFull.Height()){
		lH1=rtFull.Height()-100;
	}
	if(lH1<30){
		lH1=30;
	}
	rect.bottom=rect.top+lH1;
	ScreenToClient(&rect);
	m_OptionsList.MoveWindow(&rect);
	// Ширина колонок
	if(lColumn1==0 || lColumn2==0){
		double dWndSize=rect.Width()-::GetSystemMetrics(SM_CYVSCROLL)-5;
		lColumn1=long(dWndSize-dWndSize*OPT_VALUECOLUMN_WIDTH);
		lColumn2=long(dWndSize*OPT_VALUECOLUMN_WIDTH);
	}
	if(lColumn1+lColumn2>rect.Width()){
		lColumn2=rect.Width()-lColumn1-5;
	}
	m_OptionsList.SetColumnWidth(0,lColumn1);
	m_OptionsList.SetColumnWidth(1,lColumn2);
	m_LList.SetColumnWidth(0,300);
	Sizer.ApplyLayout(TRUE);
	return 1;
}

void CDLG_Options::AdjustRPaneW(BOOL bTwicked)
{
	CRect rt;
	m_OptionsList.GetClientRect(&rt);
	int iCol0=m_OptionsList.GetColumnWidth(0);
	int iW=rt.Width();
	if(iCol0>iW){
		iCol0=int(double(iW)*0.7);
		m_OptionsList.SetColumnWidth(0,iCol0);
	}
	int iCol1=iW-iCol0-1;
	if(bTwicked){
		m_OptionsList.SetColumnWidth(1,iCol1+2);
	}
	m_OptionsList.SetColumnWidth(1,iCol1);
	m_LList.GetClientRect(&rt);
	m_LList.SetColumnWidth(0,iW);
	m_OptionsList.Invalidate();
}

LRESULT CDLG_Options::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	int iSizerState1=Sizer.iInit;
	static BOOL bScheduleAdjust=0;
	static CRect rtPrev;
	if(bScheduleAdjust && GetKeyState(VK_LBUTTON)>=0){
		bScheduleAdjust=0;
		AdjustRPaneW(1);
		DrawNC();
	}
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		if(message==WM_SIZE || (message==WM_MOUSEMOVE && Sizer.bInResizingLock)){
			// Меряем изменение
			bScheduleAdjust=1;
			CRect rtNow;
			m_OptionsList.GetWindowRect(rtNow);
			if(rtPrev.Width()<rtNow.Width()
				|| rtPrev.Height()<rtNow.Height()
				|| GetKeyState(VK_LBUTTON)>=0){
				AdjustRPaneW();
			}
			rtPrev.CopyRect(rtNow);
		}
		return DispatchResultFromSizer;
	}
	int iSizerState2=Sizer.iInit;
	if(iSizerState1==2 && iSizerState2!=2){
		LoadColumnWidths();
	}
	if(message==WM_DESTROY){
		SaveColumnWidths();
	}
	if(message==WM_HELP){
		OnHelp();
		return TRUE;
	}
	if(message==WM_COMMAND && wParam==IDCANCEL){
		CancelThis();
		return TRUE;
	}
	if(message==WM_COMMAND && wParam==REMOVE_OPTIONS){
		PopSomeDataFromScreen(lParam);
		Invalidate();
		return TRUE;
	}
	if(message==WM_COMMAND && lParam==0 && wParam>=UPMENU_ID && m_pStartupInfo->fpUpMenuAction){
		(*m_pStartupInfo->fpUpMenuAction)(wParam,lParam,this);
		return TRUE;
	}
	LRESULT lOut=CDialog::WindowProc(message, wParam, lParam);
	if(message==WM_MOUSEMOVE || message==WM_ACTIVATE ||message==WM_NCACTIVATE || message==WM_MOVING || message==WM_SIZING ||message==WM_SIZE || message==WM_NCPAINT || message==WM_PAINT || message==WM_SHOWWINDOW){
		DrawNC();
	}	
	return lOut;
}

void CDLG_Options::OnStartEdit(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	*result=0;
	if(pRowState==NULL){
		return;
	}
	DWORD dwItemData=m_OptionsList.GetIItemData(pRowState->hIRow,pRowState->iIItem);
	COption* pOpt=aOptions[dwItemData];
	if(!pOpt){
		return;
	}
	if(pOpt && pOpt->iOptionType==OPTTYPE_HOTKEY){
		*result=(DWORD)(new CIHotkey(((COptionHotKey*)pOpt)->m_Value));
	}
	if(pRowState->dwNotifyData==IDC_INPLACE_SLIDER && pOpt && pOpt->iOptionType==OPTTYPE_NUMBER){
		*result=MAKELONG(((COptionNumber*)pOpt)->m_dwMin,((COptionNumber*)pOpt)->m_dwMax);
	}
	return;
}

void CDLG_Options::OnEndEdit(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(pRowState==NULL){
		*result=-1;
		return;
	}
	*result=0;//принять измененное значение
	DWORD dwItemData=m_OptionsList.GetIItemData(pRowState->hIRow,pRowState->iIItem);
	COption* pOpt=aOptions[dwItemData];
	if(!pOpt){
		*result=-1;
		return;
	}
	if(pOpt->iOptionType==OPTTYPE_BOOL){
		((COptionBOOL*)pOpt)->m_Value=pRowState->dwNotifyData;
		OnChanged(pOpt);
	}else if(pOpt->iOptionType==OPTTYPE_DATE){
		int iY=LOWORD(pRowState->dwNotifyData);
		int iM=LOBYTE(HIWORD(pRowState->dwNotifyData));
		int iD=HIBYTE(HIWORD(pRowState->dwNotifyData));
		((COptionDate*)pOpt)->m_Value=COleDateTime(iY,iM,iD,0,0,0);
		m_OptionsList.SetIItemText(pOpt->hIRow, 1, ConvertToLocaleDate(((COptionDate*)pOpt)->m_Value));
		OnChanged(pOpt);
	}else if(pOpt->iOptionType==OPTTYPE_NUMBER){
		long dwVal=atol(pRowState->strText);
		CString sNew=Format("%i",dwVal);
		if(sNew!=pRowState->strText || dwVal<((COptionNumber*)pOpt)->m_dwMin || dwVal>((COptionNumber*)pOpt)->m_dwMax){
			this->MessageBox(_l("Value must be between")+" "+Format("%i",((COptionNumber*)pOpt)->m_dwMin)+" "+_l("and")+" "+Format("%i",((COptionNumber*)pOpt)->m_dwMax));
			*result=-1;
		}else{
			((COptionNumber*)pOpt)->m_Value=dwVal;
			OnChanged(pOpt);
		}
	}else if(pOpt->iOptionType==OPTTYPE_STRING){
		// Здесь можно не проверять на ForGroup...
		((COptionString*)pOpt)->m_Value=pRowState->strText;
		OnChanged(pOpt);
	}else if(pOpt->iOptionType==OPTTYPE_COMBO){
		CString sComboItemsData=((COptionCombo*)pOpt)->m_ComboData;
		CStringArray sItemsData;
		ConvertComboDataToArray(sComboItemsData,sItemsData);
		for(int i=0;i<sItemsData.GetSize();i++){
			if(sItemsData[i]==pRowState->strText){
				((COptionCombo*)pOpt)->m_Value=i;
				OnChanged(pOpt);
				break;
			}
		}
	}else if(pOpt->iOptionType==OPTTYPE_HOTKEY){
		CIHotkey* pHotkey=(CIHotkey*)(pRowState->dwNotifyData);
		if(pHotkey){
			bAnyHotkeyWasChanged=1;
			((COptionHotKey*)pOpt)->m_Value=*pHotkey;
			pRowState->strText=getHotKeyStr(((COptionHotKey*)pOpt)->m_Value,TRUE);
			OnChanged(pOpt);
			delete pHotkey;
		}
	}else{
		*result=-1;//оставить прежнее значение
	}
	return;
}

void CDLG_Options::OnCustomDialog(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(pRowState==NULL || isDialogInProcess>0){
		*result=-1;
		return;
	}
	SimpleTracker Track(isDialogInProcess);
	*result=0;//принять измененное значение
	DWORD dwItemData=m_OptionsList.GetIItemData(pRowState->hIRow,pRowState->iIItem);
	COption* pOpt=aOptions[dwItemData];
	if(!pOpt){
		*result=-1;
		return;
	}
	if(pOpt && pOpt->iOptionType==OPTTYPE_STRING){
		// Здксь можно не проверять на ForGroup
		dwItemData=m_OptionsList.GetIItemData(pRowState->hIRow,0);
		pOpt=aOptions[dwItemData];
		if(!pOpt){
			*result=-1;
			return;
		}
	}
	if(pOpt && pOpt->iOptionType==OPTTYPE_ACTION){
		LPVOID fpData= ((COptionAction*)pOpt)->functionParam;
		if(((COptionAction*)pOpt)->function){
			FP_OPTIONACTION fp = ((COptionAction*)pOpt)->function;
			if(fp){
				if((*fp)(HIROW(fpData),this)){
					OnChanged(pOpt);
				}
			}
		}else{
			FP_OPTIONACTION2 fp = ((COptionAction*)pOpt)->function2;
			if(fp){
				if((*fp)(fpData,this)){
					OnChanged(pOpt);
				}
			}
		}
	}else if(pOpt && pOpt->iOptionType==OPTTYPE_COLOR){
		CColorDialog dlg(((COptionColor*)pOpt)->m_Value,0,this);
		if(dlg.DoModal()==IDOK){
			DWORD dwColor=dlg.GetColor();
			((COptionColor*)pOpt)->m_Value=dwColor;
			m_OptionsList.SetIItemColors(((COptionColor*)pOpt)->hIRow,1,SPECINVERTCOL(dwColor),dwColor);
			m_OptionsList.SetIRowTextStyle(((COptionColor*)pOpt)->hIRow,FL_SELBORD);
			if(bShowColorForColorOptions){
				m_OptionsList.SetIItemText(((COptionColor*)pOpt)->hIRow, 1, Format("#%02X%02X%02X",GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor)));
			}
			OnChanged(pOpt);
		}
	}else if(pOpt && pOpt->iOptionType==OPTTYPE_FONT){
		LOGFONT logfont;
		DWORD dwEffects=CF_EFFECTS|CF_SCREENFONTS;
		CFont** font=((COptionFont*)pOpt)->m_Value_Real;
		if(font){
			(*font)->GetLogFont(&logfont);
			CFontDialog dlg(&logfont,dwEffects,NULL,this);
			dlg.m_cf.Flags|=dwEffects|CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST|CF_NOVERTFONTS;
			dlg.m_cf.lpLogFont=&logfont;
			if(dlg.DoModal()==IDOK){
				delete (*font);
				(*font)=new CFont();
				(*font)->CreateFontIndirect(dlg.m_cf.lpLogFont);
				m_OptionsList.SetIItemText(((COptionFont*)pOpt)->hIRow,1,GetFontTextDsc((*font)));
				OnChanged(pOpt);
			}
		}
	}else if(pOpt && pOpt->iOptionType==OPTTYPE_DATE){
		static long lAlreadyOpened=0;
		if(lAlreadyOpened==0){
			SimpleTracker Track(lAlreadyOpened);
			CRect rect(0,0,0,0);
			m_OptionsList.GetItemRect(m_OptionsList.GetIRowIndex(((COptionDate*)pOpt)->hIRow), &rect, LVIR_BOUNDS);
			m_OptionsList.ClientToScreen(&rect);
			//ClientToScreen(&rect);
			CInPlaceDate* pEdit = new CInPlaceDate(((COptionDate*)pOpt)->hIRow, 1, ((COptionDate*)pOpt)->m_Value,&m_OptionsList);
			pEdit->CreateEx(0,"SysMonthCal32","",WS_POPUP|WS_BORDER, rect, &m_OptionsList, 0);
			//pEdit->Create(WS_CHILD|WS_BORDER, rect, &m_OptionsList, IDC_INPLACE_DATE);
			if(IsWindow(pEdit->GetSafeHwnd())){
				CRect rtMin;
				pEdit->GetMinReqRect(&rtMin);
				rect.left=rect.right-rtMin.Width();
				rect.bottom=rect.top+rtMin.Height();
				pEdit->SetWindowPos(&wndTopMost,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_DRAWFRAME);
				pEdit->SetFocus();
			}
		}
	}else{
		*result=-1;//оставить прежнее значение		
	}
}

void CDLG_Options::OnComboExpand(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_COMBOSTATE *pRowState=(FLNM_COMBOSTATE*) pNotifyStruct;
	structInPlaceComboItem ci;

	// Вытаскиваем данные...
	DWORD dwItemData=m_OptionsList.GetIItemData(pRowState->hIRow,pRowState->iIItem);
	COption* pOpt=aOptions[dwItemData];
	if(!pOpt){
		*result=-1;
		return;
	}
	if(pOpt->iOptionType==OPTTYPE_COMBO){
		CString sComboItemsData=((COptionCombo*)pOpt)->m_ComboData;
		CStringArray sItemsData;
		ConvertComboDataToArray(sComboItemsData,sItemsData);
		for(int i=0;i<sItemsData.GetSize();i++){
			ci.dwData=i;
			ci.strText=sItemsData[i];
			pRowState->paComboItems->Add(ci);
		}
		//	выбираем элемент, который будет выделен в списке
		for(int j=0; j<pRowState->paComboItems->GetSize(); j++){
			if(pRowState->paComboItems->GetAt(j).strText==pRowState->strText){
				pRowState->iSelectedItem=j;
				break;
			}
		}
	}
}

CString GetOptionStrings(const char* szText,CString& sTitle,CString& sDsc)
{
	sTitle=szText;
	sTitle=sTitle.SpanExcluding("\t");
	if(strlen(szText)>strlen(sTitle)){
		sDsc=szText+sTitle.GetLength()+1;
	}
	if(sDsc==""){
		return sTitle;
	}
	return sDsc;
}

HIROW CDLG_Options::AddRow(HIROW fParent, const char* szText, long dwIconNum, BOOL bAtStart)
{
	HIROW hThis=m_OptionsList.InsertIRow(fParent, bAtStart?FL_FIRST:FL_LAST, szText, dwIconNum);
	m_OptionsList.SetIItemControlType(hThis, 0, FLC_READONLY);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_READONLY);
	m_OptionsList.SetIItemData(hThis, 0, -1);
	m_OptionsList.SetIItemData(hThis, 1, -1);
	//m_OptionsList.SetIItemColors(hThis, 0, 0, RGB(255,255,255));
	return hThis;
}

BOOL CDLG_Options::isOptionClassChanged(long iOptionsClass)
{
	if(iOptionsClass==-1){
		return m_ChangedClasses.GetSize()>0;
	}
	for(int i=0;i<m_ChangedClasses.GetSize();i++){
		if(long(m_ChangedClasses[i])==iOptionsClass){
			return TRUE;
		}
	}
	return FALSE;
}

HIROW CDLG_Options::GetAliasHIROW(HIROW hRPane)
{
	HIROW hLeft=NULL;
	aFromRtoL.Lookup(hRPane,hLeft);
	return hLeft;
}

HIROW CDLG_Options::SetLAliasTarget(HIROW hLPaneFrom,HIROW hRPaneTo)
{
	m_LList.SetIItemData(hLPaneFrom, 0, (DWORD)hRPaneTo);
	return hLPaneFrom;
}

HIROW CDLG_Options::GetOrAddAlias(HIROW hRPane, const char* szText, DWORD dwIcon, HIROW fRParent, HIROW hAfter)
{
	HIROW hLeft=GetAliasHIROW(hRPane);
	if(hLeft!=NULL){
		SetLAliasTarget(hLeft, hRPane);
		return hLeft;
	}
	HIROW hRParent=GetAliasHIROW(fRParent);
	if(hRParent==NULL){
		hRParent=hLPaneRoot;
	}
	if(hAfter==FL_ROOT){
		hAfter=hTopLPaneItem;
	}
	if(hRParent==FL_ROOT && hAfter==FL_ROOT){
		hAfter=FL_LAST;
	}
	hLeft=m_LList.InsertIRow(hRParent, (hAfter)?(hAfter==HIROW(-1)?FL_FIRST:hAfter):FL_LAST, szText, dwIcon);
	m_LList.SetIItemControlType(hLeft, 0, FLC_READONLY);
	m_LList.SetIItemData(hLeft, 0, (DWORD)hRPane);
	if(hRPane!=NULL){
		aFromRtoL.SetAt(hRPane,hLeft);
	}
	return hLeft;
}

BOOL CDLG_Options::CompareToDefTopic(const char* szText)
{
	if(m_pStartupInfo->szDefaultTopicTitle && strlen(m_pStartupInfo->szDefaultTopicTitle)>0){
		 //&& stricmp(m_pStartupInfo->szDefaultTopicTitle,szText)==0
		CString s1=m_pStartupInfo->szDefaultTopicTitle;
		CString s2=szText;
		s1=s1.SpanExcluding("[({})]");
		s2=s2.SpanExcluding("[({})]");
		if(s1.CompareNoCase(s2)==0){
			//CString s;			s.Format("Def %s vs %s",s1,s2);			AfxMessageBox(s);
			return TRUE;
		}
	}
	return 0;
}

HIROW CDLG_Options::Option_AddAlias(HIROW hData, const char* szText, DWORD dwIcon, HIROW hParent, BOOL bToBeginning)
{
	HIROW hLeft=GetOrAddAlias(hData, szText, dwIcon, hParent, bToBeginning?HIROW(-1):0);
	if(m_pStartupInfo->szDefaultTopicTitle && CompareToDefTopic(szText)){
		hDefaultTopic=hData;
	}
	return hLeft;
}

//bWithAlias==0 - без пункта слева
//bWithAlias==1 - с пунктом слева, ведущим на правую половину
//bWithAlias==2 - с пунктом слева, не ведущим никуда
//bWithAlias==3 - с пунктом слева, ведущим на правую половину, добавленным в начало списка
HIROW CDLG_Options::Option_AddTitleEx(HIROW fParent, const char* szText, DWORD dwIcon, BOOL bWithAlias, HIROW hAfter)
{
	CString sTitle,sDsc;
	GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThisParent=fParent;
	if(bWithAlias){
		hThisParent=FL_ROOT;
	}
	HIROW hThis=AddRow(hThisParent,sTitle,dwIcon);
	if(hThisParent==FL_ROOT){
		m_OptionsList.SetIRowTextStyle(hThis,FL_SKIPTRI);
	}
	if(bWithAlias){
		HIROW hLeft=GetOrAddAlias((bWithAlias==2)?NULL:hThis,sTitle,dwIcon,fParent,hAfter);
	}
	Option_SetRowColor(hThis,0,90);
	Option_SetRowColor(hThis,1,90);
	if(sDsc!=""){
		m_OptionsList.SetIItemText(hThis, 1, sDsc);
		/*
		{
		DWORD crText1=0,crBg1=0;
		m_OptionsList.GetIItemColors(hThis,1,crText1,crBg1);
		DWORD crText=m_OptionsList.m_crIItemText,crBackground=m_OptionsList.m_crIRowBackground;
		m_OptionsList.SetIItemColors(hThis,1,RGB(GetRValue(crText)+int(GetRValue(crBackground)-GetRValue(crText))/2,GetGValue(crText)+int(GetGValue(crBackground)-GetGValue(crText))/2,GetBValue(crText)+int(GetBValue(crBackground)-GetBValue(crText))/2),crBg1);
		}
		*/
	}else{
		m_OptionsList.SetIItemColSpan(hThis, 0, 1);
	}
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_READONLY);
	if(!hDefaultTopic && CompareToDefTopic(sTitle)){
		hDefaultTopic=hThis;
	}
	return hThis;
}

HIROW CDLG_Options::Option_AddHeader(HIROW fParent, const char* szText, DWORD dwIcon, BOOL bWithoutAlias, HIROW hAfter)
{
	return Option_AddTitleEx(fParent, szText, dwIcon, bWithoutAlias?2:1, hAfter);
}

HIROW CDLG_Options::Option_AddTitle(HIROW fParent, const char* szText, DWORD dwIcon)
{
	return Option_AddTitleEx(fParent, szText, dwIcon, 0);
}

HIROW CDLG_Options::Option_AddDate(HIROW fParent, const char* szText, COleDateTime* pDateVariable, COleDateTime dtDefValue, long dwIcon, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	long lDateIcon=80;
	if(dwIcon>0){
		lDateIcon=dwIcon;
	}
	if(dwIcon<-1){
		lDateIcon=-dwIcon;
	}
	HIROW hThis=AddRow(fParent,sTitle, lDateIcon);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_CUSTOMDIALOG);
	m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	if(pDateVariable->GetStatus()!=COleDateTime::valid){
		*pDateVariable=COleDateTime::GetCurrentTime();
	}
	CString sText=ConvertToLocaleDate(*pDateVariable);
	m_OptionsList.SetIItemText(hThis, 1, sText);
	COption* pOpt=new COptionDate(dtDefValue, pDateVariable, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	if(!hDefaultTopic && CompareToDefTopic(sTitle)){
		hDefaultTopic=hThis;
	}
	return hThis;
}

HIROW CDLG_Options::Option_AddBOOL(HIROW fParent, const char* szText, long* pBoolVariable, long dwDefValue, long dwIcon, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	long lBoolIcon=-1;
	if(dwIcon>0){
		lBoolIcon=dwIcon;
	}
	if(dwIcon<-1){
		lBoolIcon=-dwIcon;
	}
	HIROW hThis=AddRow(fParent,sTitle, lBoolIcon);
	if(dwIcon>=0){
		m_OptionsList.SetIItemControlType(hThis, 0, FLC_CHECKBOX);
	}
	m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	m_OptionsList.SetIItemColSpan(hThis, 0, 1);
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	m_OptionsList.SetIItemCheck(hThis,0, (*pBoolVariable)!=0,TRUE, FALSE);
	COption* pOpt=new COptionBOOL(dwDefValue,pBoolVariable, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	if(!hDefaultTopic && CompareToDefTopic(sTitle)){
		hDefaultTopic=hThis;
	}
	return hThis;
}

HIROW CDLG_Options::Option_AddBOOL_Plain(HIROW fParent, const char* szText, long* pBoolVariable, long dwDefValue, long dwIcon, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=AddRow(fParent,sTitle, dwIcon? dwIcon:52);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_CHECKBOX);
	m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	m_OptionsList.SetIItemCheck(hThis,1, (*pBoolVariable)!=0,TRUE, FALSE);
	COption* pOpt=new COptionBOOL(dwDefValue,pBoolVariable, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

HIROW CDLG_Options::Option_AddNString(HIROW fParent, const char* szText, long* pNStrVariable, long dwDefValue, long dwMin, long dwMax, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=(sTitle=="")?fParent:AddRow(fParent,sTitle,57);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_EDIT);
	if(sTitle!=""){
		m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	}else{
		m_OptionsList.SetIItemColSpan(hThis, 0, -1);
	}
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	CString sText=Format("%i",*pNStrVariable);
	m_OptionsList.SetIItemText(hThis, 1, sText);
	COption* pOpt=new COptionNumber(dwDefValue,pNStrVariable, dwMin, dwMax, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

HIROW CDLG_Options::Option_AddNSlider(HIROW fParent, const char* szText, long* pNStrVariable, long dwDefValue, long dwMin, long dwMax, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=(sTitle=="")?fParent:AddRow(fParent,sTitle,57);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_SLIDER);
	if(sTitle!=""){
		m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	}else{
		m_OptionsList.SetIItemColSpan(hThis, 0, -1);
	}
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	CString sText=Format("%lu",*pNStrVariable);
	m_OptionsList.SetIItemText(hThis, 1, sText);
	COption* pOpt=new COptionNumber(dwDefValue,pNStrVariable, dwMin, dwMax, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

typedef int (WINAPI *_WKPluginShowEvent)(char szEvent[128],HWND pParent);
typedef int (WINAPI *_WKPluginIsEventPresent)(char szEvent[128]);
BOOL IsEventReminded(const char* szId)
{
	HINSTANCE h=CheckPlugin("wp_schedule",1);
	if(h){
		_WKPluginIsEventPresent fp=(_WKPluginIsEventPresent)GetProcAddress(h,"WKPluginIsEventPresent");
		if(fp){
			char szEvent[128]={0};
			strcpy(szEvent,szId);
			return (*fp)(szEvent);
		}
	}
	return 0;
}

#ifdef _WIREKEYS
typedef int (WINAPI *_WKIHOTKEYS_GetHotkeyDscByID)(char const* szEventID,char* szOut,int iOut);
typedef int (WINAPI *_AddTypingAlias)(char const* szEventID,int iType);
BOOL CALLBACK AddModifyTAlias(LPVOID pData,CDLG_Options* pDialog)
{
	CString sId=pDialog->aCommonStrings[(int)pData];
	HINSTANCE h=CheckPlugin("wp_autotext",1);
	if(h && sId!=""){
		CString sActionTitle=_l("Start item");
		HINSTANCE hLLHookInst=CheckPlugin("WP_KeyMaster");
		_WKIHOTKEYS_GetHotkeyDscByID fpHk=(_WKIHOTKEYS_GetHotkeyDscByID)GetProcAddress(hLLHookInst,"WKIHOTKEYS_GetHotkeyDscByID");
		if(fpHk){
			char szTitle[256]={0};
			(*fpHk)(sId,szTitle,sizeof(szTitle));
			if(szTitle[0]!=0){
				sActionTitle+=" '";
				sActionTitle+=szTitle;
				sActionTitle+="'";
			}
		}
		sActionTitle+="\r\n---\r\n";
		sActionTitle+=_l("Item identifier (leave intact)");
		sActionTitle+=":\r\n[ID=";
		sActionTitle+=sId;
		sActionTitle+="]";
		_AddTypingAlias fpTA=(_AddTypingAlias)GetProcAddress(h,"AddTypingAlias");
		if(fpTA){
			(*fpTA)(sActionTitle,1);
		}
	}
	return 0;
}

BOOL CALLBACK AddModifyReminder(LPVOID pData,CDLG_Options* pDialog)
{
	CString sId=pDialog->aCommonStrings[(int)pData];
	HINSTANCE h=CheckPlugin("wp_schedule",1);
	if(h && sId!=""){
		CString sTitle=_l("Do you want to\nschedule this action");
		{
			HINSTANCE hLLHookInst=CheckPlugin("WP_KeyMaster");
			_WKIHOTKEYS_GetHotkeyDscByID fpHk=(_WKIHOTKEYS_GetHotkeyDscByID)GetProcAddress(hLLHookInst,"WKIHOTKEYS_GetHotkeyDscByID");
			if(fpHk){
				char szTitle[256]={0};
				(*fpHk)(sId,szTitle,sizeof(szTitle));
				if(szTitle[0]!=0){
					sTitle+="\r\n'";
					sTitle+=szTitle;
					sTitle+="'";
				}
			}
		}
		sTitle+="?";
		if(!IsEventReminded(sId)){
			if(MessageBox(pDialog->GetSafeHwnd(),sTitle,_l("Scheduler"),MB_YESNO|MB_ICONQUESTION|MB_APPLMODAL)!=IDYES){
				return 0;
			}
		}
		_WKPluginShowEvent fp=(_WKPluginShowEvent)GetProcAddress(h,"WKPluginShowEvent");
		if(fp){
			char szEvent[128]={0};
			strcpy(szEvent,sId);
			(*fp)(szEvent,pDialog->GetSafeHwnd());
		}
	}
	return 0;
}

BOOL CALLBACK AddModifyDLink(LPVOID pData,CDLG_Options* pDialog)
{
	CString sTitle=_l("Action");
	CString sId=pDialog->aCommonStrings[(int)pData];
	HINSTANCE hLLHookInst=CheckPlugin("WP_KeyMaster");
	_WKIHOTKEYS_GetHotkeyDscByID fpHk=(_WKIHOTKEYS_GetHotkeyDscByID)GetProcAddress(hLLHookInst,"WKIHOTKEYS_GetHotkeyDscByID");
	if(fpHk){
		char szTitle[256]={0};
		(*fpHk)(sId,szTitle,sizeof(szTitle));
		if(szTitle[0]!=0){
			sTitle=szTitle;
		}
	}
	CString sExe=GetApplicationDir();
	sExe+=GetApplicationName();
	sExe+=".exe";
	CString sTitleSafe=sTitle;
	MakeSafeFileName(sTitleSafe,sSafeChar.GetLength()?sSafeChar[0]:0);
	CString sTargetFolder=Format("%s\\Link to %s.lnk",getDesktopPath(),sTitleSafe);
	CString sParameter="-execute=";
	sParameter+=sId;
	CString sLinkDsc=Format("%s %s",_l("Shortcut to"),sTitle);
	BOOL bRes=(CreateLink(sExe, sTargetFolder, sLinkDsc,sParameter)==S_OK);
	if(bRes){
		AfxMessageBox(sLinkDsc+": "+_l("created successfully"));
	}else{
		AfxMessageBox(_l("Error: failed to create")+" "+sLinkDsc);
	}
	return TRUE;
}
#endif

HIROW CDLG_Options::Option_AddHotKey(HIROW fParent, const char* szText, CIHotkey* pHotKeyVariable, CIHotkey dwDefValue, long iOptionsClass, int iIcon)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=AddRow(fParent,sTitle,iIcon==-1?55:iIcon);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_HOTKEY);
	m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	m_OptionsList.SetIItemText(hThis, 1, getHotKeyStr(*pHotKeyVariable,TRUE));
	COption* pOpt=new COptionHotKey(dwDefValue, pHotKeyVariable, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
#ifdef _WIREKEYS
	if(strlen(pHotKeyVariable->GetActionId())>0){// reminder
		CString sHKTitle="";
		HINSTANCE hLLHookInst=CheckPlugin("WP_KeyMaster");
		if(hLLHookInst && pHotKeyVariable->sExcludedInApps && pHotKeyVariable->sExclusiveInApps){
			Option_AddString(hThis, _l("Disable in this applications")+"\t"+_l("You can use mask with '*' char and ';' as delimiter, example: 'explorer*;*pad*'"), (pHotKeyVariable->sExcludedInApps), *(pHotKeyVariable->sExcludedInApps), iOptionsClass);
			Option_AddString(hThis, _l("Activate only in this applications")+"\t"+_l("You can use mask with '*' char and ';' as delimiter, example: 'explorer*;*pad*'"), (pHotKeyVariable->sExclusiveInApps), *(pHotKeyVariable->sExcludedInApps), iOptionsClass);
		}
		_WKIHOTKEYS_GetHotkeyDscByID fpHk=(_WKIHOTKEYS_GetHotkeyDscByID)GetProcAddress(hLLHookInst,"WKIHOTKEYS_GetHotkeyDscByID");
		if(fpHk){
			char szTitle[256]={0};
			(*fpHk)(pHotKeyVariable->GetActionId(),szTitle,sizeof(szTitle));
			sHKTitle=szTitle;
		}
		//HINSTANCE h=CheckPlugin("wp_schedule");
		//if(h)
		{
			CString sNameA=_l("Add/Modify schedule for");
			sNameA+=" '";
			sNameA+=sHKTitle;
			sNameA+="'";
/*
#ifdef _DEBUG
			sNameA+=" (";
			sNameA+=pHotKeyVariable->GetActionId();
			sNameA+=")";
#endif
*/
			Option_AddAction2(hThis,sNameA, AddModifyReminder, (LPVOID)aCommonStrings.Add(pHotKeyVariable->GetActionId()), iOptionsClass, lSipleRemindIconNum);
		}

		//HINSTANCE h2=CheckPlugin("wp_autotext");
		//if(h2)
		{
			CString sNameA=_l("Add/Modify typing alias for");
			sNameA+=" '";
			sNameA+=sHKTitle;
			sNameA+="'";
			Option_AddAction2(hThis,sNameA, AddModifyTAlias, (LPVOID)aCommonStrings.Add(pHotKeyVariable->GetActionId()), iOptionsClass);
		}
		CString sNameD=_l("Add shortcut to desktop");
		Option_AddAction2(hThis, sNameD, AddModifyDLink, (LPVOID)aCommonStrings.Add(pHotKeyVariable->GetActionId()), iOptionsClass);
	}
#endif
	return hThis;
}

HIROW CDLG_Options::Option_AddString(HIROW fParent, const char* szText, CString* pStrVariable, const char* szDefValue, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	CString sDefValue=szDefValue;
	HIROW hThis=(sTitle=="")?fParent:AddRow(fParent,sTitle,56);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_EDIT);
	if(sTitle!=""){
		m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	}else{
		m_OptionsList.SetIItemColSpan(hThis, 0, -1);
	}
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	m_OptionsList.SetIItemText(hThis, 1, *pStrVariable);
	COption* pOpt=new COptionString(sDefValue,pStrVariable, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	if(!hDefaultTopic && CompareToDefTopic(sTitle)){
		hDefaultTopic=hThis;
	}
	return hThis;
}

HIROW CDLG_Options::Option_AddPassword(HIROW fParent, const char* szText, CString* pStrVariable, const char* szDefValue, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	CString sDefValue=szDefValue;
	HIROW hThis=(sTitle=="")?fParent:AddRow(fParent,sTitle,56);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_PASSWORD);
	if(sTitle!=""){
		m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	}else{
		m_OptionsList.SetIItemColSpan(hThis, 0, -1);
	}
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	m_OptionsList.SetIItemText(hThis, 1, *pStrVariable);
	COption* pOpt=new COptionString(sDefValue,pStrVariable, iOptionsClass, TRUE);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

HIROW CDLG_Options::Option_AddActionToEdit(HIROW fParent, FP_OPTIONACTION fp, long iOptionsClass)
{
	return Option_AddActionToEdit(fParent, fp, fParent, iOptionsClass);
}

HIROW CDLG_Options::Option_AddActionToEdit(HIROW fParent, FP_OPTIONACTION fp, HIROW hActionData, long iOptionsClass, int iNewIcon)
{
	HIROW hThis=fParent;//AddRow(fParent,sTitle,54);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_EDITWITHCUSTOM);
	m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	if(iNewIcon!=-1){
		m_OptionsList.SetIItemImage(hThis, 0, iNewIcon);
	}
	//m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);// Оставляем от текстового поля
	//m_OptionsList.SetIItemText(hThis, 1, szButtonText);
	COption* pOpt=new COptionAction(fp,0,hActionData,iOptionsClass);
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

HIROW CDLG_Options::Option_AddAction(HIROW fParent, const char* szButtonText, FP_OPTIONACTION fp, HIROW hActionData,  long iOptionsClass, int iIcon)
{
	return Option_AddActionEx(fParent, szButtonText, fp, 0, hActionData, iIcon==-1?54:iIcon, iOptionsClass);
}

HIROW CDLG_Options::Option_AddAction2(HIROW fParent, const char* szButtonText, FP_OPTIONACTION2 fp, LPVOID pActionData,  long iOptionsClass, int iIcon)
{
	return Option_AddActionEx(fParent, szButtonText, 0, fp, pActionData, (iIcon==-1)?54:iIcon, iOptionsClass);
}

HIROW CDLG_Options::Option_AddActionEx(HIROW fParent, const char* szText, FP_OPTIONACTION fp, FP_OPTIONACTION2 fp2, LPVOID pActionData,  int iIcon, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=AddRow(fParent,sTitle,iIcon);
	m_OptionsList.SetIItemControlType(hThis, 0, FLC_CUSTOMDIALOG2);//1
	m_OptionsList.SetIItemColSpan(hThis, 0, 1);
	m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	m_OptionsList.SetIItemText(hThis, 0, sTitle);//1
	m_OptionsList.SetIItemColors(hThis, 0, RGB(0,0,255), -1);
	m_OptionsList.SetIRowTextStyle(hThis, FL_UNLINE);
	COption* pOpt=new COptionAction(fp,fp2,pActionData,iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	if(iIcon!=lSipleActionIconNum){
		if(GetAliasHIROW(fParent)==0){
			fParent=m_OptionsList.GetParentIRow(fParent);
		}
		Option_AddMenuActionEx(szText, fp, fp2, pActionData,fParent,1,iIcon);
	}
	return hThis;
}

int CDLG_Options::Option_AddMenuActionEx(const char* szText, FP_OPTIONACTION fp, FP_OPTIONACTION2 fp2, LPVOID pActionData, HIROW hTargetItem, BOOL bPlaceAtTop, long lIcon)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	COptionAction* pOpt=new COptionAction(fp,fp2,pActionData,0);
	pOpt->hIRow=hTargetItem;
	pOpt->sDsc=sTitle;
	pOpt->lIcon=lIcon;
	if(bPlaceAtTop){
		aMenuOptions.InsertAt(0,pOpt);
	}else{
		aMenuOptions.SetAtGrow(aMenuOptions.GetSize(),pOpt);
	}
	return aMenuOptions.GetSize();
}

int CDLG_Options::Option_AddMenuAction(const char* szText, FP_OPTIONACTION fp, HIROW hActionData, HIROW hTargetItem, BOOL bPlaceAtTop, long lIcon)
{
	return Option_AddMenuActionEx(szText, fp, 0, hActionData, hTargetItem, bPlaceAtTop, lIcon);
}

HIROW CDLG_Options::Option_AddColor(HIROW fParent, const char* szText, long* pNStrVariable, long dwDefValue, long iOptionsClass, int iNewIcon)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=(sTitle=="")?fParent:AddRow(fParent,sTitle,(iNewIcon==-1)?54:iNewIcon);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_CUSTOMDIALOG);
	if(sTitle!=""){
		m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	}else{
		m_OptionsList.SetIItemColSpan(hThis, 0, -1);
	}
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	DWORD dwColor=*pNStrVariable;
	if(bShowColorForColorOptions){
		m_OptionsList.SetIItemText(hThis, 1, Format("#%02X%02X%02X",GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor)));
	}else{
		m_OptionsList.SetIItemText(hThis, 1, "");
	}	
	m_OptionsList.SetIItemColors(hThis,1,SPECINVERTCOL(dwColor),dwColor);
	m_OptionsList.SetIRowTextStyle(hThis,FL_SELBORD);
	COption* pOpt=new COptionColor(dwDefValue,pNStrVariable, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

HIROW CDLG_Options::Option_AddFont(HIROW fParent, const char* szText, CFont** pFontVariable, long iOptionsClass, int iNewIcon)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=AddRow(fParent,sTitle,(iNewIcon==-1)?54:iNewIcon);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_CUSTOMDIALOG);
	m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	m_OptionsList.SetIItemText(hThis, 1, GetFontTextDsc(*pFontVariable));
	COption* pOpt=new COptionFont(pFontVariable, iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

HIROW CDLG_Options::Option_AddCombo(HIROW fParent, const char* szText, long* pIntVariable, long iDefValue, const char* szComboItems, long iOptionsClass)
{
	CString sTitle,sDsc;
	sDsc=GetOptionStrings(szText,sTitle,sDsc);
	HIROW hThis=(sTitle=="")?fParent:AddRow(fParent,sTitle,58);
	m_OptionsList.SetIItemControlType(hThis, 1, FLC_COMBOBOX);
	if(sTitle!=""){
		m_OptionsList.SetIItemData(hThis, 0, dwOptionCount);
	}else{
		m_OptionsList.SetIItemColSpan(hThis, 0, -1);
	}
	m_OptionsList.SetIItemData(hThis, 1, dwOptionCount);
	CStringArray aComboItems;
	ConvertComboDataToArray(szComboItems,aComboItems);
	long iSelected=*pIntVariable;
	if(iDefValue<0 || iDefValue>=aComboItems.GetSize()){
		iDefValue=0;
	}
	if(iSelected<0 || iSelected>=aComboItems.GetSize()){
		iSelected=iDefValue;
		*pIntVariable=iSelected;
	}
	m_OptionsList.SetIItemText(hThis, 1, aComboItems[iSelected]);
	COption* pOpt=new COptionCombo(iDefValue,pIntVariable,szComboItems,iOptionsClass);
	pOpt->sDsc=sDsc;
	pOpt->hIRow=hThis;
	aOptions.SetAtGrow(dwOptionCount,pOpt);
	dwOptionCount++;
	return hThis;
}

void CDLG_Options::Option_SetRowColor(HIROW fRow, int iItem, int iColType)
{
	DWORD crText=m_OptionsList.m_crIItemText,crBackground=m_OptionsList.m_crIRowBackground;
	DWORD crRealBgColor=crBackground;
	double dRate=double(iColType)/100;
	crRealBgColor=RGB(GetRValue(crBackground)*dRate,GetGValue(crBackground)*dRate,GetBValue(crBackground)*dRate);
	m_OptionsList.SetIItemColors(fRow,iItem,-1,crRealBgColor);
	return;
}

void CDLG_Options::OnOK() 
{
	MyEndDialog(IDOK);
}

void CDLG_Options::CancelThis() 
{
	m_ChangedClasses.RemoveAll();
	MyEndDialog(IDCANCEL);
}

void CDLG_Options::OnHelp()
{
	ShowHelp(sHelpPage==""?"Preferences":sHelpPage);
}

void CDLG_Options::ResetOption(COption* pOpt)
{
	if(!pOpt){
		return;
	}
	switch(pOpt->iOptionType){
	case OPTTYPE_DATE:
		if(((COptionDate*)pOpt)->m_Value!=((COptionDate*)pOpt)->m_Value_Def){
			((COptionDate*)pOpt)->m_Value=((COptionDate*)pOpt)->m_Value_Def;
			OnChanged(pOpt);
			m_OptionsList.SetIItemText(pOpt->hIRow, 1, ConvertToLocaleDate(((COptionDate*)pOpt)->m_Value));
		}
		break;
	case OPTTYPE_BOOL:
		if(((COptionBOOL*)pOpt)->m_Value!=((COptionBOOL*)pOpt)->m_Value_Def){
			((COptionBOOL*)pOpt)->m_Value=((COptionBOOL*)pOpt)->m_Value_Def;
			OnChanged(pOpt);
			m_OptionsList.SetIItemCheck(pOpt->hIRow, 0, (((COptionBOOL*)pOpt)->m_Value)!=0,TRUE, FALSE);
		}
		break;
	case OPTTYPE_NUMBER:
		if(((COptionNumber*)pOpt)->m_Value!=((COptionNumber*)pOpt)->m_Value_Def){
			((COptionNumber*)pOpt)->m_Value=((COptionNumber*)pOpt)->m_Value_Def;
			OnChanged(pOpt);
			m_OptionsList.SetIItemText(pOpt->hIRow, 1, Format("%lu",((COptionNumber*)pOpt)->m_Value));
		}
		break;
	case OPTTYPE_STRING:
		if(((COptionString*)pOpt)->m_Value!=((COptionString*)pOpt)->m_Value_Def){
			((COptionString*)pOpt)->m_Value=((COptionString*)pOpt)->m_Value_Def;
			OnChanged(pOpt);
			m_OptionsList.SetIItemText(pOpt->hIRow, 1, ((COptionString*)pOpt)->m_Value);
		}
		break;
	case OPTTYPE_COMBO:
		{
			if(((COptionCombo*)pOpt)->m_Value!=((COptionCombo*)pOpt)->m_Value_Def){
				((COptionCombo*)pOpt)->m_Value=((COptionCombo*)pOpt)->m_Value_Def;
				OnChanged(pOpt);
				CStringArray aComboItems;
				ConvertComboDataToArray(((COptionCombo*)pOpt)->m_ComboData,aComboItems);
				m_OptionsList.SetIItemText(pOpt->hIRow, 1, aComboItems[((COptionCombo*)pOpt)->m_Value]);
			}
		}
		break;
	case OPTTYPE_HOTKEY:
		if(((COptionHotKey*)pOpt)->m_Value!=((COptionHotKey*)pOpt)->m_Value_Def){
			((COptionHotKey*)pOpt)->m_Value=((COptionHotKey*)pOpt)->m_Value_Def;
			OnChanged(pOpt);
			m_OptionsList.SetIItemText(pOpt->hIRow, 1, getHotKeyStr(((COptionHotKey*)pOpt)->m_Value,TRUE));
		}
		break;
	case OPTTYPE_COLOR:
		if(((COptionColor*)pOpt)->m_Value!=((COptionColor*)pOpt)->m_Value_Def){
			((COptionColor*)pOpt)->m_Value=((COptionColor*)pOpt)->m_Value_Def;
			OnChanged(pOpt);
			DWORD dwColor=((COptionColor*)pOpt)->m_Value;
			m_OptionsList.SetIItemColors(pOpt->hIRow,1,SPECINVERTCOL(dwColor),dwColor);
			m_OptionsList.SetIRowTextStyle(pOpt->hIRow,FL_SELBORD);
			if(bShowColorForColorOptions){
				m_OptionsList.SetIItemText(pOpt->hIRow, 1, Format("#%02X%02X%02X",GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor)));
			}
		}
		break;
	}
	m_OptionsList.UpdateIRow(pOpt->hIRow);
}

void CDLG_Options::OnReset()
{
	if(AfxMessageBox(_l("Warning: *ALL* settings will be reverted to default values!\nThis operation can`t be undone. Continue")+"?",MB_YESNO|MB_ICONQUESTION)!=IDYES){
		return;
	}
	int i=0;
	SetWindowText(m_pStartupInfo->szWindowTitle);
	for(i=0;i<aOptions.GetSize();i++){
		COption* pOpt=aOptions[i];
		ResetOption(pOpt);
	}
	return;	
}

void CDLG_Options::CommitData(int iOptionClass)
{
	if(bManualPreventCommit){
		return;
	}
	int i=0;
	SetWindowText(m_pStartupInfo->szWindowTitle);
	for(i=0;i<aOptions.GetSize();i++){
		COption* pOpt=aOptions[i];
		if(!pOpt){
			continue;
		}
		if(iOptionClass!=-1 && pOpt->iOptionClass!=iOptionClass){
			continue;
		}
		switch(pOpt->iOptionType){
		case OPTTYPE_DATE:
			*((COptionDate*)pOpt)->m_Value_Real=((COptionDate*)pOpt)->m_Value;
			break;
		case OPTTYPE_BOOL:
			*((COptionBOOL*)pOpt)->m_Value_Real=((COptionBOOL*)pOpt)->m_Value;
			break;
		case OPTTYPE_NUMBER:
			*((COptionNumber*)pOpt)->m_Value_Real=((COptionNumber*)pOpt)->m_Value;
			break;
		case OPTTYPE_STRING:
			*((COptionString*)pOpt)->m_Value_Real=((COptionString*)pOpt)->m_Value;
			break;
		case OPTTYPE_COMBO:
			*((COptionCombo*)pOpt)->m_Value_Real=((COptionCombo*)pOpt)->m_Value;
			break;
		case OPTTYPE_HOTKEY:
			*((COptionHotKey*)pOpt)->m_Value_Real=((COptionHotKey*)pOpt)->m_Value;
			break;
		case OPTTYPE_COLOR:
			*((COptionColor*)pOpt)->m_Value_Real=((COptionColor*)pOpt)->m_Value;
			break;
		}
	}
	return;
}

void CDLG_Options::PopSomeDataFromScreen(long iOptionClass)
{
	int i=0;
	int iCount=aOptions.GetSize();
	for(i=0;i<iCount;i++){
		COption* pOpt=aOptions[i];
		if(!pOpt){
			continue;
		}
		if(pOpt->iOptionClass!=iOptionClass){
			continue;
		}
		aOptions[i]=NULL;
		if(pOpt->iOptionType==OPTTYPE_HOTKEY){
			bAnyHotkeyWasChanged=1;
		}
		int iListCount=m_OptionsList.GetItemCount();
		HIROW hLeft=GetAliasHIROW(pOpt->hIRow);
		if(hLeft!=NULL){
			m_LList.DeleteIRow(hLeft);
			aFromRtoL.RemoveKey(pOpt->hIRow);
		}
		m_OptionsList.DeleteIRow(pOpt->hIRow);
		pOpt->hIRow=NULL;
		delete pOpt;
	}
	// Чтобы небыло глюков
	// Иногда удаляют дефолтный топик...
	hDefaultTopic=0;
}

BOOL CDLG_Options::PushOptionOnScreen(int iOptionIndex)
{
	COption* pOpt=aOptions[iOptionIndex];
	if(!pOpt){
		return FALSE;
	}
	switch(pOpt->iOptionType){
	case OPTTYPE_DATE:
		((COptionDate*)pOpt)->m_Value=*((COptionDate*)pOpt)->m_Value_Real;
		m_OptionsList.SetIItemText(pOpt->hIRow, 1, ConvertToLocaleDate(((COptionDate*)pOpt)->m_Value));
		break;
	case OPTTYPE_BOOL:
		((COptionBOOL*)pOpt)->m_Value=*((COptionBOOL*)pOpt)->m_Value_Real;
		m_OptionsList.SetIItemCheck(pOpt->hIRow, 0, ((COptionBOOL*)pOpt)->m_Value);
		break;
	case OPTTYPE_NUMBER:
		((COptionNumber*)pOpt)->m_Value=*((COptionNumber*)pOpt)->m_Value_Real;
		if(((COptionNumber*)pOpt)->m_Value<((COptionNumber*)pOpt)->m_dwMin){
			((COptionNumber*)pOpt)->m_Value=((COptionNumber*)pOpt)->m_dwMin;
		}
		if(((COptionNumber*)pOpt)->m_Value>((COptionNumber*)pOpt)->m_dwMax){
			((COptionNumber*)pOpt)->m_Value=((COptionNumber*)pOpt)->m_dwMax;
		}
		m_OptionsList.SetIItemText(pOpt->hIRow,1,Format("%i",((COptionNumber*)pOpt)->m_Value));
		break;
	case OPTTYPE_STRING:
		((COptionString*)pOpt)->m_Value=*((COptionString*)pOpt)->m_Value_Real;
		m_OptionsList.SetIItemText(pOpt->hIRow,1,((COptionString*)pOpt)->m_Value);
		break;
	case OPTTYPE_COMBO:
		{
			CStringArray sItemsData;
			((COptionCombo*)pOpt)->m_Value=*((COptionCombo*)pOpt)->m_Value_Real;
			ConvertComboDataToArray(((COptionCombo*)pOpt)->m_ComboData,sItemsData);
			m_OptionsList.SetIItemText(pOpt->hIRow,1,sItemsData[((COptionCombo*)pOpt)->m_Value]);
			break;
		}
	case OPTTYPE_HOTKEY:
		bAnyHotkeyWasChanged=1;
		((COptionHotKey*)pOpt)->m_Value=*((COptionHotKey*)pOpt)->m_Value_Real;
		break;
	case OPTTYPE_COLOR:
		{
			((COptionColor*)pOpt)->m_Value=*((COptionColor*)pOpt)->m_Value_Real;
			DWORD dwColor=((COptionColor*)pOpt)->m_Value;
			m_OptionsList.SetIItemColors(((COptionColor*)pOpt)->hIRow,1,SPECINVERTCOL(dwColor),dwColor);
			m_OptionsList.SetIRowTextStyle(((COptionColor*)pOpt)->hIRow,FL_SELBORD);
			if(bShowColorForColorOptions){
				m_OptionsList.SetIItemText(((COptionColor*)pOpt)->hIRow, 1, Format("#%02X%02X%02X",GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor)));
			}
		}
		break;
	}
	m_OptionsList.UpdateIRow(pOpt->hIRow);
	return TRUE;
}


void CDLG_Options::PushSomeDataOnScreen(long iOptionClass)
{
	int i=0;
	for(i=0;i<aOptions.GetSize();i++){
		COption* pOpt=aOptions[i];
		if(pOpt){
			if(iOptionClass!=-1){
				if(pOpt->iOptionClass!=iOptionClass){
					continue;
				}
			}
			if(!PushOptionOnScreen(i)){
				continue;
			}
		}
	}
}


COption* CDLG_Options::GetOptionUnderCursor(int* iOptionIndex) 
{
	COption* pOpt=NULL;
	CPoint p;
	GetCursorPos(&p);
	m_OptionsList.ScreenToClient(&p);
	int iCol=-1;
	int iRow=m_OptionsList.HitTestEx(p, &iCol);
	if(iRow!=-1){
		HIROW hIRow=m_OptionsList.GetIRowFromIndex(iRow);
		if(hIRow){
			long dwItemData=m_OptionsList.GetIItemData(hIRow,1);
			if(dwItemData>=0 && dwItemData<aOptions.GetSize()){
				pOpt=aOptions[dwItemData];
				if(iOptionIndex){
					*iOptionIndex=dwItemData;
				}
			}
		}
	}
	return pOpt;
}

void CDLG_Options::GetOptionDsc(HIROW hIRow, int iCol, CString& sText, CString& sPopupText)
{
	if(hIRow){
		long dwItemData=m_OptionsList.GetIItemData(hIRow,1);
		BOOL bSubItems=m_OptionsList.GetChildIRowCount(hIRow);
		COption* pOpt=NULL;
		BOOL bNoEditWarn=FALSE;
		if(dwItemData>=0 && dwItemData<aOptions.GetSize()){
			pOpt=aOptions[dwItemData];
			if(pOpt){
				if(iCol==0){
					sText=pOpt->sDsc;
				}else{
					sText=pOpt->GetDsc();
				}
			}else{
				bNoEditWarn=TRUE;
			}
		}else{
			bNoEditWarn=TRUE;
		}
		CString sItemText="";
		if(iCol!=-1){
			sItemText=m_OptionsList.GetIItemText(hIRow,iCol);
		}else{
			sItemText=m_OptionsList.GetIItemText(hIRow,0);
		}
		if(sText==""){
			sText=sItemText;
		}
		if(sText.Find(sItemText)==-1){
			sText=sItemText+"\n"+sText;
		}
		DWORD dwType=0;
		sPopupText=sText;
		if(iCol==0){
			sPopupText=sItemText;
			if(pOpt!=NULL){
				dwType=m_OptionsList.GetIItemControlType(hIRow, 0);
				if(dwType==FLC_CHECKBOX){
					bNoEditWarn=TRUE;
					sPopupText=pOpt->GetDsc();
					if(sPopupText==""){
						sPopupText=sItemText;
					}
					/*
					if(sText!=sItemText && sText!="" && sPopupText!=""){
						sText=sPopupText+"; "+sText;
					}else{
						sText=sPopupText;
					}
					*/
				}
			}
		}
		dwType=m_OptionsList.GetIItemControlType(hIRow, 1);
		if(dwType==FLC_READONLY || dwType==FLC_COMBOBOX){
			bNoEditWarn=TRUE;
		}
		if(!bNoEditWarn){
			if(dwType==FLC_HOTKEY){
				sText+=" (";
				sText+=_l("Hotkey");
				sText+=")";
			}
			/*
			if(iCol==0){
				if(sText!=""){
					sText+="; ";
				}
				sText+=_l("Double-click right column to edit value");
			}else{
				if(sText!=""){
					sText+="; ";
				}
				sText+=_l("Double-click cell to edit value");
			}*/
		}
		if(bSubItems){
			if(sText!=""){
				sText+="; ";
			}
			sText+=_l("This item has additional options, Expand it to see them");
		}
	}
}

int iLastRow=-1;
int iLastCol=-1;
BOOL CDLG_Options::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	if(IsWindow(m_tooltip.GetSafeHwnd())){
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	UINT iCtrlID=0;
	if(pMsg->hwnd!=NULL){
		iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	}
	if(iCtrlID==IDC_INPLACE_HOTKEY){
		if(pMsg->message==WM_KEYDOWN || pMsg->message==WM_KEYUP){
			return FALSE;
		}
	}
	if(bShowFind==2 && (pMsg->message==WM_LBUTTONDOWN || pMsg->message==WM_NCLBUTTONDOWN)){
		CPoint pt;
		GetCursorPos(&pt);
		CRect rt,rtMain;
		GetClientRect(&rtMain);
		ClientToScreen(&rtMain);
		rt.CopyRect(rtMain);
		rt.left=rt.right-GetSystemMetrics(SM_CYMENU);
		rt.bottom=rt.top;
		rt.top=rt.top-GetSystemMetrics(SM_CYMENU);
		if(rt.PtInRect(pt)){
			PostMessage(WM_COMMAND,IDC_BUTTON_FIND,0);
		}
	}
	if(pMsg->message==WM_KEYDOWN){
		if(GetKeyState(VK_CONTROL)<0){
			if(pMsg->wParam=='S'){
				OnApply();
				return TRUE;
			}
			if(pMsg->wParam==VK_RETURN){
				OnOK();
				return TRUE;
			}
		}else{
			if(pMsg->wParam==VK_RETURN && iCtrlID==IDC_EDIT_FIND){
				OnFind();
				return TRUE;
			}
		}
		if(pMsg->wParam==VK_ESCAPE){
			CancelThis();
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN || (iCtrlID!=IDC_INPLACE_EDIT && pMsg->wParam==VK_SPACE)){
			CWnd* pFocus=GetFocus();
			pFocus->SendMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
			return TRUE;
		}
		if(iCtrlID==IDC_OPTIONSLIST){
			HIROW hRow=m_OptionsList.GetSelectedIRow();
			int iItem=m_OptionsList.GetSelectedIItem();
			if(hRow && iItem==0){
				if(pMsg->wParam==VK_SPACE || pMsg->wParam==VK_LEFT || pMsg->wParam==VK_BACK){
					if(m_OptionsList.IsCollapsed(hRow)){
						m_OptionsList.Expand(hRow);
					}else{
						m_OptionsList.Collapse(hRow);
					}
					return TRUE;
				}
			}
			/*if(pMsg->wParam==VK_TAB){
				m_OptionsList.SelectIItem(hRow,1);
				return TRUE;
			}*/
		}
	}
	if(pMsg->message==WM_RBUTTONUP){
		if(iCtrlID==IDC_OPTIONSLIST || iCtrlID==IDC_LPANE){
			CPoint p;
			int iCol=-1;
			GetCursorPos(&p);
			m_LList.ScreenToClient(&p);
			int iRow=m_LList.HitTestEx(p, &iCol);
			if(iRow!=-1){
				HIROW hIRow=m_LList.GetIRowFromIndex(iRow);
				MoveGlobalFocus(hIRow,FALSE);
			}
			OnAdditionalEx(TRUE,(iCtrlID==IDC_LPANE)?1:2);
			return TRUE;
		}
	}
	if(pMsg->message==WM_MOUSEMOVE || pMsg->message==WM_LBUTTONDOWN){
		CString sText;
		if(iCtrlID==IDC_OPTIONSLIST || iCtrlID==IDC_LPANE){
			if(pMsg->message==WM_LBUTTONDOWN){
				int i=0;
			}
			CPoint p;
			int iCol=-1;
			GetCursorPos(&p);
			m_OptionsList.ScreenToClient(&p);
			int iRow=m_OptionsList.HitTestEx(p, &iCol);
			if(iRow!=-1 && (pMsg->message==WM_LBUTTONDOWN || iLastRow!=iRow || iCol!=iLastCol)){
				iLastRow=iRow;
				iLastCol=iCol;
				CString sPopupText;
				HIROW hIRow=m_OptionsList.GetIRowFromIndex(iRow);
				GetOptionDsc(hIRow, iCol, sText, sPopupText);
				m_tooltip.Pop();
				m_tooltip.UpdateTipText((sPopupText.Find("\n")==-1)?TrimMessage(sPopupText,100):TrimMessage(sPopupText,2000),GetDlgItem(IDC_OPTIONSLIST));
			}else{
				GetCursorPos(&p);
				m_LList.ScreenToClient(&p);
				iRow=m_LList.HitTestEx(p, &iCol);
				static int iLastLRow=-1;
				static int iLastLCol=-1;
				if(iRow!=-1 && (pMsg->message==WM_LBUTTONDOWN || iLastLRow!=iRow || iLastLCol!=iCol)){
					iLastLRow=iRow;
					iLastLCol=iCol;
					HIROW hIRow=m_LList.GetIRowFromIndex(iRow);
					sText=m_LList.GetIItemText(hIRow,iCol);
				}
			}
		}else{
			static DWORD iLastStatId=0;
			if(pMsg->message==WM_LBUTTONDOWN || iLastStatId!=iCtrlID){
				iLastStatId=iCtrlID;
				CWnd* pCtrlWnd=GetDlgItem(iCtrlID);
				if(pCtrlWnd){
					//m_tooltip.GetText(sText,pCtrlWnd,0);
					sText=GetTooltipText(&m_tooltip, pCtrlWnd);
				}
			}
		}
		if(sText!="" && pMsg->message==WM_LBUTTONDOWN){
			SetQHelpText(CString(" ")+sText);
		}
	}
	if(pMsg->message==WM_DROPFILES){
		int iOptionIndex=-1;
		COption* pOpt=GetOptionUnderCursor(&iOptionIndex);
		COptionString* pOptStr=NULL;
		if(pOpt && pOpt->iOptionType==OPTTYPE_STRING){
			pOptStr=(COptionString*)pOpt;
		}
		if(pOptStr && iOptionIndex>=0){
			HDROP hDropInfo=(HDROP)pMsg->wParam;
			char szFilePath[256];
			UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
			for (UINT u = 0; u < cFiles; u++){
				DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
				*pOptStr->m_Value_Real=szFilePath;
				break;
			}
			DragFinish(hDropInfo);
			PushOptionOnScreen(iOptionIndex);
		}
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONDBLCLK || (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)){
		if(iCtrlID==IDC_OPTIONSLIST){
			CPoint p;
			int iCol=-1;
			GetCursorPos(&p);
			m_OptionsList.ScreenToClient(&p);
			int iRow=m_OptionsList.HitTestEx(p, &iCol);
			if(iCol==0){
				int iOptionIndex=-1;
				COption* pOpt=GetOptionUnderCursor(&iOptionIndex);
				if(pOpt && m_OptionsList.GetIItemControlType(pOpt->hIRow,1)!=FLC_READONLY && m_OptionsList.GetIItemControlType(pOpt->hIRow,0)==FLC_READONLY){
					PostMessage(EDITITEM,(WPARAM)pOpt->hIRow,1);
					return TRUE;
				}
			}
		}
	}
	if(pMsg->message==EDITITEM){
		m_OptionsList.SelectAndEditIItem((HIROW)pMsg->wParam,pMsg->lParam);
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONDOWN){
		if(iCtrlID==IDC_LPANE){
			BOOL bExpand=FALSE;
			HIROW hIRow=NULL;
			CPoint p;
			int iCol=-1;
			GetCursorPos(&p);
			m_LList.ScreenToClient(&p);
			int iRow=m_LList.HitTestEx(p, &iCol);
			if(iRow!=-1){
				hIRow=m_LList.GetIRowFromIndex(iRow);
				if(hLastOpenedRPane!=hIRow){
					hLastOpenedRPane=hIRow;
					bExpand=!m_LList.HitTestCollapse(hIRow,p);
					MoveGlobalFocus(hIRow,bExpand);
				}
			}
			if(bExpand){
				m_LList.SelectIRow(hIRow,TRUE);
				return TRUE;
			}
		}
	}
	BOOL lRes=CDialog::PreTranslateMessage(pMsg);
	if(lRes){
		if(pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_UP || pMsg->wParam==VK_DOWN)){
			if(iCtrlID==IDC_LPANE){
				HIROW hIRow=m_LList.GetSelectedIRow();
				if(hLastOpenedRPane!=hIRow){
					hLastOpenedRPane=hIRow;
					MoveGlobalFocus(hIRow,TRUE,TRUE);
				}
			}
			CString sT,sP;
			GetOptionDsc(m_OptionsList.GetSelectedIRow(),m_OptionsList.GetSelectedIItem(),sT,sP);
			if(sT=="" && sP==""){
				HIROW hLeftSelected=m_LList.GetSelectedIRow();
				if(hLeftSelected){
					sT=m_LList.GetIItemText(hLeftSelected,0);
				}
			}
			SetQHelpText(CString(" ")+sT==""?sP:sT);
		}
	}
	UINT message=pMsg->message;
	if(bShowFind==2 && (message==WM_MOUSEMOVE || message==WM_ACTIVATE ||message==WM_NCACTIVATE || message==WM_MOVING || message==WM_SIZING ||message==WM_SIZE || message==WM_NCPAINT || message==WM_PAINT || message==WM_SHOWWINDOW)){
		DrawNC();
	}
	return lRes;
}

BOOL CDLG_Options::DrawNC()
{
	HWND hwnd=this->GetSafeHwnd();
	if(!hwnd || !IsWindow(hwnd) || bShowFind!=2){
		return 0;
	}
	HDC hdc=::GetWindowDC(hwnd);
	CRect rt,rtMain,rtMainWnd;
	GetClientRect(&rtMain);
	GetWindowRect(&rtMainWnd);
	ClientToScreen(&rtMain);
	rt.CopyRect(rtMain);
	rt.left=rt.right-GetSystemMetrics(SM_CYMENU);
	rt.bottom=rt.top;
	rt.top=rt.top-GetSystemMetrics(SM_CYMENU);
	rt.OffsetRect(-rtMainWnd.left,-rtMainWnd.top);
	//DrawFrameControl(hdc,&rt,DFC_CAPTION,DFCS_CAPTIONHELP);
	::SetBkMode(hdc, TRANSPARENT);
	::SelectObject(hdc,HFONT(*GetFont()));
	::DrawText(hdc,"[?]",3,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	::ReleaseDC(hwnd, hdc);
	return 0;
}

BOOL CDLG_Options::CallApply(DWORD dwRes, BOOL bSaveAndExit)
{
	if(m_pStartupInfo->fpApplyOptionList){
		EnableWindow(FALSE);
		if(dwRes==IDOK){
			CommitData();
		}
		HIROW hPrevTopic=m_LList.GetSelectedIRow();
		if(hPrevTopic && !bSaveAndExit){
			m_pStartupInfo->szDefaultTopicTitle=m_LList.GetIItemText(hPrevTopic,0);
		}
		BOOL bRes=m_pStartupInfo->fpApplyOptionList(bSaveAndExit,dwRes,this);
		if(bSaveAndExit){
			// Можно скрыть уже...
			ShowWindowAsync(GetSafeHwnd(),SW_HIDE);
		}else{
			if(bRes){
				// Перестраиваем меню!
				HCURSOR hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
				m_LList.m_bStopUpdateScreen++;
				m_OptionsList.m_bStopUpdateScreen++;
				DestroyOptions(1);
				IniIList();// Заново иницим иконки
				FillOptionListWithData();
				m_LList.m_bStopUpdateScreen--;
				m_OptionsList.m_bStopUpdateScreen--;
				// Перерисовываем кнопки...
				SetButtons();
				// Усе...
				Invalidate();
				SetCursor(hCur);
				//pParentWnd???
			}
		}
		EnableWindow(TRUE);
	}
	return TRUE;
}

void CDLG_Options::OnApply()
{
	CallApply(IDOK,FALSE);
	GetDlgItem(IDAPPLY)->EnableWindow(FALSE);
	m_ChangedClasses.RemoveAll();
}

BOOL CDLG_Options::MyEndDialog(DWORD dwRes)
{
	CallApply(dwRes,TRUE);
	CMenu* pSysMenu=GetMenu();
	if(pSysMenu){
		delete pSysMenu;
	}
	EndDialog(dwRes);
	return TRUE;
}

BOOL CDLG_Options::FinishFillOptionListWithData()
{
	Sizer.UpdateClientOffsets();
	if(bRPaneOnly){
		CRect rect,rect2;
		m_LList.GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_OptionsList.GetWindowRect(&rect2);
		ScreenToClient(&rect2);
		rect2.left-=rect.Width();
		rect.right=rect.left;
		m_LList.MoveWindow(&rect);
		m_LList.ShowWindow(SW_HIDE);
		m_OptionsList.MoveWindow(&rect2);
		Sizer.ApplyLayout(TRUE);
	}
	return TRUE;
}

BOOL CDLG_Options::FillOptionListWithData()
{
	InitExtraData();
	if(m_pStartupInfo->sz1stTopic.GetLength()){
		hTopLPaneItem=m_LList.InsertIRow(FL_ROOT,FL_FIRST,m_pStartupInfo->sz1stTopic,5);
		m_LList.SetIRowTextStyle(hTopLPaneItem,FL_SKIPTRI);
		m_LList.SetIItemData(hTopLPaneItem,0,0);
	}else{
		hTopLPaneItem=FL_ROOT;
	}
	m_OptionsList.SetSilentAddMode(TRUE);
	if(!(*m_pStartupInfo->fpInitOptionList)(this)){
		MyEndDialog(IDCANCEL);
		return FALSE;
	}
	m_OptionsList.SetSilentAddMode(FALSE);
	if(!hDefaultTopic){
		int iPos=0;
		HIROW h=m_LList.GetIRowFromIndex(iPos);
		while(m_LList.GetIItemData(h,0)==0){
			h=m_LList.GetIRowFromIndex(iPos);
			iPos++;
			if(iPos>=m_LList.GetIRowCount()){
				iPos--;
				break;
			}
		}
		hDefaultTopic=m_LList.GetIRowFromIndex(iPos);
	}else{
		HIROW hRight=hDefaultTopic;
		hDefaultTopic=GetAliasHIROW(hDefaultTopic);
		while(hDefaultTopic==0 && hDefaultTopic!=FL_ROOT && hRight!=0 && hRight!=FL_ROOT){
			hRight=m_OptionsList.GetParentIRow(hRight);
			aFromRtoL.Lookup(hRight,hDefaultTopic);
		}
		m_LList.SetIRowTextStyle(hDefaultTopic,FL_BOLD);
	}
	CArray<HIROW,HIROW> aTree;
	if(m_pStartupInfo->bExpandByDef){
		if(m_pStartupInfo->bExpandByDef==2){
			EnableWindow(FALSE);
			ProcessEveryFolder(FL_ROOT,FALSE,1);
			EnableWindow(TRUE);
		}else{
			OnExpandAll();
		}
	}else{
		// Только дефолтный
		aTree.Add(hDefaultTopic);
		HIROW hIParent=m_LList.GetParentIRow(hDefaultTopic);
		while(hIParent!=FL_ROOT){
			aTree.Add(hIParent);
			hIParent=m_LList.GetParentIRow(hIParent);
		}
		for(int i=aTree.GetSize()-1;i>=0;i--){
			m_LList.Expand(aTree[i]);
		}
	}
	m_LList.EnsureVisible(hDefaultTopic,0,FALSE);
	MoveGlobalFocus(hDefaultTopic);
	if(m_pStartupInfo->fpPostAction){
		m_pStartupInfo->fpPostAction((HIROW)m_pStartupInfo->pPostActionParam,this);
	}
	if(aFromRtoL.GetCount()<=1){
		bDoNotSavePanePos=1;
		bRPaneOnly=1;
	}
	return TRUE;
}

BOOL CDLG_Options::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) lParam;
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CDLG_Options::AddChanged(int iClass)
{
	m_ChangedClasses.Add(iClass);
	SetWindowText(m_pStartupInfo->szWindowTitle+" *");
	GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
	return TRUE;
}

BOOL CDLG_Options::OnChanged(COption* pOpt)
{
	if(!isOptionClassChanged(pOpt->iOptionClass)){
		return AddChanged(pOpt->iOptionClass);
	}
	return FALSE;
}

void CDLG_Options::ProcessEveryFolder(HIROW hRoot, BOOL bCollapse, BOOL bParLen)
{
	static long lDeep=0;
	if(bParLen>0 && lDeep>=bParLen){
		return;
	}
	int iFolders=m_LList.GetChildIRowCount(hRoot);
	for(int i=0;i<iFolders;i++){
		HIROW hRow=m_LList.GetChildIRowAt(hRoot,i);
		if(hRow){
			if(bCollapse){
				m_LList.Collapse(hRow);
			}else{
				m_LList.Expand(hRow);
			}
			lDeep++;
			ProcessEveryFolder(hRow,bCollapse,bParLen);
			lDeep--;
		}
	}
}

void CDLG_Options::OnResetCurOption()
{
	if(m_OptionsList.GetSelectedIRow()==0){
		return;
	}
	long dwItemData=m_OptionsList.GetIItemData(m_OptionsList.GetSelectedIRow(),1);
	COption* pOpt=NULL;
	if(dwItemData>=0 && dwItemData<aOptions.GetSize()){
		pOpt=aOptions[dwItemData];
		CString sName=m_OptionsList.GetIItemText(m_OptionsList.GetSelectedIRow(),0);
		if(!pOpt || AfxMessageBox(Format("%s: %s?",sName,_l("Reset to default value")),MB_YESNO|MB_ICONQUESTION)!=IDYES){
			return;
		}
		ResetOption(pOpt);
	}
}

void CDLG_Options::OnCollapseAll()
{
	HCURSOR hCur=SetCursor(AfxGetApp()->LoadCursor(IDC_WAIT));
	EnableWindow(FALSE);
	m_LList.LockWindowUpdate();
	ProcessEveryFolder(FL_ROOT,TRUE);
	m_LList.UnlockWindowUpdate();
	EnableWindow(TRUE);
	SetCursor(hCur);
}

void CDLG_Options::OnExpandAll()
{
	EnableWindow(FALSE);
	ProcessEveryFolder(FL_ROOT,FALSE);
	EnableWindow(TRUE);
}

HBRUSH CDLG_Options::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	DWORD dwID=pWnd->GetDlgCtrlID();
	/*if(dwID==IDC_QHELP_TXT){
		pDC->SetBkColor(GetSysColor(COLOR_INFOBK));//RGB(YELLOWR,YELLOWG,YELLOWB)
		pDC->SetTextColor(GetSysColor(COLOR_INFOTEXT));//RGB(0,0,0)
		return brYellow;
	}*/
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

#define ACT_MINNUM	100
void CDLG_Options::OnAdditional()
{
	OnAdditionalEx(FALSE,0);
}

void CDLG_Options::OnAdditionalEx(BOOL bAtCursor,BOOL bType)
{
	CMenu menu;
	menu.CreatePopupMenu();
	if(bAtCursor && hCurButtonsBarIRow){
		int i=1,iPos=-1;
		COptionAction* act=GetMenuAction(hCurButtonsBarIRow,i,0,&iPos);
		if(act && iPos>=0){
			do{
				AddMenuString(&menu,ACT_MINNUM+iPos,act->sDsc);
				i++;
			}while((act=GetMenuAction(hCurButtonsBarIRow,i,0,&iPos))!=0);
			menu.AppendMenu(MF_SEPARATOR, 0, "");
		}
	}
	if(bType==0 || bType==1){
		AddMenuString(&menu,1,_l("Reset to defaults"));//,_bmp().Get(_IL(37)));
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		AddMenuString(&menu,2,_l("Expand all folders"));
		AddMenuString(&menu,3,_l("Collapse all folders"));
	}
	if(bType==0 || bType==2){
		AddMenuString(&menu,4,_l("Reset current option"));
	}
	if(aMenuOptions.GetSize()>0){
		BOOL bSepWas=0;
		for(int i=0;i<aMenuOptions.GetSize();i++){
			COptionAction* act=aMenuOptions[i];
			if(act->hIRow==0){
				if(!bSepWas){
					menu.AppendMenu(MF_SEPARATOR, 0, "");
					bSepWas=1;
				}
				AddMenuString(&menu,ACT_MINNUM+i,act->sDsc);
			}
		}
	}
	// Показываем меню
	CPoint pt;
	if(bAtCursor){
		GetCursorPos(&pt);		
	}else{
		CRect rt;
		GetDlgItem(IDRESET)->GetWindowRect(&rt);
		pt.x=rt.right;
		pt.y=rt.top;
	}
	this->PostMessage(WM_NULL,0,0);
	this->SetForegroundWindow();
	DWORD dwRetCode=TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
	DWORD dwError=GetLastError();
	if(dwRetCode==1){
		OnReset();
	}
	if(dwRetCode==2){
		OnExpandAll();
	}
	if(dwRetCode==3){
		OnCollapseAll();
	}
	if(dwRetCode==4){
		OnResetCurOption();
	}
	if(dwRetCode>=ACT_MINNUM){
		COptionAction* act=aMenuOptions[dwRetCode-ACT_MINNUM];
		FP_OPTIONACTION fp = act->function;
		if(fp){
			(*fp)((HIROW)act->functionParam,this);
		}
	}
	return;
}

BOOL CDLG_Options::MoveGlobalFocus(HIROW hLeftTopic, BOOL bExpand, BOOL bPreventFocusStealing)
{
	HIROW hRPaneToShow=NULL;
	if(!m_LList.CheckIRow(hLeftTopic)){
		return FALSE;
	}
	if(hLeftTopic!=NULL){
		hRPaneToShow=(HIROW)m_LList.GetIItemData(hLeftTopic,0);
		if(hRPaneToShow==NULL){
			return FALSE;
		}
		if(m_LList.GetSelectedIRow()!=hLeftTopic){
			m_LList.SelectIRow(hLeftTopic,TRUE,FALSE);
		}
	}
	if(hRPaneToShow!=NULL){
		int iRCnt = m_OptionsList.GetChildIRowCount(hRPaneToShow);
		if(iRCnt==0 && bAutoSkipEmptyPanes){
			return MoveGlobalFocus(m_LList.GetFirstChildIRow(hLeftTopic), bExpand, bPreventFocusStealing);
		}
		if(bExpand){
			m_LList.EnsureVisible(hLeftTopic,0,TRUE);
			m_LList.Expand(hLeftTopic);
			HIROW hSelIRow=m_LList.GetSelectedIRow();
			m_LList.SelectIRow(hSelIRow, TRUE);
		}
		if(iRCnt==0){
			return TRUE;
		}
	}
	//m_OptionsList.LockWindowUpdate();
	m_OptionsList.ShowWindow(SW_HIDE);
	HIROW hRPane=NULL,hLPane=NULL;
	POSITION pos=NULL;
	//AdjustRPaneW();
	{// Скрываем все
		pos=aFromRtoL.GetStartPosition();
		while(pos){
			aFromRtoL.GetNextAssoc(pos,hRPane,hLPane);
			m_OptionsList.SetUnCollapsible(hRPane,FALSE);
			m_OptionsList.Collapse(hRPane,TRUE);
		}
		m_OptionsList.RefreshIndexes();
	}
	if(m_LList.IsValidHIRow(hDefaultTopic) && m_LList.GetIRowTextStyle(hDefaultTopic)){
		m_LList.SetIRowTextStyle(hDefaultTopic,FL_NORMAL);
	}
	PushButtonsRow(hRPaneToShow);
	m_OptionsList.ShowWindow(SW_SHOW);
	//m_OptionsList.UnlockWindowUpdate();
	//m_LList.Invalidate();
	{// Показываем нужную
		CString sTitle,sTitleDsc;
		pos=aFromRtoL.GetStartPosition();
		while(pos){
			aFromRtoL.GetNextAssoc(pos,hRPane,hLPane);
			if(hRPane==hRPaneToShow){
				m_OptionsList.SelectIRow(NULL,FALSE,FALSE);
				m_OptionsList.Expand(hRPane,TRUE,FALSE);
				m_OptionsList.SetUnCollapsible(hRPane,TRUE);
				sTitle=m_OptionsList.GetIItemText(hRPane,0);
				sTitleDsc=m_OptionsList.GetIItemText(hRPane,1);
			}
		}
		SetQHelpText(sTitleDsc==""?sTitle:sTitleDsc);
		m_OptionsList.RefreshIndexes();
	}
	m_LList.SetFocus();
	sCurrentOptionFromLPane=m_LList.GetIItemText(hLeftTopic,0);
	AdjustRPaneW();
	return TRUE;
}

void CDLG_Options::SetQHelpText(CString szText)
{
	szText.TrimLeft();
	szText.TrimRight();
	if(szText==""){
		szText=m_pStartupInfo->szQHelpByDefault;
	}
	if(m_pStartupInfo->szTooltipPostfix!=""){
		szText+=m_pStartupInfo->szTooltipPostfix;
	}
	szText.TrimLeft();
	szText.TrimRight();
	GetDlgItem(IDC_QHELP_TXT)->SetWindowText(szText);
}

COptionAction* CDLG_Options::GetMenuAction(HIROW hRightPane, int iCount, BOOL bNoIconOnly, int* iOutNumber)
{
	if(iOutNumber){
		*iOutNumber=-1;
	}
	if(hRightPane==0 || hRightPane==FL_ROOT || hRightPane==hLPaneRoot){
		return 0;
	}

	HIROW hParent=NULL;
	if(hRightPane && hRightPane!=FL_ROOT){
		HIROW hLeftTopic=GetAliasHIROW(hRightPane);
		/*if(bNoIconOnly==1 && hLeftTopic==0){
			// Поддерживаем одиночную иерархию, так уж и быть
			hRightPane=m_OptionsList.GetParentIRow(hRightPane);
			hLeftTopic=GetAliasHIROW(hRightPane);
		}*/
		if(hLeftTopic && hLeftTopic!=hLPaneRoot){
			HIROW hLeftTopicParent=m_LList.GetParentIRow(hLeftTopic);
			if(hLeftTopicParent && hLeftTopicParent!=hLPaneRoot){
				hParent=(HIROW)m_LList.GetIItemData(hLeftTopicParent,0);
			}
		}
	}
	if(aMenuOptions.GetSize()>0){
		for(int i=0;i<aMenuOptions.GetSize();i++){
			COptionAction* act=aMenuOptions[i];
			if(act->hIRow==hRightPane || (bNoIconOnly==0 && hParent!=NULL && act->hIRow==hParent)){
				if((bNoIconOnly==1 && act->lIcon!=0) || (bNoIconOnly==0 && act->lIcon==0)){
					iCount--;
					if(iCount==0){
						if(iOutNumber){
							*iOutNumber=i;
						}
						return act;
					}
				}
			}
		}
	}
	return 0;
}

BOOL CDLG_Options::PushButtonsRow(HIROW hRightPane)
{
	// Ищем...
	COptionAction* pAction=0;
	hCurButtonsBarIRow=hRightPane;
	BOOL bAnyButton=0;

	COptionAction* pAction1=GetMenuAction(hRightPane,1,0);
	if(pAction1){
		GetDlgItem(ID_FOLDERACTIONS1)->SetWindowText(pAction1->sDsc);
		GetDlgItem(ID_FOLDERACTIONS1)->ShowWindow(SW_SHOW);
		COptionAction* pAction2=GetMenuAction(hRightPane,2,0);
		if(pAction2){
			GetDlgItem(ID_FOLDERACTIONS2)->ShowWindow(SW_SHOW);
		}else{
			GetDlgItem(ID_FOLDERACTIONS2)->ShowWindow(SW_HIDE);
		}
		bAnyButton++;
	}else{
		GetDlgItem(ID_FOLDERACTIONS1)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_FOLDERACTIONS2)->ShowWindow(SW_HIDE);
	}	
#ifdef OPTIONS_TOOLBAR
	CRect rtFolder;
	BOOL bPreventToolbar=0;
	GetDlgItem(ID_FOLDERACTIONS2)->GetWindowRect(&rtFolder);
	sSqBt=CSize(rtFolder.Width(),rtFolder.Height());	
	pAction=GetMenuAction(hRightPane,1,1);
	if(pAction && lSipleRemindIconNum && pAction->lIcon==lSipleRemindIconNum){
		// Беда... одни реминдеры!
		// Не пойдет!
		bPreventToolbar=1;
	}
	if(pAction && !bPreventToolbar){
		m_Toolbar1.SetStyles(CBS_NORMAL);//CBS_HIQUAL
		m_Toolbar1.SetBitmap(m_OptionsListImages.ExtractIcon(pAction->lIcon));
		SetButtonSize(this,ID_TOOLBAR1,sSqBt);
		CString sDescription=TrimMessage(pAction->GetDsc(),50);
		m_tooltip.UpdateTipText(sDescription, GetDlgItem(ID_TOOLBAR1));
		GetDlgItem(ID_TOOLBAR1)->ShowWindow(SW_SHOW);
		bAnyButton++;
	}else{
		GetDlgItem(ID_TOOLBAR1)->ShowWindow(SW_HIDE);
		SetButtonSize(this,ID_TOOLBAR1,CSize(1,1));
	}
	pAction=GetMenuAction(hRightPane,2,1);
	if(pAction && !bPreventToolbar){
		m_Toolbar2.SetStyles(CBS_NORMAL);//CBS_HIQUAL
		m_Toolbar2.SetBitmap(m_OptionsListImages.ExtractIcon(pAction->lIcon));
		SetButtonSize(this,ID_TOOLBAR2,sSqBt);
		CString sDescription=TrimMessage(pAction->GetDsc(),50);
		m_tooltip.UpdateTipText(sDescription, GetDlgItem(ID_TOOLBAR2));
		GetDlgItem(ID_TOOLBAR2)->ShowWindow(SW_SHOW);
		bAnyButton++;
	}else{
		GetDlgItem(ID_TOOLBAR2)->ShowWindow(SW_HIDE);
		SetButtonSize(this,ID_TOOLBAR2,CSize(1,1));
	}
	pAction=GetMenuAction(hRightPane,3,1);
	if(pAction && !bPreventToolbar){
		m_Toolbar3.SetStyles(CBS_NORMAL);//CBS_HIQUAL
		m_Toolbar3.SetBitmap(m_OptionsListImages.ExtractIcon(pAction->lIcon));
		SetButtonSize(this,ID_TOOLBAR3,sSqBt);
		CString sDescription=TrimMessage(pAction->GetDsc(),50);
		m_tooltip.UpdateTipText(sDescription, GetDlgItem(ID_TOOLBAR3));
		GetDlgItem(ID_TOOLBAR3)->ShowWindow(SW_SHOW);
		bAnyButton++;
	}else{
		GetDlgItem(ID_TOOLBAR3)->ShowWindow(SW_HIDE);
		SetButtonSize(this,ID_TOOLBAR3,CSize(1,1));
	}
	pAction=GetMenuAction(hRightPane,4,1);
	if(pAction && !bPreventToolbar){
		m_Toolbar4.SetStyles(CBS_NORMAL);//CBS_HIQUAL
		m_Toolbar4.SetBitmap(m_OptionsListImages.ExtractIcon(pAction->lIcon));
		SetButtonSize(this,ID_TOOLBAR4,sSqBt);
		CString sDescription=TrimMessage(pAction->GetDsc(),50);
		m_tooltip.UpdateTipText(sDescription, GetDlgItem(ID_TOOLBAR4));
		GetDlgItem(ID_TOOLBAR4)->ShowWindow(SW_SHOW);
		bAnyButton++;
	}else{
		GetDlgItem(ID_TOOLBAR4)->ShowWindow(SW_HIDE);
		SetButtonSize(this,ID_TOOLBAR4,CSize(1,1));
	}
	pAction=GetMenuAction(hRightPane,5,1);
	if(pAction && !bPreventToolbar){
		m_Toolbar5.SetStyles(CBS_NORMAL);//CBS_HIQUAL
		m_Toolbar5.SetBitmap(m_OptionsListImages.ExtractIcon(pAction->lIcon));
		SetButtonSize(this,ID_TOOLBAR5,sSqBt);
		CString sDescription=TrimMessage(pAction->GetDsc(),50);
		m_tooltip.UpdateTipText(sDescription, GetDlgItem(ID_TOOLBAR5));
		GetDlgItem(ID_TOOLBAR5)->ShowWindow(SW_SHOW);
		bAnyButton++;
	}else{
		GetDlgItem(ID_TOOLBAR5)->ShowWindow(SW_HIDE);
		SetButtonSize(this,ID_TOOLBAR5,CSize(1,1));
	}
	pAction=GetMenuAction(hRightPane,6,1);
	if(pAction && !bPreventToolbar){
		m_Toolbar6.SetStyles(CBS_NORMAL);//CBS_HIQUAL
		m_Toolbar6.SetBitmap(m_OptionsListImages.ExtractIcon(pAction->lIcon));
		SetButtonSize(this,ID_TOOLBAR6,sSqBt);
		CString sDescription=TrimMessage(pAction->GetDsc(),50);
		m_tooltip.UpdateTipText(sDescription, GetDlgItem(ID_TOOLBAR6));
		GetDlgItem(ID_TOOLBAR6)->ShowWindow(SW_SHOW);
		bAnyButton++;
	}else{
		GetDlgItem(ID_TOOLBAR6)->ShowWindow(SW_HIDE);
		SetButtonSize(this,ID_TOOLBAR6,CSize(1,1));
	}
#endif
	Sizer.SetGotoRule(MainRulePos,bAnyButton);
	Sizer.ApplyLayout(TRUE);
	return TRUE;
}

void CDLG_Options::ExecuteAction(COptionAction* act)
{
	if(!hCurButtonsBarIRow || !act){
		return;
	}
	FP_OPTIONACTION fp = act->function;
	if(fp){
		(*fp)((HIROW)act->functionParam,this);
	}else{
		FP_OPTIONACTION2 fp2 = act->function2;
		if(fp2){
			(*fp2)(act->functionParam,this);
		}
	}
}

void CDLG_Options::OnAction1()
{
	ExecuteAction(GetMenuAction(hCurButtonsBarIRow,1,0));
}

void CDLG_Options::OnTOOLBAR1()
{
	ExecuteAction(GetMenuAction(hCurButtonsBarIRow,1,1));
}

void CDLG_Options::OnTOOLBAR2()
{
	ExecuteAction(GetMenuAction(hCurButtonsBarIRow,2,1));
}

void CDLG_Options::OnTOOLBAR3()
{
	ExecuteAction(GetMenuAction(hCurButtonsBarIRow,3,1));
}

void CDLG_Options::OnTOOLBAR4()
{
	ExecuteAction(GetMenuAction(hCurButtonsBarIRow,4,1));
}

void CDLG_Options::OnTOOLBAR5()
{
	ExecuteAction(GetMenuAction(hCurButtonsBarIRow,5,1));
}

void CDLG_Options::OnTOOLBAR6()
{
	ExecuteAction(GetMenuAction(hCurButtonsBarIRow,6,1));
}

void CDLG_Options::OnAction2()
{
	if(!hCurButtonsBarIRow){
		return;
	}
	CMenu menu;
	menu.CreatePopupMenu();
	int i=1;
	COptionAction* act=NULL;
	CArray<CBitmap*,CBitmap*> bmpArr;
	while((act=GetMenuAction(hCurButtonsBarIRow,i,0))!=NULL){
		CBitmap* bp=BitmapFromIcon(act->lIcon?m_OptionsListImages.ExtractIcon(act->lIcon):0);
		AddMenuString(&menu,i,act->sDsc,bp);
		bmpArr.Add(bp);
		i++;
	}
	// Показываем меню
	CRect rt;
	GetDlgItem(ID_FOLDERACTIONS2)->GetWindowRect(&rt);
	DWORD dwRetCode=TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, rt.right, rt.top, 0, this->GetSafeHwnd(), NULL);
	for(int i2=0;i2<bmpArr.GetSize();i2++){
		if(bmpArr[i2]){
			delete bmpArr[i2];
		}
	}
	if(dwRetCode>0){
		COptionAction* act=GetMenuAction(hCurButtonsBarIRow,dwRetCode,0);
		FP_OPTIONACTION fp = act->function;
		if(fp){
			(*fp)((HIROW)act->functionParam,this);
		}
	}
}

CString& getOptionsDefaultFind()
{
	static CString sTextOrig;
	return sTextOrig;
}

void CDLG_Options::OnFind()
{
	CString sTextOrig=getOptionsDefaultFind();
	GetDlgItem(IDC_EDIT_FIND)->GetWindowText(sTextOrig);
	getOptionsDefaultFind()=sTextOrig;
	if(bShowFind>=2){
		if(fpExternalSearch){
			if(!(*fpExternalSearch)(_l("Type option name to find it"),sTextOrig,this)){
				return;
			}
		}
	}
	CString sText=UnifyString(sTextOrig);
	if(sText==""){
		AfxMessageBox(_l("Type single word from option title or description first to quickly move to corresponding section"));
		return;
	}
	GetDlgItem(IDC_EDIT_FIND)->EnableWindow(FALSE);
	static CString sPrevSearch="";
	static HIROW hPrevSearch1=0;
	if(sText!=sPrevSearch){
		hPrevSearch1=0;
	}
	BOOL bCanStop=0;
	if(hPrevSearch1==0){
		bCanStop=1;
	}
	BOOL bRes=0;
	sPrevSearch=sText;
	{// Part 1 - LPane
		POSITION pos=m_LList.GetStartPosition();
		while(pos){
			HIROW hLPane=m_LList.GetNextAssoc(pos);
			CString sKey=UnifyString(m_LList.GetIItemText(hLPane,0));
			if(sKey.Find(sText)!=-1){
				if(bCanStop){
					bRes=1;
					hPrevSearch1=hLPane;
					MoveGlobalFocus(hLPane);
					GetDlgItem(IDC_EDIT_FIND)->EnableWindow(TRUE);
					return;
				}else if(hPrevSearch1==hLPane){
					bCanStop=1;
				}
			}
		}
	}
	{// Part 2 - RPane
		POSITION pos=m_OptionsList.GetStartPosition();
		while(pos){
			HIROW hRPane=m_OptionsList.GetNextAssoc(pos);
			CString sKey=UnifyString(m_OptionsList.GetIItemText(hRPane,0));
			CString sKey2=UnifyString(m_OptionsList.GetIItemText(hRPane,1));
			if(sKey.Find(sText)!=-1 || sKey2.Find(sText)!=-1){
				HIROW hLPane=GetAliasHIROW(hRPane);
				while(hLPane==0){
					hRPane=m_OptionsList.GetParentIRow(hRPane);
					hLPane=GetAliasHIROW(hRPane);
				}
				if(hLPane!=0){
					if(bCanStop){
						bRes=1;
						hPrevSearch1=hLPane;
						MoveGlobalFocus(hLPane);
						GetDlgItem(IDC_EDIT_FIND)->EnableWindow(TRUE);
						return;
					}else if(hPrevSearch1==hLPane){
						bCanStop=1;
					}
				}
			}
		}
	}
	// Ниче не нашли, бум сначала
	sPrevSearch="";
	GetDlgItem(IDC_EDIT_FIND)->EnableWindow(TRUE);
	Sizer.ApplyLayout(TRUE);
	if(!bRes){
		AfxMessageBox(_l("Sorry, nothing familiar found! Try to reformulate the query"));
	}
	return;
}
