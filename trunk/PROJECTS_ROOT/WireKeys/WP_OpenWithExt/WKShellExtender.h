// WKShellExtender.h : Declaration of the CWKShellExtender

#ifndef __WKShellExtender_H_
#define __WKShellExtender_H_

#include "resource.h"       // main symbols

#include "..\..\SmartWires\Atl\crt_list.h"
#include "..\..\SmartWires\Atl\crt_cstring.h"
typedef CCOMList<CCOMString> string_list;

struct __declspec(uuid("000214e4-0000-0000-c000-000000000046")) IContextMenu;
typedef interface IContextMenu IContextMenu;
_COM_SMARTPTR_TYPEDEF(IContextMenu, __uuidof(IContextMenu));

/////////////////////////////////////////////////////////////////////////////
// CWKShellExtender
//CComSingleThreadModel
class ATL_NO_VTABLE CWKShellExtender : 
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CWKShellExtender, &CLSID_WKShellExtender>,
    public IShellExtInit,
    public IContextMenu
{
public:
DECLARE_REGISTRY_RESOURCEID(IDR_WKShellExtender)
DECLARE_NOT_AGGREGATABLE(CWKShellExtender)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWKShellExtender)
    COM_INTERFACE_ENTRY(IShellExtInit)
    COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()

public:
    CWKShellExtender(){}

	// IShellExtInit
    STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    STDMETHOD(GetCommandString)(UINT, UINT, UINT*, LPSTR, UINT);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
    STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);
	CCOMString GetFirstValidFolder();
	HWND hFore;
protected:
    TCHAR m_szSelectedFile[MAX_PATH+2];
	string_list m_szSelectedFiles;
	long lFiles;
	long lFolders;
	long lDLLs;
	long lLNKs;
	BOOL bDesktop;
	BOOL bBGFolder;
	long lWKPack;
};

class CWKShellExtender2: public CWKShellExtender
{
public:
	CWKShellExtender2()
	{
		lFiles=0;
		lFolders=0;
		lDLLs=0;
		lLNKs=0;
		bDesktop=0;
		bBGFolder=0;
		lWKPack=0;
	};
	long __stdcall QueryInterface(const struct _GUID &,void ** ){return 0;};
	unsigned long __stdcall AddRef(void){return 0;};
	unsigned long __stdcall Release(void){return 0;};
};
	
STDAPI DllRegisterServer(void);
STDAPI DllUnregisterServer(void);
#endif //__WKShellExtender_H_
