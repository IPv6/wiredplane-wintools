#include "stdafx.h"
#include "..\SmartWires\SmallXmlRpcClientServer\exports.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CALLBACK XmlRpcReceiveMessage(int iServerNum, XmlRpcRequest* lpRequest, XmlRpcResult* lpResult)
{
	CActionError error;
	CString sFrom=lpRequest->values.GetAsString(0,error);
	CString sTo=lpRequest->values.GetAsString(1,error);
	CString sMessage=lpRequest->values.GetAsString(2,error);
	CString sAttach=lpRequest->values.GetAsString(3,error);
	CDataXMLSaver::Xml2Str(sMessage);
	sFrom+="/xpc";
	OpenMessage(sFrom,sTo,sMessage,"",sAttach);
	lpResult->values<<"OK";
	return TRUE;
}

BOOL CALLBACK XmlRpcExchangeStatus(int iServerNum, XmlRpcRequest* lpRequest, XmlRpcResult* lpResult)
{
	CActionError error;
	UpdateUserIconFromStatusStringID(lpRequest->values.GetAsString(0,error));
	CString sOtherLik=lpRequest->values.GetAsString(1,error);
	lpResult->values<<CreateMyStatusStringID(objSettings.sLocalComputerName);
	return TRUE;
}

#define MAXBUF 256
#define PUERTO 5000
#define GRUPO "225.1.1.1"
#include <winsock.h>
#include <ws2tcpip.h>
int s, n, r;
struct sockaddr_in srv, cli;
char buf[MAXBUF];
struct ip_mreq mreq;
DWORD WINAPI UDPTranslater(LPVOID p)
{
	/*
	WSADATA wsaData;
    if ((WSAStartup(0x202,&wsaData)) != 0) {
//        WSACleanup();
        return -1;
    }
	CNetXPCProtocol* pThis=(CNetXPCProtocol*)p;
	ZeroMemory(&srv, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_port = htons(9988);//pThis->m_iPort
	srv.sin_addr.S_un.S_addr=inet_addr(GRUPO);
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		Alert("socket");
		return 1;
	}

	int bOptVal=1;
	if (setsockopt(s, SOL_SOCKET,SO_REUSEADDR,(char*)&bOptVal,sizeof(bOptVal)) < 0) {
		Alert("Reusing ADDR failed");
		return 1;
	}
	if (bind(s, (SOCKADDR*)&srv, sizeof(srv)) < 0) {
		DWORD dwErr=WSAGetLastError();
		Alert("bind");
		return 1;
	}
	mreq.imr_multiaddr.S_un.S_addr=inet_addr(GRUPO);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(s,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mreq,sizeof(mreq)) < 0) {
		Alert("setsockopt");
		return 1;
	}
	n = sizeof(cli);
	while (1) {
		if ((r = recvfrom(s, buf, MAXBUF, 0, (SOCKADDR*)&cli, &n)) < 0) {
			Alert("recvfrom");
		} else {
			buf[r] = 0;
			Alert(Format("Message received from %s: %s", inet_ntoa(cli.sin_addr), buf));
		}
	}
	*/
	return 0;
}

BOOL CNetXPCProtocol::FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers)
{
	/*
	int s;
	struct sockaddr_in srv;
	char buf[MAXBUF];
	ZeroMemory(&srv, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_port = htons(PUERTO);
	srv.sin_addr.S_un.S_addr=inet_addr(GRUPO);
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		Alert("socket");
		return 1;
	}
	const char* buf2="test message";
	if (sendto(s, buf2, strlen(buf), 0, (SOCKADDR*)&srv, sizeof(srv)) < 0) {
		Alert("recvfrom");
	} else {
		Alert(Format( "Enviado a %s: %s\n", GRUPO, buf2));
	}
	return TRUE;
	*/
	return FALSE;
}

BOOL CNetXPCProtocol::Start()
{
	InitXmlRpcClient(objSettings.dwNetMessagingTimeout,5);
	CActionError error;
	iServerIndex=CreateServer(m_iPort,3,10,objSettings.dwNetMessagingTimeout,error);
	if(!error.allOk()){
		iServerIndex=-1;
		bStarted=0;
		return FALSE;
	}else{
		AddXmlRpcMethod(iServerIndex,SENDTEXT_ONLINE,XmlRpcReceiveMessage,error);
		AddXmlRpcMethod(iServerIndex,EXCHSTATUS_ONLN,XmlRpcExchangeStatus,error);
		if(!StartServer(iServerIndex,error) || !error.allOk()){
			objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l(Format("%s (XPC)",error.getErrorDsc())));
			StopServer(iServerIndex,error);
			iServerIndex=-1;
			bStarted=0;
			return FALSE;
		}
		// Поток, слушающий нотификации о поиске компьютеров
		// hUDPtatusThread=CreateThread( NULL, 0, UDPTranslater, (LPVOID)this, 0, &dwUDPThreadID);
	}
	bStarted=1;
	return TRUE;
}

CNetXPCProtocol::CNetXPCProtocol(int iPort)
{
	dwUDPThreadID=0;
	hUDPtatusThread=0;
	m_iPort=iPort;
}

CNetXPCProtocol::~CNetXPCProtocol()
{
	bStarted=0;
	if(iServerIndex>=0){
		CActionError error;
		StopServer(iServerIndex,error);
	}
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus<0){
		TerminateThread(hUDPtatusThread,99);
	}else if(hUDPtatusThread){
		TerminateThread(hUDPtatusThread,99);
		hUDPtatusThread=0;
	}
	DeinitXmlRpcClient();
}

BOOL CNetXPCProtocol::SendNet_Text(const char* szRecipient, CNetMessage_Text* str)
{
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return 0;
	}
	if(bStarted==0){
		Alert(_l(DEFAULT_NOPROTWARN),5000,FALSE,DT_CENTER|DT_VCENTER);
	}
	XmlRpcRequest Request;
	CString sRecipient=szRecipient;
	GetRecipientIP(sRecipient);
	if(sRecipient==""){
		return FALSE;
	}
	CString sOutURI=Format("http://%s:%i/RPC2",sRecipient,m_iPort);
	Request.szURI=sOutURI;
	Request.szMethodName=SENDTEXT_ONLINE;
	CString sFrom=objSettings.sLocalComputerName;
	CString sTo=szRecipient;
	CString sMessage=str->sMessageBody;
	ExtractSenderFromText(sFrom, sMessage);
	CDataXMLSaver::Str2Xml(sMessage);
	CString sAttach=str->sMessageAttach;
	Request.values<<sFrom<<sTo<<sMessage<<sAttach;
	// Добавляем параметры
	XmlRpcResult result;
	CActionError error;
	MakeXmlRpcCall(Request, result, objSettings.dwNetMessagingTimeout, error);
	//
	if(!error.allOk()){
		str->sError=_l(Format("%s (XPC)",error.getErrorDsc()));
		return FALSE;
	}
	if(!result.error.allOk()){
		str->sError=_l(Format("%s (XPC)",error.getErrorDsc()));
		return FALSE;
	}
	return TRUE;
}

BOOL CNetXPCProtocol::SendNet_StatusExch(const char* szRecipient,const char* szProtocol)
{
	if(bStarted==0){
		return FALSE;
	}
	XmlRpcRequest Request;
	CString sRecipient=szRecipient;
	GetRecipientIP(sRecipient);
	if(sRecipient==""){
		return FALSE;
	}
	CString sOutURI=Format("http://%s:%i/RPC2",sRecipient,m_iPort);
	Request.szURI=sOutURI;
	Request.szMethodName=EXCHSTATUS_ONLN;
	// Добавляем параметры
	Request.values<<CreateMyStatusStringID(objSettings.sLocalComputerName);
	XmlRpcResult result;
	CActionError error;
	MakeXmlRpcCall(Request, result, objSettings.dwNetMessagingTimeout, error);
	if(!error.allOk()){
		return -1;
	}
	if(!result.error.allOk()){
		return -1;
	}
	UpdateUserIconFromStatusStringID(result.values.GetAsString(0,error));
	return TRUE;
}


