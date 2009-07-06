// Copyright (c) 2002 Andreas Jцnsson, www.AngelCode.com

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "IEnliver.h"
#include "overlay.h"
#include "image.h"
extern CIEnliver objSettings;
#define INITGUID
#include <objbase.h>
#include <initguid.h>
#include <ddraw.h>
// Global variables
IDirectDraw7        *ddraw      = 0;
IDirectDrawSurface7 *primary    = 0;
IDirectDrawSurface7 *overlay    = 0;
IDirectDrawSurface7 *backbuffer = 0;

// Function prototypes
HRESULT CreatePrimarySurface();
int     CopyImageToSurface(IDirectDrawSurface7 *surface);
DWORD   GetPixelValue(IDirectDrawSurface7 *surface, COLORREF rgb);
int     InitDDraw();
void    UninitDDraw();
int     CheckOverlaySupport();
int     CreateOverlay();
void    DestroyOverlay();
int     ShowOverlay();

//-------------------------------------------------------------------

int InitOverlay()
{
	// Initialize DirectDraw
	if( InitDDraw() < 0 )
	{
		::MessageBox(0, "Failed to initialize DirectDraw.", "Error", MB_OK);
		return -1;
	}

	// Check overlay support
	if( CheckOverlaySupport() < 0 )
	{
		::MessageBox(0, "Your hardware doesn't support the overlay surfaces we are trying to create.", "Error", MB_OK);
		return -1;
	}

	// Create the overlay surface
	if( CreateOverlay() < 0 )
	{
		::MessageBox(0, "Failed to create the overlay surface.", "Error", MB_OK);
		return -1;
	}

	// Show the overlay surface
	if( ShowOverlay() < 0 )
	{
		::MessageBox(0, "Failed to show the overlay surface", "Error", MB_OK);
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------

void UninitOverlay()
{
	DestroyOverlay();

	UninitDDraw();
}

//-------------------------------------------------------------------
HINSTANCE hDirectDrawDLL=NULL;
typedef HRESULT (WINAPI *_DirectDrawCreateEx)( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );
_DirectDrawCreateEx fp=NULL;
int InitDDraw()
{
    HRESULT hr;

    // Create the DirectDraw object 
	hDirectDrawDLL=LoadLibrary("ddraw.dll");
	if(!hDirectDrawDLL){
		goto ErrorOut;
	}
	fp=NULL;
	fp=(_DirectDrawCreateEx)GetProcAddress(hDirectDrawDLL,"DirectDrawCreateEx");
	if(!fp){
		goto ErrorOut;
	}
    hr = (*fp)(NULL, (VOID**)&ddraw, IID_IDirectDraw7, NULL);
    if( FAILED(hr) )
        goto ErrorOut;

	// Since we aren't going fullscreen we should not use exclusive mode
    // For NORMAL cooperative level we don't need to provide a HWND.
    hr = ddraw->SetCooperativeLevel(NULL, DDSCL_NORMAL);
    if( FAILED(hr) )
        goto ErrorOut;

	// Obtain a reference for the primary surface that represents 
	// the entire screen. 
    hr = CreatePrimarySurface();
    if( FAILED(hr) )
        goto ErrorOut;

	return 0;

ErrorOut:
	UninitDDraw();

	return -1;
}

//-------------------------------------------------------------------

void UninitDDraw()
{
	if(primary)
    {
        primary->Release();
        primary = 0;
    }
    
    if(ddraw)
    {
        ddraw->Release();
        ddraw = 0;
    }
	if(hDirectDrawDLL){
		FreeLibrary(hDirectDrawDLL);
	}
}

//-------------------------------------------------------------------

HRESULT CreatePrimarySurface()
{
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;
    
    if( !ddraw ) 
		return E_FAIL;
    
	// Create the primary surface which is the entire screen.
	// In this demo we will not draw on this surface directly, 
	// but we still need it for reference.
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    hr = ddraw->CreateSurface(&ddsd, &primary, 0);
    
    return hr;
}

//-------------------------------------------------------------------

int CheckOverlaySupport()
{
    DDCAPS  capsDrv;
    HRESULT hr;
    
    // Get driver capabilities to determine overlay support.
    memset(&capsDrv, 0, sizeof(capsDrv));
	capsDrv.dwSize = sizeof(capsDrv);
    hr = ddraw->GetCaps(&capsDrv, 0);
    if( FAILED(hr) )
        return -1;
    
    // Does the driver support overlays in the current mode? 
    if( !(capsDrv.dwCaps & DDCAPS_OVERLAY) )
        return -2;

	// Make sure support for stretching is available
	if( !(capsDrv.dwCaps & DDCAPS_OVERLAYSTRETCH) )
		return -3;

	// Make sure the hardware is able to stretch the overlay
	// to the size of the screen. The stretch capabilities may
	// vary with resolution and refresh rate. Only the x axis
	// must be verified. The stretch factor is multiplied with 
	// 1000 to keep three decimal digits 
	UINT stretch = GetSystemMetrics(SM_CXSCREEN)*1000/objSettings.dwImgW;
	if( capsDrv.dwMinOverlayStretch > stretch )
		return -4;

	if( capsDrv.dwMaxOverlayStretch < stretch )
		return -5;
	
	// Check alignment restrictions (only x axis needed)
	// Src rect (0, 0, objSettings.dwImgW, objSettings.dwImgH)
	// Dst rect (0, 0, screenW, screenH)
//	if( capsDrv.dwCaps & DDCAPS_ALIGNBOUNDARYDEST && 0 % capsDrv.dwAlignBoundaryDest != 0 )
//	if( capsDrv.dwCaps & DDCAPS_ALIGNBOUNDARYSRC && 0 % capsDrv.dwAlignBoundarySrc != 0 )
	if( capsDrv.dwCaps & DDCAPS_ALIGNSIZEDEST && GetSystemMetrics(SM_CXSCREEN) % capsDrv.dwAlignSizeDest != 0 )
		return -6;
	if( capsDrv.dwCaps & DDCAPS_ALIGNSIZESRC && objSettings.dwImgW % capsDrv.dwAlignSizeSrc != 0 )
		return -7;

	// Does the overlay hardware support dest color-keying? We will
	// use destination color keying to mask out the background 
	// behind the windows.
    if( !(capsDrv.dwCKeyCaps & DDCKEYCAPS_DESTOVERLAY) )
        return -8;
    
    return 0;
}

//-------------------------------------------------------------------
/* Full list from http://www.geocrawler.com/archives/3/9376/2001/12/0/7334013/
 {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, {24},   {0xff0000},   {0x00ff00},   {0x0000ff},   {0x000000}},   // R8G8B8
  {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, {32},   {0x00ff0000}, {0x0000ff00}, {0x000000ff}, {0xff000000}}, // A8R8G8B8 
  {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, {32},   {0x00ff0000}, {0x0000ff00}, {0x000000ff}, {0x00000000}}, // X8R8G8B8 
  {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, {16},   {0xf800},     {0x07e0},     {0x001f},     {0x0000}},     // R5G6B5 
  {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, {16},   {0x7c00},     {0x03e0},     {0x001f},     {0x0000}},     // X1R5G5B5 
  {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, {16},   {0x7c00},     {0x03e0},     {0x001f},     {0x8000}},     // A1R5G6B5 
  {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, {16},   {0x0f00},     {0x00f0},     {0x000f},     {0xf000}},     // A4R4G4B4 
  {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, {8},   {0xe0},       {0x1c},       {0x03},       {0x00}},       // R3G3B2 
  {sizeof(DDPIXELFORMAT), DDPF_ALPHA, 0, {8},   {0x00},       {0x00},       {0x00},       {0xff}},       // A8 
  {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, {16},   {0x00e0},     {0x001c},     {0x0003},     {0xff00}},     // A8R3G3B2 
  {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, {16},   {0x0f00},     {0x00f0},     {0x000f},     {0x0000}},     // X4R4G4B4 
  {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, {32},   {0x3ff00000}, {0x000ffc00}, {0x000003ff}, {0xc0000000}}, // A2B10G10R10 
  {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, {32},   {0x0000ffff}, {0xffff0000}, {0x00000000}, {0x00000000}}, // G16R16 
  {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_PALETTEINDEXED8 | DDPF_ALPHAPIXELS, 0, {16},   {0x0000},     {0x0000},     {0x0000},     {0xff00}},     // A8P8 
  {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_PALETTEINDEXED8, 0, {8},   {0x00},       {0x00},       {0x00},       {0x00}},       // P8 
  {sizeof(DDPIXELFORMAT), DDPF_LUMINANCE, 0, {8},  {0xff},       {0x00},       {0x00},       {0x00}},       // L8 
  {sizeof(DDPIXELFORMAT), DDPF_LUMINANCE | DDPF_ALPHAPIXELS, 0, {16},   {0x00ff},     {0x0000},     {0x0000},     {0xff00}},     // A8L8 
  {sizeof(DDPIXELFORMAT), DDPF_LUMINANCE | DDPF_ALPHAPIXELS, 0, {8},   {0x0f},       {0x00},       {0x00},       {0xf0}},       // A4L4 
  {sizeof(DDPIXELFORMAT), DDPF_BUMPDUDV, 0, {16},   {0x00ff},     {0xff00},     {0x0000},     {0x0000}},     // V8U8 
  {sizeof(DDPIXELFORMAT), DDPF_BUMPDUDV | DDPF_BUMPLUMINANCE, 0, {16},   {0x001f},     {0x03e0},     {0xfc00},     {0x0000}},     // L6V5U5 
  {sizeof(DDPIXELFORMAT), DDPF_BUMPDUDV | DDPF_BUMPLUMINANCE, 0, {32},   {0x000000ff}, {0x0000ff00}, {0x00ff0000}, {0x00000000}}, // X8L8V8U8 
  {0}, // Q8L8V8U8 - what's that? 
  {sizeof(DDPIXELFORMAT), DDPF_BUMPDUDV, 0, {32},   {0x0000ffff}, {0xffff0000}, {0x00000000}, {0x00000000}}, // V16U16 
  {0}, // W11V11U10 - what's that? 
  {0}, // A2W10V10U10 - what's that? 
  {sizeof(DDPIXELFORMAT), DDPF_ZBUFFER, 0, {16},   {0},          {0xffff},     {0x0000},     {0x0000}},     // D16_LOCKABLE 
  {sizeof(DDPIXELFORMAT), DDPF_ZBUFFER, 0, {32},   {0},          {0xffffffff}, {0x00000000}, {0x0000}},     // D32 
  {sizeof(DDPIXELFORMAT), DDPF_ZBUFFER | DDPF_STENCILBUFFER, 0, {16},   {1},          {0xfffe},     {0x0001},     {0x0000}},     // D15S1 
  {sizeof(DDPIXELFORMAT), DDPF_ZBUFFER | DDPF_STENCILBUFFER, 0, {32},   {8},          {0xffffff00}, {0x000000ff}, {0x0000}},     // D24S8 
  {sizeof(DDPIXELFORMAT), DDPF_ZBUFFER, 0, {32},   {8},          {0xffffff00}, {0x00000000}, {0x0000}},     // D24X8 
  {sizeof(DDPIXELFORMAT), DDPF_ZBUFFER | DDPF_STENCILBUFFER, 0, {32},   {8},          {0xffffff00}, {0x0000000f}, {0x0000}},     // D24X4S4 
  {sizeof(DDPIXELFORMAT), DDPF_ZBUFFER, 0, {16},   {0},          {0xffff},     {0x0000},     {0x0000}},     // D16 
*/
int CreateOverlay()
{
	if( ddraw == 0 || primary == 0 )
		return -1;

	// It is not possible to enumerate supported pixel format so we
	// will have to just try one by one until we find a working one

	// These are the formats we support. For overlay surfaces we
	// should support at least these formats. Some video accelerators
	// don't support YUV formats, but others support only YUV formats.
	DDPIXELFORMAT formats[] = 
	{
		{sizeof(DDPIXELFORMAT), DDPF_RGB   , 0,	{32},	{0x00FF0000},	{0x0000FF00},	{0x000000FF},	{0x00000000}},	// 32-bit
		{sizeof(DDPIXELFORMAT), DDPF_RGB   , 0,	{24},	{0xFF0000},		{0x00FF00},		{0x0000FF},		{0x000000}},		// 24-bit
		{sizeof(DDPIXELFORMAT), DDPF_RGB   , 0, 16,	0xF800,		0x07e0,		0x001F,		0},			    // 16-bit RGB 5:6:5
		{sizeof(DDPIXELFORMAT), DDPF_RGB   , 0, 16,	0x7C00,		0x03e0,		0x001F,		0},				// 16-bit RGB 5:5:5
		{sizeof(DDPIXELFORMAT), DDPF_FOURCC, MAKEFOURCC('U','Y','V','Y'),0,0,0,0,0},// UYVY
		{sizeof(DDPIXELFORMAT), DDPF_FOURCC, MAKEFOURCC('Y','U','Y','2'),0,0,0,0,0}// YUY2		
	};
	// The surface must be placed in video memory as the video 
	// accelerator must have the memory at hand when it sends the
	// signal to the monitor.

	// We will try to create it with one backbuffer for smoother 
	// animations, this will normally work unless we are asking for
	// too much memory.
	DDSURFACEDESC2 ddsdOverlay;
	memset(&ddsdOverlay, 0, sizeof(ddsdOverlay));
	ddsdOverlay.dwSize = sizeof(ddsdOverlay);
	ddsdOverlay.ddsCaps.dwCaps    = DDSCAPS_OVERLAY | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
    ddsdOverlay.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_BACKBUFFERCOUNT | DDSD_PIXELFORMAT;
    ddsdOverlay.dwWidth           = objSettings.dwImgW;
    ddsdOverlay.dwHeight          = objSettings.dwImgH;
    ddsdOverlay.dwBackBufferCount = 1;
	ddsdOverlay.dwAlphaBitDepth   = 0;

	HRESULT hr=0;
	for( int n = 0; n < 5; n++ )
	{
		ddsdOverlay.ddpfPixelFormat = formats[n];
		hr = ddraw->CreateSurface(&ddsdOverlay, &overlay, 0);
		//DERR_INVALIDOBJECT
		//DDERR_NODIRECTDRAWHW
		//DDERR_NOCOOPERATIVELEVELSET
		//-2005532527==0x88760091
		DWORD dwCErr=DDERR_INVALIDPIXELFORMAT;//DDERR_NOCOOPERATIVELEVELSET;
		DWORD dwMyErr=hr;
		if( SUCCEEDED(hr) ){
			break;
		}
	}

	// We weren't able to create a flippable overlay surface
	// we could try to create a non-flippable surface, but I'll
	// leave that as an exercise to the interested developer
	if( FAILED(hr) )
		return -1;

	// Get a pointer to the backbuffer that we will later draw to
	DDSCAPS2 caps;
	caps.dwCaps = DDSCAPS_BACKBUFFER; 
	caps.dwCaps2 = caps.dwCaps3 = caps.dwCaps4 = 0;
	hr = overlay->GetAttachedSurface(&caps, &backbuffer);
	if( FAILED(hr) )
		return -1;

	// Initialize the front buffer
	hr = CopyImageToSurface(overlay);
	if( FAILED(hr) )
		return -1;

	return 0;
}

//-------------------------------------------------------------------

int CopyImageToSurface(IDirectDrawSurface7 *surface)
{
	DDSURFACEDESC2 ddsd;
	HRESULT hr;
	
	// Lock the surface so that we can write to it
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	hr = surface->Lock(0, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, 0);
	if( FAILED(hr) )
		return -1;
	// Check surface format
	// Для 32-ух битного формата - просто блиттим
	if( ddsd.ddpfPixelFormat.dwRGBBitCount==32){
		// The surface is in normal 32bit RGB format 
		DWORD w       = ddsd.dwWidth;
		DWORD h       = ddsd.dwHeight;
		int pitch     = ddsd.lPitch / 2;
		DWORD *dest   = (DWORD*)ddsd.lpSurface;
		if(w==objSettings.dwImgW && h==objSettings.dwImgH){
			memcpy(dest,objSettings.image,sizeof(DWORD)*objSettings.dwImgW*objSettings.dwImgH);
		}else{
			for( DWORD y = 0; y < h; y++ ){
				for( DWORD x = 0; x < w; x++ ){
					*dest = objSettings.image[x+y*objSettings.dwImgW];
					dest++;
				}
			}
		}
	}else if( ddsd.ddpfPixelFormat.dwFlags == DDPF_FOURCC ){
		// The surface is in one of the YUV formats we specified.
		// These YUV formats have three components Y, U and V, where
		// Y is sampled for every pixel and U and V is sampled only
		// every second pixel, thus every pair of pixels share U and V
		// values.		
		int w         = ddsd.dwWidth;
		int h         = ddsd.dwHeight;
		int pitch     = ddsd.lPitch;
		BYTE *dest    = (BYTE*)ddsd.lpSurface;

		for( int y = 0; y < h; y++ )
		{
			// We'll update two pixels at a time
			for( int x = 0; x < w; x += 2 )
			{
				// NOTE: The values used for the conversion between 
				// RGB and YUV are probably not the exact values, but
				// for this sample app they are good enough

				// We are using pure grayscale image which would allow
				// for plenty of optimizations but I'll show the complete
				// formula for those interested in showing color images
				COLORREF clr = objSettings.image[x+y*objSettings.dwImgW];
				int iR=GetRValue(clr);
				int iG=GetGValue(clr);
				int iB=GetBValue(clr);
				//                          red     green     blue
				BYTE Y0 = (BYTE)(        0.29*iR + 0.59*iG + 0.14*iB);
				BYTE U0 = (BYTE)(128.0 - 0.14*iR - 0.29*iG + 0.43*iB);
				BYTE V0 = (BYTE)(128.0 + 0.36*iR - 0.29*iG - 0.07*iB);
	
				clr = objSettings.image[x+1+y*objSettings.dwImgW];
				BYTE Y1 = (BYTE)(        0.29*iR + 0.57*iG + 0.14*iB);
				BYTE U1 = (BYTE)(128.0 - 0.14*iR - 0.29*iG + 0.43*iB);
				BYTE V1 = (BYTE)(128.0 + 0.36*iR - 0.29*iG - 0.07*iB);

				if( ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('Y','U','Y','2') )
				{
					*dest++ = Y0;
					*dest++ = (U0 + U1)/2;
					*dest++ = Y1;
					*dest++ = (V0 + V1)/2;
				}
				else
				{
					*dest++ = (U0 + U1)/2;
					*dest++ = Y0;
					*dest++ = (V0 + V1)/2;
					*dest++ = Y1;
				}
			}

			dest += (pitch - w*2);
		}
	}else{
		// The surface is in normal 16bit RGB format 
		int w         = ddsd.dwWidth;
		int h         = ddsd.dwHeight;
		int pitch     = ddsd.lPitch / 2;
		WORD *dest    = (WORD*)ddsd.lpSurface;
		WORD pixel;

		for( int y = 0; y < h; y++ )
		{
			for( int x = 0; x < w; x++ )
			{
				COLORREF clr = objSettings.image[x+y*objSettings.dwImgW];
				BYTE iR=GetRValue(clr);
				BYTE iG=GetGValue(clr);
				BYTE iB=GetBValue(clr);
				if( ddsd.ddpfPixelFormat.dwRGBBitCount==32){
					pixel = (WORD)clr;
				}else if( ddsd.ddpfPixelFormat.dwGBitMask == 0x03e0 ){
					// RGB 5:5:5
					pixel = WORD(((iR>>3)<<10) + ((iG>>3)<<5) + (iB>>3));
				}else{
					// RGB 5:6:5
					pixel = WORD(((iR>>3)<<11) + ((iG>>2)<<5) + (iB>>3));
				}
				*dest++ = pixel;
			}
			dest += (pitch - w);
		}
	}

	surface->Unlock(0);

	return 0;
}

//-------------------------------------------------------------------

void DestroyOverlay()
{
	// Release the backbuffer
	if( backbuffer != 0 )
	{
		backbuffer->Release();
		backbuffer = 0;
	}

	// Release the overlay surface
    if( overlay )
    {
        // Use UpdateOverlay() with the DDOVER_HIDE flag 
        // to remove an overlay from the display.
        overlay->UpdateOverlay(0, primary, 0, DDOVER_HIDE, 0);
        overlay->Release();
        overlay = 0;
    }	
} 

//-------------------------------------------------------------------

DWORD GetPixelValue(IDirectDrawSurface7 *surface, COLORREF rgb)
{
    COLORREF       oldPixel;
    HDC            dc;
    DWORD          pixel = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;

	if( rgb == CLR_INVALID )
		return CLR_INVALID;

	// We will use GDI to put a pixel of the wanted color on the
	// surface as GDI automatically transforms the color into one
	// that exists in the palette.
	hr = surface->GetDC(&dc);
    if( SUCCEEDED(hr) )
    {
		// Save current pixel value
        oldPixel = GetPixel(dc, 0, 0);     

		// Set our value
        SetPixel(dc, 0, 0, rgb);
		       
        surface->ReleaseDC(dc);
    }

    // Now lock the surface so we can read back the converted color
    ddsd.dwSize = sizeof(ddsd);
	hr = surface->Lock(0, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, 0);
	if( SUCCEEDED(hr) )
	{
		// Read a DWORD from the surface
        pixel = *(DWORD *)ddsd.lpSurface;                 
		
		// Mask out bits that are not part of the pixel
        if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 )
            pixel &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;  
        
		surface->Unlock(NULL);
    }

    // Restore the original pixel
	hr = surface->GetDC(&dc);
    if( SUCCEEDED(hr) )
    {
        SetPixel(dc, 0, 0, oldPixel);
        surface->ReleaseDC(dc);
    }

    return pixel;
}

//-------------------------------------------------------------------

int ShowOverlay()
{
	if( ddraw == 0 || primary == 0 || overlay == 0 )
		return -1;

	// Destination rectangle is the full screen
	RECT rcDst;
	rcDst.left   = 0;
	rcDst.top    = 0;
	rcDst.right  = GetSystemMetrics(SM_CXSCREEN);
	rcDst.bottom = GetSystemMetrics(SM_CYSCREEN);

	// Source rectangle is the full overlay surface
	RECT rcSrc;
	rcSrc.left   = 0;
	rcSrc.top    = 0;
	rcSrc.right  = objSettings.dwImgW;
	rcSrc.bottom = objSettings.dwImgH;

	// Set the destination color key. Note that the value
	// must have the same pixelformat as the destination surface
	DDOVERLAYFX fx;
	memset(&fx, 0, sizeof(fx));
	fx.dwSize = sizeof(fx);
	fx.dckDestColorkey.dwColorSpaceLowValue  = GetPixelValue(primary, COLORKEY);
	fx.dckDestColorkey.dwColorSpaceHighValue = GetPixelValue(primary, COLORKEY);

	DWORD flags = DDOVER_SHOW | DDOVER_DDFX | DDOVER_KEYDESTOVERRIDE;

	// Show the overlay surface
	HRESULT hr;
	hr = overlay->UpdateOverlay(&rcSrc, primary, &rcDst, flags, &fx);
	if( FAILED(hr) )
		return -1;

	return 0;
}

//-------------------------------------------------------------------

int UpdateOverlay()
{
	// Make sure we haven't lost the surface
	if( primary == 0 || primary->IsLost() != DD_OK )
	{
		// Lost the primary surface, which normally means another
		// application stole it from us. If we loose the primary
		// surface we might as well assume we've lost the overlay
		// surface too. At this moment we could try to recreate
		// the surface, or tell the application to try again later.
	  	return -1;
	}

	// Copy the image to the backbuffer
	CopyImageToSurface(backbuffer);

	// Show the backbuffer
	HRESULT hr = overlay->Flip(NULL, DDFLIP_WAIT);
	if( FAILED(hr) )
		return -2;

	return 0;
}