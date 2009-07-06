#include "stdafx.h"
#include "WP_KeyMaster.h"
#include "parsUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void SavePairedValues(CString& sData,CStringArray& a1,CStringArray& a2)
{
	for(int i=0;i<a1.GetSize();i++){
		CString sKey;
		sKey.Format("%i",i);
		sData+="<key";
		sData+=sKey;
		sData+=">";
		sData+=a1[i];
		sData+="</key";
		sData+=sKey;
		sData+=">";
		sData+="<value";
		sData+=sKey;
		sData+=">";
		sData+=a2[i];
		sData+="</value";
		sData+=sKey;
		sData+=">";
		sData+="\n";
	}
}

void LoadPairedValues(CString sData,CStringArray& a1,CStringArray& a2)
{
	int i=0;
	while(true){
		CString sKeyTempl,sKeyTemple;
		sKeyTempl.Format("<key%i>",i);
		sKeyTemple.Format("</key%i>",i);
		CString k1=CStringSubstr::getTag(sKeyTempl,sKeyTemple,sData);
		CString sValTempl,sValTemple;
		sValTempl.Format("<value%i>",i);
		sValTemple.Format("</value%i>",i);
		CString k2=CStringSubstr::getTag(sValTempl,sValTemple,sData);
		if(k1.GetLength()==0 || k2.GetLength()==0){
			break;
		}
		a1.Add(k1);
		a2.Add(k2);
		i++;
	}
}

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

void ConvertComboDataToArray(const char* szData, CStringArray& aStrings, const CString& sChar, char cComment)
{
	CString sData=szData;
	sData.Replace(sChar,sChar+sChar);
	sData=sChar+sData+sChar;
	int iFrom=0;
	while(iFrom>=0){
		CString sLine=CStringSubstr::getTag(sChar,sChar,sData,iFrom,FALSE);
		sLine.TrimLeft();
		sLine.TrimRight();
		if(sLine!=""){
			if(cComment==0 || sLine[0]!=cComment){
				aStrings.Add(sLine);
			}
		}
	}
}

bool PatternMatchList(const char* s, const char* mask,const char* szDelimiter)
{
	CString sList=mask;
	if(sList==""){
		return true;
	}
	int iC=sList.Replace(szDelimiter/*";"*/,"\t");
	if(iC==0 && sList.Find("*")==-1){
		sList=sList+"*";
	}
	CStringArray sAppList;
	ConvertComboDataToArray(sList,sAppList,"\t",0);
	for(int i=0;i<sAppList.GetSize();i++){
		if(PatternMatch(s,sAppList[i])){
			return true;
		}
	}
	return false;
}

CString ReadFile(const char* szFile)
{
	CString sData;
	DWORD dwSize=GetFileSize(szFile);
	if(dwSize){
		FILE* f=fopen(szFile,"rb");
		if(f){
			char* szData = new char[dwSize+1];
			memset(szData,0,dwSize+1);
			DWORD dwReadedBytes=fread(szData,1,dwSize,f);
			sData=szData;
			delete[] szData;
			fclose(f);
		}
	}
	return sData;
}

BOOL IsVista()
{
	static BOOL bIsWinV=-1;
	if(bIsWinV==-1){
		bIsWinV=0;
		OSVERSIONINFOEX winfo; 
		ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
		winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
		if (GetVersionEx((OSVERSIONINFO *)&winfo)){ 
			bIsWinV=(winfo.dwMajorVersion>=6);
		} 
	}
	return bIsWinV;
}


BOOL isWinXP()
{
	//return 1;
	static long bWinXP=-1;
	if(bWinXP==-1){
		bWinXP=0;
		OSVERSIONINFOEX winfo; 
		ZeroMemory(&winfo, sizeof(OSVERSIONINFOEX)); 
		winfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
		if (GetVersionEx((OSVERSIONINFO *)&winfo)){ 
			if (winfo.dwMajorVersion >= 5) 
			{ 
				if (winfo.dwMinorVersion >= 1) 
					bWinXP = 1;
			}
		}
	}
	return bWinXP;
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

BOOL SaveFile(const char* sHidIni,const char* sData)
{
	FILE* m_pFile=fopen(sHidIni,"w+b");
	if(m_pFile){
		DWORD nRead=fwrite(sData,sizeof(char),strlen(sData),m_pFile);
		fclose(m_pFile);
		return TRUE;
	}
	return FALSE;
}


DWORD atolx(const char* szStr)
{
	if(szStr==0 || szStr[0]==0){
		return 0;
	}
	BOOL bX16=0;
	char const* szPos=szStr;
	if(strlen(szStr)>2 && szStr[0]=='0' && szStr[1]=='x'){
		bX16=1;
		szPos=szStr+2;
	}else if(strlen(szStr)>2 && szStr[0]=='&' && szStr[1]=='#'){
		if(szStr[2]=='x' || szStr[2]=='X'){
			bX16=1;
			szPos=szStr+3;
		}else{
			szPos=szStr+2;
		}
	}else if(strlen(szStr)>1 && szStr[0]=='#'){
		bX16=1;
		szPos=szStr+1;
	}
	if(bX16){
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