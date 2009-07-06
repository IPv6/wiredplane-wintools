/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Interface of the class "CCalendarDlg"
	  (handy calendar)
	  
		$Log$
		
******************************************************************************/
#include "Resource.h"
#include "Pickers.h"
#include "..\..\SmartWires\SmartWndSizer\ResizeableDialog.h"

/*** Declaration of the class "CCalendarDlg" *********************************/
class CCalendarDlg : public CResizeableDialog
{
	// Construction
public:
	CString sButton;
	CString sTitle;
	COleDateTime dtSelected;
	CCalendarDlg(CWnd* pParent = NULL);	// standard constructor
	// Dialog Data
	//{{AFX_DATA(CCalendarDlg)
	enum { IDD = IDD_CALENDAR_DIALOG };
	CMonthCalCtrlEx	m_mcMonthCal;
	//}}AFX_DATA
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarDlg)
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CCalendarDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeMonthcalendar1(NMHDR*, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
private:
	void DisplayNameOfSelectedDay();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
