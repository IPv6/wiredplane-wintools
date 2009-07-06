#ifndef SYSTEMINFO_H_INCLUDED
#define SYSTEMINFO_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>
#include <afxtempl.h>
#include <shlobj.h>
#include <process.h>
#include <afxmt.h>
#include "Debug.h"
//#include <afxmt.h>


typedef LONG    NTSTATUS;
typedef LONG    KPRIORITY;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
#define SystemProcessesAndThreadsInformation    5
#define DEF_SystemModuleInformation				    11
// Object = 17

typedef struct _CLIENT_ID {
    DWORD         UniqueProcess;
    DWORD         UniqueThread;
} CLIENT_ID;

typedef struct _UNICODE_STRING {
    USHORT        Length;
    USHORT        MaximumLength;
    PWSTR         Buffer;
} UNICODE_STRING;

typedef struct _VM_COUNTERS {
    SIZE_T        PeakVirtualSize;
    SIZE_T        VirtualSize;
    ULONG         PageFaultCount;
    SIZE_T        PeakWorkingSetSize;
    SIZE_T        WorkingSetSize;
    SIZE_T        QuotaPeakPagedPoolUsage;
    SIZE_T        QuotaPagedPoolUsage;
    SIZE_T        QuotaPeakNonPagedPoolUsage;
    SIZE_T        QuotaNonPagedPoolUsage;
    SIZE_T        PagefileUsage;
    SIZE_T        PeakPagefileUsage;
} VM_COUNTERS;

typedef struct _SYSTEM_THREADS {
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG         WaitTime;
    PVOID         StartAddress;
    CLIENT_ID     ClientId;
    KPRIORITY     Priority;
    KPRIORITY     BasePriority;
    ULONG         ContextSwitchCount;
    LONG          State;
    LONG          WaitReason;
} SYSTEM_THREADS, * PSYSTEM_THREADS;

typedef struct _SYSTEM_MODULE_INFORMATION { // Information Class 11
	ULONG Reserved[2];
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

#ifndef QUOTA_LIMITS_HARDWS_MIN_ENABLE
typedef struct _IO_COUNTERS {
    ULONGLONG  ReadOperationCount;
    ULONGLONG  WriteOperationCount;
    ULONGLONG  OtherOperationCount;
    ULONGLONG ReadTransferCount;
    ULONGLONG WriteTransferCount;
    ULONGLONG OtherTransferCount;
} IO_COUNTERS;
typedef IO_COUNTERS *PIO_COUNTERS;
#endif

typedef struct _SYSTEM_PROCESSES {
    ULONG             NextEntryDelta;
    ULONG             ThreadCount;
    ULONG             Reserved1[6];
    LARGE_INTEGER     CreateTime;
    LARGE_INTEGER     UserTime;
    LARGE_INTEGER     KernelTime;
    UNICODE_STRING    ProcessName;
    KPRIORITY         BasePriority;
    ULONG             ProcessId;
    ULONG             InheritedFromProcessId;
    ULONG             HandleCount;
    ULONG             Reserved2[2];
    VM_COUNTERS       VmCounters;
#if _WIN32_WINNT >= 0x500
    IO_COUNTERS       IoCounters;
#endif
    SYSTEM_THREADS    Threads[1];
} SYSTEM_PROCESSES, * PSYSTEM_PROCESSES;

typedef struct _DEBUG_MODULE_INFORMATION { // c.f. SYSTEM_MODULE_INFORMATION
	ULONG Reserved[2];
	ULONG Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} DEBUG_MODULE_INFORMATION, *PDEBUG_MODULE_INFORMATION;

typedef struct _DEBUG_BUFFER {
	HANDLE SectionHandle;
	PVOID SectionBase;
	PVOID RemoteSectionBase;
	ULONG SectionBaseDelta;
	HANDLE EventPairHandle;
	ULONG Unknown[2];
	HANDLE RemoteThreadHandle;
	ULONG InfoClassMask;
	ULONG SizeOfInfo;
	ULONG AllocatedSize;
	ULONG SectionSize;
	PVOID ModuleInformation;
	PVOID BackTraceInformation;
	PVOID HeapInformation;
	PVOID LockInformation;
	PVOID Reserved[8];
} DEBUG_BUFFER, *PDEBUG_BUFFER;


typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation, // 0 Y N
		ProcessQuotaLimits, // 1 Y Y
		ProcessIoCounters, // 2 Y N
		ProcessVmCounters, // 3 Y N
		ProcessTimes, // 4 Y N
		ProcessBasePriority, // 5 N Y
		ProcessRaisePriority, // 6 N Y
		ProcessDebugPort, // 7 Y Y
		ProcessExceptionPort, // 8 N Y
		ProcessAccessToken, // 9 N Y
		ProcessLdtInformation, // 10 Y Y
		ProcessLdtSize, // 11 N Y
		ProcessDefaultHardErrorMode, // 12 Y Y
		ProcessIoPortHandlers, // 13 N Y
		ProcessPooledUsageAndLimits, // 14 Y N
		ProcessWorkingSetWatch, // 15 Y Y
		ProcessUserModeIOPL, // 16 N Y
		ProcessEnableAlignmentFaultFixup, // 17 N Y
		ProcessPriorityClass, // 18 N Y
		ProcessWx86Information, // 19 Y N
		ProcessHandleCount, // 20 Y N
		ProcessAffinityMask, // 21 N Y
		ProcessPriorityBoost, // 22 Y Y
		ProcessDeviceMap, // 23 Y Y
		ProcessSessionInformation, // 24 Y Y
		ProcessForegroundInformation, // 25 N Y
		ProcessWow64Information // 26 Y N
} PROCESSINFOCLASS;

typedef struct _SYSTEM_PROCESSOR_TIMES {//Information Class 8
	LARGE_INTEGER IdleTime;
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER DpcTime;
	LARGE_INTEGER InterruptTime;
	ULONG InterruptCount;
}SYSTEM_PROCESSOR_TIMES,*PSYSTEM_PROCESSOR_TIMES;

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[229];
    PVOID Reserved3[59];
    ULONG SessionId;
} PEB, *PPEB;

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG* UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

typedef struct _SYSTEM_HANDLE
{
	DWORD	ProcessID;
	WORD	HandleType;
	WORD	HandleNumber;
	DWORD	KernelAddress;
	DWORD	Flags;
} SYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	DWORD			Count;
	SYSTEM_HANDLE	Handles[1];
} SYSTEM_HANDLE_INFORMATION;


typedef struct _UNICODE_STRING2
{
	WORD  Length;
	WORD  MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING2;

class CProcInfo
{
public:
	DWORD dwProcId;
	BOOL bTerminable;
	long lWorkingSet;
	LONGLONG lProcent;
	LONGLONG KernelTime;
	LONGLONG UserTime;
	LONGLONG CreateTime;
	CString sProcName;
	CString sProcExePath;
	CString sIconIndex;

	DWORD dwCustomMessageID;
	CBitmap* dwCstomIconIndex;
	CProcInfo()
	{
		lProcent=0;
		bTerminable=TRUE;
		dwProcId=bTerminable=lWorkingSet=0;
		sProcName="";
		sProcExePath="";
		sIconIndex="";
		dwCustomMessageID=0;
		dwCstomIconIndex=0;
	};
	CProcInfo& operator=(const CProcInfo& from)
	{
		lProcent=from.lProcent;
		KernelTime=from.KernelTime;
		UserTime=from.UserTime;
		dwProcId=from.dwProcId;
		bTerminable=from.bTerminable;
		lWorkingSet=from.lWorkingSet;
		sProcName=from.sProcName;
		sProcExePath=from.sProcExePath;
		sIconIndex=from.sIconIndex;
		dwCustomMessageID=from.dwCustomMessageID;
		dwCstomIconIndex=from.dwCstomIconIndex;
		return *this;
	}
};

void EnableDebugPriv();
typedef CArray<CProcInfo,CProcInfo&> _ProcEnum;
CString GetExeFromHwnd(DWORD dwProcID);
CString GetExeFromHwnd(HWND hWin);
_ProcEnum& GetProccessCash(BOOL bLock=-1);
BOOL WINAPI EnumProcesses(BOOL bWithProcTimes=0, BOOL bSort=0);
BOOL SystemGetOpenedHandles(DWORD dwProcID, CString& sRes);
HWND GetMainWindowFromProcessId(DWORD dwProcID);
HWND GetMainWindowFromHandle(HANDLE hProcess);
double GetCPUTimesPercents();
double GetMemoryPercents();
LARGE_INTEGER GetCPUTimes();
BOOL IsRealNT();
//
/* Набор классов для вытаскивания информации о системе
OS Version
Windows Directory
System Directory
Work Directory
Browser version 

Sytem CPU info
Number of processor

Total physical memory
Avail physical memory
Total virtual memory
Avail virtual memory

Video resolution info

System bios version
System Bios date 

Local drives info :
A:\ REMOVABLE
C:\ total 2047Mb free 1193Mb FAT FIXED
D:\ total 6644Mb free 3561Mb NTFS FIXED
E:\ CDROM
S:\ total 499Mb free 199Mb FAT FIXED
U:\ total 6644Mb free 3561Mb NTFS FIXED
V:\ total 6644Mb free 3561Mb NTFS FIXED
Y:\ total 2578Mb free 150Mb NTFS FIXED
*****/

// InfoCollector.h: interface for the CInfoCollector class.
//
//////////////////////////////////////////////////////////////////////

// from Platform SDK
#define VER_SERVER_NT                       0x80000000
#define VER_WORKSTATION_NT                  0x40000000
#define VER_SUITE_SMALLBUSINESS             0x00000001
#define VER_SUITE_ENTERPRISE                0x00000002
#define VER_SUITE_BACKOFFICE                0x00000004
#define VER_SUITE_COMMUNICATIONS            0x00000008
#define VER_SUITE_TERMINAL                  0x00000010
#define VER_SUITE_SMALLBUSINESS_RESTRICTED  0x00000020
#define VER_SUITE_EMBEDDEDNT                0x00000040
#define VER_SUITE_DATACENTER                0x00000080
#define VER_SUITE_SINGLEUSERTS              0x00000100
#define VER_SUITE_PERSONAL                  0x00000200
#define VER_SUITE_BLADE                     0x00000400

// from Platform SDK
#define VER_NT_WORKSTATION              0x0000001
#define VER_NT_DOMAIN_CONTROLLER        0x0000002
#define VER_NT_SERVER                   0x0000003


class CInfoCollector  
{
public:
    CInfoCollector();
    virtual ~CInfoCollector();

    virtual CString GetOverallInfo();

public:
    // from Platform SDK
    typedef struct _OSVERSIONINFOEX2 {
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        TCHAR szCSDVersion[ 128 ];
        WORD wServicePackMajor;
        WORD wServicePackMinor;
        WORD wSuiteMask;
        BYTE wProductType;
        BYTE wReserved;
    } OSVERSIONINFOEX2;
    
protected:
    virtual DWORD CalcCPUSpeed();
    virtual BOOL GetBrowserInfo(CString& szBrowserInfo);
    virtual BOOL GetDirectoriesInfo(CString& szDirsInfo);
    virtual BOOL GetBiosInfo(CString& szBiosInfo);
    virtual BOOL GetVideoResolution(CString& szVideoInfo);
    virtual BOOL GetOSVersion(CString& szVersion);
    virtual BOOL GetMemoryInfo(CString& szMemInfo);
    virtual BOOL GetAllDisksInfo(CString& szDiskInfo);
    virtual BOOL GetCpuInfo(CString& szCpuInfo);

    BOOL CheckWin32Win(CString& szVersion,OSVERSIONINFOEX2& osvi);
    BOOL CheckWin32NT(CString &szVersion,OSVERSIONINFOEX2& osvi,BOOL bEx);
    DWORD CalculateSpeed();
    DWORD TimeStamp();
    BOOL CheckDirectoryAccess(LPCTSTR szPath);
    void GetDiskFree(LPCTSTR pszDrive,DWORD& dwTotal,DWORD& dwFree);
    
    static TCHAR* STR_UNKNOWN;
    static TCHAR* STR_NEWLINE;

    enum {
        WIN32s =     0x1,  
        WIN95 =        0x2,  
        WIN95OSR =    0x4,  
        WIN95OSR2 = 0x8,  
        WIN98 =        0x10, 
        WIN98SE =    0x20, 
        WINME =        0x40, 
        WINNT351 =    0x80, 
        WINNT40 =    0x100,
        WINNT50 =    0x200,
        WINTXP =        0x400,
        WHISTLER =    0x800
    };
    DWORD m_dwOsType;

};
typedef BOOL (WINAPI* PFNGETDISKFREESPACEEX)(LPCTSTR, PULARGE_INTEGER,PULARGE_INTEGER, PULARGE_INTEGER);

class CAppBooster
{
public:
	CAppBooster();
	~CAppBooster();
};

class CCSLock
{
public:
	CCriticalSection* p;
	CCSLock(CCriticalSection* _p, int lock){
		p=_p;
		p->Lock();
	};
	~CCSLock(){
		p->Unlock();
	};
};
#endif