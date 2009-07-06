// WKPluginEngine.cpp: implementation of the CWKPluginEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wirekeys.h"
#include "WKPluginEngine.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//Special event classes (Class name = ../t@...)
//"@ROOT"
//"@NU"
//"@NS"
//"@NSC"
//"@LONGPLAY"

extern int iGlobalState;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPlugin::~CPlugin()
{_XLOG_
	ClearBitmap(hRealPluginBmp);
	hRealPluginBmp=0;
	CString sName;
	CPluginOption* pValue=NULL;
	for(int i=0;i<options.GetSize();i++){_XLOG_
		if(options[i]){_XLOG_
			delete options[i];
		}
	}
};

int CallPluginStartFunction(FPLUGINSTARTFUNCTION plugFunc)
{_XLOG_
	int iRes=0;
	__try
	{_XLOG_
		iRes=(*plugFunc)();
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		iRes=-1;
	}
	return iRes;
}

int CallPluginStopFunction(FPLUGINSTOPFUNCTION plugFunc)
{_XLOG_
	if(plugFunc==NULL){_XLOG_
		return 0;
	}
	int iRes=0;
	__try
	{_XLOG_
		iRes=(*plugFunc)();
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		iRes=-1;
	}
	return iRes;
}


int CallPluginTPFunction(CPlugin* oPlug,int iActionType, int& iItemId, HMENU hMenu)
{_XLOG_
	if(oPlug==NULL){_XLOG_
		return 0;
	}
	
	FPLUGINDPOPUPMENUFUNCTION plugTPFunc=oPlug->fTrayMenuFunction;
	if(plugTPFunc!=NULL){_XLOG_
		__try
		{_XLOG_
			plugTPFunc(iActionType, iItemId, hMenu);
		}
		__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
			return 0;
		}
	}
	return 1;
}

int CallPluginPauseFunction(CPlugin* oPlug, BOOL bPause)
{_XLOG_
	if(oPlug==NULL){_XLOG_
		return 0;
	}
	
	FPLUGINDSWITCHFUNCTION plugDSFunc=oPlug->fDSwitchFunction;
	if(plugDSFunc!=NULL){_XLOG_
		__try
		{_XLOG_
			plugDSFunc(bPause);
		}
		__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
			return 0;
		}
		return 1;
	}
	
	FPLUGINPAUSEFUNCTION plugFunc=oPlug->fPauseFunction;
	if(plugFunc==NULL){_XLOG_
		return 0;
	}
	int iRes=0;
	__try
	{_XLOG_
		iRes=(*plugFunc)(bPause);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		iRes=-1;
	}
	return iRes;
}

HINSTANCE SafeLoadLibrary(const char* szFile)
{_XLOG_
	HINSTANCE hInst=0;
	__try
	{_XLOG_
		hInst=LoadLibrary(szFile);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		hInst=NULL;
	}
	return hInst;
};

typedef WKCallbackInterface*& (WINAPI *FPLUGINCONTFUNCTION)();
BOOL LoadPluginRaw(CPlugin*& oPlug)
{_XLOG_
	if(!oPlug){_XLOG_
		return 0;
	}
	if(oPlug->hInst!=NULL){_XLOG_
		// Уже загружен
		return TRUE;
	}
	SetLastError(0);
	oPlug->hInst = SafeLoadLibrary(oPlug->sFile);
	TRACE1("Loading %s",oPlug->sFile);
	if(oPlug->hInst){_XLOG_
		FPLUGINCONTFUNCTION fContFunction=NULL;
		oPlug->fCallFunction=(FPLUGINCALLFUNCTION)DBG_GETPROC(oPlug->hInst, "WKCallPluginFunction");
		if(oPlug->fCallFunction){_XLOG_
			oPlug->fActualParamsFunction=(FPLUGINACTDSCFUNCTION)DBG_GETPROC(oPlug->hInst, "WKGetPluginFunctionActualDesc");
			oPlug->fOptionsFunction=(FPLUGINOPTIONSFUNCTION)DBG_GETPROC(oPlug->hInst, "WKPluginOptions");
			oPlug->fOptionsManager=(FPLUGINOPTMANSFUNCTION)DBG_GETPROC(oPlug->hInst, "WKPluginOptionsManager");
			oPlug->fStopFunction=(FPLUGINSTOPFUNCTION)DBG_GETPROC(oPlug->hInst, "WKPluginStop");
			oPlug->fPauseFunction=(FPLUGINPAUSEFUNCTION)DBG_GETPROC(oPlug->hInst, "WKPluginPause");
			oPlug->fDSwitchFunction=(FPLUGINDSWITCHFUNCTION)DBG_GETPROC(oPlug->hInst, "WKDesktopSwitching");
			oPlug->fInitFunction=(FPLUGININITFUNCTION)DBG_GETPROC(oPlug->hInst, "WKPluginInit");
			oPlug->fStartFunction=(FPLUGINSTARTFUNCTION)DBG_GETPROC(oPlug->hInst, "WKPluginStart");
			oPlug->fTrayMenuFunction=(FPLUGINDPOPUPMENUFUNCTION)DBG_GETPROC(oPlug->hInst, "WKTrayMenuPopup");
			fContFunction=(FPLUGINCONTFUNCTION)DBG_GETPROC(oPlug->hInst, "WKGetPluginContainer");
		}
		if(objSettings.lDoNotLoadMacros && oPlug->fPauseFunction==0){_XLOG_
			UnloadPluginRaw(oPlug);
			return FALSE;
		}
		if(fContFunction){_XLOG_
			// Инициализируем указатель
			(*fContFunction)()=oPlug;
		}
		if(oPlug->fStartFunction){_XLOG_
			int iRes=CallPluginStartFunction(oPlug->fStartFunction);
			if(iRes>0){_XLOG_
				CString sP=CString("WKPLUG_")+oPlug->sFileName;
				sP.MakeUpper();
				HWND hW=FindWindow("WK_MAIN",0);
				if(hW!=0){_XLOG_
					SetProp(hW,sP,oPlug->hInst);
				}
				return TRUE;
			}
		}
	}
	AddError(Format("Failed to load DLL '%s':%s",oPlug->sFile,GetCOMError(GetLastError())),TRUE);
	UnloadPluginRaw(oPlug);
	return FALSE;
}

BOOL UnloadPluginRawGuarded(HINSTANCE hInst, const char* szFile)
{_XLOG_
	iGlobalState=10;
	// Если ФИНАЛЬНОЕ выключение - то не выключаем
	if(!pMainDialogWindow || pMainDialogWindow->iRunStatus==-2){
		return 1;
	}
	int iRes=0;
	__try
	{_XLOG_
		iRes=FreeLibrary(hInst);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		//LOGERROR1("UnloadPluginRawGuarded failed on %s!",szFile);
		iRes=0;
	}
	return iRes;
}

BOOL UnloadPluginRaw(CPlugin*& oPlug)
{_XLOG_
	if(!oPlug){_XLOG_
		return 0;
	}
	if(oPlug->hInst==NULL){_XLOG_
		// Уже выгружен
		return TRUE;
	}
	__FLOWLOG2("Unloading plugin:",oPlug->sFile);
	// Стопаем. там настройки могут пометится как
	// требующие сохранения!!!
	TRACE1("Unloading plugin: %s\n",oPlug->sFile);
	CallPluginStopFunction(oPlug->fStopFunction);
	TRACE1("Unloading plugin done: %s\n",oPlug->sFile);
	CString sP=CString("WKPLUG_")+oPlug->sFileName;
	sP.MakeUpper();
	HWND hW=FindWindow("WK_MAIN",0);
	if(hW!=0){_XLOG_
		SetProp(hW,sP,0);
	}
	// Вызываем умное сохранение
	// Сохранится только если нужно...
	SavePluginOptions(oPlug);
	UnloadPluginRawGuarded(oPlug->hInst,oPlug->sFile);
	oPlug->hInst=NULL;
	oPlug->fInitFunction=NULL;
	oPlug->fCallFunction=NULL;
	oPlug->fActualParamsFunction=NULL;
	oPlug->fOptionsFunction=NULL;
	oPlug->fOptionsManager=NULL;
	oPlug->fStartFunction=NULL;
	oPlug->fStopFunction=NULL;
	oPlug->fPauseFunction=NULL;
	oPlug->fTrayMenuFunction=NULL;
	oPlug->iPluginIconNum=-1;
	ClearBitmap(oPlug->hRealPluginBmp);
	oPlug->hRealPluginBmp=0;
	return TRUE;
}

// Возвращает нужно ли добавлять плагин
BOOL LoadPlugin(CPlugin*& oPlug, BOOL bForceLoad)
{_XLOG_
	__FLOWLOG2("loading plugin:",oPlug->sFile);
	char szFile[MAX_PATH]="C://", szPath[MAX_PATH]="";
	_splitpath(oPlug->sFile, NULL, szPath, szFile, NULL);
	oPlug->sFileName=szFile;
	oPlug->bIsActive=1;
	oPlug->sTitle="NAMEPLG_"+oPlug->sFileName;
	oPlug->sDescription="";
	CString sLName=oPlug->sFileName;
	sLName.MakeLower();
	sLName=" "+sLName+" ";
	if(objSettings.sActivePlugins.Find(sLName)==-1){_XLOG_
		if(bForceLoad){_XLOG_
			objSettings.sActivePlugins+=sLName;
		}else{_XLOG_
			// Нельзя. Просто задаем некоторые свойства а сам плагин более не грузим
			oPlug->bIsActive=0;
			return TRUE;
		}
	}
	if(oPlug->hInst!=NULL){//!bForceLoad тут не учитывается! //  || (!oPlug->bResident && oPlug->bOptionsWasChanged)
		// Уже загружен или плагин не резидентный а опции у него уже менялись...
		return TRUE;
	}
	//Имя файла плагина
	if(LoadPluginRaw(oPlug) && oPlug->hInst){_XLOG_
		FPLUGINCOMDSCFUNCTION fpTitle=(FPLUGINCOMDSCFUNCTION)DBG_GETPROC(oPlug->hInst, "GetPluginDsc");
		WKPluginDsc dsc;
		if(fpTitle){_XLOG_
			(*fpTitle)(&dsc);
			if(dsc.szTitle[0]){_XLOG_
				oPlug->sTitle=dsc.szTitle;
			}
			oPlug->sDescription=dsc.szDesk;
			oPlug->bResident=TRUE;//dsc.bResidentPlugin;
			oPlug->sAuthors=dsc.szAuthors;
			oPlug->dwVersionHi=dsc.dwVersionHi;
			oPlug->dwVersionLo=dsc.dwVersionLo;
		}
		if(dsc.hPluginBmp!=NULL && oPlug->hRealPluginBmp==NULL){_XLOG_
			CopyBitmapToBitmap(dsc.hPluginBmp,oPlug->hRealPluginBmp,CSize(IMAGELIST_SIZE,IMAGELIST_SIZE),NULL,TRUE);
			if(oPlug->hRealPluginBmp!=NULL){_XLOG_
				HBITMAP hCopy=(HBITMAP)CopyImage(oPlug->hRealPluginBmp,IMAGE_BITMAP, IMAGELIST_SIZE, IMAGELIST_SIZE,0);
				oPlug->iPluginIconNum=ImageList_AddMasked(theApp.MainImageList, hCopy, RGB(255,255,255));
				ClearBitmap(hCopy);
			}
		}
		//Файл с сохраненными настройками...
		CString sPluginHash=GetPathPart(oPlug->sFile,0,0,1,0);
		if(!(oPlug->dwVersionHi==0 && oPlug->dwVersionLo==0)){_XLOG_
			sPluginHash+=Format("_%i_%i",oPlug->dwVersionHi,oPlug->dwVersionLo);
		}
		// Считываем значения настроек
		CDataXMLSaver PluginIniFile(getFileFullName(Format("Plugins\\%s.conf",sPluginHash)),"plugin-config",TRUE);
		int iOptionNum=0;
		CString sName,sValue;
		while(PluginIniFile.getValue(Format("opt%lu-name",iOptionNum),sName)){_XLOG_
			PluginIniFile.getValue(Format("opt%lu-value",iOptionNum),sValue);
			CPluginOption* pNewOption=new CPluginOption();
			pNewOption->m_sValue=sValue;
			pNewOption->m_lValue=atol(sValue);
			pNewOption->sName=sName;
			oPlug->options.Add(pNewOption);
			iOptionNum++;
		}
		if(oPlug->fOptionsManager){_XLOG_
			(*oPlug->fOptionsManager)(OM_STARTUP_ADD,oPlug,0);
		}
		FPLUGINDSCFUNCTION fpDsc=(FPLUGINDSCFUNCTION)DBG_GETPROC(oPlug->hInst, "WKGetPluginFunctionCommonDesc");
		if(fpDsc){_XLOG_
			int iFunction=0;
			while(1 && iFunction<100){_XLOG_
				int iAlreadyFuncsCount=WKPluginFunctionDscEx::dwCommandCodeCount-(WM_USER+PLUGIN_FUNCTION);
				if(iAlreadyFuncsCount>MAX_PLUGFUNCS){_XLOG_
					break;
				}
				WKPluginFunctionDscEx dsc;
				if((*fpDsc)(iFunction,(WKPluginFunctionDsc*)&dsc)){_XLOG_
					CIHotkey iDefHK(dsc.dwDefaultHotkey);
					FPLUGINDSCFUNCTIONHINTS fpDscHint=(FPLUGINDSCFUNCTIONHINTS)DBG_GETPROC(oPlug->hInst, "WKGetPluginFunctionHints");
					if(fpDscHint){_XLOG_
						LPVOID pHKP=0;
						int iRes=fpDscHint(iFunction,0,pHKP);
						if(iRes && pHKP){_XLOG_
							iDefHK.InitFromSave((char*)pHKP,FALSE);
						}
					}
					if(dsc.hItemBmp){_XLOG_
						CopyBitmapToBitmap(dsc.hItemBmp,dsc.hRealItemBmp,_bmp().MenuIconSize,NULL,TRUE);
					}
					PluginIniFile.getValue(Format("func%lu-hotkey",iFunction),dsc.oRealHotkey,iDefHK);
					dsc.oRealDefHotkey=iDefHK;
					dsc.dwCommandCode=WKPluginFunctionDscEx::dwCommandCodeCount;
					WKPluginFunctionDscEx::dwCommandCodeCount++;
					oPlug->funcs.SetAtGrow(iFunction,dsc);
				}else{_XLOG_
					break;
				}
				iFunction++;
			}
		}
		if(!oPlug->bResident){_XLOG_
			UnloadPluginRaw(oPlug);
		}else if(oPlug->fInitFunction){_XLOG_
			FORK(PluginOperation,oPlug->lIndex);
		}
		CString sLowerFile=oPlug->sFileName;
		sLowerFile.MakeLower();
		return TRUE;
	}
	oPlug->bIsActive=0;
	return FALSE;
}

BOOL SavePluginOptions(CPlugin* oPlug)
{_XLOG_
	if(objSettings.lSkipSavings){
		return FALSE;
	}
	if(oPlug->bOptionsWasChanged==FALSE){_XLOG_
		return FALSE;
	}
	oPlug->bOptionsWasChanged=FALSE;
	if(objSettings.lStartupDesktop!=0 || objSettings.iLikStatus==1){_XLOG_
		return FALSE;
	}
	CString sPluginHash=GetPathPart(oPlug->sFile,0,0,1,0);
	if(!(oPlug->dwVersionHi==0 && oPlug->dwVersionLo==0)){_XLOG_
		sPluginHash+=Format("_%i_%i",oPlug->dwVersionHi,oPlug->dwVersionLo);
	}
	// Сохраняем последнее...
	CDataXMLSaver PluginIniFile(getFileFullName(Format("Plugins\\%s.conf",sPluginHash)),"plugin-config",FALSE);
	PluginIniFile.putValue("file",oPlug->sFile);
	// Сохраняем значения гор. клавиш
	for(int j=0;j<oPlug->funcs.GetSize();j++){_XLOG_
		WKPluginFunctionDscEx* pFunc=&oPlug->funcs[j];
		if(pFunc){_XLOG_
			PluginIniFile.putValue(Format("func%lu-hotkey",j),pFunc->oRealHotkey);
		}
	}
	// Сохраняем значения настроек
	int iOptionNum=0;
	CString sName;
	CPluginOption* pValue=NULL;
	for(int i=0;i<(oPlug->options.GetSize());i++){_XLOG_
		pValue=oPlug->options[i];
#ifdef _DEBUG
		if(!pValue->m_pOption){_XLOG_
			TRACE("Empty plugin option!\n");
		}
#endif
		int iType=pValue->m_pOption?(pValue->m_pOption->iOptionType):0;
		if(iType==OPTTYPE_HOTKEY){_XLOG_
			continue;
		}
		if(pValue && pValue->m_pOption){_XLOG_
			PluginIniFile.putValue(Format("opt%lu-name",iOptionNum),pValue->sName);
			PluginIniFile.putValue(Format("opt%lu-value",iOptionNum),(pValue->m_pOption->iOptionType==OPTTYPE_STRING)?pValue->m_sValue:Format("%i",pValue->m_lValue));
			iOptionNum++;
		}
	}
	return TRUE;
}

BOOL UnloadPlugin(CPlugin* oPlug)
{_XLOG_
__FLOWLOG
	oPlug->bIsActive=0;
	DWORD dwTime1=GetTickCount();
	iGlobalState=11;
	TRACE2("%i Unloading plugin %s\n",GetCurrentThreadId(),oPlug->sFile);
	// Вызываем умное сохранение
	// Сохранится только если нужно..
	SavePluginOptions(oPlug);
	CString sLowerFile=oPlug->sFileName;
	sLowerFile.MakeLower();
	if(oPlug->hInst==NULL && oPlug->bResident){_XLOG_
		// Уже выгружено
		FLOG2("plugin %s unloaded in %lu ms\n",oPlug->sTitle,GetTickCount()-dwTime1);
		return TRUE;
	}
	// Выгружаем все нафиг
	UnloadPluginRaw(oPlug);
	for(int j=0;j<oPlug->funcs.GetSize();j++){_XLOG_
		WKPluginFunctionDscEx* pFunc=&oPlug->funcs[j];
		if(pFunc){_XLOG_
			ClearBitmap(pFunc->hRealItemBmp);
		}
	}
	oPlug->funcs.RemoveAll();
	FLOG2("plugin %s unloaded in %lu ms\n",oPlug->sTitle,GetTickCount()-dwTime1);
	return TRUE;
}

DWORD WKPluginFunctionDscEx::dwCommandCodeCount=WM_USER+PLUGIN_FUNCTION;
void LoadPlugins()
{_XLOG_
	// Устанавливаем каталог для того, чтобы горячие клавиши могли влиять на процесс получения их...
	CString sStartDir=GetPluginsDirectory();
	if(sStartDir=="" || !isFileExist(sStartDir)){_XLOG_
		Alert("Plugins directory not found!");
		return;
	}
	// Обследуем каталог и считаем настройки
	CStringArray aPresentPlugins;
	CFileInfoArray dir;
	dir.AddDir(sStartDir,"*.wkp;*.exe",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	for(int i=0;i<dir.GetSize();i++){_XLOG_
		CString sFile=dir[i].GetFilePath();
		CString sFileName=GetFileFromPath(sFile);
		if(sFile!=""){_XLOG_
			CString sLowerFile=sFile;
			sLowerFile.MakeLower();
			// Нужные макросы грузим в первую очередь
			if(sLowerFile.Find("wp_keylogger")==-1){// Больше не грузим!!!
				if(sLowerFile.Find("wp_keymaster")!=-1 || sLowerFile.Find("wp_autotext")!=-1
					 || sLowerFile.Find("wp_schedule")!=-1){_XLOG_
					aPresentPlugins.InsertAt(0,sFile);
				}else{_XLOG_
					aPresentPlugins.Add(sFile);
				}
			}
		}
	}
	for(int j=0;j<aPresentPlugins.GetSize();j++){_XLOG_
		CPlugin* oPlug=new CPlugin();
		oPlug->sFile=aPresentPlugins[j];
		// Даже если не грузим - все равно добавляем
		oPlug->lIndex=objSettings.plugins.Add(oPlug);
		if(!LoadPlugin(oPlug,FALSE)){_XLOG_
			// Загрузка капитально не удалась
			objSettings.plugins.RemoveAt(oPlug->lIndex);
			delete oPlug;
			oPlug=NULL;
		}
	}
}

int UnLoadPlugins(BOOL bAndDisableForFuture)
{_XLOG_
	iGlobalState=12;
	int iRes=0;
	if(objSettings.plugins.GetSize()==0){_XLOG_
		return iRes;
	}
	int i=0;
	// сначала все сохраняем
	for(i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
		if(objSettings.plugins[i]){_XLOG_
			SavePluginOptions(objSettings.plugins[i]);
		}
	}
	if(bAndDisableForFuture){_XLOG_
		objSettings.sActivePlugins="";
	}
	for(i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
		if(objSettings.plugins[i]){_XLOG_
			//FORK(UnloadPlugin_InThread)
			UnloadPlugin(objSettings.plugins[i]);
			delete objSettings.plugins[i];
			iRes++;
		}
	}
	objSettings.plugins.RemoveAll();
	return iRes;
}

CCriticalSection csPluginPause;
BOOL PauseAllWK(BOOL bPause)
{_XLOG_
	__FLOWLOG;
	if(bPause==FALSE){_XLOG_
		if(objSettings.lEverythingIsPaused){_XLOG_
			objSettings.lDisableSounds=0;
			objSettings.lEverythingIsPaused=0;
			OpenCurrentDeskAtom(1);
		}
	}else{_XLOG_
		if(objSettings.lEverythingIsPaused==0){_XLOG_
			objSettings.lDisableSounds=1;
			objSettings.lEverythingIsPaused=1;
			OpenCurrentDeskAtom(0);
		}
	}
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->SetWindowIcon();
	}
	return 0;
}

int PrepareDesktopSwitch(BOOL bPause)
{_XLOG_
	FLOG1("Switching desktop: %i",bPause);
	int iRes=0;
	PauseAllWK(bPause);
	if(objSettings.plugins.GetSize()==0){_XLOG_
		return iRes;
	}
#ifdef _DEBUG
	CString sStats=bPause?"pausing":"unpausing";
#endif
	CSmartLock SL(&csPluginPause,TRUE);
	for(int i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
		CPlugin* oPlug=objSettings.plugins[i];
		if(oPlug){_XLOG_
#ifdef _DEBUG
			DWORD dwA=GetTickCount();
			sStats+="\n"+oPlug->sFileName+" ";
#endif
			if(objSettings.lDoNotLoadMacros && oPlug->sFileName.CompareNoCase("WP_KeyMaster")!=0){_XLOG_
				continue;
			}
			if(oPlug && oPlug->bIsActive){_XLOG_
				CallPluginPauseFunction(oPlug, bPause);
			}
#ifdef _DEBUG
			DWORD dwB=GetTickCount();
			//if((dwB-dwA)>10)
			{_XLOG_
				sStats+=Format(": %.02f",(dwB-dwA)/100.0);
			}
#endif
		}
	}
#ifdef _DEBUG
	//Alert(sStats,"Debug");
#endif
	return iRes;
}

void CPlugin::SetPluginDsc(CPluginOption* pPlug, const char* szOptionTitle, const char* szOptionDescription,DWORD dwOptionPosition)
{_XLOG_
	pPlug->m_dwOptionPosition=dwOptionPosition;
	if(szOptionTitle==0 || strlen(szOptionTitle)==0){_XLOG_
		pPlug->m_sOptionTitle="";
		return;
	}
	pPlug->m_sOptionTitle=this->_l(CString("POPT_")+TrimMessage(szOptionTitle,10)+"_"+GetStringHash(szOptionTitle),szOptionTitle);
	if(szOptionDescription!=NULL && strlen(szOptionDescription)>0){_XLOG_
		pPlug->m_sOptionTitle+="\t";
		pPlug->m_sOptionTitle+=this->_l(CString("POPT_")+TrimMessage(szOptionDescription,10)+"_"+GetStringHash(szOptionDescription),szOptionDescription);
	}
	return;
}

int CPlugin::Lookup(const char* szName,CPluginOption*& pOption)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	for(int i=0;i<options.GetSize();i++){_XLOG_
		if(options[i]){_XLOG_
			CString sNmae=options[i]->sName;
			if(sNmae.Compare(szName)==0){_XLOG_
				pOption=options[i];
				return i;
			}
		}
	}
	pOption=NULL;
	return -1;
}

BOOL CPlugin::AddBoolOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long bDefaultValue, DWORD dwOptionPosition)
{_XLOG_
	return AddBoolOptionEx(szOptionName, szOptionTitle, szOptionDescription, bDefaultValue, dwOptionPosition,NULL)!=-1;
}

int CPlugin::AddActionHotkey(const char* szOptionName, const char* szActionTitle, const char* szOptionDescription, DWORD dwActionNumber, DWORD dwOptionPosition, const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* objNewOption=NULL;
	int iPos=Lookup(szOptionName,objNewOption);
	if(iPos==-1){_XLOG_
		objNewOption=new CPluginOption();
	}
	objNewOption->m_pParent=szParent;
	if(objNewOption->m_pOption){_XLOG_
		// Нужно удалить так как при считывании
		// тип наверно был установлен совсем другой...
		delete objNewOption->m_pOption;
		objNewOption->m_pOption=0;
	}
	if(objNewOption->m_pOption==0){_XLOG_
		objNewOption->m_pOption=new COptionHotKey(CIHotkey(), 0);
	}
	objNewOption->m_lValue=dwActionNumber;
	SetPluginDsc(objNewOption,szActionTitle,szOptionDescription,dwOptionPosition);
	objNewOption->sName=szOptionName;
	if(iPos==-1){_XLOG_
		return options.Add(objNewOption);
	}
	return iPos;
}

int CPlugin::AddColorOption(const char* szOptionName, const char* szActionTitle, const char* szOptionDescription, DWORD dwDefaultColor, DWORD dwOptionPosition, const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* objNewOptionOld=NULL;
	CPluginOption* objNewOption=new CPluginOption();
	objNewOption->m_pParent=szParent;
	objNewOption->m_pOption=new COptionColor(dwDefaultColor, &objNewOption->m_lValue);
	if(Lookup(szOptionName,objNewOptionOld)!=-1){_XLOG_
		objNewOption->m_lValue=objNewOptionOld->m_lValue;
		RemoveOption(szOptionName);
	}else{_XLOG_
		// Заполняем значением по умолчанию
		objNewOption->m_lValue=dwDefaultColor;
	}
	SetPluginDsc(objNewOption,szActionTitle,szOptionDescription,dwOptionPosition);
	objNewOption->sName=szOptionName;
	return options.Add(objNewOption);
	return -1;
}

int CPlugin::AddBoolOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long bDefaultValue, DWORD dwOptionPosition, const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* objNewOptionOld=NULL;
	CPluginOption* objNewOption=new CPluginOption();
	objNewOption->m_pParent=szParent;
	objNewOption->m_pOption=new COptionBOOL(bDefaultValue, &objNewOption->m_lValue);
	if(Lookup(szOptionName,objNewOptionOld)!=-1 && objNewOptionOld!=NULL){_XLOG_
		// Переносим значение из считанного
		objNewOption->m_sValue=objNewOptionOld->m_sValue;
		objNewOption->m_lValue=objNewOptionOld->m_lValue;
		RemoveOption(szOptionName);
	}else{_XLOG_
		// Заполняем значением по умолчанию
		objNewOption->m_lValue=bDefaultValue;
	}
	SetPluginDsc(objNewOption,szOptionTitle,szOptionDescription,dwOptionPosition);
	objNewOption->sName=szOptionName;
	return options.Add(objNewOption);
}

BOOL CPlugin::AddStringOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szDefaultValue, DWORD dwHint, DWORD dwOptionPosition)
{_XLOG_
	return AddStringOptionEx(szOptionName, szOptionTitle, szOptionDescription, szDefaultValue, dwHint, dwOptionPosition, NULL)!=-1;
}

int CPlugin::AddStringOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szDefaultValue, DWORD dwHint, DWORD dwOptionPosition,const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* objNewOptionOld=NULL;
	CPluginOption* objNewOption=new CPluginOption();
	objNewOption->m_pParent=szParent;
	objNewOption->m_pOption=new COptionString(szDefaultValue, &objNewOption->m_sValue);
	if(Lookup(szOptionName,objNewOptionOld)!=-1 && objNewOptionOld!=NULL){_XLOG_
		// Переносим значение из считанного
		objNewOption->m_sValue=objNewOptionOld->m_sValue;
		RemoveOption(szOptionName);
	}else{_XLOG_
		// Заполняем значением по умолчанию
		objNewOption->m_sValue=szDefaultValue;
	}
	objNewOption->m_lValue=dwHint;
	SetPluginDsc(objNewOption,szOptionTitle,szOptionDescription,dwOptionPosition);
	objNewOption->sName=szOptionName;
	return options.Add(objNewOption);
}

BOOL CPlugin::AddBinOption(const char* szOptionName)
{_XLOG_
	return AddStringOption(szOptionName,0,0,0,0,0);
}

BOOL CPlugin::AddNumberOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long lDefaultValue, long lMinValue, long lMaxValue, DWORD dwOptionPosition)
{_XLOG_
	return AddNumberOptionEx(szOptionName, szOptionTitle, szOptionDescription, lDefaultValue, lMinValue, lMaxValue, dwOptionPosition, NULL)!=-1;
}

int CPlugin::AddNumberOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long lDefaultValue, long lMinValue, long lMaxValue, DWORD dwOptionPosition, const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* objNewOptionOld=NULL;
	CPluginOption* objNewOption=new CPluginOption();
	objNewOption->m_pParent=szParent;
	objNewOption->m_pOption=new COptionNumber(lDefaultValue, &objNewOption->m_lValue,lMinValue,lMaxValue);
	if(Lookup(szOptionName,objNewOptionOld)!=-1 && objNewOptionOld!=NULL){_XLOG_
		// Переносим значение из считанного
		objNewOption->m_sValue=objNewOptionOld->m_sValue;
		objNewOption->m_lValue=objNewOptionOld->m_lValue;
		RemoveOption(szOptionName);
	}else{_XLOG_
		// Заполняем значением по умолчанию
		objNewOption->m_lValue=lDefaultValue;
	}
	SetPluginDsc(objNewOption,szOptionTitle,szOptionDescription,dwOptionPosition);
	objNewOption->sName=szOptionName;
	return options.Add(objNewOption);
}

BOOL CPlugin::AddListOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szTabDelimitedList, long lDefaultPosition, DWORD dwOptionPosition)
{_XLOG_
	return AddListOptionEx(szOptionName, szOptionTitle, szOptionDescription, szTabDelimitedList, lDefaultPosition, dwOptionPosition, NULL)!=-1;
}

int CPlugin::AddListOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szTabDelimitedList, long lDefaultPosition, DWORD dwOptionPosition, const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* objNewOptionOld=NULL;
	CPluginOption* objNewOption=new CPluginOption();
	objNewOption->m_pParent=szParent;
	objNewOption->m_pOption=new COptionCombo(lDefaultPosition, &objNewOption->m_lValue,szTabDelimitedList);
	if(Lookup(szOptionName,objNewOptionOld)!=-1 && objNewOptionOld!=NULL){_XLOG_
		// Переносим значение из считанного
		objNewOption->m_lValue=objNewOptionOld->m_lValue;
		RemoveOption(szOptionName);
	}else{_XLOG_
		// Заполняем значением по умолчанию
		objNewOption->m_lValue=lDefaultPosition;
	}
	objNewOption->m_sValue=szTabDelimitedList;
	SetPluginDsc(objNewOption,szOptionTitle,szOptionDescription,dwOptionPosition);
	objNewOption->sName=szOptionName;
	return options.Add(objNewOption);
}


BOOL CPlugin::AddTip(const char* szOptionName, const char* szTip,DWORD dwOptionPosition)
{_XLOG_
	return AddTipEx(szOptionName, szTip,dwOptionPosition, NULL)!=-1;
}

int CPlugin::AddTipEx(const char* szOptionName, const char* szTip,DWORD dwOptionPosition, const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	RemoveOption(szOptionName);
	CPluginOption* objNewOption=new CPluginOption();
	objNewOption->m_pParent=szParent;
	objNewOption->m_pOption=0;
	objNewOption->m_sValue=szTip;
	SetPluginDsc(objNewOption,szTip,"",dwOptionPosition);
	objNewOption->m_lValue=0;
	objNewOption->sName=szOptionName;
	return options.Add(objNewOption);
}

BOOL CPlugin::AddActionOption(const char* szOptionName, const char* szOptionTitle, long iPluginFunction, DWORD dwOptionPosition)
{_XLOG_
	return AddActionOptionEx(szOptionName, szOptionTitle, iPluginFunction, dwOptionPosition, NULL)!=-1;
}

int CPlugin::AddActionOptionEx(const char* szOptionName, const char* szOptionTitle, long iPluginFunction, DWORD dwOptionPosition, const char* szParent)
{_XLOG_
	if(szOptionName==NULL || strlen(szOptionName)==0){_XLOG_
		return FALSE;
	}
	CSmartLock SL(&csOptions,TRUE);
	RemoveOption(szOptionName);
	CPluginOption* objNewOption=new CPluginOption();
	objNewOption->m_pParent=szParent;
	objNewOption->m_pOption=new COptionAction(0, 0, (HIROW)iPluginFunction);
	SetPluginDsc(objNewOption,szOptionTitle,"",dwOptionPosition);
	objNewOption->m_lValue=iPluginFunction;
	objNewOption->sName=szOptionName;
	return options.Add(objNewOption);
}

long CPlugin::GetBoolOption(const char* szOptionName)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		return pOption->m_lValue;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return 0;
}

size_t CPlugin::GetStringOption(const char* szOptionName, char* szOut, size_t dwOutLen)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		mymemcpy(szOut,pOption->m_sValue,dwOutLen);
		return strlen(pOption->m_sValue);
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return 0;
}

long CPlugin::GetNumberOption(const char* szOptionName)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		return pOption->m_lValue;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return 0;
}

long CPlugin::GetListOption(const char* szOptionName)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		return pOption->m_lValue;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return -1;
}

long CPlugin::GetBinOption(const char* szOptionName, LPVOID pData, DWORD dwDataSize)
{_XLOG_
	if(pData==0){_XLOG_
		return 0;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		HGLOBAL hGlob=NULL;
		memset(pData,0,dwDataSize);
		PackFromString(pOption->m_sValue, dwDataSize, hGlob);
		char* pUnpackedData=(char*)::GlobalLock(hGlob);
		if(pUnpackedData==0){_XLOG_
			return 0;
		}
		memcpy(pData,pUnpackedData,dwDataSize);
		::GlobalFree(hGlob);
		return 1;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return 0;
}

BOOL CPlugin::SetBoolOption(const char* szOptionName, long lNewValue)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		if(pOption->m_lValue!=lNewValue){_XLOG_
			bOptionsWasChanged=TRUE;
			pOption->m_lValue=lNewValue;
		}
		return TRUE;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return FALSE;
}

BOOL CPlugin::SetStringOption(const char* szOptionName, const char* szNewValue)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		if(pOption->m_sValue!=szNewValue){_XLOG_
			bOptionsWasChanged=TRUE;
			pOption->m_sValue=szNewValue;
		}
		return TRUE;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return FALSE;
}

BOOL CPlugin::SetNumberOption(const char* szOptionName, long lNewValue)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		if(pOption->m_lValue!=lNewValue){_XLOG_
			bOptionsWasChanged=TRUE;
			pOption->m_lValue=lNewValue;
		}
		return TRUE;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return FALSE;
}

BOOL CPlugin::SetListOption(const char* szOptionName, long lNewValue)
{_XLOG_
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		if(pOption->m_lValue!=lNewValue){_XLOG_
			bOptionsWasChanged=TRUE;
			pOption->m_lValue=lNewValue;
		}
		return TRUE;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return FALSE;
}

BOOL CPlugin::SetBinOption(const char* szOptionName, LPVOID pData, DWORD dwDataSize)
{_XLOG_
	if(pData==0){_XLOG_
		return 0;
	}
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* pOption=NULL;
	if(Lookup(szOptionName,pOption)!=-1 && pOption!=NULL){_XLOG_
		CString sPacked=PackToString((const char*)pData, dwDataSize);
		if(pOption->m_sValue!=sPacked){_XLOG_
			bOptionsWasChanged=TRUE;
			pOption->m_sValue=sPacked;
		}
		return TRUE;
	}
#ifdef _DEBUG
	AfxMessageBox(Format("Warning: Undeclared option used: %s (%s)",szOptionName,sFile));
#endif
	return FALSE;
}

size_t CPlugin::TranslateText(const char* szToTranslate,char* szText, size_t dwTextSize)
{_XLOG_
	return GetTranslation(szToTranslate, szText, dwTextSize);
}

size_t CPlugin::GetBaseTranslation(const char* szToTranslate,char* szText, size_t dwTextSize)
{
	CString s=::_l(szToTranslate);
	strcpy(szText,s);
	return s.GetLength();
}

BOOL CPlugin::RemoveOption(const char* szOptionName)
{_XLOG_
	BOOL bRes=FALSE;
	CSmartLock SL(&csOptions,TRUE);
	CPluginOption* objOptionOld=NULL;
	int iPos=Lookup(szOptionName,objOptionOld);
	if(iPos!=-1){_XLOG_
		options.RemoveAt(iPos);
		bRes=TRUE;
	}
	if(objOptionOld!=NULL){_XLOG_
		delete objOptionOld;
	}
	return bRes;
}

size_t CPlugin::GetSelectedText(char* szText, size_t size)
{_XLOG_
	//pMainDialogWindow->SetCurrentDesktop();
	CString sText=GetSelectedTextInFocusWnd(1);
	if(sText==""){_XLOG_
		return NULL;
	}
	mymemcpy(szText,sText,size);
	return sText.GetLength();
}

BOOL CPlugin::PutText(const char* szText)
{_XLOG_
	//SetCurrentDesktop();
	FORK(PasteClipText,strdup(szText));
	return TRUE;
}

DWORD CPlugin::GetClipCount()
{_XLOG_
	return objSettings.sClipBuffer.GetSize();
}

size_t CPlugin::GetClipContent(DWORD dwClipNum, char* szText, size_t size)
{_XLOG_
	SimpleLocker l(&csClipBuffer);
	if(dwClipNum>=0 && dwClipNum<DWORD(objSettings.sClipBuffer.GetSize())){_XLOG_
		CString sText=objSettings.sClipBuffer[dwClipNum];
		if(sText==""){_XLOG_
			return 0;
		}
		mymemcpy(szText,sText,size);
		return sText.GetLength();
	}
	return 0;
}

BOOL CPlugin::SetClipContent(DWORD dwClipNum, const char* szContent)
{
	CString sText=szContent;
	if((int)dwClipNum < 0){
		USES_CONVERSION;
		BOOL bThroughGlobal=FALSE;
		SetClipboardText(A2W(szContent),bThroughGlobal,0);
		return TRUE;
	}
	return SetTextToClip(dwClipNum,sText);
}

BOOL CPlugin::ShowAlert(const char* szText, const char* szTitle)
{_XLOG_
	//SetCurrentDesktop();
	CString sAlertTitle=szTitle;
	CString sPluginName=this->_l(sTitle);
	if(szTitle && strstr(szTitle,sPluginName)==0){_XLOG_
		if(szTitle && strlen(szTitle)>0){_XLOG_
			sAlertTitle+=" (";
		}
		sAlertTitle+=sPluginName;
		if(szTitle && strlen(szTitle)>0){_XLOG_
			sAlertTitle+=")";
		}
	}
	Alert(szText, sAlertTitle);
	return TRUE;
}

size_t CPlugin::GetPreferencesFolder(char* szText, size_t size)
{_XLOG_
	CString sText=GetUserFolder();
	mymemcpy(szText,sText,size);
	return sText.GetLength();
}

size_t CPlugin::GetTranslation(const char* szToTranslate,char* szText, size_t size)
{_XLOG_
	CString sText=_getLngString(szToTranslate,NULL,GetPluginsDirectory(),sFileName);
	mymemcpy(szText,(const char*)sText,size);
	return sText.GetLength();
}

CString CPlugin::_l(const char* szText,const char* szDef)
{_XLOG_
	return _getLngString(szText,szDef,GetPluginsDirectory(),sFileName);
}

BOOL CPlugin::ProcessWindow(HWND hWnd, int iAction)
{_XLOG_
	if(!iAction || !IsWindow(hWnd) || !pMainDialogWindow){_XLOG_
		return FALSE;
	}
	pMainDialogWindow->HideApplication(hWnd,iAction);
	return TRUE;
}

BOOL CPlugin::ShowPopup(const char* szText, const char* szTitle, DWORD dwTimeOut)
{_XLOG_
	if(dwTimeOut==0){_XLOG_
		TRACE3("%s [%s] %s\n",sFileName,szTitle,szText);
	}else{_XLOG_
		ShowBaloon(szText, szTitle, dwTimeOut);
	}
	return TRUE;
}

DWORD WINAPI PerformTrayAction_InThread(LPVOID pData);
BOOL CPlugin::CallWKAction(int iAction)
{_XLOG_
	if(!pMainDialogWindow){_XLOG_
		return FALSE;
	}
	FORK(PerformTrayAction_InThread,iAction);
	return TRUE;
}

DWORD CPlugin::GetWKActionHKCode(int iAction)
{_XLOG_
	if(iAction<0 || iAction>LAST_ACTION_CODE){_XLOG_
		return FALSE;
	}
	return objSettings.DefActionMapHK[iAction].GetAsDWORD();
}

HWND CPlugin::GetWKMainWindow()
{_XLOG_
	if(!pMainDialogWindow){_XLOG_
		return NULL;
	}
	return pMainDialogWindow->GetSafeHwnd();
}

int CPlugin::GetWKState()
{_XLOG_
	if(!pMainDialogWindow){_XLOG_
		return FALSE;
	}
	return pMainDialogWindow->GetWKState();
}

size_t CPlugin::GetWKPluginsDirectory(char* szPath, size_t dwPathSize)
{_XLOG_
	CString sPath=GetPluginsDirectory();
	mymemcpy(szPath,sPath,dwPathSize);
	return sPath.GetLength();
}

BOOL CPlugin::AddQrunFromHWND(HWND hwnd)
{_XLOG_
	if(!pMainDialogWindow){_XLOG_
		return FALSE;
	}
	objSettings.hHotMenuWnd=hwnd;
	pMainDialogWindow->PostMessage(WM_COMMAND,ID_HOTMENU_ADDTOQUICKRUNS,0);
	return TRUE;
}

BOOL CPlugin::AddQrunFromFile(const char* szQRunFile)
{_XLOG_
	if(!pMainDialogWindow){_XLOG_
		return FALSE;
	}
	pMainDialogWindow->PostMessage(WM_COMMAND,ADDTOQUICKRUNS,LPARAM(strdup(szQRunFile)));
	return TRUE;
}

size_t CPlugin::SubstPatterns(char* szValue, size_t dwValueSize)
{_XLOG_
	CString sValue=szValue;
	SubstMyVariablesWK(sValue);
	mymemcpy(szValue,sValue,dwValueSize);
	return sValue.GetLength();
}

BOOL CPlugin::ShowOSD(const char* szText,DWORD dwTimeToShow)
{_XLOG_
	if(objSettings.lNoNeedOSD){_XLOG_
		return TRUE;
	}
	static long lNoRecurse=0;
	if(lNoRecurse){_XLOG_
		return FALSE;
	}
	SimpleTracker lc(lNoRecurse);
	if(szText==0 || *szText==0){_XLOG_
		if(pOSD){_XLOG_
			delete pOSD;
			pOSD=0;
		}
		return TRUE;
	}
	if(pOSD){_XLOG_
		pOSD->SetTextOnly(szText);
		if(IsWindow(pOSD->wnd->GetSafeHwnd())){_XLOG_
			pOSD->wnd->Invalidate();
		}
	}else{_XLOG_
		CSplashParams par;
		FillSplashParams(sTitle,par);
		par.szText=szText;
		pOSD=new CSplashWindow(&par);
	}
	pOSD->AutoClose(dwTimeToShow,&pOSD);
	return TRUE;
}

BOOL CPlugin::StartQuickRunByName(const char* szQRunTitle, const char* szAdditionalParameter)
{_XLOG_
	CString sMacro=szQRunTitle;
	if(sMacro.Find("[ID=")!=-1){
		sMacro=CDataXMLSaver::GetInstringPartGreedly("[ID=","]",sMacro);
	}
	if(sMacro.Find(SCRIPT_MACRO_PREF)==0){
		sMacro=sMacro.Mid(strlen(SCRIPT_MACRO_PREF));
		sMacro.TrimLeft();
		sMacro.TrimRight();
		ExecuteScript(sMacro);
		return TRUE;
	}
	return StartQRunByName(sMacro, szAdditionalParameter);
}

typedef BOOL (WINAPI *_DeleteFileNOW)(const char*);
BOOL WINAPI DeleteFileNOW(const char* sFile)
{_XLOG_
	__FLOWLOG;
	BOOL bRes=FALSE;
	HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils.dll");
	if(hUtils){_XLOG_
		_DeleteFileNOW fp=(_DeleteFileNOW)DBG_GETPROC(hUtils,"DeleteFileNOW");
		if(fp){_XLOG_
			bRes=(*fp)((const char*)sFile);
		}
	}
	FreeLibrary(hUtils);
	return bRes;
}

BOOL CPlugin::PauseWKCompletely(BOOL bPause)
{_XLOG_
	TRACE2("WK PAUSE: %i (curstate=%i)\n",bPause,WKUtils::isWKUpAndRunning());
	return PauseAllWK(bPause);
}

#include "..\SmartWires\SystemUtils\Lik\checksum.h"
void CPlugin::getMD5(char* szOut, BYTE* pFrom, size_t size)
{
	strcpy(szOut,MD5_HexBytes(pFrom,size));
}

void CPlugin::SaveScreenshotTo(char* szPath, int iType)
{
	SaveScreenshotToFile(szPath, iType);
}

HINSTANCE CPlugin::GetPluginDllInstance()
{_XLOG_
	return hInst;
}

BOOL CPlugin::ShowPreferencesTopic(const char* szTitle_EN)
{_XLOG_
	CString sTopTitle=szTitle_EN!=NULL?_l(szTitle_EN):_l(sTitle);
	objSettings.OpenOptionsDialog(0,sTopTitle,pMainDialogWindow);
	return TRUE;
}

BOOL CPlugin::CallWKActionX(int iAction, const char* szParameter)
{_XLOG_
	if(iAction<0){_XLOG_
		return DeleteFileNOW(szParameter);
	}
	return TRUE;
}

BOOL ExecuteMacroByContent(CString& sMacrosContent,CString& sMacrosFile, int iEntryPoint, CString sAddPAram, BOOL bSelectionSubst, CString sSelectionSubst, CQuickRunItem* pW, HANDLE hThis);
BOOL ExecutePluginScript(const char* szText)
{
	CString sMacrosContent=szText;
	CString sMacrosFile="";
	BOOL bSendkeys=sMacrosContent.Replace("##SENDKEY_USE#!#","");
	if(bSendkeys){
		CString s="";
		s+="var WshShell = new ActiveXObject('WScript.Shell');\r\n";
		sMacrosContent.Replace("{","{{###BLBLBLBLB$NOWAYThisShitcAnBEINSCRipt$$###");// Сначала!!!
		sMacrosContent.Replace("}","{}}");// Сначала!!!
		sMacrosContent.Replace("###BLBLBLBLB$NOWAYThisShitcAnBEINSCRipt$$###","}");// Сначала!!!
		sMacrosContent.Replace("\r","");
		sMacrosContent.Replace("\n","{ENTER}");
		sMacrosContent.Replace("+","{+}");
		sMacrosContent.Replace("^","{^}");
		sMacrosContent.Replace("%","{%}");
		sMacrosContent.Replace("~","{~}");
		sMacrosContent.Replace("(","{(}");
		sMacrosContent.Replace(")","{)}");
		sMacrosContent.Replace("[","{[}");
		sMacrosContent.Replace("]","{]}");
		sMacrosContent.Replace("$","{$}");
		//WireKeys.Alert(unescape('"+EscapeString3(sMacrosContent)+"'));
		s+="WshShell.SendKeys(unescape('"+EscapeString3(sMacrosContent)+"'));\r\n/*#SILENCE*/";
		sMacrosContent=s;
	}
	ExecuteMacroByContent(sMacrosContent,sMacrosFile,0,"",0,"",0,0);
	return TRUE;
}

BOOL CPlugin::ExecuteScript(const char* szText)
{
  return ExecutePluginScript(szText);
}

BOOL CPlugin::StartQuickRunByNameX(const char* szQRunTitle, const char* szAdditionalParameter, const char* szSelectionSubstitution, int iEntryNum)
{_XLOG_
	CString sMacro=szQRunTitle;
	if(sMacro.Find("[ID=")!=-1){_XLOG_
		sMacro=CDataXMLSaver::GetInstringPartGreedly("[ID=","]",sMacro);
	}
	CString sAdd=szAdditionalParameter;
	CString sSel=szSelectionSubstitution;
	return StartQRunByName(sMacro, sAdd, sSel);
}

BOOL CPlugin::ShowAppInfo(HWND hWnd)
{_XLOG_
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->PostMessage(WM_COMMAND,APPINFO,LPARAM(hWnd));
	}
	return TRUE;
}

BOOL CPlugin::PushMyOptions(DWORD dwParameter)
{_XLOG_
	if(fOptionsManager){_XLOG_
		// Сначала переносим настройки если что...
		(*fOptionsManager)(OM_OPTIONS_GET,this,dwParameter);
		if(long(dwParameter)>=0){_XLOG_
			bOptionsWasChanged=TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CPlugin::ShowWKMenu(HWND hWnd)
{_XLOG_
	if(!pMainDialogWindow){_XLOG_
		return FALSE;
	}
	while(GetAsyncKeyState(VK_RBUTTON)<0){_XLOG_
		Sleep(200);
	}
	pMainDialogWindow->m_STray.hLastActiveWindow=hWnd;
	pMainDialogWindow->SendMessage(WM_COMMAND,ID_SYSTRAY_SHOWWKMENU,0);
	return TRUE;
}

BOOL CPlugin::GetWKVersion(char szVersion[32])
{_XLOG_
	strcpy(szVersion,PROG_VERSION);
	szVersion[30]=LOBYTE(PLUGINAPI_VER);
	szVersion[31]=HIBYTE(PLUGINAPI_VER);
	return TRUE;
}

BOOL CPlugin::GetWKActionHKDesc(int iAction, char szAsString[32])
{_XLOG_
	if(iAction<0 || iAction>LAST_ACTION_CODE){_XLOG_
		return FALSE;
	}
	strcpy(szAsString,getActionHotKeyDsc(iAction,"","",""));
	return TRUE;
}

BOOL CPlugin::RegisterEvent(char szEventID[5], char szShortDescription[64], char szEventClassName[64])
{_XLOG_
	WKIHOTKEYS_RegisterExternEvent(szEventID, szShortDescription, szEventClassName);
	return TRUE;
}

BOOL CPlugin::UnRegisterEvent(char szEventID[5])
{_XLOG_
	WKIHOTKEYS_UnRegisterExternEvent(szEventID);
	return TRUE;
}

BOOL CPlugin::NotifyEventStarted(char szEventID[5])
{_XLOG_
	WKIHOTKEYS_ExternEventStart(szEventID);
	return TRUE;
}

BOOL CPlugin::NotifyEventFinished(char szEventID[4])
{_XLOG_
	WKIHOTKEYS_ExternEventStop(szEventID);
	return TRUE;
}

BOOL CPlugin::RegisterOnAddRemoveEventCallback(FPONEVENTADD fpAdd,FPONEVENTREMOVE fpRemove)
{_XLOG_
	WKIHOTKEYS_RegisterOnAddRemoveEventCallback(fpAdd,fpRemove);
	return TRUE;
}

BOOL CPlugin::SubmitToSystemEventNotification(int iEventType, FPONSYSTEMEVENT fp)
{_XLOG_
	AddSysEventSubmit(iEventType, fp);
	return TRUE;
}

BOOL CPlugin::UnSubmitToSystemEventNotification(int iEventType, FPONSYSTEMEVENT fp)
{_XLOG_
	RemSysEventSubmit(iEventType, fp);
	return TRUE;
}

extern CArray<int,int> aEventCountByType;
BOOL CPlugin::NotifyAboutSystemEvent(CWKSYSEvent* pEvent)
{
	return GlobalNotifyAboutSystemEvent(pEvent);
}

BOOL GlobalNotifyAboutSystemEvent(CWKSYSEvent* pEvent)
{_XLOG_
	if(pEvent && pEvent->iEventType==WKSYSEVENTTYPE_MOUSEMOVE){_XLOG_
		// Статистика - в пикселах автопробег
		static long lPrevX=0;
		static long lPrevY=0;
		if(lPrevX==0){_XLOG_
			lPrevX=pEvent->iCurPosX;
		}
		if(lPrevY==0){_XLOG_
			lPrevY=pEvent->iCurPosY;
		}
		objSettings.lStat_mouseLen+=long(sqrt(double(((lPrevX-pEvent->iCurPosX)*(lPrevX-pEvent->iCurPosX)
			+(lPrevY-pEvent->iCurPosY)*(lPrevY-pEvent->iCurPosY)))));
		lPrevX=pEvent->iCurPosX;
		lPrevY=pEvent->iCurPosY;
	}
	if(pEvent && pEvent->iEventType==WKSYSEVENTTYPE_CHAR){_XLOG_
		objSettings.lStat_keybCount++;
	}
	if(!pEvent || pEvent->iEventType<0 || pEvent->iEventType>=aEventCountByType.GetSize()){_XLOG_
		return 0;
	}
	if(aEventCountByType[pEvent->iEventType]==0){_XLOG_
		return 0;
	}
	__try{_XLOG_
		DWORD dw1=GetTickCount();
		OnActivateEvent(pEvent);
		DWORD dw2=GetTickCount();
		if((dw2-dw1)>5000){_XLOG_
			// Ахтунг! бросаем все!
			aEventCountByType.SetAt(pEvent->iEventType,0);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		return 0;
	}
	return 1;
}

BOOL CPlugin::WaitWhileAllKeysAreReleased()
{_XLOG_
	WaitWhileAllKeysAreFree();
	return TRUE;
}

DWORD dwShellExID=0;
LPVOID CPlugin::setOptionEx(DWORD dwOption,DWORD dwParameter)
{_XLOG_
	
	if(dwOption==1346){_XLOG_
		dwShellExID=dwParameter;
	}
	return 0;
}

LPVOID CPlugin::getOptionEx(DWORD dwOption,DWORD dwParameter)
{_XLOG_
	if(dwOption==1346){_XLOG_
		return (LPVOID)dwShellExID;
	}
	if(dwOption==0){_XLOG_
		return (LPVOID)CUITranslationImpl::getInstance();
	}
	if(dwOption==1){_XLOG_
		return &funcs[dwParameter].oRealHotkey;
	}
	if(dwOption==2){_XLOG_
		HWND hRes=pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():0;
		if(objSettings.pOpenedOptions){_XLOG_
			hRes=objSettings.pOpenedOptions->GetSafeHwnd();
		}
		return (LPVOID)(hRes);
	}
	return 0;
}

BOOL CPlugin::getOption(DWORD dwOption)
{_XLOG_
	BOOL bRes=0;
	if(dwOption==WKOPT_WKINITIALIZED){_XLOG_
		bRes=pMainDialogWindow?(pMainDialogWindow->iRunStatus==1):0;
	}
	if(dwOption==WKOPT_EXTHKPROC){_XLOG_
		bRes=wk.isPluginLoaded("WP_KeyMaster");
	}
	if(dwOption==WKOPT_ISSHUTDOWN){_XLOG_
		bRes=objSettings.bShutIP;
	}
	if(dwOption==WKOPT_AUTOTEXT){_XLOG_
		bRes=wk.isPluginLoaded("WP_Autotext");
	}
	if(long(dwOption)==-1){_XLOG_
		bRes=objSettings.bPreserveTitle;
	}

	return bRes;
}

BOOL CPlugin::ChangeItemIcon(const char* szItemPath)
{_XLOG_
	pMainDialogWindow->PostMessage(WM_COMMAND,CHANGEICON,LPARAM(strdup(szItemPath)));
	return TRUE;
}

BOOL CPlugin::triggerTweak(DWORD dwOption)
{_XLOG_
	if(dwOption==1){_XLOG_
		objSettings.bIgnoreNextClipAction=1;
	}
	if(dwOption==2){
		return GetApplicationLang();
	}
	if(dwOption==3){
		return spyMode();
	}
	return 1;
}

BOOL CPlugin::ShowBaloon(const char* szText,const char* szTitle,DWORD dwTimeToShow)
{_XLOG_
	::ShowBaloon(szText,szTitle,dwTimeToShow);
	return TRUE;
}

CPluginOption* CPlugin::GetOptionByName(const char* szName)
{_XLOG_
	for(int i=0;i<(options.GetSize());i++){_XLOG_
		CPluginOption* pValue=options[i];
		if(pValue && pValue->sName==szName){_XLOG_
			return pValue;
		}
	}
	return NULL;
}

BOOL CPlugin::Sound(int iType)
{_XLOG_
	AsyncPlaySound(SND_ERROR);
	return TRUE;
}

BOOL CPlugin::ShowPluginPrefernces()
{_XLOG_
	int ir=GetWKState();
	if(ir==3){_XLOG_
		return 0;
	}
	pMainDialogWindow->PostMessage(WM_COMMAND,OPT_TOPIC,(LPARAM)strdup(this->_l(this->sTitle)));
	return 1;
}

extern CCriticalSection pCahLock;
BOOL CPlugin::TestRunningApplicationsByMask(const char* szMask)
{_XLOG_
__FLOWLOG;
	if(!szMask || szMask[0]==0){_XLOG_
		return FALSE;
	}
	if(!EnumProcesses()){_XLOG_
		return FALSE;
	}
	CString sMask=szMask;
	sMask.MakeLower();
	CCSLock lpc(&pCahLock,1);
	for(int iProcNum=0; iProcNum<GetProccessCash().GetSize();iProcNum++){_XLOG_
		CProcInfo& info=GetProccessCash()[iProcNum];
		CString sTitle=info.sProcName;
		if(sTitle!=""){_XLOG_
			sTitle.MakeLower();
			if(PatternMatchList(sTitle,sMask)){_XLOG_
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CPlugin::ReturnKeyboardFocusToActiveApp(WKPluginFunctionStuff* stuff)
{_XLOG_
	if(stuff){_XLOG_
		objSettings.hHotMenuWnd=stuff->hForegroundWindow;
		objSettings.hFocusWnd=stuff->hCurrentFocusWnd;
	}
	SetMainWindowForeground(objSettings.hHotMenuWnd,objSettings.hHotMenuWnd,&objSettings.hFocusWnd);
}

BOOL CPlugin::GetRegSetting(const char* szRegSettingsName, char* szOut, size_t dwOutSize)
{_XLOG_
	return ::GetRegSetting(sFileName,szRegSettingsName, szOut, dwOutSize);
}

BOOL CPlugin::SetRegSetting(const char* szRegSettingsName, char* szOut)
{_XLOG_
	return ::SetRegSetting(sFileName, szRegSettingsName, szOut);
}

BOOL CPlugin::PlayWavSound(const char* szWav)
{_XLOG_
	AsyncPlaySound(szWav);
	return TRUE;
} 
 