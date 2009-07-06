// IEnliver.h: interface for the CIEnliver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IENLIVER_H__16__INCLUDED_)
#define AFX_IENLIVER_H__16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Функции для библиотек-рендеров
#define CALLER_OPTION_WRAPTEXT	0x0001
class CCallbacks
{
public:
	virtual BOOL GetRndWallpaperPath(int iType, char*)=0;
	virtual BOOL AddActiveZone(CRect* rtZone, const char* szAction)=0;
	virtual BOOL ShowBaloon(const char* szText,const char* szTitle,DWORD dwTimeout=0)=0;
	virtual BOOL GenerateFullFeaturedWallpaper(DWORD dwIndex, HBITMAP& pBmpOut, CRect sizeOut, BOOL bEffects=0)=0;
	virtual long GetCallerOption(long lOptionID)=0;
	virtual char* GetWebPage(const char* szUrl)=0;
	virtual BOOL FreeChar(char* szChar)=0;
	virtual BOOL GetBaseURI(char* szPath, const char* szForWhat)=0;
	virtual BOOL RenderHTML(HDC dc,RECT rtArea,const char* szBlock, BOOL bAddActiveZonex = 0)=0;
};

class CImgLoader;
typedef long& (WINAPI *_GetApplicationLang)();
typedef CImgLoader*& (WINAPI *__bmpX)();
typedef int (WINAPI *_Handshake)(const char*, int iHash);
typedef int (WINAPI *_FindDupes)(const char* szList, HWND hParent);
typedef int (WINAPI *_SpecOptions)(DWORD dw);
typedef BOOL (WINAPI *_RenderScript)(HBITMAP& hBitmap, const char* szScriptContent, CCallbacks* pCallback);
typedef BOOL (WINAPI *_GetEditKey)(CCallbacks* pCallback, char* szOutKey, size_t dwOutKey, char* szOutTitle, size_t dwOutTitle);
typedef BOOL (WINAPI *_GetEditKeyX)(CCallbacks* pCallback, char* szOutKey, size_t dwOutKey, char* szOutTitle, size_t dwOutTitle, char* szOutOnDate, size_t dwOutOnDate);
typedef BOOL (WINAPI *_EditDateFile)(CCallbacks* pCallback, const char* szFile, HWND pParent);
#endif // !defined(AFX_IENLIVER_H__16__INCLUDED_)
