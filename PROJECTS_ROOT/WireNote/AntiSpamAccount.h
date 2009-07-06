// AntiSpamAccount.h: interface for the CAntiSpamAccount class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANTISPAMACCOUNT_H__0B20B3AD_A0C9_49CE_9450_98CD84465147__INCLUDED_)
#define AFX_ANTISPAMACCOUNT_H__0B20B3AD_A0C9_49CE_9450_98CD84465147__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DLG_AntiSpam.h"
#include "..\SmartWires\SystemUtils\Pop3.h"

#define MAIL_SUBJFIELD	0
#define MAIL_FROMFIELD	1
#define MAIL_TOFIELD	2
#define MAIL_BODYFIELD	3
#define MAIL_BDFLD_PR	4
#define MAIL_CHARSET	5
#define MAIL_HEADERS	6
#define MAIL_INCOMMON	100
#define MAIL_ATTACHMENT	101
#define UNK_CHARSET		"Unknown"
class CEmailHeader
{
public:
	CEmailHeader();
	~CEmailHeader(){};
	CEmailHeader& operator=(CEmailHeader& CopyObj)
	{
		iIndex=CopyObj.iIndex;
		hRow=CopyObj.hRow;
		sHeader=CopyObj.sHeader;
		bBroken=CopyObj.bBroken;
		bAttachment=CopyObj.bAttachment;
		dwSize=CopyObj.dwSize;
		bDeleted=CopyObj.bDeleted;
		sWhyMarked=CopyObj.sWhyMarked;
		sWhyNotMarked=CopyObj.sWhyNotMarked;
		lMarkedForDeletion=CopyObj.lMarkedForDeletion;
		dwDownloadedLines=CopyObj.dwDownloadedLines;
		sID=CopyObj.sID;
		bFromWhiteList=CopyObj.bFromWhiteList;
		for(int i=0;i<CopyObj.sFields.GetSize();i++){
			sFields[i]=CopyObj.sFields[i];
		}
		for(int j=0;j<CopyObj.aAttachedFiles.GetSize();j++){
			aAttachedFiles.SetAtGrow(j,CopyObj.aAttachedFiles[j]);
		}
		return *this;
	}
public:
	DWORD dwDownloadedLines;
	DWORD dwSize;
	BOOL bBroken;
	BOOL bAttachment;
	CStringArray aAttachedFiles;
	int bDeleted;//(0)-не удалено, (1)-удалено, (-1)-оставлено на сервере
	CString sHeader;
	CStringArray sFields;
	long lMarkedForDeletion;
	long bFromWhiteList;
	CString sWhyMarked;
	CString sWhyNotMarked;
	HIROW hRow;
	int iIndex;
	CString sID;
};

class CAntiSpamAccount  
{
public:
	CCriticalSection csMails;
	long dwReceivedMessagesCount;
	HANDLE hWaitForDelete;
	// Сохраняемые данные
	CString sAccountName;
	CString sAccount;
	CString sAccountPsw;
	CString sPOP3Server;
	long lStartCheckAuto;
	long dwPOP3Port;
	long lActive;
	long dwCheckingPeriod;
	long lAfterCheck;
	long lRunEmailAfterUserCheck;
	//long lAdditionalHeaderLines;
	CIHotkey dwHotkey;
	// Функции и другое
	CString GetAccountName();
	CString GetAccountDsc();
	CAntiSpamAccount();
	virtual ~CAntiSpamAccount();
	CString SerializeToString();
	void DeserializeFromString(CString& sData);
	BOOL ShowAntiSpamWindow();
	CDLG_AntiSpam* wnd;
	CPop3Connection* m_pcPOP3;
	//------------------
	long lIndex;
	long lCurrentStatus;//0-ждем, 1-скачиваем, 2-обрабатываем, 3-удаляем помеченные сообщения, -1-останавливаем обработку
	long lCurrentSubStatus;
	COleDateTime dtLastCheck;
	COleDateTime dtNextCheck;
	COleDateTime dtStartCheck;
	CString sLogString;
	CString sThisCheckLogString;
	void AddServerLogString(const char* sShort, const char* szStr, BOOL bToLast=FALSE);
	BOOL RetrieveSingleMessage(int i, CEmailHeader* email, int iLines);
	static BOOL RetrieveSingleMessageRaw(CPop3Connection* m_pcPOP3, int i, CEmailHeader* email, int iLines);
	BOOL CheckRetrievedMessages(long* lCount=NULL, BOOL bRemarkMarked=TRUE);
	BOOL CheckRetrievedMessagesRaw(CString& szBanned, int iFieldNum);
	BOOL DeleteMarkedMessages(long* iLeavedOut=NULL);
	BOOL DeleteMessage(CEmailHeader* item);
	static BOOL DeleteMessageRaw(CPop3Connection* m_pcPOP3, CEmailHeader* item);
	BOOL RetrieveMessages();
	BOOL CheckMail();
	CArray<CEmailHeader,CEmailHeader&> messages;
};

DWORD WINAPI CheckMailInTread(LPVOID pData);
#endif // !defined(AFX_ANTISPAMACCOUNT_H__0B20B3AD_A0C9_49CE_9450_98CD84465147__INCLUDED_)
