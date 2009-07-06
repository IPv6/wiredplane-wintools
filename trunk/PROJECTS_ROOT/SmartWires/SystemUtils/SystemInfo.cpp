//#include "stdafx.h"
#include "SystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCriticalSection pCahLock;
_ProcEnum& GetProccessCash(BOOL bLock)
{
	static CCriticalSection cs;
	static _ProcEnum aProcesses;
	if(bLock==0){
		cs.Unlock();
	}
	if(bLock==1){
		cs.Lock();
	}
	return aProcesses;
}

void GetProcessInfo(DWORD dwProcId, CProcInfo*& pInfo)
{
	pInfo=NULL;
	CCSLock lpc(&pCahLock,1);
	int iSize=GetProccessCash().GetSize();
	for(int i=0; i<iSize;i++){
		DWORD dwCurProcId=GetProccessCash()[i].dwProcId;
		if(dwProcId==dwCurProcId){
			pInfo=&(GetProccessCash()[i]);
			break;
		}
	}
	return;
}

	
typedef DWORD (WINAPI *PNtQueryObject)( HANDLE, DWORD, VOID*, DWORD, VOID* );
typedef DWORD (WINAPI *PNtQuerySystemInformation)( DWORD, VOID*, DWORD, ULONG* );
typedef DWORD (WINAPI *PNtQueryInformationThread)(HANDLE, ULONG, PVOID,	DWORD, DWORD* );
typedef DWORD (WINAPI *PNtQueryInformationFile)(HANDLE, PVOID,	PVOID, DWORD, DWORD );
typedef DWORD (WINAPI *PNtQueryInformationProcess)(HANDLE, DWORD, PVOID, DWORD, PVOID );
class CNTZwWrap
{
public:
	BOOL bIsRealNT;
    HINSTANCE	hNtDll;
	PNtQuerySystemInformation	NtQuerySystemInformation;
	PNtQueryObject				NtQueryObject;
	PNtQueryInformationThread	NtQueryInformationThread;
	PNtQueryInformationFile		NtQueryInformationFile;
	PNtQueryInformationProcess	NtQueryInformationProcess;

    NTSTATUS	(WINAPI * _ZwQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);
	PDEBUG_BUFFER (WINAPI * _RtlCreateQueryDebugBuffer)(ULONG,BOOLEAN);
	NTSTATUS	(WINAPI * _RtlDestroyQueryDebugBuffer)(PDEBUG_BUFFER);
	NTSTATUS	(WINAPI * _RtlQueryProcessDebugInformation)(ULONG,ULONG,PDEBUG_BUFFER);
	NTSTATUS	(WINAPI * _NtQueryInformationProcess)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation,ULONG ProcessInformationLength,PULONG ReturnLength);
	NTSTATUS	(WINAPI * _GetProcessId)(HANDLE Process);

	CNTZwWrap()
	{
		bIsRealNT=1;
		hNtDll = 0;
		_ZwQuerySystemInformation = 0;
		_RtlCreateQueryDebugBuffer = 0;
		_RtlDestroyQueryDebugBuffer = 0;
		_RtlQueryProcessDebugInformation = 0;
		_NtQueryInformationProcess = 0;
		_GetProcessId = 0;
		// получаем описатель NTDLL.DLL
		hNtDll = LoadLibrary(_T("ntdll.dll"));//GetModuleHandle(_T("ntdll.dll"));
		if(!hNtDll){
			AlertErr();
			return;
		}

		// находим адрес ZwQuerySystemInformation
		*(FARPROC *)&_ZwQuerySystemInformation = DBG_GETPROC(hNtDll, "ZwQuerySystemInformation");
		if (_ZwQuerySystemInformation == NULL){
			AlertErr();
			return;
		}
		
		*(FARPROC *)&_RtlCreateQueryDebugBuffer = DBG_GETPROC(hNtDll, "RtlCreateQueryDebugBuffer");
		if (_RtlCreateQueryDebugBuffer == NULL){
			AlertErr();
			return;
		}
		
		*(FARPROC *)&_RtlDestroyQueryDebugBuffer = DBG_GETPROC(hNtDll, "RtlDestroyQueryDebugBuffer");
		if (_RtlDestroyQueryDebugBuffer == NULL){
			AlertErr();
			return;
		}
		
		*(FARPROC *)&_RtlQueryProcessDebugInformation = DBG_GETPROC(hNtDll, "RtlQueryProcessDebugInformation");
		if (_RtlQueryProcessDebugInformation == NULL){
			AlertErr();
			return;
		}
		
		// Get the NtDll function pointers
		NtQuerySystemInformation = (PNtQuerySystemInformation)
			DBG_GETPROC( GetModuleHandle( _T( "ntdll.dll" ) ),
			_T("NtQuerySystemInformation") );
		
		NtQueryObject = (PNtQueryObject)
			DBG_GETPROC(	GetModuleHandle( _T( "ntdll.dll" ) ),
			_T("NtQueryObject") );
		
		NtQueryInformationThread = (PNtQueryInformationThread)
			DBG_GETPROC(	GetModuleHandle( _T( "ntdll.dll" ) ),
			_T("NtQueryInformationThread") );
		
		NtQueryInformationFile = (PNtQueryInformationFile)
			DBG_GETPROC(	GetModuleHandle( _T( "ntdll.dll" ) ),
			_T("NtQueryInformationFile") );
		
		NtQueryInformationProcess = (PNtQueryInformationProcess)
			DBG_GETPROC(	GetModuleHandle( _T( "ntdll.dll" ) ),
			_T("NtQueryInformationProcess") );
		
		*(FARPROC *)&_NtQueryInformationProcess = DBG_GETPROC(hNtDll, "NtQueryInformationProcess");
		*(FARPROC *)&_GetProcessId = DBG_GETPROC(hNtDll, "GetProcessId");
		//
	};

	void AlertErr()
	{
		bIsRealNT=0;
		//AfxMessageBox("Warning:\r\nYou must work under Windows NT OS family\r\nin order to use some system features!");
	}

	~CNTZwWrap()
	{
		if(hNtDll){
			FreeLibrary(hNtDll);
			hNtDll = 0;
		}
	}
};

CNTZwWrap& getNTWrap()
{
	static CNTZwWrap wrap;
	return wrap;
}

class CPsApiWrap
{
public:
	DWORD (WINAPI * _GetModuleFileNameEx)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);
	BOOL (WINAPI * _EnumProcessModules)(HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded);
	HINSTANCE hPsApiDll;
	CPsApiWrap()
	{
		hPsApiDll = LoadLibrary(_T("psapi.dll"));
		if(hPsApiDll){
#ifdef UNICODE
			*(FARPROC *)&_GetModuleFileNameEx = DBG_GETPROC(hPsApiDll, "GetModuleFileNameExW");
#else
			*(FARPROC *)&_GetModuleFileNameEx = DBG_GETPROC(hPsApiDll, "GetModuleFileNameExA");
#endif
			*(FARPROC *)&_EnumProcessModules = DBG_GETPROC(hPsApiDll, "EnumProcessModules");
		}
	};

	~CPsApiWrap()
	{
		if(hPsApiDll){
			FreeLibrary(hPsApiDll);
			hPsApiDll = 0;
		}
	}
};

CPsApiWrap& getPsApiWrap()
{
	static CPsApiWrap wrap;
	return wrap;
}


typedef VOID (WINAPI *_GetStartupInfo)(STARTUPINFO*);
typedef LPSTR (WINAPI *_GetCommandLine)();
struct StartupInfoPars
{
	_GetStartupInfo f_GetStartupInfo;
	_GetCommandLine f_GetCommandLine;
	STARTUPINFO pStartupInfo;
	LPSTR pCmdLine;
};

DWORD WINAPI GetStartupParams(LPVOID pData)
{
	StartupInfoPars* pInf=(StartupInfoPars*)pData;
	if(pInf!=NULL && pInf->f_GetStartupInfo){
		memset(&pInf->pStartupInfo,0,sizeof(STARTUPINFO));
		pInf->pStartupInfo.cb=sizeof(STARTUPINFO);
		pInf->f_GetStartupInfo(&pInf->pStartupInfo);
	}
	if(pInf!=NULL && pInf->f_GetCommandLine){
		pInf->pCmdLine=pInf->f_GetCommandLine();
	}
	return 0;
}

#define MAX_CMDLINE	1024
BOOL GetProcessComLine(DWORD dwProcID, CString& sCmdLine)
{
	sCmdLine="<not available>";
	if(getNTWrap().bIsRealNT==0){
		return FALSE;
	}
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION,FALSE,dwProcID);
	if(hProcess == NULL){
		return FALSE;
	}
	HINSTANCE hKernel32=LoadLibrary("kernel32.dll");
	LPTHREAD_START_ROUTINE fp=(LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "GetCommandLineA");
	if(fp){
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, fp, NULL, 0, NULL);
		WaitForSingleObject(hThread,INFINITE);
		LPVOID pRemoteComline=NULL;
		if(GetExitCodeThread(hThread,(DWORD*)&pRemoteComline) && pRemoteComline!=0){
			char szBuffer[MAX_CMDLINE]="";
			if(ReadProcessMemory(hProcess,pRemoteComline,szBuffer,sizeof(szBuffer),NULL)){
				sCmdLine=szBuffer;
			}
		}
		CloseHandle(hThread);
	}
	CloseHandle(hProcess);
	FreeLibrary(hKernel32);
	return TRUE;
}

CString GetProcessComLine(HWND hWin)
{
	DWORD dwProcID=0;
	DWORD dwThreadID=GetWindowThreadProcessId(hWin,&dwProcID);
	CString sComLine;
	if(GetProcessComLine(dwProcID, sComLine)){
		return sComLine;
	}
	return "";
}

BOOL GetProcessStartupInfo(DWORD dwProcID,STARTUPINFO* si, CString* sCmdLine)
{// Пока не работает - функцию оказывается нужно в чужой процесс копировать сначала (см. RSDN)
	if(si){
		memset(si,0,sizeof(STARTUPINFO));
	}
	if(getNTWrap().bIsRealNT==0){
		return FALSE;
	}
	HANDLE h = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION,FALSE,dwProcID);
	if(h == NULL){
		return FALSE;
	}
	// Пошли читать!
	StartupInfoPars pData;
	HINSTANCE hMod=LoadLibrary("kernel32.dll");
	pData.f_GetStartupInfo=(_GetStartupInfo)DBG_GETPROC(hMod,"GetStartupInfoA");
	pData.f_GetCommandLine=(_GetCommandLine)DBG_GETPROC(hMod,"GetCommandLineA");
	FreeLibrary(hMod);
	LPVOID pBuf  = VirtualAllocEx(h,NULL,sizeof(StartupInfoPars),MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
	if(pBuf != NULL){
		pData.pCmdLine=NULL;//VirtualQueryEx//VirtualProtectEx
		WriteProcessMemory(h,pBuf,&pData,sizeof(StartupInfoPars),NULL);
		HANDLE h2=CreateRemoteThread(h, NULL, 0, GetStartupParams, LPVOID(pBuf), 0, NULL);
		if(h2!=NULL){
			WaitForSingleObject(h2,INFINITE);
			ReadProcessMemory(h,pBuf,&pData,sizeof(StartupInfoPars),NULL);
			if(si){
				memcpy(si,&pData.pStartupInfo,sizeof(STARTUPINFO));
			}
			if(sCmdLine && pData.pCmdLine){
				char szBuffer[MAX_CMDLINE]="";
				ReadProcessMemory(h,pData.pCmdLine,szBuffer,MAX_CMDLINE,NULL);
				*sCmdLine=szBuffer;
			}
			CloseHandle(h2);
		}
		VirtualFreeEx(h,pBuf,0,MEM_RELEASE);
	}
	CloseHandle(h);
	return TRUE;
}

LARGE_INTEGER GetCPUTimesOnce()
{
	LARGE_INTEGER dwRes;
	dwRes.QuadPart=0;
	if(getNTWrap().bIsRealNT!=0){
		SYSTEM_PROCESSOR_TIMES *CPUNTUsage;
		ULONG size = sizeof(SYSTEM_PROCESSOR_TIMES);
		CPUNTUsage = (SYSTEM_PROCESSOR_TIMES *)malloc(size);
		if (CPUNTUsage) {
			getNTWrap()._ZwQuerySystemInformation(8,CPUNTUsage,size,NULL);
			// The idle time, measured in units of 100-nanoseconds, of the processor.
			dwRes = CPUNTUsage[0].IdleTime;
			free(CPUNTUsage);
		}
	}
	return dwRes;
}

#define CPUUSAGE_CHECKTIME_98	500
class CWin98CPUUsage
{
public:
	CWin98CPUUsage()
	{
		hOpenedCounter=0;
		//EnableDataCollection("KERNEL","CPUUsage");
	};
	~CWin98CPUUsage()
	{
		//DisableDataCollection("KERNEL","CPUUsage");
	};
	HKEY hOpenedCounter;
	// Определение загрузки проца под виндами 9x
	BOOL EnableDataCollection(char *szObjName, char *szCounterName)
	{
		
		HKEY hOpen;
		DWORD cbData;
		DWORD dwType;
		LPBYTE pByte;
		DWORD rc;
		char *szCounterData;
		BOOL bSuccess = TRUE;
		
		if ( (rc = RegOpenKeyEx(HKEY_DYN_DATA,"PerfStats\\StartStat", 0,
			KEY_READ, &hOpen)) == ERROR_SUCCESS)
		{
			// concatenate the object and couter key names
			szCounterData = (char*)LocalAlloc(LPTR,
				lstrlen(szObjName) + lstrlen(szCounterName) + 2);
			wsprintf(szCounterData, "%s\\%s", szObjName, szCounterName);
			
			// query to get data size
			if ( (rc = RegQueryValueEx(hOpen,szCounterData,NULL,&dwType,
				NULL, &cbData )) == ERROR_SUCCESS )
			{
				pByte = (unsigned char*)LocalAlloc(LPTR, cbData);
				
				// query the performance start key to initialize performance data
				// query the start key to initialize performance data
				rc = RegQueryValueEx(hOpen,szCounterData,NULL,&dwType, pByte,
					&cbData );
				// at this point we don't do anything with the data
				//  free up resources
				LocalFree(pByte);
			}
			else
				bSuccess = FALSE;
			
			RegCloseKey(hOpen);
			LocalFree(szCounterData);
		}
		else
			bSuccess = FALSE;
		//------------------
		// concatenate the object and counter keys
		szCounterData = (char*)LocalAlloc(LPTR,
			lstrlen(szObjName) + lstrlen(szCounterName) + 2);
		wsprintf(szCounterData, "%s\\%s", szObjName, szCounterName);
		
		// open performance data key
		RegOpenKeyEx(HKEY_DYN_DATA,"PerfStats\\StatData", 0,	KEY_READ, &hOpenedCounter);
		
		return bSuccess;
		
	}
				
	BOOL CollectPerfData(char *szObjName, char *szCounterName, DWORD *pdwData)
	{
		EnableDataCollection("KERNEL","CPUUsage");
		Sleep(CPUUSAGE_CHECKTIME_98);
		
		DWORD dwType;
		DWORD cbData;
		char *szCounterData;
		DWORD rc;
		BOOL bSuccess = TRUE;
		
		// concatenate the object and counter keys
		szCounterData = (char*)LocalAlloc(LPTR,
			lstrlen(szObjName) + lstrlen(szCounterName) + 2);
		wsprintf(szCounterData, "%s\\%s", szObjName, szCounterName);
		
		cbData = sizeof(DWORD);
		// retrieve performance data which is of size of DWORD
		rc=RegQueryValueEx(hOpenedCounter,szCounterData,0,&dwType,(LPBYTE)pdwData,&cbData );
		// let the caller know if it worked or not
		bSuccess = rc == ERROR_SUCCESS;
		// free the resources
		LocalFree(szCounterData);
		DisableDataCollection("KERNEL","CPUUsage");
		return bSuccess;
	}
				
	BOOL DisableDataCollection(char *szObjName, char *szCounterName)
	{
		
		HKEY hOpen;
		LPBYTE pByte;
		DWORD cbData;
		DWORD dwType;
		DWORD rc;
		char *szCounterData;
		BOOL bSuccess = TRUE;
		RegCloseKey(hOpenedCounter);
		if ( (rc = RegOpenKeyEx(HKEY_DYN_DATA,"PerfStats\\StopStat", 0,
			KEY_READ, &hOpen)) == ERROR_SUCCESS)
		{
			// concatenate the object and couter key names
			szCounterData = (char*)LocalAlloc(LPTR,
				lstrlen(szObjName) + lstrlen(szCounterName) + 2);
			wsprintf(szCounterData, "%s\\%s", szObjName, szCounterName);
			
			// query to get data size
			if ( (rc = RegQueryValueEx(hOpen,szCounterData,NULL,&dwType,
				NULL, &cbData )) == ERROR_SUCCESS )
			{
				pByte = (unsigned char*)LocalAlloc(LPTR, cbData);
				
				// query the performance start key to initialize performance data
				// query the start key to initialize performance data
				rc = RegQueryValueEx(hOpen,szCounterData,NULL,&dwType, pByte,
					&cbData );
				// at this point we don't do anything with the data
				// free up resources
				LocalFree(pByte);
				
			}
			else
				bSuccess = FALSE;
			
			RegCloseKey(hOpen);
			LocalFree(szCounterData);
		}
		else
			bSuccess = FALSE;
		
		return bSuccess;
		
	}
};

CWin98CPUUsage& GetWin98CPUUsage()
{
	static CWin98CPUUsage w98CPU;
	return w98CPU;
};

double GetMemoryPercents()
{
	MEMORYSTATUS memstat;
	memset(&memstat,0,sizeof(memstat));
	GlobalMemoryStatus(&memstat);
	double dPercent=100*double(memstat.dwAvailPhys)/double(memstat.dwTotalPhys);
	return dPercent;
}

#define CPUUSAGE_CHECKTIME		300
double GetCPUTimesPercents()
{
	if(getNTWrap().bIsRealNT==0){
		DWORD dwBuff=0;
		if(!GetWin98CPUUsage().CollectPerfData("KERNEL","CPUUsage",&dwBuff)){
			return 0;
		}
		return double(dwBuff);
	}
	LARGE_INTEGER lRes1=GetCPUTimesOnce();
	if(lRes1.QuadPart==0){
		return 0;
	}
	Sleep(CPUUSAGE_CHECKTIME);
	LARGE_INTEGER lRes2=GetCPUTimesOnce();
	return 100-double(lRes2.QuadPart-lRes1.QuadPart)/(CPUUSAGE_CHECKTIME*100);
}

CString GetExeFromHwnd(HWND hWin)
{
	if(!hWin){
		return "";
	}
	DWORD dwProcID=0;
	GetWindowThreadProcessId(hWin,&dwProcID);
	return GetExeFromHwnd(dwProcID);
}

CString GetExeFromHwnd(DWORD dwProcID)
{
	if(getPsApiWrap()._GetModuleFileNameEx && getPsApiWrap()._EnumProcessModules){
		// Ищем первый модуль...
		HANDLE hProc=::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwProcID);
		HMODULE lModule;
		DWORD dwModulesSize=0;
		memset(&lModule,0,sizeof(lModule));
		if(getPsApiWrap()._EnumProcessModules(hProc,&lModule,sizeof(lModule),&dwModulesSize)){
			char szExePath[MAX_PATH]="";
			getPsApiWrap()._GetModuleFileNameEx(hProc,lModule,szExePath,sizeof(szExePath));
			return szExePath;
		}
	}else{
		// 98ые
		CProcInfo* info=NULL;
		EnumProcesses();
		GetProcessInfo(dwProcID,info);
		if(info){
			return info->sProcExePath;
		}
	}
	return "";
}

DWORD GetPagesSize(DWORD PID)
{
	MEMORY_BASIC_INFORMATION myMBI; 
	BOOL BAddrOK=0,FreeOK=0,ReservedOK=0;
	DWORD ThisValueWillBeReturned=0;
	HANDLE p=OpenProcess(PROCESS_ALL_ACCESS,false,PID);
	for (DWORD CountForPages=0;VirtualQueryEx(p,(LPCVOID) (CountForPages+1),&myMBI,sizeof(MEMORY_BASIC_INFORMATION));CountForPages=(DWORD)myMBI.BaseAddress + myMBI.RegionSize + 1)
	{ //for pages
		BAddrOK=((DWORD)myMBI.RegionSize);
		FreeOK= (myMBI.State != MEM_FREE);
		ReservedOK=(myMBI.State != MEM_RESERVE);
		if ( BAddrOK && FreeOK && ReservedOK){ 
			ThisValueWillBeReturned += myMBI.RegionSize; 
		}
	} 
	return ThisValueWillBeReturned;
}


#include <tlhelp32.h>
BOOL WINAPI EnumProcesses_98Api()
{
	CCSLock lpc(&pCahLock,1);
    HINSTANCE hKernel;
    HANDLE (WINAPI *_CreateToolhelp32Snapshot)(DWORD a, DWORD b);
    BOOL (WINAPI *_Process32First)(HANDLE h, PROCESSENTRY32 * e);
    BOOL (WINAPI *_Process32Next)(HANDLE h, PROCESSENTRY32 * e);
	
    // получаем описатель KERNEL32.DLL
    hKernel = GetModuleHandle(_T("kernel32.dll"));
    if(hKernel == NULL){
		return FALSE;
	}
	
    // находим необходимые функции в KERNEL32.DLL
    *(FARPROC *)&_CreateToolhelp32Snapshot = DBG_GETPROC(hKernel, "CreateToolhelp32Snapshot");
#ifdef UNICODE
    *(FARPROC *)&_Process32First = DBG_GETPROC(hKernel, "Process32FirstW");
    *(FARPROC *)&_Process32Next = DBG_GETPROC(hKernel, "Process32NextW");
#else
    *(FARPROC *)&_Process32First = DBG_GETPROC(hKernel, "Process32First");
    *(FARPROC *)&_Process32Next = DBG_GETPROC(hKernel, "Process32Next");
#endif
	
    if (_CreateToolhelp32Snapshot == NULL ||
        _Process32First == NULL ||
        _Process32Next == NULL)
        return FALSE;
	
    HANDLE hSnapshot;
    PROCESSENTRY32 Entry;
	
    // создаем моментальный снимок
    hSnapshot = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return FALSE;
	
    // получаем информацию о первом процессе
    Entry.dwSize = sizeof(Entry);
    if (!_Process32First(hSnapshot, &Entry))
        return FALSE;
	
    // перечисляем остальные процессы
    do{
        LPTSTR pszProcessName = NULL;
		
        if (Entry.dwSize > offsetof(PROCESSENTRY32, szExeFile))
        {
            pszProcessName = _tcsrchr(Entry.szExeFile, _T('\\'));
            if (pszProcessName == NULL){
                pszProcessName = Entry.szExeFile;
			}else{
				pszProcessName++;
			}
        }
		CProcInfo proc;
		proc.dwProcId=Entry.th32ProcessID;
		proc.sProcName=pszProcessName;
		proc.sProcExePath=Entry.szExeFile;
		proc.lWorkingSet=0;//GetPagesSize(proc.dwProcId);
		proc.bTerminable=TRUE;//Убивать можно всегда
		/*HANDLE hProc=::OpenProcess(PROCESS_TERMINATE,FALSE,proc.dwProcId);//|PROCESS_QUERY_INFORMATION|PROCESS_VM_READ
		if(hProc){
			proc.bTerminable=TRUE;
			CloseHandle(hProc);
		}else{
			proc.bTerminable=FALSE;
		}*/
		GetProccessCash().Add(proc);
        Entry.dwSize = sizeof(Entry);
    }
    while (_Process32Next(hSnapshot, &Entry));
    CloseHandle(hSnapshot);
	return TRUE;
}

CString FindModulePath(PDEBUG_BUFFER db, const char* szModuleName)
{
	ULONG n = db->ModuleInformation ? *PULONG(db->ModuleInformation) : 0;
	PDEBUG_MODULE_INFORMATION p	= PDEBUG_MODULE_INFORMATION(PULONG(db->ModuleInformation) + 1);
	for (ULONG i = 0; i < n; i++) {
		if(stricmp(p[i].ImageName + p[i].ModuleNameOffset,szModuleName)==0){
			return p[i].ImageName;
		}
	}
	return "";
}

BOOL WINAPI EnumProcesses_NtApi()
{
	EnableDebugPriv();
    // получаем описатель кучи процесса по умолчанию
    HANDLE hHeap = GetProcessHeap();
    NTSTATUS Status;
    ULONG cbBuffer = 0x8000;
    PVOID pBuffer = NULL;
	CCSLock lpc(&pCahLock,1);
    // трудно заранее определить, какой размер выходного
    // буфера будет достаточным, поэтому мы начинам с буфера
    // размером 32K и увеличиваем его по необходимости
    do{
        pBuffer = HeapAlloc(hHeap, 0, cbBuffer);
        if (pBuffer == NULL)
            return SetLastError(ERROR_NOT_ENOUGH_MEMORY), FALSE;
		
        Status = getNTWrap()._ZwQuerySystemInformation(
			SystemProcessesAndThreadsInformation,
			pBuffer, cbBuffer, NULL);
		
        if (Status == STATUS_INFO_LENGTH_MISMATCH){
            HeapFree(hHeap, 0, pBuffer);
            cbBuffer *= 2;
        }else if (!NT_SUCCESS(Status))
        {
            HeapFree(hHeap, 0, pBuffer);
            return SetLastError(Status), FALSE;
        }
    }while (Status == STATUS_INFO_LENGTH_MISMATCH);
    PSYSTEM_PROCESSES pProcesses = (PSYSTEM_PROCESSES)pBuffer;
    for (;;){
        PCWSTR pszProcessName = pProcesses->ProcessName.Buffer;
        if (pszProcessName == NULL){
            pszProcessName = L"Idle";
		}
		CHAR szProcessName[MAX_PATH];
#ifdef UNICODE
		strcpy(szProcessName,pszProcessName);
#else
        WideCharToMultiByte(CP_ACP, 0, pszProcessName, -1,
			szProcessName, MAX_PATH, NULL, NULL);
#endif
		CProcInfo proc;
		proc.dwProcId=pProcesses->ProcessId;
		proc.sProcName=szProcessName;
		proc.lWorkingSet=pProcesses->VmCounters.WorkingSetSize;
		proc.KernelTime=pProcesses->KernelTime.QuadPart;
		proc.UserTime=pProcesses->UserTime.QuadPart;
		proc.CreateTime=pProcesses->CreateTime.QuadPart;
		proc.lProcent=0;
		// Путь к exe-файлу
		proc.sProcExePath="";
		if(getPsApiWrap()._GetModuleFileNameEx && getPsApiWrap()._EnumProcessModules){
			// Ищем первый модуль...
			HMODULE lModule;
			DWORD dwModulesSize=0;
			memset(&lModule,0,sizeof(lModule));
			HANDLE hProc=::OpenProcess(PROCESS_TERMINATE|PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pProcesses->ProcessId);
			if(getPsApiWrap()._EnumProcessModules(hProc,&lModule,sizeof(lModule),&dwModulesSize)){
				char szExePath[MAX_PATH]="";
				getPsApiWrap()._GetModuleFileNameEx(hProc,lModule,szExePath,sizeof(szExePath));
				proc.sProcExePath=szExePath;
			}else{
				DWORD dwErr=GetLastError();
			}
			if(hProc){
				proc.bTerminable=TRUE;
				CloseHandle(hProc);
			}else{
				proc.bTerminable=FALSE;
			}
		}else{
			// Зависает на некоторых задачах
			PDEBUG_BUFFER pDBuffer=getNTWrap()._RtlCreateQueryDebugBuffer(0,0);
			getNTWrap()._RtlQueryProcessDebugInformation(pProcesses->ProcessId,1,pDBuffer);//1==PDI_MODULES
			proc.sProcExePath=FindModulePath(pDBuffer,proc.sProcName);
			getNTWrap()._RtlDestroyQueryDebugBuffer(pDBuffer);
		}
		GetProccessCash().Add(proc);
        if (pProcesses->NextEntryDelta == 0){
            break;
		}
        // find the address of the next process structure
        pProcesses = (PSYSTEM_PROCESSES)(((LPBYTE)pProcesses)+ pProcesses->NextEntryDelta);
    }
    HeapFree(hHeap, 0, pBuffer);
    return TRUE;
}

BOOL WINAPI EnumProcesses(BOOL bWithProcTimes, BOOL bSort)
{
	CCSLock lpc(&pCahLock,1);
	BOOL bRes=FALSE;
	GetProccessCash().RemoveAll();
	if(getNTWrap().bIsRealNT==0){
		bRes=EnumProcesses_98Api();//Это не NT
	}else{
		bRes=EnumProcesses_NtApi();
		if(bWithProcTimes){
			// Запоминаем Времена...
			_ProcEnum Proc1;
			for(int i0=0; i0<GetProccessCash().GetSize();i0++){
				Proc1.Add(GetProccessCash()[i0]);
			}
			/*
			FILETIME	CreationTime, ExitingTime, KernelTime, UserTime;
			FILETIME	CreationTime2, ExitingTime2, KernelTime2, UserTime2;
			GetThreadTimes( GetCurrentThread(), &CreationTime, &ExitingTime, &KernelTime, &UserTime );
			Sleep(100);
			GetThreadTimes( GetCurrentThread(), &CreationTime2, &ExitingTime2, &KernelTime2, &UserTime2 );
			ULARGE_INTEGER BaseKernel,BaseKernel2,BaseUser,BaseUser2;
			BaseKernel.LowPart=KernelTime.dwLowDateTime;
			BaseKernel.HighPart=KernelTime.dwHighDateTime;
			BaseKernel2.LowPart=KernelTime2.dwLowDateTime;
			BaseKernel2.HighPart=KernelTime2.dwHighDateTime;
			BaseUser.LowPart=UserTime.dwLowDateTime;
			BaseUser.HighPart=UserTime.dwHighDateTime;
			BaseUser2.LowPart=UserTime2.dwLowDateTime;
			BaseUser2.HighPart=UserTime2.dwHighDateTime;
			LONGLONG lBase=(BaseUser2.QuadPart-BaseUser.QuadPart)+(BaseKernel2.QuadPart-BaseKernel.QuadPart);
			*/
			Sleep(500);
			//LONGLONG lBase=LONGLONG(500)*LONGLONG(100000);
			//if(lBase!=0)
			{
				GetProccessCash().RemoveAll();
				bRes=EnumProcesses_NtApi();
				for(int i=0; i<GetProccessCash().GetSize();i++){
					for(int j=0;j<Proc1.GetSize();j++){
						if(GetProccessCash()[i].dwProcId==Proc1[j].dwProcId){
							LONGLONG pDif	=GetProccessCash()[i].KernelTime	-Proc1[j].KernelTime;
							LONGLONG pDif2	=GetProccessCash()[i].UserTime		-Proc1[j].UserTime;
							LONGLONG lRes=(pDif+pDif2);
							GetProccessCash()[i].lProcent=lRes;
							TRACE3("Proc %s: %I64i vs %I64i\n",GetProccessCash()[i].sProcName,pDif,pDif2);
						}
					}
				}
			}
		}
	}
	if(bRes && !bWithProcTimes && bSort){
		// Сортируем
		for(int i=0; i<GetProccessCash().GetSize()-1;i++){
			for(int j=i+1; j<GetProccessCash().GetSize();j++){
				CString sProc1=(GetProccessCash()[i]).sProcName;
				sProc1.MakeLower();
				CString sProc2=(GetProccessCash()[j]).sProcName;
				sProc2.MakeLower();
				if( sProc1 > sProc2){
					CProcInfo tmp=GetProccessCash()[i];
					GetProccessCash()[i]=GetProccessCash()[j];
					GetProccessCash()[j]=tmp;
				}
			}
		}
	}
	return bRes;
}

DWORD dwGlobalProcessIDForEnum=0;
BOOL CALLBACK EnumWindowsProcByProID(HWND hwnd, LPARAM lParam)
{
	DWORD dwID=0;
	char sT[101]="";
	::GetWindowText(hwnd,sT,100);
	DWORD dwThreadID=GetWindowThreadProcessId(hwnd,&dwID);
    if(dwID==dwGlobalProcessIDForEnum){
        *((HWND *)lParam) = hwnd;
		if(::IsWindowVisible(hwnd)){//Можно выходить
			return FALSE;
		}
    }
    return TRUE;
}

DWORD WINAPI GetProcessIDByHandle(HANDLE hProcess)
{
	if(hProcess==0){
		return 0;
	}
	if(getNTWrap()._GetProcessId){
		return getNTWrap()._GetProcessId(hProcess);
	}else if(getNTWrap()._NtQueryInformationProcess){
		PEB p;
		PROCESS_BASIC_INFORMATION buffer;
		memset(&buffer,0,sizeof(buffer));
		memset(&p,0,sizeof(p));
		buffer.PebBaseAddress=&p;
		NTSTATUS Status=getNTWrap()._NtQueryInformationProcess(hProcess,ProcessBasicInformation,&buffer, sizeof(buffer),NULL);
		if (NT_SUCCESS(Status) && buffer.UniqueProcessId!=NULL){
			return (DWORD)(buffer.UniqueProcessId);
		}
	}else{
		// Not work yet
	}
	return 0;
}

HWND GetMainWindowFromHandle(HANDLE hProcess)
{
	return GetMainWindowFromProcessId(GetProcessIDByHandle(hProcess));
}

void GetWndTopParent(HWND& hWnd)
{
	if(!hWnd){
		return;
	}
	//hWnd=::GetAncestor(hWnd,GA_ROOT);
	HWND hWnd2=hWnd;
	while(hWnd2!=NULL){
		hWnd=hWnd2;
		hWnd2=::GetWindow(hWnd,GW_OWNER);
		if(!hWnd2){
			hWnd2=::GetParent(hWnd);
		}
	}
	return;
}

HWND GetMainWindowFromProcessId(DWORD dwProcID)
{
	if(dwProcID==0){
		return NULL;
	}
    HWND hWindow = NULL;
	dwGlobalProcessIDForEnum = dwProcID;
	EnumWindows(EnumWindowsProcByProID, (LPARAM)(&hWindow));
	GetWndTopParent(hWindow);
    return hWindow;
}

BOOL IsRealNT()
{
	if(getNTWrap().bIsRealNT==0){
		return 0;
	}
	return 1;
}

void EnableDebugPriv( void )
{
	if(getNTWrap().bIsRealNT==0){
		return;//Это не NT, привилегий нету вообще
	}
	static BOOL bEnabled=0;
	if(bEnabled){
		return;
	}
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	if(!OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) ){
		_tprintf( _T("OpenProcessToken() failed, Error = %d SeDebugPrivilege is not available.\n"), GetLastError() );
		bEnabled=-1;
		return;
	}
	if(!LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue)){
		_tprintf( _T("LookupPrivilegeValue() failed, Error = %d SeDebugPrivilege is not available.\n"), GetLastError() );
		CloseHandle( hToken );
		bEnabled=-1;
		return;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) ){
		_tprintf( _T("AdjustTokenPrivileges() failed, Error = %d SeDebugPrivilege is not available.\n"), GetLastError() );
	}
	CloseHandle( hToken );
	bEnabled=1;
}


// InfoCollector.cpp: implementation of the CInfoCollector class.
//
//////////////////////////////////////////////////////////////////////
CHAR* CInfoCollector::STR_UNKNOWN = _T("Unknown");
CHAR* CInfoCollector::STR_NEWLINE = _T("\r\n");


CInfoCollector::CInfoCollector()
{
}

CInfoCollector::~CInfoCollector()
{
    
}

BOOL CInfoCollector::GetOSVersion(CString& szVersion)
{
    OSVERSIONINFOEX2 osvi;
    BOOL bEx = FALSE;

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    
   if( !(bEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return FALSE;
   }

    
   switch (osvi.dwPlatformId)
   {
    case VER_PLATFORM_WIN32_NT:
        return CheckWin32NT(szVersion,osvi,bEx);        
        
    case VER_PLATFORM_WIN32_WINDOWS:
        return CheckWin32Win(szVersion,osvi);
            
    case VER_PLATFORM_WIN32s:
        szVersion.Format(_T("Microsoft Win 3.1 or Win 3.11"));
        break;
   }
   return TRUE;
}

BOOL CInfoCollector::CheckWin32NT(CString &szVersion,CInfoCollector::OSVERSIONINFOEX2& osvi,BOOL bEx)
{
    CString szTemp = "";
    HKEY hKey;
    TCHAR szProductType[80];
    DWORD dwBufLen;
    BOOL b40 = FALSE;

    
    // Test for the product.
    
    if ( osvi.dwMajorVersion <= 4 )
    {
        szVersion = _T("Microsoft Windows NT ");
        b40 = TRUE;
    }
    
    if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
        szVersion = _T("Microsoft Windows 2000 ");
    
    
    if( bEx )  // Use information from GetVersionEx.
    { 
        // Test for the workstation type.
        if ( osvi.wProductType == VER_NT_WORKSTATION )
        {
            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
                szVersion = _T("Microsoft Windows XP ");
            
            if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                szVersion += _T("Home Edition " );
            else
                if( !b40 )
                    szVersion += _T("Professional " );
                else
                    szVersion += _T("Workstation " );
        }
        
        // Test for the server type.
        else if ( osvi.wProductType == VER_NT_SERVER )
        {
            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
                szVersion = _T("Microsoft Windows .NET ");
            
            if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                szVersion += _T("DataCenter Server " );
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                if( osvi.dwMajorVersion == 4 )
                    szVersion += _T("Advanced Server " );
                else
                    szVersion += _T("Enterprise Server " );
                else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                    szVersion += _T("Web Server " );
                else
                    szVersion += _T("Server " );
        }
    }
    else   // Use the registry on early versions of Windows NT.
    {
        RegOpenKeyEx( HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
            0, KEY_QUERY_VALUE, &hKey );
        RegQueryValueEx( hKey, "ProductType", NULL, NULL,
            (LPBYTE) szProductType, &dwBufLen);
        RegCloseKey( hKey );
        if ( lstrcmpi( "WINNT", szProductType) == 0 )
            szVersion += _T("Professional " );
        if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
            szVersion += _T("Server " );
        if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
            szVersion += _T("Advanced Server " );
    }
    
    // Display version, service pack (if any), and build number.
    
    if ( osvi.dwMajorVersion <= 4 )
    {
        szTemp.Format("%d.%d %s (Build %d)",
            osvi.dwMajorVersion,
            osvi.dwMinorVersion,
            osvi.szCSDVersion,
            osvi.dwBuildNumber & 0xFFFF);
    }
    else
    { 
        szTemp.Format("%s (Build %d)",
            osvi.szCSDVersion,
            osvi.dwBuildNumber & 0xFFFF);
    }
    szVersion += szTemp;
    return TRUE;
}

BOOL CInfoCollector::CheckWin32Win(CString &szVersion,CInfoCollector::OSVERSIONINFOEX2& osvi)
{
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
    {
        szVersion = _T("Microsoft Windows 95 ");
        if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
            szVersion += _T("OSR2 " );
        return TRUE;
    } 
    
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
    {
        szVersion = _T("Microsoft Windows 98 ");
        if ( osvi.szCSDVersion[1] == 'A' )
            szVersion += _T("SE " );
        return TRUE;
    } 
    
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
    {
        szVersion = _T("Microsoft Windows Millennium Edition ");
        return TRUE;
    } 

    return FALSE;
}

BOOL CInfoCollector::GetCpuInfo(CString& szCpuInfo)
{
    LONG lRes;
    HKEY hKey;
    DWORD dwData;
    TCHAR chData[1024];
    DWORD dwDataSize = sizeof(dwData);
    SYSTEM_INFO s_info;
    CString szTemp;

    lRes = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE,
        _T("Hardware\\Description\\System\\CentralProcessor\\0"),
        0, KEY_QUERY_VALUE, &hKey);

    if(lRes != ERROR_SUCCESS)
        return FALSE;
    
    lRes = ::RegQueryValueEx (hKey, _T("~MHz"), NULL, NULL,
            (LPBYTE)&dwData, &dwDataSize);
    
    if(lRes)
        dwData = CalcCPUSpeed();


    dwDataSize = sizeof(chData);
    lRes = ::RegQueryValueEx (hKey, _T("Identifier"), NULL, NULL,
        (LPBYTE)chData, &dwDataSize);

    if( lRes )
        szCpuInfo.Format (_T("%d MHz "), dwData);
    else
        szCpuInfo.Format(_T("%s %d MHz "), chData,dwData);
        
    lRes += ::RegQueryValueEx (hKey, _T("VendorIdentifier"), NULL, NULL,
            (LPBYTE)chData, &dwDataSize);
    szCpuInfo += chData;
        
    
    lRes += RegCloseKey (hKey);

    GetSystemInfo(&s_info);
    szTemp.Format(_T("\r\nNumber of processor = %u"),s_info.dwNumberOfProcessors);
    szCpuInfo += szTemp;

    
    return TRUE;
}

BOOL CInfoCollector::GetMemoryInfo(CString& szCpuInfo)
{
    MEMORYSTATUS memstat;
    CString szTemp;

    GlobalMemoryStatus(&memstat);

    szCpuInfo.Format(_T("Total physical memory : %u Kb\r\n"),memstat.dwTotalPhys/1024);
    szTemp.Format(_T("Avail physical memory : %u Kb\r\n"),memstat.dwAvailPhys/1024);
    szCpuInfo += szTemp;

    szTemp.Format(_T("Total virtual memory : %u Kb\r\n"),memstat.dwTotalPageFile /1024);
    szCpuInfo += szTemp;

    szTemp.Format(_T("Avail virtual memory : %u Kb"),memstat.dwAvailPageFile/1024);
    szCpuInfo += szTemp;
    return TRUE;
}

BOOL CInfoCollector::GetVideoResolution(CString& szVideoInfo)
{
    HDC hdc = GetDC(NULL);

    szVideoInfo.Format(_T("%ux%u %u bits per pixel"),
                                GetSystemMetrics(SM_CXSCREEN),
                                GetSystemMetrics(SM_CYSCREEN),
                                GetDeviceCaps(hdc,BITSPIXEL));
    ReleaseDC(NULL,hdc);
    
    return TRUE;
}



BOOL CInfoCollector::GetBiosInfo(CString &szBiosInfo)
{
    HKEY hKey;
    LONG lRes;
    TCHAR data[1024];
    DWORD dwDataSize = sizeof(data);
    CString szTemp;

    lRes = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE,
        _T("Hardware\\Description\\System"),
        0, KEY_QUERY_VALUE, &hKey);

    if(lRes != ERROR_SUCCESS)
        return FALSE;
    
    lRes += ::RegQueryValueEx (hKey, _T("SystemBiosVersion"), NULL, NULL,
            (LPBYTE)data, &dwDataSize);
        
    szBiosInfo.Format ("%s \r\n", data);
        
    dwDataSize = sizeof(data);
    lRes += ::RegQueryValueEx (hKey, _T("SystemBiosDate"), NULL, NULL,
            (LPBYTE)data, &dwDataSize);

    szTemp.Format(_T("System Bios date : %s"),data);
    szBiosInfo += szTemp;
        
    
    lRes += RegCloseKey (hKey);

    
    return !lRes;
}


BOOL CInfoCollector::GetAllDisksInfo(CString& szDiskInfo)
{
    TCHAR data[1024];    
    TCHAR* ptr = data; 
    DWORD dwTotal,dwFree;
    CString szTemp;
    UINT type;

    TCHAR szFileSystemNameBuffer[256];
    DWORD dwVolumeSerialNumber;
    DWORD dwFileSystemNameSize=256;

    BOOL bResult;


    szDiskInfo = _T("Local drives info :\r\n");
    GetLogicalDriveStrings(1024,data);

    while(*ptr != TCHAR('\0') )
    {
        type = GetDriveType(ptr);
        if(type != DRIVE_REMOVABLE && type != DRIVE_CDROM)
        {
            GetDiskFree(ptr,dwTotal,dwFree);
            szTemp.Format(_T(" %s total %uMb free %uMb "),ptr,dwTotal,dwFree);

            bResult = GetVolumeInformation(ptr,
                NULL,
                0,
                &dwVolumeSerialNumber,
                NULL,
                NULL,
                szFileSystemNameBuffer,
                dwFileSystemNameSize);
            
            if( bResult )
                szTemp += szFileSystemNameBuffer;
        }
        else
            szTemp.Format(_T(" %s "),ptr);

        szDiskInfo += szTemp;

        switch( type )
        {
        case DRIVE_REMOVABLE:
            szDiskInfo += _T(" REMOVABLE\r\n");
            break;
        case DRIVE_FIXED:
            szDiskInfo += _T(" FIXED\r\n");
            break;
        case DRIVE_REMOTE:     
            szDiskInfo += _T(" NETWORK\r\n");
            break;
        case DRIVE_CDROM:
            szDiskInfo += _T(" CDROM\r\n");
            break;
        case DRIVE_RAMDISK :
            szDiskInfo += _T(" RAMDISK\r\n");
            break;
        default:
            szDiskInfo += _T(" UNKNOWN TYPE\r\n");
        }

        while(*ptr != TCHAR('\0')) 
            ptr++; 
        ptr++;
    }

    
    return  TRUE;
}


CString CInfoCollector::GetOverallInfo()
{
    CString szTemp;
    CString szInfo;
    CWaitCursor cursor;

    szInfo = _T("OS Version = ");
    if(GetOSVersion(szTemp))
        szInfo += szTemp;
    else
        szInfo += STR_UNKNOWN;

    szInfo+=STR_NEWLINE;
    GetDirectoriesInfo(szTemp);
    szInfo+=szTemp;

    szInfo+=_T("\r\nBrowser version = ");
    if(GetBrowserInfo(szTemp))
        szInfo += szTemp;
    else
        szInfo += STR_UNKNOWN;
    

    szInfo+=_T("\r\n\r\nSytem CPU info = ");
    if(GetCpuInfo(szTemp))
        szInfo += szTemp;
    else
        szInfo += STR_UNKNOWN;
    
    GetMemoryInfo(szTemp);
    szInfo += STR_NEWLINE;
    szInfo += STR_NEWLINE;
    szInfo += szTemp;
    
    szInfo += _T("\r\n\r\nVideo resolution info = ");
    if(GetVideoResolution(szTemp))
        szInfo += szTemp;
    else
        szInfo += STR_UNKNOWN;
    
    szInfo += _T("\r\n\r\nSystem bios version = ");
    if(GetBiosInfo(szTemp))
        szInfo += szTemp;
    else
        szInfo += STR_UNKNOWN;

    GetAllDisksInfo(szTemp);
    szInfo += STR_NEWLINE;
    szInfo += STR_NEWLINE;
    szInfo += szTemp;

    return szInfo;
}

inline double find_disk_free(char *path)
{
    double val = 0.0;
    DWORD SectorsPerCluster;        // address of sectors per cluster 
    DWORD BytesPerSector;            // address of bytes per sector 
    DWORD NumberOfFreeClusters;    // address of number of free clusters  
    DWORD TotalNumberOfClusters;     // address of total number of clusters  
    
    LPSTR    pPathPointer = path;
    char    TmpPath[_MAX_PATH];
    int    len = lstrlen(path);
    
    if (len > 3) {
        lstrcpy(TmpPath, path);
        pPathPointer = TmpPath;
        if((TmpPath[0] == '\\' && TmpPath[1] == '\\') ||
            (TmpPath[0] == '/' && TmpPath[1] == '/')) {
            //            Check Network Drive
            pPathPointer = TmpPath+2;
            pPathPointer += strcspn(pPathPointer, "\\/");
            if(*pPathPointer != 0) {
                pPathPointer ++;
                pPathPointer += strcspn(pPathPointer, "\\/");
                if(*pPathPointer != 0) {
                    *(pPathPointer+1) = 0;
                }
            }
            pPathPointer = TmpPath;
        }
        else {
            *(pPathPointer+3) = 0;
        }
    }
    if (len < 3)
        val = (double)1000000.;
    else if (GetDiskFreeSpace(pPathPointer,
        &SectorsPerCluster,
        &BytesPerSector,
        &NumberOfFreeClusters,
        &TotalNumberOfClusters))
        val = (double)NumberOfFreeClusters *
        (double)SectorsPerCluster *
        (double)BytesPerSector;
    
    return val;
}

BOOL CInfoCollector::GetDirectoriesInfo(CString& szDirsInfo)
{
    TCHAR data[MAX_PATH+1];
    TCHAR* ptr = data;

    szDirsInfo = _T("Windows Directory = ");
    GetWindowsDirectory(data,MAX_PATH+1);

    find_disk_free(data);
    szDirsInfo += data;
    if( !CheckDirectoryAccess(data) )
        szDirsInfo += _T("Write access denied !");

    szDirsInfo += _T("\r\nSystem Directory = ");
    GetSystemDirectory(data,MAX_PATH+1);
    find_disk_free(data);
    szDirsInfo += data;
    if( !CheckDirectoryAccess(data) )
        szDirsInfo+=_T("Write access denied !");
        
    szDirsInfo += _T("\r\nWork Directory = ");
    GetModuleFileName(GetModuleHandle(NULL),data,MAX_PATH+1);
    while(*ptr) ptr++;
    while(*ptr != TCHAR('\\')) ptr--;
    *ptr = TCHAR('\0');

    find_disk_free(data);

    szDirsInfo += data;
    if( !CheckDirectoryAccess(data) )
        szDirsInfo+=_T("Write access denied !");
    
    return TRUE;
}

BOOL CInfoCollector::GetBrowserInfo(CString &szBrowserInfo)
{
    LONG lRes;
    HKEY hKey;
    TCHAR chData[64];
    DWORD dwDataSize = sizeof(chData);
    
    lRes = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE,
        _T("Software\\Microsoft\\Internet Explorer"),
        0, KEY_QUERY_VALUE, &hKey);
    
    if(lRes != ERROR_SUCCESS)
        return FALSE;
    
    lRes += ::RegQueryValueEx (hKey, _T("Version"), NULL, NULL,
        (LPBYTE)chData, &dwDataSize);
    
    szBrowserInfo = chData;
    

    lRes += RegCloseKey (hKey);
    
    return lRes == 0;
}

DWORD CInfoCollector::CalcCPUSpeed()
{
   DWORD dwSpeed = 0;
    
    DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    
    dwSpeed = CalculateSpeed();
    
    SetPriorityClass(GetCurrentProcess(), dwStartingPriority);
    
    return dwSpeed;
}


DWORD CInfoCollector::CalculateSpeed()
{
   int    nTimeStart = 0;
   int    nTimeStop = 0;
   DWORD dwStartTicks = 0;
   DWORD dwEndTicks = 0;
   DWORD dwTotalTicks = 0;
   DWORD dwCpuSpeed = 0;
    
    
   nTimeStart = GetTickCount();
    
    dwStartTicks = TimeStamp();
    
   for(;;)
   {
      nTimeStop = GetTickCount();
      if ((nTimeStop - nTimeStart) > 2000)    // one-half second
      {
         dwEndTicks = TimeStamp();
         break;
      }
   }
    
   dwCpuSpeed = (dwEndTicks - dwStartTicks) / (20*100000); // MHz
    
   return (dwCpuSpeed);
}

DWORD CInfoCollector::TimeStamp()
{
   DWORD dwTickVal;
    
   __asm
   {
      _emit 0Fh   // RDTSC
        _emit 31h
        mov   dwTickVal, eax
   }
    
   return dwTickVal;    
}

void CInfoCollector::GetDiskFree(LPCTSTR pszDrive, DWORD &dwTotal, DWORD &dwFree)
{
    PFNGETDISKFREESPACEEX pGetDiskFreeSpaceEx = NULL;
    __int64 i64FreeBytesToCaller;
    __int64 i64TotalBytes;
    __int64 i64FreeBytes;
    DWORD    dwSectPerClust; 
    DWORD    dwBytesPerSect;
    DWORD    dwFreeClusters; 
    DWORD    dwTotalClusters;
    

#ifdef _UNICODE
    pGetDiskFreeSpaceEx = (PFNGETDISKFREESPACEEX)DBG_GETPROC( GetModuleHandle("kernel32.dll"),
        "GetDiskFreeSpaceExW");
#else
    pGetDiskFreeSpaceEx = (PFNGETDISKFREESPACEEX)DBG_GETPROC( GetModuleHandle("kernel32.dll"),
        "GetDiskFreeSpaceExA");
#endif
    
    if (pGetDiskFreeSpaceEx)
    {
        pGetDiskFreeSpaceEx (pszDrive,
            (PULARGE_INTEGER)&i64FreeBytesToCaller,
            (PULARGE_INTEGER)&i64TotalBytes,
            (PULARGE_INTEGER)&i64FreeBytes);

        dwTotal = DWORD(i64TotalBytes / (1024*1024));
        dwFree =  DWORD(i64FreeBytesToCaller / (1024*1024));
    }
    else 
    {
        GetDiskFreeSpace (pszDrive, 
            &dwSectPerClust, 
            &dwBytesPerSect,
            &dwFreeClusters, 
            &dwTotalClusters);

        dwTotal = DWORD((__int64(dwTotalClusters) * dwSectPerClust * dwBytesPerSect)/(1024*1024));
        dwFree = DWORD((__int64(dwFreeClusters) * dwSectPerClust * dwBytesPerSect)/(1024*1024));
        
    }
}

BOOL CInfoCollector::CheckDirectoryAccess(LPCTSTR szPath)
{
    TCHAR* szPre = "mad";  
    TCHAR szFileName[MAX_PATH+1];
    HANDLE hFile = NULL;
    int iResult = 0;

    GetTempFileName(szPath,szPre,0,szFileName);

    hFile = CreateFile(szFileName,
        GENERIC_WRITE |GENERIC_READ,
        0,NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if(hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    iResult += (int)!CloseHandle(hFile);
    iResult += (int)!DeleteFile(szFileName);    

    return  !iResult;
}
 
CAppBooster::CAppBooster()
{
	::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
};

CAppBooster::~CAppBooster()
{
	::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_NORMAL);
};

typedef char* (WINAPI *_ProcInfo)(DWORD);
BOOL SystemGetOpenedHandles(DWORD dwProcID, CString& sResult)
{
	BOOL bRes=FALSE;
	HINSTANCE hUtils=LoadLibrary("WKUtils.dll");
	if(hUtils){
		_ProcInfo fp=(_ProcInfo)DBG_GETPROC(hUtils,"SystemGetOpenedHandles");
		if(fp){
			char* hGlob=(*fp)(dwProcID);
			sResult=hGlob;
			bRes=(hGlob!=NULL);
			::GlobalFree(hGlob);
		}
	}
	FreeLibrary(hUtils);
	return bRes;
}
