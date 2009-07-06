// DLG_Note.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "resource.h"
#include "DLG_Note.h"
#include "..\SmartWires\SystemUtils\SupportClasses.h"
extern BOOL bEnableDragWidgetMode;
extern CPoint pPrevEnableDragWidgetMode;
void DropDragWidgetMode();
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL IsVista();
/////////////////////////////////////////////////////////////////////////////
// CDLG_Note dialog

CDLG_Note::CDLG_Note(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_Note::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_Note)
	m_Title = _T("");
	//}}AFX_DATA_INIT
	dwToolTipDelay=dwToolTipDelay2=dwToolTipDelay3=dwToolTipDelay4=0;
	MainRulePos=-1;
	bNoColorSelect=FALSE;
	bNoChangeView=FALSE;
	bTitleDisabled=FALSE;
	bNoChangeIcon=FALSE;
	bNoMove=FALSE;
	bNoSize=FALSE;
	bJustEdit=FALSE;
	bNoLayout=FALSE;
	bNoBGImage=FALSE;
	bNewNote=FALSE;
	pNote=NULL;
	m_iIcon=0;
	m_bTileBG=1;
	m_bOpacity=0;
	hIco=NULL;
	bNoFont=0;
	pFont=0;
	bmBackground=NULL;
	bForceFocus=0;
}

void CDLG_Note::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Note)
	DDX_Control(pDX, IDC_NOTEICON, m_Icon);
	DDX_Control(pDX, IDC_BG, m_BG);
	DDX_Control(pDX, IDC_TX, m_TX);
	DDX_Control(pDX, IDC_FN, m_FN);
	DDX_Control(pDX, IDC_BG2, m_BG2);
	DDX_Control(pDX, IDC_TX2, m_TX2);
	DDX_Control(pDX, IDC_FN2, m_FN2);
	DDX_Control(pDX, IDC_VIEW, m_VI);
	DDX_Control(pDX, IDC_VIEW2, m_VI2);
	DDX_Text(pDX, IDC_TITLE, m_Title);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_Note, CDialog)
	//{{AFX_MSG_MAP(CDLG_Note)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_COMMAND(IDC_TX, OnTX)
	ON_COMMAND(IDC_BG, OnBG)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_BN_CLICKED(IDC_VIEW2, OnView2)
	ON_BN_CLICKED(IDC_NOTEICON, OnNoteicon)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_FN, OnFn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_Note message handlers

DWORD WINAPI CatchFocus2(LPVOID p)
{
	HWND h=(HWND)p;
	int iTimer=0;
	RECT recta;
	::GetWindowRect(h,&recta);
	DWORD dwX=(recta.left+recta.right)/2;
	DWORD dwY=(recta.top+recta.bottom)/2;
	CRect rtDesktop;
	GetWindowRect(GetDesktopWindow(),&rtDesktop);
	dwX=long(65536*double(dwX)/double(rtDesktop.Width()));
	dwY=long(65536*double(dwY)/double(rtDesktop.Height()));
	mouse_event(MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_ABSOLUTE,dwX,dwY,0,0);
	Sleep(200);
	mouse_event(MOUSEEVENTF_LEFTUP|MOUSEEVENTF_ABSOLUTE,dwX,dwY,0,0);
	while(iTimer<5 && IsWindow(h)){
		/*if(IsWindowVisible((HWND)p)||IsWindowVisible(GetParent((HWND)p))){
			break;
		}*/
		if(::GetForegroundWindow()==h){
			break;
		}
		iTimer++;
		SwitchToWindow(h,1);
		Sleep(200);
		//::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)(pMainDialogWindow->GetSafeHwnd()));::PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)(p));
	}
	return 0;
}


DWORD WINAPI CatchFocus(LPVOID p)
{
	/*int iTimer=0;
	while(iTimer<30){
		Sleep(200);
		if(IsWindowVisible((HWND)p)||IsWindowVisible(GetParent((HWND)p))){
			break;
		}
		iTimer++;
	}
	Sleep(5000);*/
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)(pMainDialogWindow->GetSafeHwnd()));
	::PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)(p));
	return 0;
}

BOOL CDLG_Note::OnInitDialog() 
{
#ifdef ART_VERSION
	objSettings.lNoteView=1;
	bNoLayout=1;
#endif
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,0,NULL);
	::CreateWindow("EDIT","",WS_VISIBLE|WS_BORDER|WS_CHILD|ES_WANTRETURN|ES_MULTILINE|ES_AUTOVSCROLL|(objSettings.lAutoWrapNote?0:ES_AUTOHSCROLL),4,20,CtrlWidth(this,IDC_NOTEICON)*3,CtrlHeight(this,IDC_NOTEICON)*2,this->GetSafeHwnd(),(HMENU)IDC_TEXT,AfxGetApp()->m_hInstance,NULL);
	CRect minRect(0,0,CtrlWidth(this,IDC_OK)+CtrlWidth(this,IDC_VIEW)+CtrlWidth(this,IDC_VIEW2)+25,CtrlHeight(this,IDC_NOTEICON)+CtrlHeight(this,IDC_OK)*3+8);
	Sizer.SetMinRect(minRect);
	//Sizer.SetControlSafeZone(4);
	// Центруем под мышой
	BOOL bCenterUnder=FALSE;
	if(pNote->left==0 && pNote->right==0){
		bCenterUnder=TRUE;
	}
	BOOL bVista=IsVista();
	SetButtonSize(this,IDC_VIEW,bVista?CSize(20,20):CSize(16,16));
	if(isWinXP()){
		SetButtonSize(this,IDC_OK,CSize(60,20));
		SetButtonSize(this,IDC_DEL,CSize(60,20));
	}else{
		SetButtonSize(this,IDC_OK,CSize(42,20));
		SetButtonSize(this,IDC_DEL,CSize(42,20));
	}
	Sizer.SetOptions(STICKABLE|SETCLIPSIBL|SIZEABLE|(bCenterUnder?CENTERUNDERMOUSE:0)|(bNoMove?FIXEDPOS:0)|(bNoSize?FIXEDSIZE:0));
	Sizer.AddDialogElement(IDC_VIEW,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_VIEW2,widthSize|heightSize,IDC_VIEW);
	Sizer.AddDialogElement(IDC_NOTEICON,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_OK,widthSize|heightSize|alwaysVisible);
	Sizer.AddDialogElement(IDC_TX,widthSize|heightSize|hidable,IDC_VIEW);
	Sizer.AddDialogElement(IDC_BG,widthSize|heightSize|hidable,IDC_VIEW);
	Sizer.AddDialogElement(IDC_FN,widthSize|heightSize|hidable,IDC_VIEW);
	Sizer.AddDialogElement(IDC_TX2,widthSize|heightSize|hidable,IDC_VIEW);
	Sizer.AddDialogElement(IDC_BG2,widthSize|heightSize|hidable,IDC_VIEW);
	Sizer.AddDialogElement(IDC_FN2,widthSize|heightSize|hidable,IDC_VIEW);
	Sizer.AddDialogElement(IDC_TITLE,heightSize);
	Sizer.AddDialogElement(IDC_TEXT,0);
	Sizer.AddDialogElement(IDC_DEL,widthSize|heightSize|hidable);

	int iViewType=0;
	if(bJustEdit==2){
		iViewType=1;
	}
	if(bVista){
		// Все под одну гребенку!!!
		iViewType=0;
		objSettings.lNoteView=0;
	}
	if(iViewType==1){
		Sizer.AddLeftTopAlign(IDC_TEXT,0,4);
		Sizer.AddRightBottomAlign(IDC_TEXT,0,-4);
		Sizer.AddTopAlign(IDC_TITLE,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_NOTEICON,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_OK,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_DEL,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_TX,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_BG,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_FN,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_TX2,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_BG2,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_FN2,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_VIEW,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_VIEW2,0,bottomPos,4);
	}else{
		MainRulePos=Sizer.AddGotoRule(bVista?0:(bJustEdit?1:objSettings.lNoteView));
		Sizer.AddGotoLabel(0);
		// Развернутый вид
		if(bVista){
			Sizer.AddLeftTopAlign(IDC_TITLE,0,4);
			Sizer.AddRightTopAlign(IDC_TITLE,0,4);

			Sizer.AddBottomAlign(IDC_DEL,0,-4);
			Sizer.AddBottomAlign(IDC_OK,0,-4);
			Sizer.AddBottomAlign(IDC_VIEW2,0,-4);
			Sizer.AddBottomAlign(IDC_TEXT,IDC_DEL,topPos,-4);
			Sizer.AddLeftAlign(IDC_DEL,0,4);
			Sizer.AddRightAlign(IDC_TEXT,0,-4);
			Sizer.AddLeftAlign(IDC_TEXT,0,4);
			Sizer.AddRightAlign(IDC_VIEW2,0,-4);
			Sizer.AddRightAlign(IDC_OK,IDC_VIEW2,leftPos,-4);

			//Sizer.AddRightBottomAlign(IDC_TEXT,0,4);
			//Sizer.AddLeftBottomAlign(IDC_TEXT,0,4);

			Sizer.AddTopAlign(IDC_TEXT,IDC_TITLE,bottomPos,4);
			Sizer.AddTopAlign(IDC_NOTEICON,0,bottomPos,4);
			Sizer.AddTopAlign(IDC_VIEW,0,bottomPos,4);
			Sizer.AddTopAlign(IDC_TX,0,bottomPos,4);
			Sizer.AddTopAlign(IDC_BG,0,bottomPos,4);
			Sizer.AddTopAlign(IDC_FN,0,bottomPos,4);
			Sizer.AddTopAlign(IDC_TX2,0,bottomPos,4);
			Sizer.AddTopAlign(IDC_BG2,0,bottomPos,4);
			Sizer.AddTopAlign(IDC_FN2,0,bottomPos,4);
		}else{
			Sizer.AddLeftTopAlign(IDC_TITLE,0,4);
			Sizer.AddRightTopAlign(IDC_NOTEICON,0,4);
			Sizer.AddRightAlign(IDC_TX,0,-4);
			Sizer.AddRightAlign(IDC_BG,0,-4);
			Sizer.AddRightAlign(IDC_FN,0,-4);
			Sizer.AddRightAlign(IDC_TX2,IDC_TX,leftPos,-3);
			Sizer.AddRightAlign(IDC_BG2,IDC_BG,leftPos,-3);
			Sizer.AddRightAlign(IDC_FN2,IDC_FN,leftPos,-3);
			Sizer.AddRightAlign(IDC_VIEW2,0,-4);
			Sizer.AddRightAlign(IDC_VIEW,IDC_VIEW2,leftPos,-3);
			Sizer.AddRightAlign(IDC_TITLE,IDC_NOTEICON,leftPos,-4);
			Sizer.AddRightAlign(IDC_OK,IDC_VIEW,leftPos,-4);
			Sizer.AddTopAlign(IDC_TX,IDC_NOTEICON,bottomPos,4);
			Sizer.AddTopAlign(IDC_BG,IDC_TX,bottomPos,4);
			Sizer.AddTopAlign(IDC_FN,IDC_BG,bottomPos,4);
			Sizer.AddTopAlign(IDC_TX2,IDC_NOTEICON,bottomPos,4);
			Sizer.AddTopAlign(IDC_BG2,IDC_TX2,bottomPos,4);
			Sizer.AddTopAlign(IDC_FN2,IDC_BG2,bottomPos,4);
			Sizer.AddLeftAlign(IDC_DEL,0,4);
			Sizer.AddBottomAlign(IDC_DEL,0,-4);
			Sizer.AddBottomAlign(IDC_OK,0,-4);
			Sizer.AddBottomAlign(IDC_VIEW,0,-6);
			Sizer.AddBottomAlign(IDC_VIEW2,0,-6);
			Sizer.AddBottomAlign(IDC_TEXT,IDC_DEL,topPos,-4);
			Sizer.AddTopAlign(IDC_TEXT,IDC_TITLE,bottomPos,4);
			Sizer.AddRightAlign(IDC_TEXT,IDC_NOTEICON,leftPos,-4);
			Sizer.AddLeftAlign(IDC_TEXT,0,4);
		}
		Sizer.AddGotoRule(100);
		// Свернутый вид
		Sizer.AddGotoLabel(1);
		Sizer.AddLeftTopAlign(IDC_TITLE,0,4);
		Sizer.AddRightTopAlign(IDC_TITLE,0,4);
		Sizer.AddRightBottomAlign(IDC_TEXT,0,4);
		Sizer.AddLeftBottomAlign(IDC_TEXT,0,4);
		Sizer.AddTopAlign(IDC_TEXT,IDC_TITLE,bottomPos,4);
		Sizer.AddTopAlign(IDC_OK,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_NOTEICON,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_DEL,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_TX,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_BG,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_FN,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_TX2,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_BG2,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_FN2,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_VIEW,0,bottomPos,4);
		Sizer.AddTopAlign(IDC_VIEW2,0,bottomPos,4);
		Sizer.AddRightAlign(IDC_TEXT,0,-4);
		Sizer.AddGotoRule(100);
		Sizer.AddGotoLabel(100);
		// Конец
	}
	SetTaskbarButton(this->GetSafeHwnd());
	SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW
	if(!bCenterUnder){
		CRect rtNote(pNote->left,pNote->top,pNote->right,pNote->bottom);
		AdjustToSceenCoords(rtNote);
		if(!objSettings.lUseNoreDirectPositioning){
			AdjustWindowRect(&rtNote,GetWindowLong(this->GetSafeHwnd(),GWL_STYLE),FALSE);
		}
		if(bJustEdit==2){
			long lDifX=rtNote.Width()-(minRect.Width()+50);
			if(lDifX<0){
				lDifX=-lDifX;
				rtNote.left-=lDifX/2;
				rtNote.right+=lDifX/2;
			}
			long lDifY=rtNote.Height()-(minRect.Height()+90);
			if(lDifY<0){
				lDifY=-lDifY;
				rtNote.top-=lDifY/2;
				rtNote.bottom+=lDifY/2;
			}
			Sizer.TestOnScreenOut(rtNote,3);
		}
		Sizer.SetStartupPos(rtNote);
		if(bNoMove||bNoSize){
			Sizer.SetMinRect(rtNote);
			SetWindowPos(&wndTopMost,Sizer.m_rMain.left,Sizer.m_rMain.top,Sizer.m_rMain.Width(),Sizer.m_rMain.Height(),0);
		}
	}
	if(!bCenterUnder){
		Sizer.TestOnScreenOut(Sizer.m_rMain,objSettings.iStickPix,1);
		MoveWindow(Sizer.m_rMain,TRUE);
	}
	// Продолжаем...
	if(bJustEdit==2){
		sTitle=pNote->sTitle;
	}else if(bJustEdit){
		sTitle=_l("Edit field");
	}else if(bNewNote){
		sTitle=_l("New desktop note");
	}else{
		sTitle=_l("Edit desktop note");
	}
	if(bTitleDisabled){
		((CEdit*)GetDlgItem(IDC_TITLE))->SetReadOnly(TRUE);
	}
	SetWindowText(sTitle);
	GetDlgItem(IDC_OK)->SetWindowText(_l("Save"));
	GetDlgItem(IDC_DEL)->SetWindowText(_l("Delete"));
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDC_OK), _l("Save note to desktop")+" (Ctrl-Enter)");
		m_tooltip.AddTool(GetDlgItem(IDC_NOTEICON), _l("Note icon")+" (Ctrl-I)");
		m_tooltip.AddTool(GetDlgItem(IDC_DEL), _l("Delete note")+" (Ctrl-D)");
		m_tooltip.AddTool(GetDlgItem(IDC_BG), _l("Note background color")+" (Ctrl-B)");
		m_tooltip.AddTool(GetDlgItem(IDC_BG2), _l("Note background color")+" (Ctrl-B)");
		m_tooltip.AddTool(GetDlgItem(IDC_TX), _l("Note text color")+" (Ctrl-T)");
		m_tooltip.AddTool(GetDlgItem(IDC_TX2), _l("Note text color")+" (Ctrl-T)");
		m_tooltip.AddTool(GetDlgItem(IDC_FN), _l("Change font")+" (Ctrl-F)");
		m_tooltip.AddTool(GetDlgItem(IDC_FN2), _l("Change font")+" (Ctrl-F)");
		m_tooltip.AddTool(GetDlgItem(IDC_VIEW), _l("Change layout")+" (Ctrl-Space)");
		m_tooltip.AddTool(GetDlgItem(IDC_VIEW2), _l("Change type")+" (Ctrl-1/4)");
	}
	bDefColors=0;
	dwBG=pNote->dwBColor;
	dwTX=pNote->dwTColor;
	if(dwBG==0 && dwTX==0){
		bDefColors=1;
		dwTX=0xFFFFFFFF;
		dwBG=0x44444444;
	}
	brBG.CreateSolidBrush(dwBG);
	brTX.CreateSolidBrush(dwTX);
	m_BG.SetBrush(dwBG);
	m_TX.SetBrush(dwTX);
	m_Icon.SetStyles(CBS_NORMAL|CBS_DDOWN);
	m_BG.SetStyles(CBS_NORMAL|CBS_DDOWN2|CBS_NONSQR);
	m_TX.SetStyles(CBS_NORMAL|CBS_DDOWN2|CBS_NONSQR);
	m_FN.SetStyles(CBS_NORMAL|CBS_DDOWN2|CBS_NONSQR);
	m_VI2.SetStyles(CBS_NORMAL|CBS_DDOWN2);
	m_VI.SetStyles(CBS_NORMAL);
	m_FN2.SetStyles(CBS_NOBG|CBS_HIQUAL);
	m_TX2.SetStyles(CBS_NOBG|CBS_HIQUAL);
	m_BG2.SetStyles(CBS_NOBG|CBS_HIQUAL);
	m_FN.SetBitmap(IDI_ICON_FN2);
	m_FN2.SetBitmap(IDI_ICON_FN);
	m_TX2.SetBitmap(IDI_ICON_TX);
	m_BG2.SetBitmap(IDI_ICON_BG);
	m_VI.SetBitmap(::AfxGetApp()->LoadIcon(IDI_ICON_VI));
	m_VI2.SetBitmap(::AfxGetApp()->LoadIcon(IDI_ICON_VI2));
	CString sNoteText=pNote->GetRealTextFromNote();//pNote->sNoteOriginalText;
	sNoteText.Replace("\n","\r\n");
	GetDlgItem(IDC_TEXT)->SetWindowText(sNoteText);
	m_Title=pNote->sTitle;
	if(bNoChangeIcon){
		m_iIcon=99;
		GetDlgItem(IDC_NOTEICON)->ShowWindow(FALSE);
	}else{
		m_iIcon=pNote->iIcon;
	}
	m_bTileBG=pNote->lTileBG;
	m_bOpacity=pNote->lOpacity;
	if(bNoFont){
		GetDlgItem(IDC_FN)->ShowWindow(FALSE);
		GetDlgItem(IDC_FN2)->ShowWindow(FALSE);
	}
	if(bNoColorSelect){
		GetDlgItem(IDC_TX)->ShowWindow(FALSE);
		GetDlgItem(IDC_TX2)->ShowWindow(FALSE);
		GetDlgItem(IDC_BG)->ShowWindow(FALSE);
		GetDlgItem(IDC_BG2)->ShowWindow(FALSE);
	}
	bmBackground=0;
	if(!bJustEdit){
		bmBackground=_bmp().GetRaw(GetNSkinBmp(objSettings.lNoteSkin,"TEXT"));
	}
	if(bmBackground){
		if(!objSettings.bUnderWindowsNT){
			CSize sz=GetBitmapSize(bmBackground);
			bmBackground=_bmp().Clone(bmBackground,sz);
		}
		brText.CreatePatternBrush(bmBackground);
	}else{
		brText.CreateSolidBrush(dwBG);
	}
	m_Icon.SetBrush(GetSysColor(COLOR_WINDOW));
	ChooseCustomFont(-2);
	pNote->bUnderEdit++;
	UpdateData(FALSE);
	RefreshIcon();
	hIDel=theApp.LoadIcon(IDI_ICON_DEL);
	hISave=theApp.LoadIcon(IDI_ICON_SAVE);
	hISwitch=theApp.LoadIcon(IDI_ICON_SWT);
	hIRem=theApp.LoadIcon(IDI_ICON_REM);
	if(!IsVista() && !bEnableDragWidgetMode){
		if(bJustEdit==2 && objSettings.bFirstJustEditNote){
			if(objSettings.bFirstJustEditNote==2){
				Alert(_l("Here you can enter text for selected desktop item.\nRight click to change colors and icon for the note.\nUse buttons in the title to save or delete note")+".\n"+_l("Click on 'Clock' icon to assign reminder to that note"),_l(AppName()+": Hint"),NULL,TRUE);
			}else{
				Alert(_l("Hint: Right-click note to popup menu\nwith all possible options and choices")+".\n"+_l("Click on 'Clock' icon to assign reminder to that note")+".",_l(AppName()+": Hint"),NULL,TRUE);
			}
			objSettings.bFirstJustEditNote=0;
			bForceFocus=0;
		}
	}
	if(pMainDialogWindow){
		pMainDialogWindow->ShowDescTooltip(NULL);
	}
	if(bEnableDragWidgetMode>0){
		DropDragWidgetMode();
		if(!bNoMove){
			//PostMessage(WM_SYSCOMMAND,SC_MOVE,0);
			FORK(EmutalewWMovw,GetSafeHwnd());
		}
	}else if(GetDlgItem(IDC_TEXT)){
		GetDlgItem(IDC_TEXT)->SetFocus();
	}
	if(bForceFocus){
		FORK(CatchFocus2,GetSafeHwnd());
	}
	return FALSE;
}

BOOL CDLG_Note::RefreshIcon()
{
	BOOL bRes=0;
	CBitmap* bmp=0;
	if(m_iIcon>=0){
		bmp=_bmp().GetRaw(GetNSkinBmp(objSettings.lNoteSkin,"ICO",m_iIcon));
	}else{
		bmp=_bmp().AddBmpFromDiskIfNotPresent(pNote->sCustomIcon,pNote->sCustomIcon+DEFNOTEPRF,DEFNOTEICONSIZE);
		_bmp().AddBmpFromDiskIfNotPresent(pNote->sCustomIcon,pNote->sCustomIcon+DEFNOTEPRFM,DEFNOTEICONSIZM);
	}
	if(!bmp){
		bmp=_bmp().GetRaw(IDB_BM_NOICON);
	}
	if(bmp){
		CRect rt;
		m_Icon.GetClientRect(&rt);
		m_Icon.SetBitmap(bmp);
		m_Icon.SetStyles(CBS_NORMAL|CBS_DDOWN);
		ClearIcon(hIco);
		hIco=CreateIconFromBitmap(bmp);
		SetIcon(hIco,TRUE);
		SetIcon(hIco,FALSE);
		bRes=TRUE;
	}
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_NOCHILDREN|RDW_INVALIDATE);
	return bRes;
}

void CDLG_Note::IncreaseFont(int iDir)
{
	CFont* font=pFont?pFont:objSettings.pNoteFont;
	if(font){
		CFont ft;
		LOGFONT logfont;
		font->GetLogFont(&logfont);
		logfont.lfHeight+=iDir;
		/*if(logfont.lfHeight<4){
			logfont.lfHeight=4;
		}*/
		ft.CreateFontIndirect(&logfont);
		CString sCustomFont=CreateStrFromFont(&ft, "note", 0, 0, 0, 0, 1);
		SetCustomFont(sCustomFont);
	}
}

BOOL CDLG_Note::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	if(pMsg->message==WM_RBUTTONUP){
		OnNoteiconEx(TRUE,TRUE);
		return TRUE;
	}
	if(pMsg->message==WM_MOUSEWHEEL){
		if(bJustEdit!=2 && GetKeyState(VK_CONTROL)<0){
			if((pMsg->wParam)&0x80000000){
				IncreaseFont(1);
			}else{
				IncreaseFont(-1);
			}
		}else{
			if((pMsg->wParam)&0x80000000){
				GetDlgItem(IDC_TEXT)->SendMessage(EM_LINESCROLL,0,(LPARAM)1);
			}else{
				GetDlgItem(IDC_TEXT)->SendMessage(EM_LINESCROLL,0,(LPARAM)-1);
			}
		}
		return 0;
	}
	UINT iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	if(pMsg->message==WM_KEYDOWN){
		if(iCtrlID==IDC_TITLE && pMsg->wParam==VK_TAB){
			GetDlgItem(IDC_TEXT)->SetFocus();
			return TRUE;
		}
		if(iCtrlID==IDC_TEXT && pMsg->wParam==VK_TAB){
			GetDlgItem(IDC_TITLE)->SetFocus();
			return TRUE;
		}
		if(GetKeyState(VK_CONTROL)<0){
			if(bJustEdit!=2){
				if(pMsg->wParam==VK_UP){
					IncreaseFont(1);
					return TRUE;
				}
				if(pMsg->wParam==VK_DOWN){
					IncreaseFont(-1);
					return TRUE;
				}
			}
			if(pMsg->wParam==VK_RETURN || pMsg->wParam=='S'){
				OnOk();
				return TRUE;
			}
			if(pMsg->wParam=='A'){
				GetDlgItem(IDC_TEXT)->SendMessage(EM_SETSEL,0,(LPARAM)-1);
				return TRUE;
			}
			if(pMsg->wParam=='C' || pMsg->wParam==VK_INSERT){
				int iBeg=0,iEnd=0;
				((CEdit*)GetDlgItem(IDC_TEXT))->GetSel(iBeg,iEnd);
				if(iBeg!=iEnd){
					USES_CONVERSION;
					CString sText;
					((CEdit*)GetDlgItem(IDC_TEXT))->GetWindowText(sText);
					sText=sText.Mid(iBeg,iEnd-iBeg);
					BOOL bGlo=0;
					SetClipboardText(A2W(sText),bGlo,0);
					return TRUE;
				}
			}
			/*
			if(pMsg->wParam=='C' || pMsg->wParam==VK_INSERT){
				ChooseCustomFont();
				return TRUE;
			}*/
			if(pMsg->wParam=='F'){
				ChooseCustomFont();
				return TRUE;
			}
			if(pMsg->wParam=='G'){
				ChooseCustomBg();
				return TRUE;
			}
			if(pMsg->wParam=='1'){
				SetViewMode(0);
				return TRUE;
			}
			if(pMsg->wParam=='2'){
				SetViewMode(1);
				return TRUE;
			}
			if(pMsg->wParam=='3'){
				SetViewMode(2);
				return TRUE;
			}
			if(pMsg->wParam=='4'){
				SetViewMode(3);
				return TRUE;
			}
			if(pMsg->wParam=='I'){
				OnNoteiconEx(TRUE,FALSE,IDC_NOTEICON);
				return TRUE;
			}
			if(pMsg->wParam=='P'){
				PrintNote();
				return TRUE;
			}
			if(objSettings.hRemin && pMsg->wParam=='R'){
				OnNotemenuReminder();
				return TRUE;
			}
			if(pMsg->wParam=='D'){
				OnDel();
				return TRUE;
			}
			if(pMsg->wParam=='B'){
				OnBG();
				return TRUE;
			}
			if(pMsg->wParam=='T'){
				OnTX();
				return TRUE;
			}
			if(pMsg->wParam==VK_SPACE){
				OnView();
				return TRUE;
			}
			if(GetKeyState(VK_SHIFT)>=0){
				if(pMsg->wParam==VK_UP){
					DelatIcon(1);
					return TRUE;
				}
				if(pMsg->wParam==VK_DOWN){
					DelatIcon(-1);
					return TRUE;
				}
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDLG_Note::GetCapButtonRect(CRect& rt, int iBtNum)
{
	GetWindowRect(&rt);
	rt.OffsetRect(-rt.left,-rt.top);
	rt.top=GetSystemMetrics(SM_CXSIZEFRAME);
	rt.bottom=rt.top+GetSystemMetrics(SM_CYCAPTION);
	rt.left=rt.right-GetSystemMetrics(SM_CXSIZEFRAME)-iBtNum*GetSystemMetrics(SM_CYCAPTION)+iBtNum-2;
	rt.right=rt.left+GetSystemMetrics(SM_CYCAPTION);
	rt.DeflateRect(1,1);
	return TRUE;
}

BOOL CDLG_Note::DrawNCArea(HDC hdc)
{
	CRect rt2,rt3,rt4,rt5;
	GetCapButtonRect(rt2,2);
	if(objSettings.lNoteView || bJustEdit){
		GetCapButtonRect(rt3,3);
		GetCapButtonRect(rt4,4);
		GetCapButtonRect(rt5,5);
		::DrawIconEx(hdc,rt2.left,rt2.top,hISave,rt2.Width(),rt2.Height(),0,brText,DI_NORMAL);
		::DrawIconEx(hdc,rt3.left,rt3.top,hIDel,rt3.Width(),rt3.Height(),0,brText,DI_NORMAL);
		if(objSettings.hRemin){
			::DrawIconEx(hdc,rt4.left,rt4.top,hIRem,rt4.Width(),rt4.Height(),0,brText,DI_NORMAL);
		}
		if(!bNoLayout){
			::DrawIconEx(hdc,rt5.left,rt5.top,hISwitch,rt5.Width(),rt5.Height(),0,brText,DI_NORMAL);
		}
	}else{
		if(objSettings.hRemin){
			::DrawIconEx(hdc,rt2.left,rt2.top,hIRem,rt2.Width(),rt2.Height(),0,brText,DI_NORMAL);
		}
	}
	return TRUE;
}

LRESULT CDLG_Note::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	if(objSettings.lAutoSaveNote && message==WM_ACTIVATEAPP && wParam==FALSE){
		PostMessage(WM_COMMAND,IDC_OK,0);
	}
	if(message==WM_NCHITTEST){
		LRESULT bRes=CDialog::DefWindowProc(message, wParam, lParam);
		if(bRes==HTSYSMENU){
			return LRESULT(-3);
		}
	}
	if(message==WM_NCLBUTTONDOWN || message==WM_NCRBUTTONDOWN){
		if(wParam==LRESULT(-3)){
			OnNoteiconEx(TRUE);
			return 0;
		}
	}
	static int bPrevCatch=0;
	if(message==WM_MOUSEMOVE){
		if(bPrevCatch!=0){
			SetWindowText(sTitle);
			RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
			bPrevCatch=0;
		}
	}
	if(message==WM_NCLBUTTONDOWN || message==WM_NCMOUSEMOVE){
		CRect rtO;
		BOOL bCatch=0;
		GetWindowRect(&rtO);
		SHORT x=LOWORD(lParam);
		SHORT y=HIWORD(lParam);
		CRect rt(rtO);
		CPoint pt(x-rt.left,y-rt.top);
		if(objSettings.lNoteView || bJustEdit){
			GetCapButtonRect(rt,3);
			if(rt.PtInRect(pt)){
				if(message==WM_NCMOUSEMOVE){
					bCatch=1;
					if(bCatch!=bPrevCatch){
						SetWindowText(_l("Note_Delete_note_button","Delete"));
						RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
					}
				}else{
					//OnDel();
					PostMessage(WM_COMMAND,IDC_DEL,0);
					// Предотвращаем повторное срабатывание...
					return 1;
				}
			}
			GetCapButtonRect(rt,2);
			if(rt.PtInRect(pt)){
				if(message==WM_NCMOUSEMOVE){
					bCatch=2;
					if(bCatch!=bPrevCatch){
						SetWindowText(_l("Note_Save_note_button","Save"));
						RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
					}
				}else{
					//OnOk();
					PostMessage(WM_COMMAND,IDC_OK,0);
					// Предотвращаем повторное срабатывание...
					return 1;
				}
			}
			if(objSettings.hRemin){
				GetCapButtonRect(rt,4);
				if(rt.PtInRect(pt)){
					if(message==WM_NCMOUSEMOVE){
						bCatch=3;
						if(bCatch!=bPrevCatch){
							SetWindowText(_l("Note_Reminder_layout_button","Remind later"));
							RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
						}
					}else{
						OnNotemenuReminder();
						return 0;
					}
				}
			}
			if(objSettings.lNoteView!=0 && !bNoLayout){
				GetCapButtonRect(rt,5);
				if(rt.PtInRect(pt)){
					if(message==WM_NCMOUSEMOVE){
						bCatch=3;
						if(bCatch!=bPrevCatch){
							SetWindowText(_l("Note_Change_layout_button","Layout"));
							RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
						}
					}else{
						OnView();
						return 0;
					}
				}
			}
		}else{
			if(objSettings.hRemin){
				GetCapButtonRect(rt,2);
				if(rt.PtInRect(pt)){
					if(message==WM_NCMOUSEMOVE){
						bCatch=3;
						if(bCatch!=bPrevCatch){
							SetWindowText(_l("Note_Reminder_layout_button","Remind later"));
							RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
						}
					}else{
						OnNotemenuReminder();
						return 0;
					}
				}
			}
		}
		if(bPrevCatch!=bCatch && !bCatch){
			SetWindowText(sTitle);
			RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
		}
		bPrevCatch=bCatch;
	}
	if(message==WM_PRINT){
		LRESULT bRes=CDialog::DefWindowProc(message, wParam, lParam);
		DrawNCArea(HDC(wParam));
		return bRes;
	}
	if(message==WM_NCPAINT || message==WM_NCACTIVATE){
		LRESULT bRes=CDialog::DefWindowProc(message, wParam, lParam);
		HWND hwnd=this->GetSafeHwnd();
		HDC hdc=::GetWindowDC(hwnd);
		DrawNCArea(hdc);
		::ReleaseDC(hwnd, hdc);
		return bRes;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDLG_Note::OnDel() 
{
	DropDragWidgetMode();
	CString sRMKey=pNote?pNote->GetReminderKey():"";
	if(objSettings.hRemin && sRMKey!="" && sNoteOnDate==""){// Если на дату - не удаляем связанны реминдер!
		_RemoveReminder fp3=(_RemoveReminder)GetProcAddress(objSettings.hRemin,"RemoveReminder");
		_IsReminderFamilyPresent fpPr=(_IsReminderFamilyPresent)GetProcAddress(objSettings.hRemin,"IsReminderFamilyPresent");
		int iMaxIndex=0;
		if(fpPr && (*fpPr)(sRMKey,&iMaxIndex)){
			for(int i=0;i<=iMaxIndex;i++){
				if(fp3){
					char szKey[64]={0};
					strcpy(szKey,GetNextPrefixedId(sRMKey,i));
					(*fp3)(szKey);
				}
			}
		}
	}
	CDialog::EndDialog(IDC_DEL);
}

void CDLG_Note::Apply(BOOL bClosingNote)
{
	DropDragWidgetMode();
	//-----------------
	UpdateData(TRUE);
	pNote->sTitle=m_Title;
	CString sNoteText;
	GetDlgItem(IDC_TEXT)->GetWindowText(sNoteText);
	sNoteText.Replace("\r\n","\n");
	pNote->SetRealTextFromNote(sNoteText);//sNoteText;
	pNote->iIcon=m_iIcon;
	pNote->lTileBG=m_bTileBG;
	pNote->lOpacity=m_bOpacity;
	CRect rt;
	if(!objSettings.lUseNoreDirectPositioning){
		GetClientRect(&rt);
		ClientToScreen(&rt);
	}else{
		GetWindowRect(&rt);
	}
	AdjustToSceenCoords(rt,FALSE);
	pNote->left=rt.left;
	pNote->top=rt.top;
	pNote->right=rt.right;
	pNote->bottom=rt.bottom;
	if(!bDefColors){
		pNote->dwBColor=dwBG;
		pNote->dwTColor=dwTX;
	}else if(bClosingNote){
		dwBG=0;
		dwTX=0;
	}
	if(bClosingNote){
		if((objSettings.iLikStatus<2 || isTempKey()) && rnd(0,100)>80){
			UnregAlert(_l("Warning: Several "+AppName()+" features are still locked")+"\n"+_l("Register "+AppName()+" to remove this limitation")+"!");
		}
	}
}

void CDLG_Note::OnOk() 
{
	Apply();
	CDialog::OnOK();
}

void CDLG_Note::DelatIcon(int iDir) 
{
	int iCurIcon=m_iIcon;
	if(iDir>0){
		m_iIcon++;
	}else{
		m_iIcon--;
	}
	if(m_iIcon<0){
		m_iIcon=objSettings.aNoteSkin[objSettings.lNoteSkin].iMaxIcons-1;
	}
	if(m_iIcon>=objSettings.aNoteSkin[objSettings.lNoteSkin].iMaxIcons){
		m_iIcon=0;
	}
	if(!RefreshIcon()){
		m_iIcon=iCurIcon;
	}
}

void CDLG_Note::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;
	if(pNMUpDown){
		if((pNMUpDown->iDelta)<=0){
			DelatIcon(1);
		}else{
			DelatIcon(-1);
		}
	}
}

void CDLG_Note::PostNcDestroy() 
{
	if(!objSettings.bUnderWindowsNT && bmBackground){
		delete bmBackground;
		bmBackground=NULL;
	}
	ClearIcon(hIDel);
	ClearIcon(hISave);
	ClearIcon(hISwitch);
	ClearIcon(hIco);
	ClearIcon(hIRem);
	if(pFont){
		delete pFont;
		pFont=0;
	}
	pNote->bUnderEdit--;
	CDialog::PostNcDestroy();
}

HBRUSH CDLG_Note::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT){
		pDC->SetBkColor(dwBG);
		pDC->SetTextColor(dwTX);
		pDC->SetBkMode(OPAQUE);
		return brBG;//CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	if(pWnd){
		DWORD dwID=pWnd->GetDlgCtrlID();
		if(dwID==IDC_TX){
			return brTX;
		}
		if(dwID==IDC_BG){
			return brBG;
		}
	}
	pDC->SetBkColor(dwBG);
	pDC->SetTextColor(dwTX);
	pDC->SetBkMode(TRANSPARENT);
	return brText;
}


void CDLG_Note::OnBG()
{
	if(bNoColorSelect){
		return;
	}
	DWORD dwCol=dwBG;//CC_ANYCOLOR
	CColorDialog dlg(dwCol,0,this);
	if(dlg.DoModal()==IDOK){
		bDefColors=0;
		dwBG=dlg.GetColor();
		brBG.Detach();
		m_BG.SetBrush(dwBG);
		brBG.CreateSolidBrush(dwBG);
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
	}
}

void CDLG_Note::SetNewTX(COLORREF dwCol)
{
	dwTX=dwCol;
	brTX.Detach();
	brTX.CreateSolidBrush(dwTX);
	m_TX.SetBrush(dwTX);
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ALLCHILDREN);
}

void CDLG_Note::OnTX()
{
	if(bNoColorSelect){
		return;
	}
	DWORD dwCol=dwTX;
	CColorDialog dlg(dwCol,0,this);
	if(dlg.DoModal()==IDOK){
		bDefColors=0;
		SetNewTX(dlg.GetColor());
	}
}

void CDLG_Note::ApplyView()
{
	if(!IsVista()){
		if(objSettings.lNoteView<0 || objSettings.lNoteView>1){
			objSettings.lNoteView=0;
		}
	}
	if(MainRulePos!=-1){
		Sizer.SetGotoRule(MainRulePos,objSettings.lNoteView);
	}
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASENOW|RDW_UPDATENOW|RDW_NOCHILDREN|RDW_INVALIDATE);
	Sizer.ApplyLayout();
	if(objSettings.lNoteView==1 && objSettings.bFirstMiniNote && bNoLayout!=2){
		Alert(_l("Use buttons in the title to save and delete note. Green - to save, \nRed - to delete, Black - to switch note into full layout")+".\n"+_l("Click on 'Clock' icon to assign reminder to that note")+".",_l(AppName()+": Hint"));
		objSettings.bFirstMiniNote=0;
	}
}

void CDLG_Note::OnView2()
{
	OnNoteiconEx(0,0,IDC_VIEW2);
}

void CDLG_Note::OnView()
{
	objSettings.lNoteView=1-objSettings.lNoteView;
	ApplyView();
}

CCriticalSection cLoadBmpSection;
void CDLG_Note::OnNoteiconEx(BOOL bAtCursor, BOOL bWithCP,DWORD dwUID)
{
#ifdef ART_VERSION
	return;
#endif
	CMenu menu;
	menu.CreatePopupMenu();
	CPoint pt;
	if(bAtCursor){
		GetCursorPos(&pt);
	}else{
		CRect rt;
		if(dwUID){
			GetDlgItem(dwUID)->GetWindowRect(&rt);
		}else if(bTitleDisabled){
			GetDlgItem(IDC_VIEW)->GetWindowRect(&rt);
		}else{
			GetDlgItem(IDC_NOTEICON)->GetWindowRect(&rt);
		}
		pt.x=rt.right;
		pt.y=rt.top;
	}
	if(bWithCP){
		AddMenuString(&menu,WM_USER+18,_l("Cut")+"\tCtrl-X",_bmp().Get(_IL(46)));
		AddMenuString(&menu,WM_USER+19,_l("Copy")+"\tCtrl-C",_bmp().Get(_IL(43)));
		AddMenuString(&menu,WM_USER+17,_l("Paste")+"\tCtrl-V",_bmp().Get(_IL(31)));
	}
	BOOL bAddingIconMenu=!bNoChangeIcon;
	/*if((objSettings.lNoteView==0 && bJustEdit!=2) || dwUID==IDC_VIEW2){//bWithCP && 
		bAddingIconMenu=0;
	}*/
	if(menu.GetMenuItemCount()>0){
		menu.AppendMenu(MF_SEPARATOR, 0, "");
	}
	if(bAddingIconMenu){
		CMenu icoMenu;
		icoMenu.CreatePopupMenu();
		CMenu* pIcoM=&icoMenu;
		if(dwUID==IDC_NOTEICON){
			pIcoM=&menu;
		}
		if(objSettings.aNoteSkin.GetSize()>0 && objSettings.lNoteSkin<objSettings.aNoteSkin.GetSize()){// Меню с иконками
			int iMaxIcons=objSettings.aNoteSkin[objSettings.lNoteSkin].iMaxIcons;
			int iMaxIcoTitles=objSettings.aNoteSkin[objSettings.lNoteSkin].aIconsTitles.GetSize();
			AddMenuString(pIcoM,iMaxIcons+1,_l("No icon"),_bmp().Get(IDB_BM_NOICON));
			for(int i=0;i<iMaxIcons;i++){
				AddMenuString(pIcoM,1+i,TrimMessage(_getLngString(objSettings.aNoteSkin[objSettings.lNoteSkin].aIconsTitles[i],NULL,objSettings.aNoteSkin[objSettings.lNoteSkin].sFolder,"strings"),30,2),_bmp().Get(GetNSkinBmp(objSettings.lNoteSkin,"ICO_TN",i)));
			}
			if(objSettings.aCustomIcons.GetSize()==0){
				AddMenuString(pIcoM,WM_USER+7,_l("Choose icon")+"\tCtrl-I",_bmp().Get(_IL(70)));
			}else{
				CSmartLock SL(&cLoadBmpSection,TRUE);
				CMenu menuXC;
				menuXC.CreatePopupMenu();
				//(pNote->sCustomIcon!="")?TrimMessage(pNote->sCustomIcon,30,3):_l("Choose icon")
				AddMenuString(&menuXC,WM_USER+7,_l("Choose another icon")+"\tCtrl-I",_bmp().Get(_IL(70)));
				for(int i2=0;i2<objSettings.aCustomIcons.GetSize();i2++){
					_bmp().AddBmpFromDiskIfNotPresent(objSettings.aCustomIcons[i2],objSettings.aCustomIcons[i2]+DEFNOTEPRFM,DEFNOTEICONSIZM);
					AddMenuString(&menuXC,WM_USER+100+i2,TrimMessage(objSettings.aCustomIcons[i2],30,3),_bmp().GetRaw(objSettings.aCustomIcons[i2]+DEFNOTEPRFM));
				}
				AddMenuString(&menuXC,WM_USER+9,_l("Clear history"),_bmp().Get(IDB_BM_DEL));
				AddMenuSubmenu(pIcoM,&menuXC,_l("Custom icon"),_bmp().Get(_IL(70)));
			}
			if(m_iIcon!=-1){
				::SetMenuDefaultItem(pIcoM->GetSafeHmenu(), 1+m_iIcon, FALSE);
			}
		}
		if(pIcoM!=&menu){
			AddMenuSubmenu(&menu,pIcoM,_l("Note icon"),_bmp().Get(_IL(6)));
		}
	}
	if(dwUID!=IDC_NOTEICON || objSettings.lNoteView!=0){
		if(!bNoColorSelect){
			AddMenuString(&menu,WM_USER+15,_l("Note text color")+"\tCtrl-T",_bmp().Get(_IL(67)));
			AddMenuString(&menu,WM_USER+16,_l("Note background color")+"\tCtrl-B",_bmp().Get(_IL(67)));
		}
	}
	{//if(bAddingIconMenu|| dwUID==IDC_VIEW2)
		if(!bNoBGImage){
			if(pNote->sCustomBg!=""){
				_bmp().AddBmpFromDiskIfNotPresent(pNote->sCustomBg,pNote->sCustomBg+DEFNOTEPRFB);
			}
			CSmartLock SL(&cLoadBmpSection,TRUE);
			CMenu menuXC;
			menuXC.CreatePopupMenu();
			AddMenuString(&menuXC,WM_USER+11,_l("Choose background")+"\tCtrl-G",_bmp().Get(_IL(70)));
			for(int i2=0;i2<objSettings.aCustomBg.GetSize();i2++){
				_bmp().AddBmpFromDiskIfNotPresent(objSettings.aCustomBg[i2],objSettings.aCustomBg[i2]+DEFNOTEPRFBM,DEFNOTEICONSIZM);
				AddMenuString(&menuXC,WM_USER+300+i2,TrimMessage(objSettings.aCustomBg[i2],30,3),_bmp().GetRaw(objSettings.aCustomBg[i2]+DEFNOTEPRFBM));
			}
			if(pNote->sCustomBg!=""){
				AddMenuString(&menuXC,WM_USER+26,_l("No background"));
			}
			menuXC.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&menuXC,WM_USER+20,_l("Default opacity"),0,m_bOpacity==0);
			AddMenuString(&menuXC,WM_USER+21,_l("Opacity")+" 100%",0,m_bOpacity==100);
			AddMenuString(&menuXC,WM_USER+22,_l("Opacity")+" 80%",0,m_bOpacity==80);
			AddMenuString(&menuXC,WM_USER+23,_l("Opacity")+" 60%",0,m_bOpacity==60);
			AddMenuString(&menuXC,WM_USER+24,_l("Opacity")+" 40%",0,m_bOpacity==40);
			AddMenuString(&menuXC,WM_USER+25,_l("Opacity")+" 20%",0,m_bOpacity==20);
			menuXC.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&menuXC,WM_USER+14,_l("Tile background"),0,m_bTileBG?1:0);
			if(objSettings.aCustomBg.GetSize()>0){
				AddMenuString(&menuXC,WM_USER+12,_l("Clear history"),_bmp().Get(IDB_BM_DEL));
			}
			AddMenuSubmenu(&menu,&menuXC,_l("Note background"),_bmp().Get(_IL(70)));
		}
	}
	if(dwUID!=IDC_NOTEICON || objSettings.lNoteView!=0){
		if(!bNoColorSelect){
			if(!bNoFont){
				if(objSettings.aCustomFonts.GetSize()==0){
					AddMenuString(&menu,WM_USER+8,_l("Choose font")+"\tCtrl-F",_bmp().Get(_IL(69)));
				}else{
					CSmartLock SL(&cLoadBmpSection,TRUE);
					CMenu menuXC;
					menuXC.CreatePopupMenu();
					AddMenuString(&menuXC,WM_USER+8,_l("Choose font")+"\tCtrl-F",_bmp().Get(_IL(69)));
					for(int i2=0;i2<objSettings.aCustomFonts.GetSize();i2++){
						AddMenuString(&menuXC,WM_USER+200+i2,TrimMessage(GetFontTextDsc(objSettings.aCustomFonts[i2]),30),_bmp().Get(_IL(69)));
					}
					AddMenuString(&menuXC,WM_USER+10,_l("Clear history"),_bmp().Get(IDB_BM_DEL));
					AddMenuSubmenu(&menu,&menuXC,_l("Custom font"),_bmp().Get(_IL(69)));
				}
			}
		}
		if(!bNoChangeView){
			//menu.AppendMenu(MF_SEPARATOR, 0, "");
			CMenu menuXC;
			menuXC.CreatePopupMenu();
			AddMenuString(&menuXC,WM_USER+1,_l("Full view")+"\tCtrl-1",0,pNote->lShowMode==0);
			AddMenuString(&menuXC,WM_USER+2,_l("Title, Icon")+"\tCtrl-2",0,pNote->lShowMode==1);
			AddMenuString(&menuXC,WM_USER+3,_l("Only Icon")+"\tCtrl-3",0,pNote->lShowMode==2);
			//AddMenuString(&menuXC,WM_USER+4,_l("Only Text")+"\tCtrl-4",0,pNote->lShowMode==3);
			AddMenuString(&menuXC,WM_USER+32,_l("Invisible"),0,pNote->lShowMode==4);
			AddMenuSubmenu(&menu,&menuXC,_l("View mode"),_bmp().Get(_IL(68)));
		}
		{
			CMenu menuAc;
			menuAc.CreatePopupMenu();
			AddMenuString(&menuAc,WM_USER+33,_l("Remind me later")+"\tCtrl-R",_bmp().Get(_IL(34)));
			if(!bNoLayout){
				AddMenuString(&menuAc,WM_USER+13,_l("Change layout")+"\tCtrl-Space");
				AddMenuString(&menuAc,WM_USER+31,_l("Snap size"));
			}
			AddMenuString(&menuAc,WM_USER+30,_l("Print note")+"\tCtrl-P",_bmp().Get(_IL(81)));
			AddMenuString(&menuAc,WM_USER+34,_l("Save to file"),_bmp().Get(_IL(45)));
			AddMenuString(&menuAc,WM_USER+35,_l("Bind to file"),0,pNote->sNoteOriginalTextFilePath==""?FALSE:TRUE);
			AddMenuSubmenu(&menu,&menuAc,_l("Actions"),_bmp().Get(_IL(65)));
		}
		if(menu.GetMenuItemCount()>0){
			menu.AppendMenu(MF_SEPARATOR, 0, "");
		}
		AddMenuString(&menu,WM_USER+6,_l("Delete note")+"\tCtrl-D",_bmp().Get(IDB_BM_NOTE_DEL));
		AddMenuString(&menu,WM_USER+5,_l("Save to desktop")+"\tCtrl-S",_bmp().Get(IDB_BM_NOTE_OK));
	}
	int iNum=::TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
	if(iNum==WM_USER+15){
		PostMessage(WM_COMMAND,IDC_TX,0);
	}else if(iNum==WM_USER+1){
		SetViewMode(0);
	}else if(iNum==WM_USER+2){
		SetViewMode(1);
	}else if(iNum==WM_USER+3){
		SetViewMode(2);
	}else if(iNum==WM_USER+4){
		SetViewMode(3);
	}else if(iNum==WM_USER+32){
		SetViewMode(4);
	}else if(iNum==WM_USER+16){
		PostMessage(WM_COMMAND,IDC_BG,0);
	}else if(iNum==WM_USER+5){
		PostMessage(WM_COMMAND,IDC_OK,0);
	}else if(iNum==WM_USER+6){
		PostMessage(WM_COMMAND,IDC_DEL,0);
	}else if(iNum==WM_USER+7){
		ChooseCustomIcon(-1);
	}else if(iNum==WM_USER+8){
		ChooseCustomFont(-1);
	}else if(iNum==WM_USER+9){
		objSettings.aCustomIcons.RemoveAll();
	}else if(iNum==WM_USER+10){
		objSettings.aCustomFonts.RemoveAll();
	}else if(iNum==WM_USER+11){
		ChooseCustomBg(-1);
	}else if(iNum==WM_USER+12){
		objSettings.aCustomBg.RemoveAll();
	}else if(iNum==WM_USER+13){
		objSettings.lNoteView=1-objSettings.lNoteView;
		ApplyView();
	}else if(iNum==WM_USER+14){
		m_bTileBG=1-m_bTileBG;
	}else if(iNum==WM_USER+20){
		m_bOpacity=0;
	}else if(iNum==WM_USER+21){
		m_bOpacity=100;
	}else if(iNum==WM_USER+22){
		m_bOpacity=80;
	}else if(iNum==WM_USER+23){
		m_bOpacity=60;
	}else if(iNum==WM_USER+24){
		m_bOpacity=40;
	}else if(iNum==WM_USER+25){
		m_bOpacity=20;
	}else if(iNum==WM_USER+26){
		pNote->sCustomBg="";
	}else if(iNum==WM_USER+30){
		PrintNote();
	}else if(iNum==WM_USER+31){
		OnNotemenuSnaptocontent();
	}else if(iNum==WM_USER+33){
		OnNotemenuReminder();
	}else if(iNum==WM_USER+34){
		OnNotemenuSaveToFile();
	}else if(iNum==WM_USER+35){
		OnNotemenuBindToFile();
	}else if(iNum>=WM_USER+100 && iNum<WM_USER+100+100){
		ChooseCustomIcon(iNum-(WM_USER+100));
	}else if(iNum>=WM_USER+200 && iNum<WM_USER+200+100){
		ChooseCustomFont(iNum-(WM_USER+200));
	}else if(iNum>=WM_USER+300 && iNum<WM_USER+300+100){
		ChooseCustomBg(iNum-(WM_USER+300));
	}else if(iNum==WM_USER+17 || iNum==WM_USER+18 || iNum==WM_USER+19){
		CWnd* pWnd=GetFocus();
		if(pWnd){
			if(iNum==WM_USER+17){
				pWnd->PostMessage(WM_PASTE,0,0);
			}
			if(iNum==WM_USER+18){
				pWnd->PostMessage(WM_CUT,0,0);
			}
			if(iNum==WM_USER+19){
				pWnd->PostMessage(WM_COPY,0,0);
			}
		}
	}else if(iNum>0){
		pNote->sCustomIcon="";
		m_iIcon=iNum-1;
		RefreshIcon();
	}
	return;
}

void CDLG_Note::SetViewMode(int iNewView)
{
	if(bNoChangeView){
		return;
	}
	pNote->lShowMode=iNewView;
}

void CDLG_Note::OnNoteicon() 
{
	OnNoteiconEx(FALSE,FALSE,IDC_NOTEICON);
}

void CDLG_Note::OnClose()
{
	if(bJustEdit==2){
		Apply();
		CDialog::EndDialog(IDOK);
		return;
	}
	CDialog::OnClose();
}

BOOL CDLG_Note::ChooseCustomIcon(int iBased)
{
	BOOL bOk=FALSE;
	if(iBased!=-1){
		bOk=TRUE;
		pNote->sCustomIcon=objSettings.aCustomIcons[iBased];
	}else{
		if(pNote->sCustomIcon==""){
			pNote->sCustomIcon=GetNSkinBmp(objSettings.lNoteSkin,"ICO",0);
		}
		CPreviewImage PreviewWnd;
		CPreviewFileDialog dlg(&PreviewWnd, TRUE, NULL, NULL, OFN_FILEMUSTEXIST, "Bitmaps (*.bmp;*.gif;*.jpg;*.jpeg;*.ico;*.png;*.pcx;*.tif;*.tga;*.tiff)|*.bmp;*.gif;*.jpg;*.jpeg;*.ico;*.png;*.pcx;*.tif;*.tga;*.tiff|Vector graphics (*.wmf)|*.wmf|All Files (*.*)|*.*||", this);
		char szFileNameBuffer[1024]="";
		dlg.m_ofn.lpstrFile=szFileNameBuffer;
		dlg.m_ofn.lpstrInitialDir=objSettings.sIcoInitDir;
		dlg.m_ofn.nMaxFile=sizeof(szFileNameBuffer);
		SimpleTracker Track(objSettings.bDoModalInProcess);
		int iCounter=0;
		if(dlg.DoModal()==IDOK){
			pNote->sCustomIcon=dlg.GetPathName();
			objSettings.sIcoInitDir=GetPathFolder(pNote->sCustomIcon);
			BOOL bNew=TRUE;
			for(int i=0;i<objSettings.aCustomIcons.GetSize();i++){
				if(objSettings.aCustomIcons[i]==pNote->sCustomIcon){
					bNew=FALSE;
					break;
				}
			}
			if(bNew){
				objSettings.aCustomIcons.InsertAt(0,pNote->sCustomIcon);
				if(objSettings.aCustomIcons.GetSize()>objSettings.iCustomsBuf){
					objSettings.aCustomIcons.SetSize(objSettings.iCustomsBuf);
				}
			}
			bOk=TRUE;
		}
	}
	if(bOk){
		m_iIcon=-1;
	}
	RefreshIcon();
	return TRUE;
}

BOOL CDLG_Note::SetCustomFont(CString sNewFont)
{
	if(pNote){
		if(pFont){
			delete pFont;
			pFont=0;
		}
		pNote->sCustomFont=sNewFont;
		if(sNewFont!=""){
			pFont=CreateFontFromStr(sNewFont);
		}
	}
	GetDlgItem(IDC_TEXT)->SetFont(pFont?pFont:objSettings.pNoteFont);
	GetDlgItem(IDC_TEXT)->Invalidate();
	return 1;
}

BOOL CDLG_Note::ChooseCustomFont(int iBased)
{
	BOOL bOk=FALSE;
	if(iBased!=-2){
		if(iBased!=-1){
			bOk=TRUE;
			pNote->sCustomFont=objSettings.aCustomFonts[iBased];
		}else{
			LOGFONT logfont;
			DWORD dwEffects=CF_EFFECTS|CF_SCREENFONTS;
			CFont* font=pFont?pFont:objSettings.pNoteFont;
			if(font){
				font->GetLogFont(&logfont);
				CFontDialog dlg(&logfont,dwEffects,NULL,this);
				dlg.m_cf.Flags|=dwEffects|CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST|CF_NOVERTFONTS;
				dlg.m_cf.lpLogFont=&logfont;
				dlg.m_cf.rgbColors=dwTX;
				if(dlg.DoModal()==IDOK){
					if(dlg.m_cf.rgbColors!=0 && dlg.m_cf.rgbColors!=dwTX){
						SetNewTX(dlg.m_cf.rgbColors);
					}
					CFont ft;
					ft.CreateFontIndirect(dlg.m_cf.lpLogFont);
					pNote->sCustomFont=CreateStrFromFont(&ft, "note", 0, 0, 0, 0, 1);
					BOOL bNew=TRUE;
					for(int i=0;i<objSettings.aCustomFonts.GetSize();i++){
						if(objSettings.aCustomFonts[i]==pNote->sCustomFont){
							bNew=FALSE;
							break;
						}
					}
					if(bNew){
						objSettings.aCustomFonts.InsertAt(0,pNote->sCustomFont);
						if(objSettings.aCustomFonts.GetSize()>objSettings.iCustomsBuf){
							objSettings.aCustomFonts.SetSize(objSettings.iCustomsBuf);
						}
					}
					bOk=TRUE;
				}
			}
		}
	}
	SetCustomFont(pNote?pNote->sCustomFont:"");
	return bOk;
}

BOOL CDLG_Note::ChooseCustomBg(int iBased)
{
	if(objSettings.aNoteSkin[objSettings.lNoteSkin].bIsHTML){
		// Not supported
		return FALSE;
	}
	BOOL bOk=FALSE;
	if(iBased!=-1){
		bOk=TRUE;
		pNote->sCustomBg=objSettings.aCustomBg[iBased];
	}else{
		if(pNote->sCustomBg==""){
			pNote->sCustomBg=GetNSkinBmp(objSettings.lNoteSkin,"TEXT");
		}
		CPreviewImage PreviewWnd;
		CPreviewFileDialog dlg(&PreviewWnd, TRUE, NULL, NULL, OFN_FILEMUSTEXIST, "Bitmaps (*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx;*.tif;*.tga;*.tiff;*.ico)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx;*.tif;*.tga;*.tiff;*.ico|Vector graphics (*.wmf)|*.wmf|All Files (*.*)|*.*||", this);
		char szFileNameBuffer[1024]="";
		dlg.m_ofn.lpstrFile=szFileNameBuffer;
		dlg.m_ofn.lpstrInitialDir=objSettings.sIcoInitDir;
		dlg.m_ofn.nMaxFile=sizeof(szFileNameBuffer);
		SimpleTracker Track(objSettings.bDoModalInProcess);
		int iCounter=0;
		if(dlg.DoModal()==IDOK){
			pNote->sCustomBg=dlg.GetPathName();
			objSettings.sIcoInitDir=GetPathFolder(pNote->sCustomBg);
			BOOL bNew=TRUE;
			for(int i=0;i<objSettings.aCustomBg.GetSize();i++){
				if(objSettings.aCustomBg[i]==pNote->sCustomBg){
					bNew=FALSE;
					break;
				}
			}
			if(bNew){
				objSettings.aCustomBg.InsertAt(0,pNote->sCustomBg);
				if(objSettings.aCustomBg.GetSize()>objSettings.iCustomsBuf){
					objSettings.aCustomBg.SetSize(objSettings.iCustomsBuf);
				}
			}
			bOk=TRUE;
		}
	}
	return TRUE;
}

void CDLG_Note::OnFn() 
{
	if(bNoFont){
		return;
	}
	if(objSettings.aCustomFonts.GetSize()==0){
		ChooseCustomFont(-1);
		return;
	}
	CMenu menuXC;
	menuXC.CreatePopupMenu();
	CPoint pt;
	CRect rt;
	GetDlgItem(IDC_FN)->GetWindowRect(&rt);
	pt.x=rt.right;
	pt.y=rt.top;
	if(!bNoColorSelect){
		CSmartLock SL(&cLoadBmpSection,TRUE);
		AddMenuString(&menuXC,WM_USER+8,_l("Choose font")+"\tCtrl-F",_bmp().Get(_IL(69)));
		for(int i2=0;i2<objSettings.aCustomFonts.GetSize();i2++){
			AddMenuString(&menuXC,WM_USER+200+i2,TrimMessage(GetFontTextDsc(objSettings.aCustomFonts[i2]),30),_bmp().Get(_IL(69)));
		}
		AddMenuString(&menuXC,WM_USER+10,_l("Clear history"),_bmp().Get(IDB_BM_DEL));
	}
	int iNum=::TrackPopupMenu(menuXC.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
	if(iNum==WM_USER+10){
		objSettings.aCustomFonts.RemoveAll();
	}else if(iNum==WM_USER+8){
		ChooseCustomFont(-1);
	}else if(iNum>=WM_USER+200 && iNum<WM_USER+200+100){
		ChooseCustomFont(iNum-(WM_USER+200));
	}
	return;
}

void CDLG_Note::PrintNote()
{
	if(!PrintWindow(this,pNote->GetRealTextFromNote(),pNote->sTitle)){
		Alert(_l("Failed to print ")+" '"+pNote->sTitle+"'!");
	}
}

void CDLG_Note::OnNotemenuSnaptocontent() 
{
	CSize size(200,100);
	CString sText;
	((CEdit*)GetDlgItem(IDC_TEXT))->GetWindowText(sText);
	CDC* pDC=((CEdit*)GetDlgItem(IDC_TEXT))->GetDC();
	if(pDC){
		int iLines=0;
		pDC->SelectObject(pFont?pFont:objSettings.pNoteFont);
		size=GetLargestLineSize(pDC,sText,iLines);
		iLines+=2;
		pDC->LPtoDP(&size);
		size.cy=size.cy*iLines;
		ReleaseDC(pDC);
	}
	Sizer.m_rMain.right=Sizer.m_rMain.left+size.cx+10;
	Sizer.m_rMain.bottom=Sizer.m_rMain.top+size.cy+10;
	MoveWindow(Sizer.m_rMain);
	Sizer.ApplyLayout();
}

void CDLG_Note::OnNotemenuBindToFile()
{
	if(pNote->sNoteOriginalTextFilePath!=""){
		pNote->sNoteOriginalTextFilePath="";
	}else{
		CString sFile;
		if(!OpenFileDialog("Any files (*.*)|*.*||",&sFile,0,FALSE,TRUE,this)){
			return;
		}
		if(!isFileExist(sFile)){
			return;
		}
		pNote->sNoteOriginalTextFilePath=sFile;
		CString sNoteText;
		ReadFile(pNote->sNoteOriginalTextFilePath,sNoteText);
		GetDlgItem(IDC_TEXT)->SetWindowText(sNoteText);
	}
}

void CDLG_Note::OnNotemenuSaveToFile()
{
	UpdateData(TRUE);
	CString sText=m_Title;
	CString sNoteText;
	GetDlgItem(IDC_TEXT)->GetWindowText(sNoteText);
	sText+="\r\n";
	sText+=sNoteText;
	DWORD dwDataSize=strlen(sText);
	HGLOBAL hGlobal=::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, dwDataSize+1);
	if (hGlobal == NULL){
		return;
	};
	char* sFileContent = (char*)hGlobal;
	strcpy(sFileContent,sText);
	CString sSavePath=m_Title+".txt";
	BOOL bSaveRes=SaveFileFromHGlob(sSavePath,getMyDocPath()+"\\",hGlobal,dwDataSize,TRUE,0,this);
	VERIFY(::GlobalFree(hGlobal)==NULL);
	hGlobal=NULL;
}

CString GetRemDsc(CWPReminder& rem, BOOL bDate=0)
{
	CString sDsc=TrimMessage(rem.szText,30,1);
	if(sDsc==""){
		sDsc=_l("No description");
	}
	if(bDate){
		char szTmp[64]={0};
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&rem.EventTime,0,szTmp,sizeof(szTmp));
		return Format("%s\t%s %02i:%02i",sDsc,szTmp,rem.EventTime.wHour,rem.EventTime.wMinute);
	}else{
		char szTmp[64]={0};
		GetTimeFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&rem.EventTime,0,szTmp,sizeof(szTmp));
		//return Format("%02i:%02i %s",rem.EventTime.wHour,rem.EventTime.wMinute,sDsc);
		return Format("%s %s",szTmp,sDsc);
	}
}

void AddMapItems(CStringArray& aKeys,CStringArray& aTitles, const char* szKey, const char* szDsc)
{
	for(int i=0;i<aKeys.GetSize();i++){
		if(aKeys[i]==szKey){
			return;
		}
	}
	aTitles.SetAtGrow(aKeys.Add(szKey),szDsc);
}

void CDLG_Note::OnNotemenuReminder()
{
	if(!objSettings.hRemin || pNote==0){
		return;
	}
	sNoteOnDate.TrimRight();
	sNoteOnDate.TrimLeft();
	BOOL bReadOnlyModify=0;//(sNoteOnDate=="")?0:1;
	BOOL bDscWithDate=1;//(sNoteOnDate=="")?1:0;
	CString sRMKey=pNote->GetReminderKey();
	if(sRMKey.GetLength()==0){
		sRMKey="COMMON";
		if(sNoteOnDate==""){
			bReadOnlyModify=0;
			sNoteOnDate="ALL";
		}
	}
#ifdef USE_LITE
	AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
	return;
#endif
	Apply(FALSE);
	// Возможно нужен выбор?
	CStringArray aTitles;
	CStringArray aKeys;
	aKeys.SetAtGrow(0,sRMKey);
	aTitles.SetAtGrow(0,_l("Add reminder"));
	int iMaxIndex=0;
	_GetReminder fp=(_GetReminder)GetProcAddress(objSettings.hRemin,"GetReminder");
	_IsReminderFamilyPresent fpPr=(_IsReminderFamilyPresent)GetProcAddress(objSettings.hRemin,"IsReminderFamilyPresent");
	_GetCountOnDate fpCountOnDate=(_GetCountOnDate)GetProcAddress(objSettings.hRemin,"GetCountOnDate");
	if(fpPr && (*fpPr)(sRMKey,&iMaxIndex)){
		aKeys[0]="";
		CString sRMKeyTmp=sRMKey;
		for(int i=0;i<=iMaxIndex;i++){
			if(fp){
				CWPReminder rem;
				strcpy(rem.szKey,GetNextPrefixedId(sRMKey,i));
				if((*fp)(rem.szKey,rem)){
					AddMapItems(aKeys,aTitles, rem.szKey, GetRemDsc(rem,bDscWithDate));
				}
			}
		}
	}
	if(sNoteOnDate!=""){
		int iCount=0;
		char szDate[64]={0};
		strcpy(szDate,sNoteOnDate);
		if(fpCountOnDate){
			iCount=(*fpCountOnDate)(szDate);
		}
		if(iCount>0){
			_GetReminderOnDate fp3=(_GetReminderOnDate)GetProcAddress(objSettings.hRemin,"GetReminderOnDate");
			if(fp3){
				for(int i=0;i<iCount;i++){
					CWPReminder rem;
					if(!(*fp3)(szDate,i,rem)){
						break;
					}
					AddMapItems(aKeys,aTitles, rem.szKey, GetRemDsc(rem,bDscWithDate));
				}
			}
		}
	}
	//
	if(aKeys.GetSize()==1){
		sRMKey=aKeys[0];
	}else{
		CMenu menu;
		menu.CreatePopupMenu();
		CPoint pt;
		GetCursorPos(&pt);
		for(int i=0;i<aKeys.GetSize();i++){
			AddMenuString(&menu,WM_USER+i,aTitles[i]);
		}
		int iNum=::TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
		if(iNum==0 || iNum<WM_USER || iNum>=WM_USER+aKeys.GetSize()){
			return;
		}
		CString sRMKeyTmp=aKeys[iNum-WM_USER];
		if(sRMKeyTmp==""){
			sRMKeyTmp=GetNextPrefixedId(sRMKey);
			while((fpPr && (*fpPr)(sRMKeyTmp,0))){
				sRMKeyTmp=GetNextPrefixedId(sRMKeyTmp);
			}
		}
		sRMKey=sRMKeyTmp;
	}
	//
	BOOL bNewReminder=0;
	CWPReminder rem;
	memset(&rem,0,sizeof(CWPReminder));
	if(fp){
		strcpy(rem.szKey,sRMKey);
		if(!(*fp)(rem.szKey,rem)){
			if(objSettings.iLikStatus<2){
				char szAlll[64]="ALL";
				if((*fpCountOnDate)(szAlll)>2){
					UnregAlert(OPTION_REMD_UNREG);
					return;
				}
			}
			// Создаем новый!
			bNewReminder=1;
			COleDateTime dt=COleDateTime::GetCurrentTime();
			if(sNoteOnDate!=""){
				// Берем только дату!!!
				COleDateTime dt2=(CDataXMLSaver::Str2OleDate(sNoteOnDate));
				dt.SetDateTime(dt2.GetYear(),dt2.GetMonth(),dt2.GetDay(),
					dt.GetHour(),dt.GetMinute(),dt.GetSecond());
			}
			dt=dt+COleDateTimeSpan(0,0,15,0);
			dt.GetAsSystemTime(rem.EventTime);
			rem.bActSound=1;
			rem.bActPopup=1;
			rem.bLoopSound=objSettings.lLoopRemidersSound;
			rem.bDisableRemider=1;//Актвным станет лишь после модификации!!!
			strcpy(rem.szText,pNote->GetRealTextFromNote().Left(4000));
			pMainDialogWindow->GetRemDefWav(rem.szSoundPath,sizeof(rem.szSoundPath));
			_PutReminder fp2=(_PutReminder)GetProcAddress(objSettings.hRemin,"PutReminder");
			if(fp2){
				(*fp2)(rem.szKey,rem);
			}
		}
	}
	_CallModifyReminder fp1=(_CallModifyReminder)GetProcAddress(objSettings.hRemin,"CallModifyReminder");
	if(fp1){
		int iRes=(*fp1)(rem.szKey,GetSafeHwnd(),bReadOnlyModify);
		if(iRes==IDCANCEL && bNewReminder){
			_RemoveReminder fp3=(_RemoveReminder)GetProcAddress(objSettings.hRemin,"RemoveReminder");
			if(fp3){
				(*fp3)(rem.szKey);
			}
		}
	}
}
