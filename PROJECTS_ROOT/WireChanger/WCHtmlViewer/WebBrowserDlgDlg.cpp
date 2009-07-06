// WebBrowserDlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WebBrowserDlg.h"
#include "WebBrowserDlgDlg.h"
#include "CustomMenus.h"
#include <Mshtml.h>
#include <atlbase.h>
#include <comutil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString g_sID="";
CString g_sLoading="Loading...";
CString g_sFinishUrl="";
extern CWebBrowserDlgApp theApp;
WebContextMenuMode dwDefaultMode=kTextSelectionOnly;
#define WM_GETREADYSTATE	WM_USER+0
#define WM_HANDLEREADYSTATE	WM_USER+1
#define WM_HIDEEXPLORER		WM_USER+2
#define WM_SHOWEXPLORER		WM_USER+3
#define WM_SETBGTEXT		WM_USER+4
#define WM_GETURL			WM_USER+5
/////////////////////////////////////////////////////////////////////////////
// CWebBrowserDlgDlg dialog

CWebBrowserDlgDlg::CWebBrowserDlgDlg(CWnd* pParent /*=NULL*/)
: CResizeableDialog(CWebBrowserDlgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebBrowserDlgDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hLookingThread=0;
	iCounter=0;
	dwTimer=0;
}

void CWebBrowserDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizeableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebBrowserDlgDlg)
	DDX_Control(pDX, IDC_EXPLORER1, m_WebBrowser);
	//}}AFX_DATA_MAP
}

const UINT WM_CUSTOM_CONTROLSITE_MSG  = RegisterWindowMessage(_T("CustomControlSiteMsg"));

BEGIN_MESSAGE_MAP(CWebBrowserDlgDlg, CResizeableDialog)
	//{{AFX_MSG_MAP(CWebBrowserDlgDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE( WM_CUSTOM_CONTROLSITE_MSG, OnCustomControlSiteMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserDlgDlg message handlers
CString sOnLoadScript;
long lApproach=0;
long lStopWatch=0;
CWnd* wndMainWindow=0;
DWORD WINAPI CheckOnLoad(LPVOID p)
{
	DWORD dwI=0;
	HWND hMain=(HWND)p;
	HWND hExp=::GetDlgItem(hMain,IDC_EXPLORER1);
	::ShowWindow(hExp,SW_SHOW);
	int iUnhide=0;
	while(IsWindow(hMain)){
		// Ждем чтобы браузер начал грузить
		while(lApproach>0 && IsWindow(hMain) && lStopWatch==0){
			if(SendMessage(hMain,WM_GETREADYSTATE,0,0)==4){
				Sleep(50);
			}else{
				break;
			}
		}
		if(lStopWatch==1 || !IsWindow(hMain)){
			return 1;
		}
		SendMessage(hMain,WM_HIDEEXPLORER,0,0);
		lApproach++;
		BOOL bLoaded=0;
		int iCounterTimer=0;
		while(IsWindow(hMain)){
			CString sCurTime=TimeFormat(COleDateTime::GetCurrentTime(),1,1);
			CString sText=g_sLoading;
			sText.Replace("%PROGRESSTIME",sCurTime);
			SendMessage(hMain,WM_SETBGTEXT,0,(LPARAM)strdup(sText));
			iCounterTimer++;
			Sleep(500);
			if(g_sFinishUrl!=""){
				CString sTargetUrl="";
				SendMessage(hMain,WM_GETURL,(WPARAM)&sTargetUrl,0);
				if(sTargetUrl.CompareNoCase(g_sFinishUrl)==0){
					bLoaded=(SendMessage(hMain,WM_GETREADYSTATE,0,0)==4);
				}
			}else{
				if(SendMessage(hMain,WM_GETREADYSTATE,0,0)==4){
					bLoaded=1;
				}
			}
			if(bLoaded){
				break;
			}
		}
		if(bLoaded){
			iUnhide=SendMessage(hMain,WM_HANDLEREADYSTATE,0,0);
			if(iUnhide){
				SendMessage(hMain,WM_SHOWEXPLORER,0,0);
			}else{
				SendMessage(hMain,WM_HIDEEXPLORER,0,0);
			}
		}
	}
	return 0;
}

BOOL CWebBrowserDlgDlg::OnInitDialog()
{
	CResizeableDialog::OnInitDialog();
	wndMainWindow=this;
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	RegisterControl(IDC_EXPLORER1, ATTACH_TO_HORIZONTAL_EDGES | ATTACH_TO_VERTICAL_EDGES);
	CString sCMnuW,sCMnuH;
	CString sScriptix=GetInstringPart("scriptex=\"","\"",GetCommandLine());
	if(sScriptix!=""){
		dwDefaultMode=kTextSelectionOnly;
		// Наш супер скрипт
		CCOMString cont;
		ReadFile(sScriptix,cont);
		char szDisk[MAX_PATH],szDir[MAX_PATH];
		_splitpath(sScriptix,szDisk,szDir,0,0);
		CString sScriptPath=szDisk;
		sScriptPath+=szDir;
		cont.Replace("\r","");
		cont+="\n";
		g_sID=GetInstringPart("WndID=","\n",cont.GetString());
		if(g_sID==""){
			g_sID="WPWebSurf";
		}
		// Позиционируем из реестра. До скрипта так как в нем берем позицию
		CRect rtOut;
		CRegKey hKey;
		hKey.Open(HKEY_CURRENT_USER, "Software\\WiredPlane");
		rtOut.left=atol(GetRegStr(hKey.m_hKey,g_sID+"_left"));
		rtOut.right=atol(GetRegStr(hKey.m_hKey,g_sID+"_right"));
		rtOut.top=atol(GetRegStr(hKey.m_hKey,g_sID+"_top"));
		rtOut.bottom=atol(GetRegStr(hKey.m_hKey,g_sID+"_bottom"));
		if(rtOut.IsRectEmpty() || rtOut.left<0 || rtOut.top<0){
			CRect rtScreen;
			GetWindowRect(&rtScreen);
			rtOut.left=rtScreen.Width()/2-40;
			rtOut.right=rtScreen.Width()/2+40;
			rtOut.top=rtScreen.Height()/2-30;
			rtOut.bottom=rtScreen.Height()/2+30;
		}
		MoveWindow(rtOut,1);	
		CRect rtWebBrowser;
		m_WebBrowser.GetWindowRect(&rtWebBrowser);
		// Заменяем входные параметры
		CStringArray aInputs;
		ConvertToArray(GetCommandLine(),aInputs,' ');
		while(1){
			int iReplaceCount=0;
			for(int iParamCount=0;iParamCount<aInputs.GetSize();iParamCount++){
				CStringArray aInputs2;
				CString sLine=aInputs[iParamCount];
				ConvertToArray(sLine,aInputs2,'=');
				if(aInputs2.GetSize()==2){
					CString sL1=toString("%%%s",aInputs2[0]);
					CString sR=aInputs2[1];
					iReplaceCount+=cont.Replace(sL1,sR);
					sL1=toString("%%unescape(%s)",aInputs2[0]);
					if(cont.Find(sL1)!=-1){
						sR=UnescapeString(aInputs2[1]);
						iReplaceCount+=cont.Replace(sL1,sR);
					}
					CString sL2=toString("%%unescapex(%s)",aInputs2[0]);
					if(cont.Find(sL2)!=-1){
						sR=UnescapeString(aInputs2[1]);
						sR.Replace("\n","\\n");
						iReplaceCount+=cont.Replace(sL2,sR);
					}
				}
			}
			// Заменяем инклюды
			iReplaceCount+=cont.Replace("%BASEPATH",GetAppDir());
			iReplaceCount+=cont.Replace("%THISPATH",sScriptPath);
			iReplaceCount+=cont.Replace("%WNDHEIGHT",toString("%i",rtWebBrowser.Height()));
			iReplaceCount+=cont.Replace("%WNDWIDTH",toString("%i",rtWebBrowser.Width()));
			while(1){
				CString sIncl=GetInstringPart("%include(",")",cont.GetString());
				if(sIncl==""){
					break;
				}
				CCOMString contIn;
				ReadFile(sIncl,contIn);
				iReplaceCount+=cont.Replace(toString("%%include(%s)",sIncl),contIn);
			}
			if(iReplaceCount==0){
				break;
			}
		}
		g_sLoading=GetInstringPart("WndLoading=","\n",cont.GetString());
		g_sLoading.Replace("\\n","\n");
		CString sIcon=GetInstringPart("WndIcon=","\n",cont.GetString());
		if(sIcon!=""){
			DestroyIcon(m_hIcon);
			m_hIcon=(HICON)LoadImage(NULL,sIcon,IMAGE_ICON,0,0,LR_LOADFROMFILE);
			SetIcon(m_hIcon, TRUE);			// Set big icon
			SetIcon(m_hIcon, FALSE);		// Set small icon
		}
		CString sTitle=GetInstringPart("WndTitle=","\n",cont.GetString());
		sTitle.TrimLeft();
		sTitle.TrimRight();
		SetWindowText(sTitle);
		CString sUrl=GetInstringPart("WndUrl=","\n",cont.GetString());
		CString sFinishUrl=GetInstringPart("WndFinishUrl=","\n",cont.GetString());
		g_sFinishUrl=sFinishUrl;
		BOOL bAllowResize=atol(GetInstringPart("WndResize=","\n",cont.GetString()));
		bMiniScroll=200;
		if(!bAllowResize){
			ModifyStyle(WS_SIZEBOX,WS_BORDER);
			ModifyStyle(WS_MINIMIZEBOX|WS_MAXIMIZEBOX,0);
			//bMiniScroll=200;// За пределами окна
		}
		BOOL bAllowToolw=atol(GetInstringPart("WndToolStyle=","\n",cont.GetString()));
		if(bAllowToolw){
			ModifyStyleEx(0,WS_EX_TOOLWINDOW);
		}
		::SetWindowPos(GetSafeHwnd(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
		sOnLoadScript=GetInstringPart("WndOnLoadScriptBegin","WndOnLoadScriptEnd",cont.GetString());
		COleVariant vURL(sUrl, VT_BSTR);
		COleVariant vFlags((long) (navNoHistory|navNoReadFromCache|navNoWriteToCache), VT_I4);
		COleVariant vTargetFrameName((LPCSTR)NULL, VT_BSTR);
		COleSafeArray vPostData;
		COleVariant vHeaders((LPCSTR)NULL, VT_BSTR);
		m_WebBrowser.Navigate2(vURL, vFlags, vTargetFrameName, vPostData, vHeaders);
		DWORD dw=0;
		hLookingThread=CreateThread(0,0,CheckOnLoad,GetSafeHwnd(),0,&dw);
	}else{
		CString sUrl=GetInstringPart("url=\"","\"",GetCommandLine());
		if(sUrl==""){
			sUrl="http://www.wiredplane.com/";
		}else{
			CCOMString cont;
			ReadFile(sUrl,cont);
			CString sTitle=GetInstringPart("<!-- FORMTITLE=","-->",cont.GetString());
			if(sTitle!=""){
				SetWindowText(sTitle);
			}else{
				SetWindowText("Fill this form");
			}
			CString sCMnu=GetInstringPart("<!-- FORMMENU=","-->",cont.GetString());
			if(sCMnu=="ALL"){
				dwDefaultMode=kAllowAllButViewSource;
			}
			if(sCMnu=="NONE"){
				dwDefaultMode=kTextSelectionOnly;
			}
			sCMnuW=GetInstringPart("<!-- FORMW=","-->",cont.GetString());
			sCMnuH=GetInstringPart("<!-- FORMW=","-->",cont.GetString());
		}
		m_WebBrowser.Navigate(sUrl, NULL, NULL, NULL, NULL);
		// Тут возможно поддержим размеры....
		if(sCMnuW=="MAX" || sCMnuH=="MAX" || (sCMnuW=="" && sCMnuH=="")){
			PostMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);
		}else{
			int x=atol(sCMnuW);
			int y=atol(sCMnuH);
			CRect rt(GetDesktopRect());
			CRect rtOut;
			rtOut.left=(rt.left+rt.right)/2-x/2;
			rtOut.right=(rt.left+rt.right)/2+x/2;
			rtOut.top=(rt.top+rt.bottom)/2-y/2;
			rtOut.bottom=(rt.top+rt.bottom)/2+y/2;
			MoveWindow(rtOut,1);
			::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)GetSafeHwnd());
			return FALSE;
		}
	}
	return TRUE;
}

void CWebBrowserDlgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CResizeableDialog::OnPaint();
		//ValidateRect(&rLastGripRect);
		if(iGlobalOffsetX<0){
			CDC* pDC=GetDC();
			CRect rt;
			GetClientRect(&rt);
			rt.DeflateRect(4,4,4,4);
			CBrush br;
			br.CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));
			pDC->FrameRect(&rt,&br);
			rt.DeflateRect(1,1,1,1);
			static DWORD dwLastTick=0;
			static int cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 
			if(dwTimer==0){
				dwTimer=SetTimer(1,500,0);
			}
			CRect rtText(rt);
			rt.top=rt.bottom-cyVScroll;
			CRect rtOrig(rt);
			rt.bottom=rt.top-1;
			if(GetTickCount()-dwLastTick>400){
				dwLastTick=GetTickCount();
			}
			CFont* pOld=pDC->SelectObject(GetFont());
			pDC->SetBkMode(TRANSPARENT);
			pDC->DrawText(sBGText,&rtText,DT_CENTER|DT_WORDBREAK);
			pDC->FillSolidRect(rtOrig.left,rtOrig.top,rtOrig.Width(),rtOrig.Height(),GetSysColor(COLOR_INACTIVECAPTION));
			CRect rtStr(rtOrig);
			rtStr.top+=3;
			rtStr.bottom-=3;
			int iL=rtStr.left+(rtStr.Width()/50*((iCounter%62)-8));
			int iR=rtStr.left+(rtStr.Width()/50*((iCounter%62)+1));
			if(iL<rtStr.left+1){
				iL=rtStr.left+1;
			}
			if(iR>rtStr.right-1){
				iR=rtStr.right-1;
			}
			pDC->FillSolidRect(iL,rtStr.top,iR-iL,rtStr.Height(),GetSysColor(COLOR_ACTIVECAPTION));
			pDC->SelectObject(pOld);
			ReleaseDC(pDC);
		}else if(dwTimer){
			KillTimer(dwTimer);
			dwTimer=0;
		}
	}
}

BOOL CWebBrowserDlgDlg::OnEraseBkgnd( CDC * pDC)
{
	CRect rtOrig;
	GetClientRect(&rtOrig);
	if(iGlobalOffsetX<0){
		pDC->FillSolidRect(rtOrig.left,rtOrig.top,rtOrig.Width(),rtOrig.Height(),GetSysColor(COLOR_BTNFACE));
		return TRUE;
	}
	CRect rtExp;
	m_WebBrowser.GetWindowRect(&rtExp);
	ScreenToClient(&rtExp);
	pDC->FillSolidRect(rtOrig.left,rtOrig.top,rtOrig.Width(),rtExp.top-rtOrig.top,GetSysColor(COLOR_BTNFACE));
	pDC->FillSolidRect(rtOrig.left,rtExp.bottom,rtOrig.Width(),rtOrig.bottom-rtExp.bottom,GetSysColor(COLOR_BTNFACE));
	pDC->FillSolidRect(rtOrig.left,rtOrig.top,rtExp.left-rtOrig.left,rtOrig.Height(),GetSysColor(COLOR_BTNFACE));
	pDC->FillSolidRect(rtExp.right,rtOrig.top,rtOrig.right-rtExp.right,rtOrig.Height(),GetSysColor(COLOR_BTNFACE));

	return TRUE;
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWebBrowserDlgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWebBrowserDlgDlg::OnSelchangeMenuMode() 
{
	//We need m_MenuMode to always reflect the current state of the comboBox
	this->UpdateData(TRUE);
}

LRESULT CWebBrowserDlgDlg::OnCustomControlSiteMsg(WPARAM wParam, LPARAM lParam)
{
	LRESULT hasBeenHandled = FALSE;
	ASSERT((wParam > kCCSN_NoMessage) && (wParam < kCCSN_MessageLimit));
	switch (wParam)
	{
	case kCCSN_CreateSite:
		if (lParam != NULL){
			kCCSN_CreateSiteParams *params = (kCCSN_CreateSiteParams *)lParam;
			if (params->pCtrlCont != NULL){
				params->pSite = new CCustomControlSite(params->pCtrlCont);
				hasBeenHandled = TRUE;
			}
		}
		break;
	case kCCSN_ShowContextMenu:
		if (lParam != NULL){
			WebContextMenuMode menuModeMap[] = {kDefaultMenuSupport, kNoContextMenu, 
				kTextSelectionOnly, kAllowAllButViewSource, kCustomMenuSupport};
			WebContextMenuMode	menuMode = dwDefaultMode;
			/*if ( (m_MenuMode > 0) && (m_MenuMode < sizeof menuModeMap) )
			menuMode = menuModeMap[m_MenuMode];*/
			kCCSN_ShowContextMenuParams *params = (kCCSN_ShowContextMenuParams *)lParam;
			params->result = CustomShowContextMenu(menuMode, params->dwID, params->pptPosition, 
				params->pCommandTarget, params->pDispatchObjectHit);
			hasBeenHandled = TRUE;
		}
		break;
	}
	return hasBeenHandled;
}

BOOL CWebBrowserDlgDlg::PreTranslateMessage(MSG* pMsg) 
{
	//FLOG1("WBMSG_PARENT_PT=%X",pMsg->message);
	UINT message=pMsg->message;
	if(message==WM_TIMER && pMsg->wParam==1){
		iCounter++;
		Invalidate();
		return TRUE;
	}
	if(message==WM_LBUTTONUP || message==WM_LBUTTONDOWN || message==WM_RBUTTONUP || message==WM_RBUTTONDOWN){
		if(iGlobalOffsetX<0){
			return 1;
		}
	}
	return CResizeableDialog::PreTranslateMessage(pMsg);
}

LRESULT CWebBrowserDlgDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//FLOG1("WBMSG_PARENT=%X",message);
	if(message==WM_GETURL){
		LPDISPATCH pHtmlDocDisp = (IHTMLDocument2*)m_WebBrowser.GetDocument();
		if(pHtmlDocDisp){
			IHTMLDocument2* spDoc=0;
			pHtmlDocDisp->QueryInterface (IID_IHTMLDocument2, (void**)&spDoc);
			if(spDoc!=NULL){
				BSTR p;
				spDoc->get_URL(&p);
				CString sTargetUrl(p);
				CString* pOut=(CString*)wParam;
				*pOut=sTargetUrl;
				spDoc->Release();
			}
			pHtmlDocDisp->Release();
		}
		return TRUE;
	}
	if(message==WM_HANDLEREADYSTATE){
		BOOL bUnhide=0;
		//GetDlgItem(IDC_EXPLORER1)->ShowWindow(SW_SHOW);
		LPDISPATCH pHtmlDocDisp = (IHTMLDocument2*)m_WebBrowser.GetDocument();
		if(pHtmlDocDisp){
			IHTMLDocument2* spDoc=0;
			pHtmlDocDisp->QueryInterface (IID_IHTMLDocument2, (void**)&spDoc);
			if(spDoc!=NULL){
				BOOL bSkip=0;
				if(bSkip==0){
					CComQIPtr<IHTMLWindow2> spWin;
					spDoc->get_parentWindow(&spWin);
					if(spWin){
						VARIANT varret;
						CString sScript=sOnLoadScript;
						sScript.Replace("%COUNTER",toString("%i",lApproach));
						CComBSTR Script=sScript;
						CComBSTR ScriptLanguage=L"jscript";
						VariantInit(&varret);
						HRESULT hr2=spWin->execScript(Script,ScriptLanguage,&varret);
						VariantClear(&varret);
						BSTR bsStatus;
						spWin->get_defaultStatus(&bsStatus);
						CString sRes(bsStatus);
						sRes.MakeUpper();
						FLOG1("RESULT_OF_PAGE='%s'",sRes);
						if(sRes.Find("%STOP")!=-1){
							lStopWatch=1;
						}
						if(sRes.Find("%STEP2WATCH")!=-1){
							lStopWatch=2;
						}
						if(sRes.Find("%STARTWATCH")!=-1){
							lStopWatch=0;
						}
						if(sRes.Find("%CLEARFURL")!=-1){
							g_sFinishUrl="";
						}
						if(sRes.Find("%SETFURL")!=-1){
							g_sFinishUrl=GetInstringPart("%SETFURL(",")",sRes);
						}
						bUnhide=0;
						if(sRes.Find("%CONTINUE")==-1){
							bUnhide=1;
						}
						if(sRes.Find("TRUE")!=-1 || sRes.Find("%SUCCESS")!=-1){
							// Заканчиваем - успех
							ExitProcess(1);
						}
						if(sRes.Find("FALSE")!=-1 || sRes.Find("%FAILURE")!=-1){
							if(sRes.Find("%UNREACHABLE")!=-1){
								AfxMessageBox("Sorry, web service is not available at this time\nTry again later");
							}
							// Заканчиваем - неуспех
							ExitProcess(-1);
						}
					}
				}
				spDoc->Release();
			}
			pHtmlDocDisp->Release();
		}
		return bUnhide;
	}
	if(message==WM_GETREADYSTATE){
		if(m_WebBrowser.GetControlUnknown()==0){
			return 0;
		}
		int iRes=m_WebBrowser.GetReadyState();
		/*if(iRes==4){
			// Смотрим на 
			// Возможно скрипт испирирует скорую смену урла
			LPDISPATCH pHtmlDocDisp = (IHTMLDocument2*)m_WebBrowser.GetDocument();
			if(pHtmlDocDisp){
				IHTMLDocument2* spDoc=0;
				pHtmlDocDisp->QueryInterface (IID_IHTMLDocument2, (void**)&spDoc);
				if(spDoc!=NULL){
					BOOL bSkip=0;
					if(bSkip==0){
						CComQIPtr<IHTMLWindow2> spWin;
						spDoc->get_parentWindow(&spWin);
						if(spWin){
							BSTR bsStatus;
							spWin->get_defaultStatus(&bsStatus);
							CString sRes(bsStatus);
							sRes.MakeUpper();
						}
					}
					spDoc->Release();
				}
				pHtmlDocDisp->Release();
			}
		}*/
		return iRes;
	}
	if(message==WM_NCDESTROY || !IsWindow(m_WebBrowser)){
		if(hLookingThread){
			TerminateThread(hLookingThread,0);
			CloseHandle(hLookingThread);
			hLookingThread=0;
		}
	}
	if(message==WM_HIDEEXPLORER){
		iGlobalOffsetX=-1000;
		ApplyLayout();
		return TRUE;
	}
	if(message==WM_SHOWEXPLORER){
		iGlobalOffsetX=0;
		ApplyLayout();
		return TRUE;
	}
	if(message==WM_SETBGTEXT){
		const char* sz=(const char*)lParam;
		sBGText=sz;
		free((LPVOID)sz);
		return TRUE;
	}
	int iRet=CResizeableDialog::WindowProc(message, wParam, lParam);
	if(message==WM_SIZING || message==WM_WINDOWPOSCHANGING){
		CRect rtOut;
		GetWindowRect(&rtOut);
		CRegKey hKey;
		hKey.Open(HKEY_CURRENT_USER, "Software\\WiredPlane");
		if(!rtOut.IsRectEmpty()){
			SetRegStr(hKey.m_hKey,g_sID+"_left",toString("%i",rtOut.left));
			SetRegStr(hKey.m_hKey,g_sID+"_right",toString("%i",rtOut.right));
			SetRegStr(hKey.m_hKey,g_sID+"_top",toString("%i",rtOut.top));
			SetRegStr(hKey.m_hKey,g_sID+"_bottom",toString("%i",rtOut.bottom));
			/*if(wndHidder){
				CRect rtWin;
				GetClientRect(&rtWin);
				ClientToScreen(&rtWin.TopLeft());
				ClientToScreen(&rtWin.BottomRight());
				::SetWindowPos(wndHidder->GetSafeHwnd(),GetSafeHwnd(),rtWin.left,rtWin.top,rtWin.Width(),rtWin.Height(),SWP_NOACTIVATE);
			}*/
		}
	}
	return iRet;
}

