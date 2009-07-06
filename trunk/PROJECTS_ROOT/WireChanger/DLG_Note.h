#if !defined(AFX_DLG_NOTE_H__046438A3_913C_4902_AA9C_E38347C79C7A__INCLUDED_)
#define AFX_DLG_NOTE_H__046438A3_913C_4902_AA9C_E38347C79C7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_Note.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_Note dialog
#include "WallpaperUtils.h"
#include "..\SmartWires\BitmapUtils\MenuToolTip.h"
class CDLG_Note : public CDialog
{
// Construction
public:
	void IncreaseFont(int iDir);
	BOOL SetCustomFont(CString sNewFont);
	CString sNoteOnDate;
	void PrintNote();
	void OnNotemenuReminder();
	void OnNotemenuSnaptocontent();
	void SetNewTX(COLORREF dwCol);
	void OnNotemenuSaveToFile();
	void OnNotemenuBindToFile();
	CString sTitle;
	void ApplyView();
	CFont* pFont;
	BOOL ChooseCustomFont(int iBased=-1);
	BOOL ChooseCustomIcon(int iBased=-1);
	BOOL ChooseCustomBg(int iBased=-1);
	BOOL bNoColorSelect;
	BOOL bNoChangeView;
	BOOL bNoChangeIcon;
	void SetViewMode(int iNewView);
	void Apply(BOOL bNotify=TRUE);
	BOOL DrawNCArea(HDC hdc);
	void OnNoteiconEx(BOOL bAtCursor, BOOL bWithCP=FALSE,DWORD dwUID=0);
	CBitmap* bmBackground;
	int MainRulePos;
	HICON hIco;
	CNote* pNote;
	CBrush brText;
	CBrush brBG;
	CBrush brTX;
	HICON hIDel;
	HICON hISave;
	HICON hISwitch;
	HICON hIRem;
	BOOL bTitleDisabled;
	BOOL bNoMove;
	BOOL bNoSize;
	BOOL bNoFont;
	BOOL bJustEdit;
	BOOL bNoLayout;
	BOOL bNoBGImage;
	BOOL bNewNote;
	BOOL bDefColors;
	DWORD dwTX;
	DWORD dwBG;
	BOOL RefreshIcon();
	CSmartWndSizer Sizer;
	CToolTipCtrl m_tooltip;
	DWORD dwToolTipDelay;
	DWORD dwToolTipDelay2;
	DWORD dwToolTipDelay3;
	DWORD dwToolTipDelay4;
	BOOL m_bTileBG;
	BOOL m_bOpacity;
	void DelatIcon(int iDir);
	BOOL GetCapButtonRect(CRect& rt, int iBtNum);
	CDLG_Note(CWnd* pParent = NULL);
	~CDLG_Note(){};//Все в PostNcDestroy
	BOOL bForceFocus;
// Dialog Data
	//{{AFX_DATA(CDLG_Note)
	enum { IDD = IDD_NOTE };
	CBitmapStatic m_Icon;
	CBitmapStatic m_BG;
	CBitmapStatic m_TX;
	CBitmapStatic m_FN;
	CBitmapStatic m_BG2;
	CBitmapStatic m_TX2;
	CBitmapStatic m_FN2;
	CBitmapStatic m_VI2;
	CBitmapStatic m_VI;
	CString	m_Title;
	int m_iIcon;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Note)
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
	//{{AFX_MSG(CDLG_Note)
	virtual BOOL OnInitDialog();
	afx_msg void OnDel();
	afx_msg void OnOk();
	afx_msg void OnTX();
	afx_msg void OnBG();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnView();
	afx_msg void OnView2();
	afx_msg void OnNoteicon();
	afx_msg void OnClose();
	afx_msg void OnFn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_DLG_NOTE_H__046438A3_913C_4902_AA9C_E38347C79C7A__INCLUDED_)
