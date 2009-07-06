/////////////////////////////////////////////////////////////////////////////
// SystemTray.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then acknowledgement must be made to the author of this file 
// (in whatever form you wish).
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 

#ifndef _INCLUDED_SYSTEMTRAY_H_
#define _INCLUDED_SYSTEMTRAY_H_

#include <afxtempl.h>
typedef struct _NOTIFYICONDATA2 { 
    DWORD cbSize; 
    HWND hWnd; 
    UINT uID; 
    UINT uFlags; 
    UINT uCallbackMessage; 
    HICON hIcon; 
	TCHAR szTip[128];
	DWORD dwState; 
	DWORD dwStateMask; 
	TCHAR szInfo[256]; 
	union {
		UINT  uTimeout; 
		UINT  uVersion; 
	} DUMMYUNIONNAME;
	TCHAR szInfoTitle[64]; 
	DWORD dwInfoFlags; 
} NOTIFYICONDATA2;

typedef struct _NOTIFYICONDATA0 {
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	CHAR   szTip[64];
} NOTIFYICONDATA0;

/////////////////////////////////////////////////////////////////////////////
// CSystemTray window
#define	ADDFLAGS_LBUTTON	0x0001
#define	ADDFLAGS_DLBHAND	0x0002
#define NIM_ADD         0x00000000
#define NIM_MODIFY      0x00000001
#define NIM_DELETE      0x00000002
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010
#define NIS_HIDDEN      0x00000001
#define NIS_SHAREDICON  0x00000002

typedef CMenu*	(FAR WINAPI *RefreshMenuProc)(void* pData);
typedef void	(FAR WINAPI *OnMoveCallback)();
class CSystemTray : public CWnd
{
// Construction/destruction
public:
	HWND hActiveWndsExceptions;
	DWORD m_dwTargetThread;
	static BOOL bShowErrors;
	long lMenuRefreshing;
	long iTrayMenuOpened;
	static BOOL bUsingShell5;
	void SetMenuFlags(DWORD dwFlags, DWORD dwAdditionalFlags=DWORD(-1));
	BOOL SetTargetThread(DWORD dwTarg);
	DWORD GetMenuFlags();
	DWORD dwDefMenuFlags;
	DWORD dwAdditionalFlags;
	BOOL bAnimationInProcess;
	UINT WM_TASKBARCREATED;
    CSystemTray();
    CSystemTray(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID, CWnd* pTargetWnd = NULL);
    virtual ~CSystemTray();

    DECLARE_DYNAMIC(CSystemTray)

// Operations
public:
	static HWND hLastActiveWindow;
	long* pdwDefTICONAction;
	RefreshMenuProc m_rProc;
	OnMoveCallback m_rMoveProc;
	void* m_pUserData;
	void SetUserMenu(RefreshMenuProc rProc, OnMoveCallback rMoveProc, void* pUserData=NULL);
    BOOL Enabled() {
		return m_bEnabled;
	}
    BOOL Visible() {
		return (m_bHidden==0);
	}

    // Create the tray icon
    int Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID, CWnd* pTargetWnd = NULL, BOOL bStartHided=FALSE);

    // Change or retrieve the Tooltip text
    BOOL    SetTooltipText(LPCTSTR pszTooltipText);
    BOOL    SetTooltipText(UINT nID);
    CString GetTooltipText() const;
	BOOL	PopupBaloonTooltip(CString sTitle, CString sText, UINT uFlags=0, DWORD dwTimeout=9000);

    // Change or retrieve the icon displayed
    BOOL  SetIcon(HICON hIcon, BOOL bSetInList=FALSE);
    BOOL  SetIcon(LPCTSTR lpszIconName);
    BOOL  SetIcon(UINT nIDResource);
    BOOL  SetStandardIcon(LPCTSTR lpIconName);
    BOOL  SetStandardIcon(UINT nIDResource);
    HICON GetIcon() const;
    void  HideIcon();
    void  ShowIcon();
    void  RemoveIcon();
    void  MoveToRight();

    // For icon animation
	int iAnimStart;
	int iAnimStop;
	void  SetAnimationLimits(int iStart, int iStop);
    BOOL  SetIconList(UINT uFirstIconID, UINT uLastIconID=0); 
    BOOL  SetIconList(HICON* pHIconList, UINT nNumIcons); 
    BOOL  Animate(UINT nDelayMilliSeconds, int nNumSeconds = -1);
    BOOL  StepAnimation();
    BOOL  StopAnimation();
	void  ClearIconList();

    // Change menu default item
    void GetMenuDefaultItem(UINT& uItem, BOOL& bByPos);
    BOOL SetMenuDefaultItem(UINT uItem, BOOL bByPos);
	void SetMenuDefaultItem2(UINT uItem);
	BOOL RefreshMenu(CMenu*& pMenuOut, CMenu& mLoadMenu);

    // Change or retrieve the window to send notification messages to
    BOOL  SetNotificationWnd(CWnd* pNotifyWnd);
    CWnd* GetNotificationWnd() const;

    // Default handler for tray notification message
    virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSystemTray)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
    void Initialise();

    BOOL             m_bEnabled;   // does O/S support tray icon?
    BOOL             m_bHidden;    // Has the icon been hidden?
    NOTIFYICONDATA2  m_tnd;

    CArray<HICON, HICON> m_IconList;
    static UINT  m_nIDEvent;
    UINT         m_uIDTimer;
    int          m_nCurrentIcon;
    COleDateTime m_StartTime;
    int          m_nAnimationPeriod;
    UINT         m_DefaultMenuItemID;
	UINT         m_DefaultMenuItemID2;
    BOOL         m_DefaultMenuItemByPos;
	CWnd*        m_pTargetWnd;// Window to activate on DblClick And so on

// Generated message map functions
protected:
	//{{AFX_MSG(CSystemTray)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


#endif

/////////////////////////////////////////////////////////////////////////////
