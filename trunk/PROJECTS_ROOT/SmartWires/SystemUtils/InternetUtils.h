// SupportClasses.h: interface for the SupportClasses class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_InternetUtils_H)
#define AFX_InternetUtils_H
//#include <wininet.h>
#ifndef DWORD_PTR
#define	DWORD_PTR DWORD
#endif
typedef LPVOID HINTERNET;
#define FLAG_ICC_FORCE_CONNECTION						0x00000001
typedef BOOL (WINAPI * _HttpQueryInfoA)(HINTERNET,DWORD,LPVOID,LPDWORD,LPDWORD);
typedef HINTERNET (WINAPI * _InternetOpenA)(LPCSTR,DWORD,LPCSTR,LPCSTR,DWORD);
typedef HINTERNET (WINAPI * _InternetOpenUrlA)(HINTERNET,LPCSTR,LPCSTR,DWORD,DWORD,DWORD);
typedef BOOL (WINAPI * _InternetCloseHandle)(HINTERNET);
typedef BOOL (WINAPI * _InternetReadFile)(HINTERNET,LPVOID,DWORD,LPDWORD);
typedef BOOL (WINAPI * _InternetCheckConnectionA)(LPCSTR,DWORD,DWORD);
typedef DWORD (WINAPI * _InternetDialA)(IN HWND hwndParent,IN LPSTR lpszConnectoid,IN DWORD dwFlags,OUT DWORD_PTR *lpdwConnection,IN DWORD dwReserved);
typedef BOOL (WINAPI * _InternetSetOptionA)(IN HINTERNET hInternet OPTIONAL,IN DWORD dwOption,IN LPVOID lpBuffer,IN DWORD dwBufferLength);


class CWebWorld 
{
public:
	static CString sProxyURL;
	static long lProxyPort;
	static CString sProxyUser;
	static CString sProxyPsw;

	BOOL bOnlyCheckConnection;
	HINSTANCE hWininet;
	CString GetErrorMessage();
    void SetErrorMessage(CString s);
    CString GetWebPage(const CString& Url);
	BOOL GetWebFile(const CString Url, CString sOutFileName, DWORD* dwStatusCode=0);
	BOOL TestWebPage(const CString& Url, DWORD dwTimeout, CString* sUrlError=0);
    CWebWorld();
    virtual ~CWebWorld();
	BOOL CheckConnection(LPCSTR lpszUrl, DWORD dwFlags, DWORD dwReserved);
	_InternetDialA InternetDial;
	_HttpQueryInfoA HttpQueryInfo;
	_InternetOpenA InternetOpen;
	_InternetOpenUrlA InternetOpenUrl;
	_InternetCloseHandle InternetCloseHandle;
	_InternetSetOptionA InternetSetOption;
	_InternetReadFile InternetReadFile;
	_InternetCheckConnectionA InternetCheckConnection;
	static void SetProxyInfo(HINTERNET hIn=0);
	static const char* GetProxyLine();
private:
    CString m_ErrorMessage;
    HINTERNET m_Session;
};
#endif // !defined(AFX_SUPPORTCLASSES_H__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_)
