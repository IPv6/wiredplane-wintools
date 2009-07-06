// Netbios.h: interface for the Netbios class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETBIOS_H__DB4AF1F4_88FA_49A7_88FB_9D79DCEF943E__INCLUDED_)
#define AFX_NETBIOS_H__DB4AF1F4_88FA_49A7_88FB_9D79DCEF943E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Netbios.cpp: implementation of the Netbios class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

DWORD WINAPI NetBiosListener(LPVOID lpdwstatus);
void MakeNetbiosName (char *achDest, LPCSTR szSrc,int iLastChar=32);

#endif // !defined(AFX_NETBIOS_H__DB4AF1F4_88FA_49A7_88FB_9D79DCEF943E__INCLUDED_)
