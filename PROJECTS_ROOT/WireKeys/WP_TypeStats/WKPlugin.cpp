// IconsFont.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <atlbase.h>
#include <math.h>
#include "resource.h"
#include "..\wkplugin.h"
#include "..\..\SmartWires\SystemUtils\Macros.h"
#include "..\..\SmartWires\SystemUtils\hotkeys.h"
//#define	_CMLHTDLL_NOLIB_
//#define	_CMLHTDLL_BUILDDLL_ 
#include "..\..\SmartWires\GUIClasses\FileDialogST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WKCallbackInterface*& WKGetPluginContainer()
{
	static WKCallbackInterface* pCallback=0;
	return pCallback;
}

CString _pl(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	return szOut;
}

CString _pl2(const char* szText)
{
	char szOut[128]="";
	WKGetPluginContainer()->GetTranslation(szText,szOut,sizeof(szOut));
	int i;
	for(i=strlen(szOut);i<10;i++){
		szOut[i]=' ';
	}
	szOut[i]=0;
	return szOut;
}

int WINAPI HandleSystemEvent(CWKSYSEvent* pEvent);
int	WINAPI WKPluginInit(WKCallbackInterface* init)
{
	if(init){
		// Version...
		char szVer[32]="";
		init->GetWKVersion(szVer);
		DWORD dwBuild=MAKEWORD(szVer[30],szVer[31]);
		if(dwBuild<348){
			init->ShowAlert("Sorry, this plugin can be used with WireKeys 3.4.8 and higher only","Plugin error");
			return 0;
		}
	}
	/*if(!init->getOption(WKOPT_EXTHKPROC)){
		init->ShowAlert("Sorry, you must enable extended hotkey processing\nin order to use this plugin. You can enable it\nin the preferences dialog","Plugin error");
		return 0;
	}*/
	WKGetPluginContainer()->SubmitToSystemEventNotification(WKSYSEVENTTYPE_CHAR, (FPONSYSTEMEVENT)HandleSystemEvent);
	WKGetPluginContainer()->SubmitToSystemEventNotification(WKSYSEVENTTYPE_KEYPRESS, (FPONSYSTEMEVENT)HandleSystemEvent);
	return 1;
}

int	WINAPI WKPluginStart()
{
	return 1;
}

int	WINAPI WKPluginStop()
{
	WKGetPluginContainer()->UnSubmitToSystemEventNotification(WKSYSEVENTTYPE_CHAR, (FPONSYSTEMEVENT)HandleSystemEvent);
	WKGetPluginContainer()->UnSubmitToSystemEventNotification(WKSYSEVENTTYPE_KEYPRESS, (FPONSYSTEMEVENT)HandleSystemEvent);
	WKGetPluginContainer()->PushMyOptions(0);
	return 1;
}


HINSTANCE g_hinstDll=NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	switch (fdwReason) {
		
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		g_hinstDll = hinstDll;
		break;
	case DLL_PROCESS_DETACH:
		// The calling process is detaching the DLL from its address space.
		break;
	case DLL_THREAD_ATTACH:
		// A new thread is being created in the current process.
		break;
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly.
		break;
	}
	return(TRUE);
}

int	WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff)
{
	if(iPluginFunction>=1 || stuff==NULL){
		return 0;
	}
	if(iPluginFunction==0){
		strcpy(stuff->szItemName,"Show/Hide statisctics");
		stuff->hItemBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
		stuff->dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
		stuff->dwDefaultHotkey=0;
	}
	return 1;
}

int	WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff)
{
	if(iPluginFunction>1 || stuff==NULL){
		return 0;
	}
	return 1;
}

int isAlphaChar(char c)
{
	if(c>='a' && c<='z'){
		return 1;
	}
	if(c>='A' && c<='Z'){
		return 1;
	}
	if(c>='0' && c<='9'){
		return 1;
	}
	if(UINT(c)>127){
		return 1;
	}
	return 0;
}

BOOL bWaitingForWord=1;
DWORD g_CharsPMTick=GetTickCount();
DWORD g_AllPMTick=GetTickCount();
DWORD g_AllPMTickPrev=GetTickCount();
long g_lLastpause=0;

long g_MaxPausePerMinute=2;
long lShowStat1=1;
long lShowStat2=1;
long lShowStat3=1;
long lShowStat4=1;
long lXPos=0;
long lYPos=0;
DWORD lBG=0;
DWORD lTX=65280;


class COptions
{
public:
	long g_CharsTotal;
	long g_DelsTotal;
	long g_NewLinesTotal;
	long g_SpaceTotal;
	long g_WordsTotal;
	long g_CharsPM;
	long g_CharsPMTime;
	long g_WordsPM;
	long g_Chars[256];
	long g_AritmiaCount;
	double g_Aritmia;
	void Reset()
	{
		memset(this,0,sizeof(COptions));
		g_Aritmia=0;
	}
	COptions(){
		Reset();
	}
	~COptions(){
	}
};

HWND hDialog=0;
COptions plgOptions;
int iDialogMode=0;
void GetStats(char* szOUT,int iType=-1)
{
	char sz[128]="";
	if(iType==-1 || iType==0){
		if(lShowStat1){
			if(plgOptions.g_CharsPMTime>1000){
				int iS=int(plgOptions.g_CharsPMTime/1000);
				int iM=int(iS/60);
				int iH=int(iM/60);
				sprintf(sz,"%s\t%02i:%02i:%02i",_pl2("Time"),iH,iM%60,iS%60);
				strcat(szOUT,sz);
				strcat(szOUT,"\r\n");
			}
			sprintf(sz,"%s\t%i",_pl2("Symbols"),plgOptions.g_CharsTotal);
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
			
			
			sprintf(sz,"%s\t%0.2f",_pl2("Sym./Min."),plgOptions.g_CharsPMTime==0?0.0:double(plgOptions.g_CharsPM)/(double(plgOptions.g_CharsPMTime)/1000/60));
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
		}
		
		if(lShowStat2){
			sprintf(sz,"%s\t%i",_pl2("Words"),plgOptions.g_WordsTotal);
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
			
			sprintf(sz,"%s\t%0.2f",_pl2("Words/Min."),plgOptions.g_CharsPMTime==0?0.0:double(plgOptions.g_WordsPM)/(double(plgOptions.g_CharsPMTime)/1000/60));
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
		}
		
		if(lShowStat3){
			sprintf(sz,"%s\t%0.2f%% (%i)",_pl2("Lines"),(plgOptions.g_NewLinesTotal==0||plgOptions.g_CharsTotal==0)?0.0:100.0*double(plgOptions.g_NewLinesTotal)/double(plgOptions.g_CharsTotal),plgOptions.g_NewLinesTotal);
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
			
			sprintf(sz,"%s\t%0.2f%% (%i)",_pl2("Spaces"),(plgOptions.g_SpaceTotal==0||plgOptions.g_CharsTotal==0)?0.0:100.0*double(plgOptions.g_SpaceTotal)/double(plgOptions.g_CharsTotal),plgOptions.g_SpaceTotal);
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
			
			sprintf(sz,"%s\t%0.2f%% (%i)",_pl2("Deletions"),(plgOptions.g_DelsTotal==0||plgOptions.g_CharsTotal==0)?0.0:100.0*double(plgOptions.g_DelsTotal)/double(plgOptions.g_CharsTotal),plgOptions.g_DelsTotal);
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
		}
		
		if(lShowStat4){
			double dArithm=(plgOptions.g_AritmiaCount==0)?0.0:100.0*plgOptions.g_Aritmia/double(plgOptions.g_AritmiaCount);
			sprintf(sz,"%s\t%0.2f%%",_pl2("Rhythm"),dArithm);
			strcat(szOUT,sz);
			strcat(szOUT,"\r\n");
		}
	}
	if(iType==-1 || iType==1){
		if(iType==-1){
			strcat(szOUT,"\r\n");
		}
		int iCounter=0;
		for(int i=32;i<256;i++){
			if(plgOptions.g_Chars[i]>0 && i!=185){
				//sprintf(sz,"'%c': %04i",i,plgOptions.g_Chars[i]);
				char szt[]="' '      ";
				szt[1]=i;
				sprintf(sz,"%s%02.01f%% (%i)",szt,(plgOptions.g_CharsTotal==0)?0.0:100.0*double(plgOptions.g_Chars[i])/double(plgOptions.g_CharsTotal),plgOptions.g_Chars[i]);
				strcat(szOUT,sz);
				strcat(szOUT,"\r\n");
				/*
				if(((iCounter+0)%2)==1){
					strcat(szOUT,"\r\n");
				}else{
					strcat(szOUT,"");
				}*/
				iCounter++;
			}
		}
		if(iCounter==0){
			strcat(szOUT,_pl("None"));
		}
	}
};

void UpdateDialog()
{
	if(!hDialog || !IsWindow(hDialog) || hDialog==HWND(1)){
		return;
	}
	char szOUT[16000]="";
	if(iDialogMode){
		sprintf(szOUT,"%s:\r\n",_pl("Detailed statistic"));
	}else{
		sprintf(szOUT,"%s:\r\n",_pl("Common statistic"));
	}
	GetStats(szOUT,iDialogMode);
	// Наружу...
	::SetWindowText(GetDlgItem(hDialog,IDC_RICHEDIT),szOUT);
}

int WINAPI HandleSystemEvent(CWKSYSEvent* pEvent)
{
	if(pEvent){
		BOOL bChanges=0;
		long dwMaxPause=g_MaxPausePerMinute*1000;
		if(dwMaxPause<100){
			dwMaxPause=100;
		}
		g_lLastpause=GetTickCount()-g_AllPMTickPrev;
		if(g_lLastpause<dwMaxPause){
			if(pEvent->iEventType==WKSYSEVENTTYPE_CHAR){
				BYTE c=(BYTE)pEvent->pData;
				if(c>=32){
					if(c<256){ 
						plgOptions.g_Chars[BYTE(c)]+=1;
					}
					bChanges++;
					// iF g_CharsPMTick==0 Слишком много времени с прошлого клика
					// все игнорим, лишь обновляем время нажатия
					if(g_CharsPMTick!=0){
						DWORD dwThisCharDelay=GetTickCount()-g_CharsPMTick;
						plgOptions.g_CharsTotal++;
						{
							plgOptions.g_CharsPM++;
							plgOptions.g_CharsPMTime+=dwThisCharDelay;
						}
						if(isspace(c)){
							bChanges++;
							plgOptions.g_SpaceTotal++;
						}
						BOOL bAl=isAlphaChar(char(c));
						if(bWaitingForWord && bAl){
							bWaitingForWord=0;
							plgOptions.g_WordsTotal++;
							bChanges++;
							{
								plgOptions.g_WordsPM++;
							}
						}else if(!bWaitingForWord && !bAl){
							bWaitingForWord=1;
						}
						if(plgOptions.g_CharsPM>0){
							double dArithmRaw=dwThisCharDelay;
							double dMiddleValue=double(plgOptions.g_CharsPMTime)/double(plgOptions.g_CharsPM);
							double dArithm=fabs(1.0-(dArithmRaw/dMiddleValue));
							if(dArithm>1.0){
								dArithm=1.0;
							}
							if(dArithm<0.0){
								dArithm=0.0;
							}
							TRACE3("%f, %f->%f\n",dMiddleValue,dArithmRaw,dArithm);
							plgOptions.g_Aritmia+=dArithm;
							plgOptions.g_AritmiaCount++;
						}
					}else{
						// Но символ засчитываем...
						plgOptions.g_CharsTotal++;
					}
				}else{
					if(c==VK_TAB){
						bChanges++;
						bWaitingForWord=1;
						plgOptions.g_SpaceTotal++;
					}
					if(c==VK_RETURN){
						bChanges++;
						bWaitingForWord=1;
						plgOptions.g_NewLinesTotal++;
					}
					if(c==VK_BACK){
						bChanges++;
						plgOptions.g_DelsTotal++;
					}
				}
			}
			if(pEvent->iEventType==WKSYSEVENTTYPE_KEYPRESS){
				DWORD c=(DWORD)pEvent->pData;
				if(c==VK_DELETE){
					bChanges++;
					plgOptions.g_DelsTotal++;
				}
			}
			if(bChanges){
				// Чтобы не на все реагировать
				g_CharsPMTick=GetTickCount();
				UpdateDialog();
			}
		}else{
			g_CharsPMTick=0;
		}
		g_AllPMTickPrev=g_AllPMTick;
		g_AllPMTick=GetTickCount();
	}
	return 1;
}

HBRUSH hBG=0;
HFONT hfCreated=0;
#include <Richedit.h>
int CALLBACK StatsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		hDialog=hwndDlg;
		::SetWindowLong(hwndDlg,GWL_EXSTYLE,WS_EX_APPWINDOW|::GetWindowLong(hwndDlg,GWL_EXSTYLE));
		::SetWindowPos(hwndDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSENDCHANGING|SWP_NOSIZE|SWP_NOMOVE);
		::SetWindowText(hDialog,_pl("Typing statistics"));
		::SetWindowText(GetDlgItem(hDialog,IDOK),_pl("Close"));
		::SetWindowText(GetDlgItem(hDialog,IDRESET),_pl("Reset"));
		iDialogMode=0;
		::SetWindowText(GetDlgItem(hDialog,IDDETAILS),_pl("Details"));
		LOGFONT lg;
		HFONT hf=(HFONT)::SendMessage(hwndDlg,WM_GETFONT,0,0);
		::GetObject(hf,sizeof(lg),&lg);
		strcpy(lg.lfFaceName,"Courier");
		hfCreated=CreateFontIndirect(&lg);
		SendMessage(GetDlgItem(hwndDlg,IDC_RICHEDIT),WM_SETFONT,WPARAM(hfCreated),0);
		// Цвет текста...
		CHARFORMAT cf;
		cf.cbSize=sizeof(cf);
		cf.dwMask = CFM_COLOR;
		cf.dwEffects = 0;
		cf.crTextColor = lTX;
		SendMessage(GetDlgItem(hwndDlg,IDC_RICHEDIT),EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&cf);
		SendMessage(GetDlgItem(hwndDlg,IDC_RICHEDIT),EM_SETBKGNDCOLOR,0,(LPARAM)lBG);
		if(hBG){
			::DeleteObject(hBG);
		}
		hBG=::CreateSolidBrush(lBG);
		if(lXPos!=0 && lYPos!=0){
			CRect rt;
			::GetWindowRect(hwndDlg,&rt);
			::MoveWindow(hwndDlg,lXPos,lYPos,rt.Width(),rt.Height(),1);
		}
		//m_hBr = new CBrush(cBGColor); 
		UpdateDialog();
	}
	if(uMsg==WM_NCDESTROY){
		CRect rt;
		::GetWindowRect(hwndDlg,&rt);
		lXPos=rt.left;
		lYPos=rt.top;
		if(hfCreated){
			::DeleteObject(hfCreated);
			hfCreated=0;
		}
		if(hBG){
			::DeleteObject(hBG);
			hBG=0;
		}
	}
	if(uMsg==WM_CTLCOLOR || uMsg==WM_CTLCOLORBTN || uMsg==WM_CTLCOLOREDIT
		|| uMsg==WM_CTLCOLORDLG || uMsg==WM_CTLCOLORLISTBOX
		|| uMsg==WM_CTLCOLORSCROLLBAR || uMsg==WM_CTLCOLORSTATIC){
		HDC hDC=(HDC)wParam;
		SetTextColor(hDC,lTX);
		SetBkColor(hDC,lBG);
		return (int)hBG;
	}
	if(uMsg==WM_CLOSE || (uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE)){
		uMsg=WM_COMMAND;
		wParam=IDOK;
	}
	if(uMsg==WM_COMMAND && wParam==IDDETAILS){
		iDialogMode=1-iDialogMode;
		if(iDialogMode){
			::SetWindowText(GetDlgItem(hDialog,IDDETAILS),_pl("Common"));
		}else{
			::SetWindowText(GetDlgItem(hDialog,IDDETAILS),_pl("Details"));
		}
		UpdateDialog();
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	if(uMsg==WM_COMMAND && wParam==IDRESET){
		{// Сохранить в файл?
			if(AfxMessageBox(_pl("Save statistics into file first")+"?",MB_YESNO|MB_ICONQUESTION)==IDYES){
				//Enable3dControlsStatic();	// Call this when linking to MFC statically
				CWnd pThis;
				pThis.Attach(hwndDlg);
				OleInitialize(0);
				CFileDialogST dlg(0, NULL, NULL, OFN_NODEREFERENCELINKS, "All files (*.*)|*.*||", &pThis);//OFN_EXPLORER|OFN_ENABLESIZING|
				if(dlg.DoModal()==IDOK){
					CString sFile=dlg.GetPathName();
					char szOUT[16000]="";
					GetStats(szOUT);
					sFile.TrimLeft();
					sFile.TrimRight();
					FILE* m_pFile=fopen(sFile,"w+b");
					if(m_pFile){
						DWORD nRead=fwrite(szOUT,sizeof(char),strlen(szOUT),m_pFile);
						fclose(m_pFile);
					}
				}
				pThis.Detach();
			}
		}	
		plgOptions.Reset();
		UpdateDialog();
		return TRUE;
	}
	return FALSE;
}

DWORD WINAPI ShowMainDialog(LPVOID p)
{
	if(!WKGetPluginContainer()->getOption(WKOPT_AUTOTEXT)){
		WKGetPluginContainer()->ShowAlert(_pl2("Sorry, you must enable Autotext plugin\nin order to use this plugin. You can enable it\nin the preferences dialog"),_pl2("Plugin error"));
		return 0;
	}
	OleInitialize(0);
	AfxInitRichEdit();
	DWORD dwRes=DialogBox(g_hinstDll,MAKEINTRESOURCE(IDD_DIALOG1),(HWND)(WKGetPluginContainer()->getOptionEx(2,0)),StatsDialogProc);
	DWORD dwErr=GetLastError();
	OleUninitialize();
	if(!WKUtils::isWKUpAndRunning()){
		return 0;
	}
	hDialog=0;
	return 1;
}

int	WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff)
{
	if(hDialog!=0){
		if(hDialog!=HWND(1)){
			HWND hTmp=hDialog;
			hDialog=0;
			EndDialog(hTmp,0);
		}
		return 1;
	}
	hDialog=HWND(1);
	FORK(ShowMainDialog,stuff?stuff->hMainWKWindow:0)
	return 1;
}

int	WINAPI GetPluginDsc(WKPluginDsc* dsc)
{
	dsc->bResidentPlugin=TRUE;
	dsc->bLoadPluginByDefault=TRUE;
	//strcpy(dsc->szTitle,"Typing statistics");
	strcpy(dsc->szDesk,"Shows your skill in typing");
	dsc->hPluginBmp=::LoadBitmap(g_hinstDll,MAKEINTRESOURCE(IDB_BM_ICON));
	return 1;
}


int	WINAPI WKPluginOptionsManager(int iAction, WKOptionsCallbackInterface* pOptionsCallback, DWORD dwParameter)
{
	if(iAction==OM_STARTUP_ADD){
		pOptionsCallback->AddNumberOption("g_MaxPausePerMinute","Stop counting while keyboard is not used specified period of seconds","",2,0,9999);
		pOptionsCallback->AddBoolOption("lShowStat1","Show symbol stats","");
		pOptionsCallback->AddBoolOption("lShowStat2","Show word stats","");
		pOptionsCallback->AddBoolOption("lShowStat3","Show special stats","");
		pOptionsCallback->AddBoolOption("lShowStat4","Show rhythm deviation","");
		pOptionsCallback->AddNumberOption("xwndpos","","",0,0,9999,0);
		pOptionsCallback->AddNumberOption("ywndpos","","",0,0,9999,0);
		pOptionsCallback->AddColorOption("lTX","Window text color","",0x0000FF11);
		pOptionsCallback->AddColorOption("lBG","Window background color","",0x00000000);
		pOptionsCallback->AddBinOption("g_CharsTotal");
	}
	if(iAction==OM_STARTUP_ADD || iAction==OM_OPTIONS_SET){
		if(pOptionsCallback){
			g_MaxPausePerMinute=pOptionsCallback->GetBoolOption("g_MaxPausePerMinute");
			lShowStat1=pOptionsCallback->GetBoolOption("lShowStat1");
			lShowStat2=pOptionsCallback->GetBoolOption("lShowStat2");
			lShowStat3=pOptionsCallback->GetBoolOption("lShowStat3");
			lShowStat4=pOptionsCallback->GetBoolOption("lShowStat4");
			lXPos=pOptionsCallback->GetNumberOption("xwndpos");
			lYPos=pOptionsCallback->GetNumberOption("ywndpos");
			lTX=pOptionsCallback->GetNumberOption("lTX");
			lBG=pOptionsCallback->GetNumberOption("lBG");
			pOptionsCallback->GetBinOption("g_CharsTotal",&plgOptions,sizeof(plgOptions));
		}
	}
	if(iAction==OM_OPTIONS_GET){
		pOptionsCallback->SetBoolOption("g_MaxPausePerMinute",g_MaxPausePerMinute);
		pOptionsCallback->SetBoolOption("lShowStat1",lShowStat1);
		pOptionsCallback->SetBoolOption("lShowStat2",lShowStat2);
		pOptionsCallback->SetBoolOption("lShowStat3",lShowStat3);
		pOptionsCallback->SetBoolOption("lShowStat4",lShowStat4);
		pOptionsCallback->SetNumberOption("xwndpos",lXPos);
		pOptionsCallback->SetNumberOption("ywndpos",lYPos);
		pOptionsCallback->SetNumberOption("lTX",lTX);
		pOptionsCallback->SetNumberOption("lBG",lBG);
		pOptionsCallback->SetBinOption("g_CharsTotal",&plgOptions,sizeof(plgOptions));
		UpdateDialog();
	}
	return 1;
}


int WINAPI RenderScript(const char* szParameters_in, char* szResult_out, size_t dwResultSize_in)
{
	if(szResult_out){
		*szResult_out=0;
	}
	int iFrom=0;
	CString sBody=szParameters_in,sRes;
	CString sCommand=sBody.SpanExcluding(" \t\r\n");
	if(sCommand.CompareNoCase("Reset")==0){
		plgOptions.Reset();
	}
	if(sCommand.CompareNoCase("Save")==0){
		char szOUT[16000]="";
		GetStats(szOUT);
		CString sFile=sBody.Mid(strlen(sCommand));
		sFile.TrimLeft();
		sFile.TrimRight();
		FILE* m_pFile=fopen(sFile,"w+b");
		if(m_pFile){
			DWORD nRead=fwrite(szOUT,sizeof(char),strlen(szOUT),m_pFile);
			fclose(m_pFile);
		}
	}
	return 0;
}