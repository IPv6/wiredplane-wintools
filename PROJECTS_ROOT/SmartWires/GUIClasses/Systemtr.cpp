/////////////////////////////////////////////////////////////////////////////
// SystemTray.cpp : implementation file
//
// This is a conglomeration of ideas from the MSJ "Webster" application,
// sniffing round the online docs, and from other implementations such
// as PJ Naughter's "CTrayNotifyIcon" (http://indigo.ie/~pjn/ntray.html)
// especially the "CSystemTray::OnTrayNotification" member function.
// Joerg Koenig suggested the icon animation stuff
//
// This class is a light wrapper around the windows system tray stuff. It
// adds an icon to the system tray with the specified ToolTip text and 
// callback notification value, which is sent back to the Parent window.
//
// The tray icon can be instantiated using either the constructor or by
// declaring the object and creating (and displaying) it later on in the
// program. eg.
//
//        CSystemTray m_SystemTray;    // Member variable of some class
//        
//        ... 
//        // in some member function maybe...
//        m_SystemTray.Create(pParentWnd, WM_MY_NOTIFY, "Click here", 
//                          hIcon, nSystemTrayID);
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Copyright (c) 1998.
//
// Updated: 25 Jul 1998 - Added icon animation, and derived class
//                        from CWnd in order to handle messages. (CJM)
//                        (icon animation suggested by Joerg Koenig.
//                        Added API to set default menu item. Code provided
//                        by Enrico Lelina.
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
//
/////////////////////////////////////////////////////////////////////////////
    
#include "stdafx.h"
#include "SystemTr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSystemTray, CWnd)
CWnd* AfxWPGetMainWnd();
UINT CSystemTray::m_nIDEvent = 4567;
CMenu* pDumpTrayMenu=NULL;
/////////////////////////////////////////////////////////////////////////////
// CSystemTray construction/creation/destruction

CSystemTray::CSystemTray()
{
    Initialise();
}

CSystemTray::CSystemTray(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szToolTip, 
                         HICON icon, UINT uID, CWnd* pTargetWnd)
{
    Initialise();
    Create(pParent, uCallbackMessage, szToolTip, icon, uID, pTargetWnd);
}

long dwDefaultWinMessageForDefIconAction=1;
void CSystemTray::Initialise()
{
	hActiveWndsExceptions=0;
	m_dwTargetThread=0;
	lMenuRefreshing=0;
	iTrayMenuOpened=0;
	hLastActiveWindow = NULL;
	WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
    memset(&m_tnd, 0, sizeof(m_tnd));
	dwAdditionalFlags=0;
	dwDefMenuFlags=0;
    m_bEnabled   = FALSE;
    m_bHidden    = FALSE;
    m_uIDTimer   = 0;
    m_DefaultMenuItemID = 0;
	m_DefaultMenuItemID2 = 0;
    m_DefaultMenuItemByPos = TRUE;
	m_pTargetWnd = NULL;
	m_rProc		 = NULL;
	m_pUserData  = NULL;
	m_rMoveProc  = NULL;
	iAnimStart		=0;
	iAnimStop		=-1;
	pdwDefTICONAction = &dwDefaultWinMessageForDefIconAction;
	bAnimationInProcess = FALSE;
}

BOOL CSystemTray::SetTargetThread(DWORD dwTarg)
{
	m_dwTargetThread=dwTarg;
	return TRUE;
}

BOOL CSystemTray::bUsingShell5=-1;
BOOL CSystemTray::bShowErrors=TRUE;
typedef HRESULT (CALLBACK *FPDllGetVersion) (DLLVERSIONINFO *pdvi);
BOOL CSystemTray::Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szToolTip, 
                         HICON icon, UINT uID, CWnd* pTargetWnd, BOOL bStartHided)
{
    // this is only for Windows 95 (or higher)
    VERIFY(m_bEnabled = ( GetVersion() & 0xff ) >= 4);
    if (!m_bEnabled) return FALSE;

    // Make sure Notification window is valid (not needed - CJM)
    // VERIFY(m_bEnabled = (pParent && ::IsWindow(pParent->GetSafeHwnd())));
    // if (!m_bEnabled) return FALSE;
    
    // Make sure we avoid conflict with other messages
    ASSERT(uCallbackMessage >= WM_USER);

	// Window for activation on click
	m_pTargetWnd = pTargetWnd;
    // Create an invisible window
    CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0,0,10,10, NULL, 0);
    // load up the NOTIFYICONDATA structure
	if(bUsingShell5==-1){
		bUsingShell5=0;
		// Определяем версию ShellDLL
		HMODULE hShell=LoadLibrary("Shell32.dll");
		if(hShell){
			FPDllGetVersion fp=(FPDllGetVersion)DBG_GETPROC(hShell,"DllGetVersion");
			if(fp){
				DLLVERSIONINFO dvi;
				memset(&dvi,0,sizeof(dvi));
				dvi.cbSize=sizeof(DLLVERSIONINFO);
				(*fp)(&dvi);
				if(dvi.dwMajorVersion>=5){
					bUsingShell5=1;
				}
			}
			FreeLibrary(hShell);
		}
	}
    m_tnd.cbSize = bUsingShell5?sizeof(NOTIFYICONDATA2):sizeof(NOTIFYICONDATA0);
    m_tnd.hWnd   = pParent? pParent->GetSafeHwnd() : m_hWnd;
    m_tnd.uID    = uID;
    m_tnd.hIcon  = icon;
	m_tnd.dwState= NIS_SHAREDICON;
	m_tnd.dwStateMask=0;
    m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_STATE;
    m_tnd.uCallbackMessage = uCallbackMessage;
	// Tray only supports tooltip text up to 64 characters
    if(_tcslen(szToolTip) <= 63){
		_tcscpy(m_tnd.szTip, szToolTip);
	}else{
		memcpy(m_tnd.szTip, szToolTip,60);
		m_tnd.szTip[60]=m_tnd.szTip[61]=m_tnd.szTip[62]='.';
		m_tnd.szTip[63]=0;
	}

    // Set the tray icon
	m_bHidden = bStartHided;
	if(m_tnd.hIcon!=NULL && !m_bHidden){
		m_bEnabled = Shell_NotifyIcon(NIM_ADD, (PNOTIFYICONDATA)&m_tnd);
#ifdef _DEBUG
		if(m_bEnabled==0 && bShowErrors){
			CString sErr;
			sErr.Format("Failed to show tray icon! (err. 0x%X)",GetLastError());
			AfxMessageBox(sErr);
		}
#endif
	}else{
		m_bEnabled = FALSE;
	}
    return m_bEnabled;
}

void CSystemTray::ClearIconList()
{
	for(int i=0; i<m_IconList.GetSize();i++){
		DestroyIcon(m_IconList.GetAt(i));
	}
    m_IconList.RemoveAll();
}

CSystemTray::~CSystemTray()
{
    RemoveIcon();
	ClearIconList();
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray icon manipulation

void CSystemTray::SetUserMenu(RefreshMenuProc rProc, OnMoveCallback rMoveProc, void* pUserData)
{
	m_rProc=rProc;
	m_rMoveProc=rMoveProc;
	m_pUserData=pUserData;
};

void CSystemTray::MoveToRight()
{
    HideIcon();
    ShowIcon();
};

void CSystemTray::RemoveIcon()
{
    if (!m_bEnabled) return;
	//
    m_tnd.uFlags = 0;
    Shell_NotifyIcon(NIM_DELETE, (PNOTIFYICONDATA)&m_tnd);
    m_bEnabled = FALSE;
	m_bHidden = TRUE;
}

void CSystemTray::HideIcon()
{
    if (m_bEnabled && !m_bHidden){
        m_tnd.uFlags = NIF_ICON;
        Shell_NotifyIcon (NIM_DELETE, (PNOTIFYICONDATA)&m_tnd);
        m_bHidden = TRUE;
    }
}

void CSystemTray::ShowIcon()
{
	if(m_bHidden || !m_bEnabled){
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_bEnabled = Shell_NotifyIcon(NIM_ADD, (PNOTIFYICONDATA)&m_tnd);
		m_bHidden = FALSE;
	}
}

BOOL CSystemTray::SetIcon(HICON hIcon, BOOL bSetInList)
{
	//
	/*if(m_tnd.hIcon){//Для анимации - смертельно
		DestroyIcon(m_tnd.hIcon);
	}*/
	m_tnd.hIcon = hIcon;
	if(bSetInList){
		if(m_IconList.GetSize()>0){
			DestroyIcon(m_IconList.GetAt(0));
			m_IconList.SetAt(0,hIcon);
		}else{
			m_IconList.Add(hIcon);
		}
	}
    if (!m_bEnabled) return FALSE;
	//
    m_tnd.uFlags = NIF_ICON;
	if(!m_bHidden){
		BOOL bRes=Shell_NotifyIcon(NIM_MODIFY, (PNOTIFYICONDATA)&m_tnd);
		return bRes;
	}
	return TRUE;
}

BOOL CSystemTray::SetIcon(LPCTSTR lpszIconName)
{
    HICON hIcon = AfxGetApp()->LoadIcon(lpszIconName);
    return SetIcon(hIcon);
}

BOOL CSystemTray::SetIcon(UINT nIDResource)
{
    HICON hIcon = AfxGetApp()->LoadIcon(nIDResource);
    return SetIcon(hIcon);
}

BOOL CSystemTray::SetStandardIcon(LPCTSTR lpIconName)
{
    HICON hIcon = LoadIcon(NULL, lpIconName);
    return SetIcon(hIcon);
}

BOOL CSystemTray::SetStandardIcon(UINT nIDResource)
{
    HICON hIcon = LoadIcon(NULL, MAKEINTRESOURCE(nIDResource));
    return SetIcon(hIcon);
}
 
HICON CSystemTray::GetIcon() const
{
    return (m_bEnabled)? m_tnd.hIcon : NULL;
}

BOOL CSystemTray::SetIconList(UINT uFirstIconID, UINT uLastIconID) 
{
	if (uFirstIconID > uLastIconID)
        return FALSE;
	UINT uIconArraySize = uLastIconID - uFirstIconID + 1;
	const CWinApp * pApp = AfxGetApp();
    if(!pApp)
		return FALSE;
    ClearIconList();
    try {
		if(uLastIconID==0){
			m_IconList.Add(pApp->LoadIcon(uFirstIconID));
		}else{
			for (UINT i = uFirstIconID; i <= uLastIconID; i++){
				m_IconList.Add(pApp->LoadIcon(i));
			}
		}
    }
    catch (CMemoryException *e)
    {
        e->ReportError();
        e->Delete();
        ClearIconList();
        return FALSE;
    }

    return TRUE;
}

void CSystemTray::SetAnimationLimits(int iStart, int iStop)
{
	iAnimStart=iStart;
	iAnimStop=iStop;
}

BOOL CSystemTray::SetIconList(HICON* pHIconList, UINT nNumIcons)
{
    ClearIconList();
    try {
	    for (UINT i = 0; i < nNumIcons; i++)
		    m_IconList.Add(pHIconList[i]);
    }
    catch (CMemoryException *e)
    {
        e->ReportError();
        e->Delete();
        ClearIconList();
        return FALSE;
    }

    return TRUE;
}

BOOL CSystemTray::Animate(UINT nDelayMilliSeconds, int nNumSeconds /*=-1*/)
{
    StopAnimation();

    m_nCurrentIcon = iAnimStart;
    m_StartTime = COleDateTime::GetCurrentTime();
    m_nAnimationPeriod = nNumSeconds;
	// Setup a timer for the animation
	m_uIDTimer = SetTimer(m_nIDEvent, nDelayMilliSeconds, NULL);
	bAnimationInProcess=(m_uIDTimer!=0);
    return (m_uIDTimer != 0);
}

BOOL CSystemTray::StepAnimation()
{
    if (!m_IconList.GetSize())
        return FALSE;

    m_nCurrentIcon++;
	if(iAnimStop==-1){
		iAnimStop=m_IconList.GetSize()-1;
	}
    if (m_nCurrentIcon > iAnimStop){
		m_nCurrentIcon = iAnimStart;
    }
	if(m_nCurrentIcon < 0 || m_nCurrentIcon>=m_IconList.GetSize() || m_IconList[m_nCurrentIcon]==NULL){
		return FALSE;
	}else{
		HICON iconStep=m_IconList[m_nCurrentIcon];
		return SetIcon(iconStep);
	}
}

BOOL CSystemTray::StopAnimation()
{
    if (!m_uIDTimer || !bAnimationInProcess){
		return FALSE;
    }

    BOOL bResult = KillTimer(m_uIDTimer);
    m_uIDTimer = 0;
    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray tooltip text manipulation

BOOL CSystemTray::SetTooltipText(LPCTSTR pszTip)
{
	//
	if(_tcslen(pszTip) <= 63){
		_tcscpy(m_tnd.szTip, pszTip);
	}else{
		memcpy(m_tnd.szTip, pszTip, 60);
		m_tnd.szTip[60]=m_tnd.szTip[61]=m_tnd.szTip[62]='.';
		m_tnd.szTip[63]=0;
	}
	if (!m_bEnabled) return FALSE;
	//
	m_tnd.uFlags = NIF_TIP;
	if(!m_bHidden){
		return Shell_NotifyIcon(NIM_MODIFY, (PNOTIFYICONDATA)&m_tnd);
	}else{
		return TRUE;
	}
}

BOOL CSystemTray::SetTooltipText(UINT nID)
{
    CString strText;
    VERIFY(strText.LoadString(nID));

    return SetTooltipText(strText);
}

CString CSystemTray::GetTooltipText() const
{
    CString strText;
    if (m_bEnabled)
        strText = m_tnd.szTip;

    return strText;
}

BOOL CSystemTray::PopupBaloonTooltip(CString sTitle, CString sText, UINT uFlags, DWORD dwTimeout)
{
	if(bUsingShell5==0){
		return FALSE;
	}
	if(uFlags==0){
		uFlags=1;//NIIF_INFO;
	}
	if (!m_bEnabled) return FALSE;
	sTitle=sTitle.Left(63);
	sText=sText.Left(255);
	_tcscpy(m_tnd.szInfo, sText);
	_tcscpy(m_tnd.szInfoTitle, sTitle);
	m_tnd.dwInfoFlags=uFlags;
	m_tnd.uTimeout=dwTimeout;
	//
	m_tnd.uFlags = 0x00000010;//NIF_INFO;
	if(!m_bHidden){
		return Shell_NotifyIcon(NIM_MODIFY, (PNOTIFYICONDATA)&m_tnd);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray notification window stuff

BOOL CSystemTray::SetNotificationWnd(CWnd* pWnd)
{
    // Make sure Notification window is valid
    ASSERT(pWnd && ::IsWindow(pWnd->GetSafeHwnd()));
    m_tnd.hWnd = pWnd->GetSafeHwnd();
	if (!m_bEnabled) return FALSE;
	//
    m_tnd.uFlags = 0;
	if(!m_bHidden){
		return Shell_NotifyIcon(NIM_MODIFY, (PNOTIFYICONDATA)&m_tnd);
	}else{
		return TRUE;
	}
}

CWnd* CSystemTray::GetNotificationWnd() const
{
    return CWnd::FromHandle(m_tnd.hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray menu manipulation

void CSystemTray::SetMenuDefaultItem2(UINT uItem)
{
	m_DefaultMenuItemID2=uItem;
}

BOOL CSystemTray::SetMenuDefaultItem(UINT uItem, BOOL bByPos)
{
    m_DefaultMenuItemID = uItem;
    m_DefaultMenuItemByPos = bByPos;
    return TRUE;
}

void CSystemTray::GetMenuDefaultItem(UINT& uItem, BOOL& bByPos)
{
    uItem = m_DefaultMenuItemID;
    bByPos = m_DefaultMenuItemByPos;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemTray message handlers

BEGIN_MESSAGE_MAP(CSystemTray, CWnd)
	//{{AFX_MSG_MAP(CSystemTray)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSystemTray::OnTimer(UINT nIDEvent) 
{
    ASSERT(nIDEvent == m_nIDEvent);

    COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
    COleDateTimeSpan period = CurrentTime - m_StartTime;
    if (m_nAnimationPeriod > 0 && m_nAnimationPeriod < period.GetTotalSeconds())
    {
        StopAnimation();
        return;
    }

    StepAnimation();
}

class CLock
{
	long* m_lVal;
public:
	CLock(long* lVal)
	{
		m_lVal=lVal;
		InterlockedIncrement(m_lVal);
	}
	~CLock()
	{
		InterlockedDecrement(m_lVal);
	}
};

BOOL CSystemTray::RefreshMenu(CMenu*& pMenuOut, CMenu& mLoadMenu) 
{
	if(!m_rProc){
		if (!mLoadMenu.LoadMenu(m_tnd.uID))
			return FALSE;
		if (!(pMenuOut = mLoadMenu.GetSubMenu(0)))
			return FALSE;
	}else if(lMenuRefreshing==0){
		CLock lc(&lMenuRefreshing);
		pMenuOut=m_rProc(m_pUserData);
	}
	return pMenuOut!=NULL?TRUE:FALSE;
}

HWND CSystemTray::hLastActiveWindow=NULL;
LRESULT CSystemTray::OnTrayNotification(UINT wParam, LONG lParam) 
{
    //Return quickly if its not for this tray icon
    if (wParam != m_tnd.uID)
        return 0;
	if(iTrayMenuOpened>0){
		return 0;
	}
	CLock l(&iTrayMenuOpened);
    //
	CWnd* pTarget = m_pTargetWnd;
	CMenu menu;
	CMenu* pSubMenu=NULL;
	if(pTarget==NULL && m_dwTargetThread==0){
		pTarget = AfxWPGetMainWnd();
	}
	// Запоминаем то окно, которое было активно до прихода события
	HWND hPrevActive=::GetForegroundWindow();
	if(hPrevActive!=NULL && hPrevActive!=this->GetSafeHwnd() && hPrevActive!=(pTarget?pTarget->GetSafeHwnd():NULL) && hPrevActive!=hActiveWndsExceptions){
		char szClassName[64]="";
		::GetClassName(hPrevActive,szClassName,sizeof(szClassName));
		if(stricmp(szClassName,"Shell_TrayWnd")!=0){
			hLastActiveWindow=hPrevActive;
		}
	}
	if (LOWORD(lParam) == WM_MOUSEMOVE){
		if(m_rMoveProc){
			m_rMoveProc();
		}
	}else{
		static BOOL bSkipNextRButtonUp=0;
		DWORD dwRightBt=(dwAdditionalFlags & ADDFLAGS_LBUTTON);
		DWORD dwCustomDblHandler=(dwAdditionalFlags & ADDFLAGS_DLBHAND);
		if(dwCustomDblHandler && LOWORD(lParam) == WM_RBUTTONUP){
			// Пытаемся дождаться двойного нажатия на правую кнопку!!!
			DWORD dwStart=GetTickCount();
			while(dwStart+150>GetTickCount()){
				if(GetAsyncKeyState(VK_RBUTTON)<0){
					lParam=WM_RBUTTONDBLCLK;
					bSkipNextRButtonUp=1;
					return 0;
				}
				Sleep(10);
			}
		}
		if(bSkipNextRButtonUp && LOWORD(lParam) == WM_RBUTTONUP){
			bSkipNextRButtonUp=0;
			return 0;
		}
		if (LOWORD(lParam) == WM_RBUTTONUP || (dwRightBt!=0 && DWORD(LOWORD(lParam)) == WM_LBUTTONUP)){
			if(!RefreshMenu(pSubMenu,menu)){
				return 0;
			}
			::SetMenuDefaultItem(pSubMenu->m_hMenu, m_DefaultMenuItemID, m_DefaultMenuItemByPos);
			if(pTarget!=NULL && IsWindow(pTarget->GetSafeHwnd())){
				pTarget->SetForegroundWindow();  
			}
			// Display and track the popup menu
			CPoint pos;
			GetCursorPos(&pos);
			if(pTarget!=NULL){
				BOOL pAnim=FALSE;
				if((dwDefMenuFlags & 0x4000L)!=0){//TPM_NOANIMATION
					SystemParametersInfo(0x1002/*SPI_GETMENUANIMATION*/,0,&pAnim,0);
					if(pAnim){
						BOOL pNoAnim=0;
						SystemParametersInfo(0x1003/*SPI_SETMENUANIMATION*/,0,NULL,0);//SPIF_SENDCHANGE|SPIF_UPDATEINIFILE
					}
				}
				pSubMenu->TrackPopupMenu(dwDefMenuFlags, pos.x, pos.y, pTarget, NULL);
				if((dwDefMenuFlags & 0x4000L)!=0 && pAnim){//TPM_NOANIMATION
					SystemParametersInfo(0x1003/*SPI_SETMENUANIMATION*/,0,&pAnim,0);
				}
				if(::IsWindow(pTarget->GetSafeHwnd())){
					pTarget->PostMessage(WM_NULL, 0, 0);
				}
			}
		}else{
			if(dwCustomDblHandler && (LOWORD(lParam) == WM_LBUTTONDBLCLK || LOWORD(lParam) == WM_RBUTTONDBLCLK || LOWORD(lParam) == WM_MBUTTONDBLCLK)){
				if(pTarget!=NULL){
					pTarget->PostMessage(WM_COMMAND, m_DefaultMenuItemID2, LOWORD(lParam));
				}
			}else if (DWORD(LOWORD(lParam)) == DWORD((*pdwDefTICONAction)?WM_LBUTTONDBLCLK:WM_LBUTTONUP)){
				if(m_dwTargetThread!=0){
					PostThreadMessage(m_dwTargetThread,WM_COMMAND, m_DefaultMenuItemID, m_DefaultMenuItemByPos);
				}else{
					if(!RefreshMenu(pSubMenu,menu)){
						return 0;
					}
					UINT uItem=0;
					if (m_DefaultMenuItemByPos){
						uItem = pSubMenu->GetMenuItemID(m_DefaultMenuItemID);
					}else{
						uItem = m_DefaultMenuItemID;
					}
					if(pTarget!=NULL){
						pTarget->SendMessage(WM_COMMAND, uItem, 0);
						//pTarget->PostMessage(WM_COMMAND, uItem, 0);
					}
				}
			}
		}
	}
	menu.DestroyMenu();
	if(pSubMenu!=NULL && m_rProc){
		delete pSubMenu;
	}
    return 1;
}

LRESULT CSystemTray::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if (message == m_tnd.uCallbackMessage)
        return OnTrayNotification(wParam, lParam);
	if (message==WM_TASKBARCREATED && (m_bEnabled && !m_bHidden)){
		MoveToRight();
		return 0;
	}
	return CWnd::WindowProc(message, wParam, lParam);
}

void CSystemTray::SetMenuFlags(DWORD dwFlags, DWORD dwAddFlags)
{
	dwDefMenuFlags=dwFlags;
	if(int(dwAddFlags)!=-1){
		dwAdditionalFlags=dwAddFlags;
	}
}

DWORD CSystemTray::GetMenuFlags()
{
	return dwDefMenuFlags;
}