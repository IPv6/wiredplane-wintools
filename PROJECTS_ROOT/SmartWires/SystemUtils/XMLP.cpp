// DataXMLSaver.cpp: implementation of the CDataXMLSaver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLP.h"

// Возвращает указатель на символ с закрывающим тегом
const char* FindClosingTag(const char* str,const char* szStart, const char* szStop)
{
	if(!str){
		return 0;
	}
	int iStartLen=strlen(szStart);
	int iStopLen=strlen(szStop);
	int iLevel=0;
	while(*str){
		if(memcmp(str,szStart,iStartLen)==0){
			str+=iStartLen;
			iLevel++;
		}else if(memcmp(str,szStop,iStopLen)==0){
			iLevel--;
			if(iLevel==0){
				return str;
			}
			str+=iStopLen;
		}else{
			str++;
		}
		if(iLevel<0){
			return 0;
		}
	}
	return 0;
}

DOUBLE_RANGE _tagraw(const char* szTag, const char* szSource, char cStart, char cStop, BOOL bCheckClosingTag)
{
	// Формируем тег - начало
	DOUBLE_RANGE res;
	res.outerBegin=res.innerEnd=res.outerBegin=res.outerEnd=-1;
	if(!szSource || !szTag){
		return res;
	}
	char const* szBegin=strstr(szSource,CString(cStart)+szTag);
	if(szBegin==0){
		return res;
	}
	res.outerBegin=(szBegin-szSource);
	char const* szBeginninEnd=FindClosingTag(szBegin,CString(cStart), CString(cStop));
	if(!szBeginninEnd){
		return res;
	}
	if((bCheckClosingTag&XMLP_NOCHECKCLOTAG)!=0){
		res.outerEnd=(szBeginninEnd-szSource)+1;
		res.innerEnd=res.outerEnd-1;
		res.innerBegin=res.outerBegin+1;
		return res;
	}
	if(szBeginninEnd[-1]=='/'){
		res.outerEnd=(szBeginninEnd-szSource)+1;
		res.innerEnd=res.outerEnd-1;
		res.innerBegin=res.innerEnd;//res.outerBegin+1;
		return res;
	}
	res.innerBegin=(szBeginninEnd-szSource)+1;
	char const* szEnd=FindClosingTag(szBegin,CString(cStart)+szTag, CString(cStart)+CString("/")+szTag);
	if(!szEnd){
		return res;
	}
	res.innerEnd=(szEnd-szSource);
	char const* szEndEnding=FindClosingTag(szEnd,CString(cStart), CString(cStop));
	if(!szEndEnding){
		return res;
	}
	res.outerEnd=(szEndEnding-szSource)+1;
	return res;
}

int _tag(const char* szTag, CString& szSource, CString* sOut, CString* sOutAttribs, BOOL bExtract, char cStart, char cStop)
{
	BOOL bInner=0,bAttr=0;
	CString sInner,sAttribs;
	DOUBLE_RANGE r=_tagraw(szTag,szSource,cStart, cStop,bExtract);
	if(sOut){
		if(r.innerBegin!=-1 && r.innerEnd!=-1){
			sInner=szSource.Mid(r.innerBegin,r.innerEnd-r.innerBegin);
			bInner=1;
		}
	}
	if(sOutAttribs){
		if(r.outerBegin!=-1 && r.innerBegin!=-1 && r.outerBegin!=r.innerBegin){
			int iTagLen=strlen(szTag)+1;
			sAttribs=szSource.Mid(r.outerBegin+iTagLen,r.innerBegin-r.outerBegin-1-iTagLen);
			bAttr=1;
		}
	}
	if((bExtract&XMLP_STRIPSOURCE)!=0){
		if(r.outerBegin!=-1 && r.outerEnd!=-1){
			szSource=szSource.Left(r.outerBegin)+szSource.Mid(r.outerEnd);
		}
	}
	if(sOut && bInner){
		*sOut=sInner;
	}
	if(sOutAttribs && bAttr){
		sAttribs.TrimLeft();
		sAttribs.TrimRight();
		*sOutAttribs=sAttribs;
	}
	return r.outerBegin;
}


int extTag(const char* szTag, CString& szSource, CString* sOut, CString* sOutAttribs, char cStart, char cStop)
{
	return _tag(szTag, szSource, sOut, sOutAttribs, XMLP_STRIPSOURCE, cStart, cStop);
}

int extTagSimple(const char* szTag, CString& szSource, CString* sOut, CString* sOutAttribs, char cStart, char cStop)
{
	return _tag(szTag, szSource, sOut, sOutAttribs, XMLP_NOCHECKCLOTAG|XMLP_STRIPSOURCE, cStart, cStop);
}

CString _atr(const char* szAttr,const char* szSource,const char* szDef)
{
	CString s=szAttr;
	char const* szBegin=strstr(szSource,s+"{");
	if(!szBegin){
	    // Смотрим с пробелами и табуляциями
		szBegin=strstr(szSource,s);
		if(szBegin==0){
			return szDef;
		}
		while(s[0]!=0){
			szBegin+=s.GetLength();
			while(*szBegin && (*szBegin==' ' || *szBegin=='\t' || *szBegin=='\r' || *szBegin=='\n')){
				szBegin++;
			}
			if(*szBegin!='{'){
				szBegin=strstr(szBegin,s);
				if(szBegin==0){
					return szDef;
				}
				continue;
			}
			break;
		}
	}else{
		szBegin+=s.GetLength();
	}
	char const* szEnd=FindClosingTag(szBegin,"{", "}");//szBegin обязательно должен казать на {
	if(!szEnd){
		return szDef;
	}
	return CString(szBegin+1,szEnd-szBegin-1);
}

CString _arr(const char* szSrc,int iIndex, const char* szDef, const char* szd)
{
	CString sSrc=szSrc;
	if(sSrc.Find("(")!=-1){
		_tag("", sSrc, &sSrc, 0, XMLP_NOCHECKCLOTAG, '(', ')');
	}
	while(iIndex>=0){
		CString sPart=sSrc.SpanExcluding(szd);
		iIndex--;
		if(iIndex<0){
			return sPart;
		}
		if(sSrc.GetLength()<=sPart.GetLength()+1){
			return szDef;
		}
		sSrc=sSrc.Mid(sPart.GetLength()+1);
	}
	return szDef;
}

BOOL isTrue(const char* sz)
{
	if(sz && sz[0]=='1'){
		return 1;
	}
	if(sz && stricmp(sz,"true")==0){
		return 1;
	}
	if(sz && stricmp(sz,"yes")==0){
		return 1;
	}
	return FALSE;
}

CString Xml2Str(const char* s)
{
	CString sRes=s;
	sRes.Replace("&lt;","<");
	sRes.Replace("&gt;",">");
	sRes.Replace("&quot;","\"");
	sRes.Replace("&amp;","&");
	return sRes;
}
