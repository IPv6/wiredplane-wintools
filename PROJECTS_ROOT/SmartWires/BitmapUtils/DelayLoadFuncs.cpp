// SupportClasses.cpp: implementation of the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DelayLoadFuncs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CNTWnWrap& getCNTWnWrap()
{
	static CNTWnWrap wrap;
	return wrap;
}

BOOL SetLayeredWindowStyle(HWND m_hWnd, BOOL bSet)
{
	if(getCNTWnWrap().m_hUser32Instance){
		if(bSet){
			SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd, GWL_EXSTYLE) | (0x00080000L));//WS_EX_APPWINDOW         0x00040000L
		}else{
			SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~(0x00080000L));
		}
		return TRUE;
	}
	return FALSE;
}

BOOL SetWndAlpha(HWND m_hWnd, DWORD dwPercent, BOOL bAlpha)
{
	if(getCNTWnWrap().m_hUser32Instance){
		if(!getCNTWnWrap().SetLayerAttribs){
			getCNTWnWrap().SetLayerAttribs=(FARPROCL)DBG_GETPROC(getCNTWnWrap().m_hUser32Instance,"SetLayeredWindowAttributes");
		}
		if(getCNTWnWrap().SetLayerAttribs){
			if(bAlpha){
				return getCNTWnWrap().SetLayerAttribs(m_hWnd, 0, (255 * (BYTE)dwPercent) / 100, 2);
			}else{
				return getCNTWnWrap().SetLayerAttribs(m_hWnd, dwPercent, 255, 1);
			}
		}
	}
	return FALSE;
}

BOOL SwitchToWindow(HWND hWnd, BOOL bMoveFocus)
{
	if(!::IsWindow(hWnd)){
		return FALSE;
	}
	BOOL bCanReturnIfSwitched=0;
	if(GetForegroundWindow()!=hWnd){
		bCanReturnIfSwitched=1;
	}
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hWnd);
	if(bCanReturnIfSwitched && GetForegroundWindow()==hWnd){
		return TRUE;
	}
	BOOL bTopMost=::GetWindowLong(hWnd,GWL_EXSTYLE) & WS_EX_TOPMOST;
	BOOL bRes=FALSE;
	int iShowCmd=SW_SHOWNORMAL;
	if(::IsZoomed(hWnd)){
		iShowCmd=SW_SHOW;
	}
	HWND hFore=GetForegroundWindow();
	if(hFore){
		::SetWindowPos(hWnd,hFore,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|iShowCmd);
	}
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	::ShowWindow(hWnd,iShowCmd);
	bRes=::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|iShowCmd);
	bRes=::RedrawWindow(hWnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_FRAME) && bRes;
	if(!bTopMost){
		bRes=::SetWindowPos(hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW) && bRes;
	}
	if(bMoveFocus){
		::SetFocus(hWnd);
	}
	return bRes;
}

void User32Animate(HWND hWnd,DWORD dwMilli,DWORD dwFlags)
{
	if(getCNTWnWrap().m_hUser32Instance){
		if(!getCNTWnWrap().fAnimate){
			getCNTWnWrap().fAnimate=(FARPROCA)DBG_GETPROC(getCNTWnWrap().m_hUser32Instance,"AnimateWindow");
		}
		if(getCNTWnWrap().fAnimate){
			getCNTWnWrap().fAnimate(hWnd, dwMilli, dwFlags);
		}
	}
}

void ILockSetForegroundWindow(BOOL bDir)
{
	if(getCNTWnWrap().m_hUser32Instance){
		if(!getCNTWnWrap().fLock){
			getCNTWnWrap().fLock=(FARPROCLOCKFOREG)DBG_GETPROC(getCNTWnWrap().m_hUser32Instance,"LockSetForegroundWindow");
		}
		if(getCNTWnWrap().fLock){
			getCNTWnWrap().fLock(bDir?1:2);
		}
	}
}

BOOL SafeGetWindowText(HWND hWnd, CString& out)
{
	int iOutCount=0;
	if(0){// На WinXP - не работает!!!!
		if(getCNTWnWrap().m_hUser32Instance){
			if(!getCNTWnWrap().InternalGetWindowText){
				getCNTWnWrap().InternalGetWindowText=(FARPROCG)DBG_GETPROC(getCNTWnWrap().m_hUser32Instance,"InternalGetWindowText");
			}
			if(getCNTWnWrap().InternalGetWindowText){
				WCHAR str[1024];
				memset(str,0,sizeof(str));
				iOutCount=getCNTWnWrap().InternalGetWindowText(hWnd, str, sizeof(str));
				DWORD dwGetLastError=GetLastError();
				if(dwGetLastError==0){
					out=str;
					return (iOutCount!=0);
				}
			}
		}
	}
	char str[1024]="";
	memset(str,0,sizeof(str));
	iOutCount=::GetWindowText(hWnd, str, sizeof(str));
	out=str;
	return (iOutCount!=0);
}

typedef BOOL (WINAPI *_BlockInput)(BOOL);
BOOL BlockInputMy(BOOL bBlock)
{
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	_BlockInput fp=NULL;
	if(hUser32){
		fp=(_BlockInput)DBG_GETPROC(hUser32,"BlockInput");
	}
	if(hUser32 && fp){
		return (*fp)(bBlock);
	}
	return FALSE;
}

typedef BOOL (WINAPI *_LockSetForegroundWindow)(UINT);
void MyLockSetForegroundWindow(BOOL bLock)
{
	// В WinME эта блокировка все намертво вешает!
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	if(hUser32){
		_LockSetForegroundWindow fp=(_LockSetForegroundWindow)DBG_GETPROC(hUser32,"LockSetForegroundWindow");
		if(fp){
			(*fp)(bLock?1:2);
		}
		FreeLibrary(hUser32);
	}
	BlockInputMy(bLock);
	return;
}

typedef BOOL (WINAPI *_SetMenuInfo)(HMENU hMenu,MYLPCMENUINFO cmi);
BOOL MySetMenuInfo(HMENU hMenu,MYLPCMENUINFO cmi)
{
	BOOL bRes=0;
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	if(hUser32){
		_SetMenuInfo fp=(_SetMenuInfo)DBG_GETPROC(hUser32,"SetMenuInfo");
		if(fp){
			bRes=(*fp)(hMenu,cmi);
		}
		FreeLibrary(hUser32);
	}
	return bRes;
}

typedef BOOL (WINAPI *_IsHungAppWindow)(HWND hw);
BOOL MyIsHungAppWindow(HWND hw)
{
	BOOL bRes=0;
	HINSTANCE hUser32=LoadLibrary("user32.dll");
	if(hUser32){
		_IsHungAppWindow fp=(_IsHungAppWindow)DBG_GETPROC(hUser32,"IsHungAppWindow");
		if(fp){
			bRes=(*fp)(hw);
		}
		FreeLibrary(hUser32);
	}
	return bRes;
}

typedef HANDLE (WINAPI *_OpenThread)(DWORD dwDesiredAccess, BOOL bInheritHandle,DWORD dwThreadId);
HANDLE MyOpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle,DWORD dwThreadId)
{
	HANDLE bRes=0;
	HINSTANCE hUser32=LoadLibrary("kernel32.dll");
	if(hUser32){
		_OpenThread fp=(_OpenThread)DBG_GETPROC(hUser32,"OpenThread");
		if(fp){
			bRes=(*fp)(dwDesiredAccess, bInheritHandle,dwThreadId);
		}
		FreeLibrary(hUser32);
	}
	return bRes;
}

//GradientFill
