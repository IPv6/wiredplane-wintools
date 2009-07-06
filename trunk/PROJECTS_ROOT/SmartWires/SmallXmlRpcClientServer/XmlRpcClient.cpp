// XmlRpcClient.cpp: implementation of the CXmlRpcClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmallXmlRpcClientServer.h"
#ifndef _DEBUG2_
#include "DLG_ClientTest.h"
#endif
#include "XmlRpcClient.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXmlRpcClient& XmlRpcClients()
{
	static CXmlRpcClient g_XmlRpcClient;
	return g_XmlRpcClient;
}

CMapWordToPtr CXmlRpcClient::m_CallsInProcess;
CRITICAL_SECTION CXmlRpcClient::MapLock;
long CXmlRpcClient::lLastCallNumber=1;
long CXmlRpcClient::lTotalCalls=0;
long CXmlRpcClient::lErrorCalls=0;
long CXmlRpcClient::lBytesSend=0;
long CXmlRpcClient::lBytesReceived=0;
long CXmlRpcClient::lTimeOutToHARDTerminateCall=2000;
long CXmlRpcClient::lRequestsBetweenCleaup=100;
long CXmlRpcClient::lLockTimeout=1000;
OnDataReceivedCallbackFunc CXmlRpcClient::OnReceived=0;
OnErrorCallbackFunc CXmlRpcClient::OnError=0;
OnDataSendedCallbackFunc CXmlRpcClient::OnSended=0;

CXmlRpcClient::CXmlRpcClient()
{
	InitializeCriticalSection(&CXmlRpcClient::MapLock);
}

BOOL CXmlRpcClient::SetClientParams(long lTimeoutToHardTermination, long lReqBetweenCleanUp)
{
	CXmlRpcClient::lTimeOutToHARDTerminateCall=lTimeoutToHardTermination;
	CXmlRpcClient::lRequestsBetweenCleaup=lReqBetweenCleanUp;
	return TRUE;
};


CXmlRpcClient::~CXmlRpcClient()
{
	TerminateAll();
	DeleteCriticalSection(&CXmlRpcClient::MapLock);
}

BOOL CXmlRpcClient::TerminateCall(int iIndex)
{
	EnterCriticalSection(&CXmlRpcClient::MapLock);
	void* pVal;
	if(m_CallsInProcess.Lookup(iIndex,pVal)){
		XmlRpcRequestEx* pCall=(XmlRpcRequestEx*)pVal;
		if(pCall->bStatus!=3){
			pCall->bStatus=0;
		}
		DWORD dwCurTime=GetTickCount();
		while(pCall->bStatus!=3 && (dwCurTime+lTimeOutToHARDTerminateCall<GetTickCount())){
			Sleep(10);
		}
		TerminateThread(pCall->hThread,0);
		delete pCall;
		m_CallsInProcess.RemoveKey(iIndex);
	}
	LeaveCriticalSection(&CXmlRpcClient::MapLock);
	return TRUE;
}

BOOL CXmlRpcClient::TerminateAll()
{
	BOOL bRes=FALSE;
	CleanRequestBuffer();
	while(m_CallsInProcess.GetCount()>0){
		POSITION p=m_CallsInProcess.GetStartPosition();
		WORD dwID;
		void* pCall;
		m_CallsInProcess.GetNextAssoc(p,dwID,pCall);
		bRes=bRes | TerminateCall(dwID);
	}
	return bRes;
}

BOOL CXmlRpcClient::CleanRequestBuffer()
{
	EnterCriticalSection(&CXmlRpcClient::MapLock);
	CDWordArray itemToDelete;
	WORD dwID;
	void* pCall;
	POSITION p=m_CallsInProcess.GetStartPosition();
	while(p!=NULL){
		m_CallsInProcess.GetNextAssoc(p,dwID,pCall);
		if(((XmlRpcRequestEx*)pCall)->bStatus==3){
			delete ((XmlRpcRequestEx*)pCall);
			itemToDelete.Add(dwID);
		}
	}
	for(int i=0;i<itemToDelete.GetSize();i++){
		m_CallsInProcess.RemoveKey((WORD)itemToDelete[i]);
	}
	LeaveCriticalSection(&CXmlRpcClient::MapLock);
	return TRUE;
}

BOOL CXmlRpcClient::SetCallback_onSended(OnDataSendedCallbackFunc func)
{
	OnSended=func;
	return TRUE;
}

BOOL CXmlRpcClient::SetCallback_onReceived(OnDataReceivedCallbackFunc func)
{
	OnReceived=func;
	return TRUE;
}

BOOL CXmlRpcClient::SetCallback_onError(OnErrorCallbackFunc func)
{
	OnError=func;
	return TRUE;
}

BOOL CXmlRpcClient::OnErrorFirst(SOCKET* sock, const char* szData, CActionError& error)
{
	if(OnError!=NULL){
		if(OnError(-1,sock, szData, error)==TRUE){
			return TRUE;
		}
	}
	closesocket(*sock);
	return TRUE;
}

DWORD WINAPI MakeXmlRpcCallWithCallback(LPVOID lpdwstatus)
{
	XmlRpcRequestEx* RequestEx=(XmlRpcRequestEx*)lpdwstatus;
	try{
		if(RequestEx->bStatus!=1){
			RequestEx->error.setError(AE_AsyncCallParametersNotValid);
			throw RequestEx->error;
		}
		RequestEx->bStatus=2;
		// Результат не проверяем, т.к. все нужные вызовы будут в CXmlRpcClient::InitiateXmlRpcCall
		CXmlRpcClient::InitiateXmlRpcCall(RequestEx->szURI, RequestEx->szMethodName, RequestEx->values, RequestEx->result, RequestEx->dwTimeout, RequestEx->error);
		if(RequestEx->bStatus!=2){
			RequestEx->error.setError(AE_AsyncCallTerminating);
			throw RequestEx->error;
		}
		// Вызываем пользовательскую функцию
		if(RequestEx->func!=NULL){
			RequestEx->func(RequestEx);
		}
	}catch(CActionError e){
	}
	RequestEx->bStatus=3;
	return 0;
}

BOOL CXmlRpcClient::AddXmlRpcCall(XmlRpcRequest& Request, XmlRpcResult& result, DWORD dwMilliseconds, CActionError& error, OnXmlRpcOnMethodReturnedCallbackFunc func, XmlRpcRequestUserData* UserData)
{
	if(func!=NULL){
		// Для вызова с callback не используются XmlRpcResult& result и CActionError& error
		if(lLastCallNumber%CXmlRpcClient::lRequestsBetweenCleaup==0){
			// Чистим
			CleanRequestBuffer();
		}
		XmlRpcRequestEx* RequestEx=new XmlRpcRequestEx;
		RequestEx->bStatus=1;
		RequestEx->CallID=(WORD)lLastCallNumber;
		RequestEx->dwTimeout=dwMilliseconds;
		RequestEx->func=func;
		RequestEx->szURI=strdup(Request.szURI);
		RequestEx->szMethodName=strdup(Request.szMethodName);
		RequestEx->values=Request.values;
		RequestEx->UserData=UserData;
		DWORD dwThreadID;
		RequestEx->hThread=CreateThread(NULL,0,MakeXmlRpcCallWithCallback,(LPVOID)RequestEx,0,&dwThreadID);//!!! :)
		EnterCriticalSection(&CXmlRpcClient::MapLock);
		m_CallsInProcess[RequestEx->CallID]=RequestEx;
		LeaveCriticalSection(&CXmlRpcClient::MapLock);
		InterlockedIncrement(&lLastCallNumber);
		return TRUE;
	}else{
		return InitiateXmlRpcCall(Request.szURI, Request.szMethodName, Request.values, result, dwMilliseconds, error);
	}
}

BOOL CXmlRpcClient::InitiateXmlRpcCall(const char* szURI, const char* szFunction, XmlRpcValues& parameters, XmlRpcResult& result, DWORD dwMilliseconds, CActionError& error)
{
	CString sXml;
	RET_IF_FAULT(CreateXmlFromRpcMethodCallRequest(szFunction, &parameters, sXml, error));
	CString sHost,sQuery;
	GetHostAndQueryFromURI(szURI,sHost,sQuery);
	CString sRequest=CreateRequest(sHost, sQuery, sXml);
	// Теперь надо послать это все по сокету
	// Ищем порт...
	int nPort=80;
	int iPortPos=sHost.Find(":");
	if(iPortPos!=-1){
		nPort=atol(sHost.Mid(iPortPos+1));
		sHost=sHost.Left(iPortPos);
	}
	SOCKET sock;
	sock=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in SrvAddress;
	SrvAddress.sin_family=AF_INET;
	SrvAddress.sin_addr.s_addr=inet_addr(sHost);
	SrvAddress.sin_port=htons(nPort);
	int res=connect(sock,(PSOCKADDR)&SrvAddress,sizeof(SrvAddress));
	if(res==SOCKET_ERROR){
		error.setError(AE_CantConnectToSocket,GetLastWSAError());
		closesocket(sock);
		return FALSE;
	}
	//Шлем запрос
	long lAmountOfBytes=0;
	if(SendTextToSocket(&sock, sRequest, &lAmountOfBytes, error)==FALSE){
		OnErrorFirst(&sock, sRequest, error);
		return FALSE;
	}
	InterlockedExchangeAdd(&lBytesSend,lAmountOfBytes);
	if(OnSended!=NULL){
		OnSended(-1,sRequest,lAmountOfBytes);
	}
	// Ждем ответ!
	CString sResultData;
	CString sReqBody;
	DWORD dwTotalLoaded=0;
	if(LoadHTTPRequestFromSocket(&sock, sResultData, sReqBody, dwTotalLoaded, dwMilliseconds, error)==FALSE){
		OnErrorFirst(&sock,sReqBody,error);
		return FALSE;
	}
	InterlockedExchangeAdd(&lBytesReceived,dwTotalLoaded);
	if(OnReceived!=NULL){
		OnReceived(-1,sResultData,dwTotalLoaded);
	}
	if(CreateXmlRpcResultFromXml(sReqBody, result, error)==FALSE){
		OnErrorFirst(&sock,sReqBody,error);
		return FALSE;
	}
	closesocket(sock);
	return error.allOk();
}

#ifndef _DEBUG2_
void TestClientSide()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DLG_ClientTest* dlg=new DLG_ClientTest;
	dlg->Create(DLG_ClientTest::IDD);
	dlg->ShowWindow(TRUE);
};
#endif

BOOL InitXmlRpcClient(long lTimeoutToHardTermination, long lReqBetweenCleanUp)
{
	CXmlRpcClient::SetClientParams(lTimeoutToHardTermination, lReqBetweenCleanUp);
	return TRUE;
};

BOOL DeinitXmlRpcClient()
{
	return FALSE;
};

BOOL MakeXmlRpcCall(XmlRpcRequest& Request, XmlRpcResult& result, DWORD dwMilliseconds, CActionError& error)
{
	XmlRpcClients().AddXmlRpcCall(Request,result,dwMilliseconds,error);
	return error.allOk();
};

BOOL MakeCallbackXmlRpcCall(XmlRpcRequest& Request, DWORD dwMilliseconds, OnXmlRpcOnMethodReturnedCallbackFunc func, XmlRpcRequestUserData* UserData)
{
	CActionError error;
	XmlRpcResult result;
	XmlRpcClients().AddXmlRpcCall(Request,result,dwMilliseconds,error,func,UserData);
	return error.allOk();
};

BOOL TerminateAllCalls()
{
	XmlRpcClients().TerminateAll();
	return TRUE;
};

BOOL GetClientStatistics(long& lTotal, long& lErrors, long& lInProcess, long& lFinishedButNotDeleted, long& lBytesSend, long& lBytesRec)
{
	XmlRpcClients().GetStatistics(lTotal, lErrors, lInProcess, lFinishedButNotDeleted, lBytesSend, lBytesRec);
	return TRUE;
};
