// SimpleMixer.cpp: implementation of the CSimpleMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SimpleMixer.h"

#pragma comment(lib, "Winmm.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimpleMixer::CSimpleMixer(BOOL bCacheMixer)
{
	bCasheValue=bCacheMixer;
	lCachedValue=-1;
	m_hMixer=NULL;
	m_hForOpen=0;
	hLib=0;
}

CSimpleMixer::~CSimpleMixer()
{
	if(hLib){
		FreeLibrary(hLib);
	}
}

BOOL CSimpleMixer::Open(UINT mixerID, HWND hNotificationReciever, DWORD dwCompType)
{
	if(hLib==0){
		hLib=LoadLibrary("winmm.dll");
		if(hLib==0){
			return 0;
		}
		my_mixerOpen=(_mixerOpen)DBG_GETPROC(hLib,"mixerOpen");
		if(my_mixerOpen==0){
			return 0;
		}
		my_mixerGetLineInfo=(_mixerGetLineInfoA)DBG_GETPROC(hLib,"mixerGetLineInfoA");
		if(my_mixerGetLineInfo==0){
			return 0;
		}
		my_mixerGetLineControls=(_mixerGetLineControlsA)DBG_GETPROC(hLib,"mixerGetLineControlsA");
		if(my_mixerGetLineControls==0){
			return 0;
		}
		my_mixerClose=(_mixerClose)DBG_GETPROC(hLib,"mixerClose");
		if(my_mixerClose==0){
			return 0;
		}
		my_mixerGetControlDetails=(_mixerGetControlDetailsA)DBG_GETPROC(hLib,"mixerGetControlDetailsA");
		if(my_mixerGetControlDetails==0){
			return 0;
		}
		my_mixerSetControlDetails=(_mixerSetControlDetails)DBG_GETPROC(hLib,"mixerSetControlDetails");
		if(my_mixerSetControlDetails==0){
			return 0;
		}
		 
		 
	}
	// Открываем миксер.
	m_hForOpen=hNotificationReciever;
	if((*my_mixerOpen)(
		&m_hMixer,
		mixerID,
		(DWORD)hNotificationReciever,
		0,
		MIXER_OBJECTF_HMIXER | (hNotificationReciever == NULL ? 0 : CALLBACK_WINDOW)
	) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	// Получаем ID аудио линии, связанной с колонками.
	MIXERLINE ml;
	ml.cbStruct = sizeof(ml);
	ml.dwComponentType = dwCompType;
	if((*my_mixerGetLineInfo)(
		(HMIXEROBJ)m_hMixer,
		&ml,
		MIXER_GETLINEINFOF_COMPONENTTYPE
	) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	// Получаем ID выключателя звука.
	MIXERCONTROL mc;
	MIXERLINECONTROLS mcs;
	mcs.cbStruct = sizeof(mcs);
	mcs.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mcs.dwLineID = ml.dwLineID;
	mcs.cControls = 1;
	mcs.cbmxctrl = sizeof(mc);
	mcs.pamxctrl = &mc;

	if((*my_mixerGetLineControls)(
		(HMIXEROBJ)m_hMixer,
		&mcs,
		MIXER_GETLINECONTROLSF_ONEBYTYPE
	) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	m_dwMuteCtrlID = mc.dwControlID;

	// Получаем ID регулятора громкости.
	mcs.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	if((*my_mixerGetLineControls)(
		(HMIXEROBJ)m_hMixer,
		&mcs,
		MIXER_GETLINECONTROLSF_ONEBYTYPE
	) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	m_dwVolumeCtrlID = mc.dwControlID;
	
	return TRUE;
}

BOOL CSimpleMixer::Close()
{
	if(m_hMixer && (*my_mixerClose)(m_hMixer) != MMSYSERR_NOERROR)
		return FALSE;

	return TRUE;
}

BOOL CSimpleMixer::GetMuteCtrlValue()
{
	if(!isOpened()){
		Open(0,m_hForOpen);
		if(!isOpened()){
			return 0;
		}
	}
	MIXERCONTROLDETAILS_BOOLEAN mcd_b;
	MIXERCONTROLDETAILS mcd;
	mcd.cbStruct = sizeof(mcd);
	mcd.dwControlID = m_dwMuteCtrlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;
	mcd.cbDetails = sizeof(mcd_b);
	mcd.paDetails = &mcd_b;

	(*my_mixerGetControlDetails)((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);

	return (BOOL)mcd_b.fValue;
}

void CSimpleMixer::SetMuteCtrlValue(BOOL bValue)
{
	if(!isOpened()){
		Open(0,m_hForOpen);
		if(!isOpened()){
			return;
		}
	}
	MIXERCONTROLDETAILS_BOOLEAN mcd_b;
	mcd_b.fValue = (LONG)bValue;

	MIXERCONTROLDETAILS mcd;
	mcd.cbStruct = sizeof(mcd);
	mcd.dwControlID = m_dwMuteCtrlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;
	mcd.cbDetails = sizeof(mcd_b);
	mcd.paDetails = &mcd_b;

	(*my_mixerSetControlDetails)((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);
}

__int64 CSimpleMixer::GetVolumeCtrlValue()
{
	if(!isOpened()){
		Open(0,m_hForOpen);
		if(!isOpened()){
			return 0;
		}
	}
	if(bCasheValue){
		if(lCachedValue>=0){
			return lCachedValue;
		}
	}
	MIXERCONTROLDETAILS_UNSIGNED mcd_u;
	MIXERCONTROLDETAILS mcd;
	mcd.cbStruct = sizeof(mcd);
	mcd.dwControlID = m_dwVolumeCtrlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;
	mcd.cbDetails = sizeof(mcd_u);
	mcd.paDetails = &mcd_u;

	(*my_mixerGetControlDetails)((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);
	lCachedValue=mcd_u.dwValue;
	return (__int64)mcd_u.dwValue;
}

void CSimpleMixer::SetVolumeCtrlValue(UINT uValue)
{
	if(!isOpened()){
		Open(0,m_hForOpen);
		if(!isOpened()){
			return;
		}
	}

	MIXERCONTROLDETAILS_UNSIGNED mcd_u;
	mcd_u.dwValue = (DWORD)uValue;

	MIXERCONTROLDETAILS mcd;
	mcd.cbStruct = sizeof(mcd);
	mcd.dwControlID = m_dwVolumeCtrlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;
	mcd.cbDetails = sizeof(mcd_u);
	mcd.paDetails = &mcd_u;

	(*my_mixerSetControlDetails)((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);
	lCachedValue=uValue;
}

void CSimpleMixer::SetWndForOpen(HWND h)
{
	m_hForOpen=h;
}

// VISTA code
/*
void CSimpleMixerVista::Mute()
{
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume *endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL; 

	// -------------------------

	endpointVolume->SetMute(TRUE, NULL);

	endpointVolume->Release();

	CoUninitialize();
}
*/