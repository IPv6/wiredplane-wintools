// SpellChecker.cpp: implementation of the CSpellChecker class.
//
//////////////////////////////////////////////////////////////////////

#include "SpellChecker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpellChecker::CSpellChecker(const char* szAffPath, const char* szDicPath) : MySpell(szAffPath, szDicPath)
{

}

CSpellChecker::~CSpellChecker()
{

}

bool CSpellChecker::IsValid()
{
	return MySpell::isvalid();
}

void CSpellChecker::Release()
{
	delete this;
}

bool CSpellChecker::CheckSpelling(const char* szWord)
{
	return (spell(szWord) > 0);
}

bool CSpellChecker::CheckSpelling(const char* szWord, char**& pSuggestions, int& nNumSuggestions)
{
	bool bResult = CheckSpelling(szWord);

	if (!bResult)
		nNumSuggestions = suggest(&pSuggestions, szWord);

	return bResult;
}

void CSpellChecker::FreeSuggestions(char**& pSuggestions)
{
	delete[] (pSuggestions);
	pSuggestions = NULL;
}
