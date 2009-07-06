/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Implementation of class "CMonthCalCtrlEx"
	  (month calendar control with holidays displayed in boldface)
	  
		$Log$
		
******************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*** Definition of class "CMonthCalCtrlEx" ***********************************/

const double CMonthCalCtrlEx::kDegRad	         = 0.0174532925199433;
const int    CMonthCalCtrlEx::HOURS_PER_DAY    = 24;
const int    CMonthCalCtrlEx::MINUTES_PER_HOUR = 60;
const int    CMonthCalCtrlEx::MINUTES_PER_DAY  =
MINUTES_PER_HOUR * HOURS_PER_DAY;

/*** Constructor *************************************************************/
CMonthCalCtrlEx::CMonthCalCtrlEx()
{
	pDelegates=0;
	memset(m_fixedHolidays, 0, sizeof m_fixedHolidays);
	m_pMDS            = 0;
	m_nMDSAllocated   = 0;
	m_nYearMonthStart = 0;
	m_nYearMonthEnd   = 0;
	memset(&m_stOldToolTipDate, 0, sizeof m_stOldToolTipDate);
}

/*** Destructor **************************************************************/
CMonthCalCtrlEx::~CMonthCalCtrlEx()
{
	ClearFixedDayList  (m_lstFixedHolidays);
	ClearFixedDayList  (m_lstFixedNamedDays);
	ClearMovableDayList(&m_lstMovableHolidays);
	ClearMovableDayList(&m_lstMovableNamedDays);
	delete[] m_pMDS;
	m_mapHolidayNames.RemoveAll();
}

/*** Public member functions *************************************************/

/*** Returns the name(s) of a given day **************************************/
CString CMonthCalCtrlEx::GetNameOfDay(COleDateTime& date)
{
	SYSTEMTIME st;
	
	if (date.GetAsSystemTime(st)) return GetNameOfDay(&st); else return _T("");
}

CString CMonthCalCtrlEx::GetNameOfDay(CTime& date)
{
	SYSTEMTIME st;
	
	if (date.GetAsSystemTime(st)) return GetNameOfDay(&st); else return _T("");
}

CString CMonthCalCtrlEx::GetNameOfDay(LPSYSTEMTIME pdate)
{
	int     nYearMonth    = pdate->wYear*100 + pdate->wMonth;
	int     nYearMonthDay = nYearMonth*100 + pdate->wDay;
	CString strName;
	
	if (nYearMonth < m_nYearMonthStart || nYearMonth > m_nYearMonthEnd)
		SetMonthDayStates(*pdate, 1, false);
	
	m_mapHolidayNames.Lookup(nYearMonthDay, strName);
	return strName;
}

/*** Read all holidays from a file *******************************************/
void CMonthCalCtrlEx::SetHolidaysFromFile(LPCTSTR pszFileName)
{
	CPrivateProfile prof(pszFileName);
	POSITION        pos;
	CString         strHolidayName;
	
	// read fixed holidays
	memset           (m_fixedHolidays, 0, sizeof m_fixedHolidays);
	ClearFixedDayList(m_lstFixedHolidays);
	for (pos = prof.GetStartPosition(_T("FixedHolidays")); pos;)
	{
		LPCTSTR pszMonthDay = prof.GetNext(pos);
		int     nMonthDay   = _ttoi(pszMonthDay);
		int     nMonth      = nMonthDay / 100;
		int     nDay        = nMonthDay % 100;
		
		if (nMonth >= 1 && nMonth <= 12 && nDay >= 1 && nDay <= 31)
		{
			m_fixedHolidays[nMonth-1] |= 1 << (nDay-1);
			
			strHolidayName = prof.GetString(_T("FixedHolidays"), pszMonthDay);
			if (!strHolidayName.IsEmpty())
			{
				FIXED_HOLIDAY* pfh = new FIXED_HOLIDAY(nDay, strHolidayName);
				
				m_lstFixedHolidays[nMonth-1].AddTail(pfh);
			}
		}
	}
	
	// read movable holidays
	ClearMovableDayList(&m_lstMovableHolidays);
	for (pos = prof.GetStartPosition(_T("MovableHolidays")); pos;)
	{
		CString strRule(prof.GetNext(pos));
		
		strRule.MakeUpper();    // rule is not case-sensitive!
		
		strHolidayName = prof.GetString(_T("MovableHolidays"), strRule);
		if (!strHolidayName.IsEmpty())
		{
			MOVABLE_HOLIDAY* pmh = new MOVABLE_HOLIDAY(strRule, strHolidayName);
			
			m_lstMovableHolidays.AddTail(pmh);
		}
	}
	
	// read fixed named days which are not holidays
	ClearFixedDayList(m_lstFixedNamedDays);
	for (pos = prof.GetStartPosition(_T("FixedNamedDays")); pos;)
	{
		LPCTSTR pszMonthDay = prof.GetNext(pos);
		int     nMonthDay   = _ttoi(pszMonthDay);
		int     nMonth      = nMonthDay / 100;
		int     nDay        = nMonthDay % 100;
		
		if (nMonth >= 1 && nMonth <= 12 && nDay >= 1 && nDay <= 31)
		{
			strHolidayName = prof.GetString(_T("FixedNamedDays"), pszMonthDay);
			if (!strHolidayName.IsEmpty())
			{
				FIXED_HOLIDAY* pfh = new FIXED_HOLIDAY(nDay, strHolidayName);
				
				m_lstFixedNamedDays[nMonth-1].AddTail(pfh);
			}
		}
	}
	
	// read movable named days which are not holidays
	ClearMovableDayList(&m_lstMovableNamedDays);
	for (pos = prof.GetStartPosition(_T("MovableNamedDays")); pos;)
	{
		CString strRule(prof.GetNext(pos));
		
		strRule.MakeUpper();    // key is not case-sensitive!
		
		strHolidayName = prof.GetString(_T("MovableNamedDays"), strRule);
		if (!strHolidayName.IsEmpty())
		{
			MOVABLE_HOLIDAY* pmh = new MOVABLE_HOLIDAY(strRule, strHolidayName);
			
			m_lstMovableNamedDays.AddTail(pmh);
		}
	}
}

/*** Protected member functions **********************************************/

/*** Calculate the holidays for the next displayed range of months ***********/
void CMonthCalCtrlEx::OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(pDelegates){
		pDelegates->OnGetdaystate(pNMHDR, pResult);
	}else{
		LPNMDAYSTATE pDayState = reinterpret_cast<LPNMDAYSTATE>(pNMHDR);
		SetMonthDayStates(pDayState->stStart, pDayState->cDayState, true);
		pDayState->prgDayState = m_pMDS;
	}
	*pResult = 0;
}

LRESULT CMonthCalCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_LBUTTONDBLCLK){
		GetParent()->PostMessage(WM_COMMAND,IDOK,0);
		return 0;
	}
	return 	CMonthCalCtrl::WindowProc(message, wParam, lParam);
}

BOOL CMonthCalCtrlEx::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_LBUTTONDBLCLK){
		GetParent()->PostMessage(WM_COMMAND,IDOK,0);
		return TRUE;
	}
	return 	CMonthCalCtrl::PreTranslateMessage(pMsg);
}

/*** The mouse cursor has moved **********************************************/
void CMonthCalCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Display holiday name in a tooltip
	MCHITTESTINFO mcHitTest = {sizeof MCHITTESTINFO};
	
	mcHitTest.pt = point;
	switch (HitTest(&mcHitTest))
	{
    case MCHT_CALENDARDATE:
    case MCHT_CALENDARDATENEXT:
    case MCHT_CALENDARDATEPREV:
		if (memcmp(&mcHitTest.st, &m_stOldToolTipDate, sizeof(SYSTEMTIME)))
		{
			m_tooltip.Activate(FALSE);
			
			CString strToolTip;
			if (m_mapHolidayNames.Lookup(mcHitTest.st.wYear * 10000 +
				mcHitTest.st.wMonth * 100 +
				mcHitTest.st.wDay, strToolTip))
			{
				m_tooltip.Activate     (TRUE);
				m_tooltip.UpdateTipText(strToolTip, this);
				
				MSG msg;
				msg.hwnd    = m_hWnd;
				msg.message = WM_MOUSEMOVE;
				m_tooltip.RelayEvent(&msg);
			}
			
			m_stOldToolTipDate = mcHitTest.st;
		}
		break;
		
    default:
		memset            (&m_stOldToolTipDate, 0, sizeof m_stOldToolTipDate);
		m_tooltip.Activate(FALSE);
		break;
	}
	
	CMonthCalCtrl::OnMouseMove(nFlags, point);
}

/*** Calculate the holidays for the range of months initially displayed ******/
void CMonthCalCtrlEx::PreSubclassWindow() 
{
	SYSTEMTIME stMinRange, stMaxRange;
	GetMonthRange(&stMinRange, &stMaxRange, GMR_DAYSTATE);
	
	int nMonths = (stMaxRange.wYear - stMinRange.wYear) * 12 +
		stMaxRange.wMonth - stMinRange.wMonth + 1;
	SetMonthDayStates(stMinRange, nMonths, true);
	SetDayState      (nMonths, m_pMDS);
	
	m_tooltip.Create        (this);
	m_tooltip.AddTool       (this, _T("Calendar Tooltip"));
	m_tooltip.SetMaxTipWidth(SHRT_MAX); // enable multiline tooltips
	
	CMonthCalCtrl::PreSubclassWindow();
}

/*** Private member functions ************************************************/

/*** Get the date of Advent for a given year *********************************/
COleDateTime CMonthCalCtrlEx::GetAdvent(int nYear) const
{
	COleDateTime ctChristmas(nYear, 12, 25, 0, 0, 0);
	int          nChristmasDayOfWeek = ctChristmas.GetDayOfWeek() - 1;
	
	if (nChristmasDayOfWeek == 0) nChristmasDayOfWeek = 7;
	
	return ctChristmas - COleDateTimeSpan(nChristmasDayOfWeek + 21, 0, 0, 0);
}

/*** Get the date of Easter for a given year *********************************/
COleDateTime CMonthCalCtrlEx::GetEaster(int nYear) const
{
	// The method used below has been given by Spencer Jones in his book
	// "General Astronomy" (pages 73-74 of the edition of 1922). It has been
	// published again in the "Journal of the British Astronomical Association",
	// Vol.88, page 91 (December 1977) where it is said that it was devised in
	// 1876 and appeared in the Butcher's "Ecclesiastical Calendar."
	int a = nYear % 19;
	int b = nYear / 100;
	int c = nYear % 100;
	int d = b / 4;
	int e = b % 4;
	int f = (b + 8) / 25;
	int g = (b - f + 1) / 3;
	int h = (19 * a + b - d - g + 15) % 30;
	int i = c / 4;
	int k = c % 4;
	int l = (32 + 2 * e + 2 * i - h - k) % 7;
	int m = (a + 11 * h + 22 * l) / 451;
	int n = h + l - 7 * m + 114;
	
	return COleDateTime(nYear, n / 31, n % 31 + 1, 0, 0, 0);
}

/*** Return the first day of a given season in a given year ******************/
int CMonthCalCtrlEx::GetEquinoxSolstice(int nYear, SEASON season)
{
	// result already calculated?
	int nDay;
	if (m_mapEquinoxesSolstices[season].Lookup(nYear, nDay)) return nDay;
	
	// Calculate equinoxes and solstices
	// (Meeus, Jean, "Astronomical Algorithms, 2nd ed." Willmann-Bell. Inc. 1998,
	//	pp. 177)
	int    i;             // common loop index
	double JDE0 = 0;		  // Julian Ephemeris Day
	double Y    = (nYear - 2000) / 1000.0;
	
	static const double coeff[4][5] =
	{
		{-0.00057, -0.00411,  0.05169, 365242.37404, 2451623.80984},
		{-0.00030,  0.00888,  0.00325, 365241.62603, 2451716.56767},
		{ 0.00078,  0.00337, -0.11575, 365242.01767, 2451810.21715},
		{ 0.00032, -0.00823, -0.06223, 365242.74049, 2451900.05952}
	};
	
	for (i = 0; i < 5; i++) JDE0 = coeff[season][i] + Y * JDE0 ;
	
	double T           = (JDE0 - 2451545) / 36525;    // Julian Centuries
	double W           = kDegRad * (35999.373 * T - 2.47);
	double DELTAlambda = 1 + 0.0334 * cos(W) + 0.0007 * cos(2*W);
	
	static const double c[][3] =
	{
		{485, kDegRad * 324.96, kDegRad *   1934.136},
		{203, kDegRad * 337.23, kDegRad *  32964.467},
		{199, kDegRad * 342.08, kDegRad *     20.186},
		{182, kDegRad *  27.85, kDegRad * 445267.112},
		{156, kDegRad *  73.14, kDegRad *  45036.886},
		{136, kDegRad * 171.52, kDegRad *  22518.443},
		{ 77, kDegRad * 222.54, kDegRad *  65928.934},
		{ 74, kDegRad * 296.72, kDegRad *   3034.906},
		{ 70, kDegRad * 243.58, kDegRad *   9037.513},
		{ 58, kDegRad * 119.81, kDegRad *  33718.147},
		{ 52, kDegRad * 297.17, kDegRad *    150.678},
		{ 50, kDegRad *  21.02, kDegRad *   2281.226},
		{ 45, kDegRad * 247.54, kDegRad *  29929.562},
		{ 44, kDegRad * 325.15, kDegRad *  31555.956},
		{ 29, kDegRad *  60.93, kDegRad *   4443.417},
		{ 18, kDegRad * 155.12, kDegRad *  67555.328},
		{ 17, kDegRad * 288.79, kDegRad *   4562.452},
		{ 16, kDegRad * 198.04, kDegRad *  62894.029},
		{ 14, kDegRad * 199.76, kDegRad *  31436.921},
		{ 12, kDegRad *  95.39, kDegRad *  14577.848},
		{ 12, kDegRad * 287.11, kDegRad *  31931.756},
		{ 12, kDegRad * 320.81, kDegRad *  34777.259},
		{  9, kDegRad * 227.73, kDegRad *   1222.114},
		{  8, kDegRad *  15.45, kDegRad *  16859.074}
	};
	
	double S = 0;
	
	for (i = 0; i < sizeof c / sizeof c[0]; i++)
		S += c[i][0] * cos(c[i][1] + c[i][2] * T);
	
	nDay = JulianDayToGregorianDate(JDE0 + 0.00001 * S / DELTAlambda).GetDay();
	return m_mapEquinoxesSolstices[season][nYear] = nDay;
}

/*** Get a movable holiday of a given year by rule ***************************/
COleDateTime CMonthCalCtrlEx::GetMovableHoliday(const CString& strRule,
                                                int nYear)
{
	COleDateTime ctMovableHoliday;
	
	if (strRule == _T("AU"))
		// First Day of Autumn
		ctMovableHoliday =
		COleDateTime(nYear, 9, GetEquinoxSolstice(nYear, AUTUMN), 0, 0, 0);
	else if (strRule[0] == _T('A'))
	{
		// relative to Advent
		COleDateTime ctAdvent(GetAdvent(nYear));
		int          nOffset = _ttoi((LPCTSTR)strRule + 1);
		
		if (nOffset < 0)
			ctMovableHoliday = ctAdvent - COleDateTimeSpan(-nOffset, 0, 0, 0);
		else
			ctMovableHoliday = ctAdvent + COleDateTimeSpan(nOffset, 0, 0, 0);
	}
	else if (strRule[0] == _T('E'))
	{
		// relative to Easter
		COleDateTime ctEaster(GetEaster(nYear));
		int          nOffset = _ttoi((LPCTSTR)strRule + 1);
		
		if (nOffset < 0)
			ctMovableHoliday = ctEaster - COleDateTimeSpan(-nOffset, 0, 0, 0);
		else
			ctMovableHoliday = ctEaster + COleDateTimeSpan(nOffset, 0, 0, 0);
	}
	else if (strRule == _T("SP"))
		// First Day of Spring
		ctMovableHoliday =
		COleDateTime(nYear, 3, GetEquinoxSolstice(nYear, SPRING), 0, 0, 0);
	else if (strRule == _T("SU"))
		// First Day of Summer
		ctMovableHoliday =
		COleDateTime(nYear, 6, GetEquinoxSolstice(nYear, SUMMER), 0, 0, 0);
	else if (strRule == _T("WI"))
		// First Day of Winter
		ctMovableHoliday =
		COleDateTime(nYear, 12, GetEquinoxSolstice(nYear, WINTER), 0, 0, 0);
	else
	{
		int nMonth     = 0;
		int nWeekday   = 0;
		int nThWeekday = 0;
		
		_stscanf(strRule, _T("%d,%d,%d"), &nMonth, &nWeekday, &nThWeekday);
		
		// n-th weekday of month?
		if (nMonth     >= 1 && nMonth     <= 12 &&
			nWeekday   >= 1 && nWeekday   <=  7 &&
			nThWeekday >= 1 && nThWeekday <=  5)
		{
			ctMovableHoliday = COleDateTime(nYear, nMonth, 1, 0, 0, 0);
			int nWeekday1st = ctMovableHoliday.GetDayOfWeek();
			
			if (nWeekday < nWeekday1st) nWeekday += 7;
			
			ctMovableHoliday +=
				COleDateTimeSpan(nWeekday - nWeekday1st + (nThWeekday-1)*7, 0, 0,
				0);
			
			// correct wrap-around into next month
			if (ctMovableHoliday.GetMonth() != nMonth)
				ctMovableHoliday -= COleDateTimeSpan(7, 0, 0, 0);
		}
		else
		{
			int nMonthDay = 0;
			nWeekday      = 0;
			nThWeekday    = 0;
			
			_stscanf(strRule, _T("%d%d,%d"), &nMonthDay, &nWeekday, &nThWeekday);
			
			nMonth   = nMonthDay / 100;
			int nDay = nMonthDay % 100;
			
			// n-th weekday before or after a fixed day?
			if (nMonth     >=  1 && nMonth     <= 12  &&
				nDay       >=  1 && nDay       <= 31  &&
				(nWeekday  >=  1 && nWeekday   <=  7  ||
				nWeekday  >= -7 && nWeekday   <= -1) &&
				nThWeekday >=  1 && nThWeekday <= 53)
			{
				ctMovableHoliday = COleDateTime(nYear, nMonth, nDay, 0, 0, 0);
				
				if (nWeekday > 0)
				{
					nDay = nWeekday - ctMovableHoliday.GetDayOfWeek();
					if (nDay <= 0) nDay += 7;
					ctMovableHoliday += COleDateTimeSpan(nDay + (nThWeekday-1)*7, 0, 0,
						0);
				}
				else
				{
					nDay = nWeekday + ctMovableHoliday.GetDayOfWeek();
					if (nDay <= 0) nDay += 7;
					ctMovableHoliday -= COleDateTimeSpan(nDay + (nThWeekday-1)*7, 0, 0,
						0);
				}
			}
		}
	}
	
	return ctMovableHoliday;
}

/*** Convert Julian day number to Gregorian date *****************************/
COleDateTime CMonthCalCtrlEx::JulianDayToGregorianDate(double JD) const
{
	// Convert Julian Day to Gregorian month, day and year
	// (Meeus, Jean, "Astronomical Algorithms, 2nd ed." Willmann-Bell. Inc. 1998,
	//	pp. 63)
	JD += 0.5;
	
	double Z = floor(JD);
	double F = JD - Z;
	double A;
	if (Z >= 2291161)
	{
		double alpha = floor((Z - 1867216.25)/36524.25);
		
		A = Z + 1 + alpha - floor(alpha/4);
	}
	else
		A = Z;
	
	double B = A + 1524;
	double C = floor((B - 122.1)/365.25);
	double D = floor(365.25 * C);
	double E = floor((B - D)/30.6001);
	
	double     d = B - D - floor(30.6001 * E) + F;
	SYSTEMTIME st;
	
	st.wDay = static_cast<WORD>(d);
	
	// calculate hour, minutes, and seconds
	d               -= floor(d);
	st.wHour         = static_cast<WORD>(d * HOURS_PER_DAY);
	st.wMinute       =
		static_cast<WORD>(
		static_cast<UINT>(d * MINUTES_PER_DAY) % MINUTES_PER_HOUR);
	st.wSecond       = 0;
	st.wMilliseconds = 0;
	
	st.wMonth = static_cast<WORD>(E);
	if (st.wMonth < 14)
		st.wMonth--;
	else if (st.wMonth == 14 || st.wMonth == 15)
		st.wMonth -= 13;
	else
	{
		ASSERT(false);
		return COleDateTime();    // error
	}
	
	if (st.wMonth > 2)
		st.wYear = static_cast<WORD>(C - 4716);
	else if (st.wMonth == 1 || st.wMonth == 2)
		st.wYear = static_cast<WORD>(C - 4715);
	else
	{
		ASSERT(false);
		return COleDateTime();    // error
	}
	
	// convert universal time to local time
	FILETIME ft, lft;
	if (SystemTimeToFileTime(&st, &ft) && FileTimeToLocalFileTime(&ft, &lft))
		FileTimeToSystemTime(&lft, &st);
	
	return COleDateTime(st.wYear, st.wMonth, st.wDay, 0, 0, 0);
}

/*** Calculate the fixed holidays for a given month span *********************/
void CMonthCalCtrlEx::SetFixedHolidays(LPMONTHDAYSTATE pMDS, int nYear, 
                                       int nMinMonth, int nMaxMonth)
{
	for (int nMonth = nMinMonth; nMonth <= nMaxMonth; nMonth++)
	{
		// set Sundays
		COleDateTime ct1stOfMonth(nYear, nMonth, 1, 0, 0, 0);
		int          n1stSunday = (7 - (ct1stOfMonth.GetDayOfWeek()-1)) % 7;
		
		for (MONTHDAYSTATE mask = 1 << n1stSunday;
		n1stSunday < 31;
		n1stSunday += 7, mask <<= 7) pMDS[nMonth-nMinMonth] |= mask;
		
		// set fixed holidays
		pMDS[nMonth-nMinMonth] |= m_fixedHolidays[nMonth-1];
		
		// get holiday names
		for (POSITION pos = m_lstFixedHolidays[nMonth-1].GetHeadPosition(); pos;)
		{
			FIXED_HOLIDAY* pfh           = m_lstFixedHolidays[nMonth-1].GetNext(pos);
			int            nYearMonthDay = nYear*10000 + nMonth*100 + pfh->m_nDay;
			CString        strName;
			
			if (m_mapHolidayNames.Lookup(nYearMonthDay, strName))
				strName += _T("\r\n");
			m_mapHolidayNames[nYearMonthDay] = strName + pfh->m_strName;
		}
	}
}

/*** Calculate the fixed named days for a given month span *******************/
void CMonthCalCtrlEx::SetFixedNamedDays(int nYear, int nMinMonth,
                                        int nMaxMonth)
{
	for (int nMonth = nMinMonth; nMonth <= nMaxMonth; nMonth++)
	{
		// get day names
		for (POSITION pos = m_lstFixedNamedDays[nMonth-1].GetHeadPosition(); pos;)
		{
			FIXED_HOLIDAY* pfh           =
				m_lstFixedNamedDays[nMonth-1].GetNext(pos);
			int            nYearMonthDay = nYear*10000 + nMonth*100 + pfh->m_nDay;
			CString        strName;
			
			if (m_mapHolidayNames.Lookup(nYearMonthDay, strName))
				strName += _T("\r\n");
			m_mapHolidayNames[nYearMonthDay] = strName + pfh->m_strName;
		}
	}
}

/*** Calculate the movable holidays for a given month span *******************/
void CMonthCalCtrlEx::SetMovableHolidays(LPMONTHDAYSTATE pMDS, int nYear,
                                         int nMinMonth, int nMaxMonth)
{
	for (POSITION pos = m_lstMovableHolidays.GetHeadPosition(); pos;)
	{
		MOVABLE_HOLIDAY* pmh              = m_lstMovableHolidays.GetNext(pos);
		COleDateTime     ctMovableHoliday =
			GetMovableHoliday(pmh->m_strRule, nYear);
		
		if (ctMovableHoliday.GetStatus() != COleDateTime::null)
		{
			int nMonth = ctMovableHoliday.GetMonth();
			
			if (ctMovableHoliday.GetYear() == nYear &&
				nMonth >= nMinMonth && nMonth <= nMaxMonth)
			{
				pMDS[nMonth-nMinMonth] |= 1 << (ctMovableHoliday.GetDay()-1);
				
				int     nYearMonthDay =
					nYear*10000 + nMonth*100 + ctMovableHoliday.GetDay();
				CString strName;
				
				if (m_mapHolidayNames.Lookup(nYearMonthDay, strName))
					strName += _T("\r\n");
				m_mapHolidayNames[nYearMonthDay] = strName + pmh->m_strName;
			}
		}
	}
}

/*** Calculate the movable holidays for a given month span *******************/
void CMonthCalCtrlEx::SetMovableNamedDays(int nYear, int nMinMonth,
                                          int nMaxMonth)
{
	for (POSITION pos = m_lstMovableNamedDays.GetHeadPosition(); pos;)
	{
		MOVABLE_HOLIDAY* pmh              = m_lstMovableNamedDays.GetNext(pos);
		COleDateTime     ctMovableHoliday =
			GetMovableHoliday(pmh->m_strRule, nYear);
		
		if (ctMovableHoliday.GetStatus() != COleDateTime::null)
		{
			int nMonth = ctMovableHoliday.GetMonth();
			
			if (ctMovableHoliday.GetYear() == nYear &&
				nMonth >= nMinMonth && nMonth <= nMaxMonth)
			{
				int     nYearMonthDay =
					nYear*10000 + nMonth*100 + ctMovableHoliday.GetDay();
				CString strName;
				
				if (m_mapHolidayNames.Lookup(nYearMonthDay, strName))
					strName += _T("\r\n");
				m_mapHolidayNames[nYearMonthDay] = strName + pmh->m_strName;
			}
		}
	}
}

/*** Prepare the holidays of a given range for display ***********************/
void CMonthCalCtrlEx::SetMonthDayStates(SYSTEMTIME& stStart, int nMonths,
                                        bool bClearNames)
{
	if (bClearNames) m_mapHolidayNames.RemoveAll();
	
	if (nMonths > 0)
	{
		if (nMonths > m_nMDSAllocated)
		{
			delete[] m_pMDS;
			m_pMDS          = new MONTHDAYSTATE[nMonths];
			m_nMDSAllocated = nMonths;
		}
		memset(m_pMDS, 0, nMonths * sizeof(MONTHDAYSTATE));
		
		LPMONTHDAYSTATE pMDS      = m_pMDS;
		int             nMaxMonth = 0;
		int             nYear;
		for (nYear = stStart.wYear; nMonths > 0; nYear++)
		{
			int nMinMonth = nYear == stStart.wYear ? stStart.wMonth : 1;
			nMaxMonth = nMinMonth + nMonths - 1;
			
			if (nMaxMonth > 12) nMaxMonth = 12;
			
			SetFixedHolidays   (pMDS, nYear, nMinMonth, nMaxMonth);
			SetMovableHolidays (pMDS, nYear, nMinMonth, nMaxMonth);
			SetFixedNamedDays  (nYear, nMinMonth, nMaxMonth);
			SetMovableNamedDays(nYear, nMinMonth, nMaxMonth);
			
			int nMonthCount = nMaxMonth - nMinMonth + 1;
			nMonths -= nMonthCount;
			pMDS    += nMonthCount;
		}
		m_nYearMonthStart = stStart.wYear * 100 + stStart.wMonth;
		m_nYearMonthEnd   = (nYear-1) * 100 + nMaxMonth;
	}
	else
	{
		m_nYearMonthStart = 0;
		m_nYearMonthEnd   = 0;
	}
}

/*** Table of message handlers ***********************************************/
BEGIN_MESSAGE_MAP(CMonthCalCtrlEx, CMonthCalCtrl)
//{{AFX_MSG_MAP(CMonthCalCtrlEx)
ON_NOTIFY_REFLECT(MCN_GETDAYSTATE, OnGetdaystate)
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
