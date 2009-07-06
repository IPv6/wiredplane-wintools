// EmailBox.cpp: implementation of the CEmailBox class.
//
//////////////////////////////////////////////////////////////////////

Сделать просьбу отправить смс! в качестве рекламы
и для языкатора

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL& IsEmbedded();
void AlertEmProtOff(CString szErr)
{
	static long lErrCount=0;
	if(szErr==""){
		lErrCount=0;
	}else{
		lErrCount++;
		if(lErrCount>10){
			Alert(szErr/*+"\n"+_l("EmailBox protocol disabled")*/,_l("EmailBox protocol"));
		}
#ifdef _DEBUG
		DebugBreak();
#endif
	}
}

BOOL StripTags(CString& sBodyText);
BOOL DeEntitize(CString& sBodyText);
DWORD WINAPI EMListener(LPVOID lpdwstatus)
{
	CNetEMProtocol* pParent=(CNetEMProtocol*)lpdwstatus;
	if(!pParent){
		return 1;
	}
	::CoInitialize(NULL);
	CString sError;
	DWORD dwSleepTime=30*1000;
	DWORD dwWaitRes=0;
	while(TRUE){
		dwWaitRes=WaitForSingleObject(pParent->hWaitEvent,dwSleepTime);
		dwSleepTime=objSettings.lEMPeriod*1000*60;
		if(dwWaitRes==WAIT_OBJECT_0 && !pParent->bStarted){
			break;
		}
		pParent->lInCheck++;
		CPop3Connection m_pcPOP3;
		m_pcPOP3.SetTimeout(objSettings.dwMailboxTimeout*1000);
		BOOL blRet = m_pcPOP3.Connect(objSettings.sEM_p3host, objSettings.sEM_p3login, objSettings.sEM_p3psw, objSettings.lEM_p3port, &sError);
		if(objSettings.sEM_p3host!=""){
			if(blRet != false){
				int iCount=0, iSize=0, i=0;
				BOOL blRet = m_pcPOP3.Statistics(iCount, iSize);
				CArray<CEmailHeader,CEmailHeader&> aMsgs;
				for(i=0;i<iCount;i++){
					CEmailHeader eh;
					CAntiSpamAccount::RetrieveSingleMessageRaw(&m_pcPOP3, i, &eh, 20);
					eh.iIndex=aMsgs.GetSize();
					aMsgs.Add(eh);
				}
				// Смотрим наши сообщения и фигачим их
				for(i=0;i<aMsgs.GetSize();i++){
					if(aMsgs[i].bBroken){
						continue;
					}
					if(aMsgs[i].bAttachment || aMsgs[i].sFields[MAIL_BODYFIELD].Find("multipart/mixed")!=-1){
						continue;
					}
					BOOL bOurSubj=0;
					CString sSubj=aMsgs[i].sFields[MAIL_SUBJFIELD];
					sSubj.Replace(" ","");
					sSubj.Replace("_","");
					sSubj.Replace("-","");
					if(sSubj.Find(DEF_EMAIL_SUBJ)!=-1){
						bOurSubj=1;
					}
					CString sFrom=CDataXMLSaver::GetInstringPart("<",">",aMsgs[i].sFields[MAIL_FROMFIELD]);
					if(sFrom==""){
						sFrom=aMsgs[i].sFields[MAIL_FROMFIELD].SpanExcluding(" \t");
						sFrom.TrimLeft(" \r\n<>(){}[]");
						sFrom.TrimRight(" \r\n<>(){}[]");
					}
					if(objSettings.sEM_Ads!="" && PatternMatchList((sSubj+" "+sFrom),objSettings.sEM_Ads,";")){
						bOurSubj=2;
					}
					int iPerson=objSettings.AddrBook.FindPersonByIP(sFrom);
					BOOL bDaemon=(sFrom.CompareNoCase("MAILER-DAEMON")==-1)?1:0;
					if( bOurSubj==2 || (!bDaemon && bOurSubj)){
						CEmailHeader note;
						CAntiSpamAccount::RetrieveSingleMessageRaw(&m_pcPOP3, i, &note, -1);
						if(!note.bBroken && note.sFields[MAIL_BDFLD_PR].Find("This is a multi-part message in MIME format")==-1){
							CString sRealFrom=note.sFields[MAIL_FROMFIELD];
							CString sFrom=(iPerson==-1)?(sRealFrom+"/em"):(objSettings.AddrBook.aPersons[iPerson]->GetTitle()+" <"+sRealFrom+">");
							CString sTo=note.sFields[MAIL_TOFIELD];
							CString sMessage=note.sFields[MAIL_BDFLD_PR];
							CString sAttach=CDataXMLSaver::GetInstringPart("<!--WNSYS'","'-->",note.sFields[MAIL_BODYFIELD]);
							CDataXMLSaver::StripInstringPart("<!--","-->",sMessage);
							StripTags(sMessage);
							DeEntitize(sMessage);
							OpenMessage(sFrom,sTo,sMessage,sFrom,sAttach,note.sFields[MAIL_SUBJFIELD]);
							CAntiSpamAccount::DeleteMessageRaw(&m_pcPOP3,&aMsgs[i]);
						}
					}
				}
				m_pcPOP3.Disconnect();
			}
		}
		pParent->lInCheck--;
	}
	CloseHandle(pParent->hWaitEvent);
	::CoUninitialize();
	return 0;
}

BOOL CNetEMProtocol::Start()
{
	bStarted=1;
	hEMStatusThread = CreateThread( NULL, 0, EMListener, this, 0, &dwEMThreadID);
	return TRUE;
}

CNetEMProtocol::CNetEMProtocol()
{
	lInCheck=0;
	bStarted=0;
	dwEMThreadID=0;
	hEMStatusThread=NULL;
	hWaitEvent=::CreateEvent(NULL,FALSE,FALSE,"");
}

CNetEMProtocol::~CNetEMProtocol()
{
	bStarted=0;
	SetEvent(hWaitEvent);
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus<0){
		TerminateThread(hEMStatusThread,99);
	}else if(WaitForSingleObject(hEMStatusThread,objSettings.dwNetMessagingTimeout)!=WAIT_OBJECT_0){
		if(hEMStatusThread){
			TerminateThread(hEMStatusThread,98);
		}
	}
	CloseHandle(hEMStatusThread);
}

BOOL CNetEMProtocol::SendNet_Text(const char* szRecipientIn, CNetMessage_Text* str)
{
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return 0;
	}
	objSettings.sEMmail.TrimLeft();
	objSettings.sEMmail.TrimRight();
	if(objSettings.sEMmail==""){
		static long lAlread=0;
		if(lAlread){
			return 0;
		}
		SimpleTracker Lock(lAlread);
		/*
		Alert(_l("You should set your email first\nCheck Email protocol preferences")+"!",_l("New email message"));
		pMainDialogWindow->SendMessage(OPENPREFS,NULL,LPARAM(strdup(DEF_ACTIVEPROTS)));
		*/
		if(!IsEmbedded()){
			CDlgSimpleEdit ed;
			ed.m_Edit=_l("Enter your email address (will be used as 'Sender' of this message)");
			ed.m_EditTitle="";
			ed.iStyle=3;
			if(ed.DoModal()!=IDOK){
				return 0;
			}
			objSettings.sEMmail=ed.m_EditTitle;
		}else{
			objSettings.sEMmail="<reply address missed>";
		}
	}
	BOOL bRes=0;
	CString sEmailSubjDef=DEF_EMAIL_SUBJ;
	sEmailSubjDef+=" ";
	if(IsEmbedded()){
		sEmailSubjDef="";
	}
	if(str->sMessageTitle==""){
		str->sMessageTitle=Format("%s%s %s",sEmailSubjDef,_l("Message from"),objSettings.sEMmail);
	}else{
		str->sMessageTitle=Format("%s%s",sEmailSubjDef,str->sMessageTitle);
	}
	CString sEmailBody=str->sMessageBody;
	sEmailBody.Replace("\r","");
	sEmailBody.Replace("\n","<br>");
	CString sBody="";//Format("<!--WNDATA %s %s %s WNDATA-->",DEF_EMAIL_SUBJ,_l("from"),objSettings.sEMmail);
	sBody+=sEmailBody;
	str->sMessageAttach.TrimRight();
	if(str->sMessageAttach!="" && !objSettings.lDisableAttachExtens){
		sBody+="\r\n";
		str->sMessageAttach.Replace("&","&amp;");
		str->sMessageAttach.Replace(">","&gt;");
		sBody+="<!--WNSYS'"+str->sMessageAttach+"'-->";
	}
	// Сама отправка
	if(objSettings.sEM_smhost==""){
		CString sURL=GetURLForMailByWWW(objSettings.sEMmail,szRecipientIn,str->sMessageTitle,sBody);
		CWebWorld wb;
		CString sResult=wb.GetWebPage(sURL);
		if(sResult.CompareNoCase("TRUE")==0){
			return TRUE;
		}
		CIMapi mail;
		if (mail.Error() == CIMapi::IMAPI_SUCCESS)
		{
			mail.To(szRecipientIn);
			mail.From(objSettings.sEMmail);
			mail.Subject(str->sMessageTitle);
			//mail.Attach("somefilename");
			mail.Text(sBody);//CString("Content-Type: Text/HTML;\n\n")+
			//mail.SetHTML();
			bRes=mail.Send();
			if(!bRes){
				str->sError=_l("Failed to send email message");
			}
			if(bRes==2){
				Alert(_l("MAPI call failed. Check your default email client\nIt should be started and configured as MAPI handler")+"!",_l("New email message"));
			}
		}else{
			str->sError=_l("Failed to create MAPI client");
			return FALSE;
		}
		str->sError="Unknown";
		return FALSE;
	}else{
		CFastSmtp mail;
		if(objSettings.sEM_smlogin!=""){
			mail.SetUserName(objSettings.sEM_smlogin);
			mail.SetPassward(objSettings.sEM_smpsw);
		}
		if (mail.ConnectServer(objSettings.sEM_smhost,(unsigned short)atol(objSettings.sEM_smport))) {
			mail.SetOutFormat("text/html");
			objSettings.sEMmail.TrimLeft();
			objSettings.sEMmail.TrimRight();
			if(objSettings.sEMmail==""){
				Alert(_l("Email protocol options are not correct. Please fill your email (it will be used for reply)"));
			}
			mail.SetSenderName(objSettings.sEMmail);
			mail.SetSenderEmail(objSettings.sEMmail);
			mail.SetReplyTo(objSettings.sEMmail);
			mail.SetSubject(str->sMessageTitle);
			mail.AddRecipient(szRecipientIn);
			mail.SetMessageBody(sBody);
			if (mail.GetConnectStatus()) {        
				CString sErr;
				bRes=mail.Send(sErr);
				if(!bRes){
					str->sError=_l("Failed to send email message")+": \r\n"+sErr;
				}
			}else{
				str->sError=_l("No SMTP connection");
			}
			mail.Disconnect();
		}else{
			str->sError=_l("Failed to connect to SMTP server");
			return FALSE;
		}
	}
	return bRes;
}

BOOL CNetEMProtocol::SendNet_StatusExch(const char* szRecipient,const char* szProtocol)
{
	return TRUE;
}

BOOL CNetEMProtocol::FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers)
{
	return FALSE;
}
