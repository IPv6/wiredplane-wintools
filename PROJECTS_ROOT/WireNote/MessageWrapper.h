// MessageWrapper.h: interface for the CMessageWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEWRAPPER_H__DBC51847_AB96_47E5_B317_28DDB1C4CE4A__INCLUDED_)
#define AFX_MESSAGEWRAPPER_H__DBC51847_AB96_47E5_B317_28DDB1C4CE4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DLG_Find.h"
class CNetMessage_Text
{
public:
	CString sMessageBody;
	CString sMessageAttach;
	CString sError;
	CString sCorrespondingNoteId;
	CString sProtocol;
	CString sMessageTitle;
	BOOL bMessageTitleManual;
	// Trais
	InfoWnd* infoAlert;
	BOOL bDeleteCorrespondingNote;
};

class SendingMessageStruct
{
public:
	CString sRecipient;
	int iMessageType;//0-сообщение, 1-обмен статусом, 2-обмен ключами
	DWORD dwSendingThread;
	CNetMessage_Text msg_Text;
	DWORD dwNotificationMsg;
};

class CNetProtocol
{
public:
	BOOL bStarted;
	CNetProtocol()
	{
		bStarted=0;
	}
	virtual BOOL Start()=0;
	virtual BOOL SendNet_Text(const char* szRecipient, CNetMessage_Text* str)=0;
	virtual BOOL SendNet_StatusExch(const char* szRecipient,const char* szProtocol)=0;
	virtual BOOL FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers)=0;
};

class CNetMLProtocol : public CNetProtocol
{
public:
	BOOL Start();
	CString m_sSlot;
	HANDLE SMBMailslot;
	HANDLE hSMBFMailslot;
	DWORD dwSMBThreadID;
	HANDLE hSMBStatusThread;
	DWORD dwSMBFThreadID;
	HANDLE hSMBFStatusThread;
	CNetMLProtocol(const char* szSlot);
	~CNetMLProtocol();
	virtual BOOL SendNet_Text(const char* szRecipient, CNetMessage_Text* str);
	virtual BOOL SendNet_StatusExch(const char* szRecipient,const char* szProtocol);
	virtual BOOL FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers);
};

class CNBListenerInfo;
class CNetNBProtocol : public CNetProtocol
{
public:
	CNBListenerInfo* info1;
	CNBListenerInfo* info2;
	BOOL Start();
	DWORD bUseMessengerServiceToSend;
	int m_iLana;
	CString m_sAlias;
	char achLocalMachineName[MAXMACHINENAME];
	DWORD dwNetBiosThreadID;
	HANDLE hNetBiosStatusThread;
	HANDLE hNetBiosStatusThread2;
	CNetNBProtocol(const char* szAlias, int iLana);
	~CNetNBProtocol();
	int PrepareNBSession(CNCB& m_thisNCB, const char* szTo, const char* szFrom);
	virtual BOOL SendNet_Text(const char* szRecipient, CNetMessage_Text* str);
	virtual BOOL SendNet_StatusExch(const char* szRecipient,const char* szProtocol);
	virtual BOOL FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers);
};

class CNBListenerInfo
{
public:
	CNBListenerInfo()
	{
		bMain=0;
		pParent=NULL;
		memset(achAlias,0,sizeof(achAlias));
	}
	CNetNBProtocol* pParent;
	char achAlias[MAXMACHINENAME+1];
	BOOL bMain;
};

class CNetXPCProtocol : public CNetProtocol
{
public:
	BOOL Start();
	int m_iPort;
	int iServerIndex;
	CNetXPCProtocol(int iPort);
	~CNetXPCProtocol();
	DWORD dwUDPThreadID;
	HANDLE hUDPtatusThread;
	virtual BOOL SendNet_Text(const char* szRecipient, CNetMessage_Text* str);
	virtual BOOL SendNet_StatusExch(const char* szRecipient,const char* szProtocol);
	virtual BOOL FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers);
};

class CNetFBProtocol : public CNetProtocol
{
public:
	BOOL WriteStatusInfo_raw(const char* sTargetDir,const char* sPrefix);
	BOOL ReadStatusInfo(const char* szUserFBID, CString* sUserDataO=NULL, CString* sNickO=NULL, COleDateTime* dtO=NULL);
	BOOL WriteStatusInfo(const char* sTargetDir="",const char* sPrefix="");
	BOOL Start();
	BOOL bStarted;
	HANDLE hWaitEvent;
	long dwSleepTime;
	CString m_sFB_ID;
	CString m_sFBDir;
	CString m_sFBNick;
	DWORD dwFBThreadID;
	HANDLE hFBStatusThread;
	CNetFBProtocol(const char* sFBDir, const char* sFB_ID, const char* sFBNick);
	~CNetFBProtocol();
	virtual BOOL SendNet_Text(const char* szRecipient, CNetMessage_Text* str);
	virtual BOOL SendNet_StatusExch(const char* szRecipient,const char* szProtocol);
	virtual BOOL FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers);
};

class CNetEMProtocol : public CNetProtocol
{
public:
	BOOL Start();
	BOOL bStarted;
	HANDLE hWaitEvent;
	DWORD dwEMThreadID;
	HANDLE hEMStatusThread;
	long lInCheck;
	CNetEMProtocol();
	~CNetEMProtocol();
	virtual BOOL SendNet_Text(const char* szRecipient, CNetMessage_Text* str);
	virtual BOOL SendNet_StatusExch(const char* szRecipient,const char* szProtocol);
	virtual BOOL FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers);
};

class CNetRssProtocol : public CNetProtocol
{
public:
	BOOL SaveTraces();
	BOOL Start();
	BOOL bStarted;
	HANDLE hWaitEvent;
	//DWORD dwEMThreadID;
	//HANDLE hEMStatusThread;
	long lInCheck;
	CNetRssProtocol();
	~CNetRssProtocol();
	virtual BOOL SendNet_Text(const char* szRecipient, CNetMessage_Text* str);
	virtual BOOL SendNet_StatusExch(const char* szRecipient,const char* szProtocol);
	virtual BOOL FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers);
};

class CMessageWrapper  
{
public:
	CNetNBProtocol* pNBNet;
	CNetMLProtocol* pMLNet;
	CNetXPCProtocol* pXPCNet;
	CNetFBProtocol* pFBNet;
	CNetEMProtocol* pEMNet;
	CNetRssProtocol* pRSNet;

	CMessageWrapper();
	virtual ~CMessageWrapper();
	void ConnectAll();
	int SendNetMessage_ByProtocol(SendingMessageStruct* str);
	void RecreateMLModule();
	void RecreateNBModule();
	void RecreateXPCModule();
	void RecreateFBModule();
	void RecreateEMModule();
	void RecreateRSModule();
	BOOL isAnyProtocolEnabled();
};

DWORD WINAPI SendMessageInAnotherThread(LPVOID lpdwstatus);
void SendNetMessage(const char* sRecipientComp, const char* sMessage, const char* szMessTitle, const char* sAttach=NULL,const char* szIconTitle=NULL, const char* sCorNoteId=NULL, int iAlertStyle=0, BOOL bDelCorrNote=FALSE);
void RefreshUsersStatusIcons();// Просто обновить иконки
void RefreshUserStatusIcon(int iPerson);// Просто обновить иконку
void ExchangeOnlineStatuses();
void ExchangeOnlineStatusWithPerson(int i, BOOL bForceRefresh=FALSE);
void OpenMessage(CString sFrom, CString sTo, CString sMessage,CString sRealFrom="", CString sAttach="", CString sTitle="");
void ExtractSenderFromText(CString& sRealFrom, CString& sText);
CString CreateMyStatusStringID(const char* szMyName, BOOL bWithLiks=TRUE, long* lStatus=NULL);
int GetUserStatusFromStatusStringID(const char* szStrID);
void UpdateUserIconFromStatusStringID(const char* szStrID);
void GetRecipientIP(CString& sRecipient, BOOL bLookInCasheOnly=FALSE);
#endif // !defined(AFX_MESSAGEWRAPPER_H__DBC51847_AB96_47E5_B317_28DDB1C4CE4A__INCLUDED_)
