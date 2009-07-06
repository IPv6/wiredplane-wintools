// XmlRpcCommon.cpp: implementation of the XmlRpcCommon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmallXmlRpcClientServer.h"
#include "XmlRpcCommon.h"

BOOL OnXmlRpcServerDataSended(int iServerNum, OnDataSendedCallbackFunc lpFunction)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CActionError error;
	if(iServerNum==-1){
		XmlRpcClients().SetCallback_onSended(lpFunction);
	}else{
		CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
		RET_IF_FAULT(server!=NULL);
		server->SetCallback_onSended(lpFunction);
	}
	return error.allOk();
};

BOOL OnXmlRpcServerDataReceived(int iServerNum, OnDataReceivedCallbackFunc lpFunction)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CActionError error;
	if(iServerNum==-1){
		XmlRpcClients().SetCallback_onReceived(lpFunction);
	}else{
		CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
		RET_IF_FAULT(server!=NULL);
		server->SetCallback_onReceived(lpFunction);
	}
	return error.allOk();
};

BOOL OnXmlRpcServerError(int iServerNum, OnErrorCallbackFunc lpFunction)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CActionError error;
	if(iServerNum==-1){
		XmlRpcClients().SetCallback_onError(lpFunction);
	}else{
		CXmlRpcServer* server=XmlRpcServers().GetServer(iServerNum,error);
		RET_IF_FAULT(server!=NULL);
		server->SetCallback_onError(lpFunction);
	}
	return error.allOk();
};