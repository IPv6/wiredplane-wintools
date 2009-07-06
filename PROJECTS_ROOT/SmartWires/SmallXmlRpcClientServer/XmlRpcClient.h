// XmlRpcClient.h: interface for the CXmlRpcClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLRPCCLIENT_H__8120A9E8_B6F3_43B9_A873_0FB84DA43710__INCLUDED_)
#define AFX_XMLRPCCLIENT_H__8120A9E8_B6F3_43B9_A873_0FB84DA43710__INCLUDED_
#include <afxmt.h>
#include "exports.h"

class CXmlRpcClient  
{
private:
	static CRITICAL_SECTION MapLock;
	static CMapWordToPtr m_CallsInProcess;
	static long lLastCallNumber;
	static long lTotalCalls;
	static long lErrorCalls;
	static long lBytesSend;
	static long lBytesReceived;
	static long lTimeOutToHARDTerminateCall;
	static long lRequestsBetweenCleaup;
	static long lLockTimeout;
	static OnDataReceivedCallbackFunc OnReceived;
	static OnErrorCallbackFunc OnError;
	static OnDataSendedCallbackFunc OnSended;

public:
	CXmlRpcClient();
	virtual ~CXmlRpcClient();
	static BOOL SetCallback_onReceived(OnDataReceivedCallbackFunc func);
	static BOOL SetCallback_onSended(OnDataSendedCallbackFunc func);
	static BOOL SetCallback_onError(OnErrorCallbackFunc func);
	static BOOL OnErrorFirst(SOCKET* sock, const char* szData, CActionError& error);
	static BOOL InitiateXmlRpcCall(const char* szURI, const char* szFunction, XmlRpcValues& parameters, XmlRpcResult& result, DWORD dwMilliseconds, CActionError& error);
	static BOOL AddXmlRpcCall(XmlRpcRequest& Request, XmlRpcResult& result, DWORD dwMilliseconds, CActionError& error, OnXmlRpcOnMethodReturnedCallbackFunc func=NULL, XmlRpcRequestUserData* UserData=NULL);
	static BOOL TerminateCall(int iIndex);
	static BOOL TerminateAll();
	static BOOL CleanRequestBuffer();
	static BOOL SetClientParams(long lTimeoutToHardTermination, long lReqBetweenCleanUp);
	static void GetStatistics(long& lTotal, long& lErrors, long& lInProcess, long& FinishedButNotDeleted,long& lBSend, long& lBytesRec)
	{
		lTotal=lTotalCalls;
		lErrors=lErrorCalls;
		lBSend=lBytesSend;
		lBytesRec=lBytesReceived;
		lInProcess=0;
		FinishedButNotDeleted=0;
		WORD dwID;
		void* pCall;
		POSITION p=m_CallsInProcess.GetStartPosition();
		while(p!=NULL){
			m_CallsInProcess.GetNextAssoc(p,dwID,pCall);
			if(((XmlRpcRequestEx*)pCall)->bStatus==3){
				FinishedButNotDeleted++;
			};
			if(((XmlRpcRequestEx*)pCall)->bStatus==2){
				lInProcess++;
			};
		};
	};
};
CXmlRpcClient& XmlRpcClients();
#endif // !defined(AFX_XMLRPCCLIENT_H__8120A9E8_B6F3_43B9_A873_0FB84DA43710__INCLUDED_)
