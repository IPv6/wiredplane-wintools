#include "stdafx.h"
#include "behavior_aux.h"

#include <math.h>

namespace htmlayout 
{

/*
BEHAVIOR: shell-icon
    goal: Renders icon associated with the file with the given extension.
TYPICAL USE CASE:
    <img class="shell-icon" filename="something.png" /> 
    will render icon for the png files registered in system if style of such image will be defined as:
STYLE:
    img.shell-icon { width:18px; height:18px; behavior:shell-icon;  } 
*/

struct shellicon: public behavior
{
    // ctor
    shellicon(): behavior(HANDLE_DRAW, "shell-icon") {}

    void draw_file_icon(HDC hdc, int x, int y, LPCWSTR filename)
    {
        SHFILEINFOW sfi;
        HIMAGELIST hlist = (HIMAGELIST) SHGetFileInfoW( filename, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
        if(hlist)
          ImageList_Draw(hlist, sfi.iIcon, hdc, x, y, ILD_TRANSPARENT); 
    }
    
    virtual BOOL on_draw   (HELEMENT he, UINT draw_type, HDC hdc, const RECT& rc ) 
    { 
      if( draw_type != DRAW_CONTENT )
        return FALSE; /*do default draw*/ 

      dom::element el = he;
      const wchar_t* filename = el.get_attribute("filename");
      if( !filename )
        return FALSE;  // no such attribute at all.

      int szx = GetSystemMetrics(SM_CXSMSIZE);
      int szy = GetSystemMetrics(SM_CYSMSIZE);

      int x = rc.left + (rc.right - rc.left - szx) / 2;
      int y = rc.top + (rc.bottom - rc.top - szy) / 2;

      draw_file_icon(hdc, x, y, filename);
           
      return TRUE; /*skip default draw as we did it already*/ 
    }

   
};

// instantiating and attaching it to the global list
shellicon shellicon_instance;

} // htmlayout namespace


