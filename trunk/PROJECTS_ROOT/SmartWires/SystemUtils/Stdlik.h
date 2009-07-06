#ifndef _LIKCODEGHEN_
#define _LIKCODEGHEN_

int TrialityStatus(const char* szUser, const char* szKey, int iProgNum);
int& GetWindowDays();
int GetStrMiniHash(const char* szName);
CString HashedValue(CString sStringToHash);
BOOL KeyChkSum(char* szKey);
WORD GetLevel0(char* szKey);
WORD GetLevel1(char* szKey);
WORD GetLevel2(char* szKey);
WORD GetLevel3(char* szKey);
char* StripC(char* szKey);
BOOL ChkKeWthU(const char* szKey, char* szUser, int iProgNum);
WORD GetLevelU(char* szUser);
BOOL isUserNameValid(CString& szUser, CString& sError);//Подгоняет имя под стандарты
BOOL isKeyValid(const char* szKey);
//Защита от хакеров-макеров
static const char* szBlockedUsers[]={"KpTeaM",""};
static const char* szBlockedKeys[]={"SpK!Lu4Y9XF6VnCot82J",""};

#define IS_BLOCKED(x_name,y_blk) \
{\
	int iUB=0;\
	while(*y_blk[iUB]!=0){\
		if(strstr(x_name,y_blk[iUB])!=NULL){\
			CString* pOrigData=NULL;\
			pOrigData->TrimLeft();\
		}\
		iUB++;\
	}\
}
//Конец защиты от хакеров-макеров
#endif