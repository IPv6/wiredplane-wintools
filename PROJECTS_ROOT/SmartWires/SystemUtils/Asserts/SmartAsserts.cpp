// Достаточно добавить этот файл в проект и все!!
// После этого все ассерты будут выдавать ошибку с полным стеком вызова в особый файл 
// - "debughelper.log" в корне упавшей программы
// Только для работы будет требоваться debughelper.dll и pdb файлы
// Без pdb файлов в лог тоже будет писаться информация из стека, правда без названий функций
#ifdef _DEBUG
#ifndef _SMARTASSERT_H_
#define _SMARTASSERT_H_

#include <afx.h>
#include <afxwin.h>
#include <afxext.h>

#ifndef DEBUG_NEW
// Для поиска утечек памяти
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// Нужно чтобы компилировалось один раз - в stdafx.cpp
#define DEBUG_HELPER "debughelper"
#define DA_USEDEFAULTS      0x0000
#define DA_SHOWMSGBOX       0x0001
#define DA_SHOWODS          0x0002
#define DA_SHOWSTACKTRACE   0x0004
// Prototype...
int DebugHelper(int nRptType, char *szMsg, int *retVal);
typedef BOOL (FAR WINAPI *FARDA_PROC)(DWORD dwOverrideOpts, LPCTSTR szMsg, char* pOutBuf, long lOutBufSize);

// Class...
class CAssertWrapper
{
	HINSTANCE m_hInstance;
public:
	CAssertWrapper()
	{
		// Грузим длл
		m_hInstance=LoadLibrary(DEBUG_HELPER".dll");
		if(m_hInstance){
			// Устанавливаем хук на ассерты
			_CrtSetReportHook(DebugHelper);
		}
	};

	~CAssertWrapper()
	{
		_CrtSetReportHook(NULL);
		if(m_hInstance){
			FreeLibrary(m_hInstance);
			m_hInstance=NULL;
		}
	};

	BOOL WriteAssert(const char* szText)
	{
		if(!m_hInstance){
			m_hInstance=LoadLibrary(DEBUG_HELPER".dll");
		}
		BOOL bRes=FALSE;
		if(m_hInstance){
			char szStackData[4096]="Stack gathering failed";
			FARDA_PROC fp=(FARDA_PROC)GetProcAddress(m_hInstance,"DiagAssert");
			if(fp){
				int iBoxRes=(*fp)(DA_SHOWSTACKTRACE|DA_SHOWMSGBOX, szText , szStackData, sizeof(szStackData));
				if(::GetAsyncKeyState(VK_SHIFT)<0){
					AfxMessageBox(szStackData);
				}
				// Сохраняем!!! - после мессадж бокса, так как текст сохранения идет именно из него
				HANDLE pFile=CreateFile(
					"debughelp.log", 
					GENERIC_WRITE, 
					0,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,
					NULL);
				if(pFile){
					SetFilePointer(pFile,-2,0,FILE_END);
					DWORD dwWritten=0;
					WriteFile(pFile, szStackData, strlen(szStackData), &dwWritten, NULL);
					CloseHandle(pFile);
					// Все ок
					bRes=TRUE;
				}
				if (iBoxRes == IDRETRY){
					DebugBreak();
				}
				if (iBoxRes == IDABORT){
					ExitProcess ( (UINT)-1 ) ;
				}
			}else{
				m_hInstance=NULL;
			}
		}
		return bRes;
	}
};

#pragma message("\nWarning: Including SmartAssert!! <---------------------\n\n")
__declspec ( dllexport ) CAssertWrapper assertWrapperObject;
int DebugHelper(int nRptType, char *szMsg, int *retVal)
{
	BOOL bHandled=FALSE;
	if(nRptType!=_CRT_WARN){//_CRT_ERROR,_CRT_ASSERT
		_CrtSetReportHook(NULL);// Чтобы небыло супер циклов..
		bHandled=assertWrapperObject.WriteAssert(szMsg);
		_CrtSetReportHook(DebugHelper);
	}
	return bHandled;
}

#endif
#endif