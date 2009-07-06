#ifndef _CNCB_H
#define _CNCB_H

#include <nb30.h>
#define MAXMACHINENAME 16

// compound structures
typedef struct ADAPTER_STATUS_BLOCK
{
	ADAPTER_STATUS asb_header;
	NAME_BUFFER asb_Names[NCBNAMSZ];
} ADAPTER_STATUS_BLOCK, *PADAPTER_STATUS_BLOCK;

typedef struct SESSION_INFO_BLOCK
{
	SESSION_HEADER sib_header;
	SESSION_BUFFER sib_Names[NCBNAMSZ];
} SESSION_INFO_BLOCK, *PSESSION_INFO_BLOCK;

typedef UCHAR (APIENTRY *FPNetbios) (PNCB pncb);

class CNCB
{ 
private:
	NCB m_NCB;
	int m_iLanaNum;
public:
	HMODULE hNetapiModule;
	FPNetbios fp;
	// constructor
	CNCB(int iLanaNum);
	~CNCB();
	// helper function
	void* GetBuffer();
	void ClearNCB();
	UCHAR GetLSN();
	WORD GetLength();
	void Fill(CNCB ncbSource);
	UCHAR GetCommand();
	// Name Management Services
	UCHAR AddName(PSTR pName);
	UCHAR AddGroupName(PSTR pName);
	UCHAR DeleteName(PSTR pName);
	UCHAR FindName();
	
	// Data Transfer Services
	UCHAR Call(PSTR pWe,PSTR pTheOther,UCHAR wSendTO,UCHAR wRecvTO);   // fill in parameters later;;;
	UCHAR Listen(PSTR pWe,PSTR pTheOther,UCHAR wSendTO,UCHAR wRecvTO);
	UCHAR Hangup(UCHAR wSessionNumber);
	// Connectionless Data Transfer
	UCHAR Cancel();
	UCHAR Send(UCHAR wSessionNumber,LPSTR lpPacket, UINT wLength);
	UCHAR SendNoAck(UCHAR wSessionNumber,LPSTR lpPacket, UINT wLength);
	UCHAR SendDatagram(UCHAR wSessionNumber,LPSTR lpPacket, WORD wLength);
	UCHAR SendBroadcastDatagram();
	UCHAR Receive(UCHAR wSessionNumber,LPSTR lpPacket, UINT wLength);
	UCHAR ReceiveAny();
	UCHAR ReceiveDatagram(UCHAR wSessionNumber,LPSTR lpPacket, WORD wLength);
	UCHAR ReceiveBroadcastDatagram();
	UCHAR ChainSend(UCHAR wSessionNumber,LPSTR lpPacket1, UINT wLength1, LPSTR lpPacket2, UINT wLength2);
	UCHAR ChainSendNoAck();
	
	// General Purpose Services
	UCHAR Reset(UCHAR wSessions, UCHAR wNCBs);
	UCHAR GetAdapterStatus(PSTR pName);
	UCHAR GetSessionStatus(PSTR pName);
	static UCHAR EnumerateAdapters(LANA_ENUM* asEnum);
	UCHAR StatusAlert();
	UCHAR Action();
};
#endif