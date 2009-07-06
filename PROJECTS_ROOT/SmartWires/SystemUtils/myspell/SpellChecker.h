// SpellChecker.h: interface for the CSpellChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLCHECKER_H__74596014_29E0_4289_9BC7_28A7996FA37B__INCLUDED_)
#define AFX_SPELLCHECKER_H__74596014_29E0_4289_9BC7_28A7996FA37B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MySpell.hxx"

class CSpellChecker : protected MySpell
{
public:
	CSpellChecker(const char* szAffPath, const char* szDicPath);
	virtual ~CSpellChecker();

    void Release(); // releases the interface
    bool CheckSpelling(const char* szWord);
    bool CheckSpelling(const char* szWord, char**& pSuggestions, int& nNumSuggestions);
    void FreeSuggestions(char**& pSuggestions);

	bool IsValid();

};

#endif // !defined(AFX_SPELLCHECKER_H__74596014_29E0_4289_9BC7_28A7996FA37B__INCLUDED_)
