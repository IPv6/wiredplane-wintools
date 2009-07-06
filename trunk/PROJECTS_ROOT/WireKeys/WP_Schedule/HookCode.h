// HookCode.h: interface for the CHookCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
#define AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
typedef int (WINAPI *FPAVER_AHWInit)();
typedef int (WINAPI *FPAVER_AHWFree)();
typedef int (WINAPI *FPAVER_IsRemoteDataReady)();
typedef BYTE (WINAPI *FPAVER_GetRemoteData)();
typedef int (WINAPI *FPAVER_SetRemoteFlag)(int);
typedef int (WINAPI *FPAVER_GetRemoteFlag)();
#define MAX_KEYS	100
struct COptions
{
	COptions(){
		szIniFilePath[0]=0;
		RC_LowGpioPin=0x01;
		RC_HighGpioPin=0x0d;
		RC_ResetGpioPin=0x0f;
		RC_StatusGpioPin=0x0e;
		bIgnoreUnknownKeys=0;
		RC_AndMask=0xFFEF;
		RC_PressedBit=4;
		bSkipFirst=1;
		iUseRepeatBlock=0;
		iPressTime=500;
		bDebugMode=0;
		dwPrevMode=-1;
		dwModeStates[0]=dwModeStates[1]=dwModeStates[2]=dwModeStates[3]=0;
		for(int i=0;i<MAX_KEYS;i++){
			memset(szButtonKeys[i],0,16);
			memset(szButtonValues[i],0,16);
			memset(szButtonDesc[i],0,16);
		}
	};
	DWORD dwModeStates[4];
	long dwPrevMode;
	BOOL bDebugMode;
	BOOL bSkipFirst;
	BOOL bIgnoreUnknownKeys;
	char szIniFilePath[MAX_PATH];
	long RC_LowGpioPin;
	long RC_HighGpioPin;
	long RC_ResetGpioPin;
	long RC_StatusGpioPin;
	long RC_PressedBit;
	long RC_AndMask;
	char szButtonKeys[MAX_KEYS][16];
	char szButtonValues[MAX_KEYS][16];
	char szButtonDesc[MAX_KEYS][16];
	long iUseRepeatBlock;
	long iPressTime;
};
#define KBBUFFER_SIZE			256
#define KBBUFFER_DSIZE			KBBUFFER_SIZE*2
#define VK_FAKEKEY				KBBUFFER_DSIZE-12// Особая клавиша - ничего не делает, лишь сбрасывает состояние кнопок движения мыши и др.
#endif // !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
