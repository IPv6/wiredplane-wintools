/////////////////////////////////////////////////////////////
// hooklib.c
// серверная часть
// #define	KEY_DEBUG
#define __GLOBAL_HOOK
#include <windows.h>
#include <stdio.h>
#include <memory.h> 
//#include "..\..\..\SmartWires\SystemUtils\Macros.h"
#include "..\..\..\SmartWires\SystemUtils\Hotkeys_def.h"
//------------------------------------
#define BUF_MUTEX		"WK_KEYBMUTEX"
#define BUF_MUTEX2		"WK_KEYBMUTEX2"
#pragma data_seg("Shared")
DWORD	dwAutoHideLimit=0;
long	lHookActive=0;
BOOL	bActive=0;
DWORD	dwFlags=0;
DWORD	g_LastNcHit=0;
DWORD	g_dwLastHitTime=0;
BYTE	baKeyState[KBBUFFER_DSIZE]={0};
DWORD	baBlockKeys[KBBUFFER_DSIZE]={0};
DWORD	g_dwPenaltyTime=0;
HHOOK	g_hhook	= NULL;
HHOOK	g_hhookM = NULL;
HHOOK	g_hhookM2 = NULL;
HWND	g_hTargetWnd=NULL;
HWND	hPromiscModeWnd=0;
RECT	rcPromiscModeRect={0};
DWORD	dwLastKeybActionTime=0;
WPARAM	wLastKB_WPARAM=0;
LPARAM	lLastKB_LPARAM=0;
DWORD	dwBordersAutoRepeatLastTim=0;
DWORD	dwBordersAutoRepeatLastAct=0;
RECT	rDesktopRECT={0,0,0,0};
DWORD	dwRectCheckTime=0;
DWORD	dwDblClcTime=0;
DWORD	dwDblClcType=0;
DWORD	dwDblClcMaxTime=500;
DWORD	dwPressnHoldTime=0;
DWORD	dwPressnholdMaxTimeout=1000;
DWORD	dwNxLastKey=0;
DWORD	dwNxLastKeyCnt=0;
DWORD	dwNxLastKeyTime=0;
DWORD	dwNxLastPState=0;
long	g_dwOkHOOKNumber=0;
#pragma data_seg()
// Для хуков, работающих в текущем контексте, запоминаем
// Показатель активного хука!
long dwOkHOOKNumber=0;

inline BOOL isWin9x() {
   OSVERSIONINFO vi = { sizeof(vi) };
   GetVersionEx(&vi);
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
      return TRUE;
   }
   return FALSE;
}

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

#pragma comment(linker, "/section:Shared,rws")
HINSTANCE g_hinstDll = NULL;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hinstDll = (HINSTANCE)hModule;
			break;
	
		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

void GetWindowTopParent(HWND& hWnd)
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

BOOL WINAPI SetKBBlockKeys(DWORD* btIn)
{
	HANDLE hMutex=CreateMutex(NULL,TRUE,BUF_MUTEX2);
	int iSz=sizeof(baBlockKeys[0]);
	memcpy(baBlockKeys,btIn,KBBUFFER_DSIZE*iSz);
	CloseHandle(hMutex);
	return TRUE;
}

BOOL WINAPI GetKBBufferState(BYTE* btOut)
{
	HANDLE hMutex=CreateMutex(NULL,TRUE,BUF_MUTEX);
	memcpy(btOut,baKeyState,sizeof(baKeyState[0])*KBBUFFER_DSIZE);
	CloseHandle(hMutex);
	return TRUE;
}

void DropNotAutodKeys()
{
	baKeyState[WHEEL_DO]=0;
	baKeyState[WHEEL_UP]=0;
	baKeyState[WHEEL_CL]=0;
	baKeyState[VK_MBT_DBL]=0;
	baKeyState[VK_LBT_DBL]=0;
	baKeyState[VK_RBT_DBL]=0;
	baKeyState[VK_MBORDER_T]=0;
	baKeyState[VK_MBORDER_B]=0;
	baKeyState[VK_MBORDER_R]=0;
	baKeyState[VK_MBORDER_L]=0;
	baKeyState[VK_MCORNER_RT]=0;
	baKeyState[VK_MCORNER_RB]=0;
	baKeyState[VK_MCORNER_LT]=0;
	baKeyState[VK_MCORNER_LB]=0;
	baKeyState[VK_MOB_MLES]=0;
	baKeyState[VK_MOB_HLP]=0;
	baKeyState[VK_MOB_ICO]=0;
	baKeyState[VK_MOB_MAX]=0;
	baKeyState[VK_MOB_MIN]=0;
	baKeyState[VK_MOB_CLOSE]=0;
	baKeyState[VK_MO_TITLE]=0;
	baKeyState[VK_FJ_KEY]=0;
}

void WINAPI ResetKBBuffer(BOOL bNotAutodOnly)
{
	HANDLE hMutex=CreateMutex(NULL,TRUE,BUF_MUTEX);
	if(bNotAutodOnly){
		DropNotAutodKeys();
		if(bNotAutodOnly==2 || bNotAutodOnly==3){//3-gjckt ujh/ rkfdbib!!!
			baKeyState[VK_2X]=0;
			baKeyState[VK_3X]=0;
		}
		if(bNotAutodOnly==3){
			dwAutoHideLimit=0;
		}
	}else{
		memset(baKeyState,0,sizeof(baKeyState[0])*KBBUFFER_DSIZE);
	}
	dwNxLastKeyCnt=0;
	dwNxLastKey=0;
	dwNxLastKeyTime=0;
	dwPressnHoldTime=0;
	dwNxLastPState=0;
	dwLastKeybActionTime=0;
	CloseHandle(hMutex);
}

#ifndef CURSOR_SHOWING
typedef struct {
    DWORD cbSize;
    DWORD flags;
    HCURSOR hCursor;
    POINT ptScreenPos;
} CURSORINFO, *PCURSORINFO, *LPCURSORINFO;
#endif
// From WKKeyMaster
#define MSGKB_JUSTINFORM	WM_USER+99
#define MSGKB_CURSORHIDE	WM_USER+100
#define MSGKB_JUSTINFORM_MM WM_USER+104
#define MSGKB_JUSTINFORM_EV	WM_USER+105
typedef DWORD (WINAPI *_GetLastHitTest)();
LRESULT ProcessKeyboardMessage(WPARAM wParam,LPARAM lParam, BOOL bFromMouse, DWORD dwRawScan=DWORD(-1))
{
	// Автоскрытие курсора
	if(((dwFlags&1)!=0) && ((!bFromMouse && wParam!=VK_CONTROL && wParam!=VK_SHIFT && wParam!=VK_MENU && wParam!=VK_RCONTROL && wParam!=VK_RSHIFT && wParam!=VK_RMENU && wParam!=VK_LCONTROL && wParam!=VK_LSHIFT && wParam!=VK_LMENU)
		|| wParam==WHEEL_DO || wParam==WHEEL_UP)){
		// Скрываем...
		if(dwAutoHideLimit<10){
			dwAutoHideLimit++;
		}else{
			::PostMessage(g_hTargetWnd,MSGKB_CURSORHIDE,0,0);
			dwAutoHideLimit=0;
		}
	}
	BOOL bPressAndHoldEnabled=0;
	BOOL bExtended=(1&(lParam>>24));
	BOOL bNotPressed=(1&(lParam>>31));
	DWORD dwScanCode=(lParam>>16)&0xFF;
	UINT uVkKey=wParam;
	BOOL bHonestAutoRepeat=0;
	if(wParam!=0){// Устанавливаем статус кнопки
		if(hPromiscModeWnd==NULL && dwLastKeybActionTime!=0 
			&& GetTickCount()-dwLastKeybActionTime>dwPressnholdMaxTimeout+1000){
			if(dwPressnholdMaxTimeout!=0 || GetTickCount()-dwLastKeybActionTime>60*1000){
				ResetKBBuffer(0);
			}else{
				ResetKBBuffer(2);
			}
			dwLastKeybActionTime=0;
		}
		if(uVkKey==VK_CONTROL){
			if(bExtended){
				uVkKey=VK_RCONTROL;
			}else{
				uVkKey=VK_LCONTROL;
			}
		}
		if(uVkKey==VK_SHIFT){
			if(dwScanCode==54){
				uVkKey=VK_RSHIFT;
			}else{
				uVkKey=VK_LSHIFT;
			}
		}
		if(uVkKey==VK_MENU){
			if(bExtended){
				uVkKey=VK_RMENU;
			}else{
				uVkKey=VK_LMENU;
			}
		}
		if(wParam==255 && bExtended){
			uVkKey=BYTE(dwScanCode)+KBBUFFER_SIZE;
		}
		baKeyState[VK_2X]=0;
		baKeyState[VK_3X]=0;
		if(uVkKey<=KBBUFFER_DSIZE){
			HANDLE hMutex=CreateMutex(NULL,TRUE,BUF_MUTEX);
			if(uVkKey!=VK_NULL){
				BYTE bNewValue=bNotPressed?0:0x80;
				if(dwPressnholdMaxTimeout>0){
					if(!bNotPressed && baKeyState[uVkKey]==bNewValue){
						// Это все работает на факте автоповтора
						// нажатой клавиши в Windows!
						if(dwPressnHoldTime==0){
							dwPressnHoldTime=GetTickCount();
						}else if(GetTickCount()-dwPressnHoldTime>dwPressnholdMaxTimeout){
							bPressAndHoldEnabled=TRUE;
							baKeyState[VK_PRESSNHOLD]=0x80;
							dwPressnHoldTime=0;
						}
					}
				}
				// Поддержка 2Х и 3Х
				if(bNotPressed){
					dwNxLastPState=0;
				}else{
					if(uVkKey==dwNxLastKey && GetTickCount()-dwNxLastKeyTime<(dwPressnHoldTime>0?dwPressnHoldTime:1000) && baKeyState[VK_PRESSNHOLD]==0){
						dwNxLastKeyCnt++;
					}else{
						dwNxLastKey=0;
						dwNxLastKeyCnt=0;
						dwNxLastKey=uVkKey;
					}
					dwNxLastKeyTime=GetTickCount();
					if(dwNxLastPState==0){
						dwNxLastPState=1;
					}
				}
				if(!bNotPressed){
					if(dwNxLastKeyCnt==1){
						baKeyState[VK_2X]=0x80;
					}else if(dwNxLastKeyCnt==2){
						baKeyState[VK_3X]=0x80;
					}else if(dwNxLastKeyCnt==3){
						baKeyState[VK_2X]=0x80;
					}else{
						dwNxLastKeyCnt=0;
					}
				}
				// Сасываем лишние клавиши
				DropNotAutodKeys();
				// Все, заносим значение!
				if(!bNotPressed && baKeyState[uVkKey]==bNewValue){
					bHonestAutoRepeat=1;
				}
				baKeyState[uVkKey]=bNewValue;
				// Дополнение для нажатий кнопок мыши над заголовком и т.д.
				if(!bNotPressed)
				{
					if((uVkKey<=WHEEL_UP && uVkKey>=VK_RBT_DBL && !(uVkKey<=VK_MCORNER_RT && uVkKey>=VK_MBORDER_L)) || (uVkKey==VK_LBUTTON || uVkKey==VK_RBUTTON || uVkKey==VK_MBUTTON || uVkKey==VK_MOB_MLES)){
						// Положение над элементаи окна
						if(g_dwLastHitTime==0 || GetTickCount()-g_dwLastHitTime>200)
						{
							HWND hTop=0;
							BOOL bSkip=0;
							g_dwLastHitTime=GetTickCount();
							HINSTANCE hWPTModule=GetModuleHandle("wp_wtraits.wkp");
							if(hWPTModule){
								_GetLastHitTest fp=(_GetLastHitTest)GetProcAddress(hWPTModule,"GetLastHitTest");
								if(fp){
									g_LastNcHit=(*fp)();
									if(g_LastNcHit!=0){
										bSkip=1;
									}
								}
							}
							if(bSkip==0){
								POINT ptHit;
								GetCursorPos(&ptHit);
								HWND hUCursor=WindowFromPoint(ptHit);//pkbhs->hwnd;
								hTop=hUCursor;
								GetWindowTopParent(hTop);
								g_LastNcHit=DefWindowProc(hTop,WM_NCHITTEST,0,MAKELONG(ptHit.x,ptHit.y));
								if(g_LastNcHit==1 && hUCursor!=hTop){
									g_LastNcHit=DefWindowProc(hUCursor,WM_NCHITTEST,0,MAKELONG(ptHit.x,ptHit.y));
								}
							}
						}
						if(g_LastNcHit==HTCAPTION){
							baKeyState[VK_MO_TITLE]=0x80;
						}else if(g_LastNcHit==HTSYSMENU){
							baKeyState[VK_MOB_ICO]=0x80;
						}else if(g_LastNcHit==HTHELP || g_LastNcHit==HTOBJECT){
							baKeyState[VK_MOB_HLP]=0x80;
						}else if(g_LastNcHit==HTCLOSE){
							baKeyState[VK_MOB_CLOSE]=0x80;
						}else if(g_LastNcHit==HTMAXBUTTON){
							baKeyState[VK_MOB_MAX]=0x80;
						}else if(g_LastNcHit==HTMINBUTTON){
							baKeyState[VK_MOB_MIN]=0x80;
						}
					}
				}
			}
			if(bNotPressed || uVkKey==WHEEL_DO || uVkKey==WHEEL_UP || uVkKey==WHEEL_CL){
				baKeyState[VK_PRESSNHOLD]=0;
				dwPressnHoldTime=0;
			}
			CloseHandle(hMutex);
		}
	}
	BOOL bUseSend=1;
	DWORD bHotkey=0;
	dwLastKeybActionTime=GetTickCount();
#ifdef _DEBUG
	const char* sss="";
#endif
	if(bFromMouse==2){
		// Это движение мыши, по любому не блоким от него ничего...
		bHotkey=0;
		bUseSend=0;
		#ifdef _DEBUG
			sss="Posting message - mouse";
		#endif
	}else if((baBlockKeys[uVkKey] & BLOCKSTATE_BLOCK)!=0){
		#ifdef _DEBUG
			sss="Posting message - block";
		#endif
		bHotkey=1;
		bUseSend=0;
	}else if(!(baBlockKeys[uVkKey] & BLOCKSTATE_PROCESS)){
		#ifdef _DEBUG
			sss="Posting message - no proc";
		#endif
		bHotkey=0;
		bUseSend=0;
	}else if(g_dwPenaltyTime && GetTickCount()-g_dwPenaltyTime<5000){
		#ifdef _DEBUG
			sss="Posting message - penalty";
		#endif
		bUseSend=0;
	}
	if(bHonestAutoRepeat){
		lParam|=(1<<26);
	}
	if(bUseSend){
		// Пытаемся отправить
		if(!::SendMessageTimeout(g_hTargetWnd,MSGKB_JUSTINFORM,uVkKey,lParam,SMTO_ABORTIFHUNG,300,&bHotkey)){
			// Возможно перегрузка системы? Пенальти на 5 секунд
			g_dwPenaltyTime=GetTickCount();
			bUseSend=0;
		}else{
			g_dwPenaltyTime=0;
		}
	}
	if(!bUseSend){
		// Шлем на всякий случай - пусть там знают что что-то случилось...
		::PostMessage(g_hTargetWnd,MSGKB_JUSTINFORM,uVkKey,lParam);
	}
	{// Пост обработка
		static UINT lPrevVK=0;
		if(bHotkey){// Не даем сообщению пройти дальше
			if(bFromMouse && !bNotPressed){
				lPrevVK=uVkKey;
			}
			return 1;
		}
		if(bFromMouse){
			// Проверяем на скрытие мышимных отжажатий когда нажатие привело к гор. клаве
			if(lPrevVK==uVkKey && bNotPressed){
				lPrevVK=0;
				return 1;
			}
			lPrevVK=0;
		}
	}
	if(hPromiscModeWnd!=NULL){
		BOOL bPressed=!bNotPressed;
		BOOL bFromMouseButGood=bPressed && ((uVkKey>=VK_XBTTNWK4 && uVkKey<=WHEEL_UP) || (uVkKey>=VK_RBT_DBL && uVkKey<=VK_LBT_DBL));
		if(IsWindow(hPromiscModeWnd)){
			BOOL bNew=(lLastKB_LPARAM!=lParam || wLastKB_WPARAM!=wParam || uVkKey==VK_ESCAPE || uVkKey==VK_DELETE);
			if((!bFromMouse || bFromMouseButGood) && (bNew || bPressAndHoldEnabled)){
				::PostMessage(hPromiscModeWnd,WM_HOTKEY,uVkKey,bPressed);
				//LOG3("button sended!!!! w=%08x, l=%08x, res=%i",uVkKey,WHEEL_UP,99);
			}
			if(!bFromMouse){
				lLastKB_LPARAM=lParam;
				wLastKB_WPARAM=wParam;
			}
		}else{
			hPromiscModeWnd=NULL;
		}
		// Пропускаем некоторые мышинные события
		if(!bFromMouse || bFromMouseButGood){
			return 1;// Не даем сообщению пройти дальше
		}
	}
	return 0;
}

#include "../../WKPlugin.h"
LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning()){
		SimpleTracker lock(lHookActive);
		if (bActive && code==HC_ACTION && g_hTargetWnd!=NULL){
#ifdef KEY_DEBUG
			LOG3("KeyboardProc wPar=%08X, lPar=%08X",wParam,lParam,0);
#endif
			lParam=lParam&(~(1<<25));
			if(hPromiscModeWnd!=NULL){
				//if((dwFlags&2)!=0)
				{
					::PostMessage(g_hTargetWnd,WM_USER+101/*MSGKB_SHOWOSD*/,wParam,lParam);
				}
			}
			if(ProcessKeyboardMessage(wParam,lParam, 0)){
				return 1;
			}
		}
	}
	return CallNextHookEx( g_hhook, code, wParam, lParam );   
}

// Это все делается Как журналинг в контексте потока установившего хук!
// Соответсвенно длл по идее не нужна - но оставим как есть
//#include <winuser.h>
#define WH_KEYBOARD_LL			13
#define WH_MOUSE_LL				14
/*
 * Low level hook flags
 */
#define LLKHF_EXTENDED       0x00000001
#define LLKHF_INJECTED       0x00000010
#define LLKHF_ALTDOWN        0x00000020
#define LLKHF_UP             0x00000080
#define LLMHF_INJECTED       0x00000001
/*
 * Structure used by WH_KEYBOARD_LL
 */
typedef struct tagKBDLLHOOKSTRUCT {
    DWORD   vkCode;
    DWORD   scanCode;
    DWORD   flags;
    DWORD   time;
    DWORD   dwExtraInfo;
} KBDLLHOOKSTRUCT, FAR *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;
/*
 * Structure used by WH_MOUSE_LL
 */
typedef struct tagMSLLHOOKSTRUCT {
    POINT   pt;
    DWORD   mouseData;
    DWORD   flags;
    DWORD   time;
    DWORD   dwExtraInfo;
} MSLLHOOKSTRUCT, FAR *LPMSLLHOOKSTRUCT, *PMSLLHOOKSTRUCT;
LRESULT CALLBACK LowLevelKeyboardProc(int code,WPARAM wParam,LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning() && dwOkHOOKNumber==g_dwOkHOOKNumber){
		SimpleTracker lock(lHookActive);
		if (bActive && code==HC_ACTION && g_hTargetWnd!=NULL){
			KBDLLHOOKSTRUCT* pkbhs=(KBDLLHOOKSTRUCT*)lParam;
			if(pkbhs){
#ifdef KEY_DEBUG
				LOG4("--- LowLevelKeyboardProc --- ex=%08X, scan=%08X, vkCode=%08X, flags=%08X",pkbhs->dwExtraInfo,pkbhs->scanCode,pkbhs->vkCode,pkbhs->flags);
#endif
				WPARAM wParamKB=pkbhs->vkCode;
				LPARAM lParamKB=((pkbhs->scanCode)<<16);
				if(pkbhs->flags & LLKHF_UP){
					lParamKB|=(1<<31);
				}
				if(pkbhs->flags & LLKHF_EXTENDED){
					lParamKB|=(1<<24);
				}
				if(pkbhs->flags & LLKHF_INJECTED){
					lParamKB|=(1<<25);
				}
				if(hPromiscModeWnd!=NULL){
					//if((dwFlags&2)!=0)
					{
						::PostMessage(g_hTargetWnd,WM_USER+102/*MSGKB_SHOWOSD*/,wParamKB,lParamKB);
					}
				}
				if(ProcessKeyboardMessage(wParamKB,lParamKB, 0, pkbhs->scanCode)){
					return 1;
				}
			}
		}
	}
	return CallNextHookEx( g_hhook, code, wParam, lParam );   
}

long mabs(long l)
{
	if(l>=0){
		return l;
	}
	return -l;
}

#define WM_XBUTUP	0x20C
#define WM_XBUTDOWN	0x20B
#define _WHEEL_DELTA 120     /* Value for rolling one detent */
#define CORNER_SIZE	60
#define BORDER_SIZE	3
BOOL ProcessMouseMessage(WPARAM wParam, LPARAM lParam, BOOL bLL)
{
	POINT ptCheck;
	ptCheck.x=ptCheck.y=0;
	WPARAM wParamMS=0;
	LPARAM lParamMS=0;
	BOOL bMoveCommand=FALSE;
	if(bLL){
		MSLLHOOKSTRUCT* pkbhs=(MSLLHOOKSTRUCT*)lParam;
		ptCheck.x=pkbhs->pt.x;
		ptCheck.y=pkbhs->pt.y;
	}else{
		MOUSEHOOKSTRUCT* pkbhs=(MOUSEHOOKSTRUCT*)lParam;
		ptCheck.x=pkbhs->pt.x;
		ptCheck.y=pkbhs->pt.y;
		//dwNcHit=pkbhs->wHitTestCode;
	}
	/*if(dwPressnholdMaxTimeout>0){
		static DWORD dwMotionlessCount=0;
		static POINT pMotlessP={0};
		if(mabs(pMotlessP.x-ptCheck.x)>5 || mabs(pMotlessP.y-ptCheck.y)>5){
			dwMotionlessCount=GetTickCount();
			pMotlessP.x=ptCheck.x;
			pMotlessP.y=ptCheck.y;
			//baKeyState[VK_MOB_MLES]=0;
		}
		if(GetTickCount()-dwMotionlessCount>dwPressnholdMaxTimeout){
			dwMotionlessCount=GetTickCount();
			wParamMS=VK_MOB_MLES;
		}
	}*/
	if(wParam==WM_MOUSEMOVE){
		::PostMessage(g_hTargetWnd,MSGKB_JUSTINFORM_MM,wParam,lParam);
		if(wParamMS==0){
			// Проверяем углы
			if(dwRectCheckTime && (GetTickCount()-dwRectCheckTime>6000)){
				rDesktopRECT.left=rDesktopRECT.top=rDesktopRECT.right=rDesktopRECT.bottom=0;
			}
			if(rDesktopRECT.right==0 && rDesktopRECT.bottom==0){
				::GetWindowRect(::GetDesktopWindow(),&rDesktopRECT);
				dwRectCheckTime=GetTickCount();
			}
			if(ptCheck.x-rDesktopRECT.left<=CORNER_SIZE && ptCheck.y-rDesktopRECT.top<=CORNER_SIZE){
				wParamMS=VK_MCORNER_LT;
			}else if(ptCheck.x-rDesktopRECT.left<=CORNER_SIZE && rDesktopRECT.bottom-ptCheck.y<=CORNER_SIZE){
				wParamMS=VK_MCORNER_LB;
			}else if(rDesktopRECT.right-ptCheck.x<=CORNER_SIZE && ptCheck.y-rDesktopRECT.top<=CORNER_SIZE){
				wParamMS=VK_MCORNER_RT;
			}else if(rDesktopRECT.right-ptCheck.x<=CORNER_SIZE && rDesktopRECT.bottom-ptCheck.y<=CORNER_SIZE){
				wParamMS=VK_MCORNER_RB;
			}else if(rDesktopRECT.right-ptCheck.x<=BORDER_SIZE){
				wParamMS=VK_MBORDER_R;
			}else if(ptCheck.x-rDesktopRECT.left<=BORDER_SIZE){
				wParamMS=VK_MBORDER_L;
			}else if(ptCheck.y-rDesktopRECT.top<=BORDER_SIZE){
				wParamMS=VK_MBORDER_T;
			}else if(rDesktopRECT.bottom-ptCheck.y<=BORDER_SIZE){
				wParamMS=VK_MBORDER_B;
			}
		}
		if(wParamMS==0){
			if(baKeyState[VK_MCORNER_RT]!=0 || baKeyState[VK_MCORNER_RB]!=0 || baKeyState[VK_MCORNER_LT]!=0 || baKeyState[VK_MCORNER_LB]!=0
				|| baKeyState[VK_MBORDER_R]!=0 || baKeyState[VK_MBORDER_L]!=0 || baKeyState[VK_MBORDER_T]!=0 || baKeyState[VK_MBORDER_B]!=0 || baKeyState[VK_MOB_MLES]!=0){
				wParamMS=VK_NULL;
				lParamMS|=(1<<31);
			}
		}
		if(wParamMS!=VK_NULL && wParamMS!=0){
			if(dwBordersAutoRepeatLastTim!=0 && GetTickCount()-dwBordersAutoRepeatLastTim<700 && dwBordersAutoRepeatLastAct==wParamMS){
				// Игнорируем сообщение
				return 0;
			}
			dwBordersAutoRepeatLastAct=wParamMS;
			dwBordersAutoRepeatLastTim=GetTickCount();
		}
		bMoveCommand|=(wParamMS!=0);
	}
	if(wParamMS==0 || wParamMS==VK_NULL){
		if(wParam==WM_LBUTTONUP || wParam==WM_RBUTTONUP || wParam==WM_MBUTTONUP || wParam==WM_XBUTUP){
			lParamMS|=(1<<31);
		}
		if(wParam==WM_LBUTTONUP || wParam==WM_LBUTTONDOWN){
			wParamMS=VK_LBUTTON;
		}
		if(wParam==WM_RBUTTONUP || wParam==WM_RBUTTONDOWN){
			wParamMS=VK_RBUTTON;
		}
		if(wParam==WM_MBUTTONUP || wParam==WM_MBUTTONDOWN){
			wParamMS=VK_MBUTTON;
		}
		if(wParam==WM_MBUTTONUP || wParam==WM_MBUTTONDOWN){
			wParamMS=VK_MBUTTON;
		}
		if(wParam==WM_XBUTUP || wParam==WM_XBUTDOWN){
			wParamMS=VK_XBTTNWK1;
		}
		if(wParam==WM_LBUTTONDOWN || wParam==WM_RBUTTONDOWN || wParam==WM_MBUTTONDOWN){
			if(dwDblClcType!=wParam){
				dwDblClcTime=0;
			}
			if(dwDblClcTime!=0 && GetTickCount()-dwDblClcTime<dwDblClcMaxTime){
				dwDblClcTime=0;
				if(wParam==WM_LBUTTONDOWN){
					wParamMS=VK_LBT_DBL;
				}
				if(wParam==WM_RBUTTONDOWN){
					wParamMS=VK_RBT_DBL;
				}
				if(wParam==WM_MBUTTONDOWN){
					wParamMS=VK_MBT_DBL;
				}
			}
			dwDblClcTime=GetTickCount();
			dwDblClcType=wParam;
		}
		if(bLL){
			MSLLHOOKSTRUCT* pkbhs=(MSLLHOOKSTRUCT*)lParam;
			WORD wWd=HIWORD(pkbhs->mouseData);
			if(wParam==0x020A){//WM_MOUSEWHEEL WHEEL_DELTA
				//if(wWd == 120){wParamMS=WHEEL_CL;}
				if(wWd&0x8000){
					wParamMS=WHEEL_DO;
				}else{
					wParamMS=WHEEL_UP;
				}
			}
			if(wParam==WM_XBUTUP || wParam==WM_XBUTDOWN){
				if(wWd==2){
					wParamMS=VK_XBTTNWK2;
				}
			}
		}
	}
	// Автоскрытие курсора
	if(((dwFlags&1)!=0) && (wParamMS!=WHEEL_DO && wParamMS!=WHEEL_UP)){
		// Показываем
		//ShowCursor(1);
		::PostMessage(g_hTargetWnd,MSGKB_CURSORHIDE,0,1);
		dwAutoHideLimit=0;
	}
	if(hPromiscModeWnd!=NULL && !bMoveCommand){
		if(rcPromiscModeRect.left==0 && rcPromiscModeRect.top==0 && rcPromiscModeRect.right==0 && rcPromiscModeRect.bottom==0){
			::GetWindowRect(hPromiscModeWnd,&rcPromiscModeRect);
		}
		if(ptCheck.x<rcPromiscModeRect.left || ptCheck.y<rcPromiscModeRect.top){
			return FALSE;
		}
		if(ptCheck.x>rcPromiscModeRect.right || ptCheck.y>rcPromiscModeRect.bottom){
			return FALSE;
		}
	}
	if(wParamMS==0){
		//char s[100]="";
		//sprintf(s,"%04X",wParam);
		return FALSE;
	}
	if(hPromiscModeWnd!=NULL){
		//if((dwFlags&2)!=0)
		{
			::PostMessage(g_hTargetWnd,WM_USER+103/*MSGKB_SHOWOSD*/,wParamMS,lParamMS);
		}
	}
	return ProcessKeyboardMessage(wParamMS,lParamMS, (wParam==WM_MOUSEMOVE)?2:1) && !bMoveCommand;
}

LRESULT CALLBACK MouseProc(int code,WPARAM wParam,LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning()){
		SimpleTracker lock(lHookActive);
		if (bActive && code==HC_ACTION && g_hTargetWnd!=NULL){
			if(wParam==WM_MOUSEMOVE || g_hhookM==0){
				MOUSEHOOKSTRUCT* pkbhs=(MOUSEHOOKSTRUCT*)lParam;
				pkbhs->wHitTestCode=g_LastNcHit;
				if(ProcessMouseMessage(wParam,lParam, FALSE)){
					return 1;
				}
			}
		}
	}
	return CallNextHookEx( g_hhookM2, code, wParam, lParam );   
}

LRESULT CALLBACK LowLevelMouseProc(int code,WPARAM wParam,LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning() && dwOkHOOKNumber==g_dwOkHOOKNumber){
		SimpleTracker lock(lHookActive);
		if (bActive && code==HC_ACTION && g_hTargetWnd!=NULL){
			if((wParam!=WM_MOUSEMOVE || g_hhookM2==0) && ProcessMouseMessage(wParam,lParam, TRUE)){
				return 1;
			}
		}
	}
	return CallNextHookEx( g_hhookM, code, wParam, lParam );   
}

BOOL WINAPI SetActivity(BOOL bSetActive)
{
	BOOL bOld=bActive;
	//bActive=bSetActive;
	return bOld;
}

BOOL WINAPI SetOptions(BOOL bSetOptions, DWORD& dwInOutFlags, DWORD dwTime, DWORD dwPnholdMaxTimeout)
{
	if(bSetOptions){
		dwFlags=dwInOutFlags;
	}else{
		dwInOutFlags=dwFlags;
	}
	dwDblClcMaxTime=dwTime;
	dwPressnholdMaxTimeout=dwPnholdMaxTimeout*1000;
	if(dwPressnholdMaxTimeout>100000){
		dwPressnholdMaxTimeout=100000;
	}
	return TRUE;
}

BOOL WINAPI SetPromiscMode(HWND hWnd,RECT* rtPromisc)
{
	hPromiscModeWnd=hWnd;
	memset(&rcPromiscModeRect,0,sizeof(rcPromiscModeRect));
	if(rtPromisc){
		rcPromiscModeRect.left=rtPromisc->left;
		rcPromiscModeRect.right=rtPromisc->right;
		rcPromiscModeRect.top=rtPromisc->top;
		rcPromiscModeRect.bottom=rtPromisc->bottom;
	}
	return TRUE;
}

void WaitForHookNULL(HHOOK& h)
{
	return;
	int c=100;
	while(h!=0 && c>0){
		c--;
		Sleep(100);
	}
}

BOOL WINAPI InstallHook(BOOL bSet, BOOL bSetMouse, HWND hTargetWnd, BOOL bPaused)
{
	HANDLE hMutex=CreateMutex(NULL,TRUE,BUF_MUTEX);
	if(bPaused){
		dwFlags=0;
	}
	BOOL fOk=FALSE;
	if (bSet) {
		g_hTargetWnd = hTargetWnd;
		g_hhook = NULL;
		ResetKBBuffer(0);
		WaitForHookNULL(g_hhook);
		if(!isWin9x()){
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hDLL, 0);
		}
		if(g_hhook == NULL){
			char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
			g_hhook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hDLL, 0);
		}
		if(bSetMouse){
			WaitForHookNULL(g_hhookM);
			if(!isWin9x()){
				char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
				g_hhookM = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hDLL, 0);
			}
			if(g_hhookM == NULL && g_hhookM2 == NULL){
				WaitForHookNULL(g_hhookM2);
				char szDLLPath[MAX_PATH]={0};GetModuleFileName(g_hinstDll,szDLLPath,sizeof(szDLLPath));HINSTANCE hDLL=LoadLibrary(szDLLPath);
				g_hhookM2 = SetWindowsHookEx(WH_MOUSE, MouseProc, hDLL, 0);
			}
		}
		fOk = (g_hhook != NULL);
		InterlockedIncrement(&g_dwOkHOOKNumber);
		dwOkHOOKNumber=g_dwOkHOOKNumber;
		bActive=1;
	}else{
		bActive=0;
		g_hTargetWnd= NULL;
		BOOL bNoNeedToWait=FALSE;
		if(g_hhookM!=NULL){
			SmartUnhookWindowsHookEx(g_hhookM,&bNoNeedToWait);
			g_hhookM=NULL;
		}
		if(g_hhookM2!=NULL){
			SmartUnhookWindowsHookEx(g_hhookM2,&bNoNeedToWait);
			g_hhookM2=NULL;
		}
		if(g_hhook!=NULL){
			fOk=SmartUnhookWindowsHookEx(g_hhook,&bNoNeedToWait);
			g_hhook=NULL;
		}
		SmartUnhookWindowsHookEx(NULL,&bNoNeedToWait);
		if(!bNoNeedToWait)
		{
			long lCount=0;
			while(lHookActive>0){
				lCount++;
				Sleep(20);
				if(lCount>200){
					break;
				}
			}
		}
	}
	CloseHandle(hMutex);
	ResetKBBuffer(0);
	return fOk;
}