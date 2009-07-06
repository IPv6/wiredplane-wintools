// IEnliver.cpp: implementation of the CIEnliver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WCEnliver.h"
#include "IEnliver.h"
#include "DLG_Options.h"
#include <atlbase.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int InitOverlay();
int ShowOverlay();
void UninitOverlay();
int CreateOverlay();
void DestroyOverlay();
CIEnliver objSettings;
CRITICAL_SECTION csMain;
DWORD WINAPI MainOVRThread(LPVOID pData)
{
	static long lLock=0;
	if(lLock>0){
		return 0;
	}
	::EnterCriticalSection(&csMain);
	srand(time(NULL));
	SimpleTracker tr(lLock);
	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_IDLE );
	while(1){
		WaitForSingleObject(objSettings.hSync,objSettings.dwTimeout);
		if(objSettings.bRunState==0){
			break;
		}
		HWND hWin=GetForegroundWindow();
		if(hWin==NULL || !IsZoomed(hWin)){
			// Animate the image
			UpdateImage();
			// Show the new frame
			if(UpdateOverlay()==-1){
				if(objSettings.bRunState){
					static DWORD dwTry=0;
					if(dwTry==0 || long(GetTickCount()-dwTry)>10000){
						dwTry=GetTickCount();
						UninitOverlay();
						if(InitOverlay()>=0){
							dwTry=0;
						}
					}
				}
			};
		}
	}
	::LeaveCriticalSection(&csMain);
	return 0;
}

BOOL WINAPI ChangeState(BOOL bNewState)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(objSettings.bRunState!=bNewState){
		if(objSettings.bRunState==0 && bNewState!=0){
			if( InitOverlay() < 0 ){
				return FALSE;
			}
		}
		if(objSettings.bRunState!=0 && bNewState==0){
			UninitOverlay();
		}
		objSettings.bRunState=bNewState;
		if(objSettings.bRunState){
			DWORD dwTID=0;
			HANDLE hMainThread=CreateThread(NULL,0,MainOVRThread,0,0,&dwTID);
			CloseHandle(hMainThread);
		}
		SetEvent(objSettings.hSync);
		if(!objSettings.bRunState){
			::EnterCriticalSection(&csMain);
			::LeaveCriticalSection(&csMain);
		}
	}
	return TRUE;
};

BOOL WINAPI ChangeBackground(HBITMAP& hNewBGBitmap, DWORD& dwOverlayBGColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	srand(time(NULL)+GetTickCount());
	dwOverlayBGColor=COLORKEY;
	objSettings.dbBgImage.Destroy();
	objSettings.szBitmapSize=GetBitmapSize(hNewBGBitmap);
	objSettings.dbBgImage.Create(objSettings.szBitmapSize.cx, objSettings.szBitmapSize.cy);
	CDC dcScreen,dcReal;
	dcScreen.CreateDC("DISPLAY", NULL, NULL, NULL);
	dcReal.CreateCompatibleDC(&dcScreen);
	HBITMAP hbmpTmp=(HBITMAP)CopyImage(hNewBGBitmap,IMAGE_BITMAP, objSettings.szBitmapSize.cx, objSettings.szBitmapSize.cy, 0);
	objSettings.dbBgImage.SetBitmap(&dcReal,hbmpTmp);
	DWORD dwErr=GetLastError();
	ClearBitmap(hbmpTmp);
	InitImage();
	return TRUE;
};

#define SAVE_REGKEY	"SOFTWARE\\WiredPlane\\WireChanger\\Enliver"
BOOL WINAPI StartEnliverRaw(BOOL bGetFromReg)
{
	::InitializeCriticalSection(&csMain);
	if(bGetFromReg){
		// Reading from registry...
		CRegKey key;
		BOOL bNoInit=TRUE;
		CEnlOption* pOpt=&objSettings;
		if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)==ERROR_SUCCESS && key.m_hKey!=NULL){
			DWORD lSize = sizeof(CEnlOption),dwType=0;
			if(RegQueryValueEx(key.m_hKey,"Options",NULL, &dwType,(LPBYTE)(pOpt), &lSize)==ERROR_SUCCESS){
				bNoInit=FALSE;
			}
		}
		if(bNoInit){
			pOpt->dwImgH=IMG_W_DEF;
			pOpt->dwImgW=IMG_H_DEF;
			pOpt->dwQuality=1;
			pOpt->dwTimeout=200;
			pOpt->dwEffect=0;
		}
	}
	//-----------------
	objSettings.image=new DWORD[objSettings.dwImgW*objSettings.dwImgH];
	memset(objSettings.image, 0, objSettings.dwImgW*objSettings.dwImgH*sizeof(objSettings.image[0]));
	InitImage();
	return TRUE;
};

BOOL WINAPI StartEnliver()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return StartEnliverRaw(TRUE);
}

BOOL WINAPI StopEnliver()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(!objSettings.image){
		return 0;
	}
	ChangeState(0);
	delete[] objSettings.image;
	objSettings.image=NULL;
	CRegKey key;
	if(key.Open(HKEY_CURRENT_USER, SAVE_REGKEY)!=ERROR_SUCCESS){
		key.Create(HKEY_CURRENT_USER, SAVE_REGKEY);
	}
	CEnlOption* pOpt=&objSettings;
	if(key.m_hKey!=NULL){
		RegSetValueEx(key.m_hKey,"Options",0,REG_BINARY,(BYTE*)(pOpt),sizeof(CEnlOption));
	}
	::DeleteCriticalSection(&csMain);
	return TRUE;
};


BOOL WINAPI EnliverOptions(HWND hParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BOOL bState=objSettings.bRunState;
	StopEnliver();
	//AfxMessageBox("Created by Ilja Razinkov @2008");
	CDLG_Options dlg;
	dlg.DoModal();
	StartEnliverRaw(FALSE);
	ChangeState(bState);
	return TRUE;
};

CIEnliver::CIEnliver()
{
	bRunState=0;
	dwTimeout=50;
	dwQuality=1;
	hSync=CreateEvent(NULL,FALSE,FALSE,"");
	dwImgW=IMG_W_DEF;
	dwImgH=IMG_H_DEF;
	dwEffect=0;
	image=0;
}

CIEnliver::~CIEnliver()
{
	StopEnliver();
	CloseHandle(hSync);
}
