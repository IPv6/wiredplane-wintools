// SmartWndSizer.cpp: implementation of the CSmartWndSizer class.
//
//////////////////////////////////////////////////////////////////////
#include "SmartWndSizer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ADDDEBUGCHAR(c)
//#define ADDDEBUGCHAR(c) {CString s;m_pMain->GetWindowText(s);s=CString(c)+s;m_pMain->SetWindowText(s);}
typedef struct tagMENUBARINFO
{
    DWORD cbSize;
    RECT  rcBar;          // rect of bar, popup, item
    HMENU hMenu;          // real menu handle of bar, popup
    HWND  hwndMenu;       // hwnd of item submenu if one
    BOOL  fBarFocused:1;  // bar, popup has the focus
    BOOL  fFocused:1;     // item has the focus
} MENUBARINFO, *PMENUBARINFO, *LPMENUBARINFO;
typedef BOOL (WINAPI *_GetMenuBarInfo)(HWND hwnd,LONG idObject,LONG idItem,MENUBARINFO* pmbi);

CSmartCritSection& GetSizerLock()
{
	static CSmartCritSection cs;
	return cs;
}

CSmartCritSection* CSmartWndSizer::rectLock=&GetSizerLock();
CArray<SizerWnd,SizerWnd&>* CSmartWndSizer::m_AllWndSizerRectangles=NULL;
int CSmartWndSizer::iGlobalCounter=0;
CRect CSmartWndSizer::DesktopDialogRect(0,0,0,0);
BOOL CSmartWndSizer::bSlowMachine=-1;
CSmartWndSizer::CSmartWndSizer()
{
	m_iSaWi=0;
	iMenuH=-1;
	bMaximized=0;
	bSkipResize=0;
	if(iGlobalCounter==0){
		//rectLock=new CSmartCritSection();
		m_AllWndSizerRectangles=new CArray<SizerWnd,SizerWnd&>;
	}
	iGlobalCounter++;
	if(bSlowMachine==-1){
		bSlowMachine=GetSystemMetrics(SM_SLOWMACHINE);
	}
	iInit=0;m_hKeyRoot=HKEY_CURRENT_USER;m_dwOptions=STICKABLE;
	if(DesktopDialogRect.IsRectEmpty()){
		//::SystemParametersInfo(SPI_GETWORKAREA,0,&DesktopDialogRect,0);
		DesktopDialogRect=GetAllMonitorsRECT();
	}
	bInResizingLock=0;
	dwSizerLevel=0;
	bMessageRecursingPreventer=0;
	bResizingElementsArePresent=-1;
	pResizeAffectedWnd=NULL;
	iInPresetPosition=0;
	hResizeCur=NULL;
	iAffectType=-1;
};

CSmartWndSizer::~CSmartWndSizer()
{
	SaveSizeAndPosToRegistry();
	iInit=0;
	int i=0;
	{
		CSmartLocker lock(rectLock);
		int iMax=m_AllWndSizerRectangles->GetSize();
		for(i=0; i<iMax;i++){
			SizerWnd& aRect=(*m_AllWndSizerRectangles)[i];
			DWORD dwWnd1=DWORD(aRect.pWnd);
			DWORD dwWnd2=DWORD(m_pMain);
			if(dwWnd1==dwWnd2){
				m_AllWndSizerRectangles->RemoveAt(i);
				break;
			}
		}
	}
	for(i=0;i<m_AlignRules.GetSize();i++){
		delete m_AlignRules[i];
	}
	m_AlignRules.RemoveAll();
	int iKey;
	CSmartWndSizer_Element* pElem;
	POSITION p=m_WndElements.GetStartPosition();
	while(p!=NULL){
		m_WndElements.GetNextAssoc(p, iKey, pElem);
		delete pElem;
	}
	m_WndElements.RemoveAll();
	m_Labels.RemoveAll();
	iGlobalCounter--;
	if(iGlobalCounter==0){
		//delete rectLock;rectLock=0;
		delete m_AllWndSizerRectangles;
		m_AllWndSizerRectangles=0;
	}
	if(hInternalWnd.m_hWnd){
		hInternalWnd.Detach();
	}
}

void CSmartWndSizer::InitSizer(HWND pMainWnd, int iStickness, HKEY hKeyRoot,const char* szKey)
{
	hInternalWnd.Attach(pMainWnd);
	InitSizer(&hInternalWnd, iStickness, hKeyRoot,szKey);
}

void CSmartWndSizer::InitSizer(CWnd* pMainWnd, int iStickness, HKEY hKeyRoot,const char* szKey)
{
	CSmartLocker lock(rectLock);
	if(!IsWindow(pMainWnd->GetSafeHwnd())){
		return;
	}
	m_rMain.SetRect(0,0,0,0);
	m_rMainMin.SetRect(0,0,0,0);
	m_rMainMax.SetRect(0,0,0,0);
	m_pMain=pMainWnd;
	m_hKeyRoot=hKeyRoot;
	m_szRegKey=szKey;
	m_iStickness=iStickness;
	SizerWnd pObj;
	pObj.pWnd=m_pMain;
	m_AllWndSizerRectangles->Add(pObj);
	iInit=2;
}

CSmartWndSizer& CSmartWndSizer::operator=(CSmartWndSizer& pCopy)
{
	m_pMain=pCopy.m_pMain;
	m_szRegKey=pCopy.m_szRegKey;
	m_iStickness=pCopy.m_iStickness;
	iInit=pCopy.iInit;
	m_rMainMin.CopyRect(pCopy.m_rMainMin);
	m_rMainMax.CopyRect(pCopy.m_rMainMax);
	m_dwOptions=pCopy.m_dwOptions;
	// Копируем элементы...
	int iKey;
	CSmartWndSizer_Element* pElem;
	POSITION p=m_WndElements.GetStartPosition();
	while(p!=NULL){
		m_WndElements.GetNextAssoc(p, iKey, pElem);
		delete pElem;
	}
	m_WndElements.RemoveAll();
	p=pCopy.m_WndElements.GetStartPosition();
	while(p!=NULL){
		pCopy.m_WndElements.GetNextAssoc(p, iKey, pElem);
		m_WndElements[iKey]=pElem->Copy();
	}
	// Копируем правила...
	for(int i=0;i<m_AlignRules.GetSize();i++){
		delete m_AlignRules[i];
	}
	m_AlignRules.RemoveAll();
	m_AlignRules.Copy(pCopy.m_AlignRules);
	// копируем метки...
	m_Labels.RemoveAll();
	DWORD dwKey, dwLabelID;
	p=pCopy.m_Labels.GetStartPosition();
	while(p!=NULL){
		pCopy.m_Labels.GetNextAssoc(p, dwKey, dwLabelID);
		m_Labels[dwKey]=dwLabelID;
	}
	return *this;
}

int CSmartWndSizer::AddDialogElement(int ID, DWORD dwFixed, int iFixedFrom)
{
	if(ID==0 || !iInit){
		return FALSE;
	}
	CSmartWndSizer_Element* bPrevElem=NULL;
	if(m_WndElements.Lookup(ID,bPrevElem) && bPrevElem!=NULL){
		delete bPrevElem;
	}
	m_WndElements.SetAt(ID,new CSmartWndSizer_Element(ID,dwFixed,iFixedFrom));
	if(dwFixed & transparent){
		CWnd* pWnd=m_pMain->GetDlgItem(ID);
		SetWindowLong(pWnd->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(pWnd->GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_TRANSPARENT);//WS_EX_COMPOSITED-0x02000000L
		pWnd->Invalidate();
	}
	return TRUE;
}

void CSmartWndSizer::SetStickness(int iStickness)
{
	m_iStickness=iStickness;
}

int CSmartWndSizer::GetWindowMenuHeightSW(BOOL bForceReCalc)
{
	if(m_pMain->GetMenu() && (iMenuH==-1 || bForceReCalc)){
		MENUBARINFO pmbi;
		memset(&pmbi,0,sizeof(pmbi));
		pmbi.cbSize=sizeof(pmbi);
		static HINSTANCE hUser32=HINSTANCE(-1);
		if(hUser32==HINSTANCE(-1)){
			hUser32=GetModuleHandle("user32.dll");
		}
		if(hUser32){
			static _GetMenuBarInfo fp=_GetMenuBarInfo(-1);
			if(fp==_GetMenuBarInfo(-1)){
				fp=(_GetMenuBarInfo)::GetProcAddress(hUser32,"GetMenuBarInfo");
			}
			if(fp){
				(*fp)(m_pMain->m_hWnd,0xFFFFFFFD,0,&pmbi);//OBJID_MENU
				iMenuH=(pmbi.rcBar.bottom-pmbi.rcBar.top);
			}
		}
	}
	return iMenuH;
}

void CSmartWndSizer::UpdateClientOffsets()
{
	iMenuH=-1;
	CRect rtClient;
	m_pMain->GetClientRect(&rtClient);
	m_pMain->ClientToScreen(&rtClient);
	lClientOffsetL=rtClient.left-m_rMain.left;
	lClientOffsetR=m_rMain.right-rtClient.right;//-GetSystemMetrics(SM_CXDLGFRAME);SM_CYSIZEFRAME
	lClientOffsetT=rtClient.top-m_rMain.top;
	lClientOffsetB=m_rMain.bottom-rtClient.bottom;//-GetSystemMetrics(SM_CYDLGFRAME);
	// Если подчиненное окно находится за пределами главного - игнорируем его offset!
	if(lClientOffsetR<0){
		lClientOffsetR=0;
		lClientOffsetL=0;
	}
	if(lClientOffsetB<0){
		lClientOffsetB=0;
		lClientOffsetT=0;
	}
}

BOOL CSmartWndSizer::SmartPRESetWindowPosition()
{
	iInPresetPosition++;
	// Считываем минимальный размер...
	BOOL bLoadRes=LoadSizeAndPosFromRegistry();
	if((m_dwOptions & FIXEDSIZE) && (m_rMainMin.Width()>0 || m_rMainMin.Height()>0)){
		if(m_rMainMin.Width()>0){
			m_rMain.right=m_rMain.left+m_rMainMin.Width();
		}
		if(m_rMainMin.Height()>0){
			m_rMain.bottom=m_rMain.top+m_rMainMin.Height();
		}
	}
	if(!m_rMainMax.IsRectEmpty()){
		if(m_rMain.Width()>m_rMainMax.Width()){
			m_rMain.right=m_rMain.left+m_rMainMax.Width();
		}
		if(m_rMain.Height()>m_rMainMax.Height()){
			m_rMain.bottom=m_rMain.top+m_rMainMax.Height();
		}
	}
	if(m_pMain==NULL || !::IsWindow(m_pMain->m_hWnd)){
		return FALSE;
	}
	/*
	if(m_dwOptions & DOUBLEBUFFER){
		SetWindowLong(m_pMain->m_hWnd,GWL_EXSTYLE,GetWindowLong(m_pMain->m_hWnd,GWL_EXSTYLE)|0x02000000L);//WS_EX_COMPOSITED-0x02000000L
	}
	*/
	iInit=1;
	RECT rect;
	m_pMain->GetWindowRect(&rect);
	if(m_rMain.IsRectEmpty()){
		m_rMain.CopyRect(&rect);
	};
	if(m_dwOptions & CENTERUNDERMOUSE){
		CPoint p;
		::GetCursorPos(&p);
		long w=m_rMain.Width();
		m_rMain.left=p.x-w/2;
		m_rMain.right=p.x+w/2;
		long h=m_rMain.Height();
		m_rMain.top=p.y-h/2;
		m_rMain.bottom=p.y+h/2;
		TestOnScreenOut(m_rMain,m_iStickness,1,1);
	}
	TestStickness(m_rMain, FALSE);
	m_pMain->MoveWindow(m_rMain);
	if(bMaximized){
		m_pMain->SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);
	}
	// Запоминаем соотношение между клиентской и всем окном
	UpdateClientOffsets();
	ADDDEBUGCHAR("1");
	ApplyLayout();
	//------------------------------------------
	iInPresetPosition--;
	return TRUE;
}

int CSmartWndSizer::ApplyShowRules(BOOL bFullCheck)
{// Скрываем/показываем окна
	if(iInPresetPosition){
		return 0;
	}
	int iID=0;
	CSmartWndSizer_Element* pControlWnd;
	POSITION p=m_WndElements.GetStartPosition();
	while(p){
		m_WndElements.GetNextAssoc(p,iID,pControlWnd);
		if(pControlWnd && (pControlWnd->m_dwFixed & alwaysInvisible)!=0){
			CWnd* pControl=m_pMain->GetDlgItem(pControlWnd->m_ID);
			pControl->ShowWindow(SW_HIDE);
		}
		if(pControlWnd && (pControlWnd->m_dwFixed & hidable)!=0){
			CWnd* pControl=m_pMain->GetDlgItem(pControlWnd->m_ID);
			if(pControl && (bFullCheck?(::IsWindow(pControl->GetSafeHwnd())):TRUE)){
				BOOL bIntersected=IsWindowIntersected(iID,bFullCheck,TRUE);
				BOOL bVisible=bFullCheck?(::IsWindowVisible(pControl->GetSafeHwnd())):TRUE;
				BOOL bStateSet=-1;
				if(bIntersected && bVisible){
					bStateSet=SW_HIDE;
				}
				if(!bIntersected && !bVisible && pControlWnd->m_bCanBeVisible){
					bStateSet=SW_SHOW;
				}
				if(bStateSet!=-1){
					pControl->ShowWindow(bStateSet);
				}
			}
		}
	}
	return 1;
}

int CSmartWndSizer::ApplyLayout(BOOL bClearCash, BOOL bCheckIfOnlyRules)
{
	if(iInit==0 || !::IsWindow(m_pMain->m_hWnd)){
		return FALSE;
	};
	if((m_dwOptions & SIZEABLE)==0 && (m_dwOptions & FIXEDXSIZE)==0 && (m_dwOptions & FIXEDYSIZE)==0 && (m_dwOptions & FIXEDSIZE)==0){
		return FALSE;
	}
	if(bClearCash){
		// Очищаем кеш местоположений
		int iKey;
		CSmartWndSizer_Element* pElem;
		POSITION p=m_WndElements.GetStartPosition();
		while(p!=NULL){
			m_WndElements.GetNextAssoc(p, iKey, pElem);
			pElem->m_Rect.top=-1;
		}
	}
	BOOL bIfOnlyRuleTriggered=0;
	int i=0;
	for(i=0;i<m_AlignRules.GetSize();i++){
		////////////////
		// Подготовка...
		CSmartWndSizer_AlignRule* pCurRule=m_AlignRules[i];
		////////////////
		// Переходы если есть...
		if(pCurRule->m_GotoLabel!=-1){
			DWORD dwNewIndex;
			if(m_Labels.Lookup(pCurRule->m_GotoLabel,dwNewIndex)){
				i=dwNewIndex-1;
				continue;
			}
		}
		if(pCurRule->m_Level!=0 && (pCurRule->m_Level & dwSizerLevel)==0){
			// не тот уровень...
			continue;
		}
		////////////////
		CRect rTo;
		CRect rFrom;
		CSmartWndSizer_Element* pWnd;
		//////////////////
		// Откуда брать...
		if(pCurRule->m_TakeFrom==0){
			m_pMain->GetClientRect(&rFrom);
			//rFrom.bottom-=lClientOffsetB;
			//rFrom.right-=lClientOffsetR;
			/*
			rFrom.CopyRect(m_rMain);
			rFrom.OffsetRect(-m_rMain.left,-m_rMain.top);
			rFrom.bottom-=lClientOffsetT+lClientOffsetB;
			rFrom.right-=lClientOffsetL+lClientOffsetR;
			*/
		}else{
			if(!m_WndElements.Lookup(pCurRule->m_TakeFrom,pWnd)){
				continue;
			}
			if(pWnd->m_Rect.top==-1){
				m_pMain->GetDlgItem(pWnd->m_ID)->GetWindowRect(rFrom);
				CRect rScreenBase;
				m_pMain->GetWindowRect(rScreenBase);
				rFrom.OffsetRect(-rScreenBase.left,-rScreenBase.top);
				pWnd->m_Rect=rFrom;
			}else{
				rFrom=pWnd->m_Rect;
			}
		}
		/////////////////
		// Куда ложить...
		if(pCurRule->m_TargetElement==0){
			// Нельзя применять align к главному окну
			continue;
		}
		CSmartWndSizer_Element* pToWnd=NULL;
		if(!m_WndElements.Lookup(pCurRule->m_TargetElement,pToWnd)){
			continue;
		}
		CWnd* p_IDWnd=m_pMain->GetDlgItem(pToWnd->m_ID);
		if(!p_IDWnd || !::IsWindow(p_IDWnd->GetSafeHwnd())){
			continue;
		}
		if(pToWnd->m_Rect.top==-1){
			p_IDWnd->GetWindowRect(rTo);
			m_pMain->ScreenToClient(rTo);
			pToWnd->m_Rect=rTo;
		}else{
			rTo=pToWnd->m_Rect;
		}
		/////////////////////////////////
		// Фиксировать параметры с чем...
		DWORD dwFixes=pToWnd->m_dwFixed;
		CRect rFixedFrom;
		if(pToWnd->m_iFixedFrom==0){
			// оригинальные значения...
			rFixedFrom=rTo;
		}else{
			// неоригинальные значения...
			if((pToWnd->m_dwFixed) & fixedDimensions){
				// Жестко заданное значение...
				rFixedFrom=rTo;
				rFixedFrom.right=rFixedFrom.left+pToWnd->m_iFixedFrom;
				rFixedFrom.bottom=rFixedFrom.top+pToWnd->m_iFixedFrom;
			}else{
				// С другого элемента...
				CSmartWndSizer_Element* pFixedFrom=NULL;
				if(!m_WndElements.Lookup(pToWnd->m_iFixedFrom,pFixedFrom)){
					continue;
				}
				if(pFixedFrom->m_Rect.top==-1){
					(m_pMain->GetDlgItem(pFixedFrom->m_ID))->GetWindowRect(rFixedFrom);
					m_pMain->ScreenToClient(rFixedFrom);
					pFixedFrom->m_Rect=rFixedFrom;
				}else{
					rFixedFrom=pFixedFrom->m_Rect;
				}
			}
		}
		///////////////////////
		// Вытаскиваем значения
		BOOL bIfOnlyBigger=(pCurRule->m_TakeWhat & ifBigger);
		BOOL bIfOnlyLess=(pCurRule->m_TakeWhat & ifLess);
		if(!bCheckIfOnlyRules && (bIfOnlyLess|bIfOnlyBigger)){
			continue;
		}
		long lValue=0;
		
		if((pCurRule->m_TakeWhat & visibility)!=0 && p_IDWnd){
			if(pCurRule->m_Spacing){
				if(!p_IDWnd->IsWindowVisible()){
					pToWnd->m_bCanBeVisible=TRUE;
					//CString sT;p_IDWnd->GetWindowText(sT);
					p_IDWnd->ShowWindow(SW_SHOW);
				}
			}else{
				if(p_IDWnd->IsWindowVisible()){
					pToWnd->m_bCanBeVisible=FALSE;
					p_IDWnd->ShowWindow(SW_HIDE);
				}
			}
			continue;
		}else if(pCurRule->m_TakeWhat & leftPos){
			lValue=rFrom.left;
		}else if(pCurRule->m_TakeWhat & rightPos){
			lValue=rFrom.right;
		}else if(pCurRule->m_TakeWhat & topPos){
			lValue=rFrom.top;
		}else if(pCurRule->m_TakeWhat & bottomPos){
			lValue=rFrom.bottom;
		}else if(pCurRule->m_TakeWhat & widthSize){
			lValue=rFrom.Width();
		}else if(pCurRule->m_TakeWhat & heightSize){
			lValue=rFrom.Height();
		}else if(pCurRule->m_TakeWhat & topCenter){
			lValue=rFrom.left+(rFrom.Width()/2);
		}else if(pCurRule->m_TakeWhat & sideCenter){
			lValue=rFrom.top+(rFrom.Height()/2);
		}else{
			// Странно ... сюда попадать не должно!
		}
		lValue=lValue+pCurRule->m_Spacing;
		//////////////
		// Выравниваем
		if(pCurRule->m_ApplyToWhat & leftPos){
			if(bIfOnlyBigger){
				if(rTo.left<lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			if(bIfOnlyLess){
				if(rTo.left>lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			rTo.left=lValue;
			if(!bIfOnlyRuleTriggered && (dwFixes & widthSize)){
				rTo.right=rTo.left+rFixedFrom.Width();
			}
		}
		if(pCurRule->m_ApplyToWhat & rightPos){
			if(bIfOnlyBigger){
				if(rTo.right<lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			if(bIfOnlyLess){
				if(rTo.right>lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			rTo.right=lValue;
			if(!bIfOnlyRuleTriggered && (dwFixes & widthSize)){
				rTo.left=rTo.right-rFixedFrom.Width();
			}
		}
		if(pCurRule->m_ApplyToWhat & topPos){
			if(bIfOnlyBigger){
				if(rTo.top<lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			if(bIfOnlyLess){
				if(rTo.top>lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			rTo.top=lValue;
			if(!bIfOnlyRuleTriggered && (dwFixes & heightSize)){
				rTo.bottom=rTo.top+rFixedFrom.Height();
			}
		}
		if(pCurRule->m_ApplyToWhat & bottomPos){
			if(bIfOnlyBigger){
				if(rTo.bottom<lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			if(bIfOnlyLess){
				if(rTo.bottom>lValue){
					continue;
				}
				bIfOnlyRuleTriggered=1;
			}
			rTo.bottom=lValue;
			if(!bIfOnlyRuleTriggered && (dwFixes & heightSize)){
				rTo.top=rTo.bottom-rFixedFrom.Height();
			}
		}
		if(pCurRule->m_ApplyToWhat & widthSize){
			if(dwFixes & rightPos){
				rTo.left=rTo.right-lValue;
			}else{
				rTo.right=rTo.left+lValue;
			}
		}
		if(pCurRule->m_ApplyToWhat & heightSize){
			if(dwFixes & bottomPos){
				rTo.top=rTo.bottom-lValue;
			}else{
				rTo.bottom=rTo.top+lValue;
			}
		}
		if(pCurRule->m_ApplyToWhat & topCenter){
			rTo.left=lValue-(rTo.Width()/2);
			rTo.right=rTo.left+rFixedFrom.Width();
		}
		if(pCurRule->m_ApplyToWhat & sideCenter){
			rTo.top=lValue-(rTo.Height()/2);
			rTo.bottom=rTo.top+rFixedFrom.Height();
		}
		pToWnd->m_Rect=rTo;
		p_IDWnd->MoveWindow(rTo,FALSE);
#ifdef _DEBUG
		{
			CRect rtNew(rTo),rtNewMain(m_rMain);
			m_pMain->ClientToScreen(rtNew);
		}
#endif
	}
	int iRes=0;
	if(bIfOnlyRuleTriggered){
		ADDDEBUGCHAR("2");
		iRes=ApplyLayout(FALSE,FALSE);
	}else{
		ApplyShowRules(TRUE);
		iRes=ApplyStickness(FALSE,!bClearCash);
		RedrawWindowFlickerFree(m_pMain);
	}
	return iRes;
}

int CSmartWndSizer::RedrawWindowFlickerFree(CWnd* pWnd)
{
	if(m_dwOptions & SETCLIPSIBL){
		int iID=0;
		pWnd->RedrawWindow();
		CSmartWndSizer_Element* pControlWnd;
		POSITION p=m_WndElements.GetStartPosition();
		while(p){
			m_WndElements.GetNextAssoc(p,iID,pControlWnd);
			if(pControlWnd){
				CWnd* pControl=m_pMain->GetDlgItem(pControlWnd->m_ID);
				if(pControl && ::IsWindowVisible(pControl->GetSafeHwnd())){
					pControl->RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);//| RDW_ALLCHILDREN | RDW_ERASENOW
				}
			}
		}
		return TRUE;
	}
	if(bSlowMachine || (m_dwOptions & NODOUBLEBUFFER)!=0){// || (GetWindowLong(pWnd->GetSafeHwnd(), GWL_EXSTYLE) & (0x00080000L)!=0)){
		pWnd->RedrawWindow();
		return TRUE;
	}
	if(!pWnd || !IsWindow(pWnd->GetSafeHwnd())){
		return FALSE;
	}
	if(pWnd->IsWindowVisible()){
		CDC* pDC=pWnd->GetWindowDC();
		if(!pDC){
			return FALSE;
		}
		CRect rt,rtWin;
		pWnd->GetClientRect(&rt);
		pWnd->GetWindowRect(&rtWin);
		CDC dcMem;
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC,rtWin.Width(),rtWin.Height());
		dcMem.CreateCompatibleDC(pDC);
		CBitmap* bmpOld=dcMem.SelectObject(&bmp);
		pWnd->SendMessage(WM_PRINT,WPARAM(dcMem.GetSafeHdc()),LPARAM(PRF_CHECKVISIBLE|PRF_OWNED|PRF_CHILDREN|PRF_CLIENT|PRF_ERASEBKGND|PRF_NONCLIENT));
		pDC->BitBlt(rt.left,rt.top,rtWin.Width(),rtWin.Height(),&dcMem,0,0,SRCCOPY);
		dcMem.SelectObject(bmpOld);
		pWnd->ReleaseDC(pDC);
		pWnd->ValidateRect(&rt);
	}
	return TRUE;
}

int CSmartWndSizer::IsWindowIntersected(int iIDWith, BOOL bFullCheck, BOOL bWithMainWindow)
{
	CSmartWndSizer_Element* pLookWith=NULL;
	if(!m_WndElements.Lookup(iIDWith, pLookWith)){
		return 0;
	}
	CRect rtWith=pLookWith->m_Rect;
	rtWith.InflateRect(m_iSaWi,m_iSaWi);
	if(bWithMainWindow){
		if(rtWith.left<0 || rtWith.top<0){
			return 1;
		}
		CRect rt2;
		m_pMain->GetClientRect(&rt2);
		int iW=rt2.Width(),iH=rt2.Height();
		if(rtWith.right>iW || rtWith.bottom>iH){
			return 1;
		}
	}
	CSmartWndSizer_Element* pControlWnd=NULL;
	int iID=0;
	POSITION p=m_WndElements.GetStartPosition();
	while(p){
		m_WndElements.GetNextAssoc(p,iID,pControlWnd);
		if(pControlWnd && iID!=iIDWith){
			if(pControlWnd && pControlWnd->m_Rect.left>0){
				CWnd* pWndWith=m_pMain->GetDlgItem(iID);
				if(pWndWith){
					BOOL bWithOK=1;
					if(bFullCheck && (pControlWnd->m_dwFixed & alwaysVisible)==0){
						BOOL bIsWnd=::IsWindow(pWndWith->GetSafeHwnd());
						BOOL bVisible=::IsWindowVisible(pWndWith->GetSafeHwnd());
						bWithOK=(bIsWnd && bVisible);
					}
					if(bWithOK){
						CRect inter(0,0,0,0);
						inter.IntersectRect(rtWith,pControlWnd->m_Rect);
						if(!inter.IsRectEmpty()){
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}

int CSmartWndSizer::TestOnScreenOut(CRect& CurDialogRect, int Stickness, BOOL bForceInScreen, BOOL bBestMonitorForMM)
{
	BOOL bOnMulty=FALSE;
	CRect CurDialogRectCp(CurDialogRect);
	DesktopDialogRect=GetScreenRect(&CurDialogRectCp,&bOnMulty,bBestMonitorForMM);
	int iRightEdge=DesktopDialogRect.right-CurDialogRect.Width();
	int iBottomEdge=DesktopDialogRect.bottom-CurDialogRect.Height();
	if(bForceInScreen || (!bOnMulty && IsScreenBordersAreFAT()==1)){
		if(CurDialogRect.left>iRightEdge-Stickness){
			CurDialogRect.left=iRightEdge;
		}
		if(CurDialogRect.left<DesktopDialogRect.left+Stickness){
			CurDialogRect.left=DesktopDialogRect.left;
		}
		if(CurDialogRect.top>iBottomEdge-Stickness){
			CurDialogRect.top=iBottomEdge;
		}
		if(CurDialogRect.top<DesktopDialogRect.top+Stickness){
			CurDialogRect.top=DesktopDialogRect.top;
		}
	}else{
		if(abs(CurDialogRect.left-iRightEdge)<Stickness){
			CurDialogRect.left=iRightEdge;
		}
		if(abs(CurDialogRect.left-DesktopDialogRect.left)<Stickness){
			CurDialogRect.left=DesktopDialogRect.left;
		}
		if(abs(CurDialogRect.top-iBottomEdge)<Stickness){
			CurDialogRect.top=iBottomEdge;
		}
		if(abs(CurDialogRect.top-DesktopDialogRect.top)<Stickness){
			CurDialogRect.top=DesktopDialogRect.top;
		}
	}
	CurDialogRect.right=CurDialogRect.left+CurDialogRectCp.Width();
	CurDialogRect.bottom=CurDialogRect.top+CurDialogRectCp.Height();
	return (CurDialogRect.left!=CurDialogRectCp.left ||	CurDialogRect.top!=CurDialogRectCp.top || CurDialogRect.right!=CurDialogRectCp.right ||	CurDialogRect.bottom!=CurDialogRectCp.bottom);
}

int CSmartWndSizer::IsRectStickedTogether(CRect rect1, CRect rect2, int iStickness)
{
	if (iStickness==0){
		iStickness=m_iStickness;
	}
	// Пересекать будем с расширенным прямоугольником
	// чтобы окно притягивалось и снаружи тоже
	CRect IntersectWith(rect2);
	IntersectWith.InflateRect(iStickness,iStickness,iStickness,iStickness);
	CRect Intersect;
	if(Intersect.IntersectRect(rect1,IntersectWith)!=0){
		return 1;
	}
	return 0;
}

int CSmartWndSizer::TestStickness(CRect& CurDialogRect, BOOL bTestVisibleOnly)
{
	if(iInit==0 || !::IsWindow(m_pMain->m_hWnd) || ((m_dwOptions & STICKABLE)==0)){
		return 0;
	};
	if(bTestVisibleOnly && !::IsWindowVisible(m_pMain->m_hWnd)){
		return 0;
	};
	if(CurDialogRect.IsRectEmpty()){
		return 0;
	}
	BOOL bWindowStickToMinSize=0;
	CRect rMinimumRect(m_rMainMin);
	if(CurDialogRect.Height()<rMinimumRect.Height()){
		CurDialogRect.bottom=CurDialogRect.top+rMinimumRect.Height();
		bWindowStickToMinSize=TRUE;
	}
	if(CurDialogRect.Width()<rMinimumRect.Width()){
		CurDialogRect.right=CurDialogRect.left+rMinimumRect.Width();
		bWindowStickToMinSize=TRUE;
	}

	CRect rMaximumRect(m_rMainMax);
	if(!rMaximumRect.IsRectEmpty()){
		if(CurDialogRect.Height()>rMaximumRect.Height()){
			CurDialogRect.bottom=CurDialogRect.top+rMaximumRect.Height();
			bWindowStickToMinSize=TRUE;
		}
		if(CurDialogRect.Width()>rMaximumRect.Width()){
			CurDialogRect.right=CurDialogRect.left+rMaximumRect.Width();
			bWindowStickToMinSize=TRUE;
		}
	}
	if((m_dwOptions & FIXEDYSIZE) && CurDialogRect.Height()!=m_rMainMin.Height()){
		CurDialogRect.bottom=CurDialogRect.top+m_rMainMin.Height();
		bWindowStickToMinSize=TRUE;
	}
	if((m_dwOptions & FIXEDXSIZE) && CurDialogRect.Width()!=m_rMainMin.Width()){
		CurDialogRect.right=CurDialogRect.left+m_rMainMin.Width();
		bWindowStickToMinSize=TRUE;
	}
	BOOL bWindowMovedInScreen=bMaximized?0:TestOnScreenOut(CurDialogRect,m_iStickness);
	if(m_iStickness<0){
		return bWindowMovedInScreen;
	}
	// Прилипание к другим окнам...
	BOOL bTwiced=FALSE;
	int iWidth=CurDialogRect.Width();
	int iHeight=CurDialogRect.Height();
	CRect CurDialogRectCp(CurDialogRect);
	{
		CSmartLocker lock(rectLock);
		for(int i=0;i<m_AllWndSizerRectangles->GetSize();i++){
			CWnd* pCompWith=(*m_AllWndSizerRectangles)[i].pWnd;
			if(pCompWith==NULL || pCompWith==m_pMain || !IsWindow(pCompWith->m_hWnd) || !(pCompWith->IsWindowVisible())){
				continue;
			}
			CRect rCompWith;
			pCompWith->GetWindowRect(rCompWith);
			if(rCompWith.IsRectEmpty()){
				continue;
			}
			if(IsRectStickedTogether(CurDialogRect,rCompWith)==0){
				continue;
			}
			bTwiced=FALSE;
			// Внутри ....
			if(abs(CurDialogRect.left-rCompWith.left)<m_iStickness){
				CurDialogRect.left=rCompWith.left;
				CurDialogRect.right=CurDialogRect.left+iWidth;
				bTwiced=TRUE;
			}
			if(abs(CurDialogRect.right-rCompWith.right)<m_iStickness){
				CurDialogRect.right=rCompWith.right;
				if(!bTwiced){
					CurDialogRect.left=CurDialogRect.right-iWidth;
				}
			}
			bTwiced=FALSE;
			if(abs(CurDialogRect.top-rCompWith.top)<m_iStickness){
				CurDialogRect.top=rCompWith.top;
				CurDialogRect.bottom=CurDialogRect.top+iHeight;
				bTwiced=TRUE;
			}
			if(abs(CurDialogRect.bottom-rCompWith.bottom)<m_iStickness){
				CurDialogRect.bottom=rCompWith.bottom;
				if(!bTwiced){
					CurDialogRect.top=CurDialogRect.bottom-iHeight;
				}
			}
			// Снаружи...
			bTwiced=FALSE;
			if(abs(CurDialogRect.left-rCompWith.right)<m_iStickness){
				CurDialogRect.left=rCompWith.right;
				CurDialogRect.right=CurDialogRect.left+iWidth;
				bTwiced=TRUE;
			}
			if(abs(CurDialogRect.right-rCompWith.left)<m_iStickness){
				CurDialogRect.right=rCompWith.left;
				if(!bTwiced){
					CurDialogRect.left=CurDialogRect.right-iWidth;
				}
			}
			bTwiced=FALSE;
			if(abs(CurDialogRect.top-rCompWith.bottom)<m_iStickness){
				CurDialogRect.top=rCompWith.bottom;
				CurDialogRect.bottom=CurDialogRect.top+iHeight;
				bTwiced=TRUE;
			}
			if(abs(CurDialogRect.bottom-rCompWith.top)<m_iStickness){
				CurDialogRect.bottom=rCompWith.top;
				if(!bTwiced){
					CurDialogRect.top=CurDialogRect.bottom-iHeight;
				}
			}
		}
	}
	BOOL bWindowMoved=(CurDialogRect.left!=CurDialogRectCp.left ||	CurDialogRect.top!=CurDialogRectCp.top || CurDialogRect.right!=CurDialogRectCp.right ||	CurDialogRect.bottom!=CurDialogRectCp.bottom);
	return (bWindowMovedInScreen?windowStickToScreen:0)|(bWindowMoved?windowStickToWnd:0)|(bWindowStickToMinSize?windowStickToMinSize:0);
}

int CSmartWndSizer::ApplyStickness(BOOL bRedraw, BOOL bUseCashedRect)
{
	if(iInit==0 || !::IsWindow(m_pMain->m_hWnd) || !::IsWindowVisible(m_pMain->m_hWnd) || ((m_dwOptions & STICKABLE)==0)){
		return FALSE;
	};
	// Прилипание к краям экрана...
	CRect CurDialogRect;
	if(bUseCashedRect){
		CurDialogRect=m_rMain;
	}else{
		m_pMain->GetWindowRect(CurDialogRect);
	}
	BOOL bMoved=TestStickness(CurDialogRect);
	if(bMoved!=0 && 
		(CurDialogRect.left!=m_rMain.left || CurDialogRect.top!=m_rMain.top ||
		CurDialogRect.right!=m_rMain.right || CurDialogRect.bottom!=m_rMain.bottom)){
		m_pMain->MoveWindow(CurDialogRect,TRUE);
		// Берем положение основного окна для сохранения...
		m_rMain.CopyRect(CurDialogRect);
	}
	return bMoved;
}

int CSmartWndSizer::SaveSizeAndPosToRegistry()
{
	CString sKeyName=m_szRegKey;
	if(m_pMain==NULL || sKeyName==""){
		return 0;
	}
	int iPos=sKeyName.ReverseFind('\\');
	if(iPos==-1){
		return 0;
	}
	if(m_pMain && IsWindow(m_pMain->GetSafeHwnd())){
		bMaximized=IsZoomed(m_pMain->GetSafeHwnd());
	}
	CString sValueName=sKeyName.Mid(iPos+1);
	if(m_pMain && ::IsWindow(m_pMain->GetSafeHwnd())){
		if(bMaximized!=0){
			WINDOWPLACEMENT wndpl;
			m_pMain->GetWindowPlacement(&wndpl);
			m_rMain.CopyRect(&wndpl.rcNormalPosition);
		}else{
			m_pMain->GetWindowRect(&m_rMain);
		}
	}
	CRect rtAll(GetAllMonitorsRECT());
	if(m_rMain.left<rtAll.left || m_rMain.top<rtAll.top
		|| m_rMain.right>rtAll.right || m_rMain.bottom>rtAll.bottom){
		// Не сохраняем плохих данных
		return 0;
	}
	sKeyName=sKeyName.Left(iPos);
	char szValue[256];
	sprintf(szValue,"x={%i};y={%i};x1={%i};y1={%i}",m_rMain.left,m_rMain.top,m_rMain.right,m_rMain.bottom);
	if(bMaximized){
		strcat(szValue,"{MAX}");
	}
	CRegKey key;
	key.Create(m_hKeyRoot, sKeyName);
	DWORD dwType=REG_SZ,dwSize=0;
	if(RegSetValueEx(key.m_hKey,sValueName,0,REG_SZ,(BYTE*)szValue,lstrlen(szValue)+1)!= ERROR_SUCCESS){
		return 0;
	}
	return 1;
}

/*
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
*/
CRect GetAllMonitorsRECT()
{
	CRect rDesktopRECT(GetSystemMetrics(76),GetSystemMetrics(77),GetSystemMetrics(78),GetSystemMetrics(79));
	if(rDesktopRECT.IsRectEmpty() || GetSystemMetrics(80)<=1){
		::SystemParametersInfo(SPI_GETWORKAREA,0,&rDesktopRECT,0);
	}
	return rDesktopRECT;
}

int CSmartWndSizer::LoadSizeAndPosFromRegistry()
{
	if(m_pMain==NULL){
		return 0;
	}
	CString sKeyName=m_szRegKey;
	int iPos=sKeyName.ReverseFind('\\');
	if(iPos==-1){
		return 0;
	}
	CString sValueName=sKeyName.Mid(iPos+1);
	sKeyName=sKeyName.Left(iPos);
	char szValue[256]={0};
	CRegKey key;
	key.Create(m_hKeyRoot, sKeyName);
	DWORD dwCount=sizeof(szValue);
	DWORD dwType=0;
	if(RegQueryValueEx(key.m_hKey,sValueName,NULL, &dwType,(LPBYTE)szValue, &dwCount)!= ERROR_SUCCESS){
		return 0;
	}
	CRect m_rMainTmp;
	char* szPos=strstr(szValue,"{");
	if(!szPos) return 0; else szPos++;
	m_rMainTmp.left=atol(szPos);
	szPos=strstr(szPos,"{");
	if(!szPos) return 0; else szPos++;
	m_rMainTmp.top=atol(szPos);
	szPos=strstr(szPos,"{");
	if(!szPos) return 0; else szPos++;
	m_rMainTmp.right=atol(szPos);
	szPos=strstr(szPos,"{");
	if(!szPos) return 0; else szPos++;
	m_rMainTmp.bottom=atol(szPos);
	m_rMain.CopyRect(m_rMainTmp);
	// Должно попадать в основной десктоп
	CRect rtAll(GetAllMonitorsRECT());
	if(m_rMain.left<rtAll.left){
		m_rMain.OffsetRect(rtAll.left-m_rMain.left,0);
	}
	if(m_rMain.top<rtAll.top){
		m_rMain.OffsetRect(0,rtAll.top-m_rMain.top);
	}
	if(strstr(szValue,"{MAX}")){
		bMaximized=1;
	}
	return 1;
}

class SimpleTracker
{
public:
	long* bValue;
	SimpleTracker(long& b){
		bValue=&b;
		InterlockedIncrement(bValue);
	};
	~SimpleTracker(){
		InterlockedDecrement(bValue);
	};
};
#define CSimpleLock	SimpleTracker

BOOL CSmartWndSizer::ApllyAndRefresh(BOOL bNotUsingSys)
{
	static BOOL bNoRepaint=-1;
	if(bNoRepaint==-1){
		::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS,0,&bNoRepaint,0);
	}
	if(bNotUsingSys || bNoRepaint){
		ApplyLayout();
	}
	return TRUE;
}

#define RESIZE_SPOT	5
BOOL CSmartWndSizer::HandleWndMesages(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& DispatchResultFromSizer)
{
	if(bMessageRecursingPreventer>0){
		// Чтобы не входить в штопор от сообщений,
		// которые порождаются при обработке др. сообщений
		return FALSE;
	}
	SimpleTracker track(bMessageRecursingPreventer);
	//------------------------
	DispatchResultFromSizer=0;
	if((message==WM_SHOWWINDOW && wParam==TRUE) || message==WM_ACTIVATE || message==WM_NCCREATE || (message==WM_WINDOWPOSCHANGED && iInit==2)){
		if(iInit==2){
			// сюда должно попадать только один раз
			SmartPRESetWindowPosition();
			ApplyShowRules(FALSE);
		}
	}
	if((message==WM_DESTROY || message==WM_CLOSE) && iInit==1){// Если не был инициализирован, то и сохранять нечего!
		SaveSizeAndPosToRegistry();
	}
	if(message==WM_SYSCOMMAND && wParam==SC_MAXIMIZE){
		bMaximized=1;
	}
	if(message==WM_SYSCOMMAND && wParam==SC_RESTORE){
		bMaximized=0;
	}
	if((message==WM_WINDOWPOSCHANGING || message==WM_SIZE || message==WM_SIZING) && iInit==1){
		if(int(GetProp(m_pMain->GetSafeHwnd(),"ALLOW_OFFSCR"))!=1){
			CRect rect(m_rMain);
			WINDOWPOS* pos=(WINDOWPOS*)lParam;
			if(((message==WM_WINDOWPOSCHANGING || message==WM_WINDOWPOSCHANGED)&& (m_dwOptions & FIXEDPOS)) || ((message==WM_SIZING || message==WM_SIZE)&& (m_dwOptions & FIXEDSIZE))){
				if(IsWindow(m_pMain->GetSafeHwnd()) && IsWindowVisible(m_pMain->GetSafeHwnd())){
					pos->x=rect.left;
					pos->y=rect.top;
					pos->cx=rect.Width();
					pos->cy=rect.Height();
					ADDDEBUGCHAR("a");
					ApllyAndRefresh((message==WM_WINDOWPOSCHANGED || message==WM_SIZE));
					return TRUE;
				}
			}
			BOOL bFollow=::GetKeyState(VK_CONTROL)<0 && ::GetKeyState(VK_MENU)<0;
			if(!bFollow){
				bFollowModeEnabled=FALSE;
			}
			if(message==WM_WINDOWPOSCHANGING){
				rect.SetRect(pos->x,pos->y,pos->x+pos->cx,pos->y+pos->cy);
				CPoint pOffset(rect.left-m_rMain.left,rect.top-m_rMain.top);
				if(bFollow && !rect.IsRectEmpty() && !(pOffset.x==0 && pOffset.y==0) && !((pos->flags & SWP_NOMOVE) || (pos->flags & SWP_NOSIZE) || (pos->flags & SWP_NOSENDCHANGING))){
					FollowWndRect(m_rMain,pOffset);
				}
			}
			if(message==WM_SIZING && lParam && !bSkipResize){
				CRect rMessageRect=*((RECT*)lParam);
				if(rMessageRect.left!=m_rMain.left || rMessageRect.right!=m_rMain.right ||
					rMessageRect.top!=m_rMain.top || rMessageRect.bottom!=m_rMain.bottom){
						if((TestStickness(rMessageRect)&windowStickToMinSize)==0){
							m_rMain.CopyRect(rMessageRect);
							ADDDEBUGCHAR("b");
							ApllyAndRefresh(message==WM_SIZE);
						}
						rect.CopyRect(m_rMain);
					}
			}else if(message==WM_SIZE && lParam && !bSkipResize){
				WORD lWClientArea=LOWORD(lParam);
				WORD lHClientArea=HIWORD(lParam);
				DWORD dwMainClientBordersW=lClientOffsetR+lClientOffsetL;
				DWORD dwMainClientBordersH=lClientOffsetB+lClientOffsetT+GetWindowMenuHeightSW(TRUE);
				if(lWClientArea!=(m_rMain.Width()-dwMainClientBordersW) || lHClientArea!=(m_rMain.Height()-dwMainClientBordersH)){
					m_pMain->GetWindowRect(&m_rMain);
					rect.CopyRect(m_rMain);
					ADDDEBUGCHAR("d");
					ApllyAndRefresh(message==WM_SIZE);
				}
			}else if(message==WM_WINDOWPOSCHANGING && pos->cy!=0 && pos->cx!=0){
				if(!bFollowModeEnabled){// Если идет drag с follow, ничего не проверяем
					if(::GetKeyState(VK_SHIFT)>=0){// Если прилипание отключено, не проверяем
						CRect rectNew(rect);//pos->x,pos->y,pos->x+pos->cx,pos->y+pos->cy);
						if(TestStickness(rectNew)!=0){
							if(pos->x!=rectNew.left || pos->y!=rectNew.top || pos->cx!=rectNew.Width() || pos->cy!=rectNew.Height()){
								pos->x=rectNew.left;
								pos->y=rectNew.top;
								pos->cx=rectNew.Width();
								pos->cy=rectNew.Height();
								ADDDEBUGCHAR("?");
							}
						}
					}
				}
				if(!rect.IsRectEmpty() && rect!=m_rMain){
					m_rMain.CopyRect(rect);
				}
			}
			return TRUE;
		}
	}
	static CPoint pRevPT(-1,-1);
	if(message==WM_LBUTTONUP){
		bFollowModeEnabled=FALSE;
		if(bInResizingLock){
			if(hResizeCur){
				SetCursor(hResizeCur);
			}
			pRevPT.x=-1;
			pRevPT.y=-1;
			bInResizingLock=0;
			ReleaseCapture();
			return TRUE;
		}
	}
	if(message==WM_LBUTTONDOWN){
		if(pResizeAffectedWnd!=NULL){
			pRevPT.x=LOWORD(lParam);
			pRevPT.y=HIWORD(lParam);
			m_pMain->SetCapture();
			if(iAffectType<2){
				SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE)));
			}else{
				SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS)));
			}
			bInResizingLock=TRUE;
			return TRUE;
		}
	}
	if(message==WM_MOUSEMOVE && bResizingElementsArePresent!=0){
		CPoint pt(LOWORD(lParam),HIWORD(lParam));
		if(bInResizingLock && pResizeAffectedWnd){
			if(pRevPT.x>=0 && pRevPT.y>=0 && pt.x>=0 && pt.x<=m_rMain.Width() && pt.y>=0 && pt.y<=m_rMain.Height()){
				//&& (abs(pt.x-pResizeAffectedWnd->m_Rect.left)<RESIZE_SPOT || abs(pt.x-pResizeAffectedWnd->m_Rect.right)<RESIZE_SPOT || abs(pt.y-pResizeAffectedWnd->m_Rect.top)<RESIZE_SPOT || abs(pt.y-pResizeAffectedWnd->m_Rect.bottom)<RESIZE_SPOT)
				BOOL bChanged=FALSE;
				if(iAffectType==0){
					long lNew=pResizeAffectedWnd->m_Rect.left+pt.x-pRevPT.x;
					pResizeAffectedWnd->m_Rect.left=lNew;
					bChanged=TRUE;
				}else if(iAffectType==1){
					long lNew=pResizeAffectedWnd->m_Rect.right+pt.x-pRevPT.x;
					pResizeAffectedWnd->m_Rect.right=lNew;
					bChanged=TRUE;
				}else if(iAffectType==2){
					long lNew=pResizeAffectedWnd->m_Rect.top+pt.y-pRevPT.y;
					pResizeAffectedWnd->m_Rect.top=lNew;
					bChanged=TRUE;
				}else if(iAffectType==3){
					long lNew=pResizeAffectedWnd->m_Rect.bottom+pt.y-pRevPT.y;
					pResizeAffectedWnd->m_Rect.bottom=lNew;
					bChanged=TRUE;
				}
				if(bChanged){
					pRevPT=pt;
					m_pMain->GetDlgItem(pResizeAffectedWnd->m_ID)->MoveWindow(pResizeAffectedWnd->m_Rect);
					m_pMain->GetWindowRect(&m_rMain);
					ADDDEBUGCHAR("e");
					ApllyAndRefresh(1);
				}
				return TRUE;
			}
		}else{
			pResizeAffectedWnd=NULL;
			BOOL bNeedSetCursor=FALSE;
			int iID=0;
			iAffectType=-1;
			CSmartWndSizer_Element* pControlWnd=NULL;
			// Бегаем и ищем контрол, который можно сресайзить
			POSITION p=m_WndElements.GetStartPosition();
			while(p){
				m_WndElements.GetNextAssoc(p,iID,pControlWnd);
				if(pControlWnd->m_dwFixed & leftResize){
					bResizingElementsArePresent=1;
					if(pt.y>pControlWnd->m_Rect.top && pt.y<pControlWnd->m_Rect.bottom){
						if(abs(pt.x-pControlWnd->m_Rect.left)<RESIZE_SPOT){
							pResizeAffectedWnd=pControlWnd;
							iAffectType=0;
							break;
						}
					}
				}
				if(pControlWnd->m_dwFixed & rightResize){
					bResizingElementsArePresent=1;
					if(pt.y>pControlWnd->m_Rect.top && pt.y<pControlWnd->m_Rect.bottom){
						if(abs(pt.x-pControlWnd->m_Rect.right)<RESIZE_SPOT){
							pResizeAffectedWnd=pControlWnd;
							iAffectType=1;
							break;
						}
					}
				}
				if(pControlWnd->m_dwFixed & topResize){
					bResizingElementsArePresent=1;
					if(pt.x>pControlWnd->m_Rect.left && pt.x<pControlWnd->m_Rect.right){
						if(abs(pt.y-pControlWnd->m_Rect.top)<RESIZE_SPOT){
							pResizeAffectedWnd=pControlWnd;
							iAffectType=2;
							break;
						}
					}
				}
				if(pControlWnd->m_dwFixed & bottomResize){
					bResizingElementsArePresent=1;
					if(pt.x>pControlWnd->m_Rect.left && pt.x<pControlWnd->m_Rect.right){
						if(abs(pt.y-pControlWnd->m_Rect.bottom)<RESIZE_SPOT){
							pResizeAffectedWnd=pControlWnd;
							iAffectType=3;
							break;
						}
					}
				}
			}
			if(bResizingElementsArePresent<0){
				bResizingElementsArePresent=0;
			}
			if(pResizeAffectedWnd){
				if(iAffectType<2){
					hResizeCur=SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE)));
				}else{
					hResizeCur=SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS)));
				}
			}else if(hResizeCur){
				SetCursor(hResizeCur);
			}
		}
	}
	return FALSE;
};

BOOL CSmartWndSizer::bFollowModeEnabled=FALSE;
void CSmartWndSizer::FollowWndRect(CRect rMaster, CPoint pOffset)
{
	int i=0;
	CSmartLocker lock(rectLock);
	for(i=0;i<m_AllWndSizerRectangles->GetSize();i++){
		(*m_AllWndSizerRectangles)[i].bStickedForFollow=FALSE;
	}
	bFollowModeEnabled=TRUE;
	for(i=0;i<m_AllWndSizerRectangles->GetSize();i++){
		CWnd* pCompWith=(*m_AllWndSizerRectangles)[i].pWnd;
		if(pCompWith==NULL || !IsWindow(pCompWith->m_hWnd) || !(pCompWith->IsWindowVisible())){
			continue;
		}
		CRect rCompWith;
		pCompWith->GetWindowRect(rCompWith);
		if(rMaster.EqualRect(rCompWith)){
			continue;
		}
		if(!(*m_AllWndSizerRectangles)[i].bStickedForFollow && IsRectStickedTogether(rMaster, rCompWith, 1)){
			rCompWith.OffsetRect(pOffset);
			pCompWith->MoveWindow(rCompWith,TRUE);
			(*m_AllWndSizerRectangles)[i].bStickedForFollow=TRUE;
		}
	}
	return;
}

BOOL CSmartWndSizer::TestRectInWindows(CRect rt, BOOL bExcludeSelf, BOOL bExcludeInvises, const char* szTestOnProp)
{
	if(TestPointInWindows(rt.TopLeft())==2 || TestPointInWindows(rt.BottomRight())==2){
		return 2;
	}
	CSmartLocker lock(rectLock);
	for(int i=0;i<m_AllWndSizerRectangles->GetSize();i++){
		CWnd* pCompWith=(*m_AllWndSizerRectangles)[i].pWnd;
		if(pCompWith==NULL || !IsWindow(pCompWith->m_hWnd)){
			continue;
		}
		if(bExcludeInvises && !(pCompWith->IsWindowVisible())){
			continue;
		}
		if(bExcludeInvises==0 && szTestOnProp){
			HANDLE h=GetProp(pCompWith->m_hWnd,szTestOnProp);
			if(h==0){
				continue;
			}
		}
		CRect rCompWith;
		pCompWith->GetWindowRect(rCompWith);
		if(bExcludeSelf && rCompWith.EqualRect(&rt)){
			continue;
		}
		CRect Intersect(rt);
		Intersect.IntersectRect(rCompWith,rt);
		if(!Intersect.IsRectEmpty()){
			return 1;
		}
	}
	return 0;
}

BOOL CSmartWndSizer::TestPointInWindows(CPoint pt)
{
	//::SystemParametersInfo(SPI_GETWORKAREA,0,&DesktopDialogRect,0);
	DesktopDialogRect=GetAllMonitorsRECT();
	if(!DesktopDialogRect.PtInRect(pt)){
		return 2;
	}
	CSmartLocker lock(rectLock);
	for(int i=0;i<m_AllWndSizerRectangles->GetSize();i++){
		CWnd* pCompWith=(*m_AllWndSizerRectangles)[i].pWnd;
		if(pCompWith==NULL || !IsWindow(pCompWith->m_hWnd) || !(pCompWith->IsWindowVisible())){
			continue;
		}
		CRect rCompWith;
		pCompWith->GetWindowRect(rCompWith);
		if(rCompWith.PtInRect(pt)){
			return 1;
		}
	}
	return 0;
}

#include <multimon.h>
#ifdef COMPILE_MULTIMON_STUBS
#undef COMPILE_MULTIMON_STUBS
#endif
CRect& CSmartWndSizer::GetScreenRect(CRect* rtBase, BOOL* bMultiMonsOut, BOOL bForBestMonitor)
{
	static CRect rtRes(0,0,0,0);
	static BOOL bMultyMons=FALSE;
	if(rtBase==0 && bMultiMonsOut==0){
		return rtRes;
	}
	if(rtRes.Width()==0 && rtRes.Height()==0){
		bMultyMons=FALSE;
		if(rtBase!=NULL && GetSystemMetrics(80)>1){//SM_CMONITORS
			bMultyMons=TRUE;
			DesktopDialogRect=GetAllMonitorsRECT();
			rtRes=DesktopDialogRect;
		}
		if(rtRes.Width()==0 && rtRes.Height()==0){
			CRect DesktopDialogRect;
			::SystemParametersInfo(SPI_GETWORKAREA,0,&DesktopDialogRect,0);
			//DesktopDialogRect=GetAllMonitorsRECT();
			rtRes=DesktopDialogRect;
		}
	}
	if(bForBestMonitor && bMultyMons){
		MONITORINFO mi;
		memset(&mi,0,sizeof(mi));
		mi.cbSize = sizeof(mi);
		HMONITOR hMonitor=NULL;
		hMonitor = MonitorFromRect(rtBase, MONITOR_DEFAULTTONEAREST);
		if(hMonitor != NULL){
			GetMonitorInfo(hMonitor, &mi);
			rtRes=mi.rcWork;
		}
	}
	if(bMultiMonsOut){
		*bMultiMonsOut=bMultyMons;
	}
	return rtRes;
}

BOOL& IsScreenBordersAreFAT()
{
	static BOOL b=1;
	return b;
}