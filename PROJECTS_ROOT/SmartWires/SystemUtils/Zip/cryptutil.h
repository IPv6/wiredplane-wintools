#ifndef _CRYPT_UTIL_ZIP_H_
#define _CRYPT_UTIL_ZIP_H_
// for crypting

#include "zlib.h"
#include <time.h>

typedef struct crypt_info
{
	char pwd[64];
	uLong keys[3];
	int isCalcCRC;
	int CalcCRC;
} crypt_info;

//int CryptUnZipInit(FILE* source,uLong crc,uLong dosDate,uLong flg,crypt_info* cr);
//void buffdecode(char* buff,int len,crypt_info* cr);
//void crypthead(char *passwd,uLong crc,FILE* zfile,crypt_info* cr);
//void cryptwrite(char* buff,int len,int i,FILE* f,crypt_info* cr);
int decrypt_byte(crypt_info* cr);
int update_keys(int c,crypt_info* cr);
void init_keys(char* passwd,crypt_info* cr);
uLong GetCRCForFile(FILE* f);
uLong ToZipTime(time_t nowtime);

#endif

