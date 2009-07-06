// IListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "IListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#define FLOG3A(x,z1,z2,z3) {FILE* f=fopen("c:\\wk_debug_2a.txt","a+");fprintf(f,x,z1,z2,z3);fprintf(f,"\n");fclose(f);};
CWnd* AfxWPGetMainWnd();
HINSTANCE IsPluginOn(const char* sz)
{
	HINSTANCE h=GetModuleHandle(CString(sz)+".wkp");
	if(h){
		return h;
	}
	CString sP=CString("WKPLUG_")+sz;
	sP.MakeUpper();
	HWND hW=FindWindow("WK_MAIN",0);
	if(hW!=0){
		return (HINSTANCE)GetProp(hW,sP);
	}
	return 0;
}

CString GetLinksFromClipboardAsText()
{
	CWnd* pWnd=AfxWPGetMainWnd();
	if(!pWnd || !pWnd->OpenClipboard()){
		return "";
	}
	CString str="";
	HDROP hCliptext=(HDROP)::GetClipboardData(CF_HDROP);
	if(hCliptext){
		char szFilePath[256];
		UINT cFiles = DragQueryFile(hCliptext, (UINT)-1, NULL, 0);
		for (UINT u = 0; u < cFiles; u++){
			DragQueryFile(hCliptext, u, szFilePath, sizeof(szFilePath));
			str+=szFilePath;
			if(u!=cFiles-1){
				str+="\n";
			}
		}
	}
	::CloseClipboard();
	return str;
}

CString GetTextFromClipboard(BOOL* bFromLink)
{
	CWnd* pWnd=AfxWPGetMainWnd();
	if(!pWnd || !pWnd->OpenClipboard()){
		return "";
	}
	CString str="";
	HANDLE hCliptext=::GetClipboardData(CF_TEXT);
	if(hCliptext){
		str = (char*)GlobalLock(hCliptext); 
		GlobalUnlock(hCliptext);
	}
	::CloseClipboard();
	if(bFromLink){
		*bFromLink=FALSE;
	}
	if(str==""){
		if(bFromLink){
			*bFromLink=TRUE;
		}
		str=GetLinksFromClipboardAsText();
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrlWithCustomDraw
CListCtrlWithCustomDraw::CListCtrlWithCustomDraw()
{
}

CListCtrlWithCustomDraw::~CListCtrlWithCustomDraw()
{
}


BEGIN_MESSAGE_MAP(CListCtrlWithCustomDraw, CListCtrl)
    //{{AFX_MSG_MAP(CListCtrlWithCustomDraw)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlWithCustomDraw message handlers


void CListCtrlWithCustomDraw::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // first, lets extract data from
    // the message for ease of use later
    NMLVCUSTOMDRAW* pNMLVCUSTOMDRAW = (NMLVCUSTOMDRAW*)pNMHDR;

    // we'll copy the device context into hdc 
    // but wont convert it to a pDC* until (and if)
    // we need it as this requires a bit of work
    // internally for MFC to create temporary CDC
    // objects
    HDC hdc = pNMLVCUSTOMDRAW->nmcd.hdc;
    CDC* pDC = NULL;

    // here is the item info
    // note that we don't get the subitem
    // number here, as this may not be
    // valid data except when we are
    // handling a sub item notification
    // so we'll do that separately in
    // the appropriate case statements
    // below.
    int nItem = pNMLVCUSTOMDRAW->nmcd.dwItemSpec;
    UINT nState = pNMLVCUSTOMDRAW->nmcd.uItemState;
    LPARAM lParam = pNMLVCUSTOMDRAW->nmcd.lItemlParam;

    // next we set up flags that will control
    // the return value for *pResult
    bool bNotifyPostPaint = false;
    bool bNotifyItemDraw = false;
    bool bNotifySubItemDraw = false;
    bool bSkipDefault = false;
    bool bNewFont = false;

    // what we do next depends on the
    // drawing stage we are processing
    switch (pNMLVCUSTOMDRAW->nmcd.dwDrawStage) {
    case CDDS_PREPAINT:
        {
            // PrePaint
            m_pOldItemFont = NULL;
            m_pOldSubItemFont = NULL;
            bNotifyPostPaint = IsNotifyPostPaint();
            bNotifyItemDraw = IsNotifyItemDraw();
            // do we want to draw the control ourselves?
            if (IsDraw()) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                // do the drawing
                if (OnDraw(pDC)) {
                    // we drew it all ourselves
                    // so don't do default
                    bSkipDefault = true;
                }
            }
        }
        break;
    case CDDS_ITEMPREPAINT:
        {
            // Item PrePaint
            m_pOldItemFont = NULL;
            bNotifyPostPaint = IsNotifyItemPostPaint(nItem,nState,lParam);
            bNotifySubItemDraw = IsNotifySubItemDraw(nItem,nState,lParam);
            // set up the colors to use
            pNMLVCUSTOMDRAW->clrText = TextColorForItem(nItem,nState,lParam);
            pNMLVCUSTOMDRAW->clrTextBk = BkColorForItem(nItem,nState,lParam);
            // set up a different font to use, if any
            CFont* pNewFont = FontForItem(nItem,nState,lParam);
            if (pNewFont) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                m_pOldItemFont = pDC->SelectObject(pNewFont);
                bNotifyPostPaint = true;    // need to restore old font
            }
            // do we want to draw the item ourselves?
            if (IsItemDraw(nItem,nState,lParam)) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                if (OnItemDraw(pDC,nItem,nState,lParam)) {
                    // we drew it all ourselves
                    // so don't do default
                    bSkipDefault = true;
                }
            }
        }
        break;
    case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
        {
            // Sub Item PrePaint
            // set sub item number (data will be valid now)
            int nSubItem = pNMLVCUSTOMDRAW->iSubItem;
            m_pOldSubItemFont = NULL;
            bNotifyPostPaint = IsNotifySubItemPostPaint(nItem,nSubItem,nState,lParam);
            // set up the colors to use
            pNMLVCUSTOMDRAW->clrText = TextColorForSubItem(nItem,nSubItem,nState,lParam);
            pNMLVCUSTOMDRAW->clrTextBk = BkColorForSubItem(nItem,nSubItem,nState,lParam);
            // set up a different font to use, if any
            CFont* pNewFont = FontForSubItem(nItem,nSubItem,nState,lParam);
            if (pNewFont) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                m_pOldSubItemFont = pDC->SelectObject(pNewFont);
                bNotifyPostPaint = true;    // nmeed to restore old font
            }
            // do we want to draw the item ourselves?
            if (IsSubItemDraw(nItem,nSubItem,nState,lParam)) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                if (OnSubItemDraw(pDC,nItem,nSubItem,nState,lParam)) {
                    // we drew it all ourselves
                    // so don't do default
                    bSkipDefault = true;
                }
            }
        }
        break;
    case CDDS_ITEMPOSTPAINT|CDDS_SUBITEM:
        {
            // Sub Item PostPaint
            // set sub item number (data will be valid now)
            int nSubItem = pNMLVCUSTOMDRAW->iSubItem;
            // restore old font if any
            if (m_pOldSubItemFont) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                pDC->SelectObject(m_pOldSubItemFont);
                m_pOldSubItemFont = NULL;
            }
            // do we want to do any extra drawing?
            if (IsSubItemPostDraw()) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                OnSubItemPostDraw(pDC,nItem,nSubItem,nState,lParam);
            }
        }
        break;
    case CDDS_ITEMPOSTPAINT:
        {
            // Item PostPaint
            // restore old font if any
            if (m_pOldItemFont) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                pDC->SelectObject(m_pOldItemFont);
                m_pOldItemFont = NULL;
            }
            // do we want to do any extra drawing?
            if (IsItemPostDraw()) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                OnItemPostDraw(pDC,nItem,nState,lParam);
            }
        }
        break;
    case CDDS_POSTPAINT:
        {
            // PostPaint
            // do we want to do any extra drawing?
            if (IsPostDraw()) {
                if (! pDC) pDC = CDC::FromHandle(hdc);
                OnPostDraw(pDC);
            }
        }
        break;
    }

    ASSERT(CDRF_DODEFAULT==0);
    *pResult = 0;
    if (bNotifyPostPaint) {
        *pResult |= CDRF_NOTIFYPOSTPAINT;
    }
    if (bNotifyItemDraw) {
        *pResult |= CDRF_NOTIFYITEMDRAW;
    }
    if (bNotifySubItemDraw) {
        *pResult |= CDRF_NOTIFYSUBITEMDRAW;
    }
    if (bNewFont) {
        *pResult |= CDRF_NEWFONT;
    }
    if (bSkipDefault) {
        *pResult |= CDRF_SKIPDEFAULT;
    }
    if (*pResult == 0) {
        // redundant as CDRF_DODEFAULT==0 anyway
        // but shouldn't depend on this in our code
        *pResult = CDRF_DODEFAULT;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CIListCtrl

CIListCtrl::CIListCtrl():m_rowTopRoot(&m_mapAllRows)
{
	bSkipTreeLines=FALSE;
	m_bStopUpdateScreen=FALSE;
	bLockUpdateForCollapse=FALSE;
	bIgnoreTabKey=FALSE;
	bInDestroy=0;
	bExcludeLabelFromCheckboxes=FALSE;
	bShowButtonsAsLinks=FALSE;
	bDoNotShowTree=FALSE;
	bDoNotShowGrid=FALSE;
	bAllowMultySelect=FALSE;
	bDrawTree0=FALSE;
	bSilentAdd=FALSE;
	m_bReadOnly=TRUE;
	m_hSelectedIRow=NULL;
	m_iSelectedIItem=-1;
	m_iLockSelChangeCounter=0;
	m_iLockMSelChangeCounter=0;
	m_rowTopRoot.m_bCollapsed=FALSE;
	m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
	m_crIRowBackground=GetSysColor(COLOR_WINDOW);
	m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	m_crSelectedIItemBackground=RGB(240, 220, 40);
	m_crSelectedIItemText=RGB(0, 0, 0);
	m_crSelectedBold=0;
}

CIListCtrl::~CIListCtrl()
{
}


BEGIN_MESSAGE_MAP(CIListCtrl, CListCtrlWithCustomDraw)
	//{{AFX_MSG_MAP(CIListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(FLNM_END_INPLACE_EDIT, OnEndEdit)
	ON_NOTIFY_REFLECT(FLNM_END_INPLACE_COMBO, OnEndCombo)
	ON_NOTIFY_REFLECT(FLNM_END_INPLACE_BUTTON, OnEndInplaceButton)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/* Не нужно
ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnDrag)
*/
#ifndef _NO_XMLHOTKEYSUPPORT
CInPlaceHotKey* lHotkeyInputInProgress=0;
#endif
BOOL CIListCtrl::PreTranslateMessage(MSG* pMsg)
{
	UINT iCtrlID=0;
	if(pMsg->hwnd!=NULL){
		iCtrlID=::GetDlgCtrlID(pMsg->hwnd);
	}
	if(pMsg->message==WM_KEYUP || pMsg->message==WM_KEYDOWN){
#ifndef _NO_XMLHOTKEYSUPPORT
		if(iCtrlID==IDC_INPLACE_HOTKEY){
			if(pMsg->message==WM_KEYUP){
				BOOL bNeedToStopThis=FALSE;
				if(lHotkeyInputInProgress){
					bNeedToStopThis=lHotkeyInputInProgress->isStopHotkey();
				}else{
					bNeedToStopThis=(pMsg->wParam==VK_ESCAPE && GetAsyncKeyState(VK_SHIFT)>=0 && GetAsyncKeyState(VK_MENU)>=0 && GetAsyncKeyState(VK_CONTROL)>=0 && GetAsyncKeyState(VK_RWIN)>=0 && GetAsyncKeyState(VK_LWIN)>=0);
				}
				if(bNeedToStopThis){
					if(lHotkeyInputInProgress){
						lHotkeyInputInProgress->AbortProcess();
					};
					SetFocus();
					return TRUE;
				}
			}
			return FALSE;
		}
#endif
	}
	if(pMsg->message==WM_MOUSEWHEEL){
		if(GetFocus()!=this){
			return TRUE;
		}
	}
	if(pMsg->message==WM_MOUSEMOVE){
		if(bShowButtonsAsLinks){
			CPoint p;
			int iCol=-1;
			GetCursorPos(&p);
			ScreenToClient(&p);
			CIListRow* pIRow=0;
			int iRow=HitTestEx(p, &iCol,pIRow);
			if(pIRow){
				static BOOL bCursorChanged=0;
				if(pIRow->m_aItems[iCol]->m_iControlType & FLC_CUSTOMDIALOG2){
					bCursorChanged=1;
					::SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(32649)));
				}else if(bCursorChanged==1){
					bCursorChanged=0;
					::SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
				}
			}
		}
	}
	return CListCtrlWithCustomDraw::PreTranslateMessage(pMsg);
}

CInPlaceSlider* g_CurrentActiveSlider=0;
LRESULT  CIListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_HSCROLL){
		UINT iCtrlID=::GetDlgCtrlID(::GetFocus());
		if(iCtrlID==IDC_INPLACE_SLIDER  && g_CurrentActiveSlider){
			g_CurrentActiveSlider->UpdatePos();
			return TRUE;
		}
	}
	return CListCtrlWithCustomDraw::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CIListCtrl message handlers

void CIListCtrl::OnEndInplaceButton( NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;
	if(pRowState->dwData==VK_RETURN)
	{
		CIListRow* pSelIRow=FindRow(pRowState->hIRow);
		if(pSelIRow){
			NotifyOwner(FLNM_CUSTOMDIALOG, pSelIRow);
		}
	}
	EditFinish(pRowState);
}

void CIListCtrl::OnEndCombo( NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;

	if(pRowState->dwData!=VK_ESCAPE)
	{
		CIListRow* pSelIRow=FindRow(pRowState->hIRow);
		if(!NotifyOwner(FLNM_ENDEDIT, pSelIRow, pRowState->strText, pRowState->dwNotifyData)){
			SetIItemText(pRowState->hIRow, pRowState->iIItem, pRowState->strText);
		}
	}
	EditFinish(pRowState);
}

void CIListCtrl::OnEndEdit( NMHDR * pNotifyStruct, LRESULT * result)
{
	FLNM_ROWSTATE *pRowState=(FLNM_ROWSTATE*) pNotifyStruct;

	if(pRowState->dwData!=VK_ESCAPE)
	{
		CIListRow* pSelIRow=FindRow(pRowState->hIRow);
		if(!NotifyOwner(FLNM_ENDEDIT, pSelIRow, pRowState->strText, pRowState->dwNotifyData))
			SetIItemText(pRowState->hIRow, pRowState->iIItem, pRowState->strText);
	}
	EditFinish(pRowState);
}

void CIListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	int nCol=0;
	CIListRow* pIRow=NULL;
	int nItem = HitTestEx(point, &nCol, pIRow);
	if (pIRow)
	{
		SelectIRow(pIRow->m_hIRow, TRUE);
		SelectIItem(pIRow->m_hIRow, nCol);
		CListCtrl::SetFocus();
	}
	else
		CListCtrlWithCustomDraw::OnRButtonDown(nFlags, point);
}

void CIListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	HitTestMouseClick(point);
}

void CIListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(!HitTestMouseClick(point))
		EditBegin(-1);
}

BOOL CIListCtrl::HitTestMouseClick(CPoint point)
{
	BOOL bResult=TRUE;
	int nCol=0;
	CIListRow* pIRow=NULL;
	int nItem = HitTestEx(point, &nCol, pIRow);
	if (pIRow)
	{
		CListCtrl::SetFocus();
		BOOL bSelected=pIRow->m_hIRow==GetSelectedIRow() && nCol==GetSelectedIItem();
		BOOL bCollapseRes=HitTestCollapse(pIRow, point);
		if(bCollapseRes==TRUE || (bSelected && pIRow->GetChildIRowCount() && (pIRow->m_aItems[nCol]->m_iControlType==FLC_READONLY || (pIRow->m_aItems[nCol]->m_iControlType==FLC_CHECKBOX && !HitTestCheckbox(point, pIRow, nCol))))){
			if(IsCollapsed(pIRow)){
				Expand(pIRow);
			}else{
				Collapse(pIRow);
			}
		}else{
			if(HitTestCheckbox(point, pIRow, nCol) || (bSelected && pIRow->m_aItems[nCol]->m_iControlType==FLC_CHECKBOX) && (!bExcludeLabelFromCheckboxes || (bExcludeLabelFromCheckboxes && !HitTestLabel(pIRow, point, nCol)))){
				DWORD dwNewCheck=!pIRow->m_aItems[nCol]->m_bChecked;
				// Добавить нотификатор
				if(NotifyOwner(FLMN_CHECKBOX_TRIGGER, pIRow, NULL, dwNewCheck)==0){
					pIRow->m_aItems[nCol]->m_bChecked=dwNewCheck;
				}
			}else{
				if(bCollapseRes==-1){
					bResult= TRUE;
				}else{
					bResult= FALSE;
				}
			}
			EnsureVisible(pIRow->m_hIRow, nCol, TRUE);
			SelectIItem(pIRow->m_hIRow, nCol, TRUE);
		}
	}else{
		bResult= FALSE;
	}
	return bResult;
}

//
//	INTERFACE
//

//
int CIListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	int lResult = CListCtrl::Create(dwStyle, rect, pParentWnd, nID);
	return lResult;
}

int CIListCtrl::InsertFColumn(int iIndex, LPCTSTR lpszText, BOOL fTree, int nFormat, int nWidth)
{
	int iColumnFormat=0;
	if(nFormat>=0){
		iColumnFormat=(nFormat==LVCFMT_RIGHT ? DT_RIGHT : (nFormat==LVCFMT_CENTER ? DT_CENTER : DT_LEFT));
	}else{
		iColumnFormat=-nFormat;
		nFormat=(iColumnFormat & DT_RIGHT)?LVCFMT_RIGHT:((iColumnFormat & DT_CENTER)?LVCFMT_CENTER:LVCFMT_LEFT);
	}
	m_aFColumns.Insert(iIndex, fTree, iColumnFormat);
	//
	void* rKey;
	void* rValue;
	CIListRow* pIRow;
	POSITION pos=m_mapAllRows.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapAllRows.GetNextAssoc(pos, rKey, rValue);
		pIRow=(CIListRow*) rValue;

		pIRow->InsertItem(iIndex, _T(""), -1);
	}

	return CListCtrl::InsertColumn(iIndex, lpszText, nFormat, nWidth);
}
//
void CIListCtrl::SetFColumnImageOffset(int nCol, BOOL fImageOffset)
{
	m_aFColumns.GetAt(nCol)->m_fImageOffset=fImageOffset;
}
//
BOOL CIListCtrl::IsFColumnImageOffset(int nCol)
{
	return m_aFColumns.GetAt(nCol)->m_fImageOffset;
}
//
int CIListCtrl::SetFColumnWidth(int nCol, int cx)
{return SetColumnWidth(nCol, cx);}
//
BOOL CIListCtrl::GetFColumnWidth(int nCol, int cx)
{return GetColumnWidth(nCol);}

//
int CIListCtrl::GetFColumnCount()
{
	return GetHeaderCtrl()->GetItemCount();
}
//
void CIListCtrl::DeleteFColumn(int iIndex)
{
	m_aFColumns.Delete(iIndex);
	//
	void* rKey;
	void* rValue;
	CIListRow* pIRow;
	POSITION pos=m_mapAllRows.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapAllRows.GetNextAssoc(pos, rKey, rValue);
		pIRow=(CIListRow*) rValue;

		pIRow->RemoveItem(iIndex);
	}
	m_aFColumns.Delete(iIndex);

	CListCtrl::DeleteColumn(iIndex);
}
//
HIROW CIListCtrl::InsertIRow(HIROW hParentRow, HIROW hPosition, LPCTSTR lpszText, int lImageIndex)
{
	LockSelChange(TRUE);
	CIListRow *pIRow= new CIListRow(&m_mapAllRows);
	//	Init Row
	int iSize=m_aFColumns.Count();
	int iTreeIndex=-1;
	for(int i=0; i<iSize; i++)
	{
		if(m_aFColumns.GetAt(i)->m_fTree)
		{
			pIRow->InsertItem(i, lpszText, lImageIndex);
			iTreeIndex=i;
		}
		else
			pIRow->InsertItem(i, _T(""), -1);

		//
		pIRow->m_aItems[i]->m_iControlType=m_aFColumns.GetAt(i)->m_iControlType;
		pIRow->m_aItems[i]->m_iCheckedImage=m_aFColumns.GetAt(i)->m_iCheckedImage;
		pIRow->m_aItems[i]->m_iUncheckedImage=m_aFColumns.GetAt(i)->m_iUncheckedImage;
	}

	if(iTreeIndex<0)
	{
		pIRow->GetAt(0)->m_strText=lpszText;
		pIRow->GetAt(0)->m_iImageIndex =lImageIndex;
	}

	//
	HIROW hIRow;
	if(hParentRow==FL_ROOT){
		hIRow=m_rowTopRoot.Insert(pIRow, hPosition);
		AddRow(hIRow, pIRow);
	}
	else{
		hIRow=FindRow(hParentRow)->Insert(pIRow, hPosition);
		AddRow(hIRow, pIRow);
	}
	pIRow->m_hIRow=hIRow;
	//
	if(!bSilentAdd){
		if(pIRow->m_pParentRow==NULL || !pIRow->m_pParentRow->m_bCollapsed)
		{
			if(pIRow->m_pPrevRow!=NULL)
				ShowIRow(hIRow, pIRow->m_pPrevRow->GetNextListIndex());
			else
				ShowIRow(hIRow, pIRow->m_pParentRow->m_iListIndex+1);
		}
	}
	LockSelChange(FALSE);
	return hIRow;
}

//
CInPlaceButton* g_pInPlaceButton=NULL;
void CIListCtrl::DeleteIRow(HIROW hIRow, BOOL bSaveSelected)
{
	HIROW hSelectIRow=NULL;
	BOOL bSelect=FALSE;
	CIListRow* pIRow=FindRow(hIRow);
	if(pIRow==NULL){
		return;
	}
	if(g_pInPlaceButton!=NULL){
		g_pInPlaceButton->m_uChar=VK_ESCAPE;
		SetFocus();
	}
	LockSelChange(TRUE);
	HIROW hRootIRow=GetParentIRow(hIRow);
	Collapse(pIRow);
	if(bSaveSelected && hIRow==GetSelectedIRow())
	{
		bSelect=TRUE;
		hSelectIRow=GetPrevIRow(hIRow);
		if(hSelectIRow==NULL)
			hSelectIRow=GetParentIRow(hIRow);
	}
	HideIRow(pIRow, TRUE);
	CIListRow* pParentIRow=pIRow->m_pParentRow;
	pParentIRow->RemoveChildIRow(hIRow,this);
	LockSelChange(FALSE);
	if(bSelect){
		if(hSelectIRow==FL_ROOT)
			hSelectIRow=GetFirstChildIRow(FL_ROOT);
		SelectIRow(hSelectIRow, TRUE);
	}

	if(!bInDestroy && hRootIRow!=FL_ROOT && GetFirstChildIRow(hRootIRow)==NULL)
		UpdateIRow(hRootIRow);
}

//dwBgColor/dwTextColor - если -1 то по умолчанию
BOOL CIListCtrl::SetIItemColors(HIROW hIRow, int iSubItem, DWORD dwTextColor, DWORD dwBgColor)
{
	CIListRow *pIRow=FindRow(hIRow);
	if(pIRow==NULL){
		return FALSE;
	}
	pIRow->GetAt(iSubItem)->m_TextColor=dwTextColor;
	pIRow->GetAt(iSubItem)->m_BgColor=dwBgColor;
	if(pIRow->m_iListIndex>=0){
		UpdateIRow(hIRow);
		return TRUE;
	}
	return FALSE;
}

BOOL CIListCtrl::GetIRowColors(DWORD& dwTextColor, DWORD& dwBgColor)
{
	dwBgColor=m_crIRowBackground;
	dwTextColor=m_crIItemText;
	return TRUE;
}

BOOL CIListCtrl::GetIItemColors(HIROW hIRow, int iSubItem, DWORD& dwTextColor, DWORD& dwBgColor)
{
	CIListRow *pIRow=FindRow(hIRow);
	if(pIRow==NULL){
		return FALSE;
	}
	dwTextColor=pIRow->GetAt(iSubItem)->m_TextColor;
	dwBgColor=pIRow->GetAt(iSubItem)->m_BgColor;
	return TRUE;
}

//
BOOL CIListCtrl::SetIItemText(HIROW hIRow, int iSubItem, LPCTSTR lpszText)
{
	CIListRow *pIRow=FindRow(hIRow);
	if(pIRow==NULL){
		return FALSE;
	}
	pIRow->GetAt(iSubItem)->m_strText=lpszText;
	if(pIRow->m_iListIndex>=0){
		SetItemText(pIRow->m_iListIndex, iSubItem, lpszText);
		return TRUE;
	}
	return FALSE;
}
//
CString CIListCtrl::GetIItemText(HIROW hIRow, int iSubItem)
{
	CIListRow *pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	return pIRow->GetAt(iSubItem)->m_strText;
}
//
void CIListCtrl::SetIItemImage(HIROW hIRow, int iSubItem, int iImageIndex)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	int iOldImage=pIRow->GetAt(iSubItem)->m_iImageIndex;
	pIRow->GetAt(iSubItem)->m_iImageIndex=iImageIndex;
	if(iOldImage!=iImageIndex){
		UpdateIRow(hIRow);
	}
}
//
int CIListCtrl::GetIItemImage(HIROW hIRow, int iSubItem)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	return pIRow->GetAt(iSubItem)->m_iImageIndex;
}
//
BOOL CIListCtrl::IsCollapsed(HIROW hIRow)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	return IsCollapsed(pIRow);
}
//
void CIListCtrl::SetUnCollapsible(HIROW hIRow, BOOL bUncol)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	pIRow->m_bUncollapsible=bUncol;
}

void CIListCtrl::Expand(HIROW hIRow, BOOL bWithRoot, BOOL bNotify)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	if(bWithRoot){
		ShowIRow(pIRow,0);
	}
	Expand(pIRow,bNotify);
}
//
void CIListCtrl::Collapse(HIROW hIRow, BOOL bWithRoot)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	if(pIRow->m_bUncollapsible){
		return;
	}
	Collapse(pIRow,bWithRoot?FALSE:TRUE);
	if(bWithRoot){
		HideIRow(pIRow,TRUE);
	}
}
//
HIROW CIListCtrl::GetSelectedIRow()
{
	return m_hSelectedIRow;
	//int iSel=GetSelectionMark();
	//if(iSel<0)
	//	return NULL;
	//return ((CIListRow*)GetItemData(iSel))->m_hIRow;
}
//
int CIListCtrl::GetIRowCount()
{
	return m_mapAllRows.GetCount();
}
//
DWORD CIListCtrl::CheckIRow(HIROW hIRow)
{
	CIListRow* pIRow=FindRow(hIRow);
	return pIRow!=0;
}

DWORD CIListCtrl::GetIRowData(HIROW hIRow)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	return pIRow->m_dwData;
}

HIROW CIListCtrl::GetIRowFromIndex(int index)
{
	CIListRow* frow=(CIListRow*)GetItemData(index);
	if(frow){
		return frow->m_hIRow;
	}else{
		return NULL;
	}
}

int CIListCtrl::GetIRowIndex(HIROW hIRow)
{
	CIListRow* frow=FindRow(hIRow);
	if(frow){
		return frow->m_iListIndex;
	}
	return -1;
}

void CIListCtrl::SetIRowData(HIROW hIRow, DWORD dwData)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	pIRow->m_dwData=dwData;
}
//
HIROW CIListCtrl::GetParentIRow(HIROW hIRow)
{
	if(hIRow==FL_ROOT){
		return FL_ROOT;
	}
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	if(pIRow->m_iDeep>1)
		return FindRow(hIRow)->m_pParentRow->m_hIRow;
	else
		return FL_ROOT;
}
//
int CIListCtrl::GetChildIRowCount(HIROW hIRow)
{
	CIListRow* pIRow= hIRow==FL_ROOT ? &m_rowTopRoot : FindRow(hIRow);
	return pIRow==NULL ? 0 : pIRow->m_lstChildRows.GetCount();
}
//
HIROW CIListCtrl::GetChildIRowAt(HIROW hParentIRow, int index)
{
	CIListRow* pIRow= hParentIRow==FL_ROOT ? &m_rowTopRoot: FindRow(hParentIRow);
	ASSERT(pIRow!=NULL);
	return (HIROW)pIRow->m_lstChildRows.FindIndex(index);
}

DWORD CIListCtrl::GetChildIRowDeep(HIROW hIRow)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	return pIRow->m_iDeep;
}


//
HIROW CIListCtrl::GetFirstChildIRow(HIROW hIRow)
{
	CIListRow* pIRow= hIRow==FL_ROOT ? m_rowTopRoot.GetChildIRow(FL_FIRST) : FindRow(hIRow)->GetChildIRow(FL_FIRST);
	//	ASSERT(pIRow!=NULL);
	return pIRow==NULL ? NULL : pIRow->m_hIRow;
}
//
HIROW CIListCtrl::GetLastChildIRow(HIROW hIRow)
{
	CIListRow* pIRow= hIRow==FL_ROOT ? m_rowTopRoot.GetChildIRow(FL_LAST) : FindRow(hIRow)->GetChildIRow(FL_LAST);
	//	ASSERT(pIRow!=NULL);
	return pIRow==NULL ? NULL : pIRow->m_hIRow;
}
//
HIROW CIListCtrl::GetNextIRow(HIROW hIRow)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	if(pIRow->m_pNextRow!=NULL)
	{
		return pIRow->m_pNextRow->m_hIRow;
	}
	return NULL;
}
//
HIROW CIListCtrl::GetPrevIRow(HIROW hIRow)
{
	CIListRow* pIRow=FindRow(hIRow);
	ASSERT(pIRow!=NULL);
	if(pIRow->m_pPrevRow!=NULL)
		return pIRow->m_pPrevRow->m_hIRow;
	return NULL;
}
//



void CIListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//Navigation(nChar, FALSE);
	if(!Navigation(nChar, FALSE))
		CListCtrlWithCustomDraw::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CIListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
}

void CIListCtrl::OnColumnDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
}

//
//
//		CInPlaceButton
//
//

CInPlaceButton::CInPlaceButton(HIROW hIRow, int iIItem, CString strInitText)
{
	m_hIRow = hIRow;
	m_iIItem = iIItem;
	m_strInitText=strInitText;
	m_uChar=0;
	m_bCapture=FALSE;
	g_pInPlaceButton=this;
}

CInPlaceButton::~CInPlaceButton()
{
	g_pInPlaceButton=NULL;
}

BEGIN_MESSAGE_MAP(CInPlaceButton, CButton)
	//{{AFX_MSG_MAP(CInPlaceButton)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CInPlaceButton::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_DRAWITEM){
		OnDrawItem((int)pMsg->lParam,(LPDRAWITEMSTRUCT)pMsg->wParam);
		return TRUE;
	}
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_DELETE || pMsg->wParam == VK_INSERT){
			// Не имеет отношения к кнопке
			m_uChar=pMsg->wParam;
			return FALSE;
		}else if((pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || GetKeyState( VK_CONTROL)))
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;		    	// DO NOT process further
		}
	}

	return CButton::PreTranslateMessage(pMsg);
}

void CInPlaceButton::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(!lpDrawItemStruct){
		return;
	}
	CRect rcItem=lpDrawItemStruct->rcItem;
	rcItem.left=rcItem.right-rcItem.Height();
	//draw text
	CString str;
	GetWindowText(str);
	//rcItem.right-=rcItem.Height()+2;
	DrawText(lpDrawItemStruct->hDC,str, -1, rcItem, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
	//rcItem.right+=rcItem.Height()+2;
	DrawFrameControl(lpDrawItemStruct->hDC,rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);//|(?DFCS_PUSHED:0)
}

void CInPlaceButton::OnKillFocus(CWnd* pNewWnd)
{
	//	ReleaseCapture();
	//
	CButton::OnKillFocus(pNewWnd);
	CString str;
	GetWindowText(str);
	// Send Notification to parent of ListView ctrl
	FLNM_ROWSTATE flnmRowState;
	flnmRowState.hdr.code=FLNM_END_INPLACE_BUTTON;
	flnmRowState.hdr.hwndFrom=GetParent()->m_hWnd;
	flnmRowState.hdr.idFrom=GetDlgCtrlID();

	flnmRowState.bCollapsed= FALSE;
	flnmRowState.dwData= (DWORD)m_uChar;
	flnmRowState.strText=str;
	flnmRowState.hIRow= m_hIRow;
	flnmRowState.iIItem=m_iIItem;
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(),  (LPARAM) &flnmRowState);
	//
	DestroyWindow();
}

void CInPlaceButton::OnNcDestroy()
{
	CButton::OnNcDestroy();

	delete this;
}

void CInPlaceButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	m_bCapture=TRUE;
	CButton::OnLButtonDown(nFlags, point);
}

void CInPlaceButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CButton::OnLButtonUp(nFlags, point);

	if(m_bCapture)
	{
		ReleaseCapture();
		CRect rect;
		GetWindowRect(&rect);
		rect.InflateRect(rect.left, rect.top);
		if(rect.PtInRect(point))
		{
			m_uChar=VK_RETURN;
			GetParent()->SetFocus();
		}
	}
}

void CInPlaceButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_SPACE){
		nChar=VK_RETURN;
	}
	switch(nChar){
	case VK_SPACE:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_UP:
	case VK_ESCAPE:
	case VK_RETURN:
	case VK_TAB:
	case VK_INSERT:
	case VK_DELETE:
	case VK_PRIOR:
	case VK_NEXT:
		m_uChar=nChar;
		GetParent()->SetFocus();
		break;
	default:
		CButton::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}
}

void CInPlaceButton::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CButton::OnChar(nChar, nRepCnt, nFlags);
}

int CInPlaceButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_strInitText );
	SetFocus();
	return 0;
}

//
//
//		CInPlaceEdit
//
//
CInPlaceEdit::CInPlaceEdit(HIROW hIRow, int iIItem, CString strInitText, BOOL bSelectText)
{
	m_hIRow = hIRow;
	m_iIItem = iIItem;
	m_strInitText=strInitText;
	m_bSelectText=bSelectText;
	m_uChar=0;
}

CInPlaceEdit::~CInPlaceEdit()
{
}


BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers

BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_PASTE || (pMsg->message == WM_KEYDOWN && ((GetAsyncKeyState(VK_CONTROL)<0 && pMsg->wParam=='V') || ((GetAsyncKeyState(VK_SHIFT)<0 && pMsg->wParam==VK_INSERT)))))
	{
		BOOL bMultyLine=FALSE;
		CString sText=GetTextFromClipboard(&bMultyLine);
		bMultyLine|=(sText.Replace("\r\n","\n")>0);
		bMultyLine|=(sText.Replace("\n"," ")>0);
		bMultyLine|=(sText.Replace("\t"," ")>0);
		if(bMultyLine){
			SetWindowText(sText);
			//ReplaceSel(sText,TRUE);
			return TRUE;
		}
	}

	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL)
			)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;		    	// DO NOT process further
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}


void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	//	ReleaseCapture();
	//
	CEdit::OnKillFocus(pNewWnd);
	CString str;
	GetWindowText(str);
	// Send Notification to parent of ListView ctrl
	FLNM_ROWSTATE flnmRowState;
	flnmRowState.hdr.code=FLNM_END_INPLACE_EDIT;
	flnmRowState.hdr.hwndFrom=GetParent()->m_hWnd;
	flnmRowState.hdr.idFrom=GetDlgCtrlID();

	flnmRowState.bCollapsed= FALSE;
	flnmRowState.dwData= (DWORD)m_uChar;
	flnmRowState.strText=str;
	flnmRowState.hIRow= m_hIRow;
	flnmRowState.iIItem=m_iIItem;
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(),  (LPARAM) &flnmRowState);
	//
	if(::IsWindow(this->GetSafeHwnd())){
		DestroyWindow();
	}
}

void CInPlaceEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}

void CInPlaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar){
	case VK_LEFT:
	case VK_RIGHT:
		if(GetKeyState(VK_CONTROL)>=0)
		{
			CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
		}
	case VK_PRIOR:
	case VK_NEXT:
	case VK_DOWN:
	case VK_UP:
	case VK_ESCAPE:
	case VK_RETURN:
		//case VK_SPACE:
	case VK_TAB:
		m_uChar=nChar;
		GetParent()->SetFocus();
		break;
	default:
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}
}

void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//if( nChar == VK_ESCAPE || nChar == VK_RETURN || nChar==VK_TAB)
	//{
	//	m_uChar=nChar;
	//	GetParent()->SetFocus();
	//	return;
	//}


	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;

	GetWindowText( str );
	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject( pFont );
	CSize size = dc.GetTextExtent( str );
	dc.SelectObject( pFontDC );
	size.cx += 5;			   	// add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect( &rect );
	GetParent()->GetClientRect( &parentrect );

	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );

	// Check whether control needs to be resized
	// and whether there is space to grow
	if( size.cx > rect.Width() )
	{
		if( size.cx + rect.left < parentrect.right )
			rect.right = rect.left + size.cx;
		else
			rect.right = parentrect.right;
		MoveWindow( &rect );
	}
}

int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_strInitText );
	if(m_bSelectText){
		SetSel( 0, -1 );
	}else{
		SetSel(m_strInitText.GetLength(), -1);
	}
	ShowCaret();
	return 0;
}

//
//
//	CInPlaceList
//
//

CInPlaceList::CInPlaceList(HIROW hIRow, int iIItem, ARR_INPLACE_COMBOITEM* pComboItems , int iSelItem)
{
	m_hIRow=hIRow;
	m_iIItem=iIItem;

	m_aComboItems.Append(*pComboItems);
	m_iSelItem=iSelItem;
	m_uChar=0;

	m_crBackground=RGB(255, 255, 255);
	m_crText=RGB(0, 0, 0);
	m_nFormat=DT_LEFT;
}

CInPlaceList::~CInPlaceList()
{
}


BEGIN_MESSAGE_MAP(CInPlaceList, CComboBox)
	//{{AFX_MSG_MAP(CInPlaceList)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceList message handlers

int CInPlaceList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	for(int i=0; i<m_aComboItems.GetSize(); i++)
		AddString(m_aComboItems[i].strText);
	SetCurSel(m_iSelItem);
	//
	SetFocus();
	return 0;
}

BOOL CInPlaceList::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CInPlaceList::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);
	m_iSelItem=GetCurSel();
	if(m_iSelItem==CB_ERR)
		m_iSelItem=-1;

	// Send Notification to parent of ListView ctrl
	FLNM_ROWSTATE flnmRowState;
	flnmRowState.hdr.code=FLNM_END_INPLACE_COMBO;
	flnmRowState.hdr.hwndFrom=GetParent()->m_hWnd;
	flnmRowState.hdr.idFrom=GetDlgCtrlID();

	flnmRowState.bCollapsed= FALSE;
	flnmRowState.dwData= (DWORD) m_uChar;
	//flnmRowState.dwNotifyData = m_iSelItem<0 ? 0 : GetItemData(m_iSelItem);
	flnmRowState.dwNotifyData = m_iSelItem;
	flnmRowState.strText= str;
	flnmRowState.hIRow= m_hIRow;
	flnmRowState.iIItem=m_iIItem;

	Invalidate();
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(),  (LPARAM) &flnmRowState);
	//
	//DestroyWindow();

	PostMessage( WM_CLOSE );
}

void CInPlaceList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_RETURN && !GetDroppedState()){
		ShowDropDown(TRUE);
		return;
	}
	switch(nChar){
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		//if(GetDroppedState())
		{
			CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
		}
	case VK_LEFT:
	case VK_RIGHT:
	case VK_ESCAPE:
	case VK_RETURN:
	case VK_TAB:
		m_uChar=nChar;
		GetParent()->SetFocus();
		break;
	default:
		CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}
}

void CInPlaceList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CInPlaceList::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	delete this;
}

void CInPlaceList::OnCloseup()
{
	m_uChar=VK_RETURN;
	GetParent()->SetFocus();
}

void CInPlaceList::OnPaint()
{
	Default();
	CDC *pDC = GetDC();
	CRect rcItem;
	GetClientRect(&rcItem);

	//fill background
	pDC->DrawFocusRect(rcItem);
	CRect rcFill=rcItem;
	rcFill.DeflateRect(0, 1, 0, 1);
	CBrush br(m_crBackground);
	pDC->FillRect(rcFill, &br);

	//draw text
	CString str;
	GetWindowText(str);
	pDC->SetBkColor (m_crBackground);
	pDC->SetTextColor (m_crText);

	//
	CFont *pFont=(CFont*)pDC->SelectObject(&m_font);
	rcItem.right-=rcItem.Height()+2;
	pDC->DrawText(str, -1, rcItem, m_nFormat | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
	pDC->SelectObject(pFont);
	rcItem.right+=rcItem.Height()+2;

	//button
	rcItem.left = rcItem.right-rcItem.Height();
	pDC->DrawFrameControl(rcItem, DFC_SCROLL, DFCS_SCROLLDOWN);

	ReleaseDC(pDC);
}

void CInPlaceList::OnSelchange()
{
	Invalidate();
}

//
//
//
//
//

void CIListCtrl::OnDestroy()
{
	bInDestroy=TRUE;
	DeleteAllIRows();
	CListCtrlWithCustomDraw::OnDestroy();
}

BOOL CIListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL;
	BOOL bResult = CListCtrlWithCustomDraw::PreCreateWindow(cs);
	return bResult;
}

int CIListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrlWithCustomDraw::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetExtendedStyle(LVS_EX_FULLROWSELECT);
	return 0;
}


//
void CIListCtrl::EditPrepare(UINT nChar)
{
	HIROW hSelIRow=GetSelectedIRow();
	int iSelIItem=GetSelectedIItem();
	if(GetReadOnly() || hSelIRow==NULL || iSelIItem<0)
		return;

	//
	switch(GetIItemControlType(hSelIRow, iSelIItem, ~FLC_CHECKBOX)){
	case FLC_COMBOBOX:
		EditBegin(nChar);
		break;
	case FLC_EDITWITHCUSTOM:
	case FLC_CUSTOMDIALOG:
		EditBegin(nChar);
		break;
	case FLC_CUSTOMDIALOG2:
		EditBegin(nChar);
		break;
	}
}

void CIListCtrl::EditBegin(UINT nChar)
{
	HIROW hSelIRow=GetSelectedIRow();
	int iSelIItem=GetSelectedIItem();
	CIListRow* pIRow=FindRow(hSelIRow);
	if(GetReadOnly() || hSelIRow==NULL || iSelIItem<0)
		return;

	//
	int iControlType=GetIItemControlType(hSelIRow, iSelIItem, ~FLC_CHECKBOX);
	if(iControlType==FLC_EDITWITHCUSTOM){
		if(nChar==-1){
			iControlType=FLC_EDIT;
		}else{
			iControlType=FLC_CUSTOMDIALOG;
		}
	}
	switch(iControlType){
	case FLC_READONLY:
		if(IsCollapsed(pIRow)){
			Expand(pIRow);
		}else{
			Collapse(pIRow);
		}
		break;
	case FLC_CUSTOMDIALOG:
		{
			if(g_pInPlaceButton==NULL){
				EnsureVisible(hSelIRow, iSelIItem, TRUE);
				//
				//CIListRow* pIRow=FindRow(hSelIRow);
				//
				ASSERT(pIRow!=NULL && pIRow->m_iListIndex>=0);
				CRect rect;
				GetSubItemRectEx(pIRow, iSelIItem, LVIR_BOUNDS, rect);
				CRect rParentRect;
				GetClientRect(&rParentRect);
				rect.IntersectRect(rect,rParentRect);
				rect.left=rect.right-rect.Height();
				CInPlaceButton* pButton=new CInPlaceButton(hSelIRow, iSelIItem, "...");
				pButton->Create(_T("..."), WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON, rect, this, IDC_INPLACE_BUTTON);//|BS_OWNERDRAW
				CIListItem* pIItem=pIRow->GetAt(iSelIItem);
				pIItem->m_fOffsetForControlButton=TRUE;
				UpdateIRow(hSelIRow);
			}else{
				g_pInPlaceButton->m_uChar=((nChar==-1)?VK_RETURN:nChar);
				SetFocus();
			}
		}
		break;
	case FLC_PASSWORD:
	case FLC_EDIT:
		{
			if(g_pInPlaceButton!=NULL){
				g_pInPlaceButton->m_uChar=VK_ESCAPE;
				SetFocus();
			}
			EnsureVisible(hSelIRow, iSelIItem, TRUE);
			//
			if(NotifyOwner(FLNM_STARTEDIT, pIRow, GetIItemText(hSelIRow, iSelIItem)))
				break;
			//
			ASSERT(pIRow!=NULL && pIRow->m_iListIndex>=0);
			CRect rect;
			GetSubItemRectEx(pIRow, iSelIItem, LVIR_BOUNDS, rect);
			//
			CString sText=GetIItemText(hSelIRow, iSelIItem);//nChar ? nChar : GetIItemText(hSelIRow, iSelIItem);
			CInPlaceEdit* pEdit = new CInPlaceEdit(hSelIRow, iSelIItem, sText, TRUE);//!nChar
			pEdit->Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER
				|(m_aFColumns.GetAt(iSelIItem)->m_nFormat)|((iControlType==FLC_PASSWORD)?ES_PASSWORD:0), rect, this, IDC_INPLACE_EDIT);
			pEdit->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
			pEdit->SetFocus();
			//
		}
		break;
#ifndef _NO_XMLHOTKEYSUPPORT
	case FLC_HOTKEY:
		if(lHotkeyInputInProgress==0){
			EnsureVisible(hSelIRow, iSelIItem, TRUE);
			// Эта гор. клавиша не удаляется так как передается внутрь CInPlaceHotKey
			CIHotkey* oHotKey=(CIHotkey*)NotifyOwner(FLNM_STARTEDIT, pIRow, GetIItemText(hSelIRow, iSelIItem));
			if(oHotKey){
				ASSERT(pIRow!=NULL && pIRow->m_iListIndex>=0);
				CRect rect;
				GetSubItemRectEx(pIRow, iSelIItem, LVIR_BOUNDS, rect);
				rect.DeflateRect(1,1);
				rect.left+=1;
				CInPlaceHotKey* pHotKey = new CInPlaceHotKey(hSelIRow, iSelIItem, oHotKey);
				pHotKey->Create("",WS_CHILD|WS_VISIBLE|SS_NOTIFY, rect, this, IDC_INPLACE_HOTKEY);
				pHotKey->m_crBackground = m_crIRowBackground;
				pHotKey->m_crText=m_crIItemText;
				//set font
				CFont* pFont=GetFont();
				LOGFONT logFont;
				pFont->GetLogFont(&logFont);
				if(pIRow->m_fBoldText & FL_BOLD){
					logFont.lfWeight=FW_BOLD;
				}
				pHotKey->m_font.CreateFontIndirect(&logFont);
				pHotKey->SetFocus();
				pHotKey->Invalidate();
			}
		}
		break;
#endif
	case FLC_SLIDER:
		{
			EnsureVisible(hSelIRow, iSelIItem, TRUE);
			//
			CString sText=GetIItemText(hSelIRow, iSelIItem);
			DWORD dwMinMax=NotifyOwner(FLNM_STARTEDIT, pIRow, sText,IDC_INPLACE_SLIDER);
			//
			ASSERT(pIRow!=NULL && pIRow->m_iListIndex>=0);
			CRect rect;
			GetSubItemRectEx(pIRow, iSelIItem, LVIR_BOUNDS, rect);
			//
			CInPlaceSlider* pHotKey = new CInPlaceSlider(hSelIRow, iSelIItem, dwMinMax, WORD(atol(sText)));
			rect.DeflateRect(2,2);
			pHotKey->Create(WS_CHILD|WS_VISIBLE|TBS_TOOLTIPS|TBS_AUTOTICKS|TBS_HORZ|TBS_BOTTOM, rect, this, IDC_INPLACE_SLIDER);//|TBS_FIXEDLENGTH
			pHotKey->SetFocus();
		}
		break;
	case FLC_COMBOBOX:
		{
			EnsureVisible(hSelIRow, iSelIItem, TRUE);

			ARR_INPLACE_COMBOITEM aComboItems;
			//
			FLNM_COMBOSTATE flnmComboState;
			flnmComboState.hdr.code=FLNM_COMBOEXPAND;
			flnmComboState.hdr.hwndFrom=m_hWnd;
			flnmComboState.hdr.idFrom=GetDlgCtrlID();
			flnmComboState.bCollapsed= pIRow==NULL ? NULL : pIRow->m_bCollapsed;
			flnmComboState.dwData= pIRow==NULL ? NULL : pIRow->m_dwData;
			flnmComboState.dwNotifyData= 0;
			flnmComboState.strText= GetIItemText(hSelIRow, iSelIItem);
			flnmComboState.hIRow= pIRow==NULL ? NULL : pIRow->m_hIRow;
			flnmComboState.iIItem=m_iSelectedIItem;
			flnmComboState.iSelectedItem=-1;
			flnmComboState.paComboItems=&aComboItems;
			LRESULT lr=GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(),  (LPARAM) &flnmComboState);

			//
			if(!lr && flnmComboState.paComboItems->GetSize())
			{
				//
				ASSERT(pIRow!=NULL && pIRow->m_iListIndex>=0);
				CRect rect;
				GetSubItemRectEx(pIRow, iSelIItem, LVIR_BOUNDS, rect);
				CRect rParentRect;
				GetClientRect(&rParentRect);
				rect.IntersectRect(rect,rParentRect);
				//rect.left+=FL_BASE_OFFSET;//FL_BRANCH_DECOFFSET
				CInPlaceList* pCombo= new CInPlaceList(hSelIRow, iSelIItem,
					flnmComboState.paComboItems, flnmComboState.iSelectedItem);
				int nHeight=rect.Height();
				rect.bottom += 10 * rect.Height();
				pCombo->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL|CBS_DROPDOWNLIST|
					CBS_DISABLENOSCROLL, rect, this, IDC_INPLACE_COMBO);
				pCombo->m_crBackground = m_crIRowBackground;
				pCombo->m_crText=m_crIItemText;

				//set font
				CFont* pFont=GetFont();
				LOGFONT logFont;
				pFont->GetLogFont(&logFont);
				if(pIRow->m_fBoldText & FL_BOLD){
					logFont.lfWeight=FW_BOLD;
				}
				pCombo->m_font.CreateFontIndirect(&logFont);
				//pCombo->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
				pCombo->ModifyStyle(0, WS_BORDER);
				CRect rcWin;
				pCombo->GetWindowRect(&rcWin);
				pCombo->SetItemHeight(-1, nHeight-(rcWin.Height()-pCombo->GetItemHeight(-1)));

				pCombo->m_nFormat=m_aFColumns.GetAt(iSelIItem)->m_nFormat;
			}
		}
		break;
	case FLC_CUSTOMDIALOG2:
		{
			ASSERT(pIRow!=NULL && pIRow->m_iListIndex>=0);
			EnsureVisible(hSelIRow, iSelIItem, TRUE);
			FLNM_ROWSTATE flnmRowState;
			flnmRowState.hdr.code=FLNM_END_INPLACE_BUTTON;
			flnmRowState.hdr.hwndFrom=m_hWnd;
			flnmRowState.hdr.idFrom=GetDlgCtrlID();
			//
			flnmRowState.bCollapsed= pIRow==NULL ? NULL : pIRow->m_bCollapsed;
			flnmRowState.dwData=(nChar==VK_LBUTTON?VK_RETURN:nChar);
			flnmRowState.dwNotifyData=0;
			flnmRowState.strText= "";
			flnmRowState.hIRow= pIRow==NULL ? NULL : pIRow->m_hIRow;
			flnmRowState.iIItem=GetSelectedIItem();
			CIListItem* pIItem=pIRow->GetAt(flnmRowState.iIItem);
			if(flnmRowState.dwData==VK_RETURN || flnmRowState.dwData==VK_SPACE){
				pIItem->m_bChecked=TRUE;
				UpdateIRow(hSelIRow);
				Sleep(200);
				pIItem->m_bChecked=FALSE;
				UpdateIRow(hSelIRow);
			}
			GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(),  (LPARAM) &flnmRowState);
		}
		break;
	}
}

//
void CIListCtrl::EditFinish(FLNM_ROWSTATE *pRowState)
{
	HIROW hSelIRow=GetSelectedIRow();
	int iSelIItem=GetSelectedIItem();
	CIListRow* pIRow=FindRow(hSelIRow);
	if(pIRow){
		CIListItem* pIItem=pIRow->GetAt(iSelIItem);
		if(pIItem){
			pIItem->m_fOffsetForControlButton=FALSE;
		}
		UpdateIRow(hSelIRow);
	}

	/*switch((UINT)pRowState->dwData){
	case VK_ESCAPE:
	case VK_RETURN:
	EditPrepare();
	break;
	default:
	Navigation((UINT)pRowState->dwData, TRUE);
	break;
	}*/
}


BOOL CIListCtrl::Navigation(UINT nChar, BOOL bControlLostFocus)
{
	BOOL bResult=TRUE;
	HIROW hSelIRow=GetSelectedIRow();
	int iSelIItem=GetSelectedIItem();
	CIListRow* pIRow=FindRow(hSelIRow);
	if(pIRow==NULL && (nChar==VK_LEFT || nChar==VK_RIGHT || nChar==VK_UP || nChar==VK_DOWN)){
		if(hSelIRow==NULL){
			pIRow=(CIListRow*) GetItemData(0);
			if(pIRow){
				hSelIRow=pIRow->m_hIRow;
			}
		}
	}
	if(pIRow==NULL){
		return FALSE;
	}

	switch(nChar){
	case VK_LEFT:
		if(!GetReadOnly())
		{
			if(iSelIItem>0)
			{
				SelectIItem(hSelIRow, iSelIItem-1);
				EnsureVisible(hSelIRow, GetSelectedIItem(), FALSE);
			}
		}
		break;

	case VK_RIGHT:
		if(!GetReadOnly())
		{
			if(iSelIItem<(GetFColumnCount()-1))
			{
				SelectIItem(hSelIRow, iSelIItem+1);
				EnsureVisible(hSelIRow, GetSelectedIItem(), FALSE);
			}
		}
		UpdateIRow(hSelIRow);
		break;

	case VK_HOME:
		if(!GetReadOnly())
		{
			SelectIItem(hSelIRow, 0);
			EnsureVisible(hSelIRow, GetSelectedIItem(), FALSE);
		}
		UpdateIRow(hSelIRow);
		break;

	case VK_END:
		if(!GetReadOnly())
		{
			SelectIItem(hSelIRow, GetFColumnCount()-1);
			EnsureVisible(hSelIRow, GetSelectedIItem(), FALSE);
		}
		UpdateIRow(hSelIRow);
		break;

	case VK_UP:
		{
			if(hSelIRow!=NULL)
			{
				int iListIndex=pIRow->m_iListIndex;
				if(iListIndex>0){
					CListCtrl::EnsureVisible(iListIndex-1, FALSE);
					pIRow=(CIListRow*) GetItemData(iListIndex-1);
					SelectIRow(pIRow->m_hIRow, TRUE);
					SelectIItem(GetSelectedIRow(), GetSelectedIItem());
				}
			}
		}
		break;

	case VK_DOWN:
		{
			if(hSelIRow!=NULL)
			{
				int iListIndex=pIRow->m_iListIndex;
				if(iListIndex<GetItemCount()-1){
					CListCtrl::EnsureVisible(iListIndex+1, FALSE);
					pIRow=(CIListRow*) GetItemData(iListIndex+1);
					SelectIRow(pIRow->m_hIRow, TRUE);
					SelectIItem(GetSelectedIRow(), GetSelectedIItem());
				}
			}
		}
		break;

	case VK_TAB:
		{
			if(bIgnoreTabKey){
				bResult=FALSE;
				break;
			}
			if(!GetReadOnly())
			{
				ASSERT(pIRow!=NULL);

				if(GetKeyState(VK_SHIFT)>=0)
				{
					if(iSelIItem<(GetFColumnCount()-1)){
						SelectIItem(hSelIRow, iSelIItem+1);
						EnsureVisible(hSelIRow, GetSelectedIItem(), FALSE);
					}
					else{
						int iListIndex=pIRow->m_iListIndex;
						if(iListIndex<GetItemCount()-1){
							pIRow=(CIListRow*) GetItemData(iListIndex+1);
							SelectIItem(pIRow->m_hIRow, 0);
							CListCtrl::EnsureVisible(iListIndex+1, FALSE);
						}
					}
				}
				else{
					if(iSelIItem>0){
						SelectIItem(hSelIRow, iSelIItem-1);
						EnsureVisible(hSelIRow, GetSelectedIItem(), FALSE);
					}
					else{
						int iListIndex=pIRow->m_iListIndex;
						if(iListIndex>0){
							pIRow=(CIListRow*) GetItemData(iListIndex-1);
							SelectIItem(pIRow->m_hIRow, GetFColumnCount()-1);
							CListCtrl::EnsureVisible(iListIndex-1, FALSE);
						}
					}
				}
			}
		}
		break;

	case VK_ADD:
		if(hSelIRow!=NULL && IsCollapsed(hSelIRow))
			Expand(hSelIRow);
		break;

	case VK_SUBTRACT:
		if(hSelIRow!=NULL && !IsCollapsed(hSelIRow))
			Collapse(hSelIRow);
		break;

	case VK_SPACE:
		if(hSelIRow!=NULL && iSelIItem>=0){
			int iType=GetIItemControlType(hSelIRow, iSelIItem);
			if((iType & FLC_CHECKBOX) != 0){
				SetIItemCheck(hSelIRow, iSelIItem, !IsIItemChecked(hSelIRow, iSelIItem));
				break;
			}
		}

	case VK_F2:
	case VK_RETURN:
		EditBegin(VK_RETURN);
		return TRUE;

	case VK_NEXT:
		if(hSelIRow!=NULL)
		{
			int iListIndex=pIRow->m_iListIndex;
			if(iListIndex+GetCountPerPage()-2<GetItemCount()){
				CListCtrl::EnsureVisible(iListIndex+GetCountPerPage()-2, FALSE);
				pIRow=(CIListRow*) GetItemData(iListIndex+GetCountPerPage()-2);
				SelectIRow(pIRow->m_hIRow, TRUE);
				SelectIItem(GetSelectedIRow(), GetSelectedIItem());
			}
			else if(iListIndex<GetItemCount()-1)
			{
				CListCtrl::EnsureVisible(GetItemCount()-1, FALSE);
				pIRow=(CIListRow*) GetItemData(GetItemCount()-1);
				SelectIRow(pIRow->m_hIRow, TRUE);
				SelectIItem(GetSelectedIRow(), GetSelectedIItem());
			}
		}
		break;

	case VK_PRIOR:
		if(hSelIRow!=NULL)
		{
			int iListIndex=pIRow->m_iListIndex;
			if(iListIndex-GetCountPerPage()+2>=0){
				CListCtrl::EnsureVisible(iListIndex-GetCountPerPage()+2, FALSE);
				pIRow=(CIListRow*) GetItemData(iListIndex-GetCountPerPage()+2);
				SelectIRow(pIRow->m_hIRow, TRUE);
				SelectIItem(GetSelectedIRow(), GetSelectedIItem());
			}
			else if(iListIndex>0){
				CListCtrl::EnsureVisible(0, FALSE);
				pIRow=(CIListRow*) GetItemData(0);
				SelectIRow(pIRow->m_hIRow, TRUE);
				SelectIItem(GetSelectedIRow(), GetSelectedIItem());
			}
		}
		break;

	default:
		bResult=FALSE;
		break;
	}
	if(bResult)
		EditPrepare(nChar);


	return bResult;
}

//
void CIListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	HIROW hSelIRow=GetSelectedIRow();
	int iSelIItem=GetSelectedIItem();
	int iType=0;
	if(iSelIItem!=-1){
		iType=GetIItemControlType(hSelIRow, iSelIItem, ~FLC_CHECKBOX);
	}
	if(!GetReadOnly() && iSelIItem!=-1 && ((iType & FLC_EDIT)!=0 || (iType & FLC_PASSWORD)!=0) && ::IsCharAlphaNumeric(nChar))
		EditBegin(nChar);
	else
		CListCtrlWithCustomDraw::OnChar(nChar, nRepCnt, nFlags);
}

void CIListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrlWithCustomDraw::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CIListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrlWithCustomDraw::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CIListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY	*pHDN = (HD_NOTIFY*)lParam;
	LPNMHDR pNH = (LPNMHDR) lParam;

	if(pNH->hwndFrom == GetHeaderCtrl()->m_hWnd)
		SetFocus();

	return CListCtrlWithCustomDraw::OnNotify(wParam, lParam, pResult);
}

//-------------------------------------------------------------------------
int CIListCtrl::HitTestEx(CPoint &point, int *col, CIListRow*& pIRow) const
{
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	CRect rtHeaderRect;
	pHeader->GetWindowRect(&rtHeaderRect);
	ScreenToClient(&rtHeaderRect);
	if(rtHeaderRect.PtInRect(point)){
		return -1;
	}

	int colnum = 0;
	int row = HitTest( point, NULL );
	pIRow=NULL;
	if(row>=0){
		pIRow=(CIListRow*)GetItemData(row);
	}
	if( col ) *col = 0;
	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;
	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage()+1;
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Loop through the visible rows
	for( ;row < bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect(0,0,0,0);
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) ){
			// Now find the column
			if(col==NULL){
				return row;// Column is not needed
			}else{
				for( colnum = 0; colnum < nColumnCount; colnum++ ){
					int colwidth = GetColumnWidth(colnum);
					if( point.x >= rect.left && point.x <= (rect.left + colwidth )){
						// Определяем с учетом ColSpan-а
						if(pIRow){
							int iItemsCount=pIRow->m_aItems.GetSize();
							for(int i=0;i<colnum;i++){
								CIListItem* pItemPrev=pIRow->m_aItems[i];
								int iSpan=pItemPrev->m_iColSpan;
								if(iSpan>0){
									// Берем размер с учетом перекрываемых ячеек
									if(i+iSpan>=iItemsCount){
										iSpan=iItemsCount-i-1;
									}
									if(pItemPrev && (i+iSpan)>=colnum){
										colnum=i;
										break;
									}
								}
							}
							*col = colnum;
							return row;
						}
					}
					rect.left += colwidth;
				}//for
			}
		}//if
		
	}	//for
	
	return -1;
}

BOOL CIListCtrl::SmartGetSubItemRect(CIListRow* pIRow, int& iSubItem, int nArea, CRect& ref)
{
	ref.SetRectEmpty();
	if(pIRow->m_iListIndex==-1){
		return FALSE;
	}
	BOOL bRes=GetSubItemRect(pIRow->m_iListIndex, iSubItem, nArea, ref);
	if(pIRow->Count()<=1){
		return bRes;
	}
	if(iSubItem==0){
		ref.right=ref.left+GetColumnWidth(0);
	}
	//
	int iItemsCount=pIRow->m_aItems.GetSize();
	if(iSubItem>=iItemsCount){
		return FALSE;
	}
	CIListItem* pItem=pIRow->m_aItems[iSubItem];
	if(pItem){
		int iSpan=pItem->m_iColSpan;
		if(iSpan>0){
			// Берем размер с учетом перекрываемых ячеек
			if(iSubItem+iSpan>=iItemsCount){
				iSpan=iItemsCount-iSubItem-1;
			}
			CRect span(0,0,0,0);
			if(GetSubItemRect(pIRow->m_iListIndex, iSubItem+iSpan, nArea, span)){
				ref.right=span.right;
				bRes=2;
			}
		}else{
			// Если где-то ДО есть перекрывающая данную ячейку, то - пусто
			for(int i=0;i<iSubItem;i++){
				CIListItem* pItemPrev=pIRow->m_aItems[i];
				if(pItemPrev){
					int iSpan=pItemPrev->m_iColSpan;
					if(iSpan>0){
						if(i+iSpan>=iItemsCount){
							iSpan=iItemsCount-i-1;
						}
						if((i+iSpan)>=iSubItem){
							//ref.left=ref.right=ref.top=ref.bottom=0;
							iSubItem=i;
							SmartGetSubItemRect(pIRow, i, nArea, ref);
							return 3;
						}
					}
				}
			}
		}
	}
	//
	return bRes;
}

// draw cell
bool CIListCtrl::OnSubItemDraw(CDC* pDCIn, int nItem, int nSubItem, UINT nState, LPARAM lParam)
{
	if(m_bStopUpdateScreen){
		return true;
	}
	CDC* pDC=pDCIn;
	CIListRow* pIRow=(CIListRow*)GetItemData(nItem);
	if(!pIRow || pIRow->m_hIRow==NULL){
		return false;
	}
	CRect rcSubItem;
	int iStartSubItem=nSubItem;
	int iFindRectRes=SmartGetSubItemRect(pIRow, nSubItem, LVIR_BOUNDS, rcSubItem);
	if(!iFindRectRes){
		return true;
	}
	if(rcSubItem.IsRectEmpty()){
		return true;
	}
	CIListItem* pIItem=pIRow->GetAt(nSubItem);
	if(!pIItem){
		return false;
	}
	COLORREF cListBg=m_crIRowBackground,cListFg=m_crIItemText;
	DWORD dwListPaleColor=RGB(
		GetRValue(cListBg)+int(GetRValue(cListFg)-GetRValue(cListBg))/4,
		GetGValue(cListBg)+int(GetGValue(cListFg)-GetGValue(cListBg))/4,
		GetBValue(cListBg)+int(GetBValue(cListFg)-GetBValue(cListBg))/4);
	if(long(pIItem->m_BgColor)!=-1){
		cListBg=pIItem->m_BgColor;
	}
	if(long(pIItem->m_TextColor)!=-1){
		cListFg=pIItem->m_TextColor;
	}
	/*if(m_crSelectedBold==-1){
		CRect rt;
		GetWindowRect(&rt);
		this->ScreenToClient(&rt);
		CBrush br(cListFg);
		::FrameRect(*pDC,&rt,br);
	}*/
	if(iFindRectRes==3){
		bool bRes=TRUE;
		CIListItem* pIItemThis=pIRow->GetAt(iStartSubItem);
		if(pIItem->m_dwFlickerFlag==pIItemThis->m_dwFlickerFlag){
			pIItem->m_dwFlickerFlag++;
			// Нужно обновлять предыдущую ячейку!!!
			InvalidateRect(rcSubItem,FALSE);
		}else{
			pIItemThis->m_dwFlickerFlag=pIItem->m_dwFlickerFlag;
		}
		return bRes;
	}
	//
	CRect rect(&rcSubItem);
	DWORD dwControlType=pIItem->m_iControlType;
	//
	CPoint point;	
	point= rect.TopLeft();
	point.x+=FL_BASE_OFFSET;
	if(IsFColumnTree(nSubItem)){
		if(bDoNotShowTree){
			point.x+=0;
		}else{
			point.x+=(pIRow->m_iDeep-1)*FL_IMAGE_WIDTH-(pIRow->m_iDeep-1)*FL_BRANCH_DECOFFSET;
		}
	}
	//
	COLORREF crBackground=0, crText=255;
	BOOL bSelect= pIRow->m_hIRow==GetSelectedIRow()? TRUE : FALSE;
	BOOL bSelectAsBrd =((pIRow->m_fBoldText & FL_SELBORD)!=0) && (pIItem->m_BgColor != -1);
	crBackground = cListBg;			
	crText = cListFg;
	//выбор цвета текста и фона
	if (bSelect && !bSelectAsBrd)//строка выделена
	{
		if((!GetReadOnly() && nSubItem==GetSelectedIItem()) || m_crSelectedBold==-1)
		{//текущий элемент является выделенным
			if(m_crSelectedIItemBackground!=-1){
				crBackground=m_crSelectedIItemBackground;
			}
			if(m_crSelectedIItemText!=-1){
				crText=m_crSelectedIItemText;
			}
		}
	}
	DWORD dwTextPaleColor=RGB(
		GetRValue(crText)+int(GetRValue(crBackground)-GetRValue(crText))/2,
		GetGValue(crText)+int(GetGValue(crBackground)-GetGValue(crText))/2,
		GetBValue(crText)+int(GetBValue(crBackground)-GetBValue(crText))/2);
	if(pIRow->m_fBoldText & FL_PALEBG){
		crBackground=dwTextPaleColor;
	}
	// Начало собственно разрисовки ячейки
	CRect rtInitialCellRect(rect);
	CBrush br(crBackground);
	pDC->FillRect(rect, &br);
	if(bDoNotShowGrid==2 && nSubItem>0){
		CPen* oldPen=NULL;
		CPen pen(PS_SOLID,1,dwListPaleColor);
		oldPen=pDC->SelectObject(&pen);
		pDC->MoveTo(rect.left,rect.bottom-1);
		pDC->LineTo(rect.right-1,rect.bottom-1);
		//pDC->LineTo(rect.right-1,rect.top);
		rect.right=rect.right-1;//Чтобы дальнейшие действия не затирали границу
		if(oldPen){
			pDC->SelectObject(oldPen);
		}
	}
	if(!bDoNotShowGrid){
		CPen* oldPen=NULL;
		CPen pen(PS_SOLID,1,dwListPaleColor);
		oldPen=pDC->SelectObject(&pen);
		pDC->MoveTo(rect.left,rect.bottom-1);
		pDC->LineTo(rect.right-1,rect.bottom-1);
		pDC->LineTo(rect.right-1,rect.top);
		rect.right=rect.right-1;//Чтобы дальнейшие действия не затирали границу
		if(oldPen){
			pDC->SelectObject(oldPen);
		}
	}
	if(bSelect && bSelectAsBrd){
		CPen* oldPen=NULL;
		CPen pen(PS_SOLID,1,m_crSelectedIItemBackground);
		oldPen=pDC->SelectObject(&pen);
		pDC->MoveTo(rect.left,rect.bottom-2);
		pDC->LineTo(rect.left,rect.top);
		pDC->LineTo(rect.right-1,rect.top);
		pDC->LineTo(rect.right-1,rect.bottom-2);
		pDC->LineTo(rect.left,rect.bottom-2);
		if(oldPen){
			pDC->SelectObject(oldPen);
		}
	}
	//если текущая колонка содержит дерево, то рисуем значок + или -
	CImageList* il=GetImageList(LVSIL_SMALL);
	BOOL fTree=m_aFColumns.GetAt(nSubItem)->m_fTree;
	if(fTree && (pIRow->m_fBoldText & FL_SKIPTRI)==0)
	{
		if(il!=NULL){
			// подвод
			if(pIRow->m_iDeep>1){
				BOOL bLast=FALSE;
				CIListRow* pIRowLast=FindRow(GetLastChildIRow(GetParentIRow(pIRow->m_hIRow)));
				if(pIRowLast==pIRow){
					bLast=TRUE;
				}
				if(!bSkipTreeLines){
					il->Draw(pDC, bLast?3:2, point, ILD_TRANSPARENT);//ILD_NORMAL | ILD_MASK);
				}
				CPoint point1(point);
				point1.x-=FL_IMAGE_WIDTH;
				for(int i=pIRow->m_iDeep-2;i>0;i--){
					bLast=FALSE;
					if(pIRowLast){
						HIROW hParentIRow=GetParentIRow(pIRowLast->m_hIRow);
						if(hParentIRow){
							pIRowLast=FindRow(hParentIRow);
							if(pIRowLast && pIRowLast==FindRow(GetLastChildIRow(GetParentIRow(pIRowLast->m_hIRow)))){
								bLast=TRUE;
							}
						}
					}
					if(!bLast){
						if(!bSkipTreeLines){
							il->Draw(pDC, 4, point1, ILD_TRANSPARENT);//ILD_NORMAL | ILD_MASK);
						}
					}
					point1.x-=FL_IMAGE_WIDTH;
				}
				if(bDrawTree0){
					if(!bSkipTreeLines){
						il->Draw(pDC, 4, point1, ILD_TRANSPARENT);//ILD_NORMAL | ILD_MASK);
					}
				}
			}else{
				if(bDrawTree0){
					// Просто "Типа дерево"
					BOOL bLast=FALSE;
					CIListRow* pIRowLast=FindRow(GetLastChildIRow(FL_ROOT));
					if(pIRowLast && pIRowLast==FindRow(pIRow->m_hIRow)){
						bLast=TRUE;
					}
					if(!bSkipTreeLines){
						il->Draw(pDC, bLast?3:2, point, ILD_TRANSPARENT);//ILD_TRANSPARENT
					}
				}
			}
			if(pIRow->GetChildIRowCount()){
				// +/-
				il->Draw(pDC, (pIRow->m_bCollapsed ? 0 : 1), point, ILD_TRANSPARENT);//ILD_NORMAL | ILD_MASK);
			}
			point.x+=FL_IMAGE_WIDTH;
		}
	}
	//checkbox
	if(pIItem->m_iControlType & FLC_CHECKBOX)
	{
		CRect rectCheckBox=rect;
		rectCheckBox.left=point.x-2;
		rectCheckBox.top=point.y;
		rectCheckBox.right=rectCheckBox.left+ FL_IMAGE_WIDTH;
		rectCheckBox.bottom-=1;//rectCheckBox.top+ FL_IMAGE_WIDTH;

		if(pIItem->m_bChecked)
		{
			if(pIItem->m_iCheckedImage<0)				
				pDC->DrawFrameControl(rectCheckBox, DFC_BUTTON, DFCS_FLAT|DFCS_CHECKED|DFCS_BUTTONCHECK);
			else
				il->Draw(pDC, pIItem->m_iCheckedImage, point, ILD_TRANSPARENT);
		}
		else{
			if(pIItem->m_iUncheckedImage<0)				
				pDC->DrawFrameControl(rectCheckBox, DFC_BUTTON, DFCS_FLAT|DFCS_BUTTONCHECK);
			else
				il->Draw(pDC, pIItem->m_iUncheckedImage, point, ILD_TRANSPARENT);
		}
		point.x+=FL_IMAGE_WIDTH;//::GetSystemMetrics(SM_CXSMICON);
	}
	//рисуем image
	if(pIItem->m_iImageIndex>=0 && il!=NULL)
	{
		il->Draw(pDC, pIItem->m_iImageIndex, point, ILD_TRANSPARENT);
		point.x+=FL_IMAGE_WIDTH;
	}
	else if(IsFColumnImageOffset(nSubItem)){
		point.x+=FL_IMAGE_WIDTH;
	}
	// Восстанавливаем прямоугольник...
	rect.left=point.x;
	rect.top=point.y;
	// Если это - кнопка, то рисуем кнопку
	if((dwControlType & FLC_CUSTOMDIALOG2) && !bShowButtonsAsLinks){
		rect.DeflateRect(0,0,0,1);
		if(rect.right-rect.left>FL_IMAGE_WIDTH*2+5){
			rect.DeflateRect(0,0,FL_IMAGE_WIDTH*2,0);
		}
		pDC->DrawFrameControl( rect, DFC_BUTTON, DFCS_BUTTONPUSH|(pIItem->m_bChecked?DFCS_PUSHED:DFCS_FLAT));
		rect.DeflateRect(10,0,10,0);
		DWORD dwTextProps=DT_CENTER;
		CString sText=pIItem->m_strText;
		int iTabPosition=sText.Find("\t");
		if(iTabPosition!=-1){
			sText.Replace("\t"," ");
			CRect size(0,0,0,0);
			::DrawText(*pDC, sText, -1, &size, DT_CALCRECT);
			if(size.Width()>rect.Width()){
				sText=sText.Mid(iTabPosition+1);
			}
		}
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(sText,-1, rect, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | dwTextProps);
		return true;
	}
	pDC->SetBkColor(crBackground);
	pDC->SetTextColor(crText);
	UINT nFormat=m_aFColumns.GetAt(nSubItem)->m_nFormat;
	if(pIItem->m_fOffsetForControlButton)
		rect.right-=rect.Height()+2;
	//рисуем текст
	CFont font;
	CFont* pItemFont=NULL;
	if(pIRow->m_fBoldText>0 || (m_crSelectedBold>0 && bSelect)){
		LOGFONT logFont;
		pItemFont=pDC->GetCurrentFont();
		pItemFont->GetLogFont(&logFont);
		if((pIRow->m_fBoldText & FL_BOLD) || (m_crSelectedBold>0 && bSelect)){
			logFont.lfWeight=FW_BOLD;
		}
		if(pIRow->m_fBoldText & FL_STRIKE){
			logFont.lfStrikeOut=TRUE;
		}
		if(pIRow->m_fBoldText & FL_UNLINE){
			logFont.lfUnderline=TRUE;
		}
		if(pIRow->m_fBoldText & FL_ITALIC){
			logFont.lfItalic=TRUE;
		}
		if(pIRow->m_fBoldText & FL_PALE){
			pDC->SetTextColor(dwTextPaleColor);
		}
		font.CreateFontIndirect(&logFont);
		pItemFont=(CFont*)pDC->SelectObject(&font);
	}
	CString sText=pIItem->m_strText;
	if(pIItem->m_iControlType & FLC_PASSWORD){
		sText=CString('*',sText.GetLength());
	}
	if(nFormat!=DT_LEFT){// Сложное форматирование
		::SetBkMode(*pDC, TRANSPARENT);
		pDC->DrawText(sText,-1, rect, nFormat | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
	}else{// Простое форматирование - быстрая функция
		pDC->ExtTextOut(rect.left,rect.top,ETO_CLIPPED,&rect,sText,NULL);
	}
	if(pItemFont!=NULL){// Выбираем старый фонт, удаляем созданный
		pDC->SelectObject(pItemFont)->DeleteObject();
	}
	return true; 
}

//
//
//		CInPlaceSlider
//
//
BEGIN_MESSAGE_MAP(CInPlaceSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CInPlaceSlider)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CInPlaceSlider::CInPlaceSlider(HIROW hIRow, int iIItem, DWORD dwMinMax, WORD wStartFrom)
{
	m_hIRow=hIRow;
	m_iIItem=iIItem;
	wMin=LOWORD(dwMinMax);
	wMax=HIWORD(dwMinMax);
	wPos=wStartFrom;
	dwClickPreventer=GetTickCount();
	g_CurrentActiveSlider=this;
}

CInPlaceSlider::~CInPlaceSlider()
{
	g_CurrentActiveSlider=0;
}

#define SLIDEREDIT_W	30
BOOL CInPlaceSlider::PreCreateWindow(CREATESTRUCT& cs)
{
	rtPos.SetRect(cs.x,cs.y,cs.x+cs.cx,cs.y+cs.cy);
	rtePos.CopyRect(rtPos);
	rtPos.left+=SLIDEREDIT_W;
	rtePos.right=rtPos.left-1;
	cs.x=rtPos.left;
	cs.y=rtPos.top;
	cs.cx=rtPos.Width();
	cs.cy=rtPos.Height();
	return TRUE;
}

int CInPlaceSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSliderCtrl::OnCreate(lpCreateStruct) == -1){
		return -1;
	}
	char szTmp[128]={0};
	sprintf(szTmp,"%i",wPos);
	if(ed.CreateEx(0,"EDIT",szTmp,WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rtePos, GetParent(), 0)){
		ed.ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	}
	SetTipSide(TBTS_TOP);
	CFont* font = GetParent()->GetFont();
	ed.SetFont(font);
	SetFont(font);
	CRect rt;
	GetWindowRect(&rt);
	SetRange(wMin,wMax,TRUE);
	SetTicFreq(10);
	MySetPos(wPos);
	return 0;
}

void CInPlaceSlider::UpdatePos()
{
	MySetPos(GetPos());
}

void CInPlaceSlider::MySetPos(long lNewPos)
{
	if(IsWindow(ed.GetSafeHwnd())){
		char szTmp[128]={0};
		sprintf(szTmp,"%i",lNewPos);
		ed.SetWindowText(szTmp);
	}
	SetPos(lNewPos);
}

BOOL CInPlaceSlider::PreTranslateMessage(MSG* pMsg)
{
	if(dwClickPreventer!=0 && (pMsg->message==WM_LBUTTONUP || pMsg->message==WM_RBUTTONUP || GetTickCount()-dwClickPreventer>1000)){
		dwClickPreventer=0;
	}
	if(pMsg->message==WM_CHAR){
		static DWORD dwLastKeyPress=0;
		char c=pMsg->wParam;
		if(c>='0' && c<='9'){
			static int iCurPos=0;
			CString sCurValue;
			sCurValue.Format("%lu",GetPos());
			if(GetTickCount()-dwLastKeyPress>1000 || sCurValue.GetLength()>=3){
				iCurPos=0;
				sCurValue=c;
			}else{
				iCurPos++;
			}
			if(iCurPos==sCurValue.GetLength()){
				sCurValue+="0";
			}
			if(iCurPos<sCurValue.GetLength()){
				sCurValue.SetAt(iCurPos,c);
				MySetPos(atol(sCurValue));
				dwLastKeyPress=GetTickCount();
			}
		}
	}
	if(pMsg->message==WM_LBUTTONDOWN || pMsg->message==WM_RBUTTONDOWN || (pMsg->message==WM_MOUSEMOVE && (GetAsyncKeyState(VK_LBUTTON)<0 || GetAsyncKeyState(VK_RBUTTON)<0))){
		if(dwClickPreventer==0){
			CRect rt;
			CPoint pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			GetChannelRect(&rt);
			CRect rtEd;
			ed.GetWindowRect(&rtEd);
			pt.x-=rt.left;//SLIDEREDIT_W pt.x+=rtEd.Width();
			pt.y-=rt.top;
			
			/*GetWindowRect(&rt);
			rt.OffsetRect(-SLIDEREDIT_W,0);
			pt.x=pt.x-rt.left;*/
			long lNewPos=0;
			if(pt.x<0){
				lNewPos=wMin;
			}else if(pt.x>=rt.Width()){
				lNewPos=wMax;
			}else if(pt.x>=0 && pt.x<rt.Width()){
				lNewPos=wMin+long(double(pt.x)/rt.Width()*(wMax-wMin));
			}
			MySetPos(lNewPos);
			return TRUE;
		}
	}
	return CSliderCtrl::PreTranslateMessage(pMsg);
}

void CInPlaceSlider::OnKillFocus(CWnd* pNewWnd)
{
	if(!IsWindow(this->GetSafeHwnd())){
		return;
	}
	CWnd* pParent=GetParent();
	if(pParent && !IsWindow(pParent->GetSafeHwnd())){
		return;
	}
	char szTemp[128]={0};
	sprintf(szTemp,"%i",GetPos());
	// Send Notification to parent of ListView ctrl
	FLNM_ROWSTATE flnmRowState;
	flnmRowState.hdr.code=FLNM_END_INPLACE_EDIT;
	flnmRowState.hdr.hwndFrom=GetParent()->GetSafeHwnd();
	flnmRowState.hdr.idFrom=IDC_INPLACE_DATE;//GetDlgCtrlID();
	flnmRowState.dwNotifyData=GetPos();
	flnmRowState.bCollapsed= FALSE;
	flnmRowState.strText=szTemp;
	flnmRowState.hIRow= m_hIRow;
	flnmRowState.iIItem=m_iIItem;
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(),  (LPARAM) &flnmRowState);
	CSliderCtrl::OnKillFocus(pNewWnd);
	DestroyWindow();
}

void CInPlaceSlider::OnNcDestroy()
{
	CSliderCtrl::OnNcDestroy();
	delete this;
}

void CInPlaceSlider::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInPlaceSlider::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL IsPromiscEnable()
{
	return IsPluginOn("WP_KeyMaster")?1:0;
}

CString& GetPluginsDirectoryX()
{
	static CString sDir;
	return sDir;
}

#ifndef _NO_XMLHOTKEYSUPPORT
//////////////////////
// CInPlaceHotKey
BEGIN_MESSAGE_MAP(CInPlaceHotKey, CStatic)
	//{{AFX_MSG_MAP(CInPlaceHotKey)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#include <mmsystem.h>
CString _l(const char* szText,const char* szDef);
#define PROC_HKNAME(x) if(x==""){x="<";x+=HotkeysSkipDD()?_l("Press hotkey, DEL to clear",0):_l("Press hotkey. @RMB for more",0);x+=">";};
typedef BOOL (WINAPI *_SetPromiscMode)(HWND,RECT*);
typedef HWND& (WINAPI *_WKIHOTKEYS_EventsHandlerDuplicateWindow)();
typedef int (WINAPI *_WKIHOTKEYS_NotifyOnAddEvent)(const char* szEvent);
#define HK_BUTTONS	5
CCriticalSection csHotkeyTrackPopup;
CInPlaceHotKey::CInPlaceHotKey(HIROW hIRow, int iIItem, CIHotkey* oHotKey)
{
	pRParent=0;
	m_bPromiscON=FALSE;
	m_hIRow = hIRow;
	m_iIItem = iIItem;
	m_dwHotKeyStartup = oHotKey;
	m_dwHotKey = m_dwHotKeyStartup;
	sStartupValue=GetIHotkeyName(*m_dwHotKeyStartup);
	if(lHotkeyInputInProgress){
		lHotkeyInputInProgress->SendMessage(WM_KILLFOCUS,0,0);
	}
	rcHKNBt.left=rcHKNBt.right=rcHKNBt.top=rcHKNBt.bottom=0;
	rcHKNBt1.left=rcHKNBt1.right=rcHKNBt1.top=rcHKNBt1.bottom=0;
	rcHKNBt2.left=rcHKNBt2.right=rcHKNBt2.top=rcHKNBt2.bottom=0;
	rcHKNBt3.left=rcHKNBt3.right=rcHKNBt3.top=rcHKNBt3.bottom=0;
	rcCloseBt.left=rcCloseBt.right=rcCloseBt.top=rcCloseBt.bottom=0;
	rcMenuBt.left=rcMenuBt.right=rcMenuBt.top=rcMenuBt.bottom=0;
	bHKNBt=bMenuBt=bCloseBt=0;
	lHotkeyInputInProgress=this;
	//HideCaret();
}

int CInPlaceHotKey::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Set the proper font
	pRParent=GetParent();
	CFont* font = GetParent()->GetFont();
	SetFont(font);
	HideCaret();
	PROC_HKNAME(sStartupValue);
	SetWindowText(sStartupValue);
	HINSTANCE hLLHookInst=GetModuleHandle("wkkeyh.dll");
	if(hLLHookInst && IsPromiscEnable()){
		_SetPromiscMode fp=(_SetPromiscMode)GetProcAddress(hLLHookInst,"SetPromiscMode");
		if(fp){
			CRect rt;
			GetWindowRect(&rt);
			rt.right-= HK_BUTTONS*rt.Height()-HK_BUTTONS;
			m_bPromiscON=(*fp)(this->GetSafeHwnd(),&rt);
		}
		/*
		_WKIHOTKEYS_EventsHandlerDuplicateWindow fp2=(_WKIHOTKEYS_EventsHandlerDuplicateWindow)GetProcAddress(hLLHookInst,"WKIHOTKEYS_EventsHandlerDuplicateWindow");
		if(fp2){
			(*fp2)()=this->GetSafeHwnd();
		}
		*/
		//FreeLibrary(hLLHookInst);
	}
	WKIHOTKEYS_EventsHandlerDuplicateWindow()=this->GetSafeHwnd();
	return 0;
}

CInPlaceHotKey::~CInPlaceHotKey()
{
	HINSTANCE hLLHookInst=GetModuleHandle("wkkeyh.dll");
	if(hLLHookInst && IsPromiscEnable()){
		_SetPromiscMode fp=(_SetPromiscMode)GetProcAddress(hLLHookInst,"SetPromiscMode");
		if(fp){
			(*fp)(NULL,NULL);
		}
		/*
		_WKIHOTKEYS_EventsHandlerDuplicateWindow fp2=(_WKIHOTKEYS_EventsHandlerDuplicateWindow)GetProcAddress(hLLHookInst,"WKIHOTKEYS_EventsHandlerDuplicateWindow");
		if(fp2){
			(*fp2)()=0;
		}
		*/
		//FreeLibrary(hLLHookInst);
	}
	WKIHOTKEYS_EventsHandlerDuplicateWindow()=0;
	lHotkeyInputInProgress=NULL;
	if(m_dwHotKeyStartup){
		delete m_dwHotKeyStartup;
		m_dwHotKeyStartup=0;
		m_dwHotKey=0;
	}
}

void CInPlaceHotKey::OnNcDestroy()
{
	//ShowCaret();
	CStatic::OnNcDestroy();
	delete this;
}

void CInPlaceHotKey::RevertStartupValue(CIHotkey dwNewKey)
{
	if(dwNewKey==CIHotkey(0)){
		m_dwHotKey=m_dwHotKeyStartup;
	}else{
		*m_dwHotKeyStartup=dwNewKey;
	}
	SetWindowText(GetIHotkeyName(*m_dwHotKeyStartup));
	Invalidate();
}

BOOL CInPlaceHotKey::isStopHotkey(CIHotkey* pHotkey)
{
	CIHotkey oHotkey;
	if(pHotkey){
		oHotkey=*pHotkey;
	}else{
		oHotkey=getCurrentlyPressedKeys(FALSE);
	}
	oHotkey.DropRawKeyInHotkey(VK_2X);
	oHotkey.DropRawKeyInHotkey(VK_3X);
	BOOL bRes=(oHotkey==CIHotkey(VK_DELETE,0)?1:0);
	if(oHotkey==CIHotkey(VK_BACK,0)){
		bRes=2;
	}
	return bRes;
}

BOOL CInPlaceHotKey::AbortProcess()
{
	//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,0,0);
	if(m_dwHotKeyStartup && m_dwHotKey){
		//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,0,0);
		m_dwHotKey->Reset();
	};
	CString sHKName="";
	//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,0,0);
	PROC_HKNAME(sHKName);
	//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,0,0);
	SetWindowText(sHKName);
	//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,0,0);
	Invalidate();
	//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,0,0);
	return TRUE;
}

afx_msg void CInPlaceHotKey::ActualizeHotkey(BOOL bFromScratch, BOOL bStopKeysOnly, UINT iAdded)
{
	if((m_dwHotKey->Options()&ADDOPT_EVENT)!=0){
		m_dwHotKey->Reset();
	}
	CIHotkey oHotkey=getCurrentlyPressedKeys(FALSE);
	BOOL bStopKey=isStopHotkey(&oHotkey);
	if(oHotkey==CIHotkey(VK_ESCAPE,0)){
		bStopKey=1;
	}
	if(bStopKey==2){
		m_dwHotKey->RemoveOneKey();
		PostProcessHotkey(m_dwHotKey);
		CString sHKName=GetIHotkeyName(*m_dwHotKey);
		PROC_HKNAME(sHKName);
		SetWindowText(sHKName);
		Invalidate();
		if(!m_dwHotKey->isSeq() && !m_dwHotKey->Present()){
			(pRParent?pRParent:GetParent())->SetFocus();
		}
	}else if(bStopKey==1){
		if(HotkeysSkipDD()){
			m_dwHotKey->Reset();
			CString sHKName=GetIHotkeyName(*m_dwHotKey);
			PROC_HKNAME(sHKName);
			SetWindowText(sHKName);
			Invalidate();
		}else{
			(pRParent?pRParent:GetParent())->SetFocus();
		}
	}else if(!bStopKey && !bStopKeysOnly){
		if(m_dwHotKey->isSeq()){
			if(iAdded){
				m_dwHotKey->AddOneKey(iAdded);
				PostProcessHotkey(m_dwHotKey);
				CString sHKName=GetIHotkeyName(*m_dwHotKey,FALSE);
				PROC_HKNAME(sHKName);
				SetWindowText(sHKName);
			}
		}else{
			// Сбрасываем гор. клавиши при смене тенденции
			if(oHotkey.GetModifiers()!=m_dwHotKey->GetModifiers() || oHotkey.GetBaseKeys()!=m_dwHotKey->GetBaseKeys() || !oHotkey.GetModifiers().Present() || !oHotkey.GetBaseKeys().Present()){
				m_dwHotKey->CopyHotkey(oHotkey);
				PostProcessHotkey(m_dwHotKey);
				CString sHKName=GetIHotkeyName(*m_dwHotKey,FALSE);
				PROC_HKNAME(sHKName);
				SetWindowText(sHKName);
			}
		}
		Invalidate();
	}
}

DWORD WINAPI UnPressHK(LPVOID pd)
{
	//FLOG3A("DWORD WINAPI UnPressHK:%i %x/%x",__LINE__,pd,lHotkeyInputInProgress);
	if(lHotkeyInputInProgress){
		//FLOG3A("DWORD WINAPI UnPressHK:%i %x/%x",__LINE__,pd,lHotkeyInputInProgress);
		lHotkeyInputInProgress->Invalidate();
	}
	//Sleep(500);
	//FLOG3A("DWORD WINAPI UnPressHK:%i %x/%x",__LINE__,pd,lHotkeyInputInProgress);
	if(lHotkeyInputInProgress){
		//FLOG3A("DWORD WINAPI UnPressHK:%i %x/%x",__LINE__,pd,lHotkeyInputInProgress);
		lHotkeyInputInProgress->bMenuBt=0;
		lHotkeyInputInProgress->bCloseBt=0;
		lHotkeyInputInProgress->Invalidate();
		//FLOG3A("DWORD WINAPI UnPressHK:%i %x/%x",__LINE__,pd,lHotkeyInputInProgress);
	}
	return 0;
}

BOOL AddEventsByClass(CExternEventClass* pEventClass, CMenu& menu, BOOL bRoots)
{
	CString sClass=pEventClass->szEventClassName;
	if(((bRoots==0)?1:0) != ((sClass.Find("@ROOT")!=-1)?0:1)){
		return 0;
	}
	CMenu menuE;
	menuE.CreatePopupMenu();
	CExternEvent* pEvent=WKIHOTKEYS_ExternGetEvent(0);
	int iCount=0;
	CDWordArray aCodes;
	CStringArray aNames;
	while(pEvent){
		if(pEvent->pEventClass==pEventClass){
			aCodes.Add(WM_USER-iCount);
			TRACE(pEvent->szEventName);
			aNames.Add(pEvent->szEventName);
		}
		pEvent=pEvent->pNext;
		iCount++;
	}
	int iLen=aCodes.GetSize();
	// Ищем делимитер...
	int iComfortColumn=iLen;
	if(iComfortColumn>60){
		iComfortColumn=iLen/3;
	}else if(iComfortColumn>30){
		iComfortColumn=iLen/2;
	}
	for(int i=iLen-1;i>=0;i--){
		BOOL bBreak=FALSE;
		if(i<iLen-1 && i>1){
			bBreak=(i%iComfortColumn)==0;
		}
		if(sClass.Find("@ROOT")==-1){
			menuE.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), aCodes[i], aNames[i]);
		}else{
			menu.AppendMenu(MF_STRING, aCodes[i], aNames[i]);
		}
	}
	if(sClass.Find("@ROOT")==-1){
		sClass=sClass.SpanExcluding("\t");
		menu.AppendMenu(MF_STRING|MF_POPUP, UINT(menuE.GetSafeHmenu()), sClass);
	}
	return TRUE;
}

LRESULT CInPlaceHotKey::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_LBUTTONDOWN || message==WM_RBUTTONDOWN){
		//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,message,wParam);
		if(!HotkeysSkipDD()){
			CPoint ptOrig;
			::GetCursorPos(&ptOrig);
			CPoint pt=ptOrig;
			ScreenToClient(&pt);
			if(rcHKNBt.PtInRect(pt)){
				PostMessage(WM_COMMAND,WM_USER+'A',0);
				return 1;
			}
			if(rcHKNBt1.PtInRect(pt)){
				PostMessage(WM_COMMAND,WM_USER+'A'+1,0);
				return 1;
			}
			if(rcHKNBt2.PtInRect(pt)){
				PostMessage(WM_COMMAND,WM_USER+'A'+2,0);
				return 1;
			}
			if(rcHKNBt3.PtInRect(pt)){
				PostMessage(WM_COMMAND,WM_USER+'A'+3,0);
				return 1;
			}
			if(rcCloseBt.PtInRect(pt)){
				//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,message,wParam);
				bCloseBt=1;
				UnPressHK(0);
				AbortProcess();
				//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,message,wParam);
				//FORK(UnPressHK,0);
				//FLOG3A("CInPlaceHotKey::WindowProc:%i %x/%x",__LINE__,message,wParam);
				return 1;
			}
			if(rcMenuBt.PtInRect(pt) || message==WM_RBUTTONDOWN){
				SimpleLocker lc(&csHotkeyTrackPopup);
				if(message!=WM_RBUTTONDOWN){
					bMenuBt=1;
					Invalidate();
				}
				// Рисуем менюшку
				CMenu menu;
				menu.CreatePopupMenu();
				{// Keys
					CMenu menu2;
					menu2.CreatePopupMenu();
					for(int i=VK_SPACE; i<=VK_SCROLL;i++){
						if(i<=VK_HELP || i>=VK_APPS){
							CString sName=GetIHotkeyName(CIHotkey(i));
							sName.TrimLeft();
							if(sName!=""){
								BOOL bBreak=((i==0x60)||(i==0x70));//((menu2.GetMenuItemCount()-1)%14)==13;
								if(i==0x5d){//sName=="Apps"
									menu2.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+VK_ESCAPE, "Esc");
									menu2.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+VK_CAPITAL, "Caps Lock");
									menu2.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+VK_PAUSE, "Pause");
									bBreak=0;
								}
								if(i==0x6f){
									menu2.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+VK_BACK, "Backspace");
									bBreak=0;
								}
								if(i==0x90){
									menu2.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+VK_TAB, "Tab");
									menu2.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+VK_RETURN, "Enter");
									bBreak=0;
								}
#ifdef _DEBUG
								CString sID;
								sID.Format(" #%02x",i);
								sName+=sID;
#endif
								menu2.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+i, sName);
							}
						}
					}
					menu.AppendMenu(MF_STRING|MF_POPUP|(m_dwHotKey->isEvent()?/*MF_GRAYED*/0:0), UINT(menu2.GetSafeHmenu()), _l("Regular keys",0));
				}
				{// Modifiers
					CMenu menu3;
					menu3.CreatePopupMenu();
					
					menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_SHIFT, GetIHotkeyName(CIHotkey(0,HOTKEYF_SHIFT)));
					if(m_bPromiscON){
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_RSHIFT, GetIHotkeyName(CIHotkey(0,HOTKEYF_RSHIFT)));
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_LSHIFT, GetIHotkeyName(CIHotkey(0,HOTKEYF_LSHIFT)));
						menu3.AppendMenu(MF_SEPARATOR, 0, "");
					}
					
					menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_CONTROL, GetIHotkeyName(CIHotkey(0,HOTKEYF_CONTROL)));
					if(m_bPromiscON){
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_RCONTROL, GetIHotkeyName(CIHotkey(0,HOTKEYF_RCONTROL)));
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_LCONTROL, GetIHotkeyName(CIHotkey(0,HOTKEYF_LCONTROL)));
						menu3.AppendMenu(MF_SEPARATOR, 0, "");
					}
					
					menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_ALT, GetIHotkeyName(CIHotkey(0,HOTKEYF_ALT)));
					if(m_bPromiscON){
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_RALT, GetIHotkeyName(CIHotkey(0,HOTKEYF_RALT)));
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_LALT, GetIHotkeyName(CIHotkey(0,HOTKEYF_LALT)));
						menu3.AppendMenu(MF_SEPARATOR, 0, "");
					}
					
					menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_EXT, GetIHotkeyName(CIHotkey(0,HOTKEYF_EXT)));
					if(m_bPromiscON){
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_REXT, GetIHotkeyName(CIHotkey(0,HOTKEYF_REXT)));
						menu3.AppendMenu(MF_STRING, WM_USER+KBBUFFER_DSIZE+HOTKEYF_LEXT, GetIHotkeyName(CIHotkey(0,HOTKEYF_LEXT)));
						menu3.AppendMenu(MF_SEPARATOR, 0, "");
					}
					if(m_bPromiscON){
						menu3.AppendMenu(MF_STRING, WM_USER+VK_2X, GetIHotkeyName(CIHotkey(VK_2X)));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_3X, GetIHotkeyName(CIHotkey(VK_3X)));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_PRESSNHOLD, GetIHotkeyName(CIHotkey(VK_PRESSNHOLD)));
						menu3.AppendMenu(MF_SEPARATOR, 0, "");
						menu3.AppendMenu(MF_STRING|MF_GRAYED, 0, _l("Window parts modifiers",0));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_MOB_ICO, GetIHotkeyName(CIHotkey(VK_MOB_ICO)));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_MOB_MAX, GetIHotkeyName(CIHotkey(VK_MOB_MAX)));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_MOB_MIN, GetIHotkeyName(CIHotkey(VK_MOB_MIN)));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_MOB_HLP, GetIHotkeyName(CIHotkey(VK_MOB_HLP)));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_MOB_CLOSE, GetIHotkeyName(CIHotkey(VK_MOB_CLOSE)));
						menu3.AppendMenu(MF_STRING, WM_USER+VK_MO_TITLE, GetIHotkeyName(CIHotkey(VK_MO_TITLE)));
					}
					menu.AppendMenu(MF_STRING|MF_POPUP|(m_dwHotKey->isEvent()?/*MF_GRAYED*/0:0), UINT(menu3.GetSafeHmenu()), _l("Add modifier",0));
				}
				if(m_bPromiscON){// Media Keys
					CMenu menu4;
					menu4.CreatePopupMenu();
					for(int i=0xa6; i<=0xb5;i++){
						CString sName=GetIHotkeyName(CIHotkey(i));
						if(sName!=""){
							BOOL bBreak=(menu4.GetMenuItemCount()%9)==8;
							menu4.AppendMenu(MF_STRING|(bBreak?MF_MENUBARBREAK:0), WM_USER+i, sName);
						}
					}
					menu.AppendMenu(MF_STRING|MF_POPUP|(m_dwHotKey->isEvent()?/*MF_GRAYED*/0:0), UINT(menu4.GetSafeHmenu()), _l("Add media key",0));
				}
				if(m_bPromiscON){// Mouse Keys
					CMenu menu5;
					menu5.CreatePopupMenu();
					menu5.AppendMenu(MF_STRING, WM_USER+VK_LBUTTON, GetIHotkeyName(CIHotkey(VK_LBUTTON)));
					menu5.AppendMenu(MF_STRING, WM_USER+VK_RBUTTON, GetIHotkeyName(CIHotkey(VK_RBUTTON)));
					menu5.AppendMenu(MF_STRING, WM_USER+VK_MBUTTON, GetIHotkeyName(CIHotkey(VK_MBUTTON)));
					menu5.AppendMenu(MF_STRING, WM_USER+VK_LBT_DBL, GetIHotkeyName(CIHotkey(VK_LBT_DBL)));
					menu5.AppendMenu(MF_STRING, WM_USER+VK_MBT_DBL, GetIHotkeyName(CIHotkey(VK_MBT_DBL)));
					menu5.AppendMenu(MF_STRING, WM_USER+VK_RBT_DBL, GetIHotkeyName(CIHotkey(VK_RBT_DBL)));
					int i;
					for(i=VK_XBTTNWK4; i<=WHEEL_UP;i++){
						menu5.AppendMenu(MF_STRING, WM_USER+i, GetIHotkeyName(CIHotkey(i)));
						if(i==VK_LBT_DBL || i==VK_XBTTNWK1){
							menu5.AppendMenu(MF_SEPARATOR, 0, "");
						}
					}
					//menu5.AppendMenu(MF_STRING, WM_USER+VK_MOB_MLES, GetIHotkeyName(CIHotkey(VK_MOB_MLES)));
					menu5.AppendMenu(MF_SEPARATOR, 0, "");
					menu5.AppendMenu(MF_STRING|MF_GRAYED, 0, _l("Screen borders",0));
					for(i=VK_MBORDER_L; i<=VK_MCORNER_RT;i++){
						if(i!=VK_EVENTKEY){
							menu5.AppendMenu(MF_STRING, WM_USER+i, GetIHotkeyName(CIHotkey(i)));
						}
					}
					menu.AppendMenu(MF_STRING|MF_POPUP|(m_dwHotKey->isEvent()?/*MF_GRAYED*/0:0), UINT(menu5.GetSafeHmenu()), _l("Add mouse key",0));
				}
				{// Extern events;
					CExternEventClass* pEventClass=0;
					pEventClass=WKIHOTKEYS_ExternGetClass(0);
					int iCount=0;
					while(pEventClass){
						AddEventsByClass(pEventClass,menu,0);
						pEventClass=pEventClass->pNext;
					}
					// Снова...
					pEventClass=WKIHOTKEYS_ExternGetClass(0);
					while(pEventClass){
						AddEventsByClass(pEventClass,menu,1);
						pEventClass=pEventClass->pNext;
					}
				}
				if(m_bPromiscON){
					CMenu menu7;
					menu7.CreatePopupMenu();
					menu7.AppendMenu(MF_STRING|((!m_dwHotKey->isEvent() && !m_dwHotKey->isSeq())?MF_CHECKED:0), WM_USER+'A'+7, _l("Normal",0));
					menu7.AppendMenu(MF_STRING|((!m_dwHotKey->isEvent() && m_dwHotKey->isSeq())?MF_CHECKED:0), WM_USER+'A'+7, _l("Sequence",0));
					if(m_dwHotKey->isEvent()){
						menu7.AppendMenu(MF_STRING|MF_CHECKED, 0, _l("Extern events",0));
					}
					menu7.AppendMenu(MF_SEPARATOR, 0, "");
					menu7.AppendMenu(MF_STRING|(((m_dwHotKey->Options()&ADDOPT_TRANSPARENT)!=0)?MF_CHECKED:0)|((m_dwHotKey->isSeq()||m_dwHotKey->isEvent())?MF_GRAYED:0), WM_USER+'A'+5, _l("Option",0)+": "+_l("Pass-through",0));
					menu7.AppendMenu(MF_STRING|(((m_dwHotKey->Options()&ADDOPT_ON_UNPRESS)!=0)?MF_CHECKED:0)|(m_dwHotKey->isSeq()||m_dwHotKey->isEvent()?MF_GRAYED:0), WM_USER+'A'+4, _l("Option",0)+": "+_l("Act on unpress",0));
					menu7.AppendMenu(MF_STRING|(((m_dwHotKey->Options()&ADDOPT_ON_WPRESSED)!=0)?MF_CHECKED:0)|(m_dwHotKey->isSeq()||m_dwHotKey->isEvent()?MF_GRAYED:0), WM_USER+'A'+8, _l("Option",0)+": "+_l("Press + unpress",0));
					menu7.AppendMenu(MF_SEPARATOR, 0, "");
					menu7.AppendMenu(MF_STRING, WM_USER+'A'+6, _l("What is that?",0));
					menu.AppendMenu(MF_SEPARATOR, 0, "");
					menu.AppendMenu(MF_STRING|MF_POPUP, UINT(menu7.GetSafeHmenu()), _l("Hotkey behaviour",0));
				}
				menu.AppendMenu(MF_SEPARATOR, 0, "");
				menu.AppendMenu(MF_STRING|((bHKNBt==0)?MF_CHECKED:0), WM_USER+'A', _l("Primary hotkey",0));
				menu.AppendMenu(MF_STRING|((bHKNBt==1)?MF_CHECKED:0), WM_USER+'A'+1, _l("Secondary hotkey",0));
				menu.AppendMenu(MF_STRING|((bHKNBt==2)?MF_CHECKED:0), WM_USER+'A'+2, _l("Third hotkey",0));
				menu.AppendMenu(MF_SEPARATOR, 0, "");
				menu.AppendMenu(MF_STRING, WM_USER+VK_EVENTKEY, _l("Clear hotkey",0));
				menu.AppendMenu(MF_STRING, WM_USER+'A'+3, _l("Save hotkey",0));
				menu.AppendMenu(MF_STRING, WM_USER+'A'+6, _l("Help",0));
				HWND hWinThis=this->GetSafeHwnd();
				DWORD dwRetCode=TrackPopupMenu(menu.GetSafeHmenu(), TPM_LEFTBUTTON, ptOrig.x, ptOrig.y, 0, hWinThis, NULL);//|TPM_RETURNCMD
				if(lHotkeyInputInProgress==0 || !IsWindow(hWinThis)){
					return 0;
				}
				if(message!=WM_RBUTTONDOWN){
					bMenuBt=0;
					Invalidate();
				}
				return 0;
			}
		}
	}
	if(message==WM_HOTKEY){
		if(lParam!=0){
			if(wParam==VK_EVENTKEY){
				AddEventToHotkey((char*)lParam,m_dwHotKey);
				PostProcessHotkey(m_dwHotKey);
				CString sHKName=GetIHotkeyName(*m_dwHotKey);
				PROC_HKNAME(sHKName);
				SetWindowText(sHKName);
				Invalidate();
			}else{
				ActualizeHotkey(1,0,wParam);
			}
		}else{
			ActualizeHotkey(TRUE,TRUE,wParam);
		}
		return 0;
	}
	if(message==WM_KEYDOWN || message==WM_SYSKEYDOWN){
		ActualizeHotkey(1,0,0);
		return 0;
	}
	if(message>=WM_KEYFIRST && message<=WM_KEYLAST){
		return 0;
	}
	return CStatic::WindowProc(message, wParam, lParam);
}

void ShowHelp(const char* szTopicName);
BOOL CInPlaceHotKey::AddEventToHotkey(char const* szEvent, CIHotkey*& hk)
{
	CString sConflictingEvent=hk->IsEventOfProhibitedClassInHotkey(szEvent);
	if(sConflictingEvent!=""){
		return 0;
	}
	int iRes=0;
	HINSTANCE hLLHookInst=IsPluginOn("WP_KeyMaster");
	_WKIHOTKEYS_NotifyOnAddEvent fp2=(_WKIHOTKEYS_NotifyOnAddEvent)GetProcAddress(hLLHookInst,"WKIHOTKEYS_NotifyOnAddEvent");
	if(fp2){
		iRes=(*fp2)(szEvent);
	}
	if(iRes==0){
		// Никто не блокнул
		hk->AddOneEvent(szEvent);
		return 1;
	}
	return 0;
}

typedef void (WINAPI *_GetDefaultPauseKey)(void*);
typedef BOOL (WINAPI *_PostProcessHotkey)(void*);
BOOL CInPlaceHotKey::PreTranslateMessage(MSG* pMsg)
{
	static int c=0;
	TRACE3("PreTranslateMessage - %i, %x, l=%i!\n",++c, pMsg->message,pMsg->lParam);
	if(pMsg->message==WM_COMMAND){
		int iAddCode=pMsg->wParam-WM_USER;
		if(iAddCode<=0){
			CExternEvent* pEventStart=WKIHOTKEYS_ExternGetEvent(-iAddCode);
			if(pEventStart){
				if(AddEventToHotkey(pEventStart->szEventID,m_dwHotKey)==0){
					return 1;
				}
			}
		}else if(iAddCode>KBBUFFER_DSIZE){
			iAddCode-=KBBUFFER_DSIZE;
			m_dwHotKey->AddOneModifier(iAddCode);
		}else if(iAddCode!=VK_EVENTKEY){
			if(iAddCode=='A'+4){
				if(m_dwHotKey->Options()&ADDOPT_ON_UNPRESS){
					m_dwHotKey->Options()&=~ADDOPT_ON_UNPRESS;
				}else{
					m_dwHotKey->Options()|=ADDOPT_ON_UNPRESS;
					m_dwHotKey->Options()&=~ADDOPT_SEQUENCE;
				}
			}else if(iAddCode=='A'+5){
				if(m_dwHotKey->Options()&ADDOPT_TRANSPARENT){
					m_dwHotKey->Options()&=~ADDOPT_TRANSPARENT;
				}else{
					m_dwHotKey->Options()|=ADDOPT_TRANSPARENT;
				}
			}else if(iAddCode=='A'+6){
				ShowHelp("hotkeys");
			}else if(iAddCode=='A'+7){
				if(m_dwHotKey->isSeq()){
					m_dwHotKey->Reset();
					//m_dwHotKey->Options()&=~ADDOPT_SEQUENCE;
				}else{
					m_dwHotKey->Reset();
					m_dwHotKey->Options()|=ADDOPT_SEQUENCE;
					m_dwHotKey->Options()|=ADDOPT_TRANSPARENT;
					//m_dwHotKey->Options()&=~ADDOPT_ON_UNPRESS;
					//Значение паузы по-умолчанию
					HINSTANCE hLLHookInst=IsPluginOn("WP_KeyMaster");
					_GetDefaultPauseKey fp2=(_GetDefaultPauseKey)GetProcAddress(hLLHookInst,"GetDefaultPauseKey");
					if(fp2){
						(*fp2)(m_dwHotKey);
					}
				}
			}else if(iAddCode=='A'+8){
				if(m_dwHotKey->Options()&ADDOPT_ON_WPRESSED){
					m_dwHotKey->Options()&=~ADDOPT_ON_WPRESSED;
				}else{
					m_dwHotKey->Options()|=ADDOPT_ON_WPRESSED;
					m_dwHotKey->Options()&=~ADDOPT_ON_UNPRESS;
					m_dwHotKey->Options()&=~ADDOPT_SEQUENCE;
				}
			}else if(iAddCode=='A'+3){
				GetParent()->SetFocus();
				return 1;
			}else if(iAddCode<'A' || iAddCode>'A'+7){
				m_dwHotKey->AddOneKey(iAddCode);
			}else{
				bHKNBt=iAddCode-'A';
				if(bHKNBt>0 && !m_dwHotKeyStartup->pNextHK){
					m_dwHotKeyStartup->pNextHK=new CIHotkey();
				}
				if(bHKNBt>1 && !m_dwHotKeyStartup->pNextHK->pNextHK){
					m_dwHotKeyStartup->pNextHK->pNextHK=new CIHotkey();
				}
				if(bHKNBt>2 && !m_dwHotKeyStartup->pNextHK->pNextHK->pNextHK){
					m_dwHotKeyStartup->pNextHK->pNextHK->pNextHK=new CIHotkey();
				}
				if(bHKNBt==0){
					m_dwHotKey=m_dwHotKeyStartup;
				}
				if(bHKNBt==1){
					m_dwHotKey=m_dwHotKeyStartup->pNextHK;
				}
				if(bHKNBt==2){
					m_dwHotKey=m_dwHotKeyStartup->pNextHK->pNextHK;
				}
				if(bHKNBt==3){
					m_dwHotKey=m_dwHotKeyStartup->pNextHK->pNextHK->pNextHK;
				}
			}
		}
		if(iAddCode==VK_EVENTKEY){
			AbortProcess();
		}else{
			PostProcessHotkey(m_dwHotKey);
			CString sHKName=GetIHotkeyName(*m_dwHotKey);
			PROC_HKNAME(sHKName);
			SetWindowText(sHKName);
			Invalidate();
		}
		return 1;
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void CInPlaceHotKey::PostProcessHotkey(CIHotkey* m_dwHotKey)
{
	HINSTANCE hLLHookInst=IsPluginOn("WP_KeyMaster");
	_PostProcessHotkey fp2=(_PostProcessHotkey)GetProcAddress(hLLHookInst,"PostProcessHotkey");
	if(fp2){
		(*fp2)(m_dwHotKey);
	}
}

#define SMB_WIDTH	0.7
void CInPlaceHotKey::OnPaint()
{
	CString str;
	GetWindowText(str);
	static CString sText="";
	CDC *pDC = GetDC();
	CRect rcItem;
	GetClientRect(&rcItem);
	CRect rcFill=rcItem;
	CRect rcItemOrig=rcItem;
	CBrush br(m_crBackground);
	pDC->FillRect(rcFill, &br);
	//draw text
	pDC->SetBkColor (m_crBackground);
	pDC->SetTextColor (m_crText);
	CFont *pFont=(CFont*)pDC->SelectObject(&m_font);
	if(!HotkeysSkipDD()){
		rcItem.right-= HK_BUTTONS*rcItem.Height()+2;
	}
		pDC->DrawText(str, -1, rcItem, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
	if(!HotkeysSkipDD()){
		rcItem.right+= HK_BUTTONS*rcItem.Height()+2;
	}
	if(!HotkeysSkipDD()){
		CFont *pFontTmp=0;
		LOGFONT logf;
		CFont fBoldFont;
		pFont->GetLogFont(&logf);
		//logf.lfWeight=800;
		logf.lfUnderline=1;
		fBoldFont.CreateFontIndirect(&logf);
		//button
		rcMenuBt.left=rcMenuBt.right=rcMenuBt.top=rcMenuBt.bottom=0;
		rcCloseBt.left=rcCloseBt.right=rcCloseBt.top=rcCloseBt.bottom=0;
		rcHKNBt.left=rcHKNBt.right=rcHKNBt.top=rcHKNBt.bottom=0;
		rcHKNBt1.left=rcHKNBt1.right=rcHKNBt1.top=rcHKNBt1.bottom=0;
		rcHKNBt2.left=rcHKNBt2.right=rcHKNBt2.top=rcHKNBt2.bottom=0;
		rcHKNBt3.left=rcHKNBt3.right=rcHKNBt3.top=rcHKNBt3.bottom=0;
		{
			// X button
			rcCloseBt=rcItem;
			rcCloseBt.left = rcCloseBt.right-rcCloseBt.Height();
			pDC->DrawFrameControl(rcCloseBt, DFC_CAPTION, DFCS_CAPTIONCLOSE|(bCloseBt?DFCS_PUSHED:0));
			// V button
			rcMenuBt=rcItem;
			rcMenuBt.left = rcMenuBt.right-2*rcMenuBt.Height()-1;
			rcMenuBt.right = rcMenuBt.left+rcMenuBt.Height();
			pDC->DrawFrameControl(rcMenuBt, DFC_SCROLL, DFCS_SCROLLDOWN|(bMenuBt?DFCS_PUSHED:0));
			::SetBkMode(*pDC, TRANSPARENT);
			/*
			// 4 button
			rcHKNBt3=rcItem;
			rcHKNBt3.left = rcHKNBt3.right-2*rcHKNBt3.Height()-1*rcHKNBt3.Height()/2-1;
			rcHKNBt3.right = rcHKNBt3.left+rcHKNBt3.Height()/2;
			pDC->DrawFrameControl(rcHKNBt3, DFC_BUTTON, DFCS_BUTTONPUSH|((bHKNBt==3)?DFCS_PUSHED:DFCS_FLAT));
			pDC->DrawText("4", -1, rcHKNBt3, DT_SINGLELINE | DT_NOPREFIX | DT_CENTER | DT_VCENTER | DT_EXTERNALLEADING);
			*/
			// 3 button
			if(m_dwHotKeyStartup && m_dwHotKeyStartup->pNextHK && m_dwHotKeyStartup->pNextHK->pNextHK && m_dwHotKeyStartup->pNextHK->pNextHK->Present()){
				pFontTmp=(CFont*)pDC->SelectObject(&fBoldFont);
			}
			rcHKNBt2=rcItem;
			rcHKNBt2.left = long(rcHKNBt2.right-2*rcHKNBt2.Height()-1*rcHKNBt2.Height()*SMB_WIDTH);
			rcHKNBt2.right = long(rcHKNBt2.left+rcHKNBt2.Height()*SMB_WIDTH);
			pDC->DrawFrameControl(rcHKNBt2, DFC_BUTTON, DFCS_BUTTONPUSH|((bHKNBt==2)?DFCS_PUSHED:DFCS_FLAT));
			pDC->DrawText("3", -1, rcHKNBt2, DT_SINGLELINE | DT_NOPREFIX | DT_CENTER | DT_VCENTER | DT_EXTERNALLEADING);
			if(m_dwHotKeyStartup && m_dwHotKeyStartup->pNextHK && m_dwHotKeyStartup->pNextHK->pNextHK && m_dwHotKeyStartup->pNextHK->pNextHK->Present()){
				pDC->SelectObject(pFontTmp);
			}
			// 2 button
			if(m_dwHotKeyStartup && m_dwHotKeyStartup->pNextHK && m_dwHotKeyStartup->pNextHK->Present()){
				pFontTmp=(CFont*)pDC->SelectObject(&fBoldFont);
			}
			rcHKNBt1=rcItem;
			rcHKNBt1.left = long(rcHKNBt1.right-2*rcHKNBt1.Height()-2*rcHKNBt1.Height()*SMB_WIDTH);
			rcHKNBt1.right = long(rcHKNBt1.left+rcHKNBt1.Height()*SMB_WIDTH);
			pDC->DrawFrameControl(rcHKNBt1, DFC_BUTTON, DFCS_BUTTONPUSH|((bHKNBt==1)?DFCS_PUSHED:DFCS_FLAT));
			pDC->DrawText("2", -1, rcHKNBt1, DT_SINGLELINE | DT_NOPREFIX | DT_CENTER | DT_VCENTER | DT_EXTERNALLEADING);
			if(m_dwHotKeyStartup && m_dwHotKeyStartup->pNextHK && m_dwHotKeyStartup->pNextHK->Present()){
				pDC->SelectObject(pFontTmp);
			}
			// 1 button
			if(m_dwHotKeyStartup && m_dwHotKeyStartup->Present()){
				pFontTmp=(CFont*)pDC->SelectObject(&fBoldFont);
			}
			rcHKNBt=rcItem;
			rcHKNBt.left = long(rcHKNBt.right-2*rcHKNBt.Height()-3*rcHKNBt.Height()*SMB_WIDTH);
			rcHKNBt.right = long(rcHKNBt.left+rcHKNBt.Height()*SMB_WIDTH);
			pDC->DrawFrameControl(rcHKNBt, DFC_BUTTON, DFCS_BUTTONPUSH|((bHKNBt==0)?DFCS_PUSHED:DFCS_FLAT));
			pDC->DrawText("1", -1, rcHKNBt, DT_SINGLELINE | DT_NOPREFIX | DT_CENTER | DT_VCENTER | DT_EXTERNALLEADING);
			if(m_dwHotKeyStartup && m_dwHotKeyStartup->Present()){
				pDC->SelectObject(pFontTmp);
			}
		}
	}
	pDC->SelectObject(pFont);
	sText=str;
	ReleaseDC(pDC);
	ValidateRect(&rcItemOrig);
}

void CInPlaceHotKey::OnKillFocus(CWnd* pNewWnd)
{
	if(!IsWindow(this->GetSafeHwnd())){
		return;
	}
	SimpleLocker lc(&csHotkeyTrackPopup);
	CWnd* pParent=GetParent();
	if(pParent && !IsWindow(pParent->GetSafeHwnd())){
		return;
	}
	if(isStopHotkey(m_dwHotKeyStartup)==1 || (m_dwHotKeyStartup->pNextHK && isStopHotkey(m_dwHotKeyStartup->pNextHK)==1)){
		m_dwHotKeyStartup->Reset();
	}
	//Options()|=ADDOPT_TRANSPARENT
	if(m_dwHotKeyStartup->Options()==0 && ((*m_dwHotKeyStartup)==CIHotkey(VK_LBUTTON) || (*m_dwHotKeyStartup)==CIHotkey(VK_RBUTTON))){
		m_dwHotKeyStartup->Reset();
	}
	{
		// Отсылаем даже если пусто
		FLNM_ROWSTATE flnmRowState;
		m_dwHotKeyStartup->NormalizeHotkeyChain();
		flnmRowState.dwNotifyData=DWORD(new CIHotkey(*m_dwHotKeyStartup));
		flnmRowState.strText=GetIHotkeyName(*m_dwHotKeyStartup,TRUE);
		flnmRowState.hdr.code=FLNM_END_INPLACE_EDIT;
		flnmRowState.hdr.hwndFrom=pParent->GetSafeHwnd();
		flnmRowState.hdr.idFrom=GetDlgCtrlID();
		flnmRowState.bCollapsed=FALSE;
		flnmRowState.hIRow= m_hIRow;
		flnmRowState.iIItem=m_iIItem;
		pParent->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(),  (LPARAM) &flnmRowState);
	}
	DestroyWindow();
}
#endif
//
//
//		CInPlaceDate
//
//
BEGIN_MESSAGE_MAP(CInPlaceDate, CMonthCalCtrl)
	//{{AFX_MSG_MAP(CInPlaceDate)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CInPlaceDate::CInPlaceDate(HIROW hIRow, int iIItem, COleDateTime dt, CWnd* pParent)
{
	m_hIRow=hIRow;
	m_iIItem=iIItem;
	bSended=0;
	m_dt=dt;
	m_pParent=pParent;
	INITCOMMONCONTROLSEX ics;
	memset(&ics,0,sizeof(ics));
	ics.dwSize=sizeof(ics);
	ics.dwICC=ICC_DATE_CLASSES;
	InitCommonControlsEx(&ics);
}

BOOL CInPlaceDate::PreCreateWindow(CREATESTRUCT& cs)
{
	/*
	cs.x=rtPos.left;
	cs.y=rtPos.top;
	cs.cx=rtPos.Width();
	cs.cy=rtPos.Height();
	*/
	return TRUE;
}

int CInPlaceDate::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMonthCalCtrl::OnCreate(lpCreateStruct) == -1){
		return -1;
	}
	SetCurSel(m_dt);
	return 0;
}

BOOL CInPlaceDate::PreTranslateMessage(MSG* pMsg)
{
	BOOL bRes=CMonthCalCtrl::PreTranslateMessage(pMsg);
	if(pMsg->message==WM_KEYUP && pMsg->wParam==VK_RETURN){
		SendUp();
		return TRUE;
	}
	if(pMsg->message==WM_LBUTTONUP){
		if(OnButtonUp()){
			return TRUE;
		}
	}
	return bRes;
}

BOOL CInPlaceDate::OnButtonUp()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	MCHITTESTINFO hi;
	memset(&hi,0,sizeof(hi));
	hi.cbSize=sizeof(hi);
	hi.pt.x=point.x;
	hi.pt.y=point.y;
	DWORD dwRes=HitTest(&hi);
	COleDateTime dtCur=COleDateTime(hi.st);
	if(dwRes==MCHT_CALENDARDATE && dtCur.GetStatus()!=COleDateTime::invalid){
		if(dtCur==m_dt){
			SendUp();
			return TRUE;
		}
		m_dt=dtCur;
		SetCurSel(&hi.st);
		Invalidate();
	}
	return FALSE;
}

void CInPlaceDate::SendUp()
{
	if(bSended){
		return;
	}
	bSended=1;
	// Send Notification to parent of ListView ctrl
	COleDateTime dt;
	//BOOL bRes=GetCurSel(dt);
	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)::SendMessage(GetSafeHwnd(), MCM_GETCURSEL, 0, (LPARAM) &sysTime);
	if(bResult){
		dt=COleDateTime(sysTime.wYear,sysTime.wMonth,sysTime.wDay,0,0,0);
	}
	if(dt.GetStatus()==COleDateTime::valid){
		SetDlgCtrlID(IDC_INPLACE_DATE);
		FLNM_ROWSTATE flnmRowState;
		flnmRowState.hdr.code=FLNM_END_INPLACE_DATE;
		flnmRowState.hdr.hwndFrom=m_pParent->GetSafeHwnd();
		flnmRowState.hdr.idFrom=IDC_INPLACE_DATE;
		flnmRowState.dwNotifyData=MAKELONG(dt.GetYear(),MAKEWORD(dt.GetMonth(),dt.GetDay()));
		flnmRowState.bCollapsed= FALSE;
		flnmRowState.strText=dt.Format("%x");
		flnmRowState.hIRow= m_hIRow;
		flnmRowState.iIItem=m_iIItem;
		DWORD dwPId=m_pParent->GetDlgCtrlID();
		GetParent()->SendMessage(WM_NOTIFY, dwPId,  (LPARAM) &flnmRowState);
	}
	TRACE1("----->%08X\n",GetSafeHwnd());
	ShowWindow(SW_HIDE);
	DestroyWindow();
}

void CInPlaceDate::OnKillFocus(CWnd* pNewWnd)
{
	if(!IsWindow(this->GetSafeHwnd())){
		return;
	}
	CWnd* pParent=GetParent();
	if(pParent && !IsWindow(pParent->GetSafeHwnd())){
		return;
	}
	CMonthCalCtrl::OnKillFocus(pNewWnd);
	TRACE1("-----2>%08X\n",GetSafeHwnd());
	SendUp();
}

void CInPlaceDate::OnNcDestroy()
{
	TRACE1("-----3>%08X\n",GetSafeHwnd());
	CMonthCalCtrl::OnNcDestroy();
	delete this;
}

BOOL& HotkeysSkipDD()
{
	static BOOL b=0;
	return b;
}