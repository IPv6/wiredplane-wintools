#if !defined(AFX_DLG_INFOWINDOW_H__371CD9EC_22B0_4D2E_B199_76446DFE733A__INCLUDED_)
#define AFX_DLG_INFOWINDOW_H__371CD9EC_22B0_4D2E_B199_76446DFE733A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_InfoWindow.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// InfoWnd dialog
//#include "\PROJECTS_ROOT\Tools\SmartWndSizer\SmartWndSizer.h"
#include "_common.h"
class InfoWnd;
class CInfoDsc : public CButton
{
	DECLARE_DYNAMIC(CInfoDsc)
	InfoWnd* m_pDialog;

public:
	BOOL bFirstTime;
	CInfoDsc(InfoWnd* pDialog);
	virtual ~CInfoDsc();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};


typedef BOOL (CALLBACK *OnButtonCallback) (void* Param, int iButtonNum, HWND hThisWindow);
class InfoWndStruct
{
public:
	CWnd* pParent;
	BOOL bAsModal;
	BOOL bCenterUnderMouse;
	BOOL bPlaceOnTaskbarAnyway;
	BOOL bHideInfoTitler;
	CString sText;
	CString sTitle;
	CString sIconTooltip;
	CString sOkButtonText;
	CString sCancelButtonText;
	CString sAskAgainButtonText;
	DWORD dwBlockCancel;
	long* dwDontAskHolder;// Null - не нужен, 0 - не спрашивать снова, 1 - да, 2- нет
	long iIcon;// Может быть меньше 0я - означает картинку из главного image list
	DWORD dwTimer;
	BOOL bStartHided;//0-не скрыта, 1-скрыта, 2-скрыта, самоудаляется после открытия
	HWND hWnd;// ссылка на окно?
	int iStyle;
	DWORD dwStaticPars;
	CRect rtStart;
	//
	InfoWndStruct();
	~InfoWndStruct();
};

class InfoWnd : public CDialog
{
// Construction
public:
	BOOL bCloseReason;
	DWORD dwAdditionalTxtOpts;
	DWORD m_dwBGCOLOR;
	DWORD m_dwTXCOLOR;
	void GetInfoIcon();
	int MainRulePos;
	BOOL bCallbackInProgress;
	BOOL InCallback();
	BOOL CheckCallback(int iButton);
	InfoWndStruct* Sets;
	DWORD dwTimer;
	DWORD dwMilliseconds;
	DWORD dwMillisecondsStart;
	void* CBParam;
	OnButtonCallback callback;
	POINT fromPoint;
	BOOL isMove;
	HICON hIcon;
	CRect Rect;
	CBrush* m_hBr;
	CSystemTray m_STray;
	CSmartWndSizer Sizer;
	void SetTextParams(DWORD dwFlags);
	void OpenSlayerWindow();
	void SetParams(long iIcon=IDI_ICON_INFO_AST, const char* szTitle=NULL, const char* szText=NULL);
	void SetInfoIcon(long iIcon);
	void ExitWindow(int iRes);
	BOOL GetInfoWndSize(CSize& size);
	void SetCallback(OnButtonCallback func, void* param);
	void SetOkText(const char* szOk);
	void SetText(const char* szText);
	void SetTitle(const char* szText);
	void SetAutoCloseTime(DWORD dwMilliseconds);
	void Cleanup();
	void RunTimer(DWORD dwTimeout);
	void StartTimer(DWORD dwMilliseconds);
	void SetTime(DWORD dwTime);
	void Init(InfoWndStruct* pInfo);
	afx_msg void OnInfowndOpen();
	~InfoWnd();
	InfoWnd(InfoWndStruct* pInfo);
	InfoWnd(InfoWndStruct* pInfo, CWnd* pParWnd);
// Dialog Data
	//{{AFX_DATA(InfoWnd)
	enum { IDD = IDD_INFO_WND };
	CButton	m_AskAgain;
	CStatic	m_Title;
	CButton	m_Cancel;
	CBitmapStatic m_Icon;
	CInfoDsc m_Text;
	CButton	m_OkButton;
	CBitmapStatic m_BtMinim;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(InfoWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(InfoWnd)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnInfowndDelete();
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAskagain();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMinimize();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_DLG_INFOWINDOW_H__371CD9EC_22B0_4D2E_B199_76446DFE733A__INCLUDED_)
