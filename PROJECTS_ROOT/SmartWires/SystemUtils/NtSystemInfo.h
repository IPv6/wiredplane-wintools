#ifndef _NTSYSINFO_H_
#define _NTSYSINFO_H_

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

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemModuleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemModuleInformation
{
public:
	typedef struct _MODULE_INFO
	{
		DWORD	ProcessId;
		TCHAR	FullPath[_MAX_PATH];
		HMODULE Handle;
	} MODULE_INFO;

public:
	typedef DWORD (WINAPI *PEnumProcessModules)(
				HANDLE hProcess,      // handle to process
				HMODULE *lphModule,   // array of module handles
				DWORD cb,             // size of array
				LPDWORD lpcbNeeded    // number of bytes required
			);

	typedef DWORD (WINAPI *PGetModuleFileNameEx)(
				HANDLE hProcess,    // handle to process
				HMODULE hModule,    // handle to module
				LPTSTR lpFilename,  // path buffer
				DWORD nSize         // maximum characters to retrieve
			);

public:
	SystemModuleInformation( DWORD pID = (DWORD)-1, BOOL bRefresh = FALSE );

	BOOL Refresh();

protected:
	void GetModuleListForProcess( DWORD processID );

public:
	DWORD m_processId;
	CList< MODULE_INFO, MODULE_INFO& > m_ModuleInfos;

protected:
	PEnumProcessModules		m_EnumProcessModules;
	PGetModuleFileNameEx	m_GetModuleFileNameEx;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemWindowInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemWindowInformation
{
public:
	enum { MaxCaptionSize = 1024 };

	typedef struct _WINDOW_INFO
	{
		DWORD	ProcessId;
		TCHAR	Caption[MaxCaptionSize];
		HWND	hWnd;
	} WINDOW_INFO;

public:
	SystemWindowInformation( DWORD pID = (DWORD)-1, BOOL bRefresh = FALSE );

	BOOL Refresh();

protected:
	static BOOL CALLBACK EnumerateWindows( HWND hwnd, LPARAM lParam );
	
public:
	DWORD m_processId;
	CList< WINDOW_INFO, WINDOW_INFO& > m_WindowInfos;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfoUtils
//
//////////////////////////////////////////////////////////////////////////////////////

// Helper functions

class SystemInfoUtils
{
public:

	//////////////////////////////////////////////////////////////////////////////////
	// String conversion functions

	// From wide char string to CString
	static void LPCWSTR2CString( LPCWSTR strW, CString& str );
	// From unicode string to CString
	static void Unicode2CString( UNICODE_STRING* strU, CString& str );

	//////////////////////////////////////////////////////////////////////////////////
	// File name conversion functions

	static BOOL GetDeviceFileName( LPCTSTR, CString& );
	static BOOL GetFsFileName( LPCTSTR, CString& );

	//////////////////////////////////////////////////////////////////////////////////
	// Information functions

	static DWORD GetNTMajorVersion();
};

//////////////////////////////////////////////////////////////////////////////////////
//
// INtDll
//
//////////////////////////////////////////////////////////////////////////////////////

class INtDll
{
public:
	typedef DWORD (WINAPI *PNtQueryObject)( HANDLE, DWORD, VOID*, DWORD, VOID* );
	typedef DWORD (WINAPI *PNtQuerySystemInformation)( DWORD, VOID*, DWORD, ULONG* );
	typedef DWORD (WINAPI *PNtQueryInformationThread)(HANDLE, ULONG, PVOID,	DWORD, DWORD* );
	typedef DWORD (WINAPI *PNtQueryInformationFile)(HANDLE, PVOID,	PVOID, DWORD, DWORD );
	typedef DWORD (WINAPI *PNtQueryInformationProcess)(HANDLE, DWORD, PVOID, DWORD, PVOID );
	
public:
	static PNtQuerySystemInformation	NtQuerySystemInformation;
	static PNtQueryObject				NtQueryObject;
	static PNtQueryInformationThread	NtQueryInformationThread;
	static PNtQueryInformationFile		NtQueryInformationFile;
	static PNtQueryInformationProcess	NtQueryInformationProcess;

	static BOOL							NtDllStatus;

	static DWORD						dwNTMajorVersion;

protected:
	static BOOL Init();
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemProcessInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemProcessInformation : public INtDll
{
public:
	typedef LARGE_INTEGER   QWORD;

	typedef struct _PROCESS_BASIC_INFORMATION {
			DWORD ExitStatus;
			PVOID PebBaseAddress;
			DWORD AffinityMask;
			DWORD BasePriority;
			DWORD UniqueProcessId;
			DWORD InheritedFromUniqueProcessId;
		} PROCESS_BASIC_INFORMATION;

	typedef struct _VM_COUNTERS
		{
		DWORD PeakVirtualSize;
		DWORD VirtualSize;
		DWORD PageFaultCount;
		DWORD PeakWorkingSetSize;
		DWORD WorkingSetSize;
		DWORD QuotaPeakPagedPoolUsage;
		DWORD QuotaPagedPoolUsage;
		DWORD QuotaPeakNonPagedPoolUsage;
		DWORD QuotaNonPagedPoolUsage;
		DWORD PagefileUsage;
		DWORD PeakPagefileUsage;
		} VM_COUNTERS;

	typedef struct _SYSTEM_THREAD
		{
		DWORD        u1;
		DWORD        u2;
		DWORD        u3;
		DWORD        u4;
		DWORD        ProcessId;
		DWORD        ThreadId;
		DWORD        dPriority;
		DWORD        dBasePriority;
		DWORD        dContextSwitches;
		DWORD        dThreadState;      // 2=running, 5=waiting
		DWORD        WaitReason;
		DWORD        u5;
		DWORD        u6;
		DWORD        u7;
		DWORD        u8;
		DWORD        u9;
		} SYSTEM_THREAD;

	typedef struct _SYSTEM_PROCESS_INFORMATION
		{
		DWORD          dNext;
		DWORD          dThreadCount;
		DWORD          dReserved01;
		DWORD          dReserved02;
		DWORD          dReserved03;
		DWORD          dReserved04;
		DWORD          dReserved05;
		DWORD          dReserved06;
		QWORD          qCreateTime;
		QWORD          qUserTime;
		QWORD          qKernelTime;
		UNICODE_STRING usName;
		DWORD	       BasePriority;
		DWORD          dUniqueProcessId;
		DWORD          dInheritedFromUniqueProcessId;
		DWORD          dHandleCount;
		DWORD          dReserved07;
		DWORD          dReserved08;
		VM_COUNTERS    VmCounters;
		DWORD          dCommitCharge;
		SYSTEM_THREAD  Threads[1];
		} SYSTEM_PROCESS_INFORMATION;

	enum { BufferSize = 0x10000 };

public:
	SystemProcessInformation( BOOL bRefresh = FALSE );
	virtual ~SystemProcessInformation();

	BOOL Refresh();

public:
	CMap< DWORD, DWORD&, SYSTEM_PROCESS_INFORMATION*, SYSTEM_PROCESS_INFORMATION*> m_ProcessInfos;
	SYSTEM_PROCESS_INFORMATION* m_pCurrentProcessInfo;

protected:
	UCHAR*						m_pBuffer;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemThreadInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemThreadInformation : public INtDll
{
public:
	typedef struct _THREAD_INFORMATION
		{
		DWORD		ProcessId;
		DWORD		ThreadId;
		HANDLE		ThreadHandle;	
		} THREAD_INFORMATION;

	
	typedef struct _BASIC_THREAD_INFORMATION {
		DWORD u1;
		DWORD u2;
		DWORD u3;
		DWORD ThreadId;
		DWORD u5;
		DWORD u6;
		DWORD u7;
	} BASIC_THREAD_INFORMATION;

public:
	SystemThreadInformation( DWORD pID = (DWORD)-1, BOOL bRefresh = FALSE );

	BOOL Refresh();

public:
	CList< THREAD_INFORMATION, THREAD_INFORMATION& > m_ThreadInfos;
	DWORD m_processId;
};


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemHandleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemHandleInformation : public INtDll
{
public:
	enum {
		OB_TYPE_UNKNOWN = 0,
		OB_TYPE_TYPE = 1,
		OB_TYPE_DIRECTORY,
		OB_TYPE_SYMBOLIC_LINK,
		OB_TYPE_TOKEN,
		OB_TYPE_PROCESS,
		OB_TYPE_THREAD,
		OB_TYPE_UNKNOWN_7,
		OB_TYPE_EVENT,
		OB_TYPE_EVENT_PAIR,
		OB_TYPE_MUTANT,
		OB_TYPE_UNKNOWN_11,
		OB_TYPE_SEMAPHORE,
		OB_TYPE_TIMER,
		OB_TYPE_PROFILE,
		OB_TYPE_WINDOW_STATION,
		OB_TYPE_DESKTOP,
		OB_TYPE_SECTION,
		OB_TYPE_KEY,
		OB_TYPE_PORT,
		OB_TYPE_WAITABLE_PORT,
		OB_TYPE_UNKNOWN_21,
		OB_TYPE_UNKNOWN_22,
		OB_TYPE_UNKNOWN_23,
		OB_TYPE_UNKNOWN_24,
		//OB_TYPE_CONTROLLER,
		//OB_TYPE_DEVICE,
		//OB_TYPE_DRIVER,
		OB_TYPE_IO_COMPLETION,
		OB_TYPE_FILE                        
	} SystemHandleType;

public:
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

protected:
	typedef struct _GetFileNameThreadParam
	{
		HANDLE		hFile;
		CString*	pName;
		ULONG		rc;
	} GetFileNameThreadParam;

public:
	SystemHandleInformation( DWORD pID = (DWORD)-1, BOOL bRefresh = FALSE, LPCTSTR lpTypeFilter = NULL );
	~SystemHandleInformation();

	BOOL SetFilter( LPCTSTR lpTypeFilter, BOOL bRefresh = TRUE );
	const CString& GetFilter();
	
	BOOL Refresh();

public:
	//Information functions
	static BOOL GetType( HANDLE, WORD&, DWORD processId = GetCurrentProcessId() );
	static BOOL GetTypeToken( HANDLE, CString&, DWORD processId = GetCurrentProcessId() );
	static BOOL GetTypeFromTypeToken( LPCTSTR typeToken, WORD& type );
	static BOOL GetNameByType( HANDLE, WORD, CString& str, DWORD processId = GetCurrentProcessId());
	static BOOL GetName( HANDLE, CString&, DWORD processId = GetCurrentProcessId() );

	//Thread related functions
	static BOOL GetThreadId( HANDLE, DWORD&, DWORD processId = GetCurrentProcessId() );

	//Process related functions
	static BOOL GetProcessId( HANDLE, DWORD&, DWORD processId = GetCurrentProcessId() );
	static BOOL GetProcessPath( HANDLE h, CString& strPath, DWORD processId = GetCurrentProcessId());

	//File related functions
	static BOOL GetFileName( HANDLE, CString&, DWORD processId = GetCurrentProcessId() );

public:
	//For remote handle support
	static HANDLE OpenProcess( DWORD processId );
	static HANDLE DuplicateHandle( HANDLE hProcess, HANDLE hRemote );

protected:
	static void GetFileNameThread( PVOID /* GetFileNameThreadParam* */ );
	BOOL IsSupportedHandle( SYSTEM_HANDLE& handle );

public:
	CList< SYSTEM_HANDLE, SYSTEM_HANDLE& > m_HandleInfos;
	DWORD	m_processId;

protected:
	CString	m_strTypeFilter;
};
#endif