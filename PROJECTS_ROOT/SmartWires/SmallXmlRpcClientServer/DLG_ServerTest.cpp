// DLG_ServerTest.cpp : implementation file
//

#include "stdafx.h"
#include "SmallXmlRpcClientServer.h"
#include "DLG_ServerTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DLG_ServerTest dialog


DLG_ServerTest::DLG_ServerTest(CWnd* pParent /*=NULL*/)
	: CDialog(/*DLG_ServerTest::IDD, pParent*/)
{
	//{{AFX_DATA_INIT(DLG_ServerTest)
	m_Port = _T("");
	m_Statistics = _T("");
	m_Status = _T("");
	m_data = _T("");
	//}}AFX_DATA_INIT
}


void DLG_ServerTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DLG_ServerTest)
	DDX_Text(pDX, IDC_PORT, m_Port);
	DDX_Text(pDX, IDC_STATISTIC, m_Statistics);
	DDX_Text(pDX, IDC_STATUSLINE, m_Status);
	DDX_Text(pDX, IDC_CONTENT, m_data);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DLG_ServerTest, CDialog)
	//{{AFX_MSG_MAP(DLG_ServerTest)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_GETSTATISTIC, OnGetstatistic)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#include "utils.h"
DLG_ServerTest* mainwnd;
BOOL bNeedUpdate=0;
BOOL CALLBACK OnDataSend(int iServerNum, const char* szData, int iDataLength)
{
	CString sText;
	sText+=TimeStamp()+"\r\n-------------- Sended data ------------\r\n";
	sText+=szData;
	sText+="\r\n";
	mainwnd->m_data=sText+mainwnd->m_data;
	bNeedUpdate=1;
	return FALSE;
}

BOOL CALLBACK OnDataReceive(int iServerNum, const char* szData, int iDataLength)
{
	CString sText;
	sText+=TimeStamp()+"\r\n-------------- Received data ------------\r\n";
	sText+=szData;
	sText+="\r\n";
	mainwnd->m_data=sText+mainwnd->m_data;
	bNeedUpdate=1;
	return FALSE;
}

BOOL CALLBACK OnError(int iServerNum, SOCKET* sock, const char* szData, CActionError& error)
{
	CString sText;
	sText+=TimeStamp()+"\r\n----------------- error detected ------------\r\n";
	sText+=Format("ERROR #%i %s (WSA:%s)\r\nData received:\r\n%s",error.getErrorCode(),error.getErrorDsc(),GetLastWSAError(),szData);
	sText+="\r\n";
	mainwnd->m_data=sText+mainwnd->m_data;
	bNeedUpdate=1;
	return FALSE;
}

BOOL CALLBACK ExamplesEcho(int iServerNum, XmlRpcRequest* lpRequest, XmlRpcResult* lpResult)
{
	CString sres=TimeStamp()+"\r\n ----------------- Request to ExamplesEcho------------\r\n"+CRLF;
	sres+="Method:";
	sres+=(CString)lpRequest->szMethodName+CRLF;
	sres+="URI:";
	sres+=(CString)lpRequest->szURI+CRLF;
	sres+=(CString)"Values section:"+CRLF;
	for(int i=0;i<lpRequest->values.GetSize();i++){
		sres+=(CString)lpRequest->values.GetAsString(i,lpResult->error)+CRLF;
	}
	sres+=(CString)"Error section:"+CRLF;
	sres+=Format("%i-%s",lpResult->error.getErrorCode(),lpResult->error.getErrorDsc());
	sres+="\r\n";
	mainwnd->m_data=sres+mainwnd->m_data;
	bNeedUpdate=1;
	lpResult->values.AddValue(new XmlRpcValue("Yahoo! This is my first reply! :)))"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DLG_ServerTest message handlers

BOOL DLG_ServerTest::OnInitDialog() 
{
	mainwnd=this;
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	dwTimer=SetTimer(0,2000,NULL);
	Sizer.InitSizer(this,10,HKEY_LOCAL_MACHINE,"SOFTWARE\\WiredPlane\\ServerTestPadWnd");
	Sizer.SetOptions(STICKABLE);
	iServerIndex=-1;
	m_Port="5432";
	m_Status="No active servers";
	UpdateData(FALSE);
	OnStart();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DLG_ServerTest::OnDestroy() 
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
	KillTimer(dwTimer);
}

void DLG_ServerTest::OnStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CActionError error;
	if(iServerIndex<0){
		iServerIndex=CreateServer(atol(m_Port),3,10,3000,error);
		OnXmlRpcServerDataReceived(iServerIndex,OnDataReceive);
		OnXmlRpcServerError(iServerIndex,OnError);
		OnXmlRpcServerDataSended(iServerIndex,OnDataSend);
		AddXmlRpcMethod(iServerIndex,"examples.echo",ExamplesEcho,error);
		StartServer(iServerIndex,error);
		if(!error.allOk()){
			AfxMessageBox(Format("Server creation error! index=%i, errcode=%i, errstr=%s",iServerIndex,error.getErrorCode(),error.getErrorDsc()));
		}else{
			m_Status=Format("Server %i, port %i, active",iServerIndex,atol(m_Port));
		}
		UpdateData(FALSE);
	}else{
		AfxMessageBox("First stop prevous server!");
	}
}

void DLG_ServerTest::OnStop() 
{
	// TODO: Add your control notification handler code here
	CActionError error;
	if(iServerIndex>=0){
		StopServer(iServerIndex,error);
		AfxMessageBox(Format("Server stopped! errcode=%i",error.getErrorCode()));
		iServerIndex=-1;
		m_Status="No active servers";
		mainwnd->GetDlgItem(IDC_CONTENT)->SetWindowText("");
		m_Statistics="";
		UpdateData(FALSE);
	}else{
		AfxMessageBox("First start server!");
	}
}

void DLG_ServerTest::OnGetstatistic() 
{
	// TODO: Add your control notification handler code here
	CXmlRpcServerStatistics info;
	GetServerStatistics(iServerIndex,info);
	m_Statistics=Format("total:%i\r\nerrs:%i\r\nbytes:%i\r\nSended:%i",info.lRequestHandled_Total,info.lRequestHandled_Error,info.lTotalBytesReceived,info.lTotalBytesSended);
	UpdateData(FALSE);
}

LRESULT DLG_ServerTest::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}	
	return CDialog::WindowProc(message, wParam, lParam);
}

void DLG_ServerTest::OnUpdate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
}

void DLG_ServerTest::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(bNeedUpdate==1){
		bNeedUpdate=0;
		OnGetstatistic();
		OnUpdate();
	}
	CDialog::OnTimer(nIDEvent);
}
