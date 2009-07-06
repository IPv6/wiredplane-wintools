// Reminders.cpp: implementation of the CReminders class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define REGREMIND_ID	"REGIST_REMIND"
#define HOURLYTICK_ID	"HOURLY_TICKER"
Reminder::Reminder(const char* szID):CItem(3)
{
	for(int i=0; i<DEFAULT_REMACTCOUNT; i++){
		ActionPars.Add("");
		ActionUse.Add(0);
	};
	SetID(szID);
	bUndeletable=0;
	SetState(1);
	SetColorCode(objSettings.iRemdStyle);
	ActionUse[0]=objSettings.bRemSoundByDef;
	ActionPars[0]=objSettings.sRemFileWav;
	ActionUse[1]=FALSE;
	ActionUse[5]=objSettings.bRemHideToTrayDef;
	ActionUse[11]=1;
	bInAction=FALSE;
	bTimeType=bYearType=bMonType=bDayType=0;
	LastTriggerTime=CreationTime=COleDateTime::GetCurrentTime();
	dwDay=CreationTime.GetDay();
	dwWDay=CreationTime.GetDayOfWeek();
	dwMon=CreationTime.GetMonth();
	dwYear=CreationTime.GetYear();
	dwTime=CTime(2000,1,1,CreationTime.GetHour(),CreationTime.GetMinute(),0);
	dwTimeNum=0;
	iTimeNumType=0;
	m_sTitle="";
	bExplicitDeleteReminder=0;
	ClosestTriggerTime.SetStatus(COleDateTime::invalid);
	LastTriggerTime.SetStatus(COleDateTime::invalid);
	dlg=NULL;
}

Reminder::~Reminder()
{
	BOOL bRes=TRUE;
	DWORD a=0;
	if(dlg){
		dlg->reminder=NULL;
		if(::IsWindow(dlg->m_hWnd)){
			bRes=DestroyWindow(dlg->GetSafeHwnd());
			a=GetLastError();
		}
	}
	dlg=NULL;
};

BOOL Reminder::CreateRemDialog(BOOL bNew)
{
	if(dlg==NULL){
		dlg=new DLG_Reminder();
		dlg->bThisIsNewReminder=bNew;
		dlg->reminder=this;
		dlg->Create(DLG_Reminder::IDD,NULL);
	}else{
		SwitchToWindow(dlg->GetSafeHwnd());
	}
	return TRUE;
}

void Reminder::SetID(const char* szID)
{
	if(szID && strlen(szID)>0){
		if(strcmp(szID,REGREMIND_ID)==0){
			CItem::SetIconNum(REG_ICONS_OFFSET);
		}
		if(strcmp(szID,HOURLYTICK_ID)==0){
			CItem::SetIconNum(SCLOCK_ICONS_OFFSET);
		}
	}
	CItem::SetID(szID);
}

CString Reminder::GetItemFile()
{
	return getFileFullName(objSettings.sRemdDirectory,TRUE)+GetID()+"."REMD_EXTENSION;
}

CString Reminder::GetFileName(BOOL bTrash)
{
	if(bTrash){
		return getFileFullName(objSettings.sTrashDirectory,TRUE)+GetID()+"."REMD_EXTENSION;
	}
	return GetItemFile();
}

BOOL Reminder::SaveToFile(const char* szFile)
{
	if(!Saveable()){
		return FALSE;
	}
	CString sFile=szFile?szFile:GetFileName();
	CDataXMLSaver xmlFile(sFile,"REMINDER",FALSE);
	CString sItem=ConvertToString();
	xmlFile.putValue("item",sItem,XMLNoConversion);
	xmlFile.putValue("undeletable",bUndeletable);
	xmlFile.putValue("day",dwDay);
	xmlFile.putValue("wday",dwWDay);
	xmlFile.putValue("day-type",bDayType);
	xmlFile.putValue("mon",dwMon);

	xmlFile.putValue("mon-type",bMonType);
	xmlFile.putValue("year",dwYear);
	xmlFile.putValue("year-type",bYearType);
	xmlFile.putValue("time",(DWORD)dwTime.GetTime());
	xmlFile.putValue("time-num",dwTimeNum);
	xmlFile.putValue("time-type",bTimeType);
	xmlFile.putValue("time-num-type",iTimeNumType);
	xmlFile.putValue("del-exp",bExplicitDeleteReminder);
	if(LastTriggerTime.GetStatus()==COleDateTime::valid){
		xmlFile.putValue("last-triggered",LastTriggerTime);
	}
	for(int j=0;j<ActionUse.GetSize();j++){
		xmlFile.putValue(Format("action-use-%i",j),ActionUse[j]);
		xmlFile.putValue(Format("action-par-%i",j),ActionPars[j]);
	}
	if(objSettings.lFBCrypt){
		xmlFile.CryptBody();
	}
	return xmlFile.SaveDataIntoSource();
}

BOOL Reminder::ShareItem(BOOL bNewSharingData, int iSectionNum)
{
	CSmartLock SL(&objSettings.csSharing,TRUE,IDC_CURSORSAVE);
	SL.SetMinWait(500);
	if(dlg){
		if(!dlg->ApplySettedValues()){
			return FALSE;
		}
		dlg->ExitRemWindow(IDOK);
	}
	CString sFile=GetValidFileName(this,DEF_BB_RDIR,REMD_EXTENSION,iSectionNum);
	if(sFile==""){
		return FALSE;
	}
	SetAuthor(objSettings.sDefAuthor);
	if(!SaveToFile(sFile)){
		Alert(_l("Failed to publish item at Billboard\nBillboard is temporarily down"),_l("Billboard error"),9000);
		return FALSE;
	}
	if(!IsShared()){
		SetPath(BB_PATH);//REMD_PATH"/"
		CItem* pItem=CItem::GetItemByID(BB_PATH);
		if(pItem){
			pMainDialogWindow->DragItem(this,pItem);
		}
	}
	SetShared(1,sFile);
	SmartWarning(_l("Item successfully published at Billboard\nAfter some minutes it will be delivered to others\nTo change this item at billboard, you have to republish it\n"),_l("Item published"),&objSettings.bSWarning_ShareOK);
	return TRUE;
}

BOOL Reminder::LoadFromFile(const char* szFileName)
{
	if(!szFileName){
		return 0;
	}
	CDataXMLSaver xmlFile(szFileName,"REMINDER",TRUE);
	xmlFile.UncryptBody();
	CString sItem;
	xmlFile.getValue("item",sItem);
	ConvertFromString(sItem,szFileName);
	if(isItemCorrupted()){
		return FALSE;
	}
	xmlFile.getValue("undeletable",bUndeletable);
	xmlFile.getValue("day",dwDay);
	xmlFile.getValue("wday",dwWDay);
	xmlFile.getValue("day-type",bDayType);
	xmlFile.getValue("mon",dwMon);

	xmlFile.getValue("mon-type",bMonType);
	xmlFile.getValue("year",dwYear);
	xmlFile.getValue("year-type",bYearType);
	DWORD dwTimeInDWORD=0;
	xmlFile.getValue("time",dwTimeInDWORD);
	dwTime=CTime((time_t)dwTimeInDWORD);
	xmlFile.getValue("time-num",dwTimeNum);
	xmlFile.getValue("time-type",bTimeType);
	xmlFile.getValue("time-num-type",iTimeNumType);
	xmlFile.getValue("del-exp",bExplicitDeleteReminder);
	COleDateTime dtInvalid;
	dtInvalid.SetStatus(COleDateTime::invalid);
	xmlFile.getValue("last-triggered",LastTriggerTime,dtInvalid);
	for(int j=0;j<ActionUse.GetSize();j++){
		xmlFile.getValue(Format("action-use-%i",j),ActionUse[j]);
		xmlFile.getValue(Format("action-par-%i",j),ActionPars[j]);
	}
	if(strstr(szFileName,DEF_BB_RDIR)!=0){
		lShared=1;
	}
	if(Saveable()){
		return TRUE;
	}
	return FALSE;
}

BOOL Reminder::DelFile()
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

BOOL Reminder::Saveable()
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReminders::CReminders()
{
	OnModify();
}

CReminders::~CReminders()
{
	for(int i=0;i<m_aReminders.GetSize();i++){
		Reminder* rem=m_aReminders[i];
		if(rem){
			delete m_aReminders[i];
			m_aReminders[i]=NULL;
		}
	}
}

void CReminders::OnModify()
{
	ClosestTime.SetStatus(COleDateTime::invalid);
	ClosestRemNum=-1;
}

void CReminders::Add(Reminder* rem)
{
	m_aReminders.Add(rem);
	rem->Attach();
	OnModify();
	rem->SaveToFile();
}

void CReminders::Delete(const char* szID)
{
	OnModify();
	for(int i=0;i<m_aReminders.GetSize();i++){
		if(strcmp(m_aReminders[i]->GetID(),szID)==0 && m_aReminders[i]->bUndeletable==0){
			m_aReminders[i]->DelFile();
			delete m_aReminders[i];
			m_aReminders.RemoveAt(i);
			return;
		}
	}
}

Reminder* CReminders::GetReminder(const char* szID)
{
	for(int i=0;i<m_aReminders.GetSize();i++){
		if(strcmp(m_aReminders[i]->GetID(),szID)==0){
			return m_aReminders[i];
		}
	}
	return NULL;
}

Reminder* CReminders::GetReminder(int ind)
{
	if(ind<0 || ind>=m_aReminders.GetSize()){
		return NULL;
	}
	return m_aReminders[ind];
}

void CReminders::SetRegReminderParams(Reminder* rem)
{
	rem->SetState(1);
	rem->bUndeletable=TRUE;
	rem->SetTitle(_l(UNREG_FUNC_TITLE));
	rem->dwTime=CTime(2002,1,30,0,0,0);
	rem->bDayType=rem->bMonType=rem->bYearType=rem->bTimeType=1;
	rem->dwTimeNum=6;
	rem->iTimeNumType=1;
	rem->ActionUse[1]=1;
	rem->ActionPars[1]=DEFAULT_REGNOTICE;
	rem->ActionUse[5]=0;
}

Reminder* CReminders::GetRegReminder()
{
	BOOL bNeedToAdd=FALSE;
	Reminder* rem=GetReminder(REGREMIND_ID);
	if(objSettings.iLikStatus==2 && rem!=NULL){
		rem->bUndeletable=FALSE;
		Delete(REGREMIND_ID);
		rem=NULL;
	}
	if(rem==NULL){
		if(objSettings.iLikStatus<2){
			rem=new Reminder();
			bNeedToAdd=TRUE;
		}else{
			return 0;
		}
	}else{
		SetRegReminderParams(rem);
		return rem;
	}
	rem->SetID(REGREMIND_ID);
	rem->SetPath(REMS_PATH"/"REMD_PATH);
	SetRegReminderParams(rem);
	if(bNeedToAdd){
		objSettings.m_Reminders.Add(rem);
	}
	return rem;
}

void CReminders::SetHourlyAsOptions()
{
	Reminder* rem=GetHourTicker(objSettings.dwStrickingClockActive);
	if(rem){
		rem->ActionPars[0]=objSettings.sHourlyWav+(objSettings.bHourlyByHCount?"?alarm=h12&sleep=1000":"");
		rem->SetState(objSettings.dwStrickingClockActive);
	}
}

Reminder* CReminders::GetHourTicker(BOOL bCreateIfNotPresent)
{
	BOOL bNeedToAdd=FALSE;
	Reminder* rem=GetReminder(HOURLYTICK_ID);
	if(rem==NULL){
		if(bCreateIfNotPresent){
			rem=new Reminder();
			bNeedToAdd=TRUE;
		}else{
			return 0;
		}
	}else{
		return rem;
	}
	rem->SetID("HOURLY_TICKER");
	rem->SetPath(REMS_PATH"/"REMD_PATH);
	rem->bUndeletable=0;
	rem->SetTitle(_l("Striking clock"));
	rem->dwTime=CTime(2002,1,30,0,0,0);
	rem->bDayType=rem->bMonType=rem->bYearType=rem->bTimeType=1;
	rem->dwTimeNum=1;
	rem->iTimeNumType=1;
	rem->ActionUse[0]=1;
	rem->ActionPars[0]=objSettings.sHourlyWav+(objSettings.bHourlyByHCount?"?alarm=h12&sleep=1000":"");
	rem->ActionUse[5]=0;
	if(bNeedToAdd){
		objSettings.m_Reminders.Add(rem);
	}
	return rem;
}

void CReminders::LoadReminders(BOOL bShared)
{
	CString sDirectoryPath=getFileFullName(objSettings.sRemdDirectory,TRUE);
	CFileInfoArray dir;
	if(bShared){
		for(int i=0;i<objSettings.aSubsBB.GetSize();i++){
			sDirectoryPath=GetFBFolder(DEF_BB_RDIR,objSettings.aSubsBB[i]);
			dir.AddDir(sDirectoryPath,"*."REMD_EXTENSION,TRUE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTDESCENDING,FALSE);
		}
	}else{
		dir.AddDir(sDirectoryPath,"*."REMD_EXTENSION,TRUE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTDESCENDING,FALSE);
	}
	BOOL bChancged=0;
	for (int i=0;i<dir.GetSize();i++){
		if(bShared && !CheckSharedItem(dir[i])){
			continue;
		}
		CString sItemPath=dir[i].GetFilePath();
		Reminder* rem=new Reminder();
		BOOL bRes=rem->LoadFromFile(sItemPath);
		Reminder* pOldRemd=objSettings.m_Reminders.GetReminder(rem->GetID());
		if(!bRes || (pOldRemd && !bShared)){
			delete rem;
			continue;
		}
		if(pOldRemd && bShared && pOldRemd->dlg!=0){
			Alert(_l("Shared item")+" '"+pOldRemd->GetTitle()+"' "+_l("is out-of-date")+"\n"+_l("Newer version is available at Billboard")+"\n"+_l("New revision of this item will be taken next time"),_l("Billboard: new item available"));
			delete rem;
			continue;
		}
		if(bShared){
			rem->ClosestTriggerTime.SetStatus(COleDateTime::invalid);
			rem->LastTriggerTime.SetStatus(COleDateTime::invalid);
		}
		if(bShared){
			CString sIDD=rem->GetID();
			if(pOldRemd){
				rem->SetRevision(atol(CDataXMLSaver::GetInstringPart("[","]",sItemPath)));
				if(pOldRemd->GetRevision()>rem->GetRevision()){
					AddBBExcl(GetPathPart(sItemPath,0,0,1,1));
					delete rem;
					continue;
				}else{
					*pOldRemd=*rem;
					delete rem;
					rem=pOldRemd;
					bChancged=1;
					objSettings.lFBStat_UpdR++;
				}
			}else{
				objSettings.lFBStat_NewR++;
				rem->SetPath(BB_PATH);//REMD_PATH"/"
				bChancged=1;
				objSettings.m_Reminders.Add(rem);
			}
			NotifyAboutSharedItem(sIDD,sItemPath);
		}else{
			bChancged=1;
			objSettings.m_Reminders.Add(rem);
		}
	}
	if(bChancged){
		OnModify();
	}
	if(objSettings.iLikStatus==1){
		Reminder* rem=GetRegReminder();
		if(rem){
			rem->PerformReminderActions();
		}
	}
}

void CReminders::Save()
{
	CreateDirIfNotExist(getFileFullName(objSettings.sRemdDirectory,TRUE));
	for(int i=0;i<GetSize();i++){
		m_aReminders[i]->SaveToFile();
	}
}

COleDateTime CReminders::GetClosestTriggerTime(int& iNum)
{
	if(ClosestTime.GetStatus()!=COleDateTime::invalid){
		iNum=ClosestRemNum;
		return ClosestTime;
	}
	iNum=ClosestRemNum=-1;
	COleDateTime res,tmp;
	res.SetStatus(COleDateTime::invalid);
	for(int i=0;i<m_aReminders.GetSize();i++){
		if(m_aReminders[i]->GetState()==0){
			continue;
		}
		if(m_aReminders[i]->CalulateClosestTime(tmp)!=TRUE){
			continue;
		}
		//int i1=res.GetDay();int i2=res.GetMonth();int i3=res.GetYear();int i4=res.GetHour();int i5=res.GetMinute();
		//int i1a=tmp.GetDay();int i2a=tmp.GetMonth();int i3a=tmp.GetYear();int i4a=tmp.GetHour();int i5a=tmp.GetMinute();
		BOOL bIsResValid=res.GetStatus()==COleDateTime::valid;
		BOOL bIsTmpLessRes=(!bIsResValid || (bIsResValid && tmp<res));
		BOOL bSkipIfMissed=!m_aReminders[i]->ActionUse[11];
		BOOL bTimeWasMissed=tmp<objSettings.dStartTime;
		if(bTimeWasMissed && bSkipIfMissed){
			continue;
		}
		if(bTimeWasMissed || bIsTmpLessRes){
			ClosestRemNum=i;
			res=tmp;
		}
	}
	iNum=ClosestRemNum;
	ClosestTime=res;
	if(objSettings.lRemindFromSleep && pMainDialogWindow){
		FILETIME fTime,ft0;
		memset(&ft0,0,sizeof(FILETIME));
		memset(&fTime,0,sizeof(FILETIME));
		SYSTEMTIME tSysTime;
		memset(&tSysTime,0,sizeof(tSysTime));
		ClosestTime.GetAsSystemTime(tSysTime);
		SystemTimeToFileTime(&tSysTime,&ft0);
		LocalFileTimeToFileTime(&ft0,&fTime);
		LARGE_INTEGER* liDueTime=new LARGE_INTEGER;
		liDueTime->HighPart = fTime.dwHighDateTime;
		liDueTime->LowPart = fTime.dwLowDateTime;
		pMainDialogWindow->PostMessage(WM_COMMAND,SETWAITT,LPARAM(liDueTime));
	}
	return res;
}

CString Reminder::GetTextContent()
{
	return ActionPars[1];
};

CString Reminder::GetItemDsc()
{
	CString sDsc;
	sDsc+=GetTitle();
	sDsc+=" (";
	sDsc+=GetState()?_l("Active"):_l("Inactive");
	sDsc+=")";
	if(IsShared()){
		sDsc+=Format("\n %s %s",_l("Published at billboard by"), GetAuthor());
	}
	if(GetRevision()){
		sDsc+=Format("\n %s: %i",_l("Revision"), GetRevision());
	}
	if(!objSettings.bHideDateFld){
		sDsc+=Format("\n %s: %s",_l("Modified"),GetDateInCurFormat(GetCrTime()));
	}
	COleDateTime time;
	if(CalulateClosestTime(time)){
		sDsc+="\n ";
		sDsc+=_l("Actuation")+": ";
		sDsc+=time.Format(LOGTIMEFRMT);
	}
	return sDsc;
};

void Reminder::ActivateChanges(const char* szTitle, DWORD* pdwPrir, DWORD* pdwState)
{
	if(szTitle){
		SetTitle(szTitle);
	}
	if(pdwPrir){
		SetPriority(*pdwPrir);
	}
	if(GetState()!=-1 && pdwState){
		SetState(*pdwState);
	}
	ItemContentChanged();
}

void Reminder::OnReminderActivated()
{
	LastTriggerTime=ClosestTriggerTime;
	ClosestTriggerTime.SetStatus(COleDateTime::invalid);
	OnModify();
}

BOOL Reminder::CalulateClosestTime(COleDateTime& CheckTimeMustBe)
{
	if(ClosestTriggerTime.GetStatus()==COleDateTime::valid){
		CheckTimeMustBe=ClosestTriggerTime;
		return TRUE;
	}
	COleDateTime CurTime=COleDateTime::GetCurrentTime();
	int year=CurTime.GetYear();
	int month=CurTime.GetMonth();
	int day=CurTime.GetDay();
	int hour=CurTime.GetHour();
	int minute=CurTime.GetMinute();
	if(bYearType==0){
		year=dwYear;
	}
	if(bMonType==0){
		month=dwMon;
	}
	if(bDayType==0){
		day=dwDay;
	}
	if(bDayType==2){
		int iCurWDay=CurTime.GetDayOfWeek();
		day=day-(iCurWDay+(iCurWDay<dwWDay?7:0)-dwWDay);
		if(day<=0){
			day+=7;
		}
	}
	//Время
	if(bTimeType==0){
		hour=dwTime.GetHour();
		minute=dwTime.GetMinute();
	}
	CheckTimeMustBe.SetDateTime(year,month,day,hour,minute,0);
	if(CheckTimeMustBe.GetStatus()!=COleDateTime::valid){
		return FALSE;
	}
	if(bTimeType==1 && dwTimeNum!=0){
		long lTotalMins=dwTimeNum;//Минуты
		if(iTimeNumType==1){//часы
			lTotalMins=dwTimeNum*60;
		}else if(iTimeNumType==2){//дни
			lTotalMins=dwTimeNum*60*24;
		}
		//Разница между текущим и CreationTime в минутах
		COleDateTime CrDateSettedTime(CreationTime.GetYear(),CreationTime.GetMonth(),CreationTime.GetDay(),dwTime.GetHour(),dwTime.GetMinute(),0);
		long lDiffMinutes=(long)(CurTime-CrDateSettedTime).GetTotalMinutes();
		//Столько надо добавить к текущему чтобы сработал
		double dFromLastTrig=lDiffMinutes%lTotalMins;
		double dMinToAdd=lTotalMins-dFromLastTrig;
		COleDateTimeSpan AddDays(dMinToAdd/60/24);
		CheckTimeMustBe=CheckTimeMustBe+AddDays;
	}
	// Иногда вычисленное время попадает в период до появления reminderа вообще!
	if(CheckTimeMustBe<=GetCrTime()){
		return FALSE;
	}
	if(LastTriggerTime.GetStatus()==COleDateTime::valid && CheckTimeMustBe<=LastTriggerTime && LastTriggerTime<=CurTime){
		return FALSE;
	}
	ClosestTriggerTime=CheckTimeMustBe;
	return TRUE;
};

BOOL Reminder::IsStrictReminder()
{
	if(bTimeType==0 && bDayType==0 && bMonType==0 && bYearType==0){
		return TRUE;
	}
	return FALSE;
};

BOOL CALLBACK PostponeAlert(void* param, int iButton)
{
	BOOL bRes=TRUE;
	Reminder* rem=(Reminder*)param;
	if(rem){
		BOOL bNeedToDel=TRUE;
		if(iButton==2){
			CMenu pRemind;
			pRemind.CreatePopupMenu();
			AddMenuString(&pRemind, ID_NOTEMENU_REMINDIN15MIN, _l("Remind in 15 min"));
			AddMenuString(&pRemind, ID_NOTEMENU_REMINDIN30MIN, _l("Remind in 30 min"));
			AddMenuString(&pRemind, ID_NOTEMENU_REMINDIN1H, _l("Remind in 1h"));
			AddMenuString(&pRemind, ID_NOTEMENU_REMINDAFTER3H, _l("Remind in 3h"));
			AppendAdverts(&pRemind);
			AddMenuString(&pRemind, ID_NOTEMENU_REMINDTOMORROW, _l("Remind in")+"\t...");
			CPoint pos;
			::GetCursorPos(&pos);
			int iRes=::TrackPopupMenu(pRemind.GetSafeHmenu(), TPM_RETURNCMD, pos.x, pos.y, 0, pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL, NULL);
			bNeedToDel=FALSE;
			if(iRes!=0){
				ParseAdvert(iRes);
				objSettings.m_Reminders.Add(rem);
				switch(iRes){
				case ID_NOTEMENU_REMINDIN15MIN:
					rem->dwTime+=CTimeSpan(0,0,15,0);
					break;
				case ID_NOTEMENU_REMINDIN30MIN:
					rem->dwTime+=CTimeSpan(0,0,30,0);
					break;
				case ID_NOTEMENU_REMINDIN1H:
					rem->dwTime+=CTimeSpan(0,1,0,0);
					break;
				case ID_NOTEMENU_REMINDAFTER3H:
					rem->dwTime+=CTimeSpan(0,3,0,0);
					break;
				case ID_NOTEMENU_REMINDTOMORROW:
					rem->CreateRemDialog();
					break;
				default:
					bNeedToDel=FALSE;
					break;
				}
			}else{
				bRes=FALSE;
			}
		}
		if(bNeedToDel){
			delete rem;
		}
	}
	return bRes;
}

void Reminder::PerformReminderActions(BOOL bTest)
{
	if(bInAction){
		// Этот процесс уже идет!!!
		return;
	}
	bInAction=TRUE;
	if(!bTest){
		OnReminderActivated();
	}
	// Сверяем игнор...
	BOOL bIgnoring=0;
	COleDateTime dt=COleDateTime::GetCurrentTime();
	if(ActionUse[12]==1){
		CString sTest=Format(",%s,",ActionPars[12]);
		CString sDay=Format(",%s,",GetWeekDay(dt.GetDayOfWeek()));
		if(sTest.Find(sDay)!=-1){
			bIgnoring=1;
		}
		sDay=Format(",%s,",GetWeekDay(dt.GetDayOfWeek(),TRUE));
		if(sTest.Find(sDay)!=-1){
			bIgnoring=1;
		}
	}
	if(ActionUse[13]==1){
		CString sTest=Format(",%s,",ActionPars[13]);
		CString sDay=Format(",%i,",dt.GetDay());
		if(sTest.Find(sDay)!=-1){
			bIgnoring=1;
		}
	}
	BOOL bShutdown=0;
	if(!bIgnoring){//Производим действия
		if(!bTest && (ActionUse[7]==1 && strlen(ActionPars[7])>0)){
			if(!objSettings.bNoSend){
				// Шлем сообщение
				CString sComp=objSettings.AddrBook.TranslateNickToIP(ActionPars[7]);
				SendNetMessage(sComp,ActionPars[1],"","",_l("Reminder: sending net message..."));
			}
		}
		if(ActionUse[0]==1){
			// Звук, если надо - повторяющийся
			if(ActionPars[0].Find("alarm=")!=-1){
				AsyncPlaySound(ActionPars[0]);
			}else{
				PlayMusicX(ActionPars[0],0,"WNRemind");
			}
		}
		if(ActionUse[2]==1){
			// Трясем окно в течении X секунд
			DWORD dwTimeout=atol(ActionPars[2])*1000;
			if(dwTimeout==0){
				dwTimeout=1000;
			}
			IAnimateWindow(CWnd::FromHandle(::GetForegroundWindow()),-1,dwTimeout);
		}
		if((ActionUse[1]==1 || ActionUse[5]==1) && ActionUse[7]==0){
			// Выводим тект
			InfoWndStruct* info=new InfoWndStruct();
			if(ActionPars[1].GetLength()>0){
				info->sTitle=GetTitle();
				info->sText=ActionPars[1];
			}else{
				info->sTitle=dt.Format("%d.%m.%Y %H:%M:%S");
				info->sText=GetTitle();
			}
			info->lBlockOKForAwhile=objSettings.lRemSubmitPreventer?DEF_OKBLOCKTIME:0;
			info->iIcon=-GetIconNum();
			info->bStartHided=(ActionUse[5]==1)?TRUE:FALSE;
			info->iStyle=this->GetColorCode();
			info->dwStaticPars=DT_CENTER;
			info->sCancelButtonText=_l("Postpone");
			if(ActionUse[10]==1){
				DWORD dwTOut=atol(ActionPars[10]);
				info->dwTimer=((dwTOut>0)?dwTOut:60)*1000;
			}
			InfoWnd* pAlertWnd=Alert(info);
			if(pAlertWnd && !bTest){
				CString sPostponedID=CString(GetID())+"_POSTPONED";
				Reminder* pPostponed=NULL;//(Reminder*)CItem::GetItemByID(sPostponedID);
				if(pPostponed==NULL){
					pPostponed=new Reminder();
					*pPostponed=*this;
					pPostponed->SetPath(REMS_PATH"/"REMD_PATH);
					CString sOldTitle=GetTitle();
					CString sPostp=_l("Postponed");
					int iCount=0;
					if(sOldTitle.Find(sPostp)!=-1){
						iCount=sOldTitle.Replace(sPostp,"");
						sOldTitle.TrimLeft(" [");
						iCount+=atol(sOldTitle);
						sOldTitle.TrimLeft("1234567890 ]:");
						if(iCount<1){
							iCount=1;
						}
						sPostp=Format("%s [%i]",sPostp,iCount);
					}
					pPostponed->SetTitle(sPostp+": "+sOldTitle);
				}
				COleDateTime dt=COleDateTime::GetCurrentTime();
				pPostponed->bExplicitDeleteReminder=TRUE;
				pPostponed->LastTriggerTime.SetStatus(COleDateTime::invalid);
				pPostponed->dwDay=dt.GetDay();
				pPostponed->dwMon=dt.GetMonth();
				pPostponed->dwYear=dt.GetYear();
				pPostponed->dwTime=CTime(dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
				pPostponed->bDayType=pPostponed->bMonType=pPostponed->bTimeType=pPostponed->bYearType=0;
				pAlertWnd->SetCallback(PostponeAlert,pPostponed);
			}
		}
		if(ActionUse[3]==1){
			// Попапим ноту
			if(ActionPars[3].GetLength()>0){
				CItem* pItem=CItem::GetItemByTitle(ActionPars[3]);
				if(pItem){
					if(pItem->GetType()==1 && pMainDialogWindow){
						::SetForegroundWindow(pMainDialogWindow->GetSafeHwnd());
						pMainDialogWindow->PostMessage(CREATE_NEWTODO,WPARAM(pItem),1);
						
					}
					if(pItem->GetType()==2){
						CDLG_Note* pNote=objSettings.m_Notes.FindNoteByID(pItem->GetID());
						if(pNote){
							pNote->RemindThisNote();
						}
					}
				}
			}
		}
		if(ActionUse[4]==1){
			// Приложение
			if(ActionPars[4].GetLength()>0){
				CString sExeline=ActionPars[4];
				CString sParams="";
				SubstMyVariables(sExeline);
				if(sExeline.GetAt(0)=='\"'){
					int iParPos=sExeline.Find("\"",1);
					if(iParPos>0 && iParPos<(sExeline.GetLength()-2) && sExeline.GetAt(iParPos+1)==' '){
						sParams=sExeline.Mid(iParPos+2);
						sExeline=sExeline.Left(iParPos);
						sExeline=sExeline.Mid(1);
					}
				}
				char szDisk[MAX_PATH]="",szPath[MAX_PATH]="";
				_splitpath(sExeline,szDisk,szPath,NULL,NULL);
				CString sExeDir=szDisk;
				sExeDir+=szPath;
				int iRes=(int)::ShellExecute((pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL),"open",sExeline,sParams,sExeDir,SW_SHOWNORMAL);
				if(iRes<=32){//ERROR_FILE_NOT_FOUND
					Alert(_l("Failed to start")+" "+TrimMessage(sExeline,20,2)+"\n"+_l("Error")+" "+Format("%i",iRes),_l("WireNote: Reminder"));
				}
			}
		}
		//5ка проверяется в п.1
		if(ActionUse[8]==1){
			// Активируем напоминатель/задачу
			if(ActionPars[8].GetLength()>0){
				CItem* pItem=CItem::GetItemByTitle(ActionPars[8]);
				if(pItem){
					if(pItem->GetType()==3){
						pItem->SetState(TRUE);
					}else{
						if(pItem->GetType()==1){
							objSettings.m_Tasks.TaskEnter(pItem->GetID(),FALSE);
						}
					}
				}
			}
		}
		if(ActionUse[9]==1){
			// Деактивируем напоминатель/задачу
			if(ActionPars[9].GetLength()>0){
				CItem* pItem=CItem::GetItemByTitle(ActionPars[9]);
				if(pItem){
					if(pItem->GetType()==3){
						pItem->SetState(FALSE);
					}else{
						if(pItem->GetType()==1){
							objSettings.m_Tasks.TaskLeave(pItem->GetID());
						}
					}
				}
			}
		}
		//6ка...
		if(ActionUse[6]==1){
			bShutdown=1;
		}
	}
	bInAction=FALSE;
	//Удаляем сработавший ремаиндер (кроме регистрационного)
	if(!bTest){
		BOOL bNeedDel=-1;
		if(bExplicitDeleteReminder){
			bNeedDel=1;
		}else if(IsStrictReminder()==TRUE && strcmp(GetID(),REGREMIND_ID)!=0){
			if(objSettings.bRemDelStrictDef){
				// Удаляем ремаиндер, т.к. он стриктовый
				bNeedDel=1;
			}else{
				bNeedDel=0;
			}
		}
		if(bNeedDel!=-1){
			if(bNeedDel==1){
				objSettings.m_Reminders.Delete(GetID());
			}else{
				SetState(0);
			}
		}
	}
	if(bShutdown){
		objSettings.bSaveAllowed=1;
		objSettings.SaveAll();
		// Выключаем комп
		ShutDownComputer();
	}
}

Reminder* AddReminder(int Minutes, const char* szTitle)
{
	if(!szTitle || strlen(szTitle)==0){
		return NULL;
	}
	Reminder* rem=new Reminder();
	rem->SetPath(REMS_PATH"/"REMD_PATH);
	rem->SetTitle(_l("Remind about")+" '"+szTitle+"'");
	rem->dwTime=rem->dwTime+CTimeSpan(0,0,Minutes,0);
	rem->SetIconNum(NOTE_ICON_NUM);
	rem->ActionUse[3]=1;
	rem->ActionPars[3]=szTitle;
	rem->ActionUse[0]=objSettings.bRemNoteSoundByDef;
	rem->ActionPars[0]=objSettings.sRemNoteFileWav;
	rem->ActionUse[5]=0;
	objSettings.m_Reminders.Add(rem);
	return rem;
}
