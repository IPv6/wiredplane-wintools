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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/*** Definition of the class "CCalendarDlg2" **********************************/

/*** Constructor *************************************************************/
CCalendarDlg2::CCalendarDlg2(CWnd* pParent): CResizeableDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalendarDlg2)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN);
	dtSelected.SetStatus(COleDateTime::invalid);
	bINewLine=0;
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
BOOL CCalendarDlg2::OnInitDialog()
{
	m_mcMonthCal.SetHolidaysFromFile(_T("HOLIDAYS.INI"));
	m_mcMonthCal.pDelegates=this;
	CResizeableDialog::OnInitDialog();
	// Парсим файл
	CString sContent;
	ReadFile(sMainFile,sContent);
	sContent.Replace("\r\n","\n");
	sContent=CString("\n")+sContent+"\n";
	while(sContent!=""){
		CString sLine=sContent.SpanExcluding("\n");
		if(sLine!=""){
			aItems.Add(sLine);
			if(sContent.GetLength()>sLine.GetLength()){
				sContent=sContent.Mid(sLine.GetLength()+1);
			}else{
				sContent="";
			}
		}
		sContent.TrimLeft();
	}
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
	GetDlgItem(IDC_STATIC1)->SetWindowText(_l(sStatic1));
	GetDlgItem(IDC_STATIC2)->SetWindowText(_l(sStatic2));
	SetWindowText(_l(sTitle));
	GetDlgItem(IDOK)->SetWindowText(_l(sButton));
	GetDlgItem(IDCLEAR)->SetWindowText(_l("Clear field"));
	// Register resizable controls
	RegisterControl(IDC_STATIC1, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_MONTHCALENDAR1, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_STATIC2, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_EDIT_TXT, ATTACH_TO_TOP_EDGE | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_STATIC3, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDCLEAR, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
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
		/*
		MoveWindow(rect);
		ThisSizeIsMaxSize();

		CRect rtBegin(rect);
		MoveWindow(rtBegin);
		*/
	}
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	BOOL bResult = (BOOL)::SendMessage(m_mcMonthCal, MCM_SETCURSEL, 0, (LPARAM) &sysTime);
	SetTextForDay(&sysTime);
	return TRUE;  // return TRUE  unless you set the focus to a control
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
		dtSelected=COleDateTime(sysTime.wYear,sysTime.wMonth,sysTime.wDay,0,0,0);
		SetTextForDay(&sysTime);
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

void CCalendarDlg2::OnClear()
{
	m_eText.SetWindowText("");
}

void CCalendarDlg2::OnOK()
{
	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)::SendMessage(m_mcMonthCal, MCM_GETCURSEL, 0, (LPARAM) &sysTime);
	if(bResult){
		dtSelected=COleDateTime(sysTime.wYear,sysTime.wMonth,sysTime.wDay,0,0,0);
		SetTextForDay(0);
	}
	CResizeableDialog::OnOK();
	CString sNewContent;
	for(int i=0;i<aItems.GetSize();i++){
		sNewContent+=aItems[i];
		sNewContent+="\r\n";
	}
	SaveFile(sMainFile,sNewContent);
}


BOOL CCalendarDlg2::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN && GetKeyState(VK_CONTROL)<0){
		if(pMsg->wParam==VK_RETURN || pMsg->wParam=='S'){
			OnOK();
			return TRUE;
		}
		if(pMsg->wParam=='A'){
			GetDlgItem(IDC_EDIT_TXT)->SendMessage(EM_SETSEL,0,(LPARAM)-1);
			return TRUE;
		}
	}
	return CResizeableDialog::PreTranslateMessage(pMsg);
}

/*** Message handler table ***************************************************/
BEGIN_MESSAGE_MAP(CCalendarDlg2, CResizeableDialog)
//{{AFX_MSG_MAP(CCalendarDlg2)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR1, OnSelchangeMonthcalendar1)
ON_COMMAND(IDOK, OnOK)
ON_COMMAND(IDCLEAR, OnClear)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCalendarDlg2::SetTextForDay(SYSTEMTIME* sysTime)
{
	static BOOL bFirst=1;
	static CString sDovesok;
	static COleDateTime dtCurrent;
	if(!bFirst && dtCurrent.GetStatus()!=COleDateTime::invalid){
		CString sThisDayNKey=Format(sKey,dtCurrent.GetDay(),dtCurrent.GetMonth(),dtCurrent.GetYear());
		CString sCurDay;
		m_eText.GetWindowText(sCurDay);
		sCurDay.TrimLeft();
		sCurDay.TrimRight();
		if(sCurDay!=""){
			sCurDay.Replace("\r\n","\n");
			if(sDovesok!=""){
				sCurDay+=Format("[?CDATA{%s}?DATAC:)]",sDovesok);
			}
			if(!bINewLine){
				sCurDay.Replace("\n","<br>");
			}
			CString sContent=CString("\n")+sCurDay+"\n";
			while(sContent!=""){
				CString sLine=sContent.SpanExcluding("\n");
				if(sLine!=""){
					aItems.Add(sThisDayNKey+sLine);
					if(sContent.GetLength()>sLine.GetLength()){
						sContent=sContent.Mid(sLine.GetLength()+1);
					}else{
						sContent="";
					}
				}
				sContent.TrimLeft();
			}
		}
		sDovesok="";
	}
	bFirst=0;
	if(sysTime){
		COleDateTime dtTime=COleDateTime(sysTime->wYear,sysTime->wMonth,sysTime->wDay,0,0,0);
		char szTmp[256]={0};
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,sysTime,0,szTmp,sizeof(szTmp));
		GetDlgItem(IDC_STATIC3)->SetWindowText(Format("%s: %s",_l("Current date"),szTmp));
		CString sThisDayNKey=Format(sKey,dtTime.GetDay(),dtTime.GetMonth(),dtTime.GetYear());
		dtCurrent=dtTime;
		CString sDayNote;
		for(int i=aItems.GetSize()-1;i>=0;i--){
			if(aItems[i].Find(sThisDayNKey)==0){
				if(sDayNote!=""){
					sDayNote+="\r\n";
				}
				sDayNote+=aItems[i].Mid(sThisDayNKey.GetLength());
				aItems.RemoveAt(i);
			}
		}
		sDovesok=CDataXMLSaver::GetInstringPart("[?CDATA{","}?DATAC:)]",sDayNote);
		sDayNote.Replace(Format("[?CDATA{%s}?DATAC:)]",sDovesok),"");
		sDayNote.Replace("<br>","\r\n");
		m_eText.SetWindowText(sDayNote);
	}
}

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
			CString sThisDayNKey=Format(sKey,j,stStart.wMonth,stStart.wYear);
			BOOL bBold=0;
			for(int i2=aItems.GetSize()-1;i2>=0;i2--){
				if(aItems[i2].Find(sThisDayNKey)==0){
					bBold=1;
					break;
				}
			}
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

CWnd* AfxWPGetMainWnd()
{
	return 0;
}