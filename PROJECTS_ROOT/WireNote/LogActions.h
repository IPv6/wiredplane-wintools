// LogActions.h: interface for the CLogActions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGACTIONS_H__692A849E_6BC5_47D8_A7D5_83C6C5DA4B92__INCLUDED_)
#define AFX_LOGACTIONS_H__692A849E_6BC5_47D8_A7D5_83C6C5DA4B92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HTMLCUTLINE1	"\r\n[BEGIN]"
#define HTMLCUTLINE2	"\r\n[END]\r\n"
#define HTMLCUTLINE3	"\r\n----\r\n"
class CLogActions  
{
public:
	CString LANG_LogFile;
	CString GetLogFileName(const char* szLogName);
	CCriticalSection cs;
	CMapStringToPtr aLogBuffer;
	CString* ReadFromDisk(const char* szFile);
	CLogActions();
	virtual ~CLogActions();
	void FlushAllToDisk();
	void AddLogSimpleLine(const char* szFile, const char* sString, DWORD dwMaxLogSize=0);
	void AddLogLine(const char* szFile, const char *sBuffer, DWORD dwMaxLogSize=0, const char* szPostLineString=NULL);
	//--------------------------------------
	void AddMailLogLine(const char *szFormat, ...);
	void AddMsgLogLine(const char* szTargetLog,const char *szFormat, ...);
	void AddMsgLogLineRaw(const char* szTargetLog,const char *sBuffer, const char *sBuffer2=NULL);
};

#endif // !defined(AFX_LOGACTIONS_H__692A849E_6BC5_47D8_A7D5_83C6C5DA4B92__INCLUDED_)
