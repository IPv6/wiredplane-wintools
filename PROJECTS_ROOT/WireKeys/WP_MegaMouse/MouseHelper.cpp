#include "StdAfx.h"
#include "resource.h"
#include <string>
#include <stdio.h>
#include "mousehelper.h"
#include "WP_MegaMouse.h"
#include "MouseHook.h"

HBRUSH g_hBgBrush = 0;
HANDLE g_hSemaphore;
extern HINSTANCE g_hinstDll;
HWND g_wnd;
MouseHelper* MouseHelper::m_pInstance = NULL;

// ************ Helper functions ****************************//
void inline CreateWKPBrush()
{
    LOGBRUSH brush;
    brush.lbColor = RGB(255, 255, 160);
    brush.lbHatch = 0;
    brush.lbStyle = BS_SOLID;
    g_hBgBrush = CreateBrushIndirect(&brush);
}

BOOL bFocusCatched=0;
char* _l(const char* szText, char* szOut, int size);
BOOL CALLBACK DialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
		{
			/*SetWindowLong(hwnd, 
            GWL_EXSTYLE, 
            GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hwnd, 0, (255 * 80) / 100, LWA_ALPHA);*/
			SetLayeredWindowStyle(hwnd,1);
			SetWndAlpha(hwnd,80,1);
			SetCapture(hwnd);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			char szBuf[256]={0};
			SetWindowText(hwnd,_l("ColorPickerWindow", szBuf, 256));
		}
        return TRUE;

    case WM_LBUTTONDOWN:
        MouseHelper::m_pInstance->ActivateDrag();

        return TRUE;

    case WM_LBUTTONUP:
        MouseHelper::m_pInstance->m_bIsDragMode = false;
        return TRUE;

    case WM_KEYDOWN:
        if (wParam == VK_ADD)
            MouseHelper::m_pInstance->IncreaseTransparency();
        else if (wParam == VK_SUBTRACT)
            MouseHelper::m_pInstance->DecreaseTransparency();
        else if (wParam == VK_ESCAPE)
            MouseHelper::m_pInstance->HidePluginWindow();
		else if (wParam == VK_MULTIPLY){
            MouseHelper::m_pInstance->m_options->iZoomOrder+=1;
			if(MouseHelper::m_pInstance->m_options->iZoomOrder>4 || MouseHelper::m_pInstance->m_options->iZoomOrder==3){
				MouseHelper::m_pInstance->m_options->iZoomOrder=4;
			}
			MouseHelper::m_pInstance->RedrawWindow();
		}else if (wParam == VK_DIVIDE){
            MouseHelper::m_pInstance->m_options->iZoomOrder-=1;
			if(MouseHelper::m_pInstance->m_options->iZoomOrder<1){
				MouseHelper::m_pInstance->m_options->iZoomOrder=1;
			}
			if(MouseHelper::m_pInstance->m_options->iZoomOrder==3){
				MouseHelper::m_pInstance->m_options->iZoomOrder=2;
			}
			MouseHelper::m_pInstance->RedrawWindow();
		}
        else if ((wParam >= VK_LEFT) && (wParam <= VK_DOWN))
            MouseHelper::m_pInstance->ModifyMousePosition((int)wParam);
        return TRUE;
    case WM_MOUSEMOVE:
        if (MouseHelper::m_pInstance->m_bIsDragMode)
            MouseHelper::m_pInstance->UpdateWindowPosition();
        else
            MouseHelper::m_pInstance->RedrawWindow();
		if(bFocusCatched==0){
			// Чтобы горячие клавиши ээкранной лупы начали работать прям сразу
			// переводим на это окно все фокусы
			// раньше это сделать нельзя, так как
			// смена Layered атрибута *сносит* активное окно
			SwitchToWindow(hwnd,1);
			bFocusCatched=1;
		}
        return FALSE;

    case WM_CTLCOLORDLG:
        if (!g_hBgBrush)
            CreateWKPBrush();
        return (BOOL)(LRESULT)g_hBgBrush;

    case WM_DESTROY:
		bFocusCatched=0;
        DeleteObject(g_hBgBrush);
        g_hBgBrush = 0;
        PostQuitMessage(0);
        return TRUE;

    case WM_CLOSE:
        ReleaseCapture();
        DestroyWindow(hwnd);
        g_wnd = 0;
        return TRUE;
    }

    return FALSE;
}

DWORD WINAPI Thread(LPVOID lpParameter)
{
    HWND hwnd=CreateDialog(g_hinstDll, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)DialogProc);
    g_wnd = hwnd;
    ShowWindowAsync(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    ReleaseSemaphore(g_hSemaphore, 1, NULL);
    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) 
    {
        if (bRet == -1)
        {
            return 1;
        }
        else
        {
            if (msg.message == WM_KEYDOWN)
                SendMessage(hwnd, msg.message, msg.wParam, msg.lParam);
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
			}
        }
    }

    return 0;
}
// ************* MouseHelper class ***************************//
MouseHelper::MouseHelper(COptions* pOptions)
{
	m_pErrorMessage=0;
    m_options = pOptions;
    m_hWindow = NULL;
    m_iXBeginPosition = 0;
    m_iYBeginPosition = 0;
    m_bIsDragMode = false;
    MouseHelper::m_pInstance = this;
    m_uiTransparency = 80;
}

MouseHelper::~MouseHelper(void)
{
    if (m_pErrorMessage){
        delete[] m_pErrorMessage;
		m_pErrorMessage=0;
	}
    
    MouseHelper::m_pInstance = NULL;
}

bool MouseHelper::CopyCurrentPixelColor()
{
    if (!OpenClipboard(GetDesktopWindow()))
        return false;
    EmptyClipboard();
	HGLOBAL m_hglbCopy=0;
    if (!GetCurrentPixelColor(m_hglbCopy))
        return false;
    SetClipboardData(CF_TEXT, m_hglbCopy);

    CloseClipboard();

    return true;
}


bool MouseHelper::ShowPluginWindow()
{
    if (m_hWindow){
        return false;
	}
    g_hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
    CreateThread(NULL,0,Thread,NULL,0,NULL);
    WaitForSingleObject(g_hSemaphore, 1000); // wait while window is created
    CloseHandle(g_hSemaphore);
    m_hWindow = g_wnd;
    g_wnd = 0;
    WINDOWPLACEMENT plc;
    GetWindowPlacement(m_hWindow, &plc);

    MoveWindow(m_hWindow,
               plc.rcNormalPosition.left,
               plc.rcNormalPosition.top,
               (m_options->pZoomWindowSize.x) + 90,
               (m_options->pZoomWindowSize.y) + 6, TRUE);
    RedrawWindow();

    return true;
}

bool MouseHelper::IsWindowVisible()
{
    return (m_hWindow != 0);
}

void MouseHelper::HidePluginWindow()
{
    if (m_hWindow)
    {
        SendMessage(m_hWindow, WM_CLOSE, 0, 0);
        m_hWindow = 0;
    }
}

void MouseHelper::ApplySettings(COptions* pOptions)
{
    if (IsWindowVisible())
        HidePluginWindow();
    // resize window if needed
    //if (pOptions->pZoomWindowSize.x != m_options->pZoomWindowSize.x)
    //{
    //    WINDOWPLACEMENT plc;
    //    GetWindowPlacement(m_hWindow, &plc);
    //    
    //    MoveWindow(m_hWindow,
    //        plc.rcNormalPosition.left,
    //        plc.rcNormalPosition.top,
    //        (m_options->pZoomWindowSize.x*m_options->iZoomOrder) + 90,
    //        (m_options->pZoomWindowSize.y*m_options->iZoomOrder) + 5, TRUE);
    //}
    m_options = pOptions;
}

bool MouseHelper::GetCurrentPixelColor(HGLOBAL& m_hglbCopy)
{
    LPTSTR lptstrCopy;
    HDC hDesktop = GetDC(NULL);
    POINT pt;

    GetCursorPos(&pt);
    COLORREF col = GetPixel(hDesktop, pt.x, pt.y);
    BYTE rVal = GetRValue(col);
    BYTE gVal = GetGValue(col);
    BYTE bVal = GetBValue(col);

    TCHAR buff[12];
	sprintf(buff, "#%02X%02X%02X", rVal, gVal, bVal);
    if (m_hglbCopy == 0)
        m_hglbCopy = GlobalAlloc(GMEM_MOVEABLE, 12*sizeof(TCHAR));

    lptstrCopy = (LPTSTR)GlobalLock(m_hglbCopy);
    memcpy(lptstrCopy, buff, 12*sizeof(TCHAR));
    GlobalUnlock(m_hglbCopy);

    ReleaseDC(GetDesktopWindow(), hDesktop);

    return true;
}

/*
void MouseHelper::SetLastError(TCHAR* szBuff)
{
    if (m_pErrorMessage){
        delete[] m_pErrorMessage;
		m_pErrorMessage=0;
	}

    int size = (int)strlen(szBuff);
    m_pErrorMessage = new TCHAR[size];
    memcpy(m_pErrorMessage, szBuff, size);
}
*/

void MouseHelper::UpdateWindowPosition()
{
    int m_nScreenX = GetSystemMetrics(SM_CXSCREEN); // You get the width of the users monitor
    int m_nScreenY = GetSystemMetrics(SM_CYSCREEN); // You get the height of the users monitor

    WINDOWPLACEMENT placement;

    if (!GetWindowPlacement(m_hWindow, &placement)) //Get placement of the dialog
        return;

    int nTop = placement.rcNormalPosition.top;
    int nBottom = placement.rcNormalPosition.bottom;
    int nLeft = placement.rcNormalPosition.left;
    int nRight = placement.rcNormalPosition.right;

    POINT ptGlobPos;
    if (!GetCursorPos(&ptGlobPos)) //Get coordinates of the mouse (complete screen)
        return;

    if ((!m_iXBeginPosition) || (!m_iYBeginPosition))
    {
        m_iXBeginPosition = ptGlobPos.x;
        m_iYBeginPosition = ptGlobPos.y;
    }

    int nDiffX = ptGlobPos.x - m_iXBeginPosition;    //Calculate the difference between old 
                                                //and new X pos of the mouse.
    
    int nDiffY = ptGlobPos.y - m_iYBeginPosition;    //Calculate the difference between old 
                                                //and new Y pos of the mouse.

    nLeft = nLeft + nDiffX; //Calculate the new windowposition
    nTop = nTop + nDiffY;
    nRight = nRight + nDiffX;
    nBottom = nBottom + nDiffY;

    int nWidth  = nRight-nLeft; //Calculate the width and height of the window
    int nHeight = nBottom-nTop;
    
    if (nLeft<0)
        nLeft = 0;
    if (nRight>m_nScreenX)
        nLeft = m_nScreenX - nWidth;
    if (nTop<0)
        nTop = 0;
    if (nBottom>m_nScreenY)
        nTop = m_nScreenY - nHeight;
    
    m_iXBeginPosition = ptGlobPos.x; //Save actual Mousepos for the next call of OnMouseMove()
    m_iYBeginPosition = ptGlobPos.y;
    
    MoveWindow(m_hWindow, nLeft, nTop, nWidth, nHeight, TRUE); //And finally move the window to new coordinates.
}

void MouseHelper::UpdateCoordinates(HDC dc, POINT pt, WINDOWPLACEMENT plc)
{
    RECT rc;
    int width = plc.rcNormalPosition.right - plc.rcNormalPosition.left;
    TCHAR buff[128]={0};
    sprintf(buff, "XY: %dx%d", pt.x, pt.y);
    rc.left = width - 75;
    rc.top = 5;
    rc.bottom = 20;
    rc.right = width - 5;
    FillRect(dc, &rc, g_hBgBrush);
    SetBkColor(dc, RGB(255, 255, 160));
    TextOut(dc, rc.left, rc.top, buff, (int)strlen(buff));
}

void MouseHelper::UpdateColor(HDC dc, POINT pt, WINDOWPLACEMENT plc)
{
    HDC hDesktop = GetDC(NULL);
    TCHAR buff[128]={0};
    RECT rc;

    // get pixel color in text format
    COLORREF col = GetPixel(hDesktop, pt.x, pt.y);
    BYTE rVal = GetRValue(col);
    BYTE gVal = GetGValue(col);
    BYTE bVal = GetBValue(col);
    //sprintf(buff, "#%02X%02X%02X", rVal, gVal, bVal);
	sprintf(buff, "H: #%02X%02X%02X", rVal, gVal, bVal);

    // draw string in plugin window
    int width = plc.rcNormalPosition.right - plc.rcNormalPosition.left;
    rc.left = width - 75;
    rc.top = 27;
    rc.bottom = 42+15;
    rc.right = width - 5;
    FillRect(dc, &rc, g_hBgBrush);
    SetBkColor(dc, RGB(255, 255, 160));
    TextOut(dc, rc.left, rc.top, buff, (int)strlen(buff));
	memset(buff,0,sizeof(buff));
	sprintf(buff, "D: %lu", RGB(rVal, gVal, bVal));
	rc.top += 15;
	TextOut(dc, rc.left, rc.top, buff, (int)strlen(buff));
	memset(buff,0,sizeof(buff));
	sprintf(buff, "(%03i,%03i,%03i)", rVal, gVal, bVal);
	rc.top += 15;
	TextOut(dc, rc.left, rc.top, buff, (int)strlen(buff));

    // draw sample rect
    LOGBRUSH brush;
    brush.lbColor = col;
    brush.lbHatch = 0;
    brush.lbStyle = BS_SOLID;
    HBRUSH hCurColor = CreateBrushIndirect(&brush);
    rc.top += 15;
    rc.bottom = rc.top + 31;
	rc.left-=9;
	rc.right+=1;
    FillRect(dc, &rc, hCurColor);

    // free resources
    ReleaseDC(GetDesktopWindow(), hDesktop);
    DeleteObject(hCurColor);
}

void MouseHelper::UpdateZoom(HDC dc, POINT pt, WINDOWPLACEMENT plc)
{
    if (!m_hWindow)
        return;

    int widthDest = m_options->pZoomWindowSize.x;
    int heightDest = m_options->pZoomWindowSize.y;
    int widthSrc = m_options->pZoomWindowSize.x/m_options->iZoomOrder;
    int heightSrc = m_options->pZoomWindowSize.y/m_options->iZoomOrder;

    HDC hdcScreen = GetWindowDC(GetDesktopWindow());
    HDC hdcCompatible = GetWindowDC(m_hWindow);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, widthDest, heightDest);
    SelectObject(hdcCompatible, hBitmap);
	RECT rt;
	rt.left=3;
	rt.top=3;
	rt.right=rt.left+widthDest;
	rt.bottom=rt.top+heightDest;
	FillRect(hdcCompatible, &rt, g_hBgBrush);
    StretchBlt(hdcCompatible, 3, 3, widthDest, heightDest, hdcScreen,
               pt.x - (widthSrc/2 + 1), pt.y - (heightSrc/2 + 1), widthSrc, heightSrc, SRCCOPY);

    DrawIcon(hdcCompatible, (widthDest/2 - 8), (heightDest/2 - 8),
             (HICON)LoadCursor(g_hinstDll, MAKEINTRESOURCE(IDC_TARGET)));

    DeleteObject(hBitmap);
    ReleaseDC(GetDesktopWindow(), hdcScreen);
    ReleaseDC(m_hWindow, hdcCompatible);
}

void MouseHelper::IncreaseTransparency()
{
    if (m_uiTransparency <= 99)
    {
        m_uiTransparency += 1;
		SetWndAlpha(m_hWindow,m_uiTransparency,1);
        //SetLayeredWindowAttributes(m_hWindow, 0, (255 * m_uiTransparency) / 100, LWA_ALPHA);
    }
}

void MouseHelper::DecreaseTransparency()
{
    if (m_uiTransparency > 20)
    {
        m_uiTransparency -= 1;
		SetWndAlpha(m_hWindow,m_uiTransparency,1);
        //SetLayeredWindowAttributes(m_hWindow, 0, (255 * m_uiTransparency) / 100, LWA_ALPHA);
    }
}

void MouseHelper::ModifyMousePosition(int action)
{
    POINT pt;
    GetCursorPos(&pt);

    if (action == VK_UP)
        pt.y--;
    else if (action == VK_DOWN)
        pt.y++;
    else if (action == VK_LEFT)
        pt.x--;
    else if (action == VK_RIGHT)
        pt.x++;

    SetCursorPos(pt.x, pt.y);
    if (!m_bIsDragMode)
        RedrawWindow();
}

void MouseHelper::RedrawWindow()
{
    HDC dc;
    POINT pt;
    WINDOWPLACEMENT plc;
    HFONT oldFont, newFont;
    GetWindowPlacement(m_hWindow, &plc);
    GetCursorPos(&pt);
    dc = GetWindowDC(m_hWindow);
    newFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
    oldFont = (HFONT)SelectObject(dc, (HGDIOBJ)newFont);

    if (m_options->bStickToMouse)
        MoveWindowToMouse(pt, plc);
    if (m_options->bColorVisible)
        UpdateColor(dc, pt, plc);
    if (m_options->bPositionVisible)
        UpdateCoordinates(dc, pt, plc);
    if (m_options->bZoomVisible)
        UpdateZoom(dc, pt, plc);

    SelectObject(dc, (HGDIOBJ)oldFont);
    ReleaseDC(m_hWindow, dc);
}

void MouseHelper::ActivateDrag()
{
    POINT ptGlobPos;
    if (!GetCursorPos(&ptGlobPos)) //Get coordinates of the mouse (complete screen)
        return;

    m_iXBeginPosition = ptGlobPos.x;
    m_iYBeginPosition = ptGlobPos.y;
    m_bIsDragMode = true;
}

void MouseHelper::MoveWindowToMouse(POINT pt, WINDOWPLACEMENT plc)
{
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    int width = plc.rcNormalPosition.right - plc.rcNormalPosition.left;
    int height = plc.rcNormalPosition.bottom - plc.rcNormalPosition.top;
    int x = pt.x + 8;
    if ((x + width) > screenX)
        x = screenX - width;
    int y = pt.y + 8;
    if ((y + height) > screenY)
        y = screenY - height;
    MoveWindow(m_hWindow, x, y, width, height, TRUE);
}