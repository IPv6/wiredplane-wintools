// SimpleMixer.h: interface for the CSimpleMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEMIXER_H__12A45D24_96E0_11D5_93A8_BDF05C053B6A__INCLUDED_)
#define AFX_SIMPLEMIXER_H__12A45D24_96E0_11D5_93A8_BDF05C053B6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mmsystem.h"

class CSimpleMixer  
{
private:
	HMIXER m_hMixer;

public:
	BOOL isOpened(){return (m_hMixer!=NULL);};
	DWORD m_dwVolumeCtrlID;
	DWORD m_dwMuteCtrlID;

	CSimpleMixer();
	virtual ~CSimpleMixer();

	BOOL Open(UINT mixerID, HWND hNotificationReciever = NULL, DWORD dwCompType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS);
	BOOL Close();

	BOOL GetMuteCtrlValue() const;
	void SetMuteCtrlValue(BOOL bValue);

	UINT GetVolumeCtrlValue() const;
	void SetVolumeCtrlValue(UINT uValue);

	static UINT GetMixerNumber() { return mixerGetNumDevs(); }
};

#endif // !defined(AFX_SIMPLEMIXER_H__12A45D24_96E0_11D5_93A8_BDF05C053B6A__INCLUDED_)
