// DataXMLSaver.cpp: implementation of the CDataXMLSaver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SupportClasses.h"
#include "DataXMLSaver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CDataXMLSaver::Init()
{_XLOG_
	m_bAppendMode=FALSE;
	m_bRead=TRUE;
	m_sSource="";
	m_sRoot="DATA";
	m_pFile=NULL;
	m_sFileName="";
	bFileLoaded=FALSE;
	m_bDataSaved=FALSE;
}

CDataXMLSaver::CDataXMLSaver(const char* pString)
{_XLOG_
	Init();
	SetRoot("");
	SetSource(pString);
}

CDataXMLSaver::CDataXMLSaver(const char* szFile, const char* szRoot, BOOL bRead, BOOL bAppendMode)
{_XLOG_
	Init();
	m_bRead=bRead;
	m_bAppendMode=bAppendMode;
	SetRoot(szRoot);
	m_sFileName=szFile;
	if(m_bRead){_XLOG_
		LoadDataToSource(szFile);
	}
}

BOOL CDataXMLSaver::LoadDataToSource(const char* szFile)
{
	if(strlen(m_sFileName)==0 || ReadFile(m_sFileName,m_sSource)==FALSE){
		return FALSE;
	}
	if(m_sRoot.GetLength()>0){
		SetSource(GetInstringPart((CString)"<"+m_sRoot+">",(CString)"</"+m_sRoot+">",m_sSource));
	}
	bFileLoaded=TRUE;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// The 7-bit alphabet used to encode binary information
int CBase64XXX::m_nMask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };
CBase64XXX::CBase64XXX(BOOL bUseWierd)
{
	if(bUseWierd){
		m_sBase64Alphabet = _T( "ghijkstuDEFGHIvwxyz+ABCJ89KLM0123abcdef45NOPQRSTlmnopqrUVWXYZ/67" );
	}else{
		m_sBase64Alphabet = _T( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" );
	}
}

CBase64XXX::~CBase64XXX()
{
}

CString CBase64XXX::Encode(LPCTSTR szEncoding, int nSize)
{
    int nNumBits = 6;
    UINT nDigit = 0;
    int lp = 0;
    if( szEncoding == NULL ){
        return "";
	}
    m_szInput = szEncoding;
	if(nSize==-1){
		nSize=strlen(szEncoding);
	}
	DWORD dwNewLen=1+(nSize+100)*4/3;
	//CString sOutput = _T( "" );
	char* szOutput=new char[dwNewLen];
	if(szOutput==0){
		return "";
	}
	memset(szOutput,0,dwNewLen);
	int iOutputPos=0;
    m_nInputSize = nSize;
    m_nBitsRemaining = 0;
    nDigit = read_bits( nNumBits, &nNumBits, lp );
    while( nNumBits > 0 )
    {
        szOutput[iOutputPos++]=m_sBase64Alphabet[ (int)nDigit ];
        nDigit = read_bits( nNumBits, &nNumBits, lp );
    }
    // Pad with '=' as per RFC 1521
    while( iOutputPos % 4 != 0 )
    {
        szOutput[iOutputPos++]='=';
    }
    CString sRes=szOutput;
    delete[] szOutput;
    return sRes;
}

// The size of the output buffer must not be less than
// 3/4 the size of the input buffer. For simplicity,
// make them the same size.
CString CBase64XXX::Decode(LPCTSTR szDecoding, int nSize, char* szOutputDirect)
{
    CString sInput, ResultStr;
    int c, lp =0;
    int nDigit;
    int nDecode[ 256 ];
    if(nSize==-1){
		nSize=strlen(szDecoding);
	}
	LPTSTR szOutput = szOutputDirect?szOutputDirect:ResultStr.GetBuffer(nSize); // Output is never longer than input
    if( szDecoding == NULL )
        return _T("");
    sInput = szDecoding;
    if( sInput.GetLength() == 0 )
        return _T("");
    m_lBitStorage = 0;
    m_nBitsRemaining = 0;

    // Build Decode Table
    //
    int i;
    for( i = 0; i < 256; i++ )
    {
        nDecode[i] = -2; // Illegal digit
	}
    for( i=0; i < 64; i++ )
    {
        nDecode[ m_sBase64Alphabet[ i ] ] = i;
        nDecode[ m_sBase64Alphabet[ i ] | 0x80 ] = i; // Ignore 8th bit
        nDecode[ '=' ] = -1; 
        nDecode[ '=' | 0x80 ] = -1; // Ignore MIME padding char
        nDecode[ '\n'] = -1; // Skip the CR/LFs
        nDecode[ '\r'] = -1;
    }

    // Decode the Input
    //
    for( lp = 0, i = 0; lp < sInput.GetLength(); lp++ )
    {
        c = sInput[ lp ];
        nDigit = nDecode[ c & 0x7F ];
        if( nDigit < -1 ) 
        {
            return _T("");
        } 
        else if( nDigit >= 0 ) 
            // i (index into output) is incremented by write_bits()
            write_bits( nDigit & 0x3F, 6, szOutput, i );
    } 
    if(szOutputDirect==NULL){  
		ResultStr.ReleaseBuffer(i);
    }
    return ResultStr;
}

UINT CBase64XXX::read_bits(int nNumBits, int * pBitsRead, int& lp)
{
    ULONG lScratch;
    while( ( m_nBitsRemaining < nNumBits ) && 
           ( lp < m_nInputSize ) ) 
    {
        int c = m_szInput[ lp++ ];
        m_lBitStorage <<= 8;
        m_lBitStorage |= (c & 0xff);
        m_nBitsRemaining += 8;
    }
    if( m_nBitsRemaining < nNumBits ) 
    {
        lScratch = m_lBitStorage << ( nNumBits - m_nBitsRemaining );
        *pBitsRead = m_nBitsRemaining;
        m_nBitsRemaining = 0;
    } 
    else 
    {
        lScratch = m_lBitStorage >> ( m_nBitsRemaining - nNumBits );
        *pBitsRead = nNumBits;
        m_nBitsRemaining -= nNumBits;
    }
    return (UINT)lScratch & m_nMask[nNumBits];
}


void CBase64XXX::write_bits(UINT nBits,
                         int nNumBits,
                         LPTSTR szOutput,
                         int& i)
{
    UINT nScratch;

    m_lBitStorage = (m_lBitStorage << nNumBits) | nBits;
    m_nBitsRemaining += nNumBits;
    while( m_nBitsRemaining > 7 ) 
    {
        nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
        szOutput[ i++ ] = nScratch & 0xFF;
        m_nBitsRemaining -= 8;
    }
}

BOOL CDataXMLSaver::CryptBody(CString sTag)
{
	CBase64XXX dc(1);
	m_sSource=Format("<%s>%s</%s>",sTag,dc.Encode(m_sSource),sTag);
	return TRUE;
}

BOOL CDataXMLSaver::UncryptBody(CString sTag)
{
	CString sCrypted;
	getValue(sTag,sCrypted);
	if(sCrypted==""){
		return FALSE;
	}
	CBase64XXX dc(1);
	m_sSource=dc.Decode(sCrypted);
	return TRUE;
}

void CDataXMLSaver::SetRoot(const char* szRootName)
{
	m_sRoot=szRootName;
}

CString CDataXMLSaver::GetResult()
{
	CString sOut;
	if(m_sRoot.GetLength()>0){
		sOut+="<";
		sOut+=m_sRoot;
		sOut+=">\n";
	}
	sOut+=m_sSource;
	if(m_sRoot.GetLength()>0){
		sOut+="</";
		sOut+=m_sRoot;
		sOut+=">\n";
	}
	return sOut;
}

void CDataXMLSaver::SetSource(const char* szSource)
{
	m_sSource=szSource;
	return;
}

CString CDataXMLSaver::GetSource()
{
	return m_sSource;
}

BOOL CDataXMLSaver::SetAsSaved(BOOL bRes)
{
	m_bDataSaved=bRes;
	return TRUE;
}

BOOL CDataXMLSaver::SaveDataIntoSource(const char* szFileName)
{
	m_bDataSaved=FALSE;
	if(szFileName==NULL){
		szFileName=m_sFileName;
	}
	if(strlen(szFileName)!=0){
		CString sFileContent;
		if(m_bAppendMode){
			sFileContent+="\n<?wk-xml-warning:XML-incompatible append?>\n";
		}
		if(m_sRoot.GetLength()>0){
			sFileContent+="<?xml version='1.0'?>\n\n<";
			sFileContent+=m_sRoot;
			sFileContent+=">\n";
		}
		sFileContent+=m_sSource;
		sFileContent.TrimRight();
		if(m_sRoot.GetLength()>0){
			sFileContent+="\n</";
			sFileContent+=m_sRoot;
			sFileContent+=">\n";
		}
		CString sPrevContent;
		if(m_bAppendMode){
			ReadFile(szFileName,sPrevContent);
		}
		BOOL bRes=SaveFile(szFileName,sPrevContent+sFileContent);
		SetAsSaved(bRes);
	}
	return m_bDataSaved;
}

CDataXMLSaver::~CDataXMLSaver()
{_XLOG_
	if(!m_bRead && !m_bDataSaved){_XLOG_
		SaveDataIntoSource(m_sFileName);
	}
}

COleDateTime CDataXMLSaver::Str2OleDate(CString sData)
{_XLOG_
	COleDateTime dt;
	if(sData=="invalid"){_XLOG_
		dt.SetStatus(COleDateTime::invalid);
	}else{_XLOG_
		//sData=dt.Format("%d.%m.%Y %H:%M:%S");
		int iDotPos1=sData.Find(".")+1;
		if(iDotPos1==0){_XLOG_
			dt.ParseDateTime(sData);
			return dt;
		}
		int iDotPos2=sData.Find(".",iDotPos1)+1;
		if(iDotPos2==0){_XLOG_
			dt.ParseDateTime(sData);
			return dt;
		}
		int iSpacePos=sData.Find(" ",iDotPos2)+1;
		if(iSpacePos==0){_XLOG_
			dt.ParseDateTime(sData);
			return dt;
		}
		int iDDPos1=sData.Find(":",iSpacePos)+1;
		if(iDDPos1==0){_XLOG_
			dt.ParseDateTime(sData);
			return dt;
		}
		int iDDPos2=sData.Find(":",iDDPos1)+1;
		if(iDDPos2==0){_XLOG_
			dt.ParseDateTime(sData);
			return dt;
		}
		int nYear=atol(sData.Mid(iDotPos2));
		int nMonth=atol(sData.Mid(iDotPos1));
		int nDay=atol(sData);
		int nHour=atol(sData.Mid(iSpacePos));
		int nMin=atol(sData.Mid(iDDPos1));
		int nSec=atol(sData.Mid(iDDPos2));
		dt=COleDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
	}
	return dt;
}

BOOL CDataXMLSaver::getValue(const char* szKey,COleDateTime& dt, COleDateTime dtDefault)
{_XLOG_
	CString sData;
	if(!getValue(szKey,sData,"",XMLAsAttribute) || sData==""){_XLOG_
		dt=dtDefault;
		return FALSE;
	}
	dt=Str2OleDate(sData);
	return TRUE;
}

BOOL CDataXMLSaver::getValue(const char* szKey, DWORD& iParam, DWORD iDefault)
{
	CString sData;
	if(!getValue(szKey,sData,"",XMLAsAttribute) || sData==""){
		iParam=iDefault;
		return FALSE;
	}
	iParam=atol(sData);
	return TRUE;
}

BOOL CDataXMLSaver::getValue(const char* szKey,int& iParam, int iDefault)
{
	CString sData;
	if(!getValue(szKey,sData,"",XMLAsAttribute) || sData==""){
		iParam=iDefault;
		return FALSE;
	}
	iParam=atol(sData);
	return TRUE;
}

BOOL CDataXMLSaver::getValue(const char* szKey,long& lParam, long lDefault)
{
	CString sData;
	if(!getValue(szKey,sData,"",XMLAsAttribute) || sData==""){
		lParam=lDefault;
		return FALSE;
	}
	lParam=atol(sData);
	return TRUE;
}

BOOL CDataXMLSaver::getValue(const char* szKey,double& dParam, double dDefault)
{
	CString sData;
	if(!getValue(szKey,sData,"",XMLAsAttribute) || sData==""){
		dParam=dDefault;
		return FALSE;
	}
	dParam=atof(sData);
	return TRUE;
}

CString CDataXMLSaver::getValue(const char* szKey)
{
	CString sRes;
	getValue(szKey,sRes,"");
	return sRes;
}

BOOL CDataXMLSaver::getValue(const char* szKey, CString& sParam, const char* sDefault, DWORD dwOptions)
{
	if(dwOptions & XMLAsAttribute){
		char szLBuffer[256];
		sprintf(szLBuffer,"<%s value=\"",szKey);
		char szRBuffer[256]="\"/>";
		sParam=GetInstringPart(szLBuffer,szRBuffer,m_sSource,dwOptions);
		if(sParam.GetLength()==0){
			if(!getValue(szKey,sParam)){
				sParam=sDefault;
				return FALSE;
			}
		}
	}else{
		char szLBuffer[256];
		sprintf(szLBuffer,"<%s>",szKey);
		char szRBuffer[256];
		sprintf(szRBuffer,"</%s>",szKey);
		int iFrom=0;
		sParam=GetInstringPart(szLBuffer,szRBuffer,m_sSource,dwOptions,&iFrom);
		if(iFrom==-1){
			sParam=sDefault;
			return FALSE;
		}
	}
	if(dwOptions & XMLTrimSpaces){
		sParam.TrimLeft();
		sParam.TrimRight();
	}
	return TRUE;
}


CString CDataXMLSaver::OleDate2Str(COleDateTime dt)
{_XLOG_
	CString sData="invalid";
	if(dt.GetStatus()==COleDateTime::valid){_XLOG_
		sData=dt.Format("%d.%m.%Y %H:%M:%S");
	}
	return sData;
}

BOOL CDataXMLSaver::putValue(const char* szKey,COleDateTime dt)
{_XLOG_
	return putValue(szKey,OleDate2Str(dt),XMLAsAttribute);
}

BOOL CDataXMLSaver::putValue(const char* szKey,DWORD iParam)
{
	CString sData;
	sData.Format("%i",iParam);
	return putValue(szKey,sData,XMLAsAttribute);
}

BOOL CDataXMLSaver::putValue(const char* szKey,int iParam)
{
	CString sData;
	sData.Format("%i",iParam);
	return putValue(szKey,sData,XMLAsAttribute);
}

BOOL CDataXMLSaver::putValue(const char* szKey,long lParam)
{
	CString sData;
	sData.Format("%i",lParam);
	return putValue(szKey,sData,XMLAsAttribute);
}

BOOL CDataXMLSaver::putValue(const char* szKey,double dParam)
{
	CString sData;
	sData.Format("%f",dParam);
	return putValue(szKey,sData,XMLAsAttribute);
}

BOOL CDataXMLSaver::putValue(const char* szKey,const char* sParam, DWORD dwOptions)
{
	m_sSource+="<";
	m_sSource+=szKey;
	if(dwOptions & XMLAsAttribute){
		m_sSource+=" value=\"";
	}else{
		m_sSource+=">";
		if(dwOptions & XMLWithStartCR){
			m_sSource+="\n";
		}
	}
	CString sValue=sParam;
	if(!(dwOptions & XMLNoConversion)){
		Str2Xml(sValue,dwOptions);
	}
	if(dwOptions & XMLTrimSpaces){
		sValue.TrimLeft();
		sValue.TrimRight();
	}
	m_sSource+=sValue;
	if(dwOptions & XMLAsAttribute){
		m_sSource+="\"/>";
	}else{
		if(dwOptions & XMLWithEndCR){
			m_sSource+="\n";
		}
		m_sSource+="</";
		m_sSource+=szKey;
		m_sSource+=">";
	}
	if(m_sFileName.GetLength()>0){
		m_sSource+="\n";
	}
	return TRUE;
}

CString CDataXMLSaver::GetInstringPart(const char* szStart, const char* szString, DWORD dwOptions, int* iFrom)
{
	int iPositionToLookFrom=0;
	if(iFrom){
		iPositionToLookFrom=*iFrom;
	}
	CString sRes=GetInstringPart(szStart, NULL, szString,iPositionToLookFrom,dwOptions);
	if(iFrom){
		*iFrom=iPositionToLookFrom;
	}
	return sRes;
}

CString CDataXMLSaver::GetInstringPartGreedly(const char* szStart, const char* szEnd, const char* szString, DWORD dwOptions, int* iFrom)
{
	int iPositionToLookFrom=0;
	if(iFrom){
		iPositionToLookFrom=*iFrom;
	}
	CString sRes=GetInstringPartGreedly(szStart, szEnd, szString,iPositionToLookFrom,dwOptions);
	if(iFrom){
		*iFrom=iPositionToLookFrom;
	}
	return sRes;
}

CString CDataXMLSaver::GetInstringPart(const char* szStart, const char* szEnd, const char* szString, DWORD dwOptions, int* iFrom)
{
	int iPositionToLookFrom=0;
	if(iFrom){
		iPositionToLookFrom=*iFrom;
	}
	CString sRes=GetInstringPart(szStart, szEnd, szString,iPositionToLookFrom,dwOptions);
	if(iFrom){
		*iFrom=iPositionToLookFrom;
	}
	return sRes;
}

BOOL CDataXMLSaver::StripInstringPart(const char* szStart, const char* szEnd, CString& szString, int* iFrom, BOOL bIgnoreCase, int iStartOffset)
{
	if(iFrom && *iFrom>=(int)strlen(szString)){
		*iFrom=-1;
		return 0;
	}
	CString szStringU;
	int iBeginFrom=iFrom?(*iFrom):0;
	int iStartPos=-1;
	if(bIgnoreCase){
		szStringU=szString;
		CString szStartU=szStart;
		szStringU.MakeUpper();
		szStartU.MakeUpper();
		iStartPos=szStringU.Find(szStartU,iBeginFrom);
	}else{
		iStartPos=szString.Find(szStart,iBeginFrom);
	}
	if(iStartPos!=-1){
		int iEndPos=-1;
		if(bIgnoreCase){
			CString szEndU=szEnd;
			szEndU.MakeUpper();
			iEndPos=(szEndU!="")?szStringU.Find(szEndU,iStartPos+iStartOffset):-1;
		}else{
			iEndPos=szEnd?szString.Find(szEnd,iStartPos+iStartOffset):-1;
		}
		if(iEndPos!=-1){
			szString=szString.Left(iStartPos+iStartOffset)+szString.Mid(iEndPos+strlen(szEnd));
		}else{
			szString=szString.Left(iStartPos+iStartOffset);
		}
	}
	return 1;
}

BOOL CDataXMLSaver::ReplaceInstringPart(const char* szStart, const char* szEnd, const char* szWith, CString& szString, int* iFrom)
{
	if(iFrom && *iFrom>=(int)strlen(szString)){
		*iFrom=-1;
		return 0;
	}
	int iBeginFrom=iFrom?(*iFrom):0;
	int iStartPos=szString.Find(szStart,iBeginFrom);
	if(iStartPos!=-1){
		int iEndPos=szEnd?szString.Find(szEnd,iStartPos):-1;
		if(iEndPos!=-1){
			szString=szString.Left(iStartPos)+szStart+szWith+szEnd+szString.Mid(iEndPos+strlen(szEnd));
		}else{
			szString=szString.Left(iStartPos)+szStart+szWith+szEnd;
		}
	}
	return 1;
}

int CountChars(CString& sSource, const char* szWhat,int iFrom,int iTo)
{
	int iRes=0;
	for(int i=iFrom;i<iTo && i<sSource.GetLength();i++){
		if(szWhat && strchr(szWhat,sSource[i])){
			iRes++;
		}
	}
	return iRes;
}

int ReverseFind(CString& sSource, const char* szWhat,int iFrom)
{
	int iPrevRes=sSource.Find(szWhat), iRes=iPrevRes, iLen=sSource.GetLength();
	if(iFrom>=0){
		// искать начиная с iFrom
		while(iPrevRes!=-1 && iPrevRes>=iFrom && iPrevRes<iLen){
			iPrevRes=sSource.Find(szWhat,iPrevRes+1);
			if(iPrevRes!=-1 && iPrevRes>=iFrom && iPrevRes<iLen){
				iRes=iPrevRes;
			}
		}
	}else{
		// искать до iFrom
		iLen=-iFrom;
		iFrom=0;
		while(iPrevRes!=-1 && iPrevRes>=iFrom && iPrevRes<iLen){
			iPrevRes=sSource.Find(szWhat,iPrevRes+1);
			if(iPrevRes!=-1 && iPrevRes>=iFrom && iPrevRes<iLen){
				iRes=iPrevRes;
			}
		}
	}
	return iRes;
}

CString CDataXMLSaver::GetInstringPartGreedly(const char* szStart, const char* szEnd, const char* szString, int& iFrom, DWORD dwOptions)
{
	if(iFrom>=(int)strlen(szString)){
		iFrom=-1;
		return "";
	}
	CString sRes="";
	const char* szSearchFrom=szString+iFrom;
	char* szFrom=(char*)strstr(szString+iFrom,szStart);
	if(szFrom){
		sRes=(const char*)(szFrom+strlen(szStart));
		iFrom=(szFrom-szString)+strlen(szStart);
		const char* szSearchFromAfterStart=szString+iFrom;
	}else{
		iFrom=-1;
		return "";
	}
	if(szEnd!=NULL){
		int iEnd=ReverseFind(sRes,szEnd);
		if(iEnd!=-1){
			sRes=sRes.Left(iEnd);
			iFrom+=strlen(sRes)+strlen(szEnd);
			const char* szSearchFromEnd=szString+iFrom;
		}else{
			sRes="";
		}
	}else{
		sRes.TrimRight();
		iFrom+=strlen(sRes);
	}
	Xml2Str(sRes,dwOptions);
	return sRes;
}

CString CDataXMLSaver::GetInstringPart(const char* szStart, const char* szEnd, const char* szString, int& iFrom, DWORD dwOptions, const char* szStringOriginal)
{
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
	if(szEnd!=NULL){
		const char* szEndPos=strstr(szBeginTmp,szEnd);
		if(szEndPos){
			if(szStringOriginal){
				sRes=CString(szStringOriginal+(szBeginTmp-szString),szEndPos-szBeginTmp);
			}else{
				sRes=CString(szBeginTmp,szEndPos-szBeginTmp);
			}
			iFrom+=strlen(sRes)+strlen(szEnd);
			const char* szSearchFromEnd=szString+iFrom;
		}else{
			iFrom=strlen(szString);
			sRes="";
		}
	}else{
		if(dwOptions & QT_FORWARD2){
			if(szStringOriginal){
				sRes=CString(szStringOriginal+(szBeginTmp-szString),2);
			}else{
				sRes=CString(szBeginTmp,2);
			}
		}else{
			if(szStringOriginal){
				sRes=CString(szStringOriginal+(szBeginTmp-szString));
			}else{
				sRes=szBeginTmp;
			}
			sRes.TrimRight();
		}
		iFrom+=strlen(sRes);
	}
	Xml2Str(sRes,dwOptions);
	return sRes;
}

void CDataXMLSaver::Str2Xml(CString& sRes, DWORD dwFlags)
{
	if((dwFlags & XMLRNConversion) || (dwFlags & XMLJScriptConv)){
		sRes.Replace("\r\n","\n");
	}
	if(!(dwFlags & XMLNoConversion)){
		sRes.Replace("&","&amp;");
		sRes.Replace("\"","&quot;");
		if(!(dwFlags & XMLNoLTGTConv)){
			sRes.Replace("<","&lt;");
			sRes.Replace(">","&gt;");
		}
	}
	if(dwFlags & XMLJScriptConv){
		sRes.Replace("\\","&#92;");
		sRes.Replace("'","&#39;");
		sRes.Replace("\"","&#34;");
		sRes.Replace("\n","<br>");
	}
	return;
}

void CDataXMLSaver::Xml2Str(CString& sRes, DWORD dwFlags)
{
	if(dwFlags & XMLRNConversion){
		sRes.Replace("\n","\r\n");
	}
	if(!(dwFlags & XMLNoConversion)){
		sRes.Replace("&lt;","<");
		sRes.Replace("&gt;",">");
		sRes.Replace("&quot;","\"");
		sRes.Replace("&amp;","&");
	}
	if(dwFlags & XMLJScriptConv){
		sRes.Replace("&quot;","\"");
		sRes.Replace("&#92;","\\");
		sRes.Replace("&#39;","'");
		sRes.Replace("&#34;","\"");
		sRes.Replace("<br>","\n");
	}
	return;
}

// Считывание в буффер. Вызывающая функция должна освободить буфер после использования
BOOL ReadFileToBuffer(const char* sStartDir, LPVOID hGlobalBuffer, DWORD dwFileSize)
{_XLOG_
	BOOL bRes=FALSE;
	if(!hGlobalBuffer){_XLOG_
		return bRes;
	}
	HANDLE hFile = ::CreateFile(sStartDir, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile==INVALID_HANDLE_VALUE ){_XLOG_
		return FALSE;
	}
	DWORD dwBytesRead=0;
	if (dwFileSize > 0){_XLOG_
		::ReadFile(hFile, hGlobalBuffer, dwFileSize, &dwBytesRead, NULL);
	}
	if (dwFileSize != dwBytesRead){_XLOG_
		bRes=FALSE;
	}else{_XLOG_
		bRes=TRUE;
	}
	::CloseHandle(hFile);
	return bRes;
}

DWORD GetFileSize(const char* szFileName)
{_XLOG_
	DWORD dwFileSize=0;
	FILE* f=fopen(szFileName,"rb");
	if (f==NULL){_XLOG_
		return 0;
	}
	fseek(f,0,SEEK_END);
	dwFileSize = ftell(f);
	fclose(f);
	return dwFileSize;
}

//bAloowWPTextExtensions
//==0 просто чтение
//==1 если юникод то юникод
//==2 проверяем на экзекутабельность
//==гарантировано юникодный файл должен быть
BOOL ReadFile(const char* szFileName, CString& sFileContent, BOOL bAloowWPTextExtensions)
{_XLOG_
	if(szFileName==NULL || strlen(szFileName)==0){_XLOG_
		return FALSE;
	}
	DWORD dwSize=GetFileSize(szFileName);
	if(dwSize==0){_XLOG_
		return FALSE;
	}
	if(bAloowWPTextExtensions==2 && IsExecutableFile(szFileName)){
		// Перехватим консоль
		char sTempFolder[MAX_PATH]={0};
		GetTempPath(sizeof(sTempFolder),sTempFolder);
		static long t=0;
		CString sFileNameTmp=Format("%s_contmp_%i_%i.txt",sTempFolder,t++,time(NULL));
#ifdef _DEBUG
		sFileNameTmp="c:\\tmp.wc.txt";
#endif
		CString sLine=Format("/Q /U /C \"\"%s\" >\"%s\"\"",szFileName,sFileNameTmp);
		SHELLEXECUTEINFO sei;
		ZeroMemory(&sei,sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.lpFile = "cmd.exe";
		sei.lpParameters = sLine;
		sei.lpVerb = "";
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.nShow = SW_HIDE;
		if(ShellExecuteEx(&sei)){
			WaitForSingleObject(sei.hProcess,5000);
		}
		ReadFile(sFileNameTmp,sFileContent,3);
#ifndef _DEBUG
		DeleteFile(sFileNameTmp);
#endif
		return TRUE;
	}
	FILE* f=fopen(szFileName,"rb");
	if(f==NULL){_XLOG_
		return FALSE;
	}
	BOOL bSimple=1;
	char* szData = new char[dwSize+2];
	memset(szData,0,dwSize+2);
	DWORD dwReadedBytes=fread(szData,1,dwSize,f);
	if(bAloowWPTextExtensions && dwSize>3 && dwSize%2==0){_XLOG_
		static const BYTE bszWPPrefix[]={0xFF,0xFE,'[',0,'W',0,'P',0,'C',0,'O',0,'D',0,'E',0,':'};
		BOOL bBom=(BYTE(szData[0])==0xFF && BYTE(szData[1])==0xFE);
		int iFlags=IS_TEXT_UNICODE_SIGNATURE|IS_TEXT_UNICODE_ASCII16|IS_TEXT_UNICODE_ODD_LENGTH|IS_TEXT_UNICODE_REVERSE_ASCII16;
		BOOL bCustUni=(bAloowWPTextExtensions==3 && szData[1]==0 && szData[3]==0);//(szData[1]==0 && IsTextUnicode(szData,dwSize,&iFlags));
		if(bBom || bCustUni){_XLOG_
			if(memcmp(szData,bszWPPrefix,sizeof(bszWPPrefix)-1)==0){_XLOG_
				// Юникод!!!
				wchar_t* szUBuffer=(wchar_t*)szData;
				wchar_t* szUBCode=wcschr(szUBuffer,':');
				DWORD dwCodePage=0;
				if(szUBCode){_XLOG_
					dwCodePage=_wtoi(szUBCode+1);
				}
				wchar_t* wszContinue=wcschr(szUBuffer,'\n');
				if(dwCodePage>0 && wszContinue){_XLOG_
					wszContinue+=1;
					char* szData2 = new char[dwSize/2+1];
					memset(szData2,0,dwSize/2+1);
					DWORD dwRes=::WideCharToMultiByte(dwCodePage,0,wszContinue,-1,szData2,dwSize/2+1,0,0);
#ifdef _DEBUG
					DWORD dwErr=GetLastError();
#endif

					sFileContent=szData2;
					delete[] szData2;
					bSimple=0;
				}
			}else{_XLOG_
				wchar_t* szUBuffer=(wchar_t*)szData;
				USES_CONVERSION;
				sFileContent=W2A(szUBuffer);
				bSimple=0;
			}
		}
	}
	if(bSimple){_XLOG_
		sFileContent=szData;
	}
	delete[] szData;
	fclose(f);
	// Проверяем и выходим
	if(dwReadedBytes!=dwSize){_XLOG_
		return FALSE;
	}
	return TRUE;
}

BOOL ReadFile(const char* szFileName, wchar_t*& wszFileContent)
{_XLOG_
	wszFileContent=0;
	if(szFileName==NULL || strlen(szFileName)==0){_XLOG_
		return FALSE;
	}
	DWORD dwSize=GetFileSize(szFileName);
	if(dwSize==0){_XLOG_
		return FALSE;
	}
	FILE* f=fopen(szFileName,"rb");
	if(f==NULL){_XLOG_
		return FALSE;
	}
	wszFileContent = (wchar_t*)new char[dwSize+2];
	memset(wszFileContent,0,dwSize+2);
	DWORD dwReadedBytes=fread(wszFileContent,1,dwSize,f);
	fclose(f);
	// Проверяем и выходим
	if(dwReadedBytes!=dwSize){_XLOG_
		return FALSE;
	}
	return TRUE;
}

BOOL IsFileUnicode(const char* szFileName)
{_XLOG_
	if(szFileName==NULL || strlen(szFileName)==0){_XLOG_
		return FALSE;
	}
	DWORD dwSize=GetFileSize(szFileName);
	if(dwSize<2){_XLOG_
		return FALSE;
	}
	FILE* f=fopen(szFileName,"rb");
	if(f==NULL){_XLOG_
		return FALSE;
	}
	BYTE wszFileContent[3]={0};
	DWORD dwReadedBytes=fread(wszFileContent,1,3,f);
	fclose(f);
	return wszFileContent[0]==0xFF && wszFileContent[1]==0xFE;
}

CMapStringToString aFileToContentMap;
BOOL ClearReadFileCash()
{
	aFileToContentMap.RemoveAll();
	return TRUE;
}

BOOL ReadFileCashed(const char* szFileName, CString& sFileContent)
{_XLOG_
#pragma message("-- добавить поддержку определения изменения даты/размера файла и автоперечитывания --")
	if(aFileToContentMap.Lookup(szFileName, sFileContent)){_XLOG_
		return TRUE;
	}
	BOOL bRes=ReadFile(szFileName, sFileContent);
	if(bRes){_XLOG_
		aFileToContentMap.SetAt(szFileName, sFileContent);
	}
	return bRes;
}

/*BOOL ReadFile(const char* szFileName, CString& sFileContent)
{_XLOG_
	DWORD dwSize=GetFileSize(szFileName);
	if(dwSize==0){_XLOG_
		return FALSE;
	}
	HGLOBAL hGlob=::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, dwSize+1);
	if(!ReadFileToBuffer(szFileName, hGlob, dwSize)){_XLOG_
		return FALSE;
	}
	char* szData = (char*)hGlob;
	if(szData){_XLOG_
		szData[dwSize]=0;
		sFileContent=szData;
	}
	VERIFY(::GlobalFree(hGlob)==NULL);
	return TRUE;
}*/

#define MAX_LEN_ENV	20000
void SubstEnvVariables(CString& sText)
{
	// Environment variables
	int iLen=strlen(sText);
	if(iLen>0 && iLen<MAX_LEN_ENV && sText.Find("%")!=-1){
		char szPathOut[MAX_LEN_ENV]="";
		strcpy(szPathOut,sText);
		DWORD dwDoEnv=DoEnvironmentSubst(szPathOut,sizeof(szPathOut));
		if(HIWORD(dwDoEnv)>0){
			sText=szPathOut;
		}
	}
}

CString ExtractInlineScript(CString sFileIcon)
{
	sFileIcon.TrimLeft();
	sFileIcon.TrimRight();
	if(strlen(sFileIcon)>11 && memicmp(sFileIcon,"javascript:",11)==0){
		return sFileIcon.Mid(11);
	}
	if(strlen(sFileIcon)>11 && memicmp(sFileIcon,"jscript:",8)==0){
		return sFileIcon.Mid(8);
	}
	return "";
}

BOOL isFileIExist(const char* szFile)
{_XLOG_
	CString sFile=szFile;
	if(isFileExist(sFile)){_XLOG_
		return 1;
	}
	SubstEnvVariables(sFile);
	sFile=sFile.SpanExcluding(",");
	return isFileExist(sFile);
}

BOOL isFileExist(const char* szFile)
{_XLOG_
	if(szFile==NULL || szFile[0]==0){_XLOG_
		return FALSE;
	}
	char szTmp[MAX_PATH]={0};
	strcpy(szTmp,szFile);
	strlwr(szTmp);
	char* szZipPos=0;
	if((szZipPos=strstr(szTmp,".zip\\"))!=0 || (szZipPos=strstr(szTmp,".wsc\\"))!=0){_XLOG_
		if(szZipPos){_XLOG_
			int iLen=(szZipPos-szTmp);
			CString sZipPath(szFile,iLen+4);
			return isFileExist(sZipPath);
		}
		return FALSE;
	}
	if(szFile[strlen(szFile)-1]=='\\'){_XLOG_
		CString sFile2=szFile;
		sFile2=sFile2.Left(strlen(szFile)-1);
		return isFileExist(sFile2);
	}
	if(int(GetFileAttributes(szFile))==-1){_XLOG_
		return FALSE;
	}
	return TRUE;
}

BOOL isFolder(const char* szFile)
{_XLOG_
	if(szFile==NULL || szFile[0]==0 || !isFileExist(szFile)){_XLOG_
		return FALSE;
	}
	if((GetFileAttributes(szFile) & FILE_ATTRIBUTE_DIRECTORY)!=0){_XLOG_
		return TRUE;
	}
	return FALSE;
}

CString GetPathFolder(const char* szFile)
{_XLOG_
	char szDisk[MAX_PATH]="",szPath[MAX_PATH]="";
	_splitpath(szFile,szDisk,szPath,NULL,NULL);
	CString sKatalog=szDisk;
	sKatalog+=szPath;
	return sKatalog;
}

BOOL PreparePathForWrite(const char* szPath)
{_XLOG_
	//SHPathPrepareForWrite
	CString sKatalog=GetPathFolder(szPath);
	if(!isFileExist(sKatalog)){_XLOG_
		return ::CreateDirectory(sKatalog,NULL);
	}
	//SHPathPrepareForWrite();
	return TRUE;
}

BOOL SaveBuffer(const char* sStartDir, void* sFileContent, DWORD dwSize)
{
	FILE* m_pFile=fopen(sStartDir,"w+b");
	if(!m_pFile){
		return FALSE;
	}
	DWORD nRead=fwrite(sFileContent,sizeof(BYTE),dwSize,m_pFile);
	fclose(m_pFile);
	m_pFile=NULL;
	return (nRead==dwSize);
}

BOOL AppendFile(const char* sStartDir, const char* sFileContent)
{
	FILE* m_pFile=fopen(sStartDir,"a+b");
	if(!m_pFile){_XLOG_
		return FALSE;
	}
	DWORD nRead=fwrite(sFileContent,sizeof(char),strlen(sFileContent),m_pFile);
	fclose(m_pFile);
	return TRUE;
}

BOOL SaveFile(const char* sStartDir, const char* sFileContent)
{_XLOG_
	PreparePathForWrite(sStartDir);
	if(IsExecutableFile(sStartDir)){
		return FALSE;
	}
	FILE* m_pFile=fopen(sStartDir,"w+b");
	if(!m_pFile){_XLOG_
		return FALSE;
	}
	DWORD nRead=fwrite(sFileContent,sizeof(char),strlen(sFileContent),m_pFile);
	fclose(m_pFile);
	m_pFile=NULL;
	CString sFileContentOld;
	if(aFileToContentMap.Lookup(sStartDir, sFileContentOld)){_XLOG_
		aFileToContentMap.RemoveKey(sStartDir);
	}
	return (nRead==strlen(sFileContent));
}

BOOL CCritSectionLock::Init(CSyncObject* pObject, BOOL bInitialLock)
{
	m_pObject=NULL;
	bLocked=bInitialLock;
	if(pObject){
		m_pObject=pObject;
		if(bInitialLock){
			bLocked=m_pObject->Lock();
		}
	}
	return TRUE;
}

CCritSectionLock::CCritSectionLock(CSyncObject& pObject, BOOL bInitialLock):CSingleLock(&pObject, bInitialLock)
{
	Init(&pObject, bInitialLock);
}

CCritSectionLock::CCritSectionLock(CSyncObject* pObject, BOOL bInitialLock):CSingleLock(pObject, bInitialLock)
{
	Init(pObject, bInitialLock);
};

CCritSectionLock::~CCritSectionLock()
{
	if(bLocked){
		m_pObject->Unlock();
	}
}

int b64encode(const char *from,char *to,int length,int quads)
{
	CBase64XXX dc(0);
	CString sOut=dc.Encode(from,length);
	strcpy(to,sOut);
	return 1;
}

CCriticalSection cPackSection;
CString PackToString(const char* szData, DWORD dwDataSize)
{_XLOG_
	CCritSectionLock SL(&cPackSection,TRUE);
	CBase64XXX dc(0);
	CString sOut=dc.Encode(szData,dwDataSize);
	return sOut;
}

BOOL PackFromString(const char* szData, DWORD dwDataSize, HGLOBAL& hGlobal, long lAllocParams)
{_XLOG_
	CCritSectionLock SL(&cPackSection,TRUE);
	hGlobal=NULL;
	const int iAlign=4;
	hGlobal = ::GlobalAlloc((lAllocParams==-1)?(GMEM_FIXED|GMEM_ZEROINIT):lAllocParams, dwDataSize+iAlign);//для алаймента
	if (hGlobal == NULL){_XLOG_
		return FALSE;
	};
	char* sFileContent = 0;
	if(lAllocParams!=-1 && (lAllocParams&GMEM_MOVEABLE)!=0){_XLOG_
		sFileContent = (char*)::GlobalLock(hGlobal);
	}else{_XLOG_
		sFileContent = (char*)hGlobal;
	}
	memset(sFileContent,0,dwDataSize+iAlign);
	CBase64XXX dc(0);
	dc.Decode(szData,dwDataSize,sFileContent);
	BOOL bRes=TRUE;
	if(lAllocParams!=-1 && (lAllocParams&GMEM_MOVEABLE)!=0){_XLOG_
		::GlobalUnlock(hGlobal);
	}
	if(!bRes){_XLOG_
		if(::GlobalFree(hGlobal)!=NULL){_XLOG_
			AfxMessageBox("Memory error!");
		}
		hGlobal=NULL;
	}
	return bRes;
}

CString EscapeString(const WCHAR* szString)
{_XLOG_
	if(!szString || *szString==0){_XLOG_
		return "";
	}
	CString sRes;
	const WCHAR* szPos=szString;
	while(*szPos){
		sRes+=Format("%%u%04X",*szPos);
		szPos++;
	}
	return sRes;
}

CString EscapeStringUTF8(const WCHAR* szString)
{_XLOG_
	if(!szString || *szString==0){_XLOG_
		return "";
	}
	CString sRes;
	USES_CONVERSION;
	const WCHAR* szPos=szString;
	while(*szPos){
		WCHAR szwUTF8[2]={0};
		szwUTF8[0]=*szPos;
		szwUTF8[1]=0;
		const char* szUTF8C=W2A(szwUTF8);
		if(szUTF8C){
			if(szUTF8C[0]>'z' || szUTF8C[0]<'0' || (szUTF8C[0]>'9' && szUTF8C[0]<'A')  || (szUTF8C[0]>'Z' && szUTF8C[0]<'a')){
				sRes+=Format("%%%02X",(unsigned char)szUTF8C[0]);
			}else{
				sRes+=(unsigned char)szUTF8C[0];
			}
		}else{
			sRes+='.';
		}
		szPos++;
	}
	return sRes;
}

char UniCode(const char* szValue)
{_XLOG_
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
{_XLOG_
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
{_XLOG_
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

CString EscapeStringSP(const char* szString)
{_XLOG_
	CString s=szString;
	s.Replace(" ","%20");
	return s;
}

CString EscapeSlashes(const char* szString)
{_XLOG_
	CString s=szString;
	s.Replace("\\","\\\\");
	s.Replace("\"","\\\"");
	return s;
}

CString EscapeString3(const char* szString)
{_XLOG_
	if(!szString || *szString==0){_XLOG_
		return "";
	}
	CString sRes;
	char const* szPos=szString;
	while(*szPos){
		unsigned char c=*szPos;
		sRes+=Format("%%%02X",c);
		szPos++;
	}
	return sRes;
}

CString EscapeString2(const char* szString,CString sAddChars)
{_XLOG_
	if(!szString || *szString==0){_XLOG_
		return "";
	}
	CString sRes;
	char const* szPos=szString;
	while(*szPos){
		unsigned char c=*szPos;
		if(c<40 || sAddChars.Find(c)!=-1 || c>126){
			sRes+=Format("%%%02X",c);
		}else{
			sRes+=(*szPos);
		}
		szPos++;
	}
	return sRes;
}

CString EscapeString(const char* szString,BOOL bUseExUnicod, CString sDelims, int* iExtCounts2, int* iExtCounts4)
{_XLOG_
	if(!szString || *szString==0){_XLOG_
		return "";
	}
	char szTmp[2]="";
	memset(szTmp,0,sizeof(szTmp));
	CString sRes;
	char const* szPos=szString;
	USES_CONVERSION;
	while(*szPos){
		unsigned char c=*szPos;
		if(c<40 || sDelims.Find(c)!=-1){
			if(iExtCounts2){
				*iExtCounts2=*iExtCounts2+1;
			}
			sRes+=Format("%%%02X",c);
		}else if(c>126 && bUseExUnicod){
			if(iExtCounts4){
				*iExtCounts4=*iExtCounts4+1;
			}
			szTmp[0]=c;
			wchar_t* wc=A2W(szTmp);
			sRes+=Format("%%u%04X",*wc);
		}else{
			sRes+=(*szPos);
		}
		szPos++;
	}
	return sRes;
}

#ifdef _INCLUDE_IPV6_HOTKEYS_IN_XMLSAVER
BOOL CDataXMLSaver::getValue(const char* szKey,CIHotkey& lParam, CIHotkey lDefault)
{_XLOG_
	CString sData;
	if(!getValue(szKey,sData,"",XMLAsAttribute) || sData==""){_XLOG_
		lParam=lDefault;
		return FALSE;
	}
	lParam.InitFromSave(sData,FALSE);
	return TRUE;
}

BOOL CDataXMLSaver::putValue(const char* szKey,CIHotkey lParam)
{_XLOG_
	return putValue(szKey,lParam.GetForSave(FALSE),XMLAsAttribute);
}
#endif

CString GetStringHash(const char* szName)
{_XLOG_
	char szOut[10+1]="";
	ZeroMemory(szOut,sizeof(szOut));
	int iPos=0;
	int iHash=0;
	char const* szPos=szName;
	//-----------------------
	int i=0;
	while(*szPos){
		BYTE c=szOut[i];
		c=c<<1 | c>>7;
		c+=*szPos;
		if(c==0 && i==0){
			szOut[i]=6;
		}else{
			szOut[i]=c;
		}
		szPos++;
		i++;
		if(i>(sizeof(szOut)-2)){
			i=0;
		}
	}
	i=0;
	while(szOut[i]!=0){
		BYTE c=szOut[i];
		c='A'+c%('Z'-'A');
		szOut[i]=c;
		i++;
	}
	return szOut;
}

void ArithReplace(CString& sWho, const char* szWhat, const char* szWith)
{_XLOG_
	int iFrom=0;
	while(iFrom!=-1){
		iFrom=sWho.Find(szWhat);
		if(iFrom>=0){
			int iTo=iFrom+strlen(szWhat);
			BOOL bOperType=0;
			int bOperPos=0;
			while(strchr("+-0123456789",sWho[iTo])!=0){
				if(sWho[iTo]=='+'){
					bOperType=1;
					bOperPos=iTo;
				}
				if(sWho[iTo]=='-'){
					bOperType=2;
					bOperPos=iTo;
				}
				iTo++;
			}
			if(bOperType==0){
				sWho=sWho.Left(iFrom)+szWith+sWho.Mid(iTo);
			}
			if(bOperType==1){
				CString szArg=sWho.Mid(bOperPos+1,iTo-(bOperPos+1));
				sWho=sWho.Left(iFrom)+Format("%i",(atol(szWith)+atol(szArg)))+sWho.Mid(iTo);
			}
			if(bOperType==2){
				CString szArg=sWho.Mid(bOperPos+1,iTo-(bOperPos+1));
				sWho=sWho.Left(iFrom)+Format("%i",(atol(szWith)-atol(szArg)))+sWho.Mid(iTo);
			}
		}
	}
}

BOOL DeEntitize(CString& sBodyText)
{_XLOG_
	USES_CONVERSION;
	wchar_t szChar[10]={0};
	BOOL bAnyTagsStripped=FALSE;
	int iFrom=0;
	while(iFrom>=0 && iFrom<sBodyText.GetLength()){
		CString sTextLine=CDataXMLSaver::GetInstringPart("&#",";",sBodyText,iFrom,XMLNoConversion);
		if(iFrom<0){
			break;
		}
		sTextLine.MakeUpper();
		CString sTextToReplace=Format("&#%s;",sTextLine);
		memset(&szChar,0,sizeof(szChar));
		DWORD dwHexChar=atolx(sTextToReplace);
		((WORD*)(&szChar))[0]=LOWORD(dwHexChar);
		((WORD*)(&szChar))[1]=0;
		sTextLine=W2A(szChar);
		int iReplacedItemsCount=sBodyText.Replace(sTextToReplace,sTextLine);
		if(iReplacedItemsCount>0){
			iFrom+=strlen(sTextLine)-strlen(sTextToReplace);
			bAnyTagsStripped=TRUE;
		}else{
			iFrom+=strlen(sTextToReplace);
		}
	}
	sBodyText.Replace("&nbsp;"," ");
	sBodyText.Replace("&nbsp"," ");
	sBodyText.Replace("&lt;","<");
	sBodyText.Replace("&lt","<");
	sBodyText.Replace("&gt;",">");
	sBodyText.Replace("&gt",">");
	sBodyText.Replace("&lt;","<");
	sBodyText.Replace("&quot;","\"");
	sBodyText.Replace("&quot","\"");
	sBodyText.Replace("&amp;","&");
	sBodyText.Replace("&amp","&");
	sBodyText.Replace("&mdash;","-");
	sBodyText.Replace("&mdash","-");
	
	sBodyText.Replace("&copy;","(C)");
	sBodyText.Replace("&copy","(C)");
	sBodyText.TrimLeft();
	sBodyText.TrimRight();
	return bAnyTagsStripped;
}

BOOL BlockTag(const char* szTag)
{_XLOG_
	if(stricmp(szTag,"title")==0 || stricmp(szTag,"script")==0 || stricmp(szTag,"object")==0 || stricmp(szTag,"embed")==0 || stricmp(szTag,"style")==0){_XLOG_
		return TRUE;
	}
	return FALSE;
}

CString TrimChars(const char* szIn, const char* szCharsR, const char* szCharsL)
{_XLOG_
	CString s=szIn;
	if(szCharsR){_XLOG_
		s.TrimRight(szCharsR);
	}
	if(szCharsL){_XLOG_
		s.TrimLeft(szCharsL);
	}
	return s;
}

BOOL StripTags(CString& sBodyText)
{_XLOG_
	BOOL bAnyTagsStripped=FALSE;
	int iFrom=0;
	// Общий мусор...
	sBodyText.Replace("\r","");
	sBodyText.Replace("\t"," ");
	sBodyText.Replace("\n"," ");
	while(sBodyText.Replace("  "," ")){};
	// теги...
	sBodyText.Replace("<![CDATA[","");// CDATA
	sBodyText.Replace("]]>","");// CDATA
	sBodyText.Replace("/>",">");// XML-like теги
	while(sBodyText.Replace(" >",">")){};
	CDataXMLSaver::StripInstringPart("<!--","-->",sBodyText);
	while(iFrom>=0 && iFrom<sBodyText.GetLength()){
		CString sTextLine=CDataXMLSaver::GetInstringPart("<",">",sBodyText,iFrom,XMLNoConversion);
		if(iFrom<0){
			break;
		}
		CString sTextToReplace=Format("<%s>",sTextLine);
		CString sFirstTag=sTextLine.SpanExcluding(" ");
		sFirstTag.MakeLower();
		if(BlockTag(sFirstTag)){
			CString sClosingTag=Format("/%s>",sFirstTag);
			int iClosePos=sBodyText.Find(sClosingTag,iFrom);
			if(iClosePos!=-1){
				sBodyText=sBodyText.Left(iFrom-sTextLine.GetLength()-2)+sBodyText.Mid(iClosePos+sClosingTag.GetLength());
				sTextToReplace="";
				iFrom=0;
			}
		}
		if(sTextToReplace!=""){
			sTextLine="";// Убиваем таг!
			if(sFirstTag=="img" || sFirstTag=="a"){
				CString sImgSrc=sTextToReplace;
				sImgSrc.Replace("'","\"");
				CString sImgSrcR=sImgSrc;
				sImgSrc.MakeLower();
				if(sImgSrc.Find("width")==-1 || sImgSrc.Find("height")==-1){
					sImgSrcR.Replace("HREF","href");
					sImgSrcR.Replace("SRC","src");
					if(sImgSrcR.Find("src=")!=-1){
						CString sURL=CDataXMLSaver::GetInstringPart("src=\"","\"",sImgSrcR);
						if(sURL.GetLength()>7 && sURL.Left(7)=="http://"){
							if(sBodyText.Find(TrimChars(sURL,"/\\ '\"","/\\ '\""),iFrom)==-1){
								sTextLine=" "+EscapeStringSP(sURL)+" ";
							}
						}
					}
					if(sImgSrcR.Find("href=")!=-1){
						CString sURL=CDataXMLSaver::GetInstringPart("href=\"","\"",sImgSrcR);
						if(sURL.GetLength()>7 && sURL.Left(7)=="http://"){
							if(sBodyText.Find(TrimChars(sURL,"/\\ '\"","/\\ '\""),iFrom)==-1){
								sTextLine=" "+EscapeStringSP(sURL)+" ";
							}
						}
					}
				}
			}else if(sFirstTag=="br" || sFirstTag=="p" || sFirstTag=="div"){
				sTextLine="\n";// Перевод строки
			}else if(sFirstTag=="hr"){
				sTextLine="\n---------------\n";
			}
			int iReplacedItemsCount=sBodyText.Replace(sTextToReplace,sTextLine);
			if(iReplacedItemsCount>0){
				iFrom+=strlen(sTextLine)-strlen(sTextToReplace);
				bAnyTagsStripped=TRUE;
			}else{
				iFrom+=strlen(sTextToReplace);
			}
		}
	}
	while(sBodyText.Replace("  "," ")){};
	while(sBodyText.Replace(" \n","\n")){};
	while(sBodyText.Replace("\n\n","\n")){};
	return bAnyTagsStripped;
}

DWORD atolx(const char* szStr)
{_XLOG_
	if(szStr==0 || szStr[0]==0){_XLOG_
		return 0;
	}
	BOOL bX16=0;
	char const* szPos=szStr;
	if(strlen(szStr)>2 && szStr[0]=='0' && szStr[1]=='x'){_XLOG_
		bX16=1;
		szPos=szStr+2;
	}else if(strlen(szStr)>2 && szStr[0]=='&' && szStr[1]=='#'){_XLOG_
		if(szStr[2]=='x' || szStr[2]=='X'){_XLOG_
			bX16=1;
			szPos=szStr+3;
		}else{_XLOG_
			szPos=szStr+2;
		}
	}else if(strlen(szStr)>1 && szStr[0]=='#'){_XLOG_
		bX16=1;
		szPos=szStr+1;
	}
	if(bX16){_XLOG_
		DWORD dwRes=0;
		while(*szPos){
			DWORD dwC=0;
			char c=szPos[0];
			if(c>='0' && c<='9'){
				dwC=c-'0';
			}else if(c>='a' && c<='f'){
				dwC=10+c-'a';
			}else if(c>='A' && c<='F'){
				dwC=10+c-'A';
			}else{
				break;
			}
			dwRes*=16;
			dwRes+=dwC;
			szPos++;
		}
		return dwRes;
	}
	return atol(szPos);
}

CString getURLHost(CString sTagValue, BOOL bToLast)
{_XLOG_
	int iPrPos=bToLast?sTagValue.ReverseFind('/'):sTagValue.Find("://");
	if(iPrPos!=-1){_XLOG_
		if(bToLast){_XLOG_
			sTagValue=sTagValue.Left(iPrPos);
		}else{_XLOG_
			int iPrPos2=sTagValue.Find("/",iPrPos+3);
			if(iPrPos2!=-1){_XLOG_
				sTagValue=sTagValue.Left(iPrPos2);
			}
		}
	}
	return sTagValue;
}

BOOL IsBatFile2(const char* szCommand)
{_XLOG_
	CString sFile=szCommand;
	sFile.MakeLower();
	if(sFile.Find(".bat")!=-1){_XLOG_
		return TRUE;
	}
	if(sFile.Find(".cmd")!=-1){_XLOG_
		return TRUE;
	}
	return FALSE;
}


BOOL IsExecutableFile(const char* szCommand)
{_XLOG_
	CString sFile=szCommand;
	sFile.MakeLower();
	if(sFile.Find(".exe")!=-1){_XLOG_
		return TRUE;
	}
	if(sFile.Find(".com")!=-1){_XLOG_
		return TRUE;
	}
	return IsBatFile2(szCommand);
}

void AddToTextCab(CString& sCabContent,CString sFileToAdd,CString sBase)
{
	CString sFile,sFileContent;
	int iCabLen=sCabContent.GetLength();
	if(iCabLen==0){
		// Формируем запирающую строку
		sCabContent="{HEADER-END}\n\n";
	}
	int iSize=GetFileSize(sFileToAdd);
	int iB64=0;
	if(iSize){
		void* pBuffer=new BYTE[iSize+1];
		memset(pBuffer,0,iSize+1);
		ReadFileToBuffer(sFileToAdd,pBuffer,iSize);
		if(memchr(pBuffer,0,iSize)!=0){
			// Base64
			sFileContent=PackToString((const char*)pBuffer,iSize);
			iB64=1;
		}else{
			sFileContent=(const char*)pBuffer;
		}
		delete[] pBuffer;
	}
	int iMetka=0;
	CString sMetka=Format("WK#FCON_%i",iCabLen);
	while((sFileContent.Find(sMetka)!=-1 || sCabContent.Find(sMetka)!=-1) && iMetka<100){
		sMetka=Format("WK#FCON_%i_%i_%iZ",iCabLen,iMetka++,GetTickCount());
	}
	sFileToAdd.Replace(sBase,"");
	sCabContent=Format("{RECORD::[FILE::%s::][METKA:%s][B64:%i][FILELEN:%i]::}\n",sFileToAdd,sMetka,iB64,iSize)
		+sCabContent
		+Format("<%s-BEGIN>%s<%s-END>\n",sMetka,sFileContent,sMetka);
	return;
}

void CDataXMLSaver::GetInstringPartArray(CStringArray& res, const char* szStart, const char* szEnd, const char* szString, DWORD dwOptions, const char* szStringOriginal)
{
	int iFrom=0;

	while(iFrom!=-1){
		CString sLine=GetInstringPart(szStart, szEnd, szString, iFrom, dwOptions, szStringOriginal);
		if(sLine!=""){
			res.Add(sLine);
		}
	}
	return;
}

BOOL ExtractFromTextCab(CString& sCabContent,CString sBase)
{
	int iHeader=sCabContent.Find("{HEADER-END}");
	if(iHeader==-1){
		return 0;
	}
	int iRes=0;
	CString sHeader=sCabContent.Left(iHeader);
	CStringArray arr;
	CDataXMLSaver::GetInstringPartArray(arr,"{RECORD::","::}",sHeader);
	for(int i=0;i<arr.GetSize();i++){
		CString sRecord=arr[i];
		CString sFileName=CDataXMLSaver::GetInstringPart("[FILE::","::]",sRecord);
		CString sMetka=CDataXMLSaver::GetInstringPart("[METKA:","]",sRecord);
		int iB64=atol(CDataXMLSaver::GetInstringPart("[B64:","]",sRecord));
		int iFileLen=atol(CDataXMLSaver::GetInstringPart("[FILELEN:","]",sRecord));
		int iFrom=iHeader;
		CString sFileContent=CDataXMLSaver::GetInstringPart(Format("<%s-BEGIN>",sMetka),Format("<%s-END>",sMetka),sCabContent,iFrom);
		if(iB64){
			char* pBuffer=new char[iFileLen];
			memset(pBuffer,0,iFileLen);
			CBase64XXX dc(0);
			dc.Decode(sFileContent,iFileLen,pBuffer);
			SaveBuffer(sBase+sFileName,pBuffer,iFileLen);
			iRes++;
		}else{
			if(sFileContent!=""){
				SaveFile(sBase+sFileName,sFileContent);
				iRes++;
			}
		}
	}
	return iRes;
}