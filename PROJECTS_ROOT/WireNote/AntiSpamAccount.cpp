// AntiSpamAccount.cpp: implementation of the CAntiSpamAccount class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "AntiSpamAccount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define CHECK_ABORT		if(lCurrentStatus<0){throw FALSE;};
CAntiSpamAccount::CAntiSpamAccount()
{
	wnd=NULL;
	lCurrentStatus=0;
	lCurrentSubStatus=0;
	sAccount=DEFACCLOGIN;
	sAccountPsw="";
	sPOP3Server=DEFPOP3SERVER;
	sAccountName="";
	lActive=1;
	lIndex=0;
	dwPOP3Port=110;
	//lAdditionalHeaderLines=0;
	lStartCheckAuto=0;
	dwReceivedMessagesCount=0;
	lRunEmailAfterUserCheck=1;
	dwCheckingPeriod=lAfterCheck=0;
	dtLastCheck.SetStatus(COleDateTime::invalid);
	dtStartCheck.SetStatus(COleDateTime::invalid);
	hWaitForDelete=::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_pcPOP3=new CPop3Connection;
}

CAntiSpamAccount::~CAntiSpamAccount()
{
	if(wnd){
		wnd->DestroyWindow();
		wnd=0;
	}
	if(m_pcPOP3){
		delete m_pcPOP3;
		m_pcPOP3=0;
	}
	CloseHandle(hWaitForDelete);
}

CString CAntiSpamAccount::SerializeToString()
{
	CString sData;
	CDataXMLSaver accountData(sData,"",FALSE);
	accountData.putValue("active",lActive);
	accountData.putValue("hotkey",dwHotkey);
	accountData.putValue("name",sAccountName);
	accountData.putValue("checking-period",dwCheckingPeriod);
	accountData.putValue("after-check",lAfterCheck);
	accountData.putValue("after-check2",lRunEmailAfterUserCheck);
	accountData.putValue("auto-start",lStartCheckAuto);
	accountData.putValue("account",sAccount);
	accountData.putValue("accountpsw",sAccountPsw);
	//accountData.putValue("body-lines",lAdditionalHeaderLines);
	accountData.putValue("pop3-server",sPOP3Server);
	accountData.putValue("pop3-port",dwPOP3Port);
	return accountData.GetResult();
}

void CAntiSpamAccount::DeserializeFromString(CString& sData)
{
	CDataXMLSaver accountData(sData);
	accountData.getValue("active",lActive,0);
	accountData.getValue("hotkey",dwHotkey,0);
	accountData.getValue("name",sAccountName, GetAccountName());
	accountData.getValue("checking-period",dwCheckingPeriod,60);
	accountData.getValue("after-check",lAfterCheck);
	accountData.getValue("after-check2",lRunEmailAfterUserCheck,1);
	accountData.getValue("auto-start",lStartCheckAuto);
	accountData.getValue("account",sAccount);
	accountData.getValue("accountpsw",sAccountPsw);
	//accountData.getValue("body-lines",lAdditionalHeaderLines,objSettings.lAdditionalHeaderLines);
	accountData.getValue("pop3-server",sPOP3Server);
	accountData.getValue("pop3-port",dwPOP3Port,110);
}

BOOL CAntiSpamAccount::ShowAntiSpamWindow()
{
	if(wnd){
		wnd->ShowWindow(SW_SHOW);
		wnd->PostMessage(WM_REFRESHANTISPAM,0,0);
	}else{
		wnd=new CDLG_AntiSpam(this);
		wnd->Create(CDLG_AntiSpam::IDD,NULL);
	}
	SwitchToWindow(wnd->GetSafeHwnd());
	wnd->SetFocus();
	if(lStartCheckAuto && lCurrentStatus==0){
		FORK(CheckMailInTread,this);
	}
	return TRUE;
}

void CAntiSpamAccount::AddServerLogString(const char* sShort, const char* szStr, BOOL bToLast)
{
	if(szStr && *szStr!=0){
		objLog.AddMailLogLine("%s",szStr);
	}
	if(bToLast){
		int iLineEnd=sLogString.Find("\n");
		if(iLineEnd>=0){
			sLogString=sLogString.Mid(iLineEnd+1);
		}
	}
	sLogString=CString(szStr)+"\n"+sLogString;
	sThisCheckLogString=CString(szStr)+"\n"+sThisCheckLogString;
	if(sLogString.GetLength()>1000){
		sLogString=sLogString.Left(1000);
	}
	if(wnd){
		wnd->SetTitle(sShort);
		wnd->PostMessage(WM_REFRESHANTISPAM,0,0);
	}
}

DWORD WINAPI CheckMailInTread(LPVOID pData)
{
	CAntiSpamAccount* pMailbox=(CAntiSpamAccount*)pData;
	if(pMailbox){
		pMailbox->CheckMail();
	}
	return 0;
}

BOOL CAntiSpamAccount::CheckMail()
{
	if(lCurrentStatus!=0){
		// Уже проверяется
		return FALSE;
	}
	::CoInitialize(NULL);
	messages.RemoveAll();
	dwReceivedMessagesCount=0;
	dtStartCheck=COleDateTime::GetCurrentTime();
	AddServerLogString("","");
	sThisCheckLogString="";
	AddServerLogString(_l("Starting"),Format("*** %s %s ***",dtStartCheck.Format(MAILTIMEFRMT),_l("Start check")));
	long lLeavedMessages=0;
	CString sError="";
	// 1. Коннектимся
	try{
		lCurrentStatus=1;
		AddServerLogString(_l("Connecting"),CString("[1/4] ")+_l("Connecting to server")+" '"+sPOP3Server+Format(":%lu",dwPOP3Port)+"' ("+_l("Login")+": "+sAccount+")...");
		m_pcPOP3->SetTimeout(objSettings.dwMailboxTimeout*1000);
		BOOL blRet = m_pcPOP3->Connect(sPOP3Server, sAccount, sAccountPsw, dwPOP3Port, &sError);
		if(blRet == false){
			sError.Replace("\n","");
			sError.Replace("\r","");
			sError.Replace("\t","");
			AddServerLogString(_l("Connect failed"),CString("[...] ")+_l(sError));
			throw TRUE;
		}
		CHECK_ABORT;
		// 1.5 Получаем сообщения
		try{
			RetrieveMessages();
		}catch(int e){
			if(!e){
				AddServerLogString(_l("Aborted"),CString("[...] ")+_l("Aborted by user")+"!");
			}
		}
		// 2. Проверяем
		if(dwReceivedMessagesCount==0){
			AddServerLogString(_l("Skipping"), CString("[2/4] ")+_l("Skipping")+"...");
		}else{
			try{
				lCurrentStatus=2;
				lCurrentSubStatus=0;
				AddServerLogString(_l("Checking"),CString("[2/4] ")+_l("Checking received messages for spam")+"...");
				CheckRetrievedMessages();
				if(wnd){
					wnd->PostMessage(WM_REFRESHANTISPAM,0,1);
				}
				dtLastCheck=COleDateTime::GetCurrentTime();
				if(lAfterCheck==0 && pMainDialogWindow){// Делаем Дью если нужно
					pMainDialogWindow->SendMessage(WM_COMMAND,WM_USER+SHOW_ANTISPAM+lIndex,0);
					//Начинаем заигрывать с юзверем...
					lCurrentSubStatus=1;
					AddServerLogString(_l("Waiting"),CString("[2/4] ")+Format("%s (%s)",_l("Waiting for user"),_l("Mark SPAM messages"))+"...");
					Alert(Format("%s: %s %lu %s (%s: %lu)\n%s",_l("Mailbox check"),_l("Your mailbox contain"),dwReceivedMessagesCount,_l("email(s)"),_l("Retrieved"),messages.GetSize(),_l("Mark SPAM messages and press 'Finish'")),Format("%s: %s",_l("Anti-SPAM"),GetAccountName()),5000);
					ResetEvent(hWaitForDelete);
					AsyncPlaySound(SOUND_ONMAIL);
					DWORD dwWaitTime=dwCheckingPeriod*60*1000;
					if(dwWaitTime<20*60*1000){
						dwWaitTime=20*60*1000;
					}
					DWORD dwRes=WaitForSingleObject(hWaitForDelete,dwWaitTime);
					if(dwRes==WAIT_TIMEOUT){
						AddServerLogString(_l("Waiting"),CString("[2/4] ")+_l("Waiting for user - timed out, continue execution")+"...");
					}
					lCurrentSubStatus=0;
				}
			}catch(int e){
				if(!e){
					AddServerLogString(_l("Aborted"),CString("[...] ")+_l("Aborted by user")+"!");
				}
			}
		}
		// 3. Удаляем
		if(dwReceivedMessagesCount==0){
			AddServerLogString(_l("Skipping"),CString("[3/4] ")+_l("Skipping")+"...");
		}else{
			try{
				lCurrentStatus=3;
				AddServerLogString(_l("Deleting"),CString("[3/4] ")+_l("Deleting marked messages")+"...");
				DeleteMarkedMessages(&lLeavedMessages);
			}catch(int e){
				if(!e){
					AddServerLogString(_l("Aborted"),_l("Aborted by user")+"!");
				}
			}
		}
	}catch(int e){
		if(!e){
			AddServerLogString(_l("Aborted"),CString("[...] ")+_l("Aborted by user")+"!");
		}
	}
	// 4. Отсоединяемся
	BOOL bDiskoRes=FALSE;
	if (m_pcPOP3->m_bConnected){
		AddServerLogString(_l("Disconnecting"),CString("[4/4] ")+_l("Disconnecting from server")+" '"+sPOP3Server+Format(":%lu",dwPOP3Port)+"'...");
		bDiskoRes=m_pcPOP3->Disconnect();
		if(!bDiskoRes){
			AddServerLogString(_l("Disconnect failed"),CString("[4/4] ")+_l("Failed to disconnect from server")+"!");
		}
	}
	// 5. Показываем отчет если надо
	lCurrentStatus=0;
	dtLastCheck=COleDateTime::GetCurrentTime();
	COleDateTime dtStopCheck=COleDateTime::GetCurrentTime();
	dtNextCheck=dtLastCheck+COleDateTimeSpan(0,0,dwCheckingPeriod,0);
	CString sNextCheckInfo="";
	if(dwCheckingPeriod>0 && lActive){
		sNextCheckInfo=Format("%s: %s ",_l("Next check"),dtNextCheck.Format(MAILTIMEFRMT));
	}
	AddServerLogString("",Format("*** %s %s! %s***",dtStopCheck.Format(MAILTIMEFRMT),_l("Check finished"),sNextCheckInfo));
	if((lAfterCheck==1 || (lAfterCheck==0 && lRunEmailAfterUserCheck!=0)) && lLeavedMessages>0){
		if(bDiskoRes){
			AddServerLogString(_l("Email client"),_l("Starting email client")+"...");
			AsyncPlaySound(SOUND_ONMAIL);
			RunDefaultEmailClient();
		}
	}
	if(lAfterCheck==0 && lRunEmailAfterUserCheck!=0 && dwReceivedMessagesCount>0){
		if(wnd){
			wnd->PostMessage(WM_COMMAND,IDC_CLOSE,0);
		}
	}
	long lDeletedCount=dwReceivedMessagesCount-lLeavedMessages;
	if(dwReceivedMessagesCount>0){
		if(lAfterCheck==2){
			CNoteSettings* NewNote=new CNoteSettings(objSettings.NoteSettings);
			NewNote->sText=_l("Check results for")+" "+GetAccountName()+" ("+_l("Server")+": "+sPOP3Server+Format(":%lu",dwPOP3Port)+")\n";
			NewNote->sText+=Format("%s: %lu %s\n",_l("Received"),dwReceivedMessagesCount,_l("email(s)"));
			NewNote->sText+=Format("%s: %lu %s\n",_l("Marked as SPAM"),lDeletedCount,_l("email(s)"));
			NewNote->sText+=Format("\n%s:\n%s",_l("Log file"),sThisCheckLogString);
			NewNote->bTopMost=TRUE;
			NewNote->SetReadOnly(TRUE);
			NewNote->bWordWrap=FALSE;
			NewNote->bNeedFocusOnStart=FALSE;
			NewNote->iViewMode=1;
			NewNote->bSaveOnExit=FALSE;
			AsyncPlaySound(SOUND_ONMAIL);
			OpenNote(NewNote);
		}
	}
	if(pMainDialogWindow && objSettings.lShowSpamReportInTray && (dwReceivedMessagesCount+lDeletedCount>0)){
		CString sText;
		sText+=Format("%s: %lu %s %lu %s",_l("Marked as SPAM"),lDeletedCount,_l("of"),dwReceivedMessagesCount,_l("email(s)"));
		pMainDialogWindow->ShowBaloon(_l("Spam check for")+" "+GetAccountName(),sText);
	}
	objSettings.lStatReceivedMails+=dwReceivedMessagesCount;
	objSettings.lStatReceivedSpamMails+=lDeletedCount;
	return TRUE;
}

/*
CString DecodeEqQuoted(CString& szDecoding)
{
	szDecoding=StringSmartDecode(szDecoding,"=",2);
	szDecoding=StringSmartDecode(szDecoding,"&#",4,";");
}
*/

CString NormalizeEncoding(CString& str, CString sDefCharset, BOOL bDirectDecode=0)
{
	CString m_sCharset;
	if(bDirectDecode==0){
		str=DecodeHeaderValue(str, &m_sCharset);
    }
	if (m_sCharset.GetLength()==0){
		m_sCharset=sDefCharset;
	}
    if (m_sCharset.GetLength()>0){
        str = CCharsetDecoder(m_sCharset).Decode(str);
	}
	// Decode specials
	return m_sCharset;
}

CEmailHeader::CEmailHeader()
{
	bBroken=0;
	bDeleted=0;
	bAttachment=0;
	bFromWhiteList=0;
	dwDownloadedLines=0;
	sFields.SetSize(7);
	lMarkedForDeletion=0;
	hRow=0;
	dwSize=0;
	iIndex=-1;
};

BOOL DeQuotaize(CString& sBodyText)
{
	USES_CONVERSION;
	wchar_t szChar[10]={0};
	BOOL bAnyTagsStripped=FALSE;
	int iFrom=0;
	while(iFrom>=0 && iFrom<sBodyText.GetLength()){
		CString sTextLine=CDataXMLSaver::GetInstringPart("=",0,sBodyText,iFrom,XMLNoConversion|QT_FORWARD2);
		if(iFrom<0){
			break;
		}
		sTextLine.MakeUpper();
		CString sTextToReplace=Format("=%s",sTextLine);
		memset(&szChar,0,sizeof(szChar));
		DWORD dwHexChar=atolx(CString("#")+sTextLine);
		if(dwHexChar>0){
			((WORD*)(&szChar))[0]=LOWORD(dwHexChar);
			((WORD*)(&szChar))[1]=0;
			sTextLine=W2A(szChar);
			int iReplacedItemsCount=sBodyText.Replace(sTextToReplace,sTextLine);
			if(iReplacedItemsCount>0){
				iFrom+=strlen(sTextLine)-strlen(sTextToReplace);
				bAnyTagsStripped=TRUE;
			}else{
				iFrom+=strlen(sTextToReplace);
			}
		}
	}
	return bAnyTagsStripped;
}

// Есть ли аттачменты (Content-Type: text/xml; name="test.xsl" Content-transfer-encoding: base64 Content-Disposition: attachment; filename="test.xsl")
#define CONT_DISP_T	"content-disposition:attachment;"
#define FILE_NAME_T	"filename="
BOOL ExtractAttachedFilenames(CString szContent,CStringArray& aFiles)
{
	int iFrom=0;
	while(iFrom>=0){
		iFrom=szContent.Find(CONT_DISP_T,iFrom);
		if(iFrom<0 || iFrom>=szContent.GetLength()){
			break;
		}
		iFrom+=strlen(CONT_DISP_T);
		/*while(szContent[iFrom]==' '|| szContent[iFrom]=='\t' || szContent[iFrom]=='\r' || szContent[iFrom]=='\n'){
			iFrom++;
		}*/
		iFrom=szContent.Find(FILE_NAME_T,iFrom);
		if(iFrom<0 || iFrom>=szContent.GetLength()){
			break;
		}
		iFrom+=strlen(FILE_NAME_T);
		CString sFileName=szContent.Mid(iFrom).SpanExcluding("\n\r\t");
		sFileName.TrimLeft("\'\"\n\t\r ");
		sFileName.TrimRight("\'\"\n\t\r ");
		aFiles.Add(sFileName);
	}
	return aFiles.GetSize()>0;
}


BOOL CAntiSpamAccount::RetrieveSingleMessage(int i, CEmailHeader* email, int iLines)
{
	AddServerLogString(Format("%s %i/%i",_l("Letter"),i+1,dwReceivedMessagesCount),CString("[...] ")+_l("Retrieving message header")+Format(" #%lu (%s %lu)",i+1,_l("of"),dwReceivedMessagesCount)+"...");
	BOOL bRes=RetrieveSingleMessageRaw(m_pcPOP3, i, email, iLines);
	if(bRes){
		if(!email->bBroken){
			AddServerLogString(Format("%s %i/%i",_l("Letter"),i+1,dwReceivedMessagesCount),CString("[...] ")+_l("Retrieved message")+Format(" #%lu ",i+1)+_l("from")+" '"+email->sFields[MAIL_FROMFIELD]+"' ("+_l("Subject")+": '"+email->sFields[MAIL_SUBJFIELD]+"')",TRUE);
		}
	}else{
		AddServerLogString(Format("%s %i/%i %s",_l("Letter"),i+1,dwReceivedMessagesCount,_l("Error")),CString("[...] ")+_l("Failed to retrieve message header")+"! ("+_l("Message")+Format(" #%lu)",i+1));
	}
	return bRes;
}

BOOL CAntiSpamAccount::RetrieveSingleMessageRaw(CPop3Connection* m_pcPOP3, int i, CEmailHeader* email, int iLines)
{
	if(!email){
		return FALSE;
	}
	email->sID="";
	CPop3Message m_cMessage;
	BOOL blRet=0;
	if(iLines<0){
		blRet = m_pcPOP3->Retrieve(i+1, m_cMessage);
	}else{
		blRet = m_pcPOP3->GetMessageHeader(i+1, m_cMessage, iLines, &email->dwSize);
	}
	if(blRet == false){
		email->bBroken=TRUE;
		email->dwDownloadedLines=0;
	}else{
		email->bBroken=FALSE;
		email->dwDownloadedLines=iLines;
		m_pcPOP3->GetMessageID(i+1, email->sID);
	}
	// Добавляем сообщение к полученным даже в случае неудачи так как порядковый номер сообщения является его же ID
	CString sCType=m_cMessage.GetHeaderItem("Content-type");
	sCType.MakeLower();
	email->sHeader=m_cMessage.GetHeader();
	CString sHeader=email->sHeader+"\n";//"\n" для удобства поиска content-transfer-encoding:
	sHeader.MakeLower();
	email->sFields[MAIL_HEADERS]=sHeader;
	email->sFields[MAIL_CHARSET]=CDataXMLSaver::GetInstringPart("charset=","\n",sHeader);
	if(email->sFields[MAIL_CHARSET]==""){
		// Получем чарсет из тела-для мультипартов
		email->sFields[MAIL_CHARSET]=CDataXMLSaver::GetInstringPart("charset=","\n",m_cMessage.GetBody());
	}
	email->sFields[MAIL_CHARSET].TrimLeft(",; \t\"\n\r");
	email->sFields[MAIL_CHARSET].TrimRight(",; \t\"\n\r");
	// Вытаскиваем поля
	email->sFields[MAIL_TOFIELD]=m_cMessage.GetTo();
	CString sCharset1=NormalizeEncoding(email->sFields[MAIL_TOFIELD],email->sFields[MAIL_CHARSET]);
	email->sFields[MAIL_FROMFIELD]=m_cMessage.GetFrom();
	CString sCharset2=NormalizeEncoding(email->sFields[MAIL_FROMFIELD],email->sFields[MAIL_CHARSET]);
	email->sFields[MAIL_SUBJFIELD]=m_cMessage.GetSubject();
	CString sCharset3=NormalizeEncoding(email->sFields[MAIL_SUBJFIELD],email->sFields[MAIL_CHARSET]);
	email->sFields[MAIL_BODYFIELD]=m_cMessage.GetBody();
	email->sFields[MAIL_BODYFIELD].TrimLeft();
	email->sFields[MAIL_BODYFIELD].TrimRight();
	//Content-Transfer-Encoding:
	CString sContentEncoding=CDataXMLSaver::GetInstringPart("content-transfer-encoding:","\n",sHeader);
	sContentEncoding.TrimLeft();
	sContentEncoding.TrimRight();
	if(sContentEncoding!=""){
		NormalizeEncoding(email->sFields[MAIL_BODYFIELD],sContentEncoding,1);
	}
	//----------------------------------------
	CString sBodyCharset;
	if(sCharset3.GetLength()>0){
		email->sFields[MAIL_CHARSET]=sCharset3;
		sBodyCharset=sCharset3;
	}else if(sCharset2.GetLength()>0){
		email->sFields[MAIL_CHARSET]=sCharset2;
		sBodyCharset=sCharset2;
	}else if(sCharset1.GetLength()>0){
		email->sFields[MAIL_CHARSET]=sCharset1;
		sBodyCharset=sCharset1;
	}else{
		sBodyCharset=email->sFields[MAIL_CHARSET];
	}
	if(sBodyCharset.GetLength()>0){
		email->sFields[MAIL_BODYFIELD] = CCharsetDecoder(email->sFields[MAIL_CHARSET]).Decode(email->sFields[MAIL_BODYFIELD]);
	}
	if(email->sFields[MAIL_CHARSET]!="" && !CCharsetDecoder(email->sFields[MAIL_CHARSET]).isKnown()){
		email->sFields[MAIL_CHARSET]=CString(UNK_CHARSET)+" "+email->sFields[MAIL_CHARSET];
	}
	CString sCharsetInBody=email->sFields[MAIL_CHARSET]+email->sFields[MAIL_BODYFIELD];
	sCharsetInBody.MakeLower();
	email->sFields[MAIL_BDFLD_PR]=email->sFields[MAIL_BODYFIELD];
	if(sCharsetInBody.Find("quoted-printable")!=-1){
		DeQuotaize(email->sFields[MAIL_BDFLD_PR]);
	}
	if( sCType.Find("text/html")!=-1 || sCharsetInBody.Find("text/html")!=-1 || sCharsetInBody.Find("<html>")!=-1 || sCharsetInBody.Replace("&#","&#")>3){
		StripTags(email->sFields[MAIL_BDFLD_PR]);
		DeEntitize(email->sFields[MAIL_BDFLD_PR]);
	}
	email->sFields[MAIL_BDFLD_PR].Replace("\r\n","\n");
	int iDummyCount=0;
	while(email->sFields[MAIL_BDFLD_PR].Replace("\n\n\n","\n\n")){
		iDummyCount++;
	}
	// Есть ли аттачменты (Content-Type: text/xml; name="test.xsl" Content-transfer-encoding: base64 Content-Disposition: attachment; filename="test.xsl")
	CString sPackedInLow=email->sFields[MAIL_BODYFIELD];
	sPackedInLow.MakeLower();
	sPackedInLow.Replace(" ","");
	email->aAttachedFiles.RemoveAll();
	if(sHeader.Find("multipart/mixed")!=-1 && sPackedInLow.Find("content-disposition:attachment")!=-1){
		email->bAttachment=TRUE;
		// Вытаскиваем названия приаттаченных файлов
		ExtractAttachedFilenames(sPackedInLow,email->aAttachedFiles);
	}	
	return TRUE;
}

BOOL CAntiSpamAccount::RetrieveMessages()
{
	if(lCurrentStatus!=1){
		return FALSE;
	}
	long lSize=0;
	dwReceivedMessagesCount=0;
	{// Получаем количество сообщений
		INT iSize=0;
		INT iCount=0;
		AddServerLogString(_l("Handshake"),CString("[...] ")+_l("Retrieving number of messages")+"...");
		BOOL blRet = m_pcPOP3->Statistics(iCount, iSize);
		if(blRet == false){
			AddServerLogString(_l("Handshake failed"),CString("[...] ")+_l("Failed to retrieve number of messages")+"!", TRUE);
			return FALSE;
		}
		lSize=iSize;
		dwReceivedMessagesCount=iCount;
		AddServerLogString(_l("Handshake"),CString("[...] ")+Format("%s %lu %s, %s: %lu",_l("Your mailbox contain"),dwReceivedMessagesCount,_l("email(s)"),_l("Mailbox size"),lSize),TRUE);
		CHECK_ABORT;
	}
	for(long i=0;i<dwReceivedMessagesCount;i++){
		CEmailHeader email;
		RetrieveSingleMessage(i,&email,objSettings.lAdditionalHeaderLines);
		email.iIndex=messages.GetSize();
		{
			CSmartLock l(csMails);
			messages.Add(email);
		}
		CHECK_ABORT;
	}
	return TRUE;
}

BOOL isStringBanned(CString& sCheckWhat, CString& sBannedStr, CString& sExactString, BOOL& bMatchWhiteList, BOOL bUseMask=FALSE)
{
	sExactString="";
	if(sCheckWhat.GetLength()==0){
		return FALSE;
	}
	bMatchWhiteList=FALSE;
	//---------------------
	int iFrom=0;
	while(iFrom!=-1){
		int iNextNewLine=sBannedStr.Find('\n',iFrom+1);
		CString sTextToCheckWith;
		if(iNextNewLine!=-1){
			sTextToCheckWith=sBannedStr.Mid(iFrom,iNextNewLine-iFrom);
		}else{
			sTextToCheckWith=sBannedStr.Mid(iFrom);
		}
		BOOL bWhiteString=FALSE;
		sTextToCheckWith.TrimLeft();
		sTextToCheckWith.TrimRight();
		int iSpecialsPos=sTextToCheckWith.Find("\t");
		if(iSpecialsPos>0){
			CString sOptions=sTextToCheckWith.Mid(iSpecialsPos);
			sTextToCheckWith=sTextToCheckWith.Left(iSpecialsPos);
			if(sOptions.Find(MSG_WHITE_PART)!=-1){
				bWhiteString=TRUE;
			}
		}
		BOOL bUseMaskLocal=bUseMask;
		if(bUseMaskLocal==-1){
			bUseMaskLocal=(sTextToCheckWith.Find('*')!=-1 || sTextToCheckWith.Find('?')!=-1)?TRUE:FALSE;
		}
		if(sTextToCheckWith.GetLength()>0 && ((!bUseMaskLocal && sCheckWhat.Find(sTextToCheckWith)!=-1)||(bUseMaskLocal && PatternMatch(sCheckWhat,sTextToCheckWith)))){
			sExactString=sTextToCheckWith;
			if(bWhiteString){
				bMatchWhiteList=TRUE;
			}
			return TRUE;
		}
		iFrom=iNextNewLine;
	}
	return FALSE;
}

BOOL CAntiSpamAccount::CheckRetrievedMessages(long* lSpamCount, BOOL bRemarkMarked)
{
	CSmartLock l(csMails);
	for(int i=0;i<messages.GetSize();i++){
		CEmailHeader& head=messages[i];
		if(head.lMarkedForDeletion && bRemarkMarked){
			head.lMarkedForDeletion=0;
			head.bFromWhiteList=0;
		}
		head.sWhyMarked="";
		head.sWhyNotMarked="";
	}
	CString sBannedAddr,sBannedSubj,sBannedBody;
	ReadFile(objSettings.sSpamSubjectsFile,sBannedSubj);
	CheckRetrievedMessagesRaw(sBannedSubj, MAIL_SUBJFIELD);
	ReadFile(objSettings.sSpamAddressesFile,sBannedAddr);
	CheckRetrievedMessagesRaw(sBannedAddr, MAIL_FROMFIELD);
	ReadFile(objSettings.sSpamBodyFile,sBannedBody);
	CheckRetrievedMessagesRaw(sBannedBody, MAIL_BODYFIELD);
	ReadFile(objSettings.sSpamHeadFile,sBannedBody);
	CheckRetrievedMessagesRaw(sBannedBody, MAIL_HEADERS);
	CheckRetrievedMessagesRaw(sBannedBody, MAIL_INCOMMON);
	CString sSPAMFileExtensions=objSettings.sSPAMFileExtensions;
	sSPAMFileExtensions.MakeLower();
	sSPAMFileExtensions.Replace(" ","");
	sSPAMFileExtensions.Replace("\t","");
	sSPAMFileExtensions.Replace(",","\n");
	sSPAMFileExtensions.Replace(";","\n");
	CheckRetrievedMessagesRaw(sSPAMFileExtensions, MAIL_ATTACHMENT);
	long lCount=0;
	{// Количетсво меченных
		for(int i=0;i<messages.GetSize();i++){
			CEmailHeader& head=messages[i];
			if(head.bFromWhiteList){
				head.lMarkedForDeletion=0;
			}else if(head.lMarkedForDeletion){
				lCount++;
			}
		}
	}
	if(lSpamCount){
		*lSpamCount=lCount;
	}
	AddServerLogString(_l("Checking"),CString("[...] ")+Format("%s: %lu %s %s %lu",_l("Marked as SPAM"), lCount,_l("email(s)"),_l("of"),messages.GetSize()));
	return TRUE;
}

BOOL isEngChar(char c)
{
	if(c>='a' && c<='z'){
		return TRUE;
	}
	if(c>='A' && c<='Z'){
		return TRUE;
	}
	return FALSE;
}

BOOL isRusChar(char c)
{
	if(c>='а' && c<='я'){
		return TRUE;
	}
	if(c>='А' && c<='Я'){
		return TRUE;
	}
	return FALSE;
}

const char* szFieldNames[]={"'Subject' field","'From' field","'Recipient' field","Message body","Message body","Charset","Mail headers"};
BOOL CAntiSpamAccount::CheckRetrievedMessagesRaw(CString& szBanned, int iFieldNum)
{
	if(lCurrentStatus!=2){
		return FALSE;
	}
	static long lLock=0;
	if(lLock>0){
		// Процесс уже идет
		return FALSE;
	}
	//
	if(objSettings.lRemoveSubjCase){
		szBanned.MakeLower();
	}
	szBanned.TrimLeft();
	szBanned.TrimRight();
	if(objSettings.lRemoveSubjWhiteSpaces){
		szBanned.Replace(" ","");
	}
	//
	for(int i=0;i<messages.GetSize();i++){
		CSmartLock l(csMails);
		CEmailHeader& head=messages[i];
		if(head.bFromWhiteList){
			head.lMarkedForDeletion=0;
			// Уже все ок
			continue;
		}
		BOOL bIsBanned=FALSE;
		BOOL bFromWhiteList=FALSE;
		if(iFieldNum==MAIL_ATTACHMENT){
			for(int i=0;i<head.aAttachedFiles.GetSize();i++){
				CString sExactString;
				BOOL bIsBannedExt=isStringBanned(head.aAttachedFiles[i], szBanned, sExactString, bFromWhiteList, TRUE);
				if(bFromWhiteList){
					bIsBanned=FALSE;
					head.sWhyNotMarked+=Format("- %s '%s'\n",_l("Attachment of the good type"),sExactString);
				}else if(bIsBannedExt){
					bIsBanned|=bIsBannedExt;
					if(bIsBannedExt){
						head.sWhyMarked+=Format("- %s '%s'\n",_l("Attachment of the forbidden type"),sExactString);
					}
				}
			}
		}else if(iFieldNum!=MAIL_INCOMMON){
			// Проверка по конкретному полу письма
			CString sCheckWhat=head.sFields[iFieldNum];
			if(objSettings.lRemoveSubjCase){
				sCheckWhat.MakeLower();
			}
			sCheckWhat.TrimLeft();
			sCheckWhat.TrimRight();
			if(objSettings.lRemoveSubjWhiteSpaces){
				sCheckWhat.Replace(" ","");
				sCheckWhat.Replace("\t","");
			}
			CString sExactString;
			bIsBanned=isStringBanned(sCheckWhat,szBanned,sExactString,bFromWhiteList,objSettings.lAllowMasksInSpamStrings?-1:0);
			if(bFromWhiteList){
				bIsBanned=FALSE;
				head.sWhyNotMarked+=Format("- %s %s '%s'\n",_l(szFieldNames[iFieldNum]),_l("contain"),sExactString);
			}else if(bIsBanned){
				head.sWhyMarked+=Format("- %s %s '%s'\n",_l(szFieldNames[iFieldNum]),_l("contain"),sExactString);
			}
			if(objSettings.lSubjSeqSpaces && iFieldNum==MAIL_SUBJFIELD && !bFromWhiteList){
				CString sSubj=head.sFields[MAIL_SUBJFIELD];
				int iCount=0;
				iCount=sSubj.Replace("     ","");
				if(iCount>0){
					head.sWhyMarked+=Format("- %s %s %s\n",_l(szFieldNames[iFieldNum]),_l("contain"),_l("5 sequential spaces"));
					bIsBanned=TRUE;
				}
				iCount=sSubj.Replace(".....","");
				if(iCount>0){
					head.sWhyMarked+=Format("- %s %s %s\n",_l(szFieldNames[iFieldNum]),_l("contain"),_l("5 sequential dots"));
					bIsBanned=TRUE;
				}
			}
		}else{
			// Проверка по общим признакам
			// Размер письма
			if(long(head.dwSize)>objSettings.lMaxSizeToBeSpam){
				head.sWhyMarked+=Format("- %s %lu\n",_l("Message is larger then"),objSettings.lMaxSizeToBeSpam);
				bIsBanned=TRUE;
			}
			// Кодировка письма
			if(objSettings.sBlockMailsWithThisCharsets!=""){
				CString sSPAMBadCharset=objSettings.sBlockMailsWithThisCharsets;
				sSPAMBadCharset.MakeLower();
				sSPAMBadCharset.Replace(" ","");
				sSPAMBadCharset.Replace("\t","");
				sSPAMBadCharset.Replace(",","\n");
				sSPAMBadCharset.Replace(";","\n");
				CString sCharset=head.sFields[MAIL_CHARSET];
				sCharset.MakeLower();
				CString sExactString;
				bIsBanned=isStringBanned(sCharset,sSPAMBadCharset,sExactString,bFromWhiteList,objSettings.lAllowMasksInSpamStrings?-1:0);
				if(bFromWhiteList){
					bIsBanned=FALSE;
					head.sWhyNotMarked+=Format("- %s %s %s\n",_l(szFieldNames[MAIL_CHARSET]),_l("is in the"),_l("good charset"));
				}else if(bIsBanned){
					head.sWhyMarked+=Format("- %s %s %s\n",_l(szFieldNames[MAIL_CHARSET]),_l("is in the"),_l("forbidden charset"));
				}
			}
			if(objSettings.lBlockMailInUnknownCharset && head.sFields[MAIL_CHARSET].Find(UNK_CHARSET)!=-1){
				head.sWhyMarked+=Format("- %s %s %s\n",_l(szFieldNames[MAIL_CHARSET]),_l("is in the"),_l("unknown charset"));
				bIsBanned=TRUE;
			}
			if(objSettings.lBlockMailWithMessedChars){
				char const* szPos=head.sFields[MAIL_SUBJFIELD];
				while(*szPos!=0 && *(szPos+1)!=0){
					if((isEngChar(*szPos) && isRusChar(*(szPos+1))) ||
						(isRusChar(*szPos) && isEngChar(*(szPos+1)))){
						head.sWhyMarked+=Format("- %s\n",_l("Subject contains charset-messed characters"));
						bIsBanned=TRUE;
						break;
					}
					szPos++;
				}
			}
		}
		head.bFromWhiteList=bFromWhiteList;
		head.lMarkedForDeletion|=bIsBanned;
		CHECK_ABORT;
	}
	return TRUE;
}

BOOL CAntiSpamAccount::DeleteMarkedMessages(long* iLeavedOut)
{
	if(lCurrentStatus!=3){
		return FALSE;
	}
	static long lLock=0;
	if(lLock>0){
		// Процесс уже идет
		return FALSE;
	}
	SimpleTracker Track(lLock);
	SetEvent(hWaitForDelete);
	//
	int iCount=0;
	long iLeaved=0;
	for(int i=0;i<messages.GetSize();i++){
		CSmartLock l(csMails);
		CEmailHeader& head=messages[i];
		if(head.lMarkedForDeletion){
			DeleteMessage(&head);
			if(head.bDeleted>0){
				iCount++;
			}
		}else{
			iLeaved++;
			head.bDeleted=-1;
			AddServerLogString(Format("%s #%i",_l("Leaving"),head.iIndex+1),CString("[...] ")+_l("Leaving message")+Format(" #%lu ",head.iIndex+1)+_l("from")+" '"+head.sFields[MAIL_FROMFIELD]+"' ("+_l("Subject")+": '"+head.sFields[MAIL_SUBJFIELD]+"')");
		}
		CHECK_ABORT;
	}
	AddServerLogString("",CString("[...] ")+Format("%s %lu %s, %lu %s",_l("Deleted"),iCount,_l("email(s)"),iLeaved,_l("leaved on server")));
	if(iLeavedOut){
		*iLeavedOut=iLeaved;
	}
	return TRUE;
}

BOOL CAntiSpamAccount::DeleteMessage(CEmailHeader* item)
{
	if(!item || item->bDeleted!=0){
		// Уже удалено?
		return FALSE;
	}
	AddServerLogString(Format("%s #%i",_l("Killing"),item->iIndex+1),CString("[...] ")+_l("Deleting message")+Format(" #%lu %s %lu ",item->iIndex+1,_l("of"),dwReceivedMessagesCount)+_l("from")+" '"+item->sFields[MAIL_FROMFIELD]+"' ("+_l("Subject")+": '"+item->sFields[MAIL_SUBJFIELD]+"')");
	BOOL bRes=DeleteMessageRaw(m_pcPOP3,item);
	if(bRes == false){
		AddServerLogString(Format("%s #%i",_l("Kill failed"),item->iIndex+1),CString("[...] ")+_l("Failed to delete message")+"! ("+_l("Message")+Format(" #%lu)",item->iIndex+1));
	}else{
		AddServerLogString(Format("%s #%i",_l("Killed"),item->iIndex+1),CString("[...] ")+_l("Message")+Format(" #%lu %s, ",item->iIndex+1,_l("deleted"))+_l("from")+" '"+item->sFields[MAIL_FROMFIELD]+"' ("+_l("Subject")+": '"+item->sFields[MAIL_SUBJFIELD]+"')",TRUE);
	}
	return bRes;
}

BOOL CAntiSpamAccount::DeleteMessageRaw(CPop3Connection* m_pcPOP3, CEmailHeader* item)
{
	item->bDeleted=-1;
	BOOL blRet = FALSE;
	CString sImmediateID;
	m_pcPOP3->GetMessageID(item->iIndex+1,sImmediateID);
	if(item->sID!="" && item->sID!=sImmediateID){
		//AddServerLogString(Format("%s #%i",_l("Kill failed: sync. error"),item->iIndex+1),CString("[...] ")+_l("Failed to delete message")+"! ("+_l("Message")+Format(" #%lu)",item->iIndex+1));
		return FALSE;
	}
	blRet = m_pcPOP3->Delete(item->iIndex+1);
	if(blRet != false){
		item->bDeleted=1;
	}
	return TRUE;
}

CString CAntiSpamAccount::GetAccountName()
{
	CString sRes=_l("Mailbox");
	if(sAccountName.GetLength()>0){
		sRes=sAccountName;
	}else{
		sRes+=" '";
		if(sAccount.GetLength()>0 && sAccount!=DEFACCLOGIN){
			sRes+=sAccount;
		}else{
			sRes+=Format("#%i",lIndex+1);
		}
		sRes+="'";
	}
	return sRes;
}

CString CAntiSpamAccount::GetAccountDsc()
{
	CString sRes=Format("%s #%lu - ",_l("Mailbox"),lIndex+1);
	if(sPOP3Server!="" && sAccount!=""){
		sRes+="'";
		sRes+=sAccount;
		sRes+="' ";
		sRes+=_l("at");
		sRes+=" '";
		sRes+=sPOP3Server;
		sRes+="'";
	}else{
		sRes+=_l("You need to set up login and POP3 data");
	}
	return sRes;
}