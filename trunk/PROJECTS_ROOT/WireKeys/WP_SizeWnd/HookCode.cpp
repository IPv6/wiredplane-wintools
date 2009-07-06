// HookCode.cpp: implementation of the CHookCode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HookCode.h"

// Nonshared variables
void ForceDebugBreak()
{
   __try { _asm { int 3 }; }
   __except(UnhandledExceptionFilter(GetExceptionInformation())) { }
}

//HINSTANCE g_hinstDll=NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain_(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		//g_hinstDll = hinstDll;
		break;
	case DLL_PROCESS_DETACH:
		// The calling process is detaching the DLL from its address space.
		break;
	case DLL_THREAD_ATTACH:
		// A new thread is being created in the current process.
		break;
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly.
		break;
	}
	return(TRUE);
}

