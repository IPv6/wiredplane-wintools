// Copyright (C) 2000 by Feng Yuan (www.fengyuan.com)

class CPaintHook
{
    BYTE      m_thunk[9];
    WNDPROC   m_OldWndProc;
    HDC       m_hDC;

    static HDC  WINAPI MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
    static BOOL WINAPI MyEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);

    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
public:
    
    bool Within_WM_PRINT(void) const
    {  
        if ( IsBadWritePtr((void *) this, sizeof(CPaintHook)) )
            return false;

        if ( (m_thunk[0]==0xB9) && ((* (unsigned *) (m_thunk+5))==0x20FF018B) )
            return m_hDC !=0;
        else
            return false;
    }    
    
    CPaintHook(void);

    void SubClass(HWND hWnd);
};


