// WCRemind.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "WCRemind.h"
#include "CalendarDlg2.h"
#include "AlertDialog.h"
#include "OCalendar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWCRemindApp

BEGIN_MESSAGE_MAP(CWCRemindApp, CWinApp)
	//{{AFX_MSG_MAP(CWCRemindApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWCRemindApp construction

CWCRemindApp::CWCRemindApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWCRemindApp object

CWCRemindApp theApp;
char* WINAPI _path()
{
	static char szPath[MAX_PATH]={0};
	return szPath;
}

CCriticalSection cs;
BOOL bNeedResort=0;
CArray<CWPReminder,CWPReminder&> aReminders;
void LoadReminder(const char* szFile)
{
	CWPReminder rem;
	CString sOutFile=szFile;
	if(!isFileExist(szFile)){
		return;
	}
	_splitpath(sOutFile,0,0,rem.szKey,0);
	CDataXMLSaver remXml(sOutFile,"reminder",TRUE);
	COleDateTime dt;
	remXml.getValue("EventTime",dt);
	if(dt.GetStatus()!=COleDateTime::valid || !dt.GetAsSystemTime(rem.EventTime)){
		return;
	}
	if(rem.EventTime.wHour==0 && rem.EventTime.wMinute==0 && rem.EventTime.wSecond==0)
	{// Битый!!!
		/*#ifdef _DEBUG
		AfxMessageBox("Invalid reminder found!");
		#endif // _DEBUG
		return;*/
		rem.EventTime.wSecond=1;
	}
	CString sTmp;
	remXml.getValue("Text",sTmp);
	strcpy(rem.szText,sTmp);
	remXml.getValue("Recurrency",rem.iRecurrency);
	remXml.getValue("ActPopup",rem.bActPopup);
	remXml.getValue("ActLaunch",rem.bActLaunch);
	remXml.getValue("ActSount",rem.bActSound);
	remXml.getValue("LoopSound",rem.bLoopSound);
	remXml.getValue("ActIgnoreOnSkip",rem.bActIgnoreOnSkip);
	remXml.getValue("ActAutoClosePopup",rem.bActAutoClosePopup);
	remXml.getValue("LaunchPath",sTmp);
	remXml.getValue("XMinHourPeriod",rem.dXMinHourPeriod);
	strcpy(rem.szLaunchPath,sTmp);
	remXml.getValue("SoundPath",sTmp);
	strcpy(rem.szSoundPath,sTmp);
	remXml.getValue("Additional",sTmp);
	strcpy(rem.szReserved,sTmp.Left(sizeof(rem.szReserved)-1));
	PutReminderRaw(rem.szKey,rem,0);
}

void SaveReminder(char szKey[64], const char* szFile=0)
{
	CWPReminder rem;
	if(!GetReminder(szKey,rem)){
		return;
	}
	CString sOutFile=szFile;
	if(sOutFile==""){
		sOutFile=_path();
		sOutFile+=szKey;
		sOutFile+=".xml";
	}
	CDataXMLSaver remXml(sOutFile,"reminder",FALSE);
	remXml.putValue("Text",rem.szText);
	remXml.putValue("Recurrency",rem.iRecurrency);
	remXml.putValue("EventTime",COleDateTime(rem.EventTime));
	remXml.putValue("ActPopup",rem.bActPopup);
	remXml.putValue("ActLaunch",rem.bActLaunch);
	remXml.putValue("ActSount",rem.bActSound);
	remXml.putValue("ActIgnoreOnSkip",rem.bActIgnoreOnSkip);
	remXml.putValue("ActAutoClosePopup",rem.bActAutoClosePopup);
	remXml.putValue("LoopSound",rem.bLoopSound);
	remXml.putValue("LaunchPath",rem.szLaunchPath);
	remXml.putValue("XMinHourPeriod",rem.dXMinHourPeriod);
	remXml.putValue("SoundPath",rem.szSoundPath);
	remXml.putValue("Additional",rem.szReserved);
}

void LoadReminders()
{
	CCriticalSectionGuard l(&cs);
	bNeedResort=1;
	CFileInfoArray dir;
	char szD[MAX_PATH]="",szP[MAX_PATH]="";
	dir.AddDir(_path(),"*.xml",0,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	for (int i=0;i<dir.GetSize();i++){
		CString sFile=dir[i].GetFilePath();
		if(sFile!=""){
			LoadReminder(sFile);
		}
	}
}

BOOL ModifyOnRecurrency(int iRecurrency,SYSTEMTIME& EventTime, DWORD lAddParam)
{
	COleDateTime dt(EventTime);
	switch(iRecurrency){
	case 107:
		return 0;
		break;
	case 101:
		dt+=COleDateTimeSpan(0,0,5,0);
		break;
	case 102:
		dt+=COleDateTimeSpan(0,0,15,0);
		break;
	case 103:
		dt+=COleDateTimeSpan(0,0,30,0);
		break;
	case 104:
		dt+=COleDateTimeSpan(0,1,0,0);
		break;
	case 105:
	case RECURRENCY_DAILY:
		dt+=COleDateTimeSpan(1,0,0,0);
		break;
	case 106:
	case RECURRENCY_WEEKLY:
		dt+=COleDateTimeSpan(7,0,0,0);
		break;
	case RECURRENCY_MONTHLY:
		if(dt.GetMonth()==12){
			dt.SetDate(dt.GetYear()+1,1,dt.GetDay());
		}else{
			dt.SetDate(dt.GetYear(),dt.GetMonth()+1,dt.GetDay());
		}
		break;
	case RECURRENCY_ANNUALLY:
		dt.SetDate(dt.GetYear(),dt.GetMonth(),dt.GetDay());
		break;
	case RECURRENCY_XMINUTES:
		if(lAddParam==0){
			lAddParam=15;
		}
		dt+=COleDateTimeSpan(0,0,lAddParam,0);
		break;
	case RECURRENCY_XHOURS:
		if(lAddParam==0){
			lAddParam=1;
		}
		dt+=COleDateTimeSpan(0,lAddParam,0,0);
		break;
	}
	dt.GetAsSystemTime(EventTime);
	return 1;
}

BOOL ModifyOnRecurrencyEx(int iRecurrency,SYSTEMTIME& EventTime, DWORD lAddParam)
{
	ModifyOnRecurrency(iRecurrency,EventTime, lAddParam);
	COleDateTime dtCur=COleDateTime::GetCurrentTime();
	while(COleDateTime(EventTime)<dtCur){
		ModifyOnRecurrency(iRecurrency,EventTime, lAddParam);
	}
	return 1;
}

#include <mmsystem.h>
HWND PlayMusic(CString szSoundPath,BOOL bLoopSound, CString sAlias)
{
	CString sLow=szSoundPath;
	sLow.MakeLower();
	if(sLow.Find(".wav")!=-1){
		if(bLoopSound>=0){
			getWinmm().myPlaySound(szSoundPath,NULL,SND_FILENAME|SND_ASYNC|(bLoopSound?SND_LOOP:0));//SND_FILENAME|SND_NODEFAULT|SND_NOSTOP|SND_NOWAIT|SND_ASYNC - падает! :(
		}else{
			getWinmm().myPlaySound(NULL,NULL,SND_FILENAME|SND_ASYNC);
		}
	}else{
		CString sCommand;
		MCIERROR iErr=0;
		if(bLoopSound>=0){
			szSoundPath=CString("\"")+szSoundPath+CString("\"");
			sCommand=CString("open ")+szSoundPath+" alias "+sAlias;
			iErr=getWinmm().myMciSendString(sCommand, 0, 0, 0);
			if(iErr==0){
				sCommand=CString("play ")+sAlias+(bLoopSound?" repeat":"");
				iErr=getWinmm().myMciSendString(sCommand, 0, 0, 0);
				TRACE1("Playing %s...\n",szSoundPath);
			}
			//mciExecute(CString("play ")+szSoundPath);
		}else{
			MCIERROR iErr=getWinmm().myMciSendString(CString("close ")+sAlias, 0, 0, 0);
			TRACE1("Stopping %s...\n",szSoundPath);
		}
		/*
		HWND m_Video = hPrevInstance;
		if(m_Video != NULL)
		{
			//MCIWndHome(m_Video);
			MCIWndStop(m_Video);
			MCIWndDestroy(m_Video);
		}
		if(szSoundPath!=""){
			m_Video = MCIWndCreate(hParent,
				theApp.m_hInstance,
				(hParent?WS_CHILD:WS_POPUP)|MCIWNDF_NOMENU,szSoundPath);
			MCIWndPlay(m_Video);
		}
		return m_Video;
		*/
	}
	return 0;
}

COleDateTime dtAppStartTime;
CCriticalSection csWorking;
_ReminderCallback pCallbackFunction=0;
DWORD WINAPI TriggerReminder(LPVOID p)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	char* szKeyI=(char*)p;
	CString szKey=szKeyI;
	free(szKeyI);
	BOOL bFinded=0;
	BOOL bLoopSound=0;
	BOOL bTestDrive=FALSE;
	CWPReminder rem;
	{// Защищаем работу с массивом
		CCriticalSectionGuard lc(&csWorking);
		int iIndex=GetReminderIndex(szKey);
		if(iIndex!=-1){
			bFinded=1;
			memcpy(&rem,&aReminders[iIndex],sizeof(CWPReminder));
			// Блокируем его чтобы даже за это небольшое время он не отработал еще раз
			aReminders[iIndex].bDisableRemider=1;
		}
		if(bFinded){
			COleDateTime dtRem(rem.EventTime);
			bTestDrive = (rem.bActIgnoreOnSkip && dtRem<dtAppStartTime);
			if(bTestDrive)
			{
				bTestDrive=1;
			}
			if(!bTestDrive){
				if(pCallbackFunction){
					if((*pCallbackFunction)(rem)){
						iIndex=GetReminderIndex(szKey);
						if(iIndex!=-1){// Заново!
							aReminders[iIndex].bDisableRemider=0;
						}
						return 0;
					}
				}
			}
			bLoopSound=rem.bLoopSound;
			if(!rem.bActPopup){
				bLoopSound=FALSE;
			}
			RemoveReminder(rem.szKey);
			if(rem.iRecurrency!=RECURRENCY_NO){
				CWPReminder rem2;
				memcpy(&rem2,&rem,sizeof(CWPReminder));
				// Двигаем!!!
				ModifyOnRecurrencyEx(rem2.iRecurrency,rem2.EventTime,rem2.dXMinHourPeriod);
				PutReminder(rem2.szKey,rem2);
				// Портим старый ключ
				rem.iRecurrency=RECURRENCY_NO;
				CString sKey=GetNextPrefixedId(rem.szKey);
				strcpy(rem.szKey,sKey.Left(sizeof(rem.szKey)-1));
				while(GetReminderIndex(rem.szKey)!=-1){
					sKey=GetNextPrefixedId(rem.szKey);
					strcpy(rem.szKey,sKey.Left(sizeof(rem.szKey)-1));
				}
			}
			if(!bTestDrive){
				if(rem.bActLaunch){
					ShellExecute(0,_T("open"),rem.szLaunchPath,NULL,NULL,SW_SHOWNORMAL);
				}
				if(rem.bActSound){
					PlayMusic(rem.szSoundPath,bLoopSound);
				}
			}
		}
	}
	BOOL bReminderOk = TRUE;
	// Это уже в незащищенной части...
	if(!bTestDrive){
		static long lPreventFloodCnt=0;
		SimpleTracker lc(lPreventFloodCnt);
		if(bFinded && rem.bActPopup && lPreventFloodCnt<10){
			CString sText="";
			char szTmp[128]={0};
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&rem.EventTime,0,szTmp,sizeof(szTmp));
			char szTmp2[128]={0};
			GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,&rem.EventTime,0,szTmp2,sizeof(szTmp2));
			sText=Format("%s %s ",szTmp,szTmp2);
			CString sPostponeMark=_lx("RMX.POSTPONED","Postponed");
			if(strstr(rem.szText,sPostponeMark)==0){
				sText+="\n";
			}
			if(strlen(rem.szText)==0){
				sText+=_lx("RMX.EMPTYALERT","Attention! Time has come");
			}else{
				sText+=rem.szText;
			}
			sText+="\n\n";
			CAlertDialog dlg;
			dlg.iRes=-1;
			dlg.sText=sText;
#ifdef _DEBUG
			dlg.sText+="[";
			dlg.sText+=szKey;
			dlg.sText+="]";
#endif
			if(rem.bActAutoClosePopup)
			{
				dlg.lDoAutoclose=15;//15 secs
			}
			dlg.DoModal();
			DWORD dwErr=GetLastError();
			if(dlg.iRes!=0){
				CString sText=rem.szText;
				CString sPostponeMark=_lx("RMX.POSTPONED","Postponed");
				if(sText.Find(sPostponeMark)==-1){
					sText=sPostponeMark+"\r\n"+sText;
					strcpy(rem.szText,sText.Left(4000));
				}
				{// Если время уже ушло,
					// то откладываем от ТЕКУЩЕГО момента!!!
					COleDateTime dtNow=COleDateTime::GetCurrentTime();
					COleDateTime dtRem(rem.EventTime);
					if(dtRem<dtNow){
						dtNow.GetAsSystemTime(rem.EventTime);
					}
				}
				if(!ModifyOnRecurrency(dlg.iRes,rem.EventTime,0)){
					PutReminder(rem.szKey,rem);
					CallModifyReminder(rem.szKey,0,0);
				}else{
					PutReminder(rem.szKey,rem);
				}
			}
		}
	}
	if(!bTestDrive){
		if(bFinded && bLoopSound && rem.bActSound)
		{
			PlayMusic(rem.szSoundPath,-1);
		}
	}
	return 0;
}

BOOL bStop=0;
HANDLE hTimer=0;
HANDLE hStopEvent=0;
DWORD WINAPI GlobalOnTimer(LPVOID)
{
	dtAppStartTime=COleDateTime::GetCurrentTime();
	LoadReminders();
	int iNextTimeout=5;
	while(!bStop){
		WaitForSingleObject(hStopEvent,iNextTimeout*1000+100);
		if(bStop){
			break;
		}
		{// Обрабатываем и считаем время до следзадержки!
			//CCriticalSectionGuard lc(&csWorking);
			BOOL bFoundNext=0;
			CStringArray aKeys;
			CCriticalSectionGuard l(&cs);
			COleDateTime dtNow=COleDateTime::GetCurrentTime();
			COleDateTime dtNext=dtNow+COleDateTimeSpan(99,99,99,99);
			for(int i=0;i<aReminders.GetSize();i++){
				CWPReminder& rem=aReminders[i];
				COleDateTime dt(rem.EventTime);
				if(rem.bDisableRemider==0){
					if(dt<dtNow){
						BOOL bAdd=1;
						for(int j=0;j<aKeys.GetSize();j++){
							if(strcmp(aKeys[j],rem.szKey)==0){
								bAdd=0;
								break;
							}
						}
						if(bAdd){
							aKeys.Add(rem.szKey);
						}
					}else{
						if(dt>dtNow && dt<dtNext){
							bFoundNext=1;
							dtNext=dt;
						}
					}
				}
			}
			for(int j=0;j<aKeys.GetSize();j++){
				FORK(TriggerReminder,strdup(aKeys[j]));
			}
			if(bFoundNext){
				COleDateTimeSpan dtToNext=(dtNext-COleDateTime::GetCurrentTime());
				iNextTimeout=dtToNext.GetSeconds()+1;
			}
			if(!bFoundNext || iNextTimeout<0){
				// По умолчанию раз в минуту...
				iNextTimeout=50;
			}
		}
	}
	return 0;
}

CImgLoader*& WINAPI _bmpX()
{
	static CImgLoader* bmpThis=0;
	/*
	if(!bmpThis){
		bmpThis=&_bmp();
	}
	*/
	return bmpThis;
}


IUITranslation*& _lngX()
{
	static IUITranslation* lang=CUITranslationImpl::getInstance();
	return lang;
};

CString _lx(const char* szText,const char* szDef)
{
	char* sz=_lngX()->uit(szText,szDef);
	CString sRes=sz;
	_lngX()->freestr(sz);
	return sRes;
}

BOOL WINAPI InitDLL(int ihash, const char* szInitDir,_ReminderCallback pCallbackFunctionIn)
{
	bStop=1;
	if(ihash!=0){
		return 0;
	}
	bStop=0;
	DWORD dwId=0;
	strcpy(_path(),szInitDir);
	pCallbackFunction=pCallbackFunctionIn;
	hStopEvent=CreateEvent(0,0,0,0);
	hTimer=CreateThread(0,0,GlobalOnTimer,0,0,&dwId);
	return 1;
}

BOOL WINAPI StopDLL()
{
	bStop=1;
	SetEvent(hStopEvent);
	WaitForSingleObject(hTimer,INFINITE);
	CloseHandle(hTimer);
	CloseHandle(hStopEvent);
	return 1;
}

int WINAPI GetCountOnDate(char szDate[64])
{
	if(strcmp(szDate,"ALL")==0){
		return aReminders.GetSize();
	}
	if(bNeedResort){
		SortReminders();
	}
	int iCount=0;
	COleDateTime dt=CDataXMLSaver::Str2OleDate(szDate);
	if(dt.GetStatus()!=COleDateTime::valid){
		return 0;
	}
	CCriticalSectionGuard l(&cs);
	for(int i=0;i<aReminders.GetSize();i++){
		SYSTEMTIME sm;
		memcpy(&sm,&aReminders[i].EventTime,sizeof(SYSTEMTIME));
		if(sm.wDay==dt.GetDay() && sm.wMonth==dt.GetMonth() && sm.wYear==dt.GetYear()){
			iCount++;
		}
	}
	return iCount;
}

BOOL WINAPI GetReminderOnDate(char szDate[64], int iIndex, CWPReminder& pOut)
{
	if(iIndex<0 || iIndex>=aReminders.GetSize()){
		return 0;
	}
	if(strcmp(szDate,"ALL")==0){
		memcpy(&pOut,&aReminders[iIndex],sizeof(CWPReminder));
		return TRUE;
	}
	if(bNeedResort){
		SortReminders();
	}
	int iCount=0;
	COleDateTime dt=CDataXMLSaver::Str2OleDate(szDate);
	if(dt.GetStatus()!=COleDateTime::valid){
		return 0;
	}
	CCriticalSectionGuard l(&cs);
	for(int i=0;i<aReminders.GetSize();i++){
		SYSTEMTIME sm;
		memcpy(&sm,&aReminders[i].EventTime,sizeof(SYSTEMTIME));
		if(sm.wDay==dt.GetDay() && sm.wMonth==dt.GetMonth() && sm.wYear==dt.GetYear()){
			if(iIndex==iCount){
				memcpy(&pOut,&aReminders[i],sizeof(CWPReminder));
				return TRUE;
			}
			iCount++;
		}
	}
	return FALSE;
}

BOOL WINAPI CallModifyReminderX(char szKey[64], CWnd* wParent, DWORD dwOptions)
{
	int iOut=IDCANCEL;
	int iIndex=-1;
	{
		CCriticalSectionGuard l(&cs);
		iIndex=GetReminderIndex(szKey);
		if(iIndex==-1){
			return IDCANCEL;
		}
		aReminders[iIndex].bDisableRemider=1;
	}
	CWPReminder rem;
	memcpy(&rem,&aReminders[iIndex],sizeof(CWPReminder));
	CCalendarDlg2 dlg(rem,wParent,dwOptions);
	DWORD dwErr=GetLastError();
	if(dlg.DoModal()>=0){
		aReminders[iIndex].bDisableRemider=0;// При отмене валидного ремаиндера он должен остаться енабленным ведь!!!!
		if(dlg.iModalRes==IDABORT){
			RemoveReminder(szKey);
		}else if(dlg.iModalRes==IDOK){
			PutReminder(szKey,dlg.rem);
		}
		// Обновление времени...
		SetEvent(hStopEvent);
		iOut=dlg.iModalRes;
	}
	return iOut;
}

BOOL WINAPI CallModifyReminder(char szKey[64], HWND hParent, DWORD dwOptions)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BOOL bRes=0;
	CWnd* pParent=CWnd::FromHandlePermanent(hParent);
	if(pParent){
		bRes=CallModifyReminderX(szKey, pParent, dwOptions);
	}else{
		CWnd wParent;
		wParent.Attach(hParent);
		bRes=CallModifyReminderX(szKey, &wParent, dwOptions);
		wParent.Detach();
	}
	return bRes;
}

int WINAPI GetReminderIndex(const char* szKey)
{
	int iRes=-1;
	CCriticalSectionGuard l(&cs);
	for(int i=0;i<aReminders.GetSize();i++){
		const char* szTestKey=aReminders[i].szKey;
		if(aReminders[i].EventTime.wHour==0 && aReminders[i].EventTime.wMinute==0 && aReminders[i].EventTime.wSecond==0)
		{
			AfxMessageBox("Invalid reminder found!!! - check by key");
			continue;
		}
		if(strcmp(szTestKey,szKey)==0){
			iRes=i;
			break;
		}
	}
	return iRes;
}

BOOL WINAPI GetReminder(char szKey[64], CWPReminder& pOut)
{
	CCriticalSectionGuard l(&cs);
	int iIndex=-1;
	// Возможно нам передали индекс???
	if(szKey[0]=='#'){
		iIndex=atol(szKey+1);
	}else{
		iIndex=GetReminderIndex(szKey);
	}
	if(iIndex>=0 && iIndex<aReminders.GetSize()){
		memcpy(&pOut,&aReminders[iIndex],sizeof(CWPReminder));
		return TRUE;
	}
	return FALSE;
}

BOOL WINAPI PutReminderRaw(char szKey[64], CWPReminder& pIn, BOOL bAdjustOld)
{
	CCriticalSectionGuard l(&cs);
	bNeedResort=1;
	for(int i=0;i<aReminders.GetSize();i++){
		if(strcmp(aReminders[i].szKey,szKey)==0){
			RemoveReminder(szKey);
			break;
		}
	}
	CString sText=pIn.szText;
	sText.TrimLeft();
	sText.TrimRight();
	strcpy(pIn.szText,sText);
	if(pIn.EventTime.wHour==0 && pIn.EventTime.wMinute==0 && pIn.EventTime.wSecond==0)
	{// 0:0:0 - запретное значение
		pIn.EventTime.wSecond=1;
	}
	if(bAdjustOld){// Если время уже ушло
		COleDateTime dtNow=COleDateTime::GetCurrentTime();
		if(pIn.iRecurrency!=RECURRENCY_NO){
			COleDateTime dtRem(pIn.EventTime);
			while(dtRem<dtNow){
				COleDateTime dtRem1(pIn.EventTime);
				ModifyOnRecurrency(pIn.iRecurrency,pIn.EventTime,0);
				dtRem=COleDateTime(pIn.EventTime);
				if(dtRem==dtRem1){
					// Фигня :(((
					break;
				}
			}
		}
	}
	pIn.bDisableRemider=0;// Местами в путреминдер передаются задисабленные... но нам такие не нужны
	aReminders.Add(pIn);
	// Обновление времени...
	SetEvent(hStopEvent);
	return TRUE;
}

BOOL WINAPI PutReminder(char szKey[64], CWPReminder& pIn)
{
	PutReminderRaw(szKey,pIn,1);
	SaveReminder(szKey);
	return TRUE;
}

BOOL WINAPI RemoveReminder(char szKey[64])
{
	CCriticalSectionGuard l(&cs);
	for(int i=0;i<aReminders.GetSize();i++){
		if(strcmp(aReminders[i].szKey,szKey)==0){
			aReminders.RemoveAt(i);
			CString sOutFile;
			sOutFile=_path();
			sOutFile+=szKey;
			sOutFile+=".xml";
			DeleteFile(sOutFile);
			// Обновление времени...
			SetEvent(hStopEvent);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL WINAPI SortReminders()
{
	bNeedResort=0;
	CCriticalSectionGuard l(&cs);
	for(int i=0;i<aReminders.GetSize()-1;i++){
		for(int j=i+1;j<aReminders.GetSize();j++){
			COleDateTime dtI(aReminders[i].EventTime);
			COleDateTime dtJ(aReminders[j].EventTime);
			if(dtI.GetStatus()==COleDateTime::valid && dtJ.GetStatus()==COleDateTime::valid && dtI>dtJ){
				CWPReminder tmp;
				memcpy(&tmp,&aReminders[i],sizeof(CWPReminder));
				memcpy(&aReminders[i],&aReminders[j],sizeof(CWPReminder));
				memcpy(&aReminders[j],&tmp,sizeof(CWPReminder));
			}
		}
	}
	return 1;
}

BOOL WINAPI IsReminderFamilyPresent(const char* szKeyCC, int* iMaxSearchNumber)
{
	if(iMaxSearchNumber==0){
		return (GetReminderIndex(szKeyCC)!=-1);
	}
	int iMaxRes=-1;
	char szKey[64]={0};
	for(int i=0;i<=aReminders.GetSize();i++){
		strcpy(szKey,GetNextPrefixedId(szKeyCC,i));
		int iIndex=GetReminderIndex(szKey);
		if(iIndex!=-1){
			iMaxRes=i;
		}
	}
	if(iMaxSearchNumber){
		*iMaxSearchNumber=iMaxRes;
	}
	return (iMaxRes!=-1);
}

BOOL WINAPI OpenCalendar(CCalendarCallback* pCallback,BOOL bView)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	static COCalendar* pWnd=NULL;
	if(pWnd){
		SwitchToWindow(pWnd->GetSafeHwnd(),1);
		// Заапдейтить!!!
		pWnd->RefreshCalendar();
		return FALSE;
	}
	COCalendar dlg(pCallback,bView);
	pWnd=&dlg;
	dlg.DoModal();
	pWnd=NULL;
	return TRUE;
}

BOOL WINAPI CreateNewReminder(COleDateTime dt,CCalendarCallback* pCallback,CWnd* wParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWPReminder rem;
	dt=dt+COleDateTimeSpan(0,0,15,0);
	dt.GetAsSystemTime(rem.EventTime);
	rem.bActSound=1;
	rem.bActPopup=1;
	rem.bLoopSound=0;
	rem.bDisableRemider=1;//Актвным станет лишь после модификации!!!
	pCallback->GetRemDefText(rem.szText,sizeof(rem.szText));
	pCallback->GetRemDefWav(rem.szSoundPath,sizeof(rem.szSoundPath));
	pCallback->GetRemDefKey(rem.szKey,sizeof(rem.szKey));
	PutReminder(rem.szKey,rem);
	return CallModifyReminderX(rem.szKey,wParent,0);
}