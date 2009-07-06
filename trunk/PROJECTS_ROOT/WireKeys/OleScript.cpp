#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include <shlobj.h>
#include "msscript.cpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CALC_FILE	"clcout.dat"
#define BAT_FILE	"script.bat"
extern CCriticalSection pCahLock;
BOOL CalculateExpression_Cmd(CString& sExpr)
{
	int iRes=0;
	char szScriptFile[MAX_PATH+5]="";
	if(GetTempFileName(GetUserFolder(),"CLC",time(NULL),szScriptFile)==0){
		strcpy(szScriptFile,GetUserFolder()+"script.tmp");
	}
	strcat(szScriptFile,".js");
	CString sScript=Format("WScript.Echo(%s);",sExpr);
	SaveFile(szScriptFile,sScript);
	CString sResFile=GetUserFolder()+CALC_FILE;
	CString sBatFile=GetUserFolder()+BAT_FILE;
	CString sRunString=CString("cscript.exe //E:JScript //Nologo \"")+szScriptFile+"\""+CString(" >\"")+sResFile+"\"";
	SaveFile(sBatFile,sRunString);
	STARTUPINFO stInfo;
	GetStartupInfo(&stInfo);
	PROCESS_INFORMATION pInfo;
	memset(&pInfo,0,sizeof(pInfo));
	if(CreateProcess(sBatFile,"",NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,GetUserFolder(),&stInfo,&pInfo)){
		WaitForSingleObject(pInfo.hProcess,3000);
		CloseHandle(pInfo.hProcess);
		CloseHandle(pInfo.hThread);
		if(!ReadFile(sResFile,sExpr)){
			iRes=-1;
		}else{
			sExpr.TrimLeft();
			sExpr.TrimRight();
			iRes=1;
		}
	}else{
		DWORD dw=GetLastError();
		iRes=-1;
	}
	DeleteFile(sResFile);
	DeleteFile(szScriptFile);
	DeleteFile(sBatFile);
	return iRes;
}

CString VariantToString(VARIANT * va)
{
    CString s;
    switch(va->vt)
	{ /* vt */
	case VT_BSTR:
		return CString(va->bstrVal);
	case VT_BSTR | VT_BYREF:
		return CString(*va->pbstrVal);
	case VT_I4:
		s.Format(_T("%d"), va->lVal);
		return s;
	case VT_I4 | VT_BYREF:
		s.Format(_T("%d"), *va->plVal);
	case VT_BOOL:
		s.Format(_T("%i"), va->bVal);
		return s;
	case VT_BOOL | VT_BYREF:
		s.Format(_T("%i"), *va->pbVal);
		return s;
	case VT_R4:
		s.Format(_T("%f"), va->fltVal);
		return s;
	case VT_R4 | VT_BYREF:
		s.Format(_T("%f"), *va->pfltVal);
		return s;
	case VT_R8:
		s.Format(_T("%f"), va->dblVal);
		return s;
	case VT_R8 | VT_BYREF:
		s.Format(_T("%f"), *va->pdblVal);
		return s;
	default:
		ASSERT(FALSE); // unknown VARIANT type (this ASSERT is optional)
		return CString("");
	} /* vt */
}

CCriticalSection csHandles;
CArray<HMODULE,HMODULE&> hInvokedDlls;
CStringArray sInvokedDlls;
void FreeInvokedDlls()
{
	csHandles.Lock();
	for(int i=0;i<hInvokedDlls.GetSize();i++){
		HMODULE h=hInvokedDlls[i];
		hInvokedDlls[i]=0;
		if(h!=0){
			FreeLibrary(h);
		}
	}
	hInvokedDlls.RemoveAll();
	sInvokedDlls.RemoveAll();
	csHandles.Unlock();
}

//typedef int (WINAPI *FPLUGINEXTFUNCTION)(const char* szParameters_in,char* szResult_out, size_t dwResultSize_in);

CString WKDISP_CallDLL(CStringArray& aParams)
{
  	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	CString sP2;
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	CString sP3;
	if(aParams.GetSize()>2){
		sP3=aParams[2];
	}
	if(sP1.GetLength()>0){
		HMODULE h=0;
		// Сначала смотрим можно ли ее GetModuleHandle
		h=GetModuleHandle(GetPathPart(sP1,0,0,1,1));
		if(h==0){
			csHandles.Lock();
			for(int i=0;i<hInvokedDlls.GetSize();i++){
				if(sP1==sInvokedDlls[i]){
					h=hInvokedDlls[i];
					break;
				}
			}
			if(h==0){
				// Не нашли. Если бы нашли - ниче сюда не добавляли бы!!!
				h=LoadLibrary(sP1);
				int iInd=hInvokedDlls.Add(h);
				sInvokedDlls.SetAtGrow(iInd,sP1);
			}
			csHandles.Unlock();
		}
		if(h==0){
			return "";
		}
		FPLUGINEXTFUNCTION fp=(FPLUGINEXTFUNCTION)DBG_GETPROC(h,sP2);
		if(fp==0){
			return "";
		}
		char szBuffer[2048]={0};
		int iRes=fp(sP3,szBuffer,sizeof(szBuffer));
		return szBuffer;
	}
	return "";
}

CString WKDISP_CallPlug(CStringArray& aParams)
{
	if(aParams.GetSize()!=2){
		return "";
	}
	CString sPath=aParams[0];
	if(sPath.Find("://")==-1){
		sPath=GetPluginsDirectory()+sPath+".wkp";
	}
	aParams[0]=sPath;
	aParams.InsertAt(1,"RenderScript");
	return WKDISP_CallDLL(aParams);
}

CString WKDISP_PlaySound(CStringArray& aParams)
{
	if(aParams.GetSize()==0){
		return "";
	}
	CString sPath=aParams[0];
	CString sP2;
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	if(sP2==""){
		sP2="0";
	}
	CString sP3;
	if(aParams.GetSize()>2){
		sP3=aParams[2];
	}
	if(sP3==""){
		sP3="WK_MACRO";
	}
	PlayMusicX(sPath,atol(sP2),sP3);
	return "";
}

CString WKDISP_PlayRecord(CStringArray& aParams)
{
	if(aParams.GetSize()==0){
		return "";
	}
	CString sPath=aParams[0];
	aParams.SetAtGrow(1,sPath);
	aParams.SetAtGrow(0,"WP_MacroRec");
	return WKDISP_CallPlug(aParams);
}

CString WKDISP_LoadUrl(CStringArray& aParams)
{
	if(aParams.GetSize()==0){
		return "";
	}
	CString sPath=aParams[0];
	CWebWorld url;
	url.SetErrorMessage("");
	return url.GetWebPage(sPath);
}

CString WKDISP_Sleep(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		Sleep(atol(aParams[0]));
	}
	return "";
}

CString WKDISP_SleepW(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		Sleep(DWORD(atof(aParams[0])*1000));
	}
	return "";
}

CString WKDISP_GetSel(CStringArray& aParams)
{
	return GetSelectedTextInFocusWnd();
}

CString WKDISP_SetClipboard(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		USES_CONVERSION;
		BOOL bThroughGlobal=0;
		SetClipboardText(A2W(aParams[0]), bThroughGlobal, 0);
	}
	return "";
}


CString WKDISP_PutSel(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		PutSelectedTextInFocusWnd(aParams[0]);
	}
	return "";
}

CString WKDISP_MsgB(CStringArray& aParams)
{
 	CString sText="";
	if(aParams.GetSize()>0){
		sText=aParams[0];
	}
	DWORD dwType=MB_OK|MB_ICONINFORMATION;
	if(aParams.GetSize()>1){
		dwType=atolx(aParams[1]);
	}
	int iRes=AfxMessageBox(sText,dwType|MB_SETFOREGROUND|MB_TOPMOST);
	return Format("%i",iRes);
}

CString WKDISP_Alert(CStringArray& aParams)
{
 	CString sText="";
 	CString sTitle=PROGNAME;
	if(aParams.GetSize()>0){
		sText=aParams[0];
	}
	if(aParams.GetSize()>1){
		sTitle=aParams[1];
	}
	DWORD dwTimer=0;
	if(aParams.GetSize()>2){
		dwTimer=atol(aParams[2]);
	}
	Alert(sText,sTitle,dwTimer*1000);
	return "";
}

CString WKDISP_GetWndClass(CStringArray& aParams)
{
  CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	HWND h=(HWND)atolx(sP1);
	char sz[256]={0};
	::GetClassName(h,sz,sizeof(sz));
	return sz;
}

CString WKDISP_GetWndText(CStringArray& aParams)
{
  	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	HWND h=(HWND)atolx(sP1);
	char sz[256]={0};
	::GetWindowText(h,sz,sizeof(sz));
	return sz;
}

CString WKDISP_TestDosMask(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	return PatternMatch(sP1,sP2)?"1":"0";
}
	
CString WKDISP_GetWndFromPoint(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	CPoint pt(atol(sP1),atol(sP2));
	HWND hOut=WindowFromPoint(pt);
	CString sP3;
	if(aParams.GetSize()>2){
		sP3=aParams[2];
	}
	if(hOut && atol(sP3)){
		//CPoint pt();
		ScreenToClient(hOut,&pt);
		hOut=ChildWindowFromPointEx(hOut,pt,0);
	}
	return Format("0x%08X",hOut);
}

CString WKDISP_GetWndFromCursor(CStringArray& aParams)
{
  	CPoint pt;
	GetCursorPos(&pt);
	HWND hOut=WindowFromPoint(pt);
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(hOut && atol(sP1)){
		//CPoint pt();
		ScreenToClient(hOut,&pt);
		hOut=ChildWindowFromPointEx(hOut,pt,0);
	}
	return Format("0x%08X",hOut);
}

CString WKDISP_GetForeWindow(CStringArray& aParams)
{
	HWND hOut=GetForegroundWindow();
	if(hOut){
		return Format("0x%08X",hOut);
	}
	return "";
}

CString WKDISP_WaitForWindow(CStringArray& aParams)
{
	DWORD dwMaxTimeout=10000;
	if(aParams.GetSize()>0){
		dwMaxTimeout=atolx(aParams[0])*1000;
	}
	CString sAddText;
	if(aParams.GetSize()>1){
		sAddText=aParams[1];
		sAddText.MakeLower();
	}
	int iRes=0;
	char szTitle[256]={0};
	HWND h=GetForegroundWindow();
	if(h==0){
		return "0";
	}
	::GetWindowText(h,szTitle,sizeof(szTitle)-1);
	char szTitle2[256]={0};
	DWORD dwStart=GetTickCount();
	while(dwStart+dwMaxTimeout>GetTickCount()){
		if(!IsWindow(h)){
			iRes=1;
			break;
		}
		::GetWindowText(h,szTitle2,sizeof(szTitle2)-1);
		if(strcmp(szTitle2,szTitle)!=0){
			iRes=1;
			break;
		}
		if(sAddText!=""){
			strlwr(szTitle2);
			if(strstr(szTitle2,sAddText)!=0){
				iRes=1;
				break;
			}
		}
	}
	return Format("%i",iRes);
}

CString WKDISP_Ask(CStringArray& aParams)
{
	CString sTitle="Enter text";
	if(aParams.GetSize()>0){
		sTitle=aParams[0];
	}
	CString sValue="";
	if(aParams.GetSize()>1){
		sValue=aParams[1];
	}
	pMainDialogWindow->SendMessage(EDITTEXT,(WPARAM)&sTitle, (LPARAM)&sValue);
	return sValue;
}

CString WKDISP_SetWindowPosition(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	long X=0;
	if(aParams.GetSize()>1){
		X=atolx(aParams[1]);
	}
	long Y=0;
	if(aParams.GetSize()>2){
		Y=atolx(aParams[2]);
	}
	if(H){
		SetWindowPos(H,0,X,Y,0,0,SWP_NOZORDER|SWP_NOSIZE);
	}
	return "1";
}

CString WKDISP_SetWindowSize(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	long X=0;
	if(aParams.GetSize()>1){
		X=atolx(aParams[1]);
	}
	long Y=0;
	if(aParams.GetSize()>2){
		Y=atolx(aParams[2]);
	}
	if(H){
		SetWindowPos(H,0,0,0,X,Y,SWP_NOZORDER|SWP_NOMOVE);
	}
	return "1";
}

CString WKDISP_EnableWindow(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	UINT M=1;
	if(aParams.GetSize()>1){
		M=(UINT)atolx(aParams[1]);
	}
	int iRes=::EnableWindow(H,M);
	return Format("%i",iRes);
}

CString WKDISP_Screenshot(CStringArray& aParams)
{
	CString sPath;
	if(aParams.GetSize()>0){
		sPath=aParams[0];
	}
	UINT M=0;
	if(aParams.GetSize()>1){
		M=(UINT)atolx(aParams[1]);
	}
	SaveScreenshotToFile(sPath, M);
	/*{
		// Выходное изображение
		CBitmap* bmpResult=NULL;
		CBitmap* pOld=NULL;
		CBitmap bmp;
		// Вспомогательный DC
		CDC pSHDC;
		pSHDC.CreateCompatibleDC(&dcScreen);
		pSHDC.SetStretchBltMode(COLORONCOLOR);
		BOOL bResized=FALSE;
		{// Маштабируем качественно или не маштабируем
			CBitmap bmp2;
			bmp2.CreateCompatibleBitmap(&dcScreen,rDesktopRECT.Width(),rDesktopRECT.Height());
			CDC pSHDC2;
			pSHDC2.CreateCompatibleDC(&dcScreen);
			CBitmap* pOld2=pSHDC2.SelectObject(&bmp2);
			pSHDC2.BitBlt(0, 0, rDesktopRECT.Width(),rDesktopRECT.Height(),&dcScreen, rDesktopRECT.left, rDesktopRECT.top, SRCCOPY|xCAPTUREBLT);
			if(wndShotPhase2){
				delete wndShotPhase2;
				wndShotPhase2=0;
			}
			if(objSettings.lAddCursorToScreenshot){
				CPoint pt;
				::GetCursorPos(&pt);
				HCURSOR hCur=::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
				ICONINFO inf;
				GetIconInfo(hCur,&inf);
				::DrawIconEx(pSHDC2,pt.x-rDesktopRECT.left-inf.xHotspot,pt.y-rDesktopRECT.top-inf.yHotspot,hCur,0,0,0,NULL,DI_DEFAULTSIZE|DI_NORMAL);
			}
			HBITMAP pOutBmp=NULL;
			bResized=CopyBitmapToBitmap(bmp2,pOutBmp,rDesktopRECT.Size(),&(rScrSize.Size()),bNeedQualityResizing?TRUE:FALSE);
			pSHDC2.SelectObject(pOld2);
			// Подготавливаем дальше...
			bmpResult=new CBitmap();
			bmpResult->Attach(pOutBmp);
			pOld=pSHDC.SelectObject(bmpResult);
		}
		// Подготавливаем к сохранению
		CString sPath=(lScrshotAction==0?objSettings.sScrshotDir:(objSettings.sScrshotDir2!=""?objSettings.sScrshotDir2:objSettings.sScrshotDir));
		AddSlashToPath(sPath);
		CString sFile=objSettings.sScrshotFile;
		CString sSign=objSettings.sSignString;
		SubstMyVariablesWK(sPath);
		SubstMyVariablesWK(sFile);
		SubstMyVariablesWK(sSign);

		SubstMyVariablesForScreenshot(sFile,sNewTitle);
		SubstMyVariablesForScreenshot(sSign,sNewTitle);
		SubstMyVariablesForScreenshot(sPath,sNewTitle);
		if(bResized && bmpResult){
			if(objSettings.bBlackAndWhiteConfirm==1){
				dcScreen.InvertRect(&rDesktopRECT);
			}
			DWORD dwStart=GetTickCount();
			MakeSafeFileName(sFile);
			// Добавляем мерзкую подпись для незарегистрированных чайников...
			if(objSettings.iLikStatus!=2 && GetWindowDaysFromInstall()>UNREGDAY_SCR){
				objSettings.bSignSShot=TRUE;
				sSign=CString("["PROGNAME": ")+_l("Unregistered version")+". "+_l("Please register")+"!]";
			}
			if(objSettings.bSignSShot){
				CRect rt;
				rt=rScrSize;
				rt.InflateRect(-20,-20,-20,-20);
				SmartDrawText(pSHDC, sSign, rt, NULL, RGB(0,0,0), RGB(250,250,250), TXSHADOW_GLOW);
			}
			//
			CString sPictureFile=sPath+sFile;
			BOOL bSavedOk=SaveBitmapToFile(sPictureFile,bmpResult);
			if(!objSettings.lScreenshotByTimer){
				if(objSettings.bPutScreshotInClipboard && bmpResult){
					BITMAP bmpInfo;
					bmpInfo.bmBits=NULL;
					bmpResult->GetBitmap(&bmpInfo);
					CBitmap bmpNew;
					bmpNew.CreateBitmapIndirect(&bmpInfo);
					DWORD dwCount=bmpInfo.bmHeight*bmpInfo.bmWidth*bmpInfo.bmPlanes*bmpInfo.bmBitsPixel;//bmpInfo.bmWidthBytes
					BYTE* lpBits=new BYTE[dwCount];
					bmpResult->GetBitmapBits(dwCount, lpBits);
					bmpNew.SetBitmapBits(dwCount, lpBits);
					CopyBitmapToClipboard(this,bmpNew,NULL);
					delete[] lpBits;
				}
			}
			if(objSettings.bBlackAndWhiteConfirm==1){
				DWORD dwStop=GetTickCount();
				if(dwStop-dwStart<150){
					Sleep((dwStart+150)-dwStop);
				}
				dcScreen.InvertRect(&rDesktopRECT);
			}
			if(bSavedOk){
				if(!objSettings.lScreenshotByTimer){
					if(objSettings.bBlackAndWhiteConfirm==2){
						Alert(_l("Screenshot successfully saved to")+"\n"+TrimMessage(sPictureFile,60,3),5000,FALSE,DT_CENTER|DT_VCENTER);
					}
				}
				objSettings.sLastScreenshot=sPictureFile;
				objSettings.iGlobalScrShotCount++;
				AsyncPlaySound(TAKE_SSHOT);
				if(!objSettings.lScreenshotByTimer){
					if(objSettings.bAutoEditOnShot==3 || (objSettings.bAutoEditOnShot==1 && lScrshotAction==0) || (objSettings.bAutoEditOnShot==2 && lScrshotAction!=0)){
						CallBMPEdit(sPictureFile);
					}
				}
			}
		}
		if(pOld){
			pSHDC.SelectObject(pOld);
		}
		if(!objSettings.lScreenshotByTimer){
			lSeqScreenshowInProgress=0;
		}
		if(lSeqScreenshowInProgress){
			if(objSettings.lScreenshotByTimerSec<5){
				objSettings.lScreenshotByTimerSec=5;
			}
			Sleep(objSettings.lScreenshotByTimerSec*1000);
		}
		delete bmpResult;
	}*/
	return "1";
}

CString WKDISP_IsVisible(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	int iRes=::IsWindowVisible(H);
	return iRes?"1":"0";
}

CString WKDISP_SetVisible(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	UINT M=1;
	if(aParams.GetSize()>1){
		M=(UINT)atolx(aParams[1]);
	}
	int iRes=::ShowWindowAsync(H,M?SW_SHOW:SW_HIDE);
	return Format("%i",iRes);
}

CString WKDISP_IsEnabled(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	int iRes=::IsWindowEnabled(H);
	return iRes?"1":"0";
}

CString WKDISP_PostM(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	UINT M=0;
	if(aParams.GetSize()>1){
		M=(UINT)atolx(aParams[1]);
	}
	WPARAM W=0;
	if(aParams.GetSize()>2){
		W=(WPARAM)atolx(aParams[2]);
	}
	LPARAM L=0;
	if(aParams.GetSize()>3){
		L=(LPARAM)atolx(aParams[3]);
	}
	int iRes=::PostMessage(H,M,W,L);
	return Format("%i",iRes);
}

CString WKDISP_SendM(CStringArray& aParams)
{
	HWND H=0;
	if(aParams.GetSize()>0){
		H=(HWND)atolx(aParams[0]);
	}
	UINT M=0;
	if(aParams.GetSize()>1){
		M=(UINT)atolx(aParams[1]);
	}
	WPARAM W=0;
	if(aParams.GetSize()>2){
		W=(WPARAM)atolx(aParams[2]);
	}
	LPARAM L=0;
	if(aParams.GetSize()>3){
		L=(LPARAM)atolx(aParams[3]);
	}
	int iRes=::SendMessage(H,M,W,L);
	return Format("%i",iRes);
}

CString WKDISP_SetC(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	if(sP1.GetLength()>0){
		if(sP2.GetLength()==0){
			getMCookies().RemoveKey(sP1);
		}else{
			getMCookies().SetAt(sP1,sP2);
		}
	}
	return "";
}

CString WKDISP_GetC(CStringArray& aParams)
{
  	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(sP1.GetLength()>0){
		return getCookie(sP1);
	}
	return "";
}

CString WKDISP_CallM(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(sP1.GetLength()>0){
		ExecuteMacroByString(sP1);
	}
	return "";
}

CString WKDISP_WWAKAR(CStringArray& aParams)
{
	WaitWhileAllKeysAreFree();
	return "";
}

CString WKDISP_OSD(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	if(sP1.GetLength()>0){
		if(objSettings.lNoNeedOSD){
			return "";
		}
		static long lNoRecurse=0;
		if(lNoRecurse){
			return "";
		}
		static CSplashWindow* pOSD=0;
		SimpleTracker lc(lNoRecurse);
		if(pOSD){
			pOSD->SetTextOnly(sP1);
			pOSD->wnd->Invalidate();
		}else{
			CSplashParams par;
			FillSplashParams(sP1,par);
			par.szText=sP1;
			pOSD=new CSplashWindow(&par);
		}
		DWORD dwTimeToShow=atol(sP2);
		if(dwTimeToShow==0){
			dwTimeToShow=9000;
		}
		pOSD->AutoClose(dwTimeToShow,&pOSD);
	}
	return "";
}

CString WKDISP_Baloon(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	CString sP3;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	if(aParams.GetSize()>2){
		sP3=aParams[2];
	}
	DWORD dwP3=atol(sP3);
	if(dwP3==0){
		dwP3=9000;
	}
	if(sP1.GetLength()>0){
		ShowBaloon(sP1,sP2,dwP3);
	}
	return "";
}

CString WKDISP_GetPixel(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	CDesktopDC dc;
	DWORD dwP3=dc->GetPixel(atol(sP1),atol(sP2));
	return Format("#%02X%02X%02X",GetRValue(dwP3),GetGValue(dwP3),GetBValue(dwP3));
}

CString WKDISP_SetOption(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
		sP1.MakeLower();
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	if(sP1=="screensaverautostartfile"){
		objSettings.sAppToRWS=sP2;
	}
	if(sP1=="screensaverautostopfile"){
		objSettings.lStopAppToRWS=atol(sP2);
	}
	return "0";
}

CString WKDISP_AskFile(CStringArray& aParams)
{
  	CString sP1;
  	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	CString sFile="";
	CFakeWindow fakeWnd(sP1==""?_l("Browse for file"):sP1);
	CFileDialog dlg(TRUE, NULL, NULL, OFN_NODEREFERENCELINKS, sP2==""?"All files (*.*)|*.*||":sP2, fakeWnd.GetWnd());
	if(dlg.DoModal()==IDOK){
		return dlg.GetPathName();
	}
	return "";
}

CString WKDISP_AskFolder(CStringArray& aParams)
{
	CString sP1;
	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	char szDir[MAX_PATH]="c:\\";
	if(sP2!=""){
		strcpy(szDir,sP2);
	}
	CFakeWindow fakeWnd(sP1==""?_l("Browse for folder"):sP1);
	if(GetFolder(_l("Choose directory"), szDir, NULL, fakeWnd.GetHwnd())){//GetAppWnd()->GetSafeHwnd()
		return szDir;
	}
	return "";
}

CString WKDISP_GetCPixel(CStringArray& aParams)
{
  	CPoint pt;
	GetCursorPos(&pt);
	CDesktopDC dc;
	DWORD dwP3=dc->GetPixel(pt);
	return Format("#%02X%02X%02X",GetRValue(dwP3),GetGValue(dwP3),GetBValue(dwP3));
}

CString WKDISP_GetCursorPosX(CStringArray& aParams)
{
  	CPoint pt;
	GetCursorPos(&pt);
	return Format("%i",pt.x);
}

CString WKDISP_GetCursorPosY(CStringArray& aParams)
{
  	CPoint pt;
	GetCursorPos(&pt);
	return Format("%i",pt.y);
}

CPoint GetCaretScreenPos(BOOL bUseCashe);
CString WKDISP_GetCaretPosX(CStringArray& aParams)
{
  	CPoint pos=GetCaretScreenPos(1); 
	return Format("%i",pos.x);
}

CString WKDISP_GetCaretPosY(CStringArray& aParams)
{
  	CPoint pos=GetCaretScreenPos(1);
	return Format("%i",pos.y);
}

CString WKDISP_GetClipboard(CStringArray& aParams)
{
	return GetClipboardText(0,0);
}

int CallPluginFunction(CPlugin* plg, int iNum, WKPluginFunctionStuff* stuff);
CString WKDISP_BlockHK(CStringArray& aParams)
{
	CPlugin* plg=0;
	wk.isPluginLoaded("WP_KeyMaster", &plg);
	if(plg){
		WKPluginFunctionStuff stuff;
		stuff.wireKeysInt=(WKCallbackInterface*)(plg);
		CallPluginFunction(plg,2,&stuff);
		return "1";//WKIHOTKEYS_ActHotkeyByID("@PLUGIN-WP_KeyMaster-2");
	}else{
		return "0";
	}
}

CString WKDISP_BlockKB(CStringArray& aParams)
{
	CPlugin* plg=0;
	wk.isPluginLoaded("WP_KeyMaster", &plg);
	if(plg){
		WKPluginFunctionStuff stuff;
		stuff.wireKeysInt=(WKCallbackInterface*)(plg);
		CallPluginFunction(plg,3,&stuff);
		return "1";//WKIHOTKEYS_ActHotkeyByID("@PLUGIN-WP_KeyMaster-3");
	}else{
		return "0";
	}
}

CString WKDISP_fromUTF8(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	CString sP2;
	UTF2U(sP1,sP2);
	return sP2;
}

CString WKDISP_toUTF8(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	USES_CONVERSION;
	return EscapeStringUTF8(A2W(sP1));
}

CString WKDISP_fromEscape2(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}

	return UnescapeString(sP1);
}

CString WKDISP_toEscape2(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	return EscapeString2(sP1);
}

CString WKDISP_i18n(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	return _l(sP1);
}

CString WKDISP_IsAppRunning(CStringArray& aParams)
{
	CString sP1;
	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
		sP1.MakeLower();
	}
	CCSLock lpc(&pCahLock,1);
	EnumProcesses();
	for(int i=0;i<GetProccessCash().GetSize();i++){
		CProcInfo* pInf=&GetProccessCash()[i];
		CString sName=pInf?pInf->sProcExePath:"";
		sName.MakeLower();
		if(sName.Find(sP1)!=-1 && pMainDialogWindow){
			return "1";
		}
	}
	return "0";
}

CString WKDISP_CloseApp(CStringArray& aParams)
{
	CString sP1;
	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
		sP1.MakeLower();
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
		sP2.MakeLower();
	}
	CCSLock lpc(&pCahLock,1);
	EnumProcesses();
	for(int i=0;i<GetProccessCash().GetSize();i++){
		CProcInfo* pInf=&GetProccessCash()[i];
		CString sName=pInf?pInf->sProcExePath:"";
		sName.MakeLower();
		if(sName.Find(sP1)!=-1 && pMainDialogWindow){
			if(sP2=="1" || sP2=="true"){
				KillProcess(pInf->dwProcId,NULL);
			}else{
				pMainDialogWindow->PopupAppWindow(pInf->dwProcId,1);
			}
			return "1";
		}
	}
	return "0";
}

CString WKDISP_ActivateWndByClass(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	HWND hWnd1=::FindWindow(sP1,NULL);
	if(hWnd1){
		SwitchToWindow(hWnd1,1);
		return "1";
	}
	return "0";
}

CString WKDISP_ActivateWndByApp(CStringArray& aParams)
{
	CString sP1;
	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
		sP1.MakeLower();
	}
	CCSLock lpc(&pCahLock,1);
	EnumProcesses();
	for(int i=0;i<GetProccessCash().GetSize();i++){
		CProcInfo* pInf=&GetProccessCash()[i];
		CString sName=pInf?pInf->sProcExePath:"";
		sName.MakeLower();
		if(sName.Find(sP1)!=-1 && pMainDialogWindow){
			pMainDialogWindow->PopupAppWindow(pInf->dwProcId);
			return "1";
		}
	}
	return "0";
}

struct FindWindowByTitleParam
{
	FindWindowByTitleParam()
	{
		In=0;
		lCnt=0;
		hLastWindow=0;
		bDoSwitch=0;
	}
	BOOL bDoSwitch;
	CString* In;
	long lCnt;
	HWND hLastWindow;
};

BOOL CALLBACK FindWindowByTitle(HWND h, LPARAM l)
{
	FindWindowByTitleParam* Info=(FindWindowByTitleParam*)l;
	char sz[256]={0};
	::GetWindowText(h,sz,sizeof(sz));
	BOOL bOk=0;
	if(Info->In->Find('*')==-1 && Info->In->Find('?')==-1)
	{
		if(strstr(sz,*Info->In)!=NULL)
		{
			bOk=1;
			Info->hLastWindow=h;
			Info->lCnt++;
		}
	}else if(PatternMatch(sz,*Info->In)){
		bOk=1;
		Info->hLastWindow=h;
		Info->lCnt++;
	}
	if(bOk && Info->bDoSwitch)
	{
		SwitchToWindow(h,1);
	}
	return TRUE;
}

CString WKDISP_ActivateWndByTitle(CStringArray& aParams)
{
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
		sP1.MakeLower();
	}
	if(sP1.GetLength()==0){
		return "0";
	}
	FindWindowByTitleParam data;
	data.In=&sP1;
	data.bDoSwitch=1;
	EnumWindows(FindWindowByTitle,(LPARAM)&data);
	return data.lCnt>0?"1":"0";
}

CString WKDISP_Find(CStringArray& aParams)
{
	HWND hOut=0;
	CString sClass;
	CString sTitle;
	if(aParams.GetSize()>0){
		sClass=aParams[0];
	}
	if(aParams.GetSize()>1){
		sTitle=aParams[1];
	}
	if(sClass.GetLength()>0 || sTitle.GetLength()>0){
		//hOut=::FindWindow((?sClass:NULL),(sTitle.GetLength()>0?sTitle:NULL));
		const char* szC=0;
		if(sClass.GetLength()>0){
			szC=sClass;
		}
		const char* szT=0;
		if(sTitle.GetLength()>0){
			szT=sTitle;
		}
		hOut=::FindWindow(szC,szT);
	}
	return Format("0x%08X",hOut);
}

CString WKDISP_FindByTitle(CStringArray& aParams)
{
	HWND hOut=0;
	CString sClass;
	if(aParams.GetSize()>0){
		sClass=aParams[0];
	}
	FindWindowByTitleParam data;
	data.In=&sClass;
	EnumWindows(FindWindowByTitle,(LPARAM)&data);
	hOut=data.hLastWindow;
	return Format("0x%08X",hOut);
}

CString WKDISP_ActivateWndByAll(CStringArray& aParams)
{
	HWND hWnd1=0;
	CString sP1;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(sP1.GetLength()>2 && sP1[0]=='0' && sP1[1]=='x'){
		hWnd1=(HWND)atolx(sP1);
	}
	if(!hWnd1){
		if(WKDISP_ActivateWndByClass(aParams)=="1"){
			return "1";
		}
		if(WKDISP_ActivateWndByApp(aParams)=="1"){
			return "1";
		}
		hWnd1=::FindWindow(NULL,sP1);
	}
	if(hWnd1){
		SwitchToWindow(hWnd1,1);
		return "1";
	}
	return WKDISP_ActivateWndByTitle(aParams);
}

CString WKDISP_ShellExec(CStringArray& aParams)
{
	CString sP1;
	CString sP2;
	if(aParams.GetSize()>0){
		sP1=aParams[0];
	}
	if(aParams.GetSize()>1){
		sP2=aParams[1];
	}
	if(sP2==""){
		sP2="1";
	}
	if(sP1!=""){
		WinExec(sP1,atol(sP2));
	}
	return "1";
}

CString WKDISP_BAtPos(CStringArray& aParams)
{
	CStringArray* p=new CStringArray();
	p->Append(aParams);
	pMainDialogWindow->PostMessage(SHOWCURBALOON,0,(LPARAM)p);
	return "";
}

BOOL PauseAllWK(BOOL bPause);
CString WKDISP_PasteClip(CStringArray& aParams)
{
	PauseAllWK(1);
	Sleep(5);
	::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),0,0);
	Sleep(5);
	::keybd_event('V', ::MapVirtualKey('V',0),0,0);
	Sleep(5);
	::keybd_event('V', ::MapVirtualKey('V',0),KEYEVENTF_KEYUP,0);
	Sleep(5);
	::keybd_event(VK_CONTROL, ::MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
	Sleep(5);
	PauseAllWK(0);
	return "";
}

CString WKDISP_PasteClip2(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		int iClip=atol(aParams[0]);
		SimpleLocker l(&csClipBuffer);
		if(iClip>=0 && iClip<objSettings.sClipBuffer.GetSize() && objSettings.sClipBuffer[iClip].GetLength()!=0){_XLOG_
			SimpleLocker l(&csClipBuffer);
			pMainDialogWindow->PostMessage(WM_COMMAND,PASTETEXT,(LPARAM)strdup(objSettings.sClipBuffer[iClip]));
		}
	}
	return "";
}

CString WKDISP_PauseWK(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		PauseAllWK((aParams[0].CompareNoCase("true")==0)?1:atol(aParams[0]));
	}
	return "";
}

CString WKDISP_ETrash(CStringArray& aParams)
{
	SHEmptyRecycleBin((pMainDialogWindow==0)?0:pMainDialogWindow->GetSafeHwnd(),(aParams.GetSize()==0)?0:(const char*)aParams[0],SHERB_NOCONFIRMATION);
	return "";
}

CString WKDISP_OpenCD(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		OpenCD(aParams[0].GetAt(0),1);
	}
	return "";
}

CString WKDISP_CloseCD(CStringArray& aParams)
{
	if(aParams.GetSize()>0){
		CloseCD(aParams[0].GetAt(0),1);
	}
	return "";
}

CString WKDISP_LastTypedText(CStringArray& aParams)
{
	CPlugin* plg=0;
	wk.isPluginLoaded("wp_keylogger", &plg);
	if(plg){
		FPLUGINEXTFUNCTION fp=(FPLUGINEXTFUNCTION)DBG_GETPROC(plg->hInst,"LastTypedText");
		if(fp==0){
			return "";
		}
		char szBuffer[2048]={0};
		int iRes=fp("",szBuffer,sizeof(szBuffer));
		return szBuffer;
	}
	AfxMessageBox(_l("To get recently typed text you should activate AutoText plugin first!"));
	return "0";
}

CString WKDISP_ShowPopupMenu(CStringArray& aParams)
{
	if(aParams.GetSize()==1){
		CString sList=aParams[0];
		aParams.RemoveAll();
		ConvertToArray(sList,aParams,'|');
	}
	if(aParams.GetSize()>0){
		int iRes=SelectFromMenu(aParams, 0);
		return Format("%i",iRes);
	}
	return "";
}

CString WKDISP_SwResolution(CStringArray& aParams)
{
	int iP1;
	int iP2;
	if(aParams.GetSize()>0){
		iP1=atol(aParams[0]);
	}
	if(aParams.GetSize()>1){
		iP2=atol(aParams[1]);
	}
	int iMaxFr=0;
	int iCod=-1;
	for(int i=0;i<objSettings.videoModes.GetSize();i++){
		CVideoMode& vm=objSettings.videoModes[i];
		if((iP1==0 || vm.m_dwWidth==iP1) || (iP2==0 || vm.m_dwHeight==iP2)){
			if(vm.m_dwFrequency>iMaxFr){
				iMaxFr=vm.m_dwFrequency;
				iCod=i;
			}
		}
	}
	if(iCod>=0){
		pMainDialogWindow->SwitchResolution(iCod);
	}
	return "";
}

CString WKDISP_IsKeyPressed(CStringArray& aParams)
{
	UINT iKey=0;
	CString sName;
	if(aParams.GetSize()){
		sName=aParams[0];
		iKey=atol(sName);
		sName.MakeLower();
	}
	if(sName.Find("shift")){
		iKey=VK_SHIFT;
	}
	if(sName.Find("alt")){
		iKey=VK_MENU;
	}
	if(sName.Find("control")){
		iKey=VK_CONTROL;
	}
	if(sName.Find("lbutton")){
		iKey=VK_LBUTTON;
	}
	if(sName.Find("rbutton")){
		iKey=VK_RBUTTON;
	}
	if(sName.Find("mbutton")){
		iKey=VK_MBUTTON;
	}
	if(sName.Find("rwin")){
		iKey=VK_RWIN;
	}
	if(sName.Find("lwin")){
		iKey=VK_LWIN;
	}
	if(::GetAsyncKeyState(iKey)<0){
		return "1";
	}
	return "0";
}

#include <wininet.h>
CString WKDISP_StartInetConnection(CStringArray& aParams)
{
	CWebWorld ww;
	if(aParams.GetSize()>0 && ww.InternetDial){
		DWORD dwOutCNum=0;
		char szDialname[MAX_PATH]="";
		strcpy(szDialname,aParams[0]);
		DWORD dwErr=(*ww.InternetDial)((pMainDialogWindow==0)?0:pMainDialogWindow->GetSafeHwnd(), szDialname, INTERNET_DIAL_UNATTENDED, &dwOutCNum, 0);
		if(dwErr!=ERROR_SUCCESS){
			Alert(Format("%s '' (0x%0X)",_l("Failed to create internet connection"),dwErr));
		}
	}
	return "";
}

typedef struct cmdtable 
{
	char name[50];
	CString (*handler)(CStringArray& aParams);
} cmdtable; 

cmdtable WKDispatchMethods[] = {
	{ "calldll", &WKDISP_CallDLL },
	{ "callplugin", &WKDISP_CallPlug },
	{ "callaction", &WKDISP_CallM },
	{ "callmacro", &WKDISP_CallM },
	{ "getselectedtext", &WKDISP_GetSel },
	{ "getselection", &WKDISP_GetSel },
	{ "copy", &WKDISP_GetSel },
	{ "putselectedtext", &WKDISP_PutSel },
	{ "setselectedtext", &WKDISP_PutSel },
	{ "putselection", &WKDISP_PutSel },
	{ "setselection", &WKDISP_PutSel },
	{ "sendtext", &WKDISP_PutSel },
	{ "sendkeys", &WKDISP_PutSel },
	{ "paste", &WKDISP_PutSel },
	{ "pasteclip", &WKDISP_PasteClip2 },
	{ "pastewkclip", &WKDISP_PasteClip2 },
	{ "pasteclipboard", &WKDISP_PasteClip },
	{ "getcookie", &WKDISP_GetC },
	{ "putcookie", &WKDISP_SetC },
	{ "setcookie", &WKDISP_SetC },
	{ "sleep", &WKDISP_Sleep },
	{ "wait", &WKDISP_SleepW },
	{ "waitkeyrelease", &WKDISP_WWAKAR },
	{ "findwindow", &WKDISP_Find },
	{ "findwindowbytitle", &WKDISP_FindByTitle },
	{ "sendmessage", &WKDISP_SendM },
	{ "postmessage", &WKDISP_PostM },
	{ "messagebox", &WKDISP_MsgB },
	{ "showballoon", &WKDISP_Baloon },
	{ "showosd", &WKDISP_OSD },
	{ "alert", &WKDISP_Alert },
	{ "getpixel", &WKDISP_GetPixel },
	{ "getscreenpixel", &WKDISP_GetPixel },
	{ "getcursorpixel", &WKDISP_GetCPixel },
	{ "getcursorposx", &WKDISP_GetCursorPosX },
	{ "getcursorx", &WKDISP_GetCursorPosX },
	{ "getcursorposy", &WKDISP_GetCursorPosY },
	{ "getcursory", &WKDISP_GetCursorPosY },
	{ "getcaretx", &WKDISP_GetCaretPosX },
	{ "getcarety", &WKDISP_GetCaretPosY },
	{ "getclipboard", &WKDISP_GetClipboard },
	{ "getwindowfrompoint", &WKDISP_GetWndFromPoint },
	{ "getwindowundercursor", &WKDISP_GetWndFromCursor },
	{ "getwindowtext", &WKDISP_GetWndText },
	{ "getwindowclass", &WKDISP_GetWndClass },
	{ "testdosmask", &WKDISP_TestDosMask },
	{ "blockhotkey", &WKDISP_BlockHK },
	{ "blockkeyboard", &WKDISP_BlockKB },
	{ "pausewirekeys", &WKDISP_PauseWK },	
	{ "setwkoption", &WKDISP_SetOption },
	{ "askfolder", &WKDISP_AskFolder },
	{ "askfile", &WKDISP_AskFile },
	{ "activate", &WKDISP_ActivateWndByAll },
	{ "activatewin", &WKDISP_ActivateWndByAll },
	{ "activatewnd", &WKDISP_ActivateWndByAll },
	{ "activatewindow", &WKDISP_ActivateWndByAll },
	{ "activatewindowbyapp", &WKDISP_ActivateWndByApp },
	{ "activatewindowbyclass", &WKDISP_ActivateWndByClass },
	{ "activatewindowbytitle", &WKDISP_ActivateWndByTitle },
	{ "shellexec", &WKDISP_ShellExec },
	{ "isapprunning", &WKDISP_IsAppRunning },
	{ "closeapp", &WKDISP_CloseApp },
	{ "playrecord", &WKDISP_PlayRecord },
	{ "play", &WKDISP_PlaySound },
	{ "playsound", &WKDISP_PlaySound },
	{ "loadurl", &WKDISP_LoadUrl },
	{ "showballoonatposition", &WKDISP_BAtPos },
	{ "i18n", &WKDISP_i18n },
	{ "toutf8", &WKDISP_toUTF8 },
	{ "fromutf8", &WKDISP_fromUTF8 },
	{ "escape", &WKDISP_toEscape2 },
	{ "unescape", &WKDISP_fromEscape2 },
	{ "waitforwindow", &WKDISP_WaitForWindow },
	{ "enablewindow", &WKDISP_EnableWindow },
	{ "isenabled", &WKDISP_IsEnabled },
	{ "iswindowenabled", &WKDISP_IsEnabled },
	{ "isvisible", &WKDISP_IsVisible },
	{ "iswindowvisible", &WKDISP_IsVisible },
	{ "getenabled", &WKDISP_IsEnabled },
	{ "setvisible", &WKDISP_SetVisible },
	{ "iswindowenabled", &WKDISP_IsEnabled },
	{ "savescreenshot", &WKDISP_Screenshot },
	{ "ask", &WKDISP_Ask },
	{ "emptyrecyclebin", &WKDISP_ETrash },
	{ "opencd", &WKDISP_OpenCD },
	{ "closecd", &WKDISP_CloseCD },
	{ "startinetconnection", &WKDISP_StartInetConnection },
	{ "startinternetconnection", &WKDISP_StartInetConnection },
	{ "showpopupmenu", &WKDISP_ShowPopupMenu},
	{ "lasttypedtext", &WKDISP_LastTypedText},
	{ "setclipboard", &WKDISP_SetClipboard},
	{ "iskeypressed", &WKDISP_IsKeyPressed},
	{ "switchresolution", &WKDISP_SwResolution},
	{ "getforegroundwindow", &WKDISP_GetForeWindow},
	{ "setwindowposition", &WKDISP_SetWindowPosition},
	{ "setwindowsize", &WKDISP_SetWindowSize},
	{ "", NULL }
};

class CWKDispatch: public IDispatch
{
public:
	HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject)
	{
		if(IsEqualGUID(riid,IID_IUnknown)){
			*ppvObject=(IUnknown*)this;
			return S_OK;
		}
		if(IsEqualGUID(riid,IID_IDispatch)){
			*ppvObject=(IDispatch*)this;
			return S_OK;
		}
		*ppvObject=NULL;
		return E_NOINTERFACE;
	};
	
	ULONG STDMETHODCALLTYPE AddRef()
	{
		return 0;
	};
	
	ULONG STDMETHODCALLTYPE Release()
	{
		return 0;
	};
	
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ UINT *pctinfo)
	{
		*pctinfo=0;
		return E_NOTIMPL;
	}
        
    HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo **ppTInfo)
	{
		*ppTInfo=0;
		return E_NOTIMPL;
	}
        
    HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ REFIID riid,
		/* [size_is][in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ DISPID *rgDispId)
	{
		HRESULT hRes=NOERROR;
		for(UINT i=0;i<cNames;i++){
			CString sName(rgszNames[i]);
			sName.MakeLower();
			int j=0;
			while(WKDispatchMethods[j].handler){
				if(sName==WKDispatchMethods[j].name){
					rgDispId[i]=j+1;
					j=-1;
					break;
				}
				j++;
			}
			if(j!=-1){
				hRes=DISP_E_UNKNOWNNAME;
				break;
			}
		}
		return hRes;
	}
        
    HRESULT STDMETHODCALLTYPE Invoke( 
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr)
	{
		if ((wFlags & DISPATCH_METHOD) == 0){
			return ResultFromScode(E_INVALIDARG);
		}
		if(!IsEqualIID(riid, IID_NULL)){
			return ResultFromScode(DISP_E_UNKNOWNINTERFACE);
		}
		if(dispIdMember<1 || dispIdMember>sizeof(WKDispatchMethods)/sizeof(WKDispatchMethods[0])){
			return ResultFromScode(DISP_E_UNKNOWNINTERFACE);
		}
		VARIANT varResultDummy;
		if(pVarResult == NULL){
			pVarResult = &varResultDummy;
		}
		VariantInit(pVarResult);
		// Загоняем в массив строк, для простоты
		CStringArray aParameters;
		if(pDispParams){
			for(int i=pDispParams->cArgs-1;i>=0;i--){
				//DispGetParam??
				CString sElem=VariantToString(&pDispParams->rgvarg[i]);
				aParameters.Add(sElem);
			}
		}
		CString sResult=WKDispatchMethods[dispIdMember-1].handler(aParameters);
		pVarResult->vt=VT_BSTR;
		pVarResult->bstrVal=_com_util::ConvertStringToBSTR(sResult);
		//Alert(Format("Invoke!!! DispId=%i, Pars=%s",dispIdMember,sResult));
		return NOERROR;
	}
};

IDispatch* GetWKDispObject()
{
	static CWKDispatch wkdisp;
	return &wkdisp;
}

#define DISPOBJNAME		"WireKeys"
#define DISPOBJNAME2	"WK"
void InitMSScriptSite(MSScriptControl::IScriptControlPtr& ScriptEngine, const char* szLang, BOOL bAddObj)
{
	ScriptEngine.CreateInstance("MSScriptControl.ScriptControl"); 
	ScriptEngine->Language=szLang;
	ScriptEngine->Timeout = -1;//No timeout
	if(bAddObj){
		{
			_bstr_t wkOName(DISPOBJNAME);
			VARIANT_BOOL bMembers(FALSE);
			ScriptEngine->AddObject(wkOName,GetWKDispObject(), bMembers);
		}
		{
			_bstr_t wkOName(DISPOBJNAME2);
			VARIANT_BOOL bMembers(FALSE);
			ScriptEngine->AddObject(wkOName,GetWKDispObject(), bMembers);
		}
	}
}

//CCriticalSection cs;
typedef BOOL (WINAPI *_FPCALLMACRO)(const char* szMacros, int iEntryPoint, WKCallbackInterface* pCallback, HGLOBAL& hOut);
BOOL CalculateExpression_Ole(CString& sExpr, CString sLang, BOOL bInSilence, int iEntryPoint, HANDLE h)
{
	//cs.Lock();
	int iRes=0;
	::CoInitialize(NULL);
	BOOL bJScriptLang=(sLang.CompareNoCase("JScript")==0 || sLang=="");
	BOOL bVBScriptLang=(sLang.CompareNoCase("VBScript")==0);
#if _DEBUG
	CString sDebugFile="// -> ";
	sDebugFile+=sLang;
	if(bJScriptLang)
		sDebugFile+=" ,js";
	if(bVBScriptLang)
		sDebugFile+=" ,vbs";
	sDebugFile+="\r\n";
	sDebugFile+=sExpr;
	SaveFile("c:\\lastmacro.txt",sDebugFile);
#endif
	if(bJScriptLang || bVBScriptLang)
	{// Создаем область видимости чтобы разрушение ком-объекта было нормальным
		BOOL bAddObj=(sExpr.Find(DISPOBJNAME".")!=-1);
		MSScriptControl::IScriptControlPtr ScriptEngine;
		if(h){
			CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
			objSettings.aQRThreads.SetAt(h,&ScriptEngine);
		}
		InitMSScriptSite(ScriptEngine,(bJScriptLang?"JScript":"VBScript"),bAddObj);
		try {
			ScriptEngine->PutState((enum MSScriptControl::ScriptControlStates)0);
			_variant_t result;
			if(!bJScriptLang){
				sExpr=CString("Dim OUTPUT\r\n")+sExpr;
				ScriptEngine->ExecuteStatement(bstr_t(sExpr));
				sExpr="OUTPUT";
			}
			result=ScriptEngine->Eval(bstr_t(sExpr));
			sExpr=(const char*)_bstr_t(result);
			iRes=1;
		}catch(_com_error err){
			if(!bInSilence){
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
			}
			iRes=-1;
		};
		FreeInvokedDlls();
	}else
	{// Загружаем дллку
		iRes=FALSE;
		HINSTANCE hDLL=LoadLibrary(CString(GetPluginsDirectory())+sLang);
		if(hDLL){
			_FPCALLMACRO fp=(_FPCALLMACRO)DBG_GETPROC(hDLL,"ExecuteMacro");
			if(fp){
				HGLOBAL hOut=NULL;
				CPlugin* pCBI=0;
				int iRDot=sLang.ReverseFind('.');
				if(iRDot!=-1){
					sLang=sLang.Left(iRDot);
				}
				wk.isPluginLoaded(sLang,&pCBI,FALSE);
				iRes=(*fp)((const char*)sExpr,iEntryPoint,pCBI,hOut);
				if(hOut){
					sExpr=(char*)::GlobalLock(hOut);
					::GlobalUnlock(hOut);
					::GlobalFree(hOut);
				}else{
					sExpr="";
				}
			}
			FreeLibrary(hDLL);
		}else{
			Alert(_l("Can`t find component to play this macro")+"!");
		}
	}
	::CoUninitialize();
	//cs.Unlock();
	return iRes;
}


