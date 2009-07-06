// OCalendar.cpp : implementation file
//

#include "stdafx.h"
#include "WCRemind.h"
#include "OCalendar.h"
#include ".\ocalendar.h"


// COCalendar dialog
#define VIEW_MON 0x0001
#define VIEW_WNM 0x0002
BOOL WINAPI CreateNewReminder(COleDateTime dt,CCalendarCallback* pCallback,CWnd* wParent);
BOOL WINAPI CallModifyReminderX(char szKey[64], CWnd* wParent, DWORD dwOptions);
COCalendar::COCalendar(CCalendarCallback* opCallback, BOOL bViewDef, CWnd* pParent /*=NULL*/)
	: CResizeableDialog(COCalendar::IDD, pParent)
{
	bMonView=(bViewDef & VIEW_MON)?0:1;
	if(bViewDef & VIEW_WNM)
	{
		m_WMCtrl.SetWeekNumbers(1);
	}
	pCallback=opCallback;
}

COCalendar::~COCalendar()
{
}

void COCalendar::DoDataExchange(CDataExchange* pDX)
{
	CResizeableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CALCTRL, m_WMCtrl);
}

BEGIN_MESSAGE_MAP(COCalendar, CResizeableDialog)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_NEWR, OnBnClickedButtonNewr)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnBnClickedButtonView)
	ON_NOTIFY(NM_DBLCLK, IDC_CALCTRL, OnCellDblClick)
	ON_NOTIFY(NM_RCLICK, IDC_CALCTRL, OnCellRClick)
	ON_NOTIFY(NM_CLICK, IDC_CALCTRL, OnCellClick)	
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
END_MESSAGE_MAP()


// COCalendar message handlers

void COCalendar::OnBnClickedButtonClose()
{
	EndDialog(0);
}

void COCalendar::OnBnClickedButtonNewr()
{
	COleDateTime dt=COleDateTime::GetCurrentTime();
	COleDateTime dtTmp=m_WMCtrl.GetLastClicked();
	if(dtTmp.GetStatus()==COleDateTime::valid){
		dt.SetDateTime(dtTmp.GetYear(),dtTmp.GetMonth(),dtTmp.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
	}
	CreateNewReminder(dt,pCallback,this);
	RefreshCalendar();
}

void COCalendar::OnBnClickedButtonView()
{
	bMonView=1-bMonView;
	m_WMCtrl.SetView(bMonView?WV_MONTHVIEW:WV_WEEKVIEW, TRUE);
}

#define WS_EX_COMPOSITED        0x02000000L
BOOL COCalendar::OnInitDialog()
{
	CResizeableDialog::OnInitDialog();
	ModifyStyleEx(0,WS_EX_COMPOSITED,SWP_FRAMECHANGED);
	RegisterControl(IDC_CALCTRL, ATTACH_TO_HORIZONTAL_EDGES | ATTACH_TO_VERTICAL_EDGES);
	RegisterControl(IDC_BUTTON_VIEW, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDC_BUTTON_NEWR, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDC_BUTTON_CLOSE, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_RIGHT_EDGE);
	RegisterControl(IDC_BUTTON_PREV, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
	RegisterControl(IDC_BUTTON_NEXT, ATTACH_TO_BOTTOM_EDGE | ATTACH_TO_LEFT_EDGE);
	COleDateTime dtS = COleDateTime::GetCurrentTime();
	// Display Calendar in Month View
	il.Create(16,16,ILC_COLOR32 | ILC_MASK,0,1);
	il.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_WMCtrl.SetImageList(&il);
	m_WMCtrl.SetCurrentDate(dtS, FALSE, bMonView?WV_MONTHVIEW:WV_WEEKVIEW);
	HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	SetWindowText(_lx("RMX.UI.CALENDAR","Calendar"));
	GetDlgItem(IDC_BUTTON_VIEW)->SetWindowText(_lx("RMX.UI.CALENDAR.VIEW","Change view"));
	GetDlgItem(IDC_BUTTON_NEWR)->SetWindowText(_lx("RMX.UI.CALENDAR.NEW","New reminder"));
	GetDlgItem(IDC_BUTTON_CLOSE)->SetWindowText(_lx("RMX.UI.CALENDAR.CLOSE","Close"));
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	m_tooltip.AddTool(GetDlgItem(IDC_CALCTRL), _lx("RMX.UI.CALENDAR","Calendar"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_VIEW), _lx("RMX.UI.CALENDAR.VIEW","Change view"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_NEWR), _lx("RMX.UI.CALENDAR.NEW","New reminder"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_CLOSE), _lx("RMX.UI.CALENDAR.CLOSE","Close"));
	CRect rtThis;
	GetWindowRect(&rtThis);
	int iW=int(rtThis.Width()*0.7);
	if(iW<250){
		iW=250;
	}
	m_tooltip.SetMaxTipWidth(iW);
	RefreshCalendar();
	return FALSE;
}

CString GetRemDsc(CWPReminder& rem, BOOL bDate=0)
{
	CString sDsc=rem.szText;//TrimMessage(rem.szText,30,1);
	if(sDsc==""){
		sDsc=_l("No description");
	}
	sDsc.Replace("\r","");
	if(bDate){
		char szTmp[64]={0};
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&rem.EventTime,0,szTmp,sizeof(szTmp));
		return Format("%s\t%s %02i:%02i",sDsc,szTmp,rem.EventTime.wHour,rem.EventTime.wMinute);
	}else{
		return Format("%02i:%02i %s",rem.EventTime.wHour,rem.EventTime.wMinute,sDsc);
	}
}

extern CCriticalSection cs;
extern CArray<CWPReminder,CWPReminder&> aReminders;
BOOL COCalendar::RefreshCalendar()
{
	m_WMCtrl.DeleteAllItems();
	CCriticalSectionGuard l(&cs);
	for(int i=0;i<aReminders.GetSize();i++){
		COleDateTime dt(aReminders[i].EventTime);
		int iID=m_WMCtrl.InsertItem(dt,dt,GetRemDsc(aReminders[i]),0);
		m_WMCtrl.SetItemData(iID,i);
	}
	// Заметки...
	for (int nItem = 1; nItem <= WV_TOTALCELLS; nItem++){
		// Get Cell Data
		CWVCellData* pCell = m_WMCtrl.GetCellByID(nItem);
		SYSTEMTIME tm;
		COleDateTime dt=pCell->GetCellDate();
		dt.GetAsSystemTime(tm);
		char szDsc[2048]={0};
		if(pCallback->GetDayNote(tm,szDsc)){
			int iID=m_WMCtrl.InsertItem(dt,dt,szDsc);
			if(iID>=0){
				m_WMCtrl.SetItemData(iID,0xFFFFFFFF);
			}
		}
	}
	// Парсим файл
	CString sContent;
	char szMainFile[MAX_PATH]={0};
	if(pCallback->GetNotesFile(szMainFile,sizeof(szMainFile))){
		ReadFile(szMainFile,sContent);
		sContent.Replace("\r\n","\n");
		sContent=CString("\n")+sContent+"\n";
		while(sContent!=""){
			CString sLine=sContent.SpanExcluding("\n");
			if(sLine!=""){
				CStringArray aList;
				ConvertComboDataToArray(sLine,aList,'/');
				if(aList.GetSize()==3 && aList[2].GetLength()>5){
					CString sData=aList[2].Mid(5);
					CString sDovesok=CDataXMLSaver::GetInstringPart("[?CDATA{","}?DATAC:)]",sData);
					sData.Replace(Format("[?CDATA{%s}?DATAC:)]",sDovesok),"");
					sData.TrimLeft();
					sData.TrimRight();
					if(sData!="" && atol(aList[2])>0 && atol(aList[1])>0 && atol(aList[0])>0){
						COleDateTime dt(atol(aList[2]),atol(aList[1]),atol(aList[0]),0,0,0);
						if(dt.GetStatus()==COleDateTime::valid){
							sData.Replace("<br>","\n");
							int iID=m_WMCtrl.InsertItem(dt,dt,sData);
							if(iID>=0){
								m_WMCtrl.SetItemData(iID,0xFFFFFFFF);
								if(sDovesok.Find("BG")!=-1 && sDovesok.Find("FG")!=-1){
									CWVCellData* pItem=m_WMCtrl.GetCellByDate(dt);
									if(pItem){
										DWORD dwKeyTxColor=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("FG:(",")",sDovesok));
										DWORD dwKeyBgColor=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BG:(",")",sDovesok));
										if(dwKeyTxColor!=dwKeyBgColor){
											pItem->SetItemColor(iID,dwKeyTxColor);
											pItem->SetBkColor(dwKeyBgColor);
										}
									}
								}
							}
						}
					}
				}
				if(sContent.GetLength()>sLine.GetLength()){
					sContent=sContent.Mid(sLine.GetLength()+1);
				}else{
					sContent="";
				}
			}
			sContent.TrimLeft();
		}
	}
	return 0;
}

void COCalendar::OnCellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CCriticalSectionGuard l(&cs);
	NM_WVCELLDATA *pData = (NM_WVCELLDATA *)pNotifyStruct;
	int iIndex=-1;
	if(pData && pData->pItem){
		iIndex=pData->pItem->GetItemData();
	}
	if(pData->pCell){
		COleDateTime dtTmp=pData->pCell->GetCellDate();
		CallContextOnDate(dtTmp);
	}
	RefreshCalendar();
}

void COCalendar::OnCellDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CCriticalSectionGuard l(&cs);
	NM_WVCELLDATA *pData = (NM_WVCELLDATA *)pNotifyStruct;
	int iIndex=-1;
	if(pData && pData->pItem){
		iIndex=pData->pItem->GetItemData();
	}
	if(iIndex>=0 && iIndex<aReminders.GetSize()){
		CallModifyReminderX(aReminders[iIndex].szKey,this,0);
	}else{
		if(pData->pCell){
			COleDateTime dtTmp=pData->pCell->GetCellDate();
			CallDialogOnDate(dtTmp);
		}
	}
	RefreshCalendar();
}

void COCalendar::CallContextOnDate(COleDateTime dtTmp)
{
	SYSTEMTIME st;
	st.wYear=dtTmp.GetYear();
	st.wMonth=dtTmp.GetMonth();
	st.wDay=dtTmp.GetDay();
	int iRes=pCallback->CallContextDayNote(st,GetSafeHwnd());
	if(iRes==-1){
		CreateNewReminder(dtTmp,pCallback,this);
	}
};

void COCalendar::CallDialogOnDate(COleDateTime dtTmp)
{
	SYSTEMTIME st;
	st.wYear=dtTmp.GetYear();
	st.wMonth=dtTmp.GetMonth();
	st.wDay=dtTmp.GetDay();
	char szFG[64]="";
	char szBG[64]="";
	DWORD dwColor=m_WMCtrl.GetItemColor();
	sprintf(szFG,"#%02X%02X%02X",GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor));
	dwColor=m_WMCtrl.GetBkColor();
	sprintf(szBG,"#%02X%02X%02X",GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor));
	if(pCallback->CallEditDayNote(st,szFG,szBG)==0){
		COleDateTime dt=COleDateTime::GetCurrentTime();
		dt.SetDateTime(dtTmp.GetYear(),dtTmp.GetMonth(),dtTmp.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
		CreateNewReminder(dt,pCallback,this);
	}
};

void COCalendar::OnCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_WVCELLDATA *pData = (NM_WVCELLDATA *)pNotifyStruct;
}


BOOL COCalendar::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	
	if(pMsg->message==WM_MOUSEWHEEL){
		if((pMsg->wParam)&0x80000000){
			OnBnClickedButtonNext();
		}else{
			OnBnClickedButtonPrev();
		}
		return 0;
	}
	if(pMsg->message==WM_MOUSEMOVE){
		CPoint pt;
		GetCursorPos(&pt);
		GetDlgItem(IDC_CALCTRL)->ScreenToClient(&pt);
		CWVCellData* pCell=m_WMCtrl.HitTest(pt);
		if(pCell){
			CString sText=DateFormat(pCell->GetCellDate(),0);
			sText+="\n";
			sText+=m_WMCtrl.GetCellDsk(pCell);
			sText.Replace("\n","\r\n");
			sText.TrimRight();
			sText.TrimLeft();
			m_tooltip.UpdateTipText(sText,GetDlgItem(IDC_CALCTRL));
		}
	}
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==33){//p up
			OnBnClickedButtonPrev();
			return TRUE;
		}
		if(pMsg->wParam==34){//p down
			OnBnClickedButtonNext();
			return TRUE;
		}
		if(pMsg->wParam==VK_UP){
			if(bMonView){
				WalkDays(-7);
			}else{
				WalkDays(-1);
			}
			return TRUE;
		}
		if(pMsg->wParam==VK_DOWN){
			if(bMonView){
				WalkDays(7);
			}else{
				WalkDays(1);
			}
			return TRUE;
		}
		if(pMsg->wParam==VK_RIGHT){
			WalkDays(1);
			return TRUE;
		}
		if(pMsg->wParam==VK_LEFT){
			WalkDays(-1);
			return TRUE;
		}
		if(pMsg->wParam==VK_SPACE){
			OnBnClickedButtonView();
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN){
			CallDialogOnDate(m_WMCtrl.GetLastClicked());
			RefreshCalendar();
			return TRUE;
		}
	}
	return 	CResizeableDialog::PreTranslateMessage(pMsg);
}

void COCalendar::WalkDays(int i)
{
	COleDateTime dt=m_WMCtrl.GetLastClicked();
	if(i>0){
		dt+=COleDateTimeSpan(i,0,0,0);
	}else{
		dt-=COleDateTimeSpan(-i,0,0,0);
	}
	COleDateTime dt1;
	COleDateTime dt2;
	m_WMCtrl.GetDateRange(&dt1,&dt2);
	if(m_WMCtrl.GetCellByDate(dt) && dt>=dt1 && dt<=dt2){
		m_WMCtrl.HightlightCellByDate(dt);
	}else{
		m_WMCtrl.SetCurrentDate(dt, FALSE, bMonView?WV_MONTHVIEW:WV_WEEKVIEW);
		m_WMCtrl.HightlightCellByDate(dt);
		RefreshCalendar();
	}
}

void COCalendar::OnBnClickedButtonNext()
{
	if(bMonView){
		WalkDays(28);
	}else{
		WalkDays(7);
	}
}

void COCalendar::OnBnClickedButtonPrev()
{
	if(bMonView){
		WalkDays(-28);
	}else{
		WalkDays(-7);
	}
}
