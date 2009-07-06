#include "resource.h"
#include "..\WKPlugin.h"

#ifdef WP_MEGAMOUSE_EXPORTS
#define WP_MEGAMOUSE_API __declspec(dllexport)
#else
#define WP_MEGAMOUSE_API __declspec(dllimport)
typedef int  (CALLBACK *FARPROCG) (HWND hwnd, LPWSTR lpString, int nMaxCount);
typedef BOOL (CALLBACK *FARPROCL) (HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
typedef BOOL (CALLBACK *FARPROCW) (HWND hwnd, BOOL bExpand);
typedef BOOL (CALLBACK *FARPROCA) (HWND hwnd, DWORD, DWORD);
typedef BOOL (CALLBACK *FARPROCLOCKFOREG) (BOOL bLock);

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
BOOL SwitchToWindow(HWND hWnd, BOOL bMoveFocus);

#endif
