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


typedef BOOL (CALLBACK *OnButtonCallback) (void* Param, int iButtonNum);
class InfoWndStruct
{
public:
	static long dwIDCounter;
	DWORD dwID;
	BOOL bLastOpenedTrayMenuType;
	CWnd* pParent;
	BOOL bAsModal;
	BOOL bCenterUnderMouse;
	BOOL bDetached;
	CIHotkey fHK;
	long xPos;
	long yPos;
	CBitmap* bmScreenshot;
	CString sText;
	CString sTitle;
	CString sOkButtonText;
	CString sCancelButtonText;
	CString sAskAgainButtonText;
	long* dwDontAskHolder;// Null - не нужен, 0 - не спрашивать снова, 1 - да, 2- нет
	long iIcon;// Может быть меньше 0я - означает картинку из главного image list
	DWORD dwTimer;
	BOOL bStartHided;//0-не скрыта, 1-скрыта, 2-скрыта, самоудаляется после открытия, 3-как 2 только без иконки в трее
	HWND hWnd;// ссылка на окно?
	int iStyle;
	DWORD dwStaticPars;
	HICON hIcon;
	HICON hIconBig;
	BOOL bForceMinType;
	//
	InfoWndStruct();
	~InfoWndStruct();
};

class InfoWnd : public CDialog
{
// Construction
public:
	HANDLE hTitlePoolerEvent;
	HANDLE hTitlePooler;
	BOOL bOpenSlayerANDSwitch;
	long lMoveDist;
	BOOL bExpandableDirectory;
	long lDoNotPerformAnything;
	BOOL RegisterIWHK(BOOL bReg);
	CStringArray aExpandedFolderPaths;
	void ShowFloater(int bShow);
	DWORD dwPrevTime;
	DWORD dwLastMovementTime;
	BOOL bDetachedFromSaveValid;
	BOOL bDetachedFromSave;
	HWND hDDEWnd;
	CToolTipCtrl m_tooltip;
	long lInMenuLock;
	int iHiddenWndNumber;
	BOOL bOnTaskBar;
	void GetSlayerMenuItem(HMENU pWndMenu, DWORD dwCode);
	aNum2IDMap aM2I;
	CArray<DWORD,DWORD> aLastCommandsCodes;
	CArray<MENUITEMINFO,MENUITEMINFO&> aLastCommandsInfos;
	CStringArray aLastCommandsNames;
	CString GetTitle();
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
	CPoint fromPoint;
	BOOL isMove;
	CRect Rect;
	CBrush* m_hBr;
	CSystemTray m_STray;
	CSmartWndSizer Sizer;
	void SetTextParams(DWORD dwFlags);
	void OpenSlayerWindow();
	void SetParams(UINT iIcon=IDI_ICON_INFO_AST, const char* szTitle=NULL, const char* szText=NULL);
	void SetInfoIcon(UINT iIcon);
	void ExitWindow(int iRes);
	void GetInfoWndSize(CSize& size);
	void SetCallback(OnButtonCallback func, void* param);
	void LoadMenuItemsForFile();
	void SaveMenuItemsForFile();
	void SetOkText(const char* szOk);
	void SetText(const char* szText);
	void SetButtonsVisibility(BOOL bOK=TRUE);
	void SetAutoCloseTime(DWORD dwMilliseconds);
	void Cleanup();
	void RunTimer(DWORD dwTimeout);
	void StartTimer(DWORD dwMilliseconds);
	void Init(InfoWndStruct* pInfo);
	~InfoWnd();
	InfoWnd(InfoWndStruct* pInfo);
	InfoWnd(InfoWndStruct* pInfo, CWnd* pParent);
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
	afx_msg void OnInfowndOpen();
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

InfoWnd* Alert(InfoWndStruct* pInfo);
InfoWnd* Alert(const char* szText);
InfoWnd* Alert(const char* szText,DWORD dwTime, BOOL bStartHided=FALSE, BOOL bStaticPars=-1);
InfoWnd* Alert(const char* szText, const char* szTitle, DWORD dwTimeOut=0);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_DLG_INFOWINDOW_H__371CD9EC_22B0_4D2E_B199_76446DFE733A__INCLUDED_)
