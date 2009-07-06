#ifndef _LIKCODEGHEN_
#define _LIKCODEGHEN_
#include "md5.h"
#include "afxtempl.h"
#define TRIAL_DAYS	14
#define MAX_HASHEDVAL	50

#ifndef MD5PROTECTION
#include "EXECryptor.h"
#else
#define CRYPT_START
#define CRYPT_END
#endif

int GetStrMiniHash(const char* szName);
CString HashedValue(const char* szIn, DWORD iLen=MAX_HASHEDVAL);
void GetRegInfoFromText(CString &sText,CString& sLikUser,CString& sLikKey, const char* szSalt=0);
CString MD5_HexString(const char* SourceString);

// ARACrypt.h : header file
//
// Note: A Special Thanks to Mr. Warren Ward for his Sept. 1998 CUJ article:
// "Stream Encryption" Copyright (c) 1998 by Warren Ward
/////////////////////////////////////////////////////////////////////////////
// ARACrypt window

class ARACrypt
{
// Construction
public:
	ARACrypt();

// Attributes
public:

// Operations
public:
	virtual void TransformString(LPCTSTR csKey, CString& csTarget);

protected:
	virtual void SetKey(LPCTSTR csKey);
	virtual void GetKey(CString& csKey);
	virtual void TransformChar(unsigned char& csTarget);

// Implementation
public:
	virtual ~ARACrypt();

// Attributes
protected:

	CString m_csKey;

    unsigned long           m_LFSR_A;
    unsigned long           m_LFSR_B;
    unsigned long           m_LFSR_C;
    const unsigned long     m_Mask_A;
    const unsigned long     m_Mask_B;
    const unsigned long     m_Mask_C;
    const unsigned long     m_Rot0_A;
    const unsigned long     m_Rot0_B;
    const unsigned long     m_Rot0_C;
    const unsigned long     m_Rot1_A;
    const unsigned long     m_Rot1_B;
    const unsigned long     m_Rot1_C;

};
CString Recrypt(CString sRes);
CString B2H(const unsigned char* digest, int iLen=-1);

class checksum {
public:
    checksum() { clear(); }
    void clear() { sum = 0; r = 55665; c1 = 52845; c2 = 22719;}
    void add(DWORD w);
    void add(BOOL w) { add((DWORD)w); }
    void add(UINT w) { add((DWORD)w); }
    void add(WORD w);
    void add(const CString & s);
    void add(LPBYTE b, UINT length);
    void add(BYTE b);
    DWORD get() { return sum; }
protected:
    WORD r;
    WORD c1;
    WORD c2;
    DWORD sum;
};
CString GetHash(const char* p);
const char* GetApplicationName();
CString Format(const char* szFormat,...)
#endif