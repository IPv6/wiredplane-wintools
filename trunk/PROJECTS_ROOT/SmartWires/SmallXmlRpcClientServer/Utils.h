// Utils.h: interface for the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILS_H__D3098D2E_DC34_450C_8EDE_1E4DDA3E6A60__INCLUDED_)
#define AFX_UTILS_H__D3098D2E_DC34_450C_8EDE_1E4DDA3E6A60__INCLUDED_
#include <winsock2.h>
BOOL GetRawDataFromSocket(SOCKET* sock, CString& sBytes, long& lAmountOfBytes, DWORD dwTimeout, CActionError& error);
BOOL SendTextToSocket(SOCKET* sock, CString sBytes, long* lAmountOfBytes, CActionError& error);
CString GetInstringPart(const char* szStart, const char* szString);
CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString);
CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString, int& iFrom, BOOL bRecursive=FALSE);
CString LowerXMLTags(const char* szText);
BOOL GetHTTPHeaderFieldValue(const char* szHTTPHeader, const char* szField, CString& sValue);
BOOL GetHTTPHeaderFieldValue(const char* szHTTPHeader, const char* szField, long& lValue); 
BOOL GetHTTPBodyValue(const char* szHTTPReq, CString& sValue);
BOOL CreateXmlRpcResultFromXml(const char* szBody,XmlRpcResult& result, CActionError& error);
BOOL CreateXmlFromRpcMethodResult(XmlRpcResult& results, CString& sXml, CActionError& error);
BOOL GetHTTPRequestURI(const char* sRequestData, CString& sURI);
CString CreateReply(const char* szText);
CString CreateReply(CActionError& error);
CString CreateRequest(const char* szHost, const char* szQuery, const char* szText);
BOOL LoadHTTPRequestFromSocket(SOCKET* sock, CString& sRequestData, CString& sReqBody, DWORD& dwTotalLoaded, DWORD dwTimeout, CActionError& error);
BOOL CreateXmlFromRpcMethodCallRequest(const char* szFunction, XmlRpcValues* parameters, CString& sXml, CActionError& error);
void GetHostAndQueryFromURI(const char* szURI, CString& sHost,CString& sQuery);
BOOL CreateXmlRpcValuesFromXml(const char* szBody,XmlRpcValues& MethodParams, CActionError& error);
CString TimeStamp();
CString GetLastWSAError();
#endif // !defined(AFX_UTILS_H__D3098D2E_DC34_450C_8EDE_1E4DDA3E6A60__INCLUDED_)
