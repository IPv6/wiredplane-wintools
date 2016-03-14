// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#if !defined(WINVER)
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define NTDDI_VERSION NTDDI_WINXP
#endif

// Windows Header Files:
//#include <windows.h>
#include <afxwin.h>

// TODO: reference additional headers your program requires here
