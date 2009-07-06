// CCOMStringW.cpp : implementation file
//
// CCOMStringW Implementation
//
// Written by Paul E. Bible <pbible@littlefishsoftware.com>
// Copyright (c) 2000. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. If the source code in this file is used in 
// any  commercial application then a statement along the lines of 
// "Portions copyright (c) Paul E. Bible, 2000" must be included in
// the startup banner, "About" box -OR- printed documentation. An email 
// letting me know that you are using it would be nice as well. That's 
// not much to ask considering the amount of work that went into this.
// If even this small restriction is a problem send me an email.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// Expect bugs!
// 
// Please use and enjoy, and let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into 
// this file. 
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crt_cstringw.h"
#include <atlbase.h>
#include <WCHAR.H>
#include <stdio.h>


//////////////////////////////////////////////////////////////////////
//
// Constructors/Destructor
//
//////////////////////////////////////////////////////////////////////

CCOMStringW::CCOMStringW()
{
	m_pszString = NULL;
	AllocString(0);
}

CCOMStringW::CCOMStringW(CCOMStringW& str)
{
	m_pszString = NULL;
	int nLen = str.GetLength();
	AllocString(nLen);
	wcsncpy(m_pszString, (const WCHAR* ) str, nLen);
}

CCOMStringW::CCOMStringW(const WCHAR*  pszString)
{
	m_pszString = NULL;
	int nLen = wcslen(pszString);
	AllocString(nLen);
	wcsncpy(m_pszString, pszString, nLen);
}

CCOMStringW::CCOMStringW(WCHAR ch, int nRepeat) 
{
	m_pszString = NULL;
	if (nRepeat > 0)
	{
		AllocString(nRepeat);
		for (int i=0; i < nRepeat; i++)
			m_pszString[i] = ch;
	}
}

CCOMStringW::~CCOMStringW()
{
	free(m_pszString);
}


//////////////////////////////////////////////////////////////////////
//
// Memory Manipulation
//
//////////////////////////////////////////////////////////////////////

void CCOMStringW::AllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	if (m_pszString != NULL)
		free(m_pszString);
	m_pszString = (WCHAR*) malloc((nLen+1) * sizeof(WCHAR));
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = '\0';
}

void CCOMStringW::ReAllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	m_pszString = (WCHAR*) realloc(m_pszString, (nLen+1) * sizeof(WCHAR));
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = '\0';
}

void CCOMStringW::StringCopy(int nSrcLen, const WCHAR*  lpszSrcData)
{
	AllocString(nSrcLen);
	memcpy(m_pszString, lpszSrcData, nSrcLen * sizeof(WCHAR));
	m_pszString[nSrcLen] = '\0';
}

void CCOMStringW::StringCopy(CCOMStringW &str, int nLen, int nIndex, int nExtra) const
{
	int nNewLen = nLen + nExtra;
	if (nNewLen != 0)
	{
		str.AllocString(nNewLen);
		memcpy(str.GetString(), m_pszString+nIndex, nLen * sizeof(WCHAR));
	}
}

void CCOMStringW::ConcatCopy(const WCHAR*  lpszData)
{
	if(lpszData == NULL){
		return;
	}

	// Save the existing string
	int nLen = GetLength();
	WCHAR* pszTemp = (WCHAR*) malloc((nLen+1) * sizeof(WCHAR));
	memcpy(pszTemp, m_pszString, nLen * sizeof(WCHAR));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nDataLen = wcslen(lpszData);
	int nNewLen = nLen + nDataLen;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(WCHAR));
	memcpy(m_pszString+nLen, lpszData, nDataLen * sizeof(WCHAR));

	// Cleanup
	free(pszTemp);
}

void CCOMStringW::ConcatCopy(WCHAR ch)
{
	// Save the existing string
	int nLen = GetLength();
	WCHAR* pszTemp = (WCHAR*) malloc((nLen+1) * sizeof(WCHAR));
	memcpy(pszTemp, m_pszString, nLen * sizeof(WCHAR));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nNewLen = nLen + 1;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(WCHAR));
	m_pszString[nNewLen-1] = ch;

	// Cleanup
	//free(pszTemp);
}

void CCOMStringW::ConcatCopy(const WCHAR*  lpszData1, const WCHAR*  lpszData2)
{
	if(lpszData1 == NULL){
		return;
	}
	if(lpszData2 == NULL){
		return;
	}
	int nLen1 = wcslen(lpszData1);
	int nLen2 = wcslen(lpszData2);
	int nLen = nLen1 + nLen2;
	AllocString(nLen);
	memcpy(m_pszString, lpszData1, nLen1 * sizeof(WCHAR));
	memcpy(m_pszString+nLen1, lpszData2, nLen2 * sizeof(WCHAR)); 
}

BSTR CCOMStringW::AllocSysString()
{
	BSTR bstr = ::SysAllocStringLen(m_pszString, GetLength());
	if (bstr == NULL)
	{
		ATLASSERT(0);
		return NULL;
	}
	return bstr;
}


//////////////////////////////////////////////////////////////////////
//
// Accessors for the String as an Array
//
//////////////////////////////////////////////////////////////////////

void CCOMStringW::Empty()
{
	if (wcslen(m_pszString) > 0)
		m_pszString[0] = '\0';
}

WCHAR CCOMStringW::GetAt(int nIndex)
{
	int nLen = wcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

WCHAR CCOMStringW::operator[] (int nIndex)
{
	int nLen = wcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

void CCOMStringW::SetAt(int nIndex, WCHAR ch)
{
	int nLen = wcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	m_pszString[nIndex] = ch;	
}

int CCOMStringW::GetLength() const
{ 
	return wcslen(m_pszString); 
}

bool CCOMStringW::IsEmpty() const
{ 
	return (GetLength() > 0) ? false : true; 
}


//////////////////////////////////////////////////////////////////////
//
// Conversions
//
//////////////////////////////////////////////////////////////////////

void CCOMStringW::MakeUpper()
{
	_wcsupr(m_pszString);
}

void CCOMStringW::MakeLower()
{
	_wcslwr(m_pszString);
}

void CCOMStringW::MakeReverse()
{
	_wcsrev(m_pszString);
}

void CCOMStringW::TrimLeft()
{
	LPWSTR lpsz = m_pszString;

	while (iswspace(*lpsz)){
		lpsz = lpsz+1;
	}

	if (lpsz != m_pszString)
	{
		memmove(m_pszString, lpsz, (GetLength()+1) * sizeof(WCHAR));
	}
}

void CCOMStringW::TrimRight()
{
	LPWSTR lpsz = m_pszString;
	LPWSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (iswspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = lpsz+1;
	}

	if (lpszLast != NULL)
		*lpszLast = '\0';
}


//////////////////////////////////////////////////////////////////////
//
// Searching
//
//////////////////////////////////////////////////////////////////////

int CCOMStringW::Find(WCHAR ch) const
{
	return Find(ch, 0);
}

int	CCOMStringW::ReverseFind(WCHAR ch) const
{
	return ReverseFind(ch, 0);
}

int	CCOMStringW::ReverseFind(WCHAR ch, int nStart) const
{
	LPWSTR lpsz = wcsrchr(m_pszString + nStart, (wchar_t) ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CCOMStringW::Find(WCHAR ch, int nStart) const
{
	if (nStart >= GetLength())
		return -1;
	LPWSTR lpsz = wcschr(m_pszString + nStart, (wchar_t) ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CCOMStringW::Find(const WCHAR*  lpszSub)
{
	return Find(lpszSub, 0);
}

int CCOMStringW::Find(const WCHAR*  lpszSub, int nStart)
{
	ATLASSERT(wcslen(lpszSub) > 0);

	if (nStart > GetLength())
		return -1;

	LPWSTR lpsz = wcsstr(m_pszString + nStart, lpszSub);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CCOMStringW::FindOneOf(const WCHAR*  lpszCharSet) const
{
	LPWSTR lpsz = wcspbrk(m_pszString, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}


//////////////////////////////////////////////////////////////////////
//
// Assignment Operations
//
//////////////////////////////////////////////////////////////////////

const CCOMStringW& CCOMStringW::operator=(CCOMStringW& strSrc)
{
	if (m_pszString != strSrc.GetString())
		StringCopy(strSrc.GetLength(), strSrc.GetString());
	return *this;
}

const CCOMStringW& CCOMStringW::operator=(const WCHAR*  lpsz)
{
	if(lpsz == NULL){
		lpsz=L"";
	}
	StringCopy(wcslen(lpsz), lpsz);
	return *this;
}

//////////////////////////////////////////////////////////////////////
//
// Concatenation
//
//////////////////////////////////////////////////////////////////////

CCOMStringW operator+(CCOMStringW& strSrc1, CCOMStringW& strSrc2)
{
	CCOMStringW s;
	s.ConcatCopy((const WCHAR* ) strSrc1, (const WCHAR* ) strSrc2);
	return s;
}

CCOMStringW operator+(CCOMStringW& strSrc, const WCHAR*  lpsz)
{
	CCOMStringW s;
	s.ConcatCopy((const WCHAR* ) strSrc, lpsz);
	return s;
}

CCOMStringW operator+(const WCHAR*  lpsz, CCOMStringW& strSrc)
{
	CCOMStringW s;
	s.ConcatCopy(lpsz, (const WCHAR* ) strSrc);
	return s;
}

const CCOMStringW& CCOMStringW::operator+=(CCOMStringW& strSrc)
{
	ConcatCopy((const WCHAR* ) strSrc);
	return *this;
}

const CCOMStringW& CCOMStringW::operator+=(const WCHAR*  lpsz)
{
	ConcatCopy(lpsz);
	return *this;
}

const CCOMStringW& CCOMStringW::operator+=(WCHAR ch)
{
	USES_CONVERSION;
	ConcatCopy(ch);
	return *this;
}


//////////////////////////////////////////////////////////////////////
//
// Extraction
//
//////////////////////////////////////////////////////////////////////

CCOMStringW CCOMStringW::Mid(int nFirst, int nCount)
{
	if(nCount<0){
		nCount=GetLength() - nFirst;
	}
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;

	if (nFirst + nCount > GetLength())
		nCount = GetLength() - nFirst;
	if (nFirst > GetLength())
		nCount = 0;

	ATLASSERT(nFirst >= 0);
	ATLASSERT(nFirst + nCount <= GetLength());

	if (nFirst == 0 && nFirst + nCount == GetLength())
		return CCOMStringW(*this);

	CCOMStringW newStr;
	StringCopy(newStr, nCount, nFirst, 0);
	return newStr;
}

CCOMStringW CCOMStringW::Left(int nCount) 
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetLength())
		return CCOMStringW(*this);

	CCOMStringW newStr;
	StringCopy(newStr, nCount, 0, 0);
	return newStr;
}

CCOMStringW CCOMStringW::Right(int nCount) 
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetLength())
		return CCOMStringW(*this);

	CCOMStringW newStr;
	StringCopy(newStr, nCount, GetLength() - nCount, 0);
	return newStr;
}

CCOMStringW CCOMStringW::SpanIncluding(const WCHAR*  lpszCharSet) 
{
	ATLASSERT(lpszCharSet != NULL);
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, -1) == 0);
	return Left(wcsspn(m_pszString, lpszCharSet));
}

CCOMStringW CCOMStringW::SpanExcluding(const WCHAR*  lpszCharSet) 
{
	ATLASSERT(lpszCharSet != NULL);
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, -1) == 0);
	return Left(wcscspn(m_pszString, lpszCharSet));
}


//////////////////////////////////////////////////////////////////////
//
// Comparison
//
//////////////////////////////////////////////////////////////////////
	
int CCOMStringW::Compare(CCOMStringW& str) const
{
	ATLASSERT((const WCHAR* ) str != NULL);
	ATLASSERT(::IsBadStringPtrW((const WCHAR* ) str, -1) == 0);
	return wcscmp(m_pszString, (const WCHAR* ) str);	
}

int CCOMStringW::Compare(const WCHAR*  lpsz) const
{
	ATLASSERT(lpsz != NULL);
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
	return wcscmp(m_pszString, lpsz);	
}

int CCOMStringW::CompareNoCase(CCOMStringW& str) const
{
	ATLASSERT((const WCHAR* ) str != NULL);
	ATLASSERT(::IsBadStringPtrW((const WCHAR* ) str, -1) == 0);
	return _wcsicmp(m_pszString, (const WCHAR* ) str);	
}

int CCOMStringW::CompareNoCase(const WCHAR*  lpsz) const
{
	ATLASSERT(lpsz != NULL);
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
	return _wcsicmp(m_pszString, lpsz);	
}

int CCOMStringW::Collate(const WCHAR*  lpsz) const
{
	ATLASSERT(lpsz != NULL);
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
	return wcscoll(m_pszString, lpsz);	
}

int CCOMStringW::Collate(CCOMStringW &str) const
{
	ATLASSERT((const WCHAR* ) str != NULL);
	ATLASSERT(::IsBadStringPtrW((const WCHAR* ) str, -1) == 0);
	return wcscoll(m_pszString, (const WCHAR* ) str);	
}


//////////////////////////////////////////////////////////////////////
//
// Formatting
//
//////////////////////////////////////////////////////////////////////
CCOMStringW toString(const WCHAR*  pszCharSet, ...)
{
	va_list vl;
	va_start(vl, pszCharSet);

	WCHAR* pszTemp = NULL;
	int nBufferSize = 0;
	int nRetVal = -1;

	do {
		nBufferSize += 100;
		delete [] pszTemp;
		pszTemp = new WCHAR[nBufferSize];
		nRetVal = _vsnwprintf(pszTemp, nBufferSize, pszCharSet, vl);
	} while (nRetVal < 0);
	CCOMStringW sRes=pszTemp;
	delete [] pszTemp;
	va_end(vl);
	return sRes;
}
	
void CCOMStringW::Format(const WCHAR*  pszCharSet, ...)
{
	va_list vl;
	va_start(vl, pszCharSet);

	WCHAR* pszTemp = NULL;
	int nBufferSize = 0;
	int nRetVal = -1;

	do {
		nBufferSize += 100;
		delete [] pszTemp;
		pszTemp = new WCHAR[nBufferSize];
		nRetVal = _vsnwprintf(pszTemp, nBufferSize, pszCharSet, vl);
	} while (nRetVal < 0);

	int nNewLen = wcslen(pszTemp);
	AllocString(nNewLen);
	wcscpy(m_pszString, pszTemp);
	delete [] pszTemp;

	va_end(vl);
}


//////////////////////////////////////////////////////////////////////
//
// Replacing
//
//////////////////////////////////////////////////////////////////////

int CCOMStringW::Replace(WCHAR chOld, WCHAR chNew)
{
	int nCount = 0;

	if (chOld != chNew)
	{
		LPWSTR psz = m_pszString;
		LPWSTR pszEnd = psz + GetLength();
		while(psz < pszEnd)
		{
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = psz+1;
		}
	}

	return nCount;
}

int CCOMStringW::Replace(const WCHAR*  lpszOld, const WCHAR*  lpszNew)
{
	int nSourceLen = wcslen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplaceLen = wcslen(lpszNew);

	int nCount = 0;
	LPWSTR lpszStart = m_pszString;
	LPWSTR lpszEnd = lpszStart + GetLength();
	LPWSTR lpszTarget;

	// Check to see if any changes need to be made
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
		{
			lpszStart = lpszTarget + nSourceLen;
			nCount++;
		}
		lpszStart += wcslen(lpszStart) + 1;
	}

	// Do the actual work
	if (nCount > 0)
	{
		int nOldLen = GetLength();
		int nNewLen = nOldLen + (nReplaceLen - nSourceLen) * nCount;
		if (GetLength() < nNewLen)
		{
			CCOMStringW szTemp = m_pszString;
			ReAllocString(nNewLen);
			memcpy(m_pszString, (const WCHAR* ) szTemp, nOldLen * sizeof(WCHAR));
		}

		lpszStart = m_pszString;
		lpszEnd = lpszStart + GetLength();

		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLen - (lpszTarget - m_pszString + nSourceLen);
				memmove(lpszTarget + nReplaceLen, lpszTarget + nSourceLen, nBalance * sizeof(WCHAR));
				memcpy(lpszTarget, lpszNew, nReplaceLen * sizeof(WCHAR));
				lpszStart = lpszTarget + nReplaceLen;
				lpszStart[nBalance] = '\0';
				nOldLen += (nReplaceLen - nSourceLen);
			}
			lpszStart += wcslen(lpszStart) + 1;
		}
		ATLASSERT(m_pszString[GetLength()] == '\0');
	}
	return nCount;
}

