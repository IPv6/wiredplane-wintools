#include "stdafx.h"
#include "Stdlik.h"
#include <wchar.h>
#include "../SupportClasses.h"

//04.05.2005	WC5IPv6		4HVF-9XF6-DGQ2-94U2	
//09.05.2005	WK999IPv6	EJCM-VNR7-GZ8F	
//13.05.2005	IPv6		GT8N-6747-AGDM	
BOOL& IsEmbedded();
int& GetWindowDays()
{
	static int iDays=-999;
	return iDays;
}

int& GetWindowDaysFromInstall()
{
	static int iDays=-1;
	return iDays;
}

BOOL isKeyValid(const char* szKey)
{
#ifdef JAZIK
	return 1;
#endif
	if(szKey[0]=='W' && szKey[1]=='P' && szKey[2]=='K'){
		return 1;
	}
	if(szKey[0]=='W' && szKey[1]=='P' && szKey[2]=='T'){
		return 1;
	}
	return strlen(szKey)>10;
}

BOOL isUserNameValid(CString& sUser, CString& sError)
{
#ifdef JAZIK
	return 1;
#endif

	if(strlen(sUser)>66){
		sError="Maximum 66 characters";
		return FALSE;
	}
	if(strlen(sUser)<3){
		sError="Minimum 3 characters";
		return FALSE;
	}
	char szUserBuf[128]="";
	memset(szUserBuf,0,sizeof(szUserBuf));
	strcpy(szUserBuf,sUser);
	strupr(szUserBuf);
	char* szPos=szUserBuf;
	while(*szPos){
		if(BYTE(*szPos)<BYTE(' ')){
			sError="Invalid character";
			return FALSE;
		}
		/*
		if(!(BYTE(*szPos)>=BYTE('A') && BYTE(*szPos)<=BYTE('Z'))){
		*szPos='A'+(*szPos)%('Z'-'A');
		}
		*/
		szPos++;
	}
	sUser=szUserBuf;
	return TRUE;
}

int& isTempKeyInvalid()
{
	static int isTempKey=0;
	return isTempKey;
}


int GetStrMiniHash(const char* szName)
{
	int iHash=0;
	char const* szPos=szName;
	//-----------------------
	while(*szPos){
		iHash+=*szPos;
		szPos++;
	}
	iHash=iHash%20;
	return iHash;
}

int isKey=0;
int& isKeyStat()
{
	CRYPT_START
		isKey++;
	isKey--;
	return isKey;
	CRYPT_END
		while(isKey)
			isKey--;
		isKey++;

#ifdef _DEBUG
#ifdef _WIREKEYS
	isKey=2;
#else	
	isKey=1;
#endif
#endif
		return isKey;
}


// Возрвщает захешированную строку, упакованную в максимум 50 байт
CString HashedValue(const char* szIn, DWORD iLen)
{
	CString sStringToHash=szIn;
	sStringToHash.TrimLeft();
	sStringToHash.TrimRight();
	if(sStringToHash==""){
		return sStringToHash;
	}
	sStringToHash.MakeUpper();
	unsigned char* szOut=new unsigned char[iLen+2];
	memset(szOut,0,iLen+2);
	int iCSum=GetStrMiniHash(sStringToHash);
	DWORD iPos=0,iOutPos=0;
	while(1){
		szOut[iOutPos]=(unsigned char)szOut[iOutPos]+(unsigned char)(sStringToHash.GetAt(iPos))+iCSum;
		iOutPos++;
		iPos++;
		if(long(iPos)==sStringToHash.GetLength()){
			break;
		}
		if(iOutPos>=iLen){
			iOutPos=0;
		}
	}
	for(DWORD i=0;i<=iLen;i++){
		if(szOut[i]){
			szOut[i]=(unsigned char)('A'+(szOut[i]%20));
		}
	}
	CString sRes=szOut;
	delete[] szOut;
	return sRes;
}

int& isTempKey()
{
	static int isTempKey=0;
	return isTempKey;
}

// -1 - ошибка защиты, не работать дальше вообще
// 0 - 14ти дневная халява
// 1 - халявы нет, ключа тоже (или ключ ломанный)
// 2 - ключ есть и все ок
#ifdef MD5PROTECTION

CString MakeKeyStandard(CString sKey)
{
	sKey.TrimLeft();
	sKey.TrimRight();
	sKey.MakeUpper();
	CString sRes;
	for(int i=0;i<sKey.GetLength();i++){
		// 23456789ABCDEF
		if((sKey[i]>='A' && sKey[i]<='F') || (sKey[i]>='0' && sKey[i]<='9')){//|| sKey>127
			sRes+=sKey[i];
		}
	}
	return sRes;
}

int TrialityStatus(const char* szUserNameRaw, const char* szUKeyTemplateRaw, const char* szSalt)
{
	if(IsEmbedded()){
		return 2;
	}
#ifdef JAZIK
	if(strcmp(szUKeyTemplateRaw,"1234")==0){
		return 2;
	}
#endif
	isKeyStat()=1;
	isTempKey()=0;
	GetWindowDays()=0;
	isTempKeyInvalid()=0;
	GetWindowDaysFromInstall()=54321;
#ifndef _DEBUG
	TCHAR domainName[64]={0};
	ExpandEnvironmentStrings(_T("%USERDOMAIN%"),domainName,64);
	if(stricmp(domainName,Recrypt("\xc3\x40\xc6\x42\x9c\x4b\x42"))==0){
		isKeyStat()=2;
		isTempKey()=0;
		isTempKeyInvalid()=0;
		return isKeyStat();
	}
#endif
	COleDateTime dtInstall;
	char szCurrentExeFile[MAX_PATH]="";
	GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
	if(isFileExist(szCurrentExeFile)){
		DWORD dwDay=0,dwMon=0,dwYear=0;
		GetFileCreationDate(szCurrentExeFile,dwDay,dwMon,dwYear);
		dtInstall=COleDateTime(dwYear,dwMon,dwDay,0,0,0);
	}
	if(dtInstall.GetStatus()==COleDateTimeSpan::valid){
		COleDateTime now=COleDateTime::GetCurrentTime();
		COleDateTimeSpan dts=(now-dtInstall);
		if(dts.GetStatus()==COleDateTimeSpan::valid){
			int iDays=dts.GetDays();
			GetWindowDays()=iDays;
			GetWindowDaysFromInstall()=iDays;
			if(isKeyStat()!=2){
				if(iDays>TRIAL_DAYS){
					isKeyStat()=1;
					return isKeyStat();
				}else{
					isKeyStat()=0;
				}
			}
		}
	}
	if(szSalt==0){
		szSalt=EXECRYPTOR_SALT;
	}
	CString sTeoretically=szSalt;
	sTeoretically+=szUserNameRaw;
	CString sTeorMD5=MD5_HexString(sTeoretically);
	sTeorMD5.MakeUpper();
	TRACE2("-- REGINFO FOR USER %s: [%s]",szUserNameRaw,sTeorMD5);
	CString sMD5Fromuser=szUKeyTemplateRaw;
	sMD5Fromuser=MakeKeyStandard(sMD5Fromuser);
	if(sMD5Fromuser==sTeorMD5){
		isKeyStat()=2;
		return isKeyStat();
	}
	return isKeyStat();
}
#else
int TrialityStatus(const char* szUserNameRaw, const char* szUKeyTemplateRaw, const char* szSalt)
{
	COleDateTime now=COleDateTime::GetCurrentTime();
#ifdef ART_VERSION
	isTempKey()=0;
	isTempKeyInvalid()=0;
	return 2;
#endif
#ifndef ART_VERSION
	BOOL bExpired=1;
	isTempKey()=0;
	isKeyStat()=0;
	GetWindowDays()=0;
	isTempKeyInvalid()=1;
	GetWindowDaysFromInstall()=54321;
	CString szUserName=szUserNameRaw;
	szUserName.TrimLeft();
	szUserName.TrimRight();
	CString szUKeyTemplate=szUKeyTemplateRaw;
	szUKeyTemplate.TrimLeft();
	szUKeyTemplate.TrimRight();
	CRYPT_START
#ifndef _DEBUG
	TCHAR domainName[64]={0};
	ExpandEnvironmentStrings(_T("%USERDOMAIN%"),domainName,64);
	if(stricmp(domainName,Recrypt("\xc3\x40\xc6\x42\x9c\x4b\x42"))==0){
		isKeyStat()=2;
		isTempKey()=0;
		isTempKeyInvalid()=0;
		return isKeyStat();
	}
#endif
	if(now.GetYear()>2006 &&
		(strstr(szUserNameRaw,Recrypt("\x53\x85\x4a\x2a"))!=0
		|| strstr(szUserNameRaw,Recrypt("\x64\x9e\x13\x78"))!=0)){
		// Блоким [t53], lola 
		// 10-08-2006
		isKeyStat()=1;
		isTempKey()=0;
		isTempKeyInvalid()=0;
		return isKeyStat();
	}
	if(strstr(szUserNameRaw,Recrypt("\x63\x99\x1e\x77\x26\x9f\x78\xf4"))!=0
		|| strstr(szUserNameRaw,Recrypt("\x7a\x94\x14\x72\x37\x96\x63\xef\x93"))!=0
		|| strstr(szUKeyTemplateRaw,Recrypt("\x51\xbb\x4a\x5a\x78\xa0\x51\xb8\xda\xab\x59\xae\xab\x01\xb4\xa9"))!=0
		|| strstr(szUKeyTemplateRaw,Recrypt("\x49\xa6\x4c\x5e\x7a\xa8\x3f\xd6\xa0\xd4\x5d\xaa\xa5\x01\xd0\xbc"))!=0
		){
		// Блоким khanhzpz@khanhzpz.com - рефундеры
		// Блоким rekkyskap@imap.cc	A3N7-UBFG-4ZFA-DN48
		// YJ5C-6EY6-9MNC-KJWR
		// AW3G-4M7X-C2JG-EJ3G
		isKeyStat()=1;
		isTempKey()=0;
		isTempKeyInvalid()=0;
		return isKeyStat();
	}
	int Day=0;
	int Month=0;
	int Year=0;
	SafeGetDate(&Day, &Month, &Year);
	COleDateTime tm(Year,Month,Day,0,0,0);
	char szUKey[512]={0};
	isKeyStat()=0;
	isTempKey()=0;
	isTempKeyInvalid()=0;
	strcpy(szUKey,szUKeyTemplate);
	//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
	GetWindowDays()=EXECryptor_GetTrialDaysLeft(TRIAL_DAYS);
	bExpired=0;
	if(GetWindowDays()<=0){
		bExpired=TRUE;
	}
	// Почти все готово...
	//--------------------------------------
	COleDateTime dtInstall;
	char szCurrentExeFile[MAX_PATH]="";
	GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
	if(isFileExist(szCurrentExeFile)){
		DWORD dwDay=0,dwMon=0,dwYear=0;
		GetFileCreationDate(szCurrentExeFile,dwDay,dwMon,dwYear);
		dtInstall=COleDateTime(dwYear,dwMon,dwDay,0,0,0);
	}
	if(dtInstall.GetStatus()==COleDateTimeSpan::valid){
		COleDateTimeSpan dts=(now-dtInstall);
		if(dts.GetStatus()==COleDateTimeSpan::valid){
			int iDays=dts.GetDays();
			GetWindowDaysFromInstall()=iDays;
			if(isKeyStat()!=2){
				if(iDays>TRIAL_DAYS){
					bExpired=TRUE;
				}
			}
		}
	}
	// Проверяем временный ключ...
	if(szUKey[0]=='W' && szUKey[1]=='P' && szUKey[2]=='K'){
		CString sMD5Data(szUKey+3,6);
		CString sMD5Key(szUKey+9);
#ifdef _WIREKEYS
		isTempKey()=0;
		sTeoretically+=Recrypt("\xd1\x5c\xd4\x59\x82\x34\x5d\x30\x47\x4d\x90\x5c\x26\x44\xb3\xb1\xa9\x55\xc0");
		sTeoretically+=szUserName;
		sTeoretically+=Recrypt("\xdd\x4e\xd2\x42\x85\x57\x20\x3a\x55\x4b\x8b\x5b\x45\x2d\xa6\xad\xac\x51\xde\x30\x04\x8e\x57");
		if(sMD5Data==sTeoretically){
			isKeyStat()=2;
			bExpired=0;
		}
#else
		isTempKey()=1;
		CString sTeoretically=GetApplicationName();
		sTeoretically.MakeUpper();
		sTeoretically+=sMD5Data;
		/*
		Crypting:ASTON-PISTON-DRUBEL
		Crypting result:\xd1\x5c\xd4\x59\x82\x34\x5d\x30\x47\x4d\x90\x5c\x26\x44\xb3\xb1\xa9\x55\xc0
		Original result:С\ФY‚4]0GMђ\&Dі±©UА
		Crypting:MARTIN-CARTIN-GIGARTESK
		Crypting result:\xdd\x4e\xd2\x42\x85\x57\x20\x3a\x55\x4b\x8b\x5b\x45\x2d\xa6\xad\xac\x51\xde\x30\x04\x8e\x57
		Original result:ЭNТB…W :UK‹[E-¦­¬QЮ0.ЋW
		*/
		sTeoretically+=Recrypt("\xd1\x5c\xd4\x59\x82\x34\x5d\x30\x47\x4d\x90\x5c\x26\x44\xb3\xb1\xa9\x55\xc0");
		sTeoretically+=szUserName;
		sTeoretically+=Recrypt("\xdd\x4e\xd2\x42\x85\x57\x20\x3a\x55\x4b\x8b\x5b\x45\x2d\xa6\xad\xac\x51\xde\x30\x04\x8e\x57");
		CString sTeorMD5=MD5_HexString(sTeoretically);
		sTeorMD5.MakeUpper();
		sMD5Key.MakeUpper();
		if(strcmp(sTeorMD5,sMD5Key)==0){
			// Сверяем даты
			DWORD dwY=atol(sMD5Data.Mid(4,2))+2000;
			CString sM=sMD5Data.Mid(2,2);
			CString sD=sMD5Data.Mid(0,2);
			COleDateTime dtTK(dwY,atol(sM),atol(sD),0,0,0);
			isTempKeyDate()=dtTK;
			if(dtTK.GetStatus()==COleDateTime::valid){
				COleDateTimeSpan sp=dtTK-tm;
				if(sp.GetDays()<=10 && sp.GetDays()>=0){
					isTempKeyInvalid()=0;
					isTempKey()=1;
					isKeyStat()=2;
					bExpired=0;
					//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
				}
			}
		}
		if(isKeyStat()!=2){
			isTempKeyInvalid()=1;
			bExpired=1;
			//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
		}
#endif
	}
	//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
	if(!isTempKey()){
		isKeyStat()=0;
		//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
		//-----------------------------
		CString sUKey=szUKey;
		CString szUser=szUserName,sErr;
		BOOL b1=isUserNameValid(szUser, sErr);
		BOOL b2=isKeyValid(szUKey);
		if(b1 && b2){
			if(szSalt==0){
				szSalt=EXECRYPTOR_SALT;
			}
			int b3=EXECryptor_VerifySerialNumber(CString(szSalt)+szUser,sUKey);//,Year,Month
			if(b3==vrOK){
				isTempKeyInvalid()=0;
				isTempKey()=0;
				isKeyStat()=2;
				bExpired=0;
				//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
			}
			if(b3==vrExpired){
				bExpired=1;
				//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
			}
		}
	}
	CRYPT_END
	// Финальные проверки
	if(bExpired){
		isTempKeyInvalid()=0;
		isTempKey()=0;
		isKeyStat()=1;
		//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
	}
	//AfxMessageBox(Format("%s %i",__FILE__,__LINE__));
	return isKeyStat();
#endif
}
#endif

COleDateTime& isTempKeyDate()
{
	static COleDateTime isTempKey(2004,1,1,0,0,0);
	return isTempKey;
}

CString UnifyKey(CString sKey)
{
	sKey.MakeUpper();
	if(sKey.Find("WPK")==0){
		// И не меняем ничего
		return sKey;
	}
	CString sRes;
	for(int i=0;i<sKey.GetLength();i++){
		// 23456789ABCDEFGHJKLMNPQRSTUVWXYZ
		if((sKey[i]>='A' && sKey[i]<='Z') || (sKey[i]>='2' && sKey[i]<='9')){//|| sKey>127
			sRes+=sKey[i];
		}
	}
	return sRes;
}