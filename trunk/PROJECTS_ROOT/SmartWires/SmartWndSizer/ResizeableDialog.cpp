/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Implementation of class "CResizeableDialog" (resizeable dialog
	  box with smart controls)
	  
		$Log$
		
******************************************************************************/

#include "stdafx.h"
#include "ResizeableDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*** Definition of class "CResizeableDialog" *********************************/
const UINT CResizeableDialog::ATTACH_TO_LEFT_EDGE        = 0x01;
const UINT CResizeableDialog::ATTACH_TO_RIGHT_EDGE       = 0x02;
const UINT CResizeableDialog::ATTACH_TO_VERTICAL_EDGES   =
CResizeableDialog::ATTACH_TO_LEFT_EDGE |
CResizeableDialog::ATTACH_TO_RIGHT_EDGE;
const UINT CResizeableDialog::ATTACH_TO_TOP_EDGE         = 0x04;
const UINT CResizeableDialog::ATTACH_TO_BOTTOM_EDGE      = 0x08;
const UINT CResizeableDialog::ATTACH_TO_HORIZONTAL_EDGES =
CResizeableDialog::ATTACH_TO_TOP_EDGE |
CResizeableDialog::ATTACH_TO_BOTTOM_EDGE;
const UINT CResizeableDialog::ATTACH_TO_ALL_EDGES        =
CResizeableDialog::ATTACH_TO_VERTICAL_EDGES |
CResizeableDialog::ATTACH_TO_HORIZONTAL_EDGES;
const UINT CResizeableDialog::CENTER_HORIZONTAL          = 0x10;
const UINT CResizeableDialog::CENTER_VERTICAL            = 0x20;
const UINT CResizeableDialog::CENTER                     =
CResizeableDialog::CENTER_HORIZONTAL |
CResizeableDialog::CENTER_VERTICAL;

const int CResizeableDialog::m_nMaxStandardWidth  = 600;
const int CResizeableDialog::m_nMaxStandardHeight = 400;

/*** Constructor *************************************************************/
CResizeableDialog::CResizeableDialog(UINT nIDTemplate, CWnd* pParentWnd):
CDialog(nIDTemplate, pParentWnd)
{
	//{{AFX_DATA_INIT(CResizeableDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	bMiniScroll=0;
	iGlobalOffsetX=0;
	iGlobalOffsetY=0;
	m_nScreenWidth     = GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight    = GetSystemMetrics(SM_CYSCREEN);
	m_nMinWidth        = -1;
	m_nMinHeight       = -1;
	m_nMaxWidth        = -1;
	m_nMaxHeight       = -1;
	m_bNotYetMaximized = true;
}

/*** Destructor **************************************************************/
CResizeableDialog::~CResizeableDialog()
{
	for (POSITION pos = m_mapCtrls.GetStartPosition(); pos;)
	{
		UINT      nID;
		CtrlData* pCtrlData;
		
		m_mapCtrls.GetNextAssoc(pos, nID, pCtrlData);
		m_mapCtrls.RemoveKey   (nID);
		delete pCtrlData;
	}
}

/*** Public member functions *************************************************/

/*** Register control to be resized or centered ******************************/
UINT CResizeableDialog::RegisterControl(UINT nID, UINT nSmartMode)
{
	CtrlData* pCtrlData;
	
	if (m_mapCtrls.Lookup(nID, pCtrlData))
	{
		ASSERT(false);
		return 0;
	}
	else
	{
		// check for inconsistent and unnecessary parameters
		if (nSmartMode & ATTACH_TO_VERTICAL_EDGES)
		{
			if (nSmartMode & CENTER_HORIZONTAL)
			{
				ASSERT(false);
				nSmartMode &= ~(ATTACH_TO_VERTICAL_EDGES | CENTER_HORIZONTAL);
			}
			if (!(nSmartMode & ATTACH_TO_RIGHT_EDGE))
				nSmartMode &= ~ATTACH_TO_LEFT_EDGE;
		}
		if (nSmartMode & ATTACH_TO_HORIZONTAL_EDGES)
		{
			if (nSmartMode & CENTER_VERTICAL)
			{
				ASSERT(false);
				nSmartMode &= ~(ATTACH_TO_HORIZONTAL_EDGES | CENTER_VERTICAL);
			}
			if (!(nSmartMode & ATTACH_TO_BOTTOM_EDGE))
				nSmartMode &= ~ATTACH_TO_TOP_EDGE;
		}
		
		if (nSmartMode)
		{
			pCtrlData               = new CtrlData;
			pCtrlData->m_nSmartMode = nSmartMode;
			
			// calculate distances between control and the edges of the client area
			// of this dialog box
			CRect rectThis(m_rectOrig);
			ClientToScreen(rectThis);
			CRect rectCtrl;
			GetDlgItem(nID)->GetWindowRect(rectCtrl);
			pCtrlData->m_rectDistances.left   = rectCtrl.left   - rectThis.left;
			pCtrlData->m_rectDistances.right  = rectThis.right  - rectCtrl.right;
			pCtrlData->m_rectDistances.top    = rectCtrl.top    - rectThis.top;
			pCtrlData->m_rectDistances.bottom = rectThis.bottom - rectCtrl.bottom;
			
			m_mapCtrls[nID] = pCtrlData;
		}
	}
	return nSmartMode;
}

/*** Set maximum size to current size ****************************************/
void CResizeableDialog::ThisSizeIsMaxSize()
{
	CRect rect;
	
	GetWindowRect(rect);
	m_nMaxWidth  = rect.Width();
	m_nMaxHeight = rect.Height();
}

/*** Set minimum size to current size ****************************************/
void CResizeableDialog::ThisSizeIsMinSize()
{
	CRect rect;
	
	GetWindowRect(rect);
	m_nMinWidth  = rect.Width();
	m_nMinHeight = rect.Height();
}

/*** Protected member functions **********************************************/

/*** Data exchange: member variables <--> controls ***************************/
void CResizeableDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResizeableDialog)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/*** Windows needs to know the maximized size and position *******************/
void CResizeableDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_nMaxWidth != -1 && m_nMaxHeight != -1)
	{
		if (m_bNotYetMaximized && !IsIconic() && !IsZoomed())
		{
			// maximize the window centered to the previous restored position
			CRect rect;
			GetWindowRect(rect);
			
			// calculate position of maximized window
			rect.left -= (m_nMaxWidth  - rect.Width())  / 2;
			rect.top  -= (m_nMaxHeight - rect.Height()) / 2;
			
			// correct position of maximized window to avoid clipping
			if (rect.left + m_nMaxWidth > m_nScreenWidth)
				rect.left = m_nScreenWidth - m_nMaxWidth;
			if (rect.top + m_nMaxHeight > m_nScreenHeight)
				rect.top = m_nScreenHeight - m_nMaxHeight;
			if (rect.left < 0) rect.left = 0;
			if (rect.top < 0) rect.top = 0;
			
			lpMMI->ptMaxPosition = rect.TopLeft();
		}
		lpMMI->ptMaxSize.x      = m_nMaxWidth;
		lpMMI->ptMaxSize.y      = m_nMaxHeight;
		lpMMI->ptMaxTrackSize.x = m_nMaxWidth;
		lpMMI->ptMaxTrackSize.y = m_nMaxHeight;
	}
	
	if (m_nMinWidth != -1 && m_nMinHeight != -1)
	{
		lpMMI->ptMinTrackSize.x = m_nMinWidth;
		lpMMI->ptMinTrackSize.y = m_nMinHeight;
	}
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

/*** Called before the first display of the dialog box ***********************/
BOOL CResizeableDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// remember original (= minimum) size
	GetWindowRect(m_rectOrig);
	m_nMinWidth  = m_rectOrig.Width();
	m_nMinHeight = m_rectOrig.Height();
	GetClientRect(m_rectOrig);
	
	// calculate standard size
	m_nStandardWidth  = m_nScreenWidth  / 2;
	m_nStandardHeight = m_nScreenHeight / 2;
	
	if (m_nStandardWidth < m_nMinWidth || m_nStandardHeight < m_nMinHeight)
	{
		m_nStandardWidth  = m_nMinWidth;
		m_nStandardHeight = m_nMinHeight;
	}
	else
	{
		if (m_nStandardWidth > m_nMaxStandardWidth)
			m_nStandardWidth = m_nMaxStandardWidth;
		if (m_nStandardHeight > m_nMaxStandardHeight)
			m_nStandardHeight = m_nMaxStandardHeight;
	}
	
	m_nStandardXPos = (m_nScreenWidth  - m_nStandardWidth)  / 2;
	m_nStandardYPos = (m_nScreenHeight - m_nStandardHeight) / 2;
	
	return TRUE;
}

/*** The dialog box will be repainted ****************************************/
void CResizeableDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (!IsZoomed())
	{
		// draw size grip to signal that the dialog box can be resized
		GetClientRect(rLastGripRect);
		
		rLastGripRect.left = rLastGripRect.right  - GetSystemMetrics(SM_CXHSCROLL);
		rLastGripRect.top  = rLastGripRect.bottom - GetSystemMetrics(SM_CYVSCROLL);
		if(bMiniScroll){
			rLastGripRect.left += bMiniScroll;
			rLastGripRect.left += bMiniScroll;
		}
		dc.DrawFrameControl(rLastGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}
}

/*** The size of the dialog box has been changed *****************************/
void CResizeableDialog::ApplyLayout()
{

	CRect rectThis;
	GetClientRect (rectThis);
	ClientToScreen(rectThis);

	for (POSITION pos = m_mapCtrls.GetStartPosition(); pos;)
	{
		UINT      nID;
		CtrlData* pCtrlData;
		m_mapCtrls.GetNextAssoc(pos, nID, pCtrlData);

		CWnd* pCtrl = GetDlgItem(nID);
		if(pCtrl && pCtrl->IsWindowEnabled()){
			CRect rectCtrl;
			pCtrl->GetWindowRect(rectCtrl);
			int nCtrlWidth  = rectCtrl.Width();
			int nCtrlHeight = rectCtrl.Height();

			// update control coordinates
			if (pCtrlData->m_nSmartMode & ATTACH_TO_ALL_EDGES)
			{
				if (pCtrlData->m_nSmartMode & ATTACH_TO_LEFT_EDGE)
				{
					rectCtrl.left = rectThis.left + pCtrlData->m_rectDistances.left;
					if (!(pCtrlData->m_nSmartMode & ATTACH_TO_RIGHT_EDGE))
						rectCtrl.right = rectCtrl.left + nCtrlWidth;
				}
				if (pCtrlData->m_nSmartMode & ATTACH_TO_RIGHT_EDGE)
				{
					rectCtrl.right = rectThis.right - pCtrlData->m_rectDistances.right;
					if (!(pCtrlData->m_nSmartMode & ATTACH_TO_LEFT_EDGE))
						rectCtrl.left = rectCtrl.right - nCtrlWidth;
				}
				if (pCtrlData->m_nSmartMode & ATTACH_TO_TOP_EDGE)
				{
					rectCtrl.top = rectThis.top + pCtrlData->m_rectDistances.top;
					if (!(pCtrlData->m_nSmartMode & ATTACH_TO_BOTTOM_EDGE))
						rectCtrl.bottom = rectCtrl.top + nCtrlHeight;
				}
				if (pCtrlData->m_nSmartMode & ATTACH_TO_BOTTOM_EDGE)
				{
					rectCtrl.bottom =
						rectThis.bottom - pCtrlData->m_rectDistances.bottom;
					if (!(pCtrlData->m_nSmartMode & ATTACH_TO_TOP_EDGE))
						rectCtrl.top = rectCtrl.bottom - nCtrlHeight;
				}
			}

			if (pCtrlData->m_nSmartMode & CENTER)
			{
				if (pCtrlData->m_nSmartMode & CENTER_HORIZONTAL)
				{
					rectCtrl.left  = rectThis.left + (rectThis.Width() - nCtrlWidth) / 2;
					rectCtrl.right = rectCtrl.left + nCtrlWidth;
				}
				if (pCtrlData->m_nSmartMode & CENTER_VERTICAL)
				{
					rectCtrl.top    =
						rectThis.top + (rectThis.Height() - nCtrlHeight) / 2;
					rectCtrl.bottom = rectCtrl.top + nCtrlHeight;
				}
			}

			// convert control coordinates from screen coordinates to coordinates
			// relative to the client area of this dialog box
			rectCtrl.left   -= rectThis.left;
			rectCtrl.right  -= rectThis.left;
			rectCtrl.top    -= rectThis.top;
			rectCtrl.bottom -= rectThis.top;
			rectCtrl.OffsetRect(iGlobalOffsetX,iGlobalOffsetY);
			// resize/move control
			pCtrl->MoveWindow(rectCtrl);
		}
	}

	Invalidate();   // redraw window to erase old size grip
}

void CResizeableDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (nType == SIZE_MAXIMIZED || nType == SIZE_RESTORED)
	{
		ApplyLayout();
	}
}

/*** The position of this window has been changed ****************************/
void CResizeableDialog::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CDialog::OnWindowPosChanged(lpwndpos);
	
	if (IsZoomed())
		// remember that window has been maximized
		m_bNotYetMaximized = false;
}

/*** Table of message handlers ***********************************************/
BEGIN_MESSAGE_MAP(CResizeableDialog, CDialog)
//{{AFX_MSG_MAP(CResizeableDialog)
ON_WM_PAINT()
ON_WM_SIZE()
ON_WM_GETMINMAXINFO()
ON_WM_WINDOWPOSCHANGED()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
