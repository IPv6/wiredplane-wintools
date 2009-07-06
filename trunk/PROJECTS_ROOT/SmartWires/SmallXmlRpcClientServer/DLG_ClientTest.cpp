// DLG_ClientTest.cpp : implementation file
//

#include "stdafx.h"
#include "SmallXmlRpcClientServer.h"
#include "DLG_ClientTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DLG_ClientTest dialog


DLG_ClientTest::DLG_ClientTest(CWnd* pParent /*=NULL*/)
	: CDialog(/*DLG_ClientTest::IDD, pParent*/)
{
	//{{AFX_DATA_INIT(DLG_ClientTest)
	m_Address = _T("");
	m_Received = _T("");
	m_Sended = _T("");
	m_Method = _T("");
	m_Statistica = _T("");
	//}}AFX_DATA_INIT
}


void DLG_ClientTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DLG_ClientTest)
	DDX_Control(pDX, IDC_GETSTATISTIC, m_Stats);
	DDX_Text(pDX, IDC_ADDRESS, m_Address);
	DDX_Text(pDX, IDC_REQUEST, m_Sended);
	DDX_Text(pDX, IDC_METHOD, m_Method);
	DDX_Text(pDX, IDC_STATISTIC, m_Statistica);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DLG_ClientTest, CDialog)
	//{{AFX_MSG_MAP(DLG_ClientTest)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_GETSTATISTIC, OnGetstatistic)
	ON_BN_CLICKED(IDC_START_SYNC, OnStartSync)
	ON_BN_CLICKED(IDC_START_ASYNC, OnStartAsync)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLG_ClientTest message handlers
#include "utils.h"
DLG_ClientTest* mainwndC=NULL;
BOOL bNeedUpdateC=0;
BOOL CALLBACK OnDataSendC(int iServerNum, const char* szData, int iDataLength)
{
	CString sText;
	sText+=TimeStamp()+"\r\n-------------- Sended data ------------\r\n";
	sText+=szData;
	sText+="\r\n";
	mainwndC->m_Sended=sText+mainwndC->m_Sended;
	bNeedUpdateC=1;
	return FALSE;
}

BOOL CALLBACK OnDataReceiveC(int iServerNum, const char* szData, int iDataLength)
{
	CString sText;
	sText+=TimeStamp()+"\r\n-------------- Received data ------------\r\n";
	sText+=szData;
	sText+="\r\n";
	mainwndC->m_Sended=sText+mainwndC->m_Sended;
	bNeedUpdateC=1;
	return FALSE;
}

#include "utils.h"
BOOL CALLBACK OnErrorC(int iServerNum, SOCKET* sock, const char* szData, CActionError& error)
{
	CString sText;
	sText+=TimeStamp()+"\r\n-------------- Error detected ------------\r\n";
	sText+=Format("ERROR #%i %s (WSA:%s)\r\nData received:\r\n%s",error.getErrorCode(),error.getErrorDsc(),GetLastWSAError(),szData);
	sText+="\r\n";
	mainwndC->m_Sended=sText+mainwndC->m_Sended;
	bNeedUpdateC=1;
	return FALSE;
}

void DLG_ClientTest::OnStart() 
{
	// TODO: Add your control notification handler code here
	
}

void DLG_ClientTest::OnGetstatistic() 
{
	// TODO: Add your control notification handler code here
	long lTotal;
	long lErrors;
	long lInProcess;
	long lInMem;
	long lBytesSend;
	long lBytesRec;
	GetClientStatistics(lTotal, lErrors, lInProcess, lInMem, lBytesSend, lBytesRec);
	m_Statistica=Format("total:%i\r\nerrs:%i\r\nInProcess:%i (in mem %i)\r\nBytes sent:%i\r\nBytes Received:%i",lTotal, lErrors, lInProcess, lInMem, lBytesSend, lBytesRec);
	UpdateData(FALSE);
}

BOOL DLG_ClientTest::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	dwTimer=SetTimer(65,2000,NULL);
	Sizer.InitSizer(this,10,HKEY_LOCAL_MACHINE,"SOFTWARE\\WiredPlane\\ClientTestPadWnd");
	Sizer.SetOptions(STICKABLE);

	mainwndC=this;
	InitXmlRpcClient(1000,5);
	OnXmlRpcServerDataReceived(-1,OnDataReceiveC);
	OnXmlRpcServerError(-1,OnErrorC);
	OnXmlRpcServerDataSended(-1,OnDataSendC);
	m_Address="http://127.0.0.1:5432/RPC2";
	m_Method="server.copyright";
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DLG_ClientTest::OnStartSync() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	XmlRpcRequest Request;
	Request.szURI=m_Address;
	Request.szMethodName=m_Method;
	Request.values<<"Just a string";
	XmlRpcResult result;
	CActionError error;
	MakeXmlRpcCall(Request, result, 5000, error);
	CString sres;
	sres+=TimeStamp()+"\r\nRequest Result:"+CRLF;
	sres+=(CString)"Error during send section:";
	sres+=Format("%i-%s",error.getErrorCode(),error.getErrorDsc());
	sres+=(CString)"\r\nError section:";
	sres+=Format("%i-%s",result.error.getErrorCode(),result.error.getErrorDsc());
	sres+=(CString)"\r\nValues section:"+CRLF;
	for(int i=0;i<result.values.GetSize();i++){
		sres+=(CString)result.values.GetAsString(i,result.error)+CRLF;
	}
	sres+="\r\n";
	m_Sended=sres+m_Sended;
	bNeedUpdateC=1;
}

class XmlRpcRequestUserDataEx: public XmlRpcRequestUserData
{
public:
	CString sValue;
	XmlRpcRequestUserDataEx(const char* szTxt){sValue=szTxt;};
	virtual ~XmlRpcRequestUserDataEx(){/*AfxMessageBox(Format("UserData '%s' Deleted! ^)",sValue));*/};
};

BOOL CALLBACK CallbackFunc(XmlRpcRequestEx* lpRequest)
{
	CString sres;
	sres+=TimeStamp()+"\r\nCallback Request Result:"+CRLF;
	sres+="UserData:";
	sres+=((XmlRpcRequestUserDataEx*)lpRequest->UserData)->sValue+CRLF;
	sres+="Error during send section:";
	sres+=Format("%i-%s",lpRequest->error.getErrorCode(),lpRequest->error.getErrorDsc());
	sres+="\r\nError section:";
	sres+=Format("%i-%s",lpRequest->result.error.getErrorCode(),lpRequest->result.error.getErrorDsc());
	sres+=(CString)"\r\nValues section:"+CRLF;
	for(int i=0;i<lpRequest->result.values.GetSize();i++){
		sres+=(CString)lpRequest->result.values.GetAsString(i,lpRequest->result.error)+CRLF;
	}
	sres+="\r\n";
	mainwndC->m_Sended=sres+mainwndC->m_Sended;
	bNeedUpdateC=1;
	return TRUE;
};


int udCount=0;
void DLG_ClientTest::OnStartAsync() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	XmlRpcRequest Request;
	Request.szURI=m_Address;
	Request.szMethodName=m_Method;
	Request.values.AddValue(new XmlRpcValue("Just a string for callback call"));
	XmlRpcResult result;
	CActionError error;
	XmlRpcRequestUserDataEx* UserData=new XmlRpcRequestUserDataEx(Format("ud#%i",udCount++));
	MakeCallbackXmlRpcCall(Request, 50000, CallbackFunc, UserData);
	return;
}

LRESULT DLG_ClientTest::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}	
	return CDialog::WindowProc(message, wParam, lParam);
}

void DLG_ClientTest::OnUpdate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
}

void DLG_ClientTest::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	KillTimer(dwTimer);	
}

void DLG_ClientTest::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(bNeedUpdateC==1){
		bNeedUpdateC=0;
		OnGetstatistic();
		OnUpdate();
	}
	CDialog::OnTimer(nIDEvent);
}
