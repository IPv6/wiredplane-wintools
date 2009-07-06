#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SmartWarning(const char* szText, const char* szTitle, long* bAnswerHolder, DWORD dwTimer, DWORD dwTextTraits, CWnd* pParent)
{
	if(bAnswerHolder && *bAnswerHolder!=0){
		return;
	}
	InfoWndStruct* info=new InfoWndStruct();
	info->pParent=pParent;
	info->iIcon=IDI_ICON_INFO_AST;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	info->iStyle=objSettings.iAlertStyle;
	info->dwStaticPars=dwTextTraits;
	info->sOkButtonText=_l("Ok");
	info->dwDontAskHolder=bAnswerHolder;
	info->dwTimer=dwTimer;
	Alert(info);
}

// в bAnswerHolder: 0-спросить, 1-да, 2-нет
int AskYesNo(const char* szText, const char* szTitle, long* bAnswerHolder, CWnd* pParent)
{
	if(bAnswerHolder){
		if(*bAnswerHolder==1){
			return IDYES;
		}
		if(*bAnswerHolder==2){
			return IDNO;
		}
	}
	AsyncPlaySoundSys("SystemQuestion");
	InfoWndStruct* info=new InfoWndStruct();
	info->bAsModal=TRUE;
	info->iIcon=IDI_ICON_ASK;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	info->iStyle=objSettings.iAlertStyle;
	info->dwStaticPars=DT_CENTER|DT_VCENTER;
	info->sOkButtonText=_l("Yes");
	info->sCancelButtonText=_l("No");
	info->dwDontAskHolder=bAnswerHolder;
	info->pParent=pParent;
	int iRes=(LRESULT)Alert(info);
	if(iRes==1){
		return IDYES;
	}
	return IDNO;
}

int AskYesNo(const char* szText, long* bAnswerHolder, CWnd* pParent)
{
	return AskYesNo(szText, PROGNAME, bAnswerHolder, pParent);
}

void AddError(const char* szError, BOOL bSystem)
{
	if(bSystem){
		if(objSettings.bShowSystemErrors){
			Alert(szError,DEFAULT_READTIME);
		}
	}else{
		AsyncPlaySound(ERROR_SOUND);
	}
	CString sError=_l("Error occured")+": "+szError;
	objLog.AddMsgLogLine(DEFAULT_GENERALLOG,"%s",sError);
}

InfoWnd* Alert(const char* szText, const char* szTitle, DWORD dwTime)
{
	InfoWndStruct* info=new InfoWndStruct();
	info->iIcon=IDI_ICON_INFO_AST;
	info->sText=szText;
	info->sTitle=szTitle;
	if(dwTime!=0){
		info->dwTimer=dwTime;
	}
	info->bCenterUnderMouse=TRUE;
	info->iStyle=objSettings.iAlertStyle;
	info->dwStaticPars=DT_CENTER|DT_VCENTER;
	return Alert(info);
}

InfoWnd* Alert(const char* szText, DWORD dwTime, BOOL bStartHided, BOOL bStaticPars)
{
	InfoWndStruct* info=new InfoWndStruct();
	info->iIcon=IDI_ICON_INFO_AST;
	info->sText=szText;
	info->dwTimer=dwTime;
	info->bCenterUnderMouse=TRUE;
	info->bStartHided=bStartHided;
	info->iStyle=objSettings.iAlertStyle;
	if(bStaticPars==-1){
		info->dwStaticPars=DT_CENTER|DT_VCENTER|DT_SINGLELINE;
	}else{
		info->dwStaticPars=bStaticPars;
	}
	return Alert(info);
}

InfoWnd* Alert(const char* szText)
{
	return Alert(szText,DWORD(0));
}

InfoWnd* Alert(InfoWndStruct* pInfo)
{
	if(pMainDialogWindow){
		return (InfoWnd*)(pMainDialogWindow->SendMessage(FIREALERT,NULL,LPARAM(pInfo)));
	}
	if(!IsAppStopping()){
		AfxMessageBox(pInfo->sText);
	}
	return NULL;
}

void AlertBox(const char* szTitle, const char* szText, DWORD dwTime)
{
	InfoWndStruct* info=new InfoWndStruct();
	info->pParent=0;
	info->sText=szText;
	info->sTitle=szTitle;
	info->bCenterUnderMouse=TRUE;
	info->dwStaticPars=DT_CENTER;
	info->dwTimer=dwTime;
	Alert(info);
}

void OpenNote(CNoteSettings* pNoteParams)
{
	if(pMainDialogWindow){
		::PostMessage(pMainDialogWindow->m_hWnd,CREATE_NEWNOTE,0,(LPARAM)pNoteParams);
	}
	return;
}

void FillComboWithColorschemes(CComboBox* ID,int iStart)
{
	ID->ResetContent();
	for(long i=iStart;i<objSettings.objSchemes.GetSize();i++){
		ID->AddString(objSettings.objSchemes.getName(i));
	};
}

int FindColorSchemNumByName(const char* szName)
{
	int iRes=0;
	for(long i=0;i<objSettings.objSchemes.GetSize();i++){
		if(objSettings.objSchemes.getName(i)==szName){
			iRes=i;
			break;
		}
	}
	return iRes;
}

void deleteFont(CFont* font)
{
	if(font==NULL){
		return;
	}
	if(font!=NULL){
		VERIFY(font->DeleteObject()!=FALSE);
		delete font;
	}
}

void deleteBrush(CBrush* br)
{
	if(br==NULL){
		return;
	}
	if(br!=NULL){
		VERIFY(br->DeleteObject()!=FALSE);
		delete br;
	}
}

void GetActiveWindowTitle(CString& sNewTitle, CRect& ActiveWndRect, HWND* pActivWnd, DWORD* dwWindowLong)
{
	sNewTitle="";
	ActiveWndRect.SetRect(0,0,0,0);
	HWND pCurActivWnd=::GetForegroundWindow();
	if(!pCurActivWnd || !IsWindow(pCurActivWnd)){
		return;
	}
	if(SafeGetWindowText(pCurActivWnd,sNewTitle)==0){
		return;
	};
	::GetWindowRect(pCurActivWnd,ActiveWndRect);
	if(pActivWnd){
		*pActivWnd=pCurActivWnd;
	}else{
		objSettings.hHotMenuWnd=pCurActivWnd;
	}
	if(dwWindowLong){
		char szBuffer[64]="";
		*dwWindowLong=::GetWindowLong(pCurActivWnd,GWL_EXSTYLE);
	}
	objSettings.hFocusWnd=::GetFocus();
	return;
}

BOOL PtInWindowRect(POINT pt, CWnd* wnd)
{
	CRect rect;
	wnd->GetWindowRect(rect);
	return rect.PtInRect(pt);
}

BOOL MouseInWndRect(CWnd* wnd)
{
	POINT p;
	GetCursorPos(&p);
	return PtInWindowRect(p,wnd);
}

HRESULT FileToStream(LPCTSTR szFile, IStream* *ppStream)
{
	HRESULT hr;
	HGLOBAL hGlobal = NULL;

	// open file
	HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return HRESULT_FROM_WIN32(::GetLastError());

	// get file size
	DWORD dwBytesRead = 0, dwFileSize = ::GetFileSize(hFile, NULL);
	if (-1 != dwFileSize)
	{
		hGlobal = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, dwFileSize);
		if (NULL != hGlobal)
		{
			PVOID pvData = ::GlobalLock(hGlobal);
			ATLASSERT(NULL != pvData);

			// read file and store in global memory
			::ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
			::GlobalUnlock(hGlobal);

			if (dwFileSize != dwBytesRead)
			{
				VERIFY(::GlobalFree(hGlobal)==NULL);
				hr = E_FAIL;
			}
			else
				hr = S_OK;
		}
		else
			hr = E_OUTOFMEMORY;
	}
	else
		hr = HRESULT_FROM_WIN32(::GetLastError());

	::CloseHandle(hFile);

	// create IStream* from global memory
	if (SUCCEEDED(hr))
		hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, ppStream);
	return hr;
}

int iC=0;
#define	TIMER_DIVIDER_FORMTRACK		500
DWORD WINAPI MouseTrackerThread(LPVOID lpdwstatus)
{
	MouseTrackerRuler* pRuler=(MouseTrackerRuler*)lpdwstatus;
	if(!pRuler){
		return 0;
	}
	if(pRuler->MousePreProc){
		pRuler->MousePreProc(pRuler);
	}
	while(1){
		Sleep(TIMER_DIVIDER_FORMTRACK);
		if(!pMainDialogWindow ||
			pMainDialogWindow->iRunStatus!=1 ||
			!IsWindow(pMainDialogWindow->m_hWnd) ||
			!pRuler ||
			!pRuler->pWnd ||
			!IsWindow(pRuler->pWnd->m_hWnd))
		{
			continue;
		}
		if(pMainDialogWindow->bAnyPopupIsActive){
			continue;
		}
		POINT p;
		GetCursorPos(&p);
		CRect WindowRect;
		pRuler->pWnd->GetWindowRect(WindowRect);
		WindowRect.InflateRect(10,10);
		// Смотрим...
		BOOL bInWnd=WindowRect.PtInRect(p);
		BOOL bOnDT=TRUE;
		if(pRuler->dwTaskbarFriendly){
			APPBARDATA aDat;
			memset(&aDat,0,sizeof(aDat));
			aDat.cbSize=sizeof(aDat);
			SHAppBarMessage(ABM_GETTASKBARPOS,&aDat);
			CRect rTrayRectangle(aDat.rc);
			bOnDT=!rTrayRectangle.PtInRect(p);//objSettings.rDesktopRectangle.PtInRect(p);//например в трее
		}
		BOOL bFocusFr=pRuler->bFocusFriendly && (pRuler->pWnd->m_hWnd==::GetForegroundWindow());
		if(!bInWnd && bOnDT && !bFocusFr){
			//Курсор покинул окно
			if(pRuler->MouseOutProc){
				pRuler->MouseOutProc(pRuler);
			}
			if(pRuler->bStopWatching){
				continue;
			}
			// Выходим...
			ExitThread(0);
		};
	}
}

int GetStatusIcon(int iPerson)
{
	Person* prs=objSettings.AddrBook.aPersons[iPerson];
	if(prs->IsGroup()){
		return 5;
	}
	if(prs->iOnlineStatus<0 || prs->iOnlineStatus>4){
		return 6;
	}
	return prs->iOnlineStatus;
}

void WndShake(HWND hForegWnd, DWORD dwTime)
{
	if(::IsWindow(hForegWnd)){
		CRect ActiveWndRect;
		GetWindowRect(hForegWnd,&ActiveWndRect);
		DWORD dwStart=GetTickCount();
		while(GetTickCount()-dwStart<dwTime && ::IsWindow(hForegWnd)){
			int xr=(rand()&0x000000FF)/10-128/10;
			int yr=(rand()&0x000000FF)/10-128/10;
			::MoveWindow(hForegWnd,ActiveWndRect.left+xr,ActiveWndRect.top+yr,ActiveWndRect.Width(),ActiveWndRect.Height(),TRUE);
			Sleep(10);
		}
		::MoveWindow(hForegWnd,ActiveWndRect.left,ActiveWndRect.top,ActiveWndRect.Width(),ActiveWndRect.Height(),TRUE);
	}
}

DWORD WINAPI BlendInT(LPVOID p)
{
	User32Animate((HWND) p,900,AW_HIDE|AW_BLEND);
	return 0;
}

void WndBubbleHide(HWND pWnd, DWORD dwSpeed)
{
	if(::IsWindow(pWnd)){
		//FORK(BlendInT,pWnd);
		CRect WndRect;
		GetWindowRect(pWnd,&WndRect);
		WndRect.OffsetRect(-WndRect.left,-WndRect.top);
		CRgn NoteRgn;
		NoteRgn.CreateRectRgnIndirect(&WndRect);
		SetWindowRgn(pWnd,NoteRgn,FALSE);
		//SetLayeredWindowStyle(pWnd,TRUE);

		// Стираем по кусочкам...
		int iXC=(WndRect.left+WndRect.right)/2;
		int iYC=(WndRect.top+WndRect.bottom)/2;
		int iMaxRad=int(sqrt(double(WndRect.Width()*WndRect.Width()+WndRect.Height()*WndRect.Height())));
		int iSteps=10;
		int iDev=iMaxRad/iSteps;
		int iAlpah=95;
		for(int i=4;i<iSteps && ::IsWindow(pWnd);i++){
			HRGN hRgn=CreateRectRgn(0,0,0,0);
			::GetWindowRgn(pWnd, hRgn);
			int iRadius=rnd(i*iMaxRad/iSteps,i*iMaxRad/iSteps+iDev);
			int iX=iXC+rnd(-iDev,iDev);
			int iY=iYC+rnd(-iDev,iDev);
			CRgn hRgn2;
			hRgn2.CreateEllipticRgn(iX-iRadius/2,iY-iRadius/2,iX+iRadius/2,iY+iRadius/2);
			::CombineRgn(hRgn,hRgn,hRgn2,RGN_DIFF);
			SetWindowRgn(pWnd,hRgn,TRUE);
			DeleteObject(hRgn);
			/*SetWndAlpha(pWnd,iAlpah,1);
			iAlpah-=5;
			if(iAlpah<10){
				break;
			}*/
			Sleep(dwSpeed);
		}
	}
}

void WndBlend(HWND h, DWORD dwTime, DWORD dwParam)
{
	if(::IsWindow(h)){
		BOOL bActivate=0;
		if(dwParam & AW_ACTIVATE){
			bActivate=1;
			::SetWindowPos(h,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
			SetLayeredWindowStyle(h,TRUE);
			for(int i=40;i<100 && ::IsWindow(h);i+=7){
				SetWndAlpha(h,i,1);
				::ShowWindow(h,SW_SHOW);
				Sleep(dwTime/10);
			}
			SetLayeredWindowStyle(h,FALSE);
		}else{
			User32Animate(h,dwTime,dwParam|AW_BLEND);
		}
		if(bActivate){
			::ShowWindow(h,SW_SHOW);
		}
	}
}

void WndSlide(HWND h, DWORD dwTime, DWORD dwParam)
{	
	if(::IsWindow(h)){
		CRect ActiveWndRect;
		GetWindowRect(h,&ActiveWndRect);
		BOOL bXDir=ActiveWndRect.left>(theApp.rDesktopRECT.Width()/2);
		if(!(dwParam & AW_ACTIVATE)){
			bXDir=!bXDir;
		}
		BOOL bActivate=0;
		if(dwParam & AW_ACTIVATE){
			bActivate=1;
			CRect rt,rt2;
			::SetWindowPos(h,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
			::GetWindowRect(h,&rt);
			int iCount=1;
			SetProp(h,"ALLOW_OFFSCR",HANDLE(1));
			SetLayeredWindowStyle(h,TRUE);
			for(int i=20;i<100 && ::IsWindow(h);i+=7){
				SetWndAlpha(h,i,1);
				int iAddon=double(bXDir?1:-1)*((100.0-double(i))*double(rt.Width())/100.0);
				::SetWindowPos(h,HWND_TOPMOST,rt.left+iAddon,rt.top,rt.Width(),rt.Height(),SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOACTIVATE);
				Sleep(dwTime/10);
				iCount++;
			}
			SetLayeredWindowStyle(h,FALSE);
			SetProp(h,"ALLOW_OFFSCR",0);
			::SetWindowPos(h,HWND_TOPMOST,rt.left,rt.top,rt.Width(),rt.Height(),SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}else
		{
			User32Animate(h,dwTime,dwParam|AW_SLIDE|(bXDir?AW_HOR_NEGATIVE:AW_HOR_POSITIVE));
			/*bActivate=0;
			CRect rt,rt2;
			::SetWindowPos(h,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
			::GetWindowRect(h,&rt);
			int iCount=1;
			SetProp(h,"ALLOW_OFFSCR",HANDLE(1));
			SetLayeredWindowStyle(h,TRUE);
			InvalidateRect(h,0,1);
			for(int i=100;i>20;i-=7){
				SetWndAlpha(h,i,1);
				int iAddon=double(bXDir?1:-1)*((100.0-double(i))*double(rt.Width())/100.0);
				::SetWindowPos(h,HWND_TOPMOST,rt.left+iAddon,rt.top,rt.Width(),rt.Height(),SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOACTIVATE);
				Sleep(dwTime/10);
				iCount++;
			}
			SetProp(h,"ALLOW_OFFSCR",0);
			::ShowWindow(h,SW_HIDE);*/
		}
		if(bActivate){
			::ShowWindow(h,SW_SHOW);
		}
	}
}

// -1 - встряска
// 0 - удаление заметки
// 1 - новое сообщение
// 2 - удаление сообщения
BOOL IAnimateWindow(HWND pWnd, long lStyle, DWORD dwParam)
{
	BOOL bRes=TRUE;
	if(lStyle==-1){
		WndShake(pWnd,dwParam);
		return bRes;
	}
	if(lStyle==2){
		WndSlide(pWnd, dwParam, AW_HIDE);
		return bRes;
	}
	if(lStyle==0){
		switch(objSettings.iAnimOnNoteDel){
		case 1:
			WndBlend(pWnd, dwParam,AW_HIDE);
			break;
		case 2:
			WndBubbleHide(pWnd, 20);
			break;
		default:
			bRes=0;
		}
		::InvalidateRect(pWnd,0,1);
		return bRes;
	}
	if(lStyle==1){
		switch(objSettings.iAnimOnNewMsg){
		case 1:
			WndBlend(pWnd, dwParam, AW_ACTIVATE);
			break;
		case 2:
			WndSlide(pWnd, dwParam, AW_ACTIVATE);
			break;
		default:
			{
				bRes=0;
				::ShowWindow(pWnd,SW_SHOW);
			}
		}
		::InvalidateRect(pWnd,0,1);
		return bRes;
	}
	return 0;
}

BOOL IAnimateWindow(CWnd* pWnd, long lStyle, DWORD dwParam)
{
	if(!pWnd || !IsWindow(pWnd->GetSafeHwnd())){
		return 0;
	}
	return IAnimateWindow(pWnd->GetSafeHwnd(), lStyle, dwParam);
}

// Plays a specified waveform-audio file using MCI_OPEN and MCI_PLAY. 
// Returns when playback begins. Returns 0L on success, otherwise 
// returns an MCI error code.
DWORD playWAVEFile(const char* lpszWAVEFileName)
{
	UINT wDeviceID=0;
	DWORD dwReturn=0;
	MCI_OPEN_PARMS mciOpenParms;
	MCI_PLAY_PARMS mciPlayParms;

	// Open the device by specifying the device and filename.
	// MCI will choose a device capable of playing the specified file.
	mciOpenParms.lpstrAlias=NULL;
	mciOpenParms.dwCallback=0;
	mciOpenParms.lpstrDeviceType = "waveaudio";
	mciOpenParms.lpstrElementName = lpszWAVEFileName;
	if (dwReturn = mciSendCommand(0, MCI_OPEN,
		MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, 
		(DWORD)(LPVOID) &mciOpenParms))
	{
		// Failed to open device. Don't close it; just return error.
		return (dwReturn);
	}

	// The device opened successfully; get the device ID.
	wDeviceID = mciOpenParms.wDeviceID;

	// Begin playback. The window procedure function for the parent 
	// window will be notified with an MM_MCINOTIFY message when 
	// playback is complete. At this time, the window procedure closes 
	// the device.

	mciPlayParms.dwCallback = 0;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_WAIT, (DWORD)(LPVOID) &mciPlayParms);
	mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
	return dwReturn;
}

DWORD WINAPI PlaySoundInThread(LPVOID lpdwstatus)
{
	static long iSoundTracker=0;
	SimpleTracker Track(iSoundTracker);
	char* szSName=(char*)lpdwstatus;
	if(szSName==NULL){
		return 0;
	}
	CString sWav=szSName;
	delete szSName;
	if(sWav==ERROR_SOUND){
		if(objSettings.bErrorSound){
			sWav=objSettings.sWavOnError;
		}else{
			return 0;
		}
	}

	if(sWav==SOUND_ONACTIVATION){
		if(objSettings.bSoundOnActivation){
			sWav=objSettings.sWavOnActivation;
		}else{
			return 0;
		}
	}
	if(sWav==SOUND_ONMAIL){
		if(objSettings.bSoundOnMail){
			sWav=objSettings.sWavSoundOnMail;
		}else{
			return 0;
		}
	}
	if(sWav==SOUND_ONDONE){
		if(objSettings.bSoundOnDone){
			sWav=objSettings.sWavSoundOnDone;
		}else{
			return 0;
		}
	}
	if(sWav==SOUND_MSGSEND){
		if(objSettings.bSoundOnMsgSend){
			sWav=objSettings.sWavOnMsgSend;
		}else{
			return 0;
		}
	}
	if(sWav==SOUND_MSGDELIV){
		if(objSettings.bSoundOnMsgDeliv){
			sWav=objSettings.sWavOnMsgDeliv;
		}else{
			return 0;
		}
	}
	if(sWav==SOUND_MSGNOTDELIV){
		if(objSettings.bSoundOnMsgNotDeliv){
			sWav=objSettings.sWavOnMsgNotDeliv;
		}else{
			return 0;
		}
	}
	if(sWav==SOUND_ONMSG){
		if(objSettings.bPlaySoundOnMessage){
			sWav=objSettings.sMessageWavFile;
		}else{
			return 0;
		}
	}
	if(sWav.GetLength()>0){
		int iCount=1;
		int iAlarmPos=sWav.Find("alarm=");
		if(iAlarmPos>-1){
			CString sAlarmCount=sWav.Mid(iAlarmPos+strlen("alarm="));
			if(sAlarmCount.SpanExcluding("?&")=="h12"){
				iCount=COleDateTime::GetCurrentTime().GetHour();
				iCount=iCount%12;
				if(iCount==0){
					iCount=12;
				}
			}else{
				iCount=atol(sAlarmCount);
			}
		}
		int iSleep=2000;
		int iSleepPos=sWav.Find("sleep=");
		if(iSleepPos>-1){
			iSleep=atol(sWav.Mid(iSleepPos+strlen("sleep=")));
		}
		sWav=sWav.SpanExcluding("?&");
		CString sWavFile;
		if(sWav.Find(SOUND_CATALOG)==0){
			sWavFile=CString(GetApplicationDir())+sWav;
		}else{
			sWavFile=sWav;
		}
		for(int k=0;k<iCount;k++){
			PlaySound(sWavFile,NULL,SND_FILENAME|SND_ASYNC);//SND_FILENAME|SND_NODEFAULT|SND_NOSTOP|SND_NOWAIT|SND_ASYNC - падает! :(
			if(k<iCount-1){
				Sleep(iSleep);
			}
		}
	}
	return 0;
}

void AsyncPlaySound(const char* szSoundName)
{
	if(!szSoundName){
		return;
	}
	char* szStr=new char[strlen(szSoundName)+1];
	strcpy(szStr,szSoundName);
	DWORD dwThreadID=0;
	HANDLE hThread = CreateThread( NULL, 0, PlaySoundInThread, (void*)szStr, 0, &dwThreadID);
	CloseHandle(hThread);
}

CString getLinkedID(const char* szLinkID)
{
	CString sOut=szLinkID;
	sOut+="L";
	return sOut;
}

void InitTRIconList(CComboBoxEx& list)
{
	int iCount=0,iCount2=TR_ICONS_OFFSET;
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
	AddComboBoxExItem(&list,iCount++,"",iCount2++);
}

BOOL& IsEmbedded();
CString getActionHotKeyDsc(int iActionNum,const char* szBefore, const char* szAfter)
{
	if(IsEmbedded()){
		return "";
	}
	CString sRes=GetIHotkeyName(objSettings.DefActionMapHK[iActionNum]);
	if(szBefore){
		if(sRes!=""){
			sRes=CString(szBefore)+sRes;
		};
	};
	if(szAfter){
		if(sRes!=""){
			sRes+=szAfter;
		};
	};
	return sRes;
};

/*
Поддержка компрессии в NT          Оценить        
От:  Lonely Dog   

- RtlDecompressBuffer
- RtlCompressBuffer

RtlCompressBuffer имеет следующий прототип: 
NTSYSAPI 
ULONG 
NTAPI 
RtlCompressBuffer ( 
IN USHORT CompressionFormatAndEngine, 
IN PUCHAR UncompressedBuffer, 
IN ULONG UncompressedBufferSize, 
OUT PUCHAR CompressedBuffer, 
IN ULONG CompressedBufferSize, 
IN ULONG UncompressedChunkSize, 
OUT PULONG FinalCompressedSize, 
IN PVOID WorkSpace 
); 

Если ты разобрался с этой функцией, 
то может подскажежешь, что такое WorkSpace? 
и каким должен быть его размер 

Кстати в ntifs.h определено 

#define COMPRESSION_FORMAT_LZNT1 (0x0002) 
#define COMPRESSION_ENGINE_MAXIMUM (0x0100) 
#define COMPRESSION_ENGINE_HIBER (0x0200) 

но у меня работает только COMPRESSION_FORMAT_LZNT1 (WinXP) 

Вот пример работы с RtlCompressBuffer. (Из этого можно сделать простейший архиватор, 
если усекать файл до сжатого размера) 

#include <windows.h> 
#include <stdio.h> 

#define NTSYSAPI extern "C" 

#define COMPRESSION_FORMAT_NONE (0x0000) // winnt 
#define COMPRESSION_FORMAT_DEFAULT (0x0001) // winnt 
#define COMPRESSION_FORMAT_LZNT1 (0x0002) // winnt 

#define COMPRESSION_ENGINE_STANDART (0x0000) // winnt 
#define COMPRESSION_ENGINE_MAXIMUM (0x0100) // winnt 
#define COMPRESSION_ENGINE_HIBER (0x0200) // winnt 

NTSYSAPI 
ULONG 
NTAPI 
RtlGetCompressionWorkSpaceSize ( 
IN USHORT CompressionFormatAndEngine, 
OUT PULONG CompressBufferWorkSpaceSize, 
OUT PULONG CompressFragmentWorkSpaceSize 
); 

NTSYSAPI 
ULONG 
NTAPI 
RtlCompressBuffer ( 
IN USHORT CompressionFormatAndEngine, 

IN PUCHAR UncompressedBuffer, 
IN ULONG UncompressedBufferSize, 

OUT PUCHAR CompressedBuffer, 
IN ULONG CompressedBufferSize, 

IN ULONG UncompressedChunkSize, 

OUT PULONG FinalCompressedSize, 

IN PVOID WorkSpace 
); 

int main(int argc, char *argv[]) 
{ 
if (argc != 3) return 0; 

HANDLE hFile1 = CreateFile(argv[1], GENERIC_READ, 
FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0); 
HANDLE hFile2 = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, 
FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0); 

ULONG n = GetFileSize(hFile1, 0); 

HANDLE hMapping1 = CreateFileMapping(hFile1, 0, PAGE_READONLY, 0, 0, 0); 
HANDLE hMapping2 = CreateFileMapping(hFile2, 0, PAGE_READWRITE, 0, n, 0); 

PUCHAR p = PUCHAR(MapViewOfFileEx(hMapping1, FILE_MAP_READ, 0, 0, 0, 0)); 
PUCHAR q = PUCHAR(MapViewOfFileEx(hMapping2, FILE_MAP_WRITE, 0, 0, 0, 0)); 

ULONG ulBufferWsSize, ulFragmentWsSize; 
ULONG st; 
ULONG ulFormat = COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_STANDART; 
st = RtlGetCompressionWorkSpaceSize(ulFormat, &ulBufferWsSize, &ulFragmentWsSize); 

DWORD dwSize = 0; 

PVOID pv = GlobalAlloc(GPTR, ulBufferWsSize); 

DWORD dwSize1; 
st = RtlCompressBuffer(ulFormat, 
p, n, 
q, n, 
n, 
&dwSize, 
pv); 
printf("mem: %d\nSize = %d\n", ulBufferWsSize, dwSize); 
getchar(); 
return 0; 
} 

PS: Эту фигню надо линковать с ntdll.lib из DDK. 
PPS: COMPRESSION_FORMAT_LZNT1 - Алгоритм сжатия. (При проектировании NT, ребята из MS хотели добавить возможность расширения системы новыми 
алгоритмами сжатия, но сделали ли они это или нет, я не знаю.) 
COMPRESSION_ENGINE_XXX - Степень сжатия. 
Реально Hiber от standart отличается процентоа на 10 по степени сжатия, 
  но standart намного быстрее.
*/
DWORD GetDirectorySize(const char* szDirPath)
{
	HANDLE hFind;
	DWORD dwAllSize=0;
	WIN32_FIND_DATA FindData;
	CString sFileMask=szDirPath;
	sFileMask+="\\*.*";
	hFind=FindFirstFile(sFileMask,&FindData);
	int isFound=1;
	DWORD SavedRep=0;
	while(hFind!=INVALID_HANDLE_VALUE && isFound!=0){
		if (FindData.cFileName[0]!=TEXT('.') && FindData.cFileName[0] != 0)
			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0 )
				dwAllSize+=FindData.nFileSizeLow;
		isFound=FindNextFile(hFind,&FindData);
	};
	if (hFind!=INVALID_HANDLE_VALUE )
		FindClose(hFind);
	return dwAllSize;
}

//
int RemoveFirstFile(const char* szDirPath)
{
	HANDLE hFind;
	DWORD dwAllSize=0;
	WIN32_FIND_DATA FindData;
	DWORD dwFileSize=0;
	char szFileName[1024]="";
	FILETIME ftFirst;
	CString sFileMask=szDirPath;
	sFileMask+="\\*.*";
	hFind=FindFirstFile(sFileMask,&FindData);
	int isFound=1;
	DWORD SavedRep=0;
	while(hFind!=INVALID_HANDLE_VALUE && isFound!=0){
		if (FindData.cFileName[0]!=TEXT('.') && FindData.cFileName[0] != 0)
			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0 ){
				if (!strcmp(szFileName,"") || CompareFileTime(&ftFirst,&FindData.ftCreationTime)>0){
					strcpy(szFileName,FindData.cFileName);
					ftFirst=FindData.ftCreationTime;
					dwFileSize=FindData.nFileSizeLow;
				}
			}
		isFound=FindNextFile(hFind,&FindData);
	};
	if (hFind!=INVALID_HANDLE_VALUE)
		FindClose(hFind);
	if (strcmp(szFileName,"")){
		CString sFileToDelete=(CString(szDirPath)+"\\")+szFileName;
		SetFileAttributes(sFileToDelete,FILE_ATTRIBUTE_NORMAL);
		if(DeleteFile(sFileToDelete)==0){
			Alert(Format("Can`t delete file '%s', %s",sFileToDelete,GetCOMError(GetLastError())));
		}
	}
	return dwFileSize;
}

//
DWORD dwLastTrashCheckTime=0;
void PartlyEmptyTrash()
{
	if(objSettings.bMaxTrashSize<MIN_TRASH_SIZE){
		objSettings.bMaxTrashSize=MIN_TRASH_SIZE;
	}
	// Проверяем не чаще раза в полчаса
	if(GetTickCount()-dwLastTrashCheckTime>(30*60*1000)){
		dwLastTrashCheckTime=GetTickCount();
		DWORD dwMaxBytes=objSettings.bMaxTrashSize*1024;
		CString sTrashDir=getFileFullName(objSettings.sTrashDirectory,TRUE);
		long dwTrashBytes=GetDirectorySize(sTrashDir);
		while(dwTrashBytes>long(dwMaxBytes)){
			DWORD dwDeletedFileSize=RemoveFirstFile(sTrashDir);
			dwTrashBytes-=dwDeletedFileSize;
			if(dwDeletedFileSize<=0){
				break;
			}
		}
	}
	return;
}

/*void TimetToFileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}*/

BOOL GetFileDateTime(const char* szFile, FILETIME* tOut)
{
	if(!tOut){
		return 0;
	}
	HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile==INVALID_HANDLE_VALUE ){
		return 0;
	}
	FILETIME CreationTime,LastAccessTime;
	memset(&CreationTime,0,sizeof(FILETIME));
	memset(&LastAccessTime,0,sizeof(FILETIME));
	BOOL bRes=::GetFileTime(hFile,&CreationTime,&LastAccessTime,NULL);
	::CloseHandle(hFile);
  	if(!bRes){
		return 0;
	}
	memcpy(tOut,&CreationTime,sizeof(FILETIME));
	return TRUE;
}

DWORD dwLastSysRefreshTime=0;
void GetWndIcon(HWND& hSlayerWnd,HICON& hIcon)
{
	ClearIcon(hIcon);
	if(hIcon==NULL){
		hIcon=(HICON)::SendMessage(hSlayerWnd,WM_GETICON,ICON_SMALL,0);
	}
	if(hIcon==NULL){
		hIcon=(HICON)::SendMessage(hSlayerWnd,WM_GETICON,ICON_BIG,0);
	}
	if(hIcon==NULL){
		hIcon=theApp.MainImageList.ExtractIcon(APP_ICONS_OFFSET);
	}
}

void AddRecipientToRecentList(const char* szName)
{
	if(szName==NULL || strlen(szName)==0){
		return;
	}
	for(int j=0;j<objSettings.lMsgRecentList.GetSize();j++){
		if(objSettings.lMsgRecentList[j]==szName){
			// Удаляем ( с учетом дальнейшей вставки - Двигаем на первое место)
			objSettings.lMsgRecentList.RemoveAt(j);
			break;
		}
	}
	objSettings.lMsgRecentList.InsertAt(0,szName);
	if(objSettings.lMsgRecentList.GetSize()>=objSettings.lMsgRecentListSize){
		objSettings.lMsgRecentList.SetSize(objSettings.lMsgRecentListSize);
	}
	for(int i=0;i<objSettings.m_Notes.GetSize();i++){
		CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
		if(!pNote){
			continue;
		}
		pNote->SendMessage(UPDATERECLIST,0,0);
	}
}

void AddColorCodeIcon(COLORREF dwBg,COLORREF dwTx, const char* szKey)
{
	//
	CBitmap bmp;
	CRect rt(0,0,20,20);
	CDesktopDC dcScreen;
	bmp.CreateCompatibleBitmap(&dcScreen, rt.Width(), rt.Height());
	CDC dc;
	dc.CreateCompatibleDC(&dcScreen);
	CBitmap* pOld=dc.SelectObject(&bmp);
	dc.SetBkColor(dwBg);
	dc.FillSolidRect(&rt,dwBg);
	dc.SetTextColor(dwTx);
	dc.DrawText("T",1,rt,DT_CENTER|DT_VCENTER);
	dc.SetTextColor(0);
	CBrush br(COLORREF(0));
	dc.FrameRect(&rt,&br);
	_bmp().AddBmp(szKey,&bmp);
	dc.SelectObject(pOld);
}

void AppendBBMenu(CMenu* menu, int iSharedAlready, int iStart)
{
	int iLen=objSettings.aSubsBB.GetSize();
	if(iLen==0 || objSettings.sFBDir==""){
		return;
	}
	if(iLen==1){
		if(iSharedAlready){
			AddMenuString(menu,iStart,_l("Republish item"),_bmp().Get(IDB_BM_SHARE));
		}else{
			AddMenuString(menu,iStart,_l("Publish on billboard"),_bmp().Get(IDB_BM_SHARE));
		}
		return;
	}
	CMenu mn2;
	mn2.CreatePopupMenu();
	for(int i=0;i<iLen;i++){
		AddMenuString(&mn2,iStart+i,GetBBSectionName(objSettings.aSubsBB[i]));
	}
	if(iSharedAlready){
		AddMenuSubmenu(menu,&mn2,_l("Republish item")+"\t...",_bmp().Get(IDB_BM_SHARE));
	}else{
		AddMenuSubmenu(menu,&mn2,_l("Publish on billboard")+"\t...",_bmp().Get(IDB_BM_SHARE));
	}
}

void AppendStylesMenu(CMenu* menu, CItem* pItem, BOOL bAsSubMenu)
{
	CMenu* pNewMenu=NULL;
	if(bAsSubMenu){
		pNewMenu=new CMenu;
		pNewMenu->CreatePopupMenu();
	}else{
		pNewMenu=menu;
	}
	long i=0;
	for(i=0;i<objSettings.objSchemes.GetSize();i++){
		CString sKey=Format("ST_%i_%i",objSettings.objSchemes.getBgColor(i),objSettings.objSchemes.getTxtColor(i));
		if(!_bmp().IsPresent(sKey)){
			AddColorCodeIcon(objSettings.objSchemes.getBgColor(i),objSettings.objSchemes.getTxtColor(i),sKey);
		}
		CBitmap* pStyleBmp=_bmp().Get(sKey);
		AddMenuString(pNewMenu,WM_USER+CHANGE_NOTE_STYLE+i, objSettings.objSchemes.getName(i)+(objSettings.objSchemes.getAlpha(i)<100?Format("\t%lu%%",objSettings.objSchemes.getAlpha(i)):""), pStyleBmp, (i==pItem->GetColorCode()));
	};
	AppendAdverts(pNewMenu);
	AddMenuString(pNewMenu, ID_NOTEMENU_NEWSTYLE, _l("Change/Edit styles")+"\t...",_bmp().Get(IDB_BM_OPTIONS));
	if(bAsSubMenu){
		AddMenuSubmenu(menu,pNewMenu,_l("Change style")+"\t...",_bmp().Get(IDB_BM_CHANGECOL));
		delete pNewMenu;
	}
	AddMenuString(menu, ID_NOTEMENU_NEWFNT, _l("Change font/colors")+"\t...",_bmp().Get(_IL(98)));
}

void SetNotePosToCursor(CNoteSettings* pNoteSets)
{
	CPoint pt;
	::GetCursorPos(&pt);
	pNoteSets->iXPos=pt.x;
	pNoteSets->iYPos=pt.y;
	pNoteSets->bNeedFocusOnStart=TRUE;
}

CString GetDateInCurFormat(COleDateTime dt)
{
	/*if(objSettings.bShowDateAsAge==2){
		COleDateTimeSpan dtSpan=COleDateTime::GetCurrentTime()-dt;
		double dDays=dtSpan.GetTotalDays();
		CString sRes="";
		if(dDays>=1){
			sRes+=Format("%.2f",dDays);
		}
		return sRes;
	}*/
	return Format("%s %s",DateFormat(dt,1),TimeFormat(dt,1,1));//dt.Format(LOGTIMEFRMT);
}


const char* wd[]={"Zero","Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
const char* wd2[]={"Su","Mn","Tu","We","Th","Fr","Sa"};
CString GetWeekDay(long i,BOOL bShort)
{
	CString sRes="";
	if(bShort){
		sRes=_l(Format("Day%iShort",i),wd2[i]);
	}else{
		sRes=_l(Format("DayOfWeek%i",i),wd[i]);
	}
	if(sRes=="none"){
		sRes=bShort?wd2[i]:wd[i];
	}
	return sRes;
}

void WNSubstMyVariables(CString& sText, CString aWType)
{
	SubstMyVariables(sText);
	if(sText.Left(4).CompareNoCase("http")==0){
		sText="http://www.wiredplane.com/cgi-bin/wp_engine.php?who=WireNote&target="+aWType+"&url="+EscapeString2(sText);
	}
}

#include <shlobj.h>
#include "msscript.cpp"
#define DISPOBJNAME		"WireNote"
void InitMSScriptSite(MSScriptControl::IScriptControlPtr& ScriptEngine, const char* szLang)
{
	ScriptEngine.CreateInstance("MSScriptControl.ScriptControl"); 
	ScriptEngine->Language=szLang;
	ScriptEngine->Timeout = -1;//No timeout
}

BOOL CalculateExpression_Ole(CString& sExprIn)
{
	//cs.Lock();
	int iRes=0;
	::CoInitialize(NULL);
	CString sExpr="\n"+sExprIn;
	BOOL bJScriptLang=(sExpr.Find("\n'")==-1) || (sExpr.Find("//")!=-1);
	{// Создаем область видимости чтобы разрушение ком-объекта было нормальным
		BOOL bAddObj=(sExpr.Find(DISPOBJNAME".")!=-1);
		MSScriptControl::IScriptControlPtr ScriptEngine;
		InitMSScriptSite(ScriptEngine,(bJScriptLang?"JScript":"VBScript"));
		try {
			ScriptEngine->PutState((enum MSScriptControl::ScriptControlStates)0);
			_variant_t result;
			if(!bJScriptLang){
				sExpr=CString("Dim OUTPUT\r\n")+sExpr;
				ScriptEngine->ExecuteStatement(bstr_t(sExpr));
				sExpr="OUTPUT";
			}
			result=ScriptEngine->Eval(bstr_t(sExpr));
			sExprIn=(const char*)_bstr_t(result);
			iRes=1;
		}catch(_com_error err){
			//FLOG1("Script error: \n%s\n",sExpr)
			CString sErr((BSTR)err.Description());
			CString sErrSource((BSTR)err.Source());
			if(sErrSource!=""){
				sErr+="\n";
				sErr+=sErrSource;
			}
			if(sErr==""){
				sErr=GetCOMError(err.Error());
			}
			Alert(sErr,bJScriptLang?_l("JavaScript Error"):_l("VBScript Error"));
			iRes=0;
		};
	}
	::CoUninitialize();
	//cs.Unlock();
	return iRes;
}

CFileInfoArray& GetSmilesEnum()
{
	static CFileInfoArray dir;
	static BOOL bLoaded=0;
	if(bLoaded==0){
		bLoaded=1;
		dir.AddDir(GetAppFolder()+"\\Smiles\\","*.bmp",TRUE,0,FALSE);
	}
	return dir;
}