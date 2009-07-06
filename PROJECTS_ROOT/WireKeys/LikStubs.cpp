#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

int& GetWindowDaysFromInstall()
{_XLOG_
	static int i=0;
	return i;
}

int& GetWindowDays()
{_XLOG_
	static int i=0;
	return i;
}

BOOL isUserNameValid(CString& sUser, CString& sError)
{_XLOG_
	if(strlen(sUser)>66){_XLOG_
		sError="Maximum 66 characters";
		return FALSE;
	}
	if(strlen(sUser)<3){_XLOG_
		sError="Minimum 3 characters";
		return FALSE;
	}
	char szUserBuf[128]="";
	memset(szUserBuf,0,sizeof(szUserBuf));
	strcpy(szUserBuf,sUser);
	strupr(szUserBuf);
	char* szPos=szUserBuf;
	while(*szPos){_XLOG_
		if(BYTE(*szPos)<BYTE(' ')){_XLOG_
			sError="Invalid character";
			return FALSE;
		}
		/*
		if(!(BYTE(*szPos)>=BYTE('A') && BYTE(*szPos)<=BYTE('Z'))){_XLOG_
		*szPos='A'+(*szPos)%('Z'-'A');
		}
		*/
		szPos++;
	}
	sUser=szUserBuf;
	return TRUE;
}

BOOL isKeyValid(const char* szKey)
{_XLOG_
	if(szKey[0]=='W' && szKey[1]=='P' && szKey[2]=='K'){_XLOG_
		return 1;
	}
	return strlen(szKey)>10;
}

typedef int (WINAPI *_GetUserInfo)(const char* szU, const char* szN,int* iDays, int* iDaysFI, const char* szHash);
int UpdateTrialityStatus(CString sUser, CString sKeys)
{_XLOG_
	__FLOWLOG;
	static DWORD dwLastCheck=0;
	if(dwLastCheck==0 || GetTickCount()-dwLastCheck>3000){_XLOG_
		dwLastCheck=GetTickCount();
	}else{_XLOG_
		return objSettings.iLikStatus;
	}
	long lPrevRegStatus=objSettings.iLikStatus;
	objSettings.iLikStatus=1;
	HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils.dll");
	DWORD dwErr=GetLastError();
	BOOL bExecuted=0;
	if(hUtils){_XLOG_
		_GetUserInfo fp=(_GetUserInfo)DBG_GETPROC(hUtils,"GetUserInfo");
		if(fp){_XLOG_
			bExecuted=1;
#ifdef GPL
			objSettings.iLikStatus=2;
#elif _DEBUG
			objSettings.iLikStatus=2;
#else
			objSettings.iLikStatus=(*fp)(sUser,sKeys,&GetWindowDays(),&GetWindowDaysFromInstall(),"Note2crackers: you can freely patch this app, we don`t care anymore :)");
#endif
		}
	}
	FreeLibrary(hUtils);
	if(lPrevRegStatus!=objSettings.iLikStatus && pMainDialogWindow){_XLOG_
		pMainDialogWindow->SetRegInfo();
	}
	if(bExecuted==0){_XLOG_
		AfxMessageBox(Format("%s, error=%i",_l("ERROR: Important files are missing. PLease reinstall application"),dwErr));
	}
	return objSettings.iLikStatus;
}