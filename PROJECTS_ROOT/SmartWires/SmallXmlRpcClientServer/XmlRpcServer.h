// XmlRpcServer.h: interface for the CXmlRpcServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLRPCSERVER_H__13EEBF83_932E_4FDF_A043_17A6B86BBDCC__INCLUDED_)
#define AFX_XMLRPCSERVER_H__13EEBF83_932E_4FDF_A043_17A6B86BBDCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "exports.h"

#define RET_IF_FAULT(x)	if((x)==FALSE) return FALSE;

class CXmlRpcServer
{
	int m_iThisIndex;
	SOCKET m_sock;
	HANDLE m_hServer;
	DWORD m_dwThreadID;
	CXmlRpcServerInfo m_Info;
	static CXmlRpcServerStatistics m_Statistics;
public:
	CXmlRpcServer()
	{
		m_iThisIndex=-1;
		m_hServer=NULL;
		m_dwThreadID=0;
		m_Info.m_iStatus=-1;
		OnReceived=NULL;
		OnSended=NULL;
		OnError=NULL;
	};
	virtual ~CXmlRpcServer(){};

	OnDataReceivedCallbackFunc OnReceived;
	OnErrorCallbackFunc OnError;
	OnDataSendedCallbackFunc OnSended;
	BOOL SetCallback_onReceived(OnDataReceivedCallbackFunc func);
	BOOL SetCallback_onError(OnErrorCallbackFunc func);
	BOOL SetCallback_onSended(OnDataSendedCallbackFunc func);


	BOOL InitServer(int iPort, int iMaxConnect, int iBetweenReqTime, DWORD dwGetReqTimeout);
	BOOL TerminateServer(CActionError& error);
	BOOL RunServer(CActionError& error);
	BOOL SuspendServer(CActionError& error);
	BOOL ResumeServer(CActionError& error);
	BOOL GetInfo(CXmlRpcServerInfo& info);
	SOCKET* GetSock();
	BOOL GetStats(CXmlRpcServerStatistics& stats);
	BOOL AddMethod(const char* szMethodName, OnXmlRpcMethodCallbackFunc lpFunction, CActionError& error);
	BOOL IsRunning();
	CXmlRpcServerStatistics* ModifyStats(){return &m_Statistics;};
	int GetServerIndex(){return m_iThisIndex;};
	int GetWaitTime(){return m_Info.m_iBetweenReqTime;};
	void SetServerIndex(int iIndex){m_iThisIndex=iIndex;};
	OnXmlRpcMethodCallbackFunc GetMethod(const char* szName);
	DWORD GetTimeoutForListeningRequests(){return m_Info.m_ListenTimeout;};
};

class CXmlRpcServers
{
	CArray<CXmlRpcServer*,CXmlRpcServer*> m_Servers;
public:
	CXmlRpcServers(){};

	virtual ~CXmlRpcServers()
	{
		for(int i=0;i<m_Servers.GetSize();i++){
			if(m_Servers[i]){
				CXmlRpcServer* pServer=m_Servers[i];
				m_Servers[i]=NULL;
				delete pServer;
			}
		}
		m_Servers.RemoveAll();
	};
	BOOL CheckIndex(int iServerNum, CActionError& error);
	int Add(CXmlRpcServer* server, CActionError& error);

	/** Удаляет сервер. не сдвигает массив вниз, т.о. порядок других серверов не меняется
	*	@param	int - номер сервера
	*	@param	CActionError& - ошибка
	*	@return	BOOL - результат
	*/
	BOOL Remove(int iServerNum, CActionError& error);
	CXmlRpcServer* GetServer(int iServerNum, CActionError& error);
};
CXmlRpcServers& XmlRpcServers();
#endif // !defined(AFX_XMLRPCSERVER_H__13EEBF83_932E_4FDF_A043_17A6B86BBDCC__INCLUDED_)
