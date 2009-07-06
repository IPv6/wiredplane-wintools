// DLG_Reminder.cpp : implementation file
//

#include "stdafx.h"
#include "WireNote.h"
#include "DLG_Reminder.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DLG_Reminder dialog


DLG_Reminder::DLG_Reminder(CWnd* pParent /*=NULL*/)
	: CDialog(), m_Watch(this), Track(objSettings.bDoModalInProcess)
{
	//{{AFX_DATA_INIT(DLG_Reminder)
	m_Month_Exact = -1;
	m_Year_Exact = -1;
	m_Day_Exact = -1;
	m_Active_Reminder = FALSE;
	m_EditDay = _T("");
	m_EditMonth = _T("");
	m_EditYear = _T("");
	m_Time = 0;
	m_ReminderTitle = _T("");
	m_EditActionPar = _T("");
	m_ComboWDaysList = -1;
	m_ComboTimeDistr = -1;
	m_RepeatedTime = FALSE;
	m_ComboHourList = _T("");
	m_AmPm = -1;
	m_ReminderText = _T("");
	//}}AFX_DATA_INIT
	reminder=NULL;
	iStyle=1;
	dLastHour=-1;
	iLastMin=-1;
	bThisIsNewReminder=FALSE;
}

void DLG_Reminder::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

void DLG_Reminder::ExitRemWindow(int iCode)
{
	if(reminder){
		reminder->dlg=NULL;
	}
	DestroyWindow();//EndDialog(iCode);
}

DLG_Reminder::~DLG_Reminder()
{
	if(reminder){
		reminder->dlg=NULL;
	}
}

void DLG_Reminder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DLG_Reminder)
	DDX_Control(pDX, IDC_COMBO_ICON, m_IconCombo);
	DDX_Control(pDX, IDC_COMBO_ACTION, m_ComboActions);
	DDX_Control(pDX, IDC_MONTHCALENDAR, m_Calendar);
	DDX_Control(pDX, IDC_WATCH, m_Watch);
	DDX_Radio(pDX, IDC_RADIO_MON_EXACT, m_Month_Exact);
	DDX_Radio(pDX, IDC_RADIO_YEAR_EXACT, m_Year_Exact);
	DDX_Radio(pDX, IDC_RADIO_DAY_EXACT, m_Day_Exact);
	DDX_Check(pDX, IDC_ACTIVE, m_Active_Reminder);
	DDX_Text(pDX, IDC_EDIT_DAY, m_EditDay);
	DDX_Text(pDX, IDC_EDIT_MONTH, m_EditMonth);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_EditYear);
	DDX_DateTimeCtrl(pDX, IDC_TIME, m_Time);
	DDX_Text(pDX, IDC_TITLE, m_ReminderTitle);
	DDX_Text(pDX, IDC_EDIT_ACTION_PAR, m_EditActionPar);
	DDX_CBIndex(pDX, IDC_COMBO_WDAYS, m_ComboWDaysList);
	DDX_CBIndex(pDX, IDC_COMBO_TIME_DISCR, m_ComboTimeDistr);
	DDX_Check(pDX, IDC_TIME_EVERY, m_RepeatedTime);
	DDX_CBString(pDX, IDC_COMBO_HOUR_LIST, m_ComboHourList);
	DDX_Radio(pDX, IDC_RADIO_AM, m_AmPm);
	DDX_Text(pDX, IDC_EDIT_ACTION_PAR2, m_ReminderText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DLG_Reminder, CDialog)
	//{{AFX_MSG_MAP(DLG_Reminder)
	ON_BN_CLICKED(IDC_BUTTON_SAVEEXIT, OnButtonSaveexit)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION, OnSelchangeComboAction)
	ON_EN_CHANGE(IDC_EDIT_ACTION_PAR, OnChangeEditActionPar)
	ON_BN_CLICKED(IDC_BUTTON_ACTION_PAR_CHOSER, OnButtonActionParChoser)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_DONTSAVEEXIT, OnButtonDontsaveexit)
	ON_NOTIFY(MCN_SELECT, IDC_MONTHCALENDAR, OnSelectMonthcalendar)
	ON_BN_CLICKED(IDC_WATCH, OnWatch)
	ON_BN_CLICKED(IDC_RADIO_AM, OnRadioAm)
	ON_BN_CLICKED(IDC_RADIO_PM, OnRadioPm)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME, OnDatetimechangeTime)
	ON_BN_CLICKED(IDC_BUTTON_CVIEW, OnButtonCview)
	ON_BN_CLICKED(IDC_RADIO_DAY_EVERY_WDAY, OnRadioDayEveryWday)
	ON_BN_CLICKED(IDC_RADIO_DAY_EVERY, OnRadioDayEvery)
	ON_BN_CLICKED(IDC_RADIO_MON_EVERY, OnRadioMonEvery)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	ON_CBN_DBLCLK(IDC_COMBO_ACTION, OnDblclkComboAction)
	ON_BN_CLICKED(IDC_BUTTON_A15, OnButtonA15)
	ON_BN_CLICKED(IDC_BUTTON_A60, OnButtonA60)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ACTIVE, OnActiveChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define TIME_WIDTH	75
/////////////////////////////////////////////////////////////////////////////
// DLG_Reminder message handlers
void DLG_Reminder::SetRemText(const char* szNewText)
{
	m_ReminderText=szNewText;
	m_ReminderText.Replace("\n","\r\n");
	(GetDlgItem(IDC_EDIT_ACTION_PAR2))->SetWindowText(m_ReminderText);
}

BOOL DLG_Reminder::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_Reminder");
	Sizer.SetOptions(STICKABLE|SIZEABLE);
	CRect minRect(0,0,DIALOG_MINXSIZEW,DIALOG_MINYSIZE);
	Sizer.SetMinRect(minRect);
	SetWindowSize(this,CSize(150,150),IDC_WATCH);
	SetWindowSize(this,CSize(150,150),IDC_MONTHCALENDAR);
	SetWindowSize(this,CSize(30,20),IDC_BUTTON_A60);
	SetWindowSize(this,CSize(40,20),IDC_BUTTON_A15);
	Sizer.AddDialogElement(IDC_STATIC_DAYF,heightSize);
	Sizer.AddDialogElement(IDC_STATIC_MONF,heightSize);
	Sizer.AddDialogElement(IDC_STATIC_YEARF,heightSize);
	Sizer.AddDialogElement(IDC_STATIC_TIMEF,heightSize);
	Sizer.AddDialogElement(IDC_TITLE,heightSize);
	Sizer.AddDialogElement(IDC_COMBO_ICON,heightSize);
	Sizer.AddDialogElement(IDC_STATIC_TITLE,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_ACTIVE,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_COMBO_ACTION,heightSize);
	Sizer.AddDialogElement(IDC_EDIT_ACTION_PAR,heightSize);
	Sizer.AddDialogElement(IDC_EDIT_ACTION_PAR2,0);
	Sizer.AddDialogElement(IDC_BUTTON_ACTION_PAR_CHOSER,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_SAVEEXIT,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_DONTSAVEEXIT,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_DELETE,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_TEST,widthSize|heightSize|hidable);
	Sizer.AddDialogElement(IDC_RADIO_DAY_EXACT,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RADIO_DAY_EVERY,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RADIO_DAY_EVERY_WDAY,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_EDIT_DAY,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_COMBO_WDAYS,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RADIO_MON_EXACT,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RADIO_MON_EVERY,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_EDIT_MONTH,widthSize|heightSize);

	Sizer.AddDialogElement(IDC_TIME,heightSize);
	Sizer.AddDialogElement(IDC_RADIO_YEAR_EXACT,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RADIO_YEAR_EVERY,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_EDIT_YEAR,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_TIME_EVERY,heightSize);
	Sizer.AddDialogElement(IDC_COMBO_TIME_DISCR,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_COMBO_HOUR_LIST,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_MONTHCALENDAR,heightSize);
	Sizer.AddDialogElement(IDC_WATCH,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RADIO_AM,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RADIO_PM,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_CVIEW,heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_A15,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BUTTON_A60,widthSize|heightSize);

	//
	GetDlgItem(IDC_BUTTON_SAVEEXIT)->SetWindowText(_l("Ok"));
	GetDlgItem(IDC_BUTTON_DONTSAVEEXIT)->SetWindowText(_l("Cancel"));
	GetDlgItem(IDC_BUTTON_DELETE)->SetWindowText(_l("Delete"));
	GetDlgItem(IDC_BUTTON_TEST)->SetWindowText(_l("Test"));
	GetDlgItem(IDC_TIME_EVERY)->SetWindowText(_l("Every"));
	GetDlgItem(IDC_RADIO_YEAR_EVERY)->SetWindowText(_l("Every year"));
	GetDlgItem(IDC_RADIO_MON_EVERY)->SetWindowText(_l("Every month"));
	GetDlgItem(IDC_RADIO_DAY_EVERY)->SetWindowText(_l("Every day"));
	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(_l("Reminder title"));
	GetDlgItem(IDC_ACTIVE)->SetWindowText(_l("Active reminder"));
	GetDlgItem(IDC_RADIO_DAY_EVERY_WDAY)->SetWindowText(_l("Every"));
	GetDlgItem(IDC_STATIC_TIMEF)->SetWindowText(_l("Time"));
	GetDlgItem(IDC_STATIC_MONF)->SetWindowText(_l("Month"));
	GetDlgItem(IDC_STATIC_DAYF)->SetWindowText(_l("Day"));
	GetDlgItem(IDC_STATIC_YEARF)->SetWindowText(_l("Year"));
	GetDlgItem(IDC_BUTTON_A15)->SetWindowText(_l("+15m"));
	GetDlgItem(IDC_BUTTON_A60)->SetWindowText(_l("+1h"));
	// Правила выравнивания
	Sizer.AddLeftTopAlign(IDC_STATIC_TITLE,0,4);
	Sizer.AddTopAlign(IDC_ACTIVE,0,4);
	Sizer.AddRightAlign(IDC_ACTIVE,0,rightPos,-1);
	Sizer.AddRightBottomAlign(IDC_BUTTON_SAVEEXIT,0,-1);
	Sizer.AddLeftBottomAlign(IDC_BUTTON_CVIEW,0,1);
	Sizer.AddTopAlign(IDC_TITLE,IDC_STATIC_TITLE,bottomPos,3);
	Sizer.AddTopAlign(IDC_COMBO_ICON,IDC_STATIC_TITLE,bottomPos,1);
	Sizer.AddLeftAlign(IDC_COMBO_ICON,0,leftPos,1);
	Sizer.AddRightAlign(IDC_TITLE,0,rightPos,1);
	Sizer.AddLeftAlign(IDC_TITLE,IDC_COMBO_ICON,rightPos,1);
	Sizer.AddBottomAlign(IDC_BUTTON_DONTSAVEEXIT,0,bottomPos,-1);
	Sizer.AddBottomAlign(IDC_BUTTON_DELETE,0,bottomPos,-1);
	Sizer.AddBottomAlign(IDC_BUTTON_TEST,0,bottomPos,-1);
	Sizer.AddRightAlign(IDC_BUTTON_DONTSAVEEXIT,IDC_BUTTON_SAVEEXIT,leftPos,-1);
	Sizer.AddRightAlign(IDC_BUTTON_TEST,IDC_BUTTON_DONTSAVEEXIT,leftPos,-1);
	Sizer.AddRightAlign(IDC_BUTTON_DELETE,IDC_BUTTON_TEST,leftPos,-1);
	Sizer.AddRightAlign(IDC_BUTTON_CVIEW,IDC_BUTTON_DELETE,leftPos,-1);
	Sizer.AddBottomAlign(IDC_BUTTON_ACTION_PAR_CHOSER,IDC_BUTTON_SAVEEXIT,topPos,-3);
	Sizer.AddBottomAlign(IDC_COMBO_ACTION,IDC_BUTTON_SAVEEXIT,topPos,-3);
	Sizer.AddBottomAlign(IDC_EDIT_ACTION_PAR,IDC_BUTTON_SAVEEXIT,topPos,-3);
	Sizer.AddRightAlign(IDC_BUTTON_ACTION_PAR_CHOSER,0,rightPos,-1);
	Sizer.AddLeftAlign(IDC_COMBO_ACTION,0,leftPos,1);
	Sizer.AddRightAlign(IDC_COMBO_ACTION,0,topCenter,30);
	Sizer.AddRightAlign(IDC_EDIT_ACTION_PAR,IDC_BUTTON_ACTION_PAR_CHOSER,leftPos,-1);
	Sizer.AddLeftAlign(IDC_EDIT_ACTION_PAR,0,topCenter,31);
	Sizer.AddRightAlign(IDC_EDIT_ACTION_PAR2,0,-2);
	Sizer.AddLeftAlign(IDC_EDIT_ACTION_PAR2,0,2);
	Sizer.AddBottomAlign(IDC_EDIT_ACTION_PAR2,IDC_EDIT_ACTION_PAR,topPos,-4);
	{//Расширенный вид
		MainRulePos=Sizer.AddGotoRule(iStyle);
		Sizer.AddGotoLabel(0);
		Sizer.AddShowRule(IDC_STATIC_MONF);
		Sizer.AddShowRule(IDC_STATIC_YEARF);
		Sizer.AddShowRule(IDC_STATIC_DAYF);
		Sizer.AddShowRule(IDC_STATIC_TIMEF);
		Sizer.AddShowRule(IDC_STATIC_MONF);
		Sizer.AddShowRule(IDC_STATIC_YEARF);
		Sizer.AddShowRule(IDC_STATIC_DAYF);
		Sizer.AddShowRule(IDC_STATIC_TIMEF);
		Sizer.AddShowRule(IDC_RADIO_DAY_EXACT);
		Sizer.AddShowRule(IDC_RADIO_MON_EXACT);
		Sizer.AddShowRule(IDC_RADIO_YEAR_EXACT);
		Sizer.AddShowRule(IDC_RADIO_DAY_EVERY);
		Sizer.AddShowRule(IDC_RADIO_MON_EVERY);
		Sizer.AddShowRule(IDC_RADIO_YEAR_EVERY);
		Sizer.AddShowRule(IDC_RADIO_DAY_EVERY_WDAY);
		Sizer.AddShowRule(IDC_COMBO_WDAYS);
		Sizer.AddShowRule(IDC_EDIT_DAY);
		Sizer.AddShowRule(IDC_EDIT_MONTH);
		Sizer.AddShowRule(IDC_EDIT_YEAR);
		// Скрываем лишнее
		Sizer.AddHideRule(IDC_MONTHCALENDAR);
		Sizer.AddHideRule(IDC_WATCH);
		Sizer.AddHideRule(IDC_BUTTON_A15);
		Sizer.AddHideRule(IDC_BUTTON_A60);
		Sizer.AddHideRule(IDC_RADIO_AM);
		Sizer.AddHideRule(IDC_RADIO_PM);
		Sizer.AddSideCenterAlign(IDC_TIME,IDC_STATIC_TIMEF,sideCenter,6);
		Sizer.AddTopAlign(IDC_EDIT_ACTION_PAR2,IDC_STATIC_TIMEF,bottomPos,4);
		Sizer.AddSideCenterAlign(IDC_TIME_EVERY,IDC_TIME,0);
		Sizer.AddLeftAlign(IDC_TIME,IDC_STATIC_TIMEF,leftPos,8);
		Sizer.AddRightAlign(IDC_TIME,IDC_TIME,leftPos,TIME_WIDTH);
		Sizer.AddSideCenterAlign(IDC_COMBO_HOUR_LIST,IDC_TIME,0);
		Sizer.AddSideCenterAlign(IDC_COMBO_TIME_DISCR,IDC_TIME,0);
		Sizer.AddRightAlign(IDC_COMBO_TIME_DISCR,IDC_STATIC_TIMEF,rightPos,-8);
		Sizer.AddRightAlign(IDC_COMBO_HOUR_LIST,IDC_COMBO_TIME_DISCR,leftPos,-4);
		Sizer.AddRightAlign(IDC_TIME_EVERY,IDC_COMBO_HOUR_LIST,leftPos,-4);
		Sizer.AddLeftAlign(IDC_TIME_EVERY,IDC_TIME,rightPos,4);
		Sizer.AddGotoRule(100);
	}
	{// Упрощенный вид
		Sizer.AddGotoLabel(1);
		// Убираем лишнее
		Sizer.AddHideRule(IDC_STATIC_MONF);
		Sizer.AddHideRule(IDC_STATIC_YEARF);
		Sizer.AddHideRule(IDC_STATIC_DAYF);
		Sizer.AddHideRule(IDC_STATIC_TIMEF);
		Sizer.AddHideRule(IDC_STATIC_MONF);
		Sizer.AddHideRule(IDC_STATIC_YEARF);
		Sizer.AddHideRule(IDC_STATIC_DAYF);
		Sizer.AddHideRule(IDC_STATIC_TIMEF);
		Sizer.AddHideRule(IDC_RADIO_DAY_EXACT);
		Sizer.AddHideRule(IDC_RADIO_MON_EXACT);
		Sizer.AddHideRule(IDC_RADIO_YEAR_EXACT);
		Sizer.AddHideRule(IDC_RADIO_DAY_EVERY);
		Sizer.AddHideRule(IDC_RADIO_MON_EVERY);
		Sizer.AddHideRule(IDC_RADIO_YEAR_EVERY);
		Sizer.AddHideRule(IDC_RADIO_DAY_EVERY_WDAY);
		Sizer.AddHideRule(IDC_COMBO_WDAYS);
		Sizer.AddHideRule(IDC_EDIT_DAY);
		Sizer.AddHideRule(IDC_EDIT_MONTH);
		Sizer.AddHideRule(IDC_EDIT_YEAR);
		Sizer.AddHideRule(IDC_RADIO_AM);
		Sizer.AddHideRule(IDC_RADIO_PM);
		// Показываем что надо
		Sizer.AddShowRule(IDC_MONTHCALENDAR);
		Sizer.AddShowRule(IDC_WATCH);
		Sizer.AddShowRule(IDC_BUTTON_A15);
		Sizer.AddShowRule(IDC_BUTTON_A60);
		Sizer.AddLeftAlign(IDC_TIME,IDC_WATCH,leftPos,1);
		Sizer.AddTopAlign(IDC_TIME,IDC_WATCH,bottomPos,2);
		Sizer.AddTopAlign(IDC_EDIT_ACTION_PAR2,IDC_TIME,bottomPos,4);
		Sizer.AddRightAlign(IDC_TIME,IDC_TIME,leftPos,TIME_WIDTH);
		Sizer.AddSideCenterAlign(IDC_TIME_EVERY,IDC_TIME,0);
		Sizer.AddSideCenterAlign(IDC_COMBO_HOUR_LIST,IDC_TIME,0);
		Sizer.AddSideCenterAlign(IDC_COMBO_TIME_DISCR,IDC_TIME,0);
		Sizer.AddRightAlign(IDC_COMBO_TIME_DISCR,IDC_TIME,leftPos,-4);
		Sizer.AddRightAlign(IDC_COMBO_HOUR_LIST,IDC_COMBO_TIME_DISCR,leftPos,-2);
		Sizer.AddRightAlign(IDC_TIME_EVERY,IDC_COMBO_HOUR_LIST,leftPos,-2);
		Sizer.AddLeftAlign(IDC_TIME_EVERY,0,0);
		Sizer.AddGotoRule(100);
	}
	Sizer.AddGotoLabel(100);
	// Выравнивание основного вида
	Sizer.AddLeftAlign(IDC_MONTHCALENDAR,0,leftPos,1);
	Sizer.AddTopAlign(IDC_MONTHCALENDAR,IDC_TITLE,bottomPos,3);
	Sizer.AddRightAlign(IDC_WATCH,0,rightPos,-1);
	Sizer.AddRightAlign(IDC_MONTHCALENDAR,IDC_WATCH,leftPos,-4);
	Sizer.AddTopAlign(IDC_WATCH,IDC_TITLE,bottomPos,3);

	Sizer.AddSideCenterAlign(IDC_BUTTON_A15,IDC_TIME,0);
	Sizer.AddSideCenterAlign(IDC_BUTTON_A60,IDC_TIME,0);
	Sizer.AddRightAlign(IDC_BUTTON_A60,IDC_WATCH,rightPos,-1);
	Sizer.AddRightAlign(IDC_BUTTON_A15,IDC_BUTTON_A60,leftPos,-1);

	// Выравнивание расширенного вида
	Sizer.AddLeftAlign(IDC_STATIC_DAYF,0,leftPos,1);
	Sizer.AddRightAlign(IDC_STATIC_DAYF,0,rightPos,1);
	Sizer.AddLeftAlign(IDC_STATIC_MONF,0,leftPos,1);
	Sizer.AddRightAlign(IDC_STATIC_MONF,0,rightPos,1);
	Sizer.AddLeftAlign(IDC_STATIC_YEARF,0,leftPos,1);
	Sizer.AddRightAlign(IDC_STATIC_YEARF,0,rightPos,1);
	Sizer.AddLeftAlign(IDC_STATIC_TIMEF,0,leftPos,1);
	Sizer.AddRightAlign(IDC_STATIC_TIMEF,0,rightPos,1);
	Sizer.AddTopAlign(IDC_STATIC_DAYF,IDC_TITLE,bottomPos,2);
	Sizer.AddTopAlign(IDC_STATIC_MONF,IDC_STATIC_DAYF,bottomPos,0);
	Sizer.AddTopAlign(IDC_STATIC_YEARF,IDC_STATIC_MONF,bottomPos,0);
	Sizer.AddTopAlign(IDC_STATIC_TIMEF,IDC_STATIC_YEARF,bottomPos,0);
	// День
	Sizer.AddSideCenterAlign(IDC_RADIO_DAY_EXACT,IDC_STATIC_DAYF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_EDIT_DAY,IDC_STATIC_DAYF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_RADIO_DAY_EVERY,IDC_STATIC_DAYF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_RADIO_DAY_EVERY_WDAY,IDC_STATIC_DAYF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_COMBO_WDAYS,IDC_STATIC_DAYF,sideCenter,6);

	Sizer.AddLeftAlign(IDC_RADIO_DAY_EXACT,IDC_STATIC_DAYF,leftPos,8);
	Sizer.AddLeftAlign(IDC_EDIT_DAY,IDC_RADIO_DAY_EXACT,rightPos,6);
	Sizer.AddRightAlign(IDC_COMBO_WDAYS,IDC_STATIC_DAYF,rightPos,-8);
	Sizer.AddRightAlign(IDC_RADIO_DAY_EVERY_WDAY,IDC_COMBO_WDAYS,leftPos,-1);
	Sizer.AddRightAlign(IDC_RADIO_DAY_EVERY,IDC_RADIO_DAY_EVERY_WDAY,leftPos,-1);
	Sizer.AddLeftAlign(IDC_RADIO_DAY_EVERY,IDC_EDIT_DAY,rightPos,10);
	// Месяц
	Sizer.AddSideCenterAlign(IDC_RADIO_MON_EXACT,IDC_STATIC_MONF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_EDIT_MONTH,IDC_STATIC_MONF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_RADIO_MON_EVERY,IDC_STATIC_MONF,sideCenter,6);

	Sizer.AddLeftAlign(IDC_RADIO_MON_EXACT,IDC_STATIC_MONF,leftPos,8);
	Sizer.AddLeftAlign(IDC_EDIT_MONTH,IDC_RADIO_MON_EXACT,rightPos,6);
	Sizer.AddLeftAlign(IDC_RADIO_MON_EVERY,IDC_EDIT_MONTH,rightPos,10);
	// Год
	Sizer.AddSideCenterAlign(IDC_RADIO_YEAR_EXACT,IDC_STATIC_YEARF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_EDIT_YEAR,IDC_STATIC_YEARF,sideCenter,6);
	Sizer.AddSideCenterAlign(IDC_RADIO_YEAR_EVERY,IDC_STATIC_YEARF,sideCenter,6);

	Sizer.AddLeftAlign(IDC_RADIO_YEAR_EXACT,IDC_STATIC_YEARF,leftPos,8);
	Sizer.AddLeftAlign(IDC_EDIT_YEAR,IDC_RADIO_YEAR_EXACT,rightPos,6);
	Sizer.AddRightAlign(IDC_RADIO_YEAR_EVERY,IDC_STATIC_YEARF,rightPos,-8);
	Sizer.AddLeftAlign(IDC_RADIO_YEAR_EVERY,IDC_EDIT_YEAR,rightPos,10);
	/////////////////////////////
	// Задаем начальные значения
	/////////////////////////////
	((CComboBox*)GetDlgItem(IDC_COMBO_WDAYS))->AddString(_l("Monday"));
	((CComboBox*)GetDlgItem(IDC_COMBO_WDAYS))->AddString(_l("Tuesday"));
	((CComboBox*)GetDlgItem(IDC_COMBO_WDAYS))->AddString(_l("Wednesday"));
	((CComboBox*)GetDlgItem(IDC_COMBO_WDAYS))->AddString(_l("Thursday"));
	((CComboBox*)GetDlgItem(IDC_COMBO_WDAYS))->AddString(_l("Friday"));
	((CComboBox*)GetDlgItem(IDC_COMBO_WDAYS))->AddString(_l("Saturday"));
	((CComboBox*)GetDlgItem(IDC_COMBO_WDAYS))->AddString(_l("Sunday"));

	((CComboBox*)GetDlgItem(IDC_COMBO_HOUR_LIST))->AddString(" 1");
	((CComboBox*)GetDlgItem(IDC_COMBO_HOUR_LIST))->AddString(" 10");
	((CComboBox*)GetDlgItem(IDC_COMBO_HOUR_LIST))->AddString(" 15");
	((CComboBox*)GetDlgItem(IDC_COMBO_HOUR_LIST))->AddString(" 30");

	((CComboBox*)GetDlgItem(IDC_COMBO_TIME_DISCR))->AddString(_l("Min"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIME_DISCR))->AddString(_l("Hour"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIME_DISCR))->AddString(_l("Day"));
	CString sWTitle="";
	if(bThisIsNewReminder){
		sWTitle=_l("New Reminder");
	}else{
		sWTitle=_l("Modify reminder");
	}
	SetWindowText((reminder->IsShared()?CString("*"):CString(""))+sWTitle);
	((CButton*)GetDlgItem(IDC_BUTTON_CVIEW))->SetWindowText(_l("Recurrence"));
	m_ComboActionsImages.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 2);
	AddBitmapToIList(m_ComboActionsImages,IDB_IMAGELIST);
	m_IconCombo.SetImageList(&m_ComboActionsImages);
	InitTRIconList(m_IconCombo);
	if(m_IconCombo.GetComboBoxCtrl()->SetCurSel(reminder->GetIconNum()-TR_ICONS_OFFSET)==CB_ERR){
		m_IconCombo.GetComboBoxCtrl()->SetCurSel(REM_ICONS_OFFSET-TR_ICONS_OFFSET);
	}
	int iCount=0;
	m_ComboActions.SetImageList(&m_ComboActionsImages);
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Play sound"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Display text"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Shake active window"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Popup note/Show ToDo"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Run application"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Stay in tray"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Shutdown computer"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Send net message")+(objSettings.bNoSend?(CString(" (")+_l("Prohibited")+")"):""), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Activate todo/reminder"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Deactivate todo/reminder"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Auto close alert"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Keep reminding until dismissed"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Day of week: Ignore in that days"), reminder->ActionUse[iCount]+7);
	iCount++;
	AddComboBoxExItem(&m_ComboActions, iCount, _l("Day of month: Ignore in that days"), reminder->ActionUse[iCount]+7);
	iCount++;
	if(iCount!=DEFAULT_REMACTCOUNT){
		AfxMessageBox("Invalid reminder actions count!");
	}
	//
	if(reminder->bUndeletable && !bThisIsNewReminder){
		GetDlgItem(IDC_BUTTON_DELETE)->ShowWindow(SW_HIDE);
	}
	m_Month_Exact=reminder->bMonType;
	m_Year_Exact=reminder->bYearType;
	m_Day_Exact=reminder->bDayType;
	m_Active_Reminder=reminder->GetState();
	m_EditDay=Format("%i",reminder->dwDay);
	m_ComboHourList=Format("%i",reminder->dwTimeNum);
	m_RepeatedTime=reminder->bTimeType;
	m_ComboTimeDistr=reminder->iTimeNumType;
	m_EditMonth=Format("%i",reminder->dwMon);
	m_EditYear=Format("%i",reminder->dwYear);
	m_Time=reminder->dwTime;
	m_ReminderTitle=reminder->GetTitle();
	m_EditActionPar="";
	m_ReminderText=reminder->ActionPars[1];
	m_ReminderText.Replace("\n","\r\n");
	m_ComboWDaysList=reminder->dwWDay-2;
	if(m_ComboWDaysList<0){
		m_ComboWDaysList=m_ComboWDaysList+7;
	}
	//
	m_ComboActions.SetCurSel(objSettings.ReminderDLGLastAction);
	SetComboActionPos();
	//
	COleDateTime refTime(reminder->dwYear,reminder->dwMon,reminder->dwDay,0,0,0);
	if(refTime.GetStatus()==COleDateTime::valid){
		m_Calendar.SetMaxSelCount(1);
		m_Calendar.SetMonthDelta(1);
		m_Calendar.SetToday(refTime);
		m_Calendar.SetCurSel(refTime);
	}
	if(m_Time.GetHour()>=12){
		m_AmPm=1;
	}else{
		m_AmPm=0;
	}
	bWatchLock=0;
	bWatchLockR=0;
	UpdateData(FALSE);
	//
	int iIcon=m_IconCombo.GetComboBoxCtrl()->GetCurSel()+TR_ICONS_OFFSET;//(pTask?pTask->GetIconNum():0)+TR_ICONS_OFFSET;
	SetIcon(m_ComboActionsImages.ExtractIcon(iIcon),TRUE);
	SetIcon(m_ComboActionsImages.ExtractIcon(iIcon),FALSE);
	//
	if(!reminder->IsStrictReminder()){
		OnButtonCview();
	}
	SetTaskbarButton(this->GetSafeHwnd());
	GetDlgItem(IDC_TIME)->SetFocus();
	return TRUE;
}

void DLG_Reminder::OnButtonSaveexit()
{
	if(ApplySettedValues()){
		// Напоминаем объектам ппересчитать при случае время срабатывания
		reminder->OnModify();
		objSettings.m_Reminders.OnModify();
		ExitRemWindow(IDOK);
	}
}

BOOL DLG_Reminder::CanChooseParEdit(int iSelNum)
{
	if(iSelNum<0 || (reminder && iSelNum>=reminder->ActionPars.GetSize())){
		return FALSE;
	}
	if(iSelNum==11){
		return FALSE;
	}
	if(iSelNum==1 || iSelNum==5 || iSelNum==6){
		return FALSE;
	}
	return TRUE;
}

BOOL DLG_Reminder::CanChoosePar(int iSelNum)
{
	if(iSelNum<0 || (reminder && iSelNum>=reminder->ActionPars.GetSize())){
		return FALSE;
	}
	if(iSelNum==11){
		return FALSE;
	}
	if((iSelNum==0 || iSelNum==3 || iSelNum==4 || iSelNum>6) && iSelNum!=10){
		return TRUE;
	}
	return FALSE;
}

void DLG_Reminder::SetComboActionPos()
{
	int iCurSel=m_ComboActions.GetCurSel();
	if(iCurSel!=-1){
		if(iCurSel>=0 && iCurSel<reminder->ActionPars.GetSize()){
			m_EditActionPar=reminder->ActionPars[iCurSel];
		}
	}
	GetDlgItem(IDC_BUTTON_ACTION_PAR_CHOSER)->ShowWindow(CanChoosePar(iCurSel)?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_EDIT_ACTION_PAR)->ShowWindow(CanChooseParEdit(iCurSel)?SW_SHOW:SW_HIDE);
}

void DLG_Reminder::OnSelchangeComboAction()
{
	UpdateData(TRUE);
	SetComboActionPos();
	UpdateData(FALSE);
}

void DLG_Reminder::OnChangeEditActionPar()
{
	UpdateData(TRUE);
	int iCurSel=m_ComboActions.GetCurSel();
	if(iCurSel!=-1){
		if(iCurSel>=0 && iCurSel<reminder->ActionPars.GetSize()){
			reminder->ActionPars[iCurSel]=m_EditActionPar;
		}
		SetComboAction(TRUE);
	}
}

void DLG_Reminder::OnButtonActionParChoser() 
{
	UpdateData(TRUE);
	CString sNewValue;
	int iCurSel=m_ComboActions.GetCurSel();
	if(!CanChoosePar(iCurSel)){
		return;
	}
	CRect rt;
	GetDlgItem(IDC_BUTTON_ACTION_PAR_CHOSER)->GetWindowRect(&rt);
	CPoint pos=rt.TopLeft();
	//GetCursorPos(&pos);
	if(iCurSel==7){
		// Список из адресной книги
		if(objSettings.AddrBook.GetSize()==0){
			Alert(_l("Your address book is empty")+"!",DEFAULT_READTIME);
			return;
		}
		CMenu* pNewMenu=new CMenu;
		pNewMenu->CreatePopupMenu();
		RefreshRecipientMenu(NULL, pNewMenu);
		int iRecipient=::TrackPopupMenu(pNewMenu->GetSafeHmenu(), TPM_RETURNCMD, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL)-WM_USER-SEND_TO_OFFSET;
		if(iRecipient>=0){
			if(iRecipient<objSettings.AddrBook.aPersons.GetSize()){
				Person* pPerson=objSettings.AddrBook.aPersons[iRecipient];
				if(isPressed(VK_SHIFT)){
					sNewValue=m_EditActionPar+"; "+pPerson->GetTitle();
					sNewValue.TrimLeft(" ;");
				}else{
					sNewValue=pPerson->GetTitle();
				}
			}else{
				sNewValue=objSettings.AddrBook.GetTabComps(iRecipient);
			}
		}
		delete pNewMenu;
	}else if((iCurSel==3 || iCurSel==8 || iCurSel==9) && pMainDialogWindow){
		// Задача/напоминание
		CMenu NewMenu;
		NewMenu.CreatePopupMenu();
		pMainDialogWindow->CreateMenuTree(NewMenu, "", 0);
		int iItem=::TrackPopupMenu(NewMenu.GetSafeHmenu(), TPM_RETURNCMD, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL)-WM_USER-SWITCH_TO;
		CItem* pItem=CItem::GetItemByIndex(iItem);
		if(pItem){
			sNewValue=pItem->GetTitle();
		}
	}else if(iCurSel==12){
		sNewValue=","+m_EditActionPar+",";
		sNewValue.Replace(" ","");
		sNewValue.Replace(";",",");
		CMenu NewMenu;
		NewMenu.CreatePopupMenu();
		for(int i=1;i<=7;i++){
			BOOL bCheck=(sNewValue.Find(Format(",%s,",GetWeekDay(i)))!=-1);
			AddMenuString(&NewMenu,i,GetWeekDay(i),0,bCheck);
		}
		int iRecipient=::TrackPopupMenu(NewMenu.GetSafeHmenu(), TPM_RETURNCMD, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
		if(iRecipient>=0){
			sNewValue+=",";
			CString sDay=Format(",%s,",GetWeekDay(iRecipient));
			BOOL bCheck=(sNewValue.Find(sDay)!=-1);
			if(bCheck){
				sNewValue.Replace(sDay,",");
			}else{
				sNewValue=sDay+sNewValue;
			}
		}
		sNewValue.TrimRight();
		sNewValue.TrimRight(",");
		sNewValue.TrimLeft();
		sNewValue.TrimLeft(",");
		while(sNewValue.Replace(",,",","));
	}else if(iCurSel==13){
		sNewValue=","+m_EditActionPar+",";
		sNewValue.Replace(" ","");
		sNewValue.Replace(";",",");
		CMenu NewMenu;
		NewMenu.CreatePopupMenu();
		for(int i=1;i<=31;i++){
			BOOL bBrea=((i-1)%7==0);
			BOOL bCheck=(sNewValue.Find(Format(",%i,",i))!=-1);
			AddMenuString(&NewMenu,i,Format("%i",i),0,bCheck,0,0,0,bBrea);
		}
		int iRecipient=::TrackPopupMenu(NewMenu.GetSafeHmenu(), TPM_RETURNCMD, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
		if(iRecipient>=0){
			sNewValue+=",";
			CString sDay=Format(",%i,",iRecipient);
			BOOL bCheck=(sNewValue.Find(sDay)!=-1);
			if(bCheck){
				sNewValue.Replace(sDay,",");
			}else{
				sNewValue=sDay+sNewValue;
			}
		}
		sNewValue.TrimRight();
		sNewValue.TrimRight(",");
		sNewValue.TrimLeft();
		sNewValue.TrimLeft(",");
		while(sNewValue.Replace(",,",","));
	}else{
		CFileDialog dlg(TRUE, NULL, m_EditActionPar, 0, "All files (*.*)|*.*||", NULL);
		SimpleTracker Track(objSettings.bDoModalInProcess);
		DWORD dwRes=dlg.DoModal();
		if(dwRes!=FNERR_INVALIDFILENAME){
			sNewValue=dlg.GetPathName();
		}else{
			return;
		}
	}
	if(sNewValue!="" || iCurSel==12 || iCurSel==13){
		m_EditActionPar=sNewValue;
		UpdateData(FALSE);
		OnChangeEditActionPar();
	}
}

void DLG_Reminder::OnButtonDelete() 
{
	objSettings.m_Reminders.Delete(reminder->GetID());
	//ExitRemWindow(IDCANCEL);
}

void DLG_Reminder::OnButtonDontsaveexit() 
{
	ExitRemWindow(IDCANCEL);
}

void DLG_Reminder::OnSelectMonthcalendar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	_SYSTEMTIME sSelectedDay;
	m_Calendar.GetCurSel(&sSelectedDay);
	m_EditDay=Format("%i",sSelectedDay.wDay);
	m_EditMonth=Format("%i",sSelectedDay.wMonth);
	m_EditYear=Format("%i",sSelectedDay.wYear);
	UpdateData(FALSE);
	*pResult = 0;
}

void DLG_Reminder::OffsetWatch(int iMinuteDx, int iHourDx)
{
	UpdateData(TRUE);
	::PlaySound("Sound\\watch.wav",NULL,SND_FILENAME|SND_ASYNC|SND_NODEFAULT);
	CTimeSpan sp(0,iHourDx,iMinuteDx,0);
	m_Time=m_Time+sp;
	GetDlgItem(IDC_WATCH)->Invalidate(FALSE);
	UpdateData(FALSE);
}

void DLG_Reminder::OnWatch() 
{
	UpdateData(TRUE);
	CRect watch;
	GetDlgItem(IDC_WATCH)->GetWindowRect(watch);
	watch.DeflateRect(5,5);
	POINT pCenter;
	pCenter.x=watch.Width()/2+watch.left;
	pCenter.y=watch.Height()/2+watch.top;
	POINT p;
	GetCursorPos(&p);
	double dx=p.x-pCenter.x;
	double dy=p.y-pCenter.y;
	double ang=atan(dx/dy);
	double dDist=sqrt(dx*dx+dy*dy);
	double hour=fabs(ang*6/3.1415);
	if(dx>=0 && dy>0){
		hour=6-hour;
	}
	if(dx<0 && dy>0){
		hour=6+hour;
	}
	if(dx<0 && dy<0){
		hour=12-hour;
	}
	if(m_AmPm==1){
		if(hour<12){
			hour+=12;
		}
	}else{
		if(hour>=12){
			hour-=12;
		}
	}
	if(!bWatchLockR && dDist<(watch.Width()/3)){
		// Часы
		if(dLastHour!=-1){
			if(dLastHour<1 && hour>11){
				hour=hour+12;
			}else if(dLastHour<13 && hour>23){
				hour=hour-12;
			}else if(dLastHour>23 && hour<13){
				hour=hour-12;
			}else if(dLastHour>11 && hour<1){
				hour=hour+12;
			}
		}
		dLastHour=hour;
		int iHour=(unsigned short)hour;
		int min=int((hour-(double)iHour)*60);
		m_Time=CTime(2000,1,1,iHour,min,0);
	}else{
		// Минуты
		UINT iMin=((unsigned short)(hour*60/12))%60;
		int iHour=m_Time.GetHour();
		if(iLastMin!=-1){
			if(iLastMin<10 && iMin>50){
				iHour--;
			}else if(iLastMin>50 && iMin<10){
				iHour++;
			}
			if(iHour<0){
				iHour=23;
			}
			if(iHour>24){
				iHour=0;
			}
		}
		iLastMin=iMin;
		m_Time=CTime(2000,1,1,iHour,iMin,0);
	}
	if(hour>=12){
		m_AmPm=1;
	}else{
		m_AmPm=0;
	}
	GetDlgItem(IDC_WATCH)->Invalidate(FALSE);
	UpdateData(FALSE);
}

// ang-в часах!
void GetPolarPoint(double ang, POINT& p, double size)
{
	ang=(3.1415*ang/6)-3.1415/2;
	p.x=int(cos(ang)*size);
	p.y=int(sin(ang)*size);
}

BOOL DLG_Reminder::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_LBUTTONDOWN && ::GetDlgCtrlID(pMsg->hwnd)==IDC_WATCH){
		bWatchLock=1;
	}
	if(pMsg->message==WM_LBUTTONUP){
		bWatchLock=0;
	}
	if(pMsg->message==WM_RBUTTONDOWN && ::GetDlgCtrlID(pMsg->hwnd)==IDC_WATCH){
		bWatchLockR=1;
	}
	if(pMsg->message==WM_RBUTTONUP){
		bWatchLockR=0;
	}
	if(::GetDlgCtrlID(pMsg->hwnd)==IDC_WATCH){
		if(pMsg->message==WM_KEYDOWN){
			if(pMsg->wParam==VK_UP){
				OffsetWatch(0,1);
				return TRUE;
			}
			if(pMsg->wParam==VK_DOWN){
				OffsetWatch(0,-1);
				return TRUE;
			}
			if(pMsg->wParam==VK_LEFT){
				OffsetWatch(-5,0);
				return TRUE;
			}
			if(pMsg->wParam==VK_RIGHT){
				OffsetWatch(5,0);
				return TRUE;
			}
		}
		if(pMsg->message==WM_MOUSEMOVE && (bWatchLock||bWatchLockR)){
			::PlaySound("Sound\\watch.wav",NULL,SND_FILENAME|SND_ASYNC|SND_NODEFAULT);
			OnWatch();
		}
	}
	UINT iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==VK_ESCAPE){
			OnSysCommand(SC_CLOSE,0);
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN && iCtrlID!=IDC_EDIT_ACTION_PAR2){
			OnButtonSaveexit();
			return TRUE;
		}
		if(isPressed(VK_CONTROL)){
			if(pMsg->wParam==VK_RETURN){
				OnButtonSaveexit();
				return TRUE;
			}
			if(pMsg->wParam=='D'){
				OnButtonDelete();
				return TRUE;
			}
		}/*else{
			if(pMsg->wParam==VK_RETURN && iCtrlID!=IDC_EDIT_ACTION_PAR2){
				return TRUE;
			}
		}*/
	}
	if(iCtrlID==IDC_COMBO_ACTION && (pMsg->message==WM_LBUTTONDOWN || pMsg->message==WM_LBUTTONUP || pMsg->message==WM_LBUTTONDBLCLK)){
		CRect rect;
		m_ComboActions.GetWindowRect(&rect);
		rect.right=rect.left+16;//Ширина картинки
		CPoint p;
		GetCursorPos(&p);
		if(rect.PtInRect(p)){
			if((pMsg->message==WM_LBUTTONDOWN && !objSettings.UseBblClick) || (pMsg->message==WM_LBUTTONDBLCLK && objSettings.UseBblClick)){
				OnDblclkComboAction();
			}
			return TRUE;
		}
	}
	if(pMsg->message==WM_DROPFILES){
		HDROP hDropInfo=(HDROP)pMsg->wParam;
		int iCounter=0;
		char szFilePath[MAX_PATH]="",szFileName[MAX_PATH]="",szExtension[MAX_PATH]="";
		UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
		for (UINT u = 0; u < cFiles; u++){
			DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
			m_EditActionPar=szFilePath;
			UpdateData(FALSE);
			OnChangeEditActionPar();
			break;
		}
		DragFinish(hDropInfo);
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void DLG_Reminder::OnRadioAm() 
{
	UpdateData(TRUE);
	int min=m_Time.GetMinute();
	int hour=m_Time.GetHour();
	if(m_AmPm==1 && hour<12){
		hour+=12;
	}else if(m_AmPm==0 && hour>=12){
		hour-=12;
	}
	m_Time=CTime(2000,1,1,hour,min,0);
	UpdateData(FALSE);
}

void DLG_Reminder::OnRadioPm() 
{
	OnRadioAm();
}

void DLG_Reminder::OnDatetimechangeTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	if(m_Time.GetHour()>=12){
		m_AmPm=1;
	}else{
		m_AmPm=0;
	}
	UpdateData(FALSE);
	GetDlgItem(IDC_WATCH)->Invalidate();
	*pResult = 0;
}

void DLG_Reminder::OnButtonCview() 
{
	if(iStyle==0){
		iStyle=1;
		((CButton*)GetDlgItem(IDC_BUTTON_CVIEW))->SetWindowText(_l("Recurrence"));
	}else{
		iStyle=0;
		((CButton*)GetDlgItem(IDC_BUTTON_CVIEW))->SetWindowText(_l("Simple mode"));
	}
	Sizer.SetGotoRule(MainRulePos,iStyle);
	Sizer.ApplyLayout();
}

LRESULT DLG_Reminder::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_SHOWWINDOW && wParam==TRUE){
		long lCalenWidth=MonthCal_GetMaxTodayWidth(GetDlgItem(IDC_MONTHCALENDAR)->GetSafeHwnd());
		/*if(lCalenWidth==0){
			RECT calRectInfo;
			if(MonthCal_GetMinReqRect(GetDlgItem(IDC_MONTHCALENDAR)->GetSafeHwnd(),&calRectInfo)){
				lCalenWidth=calRectInfo.right-calRectInfo.left;
			}
		}
		//CtrlWidth(this,IDC_MONTHCALENDAR);*/
		lCalenWidth+=10;
		int iWidth=CtrlWidth(this,IDC_WATCH)+lCalenWidth+CtrlWidth(this,0)+20;
		int iHeight=CtrlHeight(this,0)+CtrlHeight(this,IDC_MONTHCALENDAR)+CtrlHeight(this,IDC_STATIC_TITLE)+CtrlHeight(this,IDC_TITLE)+CtrlHeight(this,IDC_COMBO_ACTION)+CtrlHeight(this,IDC_BUTTON_CVIEW)+CtrlHeight(this,IDC_TIME)*4;
		if(iWidth<DIALOG_MINXSIZEW){
			iWidth=DIALOG_MINXSIZEW;
		}
		if(iHeight<DIALOG_MINYSIZE){
			iHeight=DIALOG_MINYSIZE;
		}
		CRect rect3;
		GetWindowRect(rect3);
		rect3.right=rect3.left+iWidth;
		rect3.bottom=rect3.top+iHeight;
		Sizer.SetMinRect(rect3);
		SetWindowPos(&CWnd::wndTop,rect3.left,rect3.top,rect3.Width(),rect3.Height(),SWP_SHOWWINDOW);
	}
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void DLG_Reminder::OnRadioDayEveryWday() 
{
	UpdateData(TRUE);
	m_Month_Exact = 1;
	m_Year_Exact = 1;
	UpdateData(FALSE);
}

void DLG_Reminder::OnRadioDayEvery() 
{
	UpdateData(TRUE);
	m_Month_Exact = 1;
	m_Year_Exact = 1;
	UpdateData(FALSE);
}

void DLG_Reminder::OnRadioMonEvery() 
{
	UpdateData(TRUE);
	m_Year_Exact = 1;
	UpdateData(FALSE);
}

void DLG_Reminder::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID==SC_CLOSE){
		if(bThisIsNewReminder){
			OnButtonDelete();
			return;
		}
		if(!ApplySettedValues()){
			return;
		}
		ExitRemWindow(0);
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void DLG_Reminder::OnClose()
{
	CDialog::OnClose();
}

void DLG_Reminder::OnButtonTest() 
{
	if(ApplySettedValues()){
		reminder->PerformReminderActions(TRUE);
	}
}

BOOL DLG_Reminder::ApplySettedValues()
{
	UpdateData(TRUE);
	// Проверяем корректность даты/времени
	DWORD dwDay=atol(m_EditDay);
	DWORD dwMon=atol(m_EditMonth);
	DWORD dwYear=atol(m_EditYear);
	m_ComboHourList.TrimLeft();
	m_ComboHourList.TrimRight();
	DWORD dwTimeNum=atoi(m_ComboHourList);
	BOOL bInvalidDate=0;
	if(m_RepeatedTime==0 && m_Day_Exact==0 && m_Month_Exact==0 && m_Year_Exact==0){
		COleDateTime dtReminder(dwYear,dwMon,dwDay,m_Time.GetHour(),m_Time.GetMinute(),m_Time.GetSecond());
		if(dtReminder.GetStatus()!=COleDateTime::valid || dtReminder<=COleDateTime::GetCurrentTime()){
			bInvalidDate=1;
		}
	}else if(m_RepeatedTime!=0 && dwTimeNum==0){
		bInvalidDate=2;
	}
	if(bInvalidDate!=0){
		// Ошибка! ремаиндеры прошлой датой не поддерживаются
		CString sError=_l("Reminder time or date is invalid");
		if(bInvalidDate==1){
			sError=_l("Reminder already expired\nCheck time and date");
		}
		Alert(sError,_l("Reminder"));
		return FALSE;
	}
	//
	objSettings.ReminderDLGLastAction=m_ComboActions.GetCurSel();
	reminder->SetIconNum(m_IconCombo.GetComboBoxCtrl()->GetCurSel()+TR_ICONS_OFFSET);
	reminder->bMonType=m_Month_Exact;
	reminder->bYearType=m_Year_Exact;
	reminder->bDayType=m_Day_Exact;
	reminder->SetState(m_Active_Reminder);
	reminder->dwTimeNum=dwTimeNum;
	reminder->bTimeType=m_RepeatedTime;
	reminder->iTimeNumType=m_ComboTimeDistr;
	reminder->dwDay=dwDay;
	reminder->dwMon=dwMon;
	reminder->dwYear=dwYear;
	reminder->dwTime=m_Time;
	reminder->dwWDay=m_ComboWDaysList+2;
	if(reminder->dwWDay>7){
		reminder->dwWDay=reminder->dwWDay-7;
	}
	if(m_ReminderTitle.GetLength()>0){
		reminder->SetTitle(m_ReminderTitle);
	}else{
		COleDateTime dt;
		reminder->CalulateClosestTime(dt);
		reminder->SetTitle(dt.Format("Time: [%d.%m.%Y %H:%M:%S]"));
	}
	m_ReminderText.Replace("\r\n","\n");
	if(reminder->ActionPars[1]==m_ReminderText && bThisIsNewReminder){
		reminder->ActionPars[1]=m_ReminderText+Format("\n%s",reminder->GetTitle());
	}else{
		reminder->ActionPars[1]=m_ReminderText;
	}
	reminder->SaveToFile();
	if(pMainDialogWindow){
		pMainDialogWindow->UpdateFeditIfNeeded(reminder);
	}
	reminder->ItemContentChanged();
	return TRUE;
}

void DLG_Reminder::AddSomeMinutesToTime(int iMinutes) 
{
	UpdateData(TRUE);
	COleDateTime dtReminder(atol(m_EditYear),atol(m_EditMonth),atol(m_EditDay),m_Time.GetHour(),m_Time.GetMinute(),m_Time.GetSecond());
	dtReminder+=COleDateTimeSpan(0,0,iMinutes,0);
	m_EditYear=Format("%i",dtReminder.GetYear());
	m_EditMonth=Format("%i",dtReminder.GetMonth());
	m_EditDay=Format("%i",dtReminder.GetDay());
	m_Time=CTime(dtReminder.GetYear(),dtReminder.GetMonth(),dtReminder.GetDay(),dtReminder.GetHour(),dtReminder.GetMinute(),dtReminder.GetSecond());
	m_Calendar.SetCurSel(dtReminder);
	UpdateData(FALSE);
	GetDlgItem(IDC_WATCH)->Invalidate(FALSE);
}

void DLG_Reminder::OnButtonA15() 
{
	AddSomeMinutesToTime(15);
}

void DLG_Reminder::OnButtonA60() 
{
	AddSomeMinutesToTime(60);
}

void DLG_Reminder::SetComboAction(BOOL bFlag)
{
	int iNewValue=bFlag;
	int iCurSel=m_ComboActions.GetCurSel();
	if(iCurSel!=-1){
		if(iCurSel>=0 && iCurSel<reminder->ActionPars.GetSize()){
			if(((BOOL)reminder->ActionUse[iCurSel])!=bFlag){
				reminder->ActionUse[iCurSel]=bFlag;
			}
			iNewValue=reminder->ActionUse[iCurSel];
		}
		AddComboBoxExItem(&m_ComboActions, iCurSel, NULL, iNewValue+7);
		m_ComboActions.Invalidate();
	}
}

void DLG_Reminder::OnDblclkComboAction()
{
	int iCurSel=m_ComboActions.GetCurSel();
	if(iCurSel!=-1){
		if(iCurSel>=0 && iCurSel<reminder->ActionPars.GetSize()){
			SetComboAction(!reminder->ActionUse[iCurSel]);
		}
	}
}

// CWPPreview
IMPLEMENT_DYNAMIC(CWatchPreview, CButton)
CWatchPreview::CWatchPreview(DLG_Reminder* pWDialog)
{
	m_pWDialog=pWDialog;
}

CWatchPreview::~CWatchPreview()
{
}

BEGIN_MESSAGE_MAP(CWatchPreview, CButton)
END_MESSAGE_MAP()

// CWPPreview message handlers
void CWatchPreview::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_pWDialog->UpdateData(TRUE);
	CDC* dcW=CDC::FromHandle(lpDrawItemStruct->hDC);
	CDC	pDCFrom;
	CDesktopDC dcScreen;
	pDCFrom.CreateCompatibleDC(&dcScreen);
	CBitmap* pOld=pDCFrom.SelectObject(_bmp().GetRaw(IDB_BITMAP_CLOCK,CSize(150,150)));
	CRect watch,watchScr;
	m_pWDialog->GetDlgItem(IDC_WATCH)->GetClientRect(watch);
	m_pWDialog->GetDlgItem(IDC_WATCH)->GetWindowRect(watchScr);
	POINT pCenter;
	pCenter.x=watch.Width()/2+watch.left;
	pCenter.y=watch.Height()/2+watch.top;
	POINT pLeft,pLeft1;
	pLeft1.x=pLeft.x=watch.left;
	pLeft.y=pCenter.y;
	pLeft1.y=pLeft.y-1;
	dcW->SetStretchBltMode(HALFTONE);
	dcW->StretchBlt(watch.left,watch.top,watch.Width(),watch.Height(),&pDCFrom,0,0,150,150,SRCCOPY);
	pDCFrom.SelectObject(pOld);
	watch.DeflateRect(15,15);
	/*Если рисовать самому...
	//dcW->Ellipse(watch.left,watch.top,watch.right,watch.bottom);
	CRect watchIn(watch);
	watchIn.DeflateRect(watch.Width()/6,watch.Width()/6);
	dcW->Arc( &watchIn, pLeft, pLeft);
	// Рисуем риски
	for(int i=0;i<12;i++){
		POINT pTick;
		GetPolarPoint(i,pTick,watch.Width()/2);
		pTick.x=pTick.x+pCenter.x;
		pTick.y=pTick.y+pCenter.y;
		dcW->Ellipse(pTick.x-3,pTick.y-3,pTick.x+3,pTick.y+3);
		dcW->MoveTo(pTick);
		dcW->LineTo(pTick.x+(pCenter.x-pTick.x)/4,pTick.y+(pCenter.y-pTick.y)/4);
	}
	*/
	//Час
	//POINT pHour;
	POINT Strelka[3];
	GetPolarPoint(double(m_pWDialog->m_Time.GetHour())+double(m_pWDialog->m_Time.GetMinute())/60,Strelka[0],watch.Width()/3);
	Strelka[0].x=pCenter.x+Strelka[0].x;
	Strelka[0].y=pCenter.y+Strelka[0].y;
	GetPolarPoint(double(m_pWDialog->m_Time.GetHour())+double(m_pWDialog->m_Time.GetMinute())/60+3,Strelka[1],10);
	Strelka[1].x=pCenter.x+Strelka[1].x;
	Strelka[1].y=pCenter.y+Strelka[1].y;
	GetPolarPoint(double(m_pWDialog->m_Time.GetHour())+double(m_pWDialog->m_Time.GetMinute())/60-3,Strelka[2],10);
	Strelka[2].x=pCenter.x+Strelka[2].x;
	Strelka[2].y=pCenter.y+Strelka[2].y;
	dcW->Polygon(Strelka,3);
	//Минута
	GetPolarPoint(double(m_pWDialog->m_Time.GetMinute()*12)/60,Strelka[0],watch.Width()/2.1);
	Strelka[0].x=pCenter.x+Strelka[0].x;
	Strelka[0].y=pCenter.y+Strelka[0].y;
	GetPolarPoint(double(m_pWDialog->m_Time.GetMinute()*12)/60+3,Strelka[1],10);
	Strelka[1].x=pCenter.x+Strelka[1].x;
	Strelka[1].y=pCenter.y+Strelka[1].y;
	GetPolarPoint(double(m_pWDialog->m_Time.GetMinute()*12)/60-3,Strelka[2],10);
	Strelka[2].x=pCenter.x+Strelka[2].x;
	Strelka[2].y=pCenter.y+Strelka[2].y;
	dcW->Polygon(Strelka,3);
	dcW->Ellipse(pCenter.x-5,pCenter.y-5,pCenter.x+5,pCenter.y+5);
}


void DLG_Reminder::OnActiveChanged() 
{
	UpdateData(TRUE);
	if(!ApplySettedValues()){
		m_Active_Reminder=0;
		UpdateData(FALSE);
	}
}
