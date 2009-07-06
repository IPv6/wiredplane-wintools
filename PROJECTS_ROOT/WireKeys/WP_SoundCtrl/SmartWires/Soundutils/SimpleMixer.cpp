// SimpleMixer.cpp: implementation of the CSimpleMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SimpleMixer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimpleMixer::CSimpleMixer()
{
	m_hMixer=NULL;
}

CSimpleMixer::~CSimpleMixer()
{

}

BOOL CSimpleMixer::Open(UINT mixerID, HWND hNotificationReciever, DWORD dwCompType)
{
	// Открываем миксер.
	if(::mixerOpen(
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
	if(::mixerGetLineInfo(
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

	if(::mixerGetLineControls(
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
	if(::mixerGetLineControls(
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
	if(m_hMixer && ::mixerClose(m_hMixer) != MMSYSERR_NOERROR)
		return FALSE;

	return TRUE;
}

BOOL CSimpleMixer::GetMuteCtrlValue() const
{
	MIXERCONTROLDETAILS_BOOLEAN mcd_b;
	MIXERCONTROLDETAILS mcd;
	mcd.cbStruct = sizeof(mcd);
	mcd.dwControlID = m_dwMuteCtrlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;
	mcd.cbDetails = sizeof(mcd_b);
	mcd.paDetails = &mcd_b;

	::mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);

	return (BOOL)mcd_b.fValue;
}

void CSimpleMixer::SetMuteCtrlValue(BOOL bValue)
{
	MIXERCONTROLDETAILS_BOOLEAN mcd_b;
	mcd_b.fValue = (LONG)bValue;

	MIXERCONTROLDETAILS mcd;
	mcd.cbStruct = sizeof(mcd);
	mcd.dwControlID = m_dwMuteCtrlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;
	mcd.cbDetails = sizeof(mcd_b);
	mcd.paDetails = &mcd_b;

	::mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);
}

UINT CSimpleMixer::GetVolumeCtrlValue() const
{
	if(m_hMixer==NULL){
		return 0;
	}
	MIXERCONTROLDETAILS_UNSIGNED mcd_u;
	MIXERCONTROLDETAILS mcd;
	mcd.cbStruct = sizeof(mcd);
	mcd.dwControlID = m_dwVolumeCtrlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;
	mcd.cbDetails = sizeof(mcd_u);
	mcd.paDetails = &mcd_u;

	::mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);

	return (UINT)mcd_u.dwValue;
}

void CSimpleMixer::SetVolumeCtrlValue(UINT uValue)
{
	if(!isOpened()){
		return;
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

	::mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);
}