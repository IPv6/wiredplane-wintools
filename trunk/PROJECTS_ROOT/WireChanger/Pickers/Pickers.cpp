// Pickers.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Pickers.h"
#include "CalendarDlg.h"
#include "CalendarDlg2.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPickersApp

BEGIN_MESSAGE_MAP(CPickersApp, CWinApp)
	//{{AFX_MSG_MAP(CPickersApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickersApp construction

CPickersApp::CPickersApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPickersApp object

CPickersApp theApp;
BOOL WINAPI GetDate(CCallbacks* pCallback, char* szOutKey, size_t dwOutKey, char* szOutTitle, size_t dwOutTitle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleDateTime dtCurrent;
	CCalendarDlg dlg;
	dlg.sTitle="Choose date";
	dlg.sButton="OK";
	dlg.DoModal();
	szOutKey[0]=0;
	szOutTitle[0]=0;
	dtCurrent=dlg.dtSelected;
	if(dtCurrent.GetStatus() == COleDateTime::invalid){
		return FALSE;
	}
	sprintf(szOutKey,"%02i/%i/%i ",dtCurrent.GetDay(),dtCurrent.GetMonth(),dtCurrent.GetYear());
	sprintf(szOutTitle,"%s",dtCurrent.Format("%d %B %Y"));
	return TRUE;
}

BOOL WINAPI GetDateX(CCallbacks* pCallback, char* szOutKey, size_t dwOutKey, char* szOutTitle, size_t dwOutTitle, char* szOutOnDate, size_t dwOutOnDate)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleDateTime dtCurrent;
	CCalendarDlg dlg;
	dlg.sTitle="Choose date";
	dlg.sButton="OK";
	dlg.DoModal();
	szOutKey[0]=0;
	szOutTitle[0]=0;
	dtCurrent=dlg.dtSelected;
	if(dtCurrent.GetStatus() == COleDateTime::invalid){
		return FALSE;
	}
	sprintf(szOutKey,"%02i/%i/%i ",dtCurrent.GetDay(),dtCurrent.GetMonth(),dtCurrent.GetYear());
	sprintf(szOutTitle,"%s",dtCurrent.Format("%d %B %Y"));
	sprintf(szOutOnDate,"%s",CDataXMLSaver::OleDate2Str(dtCurrent));
	return TRUE;
}

BOOL WINAPI Show4Months(CCallbacks* pCallback, const char* szFile, HWND pParent)
{
	char szOutKey[256]={0};
	char szOutTitle[256]={0};
	BOOL bRes=GetDate(pCallback, szOutKey, sizeof(szOutKey), szOutTitle, sizeof(szOutTitle));
	return bRes;
}

BOOL WINAPI EditNote(CCallbacks* pCallback, const char* szFile, HWND pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CCalendarDlg2 dlg;
	dlg.sMainFile=szFile;
	dlg.bINewLine=FALSE;
	dlg.sKey="%02i/%i/%i ";
	dlg.sTitle="Edit day-by-day notes";
	dlg.sStatic1="Choose calendar cell";
	dlg.sStatic2="Edit note for that day";
	dlg.sButton="Save and Close";
	dlg.DoModal();
	return TRUE;
}

BOOL WINAPI EditHoliday(CCallbacks* pCallback, const char* szFile, HWND pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CCalendarDlg2 dlg;
	dlg.sMainFile=szFile;
	dlg.bINewLine=TRUE;
	dlg.sKey="%i/%i ";
	dlg.sStatic1="Choose calendar cell";
	dlg.sTitle="Edit annual events and birthdays";
	dlg.sStatic2="Edit annual events for that day. Place each event on the new line";
	dlg.sButton="Save and Close";
	dlg.DoModal();
	return TRUE;
}

BOOL WINAPI EditRHoliday(CCallbacks* pCallback, const char* szFile, HWND pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CCalendarDlg2 dlg;
	dlg.sMainFile=szFile;
	dlg.bINewLine=TRUE;
	dlg.sKey="%i/%i*";
	dlg.sStatic1="Choose calendar cell";
	dlg.sTitle="Edit holidays descriptions";
	dlg.sStatic2="Edit holidays for that day (corresponding cell will be shown as holiday)";
	dlg.sButton="Save and Close";
	dlg.DoModal();
	return TRUE;
}

#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif