// DLG_Newtask.cpp : implementation file
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
// CDLG_Newtask dialog


CDLG_Newtask::CDLG_Newtask(CWnd* pPar /*=NULL*/)
	: CDialog(), Track(objSettings.bDoModalInProcess)
{
	//{{AFX_DATA_INIT(CDLG_Newtask)
	m_sName = _T("");
	m_IncludeInRep = FALSE;
	//}}AFX_DATA_INIT
	bWasSemi=0;
	pTask=NULL;
	bNewTask=TRUE;
	szPath=NULL;
	font = NULL;
	m_hBr = NULL;
	isMove=FALSE;
	pParent=pPar;
	m_sDsc.bDisableStandartContextMenu=FALSE;
}

CDLG_Newtask::~CDLG_Newtask()
{
	deleteBrush(m_hBr);
	deleteFont(font);
	if(pParent){
		pParent->EnableWindow(TRUE);
	}
}

void CDLG_Newtask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_Newtask)
	DDX_Control(pDX, ID_FOLDER, m_Folder);
	DDX_Control(pDX, ID_FOLDERSEL, m_FolderSel);
	DDX_Control(pDX, ID_CYCLEBG, m_CycleBg);
	DDX_Control(pDX, IDDELETE, m_Delete);
	DDX_Control(pDX, IDACTIONS, m_Actions);
	DDX_Control(pDX, IDC_COMBO_ICON, m_ToDoIcon);
	DDX_Control(pDX, IDC_COMBO_PR, m_Priority);
	DDX_Control(pDX, IDC_EDIT_NTDSC, m_sDsc);
	DDX_Text(pDX, IDC_EDIT_NTNAME, m_sName);
	DDX_Check(pDX, IDC_INCLUDEINREP, m_IncludeInRep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_Newtask, CDialog)
	//{{AFX_MSG_MAP(CDLG_Newtask)
	ON_WM_ACTIVATE()
	ON_WM_RBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDACTIONS, OnActions)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(ID_CYCLEBG, OnCyclebg)
	ON_BN_CLICKED(ID_FOLDERSEL, OnFoldersel)
	ON_COMMAND(ID_NOTEMENU_NEWFNT, OnNotemenuNewstyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_Newtask message handlers
void CDLG_Newtask::CommitData()
{
	UpdateData(TRUE);
	iLastPriority=m_Priority.GetComboBoxCtrl()->GetCurSel()-2;
	if(pTask){
		pTask->SetTitle(m_sName);
		CString sDesc;
		m_sDsc.GetWindowText(sDesc);
		sDesc.Replace("\r\n","\n");
		pTask->sDsc=sDesc;
		pTask->bIncludeInRep=m_IncludeInRep;
		pTask->SetPriority(iLastPriority);
		pTask->SetIconNum(m_ToDoIcon.GetComboBoxCtrl()->GetCurSel()+TR_ICONS_OFFSET);
		pTask->SaveToFile();
		if(bNewTask){//Сохраняем путь для создания
			objSettings.sLastNewToDoPath=pTask->GetPath();
			//CItem* pParent=pTask->GetItemParent();
		}
		pTask->ItemContentChanged();
	}
}

BOOL CDLG_Newtask::DepictToDoFolder()
{
	if(!pTask){
		m_Folder.SetWindowText("");
	}else{
		CItem* pParent=pTask->GetItemParent();
		if(pParent){
			m_Folder.SetWindowText(pParent->GetTitle());
		}
	}
	m_Folder.Invalidate();
	return TRUE;
}

Task* CreateNewTask(const char* szPath)
{
	Task* pTask=new Task(objSettings.bGenerateRandomNames?GetRandomName():_l("New ToDo"), "");
	int iIndex=objSettings.m_Tasks.AddTask(pTask);
	CString sPath=szPath;
	sPath.TrimLeft();
	sPath.TrimRight();
	pTask->SetPath(sPath);
	CString sParentID,sParentPath;
	CItem* pRoot=pTask->GetItemParent(&sParentID,&sParentPath);
	if(sPath=="" || pRoot==NULL || (sParentID.GetLength()>0 && !pRoot)){
		pTask->SetPath(TODO_PATH);
	}
	if(pRoot){
		if(pRoot->GetColorCode()!=-1){
			pTask->SetColorCode(pRoot->GetColorCode());
		}
		pTask->SetPriority(pRoot->GetPriority());
	}
	if(objSettings.bMakeNewTDCur){
		objSettings.m_Tasks.TaskEnter(pTask->GetID(),TRUE);
	}
	CString sTitle=objSettings.sNewToDo_TitleM;
	SubstMyVariables(sTitle);
	pTask->SetTitle(sTitle);
	CString sBody=objSettings.sNewToDo_BodyM+REGNAG_TEXT;
	SubstMyVariables(sBody);
	pTask->sDsc=sBody;
	pTask->SetState(0);
	return pTask;
}

BOOL CDLG_Newtask::OnInitDialog()
{
	if(pParent){
		pParent->EnableWindow(FALSE);
	}
	CDialog::OnInitDialog();
	Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_NewTask");
	CRect minRect(0,0,DIALOG_MINXSIZE,DIALOG_MINYSIZE),btR;	
	Sizer.SetOptions(STICKABLE|SIZEABLE|SETCLIPSIBL);
	Sizer.SetMinRect(minRect);
	SetButtonSize(this,IDOK,CSize(objSettings.dwNoteButtonSize*3,objSettings.dwNoteButtonSize));
	SetButtonSize(this,IDDELETE,CSize(objSettings.dwNoteButtonSize,objSettings.dwNoteButtonSize));
	SetButtonHeight(this,ID_FOLDER,(UINT)IDC_COMBO_ICON);
	GetDlgItem(IDC_COMBO_ICON)->GetWindowRect(btR);
	SetButtonSize(this,ID_FOLDERSEL,CSize(btR.Height(),btR.Height()));
	Sizer.AddDialogElement(IDC_STATIC2,heightSize);
	Sizer.AddDialogElement(ID_FOLDERSEL,widthSize|heightSize);
	Sizer.AddDialogElement(ID_FOLDER,widthSize|heightSize);
	Sizer.AddDialogElement(IDOK,widthSize|heightSize);
	Sizer.AddDialogElement(IDDELETE,widthSize|heightSize);
	Sizer.AddDialogElement(ID_CYCLEBG,widthSize|heightSize,IDDELETE);
	Sizer.AddDialogElement(IDACTIONS,widthSize|heightSize,IDDELETE);
	Sizer.AddDialogElement(IDC_INCLUDEINREP,heightSize,IDDELETE);
	Sizer.AddDialogElement(IDC_COMBO_PR,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_COMBO_ICON,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_EDIT_NTNAME,heightSize,IDC_COMBO_ICON);
	Sizer.AddDialogElement(IDC_EDIT_NTDSC,0);
	//
	//Sizer.AddRightTopAlign(IDC_EDIT_NTDSC,0,0);
	//Sizer.AddLeftBottomAlign(IDC_EDIT_NTDSC,0,0);
	Sizer.AddLeftTopAlign(IDC_COMBO_ICON,0,4);
	Sizer.AddRightTopAlign(IDC_EDIT_NTNAME,0,4);
	Sizer.AddTopAlign(IDC_COMBO_ICON,IDC_EDIT_NTNAME,0);
	Sizer.AddTopAlign(IDC_STATIC2,IDC_EDIT_NTNAME,bottomPos,2);
	Sizer.AddTopAlign(IDC_COMBO_PR,IDC_EDIT_NTNAME,bottomPos,2);
	Sizer.AddTopAlign(ID_FOLDER,IDC_EDIT_NTNAME,bottomPos,2);
	Sizer.AddTopAlign(ID_FOLDERSEL,IDC_EDIT_NTNAME,bottomPos,2);
	Sizer.AddTopAlign(IDC_EDIT_NTDSC,IDC_STATIC2,bottomPos,4);
	//
	Sizer.AddLeftAlign(IDC_STATIC2,0,4);
	Sizer.AddLeftAlign(IDC_EDIT_NTNAME,IDC_COMBO_ICON,rightPos,2);
	Sizer.AddLeftAlign(IDC_EDIT_NTDSC,0,4);
	Sizer.AddRightAlign(ID_FOLDERSEL,0,-4);
	Sizer.AddRightAlign(ID_FOLDER,ID_FOLDERSEL,leftPos,-2);
	Sizer.AddRightAlign(IDC_COMBO_PR,ID_FOLDER,leftPos,-2);
	Sizer.AddRightAlign(IDC_STATIC2,IDC_COMBO_PR,leftPos,-2);
	Sizer.AddRightAlign(IDC_EDIT_NTDSC,0,-4);
	//
	Sizer.AddBottomAlign(IDOK,0,-4);
	Sizer.AddBottomAlign(ID_CYCLEBG,0,-4);
	Sizer.AddBottomAlign(IDDELETE,0,-4);
	Sizer.AddBottomAlign(IDACTIONS,0,-4);
	Sizer.AddSideCenterAlign(IDC_INCLUDEINREP,ID_CYCLEBG,0);
	//
	Sizer.AddRightAlign(IDOK,0,-4);
	Sizer.AddRightAlign(ID_CYCLEBG,IDOK,leftPos,-4);
	Sizer.AddRightAlign(IDACTIONS,ID_CYCLEBG,leftPos,-4);
	Sizer.AddRightAlign(IDC_INCLUDEINREP,IDACTIONS,leftPos,-3);
	Sizer.AddLeftAlign(IDDELETE,0,4);
	Sizer.AddLeftAlign(IDC_INCLUDEINREP,IDDELETE,rightPos,3);
	//
	Sizer.AddBottomAlign(IDC_EDIT_NTDSC,IDOK,topPos,-4);
	//
	GetDlgItem(IDOK)->SetWindowText(_l("OK"));
	GetDlgItem(IDC_STATIC2)->SetWindowText(_l("Priority")+":");
	GetDlgItem(IDC_INCLUDEINREP)->SetWindowText(_l("Include ToDo in report"));
	if(objSettings.bUnderWindows98){
		m_Delete.SetBitmap(IDB_BM_DEL);
	}else{
		m_Delete.SetBitmap(IDB_BM_DELPERSON);
	}
	m_Delete.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_Actions.SetBitmap(IDB_BM_OPTIONS);
	m_Actions.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_CycleBg.SetBitmap(IDB_BM_CHANGECOL);
	m_CycleBg.SetStyles(CBS_FLAT|CBS_HIQUAL);
	m_FolderSel.SetBitmap(_bmp().GetRaw(_IL(5)));
	m_Folder.SetReadOnly();
	if(objSettings.bToDoWordWrap){
		m_sDsc.SetTargetDevice(NULL, 0);
	}else{
		m_sDsc.SetTargetDevice(NULL, 1);
	}
	//
	if(pTask){
		bNewTask=FALSE;
	}else{
		bNewTask=TRUE;
		pTask=CreateNewTask(szPath);
	}
	//
	m_ToDoIconImages.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 2);
	AddBitmapToIList(m_ToDoIconImages,IDB_IMAGELIST);
	m_ToDoIcon.SetImageList(&m_ToDoIconImages);
	InitTRIconList(m_ToDoIcon);

	m_ToDoPrImages.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 2);
	AddBitmapToIList(m_ToDoPrImages,IDB_IMAGELIST);
	m_Priority.SetImageList(&m_ToDoPrImages);
	int iCount=0,iCount2=PRIOR_ICONS_OFFSET-2;
	AddComboBoxExItem(&m_Priority,0,_l(theApp.aPriorityStrings[0]),iCount2++);
	AddComboBoxExItem(&m_Priority,1,_l(theApp.aPriorityStrings[1]),iCount2++);
	AddComboBoxExItem(&m_Priority,2,_l(theApp.aPriorityStrings[2]),iCount2++);
	AddComboBoxExItem(&m_Priority,3,_l(theApp.aPriorityStrings[3]),iCount2++);
	AddComboBoxExItem(&m_Priority,4,_l(theApp.aPriorityStrings[4]),iCount2++);
	//
	CString sTitle=objSettings.bGenerateRandomNames?GetRandomName():_l("New ToDo");
	m_sName=sTitle;
	if(pTask){
		pTask->pCorDialog=this;
		SetColorCode(pTask->GetColorCode());
		if(!bNewTask){
			sTitle=_l("Modify ToDo");
		}
		m_sName=pTask->GetTitle();
		CString sDesc=pTask->sDsc;
		sDesc.Replace("\n","\r\n");
		SetDscText(sDesc);
		m_IncludeInRep=pTask->bIncludeInRep;
		iLastPriority=pTask->GetPriority()+2;
		m_Priority.GetComboBoxCtrl()->SetCurSel(iLastPriority);
		if(m_ToDoIcon.GetComboBoxCtrl()->SetCurSel(pTask->GetIconNum()-TR_ICONS_OFFSET)==CB_ERR){
			m_ToDoIcon.GetComboBoxCtrl()->SetCurSel(0);
		}
		DepictToDoFolder();
	}else{
		m_IncludeInRep=1;
		m_Priority.GetComboBoxCtrl()->SetCurSel(2);
		iLastPriority=2;
		m_ToDoIcon.GetComboBoxCtrl()->SetCurSel(0);
	}
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.SetMaxTipWidth(objSettings.rDesktopRectangle.Width()/2);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDOK), _l("Apply changes (Ctrl-Enter)"));
		m_tooltip.AddTool(GetDlgItem(IDDELETE), _l("Delete ToDo (Ctrl-D)"));
		m_tooltip.AddTool(GetDlgItem(IDC_INCLUDEINREP), _l("Include this ToDo in report"));
		m_tooltip.AddTool(GetDlgItem(IDACTIONS), _l("Additional actions menu"));
		m_tooltip.AddTool(GetDlgItem(ID_CYCLEBG), _l("Cycle ToDo color"));
		m_tooltip.AddTool(GetDlgItem(IDC_COMBO_PR), _l("ToDo priority"));
		m_tooltip.AddTool(GetDlgItem(IDC_COMBO_ICON), _l("ToDo priority"));
		m_tooltip.AddTool(GetDlgItem(ID_FOLDER), _l("Create in folder")+"\n"+_l("Double-click to select recently used folder"));
		m_tooltip.AddTool(GetDlgItem(ID_FOLDERSEL), _l("Create in folder"));
	}
	//
	SetWindowText((pTask->IsShared()?CString("*"):CString(""))+sTitle);//	SetWindowText(bNewTask?_l("New ToDo"):_l("Modify ToDo"));
	int iIcon=m_ToDoIcon.GetComboBoxCtrl()->GetCurSel()+TR_ICONS_OFFSET;//(pTask?pTask->GetIconNum():0)+TR_ICONS_OFFSET;
	SetIcon(m_ToDoIconImages.ExtractIcon(iIcon),TRUE);
	SetIcon(m_ToDoIconImages.ExtractIcon(iIcon),FALSE);
	UpdateData(FALSE);
	SetTaskbarButton(this->GetSafeHwnd());
	return TRUE;
}

void CDLG_Newtask::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

void CDLG_Newtask::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	((CEdit*)GetDlgItem(IDC_EDIT_NTNAME))->SetFocus();
	((CEdit*)GetDlgItem(IDC_EDIT_NTNAME))->SetSel(0xFFFF0000);
	Sizer.ApplyLayout();
}


LRESULT CDLG_Newtask::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CDLG_Newtask::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	UINT iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	if(pMsg->message==WM_KEYDOWN){
		if((isPressed(VK_CONTROL) && pMsg->wParam=='V') || (isPressed(VK_SHIFT) && pMsg->wParam==VK_INSERT)){
			m_sDsc.PasteSpecial(CF_TEXT);
			return TRUE;
		}
		if(isPressed(VK_CONTROL)){
			if(pMsg->wParam==VK_RETURN){
				OnOK();
				return TRUE;
			}
			if(pMsg->wParam=='S'){
				if(pTask && !bNewTask){
					CommitData();
					HCURSOR hCur=SetCursor(theApp.LoadCursor(IDC_CURSORSAVE));
					pTask->SaveToFile();
					SetCursor(hCur);
					return TRUE;
				}
			}
			if(pMsg->wParam=='B' && pTask){
				pTask->ShareItem(1,0);
				return TRUE;
			}
			if(pMsg->wParam=='D'){
				OnDelete();
				return TRUE;
			}
			if(pMsg->wParam=='O'){
				if(pMainDialogWindow && pTask){
					pMainDialogWindow->iCurrentItem=pTask->index;
					pMainDialogWindow->OnSystrayToDoAddOptions();
				}
				return TRUE;
			}
		}
	}
	if(pMsg->message==WM_LBUTTONDBLCLK){
		if(iCtrlID==ID_FOLDER){
			OnFolderDblClick();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDLG_Newtask::OnActions() 
{
	if(pMainDialogWindow){
		CRect rt;
		GetDlgItem(IDACTIONS)->GetWindowRect(&rt);
		CPoint pos(rt.TopLeft());
		SetCursorPos(pos.x,pos.y);
		pMainDialogWindow->TrackTaskPopupMenu(pTask,TRUE);
	}
}

void CDLG_Newtask::OnOK() 
{
	CommitData();
	Finalize(IDOK);
}

void CDLG_Newtask::OnCancel()
{
	Finalize(IDCANCEL);
}

void CDLG_Newtask::OnDelete() 
{
	Finalize(IDABORT);
}

void CDLG_Newtask::Finalize(int iRes)
{
	pTask->pCorDialog=NULL;// Мы теперь и сами удалимся без проблем...
	if(bNewTask && iRes!=IDOK){
		iRes=IDABORT;
	}
	if(pMainDialogWindow){
		pMainDialogWindow->UpdateFeditIfNeeded(pTask);
	}
	if(iRes==IDABORT){
		if(objSettings.m_Tasks.DeleteTask(pTask->GetID())!=TRUE){
			// ToDo не найдена, просто удаляем
			delete pTask;
		}
		pTask=NULL;
	}
	DestroyWindow();
}

void CDLG_Newtask::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

void CDLG_Newtask::SetColorCode(long iCode)
{
	//Шрифт...
	objSettings.objSchemes.getFont(iCode)->GetLogFont(&logfont);
	deleteFont(font);
	font=new CFont();
	font->CreateFontIndirect(&logfont);
	GetDlgItem(IDC_EDIT_NTDSC)->SetFont(font,FALSE);
	m_sDsc.SetFont(font,FALSE);
	// Цвет фона...
	DWORD cBGColor=objSettings.objSchemes.getBgColor(iCode);
	DWORD cTXColor=objSettings.objSchemes.getTxtColor(iCode);
	deleteBrush(m_hBr);
	m_hBr = new CBrush(cBGColor);
	m_FolderSel.SetBrush(cBGColor);
	// Цвет текста...
	m_sDsc.SetColor(cTXColor,cBGColor);
	// Прозрачность...
	if(objSettings.objSchemes.getAlpha(iCode)==100){
		// Не выключаем больше никогда
		if(!bWasSemi){
			SetLayeredWindowStyle(this->m_hWnd,FALSE);
		}else{
			SetLayeredWindowStyle(this->m_hWnd,TRUE);
			SetWndAlpha(this->m_hWnd, 100,TRUE);
		}
	}else{
		bWasSemi=1;
		SetLayeredWindowStyle(this->m_hWnd,TRUE);
		SetWndAlpha(this->m_hWnd, objSettings.objSchemes.getAlpha(iCode),TRUE);
	}
	Invalidate();
	GetDlgItem(IDC_EDIT_NTDSC)->Invalidate();
	GetDlgItem(IDC_EDIT_NTNAME)->Invalidate();
	GetDlgItem(IDC_STATIC2)->Invalidate();
	GetDlgItem(ID_FOLDER)->Invalidate();
	GetDlgItem(IDC_INCLUDEINREP)->Invalidate();
}

HBRUSH CDLG_Newtask::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(pTask){
		pDC->SetTextColor(objSettings.objSchemes.getTxtColor(pTask->GetColorCode()));
		pDC->SetBkColor(objSettings.objSchemes.getBgColor(pTask->GetColorCode()));
		return HBRUSH(*m_hBr);
	}else{
		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	}
}

void CDLG_Newtask::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fromPoint=point;
	isMove=TRUE;
	SetCapture();
}

void CDLG_Newtask::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (isMove)
		ReleaseCapture();
	isMove=FALSE;
}

void CDLG_Newtask::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (isMove){
		RECT rect;
		GetWindowRect( (LPRECT) &rect );
		rect.right=rect.right-rect.left;
		rect.bottom=rect.bottom-rect.top;
		rect.left-=fromPoint.x-point.x;
		rect.top-=fromPoint.y-point.y;
		MoveWindow(rect.left,rect.top,rect.right,rect.bottom);
	}
}

void CDLG_Newtask::OnCyclebg() 
{
	if(pTask){
		pTask->SetColorCode(pTask->GetColorCode()+1);	
	}
}

void CDLG_Newtask::OnRButtonUp(UINT nFlags, CPoint point) 
{
/*	RefreshMenu(nFlags);
	CPoint pos;
	GetCursorPos(&pos);
	m_pMenu->SetDefaultItem(0,TRUE);
	::TrackPopupMenu(m_pMenu->GetSafeHmenu(), 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);*/
	if(pMainDialogWindow){
		pMainDialogWindow->TrackTaskPopupMenu(pTask,TRUE);
	}
}

void CDLG_Newtask::OnFolderDblClick()
{
	if(!pMainDialogWindow || objSettings.lToDoFRecentList.GetSize()==0){
		return;
	}
	CRect rt;
	m_Folder.GetWindowRect(&rt);
	CMenu NewMenu;
	NewMenu.CreatePopupMenu();
	for(int i=0;i<objSettings.lToDoFRecentList.GetSize();i++){
		CItem* pItem=CItem::GetItemByID(objSettings.lToDoFRecentList[i]);
		if(pItem){
			AddMenuString(&NewMenu,1+i,pItem->GetTitle(DEF_MENUTITLELEN));
		}
	}
	int iItem=::TrackPopupMenu(NewMenu.GetSafeHmenu(), TPM_RETURNCMD, rt.left, rt.bottom, 0, this->GetSafeHwnd(), NULL);
	if(iItem>0){
		CItem* pItem=CItem::GetItemByID(objSettings.lToDoFRecentList[iItem-1]);
		if(pItem){
			pMainDialogWindow->DragItem(pTask,pItem);
		}
		DepictToDoFolder();
	}
}

void CDLG_Newtask::OnFoldersel() 
{
	if(!pMainDialogWindow){
		return;
	}
	CRect rt;
	m_FolderSel.GetWindowRect(&rt);
	CMenu NewMenu;
	NewMenu.CreatePopupMenu();
	pMainDialogWindow->CreateMenuTree(NewMenu, "", 2);
	int iItem=::TrackPopupMenu(NewMenu.GetSafeHmenu(), TPM_RETURNCMD, rt.left, rt.top, 0, this->GetSafeHwnd(), NULL)-WM_USER-SWITCH_TO;
	CItem* pItem=CItem::GetItemByIndex(iItem);
	if(pItem){
		pMainDialogWindow->DragItem(pTask,pItem);
	}
	DepictToDoFolder();
	CItem* pParent=pTask->GetItemParent();
	if(pParent){
		objSettings.lToDoFRecentList.InsertAt(0,pParent->GetID());
		if(objSettings.lToDoFRecentList.GetSize()>objSettings.lMsgRecentListSize){
			objSettings.lToDoFRecentList.SetSize(objSettings.lMsgRecentListSize);
		}
	}
}

void CDLG_Newtask::OnNotemenuNewstyle()
{
	HWND h=GetSafeHwnd();

	CString sID=pTask->GetID();
	CallStylesDialogWnd(pMainDialogWindow,pTask->GetColorCode()-1);
	CItem* item=CItem::GetItemByID(sID);
	if(item){
		// Так как итем может умереть за это время...
		item->SetColorCode(objSettings.iLastSchemeUsed+1);
	}
}

void CDLG_Newtask::SetDscText(CString sDesc)
{
	m_sDsc.SetText(sDesc);
}