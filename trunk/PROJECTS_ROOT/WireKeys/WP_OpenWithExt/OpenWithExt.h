/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Jul 06 06:50:51 2009
 */
/* Compiler settings for Z:\_WPLabs\PROJECTS_ROOT\WireKeys\WP_OpenWithExt\OpenWithExt.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifdef __cplusplus
extern "C"{
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

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

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
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWKShellExtender __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWKShellExtender __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWKShellExtender __RPC_FAR * This);
        
        END_INTERFACE
    } IWKShellExtenderVtbl;

    interface IWKShellExtender
    {
        CONST_VTBL struct IWKShellExtenderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWKShellExtender_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWKShellExtender_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWKShellExtender_Release(This)	\
    (This)->lpVtbl -> Release(This)


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
