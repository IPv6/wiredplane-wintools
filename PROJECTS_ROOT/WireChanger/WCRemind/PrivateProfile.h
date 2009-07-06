/******************************************************************************

$Author$
  
$Modtime$
$Revision$

Description: Interface of class "CPrivateProfile"
             (INI file handler)

$Log$

******************************************************************************/

/*** Declaration of class "CPrivateProfile" **********************************/
class CPrivateProfile: public CObject  
{
  public:
  CPrivateProfile(LPCTSTR pszFileName): m_strFileName(pszFileName) {}

  virtual ~CPrivateProfile();

  int GetInt(LPCTSTR pszSectionName, LPCTSTR pszKeyName, int nDefault = 0)
    const
  {
    return GetPrivateProfileInt(pszSectionName, pszKeyName, nDefault,
                                m_strFileName);
  }

  LPCTSTR  GetNext         (POSITION& position) const;
  POSITION GetStartPosition(LPCTSTR pszSectionName = 0);
  CString  GetString       (LPCTSTR pszSectionName, LPCTSTR pszKeyName,
                            LPCTSTR pszDefault = _T("")) const;

  private:
  CString                                    m_strFileName;
  CMap<CString, CString, POSITION, POSITION> m_mapSections;
};

// Hash-function for "CMap<CString, CString, POSITION, POSITION>"
template<> inline UINT AFXAPI HashKey(CString strKey)
{
  return HashKey((LPCTSTR)strKey);
}
