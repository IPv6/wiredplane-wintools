// Mailslot.cpp: implementation of the CMailslot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STEXCH_REQ	"WIRENOTE_UPDSTATUS_REQUEST:"
#define FIND_REQ	"WIRENOTE_FIND_REQUEST"
#define SYS_SLOTNM	"wpfindslot"
#define NAMELENGTH 64
CString GetFindMessageText()
{
	return Format(FIND_REQ"STATUS[%i],MLACTIVE[%i],NBACTIVE[%i],XPCACTIVE[%i],FBACTIVE[%i],EMACTIVE[%i],NBNAME[%s],XPCIP[%s],FBNAME[%s],FBID[%s],FBFOLDER[%s],EMMAIL[%s],LASTACT[%s],VERSION[%s]"
			,objSettings.iOnlineStatus
			,objSettings.bProtocolML_Active
			,objSettings.bProtocolNB_Active
			,objSettings.bProtocolXPC_Active
			,objSettings.sFBDir!=""
			,objSettings.bProtocolEM_Active
			,objSettings.bProtocolNB_Active?objSettings.sProtocolNB_Alias:""
			,objSettings.bProtocolXPC_Active?"<not available>":""
			,objSettings.sFBDir!=""?objSettings.sFBNick:""
			,objSettings.sFBDir!=""?objSettings.sFB_ID:""
			,objSettings.sFBDir!=""?objSettings.sFBDir:""
			,objSettings.sEMmail
			,objSettings.dtLastSentMsgDate.GetStatus()==COleDateTime::valid?objSettings.dtLastSentMsgDate.Format("%x %X"):""
			,PROG_VERSION);
}


void GetAllInfo(CFindedUser& pUser,CString& sStatus)
{
	// Имя пользователя
	pUser.sNick=CDataXMLSaver::GetInstringPart("FBNAME[","]",sStatus);
	if(pUser.sNick==""){
		pUser.sNick=CDataXMLSaver::GetInstringPart("NBNAME[","]",sStatus);
	}
	// Доп информация
	pUser.sAddInfo=_l("Reachable via")+" ";
	if(atol(CDataXMLSaver::GetInstringPart("MLACTIVE[","]",sStatus))){
		pUser.sAddInfo+="Mailslot";
	}
	if(atol(CDataXMLSaver::GetInstringPart("NBACTIVE[","]",sStatus))){
		if(pUser.sAddInfo!=""){
			pUser.sAddInfo=AddPathExtension(pUser.sAddInfo,", ");
		}
		pUser.sAddInfo+="WinPopup";
	}
	if(atol(CDataXMLSaver::GetInstringPart("XPCACTIVE[","]",sStatus))){
		if(pUser.sAddInfo!=""){
			pUser.sAddInfo=AddPathExtension(pUser.sAddInfo,", ");
		}
		pUser.sAddInfo+="WireNote(xpc)";
	}
	if(atol(CDataXMLSaver::GetInstringPart("FBACTIVE[","]",sStatus))){
		if(pUser.sAddInfo!=""){
			pUser.sAddInfo=AddPathExtension(pUser.sAddInfo,", ");
		}
		pUser.sAddInfo+="Billboard";
		pUser.sAddInfo+=", ";
		pUser.sAddInfo+=_l("Billboard folder")+": "+CDataXMLSaver::GetInstringPart("FBFOLDER[","]",sStatus);
	}
	// IP пользователя
	if(atol(CDataXMLSaver::GetInstringPart("NBACTIVE[","]",sStatus))){
		if(pUser.sID!=""){
			pUser.sID=AddPathExtension(pUser.sID,";");
		}
		pUser.sID+=CDataXMLSaver::GetInstringPart("NBNAME[","]",sStatus);
		pUser.sID+="/nb";
	}
	if(atol(CDataXMLSaver::GetInstringPart("XPCACTIVE[","]",sStatus))){
		if(pUser.sID!=""){
			pUser.sID=AddPathExtension(pUser.sID,";");
		}
		pUser.sID+=CDataXMLSaver::GetInstringPart("NBNAME[","]",sStatus);
		pUser.sID+="/xpc";
	}
	if(atol(CDataXMLSaver::GetInstringPart("FBACTIVE[","]",sStatus))){
		if(pUser.sID!=""){
			pUser.sID=AddPathExtension(pUser.sID,";");
		}
		pUser.sID+=CDataXMLSaver::GetInstringPart("FBID[","]",sStatus);
		pUser.sID+="/fb";
	}
	if(atol(CDataXMLSaver::GetInstringPart("MLACTIVE[","]",sStatus))){
		if(pUser.sID!=""){
			pUser.sID=AddPathExtension(pUser.sID,";");
		}
		pUser.sID+=CDataXMLSaver::GetInstringPart("NBNAME[","]",sStatus);
		pUser.sID+="/ms";
	}
	CString sEmail=CDataXMLSaver::GetInstringPart("EMMAIL[","]",sStatus);
	if(sEmail!=""){
		if(pUser.sID!=""){
			pUser.sID=AddPathExtension(pUser.sID,";");
		}
		pUser.sID+=sEmail;
		pUser.sID+="/em";
	}
	pUser.iStatus=atol(CDataXMLSaver::GetInstringPart("STATUS[","]",sStatus));
	pUser.sUserDsc=CDataXMLSaver::GetInstringPart("LASTACT[","]",sStatus);
}

BOOL SendFindOnlineReq(const char* szTarget)
{
	BOOL bRes=0;
	HANDLE hSlot = CreateFile(Format("\\\\%s\\mailslot\\"SYS_SLOTNM,szTarget),GENERIC_WRITE, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSlot != INVALID_HANDLE_VALUE){
		CString sWMessage=GetFindMessageText();
		DWORD dwTotalMsgSize=sWMessage.GetLength()+1;
		bRes=WriteFile(hSlot, (char*)sWMessage.GetBuffer(dwTotalMsgSize), dwTotalMsgSize, &dwTotalMsgSize, NULL);
		CloseHandle(hSlot);
	}
	return bRes;
}

DWORD WINAPI MailSlotListener_Raw(BOOL& bStarted, HANDLE hSlotID,BOOL bSystemMessages)
{
	USES_CONVERSION;
	COMMTIMEOUTS timeOut;
	memset(&timeOut,0,sizeof(timeOut));
	BOOL bManualSleep=TRUE;//objSettings.bUnderWindows98
	if(bManualSleep){
		// Под виндами жрет процессор и память
		memset(&timeOut,0,sizeof(timeOut));
	}else{
		timeOut.ReadIntervalTimeout=0;
		timeOut.ReadTotalTimeoutMultiplier=0;
		timeOut.ReadTotalTimeoutConstant=objSettings.dwNetMessagingTimeout;
		timeOut.WriteTotalTimeoutMultiplier=0;
		timeOut.WriteTotalTimeoutConstant=objSettings.dwNetMessagingTimeout;
	}
	::SetCommTimeouts(hSlotID,&timeOut);
	while(bStarted==1){
		char szMessage[1024]="";
		DWORD dwBytesRead=0;
		BOOL bRes=FALSE;
		if(bManualSleep){
			// Крутимся в цикле и ждем сообщения
			DWORD dwMessagesAvaiting=0;
			DWORD dwMessagesSize=0;
			DWORD dwTimeout=5000;
			while(dwMessagesAvaiting==0){
				BOOL bRes=GetMailslotInfo(hSlotID,NULL,&dwMessagesSize,&dwMessagesAvaiting,&dwTimeout);
				if(!bRes){
					// Кранты!
					return 0;
				}
				if(dwMessagesAvaiting>0){
					break;
				}
				Sleep(5000);
			}
		}
		bRes=ReadFile(hSlotID, szMessage, sizeof(szMessage),&dwBytesRead,NULL);
		if(bRes==TRUE){
			CString sFrom=szMessage;
			sFrom.TrimLeft();
			sFrom.TrimRight();
			if(sFrom!=""){
				if(bSystemMessages){
					if(sFrom.Find(STEXCH_REQ)==0){
						CString sStatus=sFrom.Mid(strlen(STEXCH_REQ));
						UpdateUserIconFromStatusStringID(sStatus);
					}
					if(sFrom.Find(FIND_REQ)==0){
						CString sStatus=sFrom.Mid(strlen(FIND_REQ));
						if(!objSettings.pCurrentFindDialog){
							SendFindOnlineReq(CDataXMLSaver::GetInstringPart("NBNAME[","]",sStatus));
						}else{
							CFindedUser pUser;
							GetAllInfo(pUser,sStatus);
							objSettings.pCurrentFindDialog->AddFindPersonResult(pUser);
						}
					}
				}else{
					CString sTo=szMessage+strlen(sFrom)+1;
					CString sMessage=szMessage+strlen(sFrom)+1+strlen(sTo)+1;
					int iTrMsgLen=strlen(sMessage)*2;
					{
						char* szTRMessage=new char[iTrMsgLen];
						memset(szTRMessage,0,iTrMsgLen);
						// Конвертим из Dos в Win
						OemToChar(sMessage,szTRMessage);
						sMessage=szTRMessage;
						delete[] szTRMessage;
					}
					sFrom+="/ms";//Показываем что mailslтовское сообщение
					OpenMessage(sFrom,sTo,sMessage);
				}
			}
		}
		Sleep(5000);
	}
	return 0;
}

DWORD WINAPI MailSlotListener2(LPVOID lpdwstatus)
{
	CNetMLProtocol* pParent=(CNetMLProtocol*)lpdwstatus;
	if(!pParent){
		return 1;
	}
	pParent->hSMBFMailslot=CreateMailslot("\\\\.\\mailslot\\"SYS_SLOTNM,0,0,(LPSECURITY_ATTRIBUTES) NULL);
	if (pParent->hSMBFMailslot == INVALID_HANDLE_VALUE){ 
		objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Error creating mailslot for finding other started instances!"));
		CloseHandle(pParent->hSMBFMailslot);
        return FALSE;
	}
	MailSlotListener_Raw(pParent->bStarted,pParent->hSMBFMailslot,TRUE);
	CloseHandle(pParent->hSMBFMailslot);
	return 0;
}

DWORD WINAPI MailSlotListener(LPVOID lpdwstatus)
{
	CNetMLProtocol* pParent=(CNetMLProtocol*)lpdwstatus;
	if(!pParent){
		return 1;
	}
	MailSlotListener_Raw(pParent->bStarted,pParent->SMBMailslot,FALSE);
	return 0;
}

BOOL CNetMLProtocol::Start()
{
	SMBMailslot=CreateMailslot(Format("\\\\.\\mailslot\\%s",m_sSlot),0,0,(LPSECURITY_ATTRIBUTES) NULL);
	if (SMBMailslot == INVALID_HANDLE_VALUE){ 
		// Mailslotы не работыют.... Наверно запущен Messenger-сервис
		objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Error creating mailslot! Mailslot messages unaviable now"));
		CloseHandle(SMBMailslot);
		bStarted=0;
        return FALSE;
	}
	bStarted=1;
	hSMBStatusThread = CreateThread( NULL, 0, MailSlotListener, this, 0, &dwSMBThreadID);
	hSMBFStatusThread = CreateThread( NULL, 0, MailSlotListener2, this, 0, &dwSMBFThreadID);
	return TRUE;
}

CNetMLProtocol::CNetMLProtocol(const char* szSlot)
{
	m_sSlot=szSlot;
	dwSMBThreadID=0;
}

CNetMLProtocol::~CNetMLProtocol()
{
	bStarted=0;
	CloseHandle(SMBMailslot);
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus<0){
		TerminateThread(hSMBStatusThread,99);
		TerminateThread(hSMBFStatusThread,99);
	}else if(WaitForSingleObject(hSMBStatusThread,500)!=WAIT_OBJECT_0){
		if(hSMBStatusThread){
			TerminateThread(hSMBStatusThread,99);
		}
	}else{
		CloseHandle(hSMBStatusThread);
	}
	if(WaitForSingleObject(hSMBFStatusThread,500)!=WAIT_OBJECT_0){
		if(hSMBFStatusThread){
			TerminateThread(hSMBFStatusThread,99);
		}
	}else{
		CloseHandle(hSMBFStatusThread);
	}
}

BOOL CNetMLProtocol::SendNet_Text(const char* szRecipient, CNetMessage_Text* str)
{
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return 0;
	}
	if(bStarted==0){
		Alert(_l(DEFAULT_NOPROTWARN),5000,FALSE,DT_CENTER|DT_VCENTER);
	}
	/*str->sMessageAttach.TrimRight();
	if(str->sMessageAttach.GetLength()>0 && !objSettings.lDisableAttachExtens){//&& str->sMessageAttach.GetLength()<100
		// Аттачим в текст!!!
		str->sMessageAttach.Replace("<","&lt;");
		str->sMessageAttach.Replace(">","&gt;");
		str->sMessageBody+="\n<!--WNSYS:[WireNote] You can safely ignore the rest of the message\n"+str->sMessageAttach+"-->";
		str->sMessageAttach="";
	}else if(str->sMessageAttach!=""){
		Alert(_l(DEFAULT_ATTWARN),DEFAULT_READTIME,FALSE,DT_CENTER|DT_VCENTER);
	}*/
	if(str->sMessageBody.GetLength()>400){
		//SmartWarning(_l("Message is larger than 400b. Due protocol limitations there is a high risk")+"\n"+_l("that recipient will not receive it at all")+"!",_l("Mailslot: Warning"),&objSettings.lSkipSizeNotification);
		CString sMsg=str->sMessageBody;
		int iRes=0;
		while(sMsg!=""){
			if(sMsg.GetLength()>400){
				str->sMessageBody=sMsg.Left(400);
				sMsg=sMsg.Mid(400);
			}else{
				str->sMessageBody=sMsg;
				sMsg="";
			}
			iRes=SendNet_Text(szRecipient,str);
		}
		return iRes;
	}
	HANDLE hSlot = CreateFile(Format("\\\\%s\\mailslot\\%s",szRecipient,m_sSlot),GENERIC_WRITE, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSlot != INVALID_HANDLE_VALUE){
		CString sWMessage=str->sMessageBody;
		//
		USES_CONVERSION;
		CString sWCompName=objSettings.sLocalComputerName;
		CString sWRecipient=szRecipient;
		ExtractSenderFromText(sWCompName, sWMessage);
		sWMessage.AnsiToOem();
		sWMessage=A2W(sWMessage);
		BOOL bRes=FALSE;
		DWORD dwTotalMsgSize=strlen(sWCompName)+strlen(sWRecipient)+strlen(sWMessage)+3;
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, dwTotalMsgSize);
		if (NULL != hGlobal){
			char* szSendText=(char*)::GlobalLock(hGlobal);
			if(szSendText){
				strcpy(szSendText,sWCompName);
				strcpy(szSendText+strlen(sWCompName)+1,sWRecipient);
				strcpy(szSendText+strlen(sWCompName)+1+strlen(sWRecipient)+1,sWMessage);
				bRes=WriteFile(hSlot, (char*)szSendText, dwTotalMsgSize, &dwTotalMsgSize, NULL);
				CloseHandle(hSlot);
				if(!bRes){
					str->sError.Format("%s (MS)",GetCOMError(GetLastError()));
				}
				::GlobalUnlock(hGlobal);
			}else{
				str->sError=_l("Unsufficient memory (MS)");
			}
			VERIFY(::GlobalFree(hGlobal)==NULL);
		}else{
			str->sError=_l("Unsufficient memory (MS)");
		}
		return bRes;
	}else{
		str->sError=_l("Invalid mailslot handle (MS)");
	}
	return FALSE;
}

BOOL CNetMLProtocol::FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers)
{
	BOOL bRes=0;
	CString sDomain="*";
	TCHAR domainName[NAMELENGTH]={0};
	DWORD domainNameLength = ExpandEnvironmentStrings(_T("%USERDOMAIN%"),	domainName,	NAMELENGTH);
	if(domainName[0]!=0){
		sDomain=domainName;
	}
	return SendFindOnlineReq(sDomain);
}

BOOL CNetMLProtocol::SendNet_StatusExch(const char* szRecipient,const char* szProtocol)
{
	BOOL bRes=0;
	HANDLE hSlot = CreateFile(Format("\\\\%s\\mailslot\\"SYS_SLOTNM,szRecipient),GENERIC_WRITE, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSlot != INVALID_HANDLE_VALUE){
		CString sWMessage=Format(STEXCH_REQ"%s",CreateMyStatusStringID(objSettings.sLocalComputerName));
		DWORD dwTotalMsgSize=sWMessage.GetLength()+1;
		bRes=WriteFile(hSlot, (char*)sWMessage.GetBuffer(dwTotalMsgSize), dwTotalMsgSize, &dwTotalMsgSize, NULL);
		CloseHandle(hSlot);
		if(!bRes){
			// неудача
		}
		bRes=0;// Чтобы по-умолчанию юыло недоступно
	}
	return bRes;
}
