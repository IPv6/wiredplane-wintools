#ifndef _MENU_TOOLTIP
#define _MENU_TOOLTIP

#ifndef TTS_NOANIMATE
	#define TTS_NOANIMATE           0x10
	#define TTS_NOFADE              0x20
	#define TTS_BALLOON             0x40
	#define TTS_CLOSE               0x80

	#define TTM_SETTITLEA           (WM_USER + 32)  // wParam = TTI_*, lParam = char* szTitle
	#define TTM_SETTITLEW           (WM_USER + 33)  // wParam = TTI_*, lParam = wchar* szTitle

	#ifdef UNICODE
	#define TTM_SETTITLE            TTM_SETTITLEW
	#else
	#define TTM_SETTITLE            TTM_SETTITLEA
	#endif
#endif


class CMenuToolTip
{

  public:
	BOOL lWasLClick;
	BOOL bCreated;
	BOOL bActivated;
	HANDLE hStopTrackingEvent;
	CRITICAL_SECTION cs;
	CMenuToolTip():m_hToolTip(0), m_hParent(0)
	{
		lWasLClick=0;
		bCreated=0;
		bActivated=0;
		bAutoTrack=0;
		hStopTrackingEvent=CreateEvent(0,0,0,0);
		InitializeCriticalSection(&cs);
	}
	
	BOOL& IsWasLclick(){return lWasLClick;};
	BOOL bAutoTrack;
	void SetAutoTrack(BOOL bSet){bAutoTrack=bSet;};
	// Create the Tooltip to associate the text
	void Create(HWND hParent, LPCTSTR sczTipText, 
			HINSTANCE hInstance = NULL, 
			DWORD dwStyle = 0,
			LPCTSTR sczTipTitle = NULL,
			DWORD dwTTStyle = TTF_IDISHWND | TTF_TRACK);//TTF_CENTERTIP//TTF_TRANSPARENT//TTF_ABSOLUTE
	
	// Call this funtions on WM_MENUSELECT of menu owner windw
	void OnMenuSelect(const char* szText, DWORD nItemID, UINT nFlags, HMENU hSubMenu);

	// Поддержка автотрекинга
	static DWORD WINAPI TrackOff(LPVOID pData)
	{
		static POINT pt;
		GetCursorPos(&pt);
		static CMenuToolTip* pThis=(CMenuToolTip*)pData;
		BOOL bTrackTupe=pThis->bAutoTrack;
		if(pThis && pThis->bAutoTrack){
			ResetEvent(pThis->hStopTrackingEvent);
			EnterCriticalSection(&pThis->cs);
			static HWND hThisToTrack=HWND(*pThis);
			static HWND hForegroundToTrack=WindowFromPoint(pt);
			if(hForegroundToTrack==hThisToTrack){
				static RECT rt;
				GetWindowRect(hForegroundToTrack,&rt);
				pt.x=rt.left-1;
				pt.y=rt.top-1;
				hForegroundToTrack=WindowFromPoint(pt);
			}
			while(1){
				BOOL bNeedToHide=0;
				GetCursorPos(&pt);
				HWND hCurActiveWindow=WindowFromPoint(pt);
				HWND hCurActiveWindow2=::GetForegroundWindow();
				if(bTrackTupe==1){
					static char z[100]={0};
					GetClassName(hCurActiveWindow,z,100);
					BOOL bOkByClass=(stricmp(z,"tooltips_class32")==0 || stricmp(z,"SHELLDLL_DefView")==0 || stricmp(z,"internet explorer_server")==0 || stricmp(z,"syslistview32")==0 || stricmp(z,"deskmover")==0);
					if(!pThis){
						break;
					}
					bNeedToHide=!bOkByClass;
				}else{
					bNeedToHide=(hCurActiveWindow!=hThisToTrack && hCurActiveWindow!=hForegroundToTrack);
				}
				static HWND hActWndTitleP=(HWND)(-1);
				if(hActWndTitleP!=(HWND)(-1) && hActWndTitleP!=hCurActiveWindow2){
					bNeedToHide=1;
				}
				hActWndTitleP=hCurActiveWindow2;
				if(bNeedToHide){
					// Шлем мессадж
					//char s[128]={0};
					//sprintf(s,"%08X,%08X,%08X",hCurActiveWindow,hThisToTrack,hForegroundToTrack);
					//pThis->UpdateToolTipText(s);
					//AfxMessageBox(z);
					pThis->ShowToolTip(FALSE);
					hActWndTitleP=(HWND)(-1);
					break;
				}
				DWORD dwRes=WaitForSingleObject(pThis->hStopTrackingEvent,500);
				if(dwRes!=WAIT_TIMEOUT){
					break;
				}
			}
			LeaveCriticalSection(&pThis->cs);
		}
		return 0;
	}
	
	static HANDLE& GetATHandle()
	{
		static HANDLE hAutoTrack=0;
		return hAutoTrack;
	}
	
	~CMenuToolTip()
	{
		if(GetATHandle()){
			SetEvent(hStopTrackingEvent);
			//TerminateThread(GetATHandle(),0);
			EnterCriticalSection(&cs);
			LeaveCriticalSection(&cs);
			GetATHandle()=0;
		}
		CloseHandle(hStopTrackingEvent);
	}
	
	// Show or hide tooltip
	void ShowToolTip(BOOL bShow, BOOL bManualShowToo=0)
	{
		static long lRecuresePreventer;
		if(lRecuresePreventer>0){
			return;
		}
		InterlockedIncrement(&lRecuresePreventer);
		if(bActivated!=bShow){
			if(bShow && bAutoTrack && !IsWindowVisible(HWND(*this))){
				// Следим за выходом за пределы текущего активного окна
				if(GetATHandle()){
					SetEvent(hStopTrackingEvent);
					EnterCriticalSection(&cs);
					LeaveCriticalSection(&cs);
					CloseHandle(GetATHandle());
					GetATHandle()=0;
				}
				DWORD dwTID=0;
				GetATHandle()=CreateThread(0,0,TrackOff,this,0,&dwTID);
			}
			TOOLINFO    ti;
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_IDISHWND;
			ti.hwnd   = m_hParent;
			ti.uId    = (UINT)m_hParent;
			::SendMessage(m_hToolTip,TTM_TRACKACTIVATE,(WPARAM)bShow,(LPARAM)&ti);
			/*if(bShow){
				EnableWindow(m_hToolTip,TRUE);
			}*/
		}
		bActivated=bShow;
		if(bManualShowToo){
			::ShowWindow(m_hToolTip,bShow?SW_SHOW:SW_HIDE);
		}
		InterlockedDecrement(&lRecuresePreventer);
	}

	// Set Tip Position
	void SetToolTipPosition(HMENU hMenu, UINT nItemID)
	{
		RECT rt = {0,0,0,0};
		
		// find Item Rectangle and position
		for(int nItem = 0; nItem < ::GetMenuItemCount(hMenu); nItem++) {
			UINT cmd = ::GetMenuItemID(hMenu, nItem);
			if(cmd == nItemID) {
				::GetMenuItemRect(NULL, hMenu, nItem, &rt);
			}
		}
		CRect rtDesktop;
		GetWindowRect(GetDesktopWindow(),&rtDesktop);
		if(rt.top>rtDesktop.bottom){
			CPoint pt;
			GetCursorPos(&pt);
			SetToolTipPosition(pt.x+5,pt.y+2);
		}else{
			SetToolTipPosition(rt.right+5,rt.top+2);
		}
	}
	
	void SetToolTipPosition(long lX, long lY)
	{	
		// move position
		::SendMessage(m_hToolTip, TTM_TRACKPOSITION, 0,
						(LPARAM)MAKELPARAM(lX, lY));
		// make it top most
		::SetWindowPos(m_hToolTip, HWND_TOPMOST ,lX,lY,0,0, SWP_NOSIZE|SWP_NOACTIVATE);
	}

	// Update tiptext
	void UpdateToolTipText(LPCTSTR szBuff, HINSTANCE hInstance = 0)
	{

		TOOLINFO    ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND;
		ti.hwnd   = m_hParent;
		ti.uId    = (UINT)m_hParent;
		ti.hinst = hInstance;
		ti.lpszText  = const_cast<LPTSTR>(szBuff);
		::SendMessage(m_hToolTip,TTM_UPDATETIPTEXT,(WPARAM)0,(LPARAM)&ti);
	}

	operator HWND()
	{
		return m_hToolTip;
	}

	CString sLastTooltipTitle;
	void UpdateToolTipTitle(LPCTSTR sczTipTitle)
	{
		if(sczTipTitle) {
			sLastTooltipTitle=sczTipTitle;
			::SendMessage(m_hToolTip, TTM_SETTITLE, 1, (LPARAM)sczTipTitle);
		}
	}

  private:
		HWND m_hToolTip;
		HWND m_hParent;
		TCHAR m_szDefault[_MAX_PATH] ;
		HINSTANCE m_hInstance;
};

inline
void CMenuToolTip::Create(HWND hParent, LPCTSTR sczTipText, HINSTANCE hInstance, 
							DWORD dwStyle, LPCTSTR sczTipTitle, DWORD dwStyleTT)
{
	INITCOMMONCONTROLSEX icex;
	TOOLINFO    ti;
	// Load the ToolTip class from the DLL.
	icex.dwSize = sizeof(icex);
	icex.dwICC  = ICC_BAR_CLASSES;

	if(!InitCommonControlsEx(&icex))
	   return;

	m_hParent = hParent;
	m_hInstance = hInstance;
	   
	// Create the ToolTip control.
	if((dwStyle & WS_CHILD)==0){
		dwStyle|=WS_POPUP;
	}
	m_hToolTip = ::CreateWindow(TOOLTIPS_CLASS, TEXT("WP_TOOLTIP"), dwStyle,
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  NULL, (HMENU)NULL, hInstance,
						  NULL);

	// Prepare TOOLINFO structure for use as tracking ToolTip.
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = dwStyleTT; //TTF_IDISHWND | TTF_TRACK ;//TTF_CENTERTIP//TTF_TRANSPARENT//TTF_ABSOLUTE
	ti.hwnd   = hParent;
	ti.uId    = (UINT)hParent;
	ti.hinst  = hInstance;
	ti.lpszText  = const_cast<LPTSTR>(sczTipText);
	if(sczTipText != LPSTR_TEXTCALLBACK) {
		if(sczTipText) {
			_tcscpy(m_szDefault, sczTipText);
		}
		else {
			_tcscpy(m_szDefault, _T("No Text associated"));
		}
	}

	ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

	// Add the tool to the control, displaying an error if needed.
	::SendMessage(m_hToolTip,TTM_ADDTOOL,0,(LPARAM)&ti);

	::SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, 300);
	if(sczTipTitle) {
		sLastTooltipTitle=sczTipTitle;
	}
	if(sLastTooltipTitle!=""){
		::SendMessage(m_hToolTip, TTM_SETTITLE, 1, (LPARAM)(const char*)sLastTooltipTitle);
	}
	bCreated=TRUE;
}

#define MF_SHOWATCURSOR 0x80000000L
inline 
void CMenuToolTip::OnMenuSelect(const char* szText, DWORD nItemID, UINT nFlags, HMENU hSubMenu)
{
	if(nFlags & MF_POPUP 
		|| (nFlags == 0xFFFF && hSubMenu == NULL)) {	// menu closing 
		ShowToolTip(FALSE);
	}

	if(!(nFlags & MF_POPUP)) {

		// Set the position
		if(nFlags & MF_SHOWATCURSOR){
			CPoint pt;
			GetCursorPos(&pt);
			SetToolTipPosition(pt.x+5,pt.y+2);
		}else{
			SetToolTipPosition(hSubMenu, nItemID);
		}	

		// Upodate the text of the tool tip
		if(szText && szText[0] != 0) {
			UpdateToolTipText(szText);
		}else{
			UpdateToolTipText(m_szDefault);
		}
		ShowToolTip(TRUE);
	}
}

#endif _MENU_TOOLTIP