// Utils.cpp: implementation of the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmallXmlRpcClientServer.h"
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CString TimeStamp()
{
	return COleDateTime::GetCurrentTime().Format("%a %d %b %Y %H:%M:%S %Z");
}

BOOL GetRawDataFromSocket(SOCKET* sock, CString& sBytes, long& lAmountOfBytes, DWORD dwTimeout, CActionError& error)
{
	sBytes="";
	DWORD dwStartTime=GetTickCount();
	int iReaded=-1,iReadedCountTotal=0,iPositionInBuffer=0,iRes;
	DWORD dwBytesWaitingFor=0;
	HGLOBAL hFullBuffer;
	// Заголовок не будет больше 1024 байт??? врядли!!!
	DWORD dwBufferSize=lAmountOfBytes>0?(lAmountOfBytes):1024;
	hFullBuffer = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, dwBufferSize+1);//+1 на всякий случай
	char* szBuffer=(char*)::GlobalLock(hFullBuffer);
	BOOL bRes=TRUE, bUnreadedBytePresent=FALSE;
	do{
		if(dwTimeout!=0 && (GetTickCount()-dwStartTime)>dwTimeout){
			// тайм аут :(
			error.setError(AE_TimeOutWhileReadingFromSocket);
			bRes=FALSE;
			break;
		}
		bUnreadedBytePresent=FALSE;
		iRes=ioctlsocket(*sock,FIONREAD,&dwBytesWaitingFor);
		if(iRes==SOCKET_ERROR){
			error.setError(AE_WSAError,GetLastWSAError());
			bRes=FALSE;
			break;
		}
		if(dwBytesWaitingFor==0){
			Sleep(10);
			continue;
		}
		if(iPositionInBuffer+dwBytesWaitingFor>=dwBufferSize){
			// Буфера не хватило, сбрасываем данные
			szBuffer[iPositionInBuffer]=0;
			sBytes+=szBuffer;
			iPositionInBuffer=0;
			dwBytesWaitingFor-=dwBufferSize;
			bUnreadedBytePresent=TRUE;
		}
		iReaded=recv(*sock,szBuffer+iPositionInBuffer,dwBufferSize-iPositionInBuffer,0);
		if(iReaded==SOCKET_ERROR){
			error.setError(AE_WSACantReadDataFromSocket,GetLastWSAError());
			bRes=FALSE;
			break;
		}
		iPositionInBuffer+=iReaded;
		iReadedCountTotal+=iReaded;
	}while(((lAmountOfBytes>0) && (iReadedCountTotal<lAmountOfBytes && iReaded!=0)) || bUnreadedBytePresent==TRUE);
	lAmountOfBytes=iReadedCountTotal;
	// Добавляем прочитанное
	szBuffer[iPositionInBuffer]=0;
	sBytes+=szBuffer;
	::GlobalUnlock(hFullBuffer);
	::GlobalFree(hFullBuffer);
	return bRes;
}

BOOL SendTextToSocket(SOCKET* sock, CString sBytes, long* lAmountOfBytes, CActionError& error)
{
	long lHowMuchToRead;
	if(lAmountOfBytes==NULL || (*lAmountOfBytes)<=0){
		lHowMuchToRead=strlen(sBytes);
	}else{
		lHowMuchToRead=*lAmountOfBytes;
	}
	int nRes=send(*sock,sBytes,lHowMuchToRead,0);
	if(nRes==SOCKET_ERROR){
		error.setError(AE_WSAError,GetLastWSAError());
		return FALSE;
	};
	if(lAmountOfBytes!=NULL){
		*lAmountOfBytes=nRes;
	}
	return TRUE;
}

CString GetInstringPart(const char* szStart, const char* szString)
{
	int i=0;
	return GetInstringPart(szStart, NULL, szString,i);
}

CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString)
{
	int i=0;
	return GetInstringPart(szStart, szEnd, szString,i);
}

CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString, int& iFrom, BOOL bRecursive)
{
	CString sRes="";
	const char* szSearchFrom=szString+iFrom;
	char* szFrom=strstr(szSearchFrom,szStart);
	if(szFrom){
		sRes=(const char*)(szFrom+strlen(szStart));
		iFrom=(szFrom-szString)+strlen(szStart);
		const char* szSearchFromAfterStart=szString+iFrom;
	}else{
		iFrom=-1;
		return "";
	}
	if(szEnd!=NULL){
		int iEnd=sRes.Find(szEnd);
		if(iEnd!=-1){
			// Смотрим вложенности
			int iRecuFind=sRes.Find(szStart);
			if(bRecursive && iRecuFind!=-1 && iRecuFind<iEnd){
				//int iStCount=sRes.Replace(szStart,szStart);
				int iStCount=0,iStOffset=0;
				while((iStOffset=sRes.Find(szStart,iStOffset+strlen(szStart)))!=-1){
					iStCount++;
				}
				while(iStCount>0){
					iEnd=sRes.Find(szEnd,iEnd+strlen(szEnd));
					iStCount--;
				}
			}
			sRes=sRes.Left(iEnd);
			iFrom+=strlen(sRes)+strlen(szEnd);
			const char* szSearchFromEnd=szString+iFrom;
		}else{
			sRes="";
		}
	}else{
		sRes.TrimRight();
		iFrom+=strlen(sRes);
	}
	return sRes;
}

BOOL GetHTTPHeaderFieldValue(const char* szHTTPHeader, const char* szField, long& lValue)
{
	CString sValue;
	if(GetHTTPHeaderFieldValue(szHTTPHeader, szField, sValue)==TRUE){
		lValue=atol(sValue);
		return TRUE;
	}
	return FALSE;
}

BOOL GetHTTPHeaderFieldValue(const char* szHTTPHeader, const char* szField, CString& sValue)
{
	CString sHeader=szHTTPHeader;
	CString sFieldName=szField;
	sHeader.MakeLower();
	sFieldName.MakeLower();
	int iPos=sHeader.Find(sFieldName);
	if(iPos==-1){
		return FALSE;
	}
	iPos+=sFieldName.GetLength();
	char const* szValuePos=(const char*)szHTTPHeader+iPos;
	while((*szValuePos==' ' || *szValuePos==':')
		&& (*szValuePos!='\n') && (*szValuePos!='\r') && (*szValuePos!=0)){
		szValuePos++;
	}
	sValue=szValuePos;
	const char* szNewLine=CRLF;
	int iEndPos=strcspn(szValuePos, szNewLine);
	sValue=sValue.Left(iEndPos);
	return TRUE;
}

BOOL GetHTTPBodyValue(const char* szHTTPReq, CString& sValue)
{
	CString sBodyDivider=CRLF CRLF;
	char const* szHeadStart=(const char*)szHTTPReq;
	while(*szHeadStart=='\n'||*szHeadStart=='\r'){
		szHeadStart++;
	}
	char const* szReqStart=strstr(szHeadStart,sBodyDivider+sBodyDivider);
	if(szReqStart==NULL){
		return FALSE;
	}
	while(*szReqStart=='\n'||*szReqStart=='\r'){
		szReqStart++;
	}
	sValue=szReqStart;
	return TRUE;
}

BOOL GetHTTPRequestURI(const char* sRequestData, CString& sURI)
{
	CString sValue;
	GetHTTPHeaderFieldValue(sRequestData,"post",sValue);
	if(sValue.GetLength()==0){
		GetHTTPHeaderFieldValue(sRequestData,"get",sValue);
	}
	CString sValueLowered=sValue;
	sValueLowered.MakeLower();
	int iHttpVer=sValueLowered.Find(" http");// Пробел в начале - не опечатка!
	if(iHttpVer>0){
		sValue=sValue.Left(iHttpVer);
	}
	CString sHost;
	GetHTTPHeaderFieldValue(sRequestData,"Host",sHost);
	sURI=sHost+sValue;
	return TRUE;
}

BOOL CreateXmlFromRpcMethodResult(XmlRpcResult& results, CString& sXml, CActionError& error)
{
	sXml="<?xml version=\"1.0\"?>"CRLF;
	if(results.error.allOk()){
		// Генерим нормальный ответ
		sXml+="<methodResponse>"CRLF;
		sXml+="<params>"CRLF;
		for(int i=0;i<results.values.GetSize();i++){
			CString sType=results.values.GetValueType(i,results.error);
			sXml+="<param><value><";
			sXml+=sType+">";
			CString sValue;
			results.values.ConvertValueToString(i,sValue,results.error);
			sXml+=sValue;
			sXml+="</";
			sXml+=sType+"></value></param>"CRLF;
		}
		sXml+="</params>"CRLF;
		sXml+="</methodResponse>";
	}else{
		// Генерим ошибку
		sXml+="<methodResponse>"CRLF;
		sXml+="<fault>"CRLF;
		sXml+="<value><struct>"CRLF;
		sXml+="<member><name>faultCode</name><value><int>";
		CString sCode;
		sCode.Format("%i",results.error.getErrorCode());
		sXml+=sCode;
		sXml+="</int></value></member>"CRLF;
		sXml+="<member><name>faultString</name><value><string>";
		sXml+=results.error.getErrorDsc();
		sXml+="</string></value></member>"CRLF;
		sXml+="</struct></value>"CRLF;
		sXml+="</fault>"CRLF;
		sXml+="</methodResponse>";
	}
	return TRUE;
}

BOOL CreateXmlFromRpcMethodCallRequest(const char* szFunction, XmlRpcValues* parameters, CString& sXml, CActionError& error)
{
	if(parameters==NULL){
		return FALSE;
	}
	sXml+="<methodCall>"CRLF;
	sXml+="<methodName>";
	sXml+=szFunction;
	sXml+="</methodName>"CRLF;
	sXml+="<params>"CRLF;
	for(int i=0;i<parameters->GetSize();i++){
		CString sType=parameters->GetValueType(i,error);
		sXml+="<param><value><";
		sXml+=sType+">";
		CString sValue;
		parameters->ConvertValueToString(i,sValue,error);
		sXml+=sValue;
		sXml+="</";
		sXml+=sType+"></value></param>"CRLF;
	}
	sXml+="</params>"CRLF;
	sXml+="</methodCall>";
	return TRUE;
}

CString LowerXMLTags(const char* szText)
{
	CString sRes=szText;
	char* szPos=sRes.GetBuffer(0);
	int iInTag=FALSE;
	while(*szPos){
		if(*szPos=='<'){
			iInTag=TRUE;
		}else if(*szPos=='>'){
			iInTag=FALSE;
		}if(iInTag==TRUE && (*szPos>='A' && *szPos<='Z')){
			*szPos+=('a'-'A');
		}
		szPos++;
	}
	sRes.ReleaseBuffer();
	return sRes;
}

BOOL CreateXmlRpcValuesFromXml(const char* szBody,XmlRpcValues& MethodParams, CActionError& error)
{
	int iPos=0;
	CString sValueData;
	CString sLoweredXMLTags=LowerXMLTags(szBody);
	sValueData=GetInstringPart("<value>","</value>",sLoweredXMLTags,iPos,TRUE);
	while(iPos!=-1){
		CString sValueType=GetInstringPart("<",">",sValueData);
		CString sStartTag="<";
		sStartTag+=sValueType+">";
		CString sCloseTag="</";
		sCloseTag+=sValueType+">";
		int tmp=0;
		sValueData=GetInstringPart(sStartTag,sCloseTag,sValueData,tmp,TRUE);
		if(sValueType=="struct" || sValueType=="array"){
			// Добавляем
			RET_IF_FAULT(CreateXmlRpcValuesFromXml(sValueData,MethodParams,error));
		}
		sValueType.MakeLower();
		if(sValueType=="string"){
			MethodParams.AddValue(new XmlRpcValue(sValueData));
		}else if(sValueType=="i4" || sValueType=="int" ){
			MethodParams.AddValue(new XmlRpcValue(atol(sValueData)));
		}else if(sValueType=="double"){
			MethodParams.AddValue(new XmlRpcValue(atof(sValueData)));
		}
		// следующее значение...
		sValueData=GetInstringPart("<value>","</value>",sLoweredXMLTags,iPos, TRUE);
	}
	return TRUE;
}

BOOL CreateXmlRpcResultFromXml(const char* szBody,XmlRpcResult& result, CActionError& error)
{	
	CString sValueData;
	XmlRpcValues MethodParams;
	CreateXmlRpcValuesFromXml(szBody,MethodParams,error);
	if(strstr(szBody,"fault")!=0){
		result.error.setError(MethodParams.GetAsLong(0,error),MethodParams.GetAsString(1,error));
	}else{
		result.values=MethodParams;
	}
	return TRUE;
}

void GetHostAndQueryFromURI(const char* szURI, CString& sHost,CString& sQuery)
{
	CString sLine=szURI;
	int iHttpEnd=sLine.Find("//");
	if(iHttpEnd!=-1){
		sHost=sLine.Mid(iHttpEnd+2);
	}else{
		sHost=sLine;
	}
	int iQuery=sHost.Find("/");
	if(iQuery!=-1){
		sQuery=sHost.Mid(iQuery);
		sHost=sHost.Left(iQuery);
	}else{
		sQuery="";
	}
}

CString CreateRequest(const char* szHost, const char* szQuery, const char* szText)
{
	CString sRes;
	sRes+="POST ";
	sRes+=szQuery;
	sRes+=" HTTP/1.1"CRLF;
	sRes+="Host: ";
	sRes+=szHost;
	sRes+=CRLF;
	sRes+="Content-Type: text/xml"CRLF;
	CString sContLen;
	sContLen.Format("%i",szText==NULL?0:strlen(szText));
	sRes+="Content-Length: "+sContLen+CRLF;
	sRes+="User-Agent: WPLabs-SimpleXmlRpc v";
	sRes+=CLIENT_VERSION;
	sRes+=", WinNT"CRLF;
	sRes+="Date: ";
	sRes+=TimeStamp();
	sRes+=CRLF;
	sRes+=CRLF;
	sRes+=szText;
	return sRes;
}

CString CreateReply(const char* szText)
{
	CString sRes;
	sRes+="HTTP/1.1 200 OK"CRLF;
	sRes+="Connection: close"CRLF;
	sRes+="Content-Type: text/xml"CRLF;
	CString sContLen;
	sContLen.Format("%i",szText==NULL?0:strlen(szText));
	sRes+="Content-Length: "+sContLen+CRLF;
	sRes+="Server: ";
	sRes+=SERVER_VERSION;
	sRes+=", WinNT"CRLF;
	sRes+="Date: ";
	sRes+=COleDateTime::GetCurrentTime().Format("%a %d %b %Y %H:%M:%S %Z");
	sRes+=CRLF;
	sRes+=CRLF;
	sRes+=szText;
	return sRes;
}

CString CreateReply(CActionError& error)
{
	XmlRpcResult Tmp;
	Tmp.error=error;
	CString sXml;
	CreateXmlFromRpcMethodResult(Tmp, sXml, error);
	return CreateReply(sXml);
}

BOOL LoadHTTPRequestFromSocket(SOCKET* sock, CString& sRequestData, CString& sReqBody, DWORD& dwTotalLoaded, DWORD dwTimeout, CActionError& error)
{
	dwTotalLoaded=0;
	sRequestData="";
	CString sBytes="";
	DWORD dwLastingEmptyReplay=GetTickCount();
	long lReaded=0,lLeftToRead=0,lBodyLen;
	const char* szEndReqSeq="\r\n\r\n";
	// Читаем данные пока не наткнемся на конец запроса
	while(1){
		if((GetTickCount()-dwLastingEmptyReplay)>=dwTimeout){
			error.setError(AE_TimeOutWhileReadingFromSocket);
			return FALSE;
		}
		// Читаем заголовок...
		lReaded=0;
		sBytes="";
		RET_IF_FAULT(GetRawDataFromSocket(sock,sBytes,lReaded,dwTimeout,error));
		if(lReaded!=0){
			// Если хоть что-то считали, сбрасываем счетчик пустых ответов
			dwLastingEmptyReplay=GetTickCount();
		}
		sRequestData+=sBytes;
		dwTotalLoaded+=lReaded;
		if(GetHTTPHeaderFieldValue(sRequestData, "Content-Length", lBodyLen)==TRUE){
			// Теперь можно узнать сколько осталось дочитать
			if(GetHTTPBodyValue(sRequestData, sReqBody)==TRUE){
				lLeftToRead=lBodyLen-sReqBody.GetLength();
				break;
			}
		}
	}
	sBytes="";
	RET_IF_FAULT(GetRawDataFromSocket(sock,sBytes,lLeftToRead,dwTimeout,error));
	sRequestData+=sBytes;
	sReqBody+=sBytes;
	dwTotalLoaded+=lLeftToRead;
	return TRUE;
}

CCriticalSection& GetErrorLock()
{
	static CCriticalSection cErrorLock;
	return cErrorLock;
}

CString GetLastWSAError()
{
	CSingleLock ErrSection(&GetErrorLock());
	ErrSection.Lock(3000);
	CString sOut="WSALastError is locked";
	if(ErrSection.IsLocked()){
		char szBuffer[1024]="";
		int iErr=WSAGetLastError();
		WSASetLastError(0);
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,iErr,0,szBuffer,sizeof(szBuffer),NULL);
		CString sFlattenBuf=szBuffer;
		sFlattenBuf.Replace("\r","");
		sFlattenBuf.Replace("\n","");
		sFlattenBuf.Replace("\t","");
		sFlattenBuf.TrimLeft();
		sFlattenBuf.TrimRight();
		sOut.Format("0x%X %s",iErr,sFlattenBuf);
	}
	return sOut;
}
