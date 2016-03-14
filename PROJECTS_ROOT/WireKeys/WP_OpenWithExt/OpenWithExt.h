

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Jan 07 17:38:12 2016
 */
/* Compiler settings for OpenWithExt.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __OpenWithExt_h__
#define __OpenWithExt_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWKShellExtender_FWD_DEFINED__
#define __IWKShellExtender_FWD_DEFINED__
typedef interface IWKShellExtender IWKShellExtender;

#endif 	/* __IWKShellExtender_FWD_DEFINED__ */


#ifndef __WKShellExtender_FWD_DEFINED__
#define __WKShellExtender_FWD_DEFINED__

#ifdef __cplusplus
typedef class WKShellExtender WKShellExtender;
#else
typedef struct WKShellExtender WKShellExtender;
#endif /* __cplusplus */

#endif 	/* __WKShellExtender_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IWKShellExtender_INTERFACE_DEFINED__
#define __IWKShellExtender_INTERFACE_DEFINED__

/* interface IWKShellExtender */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IWKShellExtender;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A7C8C7B-2D94-43EB-8A1A-4FFC1FAEF236")
    IWKShellExtender : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IWKShellExtenderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWKShellExtender * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWKShellExtender * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWKShellExtender * This);
        
        END_INTERFACE
    } IWKShellExtenderVtbl;

    interface IWKShellExtender
    {
        CONST_VTBL struct IWKShellExtenderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWKShellExtender_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWKShellExtender_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWKShellExtender_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWKShellExtender_INTERFACE_DEFINED__ */



#ifndef __WKSHELLEXTLib_LIBRARY_DEFINED__
#define __WKSHELLEXTLib_LIBRARY_DEFINED__

/* library WKSHELLEXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_WKSHELLEXTLib;

EXTERN_C const CLSID CLSID_WKShellExtender;

#ifdef __cplusplus

class DECLSPEC_UUID("AC95BA2C-8211-45D4-AB5C-C2A9BCCC8FB6")
WKShellExtender;
#endif
#endif /* __WKSHELLEXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


