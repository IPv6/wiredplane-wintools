#if !defined(AFX_DLG_FIND_H__A215B2BD_DA27_4900_B043_3F792E61D867__INCLUDED_)
#define AFX_DLG_FIND_H__A215B2BD_DA27_4900_B043_3F792E61D867__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Find.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_Find dialog
class CFindedUser
{
public:
	CFindedUser()
	{
		iStatus=0;
	};
	int iStatus;
	CString sNick;
	CString sID;
	CString sAddInfo;
	CString sUserDsc;
	CFindedUser& operator=(CFindedUser& CopyObj)
	{
		iStatus=CopyObj.iStatus;
		sNick=CopyObj.sNick;
		sID=CopyObj.sID;
		sAddInfo=CopyObj.sAddInfo;
		sUserDsc=CopyObj.sUserDsc;
		return *this;
	}
};

typedef CArray<CFindedUser,CFindedUser&> COnlineUsers;
class CDLG_Find : public CDialog
{
// Construction
public:
	int iFinded;
	BOOL AddFindPersonResult(CFindedUser& pUser);
	BOOL bAlreadyExist;
	COnlineUsers aFindedUsers;
	BOOL bSearchForPersons;
	BOOL bLimit2FB;
	BOOL bSearchForPersonStarted;
	void OnDblclickResults();
	void OnRclickResults();
	~CDLG_Find();
	LOGFONT logfont;
	CFont* font;
	void SetColorStyle();
	BOOL InitResultsList();
	CDLG_Find(CWnd* pParent = NULL);   // standard constructor
	CSmartWndSizer Sizer;
// Dialog Data
	//{{AFX_DATA(CDLG_Find)
	enum { IDD = IDD_FIND };
	CIListCtrl	m_Results;
	CString	m_Mask;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Find)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_Find)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnGo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_FIND_H__A215B2BD_DA27_4900_B043_3F792E61D867__INCLUDED_)
