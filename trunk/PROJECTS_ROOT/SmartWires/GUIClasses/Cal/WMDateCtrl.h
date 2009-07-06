#if !defined(AFX_WMDateCtrl_H__AD504FF3_388C_41D6_AD1C_BE2327E49B23__INCLUDED_)
#define AFX_WMDateCtrl_H__AD504FF3_388C_41D6_AD1C_BE2327E49B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WMDateCtrl.h : header file
//

#include "WVCellData.h"
/////////////////////////////////////////////////////////////////////////////
// CWMDateCtrl window

// This structure sent to WMDateCtrl's parent in a WM_NOTIFY message
typedef struct tagNM_WVCELLDATA {
    NMHDR hdr;
	CWVCellData *pCell;
	CWVCellDataItem *pItem;
    int   iRow;
    int   iColumn;
	BOOL bDNFClicked;
	long nItem;
} NM_WVCELLDATA;

#define WV_TOTALCELLS 42
#define WV_WEEKVIEW WM_USER + 1
#define WV_MONTHVIEW WM_USER + 2

class CWMDateCtrl : public CWnd
{
// Construction
public:
	CWMDateCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWMDateCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL bWeekNumbers;
	void SetWeekNumbers(BOOL b){bWeekNumbers=b;};
	void HightlightCellByDate(COleDateTime dt);
	COleDateTime GetCurrentDate();
	void RedrawControl();
	BOOL IsMilitaryTime();
	void SetMilitaryTime(BOOL bMil = TRUE);
	int GetDay();
	int GetView();
	void SetImageList(CImageList *pImgList);
	void SetItemColor(long nItem, COLORREF clrItem);
	int GetYear();
	int GetMonth();
	void SetView(int nV, BOOL bRedraw = TRUE);
	void GetDateRange(COleDateTime *pStartDate, COleDateTime *pEndDate);
	void DeleteAllItems();
	BOOL DeleteItem(long nItem);
	DWORD GetItemData(long nItem);
	void SetItemData(long nItem, DWORD dwData);
	void SetFont(CString strFName, int nSize);
	void SetCurrentDate(COleDateTime dtDate, BOOL bMilitaryTime = FALSE, int nView = WV_WEEKVIEW);
	long InsertItem(COleDateTime dtStart, COleDateTime dtEnd, CString strLine = "", int nImage = -1);
	void SetBkColor(COLORREF clrBk);
	COLORREF GetBkColor();
	COLORREF GetItemColor();
	virtual ~CWMDateCtrl();
	CWVCellData * GetCellByDate(COleDateTime dtDate);
	CWVCellData* HitTest(CPoint point);
	CString GetCellDsk(CWVCellData *pData);
	COleDateTime GetLastClicked();
	CWVCellData * GetCellByID(int nID);
	// Generated message map functions
protected:
	LRESULT SendMessageToParent(int nRow, int nCol, int nMessage) const;
	//{{AFX_MSG(CWMDateCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	COleDateTime dtLastClicked;
	CImageList *pImageList;
	void DrawMonthCellHeaders(CPaintDC *pDC);
	COleDateTime dtMonth;
	void DrawMonthHeader(CPaintDC *pDC);
	BOOL bMonthView;
	void PaintMonthView(CPaintDC *pDC);
	void PaintWeekView(CPaintDC *pDC);
	void DrawWeekCellHeaders(CPaintDC *pDC);
	BOOL bMilTime;
	CString strFontName;
	int nFontSize;
	long nUniqueItemID;
	CWVCellData *pCurrentCell;
	CObList m_CellList;
	void FillGradientRect(CDC *pDC, CRect *pRect);
	COleDateTime dtCurrentDate;
	COLORREF clrBackGrnd, clrOtherMonthBk;
	COLORREF clrGradBot, clrGradTop, clrGradLine;
	BOOL RegisterWindowClass();
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WMDateCtrl_H__AD504FF3_388C_41D6_AD1C_BE2327E49B23__INCLUDED_)
