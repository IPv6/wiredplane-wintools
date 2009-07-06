#if !defined(AFX_DLG_NOTESETTINGS_H__59D5DDAC_F063_46FC_A01D_0DDBAF986759__INCLUDED_)
#define AFX_DLG_NOTESETTINGS_H__59D5DDAC_F063_46FC_A01D_0DDBAF986759__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_NoteSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_NoteSettings dialog

class CDLG_NoteSettings : public CDialog
{
// Construction
public:
	CSmartWndSizer Sizer;
	SimpleTracker Track;
	CActiItem* pItem;
	CDLG_NoteSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_NoteSettings)
	enum { IDD = IDD_NOTESETTINGS };
	CString	m_PinupTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_NoteSettings)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_NoteSettings)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_NOTESETTINGS_H__59D5DDAC_F063_46FC_A01D_0DDBAF986759__INCLUDED_)
