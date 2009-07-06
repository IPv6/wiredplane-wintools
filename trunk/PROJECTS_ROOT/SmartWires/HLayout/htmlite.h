/** \mainpage Terra Informatica Lightweight Embeddable HTMLayout control.
 *
 * \section legal_sec In legalese
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * <a href="http://terrainformatica.com/htmlayout">HTMLayout Home</a>
 *
 * (C) 2003-2006, Terra Informatica Software, Inc. and Andrew Fedoniouk
 *
 * \section structure_sec Structure of the documentation
 *
 * See <a href="files.html">Files</a> section.
 **/

/*!\file
\brief HTMLite - Windowless but still interactive HTML/CSS engine. 
*/

#ifndef __htmlite_h__
#define __htmlite_h__

#include "htmlayout.h"

typedef void* HTMLITE;

typedef enum tagHPRESULT
{
  HPR_OK = 0,
  HPR_INVALID_HANDLE,
  HPR_INVALID_FORMAT,
  HPR_FILE_NOT_FOUND,
  HPR_INVALID_PARAMETER,
  HPR_INVALID_STATE, // attempt to do operation on empty document
} HPRESULT;


/** REFRESH_AREA notification.
 *
 * - HLN_REFRESH_AREA
 * 
 **/
#define HLN_REFRESH_AREA 0xAFF + 0x20

typedef struct tagNMHL_REFRESH_AREA
{
    NMHDR     hdr;              /**< Default WM_NOTIFY header, only code field is used in HTMLite */
    RECT      area;             /**< [in] area to refresh.*/
} NMHL_REFRESH_AREA, FAR *LPNMHL_REFRESH_AREA;

/** HLN_SET_TIMER notification.
 *
 * - HLN_SET_TIMER
 * 
 **/
#define HLN_SET_TIMER 0xAFF + 0x21

typedef struct tagNMHL_SET_TIMER
{
    NMHDR     hdr;              /**< Default WM_NOTIFY header, only code field is used in HTMLite */
    UINT_PTR  timerId;          /**< [in] id of the timer event.*/
    UINT      elapseTime;       /**< [in] elapseTime of the timer event, milliseconds. 
                                          If it is 0 then this timer has to be stoped. */
} NMHL_SET_TIMER, FAR *LPNMHL_SET_TIMER;


/** HLN_SET_CURSOR notification.
 *
 * - HLN_SET_CURSOR
 * 
 **/
#define HLN_SET_CURSOR 0xAFF + 0x22

typedef struct tagNMHL_SET_CURSOR
{
    NMHDR     hdr;              /**< Default WM_NOTIFY header, only code field is used in HTMLite */
    UINT      cursorId;         /**< [in] id of the cursor, .*/

} NMHL_SET_CURSOR, FAR *LPNMHL_SET_CURSOR;


/** Create instance of the engine 
 * \return \b HTMLITE, instance handle of the engine.
 **/
EXTERN_C  HTMLITE HLAPI HTMLiteCreateInstance();

/** Destroy instance of the engine
 * \param[in] hLite \b HTMLITE, handle.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteDestroyInstance(HTMLITE hLite);

/** Set custom tag value to the instance of the engine.
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] tag \b LPVOID, any pointer.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteSetTag(HTMLITE hLite, LPVOID tag);

/** Get custom tag value from the instance of the engine.
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] tag \b LPVOID*, pointer to value receiving tag value.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteGetTag(HTMLITE hLite, LPVOID *tag);

/** Load HTML from file 
 * \param[in] hLite \b HTMLITE, handle.
 * \param[out] path \b LPCWSTR, path or URL of the html file to load.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteLoadHtmlFromFile(HTMLITE hLite, LPCWSTR path);

/** Load HTML from memory buffer 
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] baseURI \b LPCWSTR, base url.
 * \param[in] dataptr \b LPCBYTE, pointer to the buffer
 * \param[in] datasize \b DWORD, length of the data in the buffer
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteLoadHtmlFromMemory(HTMLITE hLite,LPCWSTR baseURI, LPCBYTE dataptr, DWORD datasize);

/** Measure loaded HTML
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] viewWidth \b INT, width of the view area.
 * \param[in] viewHeight \b INT, height of the view area.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteMeasure(HTMLITE hLite,
          INT viewWidth,    // width of rendering area in pixels 
          INT viewHeight);  // height of rendering area in pixels

/** Render HTML
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] hdc \b HDC, device context
 * \param[in] x \b INT, x,y,sx and sy have the same meaning as rcPaint in PAINTSTRUCT
 * \param[in] y \b INT, 
 * \param[in] sx \b INT, 
 * \param[in] sy \b INT, "dirty" rectangle coordinates.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteRender(HTMLITE hLite, HDC hdc, 
          INT x,  // x position of area to render in pixels  
          INT y,  // y position of area to render in pixels  
          INT sx, // width of area to render in pixels  
          INT sy); // height of area to render in pixels  

/** Render HTML on 24bpp or 32bpp dib 
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] hbmp \b HBITMAP, device context
 * \param[in] x \b INT, 
 * \param[in] y \b INT, 
 * \param[in] sx \b INT, 
 * \param[in] sy \b INT, "dirty" rectangle coordinates.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteRenderOnBitmap(HTMLITE hLite, HBITMAP hbmp, 
          INT x,    // x position of area to render  
          INT y,    // y position of area to render  
          INT sx,   // width of area to render  
          INT sy);  // height of area to render  

/**This function is used in response to HLN_LOAD_DATA request. 
 *
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] uri \b LPCWSTR, URI of the data requested by HTMLayout.
 * \param[in] data \b LPBYTE, pointer to data buffer.
 * \param[in] dataSize \b DWORD, length of the data in bytes.
 *
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteSetDataReady(HTMLITE hLite, LPCSTR url, LPCBYTE data, DWORD dataSize);

/**Get minimum width of loaded document 
 * ATTN: for this method to work document shall have following style:
 *    html { overflow: none; }
 * Otherwize consider to use
 *    HTMLayoutGetScrollInfo( root, ... , LPSIZE contentSize );  
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteGetDocumentMinWidth(HTMLITE hLite, LPINT v);

/**Get minimum height of loaded document 
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteGetDocumentMinHeight(HTMLITE hLite, LPINT v);

/**Set media type for CSS engine, use this before loading the document
 * \See: http://www.w3.org/TR/REC-CSS2/media.html 
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteSetMediaType(HTMLITE hLite, LPCSTR mediatype);

/**Get root DOM element of loaded HTML document. 
 * \param[in] hLite \b HTMLITE, handle.
 * \param[out] phe \b HELEMENT*, address of variable receiving handle of the root element (<html>).
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteGetRootElement(HTMLITE hLite, HELEMENT* phe);


/** Find DOM element by point (x,y). 
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] x \b INT, x coordinate of the point.
 * \param[in] y \b INT, y coordinate of the point.
 * \param[in] phe \b HELEMENT*, address of variable receiving handle of the element or 0 if there are no such element.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteFindElement(HTMLITE hLite, INT x, INT y, HELEMENT* phe);

//|
//| Callback function type
//|
//| HtmLayout will call it for callbacks defined in htmlayout.h, e.g. NMHL_ATTACH_BEHAVIOR
//| 

typedef UINT CALLBACK HTMLITE_CALLBACK(HTMLITE hLite, LPNMHDR hdr);

/** Set callback function. 
 * \param[in] hLite \b HTMLITE, handle.
 * \param[in] cb \b HTMLITE_CALLBACK, address of callback function.
 * \return \b HPRESULT.
 **/
EXTERN_C  HPRESULT HLAPI HTMLiteSetCallback(HTMLITE hLite, HTMLITE_CALLBACK* cb);

EXTERN_C  HPRESULT HLAPI HTMLiteTraverseUIEvent(HTMLITE hLite, UINT evt, LPVOID eventCtlStruct, LPBOOL bOutProcessed );


#ifdef __cplusplus 

// C++ wrapper 

#include "htmlayout_behavior.hpp"

class  HTMLite
{
  HTMLITE                hLite;

  static UINT CALLBACK HTMLiteCB(HTMLITE hLite, LPNMHDR hdr)
  {
    HTMLite *pex = 0;
    HTMLiteGetTag(hLite, (void**)&pex);
    assert(pex);

    switch( hdr->code )
    {
      case HLN_LOAD_DATA: return pex->handleLoadUrlData( LPNMHL_LOAD_DATA(hdr) ); 
      case HLN_DATA_LOADED: return pex->handleUrlDataLoaded( LPNMHL_DATA_LOADED(hdr) ); 
      case HLN_ATTACH_BEHAVIOR: return pex->handleAttachBehavior( LPNMHL_ATTACH_BEHAVIOR(hdr) ); 
      case HLN_REFRESH_AREA: pex->handleRefreshArea( LPNMHL_REFRESH_AREA(hdr) ); break;
      case HLN_SET_TIMER: pex->handleSetTimer( LPNMHL_SET_TIMER(hdr) ); break;
      case HLN_SET_CURSOR: pex->handleSetCursor( LPNMHL_SET_CURSOR(hdr) ); break;
      case HLN_UPDATE_UI: pex->handleUpdate(); break;
    }
    return 0;
  }

public:
  HTMLite(const char* mediaType = "screen"):hLite(0) 
  {
    hLite = HTMLiteCreateInstance();
    assert(hLite);
    if(hLite)
    {
      // set tag 
      HTMLiteSetTag(hLite,this);
      //set media type
      HTMLiteSetMediaType(hLite, mediaType);
      // register callback
      HTMLiteSetCallback(hLite, &HTMLiteCB );
    }
  }
  ~HTMLite() { destroy(); }
  
  void destroy() 
  { 
    if(hLite) 
    {
      HTMLiteDestroyInstance(hLite);
      hLite = 0;
    }
  }
  bool load(LPCWSTR path)
  {
    HPRESULT hr = HTMLiteLoadHtmlFromFile(hLite, path);
    assert(hr == HPR_OK);
    return hr == HPR_OK;
  }

  bool load(LPCBYTE dataptr, DWORD datasize, LPCWSTR baseURI = L"")
  {
    HPRESULT hr = HTMLiteLoadHtmlFromMemory(hLite, baseURI, dataptr, datasize);
    assert(hr == HPR_OK);
    return hr == HPR_OK;
  }

  bool  measure(  int viewWidth,  // width of rendering area in device (physical) units  
                  int viewHeight) // height of rendering area in device (physical) units  
  {
    HPRESULT hr = HTMLiteMeasure(hLite, viewWidth, viewHeight);
    //assert(hr == HPR_OK); 
    return hr == HPR_OK;
  }

  bool  render(HDC hdc, int x, int y, int width, int height)
  {
    HPRESULT hr = HTMLiteRender(hLite, hdc, x, y, width, height);
    assert(hr == HPR_OK);
    return hr == HPR_OK;
  }

  bool  render(HBITMAP hbmp, int x, int y, int width, int height)
  {
    HPRESULT hr = HTMLiteRenderOnBitmap(hLite, hbmp, x, y, width, height);
    assert(hr == HPR_OK);
    return hr == HPR_OK;
  }

  bool  setDataReady(LPCSTR url, LPCBYTE data, DWORD dataSize)
  {
    HPRESULT hr = HTMLiteSetDataReady(hLite, url, data, dataSize);
    assert(hr == HPR_OK);
    return hr == HPR_OK;
  }
  
  // Get current document measured height for width
  // given in measure scaledWidth/viewportWidth parameters.
  // ATTN: You need call first measure to get valid result.
  // retunrn value is in screen pixels.

  int getDocumentMinHeight()
  {
    int v;
    HPRESULT hr = HTMLiteGetDocumentMinHeight(hLite, &v);
    assert(hr == HPR_OK); 
    return (hr == HPR_OK)? v:0;
  }

  // Get current document measured minimum (intrinsic) width.
  // ATTN: You need call first measure to get valid result.
  // return value is in screen pixels.

  int getDocumentMinWidth()
  {
    int v;
    HPRESULT hr = HTMLiteGetDocumentMinWidth(hLite, &v);
    assert(hr == HPR_OK); 
    return (hr == HPR_OK)?v:0;
  }

  HELEMENT getRootElement()
  {
    HELEMENT v;
    HPRESULT hr = HTMLiteGetRootElement(hLite, &v);
    assert(hr == HPR_OK); 
    return (hr == HPR_OK)?v:0;
  }
 

  // request to load data, override this if you need other data loading policy
  virtual UINT handleLoadUrlData(LPNMHL_LOAD_DATA pn) 
  {
    return 0; // proceed with default image loader
  }

  // data loaded
  virtual UINT handleUrlDataLoaded(LPNMHL_DATA_LOADED pn) 
  {
    return 0; // proceed with default image loader
  }

  // override this if you need other image loading policy
  virtual UINT handleAttachBehavior(LPNMHL_ATTACH_BEHAVIOR pn) 
  {
    htmlayout::event_handler *pb = htmlayout::behavior::find(pn->behaviorName, pn->element);
    if(pb) 
    {
      pn->elementTag = pb;
      pn->elementProc = htmlayout::event_handler::element_proc;
      pn->elementEvents = pb->subscribed_to;
      return TRUE;
    }
    return FALSE; // proceed with default image loader
  }

  virtual void handleRefreshArea( LPNMHL_REFRESH_AREA pn )
  {
    pn->area;//e.g. InvalidateRect(..., pn->area).
  }

  virtual void handleSetTimer( LPNMHL_SET_TIMER pn )
  {
    if( pn->elapseTime )
      ;// CreateTimerQueueTimer( this, pn->timerId, pn->elapseTime )
    else
      ;// DeleteTimerQueueTimer( .... )
  }

  virtual void handleSetCursor( LPNMHL_SET_CURSOR pn )
  {
    // (CURSOR_TYPE) pn->cursorId;
  }

  virtual void handleUpdate( )
  {
    //e.g. copy invalid area of pixel buffer to the screen.
  }


  // process mouse event,
  // see MOUSE_PARAMS for the meaning of parameters
  BOOL traverseMouseEvent( UINT mouseCmd, POINT pt, UINT buttons, UINT alt_state ) 
  {
    MOUSE_PARAMS mp; memset(&mp, 0, sizeof(mp));
    mp.alt_state = alt_state;
    mp.button_state = buttons;
    mp.cmd = mouseCmd;
    mp.pos_document = pt;
    BOOL result = FALSE;
    HLDOM_RESULT hr = HTMLiteTraverseUIEvent(hLite, HANDLE_MOUSE, &mp, &result );
    assert(hr == HLDOM_OK); hr;
    return result;
  }

  // process keyboard event,
  // see KEY_PARAMS for the meaning of parameters
  BOOL traverseKeyboardEvent( UINT keyCmd, UINT code, UINT alt_state ) 
  {
    KEY_PARAMS kp; memset(&kp, 0, sizeof(kp));
    kp.alt_state = alt_state;
    kp.key_code = code;
    kp.cmd = keyCmd;
    BOOL result = FALSE;
    HLDOM_RESULT hr = HTMLiteTraverseUIEvent(hLite, HANDLE_KEY, &kp, &result );
    return result;
  }

  // process timer event,
  BOOL traverseTimerEvent( UINT timerId ) 
  {
    KEY_PARAMS kp; memset(&kp, 0, sizeof(kp));
    BOOL result = FALSE;
    HLDOM_RESULT hr = HTMLiteTraverseUIEvent(hLite, HANDLE_TIMER, &timerId, &result );
    return result; // host must destroy timer event if result is FALSE.
  }

  

};
#endif //__cplusplus 


#endif