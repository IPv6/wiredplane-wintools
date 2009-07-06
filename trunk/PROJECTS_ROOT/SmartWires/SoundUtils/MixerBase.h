//=============================================================================
// Copyright Langis Pitre 1998
// You may do whatever you want with this code, as long as you include this
// copyright notice in your implementation files.
//
// If you wish to add new classes to this collection, feel free to do so.
// But please send me your code so that I can update the collection.
//
// Comments and bug reports: lpitre@sympatico.ca
//
//=============================================================================
#ifndef _LANGIS_PITRE_MIXER_BASE_H_
#define _LANGIS_PITRE_MIXER_BASE_H_

#include <mmsystem.h>

// Comment-out the next line to prevent warnings to popup. If there was a problem
// initializing a mixer control, your program won't crash since the classes check
// the validity of values, and your controls will simply be disabled.
// For debugging, it does help to find out where the problem is though.

//#define DO_MIXER_WARNING


const DWORD NO_SOURCE = MIXERLINE_COMPONENTTYPE_SRC_LAST + 1;

//=============================================================================
//                         CMixerBase Class
//
// Base class of all the mixer classes
// Takes care of opening the mixer device to get a valid handler.
// This handle, along with the number of channels are stored in
// member variables. The Init() method queries the mixer interface
// for the desired mixer control.
//
// You don't use this class directly, unless you want to derive new classes
// from it, but you can use the 4 inline functions to retrieve the name and
// short name of a mixer control or mixer line:
//
//	        GetControlName()
//	        GetControlShortName()
//	        GetLineName()
//	        GetLineShortName()
//
//===========================================================================

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
class CMixerBase  
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
{
public:

	CMixerBase();
	virtual ~CMixerBase();

	void* GetValuePtr() { return m_val; }

	int GetDestinLineIndex( MIXERLINE &line, DWORD lineType );
	int GetSourceLineIndex( MIXERLINE &line, DWORD lineType );

	int GetMixerControlValue();
	int SetMixerControlValue();
	
	CString GetControlName() const;
	CString GetControlShortName() const;
	CString GetLineName() const;
	CString GetLineShortName() const;

	static LPCTSTR GetControlTypeString( DWORD type );
	static LPCTSTR GetLineTypeString( DWORD type );


	
	int  Init( DWORD DstType, DWORD SrcType, DWORD ControlType, HWND hwnd = NULL);
	
	void CloseMixer();

	CString m_LineName;
	CString m_DestLineName;
	CString m_LineShortName;

	HMIXER       m_HMixer;
    MIXERCONTROL m_Control;
	int          m_nChannels;

private:
	int InitMXDetails();

    MIXERCONTROLDETAILS  m_Details;
	void *m_val;
};
	

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
inline CString CMixerBase::GetControlName() const
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
{
	if( m_HMixer )
		return CString(m_Control.szName);
	else
		return CString("");
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
inline CString CMixerBase::GetControlShortName() const
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
{
	if( m_HMixer )
		return CString(m_Control.szShortName);
	else
		return CString("");
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
inline CString CMixerBase::GetLineName() const
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
{
	return m_LineName;
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
inline CString CMixerBase::GetLineShortName() const
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
{
	return m_LineShortName;
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
inline void CMixerBase::CloseMixer()
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
{
	if ( m_HMixer ) mixerClose( m_HMixer );
	m_HMixer = 0;
	
	if( m_val ) delete[] m_val;
	m_val = NULL;
}


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : PrintMessage
//          
// Descr. : printf style function with variable number of arguments, but that
//          displays a message box once the string has been formatted.
//          
// Arg    : const char *title : string to display in the caption
// Arg    : const char *fmt   : format string for the list of arguments
// Arg    : ...               : 
//-----------------------------------------------------------------------------
inline void PrintMessage( const char *title, const char *fmt, ... )
{
#ifdef DO_MIXER_WARNING
   va_list marker;
   static char buffer[256];

   va_start( marker, fmt );
   _vsnprintf( buffer, 255, fmt, marker ); 
   va_end( marker );

   ::MessageBox( NULL, buffer, title, MB_OK|MB_ICONEXCLAMATION );
#else
   title; fmt;
#endif

}



//===========================================================================
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _LANGIS_PITRE_MIXER_BASE_H_

//===========================================================================
