#include "basics.h"

long& GetApplicationLang()
{
	static long dwInterfaceLang=0;// язык, 0- английский, 1-русский
	return dwInterfaceLang;
}

const char* GetApplicationDir()
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

const char* GetApplicationName()
{
	static char appName[MAX_PATH]="";
	if(appName[0]!=0){
		return appName;
	}
	char szCurrentExeFile[MAX_PATH]="";
	GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
	char szDrive[MAX_PATH],szDir[MAX_PATH], szExt[MAX_PATH];
	_splitpath(szCurrentExeFile, szDrive, szDir, appName, szExt);
	return appName;
}

const char* GetCurLangFile()
{
	static char appName[MAX_PATH]="";
	if(appName[0]!=0){
		return appName;
	}
	sprintf(appName,"%s%s_l%i.lng",GetApplicationDir(),GetApplicationName(),GetApplicationLang());
	return appName;
}

string getIniFileOption(const char* szFile, const char* szKey, const char* szDefault=NULL)
{
	map<string> aLines;
	getFileLines(szFile,aLines);
}

string _l(const char* szKey, const char* szDefault=NULL)
{
	return getIniFileOption(GetCurLangFile(), szKey, szDefault);
}