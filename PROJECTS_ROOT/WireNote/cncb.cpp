#include <windows.h>
#include "cncb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CNCB::CNCB(int iLana)
{
	fp=NULL;
	m_iLanaNum=iLana;
	hNetapiModule=LoadLibrary("Netapi32.dll");
	if(hNetapiModule){
		fp=(FPNetbios)GetProcAddress(hNetapiModule,"Netbios");
	}
};

CNCB::~CNCB()
{
	if(hNetapiModule){
		FreeLibrary(hNetapiModule);
		hNetapiModule=NULL;
	}
}


void CNCB::ClearNCB()
{
	memset((void *)&m_NCB,0,sizeof(NCB));
};

UCHAR CNCB::GetLSN()
{
	return m_NCB.ncb_lsn;
};

WORD CNCB::GetLength()
{
	return m_NCB.ncb_length;
};

void* CNCB::GetBuffer()
{
	return m_NCB.ncb_buffer;
}

void CNCB::Fill(CNCB ncbSource)
{
	memcpy(&m_NCB,&ncbSource.m_NCB,sizeof(NCB));
};

UCHAR CNCB::GetCommand()
{
	return m_NCB.ncb_command;
};


/*********************************************************/
UCHAR CNCB::AddName(PSTR pName)
{
	ClearNCB();
	m_NCB.ncb_command = NCBADDNAME;
	m_NCB.ncb_lana_num = m_iLanaNum;
	strcpy((char *)&m_NCB.ncb_name,pName);
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::AddGroupName(PSTR pName)
{
	ClearNCB();
	m_NCB.ncb_command = NCBADDGRNAME;
	m_NCB.ncb_lana_num = m_iLanaNum;
	strcpy((char *)&m_NCB.ncb_name,pName);
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::DeleteName(PSTR pName)
{
	ClearNCB();
	m_NCB.ncb_command = NCBDELNAME;
	m_NCB.ncb_lana_num = m_iLanaNum;
	strcpy((char *)&m_NCB.ncb_name,pName);
	return (fp?((*fp)(&m_NCB)):(0x3f));
};

/*********************************************************/
UCHAR CNCB::FindName()
{
	return 0;
};

// Data Transfer Services
/*********************************************************/
UCHAR CNCB::Call(PSTR pWe,PSTR pTheOther,UCHAR wSendTO,UCHAR wRecvTO)
{
	ClearNCB();
	strncpy((char *)&m_NCB.ncb_name,pWe,MAXMACHINENAME);
	strncpy((char *)&m_NCB.ncb_callname,pTheOther,MAXMACHINENAME);
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_rto = wRecvTO;
	m_NCB.ncb_sto = wSendTO;
	m_NCB.ncb_command = NCBCALL;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};

/*********************************************************/
UCHAR CNCB::Listen(PSTR pWe,PSTR pTheOther,UCHAR wSendTO,UCHAR wRecvTO)
{
	ClearNCB();
	strncpy((char *)&m_NCB.ncb_name,pWe,MAXMACHINENAME);
	strncpy((char *)&m_NCB.ncb_callname,pTheOther,MAXMACHINENAME);
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_rto = (UCHAR)wRecvTO;
	m_NCB.ncb_sto = (UCHAR)wSendTO;
	m_NCB.ncb_command = NCBLISTEN;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::Hangup(UCHAR wSessionNumber)
{
	ClearNCB();
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBHANGUP;
	m_NCB.ncb_lsn = wSessionNumber;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
// Connectionless Data Transfer
/*********************************************************/
UCHAR CNCB::Cancel()
{
	CNCB cbCanceller(m_iLanaNum);
	cbCanceller.ClearNCB();
	m_NCB.ncb_lana_num = m_iLanaNum;
	cbCanceller.m_NCB.ncb_buffer = (unsigned char *)&m_NCB;
	cbCanceller.m_NCB.ncb_length = sizeof(NCB);
	cbCanceller.m_NCB.ncb_command = NCBCANCEL;
	return (fp?((*fp)(&cbCanceller.m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::Send(UCHAR wSessionNumber,LPSTR lpPacket, UINT wLength)
{
	ClearNCB();
	if (wLength > 0xffff) RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,NULL);
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBSEND;
	m_NCB.ncb_lsn = wSessionNumber;
	m_NCB.ncb_length = wLength;
	m_NCB.ncb_buffer = (unsigned char *)lpPacket;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::SendNoAck(UCHAR wSessionNumber,LPSTR lpPacket, UINT wLength)
{ 
	ClearNCB();
	if (wLength > 0xffff) RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,NULL);
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBSENDNA;
	m_NCB.ncb_lsn = wSessionNumber;
	m_NCB.ncb_length = wLength;
	m_NCB.ncb_buffer = (unsigned char *)lpPacket;
	return (fp?((*fp)(&m_NCB)):(0x3f));
	return 0;
};
/*********************************************************/
UCHAR CNCB::SendDatagram(UCHAR wSessionNumber,LPSTR lpPacket, WORD wLength)
{
	ClearNCB();
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBDGSEND;
	m_NCB.ncb_lsn = wSessionNumber;
	m_NCB.ncb_length = wLength;
	m_NCB.ncb_buffer = (unsigned char *)lpPacket;
	return (fp?((*fp)(&m_NCB)):(0x3f));
}; 
/*********************************************************/
UCHAR CNCB::SendBroadcastDatagram()
{
	return 0;
};
/*********************************************************/
UCHAR CNCB::Receive(UCHAR wSessionNumber,LPSTR lpPacket, UINT wLength)
{
	ClearNCB();
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBRECV;
	m_NCB.ncb_lsn = wSessionNumber;
	// do not hardcode that!!!
	if (wLength > 0xffff) m_NCB.ncb_length = 0xffff; else
		m_NCB.ncb_length = wLength;
	m_NCB.ncb_buffer = (unsigned char *)lpPacket;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};


UCHAR CNCB::ReceiveDatagram(UCHAR wSessionNumber,LPSTR lpPacket, WORD wLength)
{
	ClearNCB();
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBDGRECV;
	m_NCB.ncb_lsn = wSessionNumber;
	m_NCB.ncb_length = wLength;
	m_NCB.ncb_buffer = (unsigned char *)lpPacket;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::ReceiveBroadcastDatagram()
{
	return 0;
};
/*********************************************************/
UCHAR CNCB::ChainSend(UCHAR wSessionNumber,LPSTR lpPacket1, UINT wLength1, LPSTR lpPacket2, UINT wLength2 )
{
	ClearNCB();
	if (wLength1 > 0xffff) RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,NULL);
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBCHAINSEND;
	m_NCB.ncb_lsn = wSessionNumber;
	m_NCB.ncb_length = wLength1;
	m_NCB.ncb_buffer = (unsigned char *)lpPacket1;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::ChainSendNoAck()
{
	return 0;
};

// General Purpose Services
UCHAR CNCB::Reset(UCHAR wSessions, UCHAR wNCBs)
{
	ClearNCB();
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_command = NCBRESET;
	m_NCB.ncb_lsn = wSessions;
	m_NCB.ncb_num = wNCBs;
	m_NCB.ncb_callname[0] = 250;
	m_NCB.ncb_callname[2] = 250;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::GetAdapterStatus(PSTR pName)
{
	ClearNCB();
	ADAPTER_STATUS_BLOCK asStatus;
	memset(&asStatus,0,sizeof(ADAPTER_STATUS_BLOCK));
	strncpy((char *)&m_NCB.ncb_callname,pName,MAXMACHINENAME);
	m_NCB.ncb_command = NCBASTAT;
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_length = sizeof(ADAPTER_STATUS_BLOCK);
	m_NCB.ncb_buffer = (unsigned char *)&asStatus;
	return (fp?((*fp)(&m_NCB)):(0x3f));
};
/*********************************************************/
UCHAR CNCB::GetSessionStatus(PSTR pName)
{
	ClearNCB();
	SESSION_INFO_BLOCK sibSession;
	memset(&sibSession,0,sizeof(SESSION_INFO_BLOCK));
	strncpy((char *)&m_NCB.ncb_name,pName,MAXMACHINENAME);
	m_NCB.ncb_command = NCBSSTAT;
	m_NCB.ncb_lana_num = m_iLanaNum;
	m_NCB.ncb_buffer = (unsigned char *)&sibSession;
	m_NCB.ncb_length = sizeof(SESSION_INFO_BLOCK);
	return((*fp)(&m_NCB));
};
/*********************************************************/
UCHAR CNCB::EnumerateAdapters(LANA_ENUM* asEnum)
{
	UINT nRes=0x3f;
	memset(asEnum,0,sizeof(asEnum));
	HMODULE hNetapiModuleT;
	FPNetbios fpT;
	hNetapiModuleT=LoadLibrary("Netapi32.dll");
	if(hNetapiModuleT){
		fpT=(FPNetbios)GetProcAddress(hNetapiModuleT,"Netbios");
		NCB mNCB;
		memset((void *)&mNCB,0,sizeof(NCB));
		mNCB.ncb_command = NCBENUM;
		mNCB.ncb_length = sizeof(asEnum);
		mNCB.ncb_buffer = (unsigned char *)asEnum;
		nRes=(fpT?((*fpT)(&mNCB)):(0x3f));
		FreeLibrary(hNetapiModuleT);
	}
	return nRes;
};
/*********************************************************/
UCHAR CNCB::StatusAlert()
{
	return 0;
};
/*********************************************************/
UCHAR CNCB::Action()
{
	return 0;
};