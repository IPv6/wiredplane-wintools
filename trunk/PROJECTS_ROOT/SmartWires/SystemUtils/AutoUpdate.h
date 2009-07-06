#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H
#include "InternetUtils.h"
#include "..\GUIClasses\systemtr.h"
class CNewVersionChecker
{
	CString m_sApp;
	CString m_sAppTitle;
	CString m_sSize;
	CString m_sDateCurrent;
	CString m_sDateAvailable;
	CString m_sVerAvailable;
	CString m_sVerCurrent;
	CString m_sCompTime;
	CString m_sAddData;
	CString m_sNewInfo;
	CString m_sMoreInfo;
	CString m_sCustomDownloadUrl;
	DWORD m_dwNotifyIcon;
	CSystemTray* m_STray;
public:
	void SetTraits(const char* szMoreInfo=0, DWORD dwNotifyIcon=0);
	CNewVersionChecker(const char* szCurVer, const char* szCompTime, const char* szAddData=NULL);
	BOOL IsNewVersionAvailable();
	BOOL CheckAndDownloadIfNeeded(DWORD dwDays);
	BOOL GetDateFromAppFile(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear);
	BOOL DownloadFileVersionFromSite(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear);
	BOOL GetDateFromString(CString sDate,DWORD& dwDay,DWORD& dwMon,DWORD& dwYear);
	BOOL GetCurVersionDate(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear);
	BOOL GetLastCheckDate(DWORD& dwDay,DWORD& dwMon,DWORD& dwYear);
	BOOL SetLastCheckDate(DWORD dwDay,DWORD dwMon,DWORD dwYear);
	BOOL StartDownloading();
	BOOL AskUserOpinionToStartDownload();
	BOOL AskUserOpinionToStartCheck(DWORD dwDays);
};

BOOL IsUpdateInProgress();
void StartCheckNewVersion(DWORD dwDays);
void _cdecl CheckNewVersion(LPVOID pValue);
#endif
