// ISpellCheck.h: interface for the ISpellCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISPELLCHECK_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
#define AFX_ISPELLCHECK_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>

// function to be exported from dll to create instance of interface
#ifdef _EXPORTING // declare this in project settings for dll _only_
#	define DLL_DECLSPEC __declspec(dllexport)
#else
#	define DLL_DECLSPEC __declspec(dllimport)
#endif 

class ISpellCheck;

typedef ISpellCheck* (*PFNCREATE)(const char*, const char*); // function prototype
extern "C" DLL_DECLSPEC ISpellCheck* CreateSpellCheckInterface(const char* szAffPath, const char* szDicPath); // single exported function

// helper method
static ISpellCheck* CreateSpellCheckInterface(const char* szDllPath, const char* szAffPath, const char* szDicPath)
{
    ISpellCheck* pInterface = NULL;
    HMODULE hDll = LoadLibrary(szDllPath);
	
    if (hDll)
    {
        PFNCREATE pCreate = (PFNCREATE)GetProcAddress(hDll, "CreateSpellCheckInterface");
		
        if (pCreate)
            pInterface = pCreate(szAffPath, szDicPath);
    }
	
    return pInterface;
}

static BOOL IsSpellCheckDll(const char* szDllPath)
{
    HMODULE hDll = LoadLibrary(szDllPath);
	
    if (hDll)
    {
        PFNCREATE pCreate = (PFNCREATE)GetProcAddress(hDll, "CreateSpellCheckInterface");
		FreeLibrary(hDll);

		return (NULL != pCreate);
	}

	return FALSE;
}

class ISpellCheck
{
public:
    virtual void Release() = 0; // releases the interface
	
    // 
    virtual bool CheckSpelling(const char* szWord) = 0;
    virtual bool CheckSpelling(const char* szWord, char**& pSuggestions, int& nNumSuggestions) = 0;
	
    // frees a previously returned buffer and sets the ptr to NULL
    virtual void FreeSuggestions(char**& pSuggestions) = 0;
	
};

static void ReleaseSpellCheckInterface(ISpellCheck*& pInterface)
{
    if (pInterface)
    {
        pInterface->Release();
        pInterface = NULL;
    }
}

#endif // !defined(AFX_ISpellCheck_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
