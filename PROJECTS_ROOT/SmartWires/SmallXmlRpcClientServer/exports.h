#ifndef _SMALL_XMLRPC_CLIENTSERVER_H_
#define _SMALL_XMLRPC_CLIENTSERVER_H_
#include <afxtempl.h>
////////////////////////////////
// Параметры...
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
// Ошибки
////////////////////////////////
struct AE_ErrorStruct{
	int code;
	const char* dsc;
};

/** Ошибки. код-описание
*	Важное замечание:
*	в вызывающих нижеописанные функции внешних модулях
*	также можно организовывать свои ошибки
*	но коды им необходимо давать только положительные
*	и еще: 0 зарезервирован за отсутсвием ошибки
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

// Класс через который возвращаются сообщения об ошибках
class CActionError
{
protected:
	// Код ошибки. 0 - нет ошибки
	int m_iCode;
	// Описание ошибки
	CString m_errorDsc;
public:
	
	/** Конструктор по умолчанию
	*	@return	
	*/
	CActionError(){resetError();};
	
	/** Конструктор копирования
	*	@param	CActionError& error
	*	@return	
	*/
	CActionError(CActionError& error){*this=error;};
	
	/** Конструктор по коду и описанию
	*	@param	int iCode
	*	@param	const char* 
	*	@return	
	*/
	CActionError(int iCode, const char* szErrorDsc){setError(iCode, szErrorDsc);};
	
	/** Конструктор по двум числам - код ошибки и код подошибки
	*	@param	int iCode
	*	@param	int iCode2
	*	@return	
	*/
	CActionError(int iCode, int iCode2){setError(iCode, iCode2);};
	virtual ~CActionError(){};
	
	/** оператор присваивания
	*	@param	CActionError& error
	*	@return	CActionError& 
	*/
	CActionError& operator=(CActionError& error){setError(error.getErrorCode(), error.getErrorDsc());return *this;};
	
	/** Установить ошибку в значения из структуры AE_ErrorStruct - там и код и описание
	*	@param	AE_ErrorStruct errDsc
	*	@return	void 
	*/
	void setError(AE_ErrorStruct errDsc){m_iCode=errDsc.code;m_errorDsc=errDsc.dsc;};
	
	/** Установить ошибку в заданные значения
	*	@param	int iCode
	*	@param	const char* szErrorDsc=NULL
	*	@return	void 
	*/
	void setError(int iCode, const char* szErrorDsc=NULL){m_iCode=iCode;m_errorDsc=szErrorDsc;};
	
	/** Установить ошибку в заданные значения с доп. кодом
	*	@param	AE_ErrorStruct errDsc
	*	@param	int iCode2
	*	@return	void 
	*/
	void setError(AE_ErrorStruct errDsc, int iCode2){m_iCode=errDsc.code;m_errorDsc.Format("%s (0x%X)",errDsc.dsc,iCode2);};

	/** Установить ошибку в заданные значения с доп. описанием
	*	@param	AE_ErrorStruct errDsc
	*	@param	const char* szAddDsc
	*	@return	void 
	*/
	void setError(AE_ErrorStruct errDsc, const char* szAddDsc){m_iCode=errDsc.code;m_errorDsc.Format("%s (%s)",errDsc.dsc,szAddDsc);};
	/** Установить ошибку в заданные значения с доп. кодом
	*	@param	int iCode
	*	@param	int iCode2
	*	@return	void 
	*/
	void setError(int iCode, int iCode2){m_iCode=iCode;char szErrorDsc[256];sprintf(szErrorDsc,"#%i",iCode2);m_errorDsc=szErrorDsc;};
	
	/** Сбросить ошибку
	*	@return	void 
	*/
	void resetError(){m_iCode=0;m_errorDsc="Action complited successfully";};
	
	/** Взять код ошибки
	*	@return	int 
	*/
	int getErrorCode(){return m_iCode;};
	
	/** Взять описание ошибки
	*	@return	const char* 
	*/
	const char* getErrorDsc(){return m_errorDsc;};
	
	/** Проверить, была ли ошибка
	*	@return	BOOL 
	*/
	BOOL allOk(){return m_iCode==0;};
};

/** Типы значений в XmlRpc запросах-ответах
*/
enum{
	XmlRpcLong=0,
	XmlRpcDouble=1,
	XmlRpcString=2
};

/** Класс - одиночное значение
*/
class DLLEXPORT XmlRpcValue
{
public:
	/** Конструктор копирования
	*	@param	XmlRpcValue* value
	*	@return	
	*/
	XmlRpcValue(XmlRpcValue* value);
	
	/** Конструктор long-значения
	*	@param	long Data
	*	@return	
	*/
	XmlRpcValue(long Data);
	
	/** Конструктор double-значения
	*	@param	double Data
	*	@return	
	*/
	XmlRpcValue(double Data);
	
	/** Конструктор string-значения
	*	@param	const char* Data
	*	@return	
	*/
	XmlRpcValue(const char* Data);

	/** Деструктор
	*	@return	
	*/
	virtual ~XmlRpcValue();

	/** Инициализировать элемент
	*	@param	int iType
	*	@param	void* pData
	*	@param	int iDataLength=0
	*	@return	BOOL 
	*/
	BOOL InitializeValue(int iType, void* pData, int iDataLength=0);

	/** тип значения
	* 0-long
	* 1-double
	* 2-const char*
	*/
	int m_iType;

	/** Само данное
	*/
	void* m_pData;
	
	/** Делает копию элемента
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
	/** Массив отдельных значений
	*/
	XmlRpcValuesArray aValues;
public:
	/** Конструктор по умолчанию
	*/
	XmlRpcValues(){};
	
	/** Деструктор
	*/
	virtual ~XmlRpcValues();
	
	/** Удалить все значения из набора
	*	@return	void 
	*/
	void RemoveAll();
	
	/** Добавить значение
	*	@param	XmlRpcValue* pValue
	*	@return	BOOL - добавлено/не добавлено
	*/
	BOOL AddValue(XmlRpcValue* pValue);
	
	/** Взять количество элементов в наборе
	*	@return	int 
	*/
	int GetSize();
	
	/** Взять элемент с заданным индексом
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	XmlRpcValue* 
	*/
	XmlRpcValue* GetValue(int iIndex, CActionError& error);
	
	/** Взять тип элемента с заданным индексом
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	const char* 
	*/
	const char* GetValueType(int iIndex, CActionError& error);
	
	/** Взять элемент с заданным индексом типа long
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	long 
	*/
	long GetAsLong(int iIndex, CActionError& error);
	
	/** Взять элемент с заданным индексом типа double
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	double 
	*/
	double GetAsDouble(int iIndex, CActionError& error);
	
	/** Взять элемент с заданным индексом типа string
	*	@param	int iIndex
	*	@param	CActionError& error
	*	@return	const char* 
	*/
	const char* GetAsString(int iIndex, CActionError& error);
	
	/** Взять элемент с заданным индексом и преобразовать его в текстовый вид (и числа тоже - в текстовое представление)
	*	@param	int iIndex
	*	@param	CString& sRes
	*	@param	CActionError& error
	*	@return	BOOL 
	*/
	BOOL ConvertValueToString(int iIndex, CString& sRes, CActionError& error);
	
	/** Оператор присваивания
	*	@param	XmlRpcValues&
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator=(XmlRpcValues&);
	
	/** Оператор для добавления значения
	*	@param	XmlRpcValue*
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(XmlRpcValue*);
	
	/** Оператор для добавления значения типа string
	*	@param	const char* value
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(const char* value);
	
	/** Оператор для добавления значения типа long
	*	@param	long value
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(long value);
	
	/** Оператор для добавления значения типа double
	*	@param	double value
	*	@return	XmlRpcValues& 
	*/
	XmlRpcValues& operator<<(double value);
};

/** Класс для возвращения результата
*/
class XmlRpcResult
{
public:

	/** Ошибка, возникшая в результате обработки запроса
	*/
	CActionError error;

	/** Результат, полученный в резлультате обработки запроса
	*/
	XmlRpcValues values;
};

/** Класс для вызова функции, реализующей XmlRpc - метод
*/
class XmlRpcRequest
{
public:
	
	/** Конструктор по умолчанию
	*	@return	
	*/
	XmlRpcRequest(){};
	
	/** Деструктор
	*	@return	virtual 
	*/
	virtual ~XmlRpcRequest(){};

	/** Адрес сервера, к которому идет обращение
	*/
	const char* szURI;

	/** Название вызываемого метода
	*/
	const char* szMethodName;

	/** Значения, передаваемые вызываемогу методу
	*/
	XmlRpcValues values;
};

/** Типы Callback функций
*/
/** По получении данных сервером / клиентом
*	Для сервера: если возвращает TRUE, то сервер дальнейшей обратоки не производит
*	Для клиента: iServerNum=-1
*	@param	int iServerNum - номер сервера, получившего данные
*	@param	const char* szData - что получил сервер
*	@param	int iDataLength - сколько получил сервер
*	@return	BOOL - результат. Для сервера - если вернуть TRUE, то сервер не будет обрабатывать запрос далее
*/
typedef BOOL (CALLBACK *OnDataReceivedCallbackFunc)(int iServerNum, const char* szData, int iDataLength);

/** По посылке данных сервером / клиентом
*	Для сервера: если возвращает TRUE, то сервер дальнейшей обратоки не производит
*	Для клиента: iServerNum=-1
*	@param	int iServerNum - номер сервера
*	@param	const char* szData - посланные данные
*	@param	int iDataLength - сколько послано
*	@return	BOOL 
*/
typedef BOOL (CALLBACK *OnDataSendedCallbackFunc)(int iServerNum, const char* szData, int iDataLength);

/** По любой ошибке (где возникла ошибка передается в последнем параметре)
*	Для сервера: если возвращает TRUE, то сервер дальнейшей обработки не производит
*	Для клиента: iServerNum=-1
*	@param	int iServerNum - номер сервера
*	@param	SOCKET* sock - сокет (для отправки например собственного сообзения об ошибке)
*	@param	const char* szData - данные, полученные сервером к этому моменту
*	@param	CActionError& error - ошибка, в результате которой выхвалась эта функция
*	@return	BOOL - если вернуть TRUE, то дальнейшей обработки производиться не будет (SOCKET будет просто закрыт)
*/
typedef BOOL (CALLBACK *OnErrorCallbackFunc)(int iServerNum, SOCKET* sock, const char* szData, CActionError& error);

/** Тип функции, реализующей XmlRpc метод. Если вернет FALSE, то обратно будет послана ошибка из lpResult->error
*	@param	int iServerNum - номер сервера, которому пришел запрос на выполнение метода
*	@param	XmlRpcRequest* lpRequest - указатель на объект с параметрами запроса
*	@param	XmlRpcResult* lpResult - указатель на объект, в который надо записывать результат или ошибки
*	@return	BOOL - если вернуть FALSE то обратно будет послана ошибка из lpResult
*/
typedef BOOL (CALLBACK *OnXmlRpcMethodCallbackFunc)(int iServerNum, XmlRpcRequest* lpRequest, XmlRpcResult* lpResult);

/** Тип функции, которая будет вызываться при получении результатов асинхронных вызовов
*	@param	XmlRpcRequestEx* lpRequest - указательна структуру с описанием запроса
*	@return	BOOL - результат
*/
class XmlRpcRequestEx;
typedef BOOL (CALLBACK *OnXmlRpcOnMethodReturnedCallbackFunc)(XmlRpcRequestEx* lpRequest);

/** Класс для передачи доп. информации callback функции, связанной с запросом
*	Для передачи доп. данных надо отнаследоваться от XmlRpcRequestUserData и перегрузить
*	деструктор (он и будет вызван при отложенном удалении данных запроса)
*/
class XmlRpcRequestUserData
{
public:
	
	/** Конструктор по умолчанию
	*	@return	
	*/
	XmlRpcRequestUserData(){};
	
	/** Деструктор. В наследованных классах его следует перегрузить чтобы освобождать
	*	выделенные для UserData(см. далее) ресурсы
	*	@return	
	*/
	virtual ~XmlRpcRequestUserData(){};
};

/** Класс с расширенной информацией о вызове
*	Эта структура передается callback функциям при выполении запроса удаленным сервером
*/
class XmlRpcRequestEx: public XmlRpcRequest
{
public:
	
	/** Конструктор по умолчанию
	*	@return	
	*/
	XmlRpcRequestEx(){bStatus=0;dwTimeout=0;func=NULL;hThread=NULL;CallID=0;UserData=NULL;};
	
	/** Деструктор
	*	@return	
	*/
	~XmlRpcRequestEx(){delete (char*)szURI; delete (char*)szMethodName;if(UserData){delete UserData;};};

	/** статус обработки запроса
	*	1 - готово к обработке
	*	2 - обрабатывается
	*	3 - обработано
	*	0 - флаг для потока - закрыться
	*/
	BOOL bStatus;

	/** Callback - функция
	*/
	OnXmlRpcOnMethodReturnedCallbackFunc func;

	/** Handle потока в котором работает вызов
	*/
	HANDLE hThread;

	/** ID вызова в карте вызовов, хранящей параметры
	*/
	WORD CallID;

	/** Тайм аут ожидания ответа (0 - вечно :)
	*/
	DWORD dwTimeout;

	/** Ошибки, возникшие в процессе обраобтки запроса (проблемы с каналом например)
	*/
	CActionError error;

	/** Результат, полученный от удаленного сервера выполнившего запрос
	*/
	XmlRpcResult result;

	/** Пользовательские данные. При создании вызова они запоминаются и передаются callback функции после получения результата от удаленного сервера
	*	Для нормального освобождения ресурсов надо отнаследоваться от XmlRpcRequestUserData и перегрузить деструктор (он и будет вызван)
	*	И передавать при вызове указатель на класс потомок
	*/
	XmlRpcRequestUserData* UserData;

};

/** Запускает форму для тестирования клиентских вызовов
*	@return	void 
*/
void TestClientSide();

/** Запускает форму для тестирования серверной части
*	@return	void 
*/
void TestServerSide();

///////////////////////////////////
// Серверные функции и структуры //
///////////////////////////////////
/** Класс состояния сервера
*/
class CXmlRpcServerInfo
{
public:

	/** Коструктор по умолчанию
	*	@return	
	*/
	CXmlRpcServerInfo(){m_iStatus=0;m_iPort=0;m_iSuspended=1;m_iBetweenReqTime=10;m_ListenTimeout=3000;};

	/** Деструктор
	*	@return	virtual 
	*/
	virtual ~CXmlRpcServerInfo(){};

	/** порт
	*/
	int m_iPort;

	/** максимальное количество запросов в очереди
	*/
	int m_iMaxConnect;

	/** статус: 0 - не запущен, 1 - запущен
	*/
	int m_iStatus;

	/** статус2: 0 - приостановлен, 1 - в работе
	*/
	int m_iSuspended;

	/** периодичнось опроса на предмет новых запросов к серверу (млсек)
	*/
	int m_iBetweenReqTime;

	/** максимальный таймаут при получении сообщения между его частями (млсек)
	*/
	DWORD m_ListenTimeout;

	/** Список поддерживаемых методов и реализ. их функций
	*/
	CMapStringToPtr MethodMap;

	/** Оператор копирования
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

/** Класс статистики сервера
*/
class CXmlRpcServerStatistics
{
public:
	
	/** Конструктор по умолчанию
	*	@return	
	*/
	CXmlRpcServerStatistics(){lRequestHandled_Total=lRequestHandled_Error=lTotalBytesReceived=0;};
	
	/** Деструктор
	*	@return	virtual 
	*/
	virtual ~CXmlRpcServerStatistics(){};
	
	/** Добавить обработанный запрос
	*	@param	int amount=1
	*	@return	void 
	*/
	void AddHandled_Total(int amount=1){InterlockedExchangeAdd(&lRequestHandled_Total,amount);};
	
	/** Добавить обработанный неудачно запрос
	*	@param	int amount=1
	*	@return	void 
	*/
	void AddHandled_Error(int amount=1){InterlockedExchangeAdd(&lRequestHandled_Error,amount);};
	
	/** Добавить количество полученных байт
	*	@param	int amount=0
	*	@return	void 
	*/
	void AddReceivedBytes(int amount=0){InterlockedExchangeAdd(&lTotalBytesReceived,amount);};
	
	/** Добавлить количетсво отосланных байт
	*	@param	int amount=0
	*	@return	void 
	*/
	void AddSendedBytes(int amount=0){InterlockedExchangeAdd(&lTotalBytesSended,amount);};

	/** Обработано
	*/
	long lRequestHandled_Total;

	/** Обработано с ошибкой
	*/
	long lRequestHandled_Error;

	/** Байт получено
	*/
	long lTotalBytesReceived;

	/** Байт отослано
	*/
	long lTotalBytesSended;
};

/** Создает (но не запускает сервер) слушающий данный порт
*	@param	int - номер порта
*	@param	int - сколько соединений могут ожидать своей очереди (лишние будут отсекаться)
*	@param	int - сколько спать медлу проверками "есть ли новый запрос к серверу" (млсек)
*	@param	DWORD - во время получения запроса сколько максимально ждать между частями сообщения (млсек)
*	@param	CActionError& - ошибка (если возникла)
*	@return	int - индекс созданного сервера (можно создавать несколько на разные порты), -1 в случае ошибки
*/
int CreateServer(int iPort, int iBacklog, int iSleepBetweenReq, DWORD dwGetReqTimeout, CActionError& error);

/** Запускает сервер
*	@param	int - номер сервера
*	@param	CActionError& - описание ошибки если была
*	@return	BOOL - результат действия 
*/
BOOL StartServer(int iServerNum, CActionError& error);

/** Приостанавливает сервер
*	@param	int - номер сервера
*	@param	CActionError& - описание ошибки если была
*	@return	BOOL - результат действия
*/
BOOL PauseServer(int iServerNum, CActionError& error);

/** Остановить сервер
*	@param	int - номер сервера, если -1 то все
*	@param	CActionError& - результат действия
*	@return	BOOL - результат (TRUE/FALSE)
*/
BOOL StopServer(int iServerNum, CActionError& error);

/** Возвращает информацию о сервере
*	@param	int - номер сервера
*	@param	CXmlRpcServerInfo& - переменная, которая будет получать информацию о сервере
*	@param	CActionError& - результат действия
*	@return	BOOL - результат (TRUE/FALSE)
*/
BOOL GetServerInfo(int iServerNum, CXmlRpcServerInfo& ServerInfo);

/** Возвращает статистику сервера
*	@param	int - номер сервера
*	@param	CXmlRpcServerStatistics& - переменная, которая будет получать статистику сервера
*	@return	BOOL - результат (TRUE/FALSE)
*/
BOOL GetServerStatistics(int iServerNum, CXmlRpcServerStatistics& ServerStatistics);

/** Устанавливает функцию обратного вызова при посылке сервером/клиентом данных
*	@param	int - номер сервера для которого установить функцию или -1 для клиента
*	@param	OnDataSendedCallbackFunc lpFunction
*	@return	BOOL 
*/
BOOL OnXmlRpcServerDataSended(int iServerNum, OnDataSendedCallbackFunc lpFunction);

/** Устанавливает функцию обратного вызова при получении сервером/клиентом данных
*	@param	int - номер сервера для которого установить функцию или -1 для клиента
*	@param	OnDataReceivedCallbackFunction - функция обратного вызова
*	@return	BOOL - результат (TRUE - установилась, FALSE - нет)
*/
BOOL OnXmlRpcServerDataReceived(int iServerNum, OnDataReceivedCallbackFunc lpFunction);

/** Устанавливает функцию обратного вызова при какой либо ошибке возникшей во время работы сервера/клиента
*	@param	int - номер сервера для которого установить функцию или -1 для клиента
*	@param	OnErrorCallbackFunc lpFunction
*	@return	BOOL 
*/
BOOL OnXmlRpcServerError(int iServerNum, OnErrorCallbackFunc lpFunction);

/** Добавляет XmlRpc метод
*	@param	int - номер сервера, который должен поддерживать метод
*	@param	const char* - имя метода
*	@param	OnXmlRpcMethodCallbackFunc - функция, реализующая метод
*	@param	CActionError& - ошибка (если возникнет)
*	@return	BOOL - результат (TRUE/FALSE)
*/
BOOL AddXmlRpcMethod(int iServerNum, const char* szMethodName, OnXmlRpcMethodCallbackFunc lpFunction, CActionError& error);

/** Инициализация клиентской части
*	@param	long lTimeoutToHardTermination - сколько ждать пока работающий поток может быть не доделает свою работу
*	@param	long lReqBetweenCleanUp - количество вызовов между очисткой буфера параметров запросов
*	@return	BOOL 
*/
BOOL InitXmlRpcClient(long lTimeoutToHardTermination, long lReqBetweenCleanUp);

/** Деинициализация клиентской части
*	@return	BOOL - результат
*/
BOOL DeinitXmlRpcClient();

/** Вызвать XmlRpc метод
*	@param	XmlRpcRequest& - параметры вызова
*	@param	XmlRpcResult& - результат (если была ошибка, она будет содержаться внутри)
*	@param	DWORD - сколько ждать ответа в миллисекундах (0- вечно)
*	@param	CActionError& - ошибка при отсылке (если возникнет)
*	@return	BOOL - результат
*/
BOOL MakeXmlRpcCall(XmlRpcRequest& Request, XmlRpcResult& result, DWORD dwMilliseconds, CActionError& error);

/** Вызвать XmlRpc метод и вернуться не дожидаясь результата
*	когда на данный запрос будет получен ответ, будет вызвана функ. обр. вызова
*	@param	XmlRpcRequest& - параметры вызова
*	@param	DWORD - сколько ждать ответа в миллисекундах (0- вечно)
*	@param	OnXmlRpcOnMethodReturnedCallbackFunc - функция обр. вызова
*	@param	XmlRpcRequestUserData* - указатель на пользовательские данные (удалять их будет XmlRpc клиент!)
*	@return	BOOL - результат
*/
BOOL MakeCallbackXmlRpcCall(XmlRpcRequest& Request, DWORD dwMilliseconds, OnXmlRpcOnMethodReturnedCallbackFunc func, XmlRpcRequestUserData* UserData);

/** Остановить все ожидающиеся вызовы
*	@return	BOOL 
*/
BOOL TerminateAllCalls();

/** Получить статистику вызовов (сколько сделано, сколько висит, сколько вернулось с ошибкой)
*	@param	long& - сколько всего сделано вызовов
*	@param	long& - из них ошибочных
*	@param	long& - сколько находится в процессе
*	@param	long& - сколько запросов выполено, но память от них еще не очищена
*	@param	long& - сколько послано
*	@param	long& - сколько получено
*	@return	BOOL 
*/
BOOL GetClientStatistics(long& lTotal, long& lErrors, long& lInProcess, long& lFinishedButNotDeleted, long& lBytesSend, long& lBytesRec);

#endif//_SMALL_XMLRPC_CLIENTSERVER_H_