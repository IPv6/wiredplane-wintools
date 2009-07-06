// WMDateCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WMDateCtrl.h"
#include "WVCellData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef _TRANM
#define _TRANM(X)  X.Format("%B")
#define _TRANW(X)  X.Format("%A")
#define _TRANWS(X) X.Format("%A")
#pragma message("Defaulting _TRAN")
#endif

#define WEEKVIEWER_CLASSNAME    _T("DBSWMDateCtrl")  // Window class name
BOOL isToday(COleDateTime dt)
{
	COleDateTime dtNow=COleDateTime::GetCurrentTime();
	if(dt.GetYear()==dtNow.GetYear() && dt.GetMonth()==dtNow.GetMonth()	&& dt.GetDay()==dtNow.GetDay()){
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWMDateCtrl
CWMDateCtrl::CWMDateCtrl()
{
	// Default values
	clrBackGrnd = RGB(255,255,213);
	clrOtherMonthBk = RGB(255, 244, 188);
	clrGradBot = RGB(249, 204, 95);	// Gradient color - bottom
	clrGradTop = RGB(236, 233, 216);
	clrGradLine = RGB(187, 85, 3);	// Gradient line color
	nFontSize = 85;
	strFontName = "TAHOMA";
	bMilTime = FALSE;	// NonMilitary Time
	bMonthView = FALSE;	// Do not draw Month view
	nUniqueItemID = 0;	// Unique Item ID Number
	dtMonth.SetDate(COleDateTime::GetCurrentTime().GetYear(), COleDateTime::GetCurrentTime().GetMonth(), 1);
	pImageList = NULL;
	bWeekNumbers=0;
	// Create ALL Cell Objects for Week or Month Views
	CWVCellData *pData = NULL;
	for (int x = 1; x <= WV_TOTALCELLS; x++)
	{
		pData = new CWVCellData();
		pData->SetCellID(x);
		pData->SetMilitaryTime(bMilTime);
		pData->SetBkColor(clrBackGrnd);
		// Add node to Cell List
		m_CellList.AddTail(pData);
	}

	RegisterWindowClass();
}

CWMDateCtrl::~CWMDateCtrl()
{
	POSITION pos = m_CellList.GetHeadPosition();
	CObject *pObj = NULL;
	while (pos != NULL)
	{
		pObj = m_CellList.GetNext(pos);
		if (pObj != NULL)
			delete pObj;
	}
	m_CellList.RemoveAll();
}

BEGIN_MESSAGE_MAP(CWMDateCtrl, CWnd)
	//{{AFX_MSG_MAP(CWMDateCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWMDateCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, WEEKVIEWER_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        //wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.hbrBackground = NULL;
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = WEEKVIEWER_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CWMDateCtrl message handlers

void CWMDateCtrl::OnDestroy() 
{
	CWnd::OnDestroy();

	POSITION pos = m_CellList.GetHeadPosition();
	CObject *pObj = NULL;
	while (pos != NULL)
	{
		pObj = m_CellList.GetNext(pos);
		if (pObj != NULL)
			delete pObj;
	}
	m_CellList.RemoveAll();
}

void CWMDateCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CWnd::PreSubclassWindow();
}

BOOL CWMDateCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	// Get client area to paint
	GetClientRect(&rect);
	// Fill with background color
	pDC->FillSolidRect(&rect, clrBackGrnd);

	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}

void CWMDateCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Clear all cell rects
	CRect rArea;
	CWVCellData *pData = NULL;
	for (int x = 1; x <= WV_TOTALCELLS; x++)
	{
		// Get current CellData
		pData = GetCellByID(x);
		if (pData)
		{
			rArea.SetRectEmpty();
			pData->SetCellRect(&rArea);	// reset cell area
		}
	}

	if (!bMonthView)
	{
		// Draw Week View
		PaintWeekView(&dc);
		// Paint Dates on Cell Headers
		DrawWeekCellHeaders(&dc);
	}
	else
	{
		// Draw Month View
		PaintMonthView(&dc);
		DrawMonthCellHeaders(&dc);
	}
	// Do not call CWnd::OnPaint() for painting messages
}

// Draw Headers within each Date Cell
void CWMDateCtrl::DrawWeekCellHeaders(CPaintDC *pDC)
{
	CRect rect, rHdr, rArea;
	// Calculate height of Cell Header to be the width of the scroll bars
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);
	CBrush brHdrBrush, brOldBrush;
	CFont fFont, *pOldFont;
	CString strBuf;
	CWVCellData *pCellData = NULL;
	COLORREF clrHL = GetSysColor(COLOR_HIGHLIGHT);	// Highlight color
	COLORREF clrFace = GetSysColor(COLOR_BTNFACE);	// Header color
	POSITION posItem = NULL;
	CWVCellDataItem *pCellItem = NULL;
	int nCellHeight = 0;
	COleDateTime dtDate = dtCurrentDate;
	CString strDay;
	strDay.Format("%s, %s",_TRANW(dtDate), dtDate.Format("%x"));

	// Find Monday
	while (dtDate.GetDayOfWeek() != 2)
	{
		dtDate -= 1;
		strDay.Format("%s, %s",_TRANW(dtDate), dtDate.Format("%x"));
	}

	// Initialize values
	fFont.CreatePointFont(nFontSize, strFontName, pDC);
	pOldFont = pDC->SelectObject(&fFont);
	// Remove time from date
	dtCurrentDate.SetDate(dtCurrentDate.GetYear(), dtCurrentDate.GetMonth(),
		dtCurrentDate.GetDay());
	// Make drawing text transparent
	pDC->SetBkMode(TRANSPARENT);

	// Get client area to paint
	GetClientRect(&rect);
	
	nCellHeight = rect.bottom / 3;
	// Build Week View Headers
	for (int x = 1; x <= 7; x++)
	{
		// Draw Header
		pCellData = GetCellByDate(dtDate);
		dtDate += 1;
		pCellData->GetCellRect(&rHdr);
		pDC->FillSolidRect(&rHdr, pCellData->GetBkColor());
		rHdr.left += 1;
		rHdr.top += 1;
		rHdr.bottom = rHdr.top + nHdrHeight;

		if (isToday(pCellData->GetCellDate()))
		{
			// Gradient Fill this header
			rHdr.left -= 1;
			rHdr.right -= 1;
			FillGradientRect(pDC, &rHdr);
			rHdr.right -= 2;
			// Calculate Rectangle - for highlighted cell
			CRect rText;
			rText.CopyRect(&rHdr);
			strBuf.Format("%s, %s %s",_TRANW(pCellData->GetCellDate()),_TRANM(pCellData->GetCellDate()),pCellData->GetCellDate().Format("%d"));
			pDC->DrawText(strBuf, &rText, DT_CALCRECT);
			if (pCellData->GetHighlight())
			{
				// Highlight Header
				rText.left = rHdr.left + (rHdr.right - rText.right)	-3;
				rText.right = rHdr.right+1;
				rText.bottom = rHdr.bottom;
				pDC->FillSolidRect(&rText, clrHL);
				CRect rtMin(rText);
				rtMin.DeflateRect(1,1,1,1);
				pDC->FillSolidRect(&rtMin, clrFace); // Display normal background
				//pDC->SetTextColor(RGB(255,255,255));
			}
		}
		else
		{
			rHdr.right-=1;
			// Header not in Gradient fill
			if (pCellData->GetHighlight())
			{
				// Normal highlight of header
				pDC->FillSolidRect(&rHdr, clrHL);	// Highlight Header
				CRect rtMin(rHdr);
				rtMin.DeflateRect(1,1,1,1);
				pDC->FillSolidRect(&rtMin, clrFace); // Display normal background
				//pDC->SetTextColor(RGB(255,255,255));
			}
			else
			{
				// Display as a regular non-modified header
				CRect rtMin(rHdr);
				//rtMin.top=rtMin.bottom-1;
				/*
				vert [0] .Red    = 0xFF00;
				vert [0] .Green  = 0xFF00;
				vert [0] .Blue   = 0xD500;

				vert [1] .Red    = 0xF900; 
				vert [1] .Green  = 0xCC00;
				vert [1] .Blue   = 0x5F00;
				*/
				pDC->FillSolidRect(&rtMin, RGB(0xF9,0xCC,0x5F));
			}
			rHdr.right-=2;
		}
		// Draw Date within Header
		strBuf.Format("%s, %s %s",_TRANW(pCellData->GetCellDate()),_TRANM(pCellData->GetCellDate()),pCellData->GetCellDate().Format("%d"));
		pDC->DrawText(strBuf, &rHdr, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		pDC->SetTextColor(RGB(0,0,0));
		// DrawItems for this cell
		pCellData->DrawItems(pDC, bMonthView);
	}

	pDC->SelectObject(pOldFont);
}

COLORREF CWMDateCtrl::GetBkColor()
{
	return clrBackGrnd;
}

COLORREF CWMDateCtrl::GetItemColor()
{
	return RGB(0,0,0);
}

CString CWMDateCtrl::GetCellDsk(CWVCellData *pData)
{
	CString sRes;
	POSITION posItem = NULL;
	CWVCellDataItem *pItem = NULL;
	// Update background color for all the CellData Items
	posItem = pData->m_ItemList.GetHeadPosition();
	while (posItem != NULL)
	{
		pItem = (CWVCellDataItem *)pData->m_ItemList.GetNext(posItem);
		CString sTxt=pItem->GetItemText();
		if(sTxt!=""){
			sRes+=sTxt;
			sRes+="\n";
		}
	}
	return sRes;
}

void CWMDateCtrl::SetBkColor(COLORREF clrBk)
{
	clrBackGrnd = clrBk;
	// Update all cells
	POSITION pos = m_CellList.GetHeadPosition();
	POSITION posItem = NULL;
	CWVCellData *pData = NULL;
	CWVCellDataItem *pItem = NULL;
	while (pos != NULL)
	{
		pData = (CWVCellData *)m_CellList.GetNext(pos);
		pData->SetBkColor(clrBk);
		// Update background color for all the CellData Items
		posItem = pData->m_ItemList.GetHeadPosition();
		while (posItem != NULL)
		{
			pItem = (CWVCellDataItem *)pData->m_ItemList.GetNext(posItem);
			pItem->SetBkColor(clrBk);
		}
	}
	InvalidateRect(NULL, TRUE);
}

void CWMDateCtrl::FillGradientRect(CDC *pDC, CRect *pRect)
{
		// Gradient Fill Header
		TRIVERTEX        vert[2] ;
		GRADIENT_RECT    gRect;

		vert [0] .x      = pRect->left;
		vert [0] .y      = pRect->top;
		vert [0] .Alpha  = 0x0000;
		vert [1] .x      = pRect->right;
		vert [1] .y      = pRect->bottom; 
		vert [1] .Alpha  = 0x0000;

		if (!bMonthView)
		{
			// Week View Gradient Colors
			vert [0] .Red    = 0xEC00;
			vert [0] .Green  = 0xE900;
			vert [0] .Blue   = 0xD800;

			vert [1] .Red    = 0xF900; 
			vert [1] .Green  = 0xCC00;
			vert [1] .Blue   = 0x5F00;
		}
		else
		{
			// Month View Gradient Colors
			vert [0] .Red    = 0xFF00;
			vert [0] .Green  = 0xFF00;
			vert [0] .Blue   = 0xD500;

			vert [1] .Red    = 0xF900; 
			vert [1] .Green  = 0xCC00;
			vert [1] .Blue   = 0x5F00;
		}

		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;
		GradientFill(pDC->m_hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

		// Draw Gradient deviding line
		CPen pLine(PS_SOLID, 1, clrGradLine);
		
		pDC->SelectObject(pLine);
		pDC->MoveTo(pRect->left, pRect->bottom);
		pDC->LineTo(pRect->right, pRect->bottom);
		pLine.DeleteObject();
		CPen pPen(PS_SOLID, 1, RGB(0,0,0));
		pDC->SelectObject(pPen);
}

// Get CellData node by ID
CWVCellData * CWMDateCtrl::GetCellByID(int nID)
{
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pData = NULL;

	while (pos != NULL)
	{
		pData = (CWVCellData *)m_CellList.GetNext(pos);
		if (pData->GetCellID() == nID)
			break;
	}
	return pData;
}

// Get CellData by Date value
CWVCellData * CWMDateCtrl::GetCellByDate(COleDateTime dtDate)
{
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pData = NULL;
	CWVCellData *pRes = NULL;
	COleDateTime dtCellDate;

	//Remove time from dtDate
	dtDate.SetDate(dtDate.GetYear(), dtDate.GetMonth(), dtDate.GetDay());

	while (pos != NULL)
	{
		pData = (CWVCellData *)m_CellList.GetNext(pos);
		dtCellDate = pData->GetCellDate();
		if (dtCellDate == dtDate){
			pRes=pData;
			break;
		}
	}
	return pRes;
}

LRESULT CWMDateCtrl::SendMessageToParent(int nRow, int nCol, int nMessage) const
{
    if (!IsWindow(m_hWnd))
        return 0;

    NM_WVCELLDATA nmgv;

	// Fillin CellData Information
	nmgv.pCell = pCurrentCell;
	if (pCurrentCell != NULL)
	{
		if (pCurrentCell->pCellItem != NULL)
		{
			nmgv.nItem = pCurrentCell->pCellItem->GetItemID();
			nmgv.pItem = pCurrentCell->pCellItem;
		}
		else
		{
			nmgv.nItem = 0;
			nmgv.pItem = NULL;
		}
		nmgv.bDNFClicked = pCurrentCell->GetDNFClick();
		// Change message to reflect a double click - ONLY when DNF is TRUE
		if (pCurrentCell->GetDNFClick())
			nMessage = NM_DBLCLK;
	}
	else
	{
		// We do not have an item
		nmgv.pItem = NULL;
		nmgv.nItem = 0;
	}

    nmgv.iRow         = nRow;
    nmgv.iColumn      = nCol;
    nmgv.hdr.hwndFrom = m_hWnd;
    nmgv.hdr.idFrom   = GetDlgCtrlID();
    nmgv.hdr.code     = nMessage;

    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
    else
        return 0;
}
void CWMDateCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// Send Message to parent window
	//SendMessageToParent(point.x, point.y, NM_CLICK);
	CWnd::OnLButtonUp(nFlags, point);
}

CWVCellData* CWMDateCtrl::HitTest(CPoint point) 
{
	// Locate clicked cell
	pCurrentCell = NULL;	// No current cell
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pCellData = NULL;
	while (pos != NULL)
	{
		CRect rect;
		pCellData = (CWVCellData *)m_CellList.GetAt(pos);
		pCellData->GetCellRect(&rect);

		// Mark which item is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Set Highlight Status
			pCurrentCell = pCellData;	// Make cell current
			break;
		}
		m_CellList.GetNext(pos);
	}
	return pCurrentCell;
}

void CWMDateCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	POSITION pos = m_CellList.GetHeadPosition();
	POSITION posItems = NULL;
	CWVCellData *pCellData = NULL;
	CWVCellDataItem *pItem = NULL;
	CRect rect, rDNF;
	CString strBuf;
	CPoint ptPos = point;

#ifdef DEBUG
	strBuf.Format("OnLButtonDown - Clicked point: %d, %d\n", point.x, point.y);
	TRACE(strBuf);
#endif
	// Clear Highlight from any of the CellDataItems
	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		pCellData->SetHighlight(FALSE);
		posItems = pCellData->m_ItemList.GetHeadPosition();
		while (posItems != NULL)
		{
			pItem = (CWVCellDataItem *)pCellData->m_ItemList.GetNext(posItems);
			pItem->SetHightlight(FALSE);
		}
	}

	// Locate clicked cell
	pCurrentCell = NULL;	// No current cell
	pos = m_CellList.GetHeadPosition();
	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetAt(pos);
		pCellData->GetCellRect(&rect);

		// Mark which item is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Set Highlight Status
			pCurrentCell = pCellData;	// Make cell current
			dtLastClicked = pCellData->GetCellDate();
			pCellData->SetHighlight(TRUE);	// Highlight Cell Header

			//Check if we are clicking on the Do Not Fit rectangle area
			pCellData->GetDNFRect(&rDNF);
			if (rDNF.PtInRect(point) == TRUE)
				pCellData->SetDNFClicked(TRUE);	// Set DNF Click status
			else
			{
				pCellData->SetDNFClicked(FALSE);	// Set DNF Click status
				// Check if we are clicking on a CellDataItem
				if (pCellData->HitItemTest(point) == TRUE)
				{
					pCellData->pCellItem->SetHightlight(TRUE);
					SendMessageToParent(point.x, point.y, NM_CLICK);
				}
			}
			break;
		}
		else
		{
			// Remove highlight from current CellData. This is used when the
			// user is clicking inside a cell itself and not a CellDataItem. This will remove
			// the highlight
			pCellData->SetHighlight(FALSE);
		}
		m_CellList.GetNext(pos);
	}
	// Force repaint to update control
	InvalidateRect(NULL, TRUE);

	CWnd::OnLButtonDown(nFlags, point);
}

void CWMDateCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pCellData = NULL;
	CRect rect;
	CString strBuf;

#ifdef DEBUG
	strBuf.Format("OnLButtonDblClk - Clicked point: %d, %d\n", point.x, point.y);
	TRACE(strBuf);
#endif

	pos = m_CellList.GetHeadPosition();
	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		pCellData->GetCellRect(&rect);

		// Mark which cell is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Check if Item is clicked and make CellDataItem internally current
			pCellData->HitItemTest(point);
			dtLastClicked = pCellData->GetCellDate();
			// Send Message to parent window
			SendMessageToParent(point.x, point.y, NM_DBLCLK);
			break;
		}
	}
	// Force repaint to update control
	InvalidateRect(NULL, TRUE);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CWMDateCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	POSITION pos = m_CellList.GetHeadPosition();
	POSITION posItems = NULL;
	CWVCellData *pCellData = NULL;
	CWVCellDataItem *pItem = NULL;
	CRect rect, rDNF;
	CString strBuf;
	CPoint ptPos = point;

#ifdef DEBUG
	strBuf.Format("OnRButtonDown - Clicked point: %d, %d\n", point.x, point.y);
	TRACE(strBuf);
#endif
	// Clear Highlight from any of the CellDataItems
	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		posItems = pCellData->m_ItemList.GetHeadPosition();
		while (posItems != NULL)
		{
			pItem = (CWVCellDataItem *)pCellData->m_ItemList.GetNext(posItems);
			pItem->SetHightlight(FALSE);
		}
	}

	// Locate clicked cell
	pCurrentCell = NULL;	// No current cell
	pos = m_CellList.GetHeadPosition();
	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		pCellData->GetCellRect(&rect);

		// Mark which item is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Set Highlight Status
			pCurrentCell = pCellData;	// Make cell current
			dtLastClicked = pCellData->GetCellDate();
			pCellData->SetHighlight(TRUE);	// Highlight Cell Header

			//Check if we are clicking on the Do Not Fit rectangle area
			pCellData->GetDNFRect(&rDNF);
			if (rDNF.PtInRect(point) == TRUE)
				pCellData->SetDNFClicked(TRUE);	// Set DNF Click status
			else
			{
				pCellData->SetDNFClicked(FALSE);	// Set DNF Click status
				// Check if we are clicking on a CellDataItem
				if (pCellData->HitItemTest(point) == TRUE)
					pCellData->pCellItem->SetHightlight(TRUE);
			}
		}
		else
		{
			// Remove highlight from current CellData. This is used when the
			// user is clicking inside a cell itself and not a CellDataItem. This will remove
			// the highlight
			pCellData->SetHighlight(FALSE);
		}
	}
	// Force repaint to update control
	InvalidateRect(NULL, TRUE);

	CWnd::OnRButtonDown(nFlags, point);
}

void CWMDateCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pCellData = NULL;
	CRect rect;
	CString strBuf;

#ifdef DEBUG
	strBuf.Format("OnRButtonUp - Clicked point: %d, %d\n", point.x, point.y);
	TRACE(strBuf);
#endif

	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		pCellData->GetCellRect(&rect);

		// Mark which item is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Send Message to parent window
			SendMessageToParent(point.x, point.y, NM_RCLICK);
			break;
		}
	}
	
	CWnd::OnRButtonUp(nFlags, point);
}

// Add a day item to the
long CWMDateCtrl::InsertItem(COleDateTime dtStart, COleDateTime dtEnd, CString strLine /*=""*/, int nImage/*=-1*/)
{
	long nItemID = -1;

	CWVCellData *pData = GetCellByDate(dtStart);	// Select Data Cell
	if (pData != NULL)
	{
		// We have a current cell to work with
		pData->SetImageList(pImageList);
		pData->InsertCellItem(dtStart, dtEnd, strLine, nImage);
		// Set unique ID Number
		nItemID = ++nUniqueItemID;
		pData->pCellItem->SetItemID(nItemID);
	}

	return nItemID;
}

void CWMDateCtrl::SetCurrentDate(COleDateTime dtDate, BOOL bMilitaryTime /*= FALSE*/, 
								 int nView /*= WV_WEEKVIEW*/)
{
	bMilTime = bMilitaryTime;	// Set Military Time display
	bMonthView = nView == WV_WEEKVIEW ? FALSE : TRUE;	// Set drawing mode - Month or Week

	dtDate.SetDate(dtDate.GetYear(), dtDate.GetMonth(), dtDate.GetDay());	// Remove time from date
	dtCurrentDate = dtDate;
	dtLastClicked = dtDate;

	// Initialize Cell Objects
	CWVCellData *pData = NULL;
	CRect rArea;
	// Need to place correct date within each cell. Set up cells for Month View.
	// Calculate 1st day of month position
	dtMonth.SetDate(dtDate.GetYear(), dtDate.GetMonth(), 1);
	dtDate = dtMonth;
	// Find Monday from first day of month
	while (dtDate.GetDayOfWeek() != 2)
		dtDate -= 1;

	for (int x = 1; x <= WV_TOTALCELLS; x++)
	{
		// Get current CellData
		pData = GetCellByID(x);
		if (pData)
		{
			// Set cell values
			pData->SetMilitaryTime(bMilTime);
			pData->SetCellDate(dtDate);

			// Place correct background color in cell
			if (dtDate.GetMonth() == dtMonth.GetMonth())
				pData->SetBkColor(clrBackGrnd);
			else
				pData->SetBkColor(clrOtherMonthBk);

			// Increment day by 1
			dtDate += 1;
		}
	} // for (x = 1; x <= WV_TOTALCELLS; x++)

	pCurrentCell = NULL;	// No current DataCellItem
	// Force a repaint of the control
	InvalidateRect(NULL, TRUE);
}

void CWMDateCtrl::SetFont(CString strFName, int nSize)
{
	nFontSize = nSize;
	strFontName = strFName;
}

// Returns the current date within the displayed week
COleDateTime CWMDateCtrl::GetCurrentDate()
{
	return dtCurrentDate;
}

void CWMDateCtrl::SetItemData(long nItem, DWORD dwData)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		// View all CelDataItems for this CellData
		pCellData->SetItemData(nItem, dwData);
	}
}

DWORD CWMDateCtrl::GetItemData(long nItem)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		// View all CelDataItems for this CellData
		return pCellData->GetItemData(nItem);
	}
	return NULL;
}

BOOL CWMDateCtrl::DeleteItem(long nItem)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		pCellData->DeleteItem(nItem);
	}
	return FALSE;
}

void CWMDateCtrl::DeleteAllItems()
{
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pData = NULL;

	while (pos != NULL)
	{
		pData = (CWVCellData *)m_CellList.GetNext(pos);
		pData->DeleteAllItems();
	}
	InvalidateRect(NULL, TRUE);
}

// Retrieve the current date range for the control
void CWMDateCtrl::GetDateRange(COleDateTime *pStartDate, COleDateTime *pEndDate)
{
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pStartData = NULL;
	CWVCellData *pEndData = NULL;

	if (bMonthView)
	{
		// Locate correct month date range
		pStartData = GetCellByID(1);
		pEndData = GetCellByID(WV_TOTALCELLS);
	}
	else
	{
		// Locate correct week date range
		COleDateTime dtDate = dtCurrentDate;

		// Get first date
		while (dtDate.GetDayOfWeek() != 2){
			dtDate -= 1;
		}
		pStartData = GetCellByDate(dtDate);
		// Get last date
		dtDate = dtCurrentDate;
		dtDate += 6;
		while (dtDate.GetDayOfWeek() != 1){
			dtDate -= 1;
		}
		pEndData = GetCellByDate(dtDate);
	}

	// Return date range
	*pStartDate = pStartData->GetCellDate();
	*pEndDate = pEndData->GetCellDate();
}

// Paint Week Date View
void CWMDateCtrl::PaintWeekView(CPaintDC *pDC)
{
	CRect rect, rCell;
	COleDateTime dtDate = dtCurrentDate;
	COleDateTime dtTemp = dtDate;

	// Find Monday
	while (dtDate.GetDayOfWeek() != 2)
		dtDate -= 1;

	// Get client area to paint
	GetClientRect(&rect);
	rCell.SetRectEmpty();	// Initialize Cell Rect

	// Drawing the WeekView Grid
	// Draw vertical dividing lines
	pDC->MoveTo(rect.right / 2, rect.top);
	pDC->LineTo(rect.right / 2, rect.bottom);

	// Make the lines dividing the rows in both columns
	int nCellHeight = rect.bottom / 3;
	int nLinePos = 0;
	CWVCellData *pCellData = NULL;

	// Line 1 across - make top two cells (2, 5)
	nLinePos = rect.top + nCellHeight;
	pDC->MoveTo(0, nLinePos);
	pDC->LineTo(rect.right, nLinePos);

	// Get Cell Node and store rectangle area
	// Monday
	//pCellData = GetCellByID(1);
	dtTemp = dtDate;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = 0;
	rCell.top = 0;
	rCell.right = rect.right / 2;
	rCell.bottom = rect.top + nCellHeight;
	pCellData->SetCellRect(&rCell);

	// Thursday
	dtTemp = dtDate;
	dtTemp += 3;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = 0;
	rCell.right = rect.right;
	rCell.bottom = rect.top + nCellHeight;
	pCellData->SetCellRect(&rCell);

	// Line 2 across - Makes middle and bottom cells (3, 6) - (4)
	nLinePos += nCellHeight;
	pDC->MoveTo(0, nLinePos);
	pDC->LineTo(rect.right, nLinePos);

	// Tuesday
	dtTemp = dtDate;
	dtTemp += 1;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = 0;
	rCell.top = nCellHeight + 1;
	rCell.right = rect.right / 2;
	rCell.bottom = (nCellHeight * 2);
	pCellData->SetCellRect(&rCell);

	// Friday
	dtTemp = dtDate;
	dtTemp += 4;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = nCellHeight + 1;
	rCell.right = rect.right;
	rCell.bottom = nCellHeight * 2;
	pCellData->SetCellRect(&rCell);

	// Wednesday
	dtTemp = dtDate;
	dtTemp += 2;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = 0;
	rCell.top = rCell.bottom + 1;
	rCell.right = rect.right / 2;
	rCell.bottom = rect.bottom;
	pCellData->SetCellRect(&rCell);

	// Line 3 for the right column - for Sat/Sun - (7, 1)
	nLinePos += (nCellHeight / 2);
	pDC->MoveTo(rect.right / 2, nLinePos);
	pDC->LineTo(rect.right, nLinePos);

	// Saturday
	dtTemp = dtDate;
	dtTemp += 5;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = (nCellHeight * 2) + 1;
	rCell.right = rect.right;
	rCell.bottom = nLinePos;
	pCellData->SetCellRect(&rCell);

	// Sunday
	dtTemp = dtDate;
	dtTemp += 6;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = rCell.bottom + 1;
	rCell.right = rect.right;
	rCell.bottom = rect.bottom; 
	pCellData->SetCellRect(&rCell);
}

// Paint Month Date View
void CWMDateCtrl::PaintMonthView(CPaintDC *pDC)
{
	// Now draw the grid of cells representing the current month/year. We begin
	// at the bottom of the current Header
	CRect rArea(0,0,0,0), rCArea(0,0,0,0);
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);

	// Draw Header
	DrawMonthHeader(pDC);

	// Calculate Real Grid Area - without the day of week header
	GetClientRect(&rArea);
	GetClientRect(&rCArea);
	rCArea.top += nHdrHeight + 1;
	rArea.SetRect(0, nHdrHeight, rCArea.right, rCArea.bottom);

	int nCol = 0, nRow = 0;
	int nInterval = (int)(rCArea.right /6);	// Width of each cell
	int nCellHeight = (int)(rCArea.bottom / 6) - 2; // Height of each cell
	CRect rCell(0,0,0,0), rHdr(0,0,0,0), rClear(0,0,0,0);
	int nX = 0, nY = 0;
	CPen penBlack(PS_SOLID, 1, RGB(0,0,0));
	CBrush brBkGrnd(clrBackGrnd);
	CWVCellData *pData = NULL;
	int nCellCount = 1;

	// Set Pen and Background Brush colors
	pDC->SelectObject(penBlack);
	pDC->SelectObject(brBkGrnd);

	// Draw Cell Rectangles for each Cell
	rCell.SetRectEmpty();
	// Cell 1 - upper left corner
	rCell.left = 0;
	rCell.top = nHdrHeight;
	rCell.right = nInterval + 1;
	rCell.bottom = rCell.top + nCellHeight;

	// For each row and column
	for (nRow = 1; nRow <= 6; nRow++)
	{
		// We go up to 6 because the sixth cell area will hold
		// Saturday and Sunday Date Cells. Otherwise calendar
		// will be off by one day for each week.
		for (nCol = 1; nCol <= 6; nCol++)
		{
			pData = GetCellByID(nCellCount);
			// Header Cell definition
			rHdr = rCell;
			rHdr.left += 2;
			rHdr.top += 1;
			rHdr.right -= 1;
			rHdr.bottom = rHdr.top + nHdrHeight;
			pData->SetHdrRect(&rHdr);

			brBkGrnd.DeleteObject(); // Delete existing brush
			brBkGrnd.CreateSolidBrush(pData->GetBkColor()); // Create new brush
			pDC->SelectObject(brBkGrnd); // Select the current brush

			if (nCol <= 5)
			{
				// Save Cell Rectangle Area coordinates
				pDC->FillSolidRect(&rCell, pData->GetBkColor());
				pData = GetCellByID(nCellCount);
				pData->SetCellRect(&rCell);
			}
			else
			{
				// Column > 5 we need to create two cells
				//for Saturday (6) and Sunday (7)
				CRect rSat, rSun;

				// Initialize Saturday and Sunday Cell Rect areas
				rSat = rSun = rCell;
				// Saturday Header Cell Dimentions
				rHdr.left -= 1;
				rHdr.right = rArea.right;
				pDC->FillSolidRect(&rHdr, pData->GetBkColor());
				// Create Saturday Cell
				rSat.bottom = (rSat.top + (nCellHeight / 2));
				rSat.right = rCell.right - 3;
				pDC->FillSolidRect(&rSat, pData->GetBkColor());
				// Save Cell Rectangle Area coordinates
				pData = GetCellByID(nCellCount);
				pData->SetHdrRect(&rHdr);
				pData->SetCellRect(&rSat);
				nCellCount++;	// Increment Cell Counter to be in Sunday's Cell

				//Sunday Header dimentions
				CWVCellData *pData2 = GetCellByID(nCellCount);
				rHdr.top = rSat.bottom + 1;
				rHdr.right = rArea.right;
				rHdr.bottom = rHdr.top + nHdrHeight + 1;
				// Create Sunday Cell
				rSun.top = rSat.bottom + 1;
				rSun.bottom = rSun.top + (nCellHeight / 2);
				rSun.right = rSat.right;
				pDC->FillSolidRect(&rSun, pData2->GetBkColor());
				// Save Sunday Cell Definition
				pData = GetCellByID(nCellCount);
				pData->SetHdrRect(&rHdr);
				pData->SetCellRect(&rSun);
			}
			nCellCount++;	// Increment Cell Counter to be in next cell
			// Change Cell Rectangle Area
			rCell.left = rCell.right + 1;
			rCell.right = rCell.left + nInterval;
		}	// for (nCol = 1; nCol <= 7; nCol++)

		// Move to next month calendar row
		rCell.left = 0;
		rCell.top = rCell.bottom;
		rCell.right = nInterval + 1;
		rCell.bottom = rCell.top + nCellHeight;
	}	// for (nRow = 1; nRow <= 5; nRow++)

	//-----------------------------------------------------
	// Draw Calendar Horizontal Grid
	GetClientRect(&rArea);
	rCell.left = 0;
	rCell.top = nCellHeight + nHdrHeight;
	rCell.right = rArea.right;
	rCell.bottom = rCell.top + nInterval;
	for (nRow = 1; nRow <= 6; nRow++)
	{
		pDC->MoveTo(rCell.left, rCell.top);
		pDC->LineTo(rCell.right, rCell.top);
		rCell.top += nCellHeight;
	}
	// Draw Calendar Vertical Lines
	GetClientRect(&rArea);
	rCell.left = nInterval + 1;
	rCell.top = nHdrHeight;
	rCell.right = rArea.right;
	rCell.bottom = rArea.bottom;
	for (nCol = 1; nCol <= 5; nCol++)
	{
		pDC->MoveTo(rCell.left, rCell.top);
		pDC->LineTo(rCell.left, rCell.bottom);
		rCell.left += nInterval + 1;
	}
	// Saturday / Sunday lines
	rCell.left -= nInterval + 1;
	rCell.top = (nCellHeight / 2) + nHdrHeight;
	for (nCol = 1; nCol <= 6; nCol++)
	{
		pDC->MoveTo(rCell.left, rCell.top);
		pDC->LineTo(rCell.right, rCell.top);
		rCell.top += nCellHeight;
	}
	penBlack.DeleteObject();
	brBkGrnd.DeleteObject();
}

// Draw Days of the Week for the Month View
void CWMDateCtrl::DrawMonthHeader(CPaintDC *pDC)
{
	// Area to paint
	CRect rArea(0,0,0,0);
	GetClientRect(&rArea);

	CRect rHdrArea, rHdr;
	CRect rHdrText(0,0,0,0);
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);
	int nInterval = (int)(rArea.right / 6);
	CPen penLLine(PS_SOLID, 1, RGB(172, 168,153));
	CPen penRLine(PS_SOLID, 1, RGB(255,255,255));
	CFont fFont, *pOldFont;
	COleDateTime dtDate = dtCurrentDate;
	CString strText = "";

	// Initialize variables
	rHdrArea.left = 0;
	rHdrArea.top = 0;
	rHdrArea.right = rArea.right;
	rHdrArea.bottom = nHdrHeight;

	// Fillin background color
	pDC->FillSolidRect(&rHdrArea, RGB(236, 233, 216));

	// Monday Header
	rHdr.left = rHdrArea.left;
	rHdr.top = rHdrArea.top;
	rHdr.right = rHdrArea.right;
	rHdr.bottom = nHdrHeight;
	pDC->Draw3dRect(rHdrArea, RGB(255,255,255), RGB(172, 168, 153));

	// Draw column separator bars
	rHdr.top = 1;
	rHdr.bottom = nHdrHeight - 1;

	// Make sure we start at Monday
	while (dtDate.GetDayOfWeek() != 2)
		dtDate -= 1;

	// Select current font
	fFont.CreatePointFont(nFontSize, strFontName, pDC);
	pOldFont = pDC->SelectObject(&fFont);

	// Header Rect Area
	rHdrArea.left = 2;
	rHdrArea.top = 2;
	rHdrArea.right = nInterval - 1;
	rHdrArea.bottom = nHdrHeight - 1;

	// Make drawing text transparent
	pDC->SetBkMode(TRANSPARENT);

	for (int x = 1; x <= 7; x++)
	{
		// Draw Day of week text
		pDC->DrawText(_TRANW(dtDate), &rHdrText, DT_SINGLELINE | DT_CALCRECT);
		rHdrText.left = rHdrArea.left;

		if (x < 6)
		{
			// Draw full day text
			pDC->DrawText(_TRANW(dtDate), &rHdrArea, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS);
		}
		else
		{
			// Always Draw Saturday and Sunday in one cell
			strText = _TRANWS(dtDate) + "/";
			dtDate += 1;
			strText += _TRANWS(dtDate);
			pDC->DrawText(strText, &rHdrArea, 
				DT_SINGLELINE | DT_BOTTOM | DT_CENTER | DT_END_ELLIPSIS);
		}
		// Increment day of week
		dtDate += 1;
		// Recalculate Header rect area
		rHdrArea.left = rHdrArea.right + 1;
		rHdrArea.right = rHdrArea.left + nInterval;

		rHdr.left += nInterval;
		rHdr.right = rHdr.left + 2;

		// Select Gray Pen and draw line
		pDC->SelectObject(penLLine);
		pDC->MoveTo(rHdr.left, rHdr.top + 1);
		pDC->LineTo(rHdr.left, rHdr.bottom - 1);

		// Select White Pen and draw line
		pDC->SelectObject(penRLine);
		rHdr.left += 1;
		rHdr.right += 1;
		pDC->MoveTo(rHdr.left, rHdr.top + 1);
		pDC->LineTo(rHdr.left, rHdr.bottom - 1);
		// Move to next column separator
		rHdr.right += nInterval;
	}
	pDC->SelectObject(pOldFont);
}

void CWMDateCtrl::DrawMonthCellHeaders(CPaintDC *pDC)
{
	CRect rect, rHdr, rArea;
	// Calculate height of Cell Header to be the width of the scroll bars
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);
	CBrush brHdrBrush, brOldBrush;
	CFont fFont, *pOldFont;
	CString strBuf;
	COLORREF clrHL = GetSysColor(COLOR_HIGHLIGHT);	// Highlight color
	COLORREF clrFace = GetSysColor(COLOR_BTNFACE);	// Header color
	POSITION posItem = NULL;
	CWVCellDataItem *pCellItem = NULL;
	int nCellHeight = 0, nItem = 1, nCellCount = 1;
	CWVCellData *pCell = NULL;

	// Initialize values
	fFont.CreatePointFont(nFontSize, strFontName, pDC);
	pOldFont = pDC->SelectObject(&fFont);
	// Remove time from date
	dtCurrentDate.SetDate(dtCurrentDate.GetYear(), dtCurrentDate.GetMonth(),
		dtCurrentDate.GetDay());
	// Make drawing text transparent
	pDC->SetBkMode(TRANSPARENT);

	// Get client area to paint
	GetClientRect(&rect);

	for (nItem = 1; nItem <= WV_TOTALCELLS; nItem++)
	{
		CString strText;
		CRect rHdr;

		// Get Cell Data
		pCell = GetCellByID(nItem);

		// Get Cell Header Rect Area
		pCell->GetHdrRect(&rHdr);
		COLORREF rgbRealTBGColor=pCell->GetBkColor();
		if (isToday(pCell->GetCellDate()))
		{
			// This is the current date within the Calendar
			// Gradient Fill this header
			rHdr.left -= 1;
			rHdr.right -= 1;
			FillGradientRect(pDC, &rHdr);
			
			// Calculate Rectangle - for highlighted cell
			CRect rText;
			rText.CopyRect(&rHdr);
			// Check if we highlight Day or Month and Day
			if (pCell->GetCellDate().GetDay() == 1)
				strBuf.Format("%s, %s",_TRANM(pCell->GetCellDate()),pCell->GetCellDate().Format("%d"));
			else
				strBuf = pCell->GetCellDate().Format("%d");

			// Draw text on text area
			pDC->DrawText(strBuf, &rText, DT_CALCRECT);

			if (pCell->GetHighlight())
			{
				// Highlight Header
				rText.left = rHdr.left + (rHdr.right - rText.right)	-2;
				rText.right = rHdr.right+1;
				rText.bottom = rHdr.bottom;
				pDC->FillSolidRect(&rText, clrHL);
				CRect rtMin(rText);
				rtMin.DeflateRect(1,1,1,1);
				pDC->FillSolidRect(&rtMin, clrFace); // Display normal background
				rgbRealTBGColor=clrFace;
				//pDC->SetTextColor(RGB(255,255,255));
			}
		}	// if (pCell->GetCellDate() == dtCurrentDate)
		else
		{
			// Check if we need to highlight cell as well
			rHdr.left -= 1;
			rHdr.top += 1;
			rHdr.right -= 1;
			rHdr.bottom += 1;
			if (pCell->GetHighlight())
			{
				// Normal highlight of header
				CRect rtGD(rHdr);
				rtGD.right+=1;
				pDC->FillSolidRect(&rtGD, clrHL);	// Highlight Header
				CRect rtMin(rtGD);
				rtMin.DeflateRect(1,1,1,1);
				pDC->FillSolidRect(&rtMin, clrFace); // Display normal background
				rgbRealTBGColor=clrFace;
				//pDC->SetTextColor(RGB(255,255,255));
			}
		}

		// Draw date within header
		if (nCellCount == 1 || pCell->GetCellDate().GetDay() == 1)
		{	
			// First cell or first of the month in first cell
			strText.Format("%s %d", _TRANM(pCell->GetCellDate()), pCell->GetCellDate().GetDay());
		}
		else
		{
			// First of the month within the calendar
			if (pCell->GetCellDate().GetDay() == 1)
				strText.Format("%s %d", _TRANM(pCell->GetCellDate()), pCell->GetCellDate().GetDay());
			else
				strText.Format("%d", pCell->GetCellDate().GetDay());	// Display normal day number
		}
		pDC->SetTextColor(RGB(0,0,0));
		pDC->DrawText(strText, &rHdr, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);

		// DrawItems for this cell
		pCell->DrawItems(pDC, bMonthView);

		if(bWeekNumbers){
			if(pCell->GetCellDate().GetDayOfWeek()==2){
				// Week number!!!
				int iWeekNumber=int(pCell->GetCellDate().GetDayOfYear()/7)+1;
				CString sWeekNumber;
				sWeekNumber.Format("%02i",iWeekNumber);
				pDC->SetBkMode(OPAQUE);
				pDC->SetBkColor(rgbRealTBGColor);
				pDC->DrawText(sWeekNumber, &rHdr, DT_SINGLELINE | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);
				pDC->SetBkMode(TRANSPARENT);
			}
		}
		
		// Increment to next cell
		nCellCount++;
	}
	// Replace current Font with original Font
	pDC->SelectObject(pOldFont);
	fFont.DeleteObject();
}

void CWMDateCtrl::SetView(int nV, BOOL bRedraw/*=TRUE*/)
{
	switch(nV)
	{
	case WV_WEEKVIEW:
		bMonthView = FALSE;
		break;

	case WV_MONTHVIEW:
		bMonthView = TRUE;
		break;
	}

	if (bRedraw)
		InvalidateRect(NULL, TRUE);
}

int CWMDateCtrl::GetMonth()
{
	return dtCurrentDate.GetMonth();
}

int CWMDateCtrl::GetYear()
{
	return dtCurrentDate.GetYear();
}

// Set Item Color
void CWMDateCtrl::SetItemColor(long nItem, COLORREF clrItem)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CWVCellData *pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		// View all CelDataItems for this CellData
		pCellData->SetItemColor(nItem, clrItem);
	}
}

void CWMDateCtrl::SetImageList(CImageList *pImgList)
{
	pImageList = pImgList;
}

int CWMDateCtrl::GetView()
{
	if (bMonthView == TRUE)
		return WV_MONTHVIEW;
	else
		return WV_WEEKVIEW;
}

int CWMDateCtrl::GetDay()
{
	return dtCurrentDate.GetDay();
}

void CWMDateCtrl::SetMilitaryTime(BOOL bMil)
{
	bMilTime = bMil;
}

BOOL CWMDateCtrl::IsMilitaryTime()
{
	return bMilTime;
}

void CWMDateCtrl::RedrawControl()
{
	InvalidateRect(NULL, TRUE);
}

void CWMDateCtrl::HightlightCellByDate(COleDateTime dt) 
{
	POSITION pos = m_CellList.GetHeadPosition();
	POSITION posItems = NULL;
	CWVCellData *pCellData = NULL;
	CWVCellDataItem *pItem = NULL;

	// Clear Highlight from any of the CellDataItems
	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		posItems = pCellData->m_ItemList.GetHeadPosition();
		while (posItems != NULL)
		{
			pItem = (CWVCellDataItem *)pCellData->m_ItemList.GetNext(posItems);
			pItem->SetHightlight(FALSE);
		}
	}

	// Locate clicked cell
	pCurrentCell = NULL;	// No current cell
	pos = m_CellList.GetHeadPosition();
	while (pos != NULL)
	{
		pCellData = (CWVCellData *)m_CellList.GetNext(pos);
		COleDateTime dtCell=pCellData->GetCellDate();
		// Mark which item is to be highlighted
		if (dtCell.GetYear()==dt.GetYear() && dtCell.GetMonth()==dt.GetMonth() && dtCell.GetDay()==dt.GetDay())
		{
			// Set Highlight Status
			pCurrentCell = pCellData;	// Make cell current
			dtLastClicked = pCellData->GetCellDate();
			pCellData->SetHighlight(TRUE);	// Highlight Cell Header
		}
		else
		{
			// Remove highlight from current CellData. This is used when the
			// user is clicking inside a cell itself and not a CellDataItem. This will remove
			// the highlight
			pCellData->SetHighlight(FALSE);
		}
	}
	// Force repaint to update control
	InvalidateRect(NULL, TRUE);
}

COleDateTime CWMDateCtrl::GetLastClicked()
{
	if(dtLastClicked.GetStatus()!=COleDateTime::valid){
		if(dtCurrentDate.GetStatus()!=COleDateTime::valid){
			return COleDateTime::GetCurrentTime();
		}
		return dtCurrentDate;
	}
	return dtLastClicked;
};