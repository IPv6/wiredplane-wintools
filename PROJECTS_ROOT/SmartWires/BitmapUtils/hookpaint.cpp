// Copyright (C) 2000 by Feng Yuan (www.fengyuan.com)

#include "stdafx.h"
#include <assert.h>

#include "hookpaint.h"

bool Hook(const TCHAR * module, const TCHAR * proc, unsigned & syscall_id, BYTE * & pProc, const void * pNewProc)
{
    HINSTANCE hMod = GetModuleHandle(module);

    pProc = (BYTE *) GetProcAddress(hMod, proc);

    if ( pProc[0] == 0xB8 )
    {
        syscall_id = * (unsigned *) (pProc + 1);

        DWORD flOldProtect;

        VirtualProtect(pProc, 5, PAGE_EXECUTE_READWRITE, & flOldProtect);

        pProc[0] = 0xE9;
        * (unsigned *) (pProc+1) = (unsigned)pNewProc - (unsigned) (pProc+5);

        pProc += 5;

        return true;
    }
    else
        return false;
}


static unsigned syscall_BeginPaint = 0;
static BYTE *   pBeginPaint        = NULL;

static unsigned syscall_EndPaint   = 0;
static BYTE *   pEndPaint          = NULL;


LRESULT CPaintHook::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    assert(m_OldWndProc);

    if ( uMsg==WM_PRINTCLIENT )
    {
        m_hDC = (HDC) wParam;
        uMsg  = WM_PAINT;
    }
        
    LRESULT hRslt = CallWindowProc(m_OldWndProc, hWnd, uMsg, wParam, lParam);

    m_hDC = NULL;

    return hRslt;
}
	

HDC WINAPI CPaintHook::MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
    const CPaintHook * pThis = (CPaintHook *) GetWindowLong(hWnd, GWL_WNDPROC);
    
    pThis = (const CPaintHook *) ( (unsigned) pThis - (unsigned) & pThis->m_thunk[0] + (unsigned) pThis );
    
    if ( pThis->Within_WM_PRINT() )
    {
        memset(lpPaint, 0, sizeof(PAINTSTRUCT));

        lpPaint->hdc = pThis->m_hDC;
        
        GetClientRect(hWnd, & lpPaint->rcPaint);
        
        return pThis->m_hDC;
    }
    else
    {
        __asm   mov     eax, syscall_BeginPaint
        __asm   push    lpPaint
        __asm   push    hWnd
        __asm   call    pBeginPaint
    }
}


BOOL WINAPI CPaintHook::MyEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
    const CPaintHook * pThis = (CPaintHook *) GetWindowLong(hWnd, GWL_WNDPROC);
    
    pThis = (const CPaintHook *) ( (unsigned) pThis - (unsigned) & pThis->m_thunk[0] + (unsigned) pThis );
    
    if ( pThis->Within_WM_PRINT() )
        return TRUE;
    else
    {
        __asm   mov     eax, syscall_EndPaint
        __asm   push    lpPaint
        __asm   push    hWnd
        __asm   call    pEndPaint
    }
}

CPaintHook::CPaintHook()
{
    static bool s_hooked = false;

    if ( ! s_hooked )
    {
        Hook("USER32.DLL", "BeginPaint", syscall_BeginPaint, pBeginPaint, MyBeginPaint);
        Hook("USER32.DLL", "EndPaint",   syscall_EndPaint,   pEndPaint,   MyEndPaint);

        s_hooked = true;
    }

    m_thunk[0]              = 0xB9;	         // mov ecx, 
    *((DWORD *)(m_thunk+1)) = (DWORD) this;  //          this
	*((DWORD *)(m_thunk+5)) = 0x20FF018B;    // mov eax, [ecx] 

    m_OldWndProc = NULL;
    m_hDC        = NULL;
}


void CPaintHook::SubClass(HWND hWnd)
{		
    m_OldWndProc = (WNDPROC) GetWindowLong(hWnd, GWL_WNDPROC);
    SetWindowLong(hWnd, GWL_WNDPROC, (LONG) ((void *) m_thunk));
}

