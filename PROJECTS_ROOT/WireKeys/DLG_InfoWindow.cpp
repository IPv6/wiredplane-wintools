// DLG_InfoWindow.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "dlg_infowindow.h"
CString Format(const char* szFormat,...);
void ShowBaloon(const char* szText, const char* szTitle, DWORD dwTimeout, BOOL bBaloonOnly);
#define THISIWHK_HOTKEY_NUM	1
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CQuickRunItem* FindQRun(HWND hWind, int* iPos=NULL)
{
	long lQRunID=(long)hWind;
	if(lQRunID<0){
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){
			if(objSettings.aQuickRunApps[i] && (-lQRunID==objSettings.aQuickRunApps[i]->lUniqueCount)){
				if(iPos){
					*iPos=i;
				}
				return objSettings.aQuickRunApps[i];
			}
		}
		if(iPos){
			*iPos=-1;// Элемент удалили!!!
		}
	}
	return 0;
}

CCriticalSection csInfoWnds;
long InfoWndStruct::dwIDCounter=1;
CMenu* _stdcall RefreshInfoMenuEx(InfoWnd* pInfo,BOOL bShowOnlyExpandedDir, BOOL bNotShowExpandedDir)
{
	if(pInfo->lInMenuLock>0){
		return NULL;
	}
	SimpleTracker tr(pInfo->lInMenuLock);
	CMenu* m_pMenu=new CMenu();
	m_pMenu->CreatePopupMenu();
	pInfo->m_STray.SetTooltipText(pInfo->GetTitle()+"\tShit-"+_l("System menu"));
	CString sIconName=_IL(38);
	int iPos=0;
	CQuickRunItem* pQRun=FindQRun(pInfo->Sets->hWnd,&iPos);
	CString sTargetFile;// Путь к файлу (если нужен)
	if(pInfo->Sets && pInfo->Sets->hWnd!=NULL){
		if(pQRun){
			sTargetFile=pQRun->sItemPath;
			SubstMyVariablesForQRun(sTargetFile,NULL,FALSE,TRUE);
			if(isFileExist(sTargetFile)){
				sIconName=GetExeIconIndex(sTargetFile,TRUE);
			}else{
				sIconName=_IL(wk.GetIconNumByMacro(pQRun->lMacros));
			}
		}else{
			sIconName=Format("HIDDEN_ICON%x08",pInfo->Sets->hWnd);
			if(_bmp().Get(sIconName)==NULL){
				_bmp().AddBmpRaw(sIconName,pInfo->Sets->hIcon);
			}
		}
	}
	CString sActtext;
	if(pInfo->Sets->fHK.Present()){
		sActtext+="\t";
		sActtext+=getHotKeyStr(pInfo->Sets->fHK);
	}else if(pQRun && pQRun->qrHotkeys[0]->oHotKey.Present()){
		sActtext+="\t";
		sActtext+=getHotKeyStr(pQRun->qrHotkeys[0]->oHotKey);
	}
	AddMenuString(m_pMenu,ID_INFOWND_OPENPR,TrimMessage(pInfo->GetTitle(),20)+sActtext,_bmp().GetRaw(sIconName));
	if(pInfo->Sets->hWnd==NULL){
		AddMenuString(m_pMenu,ID_INFOWND_DELETE,_l("Close"));
	}else if(pQRun){
		AddMenuString(m_pMenu,ID_INFOWND_EDITQ,_l("Edit quick-run item"),_bmp().Get(_IL(93)));
		CString sFolderToExpand="",sMask="";
		if(pQRun->lMacros<CATEG_MACR){
			if(pQRun->lParamType==2){
				if (pQRun->sStartupDir!=""){
					sFolderToExpand=pQRun->sStartupDir;
				}else{
					sFolderToExpand=sTargetFile;
				}
				sMask=pQRun->sAutoExpandMask;
			}else if (pQRun->sStartupDir!=""){
				sFolderToExpand=pQRun->sStartupDir;
			}
			if(sFolderToExpand!="" && !bNotShowExpandedDir){
				AddSlashToPath(sFolderToExpand);
				pInfo->aExpandedFolderPaths.RemoveAll();
				m_pMenu->InsertMenu(0,MF_BYPOSITION|MF_STRING,0,Format("%s%s%s%s",EXPAND_FOLDER_ID,sFolderToExpand,EXPAND_FOLDER_MASK,sMask));
			}
		}
		if(!bShowOnlyExpandedDir){
			if(pQRun->lMacros==CATEG_MACR){
				AppendQRunSingleToMenu(0,m_pMenu, ANY_MACRO, NULL, pQRun->sUniqueID);
				m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
			}
			if(pInfo->Sets->bDetached==0){
				AddMenuString(m_pMenu,ID_INFOWND_DETACH,_l("Detach from tray"),_bmp().Get(_IL(54)));
				AddMenuString(m_pMenu,ID_INFOWND_DELETE,_l("Remove from tray"),_bmp().Get(_IL(37)));
			}
			if(pInfo->Sets->bDetached==1){
				AddMenuString(m_pMenu,ID_INFOWND_DETACH,_l("Attach to tray"),_bmp().Get(_IL(54)));
				AddMenuString(m_pMenu,ID_INFOWND_DELETE,_l("Close floater"),_bmp().Get(_IL(37)));
			}
			if(sFolderToExpand!="" && (!bNotShowExpandedDir || bShowOnlyExpandedDir)){
				m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
			}
		}
	}else if(!bShowOnlyExpandedDir){
		if(objSettings.bHidedAppShowMenu>0){
			HMENU pWndMenu=NULL;
			BOOL bOwnerDraws=0;
			if(!objSettings.bSupportOwnerDraw){
				bOwnerDraws=-1;
			}
			if(GetAsyncKeyState(VK_SHIFT)<0){
				pInfo->Sets->bLastOpenedTrayMenuType=(objSettings.bHidedAppShowMenu==1)?1:0;
			}else{
				pInfo->Sets->bLastOpenedTrayMenuType=(objSettings.bHidedAppShowMenu==1)?0:1;
			}
			if(pInfo->Sets->bLastOpenedTrayMenuType){
				pWndMenu=::GetSystemMenu(pInfo->Sets->hWnd,FALSE);
			}else{
				pWndMenu=::GetMenu(pInfo->Sets->hWnd);
				{//Пункты быстрого доступа
					if(pInfo->aLastCommandsCodes.GetSize()>0){
						if(pInfo->aM2I.GetSize()==0 && IsMenu(pWndMenu)){
							// Делаем предварительный проход
							CMenu MenuTmp;
							MenuTmp.CreatePopupMenu();
							AppendClonedMenu(&MenuTmp,pWndMenu,WM_USER+INFOWNDOFFSET,&bOwnerDraws,&pInfo->aM2I);
						}
						m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
						for(int i=0;i<pInfo->aLastCommandsCodes.GetSize();i++){
							int iNum=FindIDNumInNum2IDMap(pInfo->aM2I,pInfo->aLastCommandsCodes[i]);
							if(iNum!=-1){
								DWORD dwCode=WM_USER+INFOWNDOFFSET+iNum;
								AddMenuString(m_pMenu,dwCode,pInfo->aLastCommandsNames[i]);
								SetMenuItemInfo(*m_pMenu,dwCode,FALSE,&pInfo->aLastCommandsInfos[i]);
							}
						}
						AddMenuString(m_pMenu,ID_INFOWND_DROPRECENTLIST,_l("Remove recent items"),_bmp().Get(_IL(37)));
					}
				}
			}
			int iCount=0;
			if(IsMenu(pWndMenu)){
				pInfo->aM2I.RemoveAll();
				m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
				iCount=AppendClonedMenu(m_pMenu,pWndMenu,WM_USER+INFOWNDOFFSET,&bOwnerDraws,&pInfo->aM2I);
			}
			DWORD dwMenuFlags=0;
			if(bOwnerDraws){// При включенной анимации OwnerDraw menu не показываются нормально :(((
				dwMenuFlags|=0x4000L;//TPM_NOANIMATION(0x4000L)|TPM_RECURSE(0x0001L)
			}
			pInfo->m_STray.SetMenuFlags(dwMenuFlags);
			if(iCount!=0){
				m_pMenu->AppendMenu(MF_SEPARATOR, 0, "");
			}
		}
		if(pInfo->Sets->bDetached==0){
			AddMenuString(m_pMenu,ID_INFOWND_DETACH,_l("Detach from tray"),_bmp().Get(_IL(54)));
		}
		if(pInfo->Sets->bDetached==1){
			AddMenuString(m_pMenu,ID_INFOWND_DETACH,_l("Attach to tray"),_bmp().Get(_IL(54)));
		}
		if(pInfo->Sets->fHK.Present()){
			AddMenuString(m_pMenu,ID_INFOWND_SETHK,_l("Change activation hotkey"),_bmp().Get(_IL(55)));
		}else{
			AddMenuString(m_pMenu,ID_INFOWND_SETHK,_l("Set activation hotkey"),_bmp().Get(_IL(55)));
		}
		if(pInfo->Sets->bLastOpenedTrayMenuType==0){
			AddMenuString(m_pMenu,ID_INFOWND_CLSEPR,_l("Close application"),_bmp().Get(_IL(40)));
		}
		AddMenuString(m_pMenu,ID_INFOWND_TERM,_l("Terminate application"),_bmp().Get(IDB_BM_DEL));
	}
	return m_pMenu;
}

CMenu* _stdcall RefreshInfoMenu(void* pData)
{
	InfoWnd* pInfo=(InfoWnd*)pData;
	if(!pInfo){
		return NULL;
	}
	return RefreshInfoMenuEx(pInfo,FALSE,FALSE);
}

InfoWndStruct::InfoWndStruct()
{
	dwID=dwIDCounter;
	bForceMinType=0;
	bLastOpenedTrayMenuType=0;
	InterlockedIncrement(&dwIDCounter);
	pParent=NULL;
	bAsModal=FALSE;
	dwDontAskHolder=NULL;
	sText="";
	sTitle=_l(PROGNAME": Warning")+"!";
	sOkButtonText=_l("OK");
	sCancelButtonText="";
	sAskAgainButtonText=_l("Don`t show again");
	iIcon=IDI_ICON_INFO_AST;
	dwTimer=0;
	bStartHided=0;
	bCenterUnderMouse=TRUE;
	iStyle=0;
	hWnd=NULL;
	dwStaticPars=DT_LEFT|DT_WORDBREAK;
	bDetached=0;
	bmScreenshot=NULL;
	hIcon=NULL;
	hIconBig=NULL;
	CPoint pt;
	::GetCursorPos(&pt);
	xPos=pt.x;
	yPos=pt.y;
};

InfoWndStruct::~InfoWndStruct()
{
	if(hIcon){
		ClearIcon(hIcon);
	}
	if(hIconBig){
		ClearIcon(hIconBig);
	}
	if(bmScreenshot){
		delete bmScreenshot;
		bmScreenshot=NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInfoDsc
IMPLEMENT_DYNAMIC(CInfoDsc, CButton)
CInfoDsc::CInfoDsc(InfoWnd* pDialog)
{
	m_pDialog=pDialog;
	bFirstTime=1;
}

CInfoDsc::~CInfoDsc()
{
}


BEGIN_MESSAGE_MAP(CInfoDsc, CButton)
END_MESSAGE_MAP()

void CInfoDsc::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(bFirstTime){
		bFirstTime=0;
		m_pDialog->RedrawWindow(0,0);
	}
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect boundsEx,bounds;
	GetClientRect(&boundsEx);
	boundsEx.DeflateRect(2,2);
	if(m_pDialog->Sets->hWnd!=NULL){
		if(pDC){
			pDC->Draw3dRect(&boundsEx,RGB(100,100,100),RGB(100,100,100));
			boundsEx.DeflateRect(1,1);
			if(m_pDialog->Sets->bmScreenshot){
				CDC dc;
				dc.CreateCompatibleDC(pDC);
				CBitmap* bmOld=dc.SelectObject(m_pDialog->Sets->bmScreenshot);
				CSize scrshotSize=GetBitmapSize(m_pDialog->Sets->bmScreenshot);
				if(scrshotSize.cx>scrshotSize.cy){
					boundsEx.top+=(boundsEx.Height()-scrshotSize.cy)/2;
				}else{
					boundsEx.left+=(boundsEx.Width()-scrshotSize.cx)/2;
				}
				pDC->BitBlt(boundsEx.left,boundsEx.top,boundsEx.Width(),boundsEx.Height(),&dc,0,0,SRCCOPY);
				dc.SelectObject(bmOld);
			}else{
				::DrawIconEx(*pDC,boundsEx.left,boundsEx.top,m_pDialog->Sets->hIconBig?m_pDialog->Sets->hIconBig:m_pDialog->Sets->hIcon,boundsEx.Width(),boundsEx.Height(),0,NULL,DI_NORMAL);
			}
		}
	}else{
		bounds=boundsEx;
		bounds.DeflateRect(4,4);
		if(pDC){
			pDC->Draw3dRect(&boundsEx,RGB(100,100,100),RGB(100,100,100));
			SmartDrawText(*pDC,m_pDialog->Sets->sText,bounds,NULL,TXCOLOR,m_pDialog->Sets->hWnd?objSettings.lFloaterBG:BGCOLOR,TXSHADOW_FILLBG,m_pDialog->Sets->dwStaticPars);//TXSHADOW_SIMPLE|TXSHADOW_MIXBG
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// InfoWnd dialog
void InfoWnd::Init(InfoWndStruct* pInfo)
{
	hTitlePooler=0;
	hTitlePoolerEvent=0;
	Sets=pInfo;
	lMoveDist=0;
	lDoNotPerformAnything=0;
	bDetachedFromSaveValid=TRUE;
	bDetachedFromSave=0;
	hDDEWnd=NULL;
	fromPoint.x=fromPoint.y=0;
	lInMenuLock=0;
	isMove=0;
	dwTimer=0;
	RunTimer(pInfo->dwTimer);
	m_hBr = NULL;
	callback=NULL;
	bOnTaskBar=FALSE;
	bCallbackInProgress=FALSE;
	dwLastMovementTime=0;
	bExpandableDirectory=0;
	bOpenSlayerANDSwitch=FALSE;
	dwPrevTime=0;
	if(FindQRun(Sets->hWnd)==NULL && Sets->hWnd!=NULL){
		// Если это 
		CSmartLock SL(&csInfoWnds,TRUE);
		iHiddenWndNumber=objSettings.aHidedWindowsMap.Add(this);
	}
}

InfoWnd::InfoWnd(InfoWndStruct* pInfo, CWnd* pParent)
	: CDialog(IDD, pParent), m_Text(this)
{
	Init(pInfo);
	Sets->bAsModal=TRUE;
}

InfoWnd::InfoWnd(InfoWndStruct* pInfo)
	: CDialog(), m_Text(this)
{
	//{{AFX_DATA_INIT(InfoWnd)
	//}}AFX_DATA_INIT
	Init(pInfo);
	Sets->bAsModal=FALSE;
}

InfoWnd::~InfoWnd()
{
	if(hTitlePooler){
		HANDLE hPoolerCopy=hTitlePoolerEvent;
		hTitlePoolerEvent=0;
		SetEvent(hPoolerCopy);
		WaitForSingleObject(hTitlePooler,500);
		CloseHandle(hTitlePooler);
		CloseHandle(hPoolerCopy);
	}
	CSmartLock SL(&csInfoWnds,TRUE);
	for(int i=0; i<objSettings.aHidedWindowsMap.GetSize();i++){
		InfoWnd* pW=objSettings.aHidedWindowsMap[i];
		if(pW){
			if(pW->Sets->dwID==Sets->dwID){
				objSettings.aHidedWindowsMap.RemoveAt(i);
				break;
			}
		}
	}
	delete Sets;
	deleteBrush(m_hBr);
}

void InfoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InfoWnd)
	DDX_Control(pDX, IDMINIMIZE, m_BtMinim);
	DDX_Control(pDX, IDC_ASKAGAIN, m_AskAgain);
	DDX_Control(pDX, IDC_TITLE, m_Title);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_WICON, m_Icon);
	DDX_Control(pDX, IDC_TEXT, m_Text);
	DDX_Control(pDX, IDOK, m_OkButton);
	//}}AFX_DATA_MAP
}

void InfoWnd::SetTextParams(DWORD dwFlags)
{
	Sets->dwStaticPars=dwFlags;
}

BEGIN_MESSAGE_MAP(InfoWnd, CDialog)
	//{{AFX_MSG_MAP(InfoWnd)
	ON_WM_TIMER()
	ON_COMMAND(ID_INFOWND_DELETE, OnInfowndDelete)
	ON_COMMAND(ID_INFOWND_OPEN, OnInfowndOpen)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_ASKAGAIN, OnAskagain)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDMINIMIZE, OnBnClickedMinimize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InfoWnd message handlers

void InfoWnd::GetInfoWndSize(CSize& size)
{
	if(Sets->hWnd!=NULL){
		size.cx=objSettings.sDetachedSize;
		size.cy=objSettings.sDetachedSize;
		return;
	}
	size.cx=200;
	size.cy=100;
	CDC* pDC=m_Text.GetDC();
	if(pDC){
		int iLines=0;
		CFont* pParentFont=GetFont();
		CFont* pPrev=pDC->SelectObject(pParentFont);
		size=GetLargestLineSize(pDC,Sets->sText,iLines);
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->LPtoDP(&size);
		pDC->SelectObject(pPrev);
		size.cx=size.cx+15;
		size.cy=5+size.cy*(iLines+((Sets->dwDontAskHolder!=0)?0:-1));
		ReleaseDC(pDC);
	}
	size.cx+=CtrlWidth(this,0)+20;
	size.cy+=CtrlHeight(this,0)+CtrlHeight(this,IDC_WICON)+20;
	if(Sets->sOkButtonText!=""){
		size.cy+=CtrlHeight(this,IDOK);
	}
	if(Sets->dwDontAskHolder!=NULL){
		if(size.cx<150){
			size.cx=150;
		}
	}else if(size.cx<150){
		size.cx=150;
	}
	if(size.cy<70){
		size.cy=70;
	}
	AdoptSizeToScreen(size);
	return;
}

CString InfoWnd::GetTitle()
{
	CQuickRunItem* pQRun=FindQRun(Sets->hWnd);
	if(Sets && Sets->hWnd && pQRun==NULL && ::IsWindow(Sets->hWnd)){
		CString sRes;
		SafeGetWindowText(Sets->hWnd,sRes);
		return sRes;
	}
	if(pQRun){
		return pQRun->sItemName;
	}
	return Sets->sTitle;
}

void InfoWnd::GetInfoIcon()
{
	m_Icon.SetStyles(CBS_FLAT);
	ClearIcon(Sets->hIcon);
	if(Sets->iIcon>0){
		Sets->hIcon=::AfxGetApp()->LoadIcon(Sets->iIcon);
		m_Icon.SetBitmap(Sets->hIcon);
	}else{
		Sets->hIcon=theApp.MainImageList.ExtractIcon(-int(Sets->iIcon));
		m_Icon.SetBitmap(AfxGetApp()->LoadIcon(IDI_ICON_BG),CBS_BACKG_N);
		m_Icon.SetStyles(CBS_FLAT | CBS_BACKG);
		CBitmap* pBMP=_bmp().Get(_IL(-Sets->iIcon));
		m_Icon.SetBitmap(pBMP,CBS_NORML_N,50,0,-10);
	}
}

DWORD WINAPI PoolWnd(LPVOID pData)
{
	InfoWnd* pWnd=(InfoWnd*)pData;
	long dwSleepTime=5000;
	HWND hWin=pWnd->Sets->hWnd;
	HWND hWinO=pWnd->GetSafeHwnd();
	HANDLE hTitlePoolerEvent=pWnd->hTitlePoolerEvent;
	char szTitle1[256]={0};
	::GetWindowText(hWin,szTitle1,sizeof(szTitle1));
	while(1){
		if(!(IsWindow(hWinO) && WaitForSingleObject(hTitlePoolerEvent,dwSleepTime)!=WAIT_OBJECT_0 && IsWindow(hWin))){
			break;
		}
		char szTitle2[256]={0};
		::GetWindowText(hWin,szTitle2,sizeof(szTitle2));
		if(IsWindow(hWinO) && strcmp(szTitle1,szTitle2)!=0){
			if(dwSleepTime>1000){
				dwSleepTime=dwSleepTime/2;
			}else{
				dwSleepTime=1000;
			}
			{// В залоченном виде устанавливаем параметры. Если не идет удаление?
				//CSmartLock SL(&csInfoWnds,TRUE);-нельзя тут ее лочить! использует в WindProc!!! Deadlock!!!
				if(!IsWindow(hWinO)){
					// А мы то удаляемся!!!
					break;
				}
				//pWnd->SetParams(0,szTitle2,0);
				PostMessage(hWinO,WM_COMMAND,ID_INFOWND_SET_TITL,(LPARAM)strdup(szTitle2));
			}
		}
		strcpy(szTitle1,szTitle2);
	}
	return 0;
}

#define INFOBUTT_WIDTH	60
long lSingleClick = 1;
BOOL InfoWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);//CS_DROPSHADOW
	bOnTaskBar=(objSettings.lAlertsOnTaskbar && (Sets->hWnd==NULL));
	Sizer.InitSizer(this,objSettings.iStickPix,Sets->bCenterUnderMouse?0:HKEY_CURRENT_USER,Sets->bCenterUnderMouse?NULL:PROG_REGKEY"\\Wnd_Info");
	if(Sets->hWnd){// Вытаскиваем до правил выравнивания - дабы считать позицию
		CQuickRunItem* pQRun=FindQRun(Sets->hWnd);
		if(pQRun){
			CString sIconName=_IL(38);
			CString sTargetFile=pQRun->sItemPath;
			SubstMyVariablesForQRun(sTargetFile,NULL,FALSE,TRUE);
			if(isFileExist(sTargetFile)){
				sIconName=GetExeIconIndex(sTargetFile,TRUE);
			}else{
				sIconName=_IL(wk.GetIconNumByMacro(pQRun->lMacros));
			}
			Sets->hIcon=_bmp().CloneAsIcon(sIconName);
			if(pQRun->lParamType==2 && pQRun->lMacros<CATEG_MACR){
				bExpandableDirectory=TRUE;
			}
		}else if(IsWindow(Sets->hWnd)){
			GetWndIcon(Sets->hWnd,Sets->hIcon,&Sets->hIconBig);
			if(objSettings.bOverlayTrayAppIcon){
				HICON hIconO=theApp.MainImageList.ExtractIcon((Sets->bStartHided==3)?85:84);
				HICON hIcon2=PutOverlayInIcon(Sets->hIcon,hIconO);
				ClearIcon(hIconO);
				ClearIcon(Sets->hIcon);
				Sets->hIcon=hIcon2;
			}
			DWORD dwId=0;
			hTitlePoolerEvent=CreateEvent(0,0,0,0);
			hTitlePooler=CreateThread(0,0,PoolWnd,this,0,&dwId);
			/*IDropTarget * pDropTarget=NULL;
			Pager_GetDropTarget(Sets->hWnd, &pDropTarget);
			//CComPtr<IUnknown> ptr=CWnd::FromHandle(Sets->hWnd)->GetControlUnknown();
			if(pDropTarget!=NULL){
			 R egisterDragDrop(*this,pDropTarget);
			}
			HRESULT TxGetDropTarget(IDropTarget **ppDropTarget);*/
		}
		LoadMenuItemsForFile();
		Sets->sTitle=GetTitle();
	}
	if(!Sets->hIcon){
		GetInfoIcon();
	}
	if(bOnTaskBar && Sets->bDetached==0){
		SetTaskbarButton(this->GetSafeHwnd());
	}else{
		::SetWindowLong(this->GetSafeHwnd(),GWL_STYLE,WS_THICKFRAME|(::GetWindowLong(this->GetSafeHwnd(),GWL_STYLE)&~WS_OVERLAPPEDWINDOW));
	}
	// Центруем под мышой
	if(Sets->bCenterUnderMouse && !Sets->hWnd){
		Sizer.SetOptions(STICKABLE|SIZEABLE|CENTERUNDERMOUSE);
	}else{
		Sizer.SetOptions(STICKABLE|SIZEABLE);
	}
	SetButtonSize(this,IDCANCEL,CSize(INFOBUTT_WIDTH,20));
	Sizer.AddDialogElement(IDCANCEL,heightSize);
	Sizer.AddDialogElement(IDOK,heightSize,IDCANCEL);
	Sizer.AddDialogElement(IDC_ASKAGAIN,heightSize,IDCANCEL);
	Sizer.AddDialogElement(IDC_TITLE,heightSize);
	Sizer.AddDialogElement(IDC_WICON,widthSize|heightSize|fixedDimensions,20);
	Sizer.AddDialogElement(IDMINIMIZE,widthSize|heightSize|fixedDimensions,20);
	Sizer.AddDialogElement(IDC_TEXT,0);
	// Правила выравнивания - для проиложения или в целом
	if(Sets->hWnd!=NULL){
		ModifyStyle(WS_SIZEBOX,0);
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ASKAGAIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_WICON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDMINIMIZE)->ShowWindow(SW_HIDE);
		Sizer.SetOptions(SIZEABLE|STICKABLE);
		Sizer.AddLeftTopAlign(IDC_TEXT,0,0);
		Sizer.AddRightBottomAlign(IDC_TEXT,0,0);
		CRect rt(Sets->xPos,Sets->yPos,Sets->xPos+objSettings.sDetachedSize,Sets->yPos+objSettings.sDetachedSize);
		Sizer.SetStartupPos(rt);
		
		if(objSettings.bAnimateFloaters){
			WINDOWPLACEMENT pl;
			pl.length=sizeof(WINDOWPLACEMENT);
			::GetWindowPlacement(Sets->hWnd,&pl);
			CRect rtBegin;
			if(pl.showCmd==SW_SHOWMINIMIZED){
				::GetWindowRect(::FindWindow("Shell_TrayWnd",0),&rtBegin);
			}else{
				::GetWindowRect(Sets->hWnd,&rtBegin);
			}
			CRect rtEnd;
			if(!Sets->bDetached){
				HWND hTray=::FindWindow("Shell_TrayWnd",0);
				::GetWindowRect(::FindWindowEx(hTray,0,"TrayNotifyWnd",NULL),&rtEnd);
			}else{
				rtEnd.CopyRect(&rt);
			}
			DrawAnimateRect(rtBegin,rtEnd);
		}		
		
	}else{
		// Основной вид алерта
		Sizer.AddGotoLabel(998);
		Sizer.AddLeftAlign(IDC_WICON,0,0);
		Sizer.AddRightAlign(IDMINIMIZE,0,0);
		if(bOnTaskBar){
			Sizer.AddBottomAlign(IDC_TITLE,0,topPos,0);
			Sizer.AddBottomAlign(IDC_WICON,0,topPos,0);
			Sizer.AddBottomAlign(IDMINIMIZE,0,topPos,0);
			Sizer.AddHideRule(IDC_TITLE);
			Sizer.AddHideRule(IDC_WICON);
			Sizer.AddHideRule(IDMINIMIZE);
		}else{
			Sizer.AddTopAlign(IDC_TITLE,0,0);
			Sizer.AddTopAlign(IDC_WICON,0,0);
			Sizer.AddTopAlign(IDMINIMIZE,0,0);
		}
		Sizer.AddRightAlign(IDC_TITLE,IDMINIMIZE,leftPos,-4);
		Sizer.AddLeftAlign(IDC_TITLE,IDC_WICON,rightPos,4);
		Sizer.AddTopAlign(IDC_TEXT,IDC_WICON,bottomPos,2);
		Sizer.AddLeftAlign(IDC_TEXT,0,2);
		Sizer.AddRightAlign(IDC_TEXT,0,-2);
		MainRulePos=Sizer.AddGotoRule(0);
		Sizer.AddGotoLabel(0);
		Sizer.AddBottomAlign(IDOK,0,-4);
		Sizer.AddShowRule(IDOK);
		Sizer.AddGotoRule(100);
		Sizer.AddGotoLabel(1);
		Sizer.AddTopAlign(IDOK,0,bottomPos,0);
		Sizer.AddHideRule(IDOK);
		Sizer.AddGotoRule(100);
		Sizer.AddGotoLabel(100);
		Sizer.AddTopAlign(IDCANCEL,IDOK,0);
		Sizer.AddLeftAlign(IDC_ASKAGAIN,0,leftPos,10);
		if(Sets->dwDontAskHolder==NULL){
			Sizer.AddBottomAlign(IDC_TEXT,IDOK,topPos,-2);
			Sizer.AddTopAlign(IDC_ASKAGAIN,IDOK,bottomPos,-2);
		}else{
			if(Sets->sCancelButtonText==""){
				Sizer.AddBottomAlign(IDC_TEXT,IDOK,topPos,-2);
				Sizer.AddTopAlign(IDC_ASKAGAIN,IDOK,topPos,0);
			}else{
				Sizer.AddBottomAlign(IDC_TEXT,IDC_ASKAGAIN,topPos,-2);
				Sizer.AddBottomAlign(IDC_ASKAGAIN,IDOK,topPos,-2);
			}
		}
		if(Sets->sCancelButtonText==""){
			Sizer.AddLeftAlign(IDCANCEL,0,rightPos,0);
			Sizer.AddRightAlign(IDCANCEL,0,rightPos,0);
			Sizer.AddRightAlign(IDOK,0,-4);
			if(Sets->dwDontAskHolder==NULL){
				Sizer.AddLeftAlign(IDOK,0,4);
			}else{
				Sizer.AddLeftAlign(IDOK,0,rightPos,-INFOBUTT_WIDTH-4);
			}
			Sizer.AddRightAlign(IDC_ASKAGAIN,IDOK,leftPos,-4);
		}else{
			Sizer.AddRightAlign(IDC_ASKAGAIN,0,rightPos,-10);
			Sizer.AddLeftAlign(IDOK,0,4);
			Sizer.AddRightAlign(IDOK,0,topCenter,-4);
			Sizer.AddLeftAlign(IDCANCEL,0,topCenter,4);
			Sizer.AddRightAlign(IDCANCEL,0,-4);
		}
	}
	{// CG: The following block was added by the ToolTips component.
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);
		m_tooltip.AddTool(GetDlgItem(IDC_TEXT), Sets->sTitle);
	}
	BOOL bNoIcon=(Sets->bStartHided==3) || bOnTaskBar || (Sets->bDetached==1);
	m_STray.Create(NULL, WM_USER, Sets->sTitle, Sets->hIcon,IDR_INFOWND_MENU,this,bNoIcon?TRUE:FALSE);
	m_STray.SetMenuDefaultItem(0, TRUE);
	m_STray.SetUserMenu(&RefreshInfoMenu,NULL,this);
	if(Sets->hWnd){
		m_STray.pdwDefTICONAction=&lSingleClick;
	}
	SetParams(Sets->iIcon, Sets->sTitle, Sets->sText);
	m_hBr = new CBrush(Sets->hWnd?objSettings.lFloaterBG:BGCOLOR);
	m_Icon.SetBrush(Sets->hWnd?objSettings.lFloaterBG:BGCOLOR);
	m_BtMinim.SetBitmap(theApp.LoadIcon(IDI_ICON_MINIM));
	m_BtMinim.SetStyles(CBS_FLAT);
	m_BtMinim.SetBrush(Sets->hWnd?objSettings.lFloaterBG:BGCOLOR);
	SetOkText(Sets->sOkButtonText);
	m_Cancel.SetWindowText(Sets->sCancelButtonText);
	m_AskAgain.SetWindowText(Sets->sAskAgainButtonText);
	GetDlgItem(IDC_WICON)->EnableWindow(FALSE);
	if(Sets->dwDontAskHolder==NULL){
		m_AskAgain.ShowWindow(SW_HIDE);
	}else{
		m_AskAgain.SetCheck(*Sets->dwDontAskHolder>0?TRUE:FALSE);
		UpdateData(FALSE);
	}
	BOOL bWindowHidden=(Sets->bStartHided && Sets->bDetached!=1) || (Sets->bStartHided==3 && Sets->bDetached==1);
	if(bWindowHidden){
		if(bOnTaskBar){
			ShowWindow(SW_SHOWMINNOACTIVE);//SW_SHOWMINIMIZED?
		}else{
			ShowWindow(SW_HIDE);
		}
	}else{
		if(Sets->hWnd!=NULL){
			ShowWindowAsync(GetSafeHwnd(),SW_SHOWNOACTIVATE);
		}else{
			ShowWindow(SW_SHOW);
		}
	}
	if(Sets->hWnd!=NULL){
		ShowFloater(0);
	}
	SetDefID(IDOK);
	BOOL bNeedFocus=(Sets->hWnd==NULL);
	dwTimer=SetTimer(INFOWND_TIMERID,1000,NULL);
	return bNeedFocus;
}

void InfoWnd::ShowFloater(int bShow)
{
	if(bShow<0){
		ShowWindow(SW_HIDE);
	}else{
		if(!objSettings.bUnderWindows98 && objSettings.lFloaterTransp<100){
			SetLayeredWindowStyle(this->GetSafeHwnd(),TRUE);
			SetWndAlpha(this->GetSafeHwnd(),objSettings.lFloaterTransp,TRUE);
		}
		if(bShow>0){// при нуле не меняем показатель видимости
			ShowWindowAsync(GetSafeHwnd(),SW_SHOWNOACTIVATE);
		}
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

void InfoWnd::GetSlayerMenuItem(HMENU pWndMenu, DWORD dwCode)
{
	int iCount=0;
	aLastCommandsCodes.InsertAt(iCount,dwCode);
	aLastCommandsNames.InsertAt(iCount,Format("Item #%lu",dwCode));
	MENUITEMINFO mInfo;
	memset(&mInfo,0,sizeof(mInfo));
	mInfo.cbSize=sizeof(mInfo);
	mInfo.fMask=MIIM_DATA|0x00000100|MIIM_STATE;
	aLastCommandsInfos.InsertAt(iCount,mInfo);
	aLastCommandsInfos[iCount].wID=dwCode;
	// Вытаскиваем информацию о пункте
	if(!GetMenuItemInfo(pWndMenu,dwCode,FALSE,&aLastCommandsInfos[iCount])){
		DWORD dwLastErr=GetLastError();
	}
	char szItemName[30]="";
	GetMenuString(pWndMenu,dwCode,szItemName,sizeof(szItemName),MF_BYCOMMAND);
	aLastCommandsNames[iCount]=szItemName;
}

CString GetAppIdentifier(HWND hWin)
{
	CString sRes="";
	if(objSettings.bTakeComLineForFloater){
		sRes=GetProcessComLine(hWin);
	}
	if(sRes==""){
		sRes=GetExeFromHwnd(hWin);
	}
	if(sRes==""){
		sRes="MAWA";
	}
	return sRes;
}

void InfoWnd::LoadMenuItemsForFile()
{
	if(Sets->hWnd==NULL){
		return;
	}
	aLastCommandsCodes.RemoveAll();
	aLastCommandsInfos.RemoveAll();
	aLastCommandsNames.RemoveAll();
	CString sFolder=getFileFullName(TRAYAPP_FOLDER,TRUE);
	CQuickRunItem* pQRun=FindQRun(Sets->hWnd);
	CString sExe;
	if(pQRun){
		sExe=pQRun->sItemName;
	}else{
		sExe=GetAppIdentifier(Sets->hWnd);
	}
	if(sExe==""){
		sExe=Sets->sTitle;
	}
	CString sFile=GetStringHash(sExe);
	CString sConfFile=sFolder+sFile+".conf";
	if(!isFileExist(sConfFile)){
		return;
	}
	CDataXMLSaver IniFile(sConfFile,"app-tray",TRUE);
	if(!Sets->bForceMinType){
		IniFile.getValue("x-pos",Sets->xPos,Sets->xPos);
		IniFile.getValue("y-pos",Sets->yPos,Sets->yPos);
	}
	IniFile.getValue("hotkey",Sets->fHK);
	IniFile.getValue("detached",bDetachedFromSave);
	RegisterIWHK(TRUE);
	if(Sets->bDetached==0 && !Sets->bForceMinType){
		// Если не задано, то считываем
		bDetachedFromSaveValid=TRUE;
		Sets->bDetached=bDetachedFromSave;
	}else{
		bDetachedFromSaveValid=FALSE;
	}
	if(pQRun==NULL){
		HMENU pWndMenu=::GetMenu(Sets->hWnd);
		if(!IsMenu(pWndMenu)){
			return;
		}
		int iCount=0;
		while(1){
			DWORD dwCode=0;
			if(!IniFile.getValue(Format("item%i-code",iCount),dwCode) || dwCode==0){
				break;
			}
			GetSlayerMenuItem(pWndMenu, dwCode);
			iCount++;
		}
		aLastCommandsCodes.SetSize(iCount);
		aLastCommandsNames.SetSize(iCount);
		aLastCommandsInfos.SetSize(iCount);
	}
}

void InfoWnd::SaveMenuItemsForFile()
{
	CreateDirIfNotExist(getFileFullName(TRAYAPP_FOLDER,TRUE));
	CString sFolder=getFileFullName(TRAYAPP_FOLDER,TRUE);
	CQuickRunItem* pQRun=FindQRun(Sets->hWnd);
	CString sExe;
	if(pQRun){
		sExe=pQRun->sItemName;
	}else{
		sExe=GetAppIdentifier(Sets->hWnd);
	}
	if(sExe==""){
		sExe=Sets->sTitle;
	}
	CString sFile=GetStringHash(sExe);
	CString sConfFile=sFolder+sFile+".conf";
	CDataXMLSaver IniFile(sConfFile,"app-tray",FALSE);
	IniFile.putValue("app-path",sExe);
	IniFile.putValue("x-pos",Sets->xPos);
	IniFile.putValue("y-pos",Sets->yPos);
	if(bDetachedFromSaveValid){
		bDetachedFromSave=Sets->bDetached;
	}
	RegisterIWHK(FALSE);
	IniFile.putValue("hotkey",Sets->fHK);
	IniFile.putValue("detached",bDetachedFromSave);
	int iCodesSize=aLastCommandsCodes.GetSize();
	for(int i=0;i<iCodesSize;i++){
		IniFile.putValue(Format("item%i-code",i),aLastCommandsCodes[iCodesSize-i-1]);
	}
}

void InfoWnd::SetCallback(OnButtonCallback func, void* param)
{
	if(func && ::IsWindow(this->GetSafeHwnd()) && GetDlgItem(IDC_TEXT)!=0){
		SetDefID(IDC_TEXT);
		GetDlgItem(IDC_TEXT)->SetFocus();
	}
	callback=func;
	CBParam=param;
}

void InfoWnd::Cleanup()
{
	m_STray.RemoveIcon();
	if(dwTimer!=0){
		KillTimer(dwTimer);
	}
}

void InfoWnd::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent!=INFOWND_TIMERID || lDoNotPerformAnything){
		return;
	}
	if(Sets->hWnd){
		int iPos=0;
		CQuickRunItem* pQRun=FindQRun(Sets->hWnd,&iPos);
		if(iPos<0 || (pQRun==NULL && (!IsWindow(Sets->hWnd) || ::IsWindowVisible(Sets->hWnd)))){
			Sets->hWnd=NULL;
			ExitWindow(0);
			return;
		}
	}
	if((::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)&WS_EX_TOPMOST)==0){
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	}
	if(dwMilliseconds==0){
		return;
	}
	if(GetTickCount()-dwMillisecondsStart>dwMilliseconds){
		ExitWindow(0);
		return;
	}
	CString sTitle=Format("%s - %i ",Sets->sTitle,int((dwMilliseconds-GetTickCount()+dwMillisecondsStart)/1000))+_l("sec. to be closed");
	m_Title.SetWindowText(sTitle);
	SetWindowText(sTitle);
	CDialog::OnTimer(nIDEvent);
}

void InfoWnd::RunTimer(DWORD dwTimeout)
{
	dwMilliseconds=dwTimeout;
	dwMillisecondsStart=GetTickCount();
}

DWORD WINAPI RunQRunInThread(LPVOID param);
BOOL InfoWnd::PreTranslateMessage(MSG* pMsg) 
{
	DWORD dwID=::GetDlgCtrlID(pMsg->hwnd);
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	if(Sets->hWnd){
		if(pMsg->message==WM_DROPFILES){
			int iQRunPos=-1;
			CQuickRunItem* pQRun=FindQRun(Sets->hWnd,&iQRunPos);
			// Пробегаемся по детям, которые принимают dropfiles
			HWND hTargetWindow=NULL;
			if(pQRun==NULL){
				hTargetWindow=hDDEWnd;
				if(hTargetWindow==NULL){
					MYGUITHREADINFO gui;			
					DWORD dwCurWinProcID=0;
					DWORD dwCurWinThreadID=GetWindowThreadProcessId(Sets->hWnd,&dwCurWinProcID);
					if(GetGUIThreadInfoMy(dwCurWinThreadID, &gui)){
						hTargetWindow=gui.hwndFocus;
						if(objSettings.hFocusWnd==NULL){
							hTargetWindow=gui.hwndCaret;
						}
						if(hTargetWindow==NULL){
							hTargetWindow=gui.hwndActive;
						}
					}
				}
				if(hTargetWindow==NULL){
					hTargetWindow=Sets->hWnd;
				}
				//LRESULT lRes=::SendMessage(hTargetWindow,pMsg->message,pMsg->wParam,pMsg->lParam);
				::PostMessage(hTargetWindow,pMsg->message,pMsg->wParam,pMsg->lParam);
			}else{
				BOOL bCopy=TRUE;
				CString sTargetFile;
				if(pQRun->lParamType==2){
					if (pQRun->sStartupDir!=""){
						sTargetFile=pQRun->sStartupDir;
					}else{
						sTargetFile=pQRun->sItemPath;
						SubstMyVariablesForQRun(sTargetFile,NULL,FALSE,TRUE);
					}
				}else{
					bCopy=FALSE;
				}
				AddSlashToPath(sTargetFile);
				char szTargetDir[MAX_PATH+4];
				memset(szTargetDir,0,MAX_PATH+4);
				strcpy(szTargetDir,sTargetFile);
				HDROP hDropInfo=(HDROP)pMsg->wParam;
				char szFilePath[MAX_PATH+3]={0},szFileName[MAX_PATH+3]={0},szExtension[MAX_PATH+3]={0};
				UINT cFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
				long lFromBufferSize=(MAX_PATH+10)*cFiles+1;
				char* szFromFiles=new char[lFromBufferSize+5];// Лишнее для SHFileOperation
				memset(szFromFiles,0,lFromBufferSize+5);
				long lPos=0;
				for (UINT u = 0; u < cFiles; u++){
					DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
					if(bCopy){
						memcpy(szFromFiles+lPos,szFilePath,strlen(szFilePath));
						lPos+=strlen(szFilePath)+1;
					}else{
						memcpy(szFromFiles+lPos,"\"",1);
						lPos+=1;
						memcpy(szFromFiles+lPos,szFilePath,strlen(szFilePath));
						lPos+=strlen(szFilePath);
						memcpy(szFromFiles+lPos,"\" ",2);
						lPos+=2;
					}
				}
				DragFinish(hDropInfo);
				BOOL bRes=FALSE;
				if(bCopy){
					SHFILEOPSTRUCT FOData;
					memset(&FOData,0,sizeof(FOData));
					FOData.hwnd=this->GetSafeHwnd();
					FOData.wFunc=FO_COPY;
					FOData.hNameMappings=0;
					FOData.fFlags=FOF_RENAMEONCOLLISION;//FOF_ALLOWUNDO|FOF_NO_CONNECTED_ELEMENTS|
					FOData.pTo=szTargetDir;
					FOData.pFrom=szFromFiles;
					bRes=(SHFileOperation(&FOData)==0);
				}else if(iQRunPos>=0){
					RunQRunInThreadParam* pParam=new RunQRunInThreadParam;
					pParam->bPopupItem=FALSE;//objSettings.bPopupRecentlyUsed && ((lParam & QRUN_ONWKSTART)==0);
					pParam->iWndNum=iQRunPos;
					pParam->bSwapCurDesk=FALSE;
					pParam->szAdditionalParameters=szFromFiles;
					StartQRun(pParam);
				}
				delete[] szFromFiles;
				if(bRes && bCopy){
					CString sRes=Format("%lu %s",cFiles,_l("File(s) copied"));
					ShowBaloon(sRes,_l("File operation"),3000,FALSE);
				}
			}
			return TRUE;
		}
	}
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE){
		return TRUE;
	}
	/*if(Sets->hWnd){
		if(pMsg->message==WM_PRINT || pMsg->message==WM_PRINTCLIENT){
			LRESULT lRes=::SendMessage(Sets->hWnd,pMsg->message,pMsg->wParam,pMsg->lParam);
			return lRes;
		}
	}*/
	if(dwID==IDC_TEXT){
		if(pMsg->message==WM_MOUSEMOVE){
			CPoint pt;
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
			OnMouseMove(0,pt);
			return TRUE;
		}
		if(pMsg->message==WM_LBUTTONUP){
			CPoint pt;
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
			OnLButtonUp(0,pt);
			return TRUE;
		}
		if(pMsg->message==WM_LBUTTONDOWN){
			CPoint pt;
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
			OnLButtonDown(0,pt);
			return TRUE;
		}
	}
	if(Sets->hWnd){
		BOOL bClick=((objSettings.lFloatUseSingleClick && lMoveDist<3 && (pMsg->message==WM_NCLBUTTONUP || pMsg->message==WM_LBUTTONUP)) || (!objSettings.lFloatUseSingleClick && (pMsg->message==WM_NCLBUTTONDBLCLK || pMsg->message==WM_LBUTTONDBLCLK)));
		if((bExpandableDirectory && objSettings.lFloatUseLBDir && bClick) || pMsg->message==WM_NCRBUTTONUP || pMsg->message==WM_RBUTTONUP){
			ReplyMessage(TRUE);
			CMenu* pMenu=RefreshInfoMenuEx(this,bExpandableDirectory && bClick, (bExpandableDirectory && objSettings.lFloatUseLBDir && !bClick));
			if(pMenu){
				POINT pos;
				::GetCursorPos(&pos);
				// Отключаем анимацию на время показа...
				BOOL pAnim=FALSE;
				if((m_STray.GetMenuFlags() & 0x4000L)!=0){//TPM_NOANIMATION
					SystemParametersInfo(0x1002/*SPI_GETMENUANIMATION*/,0,&pAnim,0);
					if(pAnim){
						BOOL pNoAnim=0;
						SystemParametersInfo(0x1003/*SPI_SETMENUANIMATION*/,0,NULL,0);
					}
				}
				pMenu->TrackPopupMenu(TPM_RECURSE, pos.x, pos.y, this, NULL);
				if((m_STray.GetMenuFlags() & 0x4000L)!=0 && pAnim){//Включаем анимацию обратно
					SystemParametersInfo(0x1003/*SPI_SETMENUANIMATION*/,0,&pAnim,0);
				}
				delete pMenu;
			}
		}else if(bClick){
			PostMessage(WM_COMMAND,ID_INFOWND_OPENPR,0);
		}
		if(pMsg->message==WM_NCLBUTTONDBLCLK || pMsg->message==WM_LBUTTONDBLCLK){
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

extern HWND g_hRecursiveWndBase;
LRESULT InfoWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		if(Sets->bDetached==1){
			if(message==WM_WINDOWPOSCHANGING || message==WM_SHOWWINDOW || message==WM_SIZING){// Запоминаем позицию
				if(IsWindow(this->GetSafeHwnd())){
					CRect rt;
					GetWindowRect(&rt);
					Sets->xPos=rt.left;
					Sets->yPos=rt.top;
				}else{
					Sets->xPos=Sizer.m_rMain.left;
					Sets->yPos=Sizer.m_rMain.top;
				}
			}
		}
		return DispatchResultFromSizer;
	}
	if(message==WM_HOTKEY){
		if(wParam==THISIWHK_HOTKEY_NUM){
			PostMessage(WM_COMMAND,ID_INFOWND_OPENPR,1);
		}
		return TRUE;
	}
	if(Sets->hWnd){
		CQuickRunItem* pQRun=FindQRun(Sets->hWnd);
		if(pQRun){
			if(message==WM_INITMENUPOPUP){
				HMENU h=(HMENU)wParam;
				// Дополняем пункты если надо (процессы, подкаталоги)
				if(h && IsMenu(h)){
					char szItem[MAX_PATH+30]="";
					memset(szItem,0,sizeof(szItem));
					if(GetMenuString(h,0,szItem,sizeof(szItem)-1,MF_BYPOSITION)){
						if(strstr(szItem,EXPAND_FOLDER_ID)!=NULL){
							SetWaitTrayIcon trWait(&m_STray);
							// Раскрываем каталог
							::DeleteMenu(h,0,MF_BYPOSITION);
							CString sFileTitle;
							char szFileName[MAX_PATH]="",szFileExt[MAX_PATH]="";
							CString sFolder=(const char*)(szItem+strlen(EXPAND_FOLDER_ID));
							if(sFolder!=""){
								// Вытаскиваем маску (если есть)
								CString sMask;
								int iMaskPos=sFolder.Find(EXPAND_FOLDER_MASK);
								if(iMaskPos>0){
									sMask=sFolder.Mid(iMaskPos+strlen(EXPAND_FOLDER_MASK));
									sFolder=sFolder.Left(iMaskPos);
									sMask.Replace(",",";");
								}
								CMenu aFiles;
								aFiles.Attach(h);
								if(!isFileExist(sFolder)){
									AddMenuString(&aFiles,0,_l("Folder not found"),NULL,FALSE,NULL,TRUE);
								}else{
									CFileInfoArray dir;
									dir.AddDir(sFolder,sMask!=""?sMask:"*.*",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,TRUE);
									long lDirSize=dir.GetSize();
									if(lDirSize==0){
										AddMenuString(&aFiles,0,_l("Folder is empty"),NULL,FALSE,NULL,TRUE);
									}else{
										long lBreakCount=0;
										long lBreakCountLim=lDirSize/3;
										if(lBreakCountLim<20){
											lBreakCountLim=lDirSize/2;
										}
										if(lBreakCountLim<20){
											lBreakCountLim=lDirSize+999;
										}
										dir.BubbleUpDirectories();
										for(int i=0;i<lDirSize;i++){
											CString sFile=dir[i].GetFilePath();
											if(sFile!=""){
												if(aExpandedFolderPaths.GetSize()==EXPAND_FILES_MAX){
													AddMenuString(&aFiles,0,CString("*** ")+_l("Too much files to load")+" ***");
													return 0;
												}
												int iIndex=aExpandedFolderPaths.Add(sFile);
												_splitpath(sFile,NULL,NULL,szFileName,szFileExt);
												sFileTitle=TrimMessage(szFileName,20);
												sFileTitle+="\t";
												sFileTitle+=szFileExt;
												CString sQRunIcon=GetExeIconIndex(sFile);
												if(dir[i].IsDirectory()){
													CMenu sub;
													sub.CreatePopupMenu();
													AddSlashToPath(sFile);
													CString sFirstItem=Format("%s%s%s%s",EXPAND_FOLDER_ID,sFile,EXPAND_FOLDER_MASK,sMask);
													AddMenuString(&sub,0,sFirstItem);
													AddMenuSubmenu(&aFiles,&sub,sFileTitle,_bmp().Get(sQRunIcon),WM_USER+EXPAND_FILES+iIndex);
													lBreakCount++;
												}else{
													BOOL bBreak=FALSE;
													if(lBreakCount>lBreakCountLim){
														lBreakCount=0;
														bBreak=TRUE;
													}
													AddMenuString(&aFiles,WM_USER+EXPAND_FILES+iIndex,sFileTitle,_bmp().Get(sQRunIcon),FALSE,0,FALSE,FALSE,bBreak);
													lBreakCount++;
												}
											}
										}
									}
								}
								TUNE_UP_ITEM2(aFiles,"Right-click: more options");
								aFiles.Detach();
							}
							return 0;
						}
					}
				}
			}
			if(message==WM_MENURBUTTONUP){
				HMENU h=(HMENU)lParam;
				if(IsMenu(h)){
					DWORD dwID=(DWORD)wParam;
					MENUITEMINFO mii;
					memset(&mii,0,sizeof(mii));
					mii.cbSize=sizeof(mii);
					mii.fMask=MIIM_ID;
					GetMenuItemInfo(h,dwID,TRUE,&mii);
					dwID=mii.wID;
					if(dwID>=WM_USER+EXPAND_FILES && dwID<(DWORD)(WM_USER+EXPAND_FILES+EXPAND_FILES_MAX)){
						dwID=dwID-(WM_USER+EXPAND_FILES);
						if(dwID>=0 && dwID<DWORD(aExpandedFolderPaths.GetSize())){
							CPoint pt;
							GetCursorPos(&pt);
							ShowContextMenu(this->GetSafeHwnd(), aExpandedFolderPaths[dwID], pt.x, pt.y);
						}
						return TRUE;
					}else if(g_hRecursiveWndBase==NULL){
						g_hRecursiveWndBase=this->GetSafeHwnd();
						LRESULT iRes=pMainDialogWindow->SendMessage(message, wParam, lParam);
						g_hRecursiveWndBase=NULL;
						return iRes;
					}
				}
			}
			if(message==WM_COMMAND){
				if(wParam>=WM_USER+EXPAND_FILES && wParam<(DWORD)(WM_USER+EXPAND_FILES+EXPAND_FILES_MAX)){
					int iItem=wParam-(WM_USER+EXPAND_FILES);
					if(iItem>=0 && iItem<aExpandedFolderPaths.GetSize()){
						CString sItemPath=aExpandedFolderPaths[iItem];
						if(sItemPath!=""){
							int iRes=(int)::ShellExecute(this->GetSafeHwnd(),"",sItemPath,NULL,NULL,SW_SHOWNORMAL);
							if(iRes<=32){
								Alert(_l("Failed to start")+"\n'"+TrimMessage(sItemPath,30,3)+"' !",_l(PROGNAME": Quick-run"));
							}
						}
					}
					return TRUE;
				}
			}
		}else{
			if(message==WM_MEASUREITEM){
				MEASUREITEMSTRUCT str;
				memcpy(&str,(LPMEASUREITEMSTRUCT)lParam,sizeof(MEASUREITEMSTRUCT));
				int iItemNum=str.itemID-WM_USER-INFOWNDOFFSET;
				if(iItemNum>=0 && iItemNum<aM2I.GetSize() && (str.CtlType & ODT_MENU)){
					int iNewID=aM2I[iItemNum];
					str.itemID=iNewID;
					LRESULT lRes=::SendMessage(Sets->hWnd,message,wParam,(LPARAM)&str);
					memcpy((LPMEASUREITEMSTRUCT)lParam,&str,sizeof(MEASUREITEMSTRUCT));
					((LPMEASUREITEMSTRUCT)lParam)->itemID=iItemNum;
					return TRUE;
				}
			}
			if(message==WM_DRAWITEM){
				DRAWITEMSTRUCT str;
				memcpy(&str,(LPDRAWITEMSTRUCT)lParam,sizeof(DRAWITEMSTRUCT));
				int iItemNum=str.itemID-WM_USER-INFOWNDOFFSET;
				if(iItemNum>=0 && iItemNum<aM2I.GetSize() && (str.CtlType & ODT_MENU)){
					int iNewID=aM2I[iItemNum];
					str.itemID=iNewID;
					LRESULT lRes=::SendMessage(Sets->hWnd,message,wParam,(LPARAM)&str);
					memcpy((LPDRAWITEMSTRUCT)lParam,&str,sizeof(DRAWITEMSTRUCT));
					((LPDRAWITEMSTRUCT)lParam)->itemID=iItemNum;
					return TRUE;
				}
			}
		}
	}
	if(Sets->hWnd && message==WM_TIMER && !isMove && GetAsyncKeyState(VK_LBUTTON)>=0 && Sets->bDetached){
		CRect rt;
		CPoint pt;
		GetWindowRect(&rt);
		GetCursorPos(&pt);
		if(rt.PtInRect(pt)){
			dwLastMovementTime=GetTickCount();
			if(dwPrevTime==0){
				dwPrevTime=dwLastMovementTime;
			}
			if(dwPrevTime>0 && dwLastMovementTime-dwPrevTime>500){
				CRect rt;
				CPoint pt;
				GetWindowRect(&rt);
				GetCursorPos(&pt);
				ShowFloater(-1);
				while(rt.PtInRect(pt)){
					Sleep(500);
					GetCursorPos(&pt);
				}
				ShowFloater(1);
				dwPrevTime=0;
			}
		}
	}else{
		dwPrevTime=0;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL InfoWnd::RegisterIWHK(BOOL bReg)
{
	BOOL bRes=0;
	if(bReg && Sets->fHK.Present()){
		char szTitle[256]="";
		::GetWindowText(Sets->hWnd,szTitle,sizeof(szTitle));
		CString sDsc=_l("Activate")+" "+szTitle;
		bRes=RegisterIHotKey(this->m_hWnd,THISIWHK_HOTKEY_NUM,Sets->fHK,Format("FLOATER_%s",GetRandomName()),sDsc);
		if(!bRes){
			Alert(_l("Sorry, this hotkey can`t be registered!"));
		}
	}
	if(!bReg){
		bRes=UnregisterIHotKey(this->m_hWnd,THISIWHK_HOTKEY_NUM);
	}
	return bRes;
}

BOOL InfoWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(lParam==0 && wParam==2){
		if(strlen(Sets->sOkButtonText)==0){
			return TRUE;
		}else{
			ExitWindow(1);
			return TRUE;
		}
	}
	if(wParam==ID_INFOWND_DROPRECENTLIST){
		aLastCommandsCodes.RemoveAll();
		aLastCommandsInfos.RemoveAll();
		aLastCommandsNames.RemoveAll();
		SaveMenuItemsForFile();
		return TRUE;
	}
	if(wParam>WM_USER){
		TRACE1("Received message: %i",wParam-WM_USER);
		if(wParam==ID_INFOWND_SET_TITL){
			char* s=(char*)lParam;
			SetParams(0,s,0);
			free(s);
			return TRUE;
		}
		if(wParam==ID_INFOWND_OPENPR){
			bOpenSlayerANDSwitch=TRUE;
			OnInfowndOpen();
			return TRUE;
		}else if(wParam==ID_INFOWND_SETHK){
			SimpleTracker lc(lDoNotPerformAnything);
			RegisterIWHK(FALSE);
			wk.g_INFOWNDHK_TOPIC_Hk=Sets->fHK;
			wk.g_INFOWNDHK_TOPIC_InfoWnd=this;
			objSettings.OpenOptionsDialog(1,INFOWNDHK_TOPIC,this);
			wk.g_INFOWNDHK_TOPIC_InfoWnd=NULL;
			Sets->fHK=wk.g_INFOWNDHK_TOPIC_Hk;
			RegisterIWHK(TRUE);
			return TRUE;
		}else if(wParam==ID_INFOWND_CLSEPR){
			if(Sets->hWnd && IsWindow(Sets->hWnd)){
				// Закрытие приложенимя отловится в обработчике таймера
				HWND hWin2=Sets->hWnd;
				GetWindowTopParent(hWin2);
				//::PostMessage(hWin2,WM_QUIT,0,0);
				::PostMessage(hWin2,WM_CLOSE,0,0);
			}else{
				ExitWindow(0);
			}
			return TRUE;
		}else if(wParam==ID_INFOWND_TERM){
			if(Sets->hWnd && IsWindow(Sets->hWnd)){
				DWORD dwProcID=0;
				DWORD dwThread=GetWindowThreadProcessId(Sets->hWnd,&dwProcID);
				// Закрытие приложенимя отловится в обработчике таймера
				if(!KillProcess(dwProcID,Sets->sTitle)){
					AddError(_l("Unable to kill process"));
				}
				return TRUE;
			}
		}else if(wParam==ID_INFOWND_DETACH){
			if(Sets->bStartHided!=3){
				if(Sets->bDetached){
					Sets->bDetached=0;
					ShowWindow(SW_HIDE);
					m_STray.ShowIcon();
				}else{
					Sets->bDetached=1;
					ShowWindow(SW_SHOW);
					m_STray.HideIcon();
				}
				SetParams(0,NULL,Sets->sText);
				return TRUE;
			}
		}else if(wParam==ID_INFOWND_EDITQ){
			int iPos=0;
			CQuickRunItem* pQRun=FindQRun(Sets->hWnd,&iPos);
			if(pQRun){
				pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+QRUNAPP_EDIT+iPos,0);
			}
			return TRUE;
		}else if(wParam>=WM_USER+QRUNAPPLICATION && wParam<WM_USER+QRUNAPPLICATION+USERMSG_STEP_HALF){
			pMainDialogWindow->PostMessage(WM_COMMAND,wParam,lParam);
			return TRUE;
		}else if(wParam>=WM_USER+INFOWNDOFFSET && wParam<WM_USER+INFOWNDOFFSET+USERMSG_STEP_HALF){
			if(Sets->hWnd){
				WPARAM iItemNum=wParam-WM_USER-INFOWNDOFFSET;
				if(iItemNum>=0 && DWORD(iItemNum)<DWORD(aM2I.GetSize())){
					DWORD wMsg=aM2I[iItemNum];
					if(Sets->bLastOpenedTrayMenuType){
						::PostMessage(Sets->hWnd,WM_SYSCOMMAND,wMsg,lParam);
					}else{
						::PostMessage(Sets->hWnd,WM_COMMAND,wMsg,0);
						if(objSettings.lHTrayRecentItems>0){
							BOOL bAlreadyExist=0;
							for(int i=0;i<aLastCommandsCodes.GetSize();i++){
								if(aLastCommandsCodes[i]==wMsg){
									//bAlreadyExist=TRUE;
									//Удаляем, чтобы добавилось к началу 
									//Это всплывание последнего использованного пункта
									aLastCommandsCodes.RemoveAt(i);
									aLastCommandsNames.RemoveAt(i);
									aLastCommandsInfos.RemoveAt(i);
									break;
								}
							}
							if(!bAlreadyExist){
								HMENU pWndMenu=::GetMenu(Sets->hWnd);
								GetSlayerMenuItem(pWndMenu,wMsg);
								if(aLastCommandsCodes.GetSize()>objSettings.lHTrayRecentItems){
									aLastCommandsCodes.SetSize(objSettings.lHTrayRecentItems);
									aLastCommandsNames.SetSize(objSettings.lHTrayRecentItems);
									aLastCommandsInfos.SetSize(objSettings.lHTrayRecentItems);
								}
							}
						}
					}
					return TRUE;
				}
			}
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}

void InfoWnd::OnInfowndDelete()
{
	if(strlen(Sets->sOkButtonText)==0){
		return;
	}
	ExitWindow(0);
}

void InfoWnd::OpenSlayerWindow()
{
	if(Sets->hWnd){
		SaveMenuItemsForFile();
		CQuickRunItem* pQRun=FindQRun(Sets->hWnd);
		if(pQRun==NULL){
			// Если показывать и само окно то MSDN Help плющит например :(
			//::ShowWindow(Sets->hWnd,SW_SHOW);
			HWND hWin2=Sets->hWnd;
			BOOL bMakeMax1=(BOOL)GetProp(hWin2,"WAS_MAXIMIZED1");
			BOOL bMakeMax2=(BOOL)GetProp(hWin2,"WAS_MAXIMIZED2");
			GetWindowTopParent(hWin2,0);
			::ShowWindowAsync(hWin2,SW_SHOW);
			if(hWin2!=Sets->hWnd){
				// B tuj nj;t
				::ShowWindowAsync(Sets->hWnd,SW_SHOW);
			};
			::ShowOwnedPopups(hWin2,TRUE);
			if(bOpenSlayerANDSwitch){
				// делаем это до максимизации!
				::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)Sets->hWnd);
				::SetForegroundWindow(hWin2);
			}
			// Восстанавливаем?
			if(!bMakeMax1 && !bMakeMax2){
				WINDOWPLACEMENT pl;
				memset(&pl,0,sizeof(pl));
				pl.length=sizeof(pl);
				::GetWindowPlacement(hWin2,&pl);
				if(pl.showCmd==SW_SHOWMINIMIZED){
					::PostMessage(hWin2,WM_SYSCOMMAND,SC_RESTORE,0);
				}
			}
			if(bMakeMax2){
				::PostMessage(Sets->hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,0);
			}else if(bMakeMax1){
				::PostMessage(hWin2,WM_SYSCOMMAND,SC_MAXIMIZE,0);
			}
			Sets->hWnd=NULL;
		}
	}
}

void InfoWnd::OnClose() 
{
	ExitWindow(0);
	return;
}

void InfoWnd::OnInfowndOpen() 
{
	if(Sets->hWnd){
		int iPos=0;
		CQuickRunItem* pQRun=FindQRun(Sets->hWnd,&iPos);
		if(pQRun==NULL){
			this->PostMessage(WM_CLOSE,0,0);
			return;
		}else if (pMainDialogWindow){
			pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+iPos,0);
		}
	}else{
		ShowWindow(SW_SHOW);
	}
}

void InfoWnd::SetAutoCloseTime(DWORD dwMilliseconds)
{
	RunTimer(dwMilliseconds);
}

void InfoWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(Sets->hWnd!=NULL){
		if(nID==SC_MINIMIZE || nID==SC_MAXIMIZE){
			return;
		}
	}
	if(nID==SC_MINIMIZE && !bOnTaskBar){
		ShowWindow(SW_HIDE);
		return;
	}	
	if(nID==SC_CLOSE){
		if(Sets->sCancelButtonText!=""){
			ExitWindow(2);
		}else{
			ShowWindow(SW_HIDE);
		}
		return;
	}	
	CDialog::OnSysCommand(nID, lParam);
}

void InfoWnd::SetParams(UINT iIcon, const char* szTitle, const char* szText)
{
	if(Sets->hWnd==NULL){
		Sets->iIcon=iIcon;
		GetInfoIcon();
		SetIcon(Sets->hIcon,TRUE);
		SetIcon(Sets->hIcon,FALSE);
		m_STray.SetIcon(Sets->hIcon);
	}
	if(szTitle){
		SetWindowText(szTitle);
		m_Title.SetWindowText(szTitle);
		Sets->sTitle=szTitle;
		m_STray.SetTooltipText(szTitle);
		m_tooltip.UpdateTipText(szTitle,GetDlgItem(IDC_TEXT));
	}
	if(szText){
		Sets->sText=szText;
		m_Text.SetWindowText(Sets->sText);
		if(Sets->hWnd==NULL){
			CSize size;
			GetInfoWndSize(size);
			CRect minRect(0,0,size.cx,size.cy);
			Sizer.SetMinRect(minRect);
			SetWindowPos(&wndTopMost,0,0,size.cx,size.cy,SWP_NOMOVE|SWP_NOACTIVATE);
			Sizer.ApplyLayout(TRUE);
		}
	}
}

void InfoWnd::SetInfoIcon(UINT iIcon)
{
	SetParams(iIcon);
}

void InfoWnd::SetText(const char* szText)
{
	SetParams(Sets->iIcon, NULL, szText);
}

HBRUSH InfoWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(TXCOLOR);
	pDC->SetBkColor(Sets->hWnd?objSettings.lFloaterBG:BGCOLOR);
	return HBRUSH(*m_hBr);
}

void InfoWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (isMove)
		ReleaseCapture();
	isMove=FALSE;
	lMoveDist=0;
}

void InfoWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fromPoint.x=point.x;
	fromPoint.y=point.y;
	isMove=TRUE;
	lMoveDist=0;
	SetCapture();
}

void InfoWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (isMove){
		CRect rect;
		GetWindowRect(&rect);
		long lWidth=rect.Width();
		long lHeight=rect.Height();
		lMoveDist+=abs(fromPoint.x-point.x);
		lMoveDist+=abs(fromPoint.y-point.y);
		rect.left-=fromPoint.x-point.x;
		rect.top-=fromPoint.y-point.y;
		rect.right=rect.left+lWidth;
		rect.bottom=rect.top+lHeight;
		MoveWindow(rect.left,rect.top,rect.Width(),rect.Height());
	}
}

void InfoWnd::PostNcDestroy() 
{
	OpenSlayerWindow();
	CDialog::PostNcDestroy();
	if(!Sets->bAsModal){// Удалиться там где создалось!
		delete this;
	}
}

void InfoWnd::SetButtonsVisibility(BOOL bOK)
{
	m_OkButton.ShowWindow(bOK?SW_SHOW:SW_HIDE);
}

void InfoWnd::SetOkText(const char* szOk)
{
	Sets->sOkButtonText=szOk;
	m_OkButton.SetWindowText(Sets->sOkButtonText);
	Sizer.SetGotoRule(MainRulePos,Sets->sOkButtonText.GetLength()>0?0:1);
	if(Sets->hWnd!=NULL){
		return;
	}
	CSize size;
	GetInfoWndSize(size);
	CRect minRect(0,0,size.cx,size.cy);
	Sizer.SetMinRect(minRect);
	SetWindowPos(&wndTopMost,Sizer.m_rMain.left,Sizer.m_rMain.top,size.cx,size.cy,SWP_NOMOVE|SWP_NOACTIVATE);
}

void InfoWnd::OnAskagain() 
{
	if(Sets->dwDontAskHolder){
		UpdateData();
		*Sets->dwDontAskHolder=m_AskAgain.GetCheck()?1:0;
	}
}

BOOL InfoWnd::InCallback()
{
	return bCallbackInProgress;
}

BOOL InfoWnd::CheckCallback(int iButton) 
{
	BOOL bRes=TRUE;
	if(callback){
		bCallbackInProgress=TRUE;
		EnableWindow(FALSE);
		bRes=callback(CBParam,iButton);
		callback=NULL;
		EnableWindow(TRUE);
		bCallbackInProgress=FALSE;
	}
	return bRes;
}

void InfoWnd::OnCancel() 
{
	if(CheckCallback(IDCANCEL)){
		ExitWindow(2);
	}
}

void InfoWnd::OnOK() 
{
	if(CheckCallback(IDOK)){
		ExitWindow(1);
	}
}

void InfoWnd::ExitWindow(int iRes)
{
	if(Sets->hWnd){
		OpenSlayerWindow();
	}
	if(Sets->dwDontAskHolder){
		UpdateData();
		*Sets->dwDontAskHolder=m_AskAgain.GetCheck()?1:0;
		if(*Sets->dwDontAskHolder){
			*Sets->dwDontAskHolder=iRes;
		}
	}
	Cleanup();
	//ShowWindow(SW_HIDE);
	if(!Sets->bAsModal){
		DestroyWindow();
	}else{
		EndDialog(iRes);
	}
}

void InfoWnd::OnBnClickedMinimize()
{
	ShowWindow(SW_HIDE);
}
