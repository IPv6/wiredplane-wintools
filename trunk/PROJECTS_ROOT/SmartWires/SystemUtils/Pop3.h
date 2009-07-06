/*
Module : POP3.H
Purpose: Defines the interface for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998

  Copyright (c) 1998 - 2001 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)
  
	All rights reserved.
	
	  Copyright / Usage Details:
	  
		You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
		when your product is released in binary form. You are allowed to modify the source code in any way you want 
		except you cannot modify the copyright details at the top of each module. If you want to distribute source 
		code with your application, then you are only allowed to distribute versions released by the author. This is 
		to maintain a single distribution point for the source code. 
		
*/


/////////////////////////////// Defines ///////////////////////////////////////
#ifndef __POP3_H__
#define __POP3_H__

#ifndef __AFXTEMPL_H__
#pragma message("POP3 classes require afxtempl.h in your PCH")																				  
#endif

#ifndef _WINSOCKAPI_
#pragma message("POP3 classes require afxsock.h or winsock.h in your PCH")
#endif

#include <MAPI.H>
//#include "imapi.h"
/////////////////////////////// Classes ///////////////////////////////////////

////// forward declaration
class CPop3Connection;

//Encapsulation of a POP3 message
class CPop3Message
{
public:
	//Constructors / Destructors
	CPop3Message();
	~CPop3Message();
	
	//Methods
	LPCSTR	GetMessageText() const { return m_pszMessage; };
	CString GetHeader() const;
	CString GetHeaderItem(const CString& sName, int nItem = 0) const;
	CString GetBody() const;
	LPCSTR	GetRawBody() const;
	CString GetSubject() const { return GetHeaderItem(_T("Subject")); }
	CString GetFrom() const 	 { return GetHeaderItem(_T("From")); }
	CString GetDate() const 	 { return GetHeaderItem(_T("Date")); }
	CString GetTo() const	 { return GetHeaderItem(_T("To")); }
	CString GetCC() const		 { return GetHeaderItem(_T("CC")); }
	CString GetReplyTo() const;
	CString GetContentType() const;
	CString GetBoundary() const;
	
	//protected:
	char* m_pszMessage;
	
	friend class CPop3Connection;
};


//Simple Socket wrapper class
class CPop3Socket
{
public:
	//Constructors / Destructors
	CPop3Socket();
	~CPop3Socket();
	
	//methods
	BOOL	Create();
	BOOL	Connect(LPCTSTR pszHostAddress, int nPort = 110);
	BOOL	Send(LPCSTR pszBuf, int nBuf);
	void	Close();
	int		Receive(LPSTR pszBuf, int nBuf);
	BOOL	IsReadible(BOOL& bReadible);
	
protected:
	BOOL		Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	SOCKET		m_hSocket;
	
	friend class CPop3Connection;
};


//The main class which encapsulates the POP3 connection
class CPop3Connection
{
public:
	//Constructors / Destructors
	CPop3Connection();
	~CPop3Connection();
	
	//Methods
	BOOL		Connect(LPCTSTR pszHostName, LPCTSTR pszUser, LPCTSTR pszPassword, int nPort = 110, CString* sError=NULL);
	BOOL		Disconnect();
	BOOL		Statistics(int& nNumberOfMails, int& nTotalMailSize);
	BOOL		Delete(int nMsg);
	BOOL		GetMessageSize(int nMsg, DWORD& dwSize);
	BOOL		GetMessageID(int nMsg, CString& sID);
	BOOL		Retrieve(int nMsg, CPop3Message& message);
	BOOL		GetMessageHeader(int nMsg, CPop3Message& message, int iLines=0, DWORD* pdwMsgSize=NULL);
	BOOL		Reset();
	BOOL		UIDL();
	BOOL		Noop();
	CString 	GetLastCommandResponse() const { return m_sLastCommandResponse; };
	DWORD		GetTimeout() const { return m_dwTimeout; };
	void		SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };
	BOOL		m_bConnected;	
protected:
	virtual BOOL	ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize);
	virtual BOOL	ReadCommandResponse();
	virtual BOOL	ReadListResponse(int nNumberOfMails);
	virtual BOOL	ReadUIDLResponse(int nNumberOfMails);
	virtual BOOL	ReadReturnResponse(CPop3Message& message, DWORD dwSize);
	virtual BOOL	ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR pszTerminator, LPSTR* ppszOverFlowBuffer, int nGrowBy=4096);
	BOOL			List();
	LPSTR			GetFirstCharInResponse(LPSTR pszData) const;
	
	CPop3Socket		m_Pop;
	int 			m_nNumberOfMails;
	BOOL			m_bListRetrieved;
	BOOL			m_bStatRetrieved;
	BOOL			m_bUIDLRetrieved;
	CDWordArray		m_msgSizes;
	CStringArray	m_msgIDs;
	CString			m_sLastCommandResponse;
	DWORD			m_dwTimeout;
};


class CMIMECode  
{
public:
    CMIMECode();
    virtual ~CMIMECode();
	
    virtual CString Decode( LPCTSTR szDecoding, int nSize = -1) = 0;
    virtual CString Encode( LPCTSTR szEncoding, int nSize = -1) = 0;
};

class CQuoted : public CMIMECode  
{
public:
    int m_linelength;
    CQuoted(int linelength=76);
    virtual ~CQuoted();
    virtual CString Decode( LPCTSTR szDecoding, int nSize = -1);
    virtual CString Encode( LPCTSTR szEncoding, int nSize = -1);
protected:
    static const CString code;
    static const CString extended_code;
};

class CBase64 : public CMIMECode  
{
public:
    CBase64();
    virtual ~CBase64();
	
	// Override the base class mandatory functions
    virtual CString Decode( LPCTSTR szDecoding, int nSize = -1);
    virtual CString Encode( LPCTSTR szEncoding, int nSize = -1);
	
protected:
    void write_bits( UINT nBits, int nNumBts, LPTSTR szOutput, int& lp );
    UINT read_bits( int nNumBits, int* pBitsRead, int& lp );
	
    int m_nInputSize;
    int m_nBitsRemaining;
    ULONG m_lBitStorage;
    LPCTSTR m_szInput;
	
    static int m_nMask[];
    static CString m_sBase64Alphabet;
private:
};

#include <mlang.h>
#include <atlbase.h>
class CCharsetDecoder  
{
public:
    CString m_sCharSetName;
    CCharsetDecoder(CString sCharSetName): m_sCharSetName(sCharSetName) {};
    CCharsetDecoder(UINT CodePage);
    ~CCharsetDecoder(){};
    CString Encode(CString sString); // Encodes the string in default charset (e.g. 1251 for Russian) to the given codepage
    CString Decode(CString sString); // Decodes the string in the given charset (e.g. koi8-r/*=20866)*/ to the default charset
	BOOL isKnown();
private:
    CString Recode(CString sString, bool bEncode, bool* bRes=NULL);
};

CString DecodeHeaderValue(CString Encoded, CString *pCharset);
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
const int DEFAULT_PROTOCOL = 0;
const int NO_FLAGS = 0;

class CFastSmtp  
{
public:        
    CFastSmtp();    
    virtual ~CFastSmtp();
    bool    AddRecipient(const char email[], const char name[]="");
    bool    AddBCCRecipient(const char email[], const char name[]="");
    bool    AddCCRecipient(const char email[], const char name[]="");    
    bool    ConnectServer(const char server[], const unsigned short port=NULL);
    bool    Disconnect();
    bool    GetConnectStatus();    
    const unsigned int    GetBCCRecipientCount();    
    const unsigned int    GetCCRecipientCount();
    const unsigned int    GetRecipientCount();    
    const unsigned int    GetSocket();
    const char*    const    GetLocalHostIp();
    const char*    const    GetLocalHostname();    
    const char*    const    GetMessageBody();    
    const char*    const    GetReplyTo();
    const char*    const    GetSenderEmail();
    const char*    const    GetSenderName();
    const char*    const    GetSubject();    
    const char*    const    GetXMailer();    
    bool    Send(CString& sErr);
    void    SetMessageBody(const char body[]);    
    void    SetSubject(const char subject[]);    
    void    SetSenderName(const char name[]);    
    void    SetSenderEmail(const char email[]);    
    void    SetReplyTo(const char replyto[]);    
    void    SetXMailer(const char xmailer[]);
	CString username;
	CString password;
	void SetUserName( CString un ){ username = un; };
	CString GetUserName() { return username; };
	void SetPassward( CString psw ) { password = psw; };
	CString GetPassward() { return password; };
	CString sFormat;
	void SetOutFormat(CString sf){sFormat=sf;};
private:
    class CRecipient
    {
    public:
        CRecipient() 
        { 
            m_pcEmail = NULL;
        };
        CRecipient& operator=(const CRecipient& src)
        {
            if (&src != this)
            {
                if (m_pcEmail)
                    delete [] m_pcEmail;
                int s = strlen(src.m_pcEmail);
                m_pcEmail = new char[s+1];
                strcpy(m_pcEmail, src.m_pcEmail);
            }
            return (*this);
        };
        virtual ~CRecipient()
        {
            if (m_pcEmail)
                delete [] m_pcEmail;
        };
        char* GetEmail()
        {
            return m_pcEmail;
        };
        void SetEmail(const char email[])
        {
            int s=strlen(email);
            if (s > 0)
            {
                m_pcEmail = new char[s+1];
                strcpy(m_pcEmail, email);
            }        
        };
    private:
        char *m_pcEmail;
    };
    bool bCon;
    char m_cHostName[MAX_PATH];
    char* m_pcFromEmail;
    char* m_pcFromName;
    char* m_pcSubject;
    char* m_pcMsgBody;
    char* m_pcXMailer;
    char* m_pcReplyTo;
    char* m_pcIPAddr;    
	
    WSADATA wsaData;
    SOCKET hSocket;
	
    CArray<CRecipient*, CRecipient*> Recipients;
    CArray<CRecipient*, CRecipient*> CCRecipients;
    CArray<CRecipient*, CRecipient*> BCCRecipients;    
    
    CString _formatHeader();    
    bool    _formatMessage();
    SOCKET  _connectServerSocket(const char* server, const unsigned short port=NULL);    
};

int b64decode(char *from,char *to,int from_length);
int b64get_encode_buffer_size(int from_length,int output_quads_per_line);
int b64strip_encoded_buffer(char *buf,int length);
int b64encode(const char *from,char *to,int length,int quads);


/*
 *	$Header: /common/IMapi.h 1     28/03/01 8:42p Admin $
 *
 *	$History: IMapi.h $
 * 
 * *****************  Version 1  *****************
 * User: Admin        Date: 28/03/01   Time: 8:42p
 * Created in $/common
 */

class CIMapi
{
public:
					CIMapi();
					~CIMapi();

	enum errorCodes
	{
		IMAPI_SUCCESS = 0,
		IMAPI_LOADFAILED,
		IMAPI_INVALIDDLL,
		IMAPI_FAILTO,
		IMAPI_FAILCC,
		IMAPI_FAILATTACH
	};

//	Attributes
	void			Subject(LPCTSTR subject);
	void			Text(LPCTSTR text)			{ m_text = text; }

	UINT			Error();
	void			From(LPCTSTR from)			{ m_from.lpszName = (LPTSTR) from; }

	static BOOL		HasEmail();

//	Operations
	BOOL			To(LPCTSTR recip);
	BOOL			Attach(LPCTSTR path, LPCTSTR name = NULL);
	
	BOOL			Send(ULONG flags = 0);
	BOOL			SetHTML(){/*m_message.lpszMessageType="text/html";*/return 1;};
private:
	BOOL			AllocNewTo();

	MapiMessage		m_message;
	MapiRecipDesc	m_from;
	UINT			m_error;
	CString			m_text;
	LHANDLE			m_sessionHandle;

	ULONG (PASCAL *m_lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	ULONG (PASCAL *m_lpfnMAPILogon)(ULONG, LPTSTR, LPTSTR, FLAGS, ULONG, LPLHANDLE);
	
	static HINSTANCE m_hInstMail;
	static BOOL		m_isMailAvail;
};
CString Str2UTF8(const WCHAR* input);
CString UTF82Str(const char* input);
CString urlEscapeString(const char* szString,CString sAddChars="+\\/:?&%='\"");
CString GetURLForMailByWWW(CString sFrom,CString sTo,CString sSubject,CString sBody);
char ntc(unsigned char n);
unsigned char ctn(char c);
int b64encode(const char *from,char *to,int length,int quads);
int b64decode(char *from,char *to,int length);
int b64get_encode_buffer_size(int l,int q);
int b64strip_encoded_buffer(const char *buf,int length);
#endif //__POP3_H__

