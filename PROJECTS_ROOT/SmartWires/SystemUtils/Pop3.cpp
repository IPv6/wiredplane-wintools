/*
Module : POP3.CPP
Purpose: Implementation for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998
History: PJN / 27-06-1998	
1) Fixed a potential buffer overflow problem in Delete
and Retrieve functions when a large message number was
specified.
2) Improve the ReadResponse code by a) passing the 
readability check onto the scoket class and b) Sleeping
for 100 ms prior to looping around waiting for new 
response data
3) Classes are now fully Unicode compliant. Unicode
build configurations are also now included.
4) Now supports the TOP POP3 command which can be
issued using the GetHeader function.
PJN / 04-01-1999 
1) Properly UNICODE enabled the code
PJN / 22-02-1999 
1) Improved the reading of responses back from the server by implementing
a growable receive buffer
2) timeout is now 60 seconds when building for debug
3) Code now yields its time slice while waiting for a timeout
4) Made most functions virtual to help end user usage
PJN / 25-03-1999	
1) Fixed memory leak in the CPop3Connection::ReadReturnResponse function.
2) Now sleeps for 250 ms instead of yielding the time slice. This helps 
reduce CPU usage when waiting for data to arrive in the socket
PJN / 15-06-1999 
1) Added functions to return the message body, header or a particular
header field of a message
2) Tidied up some of the TRACE messages which the code generates
PJN / 16-06-1999 
1) Fixed a bug in the GetHeaderItem function which was causing a header
item which appeared at the begining of the header fields to fail to be 
parsed incorrectly.
PJN / 27-06-1999 
1) Fixed a bug in the GetHeaderItem function when a header spanned 
multiple lines as is allowed by RFC 822
PJN / 29-06-1999 
1) Another improvement to GetHeaderItem. When will it end <g>. Originally 
this was reported as a bug but upon further investigation it turns out that
the message which was causing the problems had embedded tabs in the header. 
This is discouraged by the RFC which refers to mail headers (RFC 822). 
The code has been enhanced to handle this case. Thanks to Chris Bishop 
for spotting this.
2) Fix for a bug in GetHeaderItem where I accidently was using "=" instead of
"==". Thanks to Angelini Fabio for spotting this.
PJN / 05-07-1999 
1) Addition of the following functions:
i)   CPop3Message::GetReplyTo
ii)  CPop3Message::GetRawBody 	 
iii) CPop3Message::GetSubject 				
iv)  CPop3Message::GetFrom					
v)   CPop3Message::GetDate					
2) GetHeaderItem function now uses case insensitive searching
3) GetHeaderItem now allows you to search for the "n'th" header of a specified type
PJN / 24-08-1999 
1) Fixed a bug whereby the function GetHeader was sometimes failing when it
was called when the message was retrieved using the "TOP" command.
PJN / 27-03-2000 
1) Fixed a problem where GetBody and GetRawBody will fail if you call it for a 
CPop3Message object that doesn't have a message in it (i.e m_pszMessage == NULL). 
This was previously causing a goof old access violation in GetRawBody.
PJN / 20-09-2000 
1) Fixed a bug in CPop3Connection::UIDL when message id's were returned
with embedded dots in them. Thanks to Konstantin Vasserman for finding and resolving
this problem.
PJN / 25-03-2001 
1) Updated copyright information
PJN / 27-07-2001 
1) Added two methods namely: GetTo() and GetCC() to the CPop3Message class
PJN / 11-08-2001 
1) Fixed a bug in CPop3Connection::ReadResponse where determining the terminator
was failing when an embedded NULL was in the message.
PJN / 27-09-2001 
1) Fixed a bug in CPop3Connection::ReadResponse when handling disconnection errors
2) Improved the error handling in Delete, GetMessageSize & GetMessageID methods
PJN / 29-09-2001 
1) Further improved the error handling in CPop3Connection::ReadResponse
Copyright (c) 1998 - 2001 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)
All rights reserved.
Copyright / Usage Details:
You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 
*/

//	AV / 15-01-2002
//	CPop3Message::GetBoundary() added
//  CPop3Message::GetContentType() added
//  CPop3Connection::~CPop3Connection() changed

//////////////// Includes ////////////////////////////////////////////
#include "stdafx.h"
#include "pop3.h"



//////////////// Macros //////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////// Implementation //////////////////////////////////////
CPop3Message::CPop3Message()
{
	m_pszMessage = NULL;
}

CPop3Message::~CPop3Message()
{
	if (m_pszMessage)
	{
		delete [] m_pszMessage;
		m_pszMessage = NULL;
	}
}

CString CPop3Message::GetHeaderItem(const CString& sName, int nItem) const
{
	//Value which will be returned by this function
	CString sField;

	//Get the message header (add an extra "\r\n" at the
	//begining to aid in the parsing)  
	CString sHeader(_T("\r\n"));
	sHeader += GetHeader();
	CString sUpCaseHeader(sHeader);
	sUpCaseHeader.MakeUpper();

	CString sUpCaseName(sName);
	sUpCaseName.MakeUpper();

	//Find the specified line in the header
	CString sFind(CString(_T("\r\n")) + sUpCaseName + _T(":"));
	int nFindLength = sFind.GetLength();
	int nFindStart = sUpCaseHeader.Find(sFind);
	int nFind = nFindStart;
	for (int i=0; i<nItem; i++) 
	{
		//Get ready for the next loop around
		sUpCaseHeader = sUpCaseHeader.Right(sUpCaseHeader.GetLength() - nFind - nFindLength);
		nFind = sUpCaseHeader.Find(sFind);

		if (nFind == -1)
			return _T(""); //Not found
		else
			nFindStart += (nFind + nFindLength);
	}

	if (nFindStart != -1)
		nFindStart += (3 + sName.GetLength());
	if (nFindStart != -1)
	{
		BOOL bFoundEnd = FALSE;
		int i = nFindStart;
		int nLength = sHeader.GetLength();
		do
		{
			//Examine the current 3 characters
			TCHAR c1 = _T('\0');
			if (i < nLength)
				c1 = sHeader[i];
			TCHAR c2 = _T('\0');
			if (i < (nLength-1))
				c2 = sHeader[i+1];
			TCHAR c3 = _T('\0');
			if (i < (nLength-2))
				c3 = sHeader[i+2];

			//Have we found the terminator
			if ((c1 == _T('\0')) ||
				((c1 == _T('\r')) && (c2 == _T('\n')) && (c3 != _T(' ')) && c3 != _T('\t')))
			{
				bFoundEnd = TRUE;
			}
			else
			{
				//Move onto the next character	
				++i;
			}
		}
		while (!bFoundEnd);
		sField = sHeader.Mid(nFindStart, i - nFindStart);

		//Remove any embedded "\r\n" sequences from the field
		int nEOL = sField.Find(_T("\r\n"));
		while (nEOL != -1)
		{
			sField = sField.Left(nEOL) + sField.Right(sField.GetLength() - nEOL - 2);
			nEOL = sField.Find(_T("\r\n"));
		}

		//Replace any embedded "\t" sequences with spaces
		int nTab = sField.Find(_T('\t'));
		while (nTab != -1)
		{
			sField = sField.Left(nTab) + _T(' ') + sField.Right(sField.GetLength() - nTab - 1);
			nTab = sField.Find(_T('\t'));
		}

		//Remove any leading or trailing white space from the Field Body
		sField.TrimLeft();
		sField.TrimRight();
	}

	return sField;
}

CString CPop3Message::GetHeader() const
{
	//Value which will be returned by this function
	CString sHeader;

	//Find the divider between the header and body
	CString sMessage(m_pszMessage);
	int nFind = sMessage.Find(_T("\r\n\r\n"));
	if (nFind != -1)
		sHeader = sMessage.Left(nFind);
	else
	{
		//No divider, then assume all the text is the header
		sHeader = sMessage;
	}

	return sHeader;
}

LPCSTR CPop3Message::GetRawBody() const
{
	if (m_pszMessage == NULL)
		return NULL;

	char* pszStartBody = strstr(m_pszMessage, "\r\n\r\n");
	if (pszStartBody == NULL) 
		return NULL;
	else 
		return pszStartBody + 4;
}


CString CPop3Message::GetBody() const
{
	CString sBody;
	LPCSTR pszBody = GetRawBody();
	if (pszBody)
		sBody = pszBody;
	return sBody;
}


CString CPop3Message::GetReplyTo() const
{
	CString sRet = GetHeaderItem("Reply-To");
	if (sRet.IsEmpty())
	{
		sRet = GetFrom();
		if (sRet.IsEmpty())
		{
			sRet = GetHeaderItem(_T("Sender"));
			if (sRet.IsEmpty())
				sRet = GetHeaderItem(_T("Return-Path"));
		}
	}
	return sRet;
}

CString CPop3Message::GetContentType() const
{
	CString sRet = GetHeaderItem("Content-Type");
	if(sRet.IsEmpty())
		return sRet;

	INT iPos = sRet.Find(";");
	if(iPos == -1)
		return sRet;
	sRet = sRet.Left(iPos);

	return sRet;
}

CString CPop3Message::GetBoundary() const
{
	CString sRet = GetHeaderItem("Content-Type");
	if(sRet.IsEmpty())
		return sRet;

	INT iPos = sRet.Find("\"");
	if(iPos == -1)
	{
		sRet.Empty();
		return sRet;
	}
	if(iPos == sRet.GetLength())
	{
		sRet.Empty();
		return sRet;
	}
	sRet = sRet.Right(sRet.GetLength() - iPos - 1);
	iPos = sRet.Find("\"");
	if(iPos == -1)
	{
		sRet.Empty();
		return sRet;
	}
	sRet = sRet.Left(iPos);

	return sRet;
}


CPop3Socket::CPop3Socket()
{
	m_hSocket = INVALID_SOCKET; //default to an invalid scoket descriptor
}

CPop3Socket::~CPop3Socket()
{
	Close();
}

BOOL CPop3Socket::Create()
{
	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
	return (m_hSocket != INVALID_SOCKET);
}

BOOL CPop3Socket::Connect(LPCTSTR pszHostAddress, int nPort)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

	//must have been created first
	ASSERT(m_hSocket != INVALID_SOCKET);

	//Determine if the address is in dotted notation  
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)nPort);
	char* pszAsciiHostAddress = T2A((LPTSTR) pszHostAddress);
	sockAddr.sin_addr.s_addr = inet_addr(pszAsciiHostAddress);

	//If the address is not dotted notation, then do a DNS 
	//lookup of it.
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(pszAsciiHostAddress);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL); 
			return FALSE;
		}
	}

	//Call the protected version which takes an address 
	//in the form of a standard C style struct.
	return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

BOOL CPop3Socket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	return (connect(m_hSocket, lpSockAddr, nSockAddrLen) != SOCKET_ERROR);
}

BOOL CPop3Socket::Send(LPCSTR pszBuf, int nBuf)
{
	//must have been created first
	ASSERT(m_hSocket != INVALID_SOCKET);

	return (send(m_hSocket, pszBuf, nBuf, 0) != SOCKET_ERROR);
}

int CPop3Socket::Receive(LPSTR pszBuf, int nBuf)
{
	//must have been created first
	ASSERT(m_hSocket != INVALID_SOCKET);

	return recv(m_hSocket, pszBuf, nBuf, 0); 
}

void CPop3Socket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		VERIFY(SOCKET_ERROR != closesocket(m_hSocket));
		m_hSocket = INVALID_SOCKET;
	}
}

BOOL CPop3Socket::IsReadible(BOOL& bReadible)
{
	timeval timeout = {0, 0};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_hSocket, &fds);
	int nStatus = select(0, &fds, NULL, NULL, &timeout);
	if (nStatus == SOCKET_ERROR)
	{
		return FALSE;
	}
	else
	{
		bReadible = !(nStatus == 0);
		return TRUE;
	}
}

CPop3Connection::CPop3Connection()
{
	m_nNumberOfMails = 0;
	m_bListRetrieved = FALSE;
	m_bStatRetrieved = FALSE;
	m_bUIDLRetrieved = FALSE;
	m_msgSizes.RemoveAll();
	m_bConnected = FALSE;
#ifdef _DEBUG
	m_dwTimeout = 60000; //default timeout of 60 seconds when debugging
#else
	m_dwTimeout = 10000;  //default timeout of 10 seconds for normal release code
#endif
}

CPop3Connection::~CPop3Connection()
{
	//if (m_bConnected)
	//	Disconnect();
	//free up our socket
	m_Pop.Close();

}

BOOL CPop3Connection::Connect(LPCTSTR pszHostName, LPCTSTR pszUser, LPCTSTR pszPassword, int nPort, CString* sError)
{
	if(pszHostName==0 || strlen(pszHostName)==0){
		return FALSE;
	}
	if(pszUser==0 || strlen(pszUser)==0){
		return FALSE;
	}
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

	//Create the socket
	if (!m_Pop.Create())
	{
		if(sError){
			sError->Format(_T("Failed to create client socket, GetLastError:%d\n"), GetLastError());
		}
		return FALSE;
	}

	//Connect to the POP3 Host
	if (!m_Pop.Connect(pszHostName, nPort))
	{
		if(sError){
			*sError=_T("POP3 server unavailable\n");
		}
		return FALSE;
	}
	else
	{
		//We're now connected !!
		m_bConnected = TRUE;

		//check the response
		if (!ReadCommandResponse())
		{
			if(sError){
				*sError=_T("Failed to read a command response from server\n");
			}
			Disconnect();
			return FALSE;
		}

		//Send the POP3 username and check the response
		char sBuf[128];
		char* pszAsciiUser = T2A((LPTSTR) pszUser);
		ASSERT(strlen(pszAsciiUser) < 100); 
		sprintf(sBuf, "USER %s\r\n", pszAsciiUser);
		int nCmdLength = strlen(sBuf);
		if (!m_Pop.Send(sBuf, nCmdLength))
		{
			if(sError){
				*sError=_T("Failed to send the USER command to server\n");
			}
			Disconnect();
			return FALSE;
		}
		if (!ReadCommandResponse())
		{
			if(sError){
				*sError=_T("Failed to read a USER command response from server\n");
			}
			Disconnect();
			return FALSE;
		} 

		//Send the POP3 password and check the response
		char* pszAsciiPassword = T2A((LPTSTR) pszPassword);
		ASSERT(strlen(pszAsciiPassword) < 100);
		sprintf(sBuf, "PASS %s\r\n", pszAsciiPassword);
		nCmdLength = strlen(sBuf);
		if (!m_Pop.Send(sBuf, nCmdLength))
		{
			if(sError){
				*sError=_T("Failed to send the PASS command to server\n");
			}
			Disconnect();
			return FALSE;
		}
		if (!ReadCommandResponse())
		{
			if(sError){
				*sError=_T("Failed to read a PASS command response from server\n");
			}
			Disconnect();
			return FALSE;
		}

		return TRUE;
	}
}

BOOL CPop3Connection::Disconnect()
{		   
	BOOL bSuccess = FALSE;		

	//disconnect from the POP3 server if connected 
	if (m_bConnected)
	{
		char sBuf[10];
		strcpy(sBuf, "QUIT\r\n");
		int nCmdLength = strlen(sBuf);
		if (!m_Pop.Send(sBuf, nCmdLength))
			TRACE(_T("Failed to send the QUIT command to the POP3 server\n"));

		//Check the reponse
		bSuccess = ReadCommandResponse();

		//Reset all the state variables
		m_bConnected = FALSE;
		m_bListRetrieved = FALSE;
		m_bStatRetrieved = FALSE;
		m_bUIDLRetrieved = FALSE;
	}
	else
		TRACE(_T("CPop3Connection, Already disconnected\n"));

	//free up our socket
	m_Pop.Close();

	return bSuccess;
}

BOOL CPop3Connection::Delete(int nMsg)
{
	BOOL bSuccess = TRUE;

	//Must be connected to perform a delete
	ASSERT(m_bConnected);

	//if we haven't executed the LIST command then do it now
	if (!m_bListRetrieved)
		bSuccess = List();

	//Handle the error if necessary  
	if (!bSuccess)
		return FALSE;

	//Send the DELE command along with the message ID
	char sBuf[20];
	sprintf(sBuf, "DELE %d\r\n", nMsg);
	int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
	{
		TRACE(_T("Failed to send the DELE command to the POP3 server\n"));
		return FALSE;
	}

	return ReadCommandResponse();
}

BOOL CPop3Connection::Statistics(int& nNumberOfMails, int& nTotalMailSize)
{
	//Must be connected to perform a "STAT"
	ASSERT(m_bConnected);

	//Send the STAT command
	char sBuf[10];
	strcpy(sBuf, "STAT\r\n");
	int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
	{
		TRACE(_T("Failed to send the STAT command to the POP3 server\n"));
		return FALSE;
	}

	return ReadStatResponse(nNumberOfMails, nTotalMailSize);
}

BOOL CPop3Connection::GetMessageSize(int nMsg, DWORD& dwSize)
{
	BOOL bSuccess = TRUE;

	//if we haven't executed the LIST command then do it now
	if (!m_bListRetrieved)
		bSuccess = List();

	//Handle the error if necessary  
	if (!bSuccess)
		return FALSE;

	//nMsg must be in the correct range
	ASSERT((nMsg > 0) && (nMsg <= m_msgSizes.GetSize()));

	//retrieve the size from the message size array
	dwSize = m_msgSizes.GetAt(nMsg - 1);

	return bSuccess;
}

BOOL CPop3Connection::GetMessageID(int nMsg, CString& sID)
{
	BOOL bSuccess = TRUE;

	//if we haven't executed the UIDL command then do it now
	if (!m_bUIDLRetrieved)
		bSuccess = UIDL();

	//Handle the error if necessary  
	if (!bSuccess)
		return FALSE;

	//nMsg must be in the correct range
	ASSERT((nMsg > 0) && (nMsg <= m_msgIDs.GetSize()));

	//retrieve the size from the message size array
	sID = m_msgIDs.GetAt(nMsg - 1);

	return bSuccess;
}

BOOL CPop3Connection::List()
{
	//Must be connected to perform a "LIST"
	ASSERT(m_bConnected);

	//if we haven't executed the STAT command then do it now
	int nNumberOfMails = m_nNumberOfMails;
	int nTotalMailSize;
	if (!m_bStatRetrieved)
	{
		if (!Statistics(nNumberOfMails, nTotalMailSize))
			return FALSE;
		else
			m_bStatRetrieved = TRUE;
	}

	//Send the LIST command
	char sBuf[10];
	strcpy(sBuf, "LIST\r\n");
	int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
	{
		TRACE(_T("Failed to send the LIST command to the POP3 server\n"));
		return FALSE;
	}
	//And check the response
	m_bListRetrieved = ReadListResponse(nNumberOfMails);
	return m_bListRetrieved;
}

BOOL CPop3Connection::UIDL()
{
	//Must be connected to perform a "UIDL"
	ASSERT(m_bConnected);

	//if we haven't executed the STAT command then do it now
	int nNumberOfMails = m_nNumberOfMails;
	int nTotalMailSize;
	if (!m_bStatRetrieved)
	{
		if (!Statistics(nNumberOfMails, nTotalMailSize))
			return FALSE;
		else
			m_bStatRetrieved = TRUE;
	}

	//Send the UIDL command
	char sBuf[10];
	strcpy(sBuf, "UIDL\r\n");
	int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
	{
		TRACE(_T("Failed to send the UIDL command to the POP3 server\n"));
		return FALSE;
	}
	//And check the response
	m_bUIDLRetrieved = ReadUIDLResponse(nNumberOfMails);
	return m_bUIDLRetrieved;
}

BOOL CPop3Connection::Reset()
{
	//Must be connected to perform a "RSET"
	ASSERT(m_bConnected);

	//Send the RSET command
	char sBuf[10];
	strcpy(sBuf, "RSET\r\n");
	int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
	{
		TRACE(_T("Failed to send the RSET command to the POP3 server\n"));
		return FALSE;
	}

	//And check the command
	return ReadCommandResponse();
}

BOOL CPop3Connection::Noop()
{
	//Must be connected to perform a "NOOP"
	ASSERT(m_bConnected);

	//Send the NOOP command
	char sBuf[10];
	strcpy(sBuf, "NOOP\r\n");
	int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
	{
		TRACE(_T("Failed to send the NOOP command to the POP3 server\n"));
		return FALSE;
	}

	//And check the response
	return ReadCommandResponse();
}

BOOL CPop3Connection::Retrieve(int nMsg, CPop3Message& message)
{
	//Must be connected to retrieve a message
	ASSERT(m_bConnected);

	//work out the size of the message to retrieve
	DWORD dwSize;
	if (GetMessageSize(nMsg, dwSize))
	{
		//Send the RETR command
		char sBuf[20];
		sprintf(sBuf, "RETR %d\r\n", nMsg); 
		int nCmdLength = strlen(sBuf);
		if (!m_Pop.Send(sBuf, nCmdLength))
		{
			TRACE(_T("Failed to send the RETR command to the POP3 server\n"));
			return FALSE;
		}

		//And check the command
		return ReadReturnResponse(message, dwSize);
	}
	else
		return FALSE;
}

BOOL CPop3Connection::GetMessageHeader(int nMsg, CPop3Message& message, int iLines, DWORD* pdwMsgSize)
{
	// Must be connected to retrieve a message
	ASSERT(m_bConnected);

	// make sure the message actually exists
	DWORD dwSize;
	if (GetMessageSize(nMsg, dwSize))
	{
		if(pdwMsgSize){
			*pdwMsgSize=dwSize;
		}
		// Send the TOP command
		char sBuf[16];
		sprintf(sBuf, "TOP %d %d\r\n", nMsg,iLines);
		int nCmdLength = strlen(sBuf);
		if (!m_Pop.Send(sBuf, nCmdLength))
		{
			TRACE(_T("Failed to send the TOP command to the POP3 server\n"));
			return FALSE;
		}

		// And check the command
		return ReadReturnResponse(message, dwSize);
	}
	else
		return FALSE;
}

BOOL CPop3Connection::ReadCommandResponse()
{
	LPSTR pszOverFlowBuffer = NULL;
	char sBuf[1000];
	BOOL bSuccess = ReadResponse(sBuf, 1000, "\r\n", &pszOverFlowBuffer);
	if (pszOverFlowBuffer)
		delete [] pszOverFlowBuffer;

	return bSuccess;
}

LPSTR CPop3Connection::GetFirstCharInResponse(LPSTR pszData) const
{
	while ((*pszData != '\n') && *pszData)
		++pszData;

	//skip over the "\n" onto the next line
	if (*pszData)
		++pszData;

	return pszData;
}

BOOL CPop3Connection::ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR pszTerminator, LPSTR* ppszOverFlowBuffer, int nGrowBy)
{
	ASSERT(ppszOverFlowBuffer); 		 //Must have a valid string pointer
	ASSERT(*ppszOverFlowBuffer == NULL); //Initially it must point to a NULL string

	//must have been created first
	ASSERT(m_bConnected);

	int nTerminatorLen = strlen(pszTerminator);

	//The local variables which will receive the data
	LPSTR pszRecvBuffer = pszBuffer;
	int nBufSize = nInitialBufSize;

	//retrieve the reponse using until we
	//get the terminator or a timeout occurs
	BOOL bFoundTerminator = FALSE;
	int nReceived = 0;
	DWORD dwStartTicks = ::GetTickCount();
	while (!bFoundTerminator)
	{
		//Has the timeout occured
		if ((::GetTickCount() - dwStartTicks) > m_dwTimeout)
		{
			pszRecvBuffer[nReceived] = '\0';
			SetLastError(WSAETIMEDOUT);
			m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			return FALSE;
		}

		//check the socket for readability
		BOOL bReadible;
		if (!m_Pop.IsReadible(bReadible))
		{
			pszRecvBuffer[nReceived] = '\0';
			m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			return FALSE;
		}
		else if (!bReadible) //no data to receive, just loop around
		{
			Sleep(250); //Sleep for a while before we loop around again
			continue;
		}

		//receive the data from the socket
		int nBufRemaining = nBufSize-nReceived-1; //Allows allow one space for the NULL terminator
		if (nBufRemaining<0)
			nBufRemaining = 0;
		int nData = m_Pop.Receive(pszRecvBuffer+nReceived, nBufRemaining);

		//Reset the idle timeout if data was received
		if (nData > 0)
		{
			dwStartTicks = ::GetTickCount();

			//Increment the count of data received
			nReceived += nData; 						   
		}

		//If an error occurred receiving the data
		if (nData < 1)
		{
			//NULL terminate the data received
			if (pszRecvBuffer)
				pszBuffer[nReceived] = '\0';

			m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			return FALSE; 
		}
		else
		{
			//NULL terminate the data received
			if (pszRecvBuffer)
				pszRecvBuffer[nReceived] = '\0';

			if (nBufRemaining-nData == 0) //No space left in the current buffer
			{
				//Allocate the new receive buffer
				nBufSize += nGrowBy; //Grow the buffer by the specified amount
				LPSTR pszNewBuf = new char[nBufSize];

				//copy the old contents over to the new buffer and assign 
				//the new buffer to the local variable used for retreiving 
				//from the socket
				if (pszRecvBuffer)
					strcpy(pszNewBuf, pszRecvBuffer);
				pszRecvBuffer = pszNewBuf;

				//delete the old buffer if it was allocated
				if (*ppszOverFlowBuffer)
					delete [] *ppszOverFlowBuffer;

				//Remember the overflow buffer for the next time around
				*ppszOverFlowBuffer = pszNewBuf;		
			}
		}

		//Check to see if the terminator character(s) have been found
		bFoundTerminator = (strncmp(&pszRecvBuffer[nReceived - nTerminatorLen], pszTerminator, nTerminatorLen) == 0);
	}

	//Remove the terminator from the response data
	pszRecvBuffer[nReceived - nTerminatorLen] = '\0';

	//determine if the response is an error
	BOOL bSuccess = (strnicmp(pszRecvBuffer,"+OK", 3) == 0);

	if (!bSuccess)
	{
		SetLastError(WSAEPROTONOSUPPORT);
		m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
	}

	return bSuccess;
}

BOOL CPop3Connection::ReadReturnResponse(CPop3Message& message, DWORD dwSize)
{
	//Must be connected to perform a "RETR"
	ASSERT(m_bConnected);

	//Retrieve the message body
	LPSTR pszOverFlowBuffer = NULL;
	int nSize = dwSize + 100;
	char* sBuf = new char[nSize];
	char* sMessageBuf = sBuf;
	if (!ReadResponse(sBuf, nSize, "\r\n.\r\n", &pszOverFlowBuffer, 32000))
	{
		delete [] sBuf;
		if (pszOverFlowBuffer)
			delete [] pszOverFlowBuffer;

		TRACE(_T("Error retrieving the RETR response"));
		return FALSE;
	}
	if (pszOverFlowBuffer)
		sMessageBuf = pszOverFlowBuffer;

	//determine if the response is an error
	if (strnicmp(sMessageBuf,"+OK", 3) != 0)
	{
		delete [] sBuf;
		if (pszOverFlowBuffer)
			delete [] pszOverFlowBuffer;

		SetLastError(WSAEPROTONOSUPPORT);
		TRACE(_T("POP3 server did not respond correctly to the RETR response\n"));
		return FALSE;
	}
	else
	{  
		//remove the first line which contains the +OK from the message
		char* pszFirst = GetFirstCharInResponse(sMessageBuf);
		VERIFY(pszFirst);

		//transfer the message contents to the message class
		int nMessageSize = sMessageBuf - pszFirst + strlen(sMessageBuf);

		// Do we already have memory allocated? If so, destroy it!
		if (message.m_pszMessage)
		{
			delete [] message.m_pszMessage;
			message.m_pszMessage = NULL;
		}

		message.m_pszMessage = new char[nMessageSize + 1];
		memcpy(message.m_pszMessage, pszFirst, nMessageSize);
		message.m_pszMessage[nMessageSize] = '\0';
	}
	delete [] sBuf;
	if (pszOverFlowBuffer)
		delete [] pszOverFlowBuffer;

	return TRUE; 
}

BOOL CPop3Connection::ReadListResponse(int nNumberOfMails)
{
	//Must be connected to perform a "LIST"
	ASSERT(m_bConnected);

	//retrieve the reponse
	LPSTR pszOverFlowBuffer = NULL;
	int nSize = 14 * nNumberOfMails + 100;
	char* sBuf = new char[nSize];
	char* sMessageBuf = sBuf;
	if (!ReadResponse(sBuf, nSize, "\r\n.\r\n", &pszOverFlowBuffer))
	{
		delete [] sBuf;
		if (pszOverFlowBuffer)
			delete [] pszOverFlowBuffer;

		TRACE(_T("Error retrieving the LIST response from the POP3 server"));
		return FALSE;
	}
	if (pszOverFlowBuffer)
		sMessageBuf = pszOverFlowBuffer;

	//determine if the response is an error
	if (strnicmp(sMessageBuf,"+OK", 3) != 0)
	{
		delete [] sBuf;
		if (pszOverFlowBuffer)
			delete [] pszOverFlowBuffer;

		SetLastError(WSAEPROTONOSUPPORT);
		TRACE(_T("POP3 server did not respond correctly to the LIST response\n"));
		return FALSE;
	}
	else
	{  
		//Retrieve the message sizes and put them
		//into the m_msgSizes array
		m_msgSizes.RemoveAll();
		m_msgSizes.SetSize(0, nNumberOfMails);

		//then parse the LIST response
		char* pszSize = GetFirstCharInResponse(sMessageBuf);
		VERIFY(pszSize);
		for (; *pszSize != '.'; pszSize++)
			if (*pszSize == '\t' || *pszSize == ' ')
				m_msgSizes.Add(atoi(pszSize));
	}
	delete [] sBuf;
	if (pszOverFlowBuffer)
		delete [] pszOverFlowBuffer;

	return TRUE; 
}

BOOL CPop3Connection::ReadUIDLResponse(int nNumberOfMails)
{
	//Must be connected to perform a "UIDL"
	ASSERT(m_bConnected);

	//retrieve the reponse
	LPSTR pszOverFlowBuffer = NULL;
	int nSize = 14 * nNumberOfMails + 100;
	char* sBuf = new char[nSize];
	char* sMessageBuf = sBuf;
	if (!ReadResponse(sBuf, nSize, "\r\n.\r\n", &pszOverFlowBuffer))
	{
		delete [] sBuf;
		if (pszOverFlowBuffer)
			delete [] pszOverFlowBuffer;

		TRACE(_T("Error retrieving the UIDL response from the POP3 server"));
		return FALSE;
	}
	if (pszOverFlowBuffer)
		sMessageBuf = pszOverFlowBuffer;

	//determine if the response is an error
	if (strnicmp(sMessageBuf,"+OK", 3) != 0)
	{
		delete [] sBuf;
		if (pszOverFlowBuffer)
			delete [] pszOverFlowBuffer;

		SetLastError(WSAEPROTONOSUPPORT);
		TRACE(_T("POP3 server did not respond correctly to the UIDL response\n"));
		return FALSE;
	}
	else
	{  
		//Retrieve the message ID's and put them
		//into the m_msgIDs array
		m_msgIDs.RemoveAll();
		m_msgIDs.SetSize(0, nNumberOfMails);

		//then parse the UIDL response
		char* pszSize = GetFirstCharInResponse(sMessageBuf);

		VERIFY(pszSize);
		for (int iCount=0; iCount<nNumberOfMails; iCount++)
		{
			char* pszBegin = pszSize;
			while (*pszSize != '\n' && *pszSize != '\0')
				++pszSize;

			char sMsg[15];
			char sID[1000];
			*pszSize = '\0';
			sscanf(pszBegin, "%s %s", sMsg, sID);

			m_msgIDs.Add(CString(sID));
			pszSize++;
		}
	}
	delete [] sBuf;
	if (pszOverFlowBuffer)
		delete [] pszOverFlowBuffer;

	return TRUE; 
}

BOOL CPop3Connection::ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize)
{
	//Must be connected to perform a "STAT"
	ASSERT(m_bConnected);

	//retrieve the reponse
	LPSTR pszOverFlowBuffer = NULL;
	char sBuf[100];
	char* sMessageBuf = sBuf;
	if (!ReadResponse(sBuf, 100, "\r\n", &pszOverFlowBuffer))
	{
		if (pszOverFlowBuffer)
			delete [] pszOverFlowBuffer;

		TRACE(_T("Error retrieving the STAT response from the POP3 server"));
		return FALSE;
	}
	if (pszOverFlowBuffer)
		sMessageBuf = pszOverFlowBuffer;

	//determine if the response is an error
	if (strncmp(sMessageBuf,"+OK", 3) != 0)
	{
		TRACE(_T("POP3 server did not respond correctly to the STAT response\n"));
		return FALSE;
	}
	else
	{										   
		//Parse out the Number of Mails and Total mail size values
		BOOL bGetNumber = TRUE;
		for (char* pszNum=sMessageBuf; *pszNum!='\0'; pszNum++)
		{
			if (*pszNum=='\t' || *pszNum==' ')
			{						
				if (bGetNumber)
				{
					nNumberOfMails = atoi(pszNum);
					m_nNumberOfMails = nNumberOfMails;
					bGetNumber = FALSE;
				}
				else
				{
					nTotalMailSize = atoi(pszNum);
					return TRUE;
				}
			}
		}
	}
	if (pszOverFlowBuffer)
		delete [] pszOverFlowBuffer;

	return FALSE; 
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
CString DecodeHeaderValue(CString Encoded, CString *pCharset)
{
	Encoded.TrimLeft();
	Encoded.TrimRight();
	CString Result;
	int i = 0, len = Encoded.GetLength();
	while(i < len)
	{
		if (Encoded[i] == 61 // Equal sign (=)
			&& (i<len-1 && Encoded[i+1] == _T('?')))
		{ // this is a encoded_word start.
			int ce = Encoded.Find(_T('?'), i+2);
			if (ce > 0)
			{
				if (pCharset) *pCharset = Encoded.Mid(i+2, ce-i-2);
				// for now we do not care about this field;
				// scan for the next '?'

				int ee=Encoded.Find(_T('?'), ce+1);
				if (ee > 0)
				{
					CString word_encoding = Encoded.Mid(ce+1, ee-ce-1);
					word_encoding.MakeUpper();
					int we = Encoded.Find(_T('?'), ee+1);
					if (we > 0)
					{
						CString encoded_word = Encoded.Mid(ee+1, we-ee-1);
						CMIMECode *pDecoder = NULL;
						if (word_encoding == "Q")
							pDecoder = new CQuoted;
						if (word_encoding == "B")
							pDecoder = new CBase64;
						Result+= pDecoder ? pDecoder->Decode(encoded_word, we-ee-1) 
							: encoded_word;
						delete pDecoder; // it's safe to call delete on NULL pointers (ANSI)
						i=we+2;
						continue; // The word succesfully decoded;
					}
				}
			}
		}
		// Here we fall in case of any error
		Result+=Encoded[i++]; // Simply skip one char;
	}
	return Result;
}

// The 7-bit alphabet used to encode binary information
CString CBase64::m_sBase64Alphabet = 
_T( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" );

int CBase64::m_nMask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

CString CBase64::Encode(LPCTSTR szEncoding, int nSize)
{
	CString sOutput = _T( "" );
	int nNumBits = 6;
	UINT nDigit;
	int lp = 0;

	ASSERT( szEncoding != NULL );
	if( szEncoding == NULL )
		return sOutput;
	m_szInput = szEncoding;
	m_nInputSize = nSize;

	m_nBitsRemaining = 0;
	nDigit = read_bits( nNumBits, &nNumBits, lp );
	while( nNumBits > 0 )
	{
		sOutput += m_sBase64Alphabet[ (int)nDigit ];
		nDigit = read_bits( nNumBits, &nNumBits, lp );
	}
	// Pad with '=' as per RFC 1521
	while( sOutput.GetLength() % 4 != 0 )
	{
		sOutput += '=';
	}
	return sOutput;
}

// The size of the output buffer must not be less than
// 3/4 the size of the input buffer. For simplicity,
// make them the same size.
CString CBase64::Decode(LPCTSTR szDecoding, int nSize)
{
	CString sInput, Result;
	int c, lp =0;
	int nDigit;
	int nDecode[ 256 ];
	if(nSize==-1){
		nSize=strlen(szDecoding)+1;
	}
	LPTSTR szOutput = Result.GetBuffer(nSize); // Output is never longer than input

	ASSERT( szDecoding != NULL );
	if( szDecoding == NULL )
		return _T("");
	sInput = szDecoding;
	if( sInput.GetLength() == 0 )
		return _T("");
	m_lBitStorage = 0;
	m_nBitsRemaining = 0;

	// Build Decode Table
	//
	for( int i = 0; i < 256; i++ ) 
		nDecode[i] = -2; // Illegal digit
	for( i=0; i < 64; i++ )
	{
		nDecode[ m_sBase64Alphabet[ i ] ] = i;
		nDecode[ m_sBase64Alphabet[ i ] | 0x80 ] = i; // Ignore 8th bit
		nDecode[ '=' ] = -1; 
		nDecode[ '=' | 0x80 ] = -1; // Ignore MIME padding char
		nDecode[ '\n'] = -1; // Skip the CR/LFs
		nDecode[ '\r'] = -1;
	}

	// Decode the Input
	//
	for( lp = 0, i = 0; lp < sInput.GetLength(); lp++ )
	{
		c = sInput[ lp ];
		nDigit = nDecode[ c & 0x7F ];
		if( nDigit < -1 ) 
		{
			return _T("");
		} 
		else if( nDigit >= 0 ) 
			// i (index into output) is incremented by write_bits()
			write_bits( nDigit & 0x3F, 6, szOutput, i );
	}    
	Result.ReleaseBuffer(i);
	return Result;
}

UINT CBase64::read_bits(int nNumBits, int * pBitsRead, int& lp)
{
	ULONG lScratch;
	while( ( m_nBitsRemaining < nNumBits ) && 
		( lp < m_nInputSize ) ) 
	{
		int c = m_szInput[ lp++ ];
		m_lBitStorage <<= 8;
		m_lBitStorage |= (c & 0xff);
		m_nBitsRemaining += 8;
	}
	if( m_nBitsRemaining < nNumBits ) 
	{
		lScratch = m_lBitStorage << ( nNumBits - m_nBitsRemaining );
		*pBitsRead = m_nBitsRemaining;
		m_nBitsRemaining = 0;
	} 
	else 
	{
		lScratch = m_lBitStorage >> ( m_nBitsRemaining - nNumBits );
		*pBitsRead = nNumBits;
		m_nBitsRemaining -= nNumBits;
	}
	return (UINT)lScratch & m_nMask[nNumBits];
}


void CBase64::write_bits(UINT nBits,
						 int nNumBits,
						 LPTSTR szOutput,
						 int& i)
{
	UINT nScratch;

	m_lBitStorage = (m_lBitStorage << nNumBits) | nBits;
	m_nBitsRemaining += nNumBits;
	while( m_nBitsRemaining > 7 ) 
	{
		nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
		szOutput[ i++ ] = nScratch & 0xFF;
		m_nBitsRemaining -= 8;
	}
}

const CString CQuoted::code = _T("0123456789ABCDEF");
const CString CQuoted::extended_code = _T("0123456789abcdef");
CQuoted::CQuoted(int linelength):m_linelength(linelength)
{

}

CQuoted::~CQuoted()
{

}

CString CQuoted::Encode( LPCTSTR szEncoding, int nSize )
{
	CString Result;
	if (nSize < 0)
		nSize = strlen(szEncoding);
	// Estimate the buffer size;
	// we will need max 3 bytes for each incoming octet 
	// and 3 additional bytes for each line
	LPTSTR buffer = Result.GetBuffer(3*(nSize+3*nSize/(m_linelength))); 
	int chunklen;
	int bufpos=0, linepos=0;
	for (int i = 0; i<nSize; i++)
	{
		// test if the szEncoding[i] allows literal representation
		chunklen=((szEncoding[i]>32 && szEncoding[i]<=60) ||
			(szEncoding[i]>61 && szEncoding[i]<=126)) ? 1 : 3;
		// test if line break needed
		if (linepos+chunklen>m_linelength-1)
		{ // insert linebreak
			buffer[bufpos++]= 61; // Equal sign
			buffer[bufpos++]= 13; // Line 
			buffer[bufpos++]= 10; //  break
			linepos=0;
		}
		if (chunklen == 1)
		{
			buffer[bufpos++] = szEncoding[i]; // literal
			linepos++;
		}
		else
		{
			buffer[bufpos++]= 61; // Equal sign
			buffer[bufpos++]= code[(szEncoding[i] >> 4) & 0xF]; //high nibble
			buffer[bufpos++]= code[szEncoding[i] & 0xF]; //  low nibble
			linepos+=3;
		}
	}
	Result.ReleaseBuffer(bufpos);
	return Result;
}

CString CQuoted::Decode( LPCTSTR szDecoding, int nSize)
{
	CString Result;
	if (nSize < 0)
		nSize = strlen(szDecoding);
	// Guess the buffer size
	LPTSTR szOutput = Result.GetBuffer(nSize);
	int i=0, outpos=0;
	while(szDecoding[i])
	{
		if (szDecoding[i] == 61) // Equal Sign
		{ // try to decode the following chunk
			int octet;
			if (sscanf(szDecoding+i+1, _T("%2x"), &octet)>0)
			{
				szOutput[outpos++]=octet;
				i+=3;
				continue;
			}
			else 
			{
				// is this a CR/LF?
				if (szDecoding[i+1] == 13)
				{
					i+=2; // simply skip the CR...
					if (szDecoding[i] == 10) i++; //...and, probably, LF
					continue;
				}
			}
		}
		// decoding error occured! try to recover by outputting the 
		// untransformed octet and starting over from the very beginning;
		szOutput[outpos++] = szDecoding[i++];
	}
	Result.ReleaseBuffer(outpos);
	return Result;
}

BOOL CCharsetDecoder::isKnown()
{
	bool bRes=FALSE;
	Recode("abcABC", false, &bRes);
	return bRes;
}

CString CCharsetDecoder::Decode(CString sString)
{
	return Recode(sString, false);
}

CString CCharsetDecoder::Encode(CString sString)
{
	return Recode(sString, true);
}

CString CCharsetDecoder::Recode(CString sString, bool bEncode, bool* bRes)
{
	if(m_sCharSetName.CompareNoCase("7bit")==0 || m_sCharSetName.CompareNoCase("8bit")==0){
		if(bRes){
			*bRes=TRUE;
		}
		return sString;
	}
	if(m_sCharSetName.CompareNoCase("quoted-printable")==0){
		if(bRes){
			*bRes=TRUE;
		}
		CQuoted objDecoder;
		CString sRes=objDecoder.Decode(sString, -1) ;
		return sRes;
	}
	if(m_sCharSetName.CompareNoCase("base64")==0){
		if(bRes){
			*bRes=TRUE;
		}
		CBase64 objDecoder;
		CString sRes=objDecoder.Decode(sString, -1) ;
		return sRes;
	}
	if(m_sCharSetName.CompareNoCase("UTF-8")==0){
		if(bRes){
			*bRes=TRUE;
		}
		// Это мы сами!
		if(bEncode){
			USES_CONVERSION;
			return Str2UTF8(A2W(sString));
		}else{
			return UTF82Str(sString);
		}
	}
	CString sResult;
	HRESULT hr;
	CComPtr<IMultiLanguage> pLang;
	hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (LPVOID*)&pLang);
	if (hr == S_OK)
	{
		CComBSTR bsCharsetName = m_sCharSetName; // all required conversions are done here;
		MIMECSETINFO info;
		hr = pLang->GetCharsetInfo(bsCharsetName, &info);
		if (hr == S_OK)
		{
			DWORD dwMode=0;
			UINT SrcSize=sString.GetLength(), DstSize=SrcSize;
			hr = bEncode ? 
				pLang->ConvertString(&dwMode, info.uiCodePage, 
				info.uiInternetEncoding, (BYTE*)LPCSTR(sString), &SrcSize, 
				(BYTE *)sResult.GetBuffer(DstSize), &DstSize)
				:
			pLang->ConvertString(&dwMode, info.uiInternetEncoding, 
				info.uiCodePage, (BYTE*)LPCSTR(sString), &SrcSize, 
				(BYTE *)sResult.GetBuffer(DstSize), &DstSize);
			if (hr == S_OK){
				sResult.ReleaseBuffer(DstSize);
				if(bRes){
					*bRes=TRUE;
				}
				return sResult;
			}
		}
	}
	if(bRes){
		*bRes=FALSE;
	}
	return sString;
}

CCharsetDecoder::CCharsetDecoder(UINT CodePage)
{
	CComPtr<IMultiLanguage> pLang;
	HRESULT hr;
	hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (LPVOID*)&pLang);
	if (hr == S_OK)
	{
		//CComBSTR bsCharsetName = m_sCharSetName; // all required conversions are done here;
		MIMECPINFO info;
		hr = pLang->GetCodePageInfo(CodePage, &info);
		if (hr == S_OK)
		{
			m_sCharSetName = info.wszHeaderCharset;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIMECode::CMIMECode()
{

}

CMIMECode::~CMIMECode()
{

}
//-------------------------------------------------------------------------- 
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFastSmtp::CFastSmtp()
{
	// Initialize variables
	bCon            = false;

	m_pcFromEmail    = NULL;
	m_pcFromName    = NULL;
	m_pcSubject        = NULL;
	m_pcMsgBody        = NULL;
	m_pcXMailer        = NULL;
	m_pcReplyTo        = NULL;
	m_pcIPAddr        = NULL;

	// Initialize WinSock
	WORD wVer    = MAKEWORD(2,2);    
	if (WSAStartup(wVer,&wsaData) != NO_ERROR) {
		printf("WSAStartup %d\r\n", WSAGetLastError());        
		throw; 
	}
	if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) {
		printf("Can't find a useable WinSock DLL\r\n");
		WSACleanup();
		throw; 
	}    
}

CFastSmtp::~CFastSmtp()
{
	// Free recipient lists
	long n=0;
	for (n = 0; n < Recipients.GetSize(); n++)
		delete Recipients[n];
	for (n = 0; n < CCRecipients.GetSize(); n++)
		delete CCRecipients[n];
	for (n = 0; n < BCCRecipients.GetSize(); n++)
		delete BCCRecipients[n];

	// Free variables
	if (m_pcFromEmail)
		delete m_pcFromEmail;
	if (m_pcFromName)
		delete m_pcFromName;
	if (m_pcSubject)
		delete m_pcSubject;
	if (m_pcMsgBody)
		delete m_pcMsgBody;
	if (m_pcXMailer)
		delete m_pcXMailer;
	if (m_pcReplyTo)
		delete m_pcReplyTo;

	// Close connection
	if (bCon)
		Disconnect();

	// Cleanup
	WSACleanup();
}

bool CFastSmtp::AddRecipient(const char email[], const char name[])
{
	int s=strlen(email);
	if (s==0)
		return false;

	CRecipient *pRec = new CRecipient();

	char *pcBuf = new char[s+strlen(name)+4];
	sprintf(pcBuf,"%s<%s>",name,email);    
	pRec->SetEmail(pcBuf);
	Recipients.Add(pRec);
	delete pcBuf;

	return (true);    
}

bool CFastSmtp::AddCCRecipient(const char email[], const char name[])
{
	int s=strlen(email);
	if (s==0)
		return false;

	CRecipient *pRec = new CRecipient();    

	char *pcBuf = new char[s+strlen(name)+4];
	sprintf(pcBuf,"%s<%s>",name,email);    
	pRec->SetEmail(pcBuf);
	CCRecipients.Add(pRec);
	delete pcBuf;

	return (true);
}

bool CFastSmtp::AddBCCRecipient(const char email[], const char name[])
{
	int s=strlen(email);
	if (s==0)
		return false;

	CRecipient *pRec = new CRecipient();

	char *pcBuf = new char[s+strlen(name)+4];
	sprintf(pcBuf,"%s<%s>",name,email);    
	pRec->SetEmail(pcBuf);
	BCCRecipients.Add(pRec);
	delete pcBuf;    

	return (true);
}

bool CFastSmtp::Disconnect()
{
	if (!bCon) {
		printf("Not connected to server!\r\n");
		return (false);
	}

	BYTE        sReceiveBuffer[4096];
	int            iLength = 0;
	int            iEnd = 0;

	if (send(hSocket, (LPSTR)"QUIT\r\n", strlen("QUIT\r\n"),
		NO_FLAGS) == SOCKET_ERROR) {
			printf("Socket send error: %d\r\n", WSAGetLastError());    
			return (false);
		}
		iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd, NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';

		bCon=false;    
		shutdown(hSocket,0x02);
		closesocket(hSocket);	
		hSocket=NULL;
		return (true);
}

BOOL BadAnswer(const BYTE* sReceiveBufferBt,CString& sErr)
{
	CString sReceiveBuffer=(const char*)sReceiveBufferBt;
	sReceiveBuffer.Replace("\r","");
	sReceiveBuffer=CString("\n")+sReceiveBuffer;
	if(strstr(sReceiveBuffer,"450 ")!=0 || strstr(sReceiveBuffer,"451 ")!=0 || strstr(sReceiveBuffer,"452 ")!=0
		|| strstr(sReceiveBuffer,"521 ")!=0 || strstr(sReceiveBuffer,"530 ")!=0 || strstr(sReceiveBuffer,"550 ")!=0
		|| strstr(sReceiveBuffer,"552 ")!=0 || strstr(sReceiveBuffer,"553 ")!=0 || strstr(sReceiveBuffer,"554 ")!=0){
			sErr=sReceiveBuffer;
			return (true);
		}
		return (false);
}


bool CFastSmtp::Send(CString& sErr)
{
	// verify sender email
	if (m_pcFromEmail == NULL) {
		sErr=("Please specifiy a sender email address\r\n");
		return (false);
	}

	BYTE        sReceiveBuffer[4096];
	int         iLength = 0;
	int         iEnd = 0;
	char        buf[4096];
	// get proper header
	CString msgHeader = _formatHeader();

	if (msgHeader == "") {
		sErr = "Bad headers";
		return (false);
	}
	if(GetUserName().GetLength()>0 && GetPassward().GetLength()>0){// start - user
		memset(buf,0,sizeof(buf));
		strcpy(buf, "AUTH LOGIN");
		strcat(buf, "\r\n");
		if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
			sErr.Format("Socket send error: %d\r\n", WSAGetLastError()); 
			return (false);
		}
		iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd, 
			NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';
		if(BadAnswer(sReceiveBuffer,sErr)){
			return false;
		}
		char base64buf[1024]={0};
		memset( base64buf, 0, sizeof(base64buf) );
		b64encode( GetUserName().GetBuffer(100), base64buf, GetUserName().GetLength(), 4 );
		memset(buf,0,sizeof(buf));
		strcpy(buf, base64buf);
		strcat(buf, "\r\n");
		if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
			sErr.Format("Socket send error: %d\r\n", WSAGetLastError()); 
			return (false);
		}
		iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd, 
			NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';
		if(BadAnswer(sReceiveBuffer,sErr)){
			return false;
		}
		// password
		memset( base64buf, 0, sizeof(base64buf) );
		b64encode( GetPassward().GetBuffer(100), base64buf, GetPassward().GetLength(), 4 );
		memset(buf,0,sizeof(buf));
		strcpy(buf, base64buf);
		strcat(buf, "\r\n");
		if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
			sErr.Format("Socket send error: %d\r\n", WSAGetLastError()); 
			return (false);
		}
		iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd, 
			NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';
		if(BadAnswer(sReceiveBuffer,sErr)){
			return false;
		}
	}
	// start
	memset(buf,0,sizeof(buf));
	strcpy(buf, "MAIL FROM: <");
	strcat(buf, m_pcFromEmail);
	strcat(buf, ">\r\n");
	if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
		sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
		return (false);
	}
	iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd, 
		NO_FLAGS);
	iEnd += iLength;
	sReceiveBuffer[iEnd] = '\0';
	if(BadAnswer(sReceiveBuffer,sErr)){
		return false;
	}
	// create message receipts
	int i=0;
	char *token;
	for(i=0;i<Recipients.GetSize();i++) {
		token = strtok(Recipients.GetAt(i)->GetEmail(),"<");
		token = strtok(NULL,"<");    
		if (token == NULL){
			token = strtok(Recipients.GetAt(i)->GetEmail(),"<");
		}
		memset(buf,0,sizeof(buf));
		strcpy(buf, "RCPT TO: <");
		strcat(buf, token);
		strcat(buf, "\r\n");
		if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
			sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
			return (false);
		}
		iLength = recv(hSocket, 
			(LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd, 
			NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';
		if(BadAnswer(sReceiveBuffer,sErr)){
			return false;
		}
	}
	for(i=0;i<CCRecipients.GetSize();i++) {
		token = strtok(CCRecipients.GetAt(i)->GetEmail(),"<");
		token = strtok(NULL,"<");
		if (token == NULL) {
			token = strtok(Recipients.GetAt(i)->GetEmail(),"<");
		}
		memset(buf,0,sizeof(buf));
		strcpy(buf, "RCPT TO: <");
		strcat(buf, token);
		strcat(buf, "\r\n");
		if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
			sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
			return (false);
		}
		iLength = recv(hSocket, 
			(LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd,
			NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';
		if(BadAnswer(sReceiveBuffer,sErr)){
			return false;
		}
	}
	for(i=0;i<BCCRecipients.GetSize();i++) {
		token = strtok(BCCRecipients.GetAt(i)->GetEmail(),"<");
		token = strtok(NULL,"<");
		if (token == NULL){
			token = strtok(Recipients.GetAt(i)->GetEmail(),"<");
		}
		memset(buf,0,sizeof(buf));
		strcpy(buf, "RCPT TO: <");
		strcat(buf, token);
		strcat(buf, "\r\n");
		if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
			sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
			return (false);
		}
		iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,
			sizeof(sReceiveBuffer)-iEnd, 
			NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';
		if(BadAnswer(sReceiveBuffer,sErr)){
			return false;
		}
	}
	// init data
	if (send(hSocket, (LPSTR)"DATA\r\n", strlen("DATA\r\n"), NO_FLAGS) == SOCKET_ERROR) {
		sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
		return (false);
	}
	iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd,
		NO_FLAGS);
	iEnd += iLength;
	sReceiveBuffer[iEnd] = '\0';
	if(BadAnswer(sReceiveBuffer,sErr)){
		return false;
	}
	// send header    
	if (send(hSocket, msgHeader, strlen(msgHeader), NO_FLAGS) == SOCKET_ERROR) {
		sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
		return (false);
	}
	// send body
	{
		USES_CONVERSION;
		CQuoted quoter;
		CString sUTF8Body=Str2UTF8(A2W(m_pcMsgBody));
		sUTF8Body=quoter.Encode(sUTF8Body);
		if (send(hSocket, (LPSTR)sUTF8Body.GetBuffer(sUTF8Body.GetLength()+1), sUTF8Body.GetLength(), NO_FLAGS) == SOCKET_ERROR) {
			sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
			return (false);
		}
	}
	// signal end    
	if (send(hSocket,(LPSTR)"\r\n.\r\n", strlen("\r\n.\r\n"), NO_FLAGS) == SOCKET_ERROR) {
		sErr.Format("Socket send error: %d\r\n", WSAGetLastError());    
		return (false);
	}
	iLength = recv(hSocket, (LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd,
		NO_FLAGS);
	iEnd += iLength;
	sReceiveBuffer[iEnd] = '\0';
	if(BadAnswer(sReceiveBuffer,sErr)){
		return false;
	}
	return (true);
}

bool CFastSmtp::ConnectServer(const char server[], const unsigned short port)
{
	if (bCon)
		Disconnect();
	bCon=false;
	hSocket = INVALID_SOCKET;

	hSocket = _connectServerSocket(server, port);    
	if (hSocket != INVALID_SOCKET) {        
		BYTE        sReceiveBuffer[4096];
		int            iLength = 0;
		int            iEnd = 0;
		char        buf[4096];
		memset(buf,0,sizeof(buf));
		if(GetUserName().GetLength()==0){
			strcpy(buf, "HELO ");
		}else{
			strcpy(buf, "EHLO ");
		}
		strcat(buf, server);
		strcat(buf, "\r\n");
		if (send(hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS) == SOCKET_ERROR) {
			printf("Socket send error: %d\r\n", WSAGetLastError());    
			return (false);
		}
		iLength = recv(hSocket, 
			(LPSTR)sReceiveBuffer+iEnd,sizeof(sReceiveBuffer)-iEnd, 
			NO_FLAGS);
		iEnd += iLength;
		sReceiveBuffer[iEnd] = '\0';
	} else {
		printf("Invalid socket\r\n");
		return (false);
	}

	bCon=true;
	return (true);
}

SOCKET CFastSmtp::_connectServerSocket(const char server[],
									   const unsigned short port)
{
	int                nConnect;
	short            nProtocolPort;
	LPHOSTENT        lpHostEnt;
	LPSERVENT        lpServEnt;
	SOCKADDR_IN        sockAddr;        

	SOCKET            hServerSocket = INVALID_SOCKET;

	lpHostEnt = gethostbyname(server);
	if (lpHostEnt) {        
		hServerSocket = socket(PF_INET, SOCK_STREAM,DEFAULT_PROTOCOL);
		if (hServerSocket != INVALID_SOCKET) {
			if (port != NULL) {
				nProtocolPort = port;
			}else{
				lpServEnt = getservbyname("mail", DEFAULT_PROTOCOL);
				if (lpServEnt == NULL) 
					nProtocolPort = htons(IPPORT_SMTP); 
				else 
					nProtocolPort = lpServEnt->s_port;
			}
			sockAddr.sin_family = AF_INET;
			sockAddr.sin_port = htons((u_short)nProtocolPort);
			sockAddr.sin_addr = *((LPIN_ADDR)*lpHostEnt->h_addr_list);
			nConnect = connect(hServerSocket, (PSOCKADDR)&sockAddr, 
				sizeof(sockAddr));
			if(nConnect) 
				hServerSocket = INVALID_SOCKET;
			DWORD dwErr=GetLastError();
		} else {
			printf("Invalid socket\r\n");
			throw;
		}
	}
	return(hServerSocket);
}

#define         MASKBITS                0x3F
#define         MASKBYTE                0x80
#define         MASK2BYTES              0xC0
#define         MASK3BYTES              0xE0
#define         MASK4BYTES              0xF0
#define         MASK5BYTES              0xF8
#define         MASK6BYTES              0xFC
CString Str2UTF8(const WCHAR* input)
{
	CString output;
	int i=0;
	while(input[i])
	{
		// 0xxxxxxx
		DWORD dw=(DWORD)input[i];
		if(dw < 0x80)
		{
			output+=(char)((byte)input[i]);
		}
		// 110xxxxx 10xxxxxx
		else if(dw < 0x800)
		{
			output+=(char)((byte)(MASK2BYTES | dw >> 6));
			output+=(char)((byte)(MASKBYTE | dw & MASKBITS));
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if(dw < 0x10000)
		{
			output+=(char)((byte)(MASK3BYTES | dw >> 12));
			output+=(char)((byte)(MASKBYTE | dw >> 6 & MASKBITS));
			output+=(char)((byte)(MASKBYTE | dw & MASKBITS));
		}
		i++;
	}
	return output;
}

CString UTF82Str(const char* input)
{
	CString output;
	for(int i=0; input[i]!=0;)
	{
		WCHAR ch[3]={0};
		// 1110xxxx 10xxxxxx 10xxxxxx
		if((input[i] & MASK3BYTES) == MASK3BYTES)
		{
			ch[0] = ((input[i] & 0x0F) << 12) | (
				(input[i+1] & MASKBITS) << 6)
				| (input[i+2] & MASKBITS);
			i += 3;
		}
		// 110xxxxx 10xxxxxx
		else if((input[i] & MASK2BYTES) == MASK2BYTES)
		{
			ch[0] = ((input[i] & 0x1F) << 6) | (input[i+1] & MASKBITS);
			i += 2;
		}
		// 0xxxxxxx
		else if(input[i] < MASKBYTE)
		{
			ch[0] = input[i];
			i += 1;
		}
		{
			USES_CONVERSION;
			output+=W2A(ch);
		}
	}
	return output;
}

CString GetUTFEncoded(const char* szIn,char cSplitOnRus=0)
{
	if(szIn==0 || *szIn==0){
		return "";
	}
	CString s=szIn;
	s=s.Left(1000);
	CString sPostfix="";
	if(cSplitOnRus!=0){
		int iPos=s.Find(cSplitOnRus);
		if(iPos>0){
			sPostfix=s.Mid(iPos);
			s=s.Left(iPos);
		}else{
			// Не нашли разделителся, возвращаем адрес незашифрованным
			return szIn;
		}
	}
	USES_CONVERSION;
	CString sUTF8=Str2UTF8(A2W(s));
	CString sUTF8Sub="=?UTF-8?B?";
	char base64buf[4024];
	memset( base64buf, 0, sizeof(base64buf) );
	b64encode( sUTF8, base64buf, sUTF8.GetLength(), 0 );
	sUTF8Sub+=base64buf;
	sUTF8Sub+="?=";
	if(sPostfix!=""){
		sPostfix=" "+sPostfix;
	}
	return sUTF8Sub+sPostfix;
}

CString CFastSmtp::_formatHeader()
{
	// check for at least one recipient
	if(Recipients.GetSize() <= 0 )    {
		return "";
	}
	int s=0;
	int i=0;
	char *msgHeader = new char[16385];
	for (i=s=0;i<Recipients.GetSize();i++) {        
		s+=strlen(Recipients.GetAt(i)->GetEmail())+1;
	} if (s==0) s=1; char *to = new char[s*4];
	//char cc[1024];
	for (i=s=0;i<CCRecipients.GetSize();i++) {        
		s+=strlen(CCRecipients.GetAt(i)->GetEmail())+1;
	} if (s==0) s=1; char *cc = new char[s*4];
	//char bcc[1024];
	for (i=s=0;i<BCCRecipients.GetSize();i++) {        
		s+=strlen(BCCRecipients.GetAt(i)->GetEmail())+1;
	} if (s==0) s=1; char *bcc = new char[s*4];

	TCHAR szDate[500]={0};
	TCHAR sztTime[500]={0};

	// create the recipient string, cc string, and bcc string
	to[0] = '\0';        
	for (i=0;i<Recipients.GetSize();i++) {        
		i > 0 ? strcat(to,","):strcat(to,"");
		strcat(to,GetUTFEncoded(Recipients.GetAt(i)->GetEmail(),'<'));
	}

	cc[0] = '\0';    
	for (i=0;i<CCRecipients.GetSize();i++) {
		i > 0 ? strcat(cc,","):strcat(cc,"");
		strcat(cc,GetUTFEncoded(CCRecipients.GetAt(i)->GetEmail(),'<'));
	}

	bcc[0] = '\0';    
	for (i=0;i<BCCRecipients.GetSize();i++) {
		i > 0 ? strcat(bcc,","):strcat(bcc,"");
		strcat(bcc,GetUTFEncoded(BCCRecipients.GetAt(i)->GetEmail(),'<'));
	}
	// get the current date and time
	SYSTEMTIME st={0};
	::GetSystemTime(&st);
	::GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"ddd, dd MMM yyyy",szDate,sizeof(szDate));
	::GetTimeFormat(LOCALE_SYSTEM_DEFAULT,TIME_FORCE24HOURFORMAT,&st, "HH:mm:ss tt",sztTime,sizeof(sztTime));
	// here it is...the main data of the message
	wsprintf(msgHeader,"DATE: %s %s\r\n", szDate, sztTime);    
	if (m_pcFromName != NULL) {
		strcat(msgHeader,"FROM: ");
		strcat(msgHeader, GetUTFEncoded(m_pcFromName,'<'));
		strcat(msgHeader, "\r\n");
	}
	strcat(msgHeader,"To: ");
	strcat(msgHeader, to);
	strcat(msgHeader, "\r\n");
	strcat(msgHeader,"Cc: ");
	strcat(msgHeader, cc);
	strcat(msgHeader, "\r\n");
	if (m_pcSubject != NULL) {
		strcat(msgHeader, "Subject: ");
		strcat(msgHeader, GetUTFEncoded(m_pcSubject));
		strcat(msgHeader, "\r\n");
	}
	if (m_pcXMailer != NULL) {
		strcat(msgHeader,"X-Mailer: ");
		strcat(msgHeader, m_pcXMailer);
		strcat(msgHeader, "\r\n");
	}
	// start optional fields
	if (m_pcReplyTo != NULL) {
		strcat(msgHeader, "Reply-To: ");
		strcat(msgHeader, GetUTFEncoded(m_pcReplyTo,'<'));
		strcat(msgHeader, "\r\n");
	}
	// start MIME versions
	strcat(msgHeader, "MIME-Version: 1.0\r\nContent-type: ");
	if(sFormat==""){
		strcat(msgHeader, "text/plain");
	}else{
		strcat(msgHeader, sFormat);
	}
	strcat(msgHeader, "; charset=UTF-8\r\n");
	strcat(msgHeader, "Content-Transfer-Encoding: quoted-printable\r\n");
	// send header finish command
	strcat(msgHeader, "\r\n");    
	// clean up
	delete to;
	delete cc;
	delete bcc;
	// done
	CString sRes=msgHeader;
	delete [] msgHeader;
	return sRes;
}

const char* const CFastSmtp::GetLocalHostIp() 
{
	in_addr    *iaHost;

	if (gethostname(m_cHostName,255) != SOCKET_ERROR) {
		HOSTENT *pHe = NULL;        
		pHe = gethostbyname(m_cHostName);
		if (pHe != NULL) {
			for (int i=0;pHe->h_addr_list[i] != 0;i++) {
				iaHost = (LPIN_ADDR)pHe->h_addr_list[i];
				m_pcIPAddr = inet_ntoa(*iaHost);
			}
		}            
	} else {            
		DWORD dErr = WSAGetLastError();    
		printf("Failed to get the local ip address\r\n");
	}    

	return m_pcIPAddr;
}

const char* const CFastSmtp::GetLocalHostname() 
{    
	if (gethostname((char FAR*)m_cHostName,255) == SOCKET_ERROR)    
		printf("Failed to get the local hostname\r\n");
	return m_cHostName;
}


//**********************************************************************************
//*** Properties
//**********************************************************************************

bool CFastSmtp::GetConnectStatus()
{
	return (bCon);
}

unsigned const int CFastSmtp::GetBCCRecipientCount()
{
	return (BCCRecipients.GetSize());
}

unsigned const int CFastSmtp::GetCCRecipientCount() 
{
	return (CCRecipients.GetSize());
}

unsigned const int CFastSmtp::GetSocket()
{
	return (hSocket);
}

const char* const CFastSmtp::GetMessageBody() 
{
	return (m_pcMsgBody);
}

unsigned const int CFastSmtp::GetRecipientCount()
{
	return (Recipients.GetSize());
}

const char* const CFastSmtp::GetReplyTo()  
{
	return (m_pcReplyTo);
}

const char* const CFastSmtp::GetSenderEmail() 
{
	return (m_pcFromEmail);
}

const char* const CFastSmtp::GetSenderName() 
{
	return (m_pcFromName);
}

const char* const CFastSmtp::GetSubject() 
{
	return (m_pcSubject);
}

const char* const CFastSmtp::GetXMailer() 
{
	return (m_pcXMailer);
}

void CFastSmtp::SetMessageBody(const char body[])
{
	int s=strlen(body);
	if (m_pcMsgBody)
		delete [] m_pcMsgBody;
	m_pcMsgBody = new char[s+1];
	strcpy(m_pcMsgBody, body);    
}

void CFastSmtp::SetReplyTo(const char replyto[])
{
	int s=strlen(replyto);
	if (m_pcReplyTo)
		delete [] m_pcReplyTo;
	m_pcReplyTo = new char[s+1];
	strcpy(m_pcReplyTo, replyto);
}

void CFastSmtp::SetSenderEmail(const char email[])
{
	int s=strlen(email);
	if (m_pcFromEmail)
		delete [] m_pcFromEmail;
	m_pcFromEmail = new char[s+1];
	strcpy(m_pcFromEmail, email);        
}

void CFastSmtp::SetSenderName(const char name[])
{
	int s=strlen(name);
	if (m_pcFromName)
		delete [] m_pcFromName;
	m_pcFromName = new char[s+1];
	strcpy(m_pcFromName, name);
}

void CFastSmtp::SetSubject(const char subject[])
{
	int s=strlen(subject);
	if (m_pcSubject)
		delete [] m_pcSubject;
	m_pcSubject = new char[s+1];
	strcpy(m_pcSubject, subject);
}

void CFastSmtp::SetXMailer(const char xmailer[])
{
	int s=strlen(xmailer);
	if (m_pcXMailer)
		delete [] m_pcXMailer;
	m_pcXMailer = new char[s+1];
	strcpy(m_pcXMailer, xmailer);
}

/*
*	$Header: /common/IMapi.cpp 2     8/05/01 10:38p Admin $
*
*	$History: IMapi.cpp $
* 
* *****************  Version 2  *****************
* User: Admin        Date: 8/05/01    Time: 10:38p
* Updated in $/common
* Fixes to the /msg crash bug.  Changed the trigger point for Autostrut
* and auto scores.  Added auto scores.
* 
* *****************  Version 1  *****************
* User: Admin        Date: 28/03/01   Time: 8:42p
* Created in $/common
*/
HINSTANCE CIMapi::m_hInstMail = (HINSTANCE) NULL;
BOOL	  CIMapi::m_isMailAvail = (BOOL) -1;

CIMapi::CIMapi()
{
	m_error = 0;										//	Initially error free
	m_sessionHandle = LHANDLE(NULL);

	memset(&m_message, 0, sizeof(MapiMessage));
	memset(&m_from, 0, sizeof(MapiRecipDesc));
	m_message.lpOriginator = &m_from;
	m_from.ulRecipClass = MAPI_ORIG;

	if (m_hInstMail == (HINSTANCE) NULL)				//	Load the MAPI dll
		m_hInstMail = ::LoadLibraryA("MAPI32.DLL");

	if (m_hInstMail == (HINSTANCE) NULL)
	{
#ifndef SERVICE
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
#endif
		m_error = IMAPI_LOADFAILED;
		return;
	}

	ASSERT(m_hInstMail != (HINSTANCE) NULL);			//	Now get the pointer to the send function
	(FARPROC&) m_lpfnSendMail = GetProcAddress(m_hInstMail, "MAPISendMail");
	(FARPROC&) m_lpfnMAPILogon = GetProcAddress(m_hInstMail, "MAPILogon");

	if (m_lpfnSendMail == NULL || m_lpfnMAPILogon == NULL)
	{
#ifndef SERVICE
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
#endif
		m_error = IMAPI_INVALIDDLL;
		return;
	}
}

CIMapi::~CIMapi()
{
	if (m_hInstMail != (HINSTANCE) NULL)
		::FreeLibrary(m_hInstMail);

	m_hInstMail = (HINSTANCE) NULL;

	free(m_message.lpFiles);
	free(m_message.lpRecips);
	free(m_message.lpszSubject);
}

BOOL CIMapi::HasEmail()
{
	if (m_isMailAvail == (BOOL) -1)
		m_isMailAvail = ::GetProfileInt(_T("MAIL"), _T("MAPI"), 0) != 0 && SearchPath(NULL, _T("MAPI32.DLL"), NULL, 0, NULL, NULL) != 0;

	return m_isMailAvail;
}

UINT CIMapi::Error()
{
	UINT temp = m_error;

	m_error = IMAPI_SUCCESS;
	return temp;
}

BOOL CIMapi::AllocNewTo()
{
	//	Allocate a new MapiRecipDesc structure and initialise it to all zeros
	m_message.lpRecips = (MapiRecipDesc *) realloc(m_message.lpRecips, (m_message.nRecipCount + 1) * sizeof(MapiRecipDesc));
	memset(&m_message.lpRecips[m_message.nRecipCount], 0, sizeof(MapiRecipDesc));

	ASSERT(m_message.lpRecips);
	return m_message.lpRecips != (MapiRecipDesc *) NULL;
}

BOOL CIMapi::To(LPCTSTR recip)
{
	LPTSTR temp = new TCHAR[_tcslen(recip) + 1],
		target;

	_tcscpy(temp, recip);
	target = _tcstok(temp, _T(";"));

	while (target != LPTSTR(NULL))
	{
		if (AllocNewTo())
		{
			//	We succeeded in allocating a new recipient record
			m_message.lpRecips[m_message.nRecipCount].lpszName = (LPTSTR) malloc(_tcslen(target) + 1);
			_tcscpy(m_message.lpRecips[m_message.nRecipCount].lpszName, target);
			m_message.lpRecips[m_message.nRecipCount].ulRecipClass = MAPI_TO;
			m_message.nRecipCount++;
			target = _tcstok(NULL, _T(";"));
		}
		else
		{
			m_error = IMAPI_FAILTO;
			return FALSE;
		}
	}

	return TRUE;
}

void CIMapi::Subject(LPCTSTR subject)
{
	m_message.lpszSubject = (LPTSTR) calloc(_tcslen(subject) + 1, sizeof(TCHAR));
	_tcscpy(m_message.lpszSubject, subject);
}

BOOL CIMapi::Attach(LPCTSTR path, LPCTSTR name)
{
	//	Add a new attachment record
	m_message.lpFiles = (MapiFileDesc *) realloc(m_message.lpFiles, (m_message.nFileCount + 1) * sizeof(MapiFileDesc));
	memset(&m_message.lpFiles[m_message.nFileCount], 0, sizeof(MapiFileDesc));

	ASSERT(m_message.lpFiles);

	if (m_message.lpFiles == (MapiFileDesc *) NULL)
	{
		m_error = IMAPI_FAILATTACH;
		return FALSE;
	}

	m_message.lpFiles[m_message.nFileCount].lpszPathName = (LPTSTR) malloc(_tcslen(path) + 1);
	_tcscpy(m_message.lpFiles[m_message.nFileCount].lpszPathName, path);

	if (name != (LPCTSTR) NULL)
	{
		m_message.lpFiles[m_message.nFileCount].lpszFileName = (LPTSTR) malloc(_tcslen(name) + 1);
		_tcscpy(m_message.lpFiles[m_message.nFileCount].lpszFileName, name);
	}

	m_message.nFileCount++;
	return TRUE;
}

BOOL CIMapi::Send(ULONG flags)
{
#ifndef SERVICE
	CWaitCursor wait;
#endif
	int			offset = m_text.GetLength();

	//	Add 1 space per attachment at the end of the body text.
	m_text += CString(' ', m_message.nFileCount);

	//	Set each attachment to replace one of the added spaces at the end of the body text.
	for (UINT i = 0; i < m_message.nFileCount; i++)
		m_message.lpFiles[i].nPosition = offset++;

	m_message.lpszNoteText = (LPTSTR) (LPCTSTR) m_text;	//  Set the body text

#ifndef SERVICE
	// prepare for modal dialog box
	AfxGetApp()->EnableModeless(FALSE);
	HWND hWndTop;
	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, &hWndTop);
	if(pParentWnd){
		// some extra precautions are required to use MAPISendMail as it
		// tends to enable the parent window in between dialogs (after
		// the login dialog, but before the send note dialog).
		pParentWnd->SetCapture();
		::SetFocus(NULL);
		pParentWnd->m_nFlags |= WF_STAYDISABLED;
	}
#endif

#ifdef SERVICE
	int nError /*= m_lpfnMAPILogon(0, _T("Rob"), _T("mapi40"), MAPI_NEW_SESSION, 0, &m_sessionHandle)*/;

	nError = m_lpfnSendMail(m_sessionHandle, 0x00000040, &m_message, flags, 0);
#else
	int nError = m_lpfnSendMail(0,  (pParentWnd?((ULONG)pParentWnd->GetSafeHwnd()):0), &m_message, MAPI_LOGON_UI | flags, 0);
#endif

#ifndef SERVICE
	// after returning from the MAPISendMail call, the window must
	// be re-enabled and focus returned to the frame to undo the workaround
	// done before the MAPI call.
	::ReleaseCapture();
	if(pParentWnd){
		pParentWnd->m_nFlags &= ~WF_STAYDISABLED;

		pParentWnd->EnableWindow(TRUE);
		::SetActiveWindow(NULL);
		pParentWnd->SetActiveWindow();
		pParentWnd->SetFocus();
	}
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);

	AfxGetApp()->EnableModeless(TRUE);
#endif

	//	Now free malloced recipients
	for (i = 0; i < m_message.nRecipCount; i++)
		free(m_message.lpRecips[i].lpszName);

	//	Then free malloced attachments
	for (i = 0; i < m_message.nFileCount; i++)
	{
		free(m_message.lpFiles[i].lpszPathName);
		free(m_message.lpFiles[i].lpszFileName);
	}

	if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
	{
		return FALSE;
	}

	return TRUE;
}


CString urlEscapeString(const char* szString,CString sAddChars)
{
	if(!szString || *szString==0){
		return "";
	}
	CString sRes;
	char const* szPos=szString;
	while(*szPos){
		unsigned char c=*szPos;
		if(c<40 || sAddChars.Find(c)!=-1 || c>126){
			char szTemp[4]={0};
			sprintf(szTemp,"%%%02X",c);
			sRes+=szTemp;
		}else{
			sRes+=(*szPos);
		}
		szPos++;
	}
	return sRes;
}

CString MD5_HexString(const char* SourceString);
CString GetURLForMailByWWW(CString sFrom,CString sTo,CString sSubject,CString sBody)
{
	USES_CONVERSION;
	sFrom=(GetUTFEncoded(sFrom,'<'));
	sTo=(GetUTFEncoded(sTo,'<'));
	sSubject=(GetUTFEncoded(sSubject));
	sBody=(Str2UTF8(A2W(sBody)));
	CString sHash=("WN-CHECk-vaLID-"+sBody+"-"+sTo+"-"+sFrom+"-PAKA-PAKA!");
	sHash=MD5_HexString(sHash);
	sHash.MakeLower();
	
	CString sURL="http://www.wiredplane.com/cgi-bin/wp_mailer.php";
	sURL+="?e_to="+urlEscapeString(sTo);
	sURL+="&e_from="+urlEscapeString(sFrom);
	sURL+="&e_subject="+urlEscapeString(sSubject);
	sURL+="&e_message="+urlEscapeString(sBody);
	sURL+="&e_hash="+urlEscapeString(sHash);// Из-за lower
	return sURL;
}

