/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Interface of class "CMonthCalCtrlEx"
	  (month calendar control with holidays displayed in boldface)
	  
		$Log$
		
******************************************************************************/
class CDelegates
{
public:
	virtual afx_msg void OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult)=0;
};
/*** Declaration of class "CMonthCalCtrlEx" **********************************/
class CMonthCalCtrlEx: public CMonthCalCtrl
{
	// Construction
public:
	CDelegates* pDelegates;
	CMonthCalCtrlEx();
	virtual ~CMonthCalCtrlEx();
	CString GetNameOfDay       (COleDateTime& date);
	CString GetNameOfDay       (CTime& date);
	CString GetNameOfDay       (LPSYSTEMTIME pdate);
	void    SetHolidaysFromFile(LPCTSTR pszFileName);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonthCalCtrlEx)
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CMonthCalCtrlEx)
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual afx_msg void OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
		
private:
	enum SEASON {UNKNOWN = -1, SPRING, SUMMER, AUTUMN, WINTER};
	
	struct FIXED_HOLIDAY
	{
		FIXED_HOLIDAY(int nDay, LPCTSTR pszName): m_nDay(nDay), m_strName(pszName)
		{}
		
		int     m_nDay;
		CString m_strName;
	};
	
	struct MOVABLE_HOLIDAY
	{
		MOVABLE_HOLIDAY(LPCTSTR pszRule, LPCTSTR pszName): m_strRule(pszRule),
			m_strName(pszName)
		{}
		
		CString m_strRule;
		CString m_strName;
	};
	
	void ClearFixedDayList(CList<FIXED_HOLIDAY*, FIXED_HOLIDAY*>* plstFD)
	{
		for (int nMonth = 0; nMonth < 12; nMonth++)
			while (!plstFD[nMonth].IsEmpty())
				delete plstFD[nMonth].RemoveTail();
	}
	
	void ClearMovableDayList(CList<MOVABLE_HOLIDAY*, MOVABLE_HOLIDAY*>* plstMD)
	{
		while (!plstMD->IsEmpty()) delete plstMD->RemoveTail();
	}
	
	COleDateTime GetAdvent               (int nYear) const;
	COleDateTime GetEaster               (int nYear) const;
	int          GetEquinoxSolstice      (int nYear, SEASON season);
	CString      GetHolidayName          (SYSTEMTIME& st);
	COleDateTime GetMovableHoliday       (const CString& strRule, int nYear);
	COleDateTime JulianDayToGregorianDate(double JD) const;
	void         SetMonthDayStates       (SYSTEMTIME& stStart, int nMonths,
		bool bClearNames);
	void         SetFixedHolidays        (LPMONTHDAYSTATE pMDS, int nYear,
		int nMinMonth, int nMaxMonth);
	void         SetFixedNamedDays       (int nYear, int nMinMonth,
		int nMaxMonth);
	void         SetMovableHolidays      (LPMONTHDAYSTATE pMDS, int nYear,
		int nMinMonth, int nMaxMonth);
	void         SetMovableNamedDays     (int nYear, int nMinMonth,
		int nMaxMonth);
	void         SetSundays              (LPMONTHDAYSTATE pMDS, int nYear,
		int nMinMonth, int nMaxMonth);
	
	MONTHDAYSTATE                             m_fixedHolidays[12];
	CList<FIXED_HOLIDAY*, FIXED_HOLIDAY*>     m_lstFixedHolidays[12];
	CList<FIXED_HOLIDAY*, FIXED_HOLIDAY*>     m_lstFixedNamedDays[12];
	CList<MOVABLE_HOLIDAY*, MOVABLE_HOLIDAY*> m_lstMovableHolidays;
	CList<MOVABLE_HOLIDAY*, MOVABLE_HOLIDAY*> m_lstMovableNamedDays;
	CMap<int, int, CString, CString&>         m_mapHolidayNames;
	CMap<int, int, int, int>                  m_mapEquinoxesSolstices[4];
	LPMONTHDAYSTATE                           m_pMDS;
	int                                       m_nMDSAllocated;
	int                                       m_nYearMonthStart;
	int                                       m_nYearMonthEnd;
	CToolTipCtrl                              m_tooltip;
	SYSTEMTIME                                m_stOldToolTipDate;
	
	// constants
	static const double kDegRad;
	static const int    HOURS_PER_DAY;
	static const int    MINUTES_PER_HOUR;
	static const int    MINUTES_PER_DAY;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
