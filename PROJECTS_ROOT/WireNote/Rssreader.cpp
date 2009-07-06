// Rssreader.cpp: implementation of the CRssreader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "..\SmartWires\SystemUtils\xml\xmlite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL StripTags(CString& sBodyText);
BOOL DeEntitize(CString& sBodyText);
CCriticalSection csRssFeeds;
CMapStringToString aLoadedRss;
BOOL CNetRssProtocol::Start()
{
	bStarted=1;
	CString sFile=GetUserFolder()+"RssLatest.txt";
	if(isFileExist(sFile)){
		CFile file(sFile, CFile::modeRead);	// создать файл
		CArchive ar(&file, CArchive::load);
		aLoadedRss.Serialize(ar);
    }
	return TRUE;
}

CNetRssProtocol::CNetRssProtocol()
{
	lInCheck=0;
	bStarted=0;
	hWaitEvent=::CreateEvent(NULL,FALSE,FALSE,"");
}

BOOL CNetRssProtocol::SaveTraces()
{
	static CCriticalSection cs;
	CSmartLock SL(&cs,TRUE);
	CString sFile=GetUserFolder()+"RssLatest.txt";
	CFile file(sFile, CFile::modeWrite | CFile::modeCreate);	// создать файл
    CArchive ar(&file, CArchive::store);
    aLoadedRss.Serialize(ar);
    return 1;
}

CNetRssProtocol::~CNetRssProtocol()
{
	bStarted=0;
	SaveTraces();
	CloseHandle(hWaitEvent);
}

CString GetNDCRulFile(CString& sNDC,CString& sMobnum,DWORD& dwMaxLen)
{
	dwMaxLen=0;
	static CFileInfoArray dir;
	static BOOL bAdded=0;
	static CMapStringToString aNDCIndex;
	if(bAdded==0){
		bAdded=1;
		dir.AddDir(GetAppFolder()+"Sms/","*.rul",TRUE,0,FALSE);
		for(int i=0;i<dir.GetSize();i++){
			CString sItemPath=dir[i].GetFilePath();
			CString sContent;
			ReadFile(sItemPath,sContent);
			sContent.Replace("\r","");
			sContent+="\n";
			CString sLen=CDataXMLSaver::GetInstringPart("SmsMaxLength=","\n",sContent);
			CString sNDCs=CDataXMLSaver::GetInstringPart("SmsNdcList=","\n",sContent);
			CString sName=CDataXMLSaver::GetInstringPart("WndTitle=","\n",sContent);
			if(sNDCs!=""){
				CStringArray aNDCs;
				ConvertToArray(sNDCs,aNDCs,',');
				for(int j=0;j<aNDCs.GetSize();j++){
					CString sOld=aNDCIndex[aNDCs[j]];
					if(sOld!=""){
						sOld+="|";
					}
					sOld+=sItemPath+"\t"+sLen+"\t"+sName;
					aNDCIndex.SetAt(aNDCs[j],sOld);
				}
			}
		}
	}
	CString sNDCData=aNDCIndex[sNDC];
	if(sNDCData.Find("|")!=-1){
		// Выбор :(
		CStringArray aOperators;
		ConvertToArray(sNDCData,aOperators,'|');
		CStringArray aOperatorsNames;
		int i=0;
		for(i=0;i<aOperators.GetSize();i++){
			CStringArray aDats;
			ConvertToArray(aOperators[i],aDats,'\t');
			if(aDats.GetSize()>=3){
				aOperatorsNames.Add(aDats[2]);
			}
		}
		i=SelectFromMenu(aOperatorsNames,0);
		if(i>=0){
			sNDCData=aOperators[i];
		}else{
			return "000";
		}
	}
	if(sNDCData!=""){
		CStringArray aDats;
		ConvertToArray(sNDCData,aDats,'\t');
		if(aDats.GetSize()>=2){
			dwMaxLen=atol(aDats[1]);
			return aDats[0];
		}
	}
	// Доверяем гуглу?
	CString sRetFile=GetAppFolder()+"Sms/default.rul";
	if(isFileExist(sRetFile)){
		CString sContent;
		ReadFile(sRetFile,sContent);
		CString sCList=CDataXMLSaver::GetInstringPart("SmsCarriersList=","\n",sContent);
		CStringArray aCarriers;
		ConvertToArray(sCList,aCarriers,',');
		int i=SelectFromMenu(aCarriers,0);
		if(i>0){
			dwMaxLen=1000;
			sMobnum=sNDC+sMobnum;
			sNDC=Format("%i",i);
			return sRetFile;
		}
		if(i<0){
			return "000";
		}
	}
	return "";
}

BOOL CNetRssProtocol::SendNet_Text(const char* szRecipientIn, CNetMessage_Text* str)
{
	CString sRawAddr=GetCompNameNoProtocol(szRecipientIn);
	if(str->sProtocol=="sms"){
		sRawAddr.Replace("-","");
		sRawAddr.Replace(" ","");
		sRawAddr.Replace(".","");
		CString sApp=GetAppFolder()+"Sms/smssend.exe";
		if(isFileExist(sApp)){
			sRawAddr.TrimLeft("+");
			if(sRawAddr.Left(1)=="7" || sRawAddr.Left(1)=="8"){
				sRawAddr=sRawAddr.Mid(1);
			}
			if(sRawAddr.GetLength()!=10){
				Alert(_l("Incorrect mobile number. Mobile number should be in the form of +7XXXXXXXXXX"),_l("Error"));
			}else{
				CString sNDC=sRawAddr.Left(3);
				CString sMobnum=sRawAddr.Mid(3);
				int iLen2=0;
				int iLen4=0;
				CString sMessage=EscapeString(str->sMessageBody,1,"\\/:?&\"'\n\r\t%=",&iLen2,&iLen4);
				int iLenToCompare=(str->sMessageBody.GetLength()-iLen2-iLen4)+2*(iLen2+iLen4);
				DWORD dwMaxLen=0;
				CString sNDCFile=GetNDCRulFile(sNDC,sMobnum,dwMaxLen);
				if(sNDCFile=="" || sNDCFile=="000"){
					if(sNDCFile==""){
						Alert(_l("Sorry, your provider is not supported this time.\nPlease contact support@wiredplane.com for details"),_l("Error"));
					}
				}else{
					CString sParams=" scriptex=\""+sNDCFile+"\"";
					sParams+=" NDC=";
					sParams+=sNDC;
					sParams+=" MOBNUM=";
					sParams+=sMobnum;
					sParams+=" MESSAGE=";
					sParams+=sMessage;
					if(sParams.GetLength()>MAX_PATH*2 || iLenToCompare>dwMaxLen){
						Alert(_l("Sorry, message is too long"),_l("Error"));
					}else{
						STARTUPINFO su;
						PROCESS_INFORMATION pi;
						ZeroMemory(&su, sizeof(su));
						ZeroMemory(&pi, sizeof(pi));
						GetStartupInfo(&su);
						su.cb = sizeof(su);
						su.dwFlags = STARTF_USESHOWWINDOW;
						su.wShowWindow = SW_SHOWNORMAL;
						su.lpDesktop = NULL;
						char szPath[MAX_PATH]={0};
						strcpy(szPath,sApp);
						char szPars[MAX_PATH*2]={0};
						strcpy(szPars,sParams);
						if(::CreateProcess(szPath, szPars, NULL,NULL, false, NULL, NULL, GetAppFolder(), &su, &pi)){
							DWORD dwRes=WaitForSingleObject(pi.hProcess,160000);
							DWORD iExCode=0;
							GetExitCodeProcess(pi.hProcess,&iExCode);
							if(pi.hProcess){
								CloseHandle(pi.hProcess);
							}
							if(dwRes==WAIT_OBJECT_0){
								if(iExCode==1){
									// Палучилось!!!
									objSettings.iSMSCount++;
									if(objSettings.iSMSCount>100)
									{
										Alert(Format("Sms #%i: %s\n%s!",_l("You are using SMS feature extensively"),objSettings.iSMSCount,_l("If you don`t want to see this message again\nplease order WireNote Pro")),_l("Registration"));
									}
									return TRUE;
								}
							}
						}
					}
				}
			}
		}
		return FALSE;
	}
	if(sRawAddr.Left(7).CompareNoCase("http://")==0){
		CSmartLock SL(&csRssFeeds,TRUE);
		CString sLnewsLink=aLoadedRss[sRawAddr];
		sLnewsLink=sLnewsLink.SpanExcluding("\t");
		if(sLnewsLink==""){
			sLnewsLink=sRawAddr;
		}
		WNSubstMyVariables(sLnewsLink,"web-rss");
		ShellExecute(0,"open",sLnewsLink,NULL,NULL,SW_SHOWNORMAL);
		return 1;
	}
	Alert(_l("You can not send messages through this protocol\nUse it for receiving RSS news as messages"),_l(NO_SEND_TITLE));
	return 0;
}

BOOL CNetRssProtocol::SendNet_StatusExch(const char* szRec,const char* szProtocol)
{
	if(CString(szProtocol)=="sms"){
		return 1;
	}
	if(objSettings.lRSSLocUser){
		if(isScreenSaverActiveOrCLocked()){
			return -1;
		}
	}
	CString szRecipient=GetCompNameNoProtocol(szRec);
	int iPerson=objSettings.AddrBook.FindPersonByIP(szRecipient,TRUE);
	if(iPerson!=-1){
		if(objSettings.AddrBook.aPersons[iPerson]->IsGroup()){
			// Групповой-заблокирован
			return -1;
		};
	}
	CWebWorld url;
	CString sRSS=url.GetWebPage(szRecipient);
	if(sRSS==""){
		// Не доступен
		return -1;
	}
	#ifdef _DEBUG
	CString sFile=CString("lastblog")+szRecipient+".xml";//http://davydov.blogspot.com/atom.xml
	MakeSafeFileName(sFile);
	SaveFile(CString("c://")+sFile,sRSS);
	#endif
	int iStatus=1;
	XDoc* feedDoc=parseXML(sRSS);
	XNode* feed=0;
	if(feedDoc){
		feed=feedDoc->GetRoot();
		if(!feed){
			delete feedDoc;
			return -1;
		}
		CString sLnewsBuildDate=aLoadedRss[szRecipient];
		int iMaxNews=objSettings.dwProtocolRSSMAxNews;
		if(sLnewsBuildDate==""){
			iMaxNews=5;
		}
		CString sLnewsBuildDateToSet="";
		BOOL bAtom=0;
		CString sGlobalTitle;
		if(feed->GetChild("channel",1)==GetEmptyNode() && feed->GetChild("title",1)!=GetEmptyNode()){
			// Atom!!!
			bAtom=1;
			sGlobalTitle=feed->GetChild("title",1)->value;
		}else{
			sGlobalTitle=feed->GetChild("channel",1)->GetChild("title",1)->value;
		}
		CString sFrom="";
		if(iPerson!=-1){
			sFrom=objSettings.AddrBook.aPersons[iPerson]->GetTitle();
		}else{
			sFrom=sGlobalTitle+"/rss";
		}
		CXMLNodes feedItems;
		if(bAtom){
			feed->FindByTagName("entry",FALSE,feedItems);
		}else{
			feed->GetChild("channel",1)->FindByTagName("item",FALSE,feedItems);
		}
		int iCount=0;
		int iLenCount=feedItems.GetSize();
		if(iLenCount==0){
			// Может rdf???
			feed->FindByTagName("item",FALSE,feedItems);
			iLenCount=feedItems.GetSize();
		}
		CString sSummary;
		while(iCount<iLenCount && iCount<iMaxNews){
			XNode* item=feedItems[iCount];
			CString sPubDate;
			if(bAtom){
				sPubDate=item->GetChild("issued",1)->value;
			}else{
				sPubDate=item->GetChild("pubDate",1)->value;
				if(sPubDate==""){
					//rdf???
					sPubDate=item->GetChild("dc:date",1)->value;
				}
			}
			//sPubDate=UnescapeString(sPubDate);
			StripTags(sPubDate);
			DeEntitize(sPubDate);

			CString sTitle;
			sTitle=item->GetChild("title",1)->value;
			//sTitle=UnescapeString(sTitle);
			StripTags(sTitle);
			DeEntitize(sTitle);
			if(sTitle==""){
				sTitle=sGlobalTitle;
			}
			
			CString sAuthor;
			if(bAtom){
				LPXNode pAut;
				pAut=item->GetChild("author",1);
				if(pAut->iBeginPos!=-1 && pAut->iEndPos!=-1){
					sAuthor=sRSS.Mid(pAut->iBeginPos,pAut->iEndPos-pAut->iBeginPos);
				}
			}else{
				sAuthor=item->GetChild("author",1)->value;
				if(sAuthor==""){
					//rdf???
					sAuthor=item->GetChild("dc:creator",1)->value;
				}
			}
			//sAuthor=UnescapeString(sAuthor);
			StripTags(sAuthor);
			DeEntitize(sAuthor);
			if(sAuthor!=""){
				sTitle+=" (";
				sTitle+=sAuthor;
				sTitle+=")";
			}
			
			LPXNode pDesk;
			if(bAtom){
				pDesk=item->GetChild("content",1);
			}else{
				pDesk=item->GetChild("description",1);
			}
			CString sDesk;
			if(pDesk->iBeginPos!=-1 && pDesk->iEndPos!=-1){
				sDesk=sRSS.Mid(pDesk->iBeginPos,pDesk->iEndPos-pDesk->iBeginPos);
			}
			/*
			if(sDesk.Find("%20")!=-1){
				sDesk=UnescapeString(sDesk);
			}
			*/
			CString sDeskL=sDesk;
			sDeskL.MakeLower();
			if(sDeskL.Find(";div")!=-1 || sDeskL.Find(";span")!=-1){
				// Для извращенных...
				DeEntitize(sDesk);
				StripTags(sDesk);
			}else{
				// Это по правильному
				StripTags(sDesk);
				DeEntitize(sDesk);
			}
			sDesk.TrimLeft();
			sDesk.TrimRight();

			CString sLink;
			if(bAtom){
				CXMLNodes feedLinks;
				item->FindByTagName("link",FALSE,feedLinks);
				for(int j=0;j<feedLinks.GetSize();j++){
					XNode* itemLnk=feedLinks[j];
					sLink=itemLnk->GetAttr("href")->value;
					CString sType=itemLnk->GetAttr("type")->value;
					sType.MakeLower();
					if(sType.Find("text")!=-1){
						break;// Наша линка!
					}
				}
			}else{
				sLink=item->GetChild("link",1)->value;
			}
			sLink=UnescapeString(sLink);
			StripTags(sLink);
			DeEntitize(sLink);
			sLink.TrimLeft();
			sLink.TrimRight();
			
			{
				CSmartLock SL(&csRssFeeds,TRUE);
				CString sRnewsBuildDate=sLink+"\t"+sPubDate+" "+sTitle+" "+sDesk;
				if(sLnewsBuildDate==sRnewsBuildDate){
					if(iCount==0){
						iStatus=1;
					}
					break;
				}
				if(sLnewsBuildDateToSet==""){// Запомнили самую первую новость
					sLnewsBuildDateToSet=sRnewsBuildDate;
				}
				/*
				#ifdef _DEBUG
				else{
					sDesk+="\nOld id-text:"+sLnewsBuildDate;
					sDesk+="\nNew id-text:"+sRnewsBuildDate;
				}
				#endif
				*/
			}

			if(sPubDate!=""){
				COleDateTime dt;
				dt.ParseDateTime(sPubDate);
				if(dt.GetStatus()==COleDateTimeSpan::valid){
					sTitle=dt.Format("%x %X. ")+sTitle;
				}
			}
			CString sNews=sTitle;
			sNews+="\n";
			if(sDesk!=""){
				sNews+="\n";
				sNews+=sDesk;
			}
			if(sLink!=""){
				sNews+="\n\n";
				sNews+=_l("Read more")+": ";
				sNews+=sLink;
			}
			CDataXMLSaver::Xml2Str(sNews);
			if(objSettings.lRSSSummar){
				sSummary+="\n";
				sSummary+="\n";
				sSummary+=sTitle;
				sSummary+="\n";
				sSummary+=_l("Read more")+": ";
				sSummary+=sLink;
			}else{
				CString sAttach="";
				if(objSettings.dwProtocolRSSOType){
					sAttach+=Format("["DEF_OPENTYPE"%i]",objSettings.dwProtocolRSSOType);
				}
				OpenMessage(sFrom,"",sNews,"",sAttach,sGlobalTitle);
			}
			iCount++;
		}
		if(sSummary!=""){
			CString sAttach="";
			if(objSettings.dwProtocolRSSOType){
				sAttach+=Format("["DEF_OPENTYPE"%i]",objSettings.dwProtocolRSSOType);
			}
			sSummary.TrimLeft();
			OpenMessage(sFrom,"",sSummary,"",sAttach,sGlobalTitle);
		}
		if(feedDoc){
			delete feedDoc;
		}
		if(sLnewsBuildDateToSet!=""){
			aLoadedRss.SetAt(szRecipient,sLnewsBuildDateToSet);
		}
	}
	if(iPerson!=-1){
		objSettings.AddrBook.aPersons[iPerson]->iOnlineStatus=iStatus;
		RefreshUserStatusIcon(iPerson);
	}
	return 0;
}

BOOL CNetRssProtocol::FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers)
{
	return FALSE;
}
