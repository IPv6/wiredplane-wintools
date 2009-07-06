// TaskHistory.cpp: implementation of the CTaskHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////
// Task class
//////////////////////////////////////////////
Task::Task():CActiItem(1)
{
	dtClosingTime.SetStatus(COleDateTime::invalid);
	SetState(FALSE);// Изначально задача не сделана
	pCorDialog=NULL;
	SetTitle("ToDo");
	bIsActive=0;
	sHistory="";
	sDsc="";
	Attach();
}

Task::Task(const char* szName, const char* szTaskDsc, BOOL bHaveNote, BOOL bIncludeInReports):CActiItem(1)
{
	dtClosingTime.SetStatus(COleDateTime::invalid);
	SetState(FALSE);// Изначально задача не сделана
	bIncludeInRep=bIncludeInReports;
	pCorDialog=NULL;
	SetColorCode(objSettings.TaskNoteSettings.GetColorCode());
	SetTitle(szName);
	sDsc=szTaskDsc;
	bIsActive=0;
}

Task::~Task()
{
	if(pCorDialog && ::IsWindow(pCorDialog->m_hWnd)){
		DestroyWindow(pCorDialog->GetSafeHwnd());
	}
	pCorDialog=NULL;
}

CString Task::GetItemFile()
{
	return getFileFullName(objSettings.sTaskDirectory,TRUE)+GetID()+"."TODO_EXTENSION;
};

CString Task::GetFileName(BOOL bTrash)
{
	if(bTrash){
		return getFileFullName(objSettings.sTrashDirectory,TRUE)+GetID()+"."TODO_EXTENSION;
	}else{
		return GetItemFile();
	}
}

BOOL Task::ShareItem(BOOL bNewSharingData, int iSectionNum)
{
	CSmartLock SL(&objSettings.csSharing,TRUE,IDC_CURSORSAVE);
	SL.SetMinWait(500);
	if(pCorDialog){
		pCorDialog->CommitData();
		pCorDialog->Finalize(IDOK);
	}
	CString sFile=GetValidFileName(this,DEF_BB_TDIR,TODO_EXTENSION,iSectionNum);
	if(sFile==""){
		return FALSE;
	}
	SetAuthor(objSettings.sDefAuthor);
	if(!SaveToFile(sFile)){
		Alert(_l("Failed to publish item at Billboard\nBillboard is temporarily down"),_l("Billboard error"),9000);
		return FALSE;
	}
	if(!IsShared()){
		SetPath(BB_PATH);//TODO_PATH"/"
		CItem* pItem=CItem::GetItemByID(BB_PATH);
		if(pItem){
			pMainDialogWindow->DragItem(this,pItem);
		}
	}
	SetShared(1,sFile);
	SmartWarning(_l("Item successfully published at Billboard\nAfter some minutes it will be delivered to others\nTo change this item at billboard, you have to republish it\n"),_l("Item published"),&objSettings.bSWarning_ShareOK);
	return TRUE;
}

BOOL Task::SaveToFile(const char* szFile)
{
	if(!Saveable()){
		return FALSE;
	}
	CString sFile=szFile?szFile:GetFileName();
	CDataXMLSaver xmlFile(sFile,"TODO",FALSE);
	CString sItem=ConvertToString();
	xmlFile.putValue("item",sItem,XMLNoConversion);
	xmlFile.putValue("include-rep",bIncludeInRep);
	CString sHistoryToSave=sHistory;
	COleDateTimeSpan dt=SpendedTime();
	if(objSettings.bTraceToDoHist==1 && dt.GetStatus()==COleDateTimeSpan::valid){
		sHistoryToSave=Format("dayspan:[%f]",double(dt));
	}
	xmlFile.putValue("done-at",dtClosingTime);
	xmlFile.putValue("history",sHistoryToSave,XMLWithStartCR|XMLTrimSpaces);
	xmlFile.putValue("app-pinned",sPinnedToApp, XMLWithStartCR|XMLWithEndCR);
	xmlFile.putValue("dsc",sDsc);
	if(objSettings.lFBCrypt){
		xmlFile.CryptBody();
	}
	return xmlFile.SaveDataIntoSource();
}

BOOL Task::LoadFromFile(const char* szFileName)
{
	if(!szFileName){
		return 0;
	}
	CDataXMLSaver xmlFile(szFileName,"TODO",TRUE);
	xmlFile.UncryptBody();
	CString sItem;
	xmlFile.getValue("item",sItem);
	ConvertFromString(sItem,szFileName);
	if(isItemCorrupted()){
		return FALSE;
	}
	xmlFile.getValue("include-rep",bIncludeInRep);
	xmlFile.getValue("done-at",dtClosingTime);
	xmlFile.getValue("history",sHistory,"",XMLTrimSpaces);
	CString sPinnedToAppTmp;
	xmlFile.getValue("app-pinned",sPinnedToAppTmp, "", XMLWithStartCR);
	SetApplicationsList(sPinnedToAppTmp);
	xmlFile.getValue("dsc",sDsc);
	if(strstr(szFileName,DEF_BB_TDIR)!=0){
		lShared=1;
	}
	if(Saveable()){
		return TRUE;
	}
	return FALSE;
}

BOOL Task::DelFile()
{
	CString sFile=GetFileName();
	if(objSettings.bDelToTrash && Saveable()){
		CString sBakFile=GetFileName(TRUE);
		SetFileAttributes(sBakFile,FILE_ATTRIBUTE_NORMAL);
		DeleteFile(sBakFile);
		PartlyEmptyTrash();
		return MoveFile(sFile,sBakFile);
	}else{
		return DeleteFile(sFile);
	}
}

BOOL Task::Saveable()
{
	return TRUE;
}

CString Task::GetTextContent()
{
	return sDsc;
}

CString Task::GetItemDsc()
{
	CString sOut="";
	if(GetPriority()==2){
		sOut="Important: ";
	}
	if(GetPriority()==-2){
		sOut=_l("Not important")+": ";
	}
	sOut+=GetTitle();
	if(!objSettings.bHideDateFld){
		sOut+=Format(", %s: %s",_l("Modified"),GetDateInCurFormat(GetCrTime()));
	}
	if(GetState()==1){
		sOut+=", "+_l("Done");
		if(dtClosingTime.GetStatus()==COleDateTime::valid){
			sOut+=dtClosingTime.Format(" (%d.%m.%Y %H:%M:%S)");
		}
	}
	if(IsShared()){
		sOut+=Format("\n %s %s",_l("Published at billboard by"), GetAuthor());
	}
	if(GetRevision()){
		sOut+=Format("\n %s: %i",_l("Revision"), GetRevision());
	}
	COleDateTimeSpan dt=SpendedTime();
	if(dt.GetStatus()==COleDateTimeSpan::valid){
		sOut+="\n "+_l("Time spend");
		sOut+=dt.Format(": %H:%M:%S");
	}
	return sOut;
};

void Task::ActivateChanges(const char* szTitle, DWORD* pdwPrir, DWORD* pdwState)
{
	if(szTitle){
		SetTitle(szTitle);
	}
	if(pdwPrir){
		SetPriority(*pdwPrir);
	}
	if(GetState()!=-1 && pdwState){
		if(*pdwState){
			objSettings.m_Tasks.MarkAsDone(GetID());
		}else{
			objSettings.m_Tasks.MarkAsNotDone(GetID());
		}
	}
	ItemContentChanged();
}

void Task::SetActive(BOOL bActive)
{
	bIsActive=bActive;
	ItemChanged();
	ItemContentChanged();
}

COleDateTimeSpan Task::SpendedTime()
{
	COleDateTimeSpan dt;
	dt.SetStatus(COleDateTimeSpan::invalid);
	if(!objSettings.bTraceToDoHist){
		return dt;
	}
	dt.SetDateTimeSpan(0,0,0,0);
	COleDateTime dtIn,dtOut;
	COleDateTimeSpan dtSpan;
	int iFrom=0;
	while(iFrom!=-1 && iFrom!=sHistory.GetLength()){
		CString sPeriod=CDataXMLSaver::GetInstringPart("dayspan:[","]",sHistory,iFrom);
		if(sPeriod!=""){
			dt+=atof(sPeriod);
		}
	}
	iFrom=0;
	while(iFrom!=-1 && iFrom!=sHistory.GetLength()){
		CString sInTime=CDataXMLSaver::GetInstringPart("in:[","]",sHistory,iFrom);
		CString sOutTime=CDataXMLSaver::GetInstringPart("out:[","]",sHistory,iFrom);
		dtIn.ParseDateTime(sInTime);
		dtOut.ParseDateTime(sOutTime);
		if(dtIn.GetStatus()==COleDateTime::valid && dtOut.GetStatus()==COleDateTime::valid ){
			dt+=dtOut-dtIn;
		}
	}
	// Если период открыт, то считаем до тек. момента
	if(dtIn.GetStatus()==COleDateTime::valid && dtOut.GetStatus()!=COleDateTime::valid ){
		dt+=COleDateTime::GetCurrentTime()-dtIn;
	}
	if(dt.GetTotalSeconds()==0){// Слишком маленькое время
		dt.SetStatus(COleDateTimeSpan::invalid);
	}
	return dt;
}

void Task::SetColorCode(long iCode)
{
	CItem::SetColorCode(iCode);
	if(pCorDialog){
		pCorDialog->SetColorCode(iCode);
	}
}

//////////////////////////////////////////////
// End Task class
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTaskHistory::CTaskHistory()
{
	bRelevantChanges=TRUE;
	sUserActivatedTaskID="";
}

CTaskHistory::~CTaskHistory()
{
	for(int i=0;i<m_aTasks.GetSize();i++){
		Task* pTask=m_aTasks.GetAt(i);
		delete pTask;
	}
	m_aTasks.RemoveAll();
}

void CTaskHistory::SaveTasks()
{
	CreateDirIfNotExist(getFileFullName(objSettings.sTaskDirectory,TRUE));
	for(int i=0;i<m_aTasks.GetSize();i++){
		Task* pTask=m_aTasks.GetAt(i);
		if(!pTask){
			continue;
		}
		pTask->SaveToFile();
	}
}

void CTaskHistory::LoadTasks(BOOL bShared)
{
	CString sDirectoryPath=getFileFullName(objSettings.sTaskDirectory,TRUE);
	CFileInfoArray dir;
	if(bShared){
		for(int i=0;i<objSettings.aSubsBB.GetSize();i++){
			sDirectoryPath=GetFBFolder(DEF_BB_TDIR,objSettings.aSubsBB[i]);
			dir.AddDir(sDirectoryPath,"*."TODO_EXTENSION,TRUE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTDESCENDING,FALSE);
		}
	}else{
		dir.AddDir(sDirectoryPath,"*."TODO_EXTENSION,TRUE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTDESCENDING,FALSE);
	}
	BOOL bSomeTDNotLoaded=0;
	for (int i=0;i<dir.GetSize();i++){
 		if(bShared && !CheckSharedItem(dir[i])){
			continue;
		}
		CString sItemPath=dir[i].GetFilePath();
		Task* pTask=new Task();
		BOOL bRes=pTask->LoadFromFile(sItemPath);
		Task* pOldTask=LoadTaskByID(pTask->GetID());
		if(!bRes || (pOldTask && !bShared)){
			bSomeTDNotLoaded=TRUE;
			delete pTask;
			continue;
		}
		if(pOldTask && bShared && pOldTask->pCorDialog!=0){
			Alert(_l("Shared item")+" '"+pOldTask->GetTitle()+"' "+_l("is out-of-date")+"\n"+_l("Newer version is available at Billboard")+"\n"+_l("New revision of this item will be taken next time"),_l("Warning: new item available"));
			delete pTask;
			continue;
		}
		if(bShared){
			CString sIDD=pTask->GetID();
			if(pOldTask){
				if(pOldTask->GetRevision()>pTask->GetRevision()){
					// У нас более новая версия
					AddBBExcl(GetPathPart(sItemPath,0,0,1,1));
					delete pTask;
					continue;
				}else{
					*pOldTask=*pTask;
					delete pTask;
					pTask=pOldTask;
					objSettings.lFBStat_UpdT++;
				}
			}else{
				objSettings.lFBStat_NewT++;
				pTask->SetPath(BB_PATH);//TODO_PATH"/"
				m_aTasks.Add(pTask);
			}
			NotifyAboutSharedItem(sIDD,sItemPath);
		}else if(pOldTask==NULL){
			m_aTasks.Add(pTask);
		}
	}
	if(bSomeTDNotLoaded){
		SmartWarning(_l("Some ToDos are not loaded: they are corrupted or something"),_l("WireNote: Warning"),0);
	}
}

Task* CTaskHistory::LoadTaskByID(CString sID)
{
	if(sID==""){
		return NULL;
	}
	for(int i=0;i<m_aTasks.GetSize();i++){
		Task* pCurTask=m_aTasks[i];
		if(strcmp(pCurTask->GetID(),sID)==0){
			return pCurTask;
		}
	}
	return NULL;
}

int CTaskHistory::AddTask(Task* pNewTask)
{
	bRelevantChanges=TRUE;
	const int i=m_aTasks.Add(pNewTask);
	pNewTask->Attach();
	pNewTask->SaveToFile();
	return i;
}

int CTaskHistory::GetTaskByID(const char* szID)
{
	if(szID==NULL || (*szID)==0){
		return -1;
	}
	for(int i=0;i<m_aTasks.GetSize();i++){
		Task* pCurTask=m_aTasks[i];
		if(strcmp(pCurTask->GetID(),szID)==0){
			return i;
		}
	}
	return -1;
}

Task* CTaskHistory::GetTask(const char* szID)
{
	int iIndex=GetTaskByID(szID);
	return GetTask(iIndex);
}

Task* CTaskHistory::GetTask(int iIndex)
{
	if(iIndex>=0 && iIndex<m_aTasks.GetSize()){
		Task* pTask=m_aTasks.GetAt(iIndex);
		return pTask;
	}
	return NULL;
}

CString CTaskHistory::GetTaskID(int iIndex)
{
	Task* pTask=GetTask(iIndex);
	if(pTask){
		return pTask->GetID();
	}
	return "";
}

void CTaskHistory::MarkAsNotDone(const char* szID)
{
	bRelevantChanges=TRUE;
	Task* pTask=GetTask(szID);
	if(pTask){
		pTask->SetState(FALSE);
		pTask->dtClosingTime.SetStatus(COleDateTime::invalid);
	}
	return;
}

void CTaskHistory::MarkAsDone(const char* szID)
{
	bRelevantChanges=TRUE;
	Task* pTask=GetTask(szID);
	if(pTask){
		AsyncPlaySound(SOUND_ONDONE);
		TaskLeave(pTask->GetID());
		pTask->SetState(TRUE);
		pTask->dtClosingTime=COleDateTime::GetCurrentTime();
	}
	return;
}


BOOL CTaskHistory::DeleteTask(const char* szID)
{
	bRelevantChanges=TRUE;
	int iIndex=GetTaskByID(szID);
	if(iIndex>=0 && iIndex<m_aTasks.GetSize()){
		Task* pTask=m_aTasks.GetAt(iIndex);
		if(pTask->IsActive()){
			TaskLeave(pTask->GetID());
		}
		pTask->DelFile();
		delete pTask;
		m_aTasks.RemoveAt(iIndex);
		return TRUE;
	}
	return FALSE;
}

int CTaskHistory::GetSize()
{
	return m_aTasks.GetSize();
}

void CTaskHistory::TaskLeave(const char* szID)
{
	// Выходим из задачи только если она действительно активна
	Task* pTask=GetTask(szID);
	if(!pTask || !pTask->IsActive()){
		return;
	}
	if(strcmp(pTask->GetID(),sUserActivatedTaskID)==0){
		// Выключилась задача, активированная пользователем
		sUserActivatedTaskID="";
	}
	if(objSettings.bTraceToDoHist>0){
		COleDateTime dtNow=COleDateTime::GetCurrentTime();
		CString sString=dtNow.Format("\nout:["LOGTIMEFRMT"]");
		pTask->sHistory+=sString;
		bRelevantChanges=TRUE;
		pTask->ItemChanged();
	}
	pTask->SetActive(FALSE);
	SetTODOTrayTooltip();
	pTask->SaveToFile();
	pTask->SetModify();
}

void CTaskHistory::TaskEnter(const char* szID, BOOL bFromUserInput)
{
	Task* pTask=GetTask(szID);
	if(!pTask || pTask->IsActive()){
		return;
	}
	if(!objSettings.bActivateDoneToDo && pTask->IsDone()){
		return;
	}
	if(bFromUserInput){
		if(strcmp(sUserActivatedTaskID,szID)!=0){
			TaskLeave(sUserActivatedTaskID);
			sUserActivatedTaskID=szID;
		}
	}
	AsyncPlaySound(SOUND_ONACTIVATION);
	if(objSettings.bTraceToDoHist>0){
		COleDateTime dtNow=COleDateTime::GetCurrentTime();
		CString sString=dtNow.Format("\n in:["LOGTIMEFRMT"]");
		pTask->sHistory+=sString;
		bRelevantChanges=TRUE;
	}
	pTask->SetActive(TRUE);
	SetTODOTrayTooltip();
	pTask->SaveToFile();
	pTask->SetModify();
}

void CTaskHistory::Analyze(COleDateTime &StartDate, const char* szPath)
{
	CString sTargetPath=szPath;
	if(sTargetPath.Right(1)=="/"){
		sTargetPath=sTargetPath.Left(sTargetPath.GetLength()-1);
	}
	CString sOutReport, sTemp;
	COleDateTime dtTemp;
	sOutReport+=_l("Starting report")+"\n";
	dtTemp=COleDateTime::GetCurrentTime();
	sTemp=dtTemp.Format("["LOGTIMEFRMT"]");
 	sOutReport+=_l("Current time");
	sOutReport+=Format(": %s\n",sTemp);
	sTemp=StartDate.Format("["LOGTIMEFRMT"]");
	sOutReport+=_l("Report time");
	sOutReport+=Format(": %s\n\n",sTemp);
	int iTaskNum=1;
	// Cycling...
	for(int i=0;i<m_aTasks.GetSize();i++){
		BOOL bPath=TRUE;
		if(strlen(sTargetPath)>0 && strstr(m_aTasks[i]->GetPath(),sTargetPath)==NULL){
			bPath=FALSE;
		}
		if(m_aTasks[i]->bIncludeInRep && ((m_aTasks[i]->GetCrTime())>=StartDate) && bPath){
			CString sTime;
			COleDateTimeSpan dt=m_aTasks[i]->SpendedTime();
			if(dt.GetStatus()==COleDateTimeSpan::valid && dt.GetSeconds()>5){
				sTime="\n"+_l("Time spend");
				sTime+=dt.Format(": %H:%M:%S");
			}
			CString sDoneAt;
			if(m_aTasks[i]->GetState()==1 && m_aTasks[i]->dtClosingTime.GetStatus()==COleDateTime::valid){
				sDoneAt="\n"+_l("Done at");
				sDoneAt+=m_aTasks[i]->dtClosingTime.Format(": %d.%m.%Y %H:%M:%S");
			}
			sOutReport+=Format("#%i. %s %s%s%s\n\n",
				iTaskNum++,
				m_aTasks[i]->GetCrTime().Format("["LOGTIMEFRMT"]"),
				m_aTasks[i]->GetTitle(),
				sDoneAt,
				sTime);
		}
	};
	sOutReport+="\n";
	sOutReport+=_l("Report end");
	CNoteSettings* NewNote=new CNoteSettings(objSettings.NoteSettings);
	NewNote->sText=sOutReport;
	OpenNote(NewNote);
}

void CTaskHistory::LeaveAllTasks()
{
	for(int i=0;i<m_aTasks.GetSize();i++){
		Task* pTask=m_aTasks[i];
		if(pTask && pTask->IsActive()){
			TaskLeave(pTask->GetID());
		}
	}
}

void CTaskHistory::SetTODOTrayTooltip()
{
	objSettings.sTrayTooltip=objSettings.sMainWindowTitle;
	int iActiveTasks=0;
	CString sTaskDsc="";
	Task* pTask=GetTask(sUserActivatedTaskID);
	if(!pTask || !pTask->IsActive()){
		for(int i=0;i<m_aTasks.GetSize();i++){
			Task* pTask=m_aTasks[i];
			if(pTask && pTask->IsActive()){
				iActiveTasks++;
				sTaskDsc+="\n- ";
				sTaskDsc+=pTask->GetTitle(25);
			}
		}
	}else{
		sTaskDsc+=pTask->GetTitle();
		iActiveTasks++;
	}
	if(iActiveTasks>0){
		objSettings.sTrayTooltip=_l("ToDo")+": "+sTaskDsc;
	}
	if(pMainDialogWindow){
		pMainDialogWindow->SetTrayTooltip();
	}
}