#pragma once


// COCalendar dialog
#include "WCRemind.h"
class COCalendar : public CResizeableDialog
{
public:
	void CallDialogOnDate(COleDateTime dtTmp);
	void CallContextOnDate(COleDateTime dtTmp);
	void WalkDays(int i);
	CImageList il;
	CToolTipCtrl m_tooltip;
	BOOL bMonView;
	CCalendarCallback* pCallback;
	COCalendar(CCalendarCallback* pCallback, BOOL bViewDef, CWnd* pParent = NULL);   // standard constructor
	BOOL RefreshCalendar();
	virtual ~COCalendar();

	// Dialog Data
	enum { IDD = IDD_DIALOG_CL };

	CWMDateCtrl m_WMCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonNewr();
	afx_msg void OnBnClickedButtonView();
	afx_msg void OnCellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnCellDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnCellClick (NMHDR *pNotifyStruct, LRESULT* pResult);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonNext();
};
