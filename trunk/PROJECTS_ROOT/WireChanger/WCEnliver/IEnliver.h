// IEnliver.h: interface for the CIEnliver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IENLIVER_H__1695BB30_D8F7_48A7_867F_74C0986B8FD9__INCLUDED_)
#define AFX_IENLIVER_H__1695BB30_D8F7_48A7_867F_74C0986B8FD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\..\SmartWires\BitmapUtils\Dib.h"
#include <math.h>
#define IMG_W_DEF 640
#define IMG_H_DEF 480

//#define IMG_W 320
//#define IMG_H 240

//#define IMG_W 1024
//#define IMG_H 768
class CEnlOption
{
public:
	DWORD dwQuality;
	DWORD dwImgW;
	DWORD dwImgH;
	DWORD dwTimeout;
	DWORD dwEffect;
};

class CIEnliver : public CEnlOption
{
public:
	DWORD* image;
	double a,b,c,t;
	double a1,b1,c1,t1;
	CIEnliver();
	BOOL bRunState;
	CDib dbBgImage;
	CSize szBitmapSize;
	virtual ~CIEnliver();
	HANDLE hSync;
	long dwXS,dwXS2,dwYS,dwYS2,dwStart;
	DWORD dwStepType;
};
typedef BOOL (WINAPI *_StartEnliver)();
typedef BOOL (WINAPI *_StopEnliver)();
typedef BOOL (WINAPI *_EnliverOptions)(HWND hParent);
typedef BOOL (WINAPI *_ChangeState)(BOOL bNewState);
typedef BOOL (WINAPI *_ChangeBackground)(HBITMAP& hNewBGBitmap, DWORD& dwOverlayBGColor);
#endif // !defined(AFX_IENLIVER_H__1695BB30_D8F7_48A7_867F_74C0986B8FD9__INCLUDED_)
