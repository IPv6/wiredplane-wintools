// CCOMString.h : header file
//
// CCOMString Header
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

#if !defined(_COMSTR)
#define _COMSTR

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCOMString  
{
public: 
	CCOMString();
	//CCOMString(CCOMString szString);
	CCOMString(CCOMString& szString);
	CCOMString(LPCTSTR pszString);
	CCOMString(BSTR bstrString);
	CCOMString(TCHAR ch, int nRepeat);
	CCOMString(TCHAR ch);
	virtual ~CCOMString();
	
	// Assignment Operations
	const CCOMString& operator=(CCOMString& strSrc);
	const CCOMString& operator=(LPCTSTR lpsz);
	const CCOMString& operator=(BSTR bstrStr);
	operator LPCTSTR() const	{ return m_pszString; }
	TCHAR*	GetString()			{ return m_pszString; }
	LPCTSTR c_str()				{ return m_pszString; }
	BSTR	AllocSysString();
	
	// Concatenation
	const CCOMString& operator+=(CCOMString& strSrc);
	const CCOMString& operator+=(LPCTSTR lpsz);
	const CCOMString& operator+=(BSTR bstrStr);
	const CCOMString& operator+=(TCHAR ch);
	friend CCOMString operator+(CCOMString& strSrc1, CCOMString& strSrc2);
	friend CCOMString operator+(CCOMString& strSrc, LPCTSTR lpsz);
	friend CCOMString operator+(LPCTSTR lpsz, CCOMString& strSrc);
	friend CCOMString operator+(CCOMString& strSrc, BSTR bstrStr);
	friend CCOMString operator+(BSTR bstrStr, CCOMString& strSrc);

	// Accessors for the String as an Array
	int		GetLength() const;
	bool	IsEmpty() const;
	void	Empty();
	TCHAR	GetAt(int nIndex);
	void	SetAt(int nIndex, TCHAR ch);
	TCHAR	operator[] (int nIndex);

	// Conversions
	void	MakeUpper();
	void	MakeLower();
	void	MakeReverse();
	void	TrimLeft();
	void	TrimRight();

	// Searching
	int		Find(TCHAR ch) const;
	int		Find(TCHAR ch, int nStart) const;
	int		Find(LPCTSTR lpszSub);
	int		Find(LPCTSTR lpszSub, int nStart);
	int		FindOneOf(LPCTSTR lpszCharSet) const;
	int		ReverseFind(TCHAR ch, int nStart) const;
	int		ReverseFind(TCHAR ch) const;
	// Extraction
	CCOMString Mid(int nFirst, int nCount=-1) ;
	CCOMString Left(int nCount) ;
	CCOMString Right(int nCount) ;
	CCOMString SpanIncluding(LPCTSTR lpszCharSet);
	CCOMString SpanExcluding(LPCTSTR lpszCharSet);

	// Comparison
	int Compare(CCOMString& str) const;
	int Compare(LPCTSTR lpsz) const;
	int CompareNoCase(CCOMString& str) const;
	int CompareNoCase(LPCTSTR lpsz) const;
	int Collate(CCOMString& str) const;
	int Collate(LPCTSTR lpsz) const;

	// Formatting
	void Format(LPCTSTR pszCharSet, ...);

	// Replacing
	int Replace(TCHAR chOld, TCHAR chNew);
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);

	operator LPCSTR(){return GetString();};	
protected:
	LPTSTR	m_pszString;
	void	StringCopy(CCOMString& str, int nLen, int nIndex, int nExtra) const;
	void	StringCopy(int nSrcLen, LPCTSTR lpszSrcData);
	void	ConcatCopy(LPCTSTR lpszData);
	void	ConcatCopy(TCHAR ch);
	void	ConcatCopy(LPCTSTR lpszData1, LPCTSTR lpszData2);
	void	AllocString(int nLen);
	void	ReAllocString(int nLen);
};	

// Compare operations
bool operator==(const CCOMString& s1, const CCOMString& s2);
bool operator==(const CCOMString& s1, LPCTSTR s2);
bool operator==(LPCTSTR s1, const CCOMString& s2);
bool operator!=(const CCOMString& s1, const CCOMString& s2);
bool operator!=(const CCOMString& s1, LPCTSTR s2);
bool operator!=(LPCTSTR s1, const CCOMString& s2);
// Compare implementations
inline bool operator==(const CCOMString& s1, const CCOMString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const CCOMString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(LPCTSTR s1, const CCOMString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const CCOMString& s1, const CCOMString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const CCOMString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(LPCTSTR s1, const CCOMString& s2)
	{ return s2.Compare(s1) != 0; }

CCOMString toString(LPCTSTR pszCharSet, ...);
#endif // !defined(_COMSTR)
