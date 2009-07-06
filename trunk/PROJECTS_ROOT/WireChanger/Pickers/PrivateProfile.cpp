/******************************************************************************

$Author$
  
$Modtime$
$Revision$

Description: Implementation of class "CPrivateProfile"
             (INI file handler)

$Log$

******************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*** Definition of class "CPrivateProfile" ***********************************/

/*** Destructor **************************************************************/
CPrivateProfile::~CPrivateProfile()
{
  for (POSITION pos = m_mapSections.GetStartPosition(); pos;)
  {
    CString  strKey;
    POSITION posSection;

    m_mapSections.GetNextAssoc(pos, strKey, posSection);
    m_mapSections.RemoveKey   (strKey);
    free                      (posSection);
  }
}

/*** Public member functions *************************************************/

/*** Get next section or key name ********************************************/
LPCTSTR CPrivateProfile::GetNext(POSITION& position) const
{
  if (position == 0) return _T("");

  LPCTSTR psz = reinterpret_cast<LPCTSTR>(position);

  position =
    reinterpret_cast<POSITION>(
      reinterpret_cast<LPTSTR>(position) + _tcslen(psz)+1);
  if (*reinterpret_cast<LPCTSTR>(position) == _T('\0')) position = 0;

  return psz;
}

/*** Position to first key name in a given section ***************************/
POSITION CPrivateProfile::GetStartPosition(LPCTSTR pszSectionName)
{
  CString strUnifiedSectionName;
  
  if (pszSectionName)
  {
    strUnifiedSectionName = pszSectionName;
    strUnifiedSectionName.MakeUpper();
  }

  POSITION pos;
  if (m_mapSections.Lookup(strUnifiedSectionName, pos))
  {
    m_mapSections.RemoveKey(strUnifiedSectionName);
    free                   (pos);
  }

  LPTSTR pszKeyNames;
  DWORD  dwRead;
  for (DWORD dwSize = 1024-1;; dwSize = (dwSize+1)*2-1)
  {
    pszKeyNames = static_cast<LPTSTR>(malloc(dwSize * sizeof(TCHAR)));

    dwRead = GetPrivateProfileString(pszSectionName, 0, _T(""), pszKeyNames,
                                     dwSize, m_strFileName);
    if (dwRead < dwSize - 2) break;
    free(pszKeyNames);   // buffer wasn't large enough!
  }

  if (*pszKeyNames != _T('\0'))
  {
    pszKeyNames =
      static_cast<LPTSTR>(realloc(pszKeyNames, (dwRead+2) * sizeof(TCHAR)));
    return m_mapSections[strUnifiedSectionName] =
             reinterpret_cast<POSITION>(pszKeyNames);
  }
  else
  {
    free(pszKeyNames);
    return 0;
  }
}

/*** Retrieve an integer value from a specified section and key **************/
/*** Retrieve a string value from a specified section and key ****************/
CString CPrivateProfile::GetString(LPCTSTR pszSectionName, LPCTSTR pszKeyName,
                                   LPCTSTR pszDefault) const
{
  CString str;

  for (DWORD dwSize = 1024-1;; dwSize = (dwSize+1)*2-1)
  {
    DWORD dwRead = GetPrivateProfileString(pszSectionName, pszKeyName,
                                           pszDefault, str.GetBuffer(dwSize),
                                           dwSize, m_strFileName);
    str.ReleaseBuffer();
    if (dwRead < dwSize - 1) break;
    // buffer wasn't large enough!
  }

  return str;
}
