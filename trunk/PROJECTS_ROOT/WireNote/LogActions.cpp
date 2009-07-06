// LogActions.cpp: implementation of the CLogActions class.
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogActions::CLogActions()
{
	// Приходится считывать счас, так как в деструкторе может уже быть сброшен кеш
	LANG_LogFile=_l("Log file");
	return;
}

CLogActions::~CLogActions()
{
	FlushAllToDisk();
	CSmartLock SL(&cs,TRUE);
	CString sKey;
	void* pVoid=NULL;
	CString* pBuffer=NULL;
	POSITION pos=aLogBuffer.GetStartPosition();
	while(pos!=NULL){
		aLogBuffer.GetNextAssoc(pos,sKey,pVoid);
		pBuffer=(CString*)pVoid;
		delete pBuffer;
	}
	aLogBuffer.RemoveAll();
	return;
}

CString CLogActions::GetLogFileName(const char* szLogName)
{
	CString sLogPath=getFileFullName(objSettings.sMessageLogDirectory,TRUE);
	if(!objSettings.SaveMesNotesByDefForUser || strcmp(szLogName,DEFAULT_GENERALLOG)==0){
		// Если нужен общий лог или поюзерного лога нет совсем...
		return sLogPath+DEFAULT_GENERALLOG;
	}
	// Удаляем неверные символы
	CString sGoodLogName=szLogName;
	MakeSafeFileName(sGoodLogName);
	return sLogPath+sGoodLogName+DEFAULT_PERSONLOG;
}

void CLogActions::FlushAllToDisk()
{
	CSmartLock SL(&cs,TRUE);
	CString sKey;
	void* pVoid=NULL;
	POSITION pos=aLogBuffer.GetStartPosition();
	while(pos!=NULL){
		aLogBuffer.GetNextAssoc(pos,sKey,pVoid);
		if(pVoid!=NULL){
			CString sFileContent=*((CString*)pVoid);
			FILE* m_logFile=getFile(sKey+DEFAULT_LOGEXT,"wb");
			if(m_logFile!=NULL){
				//fprintf(m_logFile,"<HTML><HEAD><TITLE>");
				fprintf(m_logFile,"%s - '%s'",LANG_LogFile,sKey);
				//fprintf(m_logFile,"</TITLE></HEAD><BODY bgcolor=#EEEEEE text=#000000>");
				fprintf(m_logFile,"\r\n\r\n");
				fprintf(m_logFile,HTMLCUTLINE1);
				fprintf(m_logFile,"%s",sFileContent);
				fprintf(m_logFile,HTMLCUTLINE2);
				//fprintf(m_logFile,"</BODY>");
				fclose(m_logFile);
			}
		}
	}
}

CString* CLogActions::ReadFromDisk(const char* szFile)
{
	CSmartLock SL(&cs,TRUE);
	void* pVoid=NULL;
	aLogBuffer.Lookup(szFile,pVoid);
	CString* pBuffer=(CString*)pVoid;
	if(pBuffer!=NULL){
		aLogBuffer.SetAt(szFile,NULL);
		delete pBuffer;
		pBuffer=NULL;
	}
	pBuffer=new CString;
	ReadFile(CString(szFile)+DEFAULT_LOGEXT,*pBuffer);
	int iPos1=pBuffer->Find(HTMLCUTLINE1);
	if(iPos1!=-1){
		*pBuffer=pBuffer->Mid(iPos1+sizeof(HTMLCUTLINE1));
	}
	int iPos2=pBuffer->Find(HTMLCUTLINE2);
	if(iPos2!=-1){
		*pBuffer=pBuffer->Left(iPos2);
	}
	aLogBuffer.SetAt(szFile,pBuffer);
	return pBuffer;
}

void CLogActions::AddLogSimpleLine(const char* szFile, const char* sString, DWORD dwMaxLogSize)
{
	CSmartLock SL(&cs,TRUE);
	void* pVoid=NULL;
	CString* pBuffer=NULL;
	if(aLogBuffer.Lookup(szFile,pVoid)){
		pBuffer=(CString*)pVoid;
	}else{
		pBuffer=ReadFromDisk(szFile);
	}
	if(!pBuffer){
		return;
	}
	DWORD dwMaxSize=((dwMaxLogSize==0?objSettings.dwMaxLogFileSize:dwMaxLogSize)*1024);
	CString sNewContent=sString;
	sNewContent+=*pBuffer;
	if(strlen(sNewContent)>dwMaxSize){
		long lCutPos=sNewContent.Find(HTMLCUTLINE3,dwMaxSize);
		if(lCutPos!=-1){
			sNewContent=sNewContent.Left(lCutPos);
		}
	}
	*pBuffer=sNewContent;
	return;
}

void CLogActions::AddLogLine(const char* szFile, const char *sBuffer, DWORD dwMaxLogSize, const char* szPostLineString)
{
	CString sXmledString=sBuffer;
	//CDataXMLSaver::Str2Xml(sXmledString);
	COleDateTime dtNow=COleDateTime::GetCurrentTime();
	CString sString=Format(HTMLCUTLINE3"%02lu.%02lu.%04lu %02lu:%02lu.%02lu\t",dtNow.GetDay(),dtNow.GetMonth(),dtNow.GetYear(),dtNow.GetHour(),dtNow.GetMinute(),dtNow.GetSecond());
	sString+=sXmledString;
	sString+=szPostLineString;
	AddLogSimpleLine(szFile, sString, dwMaxLogSize);
}

void CLogActions::AddMsgLogLineRaw(const char* szTargetLog,const char *sBuffer, const char *sBuffer2)
{
	if(objSettings.SaveMesNotesByDef){
		AddLogLine(GetLogFileName(DEFAULT_GENERALLOG), sBuffer, objSettings.dwMaxLogFileSize, sBuffer2);
		if(objSettings.SaveMesNotesByDefForUser && strcmp(szTargetLog,DEFAULT_GENERALLOG)!=0){
			AddLogLine(GetLogFileName(szTargetLog), sBuffer, objSettings.dwMaxLogFileSize, sBuffer2);
		}
	}
}

void CLogActions::AddMsgLogLine(const char* szTargetLog,const char *szFormat, ...)
{
	if(objSettings.SaveMesNotesByDef){
		va_list vaList;
		va_start(vaList,szFormat);
		CString sBuffer;
		sBuffer.FormatV(szFormat,vaList);
		va_end (vaList);
		AddLogLine(GetLogFileName(DEFAULT_GENERALLOG), sBuffer, objSettings.dwMaxLogFileSize);
		if(objSettings.SaveMesNotesByDefForUser && strcmp(szTargetLog,DEFAULT_GENERALLOG)!=0){
			AddLogLine(GetLogFileName(szTargetLog), sBuffer, objSettings.dwMaxLogFileSize);
		}
	}
}

void CLogActions::AddMailLogLine(const char *szFormat, ...)
{
	va_list vaList;
	va_start(vaList,szFormat);
	CString sBuffer;
	sBuffer.FormatV(szFormat,vaList);
	va_end (vaList);
	AddLogLine(GetLogFileName(DEFAULT_ANTISPAMLOG), sBuffer, objSettings.dwMaxMailLogFileSize);
}