/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Implementation of the class "CCalendarDlg2"
	  (handy calendar)
	  
		$Log$
		
******************************************************************************/

#include "stdafx.h"
#include "CalendarDlg2.h"
#include "DLGEnterNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/*** Definition of the class "CCalendarDlg2" **********************************/

/*** Constructor *************************************************************/
CCalendarDlg2::CCalendarDlg2(CWPReminder remIn, CWnd* pParent, DWORD dwiOption): CResizeableDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalendarDlg2)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	memcpy(&rem,&remIn,sizeof(CWPReminder));
	dwOption=dwiOption;
	iModalRes=IDCANCEL;
	dXPeriod=0;
}

/*** Protected member functions **********************************************/

/*** Data exchange: member variables <--> controls ***************************/
void CCalendarDlg2::DoDataExchange(CDataExchange* pDX)
{
	CResizeableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarDlg2)
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_mcMonthCal);
	DDX_Control(pDX, IDC_EDIT_TXT, m_eText);
	//}}AFX_DATA_MAP
}

/*** Will be called before the first display of the dialog *******************/
void CCalendarDlg2::SetInfoText()
{
	CString sText=_lx(Format("RMX.UI.RECUR%i",iRecurrency),Format("RECURRENCY%i",iRecurrency));
	if(iRecurrency==RECURRENCY_XMINUTES){
		sText+=Format(", %s %i %s",_lx("RMX.UI.EVERY"),dXPeriod,_lx("RMX.UI.MIN"));
	}
	if(iRecurrency==RECURRENCY_XHOURS){
		sText+=Format(", %s %i %s",_lx("RMX.UI.EVERY"),dXPeriod,_lx("RMX.UI.HOUR"));
	}
	GetDlgItem(IDC_INFO_REC)->SetWindowText(sText+" ");
}

BOOL CCalendarDlg2::OnInitDialog()
{
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	pDialogFont.Attach((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	SetFont(&pDialogFont);
	m_mcMonthCal.pDelegates=this;
	CResizeableDialog::OnInitDialog();
	// Парсим файл
	LRESULT lRes=0;
	NMDAYSTATE pDayState;
	SYSTEMTIME stMinRange, stMaxRange;
	memset(&pDayState,0,sizeof(pDayState));
	m_mcMonthCal.GetMonthRange(&stMinRange, &stMaxRange, GMR_DAYSTATE);
	pDayState.cDayState=(stMaxRange.wYear - stMinRange.wYear) * 12 + stMaxRange.wMonth - stMinRange.wMonth + 1;
	memcpy(&pDayState.stStart,&stMinRange,sizeof(SYSTEMTIME));
	OnGetdaystate((NMHDR*)&pDayState,&lRes);
	//pDayState.prgDayState[0]=0xffff;
	//memset(pDayState.prgDayState,555,5*sizeof(MONTHDAYSTATE));
	m_mcMonthCal.SetDayState(pDayState.cDayState,pDayState.prgDayState);
	m_mcMonthCal.Invalidate();
	//-----------------------------------
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetWindowText(_lx("RMX.UI.MODIFY","Modify reminder"));
	GetDlgItem(IDOK)->SetWindowText(_lx("RMX.UI.SAVECLOSE","Save and close"));
	GetDlgItem(IDDEL)->SetWindowText(_lx("RMX.UI.DELETE","Delete reminder"));
	GetDlgItem(IDCANCEL_REM)->SetWindowText(_lx("RMX.UI.CANCEL","Cancel"));
	GetDlgItem(IDC_STATIC_TOP)->SetWindowText(_lx("RMX.UI.EDIT","Edit reminder. Fill in this form and save it to apply reminder"));
	GetDlgItem(IDC_CHECK_ACTIVE2)->SetWindowText(_lx("RMX.UI.ACTIVATEON","Activate this reminder on"));
	GetDlgItem(IDC_CHECK_REC)->SetWindowText(_lx("RMX.UI.REPEAT","Repeat reminder"));
	GetDlgItem(IDC_CHECK_ONEVENT_TEXT)->SetWindowText(_lx("RMX.UI.MESSAGE","Popup message"));
	GetDlgItem(IDC_CHECK_ONEVENT_MP3)->SetWindowText(_lx("RMX.UI.SOUND","Play sound"));
	GetDlgItem(IDC_CHECK_ONEVENT_APP)->SetWindowText(_lx("RMX.UI.LAUNCH","Launch application"));
	GetDlgItem(IDC_CHECK_MOREOPTION)->SetWindowText(_lx("RMX.UI.MOREOPTION","More options"));
	// Register resizable controls
	RegisterControl(IDC_STATIC_TOP, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_MONTHCALENDAR1, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_CHECK_ACTIVE2, ATTACH_TO_TOP_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDC_STATIC_DDMMYY, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_DATETIMEPICKER_TIME, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_BUTTON_TIME, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_CHECK_REC, ATTACH_TO_TOP_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDC_INFO_REC, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_BUTTON_REC, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_BUTTON_MOREOPTIONS, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_CHECK_MOREOPTION, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_CHECK_ONEVENT_TEXT, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_EDIT_TXT, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_CHECK_ONEVENT_MP3, ATTACH_TO_TOP_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDC_EDIT_MP3, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_BUTTON_MP3, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_BUTTON_MP3_PLAY, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_CHECK_ONEVENT_APP, ATTACH_TO_TOP_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDC_EDIT_APP, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_BUTTON_APP, ATTACH_TO_TOP_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDCANCEL_REM, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDDEL, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDOK, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_RIGHT_EDGE);
	// Calculate minimum and maximum size of calendar window
	CRect rect;
	m_mcMonthCal.GetClientRect(rect);
	int nWidth  = rect.Width();
	int nHeight = rect.Height();
	
	if (m_mcMonthCal.GetMinReqRect(rect))
	{
		int   nMinWidth   = rect.Width();
		int   nMinHeight  = rect.Height();
		int   nDiffWidth  = nMinWidth  - nWidth;
		int   nDiffHeight = nMinHeight - nHeight;
		CRect rectOffset(nDiffWidth/2, nDiffHeight/2, nDiffWidth/2 + nDiffWidth%2,
			nDiffHeight/2 + nDiffHeight%2);
		
		GetWindowRect(rect);
		rect += rectOffset;
		MoveWindow(rect);
		ThisSizeIsMinSize();
		
		int nMaxWidth  = nMinWidth  * 2 +  6*(3-1);
		int nMaxHeight = nMinHeight * 2 + 13*(4-1);
		
		nDiffWidth  = nMaxWidth  - nWidth;
		nDiffHeight = nMaxHeight - nHeight;
		rectOffset  = CRect(nDiffWidth/2, nDiffHeight/2,
			nDiffWidth/2 + nDiffWidth%2,
			nDiffHeight/2 + nDiffHeight%2);
		
		GetWindowRect(rect);
		rect += rectOffset;
		
		int nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		if (rect.Width() > nScreenWidth)
		{
			rect.left  = 0;
			rect.right = nScreenWidth-1;
		}
		if (rect.Height() > nScreenHeight)
		{
			rect.top    = 0;
			rect.bottom = nScreenHeight-1;
		}
		m_nMaxWidth  = m_nMinWidth*2;
		m_nMaxHeight = m_nMinHeight;
	}
	if((dwOption & MODIFY_OPTS_NODAYS)!=0){
		m_mcMonthCal.EnableWindow(FALSE);
		//m_mcMonthCal.ModifyStyle(WS_BORDER|WS_DLGFRAME|WS_THICKFRAME,0,SWP_FRAMECHANGED);
	}
	iRecurrency=rem.iRecurrency;
	dXPeriod=rem.dXMinHourPeriod;
	CString sText=rem.szText;
	sText.Replace("\r\n","\n");
	sText.Replace("\n","\r\n");
	//int iRecurrency;
	m_eText.SetWindowText(sText);
	SetInfoText();
	GetDlgItem(IDC_EDIT_MP3)->SetWindowText(rem.szSoundPath);
	GetDlgItem(IDC_EDIT_APP)->SetWindowText(rem.szLaunchPath);
	GetDlgItem(IDC_BUTTON_MP3_PLAY)->SetWindowText(">");
	GetDlgItem(IDC_CHECK_ONEVENT_TEXT)->SendMessage(BM_SETCHECK, rem.bActPopup, 0);
	GetDlgItem(IDC_CHECK_ONEVENT_APP)->SendMessage(BM_SETCHECK, rem.bActLaunch, 0);
	GetDlgItem(IDC_CHECK_ONEVENT_MP3)->SendMessage(BM_SETCHECK, rem.bActSound, 0);
	BOOL bResult = (BOOL)::SendMessage(m_mcMonthCal, MCM_SETCURSEL, 0, (LPARAM) &rem.EventTime);
	GetDlgItem(IDC_DATETIMEPICKER_TIME)->SendMessage(DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&rem.EventTime);
	char szTmp[256]={0};
	GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&rem.EventTime,0,szTmp,sizeof(szTmp));
	GetDlgItem(IDC_STATIC_DDMMYY)->SetWindowText(szTmp);
	m_eText.SetFocus();
	m_eText.SetSel(0,0xFFFF,0);
	return FALSE;  // return TRUE  unless you set the focus to a control
}

/*** Draw icon in minimized mode *********************************************/
void CCalendarDlg2::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CResizeableDialog::OnPaint();
	}
}

/*** The system calls this to obtain the cursor to display while the user ****/
/*** drags the minimized window                                           ****/
HCURSOR CCalendarDlg2::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*** Selection in calendar control has changed *******************************/
void CCalendarDlg2::OnSelchangeMonthcalendar1(NMHDR*, LRESULT* pResult) 
{
	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)::SendMessage(m_mcMonthCal, MCM_GETCURSEL, 0, (LPARAM) &sysTime);
	if(bResult){
		COleDateTime dtSelected=COleDateTime(sysTime.wYear,sysTime.wMonth,sysTime.wDay,0,0,0);
		SYSTEMTIME sm;
		dtSelected.GetAsSystemTime(sm);
		char szTmp[256]={0};
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&sm,0,szTmp,sizeof(szTmp));
		GetDlgItem(IDC_STATIC_DDMMYY)->SetWindowText(szTmp);
	}
	*pResult = 0;
}

/*** System menu entry has been selected *************************************/
void CCalendarDlg2::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		return;
	}
	if(nID==SC_CLOSE){
		OnCancel();
		return;
	}
	CResizeableDialog::OnSysCommand(nID, lParam);
}

void CCalendarDlg2::OnCancel()
{
	iModalRes=IDCANCEL;
	CResizeableDialog::OnCancel();
}

void CCalendarDlg2::OnDel()
{
	iModalRes=IDABORT;
	CResizeableDialog::OnCancel();
}

CString sLastMp3;
void CCalendarDlg2::OnOK()
{
	iModalRes=IDOK;
	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)::SendMessage(m_mcMonthCal, MCM_GETCURSEL, 0, (LPARAM) &sysTime);
	if(bResult){
		memcpy(&rem.EventTime,&sysTime,sizeof(rem.EventTime));
		//int iRecurrency;
		rem.iRecurrency=iRecurrency;
		rem.dXMinHourPeriod=dXPeriod;
		GetDlgItem(IDC_EDIT_TXT)->GetWindowText(rem.szText,sizeof(rem.szText));
		GetDlgItem(IDC_EDIT_MP3)->GetWindowText(rem.szSoundPath,sizeof(rem.szSoundPath));
		GetDlgItem(IDC_EDIT_APP)->GetWindowText(rem.szLaunchPath,sizeof(rem.szLaunchPath));
		rem.bActPopup=GetDlgItem(IDC_CHECK_ONEVENT_TEXT)->SendMessage(BM_GETCHECK, 0, 0);
		rem.bActLaunch=GetDlgItem(IDC_CHECK_ONEVENT_APP)->SendMessage(BM_GETCHECK, 0, 0);
		rem.bActSound=GetDlgItem(IDC_CHECK_ONEVENT_MP3)->SendMessage(BM_GETCHECK, 0, 0);
		GetDlgItem(IDC_DATETIMEPICKER_TIME)->SendMessage(DTM_GETSYSTEMTIME, 0, (LPARAM)&sysTime);
		rem.EventTime.wHour=sysTime.wHour;
		rem.EventTime.wMinute=sysTime.wMinute;
		rem.EventTime.wSecond=sysTime.wSecond;
	}
	CResizeableDialog::OnOK();
}


BOOL CCalendarDlg2::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN){
		if(GetDlgItem(IDC_EDIT_TXT)==GetFocus()){
			//Включем галочку
			//IDC_CHECK_ONEVENT_TEXT
			GetDlgItem(IDC_CHECK_ONEVENT_TEXT)->SendMessage(BM_SETCHECK, TRUE, 0);
		}
	}
	if(pMsg->message==WM_KEYDOWN && GetKeyState(VK_CONTROL)<0){
		if(pMsg->wParam==VK_RETURN || pMsg->wParam=='S'){
			OnOK();
			return TRUE;
		}
		if(pMsg->wParam=='A'){
			m_eText.SetFocus();
			m_eText.SetSel(0,0xFFFF,1);
			return TRUE;
		}
	}
	return CResizeableDialog::PreTranslateMessage(pMsg);
}

CCalendarDlg2::~CCalendarDlg2()
{
	if(sLastMp3!=""){
		PlayMusic(sLastMp3,-1,"wc_test");
		sLastMp3="";
	}
	pDialogFont.Detach();
}
/*** Message handler table ***************************************************/
BEGIN_MESSAGE_MAP(CCalendarDlg2, CResizeableDialog)
//{{AFX_MSG_MAP(CCalendarDlg2)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_MEASUREITEM() 
ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR1, OnSelchangeMonthcalendar1)
ON_COMMAND(IDOK, OnOK)
ON_COMMAND(IDDEL, OnDel)
ON_COMMAND(IDCANCEL_REM, OnCancel)
ON_COMMAND(IDC_BUTTON_MP3, OnBTMp3)
ON_COMMAND(IDC_BUTTON_MP3_PLAY, OnBTMp3Play)
ON_COMMAND(IDC_BUTTON_APP, OnBTApp)
ON_COMMAND(IDC_BUTTON_REC, OnBTRec)
ON_COMMAND(IDC_BUTTON_TIME, OnBTSetTim)
ON_COMMAND(IDC_BUTTON_MOREOPTIONS, OnMoreOptions)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOLDDAY(ds,iDay) if(iDay>0 && iDay<32){(ds)|=(0x00000001<<(iDay-1));};
void CCalendarDlg2::OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDAYSTATE pDayState = reinterpret_cast<LPNMDAYSTATE>(pNMHDR);
	SYSTEMTIME& stStart=pDayState->stStart;
	int nMonths=pDayState->cDayState;
	static MONTHDAYSTATE* m_pMDS=0;
	if(m_pMDS){
		delete [] m_pMDS;
		m_pMDS=0;
	}
	m_pMDS=new MONTHDAYSTATE[nMonths];
	memset(m_pMDS,0,nMonths*sizeof(MONTHDAYSTATE));
	for (int i=0;i<nMonths; i++){
		m_pMDS[i] = (MONTHDAYSTATE)0;
		for(int j=1;j<32;j++){
			CString sDate=CDataXMLSaver::OleDate2Str(COleDateTime(stStart.wYear,stStart.wMonth,j,0,0,0));
			char szDateIn[64]={0};
			strcpy(szDateIn,sDate);
			BOOL bBold=(GetCountOnDate(szDateIn))>0;
			if(bBold){
				BOLDDAY(m_pMDS[i],j);
			}
		}
		stStart.wMonth++;
		if(stStart.wMonth>12){
			stStart.wMonth=1;
			stStart.wYear++;
		}
	}
	pDayState->prgDayState = m_pMDS;
	*pResult = 0;
}

BOOL bFileDialogOpened=FALSE;
BOOL OpenFileDialog(const char* szExtension, char* szPath, CWnd* pParent, BOOL bSkipFileCheck=FALSE)
{
	if(bFileDialogOpened){
		return FALSE;
	}
	if(szPath==NULL){
		return FALSE;
	}
	BOOL bRes=FALSE;
	bFileDialogOpened=TRUE;
	char szFileNameBuffer[32*1024]="";
	strcpy(szFileNameBuffer,bSkipFileCheck?(""):(szPath));
	CFileDialog dlg(TRUE, NULL, szFileNameBuffer, OFN_NODEREFERENCELINKS, szExtension, pParent);
	if(dlg.DoModal()==IDOK){
		bRes=TRUE;
		CString sNewFile=dlg.GetPathName();
		strcpy(szPath,sNewFile);
	}
	bFileDialogOpened=FALSE;
	if(!bRes && !bSkipFileCheck){
		// Пробуем без стартового имени файла...
		//DWORD dwErr=CommDlgExtendedError();
		//if(dwErr==12290)
		{
			return OpenFileDialog(szExtension, szPath, pParent, TRUE);
		}
	}
	return bRes;
}

void CCalendarDlg2::OnBTMp3Play()
{
	{// Звучим!!!
		char szTmp[MAX_PATH]={0};
		GetDlgItem(IDC_EDIT_MP3)->GetWindowText(szTmp,sizeof(szTmp));
		sLastMp3=szTmp;
		if(isFileExist(sLastMp3)){
			//GetDlgItem(IDC_BUTTON_MP3_PLAY)->SetWindowText("||");
			PlayMusic(sLastMp3,-1,"wc_test");
			PlayMusic(sLastMp3,0,"wc_test");
			//GetDlgItem(IDC_BUTTON_MP3_PLAY)->SetWindowText(">");
		}
	}
}

void CCalendarDlg2::OnBTMp3()
{
	char szTmp[MAX_PATH]={0};
	GetDlgItem(IDC_EDIT_MP3)->GetWindowText(szTmp,sizeof(szTmp));
	OpenFileDialog("Any files (*.*)|*.*||",szTmp,this);
	GetDlgItem(IDC_EDIT_MP3)->SetWindowText(szTmp);
	GetDlgItem(IDC_CHECK_ONEVENT_MP3)->SendMessage(BM_SETCHECK, 1, 0);
}

void CCalendarDlg2::OnBTApp()
{
	char szTmp[MAX_PATH]={0};
	GetDlgItem(IDC_EDIT_APP)->GetWindowText(szTmp,sizeof(szTmp));
	OpenFileDialog("Any files (*.*)|*.*||",szTmp,this);
	GetDlgItem(IDC_EDIT_APP)->SetWindowText(szTmp);
	GetDlgItem(IDC_CHECK_ONEVENT_APP)->SendMessage(BM_SETCHECK, 1, 0);
}

void CCalendarDlg2::OnBTRec()
{
	CMenu menu;
	menu.CreatePopupMenu();
	CPoint pt;
	CRect rt;
	GetDlgItem(IDC_BUTTON_REC)->GetWindowRect(&rt);
	pt.x=rt.right;
	pt.y=rt.top;
	AddMenuString(&menu,WM_USER+0,_lx(Format("RMX.UI.RECUR%i",0),Format("RECURRENCY%i",0)));
	AddMenuString(&menu,WM_USER+1,_lx(Format("RMX.UI.RECUR%i",1),Format("RECURRENCY%i",1)));
	AddMenuString(&menu,WM_USER+2,_lx(Format("RMX.UI.RECUR%i",2),Format("RECURRENCY%i",2)));
	AddMenuString(&menu,WM_USER+3,_lx(Format("RMX.UI.RECUR%i",3),Format("RECURRENCY%i",3)));
	AddMenuString(&menu,WM_USER+4,_lx(Format("RMX.UI.RECUR%i",4),Format("RECURRENCY%i",4)));
	CMenu menuXMin;
	menuXMin.CreatePopupMenu();
	AddMenuString(&menuXMin,WM_USER+50,_lx("RMX.UI.XMIN15","Every 15 min."));
	AddMenuString(&menuXMin,WM_USER+51,_lx("RMX.UI.XMIN30","Every 30 min."));
	AddMenuString(&menuXMin,WM_USER+52,_lx("RMX.UI.XMIN45","Every 45 min."));
	AddMenuString(&menuXMin,WM_USER+59,_lx("RMX.UI.XMANUAL","Specify period")+"...");
	AddMenuSubmenu(&menu,&menuXMin,_lx(Format("RMX.UI.RECUR%i",5),Format("RECURRENCY%i",5))+"...");
	CMenu menuXHour;
	menuXHour.CreatePopupMenu();
	AddMenuString(&menuXHour,WM_USER+60,_lx("RMX.UI.XHOUR1","Every hour"));
	AddMenuString(&menuXHour,WM_USER+61,_lx("RMX.UI.XHOUR2","Every 2 hours"));
	AddMenuString(&menuXHour,WM_USER+62,_lx("RMX.UI.XHOUR6","Every 6 hours"));
	AddMenuString(&menuXHour,WM_USER+63,_lx("RMX.UI.XHOUR12","Every 12 hours"));
	AddMenuString(&menuXHour,WM_USER+69,_lx("RMX.UI.XMANUAL","Specify period")+"...");
	AddMenuSubmenu(&menu,&menuXHour,_lx(Format("RMX.UI.RECUR%i",6),Format("RECURRENCY%i",6))+"...");
	int iNum=::TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
	if(iNum!=0){
		iRecurrency=iNum-WM_USER;
		if(iRecurrency>=50){
			if(iRecurrency==50){
				iRecurrency=5;
				dXPeriod=15;
			}
			if(iRecurrency==51){
				iRecurrency=5;
				dXPeriod=30;
			}
			if(iRecurrency==52){
				iRecurrency=5;
				dXPeriod=45;
			}
			if(iRecurrency==59){
				iRecurrency=5;
				if(dXPeriod==0){
					dXPeriod=15;
				}
				CDLGEnterNumber dlg;
				dlg.pdwNumber=&dXPeriod;
				dlg.DoModal();
				if(dXPeriod<=0){
					dXPeriod=15;
				}
			}
			if(iRecurrency==60){
				iRecurrency=6;
				dXPeriod=1;
			}
			if(iRecurrency==61){
				iRecurrency=6;
				dXPeriod=2;
			}
			if(iRecurrency==62){
				iRecurrency=6;
				dXPeriod=6;
			}
			if(iRecurrency==63){
				iRecurrency=6;
				dXPeriod=12;
			}
			if(iRecurrency==69){
				iRecurrency=6;
				if(dXPeriod==0){
					dXPeriod=1;
				}
				CDLGEnterNumber dlg;
				dlg.pdwNumber=&dXPeriod;
				dlg.DoModal();
				if(dXPeriod<=0){
					dXPeriod=1;
				}
			}
		}
	}
	SetInfoText();
}
/*
void CCalendarDlg2::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	HMENU hMenu = AfxGetThreadState()->m_hTrackingMenu;
	CMenu	*pMenu = CMenu::FromHandle(hMenu);
	pMenu->MeasureItem(lpMeasureItemStruct);
	
	CMenuX::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}*/

class CMenuX;
BOOL ModifyOnRecurrency(int iRecurrency,SYSTEMTIME& EventTime, DWORD lAddParam);
CMenuX* pGlobalMenuX=0;
CWnd* pParentWnd=0;
CMap<DWORD,DWORD,CString, CString> aIDsMap;
class CMenuX:public CMenu
{
	CRect rtTitle;
public:
	// Override DrawItem() to implement drawing for an owner-draw CMenu object.
	void DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		COLORREF clrPrevText, clrPrevBkgnd; 
		COLORREF crMenu = GetSysColor(COLOR_MENU);//(COLORREF)lpDIS->itemData; // RGB in item data
		COLORREF crMenuH = GetSysColor(COLOR_ACTIVECAPTION);
		COLORREF crMenuT = GetSysColor(COLOR_MENUTEXT);
		if (lpDIS->itemAction & ODA_DRAWENTIRE)
		{
			// Paint the color item in the color requested
			CBrush br(crMenu);
			pDC->FillRect(&lpDIS->rcItem, &br);
		}
		if (lpDIS->itemState & ODS_DEFAULT)
		{
			// Paint the color item in the color requested
			crMenuT = GetSysColor(COLOR_WINDOW);
			crMenu = GetSysColor(COLOR_WINDOWTEXT);
			CBrush br(crMenu);
			pDC->FillRect(&lpDIS->rcItem, &br);
		}
		clrPrevText = SetTextColor(lpDIS->hDC, crMenuT); 
		clrPrevBkgnd = SetBkColor(lpDIS->hDC, crMenu); 
		CString sz=aIDsMap[lpDIS->itemID];
		sz.TrimLeft();
		if(pGlobalMenuX){
			if(sz!=""){
				CFont* pf=pParentWnd->GetFont();//CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
				if(pf){
					CFont* hfntOld = (CFont*)pDC->SelectObject(pf); 
					pDC->DrawText(sz,-1,&lpDIS->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
					pDC->SelectObject(hfntOld);
				}
			}
		}
		if(rtTitle.IsRectEmpty() && lpDIS->itemID==2){
			rtTitle.CopyRect(&lpDIS->rcItem);
		}
		if(!rtTitle.IsRectEmpty()){
			CFont* pf=pParentWnd->GetFont();//CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			if(pf){
				SetTextColor(lpDIS->hDC, GetSysColor(COLOR_MENUTEXT));
				SetBkColor(lpDIS->hDC, GetSysColor(COLOR_MENU)); 
				CString sz2=_lx("RMX.UI.CLICKONHOUR","Click on hour");
				CFont* hfntOld = (CFont*)pDC->SelectObject(pf); 
				pDC->DrawText(sz2,-1,&rtTitle,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
				pDC->SelectObject(hfntOld);
			}
		}
		if ((lpDIS->itemState & ODS_SELECTED) &&
			(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)) && sz!="" && lpDIS->itemID>=WM_USER)
		{
			// item has been selected - hilite frame
			CBrush br(crMenuH);
			pDC->FrameRect(&lpDIS->rcItem, &br);
		}
		
		if (!(lpDIS->itemState & ODS_SELECTED) &&
			(lpDIS->itemAction & ODA_SELECT))
		{
			// Item has been de-selected -- remove frame
			CBrush br(crMenu);
			pDC->FrameRect(&lpDIS->rcItem, &br);
		}
		SetTextColor(lpDIS->hDC, clrPrevText); 
		SetBkColor(lpDIS->hDC, clrPrevBkgnd); 
	}
	
	
	void MeasureItem(LPMEASUREITEMSTRUCT lpmis)
	{
		{
			CFont* pf=pParentWnd->GetFont();//CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			if(pf){
				HDC hdc = GetDC(pParentWnd->GetSafeHwnd());
				HFONT hfntOld = (HFONT)SelectObject(hdc, *pf); 
				SIZE size;
				CString sTxt="HHH";//aIDsMap[lpmis->itemID];
				GetTextExtentPoint32(hdc, sTxt, sTxt.GetLength(), &size); 
				//CRect rNewBounds(0,0,10,10);::DrawText(hdc, sTxt, -1, &rNewBounds, DT_CALCRECT);
				if(size.cx>size.cy){
					size.cy=size.cx;
				}else{
					size.cx=size.cy;
				}
				lpmis->itemWidth = UINT(size.cx*0.7);
				lpmis->itemHeight = UINT(size.cx*1.3);
				
				SelectObject(hdc, hfntOld);
				ReleaseDC(pParentWnd->GetSafeHwnd(), hdc);
			}
		}
	}
};

void CCalendarDlg2::OnMoreOptions()
{
	CMenu menu;
	menu.CreatePopupMenu();
	CPoint pt;
	CRect rt;
	GetDlgItem(IDC_BUTTON_MOREOPTIONS)->GetWindowRect(&rt);
	pt.x=rt.right;
	pt.y=rt.top;
	AddMenuString(&menu,101,_lx("RMX.UI.IGNOREONSKIP","Ignore if missed"),(CBitmap*)(NULL),rem.bActIgnoreOnSkip);
	AddMenuString(&menu,102,_lx("RMX.UI.ACLOSEPOPUP","Auto-close popup"),(CBitmap*)(NULL),rem.bActAutoClosePopup);
	

	int iNum=::TrackPopupMenu(menu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
	if(iNum==101){
		rem.bActIgnoreOnSkip=rem.bActIgnoreOnSkip?FALSE:TRUE;
	}
	if(iNum==102){
		rem.bActAutoClosePopup=rem.bActAutoClosePopup?FALSE:TRUE;
	}
}

void CCalendarDlg2::OnBTSetTim()
{
	pParentWnd=this;
	CMenuX menu;
	pGlobalMenuX=&menu;
	menu.CreatePopupMenu();
	CPoint pt;
	CRect rt;
	GetDlgItem(IDC_BUTTON_TIME)->GetWindowRect(&rt);
	pt.x=rt.right;
	pt.y=rt.top;
	aIDsMap.RemoveAll();
	SYSTEMTIME sysTime;
	GetDlgItem(IDC_DATETIMEPICKER_TIME)->SendMessage(DTM_GETSYSTEMTIME, 0, (LPARAM)&sysTime);
	aIDsMap.SetAt(0,"   ");
	aIDsMap.SetAt(WM_USER+11 ,"11");
	aIDsMap.SetAt(WM_USER+10 ,"10");
	aIDsMap.SetAt(WM_USER+9 ,"9");
	aIDsMap.SetAt(WM_USER+8 ,"8");
	aIDsMap.SetAt(WM_USER+7 ,"7");
	aIDsMap.SetAt(WM_USER+6 ,"6");
	aIDsMap.SetAt(WM_USER+5 ,"5");
	aIDsMap.SetAt(WM_USER+4 ,"4");
	aIDsMap.SetAt(WM_USER+3 ,"3");
	aIDsMap.SetAt(WM_USER+2 ,"2");
	aIDsMap.SetAt(WM_USER+1 ,"1");
	aIDsMap.SetAt(WM_USER+0 ,"12");
	aIDsMap.SetAt(WM_USER+102,_lx("RMX.UI.ADD15MINSHORT","+15m"));
	aIDsMap.SetAt(WM_USER+103,_lx("RMX.UI.ADD30INSHORT","+30m"));
	aIDsMap.SetAt(WM_USER+104,_lx("RMX.UI.ADD1HOURSHORT","+1h"));
	AddMenuString(&menu,		  0,"  ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+10 ,"10",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+9  ," 9",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+8  ," 8",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  0,"  ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+11 ,"11",MF_MENUBREAK| MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  1,"  ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+102,_lx("RMX.UI.ADD15MINSHORT","+15m"),MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  0,"  ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+7  ," 7",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+0  ,"12",MF_MENUBREAK| MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  2," ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+103,_lx("RMX.UI.ADD30INSHORT","+30m"),MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  0,"  ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+6  ," 6",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+1  ," 1",MF_MENUBREAK| MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  3,"  ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+104,_lx("RMX.UI.ADD1HOURSHORT","+1h"),MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  0,"  ",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+5  ," 5",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  0,"  ",MF_MENUBREAK| MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+2  ," 2",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+3  ," 3",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,WM_USER+4  ," 4",MF_OWNERDRAW,0,0);
	AddMenuString(&menu,		  0,"  ",MF_OWNERDRAW,0,0);
	menu.SetDefaultItem(WM_USER+(sysTime.wHour%12),0);
	int iNum=menu.TrackPopupMenu(TPM_RETURNCMD|TPM_NONOTIFY|TPM_CENTERALIGN|TPM_VCENTERALIGN, pt.x, pt.y, this, NULL);
	pGlobalMenuX=0;
	int iTime=iNum-WM_USER;
	if(iTime>=0){
		if(iTime>=100){
			ModifyOnRecurrency(iTime,sysTime,0);
		}else{
			sysTime.wHour=iTime+(sysTime.wHour>=12?12:0);
		}
		GetDlgItem(IDC_DATETIMEPICKER_TIME)->SendMessage(DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&sysTime);
	}
}

void CCalendarDlg2::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if(pGlobalMenuX){
		pGlobalMenuX->MeasureItem(lpMeasureItemStruct);
	}
}

CWnd* AfxWPGetMainWnd()
{
	return 0;
}