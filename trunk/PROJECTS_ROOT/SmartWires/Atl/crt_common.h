#if !defined(AFX_CRTCOMMONH__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_)
#define AFX_CRTCOMMONH__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_

DWORD GetFileSize(const char* szFileName);
BOOL SaveFile(const char* sStartDir, const char* sFileContent);
BOOL isFileExist(const char* szFile);
BOOL isFolder(const char* szFile);
BOOL isDLL(const char* szFile);
BOOL isLNK(const char* szFile);
#ifdef CRT_WITHSHELL
//SHGetSpecialFolderPathA
typedef BOOL (WINAPI *_SHGetSpecialFolderLocation)(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl);
BOOL MyGetSpecialFLoc(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl);
BOOL GetSpecialFolderPath(char* szOutPath, int nFolder);
HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc, LPCSTR lpszAppParams);
typedef BOOL (STDAPICALLTYPE * _PathFindOnPath)(LPSTR pszPath, LPCSTR * ppszOtherDirs);
typedef BOOL (STDAPICALLTYPE * _PathCompactPathEx)(LPSTR pszOut, LPCSTR pszSrc, UINT cchMax, DWORD dwFlags);
typedef BOOL (STDAPICALLTYPE * _PathRelativePathTo)(LPTSTR pszPath,LPCTSTR pszFrom,DWORD dwAttrFrom,LPCTSTR pszTo,DWORD dwAttrTo);
#endif

HRESULT UnpackLink(LPCSTR lpszLinkFile, LPSTR lpszPath);
HRESULT UnpackLinkTM(LPCSTR lpszLinkFile, LPSTR lpszPath, DWORD dwTimeOut);
// То, что требует CString-like процессинга
#ifdef CRT_WITHCSTR
#include "crt_cstring.h"
CCOMString GetAppDir();
CCOMString getDesktopPath();
BOOL ReadFile(const char* szFileName, CCOMString& sFileContent);
CCOMString TrimMessage(const char* szFile,int iSize);
BOOL SetRegStr(HKEY& hOpenedKey, const char* szKeyName, CCOMString sValue);
CCOMString GetRegStr(HKEY& hOpenedKey, const char* szKeyName);
#endif

class CSimpleTracker
{
public:
	long* bValue;
	CSimpleTracker(long& b){
		bValue=&b;
		InterlockedIncrement(bValue);
	};
	~CSimpleTracker(){
		InterlockedDecrement(bValue);
	};
};

bool PatternMatch(const char* s, const char* mask);
void SkipBlanks(const char*& szPos);
bool PatternMatchList(const char* s, const char* mask);
void ReplaceChar(char* szSrc, char cFrom, char cTo);
void ReplaceChar(char* szSrc, char cFrom, const char* szTo);
void LowerString(char* s);
int SCmp(char* s1, char* s2, char* szSkip=0);
BOOL isCharIn(char c, char* sz);
int GetSLen(char* s1, char* szSkip=0);
void DropSkips(char* s1, char* szSkip);
int SCmpInList(char* s1, char* s2, char* szSkip, char* szDelims);
void SCpy(char* s1, char* s2);
inline BOOL isWinXP() {
   OSVERSIONINFO vi = { sizeof(vi) };
   GetVersionEx(&vi);
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion>=5) {
      return TRUE;
   }
   return FALSE;
}

#ifdef _DEBUG
#define _FL3(x,z1,z2,z3)	{FILE* f=fopen("c:\\wk_debug.txt","a+");fprintf(f,"[%08i] ",GetTickCount());fprintf(f,x,z1,z2,z3);fprintf(f,"\n");fclose(f);};
#define _FL2(x,z1,z2)		{FILE* f=fopen("c:\\wk_debug.txt","a+");fprintf(f,"[%08i] ",GetTickCount());fprintf(f,x,z1,z2);fprintf(f,"\n");fclose(f);};
#define _FL1(x,z1)			{FILE* f=fopen("c:\\wk_debug.txt","a+");fprintf(f,"[%08i] ",GetTickCount());fprintf(f,x,z1);fprintf(f,"\n");fclose(f);};
#define _FL(x)				{FILE* f=fopen("c:\\wk_debug.txt","a+");fprintf(f,"[%08i] ",GetTickCount());fprintf(f,x);fprintf(f,"\n");fclose(f);};
#define ANDFLUSH			flushall();
#else
#define _FL3(x,z1,z2,z3)
#define _FL2(x,z1,z2)
#define _FL1(x,z1)
#define _FL(x)
#define ANDFLUSH			
#endif

BOOL GlobalRemoveAddAtom(BOOL bAdd,const char* szName);
BOOL IsExecutableFile(const char* szCommand);
#endif // !defined(AFX_CRTCOMMONH__FEA8409A_05E1_47E0_A213_EFD97DD01639__INCLUDED_)
