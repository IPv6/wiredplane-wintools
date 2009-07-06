тексты заметок писать и не на текмесяце

/*
- Подглючить на 666ой обоине :)
- поддержка прокси к эксплореру
- Добавить Outlookовские записи из дневника на календарь
- эффекты от FAST, SIRDS
*/
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "dlg_wchanger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL IsVista();
int& AddDefsWallps();
BOOL IsHelpEnabled2();
/////////////////////////////////////////////////////////////////////////////
// CDLG_WChanger dialog
CDLG_WChanger::CDLG_WChanger(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_WChanger::IDD, pParent),m_Preview(this)
{
	//{{AFX_DATA_INIT(CDLG_WChanger)
	m_Hour = _T("");
	m_Min = _T("");
	m_UsePreview = FALSE;
	m_Activate = FALSE;
	m_CycleType = -1;
	//}}AFX_DATA_INIT
	hLastSelected=0;
	iTemplatesInit=0;
	sCurrentWidget="";
	lWndMode=objSettings.lDefMWTab;
	iNotesIndex=-1;
	lTemplDupIndes=0;
	lTemplDowIndes=0;
	sImageToBeSetAfterClose[0]=0;
	hDownButtonField=0;
	lZipLock=0;
	bForceSelWallpaper=0;
	bNeddRefreshAfterClose=0;
	mainMenu=0;
	lSortLock=0;
	wpNowShowingTheme=0;
	iLastWPSetted=iLastPreviewItem=iFirstIndex=iCurrentItem=-1;
	bLastDrawPicRes=FALSE;
	bDataLoaded=0;
	hPreviewBmp=NULL;
	sWallpaperFile="";
	sizeOfRealImage.cx=sizeOfRealImage.cy=0;
	font=NULL;
	lLoadInProgress=0;
	sPreparedWallpaperFile="";
	bDialogInitOk=0;
	m_imageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 2);
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_IMAGELIST);
	m_imageList.Add(&bitmap, RGB(255, 255, 255)); 
}

CDLG_WChanger::~CDLG_WChanger()
{
	ClearBitmap(hPreviewBmp);
	deleteFont(font);
	if(mainMenu){
		delete mainMenu;
	}
}

CString sWeightDesc="How often theme will be used\n"
					"in the wallpaper rotation";
CString sWeightDescW="How often widget will be\n"
					 "used for desktop background";
void CDLG_WChanger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_WChanger)
	DDX_Control(pDX, IDC_MAINTAB, m_MainTab);
	DDX_Control(pDX, IDC_ACTIVE_THEME, m_ThActive);
	DDX_Control(pDX, IDC_BT_REM_IMG, m_BtRemImg);
	DDX_Control(pDX, IDC_BT_THMISC, m_BtImgMisc);
	DDX_Control(pDX, IDC_BT_SYNC, m_BtThSync);
	
	DDX_Control(pDX, IDC_BT_CAL, m_BtCal);
//	DDX_Control(pDX, IDC_BT_CALR, m_BtCalr);
	DDX_Control(pDX, IDC_BT_NEWTH, m_BtThNew);
	DDX_Control(pDX, IDC_BT_ADD_DIR, m_BtAddDir);
	DDX_Control(pDX, IDC_BT_ADD_FILE, m_BtAddFile);
	DDX_Control(pDX, IDC_BT_ADD_ARCH, m_BtAddArch);
	DDX_Control(pDX, IDC_BT_REM, m_BtRemove);
	DDX_Control(pDX, IDC_SETNOW, m_BtSetNow);
	DDX_Control(pDX, IDC_SCR_WEIGHT, m_WeightEdit);
	DDX_Control(pDX, IDC_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_LIST_WPAPERS, m_WList);
	DDX_Control(pDX, IDC_LIST_WIDGETS, m_DList);
	DDX_Text(pDX, IDC_HOUR, m_Hour);
	DDX_Text(pDX, IDC_MIN, m_Min);
	DDX_Check(pDX, IDC_PREVIEW_CHECK, m_UsePreview);
	DDX_Check(pDX, IDC_CHECK_ACTIVE, m_Activate);
	DDX_CBIndex(pDX, IDC_COMBO_SEQUENCE, m_CycleType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDLG_WChanger, CDialog)
	//{{AFX_MSG_MAP(CDLG_WChanger)
	ON_COMMAND(ID_WCHANGER_ADDFILE, OnWchangerAddfile)
	ON_COMMAND(ID_WCHANGER_ADDARCHIVE, OnWchangerAddArch)
	ON_COMMAND(ID_WCHANGER_DELETEALL, OnWchangerDeleteall)
	ON_COMMAND(ID_WCHANGER_DELETEFOLDER, OnWchangerDeletefolder)
	ON_COMMAND(ID_WCHANGER_DELETESINGLETHEME, OnWchangerDeleteTheme)
	ON_COMMAND(ID_WCHANGER_EXCLUDEIMAGES, OnWchangerDisableImg)
	ON_COMMAND(ID_WCHANGER_NEWFOLDER, OnWchangerNewfolder)
	ON_COMMAND(ID_WCHANGER_SORTBYNAME, OnWchangerSortByName)
	ON_COMMAND(ID_WCHANGER_SHUFFLE, OnWchangerSortShuffle)
	ON_COMMAND(ID_WCHANGER_SORTBYTYPE, OnWchangerSortByType)
	ON_COMMAND(ID_WCHANGER_SORTBYDIMENSIONS, OnWchangerSortByDims)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_ACTIVE_THEME, OnThActive)
	ON_BN_CLICKED(IDC_PREVIEW_CHECK, OnPreviewCheck)
	ON_BN_CLICKED(IDC_SETNOW, OnSetnow)
	ON_COMMAND(ID_WCHANGER_SETWPNOW, OnSetnowWallp)
	ON_COMMAND(ID_ADDFULLFOLDER, OnAddfullfolder)
	ON_COMMAND(ID_W_ADDDIRECTORY, OnAddfullfolderNew)
	ON_COMMAND(ID_WCHANGER_ADDFOLDERAC, OnAddfullfolderAC)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_THEMEFITTING, OnSelchangeComboThemefitting)
	ON_COMMAND(ID_WCHANGER_ADDHYPERLINK, OnWchangerAddhyperlink)
	ON_BN_CLICKED(IDC_BT_ADD_DIR, OnBtAddDir)
	ON_BN_CLICKED(IDC_BT_THMISC, OnBtSortTh)
	ON_BN_CLICKED(IDC_BT_SYNC, OnBtSyncTh)
	ON_BN_CLICKED(IDC_BT_REM_IMG, OnBtRemImg)
	ON_BN_CLICKED(IDC_BT_ADD_FILE, OnBtAddFile)
	ON_BN_CLICKED(IDC_BT_ADD_ARCH, OnBtAddArch)
	ON_BN_CLICKED(IDC_BT_REM, OnBtRem)
	ON_BN_CLICKED(IDC_BT_CAL, OnBtCal)
//	ON_BN_CLICKED(IDC_BT_CALR, OnBtCalR)
	ON_BN_DOUBLECLICKED(IDC_PREVIEW, OnPreviewClick)
	ON_COMMAND(ID_WCHANGER_REMOVEBROKEN, OnWchangerRemovebroken)
	ON_COMMAND(ID_WCHANGER_REMOVEDUPLICATES, OnWchangerRemoveDuplicates)
	ON_COMMAND(ID_WCHANGER_REMOVEALLDUPLICATES, OnWchangerRemoveDuplicatesAll)
	ON_COMMAND(ID_WCHANGER_REMOVEFLDDUPL, OnWchangerRemoveDuplicatesFld)
	ON_COMMAND(ID_WCHANGER_SETASSCR, OnWchangerSetScr)
	ON_COMMAND(ID_WCHANGER_NEWPRESET, OnWchangerNewPreset)
	ON_COMMAND(ID_WCHANGER_DELPRESET, OnWchangerDelPreset)
	ON_COMMAND(ID_WCHANGER_PRESETHLP, OnWchangerPresetHlp)
	ON_COMMAND(ID_FOLDER_RENAMEFOLDER, OnWchangerRenameTheme)
	ON_COMMAND(ID_WCHANGER_RENAMEIMAGE, OnWchangerRenameImage)
	ON_COMMAND(ID_WCHANGER_FOLDERSYNC, OnWchangerFoldersync)
	ON_COMMAND(ID_WCHANGER_UPDATEFROMIMAGESFOLDER, OnWchangerFoldersyncNow)
	ON_COMMAND(ID_WCHANGER_SETSYNCHROFOLDER, OnWchangerFoldersyncSet)
	ON_COMMAND(ID_WCHANGER_REMOVEWITHIMAGE, OnWchangerRemovewithimage)
	ON_CBN_SELCHANGE(IDC_COMBO_SEQUENCE, OnSelchangeComboSequence)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE, OnCheckActive)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAINTAB, OnSelchangeMaintab)
	ON_COMMAND(ID_WCHANGER_IMPORTLIST, OnWchangerImportList)
	ON_COMMAND(ID_WCHANGER_BACKUP, OnWchangerBackupAll)
	ON_COMMAND(ID_WCHANGER_RESTORE, OnWchangerRestoreAll)
	ON_COMMAND(ID_WCHANGER_PACKINTOARCHIVE, OnPackIntoArch)
	ON_BN_CLICKED(IDC_BT_NEWTH, OnWchangerNewfolder)
	ON_WM_PAINT()
	ON_NOTIFY(FLNM_SELCHANGED, IDC_LIST_WIDGETS, OnWidgetSelChanged)
	ON_NOTIFY(FLMN_CHECKBOX_TRIGGER, IDC_LIST_WIDGETS, OnWidgetCTriggerChanged)
	ON_NOTIFY(FLNM_EXPAND, IDC_LIST_WPAPERS, OnExpandWList)
	ON_NOTIFY(FLNM_ENDEDIT, IDC_LIST_WPAPERS, OnEndEditWList)
	ON_NOTIFY(FLMN_CHECKBOX_TRIGGER, IDC_LIST_WPAPERS, OnEndEditWList)
	ON_NOTIFY(FLNM_END_INPLACE_COMBO, IDC_LIST_WPAPERS, OnEndEditWList)
	ON_NOTIFY(FLNM_SELCHANGED, IDC_LIST_WPAPERS, OnSelChanged)
	ON_BN_CLICKED(IDAPPLY_MAIN, OnBnClickedOptions)
	ON_NOTIFY(FLNM_CUSTOMDIALOG, IDC_LIST_WIDGETS, OnWidgetCustomDialog)
	ON_BN_CLICKED(IDC_BT_WOPT, OnBtWopt)
	ON_BN_CLICKED(IDC_WIDGETDROPDOWN, OnBtWoptDDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_WChanger message handlers
BOOL CDLG_WChanger::UpdateThemeWeightVal(WPaper* wpTheme,int iNewVal)
{
	if(lWndMode){
		GetDSelectedRow();
		CWPT* wpt=GetWPT(sCurrentWidget);
		if(sCurrentWidget=="" || !wpt){
			return 0;
		}
		if(iNewVal<0){
			long lPrevWe=wpt->GetWeight();
			DWORD dwWe=m_WeightEdit.GetPos();
			wpt->SetWeight(dwWe);
			if(dwWe==0){
				SingleSwitchTemplate(sCurrentWidget,0);
			}else if(lPrevWe==0){
				SingleSwitchTemplate(sCurrentWidget,1);
			}
		}else{
			wpt->SetWeight(iNewVal);
			m_WeightEdit.SetPos(iNewVal);
		}
		if(wpt->hIRow){
			m_DList.SetIItemText(wpt->hIRow,0,wpt->GetFTitle());
		}
		GetDlgItem(IDC_STAT_WEIGHT_VAL)->SetWindowText(Format("%lu%%",wpt->GetWeight()));
	}else if(wpTheme){
		if(wpTheme->dwStatus){
			lThemesWTotal-=wpTheme->dwTheme;
		}
		if(iNewVal<0){
			wpTheme->dwTheme=m_WeightEdit.GetPos();
		}else{
			wpTheme->dwTheme=iNewVal;
			m_WeightEdit.SetPos(iNewVal);
		}
		if(wpTheme->dwStatus){
			lThemesWTotal+=wpTheme->dwTheme;
		}
		if(wpTheme->dwTheme>0){
			GetDlgItem(IDC_STAT_WEIGHT_VAL)->SetWindowText(Format("%lu%%",wpTheme->dwTheme));
		}else{
			GetDlgItem(IDC_STAT_WEIGHT_VAL)->SetWindowText(_l("None"));
		}
		if(lThemesWTotal>100){
			m_tooltip.UpdateTipText(Format("%s: %s! %s: %i%%",_l("Warning"),_l("Total weight of all active\nthemes is too big"),_l("Total weight"),lThemesWTotal)+"\n"+_l(sWeightDesc),GetDlgItem(IDC_SCR_WEIGHT));
		}else{
			m_tooltip.UpdateTipText(_l("Theme weight (percents)")+"\n"+_l(sWeightDesc),GetDlgItem(IDC_SCR_WEIGHT));
		}
	}
	return TRUE;
}


void CDLG_WChanger::SetThemeNameInTitle(WPaper* wp)
{
	if(lWndMode){
		GetDSelectedRow();
		CWPT* wpt=GetWPT(sCurrentWidget);
		if(sCurrentWidget=="" || !wpt){
			GetDlgItem(IDC_STAT_THEM)->SetWindowText(_l("Current widget"));
			return;
		}
		GetDlgItem(IDC_STAT_THEM)->SetWindowText(Format("%s: %s",_l("Widget"),_l(wpt->sTitle)));
		m_ThActive.SetCheck(IsWPTActive(sCurrentWidget)?1:0);
		//UpdateData(FALSE);
	}else{
		if(wp){
			GetDlgItem(IDC_STAT_THEM)->SetWindowText(Format("%s: %s",_l("Theme"),wp->sTitle));
			m_ThActive.SetCheck(wp->dwStatus?1:0);
		}else{
			GetDlgItem(IDC_STAT_THEM)->SetWindowText(_l("Current theme"));
		}
	}
	CString sTitle=_l("Desktop widgets");
	if(objSettings.sActivePreset!=""){
		sTitle+=Format(" [%s]",objSettings.sActivePreset);
	}
	char sz[256]={0};
	strcpy(sz,sTitle);
	TCITEM tc;
	memset(&tc,0,sizeof(tc));
	tc.mask=TCIF_TEXT;
	tc.pszText=sz;
	m_MainTab.SetItem(1,&tc);
}

HWND wndShotPhase2=0;
CMenu* hShotPhase2=0;
LRESULT CDLG_WChanger::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WBT_EXIT){
		CloseThisWindow();
		return TRUE;
	}
	if(message==WM_LBUTTONDOWN || message==WM_RBUTTONDOWN || message==WM_KEYDOWN){
		if(wndShotPhase2){
			::PostMessage(wndShotPhase2,WM_CLOSE,0,0);
			::DestroyWindow(wndShotPhase2);
			wndShotPhase2=0;
		}
	}
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		if(message=WM_SIZE){
			CRect rt;
			m_WList.GetClientRect(&rt);
			objSettings.iColWChWidth=rt.Width()-1;
			m_WList.SetColumnWidth(0,objSettings.iColWChWidth);
			m_DList.GetClientRect(&rt);
			m_DList.SetColumnWidth(0,rt.Width()-1);
		}
		return DispatchResultFromSizer;
	}
	if(message==WM_HSCROLL){
		UINT iCtrlID=::GetDlgCtrlID(::GetFocus());
		if(iCtrlID!=IDC_LIST_WPAPERS && iCtrlID!=IDC_LIST_WIDGETS){
			UpdateThemeWeightVal(wpNowShowingTheme,-1);
			return TRUE;
		}
	}
	if(message==WM_COMMAND){
		if(pMainDialogWindow && (
			wParam==ID_SYSTRAY_EXIT || wParam==ID_SYSTRAY_GOTOWEBSITE || wParam==ID_SYSTRAY_REGISTER
			|| wParam==ID_SYSTRAY_HELP || wParam==ID_SYSTRAY_ABOUT || wParam==ID_SYSTRAY_DOWNLOADSKINS || wParam==ID_SYSTRAY_DOWNLOADCLOCKS
			|| wParam==ID_SYSTRAY_SENDASUGGESTION || wParam==ID_SYSTRAY_CHECKFORUPDATES || wParam==ID_SYSTRAY_EXCHANGEWALLPAPER || wParam==ID_SYSTRAY_DOWNLOADTEMPLATES
			|| wParam==ID_SYSTRAY_DOWNLOADWALLPAPERS || wParam==ID_SYSTRAY_BACKUPLIST || wParam==ID_SYSTRAY_RESTORELIST || wParam==ID_SYSTRAY_TELLAFRIEND || wParam==ID_SYSTRAY_HIDESHOWICONS
			|| wParam==ID_SYSTRAY_IMPORTCLOCK || wParam==ID_SYSTRAY_OPEN2 || wParam==ID_SYSTRAY_TUTO
			)){
			pMainDialogWindow->PostMessage(message, wParam, lParam);
			return TRUE;
		}
		if(wParam==ID_SYSTRAY_OPTIONS){
			OnBnClickedOptions();
		}
	}
	if(message==WM_COMMAND && wParam==ID_WCHANGER_UPDATECURWALLPOS){
		SetLastWPSetted(lParam,1);
		GetSelectedRow();
		UpdateThemeControls(lParam);
		return TRUE;
	}
	if(message==WM_COMMAND && wParam==ID_WCHANGER_REFRESHCYCLESTATUS){
		m_Activate=objSettings.lChangePriodically;
		UpdateData(FALSE);
		OnCheckActive();
		return TRUE;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDLG_WChanger::InitWList()
{
	m_WList.SetReadOnly(FALSE);
	m_WList.SetImageList(&m_imageList, LVSIL_SMALL);
	ListView_SetExtendedListViewStyleEx(m_WList.m_hWnd, 0, LVS_EX_FULLROWSELECT);
	m_WList.InsertFColumn( 0, _l("Themes and Images"), TRUE, objSettings.bUsePathEllipses?(-DT_PATH_ELLIPSIS):LVCFMT_LEFT, objSettings.iColWChWidth);
	m_WList.SetFColumnImageOffset(0, 1);
	m_WList.SetIItemControlType(NULL, 0);
	m_WList.bLockUpdateForCollapse=TRUE;
	m_WList.SetViewOptions(VIEW_MSEL);
	//
	m_WList.m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
	m_WList.m_crIRowBackground=GetSysColor(COLOR_WINDOW);
	m_WList.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_WList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	m_WList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	m_WList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);
	//
	m_WList.DeleteAllIRows();
	m_MainTab.InsertItem(0,_l("Wallpaper images"));
	m_MainTab.InsertItem(1,_l("Desktop widgets"));
	lWndMode=objSettings.lDefMWTab;
	if(lWndMode){
		On1Tab();
	}else{
		On0Tab();
	}
}

DWORD WINAPI AddRekolbassedFile(LPVOID p)
{		//не работает
	CDLG_WChanger* pt=(CDLG_WChanger*)p;
	if(pt && objSettings.sAddThisFileAfterOpenWPList!="" && pt->dwFirstTheme!=-1){
		int iThemeToPut=-1;
		if(objSettings.m_WChanger.aUsedThemes.GetSize()>1){
			SmartWarning(_l("Add wallpaper to gallery: Choose theme or cancel operation")+"\n'"+TrimMessage(objSettings.sAddThisFileAfterOpenWPList,50,2)+"'",_l("Adding wallpaper"),&objSettings.lReportAddWPNow,DEF_WARNTIME);
			SwitchToWindow(pt->GetSafeHwnd(),TRUE);
			if(hShotPhase2){
				delete hShotPhase2;
			}
			hShotPhase2 = new CMenu();
			hShotPhase2->CreatePopupMenu();
			AddMenuString(hShotPhase2,0,_l("Adding wallpaper"),0,0,0,1);
			//pThemes.AppendMenu(MF_SEPARATOR, 0, "");
			int iSuize=objSettings.m_WChanger.aUsedThemes.GetSize();
			for(int i=0;i<iSuize;i++){
				int iIndex=objSettings.m_WChanger.aUsedThemes[i];
				WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iIndex];
				if(wp){
					BOOL bBreak=(iSuize>30 && ((i+2)%20==0) && (i+2)>0);
					AddMenuString(hShotPhase2,iIndex+1,wp->sTitle,0,0,0,0,0,bBreak);
				}
			}
			AddMenuString(hShotPhase2,0xFFFFFFFF,_l("Do not add image"));
			SetMenuDefaultItem(hShotPhase2->m_hMenu,0xFFFFFFFF,FALSE);
			CRect rt,rDesktopRECT;
			pt->GetDlgItem(IDC_BT_ADD_FILE)->GetWindowRect(&rt);
			rDesktopRECT=CSmartWndSizer::GetScreenRect();
			wndShotPhase2 = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT, "Static", "MENU_WND", WS_DISABLED, rDesktopRECT.left, rDesktopRECT.top, rDesktopRECT.Width(), rDesktopRECT.Height(), 0, 0, AfxGetApp()->m_hInstance, 0);
			pt->EnableWindow(FALSE);
			pt->On0Tab();
			int iRes=::TrackPopupMenu(hShotPhase2->m_hMenu, TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, rt.right, rt.top, 0, wndShotPhase2, NULL);
			if(hShotPhase2){
				delete hShotPhase2;
				hShotPhase2=0;
			}
			pt->EnableWindow(TRUE);
			if(wndShotPhase2){
				DestroyWindow(wndShotPhase2);
				if(iRes>0 && iRes!=0xFFFFFFFF){
					iThemeToPut=iRes-1;
				}
			}
		}else{
			iThemeToPut=0;
		}
		if(iThemeToPut>=0){
			WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iThemeToPut];
			pt->bForceSelWallpaper=1;
			int iWPNum=-1;
			pt->iLastWPSetted=-1;//Чтобы обновилась картинка
			pt->AddFile(objSettings.sAddThisFileAfterOpenWPList,iThemeToPut,TRUE,0,&iWPNum);
			if(iWPNum!=-1){
				pt->SendMessage(WM_COMMAND,ID_WCHANGER_UPDATECURWALLPOS,iWPNum);
			}
			SmartWarning(_l("Theme")+": "+wp->sTitle+"\n"+_l("Image(s) added")+": 1",_l("Adding wallpaper"),&objSettings.lReportAddWPNow,DEF_WARNTIME);
		}
		objSettings.sAddThisFileAfterOpenWPList="";
	}
	return 0;
}

void CDLG_WChanger::FillListWithData()
{
	CSmartLock SL(&cWDlgSection,TRUE);
	lThemesWTotal=0;
	m_WList.DeleteAllIRows();
	int dwLastTheme=-1;
	dwFirstTheme=-1;
	BOOL bNeedUpdateParent=0;
	for(int i=0;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[i];
		if(!wp){
			continue;
		}
		wp->hRow=NULL;
		if(wp->dwType==0){
			if(dwFirstTheme==-1){
				dwFirstTheme=i;
			}
			if(dwLastTheme==-1){
				iFirstIndex=i;
			}
			dwLastTheme=i;
			bNeedUpdateParent=2;
		}
		Attach(i,bNeedUpdateParent==1?FALSE:TRUE);
		if(bNeedUpdateParent>0){
			bNeedUpdateParent--;
		}
	}
}

DWORD WINAPI FillListWithDataThread(LPVOID lpdwstatus)
{
	if(!pMainDialogWindow){
		AfxMessageBox("pMainDialogWindow==NULL!");
	}
	if(!pMainDialogWindow->dlgWChanger){
		AfxMessageBox("pMainDialogWindow->dlgWChanger==NULL!");
	}
	CDLG_WChanger* dlg=pMainDialogWindow->dlgWChanger;
	if(pMainDialogWindow && dlg){
		dlg->iLastWPSetted=-1;
		BOOL bUseCurNumber=TRUE;
		BOOL bUseAddAfterOpen=FALSE;
		if(objSettings.sAddThisFileAfterOpenWPList!=""){
			bUseAddAfterOpen=TRUE;
			bUseCurNumber=FALSE;
		}
		long dwCurWallpaperNumber=-1;
		if(!dlg->FillInitialData()){
			if(dlg->bDataLoaded!=1){
				// Первоначальная загрузка
				dlg->FillListWithData();
				if(objSettings.lCurPicture>=0 && objSettings.lCurPicture<objSettings.m_WChanger.GetLastActivePictureNum()){
					dwCurWallpaperNumber=objSettings.m_WChanger.aImagesInRotation[objSettings.lCurPicture];
				}
			}
		}
		if(!dlg->bForceSelWallpaper){
			long lNewPaper=bUseCurNumber?dwCurWallpaperNumber:dlg->iLastWPSetted;
			if(bUseAddAfterOpen){
				dlg->iLastWPSetted=-1;
			}
			if(objSettings.lDefMWTab!=0){
				dlg->SetLastWPSetted(lNewPaper,0);
			}else{
				dlg->SendMessage(WM_COMMAND,ID_WCHANGER_UPDATECURWALLPOS,lNewPaper);
			}
			dwCurWallpaperNumber=dlg->iLastWPSetted;
		}
		// 0 нужен, т.к. темы после SelectIItem не отрисовываются
		pMainDialogWindow->dlgWChanger->Invalidate();
		dlg->bDataLoaded=1;
		if(objSettings.sAddThisFileAfterOpenWPList!=""){
			pMainDialogWindow->dlgWChanger->On0Tab();
			AddRekolbassedFile(dlg);
		}
	}
	return 0;
}

void CDLG_WChanger::SetHours()
{
	DWORD dwMinutes=objSettings.lChangePeriod;
	m_Hour=Format("%i",int(dwMinutes/60));
	m_Min=Format("%i",dwMinutes%60);
	if(strlen(m_Hour)==1){
		m_Hour=(CString)"0"+m_Hour;
	}
	if(strlen(m_Min)==1){
		m_Min=(CString)"0"+m_Min;
	}
}

#define BIG_HOLE	7
#define SML_HOLE	4
void AppendNotes(CMenu& pNotes, BOOL bMain, BOOL);

void CDLG_WChanger::RefreshNotesMenu()
{
	delete mainMenu;
	mainMenu=0;
	SetMainMenu();
	DrawMenuBar();
}

void CDLG_WChanger::SetMainMenu()
{
	if(mainMenu==0){
		mainMenu=new CMenu();
		mainMenu->CreateMenu();
		{// File
			CMenu mFile;
			mFile.CreatePopupMenu();
#ifndef ART_VERSION
			AddMenuString(&mFile,ID_SYSTRAY_BACKUPLIST,_l("Backup image list"),NULL);
			AddMenuString(&mFile,ID_SYSTRAY_RESTORELIST,_l("Restore image list"),NULL);
			AddMenuString(&mFile,ID_WCHANGER_BACKUP,_l("Backup all settings"),NULL);
			AddMenuString(&mFile,ID_WCHANGER_RESTORE,_l("Restore all settings"),NULL);
#endif
			mFile.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&mFile,ID_SYSTRAY_OPTIONS,_l("Settings"),NULL);
			mFile.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&mFile,ID_SYSTRAY_EXIT,_l("Exit"),_bmp().Get(_IL(40)));
			AddMenuSubmenu(mainMenu,&mFile,_l("File"),NULL);
		}
		{// Image
			CMenu mImage;
			mImage.CreatePopupMenu();
			AddMenuString(&mImage,ID_WCHANGER_ADDFILE,_l("Add image(s)")+"\tIns");
#ifndef ART_VERSION
			/*if(!IsVista()){
				AddMenuString(&mImage,ID_WCHANGER_ADDHYPERLINK,_l("Add URL"));
			}*/
			AddMenuString(&mImage,ID_WCHANGER_ADDARCHIVE,_l("Add archive"));
#endif
			AddMenuString(&mImage,ID_ADDFULLFOLDER,_l("Add folder (all images)")+"\tShift-Ins");
#ifndef ART_VERSION
			AddMenuString(&mImage,ID_W_ADDDIRECTORY,_l("Add folder (new images)"));
			AddMenuString(&mImage,ID_WCHANGER_ADDFOLDERAC,_l("Add themes from folder"));
#endif
			AddMenuSubmenu(mainMenu,&mImage,_l("Images"),NULL);
		}
#ifndef ART_VERSION
		{// Theme
			CMenu mTheme;
			mTheme.CreatePopupMenu();
			AddMenuString(&mTheme,ID_WCHANGER_NEWFOLDER,_l("Create new theme"),_bmp().Get(_IL(5)));
			AddMenuString(&mTheme,ID_WCHANGER_PACKINTOARCHIVE,_l("Archive current"),_bmp().Get(IDB_BM_ADDARCH),0,0,lZipLock);
			mTheme.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&mTheme,ID_WCHANGER_SETSYNCHROFOLDER,_l("Set synchronization folder"));
			AddMenuString(&mTheme,ID_WCHANGER_UPDATEFROMIMAGESFOLDER,_l("Sync with folder NOW"));
			AddMenuString(&mTheme,ID_WCHANGER_REMOVEBROKEN,_l("Remove broken images"));
			AddMenuString(&mTheme,ID_WCHANGER_REMOVEDUPLICATES,_l("Remove duplicated images"));
			mTheme.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&mTheme,ID_WCHANGER_SORTBYNAME,_l("Sort theme by name"));
			AddMenuString(&mTheme,ID_WCHANGER_SORTBYTYPE,_l("Sort theme by type"));
			AddMenuString(&mTheme,ID_WCHANGER_SORTBYDIMENSIONS,_l("Sort theme by dimensions"));
			AddMenuString(&mTheme,ID_WCHANGER_SHUFFLE,_l("Shuffle theme images"));
			AddMenuSubmenu(mainMenu,&mTheme,_l("Theme"),NULL);
		}
#endif
#ifndef	USE_LITE
		{
			CMenu mNotes;
			mNotes.CreatePopupMenu();
			AppendNotes(mNotes,1,0);
			iNotesIndex=AddMenuSubmenu(mainMenu,&mNotes,_l("Notes"));//,NULL,0,iNotesIndex
		}
		{// Presents
			CMenu mPres;
			mPres.CreatePopupMenu();
			CStringArray aPres;
			GetPresetList(aPres);
			for(int i=0;i<aPres.GetSize();i++){
				AddMenuString(&mPres,WM_USER+ACTIVATE_PRESET+i,aPres[i]==""?_l("Default"):aPres[i],0,(aPres[i]==objSettings.sActivePreset)?1:0);
			}
			mPres.AppendMenu(MF_SEPARATOR, 0, "");
			if(objSettings.sActivePreset!=""){
				AddMenuString(&mPres,ID_WCHANGER_DELPRESET,_l("Delete current preset"));
			}
			AddMenuString(&mPres,ID_WCHANGER_NEWPRESET,_l("New preset"));
			mPres.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&mPres,ID_WCHANGER_PRESETHLP,_l("What is this")+"?");
			AddMenuSubmenu(mainMenu,&mPres,_l("Presets"),NULL);
		}
#endif
#ifndef ART_VERSION
		{// Utils
			CMenu pUtils;
			pUtils.CreatePopupMenu();
			AddMenuString(&pUtils,ID_SYSTRAY_OPEN2,_l("Calendar")+getActionHotKeyDsc(OPEN_CALEN),_bmp().Get(_IL(80)));
			if(AddDefsInteract()){
				AddMenuString(&pUtils,ID_WCHANGER_SETASSCR,_l("Install as screensaver"),NULL);
			}
			pUtils.AppendMenu(MF_SEPARATOR, 0, "");
			CMenu pUtils2;
			pUtils2.CreatePopupMenu();
			AddMenuString(&pUtils2,ID_WCHANGER_REMOVEALLDUPLICATES,_l("From all themes"),NULL);
			AddMenuString(&pUtils2,ID_WCHANGER_REMOVEFLDDUPL,_l("From folder on disk"),NULL);
			AddMenuSubmenu(&pUtils,&pUtils2,_l("Remove all duplications"),NULL);
			AddMenuString(&pUtils,ID_WCHANGER_IMPORTLIST,_l("Import image list"),NULL);
			if(!IsVista()){
				AddMenuString(&pUtils,ID_SYSTRAY_IMPORTCLOCK,_l("Import Flash clock"),NULL);
			}
			AddMenuSubmenu(mainMenu,&pUtils,_l("Utilities"),NULL);
		}
#endif
		{// Web
			CMenu pWeb;
			pWeb.CreatePopupMenu();
			if(AddDefsInteract()){
				AddMenuString(&pWeb,ID_SYSTRAY_EXCHANGEWALLPAPER,_l("Exchange wallpaper"),_bmp().Get(_IL(72)));
			}
			AddMenuString(&pWeb,ID_SYSTRAY_DOWNLOADWALLPAPERS,_l("Download wallpapers"),_bmp().Get(_IL(73)));
#ifndef ART_VERSION
			AddMenuString(&pWeb,ID_SYSTRAY_DOWNLOADTEMPLATES,_l("Download templates"),_bmp().Get(_IL(73)));
			AddMenuString(&pWeb,ID_SYSTRAY_DOWNLOADCLOCKS,_l("Download more clocks"),_bmp().Get(_IL(73)));
			AddMenuString(&pWeb,ID_SYSTRAY_DOWNLOADSKINS,_l("Download more skins"),_bmp().Get(_IL(73)));
#endif
			AddMenuSubmenu(mainMenu,&pWeb,_l("On the Web"),NULL);
		}
		{// Help
			CMenu mHelp;
			mHelp.CreatePopupMenu();
			if(IsHelpEnabled2()){
				AddMenuString(&mHelp,ID_SYSTRAY_HELP,_l("Help topics"),_bmp().Get(IDB_BM_HELP));
			}
			AddMenuString(&mHelp,ID_SYSTRAY_ABOUT,_l("About program"),_bmp().Get(IDB_BM_ABPROG));
			AddMenuString(&mHelp,ID_SYSTRAY_TELLAFRIEND,_l("Tell a friend"),_bmp().Get(_IL(36)));
			if(!USE_ELEFUN){
				AddMenuString(&mHelp,ID_SYSTRAY_CHECKFORUPDATES,_l("Check for update"),_bmp().Get(_IL(22)),FALSE,NULL,IsUpdateInProgress());
			}
			AddMenuString(&mHelp,ID_SYSTRAY_TUTO,_l("Tutorial"),_bmp().Get(_IL(22)));
			mHelp.AppendMenu(MF_SEPARATOR, 0, "");
#ifndef GPL
			if(objSettings.iLikStatus!=2 || isTempKey()){
				AddMenuString(&mHelp,ID_SYSTRAY_GOTOWEBSITE,_l("Order "+AppName()),_bmp().Get(_IL(15)));
				AddMenuString(&mHelp,ID_SYSTRAY_REGISTER,_l("Enter key"),_bmp().Get(IDB_BM_LICKEY));
			}
#endif
			AddMenuString(&mHelp,ID_SYSTRAY_SENDASUGGESTION,_l("Email suggestion"),_bmp().Get(_IL(44)));
			AddMenuSubmenu(mainMenu,&mHelp,_l("Help"),NULL);
		}
		SetMenu(mainMenu);
	}
	// Устанавливаем другие параметры окна
}

BOOL CDLG_WChanger::OnInitDialog() 
{
	if(pMainDialogWindow){
		pMainDialogWindow->m_STray.ShowIcon();
	}else{
		Warn("Program closing, please wait");
		return FALSE;
	}
	iLastWPSetted=-1;
	CDialog::OnInitDialog();
	ModifyStyle( WS_VISIBLE, WS_SIZEBOX);
	Sizer.InitSizer(this,objSettings.iStickPix,HKEY_CURRENT_USER,PROG_REGKEY"\\Wnd_WChanger3");
	{
		char szPreviewPos[128]={0};
		int iPx=0,iPy=0,iPr=0,iPb=0;
		GetRegSetting("", "PreviewPos", szPreviewPos, sizeof(szPreviewPos));
		if(sscanf(szPreviewPos,"{%d,%d,%d,%d}",&iPx,&iPy,&iPr,&iPb)==4){
			CRect rtPreview(iPx,iPy,iPr,iPb);
			if(rtPreview.Width()>100 && rtPreview.Height()>100){
				GetDlgItem(IDC_STAT_PREV)->MoveWindow(rtPreview,0);
				//SetButtonSize(this,IDC_STAT_PREV,sPrv);
				/*
				CRect rtLists,rtMain;
				GetDlgItem(IDC_LIST_WPAPERS)->GetWindowRect(&rtLists);
				GetWindowRect(&rtMain);
				sPrv.cy=rtLists.Height();
				sPrv.cx=rtMain.Width()-sPrv.cx-10;
				SetButtonSize(this,IDC_LIST_WPAPERS,sPrv);
				SetButtonSize(this,IDC_LIST_WIDGETS,sPrv);
				*/
			}
		}
	}
	// Начинаем задавать правила выравнивания
	#ifndef ART_VERSION
	CRect minRect(0,0,520,520);
	#else
	CRect minRect(0,0,520,420);
	#endif
	Sizer.SetMinRect(minRect);
	Sizer.SetOptions(STICKABLE|SIZEABLE);
	SetButtonSize(this,IDC_BT_ADD_FILE,CSize(32,32));
	Sizer.AddDialogElement(IDC_LIST_WPAPERS);
	Sizer.AddDialogElement(IDC_LIST_WIDGETS);
	Sizer.AddDialogElement(IDOK,heightSize|widthSize);
	Sizer.AddDialogElement(IDAPPLY_MAIN,heightSize|widthSize|hidable);
	Sizer.AddDialogElement(IDC_PREVIEW,0);
	Sizer.AddDialogElement(IDC_SETNOW,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BT_WOPT,heightSize);
	Sizer.AddDialogElement(IDC_WIDGETDROPDOWN,widthSize|heightSize,IDC_SETNOW);
	Sizer.AddDialogElement(IDC_BT_REM_IMG,widthSize|heightSize,IDC_SETNOW);
	Sizer.AddDialogElement(IDC_BT_ADD_FILE,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_BT_ADD_DIR,widthSize|heightSize,IDC_BT_ADD_FILE);
	Sizer.AddDialogElement(IDC_BT_REM,widthSize|heightSize,IDC_BT_ADD_FILE);
	Sizer.AddDialogElement(IDC_BT_SYNC,widthSize|heightSize|hidable,IDC_BT_ADD_FILE);
	Sizer.AddDialogElement(IDC_BT_NEWTH,widthSize|heightSize,IDC_BT_ADD_FILE);
	Sizer.AddDialogElement(IDC_BT_ADD_ARCH,widthSize|heightSize,IDC_BT_ADD_FILE);
	Sizer.AddDialogElement(IDC_BT_THMISC,widthSize|heightSize|hidable,IDC_BT_ADD_FILE);
	Sizer.AddDialogElement(IDC_BT_CAL,widthSize|heightSize|alwaysVisible,IDC_BT_ADD_FILE);
//	Sizer.AddDialogElement(IDC_BT_CALR,widthSize|heightSize|alwaysVisible,IDC_BT_ADD_FILE);
	Sizer.AddDialogElement(IDC_PREVIEW_CHECK,heightSize);
	Sizer.AddDialogElement(IDC_ACTIVE_THEME,heightSize);
	Sizer.AddDialogElement(IDC_HOUR,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_MIN,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_DVOETOCHIE,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_DVOETOCHIE2,widthSize|heightSize);
	Sizer.AddDialogElement(IDC_RESINFO,heightSize);
	Sizer.AddDialogElement(IDC_OPTIONS,heightSize);
	Sizer.AddDialogElement(IDC_COMBO_SEQUENCE,heightSize);
	Sizer.AddDialogElement(IDC_FORSEQ,heightSize|transparent);
	Sizer.AddDialogElement(IDC_CHECK_ACTIVE,heightSize);
	Sizer.AddDialogElement(IDC_COMBO_THEMEFITTING,heightSize);
	Sizer.AddDialogElement(IDC_SCR_WEIGHT,heightSize);
	Sizer.AddDialogElement(IDC_STAT_DMODE,heightSize|widthSize);
	Sizer.AddDialogElement(IDC_STAT_SEQ,heightSize|widthSize);
	Sizer.AddDialogElement(IDC_STAT_WEIGHT,heightSize|widthSize);
	Sizer.AddDialogElement(IDC_STAT_WEIGHT_VAL,heightSize|widthSize);
	Sizer.AddDialogElement(IDC_STAT_PREV,widthSize|heightSize|bottomResize|leftResize|transparent);
	Sizer.AddDialogElement(IDC_STAT_THEM,heightSize|transparent);
	Sizer.AddDialogElement(IDC_MAINTAB,heightSize);
	Sizer.AddDialogElement(IDC_INFO_BLOCK,heightSize);

	Sizer.AddBottomAlign(IDC_INFO_BLOCK,0,-2);
	  // Самый верх
	Sizer.AddLeftTopAlign(IDC_BT_ADD_DIR,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_BT_ADD_FILE,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_BT_ADD_ARCH,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_BT_NEWTH,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_BT_REM,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_BT_THMISC,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_BT_SYNC,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_BT_CAL,0,BIG_HOLE);
//	Sizer.AddTopAlign(IDC_BT_CALR,0,BIG_HOLE);
	Sizer.AddTopAlign(IDC_STAT_PREV,0,BIG_HOLE);

	Sizer.AddLeftAlign(IDC_BT_ADD_FILE,IDC_BT_ADD_DIR,rightPos,SML_HOLE);
	Sizer.AddLeftAlign(IDC_BT_ADD_ARCH,IDC_BT_ADD_FILE,rightPos,SML_HOLE);
	Sizer.AddLeftAlign(IDC_BT_NEWTH,IDC_BT_ADD_ARCH,rightPos,SML_HOLE);
	Sizer.AddLeftAlign(IDC_BT_REM,IDC_BT_NEWTH,rightPos,SML_HOLE);
	Sizer.AddLeftAlign(IDC_BT_THMISC,IDC_BT_REM,rightPos,SML_HOLE);
	Sizer.AddLeftAlign(IDC_BT_SYNC,IDC_BT_THMISC,rightPos,SML_HOLE);
	Sizer.AddRightAlign(IDC_BT_CAL,IDC_STAT_PREV,leftPos,-BIG_HOLE);
//	Sizer.AddRightAlign(IDC_BT_CALR,IDC_BT_CAL,leftPos,-SML_HOLE);
	//Sizer.AddLeftAlign(IDC_BT_ADVERT,IDC_BT_SYNC,rightPos,SML_HOLE);
	//Sizer.AddRightAlign(IDC_BT_ADVERT,0,-BIG_HOLE);

	// По бордюру..
	Sizer.AddLeftAlign(IDC_MAINTAB,0,BIG_HOLE);
	Sizer.AddLeftAlign(IDC_LIST_WPAPERS,0,BIG_HOLE);
	Sizer.AddLeftAlign(IDC_LIST_WIDGETS,0,BIG_HOLE);
	Sizer.AddLeftAlign(IDC_INFO_BLOCK,0,2);

#ifdef ART_VERSION
	GetDlgItem(IDC_MAINTAB)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_ADD_FILE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_ADD_DIR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_ADD_ARCH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_REM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_NEWTH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_SYNC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_CAL)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_BT_CALR)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STAT_THEM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_THEMEFITTING)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STAT_DMODE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_WIDGETDROPDOWN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BT_WOPT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STAT_WEIGHT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STAT_WEIGHT_VAL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SCR_WEIGHT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ACTIVE_THEME)->ShowWindow(SW_HIDE);
	
	Sizer.AddLeftTopAlign(IDC_LIST_WPAPERS,0,BIG_HOLE);
	Sizer.AddLeftTopAlign(IDC_LIST_WIDGETS,0,BIG_HOLE);
#else
	Sizer.AddTopAlign(IDC_MAINTAB,IDC_BT_ADD_FILE,bottomPos,BIG_HOLE);
	Sizer.AddTopAlign(IDC_LIST_WPAPERS,IDC_MAINTAB,bottomPos,0);
	Sizer.AddTopAlign(IDC_LIST_WIDGETS,IDC_MAINTAB,bottomPos,0);
#endif
	Sizer.AddRightAlign(IDC_STAT_PREV,0,-BIG_HOLE);
	Sizer.AddRightAlign(IDC_STAT_THEM,0,-BIG_HOLE);
	Sizer.AddRightAlign(IDC_FORSEQ,0,-BIG_HOLE);
	Sizer.AddRightAlign(IDOK,0,-BIG_HOLE);
	Sizer.AddRightAlign(IDC_INFO_BLOCK,0,-2);
	// Ok-Apply
	Sizer.AddBottomAlign(IDAPPLY_MAIN,IDC_INFO_BLOCK,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDOK,IDC_INFO_BLOCK,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_LIST_WPAPERS,IDC_INFO_BLOCK,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_LIST_WIDGETS,IDC_INFO_BLOCK,topPos,-SML_HOLE);

	// Связка между списком картинок и превью и все остальное к этому скелету
	Sizer.AddRightAlign(IDC_LIST_WPAPERS,IDC_STAT_PREV,leftPos,-SML_HOLE);
	Sizer.AddRightAlign(IDC_LIST_WIDGETS,IDC_STAT_PREV,leftPos,-SML_HOLE);
	Sizer.AddRightAlign(IDC_MAINTAB,IDC_STAT_PREV,leftPos,-SML_HOLE);
	Sizer.AddLeftAlign(IDC_STAT_THEM,IDC_STAT_PREV,leftPos,0);
	Sizer.AddLeftAlign(IDC_FORSEQ,IDC_STAT_PREV,leftPos,0);
	Sizer.AddRightAlign(IDAPPLY_MAIN,IDC_STAT_PREV,topCenter,-SML_HOLE/2);
	Sizer.AddLeftAlign(IDAPPLY_MAIN,IDC_STAT_PREV,leftPos,0);
	Sizer.AddTopAlign(IDC_STAT_THEM,IDC_STAT_PREV,bottomPos,BIG_HOLE);

	// Блок с периодичностью
	Sizer.AddBottomAlign(IDC_FORSEQ,IDOK,topPos,-SML_HOLE);
	Sizer.AddLeftAlign(IDC_CHECK_ACTIVE,IDC_FORSEQ,leftPos,BIG_HOLE*2);
	Sizer.AddLeftAlign(IDC_STAT_SEQ,IDC_FORSEQ,leftPos,BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_COMBO_SEQUENCE,IDC_FORSEQ,rightPos,-BIG_HOLE*2);
	Sizer.AddLeftAlign(IDC_COMBO_SEQUENCE,IDC_STAT_SEQ,rightPos,SML_HOLE);
	Sizer.AddRightAlign(IDC_DVOETOCHIE2,IDC_FORSEQ,rightPos,-BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_MIN,IDC_DVOETOCHIE2,leftPos,0);
	Sizer.AddRightAlign(IDC_DVOETOCHIE,IDC_MIN,leftPos,0);
	Sizer.AddRightAlign(IDC_HOUR,IDC_DVOETOCHIE,leftPos,0);
	Sizer.AddRightAlign(IDC_CHECK_ACTIVE,IDC_HOUR,leftPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_STAT_SEQ,IDC_FORSEQ,bottomPos,-BIG_HOLE*2);
	Sizer.AddBottomAlign(IDC_COMBO_SEQUENCE,IDC_FORSEQ,bottomPos,-BIG_HOLE*2);
	Sizer.AddBottomAlign(IDC_CHECK_ACTIVE,IDC_COMBO_SEQUENCE,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_MIN,IDC_COMBO_SEQUENCE,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_DVOETOCHIE,IDC_COMBO_SEQUENCE,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_DVOETOCHIE2,IDC_COMBO_SEQUENCE,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_HOUR,IDC_COMBO_SEQUENCE,topPos,-SML_HOLE);


	// Блок предпросмотра
	Sizer.AddTopAlign(IDC_PREVIEW_CHECK,IDC_STAT_PREV,topPos,BIG_HOLE*2+SML_HOLE);
	Sizer.AddTopAlign(IDC_BT_REM_IMG,IDC_PREVIEW_CHECK,topPos,0);
	Sizer.AddTopAlign(IDC_SETNOW,IDC_PREVIEW_CHECK,topPos,0);
	Sizer.AddTopAlign(IDC_PREVIEW,IDC_PREVIEW_CHECK,bottomPos,SML_HOLE);
	Sizer.AddBottomAlign(IDC_RESINFO,IDC_STAT_PREV,bottomPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_PREVIEW,IDC_RESINFO,topPos,-SML_HOLE);

	Sizer.AddLeftAlign(IDC_RESINFO,IDC_STAT_PREV,leftPos,BIG_HOLE*2);
	Sizer.AddLeftAlign(IDC_PREVIEW_CHECK,IDC_STAT_PREV,leftPos,BIG_HOLE*2);
	Sizer.AddLeftAlign(IDC_PREVIEW,IDC_STAT_PREV,leftPos,BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_PREVIEW,IDC_STAT_PREV,rightPos,-BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_RESINFO,IDC_STAT_PREV,rightPos,-BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_SETNOW,IDC_STAT_PREV,rightPos,-BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_BT_REM_IMG,IDC_SETNOW,leftPos,-SML_HOLE);
	Sizer.AddRightAlign(IDC_PREVIEW_CHECK,IDC_BT_REM_IMG,leftPos,-SML_HOLE);

	// Блок настроек темы
	Sizer.AddBottomAlign(IDC_COMBO_THEMEFITTING,IDC_STAT_THEM,bottomPos,-BIG_HOLE);
	Sizer.AddBottomAlign(IDC_STAT_DMODE,IDC_COMBO_THEMEFITTING,bottomPos,0);
	Sizer.AddBottomAlign(IDC_BT_WOPT,IDC_COMBO_THEMEFITTING,bottomPos,0);
	Sizer.AddBottomAlign(IDC_WIDGETDROPDOWN,IDC_COMBO_THEMEFITTING,bottomPos,0);
	
	Sizer.AddBottomAlign(IDC_SCR_WEIGHT,IDC_COMBO_THEMEFITTING,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_STAT_WEIGHT_VAL,IDC_COMBO_THEMEFITTING,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_STAT_WEIGHT,IDC_COMBO_THEMEFITTING,topPos,-SML_HOLE);
	Sizer.AddBottomAlign(IDC_ACTIVE_THEME,IDC_SCR_WEIGHT,topPos,-SML_HOLE);

	Sizer.AddRightAlign(IDC_COMBO_THEMEFITTING,IDC_STAT_THEM,rightPos,-BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_WIDGETDROPDOWN,IDC_STAT_THEM,rightPos,-BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_BT_WOPT,IDC_WIDGETDROPDOWN,leftPos,-SML_HOLE);
	Sizer.AddRightAlign(IDC_SCR_WEIGHT,IDC_STAT_THEM,rightPos,-BIG_HOLE*2);
	Sizer.AddRightAlign(IDC_ACTIVE_THEME,IDC_STAT_THEM,rightPos,-BIG_HOLE*2);
	
	Sizer.AddLeftAlign(IDC_ACTIVE_THEME,IDC_STAT_THEM,leftPos,BIG_HOLE*2);
	Sizer.AddLeftAlign(IDC_STAT_WEIGHT,IDC_STAT_THEM,leftPos,BIG_HOLE*2);
	Sizer.AddLeftAlign(IDC_STAT_DMODE,IDC_STAT_THEM,leftPos,BIG_HOLE*2);
	Sizer.AddLeftAlign(IDC_BT_WOPT,IDC_STAT_THEM,leftPos,BIG_HOLE*2);

	Sizer.AddLeftAlign(IDC_STAT_WEIGHT_VAL,IDC_STAT_WEIGHT,rightPos,SML_HOLE);
	Sizer.AddLeftAlign(IDC_SCR_WEIGHT,IDC_STAT_WEIGHT_VAL,rightPos,1);
	Sizer.AddLeftAlign(IDC_COMBO_THEMEFITTING,IDC_STAT_DMODE,rightPos,SML_HOLE);

	Sizer.AddLeftAlign(IDC_STAT_PREV,IDC_LIST_WPAPERS,leftPos|ifLess,260);
	Sizer.AddLeftAlign(IDC_STAT_PREV,IDC_INFO_BLOCK,rightPos|ifBigger,-160);
	Sizer.AddBottomAlign(IDC_STAT_PREV,IDC_LIST_WPAPERS,bottomPos|ifBigger,-210);
	Sizer.AddBottomAlign(IDC_STAT_PREV,IDC_LIST_WPAPERS,topPos|ifLess,100);
#ifdef ART_VERSION
	GetDlgItem(IDC_MAINTAB)->ShowWindow(SW_HIDE);
#endif
	lWndMode=objSettings.lDefMWTab;
	if(lWndMode){
		GetDlgItem(IDC_LIST_WPAPERS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_WIDGETS)->ShowWindow(SW_SHOW);
	}else{
		GetDlgItem(IDC_LIST_WPAPERS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_WIDGETS)->ShowWindow(SW_HIDE);
	}
	// Установка переведенных текстов на кнопки
	SetWindowText(objSettings.sTrayTooltip+" - "+_l("Wallpapers list"));
	SetIcon(::AfxGetApp()->LoadIcon(MAIN_ICON),TRUE);
	SetIcon(::AfxGetApp()->LoadIcon(MAIN_ICON),FALSE);

	GetDlgItem(IDOK)->SetWindowText(_l("OK"));
	GetDlgItem(IDAPPLY_MAIN)->SetWindowText(_l("Options"));
	GetDlgItem(IDC_PREVIEW_CHECK)->SetWindowText(_l("Preview image"));
	GetDlgItem(IDC_STAT_WEIGHT)->SetWindowText(_l("Frequency"));
	GetDlgItem(IDC_FORSEQ)->SetWindowText(_l("Change image"));
	GetDlgItem(IDC_CHECK_ACTIVE)->SetWindowText(_l("Periodically"));
	GetDlgItem(IDC_STAT_WEIGHT_VAL)->SetWindowText("");
	GetDlgItem(IDC_DVOETOCHIE)->SetWindowText(CString(" ")+_l("Hour_short","h")+". ");
	GetDlgItem(IDC_DVOETOCHIE2)->SetWindowText(CString(" ")+_l("Min_short","m")+". ");
	GetDlgItem(IDC_STAT_SEQ)->SetWindowText(_l("Sequence"));
	GetDlgItem(IDC_STAT_DMODE)->SetWindowText(_l("Scaling"));
	GetDlgItem(IDC_BT_WOPT)->SetWindowText(_l("Widget properties"));
	GetDlgItem(IDC_WIDGETDROPDOWN)->SetWindowText("...");
	GetDlgItem(IDC_STAT_PREV)->SetWindowText(_l("Preview selected image"));
	//
	((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->AddString(_l("None, Tile"));//_l("Do not resize")
	((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->AddString(_l("Exact fit"));
	((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->AddString(_l("Proportional fit"));
	((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->AddString(_l("Automatic fit"));
	((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->AddString(_l("None, Center"));
	((CComboBox*)GetDlgItem(IDC_COMBO_SEQUENCE))->AddString(_l("Random"));
	((CComboBox*)GetDlgItem(IDC_COMBO_SEQUENCE))->AddString(_l("In order"));
	//
	// Создаем главное меню...
	SetMainMenu();

	m_BtThSync.SetBitmap(IDB_BM_SYNCBIG);
	m_BtThSync.SetStyles(CBS_DDOWN|CBS_FLAT);
	//m_BtThSync.SetHooverMode(1);
	m_BtThNew.SetBitmap(IDB_BM_NEWTH);
	m_BtThNew.SetStyles(CBS_FLAT);
	//m_BtThNew.SetHooverMode(1);
	m_BtSetNow.SetBitmap(IDB_BM_MORE);
	m_BtSetNow.SetStyles(CBS_DDOWN2|CBS_NORMAL);
	m_BtAddDir.SetBitmap(IDB_BT_ADDIR);
	m_BtAddDir.SetStyles(CBS_DDOWN|CBS_FLAT);
	//m_BtAddDir.SetHooverMode(1);
	m_BtAddFile.SetBitmap(IDB_BM_ADDFILE);
	m_BtAddFile.SetStyles(CBS_DDOWN|CBS_FLAT);
	//m_BtAddFile.SetHooverMode(1);
	m_BtAddArch.SetBitmap(IDB_BM_ADDARCH);
	m_BtAddArch.SetStyles(CBS_FLAT);
	//m_BtAddArch.SetHooverMode(1);
	m_BtRemove.SetBitmap(IDB_BM_REMFT);
	m_BtRemove.SetStyles(CBS_DDOWN|CBS_FLAT);
	//m_BtRemove.SetHooverMode(1);
	m_BtRemImg.SetBitmap(IDB_BM_REM);
	m_BtRemImg.SetStyles(CBS_DDOWN2|CBS_NORMAL);
	m_BtImgMisc.SetBitmap(IDB_BM_SORTBIG);
	m_BtImgMisc.SetStyles(CBS_DDOWN|CBS_FLAT);
	// Календар
	m_BtCal.SetBitmap(IDB_BM_CAL);
	m_BtCal.SetStyles(CBS_FLAT);
//	m_BtCalr.SetBitmap(IDB_REMIND_A);
//	m_BtCalr.SetStyles(CBS_NORMAL);
	//m_BtImgMisc.SetHooverMode(1);
	// Заполняем комбобоксs
	m_CycleType=objSettings.bChangeWPaperCycleType;
	m_WeightEdit.SetRangeMin(0);
	m_WeightEdit.SetRangeMax(100,TRUE);
	m_WeightEdit.SetTicFreq(10);
	m_WeightEdit.EnableWindow(m_CycleType==0);
	m_Activate=objSettings.lChangePriodically;
	SetHours();
	m_UsePreview=objSettings.bUsePreview;
	m_WeightEdit.SetPos(0);
	GetDlgItem(IDC_STAT_WEIGHT_VAL)->SetWindowText(_l("None"));
	GetDlgItem(IDC_STAT_THEM)->SetWindowText(_l("Theme"));
	ShowWindow(SW_SHOW);
	UpdateData(FALSE);
	OnCheckActive();
	// CG: The following block was added by the ToolTips component.
	{
		sPopTip="\n";
		sPopTip+=_l("Click to pop additional menu");
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDC_LIST_WPAPERS), _l("Themes and wallpapers")+". "+_l("Right-click to popup\nadditional menu for current item"));
		m_tooltip.AddTool(GetDlgItem(IDC_LIST_WIDGETS), _l("Desktop widgets")+". "+_l("Check widgets you wish to use at desktop"));
		m_tooltip.AddTool(GetDlgItem(IDOK), _l("Apply"));
		m_tooltip.AddTool(GetDlgItem(IDC_MAINTAB), _l("Current desktop element"));
		m_tooltip.AddTool(GetDlgItem(IDC_PREVIEW), _l("Preview")+". "+_l("Right-click to popup\nExplorer`s menu for current image"));
		m_tooltip.AddTool(GetDlgItem(IDC_PREVIEW_CHECK), _l("Show preview"));
		m_tooltip.AddTool(GetDlgItem(IDC_HOUR), _l("Hours"));
		m_tooltip.AddTool(GetDlgItem(IDC_MIN), _l("Minutes"));
		m_tooltip.AddTool(GetDlgItem(IDC_RESINFO), _l("Wallpaper information"));
		m_tooltip.AddTool(GetDlgItem(IDC_COMBO_SEQUENCE), _l("Wallpapers sequence"));
		m_tooltip.AddTool(GetDlgItem(IDC_CHECK_ACTIVE), _l("Change wallpaper periodically"));
		m_tooltip.AddTool(GetDlgItem(IDC_SETNOW), _l("More actions")+sPopTip);
		m_tooltip.AddTool(GetDlgItem(IDC_BT_SYNC), _l("Theme content synchronization")+sPopTip);
		m_tooltip.AddTool(GetDlgItem(IDC_BT_NEWTH), _l("Create new theme"));
		m_tooltip.AddTool(GetDlgItem(IDC_BT_ADD_DIR), _l("Add folder")+sPopTip);
		m_tooltip.AddTool(GetDlgItem(IDC_BT_ADD_FILE), _l("Add images or URL")+sPopTip);
		m_tooltip.AddTool(GetDlgItem(IDC_BT_ADD_ARCH), _l("Add image gallery or archive"));
		m_tooltip.AddTool(GetDlgItem(IDC_BT_REM), _l("Clear/Remove theme")+sPopTip);
		m_tooltip.AddTool(GetDlgItem(IDC_SCR_WEIGHT), _l("Theme weight (percents)")+"\n"+_l(sWeightDesc));
		m_tooltip.AddTool(GetDlgItem(IDC_COMBO_THEMEFITTING), _l("Theme display type"));
		m_tooltip.AddTool(GetDlgItem(IDC_INFO_BLOCK), _l("Short information"));
		m_tooltip.AddTool(GetDlgItem(IDC_BT_THMISC), _l("Sort images")+sPopTip);
		m_tooltip.AddTool(GetDlgItem(IDC_BT_REM_IMG), _l("Remove image")+sPopTip);
		m_tooltip.AddTool(GetDlgItem(IDC_ACTIVE_THEME), _l("Use this theme in the rotation of images"));
		m_tooltip.AddTool(GetDlgItem(IDC_OPTIONS), _l(OPTIONS_TP));
		m_tooltip.AddTool(GetDlgItem(IDC_BT_CAL), _l("Calendar"));
//		m_tooltip.AddTool(GetDlgItem(IDC_BT_CALR), _l("New reminder"));
		
		m_tooltip.SetMaxTipWidth(int(GetDesktopRect().Width()*0.7));
	}
	// После всего обязательно...
	InitWList();
	RefreshTemplates();
	//SetInfoBlock(_l("Right-click theme or image to open context-specific menu"));
	::InterlockedIncrement(&objSettings.lMainWindowOpened);
	FORK(FillListWithDataThread,NULL);
	bDialogInitOk=1;
	return TRUE;  // return TRUE unless you set the focus to a control
}

// 1ый уровень забивает 0ой и держится минимум 5 секунд
void CDLG_WChanger::SetInfoBlockAlert(const char* szText)
{
	SmartWarning(szText,AppName(),&objSettings.bReportInformAlerts,DEF_WARNTIME);
	SetInfoBlock(szText,1);
}

DWORD WINAPI AskTutorial(LPVOID)
{
	if(AfxMessageBox(_l("Would you like to read short tutorial")+"?",MB_YESNO|MB_ICONQUESTION|MB_TOPMOST|MB_SETFOREGROUND)==IDYES){
		CString sRFile=Format("%s"WP_TEMPLATE"\\Tutorial.html",GetApplicationDir(),0);
		GenerateForm(sRFile,"",0);
		return 1;
	}
	return 0;
}

BOOL CDLG_WChanger::FillInitialData()
{
	if(objSettings.m_WChanger.aWPapersAndThemes.GetSize()==0){
		// Если все происходит в первый раз...
		DWORD dwMainTheme=0;
		int iDefBmpPosition=-1;
		BOOL bFirstInit=0;
		DWORD dwWallpaperThemes=0;
		CString sMThemeFolder=Format("%sWallpapers\\",GetApplicationDir());
		{
			if(isFileExist(sMThemeFolder)){
#ifdef ART_VERSION
				dwMainTheme=AddThemeFolder(_l("Masterpieces"));
				WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[dwMainTheme];
				AddFolder(wp,objSettings.bLoadDirsRecursively,dwMainTheme,FALSE,sMThemeFolder,0);
				m_WList.Expand(wp->hRow);
#else

				dwMainTheme=AddDefsWallps()?AddThemeFolder(_l("Standard images")):0;
				iFirstIndex=dwMainTheme;
				bFirstInit=1;
				AddFolder(0,objSettings.bLoadDirsRecursively,dwMainTheme,FALSE,sMThemeFolder,2);
				if(AddDefsWallps()){
					// Текущая обоина
					char szWPPath[MAX_PATH]={0};
					SystemParametersInfo(0x0073/*SPI_GETDESKWALLPAPER*/,sizeof(szWPPath),(void*)szWPPath,0);
					if(szWPPath[0]!=0 && !IsFileHTML(szWPPath) && !IsFileURL(szWPPath) && isFileExist(szWPPath)){
						strcpy(sImageToBeSetAfterClose,szWPPath);
						objSettings.iLastStretchingFromUser=1;
						objSettings.sLastWallpaperFromUser=szWPPath;
						AddFile(szWPPath,dwMainTheme,TRUE,0,&iDefBmpPosition);
						bForceSelWallpaper=1;
					}
				}
			}
#endif
		}
#ifndef ART_VERSION
		if(AddDefsWallps()){
			CString sSysFolder;
			char szSysFolder[MAX_PATH]="%WINDIR%\\Web\\Wallpaper\\";
			DWORD dwDoEnv=DoEnvironmentSubst(szSysFolder,sizeof(szSysFolder));
			if(LOWORD(dwDoEnv)){
				sSysFolder=szSysFolder;
			}
			if(sSysFolder!="" && isFileExist(sSysFolder)){
				dwWallpaperThemes=dwMainTheme;
				WPaper* wp3=objSettings.m_WChanger.aWPapersAndThemes[dwWallpaperThemes];
				if(wp3){
					AddFolder(wp3,objSettings.bLoadDirsRecursively,dwWallpaperThemes,FALSE,sSysFolder);
				}
			}
		}
#endif
		{
			if(!bFirstInit){
				bFirstInit=1;
				dwMainTheme=dwWallpaperThemes;
			}
			WPaper* wpMainTheme=objSettings.m_WChanger.aWPapersAndThemes[dwMainTheme];
			if(wpMainTheme){
				m_WList.Expand(wpMainTheme->hRow);
			}
			iFirstIndex=dwMainTheme;
		}
		if(bForceSelWallpaper){
			SendMessage(WM_COMMAND,ID_WCHANGER_UPDATECURWALLPOS,iDefBmpPosition);
			iLastWPSetted=iDefBmpPosition;
		}
#ifndef ART_VERSION
		FORK(AskTutorial,0);
#endif
		if(objSettings.m_WChanger.aWPapersAndThemes.GetSize()>4){// Устанавливаем случайную картинку!
			objSettings.m_WChanger.ReinitWPaperShuffle("",FALSE);
			int iRandomPos=(rand()+GetTickCount())%(objSettings.m_WChanger.aWPapersAndThemes.GetSize()-3);
			WPaper* wp0=objSettings.m_WChanger.aWPapersAndThemes[iRandomPos];
			if(wp0->dwType!=1){
				wp0=objSettings.m_WChanger.aWPapersAndThemes[iRandomPos+1];
			}
			if(wp0->dwType==1){
				SetWallPaper(wp0->sTitle, 1, "");
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CDLG_WChanger::SetInfoBlock(const char* szText, DWORD dwLevel)
{
	static DWORD dwLastLevel1Time=0;
	if(dwLevel==1 || (dwLevel==0 && (GetTickCount()-dwLastLevel1Time)>5000)){
		if(dwLevel==1){
			dwLastLevel1Time=GetTickCount();
		}else if(dwLastLevel1Time>0){
			dwLastLevel1Time=0;
		}
		CString sInfo=szText;
		sInfo.Replace("\n"," ");
		sInfo.Replace("\r"," ");
		while(sInfo.Replace("  "," ")){};
		GetDlgItem(IDC_INFO_BLOCK)->SetWindowText(CString(" ")+sInfo);
	}
}

void CDLG_WChanger::GetDSelectedRow()
{
	sCurrentWidget="";
	HIROW hLastSelected=m_DList.GetSelectedIRow();
	if(hLastSelected==NULL){
		return;
	}
	int iCurID=m_DList.GetIRowData(hLastSelected);
	if(iCurID==DEF_NOTESID){
		sCurrentWidget="notes";
	}else if(iCurID>=0 && iCurID<objSettings.aLoadedWPTs.GetSize()){
		sCurrentWidget=objSettings.aLoadedWPTs[iCurID];
	}
}

void CDLG_WChanger::GetSelectedRow(BOOL bTo0)
{
	if(bTo0){
		On0Tab();
	}
	HIROW hRow=m_WList.GetSelectedIRow();
	if(hRow==NULL){
		iCurrentItem=-1;
		return;
	}
	iCurrentItem=m_WList.GetIRowData(hRow);
}

BOOL CDLG_WChanger::CloseThisWindow()
{
	SimpleLocker lc(&objSettings.csWPInChange);
	DestroyWindow();
	return TRUE;
}


void CDLG_WChanger::OnExpandWList(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	hDownButtonField=0;
}

BOOL CDLG_WChanger::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	UINT iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	if(pMsg->message==WM_HELP){
		pMainDialogWindow->OnSystrayHelp();
		return TRUE;
	}
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==VK_F1){
			pMainDialogWindow->OnSystrayHelp();
			return TRUE;
		}
		if(pMsg->wParam==VK_INSERT){
			if(::GetKeyState(VK_SHIFT)<0){
				OnAddfullfolder();
				return TRUE;
			}
			if(::GetKeyState(VK_CONTROL)<0){
				OnWchangerDisableImg();
				return TRUE;
			}
		}
		if(pMsg->wParam=='O' && GetKeyState(VK_CONTROL)<0){
			OnBnClickedOptions();
			return TRUE;
		}
		if(lWndMode==0){
			//IDC_LIST_WIDGETS DEL?IDC_LIST_WPAPERS?
			if((pMsg->wParam=='W' && GetKeyState(VK_CONTROL)<0)
				|| (pMsg->wParam==VK_RETURN && !(GetKeyState(VK_CONTROL)<0))){
				OnSetnowWallp();
				return TRUE;
			}
			if(pMsg->wParam==VK_RETURN && GetKeyState(VK_CONTROL)<0){
				PostMessage(WM_COMMAND,IDOK,0);
				return TRUE;
			}
			if(pMsg->wParam=='E' && GetKeyState(VK_CONTROL)<0){
				OnBtEdit();
				return TRUE;
			}
#ifndef ART_VERSION
			if(pMsg->wParam==VK_F2){
				OnWchangerRenameImage();
				return TRUE;
			}
#endif
			if(pMsg->wParam==VK_INSERT){
				OnWchangerAddfile();
				return TRUE;
			}
			if(pMsg->wParam==VK_DELETE){
				GetSelectedRow();
				if(iCurrentItem==-1){
					return TRUE;
				}
				WPaper* wpItem=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem];
				if(wpItem->dwType){
					if(::GetKeyState(VK_SHIFT)<0){
						OnWchangerRemovewithimage();
					}else{
						OnWchangerDeletefolder();
					}
				}else{
					OnWchangerDeleteTheme();
				}
				return TRUE;
			}
		}
		if(pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE){
			return TRUE;
		}
	}
	HIROW hPreSelectedIRow=0;
	if(iCtrlID==IDC_LIST_WPAPERS && pMsg->message==WM_LBUTTONDOWN){
		hPreSelectedIRow=m_WList.GetSelectedIRow();
	}
#ifndef ART_VERSION
	if(pMsg->message==WM_RBUTTONUP && iCtrlID==IDC_PREVIEW){
		CPoint pt;
		GetCursorPos(&pt);
		if(sPreparedWallpaperFile!=""){
			ShowContextMenu(this->GetSafeHwnd(),sPreparedWallpaperFile,pt.x,pt.y);
			return TRUE;
		}
	}
#endif
	if(iCtrlID==IDC_LIST_WPAPERS){
		if(pMsg->message==WM_LBUTTONDOWN || pMsg->message==WM_LBUTTONDBLCLK){
			CPoint p;
			::GetCursorPos(&p);
			hDownButtonField=0;
			m_WList.ScreenToClient(&p);
			int iItemIndex=m_WList.HitTestEx(p);
			if(iItemIndex>=0){
				hDownButtonField=m_WList.GetIRowFromIndex(iItemIndex);
				// А может нужно отключить картинку?
				BOOL bLabelClick=m_WList.HitTestLabel(hDownButtonField,p);
				if(hPreSelectedIRow==hDownButtonField && !bLabelClick){
					int iItem=m_WList.GetIRowData(hDownButtonField);
					OnWchangerDisableImgX(iItem);
				}
				if(pMsg->message==WM_LBUTTONDBLCLK && !bLabelClick){
					// Дальше можно не идти...
					return TRUE;
				}
				if(pMsg->message==WM_LBUTTONDBLCLK && !objSettings.lDblToRename){
					OnSetnowWallp();
					// Дальше можно не идти...
					return TRUE;
				}
			}
		}
	}
	BOOL bRes=CDialog::PreTranslateMessage(pMsg);
	if(iCtrlID==IDC_LIST_WPAPERS){
		if(pMsg->message==WM_RBUTTONUP){
			if(OnRclickWlist()){
				return TRUE;
			}
		}
		if(pMsg->message==WM_LBUTTONUP){
			hDownButtonField=0;
		}
	}
	if(iCtrlID==IDC_LIST_WIDGETS){
		if(pMsg->message==WM_RBUTTONUP){
			if(OnRclickDlist(0)){
				return TRUE;
			}
		}
	}
	if(pMsg->message==WM_MOUSEMOVE){
		if(iCtrlID==IDC_LIST_WIDGETS){
			static int iLastItem=-1;
			// Выделяем картинку под курсором
			CPoint p;
			::GetCursorPos(&p);
			m_WList.ScreenToClient(&p);
			int iItem=-1;
			int iItemIndex=m_DList.HitTestEx(p);
			HIROW hNewSelRow=0;
			if(iItemIndex>=0){
				hNewSelRow=m_DList.GetIRowFromIndex(iItemIndex);
				if(hNewSelRow!=0){
					iItem=m_DList.GetIRowData(hNewSelRow);
				}
			}
			if(iItem!=-1 && iLastItem!=iItem){
				iLastItem=iItem;
				if(iItem>=0 && iItem<objSettings.aLoadedWPTs.GetSize()){
					CString sWidget=objSettings.aLoadedWPTs[iItem];
					CWPT* wpt=GetWPT(sWidget);
					if(wpt){
						CString sPopupText=_l(wpt->sTitle);
						if(wpt->sDsc!=""){
							sPopupText=_l(wpt->sDsc);
						}
						m_tooltip.Pop();
						m_tooltip.UpdateTipText(sPopupText,GetDlgItem(IDC_LIST_WIDGETS));
						SetInfoBlock(sPopupText);
					}
				}
			}
		}
		if(iCtrlID==IDC_LIST_WPAPERS){
			static int iLastItem=-1;
			// Выделяем картинку под курсором
			CPoint p;
			::GetCursorPos(&p);
			m_WList.ScreenToClient(&p);
			int iItem=-1;
			int iItemIndex=m_WList.HitTestEx(p);
			HIROW hNewSelRow=0;
			if(iItemIndex>=0){
				hNewSelRow=m_WList.GetIRowFromIndex(iItemIndex);
				if(hNewSelRow!=0){
					iItem=m_WList.GetIRowData(hNewSelRow);
				}
			}
			if(iItem!=-1 && iLastItem!=iItem){
				iLastItem=iItem;
				int dwTheme=iItem;
				WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iItem];
				WPaper* wpTheme=GetWPTheme(dwTheme);
				if(dwTheme==iItem){
					wp=NULL;
				}
				CString sPopupText="";
				if(wp){
					sPopupText+=wp->sTitle;
					if(wp->GetWPar("PRESET")!=""){
						sPopupText+="; "+_l("Preset")+Format(": '%s'",wp->GetWPar("PRESET"));
					}
				}
				if(wpTheme){
					if(sPopupText!=""){
						sPopupText+="\n";
					}
					sPopupText+=_l("Theme")+": "+wpTheme->sTitle+", "+(wpTheme->dwStatus?_l("Active"):_l("Inactive"));
					if(wpTheme->dwTheme>0){
						sPopupText+="; "+_l("Frequency")+Format(": %lu%%",wpTheme->dwTheme);
					}
					if(wpTheme->GetWPar("SYNC")!=""){
						sPopupText+="\n"+_l("Sync. folder")+Format(": '%s'",wpTheme->GetWPar("SYNC"));
					}
					if(wpTheme->dwFolderSync){
						sPopupText+="; "+_l("Sync. at start");
					}
					if(wpTheme->GetWPar("PRESET")!=""){
						sPopupText+="; "+_l("Preset")+Format(": '%s'",wpTheme->GetWPar("PRESET"));
					}
				}
				m_tooltip.Pop();
				m_tooltip.UpdateTipText(sPopupText,GetDlgItem(IDC_LIST_WPAPERS));
				SetInfoBlock(sPopupText);
				if(::GetKeyState(VK_LBUTTON)<0 && hNewSelRow!=0 && hDownButtonField!=0 && hNewSelRow!=hDownButtonField){
					m_WList.AddMultySelection(hNewSelRow,TRUE);
				}
			}
		}
	}

	if(pMsg->message==WM_DROPFILES){
		On0Tab();
		CSmartLock SL(&cWDlgSection,TRUE);
		// Выделяем картинку под курсором
		CPoint p;
		::GetCursorPos(&p);
		m_WList.ScreenToClient(&p);
		int iItemIndex=m_WList.HitTestEx(p);
		if(iItemIndex>=0){
			HIROW hNewSelRow=m_WList.GetIRowFromIndex(iItemIndex);
			m_WList.SelectIRow(hNewSelRow,TRUE);
			iCurrentItem=m_WList.GetIRowData(hNewSelRow);
		}else{
			GetSelectedRow();
		}
		// Добавляем файлы
		int dwTheme=iCurrentItem;
		WPaper* wp=GetWPTheme(dwTheme);
		if(wp){
			HDROP hDropInfo=(HDROP)pMsg->wParam;
			char szFilePath[256];
			UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
			for (UINT u = 0; u < cFiles; u++){
				DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
				if(!AddFile(szFilePath,dwTheme,(u==0)?FALSE:TRUE)){
					break;
				}
			}
			DragFinish(hDropInfo);
		}
		return TRUE;
	}
	return bRes;
}

WPaper* CDLG_WChanger::GetItemUnderCursor(BOOL bType)
{
	WPaper* wp=NULL;
	if(bType==0){
		int nCol=m_WList.GetSelectedIItem();
		if(nCol==1){
			return NULL;
		}
		GetSelectedRow();
	}
	if(iCurrentItem!=-1){
		wp=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem];
	}
	return wp;
}

BOOL CDLG_WChanger::OnLclickWlist()
{
	// неактуально. Теперь при Double-Click файлы переименовываются
	WPaper* wp=GetItemUnderCursor();
	if(!wp || wp->dwType==0){
		return FALSE;
	}
	return OpenFileForView(wp->sTitle);
}

BOOL CDLG_WChanger::OnRclickDlist(int iPos)
{
	CMenu menu;
	menu.CreatePopupMenu();
	GetDSelectedRow();
	BOOL bChectHere=1;
	int iActiveWpt=-1;
	CWPT* wpt=NULL;
	if(sCurrentWidget=="notes"){
		bChectHere=0;
		AppendNotes(menu,1,1);
	}else{
		wpt=GetWPT(sCurrentWidget);
		if(sCurrentWidget=="" || !wpt){
			return 0;
		}
		iActiveWpt=GetTemplateMainIndex(sCurrentWidget);
		AddMenuString(&menu,0,_l("Widget")+": "+TrimMessage(GetWPTTitle(sCurrentWidget)),0,0,0,0,1);
		if(wpt->aLoadedWPTsActionsDsc.GetSize()>0){
			menu.AppendMenu(MF_SEPARATOR, 0, "");
		}
		for (int j=0;j<wpt->aLoadedWPTsActionsDsc.GetSize();j++){
			CString sTemplParTitle=wpt->aLoadedWPTsActionsDsc[j];
			AddMenuString(&menu,WM_USER+CHANGE_TEMPL_PAR+MAXROOTS_PERTEMPL*iActiveWpt+j,_l(sTemplParTitle));
		}
		if(wpt->sActionPosition!=""){
			AddMenuString(&menu,3,_l("Set position"));
		}
		if(wpt->sActionTransparency!=""){
			CString sVal;
			ReadSingleTemplateVar(wpt->sWPFileName,wpt->sActionTransparency,sVal);
			DWORD dwVal=atol(sVal);
			CMenu menu2;
			menu2.CreatePopupMenu();
			AddMenuString(&menu2,100,"100%",0,abs(int(101-dwVal))<10);
			AddMenuString(&menu2,80,"80%",0,abs(int(80-dwVal))<=10);
			AddMenuString(&menu2,60,"60%",0,abs(int(60-dwVal))<10);
			AddMenuString(&menu2,40,"40%",0,abs(int(40-dwVal))<=10);
			AddMenuString(&menu2,20,"20%",0,abs(int(20-dwVal))<10);
			AddMenuSubmenu(&menu,&menu2,_l("Transparency"));
		}
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		BOOL bDeletable=1;
		BOOL bClonable=wpt->bClonable;
		AddMenuString(&menu,1,_l("Make a copy"),0,0,0,!bClonable);
		AddMenuString(&menu,2,_l("Delete"),0,0,0,!bDeletable);
	}
	::SetMenuDefaultItem(menu.m_hMenu, 0, TRUE);
	CPoint pos;
	if(iPos){
		CRect rt;
		GetDlgItem(IDC_WIDGETDROPDOWN)->GetWindowRect(&rt);
		pos.x=rt.right;
		pos.y=rt.bottom;
	}else{
		GetCursorPos(&pos);
	}
	DWORD dwRes=::TrackPopupMenu(menu.m_hMenu, (bChectHere?(TPM_RETURNCMD|TPM_NONOTIFY):0)|TPM_RIGHTBUTTON, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	if(dwRes && bChectHere){
		if(dwRes>=WM_USER){
			bNeddRefreshAfterClose=TRUE;
			pMainDialogWindow->PostMessage(WM_COMMAND,dwRes,0);
		}
		if(dwRes==1){
			bNeddRefreshAfterClose=TRUE;
			CloneTemplate(iActiveWpt);
		}
		if(dwRes==2){
			bNeddRefreshAfterClose=TRUE;
			RemoveTemplate(iActiveWpt);
		}
		if(wpt){
			if(dwRes==3){
				FORK(CallTemplatePositionDialog_InThread,wpt);
				bNeddRefreshAfterClose=TRUE;
			}
			if(dwRes==20 || dwRes==40 || dwRes==60 || dwRes==80 || dwRes==100){
				WriteSingleTemplateVar(wpt->sWPFileName,wpt->sActionTransparency,Format("%i",dwRes));
				bNeddRefreshAfterClose=TRUE;
			}
		}
	}
	return TRUE;

}

//bType==0 - все
//bType==1 - добавление
//bType==2 - удаление
BOOL CDLG_WChanger::OnRclickWlist(BOOL bType, CPoint posT)
{
	WPaper* wp=GetItemUnderCursor(bType);
	if(!wp){
		return FALSE;
	}
	int iCurrentTheme=iCurrentItem;
	WPaper* wpTheme=GetWPTheme(iCurrentTheme);
	WPaper* wpItem=NULL;
	if(wpTheme!=NULL){
		wpItem=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem];
	}
	CMenu menu;
	menu.CreatePopupMenu();
	BOOL bMulty=m_WList.IsSelectionMulty();
	BOOL bZip=wpItem?IsFileInZip(wpItem->sTitle):0;
	DWORD dwCurFileTheme=wp->dwTheme;
	if(bType<0){
		AddMenuString(&menu,0,_l("Theme")+": "+TrimMessage(wpTheme->sTitle),0,0,0,0,1);
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		if(bType==-1){
			AddMenuString(&menu,ID_WCHANGER_ADDFILE,_l("Add image(s)")+"\tIns");
#ifndef ART_VERSION
			/*if(!IsVista()){
				AddMenuString(&menu,ID_WCHANGER_ADDHYPERLINK,_l("Add URL"));
			}*/
#endif
		}
#ifndef ART_VERSION
		if(bType==-2){
			AddMenuString(&menu,ID_WCHANGER_ADDARCHIVE,_l("Add archive"));
			AddMenuString(&menu,ID_WCHANGER_ADDFILE,_l("Add gallery"));
		}
		if(bType==-3){
			AddMenuString(&menu,ID_ADDFULLFOLDER,_l("Add folder (all images)")+"\tShift-Ins");
			AddMenuString(&menu,ID_W_ADDDIRECTORY,_l("Add folder (new images)"));
			AddMenuString(&menu,ID_WCHANGER_ADDFOLDERAC,_l("Add themes from folder"));
		}
		if(bType==-4){
			if(!wpTheme){
				bType=0;
			}else{
				AddMenuString(&menu,ID_WCHANGER_DELETESINGLETHEME,_l("Delete theme"),_bmp().Get(IDB_BM_DEL),0,0,(bMulty || (aThemesCount.GetSize()<=1)));
				AddMenuString(&menu,ID_WCHANGER_DELETEALL,_l("Clear theme"),_bmp().Get(IDB_BM_CLEAN),0,0,bMulty);
			}
		}
#endif
		if(bType==-5){
			AddMenuString(&menu,ID_WCHANGER_DELETEFOLDER,_l("Remove image")+"\tDel",_bmp().Get(IDB_BM_DEL));
			AddMenuString(&menu,ID_WCHANGER_REMOVEWITHIMAGE,_l("Delete image and file")+"\tShift-Del",_bmp().Get(IDB_BM_DEL2),0,0,bZip);
			AddMenuString(&menu,ID_WCHANGER_EXCLUDEIMAGES,_l("Disable/Enable image")+"\tCtrl-Ins",_bmp().Get(_IL(77)));
		}
#ifndef ART_VERSION
		if(bType==-6){
			if(!wpTheme){
				bType=0;
			}else{
				AddMenuString(&menu,ID_WCHANGER_SORTBYNAME,_l("Sort by name"));
				AddMenuString(&menu,ID_WCHANGER_SORTBYTYPE,_l("Sort by type"));
				AddMenuString(&menu,ID_WCHANGER_SORTBYDIMENSIONS,_l("Sort by dimensions"));
				AddMenuString(&menu,ID_WCHANGER_SHUFFLE,_l("Shuffle images"));
			}
		}
		if(bType==-7){
			if(!wpTheme){
				bType=0;
			}else{
				AddMenuString(&menu,ID_WCHANGER_SETSYNCHROFOLDER,_l("Set synchronization folder"));
				if(wpTheme->GetWPar("SYNC")!=""){
					AddMenuString(&menu,0,Format("%s: %s",_l("Now"),TrimMessage(wpTheme->GetWPar("SYNC"),30,3)),0,0,0,1);
				}
				AddMenuString(&menu,ID_WCHANGER_UPDATEFROMIMAGESFOLDER,_l("Sync with folder NOW"));
				AddMenuString(&menu,ID_WCHANGER_FOLDERSYNC,_l("FolderSync at startup"),0,wpTheme->dwFolderSync?TRUE:FALSE);
				menu.AppendMenu(MF_SEPARATOR, 0, "");
				AddMenuString(&menu,ID_WCHANGER_REMOVEBROKEN,_l("Remove broken images"));
				AddMenuString(&menu,ID_WCHANGER_REMOVEDUPLICATES,_l("Remove duplicated images"));
			}
		}
#endif
	}
	if(bType>=0){
		AddMenuString(&menu,0,TrimMessage(wp->sTitle,22,3),_bmp().Get(_IL(m_WList.GetIItemImage(wpItem->hRow,0))),FALSE,0,0,TRUE);
		menu.AppendMenu(MF_SEPARATOR, 0, "");
		if(bType!=2){
			CMenu* pNewMenu=&menu;
			CMenu AddMenu;
			AddMenu.CreatePopupMenu();
			if(bType==0){
				pNewMenu=&AddMenu;
			}
			if(wpItem->dwType!=0){			
				AddMenuString(&menu,ID_WCHANGER_SETWPNOW,_l("Set as wallpaper")+"\tCtrl-W",_bmp().Get(IDB_BM_SETNOW));
			}
			AddMenuString(pNewMenu,ID_WCHANGER_ADDFILE,_l("Add image(s)")+"\tIns");
			AddMenuString(pNewMenu,ID_ADDFULLFOLDER,_l("Add folder (all images)")+"\tShift-Ins");
#ifndef ART_VERSION
			AddMenuString(pNewMenu,ID_W_ADDDIRECTORY,_l("Add folder (new images)"));
			AddMenuString(pNewMenu,ID_WCHANGER_ADDFOLDERAC,_l("Add themes from folder"));
			AddMenuString(pNewMenu,ID_WCHANGER_ADDARCHIVE,_l("Add archive"));
			/*if(!IsVista()){
				AddMenuString(pNewMenu,ID_WCHANGER_ADDHYPERLINK,_l("Add URL"));
			}*/
			CMenu sortMenu;
			sortMenu.CreatePopupMenu();
			AddMenuString(&sortMenu,ID_WCHANGER_SORTBYNAME,_l("Sort by name"));
			AddMenuString(&sortMenu,ID_WCHANGER_SORTBYTYPE,_l("Sort by type"));
			AddMenuString(&sortMenu,ID_WCHANGER_SORTBYDIMENSIONS,_l("Sort by dimensions"));
			AddMenuString(&sortMenu,ID_WCHANGER_SHUFFLE,_l("Shuffle images"));
#endif
			if(bType==0){
				AddMenuSubmenu(&menu,pNewMenu,_l("Add")+"\t...",_bmp().Get(IDB_BM_NEWI));
			}
#ifndef ART_VERSION
			AddMenuSubmenu(&menu,&sortMenu,_l("Sort images")+"\t...",_bmp().Get(IDB_BM_SORT));
			if(wpItem->dwType!=0){
				if(aThemesCount.GetSize()>1){
					CMenu MoveMenu;
					MoveMenu.CreatePopupMenu();
					for(int i=0;i<aThemesCount.GetSize();i++){
						AddMenuString(&MoveMenu,WM_USER+MOVE_TO_THEME+i,objSettings.m_WChanger.aWPapersAndThemes[aThemesCount[i]]->sTitle);
					}
					AddMenuSubmenu(&menu,&MoveMenu,(bMulty?_l("Move selection to"):_l("Move to"))+"\t...",_bmp().Get(_IL(79)));
				}
			}
			/*// Падает периодически???
			// Не работает. надо чтобы приассайненные темы были активны только в своем пресете!
			if(wpItem->dwType==0){// Даже для тем можно назначать пресеты
				CStringArray aPres;
				GetPresetList(aPres);
				if(aPres.GetSize()>1){
					CMenu MoveMenu;
					MoveMenu.CreatePopupMenu();
					for(int i=0;i<aPres.GetSize();i++){
						AddMenuString(&MoveMenu,WM_USER+PRESET_ON_WP+i,aPres[i]==""?_l("None"):aPres[i],0,!bMulty && wp->GetWPar("PRESET")==aPres[i]);
					}
					AddMenuSubmenu(&menu,&MoveMenu,_l("Assign preset")+"\t...",_bmp().Get(_IL(61)));
				}
			}*/
#endif
#ifndef ART_VERSION
			if(wpItem->dwType!=0){
				AddMenuString(&menu,ID_WCHANGER_RENAMEIMAGE,_l("Rename image"),_bmp().Get(_IL(56)),0,0,bMulty||bZip);
			}
			AddMenuString(&menu,ID_WCHANGER_PACKINTOARCHIVE,_l("Pack into archive"),_bmp().Get(IDB_BM_ADDARCH),0,0,lZipLock);
#endif
		}
		if(bType!=1){
			if(wp->dwType!=0){
				if(bType==0){
					menu.AppendMenu(MF_SEPARATOR, 0, "");
				}
				AddMenuString(&menu,ID_WCHANGER_DELETEFOLDER,_l("Remove image")+"\tDel",_bmp().Get(IDB_BM_DEL));
				AddMenuString(&menu,ID_WCHANGER_REMOVEWITHIMAGE,_l("Delete image and file")+"\tShift-Del",_bmp().Get(IDB_BM_DEL2),0,0,bZip);
				AddMenuString(&menu,ID_WCHANGER_EXCLUDEIMAGES,_l("Disable/Enable image")+"\tIns",_bmp().Get(_IL(77)));
			}
		}
#ifndef ART_VERSION
		if(bType!=2){ 
			if(wpTheme){
				menu.AppendMenu(MF_SEPARATOR, 0, "");
				CMenu SyncMenu;
				SyncMenu.CreatePopupMenu();
				AddMenuString(&SyncMenu,ID_WCHANGER_SETSYNCHROFOLDER,_l("Set synchronization folder"));
				if(wpTheme->GetWPar("SYNC")!=""){
					AddMenuString(&SyncMenu,0,Format("%s: %s",_l("Now"),TrimMessage(wpTheme->GetWPar("SYNC"),20,3)),0,0,0,1);
				}
				AddMenuString(&SyncMenu,ID_WCHANGER_UPDATEFROMIMAGESFOLDER,_l("Sync with folder NOW"));
				AddMenuString(&SyncMenu,ID_WCHANGER_FOLDERSYNC,_l("FolderSync at startup"),0,wpTheme->dwFolderSync?TRUE:FALSE);
				SyncMenu.AppendMenu(MF_SEPARATOR, 0, "");
				AddMenuString(&SyncMenu,ID_WCHANGER_REMOVEBROKEN,_l("Remove broken images"));
				AddMenuString(&SyncMenu,ID_WCHANGER_REMOVEDUPLICATES,_l("Remove duplicated images"));
				AddMenuSubmenu(&menu,&SyncMenu,_l("Theme synchronization")+"\t...",_bmp().Get(IDB_BM_SYNC));
				if(wp->dwType==0){
					AddMenuString(&menu,ID_FOLDER_RENAMEFOLDER,_l("Rename theme"),_bmp().Get(_IL(56)),0,0,bMulty);
				}
				AddMenuString(&menu,ID_WCHANGER_NEWFOLDER,_l("Create new theme"),_bmp().Get(_IL(5)));
			}
		}
		if(bType!=1){
			//menu.AppendMenu(MF_SEPARATOR, 0, "");
			if(wp->dwType==0){
				AddMenuString(&menu,ID_WCHANGER_DELETESINGLETHEME,_l("Delete theme")+"\tDel",_bmp().Get(IDB_BM_DEL),0,0,(bMulty || (aThemesCount.GetSize()<=1)));
			}
			AddMenuString(&menu,ID_WCHANGER_DELETEALL,_l("Clear theme"),_bmp().Get(IDB_BM_CLEAN),0,0,bMulty);
		}
#endif
	}
	::SetMenuDefaultItem(menu.m_hMenu, 0, TRUE);
	CPoint pos(posT);
	if(bType==0){
		GetCursorPos(&pos);
	}
	On0Tab();
	::TrackPopupMenu(menu.m_hMenu, 0, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	return TRUE;
}

void CDLG_WChanger::OnWchangerAddfile() 
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	CSmartLock SL(&cWDlgSection,TRUE);
	GetSelectedRow();
	int dwTheme=iCurrentItem;
	WPaper* wp=GetWPTheme(dwTheme);
	if(!wp){
		return;
	}
	// С предпросмотром
	CPreviewImage PreviewWnd;
	CPreviewFileDialog dlg(&PreviewWnd, TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, "Bitmaps (*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx;*.tif;*.tga;*.tiff;*.ico)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx;*.tif;*.tga;*.tiff;*.ico|Vector graphics (*.wmf)|*.wmf|Html pages (*.htm;*.html)|*.htm;*.html|All Files (*.*)|*.*||", this);
	// Без предпросмотра...
	//CFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, "Bitmaps (*.bmp;*.gif;*.jpg;*.ico)|*.bmp;*.gif;*.jpg;*.ico|Vector graphics (*.wmf)|*.wmf|Html pages (*.htm;*.html)|*.htm;*.html|All Files (*.*)|*.*||", NULL);
	char szFileNameBuffer[32*1024]="";
	dlg.m_ofn.lpstrFile=szFileNameBuffer;
	dlg.m_ofn.lpstrInitialDir=objSettings.sPictInitDir;
	dlg.m_ofn.nMaxFile=sizeof(szFileNameBuffer);
	SimpleTracker Track(objSettings.bDoModalInProcess);
	int iCounter=0;
	if(dlg.DoModal()!=IDOK){
		return;
	}else{
		POSITION p=dlg.GetStartPosition();
		while(p){
			objSettings.sPictInitDir=dlg.GetNextPathName(p);
			if(objSettings.sPictInitDir!=""){
				if(!AddFile(objSettings.sPictInitDir,dwTheme,iCounter==0?FALSE:TRUE)){
					break;
				}
				iCounter++;
			}
		}
		HIROW hThemeIRow=objSettings.m_WChanger.aWPapersAndThemes[dwTheme]->hRow;
		m_WList.UpdateIRow(hThemeIRow);
		m_WList.Expand(hThemeIRow);
		if(iCounter>0){
			//Alert(_l("Image(s) added")+Format(": %i",iCounter),_l("Theme")+": "+wp->sTitle,this);
			SetInfoBlockAlert(_l("Theme")+": "+wp->sTitle+"\n"+_l("Image(s) added")+Format(": %i",iCounter));
		}else{
			//Alert(_l("No images added!"),_l("Theme")+": "+wp->sTitle,this);
			SetInfoBlockAlert(_l("Theme")+": "+wp->sTitle+"\n"+_l("No images added!"));
		}
	}
}

void CDLG_WChanger::OnAddfullfolderNew()
{
	BOOL bRecurse=objSettings.bLoadDirsRecursively;
	AddFolder(TRUE,bRecurse);
}

void CDLG_WChanger::OnAddfullfolder()
{
	BOOL bRecurse=objSettings.bLoadDirsRecursively;
	AddFolder(FALSE,bRecurse);
}

void CDLG_WChanger::OnAddfullfolderAC()
{
	AddFolder(FALSE,objSettings.bLoadDirsRecursively,0,TRUE);
}

int CDLG_WChanger::AddFolder(WPaper* wp, BOOL bRecurse, int dwTheme, BOOL bNewOnly, CString& szFolderFrom,BOOL bAutoCreateThemes)
{
	HIROW hThemeIRow=0;
	if(wp){
		hThemeIRow=wp->hRow;
	}
	SimpleTracker Track(objSettings.bDoModalInProcess);
	int iCounter=0;
	char szDir[MAX_PATH]="";
	if(szFolderFrom!=""){
		strcpy(szDir,szFolderFrom);
	}else{
		strcpy(szDir,objSettings.sPictInitDir);
		if(!GetFolder(_l("Choose directory"), szDir, NULL, this->GetSafeHwnd())){
			return 0;
		}
		if(bRecurse==2){
			if(AfxMessageBox(_l("Include subfolders")+"?",MB_YESNO|MB_ICONQUESTION)!=IDYES){
				bRecurse=0;
			}
		}
		objSettings.sPictInitDir=szDir;
	}
	if(szDir[strlen(szDir)-1]!='\\'){
		strcat(szDir,"\\");
	}
	CString sCurFDir="";
	CFileInfoArray dir;
	dir.AddDir(szDir,VALID_EXTEN,bRecurse,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	for (int i=0;i<dir.GetSize();i++){
		CString sFile=dir[i].GetFilePath();
		if(sFile!=""){
			if(bNewOnly && objSettings.m_WChanger.IsSuchWPExist(sFile,dwTheme)){
				// Такая обоина уже есть - не добавляем
				continue;
			}
			if(bAutoCreateThemes){
				CString sFileDir=dir[i].GetFileDir();
				sFileDir.TrimRight("/\\");
				sCurFDir.TrimRight("/\\");
				if(sFileDir!=sCurFDir){
					CString sNmae=sFileDir;
					int iBPos=sFileDir.ReverseFind('\\');
					if(iBPos!=-1){
						sNmae=sFileDir.Mid(iBPos+1);
					}
					if(bAutoCreateThemes==2){
						sNmae=_l(sNmae);
					}
					dwTheme=AddThemeFolder(sNmae,1);
				}
				sCurFDir=sFileDir;
			}
			if(!AddFile(sFile,dwTheme,iCounter>0?TRUE:FALSE)){
				break;
			}
			iCounter++;
		}
	}
	if(hThemeIRow){
		m_WList.UpdateIRow(hThemeIRow);
	}
	szFolderFrom=szDir;
	return iCounter;
}

int CDLG_WChanger::OnWchangerMoveToFolder(DWORD dwItem,DWORD dwToTheme)
{
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[dwItem];
	WPaper* wp2=objSettings.m_WChanger.aWPapersAndThemes[dwToTheme];
	if(!wp || !wp2 || wp->dwType!=1 || wp2->dwType!=0){
		return 0;
	}
	m_WList.DeleteIRow(wp->hRow);
	objSettings.m_WChanger.aWPapersAndThemes[dwItem]=0;
	wp->hRow=NULL;
	wp->dwTheme=dwToTheme;
	int ind=objSettings.m_WChanger.aWPapersAndThemes.Add(wp);
	Attach(ind,FALSE,FALSE);
	return 1;
}

void CDLG_WChanger::AddFolder(BOOL bNewOnly, BOOL bRecurse, const char* szFolder, BOOL bAutoCreateThemes)
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	CSmartLock SL(&cWDlgSection,TRUE);
	GetSelectedRow();
	int dwTheme=iCurrentItem;
	WPaper* wp=GetWPTheme(dwTheme);
	if(!wp){
		return;
	}
	int iDelCount=0;
	CString szFolderFrom=szFolder;
	int iCounter=AddFolder(wp,objSettings.bLoadDirsRecursively,dwTheme,bNewOnly, szFolderFrom,bAutoCreateThemes);
	if(objSettings.lRemBrokenOnSync){
		iDelCount=OnWchangerDeleteallFromFolder(TRUE,dwTheme);
	}
	UpdateThemeControls(dwTheme);
	m_WList.Expand(wp->hRow);
	CString sWarText;
	if(iCounter>0){
		sWarText=(_l("Theme")+": "+wp->sTitle+", "+_l("Folder")+": '"+TrimMessage(szFolderFrom,30,3)+"'\n"+_l("Image(s) added")+Format(": %i",iCounter));
	}else if(iCounter==0){
		sWarText=(_l("Theme")+": "+wp->sTitle+", "+_l("Folder")+": '"+TrimMessage(szFolderFrom,30,3)+"'\n"+_l("No new images added!"));
	}
	if(iDelCount>0){
		sWarText+=Format("; %s: %i",_l("Broken images removed"),iDelCount);
	}
	SetInfoBlockAlert(sWarText);
}

int CDLG_WChanger::OnWchangerDeleteallFromFolder(BOOL bInvalidOnly,WPaper** wpThemeOut)
{
	GetSelectedRow();
	int dwTheme=iCurrentItem;
	return OnWchangerDeleteallFromFolder(bInvalidOnly,dwTheme,wpThemeOut);
}

BOOL IsWallpaperExist(const char* szFile)
{
	return (IsFileHTML(szFile) || isFileExist(szFile));
}


int CDLG_WChanger::OnWchangerDeleteallFromFolder(BOOL bInvalidOnly,int& dwTheme,WPaper** wpThemeOut)
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return 0;
	}
	BOOL bRemoded=1;
	if(objSettings.iLikStatus<2 || isTempKey()){
		bRemoded=0;
	}
	CSmartLock SL(&cWDlgSection,TRUE);
	if(bInvalidOnly<0){
		if(!objSettings.hUniquer){
			Alert(_l("Duplication remover is missed"));
			return 0;
		}
		if(objSettings.iLikStatus<2 && !isTempKey()){
			UnregAlert(OPTION_UNREG);
			return 0;
		}
	}
	WPaper* wp=0;
	if(dwTheme>=0){
		wp=GetWPTheme(dwTheme);
		if(!wp){
			return 0;
		}
		if(wpThemeOut){
			*wpThemeOut=wp;
		}
		if(!bInvalidOnly && wp->dwImagesCount>=500 && dwTheme>=0){
			// Такую огромную тему сначала скрываем
			m_WList.Collapse(wp->hRow);
		}
	}
	int iCount=0;
	CString sList;
	for(int i=0;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
		if(!objSettings.m_WChanger.aWPapersAndThemes[i]){
			continue;
		}
		if(objSettings.m_WChanger.aWPapersAndThemes[i]->dwType==1 && (int(objSettings.m_WChanger.aWPapersAndThemes[i]->dwTheme)==dwTheme || dwTheme<0)){
			CString sPath=objSettings.m_WChanger.aWPapersAndThemes[i]->sTitle;
			if(bInvalidOnly<0){
				if(IsWallpaperExist(sPath)){
					sList+=sPath+"\r\n";
				}
			}else{
				if(bInvalidOnly && IsWallpaperExist(sPath)){
					continue;
				}
				if(bRemoded){
					m_WList.DeleteIRow(objSettings.m_WChanger.aWPapersAndThemes[i]->hRow);
					delete objSettings.m_WChanger.aWPapersAndThemes[i];
					objSettings.m_WChanger.aWPapersAndThemes[i]=NULL;
				}
			}
			iCount++;
		}
	}
	if(bInvalidOnly<0 && sList!=""){
		CRYPT_START
		SmartWarning(_l(DEF_FDUP),AppName(),&objSettings.lShowDupHelp,DEF_WARNTIME*3);
		_GetApplicationLang fp0=(_GetApplicationLang)GetProcAddress(objSettings.hUniquer,"GetApplicationLang");
		if(fp0){
			(*fp0)()=GetApplicationLang();
		}
		if(bRemoded==0){
			_SpecOptions fp=(_SpecOptions)GetProcAddress(objSettings.hUniquer,"SpecOptions");
			if(fp){
				(*fp)(0);
			}
		}
		_FindDupes fp=(_FindDupes)GetProcAddress(objSettings.hUniquer,"FindDupes");
		if(fp){
			iCount=(*fp)(sList,GetSafeHwnd());
			if(bRemoded==0 && iCount>0){
				UnregAlert(Format("%s: %i\n%s!\n%s",_l("Similar images found"),iCount,_l("Order "+AppName()+" to remove them\nfrom your hard drive completely"),_l("Demo version is able to detect them only")));
				iCount=-1;
			}
		}
		CRYPT_END
	}else if(iCount>0){
		if(bRemoded==0 && iCount>0){
			UnregAlert(Format("%s: %i\n%s!\n%s",_l("Broken images found"),iCount,_l("Order "+AppName()+" to remove broken\nlinks from the list completely"),_l("Demo version is able to detect them only")));
			iCount=-1;
		}
		RefreshThemeTitle(dwTheme);
	}
	return iCount;
}

void CDLG_WChanger::OnWchangerDeleteall()
{
	WPaper* wpTheme=NULL;
	int iCount=OnWchangerDeleteallFromFolder(FALSE,&wpTheme);
	if(iCount>0){
		SetInfoBlockAlert(_l("Theme")+": "+wpTheme->sTitle+";\n"+_l("Image(s) removed")+Format(": %i",iCount));
	}
}

int GetWPIcon(WPaper* wp)
{
	int iIcon=-1;
	if(wp->dwStatus){
		iIcon=78;
	}else{
		CString sLowTitle=wp->sTitle;
		sLowTitle.MakeLower();
		if(sLowTitle.Find(".gif")!=-1){
			iIcon=18;
		}else if(sLowTitle.Find(".jpg")!=-1){
			iIcon=19;
		}else if(sLowTitle.Find(".bmp")!=-1){
			iIcon=20;
		}else if(IsFileHTML(sLowTitle)){
			iIcon=22;
		}else{
			iIcon=21;
		}
	}
	return iIcon;
}

BOOL CDLG_WChanger::OnWchangerDisableImgX(int iNumber)
{
	if(!objSettings.m_WChanger.aWPapersAndThemes[iNumber]){
		return FALSE;
	}
	if(objSettings.m_WChanger.aWPapersAndThemes[iNumber]->dwType!=1){
		return FALSE;
	}
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iNumber];
	if(wp){
		wp->SetStatus(1-(wp->dwStatus));
		m_WList.SetIItemImage(wp->hRow,0,GetWPIcon(wp));
		return TRUE;
	}
	return FALSE;
}

void CDLG_WChanger::OnWchangerDisableImg()
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	BOOL bMulty=m_WList.IsSelectionMulty();
	if(!bMulty){
		OnWchangerDisableImgX(iCurrentItem);
	}else{
		CHIROWArray& aSelected=m_WList.GetMultySelection();
		for(int i=0;i<aSelected.GetSize();i++){
			OnWchangerDisableImgX(m_WList.GetIRowData(aSelected[i]));
		}
		m_WList.DropMultySelection();
	}
}

void CDLG_WChanger::OnWchangerDeleteThemeX(DWORD dwImageID)
{
	if(!objSettings.m_WChanger.aWPapersAndThemes[dwImageID]){
		return;
	}
	if(objSettings.m_WChanger.aWPapersAndThemes[dwImageID]->dwType==-1){
		return;
	}
	if(aThemesCount.GetSize()<=1){
		return;
	}
	if(objSettings.m_WChanger.aWPapersAndThemes[dwImageID]->dwType!=0){
		dwImageID=objSettings.m_WChanger.aWPapersAndThemes[dwImageID]->dwTheme;
	}
	// Удаляем тему из списка...
	for(int i=0;i<aThemesCount.GetSize();i++){
		if(aThemesCount[i]==dwImageID){
			WPaper* wpTheme=objSettings.m_WChanger.aWPapersAndThemes[dwImageID];
			if(wpTheme->dwStatus){
				lThemesWTotal-=wpTheme->dwTheme;
			}
			aThemesCount.RemoveAt(i);
			break;
		}
	}
	OnWchangerDeleteall();
	m_WList.DeleteIRow(objSettings.m_WChanger.aWPapersAndThemes[dwImageID]->hRow);
	delete objSettings.m_WChanger.aWPapersAndThemes[dwImageID];
	objSettings.m_WChanger.aWPapersAndThemes[dwImageID]=NULL;
}

void CDLG_WChanger::OnWchangerDeletefolderX(DWORD dwImageID)
{
	if(!objSettings.m_WChanger.aWPapersAndThemes[dwImageID]){
		return;
	}
	if(objSettings.m_WChanger.aWPapersAndThemes[dwImageID]->dwType==-1){
		return;
	}
	if(objSettings.m_WChanger.aWPapersAndThemes[dwImageID]->dwType!=0){
		// Просто картинка...
		RefreshThemeTitle(dwImageID);
		m_WList.DeleteIRow(objSettings.m_WChanger.aWPapersAndThemes[dwImageID]->hRow);
		delete objSettings.m_WChanger.aWPapersAndThemes[dwImageID];
		objSettings.m_WChanger.aWPapersAndThemes[dwImageID]=NULL;
	}
}

void CDLG_WChanger::OnWchangerDeleteTheme()
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	if(aThemesCount.GetSize()<=1){
		return;
	}
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	DWORD dwReply=AskYesNo(_l("Do you really want to delete this theme(s) with all images")+"?",_l("Confirmation"),&objSettings.lDelThemeWarn,this);
	if(dwReply!=IDYES){
		objSettings.lDelThemeWarn=0;
		return;
	}
	BOOL bMulty=m_WList.IsSelectionMulty();
	if(!bMulty){
		OnWchangerDeleteThemeX(iCurrentItem);
	}else{
		int iCount=0;
		CHIROWArray& aSelected=m_WList.GetMultySelection();
		for(int i=0;i<aSelected.GetSize();i++){
			if(m_WList.CheckIRow(aSelected[i])){
				OnWchangerDeleteThemeX(m_WList.GetIRowData(aSelected[i]));
			}
			iCount++;
		}
		if(iCount>1){
			SetInfoBlockAlert(_l("Theme(s) removed")+Format(": %i",iCount));
		}
		m_WList.DropMultySelection();
	}
}

void CDLG_WChanger::OnWchangerDeletefolder()
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	BOOL bMulty=m_WList.IsSelectionMulty();
	if(!bMulty){
		OnWchangerDeletefolderX(iCurrentItem);
	}else{
		int iCount=0;
		CHIROWArray& aSelected=m_WList.GetMultySelection();
		for(int i=0;i<aSelected.GetSize();i++){
			if(m_WList.CheckIRow(aSelected[i])){
				OnWchangerDeletefolderX(m_WList.GetIRowData(aSelected[i]));
			}
			iCount++;
		}
		if(iCount>1){
			SetInfoBlockAlert(_l("Image(s) removed")+Format(": %i",iCount));
		}
		m_WList.DropMultySelection();
	}
}

WPaper* CDLG_WChanger::Attach(int ind, BOOL bSkipParentUpdate, BOOL bNeedPrevUpdate)
{
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[ind];
	if(wp==0){
		return NULL;
	}
	if(wp->dwType==0){
		CString sThemeTitle=wp->sTitle;
		if(wp->dwImagesCount>0){
			sThemeTitle+=Format(" (%s: %lu)",_l("Images total"),wp->dwImagesCount);
		}
		if(wp->hRow==NULL){
			aThemesCount.Add(ind);
			if(wp->dwStatus){
				lThemesWTotal+=wp->dwTheme;
			}
			wp->hRow=m_WList.InsertIRow(FL_ROOT, FL_LAST, sThemeTitle, 5);
		}else{
			m_WList.SetIItemText(wp->hRow,0,sThemeTitle);
		}
		m_WList.SetIRowData(wp->hRow,ind);
		if(wp->dwImagesCount>0){
			m_WList.SetIItemControlType(wp->hRow,0,FLC_CHECKBOX);
		}else{
			m_WList.SetIItemControlType(wp->hRow,0,FLC_CHECKBOX|FLC_EDIT);
		}
		if((BOOL)wp->dwStatus!=m_WList.IsIItemChecked(wp->hRow,0)){
			m_WList.SetIItemCheck(wp->hRow,0,wp->dwStatus);
			wp->SetStatus(wp->dwStatus);
		}
	}else{
		HIROW hParent=objSettings.m_WChanger.aWPapersAndThemes[wp->dwTheme]->hRow;
		if(hParent){
			int iIcon=GetWPIcon(wp);
			CString sImagePath=wp->sTitle;
			if(wp->hRow==NULL){
				wp->hRow=m_WList.InsertIRow(hParent, FL_LAST, sImagePath, iIcon);
			}else{
				m_WList.SetIItemText(wp->hRow,0,sImagePath);
				m_WList.SetIItemImage(wp->hRow,0,iIcon);
			}
			m_WList.SetIItemControlType(wp->hRow,0,FLC_EDIT);
			m_WList.SetIRowTextStyle(wp->hRow,FL_NORMAL);
			m_WList.SetIRowData(wp->hRow,ind);
			if(!bSkipParentUpdate){
				m_WList.UpdateIRow(hParent);
			}
		}
	}
	if(wp && bNeedPrevUpdate){
		HIROW hPrev=m_WList.GetPrevIRow(wp->hRow);
		if(hPrev){
			m_WList.UpdateIRow(hPrev);
		}
	}
	return wp;
}

int CDLG_WChanger::AddThemeFolder(const char* szTitle, DWORD dwStatus)
{
	On0Tab();
	CSmartLock SL(&cWDlgSection,TRUE);
	WPaper* wp=new WPaper;
	wp->SetStatus(dwStatus);
	wp->sTitle=szTitle;
	wp->dwType=0;
	wp->dwSampling=3;
	wp->dwTheme=0;// Вес по умолчанию
	int iRes=objSettings.m_WChanger.aWPapersAndThemes.Add(wp);
	Attach(iRes);
	return iRes;
}

void CDLG_WChanger::RefreshThemeTitle(int dwThemeIndex)
{
	SimpleLocker lc(&objSettings.cAddWP);
	int iThemeInd=dwThemeIndex;
	if(iThemeInd<0){
		for(int i=0;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
			if(!objSettings.m_WChanger.aWPapersAndThemes[i]){
				continue;
			}
			if(objSettings.m_WChanger.aWPapersAndThemes[i]->dwType==0){
				WPaper* wpTh=GetWPTheme(i);
				if(wpTh && wpTh->dwImagesCount>0){
					wpTh->dwImagesCount=0;
					Attach(i,TRUE);
				}
			}
		}
	}else{
		WPaper* wpTh=GetWPTheme(iThemeInd);
		if(wpTh && wpTh->dwImagesCount>0){
			wpTh->dwImagesCount=0;
			Attach(iThemeInd,TRUE);
		}
	}
}

BOOL CDLG_WChanger::AddFile(const char* szPath, DWORD dwTheme, BOOL bSkipParentUpdate, WPaper** wpOut, int* iIndexOut)
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return 0;
	}
	SimpleLocker lc(&objSettings.cAddWP);
	/*if(objSettings.iLikStatus!=2 && objSettings.m_WChanger.aWPapersAndThemes.GetSize()>=500){
		UnregAlert(_l("Unregistered version of "PROGNAME" can not hold more than 500 images\nTo add more images, please register "PROGNAME"!"));
		return 0;
	}*/
	if(szPath==NULL || strlen(szPath)==0){
		return 1;
	}
	RefreshThemeTitle(dwTheme);
	WPaper* wp=new WPaper;
	wp->dwType=1;
	wp->sTitle=szPath;
	wp->dwTheme=dwTheme;
	int ind=objSettings.m_WChanger.aWPapersAndThemes.Add(wp);
	Attach(ind,bSkipParentUpdate,bSkipParentUpdate);
	if(wpOut){
		*wpOut=wp;
	}
	if(iIndexOut){
		*iIndexOut=ind;
	}
	return 1;
}

void CDLG_WChanger::OnWchangerNewfolder() 
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	On0Tab();
	int iPos=AddThemeFolder(Format("%s #%lu",_l("Theme"),aThemesCount.GetSize()+1));
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iPos];
	if(wp){
		m_WList.SelectIRow(wp->hRow);
		m_WList.EnsureVisible(wp->hRow,0,TRUE);
	}
}

int CDLG_WChanger::GetFirstThemeImage(int iThemeIndex, int iStart)
{
	for(int i=0;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[i];
		if(wp){
			if(wp->dwType==1 && wp->dwTheme==(DWORD)iThemeIndex){
				return i;
			}
		}
	}
	return -1;
}

void CDLG_WChanger::GetNextThemeImage(int iThemeIndex, int& iPos)
{
	int iStart=iPos+1;
	//if(!bNoNewThemesWas){
	//	iStart=iPos+1;
	//}
	for(int i=iStart;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[i];
		if(wp){
			if(wp->dwType==1 && wp->dwTheme==(DWORD)iThemeIndex){
				iPos=i;
				return;
			}
		}
	}
	iPos=-1;
	return;
}

void CDLG_WChanger::SwapThemeImage(int iPos1,int iPos2)
{
	WPaper* wp1=objSettings.m_WChanger.aWPapersAndThemes[iPos1];
	WPaper* wp2=objSettings.m_WChanger.aWPapersAndThemes[iPos2];
	if(wp1 && wp2){
		HIROW hTmp=wp1->hRow;
		wp1->hRow=wp2->hRow;
		wp2->hRow=hTmp;
		objSettings.m_WChanger.aWPapersAndThemes[iPos1]=wp2;
		objSettings.m_WChanger.aWPapersAndThemes[iPos2]=wp1;
		Attach(iPos1,TRUE);
		Attach(iPos2,TRUE);
	}
}

int iLastTheme=-1;
int iLastSortType=-1;
int bLastDirection=1;
HANDLE hSortThread=0;
void CDLG_WChanger::SortThemeImages(int iSortType)
{
	// Локируемся...
	if(lSortLock>0){
		return;
	}
	SimpleTracker Track(lSortLock);
	CSmartLock SL(&cWDlgSection,TRUE);
	//----------------------------------------------
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	int iThemeIndex=iCurrentItem;
	// Обновляем данные если тема...
	WPaper* wpTh=GetWPTheme(iThemeIndex);
	if(!wpTh){
		return;
	}
	//----------------------------------------------
	SetInfoBlock(Format("%s: %s\n%s...",_l("Theme"),wpTh->sTitle,_l("Gathering sort data")),1);
	// Подготавливаем сортировку
	if(iLastTheme==iThemeIndex){
		bLastDirection=!bLastDirection;
	}else{
		bLastDirection=0;
	}
	iLastTheme=iThemeIndex;
	iLastSortType=iSortType;
	//----------------------------------------------
	// Подготавливаем массив с номерами обоев
	int iWPCount=0;
	CDWordArray _ID_Base;
	CStringArray _ID_Keys;
	CArray<WPaper*,WPaper*> _IDs;
	int iPos=GetFirstThemeImage(iThemeIndex);
	int iWidth=0,iHeight=0;
	while(iPos!=-1){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iPos];
		CString sTitle=wp->sTitle;
		if(iSortType==2){
			int iDotPos=sTitle.ReverseFind('.');
			if(iDotPos>0){
				sTitle=sTitle.Mid(iDotPos)+sTitle;
			}
		}else if(iSortType==3){
			GetImageSizeFromFile(sTitle,iWidth,iHeight);
			sTitle=Format("%05li-%05li",iWidth,iHeight)+sTitle;
		}else if(iSortType==4){
			sTitle=GetRandomString(5);
		}
		sTitle.MakeUpper();
		// Загоняем в массив
		_ID_Keys.SetAtGrow(iWPCount,sTitle);
		_ID_Base.SetAtGrow(iWPCount,iPos);
		_IDs.SetAtGrow(iWPCount,wp);
		iWPCount++;
		// Смотрим, какая обоина следующая
		GetNextThemeImage(iThemeIndex,iPos);
	}
	if(iSortType==4){
		iSortType=1;
	}
	//----------------------------------------------
	DWORD dwTime=GetTickCount();
	DWORD dwMaxSortValue=iWPCount*iWPCount;
	// Сортируем наш массив с указателями на номера
	for(int iFirst=0;iFirst<iWPCount-1;iFirst++){
		for(iPos=iFirst+1;iPos<iWPCount;iPos++){
			CString sTitle1=_ID_Keys[iPos];
			CString sTitle2=_ID_Keys[iFirst];
			if(DWORD(sTitle1<sTitle2)^DWORD(bLastDirection)){
				WPaper* wpTmp=_IDs[iPos];
				_IDs[iPos]=_IDs[iFirst];
				_IDs[iFirst]=wpTmp;
				CString sTmp=_ID_Keys[iPos];
				_ID_Keys[iPos]=_ID_Keys[iFirst];
				_ID_Keys[iFirst]=sTmp;
			}
			if(GetTickCount()-dwTime>500){
				dwTime=GetTickCount();
				DWORD dwPercentage=DWORD(iFirst*iPos/dwMaxSortValue);
				SetInfoBlock(Format("%s: %s\n%s: %lu%%",_l("Theme"),wpTh->sTitle,_l("Sorting"),dwPercentage),1);
			}
		}
	}
	//----------------------------------------------
	SetInfoBlock(Format("%s: %s\n%s...",_l("Theme"),wpTh->sTitle,_l("Setting order")),1);
	// Восстанавливаем порядок
	SimpleTracker Track2(lSortLock);
	int i=0;
	CArray<HIROW,HIROW> hWPRows;
	for(i=0;i<iWPCount;i++){
		int iWPPosition=_ID_Base[i];
		hWPRows.SetAtGrow(i,objSettings.m_WChanger.aWPapersAndThemes[iWPPosition]->hRow);
	}
	for(i=0;i<iWPCount;i++){
		int iWPPosition=_ID_Base[i];
		objSettings.m_WChanger.aWPapersAndThemes[iWPPosition]=_IDs[i];
	}
	for(i=0;i<iWPCount;i++){
		int iWPPosition=_ID_Base[i];
		objSettings.m_WChanger.aWPapersAndThemes[iWPPosition]->hRow=hWPRows[i];
		Attach(iWPPosition,TRUE,FALSE);
	}
	//----------------------------------------------
	SetInfoBlock(Format("%s: %s\n%s!",_l("Theme"),wpTh->sTitle,_l("Sort completed")),1);
	hSortThread=0;
}

DWORD WINAPI SortThemeImages(int iSortType)
{
	if(pMainDialogWindow){
		if(pMainDialogWindow->dlgWChanger){
			pMainDialogWindow->dlgWChanger->SortThemeImages(iSortType);
		}
	}
	return 0;
}

HANDLEINTHREAD(CDLG_WChanger,OnWchangerSortByName)
{
	SortThemeImages(1);
	return 0;
}

HANDLEINTHREAD(CDLG_WChanger,OnWchangerSortByType)
{
	SortThemeImages(2);
	return 0;
}

HANDLEINTHREAD(CDLG_WChanger,OnWchangerSortByDims)
{
	if(objSettings.isLight){
		AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
		return 0;
	}
	SortThemeImages(3);
	return 0;
}

HANDLEINTHREAD(CDLG_WChanger,OnWchangerSortShuffle)
{
#ifdef USE_LITE
	AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
	return 0;
#endif
	SortThemeImages(4);
	return 0;
}

void CDLG_WChanger::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID==SC_CLOSE){
		bNeddRefreshAfterClose=0;
		if(bDialogInitOk && Apply()){
			DWORD dwReply=AskYesNo(_l("Do you want to quit "+AppName(),"Do you want to quit "+AppName()+"?\r\nPress 'NO' to minimize window"),&objSettings.lOnExitFromMainWnd,this);
			if(dwReply==IDYES){
				if(pMainDialogWindow){
					pMainDialogWindow->dlgWChanger=NULL;
					pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
				}
			}
			CloseThisWindow();
		}
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CDLG_WChanger::OnEndEditWList(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(!pRowState || pRowState->iIItem==-1){
		*result=-1;
		return;
	}
	CString sNewName=pRowState->strText;
	sNewName.TrimLeft();
	sNewName.TrimRight();
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[pRowState->dwData];
	if(wp->dwType==0){
		DWORD dwCheck=-1;
		if(wp==wpNowShowingTheme){
			m_ThActive.SetCheck(pRowState->dwNotifyData?1:0);
		}
		if(sNewName.IsEmpty()){
			wp->SetStatus(pRowState->dwNotifyData);
		}else{
			wp->sTitle=sNewName;
			SetThemeNameInTitle(wp);
		}
	}else{
		if(sNewName==wp->sTitle){
			*result=-1;
			return;
		}
		if(IsFileInZip(wp->sTitle) || IsFileInZip(sNewName)){
			SetInfoBlockAlert(_l("Error: file is in the archive")+"!");
			*result=-1;
			return;
		}
		if(isFileExist(sNewName)){
			SetInfoBlockAlert(TrimMessage(sNewName,20,3)+": "+_l("File already exist")+"!");
			*result=-1;
			return;
		}
		if((wp->sTitle.Find(":/")!=-1 || wp->sTitle.Find(":\\")!=-1) && sNewName.Find(":/")==-1 && sNewName.Find(":\\")==-1){
			*result=-1;
			return;
		}
		if(!IsFileURL(sNewName)){
			SHFILEOPSTRUCT str;
			memset(&str,0,sizeof(str));
			str.hwnd=this->GetSafeHwnd();
			str.wFunc=FO_MOVE;
			char szPath[MAX_PATH+2]="";
			memset(&szPath,0,sizeof(szPath));
			strcpy(szPath,wp->sTitle);
			str.pFrom=szPath;
			char szPath1[MAX_PATH+2]="";
			memset(&szPath1,0,sizeof(szPath1));
			strcpy(szPath1,sNewName);
			str.pTo=szPath1;
			str.fFlags=FOF_ALLOWUNDO|FOF_RENAMEONCOLLISION;
			if(SHFileOperation(&str)!=0){
				SetInfoBlockAlert(_l("Failed to rename an image file")+"!\n"+GetCOMError(GetLastError()));
				*result=-1;
				return;
			}
			sNewName=str.pTo;
		}
		wp->sTitle=sNewName;
	}
	*result=0;
}

void CDLG_WChanger::OnOK() 
{
	if(Apply()){
		static int bFirstTime=0;
		if(objSettings.lVeryFirstStart && bFirstTime==0){
			bFirstTime=1;
			// Дествия после первого раза...
#ifndef	USE_LITE
#ifndef ART_VERSION
			static BOOL bWasAdded=FALSE;
			if(!bWasAdded){
				CNote nt;
				CString sTargetDir=GetNotesFolder();
				nt.sNoteFile=Format("dnhelp_l%lu.wnt",GetApplicationLang());
				CString sThisNoteFile=GetApplicationDir()+nt.sNoteFile;
				if(isFileExist(sThisNoteFile)){
					nt.sNoteFile="dnhelp.wnt";
					CString sNewPath=GetUserFolder()+nt.sNoteFile;
					CopyFile(sThisNoteFile,sNewPath,FALSE);
					nt.LoadFromFile(sNewPath);
					objSettings.aDesktopNotes.Add(nt);
				}else{
					nt.sNoteFile="dnhelp.wnt";
					sThisNoteFile=GetApplicationDir()+nt.sNoteFile;
					if(isFileExist(sThisNoteFile)){
						nt.sNoteFile="dnhelp.wnt";
						CString sNewPath=GetUserFolder()+nt.sNoteFile;
						CopyFile(sThisNoteFile,sNewPath,FALSE);
						nt.LoadFromFile(sNewPath);
						objSettings.aDesktopNotes.Add(nt);
					}
				}
				bWasAdded=TRUE;
			}
#endif
#endif
			pMainDialogWindow->OrderModifierForNextWallpaper=-2;
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
		}
		CloseThisWindow();
	}
}

BOOL bWrapperChanged=0;
long lSelectedItemsCount=0;
BOOL bStopDelayOneTime=0;
long lbSkipIfPreviewNotAvailable=0;
long TrackSelChangeChangeInProgress=0;
DWORD WINAPI TrackSelChange(LPVOID pData)
{
	// Обеспечиваем задержку...
	InterlockedIncrement(&lSelectedItemsCount);
	long lCurrentItemCount=lSelectedItemsCount;
	if(!bStopDelayOneTime && objSettings.bEnablePreviewDelay){
		Sleep(objSettings.bEnablePreviewDelayMs);
	}
	bStopDelayOneTime=0;
	if(lCurrentItemCount!=lSelectedItemsCount){
		// Selection changed, этот поток уже не актуален
		return 0;
	}
	// 
	if(TrackSelChangeChangeInProgress>0){
		return 0;
	}
	SimpleTracker Track(TrackSelChangeChangeInProgress);
	CDLG_WChanger* pD=(CDLG_WChanger*)pData;
	if(!pD){
		return 0;
	}
	SimpleLocker lc(&objSettings.csWPInChange);
	int iSmapling=2;
	CString sDefPath=CString(GetApplicationDir())+"Template\\Images\\logo2.jpg";
	CString sWhTitle="";
	CString sWhPseudoPathTitle="";
	CString sWallpaperFilePath="";
	BOOL bTemplMode=pD->lWndMode,bTemplAD=0,bTemplReg=0;
	CString sSingleWPTMode="";
	if(bTemplMode){
		sSingleWPTMode=pD->sCurrentWidget;
		CWPT* wpt=GetWPT(pD->sCurrentWidget);
		if(wpt){
			sWallpaperFilePath=wpt->sPreview;
			bTemplAD=wpt->isAD();
			bTemplReg=0;//(objSettings.iLikStatus<2)&&(wpt->sHash=="");
		}
		if(sWallpaperFilePath==""){
			if(lbSkipIfPreviewNotAvailable){
				lbSkipIfPreviewNotAvailable=0;
				return 0;
			}
			sWallpaperFilePath=objSettings.sLastWallpaperFromUser;
		}else{
			sWhTitle=_l(wpt->sTitle);
			sWhPseudoPathTitle=sWhTitle;
			// Как обычная картинка...
			bTemplMode=0;
		}
	}
	lbSkipIfPreviewNotAvailable=0;
	if(sWallpaperFilePath==""){
		int iPathIndex=pD->iCurrentItem;
		if(iPathIndex>=0){
			CSmartLock SL(&pD->cWDlgSection,TRUE);
			sWhPseudoPathTitle="";
			int iThemeIndex=iPathIndex;
			WPaper* wp=0;
			if(iPathIndex>=0 && iPathIndex<objSettings.m_WChanger.aWPapersAndThemes.GetSize()){
				wp=objSettings.m_WChanger.aWPapersAndThemes[iPathIndex];
			}
			WPaper* wpTh=GetWPTheme(iThemeIndex);
			if(!wp || !wpTh || wp->dwType!=1){
				return 0;
			}
			sWallpaperFilePath=wp->sTitle;
			sWhTitle=wpTh->sTitle;
			iSmapling=wpTh->dwSampling;
		}else{
			sWallpaperFilePath=sDefPath;
			sWhTitle=_l("Choose image");
		}
	}
	if(sWallpaperFilePath==""){
		sWallpaperFilePath=sDefPath;
		sWhTitle=_l("Choose image");
	}
	CRect rcBounds;
	pD->m_Preview.GetWindowRect(&rcBounds);
	pD->sPreparedWallpaperFile=sWallpaperFilePath;
	pD->sWallpaperFile=sWallpaperFilePath;
	BOOL bShowPreviewRes=FALSE;
	HWND pD_hwnd=pD->GetSafeHwnd();
	if(!IsWindow(pD_hwnd)){// Окно уже закрыто?
		return 0;
	}
	{// Пытаемся показать превью с картинкой
		SimpleTracker ST(pD->lLoadInProgress);
		SimpleTracker ST3(objSettings.lAdForPrevModeIsON);
		////////////////////////////////
		// Рисуем если это новый файл...
		pD->m_Preview.sImgOverlay="";
		if(objSettings.bPreviewQuality==0 && !bTemplMode){
			// Быстрый предпросмотр
			CBitmap* pNew=_bmp().AddBmpFromDisk(sWallpaperFilePath);
			if(pNew){
				CSize pwReal=GetBitmapSize(pNew);
				CRect rtOut(rcBounds);
				FitImageToRect(rtOut,pwReal);
				CSize pwSize=rtOut.Size();
				if(!IsWindow(pD_hwnd)){return 0;};
				ClearBitmap(pD->hPreviewBmp);
				CopyBitmapToBitmap(*pNew,pD->hPreviewBmp,pwReal,&pwSize,FALSE);
				pD->sizeOfRealImage=pwReal;
				_bmp().DelBmp(sWallpaperFilePath);
				bShowPreviewRes=TRUE;
			}else{
				ShowWarningAboutMissedFile(sWallpaperFilePath);
			}
		}else{
			pD->m_Preview.Invalidate();// Типа "Loading..."
			//------------------------
			CSetWallPaper wpCommonData;
			wpCommonData.lScrMode=1;//Как для скринсейвера...
			wpCommonData.bRegardMM=0;
			wpCommonData.sInPreset=objSettings.sActivePreset;
			wpCommonData.bSuppressIntegration=TRUE;
			wpCommonData.sWallpaperThemeName=sWhTitle;
			wpCommonData.sWallpaperFile=sWallpaperFilePath;
			wpCommonData.sSingleWPTMode=bTemplMode?sSingleWPTMode:"";
			wpCommonData.bSkipWrapperStep=(pD->lWndMode)?FALSE:(objSettings.bPreviewQuality==2?FALSE:TRUE);
			wpCommonData.dwResamplingType=(objSettings.TemplatesInWork.GetSize()>0 && !wpCommonData.bSkipWrapperStep)?iSmapling:2;//Для обложки - реальный, иначе - пропрорционально
			wpCommonData.TemplatesInWork.Copy(objSettings.TemplatesInWork);
			lc.unlock();
			objSettings.lUseGlobalRandomWPTLevel=0;
			PrepareWallPaper(&wpCommonData);
			objSettings.lUseGlobalRandomWPTLevel=1;
			lc.lock();
			if(!IsWindow(pD_hwnd)){
				// Если ха это время окно закрылось-выходим
				return 0;
			};
			bWrapperChanged=0;
			if(wpCommonData.hbmOutImage){
				CSize pwSize=rcBounds.Size();
				if(!IsWindow(pD_hwnd)){return 0;};
				ClearBitmap(pD->hPreviewBmp);
				CopyBitmapToBitmap(wpCommonData.hbmOutImage,pD->hPreviewBmp,wpCommonData.sizeOfOutImage,&pwSize,TRUE);
				pD->sizeOfRealImage=wpCommonData.sizeOfRealImage;
				pD->sWallpaperFile=wpCommonData.sWallpaperFile;
				if(bTemplAD){
					pD->m_Preview.sImgOverlay=_l("Active desktop required");
					pD->m_Preview.sImgOverlay+=".\r\n";
					pD->m_Preview.sImgOverlay+=_l("Preview is not available");
					pD->m_Preview.sImgOverlay+=".\r\n";
					pD->m_Preview.sImgOverlay+=_l("Activate widget to see it in action!");
					pD->m_Preview.sImgOverlay+="\r\n";
				}
				bShowPreviewRes=TRUE;
			}
		}
	}
	if(IsWindow(pD_hwnd) && pD && IsWindow(pD->m_Preview.GetSafeHwnd())){
		pD->m_Preview.sOverwriteInfo=sWhPseudoPathTitle;
		pD->m_Preview.Invalidate();
	}
	return bShowPreviewRes;
}

void CDLG_WChanger::UpdateThemeControls(int iThemeIndex)
{
	if(lSortLock || lZipLock){
		// Сортировка - не из основного потока
		return;	
	}
	if(lWndMode){
		m_WeightEdit.EnableWindow(TRUE);
		GetDSelectedRow();
		SetThemeNameInTitle(0);
		CWPT* wpt=GetWPT(sCurrentWidget);
		if(sCurrentWidget=="" || !wpt){
			m_ThActive.SetCheck(0);
			GetDlgItem(IDC_SCR_WEIGHT)->EnableWindow(FALSE);
			GetDlgItem(IDC_ACTIVE_THEME)->EnableWindow(FALSE);
			GetDlgItem(IDC_STAT_DMODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BT_WOPT)->EnableWindow(FALSE);
			GetDlgItem(IDC_WIDGETDROPDOWN)->EnableWindow(FALSE);
			GetDlgItem(IDC_STAT_WEIGHT)->EnableWindow(FALSE);
			GetDlgItem(IDC_STAT_WEIGHT_VAL)->EnableWindow(FALSE);
			return;
		}
		GetDlgItem(IDC_SCR_WEIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_ACTIVE_THEME)->EnableWindow(TRUE);
		GetDlgItem(IDC_STAT_DMODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_WOPT)->EnableWindow(TRUE);
		GetDlgItem(IDC_WIDGETDROPDOWN)->EnableWindow(TRUE);
		GetDlgItem(IDC_STAT_WEIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_STAT_WEIGHT_VAL)->EnableWindow(TRUE);
		UpdateThemeWeightVal(0,wpt->GetWeight());
	}else{
		if(iThemeIndex<0){
			iThemeIndex=iCurrentItem;
		}
		if(iThemeIndex<0){
			return;
		}
		// Обновляем данные если тема...
		wpNowShowingTheme=GetWPTheme(iThemeIndex);
		if(wpNowShowingTheme){
			UpdateThemeWeightVal(wpNowShowingTheme,wpNowShowingTheme->dwTheme);
			SetThemeNameInTitle(wpNowShowingTheme);
			((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->SetCurSel(wpNowShowingTheme->dwSampling);
			if(iCurrentItem==-1){
				m_WList.SelectIRow(wpNowShowingTheme->hRow,TRUE,FALSE);
			}
			UpdateData(TRUE);
		}
		m_WeightEdit.EnableWindow(m_CycleType==0);
		GetDlgItem(IDC_ACTIVE_THEME)->EnableWindow(TRUE);
		GetDlgItem(IDC_STAT_DMODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_STAT_WEIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_STAT_WEIGHT_VAL)->EnableWindow(TRUE);
	}
}

void CDLG_WChanger::OnSelChanged(NMHDR * pNotifyStruct, LRESULT * result)
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	int iThemeIndex=iCurrentItem;
	UpdateThemeControls(iThemeIndex);
	// Отрисовка
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem];
	if(wp && wp->dwType==1 && wp->sTitle!=sWallpaperFile){
		if(((CButton*)GetDlgItem(IDC_PREVIEW_CHECK))->GetCheck()){
			FORK(TrackSelChange,this);
		}else{
			sWallpaperFile=wp->sTitle;
			m_Preview.Invalidate();
		}
	}
}

void CDLG_WChanger::OnPreviewCheck() 
{
	bStopDelayOneTime=TRUE;
	FORK(TrackSelChange,this);
}

void SetWallpaperBySrc(CString sSrc);
int CDLG_WChanger::Apply(BOOL bSilent)
{
	if(lZipLock){
		Alert(_l("Zip operation in progress, please wait"));
		return 0;
	}
	if(lSortLock){
		Alert(_l("Sort operation in progress, please wait"));
		return 0;
	}
	CSmartLock SL(&cWDlgSection,FALSE,IDC_CURSORSAVE);
	if(SL.IsLocked()){
		return FALSE;
	}
	if(!bDataLoaded){
		// Придется дождаться пока загрузится
		return FALSE;
	}
	UpdateData(TRUE);
	UpdateEnviverState();
	objSettings.lChangePriodically=m_Activate;
	long lNewPeriod=atol(m_Hour)*60+atol(m_Min);
	if(lNewPeriod==0){
		lNewPeriod=30;
	}
	BOOL bNeedUpdateTooltip=(objSettings.lChangePeriod!=lNewPeriod);
	objSettings.lChangePeriod=lNewPeriod;
	if(pMainDialogWindow && bNeedUpdateTooltip){
		pMainDialogWindow->SetTrayTooltip(TRUE);
	}
	objSettings.bUsePreview=m_UsePreview;
	objSettings.bChangeWPaperCycleType=m_CycleType;
	CRect rt;
	m_WList.GetClientRect(&rt);
	objSettings.iColWChWidth=rt.Width()-1;
	BOOL bBaloonWas=0;
	DWORD dwTime1=GetTickCount();
	if(objSettings.m_WChanger.aWPapersAndThemes.GetSize()>3000 && objSettings.bReportLongImageProcs==0){
		pMainDialogWindow->m_STray.PopupBaloonTooltip(_l("Please wait"),_l(AppName()+" is refreshing list of images, this can take several minutes. Please, be patient :)"));
		bBaloonWas=1;
	}
	if(strlen(sImageToBeSetAfterClose)==0){
		if(iLastWPSetted>=0 && iLastWPSetted<objSettings.m_WChanger.aWPapersAndThemes.GetSize() && objSettings.m_WChanger.aWPapersAndThemes[iLastWPSetted]!=NULL){
			strcpy(sImageToBeSetAfterClose,objSettings.m_WChanger.aWPapersAndThemes[iLastWPSetted]->sTitle);
		}
	}
	objSettings.m_WChanger.ReinitWPaperShuffle(sImageToBeSetAfterClose,FALSE);
	objSettings.m_WChanger.Save();
	objSettings.SaveActiveWPT();
	DWORD dwTime2=GetTickCount();
	if(bBaloonWas && pMainDialogWindow){
		pMainDialogWindow->m_STray.PopupBaloonTooltip("","");
		if(dwTime2-dwTime1<2000){
			objSettings.bReportLongImageProcs=1;
		}
	}
	if(!bSilent && objSettings.m_WChanger.GetLastActivePictureNum()==0){
		Alert(_l("No themes selected or\nall selected themes are empty")+"!",_l(AppName()+": Warning"));
	}
#ifdef ART_VERSION
	if(pMainDialogWindow){
		bNeddRefreshAfterClose=0;
		if(sWallpaperFile!=""){
			SetWallpaperBySrc(sWallpaperFile);
		}else{
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_RANDWALLP,0);
		}
	}
#endif
	if(bNeddRefreshAfterClose && pMainDialogWindow){
		pMainDialogWindow->OrderModifierForNextWallpaper=-2;
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
	}
	CRect rtPreviewSize;
	GetDlgItem(IDC_STAT_PREV)->GetWindowRect(rtPreviewSize);
	ScreenToClient(&rtPreviewSize);
	char szPreviewPos[128]={0};
	sprintf(szPreviewPos,"{%i,%i,%i,%i}",rtPreviewSize.left,rtPreviewSize.top,rtPreviewSize.right,rtPreviewSize.bottom);
	SetRegSetting("", "PreviewPos", szPreviewPos);
	return TRUE;
}

DWORD WINAPI SetWPInThread(LPVOID lpData)
{
	static long lLock=0;
	if(lLock>0){
		return 1;
	}
	SimpleTracker Track(lLock);
	if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
		pMainDialogWindow->dlgWChanger->m_BtSetNow.EnableWindow(FALSE);		
		pMainDialogWindow->dlgWChanger->m_BtSetNow.SetStyles(CBS_FLAT);
		pMainDialogWindow->dlgWChanger->m_BtSetNow.Invalidate();
	}
	objSettings.lCurPicture=DWORD(lpData);
	objSettings.m_WChanger.SetWallPaperByIndex(objSettings.lCurPicture);
	if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
		pMainDialogWindow->dlgWChanger->m_BtSetNow.EnableWindow(TRUE);
		pMainDialogWindow->dlgWChanger->m_BtSetNow.SetStyles(CBS_DDOWN2|CBS_NORMAL);
		pMainDialogWindow->dlgWChanger->m_BtSetNow.Invalidate();
	}
	return 0;
}

DWORD WINAPI SetWPInThreadXX(LPVOID lpData)
{
	if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
		pMainDialogWindow->dlgWChanger->SetInfoBlock(_l("Generating wallpaper")+"...",1);
	}
	//SimpleTracker Track(objSettings.lSkipMorthOnGeneration);
	SetWPInThread(lpData);
	if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
		pMainDialogWindow->dlgWChanger->SetInfoBlock(_l("Wallpaper installed successfully"),1);
	}
	return 0;
}

void CDLG_WChanger::SetLastWPSetted(int iNewValue, BOOL bSelectNew) 
{
	if(iNewValue<0 || iNewValue>=objSettings.m_WChanger.aWPapersAndThemes.GetSize()){
		return;
	}
	if(iLastWPSetted>=0 && iLastWPSetted<objSettings.m_WChanger.aWPapersAndThemes.GetSize()){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iLastWPSetted];
		if(wp && wp->hRow){
			m_WList.SetIRowTextStyle(wp->hRow,FL_NORMAL);
		}
	}
	if(iNewValue==iLastWPSetted){
		bSelectNew=FALSE;
	}
	iLastWPSetted=iNewValue;
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iLastWPSetted];
	if(wp && wp->hRow && wp->dwType==1 && objSettings.m_WChanger.aWPapersAndThemes[wp->dwTheme]->dwImagesCount<1000){
		HIROW hParent=m_WList.GetParentIRow(wp->hRow);
		if(hParent && hParent!=FL_ROOT){
			m_WList.Expand(hParent);
		}
		m_WList.EnsureVisible(wp->hRow,0,FALSE);
		m_WList.SelectIItem(wp->hRow,0,0,bSelectNew);
		m_WList.SetIRowTextStyle(wp->hRow,FL_BOLD);
		iCurrentItem=m_WList.GetIRowData(wp->hRow);
	}
}

void CDLG_WChanger::OnChangeEditWeight() 
{
	GetSelectedRow();
	int iThemeIndex=iCurrentItem;
	WPaper* wp=GetWPTheme(iThemeIndex);
	if(!wp){
		return;
	}
	wp->dwTheme=m_WeightEdit.GetPos();
}

void CDLG_WChanger::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CDLG_WChanger::OnClose() 
{
	CDialog::OnClose();
}

void CDLG_WChanger::PostNcDestroy() 
{
	{
		if(wndShotPhase2){
			::PostMessage(wndShotPhase2,WM_CLOSE,0,0);
			::DestroyWindow(wndShotPhase2);
			wndShotPhase2=0;
		}
		::InterlockedDecrement(&objSettings.lMainWindowOpened);
		// Пока не заблокировали, ждем...
		CSmartLock SL(&cWDlgSection,TRUE);
		//
		CDialog::PostNcDestroy();
		if(pMainDialogWindow){
			if(pMainDialogWindow->dlgWChanger){
				pMainDialogWindow->dlgWChanger=NULL;
			}
		}
	}
	delete this;
}

void CDLG_WChanger::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

void CDLG_WChanger::OnSelchangeComboThemefitting() 
{
	DWORD dwSampling=((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->GetCurSel();
	GetSelectedRow(0);
	int iThemeNum=iCurrentItem;
	WPaper* wp=GetWPTheme(iThemeNum);
	if(!wp){
		return;
	}
	wp->dwSampling=dwSampling;
	if(wp->dwSampling<0 || wp->dwSampling>4){
		wp->dwSampling=0;
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_THEMEFITTING))->SetCurSel(wp->dwSampling);
}

void CDLG_WChanger::OnSelchangeComboSequence() 
{
	UpdateData(TRUE);
	m_WeightEdit.EnableWindow(m_CycleType==0);
}

#define DEFLINK_URL "http://"
void CDLG_WChanger::OnWchangerAddhyperlink() 
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	GetSelectedRow();
	int iThemeNum=iCurrentItem;
	WPaper* wp=GetWPTheme(iThemeNum);
	DWORD dwTheme=iThemeNum;
	if(!wp){
		return;
	}
	CString sLink=DEFLINK_URL;
	CDLG_AddLink dlg(this);
	dlg.pLink=&sLink;
	dlg.DoModal();
	if(sLink!="" && sLink!=DEFLINK_URL){
		AddFile(sLink,dwTheme);
	}
}

// CWPPreview
IMPLEMENT_DYNAMIC(CWPPreview, CButton)
CWPPreview::CWPPreview(CDLG_WChanger* pWPDialog)
{
	m_pWPDialog=pWPDialog;
}

CWPPreview::~CWPPreview()
{
}


BEGIN_MESSAGE_MAP(CWPPreview, CButton)
END_MESSAGE_MAP()

// CWPPreview message handlers
void CWPPreview::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	BOOL bLastDrawPicRes=0;
	CRect rcBounds;
	GetClientRect(&rcBounds);
	DWORD dwSize=0;
	CString sFInfo=" ";
	CString sFNameExt="";
	CString sFName=_l("Preview");
	BOOL bPreviewON=((CButton*)(m_pWPDialog->GetDlgItem(IDC_PREVIEW_CHECK)))->GetCheck();
	CString sPath=m_pWPDialog->sWallpaperFile;
	if(sPath!=""){
		dwSize=GetFileSize(sPath);
		// Экстрактим имя файла
		sFName=sPath;
		int iPos=sFName.ReverseFind('\\');
		if(iPos!=-1){
			sFName=sFName.Mid(iPos+1);
		}
		int iPosE=sPath.ReverseFind('.');
		if(iPosE!=-1){
			sFNameExt=sPath.Mid(iPosE+1);
		}
		sFNameExt.MakeUpper();
	}
	BOOL bSizeRetOK=FALSE;
	if(bPreviewON){
		if(m_pWPDialog->hPreviewBmp!=NULL){
			pDC->FillSolidRect(rcBounds,objSettings.lWPBGColor);
			CRect rtOut(rcBounds);
			CBitmap bmpT;
			bmpT.Attach(m_pWPDialog->hPreviewBmp);
			CBitmap* pPreviewBmp=&bmpT;
			CSize bmpSz(GetBitmapSize(pPreviewBmp));
			FitImageToRect(rtOut,bmpSz);
			CDC pBitmapDC;
			CDesktopDC dcDesc;
			pBitmapDC.CreateCompatibleDC(&dcDesc);
			CBitmap* bmpTmp=pBitmapDC.SelectObject(pPreviewBmp);
			//bLastDrawPicRes=pDC->DrawState(rtOut.TopLeft(),rtOut.Size(),pPreviewBmp,DST_BITMAP|DSS_NORMAL,0);
			pDC->SetStretchBltMode(HALFTONE);
			bLastDrawPicRes=pDC->StretchBlt(rtOut.left,rtOut.top,rtOut.Width(),rtOut.Height(),&pBitmapDC,0,0,bmpSz.cx,bmpSz.cy,SRCCOPY);
			pBitmapDC.SelectObject(&bmpTmp);
			// Размер и имя
			if(m_pWPDialog->sizeOfRealImage.cx>0 && m_pWPDialog->sizeOfRealImage.cy>0){
				bSizeRetOK=TRUE;
				sFInfo+=Format("%i:%i",m_pWPDialog->sizeOfRealImage.cx,m_pWPDialog->sizeOfRealImage.cy);
			}
			sFInfo=TrimMessage(sFInfo,60);
			bmpT.Detach();
		}else{
			if(objSettings.WPInProgress){
				bLastDrawPicRes=-1;
			}
		}
	}
	if(!bSizeRetOK){
		int iW=0,iH=0;
		if(!IsFileHTML(sPath) && GetImageSizeFromFile(sPath,iW,iH)){
			sFInfo+=Format("%i:%i",iW,iH);
		}
	}
	if(!bPreviewON || m_pWPDialog->hPreviewBmp==NULL){
		if(bLastDrawPicRes!=TRUE){
			pDC->FillSolidRect(rcBounds,RGB(120,120,120));
		}
		if(bLastDrawPicRes==0){
			pDC->MoveTo(2,2);
			pDC->LineTo(rcBounds.Width()-2,rcBounds.Height()-2);
			pDC->MoveTo(2,rcBounds.Height()-2);
			pDC->LineTo(rcBounds.Width()-2,2);
		}
	}
	CBrush br(RGB(0,0,0));
	pDC->FrameRect(rcBounds,&br);
	if(sFNameExt!=""){
		sFInfo.TrimRight();
		sFInfo+=(sFInfo!=""?"; ":" ")+sFNameExt;
	}
	if(dwSize>0){
		sFInfo.TrimRight();
		sFInfo+=(sFInfo!=""?"; ":" ")+_l("Size")+": "+GetSizeStrFrom(dwSize,0);
	}
	CRect rcTxt(rcBounds);
	rcTxt.DeflateRect(5,5);
	if(bLastDrawPicRes==-1 || (m_pWPDialog->lLoadInProgress)){
		CString sText=_l("Wait, please")+"...";
		if(m_pWPDialog->sPreparedWallpaperFile!=""){
			sText=(m_pWPDialog->lWndMode?_l("Generating wallpaper"):_l("Loading file"))+Format(" \r\n %s ",m_pWPDialog->sPreparedWallpaperFile);
		}
		::SmartDrawText(*pDC,sText,rcTxt,NULL,RGB(255,255,255),RGB(0,0,0),TXSHADOW_MIXBG|TXSHADOW_SIMPLE,DT_CENTER|DT_VCENTER|DT_PATH_ELLIPSIS);//DT_RIGHT|DT_BOTTOM//TXSHADOW_VCENT|TXSHADOW_HCENT|TXSHADOW_GLOW
	}
	/*if(objSettings.iLikStatus==1){
		::SmartDrawText(*pDC,_l("Unregistered"),rcTxt,NULL,RGB(rnd(0,127),rnd(0,127),rnd(0,127)),RGB(rnd(127,255),rnd(127,255),rnd(127,255)),TXSHADOW_GLOW,DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);//DT_RIGHT|DT_BOTTOM//TXSHADOW_VCENT|TXSHADOW_HCENT
	}else */
	if(bLastDrawPicRes==0){
		::SmartDrawText(*pDC,Format(AppName()+" v%s",PROG_VERSION),rcTxt,NULL,RGB(rnd(0,255),rnd(0,255),rnd(0,255)),RGB(rnd(0,255),rnd(0,255),rnd(0,255)),TXSHADOW_GLOW,DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);//DT_RIGHT|DT_BOTTOM//TXSHADOW_VCENT|TXSHADOW_HCENT
	}
	if(sImgOverlay!=""){
		::SmartDrawText(*pDC,sImgOverlay,rcTxt,NULL,RGB(rnd(0,127),rnd(0,127),rnd(0,127)),RGB(rnd(127,255),rnd(127,255),rnd(127,255)),TXSHADOW_SIMPLE|TXSHADOW_MIXBG,DT_LEFT|DT_TOP);//DT_RIGHT|DT_BOTTOM//TXSHADOW_VCENT|TXSHADOW_HCENT
	}
	CString sForRes=sOverwriteInfo;
	if(sForRes!=""){
		m_pWPDialog->GetDlgItem(IDC_PREVIEW_CHECK)->SetWindowText(_l("Widget preview"));
		m_pWPDialog->GetDlgItem(IDC_BT_REM_IMG)->EnableWindow(FALSE);
		m_pWPDialog->GetDlgItem(IDC_SETNOW)->EnableWindow(FALSE);
	}else{
		m_pWPDialog->GetDlgItem(IDC_BT_REM_IMG)->EnableWindow(TRUE);
		m_pWPDialog->GetDlgItem(IDC_SETNOW)->EnableWindow(TRUE);
		m_pWPDialog->GetDlgItem(IDC_PREVIEW_CHECK)->SetWindowText(sFName);
		sFInfo.TrimLeft();
		if(sFInfo!="" && !IsFileHTML(sPath)){
			sForRes=CString(" ")+sFInfo;
		}else{
			sForRes=_l("Choose an image from the list");
		}
	}
	/*if(objSettings.sActivePreset!=""){
		sForRes+=Format("\r\n %s: %s",_l("Active preset"),objSettings.sActivePreset);
	}*/
	m_pWPDialog->GetDlgItem(IDC_RESINFO)->SetWindowText(sForRes);
}

void CDLG_WChanger::OnBnClickedOptions()
{
	UpdateData(TRUE);
	objSettings.bUsePreview=m_UsePreview;
	objSettings.bChangeWPaperCycleType=m_CycleType;
	objSettings.lChangePriodically=m_Activate;
	objSettings.lChangePeriod=atol(m_Hour)*60+atol(m_Min);
	objSettings.OpenOptionsDialog(_l(WALLPAPERLIST_TP),this);
	m_Activate=objSettings.lChangePriodically;
	m_UsePreview=objSettings.bUsePreview;
	m_CycleType=objSettings.bChangeWPaperCycleType;
	SetHours();
	UpdateData(FALSE);
	OnCheckActive();
}

void CDLG_WChanger::OnBtSortTh() 
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		m_WList.SelectIItem(m_WList.GetIRowFromIndex(0),0);
		GetSelectedRow();
	}
	CRect rt;
	m_BtImgMisc.GetWindowRect(&rt);
	OnRclickWlist(-6,CPoint(rt.right,rt.top));
}

void CDLG_WChanger::OnBtSyncTh() 
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		m_WList.SelectIItem(m_WList.GetIRowFromIndex(0),0);
		GetSelectedRow();
	}
	CRect rt;
	m_BtThSync.GetWindowRect(&rt);
	OnRclickWlist(-7,CPoint(rt.right,rt.top));
}

void CDLG_WChanger::OnBtRemImg() 
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		m_WList.SelectIItem(m_WList.GetIRowFromIndex(0),0);
		GetSelectedRow();
	}
	CRect rt;
	m_BtRemImg.GetWindowRect(&rt);
	OnRclickWlist(-5,CPoint(rt.right,rt.top));
}

void CDLG_WChanger::OnBtAddDir() 
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		m_WList.SelectIItem(m_WList.GetIRowFromIndex(0),0);
		GetSelectedRow();
	}
	CRect rt;
	m_BtAddDir.GetWindowRect(&rt);
	OnRclickWlist(-3,CPoint(rt.right,rt.top));
}

void CDLG_WChanger::OnBtAddFile() 
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		m_WList.SelectIItem(m_WList.GetIRowFromIndex(0),0);
		GetSelectedRow();
	}
	CRect rt;
	m_BtAddFile.GetWindowRect(&rt);
	OnRclickWlist(-1,CPoint(rt.right,rt.top));
}

void CDLG_WChanger::OnBtAddArch() 
{
	On0Tab();
	PostMessage(WM_COMMAND,ID_WCHANGER_ADDARCHIVE,0);
}

void CDLG_WChanger::OnBtRem() 
{
	// С показом меню
	GetSelectedRow();
	if(iCurrentItem==-1){
		m_WList.SelectIItem(m_WList.GetIRowFromIndex(0),0);
		GetSelectedRow();
	}
	CRect rt;
	m_BtRemove.GetWindowRect(&rt);
	OnRclickWlist(-4,CPoint(rt.right,rt.top));
}

void CDLG_WChanger::OnWchangerRemoveDuplicatesFld()
{
	if(objSettings.isLight){
		AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
		return;
	}
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	if(objSettings.iLikStatus<2 && !isTempKey()){
		UnregAlert(OPTION_UNREG);
		return;
	}
	int iCount=0;
	CString sList;
	char szDir[MAX_PATH]="";
	strcpy(szDir,objSettings.sPictInitDir);
	if(!GetFolder(_l("Choose directory with images"), szDir, NULL, this->GetSafeHwnd())){
		return;
	}
	objSettings.sPictInitDir=szDir;
	objSettings.sPictInitDir.TrimRight("\\/");
	objSettings.sPictInitDir+="\\";
	CFileInfoArray dir;
	dir.AddDir(objSettings.sPictInitDir,VALID_EXTEN,TRUE,0,FALSE);
	for (int i=0;i<dir.GetSize();i++){
		CString sFile=dir[i].GetFilePath();
		if(sFile!="" && !IsFileHTML(sFile)){
			sList+=sFile+"\r\n";
			iCount++;
		}
	}
	BOOL bActualRem=1;
	if(sList!=""){
		CRYPT_START
		SmartWarning(_l(DEF_FDUP),AppName(),&objSettings.lShowDupHelp,DEF_WARNTIME*3);
		_GetApplicationLang fp0=(_GetApplicationLang)GetProcAddress(objSettings.hUniquer,"GetApplicationLang");
		if(fp0){
			(*fp0)()=GetApplicationLang();
		}
		BOOL bRemoded=1;
		if(objSettings.iLikStatus<2 || isTempKey()){
			_SpecOptions fp=(_SpecOptions)GetProcAddress(objSettings.hUniquer,"SpecOptions");
			if(fp){
				bRemoded=0;
				(*fp)(0);
			}
		}
		_FindDupes fp=(_FindDupes)GetProcAddress(objSettings.hUniquer,"FindDupes");
		if(fp){
			iCount=(*fp)(sList,GetSafeHwnd());
			if(bRemoded==0 && iCount>0){
				UnregAlert(Format("%s: %i\n%s!\n%s",_l("Similar images found"),iCount,_l("Order "+AppName()+" to remove them\nfrom your hard drive completely"),_l("Demo version is able to detect them only")));
				iCount=-1;
			}
		}
		CRYPT_END
	}
	if(iCount>0){
		SetInfoBlockAlert(_l("Remove all duplications")+": "+_l("Folder on disk")+";\n"+_l("Image(s) removed")+Format(": %i",iCount));
	}else if(iCount==0){
		SetInfoBlockAlert(_l("Remove all duplications")+": "+_l("Folder on disk")+";\n"+_l("No duplications were found"));
	}
}

void CDLG_WChanger::OnWchangerRemoveDuplicatesAll()
{
	if(objSettings.isLight){
		AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
		return;
	}
	if(objSettings.iLikStatus<2 && !isTempKey()){
		UnregAlert(OPTION_UNREG);
		return;
	}
	WPaper* wpTheme=NULL;
	int dwTheme=-1;
	if(OnWchangerDeleteallFromFolder(-1,dwTheme,&wpTheme)>0){
		int iCount=OnWchangerDeleteallFromFolder(TRUE,dwTheme,&wpTheme);
		SetInfoBlockAlert(_l("Remove all duplications")+": "+_l("All themes")+";\n"+_l("Image(s) removed")+Format(": %i",iCount));
	}
}

void CDLG_WChanger::OnWchangerRemoveDuplicates()
{
	if(objSettings.isLight){
		AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
		return;
	}
	if(objSettings.iLikStatus<2 && !isTempKey()){
		UnregAlert(OPTION_UNREG);
		return;
	}
	WPaper* wpTheme=NULL;
	if(OnWchangerDeleteallFromFolder(-1,&wpTheme)>0){
		int iCount=OnWchangerDeleteallFromFolder(TRUE,&wpTheme);
		SetInfoBlockAlert(_l("Theme")+": "+wpTheme->sTitle+";\n"+_l("Image(s) removed")+Format(": %i",iCount));
	}
}

void CDLG_WChanger::OnWchangerRemovebroken()
{
	WPaper* wpTheme=NULL;
	int iCount=OnWchangerDeleteallFromFolder(TRUE,&wpTheme);
	if(iCount>0){
		SetInfoBlockAlert(_l("Theme")+": "+wpTheme->sTitle+";\n"+_l("Image(s) removed")+Format(": %i",iCount));
	}else if(iCount==0){
		SetInfoBlockAlert(_l("Theme")+": "+wpTheme->sTitle+";\n"+_l("No broken images found!"));
	}
}

void CDLG_WChanger::OnWchangerRenameImage()
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	WPaper* wpItem=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem];
	if(wpItem){
		m_WList.SelectAndEditIItem(wpItem->hRow,0);
	}
}

void CDLG_WChanger::OnWchangerRenameTheme()
{
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	int iCurrentTheme=iCurrentItem;
	WPaper* wpTheme=GetWPTheme(iCurrentTheme);
	if(wpTheme){
		RefreshThemeTitle(iCurrentTheme);
		m_WList.SelectAndEditIItem(wpTheme->hRow,0);
	}
}

void CDLG_WChanger::OnWchangerFoldersync() 
{
#ifdef USE_LITE
	AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
	return;
#endif
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	int iCurrentTheme=iCurrentItem;
	WPaper* wpTheme=GetWPTheme(iCurrentTheme);
	if(wpTheme){
		wpTheme->dwFolderSync=wpTheme->dwFolderSync?0:1;
	}
}

void CDLG_WChanger::OnWchangerFoldersyncSet()
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	int dwTmemeIndex=iCurrentItem;
	WPaper* wpTheme=GetWPTheme(dwTmemeIndex);
	if(wpTheme){
		char szDir[MAX_PATH]="";
		if(wpTheme->GetWPar("SYNC")!=""){
			strcpy(szDir,wpTheme->GetWPar("SYNC"));
		}else{
			strcpy(szDir,GetFirstFolderImagePath(dwTmemeIndex));
		}
		if(GetFolder(_l("Choose directory"), szDir, NULL, this->GetSafeHwnd())){
			if(szDir[strlen(szDir)-1]!='\\'){
				strcat(szDir,"\\");
			}
			wpTheme->SetWPar("SYNC",szDir);
		}else{
			wpTheme->SetWPar("SYNC","");
		}
		SetThemeNameInTitle(wpTheme);
	}
}

CString CDLG_WChanger::GetFirstFolderImagePath(DWORD dwTmemeIndex)
{
	for(int i=0;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[i];
		if(wp){
			if(wp->dwType==1 && int(wp->dwTheme)==dwTmemeIndex){
				char szDisk[MAX_PATH]="C://Temp", szPath[MAX_PATH]="NoPathEver";
				_splitpath(wp->sTitle, szDisk, szPath, NULL, NULL);
				CString sFolderPath=szDisk;
				sFolderPath+=szPath;
				return sFolderPath;
			}
		}
	}
	return "";
}

void CDLG_WChanger::OnWchangerFoldersyncNow()
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
#ifdef USE_LITE
	AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
	return;
#endif
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	int dwTmemeIndex=iCurrentItem;
	WPaper* wpTheme=GetWPTheme(dwTmemeIndex);
	if(wpTheme){
		// Ищем первую картинку из темы
		CString sFolderPath=wpTheme->GetWPar("SYNC");
		if(sFolderPath==""){
			sFolderPath=GetFirstFolderImagePath(dwTmemeIndex);
		}
		if(sFolderPath==""){
			Alert(_l("Theme is not associated with folder yet!"),AppName(),this);
			return;
		}
		AddFolder(TRUE,objSettings.bLoadDirsRecursively,sFolderPath);
	}
}

void CDLG_WChanger::OnWchangerRemovewithimageX(int iNumber)
{
	if(!objSettings.m_WChanger.aWPapersAndThemes[iNumber]){
		return;
	}
	if(objSettings.m_WChanger.aWPapersAndThemes[iNumber]->dwType!=1){
		return;
	}
	DWORD dwTheme=iNumber;
	m_WList.DeleteIRow(objSettings.m_WChanger.aWPapersAndThemes[dwTheme]->hRow);
	CString sPath=objSettings.m_WChanger.aWPapersAndThemes[dwTheme]->sTitle;
	delete objSettings.m_WChanger.aWPapersAndThemes[dwTheme];
	objSettings.m_WChanger.aWPapersAndThemes[dwTheme]=NULL;
	if(IsFileInZip(sPath)){
		SetInfoBlockAlert(_l("File is in the archive and can`t be deleted")+"!");
	}else if(sPath!="" && !IsFileHTML(sPath) && isFileExist(sPath)){
		SHFILEOPSTRUCT str;
		memset(&str,0,sizeof(str));
		str.hwnd=this->GetSafeHwnd();
		str.wFunc=FO_DELETE;
		char szPath[MAX_PATH+2]="";
		memset(&szPath,0,sizeof(szPath));
		strcpy(szPath,sPath);
		str.pFrom=szPath;
		str.pTo=NULL;
		str.fFlags=FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT;
		if(SHFileOperation(&str)!=0){
			SetInfoBlockAlert(_l("Can`t delete")+" '"+TrimMessage(sPath,20,3)+"'");
		}
	}
}

void CDLG_WChanger::OnWchangerRemovewithimage() 
{
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	DWORD dwReply=AskYesNo(_l("Do you really want to delete this image(s) and corresponding file(s)")+"?",_l("Confirmation"),&objSettings.lDelImageWarn,this);
	if(dwReply!=IDYES){
		objSettings.lDelImageWarn=0;
		return;
	}
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	BOOL bMulty=m_WList.IsSelectionMulty();
	if(!bMulty){
		OnWchangerRemovewithimageX(iCurrentItem);
	}else{
		int iCount=0;
		CHIROWArray& aSelected=m_WList.GetMultySelection();
		for(int i=0;i<aSelected.GetSize();i++){
			if(m_WList.CheckIRow(aSelected[i])){
				OnWchangerRemovewithimageX(m_WList.GetIRowData(aSelected[i]));
			}
			iCount++;
		}
		if(iCount>1){
			SetInfoBlockAlert(_l("Image(s) removed")+Format(": %i",iCount));
		}
		m_WList.DropMultySelection();
	}
}

void CDLG_WChanger::OnBtEdit() 
{
	static long lBtEditLock=0;
	if(lBtEditLock>0){
		return;
	}
	SimpleTracker Track(lBtEditLock);
	//
	GetSelectedRow();
	if(iCurrentItem==-1){
		return;
	}
	if(!objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem]){
		return;
	}
	if(objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem]->dwType!=1){
		return;
	}
	CString sPath=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem]->sTitle;
	if(sPath!="" && !IsFileHTML(sPath) && isFileExist(sPath)){
		if(int(::ShellExecute(this->GetSafeHwnd(),"edit",sPath,NULL,NULL,SW_SHOWNORMAL))<=32){
			if(int(::ShellExecute(this->GetSafeHwnd(),"open",sPath,NULL,NULL,SW_SHOWNORMAL))<=32){
				Alert(_l("Associated editor not found")+"!");
			}
		}
	}
}

void CDLG_WChanger::OnBtProp() 
{
	static long lBtPropLock=0;
	if(lBtPropLock>0){
		return;
	}
	SimpleTracker Track(lBtPropLock);
	AppDisplayProperties();
}


void CDLG_WChanger::OnCheckActive()
{
	UpdateData(TRUE);
	if(m_Activate){
		GetDlgItem(IDC_HOUR)->EnableWindow(TRUE);
		GetDlgItem(IDC_MIN)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_HOUR)->EnableWindow(FALSE);
		GetDlgItem(IDC_MIN)->EnableWindow(FALSE);
	}
}

BOOL CDLG_WChanger::OpenFileForView(CString sPath)
{
	static long lViewLock=0;
	if(lViewLock>0){
		return FALSE;
	}
	SimpleTracker Track(lViewLock);
	//
	if(sPath!="" && !IsFileHTML(sPath) && isFileExist(sPath)){
		if(int(::ShellExecute(this->GetSafeHwnd(),"open",sPath,NULL,NULL,SW_SHOWNORMAL))<=32){
			Alert(_l("Associated viewer not found")+"!");
		}else{
			return TRUE;
		}
	}
	return FALSE;
}

void CDLG_WChanger::OnPreviewClick()
{
	OpenFileForView(sPreparedWallpaperFile);
}

void CDLG_WChanger::RefreshTemplates(int iAtIndex)
{
	BOOL bScroll=0;
	if(iTemplatesInit==0){
		iTemplatesInit=1;
		{
			bScroll=1;
			m_DList.SetReadOnly(FALSE);
			m_DList.SetImageList(&m_imageList, LVSIL_SMALL);
			ListView_SetExtendedListViewStyleEx(m_DList.m_hWnd, 0, LVS_EX_FULLROWSELECT);
			m_DList.InsertFColumn( 0, _l("Installed widgets"), TRUE, LVCFMT_LEFT, objSettings.iColWChWidth);
			m_DList.SetFColumnImageOffset(0, 1);
			m_DList.SetIItemControlType(NULL, 0);
			m_DList.bLockUpdateForCollapse=TRUE;
			m_DList.SetViewOptions(VIEW_MSEL);
			//
			m_DList.m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
			m_DList.m_crIRowBackground=GetSysColor(COLOR_WINDOW);
			m_DList.SetBkColor(GetSysColor(COLOR_WINDOW));
			m_DList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
			m_DList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
			m_DList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);
			m_DList.bShowButtonsAsLinks=TRUE;
		}
		hLastSelected=0;
	}
	FORK(TrackSelChange,this);	// Рядовое переключение - обновляем
	GetDSelectedRow();
	CString sCurrentWidgetBegin=sCurrentWidget;
	HIROW hAddElems=FL_ROOT;
	HIROW hClockElems=FL_ROOT;
	m_DList.LockWindowUpdate();
	if(iAtIndex==-1){
		BOOL bExpanded=0;
		if(sCurrentWidgetBegin.GetLength()){
			bExpanded=1;
		}
		m_DList.DeleteAllIRows();
		CMap<CString,const char*,HIROW,HIROW> aCategs;
		if(objSettings.aLoadedWPTs.GetSize()>0){
			for (int i=0;i<objSettings.aLoadedWPTs.GetSize();i++){
				HIROW hParent=FL_ROOT;
				CWPT* pWPT=GetWPT(i);
				if(!pWPT){
					continue;
				}
				if(pWPT->sCategs==CHIDDEN){
					continue;
				}
				if(IsVista() && pWPT->isAD()){
					continue;
				}
				HIROW hThis=0;
				CString sCateg=pWPT->sCategs;
				CString sTitle=pWPT->GetFTitle();
				if(sCateg!=""){
					if(!aCategs.Lookup(sCateg,hParent)){
						hParent=m_DList.InsertIRow(hParent, FL_LAST, _l(sCateg), 5);
						m_DList.SetIRowData(hParent,0xFFFFFFFF);
						m_DList.SetIItemControlType(hParent,0,FLC_READONLY);
						aCategs.SetAt(sCateg,hParent);
						if(sCateg==DEF_NOTESCATEG){
							hAddElems=hParent;
							m_DList.SetIItemImage(hParent,0,76);
						}else
						if(sCateg==DEF_GAMESCATEG){
							m_DList.SetIItemImage(hParent,0,67);
						}else
						if(sCateg==DEF_CLOCKCATEG){
							hClockElems=hParent;
							m_DList.SetIItemImage(hParent,0,75);
						}else
						if(sCateg==DEF_CALENCATEG){
							m_DList.SetIItemImage(hParent,0,80);
						}else
						if(sCateg.Find(DEF_USEFCATEG)!=-1){
							m_DList.SetIItemImage(hParent,0,23);
						}else
						if(sCateg==DEF_MODIFCATEG){
							m_DList.SetIItemImage(hParent,0,68);
						}else{
							m_DList.SetIItemImage(hParent,0,9);
						}
					}
				}
				hThis=m_DList.InsertIRow(hParent, FL_LAST, sTitle, -1);
				if(objSettings.aLoadedWPTs[i] == sCurrentWidgetBegin){
					hLastSelected=hThis;
				}
				m_DList.SetIRowData(hThis,i);
				m_DList.SetIItemControlType(hThis,0,FLC_CHECKBOX);
				m_DList.SetIItemCheck(hThis,0,IsWPTActive(objSettings.aLoadedWPTs[i]));
				if(pWPT->isAD()){
					m_DList.SetIItemImage(hThis,0,83);
				}else{
					m_DList.SetIItemImage(hThis,0,82);
				}
				if(hParent!=FL_ROOT){
					if(hLastSelected==hThis || !bExpanded){
						bExpanded=1;
						m_DList.Expand(hParent,0,0);
					}
				}
				pWPT->hIRow=hThis;
			}
		}
	}else{
		POSITION p=m_DList.GetStartPosition();
		while(p){
			HIROW hThis=m_DList.GetNextAssoc(p);
			if(hThis && long(m_DList.GetIRowData(hThis))==iAtIndex){
				m_DList.SetIRowData(hThis,iAtIndex);
				m_DList.SetIItemControlType(hThis,0,FLC_CHECKBOX);
				m_DList.SetIItemCheck(hThis,0,IsWPTActive(objSettings.aLoadedWPTs[iAtIndex]));
				m_DList.SetIItemText(hThis,0,GetWPT(objSettings.aLoadedWPTs[iAtIndex])->GetFTitle());
			}
		}
	}
	if(iAtIndex==-1){
		{// Заметки
			HIROW hThis=m_DList.InsertIRow(hAddElems, FL_FIRST, _l("Desktop notes"), -1);
			m_DList.SetIRowData(hThis,DEF_NOTESID);
			m_DList.SetIItemControlType(hThis,0,FLC_CHECKBOX);
			m_DList.SetIItemCheck(hThis,0,!objSettings.lDisableNotes,1,0);
			m_DList.SetIItemImage(hThis,0,6);
		}
		{// Download
			HIROW hThis=m_DList.InsertIRow(FL_ROOT, FL_LAST, _l("Download more widgets"), 54);
			m_DList.SetIRowData(hThis,DEF_DOWNLOADID);
			m_DList.SetIItemControlType(hThis,0,FLC_CUSTOMDIALOG2);
			m_DList.SetIItemColors(hThis, 0, RGB(0,0,255), -1);
			m_DList.SetIRowTextStyle(hThis, FL_UNLINE);
		}
		{// Download
			HIROW hThis=m_DList.InsertIRow(hClockElems, FL_LAST, _l("Download more clocks"), 54);
			m_DList.SetIRowData(hThis,DEF_DOWNLOADCID);
			m_DList.SetIItemControlType(hThis,0,FLC_CUSTOMDIALOG2);
			m_DList.SetIItemColors(hThis, 0, RGB(0,0,255), -1);
			m_DList.SetIRowTextStyle(hThis, FL_UNLINE);
		}
		{// Download
			HIROW hThis=m_DList.InsertIRow(FL_ROOT, FL_LAST, _l("Apply changes"), 54);
			m_DList.SetIRowData(hThis,DEF_APPLYID);
			m_DList.SetIItemControlType(hThis,0,FLC_CUSTOMDIALOG2);
			m_DList.SetIItemColors(hThis, 0, RGB(0,0,255), -1);
			m_DList.SetIRowTextStyle(hThis, FL_UNLINE);
		}
	}
	if(hLastSelected!=0 && hLastSelected!=FL_ROOT){
		m_DList.SelectIRow(hLastSelected,1);
	}
	hLastSelected=0;
	m_DList.UnlockWindowUpdate();
	UpdateThemeControls(-1);
	SetThemeNameInTitle(0);
}

void CDLG_WChanger::OnSetnow()
{
	CMenu MoreMenu;
	MoreMenu.CreatePopupMenu();
	BOOL bMulty=m_WList.IsSelectionMulty();
	AddMenuString(&MoreMenu,1,_l("Set wallpaper NOW")+"\tCtrl-W",_bmp().Get(IDB_BM_SETNOW));
	AddMenuString(&MoreMenu,IDC_BT_EDIT,_l("Edit image")+"\tCtrl-E",_bmp().Get(IDB_BM_EDIT));
	if(aThemesCount.GetSize()>1){
		CMenu MoveMenu;
		MoveMenu.CreatePopupMenu();
		for(int i=0;i<aThemesCount.GetSize();i++){
			AddMenuString(&MoveMenu,WM_USER+MOVE_TO_THEME+i,objSettings.m_WChanger.aWPapersAndThemes[aThemesCount[i]]->sTitle);
		}
		AddMenuSubmenu(&MoreMenu,&MoveMenu,(bMulty?_l("Move selection to"):_l("Move to"))+"\t...",_bmp().Get(_IL(79)));
	}
	::SetMenuDefaultItem(MoreMenu.GetSafeHmenu(), 0, TRUE);
	CRect rt;
	m_BtSetNow.GetWindowRect(&rt);
	CPoint pos(rt.right,rt.top);
	int iRes=::TrackPopupMenu(MoreMenu.GetSafeHmenu(), TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON, pos.x, pos.y, 0, this->GetSafeHwnd(), NULL);
	if(iRes>0){
		if(iRes==1){
			OnSetnowWallp();
		}else if(iRes==IDC_BT_EDIT){
			OnBtEdit();
		}else{
			pMainDialogWindow->PostMessage(WM_COMMAND,iRes,0);
		}
	}
}

void CDLG_WChanger::OnSetnowWallp()
{
	GetSelectedRow(0);
	if(iCurrentItem<0 || iCurrentItem>=objSettings.m_WChanger.aWPapersAndThemes.GetSize()){
		return;
	}
	SetLastWPSetted(iCurrentItem,0);
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iLastWPSetted];
	if(!wp){
		return;
	}
	if(wp->dwType==0){
		return;
	}
	FORK(SetWPInThreadXX,iLastWPSetted);
}

void CDLG_WChanger::OnEnlParams() 
{
	OpenAnimProps((HIROW)GetSafeHwnd(),NULL);
	bNeddRefreshAfterClose=TRUE;
}

BOOL CDLG_WChanger::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(wParam==WM_USER+REFRTEMPL){
		// Добавляем!
		RefreshTemplates(long(lParam));
		return TRUE;
	}
	if(wParam>=WM_USER+OPEN_NOTE && wParam<(DWORD)(WM_USER+OPEN_NOTE+USERMSG_STEP)){
		pMainDialogWindow->PostMessage(WM_COMMAND,wParam,lParam);
		return TRUE;
	}
	if(wParam>=WM_USER+ACTIVATE_PRESET && wParam<(DWORD)(WM_USER+ACTIVATE_PRESET+USERMSG_STEP)){
		pMainDialogWindow->PostMessage(WM_COMMAND,wParam,lParam);
		return TRUE;
	}
	if(wParam==ID_SYSTRAY_NEWDESKTOPNOTE || wParam==ID_SYSTRAY_DISABLENOTES || wParam==ID_SYSTRAY_NOTEOPTIONS){
		pMainDialogWindow->PostMessage(WM_COMMAND,wParam,lParam);
		return TRUE;
	}
	if(wParam>=WM_USER+PRESET_ON_WP && wParam<(DWORD)(WM_USER+PRESET_ON_WP+USERMSG_STEP)){
		int iNewPresetIndex=wParam-WM_USER-PRESET_ON_WP;
		CStringArray aPres;
		GetPresetList(aPres);
		GetSelectedRow();
		if(iCurrentItem==-1 || iNewPresetIndex<0 || iNewPresetIndex>=aPres.GetSize()){
			return TRUE;
		}
		BOOL bMulty=m_WList.IsSelectionMulty();
		if(!bMulty){
			WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem];
			if(wp){
				wp->SetWPar("PRESET",aPres[iNewPresetIndex]);
			}
		}else{
			int i=0;
			CHIROWArray& aSelected=m_WList.GetMultySelection();
			CDWordArray aSelectedIDs;
			for(i=0;i<aSelected.GetSize();i++){
				aSelectedIDs.Add(m_WList.GetIRowData(aSelected[i]));
			}
			for(i=0;i<aSelectedIDs.GetSize();i++){
				WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[aSelectedIDs[i]];
				if(wp){
					wp->SetWPar("PRESET",aPres[iNewPresetIndex]);
				}
			}
			m_WList.DropMultySelection();
		}
		return TRUE;
	}
	if(wParam>=WM_USER+MOVE_TO_THEME && wParam<(DWORD)(WM_USER+MOVE_TO_THEME+USERMSG_STEP)){
		if(lSortLock>0){
			//Идет сортировка, ничего не делаем
			Alert(_l("Sort in progress, please wait"));
			return TRUE;
		}
		int iNewThemeIndex=wParam-WM_USER-MOVE_TO_THEME;
		DWORD iNewThemeID=aThemesCount[iNewThemeIndex];
		GetSelectedRow();
		if(iCurrentItem==-1){
			return TRUE;
		}
		BOOL bMulty=m_WList.IsSelectionMulty();
		if(!bMulty){
			OnWchangerMoveToFolder(iCurrentItem,iNewThemeID);
		}else{
			int i=0;
			CHIROWArray& aSelected=m_WList.GetMultySelection();
			CDWordArray aSelectedIDs;
			for(i=0;i<aSelected.GetSize();i++){
				aSelectedIDs.Add(m_WList.GetIRowData(aSelected[i]));
			}
			for(i=0;i<aSelectedIDs.GetSize();i++){
				OnWchangerMoveToFolder(aSelectedIDs[i],iNewThemeID);
			}
			m_WList.DropMultySelection();
		}
		return TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDLG_WChanger::AddArchiveToArray(const char* sFile,CStringArray& aNames)
{
	CString sZipPath=sFile;
	CZipArchive zipFile;
	CString sPassword="";
	if(sZipPath.Find("\t")!=-1){
		sPassword=sZipPath.SpanExcluding("\t");
		sZipPath=sZipPath.Mid(sPassword.GetLength()+1);
	}
	if(!zipFile.Open(sFile)){
		SetInfoBlockAlert(_l("Bad archive, operation aborted")+"!");
		return;
	}
	if(sPassword!=""){
		zipFile.SetPassword(sPassword);
	}
	BOOL bNext=0;
	ZIP_FIND_DATA pFind;
	HANDLE hSearch=zipFile.FindFirstFile(VALID_EXTEN,&pFind);
	if(hSearch){
		do{
			aNames.Add(sZipPath+"\\"+pFind.szFileName);
			bNext=zipFile.FindNextFile(hSearch, &pFind);
		}while(bNext);
	}
	zipFile.FindClose(hSearch);
	zipFile.Close();
	return;
}

void CDLG_WChanger::OnWchangerAddArch()
{
	if(objSettings.isLight){
		AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
		return;
	}
	SimpleLocker lc(&objSettings.csWPInChange);
	CString sFile;
	//;*.wsc
	if(!OpenFileDialog("Image Archives|*.zip;*.wsc||",&sFile,0,FALSE,TRUE,this)){
		return;
	}
	if(!isFileExist(sFile)){
		return;
	}
	CString sZip=sFile;
	GetSelectedRow();
	CStringArray aNames;
	if(iCurrentItem!=-1){
		int iCounter=0;
		AddArchiveToArray(sFile,aNames);
		int dwTheme=iCurrentItem;
		WPaper* wp=GetWPTheme(dwTheme);
		if(wp){
			m_WList.SelectIRow(wp->hRow);
			m_WList.EnsureVisible(wp->hRow,0,TRUE);
		}
		WPaper* wpLast=0;
		for (int i=0;i<aNames.GetSize();i++){
			sFile=aNames[i];
			if(sFile!=""){
				if(!AddFile(sFile,dwTheme,iCounter>0?TRUE:FALSE,&wpLast)){
					break;
				}
				iCounter++;
			}
		}
		m_WList.UpdateIRow(wp->hRow);
		if(wpLast && wpLast->hRow){
			m_WList.EnsureVisible(wpLast->hRow,0,1);
		}
	}
	SetInfoBlockAlert(Format("%s: %lu (%s)",_l("Images found"),aNames.GetSize(),TrimMessage(sZip,30,3)));
}

void CDLG_WChanger::OnWchangerImportList()
{
	CString sFile;
	if(!OpenFileDialog("Any files (*.*)|*.*||",&sFile,0,FALSE,TRUE,this)){
		return;
	}
	if(!isFileExist(sFile)){
		return;
	}
	CString sOrigList=sFile;
	SimpleLocker lc(&objSettings.csWPInChange);
	CStringArray aNames;
	int iCounter=0;
	CString sLower=sFile;
	sLower.MakeLower();
	if(sLower.Find(".zip")!=-1 || sLower.Find(".wsz")!=-1){
		AddArchiveToArray(sFile,aNames);
	}
	if(aNames.GetSize()==0){
		CString sContent;
		ReadFile(sFile,sContent);
		CString sContentL=sContent;
		sContentL.MakeLower();
		int iPos=0;
		const char* szExtensions[]={".bmp",".tga",".tiff",".tif",".jpg",".jpeg",".gif",".html",".htm",0};
		while(iPos!=-1){
			int iIndex=0;
			iPos=GetNearestStr(sContentL,iPos,szExtensions,&iIndex);
			if(iPos!=-1){
				iPos+=strlen(szExtensions[iIndex]);
				int iBegin=ReverseFind(sContentL,":\\",iPos);
				if(iBegin==-1){
					iBegin=ReverseFind(sContentL,"://",iPos);
				}
				if(iBegin==-1){
					iBegin=ReverseFind(sContentL,":/",iPos);
				}
				if(iBegin!=-1){
					sFile=sContent.Mid(iBegin-1,iPos-(iBegin-1));
					if(isFileExist(sFile)){
						aNames.Add(sFile);
					}
				}
			}
		}
	}
	if(aNames.GetSize()==0){
		SetInfoBlockAlert(Format("%s: %s!",TrimMessage(sFile,30,3),_l("No images found")));
		return;
	}
	int iPos=AddThemeFolder(Format("%s #%lu",_l("Theme"),aThemesCount.GetSize()+1));
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iPos];
	if(wp){
		m_WList.SelectIRow(wp->hRow);
		m_WList.EnsureVisible(wp->hRow,0,TRUE);
	}
	for (int i=0;i<aNames.GetSize();i++){
		sFile=aNames[i];
		if(sFile!="" && isFileExist(sFile)){
			if(!AddFile(sFile,iPos,iCounter>0?TRUE:FALSE)){
				break;
			}
			iCounter++;
		}
	}
	m_WList.UpdateIRow(wp->hRow);
	SetInfoBlockAlert(Format("%s: %lu (%s)",_l("Images found"),iCounter,TrimMessage(sOrigList,30,3)));
}

void CDLG_WChanger::OnChangeAnim()
{
	objSettings.lEnliverON=!objSettings.lEnliverON;
	pMainDialogWindow->OrderModifierForNextWallpaper=-2;
	pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
}

CString sZipFile;
CStringArray aImagesToPack;
DWORD WINAPI CreateZip(LPVOID p)
{
	CDLG_WChanger* t=(CDLG_WChanger*)p;
	if(!t){
		return 0;
	}
	if(t->lZipLock>0){
		return 0;
	}
	SimpleTracker cl(t->lZipLock);
	CSmartLock SL(&t->cWDlgSection,TRUE);
	CCreateZipArchive zpBackup;
	if(zpBackup.Create(sZipFile,"",0)!=ZIPRES_OK){
		t->SetInfoBlockAlert(_l("Can`t create new archive! Operation aborted"));
		return 0;
	}
	int iCount=1;
	CStringArray aAdded;
	for(int i=0;i<aImagesToPack.GetSize();i++){
		CString sFileOnArch=aImagesToPack[i];
		if(IsFileInZip(sFileOnArch) || IsFileHTML(sFileOnArch) || !isFileExist(sFileOnArch)){
			continue;
		}
		sFileOnArch=GetPathPart(sFileOnArch,0,0,1,1);
		int iount=1;
		for(int j=0;j<aAdded.GetSize();j++){
			if(sFileOnArch==aAdded[j]){
				sFileOnArch=Format("%s_%02lu%s",GetPathPart(sFileOnArch,0,0,1,0),iCount++,GetPathPart(sFileOnArch,0,0,0,1));
			}
		}
		if(zpBackup.Add(aImagesToPack[i],sFileOnArch)!=ZIPRES_OK){
			t->SetInfoBlockAlert(_l("Can`t create add file to archive! Operation aborted"));
			break;
		}
		aAdded.Add(sFileOnArch);
		t->SetInfoBlock(Format("%s %i/%i: %s, %s",_l("Added"),aAdded.GetSize(),aImagesToPack.GetSize(),TrimMessage(aImagesToPack[i],50,3),_l("Press ESC to stop")),1);
		if(GetKeyState(VK_ESCAPE)<0){
			break;
		}
	}
	zpBackup.Close();
	if(aAdded.GetSize()==0){
		t->SetInfoBlockAlert(_l("There are no images to add! Operation aborted"));
		return 0;
	}
	t->SetInfoBlockAlert(_l("Archive successfully created! Images added")+Format(": %i",aAdded.GetSize()));
	return 0;
}

void CDLG_WChanger::OnPackIntoArch()
{
	if(objSettings.isLight){
		AlertBox(_l("Restriction"),_l(DEF_LITENOTICE),6000);
		return;
	}
	if(lSortLock>0){
		//Идет сортировка, ничего не делаем
		Alert(_l("Sort in progress, please wait"));
		return;
	}
	WPaper* wp=0;
	int iCount=0;
	CSmartLock SL(&cWDlgSection,TRUE);
	GetSelectedRow();
	if(iCurrentItem<0){
		return;
	}
	aImagesToPack.RemoveAll();
	wp=objSettings.m_WChanger.aWPapersAndThemes[iCurrentItem];
	if(!wp){
		return;
	}
	int dwTheme=iCurrentItem;
	WPaper* wpT=GetWPTheme(dwTheme);
	if(m_WList.IsSelectionMulty()){
		CHIROWArray& aSelected=m_WList.GetMultySelection();
		for(int i=0;i<aSelected.GetSize();i++){
			int dwData=m_WList.GetIRowData(aSelected[i]);
			if(dwData>=0 && dwData<objSettings.m_WChanger.aWPapersAndThemes.GetSize()){
				WPaper* wp2=objSettings.m_WChanger.aWPapersAndThemes[dwData];
				if(wp2->dwType==1){
					aImagesToPack.Add(wp2->sTitle);
				}
			}
			
		}
	}else if(wp->dwType==1){
		aImagesToPack.Add(wp->sTitle);
	}else if(wp->dwType==0){
		// Тема...
		for(int i=0;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
			if(!objSettings.m_WChanger.aWPapersAndThemes[i]){
				continue;
			}
			if(objSettings.m_WChanger.aWPapersAndThemes[i]->dwType==1 && int(objSettings.m_WChanger.aWPapersAndThemes[i]->dwTheme)==dwTheme){
				aImagesToPack.Add(objSettings.m_WChanger.aWPapersAndThemes[i]->sTitle);
			}
		}
	}
	if(aImagesToPack.GetSize()==0){
		SetInfoBlockAlert(_l("There are no images to add! Operation aborted"));
		return;
	}
	// Часть вторая
	sZipFile=wpT?wpT->sTitle:"Images";
	sZipFile+=".zip";
	if(!OpenFileDialog("Zip archive (*.zip)|*.zip||",&sZipFile,0,FALSE,TRUE,this,FALSE)){
		return;
	}
	if(isFileExist(sZipFile)){
		DWORD dwReply=AskYesNo(_l("Archive already exist! Ovewrite")+"?",_l("Confirmation"),0,this);
		if(dwReply==IDNO){
			return;
		}
		DeleteFile(sZipFile);
	}
	FORK(CreateZip,this);
	return;
}

void CDLG_WChanger::OnWchangerBackupAll()
{
	BackupAllSets();
}

void CDLG_WChanger::OnWchangerRestoreAll()
{
	RestoreAllSets();
}

void CDLG_WChanger::OnWchangerSetScr()
{
	SetScreensaver(1);
}

void CDLG_WChanger::On0Tab()
{
	m_MainTab.SetCurSel(0);
	OnSelchangeMaintab(0,0);
}

void CDLG_WChanger::On1Tab()
{
	m_MainTab.SetCurSel(1);
	OnSelchangeMaintab(0,0);
}

void CDLG_WChanger::OnWidgetSelChanged(NMHDR * pNotifyStruct, LRESULT * result)
{
	UpdateThemeControls(-1);
	//lbSkipIfPreviewNotAvailable=1;
	if(TrackSelChangeChangeInProgress){
		return;
	}
	FORK(TrackSelChange,this);
}

BOOL CDLG_WChanger::SingleSwitchTemplate(CString sTempl,BOOL bNew)
{
	BOOL bRes=0;
	BOOL bActive=IsWPTActive(sCurrentWidget);
	if(bActive!=bNew){
		bRes=SwitchSingleTemplate(sCurrentWidget,0);
		if(bRes){
			bNeddRefreshAfterClose=TRUE;
			FORK(TrackSelChange,this);
		}
	}
	return bRes;
}

void CDLG_WChanger::OnWidgetCTriggerChanged(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(pRowState==NULL){
		*result=-1;
		return;
	}
	*result = 0;
	int iCurID=m_DList.GetIRowData(pRowState->hIRow);
	if(iCurID==DEF_NOTESID){
		//Заметки
		objSettings.lDisableNotes=!BOOL(pRowState->dwNotifyData);
		bNeddRefreshAfterClose=TRUE;
		FORK(TrackSelChange,this);
		return;
	}
	if(iCurID>=0 && iCurID<objSettings.aLoadedWPTs.GetSize()){
		sCurrentWidget=objSettings.aLoadedWPTs[iCurID];
	}
	if(sCurrentWidget==""){
		return;
	}
	if(!SingleSwitchTemplate(sCurrentWidget,BOOL(pRowState->dwNotifyData))){
		*result=-1;
		return;
	}
}

void CDLG_WChanger::OnWidgetCustomDialog(NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(pRowState==NULL){
		*result=-1;
		return;
	}
	*result=0;//принять измененное значение
	long dwItemData=(long)m_DList.GetIRowData(pRowState->hIRow);
	if(dwItemData==DEF_DOWNLOADID){
		DownloadWPT(0,0);
	}
	if(dwItemData==DEF_DOWNLOADCID){
		DownloadWC(0,0);
	}
	if(dwItemData==DEF_APPLYID){
		bNeddRefreshAfterClose=0;
		OnSetnowWallp();
	}
}

void CDLG_WChanger::OnThActive()
{
	if(lWndMode){
		GetDSelectedRow();
		CWPT* wpt=GetWPT(sCurrentWidget);
		if(sCurrentWidget=="" || !wpt){
			return;
		}
		if(!SingleSwitchTemplate(sCurrentWidget,m_ThActive.GetCheck())){
			m_ThActive.SetCheck(0);
		}
	}else{
		if(wpNowShowingTheme){
			wpNowShowingTheme->SetStatus(m_ThActive.GetCheck());
			if(wpNowShowingTheme->hRow){
				m_WList.SetIItemCheck(wpNowShowingTheme->hRow, 0, wpNowShowingTheme->dwStatus);
			}
		}
	}
}


void CDLG_WChanger::OnSelchangeMaintab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iNewPos=m_MainTab.GetCurSel();
	lWndMode=iNewPos;
	BOOL bEnable=!lWndMode;
	if(lWndMode){
		bEnable=0;
		GetDlgItem(IDC_LIST_WPAPERS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_WIDGETS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_WIDGETS)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIST_WPAPERS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_WOPT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_WIDGETDROPDOWN)->ShowWindow(SW_SHOW);
		SetInfoBlock(_l("Installed widgets")+". "+_l("Right-click widget for more options"),1);
		m_tooltip.UpdateTipText(_l("Widget frequency (percents)")+"\n"+_l(sWeightDescW),GetDlgItem(IDC_SCR_WEIGHT));
		m_tooltip.UpdateTipText(_l("Widget frequency (percents)")+"\n"+_l(sWeightDescW),GetDlgItem(IDC_ACTIVE_THEME));
		GetDlgItem(IDC_ACTIVE_THEME)->SetWindowText(_l("Widget is in use"));
	}else{
		GetDlgItem(IDC_LIST_WPAPERS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_WIDGETS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_WIDGETS)->EnableWindow(FALSE);
		GetDlgItem(IDC_LIST_WPAPERS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_WOPT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_WIDGETDROPDOWN)->ShowWindow(SW_HIDE);
		SetInfoBlock(_l("Themes and Images")+". "+_l("Right-click File/Theme for more options"),1);
		m_tooltip.UpdateTipText(_l("Theme weight (percents)")+"\n"+_l(sWeightDesc),GetDlgItem(IDC_SCR_WEIGHT));
		m_tooltip.UpdateTipText(_l("Use this theme in the rotation of images"),GetDlgItem(IDC_ACTIVE_THEME));
		GetDlgItem(IDC_ACTIVE_THEME)->SetWindowText(_l("Theme is used in rotation"));
	}
	CRect rt;
	m_WList.GetClientRect(&rt);
	objSettings.iColWChWidth=rt.Width()-1;
	m_WList.SetColumnWidth(0,objSettings.iColWChWidth);
	m_DList.GetClientRect(&rt);
	m_DList.SetColumnWidth(0,rt.Width()-1);
	//GetDlgItem(IDC_BT_REM_IMG)->EnableWindow(bEnable);
	//GetDlgItem(IDC_SETNOW)->EnableWindow(bEnable);
	//GetDlgItem(IDC_ACTIVE_THEME)->ShowWindow(bEnable);
	//GetDlgItem(IDC_STAT_THEM)->ShowWindow(bEnable);
	//GetDlgItem(IDC_STAT_WEIGHT)->ShowWindow(bEnable);
	//GetDlgItem(IDC_STAT_WEIGHT_VAL)->ShowWindow(bEnable);
	//GetDlgItem(IDC_SCR_WEIGHT)->ShowWindow(bEnable);
#ifndef ART_VERSION
	GetDlgItem(IDC_STAT_DMODE)->ShowWindow(bEnable);
	GetDlgItem(IDC_COMBO_THEMEFITTING)->ShowWindow(bEnable);
#endif
	UpdateThemeControls(-1);
	if(pResult){
		// Рядовое переключение - обновляем
		FORK(TrackSelChange,this);
		*pResult = 0;
	}
}

void CDLG_WChanger::OnBtWoptDDown() 
{
	OnRclickDlist(1);
}

void CDLG_WChanger::OnBtWopt() 
{
	if(lWndMode){
		GetDSelectedRow();
		CWPT* wpt=GetWPT(sCurrentWidget);
		if(sCurrentWidget=="" || !wpt){
			return;
		}
		int iActiveWpt=GetTemplateMainIndex(sCurrentWidget);
		pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+CHANGE_TEMPL_PAR+iActiveWpt,255);
	}
}

void CDLG_WChanger::OnWchangerPresetHlp()
{
	ShowHelp("dsc_presets");
}

void CDLG_WChanger::OnWchangerDelPreset()
{
	if(objSettings.sActivePreset!=""){
		CString sPresetFolder=GetPresetsFolder(objSettings.sActivePreset);
		SHFILEOPSTRUCT str;
		memset(&str,0,sizeof(str));
		str.hwnd=this->GetSafeHwnd();
		str.wFunc=FO_DELETE;
		char szPath[MAX_PATH+2]="";
		memset(&szPath,0,sizeof(szPath));
		CString sMask=(sPresetFolder+"*.*");
		strcpy(szPath,sMask);
		str.pFrom=szPath;
		str.pTo=NULL;
		str.fFlags=FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT;
		if(SHFileOperation(&str)==0){
			RemoveDirectory(sPresetFolder);
			objSettings.sActivePreset="";
			objSettings.LoadActiveWPT();
			pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+ACTIVATE_PRESET+0,0);
			RefreshNotesMenu();
		}
	}
}

void CDLG_WChanger::OnWchangerNewPreset()
{
	CString sName=GetRandomName();
	CDLG_AddLink dlg(this);
	dlg.iType=1;
	dlg.pLink=&sName;
	dlg.DoModal();
	if(sName==""){
		Alert(_l("Enter preset name first"));
		return;
	}
	CString sName0=sName;
	MakeSafeFileName(sName0);
	if(sName0!=sName){
		Alert(_l("Preset name is invalid"));
		return;
	}
	CString sOldPreset=objSettings.sActivePreset;
	// Принудительно сохраняем!!! чтобы обновился список
	objSettings.SaveActiveWPT();//со старым
	objSettings.sActivePreset=sName;
	objSettings.SaveActiveWPT();// с новым
	// Копируем текущие настройки
	CFileInfoArray dir;
	CString sPresetFolder=GetPresetsFolder(objSettings.sActivePreset);
	dir.AddDir(GetPresetsFolder(sOldPreset),"*.cfg",FALSE,0,FALSE);
	for (int i2=0;i2<dir.GetSize();i2++){
		CString sFile=dir[i2].GetFilePath();
		if(sFile.Find("actives")==-1){
			CopyFile(sFile,sPresetFolder+GetPathPart(sFile,0,0,1,1),FALSE);
		}
	}
	CStringArray aPres;
	GetPresetList(aPres);
	for(int i=0;i<aPres.GetSize();i++){
		if(aPres[i]==objSettings.sActivePreset){
			pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+ACTIVATE_PRESET+i,0);
			RefreshNotesMenu();
		}
	}
}

void CDLG_WChanger::OnBtCal()
{
	pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN2,0);
}

void CDLG_WChanger::OnBtCalR()
{

}


BOOL IsHelpEnabled2()
{
	static BOOL b=-1;
	if(b==-1){
		b=0;
		CString sFile=Format("%s%s_l%i.chm", GetApplicationDir(), GetApplicationName(), 0);
		if(isFileExist(sFile)){
			b=1;
		}
	}
	return b;
}