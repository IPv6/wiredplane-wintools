#if !defined(_SysService_h__)
#define _SysService_h__

//////////////////////////////////////////////////////////////////////////////////
//
//			CSysService Class
//
//////////////////////////////////////////////////////////////////////////////////
//
//		Written by  Jay Wheeler
//					EarthWalk Software
//
//		Version 1 - October, 2000.
//
//////////////////////////////////////////////////////////////////////////////////
//
//		CSysService is copyright © 2000. EarthWalk Software.
//
//		This library is free software; you can redistribute it and/or
//		modify it under the terms of the GNU Lesser General Public
//		License as published by the Free Software Foundation; either
//		version 2.1 of the License, or (at your option) any later version.
//
//		This library is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//		Lesser General Public License for more details.
//
//		You should have received a copy of the GNU Lesser General Public
//		License along with this library; if not, write to the Free Software
//		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////////

#include <winsvc.h>

//////////////////////////////////////////////////////////////////////////////////

#define	SYSSERVICE_NOERROR					0
#define	SYSSERVICE_STATE_ERROR				SYSSERVICE_NOERROR + 1
#define	SYSSERVICE_NOT_RUNNING				SYSSERVICE_STATE_ERROR + 1
#define	SYSSERVICE_NOT_STOPPED				SYSSERVICE_NOT_RUNNING + 1
#define	SYSSERVICE_OPEN_ERROR				SYSSERVICE_NOT_STOPPED + 1
#define	SYSSERVICE_UNABLE_TO_PAUSE			SYSSERVICE_OPEN_ERROR + 1
#define	SYSSERVICE_UNABLE_TO_STOP			SYSSERVICE_UNABLE_TO_PAUSE + 1
#define	SYSSERVICE_UNABLE_TO_CONTINUE		SYSSERVICE_UNABLE_TO_STOP + 1
#define	SYSSERVICE_UNABLE_TO_START			SYSSERVICE_UNABLE_TO_CONTINUE + 1
#define	SYSSERVICE_INVALID_REQUEST			SYSSERVICE_UNABLE_TO_START + 1

#define SYSSERVICE_QUERY_SERVICE_CONFIG		SYSSERVICE_INVALID_REQUEST + 1
#define	SYSSERVICE_INSUFFICIENT_BUFFER		SYSSERVICE_QUERY_SERVICE_CONFIG + 1
#define SYSSERVICE_NOT_SERVICE				SYSSERVICE_INSUFFICIENT_BUFFER + 1
#define SYSSERVICE_QUERY_SERVICE_STATUS		SYSSERVICE_NOT_SERVICE + 1
#define SYSSERVICE_NULL_SERVICENAME			SYSSERVICE_QUERY_SERVICE_STATUS + 1
#define SYSSERVICE_GET_DISPLAY_NAME			SYSSERVICE_NULL_SERVICENAME + 1
#define SYSSERVICE_OPEN_SC_MANAGER			SYSSERVICE_GET_DISPLAY_NAME + 1

//////////////////////////////////////////////////////////////////////////////////

#define MAX_SERVICE_BUFFER		16384

//////////////////////////////////////////////////////////////////////////////////

typedef struct _ServiceInformation
{
	SERVICE_STATUS			ServiceStatus;
	QUERY_SERVICE_CONFIG	ServiceConfig;
} ServiceInformation;

typedef struct _ServiceBuffer
{
	union
	{
		char					Data[MAX_SERVICE_BUFFER + 1];
		ServiceInformation		Structures;
	};
} ServiceBuffer;

//////////////////////////////////////////////////////////////////////////////////

class CSysService
{

public:
							CSysService(CString p_HostName="");
	virtual					~CSysService();

	//////////////////////////////////////////////////////////////////////////////

public:
	virtual bool					Open(CString p_HostName="");

	virtual bool					GetServiceInformation(CString p_ServiceName);
	virtual DWORD					Control(CString p_ServiceName, DWORD p_ServiceRequest);
	virtual bool					SetServiceConfig(void);
	virtual bool					NewService(void);
	virtual bool					RemoveService(CString p_ServiceName);

	CString							ServiceHostName(void);
	CString							ServiceName(void);
	CString							ServiceDisplayName(void);
	CString							ServiceStartName(void);

	DWORD							ServiceType(void);
	DWORD							StartType(void);
	DWORD							ServiceState(CString p_ServiceName);

	CString							LocalHostName(void);
	CString							ServiceDisplayName(CString p_ServiceName);

	CString							ErrorMessage(DWORD p_Error);
	long							Result(void);
	DWORD							Error(void);
	DWORD							ServiceError(void);
	CString							ServiceErrorMessage(DWORD p_Error);

	CString							StatusString(DWORD p_ServiceStatus);
	CString							StartupString(DWORD p_StartupType);

	void							SetPath(CString p_Param);
	void							SetAccount(CString p_Param);
	void							SetDisplayName(CString p_Param);
	void							SetPassword(bool p_ChangePassword, CString p_Param);
	void							SetServiceName(CString p_Param);

	void							SetServiceType(DWORD p_Param);
	void							SetStartType(DWORD p_Param);
	void							SetErrorControl(DWORD p_Param);

	virtual QUERY_SERVICE_CONFIG	* ServiceConfig(void);
	virtual SERVICE_STATUS			* ServiceStatus(void);

protected:

	//////////////////////////////////////////////////////////////////////////////
	
public:

protected:
	CString					g_LocalHostName;
	CString					g_ServiceHostName;
	CString					g_ServiceName;
	CString					g_ServiceDisplayName;
	CString					g_ServiceStartName;

	CString					g_Path;
	CString					g_Password;
	bool					g_ChangePassword;

	DWORD					g_ServiceType;
	DWORD					g_StartType;
	DWORD					g_ErrorControl;
	DWORD					g_ServiceState;

	long					g_Result;
	bool					g_Open;
	DWORD					g_Error;

	DWORD					g_ServiceRequest;
	int						g_ServiceRequestError;

	SC_HANDLE				g_ServiceHandle;

	DWORD					g_ServiceBufferSize;

	ServiceBuffer			g_Buffer;

private:
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif
