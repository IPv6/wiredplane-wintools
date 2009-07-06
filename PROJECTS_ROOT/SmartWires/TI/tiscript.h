#ifndef __tiscript_h__
#define __tiscript_h__

/*
 * Terra Informatica Script Engine.
 * 
 * TISCRIPT is a variation of ECMAScript.
 * Compiler, bytecoded VM, copying GC.
 * 
 * Created with the intention to be embedded in applications 
 * in 15 minutes. And by humans having their heads close to sphere 
 * and not to egg.
 *
 * Enjoy plain C API:
 * 
 * For C++ API see tiscript.hpp somewhere nearby.
 * 
 * License in plain English, if you wish:
 * ======================================
 * 
 * 1. We do not promise that this software works. (But if you find any bugs,
 *    please let us know!)
 * 2. We do not promise that this script engine conforms ECMAScript standard.
 *    It is close but is not ECMAScript, JavaScript or JScript. 
 * 3. You can use this software for whatever you want. You don't have to pay us.
 * 4. You may not pretend that you wrote this software.  Please do not 
 *    rename tiscript.dll or change copyright notes and this file.
 *    If you use it then it would be extremely nice if acknowledge somewhere
 *    that your application is using Terra Informatica Script Engine.
 *
 * Copyright 2001-2005, Andrew Fedoniouk, Terra Informatica Software,Inc.
 * 
 * e-mail of TIScript support is tiscript@terrainformatica.com 
 * and forums on terrainformatica.com. 
 *
 */

#include "json-value.h"

#ifdef TISCRIPT_EXPORTS
#define TISCRIPT_API __declspec(dllexport) __stdcall
#else
#define TISCRIPT_API __declspec(dllimport) __stdcall
#endif

/* C plain API start */

#ifdef __cplusplus
extern "C" {
#endif

typedef LPVOID HSCRIPT; // script engine instance handle


// "stream" functions
typedef int CALLBACK TIS_input(LPVOID tag);
typedef int CALLBACK TIS_output(LPVOID tag, int v);


// TIS_create_engine use_flags bits
#define USE_FILE_IO   0x00000001 // allow Stream.openFile()
#define USE_SOCKET_IO 0x00000002 // allow Stream.openSocket()
#define USE_EVAL      0x00000004 // allow eval(), load(), loadbc(), compile() in script

//
// TIS_create_engine - create instance of the engine
//
//   use_flags - or'ed USE_*** bits, see above
//   engine_tag - aux value-tag associated with the instance of the engine.
// returns:
//   engine handle or zero if not enough memory
//
HSCRIPT TISCRIPT_API TIS_create_engine(UINT use_flags, void* engine_tag, int prm_na);

//
// TIS_destroy_engine - destroys the engine 
//
VOID    TISCRIPT_API TIS_destroy_engine(HSCRIPT he);

//
// TIS_engine_tag - get value-tag associated with the engine.
//
LPVOID  TISCRIPT_API TIS_engine_tag(HSCRIPT he);

//
// TIS_set_std_streams - setup stdin, stdout and stderr for the script.
// see /doc/Global.htm and /doc/Stream for details
//
//   pstdin, pstdout, pstderr - input/output functions
//   tag_in, tag_out, tag_err - their aux value-tags
//
VOID    TISCRIPT_API TIS_set_std_streams(HSCRIPT he, 
                          LPVOID tag_in, TIS_input* pstdin, 
                          LPVOID tag_out, TIS_output* pstdout,
                          LPVOID tag_err, TIS_output* pstderr);

//
// TIS_eval - evaluate expresion(s) and return its result
//
// params:
//   sourcecode - stream input function
//   tag_sourcecode - aux value-tag passed to sourcecode function as is.
// returns: 
//   resulting value
// example:
//   eval( "1+1" ) will return integer 2 as HVALUE
//
BOOL  TISCRIPT_API TIS_eval( HSCRIPT he, LPVOID tag_sourcecode, TIS_input* sourcecode, /*out*/ JSON_VALUE* retval );

//
// TIS_load - load (compile and execute) sourcecode
//
// params:
//   sourcecode - stream input function
//   tag_sourcecode - aux value-tag passed to sourcecode function as is.
//   hypertextMode - if TRUE means hypertext mode of interpretation -
//                   some text with script included in <% %> brackets,
//                   PHP server page is a good example of the principle.
//                 - if FALSE means that stream contains pure script.
// returns: 
//   FALSE - if something went wrong (reason will be dumped into pstderr). 
//   TRUE - nirvana
//
BOOL    TISCRIPT_API TIS_load( HSCRIPT he, LPVOID tag_sourcecode, TIS_input* sourcecode, LPCWSTR streamName, BOOL hypertextMode);

//
// TIS_compile - compile sourcecode into bytecode.
//
// params: 
//   as usual except of out shall accept binary sequence (e.g. zero bytes)
//   if stream is file based then open it as fopen(..., "wb+");
// returns:
//   see TIS_load above.
//
BOOL    TISCRIPT_API TIS_compile( HSCRIPT he, 
                                LPVOID tag_in, TIS_input* in, LPCWSTR streamName,
                                LPVOID tag_out, TIS_output* out, BOOL hypertextMode); 

//
// TIS_loadbc - load and run bytecodes
//
// params:
//   bytecodes - stream input function, shall emit bytes
//   tag_bytecodes - aux value-tag passed to bytecodes function as is.
//   hypertextMode - see TIS_load above.
// returns: 
//   see TIS_load above.
//
BOOL    TISCRIPT_API TIS_loadbc( HSCRIPT he, LPVOID tag_bytecodes, TIS_input* bytecodes );

//
// TIS_call - call script function
//
// params:
//   funcpath - function path - global namespace relative name of the function, e.g.  
//              TIS_call(..., L"Math.abs", ...) will call script Math.abs function 
//              with the given arguments
//   argv - vector of arguments as HVALUEs,
//   argc - number of elements in argv.
// returns:
//   TRUE if finction was found.
//   FALSE if function was not found. Will not update retval in this case.
BOOL   TISCRIPT_API TIS_call( HSCRIPT he, LPCSTR funcpath, JSON_VALUE* argv, INT argc, JSON_VALUE* retval );

//
// typedef of C/C++ "native" functions to be called from script.
// Shall return TRUE if retval contains valid value, return FALSE toerror.
//
typedef VOID CALLBACK TIS_native_method_t( HSCRIPT he, LPVOID*  p_data_slot, JSON_VALUE* argv, INT argc, /*out*/ JSON_VALUE* retval );
typedef VOID CALLBACK TIS_native_property_t( HSCRIPT he, LPVOID*  p_data_slot, BOOL set, /*in-set/out-get*/ JSON_VALUE* val );

typedef VOID CALLBACK TIS_native_dtor_t( HSCRIPT he, LPVOID* p_data_slot_value );

struct TIS_method_def
{
  LPCSTR                  name;
  TIS_native_method_t*    method;
};

struct TIS_property_def
{
  LPCSTR                  name;
  TIS_native_property_t*  property;
};

struct TIS_class_def
{
  const char*         name;
  TIS_method_def*     methods;
  TIS_property_def*   properties;
  TIS_native_dtor_t*  dtor;
};


//
// TIS_define_class - register "native" class to for the script.
// 
// params: 
//   p_class_def - pointer to class defintion (above).
// returns:
//   TRUE - if class was added successfully to script namespace, FALSE otherwise.
//
BOOL    TISCRIPT_API TIS_define_class( HSCRIPT he, TIS_class_def* p_class_def);


//
// TIS_throw - throw error from method or property implementation code.
// 
// params: 
//   error, LPCWSTR - error message.
//

VOID    TISCRIPT_API TIS_throw( HSCRIPT he, LPCWSTR error);


/* C plain API end */

#ifdef __cplusplus
}
#endif


#endif 



