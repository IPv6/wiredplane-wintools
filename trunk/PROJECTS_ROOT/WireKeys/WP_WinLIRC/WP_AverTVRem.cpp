// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_AverTVRem.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _DEBUG
#define TRACETHREAD	TRACE3("\nCheckpoint: Thread: 0x%04x, file=%s, line=%lu\n",::GetCurrentThreadId(),__FILE__,__LINE__);
#else
#define TRACETHREAD
#endif

#define FORK(func,param) \
	{\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func, LPVOID(param), 0, &dwThread);\
		::CloseHandle(hThread);\
	}


BOOL LoadAverLib();
BOOL UnLoadAverLib();
CString _l(const char* szText);
WKCallbackInterface*& WKGetPluginContainer();
extern FPAVER_IsRemoteDataReady _IsRemoteDataReady;
extern FPAVER_GetRemoteData _GetRemoteData;
extern FPAVER_SetRemoteFlag _SetRemoteFlag;
extern FPAVER_GetRemoteFlag _GetRemoteFlag;
extern HANDLE hStopEvent;
//extern HINSTANCE g_hinstDll;
extern HANDLE hHookerThread;
extern COptions plgOptions;
extern CRITICAL_SECTION csMainThread;
char* GetSubString(const char* szFrom, const char* szBegin, char cEnd, char* szTarget, const char* szDefaultValue="", int iOutBufLen=16)
{
	strcpy(szTarget,szDefaultValue);
	char* szPos=(char*)strstr(szFrom,szBegin);
	if(szPos){
		szPos=szPos+strlen(szBegin);
		char* szEnd=strchr(szPos,cEnd);
		if(szEnd){
			int iLen=szEnd-szPos;
			if(iLen>iOutBufLen){
				iLen=iOutBufLen-1;
			}
			memcpy(szTarget,szPos,iLen);
			szTarget[iLen]=0;
		}
	}
	return szTarget;
}

BOOL GetIniFile(BOOL bForWinLIRC, char* szInit)
{
	if(bForWinLIRC){
		WKGetPluginContainer()->GetPreferencesFolder(szInit,MAX_PATH);
		strcat(szInit,"gpio_winlirc.txt");
	}else{
		strcpy(szInit,plgOptions.szIniFilePath);
		if(plgOptions.szIniFilePath[0]==0){
			WKGetPluginContainer()->GetWKPluginsDirectory(szInit,MAX_PATH);
			strcat(szInit,"gpio_avertv.txt");
		}
	}
	return 1;
}

BOOL LoadConfig(BOOL bForWinLIRC)
{
	if(plgOptions.bNoAver && !bForWinLIRC){
		return 0;
	}
	char szTmp[256]={0},szClass[128]={0};
	strcpy(szClass,_l("Remote buttons"));
	if(bForWinLIRC){
		char szTmp2[64]={0};
		strcpy(szTmp2,_l("Reset buttons"));
		WKGetPluginContainer()->RegisterEvent("IRM5",szTmp2,szClass);
	}
	char szInit[MAX_PATH]={0};
	GetIniFile(bForWinLIRC,szInit);
	HANDLE hFile = ::CreateFile(szInit, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if (INVALID_HANDLE_VALUE != hFile){
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);
		DWORD dwRead=0;
		{// Считываем, возможно - дефолтные значения
			char* szBuffer=new char[dwFileSize+10];
			memset(szBuffer,'\n',dwFileSize+10);
			::ReadFile(hFile, szBuffer, dwFileSize, &dwRead, NULL);
			szBuffer[dwRead+1]=0;// Автоматом припишется \n
			char* szPos=szBuffer;
			while(*szPos){
				if(*szPos=='\r' || *szPos=='\t'){
					*szPos='\n';
				}
				szPos++;
			}
			if(!bForWinLIRC){
				plgOptions.RC_LowGpioPin=atol(GetSubString(szBuffer,"LowGpioPin=",'\n',szTmp,"0"));
				plgOptions.RC_HighGpioPin=atol(GetSubString(szBuffer,"HighGpioPin=",'\n',szTmp,"15"));
				plgOptions.RC_PressedBit=atol(GetSubString(szBuffer,"PressFlagPin=",'\n',szTmp,"4"));
				plgOptions.RC_AndMask=atol(GetSubString(szBuffer,"AndMask=",'\n',szTmp,"65519"));//0xFFEF;
				plgOptions.RC_StatusGpioPin=atol(GetSubString(szBuffer,"StatusGpioPin=",'\n',szTmp,"14"));
				plgOptions.RC_ResetGpioPin=atol(GetSubString(szBuffer,"ResetGpioPin=",'\n',szTmp,"2"));
				plgOptions.bSkipFirst=atol(GetSubString(szBuffer,"SkipFirstButton=",'\n',szTmp,"1"));
				plgOptions.bIgnoreUnknownKeys=atol(GetSubString(szBuffer,"IgnoreUnknownButtons=",'\n',szTmp,"0"));
				//RC_LowGpioPin=0x01;
				//RC_HighGpioPin=0x0d;
				//RC_StatusGpioPin=0x0e;
				//RC_ResetGpioPin=0x0f;
				//---------------
			}
			plgOptions.iUseRepeatBlock=atol(GetSubString(szBuffer,"BlockAutoRepeats=",'\n',szTmp,"1"));
			plgOptions.iPressTime=atol(GetSubString(szBuffer,"TimeToHoldKeyDown=",'\n',szTmp,"500"));
			if(!bForWinLIRC){
				for(int iCount=0;iCount<MAX_KEYS;iCount++){
					char szKeyNames[32]={0};
					// Код клавиши в видe 0xBC33
					sprintf(szKeyNames,"ID_Button_%lu=",iCount+1);
					strcpy(plgOptions.szButtonKeys[iCount],GetSubString(szBuffer,szKeyNames,'\n',szTmp,""));
					if(strlen(plgOptions.szButtonKeys[iCount])>0){
						sprintf(szKeyNames,"DSC_Button_%lu=",iCount+1);
						strcpy(plgOptions.szButtonDesc[iCount],GetSubString(szBuffer,szKeyNames,'\n',szTmp,""));
						sprintf(szKeyNames,"IR%02lu",iCount);
						strcpy(plgOptions.szButtonValues[iCount],szKeyNames);
						WKGetPluginContainer()->RegisterEvent(plgOptions.szButtonValues[iCount],plgOptions.szButtonDesc[iCount],szClass);
					}
				}
			}else{
				plgOptions.szWinLIRCKeys.RemoveAll();
				for(int iCount=0;iCount<1000;iCount++){
					char szKeyNames[32]={0};
					int iEventNum=plgOptions.szWinLIRCKeys.GetSize();
					sprintf(szKeyNames,"WL_Button_%lu=",iEventNum);
					szTmp[0]=0;
					GetSubString(szBuffer,szKeyNames,'\n',szTmp,"");
					if(strlen(szTmp)>0){
						plgOptions.szWinLIRCKeys.Add(szTmp);
						sprintf(szKeyNames,"WR%02X",iEventNum);
						WKGetPluginContainer()->RegisterEvent(szKeyNames,szTmp,szClass);
					}else{
						break;
					}

				}
			}
			delete[] szBuffer;
		}
		CloseHandle(hFile);
	}
	return TRUE;
}

BOOL UnLoadConfig(BOOL bForWinLIRC)
{
	if(bForWinLIRC){
		for(int iEventNum=0;iEventNum<plgOptions.szWinLIRCKeys.GetSize();iEventNum++){
			char szID[5]={0};
			sprintf(szID,"WR%02X",iEventNum);
			WKGetPluginContainer()->UnRegisterEvent(szID);
		}
		plgOptions.szWinLIRCKeys.RemoveAll();
		return 0;
	}
	if(plgOptions.bNoAver){
		return 0;
	}
	int iCount=0;
	while(iCount<MAX_KEYS){
		if(strlen(plgOptions.szButtonKeys[iCount])==0){
			break;
		}
		WKGetPluginContainer()->UnRegisterEvent(plgOptions.szButtonValues[iCount]);
		memset(plgOptions.szButtonValues[iCount],0,16);
		memset(plgOptions.szButtonKeys[iCount],0,16);
		memset(plgOptions.szButtonDesc[iCount],0,16);
		iCount++;
	}
	return TRUE;
}

DWORD WINAPI DepressKey(LPVOID pData)
{
	int iCount=(int)pData;
	Sleep(plgOptions.iPressTime);
	WKGetPluginContainer()->NotifyEventFinished(plgOptions.szButtonValues[iCount]);
	return 0;
}

WINOLEAPI  CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
DWORD WINAPI GlobalHooker_Aver(LPVOID pData)
{
	TRACETHREAD;
	::EnterCriticalSection(&csMainThread);
	::CoInitialize(NULL);
	if(!InitAPI()){
		plgOptions.bNoAver=1;
		::LeaveCriticalSection(&csMainThread);
		::CoUninitialize();
		//Нету авера тут....
		//AfxMessageBox(_l("Failed to find remote control drivers")+"!");
		return 0;
	}
	WriteGPIO(plgOptions.RC_ResetGpioPin,1);
	static BOOL bFirstTime=1;
	while(WaitForSingleObject(hStopEvent,200)!=WAIT_OBJECT_0){
		BOOL bNewData=(plgOptions.RC_StatusGpioPin>=0)?ReadGPIO(plgOptions.RC_StatusGpioPin):0;
		if(bNewData==0){
			BOOL bSkipThisKeys=0;
			int iKey=ReadGpioRange(plgOptions.RC_LowGpioPin,plgOptions.RC_HighGpioPin);
			if(plgOptions.bSkipFirst){
				static int iFirstPressedKey=0;
				if(bFirstTime){
					iFirstPressedKey=iKey;
				}
				if(iFirstPressedKey!=iKey){
					iFirstPressedKey=-1;
				}
				if(iFirstPressedKey==iKey){
					bSkipThisKeys=1;
				}
			}
			if(!plgOptions.bSkipFirst || (plgOptions.bSkipFirst && !bFirstTime)){
				BOOL bPressed=-1;
				static int iPreviousDepressed=0;
				if(plgOptions.RC_PressedBit>=0){
					bPressed=((iKey & (1<<(plgOptions.RC_PressedBit-plgOptions.RC_LowGpioPin)))!=0)?1:0;
					if(bPressed){
						iPreviousDepressed=0;
					}
				}
				static long iPreviousKey=-1;
				if(!bSkipThisKeys && (!plgOptions.iUseRepeatBlock || (plgOptions.iUseRepeatBlock && iKey!=iPreviousKey))){
					iPreviousKey=iKey;
					if(plgOptions.RC_PressedBit>0){
						iKey=(iKey & ~(1<<(plgOptions.RC_PressedBit-plgOptions.RC_LowGpioPin)));
					}
					char szKey[32]={0},szOSDText[128]={0};
					sprintf(szKey,"0x%04X",iKey);
					sprintf(szOSDText,"Remote key pressed: 0x%04X, Pressed=%i",iKey,bPressed);
					int iCount=0;
					BOOL bProcessAsUsual=1;
					while(iCount<MAX_KEYS){
						if(strlen(plgOptions.szButtonKeys[iCount])==0){
							if(plgOptions.bIgnoreUnknownKeys){
								bProcessAsUsual=0;
							}
							break;
						}
						if(strcmp(plgOptions.szButtonKeys[iCount],szKey)==0){
							if(strcmpi(plgOptions.szButtonValues[iCount],"ignore")!=0){
								if(bPressed==-1 || bPressed==1){
									WKGetPluginContainer()->NotifyEventStarted(plgOptions.szButtonValues[iCount]);
									WKGetPluginContainer()->PlayWavSound(plgOptions.szWav);
									strcpy(szOSDText,plgOptions.szButtonDesc[iCount]);
								}
								if(bPressed==-1 || bPressed==0){
									if(iPreviousDepressed!=iCount){
										iPreviousDepressed=iCount;
										FORK(DepressKey,iCount);
									}
								}
								if(bPressed==0){
									strcpy(szOSDText,"");
								}
								break;
							}else{
								bProcessAsUsual=0;
							}
						}
						iCount++;
					}
					if(!plgOptions.bIgnoreUnknownKeys && iCount>=MAX_KEYS){
						bProcessAsUsual=0;
					}
					if(plgOptions.bDebugMode && bProcessAsUsual){
						if(plgOptions.dwPrevMode!=-1 && szOSDText[0]!=0){
							char szModeNum[]=" ";
							szModeNum[0]=char('1'+plgOptions.dwPrevMode);
							strcat(szOSDText," (");
							strcat(szOSDText,_l("mode"));
							strcat(szOSDText," #");
							strcat(szOSDText,szModeNum);
							strcat(szOSDText,")");
						}
						if(szOSDText[0]!=0){
							WKGetPluginContainer()->ShowOSD(szOSDText,(bPressed==-1)?3*plgOptions.iPressTime:5000);
						}
					}
				}
			}
			bFirstTime=0;
			WriteGPIO(plgOptions.RC_ResetGpioPin,1);
		}
	}
	::CoUninitialize();
	DeinitAPI();
	::LeaveCriticalSection(&csMainThread);
	return 0;
}

DWORD WINAPI GlobalHooker_Aver_averdll(LPVOID pData)
{
	::EnterCriticalSection(&csMainThread);
	OleInitialize(NULL);
	if(!LoadAverLib()){
		::LeaveCriticalSection(&csMainThread);
		AfxMessageBox(_l("Failed to find remote control drivers")+"!");
		return 0;
	}
	while(WaitForSingleObject(hStopEvent,200)!=WAIT_OBJECT_0){
		BOOL bNewData=TRUE;
		if(_IsRemoteDataReady){
			bNewData=((*_IsRemoteDataReady)()!=0);
		}
		if(bNewData){
			if(_SetRemoteFlag){
				(*_SetRemoteFlag)(1);
			}
			BYTE iKey=(*_GetRemoteData)();
			if(_SetRemoteFlag){
				(*_SetRemoteFlag)(0);
			}
			static int iPreviousKey=-1;
			if(iKey!=iPreviousKey){
				iPreviousKey=iKey;
				CString so;
				so.Format("AverKey %lu",iKey);
				AfxMessageBox(so);
			}
		}
	}
	UnLoadAverLib();
	OleUninitialize();
	::LeaveCriticalSection(&csMainThread);
	return 0;
}