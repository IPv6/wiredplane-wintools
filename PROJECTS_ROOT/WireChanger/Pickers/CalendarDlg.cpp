/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Implementation of the class "CCalendarDlg"
	  (handy calendar)
	  
		$Log$
		
******************************************************************************/

#include "stdafx.h"
#include "CalendarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*** Definition of the class "CAboutDlg" *************************************/
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*** Constructor *************************************************************/
CAboutDlg::CAboutDlg(): CDialog(IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

/*** Protected member functions **********************************************/

/*** Data exchange: member variables <--> controls ***************************/
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

/*** Message handler table ***************************************************/
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*** Definition of the class "CCalendarDlg" **********************************/

/*** Constructor *************************************************************/
CCalendarDlg::CCalendarDlg(CWnd* pParent): CResizeableDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalendarDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN);
	dtSelected.SetStatus(COleDateTime::invalid);
}

/*** Protected member functions **********************************************/

/*** Data exchange: member variables <--> controls ***************************/
void CCalendarDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizeableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarDlg)
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_mcMonthCal);
	//}}AFX_DATA_MAP
}

/*** Will be called before the first display of the dialog *******************/
BOOL CCalendarDlg::OnInitDialog()
{
	m_mcMonthCal.SetHolidaysFromFile(_T("HOLIDAYS.INI"));
	CResizeableDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetWindowText(_l(sTitle));
	GetDlgItem(IDOK)->SetWindowText(_l(sButton));
	// Register resizable controls
	RegisterControl(IDC_MONTHCALENDAR1, ATTACH_TO_ALL_EDGES);
	//RegisterControl(IDC_DAYS_SELECTED, ATTACH_TO_BOTTOM_EDGE);
	RegisterControl(IDOK, ATTACH_TO_BOTTOM_EDGE | CENTER_HORIZONTAL);
	
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
		MoveWindow(rect);
		ThisSizeIsMaxSize();
	}
	DisplayNameOfSelectedDay();
	//m_mcMonthCal.SetMaxSelCount(1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*** Draw icon in minimized mode *********************************************/
void CCalendarDlg::OnPaint() 
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
HCURSOR CCalendarDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*** Selection in calendar control has changed *******************************/
void CCalendarDlg::OnSelchangeMonthcalendar1(NMHDR*, LRESULT* pResult) 
{
	DisplayNameOfSelectedDay();
	*pResult = 0;
}

/*** System menu entry has been selected *************************************/
void CCalendarDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		return;
	}
	if(nID==SC_CLOSE){
		OnCancel();
		return;
	}
	CResizeableDialog::OnSysCommand(nID, lParam);
}

void CCalendarDlg::OnOK()
{
	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)::SendMessage(m_mcMonthCal, MCM_GETCURSEL, 0, (LPARAM) &sysTime);
	if(bResult){
		dtSelected=COleDateTime(sysTime.wYear,sysTime.wMonth,sysTime.wDay,0,0,0);
	}
	CResizeableDialog::OnOK();
}


BOOL CCalendarDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_LBUTTONDBLCLK){
		OnOK();
		return TRUE;
	}
	return CResizeableDialog::PreTranslateMessage(pMsg);
}
/*** Display name of selected day ********************************************/
void CCalendarDlg::DisplayNameOfSelectedDay()
{
	return;
	/*
	COleDateTime ctMinRange, ctMaxRange;
	CString      str;
	
	if (m_mcMonthCal.GetSelRange(ctMinRange, ctMaxRange))
	{
		COleDateTimeSpan ts    = ctMaxRange - ctMinRange;
		long             nDays = ts.GetDays() + 1;
		
		if (nDays > 1)
			str.FormatMessage(IDS_DAYS_SELECTED, nDays);
		else
		{
			str = m_mcMonthCal.GetNameOfDay(ctMinRange);
			if (str.IsEmpty())
				VERIFY(str.LoadString(IDS_1_DAY_SELECTED));
			else
				str.Replace(_T("\r\n"), _T(", "));
		}
	}
	GetDlgItem(IDC_DAYS_SELECTED)->SetWindowText(str);
	*/
}

/*** Message handler table ***************************************************/
BEGIN_MESSAGE_MAP(CCalendarDlg, CResizeableDialog)
//{{AFX_MSG_MAP(CCalendarDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR1, OnSelchangeMonthcalendar1)
ON_COMMAND(IDOK, OnOK)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
