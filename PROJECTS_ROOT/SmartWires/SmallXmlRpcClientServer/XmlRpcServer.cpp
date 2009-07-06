// XmlRpcServer.cpp: implementation of the CXmlRpcServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmallXmlRpcClientServer.h"
#include "XmlRpcServer.h"
#ifndef _DEBUG2_
#include "DLG_ServerTest.h"
#endif
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CXmlRpcServerStatistics CXmlRpcServer::m_Statistics;
CXmlRpcServers& XmlRpcServers()
{
	static CXmlRpcServers g_XmlRpcServers;
	return g_XmlRpcServers;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CXmlRpcServers::Add(CXmlRpcServer* server, CActionError& error)
{
	int iIndex=m_Servers.Add(server);
	server->SetServerIndex(iIndex);
	return server->GetServerIndex();
}

BOOL CXmlRpcServers::CheckIndex(int iServerNum, CActionError& error)
{
	if(iServerNum<0 || iServerNum>=m_Servers.GetSize()){
		error.setError(AE_ServerIndexInvalid);
		return FALSE;
	}
	return TRUE;
}

BOOL CXmlRpcServers::Remove(int iServerNum, CActionError& error)
{
	RET_IF_FAULT(CheckIndex(iServerNum,error));
	m_Servers[iServerNum]->TerminateServer(error);
	delete m_Servers[iServerNum];
	m_Servers[iServerNum]=NULL;
	return TRUE;
}

CXmlRpcServer* CXmlRpcServers::GetServer(int iServerNum, CActionError& error)
{
	if(!CheckIndex(iServerNum,error)){
		return 0;
	}
	return m_Servers[iServerNum];
}

BOOL CXmlRpcServer::InitServer(int iPort, int iMaxConnect, int iBetweenReqTime, DWORD dwGetReqTimeout)
{
	m_Info.m_iPort=iPort;
	m_Info.m_iMaxConnect=iMaxConnect;
	m_Info.m_iStatus=0;
	m_Info.m_iBetweenReqTime=iBetweenReqTime;
	m_Info.m_ListenTimeout=dwGetReqTimeout;
	return TRUE;
}

BOOL CXmlRpcServer::TerminateServer(CActionError& error)
{
	if(m_Info.m_iStatus==-1){
		error.setError(AE_ServerNotInitialized);
		return FALSE;
	}
	if(m_Info.m_iStatus==0){
		error.setError(AE_ServerNotCreated);
		return FALSE;
	}
	shutdown(m_sock,2);
    LINGER linger = { 1, 0 };
    setsockopt(m_sock, SOL_SOCKET, SO_LINGER, (char*)& linger, sizeof (linger));
	closesocket(m_sock);
	m_dwThreadID=0;
	m_Info.m_iStatus=0;
	// Ждем остановки потока...
	WaitForSingleObject(m_hServer,10*1000);//10 секунд на останов
	return TRUE;
}

struct RequestHandlerStruct{
	CXmlRpcServer* server;
	SOCKET* sock;
};

DWORD WINAPI RequestHandler(LPVOID lpdwstatus)
{
	RequestHandlerStruct* pReq=(RequestHandlerStruct*)lpdwstatus;
	CActionError error;
	CString sRequestData;
	try{
		// Уррааа! получен запрос, обрабатываем
		DWORD dwTotalLoaded;
		CString sBody;
		if(!LoadHTTPRequestFromSocket(pReq->sock, sRequestData,sBody, dwTotalLoaded, pReq->server->GetTimeoutForListeningRequests(), error)){
			throw error;
		}
		pReq->server->ModifyStats()->AddReceivedBytes(dwTotalLoaded);
		if(pReq->server->OnReceived){
			if(pReq->server->OnReceived(pReq->server->GetServerIndex(),sRequestData,dwTotalLoaded)==TRUE){
				// Запрос обработан callback функцией
				error.resetError();
				throw error;
			};
		}
		CString sContentType;
		if(GetHTTPHeaderFieldValue(sRequestData, "Content-Type", sContentType)==FALSE){
			error.setError(AE_InvalidRequestHeader);
			throw error;
		}
#ifdef USE_STRICT_CONTENTTYPE
		if(sContentType!="text/xml"){
			error.setError(AE_InvalidRequestContentType);
			throw error;
		}
#endif
		// Парсируем запрос в структуру
		CString sURI;
		if(GetHTTPRequestURI(sRequestData, sURI)==FALSE){
			error.setError(AE_InvalidHTTPRequestURI);
			throw error;
		}
		// Вытаскиваем имя функции
		CString sMethodName=GetInstringPart("<methodName>","</methodName>",sBody);
		// Есть ли она в списке
		OnXmlRpcMethodCallbackFunc methodHandler=pReq->server->GetMethod(sMethodName);
		if(methodHandler==NULL){
			error.setError(AE_XmlRpcMethodNotExist);
			throw error;
		}
		XmlRpcRequest Request;
		if(CreateXmlRpcValuesFromXml(sBody,Request.values,error)==FALSE){
			error.setError(AE_ErrorConvertingXmlToValues);
			throw error;
		}
		Request.szMethodName=sMethodName;
		Request.szURI=sURI;
		XmlRpcResult MethodResult;
		BOOL bRes=methodHandler(pReq->server->GetServerIndex(),&Request,&MethodResult);
		if(!bRes){
			// Ошибка, возвращаем ее
			throw MethodResult.error;
		}
		// шлем ответ!
		CString sXml;
		if(CreateXmlFromRpcMethodResult(MethodResult, sXml, error)==FALSE){
			error.setError(AE_ErrorConvertingValuesToXml);
			throw error;
		}
		//>>>>>>>>>>>>>>>>
		CString sSendData=CreateReply(sXml);
		long lSendedBytes=0;
		SendTextToSocket(pReq->sock,sSendData,&lSendedBytes,error);
		pReq->server->ModifyStats()->AddSendedBytes(lSendedBytes);
		if(pReq->server->OnSended){
			pReq->server->OnSended(pReq->server->GetServerIndex(),sSendData,lSendedBytes);
		}
		//>>>>>>>>>>>>>>>>>
		// Финальная проверка на ошибки
		if(!error.allOk()){
			throw error;
		}
		// Все сделано, можно пойти попить кофе
		pReq->server->ModifyStats()->AddHandled_Total();
	}catch(CActionError e){
		// обрабатываем ошибку
		pReq->server->ModifyStats()->AddHandled_Error();
		BOOL bErrorHandled=FALSE;
		if(pReq->server->OnError){
			bErrorHandled=pReq->server->OnError(pReq->server->GetServerIndex(),pReq->sock,sRequestData,e);
		}
		//>>>>>>>>>>>>>>>>>
		if(!bErrorHandled){
			CString sSendData=CreateReply(e);
			long lSendedBytes=0;
			SendTextToSocket(pReq->sock,sSendData,&lSendedBytes,e);
			pReq->server->ModifyStats()->AddSendedBytes(lSendedBytes);
			if(pReq->server->OnSended){
				pReq->server->OnSended(pReq->server->GetServerIndex(),sSendData,lSendedBytes);
			}
		}
		//>>>>>>>>>>>>>>>>>
	}
	closesocket(*pReq->sock);
	delete pReq;
	return (error.allOk());
}

DWORD WINAPI StartSocketAccepter(LPVOID lpdwstatus)
{
	CXmlRpcServer* thisServer=(CXmlRpcServer*)lpdwstatus;
	while(thisServer->IsRunning()){
		SOCKET connectedSock;
		connectedSock=accept(*thisServer->GetSock(), NULL, NULL);
		if(connectedSock!=INVALID_SOCKET){
			// Создаем поток для обработки
			DWORD dwThreadID=0;
			RequestHandlerStruct* pReq=new RequestHandlerStruct;
			pReq->server=thisServer;
			pReq->sock=&connectedSock;
			HANDLE hThread=CreateThread(NULL,0,RequestHandler,(LPVOID)pReq,0,&dwThreadID);
		}else{
			// Ждем чуть-чуть... 
			Sleep(thisServer->GetWaitTime());
		}
	}
	return 0;
}

BOOL CXmlRpcServer::RunServer(CActionError& error)
{
	try{
		if(m_Info.m_iStatus==-1){
			error.setError(AE_ServerNotInitialized);
			return FALSE;
		}
		if(m_Info.m_iStatus==1){
			error.setError(AE_ServerAlreadyRunning);
			return FALSE;
		}
		if(m_Info.m_iSuspended==0){
			return ResumeServer(error);
		}
		// Создаем
		m_sock=socket(AF_INET,SOCK_STREAM,0);
		if(m_sock==INVALID_SOCKET){
			error.setError(AE_WSACantCreateSocket,GetLastWSAError());
			throw error;
		}
		// Биндим
		struct sockaddr_in SrvAddress;
		SrvAddress.sin_family=AF_INET;
		SrvAddress.sin_addr.s_addr=INADDR_ANY;
		SrvAddress.sin_port=htons(m_Info.m_iPort);
		if(bind(m_sock,(LPSOCKADDR)&SrvAddress,sizeof(SrvAddress))==SOCKET_ERROR){
			error.setError(AE_WSACantBindSocket,GetLastWSAError());
			closesocket(m_sock);
			throw error;
		}
		// слушаем
		if(listen(m_sock,m_Info.m_iMaxConnect)==SOCKET_ERROR){
			error.setError(AE_WSACantListenSocket,GetLastWSAError());
			throw error;
		}
		// создаем поток который будет слушать
		m_hServer=CreateThread(NULL,0,StartSocketAccepter,(LPVOID)this,0,&m_dwThreadID);
		if(!m_hServer){
			error.setError(AE_CantCreateThread);
			throw error;
		}
		// все ок, сервер создан
		m_Info.m_iStatus=1;
		m_Info.m_iSuspended=0;
	}catch(CActionError e){
		// Ошибка в error уже есть
	}
	return error.allOk();
}

BOOL CXmlRpcServer::IsRunning()
{
	return (m_Info.m_iStatus==1 && m_Info.m_iSuspended==0);
}

BOOL CXmlRpcServer::SuspendServer(CActionError& error)
{
	if(m_Info.m_iStatus==-1 || m_dwThreadID==0){
		error.setError(AE_ServerNotInitialized);
		return FALSE;
	}
	if(m_Info.m_iStatus==0){
		error.setError(AE_ServerNotCreated);
		return FALSE;
	}
	if(m_Info.m_iSuspended==1){
		error.setError(AE_ServerAlreadySuspended);
		return FALSE;
	}
	SuspendThread(m_hServer);
	m_Info.m_iSuspended=1;
	return TRUE;
}

BOOL CXmlRpcServer::ResumeServer(CActionError& error)
{
	if(m_Info.m_iStatus==-1 || m_dwThreadID==0){
		error.setError(AE_ServerNotInitialized);
		return FALSE;
	}
	if(m_Info.m_iStatus==0){
		error.setError(AE_ServerNotCreated);
		return FALSE;
	}
	if(m_Info.m_iSuspended==0){
		error.setError(AE_ServerAlreadyRunning);
		return FALSE;
	}
	ResumeThread(m_hServer);
	m_Info.m_iStatus=1;
	m_Info.m_iSuspended=1;
	return TRUE;
}

BOOL CXmlRpcServer::GetInfo(CXmlRpcServerInfo& info)
{
	info=this->m_Info;
	return TRUE;
}

SOCKET* CXmlRpcServer::GetSock()
{
	return &m_sock;
}

BOOL CXmlRpcServer::GetStats(CXmlRpcServerStatistics& stats)
{
	stats.lRequestHandled_Error=m_Statistics.lRequestHandled_Error;
	stats.lRequestHandled_Total=m_Statistics.lRequestHandled_Total;
	stats.lTotalBytesReceived=m_Statistics.lTotalBytesReceived;
	stats.lTotalBytesSended=m_Statistics.lTotalBytesSended;
	return TRUE;
}

BOOL CXmlRpcServer::SetCallback_onSended(OnDataSendedCallbackFunc func)
{
	OnSended=func;
	return TRUE;
}


BOOL CXmlRpcServer::SetCallback_onReceived(OnDataReceivedCallbackFunc func)
{
	OnReceived=func;
	return TRUE;
}

BOOL CXmlRpcServer::SetCallback_onError(OnErrorCallbackFunc func)
{
	OnError=func;
	return TRUE;
}

BOOL CXmlRpcServer::AddMethod(const char* szMethodName, OnXmlRpcMethodCallbackFunc lpFunction, CActionError& error)
{
	void* func;
	if(m_Info.m_iStatus==1){
		error.setError(AE_ServerIsRunning);
		return FALSE;
	}
	if(m_Info.MethodMap.Lookup(szMethodName,func)){
		error.setError(AE_MethodAlreadyExist);
		return FALSE;
	};
	m_Info.MethodMap.SetAt(szMethodName,lpFunction);
	return TRUE;
}

OnXmlRpcMethodCallbackFunc CXmlRpcServer::GetMethod(const char* szName)
{
	void* func;
	if(!m_Info.MethodMap.Lookup(szName,func)){
		return NULL;
	};
	return (OnXmlRpcMethodCallbackFunc)func;
}

BOOL CALLBACK Copyrights(int iServerNum, XmlRpcRequest* lpRequest, XmlRpcResult* lpResult)
{
#ifdef _DEBUG2_
	lpResult->values.AddValue(new XmlRpcValue("(C)2002. Created by IPv6"));
#else
	lpResult->values.AddValue(new XmlRpcValue("(C)2002. WPLabs group company. All rights reserved"));
#endif
	return TRUE;
}

BOOL CALLBACK ServerVersion(int iServerNum, XmlRpcRequest* lpRequest, XmlRpcResult* lpResult)
{
	lpResult->values.AddValue(new XmlRpcValue(SERVER_VERSION));
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// Экспортные функции
//////////////////////////////////////////////////////////////////////
#ifndef _DEBUG2_
void TestServerSide()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DLG_ServerTest* dlg=new DLG_ServerTest;
	BOOL bRes=dlg->Create(DLG_ServerTest::IDD);
	dlg->ShowWindow(TRUE);
}
#endif

int CreateServer(int iPort, int iBacklog, int iSleepBetweenReq, DWORD dwGetReqTimeout, CActionError& error)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	error.resetError();
	CXmlRpcServer* pNewServer=new CXmlRpcServer();
	pNewServer->InitServer(iPort,iBacklog,iSleepBetweenReq,dwGetReqTimeout);
	int iServerIndex=XmlRpcServers().Add(pNewServer,error);
	// Добавляем метод - копирайт
	AddXmlRpcMethod(iServerIndex,"server.copyright",Copyrights,error);
	// Добавляем метод - версия сервера
	AddXmlRpcMethod(iServerIndex,"server.version",ServerVersion,error);
	// Все гoтово - возвращаемся
	return iServerIndex;
};

BOOL StartServer(int iServerNum, CActionError& error)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	error.resetError();
	CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
	RET_IF_FAULT(server!=NULL);
	server->RunServer(error);
	RET_IF_FAULT(error.allOk());
	return TRUE;
};

BOOL PauseServer(int iServerNum, CActionError& error)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	error.resetError();
	CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
	RET_IF_FAULT(server!=NULL);
	server->SuspendServer(error);
	RET_IF_FAULT(error.allOk());
	return TRUE;
};

BOOL StopServer(int iServerNum, CActionError& error)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	error.resetError();
	XmlRpcServers().Remove(iServerNum,error);
	RET_IF_FAULT(error.allOk());
	return TRUE;
};

BOOL GetServerInfo(int iServerNum, CXmlRpcServerInfo& ServerInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CActionError error;
	CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
	RET_IF_FAULT(server!=NULL);
	server->GetInfo(ServerInfo);
	return error.allOk();
};

BOOL GetServerStatistics(int iServerNum, CXmlRpcServerStatistics& ServerStatistics)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CActionError error;
	CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
	RET_IF_FAULT(server!=NULL);
	server->GetStats(ServerStatistics);
	return error.allOk();
};

BOOL AddXmlRpcMethod(int iServerNum, const char* szMethodName, OnXmlRpcMethodCallbackFunc lpFunction, CActionError& error)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	error.resetError();
	CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
	RET_IF_FAULT(server!=NULL);
	server->AddMethod(szMethodName, lpFunction, error);
	return error.allOk();
};

