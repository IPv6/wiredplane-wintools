/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Interface of the class "CCalendarDlg2"
	  (handy calendar)
	  
		$Log$
		
******************************************************************************/
#include "Resource.h"
#include "WCRemind.h"

/*** Declaration of the class "CCalendarDlg2" *********************************/
class CCalendarDlg2 : public CResizeableDialog, public CDelegates
{
	// Construction
public:
	DWORD dXPeriod;
	void SetInfoText();
	CFont pDialogFont;
	int iRecurrency;
	DWORD dwOption;
	int iModalRes;
	CWPReminder rem;

	CCalendarDlg2(CWPReminder remIn, CWnd* pParent = NULL, DWORD dwOption=0);	// standard constructor
	~CCalendarDlg2();
	void SelectDate(SYSTEMTIME* sysTime);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
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
	afx_msg void OnBTMp3();
	afx_msg void OnBTMp3Play();
	afx_msg void OnBTApp();
	afx_msg void OnBTRec();
	afx_msg void OnBTSetTim();
	afx_msg void OnMoreOptions();
	afx_msg void OnOK();
	afx_msg void OnDel();
	afx_msg void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeMonthcalendar1(NMHDR*, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};
HWND PlayMusic(CString szSoundPath,BOOL bLoopSound, CString sAlias="wirechanger");
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
