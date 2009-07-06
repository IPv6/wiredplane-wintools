// ForceDel.cpp : Defines the entry point for the console application.
//
// Modified by Zoltan Csizmadia, zoltan_csizmadia@yahoo.com
// For companies(Austin,TX): If you would like to get my resume, send an email.
//
///////////////////////////////////////////////////////////////////////////////
//
// Thanks Felix, you have a great site. www.mvps.org/win32
//
//
// History:
// 
// 11/6/00		Initial version
//
///////////////////////////////////////////////////////////////////////////////

#include "NtSystemInfo.h"
#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>

//Function definitions
void ShowUsage();
void EnableDebugPriv( void );
DWORD CloseRemoteHandle( LPCTSTR, DWORD, HANDLE );
void CloseRemoteFileHandles( LPCTSTR );
BOOL DeleteTheFile( LPCTSTR );

int DeleteFileNOW( TCHAR* szFileToDelete )
{
	LPCTSTR lpFileName = szFileToDelete;

	//Enable debug privilege
	EnableDebugPriv();
	TCHAR lpPath[MAX_PATH]={0};
	//Get the full path
	if ( GetFullPathName( lpFileName, _MAX_PATH, lpPath, NULL ) == 0 )
	{
		_tprintf( _T("GetFullPathName() failed. Error = %d\n"), GetLastError() );
		return 2;
	}

	//Close every handle in the system for this file
	CloseRemoteFileHandles( lpPath );
	
	//Try to delete it
	rc = DeleteTheFile( lpPath );
		
	return rc ? 0 : 1;
}

//Enable DEBUG privilege
void EnableDebugPriv( void )
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	// enable the SeDebugPrivilege
	if ( ! OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
	{
		_tprintf( _T("OpenProcessToken() failed, Error = %d SeDebugPrivilege is not available.\n"), GetLastError() );
		return;
	}

	if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
	{
		_tprintf( _T("LookupPrivilegeValue() failed, Error = %d SeDebugPrivilege is not available.\n"), GetLastError() );
		CloseHandle( hToken );
		return;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
		_tprintf( _T("AdjustTokenPrivileges() failed, Error = %d SeDebugPrivilege is not available.\n"), GetLastError() );
		
	CloseHandle( hToken );
}

//Close a handle in a remote process
DWORD CloseRemoteHandle( LPCTSTR lpProcessName, DWORD processID, HANDLE handle )
{
	HANDLE ht = 0;
	DWORD rc = 0;
	
	_tprintf( _T("Closing handle in process #%d (%s) ... "), processID, lpProcessName );

	// open the process
	HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processID );
	
	if ( hProcess == NULL )
	{
		rc = GetLastError();
		_tprintf( _T("OpenProcess() failed\n") );
		return rc;
	}

	// load kernel32.dll
	HMODULE hKernel32 = LoadLibrary( _T("kernel32.dll") );

	// CreateRemoteThread()
	ht = CreateRemoteThread( 
			hProcess, 
			0, 
			0, 
			(DWORD (__stdcall *)( void *))GetProcAddress( hKernel32, "CloseHandle" ),
			handle, 
			0, 
			&rc );
	
	if ( ht == NULL )
	{
		//Something is wrong with the privileges, or the process doesn't like us
		rc = GetLastError();
		_tprintf( _T("CreateRemoteThread() failed\n") );
		goto cleanup;
	}

	switch ( WaitForSingleObject( ht, 2000 ) )
	{
	case WAIT_OBJECT_0:
		//Well done
		rc = 0;
		_tprintf( _T("Ok\n"), rc );
		
		break;
	
	default:
		//Oooops, shouldn't be here
		rc = GetLastError();
		_tprintf( _T("WaitForSingleObject() failed\n") );
		goto cleanup;

		break;
	}

cleanup:

	//Closes the remote thread handle
	CloseHandle( ht );

	//Free up the kernel32.dll
	if ( hKernel32 != NULL)
		FreeLibrary( hKernel32 );

	//Close the process handle
	CloseHandle( hProcess );
		
	return rc;
}

//Closes the file handles in the processes which are using this file
void CloseRemoteFileHandles( LPCTSTR lpFileName )
{
	CString deviceFileName;
	CString fsFilePath;
	CString name;
	CString processName;
	SystemHandleInformation hi;
	SystemProcessInformation pi;
	SystemProcessInformation::SYSTEM_PROCESS_INFORMATION* pPi;
	
	//Convert it to device file name
	if ( !SystemInfoUtils::GetDeviceFileName( lpFileName, deviceFileName ) )
	{
		_tprintf( _T("GetDeviceFileName() failed.\n") );
		return;
	}
	
	//Query every file handle (system wide)
	if ( !hi.SetFilter( _T("File"), TRUE ) )
	{
		_tprintf( _T("SystemHandleInformation::SetFilter() failed.\n") );
		return;
	}

	if ( !pi.Refresh() )
	{
		_tprintf( _T("SystemProcessInformation::Refresh() failed.\n") );
		return;
	}

	//Iterate through the found file handles
	for ( POSITION pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
	{
		SystemHandleInformation::SYSTEM_HANDLE& h = hi.m_HandleInfos.GetNext(pos);

		if ( !pi.m_ProcessInfos.Lookup( h.ProcessID, pPi ) )
			continue;

		if ( pPi == NULL )
			continue;

		//Get the process name
		SystemInfoUtils::Unicode2CString( &pPi->usName, processName );

		//NT4 Stupid thing if I query the name of a file in services.exe
		//Messengr service brings up a message dialog ??? :(
		if ( INtDll::dwNTMajorVersion == 4 && _tcsicmp( processName, _T("services.exe") ) == 0 )
			continue;
		
		//what's the file name for this given handle?
		hi.GetName( (HANDLE)h.HandleNumber, name, h.ProcessID );

		//This is what we want to delete, so close the handle
		if ( _tcsicmp( name, deviceFileName ) == 0 )
			CloseRemoteHandle( processName, h.ProcessID, (HANDLE)h.HandleNumber );
	}
}

//Deletes the file
BOOL DeleteTheFile( LPCTSTR lpFileName )
{
	//Deletes the file
	BOOL rc = ::DeleteFile( lpFileName );
	
	if ( rc )
		_tprintf( _T("Successfully deleted.\n") );
	else
		_tprintf( _T("Couldn't delete. Error = %d\n"), GetLastError() );

	return rc;
}
