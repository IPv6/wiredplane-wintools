//************************************************************************
// Generic Window Helper Class
//*************************************************************************
// This is a generic window helper class. (Uses MSAA)
// Given a window handle , 
// -> This class can attach to the specified handle
// -> Extracts all information from the window
// -> Supports Win32/Internet Explorer_Server/Symhtml/...hopefully everything
// -> SendMessage/PostMessage to any window element/object using object name
// -> Perform clicks / doubleclicks.
// -> Perform Screen captures (in Bitmap) for the specific window.
//*************************************************************************
//Author: vinith raj
//email : slackerboy3@gmail.com
//Revision : 1 : Jan/17/2007
//My website: http://www.slacksoft.org
//*************************************************************************


//*************************************************************************
//*************************************************************************

#ifndef _WindowHelperIncluded_
#define _WindowHelperIncluded_

#pragma once 
//*************************************************************************
//*************************************************************************
//IAcc/Com Headers
//OleAcc contains implementation for IAccessible Members
#include <OleAcc.h>
#include <vector>
#pragma comment(lib, "Oleacc.lib")

#include <ExDisp.h>
#include <comutil.h>

//Winspool contains the PrintWindow(...) API
#include <WinSpool.h>
#pragma comment(lib,"Winspool.lib")
//*************************************************************************
//*************************************************************************
//macros
#define FREEVARIANTARRAY(__ary,size) \
{\
	if(__ary)\
{\
	for(int i=0;i<size;i++)\
	VariantClear(&__ary[i]);\
	delete[] __ary;\
}\
}
//*************************************************************************
//*************************************************************************
//structure to hold window information
typedef struct  tagWindowHelperDataStructure
{
	CString szName;
	CString szRole;
	HWND hwndObject;
	DWORD dwRole;
	long lHeight,lwidth;
	POINT pxy_screen; //screen
	POINT pxy_cl; //corner
	POINT pxy; //median
	CString szValue;
	CString szDsc;

} WindowHelperDataStructure, *pWindowHelperDataStructure;
//*************************************************************************
//*************************************************************************
//Datatypes
typedef std::vector<pWindowHelperDataStructure> WindowHelperDataVector;
typedef std::vector<pWindowHelperDataStructure>::iterator WindowHelperDataVectorIter;

//*************************************************************************
//*************************************************************************
//The Class
class CWindowHelper
{

public:
	//ctor//Dtor
	CWindowHelper();
	~CWindowHelper();

	//Attach a window to this class
	HRESULT Attach(HWND /*Target Window*/);

	//Release and clear all objects.//prepare object for reattaching.
	HRESULT Release();

	//Get Parent
	HWND GetParent();

	//Get Handle
	HWND GetHandle(CString /* Object Name */);
	HWND GetHandle(CString /* Object Name */, DWORD   /*Object Role*/);
	HWND GetHandle(CString /* Object Name */, CString /*Object Role*/);

	//Click specified object
	HRESULT Click(CString /* Object Name*/);
	HRESULT Click(CString /* Object Name*/, DWORD   /*Object Role*/);
	HRESULT Click(CString /* Object Name*/, CString /*Object Role*/);

	//Double click specified object
	HRESULT DblClick(CString /*Object Name*/);
	HRESULT DblClick(CString /*Object Name*/, DWORD   /*Object Role*/);
	HRESULT DblClick(CString /*Object Name*/, CString /*Object Role*/);

	//Send message to object
	HRESULT SendMessageEx(CString /*Object Name*/, UINT, WPARAM, LPARAM);
	HRESULT SendMessageEx(CString /*Object Name*/,DWORD   /*Object Role*/ ,UINT, WPARAM, LPARAM);
	HRESULT SendMessageEx(CString /*Object Name*/,CString /*Object Role*/ ,UINT, WPARAM, LPARAM);

	//Post message to object
	HRESULT PostMessageEx(CString /*Object Name*/, UINT, WPARAM, LPARAM);
	HRESULT PostMessageEx(CString /*Object Name*/,DWORD   /*Object Role*/ ,UINT, WPARAM, LPARAM);
	HRESULT PostMessageEx(CString /*Object Name*/,CString /*Object Role*/ ,UINT, WPARAM, LPARAM);

	//Get ScreenShot
	HRESULT ConvertToBitmap(CString /*Object Name*/, CString /*filename*/);
	HRESULT ConvertToBitmap(CString /*Object Name*/, DWORD   /*dwRole*/ ,CString /*filename*/);
	HRESULT ConvertToBitmap(CString /*Object Name*/, CString /*dwRole*/ ,CString /*filename*/);

	//Get ScreenShots of all objects
	HRESULT CaptureAllObjectsAsBitmaps(CString /*Capture Folder*/);

	CString GetCombinedTextInfo();

private:

	HWND m_hwndSource;
	HWND m_hwndParent;
	long m_lChildCount;
	CComPtr<IAccessible> m_pAcc;
	WindowHelperDataVector m_AccObjects;
	CBitmap m_hBmpSource;

private:
	//parse window recursively
	HRESULT ParseWindow();
	HRESULT ParseWindow_Base(IAccessible*);
	HRESULT GetObjectInfo(IAccessible*, VARIANT, pWindowHelperDataStructure);
	
	//capture
	HRESULT GetCapture(HWND /*target Object*/, int,int,int,int, CString /*filename*/);
	HRESULT SaveAsBitmap(CString);
};
//*************************************************************************
//*************************************************************************

#endif //_WindowHelperIncluded
