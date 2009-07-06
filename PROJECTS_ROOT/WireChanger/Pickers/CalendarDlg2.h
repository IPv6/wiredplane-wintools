/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Interface of the class "CCalendarDlg2"
	  (handy calendar)
	  
		$Log$
		
******************************************************************************/
#include "Resource.h"
#include "Pickers.h"
#include "..\..\SmartWires\SmartWndSizer\ResizeableDialog.h"

/*** Declaration of the class "CCalendarDlg2" *********************************/
class CCalendarDlg2 : public CResizeableDialog, public CDelegates
{
	// Construction
public:
	CString sButton;
	CString sTitle;
	CString sStatic1;
	CString sStatic2;
	CStringArray aItems;
	CString sKey;
	BOOL bINewLine;
	CString sMainFile;
	  	COleDateTime dtSelected;
		CCalendarDlg2(CWnd* pParent = NULL);	// standard constructor
		void SetTextForDay(SYSTEMTIME* sysTime);		
		// Dialog Data
		//{{AFX_DATA(CCalendarDlg2)
		enum { IDD = IDD_CALENDAR2_DIALOG };
		CMonthCalCtrlEx	m_mcMonthCal;
		CEdit m_eText;
		//}}AFX_DATA
		
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CCalendarDlg2)
protected:
	virtual afx_msg void OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CCalendarDlg2)
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnClear();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeMonthcalendar1(NMHDR*, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
