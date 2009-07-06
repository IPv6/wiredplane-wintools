// TaskHistory.h: interface for the CTaskHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKHISTORY_H__207E7F8F_F543_4552_8523_C4A50F531A49__INCLUDED_)
#define AFX_TASKHISTORY_H__207E7F8F_F543_4552_8523_C4A50F531A49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DLG_NewTask.h"
#include "ItemModif.h"

class Task : public CActiItem
{
public:
	virtual void SetColorCode(long iCode);
	CDLG_Newtask* pCorDialog;
	BOOL bIsActive;
	BOOL bIncludeInRep;
	CString sDsc;
	CString sHistory;
	COleDateTime dtClosingTime;
	//
	Task();
	Task(const char* szName, const char* szTaskDsc, BOOL bHaveNote=FALSE, BOOL bIncludeInRep=TRUE);
	virtual void ActivateChanges(const char* szTitle=NULL, DWORD* pdwPrir=NULL, DWORD* pdwState=NULL);
	virtual CString GetItemDsc();
	CString GetTextContent();
	CString GetFindText(){return GetTitle()+"|"+sDsc;};
	virtual ~Task();
	BOOL IsDone(){return bState;};
	BOOL SaveToFile(const char* szFile=0);
	CString GetItemFile();
	CString GetFileName(BOOL bTrash=FALSE);
	BOOL LoadFromFile(const char* szFileName);
	BOOL DelFile();
	BOOL Saveable();
	void SetActive(BOOL bActive);
	BOOL IsActive(){return bIsActive;};
	COleDateTimeSpan SpendedTime();
	BOOL ShareItem(BOOL bNewSharingData, int iSectionNum);
	Task& operator=(Task& CopyObj)
	{
		SetColorCode(CopyObj.GetColorCode());
		SetIconNum(CopyObj.GetIconNum());
		lShared=CopyObj.lShared;
		m_iRevision=CopyObj.m_iRevision;
		sAuthor=CopyObj.sAuthor;
		m_sTitle=CopyObj.m_sTitle;
		m_sPath=CopyObj.m_sPath;
		bIsActive=CopyObj.bIsActive;
		bIncludeInRep=CopyObj.bIncludeInRep;
		sDsc=CopyObj.sDsc;
		sHistory=CopyObj.sHistory;
		dtClosingTime=CopyObj.dtClosingTime;
		return *this;
	};
};

class CTaskHistory : public CObject  
{
	BOOL bRelevantChanges;
	CString sUserActivatedTaskID;
public:
	CArray<Task*,Task*> m_aTasks;
public:
	int AddTask(Task* pNewTask);
	void Analyze(COleDateTime &StartDate, const char* szPath);
	void SetTODOTrayTooltip();
	void TaskEnter(const char* szID, BOOL bFromUserInput=FALSE);
	void TaskLeave(const char* szID);
	void LeaveAllTasks();
	BOOL DeleteTask(const char* szID);
	void MarkAsDone(const char* szID);
	void MarkAsNotDone(const char* szID);
	//
	void SaveTasks();
	Task* LoadTaskByID(CString sID);
	void LoadTasks(BOOL bShared=0);
	//
	int GetSize();
	int GetTaskByID(const char* szID);
	CString GetTaskID(int iIndex);
	Task* GetTask(const char* szID);
	Task* GetTask(int iIndex);
	CTaskHistory();
	virtual ~CTaskHistory();
};

#endif // !defined(AFX_TASKHISTORY_H__207E7F8F_F543_4552_8523_C4A50F531A49__INCLUDED_)
