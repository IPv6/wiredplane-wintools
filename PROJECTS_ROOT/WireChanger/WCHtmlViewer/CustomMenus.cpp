// CustomMenus.cpp : An example of how to customize the WBC menus
//

#include "stdafx.h"
#include <mshtmhst.h>
#include <mshtmcid.h>
#include "CustomMenus.h"
#include "resource.h"

#define IDR_BROWSE_CONTEXT_MENU		24641
#define IDM_EXTRA_ITEM				6047

HRESULT ModifyContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved);
HRESULT CustomContextMenu(POINT *ppt, IUnknown *pcmdtReserved);

HRESULT CustomShowContextMenu(UINT mode, DWORD dwID, POINT *pptPosition,
					IUnknown *pCommandTarget, IDispatch *pDispatchObjectHit)
{
HRESULT result = S_FALSE; //Dont Interfere

	switch ( mode )
	{
		case kDefaultMenuSupport:
			break;

		case kNoContextMenu:
			result	= S_OK;
			break;

		case kTextSelectionOnly:
			if (dwID != CONTEXT_MENU_TEXTSELECT)
				result	= S_OK;
			break;

		case kAllowAllButViewSource:
			if (dwID == CONTEXT_MENU_DEFAULT)
				result	= ModifyContextMenu(dwID, pptPosition, pCommandTarget);
			break;

		case kCustomMenuSupport:
			if (dwID == CONTEXT_MENU_DEFAULT)
				result = CustomContextMenu(pptPosition, pCommandTarget);
			break;
	}

	return result;
}

HRESULT ModifyContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved) 
{
	ASSERT((ppt != NULL) && (pcmdtReserved != NULL));
	if ((ppt == NULL) || (pcmdtReserved == NULL))
		return S_OK;

    //#define IDR_BROWSE_CONTEXT_MENU  24641
    //#define IDR_FORM_CONTEXT_MENU    24640
    #define SHDVID_GETMIMECSETMENU   27
    #define SHDVID_ADDMENUEXTENSIONS 53

    HRESULT hr;
    HINSTANCE hinstSHDOCLC;
    HWND hwnd;
    HMENU hMenu;
    IOleCommandTarget *spCT;
    IOleWindow *spWnd;
    MENUITEMINFO mii = {0};
    VARIANT var, var1, var2;

    hr = pcmdtReserved->QueryInterface(IID_IOleCommandTarget, (void**)&spCT);
    hr = pcmdtReserved->QueryInterface(IID_IOleWindow, (void**)&spWnd);
    hr = spWnd->GetWindow(&hwnd);

    hinstSHDOCLC = LoadLibrary(TEXT("SHDOCLC.DLL"));

    hMenu = LoadMenu(hinstSHDOCLC,
                     MAKEINTRESOURCE(IDR_BROWSE_CONTEXT_MENU));

    hMenu = GetSubMenu(hMenu, dwID);

    // Get the language submenu
    hr = spCT->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, 0, NULL, &var);

    mii.cbSize = sizeof(mii);
    mii.fMask  = MIIM_SUBMENU;
    mii.hSubMenu = (HMENU) var.byref;

    // Add language submenu to Encoding context item
    SetMenuItemInfo(hMenu, IDM_LANGUAGE, FALSE, &mii);

    // Insert Shortcut Menu Extensions from registry
    V_VT(&var1) = VT_PTR;
    V_BYREF(&var1) = hMenu;

    V_VT(&var2) = VT_I4;
    V_I4(&var2) = dwID;

    hr = spCT->Exec(&CGID_ShellDocView, SHDVID_ADDMENUEXTENSIONS, 0, &var1, &var2);

    // Remove View Source
    ::DeleteMenu(hMenu, IDM_VIEWSOURCE, MF_BYCOMMAND);
	// Remove the item that produces the exta separator
	::DeleteMenu(hMenu, IDM_EXTRA_ITEM, MF_BYCOMMAND);

    // Show shortcut menu
    int iSelection = ::TrackPopupMenu(hMenu,
                                      TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                      ppt->x,
                                      ppt->y,
                                      0,
                                      hwnd,
                                      (RECT*)NULL);

    // Send selected shortcut menu item command to shell
	if (iSelection != 0)
		LRESULT lr = ::SendMessage(hwnd, WM_COMMAND, iSelection, NULL);

    FreeLibrary(hinstSHDOCLC);
    return S_OK;
}

HRESULT CustomContextMenu(POINT *ppt, IUnknown *pcmdtReserved) 
{
	IOleWindow	*oleWnd			= NULL;
    HWND		hwnd			= NULL;
    HMENU		hMainMenu		= NULL;
	HMENU		hPopupMenu		= NULL;
	HRESULT		hr				= 0;
	INT			iSelection		= 0;

	if ((ppt == NULL) || (pcmdtReserved == NULL))
		goto error;

    hr = pcmdtReserved->QueryInterface(IID_IOleWindow, (void**)&oleWnd);
	if ( (hr != S_OK) || (oleWnd == NULL))
		goto error;

	hr = oleWnd->GetWindow(&hwnd);
	if ( (hr != S_OK) || (hwnd == NULL))
		goto error;

    hMainMenu = 0;//LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_CUSTOM_POPUP));
	if (hMainMenu == NULL)
		goto error;

    hPopupMenu = GetSubMenu(hMainMenu, 0);
	if (hPopupMenu == NULL)
		goto error;

    // Show shortcut menu
    iSelection = ::TrackPopupMenu(hPopupMenu,
						TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
						ppt->x,
						ppt->y,
						0,
						hwnd,
						(RECT*)NULL);

    // Send selected shortcut menu item command to shell
	if (iSelection != 0)
		(void) ::SendMessage(hwnd, WM_COMMAND, iSelection, NULL);

error:

	if (hMainMenu != NULL)
		::DestroyMenu(hMainMenu);

    return S_OK;
}
