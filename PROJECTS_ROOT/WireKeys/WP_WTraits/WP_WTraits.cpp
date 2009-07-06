// WKHelper.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include <tchar.h>
#include <math.h>
#include <atlbase.h>
#include "HookCode.h"
#include "WP_WTraits.h"
#include "DLGOptions.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern HWND hLastFocusWnd;
extern HWND hLastActiveWnd;
extern char g_szStartUpWindowName[256];
extern char	g_szApplications[MAX_APPS][(10+MAX_APP_NAME)];
extern long g_IzWndAttachInProgress;
char g_DefDockTitle[128]={0};
void CheckWindwOnTop(HWND hwndDlg);


WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

void WINAPI SetLastFocusWnd(HWND h)
{
	if(h!=0){
		hLastFocusWnd=h;
	}
}

HWND& WINAPI GetLastFocusWnd()
{
	//FLOG1("Retrieving last focus window: %08x",hLastFocusWnd);
	return hLastFocusWnd;
}

char szRetranslationBuffer[1024]="";
char* _pl(const char* szText)
{
	WKGetPluginContainer()->GetTranslation(szText,szRetranslationBuffer,sizeof(szRetranslationBuffer));
	return szRetranslationBuffer;
}

CWKHekperInfoLocal plugData;
HANDLE hHookerEventDone=NULL;
HANDLE hHookerEventStop=NULL;
HANDLE hPluginStop=0;
WKCallbackInterface* pCallbackInterface=NULL;
DWORD dwHookerThreadInThisDLL=0;
extern HINSTANCE g_hinstDll;
extern CWKHekperInfo hookData;
extern DWORD dwHookerThread;
extern RECT rtScreenSize;
extern UINT msgGetScreenshot;
extern DWORD dwSnapWindowTimeData;
extern HWND hLastNCHittestWnd;
extern DWORD dwLastNCHittestRes;
BOOL IsPointInRect(LONG lX,LONG lY,RECT& rt)
{
	if(lX<rt.left){
		return FALSE;
	}
	if(lY<rt.top){
		return FALSE;
	}
	if(lX>rt.right){
		return FALSE;
	}
	if(lY>rt.bottom){
		return FALSE;
	}
	return TRUE;
}

#define INTERSECT_TYPE_NONE	0x00
#define INTERSECT_TYPE_VERT	0x01
#define INTERSECT_TYPE_HORZ	0x02
BOOL IsRectIntersected(RECT& rect, RECT& rectWith,int iDrebezg)
{
	RECT rt;
	rt.left=rectWith.left-iDrebezg;
	rt.top=rectWith.top-iDrebezg;
	rt.right=rectWith.right+iDrebezg;
	rt.bottom=rectWith.bottom+iDrebezg;
	if(rt.left>rt.right){
		LONG lTmp=rt.left;
		rt.left=rt.right;
		rt.right=lTmp;
	}
	if(rt.top>rt.bottom){
		LONG lTmp=rt.bottom;
		rt.bottom=rt.top;
		rt.top=lTmp;
	}
	int iRes=INTERSECT_TYPE_NONE;
	if((rect.left<rt.right && rect.left>rt.left) || (rect.right<rt.right && rect.right>rt.left)){
		iRes|=INTERSECT_TYPE_VERT;
	}
	if((rect.top<rt.bottom && rect.top>rt.top) || (rect.bottom<rt.bottom && rect.bottom>rt.top)){
		iRes|=INTERSECT_TYPE_HORZ;
	}
	if(IsPointInRect(rect.left,rect.top,rt) || IsPointInRect(rect.right,rect.top,rt) ||	IsPointInRect(rect.left,rect.bottom,rt) || IsPointInRect(rect.right,rect.bottom,rt)){
		iRes|=INTERSECT_TYPE_VERT;
		iRes|=INTERSECT_TYPE_HORZ;
	}
	return iRes;
}

BOOL TestSticknessWithRect(RECT& rect, RECT& rectWith,int iStickness)
{
	int iIsectType=IsRectIntersected(rect,rectWith,iStickness);
	if(iIsectType==INTERSECT_TYPE_NONE){
		return FALSE;
	}
	BOOL bChanged=0;
	int iWidth=(rect.right-rect.left);
	int iHeight=(rect.bottom-rect.top);
	if(iIsectType & INTERSECT_TYPE_HORZ){
		if(fabs(double(rect.left-rectWith.left))<iStickness && rect.left!=rectWith.left){
			bChanged=TRUE;
			rect.left=rectWith.left;
		}
		if(fabs(double(rect.right-rectWith.left))<iStickness && rect.right!=rectWith.left){
			bChanged=TRUE;
			rect.left=rectWith.left-iWidth;
		}
		if(fabs(double(rect.left-rectWith.right))<iStickness && rect.left!=rectWith.right){
			bChanged=TRUE;
			rect.left=rectWith.right;
		}
		if(fabs(double(rect.right-rectWith.right))<iStickness && rect.right!=rectWith.right){
			bChanged=TRUE;
			rect.left=rectWith.right-iWidth;
		}
	}
	if(iIsectType & INTERSECT_TYPE_VERT){
		if(fabs(double(rect.top-rectWith.top))<iStickness && rect.top!=rectWith.top){
			bChanged=TRUE;
			rect.top=rectWith.top;
		}
		if(fabs(double(rect.bottom-rectWith.top))<iStickness && rect.bottom!=rectWith.top){
			bChanged=TRUE;
			rect.top=rectWith.top-iHeight;
		}
		if(fabs(double(rect.top-rectWith.bottom))<iStickness && rect.top!=rectWith.bottom){
			bChanged=TRUE;
			rect.top=rectWith.bottom;
		}
		if(fabs(double(rect.bottom-rectWith.bottom))<iStickness && rect.bottom!=rectWith.bottom){
			bChanged=TRUE;
			rect.top=rectWith.bottom-iHeight;
		}
	}
	if(bChanged){
		rect.right=rect.left+iWidth;
		rect.bottom=rect.top+iHeight;
	}
	return bChanged;
}

#define MAX_ENUMEDWINDOWS	50
int iCurrentRectWindow=0;
RECT rActiveWindowsRect[MAX_ENUMEDWINDOWS];
HWND rActiveWindowsHwnd[MAX_ENUMEDWINDOWS];
BOOL CALLBACK EnumActiveWindowsProc(HWND hwnd, LPARAM lParam)
{
	if(::IsWindowVisible(hwnd) && hwnd!=(HWND)lParam){
		if(!::IsZoomed(hwnd)){
			WINDOWPLACEMENT wndPl;
			memset(&wndPl,0,sizeof(wndPl));
			wndPl.length=sizeof(wndPl);
			if(::GetWindowPlacement(hwnd,&wndPl)){
				if(wndPl.showCmd==SW_SHOWNORMAL){
					if(iCurrentRectWindow<MAX_ENUMEDWINDOWS){
						if(!IsWindowsSystemWnd(hwnd,1) && IsWindowStylesOk(hwnd)){
							::GetWindowRect(hwnd,&rActiveWindowsRect[iCurrentRectWindow]);
							rActiveWindowsHwnd[iCurrentRectWindow]=hwnd;
							iCurrentRectWindow++;
						}
					}else{
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}

void SetCorrectWindowPos(HWND hWin)
{
	if(hWin==NULL || !IsWindow(hWin)){
		return;
	}
	if(IsZoomed(hWin)){
		return;
	}
	int iTriggered=0;
	RECT pos,posBackup;
	BOOL bRes=FALSE;
	HWND hWinWith=NULL;
	memset(rActiveWindowsRect,0,MAX_ENUMEDWINDOWS*sizeof(RECT));
	iCurrentRectWindow=0;
	if(!::GetWindowRect(hWin,&pos) || (pos.left==-32000 && pos.top==-32000)){
		return;
	}
	TRACE2("Snapping window x=%i, y=%i\n",pos.left,pos.top);
	if((DWORD)::GetProp(hWin,"WKP_INVIS")==1){
		return;
	}
	// Есть ли у окна владелец
	BOOL bChild=FALSE;
	DWORD dwStyle=::GetWindowLong(hWin,GWL_STYLE);
	if(::GetParent(hWin)!=NULL || (dwStyle & WS_CHILD)!=0){//::GetAncestor(hWin,GA_PARENT)!=NULL
		bChild=TRUE;
	}
	memcpy(&posBackup,&pos,sizeof(posBackup));
	int iWidth=(pos.right-pos.left);
	int iHeight=(pos.bottom-pos.top);
	if(rtScreenSize.left==0 && rtScreenSize.right==0 && rtScreenSize.top==0 && rtScreenSize.bottom==0){
		::SystemParametersInfo(SPI_GETWORKAREA,0,&rtScreenSize,0);
	}
	if(iWidth>0 && iHeight>0 && (iWidth<rtScreenSize.right-rtScreenSize.left) && (iHeight<rtScreenSize.bottom-rtScreenSize.top)){
		if(!bChild && (hookData.bSnapToScreen && hookData.iSnapToScreenStickness>0)){
			BOOL bSingleRes=TestSticknessWithRect(pos,rtScreenSize,hookData.iSnapToScreenStickness);
			if(bSingleRes){
				iTriggered=2;
			}
			bRes|=bSingleRes;
		}
		if(hookData.bSnapToScreenForce && ((pos.left<rtScreenSize.left)||(pos.top<rtScreenSize.top)||(pos.right>rtScreenSize.right)||(pos.bottom>rtScreenSize.bottom))){
			// Принудительно вертаем внутрь экрана
			BOOL bScreenRes=FALSE;
			// Сдвигаем только если окно помещается внутрь монитора обоими краями
			if(pos.left<rtScreenSize.left){
				bScreenRes|=TRUE;
				iTriggered=3;
				pos.left=rtScreenSize.left;
			}
			if(pos.top<rtScreenSize.top){
				bScreenRes|=TRUE;
				iTriggered=4;
				pos.top=rtScreenSize.top;
			}
			if(pos.right>rtScreenSize.right){
				bScreenRes|=TRUE;
				iTriggered=5;
				pos.left=rtScreenSize.right-iWidth;
			}
			if(pos.bottom>rtScreenSize.bottom){
				bScreenRes|=TRUE;
				iTriggered=6;
				pos.top=rtScreenSize.bottom-iHeight;
			}
			if(bScreenRes){
				pos.right=pos.left+iWidth;
				pos.bottom=pos.top+iHeight;
			}
			bRes|=bScreenRes;
		}
	}
	if(!bChild && hookData.bSnapMovements && hookData.iStickness>0){
		// Не чаще раза в 0.5 секунды
		if(GetTickCount()-dwSnapWindowTimeData>500){
			dwSnapWindowTimeData=GetTickCount();
			::EnumWindows(EnumActiveWindowsProc,LPARAM(hWin));
			for(int i=0;i<iCurrentRectWindow;i++){
				if(rActiveWindowsRect[i].left!=rtScreenSize.left
					|| rActiveWindowsRect[i].right!=rtScreenSize.right
					|| rActiveWindowsRect[i].top!=rtScreenSize.top
					|| rActiveWindowsRect[i].bottom!=rtScreenSize.bottom){
					BOOL bSingleRes=TestSticknessWithRect(pos,rActiveWindowsRect[i],hookData.iStickness);
					bRes|=bSingleRes;
					// Test
					if(bSingleRes){
						iTriggered=1;
						hWinWith=rActiveWindowsHwnd[i];
						// ::SetWindowPos(hWinWith,hWin,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
						if(GetAsyncKeyState(VK_RWIN)<0){
							HDC dcScreen=::CreateDC("DISPLAY", NULL, NULL, NULL);
							::FrameRect(dcScreen,&rActiveWindowsRect[i],(HBRUSH)::GetStockObject(BLACK_BRUSH));
							::DeleteDC(dcScreen);
						}
					}
				}
			}
		}
	}
	if(bRes){
		// Смотрим что ничего не изменилось
		RECT pos2;
		::GetWindowRect(hWin,&pos2);
		if((pos.right-pos.left)!=(pos2.right-pos2.left)){
			return;
		}
		if((pos.bottom-pos.top)!=(pos2.bottom-pos2.top)){
			return;
		}
		if(IsZoomed(hWin)){
			return;
		}
		::SetWindowPos(hWin,NULL,pos.left,pos.top,pos.right-pos.left,pos.bottom-pos.top,SWP_NOZORDER|SWP_ASYNCWINDOWPOS|SWP_NOACTIVATE);
	}
}

bool PatternMatch(const char* s, const char* mask)
{
	const   char* cp=0;
	const   char* mp=0;
	for (; *s&&*mask!='*'; mask++,s++){
		if (*mask!=*s && *mask!='?'){
			return 0;
		}
	}
	for (;;) {
		if (!*s){
			while (*mask=='*'){
				mask++;
			}
			return !*mask;
		}
		if (*mask=='*'){ 
			if (!*++mask){
				return 1;
			}
			mp=mask;
			cp=s+1;
			continue;
		}
		if (*mask==*s||*mask=='?'){
			mask++;
			s++;
			continue;
		}
		mask=mp;
		s=cp++;
	}
	return true;
}

void SkipBlanks(const char*& szPos)
{
	while(*szPos==' ' || *szPos=='\t' || *szPos=='\r' || *szPos=='\n'){
		szPos++;
	}
}

bool PatternMatchList(const char* s, const char* mask, int& iCurrentAction)
{
	if(mask==NULL || *mask==NULL){
		return false;
	}
	if(s==NULL || *s==NULL){
		return false;
	}
	const char* szPos=mask;
	SkipBlanks(szPos);
	if(szPos==NULL || *szPos==NULL){
		return false;
	}
	int iDefAction=iCurrentAction;
	while(szPos!=NULL && *szPos!=0){
		SkipBlanks(szPos);
		iCurrentAction=iDefAction;
		const char* szPos2=strpbrk(szPos,"\r\n");
		if(szPos2==NULL){
			szPos2=szPos+(strlen(szPos));
		}
		if(szPos2<=szPos){
			break;
		}
		char szMask[256];
		memset(szMask,0,sizeof(szMask));
		memcpy(szMask,szPos,size_t(szPos2-szPos));
		if(szMask[0]!=0){
			const char* szPattern=szMask;
			if(memicmp(szMask,"Floater:",8)==0){
				iCurrentAction=9;
				szPattern=szMask+8;
			}
			if(memicmp(szMask,"Minimize:",9)==0){
				iCurrentAction=5;
				szPattern=szMask+9;
			}
			if(memicmp(szMask,"Rollup:",7)==0){
				iCurrentAction=WKGetPluginContainer()->getOption(-1)?7:8;
				szPattern=szMask+7;
			}
			if(memicmp(szMask,"Hide:",5)==0){
				iCurrentAction=3;
				szPattern=szMask+5;
			}
			if(memicmp(szMask,"Close:",6)==0){
				iCurrentAction=10;
				szPattern=szMask+6;
			}
			if(memicmp(szMask,"Tray:",5)==0){
				iCurrentAction=2;
				szPattern=szMask+5;
			}
			SkipBlanks(szPattern);
			if(PatternMatch(s,szPattern)){
				return true;
			}
		}
		szPos=szPos2;
	}
	return false;
}

bool PatternMatchList2(const char* s, const char* mask)
{
	if(mask==NULL || *mask==NULL){
		return false;
	}
	if(s==NULL || *s==NULL){
		return false;
	}
	const char* szPos=mask;
	SkipBlanks(szPos);
	if(szPos==NULL || *szPos==NULL){
		return false;
	}
	while(szPos!=NULL && *szPos!=0){
		SkipBlanks(szPos);
		const char* szPos2=strpbrk(szPos,";");
		if(szPos2==NULL){
			szPos2=szPos+(strlen(szPos));
		}
		if(szPos2<=szPos){
			break;
		}
		char szMask[256];
		memset(szMask,0,sizeof(szMask));
		memcpy(szMask,szPos,size_t(szPos2-szPos));
		if(szMask[0]!=0){
			const char* szPattern=szMask;
			SkipBlanks(szPattern);
			if(PatternMatch(s,szPattern)){
				return true;
			}
		}
		szPos=szPos2;
		if(*szPos==';'){
			szPos++;
		}
	}
	return false;
}

BOOL IsWindowInChain(HWND hChild, HWND hTest)
{
	if(hChild==0){
		return 0;
	}
	while(hTest!=hChild){
		HWND hPre=::GetWindow(hChild,GW_OWNER);
		if(hPre==0){
			hPre=::GetParent(hChild);
		}
		if(hPre==0){
			return 0;
		}
		hChild=hPre;
	}
	return 1;
}

// Forward declaration - from <winuser.h>
#ifndef STATE_SYSTEM_OFFSCREEN
#define CCHILDREN_TITLEBAR              5
#define STATE_SYSTEM_UNAVAILABLE        0x00000001  // Disabled
#define STATE_SYSTEM_INVISIBLE          0x00008000
#define STATE_SYSTEM_OFFSCREEN          0x00010000
typedef struct tagTITLEBARINFO
{
    DWORD cbSize;
    RECT  rcTitleBar;
    DWORD rgstate[CCHILDREN_TITLEBAR+1];
} TITLEBARINFO, *PTITLEBARINFO, *LPTITLEBARINFO;
#endif 
typedef BOOL (WINAPI *_GetTitleBarInfo)(HWND hwnd, PTITLEBARINFO pti);
typedef UINT (WINAPI *_RealGetWindowClass)(HWND  hwnd, LPTSTR pszType, UINT  cchType);
HWND hWndNoDups=0;
DWORD WINAPI AutoHideWindow_InThread(LPVOID pData)
{
	TRACETHREAD;
	HWND hWin=(HWND)pData;
	if(hWin==NULL || pCallbackInterface==NULL){
		return 0;
	}
	if(hWndNoDups==hWin || !IsWindow(hWin)){
		return 0;
	}
	hWndNoDups=hWin;
	HINSTANCE hUser32=LoadLibrary("User32.dll");
	// Первый этап - есть кнопка Minimize/Maximize
	TITLEBARINFO iTI;
	memset(&iTI,0,sizeof(iTI));
	iTI.cbSize=sizeof(iTI);
	_GetTitleBarInfo tbi=NULL;
	if(hUser32){
		// Считываем данные о заголовке окна
		tbi=(_GetTitleBarInfo)GetProcAddress(hUser32,"GetTitleBarInfo");
	}
	if(!tbi || (*tbi)(hWin,&iTI)){
		// Смотрим на кнопки...
		if(((iTI.rgstate[2] & STATE_SYSTEM_UNAVAILABLE)==0 && (iTI.rgstate[3] & STATE_SYSTEM_UNAVAILABLE)==0)
			&& ((iTI.rgstate[2] & STATE_SYSTEM_INVISIBLE)==0 && (iTI.rgstate[3] & STATE_SYSTEM_INVISIBLE)==0)
			&& ((iTI.rgstate[2] & STATE_SYSTEM_OFFSCREEN)==0 && (iTI.rgstate[3] & STATE_SYSTEM_OFFSCREEN)==0)){
				// Второй этап - заголовок
				char szTitle[256]="";
				memset(szTitle,0,sizeof(szTitle));
				::GetWindowText(hWin,szTitle,sizeof(szTitle)-1);
				// Смотрим на заголовок
				if(szTitle[0]!=0){
					// Определяем префик действия
					int iCurrentAction=2;
					if(PatternMatchList(szTitle,hookData.szAutoMinimizeTitle,iCurrentAction)){
						if(hookData.iINACTSEC==0){// Ожидание - если пользователь кликнул в трей, то чтоб успело прийти по адресу сообщение
							//Sleep(500);
							WaitForSingleObject(hHookerEventStop,500);
						}else{
							//Sleep(hookData.iINACTSEC*1000);
							WaitForSingleObject(hHookerEventStop,hookData.iINACTSEC*1000);
						}
						if(pCallbackInterface && IsWindow(hWin)){
							// Если приложение все еще не активно, то делаем что мы хотели!
							HWND hCurActiveWnd=::GetForegroundWindow();
							// но! Если активное приложение - чилд нужного, то все равно ничего не делаем!
							if(!IsWindowInChain(hCurActiveWnd,hWin)){
								if(pCallbackInterface && pCallbackInterface->GetWKState()==0){// Если в багдаде все еще спокойно...
									pCallbackInterface->ProcessWindow(hWin,iCurrentAction);
									::SetForegroundWindow(hCurActiveWnd);
								}
							}
						}
					}
				}
			}
	}
	if(hUser32){
		FreeLibrary(hUser32);
	}
	return 0;
}

#include <multimon.h>
/*typedef struct IPv6_tagMONITORINFO
{
    DWORD   cbSize;
    RECT    rcMonitor;
    RECT    rcWork;
    DWORD   dwFlags;
} IPv6_MONITORINFO, *IPv6_LPMONITORINFO;
#define MONITOR_DEFAULTTONEAREST    0x00000002
#if !defined(HMONITOR_DECLARED) && (_WIN32_WINNT < 0x0500)
DECLARE_HANDLE(HMONITOR);
#define HMONITOR_DECLARED
#endif*/
RECT GetScreenRect(RECT* rtBase, BOOL* bMultiMonsOut)
{
	static CRect rtRes(0,0,0,0);
	static BOOL bMultyMons=FALSE;
	if(rtRes.Width()==0 && rtRes.Height()==0){
		if(rtBase!=NULL && GetSystemMetrics(80/*SM_CMONITORS*/)>1){
			MONITORINFO mi;
			memset(&mi,0,sizeof(mi));
			mi.cbSize = sizeof(mi);
			HMONITOR hMonitor=NULL;
			hMonitor = MonitorFromRect(rtBase, MONITOR_DEFAULTTONEAREST);
			if(hMonitor != NULL){
				bMultyMons=TRUE;
				::GetMonitorInfo(hMonitor, &mi);
				rtRes=mi.rcWork;
			}
		}
		if(rtRes.Width()==0 && rtRes.Height()==0){
			bMultyMons=FALSE;
			CRect DesktopDialogRect;
			::SystemParametersInfo(SPI_GETWORKAREA,0,&DesktopDialogRect,0);
			rtRes=DesktopDialogRect;
		}
	}
	if(bMultiMonsOut){
		*bMultiMonsOut=bMultyMons;
	}
	return rtRes;
}

// 10 - вытянуть по горизонтали
// 11 - вытянуть по вертикали
// 12 - приклеить по горизонтали
// 13 - приклеить по вертикали
// 15 - вытянуть ul
// 16 - вытянуть ur
// 17 - вытянуть dl
// 18 - вытянуть dr
// 19 - приклеить ul
// 20 - приклеить ur
// 21 - приклеить dl
// 22 - приклеить dr
// 23 - Закрыть приложение
long g_lSpanWindowAction=0;
long g_lSpanWindowActionDsc=0;
DWORD WINAPI SpanWindow_InThread(LPVOID pData)
{
	TRACETHREAD;
	HWND SpanWindowHwnd=(HWND)pData;
	SimpleTracker lock(g_IzWndAttachInProgress);
	if(SpanWindowHwnd){
		static long lc=0;
		if(lc>0){
			// Уже идет обработка
			// и перемещение окна
			return 0;
		}
		SimpleTracker lock(lc);
		// Ждем пока средняя кнопка мыши не будет отпущена
		while(GetAsyncKeyState(VK_RBUTTON)<0 || GetAsyncKeyState(VK_MBUTTON)<0){
			Sleep(200);
		}
		// Проверяем что мы все еще над границей окна
		POINT curpoint;
		GetCursorPos(&curpoint);
		int lRes=SendMessage(SpanWindowHwnd,WM_NCHITTEST,0,MAKELONG(curpoint.x,curpoint.y));
		if(lRes==0 || !(lRes>=HTLEFT && lRes<=HTBORDER)){
			return 0;
		}
		RECT rt,rtBackup,rtScreen,rtParentOrig;
		rtParentOrig.left=rtParentOrig.top=rtParentOrig.right=rtParentOrig.bottom=0;
		::GetWindowRect(SpanWindowHwnd,&rt);
		rtBackup.left=rt.left;
		rtBackup.top=rt.top;
		rtBackup.right=rt.right;
		rtBackup.bottom=rt.bottom;
		HWND hParent=GetParent(SpanWindowHwnd);
		if(hParent){
			::GetWindowRect(hParent,&rtScreen);
			rtParentOrig.left=rtScreen.left;
			rtParentOrig.top=rtScreen.top;
			rtParentOrig.right=rtScreen.right;
			rtParentOrig.bottom=rtScreen.bottom;
			// Переводим в клиентские координаты
			POINT pt;
			pt.x=rt.left;
			pt.y=rt.top;
			ScreenToClient(hParent,&pt);
			rt.left=pt.x;
			rt.top=pt.y;
			pt.x=rt.right;
			pt.y=rt.bottom;
			ScreenToClient(hParent,&pt);
			rt.right=pt.x;
			rt.bottom=pt.y;
			// Трудный выбор. у Максимизированного приложения - к паренту, иначе - к экрану
			rtScreen.right-=rtScreen.left;
			rtScreen.bottom-=rtScreen.top;
			rtScreen.top=0;
			rtScreen.left=0;
			if(GetWindow(SpanWindowHwnd,GW_OWNER)==0){
				rtParentOrig.left=rtParentOrig.top=rtParentOrig.right=rtParentOrig.bottom=0;
			}else{
				rtScreen=GetScreenRect(&rt,NULL);
				// Перводим координаты
				DWORD dwW=rtScreen.right-rtScreen.left;
				DWORD dwH=rtScreen.bottom-rtScreen.top;
				rtScreen.left=rtScreen.left-rtParentOrig.left;
				rtScreen.top=rtScreen.top-rtParentOrig.top;
				rtScreen.right=rtScreen.left+dwW;
				rtScreen.bottom=rtScreen.top+dwH;
			}
		}else{
			rtScreen=GetScreenRect(&rt,NULL);
		}
		if(g_lSpanWindowAction==10 || g_lSpanWindowAction==12 || g_lSpanWindowAction>=15){
			if(g_lSpanWindowAction==12 || g_lSpanWindowAction>=19){
				if(g_lSpanWindowActionDsc==HTLEFT || g_lSpanWindowAction==15 || g_lSpanWindowAction==17 || g_lSpanWindowAction==19 || g_lSpanWindowAction==21){
					rt.left=rtScreen.left;
					rt.right=rt.left+(rtBackup.right-rtBackup.left);
				}else{
					rt.right=rtScreen.right;
					rt.left=rt.right-(rtBackup.right-rtBackup.left);
				}
			}
			if(g_lSpanWindowAction==10 || (g_lSpanWindowAction>=15 && g_lSpanWindowAction<19)){
				// Читаем свойсвта...
				DWORD dwPropL=(DWORD)::GetProp(SpanWindowHwnd,"WKP_OLDWNDLEFT");
				DWORD dwPropR=(DWORD)::GetProp(SpanWindowHwnd,"WKP_OLDWNDRIGHT");
				if(dwPropL==0 && dwPropR==0){
					::SetProp(SpanWindowHwnd,"WKP_OLDWNDLEFT",(HANDLE)rt.left);
					::SetProp(SpanWindowHwnd,"WKP_OLDWNDRIGHT",(HANDLE)rt.right);
					rt.left=rtScreen.left;
					rt.right=rtScreen.right;
				}else{
					rt.left=dwPropL;
					rt.right=dwPropR;
					::RemoveProp(SpanWindowHwnd,"WKP_OLDWNDLEFT");
					::RemoveProp(SpanWindowHwnd,"WKP_OLDWNDRIGHT");
				}
			}
		}
		if(g_lSpanWindowAction==11 || g_lSpanWindowAction==13 || g_lSpanWindowAction>=15){
			if(g_lSpanWindowAction==13 || g_lSpanWindowAction>=19){
				if(g_lSpanWindowActionDsc==HTTOP || g_lSpanWindowAction==15 || g_lSpanWindowAction==16 || g_lSpanWindowAction==19 || g_lSpanWindowAction==20){
					rt.top=rtScreen.top;
					rt.bottom=rt.top+(rtBackup.bottom-rtBackup.top);
				}else{
					rt.bottom=rtScreen.bottom;
					rt.top=rt.bottom-(rtBackup.bottom-rtBackup.top);
				}
			}else if(g_lSpanWindowAction==11 || (g_lSpanWindowAction>=15 && g_lSpanWindowAction<19)){
				DWORD dwPropT=(DWORD)::GetProp(SpanWindowHwnd,"WKP_OLDWNDTOP");
				DWORD dwPropB=(DWORD)::GetProp(SpanWindowHwnd,"WKP_OLDWNDBOTTOM");
				if(dwPropT==0 && dwPropB==0){
					::SetProp(SpanWindowHwnd,"WKP_OLDWNDTOP",(HANDLE)rt.top);
					::SetProp(SpanWindowHwnd,"WKP_OLDWNDBOTTOM",(HANDLE)rt.bottom);
					rt.top=rtScreen.top;
					rt.bottom=rtScreen.bottom;
				}else{
					rt.top=dwPropT;
					rt.bottom=dwPropB;
					::RemoveProp(SpanWindowHwnd,"WKP_OLDWNDTOP");
					::RemoveProp(SpanWindowHwnd,"WKP_OLDWNDBOTTOM");
				}
			}
		}
		::MoveWindow(SpanWindowHwnd,rt.left+rtParentOrig.left,rt.top+rtParentOrig.top,rt.right-rt.left,rt.bottom-rt.top,TRUE);
	}
	return 0;
}

// WMUSER+...
// 0 - бездействие
// 1 - установить хуки
// 2 - подогнать окно из pHookAction_HWND
// 3 - спрятать в трей
// 4 - проверить и спрятать если сходится паттерн
// 5 - показать WK-menu на активном окне
// 6 - Rollup window
// 7 - спрятать целиком
// 8 - appinfo
// 9 - qrun с окна
// 10 - вытянуть по горизонтали
// 11 - вытянуть по вертикали
// 12 - приклеить по горизонтали
// 13 - приклеить по вертикали
// 14 - snapwindow
// 15 - вытянуть ul
// 16 - вытянуть ur
// 17 - вытянуть dl
// 18 - вытянуть dr
// 19 - приклеить ul
// 20 - приклеить ur
// 21 - приклеить dl
// 22 - приклеить dr
// 23 - создать ярлык
// 24 - убить приложение
// 25 - во флоатер anyway
// 26 - в tray anyway
// 27 - сменить онтоп
// 30 - запаузить/распаузить ВК
// 31 - список рисентофолдеров
// 35 - опдейт рисентов
// wParam=HWND
// lParam=Additional info
#define SAVE_REGKEY	"SOFTWARE\\WiredPlane\\WireKeys\\WKHelper"
CRITICAL_SECTION csMainThread;
DWORD WINAPI GlobalHooker_WKHelper(LPVOID pData)
{
	TRACETHREAD;
	::EnterCriticalSection(&csMainThread);
	// Создаем очередь сообщений
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetEvent(hHookerEventDone);// Чтобы сцепить ожидающих
	while(GetMessage(&msg,0,0,0)>0){
		if(msg.message==WM_CLOSE || (msg.message==WM_ENABLE && msg.wParam==0)){
			dwHookerThread=0;
			BOOL bResult=SetHook(FALSE,0);
		}
		if(msg.message==WM_ENABLE && msg.wParam!=0){
			char szHookLibPath[MAX_PATH]={0};
			WKGetPluginContainer()->GetWKPluginsDirectory(szHookLibPath,MAX_PATH);
			BOOL bResult=SetHook(TRUE,szHookLibPath);
			dwHookerThread=dwHookerThreadInThisDLL;
			/*if(!bResult && pCallbackInterface){
				pCallbackInterface->ShowPopup("WKHelper plugin failed to initialize itself","Plugin error",5000);
			}*/
		}
		if(msg.message==WM_COMMAND){
			if(msg.lParam==0){
				memset(g_szStartUpWindowName,0,sizeof(g_szStartUpWindowName));
				GetWindowText((HWND)msg.wParam,g_szStartUpWindowName,sizeof(g_szStartUpWindowName)-1);
				WKPluginOptions(0);
			}
		}
		if(msg.message>=WM_USER && pCallbackInterface!=NULL){
			if(msg.message-WM_USER==999){
				pCallbackInterface->Sound(0);
			}
			HWND pHookAction_HWND=(HWND)msg.wParam;
			DWORD lHookerDesiredActionDsc=(DWORD)msg.lParam;
			DWORD lHookerDesiredAction=msg.message-WM_USER;
			if(pHookAction_HWND!=NULL){
#ifdef _DEBUG
				/*if(lHookerDesiredAction>=10 && lHookerDesiredAction<=22){
					char s[1000]="";
					sprintf(s,"Приклеиваем окно... %i",lHookerDesiredAction);
					pCallbackInterface->ShowOSD(s,9000);
				}*/
#endif
				/*if(lHookerDesiredAction==10 || lHookerDesiredAction==11 && lCurrentThreadActionInProgress==lHookerDesiredAction){
					continue;
				}*/
				static long lCurrentThreadActionInProgress=lHookerDesiredAction;
				if(lHookerDesiredAction==23){
					pCallbackInterface->ProcessWindow(pHookAction_HWND,11);
					lHookerDesiredAction=0;
				}
				if(lHookerDesiredAction==24){
					pCallbackInterface->ProcessWindow(pHookAction_HWND,12);
					lHookerDesiredAction=0;
				}
				if(lHookerDesiredAction==25){
					pCallbackInterface->ProcessWindow(pHookAction_HWND,14);
					lHookerDesiredAction=0;
				}
				if(lHookerDesiredAction==26){
					pCallbackInterface->ProcessWindow(pHookAction_HWND,15);
					lHookerDesiredAction=0;
				}
				if(lHookerDesiredAction==27){
					pCallbackInterface->ProcessWindow(pHookAction_HWND,1);
					CheckWindwOnTop(pHookAction_HWND);
					lHookerDesiredAction=0;
				}
				if(lHookerDesiredAction==14){
					// Проверяем подходит ли заголовок
					if(strlen(plugData.szExclusions)>0){
						char szTitle[256]="";
						::GetWindowText(pHookAction_HWND,szTitle,sizeof(szTitle));
						if(strlen(szTitle)>0 && PatternMatchList2(szTitle,plugData.szExclusions)){
							lHookerDesiredAction=0;
						}
					}
				}
				if(lHookerDesiredAction==2 || lHookerDesiredAction==6 || lHookerDesiredAction==7){
					// Ждем пока правая кнопка мыши не будет отпущена
					while(GetAsyncKeyState(VK_RBUTTON)<0){
						Sleep(200);
					}
				}
				if(lHookerDesiredAction==2 || lHookerDesiredAction==7){
					pCallbackInterface->ProcessWindow(pHookAction_HWND,(lHookerDesiredAction==7)?3:2);
				}
				if(lHookerDesiredAction==3){
					if(pCallbackInterface->GetWKState()==0){// Если в багдаде все спокойно...
						pCallbackInterface->ShowWKMenu(pHookAction_HWND);
					}
				}
				if(lHookerDesiredAction==14){
					SetCorrectWindowPos(pHookAction_HWND);
				}
				if(lHookerDesiredAction==6){
					pCallbackInterface->ProcessWindow(pHookAction_HWND,WKGetPluginContainer()->getOption(-1)?7:8);
				}
				if(lHookerDesiredAction==8){
					pCallbackInterface->ShowAppInfo(pHookAction_HWND);
				}
				if(lHookerDesiredAction==9){
					pCallbackInterface->AddQrunFromHWND(pHookAction_HWND);
				}
				if(lHookerDesiredAction>=10 && lHookerDesiredAction<=22 && lHookerDesiredAction!=14){
					g_lSpanWindowAction=lHookerDesiredAction;
					g_lSpanWindowActionDsc=lHookerDesiredActionDsc;
					FORK(SpanWindow_InThread,pHookAction_HWND);
				}
				if(lHookerDesiredAction==4){
					if(pCallbackInterface->GetWKState()==0 && pCallbackInterface->GetWKMainWindow()){// Если в багдаде все спокойно...
						FORK(AutoHideWindow_InThread,pHookAction_HWND);
					}
				}
				if(lHookerDesiredAction==30){
					pCallbackInterface->PauseWKCompletely(msg.lParam);
					lHookerDesiredAction=0;
				}
				if(lHookerDesiredAction>=31 && lHookerDesiredAction<=34){
					::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)pHookAction_HWND);
					char sz[128]={0};
					sprintf(sz,"[ID=@PLUGIN-WP_OpenWithExt-%i]",lHookerDesiredAction-31);
					pCallbackInterface->StartQuickRunByNameX(sz,0,0,0);
					lHookerDesiredAction=0;
				}
				if(lHookerDesiredAction==35){
					DWORD dwTID=(DWORD)pCallbackInterface->getOptionEx(1346,0);
					if(dwTID){
						PostThreadMessage(dwTID,WM_TIMECHANGE,0,0);
					}
					lHookerDesiredAction=0;
				}
				lCurrentThreadActionInProgress=0;
			}
		}
		hLastNCHittestWnd=0;
		dwLastNCHittestRes=0;
		SetEvent(hHookerEventDone);
		if(msg.message==WM_CLOSE || msg.message==WM_QUIT){
			break;
		}
	}
	::LeaveCriticalSection(&csMainThread);
	SetEvent(hHookerEventStop);
	return 0;
}

DWORD WINAPI GetThreadHandle()
{
	return dwHookerThreadInThisDLL;
}

void CheckWndForMin(HWND hWind);
void CheckWKPause();
DWORD WINAPI GlobalHooker_WKHelper_Timer(LPVOID)
{
	hLastActiveWnd=0;
	while(WaitForSingleObject(hPluginStop,1000)!=WAIT_OBJECT_0){
		HWND hwndNew=GetForegroundWindow();
		if(hLastActiveWnd!=hwndNew){
			if(WKUtils::isWKUpAndRunning()){
				if(hookData.bCatchAutoMinimize && dwHookerThread>0){
					if(IsWindowVisible(hwndNew)){
						DWORD dwStyle=::GetWindowLong(hLastActiveWnd,GWL_STYLE);
						DWORD dwStyleEx=::GetWindowLong(hLastActiveWnd,GWL_EXSTYLE);
						if(GetAsyncKeyState(VK_SHIFT)>=0 && ::GetParent(hLastActiveWnd)==NULL && (dwStyle & WS_CHILD)==0){//(dwStyleEx & WS_EX_APPWINDOW)!=0 && ::GetAncestor(hwnd,GA_PARENT)==NULL && (dwStyleEx & WS_EX_TOOLWINDOW)==0 && 
							PostThreadMessage(dwHookerThread,WM_USER+4,WPARAM(hLastActiveWnd),0);
						}
					}
				}
				if(hookData.szAutoMinToTray[0]!=0 || hookData.szAutoMinToFloat[0]!=0){
					//char c[100]={0};GetWindowText((HWND)hLastActiveWnd,c,100);
					//char s[100]={0};GetClassName((HWND)hLastActiveWnd,s,100);
					WINDOWPLACEMENT pl;
					memset(&pl,0,sizeof(pl));
					pl.length=sizeof(pl);
					if(::GetWindowPlacement(hLastActiveWnd,&pl) && pl.showCmd==SW_SHOWMINIMIZED){
						CheckWndForMin(hLastActiveWnd);
					}
				}
			}
			CheckWKPause();
			hLastActiveWnd=hwndNew;
		}
	}
	return 0;
}

extern char szItem_ToTray[64];
extern char szItem_Compl[64];
extern char szItem_AddMenu[64];
extern char szItem_FavMenu[64];
extern char	szItem_OnTopState[64];
extern char szItem_ToTitle[64];
extern char szItem_Info[64];
extern char szItem_QRun[64];
extern char szItem_SCut[64];
extern char szItem_Kill[64];
extern HWND g_WKWnd;
HANDLE hHookerThread=0;
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	if(init){
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
	pCallbackInterface=init;
	strcpy(szItem_ToTray,_pl("Hide to tray"));
	strcpy(szItem_Compl,_pl("Hide completely"));
	strcpy(szItem_AddMenu,_pl("Additional menu"));
	strcpy(szItem_FavMenu,_pl("Recent folders"));
	strcpy(szItem_OnTopState,_pl("Change 'on top'"));
	strcpy(szItem_ToTitle,_pl("Rollup to title"));
	strcpy(szItem_Info,_pl("Application info"));
	strcpy(szItem_QRun,_pl("Add to quick-run"));
	strcpy(szItem_SCut,_pl("Create shortcut"));
	strcpy(szItem_Kill,_pl("Kill application"));
	strcat(szItem_AddMenu,"\t...");
	strcat(szItem_FavMenu,"\t...");
	// Screenshoting...
	msgGetScreenshot=RegisterWindowMessage("WK_GETSNAP");
	// Working thread...
	g_WKWnd=init->GetWKMainWindow();
	DWORD dwThread=0,dwHookerThreadInThisDLL2=0;
	hHookerEventDone=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerEventStop=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hPluginStop=::CreateEvent(NULL,FALSE,FALSE,NULL);
	hHookerThread=::CreateThread(NULL, 0, GlobalHooker_WKHelper, NULL, 0, &dwHookerThreadInThisDLL);
	HANDLE hHookerThread2=::CreateThread(NULL, 0, GlobalHooker_WKHelper_Timer, NULL, 0, &dwHookerThreadInThisDLL2);
	CloseHandle(hHookerThread2);
	WaitForSingleObject(hHookerEventDone,INFINITE);
	PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,1,0);
	return 1;
}

extern HHOOK g_hhook1;
extern HHOOK g_hhook2;
extern HHOOK g_hhook3;
extern HHOOK g_hhook4;
void SetDefValues()
{
	hookData.iStickness=30;
	hookData.bSnapMovements=0;
	hookData.bCatchRollDock=1;
	hookData.bCatchRollExpand=1;
	hookData.bSnapToScreen=0;
	hookData.iSnapToScreenStickness=15;
	hookData.iINACTSEC=1;
	hookData.bInjectItems=1;
	hookData.bOntopicon=0;
	hookData.bSnapToScreenForce=0;
	hookData.bInjectIt1=1;
	hookData.bInjectIt2=0;
	hookData.bInjectIt3=0;
	hookData.bInjectIt4=1;
	hookData.bInjectIt5=1;
	hookData.bInjectIt6=0;
	hookData.bInjectIt7=1;
	hookData.bBlockFocuStealing=0;
}

int	WINAPI WKPluginStart()
{
	::InitializeCriticalSection(&csMainThread);
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rtScreenSize,0);
	g_hhook1 = NULL;
	g_hhook2 = NULL;
	g_hhook3 = NULL;
	g_hhook4 = NULL;
	memset(&plugData,0,sizeof(plugData));
	memset(&hookData,0,sizeof(hookData));
	memset(g_szApplications,0,sizeof(g_szApplications[0])*MAX_APPS);
	SetDefValues();
	/*// Reading from registry...
	CRegKey key;
	if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)==ERROR_SUCCESS && key.m_hKey!=NULL){
		DWORD lSize = sizeof(hookData),dwType=0;
		RegQueryValueEx(key.m_hKey,"WKHelperOptions",NULL, &dwType,(LPBYTE)(&hookData), &lSize);
	}*/
	return 1;
}

BOOL SaveToReg()
{
	/*if(!isMemEmpty(&hookData,sizeof(hookData))){
		// Saving to registry...
		CRegKey key;
		if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)!=ERROR_SUCCESS){
			key.Create(HKEY_CURRENT_USER, SAVE_REGKEY);
		}
		if(key.m_hKey!=NULL){
			RegSetValueEx(key.m_hKey,"WKHelperOptions",0,REG_BINARY,(BYTE*)(&hookData),sizeof(hookData));
		}
	}*/
	return TRUE;
}

int	WINAPI WKPluginStop()
{
	SaveToReg();
	dwHookerThread=0;
	SetEvent(hPluginStop);
	//----------------------
	if(!pCallbackInterface->getOption(WKOPT_ISSHUTDOWN)){
		PostThreadMessage(dwHookerThreadInThisDLL,WM_CLOSE,0,0);
		::WaitForSingleObject(hHookerEventStop,2000);
		//::EnterCriticalSection(&csMainThread);
		//::LeaveCriticalSection(&csMainThread);
	}
	DWORD dwTRes=0;
	if(GetExitCodeThread(hHookerThread,&dwTRes) && dwTRes==STILL_ACTIVE){
		//TerminateThread(hHookerThread,66);
		//SuspendThread(hHookerThread);
	}
	::CloseHandle(hHookerEventDone);
	::CloseHandle(hHookerEventStop);
	::CloseHandle(hHookerThread);
	::DeleteCriticalSection(&csMainThread);
	pCallbackInterface=0;
	return 1;
}

int	WINAPI WKDesktopSwitching(BOOL bPaused)
{
	if(bPaused){
		PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,0,0);
	}else{
		PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,1,0);
	}
	return 1;
}


int	WINAPI _WKPluginPause(BOOL bPaused)
{
	if(bPaused){
		PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,0,0);
	}else{
		PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,1,0);
	}
	::WaitForSingleObject(hHookerEventDone,500);
	return 1;
}

extern HWND hOpenedOptions;
DWORD WINAPI CallOptions(LPVOID pData)
{
	static BOOL bInProc=0;
	if(bInProc){
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hOpenedOptions);
		return 0;
	}
	bInProc++;
	HWND hParent=(HWND)pData;
	PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,0,0);
	::WaitForSingleObject(hHookerEventDone,500);
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPTIONS),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	SaveToReg();
	PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,1,0);
	::WaitForSingleObject(hHookerEventDone,500);
	// Pushing changed options back into WireKeys
	if(pCallbackInterface){
		pCallbackInterface->PushMyOptions(0);
	}
	bInProc--;
	return 1;
}

HFONT hfCreated=0;
HWND hOpenedOptions2=0;
int CALLBACK OptionsDialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		PostMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hwndDlg);
		hOpenedOptions=hwndDlg;
		// Локализация
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC99),_pl("Written by Ilja Razinkov @2008"));
		::SetWindowText(hwndDlg,_pl("True docking"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),"");
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_UP),_pl("Move up"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_DOWN),_pl("Move down"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_REM),_pl("Remove"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_BT_ADD),_pl("Add"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATIC_M),_pl("Here you can type window`s titles. Noted windows will be docked to the specified screen edges. To type title mask, use '*' as a wildcard for partial matches"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_CHECK_L),_pl("Left"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_CHECK_R),_pl("Right"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_CHECK_T),_pl("Top"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_CHECK_B),_pl("Bottom"));
		::SetWindowText(GetDlgItem(hwndDlg,IDOPTIONS),_pl("Options"));
		
		if(g_DefDockTitle[0]!=0){
			::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),g_DefDockTitle);
		}
		g_DefDockTitle[0]=0;
		LOGFONT lg;
		HFONT hf=(HFONT)::SendMessage(GetDlgItem(hwndDlg,IDC_LIST),WM_GETFONT,0,0);
		::GetObject(hf,sizeof(lg),&lg);
		strcpy(lg.lfFaceName,"Courier");
		hfCreated=CreateFontIndirect(&lg);
		SendMessage(GetDlgItem(hwndDlg,IDC_LIST),WM_SETFONT,WPARAM(hfCreated),0);
		int iCI=0;
		while(g_szApplications[iCI][0]!=0 && iCI<MAX_APPS){
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_ADDSTRING,0,(LPARAM)g_szApplications[iCI]);
			iCI++;
		}
	}
	if(uMsg==WM_NCDESTROY){
		::DeleteObject(hfCreated);
		hfCreated=0;
	}
	if(uMsg==WM_KEYDOWN){
		if(GetKeyState(VK_CONTROL)<0){
			if(wParam==VK_RETURN){
				uMsg=WM_COMMAND;
				wParam=IDOK;
			}
		}
	}
	if(uMsg==WM_DROPFILES){
		HDROP hDropInfo=(HDROP)wParam;
		char szFilePath[MAX_PATH]="",szD[MAX_PATH]={0},szP[MAX_PATH]={0};;
		DragQueryFile(hDropInfo, 0, szFilePath, sizeof(szFilePath));
		_splitpath(szFilePath,szD,szP,0,0);
		strcpy(szFilePath,szD);
		strcat(szFilePath,szP);
		::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),szFilePath);
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		EndDialog(hwndDlg,1);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_UP){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR && iCurSel>0){
			char szCur[MAX_APP_NAME+5]={0};
			char szNext[MAX_APP_NAME+5]={0};
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel,(LPARAM)szCur);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_DELETESTRING,iCurSel,0);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_INSERTSTRING,iCurSel-1,(LPARAM)szCur);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETCURSEL,iCurSel-1,0);
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_DOWN){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR){
			char szCur[MAX_APP_NAME+5]={0};
			char szNext[MAX_APP_NAME+5]={0};
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel,(LPARAM)szCur);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel+1,(LPARAM)szNext);
			if(szNext[0]!=0){
				SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_DELETESTRING,iCurSel+1,0);
				SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_INSERTSTRING,iCurSel,(LPARAM)szNext);
			}
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_REM){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR){
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_DELETESTRING,iCurSel,0);
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_SETCURSEL,iCurSel>1?iCurSel-1:0,0);
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDC_BT_ADD){
		char szCur[MAX_APP_NAME]={0};
		::GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),szCur,sizeof(szCur));
		if(szCur[0]!=0){
			BOOL bL=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_L), BM_GETCHECK, 0, 0);
			BOOL bR=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_R), BM_GETCHECK, 0, 0);
			BOOL bT=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_T), BM_GETCHECK, 0, 0);
			BOOL bB=(int)::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_B), BM_GETCHECK, 0, 0);
			if((bL && bR) || (bT && bB) || (bL+bR+bT+bB==0)){
				return TRUE;
			}
			memmove(szCur+5,szCur,strlen(szCur));
			szCur[0]=szCur[1]=szCur[2]=szCur[3]='.';
			szCur[4]=' ';
			if(bL){
				szCur[0]='L';
			}
			if(bR){
				szCur[1]='R';
			}
			if(bT){
				szCur[2]='T';
			}
			if(bB){
				szCur[3]='B';
			}
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_ADDSTRING,0,(LPARAM)szCur);
			::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),"");
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_T), BM_SETCHECK, 0, 0);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_L), BM_SETCHECK, 0, 0);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_B), BM_SETCHECK, 0, 0);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_R), BM_SETCHECK, 0, 0);
		}
		return TRUE;
	}
	if(uMsg==WM_COMMAND && LOWORD(wParam)==IDC_LIST && HIWORD(wParam)==LBN_SELCHANGE){
		int iCurSel=SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETCURSEL,0,0);
		if(iCurSel!=LB_ERR){
			char szCur[MAX_PATH+5]={0};
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCurSel,(LPARAM)szCur);
			::SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT),szCur+5);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_L), BM_SETCHECK, szCur[0]=='L', 0);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_R), BM_SETCHECK, szCur[1]=='R', 0);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_T), BM_SETCHECK, szCur[2]=='T', 0);
			::SendMessage(GetDlgItem(hwndDlg,IDC_CHECK_B), BM_SETCHECK, szCur[3]=='B', 0);
		}
	}
	if(uMsg==WM_COMMAND && wParam==IDOPTIONS){
		WKGetPluginContainer()->ShowPluginPrefernces();
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		int iCI=0;
		while(iCI<MAX_APPS){
			g_szApplications[iCI][0]=0;
			SendMessage(GetDlgItem(hwndDlg,IDC_LIST),LB_GETTEXT,iCI,(LPARAM)g_szApplications[iCI]);
			iCI++;
		}
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	return FALSE;
}

DWORD WINAPI CallOptions2(LPVOID pData)
{
	HWND hParent=(HWND)pData;
	static BOOL bInProc=0;
	if(bInProc){
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hOpenedOptions2);
		return 0;
	}
	bInProc++;
	DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_OPT_DIALOG2),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),OptionsDialogProc2);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	DWORD dwError=GetLastError();
	WKGetPluginContainer()->PushMyOptions(0);
	bInProc--;
	return 1;
}

int	WINAPI WKPluginOptions(HWND hParent)
{
	CallOptions(hParent);
	return 1;
}

char szHotkeyStrPresentation[32]={0};
int	WINAPI WKGetPluginFunctionHints(long iPluginFunction, long lHintCode, void*& pOut)
{
	if(iPluginFunction==4){
		strcpy(szHotkeyStrPresentation,"%0-2,489");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	if(iPluginFunction==3){
		strcpy(szHotkeyStrPresentation,"%0-2,488");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	if(iPluginFunction==2){
		strcpy(szHotkeyStrPresentation,"%0-2,490");
		pOut=szHotkeyStrPresentation;
		return 1;
	}
	return 0;
};

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction==4){
		stuff->dwItemMenuPosition=0;
		strcpy(stuff->szItemName,"Minimize window to tray");
		stuff->dwDefaultHotkey=0;
		return 1;
	}
	if(iPluginFunction==3){
		stuff->dwItemMenuPosition=0;
		strcpy(stuff->szItemName,"Rollup window");
		stuff->dwDefaultHotkey=0;
		return 1;
	}
	if(iPluginFunction==2){
		stuff->dwItemMenuPosition=0;
		strcpy(stuff->szItemName,"Show hot menu");
		stuff->dwDefaultHotkey=0;
		return 1;
	}
	if(iPluginFunction>4 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Autohide windows");
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwDefaultHotkey=0;
	}else{
		strcpy(stuff->szItemName,"Autodock windows");
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON1));
		stuff->dwDefaultHotkey=0;
	}
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>4 || stuff==NULL){
		return 0;
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	POINT pt;
	GetCursorPos(&pt);
	DWORD lHookerDesiredAction=0;
	HWND hLastWnd=WindowFromPoint(pt);
	if(iPluginFunction==4){
		lHookerDesiredAction=2;
		PostThreadMessage(dwHookerThread,WM_USER+lHookerDesiredAction,WPARAM(hLastWnd),LPARAM(0));
		return 1;
	}
	if(iPluginFunction==3){
		lHookerDesiredAction=6;
		PostThreadMessage(dwHookerThread,WM_USER+lHookerDesiredAction,WPARAM(hLastWnd),LPARAM(0));
		return 1;
	}
	if(iPluginFunction==2){
		lHookerDesiredAction=3;
		PostThreadMessage(dwHookerThread,WM_USER+lHookerDesiredAction,WPARAM(hLastWnd),LPARAM(0));
		return 1;
	}
	if(iPluginFunction==1){
		if(stuff->hForegroundWindow!=0){
			GetWindowText(stuff->hForegroundWindow,g_DefDockTitle,sizeof(g_DefDockTitle));
		}
		CallOptions2(0);
		return 1;
	}
	WKPluginOptions(0);
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	OSVERSIONINFOEX winfo; 
	ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
	winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	dsc->bLoadPluginByDefault=TRUE;
	dsc->bResidentPlugin=TRUE;
	//strcpy(dsc->szTitle,"Windows helper");
	strcpy(dsc->szDesk,"This plugin allows you to minimize windows by right clicking its minimize button, make all windows sticky and much more");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	dsc->dwVersionHi=1;
	dsc->dwVersionLo=1;
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		//pOptionsCallback->AddStringOption("amin2tray","Minimize matching applications to tray icon instead of taskbar","Use ';' as delimiter between different titles, Use '*' as a wildcard for partial matches","",0,0);
		//pOptionsCallback->AddStringOption("amin2float","Minimize matching applications to floater instead of taskbar","Use ';' as delimiter between different titles, Use '*' as a wildcard for partial matches","",0,0);
		pOptionsCallback->AddStringOption("cautominStr","","","",0,0);
		pOptionsCallback->AddBoolOption("cautomin","","",FALSE,0);
		pOptionsCallback->AddNumberOption("iINACTSEC","","",0,0,1000,0);

		//pOptionsCallback->AddBoolOption("focusSteal","Do not allow other windows to steal keyboard focus while typing","",FALSE,0);
		pOptionsCallback->AddActionOption("options","Define windows that plugin must hide automatically",0,0);
		pOptionsCallback->AddActionOption("options3","Define windows that should be minimized to tray",0,0);
		pOptionsCallback->AddActionOption("options4","Define windows that should be minimized to floater",0,0);
		pOptionsCallback->AddActionOption("options2","Define windows that plugin must dock automatically",1,0);
						
		pOptionsCallback->AddBoolOption("bSnapMovements","Snap windows to each other","",FALSE,0);
		pOptionsCallback->AddNumberOptionEx("iStickness","Snap distance (pixels)","",0,0,1000,0,"bSnapMovements");
		pOptionsCallback->AddBoolOption("bSnapToScreen","Snap windows to screen edges","",FALSE,0);
		pOptionsCallback->AddNumberOptionEx("iSnapToScreenStickness","Snap distance (pixels)","",0,0,1000,0,"bSnapToScreen");
		pOptionsCallback->AddBoolOption("s2screenForce","Force windows to be inside the screen","This feature can be temporarily disabled. Just press and hold 'Shift' key when you want to move window beyond screen borders",FALSE,0);
		pOptionsCallback->AddBoolOptionEx("s3screenForce","Force windows to be inside the screen always - regardless of windows type","",FALSE,0,"s2screenForce");
		pOptionsCallback->AddStringOptionEx("forceexclusions","Exclude matching windows from incidence of this plugin","If this plugin corrupts the look-and-feel of some windows, then you can put window title in this field. Use ';' as delimiter between different titles, Use '*' as a wildcard for partial matches","",0,0,"s2screenForce");
		pOptionsCallback->AddBoolOption("ontopicon","Change icon of 'on top' windows","",0,OL_ACTIVEWND);
		pOptionsCallback->AddBoolOption("inject","Add commands to a window system menu","",TRUE,OL_ACTIVEWND);
		pOptionsCallback->AddBoolOptionEx("inject1","'Hide to tray' item","",TRUE,OL_ACTIVEWND,"inject");
		pOptionsCallback->AddBoolOptionEx("inject3","'Rollup to title' item","",FALSE,OL_ACTIVEWND,"inject");
		pOptionsCallback->AddBoolOptionEx("inject6","'Create shortcut' item","",FALSE,OL_ACTIVEWND,"inject");
		pOptionsCallback->AddBoolOptionEx("inject2","'Hide completely' item","",FALSE,OL_ACTIVEWND,"inject");
		pOptionsCallback->AddBoolOptionEx("inject5","'Add to quick-run' item","",TRUE,OL_ACTIVEWND,"inject");
		pOptionsCallback->AddBoolOptionEx("inject4","'Application info' item","",TRUE,OL_ACTIVEWND,"inject");
		pOptionsCallback->AddBoolOptionEx("inject7","'Kill application' item","",TRUE,OL_ACTIVEWND,"inject");

		pOptionsCallback->AddBoolOption("replexk","Replace default hotkeys with custom one","",TRUE,OL_EXPLORERHK);
		
		//pOptionsCallback->AddTip("rclickt","Mouse gestures",OL_ACTIVEWND);
		pOptionsCallback->AddBoolOption("rooldock","Window border: Left click to dock","",TRUE,OL_ACTIVEWND);
		pOptionsCallback->AddBoolOption("roolexpand","Window border: Right click to expand","",TRUE,OL_ACTIVEWND);
		pOptionsCallback->AddStringOption("disableall","Disable WireKeys when one of this windows is active","Use ';' as delimiter between different titles, Use '*' as a wildcard for partial matches","",0);
		pOptionsCallback->AddTip("otheropt","Other options can be found under 'Active windows' preferences folder",0);
		pOptionsCallback->AddBinOption("items");
		pOptionsCallback->AddBinOption("amin2tray");
		pOptionsCallback->AddBinOption("amin2float");
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,0,0);
			DWORD dwRes=WaitForSingleObject(hHookerEventDone,500);
		}
		hookData.bSnapMovements=pOptionsCallback->GetBoolOption("bSnapMovements");
		hookData.iStickness=pOptionsCallback->GetBoolOption("iStickness");
		hookData.bSnapToScreen=pOptionsCallback->GetBoolOption("bSnapToScreen");
		hookData.iSnapToScreenStickness=pOptionsCallback->GetBoolOption("iSnapToScreenStickness");
		hookData.bInjectIt1=pOptionsCallback->GetBoolOption("inject1");
		hookData.bInjectIt2=pOptionsCallback->GetBoolOption("inject2");
		hookData.bInjectIt3=pOptionsCallback->GetBoolOption("inject3");
		hookData.bInjectIt4=pOptionsCallback->GetBoolOption("inject4");
		hookData.bInjectIt5=pOptionsCallback->GetBoolOption("inject5");
		hookData.bInjectIt6=pOptionsCallback->GetBoolOption("inject6");
		hookData.bInjectIt7=pOptionsCallback->GetBoolOption("inject7");
		hookData.bCatchAutoMinimize=pOptionsCallback->GetBoolOption("cautomin");
		hookData.bAllowReplaceExplorerKeys=pOptionsCallback->GetBoolOption("replexk");
		pOptionsCallback->GetStringOption("cautominStr",hookData.szAutoMinimizeTitle,sizeof(hookData.szAutoMinimizeTitle));
		pOptionsCallback->GetStringOption("disableall",hookData.szAutoDisalbe,sizeof(hookData.szAutoDisalbe));
		
		
		hookData.bSnapToScreenForce=pOptionsCallback->GetBoolOption("s2screenForce");
		hookData.bSnapToScreenForceFull=pOptionsCallback->GetBoolOption("s3screenForce");
		hookData.bCatchRollDock=pOptionsCallback->GetBoolOption("rooldock");
		hookData.bCatchRollExpand=pOptionsCallback->GetBoolOption("roolexpand");
		hookData.bInjectItems=pOptionsCallback->GetBoolOption("inject");
		hookData.bOntopicon=pOptionsCallback->GetBoolOption("ontopicon");
		hookData.bBlockFocuStealing=0;//pOptionsCallback->GetBoolOption("focusSteal");
		hookData.iINACTSEC=pOptionsCallback->GetNumberOption("iINACTSEC");
		pOptionsCallback->GetBinOption("amin2tray",hookData.szAutoMinToTray,sizeof(hookData.szAutoMinToTray));
		pOptionsCallback->GetBinOption("amin2float",hookData.szAutoMinToFloat,sizeof(hookData.szAutoMinToFloat));
		pOptionsCallback->GetStringOption("forceexclusions",plugData.szExclusions,sizeof(plugData.szExclusions));
		pOptionsCallback->GetBinOption("items",g_szApplications,MAX_APPS*sizeof(g_szApplications[0]));
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThreadInThisDLL,WM_ENABLE,1,0);
			DWORD dwRes=WaitForSingleObject(hHookerEventDone,500);
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("bSnapMovements",hookData.bSnapMovements);
		pOptionsCallback->SetBoolOption("iStickness",hookData.iStickness);
		pOptionsCallback->SetBoolOption("bSnapToScreen",hookData.bSnapToScreen);
		pOptionsCallback->SetBoolOption("iSnapToScreenStickness",hookData.iSnapToScreenStickness);
		pOptionsCallback->SetBoolOption("inject1",hookData.bInjectIt1);
		pOptionsCallback->SetBoolOption("inject2",hookData.bInjectIt2);
		pOptionsCallback->SetBoolOption("inject3",hookData.bInjectIt3);
		pOptionsCallback->SetBoolOption("inject4",hookData.bInjectIt4);
		pOptionsCallback->SetBoolOption("inject5",hookData.bInjectIt5);
		pOptionsCallback->SetBoolOption("inject6",hookData.bInjectIt6);
		pOptionsCallback->SetBoolOption("inject7",hookData.bInjectIt7);
		pOptionsCallback->SetBoolOption("cautomin",hookData.bCatchAutoMinimize);
		pOptionsCallback->SetBoolOption("replexk",hookData.bAllowReplaceExplorerKeys);
		pOptionsCallback->SetStringOption("cautominStr",hookData.szAutoMinimizeTitle);
		pOptionsCallback->SetStringOption("disableall",hookData.szAutoDisalbe);
		pOptionsCallback->SetBoolOption("s2screenForce",hookData.bSnapToScreenForce);
		pOptionsCallback->SetBoolOption("s3screenForce",hookData.bSnapToScreenForceFull);
		pOptionsCallback->SetBoolOption("rooldock",hookData.bCatchRollDock);
		pOptionsCallback->SetBoolOption("roolexpand",hookData.bCatchRollExpand);
		pOptionsCallback->SetBoolOption("inject",hookData.bInjectItems);
		pOptionsCallback->SetBoolOption("ontopicon",hookData.bOntopicon);
		//pOptionsCallback->SetBoolOption("focusSteal",hookData.bBlockFocuStealing);
		pOptionsCallback->SetNumberOption("iINACTSEC",hookData.iINACTSEC);
		pOptionsCallback->SetBinOption("amin2tray",hookData.szAutoMinToTray,sizeof(hookData.szAutoMinToTray));
		pOptionsCallback->SetBinOption("amin2float",hookData.szAutoMinToFloat,sizeof(hookData.szAutoMinToFloat));
		pOptionsCallback->SetStringOption("forceexclusions",plugData.szExclusions);
		pOptionsCallback->SetBinOption("items",g_szApplications,MAX_APPS*sizeof(g_szApplications[0]));
	}
	return 1;
}

#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif