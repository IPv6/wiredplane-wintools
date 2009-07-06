// SupportClasses.h: interface for the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DELAYLOAD__INCLUDED_)
#define DELAYLOAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxmt.h>
#include <afxtempl.h>
#include "../SystemUtils/Macros.h"
#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH   2540
#endif
#ifndef MAP_PIX_TO_LOGHIM
#define MAP_PIX_TO_LOGHIM(x,ppli)   ( (HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli) )
#endif
#ifndef MAP_LOGHIM_TO_PIX
#define MAP_LOGHIM_TO_PIX(x,ppli)   ( ((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH )
#endif

typedef int  (CALLBACK *FARPROCG) (HWND hwnd, LPWSTR lpString, int nMaxCount);
typedef BOOL (CALLBACK *FARPROCL) (HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
typedef BOOL (CALLBACK *FARPROCW) (HWND hwnd, BOOL bExpand);
typedef BOOL (CALLBACK *FARPROCA) (HWND hwnd, DWORD, DWORD);
typedef BOOL (CALLBACK *FARPROCLOCKFOREG) (BOOL bLock);
#if _WIN32_WINNT == 0x0400
#define AW_HOR_POSITIVE             0x00000001
#define AW_HOR_NEGATIVE             0x00000002
#define AW_VER_POSITIVE             0x00000004
#define AW_VER_NEGATIVE             0x00000008
#define AW_CENTER                   0x00000010
#define AW_HIDE                     0x00010000
#define AW_ACTIVATE                 0x00020000
#define AW_SLIDE                    0x00040000
#define AW_BLEND                    0x00080000
#endif
class CNTWnWrap
{
public:
    HMODULE	m_hUser32Instance;
	FARPROCG InternalGetWindowText;
	FARPROCL SetLayerAttribs;
	FARPROCW SwitchToWnd;
	FARPROCA fAnimate;
	FARPROCLOCKFOREG fLock;
	CNTWnWrap()
	{
		SetLayerAttribs=NULL;
		SwitchToWnd=NULL;
		fAnimate=NULL;
		fLock=NULL;
		m_hUser32Instance=LoadLibrary("User32.dll");
	}

	~CNTWnWrap()
	{
		if(m_hUser32Instance){
			FreeLibrary(m_hUser32Instance);
		}
	}
};
CNTWnWrap& getCNTWnWrap();
BOOL SetLayeredWindowStyle(HWND m_hWnd, BOOL bSet);
BOOL SetWndAlpha(HWND m_hWnd, DWORD dwPercent, BOOL bAlpha);


typedef struct MYtagMENUINFO
{
    DWORD   cbSize;
    DWORD   fMask;
    DWORD   dwStyle;
    UINT    cyMax;
    HBRUSH  hbrBack;
    DWORD   dwContextHelpID;
    DWORD   dwMenuData;
}   MYMENUINFO, FAR *MYLPMENUINFO;
typedef MYMENUINFO CONST FAR *MYLPCMENUINFO;

BOOL SwitchToWindow(HWND hWnd, BOOL bMoveFocus=TRUE);
void User32Animate(HWND hWnd,DWORD dwMilli,DWORD dwFlags);
void ILockSetForegroundWindow(BOOL bDir);
BOOL SafeGetWindowText(HWND hWnd, CString& out);
BOOL BlockInputMy(BOOL bBlock);
void MyLockSetForegroundWindow(BOOL bLock);
BOOL MySetMenuInfo(HMENU hMenu,MYLPCMENUINFO cmi);
HANDLE MyOpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle,DWORD dwThreadId);
BOOL MyIsHungAppWindow(HWND hw);
#endif // !defined(DELAYLOAD__INCLUDED_)
