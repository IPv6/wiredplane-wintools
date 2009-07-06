// AddrBook.h: interface for the CAddrBook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADDRBOOK_H__75394B54_F8D4_4552_90F8_87EDBB7110E5__INCLUDED_)
#define AFX_ADDRBOOK_H__75394B54_F8D4_4552_90F8_87EDBB7110E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ItemModif.h"
CString GetSingleCompName(CString sComps);
CString GetCompNameNoProtocol(CString sRecipient);
CString GetCompNameProtocol(CString sRecipient, CString& sNameNoProtocol);
int FindProtocolIndex(CString sProt);

class Person: public CItem
{
public:
	Person(const char* szNick, const char* szIP, const char* szDsc, BOOL bGroup, int iTab):CItem(5)
	{
		iTabNumber=iTab;
		dwOptions=bGroup;
		sCompName=szIP;
		SetTitle(szNick);
		sDsc=szDsc;
		iOnlineStatus=-1;
		if(szIP && strstr(szIP,"/em")){
			iOnlineStatus=0;
		}
		hABIRow=NULL;
	};
	~Person(){};
	BOOL ShareItem(BOOL bNewSharingData, int iSectionNum){return FALSE;};
	BOOL IsGroup(){return (dwOptions & 1);};
	void SetGroup(BOOL bGroup){dwOptions = bGroup?(dwOptions | 1):(dwOptions & 0);return;};
	CString GetTextContent()
	{
		return sDsc;
	}
	CString GetItemDsc()
	{
		CString sText=GetTitle();
		if(sCompName!=""){
			sText+="\n";
			sText+=Format("%s: %s",_l("Computer"),sCompName);
		}
		CString sCompSingle=GetSingleCompName(sCompName);
		CString sCompSingleNoProt=GetCompNameNoProtocol(sCompSingle);
		CString sIP=sCompSingleNoProt;
		GetRecipientIP(sIP,TRUE);
		if(sIP!=sCompSingleNoProt){
			sText+="\n";
			sText+=Format("%s: %s",_l("IP Address"),sIP);
		}
		return sText;
	};
	CString GetFindText(){return GetTitle();};
	// Может быть вида
	// COMP:PORT, IP:PORT/tcp, COMP/nb, COMP/ms...
	// tcp - xml-rpc; nb - netbios, ms - mailslot
	CString sCompName;
	// Доп. описание
	CString sDsc;
	// Опции абонента
	// bit 0 - group
	DWORD dwOptions;
	// Онлайновый статус
	// -2: - идет процесс поиска
	// -1: не инициализировано
	// 0-4: статус
	int iOnlineStatus;
	// Указатель на позицию в AddrBook
	HIROW hABIRow;
	// Номер закладки
	int iTabNumber;
};

class CAddrBook  
{
public:
	CStringArray aAddrBookTabs;
	int IsPersonKnown(const char* sName);
	BOOL bNeedResetVisibleContent;
	int GetSize();
	CArray<Person*, Person*> aPersons;
	CString TranslateNickToIP(const char* szNick, int* piNumber=NULL, int* piGroup=NULL);
	CString TranslateIPToNick(const char* szIP, int* piNumber=NULL, int* piGroup=NULL, int bSearchInGroup=2);
	int AddModPerson(const char* szNick, const char* szIP, const char* szDsc, BOOL bGroup, int iTab, BOOL bSendExchange=1);
	void DeletePerson(const char* szNick);
	void DeletePerson(int iPerson);
	int FindPersonByNick(const char* szNick);
	int FindPersonByIP(const char* szIP, BOOL bIncludeGroup=TRUE);
	void LoadFromFile(const char* szFile=0);
	void SaveToFile(const char* szFile=0);
	CAddrBook();
	~CAddrBook();
	void MovePerson(int iIndexFrom, int iIndexTo,BOOL bSwap=0);
	int GetLastPersonInTab(int iTab);
	CString GetPersonUName(int iPerson);
	CString GetTabComps(int iAboveSizeTab);
};

#endif // !defined(AFX_ADDRBOOK_H__75394B54_F8D4_4552_90F8_87EDBB7110E5__INCLUDED_)
