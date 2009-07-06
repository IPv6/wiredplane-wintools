// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_MGestures.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _DEBUG
#define TRACETHREAD	TRACE3("\nCheckpoint: Thread: 0x%04x, file=%s, line=%lu\n",::GetCurrentThreadId(),__FILE__,__LINE__);
#else
#define TRACETHREAD
#endif

#define FORK(func,param) \
	{\
		DWORD dwThread=0;\
		HANDLE hThread=::CreateThread(NULL, 0, func, LPVOID(param), 0, &dwThread);\
		::CloseHandle(hThread);\
	}

CString _pl(const char* szText);
WKCallbackInterface*& WKGetPluginContainer();
extern HANDLE hStopEvent;
extern HINSTANCE g_hinstDll;
extern HANDLE hHookerThread;
extern CRITICAL_SECTION csMainThread;
/*
char* GetSubString(const char* szFrom, const char* szBegin, char cEnd, char* szTarget, const char* szDefaultValue="", int iOutBufLen=16)
{
	strcpy(szTarget,szDefaultValue);
	char* szPos=strstr(szFrom,szBegin);
	if(szPos){
		szPos=szPos+strlen(szBegin);
		char* szEnd=strchr(szPos,cEnd);
		if(szEnd){
			int iLen=szEnd-szPos;
			if(iLen>iOutBufLen){
				iLen=iOutBufLen-1;
			}
			memcpy(szTarget,szPos,iLen);
			szTarget[iLen]=0;
		}
	}
	return szTarget;
}*/

BOOL bNotStop=0;
extern BOOL bShowInOSD;
extern BOOL bShowTrail;
//extern BOOL bStepTime;
extern CMLNet Recognizer;
DWORD WINAPI DepressKey(LPVOID pData)
{
	char c=(char)pData;
	char sz1[]="MOGA";
	sz1[3]='A'+c;
	WKGetPluginContainer()->NotifyEventStarted(sz1);
	if(bShowInOSD){
		CString sText=_pl("Mouse gesture");
		sText+=": ";
		sText+=_pl(CMLNet::getPatternName(c));
		WKGetPluginContainer()->ShowOSD(sText,3000);//bStepTime>0?bStepTime*1000:3000
	}
	Sleep(500);
	WKGetPluginContainer()->NotifyEventFinished(sz1);
	return 0;
}

class CDMark
{
public:
	int iX;
	int iY;
	int iX0;
	int iY0;
	CDC* dcScreen;
	CPen pn;
	CDMark(int x,int y,int x0,int y0)
	{
		iX=x;
		iY=y;
		iX0=x0;
		iY0=y0;
		dcScreen=new CDC();
		dcScreen->CreateDC("DISPLAY", NULL, NULL, NULL); 
		pn.CreatePen(0,4,RGB(0,0,0));
		dcScreen->SelectObject(&pn);
		dcScreen->SetROP2(R2_NOT);
		//dcScreen.FillSolidRect(iX-5,iY-5,10,10,0);
		dcScreen->MoveTo(iX0,iY0);
		dcScreen->LineTo(iX,iY);
	}
	~CDMark()
	{
		dcScreen->SetROP2(R2_NOT);
		//dcScreen.FillSolidRect(iX-5,iY-5,10,10,0);
		dcScreen->MoveTo(iX0,iY0);
		dcScreen->LineTo(iX,iY);
		delete dcScreen;
	}
};

int iNumber=0;
BOOL bGestureInAction=0;
#ifdef _DEBUG
#define FLOG1(x,z1)			{FILE* f=fopen("c:\\wk_debug.txt","a+");if(f){fprintf(f,x,z1);fprintf(f,"\n");fclose(f);};};
#else
#define FLOG1(x,z1)
#endif
DWORD WINAPI GlobalHooker_Gestures(LPVOID pData)
{
	int iCounter=iNumber++;
	FLOG1(">>> Entering gestures recognition %i\n",iCounter);
	::EnterCriticalSection(&csMainThread);
	char c=0;
	/*
	CPoint pt0;
	::GetCursorPos(&pt0);*/
	BOOL bNotStopStart=bNotStop;
	bGestureInAction=2;
	Recognizer.m_path.clear();
	Recognizer.m_path2.clear();
	if(bShowInOSD){
		WKGetPluginContainer()->ShowOSD(_pl("Draw gesture please"),30000);
	}
	POINT ptPrev;
	GetCursorPos(&ptPrev);
	CArray<CDMark*,CDMark*> aMarks;
	while(bNotStopStart==bNotStop && Recognizer.m_path.size()<1024){
		POINT pt;
		GetCursorPos(&pt);
		Recognizer.m_path.push_back(pt);
		if(bShowTrail){
			if(abs(pt.x-ptPrev.x)>10 || abs(pt.y-ptPrev.y)>10){
				aMarks.Add(new CDMark(pt.x,pt.y,ptPrev.x,ptPrev.y));
				ptPrev.x=pt.x;
				ptPrev.y=pt.y;
			}
		}
		Sleep(20);
	}
	for(int im=0;im<aMarks.GetSize();im++){
		delete aMarks[im];
	}
	POINT pt;
	GetCursorPos(&pt);
	Recognizer.m_path.push_back(pt);
	// Собрали информацию!!! Обрабатываем
	if(bShowInOSD){
		Sleep(50);
		WKGetPluginContainer()->ShowOSD("",100);
	}
	c=Recognizer.RecognizeBoard();
	if(c>=0){
		FORK(DepressKey,c);
	}
	bGestureInAction=0;
	::LeaveCriticalSection(&csMainThread);
	FLOG1(">>> Leaving gestures recognition %i\n",iCounter);
	return 0;
}
