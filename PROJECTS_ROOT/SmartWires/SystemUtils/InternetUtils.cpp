// SupportClasses.cpp: implementation of the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "InternetUtils.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define AGENT_NAME  "Mozilla/4.0 (compatible; MSIE 6.0; Win32)"
//"MSIE 6.0"
#define INTERNET_OPEN_TYPE_PRECONFIG                    0   // use registry configuration
#define INTERNET_OPEN_TYPE_DIRECT                       1   // direct to net
#define INTERNET_OPEN_TYPE_PROXY                        3   // via named proxy
#define INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY  4   // prevent using java/script/INS
#define HTTP_QUERY_CONTENT_LENGTH						5
#define HTTP_QUERY_STATUS_CODE							19
#define HTTP_QUERY_FLAG_NUMBER							0x20000000
#define INTERNET_OPTION_PROXY                   38
typedef struct {

    //
    // dwAccessType - INTERNET_OPEN_TYPE_DIRECT, INTERNET_OPEN_TYPE_PROXY, or
    // INTERNET_OPEN_TYPE_PRECONFIG (set only)
    //

    DWORD dwAccessType;

    //
    // lpszProxy - proxy server list
    //

    LPCTSTR lpszProxy;

    //
    // lpszProxyBypass - proxy bypass list
    //

    LPCTSTR lpszProxyBypass;
} INTERNET_PROXY_INFOX;
CString CWebWorld::sProxyURL="";
long CWebWorld::lProxyPort=0;
CString CWebWorld::sProxyUser="";
CString CWebWorld::sProxyPsw="";
CWebWorld::CWebWorld()
{
	HttpQueryInfo=NULL;
	InternetOpen=NULL;
	InternetOpenUrl=NULL;
	InternetCloseHandle=NULL;
	InternetReadFile=NULL;
	InternetCheckConnection=NULL;
	HINSTANCE hWininet=LoadLibrary("wininet.dll");
	if(hWininet){
		InternetDial=(_InternetDialA)DBG_GETPROC(hWininet,"InternetDialA");
		HttpQueryInfo=(_HttpQueryInfoA)DBG_GETPROC(hWininet,"HttpQueryInfoA");
		InternetOpen=(_InternetOpenA)DBG_GETPROC(hWininet,"InternetOpenA");
		InternetOpenUrl=(_InternetOpenUrlA)DBG_GETPROC(hWininet,"InternetOpenUrlA");
		InternetCloseHandle=(_InternetCloseHandle)DBG_GETPROC(hWininet,"InternetCloseHandle");
		InternetReadFile=(_InternetReadFile)DBG_GETPROC(hWininet,"InternetReadFile");
		InternetCheckConnection=(_InternetCheckConnectionA)DBG_GETPROC(hWininet,"InternetCheckConnectionA");
		InternetSetOption=(_InternetSetOptionA)DBG_GETPROC(hWininet,"InternetSetOptionA");
	}
	bOnlyCheckConnection=0;
    // Initialize the Win32 Internet functions 
	/*
    m_Session = InternetOpen(AGENT_NAME, 
        INTERNET_OPEN_TYPE_PRECONFIG|INTERNET_OPEN_TYPE_PROXY, // Use registry settings. 
        NULL, // Proxy name. NULL indicates use default.
        NULL, // List of local servers. NULL indicates default. 
        0) ;
	*/
	const char* szProxy=GetProxyLine();
	m_Session = InternetOpen(AGENT_NAME, 
		szProxy?INTERNET_OPEN_TYPE_PROXY:INTERNET_OPEN_TYPE_PRECONFIG, // Use registry settings. 
        szProxy, // Proxy name. NULL indicates use default.
        NULL, // List of local servers. NULL indicates default. 
        0) ;
#ifdef _DEBUG
	if(m_Session==0){
		DWORD dwErr=GetLastError();
		DWORD dwError=0;
		char sz[1024]={0};
		DWORD dw=sizeof(sz);
		BOOL b=0;//InternetGetLastResponseInfoA(&dwError, sz,&dw);
		CString s;
		s.Format("InternetOpen err=0x%08X, err2=0x%08X '%s'. pline=%s",dwErr,dwError,sz,GetProxyLine());
		AfxMessageBox(s);
	}
#endif
}

CWebWorld::~CWebWorld()
{
    // Closing the session
    InternetCloseHandle(m_Session);
	if(hWininet){
		FreeLibrary(hWininet);
	}
}

BOOL CWebWorld::TestWebPage(const CString& Url, DWORD dwTimeout, CString* sUrlError)
{
	HINTERNET hHttpFile=0;
	hHttpFile = InternetOpenUrl(m_Session, (const char *) Url, NULL, 0, 0, 0);
	SetProxyInfo(hHttpFile);
    if (hHttpFile){
		InternetCloseHandle(hHttpFile); // Close the connection.
		return TRUE;
	}
	if(sUrlError){
	}
	return FALSE;
}

BOOL CWebWorld::GetWebFile(const CString Url, CString sOutFileName, DWORD* dwStatusCode)
{
    HINTERNET hHttpFile=0;
    char szSizeBuffer[32]="";
    DWORD dwFileSize=0;
    DWORD dwBytesRead=0;
    BOOL bSuccessful=0;

    // Opening the Url and getting a Handle for HTTP file
    hHttpFile = InternetOpenUrl(m_Session, (const char *)Url, NULL, 0, 0, 0);
	SetProxyInfo(hHttpFile);
    if (hHttpFile)
    {    
		if(dwStatusCode){
			DWORD dwLengthSizeBuffer = sizeof(DWORD);
			HttpQueryInfo(hHttpFile,HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, dwStatusCode, &dwLengthSizeBuffer, NULL) ;
		}
		DWORD dwLengthSizeBuffer = sizeof(szSizeBuffer);
        // Getting the size of HTTP Files
        BOOL bQuery = HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH, szSizeBuffer, &dwLengthSizeBuffer, NULL) ;
		dwFileSize=atol(szSizeBuffer);
		BOOL bRead=FALSE;
		if(bQuery==FALSE && sOutFileName==""){
			// Это был тест
			if(dwFileSize==0){
				// Пробуем всеже счиать что-нить...
				char szBuffer[256]={0};
				bQuery = InternetReadFile(hHttpFile, szBuffer, sizeof(szBuffer), &dwFileSize);
			}
		}
		if(bQuery==TRUE){
			if(sOutFileName==""){
				bRead=(dwFileSize>0)?1:0;
			}else{
				// Allocating the memory space for HTTP file contents
				BYTE* bContent=new BYTE[dwFileSize];
				// Read the HTTP file 
				bRead = InternetReadFile(hHttpFile, bContent, dwFileSize, &dwBytesRead); 
				FILE* m_pFile=fopen(sOutFileName,"w+b");
				if(!m_pFile){
					return FALSE;
				}
				DWORD nRead=fwrite(bContent,sizeof(BYTE),dwFileSize,m_pFile);
				fclose(m_pFile);
				delete[] bContent;
			}
		}
		if (bRead){
			bSuccessful = TRUE;
		}
		InternetCloseHandle(hHttpFile); // Close the connection.
    }else{
        // Connection failed.
        bSuccessful = FALSE;
    } 
    return bSuccessful;
}

#define URLPAGEBUF_SIZE 50000
CString CWebWorld::GetWebPage(const CString& Url)
{
    HINTERNET hHttpFile=0;
    char szSizeBuffer[32]="";
    DWORD dwLengthSizeBuffer = sizeof(szSizeBuffer); 
    DWORD dwFileSize=0;
    DWORD dwBytesRead=0;
    BOOL bSuccessful=0;
    CString Contents;

    // Setting default error message
    Contents = m_ErrorMessage;
    
    // Opening the Url and getting a Handle for HTTP file
    hHttpFile = InternetOpenUrl(m_Session, (const char *) Url, NULL, 0, 0, 0);
	DWORD dwErr=GetLastError();
	if(hHttpFile==0){
#ifdef _DEBUG
		DWORD dwError=0;
		char sz[1024]={0};
		DWORD dw=sizeof(sz);
		BOOL b=0;//InternetGetLastResponseInfoA(&dwError, sz,&dw);
		//TranslateErrorcode
		CString s;
		s.Format("InternetOpenUrl err=0x%08X, err2=0x%08X '%s'. pline=%s",dwErr,dwError,sz,GetProxyLine());
		AfxMessageBox(s);
#endif
	}
	SetProxyInfo(hHttpFile);
    if (hHttpFile)
    {
		if(bOnlyCheckConnection){
			Contents=Url;
		}else{
			// Getting the size of HTTP Files
			BOOL bQuery = HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH, szSizeBuffer, &dwLengthSizeBuffer, NULL) ;
			BOOL bRead=FALSE;
			if(bQuery==TRUE){    
				// Allocating the memory space for HTTP file contents
				dwFileSize=atol(szSizeBuffer)+1;
				LPSTR szNewContent = new char[dwFileSize];
				memset(szNewContent ,0, dwFileSize);
				// Read the HTTP file 
				bRead = InternetReadFile(hHttpFile, szNewContent, dwFileSize, &dwBytesRead); 
				Contents=szNewContent;
				delete[] szNewContent;
			}else{
				// setting static buffer size, as we cannot determine it
				char szBuffer[URLPAGEBUF_SIZE+1];
				Contents.Empty();
				
				// Read the HTTP file 
				DWORD dwPos = 0;
				do {
					Sleep(10);
					bRead = InternetReadFile(hHttpFile, szBuffer, URLPAGEBUF_SIZE, &dwBytesRead); 
					dwPos += dwBytesRead;
					
					// ensuring, that buffer is dwBytesRead size
					if ( dwBytesRead > 0 )
					{
						szBuffer[dwBytesRead] = '\0';
						Contents += szBuffer;
					}
				}while (bRead && dwBytesRead>0);
			}
			if (bRead){
				bSuccessful = TRUE;
			}
		}
		InternetCloseHandle(hHttpFile); // Close the connection.
    }else{
        // Connection failed.
        bSuccessful = FALSE;
		DWORD dwErr=GetLastError();
		/*
		char szErr[1024]="";
		DWORD dwErr2=0,dwErrLen=sizeof(szErr);
		int iErr=InternetGetLastResponseInfo(&dwErr2,szErr,&dwErrLen);
		if(iErr==0){
			iErr=0;
		}
		*/
    } 
#ifdef _DEBUG
	//{CString s;s.Format("Loading URL %s:\n%s\n\n\n\n",Url,Contents);FILE* f=fopen("c:\\loading.html","a+");fprintf(f,"%s",s);fclose(f);};
#endif
    return Contents;
}

CString CWebWorld::GetErrorMessage()
{
	return m_ErrorMessage;
};

void CWebWorld::SetErrorMessage(CString s)
{
    m_ErrorMessage = s;
}

BOOL CWebWorld::CheckConnection(LPCSTR lpszUrl, DWORD dwFlags, DWORD dwReserved)
{
	return InternetCheckConnection(lpszUrl, dwFlags, dwReserved);
}

const char* CWebWorld::GetProxyLine()
{
	CString Proxy;
	if( sProxyURL=="" || sProxyURL[0]=='?'){
		return NULL;
	}
	if( sProxyUser!=""){
		Proxy = sProxyUser + ":" + sProxyPsw + "@";
	}
	Proxy = Proxy + sProxyURL;
	if(lProxyPort){
		char strPort[33] = {0};
		ltoa(lProxyPort,strPort,10);
		Proxy = Proxy + ":" + CString(strPort);
	}
	static char szProxy[1024]={0};
	strcpy(szProxy,Proxy);
	return szProxy;
}

void CWebWorld::SetProxyInfo(HINTERNET hIn)
{
	if(hIn!=0){
		// Уже установили
		return;
	}
	INTERNET_PROXY_INFOX ipi;
	ipi.lpszProxyBypass = NULL;

	CString Proxy=GetProxyLine();
	if( sProxyURL=="")
	{
		ipi.dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
		ipi.lpszProxy = NULL;
	}
	else
	{
		ipi.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
		ipi.lpszProxy = Proxy;
	}
	//UrlMkSetSessionOption(INTERNET_OPTION_PROXY,&ipi,sizeof(INTERNET_PROXY_INFO),0);
	CWebWorld ww;
	BOOL bRes=ww.InternetSetOption(hIn,INTERNET_OPTION_PROXY,&ipi,sizeof(ipi));
}
