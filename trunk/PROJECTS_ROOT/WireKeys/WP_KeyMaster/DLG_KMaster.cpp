// DLG_KMaster.cpp : implementation file
//

#include "stdafx.h"
#include "WP_KeyMaster.h"
#include "DLG_KMaster.h"
#include "..\..\SmartWires\SystemUtils\SystemInfo.h"
#ifdef _NORAWINPUTDEFS
#include "WINUSER.h"
#endif
#include "parsUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern BOOL g_bDialogActive;
extern _ResetKBBuffer hReset;
WKCallbackInterface*& WKGetPluginContainer();
CString _l(const char* szText);
extern CHidMaster hidmanager;
extern CWKKeyMasterApp theApp;
extern HWND g_hCathcerWnd;
typedef BOOL (WINAPI *_RegisterRawInputDevices)
(      
    PCRAWINPUTDEVICE pRawInputDevices,
    UINT uiNumDevices,
    UINT cbSize
);

typedef UINT (WINAPI *_GetRawInputData)
(      
    HRAWINPUT hRawInput,
    UINT uiCommand,
    LPVOID pData,
    PUINT pcbSize,
    UINT cbSizeHeader
);

typedef LRESULT (WINAPI *_DefRawInputProc)
(      
    PRAWINPUT *paRawInput,
    INT nInput,
    UINT cbSizeHeader
);

typedef UINT (WINAPI *_GetRawInputDeviceInfo)
(
    HANDLE hDevice,
    UINT uiCommand,
    LPVOID pData,
    PUINT pcbSize
);

_RegisterRawInputDevices RegisterRawInputDevicesMy=0;
_GetRawInputData GetRawInputDataMy=0;
_DefRawInputProc DefRawInputProcMy=0;
_GetRawInputDeviceInfo GetRawInputDeviceInfoMy=0;
/*
the WDK has examples of how to use the hid.dll apis.   you need to use these
apis to parse the data and get it back into a form in which you can use..

 HidP_GetButtons()
 
*/
/////////////////////////////////////////////////////////////////////////////
// CDLG_KMaster dialog
CDLG_KMaster::CDLG_KMaster(CWnd* pParent /*=NULL*/)
: CDialog()//CDialog(CDLG_KMaster::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_KMaster)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	bCLIsON=0;
	bSLIsON=0;
	bNLIsON=0;
}

void CDLG_KMaster::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_KMaster)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_KMaster, CDialog)
//{{AFX_MSG_MAP(CDLG_KMaster)
ON_WM_CLOSE()
ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_KMaster message handlers
extern HWND g_hKeyboardTrapWindow;
void CDLG_KMaster::PostNcDestroy() 
{
	WKIHOTKEYS_EventsHandlerWindow()=0;
	g_hKeyboardTrapWindow=NULL;
	CDialog::PostNcDestroy();
	delete this;
}

CDLG_KMaster* pKM=0;
HANDLE hLocksKeys=0;
DWORD WINAPI CheckCL(LPVOID pd)
{
	pKM=(CDLG_KMaster*)pd;
	hLocksKeys=CreateEvent(0,0,0,0);
	while(pKM){
		pKM->SetLockIconC();
		pKM->SetLockIconS();
		pKM->SetLockIconN();
		WaitForSingleObject(hLocksKeys,INFINITE);
		Sleep(10);
	}
	CloseHandle(hLocksKeys);
	return 0;
}

#define IDC_ARROW           MAKEINTRESOURCE(32512)
#define IDC_IBEAM           MAKEINTRESOURCE(32513)
#define IDC_WAIT            MAKEINTRESOURCE(32514)
#define IDC_CROSS           MAKEINTRESOURCE(32515)
#define IDC_UPARROW         MAKEINTRESOURCE(32516)
#define IDC_SIZE            MAKEINTRESOURCE(32640) /* OBSOLETE: use IDC_SIZEALL */
#define IDC_ICON            MAKEINTRESOURCE(32641) /* OBSOLETE: use IDC_ARROW */
#define IDC_SIZENWSE        MAKEINTRESOURCE(32642)
#define IDC_SIZENESW        MAKEINTRESOURCE(32643)
#define IDC_SIZEWE          MAKEINTRESOURCE(32644)
#define IDC_SIZENS          MAKEINTRESOURCE(32645)
#define IDC_SIZEALL         MAKEINTRESOURCE(32646)
#define IDC_NO              MAKEINTRESOURCE(32648) /* not in win3.1 */
#define CURSORS_RANGE		32648-32512+1
#define DEAD_ZONE_MIN		32516-32512
#define DEAD_ZONE_MAX		32640-32512
HCURSOR g_hSavedCursors[CURSORS_RANGE]={0};
void ShowHideCursor(long lParam)
{
	static long bCursorHidden=0;
	static CCriticalSection cs;
	SimpleLocker lShowHide(&cs);
	if(lParam==1 && bCursorHidden){
		::InterlockedDecrement(&bCursorHidden);
		for(int i=0;i<CURSORS_RANGE;i++){
			if(i<=DEAD_ZONE_MIN || i>=DEAD_ZONE_MAX){
				SetSystemCursor(g_hSavedCursors[i], 32512+i);
				g_hSavedCursors[i]=0;
			}
		}
		/*
		::SetCursor(g_hSavedCursor);
		ShowCursor(TRUE);
		ShowCursor(TRUE);
		int iCount=ShowCursor(TRUE);
		char s[50]={0};
		sprintf(s,"Hide: %i",iCount);
		::SetWindowText(::GetForegroundWindow(),s);
		//FLOG3("ShowCursor false File=%s, line=%lu, count=%i",__FILE__,__LINE__,iCount);
		MessageBeep(-1);
		*/
	}
	if(lParam==0 && bCursorHidden==0){
		::InterlockedIncrement(&bCursorHidden);
		HCURSOR hEmpty=LoadCursor(theApp.m_hInstance,MAKEINTRESOURCE(IDC_CURSOR_EMPTY));
		for(int i=0;i<CURSORS_RANGE;i++){
			if(i<=DEAD_ZONE_MIN || i>=DEAD_ZONE_MAX){
				g_hSavedCursors[i]=CopyCursor(LoadCursor(NULL,MAKEINTRESOURCE(32512+i)));
				SetSystemCursor(CopyCursor(hEmpty), 32512+i);
			}
		}
		/*
		g_hSavedCursor=::SetCursor(0);
		ShowCursor(FALSE);
		ShowCursor(FALSE);
		int iCount=ShowCursor(FALSE);
		char s[50]={0};
		sprintf(s,"Hide: %i",iCount);
		::SetWindowText(::GetForegroundWindow(),s);
		//FLOG3("ShowCursor false File=%s, line=%lu, count=%i",__FILE__,__LINE__,iCount);
		MessageBeep(-1);
		*/
	}
}


CDLG_KMaster::~CDLG_KMaster()
{
	pKM=0;
	hidmanager.Unregister();
	ShowHideCursor(1);
	SetEvent(hLocksKeys);
	// Just nothing todo
	Sleep(10);
}

BOOL CDLG_KMaster::OnInitDialog() 
{
	FORK(CheckCL,this);
	CDialog::OnInitDialog();
	// Тень
	SetClassLong(this->GetSafeHwnd(),GCL_STYLE,GetClassLong(this->GetSafeHwnd(),GCL_STYLE)|0x00020000);
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	ModifyStyleEx(0,WS_EX_TOOLWINDOW);
	//-----------
	Sizer.InitSizer(this,10,HKEY_CURRENT_USER,"SOFTWARE\\WiredPlane\\WireKeys\\WKKeyMaster\\WKWnd_pos");
	Sizer.SetOptions(STICKABLE|SIZEABLE);
	Sizer.AddDialogElement(IDC_FRAME,0);
	Sizer.AddDialogElement(IDC_DESCRIPTION,0);
	Sizer.AddDialogElement(IDC_BT,heightSize);
	Sizer.AddLeftTopAlign(IDC_FRAME,0,0);
	Sizer.AddRightAlign(IDC_BT,0,0);
	Sizer.AddLeftAlign(IDC_BT,0,0);
	Sizer.AddBottomAlign(IDC_BT,0,0);
	Sizer.AddRightAlign(IDC_FRAME,0,0);
	Sizer.AddBottomAlign(IDC_FRAME,IDC_BT,topPos,-2);
	Sizer.AddLeftAlign(IDC_DESCRIPTION,IDC_FRAME,5);
	Sizer.AddTopAlign(IDC_DESCRIPTION,IDC_FRAME,20);
	Sizer.AddRightBottomAlign(IDC_DESCRIPTION,IDC_FRAME,-5);
	SetWindowText(_l("Hotkey guide"));
	GetDlgItem(IDC_FRAME)->SetWindowText(_l("Possible hotkeys"));
	//GetDlgItem(IDC_BT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BT)->SetFocus();
	g_hKeyboardTrapWindow=GetSafeHwnd();
	hidmanager.Register(GetSafeHwnd());
	WKIHOTKEYS_EventsHandlerWindow()=GetSafeHwnd();
	return FALSE;
}

BOOL CHidMaster::Unregister()
{
	POSITION pos=mEventsListing.GetStartPosition();
	while(pos){
		DWORD dwKey=0;
		CDeviceEvents* eventd=0;
		mEventsListing.GetNextAssoc(pos,dwKey,eventd);
		delete eventd;
	};
	mEventsListing.RemoveAll();
	return TRUE;
}

extern GPLUGINIni g_hookData;
BOOL CHidMaster::Register(HWND toWindow)
{
	bHidChanged=0;
	if(!g_hookData.enableHID){
		return FALSE;
	}
	if(isWinXP() || IsVista()){
		// Грузим ХИД-коды
		char szPath[MAX_PATH]={0};
		WKGetPluginContainer()->GetPreferencesFolder(szPath,sizeof(szPath));
		sHidIni=szPath;
		sHidIni+="hidkeys.ini";
		//http://msdn2.microsoft.com/en-us/library/ms645546.aspx
		HINSTANCE hUserDLL=GetModuleHandle("User32.dll");
		if(hUserDLL){
			RegisterRawInputDevicesMy=(_RegisterRawInputDevices)GetProcAddress(hUserDLL,"RegisterRawInputDevices");
			GetRawInputDataMy=(_GetRawInputData)GetProcAddress(hUserDLL,"GetRawInputData");
			DefRawInputProcMy=(_DefRawInputProc)GetProcAddress(hUserDLL,"DefRawInputProc");
			GetRawInputDeviceInfoMy=(_GetRawInputDeviceInfo)GetProcAddress(hUserDLL,"GetRawInputDeviceInfoA");
		}
		if(RegisterRawInputDevicesMy){
			WKGetPluginContainer()->GetWKPluginsDirectory(szPath,sizeof(szPath));
			CString sHidDB=szPath;
			sHidDB+="hidio_devices.txt";
			CString sRawInputDB=ReadFile(sHidDB);
			//sRawInputDB.Replace("\t","");
			RAWINPUTDEVICE Rid[100]={0};
			int iRawType=0;
			// Parsing Hid db description
			while(true){
				CString sBeginD,sEndD;
				sBeginD.Format("DEVICE%i.begin",iRawType+1);
				sEndD.Format("DEVICE%i.end",iRawType+1);
				CString sDevice=CStringSubstr::getTag(sBeginD,sEndD,sRawInputDB);
				if(sDevice==""){
					break;
				}
				Rid[iRawType].usUsagePage = (BYTE)atolx(CStringSubstr::getTag("usUsagePage=","\n",sDevice));
				Rid[iRawType].usUsage = (BYTE)atolx(CStringSubstr::getTag("usUsage=","\n",sDevice));
				Rid[iRawType].dwFlags = RIDEV_INPUTSINK;
				Rid[iRawType].hwndTarget = toWindow;
				CString sClassName=CStringSubstr::getTag("DeviceName=","\n",sDevice);
				sClassName.TrimRight();
				CDeviceEvents* events= new CDeviceEvents();
				events->iEventsCount=0;
				while(true){
					CString sBeginE,sEndE;
					sBeginE.Format("EVENT%i.begin",events->iEventsCount+1);
					sEndE.Format("EVENT%i.end",events->iEventsCount+1);
					CString sEvent=CStringSubstr::getTag(sBeginE,sEndE,sDevice);
					if(sEvent==""){
						break;
					}
					CString sEventID=CStringSubstr::getTag("UniqueEventFourCC=","\n",sEvent);
					sEventID.TrimRight();
					strcpy(events->reestr[events->iEventsCount].sEventID,sEventID);
					CString sName=CStringSubstr::getTag("Name=","\n",sEvent);
					sName.TrimRight();
					CString sAnd=CStringSubstr::getTag("AND=","\n",sEvent);
					sAnd.TrimRight();
					CString sRes=CStringSubstr::getTag("RES=","\n",sEvent);
					sRes.TrimRight();
					events->reestr[events->iEventsCount].iMaskLen=(int)(sRes.GetLength()*0.5f);
					for(int kk=0;kk<events->reestr[events->iEventsCount].iMaskLen;kk++){
						char szBuff[10]="0xFF";
						szBuff[2]=sAnd[kk*2];
						szBuff[3]=sAnd[kk*2+1];
						events->reestr[events->iEventsCount].opAND[kk]=(BYTE)atolx(szBuff);
						szBuff[2]=sRes[kk*2];
						szBuff[3]=sRes[kk*2+1];
						events->reestr[events->iEventsCount].opRES[kk]=(BYTE)atolx(szBuff);
					}

					char szShortDescription[64]={0};
					strcpy(szShortDescription,sName);
					char szEventClassName[64]={0};
					strcpy(szEventClassName,sClassName);
					WKGetPluginContainer()->RegisterEvent(events->reestr[events->iEventsCount].sEventID,szShortDescription,szEventClassName);
					events->iEventsCount++;
				}
				mEventsListing[MAKEWORD(Rid[iRawType].usUsagePage,Rid[iRawType].usUsage)]=events;
				iRawType++;
			}
			if(iRawType==0){
				return FALSE;
			}
			int iHIDRes=(*RegisterRawInputDevicesMy)(Rid, iRawType, sizeof(Rid[0]));
			DWORD dwErr=GetLastError();
			if (iHIDRes == FALSE) {
				//registration failed. Call GetLastError for the cause of the error.
#ifdef _DEBUG
				CString err;
				err.Format("Failed to register HID input. %08X",GetLastError());
				AfxMessageBox(err);
#endif
				return FALSE;
			}
		}
	}
	return TRUE;
}


BOOL CHidMaster::HandleHidInput(MSG* pMsg)
{
	BOOL bHandled=0;
	if(GetRawInputDataMy){
		UINT dwSize=0;
		// Detecting size...
		(*GetRawInputDataMy)((HRAWINPUT)pMsg->lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb != NULL) 
		{
			if ((*GetRawInputDataMy)((HRAWINPUT)pMsg->lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize )
			{
				RAWINPUT* raw = (RAWINPUT*)lpb;
				RID_DEVICE_INFO di;
				UINT dwDISize=sizeof(di);
				memset(&di,0,dwDISize);
				di.cbSize=dwDISize;
				GetRawInputDeviceInfoMy(raw->header.hDevice,RIDI_DEVICEINFO,&di,&dwDISize);
				if (raw->header.dwType == RIM_TYPEKEYBOARD) 
				{
					if(g_hCathcerWnd){
						char szTempOutput[1024]={0};
						sprintf(szTempOutput, ("HID Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x"), 
							raw->data.keyboard.MakeCode, 
							raw->data.keyboard.Flags, 
							raw->data.keyboard.Reserved, 
							raw->data.keyboard.ExtraInformation, 
							raw->data.keyboard.Message, 
							raw->data.keyboard.VKey);
						::PostMessage(g_hCathcerWnd,WM_COMMAND,IDC_SHDROCA,(LPARAM)strdup(szTempOutput));
						bHandled=1;
					}
				}
				else if (raw->header.dwType == RIM_TYPEMOUSE) 
				{
					if(g_hCathcerWnd){
						char szTempOutput[1024]={0};
						sprintf(szTempOutput, ("HID Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x"), 
							raw->data.mouse.usFlags, 
							raw->data.mouse.ulButtons, 
							raw->data.mouse.usButtonFlags, 
							raw->data.mouse.usButtonData, 
							raw->data.mouse.ulRawButtons, 
							raw->data.mouse.lLastX, 
							raw->data.mouse.lLastY, 
							raw->data.mouse.ulExtraInformation);
						::PostMessage(g_hCathcerWnd,WM_COMMAND,IDC_SHDROCA,(LPARAM)strdup(szTempOutput));
						bHandled=1;
					}
				} else if (raw->header.dwType == RIM_TYPEHID)
				{
					bHandled=1;
					CDeviceEvents* eventDsc=mEventsListing[MAKEWORD(di.hid.usUsagePage,di.hid.usUsage)];
					for(UINT i=0;i<raw->data.hid.dwCount;i++){
						BYTE* hidInfo=(BYTE*)(raw->data.hid.bRawData+i*raw->data.hid.dwSizeHid);
						const char* szEvent=NULL;
						// Checking Events
						if(eventDsc){
							for(int kl=0;kl< eventDsc->iEventsCount; kl++){
								BYTE originalCopy[MAXEVENTBUFFER]={0};
								int iLen=min(eventDsc->reestr[kl].iMaskLen,(int)raw->data.hid.dwSizeHid);
								memcpy(originalCopy,hidInfo,iLen);
								BOOL bThisIsIt=TRUE;
								for(int kp=0;kp<iLen;kp++){
									BYTE b=originalCopy[kp] & (eventDsc->reestr[kl].opAND[kp]);
									if(b!=eventDsc->reestr[kl].opRES[kp]){
										bThisIsIt=FALSE;
										break;
									}
								}
								if(bThisIsIt){
									szEvent=eventDsc->reestr[kl].sEventID;
								}
							}
						}
						if(g_hCathcerWnd){
							char szTempOutput[1024]={0};
							if(raw->data.hid.dwSizeHid<MAXEVENTBUFFER){
								char szHidRaw[1024]={0};
								char szHidByte[1024]={0};
								for(UINT ikp=0;ikp<raw->data.hid.dwSizeHid;ikp++){
									sprintf(szHidByte,"%02X",(BYTE)(hidInfo[ikp]));
									strcat(szHidRaw,szHidByte);
								}
								sprintf(szTempOutput, ("HID Event: %s. Data:'%s'[%i], p=%i,u=%i"), szEvent==0?"0000":szEvent, szHidRaw, raw->data.hid.dwSizeHid,di.hid.usUsagePage,di.hid.usUsage);
							}else{
								char szMD5[128]={0};
								WKGetPluginContainer()->getMD5(szMD5,hidInfo,raw->data.hid.dwSizeHid);
								sprintf(szTempOutput, ("HID Event: %s. Size=%i (md5=%s)"), szEvent==0?"0000":szEvent, raw->data.hid.dwSizeHid,szMD5);
							}
							::PostMessage(g_hCathcerWnd,WM_COMMAND,IDC_SHDROCA,(LPARAM)strdup(szTempOutput));
						}
						char szEvent5[5]={0};
						if(szEvent){
							strcpy(szEvent5,szEvent);
							mUnfinishedEvents.Add(szEvent);
							WKGetPluginContainer()->NotifyEventStarted(szEvent5);
						}else{
							// Finishing all unfinished events
							for(int kz=0;kz<mUnfinishedEvents.GetSize();kz++){
								strcpy(szEvent5,mUnfinishedEvents[kz]);
								WKGetPluginContainer()->NotifyEventFinished(szEvent5);
							}
							mUnfinishedEvents.RemoveAll();
						}
					}
					// Delegeting call to the system anyway
				}
				if(DefRawInputProcMy){
					DefRawInputProcMy(&raw,1,sizeof(RAWINPUTHEADER));
				}
			}
			delete[] lpb; 
		}
	}
	return bHandled;
}

#define NULL_TITLE	_l("Hotkey guide")
BOOL CDLG_KMaster::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_SETFOCUS){
		//GetDlgItem(IDC_DESCRIPTION)->HideCaret();
	}
	if(pMsg->message==WM_KILLFOCUS){
		//GetDlgItem(IDC_DESCRIPTION)->ShowCaret();
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDLG_KMaster::OnClose() 
{
	if(g_bDialogActive){
		ActivateGuide(FALSE);
	}
}

struct CIHotkeyAndID
{
	DWORD dwID;
	long lSimilarity;
	HWND hWin;
};

extern GPLUGINIni g_hookData;
BOOL CDLG_KMaster::IsHotkeyPressed(DWORD wChar,BOOL bInjected,CIHotkey& hkPressed, CRegisteredHotkeyData& hkTest, CStringArray* evMap, BOOL bAutoRepeat)
{
	if(hkTest.oHotkey.isEvent()){
		return 0;
	}
	BOOL bTheSameHK=0;
	if(bAutoRepeat && ((hkTest.oHotkey.Options() & ADDOPT_ON_WPRESSED)!=0)){
		// При авторепите эти клавиши не обрабатываются
		bTheSameHK=0;
	}else{
		if((hkTest.oHotkey.Options() & ADDOPT_SEQUENCE)!=0){
			BOOL bIsBigger=0;
			bIsBigger=hkTest.oHotkey.isBigger(hkPressed);
			if(bIsBigger){
#ifdef DEBUG
			CString s1=GetIHotkeyName(hkPressed);
			CString s2=GetIHotkeyName(hkTest.oHotkey);
			TRACE3("%s vs %s > %i\n",s1,s2,0);
			TRACE2("----------- %i/%i -------------\n",bAutoRepeat,bIsBigger);
#endif
				// Проверяем наличие
				int iPos=0;
				CIHotkey hkFull;
				hkFull.Options()|=ADDOPT_SEQUENCE;
				SimpleLocker cs(&csKeySequence);
				/*if(wChar==VK_FAKEKEY)
				while(!bTheSameEvent && iPos<aEventSequence.GetSize()){
				hkFull.AddOneKeySeq(aEventSequence[iPos++],1);
				hkFull.AddOneKeySeq(aEventSequence[iPos++],1);
				bTheSameEvent=(hkFull==hkTest.oHotkey);
				}
				else*/{
				while(!bTheSameHK && iPos<aKeySequence.GetSize()){
					hkFull.AddOneKeySeq(aKeySequence[iPos++],1);
					bTheSameHK=(hkFull==hkTest.oHotkey);
				}
				}
			}
		}else{
			bTheSameHK=hkTest.oHotkey.isEqual(hkPressed,1);
		}
	}
	if(bTheSameHK){
		// Сверим активные приложения
		if(hkTest.oHotkey.AppConditionsFail()){
			bTheSameHK=FALSE;
		}
	}
	return bTheSameHK;
}

void CDLG_KMaster::RemoveObsoleteUnpressKeys(CIHotkey& hkBoss)
{
	// Нужно удалить также все "Входящие в эту" гор.клавиши, добавленные ранее
	for(int j=aKeyWaitingForUnpress.GetSize()-1;j>=0;j--){
		if(hkBoss.isBigger(aKeyWaitingForUnpress[j].value.oHotkey)){
			aKeyWaitingForUnpress.RemoveAt(j);
		}
	}
}

#define CHR_HOTKEY_KEY	0x01
#define CHR_BLOCK_KEY	0x02
#define CHR_EVEKEY_KEY	0x04
DWORD CDLG_KMaster::CheckHotkeyConditions(CIHotkey& hk, BOOL bUnpressed, BOOL bInjected, WPARAM wCode, BOOL bAutoRepeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	static CCriticalSection csUnpresslock;
	BOOL bWasHotkeyTriggered=0;
	BOOL bWasEventHotkeyTriggered=0;
	// Получим карты для сверки
	//-------------------------
	CRegisteredHotkeys& hkMap=getRegisteredHotkeysInfo(1);
	//CStringArray& evMap=WKIHOTKEYS_GetExternEvents(1);
	//-------------------------
	if(bUnpressed){
		// Горячие клавиши проверяются только при нажатии!!!
		// А здесь проверяются клавиши на отжатие
		BOOL bUnpRes=FALSE;// При отжатии никогда ничего не блокируем
		if(!hk.Present() && aKeyWaitingForUnpress.GetSize()>0){
			SimpleLocker SL(&csUnpresslock);
			CArray<CIHotkey,CIHotkey> aBossHKs;
			for(int i=aKeyWaitingForUnpress.GetSize()-1;i>=0;i--){
				//if(!IsHotkeyPressed(bInjected,hk,aKeyWaitingForUnpress[i].value,evMap))
				//bUnpRes=TRUE;
				aBossHKs.Add(aKeyWaitingForUnpress[i].value.oHotkey);

				CRegisteredHotkeyData& hkTestOriginal=hkMap[aKeyWaitingForUnpress[i].key];
				hkTestOriginal.bOnUnpress=1;
				hkTestOriginal.bAutoRepeat=bAutoRepeat;
				hkTestOriginal.dwLastKeyThatActivatedHK=wCode;
				hkTestOriginal.oRealTriggeredHotkey=aKeyWaitingForUnpress[i].value.oHotkey;
				//FLOG3("WM_HOTKEY File=%s, char=%lu, unp=%i",GetIHotkeyName(hk),wCode,hkTestOriginal.bOnUnpress);
				if(hkTestOriginal.oHotkey.isEvent()){
					bWasEventHotkeyTriggered=1;
				}
				if((hkTestOriginal.oHotkey.Options() & ADDOPT_TRANSPARENT)==0){
					bUnpRes=TRUE;
				}
				::PostMessage(aKeyWaitingForUnpress[i].key.hWindow,WM_HOTKEY,aKeyWaitingForUnpress[i].key.iID,aKeyWaitingForUnpress[i].value.oHotkey.GetAsDWORD());
				bWasHotkeyTriggered=1;
				aKeyWaitingForUnpress.RemoveAt(i);
			}
			for(int j=0;j<aBossHKs.GetSize();j++){
				RemoveObsoleteUnpressKeys(aBossHKs[j]);
			}
		}
		// Разблокируем карты
		//-------------------------
		getRegisteredHotkeysInfo(-1);
		//WKIHOTKEYS_GetExternEvents(-1);
		//-------------------------
		return bUnpRes;
	}
	CIHotkey hkGuide=hk;// Отдельный хоткей для информации
	BOOL bRefresHKGuide=g_bDialogActive;
	if(bRefresHKGuide && ((wCode>=VK_RBT_DBL && wCode<=WHEEL_UP) || wCode==VK_LBUTTON || wCode==VK_MBUTTON || wCode==VK_RBUTTON)){
		CPoint pt;
		::GetCursorPos(&pt);
		CRect rt;
		::GetWindowRect(GetSafeHwnd(),&rt);
		if(rt.PtInRect(pt)){
			bRefresHKGuide=FALSE;
		}
	}
	if(bRefresHKGuide){
		if(!hk.Present()){
			SetWindowText(NULL_TITLE);
			CWnd* pWnd=GetDlgItem(IDC_DESCRIPTION);
			if(pWnd){
				pWnd->SetWindowText(GetListOfCurrentlyRegisteredHotkeys());
			}
		}else{
			if(hkGuide.ContainMouseClicks()){//&& !hkGuide.ContainKeyClicksToo()
				if(::GetForegroundWindow()==GetSafeHwnd()){
					hkGuide.SkipMouseClicks();
				}
			}
			if(!hkGuide.Present()){
				SetWindowText(NULL_TITLE);
			}else{
				SetWindowText(GetIHotkeyName(hkGuide));
			}
		}
		// Состояние переключаемых клавиш
		char const* szNum="    ";
		char const* szCaps="    ";
		char const* szScroll="    ";
		if(bCLIsON){
			szCaps="CAPS";
		}
		if(bSLIsON){
			szScroll="SCRL";
		}
		if(bNLIsON){
			szNum="NUML";
		}
		char szKBState[128]="";
		//memset(szKBState,' ',sizeof(szKBState)-1);
		//szKBState[sizeof(szKBState)]=0;
		sprintf(szKBState," %s\t%s\t%s",szNum,szCaps,szScroll);
		CWnd* pWnd=GetDlgItem(IDC_BT);
		if(pWnd){
			pWnd->SetWindowText(szKBState);
		}
	}
	BOOL bBlockResult=FALSE;
	CArray<CIHotkeyAndID,CIHotkeyAndID&> aSimilarKeys;
	//---------------------------
	if(hk.Present() /*|| evMap.GetSize()>0*/){//hkMap.GetCount()>0 && 
		POSITION pos=hkMap.GetStartPosition();
		CRegisteredHotkeyKey iInfo;
		int iCounter=0;
		while(pos){
			CRegisteredHotkeyData hkTest;
			hkMap.GetNextAssoc(pos,iInfo,hkTest);
			iCounter++;
			if(hkTest.oHotkey.Present()){// Проверяем совпадение
				if(IsHotkeyPressed(wCode,bInjected,hk,hkTest,0/*&evMap*/, bAutoRepeat)){
					BOOL bOnUnpressOnly=(((hkTest.oHotkey.Options() & ADDOPT_ON_UNPRESS)!=0) || ((hkTest.oHotkey.Options() & ADDOPT_ON_WPRESSED)!=0));
					if(bOnUnpressOnly){
						SimpleLocker SL(&csUnpresslock);
						CRegisteredHotkeyPair pair;
						pair.key=iInfo;
						pair.value=hkTest;
						aKeyWaitingForUnpress.Add(pair);
						if((hkTest.oHotkey.Options() & ADDOPT_ON_WPRESSED)!=0){
							bOnUnpressOnly=0;
						}
						if((hkTest.oHotkey.Options() & ADDOPT_ON_UNPRESS)!=0){
							if((hkTest.oHotkey.Options() & ADDOPT_TRANSPARENT)==0){
								bBlockResult=TRUE;
							}
						}
					}
					if(!bOnUnpressOnly){
						CRegisteredHotkeyData& hkTestOriginal=hkMap[iInfo];
						hkTestOriginal.bOnUnpress=0;
						hkTestOriginal.bAutoRepeat=bAutoRepeat;
						hkTestOriginal.dwLastKeyThatActivatedHK=wCode;
						hkTestOriginal.oRealTriggeredHotkey=hkTest.oHotkey;
						//FLOG3("WM_HOTKEY File=%s, char=%lu, unp=%i",GetIHotkeyName(hk),wCode,hkTestOriginal.bOnUnpress);
						if(hkTestOriginal.oHotkey.isEvent()){
							bWasEventHotkeyTriggered=1;
						}
						::PostMessage(iInfo.hWindow, WM_HOTKEY, iInfo.iID, hkTestOriginal.oHotkey.GetAsDWORD());
						bWasHotkeyTriggered=1;
						if((hkTest.oHotkey.Options() & ADDOPT_TRANSPARENT)==0 
							&& (hkTest.oHotkey.Options() & ADDOPT_ON_WPRESSED)==0){
							// Чтобы ничто зависящее при отжатии уже не сработало...
							RemoveObsoleteUnpressKeys(hk);
						}
						if(!bBlockResult){
							bBlockResult=TRUE;
							if(hkTest.oHotkey.Options() & ADDOPT_TRANSPARENT){
								bBlockResult=FALSE;
							}
							if(bRefresHKGuide){
								CWnd* pWnd=GetDlgItem(IDC_DESCRIPTION);
								if(pWnd){
									pWnd->SetWindowText(GetListOfCurrentlyRegisteredHotkeys());
								}
							}
						}
					}
					// Теперь у нас немонопольная горячая политика
					// Одна и таже клавиша может использоваться много раз
					// разными модулями
					// return TRUE;
				}
				if(bRefresHKGuide){
					CIHotkeyAndID iDat;
					iDat.dwID=iInfo.iID;
					iDat.lSimilarity=hkGuide.GetHKSimilarity(hkTest.oHotkey,TRUE,FALSE,TRUE);
					iDat.hWin=iInfo.hWindow;
					if(iDat.lSimilarity>0){
						iDat.lSimilarity=iDat.lSimilarity*1000+(999-GetIHotkeyName(hkTest.oHotkey).GetLength());
						aSimilarKeys.Add(iDat);
					}
				}
			}
		}
	}
	//---------------------------
	//WKIHOTKEYS_GetExternEvents(-1);
	getRegisteredHotkeysInfo(-1);
	//---------------------------
	if(bRefresHKGuide){
		int i=0;
		CString sMaybeKeys;
		if(aSimilarKeys.GetSize()>0){
			for(i=0;i<aSimilarKeys.GetSize()-1;i++){
				for(int j=i+1;j<aSimilarKeys.GetSize();j++){
					if(aSimilarKeys[i].lSimilarity<aSimilarKeys[j].lSimilarity){
						CIHotkeyAndID tmp=aSimilarKeys[i];
						aSimilarKeys[i]=aSimilarKeys[j];
						aSimilarKeys[j]=tmp;
					}
				}
			}
			for(i=0;i<aSimilarKeys.GetSize();i++){
				CIHotkey hkSim;
				CRegisteredHotkeyData sDsc;
				CIHotkeyAndID iItem=aSimilarKeys[i];
				GetRegisteredHotkeyDsc(iItem.dwID,iItem.hWin,&hkSim,&sDsc);
				sMaybeKeys+=GetIHotkeyName(hkSim);
				sMaybeKeys+=":\r\n - ";
				sMaybeKeys+=sDsc.GetDsc();
				sMaybeKeys+="\r\n";
			}
		}
		if(sMaybeKeys==""){
			sMaybeKeys=GetListOfCurrentlyRegisteredHotkeys();
		}
		CWnd* pWnd=GetDlgItem(IDC_DESCRIPTION);
		if(pWnd){
			pWnd->SetWindowText(sMaybeKeys);
		}
	}
	return (bBlockResult?CHR_BLOCK_KEY:0)
		|(bWasHotkeyTriggered?CHR_HOTKEY_KEY:0)
		|(bWasEventHotkeyTriggered?CHR_EVEKEY_KEY:0);
}

extern CIHotkey g_cantBeLockedHotkey;
extern BOOL g_bTeporalyBlockHotkeys;
extern BOOL g_bTeporalyBlockAll;
extern BOOL g_bTeporalyBlockAllButHK;
DWORD WINAPI DisableCapsLock(LPVOID pData)
{
	if(g_bTeporalyBlockHotkeys || g_bTeporalyBlockAll){
		return 0;
	}
	int iType=(int)pData;
	if(!iType){
		BYTE keyState[256];
		GetKeyboardState((LPBYTE)&keyState);
		iType=((keyState[VK_CAPITAL] & 1)!=0);
	}
	if(iType){
		g_bTeporalyBlockHotkeys++;
		::keybd_event(VK_CAPITAL, 0, KEYEVENTF_EXTENDEDKEY, 0);
		Sleep(50);
		::keybd_event(VK_CAPITAL, 0, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
		Sleep(50);
		g_bTeporalyBlockHotkeys--;
	}
	return 0;
}


BOOL CDLG_KMaster::SetLockIconC()
{
	static NOTIFYICONDATA m_tnd={0};
	bCLIsON=(GetKeyState(VK_CAPITAL)&1);
	if(bCLIsON){
		if(g_hookData.bCLIcon && m_tnd.cbSize==0){
			memset(&m_tnd,0,sizeof(m_tnd));
			m_tnd.cbSize = sizeof(m_tnd);
			m_tnd.hWnd   = this->GetSafeHwnd();
			m_tnd.hIcon  = ::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICON_CL));
			m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;//| NIF_STATE
			m_tnd.uID    = 1;
			m_tnd.uCallbackMessage = NULL;
			_tcscpy(m_tnd.szTip, "CapsLock");
			Shell_NotifyIcon(NIM_ADD, (PNOTIFYICONDATA)&m_tnd);
		}
	}else if(m_tnd.cbSize!=0){
		m_tnd.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, (PNOTIFYICONDATA)&m_tnd);
		m_tnd.cbSize=0;
	}
	return 1;
}


BOOL CDLG_KMaster::SetLockIconN()
{
	static NOTIFYICONDATA m_tnd={0};
	bNLIsON=(GetKeyState(VK_NUMLOCK)&1);
	if(bNLIsON){
		if(g_hookData.bNLIcon && m_tnd.cbSize==0){
			memset(&m_tnd,0,sizeof(m_tnd));
			m_tnd.cbSize = sizeof(m_tnd);
			m_tnd.hWnd   = this->GetSafeHwnd();
			m_tnd.hIcon  = ::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICON_NL));
			m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;//| NIF_STATE
			m_tnd.uID    = 2;
			m_tnd.uCallbackMessage = NULL;
			_tcscpy(m_tnd.szTip, "NumLock");
			Shell_NotifyIcon(NIM_ADD, (PNOTIFYICONDATA)&m_tnd);
		}
	}else if(m_tnd.cbSize!=0){
		m_tnd.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, (PNOTIFYICONDATA)&m_tnd);
		m_tnd.cbSize=0;
	}
	return 1;
}

BOOL CDLG_KMaster::SetLockIconS()
{
	static NOTIFYICONDATA m_tnd={0};
	bSLIsON=(GetKeyState(VK_SCROLL)&1);
	if(bSLIsON){
		if(g_hookData.bSLIcon && m_tnd.cbSize==0){
			memset(&m_tnd,0,sizeof(m_tnd));
			m_tnd.cbSize = sizeof(m_tnd);
			m_tnd.hWnd   = this->GetSafeHwnd();
			m_tnd.hIcon  = ::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICON_SL));
			m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;//| NIF_STATE
			m_tnd.uID    = 3;
			m_tnd.uCallbackMessage = NULL;
			_tcscpy(m_tnd.szTip, "ScrollLock");
			Shell_NotifyIcon(NIM_ADD, (PNOTIFYICONDATA)&m_tnd);
		}
	}else if(m_tnd.cbSize!=0){
		m_tnd.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, (PNOTIFYICONDATA)&m_tnd);
		m_tnd.cbSize=0;
	}
	return 1;
}

BOOL bAllowSingleWin=0;
LRESULT CDLG_KMaster::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(WKUtils::isWKUpAndRunning()){
		if(message==WM_INPUT){
			MSG pMsg;
			pMsg.hwnd=GetSafeHwnd();
			pMsg.message=message;
			pMsg.wParam=wParam;
			pMsg.lParam=lParam;
			hidmanager.HandleHidInput(&pMsg);
			return 1;//If an application processes this message, it should return zero.
		}
		static BOOL bP=0;
		if(bP==0){
			bP=1;
			::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
		}
		if(message==WM_NCLBUTTONDOWN){
			if(wParam==HTCLOSE){
				if(g_bDialogActive){
					ActivateGuide(FALSE);
				}
				return 0;
			}
		}
		if(message==WM_SYSCOMMAND){
			if(wParam==SC_MINIMIZE || wParam==SC_CLOSE){
				if(g_bDialogActive){
					ActivateGuide(FALSE);
				}
				return 0;
			}
		}
		if(message==MSGKB_EXIT_RW || message==WM_ENDSESSION){
			DestroyWindow();
			PostQuitMessage(0);
			return TRUE;
		}
		if(message==MSGKB_SHOWOSD || message==MSGKB_SHOWOSD2 || message==MSGKB_SHOWOSD3){
			if(g_hookData.bScanOSD || g_hCathcerWnd){
				static long lNoRec=0;
				if(lNoRec>0){
					return TRUE;
				}
				InterlockedIncrement(&lNoRec);
				CString sText;
				static long iCount=1;
				sText.Format("[%i]. %i: %s: %08X (%08X)",iCount++,message-WM_USER,_l("Scan code"),wParam, lParam);
				if(g_hookData.bScanOSD){
					WKGetPluginContainer()->ShowOSD(sText,1000);
				}
				if(g_hCathcerWnd){
					::PostMessage(g_hCathcerWnd,WM_COMMAND,IDC_SHDROCA,(LPARAM)strdup(sText));
				}
				InterlockedDecrement(&lNoRec);
			}
			return TRUE;
		}
		if(message==MSGKB_ACTIVHGUID_RW){
			ActivateGuide(lParam);
			return TRUE;
		};
		if(message==MSGKB_CURSORHIDE){
			ShowHideCursor(lParam);
			return TRUE;
		}
		CIHotkey hk;
		BOOL bBlockThisKey=FALSE;
		BOOL bNeedCheckHotkey=1;
		BOOL bHotkeyRetrieved=FALSE;
		static BOOL bLastIsHotkey=0;
		static BOOL bAnyKeyFromWinPressed=0;
		if(message==MSGKB_JUSTINFORM_MM){
			CWKSYSEvent pEvent;
			CPoint pt;
			GetCursorPos(&pt);
			pEvent.iCurPosX=pt.x;
			pEvent.iCurPosY=pt.y;
			pEvent.iEventType=WKSYSEVENTTYPE_MOUSEMOVE;
			WKGetPluginContainer()->NotifyAboutSystemEvent(&pEvent);
			return TRUE;
		}
		if(message==MSGKB_JUSTINFORM){
			if(wParam==VK_EVENTKEY){
				// С ивентами мы больше тут не играемся
				return 0;
			}
			BOOL bNotPressed=(1&(lParam>>31));
			BOOL bInjected=(1&(lParam>>25));
			BOOL bHonestAutorepeat=(1&(lParam>>26));
			BOOL bOneOfWinKey=(wParam==VK_LWIN || wParam==VK_RWIN);//wParam==VK_MENU || 
			if(bOneOfWinKey && bAllowSingleWin>0 && bNotPressed){
				//FLOG2("bAllowSingleWin -> 0",0,0);
				bAllowSingleWin=0;
				// не проверяем не блокируем
				//FLOG2("Firing fake WIN-0x40",0,0);
				::keybd_event(0x40, ::MapVirtualKey(0x40,0),0,0);
				::keybd_event(0x40, ::MapVirtualKey(0x40,0),KEYEVENTF_KEYUP,0);
				return 0;
			}
			if(wParam==VK_NULL){
				bInjected=0;
				bNotPressed=0;
				if(lParam==0 && hReset){
					// сбрасывает не-самосбрасывающиеся клавиши
					hReset(2);
				}
			}
			static LPARAM lLastLPARAM=0;
			static WPARAM lLastWPARAM=0;
			static BOOL bNeddInitCapses=1;
			static BOOL bAutoRepeat=0;
			if(bNotPressed){
				bLastIsHotkey=0;
			}
			if(bNeddInitCapses==1 || wParam==VK_SCROLL || wParam==VK_NUMLOCK || wParam==VK_CAPITAL){
				bNeddInitCapses=0;
				SetEvent(hLocksKeys);
			}
			if(lParam==lLastLPARAM && wParam==lLastWPARAM){
				bNeedCheckHotkey=0;
				// Но! если включено автоповторение,
				// то пропускаем если а) последее действие увенчалось хоткеем
				// б) небыло отжатия клавиш
				if(g_hookData.bAutoRepeat && bLastIsHotkey){
					bNeedCheckHotkey=1;
				}
				// Или есть признак P&H - то без автоповторения 
				// на один единственный раз
				if(g_hookData.lPHTimeout>0){
					if(!bHotkeyRetrieved){
						bHotkeyRetrieved=TRUE;
						hk=getCurrentlyPressedKeys(FALSE);
					}
					static BOOL bPressAndHoldWasAlready=FALSE;
					if(hk.IsKeysInHotkey(CIHotkey(VK_PRESSNHOLD,0))){
						if(bPressAndHoldWasAlready==FALSE){
							bPressAndHoldWasAlready=TRUE;
							bNeedCheckHotkey=1;
						}else if(bLastIsHotkey){
							// Для P&H автоповторение не должно работать!
							bNeedCheckHotkey=0;
						}
					}else{
						bPressAndHoldWasAlready=FALSE;
					}
				}
			}
			if(wParam!=VK_NULL){
				static WPARAM wLastKey=0;
				if(bNotPressed){
					bAutoRepeat=0;
				}else if(wLastKey==wParam || bHonestAutorepeat){
					bAutoRepeat=1;
				}else{
					bAutoRepeat=0;
				}
				wLastKey=wParam;
				if(!bNotPressed && !g_bTeporalyBlockAll){
					SimpleLocker cs(&csKeySequence);
					static DWORD dwLastPress=0;
					if(dwLastPress!=0 && GetTickCount()-dwLastPress>5000){
						aKeySequence.RemoveAll();
					}
					dwLastPress=GetTickCount();
					// Уравниваем Disticnkts
					aKeySequence.InsertAt(0,wParam);
					aKeySequence.SetSize(10);
				}
				//FLOG2("Adding symbol=%i (notPressed=%i)",wParam,bNotPressed);
				if((g_bTeporalyBlockAll)){
					if(!bHotkeyRetrieved){
						bHotkeyRetrieved=TRUE;
						hk=getCurrentlyPressedKeys(FALSE);
					}
					if(hk!=g_cantBeLockedHotkey){
						static BOOL bHaveToSkipOneUnpress=0;
						if(!g_hookData.bBlockMouse && hk.ContainMouseClicks()){
							bHaveToSkipOneUnpress=1;
							bNeedCheckHotkey=1;
						}else{
							if(bHaveToSkipOneUnpress && bNotPressed){
								bHaveToSkipOneUnpress=0;
							}else{
								return TRUE;
							}
						}
					}else{
						bNeedCheckHotkey=1;
					}
				}
				if(g_bTeporalyBlockHotkeys){
					if(!bHotkeyRetrieved){
						bHotkeyRetrieved=TRUE;
						hk=getCurrentlyPressedKeys(FALSE);
					}
					if(hk!=g_cantBeLockedHotkey){
						return FALSE;
					}
					bNeedCheckHotkey=1;
				}
			}
			if(bNeedCheckHotkey){
				if(!bNotPressed){
					CWKSYSEvent pEvent;
					pEvent.iEventType=WKSYSEVENTTYPE_KEYPRESS;
					pEvent.pData=(LPVOID)wParam;
					WKGetPluginContainer()->NotifyAboutSystemEvent(&pEvent);
					wParam=(WPARAM)pEvent.pData;
				}else{
					CWKSYSEvent pEvent;
					pEvent.iEventType=WKSYSEVENTTYPE_KEYUNPRESS;
					pEvent.pData=(LPVOID)wParam;
					WKGetPluginContainer()->NotifyAboutSystemEvent(&pEvent);
					wParam=(WPARAM)pEvent.pData;
				}
				lLastWPARAM=wParam;
				lLastLPARAM=lParam;
				if(g_hookData.bShiftDropsCapslock==1 && bNeedCheckHotkey!=2){
					if((wParam==VK_SHIFT || wParam==VK_RSHIFT || wParam==VK_LSHIFT) && (1&(lParam>>31))==0){
						// Выключаем CapsLock
						FORK(DisableCapsLock,0);
					}
				}
				// Проверяем совпадение гор. клавиш
				if(!bHotkeyRetrieved){
					bHotkeyRetrieved=TRUE;
					hk=getCurrentlyPressedKeys(FALSE);
				}
				BOOL bModifier=0;
				if( wParam==VK_SHIFT  || wParam==VK_CONTROL
					|| wParam==VK_RSHIFT || wParam==VK_RCONTROL 
					|| wParam==VK_LSHIFT || wParam==VK_LCONTROL
					|| bOneOfWinKey){
						bModifier=TRUE;
					}
					DWORD dwCheckResult=CheckHotkeyConditions(hk,bNotPressed,bInjected,wParam,bAutoRepeat);
					bBlockThisKey=(dwCheckResult&CHR_BLOCK_KEY)?1:0;
#ifdef _DEBUG
					if(bBlockThisKey){
						//FLOG2("Should be blocked!!! Blocking %x:%i",wParam,bBlockThisKey);
					}
#endif

					// Запоминаем горячность последней клавиши
					bLastIsHotkey=(dwCheckResult&CHR_HOTKEY_KEY)?1:0;
					BOOL bLastIsHotkeyOfEvents=(dwCheckResult&CHR_EVEKEY_KEY)?1:0;
					if(bLastIsHotkey){
						if(g_hookData.bBlockWin==0 || g_hookData.bBlockWin>=4){
							BOOL bLWin=(GetAsyncKeyState(VK_LWIN)<0);
							BOOL bRWin=(GetAsyncKeyState(VK_RWIN)<0);
							if(bLWin || bRWin){
								//FLOG2("bAllowSingleWin -> 1",0,0);
								bAllowSingleWin=1;
								bModifier=0;
							}
						}
						if(hReset){
							// сбрасывает не-самосбрасывающиеся клавиши
							hReset(3);
						}
						ShowHideCursor(1);
						if(bLastIsHotkeyOfEvents){
							// Если были ивенты - сбрасываем все
							SimpleLocker cs(&csKeySequence);
							aKeySequence.RemoveAll();
						}
					}
					// Нах. неудобно
					if(bModifier){
						//FLOG2("Blocking %x:%i skipped!",wParam,bBlockThisKey);
						bBlockThisKey=0;
					}
					BOOL bShowPossiblyOSD=0;
					if(g_bTeporalyBlockAllButHK){
						// Обязательно блокируем даже транспаренты
						// так как типа пауза!
						// горячие клавиши по идее уже отработали
						// if any
						//FLOG2("Should be blocked!!! (pause) %x:%i",wParam,bBlockThisKey);
						bShowPossiblyOSD=1;
						bBlockThisKey=1;
					}
					if(g_hookData.bUnpauseOnHK && g_bTeporalyBlockAllButHK &&  (g_cantBeLockedHotkey.Options() & ADDOPT_ON_WPRESSED)==0){
						BOOL bNotUnpauseHK=g_cantBeLockedHotkey.isEqual(hk,1);
						if(bLastIsHotkey && !bNotUnpauseHK){
							bShowPossiblyOSD=0;
							WKCallPluginFunction(4,0);
						}
					}
					if(bShowPossiblyOSD && g_bTeporalyBlockAllButHK/*g_bTeporalyBlockAllButHK может изменится в паралельном потоке*/){
						// Покажем в OSD...
						CString sKeyName=GetIHotkeyName(hk);
						if(sKeyName!=""){
							//FLOG1("OSD = %s",sKeyName);
							WKGetPluginContainer()->ShowOSD(sKeyName,5000);
						}
					}
			}
			if(g_hookData.bBlockApps || (g_hookData.bBlockWin>0 && g_hookData.bBlockWin!=4) || g_hookData.bShiftDropsCapslock==2){
				// Особый случай блокировки ВИНов, Аппсов и т.п.
				// На горячие клавиши мы уже проверили!!!
				if(!bHotkeyRetrieved){
					bHotkeyRetrieved=TRUE;
					hk=getCurrentlyPressedKeys(FALSE);
				}
				if(g_hookData.bShiftDropsCapslock==2 && (hk==CIHotkey(VK_CAPITAL,HOTKEYF_SHIFT) || hk==CIHotkey(VK_CAPITAL,HOTKEYF_LSHIFT) || hk==CIHotkey(VK_CAPITAL,HOTKEYF_RSHIFT))){
					FORK(DisableCapsLock,1);
					return TRUE;
				}
				if(g_hookData.bShiftDropsCapslock==2  && wParam==VK_CAPITAL){
					return TRUE;
				}
				BOOL bTestWin=(g_hookData.bBlockWin>=1 && g_hookData.bBlockWin<=3 && bOneOfWinKey);
				if(bTestWin){
					if(g_hookData.szWinBlockMask[0]!=0){
						bTestWin=WKGetPluginContainer()->TestRunningApplicationsByMask(g_hookData.szWinBlockMask);
					}
				}
				if(bTestWin){
					if(g_hookData.bBlockWin==1 && bOneOfWinKey){
						return TRUE;
					}
					if(g_hookData.bBlockWin==2 && wParam==VK_LWIN){
						return TRUE;
					}
					if(g_hookData.bBlockWin==3 && wParam==VK_RWIN){
						return TRUE;
					}
				}
				if(g_hookData.bBlockApps && wParam==VK_APPS){
					return TRUE;
				}
			}
			if(g_bDialogActive){
				if(hk==CIHotkey(VK_ESCAPE,0)){
					ActivateGuide(0);
				}
			}
			return bBlockThisKey;
		}
	}
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDLG_KMaster::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	static HBRUSH hBr=::CreateSolidBrush(GetSysColor(COLOR_INFOBK));
	pDC->SetBkColor(GetSysColor(COLOR_INFOBK));
	pDC->SetTextColor(GetSysColor(COLOR_INFOTEXT));
	return hBr;
}

void CDLG_KMaster::ActivateGuide(BOOL bShow)
{
	BOOL g_bDialogActiveLast=g_bDialogActive;
	g_bDialogActive=bShow;
	if(g_bDialogActiveLast!=bShow){
		if(!g_bDialogActiveLast){
			SetWindowPos(&wndTopMost,0,0,0,0,0x4000|SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
			SwitchToWindow(GetSafeHwnd());
			CWnd* pWnd=GetDlgItem(IDC_DESCRIPTION);
			if(pWnd){
				pWnd->SetWindowText(GetListOfCurrentlyRegisteredHotkeys(GetMarkHKListChangedStatus()?TRUE:FALSE));
			}
		}else{
			ShowWindowAsync(GetSafeHwnd(),SW_HIDE);
		}
		SetWindowText(NULL_TITLE);
	}
}


