// MessageWrapper.cpp: implementation of the CMessageWrapper class.
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
BOOL& IsEmbedded();
CMapStringToString aRecipientIPMap;
void GetRecipientIP(CString& sRecipient, BOOL bLookInCasheOnly)
{
	if(getCharCount(sRecipient,'.')!=3){
		// Не IP адрес. конвертим
		CString sValue;
		if(aRecipientIPMap.Lookup(sRecipient,sValue)){
			sRecipient=sValue;
		}else if(!bLookInCasheOnly){
			char dot[6]="";
			char result[24]="";
			int i = 0;
			u_long *ppIpNO;
			u_long *pIpNO;
			HOSTENT FAR *lphostent;
			u_long ipHO;
			unsigned char binIp[4];
			int iterations = 0;

			//Lok up this host info via supplied name
			lphostent = gethostbyname(sRecipient);
			if (lphostent == NULL)
				return;
			//Retreive first entry (might have multiple connects)
			do
			{
				iterations++;
				ppIpNO = (u_long *)lphostent->h_addr_list;
				if (ppIpNO+i == NULL){
					break;
				}
				pIpNO = ((u_long *)*(ppIpNO+i));
				if (pIpNO == NULL){
					break;
				}
				//convert back to host order, since SOCKADDR_IN expects that
				//MessageBox(NULL,"z","x",MB_OK);
				ipHO = ntohl(*pIpNO);
				binIp[0] = (BYTE)((ipHO & 0xff000000) >> 24);
				itoa(binIp[0], dot, 10);
				strcat(result,dot);
				binIp[1] = (BYTE)((ipHO & 0x00ff0000) >> 16);
				itoa(binIp[1], dot, 10);
				strcat(result, "."); strcat(result, dot);
				binIp[2] = (BYTE)((ipHO & 0x0000ff00) >> 8);
				itoa(binIp[2], dot, 10);
				strcat(result, "."); strcat(result, dot);
				binIp[3] = (BYTE)(ipHO & 0x000000ff);
				itoa(binIp[3], dot, 10);
				strcat(result,"."); strcat(result, dot);
				i++;
			} while ((pIpNO != NULL) && (iterations < 6));
			aRecipientIPMap.SetAt(sRecipient,result);
			sRecipient=result;
		}
	}
}

CMessageWrapper::CMessageWrapper()
{
	WSADATA WSAData;
	int rc=WSAStartup(MAKEWORD(1,1),&WSAData);
	if(rc!=0){
		AddError(_l("Can not initialize socket subsystem")+" "+_l("XPC protocol unaviable now"));
	}
	pMLNet=NULL;
	pNBNet=NULL;
	pXPCNet=NULL;
	pFBNet=NULL;
	pEMNet=NULL;
	pRSNet=NULL;
}

CMessageWrapper::~CMessageWrapper()
{
	objSettings.bNetSubsystemInitialized=FALSE;
	if(pNBNet){
		delete pNBNet;
	}
	pNBNet=NULL;
	if(pMLNet){
		delete pMLNet;
	}
	pMLNet=NULL;
	if(pXPCNet){
		delete pXPCNet;
	}
	pXPCNet=NULL;
	if(pFBNet){
		delete pFBNet;
	}
	pFBNet=NULL;
	if(pEMNet){
		delete pEMNet;
	}
	pEMNet=NULL;
	if(pRSNet){
		delete pRSNet;
	}
	pRSNet=NULL;
}

void CMessageWrapper::ConnectAll()
{
	objSettings.bNetSubsystemInitialized=FALSE;
	RecreateRSModule();
	RecreateEMModule();
	if(!IsEmbedded()){
		RecreateMLModule();
		RecreateNBModule();
		RecreateXPCModule();
		RecreateFBModule();
	}
	objSettings.bNetSubsystemInitialized=TRUE;
}

void CheckByFilters(CString sFrom,CString sFromNick,CString sMessage,BOOL& bMessageTrashed,CString& sTrashedDsc)
{
	for(int i=0;i<objSettings.messageFilters.GetSize();i++){
		CMessageFilter* pFilter=objSettings.messageFilters[i];
		if(pFilter && pFilter->dwActive){
			CString sSearchText=sMessage;
			if(pFilter->dwSearchWhere==0 || pFilter->dwSearchWhere==1){
				CString sTitle="";
				sTitle+=sFrom;
				sTitle+="|||";
				sTitle+=sFromNick;
				sTitle+="|||";
				if(pFilter->dwSearchWhere==0){
					sSearchText+=sTitle;
				}else{
					sSearchText=sTitle;
				}
			}
			sSearchText.MakeLower();
			pFilter->sSearchString.MakeLower();
			if(CheckMyMaskSubstr(sSearchText,pFilter->sSearchString,FALSE)){
				if(pFilter->dwDoWhat==1){
					bMessageTrashed=TRUE;
					sTrashedDsc="Filtered out";
				}
				if(pFilter->dwDoWhat==2){
					bMessageTrashed=FALSE;
					sTrashedDsc="";
				}
				if(pFilter->dwDoWhat==3){
					bMessageTrashed=FALSE;
					sTrashedDsc="";
					pMainDialogWindow->ShowBaloon(_l("Incoming message"), Format("%s: %s!",sFromNick,_l("New message arrived")));
				}
			}
		}
	}
}

void OpenMessage(CString sFrom, CString sTo, CString sMessage, CString sRealFrom, CString sAttach, CString sMessageTitle)
{
	// Враппинг аттачей
	sAttach.TrimRight();
	sAttach.TrimLeft();
	if(sAttach=="" && sMessage.Find("<!--WNSYS'")!=-1){
		sAttach=CDataXMLSaver::GetInstringPart("<!--WNSYS'","'-->",sMessage);
		sMessage.Replace(CString("<!--WNSYS'")+sAttach+"'-->","");
		sAttach.Replace("&gt;",">");
		sAttach.Replace("&amp;","&");
	}
	// Проверяем мошенничество
	BOOL bFraud=FALSE;
	if(sRealFrom==""){
		sRealFrom=sFrom;
	}
	CString sFraudDsc;
	if(objSettings.bCheckFraudMsg && objSettings.AddrBook.TranslateIPToNick(sFrom)!=objSettings.AddrBook.TranslateIPToNick(sRealFrom)){
		bFraud=TRUE;
		sFraudDsc=Format("%s! %s: %s",_l("ATTENTION! This is a fraud message"),_l("Real sender"),sRealFrom);
		Alert(sFraudDsc);
		AddError(_l("Fraud message"));
	}
	int iPersonNum=0, iGroup=0;
	CString sFromNick=objSettings.AddrBook.TranslateIPToNick(sFrom,&iPersonNum,&iGroup);
	// Проверяем отсев сообщения по статусу/автору/телу сообщения
	BOOL bMessageTrashed=FALSE;
	CString sTrashedDsc="";
	if(objSettings.iOnlineStatus>=3){//DND
		bMessageTrashed=TRUE;
		sTrashedDsc="Message in 'DND' mode";
	}
	// Для Busy не принимаются сообщения от неизвестных юзеров...
	if(iPersonNum==-1 && objSettings.iOnlineStatus==2){
		bMessageTrashed=TRUE;
		sTrashedDsc="Message from unknown user in 'busy' mode";
	}
	CheckByFilters(sFrom,sFromNick,sMessage,bMessageTrashed,sTrashedDsc);
	if(sTrashedDsc!="" && objSettings.lNotifyAboutFMes){
		pMainDialogWindow->ShowBaloon(_l("Incoming message"), _l("Incoming message was blocked")+": "+_l(sTrashedDsc)+"\n"+_l("Check message history for details"));
	}
	//
	if(sAttach.Find("["DEF_DELIVERYNOTIFY"]")!=-1){
		CString sDelivTime="";
		if(objSettings.lShowNotInPopup){
			CString sTimeStamp=CDataXMLSaver::GetInstringPart("["DEF_NOTTIMESTAMP,"]",sAttach);
			if(sTimeStamp.GetLength()==14){
				int iY=atol(sTimeStamp.Mid(0,4));
				int iM=atol(sTimeStamp.Mid(4,2));
				int iD=atol(sTimeStamp.Mid(6,2));
				int ih=atol(sTimeStamp.Mid(8,2));
				int im=atol(sTimeStamp.Mid(10,2));
				int is=atol(sTimeStamp.Mid(12,2));
				COleDateTime dt(iY,iM,iD,ih,im,is);
				COleDateTime dtCur=COleDateTime::GetCurrentTime();
				if(dt.GetYear()==dtCur.GetYear() && dt.GetMonth()==dtCur.GetMonth() && dt.GetDay()==dtCur.GetDay()){
					sDelivTime=dt.Format("%X");
				}else{
					sDelivTime=dt.Format("%x %X");
				}
				if(objSettings.lShowNotAfterTime){
					if((dtCur-dt).GetMinutes()<5){
						bMessageTrashed=1;
					}
				}
			}
		}else{
			bMessageTrashed=1;
		}
		if(!bMessageTrashed && objSettings.lShowNotInPopup==1){
			if(pMainDialogWindow){
				CString sNotificationText=_l("Delivery notification");
				if(sAttach.Find("["DEF_READREQUEST"]")!=-1){
					sNotificationText=_l("Read notification");
				}
				pMainDialogWindow->ShowBaloon(sNotificationText, sDelivTime+" "+sMessage);
			}
			return;
		}
	}else{// Логируем...
		objSettings.lStatReceivedMessages++;
		CString sMessageText=sMessage;
		sMessageText.Replace("\n","\r\n");
		CDataXMLSaver::StripInstringPart("<!--WNSYS'","'-->",sMessageText);
		CDataXMLSaver::StripInstringPart("[DEF_","]",sMessageText);
		CString sLogMessage=Format(
			"\r\n%s: %s %s %s %s\r\n"
			"%s\r\n",
			_l("Recipient"), sTo,
			(bMessageTrashed	?Format("%s",_l(sTrashedDsc)):""),
			(bFraud				?Format("%s",sFraudDsc):""),
			(strlen(sAttach)>200?Format("%s: %ib",_l("Attachment"),strlen(sAttach)):""),
			sMessageText);
		objLog.AddMsgLogLineRaw(sFromNick, Format("%s. %s: %s (%s) ",_l("Incoming message"),_l("Sender"), sFromNick, sFrom), sLogMessage);
	}
	//
	if(iPersonNum!=-1 && !iGroup && (objSettings.AddrBook.aPersons[iPersonNum]->iOnlineStatus<0 || objSettings.AddrBook.aPersons[iPersonNum]->iOnlineStatus>3)){
		// Если информация о ком-то неуспела обновится, ставим что хотябы в онлайне
		objSettings.AddrBook.aPersons[iPersonNum]->iOnlineStatus=0;
	}
	if(bMessageTrashed){
		return;
	}
	//
	CString sLastMessage;
	BOOL bDuplicated=objSettings.aAutoAnswersRecepients.Lookup(sFrom,sLastMessage);
	objSettings.aAutoAnswersRecepients.SetAt(sFrom,sMessage);
	if(!objSettings.bNoSend && sAttach.Find("["DEF_AANSM"]")==-1){
		if(objSettings.bAutoAnswer && !bDuplicated && !bFraud){
			if(((iPersonNum!=-1 || objSettings.bARForUnknownUsers) /*Либо юзер есть либо можно неизвестным отвечать*/) && !iGroup /*групповым не является*/){
				// Шлем сообщение...
				CString sAutoanswerReply=Format("%s: %s",_l("Automatic answering machine replies"),objSettings.sAutoReplys[objSettings.iOnlineStatus]);
				SendNetMessage(sFrom,sAutoanswerReply,"","["DEF_AANSM"]");
			}
		}else if(sAttach.Find("["DEF_DELIVERYNOTIFY"]")==-1 && sAttach.Find("["DEF_DELIVERYREQUEST"]")!=-1){
			CString sAutoanswerReply=Format("%s\n%s",_l("Your message has been delivered"),TrimMessage(sMessage,100,4));
			SendNetMessage(sFrom,sAutoanswerReply,"",
				CString("["DEF_NOTID)+CDataXMLSaver::GetInstringPart("["DEF_NOTID,"]",sAttach)+CString("]")+
				CString("["DEF_NOTTIMESTAMP)+CDataXMLSaver::GetInstringPart("["DEF_NOTTIMESTAMP,"]",sAttach)+CString("]")+
				"["DEF_DELIVERYNOTIFY"]");
		}
	}
	if(sLastMessage!="" && sLastMessage==sMessage){// Это дупликат?
		// Смотрим, открыто ли такое же
		CDLG_Note* pNote=objSettings.m_Notes.FindMessageByRecAndText(sFrom,sMessage);
		if(pNote!=NULL){
			if(objSettings.bOnDuplMessage==1){
				// Игнорируем
				return;
			}
			if(objSettings.bOnDuplMessage==0){
				// Предупреждаем
				AddError(_l("Duplicated message"));
				IAnimateWindow(pNote,-1);
				SmartWarning(_l("Received duplicated message from")+Format(" '%s'",objSettings.AddrBook.TranslateIPToNick(sFrom)),_l("Duplicated message"),&objSettings.bOnDuplMessage,DEFAULT_READTIME);
				//Alert(_l("Received duplicated message from")+Format(" '%s'",objSettings.AddrBook.TranslateIPToNick(sFrom)));
				return;
			}
			// Открываем в новом окне...
		}
	}
	// Проверки закончены, открываем в соответсвии с настройками
	int iSenderPersonNum=objSettings.AddrBook.FindPersonByIP(sFrom,FALSE);
	if(iSenderPersonNum==-1){
		iSenderPersonNum=objSettings.AddrBook.FindPersonByNick(sFromNick);
	}
	// Группировать ли сообщение...
	CString sTextPrexif="";
	if(objSettings.bGroupBySender){
		COleDateTime dt=COleDateTime::GetCurrentTime();
		sTextPrexif=Format("<%s %s> ",dt.Format("%H:%M"),sFromNick);
		// Смотрим, есть ли окошко с тем-же отправителем
		CDLG_Note* pNote=objSettings.m_Notes.FindMessageByRecAndText(sFrom,NULL);
		if(pNote){
			// Добавляем текст
			CString sText=pNote->Sets.sText;
			sText=Format("%s\n%s\n\n\n%s",sTextPrexif,sMessage,sText);
			pNote->Sets.sText=sText;
			pNote->SetNoteText();
			pNote->Sets.sAttachment=sAttach+pNote->Sets.sAttachment;
			pNote->ExtractDataFromAttachment();
			pNote->Sets.bIncomingMessage=2;
			pNote->Sets.SetIconNum(theApp.dwMsgBitmap+1);
			pNote->ApplyAllLayouts(TRUE);
			IAnimateWindow(pNote,-1);
			return;
		}
	}
	CNoteSettings* NewNote=new CNoteSettings(objSettings.MessageNoteSettings);
	COleDateTime dtNow=COleDateTime::GetCurrentTime();
	NewNote->bIncomingMessage=2;
	if(sMessageTitle==""){
		sMessageTitle=NewNote->GetMessageTitle(sFromNick,TRUE,sFrom);
	}
	CString sPrevaliTitle=UnescapeString(CDataXMLSaver::GetInstringPart("["DEF_NOTTITLE,"]",sAttach));
	if(sPrevaliTitle!=""){
		sMessageTitle+=": "+sPrevaliTitle;
	}
	NewNote->SetTitle(sMessageTitle);
	NewNote->sText=Format("%s%s",sTextPrexif,sMessage);
	NewNote->sAttachment=sAttach;
	NewNote->ReplyButton=TRUE;
	NewNote->SetIconNum(theApp.dwMsgBitmap+1);
	NewNote->sSenderPerson=objSettings.AddrBook.GetPersonUName(iSenderPersonNum);
	NewNote->bSelectTextOnStart=FALSE;
	NewNote->bIncMsgOpenType=atol(CDataXMLSaver::GetInstringPart("["DEF_OPENTYPE,"]",sAttach));
	if(objSettings.bUseNickOnSend){
		NewNote->sReplyAddressName=sFromNick;
	}else{
		NewNote->sReplyAddressName=sFrom;
	}
	AsyncPlaySound(SOUND_ONMSG);
	OpenNote(NewNote);
}

//>0 - успех, <=0-неуспех
int CMessageWrapper::SendNetMessage_ByProtocol(SendingMessageStruct* str)
{
	if(!str){
		return 0;
	}
	if(str->sRecipient.GetLength()==0){
		// Пользователь типа всегда есть (имя будет зеленым)
		return 1;
	}
	int nRes=0;
	// Разбираемся с протоколом...
	CNetProtocol* pProt=NULL;
	switch(objSettings.iProtocol_Preffered)
	{
	case 0:
		pProt=pMLNet;
		break;
	case 1:
		pProt=pNBNet;
		break;
	case 2:
		pProt=pXPCNet;
		break;
	case 3:
		pProt=pFBNet;
		break;
	case 4:
		pProt=pEMNet;
		break;
	case 5:
	case 6:
		pProt=pRSNet;
		break;
	case 99:
		pProt=pRSNet;
		break;
	}
	CString sRecipientNoProt;
	CString sRecipient=str->sRecipient;
	CString sProt=GetCompNameProtocol(sRecipient, sRecipientNoProt);
	str->sRecipient=sRecipientNoProt;
	//
	if(sProt=="ms"){
		pProt=pMLNet;
	}
	if(sProt=="nb"){
		pProt=pNBNet;
	}
	if(sProt=="xpc"){
		pProt=pXPCNet;
	}
	if(sProt=="fb"){
		pProt=pFBNet;
	}
	if(sProt=="em"){
		pProt=pEMNet;
	}
	if(sProt=="sms"){
		pProt=pRSNet;
	}
	if(sProt=="rss"){
		pProt=pRSNet;
	}
	str->msg_Text.sProtocol=sProt;
	if(pProt){
		switch(str->iMessageType){
		case 0:
			{
				BOOL bLog=1;
				if(str->msg_Text.sMessageAttach.Find("["DEF_DELIVERYNOTIFY"]")==-1 && !str->dwNotificationMsg){
					if(!objSettings.lDisableAttachExtens){
						BOOL bNeedTimeStamp=0;
						if(objSettings.lReqNotifyOfDeliv && str->msg_Text.sMessageAttach.Find("["DEF_DELIVERYREQUEST"]")==-1){
							str->msg_Text.sMessageAttach+="["DEF_DELIVERYREQUEST"]";
							bNeedTimeStamp=1;
						}
						if(objSettings.lReqNotifyOfRead && str->msg_Text.sMessageAttach.Find("["DEF_READREQUEST"]")==-1){
							str->msg_Text.sMessageAttach+="["DEF_READREQUEST"]";
							bNeedTimeStamp=1;
						}
						if(bNeedTimeStamp){
							COleDateTime dt=COleDateTime::GetCurrentTime();
							str->msg_Text.sMessageAttach+=Format("[%s%04i%02i%02i%02i%02i%02i]",DEF_NOTTIMESTAMP,dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
							str->msg_Text.sMessageAttach+=Format("[%s%s]",DEF_NOTID,str->msg_Text.sCorrespondingNoteId);
						}
						if(str->msg_Text.bMessageTitleManual){
							str->msg_Text.sMessageAttach+=Format("[%s%s]",DEF_NOTTITLE,EscapeString2(str->msg_Text.sMessageTitle,"\n[ ]"));
						}
					}
				}else{
					bLog=0;
				}
				nRes=pProt->SendNet_Text(str->sRecipient, &str->msg_Text);
				if(str->msg_Text.infoAlert){
					str->msg_Text.infoAlert->SetCallback(NULL, NULL);
				}
				if(bLog){// Логируем...
					CString sMessageText=str->msg_Text.sMessageBody;
					sMessageText.Replace("\n","\r\n");
					CDataXMLSaver::StripInstringPart("<!--WNSYS'","'-->",sMessageText);
					CDataXMLSaver::StripInstringPart("[DEF_","]",sMessageText);
					CString sToNick=objSettings.AddrBook.TranslateIPToNick(sRecipient);
					BOOL bRes=(nRes!=0);
					CString sLogMessage=Format(
						"\r\n%s: %s\r\n"
						"%s\r\n",
						_l("Status"),bRes?_l("Sent"):(_l("Not sent")+", "+_l("Error")+Format(" '%s'",str->msg_Text.sError)),
						sMessageText);
					objLog.AddMsgLogLineRaw(sToNick,Format("%s. %s: %s (%s)",_l("Outcoming message"), _l("Recipient"), sToNick, sRecipient), sLogMessage);
					if(bRes){
						objSettings.lStatSendedMessages++;
					}
				}
				break;
			}
		case 1:
			{
				Sleep(20+rnd(0,10));
				nRes=pProt->SendNet_StatusExch(str->sRecipient,sProt);
				break;
			}
		default:
			{
				str->msg_Text.sError=_l("Invalid message type");
			}
		}
	}else{
		str->msg_Text.sError=_l(DEFAULT_NOPROTWARN)+" ('"+sProt+"')";
		if(sProt=="fb"){
			str->msg_Text.sError+="\n";
			str->msg_Text.sError+=_l("You should also set accessible\nshared folder before using this protocol");
		}
	}
	return nRes;
}

BOOL CALLBACK TerminateSend(void* param, int iButton)
{
	if(iButton==2){
		return FALSE;
	}
	BOOL bRes=FALSE;
	SendingMessageStruct* str=(SendingMessageStruct*)param;
	//if(AfxMessageBox(_l("Stop sending this message")+"?",MB_YESNO)!=IDYES){
	if(AskYesNo(_l("Stop sending this message")+"?","Stop message sending",NULL,str->msg_Text.infoAlert)==IDYES){
		bRes=TRUE;
		str->dwSendingThread=0;
		str->msg_Text.infoAlert=NULL;// Самоудалится!!
	}
	return bRes;
}

#include <intshcut.h>
#pragma comment(lib,"url.lib")
DWORD WINAPI SendMessageInAnotherThread(LPVOID lpdwstatus)
{
	if(!objSettings.bNetSubsystemInitialized){
		return 0;
	}
	SendingMessageStruct* str=(SendingMessageStruct*)lpdwstatus;
	if(str->msg_Text.sCorrespondingNoteId!=""){
		CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(str->msg_Text.sCorrespondingNoteId);
		if(pNote){
			pNote->SetNoteInfo(NULL,_l("Sending message")+"...");
		}
	}
	//----------------------------------------------------------
	int iSendRes=objSettings.objMessageWrapper->SendNetMessage_ByProtocol(str);
	if(str->dwSendingThread==0){
		delete str;
		return 0;
	}
	if(str->iMessageType==0){
		{// Уведомляем баклана
			if(iSendRes==0){
				BOOL bShowAlert=0;
				if(str->msg_Text.infoAlert && IsWindow(str->msg_Text.infoAlert->GetSafeHwnd())){//&& !str->msg_Text.infoAlert->InCallback()
					str->msg_Text.infoAlert->SetTitle(_l("Message not delivered"));
					str->msg_Text.infoAlert->SetInfoIcon(IDB_BM_MSG_ERR);
					str->msg_Text.infoAlert->SetText(Format("%s %s\n%s:\n",_l("Message to"),TrimMessage(str->sRecipient,30),_l("is not delivered"))+str->msg_Text.sError);
					//if(objSettings.bDelMsgAlertIfNotDeliv)
					{
						str->msg_Text.infoAlert->SetAutoCloseTime(8000,TRUE);
					}
					str->msg_Text.infoAlert->SetOkText(_l("Ok"));
					str->msg_Text.infoAlert->OnInfowndOpen();
				}else{
					bShowAlert=1;
				}
				if(str->msg_Text.sCorrespondingNoteId!=""){
					CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(str->msg_Text.sCorrespondingNoteId);
					if(pNote){
						DWORD dwBG=RGB(255,0,0);
						pNote->SetNoteInfo(&dwBG,_l("Message has not been sent"));
						// Автоповтор?
						pNote->InitAutoSend(0);
						bShowAlert=0;
					}
				}
				if(bShowAlert && !str->dwNotificationMsg){
					// Если нет заметки- кажем алерт
					Alert(Format("%s %s %s:\n",_l("Message to"),TrimMessage(str->sRecipient,30),_l("has not been delivered"))+str->msg_Text.sError,_l("Message not delivered"),8000);
				}
				// Звучим если надо...
				AsyncPlaySound(SOUND_MSGNOTDELIV);
				static long dwLastLANCheck=0;
				objSettings.dwLastSendError=1;
				if(objSettings.objMessageWrapper){
					if(InetIsOffline(0)){
						dwLastLANCheck=GetTickCount();
					}else if(dwLastLANCheck!=0){// && GetTickCount()-dwLastCheck>5000
						dwLastLANCheck=0;
						objSettings.objMessageWrapper->ConnectAll();
					}
				}
			}else{
				objSettings.dtLastSentMsgDate=COleDateTime::GetCurrentTime();
				if(str->msg_Text.infoAlert && IsWindow(str->msg_Text.infoAlert->GetSafeHwnd())){//&& !str->msg_Text.infoAlert->InCallback()
					str->msg_Text.infoAlert->SetTitle(_l("Message sent successfully"));
					str->msg_Text.infoAlert->SetText(Format("%s %s\n%s!",_l("Message to"),TrimMessage(str->sRecipient,30),_l("sent successfully")));
					str->msg_Text.infoAlert->SetInfoIcon(IDB_BM_MSG_OK);
					str->msg_Text.infoAlert->SetAutoCloseTime(2000,TRUE);
					str->msg_Text.infoAlert->SetOkText(_l("Ok"));
				}
				// Звучим если надо...
				AsyncPlaySound(SOUND_MSGDELIV);
				// Удаляем сообщение если надо...
				if(str->msg_Text.sCorrespondingNoteId!=""){
					CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(str->msg_Text.sCorrespondingNoteId);
					if(pNote){
						DWORD dwBG=RGB(0,255,0);
						pNote->SetNoteInfo(&dwBG,_l("Message has been sent"));
						pNote->InitAutoSend(1);
					}
				}
				if(objSettings.bDelMsgIfDelivered || str->msg_Text.bDeleteCorrespondingNote){
					CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(str->msg_Text.sCorrespondingNoteId);
					if(pNote){
						pNote->SendMessage(DELNOTE,1,0);
					}
				}
			}
		}
	}
	if(str->iMessageType==1){
		// Фиксируем знание об online статусе удаленного пользователя
		if(iSendRes!=0){
			// На том конце:
			// Или точно кто-то есть (nRes==1), фиксируем что живой
			// Или точно никого нет (nRes==-1), фиксируем что мертвый
			int iPerson=objSettings.AddrBook.FindPersonByIP(str->sRecipient,FALSE);
			if(iPerson!=-1){
				int& iPersonStatus=objSettings.AddrBook.aPersons[iPerson]->iOnlineStatus;
				if(iPersonStatus<0 || iSendRes==-1){//Если инфа о статусе не иниц. или коннект с userом умер...
					iPersonStatus=(iSendRes==-1)?4:0;
					RefreshUserStatusIcon(iPerson);
				}
			}
		}
	}
	delete str;
	return 0;
}

void RefreshUserStatusIcon(int iPerson)
{
	if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
		pMainDialogWindow->dlgAddBook->RefreshOnLineStatusIcon(iPerson);
	}
}

void RefreshUsersStatusIcons()
{
	if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
		pMainDialogWindow->dlgAddBook->RefreshOnLineStatusIcons();
	}
}

void SendNetMessage(const char* szRecipientComp, const char* sMessage, const char* szMessTitle, const char* sAttach,const char* szIconTitle, const char* sCorNoteId, int iAlertStyle, BOOL bDelCorrNote)
{
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return;
	}
	if(!objSettings.bNetSubsystemInitialized){
		return;
	}
	BOOL bNotificationMessage=(sAttach?(strstr(sAttach,"["DEF_DELIVERYNOTIFY"]")!=0):0);
	BOOL bAamMessage=(sAttach?(strstr(sAttach,"["DEF_AANSM"]")!=0):0);
	// Если в szRecipientComp список адресов, шлем по каждому
	CString sComps=szRecipientComp;
	objSettings.sLastMessageRecipient=sComps;
	while(sComps.GetLength()>0){
		CString sOrdinaryCompNick=GetSingleCompName(sComps);
		if(bNotificationMessage){
			// Для нотификаций отсылаем лишь первому в списке
			sComps="";
		}
		if(sOrdinaryCompNick!=sComps && sComps.GetLength()>sOrdinaryCompNick.GetLength()){
			sComps=sComps.Mid(strlen(sOrdinaryCompNick)+1);
			sComps.TrimLeft();
			sComps.TrimRight();
		}else{
			sComps="";
		}
		BOOL bGroupPerson=FALSE;
		CString sOrdinaryComp=objSettings.AddrBook.TranslateNickToIP(sOrdinaryCompNick,NULL,&bGroupPerson);
		if(bGroupPerson){
			if(bNotificationMessage || bAamMessage){
				// Не отсылаем нотификаций на группу
				continue;
			}
			sComps+="; ";
			sComps+=sOrdinaryComp;
			sOrdinaryComp="";
		}
		if(sOrdinaryComp==""){
			continue;
		}
		CString sNoP;
		CString sProtocol=GetCompNameProtocol(sOrdinaryComp,sNoP);
		if(bNotificationMessage || bAamMessage){
			BOOL bDoNotAllow=1;
			if(bAamMessage || (objSettings.lFBNotifyOfRead && bNotificationMessage)){
				if(sProtocol.Find("xpc")!=-1 || sProtocol.Find("nb")!=-1 || sProtocol.Find("fb")!=-1){
					bDoNotAllow=0;
				}
			}
			if(bDoNotAllow){
				continue;
			}
		}
		InfoWnd* infoWnd=NULL;
		if(szIconTitle && objSettings.bShowMsgProgress){
			InfoWndStruct* info=new InfoWndStruct();
			info->sIconTooltip=szIconTitle;
			info->sTitle=_l("Sending message")+"...";//szIconTitle;
			info->sText=info->sIconTooltip+"\r\n"+_l("Sending message, Please wait");
			info->iIcon=IDB_BM_NEWMESSAGE;
			info->bStartHided=1;//objSettings.bMinConfToTray;
			info->sOkButtonText="";
			if(iAlertStyle!=0){
				info->iStyle=iAlertStyle;
			}
			info->dwStaticPars=DT_CENTER|DT_WORDBREAK;
			infoWnd=Alert(info);
			// Звучим если надо...
			AsyncPlaySound(SOUND_MSGSEND);
		}
		SendingMessageStruct* str=new SendingMessageStruct;
		str->sRecipient=sOrdinaryComp;
		str->iMessageType=0;
		str->msg_Text.infoAlert=infoWnd;
		CString sPreparedMessage=sMessage,sNoteID=sCorNoteId;
		str->msg_Text.sMessageBody=sPreparedMessage;
		str->msg_Text.sMessageAttach=sAttach;
		str->msg_Text.sCorrespondingNoteId=sNoteID;
		str->msg_Text.bDeleteCorrespondingNote=bDelCorrNote;
		str->msg_Text.sMessageTitle=(szMessTitle=="")?(CNoteSettings::GetMessageTitle(sOrdinaryCompNick,TRUE)):szMessTitle;
		str->msg_Text.bMessageTitleManual=(szMessTitle!="")?1:0;
		str->dwNotificationMsg=bNotificationMessage;
		str->msg_Text.sProtocol=sProtocol;
		HANDLE hThread = CreateThread( NULL, 0, SendMessageInAnotherThread, (void*)str, 0, &str->dwSendingThread);
		/*if(str->msg_Text.info){
			info->sOkButtonText=_l("Stop");
			str->msg_Text.info->SetCallback(TerminateSend, (void*)str);
		}*/
		CloseHandle(hThread);
	}
}

void ExchangeOnlineStatusWithPerson(int iPerson,BOOL bForceRefresh)
{
	if(!objSettings.bNetSubsystemInitialized){
		return;
	}
	if(iPerson<0 || iPerson>=objSettings.AddrBook.GetSize()){
		return;
	}
	//
	if(objSettings.AddrBook.aPersons[iPerson]->IsGroup()){
		return;
	}
	if(objSettings.AddrBook.aPersons[iPerson]->sCompName==""){
		objSettings.AddrBook.aPersons[iPerson]->iOnlineStatus=0;
		RefreshUserStatusIcon(iPerson);
		return;
	}
	if(!bForceRefresh && objSettings.AddrBook.aPersons[iPerson]->iOnlineStatus==-2){
		return;
	}
	objSettings.AddrBook.aPersons[iPerson]->iOnlineStatus=-2;
	RefreshUserStatusIcon(iPerson);
	SendingMessageStruct* str= new SendingMessageStruct;
	str->sRecipient=objSettings.AddrBook.TranslateNickToIP(objSettings.AddrBook.aPersons[iPerson]->GetTitle());
	str->iMessageType=1;
	str->dwNotificationMsg=1;
	CString sNoP;
	str->msg_Text.sProtocol=GetCompNameProtocol(str->sRecipient,sNoP);
	HANDLE hThread = CreateThread( NULL, 0, SendMessageInAnotherThread, (void*)str, 0, &str->dwSendingThread);
	//WaitForSingleObject(hThread,1000);// Ждем как минимум секунду - иначе сокеты падлают при нагрузке :(
	CloseHandle(hThread);
}

void ExchangeOnlineStatuses()
{
	if(!objSettings.bNetSubsystemInitialized){
		return;
	}
	if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
		pMainDialogWindow->dlgAddBook->SetWindowIcon();
	}
	for(int i=0;i<objSettings.AddrBook.GetSize();i++){
		if(objSettings.AddrBook.aPersons[i]->IsGroup()){
			continue;
		}
		ExchangeOnlineStatusWithPerson(i);
	}
	objSettings.iStatusUpdateTimeLastTCount=GetTickCount();
}

void ExtractSenderFromText(CString& sRealFrom, CString& sText)
{
	CString sFrom=CDataXMLSaver::GetInstringPart("$=<FROM:",">",sText);
	if(sFrom.GetLength()>0){
		sRealFrom=sFrom;
		sText.Replace(Format("$=<FROM:%s>",sFrom),"");
	}
	sRealFrom.MakeUpper();
}

CString CreateMyStatusStringID(const char* szMyName, BOOL bWithLiks, long* lStatus)
{
	CString sOut="[s";
	long iStatus=objSettings.iOnlineStatus;
	if(lStatus!=NULL){
		iStatus=*lStatus;
	}
	sOut+=char('0'+iStatus);
	sOut+="][m";
	sOut+=objSettings.bAutoAnswer?"A":"";
	sOut+="][p";
	sOut+=szMyName;
	sOut+="]";
	if(bWithLiks){
		sOut+="[lu";
		sOut+=HashedValue(objSettings.sLikUser);
		sOut+="][lk";
		sOut+=HashedValue(objSettings.sLikKey);
		sOut+="]";
	}
	return sOut;
}

int GetUserStatusFromStatusStringID(const char* szStrID)
{
	return atol(CDataXMLSaver::GetInstringPart("[s","]",szStrID));
}

void UpdateUserIconFromStatusStringID(const char* szStrID)
{
	if(!szStrID){
		return;
	}
	int iStatus=GetUserStatusFromStatusStringID(szStrID);
	CString sMode=CDataXMLSaver::GetInstringPart("[m","]",szStrID);
	CString sPerson=CDataXMLSaver::GetInstringPart("[p","]",szStrID);
	CString sLU=CDataXMLSaver::GetInstringPart("[lu","]",szStrID);
	CString sLK=CDataXMLSaver::GetInstringPart("[lk","]",szStrID);
	int iPerson=objSettings.AddrBook.FindPersonByIP(sPerson,TRUE);
	if(iPerson!=-1){
		objSettings.AddrBook.aPersons[iPerson]->iOnlineStatus=iStatus;
		RefreshUserStatusIcon(iPerson);
	}
	objSettings.CheckLiks(sPerson, HashedValue(objSettings.sLikKey)+"/"+HashedValue(objSettings.sLikUser),sLK+"/"+sLU);
}

BOOL CMessageWrapper::isAnyProtocolEnabled()
{
	return (objSettings.bProtocolML_Active|objSettings.bProtocolNB_Active|objSettings.bProtocolXPC_Active|((objSettings.sFBDir!="")?1:0)|objSettings.bProtocolEM_Active);
}

void CMessageWrapper::RecreateMLModule()
{
	if(pMLNet){
		delete pMLNet;
		pMLNet=NULL;
	}
	if(objSettings.bProtocolML_Active){
		pMLNet=new CNetMLProtocol(objSettings.sProtocolML_Slot);
		pMLNet->Start();
	}else{
		pMLNet=NULL;
	}
}

void CMessageWrapper::RecreateNBModule()
{
	if(pNBNet){
		delete pNBNet;
		pNBNet=NULL;
	}
	if(objSettings.bProtocolNB_Active){
		if(!objSettings.lNB_SupportNonStandAlias){
			objSettings.sProtocolNB_Alias=objSettings.sLocalComputerName;
		}
		pNBNet=new CNetNBProtocol(objSettings.sProtocolNB_Alias,objSettings.dwNB_Lana);
		pNBNet->Start();
	}else{
		pNBNet=NULL;
	}
}

void CMessageWrapper::RecreateFBModule()
{
	if(IsEmbedded()){
		objSettings.sFBDir="";
		return;
	}
	if(pFBNet){
		delete pFBNet;
		pFBNet=NULL;
	}
	if(objSettings.sFBDir!=""){
		pFBNet=new CNetFBProtocol(objSettings.sFBDir,objSettings.sFB_ID,objSettings.sFBNick);
		pFBNet->Start();
	}else{
		pFBNet=NULL;
	}
}

void CMessageWrapper::RecreateRSModule()
{
	if(pRSNet){
		delete pRSNet;
		pRSNet=NULL;
	}
	pRSNet=new CNetRssProtocol();
	pRSNet->Start();
}

void CMessageWrapper::RecreateEMModule()
{
	if(pEMNet){
		delete pEMNet;
		pEMNet=NULL;
	}
	if(objSettings.bProtocolEM_Active || IsEmbedded()){
		pEMNet=new CNetEMProtocol();
		pEMNet->Start();
	}else{
		pEMNet=NULL;
	}
}

void CMessageWrapper::RecreateXPCModule()
{
	if(pXPCNet){
		delete pXPCNet;
		pXPCNet=NULL;
	}
	if(objSettings.bProtocolXPC_Active){
		pXPCNet=new CNetXPCProtocol(objSettings.dwProtocolXPC_Port);
		pXPCNet->Start();
	}else{
		pXPCNet=NULL;
	}
}
