#ifndef _SMALL_XMLRPC_CLIENTSERVER_H_
#define _SMALL_XMLRPC_CLIENTSERVER_H_
#include <afxtempl.h>
////////////////////////////////
// ���������...
////////////////////////////////
//#define USE_STRICT_CONTENTTYPE
#define SERVER_VERSION "SmallXmlRpc v3.0.0.1"
#define CLIENT_VERSION "SmallXmlRpc v3.0.0.0"
#define CRLF "\r\n";
#ifndef _DEBUG2_
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT 
#endif

////////////////////////////////
// ������
////////////////////////////////
struct AE_ErrorStruct{
	int code;
	const char* dsc;
};

/** ������. ���-��������
*	������ ���������:
*	� ���������� ������������� ������� ������� �������
*	����� ����� �������������� ���� ������
*	�� ���� �� ���������� ������ ������ �������������
*	� ���: 0 �������������� �� ���������� ������
*/
const AE_ErrorStruct AE_Ok								={0,"ok"};
const AE_ErrorStruct AE_ServerIndexInvalid				={-1,"Server index invalid"};
const AE_ErrorStruct AE_AsyncCallParametersNotValid		={-2,"AsyncCall parameters are not valid"};
const AE_ErrorStruct AE_AsyncCallTerminating			={-3,"AsyncCall terminated"};
const AE_ErrorStruct AE_UnsupportedValueType			={-5,"Unsupported value type"};
const AE_ErrorStruct AE_IndexOutOfBounds				={-6,"Index out-of-bounds"};
const AE_ErrorStruct AE_MethodAlreadyExist				={-7,"Method already exist"};
const AE_ErrorStruct AE_ServerAlreadyRunning			={-8,"Server already running"};
const AE_ErrorStruct AE_ServerAlreadySuspended			={-9,"Server already suspended"};
const AE_ErrorStruct AE_ServerNotCreated				={-10,"Server not created"};
const AE_ErrorStruct AE_WSACantGetInfoFromSocket		={-11,"Can`t get info from socket"};
const AE_ErrorStruct AE_ServerNotInitialized			={-12,"Server not initialized"};
const AE_ErrorStruct AE_ServerIsRunning					={-13,"Server is running"};
const AE_ErrorStruct AE_CantCreateThread				={-15,"Can`t create thread"};
const AE_ErrorStruct AE_WSAError						={-16,"WSAError"};
const AE_ErrorStruct AE_WSACantReadDataFromSocket		={-17,"Can`t read data from socket"};
const AE_ErrorStruct AE_InvalidRequestHeader			={-18,"Request: Invalid header"};
const AE_ErrorStruct AE_InvalidRequestContentType		={-19,"Request: Invalid content-type"};
const AE_ErrorStruct AE_CantConnectToSocket				={-20,"Can`t connect to socket"};
const AE_ErrorStruct AE_XmlRpcMethodNotExist			={-22,"XmlRpc method not defined"};
const AE_ErrorStruct AE_XmlRpcMethodReturnFalse			={-23,"XmlRpc method returned False"};
const AE_ErrorStruct AE_ErrorConvertingValuesToXml		={-24,"Failed to convert values to xml"};
const AE_ErrorStruct AE_ErrorConvertingXmlToValues		={-25,"Failed to convert xml to values"};
const AE_ErrorStruct AE_InvalidHTTPRequestURI			={-26,"Request: Invalid URI"};
const AE_ErrorStruct AE_TimeOutWhileReadingFromSocket	={-27,"Time-out while reading from socket"};
const AE_ErrorStruct AE_WSACantCreateSocket				={-28,"Can`t create socket"};
const AE_ErrorStruct AE_WSACantBindSocket				={-29,"Can`t bind socket"};
const AE_ErrorStruct AE_WSACantListenSocket				={-30,"Can`t listen socket"};

// ����� ����� ������� ������������ ��������� �� �������
class CActionError
{
protected:
	// ��� ������. 0 - ��� ������
	int m_iCode;
	// �������� ������
	CString m_errorDsc;
public:
	
	/** ����������� �� ���������
	*	@return	
	*/
	CActionError(){resetError();};
	
	/** ����������� �����������
	*	@param	CActionError& error
	*	@return	
	*/
	CActionError(CActionError& error){*this=error;};
	
	/** ����������� �� ���� � ��������
	*	@param	int iCode
	*	@param	const char* 
	*	@return	
	*/
	CActionError(int iCode, const char* szErrorDsc){setError(iCode, szErrorDsc);};
	
	/** ����������� �� ���� ������ - ��� ������ � ��� ���������
	*	@param	int iCode
	*	@param	int iCode2
	*	@return	
	*/
	CActionError(int iCode, int iCode2){setError(iCode, iCode2);};
	virtual ~CActionError(){};
	
	/** �������� ������������
	*	@param	CActionError& error
	*	@return	CActionError& 
	*/
	CActionError& operator=(CActionError& error){setError(error.getErrorCode(), error.getErrorDsc());return *this;};
	
	/** ���������� ������ � �������� �� ��������� AE_ErrorStruct - ��� � ��� � ��������
	*	@param	AE_ErrorStruct errDsc
	*	@return	void 
	*/
	void setError(AE_ErrorStruct errDsc){m_iCode=errDsc.code;m_errorDsc=errDsc.dsc;};
	
	/** ���������� ������ � �������� ��������
	*	@param	int iCode
	*	@param	const char* szErrorDsc=NULL
	*	@return	void 
	*/
	void setError(int iCode, const char* szErrorDsc=NULL){m_iCode=iCode;m_errorDsc=szErrorDsc;};
	
	/** ���������� ������ � �������� �������� � ���. �����
	*	@param	AE_ErrorStruct errDsc
	*	@param	int iCode2
	*	@return	void 
	*/
	void setError(AE_ErrorStruct errDsc, int iCode2){m_iCode=errDsc.code;m_errorDsc.Format("%s (0x%X)",errDsc.dsc,iCode2);};

	/** ���������� ������ � �������� �������� � ���. ���������
	*	@param	AE_ErrorStruct errDsc
	*	@param	const char* szAddDsc
	*	@return	void 
	*/
	void setError(AE_ErrorStruct errDsc, const char* szAddDsc){m_iCode=errDsc.code;m_errorDsc.Format("%s (%s)",errDsc.dsc,szAddDsc);};
	/** ���������� ������ � �������� �������� � ���. �����
	*	@param	int iCode
	*	@param	int iCode2
	*	@return	void 
	*/
	void setError(int iCode, int iCode2){m_iCode=iCode;char szErrorDsc[256];sprintf(szErrorDsc,"#%i",iCode2);m_errorDsc=szErrorDsc;};
	
	/** �������� ������
	*	@return	void 
	*/
	void resetError(){m_iCode=0;m_errorDsc="Action complited successfully";};
	
	/** ����� ��� ������
	*	@return	int 
	*/
	int getErrorCode(){return m_iCode;};
	
	/** ����� �������� ������
	*	@return	const char* 
	*/
	const char* getErrorDsc(){return m_errorDsc;};
	
	/** ���������, ���� �� ������
	*	@return	BOOL 
	*/
	BOOL allOk(){return m_iCode==0;};
};

/** ���� �������� � XmlRpc ��������-�������
*/
enum{
	XmlRpcLong=0,
	XmlRpcDouble=1,
	XmlRpcString=2
};

/** ����� - ��������� ��������
*/
class DLLEXPORT XmlRpcValue
{
public:
	/** ����������� �����������
	*	@param	XmlRpcValue* value
	*	@return	
	*/
	XmlRpcValue(XmlRpcValue* value);
	
	/** ����������� long-��������
	*	@param	long Data
	*	@return	
	*/
	XmlRpcValue(long Data);
	
	/** ����������� double-��������
	*	@param	double Data
	*	@return	
	*/
	XmlRpcValue(double Data);
	
	/** ����������� string-��������
	*	@param	const char* Data
	*	@return	
	*/
	XmlRpcValue(const char* Data);

	/** ����������
	*	@return	
	*/
	virtual ~XmlRpcValue();

	/** ���������������� �������
	*	@param	int iType
	*	@param	void* pData
	*	@param	int iDataLength=0
	*	@return	BOOL 
	*/
	BOOL InitializeValue(int iType, void* pData, int iDataLength=0);

	/** ��� ��������
	* 0-long
	* 1-double
	* 2-const char*
	*/
	int m_iType;

	/** ���� ������
	*/
	void* m_pData;
	
	/** ������ ����� ��������
	*	@return	XmlRpcValue* 
	*/
	XmlRpcValue* Copy();
};
//
typedef CArray<XmlRpcValue*,XmlRpcValue*> XmlRpcValuesArray;
//
class DLLEXPORT XmlRpcValues
{
private:
	/** ������ ��������� ��������
	*/
	XmlRpcValuesArray aValues;
public:
	/** ����������� �� ���������
	*/
	XmlRpcValues(){};
	
	/** ����������
	*/
	virtual ~XmlRpcValues();
	
	/** ������� ��� �������� �� ������
	*	@return	void 
	*/
	void RemoveAll();
	
	/** �������� ��������
	*	@param	XmlRpcValue* pValue
	*	@return	BOOL - ���������/�� ���������
	*/
	BOOL AddValue(XmlRpcValue* pValue);
	
	/** ����� ���������� ��������� � ������
	*	@return	int 
	*/
	int GetSize();
	
	/** ����� ������� � �������� ��������
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	XmlRpcValue* 
	*/
	XmlRpcValue* GetValue(int iIndex, CActionError& error);
	
	/** ����� ��� �������� � �������� ��������
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	const char* 
	*/
	const char* GetValueType(int iIndex, CActionError& error);
	
	/** ����� ������� � �������� �������� ���� long
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	long 
	*/
	long GetAsLong(int iIndex, CActionError& error);
	
	/** ����� ������� � �������� �������� ���� double
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	double 
	*/
	double GetAsDouble(int iIndex, CActionError& error);
	
	/** ����� ������� � �������� �������� ���� string
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	const char* 
	*/
	const char* GetAsString(int iIndex, CActionError& error);
	
	/** ����� ������� � �������� �������� � ������������� ��� � ��������� ��� (� ����� ���� - � ��������� �������������)
	*	@param	int iIndex
	*	@param	CString& sRes
	*	@param	CActionError& error
	*	@return	BOOL 
	*/
	BOOL ConvertValueToString(int iIndex, CString& sRes, CActionError& error);
	
	/** �������� ������������
	*	@param	XmlRpcValues&
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator=(XmlRpcValues&);
	
	/** �������� ��� ���������� ��������
	*	@param	XmlRpcValue*
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(XmlRpcValue*);
	
	/** �������� ��� ���������� �������� ���� string
	*	@param	const char* value
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(const char* value);
	
	/** �������� ��� ���������� �������� ���� long
	*	@param	long value
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(long value);
	
	/** �������� ��� ���������� �������� ���� double
	*	@param	double value
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(double value);
};

/** ����� ��� ����������� ����������
*/
class XmlRpcResult
{
public:

	/** ������, ��������� � ���������� ��������� �������
	*/
	CActionError error;

	/** ���������, ���������� � ����������� ��������� �������
	*/
	XmlRpcValues values;
};

/** ����� ��� ������ �������, ����������� XmlRpc - �����
*/
class XmlRpcRequest
{
public:
	
	/** ����������� �� ���������
	*	@return	
	*/
	XmlRpcRequest(){};
	
	/** ����������
	*	@return	virtual 
	*/
	virtual ~XmlRpcRequest(){};

	/** ����� �������, � �������� ���� ���������
	*/
	const char* szURI;

	/** �������� ����������� ������
	*/
	const char* szMethodName;

	/** ��������, ������������ ����������� ������
	*/
	XmlRpcValues values;
};

/** ���� Callback �������
*/
/** �� ��������� ������ �������� / ��������
*	��� �������: ���� ���������� TRUE, �� ������ ���������� �������� �� ����������
*	��� �������: iServerNum=-1
*	@param	int iServerNum - ����� �������, ����������� ������
*	@param	const char* szData - ��� ������� ������
*	@param	int iDataLength - ������� ������� ������
*	@return	BOOL - ���������. ��� ������� - ���� ������� TRUE, �� ������ �� ����� ������������ ������ �����
*/
typedef BOOL (CALLBACK *OnDataReceivedCallbackFunc)(int iServerNum, const char* szData, int iDataLength);

/** �� ������� ������ �������� / ��������
*	��� �������: ���� ���������� TRUE, �� ������ ���������� �������� �� ����������
*	��� �������: iServerNum=-1
*	@param	int iServerNum - ����� �������
*	@param	const char* szData - ��������� ������
*	@param	int iDataLength - ������� �������
*	@return	BOOL 
*/
typedef BOOL (CALLBACK *OnDataSendedCallbackFunc)(int iServerNum, const char* szData, int iDataLength);

/** �� ����� ������ (��� �������� ������ ���������� � ��������� ���������)
*	��� �������: ���� ���������� TRUE, �� ������ ���������� ��������� �� ����������
*	��� �������: iServerNum=-1
*	@param	int iServerNum - ����� �������
*	@param	SOCKET* sock - ����� (��� �������� �������� ������������ ��������� �� ������)
*	@param	const char* szData - ������, ���������� �������� � ����� �������
*	@param	CActionError& error - ������, � ���������� ������� ��������� ��� �������
*	@return	BOOL - ���� ������� TRUE, �� ���������� ��������� ������������� �� ����� (SOCKET ����� ������ ������)
*/
typedef BOOL (CALLBACK *OnErrorCallbackFunc)(int iServerNum, SOCKET* sock, const char* szData, CActionError& error);

/** ��� �������, ����������� XmlRpc �����. ���� ������ FALSE, �� ������� ����� ������� ������ �� lpResult->error
*	@param	int iServerNum - ����� �������, �������� ������ ������ �� ���������� ������
*	@param	XmlRpcRequest* lpRequest - ��������� �� ������ � ����������� �������
*	@param	XmlRpcResult* lpResult - ��������� �� ������, � ������� ���� ���������� ��������� ��� ������
*	@return	BOOL - ���� ������� FALSE �� ������� ����� ������� ������ �� lpResult
*/
typedef BOOL (CALLBACK *OnXmlRpcMethodCallbackFunc)(int iServerNum, XmlRpcRequest* lpRequest, XmlRpcResult* lpResult);

/** ��� �������, ������� ����� ���������� ��� ��������� ����������� ����������� �������
*	@param	XmlRpcRequestEx* lpRequest - ����������� ��������� � ��������� �������
*	@return	BOOL - ���������
*/
class XmlRpcRequestEx;
typedef BOOL (CALLBACK *OnXmlRpcOnMethodReturnedCallbackFunc)(XmlRpcRequestEx* lpRequest);

/** ����� ��� �������� ���. ���������� callback �������, ��������� � ��������
*	��� �������� ���. ������ ���� ��������������� �� XmlRpcRequestUserData � �����������
*	���������� (�� � ����� ������ ��� ���������� �������� ������ �������)
*/
class XmlRpcRequestUserData
{
public:
	
	/** ����������� �� ���������
	*	@return	
	*/
	XmlRpcRequestUserData(){};
	
	/** ����������. � ������������� ������� ��� ������� ����������� ����� �����������
	*	���������� ��� UserData(��. �����) �������
	*	@return	
	*/
	virtual ~XmlRpcRequestUserData(){};
};

/** ����� � ����������� ����������� � ������
*	��� ��������� ���������� callback �������� ��� ��������� ������� ��������� ��������
*/
class XmlRpcRequestEx: public XmlRpcRequest
{
public:
	
	/** ����������� �� ���������
	*	@return	
	*/
	XmlRpcRequestEx(){bStatus=0;dwTimeout=0;func=NULL;hThread=NULL;CallID=0;UserData=NULL;};
	
	/** ����������
	*	@return	
	*/
	~XmlRpcRequestEx(){delete (char*)szURI; delete (char*)szMethodName;if(UserData){delete UserData;};};

	/** ������ ��������� �������
	*	1 - ������ � ���������
	*	2 - ��������������
	*	3 - ����������
	*	0 - ���� ��� ������ - ���������
	*/
	BOOL bStatus;

	/** Callback - �������
	*/
	OnXmlRpcOnMethodReturnedCallbackFunc func;

	/** Handle ������ � ������� �������� �����
	*/
	HANDLE hThread;

	/** ID ������ � ����� �������, �������� ���������
	*/
	WORD CallID;

	/** ���� ��� �������� ������ (0 - ����� :)
	*/
	DWORD dwTimeout;

	/** ������, ��������� � �������� ��������� ������� (�������� � ������� ��������)
	*/
	CActionError error;

	/** ���������, ���������� �� ���������� ������� ������������ ������
	*/
	XmlRpcResult result;

	/** ���������������� ������. ��� �������� ������ ��� ������������ � ���������� callback ������� ����� ��������� ���������� �� ���������� �������
	*	��� ����������� ������������ �������� ���� ��������������� �� XmlRpcRequestUserData � ����������� ���������� (�� � ����� ������)
	*	� ���������� ��� ������ ��������� �� ����� �������
	*/
	XmlRpcRequestUserData* UserData;

};

/** ��������� ����� ��� ������������ ���������� �������
*	@return	void 
*/
void TestClientSide();

/** ��������� ����� ��� ������������ ��������� �����
*	@return	void 
*/
void TestServerSide();

///////////////////////////////////
// ��������� ������� � ��������� //
///////////////////////////////////
/** ����� ��������� �������
*/
class CXmlRpcServerInfo
{
public:

	/** ���������� �� ���������
	*	@return	
	*/
	CXmlRpcServerInfo(){m_iStatus=0;m_iPort=0;m_iSuspended=1;m_iBetweenReqTime=10;m_ListenTimeout=3000;};

	/** ����������
	*	@return	virtual 
	*/
	virtual ~CXmlRpcServerInfo(){};

	/** ����
	*/
	int m_iPort;

	/** ������������ ���������� �������� � �������
	*/
	int m_iMaxConnect;

	/** ������: 0 - �� �������, 1 - �������
	*/
	int m_iStatus;

	/** ������2: 0 - �������������, 1 - � ������
	*/
	int m_iSuspended;

	/** ������������ ������ �� ������� ����� �������� � ������� (�����)
	*/
	int m_iBetweenReqTime;

	/** ������������ ������� ��� ��������� ��������� ����� ��� ������� (�����)
	*/
	DWORD m_ListenTimeout;

	/** ������ �������������� ������� � ������. �� �������
	*/
	CMapStringToPtr MethodMap;

	/** �������� �����������
	*	@param	CXmlRpcServerInfo& obj
	*	@return	CXmlRpcServerInfo& 
	*/
	CXmlRpcServerInfo& operator=(CXmlRpcServerInfo& obj)
	{
		m_iPort=obj.m_iPort;
		m_iStatus=obj.m_iStatus;
		m_iSuspended=obj.m_iSuspended;
		m_iMaxConnect=obj.m_iMaxConnect;
		m_iBetweenReqTime=obj.m_iBetweenReqTime;
		m_ListenTimeout=obj.m_ListenTimeout;
		MethodMap.RemoveAll();
		POSITION p=obj.MethodMap.GetStartPosition();
		void* pServer=NULL;
		CString sKey;
		while(p!=NULL){
			obj.MethodMap.GetNextAssoc(p,sKey,pServer);
			MethodMap[sKey]=pServer;
		};
		return *this;
	}
};

/** ����� ���������� �������
*/
class CXmlRpcServerStatistics
{
public:
	
	/** ����������� �� ���������
	*	@return	
	*/
	CXmlRpcServerStatistics(){lRequestHandled_Total=lRequestHandled_Error=lTotalBytesReceived=0;};
	
	/** ����������
	*	@return	virtual 
	*/
	virtual ~CXmlRpcServerStatistics(){};
	
	/** �������� ������������ ������
	*	@param	int amount=1
	*	@return	void 
	*/
	void AddHandled_Total(int amount=1){InterlockedExchangeAdd(&lRequestHandled_Total,amount);};
	
	/** �������� ������������ �������� ������
	*	@param	int amount=1
	*	@return	void 
	*/
	void AddHandled_Error(int amount=1){InterlockedExchangeAdd(&lRequestHandled_Error,amount);};
	
	/** �������� ���������� ���������� ����
	*	@param	int amount=0
	*	@return	void 
	*/
	void AddReceivedBytes(int amount=0){InterlockedExchangeAdd(&lTotalBytesReceived,amount);};
	
	/** ��������� ���������� ���������� ����
	*	@param	int amount=0
	*	@return	void 
	*/
	void AddSendedBytes(int amount=0){InterlockedExchangeAdd(&lTotalBytesSended,amount);};

	/** ����������
	*/
	long lRequestHandled_Total;

	/** ���������� � �������
	*/
	long lRequestHandled_Error;

	/** ���� ��������
	*/
	long lTotalBytesReceived;

	/** ���� ��������
	*/
	long lTotalBytesSended;
};

/** ������� (�� �� ��������� ������) ��������� ������ ����
*	@param	int - ����� �����
*	@param	int - ������� ���������� ����� ������� ����� ������� (������ ����� ����������)
*	@param	int - ������� ����� ����� ���������� "���� �� ����� ������ � �������" (�����)
*	@param	DWORD - �� ����� ��������� ������� ������� ����������� ����� ����� ������� ��������� (�����)
*	@param	CActionError& - ������ (���� ��������)
*	@return	int - ������ ���������� ������� (����� ��������� ��������� �� ������ �����), -1 � ������ ������
*/
int CreateServer(int iPort, int iBacklog, int iSleepBetweenReq, DWORD dwGetReqTimeout, CActionError& error);

/** ��������� ������
*	@param	int - ����� �������
*	@param	CActionError& - �������� ������ ���� ����
*	@return	BOOL - ��������� �������� 
*/
BOOL StartServer(int iServerNum, CActionError& error);

/** ���������������� ������
*	@param	int - ����� �������
*	@param	CActionError& - �������� ������ ���� ����
*	@return	BOOL - ��������� ��������
*/
BOOL PauseServer(int iServerNum, CActionError& error);

/** ���������� ������
*	@param	int - ����� �������, ���� -1 �� ���
*	@param	CActionError& - ��������� ��������
*	@return	BOOL - ��������� (TRUE/FALSE)
*/
BOOL StopServer(int iServerNum, CActionError& error);

/** ���������� ���������� � �������
*	@param	int - ����� �������
*	@param	CXmlRpcServerInfo& - ����������, ������� ����� �������� ���������� � �������
*	@param	CActionError& - ��������� ��������
*	@return	BOOL - ��������� (TRUE/FALSE)
*/
BOOL GetServerInfo(int iServerNum, CXmlRpcServerInfo& ServerInfo);

/** ���������� ���������� �������
*	@param	int - ����� �������
*	@param	CXmlRpcServerStatistics& - ����������, ������� ����� �������� ���������� �������
*	@return	BOOL - ��������� (TRUE/FALSE)
*/
BOOL GetServerStatistics(int iServerNum, CXmlRpcServerStatistics& ServerStatistics);

/** ������������� ������� ��������� ������ ��� ������� ��������/�������� ������
*	@param	int - ����� ������� ��� �������� ���������� ������� ��� -1 ��� �������
*	@param	OnDataSendedCallbackFunc lpFunction
*	@return	BOOL 
*/
BOOL OnXmlRpcServerDataSended(int iServerNum, OnDataSendedCallbackFunc lpFunction);

/** ������������� ������� ��������� ������ ��� ��������� ��������/�������� ������
*	@param	int - ����� ������� ��� �������� ���������� ������� ��� -1 ��� �������
*	@param	OnDataReceivedCallbackFunction - ������� ��������� ������
*	@return	BOOL - ��������� (TRUE - ������������, FALSE - ���)
*/
BOOL OnXmlRpcServerDataReceived(int iServerNum, OnDataReceivedCallbackFunc lpFunction);

/** ������������� ������� ��������� ������ ��� ����� ���� ������ ��������� �� ����� ������ �������/�������
*	@param	int - ����� ������� ��� �������� ���������� ������� ��� -1 ��� �������
*	@param	OnErrorCallbackFunc lpFunction
*	@return	BOOL 
*/
BOOL OnXmlRpcServerError(int iServerNum, OnErrorCallbackFunc lpFunction);

/** ��������� XmlRpc �����
*	@param	int - ����� �������, ������� ������ ������������ �����
*	@param	const char* - ��� ������
*	@param	OnXmlRpcMethodCallbackFunc - �������, ����������� �����
*	@param	CActionError& - ������ (���� ���������)
*	@return	BOOL - ��������� (TRUE/FALSE)
*/
BOOL AddXmlRpcMethod(int iServerNum, const char* szMethodName, OnXmlRpcMethodCallbackFunc lpFunction, CActionError& error);

/** ������������� ���������� �����
*	@param	long lTimeoutToHardTermination - ������� ����� ���� ���������� ����� ����� ���� �� �������� ���� ������
*	@param	long lReqBetweenCleanUp - ���������� ������� ����� �������� ������ ���������� ��������
*	@return	BOOL 
*/
BOOL InitXmlRpcClient(long lTimeoutToHardTermination, long lReqBetweenCleanUp);

/** ��������������� ���������� �����
*	@return	BOOL - ���������
*/
BOOL DeinitXmlRpcClient();

/** ������� XmlRpc �����
*	@param	XmlRpcRequest& - ��������� ������
*	@param	XmlRpcResult& - ��������� (���� ���� ������, ��� ����� ����������� ������)
*	@param	DWORD - ������� ����� ������ � ������������� (0- �����)
*	@param	CActionError& - ������ ��� ������� (���� ���������)
*	@return	BOOL - ���������
*/
BOOL MakeXmlRpcCall(XmlRpcRequest& Request, XmlRpcResult& result, DWORD dwMilliseconds, CActionError& error);

/** ������� XmlRpc ����� � ��������� �� ��������� ����������
*	����� �� ������ ������ ����� ������� �����, ����� ������� ����. ���. ������
*	@param	XmlRpcRequest& - ��������� ������
*	@param	DWORD - ������� ����� ������ � ������������� (0- �����)
*	@param	OnXmlRpcOnMethodReturnedCallbackFunc - ������� ���. ������
*	@param	XmlRpcRequestUserData* - ��������� �� ���������������� ������ (������� �� ����� XmlRpc ������!)
*	@return	BOOL - ���������
*/
BOOL MakeCallbackXmlRpcCall(XmlRpcRequest& Request, DWORD dwMilliseconds, OnXmlRpcOnMethodReturnedCallbackFunc func, XmlRpcRequestUserData* UserData);

/** ���������� ��� ����������� ������
*	@return	BOOL 
*/
BOOL TerminateAllCalls();

/** �������� ���������� ������� (������� �������, ������� �����, ������� ��������� � �������)
*	@param	long& - ������� ����� ������� �������
*	@param	long& - �� ��� ���������
*	@param	long& - ������� ��������� � ��������
*	@param	long& - ������� �������� ��������, �� ������ �� ��� ��� �� �������
*	@param	long& - ������� �������
*	@param	long& - ������� ��������
*	@return	BOOL 
*/
BOOL GetClientStatistics(long& lTotal, long& lErrors, long& lInProcess, long& lFinishedButNotDeleted, long& lBytesSend, long& lBytesRec);

#endif//_SMALL_XMLRPC_CLIENTSERVER_H_