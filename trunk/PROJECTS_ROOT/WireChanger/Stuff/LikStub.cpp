#include "stdafx.h"
#include "..\..\SmartWires\SystemUtils\Lik\Stdlik.h"

int WINAPI GetUserInfo(const char* szU, const char* szN,int* iDays, int* iDaysFI, const char* szHash, int* iTempKI, int* iTempK, COleDateTime* iDat)
{
	if(szHash==0 || szU==0 || szN==0 || iDays==0 || iDaysFI==0 || strcmp(szHash,"Note2crackers: you can freely patch this app, we don`t care anymore :)")!=0){
		// Нас ломают ну и хрен с ними
		return 1;
	}
	int iRes=0;
	CRYPT_START
	CString sLikKey=szN;
	sLikKey=UnifyKey(sLikKey);
	// Сверяем заблоки. Ключи должны быть без '-' в верхнем регистре
	CString sLikUser=szU;
	iRes=TrialityStatus(sLikUser,sLikKey);
	*iDays=GetWindowDays();
	*iDaysFI=GetWindowDaysFromInstall();
	*iTempKI=isTempKeyInvalid();
	*iTempK=isTempKey();
	*iDat=isTempKeyDate();
	CRYPT_END
	return iRes;
}

const char* GetApplicationName()
{
	return "WireChanger";
}

BOOL GetFileCreationDate(CString sStartDir, DWORD& dwDay,DWORD& dwMon,DWORD& dwYear)
{
	HANDLE hFile = ::CreateFile(sStartDir, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile==INVALID_HANDLE_VALUE ){
		return FALSE;
	}
	FILETIME CreationTime;
	memset(&CreationTime,0,sizeof(CreationTime));
	if(!GetFileTime(hFile,NULL,NULL,&CreationTime)){
		return FALSE;
	}
	CloseHandle(hFile);
	SYSTEMTIME SystemTime;
	memset(&SystemTime,0,sizeof(SystemTime));
	if(!FileTimeToSystemTime(&CreationTime, &SystemTime)){
		return FALSE;
	}
	dwYear=SystemTime.wYear;
	dwMon=SystemTime.wMonth;
	dwDay=SystemTime.wDay;
	return TRUE; 
}