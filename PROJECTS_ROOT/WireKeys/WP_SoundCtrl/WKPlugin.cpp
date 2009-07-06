// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_SoundCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COptions plgOptions;
WKCallbackInterface* pInter=NULL;
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	pInter=init;
	if(init){
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
	return 1;
}

int	WINAPI WKPluginStart()
{
	plgOptions.lStepCount=10;
	return 1;
}

int	WINAPI _WKPluginPause(BOOL bPaused)
{
	return 1;
}

int	WINAPI WKPluginStop()
{
	return 1;
}

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

CString _l(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

#define MAX_HK	9*5
int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>=MAX_HK || stuff==NULL){
		return 0;
	}
	stuff->bVisible=0;
	return 1;
}

#include <math.h>
#include <mmsystem.h>
#define MAX_MIXER_VOL	65535
#define MAX_MIXER_PAN	100
#pragma comment(lib, "Winmm.lib")
BOOL trigMute(DWORD dwFrom,DWORD dwWhat, const char* szName)
{
	CMixerBase mixer;
	if(0!=mixer.Init(dwFrom,dwWhat,MIXERCONTROL_CONTROLTYPE_MUTE))
	{// Замутаем звук...
		mixer.GetMixerControlValue();
		tMIXERCONTROLDETAILS_BOOLEAN *m_Value = (tMIXERCONTROLDETAILS_BOOLEAN *)mixer.GetValuePtr();
		UINT iCurPos=m_Value[0].fValue;
		iCurPos=iCurPos?0:1;
		m_Value[0].fValue=iCurPos;
		mixer.SetMixerControlValue();
		if(plgOptions.lShowOSD){
			CString sSplashText;
			sSplashText.Format("'%s' %s: %s",szName,_l("Mute"),iCurPos?"ON":"OFF");
			pInter->ShowOSD(sSplashText,1000);
		}
		return TRUE;
	}
	return FALSE;}

BOOL trigPan(DWORD dwFrom,DWORD dwWhat, int iStep, const char* szName)
{
	CMixerBase mixer;
	if(0!=mixer.Init(dwFrom,dwWhat,MIXERCONTROL_CONTROLTYPE_VOLUME))
	{// Колбасим миксер
		mixer.GetMixerControlValue();
		MIXERCONTROLDETAILS_UNSIGNED *m_Value = (MIXERCONTROLDETAILS_UNSIGNED *)mixer.GetValuePtr();
		UINT iCurPos=0;
		double bBalance=1;
		if(mixer.m_nChannels<2){
			return FALSE;
		}
		double dCurStep=(double(iStep*2)/100);
		double dwMaxVol=(double)max(m_Value[1].dwValue,m_Value[0].dwValue);
		double lDif=double(m_Value[0].dwValue)-double(m_Value[1].dwValue);
		double iSlider=lDif/dwMaxVol;
		iSlider+=dCurStep;
		if(iSlider>1){
			iSlider=1;
		}
		if(iSlider<-1){
			iSlider=-1;
		}
		// Процент...
		DWORD dwTotalPercents=DWORD(50*double(iSlider+1));
		// Новое значение...
		double lNewValue=dwMaxVol-fabs(dwMaxVol*iSlider);
		//----------------------
		if(iSlider<0){
			m_Value[0].dwValue=DWORD(lNewValue);
			m_Value[1].dwValue=DWORD(dwMaxVol);
		}else{
			m_Value[0].dwValue=DWORD(dwMaxVol);
			m_Value[1].dwValue=DWORD(lNewValue);
		}
		mixer.SetMixerControlValue();
		if(plgOptions.lShowOSD){
			CString sSplashText;
			sSplashText.Format("'%s' %s: %s %i%%, %s %i%%",szName,_l("Balance"),_l("Left"),dwTotalPercents,_l("Right"),100-dwTotalPercents);
			pInter->ShowOSD(sSplashText,1000);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL trigVolume(DWORD dwFrom,DWORD dwWhat, int iStep, const char* szName)
{
	CMixerBase mixer;
	if(0!=mixer.Init(dwFrom,dwWhat,MIXERCONTROL_CONTROLTYPE_VOLUME))
	{// Колбасим миксер - громкость
		mixer.GetMixerControlValue();
		MIXERCONTROLDETAILS_UNSIGNED *m_Value = (MIXERCONTROLDETAILS_UNSIGNED *)mixer.GetValuePtr();
		UINT iCurPos=0;
		double bBalance=1;
		if(mixer.m_nChannels<2){
			iCurPos=m_Value[0].dwValue;
		}else{
			if(m_Value[0].dwValue!=m_Value[1].dwValue){
				if(m_Value[0].dwValue>m_Value[1].dwValue){
					if(m_Value[0].dwValue>0){
						bBalance=double(m_Value[1].dwValue)/double(m_Value[0].dwValue);
					}
				}else{
					if(m_Value[1].dwValue>0){
						bBalance=-double(m_Value[0].dwValue)/double(m_Value[1].dwValue);
					}
				}
			}
			iCurPos=max(m_Value[0].dwValue,m_Value[1].dwValue);
		}
		const double d10=double(MAX_MIXER_VOL)/iStep;
		double dRounded=double(iCurPos)/d10;
		iCurPos=UINT(ceil(d10*dRounded)-1);
		long lRawDx=long(MAX_MIXER_VOL*(double(iStep)/100));
		if(long(iCurPos)+lRawDx>0 && long(iCurPos)+lRawDx<MAX_MIXER_VOL){
			iCurPos+=lRawDx;
		}else{
			if(long(iCurPos)+lRawDx<=0){
				iCurPos=0;
			}
			if(long(iCurPos)+lRawDx>MAX_MIXER_VOL){
				iCurPos=MAX_MIXER_VOL;
			}
		}
		if(mixer.m_nChannels<2){
			m_Value[0].dwValue=iCurPos;
		}else{
			m_Value[0].dwValue=iCurPos;
			m_Value[1].dwValue=iCurPos;
			if(bBalance!=1){
				if(bBalance>0){
					m_Value[1].dwValue=DWORD(bBalance*iCurPos);
				}else{
					m_Value[0].dwValue=DWORD(bBalance*iCurPos);
				}
			}
		}
		mixer.SetMixerControlValue();
		if(plgOptions.lShowOSD){
			CString sSplashText;
			sSplashText.Format("'%s' %s: %i%%",szName,_l("Volume"),int(double(iCurPos/100+1)/double(MAX_MIXER_VOL/100)*100));
			pInter->ShowOSD(sSplashText,1000);
		}
		return TRUE;
	}
	return FALSE;
}

char*	aDeviceNames[]={"Wave"								,"Synth"									,"Line In"								,"CD"										,"Aux"									,"Phone line"							,"Microphone"};
DWORD	aDeviceTypes[]={MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT	,MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER	,MIXERLINE_COMPONENTTYPE_SRC_LINE		,MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC	,MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY	,MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE	,MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE};
DWORD	aDeviceTypsR[]={MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS		,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS	,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS		,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS	,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS	,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS};
int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>=MAX_HK || stuff==NULL){
		return 0;
	}
	stuff->dwDefaultHotkey=0;
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY;
	switch(iPluginFunction){
	case 0:
		strcpy(stuff->szItemName,"Wave: Increase volume");
		break;
	case 1:
		strcpy(stuff->szItemName,"Wave: Decrease volume");
		break;
	case 2:
		strcpy(stuff->szItemName,"Wave: Add right channel");
		break;
	case 3:
		strcpy(stuff->szItemName,"Wave: Add left channel");
		break;
	case 4:
		strcpy(stuff->szItemName,"Wave: Mute");
		break;
	//-----------------------------------------
	case 5:
		strcpy(stuff->szItemName,"Synth: Increase volume");
		break;
	case 6:
		strcpy(stuff->szItemName,"Synth: Decrease volume");
		break;
	case 7:
		strcpy(stuff->szItemName,"Synth: Add right channel");
		break;
	case 8:
		strcpy(stuff->szItemName,"Synth: Add left channel");
		break;
	case 9:
		strcpy(stuff->szItemName,"Synth: Mute");
		break;
	//-----------------------------------------
	case 10:
		strcpy(stuff->szItemName,"Line In: Increase volume");
		break;
	case 11:
		strcpy(stuff->szItemName,"Line In: Decrease volume");
		break;
	case 12:
		strcpy(stuff->szItemName,"Line In: Add right channel");
		break;
	case 13:
		strcpy(stuff->szItemName,"Line In: Add left channel");
		break;
	case 14:
		strcpy(stuff->szItemName,"Line In: Mute");
		break;
	//-----------------------------------------
	case 15:
		strcpy(stuff->szItemName,"CD: Increase volume");
		break;
	case 16:
		strcpy(stuff->szItemName,"CD: Decrease volume");
		break;
	case 17:
		strcpy(stuff->szItemName,"CD: Add right channel");
		break;
	case 18:
		strcpy(stuff->szItemName,"CD: Add left channel");
		break;
	case 19:
		strcpy(stuff->szItemName,"CD: Mute");
		break;
	//-----------------------------------------
	case 20:
		strcpy(stuff->szItemName,"Aux: Increase volume");
		break;
	case 21:
		strcpy(stuff->szItemName,"Aux: Decrease volume");
		break;
	case 22:
		strcpy(stuff->szItemName,"Aux: Add right channel");
		break;
	case 23:
		strcpy(stuff->szItemName,"Aux: Add left channel");
		break;
	case 24:
		strcpy(stuff->szItemName,"Aux: Mute");
		break;
	//-----------------------------------------
	case 25:
		strcpy(stuff->szItemName,"Phone line: Increase volume");
		break;
	case 26:
		strcpy(stuff->szItemName,"Phone line: Decrease volume");
		break;
	case 27:
		strcpy(stuff->szItemName,"");//"Phone line: Add right channel"
		break;
	case 28:
		strcpy(stuff->szItemName,"");//"Phone line: Add left channel"
		break;
	case 29:
		strcpy(stuff->szItemName,"Phone line: Mute");
		break;
	//-----------------------------------------
	case 30:
		strcpy(stuff->szItemName,"Microphone: Increase volume");
		break;
	case 31:
		strcpy(stuff->szItemName,"Microphone: Decrease volume");
		break;
	case 32:
		strcpy(stuff->szItemName,"");
		break;
	case 33:
		strcpy(stuff->szItemName,"");
		break;
	case 34:
		strcpy(stuff->szItemName,"Microphone: Mute");
		break;
	//-----------------------------------------	
	default:
		return 0;
	}
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	static long lNoConcurrency=0;
	if(lNoConcurrency>0){
		return 0;
	}
	BOOL bRes=0;
	SimpleTracker cl(lNoConcurrency);
	switch(iPluginFunction%5)
	{
	case 0:
		bRes=trigVolume(aDeviceTypsR[int(iPluginFunction/5)],aDeviceTypes[int(iPluginFunction/5)],plgOptions.lStepCount,aDeviceNames[int(iPluginFunction/5)]);
		break;
	case 1:
		bRes=trigVolume(aDeviceTypsR[int(iPluginFunction/5)],aDeviceTypes[int(iPluginFunction/5)],-plgOptions.lStepCount,aDeviceNames[int(iPluginFunction/5)]);
		break;
	case 2:
		bRes=trigPan(aDeviceTypsR[int(iPluginFunction/5)],aDeviceTypes[int(iPluginFunction/5)],-plgOptions.lPanStepCount,aDeviceNames[int(iPluginFunction/5)]);
		break;
	case 3:
		bRes=trigPan(aDeviceTypsR[int(iPluginFunction/5)],aDeviceTypes[int(iPluginFunction/5)],plgOptions.lPanStepCount,aDeviceNames[int(iPluginFunction/5)]);
		break;
	case 4:
		bRes=trigMute(aDeviceTypsR[int(iPluginFunction/5)],aDeviceTypes[int(iPluginFunction/5)],aDeviceNames[int(iPluginFunction/5)]);
		break;
	default:
		return 0;
	}
	if(!bRes){
		if(plgOptions.lShowOSD){
			CString sSplashText;
			sSplashText.Format("'%s': %s",aDeviceNames[int(iPluginFunction/5)],_l("Option inaccessible"));
			pInter->ShowOSD(sSplashText,1000);
		}
	}
	return 1;
}

extern HINSTANCE g_hinstDll;
int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;//FALSE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Full sound control");
	strcpy(dsc->szDesk,"This plugin allows you to control all common mixers in your system by means of hotkeys");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddNumberOption("step","Volume step (percents)","",10,0,100,0);
		pOptionsCallback->AddNumberOption("panstep","Left/Right Pan step (percents)","",10,0,100,0);
		pOptionsCallback->AddBoolOption("osd","Show status in OSD","",1);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		plgOptions.lStepCount=pOptionsCallback->GetNumberOption("step");
		plgOptions.lPanStepCount=pOptionsCallback->GetNumberOption("panstep");
		plgOptions.lShowOSD=pOptionsCallback->GetBoolOption("osd");
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetNumberOption("step",plgOptions.lStepCount);
		pOptionsCallback->SetNumberOption("panstep",plgOptions.lPanStepCount);
		pOptionsCallback->SetBoolOption("osd",plgOptions.lShowOSD);
	}
	return 1;
}
