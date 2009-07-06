// DLG_OptionStyles.h: interface for the CDLG_OptionStyles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLG_OPTIONSTYLES_H__F7026505_8DC0_442B_B642_8FCF170F947A__INCLUDED_)
#define AFX_DLG_OPTIONSTYLES_H__F7026505_8DC0_442B_B642_8FCF170F947A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COptionPage_Styles dialog

class COptionPage_Styles : public CDialog
{

// Construction
public:
	CSmartWndSizer Sizer;
	SimpleTracker Track;
	BOOL bModified;
	void SetCurStyleName();
	void GetCombosState();
	CString sStyleNameMain;
	CString sStyleNameAlert;
	CString sStyleNameMsg;
	CString sStyleNameToDo;
	CString sStyleNameNote;
	CString sStyleNameRemd;
	void InitFields();
	void SetNewFont();
	void SetTxColor(DWORD dwCol);
	void SetBgColor(DWORD dwCol);
	CString sNewName;
	COLORREF m_TxtColor;
	CBrush* hTxt;
	COLORREF m_BgColor;
	CBrush* hBg;
	CFont* font;
	LOGFONT logfont;
	COptionPage_Styles(CWnd* pParent=NULL);
	~COptionPage_Styles();
	void UpdateSchemeCombo();
// Dialog Data
	//{{AFX_DATA(COptionPage_Styles)
	enum { IDD = IDD_DLGSTYLES };
	CSliderCtrl	m_Alpha;
	CString	m_CurStyleString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionPage_Styles)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionPage_Styles)
	afx_msg void OnButtonSettxtcol();
	afx_msg void OnButtonSetbgcol();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCurStyle();
	afx_msg void OnEditchangeCurStyle();
	afx_msg void OnButtonRen();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonRnd();
	afx_msg void OnDestroy();
	afx_msg void OnButtonSetfont();
	afx_msg void OnReleasedcaptureAlphaSlider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DLG_OPTIONSTYLES_H__F7026505_8DC0_442B_B642_8FCF170F947A__INCLUDED_)
