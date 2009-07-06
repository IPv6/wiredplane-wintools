// AddrBook.cpp: implementation of the CAddrBook class.
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAddrBook::CAddrBook()
{
	bNeedResetVisibleContent=TRUE;
}

CAddrBook::~CAddrBook()
{
	for(int i=0;i<aPersons.GetSize();i++){
		if(aPersons[i]){
			delete aPersons[i];
			aPersons[i]=NULL;
		}
	};
}

int CAddrBook::FindPersonByNick(const char* szNick)
{
	for(int i=0;i<aPersons.GetSize();i++){
		if(strcmpi(szNick,aPersons[i]->GetTitle())==0){
			return i;
		}
	};
	return -1;
}

int CAddrBook::FindPersonByIP(const char* szIP, BOOL bIncludeGroup)
{
	CString sIP=szIP;
	sIP.MakeUpper();
	int iIPLen=strlen(sIP);
	if(iIPLen==0){
		return -1;
	}
	for(int i=0;i<aPersons.GetSize();i++){
		if(bIncludeGroup==0 && aPersons[i]->IsGroup()){
			continue;
		}
		CString sComp=aPersons[i]->sCompName;
		sComp.MakeUpper();
		int iCount=0,iCompLen=strlen(sComp);
		while(iCount<iCompLen){
			int iFindedIP=sComp.Find(sIP,iCount);
			if(iFindedIP==-1){
				break;
			}
			iCount=iFindedIP+1;
			char szLastCompChar=0;
			if(iFindedIP+iIPLen<iCompLen){
				szLastCompChar=sComp.GetAt(iFindedIP+iIPLen);
			}
			if(szLastCompChar==':' || szLastCompChar=='/' || szLastCompChar==0 || szLastCompChar==' ' || szLastCompChar==',' || szLastCompChar==';'){
				return i;
			}
		}
	};
	return -1;
}

// Возвращает первый IP ника из приписанных к нему (для группы возвращает все)
CString CAddrBook::TranslateNickToIP(const char* szNick, int* piNumber, int* piGroup)
{
	if(szNick==NULL || *szNick==0){
		return "";
	}
	int iPerson=FindPersonByNick(szNick);
	if(iPerson!=-1){
		CString sComp=aPersons[iPerson]->sCompName;
		if(piNumber)
			*piNumber=iPerson;
		if(piGroup)
			*piGroup=aPersons[iPerson]->IsGroup();
		if(aPersons[iPerson]->IsGroup()){
			// Возвращаем все
			return sComp;
		}else{
			// Возвращаем первый комп в списке
			return sComp.SpanExcluding(" ,;");
		}
	}
	if(piNumber)
		*piNumber=-1;
	return szNick;
}

// Ищет с учетом того что IP может заканчиваться протоколом, может быть в группе и т.д.
CString CAddrBook::TranslateIPToNick(const char* szIP, int* piNumber, int* piGroup, int bSearchInGroup)
{
	int iPerson=FindPersonByIP(szIP,bSearchInGroup==2?0:bSearchInGroup);
	if(iPerson!=-1){
		if(piNumber)
			*piNumber=iPerson;
		if(piGroup)
			*piGroup=aPersons[iPerson]->IsGroup();
		return aPersons[iPerson]->GetTitle();
	}
	CString sIPNoProtocol=szIP;
	sIPNoProtocol=GetCompNameNoProtocol(sIPNoProtocol);// Может найдется без протокола...
	iPerson=FindPersonByIP(sIPNoProtocol,bSearchInGroup==2?0:bSearchInGroup);
	if(iPerson!=-1){
		if(piNumber)
			*piNumber=iPerson;
		if(piGroup)
			*piGroup=aPersons[iPerson]->IsGroup();
		return aPersons[iPerson]->GetTitle();
	}
	if(bSearchInGroup==2){
		// Без группы поискали, поищем еще и в группах...
		return TranslateIPToNick(szIP, piNumber, piGroup, 1);
	}
	if(piNumber)
		*piNumber=-1;
	return szIP;
}

int CAddrBook::AddModPerson(const char* szNick, const char* szIP, const char* szDsc, BOOL bGroup, int iTab, BOOL bSendExchange)
{
	int iPersonNum=FindPersonByNick(szNick);
	if(iPersonNum==-1){
		iPersonNum=aPersons.Add(new Person(szNick,szIP,szDsc,bGroup,iTab));
		if(bSendExchange){
			ExchangeOnlineStatusWithPerson(iPersonNum);
		}
	}else{
		if(bSendExchange){
			if(stricmp(aPersons[iPersonNum]->sCompName,szIP)!=0){
				ExchangeOnlineStatusWithPerson(iPersonNum,TRUE);
			}
		}
		aPersons[iPersonNum]->sCompName=szIP;
		aPersons[iPersonNum]->sDsc=szDsc;
		aPersons[iPersonNum]->SetGroup(bGroup);
	}
	return iPersonNum;
}

void CAddrBook::DeletePerson(int iPerson)
{
	if(iPerson!=-1){
		if(aPersons[iPerson]->hABIRow!=NULL){
			if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
				pMainDialogWindow->dlgAddBook->m_PersonList.DeleteIRow(aPersons[iPerson]->hABIRow);
				aPersons[iPerson]->hABIRow=NULL;
			}
		}
		Person* pTmp=aPersons[iPerson];
		aPersons.RemoveAt(iPerson);
		delete pTmp;
	}
}

void CAddrBook::DeletePerson(const char* szNick)
{
	DeletePerson(FindPersonByNick(szNick));
}

void CAddrBook::LoadFromFile(const char* szFile)
{
	CString sAbThisFile=szFile;
	if(sAbThisFile==""){
		sAbThisFile=getFileFullName(objSettings.sAddrFile);
	}
	CDataXMLSaver adrbFile(sAbThisFile,"address-book",TRUE);
	adrbFile.UncryptBody();
	int iCount=0;
	CString sPersonData,sTmp;
	while(adrbFile.getValue(Format("person%i",iCount++),sPersonData)){
		CDataXMLSaver adrbUser(sPersonData);
		Person* prs=new Person("","","",0,0);
		adrbUser.getValue("item",sTmp);
		prs->ConvertFromString(sTmp,"");
		if(prs->isItemCorrupted()){
			delete prs;
			continue;
		}
		adrbUser.getValue("computer-addr",prs->sCompName);
		adrbUser.getValue("option",prs->dwOptions);
		adrbUser.getValue("dsc",prs->sDsc);
		adrbUser.getValue("tab",prs->iTabNumber);
		aPersons.Add(prs);
	}
	int iABT=0;
	CString sABTTabName;
	while(adrbFile.getValue(Format("adrb-abt%i-name",iABT),sABTTabName)){
		objSettings.AddrBook.aAddrBookTabs.SetAtGrow(iABT,sABTTabName);
		iABT++;
	}
	if(objSettings.AddrBook.aAddrBookTabs.GetSize()==0){
		objSettings.AddrBook.aAddrBookTabs.Add(_l("Main"));
		objSettings.AddrBook.aAddrBookTabs.Add(_l("Other"));
	}
}

void CAddrBook::SaveToFile(const char* szFile)
{
	CString sABFile=szFile;
	if(sABFile==""){
		sABFile=getFileFullName(objSettings.sAddrFile);
	}
	CDataXMLSaver adrbFile(sABFile,"address-book",FALSE);
	for(int i=0;i<aPersons.GetSize();i++){
		CDataXMLSaver adrbUser("");
		adrbUser.putValue("item",aPersons[i]->ConvertToString(),XMLNoConversion);
		adrbUser.putValue("option",aPersons[i]->dwOptions);
		adrbUser.putValue("computer-addr",aPersons[i]->sCompName);
		adrbUser.putValue("dsc",aPersons[i]->sDsc);
		adrbUser.putValue("tab",aPersons[i]->iTabNumber);
		adrbFile.putValue(Format("person%i",i),adrbUser.GetResult(),XMLNoConversion);
	};
	for(int iABT=0;iABT<objSettings.AddrBook.aAddrBookTabs.GetSize();iABT++){
		adrbFile.putValue(Format("adrb-abt%i-name",iABT),objSettings.AddrBook.aAddrBookTabs[iABT]);
	}
}

int CAddrBook::GetLastPersonInTab(int iTab)
{
	int iRes=aPersons.GetSize()-1;
	for(iRes=aPersons.GetSize()-1;iRes>=0;iRes--){
		if(aPersons[iRes] && aPersons[iRes]->iTabNumber==iTab){
			break;
		}
	}
	return iRes;
}

void CAddrBook::MovePerson(int iIndexFrom, int iIndexTo,BOOL bSwap)
{
	if(iIndexFrom==iIndexTo || iIndexFrom<0 || iIndexFrom>=aPersons.GetSize() || iIndexTo<0 || iIndexTo>=aPersons.GetSize()){
		return;
	}
	// Меняем персоны (менять нужно в самой таблице чтобы это отразилось на списках в мессагах)...
	Person* pPerson=aPersons[iIndexFrom];
	if(!pPerson || !aPersons[iIndexTo]){
		return;
	}
	if(bSwap){
		Person* pPerson2=aPersons[iIndexTo];
		aPersons[iIndexTo]=pPerson;
		aPersons[iIndexFrom]=pPerson2;
		HIROW hRowTarget=aPersons[iIndexFrom]->hABIRow;
		aPersons[iIndexFrom]->hABIRow=aPersons[iIndexTo]->hABIRow;
		aPersons[iIndexTo]->hABIRow=hRowTarget;
	}else{
		int iTmpTabNumber=aPersons[iIndexFrom]->iTabNumber;
		aPersons[iIndexFrom]->iTabNumber=aPersons[iIndexTo]->iTabNumber;
		aPersons[iIndexTo]->iTabNumber=iTmpTabNumber;
		HIROW hRowTarget=aPersons[iIndexTo]->hABIRow;
		if(iIndexFrom<iIndexTo){
			for(int i=iIndexTo;i>iIndexFrom;i--){
				aPersons[i]->hABIRow=aPersons[i-1]->hABIRow;
			}
		}else{
			for(int i=iIndexTo;i<iIndexFrom;i++){
				aPersons[i]->hABIRow=aPersons[i+1]->hABIRow;
			}
		}
		aPersons.RemoveAt(iIndexFrom);
		aPersons.InsertAt(iIndexTo,pPerson);
		pPerson->hABIRow=hRowTarget;
	}
	return;
}

int CAddrBook::GetSize()
{
	return aPersons.GetSize();
}

CString GetSingleCompName(CString sComps)
{
	return sComps.SpanExcluding(",;");
}

CString GetCompNameNoProtocol(CString sRecipient)
{
	for(int i=0;i<objSettings.aProtList.GetSize();i++){
		CString sProt="/";
		sProt+=objSettings.aProtList[i];
		if(sRecipient.GetLength()>sProt.GetLength() && sRecipient.Right(sProt.GetLength())==sProt){
			sRecipient=sRecipient.Mid(0,sRecipient.GetLength()-sProt.GetLength());
			return sRecipient;
		}
	}
	if(sRecipient.Find("://")!=-1){
		if(sRecipient.Left(6).CompareNoCase("rss://")==0){
			sRecipient="http://"+sRecipient.Mid(7);
		}
	}
	return sRecipient;
}

CString GetCompNameProtocol(CString sRecipient, CString& sNameNoProtocol)
{
	sNameNoProtocol=GetCompNameNoProtocol(sRecipient);
	if(sNameNoProtocol.GetLength()<sRecipient.GetLength()){
		CString sRes=sRecipient.Mid(sNameNoProtocol.GetLength()+1);
		sRes.MakeLower();
		return sRes;
	}
	if(sNameNoProtocol.Find("+7")==0 || sNameNoProtocol.Find("+8")==0){
		return "sms";
	}
	if(sNameNoProtocol.Find("://")!=-1){
		if(sNameNoProtocol.Left(7).CompareNoCase("http://")==0 || sNameNoProtocol.Left(6).CompareNoCase("rss://")==0){
			return "rss";
		}
	}
	if(sNameNoProtocol.Find("@")!=-1 && sNameNoProtocol.Find(".")!=-1){
		return "em";
	}
	return "";
}

int FindProtocolIndex(CString sProt)
{
	for(int iIndex=0;iIndex<objSettings.aProtList.GetSize();iIndex++){
		if(sProt==objSettings.aProtList[iIndex]){
			return iIndex;
		}
	}
	return -1;
}

int CAddrBook::IsPersonKnown(const char* sName)
{
	if(sName==NULL || strlen(sName)==0){
		return -1;
	}
	int iPersonNumber=0;
	objSettings.AddrBook.TranslateNickToIP(sName,&iPersonNumber);
	if(iPersonNumber==-1){
		objSettings.AddrBook.TranslateIPToNick(sName,&iPersonNumber);
	}
	return iPersonNumber;
}

CString CAddrBook::GetPersonUName(int iPerson)
{
	if(iPerson<0 || iPerson>=aPersons.GetSize()){
		return "";
	}
	return objSettings.AddrBook.aPersons[iPerson]->GetTitle();
}

CString CAddrBook::GetTabComps(int iAboveSizeTab)
{
	if(iAboveSizeTab>=aPersons.GetSize()){
		iAboveSizeTab-=aPersons.GetSize();
	}
	CString sRes;
	for(int i=0;i<aPersons.GetSize();i++){
		if(aPersons[i] && aPersons[i]->iTabNumber==iAboveSizeTab){
			sRes+=aPersons[i]->sCompName+";";
		}
	}
	sRes.TrimRight("; ");
	return sRes;
}