// DataXMLSaver.h: interface for the CDataXMLSaver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAXMLSAVER_H__969B3E6A_56E6_471A_83C3_153683EDB494__INCLUDED_)
#define AFX_DATAXMLSAVER_H__969B3E6A_56E6_471A_83C3_153683EDB494__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <comdef.h>
#ifndef _NO_XMLHOTKEYSUPPORT
#include "hotkeys.h"
#endif

enum{
	XMLNoConversion	=0x0001,
	XMLRNConversion	=0x0002,
	XMLTrimSpaces	=0x0004,
	XMLAsAttribute	=0x0008,
	XMLWithStartCR	=0x0010,
	XMLWithEndCR	=0x0020,
	XMLNoLTGTConv	=0x0040,
	XMLJScriptConv	=0x0080,
	QT_FORWARD2		=0x0100,
};

void SubstEnvVariables(CString& sText);
CString GetPathFolder(const char* szFile);
BOOL isFileExist(const char* szFile);
BOOL isFileIExist(const char* szFile);
BOOL isFolder(const char* szFile);
BOOL PreparePathForWrite(const char* szPath);
DWORD GetFileSize(const char* szFileName);
BOOL ReadFile(const char* sStartDir, CString& sFileContent, BOOL bAloowWPTextExtensions=0);
BOOL ReadFile(const char* szFileName, wchar_t*& wszFileContent);// Вызывающая сторона должна очистить строку!!!
BOOL SaveFile(const char* sStartDir, const char* sFileContent);
BOOL AppendFile(const char* sStartDir, const char* sFileContent);
BOOL ReadFileCashed(const char* szFileName, CString& sFileContent);
BOOL ClearReadFileCash();
BOOL IsFileUnicode(const char* szFileName);
BOOL ReadFileToBuffer(const char* sStartDir, LPVOID hGlobalBuffer, DWORD dwFileSize);
CString PackToString(const char* szData, DWORD dwDataSize);
BOOL PackFromString(const char* szData, DWORD dwDataSize, HGLOBAL& hGlobal, long lAllocParams=-1);
CString UnescapeString(const char* szString);
CString EscapeString(const char* szString,BOOL bUseExUnicod=1, CString sDelims="\\/:?&%=", int* iExtCounts2=0, int* iExtCounts4=0);
CString EscapeString(const WCHAR* szString);
CString EscapeString2(const char* szString,CString sAddChars="\\/:?&%='\"");
CString EscapeStringSP(const char* szString);
CString EscapeSlashes(const char* szString);
CString EscapeStringUTF8(const WCHAR* szString);
CString EscapeString3(const char* szString);
char HexCode(const char* szRawValue);
int ReverseFind(CString& sSource, const char* szWhat,int iFrom=0);
int CountChars(CString& sSource, const char* szWhat,int iFrom,int iTo);
void ArithReplace(CString& sWho, const char* szWhat, const char* szWith);
class CDataXMLSaver  
{
	BOOL bFileLoaded;
	BOOL m_bRead;
	BOOL m_bDataSaved;
	BOOL m_bAppendMode;
	CString m_sRoot;
	CString m_sSource;
	FILE* m_pFile;
	CString m_sFileName;
public:
	void Init();
	CDataXMLSaver(const char* pString="");
	CDataXMLSaver(const char* szFile, const char* szRoot, BOOL bRead, BOOL bAppendMode=FALSE);
	virtual ~CDataXMLSaver();
	BOOL SetAsSaved(BOOL bRes);
	CString GetSourceFile(){return m_sFileName;};
	CString GetSource();
	void SetSource(const char* szSource="");
	CString GetResult();
	BOOL CryptBody(CString sTag="crypted");
	BOOL UncryptBody(CString sTag="crypted");
	void SetRoot(const char* szRootName);
	BOOL SaveDataIntoSource(const char* szFile=NULL);
	BOOL LoadDataToSource(const char* szFile);
	BOOL IsFileLoaded(){return bFileLoaded;};
	CString getValue(const char* szKey);
	BOOL getValue(const char* szKey,int& iParam, int iDefault=0);
	BOOL getValue(const char* szKey,DWORD& iParam, DWORD iDefault=0);
	BOOL getValue(const char* szKey,long& lParam, long lDefault=0);
	BOOL getValue(const char* szKey,double& dParam, double dDefault=0);
	BOOL getValue(const char* szKey,COleDateTime& dt, COleDateTime dtDefault=COleDateTime::GetCurrentTime());
	BOOL getValue(const char* szKey,CString& sParam, const char* sDefault="", DWORD dwOptions=0);
	BOOL putValue(const char* szKey,int iParam);
	BOOL putValue(const char* szKey,DWORD iParam);
	BOOL putValue(const char* szKey,long lParam);
	BOOL putValue(const char* szKey,double dParam);
	BOOL putValue(const char* szKey,const char* sParam, DWORD dwOptions=0);
	BOOL putValue(const char* szKey,COleDateTime dt);
	static CString OleDate2Str(COleDateTime dt);
	static COleDateTime Str2OleDate(CString dt);
	static BOOL StripInstringPart(const char* szStart, const char* szEnd, CString& szString, int* iFrom=0, BOOL bIgnoreCase=0, int iStartOffset=0);
	static BOOL ReplaceInstringPart(const char* szStart, const char* szEnd, const char* szWith, CString& szString, int* iFrom=0);
	static CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString, int& iFrom, DWORD dwOptions=0, const char* szStringOriginal=0);
	static void GetInstringPartArray(CStringArray& arr, const char* szStart, const char* szEnd, const char* szString, DWORD dwOptions=0, const char* szStringOriginal=0);
	static CString GetInstringPart(const char* szStart, const char* szEnd, const char* szString, DWORD dwOptions=0, int* iFrom=NULL);
	static CString GetInstringPart(const char* szStart, const char* szString, DWORD dwOptions=0, int* iFrom=NULL);
	static CString GetInstringPartGreedly(const char* szStart, const char* szEnd, const char* szString, DWORD dwOptions=0, int* iFrom=NULL);
	static CString GetInstringPartGreedly(const char* szStart, const char* szEnd, const char* szString, int& iFrom, DWORD dwOptions=0);
	static void Str2Xml(CString& sRes, DWORD dwFlags=0);
	static void Xml2Str(CString& sRes, DWORD dwFlags=0);
#ifdef IPV6_HOTKEYS
	#define _INCLUDE_IPV6_HOTKEYS_IN_XMLSAVER
	BOOL putValue(const char* szKey,CIHotkey lParam);
	BOOL getValue(const char* szKey,CIHotkey& lParam, CIHotkey lDefault=0);
#else
#pragma message("--- H: XML Data saver: hotkey support disabled ---")
#endif
};
CString GetStringHash(const char* szName);
class CCritSectionLock:public CSingleLock
{
	BOOL bLocked;
	CSyncObject* m_pObject;
public:
	BOOL  Init(CSyncObject* pObject, BOOL bInitialLock = FALSE);
	CCritSectionLock(CSyncObject* pObject, BOOL bInitialLock = FALSE);
	CCritSectionLock(CSyncObject& pObject, BOOL bInitialLock = FALSE);
	~CCritSectionLock();
};

class CBase64XXX
{
public:
    CBase64XXX(BOOL bUseWierd);
    virtual ~CBase64XXX();
	
	// Override the base class mandatory functions
    virtual CString Decode( LPCTSTR szDecoding, int nSize = -1, char* szOutputDirect=NULL);
    virtual CString Encode( LPCTSTR szEncoding, int nSize = -1);
	static int b64get_encode_buffer_size(int l,int q)
	{
		int ret;
		ret = l*2;//(l/3)*4;
		if (l%3!=0) ret +=4;
		if (q!=0)
		{
			ret += (ret/(q*4));
			// if (ret%(q/4)!=0) ret ++;   // Add space for trailing \n
		}
		return ret;
	}
protected:
    void write_bits( UINT nBits, int nNumBts, LPTSTR szOutput, int& lp );
    UINT read_bits( int nNumBits, int* pBitsRead, int& lp );

    int m_nInputSize;
    int m_nBitsRemaining;
    ULONG m_lBitStorage;
    LPCTSTR m_szInput;

    static int m_nMask[];
    CString m_sBase64Alphabet;
private:
};
CString TrimChars(const char* szIn, const char* szCharsR=" ", const char* szCharsL=" ");
BOOL BlockTag(const char* szTag);
BOOL DeEntitize(CString& sBodyText);
BOOL StripTags(CString& sBodyText);
DWORD atolx(const char* szStr);
CString getURLHost(CString sTagValue, BOOL bToLast=0);
int b64encode(const char *from,char *to,int length,int quads);
BOOL IsExecutableFile(const char* szCommand);
void AddToTextCab(CString& sCabContent,CString sFileToAdd,CString sBase);
BOOL ExtractFromTextCab(CString& sCabContent,CString sBase);
BOOL SaveBuffer(const char* sStartDir, void* sFileContent, DWORD dwSize);
#endif // !defined(AFX_DATAXMLSAVER_H__969B3E6A_56E6_471A_83C3_153683EDB494__INCLUDED_)
