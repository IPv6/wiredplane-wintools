//////////////////////////////////////////////////////////////////////
// Volume Controler
// by Whoo(whoo@isWhoo.com)
// Oct.1 2001

//////////////////////////////////////////////////////////////////////
// Mixer.cpp: implementation of the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mixer.h"

#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CMixer::~CMixer()
{

}

#undef FAILED
#define FAILED(X) (MMSYSERR_NOERROR != X)
/////////////////////////////////////////////////
//  Component: value from dwComponentType member of MIXERLINE, eg://	MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
//  Kind : 0 Wave ouput tunnle, 1 Wave input tunle, 2... other. 
CMixer::CMixer(DWORD ComponentType, int dkKind): m_dwControlID(-1), m_bOK(false), m_dwChannels(0)
{
	HRESULT hr=0;
	HMIXER hMixer=0;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERLINE mxl;
	memset(&mxl,0,sizeof(mxl));
	DWORD kind=0, count=0;
	// Çהוסü למו
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = ComponentType;
	mxl.dwDestination=0;
	mxl.dwSource=0;
	hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return;
	}
	m_dwChannels = mxl.cChannels;
	if(dkKind==0 || dkKind==2){
		MIXERCONTROL mc;
		memset(&mc,0,sizeof(mc));
		mc.cbStruct = sizeof(mc);
		MIXERLINECONTROLS mxlc;
		memset(&mxlc,0,sizeof(mxlc));
		mxlc.cbStruct = sizeof(mxlc);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mc;
		hr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
		m_dwControlID = mc.dwControlID;
	}
	if(dkKind==1){
		MIXERCONTROL mc;
		memset(&mc,0,sizeof(mc));
		mc.cbStruct = sizeof(mc);
		MIXERLINECONTROLS mxlc;
		memset(&mxlc,0,sizeof(mxlc));
		mxlc.cbStruct = sizeof(mxlc);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mc;
		hr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
		m_dwControlMID = mc.dwControlID;
	}
	// Çהוסü סעאנמו
	/*MIXERCONTROL mc;
	memset(&mc,0,sizeof(mc));
	mc.cbStruct = sizeof(mc);
	MIXERLINECONTROLS mxlc;
	memset(&mxlc,0,sizeof(mxlc));
	mxlc.cbStruct = sizeof(mxlc);
	kind = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;
    hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return;
	}

	count = mxl.dwSource;
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if (mxl.dwComponentType == ComponentType)
		{
			m_dwChannels = mxl.cChannels;
			mc.cbStruct = sizeof(mc);
			mxlc.cbStruct = sizeof(mxlc);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);
			mxlc.pamxctrl = &mc;
			hr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
			m_dwControlID = mc.dwControlID;
			break;
		};
	}*/
	mixerClose(hMixer);
	m_bOK = true;
}

void CMixer::SetMute(DWORD dwVol)
{
	if (!m_bOK|| m_dwControlID==0) return;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxdu;

	mxdu.fValue = dwVol;

	mxcd.cMultipleItems = 0;
	mxcd.cChannels = 1;//m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlMID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE| MIXER_OBJECTF_HMIXER);	
	
	mixerClose(hMixer);
}

void CMixer::SetVolume(DWORD dwVol)
{
	if (!m_bOK|| m_dwControlID==0) return;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxdu;

	mxdu.dwValue = dwVol;

	mxcd.cMultipleItems = 0;
	mxcd.cChannels = 1;//m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE| MIXER_OBJECTF_HMIXER);	
	
	mixerClose(hMixer);
}

DWORD CMixer::GetVolume()
{
	if (!m_bOK || m_dwControlMID==0) return 0;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return 0;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxdu;


	mxcd.cMultipleItems = 0;
	mxcd.cChannels = 1;//m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);	
	
	mixerClose(hMixer);
	return mxdu.dwValue;
}

DWORD CMixer::GetMute()
{
	if (!m_bOK || m_dwControlMID==0) return 0;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return 0;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxdu;


	mxcd.cMultipleItems = 0;
	mxcd.cChannels = 1;//m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlMID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);	
	
	mixerClose(hMixer);
	return mxdu.fValue;
}

void CMixer::SetPan(long dwVol)
{
	if (!m_bOK || m_dwControlPID==0) return;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_SIGNED mxdu;

	mxdu.lValue = dwVol;

	mxcd.cMultipleItems = 0;
	mxcd.cChannels = 1;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlPID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE| MIXER_OBJECTF_HMIXER);	
	
	mixerClose(hMixer);
}

long CMixer::GetPan()
{
	if (!m_bOK || m_dwControlPID==0) return 0;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return 0;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_SIGNED mxdu;


	mxcd.cMultipleItems = 0;
	mxcd.cChannels = 1;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlPID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);	
	
	mixerClose(hMixer);
	return mxdu.lValue;
}