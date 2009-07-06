// CCOMStringW.h : header file
//
// CCOMStringW Header
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


class CCOMStringW  
{
public: 
	CCOMStringW();
	CCOMStringW(CCOMStringW& szString);
	CCOMStringW(const WCHAR* pszString);
	CCOMStringW(WCHAR ch, int nRepeat);
	virtual ~CCOMStringW();
	
	// Assignment Operations
	const CCOMStringW& operator=(CCOMStringW& strSrc);
	const CCOMStringW& operator=(const WCHAR* lpsz);
	operator LPCWSTR() const	{ return m_pszString; }
	LPCWSTR c_str(){ return m_pszString; }
	WCHAR*	GetString()			{return m_pszString;}
	BSTR	AllocSysString();
	
	// Concatenation
	const CCOMStringW& operator+=(CCOMStringW& strSrc);
	const CCOMStringW& operator+=(const WCHAR* lpsz);
	const CCOMStringW& operator+=(WCHAR ch);
	friend CCOMStringW operator+(CCOMStringW& strSrc1, CCOMStringW& strSrc2);
	friend CCOMStringW operator+(CCOMStringW& strSrc, const WCHAR* lpsz);
	friend CCOMStringW operator+(const WCHAR* lpsz, CCOMStringW& strSrc);

	// Accessors for the String as an Array
	int		GetLength() const;
	int		length() const{return GetLength();};
	bool	IsEmpty() const;
	void	Empty();
	WCHAR	GetAt(int nIndex);
	void	SetAt(int nIndex, WCHAR ch);
	WCHAR	operator[] (int nIndex);

	// Conversions
	void	MakeUpper();
	void	MakeLower();
	void	MakeReverse();
	void	TrimLeft();
	void	TrimRight();

	// Searching
	int		Find(WCHAR ch) const;
	int		Find(WCHAR ch, int nStart) const;
	int		Find(const WCHAR* lpszSub);
	int		Find(const WCHAR* lpszSub, int nStart);
	int		FindOneOf(const WCHAR* lpszCharSet) const;
	int		ReverseFind(WCHAR ch, int nStart) const;
	int		ReverseFind(WCHAR ch) const;
	// Extraction
	CCOMStringW Mid(int nFirst, int nCount=-1) ;
	CCOMStringW Left(int nCount) ;
	CCOMStringW Right(int nCount) ;
	CCOMStringW SpanIncluding(const WCHAR* lpszCharSet);
	CCOMStringW SpanExcluding(const WCHAR* lpszCharSet);

	// Comparison
	int Compare(CCOMStringW& str) const;
	int Compare(const WCHAR* lpsz) const;
	int CompareNoCase(CCOMStringW& str) const;
	int CompareNoCase(const WCHAR* lpsz) const;
	int Collate(CCOMStringW& str) const;
	int Collate(const WCHAR* lpsz) const;

	// Formatting
	void Format(const WCHAR* pszCharSet, ...);

	// Replacing
	int Replace(WCHAR chOld, WCHAR chNew);
	int Replace(const WCHAR* lpszOld, const WCHAR* lpszNew);
	operator LPCWSTR(){return GetString();};	
protected:
	WCHAR*	m_pszString;
	void	StringCopy(CCOMStringW& str, int nLen, int nIndex, int nExtra) const;
	void	StringCopy(int nSrcLen, const WCHAR* lpszSrcData);
	void	ConcatCopy(const WCHAR* lpszData);
	void	ConcatCopy(WCHAR ch);
	void	ConcatCopy(const WCHAR* lpszData1, const WCHAR* lpszData2);
	void	AllocString(int nLen);
	void	ReAllocString(int nLen);
};	

// Compare operations
bool operator==(const CCOMStringW& s1, const CCOMStringW& s2);
bool operator==(const CCOMStringW& s1, const WCHAR* s2);
bool operator==(const WCHAR* s1, const CCOMStringW& s2);
bool operator!=(const CCOMStringW& s1, const CCOMStringW& s2);
bool operator!=(const CCOMStringW& s1, const WCHAR* s2);
bool operator!=(const WCHAR* s1, const CCOMStringW& s2);
// Compare implementations
inline bool operator==(const CCOMStringW& s1, const CCOMStringW& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const CCOMStringW& s1, const WCHAR* s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const WCHAR* s1, const CCOMStringW& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const CCOMStringW& s1, const CCOMStringW& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const CCOMStringW& s1, const WCHAR* s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const WCHAR* s1, const CCOMStringW& s2)
	{ return s2.Compare(s1) != 0; }

CCOMStringW toString(const WCHAR* pszCharSet, ...);
#endif // !defined(_COMSTR)
