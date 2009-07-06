// DLG_Find.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLG_Find dialog
CDLG_Find::CDLG_Find(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_Find::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_Find)
	m_Mask = _T("*");
	//}}AFX_DATA_INIT
	font=NULL;
	iFinded=0;
	bAlreadyExist=0;
	bSearchForPersons=FALSE;
	bLimit2FB=0;
	bSearchForPersonStarted=0;
	if(objSettings.pCurrentFindDialog){
		bAlreadyExist=1;
		SwitchToWindow(objSettings.pCurrentFindDialog->GetSafeHwnd(),TRUE);
	}else{
		objSettings.pCurrentFindDialog=this;
	}
}


void CDLG_Find::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Find)
	DDX_Control(pDX, IDC_RESULTS, m_Results);
	DDX_Text(pDX, IDC_MASK, m_Mask);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_Find, CDialog)
	//{{AFX_MSG_MAP(CDLG_Find)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_GO, OnGo)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_Find message handlers
LRESULT CDLG_Find::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	BOOL bRes=Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer);
	static long bFirstTime=0;
	if(::IsWindow(m_Results.GetSafeHwnd()) && (message==WM_SIZE || message==WM_ACTIVATE || bFirstTime==0 || (message==WM_USER && lParam==99 && wParam==99))){
		bFirstTime=1;
		CRect rt;
		m_Results.GetClientRect(&rt);
		int iW0=m_Results.GetColumnWidth(0);
		int iW1=m_Results.GetColumnWidth(1)-rt.Width();
		static int iW1P=0;
		if(iW1P!=iW1){
			if(iW0>rt.Width()){
				iW0=rt.Width()-1;
			}
			m_Results.SetColumnWidth(1,rt.Width()-iW0);
			iW1P=iW1;
		}
	}
	if(bRes){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDLG_Find::OnRclickResults()
{
	if(bSearchForPersons){
		return;
	}
	int iCurrentItem=-1;
	HIROW hRow=m_Results.GetSelectedIRow();
	if(hRow==NULL){
		return;
	}
	iCurrentItem=m_Results.GetIRowData(hRow);
	CItem* item=CItem::GetItemByIndex(iCurrentItem);
	if(item){
		pMainDialogWindow->TrackItemMenu(item);
	}else{
		m_Results.DeleteIRow(hRow);
	}
}

void CDLG_Find::OnDblclickResults()
{
	int iCurrentItem=-1;
	HIROW hRow=m_Results.GetSelectedIRow();
	if(hRow==NULL){
		return;
	}
	iCurrentItem=m_Results.GetIRowData(hRow);
	if(bSearchForPersons){
		CFindedUser* pUser=&aFindedUsers[iCurrentItem];
		CNewPerson dlgNewPerson(this);
		dlgNewPerson.m_CompName=pUser->sID;
		dlgNewPerson.m_Nick=pUser->sNick;
		dlgNewPerson.m_Dsc=pUser->sAddInfo;
		dlgNewPerson.m_bGroup=FALSE;
		dlgNewPerson.DoModal();
		if(dlgNewPerson.m_Nick.GetLength()>0){
			int iP=objSettings.AddrBook.AddModPerson(dlgNewPerson.m_Nick,dlgNewPerson.m_CompName,dlgNewPerson.m_Dsc,dlgNewPerson.m_bGroup,dlgNewPerson.m_iTab);
			if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
				pMainDialogWindow->dlgAddBook->Refreshlist();
			}
		}
		return;
	}else{
		CItem* item=CItem::GetItemByIndex(iCurrentItem);
		if(item){
			if(pMainDialogWindow){
				pMainDialogWindow->m_TaskList.SelectIItem(item->hIRow,1);
				pMainDialogWindow->m_TaskList.EnsureVisible(item->hIRow,1,TRUE);
				if(!::IsWindowVisible(pMainDialogWindow->GetSafeHwnd())){
					::ShowWindow(pMainDialogWindow->GetSafeHwnd(),SW_SHOW);
				}
			}
			EndDialog(0);
		}else{
			m_Results.DeleteIRow(hRow);
		}
	}
}

BOOL CDLG_Find::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==VK_RETURN){
			OnGo();
			return TRUE;
		}
		if(pMsg->wParam==VK_ESCAPE && bSearchForPersonStarted){
			bSearchForPersonStarted=0;
			GetDlgItem(IDC_STATUS)->SetWindowText(Format("%s. %s: %i",_l("Search finished"),_l("Found"),iFinded));
			return TRUE;
		}
	}
	if(pMsg->message==WM_LBUTTONUP){
		PostMessage(WM_USER,99,99);
	}
	BOOL res=CDialog::PreTranslateMessage(pMsg);
	if(pMsg->message==WM_RBUTTONDOWN){
		OnRclickResults();
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONDBLCLK){
		OnDblclickResults();
		return TRUE;
	}
	if(pMsg->message==WM_ACTIVATE){
		if(pMsg->hwnd==0){
			((CEdit*)GetDlgItem(IDC_MASK))->SetFocus();
			return TRUE;
		}
	}
	return res;
}

BOOL CDLG_Find::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_DSC_TEXT)->SetWindowText(_l("Enter text to search for. Use '*' for any character"));
	if(bSearchForPersons){
		SetWindowText(_l("Find online users"));
	}else{
		SetWindowText(_l("Find items in the tree"));
	}
	GetDlgItem(IDC_GO)->SetWindowText(_l("Find"));
	GetDlgItem(IDC_STATUS)->SetWindowText(_l("Type text and press 'Find' button"));
	//
	Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_Find");
	Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
	CRect minRect(0,0,200,100);
	Sizer.SetMinRect(minRect);
	Sizer.AddDialogElement(IDC_DSC_TEXT,heightSize);
	Sizer.AddDialogElement(IDC_MASK,heightSize);
	Sizer.AddDialogElement(IDC_GO,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_STATUS,heightSize);
	Sizer.AddDialogElement(IDC_RESULTS,0);
	//
	Sizer.AddTopAlign(IDC_DSC_TEXT,0,4);
	Sizer.AddLeftAlign(IDC_DSC_TEXT,0,4);
	Sizer.AddRightAlign(IDC_DSC_TEXT,0,-4);
	Sizer.AddTopAlign(IDC_MASK,IDC_DSC_TEXT,bottomPos,2);
	Sizer.AddLeftAlign(IDC_MASK,0,4);
	Sizer.AddTopAlign(IDC_GO,IDC_DSC_TEXT,bottomPos,2);
	Sizer.AddRightAlign(IDC_GO,0,-4);
	Sizer.AddRightAlign(IDC_MASK,IDC_GO,leftPos,-4);

	Sizer.AddTopAlign(IDC_RESULTS,IDC_MASK,bottomPos,2);
	Sizer.AddLeftBottomAlign(IDC_STATUS,0,0);
	Sizer.AddRightBottomAlign(IDC_STATUS,0,0);
	Sizer.AddBottomAlign(IDC_RESULTS,IDC_STATUS,topPos,-2);
	Sizer.AddRightAlign(IDC_RESULTS,0,0);
	//
	SetIcon(theApp.MainImageList.ExtractIcon(48),TRUE);
	SetIcon(theApp.MainImageList.ExtractIcon(48),FALSE);
	InitResultsList();
	m_Mask="*";
	if(!bSearchForPersons){
		m_Mask=objSettings.sLastFindString;
	}
	GetDlgItem(IDC_MASK)->SetWindowText(objSettings.sLastFindString);
	((CEdit*)GetDlgItem(IDC_MASK))->SetFocus();
	SetTaskbarButton(this->GetSafeHwnd());
	if(bLimit2FB){
		((CEdit*)GetDlgItem(IDC_MASK))->SetWindowText("*");
		SendMessage(WM_COMMAND,IDC_GO,0);
	}
	GetDlgItem(IDC_MASK)->SetFocus();
	return FALSE;
}

BOOL CDLG_Find::InitResultsList()
{
	CRect rt;
	GetClientRect(&rt);
	m_Results.SetReadOnly(FALSE);
	m_Results.SetImageList(&theApp.MainImageList, LVSIL_SMALL);
	ListView_SetExtendedListViewStyleEx(m_Results.m_hWnd, 0, LVS_EX_FULLROWSELECT);
	// Устанавливаем столбцы диаграммы
	m_Results.InsertFColumn( 0, _l("Result"), FALSE, LVCFMT_LEFT, (int)(rt.Width()*.7));
	CString sTitle2=_l("Created");
	if(bSearchForPersons){
		sTitle2=_l("Last presence");
	}
	m_Results.InsertFColumn( 1, sTitle2, FALSE, LVCFMT_LEFT, (int)(rt.Width()*.3));
	m_Results.SetIItemControlType(NULL, 0);
	SetColorStyle();
	m_Results.DeleteAllIRows();
	aFindedUsers.RemoveAll();
	return TRUE;
}

void CDLG_Find::SetColorStyle()
{
	// Прозрачность
	/*
	if(objSettings.objSchemes.getAlpha(objSettings.iMainStyle)==100){
		SetLayeredWindowStyle(this->m_hWnd,FALSE);
	}else{
		SetLayeredWindowStyle(this->m_hWnd,TRUE);
		SetWndAlpha(this->m_hWnd, objSettings.objSchemes.getAlpha(objSettings.iMainStyle),TRUE);
	}
	*/
	m_Results.m_crIItemText=objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	m_Results.m_crIRowBackground=objSettings.objSchemes.getBgColor(objSettings.iMainStyle);
	m_Results.SetBkColor(objSettings.objSchemes.getBgColor(objSettings.iMainStyle));
	m_Results.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	m_Results.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	m_Results.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);

	// Шрифт
	objSettings.objSchemes.getFont(objSettings.iMainStyle)->GetLogFont(&logfont);
	deleteFont(font);
	font=new CFont();
	font->CreateFontIndirect(&logfont);
	m_Results.SetFont(font,FALSE);
	return;
}

void CDLG_Find::OnDestroy() 
{
	objSettings.pCurrentFindDialog=0;
	CDialog::OnDestroy();
}

void CDLG_Find::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
}

CDLG_Find::~CDLG_Find()
{
	deleteFont(font);
}

BOOL CDLG_Find::AddFindPersonResult(CFindedUser& pUser)
{
	if(!bSearchForPersons || !bSearchForPersonStarted){
		return 0;
	}
	CString sTest=pUser.sNick+pUser.sID+pUser.sAddInfo+pUser.sUserDsc;
	int iMissPattern=0;
	if(!PatternMatchIC(sTest, m_Mask)){
		return 0;
	}
	// Ищем есть ли уже такой
	int i=0;
	for(i=0;i<aFindedUsers.GetSize();i++){
		if(aFindedUsers[i].sID==pUser.sID){
			return 0;
		}
	}
	i=aFindedUsers.Add(pUser);
	CString sTitle=pUser.sNick+" ("+pUser.sID+")";
	HIROW hRow=m_Results.InsertIRow(FL_ROOT, FL_LAST, sTitle, pUser.iStatus+theApp.dwOnlineStatusStartBitmap);
	m_Results.SetIItemText(hRow, 1, pUser.sUserDsc);
	m_Results.SetIRowData(hRow,i);
	iFinded++;
	CString sStatus=Format("%s: %i",_l("Search result"),iFinded);
	if(GetTickCount()-bSearchForPersonStarted<(2*60*1000)){
		sStatus+=", ";
		sStatus+=_l("Search in progress, please wait");
		sStatus+=". ";
		sStatus+=_l("ESC to cancel");
	}
	GetDlgItem(IDC_STATUS)->SetWindowText(sStatus);
	return TRUE;
}

void CDLG_Find::OnGo() 
{
	UpdateData();
	if(m_Mask==""){
		m_Mask="*";
	}
	objSettings.sLastFindString=m_Mask;
	//
	if(m_Mask.Find("*")==-1){
		m_Mask="*"+m_Mask+"*";
	}
	// Ищем!
	EnableWindow(FALSE);
	GetDlgItem(IDC_STATUS)->SetWindowText(_l("Searching"));
	iFinded=0;
	m_Results.DeleteAllIRows();
	if(bSearchForPersons){
		bSearchForPersonStarted=GetTickCount();
		COnlineUsers aUsers;
		// Ищем пользователей в онлайне
		if(!bLimit2FB){
			if(objSettings.objMessageWrapper->pMLNet){
				objSettings.objMessageWrapper->pMLNet->FindOnlineUsers(m_Mask,aUsers);
			}
			if(objSettings.objMessageWrapper->pNBNet){
				objSettings.objMessageWrapper->pNBNet->FindOnlineUsers(m_Mask,aUsers);
			}
			if(objSettings.objMessageWrapper->pXPCNet){
				objSettings.objMessageWrapper->pXPCNet->FindOnlineUsers(m_Mask,aUsers);
			}
		}else{
			((CEdit*)GetDlgItem(IDC_MASK))->SetWindowText(_l("Double click user to add it to your adress book"));
			GetDlgItem(IDC_MASK)->EnableWindow(FALSE);
			GetDlgItem(IDC_GO)->EnableWindow(FALSE);
		}
		if(objSettings.objMessageWrapper->pFBNet){
			objSettings.objMessageWrapper->pFBNet->FindOnlineUsers(m_Mask,aUsers);
		}
		for(int i=0;i<aUsers.GetSize();i++){
			AddFindPersonResult(aUsers[i]);
		}
	}else{
		// Ищем элементы в дереве
		int size=objSettings.Items.GetSize();
		for(int i=0;i<size;i++){
			GetDlgItem(IDC_STATUS)->SetWindowText(Format("%s... (%i%%)",_l("Searching"),int(i/size*100)));
			CString sLine="NULL";
			CItem* pItem=objSettings.Items.GetAt(i);
			if(pItem && pItem->GetType()!=0){
				CString sLine=pItem->GetFindText();
				if(PatternMatchIC(sLine, m_Mask)){
					iFinded++;
					HIROW hRow=m_Results.InsertIRow(FL_ROOT, FL_LAST, pItem->GetTitle(), pItem->GetIconNum());
					m_Results.SetIItemText(hRow, 1, pItem->GetCrTime().Format("%d.%m.%Y %H:%M:%S"));
					m_Results.SetIRowData(hRow,pItem->index);
				}
			}
		}
	}
	CString sStatus=Format("%s: %i",_l("Search result"),iFinded);
	if(bSearchForPersons && objSettings.objMessageWrapper->pMLNet){
		sStatus+="; ";
		sStatus+=_l("Search in progress, please wait");
		sStatus+=". ";
		sStatus+=_l("ESC to cancel");
	}
	GetDlgItem(IDC_STATUS)->SetWindowText(sStatus);
	EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_MASK))->SetFocus();
	return;
}
