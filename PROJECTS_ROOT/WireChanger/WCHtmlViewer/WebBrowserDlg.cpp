// WebBrowserDlg.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WebBrowserDlg.h"
#include "WebBrowserDlgDlg.h"

#include "Custsite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserDlgApp

BEGIN_MESSAGE_MAP(CWebBrowserDlgApp, CWinApp)
	//{{AFX_MSG_MAP(CWebBrowserDlgApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserDlgApp construction

CWebBrowserDlgApp::CWebBrowserDlgApp()
{
	m_pDispOM = NULL;

#ifdef _DEBUG
	m_memState = NULL;
#endif
}

CWebBrowserDlgApp::~CWebBrowserDlgApp()
{
	if ( m_pDispOM != NULL)
	{
		//This is the way that the driller sample disposes 
		//the m_pDispOM object.
		//Note that the CImpIDispatch::Release() method 
		//is (intentionally?) never disposing its object! 
		delete m_pDispOM;
	}

#ifdef _DEBUG
	if (m_memState != NULL)
	{
		m_memState->DumpAllObjectsSince();

		delete m_memState;
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWebBrowserDlgApp object

CWebBrowserDlgApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserDlgApp initialization

BOOL CWebBrowserDlgApp::InitInstance()
{
	//AfxEnableControlContainer();

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(new CCustomOccManager);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

#ifdef _DEBUG
	m_memState = new CMemoryState;
	m_memState->Checkpoint();
#endif
	InitCommonControls();
	CWebBrowserDlgDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	return FALSE;
}

char UniCode(const char* szValue)
{
	char szStr[5];
	memcpy(szStr,szValue,4);
	szStr[4]=0;
	WCHAR wChar[2];
	wChar[0]=(unsigned short)strtol(szStr,NULL,16);
	wChar[1]=0;
	char cRes;
	WideCharToMultiByte(1251,0,(LPCWSTR)wChar,1,&cRes,1,NULL,NULL);
	return cRes;
}

char HexCode(const char* szRawValue)
{
	unsigned int cRes;
	char szValue[3]="12";
	szValue[0]=szRawValue[0]>='a'?(szRawValue[0]-'a'+'A'):szRawValue[0];
	szValue[1]=szRawValue[1]>='a'?(szRawValue[1]-'a'+'A'):szRawValue[1];
	if (szValue[0]<='9')
		cRes=szValue[0]-'0';
	else
		cRes=10+szValue[0]-'A';
	cRes=cRes*16;
	if (szValue[1]<='9')
		cRes+=szValue[1]-'0';
	else
		cRes+=10+szValue[1]-'A';
	return cRes;
}

CString UnescapeString(const char* szString)
{
	CString sValue="";
	int nLen=lstrlen(szString);
	const char* szNow=szString;
	while(nLen>0){
		switch (*szNow)
		{
			case '%':
				szNow++;
				nLen--;
				if (szNow[0]=='u'){
					szNow++;
					nLen--;
					char cRes=UniCode(szNow);
					if (cRes!=0){
						sValue+=cRes;
						szNow+=4;
						nLen-=4;
					};
				}else{
					// Ноpмальный код %FF
					char cCode=HexCode(szNow);
					sValue+=cCode;
					szNow+=2;
					nLen-=2;
				};
				break;
			case '+':
				sValue+=" ";
				szNow++;
				nLen--;
				break;
			default:
				sValue+=*szNow;
				szNow++;
				nLen--;
		};
	};
	// Для строк оканчивающихся на процент в конец добавляется лишний символ! Его надо отрезать
	if(nLen<0)
		sValue=sValue.Left(sValue.GetLength()-1);
	return sValue;
}


CRect GetDesktopRect()
{
	CRect rt;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);
	return rt;
}

CString TimeFormat(COleDateTime tm,BOOL bDef, BOOL bNoSecs)
{
	SYSTEMTIME EventTime;
	tm.GetAsSystemTime(EventTime);
	char szTmp[128]={0};
	GetTimeFormat(LOCALE_USER_DEFAULT,(bDef?0:TIME_FORCE24HOURFORMAT)|(bNoSecs?TIME_NOSECONDS:0),&EventTime,0,szTmp,sizeof(szTmp));
	CString sRes=szTmp;
	/*if(sRes.GetLength()==3){
		sRes=CString("0")+sRes;
	}*/
	return sRes;
}

CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString)
{
	int iFrom=0;
	if(iFrom>=(int)strlen(szString)){
		iFrom=-1;
		return "";
	}
	CString sRes="";
	const char* szSearchFrom=szString+iFrom;
	char* szFrom=(char*)strstr(szString+iFrom,szStart);
	if(!szFrom){
		iFrom=-1;
		return "";
	}
	iFrom=(szFrom-szString)+strlen(szStart);
	const char* szSearchFromAfterStart=szString+iFrom;
	const char* szBeginTmp=(szFrom+strlen(szStart));
	//sRes=szBeginTmp;
	if(szEnd!=NULL){
		const char* szEndPos=strstr(szBeginTmp,szEnd);
		if(szEndPos){
			sRes=CString(szBeginTmp,szEndPos-szBeginTmp);
			iFrom+=strlen(sRes)+strlen(szEnd);
			const char* szSearchFromEnd=szString+iFrom;
		}else{
			iFrom=strlen(szString);
			sRes="";
		}
	}else{
		sRes=szBeginTmp;
		sRes.TrimRight();
		iFrom+=strlen(sRes);
	}
	return sRes;
}

void ConvertToArray(const char* szData, CStringArray& aStrings, CString c, char cComment)
{
	CString sData=szData;
	CString sChar=c;
	
	while(sData!=""){
		CString sLine=sData.SpanExcluding(c);
		if(sData.GetLength()==sLine.GetLength()){
			// Дошли до конца
			sData="";
		}
		if(sData.GetLength()>sLine.GetLength()){
			sData=sData.Mid(sLine.GetLength());
		}
		sLine.TrimLeft(c);
		sLine.TrimRight(c);
		sData.TrimLeft(c);
		sData.TrimRight(c);
		if(sLine!=""){
			if(cComment==0 || sLine[0]!=cComment){
				aStrings.Add(sLine);
			}
		}
	}
}
