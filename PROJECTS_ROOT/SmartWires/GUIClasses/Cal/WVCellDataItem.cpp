// CWVCellDataItem : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "Resource.h"
#include "WVCellDataItem.h"

/////////////////////////////////////////////////////////////////////////////
// CWVCellDataItem

CWVCellDataItem::CWVCellDataItem()
{
	// Initialize values
	bMilTime = bHighlight = bAlarm = FALSE;
	dtAlarm = COleDateTime::GetCurrentTime();
	dtEnd = dtStart = dtAlarm;
	nItemID = -1;
	strItem = "Unknown Item";
	clrItem = RGB(0,0,0);
	clrBackGrnd = RGB(255, 255, 213);
	nFontSize = 85;
	strFontName = "TAHOMA";
	nImage = -1;
	pImgList = NULL;
}

CWVCellDataItem::~CWVCellDataItem()
{
	// TODO: add destruction code here.
}

/////////////////////////////////////////////////////////////////////////////

void CWVCellDataItem::SetItemID(long nID)
{
	nItemID = nID;
}

long CWVCellDataItem::GetItemID()
{
	return nItemID;
}

void CWVCellDataItem::SetStartTime(COleDateTime dtDate)
{
	dtStart = dtDate;
}

void CWVCellDataItem::GetStartTime(COleDateTime *dtDate)
{
	*dtDate = dtStart;
}

void CWVCellDataItem::SetEndTime(COleDateTime dtDate)
{
	dtEnd = dtDate;
}

void CWVCellDataItem::GetEndTime(COleDateTime *dtDate)
{
	*dtDate = dtEnd;
}

void CWVCellDataItem::SetItemText(CString strLine)
{
	strItem = strLine;
}

CString CWVCellDataItem::GetItemText()
{
	return strItem;
}

void CWVCellDataItem::SetItemAlarm(BOOL bSet, COleDateTime dtDate)
{
	bAlarm = bSet;
	dtAlarm = dtDate;
}

BOOL CWVCellDataItem::IsAlarmSet()
{
	return bAlarm;
}

void CWVCellDataItem::GetAlarmTime(COleDateTime *dtDate)
{
	*dtDate = dtAlarm;
}

void CWVCellDataItem::SetAlarmTime(COleDateTime dtDate)
{
	dtAlarm = dtDate;
}

void CWVCellDataItem::SetItemColor(COLORREF clrColor)
{
	clrItem = clrColor;
}

COLORREF CWVCellDataItem::GetItemColor()
{
	return clrItem;
}

void CWVCellDataItem::SetItemFont(int nSize, CString strFName)
{
	nFontSize = nSize;
	strFontName = strFName;
}

void CWVCellDataItem::GetRectArea(CRect *pRect)
{
	*pRect = rArea;
}

void CWVCellDataItem::SetRectArea(CRect *pRect)
{
	rArea.CopyRect(pRect);
}

void CWVCellDataItem::SetHightlight(BOOL bSet)
{
	bHighlight = bSet;
}

void CWVCellDataItem::DrawItem(CPaintDC *pDC, CRect *pCellArea, CRect *pCellAll, BOOL bMView/*= FALSE*/)
{
	COleDateTime dtS, dtE;
	int nItemHeight = GetSystemMetrics(SM_CXVSCROLL);
	CString strBuf;
	COLORREF clrHL = GetSysColor(COLOR_HIGHLIGHT);	// Highlight color

	// Build string line to display
	GetStartTime(&dtS);
	GetEndTime(&dtE);
	strBuf=GetItemText();
	/*
	if (bMilTime)
	{
		if (!bMView)
			strBuf.Format("%s - %s  %s", dtS.Format("%H%M"), dtE.Format("%H%M"), GetItemText());
		else
			strBuf.Format("%s  %s", dtS.Format("%H%M"), GetItemText());
	}
	else
	{
		if (!bMView)
			strBuf.Format("%s  %s  %s", dtS.Format("%I:%M%p"), dtE.Format("%I:%M%p"), GetItemText());
		else
			strBuf.Format("%s  %s", dtS.Format("%I:%M%p"), GetItemText());
	}
	*/
	// Calculate Rectangle - for highlighted cell
	if(dwData==0xFFFFFFFF){
		if(pCellAll){
			rArea.bottom=pCellAll->bottom;
		}
		strBuf.Replace("\n","\r\n");
	}else{
		strBuf.Replace("\r","");
		strBuf.Replace("\n"," ");
		strBuf.Replace("\t"," ");
	}
	CRect rText;
	rText.CopyRect(&rArea);
	pDC->DrawText(strBuf, &rText, DT_CALCRECT);
	if (bHighlight)
	{
		pDC->FillSolidRect(&rArea, clrHL);	// Highlight Header
		CRect rtMin(rArea);
		rtMin.DeflateRect(1,1,1,1);
		pDC->FillSolidRect(&rtMin, clrBackGrnd); // Display normal background
		//pDC->SetTextColor(RGB(255,255,255));
	}
	else
		pDC->FillSolidRect(&rArea, clrBackGrnd); // Display normal background

	// Draw the items in the order they appear!!
	if (nImage > -1 && pImgList != NULL)
	{
		// Draw specified image from ImageList make space for 
		// icon when displaying the text
		if (pImgList->Draw(pDC, nImage, rArea.TopLeft(), ILD_TRANSPARENT) == TRUE){
			rText.left += 20;	// Space for icon
		}
	}
	rText.right = rArea.right;
	if(dwData!=0xFFFFFFFF){
		pDC->DrawText(strBuf, &rText, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
	}else{
		if(pCellAll){
			rText.bottom=pCellAll->bottom;
		}
		pDC->DrawText(strBuf, &rText, DT_LEFT);
	}
	pDC->SetTextColor(clrItem);
}

void CWVCellDataItem::SetMilitaryTime(BOOL bSet)
{
	bMilTime = bSet;
}

void CWVCellDataItem::SetBkColor(COLORREF clrBk)
{
	clrBackGrnd = clrBk;
}

void CWVCellDataItem::SetItemData(DWORD dwD)
{
	dwData = dwD;
}

DWORD CWVCellDataItem::GetItemData()
{
	return dwData;
}

void CWVCellDataItem::SetImageList(CImageList *pIList)
{
	pImgList = pIList;
}

void CWVCellDataItem::SetItemImage(int nImg)
{
	nImage = nImg;
}
