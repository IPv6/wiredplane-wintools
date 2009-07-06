#if !defined(AFX_DLG_REMINDER_H__7ABCD887_B208_483E_9C40_3F01638725A7__INCLUDED_)
#define AFX_DLG_REMINDER_H__7ABCD887_B208_483E_9C40_3F01638725A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Reminder.h : header file
//
class DLG_Reminder;
class CWatchPreview : public CButton
{
	DECLARE_DYNAMIC(CWatchPreview)
	DLG_Reminder* m_pWDialog;

public:
	CWatchPreview(DLG_Reminder* pWDialog);
	virtual ~CWatchPreview();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// DLG_Reminder dialog
#include "Reminders.h"
#include "..\SmartWires\SystemUtils\SupportClasses.h"
#include "..\SmartWires\SmartWndSizer\SmartWndSizer.h"
class DLG_Reminder : public CDialog
{
// Construction
public:
	void SetRemText(const char* szNewText);
	BOOL CanChoosePar(int iSelNum);
	BOOL CanChooseParEdit(int iSelNum);
	void ExitRemWindow(int iCode);
	BOOL bThisIsNewReminder;
	void AddSomeMinutesToTime(int iMinutes);
	void SetComboActionPos();
	SimpleTracker Track;
	CImageList m_ComboActionsImages;
	void SetComboAction(BOOL bFlag);
	UINT iLastMin;
	double dLastHour;
	int MainRulePos;
	int iStyle;
	BOOL bWatchLock;
	BOOL bWatchLockR;
	CSmartWndSizer Sizer;
	Reminder* reminder;
	DLG_Reminder(CWnd* pParent = NULL);
	virtual ~DLG_Reminder();
	BOOL ApplySettedValues();
	void OffsetWatch(int iMinuteDx, int iHourDx);

// Dialog Data
	//{{AFX_DATA(DLG_Reminder)
	enum { IDD = IDD_REMINDER };
	CComboBoxEx	m_IconCombo;
	CComboBoxEx	m_ComboActions;
	CMonthCalCtrl	m_Calendar;
	CWatchPreview	m_Watch;
	int		m_Month_Exact;
	int		m_Year_Exact;
	int		m_Day_Exact;
	BOOL	m_Active_Reminder;
	CString	m_EditDay;
	CString	m_EditMonth;
	CString	m_EditYear;
	CTime	m_Time;
	CString	m_ReminderTitle;
	CString	m_EditActionPar;
	int		m_ComboWDaysList;
	int		m_ComboTimeDistr;
	BOOL	m_RepeatedTime;
	CString	m_ComboHourList;
	int		m_AmPm;
	CString	m_ReminderText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DLG_Reminder)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DLG_Reminder)
	afx_msg void OnClose();
	afx_msg void OnButtonSaveexit();
	afx_msg void OnSelchangeComboAction();
	afx_msg void OnChangeEditActionPar();
	afx_msg void OnButtonActionParChoser();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonDontsaveexit();
	afx_msg void OnSelectMonthcalendar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnWatch();
	afx_msg void OnRadioAm();
	afx_msg void OnRadioPm();
	afx_msg void OnDatetimechangeTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonCview();
	afx_msg void OnRadioDayEveryWday();
	afx_msg void OnRadioDayEvery();
	afx_msg void OnRadioMonEvery();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnButtonTest();
	afx_msg void OnDblclkComboAction();
	afx_msg void OnButtonA15();
	afx_msg void OnButtonA60();
	afx_msg void OnActiveChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_REMINDER_H__7ABCD887_B208_483E_9C40_3F01638725A7__INCLUDED_)
