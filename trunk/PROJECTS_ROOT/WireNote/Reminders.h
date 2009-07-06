// Reminders.h: interface for the CReminders class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMINDERS_H__1BAD9EDD_20F9_471C_9CA7_46EDA2A46848__INCLUDED_)
#define AFX_REMINDERS_H__1BAD9EDD_20F9_471C_9CA7_46EDA2A46848__INCLUDED_

#include "ItemModif.h"
class DLG_Reminder;
void GenerateNewId(CString& sRes);
class Reminder: public CItem
{
public:
	BOOL bInAction;
	BOOL bSkipIfMissed;//Пропускать ли напоминание если оно было пропущено
	BOOL bExplicitDeleteReminder;//Удалять ли в любом случае
	//Common
	COleDateTime LastTriggerTime;
	COleDateTime ClosestTriggerTime;
	BOOL bUndeletable;
	CStringArray ActionPars;
	CDWordArray ActionUse;
	//Day
	int dwDay;// Дата
	int bDayType;//0-точно, 1-каждый день, 2-опр. день месяца
	int dwWDay;//1-7, 1-воскресенье
	//Month
	int dwMon;//1-12
	int bMonType;//0-точно, 1-каждый месяц, 2-опр. месяц
	//Year
	int dwYear;
	int bYearType;//0-точно, 1-каждый год
	//Time
	CTime dwTime;// Время
	int bTimeType;// 0-точно, 1-интервал
	int dwTimeNum;// колич. единиц перед повтором
	int iTimeNumType;// единицы повтора: 0-мин 1-час 2-дни

	Reminder& operator=(Reminder& CopyObj)
	{
		SetColorCode(CopyObj.GetColorCode());
		SetIconNum(CopyObj.GetIconNum());
		m_sTitle=CopyObj.m_sTitle;
		m_sPath=CopyObj.m_sPath;
		lShared=CopyObj.lShared;
		m_iRevision=CopyObj.m_iRevision;
		sAuthor=CopyObj.sAuthor;
		//------------------
		dwDay=CopyObj.dwDay;
		bDayType=CopyObj.bDayType;
		dwWDay=CopyObj.dwWDay;
		dwMon=CopyObj.dwMon;
		bMonType=CopyObj.bMonType;
		dwYear=CopyObj.dwYear;
		bYearType=CopyObj.bYearType;
		dwTime=CopyObj.dwTime;
		bTimeType=CopyObj.bTimeType;
		dwTimeNum=CopyObj.dwTimeNum;
		iTimeNumType=CopyObj.iTimeNumType;
		bExplicitDeleteReminder=CopyObj.bExplicitDeleteReminder;
		for(int i=0;i<CopyObj.ActionUse.GetSize();i++){
			ActionUse[i]=CopyObj.ActionUse[i];
			ActionPars[i]=CopyObj.ActionPars[i];
		}
		return *this;
	}
	BOOL CreateRemDialog(BOOL bNew=FALSE);
	DLG_Reminder* dlg;
	BOOL SaveToFile(const char* szFile=0);
	CString GetItemFile();
	CString GetFileName(BOOL bTrash=FALSE);
	BOOL LoadFromFile(const char* szFileName);
	BOOL DelFile();
	BOOL Saveable();
	virtual void ActivateChanges(const char* szTitle=NULL, DWORD* pdwPrir=NULL, DWORD* pdwState=NULL);
	virtual void SetID(const char* szID);
	virtual CString GetItemDsc();
	CString GetTextContent();
	CString GetFindText(){return GetTitle();};
	void OnModify()
	{
		ClosestTriggerTime.SetStatus(COleDateTime::invalid);
	}
	void PerformReminderActions(BOOL bTest=FALSE);
	void OnReminderActivated();
	BOOL CalulateClosestTime(COleDateTime& CheckTimeMustBe);
	BOOL IsStrictReminder();
	Reminder(const char* szID=NULL);
	virtual ~Reminder();
	BOOL ShareItem(BOOL bNewSharingData, int iSectionNum);
};

class CReminders : public CObject 
{
public:
	void SetHourlyAsOptions();
	Reminder* GetHourTicker(BOOL bCreateIfNotPresent=TRUE);
	Reminder* GetRegReminder();
	void SetRegReminderParams(Reminder* rem);
	Reminder* GetReminder(const char* szID);
	Reminder* GetReminder(int ind);
	void OnModify();
	COleDateTime ClosestTime;
	int ClosestRemNum;
	int GetSize(){return m_aReminders.GetSize();};
	CArray<Reminder*,Reminder*> m_aReminders;
	CReminders();
	virtual ~CReminders();
	void Save();
	void LoadReminders(BOOL bShared=0);
	void Add(Reminder* rem);
	void Delete(const char* szID);
	COleDateTime GetClosestTriggerTime(int& iNum);
};
Reminder* AddReminder(int Minutes, const char* szTitle);
#endif // !defined(AFX_REMINDERS_H__1BAD9EDD_20F9_471C_9CA7_46EDA2A46848__INCLUDED_)
