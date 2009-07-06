// DataXMLSaver.h: interface for the CDataXMLSaver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAXMLSAVER_H__969B3E6A_56E6_471A_83C3_153683EDB494__INCLUDED_2)
#define AFX_DATAXMLSAVER_H__969B3E6A_56E6_471A_83C3_153683EDB494__INCLUDED_2

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagDOUBLE_RANGE
{
    LONG    innerBegin;
    LONG    innerEnd;
    LONG    outerBegin;
    LONG    outerEnd;
} DOUBLE_RANGE;

#define XMLP_STRIPSOURCE	0x01
#define XMLP_NOCHECKCLOTAG	0x02

DOUBLE_RANGE _tagraw(const char* szTag, const char* szSource, char cStart='{', char cStop='}', BOOL bCheckClosingTag=TRUE);
int extTag(const char* szTag, CString& szSource, CString* sOut=0, CString* sOutAttribs=0, char cStart='{', char cStop='}');
int extTagSimple(const char* szTag, CString& szSource, CString* sOut=0, CString* sOutAttribs=0, char cStart='{', char cStop='}');
int _tag(const char* szTag, CString& szSource, CString* sOut=0, CString* sOutAttribs=0, BOOL bExtract=0, char cStart='{', char cStop='}');
CString _atr(const char* szAttr,const char* szSource,const char* szDef="");
const char* FindClosingTag(const char* str,char cStart, char cStop);
CString _arr(const char* szSrc,int iIndex, const char* szDef="", const char* szd=",;\t");
BOOL isTrue(const char* sz);

#endif // !defined(AFX_DATAXMLSAVER_H__969B3E6A_56E6_471A_83C3_153683EDB494__INCLUDED_)
