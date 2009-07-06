#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include "cryptutil.h"

#define FILEBUFFERSIZE 2048

char* pwd=NULL;
int isCalcCRC=0;
int CalcCRC=0;
const uLong* crc_get=get_crc_table();

#define zdecode(c,cr)		update_keys((c) ^= decrypt_byte(cr),(cr))
#define zencode(c,t,cr)		((t)=decrypt_byte(cr), update_keys((c),(cr)), (t)^((c)))
#define CRC32(c, b)			(crc_get[((int)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

int decrypt_byte(crypt_info* cr)
{
    unsigned temp;
    temp = (cr->keys[2] & 0xffff) | 2;
    return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
}

int update_keys(int c,crypt_info* cr)
{
    cr->keys[0] = CRC32(cr->keys[0], c);
    cr->keys[1] += cr->keys[0] & 0xff;
    cr->keys[1] = ((cr->keys[1]) * (134775813L) + 1);
	int keyshift = (int)(cr->keys[1] >> 24);
	cr->keys[2] = CRC32(cr->keys[2], keyshift);
    return c;
}

void init_keys(char* passwd,crypt_info* cr)
{
    cr->keys[0] = 0x12345678;
    cr->keys[1] = 0x23456789;
    cr->keys[2] = 0x34567890;
    while (*passwd != '\0') {
        update_keys(*passwd,cr);
        passwd++;
    }
}

#define RAND_HEAD_LEN  12
#define ZCR_SEED2 (unsigned)3141592654L

extern "C" void cryptwrite(char* buff,int len,int i,FILE* f,crypt_info* cr)
{
    int t;                      
    int c;                      
	len*=i;
	for (;len>0;len--,buff++){
		c=*buff;
        int ztemp = zencode(c, t,cr);
		fputc(ztemp,f);
	};
};
extern "C" void buffdecode(char* buff,int len,crypt_info* cr)
{
	int c;
	for (;len>0;len--,buff++){
		c=*buff;
        int ztemp = zdecode(c,cr);
		*buff=ztemp;
	};
};

extern "C" void crypthead(char *passwd,uLong crc,FILE* zfile,crypt_info* cr)
{
    int n;                      
    int t;                      
    int c;                      
    int ztemp;                  
    char header[RAND_HEAD_LEN-2];
	static int i=0;
	if (i==0){
		srand((unsigned)time(NULL) ^ ZCR_SEED2);
		i++;
	};
    init_keys(passwd,cr);
    for (n = 0; n < RAND_HEAD_LEN-2; n++) {
        c = (rand() >> 7) & 0xff;
        header[n] = zencode(c, t,cr);
    }
    init_keys(passwd,cr);
	// random data
    for (n = 0; n < RAND_HEAD_LEN-2; n++) {
        ztemp = zencode(header[n], t,cr);
        putc(ztemp, zfile);
    }
	// for pkzip this must be CRC for Winzip (CRC or filetime)
	// so for pkzip we must know CRC of zipped file
    ztemp = zencode((int)(crc >> 16) & 0xff, t,cr);
    putc(ztemp, zfile);
    ztemp = zencode((int)(crc >> 24) & 0xff, t,cr);
    putc(ztemp, zfile);
}

uLong ToZipTime(time_t nowtime)
{
	struct tm* ptm;
	ptm=localtime(&nowtime);
	uLong year = (uLong)ptm->tm_year;
    if (year>1980)
        year-=1980;
    else if (year>80)
        year-=80;
    return
      (uLong) (((ptm->tm_mday) + (32 * (ptm->tm_mon+1)) + (512 * year)) << 16) |
        ((ptm->tm_sec/2) + (32* ptm->tm_min) + (2048 * (uLong)ptm->tm_hour));
};

uLong GetZipTime()
{
    time_t nowtime;
	time(&nowtime);
	return ToZipTime(nowtime);
};

uLong GetCRCForFile(FILE* f)
{
	uLong crc=0;
	unsigned char buff[FILEBUFFERSIZE];
	while (!feof(f)){
		size_t len=fread(buff,1,FILEBUFFERSIZE,f);	
		crc= crc32(crc,buff,len);
	};
	return crc;
};

extern "C" int CryptUnZipInit(FILE* source,uLong crc,uLong dosDate,uLong flg,crypt_info* cr)
{
    init_keys(cr->pwd,cr);
    /* Decrypt encryption header, save last two bytes */
    unsigned int c1 = 0,c0,r;
    for (r = RAND_HEAD_LEN; r; r--) {
        c0 = c1;
        if ((c1 = getc(source)) == EOF)
			return 0;
        zdecode(c1,cr);
    }
    c0++;
    if ( c1!=( (flg & 8) ? (255 & (dosDate >> 8)): (crc >> 24)) )
        return 0;
    return 1;
}
