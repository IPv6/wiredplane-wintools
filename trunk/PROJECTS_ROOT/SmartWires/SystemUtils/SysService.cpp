#include "stdafx.h"
#include "SysService.h"

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////
//
//				Constructor
//
//		Clear result, error and state flags
//		Set open state = false
//
//		Extend this method with constructor for
//		extended class.  The code here will execute
//		prior to the new class constructor
//
//		Parameter:
//			p_HostName
//				Name of target host.
//
//////////////////////////////////////////////////////////
CSysService::CSysService(CString p_HostName)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_Open = false;

	g_ServiceHostName = LocalHostName();
	
	if (p_HostName.IsEmpty())
		return;

	g_Open = Open(p_HostName);
}

//////////////////////////////////////////////////////////
//
//				Destructor
//
//////////////////////////////////////////////////////////
//
//		Extend this method with destructor for
//		extended class.  The code here will execute
//		prior to the new class destructor
//
//////////////////////////////////////////////////////////
CSysService::~CSysService()
{
	if (g_ServiceHandle != NULL)
		CloseServiceHandle(g_ServiceHandle);
}

//////////////////////////////////////////////////////////
//
//		Open
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			true if successful
//      Parameters:
//			p_HostName
//				Name of host to connect to.
//		Remarks:
//			Opens a connection to the service control
//			manager on the specified host.
//
//			Sets g_ServiceHandle.
//			Sets g_ServiceHostName to p_HostName.
//
//////////////////////////////////////////////////////////
bool CSysService::Open(CString p_HostName)
{
	if (g_Open)
		return true;

	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	if (p_HostName.IsEmpty())
		p_HostName = LocalHostName();
	g_ServiceHostName = p_HostName;

	g_ServiceHandle = OpenSCManager((LPCTSTR)g_ServiceHostName,
									SERVICES_ACTIVE_DATABASE,
									GENERIC_READ | 
										GENERIC_WRITE |
										GENERIC_EXECUTE);
	if (g_ServiceHandle != NULL)
        return true;
	g_ServiceRequestError = SYSSERVICE_OPEN_SC_MANAGER;
	g_Error = GetLastError();
	g_Result++;
	return false;
}

//////////////////////////////////////////////////////////
//
//		ServiceDisplayName
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			CString containing ServiceDisplayName.
//      Parameters:
//			p_ServiceName
//				CString containing name of service to query.
//		Remarks:
//			Returns the ServiceDisplayName corresponding to
//			p_ServiceName.
//
//			Sets g_ServiceDisplayName and g_ServiceName.
//
//////////////////////////////////////////////////////////
CString CSysService::ServiceDisplayName(CString p_ServiceName)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_ServiceDisplayName.Empty();
    if (p_ServiceName.IsEmpty())
	{
		g_ServiceRequestError = SYSSERVICE_NULL_SERVICENAME;
		return p_ServiceName;
	}
	g_ServiceName = p_ServiceName;
	
	memset(&g_Buffer, 0, MAX_SERVICE_BUFFER);

	g_ServiceBufferSize = MAX_SERVICE_BUFFER;

	if (GetServiceDisplayName(g_ServiceHandle,
							  (LPCTSTR)g_ServiceName,
							  g_Buffer.Data,
							  &g_ServiceBufferSize) == 0)
	{
		g_ServiceRequestError = SYSSERVICE_GET_DISPLAY_NAME;
	    g_Result++;
	    g_Error = GetLastError();
		return g_ServiceDisplayName;
	}
	g_ServiceDisplayName.Format("%s", g_Buffer.Data);
	return g_ServiceDisplayName;
}

//////////////////////////////////////////////////////////
//
//		GetServiceInformation
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			true if successful.
//      Parameters:
//			p_ServiceName
//				CString containing name of service to query.
//		Remarks:
//			Sets g_ServiceName to parameter.
//			Sets g_ServiceType, g_StartType, g_ServiceStartName,
//			and g_ServiceState to returned values.
//
//////////////////////////////////////////////////////////
bool CSysService::GetServiceInformation(CString p_ServiceName)
{
	DWORD					l_BytesNeeded = 0;

	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	if (p_ServiceName.IsEmpty())
		p_ServiceName = g_ServiceName;
	g_ServiceName = p_ServiceName;

	SC_HANDLE				l_ServiceHandle;

	l_ServiceHandle = OpenService(g_ServiceHandle, 
								  (LPCTSTR)g_ServiceName, 
								  SERVICE_ALL_ACCESS);
	if (l_ServiceHandle == NULL)
	{
		g_Result++;
		g_Error = GetLastError();
		return false;
	}
	
	if (! QueryServiceConfig(l_ServiceHandle, 
		                     &g_Buffer.Structures.ServiceConfig, 
						     sizeof(QUERY_SERVICE_CONFIG), 
							 &l_BytesNeeded))
	{
		g_Error = GetLastError();
		if (g_Error != ERROR_INSUFFICIENT_BUFFER)
		{
			g_ServiceRequestError = SYSSERVICE_QUERY_SERVICE_CONFIG;
			CloseServiceHandle(l_ServiceHandle);
			return false;
		}
		if (! QueryServiceConfig(l_ServiceHandle, 
			                     &g_Buffer.Structures.ServiceConfig, 
							     l_BytesNeeded, 
							     &l_BytesNeeded))
		{
			g_ServiceRequestError = SYSSERVICE_INSUFFICIENT_BUFFER;
			CloseServiceHandle(l_ServiceHandle);
			return false;
		}
	}
	
	g_ServiceType = g_Buffer.Structures.ServiceConfig.dwServiceType;
	if ((g_ServiceType == SERVICE_KERNEL_DRIVER) ||
		(g_ServiceType == SERVICE_FILE_SYSTEM_DRIVER))
	{
		g_ServiceRequestError = SYSSERVICE_NOT_SERVICE;
		CloseServiceHandle(l_ServiceHandle);
		return false;
	}
	g_StartType = g_Buffer.Structures.ServiceConfig.dwStartType;
	g_ServiceStartName.Empty();
	if (g_Buffer.Structures.ServiceConfig.lpServiceStartName[0] != (char)0)
		g_ServiceStartName.Format("%s",
							      g_Buffer.Structures.ServiceConfig.lpServiceStartName);
	if (g_Buffer.Structures.ServiceConfig.lpDisplayName[0] != (char)0)
		g_ServiceDisplayName.Format("%s",
							        g_Buffer.Structures.ServiceConfig.lpDisplayName);
	if(! QueryServiceStatus(l_ServiceHandle, 
							&g_Buffer.Structures.ServiceStatus))
	{
		g_ServiceRequestError = SYSSERVICE_QUERY_SERVICE_STATUS;
		CloseServiceHandle(l_ServiceHandle);
		return false;
	}
	g_ServiceState = g_Buffer.Structures.ServiceStatus.dwCurrentState;
	g_ErrorControl = g_Buffer.Structures.ServiceConfig.dwErrorControl;
	g_Path = g_Buffer.Structures.ServiceConfig.lpBinaryPathName;
	CloseServiceHandle(l_ServiceHandle);

	return true;
}
						
//////////////////////////////////////////////////////////
//
//		Control
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			DWORD value containing the result of the
//			operation.
//      Parameters:
//			p_ServiceName
//				CString containing name of service to control.
//			p_ServiceRequest
//				DWORD containing function to perform:
//					SERVICE_CONTROL_STOP
//					SERVICE_CONTROL_CONTINUE
//					SERVICE_CONTROL_PAUSE
//		Remarks:
//			Returns the service state resulting from the
//			operation.
//
//////////////////////////////////////////////////////////
DWORD CSysService::Control(CString p_ServiceName, DWORD p_ServiceRequest)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	if (p_ServiceName.IsEmpty())
		p_ServiceName = g_ServiceName;

	g_ServiceRequestError = SYSSERVICE_NOERROR;
	g_ServiceRequest = p_ServiceRequest;

	ServiceState(g_ServiceName);

	if ((g_ServiceState != SERVICE_STOPPED) &&
		(g_ServiceState != SERVICE_RUNNING) &&
		(g_ServiceState != SERVICE_PAUSED))
	{
		g_ServiceRequestError = SYSSERVICE_STATE_ERROR;
		return NULL;
	}

	if ((p_ServiceRequest == SERVICE_CONTROL_STOP) &&
		(g_ServiceState != SERVICE_RUNNING))
	{
		g_ServiceRequestError = SYSSERVICE_NOT_RUNNING;
		return NULL;
	}

	if ((p_ServiceRequest == SERVICE_CONTROL_CONTINUE) &&
		(g_ServiceState != SERVICE_STOPPED) &&
		(g_ServiceState != SERVICE_PAUSED))
	{
		g_ServiceRequestError = SYSSERVICE_NOT_STOPPED;
		return NULL;
	}

	if ((p_ServiceRequest == SERVICE_CONTROL_PAUSE) &&
		(g_ServiceState != SERVICE_RUNNING))
	{
		g_ServiceRequestError = SYSSERVICE_NOT_STOPPED;
		return NULL;
	}

	SC_HANDLE				l_ServiceHandle;

	l_ServiceHandle = OpenService(g_ServiceHandle, 
								  (LPCTSTR)g_ServiceName, 
								  GENERIC_READ | GENERIC_EXECUTE);
	if (l_ServiceHandle == NULL)
	{
		g_Result++;
		g_Error = GetLastError();
		g_ServiceRequestError = SYSSERVICE_OPEN_ERROR;
		return NULL;
	}

	switch (g_ServiceRequest)
	{
	case SERVICE_CONTROL_PAUSE:
		if (! ControlService(l_ServiceHandle,
							 g_ServiceRequest,
							 &g_Buffer.Structures.ServiceStatus))
		{
			g_ServiceRequestError = SYSSERVICE_UNABLE_TO_PAUSE;
			g_Result++;
			g_Error = GetLastError();
			CloseServiceHandle(l_ServiceHandle);
			return NULL;
		}
		break;
	case SERVICE_CONTROL_STOP:
		if (! ControlService(l_ServiceHandle,
							 g_ServiceRequest,
							 &g_Buffer.Structures.ServiceStatus))
		{
			g_ServiceRequestError = SYSSERVICE_UNABLE_TO_STOP;
			g_Result++;
			g_Error = GetLastError();
			CloseServiceHandle(l_ServiceHandle);
			return NULL;
		}
		break;
	case SERVICE_CONTROL_CONTINUE:
		if (g_ServiceState == SERVICE_PAUSED)
		{
			if (! ControlService(l_ServiceHandle,
								 g_ServiceRequest,
								 &g_Buffer.Structures.ServiceStatus))
			{
				g_ServiceRequestError = SYSSERVICE_UNABLE_TO_CONTINUE;
				g_Result++;
				g_Error = GetLastError();
				CloseServiceHandle(l_ServiceHandle);
				return NULL;
			}
		}
		else
		{
			g_Result = StartService(l_ServiceHandle, 0, NULL);
			if (g_Result == 0)
			{
				g_ServiceRequestError = SYSSERVICE_UNABLE_TO_START;
				CloseServiceHandle(l_ServiceHandle);
				return NULL;
			}
		}
		break;
	default:
		g_ServiceRequestError = SYSSERVICE_INVALID_REQUEST;
		g_Result++;
		g_Error = GetLastError();
		CloseServiceHandle(l_ServiceHandle);
		return NULL;
	}
	CloseServiceHandle(l_ServiceHandle);
	return 	ServiceState(g_ServiceName);
}

//////////////////////////////////////////////
//////////////////////////////////////////////

//////////////////////////////////////////////
//
//    SetPath
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_Param
//				path to service
//		Remarks:
//			g_Path is set to p_Param
//
//////////////////////////////////////////////
void CSysService::SetPath(CString p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_Path = p_Param;
}

//////////////////////////////////////////////
//
//    SetAccount
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_Param
//				service account
//		Remarks:
//			g_ServiceStartName is set to p_Param
//
//////////////////////////////////////////////
void CSysService::SetAccount(CString p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_ServiceStartName = p_Param;
}

//////////////////////////////////////////////
//
//    SetDisplayName
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_Param
//				Service display name
//		Remarks:
//			g_ServiceDisplayName is set to p_Param
//
//////////////////////////////////////////////
void CSysService::SetDisplayName(CString p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_ServiceDisplayName = p_Param;
}

//////////////////////////////////////////////
//
//    SetPassword
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_ChangePassword
//				true = set password
//				false = don't set password
//			p_Param
//				password
//		Remarks:
//			g_Password is set to p_Param
//			g_ChangePassword is set to p_ChangePassword
//
//////////////////////////////////////////////
void CSysService::SetPassword(bool p_ChangePassword, CString p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_ChangePassword = p_ChangePassword;
	g_Password = p_Param;
}

//////////////////////////////////////////////
//
//    SetServiceName
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_Param
//				service name
//		Remarks:
//			g_ServiceName is set to p_Param
//
//////////////////////////////////////////////
void CSysService::SetServiceName(CString p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_ServiceName = p_Param;
}

//////////////////////////////////////////////
//
//    SetServiceType
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_Param
//				service type to set
//		Remarks:
//			g_ServiceType is set to p_Param
//
//////////////////////////////////////////////
void CSysService::SetServiceType(DWORD p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_ServiceType = p_Param;
}

//////////////////////////////////////////////
//
//    SetStartType
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_Param
//				start type
//		Remarks:
//			g_StartType is set to p_Param
//
//////////////////////////////////////////////
void CSysService::SetStartType(DWORD p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_StartType = p_Param;
}

//////////////////////////////////////////////
//
//    SetErrorControl
//
//////////////////////////////////////////////
//
//		Return Value:
//			None.
//      Parameters:
//			p_Param
//				error control
//		Remarks:
//			g_ErrorControl is set to p_Param
//
//////////////////////////////////////////////
void CSysService::SetErrorControl(DWORD p_Param)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	g_ErrorControl = p_Param;
}

//////////////////////////////////////////////////////////
//
//		SetServiceConfig
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			true if successful
//      Parameters:
//			None.
//		Remarks:
//			The following global variables must be set
//			prior to calling this method:
//				g_Path
//				g_ServiceStartName
//				g_ServiceDisplayName
//				g_Password
//				g_ServiceName
//				g_ServiceType
//				g_StartType
//				g_ErrorControl
//			Changes an existing service entry with the specified
//			parameters.
//
//////////////////////////////////////////////////////////
bool CSysService::SetServiceConfig()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	SC_HANDLE				l_ServiceHandle;

	l_ServiceHandle = OpenService(g_ServiceHandle, 
								  (LPCTSTR)g_ServiceName,
								  SERVICE_ALL_ACCESS);
	if (l_ServiceHandle == NULL)
	{
		g_Result++;
		g_Error = GetLastError();
		CloseServiceHandle(l_ServiceHandle);
		return false;
	}
	BOOL l_Result;
	if (g_ChangePassword)
		l_Result = ChangeServiceConfig(l_ServiceHandle,
									   g_ServiceType,
									   g_StartType,
									   g_ErrorControl,
									   g_Path,
									   NULL,
									   0,
									   NULL,
  									   g_ServiceStartName,
									   g_Password,
									   g_ServiceDisplayName);
	else
		l_Result = ChangeServiceConfig(l_ServiceHandle,
									   g_ServiceType,
									   g_StartType,
									   g_ErrorControl,
									   g_Path,
									   NULL,
									   0,
									   NULL,
  									   g_ServiceStartName,
									   NULL,
									   g_ServiceDisplayName);
	if (! l_Result)
	{
		g_Result++;
		g_Error = GetLastError();
		CloseServiceHandle(l_ServiceHandle);
		return false;
	}
	CloseServiceHandle(l_ServiceHandle);
	return true;
}

//////////////////////////////////////////////////////////
//
//		NewService
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			true if successful
//      Parameters:
//			None.
//		Remarks:
//			The following global variables must be set
//			prior to calling this method:
//				g_Path
//				g_ServiceStartName
//				g_ServiceDisplayName
//				g_Password
//				g_ServiceName
//				g_ServiceType
//				g_StartType
//				g_ErrorControl
//			Creates a new service entry with the specified
//			parameters.
//
//////////////////////////////////////////////////////////
bool CSysService::NewService()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	SC_HANDLE				l_ServiceHandle;

	l_ServiceHandle = CreateService(g_ServiceHandle, 
								    (LPCTSTR)g_ServiceName,
									(LPCTSTR)g_ServiceDisplayName,
									SERVICE_ALL_ACCESS,
									g_ServiceType,
									g_StartType,
									g_ErrorControl,
									(LPCTSTR)g_Path,
									NULL,
									0,
									NULL,
  									g_ServiceStartName,
									g_Password);
	if (l_ServiceHandle == NULL)
	{
		g_Result++;
		g_Error = GetLastError();
		return false;
	}
	CloseServiceHandle(l_ServiceHandle);
	return true;
}

//////////////////////////////////////////////////////////
//
//		RemoveService
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			true if successful
//      Parameters:
//			p_ServiceName
//				Name of service to delete.
//		Remarks:
//			Deletes an existing service entry.
//
//////////////////////////////////////////////////////////
bool CSysService::RemoveService(CString p_ServiceName)
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	if (p_ServiceName.IsEmpty())
		return false;

	SC_HANDLE				l_ServiceHandle;

	l_ServiceHandle = OpenService(g_ServiceHandle, 
								  (LPCTSTR)p_ServiceName, 
								  DELETE);
	if (l_ServiceHandle == NULL)
	{
		g_Result++;
		g_Error = GetLastError();
		return false;
	}
	if (! DeleteService(l_ServiceHandle))
	{
		g_Result++;
		g_Error = GetLastError();
		CloseServiceHandle(l_ServiceHandle);
		return false;
	}
	CloseServiceHandle(l_ServiceHandle);
	return true;
}

//////////////////////////////////////////////////////////
//
//		LocalHostName
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			Local host name if successful, empty if not.
//      Parameters:
//			None.
//		Remarks:
//			Returns the name of the local host.
//
//////////////////////////////////////////////////////////
CString CSysService::LocalHostName()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	char	l_MachineName [MAX_COMPUTERNAME_LENGTH+1];
	DWORD	l_NameSize = sizeof(l_MachineName);
	
	g_LocalHostName.Empty();
	if (! GetComputerName(l_MachineName, &l_NameSize))
	{
		g_Result++;
		g_Error = GetLastError();
		return g_LocalHostName;
	}
	g_LocalHostName.Format("%s", l_MachineName);

	return g_LocalHostName;
}

//////////////////////////////////////////////////////////
//
//		ServiceHostName
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			Name of host connected to.
//      Parameters:
//			None.
//		Remarks:
//			Returns the name of the connected host.
//
//////////////////////////////////////////////////////////
CString CSysService::ServiceHostName()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	return g_ServiceHostName;
}

//////////////////////////////////////////////////////////
//
//		Result
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			Result of last operation.
//      Parameters:
//			None.
//		Remarks:
//			Result of last operation, for operations which
//			return a result.
//
//////////////////////////////////////////////////////////
long CSysService::Result()
{
	return g_Result;
}

//////////////////////////////////////////////////////////
//
//		Error
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			Error value from last operation.
//      Parameters:
//			None.
//		Remarks:
//			Value of GetLastError from last operation which
//			had an error and returned a value.
//
//////////////////////////////////////////////////////////
DWORD CSysService::Error()
{
	return g_Error;
}

//////////////////////////////////////////////////////////
//
//		ErrorMessage
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			CString value of p_Error.
//      Parameters:
//			p_Error
//				Value of error to decode
//		Remarks:
//			Translates p_Error to a printable CString.
//
//////////////////////////////////////////////////////////
CString CSysService::ErrorMessage(DWORD p_Error)
{
	char		l_ErrorBuffer[4096];
	CString		l_ErrorString;

	memset (l_ErrorBuffer, 0, sizeof(l_ErrorBuffer));
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL,
				  p_Error,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  l_ErrorBuffer,
				  sizeof(l_ErrorBuffer),
				  NULL);
	l_ErrorString.Format(l_ErrorBuffer);
	return l_ErrorString;
}

//////////////////////////////////////////////////////////
//
//		ServiceError		
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			DWORD result of last method.
//      Parameters:
//			None.
//		Remarks:
//			Result of last CSysService method.
//
//////////////////////////////////////////////////////////
DWORD CSysService::ServiceError()
{
	return g_ServiceRequestError;
}

//////////////////////////////////////////////////////////
//
//		ServiceErrorMessage
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			CString value of result of last operation.
//      Parameters:
//			p_Error
//				value of error to decode.
//		Remarks:
//			Translates p_Error to a printable string.
//
//////////////////////////////////////////////////////////
CString CSysService::ServiceErrorMessage(DWORD p_Error)
{
	CString		l_ErrorString;

	switch (p_Error)
	{
	case SYSSERVICE_NOERROR:
		l_ErrorString = "No error.";
		break;
	case SYSSERVICE_STATE_ERROR:
		l_ErrorString = "Unknown service status.";
		break;
	case SYSSERVICE_NOT_RUNNING:
		l_ErrorString = "Requested service is not currently running.";
		break;
	case SYSSERVICE_NOT_STOPPED:
		l_ErrorString = "Requested service is not stopped.";
		break;
	case SYSSERVICE_OPEN_ERROR:
		l_ErrorString = "Unable to open requested service.";
		break;
	case SYSSERVICE_UNABLE_TO_PAUSE:
		l_ErrorString = "Unable to pause requested service.";
		break;
	case SYSSERVICE_UNABLE_TO_STOP:
		l_ErrorString = "Unable to stop requested service.";
		break;
	case SYSSERVICE_UNABLE_TO_CONTINUE:
		l_ErrorString = "Unable to resume requested service.";
		break;
	case SYSSERVICE_UNABLE_TO_START:
		l_ErrorString = "Unable to start requested service.";
		break;
	case SYSSERVICE_INVALID_REQUEST:
		l_ErrorString = "Invalid service request.";
		break;
	case SYSSERVICE_QUERY_SERVICE_CONFIG:
		l_ErrorString = "Unable to query service configuration.";
		break;
	case SYSSERVICE_INSUFFICIENT_BUFFER:
		l_ErrorString = "Unable to allocate query service configuration buffer.";
		break;
	case SYSSERVICE_NOT_SERVICE:
		l_ErrorString = "Not a service entry.";
		break;
	case SYSSERVICE_QUERY_SERVICE_STATUS:
		l_ErrorString = "Unable to query service status.";
		break;
	case SYSSERVICE_NULL_SERVICENAME:
		l_ErrorString = "Service name cannot be null.";
		break;
	case SYSSERVICE_GET_DISPLAY_NAME:
		l_ErrorString = "Unable to get service display name.";
		break;
	case SYSSERVICE_OPEN_SC_MANAGER:
		l_ErrorString = "Unable to open service control manager.";
		break;
	default:
		l_ErrorString = "Unknown error.";
	}
	return l_ErrorString;
}

//////////////////////////////////////////////////////////
//
//		ServiceName
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			g_ServiceName
//      Parameters:
//			None.
//		Remarks:
//			Returns current setting of g_ServiceName
//
//////////////////////////////////////////////////////////
CString CSysService::ServiceName()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	return g_ServiceName;
}

//////////////////////////////////////////////////////////
//
//		ServiceDisplayName
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			g_ServiceDisplayName
//      Parameters:
//			None.
//		Remarks:
//			Returns current setting of g_ServiceDisplayName
//
//////////////////////////////////////////////////////////
CString CSysService::ServiceDisplayName()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	return g_ServiceDisplayName;
}

//////////////////////////////////////////////////////////
//
//		ServiceStartName
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			g_ServiceStartName
//      Parameters:
//			None.
//		Remarks:
//			Returns current setting of g_ServiceStartName
//
//////////////////////////////////////////////////////////
CString CSysService::ServiceStartName()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	return g_ServiceStartName;
}

//////////////////////////////////////////////////////////
//
//		ServiceType
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			g_ServiceType
//      Parameters:
//			None.
//		Remarks:
//			Returns current setting of g_ServiceType
//
//////////////////////////////////////////////////////////
DWORD CSysService::ServiceType()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	return g_ServiceType;
}

//////////////////////////////////////////////////////////
//
//		StartType
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			g_StartType
//      Parameters:
//			None.
//		Remarks:
//			Returns current setting of g_StartType
//
//////////////////////////////////////////////////////////
DWORD CSysService::StartType()
{
	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	return g_StartType;
}

//////////////////////////////////////////////////////////
//
//		ServiceState
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			Current service state code.
//      Parameters:
//			p_ServiceName
//				Name of service to query.
//		Remarks:
//			Returns current status of p_ServiceName.
//			Sets g_ServiceState.
//
//////////////////////////////////////////////////////////
DWORD CSysService::ServiceState(CString p_ServiceName)
{
	SC_HANDLE				l_ServiceHandle;

	g_Result = 0;
	g_Error = ERROR_SUCCESS;
	g_ServiceRequestError = SYSSERVICE_NOERROR;

	if (p_ServiceName.IsEmpty())
		p_ServiceName = g_ServiceName;
	g_ServiceName = p_ServiceName;

	l_ServiceHandle = OpenService(g_ServiceHandle, 
								  (LPCTSTR)g_ServiceName, 
								  SERVICE_ALL_ACCESS);

	if(! QueryServiceStatus(l_ServiceHandle, 
							&g_Buffer.Structures.ServiceStatus))
	{
		CloseServiceHandle(l_ServiceHandle);
		return NULL;
	}
	CloseServiceHandle(l_ServiceHandle);
	g_ServiceState = g_Buffer.Structures.ServiceStatus.dwCurrentState;
	return g_ServiceState;
}

//////////////////////////////////////////////////////////
//
//    ServiceConfig
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			Pointer to QUERY_SERVICE_CONFIG
//      Parameters:
//			None.
//		Remarks:
//			Returns a pointer to the global
//			QUERY_SERVICE_CONFIG structure.
//
//////////////////////////////////////////////////////////
QUERY_SERVICE_CONFIG * CSysService::ServiceConfig()
{
	return &g_Buffer.Structures.ServiceConfig;
}

//////////////////////////////////////////////////////////
//
//    ServiceStatus
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			Pointer to SERVICE_STATUS
//      Parameters:
//			None.
//		Remarks:
//			Returns a pointer to the global
//			SERVICE_STATUS structure.
//
//////////////////////////////////////////////////////////
SERVICE_STATUS * CSysService::ServiceStatus()
{
	return &g_Buffer.Structures.ServiceStatus;
}

//////////////////////////////////////////////////////////
//
//    StatusString
//
//////////////////////////////////////////////////////////
//
//		Return Value:
//			CString value of p_ServiceStatus.
//      Parameters:
//			p_ServiceStatus
//				Value of status to lookup.
//		Remarks:
//			Translates p_ServiceStatus to a printable
//			string.
//
//////////////////////////////////////////////////////////
CString CSysService::StatusString(DWORD p_ServiceStatus)
{
	CString l_Status;

	switch(p_ServiceStatus)
	{
		case SERVICE_STOPPED:
			l_Status = _T("Stopped");
			break;
		case SERVICE_START_PENDING:
			l_Status = _T("Starting");
			break;
		case SERVICE_STOP_PENDING:
			l_Status = _T("Stopping");
			break;
		case SERVICE_RUNNING:
			l_Status = _T("Running");
			break;
		case SERVICE_CONTINUE_PENDING:
			l_Status = _T("Continuing");
			break;
		case SERVICE_PAUSE_PENDING:
			l_Status = _T("Pausing");
			break;
		case SERVICE_PAUSED:
			l_Status = _T("Paused");
			break;
		default:
			l_Status = _T("Unknown");
			break;
	}

	return l_Status;
}

//////////////////////////////////////////////
//
//    GetStartupString
//
//////////////////////////////////////////////
//
//		Return Value:
//			CString value of p_StartupType.
//      Parameters:
//			p_StartupType
//				Value of status to lookup.
//		Remarks:
//			Translates p_StartupType to a printable
//			string.
//
//////////////////////////////////////////////////////////
CString CSysService::StartupString(DWORD p_StartupType)
{
	CString l_Startup;

	switch(p_StartupType)
	{
		case SERVICE_BOOT_START:
			l_Startup = _T("Boot");
			break;
		case SERVICE_SYSTEM_START:
			l_Startup = _T("System");
			break;
		case SERVICE_AUTO_START:
			l_Startup = _T("Automatic");
			break;
		case SERVICE_DEMAND_START:
			l_Startup = _T("Manual");
			break;
		case SERVICE_DISABLED:
			l_Startup = _T("Disabled");
			break;
		default:
			l_Startup = _T("Unknown");
			break;
	}

	return l_Startup;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
