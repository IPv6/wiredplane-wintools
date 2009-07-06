; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAlertDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "wcremind.h"
LastPage=0

ClassCount=5
Class1=CAlertDialog
Class2=CCalendarDlg2
Class3=CMonthCalCtrlEx
Class4=CResizeableDialog
Class5=CWCRemindApp

ResourceCount=5
Resource1=IDD_DIALOG_ENTER (Russian)
Resource2=IDD_CALENDAR2_DIALOG (Neutral)
Resource3=IDD_DIALOG_ENTER
Resource4=IDD_DIALOG_PP
Resource5=IDD_DIALOG_CL

[CLS:CAlertDialog]
Type=0
BaseClass=CDialog
HeaderFile=AlertDialog.h
ImplementationFile=AlertDialog.cpp
Filter=D
VirtualFilter=dWC
LastObject=CAlertDialog

[CLS:CCalendarDlg2]
Type=0
BaseClass=CResizeableDialog
HeaderFile=CalendarDlg2.h
ImplementationFile=CalendarDlg2.cpp

[CLS:CMonthCalCtrlEx]
Type=0
BaseClass=CMonthCalCtrl
HeaderFile=MonthCalCtrlEx.h
ImplementationFile=MonthCalCtrlEx.cpp

[CLS:CResizeableDialog]
Type=0
BaseClass=CDialog
HeaderFile=ResizeableDialog.h
ImplementationFile=ResizeableDialog.cpp

[CLS:CWCRemindApp]
Type=0
BaseClass=CWinApp
HeaderFile=WCRemind.h
ImplementationFile=WCRemind.cpp

[DLG:IDD_DIALOG_PP]
Type=1
Class=CAlertDialog
ControlCount=4
Control1=IDC_STATIC,static,1342179342
Control2=IDOK,button,1342242816
Control3=IDC_EDIT,RICHEDIT,1350633668
Control4=ID_PPREMINDER,button,1342242816

[DLG:IDD_CALENDAR2_DIALOG]
Type=1
Class=CCalendarDlg2

[DLG:IDD_CALENDAR2_DIALOG (Neutral)]
Type=1
Class=?
ControlCount=21
Control1=IDC_MONTHCALENDAR1,SysMonthCal32,1342242821
Control2=IDC_STATIC_TOP,static,1342308993
Control3=IDC_STATIC_DDMMYY,static,1342313089
Control4=IDC_DATETIMEPICKER_TIME,SysDateTimePick32,1342242857
Control5=IDC_CHECK_REC,static,1342308864
Control6=IDC_INFO_REC,static,1342312962
Control7=IDC_BUTTON_REC,button,1342242816
Control8=IDC_CHECK_ONEVENT_TEXT,button,1342242819
Control9=IDC_EDIT_TXT,edit,1352732804
Control10=IDC_CHECK_ONEVENT_MP3,button,1342242819
Control11=IDC_EDIT_MP3,edit,1350631552
Control12=IDC_BUTTON_MP3,button,1342242816
Control13=IDC_CHECK_ONEVENT_APP,button,1342242819
Control14=IDC_EDIT_APP,edit,1350631552
Control15=IDC_BUTTON_APP,button,1342242816
Control16=IDC_CHECK_ACTIVE2,static,1342308864
Control17=IDCANCEL_REM,button,1342242817
Control18=IDDEL,button,1342242817
Control19=IDOK,button,1342242817
Control20=IDC_BUTTON_TIME,button,1342242816
Control21=IDC_BUTTON_MP3_PLAY,button,1342242816

[DLG:IDD_DIALOG_ENTER]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC1,static,1342308866
Control4=IDC_EDIT1,edit,1350639744

[DLG:IDD_DIALOG_ENTER (Russian)]
Type=1
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC1,static,1342308866
Control4=IDC_EDIT1,edit,1350639744

[DLG:IDD_DIALOG_CL]
Type=1
Class=?
ControlCount=6
Control1=IDC_CALCTRL,DBSWMDateCtrl,1342242816
Control2=IDC_BUTTON_CLOSE,button,1342242816
Control3=IDC_BUTTON_NEWR,button,1342242816
Control4=IDC_BUTTON_VIEW,button,1342242816
Control5=IDC_BUTTON_NEXT,button,1342242816
Control6=IDC_BUTTON_PREV,button,1342242816

