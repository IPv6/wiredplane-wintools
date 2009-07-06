// CWVCellDataItem : header file
/////////////////////////////////////////////////////////////////////////////

#ifndef __WVCELLDATAITEM_H
#define __WVCELLDATAITEM_H

/////////////////////////////////////////////////////////////////////////////
// CWVCellDataItem class

class CWVCellDataItem : public CObject
{
// Construction / destruction
public:

	// Constructs a CWVCellDataItem object.
	CWVCellDataItem();

	// Destroys a CWVCellDataItem object, handles cleanup and de-allocation.
	virtual ~CWVCellDataItem();

// Member variables
protected:

// Member functions
public:
	void SetItemImage(int nImg);
	void SetImageList(CImageList *pIList);
	DWORD GetItemData();
	void SetItemData(DWORD dwD);
	void SetBkColor(COLORREF clrBk);
	void SetMilitaryTime(BOOL bSet);
	void DrawItem(CPaintDC *pDC, CRect *pCellArea, CRect *pCellAll, BOOL bMView = FALSE);
	void SetHightlight(BOOL bSet);
	void SetRectArea(CRect *pRect);
	void GetRectArea(CRect *pRect);
	void SetItemFont(int nSize, CString strFName);
	COLORREF GetItemColor();
	void SetItemColor(COLORREF clrColor);
	void SetAlarmTime(COleDateTime dtDate);
	void GetAlarmTime(COleDateTime *dtDate);
	BOOL IsAlarmSet();
	void SetItemAlarm(BOOL bSet, COleDateTime dtDate);
	CString GetItemText();
	void SetItemText(CString strLine);
	void GetEndTime(COleDateTime *dtDate);
	void SetEndTime (COleDateTime dtDate);
	void GetStartTime(COleDateTime *dtDate);
	void SetStartTime(COleDateTime dtDate);
	long GetItemID();
	void SetItemID(long nID);

private:
	int nImage;
	CImageList *pImgList;
	DWORD dwData;
	COLORREF clrBackGrnd;
	BOOL bMilTime;
	long nItemID;
	COleDateTime dtStart, dtEnd;
	CString strItem;
	COLORREF clrItem;
	BOOL bAlarm, bHighlight;
	COleDateTime dtAlarm;
	CRect rArea;
	int nFontSize;
	CString strFontName;
};

/////////////////////////////////////////////////////////////////////////////

#endif // __WVCELLDATAITEM_H

