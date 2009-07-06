#include "stdafx.h"
#include <stdio.h>
#include "crt_common.h"

WINOLEAPI  CoInitializeEx(IN LPVOID pvReserved, IN DWORD dwCoInit);
typedef BOOL (STDAPICALLTYPE * _PathFindOnPath)(LPSTR pszPath, LPCSTR * ppszOtherDirs);
typedef BOOL (STDAPICALLTYPE * _PathCompactPathEx)(LPSTR pszOut, LPCSTR pszSrc, UINT cchMax, DWORD dwFlags);
typedef BOOL (STDAPICALLTYPE * _PathRelativePathTo)(LPTSTR pszPath,LPCTSTR pszFrom,DWORD dwAttrFrom,LPCTSTR pszTo,DWORD dwAttrTo);
class CShlwapi
{
public:
	HINSTANCE hDll;
	_PathFindOnPath fpFind;
	_PathCompactPathEx fpCompact;
	_PathRelativePathTo fpMakeRelPath;
	CShlwapi()
	{
		fpFind=0;
		fpCompact=0;
		hDll=LoadLibrary("shlwapi.dll");
		fpFind=0;
		fpCompact=0;
		fpMakeRelPath=0;
		if(hDll){
			fpFind=(_PathFindOnPath)GetProcAddress(hDll,"PathFindOnPathA");
			fpCompact=(_PathCompactPathEx)GetProcAddress(hDll,"PathCompactPathExA");
			fpMakeRelPath=(_PathRelativePathTo)GetProcAddress(hDll,"PathRelativePathToA");
		}
	}
	~CShlwapi()
	{
		if(hDll){
			FreeLibrary(hDll);
		}
	}
};

CShlwapi& _Shlwapi()
{
	static CShlwapi obj;
	return obj;
}

DWORD GetFileSize(const char* szFileName)
{
	DWORD dwFileSize=0;
	FILE* f=fopen(szFileName,"rb");
	if (f==NULL){
		return 0;
	}
	fseek(f,0,SEEK_END);
	dwFileSize = ftell(f);
	fclose(f);
	return dwFileSize;
}

#ifdef CRT_WITHCSTR
CCOMString GetAppDir()
{
	static char appDir[MAX_PATH]="";
	if(appDir[0]!=0){
		return appDir;
	}
	char szCurrentExeFile[MAX_PATH]="";
	GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
	char szDrive[MAX_PATH],szDir[MAX_PATH];
	_splitpath(szCurrentExeFile, szDrive, szDir, NULL, NULL);
	strcpy(appDir,szDrive);
	strcat(appDir,szDir);
	return appDir;
}

BOOL ReadFile(const char* szFileName, CCOMString& sFileContent)
{
	if(szFileName==NULL || strlen(szFileName)==0){
		return FALSE;
	}
	DWORD dwSize=GetFileSize(szFileName);
	if(dwSize==0){
		return FALSE;
	}
	FILE* f=fopen(szFileName,"rb");
	if(f==NULL){
		return FALSE;
	}
	char* szData = new char[dwSize+1];
	memset(szData,0,dwSize+1);
	DWORD dwReadedBytes=fread(szData,1,dwSize,f);
	sFileContent=szData;
	delete[] szData;
	fclose(f);
	// Проверяем и выходим
	if(dwReadedBytes!=dwSize){
		return FALSE;
	}
	return TRUE;
}

#include <shlobj.h>
typedef BOOL (WINAPI *_SHGetSpecialFolderLocation)(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl);
BOOL MyGetSpecialFLoc(HWND hwndOwner, int nFolder, LPITEMIDLIST * ppidl)
{
	BOOL bRes=FALSE;
	/*HINSTANCE hShell32=LoadLibrary("Shell32.dll");
	if(hShell32){
		_SHGetSpecialFolderLocation fp=(_SHGetSpecialFolderLocation)GetProcAddress(hShell32,"SHGetSpecialFolderLocationA");
		if(!fp){
			fp=(_SHGetSpecialFolderLocation)GetProcAddress(hShell32,"SHGetSpecialFolderLocation");
		}
		if(fp){
			LPITEMIDLIST pIdl = NULL;
			bRes=(*fp)(hwndOwner,nFolder,ppidl);
			FreeLibrary(hShell32);
		}
	}*/
	bRes=SHGetSpecialFolderLocation(hwndOwner,nFolder,ppidl);
	return bRes;
}

typedef HRESULT (WINAPI* _SHGetFolderPath)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
BOOL GetSpecialFolderPath(char* szOutPath, int nFolder)
{
	strcpy(szOutPath,"c:\\temp\\");
	/*
	HINSTANCE hShell32=LoadLibrary("Shell32.dll");
	if(hShell32){
		_SHGetFolderPath fp=(_SHGetFolderPath)GetProcAddress(hShell32,"SHGetFolderPathA");
		if(!fp){
			FreeLibrary(hShell32);
			hShell32=LoadLibrary("Shfolder.dll");
			if(hShell32){
				fp=(_SHGetFolderPath)GetProcAddress(hShell32,"SHGetFolderPathA");
			}
		}
		if(fp){
			char szPath[MAX_PATH]="";
			(*fp)(0,nFolder,0,0,szPath);
			strcpy(szOutPath,szPath);
		}
		FreeLibrary(hShell32);
	}
	*/
	char szPath[MAX_PATH]="";
	SHGetSpecialFolderPath(0,szPath,nFolder,0);
	strcpy(szOutPath,szPath);
	return TRUE;
}

CCOMString getDesktopPath()
{
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_DESKTOPDIRECTORY);
	return szFolderBuffer;
}

CCOMString TrimMessage(const char* szFile,int iSize)
{
	char szPath[MAX_PATH]="";
	if(_Shlwapi().fpCompact){
		_Shlwapi().fpCompact(szPath,szFile,iSize,0);
	}else{
		strcpy(szPath,szFile);
		szPath[iSize]=0;
	}
	return szPath;
}

BOOL SetRegStr(HKEY& hOpenedKey, const char* szKeyName, CCOMString sValue)
{
    return RegSetValueEx(hOpenedKey,szKeyName,0,REG_SZ,(BYTE*)((const char*)sValue),sValue.GetLength()+1)==ERROR_SUCCESS;
}

CCOMString GetRegStr(HKEY& hOpenedKey, const char* szKeyName)
{
	char szValue[3*MAX_PATH]={0};
	DWORD lSize=sizeof(szValue);
	DWORD dwType=REG_SZ;
	if(RegQueryValueEx(hOpenedKey,szKeyName,0,&dwType,(LPBYTE)szValue,&lSize)!=ERROR_SUCCESS){
		dwType=REG_EXPAND_SZ;
		szValue[0]=0;
		RegQueryValueEx(hOpenedKey,szKeyName,0,&dwType,(LPBYTE)szValue,&lSize);
	}
	CCOMString sOut=szValue;
	return sOut;
}
#endif

BOOL SaveFile(const char* sStartDir, const char* sFileContent)
{
	FILE* m_pFile=fopen(sStartDir,"w+b");
	if(!m_pFile){
		return FALSE;
	}
	DWORD nRead=fwrite(sFileContent,sizeof(char),strlen(sFileContent),m_pFile);
	fclose(m_pFile);
	m_pFile=NULL;
	return (nRead==strlen(sFileContent));
}

BOOL isFileExist(const char* szFile)
{
	if(szFile==NULL || szFile[0]==0){
		return FALSE;
	}
#ifdef CRT_WITHCSTR
	if(szFile[strlen(szFile)-1]=='\\'){
		CCOMString sFile2=szFile;
		sFile2=sFile2.Left(strlen(szFile)-1);
		return isFileExist(sFile2);
	}
#endif
	if(int(GetFileAttributes(szFile))==-1){
		return FALSE;
	}
	return TRUE;
}

BOOL isFolder(const char* szFile)
{
	if(szFile==NULL || szFile[0]==0 || !isFileExist(szFile)){
		return FALSE;
	}
	if((GetFileAttributes(szFile) & FILE_ATTRIBUTE_DIRECTORY)!=0){
		return TRUE;
	}
	return FALSE;
}

BOOL isDLL(const char* szFile)
{
	int iLen=strlen(szFile);
	if(iLen<4){
		return FALSE;
	}
	if(stricmp(szFile+iLen-4,".dll")==0 || stricmp(szFile+iLen-4,".wkp")==0 || stricmp(szFile+iLen-4,".ocx")==0){
		return TRUE;
	}
	return FALSE;
}

BOOL isOnRemovable(const char* szFile)
{
	if(!szFile){
		return FALSE;
	}
	if(memicmp(szFile,"a:\\",3)==0 || memicmp(szFile,"b:\\",3)==0){
		return TRUE;
	}
	return FALSE;
}

BOOL isLNK(const char* szFile)
{
	int iLen=strlen(szFile);
	if(iLen<4){
		return FALSE;
	}
	if(stricmp(szFile+iLen-4,".lnk")==0 || stricmp(szFile+iLen-4,".pif")==0){
		return TRUE;
	}
	return FALSE;
}

#ifdef CRT_WITHSHELL
HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc, LPCSTR lpszAppParams)
{
    HRESULT hres; 
    IShellLink* psl; 
	
	// Get a pointer to the IShellLink interface. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl);
	if (SUCCEEDED(hres)) { 
		IPersistFile* ppf; 
		
        // Set the path to the shortcut target and add the 
        // description. 
        psl->SetPath(lpszPathObj); 
        psl->SetDescription(lpszDesc); 
		if(lpszAppParams && strlen(lpszAppParams)>0){
			psl->SetArguments(lpszAppParams);
		}
		// Query IShellLink for the IPersistFile interface for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
		
		if (SUCCEEDED(hres)) {
            WCHAR wsz[MAX_PATH]; 
			
			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, 
				wsz, MAX_PATH); 
			
			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(wsz, TRUE); 
			ppf->Release(); 
		} 
		psl->Release(); 
	}
	return hres; 
}

HRESULT UnpackLink(LPCSTR lpszLinkFile, LPSTR lpszPath) 
{ 
    HRESULT hres=0; 
    IShellLink* psl=0; 
    WIN32_FIND_DATA wfd;
    char szGotPath[MAX_PATH]={0}; 
     // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
 
        // Get a pointer to the IPersistFile interface. 
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
		
        if (SUCCEEDED(hres)) 
        { 
            WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH); 

            // Load the shortcut. 
            hres = ppf->Load(wsz, STGM_READ); 
            
            if (SUCCEEDED(hres)) 
            { 
                // Resolve the link. 
                //hres = psl->Resolve(hwnd, 0); 

                //if (SUCCEEDED(hres)) 
                { 
                    // Get the path to the link target. 
                    hres = psl->GetPath(szGotPath, 
                                        MAX_PATH, 
                                        (WIN32_FIND_DATA*)&wfd, 
                                        SLGP_SHORTPATH); 

                    if (SUCCEEDED(hres)) 
                    { 
						if(GetLongPathName(szGotPath,lpszPath,MAX_PATH)==0 || lpszPath[0]==0){
							strcpy(lpszPath, szGotPath);
						}
						//_FL2("Conver '%s'>'%s'",szGotPath,lpszPath);
                    }
                } 
            } 

            // Release the pointer to the IPersistFile interface. 
            ppf->Release(); 
        } 

        // Release the pointer to the IShellLink interface. 
        psl->Release(); 
    } 
    return hres; 
}

int iCntUnlinki=0;
class UnL
{
public:
	UnL()
	{
		iCnt=iCntUnlinki;
		iCntUnlinki++;
		lpszLinkFile="";
		lpszPath=NULL;
		lStop=0;
	};
	LPCSTR lpszLinkFile;
	LPSTR lpszPath;
	long lStop;
	long iCnt;
};

long lPos=0;
#define UNLSIZE	1000
UnL massiv[UNLSIZE];
DWORD WINAPI UnLinkI(LPVOID p)
{
	UnL* l=(UnL*)p;
	int iCnt=l->iCnt;
	CoInitializeEx(0,COINIT_APARTMENTTHREADED);
	char s1[MAX_PATH*20]={0};
	char s2[MAX_PATH*20]={0};
	strcpy(s1,l->lpszLinkFile);
	UnpackLink(s1,s2);
	if(l->lStop==0 && strlen(s2)<MAX_PATH*2-1){
		strcpy(l->lpszPath,s2);
	}
#ifndef UNLSIZE
	free(l);
#endif
	::CoUninitialize();
	return 0;
}

HRESULT UnpackLinkTM(LPCSTR lpszLinkFile, LPSTR lpszPath, DWORD dwTimeOut) 
{
	UnL* l=&massiv[lPos++];
	if(lPos>=UNLSIZE){
		lPos=0;
	}
	l->lpszLinkFile=lpszLinkFile;
	l->lpszPath=lpszPath;
	l->lStop=0;
	l->iCnt=iCntUnlinki++;
	DWORD dw=0;
	int iCnt=l->iCnt;
 	HANDLE h=CreateThread(0,0,UnLinkI,LPVOID(l),0,&dw);
	DWORD dwWaitRes=WaitForSingleObject(h,dwTimeOut);
	CloseHandle(h);
	if(dwWaitRes!=WAIT_OBJECT_0){
		l->lStop=1;
		return S_FALSE;
	}
	return S_OK;
}
#endif

bool PatternMatch(const char* s, const char* mask)
{
	const   char* cp=0;
	const   char* mp=0;
	for (; *s&&*mask!='*'; mask++,s++){
		if (*mask!=*s && *mask!='?'){
			return 0;
		}
	}
	for (;;) {
		if (!*s){
			while (*mask=='*'){
				mask++;
			}
			return !*mask;
		}
		if (*mask=='*'){ 
			if (!*++mask){
				return 1;
			}
			mp=mask;
			cp=s+1;
			continue;
		}
		if (*mask==*s||*mask=='?'){
			mask++;
			s++;
			continue;
		}
		mask=mp;
		s=cp++;
	}
	return true;
}

void SkipBlanks(const char*& szPos)
{
	while(*szPos==' ' || *szPos=='\t' || *szPos=='\r' || *szPos=='\n' || *szPos==';'){
		szPos++;
	}
}

bool PatternMatchList(const char* s, const char* mask)
{
	if(mask==NULL || *mask==NULL){
		return false;
	}
	if(s==NULL || *s==NULL){
		return false;
	}
	const char* szPos=mask;
	SkipBlanks(szPos);
	if(szPos==NULL || *szPos==NULL){
		return false;
	}
	while(szPos!=NULL && *szPos!=0){
		SkipBlanks(szPos);
		const char* szPos2=strpbrk(szPos,";");
		if(szPos2==NULL){
			szPos2=szPos+(strlen(szPos));
		}
		if(szPos2<=szPos){
			break;
		}
		char szMask[256];
		memset(szMask,0,sizeof(szMask));
		memcpy(szMask,szPos,size_t(szPos2-szPos));
		if(szMask[0]!=0){
			const char* szPattern=szMask;
			SkipBlanks(szPattern);
			if(PatternMatch(s,szPattern)){
				return true;
			}
		}
		szPos=szPos2;
		if(*szPos==';'){
			szPos++;
		}
	}
	return false;
}

void ReplaceChar(char* szSrc, char cFrom, char cTo)
{
	int iPos=0;
	while(szSrc[iPos]!=0){
		if(szSrc[iPos]==cFrom){
			szSrc[iPos]=cTo;
		}
		iPos++;
	}
}

void ReplaceChar(char* szSrc, char cFrom, const char* szTo)
{
	int iPos=0;
	long iAmount=0;
	if(szTo){
		iAmount=strlen(szTo);
	}
	while(szSrc[iPos]!=0){
		if(szSrc[iPos]==cFrom){
			// Overlap-aware fuction
			if(iAmount>0){
				long lLen=strlen(szSrc+iPos);
				memmove(szSrc+iPos+iAmount,szSrc+iPos+1,lLen);
				memcpy(szSrc+iPos,szTo,iAmount);
				iPos+=iAmount;
			}else{
				// Overlap-aware fuction
				long lLen=strlen(szSrc+(iPos+1));
				memmove(szSrc+iPos,szSrc+(iPos+1),lLen);
				szSrc[iPos+lLen]=0;
			}
		}
		iPos++;
	}
}

void LowerString(char* s)
{
	if(!s){
		return;
	}
	while(s[0]){
		if(s[0]>='A' && s[0]<='Z'){
			s[0]+='a'-'A';
		}
		s++;
	}
}

int SCmp(char* s1, char* s2, char* szSkip)
{
	int iRes=0;
	while(s1[0] && s2[0]){
		if(szSkip){
			char* sTmp=szSkip;
			while(*sTmp){
				if(s1[0]==sTmp[0]){
					s1++;
				}
				if(s2[0]==sTmp[0]){
					s2++;
				}
				sTmp++;
			}
		}
		if(s1[0]==s2[0]){
			iRes++;
		}else{
			break;
		}
		s1++;
		s2++;
	}
	return iRes;
}

BOOL isCharIn(char c, char* sz)
{
	if(c==0){
		return FALSE;
	}
	while(sz[0]){
		if(sz[0]==c){
			return TRUE;
		}
		sz++;
	}
	return FALSE;
}

int GetSLen(char* s1, char* szSkip)
{
	int iLen1=0,iLenTmp=0;
	while(s1[iLenTmp]!=0){
		if(!szSkip || !isCharIn(s1[iLenTmp],szSkip)){
			iLen1++;
		}
		iLenTmp++;
	}
	return iLen1;
}

void DropSkips(char* s1, char* szSkip)
{
	while(s1[0]){
		if(isCharIn(s1[0],szSkip)){
			char* s2=s1;
			do{
				s2[0]=s2[1];
				s2++;
			}while(s2[0]);
		}else{
			s1++;
		}
	}
}

int SCmpInList(char* s1, char* s2, char* szSkip, char* szDelims)
{
	DropSkips(s1,szSkip);
	DropSkips(s2,szSkip);
	int iLen1=GetSLen(s1);
	if(iLen1==0){
		return FALSE;
	}
	while(s2[0]){
		int iTest=SCmp(s2,s1);
		if(iTest==iLen1){
			return TRUE;
		}
		while(s2[0] && !isCharIn(s2[0],szDelims)){
			s2++;
		}
		if(s2[0]==0){
			return FALSE;
		}
		s2++;
	}
	return FALSE;
}

void SCpy(char* s1, char* s2)
{
	while(s2[0]){
		s1[1]=0;
		s1[0]=s2[0];
		s1++;
		s2++;
	}
	return;
}

BOOL GlobalRemoveAddAtom(BOOL bAdd,const char* szName)
{
	if(bAdd){
		GlobalAddAtom(szName);
	}else{
		while(GlobalFindAtom(szName)){
			GlobalDeleteAtom(GlobalFindAtom(szName));
		}
	}
	return 1;
}

