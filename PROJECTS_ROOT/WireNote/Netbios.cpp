
// Netbios.cpp: implementation of the Netbios class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "..\SmartWires\SystemUtils\SysService.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
#define SMB_HEADER_SIZE		32
#define NBHEADER_COMMAND	4

int aNBErrs_Code[]={0x00,0x01,0x03,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0d,0x0e,0x0f,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x21,0x22,0x23,0x24,0x26,0x30,0x34,0x35,0x36,0x37,0x38,0x39,0x3B,0x3C,0x3f,0x40,NERR_NetworkError,ERROR_ACCESS_DENIED,ERROR_INVALID_PARAMETER,ERROR_NOT_SUPPORTED,NERR_NameNotFound};
const char* aNBErrs_Dsc[]={"Good return","Illegal buffer length","Illegal command","Command timed out","Message incomplete, issue another command","Illegal buffer address","Session number out of range","No resource available","Session closed","Command cancelled","Duplicate name","Name table full","No deletions, name has active sessions","Local session table full","Remote session table full","Illegal name number","No callname","Can`t put * in NCB_NAME","Name in use on remote adapter","Name deleted","Session ended abnormally","Name conflict detected","Interface busy, IRET before retrying","Too many commands outstanding, retry later","ncb_lana_num field invalid","Command completed while cancel occurring","Command not valid to cancel","Name defined by another local process","Environment undefined. RESET required","Required OS resources exhausted","Max number of applications exceeded","No saps available for netbios","Requested resources are not available","Invalid ncb address or length > segment","Invalid NCB DDID","Lock of user area failed","NETBIOS not loaded","System error","General network failure","Access denied","Invalid Parameter","Request is not supported","Network name not found"};
CString GETNBERR_DSCRaw(int x,int iLine)
{
	CString sRes;
	for(int i=0;i<sizeof(aNBErrs_Code)/sizeof(int);i++){
		if(aNBErrs_Code[i]==x){
			sRes=aNBErrs_Dsc[i];
			break;
		}
	}
	if(sRes==""){
		sRes=Format("0x%X",x);
	}
	if(iLine!=0){
		sRes+=Format(" (dbg:line %i)",iLine);
	}
	return sRes;
}

#ifdef _DEBUG
#define GETNBERR_DSC(x)	GETNBERR_DSCRaw(x,__LINE__)
#else
#define GETNBERR_DSC(x)	GETNBERR_DSCRaw(x,0)
#endif

//
struct NBMsgRecData
{
	UINT lsn;// Номер NB сессии для приема сообщения
	CNetNBProtocol* pParent;
};

#define MSG_SERVICE	"Messenger"
BOOL CheckMessengerService()
{
	if(objSettings.bUnderWindows98){
		return TRUE;
	}
	CSysService scm;
	scm.Open();
	if(scm.ServiceState(MSG_SERVICE)==SERVICE_RUNNING){
		if(AskYesNo(_l("Disable Messenger service","Do you want to use WireNote instead of\nsystem service for receiving net messages")+"?", PROGNAME,&objSettings.bCheckForDefMessenger)!=IDYES){
			return FALSE;
		}
		scm.Control(MSG_SERVICE,SERVICE_CONTROL_STOP);
		if(scm.GetServiceInformation(MSG_SERVICE)){
			scm.SetStartType(SERVICE_DEMAND_START);
			scm.SetServiceConfig();
		}
	}
	return TRUE;
}

/* additional function */
void MakeNetBIOSName(PSTR pszName, PSTR pszPaddedName, int iLastChar=0)
{
	if (pszName) strcpy(pszPaddedName,pszName);
	int iLen = strlen(pszPaddedName);
	_strupr(pszPaddedName);
	pszPaddedName[iLen] = '\0';
	unsigned int iLoop;
	for (iLoop=strlen(pszPaddedName);iLoop<MAXMACHINENAME;iLoop++){
		pszPaddedName[iLoop]=' ';
	}
	pszPaddedName[MAXMACHINENAME-1]=iLastChar;
};

BOOL GetRealSender(CNetNBProtocol* pParent, UINT m_lsn, CNCB& m_thisNCB, CString& sRealFrom)
{
	char szName[NCBNAMSZ]="";
	MakeNetBIOSName((char*)(const char*)(pParent->m_sAlias),szName,3);
	DWORD dwInfoRes=m_thisNCB.GetSessionStatus(szName);
	if(dwInfoRes == NRC_GOODRET){
		SESSION_INFO_BLOCK* pInfo=(SESSION_INFO_BLOCK*)m_thisNCB.GetBuffer();
		for(int i=0;i<pInfo->sib_header.num_sess;i++){
			if(pInfo->sib_Names[i].lsn==m_lsn){
				// Наша сессия
				memcpy(szName,(const char*)pInfo->sib_Names[i].remote_name,sizeof(szName));
				szName[sizeof(szName)-2]=0;
				sRealFrom=szName;
				sRealFrom.TrimRight();
				sRealFrom+="/nb";
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CheckSpecialMessages(CNetNBProtocol* pParent, const char* SMBBlock,UINT m_lsn,CNCB& m_thisNCB, BYTE& iMsgType)
{
	if(strstr(SMBBlock,EXCHSTATUS_ONLN)!=NULL){
		// Вытаскиваем чужой статус
		UpdateUserIconFromStatusStringID(SMBBlock+sizeof(EXCHSTATUS_ONLN)-1);
		// шлем статус
		char szStatus[128]="";
		memset(szStatus,0,sizeof(szStatus));
		strcpy(szStatus,CreateMyStatusStringID(pParent->m_sAlias));
		m_thisNCB.Send(m_lsn,szStatus,strlen(szStatus));
		return 1;
	}
	iMsgType=SMBBlock[4];
	if(iMsgType==0xd0 || iMsgType==0xd5	|| iMsgType==0xd6 || iMsgType==0xd7){
		// Нужна дальнейшая обработка...
		return 0;
	}
	AddError("Unknown message type!",TRUE);
	return 1;
}

char* SetSMBHeaderCommand(char* szHeader, BYTE iCommandCode, int iBufferLen)
{
	if(iBufferLen<SMB_HEADER_SIZE){
		return NULL;
	}
	memset(szHeader,0,iBufferLen);
	szHeader[0]=(char)0xff;
	szHeader[1]='S';
	szHeader[2]='M';
	szHeader[3]='B';
	szHeader[4]=(char)iCommandCode;
	return (szHeader+SMB_HEADER_SIZE);
}

// Принимаем сообщение
#define RECBUFFER_SIZE	2048
DWORD WINAPI NBMsgReciever(LPVOID lpdwstatus)
{
	NBMsgRecData* pMsgData=(NBMsgRecData*)lpdwstatus;
	if(!pMsgData){
		return 0;
	}
	int iRes=TRUE, nRes=0;
	UINT m_lsn=pMsgData->lsn;
	CNCB m_thisNCB(pMsgData->pParent->m_iLana);
	CString sReceivedMessage="";
	char szMessageData[RECBUFFER_SIZE]="";
	int iBlocksCount=0,iMesDataOffset=0;
	CString sTo, sFrom, sMessage;
	BYTE iMsgType=0;
	try{
		while(TRUE){
			char SMBBlock[255]="";
			memset(SMBBlock,0,sizeof(SMBBlock));
			nRes = m_thisNCB.Receive(m_lsn,(char *)SMBBlock,sizeof(SMBBlock));
			if ( nRes != NRC_GOODRET){
				objLog.AddMsgLogLine(DEFAULT_GENERALLOG,"Error while receiving netbios data block, err. #%i",nRes);
				// Возвращаемся чтобы показать хотябы часть
				//throw FALSE;
				break;
			}
			iBlocksCount++;
			UINT iReadedBytes=(UINT)m_thisNCB.GetLength();
			// Смотрим что к нам пришло - сообщение или запрос о статусе или что-то неизвестное
			if(CheckSpecialMessages(pMsgData->pParent, SMBBlock, m_lsn, m_thisNCB, iMsgType)){
				throw TRUE;
			}
			// Шлем подтверждение (3/5 ноля после SMB HEADER)
			char szReply[SMB_HEADER_SIZE+5]="";
			char* szReplyData=SetSMBHeaderCommand(szReply,iMsgType,sizeof(szReply));
			if(iMsgType==0xd5){
				// Код сообщения - случайные два байта
				szReplyData[0]=rnd(1,5);
				szReplyData[1]=rnd(1,5);
			}
			m_thisNCB.Send(m_lsn,szReply,(iMsgType==0xd5)?SMB_HEADER_SIZE+5:SMB_HEADER_SIZE+3);
			// Действия в зависимости от типа сообщения
			if(iMsgType==0xd0){// Короткое сообщение, вытаскиваем данные и выходим...
				sFrom=(const char*)SMBBlock+SMB_HEADER_SIZE+4;
				int iFromOffset=strlen(sFrom);
				sTo=(const char*)SMBBlock+SMB_HEADER_SIZE+4+iFromOffset+2;
				int iToOffset=strlen(sTo);
				sMessage=(const char*)SMBBlock+SMB_HEADER_SIZE+4+iFromOffset+2+iToOffset+4;
				break;
			}
			if(iMsgType==0xd5){// Кусочковое сообщение, начало, Смотрим от кого и кому
				sFrom=(const char*)SMBBlock+SMB_HEADER_SIZE+4;
				int iFromOffset=strlen(sFrom);
				sTo=(const char*)SMBBlock+SMB_HEADER_SIZE+iFromOffset+4+2;
			}
			if(iMsgType==0xd7){// Кусочковое сообщение, блок с данными, Конкатенируем...
				int iConcatSize=iReadedBytes-SMB_HEADER_SIZE-8;
				if(iMesDataOffset+iConcatSize+3>RECBUFFER_SIZE){
					// Буффер готов к переполнению, сбрасываем в выходную строку...
					szMessageData[iMesDataOffset]=0;
					sMessage+=szMessageData;
					iMesDataOffset=0;
				}
				memcpy(szMessageData+iMesDataOffset,SMBBlock+SMB_HEADER_SIZE+8,iConcatSize);
				iMesDataOffset+=iConcatSize;
			}
			if(iMsgType==0xd6){// Кусочковое сообщение, конец, Все получено, выходим
				break;
			}
		}
		// К этому моменту сообщение полностью получено
		if(iMsgType!=0xd0){
			// Для кусочкового сообщения доберем данные из буфера
			szMessageData[iMesDataOffset]=0;
			sMessage+=szMessageData;
			iMesDataOffset=0;
		}
		// Открываем сообщение
		sFrom.OemToAnsi();
		sFrom+="/nb";
		sMessage.Replace("\r\n","\n");
		sMessage.Replace("\x14","\n");// Специально для TrayPopup
		sMessage.OemToAnsi();
		// Смотрим информацию о сессии
		CString sRealFrom;
		GetRealSender(pMsgData->pParent, m_lsn, m_thisNCB, sRealFrom);
		OpenMessage(sFrom,sTo,sMessage,sRealFrom);
	}catch(int e){
		iRes=e;
	}
	m_thisNCB.Hangup(m_lsn);
	delete pMsgData;
	return iRes;
}

// Слушаем запросы по нетbиос...
BOOL bNetbiosActive=FALSE;
DWORD WINAPI NetBiosListener(LPVOID lpdwstatus)
{
	CNBListenerInfo* pInfo=(CNBListenerInfo*)lpdwstatus;
	if(!pInfo){
		return 1;
	}
	CNetNBProtocol* pParent=pInfo->pParent;
	if(!pParent){
		return 1;
	}
	CNCB m_thisNCB(pParent->m_iLana);
	UINT uReturn = m_thisNCB.AddName(pInfo->achAlias);
	if (uReturn != NRC_GOODRET){
		CString sAlias=CString(pInfo->achAlias).Left(MAXMACHINENAME-1);
		sAlias.TrimRight();
		if(pInfo->bMain){
			// Добавить не смогли, будем пользоваться Window-сервисом...
			pParent->bUseMessengerServiceToSend=TRUE;
		}
		objLog.AddMsgLogLine(DEFAULT_GENERALLOG,"Can`t add network alias ('%s')! Netbios messages unaviable now (err. %i, %s)", sAlias, uReturn, GETNBERR_DSC(uReturn));
		return FALSE;
	};
	// Листенер...
	UCHAR lsn=0;
	while(objSettings.bRunStatus!=-1 && bNetbiosActive){
		if (m_thisNCB.Listen(pInfo->achAlias,"*              \0",(UCHAR)objSettings.dwNetMessagingTimeout/500,(UCHAR)objSettings.dwNetMessagingTimeout/500)==NRC_GOODRET){
			lsn = m_thisNCB.GetLSN();
			HANDLE hNBMsgReciever;
			DWORD dwNBMsgReciever=0;
			NBMsgRecData* pNBMsgData=new NBMsgRecData;
			pNBMsgData->lsn=lsn;
			pNBMsgData->pParent=pParent;
			hNBMsgReciever = CreateThread( NULL, 0, NBMsgReciever, (LPVOID)pNBMsgData, 0, &dwNBMsgReciever);
			CloseHandle(hNBMsgReciever);
		}
		Sleep(1000);
	}
	return TRUE;
}

BOOL CNetNBProtocol::Start()
{
	bUseMessengerServiceToSend=!CheckMessengerService();
	if(objSettings.bUnderWindows98){
		// Под 98мы виндами listener не работает...
		bStarted=1;
		#pragma TODO(ME - must find замена)
		return TRUE;
	}
	CNCB m_thisNCB(m_iLana);
	m_thisNCB.Reset(0,0);
	//
	info1=new CNBListenerInfo;
	info1->pParent=this;
	info1->bMain=TRUE;
	strncpy(info1->achAlias,m_sAlias,strlen(m_sAlias));
	MakeNetBIOSName((char *)0,info1->achAlias,3);
	hNetBiosStatusThread = CreateThread( NULL, 0, NetBiosListener, info1, 0, &dwNetBiosThreadID);
	//
	if(objSettings.lNB_SupportLocUser){
		info2=new CNBListenerInfo;
		info2->pParent=this;
		DWORD dwSize=sizeof(info2->achAlias);
		::GetUserName(info2->achAlias,&dwSize);
		MakeNetBIOSName((char *)0,info2->achAlias,3);
		hNetBiosStatusThread2 = CreateThread( NULL, 0, NetBiosListener, info2, 0, &dwNetBiosThreadID);
	}
	bStarted=1;
	return TRUE;
}

CNetNBProtocol::CNetNBProtocol(const char* szAlias, int iLana)
{
	m_sAlias=szAlias;
	m_iLana=iLana;
	bNetbiosActive=TRUE;
	dwNetBiosThreadID=0;
	bUseMessengerServiceToSend=FALSE;
	hNetBiosStatusThread=0;
	hNetBiosStatusThread2=0;
	info1=0;
	info2=0;
}

CNetNBProtocol::~CNetNBProtocol()
{
	/* Для нетбиоса чревато это...
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus<0){
		TerminateThread(hNetBiosStatusThread,99);
		CloseHandle(hNetBiosStatusThread);
		hNetBiosStatusThread=0;
		TerminateThread(hNetBiosStatusThread2,1);
		CloseHandle(hNetBiosStatusThread2);
		hNetBiosStatusThread2=0;
	}
	*/
	bStarted=0;
	bNetbiosActive=FALSE;
	//
	CNCB ncbNameDeleter(m_iLana);
	if(info1){
		ncbNameDeleter.DeleteName(info1->achAlias);
		if(hNetBiosStatusThread){
			if(WaitForSingleObject(hNetBiosStatusThread,5000)==WAIT_TIMEOUT){
				TerminateThread(hNetBiosStatusThread,1);
			}
			CloseHandle(hNetBiosStatusThread);
		}
		delete info1;
		info1=0;
	}
	if(hNetBiosStatusThread2 && info2){
		ncbNameDeleter.DeleteName(info2->achAlias);
		if(hNetBiosStatusThread2){
			if(WaitForSingleObject(hNetBiosStatusThread2,5000)==WAIT_TIMEOUT){
				TerminateThread(hNetBiosStatusThread2,1);
			}
			CloseHandle(hNetBiosStatusThread2);
		}
		delete info2;
		info1=0;
	}
}

int CNetNBProtocol::PrepareNBSession(CNCB& m_thisNCB, const char* szTo, const char* szFrom)
{
	char m_pszPaddedFileNameTo[MAXMACHINENAME]="";
	MakeNetBIOSName((char *)szTo,m_pszPaddedFileNameTo,3);
	char m_pszPaddedFileNameFrom[MAXMACHINENAME]="";
	MakeNetBIOSName((char *)szFrom,m_pszPaddedFileNameFrom,3);
	return m_thisNCB.Call(m_pszPaddedFileNameFrom,m_pszPaddedFileNameTo,(UCHAR)objSettings.dwNetMessagingTimeout/500,(UCHAR)objSettings.dwNetMessagingTimeout/500);
}

typedef DWORD (__stdcall *FPNetMessageBufferSend) (LPCWSTR  servername, LPCWSTR  msgname, LPCWSTR  fromname, LPBYTE   buf, DWORD    buflen);
/* Схема отправки большого NB-сообщения:
1) Шлется D5. Текст сообщения:
- 00, длина остатка A (1 байт), 00 (3 байта)
- остаток A. 04, строка КТО, 00, 04, строка КОМУ, 00
в ответ приходит КОД СООБЩЕНИЯ (2 байта) и 00,00,00 (всего 5 байт), с D5 в заголовке

2) Шлется D7. Текст сообщения:
- КОД СООБЩЕНИЯ (2 байта), 00, длина остатка (A+B) (1 байт), 00 (всего 5 байт)
- остаток A. 01, длина остатка B, 00 (3 байта)
- остаток B. очередной кусок посланного СООБЩЕНИЯ
в ответ приходит пустое сообщение (с 3мя 0-лями), с D7 в заголовке

3) Пункт 2 повторяется пока не будет отослано все СООБЩЕНИЕ

4) Шлется D6.Текст сообщения:
- КОД СООБЩЕНИЯ (2 байта), 00, 00, 00 (всего 5 байт)
в ответ приходит пустое сообщение (с 3мя 0-лями), с D6 в заголовке

Замечания: обычный messenger service принимает максимально 966 символов (по 136 байт+остаток)
*/
BOOL CNetNBProtocol::SendNet_Text(const char* szRecipient, CNetMessage_Text* str)
{
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return 0;
	}
	if(bStarted==0){
		Alert(_l(DEFAULT_NOPROTWARN),5000,FALSE,DT_CENTER|DT_VCENTER);
	}
	str->sMessageAttach.TrimRight();
	if(str->sMessageAttach.GetLength()>0 && !objSettings.lDisableAttachExtens){//str->sMessageAttach.GetLength()<100 && 
		// Аттачим в текст!!!
		str->sMessageAttach.Replace("&","&amp;");
		str->sMessageAttach.Replace(">","&gt;");
		str->sMessageBody+="\n<!--WNSYS'[WireNote] You can safely ignore the rest of the message\n"+str->sMessageAttach+"'-->";
		str->sMessageAttach="";
	}
	/*else if(str->sMessageAttach!=""){
		Alert(_l(DEFAULT_ATTWARN),DEFAULT_READTIME,FALSE,DT_CENTER|DT_VCENTER);
	}*/
	int nRes=FALSE;
	CString sTo=szRecipient;
	CString sMessage=str->sMessageBody;
	CString sFrom=m_sAlias;
	ExtractSenderFromText(sFrom, sMessage);
	DWORD iMessageLen=strlen(sMessage);
	sTo.MakeUpper();
	CNCB m_thisNCB(m_iLana);
	// Пытаемся послать...
	if(bUseMessengerServiceToSend){
		// Отсылаем с использованием сервиса...
		USES_CONVERSION;
		BOOL bNoDLL=TRUE;
		if(m_thisNCB.hNetapiModule){
			FPNetMessageBufferSend fp=(FPNetMessageBufferSend)GetProcAddress(m_thisNCB.hNetapiModule,"NetMessageBufferSend");
			if(fp!=NULL){
				bNoDLL=FALSE;
				int nBufSendRes=(*fp)(NULL,A2W(sTo),A2W(sFrom),(LPBYTE)(A2W(sMessage)),(iMessageLen*sizeof(WCHAR)));
				nRes=(nBufSendRes==NERR_Success);//0x8E1,NERR_NameNotFound
				if(!nRes){
					str->sError=_l(Format("%s (NB Service)",GETNBERR_DSC(nBufSendRes)));
				}
			}
		}
		if(bNoDLL){
			nRes=FALSE;
			str->sError=_l("Can not load NetApi32.dll");
		}
		return nRes;
	}
	// Отсылаем сами
	sMessage.AnsiToOem();
	//////////////////////
	// Собственно отправка
	nRes = PrepareNBSession(m_thisNCB,sTo,m_sAlias);
	if(nRes != NRC_GOODRET){
		str->sError=_l(Format("%s (NB)",GETNBERR_DSC(nRes)));
		return FALSE;
	}
	UINT m_lsn = m_thisNCB.GetLSN();
	char SMBBlock[168]="";
	char* szSMBData=NULL;
	int iFromLen=strlen(sFrom);
	int iToLen=strlen(sTo);
	int iHiMsgCode=0,iLoMsgCode=0;
	try{
		// 1. Шлем заголовок
		szSMBData=SetSMBHeaderCommand(SMBBlock,0xd5, sizeof(SMBBlock));
		DWORD dwD5ALength=1+iFromLen+2+iToLen+1;
		szSMBData[1]=(char)dwD5ALength;
		szSMBData[3]=0x04;
		szSMBData[4+iFromLen+1]=0x04;
		memcpy(szSMBData+4,sFrom,iFromLen);
		memcpy(szSMBData+4+iFromLen+2,sTo,iToLen);
		DWORD dwD5Length=3+dwD5ALength;
		nRes=m_thisNCB.Send(m_lsn,SMBBlock,SMB_HEADER_SIZE+dwD5Length);
		if(nRes!=NRC_GOODRET){
			str->sError=_l(Format("%s (NB)",GETNBERR_DSC(nRes)));
			throw FALSE;
		}
		// Ждем подтверждения
		nRes = m_thisNCB.Receive(m_lsn,(char *)SMBBlock,sizeof(SMBBlock));
		if (nRes != NRC_GOODRET){
			str->sError=_l(Format("%s (NB)",GETNBERR_DSC(nRes)));
			throw FALSE;
		}
		iHiMsgCode=SMBBlock[SMB_HEADER_SIZE];
		iLoMsgCode=SMBBlock[SMB_HEADER_SIZE+1];
		// 2. Шлем сообщение (кусочками)
		DWORD iSendedBytes=0;
		while(iSendedBytes<iMessageLen){
			szSMBData=SetSMBHeaderCommand(SMBBlock,0xd7,sizeof(SMBBlock));
			DWORD dwD7BLength=sizeof(SMBBlock)-3-5-SMB_HEADER_SIZE;
			if(iSendedBytes+dwD7BLength>iMessageLen){
				dwD7BLength=iMessageLen-iSendedBytes;
			}
			szSMBData[0]=(char)iHiMsgCode;
			szSMBData[1]=(char)iLoMsgCode;
			szSMBData[3]=(char)(dwD7BLength+3);
			szSMBData[5]=(char)(0x01);
			szSMBData[6]=(char)(dwD7BLength);
			memcpy(szSMBData+8,(const char*)sMessage+iSendedBytes,dwD7BLength);
			DWORD dwD7Length=5+3+dwD7BLength;
			char* szText=SMBBlock+SMB_HEADER_SIZE+dwD7Length;
			nRes=m_thisNCB.Send(m_lsn,SMBBlock,SMB_HEADER_SIZE+dwD7Length);
			if(nRes!=NRC_GOODRET){
				str->sError=_l(Format("%s (NB)",GETNBERR_DSC(nRes)));
				throw FALSE;
			}
			iSendedBytes+=dwD7BLength;
			// Ждем подтверждения
			nRes = m_thisNCB.Receive(m_lsn,(char *)SMBBlock,sizeof(SMBBlock));
			if ( nRes != NRC_GOODRET){
				str->sError=_l(Format("%s (NB)",GETNBERR_DSC(nRes)));
				throw FALSE;
			}
		}
		// 3. Шлем извещение что все отослано
		szSMBData=SetSMBHeaderCommand(SMBBlock,0xd6, sizeof(SMBBlock));
		DWORD dwD6Length=5;
		szSMBData[0]=(char)iHiMsgCode;
		szSMBData[1]=(char)iLoMsgCode;
		nRes=m_thisNCB.Send(m_lsn,SMBBlock,SMB_HEADER_SIZE+dwD6Length);
		if(nRes!=NRC_GOODRET){
			str->sError=_l(Format("%s (NB)",GETNBERR_DSC(nRes)));
			throw FALSE;
		}
		// Ждем подтверждения
		nRes = m_thisNCB.Receive(m_lsn,(char *)SMBBlock,sizeof(SMBBlock));
		if (nRes != NRC_GOODRET){
			str->sError=_l(Format("%s (NB)",GETNBERR_DSC(nRes)));
			throw FALSE;
		}
		// 4. Все!! :)
		nRes=TRUE;
	}catch(int e){
		nRes=e;
	}
	// Закрываем сессию
	m_thisNCB.Hangup(m_lsn);
	return nRes;
}

BOOL CNetNBProtocol::FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers)
{
	// Not implemented
	return FALSE;
}

#define NB_STATEXCH_SIZE	128
BOOL CNetNBProtocol::SendNet_StatusExch(const char* szRecipient,const char* szProtocol)
{
	if(bStarted==0){
		return FALSE;
	}
	if(bUseMessengerServiceToSend){
		// Так как используется сервис, то считаем что всегда есть
		return 1;
	}
	int nRes=0;
	CNCB m_thisNCB(m_iLana);
	nRes = PrepareNBSession(m_thisNCB,szRecipient,m_sAlias);
	if(nRes != NRC_GOODRET){
		return -1;// Сессия не открывается, там никого нет
	}
	UINT m_lsn = m_thisNCB.GetLSN();
	char szStatus[NB_STATEXCH_SIZE]="";
	memset(szStatus,0,sizeof(szStatus));
	strcpy(szStatus,EXCHSTATUS_ONLN);
	strcpy(szStatus+sizeof(EXCHSTATUS_ONLN)-1,CreateMyStatusStringID(m_sAlias));
	nRes = m_thisNCB.Send(m_lsn,szStatus,strlen(szStatus));
	if(nRes != NRC_GOODRET){
		return -1;// Сессия не открывается, там никого нет
	}
	memset(szStatus,0,sizeof(szStatus));
	nRes = m_thisNCB.Receive(m_lsn,(char *)szStatus,sizeof(szStatus));
	if (nRes != NRC_GOODRET){
		return 1;// Все таки кто-то есть раз послать удалось
	}
	UpdateUserIconFromStatusStringID(szStatus);
	m_thisNCB.Hangup(m_lsn);
	return TRUE;
}