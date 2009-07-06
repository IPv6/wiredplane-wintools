// SimpleMixer.h: interface for the CSimpleMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEMIXER_H__12A45D24_96E0_11D5_93A8_BDF05C053B6A__INCLUDED_)
#define AFX_SIMPLEMIXER_H__12A45D24_96E0_11D5_93A8_BDF05C053B6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mmsystem.h"
#ifndef DWORD_PTR
#define DWORD_PTR DWORD
#endif
typedef MMRESULT (WINAPI* _mixerOpen)( OUT LPHMIXER phmx, IN UINT uMxId, IN DWORD_PTR dwCallback, IN DWORD_PTR dwInstance, IN DWORD fdwOpen);
typedef MMRESULT (WINAPI* _mixerGetLineInfoA)( IN HMIXEROBJ hmxobj, OUT LPMIXERLINEA pmxl, IN DWORD fdwInfo);
typedef MMRESULT (WINAPI* _mixerGetLineControlsA)( IN HMIXEROBJ hmxobj, IN OUT LPMIXERLINECONTROLSA pmxlc, IN DWORD fdwControls);
typedef MMRESULT (WINAPI* _mixerClose)( IN OUT HMIXER hmx);
typedef MMRESULT (WINAPI* _mixerGetControlDetailsA)( IN HMIXEROBJ hmxobj, IN OUT LPMIXERCONTROLDETAILS pmxcd, IN DWORD fdwDetails);
typedef MMRESULT (WINAPI* _mixerSetControlDetails)( IN HMIXEROBJ hmxobj, IN LPMIXERCONTROLDETAILS pmxcd, IN DWORD fdwDetails);
class CSimpleMixer  
{
private:
	BOOL bCasheValue;
	long lCachedValue;
	HMIXER m_hMixer;
	HWND m_hForOpen;
	HMODULE hLib;
	_mixerOpen my_mixerOpen;
	_mixerClose my_mixerClose;
	_mixerGetLineInfoA my_mixerGetLineInfo;
	_mixerGetLineControlsA my_mixerGetLineControls;
	_mixerGetControlDetailsA my_mixerGetControlDetails;
	_mixerSetControlDetails my_mixerSetControlDetails;
public:
	void SetWndForOpen(HWND h);
	BOOL isOpened(){return (m_hMixer!=NULL);};
	DWORD m_dwVolumeCtrlID;
	DWORD m_dwMuteCtrlID;

	CSimpleMixer(BOOL bCacheMixer);
	virtual ~CSimpleMixer();

	BOOL Open(UINT mixerID, HWND hNotificationReciever = NULL, DWORD dwCompType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS);
	BOOL Close();

	BOOL GetMuteCtrlValue();
	void SetMuteCtrlValue(BOOL bValue);

	__int64 GetVolumeCtrlValue();
	void SetVolumeCtrlValue(UINT uValue);

	static UINT GetMixerNumber() { return mixerGetNumDevs(); }
};

#endif // !defined(AFX_SIMPLEMIXER_H__12A45D24_96E0_11D5_93A8_BDF05C053B6A__INCLUDED_)
