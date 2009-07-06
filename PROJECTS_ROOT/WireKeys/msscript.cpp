// Created by Microsoft (R) C/C++ Compiler Version 12.00.8964.0 (8153d703).
//
// d:\projects_root\util\wirekeys\debug\msscript.tlh
//
// C++ source equivalent of Win32 type library c:\winnt\system32\msscript.ocx
// compiler-generated file created 04/07/03 at 11:40:41 - DO NOT EDIT!

#pragma once
#pragma pack(push, 8)

#include <comdef.h>

namespace MSScriptControl {

//
// Forward references and typedefs
//

struct __declspec(uuid("70841c73-067d-11d0-95d8-00a02463ab28"))
/* dual interface */ IScriptProcedure;
struct __declspec(uuid("70841c71-067d-11d0-95d8-00a02463ab28"))
/* dual interface */ IScriptProcedureCollection;
struct __declspec(uuid("70841c70-067d-11d0-95d8-00a02463ab28"))
/* dual interface */ IScriptModule;
struct __declspec(uuid("70841c6f-067d-11d0-95d8-00a02463ab28"))
/* dual interface */ IScriptModuleCollection;
struct __declspec(uuid("70841c78-067d-11d0-95d8-00a02463ab28"))
/* dual interface */ IScriptError;
struct __declspec(uuid("0e59f1d3-1fbe-11d0-8ff2-00a0d10038bc"))
/* dual interface */ IScriptControl;
struct __declspec(uuid("8b167d60-8605-11d0-abcb-00a0c90fffc0"))
/* dispinterface */ DScriptControlSource;
struct /* coclass */ Procedure;
struct /* coclass */ Procedures;
struct /* coclass */ Module;
struct /* coclass */ Modules;
struct /* coclass */ Error;
struct /* coclass */ ScriptControl;

//
// Smart pointer typedef declarations
//

_COM_SMARTPTR_TYPEDEF(IScriptProcedure, __uuidof(IScriptProcedure));
_COM_SMARTPTR_TYPEDEF(IScriptProcedureCollection, __uuidof(IScriptProcedureCollection));
_COM_SMARTPTR_TYPEDEF(IScriptModule, __uuidof(IScriptModule));
_COM_SMARTPTR_TYPEDEF(IScriptModuleCollection, __uuidof(IScriptModuleCollection));
_COM_SMARTPTR_TYPEDEF(IScriptError, __uuidof(IScriptError));
_COM_SMARTPTR_TYPEDEF(IScriptControl, __uuidof(IScriptControl));
_COM_SMARTPTR_TYPEDEF(DScriptControlSource, __uuidof(IDispatch));

//
// Type library items
//
    const LPSTR GlobalModule = (char*) "Global";
    const long NoTimeout = -1;

enum ScriptControlStates
{
    Initialized = 0,
    Connected = 1
};

struct __declspec(uuid("70841c73-067d-11d0-95d8-00a02463ab28"))
IScriptProcedure : IDispatch
{
    //
    // Property data
    //

    __declspec(property(get=GetName))
    _bstr_t Name;
    __declspec(property(get=GetNumArgs))
    long NumArgs;
    __declspec(property(get=GetHasReturnValue))
    VARIANT_BOOL HasReturnValue;

    //
    // Wrapper methods for error-handling
    //

    _bstr_t GetName ( );
    long GetNumArgs ( );
    VARIANT_BOOL GetHasReturnValue ( );

    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall get_Name (
        BSTR * pbstrName ) = 0;
    virtual HRESULT __stdcall get_NumArgs (
        long * pcArgs ) = 0;
    virtual HRESULT __stdcall get_HasReturnValue (
        VARIANT_BOOL * pfHasReturnValue ) = 0;
};

struct __declspec(uuid("70841c71-067d-11d0-95d8-00a02463ab28"))
IScriptProcedureCollection : IDispatch
{
    //
    // Property data
    //

    __declspec(property(get=GetItem))
    IScriptProcedurePtr Item[];
    __declspec(property(get=GetCount))
    long Count;
    __declspec(property(get=Get_NewEnum))
    IUnknownPtr _NewEnum;

    //
    // Wrapper methods for error-handling
    //

    IUnknownPtr Get_NewEnum ( );
    IScriptProcedurePtr GetItem (
        const _variant_t & Index );
    long GetCount ( );

    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall get__NewEnum (
        IUnknown * * ppenumProcedures ) = 0;
    virtual HRESULT __stdcall get_Item (
        VARIANT Index,
        struct IScriptProcedure * * ppdispProcedure ) = 0;
    virtual HRESULT __stdcall get_Count (
        long * plCount ) = 0;
};

struct __declspec(uuid("70841c70-067d-11d0-95d8-00a02463ab28"))
IScriptModule : IDispatch
{
    //
    // Property data
    //

    __declspec(property(get=GetName))
    _bstr_t Name;
    __declspec(property(get=GetCodeObject))
    IDispatchPtr CodeObject;
    __declspec(property(get=GetProcedures))
    IScriptProcedureCollectionPtr Procedures;

    //
    // Wrapper methods for error-handling
    //

    _bstr_t GetName ( );
    IDispatchPtr GetCodeObject ( );
    IScriptProcedureCollectionPtr GetProcedures ( );
    HRESULT AddCode (
        _bstr_t Code );
    _variant_t Eval (
        _bstr_t Expression );
    HRESULT ExecuteStatement (
        _bstr_t Statement );
    _variant_t Run (
        _bstr_t ProcedureName,
        SAFEARRAY * * Parameters );

    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall get_Name (
        BSTR * pbstrName ) = 0;
    virtual HRESULT __stdcall get_CodeObject (
        IDispatch * * ppdispObject ) = 0;
    virtual HRESULT __stdcall get_Procedures (
        struct IScriptProcedureCollection * * ppdispProcedures ) = 0;
    virtual HRESULT __stdcall raw_AddCode (
        BSTR Code ) = 0;
    virtual HRESULT __stdcall raw_Eval (
        BSTR Expression,
        VARIANT * pvarResult ) = 0;
    virtual HRESULT __stdcall raw_ExecuteStatement (
        BSTR Statement ) = 0;
    virtual HRESULT __stdcall raw_Run (
        BSTR ProcedureName,
        SAFEARRAY * * Parameters,
        VARIANT * pvarResult ) = 0;
};

struct __declspec(uuid("70841c6f-067d-11d0-95d8-00a02463ab28"))
IScriptModuleCollection : IDispatch
{
    //
    // Property data
    //

    __declspec(property(get=GetItem))
    IScriptModulePtr Item[];
    __declspec(property(get=GetCount))
    long Count;
    __declspec(property(get=Get_NewEnum))
    IUnknownPtr _NewEnum;

    //
    // Wrapper methods for error-handling
    //

    IUnknownPtr Get_NewEnum ( );
    IScriptModulePtr GetItem (
        const _variant_t & Index );
    long GetCount ( );
    IScriptModulePtr Add (
        _bstr_t Name,
        VARIANT * Object = &vtMissing );

    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall get__NewEnum (
        IUnknown * * ppenumContexts ) = 0;
    virtual HRESULT __stdcall get_Item (
        VARIANT Index,
        struct IScriptModule * * ppmod ) = 0;
    virtual HRESULT __stdcall get_Count (
        long * plCount ) = 0;
    virtual HRESULT __stdcall raw_Add (
        BSTR Name,
        VARIANT * Object,
        struct IScriptModule * * ppmod ) = 0;
};

struct __declspec(uuid("70841c78-067d-11d0-95d8-00a02463ab28"))
IScriptError : IDispatch
{
    //
    // Property data
    //

    __declspec(property(get=GetNumber))
    long Number;
    __declspec(property(get=GetSource))
    _bstr_t Source;
    __declspec(property(get=GetDescription))
    _bstr_t Description;
    __declspec(property(get=GetHelpFile))
    _bstr_t HelpFile;
    __declspec(property(get=GetHelpContext))
    long HelpContext;
    __declspec(property(get=GetLine))
    long Line;
    __declspec(property(get=GetColumn))
    long Column;
    __declspec(property(get=GetText))
    _bstr_t Text;

    //
    // Wrapper methods for error-handling
    //

    long GetNumber ( );
    _bstr_t GetSource ( );
    _bstr_t GetDescription ( );
    _bstr_t GetHelpFile ( );
    long GetHelpContext ( );
    _bstr_t GetText ( );
    long GetLine ( );
    long GetColumn ( );
    HRESULT Clear ( );

    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall get_Number (
        long * plNumber ) = 0;
    virtual HRESULT __stdcall get_Source (
        BSTR * pbstrSource ) = 0;
    virtual HRESULT __stdcall get_Description (
        BSTR * pbstrDescription ) = 0;
    virtual HRESULT __stdcall get_HelpFile (
        BSTR * pbstrHelpFile ) = 0;
    virtual HRESULT __stdcall get_HelpContext (
        long * plHelpContext ) = 0;
    virtual HRESULT __stdcall get_Text (
        BSTR * pbstrText ) = 0;
    virtual HRESULT __stdcall get_Line (
        long * plLine ) = 0;
    virtual HRESULT __stdcall get_Column (
        long * plColumn ) = 0;
    virtual HRESULT __stdcall raw_Clear ( ) = 0;
};

struct __declspec(uuid("0e59f1d3-1fbe-11d0-8ff2-00a0d10038bc"))
IScriptControl : IDispatch
{
    //
    // Property data
    //

    __declspec(property(get=GetAllowUI,put=PutAllowUI))
    VARIANT_BOOL AllowUI;
    __declspec(property(get=GetUseSafeSubset,put=PutUseSafeSubset))
    VARIANT_BOOL UseSafeSubset;
    __declspec(property(get=GetModules))
    IScriptModuleCollectionPtr Modules;
    __declspec(property(get=GetError))
    IScriptErrorPtr Error;
    __declspec(property(get=GetCodeObject))
    IDispatchPtr CodeObject;
    __declspec(property(get=GetProcedures))
    IScriptProcedureCollectionPtr Procedures;
    __declspec(property(get=GetLanguage,put=PutLanguage))
    _bstr_t Language;
    __declspec(property(get=GetState,put=PutState))
    enum ScriptControlStates State;
    __declspec(property(get=GetSitehWnd,put=PutSitehWnd))
    long SitehWnd;
    __declspec(property(get=GetTimeout,put=PutTimeout))
    long Timeout;

    //
    // Wrapper methods for error-handling
    //

    _bstr_t GetLanguage ( );
    void PutLanguage (
        _bstr_t pbstrLanguage );
    enum ScriptControlStates GetState ( );
    void PutState (
        enum ScriptControlStates pssState );
    void PutSitehWnd (
        long phwnd );
    long GetSitehWnd ( );
    long GetTimeout ( );
    void PutTimeout (
        long plMilleseconds );
    VARIANT_BOOL GetAllowUI ( );
    void PutAllowUI (
        VARIANT_BOOL pfAllowUI );
    VARIANT_BOOL GetUseSafeSubset ( );
    void PutUseSafeSubset (
        VARIANT_BOOL pfUseSafeSubset );
    IScriptModuleCollectionPtr GetModules ( );
    IScriptErrorPtr GetError ( );
    IDispatchPtr GetCodeObject ( );
    IScriptProcedureCollectionPtr GetProcedures ( );
    HRESULT _AboutBox ( );
    HRESULT AddObject (
        _bstr_t Name,
        IDispatch * Object,
        VARIANT_BOOL AddMembers );
    HRESULT Reset ( );
    HRESULT AddCode (
        _bstr_t Code );
    _variant_t Eval (
        _bstr_t Expression );
    HRESULT ExecuteStatement (
        _bstr_t Statement );
    _variant_t Run (
        _bstr_t ProcedureName,
        SAFEARRAY * * Parameters );

    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall get_Language (
        BSTR * pbstrLanguage ) = 0;
    virtual HRESULT __stdcall put_Language (
        BSTR pbstrLanguage ) = 0;
    virtual HRESULT __stdcall get_State (
        enum ScriptControlStates * pssState ) = 0;
    virtual HRESULT __stdcall put_State (
        enum ScriptControlStates pssState ) = 0;
    virtual HRESULT __stdcall put_SitehWnd (
        long phwnd ) = 0;
    virtual HRESULT __stdcall get_SitehWnd (
        long * phwnd ) = 0;
    virtual HRESULT __stdcall get_Timeout (
        long * plMilleseconds ) = 0;
    virtual HRESULT __stdcall put_Timeout (
        long plMilleseconds ) = 0;
    virtual HRESULT __stdcall get_AllowUI (
        VARIANT_BOOL * pfAllowUI ) = 0;
    virtual HRESULT __stdcall put_AllowUI (
        VARIANT_BOOL pfAllowUI ) = 0;
    virtual HRESULT __stdcall get_UseSafeSubset (
        VARIANT_BOOL * pfUseSafeSubset ) = 0;
    virtual HRESULT __stdcall put_UseSafeSubset (
        VARIANT_BOOL pfUseSafeSubset ) = 0;
    virtual HRESULT __stdcall get_Modules (
        struct IScriptModuleCollection * * ppmods ) = 0;
    virtual HRESULT __stdcall get_Error (
        struct IScriptError * * ppse ) = 0;
    virtual HRESULT __stdcall get_CodeObject (
        IDispatch * * ppdispObject ) = 0;
    virtual HRESULT __stdcall get_Procedures (
        struct IScriptProcedureCollection * * ppdispProcedures ) = 0;
    virtual HRESULT __stdcall raw__AboutBox ( ) = 0;
    virtual HRESULT __stdcall raw_AddObject (
        BSTR Name,
        IDispatch * Object,
        VARIANT_BOOL AddMembers ) = 0;
    virtual HRESULT __stdcall raw_Reset ( ) = 0;
    virtual HRESULT __stdcall raw_AddCode (
        BSTR Code ) = 0;
    virtual HRESULT __stdcall raw_Eval (
        BSTR Expression,
        VARIANT * pvarResult ) = 0;
    virtual HRESULT __stdcall raw_ExecuteStatement (
        BSTR Statement ) = 0;
    virtual HRESULT __stdcall raw_Run (
        BSTR ProcedureName,
        SAFEARRAY * * Parameters,
        VARIANT * pvarResult ) = 0;
};

struct __declspec(uuid("8b167d60-8605-11d0-abcb-00a0c90fffc0"))
DScriptControlSource : IDispatch
{
    //
    // Wrapper methods for error-handling
    //

    // Methods:
    HRESULT Error ( );
    HRESULT Timeout ( );
};

struct __declspec(uuid("0e59f1da-1fbe-11d0-8ff2-00a0d10038bc"))
Procedure;
    // [ default ] interface IScriptProcedure

struct __declspec(uuid("0e59f1db-1fbe-11d0-8ff2-00a0d10038bc"))
Procedures;
    // [ default ] interface IScriptProcedureCollection

struct __declspec(uuid("0e59f1dc-1fbe-11d0-8ff2-00a0d10038bc"))
Module;
    // [ default ] interface IScriptModule

struct __declspec(uuid("0e59f1dd-1fbe-11d0-8ff2-00a0d10038bc"))
Modules;
    // [ default ] interface IScriptModuleCollection

struct __declspec(uuid("0e59f1de-1fbe-11d0-8ff2-00a0d10038bc"))
Error;
    // [ default ] interface IScriptError

struct __declspec(uuid("0e59f1d5-1fbe-11d0-8ff2-00a0d10038bc"))
ScriptControl;
    // [ default ] interface IScriptControl
    // [ default, source ] dispinterface DScriptControlSource

//
// Wrapper method implementations
//

#include "msscript2.cpp"

} // namespace MSScriptControl

#pragma pack(pop)
