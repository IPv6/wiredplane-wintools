// StoreCard.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <atlbase.h>
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


BOOL ReadFile(const char* szFileName, std::string& sFileContent)
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
	size_t dwReadedBytes=fread(szData,1,dwSize,f);
	sFileContent=szData;
	delete[] szData;
	fclose(f);
	// Проверяем и выходим
	if(dwReadedBytes!=dwSize){
		return FALSE;
	}
	return TRUE;
}


BOOL SaveFile(const char* sStartDir, const char* sFileContent)
{
	FILE* m_pFile=fopen(sStartDir,"w+b");
	if(!m_pFile){
		return FALSE;
	}
	size_t nRead=fwrite(sFileContent,sizeof(char),strlen(sFileContent),m_pFile);
	fclose(m_pFile);
	m_pFile=NULL;
	return (nRead==strlen(sFileContent));
}

BOOL isFileExist(const char* szFile)
{
	if(szFile==NULL || szFile[0]==0){
		return FALSE;
	}
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

std::string GetAppDir(const char* subPath)
{
	static char appDir[MAX_PATH]="";
	if(appDir[0]==0){
		char szCurrentExeFile[MAX_PATH]="";
		GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
		char szDrive[MAX_PATH],szDir[MAX_PATH];
		_splitpath(szCurrentExeFile, szDrive, szDir, NULL, NULL);
		strcpy(appDir,szDrive);
		strcat(appDir,szDir);
	}
	std::string res=appDir;
	res+=subPath;
	return res;
}

array_s GetFolderList(const char* szFolder, const char* szMask)
{
	std::string path=szFolder;
	path+=szMask;
	win32_file_iterator itBegin(path,true, FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_NORMAL), itEnd;
	array_s result(itBegin, itEnd);
	return result;
}

std::wstring _A2W(std::string in)
{
	USES_CONVERSION;
	std::wstring res=A2W(in.c_str());
	return res;
}

std::string _W2A(const WCHAR* in)
{
	USES_CONVERSION;
	std::string res=W2A(in);
	return res;
}

int strReplace(std::string& where, std::string what, std::string with)
{
	size_t iCount=0;
	size_t iPos=0;
	do{
		iPos=where.find(what);
		if(iPos==std::string::npos){
			break;
		}
		where.replace(iPos, what.size(), with);
		iCount++;
	}while(iPos!=std::string::npos);
	return iCount;
}

std::string getInstringPart(std::string text, std::string begin, std::string end)
{
	size_t iBegin=text.find(begin);
	if(iBegin!=std::string::npos){
		size_t iEnd=text.find(end);
		if(iEnd!=std::string::npos){
			size_t iFrom=iBegin+begin.size();
			std::string res=text.substr(iFrom,iEnd-iFrom);
			return res;
		}
	}
	return "";
}

void strTrim(std::string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}