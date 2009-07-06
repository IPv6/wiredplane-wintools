// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include "WP_MacroRec.h"
#include "HookCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND hMacroRecControlWindow;
WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

HHOOK g_hhook=NULL;
COptions plgOptions;
WKCallbackInterface* pInter=NULL;
extern HWND hwndDlgBeforeActive;
int CALLBACK COptions(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int	WINAPI WKPluginStart()
{
	if(WKGetPluginContainer()){
		// Version...
		char szVer[32]="";
		WKGetPluginContainer()->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			WKGetPluginContainer()->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
	pInter=WKGetPluginContainer();
	g_hhook=NULL;
	plgOptions.bRecPauses=1;
	plgOptions.lPauseLevel=1;
	plgOptions.bAppSwitchesRTyp=0;
	plgOptions.bAP_OpenInEd=1;
	plgOptions.bAP_Add2WK=1;
	plgOptions.lMFormat=0;
	return 1;
}

int	WINAPI WKPluginStop()
{
	return 1;
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction!=0 || stuff==NULL){
		return 0;
	}
	strcpy(stuff->szItemName,"Record new macro");
	stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
	stuff->hItemBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON));
	stuff->dwDefaultHotkey=0;
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction!=0 || stuff==NULL){
		return 0;
	}
	return 1;
}

extern BOOL bSkipPreparations;
CString _pl(const char* szText)
{
	if(bSkipPreparations){
		return szText;
	}
	char szOut[1024]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int	WINAPI WKInitRecording(HWND hParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(!pInter){
		AfxMessageBox("Container not found!!!");
		return 0;
	}
	if(hMacroRecControlWindow!=0){
		AfxMessageBox(_pl("To record new macro, You have to stop\nrecording previous macro"));
		return 0;
	}
	static long lMacroCounter=1;
	plgOptions.sLastMacroName="";
	COleDateTime dt=COleDateTime::GetCurrentTime();
	plgOptions.sLastMacroName.Format("%s (%02lu.%02lu.%04lu)",GetRandomName(),dt.GetDay(),dt.GetMonth(),dt.GetYear());
	lMacroCounter++;
	char szPrefFolder[MAX_PATH]="";
	pInter->GetPreferencesFolder(szPrefFolder,sizeof(szPrefFolder));
	CString sTargetFile=szPrefFolder;
	sTargetFile+="RecorderMacro\\";
	if(int(GetFileAttributes(sTargetFile))==-1){
		::CreateDirectory(sTargetFile,NULL);
	}
	plgOptions.sLastMacroContent="";
	DWORD dwStart=GetTickCount();
	ATOM at=GlobalAddAtom("WK_RECORDING");
	DWORD dwRes=DialogBox(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDD_OPTIONS),hParent,OptionsDialogProc);
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	GlobalDeleteAtom(at);
	if(dwRes!=0){
		// Пользователь отказался...
		return 0;
	}
	if(pInter){
		pInter->PushMyOptions(0);
	}
	if(plgOptions.sLastMacroContent==""){
		return 0;
	}
	CString sMacroFileNm=plgOptions.sLastMacroName;
	MakeSafeFileName(sMacroFileNm);
	sTargetFile+=sMacroFileNm;
	sTargetFile+=".wkm";
	// Добавлять иконку на время записи макроса
	// Префикс макроса...
	CString sMacroTemplate=	"// Language: %s\r\n"\
							"// Title: %s\r\n"\
							"// Description: Recorded macro\r\n"\
							"// ParamType: String\r\n"\
							"// Hint: wait_for_key_release;\r\n\r\n"\
							"%s"\
							"%s"\
							"\r\n"\
							"// <wkMain>: Action\r\n"\
							"function wkMain()\r\n"\
							"{// wkMain description: Replay keystrokes\r\n"\
							"%s"\
							"};\r\n"\
							"// </wkMain>: End of Action\r\n"\
							"\r\n";
	CString sDopFunc=		(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3)?"":"// System object that will be used to replay recorded keystrokes\r\nvar WshShell = new ActiveXObject('WScript.Shell');\r\n";
	sDopFunc+=				"// Playback Speed ratio ('1' means as recorded)\r\n";
	sDopFunc+=				"var iMacroSpeed = 1.0;";
	sDopFunc+=				"\r\n";
	CString sWaitFunc=		(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3)?"":"function Wait(time)\r\n"\
							"{\r\n"\
							"	var d = new Date();\r\n"\
							"	var timeCounter = 0;\r\n"\
							"	while((new Date()).getSeconds()-d.getSeconds()<(time*iMacroSpeed)){\r\n"\
							"		timeCounter++;\r\n"\
							"	};\r\n"\
							"}\r\n";
	plgOptions.sLastMacroContent=Format(sMacroTemplate,(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3)?"JScript":"JScript",plgOptions.sLastMacroName,sDopFunc,(plgOptions.bRecPauses?sWaitFunc:""),plgOptions.sLastMacroContent);
	DWORD dwStop=GetTickCount();
	CString sTime;
	sTime.Format("%.2f",double(dwStop-dwStart)/1000);
	int iRes=::MessageBox(NULL,_pl("Macro recorded")+"!\n"+_pl("Duration")+": "+sTime+_pl("sec")+". "+_pl("Continue")+"?",_pl("Macro successfully recorded")+"!",IDOK);
	if(iRes==IDOK){
		SaveFile(sTargetFile,plgOptions.sLastMacroContent);
		if(plgOptions.bAP_Add2WK && pInter){
			pInter->AddQrunFromFile(sTargetFile);
		}
		if(plgOptions.bAP_OpenInEd && pInter){
			int iRes=(int)::ShellExecute(NULL,"open","notepad.exe",CString("\"")+sTargetFile+CString("\""),NULL,SW_SHOWNORMAL);
		}
	}
	return 1;
}

int	WINAPI RecordMacro(BOOL bJScript, HWND hParent, const char* szFileOut)
{
	__FLOWLOG
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleDateTime dt=COleDateTime::GetCurrentTime();
	if(dt.GetMonth()!=4){
		AfxMessageBox("This is demo version of macro recorder.\nContact WiredPlane.com for full version");
		return 0;
	}
	if(hMacroRecControlWindow!=0){
		AfxMessageBox("To record new macro, You have to stop\nrecording previous macro");
		return 0;
	}
	hwndDlgBeforeActive=GetForegroundWindow();
	static long lMacroCounter=1;
	plgOptions.sLastMacroContent="";
	DWORD dwStart=GetTickCount();
	plgOptions.lMFormat=bJScript?2:0;
	plgOptions.bRecPauses=1;
	plgOptions.bAppSwitchesRTyp=0;
	plgOptions.lPauseLevel=1;
	plgOptions.bAppSwitchesRTyp=0;
	bSkipPreparations=1;
	ATOM at=GlobalAddAtom("WK_RECORDING");
	{
		__FLOWLOG
		DWORD dwRes=DialogBox(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDD_OPTIONS),hParent,OptionsDialogProc);
	}
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	GlobalDeleteAtom(at);
	if(plgOptions.sLastMacroContent==""){
		return 0;
	}
	CString sMacroTemplate=	"// Language: %s\r\n"\
							"// Title: %s\r\n"\
							"// Description: Recorded macro\r\n"\
							"// ParamType: String\r\n\r\n"\
							"%s"\
							"%s"\
							"\r\n"\
							"// <wkMain>: Action\r\n"\
							"function wkMain()\r\n"\
							"{// wkMain description: Replay keystrokes\r\n"\
							"%s"\
							"};\r\n"\
							"// </wkMain>: End of Action\r\n"\
							"\r\n";
	CString sDopFunc=		(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3)?"":"// System object that will be used to replay recorded keystrokes\r\nvar WshShell = new ActiveXObject('WScript.Shell');\r\n";
	sDopFunc+=				"// Playback Speed ratio ('1' means as recorded)\r\n";
	sDopFunc+=				"var iMacroSpeed = 1.0;";
	sDopFunc+=				"\r\n";
	CString sWaitFunc=		(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3)?"":"function Wait(time)\r\n"\
							"{\r\n"\
							"	var d = new Date();\r\n"\
							"	var timeCounter = 0;\r\n"\
							"	while((new Date()).getSeconds()-d.getSeconds()<(time*iMacroSpeed)){\r\n"\
							"		timeCounter++;\r\n"\
							"	};\r\n"\
							"}\r\n";
	plgOptions.sLastMacroContent=Format(sMacroTemplate,(plgOptions.lMFormat==0 || plgOptions.lMFormat==1 || plgOptions.lMFormat==3)?"JScript":"JScript",plgOptions.sLastMacroName,sDopFunc,(plgOptions.bRecPauses?sWaitFunc:""),plgOptions.sLastMacroContent);
	DWORD dwStop=GetTickCount();
	SaveFile(szFileOut,plgOptions.sLastMacroContent);
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	hwndDlgBeforeActive=stuff->hForegroundWindow;
	WKInitRecording(stuff->hMainWKWindow);
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=FALSE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Macro recorder plugin");
	strcpy(dsc->szDesk,"This plugin allows you to record keystrokes into macro. You can hold different keystrokes for different applications in one file");
	dsc->hPluginBmp=::LoadBitmap(WKGetPluginContainer()->GetPluginDllInstance(),MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}

int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddListOption("ActWnds","Recording: How to replay switches between applications during macro recording","Here you can define how to record application switches, for example you can define to record keystrokes separately",_pl("Ignore switches")+"\t"+_pl("Switch active application during macro replay as recorded")+"\t"+_pl("Replay keystrokes for active application only"),0,0);
		pOptionsCallback->AddBoolOption("RecordPauses","Recording: record pauses between keystrokes too","",0,0);
		pOptionsCallback->AddListOption("MFormat","Record format","Internal format is more powerful - can record all keys and mouse actions, JScript-SendKeys format can record keystrokes only but recorded macro can be used without WireKeys at all",_pl("Internal format")+"\t"+_pl("JScript-SendKeys"),0,0);
		pOptionsCallback->AddNumberOption("PauseLevel","Recording: record pauses between keystrokes that are bigger than (seconds)","",3,0,100,0);
		pOptionsCallback->AddBoolOption("OpenInEditor","After recording: open macro in text editor","",0,0);
		pOptionsCallback->AddBoolOption("Add2WK","After recording: add macro to WireKeys","",TRUE,0);
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		plgOptions.bRecPauses=pOptionsCallback->GetBoolOption("RecordPauses");
		plgOptions.lPauseLevel=pOptionsCallback->GetNumberOption("PauseLevel");
		plgOptions.bAppSwitchesRTyp=pOptionsCallback->GetListOption("ActWnds");
		plgOptions.bAP_OpenInEd=pOptionsCallback->GetBoolOption("OpenInEditor");
		plgOptions.bAP_Add2WK=pOptionsCallback->GetBoolOption("Add2WK");
		plgOptions.lMFormat=pOptionsCallback->GetListOption("MFormat");
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("RecordPauses",plgOptions.bRecPauses);
		pOptionsCallback->SetNumberOption("PauseLevel",plgOptions.lPauseLevel);
		pOptionsCallback->SetListOption("ActWnds",plgOptions.bAppSwitchesRTyp);
		pOptionsCallback->SetBoolOption("OpenInEditor",plgOptions.bAP_OpenInEd);
		pOptionsCallback->SetBoolOption("Add2WK",plgOptions.bAP_Add2WK);
		pOptionsCallback->SetListOption("MFormat",plgOptions.lMFormat);
	}
	return 1;
}

CCriticalSection cs;
#include "../msscript.cpp"
BOOL CalculateExpression_Ole(CString& sExpr, CString sLang, BOOL bInSilence, int iEntryPoint)
{
	cs.Lock();
	int iRes=0;
	::CoInitialize(NULL);
	BOOL bJScriptLang=(sLang=="JScript" || sLang=="");
	BOOL bVBScriptLang=(sLang=="VBScript");
	if(bJScriptLang || bVBScriptLang)
	{// Создаем область видимости чтобы разрушение ком-объекта было нормальным
		MSScriptControl::IScriptControlPtr ScriptEngine;
		ScriptEngine.CreateInstance("MSScriptControl.ScriptControl"); 
		if(bJScriptLang){
			ScriptEngine->Language="JScript";
		}else if(bVBScriptLang){
			ScriptEngine->Language="VBScript";
		}else{
			ScriptEngine->Language=(const char*)sLang;
		}
		ScriptEngine->Timeout = -1;//No timeout
		try {//RET_FAIL(spAS->AddNamedItem(OLESTR("MyObject"), SCRIPTITEM_ISVISIBLE | SCRIPTITEM_ISSOURCE), "IActiveScript::AddNamedItem()");
			_variant_t result;
			if(!bJScriptLang){
				sExpr=CString("Dim OUTPUT\r\n")+sExpr;
				ScriptEngine->ExecuteStatement(bstr_t(sExpr));
				sExpr="OUTPUT";
				result=ScriptEngine->Eval(bstr_t(sExpr));
			}else{
				result=ScriptEngine->Eval(bstr_t(sExpr));
			}
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
				AfxMessageBox(sErr);
			}
			iRes=-1;
		};
	}else
	{// Загружаем дллку
		iRes=FALSE;
	}
	::CoUninitialize();
	cs.Unlock();
	return iRes;
}

BOOL SwitchToWindow(HWND hWnd, BOOL bMoveFocus)
{
	if(!::IsWindow(hWnd)){
		return FALSE;
	}
	BOOL bCanReturnIfSwitched=0;
	if(GetForegroundWindow()!=hWnd){
		bCanReturnIfSwitched=1;
	}
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hWnd);
	if(bCanReturnIfSwitched && GetForegroundWindow()==hWnd){
		return TRUE;
	}
	BOOL bTopMost=::GetWindowLong(hWnd,GWL_EXSTYLE) & WS_EX_TOPMOST;
	BOOL bRes=FALSE;
	int iShowCmd=SW_SHOWNORMAL;
	if(::IsZoomed(hWnd)){
		iShowCmd=SW_SHOW;
	}
	HWND hFore=GetForegroundWindow();
	if(hFore){
		::SetWindowPos(hWnd,hFore,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|iShowCmd);
	}
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	::ShowWindow(hWnd,iShowCmd);
	bRes=::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|iShowCmd);
	bRes=::RedrawWindow(hWnd,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_FRAME) && bRes;
	if(!bTopMost){
		bRes=::SetWindowPos(hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW) && bRes;
	}
	if(bMoveFocus){
		::SetFocus(hWnd);
	}
	return bRes;
	/*if(getCNTWnWrap().m_hUser32Instance){
		if(!getCNTWnWrap().SwitchToWnd){
			getCNTWnWrap().SwitchToWnd=(FARPROCW)GetProcAddress(getCNTWnWrap().m_hUser32Instance,"SwitchToThisWindow");
		}
		if(getCNTWnWrap().SwitchToWnd){
			getCNTWnWrap().SwitchToWnd(hWnd, TRUE);
			return TRUE;
		}
	}
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	//::ShowWindow(hWnd,SW_SHOW);*/
	return FALSE;
}

CRect rtDesktop;
CString sSearchClass;
CString sSearchTitle;
HWND hEnumWndRes=0;
BOOL CALLBACK EnumFindWndBox(HWND hwnd,LPARAM lParam)
{
	char szClass[128]="";
	::GetClassName(hwnd,szClass,sizeof(szClass));
	char szTitle[128]="";
	::GetWindowText(hwnd,szTitle,sizeof(szTitle));
	BOOL b=1;
	if(sSearchClass!="" && sSearchClass!=szClass){
		b=0;
	}
	if(sSearchTitle!="" && sSearchTitle!=szTitle){
		b=0;
	}
	if(b==1){
		HWND* h=(HWND*)lParam;
		if(h){
			*h=hwnd;
		}
		return FALSE;
	}
	return TRUE;
}

int ParseLine(CString sLine, CString& sBody, CString& sResOut, int& iFrom, double dSpeed, WKCallbackInterface* pCallback)
{
	if(rtDesktop.bottom==0 && rtDesktop.right==0){
		GetWindowRect(GetDesktopWindow(),&rtDesktop);
	}
	CString sParams;
	sParams=CDataXMLSaver::GetInstringPartGreedly("('","')",sLine);
	if(sParams==""){
		sParams=CDataXMLSaver::GetInstringPartGreedly("(",")",sLine);
		if(sParams!=""){
			sLine=sLine.SpanExcluding("(");
		}
	}else{
		sLine=sLine.SpanExcluding("('");
	}
	if(sParams==""){
		sParams=CDataXMLSaver::GetInstringPartGreedly("\"","\"",sLine);
		if(sParams!=""){
			sLine=sLine.SpanExcluding("\"");
		}
	}
	if(sParams==""){
		sParams=CDataXMLSaver::GetInstringPartGreedly("'","'",sLine);
		if(sParams!=""){
			sLine=sLine.SpanExcluding("'");
		}
	}
	sLine.TrimLeft();
	sLine.TrimRight();
	sLine.MakeUpper();
	if(sLine!=""){
		if(sLine=="RETURN"){
			sResOut=sParams;
			sResOut.TrimLeft(" \"'\t");
			sResOut.TrimRight(" \"'\t");
			iFrom=-1;
			return 1;
		}
		if(sLine=="IF"){
			int iDelPos=sParams.Find("==");
			if(iDelPos!=-1){
				CString sLeft=sParams.Left(iDelPos);
				if(sLeft.Find("unescape")==0){
					sLeft=CDataXMLSaver::GetInstringPart("(",")",sLeft);
					sLeft=UnescapeString(sLeft);
				}
				CString sRight=sParams.Mid(iDelPos+2);
				if(sLeft!=sRight){
					iFrom=sBody.Find('}',iFrom);
				}
			}
		}
		if(sLine=="SET_SPEED"){
			double dSpeed=atof(sParams);
			if(dSpeed<0.1){
				dSpeed=1;
			}
		}
		if(sLine=="CALLACTION"){
			if(pCallback){
				pCallback->CallWKAction(atol(sParams));
			}
		}
		if(sLine=="CALLMACRO"){
			if(pCallback){
				pCallback->StartQuickRunByName(sParams,"");
			}
		}
		if(sLine=="WAIT"){
			Sleep(DWORD(atof(sParams)*1000*dSpeed));
		}
		if(sLine=="ACTIVATEWINDOW"){
			sParams.TrimLeft();
			sParams.TrimRight();
			sParams.TrimLeft("\"");
			sParams.TrimRight("\"");
			HWND hActivate=::FindWindow(NULL,sParams);
			if(hActivate){
				::SetForegroundWindow(hActivate);
				::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hActivate);
			}
		}
		if(sLine=="ALERT"){
			AfxMessageBox(sParams);
		}
		if(sLine=="WAITWHILEALLKEYSARERELEASED"){
			if(pCallback){
				pCallback->WaitWhileAllKeysAreReleased();
			}
		}
		if(sLine=="COMMAND"){
			HWND hCur=GetForegroundWindow();
#ifdef _DEBUG
			char szDebugTitle[128]={0};
			GetWindowText(hCur,szDebugTitle,128);
#endif
			CString sTargetByClass=CDataXMLSaver::GetInstringPart("targetByClass=[","]",sParams);
			CString sTargetByTitle=CDataXMLSaver::GetInstringPart("targetByTitle=[","]",sParams);
			if(sTargetByClass!="" || sTargetByTitle!=""){
				hCur=0;// Слать куда попало предназначенные спецклассу мессаги нельзя!
				// Walton Dell <WDELL@altera.com>
				HWND hTarget=::FindWindow((sTargetByClass!="")?LPCSTR(sTargetByClass):NULL,(sTargetByTitle!="")?LPCSTR(sTargetByTitle):NULL);
				if(hTarget){
					hCur=hTarget;
				}else{
					BOOL bInvis=atol(CDataXMLSaver::GetInstringPart("evenInvisible=[","]",sParams));
					if(bInvis){
						sSearchClass=sTargetByClass;
						sSearchTitle=sTargetByTitle;
						::EnumWindows(EnumFindWndBox,LPARAM(&hTarget));
						if(hTarget){
							hCur=hTarget;
						}
					}
				}
			}
			BOOL bUsedChildWindows=0;
			if(hCur){
				CString sCTargetByID=CDataXMLSaver::GetInstringPart("childTargetByID=[","]",sParams);
				if(sCTargetByID!=""){
					HWND hTarget=::GetDlgItem(hCur,atolx(sCTargetByID));
					if(hTarget){
						bUsedChildWindows=1;
						hCur=hTarget;
					}
				}
				CString sCTargetByClass=CDataXMLSaver::GetInstringPart("childTargetByClass=[","]",sParams);
				CString sCTargetByTitle=CDataXMLSaver::GetInstringPart("childTargetByTitle=[","]",sParams);
				if(sCTargetByClass!="" || sCTargetByClass!=""){
					HWND hTarget=::FindWindowEx(hCur,0,(sCTargetByClass!="")?LPCSTR(sCTargetByClass):NULL,(sCTargetByTitle!="")?LPCSTR(sCTargetByTitle):NULL);
					if(hTarget){
						bUsedChildWindows=1;
						hCur=hTarget;
					}
				}
			}
			{// Выбираем тип и выполняем код
				if(hCur && sParams.Find("activate::")!=-1){
					SwitchToWindow(hCur,TRUE);
				}
				if(sParams.Find("wnd::")!=-1){
					CString sX=CDataXMLSaver::GetInstringPart("x=[","]",sParams);
					long lX=atolx(sX);
					if(sX=="SCREEN_R"){
						lX=rtDesktop.right;
					}
					if(sX=="SCREEN_L"){
						lX=rtDesktop.left;
					}
					CString sY=CDataXMLSaver::GetInstringPart("y=[","]",sParams);
					long lY=atolx(sY);
					if(sY=="SCREEN_B"){
						lY=rtDesktop.bottom;
					}
					if(sY=="SCREEN_T"){
						lY=rtDesktop.top;
					}
					CString sRel=(CDataXMLSaver::GetInstringPart("rel=[","]",sParams));
					long lRel=atolx(sRel);
					sRel.MakeUpper();
					long lSz=atolx(CDataXMLSaver::GetInstringPart("savesize=[","]",sParams));
					long lRb=atolx(CDataXMLSaver::GetInstringPart("rightbottom=[","]",sParams));
					long lCn=atolx(CDataXMLSaver::GetInstringPart("center=[","]",sParams));
					if(hCur){
						HWND hOrif=hCur;
						CRect rt;
						if(!bUsedChildWindows){
							GetWindowTopParent(hCur);
							GetWindowRect(hCur,&rt);
						}else{
							GetClientRect(hCur,&rt);
						}
						CSize sz=rt.Size();
						if(sY=="NOCHANGE_B"){
							lY=rt.bottom;
						}
						if(sY=="NOCHANGE_T"){
							lY=rt.top;
						}
						if(sX=="NOCHANGE_L"){
							lX=rt.left;
						}
						if(sX=="NOCHANGE_R"){
							lX=rt.right;
						}
						/*if(sRel=="CURSOR"){
						CPoint pt;
						GetCursorPos(&pt);
						rt.OffsetRect(lX,lY);
					}*/
						if(lRel || sRel=="RELATIVE"){
							rt.OffsetRect(lX,lY);
						}else if(lSz){
							if(lCn){
								rt.left=(lX-sz.cx)/2;
								rt.top=(lY-sz.cy)/2;
								rt.right=rt.left+sz.cx;
								rt.bottom=rt.top+sz.cy;
							}else if(lRb){
								rt.OffsetRect(lX-rt.right,lY-rt.bottom);
							}else{
								rt.OffsetRect(lX-rt.left,lY-rt.top);
							}
						}else if(lRb){
							rt.right=lX;
							rt.bottom=lY;
						}else{
							rt.left=lX;
							rt.top=lY;
						}
						MoveWindow(hCur,rt.left,rt.top,rt.Width(),rt.Height(),TRUE);
					}
				}
				if(hCur && sParams.Find("cmd::")!=-1){
					DWORD dwMsg=atolx(CDataXMLSaver::GetInstringPart("msg=[","]",sParams));
					LPARAM lPar=atolx(CDataXMLSaver::GetInstringPart("l=[","]",sParams));
					WPARAM wPar=atolx(CDataXMLSaver::GetInstringPart("h=[","]",sParams));
					if(hCur){
						HWND hOrif=hCur;
						if(!bUsedChildWindows){
							GetWindowTopParent(hCur);
						}
						BOOL bRes=0; 
						bRes=::SendMessage(hOrif,dwMsg,wPar,lPar);
					}
				}
				if(sParams.Find("key::")!=-1){
					DWORD dwMsg=atolx(CDataXMLSaver::GetInstringPart("msg=[","]",sParams));
					CString sLow=CDataXMLSaver::GetInstringPart("l=[","]",sParams);
					LPARAM lPar=0;
					if(sLow.GetLength()==9 && sLow[0]=='#'){
						lPar=(hexCode(sLow.Mid(1,2))<<(8*3))+(hexCode(sLow.Mid(3,2))<<(8*2))+(hexCode(sLow.Mid(5,2))<<8)+hexCode(sLow.Mid(7,2));
					}else{
						lPar=atolx(sLow);
					}
					BOOL bKeyDown=atolx(CDataXMLSaver::GetInstringPart("keydown=[","]",sParams));
					DWORD dwVK=LOBYTE(lPar);
					DWORD dwScan=HIBYTE(lPar);
					BOOL bExtended=atolx(CDataXMLSaver::GetInstringPart("ext=[","]",sParams));;
					::keybd_event(BYTE(dwVK), BYTE(dwScan), (bKeyDown?0:KEYEVENTF_KEYUP)|(bExtended?KEYEVENTF_EXTENDEDKEY:0), 0);
				}
				if(sParams.Find("mou::")!=-1){
					DWORD dwMsg=atolx(CDataXMLSaver::GetInstringPart("msg=[","]",sParams));
					long dwX=atolx(CDataXMLSaver::GetInstringPart("x=[","]",sParams));
					long dwY=atolx(CDataXMLSaver::GetInstringPart("y=[","]",sParams));
					// Если все относительно, то колбасим...
					CString sRel=(CDataXMLSaver::GetInstringPart("rel=[","]",sParams));
					sRel.MakeUpper();
					int dwRel=atolx(sRel);
					if(dwRel>0 || sRel=="WINDOW" || sRel=="RELATIVE"){
						if(hCur){
							CRect rt;
							GetWindowRect(hCur,&rt);
							dwX+=rt.left;
							dwY+=rt.top;
						}else{
							// Относительные клики без активного окна скипаем нахрен!
							return 0;
						}
					}
					if(dwRel<0 || sRel=="CURSOR"){
						CPoint pt;
						GetCursorPos(&pt);
						dwX+=pt.x;
						dwY+=pt.y;
					}
					dwX=long(65536*double(dwX)/double(rtDesktop.Width()));
					dwY=long(65536*double(dwY)/double(rtDesktop.Height()));
					if(dwMsg!=WM_MOUSEMOVE){// Сначала двигаем
						::mouse_event(
							MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE
							,dwX
							,dwY
							,0,0);
					}
					if(dwMsg==WM_LBUTTONDOWN){
						dwMsg=MOUSEEVENTF_LEFTDOWN;
					}else if(dwMsg==WM_LBUTTONUP){
						dwMsg=MOUSEEVENTF_LEFTUP;
					}else if(dwMsg==WM_RBUTTONDOWN){
						dwMsg=MOUSEEVENTF_RIGHTDOWN;
					}else if(dwMsg==WM_RBUTTONUP){
						dwMsg=MOUSEEVENTF_RIGHTUP;
					}else if(dwMsg==WM_MBUTTONDOWN){
						dwMsg=MOUSEEVENTF_MIDDLEDOWN;
					}else if(dwMsg==WM_MBUTTONUP){
						dwMsg=MOUSEEVENTF_MIDDLEUP;
					}else{
						dwMsg=MOUSEEVENTF_MOVE;
					}
					::mouse_event(
						MOUSEEVENTF_ABSOLUTE|dwMsg
						,dwX
						,dwY
						,0,0);
				}
			}
		}
	}
	return 0;
}

BOOL WINAPI ExecuteMacro(const char* szMacros,int iEntryPoint,WKCallbackInterface* pCallback, HGLOBAL& hGlobal)
{
	hGlobal=NULL;
	CString sBody="\n";
	sBody+=szMacros;
	sBody.Replace("\r","");
	// Заворот для JScript
	CString sMacroLang=CDataXMLSaver::GetInstringPart("Language:","\n",sBody);
	sMacroLang.TrimLeft();
	sMacroLang.TrimRight();
	if(sMacroLang=="JScript" || sMacroLang=="VBScript" || sBody.Find("SendKeys")!=-1){
		sBody+="\r\nwkMain();";
		int iRes=CalculateExpression_Ole(sBody,sMacroLang,1,0);
		if(sBody!=""){
			int iLen=strlen(sBody)+1;
			hGlobal=::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, iLen);
			strcpy((char*)hGlobal,sBody);
		}
		return iRes;
	}
	// Выделяем нужный Entry point
	if(sBody.Find("<wkMain")!=-1){
		CString sFunctionToExtract="wkMain";
		if(iEntryPoint>0){
			sFunctionToExtract=Format("wkMain%i",iEntryPoint+1);
		}
		int iPos=sBody.Find(sFunctionToExtract);
		if(iPos!=-1){
			sBody=CDataXMLSaver::GetInstringPart(Format("<%s>",sFunctionToExtract),Format("</%s>",sFunctionToExtract),sBody);
		}
	}
	sBody+="\n";
	double dSpeed=1;
	// Погнали!
	int iFrom=0;
	//::SystemParametersInfo(SPI_GETWORKAREA,0,&DesktopDialogRect,0);
	int iBodyLen=strlen(sBody);
	CString sResOut="";
	while(iFrom!=-1){
		CString sLine=CDataXMLSaver::GetInstringPart("\n","\n",sBody,iFrom);
		if(iFrom!=-1 && iFrom<iBodyLen-1 && sBody[iFrom-1]=='\n'){
			iFrom--;
		}
		if(ParseLine(sLine, sBody, sResOut, iFrom, dSpeed, pCallback)){
			break;
		}
	}
	if(sResOut!=""){
		int iLen=strlen(sResOut)+1;
		hGlobal=::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, iLen);
		strcpy((char*)hGlobal,sResOut);
	}
	return TRUE;
}

void WaitWhileAllKeysAreFree()
{
	int iCount=0;
	BOOL bIgnoreDownOnly=0;
	while(1){
		BOOL bC=0;
		BOOL bS=0;
		BOOL bM=0;
		BOOL bLW=0;
		BOOL bRW=0;
		BOOL bI=0;
		BOOL bI2=0;
		BOOL bcC=0;
		BOOL bcV=0;
		bC= (GetAsyncKeyState(VK_CONTROL)<0);
		bS= (GetAsyncKeyState(VK_SHIFT)<0);
		bM= (GetAsyncKeyState(VK_MENU)<0);
		bLW=(GetAsyncKeyState(VK_LWIN)<0);
		bRW=(GetAsyncKeyState(VK_RWIN)<0);
		bcC=(GetAsyncKeyState('C')<0);
		bcV=(GetAsyncKeyState('V')<0);
		bI=0;//(GetAsyncKeyState(VK_INSERT)<0);
		if(!bC && !bS && !bM && !bLW && !bRW && !bI && !bcC && !bcV){
			break;
		}
		if(iCount>500){
			break;
		}
		iCount++;
		Sleep(50);
	}
}

BOOL WINAPI PlayMacro(const char* szMacroFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleDateTime dt=COleDateTime::GetCurrentTime();
	if(dt.GetMonth()!=4){
		AfxMessageBox("This is demo version of macro recorder.\nContact WiredPlane.com for full version");
		return 0;
	}
	WaitWhileAllKeysAreFree();
	CString szMacros;
	HGLOBAL hGlobal=0;
	ReadFile(szMacroFile,szMacros);
	BOOL bRes=ExecuteMacro(szMacros,0,0, hGlobal);
	return bRes;
}

int WINAPI RenderScript(const char* szParameters_in, char* szResult_out, size_t dwResultSize_in)
{
	if(szResult_out){
		*szResult_out=0;
	}
	int iFrom=0;
	CString sBody=szParameters_in,sRes;
	ParseLine(sBody,sBody,sRes,iFrom,1,0);
	return 0;
}

CWnd* AfxWPGetMainWnd()
{
	static CWnd wnd;
	static BOOL bInit=0;
	if(bInit==0){
		bInit=1;
		wnd.Attach(WKGetPluginContainer()->GetWKMainWindow());
	}
	return &wnd;
}