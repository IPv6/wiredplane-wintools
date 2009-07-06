#if !defined(H_MACROSES_IPV6)
#define H_MACROSES_IPV6
#define SUPPORT_EMAIL	"support@wiredplane.com"
#define DOWNLOAD_URL	"www.wiredplane.com"
#define	COMPANY_NAME	"WiredPlane"
#define LOGTIMEFRMT		"%d.%m.%Y %H:%M:%S"
#define BUFSIZE			1024
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a)   ((a<0)?-a:a)
// Pragma message helper macros
#define chSTR2(x)	#x
#define chSTR(x)	chSTR2(x)
#define TODO(desc)	message(__FILE__ "(" chSTR(__LINE__) "):" #desc)
// Windows units conversions
#define HIMETRIC_PER_INCH   2540
#ifndef MAP_LOGHIM_TO_PIX
#define MAP_PIX_TO_LOGHIM(x,ppli)   ( (HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli) )
#define MAP_LOGHIM_TO_PIX(x,ppli)   ( ((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH )
#endif
// This macro returns TRUE if a number is between two others
#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))
// This macro evaluates to the number of elements in an array. 
#define chDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))
// Quick MessageBox Macro
inline void chMB(const char* s) {
   char szTMP[128];
   GetModuleFileNameA(NULL, szTMP, chDIMOF(szTMP));
   MessageBoxA(GetActiveWindow(), s, szTMP, MB_OK);
}
// Assert/Verify Macros //////////////////////////////////////
inline void chFAIL(PSTR szMsg) {
   chMB(szMsg);
   DebugBreak();
}

// Put up an assertion failure message box.
inline void chASSERTFAIL(LPCSTR file, int line, PCSTR expr) {
   char sz[128];
   wsprintfA(sz, "File %s, line %d : %s", file, line, expr);
   chFAIL(sz);
}

// Put up a message box if an assertion fails in a debug build.
#ifdef _DEBUG
#define chASSERT(x) if (!(x)) chASSERTFAIL(__FILE__, __LINE__, #x)
#else
#define chASSERT(x)
#endif

// Assert in debug builds, but don't remove the code in retail builds.
#ifdef _DEBUG
#define chVERIFY(x) chASSERT(x)
#else
#define chVERIFY(x) (x)
#endif
// DebugBreak Improvement for x86 platforms
#ifdef _X86_
#define DebugBreak()    _asm { int 3 }
#endif
/*// This function forces the debugger to be invoked
#ifdef _DEBUG
void ForceDebugBreak() {
   __try { DebugBreak(); }
   __except(UnhandledExceptionFilter(GetExceptionInformation())) { }
}
#else
#define ForceDebugBreak()
#endif*/
// Useful macro for creating your own software exception codes
#define MAKESOFTWAREEXCEPTION(Severity, Facility, Exception) \
   ((DWORD) ( \
   /* Severity code    */  (Severity       ) |     \
   /* MS(0) or Cust(1) */  (1         << 29) |     \
   /* Reserved(0)      */  (0         << 28) |     \
   /* Facility code    */  (Facility  << 16) |     \
   /* Exception code   */  (Exception <<  0)))
////////////////////////////////////////////////////////////////
// OS Version Check Macros
inline BOOL isWin9x() {
   OSVERSIONINFO vi = { sizeof(vi) };
   GetVersionEx(&vi);
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
      return TRUE;
   }
   return FALSE;
}

inline void chWindows9xNotAllowed() {
   OSVERSIONINFO vi = { sizeof(vi) };
   GetVersionEx(&vi);
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
      chMB("This application requires features not present in Windows 9x.");
      ExitProcess(0);
   }
}

inline void chWindows2000Required() {
   OSVERSIONINFO vi = { sizeof(vi) };
   GetVersionEx(&vi);
   if ((vi.dwPlatformId != VER_PLATFORM_WIN32_NT) && (vi.dwMajorVersion < 5)) {
      chMB("This application requires features present in Windows 2000.");
      ExitProcess(0);
   }
}

#define	SHUTDOWN_NAME	"WK_SHUTDOWN"
inline BOOL SmartUnhookWindowsHookEx(HHOOK g_hhook, BOOL* bNoNeedToWait=NULL)
{
	if(bNoNeedToWait){
		*bNoNeedToWait=FALSE;
	}
	HANDLE hMutexOneInstance = ::CreateMutex( NULL, TRUE, SHUTDOWN_NAME);
	DWORD dwLastErr=GetLastError();
	CloseHandle(hMutexOneInstance);
    if(dwLastErr == ERROR_ALREADY_EXISTS){
		// Система выключается и хуки уже по барабану
		if(bNoNeedToWait){
			*bNoNeedToWait=TRUE;
		}
    	return TRUE;
    }
	// Обычное снятие хука...
	BOOL bRes=TRUE;
	if(g_hhook){
		bRes=UnhookWindowsHookEx(g_hhook);
	}
	//#pragma TODO(check situation ::SendMessageTimeout HWND_BROADCAST)
	/*if(bWithSendMsg){
		DWORD dwRes=0;
		::SendMessageTimeout(HWND_BROADCAST, WM_NULL, 0, 0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 100, &dwRes);
	}*/
	return bRes;
}

// Макрос - начало потока
#ifdef _DEBUG
#ifdef TRACE3
#define TRACETHREAD	TRACE3("\nWP Thread started: %i, file=%s, line=%lu\n",::GetCurrentThreadId(),__FILE__,__LINE__);
#define TRACETHREAD1(x)	TRACE3("\nWP Thread created: %i, file=%s, line=%lu\n",x,__FILE__,__LINE__);
#endif
#endif

#ifndef TRACETHREAD
#define TRACETHREAD
#define TRACETHREAD1(x)
#endif

// Макрос для вызова функции в отдельном потоке //TRACETHREAD1(dwThread);
#define FORK(func,param) \
	{\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func, LPVOID(param), 0, &dwThread);\
		::CloseHandle(hThread);\
	}
// Макрос для вызова функции в отдельном потоке //ATLTRACE("[DEBUG THREAD: FORKING2 %i]",dwThread);
#define FORK_WAIT(func,param,timeout) \
	{\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func, LPVOID(param), 0, &dwThread);\
		::WaitForSingleObject(hThread,timeout);\
		::CloseHandle(hThread);\
	}
// Макрос для вызова обработчика в отдельном потоке
#define HANDLEINTHREAD(classname,func) \
	DWORD WINAPI func##_InThread(LPVOID pValue);\
	afx_msg void classname::func()\
	{\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func##_InThread, NULL, 0, &dwThread);\
		ATLTRACE("[DEBUG THREAD: FORKING3 %i]",dwThread);\
		::CloseHandle(hThread);\
	}\
	DWORD WINAPI func##_InThread(LPVOID pValue)
//
// Макрос для вызова обработчика с одним параметром - this - в отдельном потоке
#define HANDLEINTHREAD2(classname,func) \
	DWORD WINAPI func##_InThread(LPVOID pThis);\
	afx_msg void classname::func()\
	{\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func##_InThread, LPVOID(this), 0, &dwThread);\
		ATLTRACE("[DEBUG THREAD: FORKING4 %i]",dwThread);\
		::CloseHandle(hThread);\
	}\
	DWORD WINAPI func##_InThread(LPVOID pThis)
// Макрос для вызова обработчика в отдельном потоке с выполнением одной функции перед вызовом
#define HANDLEINTHREAD3(classname,func,statement) \
	DWORD WINAPI func##_InThread(LPVOID pValue);\
	afx_msg void classname::func()\
	{\
		{statement;};\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func##_InThread, NULL, 0, &dwThread);\
		ATLTRACE("[DEBUG THREAD: FORKING5 %i]",dwThread);\
		::CloseHandle(hThread);\
	}\
	DWORD WINAPI func##_InThread(LPVOID pValue)
// Макрос для логирования ошибки на диск
//#define DEBUG_LOG3(x,z1,z2,z3)	{FILE* f=fopen("c:\\wiredplane_debug.txt","a+");if(f){time_t tmt=time(NULL);fprintf(f,"%s %s(%lu): ",ctime(&tmt),__FILE__,__LINE__);fprintf(f,x,z1,z2,z3);fprintf(f,"\n");fclose(f);};};
//#define DEBUG_LOG4(x,z1,z2,z3,z4)	{FILE* f=fopen("c:\\wiredplane_debug.txt","a+");if(f){time_t tmt=time(NULL);fprintf(f,"%s %s(%lu): ",ctime(&tmt),__FILE__,__LINE__);fprintf(f,x,z1,z2,z3,z4);fprintf(f,"\n");fclose(f);};};

#include <time.h>
// Простой класс для синхронизации
class SimpleTracker
{
public:
	long* bValue;
	SimpleTracker(long& b){
		bValue=&b;
		InterlockedIncrement(bValue);
	};
	~SimpleTracker(){
		InterlockedDecrement(bValue);
	};
};
#define RETURN_IFRECURSE(x)	static long l##__LINE__=0;if(l##__LINE__>0) return x;SimpleTracker lc##__LINE__(l##__LINE__);

#ifdef __AFXMT_H__
class SimpleLocker
{
	CSyncObject* m_pObject;
public:
	SimpleLocker(CSyncObject* pObject)
	{
		m_pObject=pObject;
		m_pObject->Lock();
	};
	~SimpleLocker()
	{
		m_pObject->Unlock();
	};
	void lock()
	{
		m_pObject->Lock();
	};
	void unlock()
	{
		m_pObject->Unlock();
	};
};
#define CLocker SimpleLocker
#endif
// Проверка на наличие числа в массиве: if(ISIN(aaa,))
#define ISIN2(x,y,z) ((x)==y || (x)==z)
#define GETCURTIME	COleDateTime(time(NULL))
#include "Debug.h"
////////////////////////////////////////////////////////////////
// Force Windows subsystem
#ifndef _CONSOLE
#pragma comment(linker, "/subsystem:Windows")
#endif
#endif // H_MACROSES_IPV6


