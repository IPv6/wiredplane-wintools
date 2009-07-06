#if !defined(H_MACROSES_IPV6_2)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define H_MACROSES_IPV6_2
class CDebugWPHelper
{
public:
	static BOOL& isDebug(BOOL bType=0)
	{
		static BOOL bDEBUG=-1;
		BOOL bRes=bDEBUG;
		if(bRes==-1){
			bRes=0;
			char szCurrentExeFile[MAX_PATH]="";
			GetModuleFileName(NULL, szCurrentExeFile, sizeof(szCurrentExeFile));
			char szFILE[MAX_PATH];
			_splitpath(szCurrentExeFile, 0, 0, szFILE, NULL);
			if(GlobalFindAtom("WPLABSDEBUG-WC")!=0 && (bType==1 || stricmp(szFILE,"wirechanger")==0)){
				bRes=1;
			}
			if(GlobalFindAtom("WPLABSDEBUG-WK")!=0 && (bType==2 || stricmp(szFILE,"wirekeys")==0)){
				bRes=1;
			}
			bDEBUG=bRes;
		}
		return bDEBUG;
	}
	
	static FILE* OpenDebgFile()
	{
		FILE* f=fopen("debug.log","a+");
		if(f){
			time_t tmt=time(NULL);fprintf(f,"%s %%%04X: ",ctime(&tmt),GetCurrentThreadId());
		}
		return f;
	}
	
	static void CloseDebgFile(FILE* f,const char* szFile, DWORD dwLine)
	{
		if(f){
			fprintf(f," [%s:%lu]\n",szFile,dwLine);
			fclose(f);
			flushall();
		}
	}

};

// Макрос для логирования ошибки на диск
#define BEGIN_DEBUG_WRITE				if(CDebugWPHelper::isDebug()){FILE* f=CDebugWPHelper::OpenDebgFile();
#define END_DEBUG_WRITE					CDebugWPHelper::CloseDebgFile(f,__FILE__,__LINE__);};

#define DEBUG_INFO2(x,z1,z2)			BEGIN_DEBUG_WRITE;if(f){fprintf(f,x,z1,z2);};END_DEBUG_WRITE;
#define DEBUG_INFO3(x,z1,z2,z3)			BEGIN_DEBUG_WRITE;if(f){fprintf(f,x,z1,z2,z3);};END_DEBUG_WRITE;
#define DEBUG_INFO4(x,z1,z2,z3,z4)		BEGIN_DEBUG_WRITE;if(f){fprintf(f,x,z1,z2,z3,z4);};END_DEBUG_WRITE;
#define DEBUG_INFO5(x,z1,z2,z3,z4,z5)	BEGIN_DEBUG_WRITE;if(f){fprintf(f,x,z1,z2,z3,z4,z5);};END_DEBUG_WRITE;
#define DBG_GETPROC(hLib,sName)			::GetProcAddress(hLib,sName);if(CDebugWPHelper::isDebug()){if(::GetProcAddress(hLib,sName)==0){DEBUG_INFO2("GetProcAddress failed: 0x%x:%s",hLib,sName);};};

#endif


