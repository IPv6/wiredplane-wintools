// Filebox.cpp: implementation of the CFilebox class.
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

void AlertProtOff(CString szErr="",CNetFBProtocol* pParent=0)
{
	static BOOL bWarned=0;
	static long lErrCount=0;
	if(szErr==""){
		lErrCount=0;
		bWarned=0;
	}else{
		lErrCount++;
		// Создаем свой каталог если получится?
		// CreateDirIfNotExist(GetUserFBDir(pParent,pParent->m_sFB_ID));
		if(!bWarned){
			bWarned++;
			objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Warning")+": "+_l("Billboard is not accessible")+", Error: "+szErr);
			//pMainDialogWindow->ShowBaloon( _l("Warning"),_l("Billboard is not accessible.\nYou will not receive new items published on billboard")+"\n"+szErr);
		}
	}
}

BOOL CNetFBProtocol::WriteStatusInfo(const char* sTargetDir,const char* sPrefix)
{
	__try{
		return WriteStatusInfo_raw(sTargetDir,sPrefix);
	}
	__except(GetExceptionCode()!=0){
	};
	return FALSE;
}

BOOL CNetFBProtocol::WriteStatusInfo_raw(const char* szTargetDir,const char* szPrefix)
{
	CString sTargetDir=szTargetDir;
	CString sPrefix=szPrefix;
	CNetFBProtocol* pParent=this;
	static CString sLastContent="";
	CString sDir=((sTargetDir!="")?sTargetDir:GetFBFolder(pParent->m_sFB_ID));
	if(sDir==""){
		return 0;
	}
	CString sFile=sDir;
	if(sPrefix!=""){
		sFile+=sPrefix+"_";
	}
	sFile+=FBSTATUS;
	// Если есть файл удаляем его, о не чаще раза в 3 мин
	static DWORD dwLastStatDelTime=0;
	if(GetTickCount()-dwLastStatDelTime>3000){
		dwLastStatDelTime=GetTickCount();
		if(isFileExist(sFile)){
			if(!::DeleteFile(sFile)){
				AlertProtOff(_l("Can`t write data into shared folder")+"!",pParent);
				return FALSE;
			}
			AlertProtOff();
			sLastContent="";
		}
	}
	CDataXMLSaver xmlFile(sFile,"FB-STATUS",FALSE);
	// Ключи можно будет включить потом, когда появится шифрование
	long lStatus=objSettings.iOnlineStatus;
	if(!pParent->bStarted){
		lStatus=4;
	}
	xmlFile.putValue("Status",CreateMyStatusStringID(pParent->m_sFB_ID,FALSE,&lStatus));
	xmlFile.putValue("Nick",pParent->m_sFBNick);
	xmlFile.putValue("TimeStamp",COleDateTime::GetCurrentTime());
	if(lStatus==4 || sLastContent!=xmlFile.GetSource()){
		BOOL bRes=xmlFile.SaveDataIntoSource();
		if(bRes && lStatus!=4){
			sLastContent=xmlFile.GetSource();
		}
		return bRes;
	}
	xmlFile.SetAsSaved(TRUE);
	return TRUE;
}

BOOL CNetFBProtocol::ReadStatusInfo(const char* szUserFBID, CString* sUserDataO, CString* sNickO, COleDateTime* dtO)
{
	CNetFBProtocol* pParent=this;
	CString sDir=GetFBFolder(szUserFBID);
	if(sDir==""){
		return 0;
	}
	CString sFile=sDir+FBSTATUS;
	CDataXMLSaver xmlFile(sFile,"FB-STATUS",TRUE);
	if(!xmlFile.IsFileLoaded()){
		return FALSE;
	}
	COleDateTime dt;
	CString sUserData,sNick;
	xmlFile.getValue("Status",sUserData);
	UpdateUserIconFromStatusStringID(sUserData);
	xmlFile.getValue("Nick",sNick);
	xmlFile.getValue("TimeStamp",dt);
	if(sUserDataO){
		*sUserDataO=sUserData;
	}
	if(sNickO){
		*sNickO=sNick;
	}
	if(dtO){
		*dtO=dt;
	}
	return TRUE;
}

CString GetFBIdFromFile(CString sFile)
{
	sFile=GetDirectoryFromPath(sFile);
	sFile.TrimRight("\\/");
	int iFPos=sFile.Find(FBUSER_PR);
	if(iFPos!=-1){
		sFile=sFile.Mid(iFPos+strlen(FBUSER_PR));
		return sFile;
	}
	return "";
}

DWORD WINAPI FBListener(LPVOID lpdwstatus)
{
	CNetFBProtocol* pParent=(CNetFBProtocol*)lpdwstatus;
	if(!pParent){
		return 1;
	}
	CString sHomeFBDir=GetFBFolder(pParent->m_sFB_ID);
	if(sHomeFBDir==""){
		return 0;
	}
	static BOOL bBootup=1;
	if(bBootup){
		bBootup=0;
		WaitForSingleObject(pParent->hWaitEvent,25000);
	}
	// Сигнализируем что мы Online
	pParent->WriteStatusInfo();
	// Слушаем его периодически
	while(pParent->bStarted==1){
		if(isFileExist(sHomeFBDir)){
			objSettings.lFBStat_Titles="";
			objSettings.lFBStat_NewR=0;
			objSettings.lFBStat_UpdR=0;
			objSettings.lFBStat_NewT=0;
			objSettings.lFBStat_UpdT=0;
			objSettings.lFBStat_NewN=0;
			objSettings.lFBStat_UpdN=0;
			objSettings.lFBStat_UpdD=0;
			// Элементы доски объявлений
			if(objSettings.bOnSharedItemN){
				objSettings.m_Notes.LoadNotes(1);
			}
			if(objSettings.bOnSharedItemR){
				objSettings.m_Reminders.LoadReminders(1);
			}
			if(objSettings.bOnSharedItemT){
				objSettings.m_Tasks.LoadTasks(1);
			}
			if(objSettings.bOnSharedItemM){// Личные сообщения
				CFileInfoArray dir;
				dir.AddDir(sHomeFBDir,"*."FBMSG_EXT,TRUE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTASCENDING,FALSE);
				if(dir.GetSize()>0){
					// Сохраняем инфу о последнем времени проверки
					if(pParent->WriteStatusInfo()){
						BOOL bAllDeletedOK=TRUE;
						for (int i=0;i<dir.GetSize();i++){
							COleDateTime dt;
							CString sFrom,sRealFrom,sTo,sMessage,sAttach,sFile=dir[i].GetFilePath();
							if(sFile!="" && isFileExist(sFile)){
								CDataXMLSaver xmlFile(sFile,"FB-MESSAGE",TRUE);
								if(xmlFile.IsFileLoaded() && xmlFile.GetSource().GetLength()>0){
									xmlFile.UncryptBody();
									xmlFile.getValue("TimeStamp",dt);
									xmlFile.getValue("to",sTo,"To Unknown");
									xmlFile.getValue("from",sFrom,"From Unknown");
									xmlFile.getValue("message",sMessage);
									xmlFile.getValue("attach",sAttach);
									if(isFileExist(sFile)){
										if(!::DeleteFile(sFile)){
											bAllDeletedOK=FALSE;
										}
									}
									sRealFrom=GetFBIdFromFile(sFile);// Так как терь все шифруется и имя файла больше не показатель...
									sFrom+="/fb";//Показываем что FBшное сообщение
									if(sRealFrom==""){
										sRealFrom=sFrom;
									}
									OpenMessage(sFrom,sTo,sMessage,"",sAttach);
								}
							}
						}
						if(!bAllDeletedOK){
							AlertProtOff(_l("Can`t clean up shared folder")+"!",pParent);
							Alert(_l("Warning: can`t delete message from shared folder")+"!\n"+_l("Shared folder must be accessible both for read and write in the same time")+"!",_l("Billboard error"));
						}else{
							AlertProtOff();
						}
					}
				}
			}
			NotifyAboutSharedItem("",0);
		}
		if(pParent->dwSleepTime>objSettings.dwMidFBPeriod*1000 || pParent->dwSleepTime==0){
			pParent->dwSleepTime=objSettings.dwMidFBPeriod*1000;
		}
		WaitForSingleObject(pParent->hWaitEvent,pParent->dwSleepTime);
		if(pParent->dwSleepTime<objSettings.dwMidFBPeriod*1000){
			pParent->dwSleepTime+=objSettings.dwMinFBPeriod*1000;
		}
	}
	CloseHandle(pParent->hWaitEvent);
	return 0;
}

DWORD WINAPI FBListener_Try(LPVOID p)
{
	while(true){
		__try{
			FBListener(p);
			break;
		}
		__except(GetExceptionCode()!=0){
		};
	}
	return 0;
}

BOOL CNetFBProtocol::Start()
{
	if(m_sFBDir=="" || GetFBFolder(m_sFB_ID)==""){
		objLog.AddMsgLogLine(DEFAULT_GENERALLOG,_l("Error creating Filebox! Shared dir is not accessible"));
		bStarted=0;
        return FALSE;
	}
	bStarted=1;
	hFBStatusThread = CreateThread( NULL, 0, FBListener_Try, this, 0, &dwFBThreadID);
	return TRUE;
}

CNetFBProtocol::CNetFBProtocol(const char* sFBDir, const char* sFB_ID, const char* sFBNick)
{
	objSettings.lFBDirNorm=0;
	bStarted=0;
	dwFBThreadID=0;
	m_sFBDir=sFBDir;
	m_sFB_ID=sFB_ID;
	m_sFBNick=sFBNick;
	hFBStatusThread=NULL;
	hWaitEvent=::CreateEvent(NULL,FALSE,FALSE,"");
	dwSleepTime=0;
}

CNetFBProtocol::~CNetFBProtocol()
{
	bStarted=0;
	WriteStatusInfo();
	SetEvent(hWaitEvent);
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus<0){
		TerminateThread(hFBStatusThread,99);
	}else if(WaitForSingleObject(hFBStatusThread,objSettings.dwNetMessagingTimeout)!=WAIT_OBJECT_0){
		if(hFBStatusThread){
			TerminateThread(hFBStatusThread,98);
		}
	}else{
		CloseHandle(hFBStatusThread);
	}
}

BOOL CNetFBProtocol::SendNet_Text(const char* szRecipientIn, CNetMessage_Text* str)
{
	CString szRecipient=szRecipientIn;
	if(objSettings.bNoSend){
		Alert(_l(NO_SEND_BODY),_l(NO_SEND_TITLE));
		return 0;
	}
	if(bStarted==0){
		Alert(_l(DEFAULT_NOPROTWARN),5000,FALSE,DT_CENTER|DT_VCENTER);
		return FALSE;
	}
	str->sMessageAttach.TrimRight();
	CString sTargetDir,sTargetKatalog;
	BOOL iValidCount=0;
	while(iValidCount<3){
		sTargetDir=GetFBFolder(szRecipient);
		sTargetKatalog=GetDirectoryFromPath(sTargetDir);
		if(!isFileExist(sTargetKatalog)){
			// Ищем...
			COnlineUsers aUsers;
			FindOnlineUsers(szRecipient,aUsers);
			if(aUsers.GetSize()==1){
				szRecipient=aUsers[0].sID;
			}else{
				if(aUsers.GetSize()>1){
					str->sError=_l("There are more than\none user with this nickname.\nUse 'Find users' to look\nat the possible recipients");
				}else{
					str->sError=_l("Recipient not found.\nUse 'Find users' to look\nat the possible recipients");
				}
				return FALSE;
			}
		}else{
			break;
		}
		iValidCount++;
	}
	CString sFile=sTargetDir+GenerateNewId(HashedValue(m_sFB_ID,5),Format("%03i",objSettings.bFBMessageCount))+"."+FBMSG_EXT;
	objSettings.bFBMessageCount=BYTE(objSettings.bFBMessageCount+1);
	CString sFrom=m_sFB_ID;
	CString sMessage=str->sMessageBody;
	BOOL bRes=0;
	{
		CDataXMLSaver xmlFile(sFile,"FB-MESSAGE",FALSE);
		ExtractSenderFromText(sFrom, sMessage);
		xmlFile.putValue("TimeStamp",COleDateTime::GetCurrentTime());
		xmlFile.putValue("to",szRecipient);
		xmlFile.putValue("from",sFrom);
		xmlFile.putValue("message",sMessage);
		xmlFile.putValue("attach",str->sMessageAttach);
		if(objSettings.lFBCrypt){
			xmlFile.CryptBody();
		}
		bRes=xmlFile.SaveDataIntoSource();
	}
	if(!bRes){
		str->sError=_l("Recipient unavailable or shared folder is not accessible");
		return FALSE;
	}
	// Устанавливаем режим повышенной частоты проверки ответа
	dwSleepTime=objSettings.dwMinFBPeriod*1000;
	// И будим поток
	SetEvent(hWaitEvent);
	return bRes;
}

BOOL CNetFBProtocol::SendNet_StatusExch(const char* szRecipient,const char* szProtocol)
{
	if(m_sFBDir=="" || !isFileExist(m_sFBDir)){
		int iPerson=objSettings.AddrBook.FindPersonByIP(szRecipient,TRUE);
		if(iPerson!=-1){
			objSettings.AddrBook.aPersons[iPerson]->iOnlineStatus=4;
			RefreshUserStatusIcon(iPerson);
		}
		return TRUE;
	}
	WriteStatusInfo();
	return ReadStatusInfo(szRecipient);
}

BOOL CNetFBProtocol::FindOnlineUsers(const char* m_Mask, COnlineUsers& aUsers)
{
	if(m_sFBDir=="" || !isFileExist(m_sFBDir)){
		return FALSE;
	}
	CFileInfoArray dir;
	dir.AddDir(m_sFBDir,FBUSER_PR"*.*",FALSE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTASCENDING,TRUE);
	if(dir.GetSize()>0){
		for(int i=0;i<dir.GetSize();i++){
			COleDateTime dt;
			CFindedUser user;
			CString sUserData;
			user.sID=GetFBIdFromFile(dir[i].GetFilePath()+"\\");
			if(user.sID!="" && ReadStatusInfo(user.sID,&sUserData,&user.sNick,&dt)){
				BOOL bOk=TRUE;
				if(m_Mask && strlen(m_Mask)>0 && strcmp(m_Mask,"*")!=0){
					bOk=PatternMatchIC(user.sNick, m_Mask) || PatternMatchIC(user.sID, m_Mask);
				}
				if(bOk ){
					if(user.sID==m_sFB_ID){
						user.sNick+=Format(" (%s)",_l("You"));
					}
					user.sID+="/fb";
					user.iStatus=GetUserStatusFromStatusStringID(sUserData);
					if(dt.GetStatus()==COleDateTime::valid){
						user.sUserDsc=dt.Format("%d.%m.%Y %H:%M:%S");
					}
					aUsers.Add(user);
				}
			}
		}
	}
	return TRUE;
}
