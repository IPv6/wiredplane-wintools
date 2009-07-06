// WP_Enabler.cpp : Defines the entry point for the DLL application.
//
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include "../WKPlugin.h"
#pragma data_seg("Shared")

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

//================================
//переменная обозначающая, 
//что используется
//новый метод включения окон
//меньше ест процессор, но для для 
//включения нужно провести мышь по
//самому окну или его родителю
//вдобавок еще нет возможности 
//делать окна немодальными
BOOL new_mode = FALSE;
//BOOL startByDef = TRUE;

//лучше будет включать плагин 
//по умолчанию - если пользователь
//загрузит его и не догадается включить?
//да при каждой загрузке включать замаешься
BOOL stopped = FALSE;
BOOL paused = FALSE;
POINT P; // Точка "под мышкой"
HINSTANCE g_hinstDll = 0;
HHOOK g_hhook1 = NULL;
long g_ActiveCounter = 0;

#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")

//*все функции объявлены __inline, но это ничего не значит
void __inline  GetChildren(HWND hwndP)
{
    //*фактически эта функция повторяет конец
    //*основной функции
    //*это сделано для того чтобы ее рекурсивно вызывать
    //*переменные
    HWND h; //
    HWND hT;
    //*Подчиненные есть?  
    h = GetWindow(hwndP, GW_CHILD);
    //*если нет, то валим
    if (h == NULL) return;
    //*ну а уж если есть, то 
    //*передираем их все, пока не кончатся
    while (h != NULL)//то есть H !=
    {
        //*само окно
        EnableWindow(h, TRUE);
        //*отлавливаем детей
        hT = GetWindow(h, GW_CHILD);
        //*если ни кого-то не поймали, то
        if(hT == NULL)
        {
            //*получаем братское окно
            h = GetWindow(h, GW_HWNDNEXT);
            //*и врубаем его
            EnableWindow(h, TRUE);
            //*пропускаем код врубания детей, так как
            //*их нет
            continue;
        }
        //*если же поймали, то теперь
        //*повторяем все, но уже с детьми текущего окна
        GetChildren(h);      
        //*напоследок ловим соседа 
        h = GetWindow(h, GW_HWNDNEXT);
        //*и врубаем его
        EnableWindow(h, TRUE);
    }
}

HWND __inline FindOldestWindow(HWND hwnd)
{
    //*переменные 
    HWND hTemp;//*временное окно
    //*ищем предка
    hTemp = GetParent(hwnd);
    //*если мы самые старые, то сами за все и ответим
    if (hTemp == NULL) return hwnd;
    //*а если нет, то пусть отцы отдуваются
    //*на тот случай если у у нас еще и деды есть
    //*мы их тоже спрашиваем, про предков
    hTemp = FindOldestWindow(hTemp);
    //*ну и в конце возвращаем самого старого
    return hTemp; 
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

//*основная функция - здесь кипит вся работа
// iType==0 просто окно подкурсором
// iType==1 ородитель окна под курсором/модальность/немодальность
void __inline  OnTimer(int iType,HWND hDefault=0)
{
    HWND ParentWin; //*родительское окно
    HWND CurWin=hDefault; //*текущее окно (под курсором)
	if(!CurWin){
		//Получим текущую позицию курсора
		GetCursorPos(&P);
		//Получим handle текущего окна (под курсором)
		CurWin = WindowFromPoint(P);
	}
	EnableWindow(CurWin, TRUE);
    //*если ничего под нами нет, то пока уходим,
    //*но я тут подумал и понял, что такой ситуации
    //*быть не может, так как всегда есть окно десктопа
    if (CurWin == GetDesktopWindow()) return; //выходим, если не над окном
	if(iType){
		// Это для надежности. пред. алгоритм не всегда подхватывал верхние окна
		HWND hWin2=CurWin;
		GetWindowTopParent(hWin2);
		OnTimer(0,hWin2);
		if(hWin2!=CurWin){
			::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)CurWin);
		}
	}
    //Видимо рекурсия от общего предка не проходит
    //все окна, поэтому вызываем ее и от текущего окна
    GetChildren(CurWin);
    // Получим handle родителя
    ParentWin = FindOldestWindow(CurWin);
    //Получим 1-го потомка. Самого верхнего
    ParentWin = GetWindow(ParentWin, GW_CHILD); 
    ParentWin = GetWindow(ParentWin, GW_HWNDFIRST);
    //Пройдемся по всем дочерним элементам родительского окна
    while (ParentWin != NULL)
    {
        // Дочернее окно может само оказаться родичем 
        // Рекурсивная ф-ция позволяет дойти до самого       
        // младшего в семейке
        GetChildren(ParentWin);  
        //*включаем текущее окно
        EnableWindow(ParentWin, TRUE);
        //*получаем следущее окно
        ParentWin = GetWindow(ParentWin, GW_HWNDNEXT);
    }
}

//----- Hook code -------
LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(WKUtils::isWKUpAndRunning()){
		::InterlockedIncrement(&g_ActiveCounter);
		if (nCode >= 0 && lParam != 0 && !stopped && !paused)
		{
			CWPRETSTRUCT* msg = ((CWPRETSTRUCT*)lParam);
			//CWPSTRUCT* msg=((CWPSTRUCT*)lParam);
			if(msg->message == WM_INITMENUPOPUP || msg->message == WM_INITMENU)
			{
				HMENU h = (HMENU)msg->wParam;
				if(h && ::IsMenu(h))
				{
					DWORD dwCount = ::GetMenuItemCount(h);
					for(DWORD iPos = 0; iPos < dwCount; ++iPos)
					{
						::EnableMenuItem(h, iPos, MF_BYPOSITION | MF_ENABLED);
					}
				}
			}
			if(msg->message == WM_MENUSELECT)
			{
				HMENU h = (HMENU)msg->lParam;
				int iPos = LOWORD(msg->wParam);
				if (h && ::IsMenu(h))
				{
					::EnableMenuItem(h, iPos, MF_BYPOSITION | MF_ENABLED);
				}
			}
		}
		::InterlockedDecrement(&g_ActiveCounter);
	}
    return (CallNextHookEx(g_hhook1, nCode, wParam, lParam));
}

/*
BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            g_hinstDll=hModule;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
*/

//--------------------------------------------
// Functions definitions. 
// All required functions MUST be exported (throught .DEF file)
//--------------------------------------------
/*
    Function must fill szTitle with Plugin name
    and some other options. Result is ignored
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINCOMDSCFUNCTION)(WKPluginDsc*);
int    WINAPI GetPluginDsc(WKPluginDsc* stuff)
{
    stuff->bResidentPlugin = TRUE;
    stuff->bLoadPluginByDefault = TRUE;
    strcpy(stuff->szTitle, "Windows, buttons, menus enabler");
    strcpy(stuff->szDesk, "This plugin will help you to enable disabled items of user interface");
	g_hinstDll=WKGetPluginContainer()->GetPluginDllInstance();
    stuff->hPluginBmp = ::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BITMAP));
    if(stuff->cbsize >= sizeof(WKPluginDsc)){
        strcpy(stuff->szAuthors, "Bulavin Alex & Mushinsky Sergey");
    }
    return 1;
}

/*
    This function will be called when plugin is loaded
    MUST NOT invlolve any operations with long duration
    (use WKPluginInit for such initializations)
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int    (WINAPI *FPLUGINSTARTFUNCTION)();
int    WINAPI WKPluginStart()
{
    return 1;
}

/*
    This function will be called when for resident plugin
    when it is loaded, but in secondary thread. 
    (WKCallbackInterface*) pointer can be saved for later use
    This method may involve long-duration operations
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
HANDLE hThread = 0;
HANDLE hThreadStopped = 0;

char szHookLibPath[MAX_PATH] = "";
typedef WINUSERAPI long  (WINAPI  *BroadcastSystemMessageA)(DWORD, LPDWORD, UINT, WPARAM, LPARAM);
DWORD WINAPI MainThread(LPVOID)
{
    //Загружаем вторую копию этой библиотеки так как,
    //Windows выгрузит эту копию при вызове UnhookWindowsHookEx!!!!
    HINSTANCE hCopy = LoadLibrary(szHookLibPath);
    hThreadStopped = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    g_hhook1 = SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndRetProc, hCopy, 0);
    if(g_hhook1 == NULL)
    {
        char szErr[100] = {0};
        sprintf(szErr, "Failed to start menus enabler, error 0x%08X",GetLastError());
        MessageBox(0, szErr, "Enabler Message", 0);
    }
    SetThreadPriority(GetCurrentThread, THREAD_PRIORITY_IDLE);
    int wnd = 0;
	DWORD dwCommonSleepTime=0;
    while(!stopped)
    {
		if(!paused){
			dwCommonSleepTime=500;
			if (new_mode == TRUE){
				OnTimer(0);
			}else{
				for (wnd = 0; wnd < 10000; ++wnd){
					EnableWindow((HWND)wnd, TRUE);
					if (stopped){
						break;
					}
				};        
			};
		}else{
			dwCommonSleepTime=2000;
		}
		// Если все запаузено, то будет раз в две секунды
		// иначе раз в 500 миллисекунд
		Sleep(dwCommonSleepTime);
    };
    DWORD dwRes = 0;
    SetThreadPriority(GetCurrentThread, THREAD_PRIORITY_NORMAL);
    UnhookWindowsHookEx(g_hhook1);
    //::SendMessageTimeout(HWND_BROADCAST, WM_NULL, 0, 0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 100, &dwRes);
    DWORD dwTick=GetTickCount();
    while(g_ActiveCounter > 0 && GetTickCount() - dwTick < 5000)
    {
        Sleep(100);
    }
    g_hhook1 = 0;
    SetEvent(hThreadStopped);
    return 0;
}

typedef int    (WINAPI *FPLUGININITFUNCTION)(WKCallbackInterface*);
int    WINAPI WKPluginInit(WKCallbackInterface* i)
{
	if(i){
		// Version...
		char szVer[32]="";
		i->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			i->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
    if (i)
    {
        szHookLibPath[0] = 0;
        i->GetWKPluginsDirectory(szHookLibPath, MAX_PATH);
        strcat(szHookLibPath, "wp_enabler.wkp");
    }
    DWORD dwID = 0;
    stopped = FALSE;
    hThread = ::CreateThread(0, 0, MainThread, 0, 0, &dwID);
    return 1;
}


/*
    This function will be called when plugin is unloaded
    MUST NOT invlolve any oprations with long duration
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int    (WINAPI *FPLUGINSTOPFUNCTION)();
int    WINAPI WKPluginStop()
{
    stopped = TRUE;
    if(hThread)
    {
        if(hThreadStopped)
        {
            WaitForSingleObject(hThreadStopped,1000);
            CloseHandle(hThreadStopped);
        }
        CloseHandle(hThread);
        hThread=0;
    }
    return 1;
}

/*
    This function will be called when user wants to change Plugin preferences
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int    (WINAPI *FPLUGINOPTIONSFUNCTION)(HWND);
int    WINAPI WKPluginOptions(HWND hParent)
{
    return 1;
}


//    WireKeys detect plugins by presence of this function
//    At start up WireKeys will call this function 
//    with iPluginFunction from 0 to '...' while WKGetPluginFunctionDescription(..)!=0
//    Plugin must fill WKPluginFunctionDsc for every supported function
//    Result must be:
//    -------------------------------------------------
//    0    Function with this iPluginFunction is not supported by plugin
//    1    Function with this iPluginFunction supported by plugin
//    -------------------------------------------------
//    This is required function
int    WINAPI WKGetPluginFunctionCommonDesc(DWORD iPluginFunction, WKPluginFunctionDsc* stuff)
{
    if (!stuff) return 0;
    if(iPluginFunction==0)
    {
		strcpy(stuff->szItemName, "Start/stop Enabler");
		stuff->dwItemMenuPosition = WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->hItemBmp = ::LoadBitmap(g_hinstDll, MAKEINTRESOURCE(IDB_BITMAP));
		stuff->dwDefaultHotkey = 0;
		return 1;
    };
    if(iPluginFunction==1)
    {
		strcpy(stuff->szItemName, "Enable window under cursor");
		stuff->dwItemMenuPosition = 0;
		stuff->hItemBmp = ::LoadBitmap(g_hinstDll, MAKEINTRESOURCE(IDB_BITMAP));
		stuff->dwDefaultHotkey = 0;
		return 1;
    };
	return 0;
}
/*
    This function will be called for every plugin menu item,
    So Plugin can temporaly disable/gray out/check/uncheck
    his own menu items.
    Result must be 1.
    -------------------------------------------------
    This is not required function
*/
int    WINAPI WKGetPluginFunctionActualDesc(DWORD iPluginFunction, WKPluginFunctionActualDsc* stuff);

/*
    This function will be called from WireKeys 
    when user will choose appropriate menuitem/hotkey
    -------------------------------------------------
    Result must be:
    0    Error occured, If szError[0]!=NULL, alert will be shown to the user
    >0    Everything OK
    <0    Reserved
    -------------------------------------------------
    This is required function
*/
int    WINAPI WKCallPluginFunction(DWORD iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(iPluginFunction==1){
		OnTimer(1);
	}else{
		if (stopped)
		{
			paused=0;
			WKPluginInit(0);
			MessageBox(0, "Enabler started", "Enabler Message", 0);
		}
		else
		{
			WKPluginStop();
			MessageBox(0, "Enabler stopped", "Enabler Message", 0);
		}
	}
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddBoolOption("logOn","Mouse-driven method of enabling interface items","If checked, plugin will enable elements under mouse cursor only. This method save CPU resources",TRUE,0);
		//pOptionsCallback->AddBoolOption("active","Pause Enabler at start by default","",FALSE,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		/*
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
			WaitForSingleObject(hHookerEventDone,1000);
		}
		*/
		new_mode=pOptionsCallback->GetBoolOption("logOn");
		//startByDef=pOptionsCallback->GetBoolOption("active");
		//paused=startByDef;
		/*
		if(iAction==OM_OPTIONS_SET){
			PostThreadMessage(dwHookerThread,WM_ENABLE,0,0);
			WaitForSingleObject(hHookerEventDone,1000);
		}
		*/
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("logOn",new_mode);
		//pOptionsCallback->SetBoolOption("active",startByDef);
	}
	return 1;
}
