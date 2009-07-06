// HookCode.h: interface for the CHookCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
#define AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "../WKPlugin.h"
#include "..\..\SmartWires\Atl\crt_common.h"
#include "..\..\SmartWires\Atl\crt_cstring.h"
#include "..\..\SmartWires\SystemUtils\Macros.h"
#include "..\..\SmartWires\Atl\filemap.h"
#include "OpenWithExt.h"
#include "WKShellExtender.h"
#define MAX_CITEMS	20
struct COptions
{
	DWORD dwWKThread;
	BOOL bShowOnlyIfShift;
	BOOL bShowOnlyIfShift2;
	long lDefView1;
	long lDefView2;
	long lDefView3;
	long bMenuBlocked;
	long bRegistered;
	long dwRItemsToGet;
	long bSetInFirst;
	char szListingEditor[MAX_PATH];
	char szCustomItems[MAX_CITEMS][MAX_PATH];
	char szPath1[MAX_PATH];
	char szPath2[MAX_PATH];
	char szPath3[MAX_PATH];
	long lAddToRoot;
	char szDiffEditor[MAX_PATH];
	long bDobyone;
	long bExtrastuff;
	long lDefViewT1;
	long lDefViewT2;
	long lDefViewT3;
	BOOL bAddbutton;
	BOOL bAddsysmen;
};

#endif // !defined(AFX_HOOKCODE_H__64477B7D_244B_4B80_B147_FBB50C232127__INCLUDED_)
