#pragma once
#include "windows.h"

// zoom order variants
// no zoom, 2x and 4x
#define ZO_NOZOOM 1
#define ZO_NORMAL 2
#define ZO_HIGH   4

// zoom window size variants
// 102x102, 152x152
#define ZS_NORMAL 100
#define ZS_LARGE  150

class COptions
{
public:
    bool  bPositionVisible;
    bool  bZoomVisible;
    int   iZoomOrder;
    POINT pZoomWindowSize;
    bool  bColorVisible;
    bool  bStickToMouse;
};

class MouseHelper
{
public:
    MouseHelper(COptions* pOptions);
    ~MouseHelper(void);

    //****** Plugin fucntions *******//
    
    bool static CopyCurrentPixelColor(); // copies color of current pixel to clipboard
    bool ShowPluginWindow();
    bool IsWindowVisible();
    void HidePluginWindow();
    void ApplySettings(COptions* pOptions);
    HWND GetWindow()
    {
        return m_hWindow;
    }
    
    friend BOOL CALLBACK DialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
    bool static GetCurrentPixelColor(HGLOBAL& m_hglbCopy);
    void SetLastError(TCHAR* szBuff);
    void UpdateWindowPosition();
    void UpdateCoordinates(HDC dc, POINT pt, WINDOWPLACEMENT plc);
    void UpdateColor(HDC dc, POINT pt, WINDOWPLACEMENT plc);
    void UpdateZoom(HDC dc, POINT pt, WINDOWPLACEMENT plc);
    void IncreaseTransparency();
    void DecreaseTransparency();
    void ModifyMousePosition(int action);
    void RedrawWindow();
    void MoveWindowToMouse(POINT pt, WINDOWPLACEMENT plc);
    void ActivateDrag();
    
protected:
    static MouseHelper* m_pInstance;
    COptions* m_options;
    HWND      m_hWindow;
    TCHAR*    m_pErrorMessage;
    int       m_iXBeginPosition;
    int       m_iYBeginPosition;
    bool      m_bIsDragMode;
    int       m_uiTransparency;
};