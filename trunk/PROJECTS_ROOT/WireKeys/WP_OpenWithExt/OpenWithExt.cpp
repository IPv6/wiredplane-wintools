// OpenWithExt.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f OpenWithExtps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "WP_OPE.h"
#include "OpenWithExt.h"
#include "OpenWithExt_i.c"
#include "WKShellExtender.h"
extern COptions plgOptions;

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_WKShellExtender, CWKShellExtender)
END_OBJECT_MAP()
WINOLEAPI  CoInitializeEx(IN LPVOID pvReserved, IN DWORD dwCoInit);
/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
HINSTANCE g_hinstDll=NULL;
long lDetached=0;
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		g_hinstDll=hInstance;
        _Module.Init(ObjectMap, hInstance, &LIBID_WKSHELLEXTLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH && !lDetached){
        _Module.Term();
	}
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
#define SAVE_REGKEY_ROOT	"SOFTWARE\\WiredPlane\\WireKeys\\"
#define SAVE_REGKEY			SAVE_REGKEY_ROOT"WKOpenWithExt"
STDAPI DllRegisterServerRaw(BOOL bForce)
{
	if(!bForce){// Может мы уже зарегены?
		CRegKey reg;
		LONG    lRet=0;
		CRegKey key2;
		if(key2.Open(HKEY_CURRENT_USER, SAVE_REGKEY)==ERROR_SUCCESS && key2.m_hKey!=NULL){
			DWORD hookData=0;
			DWORD lSize = sizeof(hookData),dwType=0;
			RegQueryValueEx(key2.m_hKey,"ShellExtRegistered",NULL, &dwType,(LPBYTE)(&hookData), &lSize);
			if(hookData!=0){
				plgOptions.bRegistered=1;
			}
		}
	}
	// Не смотря ни на что даже при наличии регистрации в эксплорер меню может быть не видно!!! :(
	// поэтому всеже регим каждый раз
	/*if(plgOptions.bRegistered){
		return 0;
	}*/
	CoInitializeEx(0,COINIT_APARTMENTTHREADED);
	// If we're on NT, add ourselves to the list of approved shell extensions.
    // Note that you should *NEVER* use the overload of CRegKey::SetValue with
    // 4 parameters.  It lets you set a value in one call, without having to 
    // call CRegKey::Open() first.  However, that version of SetValue() has a
    // bug in that it requests KEY_ALL_ACCESS to the key.  That will fail if the
    // user is not an administrator.  (The code should request KEY_WRITE, which
    // is all that's necessary.)
	if ( 0 == (GetVersion() & 0x80000000UL) ){
		CRegKey reg;
		LONG    lRet=0;
		lRet = reg.Open ( HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),KEY_SET_VALUE);
		if ( ERROR_SUCCESS == lRet ){
			lRet = reg.SetValue ( _T("WKShellExtender class"), _T("{AC95BA2C-8211-45D4-AB5C-C2A9BCCC8FB6}") );
		}
	}
	// registers object, typelib and all interfaces in typelib
	BOOL bRes=_Module.RegisterServer(FALSE);
	if(bRes==0){
		if(!plgOptions.bRegistered){// Запоминаем сами...
			CRegKey key2;
			if(key2.Open(HKEY_CURRENT_USER, SAVE_REGKEY)!=ERROR_SUCCESS){
				key2.Create(HKEY_CURRENT_USER, SAVE_REGKEY);
			}
			if(key2.m_hKey!=NULL){
				DWORD hookData=1;
				RegSetValueEx(key2.m_hKey,"ShellExtRegistered",0,REG_DWORD,(BYTE*)(&hookData),sizeof(hookData));
			}
		}
		InterlockedIncrement(&plgOptions.bRegistered);
	}
	CoUninitialize();
    return bRes;
}

STDAPI DllRegisterServer()
{
	return DllRegisterServerRaw(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	if(plgOptions.bRegistered==0)
	{
		return 0;
	}
	// If we're on NT, remove ourselves from the list of approved shell extensions.
	// Note that if we get an error along the way, I don't bail out since I want
	// to do the normal ATL unregistration stuff too.
	if ( 0 == (GetVersion() & 0x80000000UL) )   {
		CRegKey reg;
		LONG    lRet;
		lRet = reg.Open ( HKEY_LOCAL_MACHINE,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
			KEY_SET_VALUE );
		if ( ERROR_SUCCESS == lRet ){
			lRet = reg.DeleteValue ( _T("{AC95BA2C-8211-45D4-AB5C-C2A9BCCC8FB6}") );
		}
	}
	BOOL bRes=1; 
	CoInitializeEx(0,COINIT_APARTMENTTHREADED);
	bRes=_Module.UnregisterServer(FALSE);
	if(bRes==0){
		InterlockedDecrement(&plgOptions.bRegistered);
	}
	CoUninitialize();
	{// Запоминаем сами...
		CRegKey key2;
		if(key2.Open(HKEY_CURRENT_USER, SAVE_REGKEY)!=ERROR_SUCCESS){
			key2.Create(HKEY_CURRENT_USER, SAVE_REGKEY);
		}
		if(key2.m_hKey!=NULL){
			DWORD hookData=0;
			RegSetValueEx(key2.m_hKey,"ShellExtRegistered",0,REG_DWORD,(BYTE*)(&hookData),sizeof(hookData));
		}
	}
    return bRes;
}
