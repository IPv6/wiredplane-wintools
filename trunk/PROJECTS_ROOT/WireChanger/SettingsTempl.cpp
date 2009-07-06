#include "stdafx.h"
#include <process.h>
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRect rtTargetRect(0,0,0,0);
HANDLE hCloseAlertEvent=0;
BOOL CALLBACK SetPosFinished(void* Param, int iButtonNum, HWND hThisWindow)
{
	if(hThisWindow){
		::GetWindowRect(hThisWindow,&rtTargetRect);
		AdjustToSceenCoords(rtTargetRect,FALSE);
	}
	SetEvent(hCloseAlertEvent);
	return TRUE;
}

BOOL ShowSetPosDialog(CString sTitle,CWnd* pParent,long& lX, long& lY, long& lW, long& lH)
{
	InfoWndStruct* info=new InfoWndStruct();
	info->sText=_l("This window is placed in the exact location of the wallpaper element");
	if(sTitle!=""){
		info->sText+=" '";
		info->sText+=sTitle;
		info->sText+="'";
	}
	info->sText+="\n";
	info->sText+=_l("You can move and resize this window to place element in the desired location");
	info->sText+="\n";
	info->sText+=_l("Click 'Save and Close' to close this window when you finish");
	info->sTitle="";
	if(sTitle!=""){
		info->sTitle+=" '";
		info->sTitle+=sTitle;
		info->sTitle+="': ";
	}
	info->sTitle+=_l("Select size and position");
	info->sOkButtonText=_l("Save and close");
	info->bCenterUnderMouse=FALSE;
	info->dwStaticPars=DT_CENTER|DT_WORDBREAK;
	info->rtStart.left=lX;
	info->rtStart.top=lY;
	if(lW==0){
		lW=lH;
	}
	if(lH==0){
		lH=lW;
	}
	info->rtStart.right=lX+lW;
	info->rtStart.bottom=lY+lH;
	AdjustToSceenCoords(info->rtStart);
	info->bPlaceOnTaskbarAnyway=TRUE;
	info->bHideInfoTitler=TRUE;
	info->pParent=pParent;
	rtTargetRect.SetRect(0,0,0,0);
	hCloseAlertEvent=CreateEvent(0,0,0,0);
	if(pParent){
		pParent->EnableWindow(FALSE);
	}
	InfoWnd* pTargWnd=Alert(info);
	if(!pTargWnd){
		return FALSE;
	}
	pTargWnd->SetCallback(SetPosFinished,0);
	WaitForSingleObject(hCloseAlertEvent,INFINITE);
	CloseHandle(hCloseAlertEvent);
	if(pParent){
		pParent->EnableWindow(TRUE);
		//pParent->ShowWindow(SW_SHOW);
		//pParent->SetForegroundWindow();
		SwitchToWindow(pParent->GetSafeHwnd(),TRUE);
		/*
		::SetForegroundWindow(hActivate);
		::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)hActivate);
		*/
	}
	if(!rtTargetRect.IsRectEmpty() && pTargWnd->bCloseReason!=IDCANCEL){
		lX=rtTargetRect.left;
		lY=rtTargetRect.top;
		lW=rtTargetRect.Width();
		lH=rtTargetRect.Height();
	}
	return TRUE;
}

class CWPTOptionList
{
public:
	static long lEmptyLong;
	CMapStringToString aStringsHolder;
	CMap<CString,const char*,long,long&> aLongHolder;
	CMap<CString,const char*,long,long&> aColorHolder;
	CMap<CString,const char*,COleDateTime,COleDateTime&> aDateHolder;
	CMap<CString,const char*,CFont*,CFont*> aFontHolder;
	CFont*& AddOptionHolder_Font(const char* szName, const char* sStartVal)
	{
		CString sLine=sStartVal;
		sLine.Replace("[","<");
		sLine.Replace("]",">");
		aFontHolder.SetAt(szName,CreateFontFromStr(sLine, NULL, NULL, NULL, NULL));
		return aFontHolder[szName];
	}
	
	COleDateTime* AddOptionHolder_Date(const char* szName, const char* lStartVal, const char* szOptionalString=0)
	{
		aDateHolder.SetAt(szName,CDataXMLSaver::Str2OleDate(lStartVal));
		return &aDateHolder[szName];
	}

	long* AddOptionHolder_Long(const char* szName, long lStartVal, const char* szOptionalString=0)
	{
		aLongHolder.SetAt(szName,lStartVal);
		if(szOptionalString!=0){
			aStringsHolder.SetAt(CString("LIST_")+szName,szOptionalString);
		}
		return &aLongHolder[szName];
	}
	long* AddOptionHolder_Color(const char* szName, long lStartVal)
	{
		aColorHolder.SetAt(szName,lStartVal);
		return &aColorHolder[szName];
	}
	CString* AddOptionHolder_String(const char* szName, const char* szStartVal)
	{
		aStringsHolder.SetAt(szName,szStartVal);
		return &aStringsHolder[szName];
	}
	CString GetOptionHolder_String(const char* szName)
	{
		CString sVal;
		if(aStringsHolder.Lookup(szName,sVal)){
			return sVal;
		}
		return "";
	}
	long& GetOptionHolder_Long(const char* szName)
	{
		long lVal;
		if(aLongHolder.Lookup(szName,lVal)){
			return aLongHolder[szName];
		}
		lEmptyLong=0;
		return lEmptyLong;
	}
	long& GetOptionHolder_Color(const char* szName)
	{
		long lVal;
		if(aColorHolder.Lookup(szName,lVal)){
			return aLongHolder[szName];
		}
		lEmptyLong=0;
		return lEmptyLong;
	}
	void FreeMemory()
	{
		aStringsHolder.RemoveAll();
		aLongHolder.RemoveAll();
		aColorHolder.RemoveAll();
		// Шрифты...
		CString sName;
		CFont* pFont=0;
		POSITION pos=aFontHolder.GetStartPosition();
		while(pos){
			aFontHolder.GetNextAssoc(pos,sName,pFont);
			delete pFont;
		}
		aFontHolder.RemoveAll();
	}
	~CWPTOptionList()
	{
		FreeMemory();
	};
} aWPTOptions;

CString sWPTemplateTitle="";
CString sWPTemplateSource="";
CStringArray aActionsStrings;
long CWPTOptionList::lEmptyLong=0;
BOOL CALLBACK TemplateActions(HIROW hData, CDLG_Options* pDialog)
{
	int iAction=int(hData);
	if(iAction<0 || iAction>=aActionsStrings.GetSize()){
		return FALSE;
	}
	CString sString=aActionsStrings[iAction];
	CallFileForOpenOrEdit(sString);
	return TRUE;
}

//CDLG_Options* g_pDialogTmp=0;
HANDLE TemplateActions_SetPosEvent=0;
struct CTemplateActions_SetPos_InThread
{
	int m_iAction;
	CDLG_Options* m_pDialog;
	CWPT* pWpt;
};

DWORD WINAPI TemplateActions_SetPos_InThread(LPVOID hData)
{
	CTemplateActions_SetPos_InThread* p=(CTemplateActions_SetPos_InThread*)hData;
	if(!p){
		return 0;
	}
	int iAction=p->m_iAction;
	CDLG_Options* pDialogTmp=p->m_pDialog;
	CWPT* wpt=p->pWpt;
	delete p;
	if(iAction<0 || iAction>=aActionsStrings.GetSize()){
		return FALSE;
	}
	if(pDialogTmp){
		//pDialogTmp->Lock
	}
	if(TemplateActions_SetPosEvent!=0){
		ResetEvent(TemplateActions_SetPosEvent);
	}
	CString sString=aActionsStrings[iAction];
	DWORD dwOC=atol(CDataXMLSaver::GetInstringPart("OC:(",")",sString));
	if(dwOC && pDialogTmp){
		pDialogTmp->CommitData(dwOC);
	}
	CString sXName=CDataXMLSaver::GetInstringPart("X:(",")",sString);
	CString sYName=CDataXMLSaver::GetInstringPart("Y:(",")",sString);
	CString sWName=CDataXMLSaver::GetInstringPart("W:(",")",sString);
	CString sHName=CDataXMLSaver::GetInstringPart("H:(",")",sString);
	CWnd* wndp=pDialogTmp;
	if(wndp==0){
		if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
			wndp=pMainDialogWindow->dlgWChanger;
		}
	}
	ShowSetPosDialog(wpt?(wpt->GetFTitle()):"",wndp,aWPTOptions.GetOptionHolder_Long(sXName),aWPTOptions.GetOptionHolder_Long(sYName),aWPTOptions.GetOptionHolder_Long(sWName),aWPTOptions.GetOptionHolder_Long(sHName));
	if(dwOC && pDialogTmp){
		pDialogTmp->PushSomeDataOnScreen(dwOC);
	}
	if(TemplateActions_SetPosEvent!=0){
		SetEvent(TemplateActions_SetPosEvent);
	}
	if(pDialogTmp){
		//pDialogTmp->UnLock
	}
	return TRUE;
}

BOOL CALLBACK TemplateActions_SetPos(HIROW hData, CDLG_Options* pDialog)
{
//	g_pDialogTmp=pDialog;
	TemplateActions_SetPosEvent=CreateEvent(0,0,0,0);
	CTemplateActions_SetPos_InThread* p=new CTemplateActions_SetPos_InThread;
	p->m_iAction=(int)hData;
	p->m_pDialog=pDialog;
	p->pWpt=0;
	FORK(TemplateActions_SetPos_InThread,p);
	if(pDialog==0 && TemplateActions_SetPosEvent){
		WaitForSingleObject(TemplateActions_SetPosEvent,INFINITE);
	}
	if(TemplateActions_SetPosEvent!=0){
		CloseHandle(TemplateActions_SetPosEvent);
		TemplateActions_SetPosEvent=0;
	}
//	g_pDialogTmp=0;
	return TRUE;
}

CString sOptionWndTitle;
CString g_CurrentEditingFile;
BOOL g_CurrentEditingClocks=0;
int g_CurrentEditingIndex=0;
BOOL CALLBACK ApplyWOptionsFromDialog(BOOL bSaveAndClose,DWORD dwRes,CDLG_Options* dlgOpt)
{
	BOOL bRes=FALSE;
	if(dwRes==IDOK){
		objSettings.bTemplatesInWorkChanged=1;
		// Сохраняем в файл конфигурации
		CString sConfigFile=GetHTMLConfigFile(g_CurrentEditingFile,objSettings.sActivePreset);
		CString sConfig=Format("$=<Configuration file for '%s'>\r\n",g_CurrentEditingFile);
		{// Заменяем списочные/числовые значения новыми...
			POSITION pos=aWPTOptions.aLongHolder.GetStartPosition();
			CString sKey,sTempVal;
			long lVal;
			while(pos){
				aWPTOptions.aLongHolder.GetNextAssoc(pos,sKey,lVal);
				sConfig+=Format("$=<PARVAL:%s=%i>\r\n",sKey,lVal);
				if(aWPTOptions.aStringsHolder.Lookup(CString("LIST_")+sKey,sTempVal)){
					// Это номер из списка!!!
					CStringArray sItemsData;
					ConvertComboDataToArray(sTempVal,sItemsData,'\t');
					if(lVal>=0 && lVal<sItemsData.GetSize()){
						CString sVal=sItemsData[lVal];
						sConfig+=Format("$=<PARVAL:%s-PATH=%s>\r\n",sKey,sVal);
					}
				}
			}
		}
		{// Заменяем строковые значения новыми...
			POSITION pos=aWPTOptions.aStringsHolder.GetStartPosition();
			CString sKey,sVal;
			while(pos){
				aWPTOptions.aStringsHolder.GetNextAssoc(pos,sKey,sVal);
				sConfig+=Format("$=<PARVAL:%s=%s>\r\n",sKey,sVal);
			}
		}
		{// Заменяем датовые значения новыми...
			POSITION pos=aWPTOptions.aDateHolder.GetStartPosition();
			CString sKey;
			COleDateTime sVal;
			while(pos){
				aWPTOptions.aDateHolder.GetNextAssoc(pos,sKey,sVal);
				sConfig+=Format("$=<PARVAL:%s=%s>\r\n",sKey,CDataXMLSaver::OleDate2Str(sVal));
			}
		}
		{// Заменяем цветовые значения новыми...
			POSITION pos=aWPTOptions.aColorHolder.GetStartPosition();
			CString sKey;
			long lVal;
			while(pos){
				aWPTOptions.aColorHolder.GetNextAssoc(pos,sKey,lVal);
				DWORD dwColor=(DWORD)lVal;
				sConfig+=Format("$=<PARVAL:%s=#%02X%02X%02X>\r\n",sKey,GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor));
			}
		}
		{// Заменяем шрифты значения новыми...
			POSITION pos=aWPTOptions.aFontHolder.GetStartPosition();
			CString sKey;
			CFont* pFont;
			while(pos){
				aWPTOptions.aFontHolder.GetNextAssoc(pos,sKey,pFont);
				CString sLine=CreateStrFromFont(pFont,"",0,0,0,0,TRUE);
				sLine.Replace("<","[");
				sLine.Replace(">","]");
				sConfig+=Format("$=<PARVAL:%s=%s>\r\n",sKey,sLine);
			}
		}
		SaveFile(sConfigFile,sConfig);
		// Обновляем обоину
		if(pMainDialogWindow && (g_CurrentEditingIndex<0 || g_CurrentEditingIndex>objSettings.aLoadedWPTs.GetSize()-1 || IsWPTActive(objSettings.aLoadedWPTs[g_CurrentEditingIndex]))){
			if(!objSettings.lMainWindowOpened){
				pMainDialogWindow->OrderModifierForNextWallpaper=-2;
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
			}else{
				pMainDialogWindow->dlgWChanger->bNeddRefreshAfterClose=1;
			}
		}
	}
	return bRes;
}

BOOL CALLBACK EnDisTemplSep(HIROW hData, CDLG_Options* pDialog)
{
	CString sCurrentWidget=objSettings.aLoadedWPTs[int(hData)];
	SwitchSingleTemplate(sCurrentWidget,TRUE);
	//pDialog->PostMessage(WM_COMMAND,IDCANCEL,0);
	if(IsWPTActive(sCurrentWidget)){
		Alert(CString("'")+GetWPTTitle(sCurrentWidget)+"': "+_l("Enabled"));
	}else{
		Alert(CString("'")+GetWPTTitle(sCurrentWidget)+"': "+_l("Disabled"));
	}
	return TRUE;
}


BOOL CALLBACK DeleteTemplSep(HIROW hData, CDLG_Options* pDialog)
{
	if(AfxMessageBox(_l("Do you really want to delete this template?\nTemplate file will be removed from your disk too"),MB_YESNO|MB_ICONQUESTION)!=IDYES){
		return FALSE;
	}
	RemoveTemplate(int(hData));
	pDialog->PostMessage(WM_COMMAND,IDCANCEL,0);
	return TRUE;
}

BOOL CALLBACK CloneTempl(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK AddWTemplateOptions(CDLG_Options* pData)
{
	CDLG_Options* pDialog=pData;
	if(!pDialog){
		return FALSE;
	}
	//
	aActionsStrings.RemoveAll();
	pDialog->bShowColorForColorOptions=FALSE;
	/*
	pDialog->m_OptionsList.m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
	pDialog->m_OptionsList.m_crIRowBackground=GetSysColor(COLOR_WINDOW);
	pDialog->m_OptionsList.SetBkColor(GetSysColor(COLOR_WINDOW));
	pDialog->m_OptionsList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);
	*/
	pDialog->m_OptionsList.m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
	pDialog->m_OptionsList.m_crIRowBackground=GetSysColor(COLOR_BTNFACE);
	pDialog->m_OptionsList.SetBkColor(GetSysColor(COLOR_BTNFACE));
	pDialog->m_OptionsList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);

	CString sW98Warn=CDataXMLSaver::GetInstringPart("WIN98WARN=(",")",sWPTemplateSource);
	////////////////////////
	// Задаем список опций
	////////////////////////
	HIROW hRoot=FL_ROOT;
	CMapStringToPtr aRowMap;
	int iFrom=0,iFromPar=0,iCount=0;
	if(sWPTemplateSource.Find("{wp:define")!=-1){
		CString sTagValue,sWPPar;
		while((iFrom=extTag("wp:define",sWPTemplateSource,&sTagValue))!=-1){
			sWPPar=CDataXMLSaver::GetInstringPart("[","]",sTagValue);
			CString sWPParDefVal=_atr(Format("[%s]",sWPPar),sTagValue);
			CString sWPParDsc=_atr("title",sTagValue);
			if(sWPParDsc.IsEmpty()){
				sWPParDsc=sWPPar;
			}
			CString sWPParDetails=_atr("description",sTagValue);
			if(sWPParDetails.IsEmpty()){
				sWPParDetails=_atr("desc",sTagValue);
			}
			if(!sWPParDetails.IsEmpty()){
				sWPParDetails=CString("\t")+_l(sWPParDetails);
			}
			// Это пока по старому...
			int iFromPar=0;
			CString sWPParValue=CDataXMLSaver::GetInstringPart(Format("$=<PARVAL:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			if(iFromPar==-1){
				sWPParValue=sWPParDefVal;
			}
			CString sWPParType=_atr("type",sTagValue);
			sWPParType.MakeLower();
			CString sWPParList=_atr("items",sTagValue);
			CString sWPParMin=_atr("min",sTagValue);
			CString sWPParMax=_atr("max",sTagValue);
			if(sWPParMax.IsEmpty()){
				sWPParMax="9999";
			}
			CString sWPParErr=_atr("onerror",sTagValue);
			CString sWPParAction=_atr("action",sTagValue);
			CString sWPParOC=_atr("oc",sTagValue);
			CString sWPParPar=_atr("parent",sTagValue);
			void* pMapValue=NULL;
			HIROW hInsertPoint=NULL, hInserted=NULL;
			if(sWPParPar!=""){
				aRowMap.Lookup(sWPParPar, pMapValue);
				hInsertPoint=(HIROW)pMapValue;
			}
			if(hInsertPoint==NULL){
				hInsertPoint=hRoot;
			}
			if(sWPParType=="list"){
				sWPParList=_l(sWPParList);
				sWPParList.Replace(",","\t");
				hInserted=pDialog->Option_AddCombo(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal),sWPParList);
				iCount++;
			}else if(sWPParType=="hidden"){
				// Nothing to do
			}else if(sWPParType=="dirlist"){
				CString sList,sFileList;
				if(sWPParList.Find("HKLM:")!=-1 || sWPParList.Find("HKCU:")!=-1){
					CRegKey key;
					CString sPath=sWPParList.Mid(strlen("????:"));
					BOOL bRes=0;
					if(sWPParList.Find("HKLM:")!=-1 && key.Open(HKEY_LOCAL_MACHINE, sPath)==ERROR_SUCCESS && key.m_hKey!=NULL){
						bRes=1;
					}
					if(sWPParList.Find("HKCU:")!=-1 && key.Open(HKEY_CURRENT_USER, sPath)==ERROR_SUCCESS && key.m_hKey!=NULL){
						bRes=1;
					}
					int iAdded=0;
					if(bRes){
						// Список!
						iCount=0;
						char szKeysNameBuf[256]={0};
						CString sChangedKey="";
						DWORD dwBufSize=sizeof(szKeysNameBuf);
						FILETIME pFileTime;
						memset(&pFileTime,0,sizeof(pFileTime));
						while(RegEnumKeyEx(key.m_hKey,iCount++,szKeysNameBuf,&dwBufSize,0,0,0,&pFileTime)==ERROR_SUCCESS){
							CString sPathK=szKeysNameBuf;
							sFileList+=sPath+"\\"+sPathK+"\t";
							int iLP=sPathK.ReverseFind('\\');
							if(iLP!=-1){
								sList+=sPathK.Mid(iLP+1)+"\t";
							}else{
								sList+=sPathK+"\t";
							}
							szKeysNameBuf[0]=0;
							dwBufSize=sizeof(szKeysNameBuf);
						}
					}
				}else{
					CString sMask="*.def";
					int iLastSlPos=sWPParList.ReverseFind('\\');
					if(iLastSlPos==-1){
						iLastSlPos=sWPParList.ReverseFind('/');
					}
					if(iLastSlPos!=-1){
						sMask=sWPParList.Mid(iLastSlPos+1);
						sWPParList=sWPParList.Left(iLastSlPos+1);
					}
					CFileInfoArray dir;
					dir.AddDir(sWPParList,sMask,TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
					for (int i=0;i<dir.GetSize();i++){
						CString sFile=dir[i].GetFilePath();
						if(sFile!="" && sFile.Find(".def")!=-1){
							CString sName;
							ReadFile(sFile,sName);
							sName=CDataXMLSaver::GetInstringPart("ITEM-NAME:(",")",sName);
							if(sName!=""){
								sFileList+=sFile+"\t";
								sList+=_l(sName)+"\t";
							}
						}else{
							sFileList+=sFile+"\t";
							sList+=_l(GetPathPart(sFile,0,0,1,0))+"\t";
						}
					}
				}
				sList.TrimRight();
				sFileList.TrimRight();
				if(sList!=""){
					hInserted=pDialog->Option_AddCombo(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue),sFileList),atol(sWPParDefVal),sList);
					iCount++;
				}else if(sWPParErr!=""){
					hInserted=pDialog->Option_AddTitle(hInsertPoint,_l(sWPParErr),63);
				}
			}else if(sWPParType=="action"){
				int iActionIndex=aActionsStrings.Add(sWPParAction);
				hInserted=pDialog->Option_AddAction(hInsertPoint,_l(sWPParDsc)+sWPParDetails,TemplateActions,HIROW(iActionIndex));
				pDialog->Option_AddMenuAction(_l(sWPParDsc), TemplateActions, HIROW(iActionIndex), hInsertPoint);
				iCount++;
			}else if(sWPParType=="set_position"){
				int iActionIndex=aActionsStrings.Add(sWPParAction);
				hInserted=pDialog->Option_AddAction(hInsertPoint,_l(sWPParDsc)+sWPParDetails,TemplateActions_SetPos,HIROW(iActionIndex));
				pDialog->Option_AddMenuAction(_l(sWPParDsc)+sWPParDetails, TemplateActions_SetPos, HIROW(iActionIndex), hInsertPoint,TRUE);
				iCount++;
			}else if(sWPParType=="number"){
				hInserted=pDialog->Option_AddNString(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal),atol(sWPParMin),atol(sWPParMax),atol(sWPParOC));
				iCount++;
			}else if(sWPParType=="slider"){
				hInserted=pDialog->Option_AddNSlider(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal),atol(sWPParMin),atol(sWPParMax),atol(sWPParOC));
				iCount++;
			}else if(sWPParType=="font"){
				hInserted=pDialog->Option_AddFont(hInsertPoint,_l(sWPParDsc)+sWPParDetails,&aWPTOptions.AddOptionHolder_Font(sWPPar,sWPParValue),0,69);
				iCount++;
			}else if(sWPParType=="color"){
				hInserted=pDialog->Option_AddColor(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Color(sWPPar,(long)GetRGBFromHTMLString(sWPParValue)),(long)GetRGBFromHTMLString(sWPParDefVal));
				iCount++;
			}else if(sWPParType=="bool"){
				hInserted=pDialog->Option_AddBOOL(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal));
				iCount++;
			}else if(sWPParType=="date"){
				hInserted=pDialog->Option_AddDate(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Date(sWPPar,sWPParValue),CDataXMLSaver::Str2OleDate(sWPParDefVal));
				iCount++;
			}else if(sWPParType=="tip"){
				hInserted=pDialog->Option_AddTitle(hInsertPoint,_l(sWPParDsc)+sWPParDetails,63);
			}else if(sWPParType=="folder" || sWPParType=="root"){
				if(sWPParType=="folder"){
					hInserted=pDialog->Option_AddTitle(hInsertPoint,_l(sWPParDsc)+sWPParDetails);
					hInsertPoint=hInserted;
				}else{
					CWPT* wpt=0;
					if(pMainDialogWindow && g_CurrentEditingIndex>=0 && g_CurrentEditingIndex<objSettings.aLoadedWPTs.GetSize()){
						wpt=GetWPT(g_CurrentEditingIndex);
						if(wpt && sWPParDetails=="" && wpt->sDsc!=""){
							CString sDesk=_l(wpt->sDsc);
							sDesk.Replace("\r","");
							sDesk.Replace("\n"," ");
							sDesk.Replace("\t"," ");
							sDesk.Replace("  "," ");
							sWPParDetails="\t"+_l("Widget description")+": "+sDesk;
						}
					}
					hInserted=pDialog->Option_AddHeader(FL_ROOT,_l(sWPParDsc)+sWPParDetails);
					if(sW98Warn!="" && isWin9x()){
						pDialog->Option_AddTitle(hInserted,_l(sW98Warn),63);
					}
					pDialog->m_OptionsList.Expand(hInserted);
					hRoot=hInserted;
				}
				iCount++;
			}else{
				hInserted=pDialog->Option_AddString(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_String(sWPPar,sWPParValue),sWPParDefVal);
				if(sWPParType=="file"){
					pDialog->Option_AddActionToEdit(hInserted,ChooseAnyFile,hInserted,0,70);
				}
				if(sWPParType=="directory"){
					pDialog->Option_AddActionToEdit(hInserted,ChooseDir,hInserted,0,70);
				}
				iCount++;
			}
			pMapValue=(void*)hInserted;
			aRowMap.SetAt(sWPPar,pMapValue);
			sTagValue="";
		}
	}else{
		while(iFrom>=0){
			CString sWPPar=CDataXMLSaver::GetInstringPart("$=<PARDEF:",">",sWPTemplateSource,iFrom);
			if(sWPPar.IsEmpty()){
				break;
			}
			int iDefPos=sWPPar.Find("=");
			CString sWPParDefVal;
			if(iDefPos!=-1){
				sWPParDefVal=sWPPar.Mid(iDefPos+1);
				if(sWPParDefVal=="\"\""){
					sWPParDefVal="";
				}
				sWPPar=sWPPar.Left(iDefPos);
			}
			iFromPar=iFrom;
			CString sWPParDsc=CDataXMLSaver::GetInstringPart(Format("$=<PARDSC:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			if(sWPParDsc.IsEmpty()){
				sWPParDsc=sWPPar;
			}
			CString sWPParDetails=CDataXMLSaver::GetInstringPart(Format("$=<PARDET:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			if(!sWPParDetails.IsEmpty()){
				sWPParDetails=CString("\t")+_l(sWPParDetails);
			}
			iFromPar=0;
			CString sWPParValue=CDataXMLSaver::GetInstringPart(Format("$=<PARVAL:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			if(iFromPar==-1){
				sWPParValue=sWPParDefVal;
			}
			iFromPar=iFrom;
			CString sWPParType=CDataXMLSaver::GetInstringPart(Format("$=<PARTYP:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			sWPParType.MakeLower();
			iFromPar=iFrom;
			CString sWPParList=CDataXMLSaver::GetInstringPart(Format("$=<PARLST:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			iFromPar=iFrom;
			CString sWPParMin=CDataXMLSaver::GetInstringPart(Format("$=<PARMIN:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			iFromPar=iFrom;
			CString sWPParMax=CDataXMLSaver::GetInstringPart(Format("$=<PARMAX:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			if(sWPParMax.IsEmpty()){
				sWPParMax="9999";
			}
			iFromPar=iFrom;
			CString sWPParErr=CDataXMLSaver::GetInstringPart(Format("$=<PARERR:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			iFromPar=iFrom;
			CString sWPParAction=CDataXMLSaver::GetInstringPart(Format("$=<PARACTION:%s=",sWPPar),"/>",sWPTemplateSource,iFromPar);
			iFromPar=iFrom;
			CString sWPParOC=CDataXMLSaver::GetInstringPart(Format("$=<PAROC:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			iFromPar=iFrom;
			CString sWPParPar=CDataXMLSaver::GetInstringPart(Format("$=<PARPAR:%s=",sWPPar),">",sWPTemplateSource,iFromPar);
			void* pMapValue=NULL;
			HIROW hInsertPoint=NULL, hInserted=NULL;
			if(sWPParPar!=""){
				aRowMap.Lookup(sWPParPar, pMapValue);
				hInsertPoint=(HIROW)pMapValue;
			}
			if(hInsertPoint==NULL){
				hInsertPoint=hRoot;
			}
			if(sWPParType=="list"){
				sWPParList=_l(sWPParList);
				sWPParList.Replace(",","\t");
				hInserted=pDialog->Option_AddCombo(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal),sWPParList);
				iCount++;
			}else if(sWPParType=="hidden"){
				// Nothing to do
			}else if(sWPParType=="dirlist"){
				CString sList,sFileList;
				if(sWPParList.Find("HKLM:")!=-1 || sWPParList.Find("HKCU:")!=-1){
					CRegKey key;
					CString sPath=sWPParList.Mid(strlen("????:"));
					BOOL bRes=0;
					if(sWPParList.Find("HKLM:")!=-1 && key.Open(HKEY_LOCAL_MACHINE, sPath)==ERROR_SUCCESS && key.m_hKey!=NULL){
						bRes=1;
					}
					if(sWPParList.Find("HKCU:")!=-1 && key.Open(HKEY_CURRENT_USER, sPath)==ERROR_SUCCESS && key.m_hKey!=NULL){
						bRes=1;
					}
					int iAdded=0;
					if(bRes){
						// Список!
						iCount=0;
						char szKeysNameBuf[256]={0};
						CString sChangedKey="";
						DWORD dwBufSize=sizeof(szKeysNameBuf);
						FILETIME pFileTime;
						memset(&pFileTime,0,sizeof(pFileTime));
						while(RegEnumKeyEx(key.m_hKey,iCount++,szKeysNameBuf,&dwBufSize,0,0,0,&pFileTime)==ERROR_SUCCESS){
							CString sPathK=szKeysNameBuf;
							sFileList+=sPath+"\\"+sPathK+"\t";
							int iLP=sPathK.ReverseFind('\\');
							if(iLP!=-1){
								sList+=sPathK.Mid(iLP+1)+"\t";
							}else{
								sList+=sPathK+"\t";
							}
							szKeysNameBuf[0]=0;
							dwBufSize=sizeof(szKeysNameBuf);
						}
					}
				}else{
					CString sMask="*.def";
					sWPParList.Replace("/","\\");
					int iLastSlPos=sWPParList.ReverseFind('\\');
					if(iLastSlPos==-1){
						iLastSlPos=sWPParList.ReverseFind('/');
					}
					if(iLastSlPos!=-1){
						sMask=sWPParList.Mid(iLastSlPos+1);
						sWPParList=sWPParList.Left(iLastSlPos+1);
					}
					CFileInfoArray dir;
					dir.AddDir(sWPParList,sMask,TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
					for (int i=0;i<dir.GetSize();i++){
						CString sFile=dir[i].GetFilePath();
						if(sFile!="" && sFile.Find(".def")!=-1){
							CString sName;
							ReadFile(sFile,sName);
							sName=CDataXMLSaver::GetInstringPart("ITEM-NAME:(",")",sName);
							if(sName!=""){
								sFileList+=sFile+"\t";
								sList+=_l(sName)+"\t";
							}
						}else{
							sFileList+=sFile+"\t";
							sList+=_l(GetPathPart(sFile,0,0,1,0))+"\t";
						}
					}
				}
				sList.TrimRight();
				sFileList.TrimRight();
				if(sList!=""){
					hInserted=pDialog->Option_AddCombo(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue),sFileList),atol(sWPParDefVal),sList);
					iCount++;
				}else if(sWPParErr!=""){
					hInserted=pDialog->Option_AddTitle(hInsertPoint,_l(sWPParErr),63);
				}
			}else if(sWPParType=="action"){
				int iActionIndex=aActionsStrings.Add(sWPParAction);
				hInserted=pDialog->Option_AddAction(hInsertPoint,_l(sWPParDsc)+sWPParDetails,TemplateActions,HIROW(iActionIndex));
				pDialog->Option_AddMenuAction(_l(sWPParDsc), TemplateActions, HIROW(iActionIndex), hInsertPoint);
				iCount++;
			}else if(sWPParType=="set_position"){
				int iActionIndex=aActionsStrings.Add(sWPParAction);
				hInserted=pDialog->Option_AddAction(hInsertPoint,_l(sWPParDsc)+sWPParDetails,TemplateActions_SetPos,HIROW(iActionIndex));
				pDialog->Option_AddMenuAction(_l(sWPParDsc)+sWPParDetails, TemplateActions_SetPos, HIROW(iActionIndex), hInsertPoint,TRUE);
				iCount++;
			}else if(sWPParType=="number"){
				hInserted=pDialog->Option_AddNString(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal),atol(sWPParMin),atol(sWPParMax),atol(sWPParOC));
				iCount++;
			}else if(sWPParType=="slider"){
				hInserted=pDialog->Option_AddNSlider(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal),atol(sWPParMin),atol(sWPParMax),atol(sWPParOC));
				iCount++;
			}else if(sWPParType=="font"){
				hInserted=pDialog->Option_AddFont(hInsertPoint,_l(sWPParDsc)+sWPParDetails,&aWPTOptions.AddOptionHolder_Font(sWPPar,sWPParValue),0,69);
				iCount++;
			}else if(sWPParType=="color"){
				hInserted=pDialog->Option_AddColor(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Color(sWPPar,(long)GetRGBFromHTMLString(sWPParValue)),(long)GetRGBFromHTMLString(sWPParDefVal));
				iCount++;
			}else if(sWPParType=="bool"){
				hInserted=pDialog->Option_AddBOOL(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Long(sWPPar,atol(sWPParValue)),atol(sWPParDefVal));
				iCount++;
			}else if(sWPParType=="date"){
				hInserted=pDialog->Option_AddDate(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_Date(sWPPar,sWPParValue),CDataXMLSaver::Str2OleDate(sWPParDefVal));
				iCount++;
			}else if(sWPParType=="tip"){
				hInserted=pDialog->Option_AddTitle(hInsertPoint,_l(sWPParDsc)+sWPParDetails,63);
			}else if(sWPParType=="folder" || sWPParType=="root"){
				if(sWPParType=="folder"){
					hInserted=pDialog->Option_AddTitle(hInsertPoint,_l(sWPParDsc)+sWPParDetails);
					hInsertPoint=hInserted;
				}else{
					CWPT* wpt=0;
					if(pMainDialogWindow && g_CurrentEditingIndex>=0 && g_CurrentEditingIndex<objSettings.aLoadedWPTs.GetSize()){
						wpt=GetWPT(g_CurrentEditingIndex);
						if(wpt && sWPParDetails=="" && wpt->sDsc!=""){
							CString sDesk=_l(wpt->sDsc);
							sDesk.Replace("\r","");
							sDesk.Replace("\n"," ");
							sDesk.Replace("\t"," ");
							sDesk.Replace("  "," ");
							sWPParDetails="\t"+_l("Widget description")+": "+sDesk;
						}
					}
					hInserted=pDialog->Option_AddHeader(FL_ROOT,_l(sWPParDsc)+sWPParDetails);
					if(sW98Warn!="" && isWin9x()){
						pDialog->Option_AddTitle(hInserted,_l(sW98Warn),63);
					}
					pDialog->m_OptionsList.Expand(hInserted);
					hRoot=hInserted;
				}
				iCount++;
			}else{
				hInserted=pDialog->Option_AddString(hInsertPoint,_l(sWPParDsc)+sWPParDetails,aWPTOptions.AddOptionHolder_String(sWPPar,sWPParValue),sWPParDefVal);
				if(sWPParType=="file"){
					pDialog->Option_AddActionToEdit(hInserted,ChooseAnyFile,hInserted,0,70);
				}
				if(sWPParType=="directory"){
					pDialog->Option_AddActionToEdit(hInserted,ChooseDir,hInserted,0,70);
				}
				iCount++;
			}
			pMapValue=(void*)hInserted;
			aRowMap.SetAt(sWPPar,pMapValue);
		}
	}
	if(iCount==0){
		Alert(_l("No parameters found")+"!");
		return FALSE;
	}
#ifndef ART_VERSION
	if(!g_CurrentEditingClocks){
		pDialog->Option_AddAction(hRoot,_l("Enable/Disable widget"), EnDisTemplSep, HIROW(g_CurrentEditingIndex), 0);
		CString sTitle=GetWPTTitle(g_CurrentEditingIndex);
		BOOL bWPTClonable=GetWPTClonable(g_CurrentEditingIndex);
		if(bWPTClonable){
			pDialog->Option_AddAction(hRoot,_l("Make a copy of")+" '"+sTitle+"'", CloneTempl, HIROW(g_CurrentEditingIndex), 0);
		}
		//pDialog->Option_AddAction(hRoot,_l("Remove this widget"), DeleteTemplSep, HIROW(g_CurrentEditingIndex), 0);
	}
#endif
	return TRUE;
}

BOOL AppMainDlg::GetHTMLForClock(const char* szFile,CString& sPreContent,CString sTemplFile, BOOL bSmallize)
{
	sPreContent.Replace("TITLE","title");
	sPreContent.Replace("SWF","swf");
	sPreContent.Replace("OBJECT","object");
	sPreContent.Replace("MOVIE","movie");
	sPreContent.Replace("VALUE","value");
	sPreContent.Replace("WIDTH","width");
	sPreContent.Replace("HEIGHT","height");
	BOOL bDefs=0;
	CString sTitle=CDataXMLSaver::GetInstringPart("<title>","</title>",sPreContent);
	if(sTitle=="" || sTitle.CompareNoCase("%title%")==0){
		sTitle=GetPathPart(szFile,0,0,1,0);
		sTitle.Replace("_"," ");
		if(sTitle[0]>='a' && sTitle[0]<='z'){
			sTitle.SetAt(0,'A'+sTitle[0]-'a');
		}
		bDefs=1;
	}
	sPreContent=CDataXMLSaver::GetInstringPart("<object","/object>",sPreContent);
	int iSwfFrom=sPreContent.Find("movie");
	CString sSwf=CDataXMLSaver::GetInstringPart("value=\"",".swf\"",sPreContent,iSwfFrom);
	if(sSwf=="" || sSwf.CompareNoCase("%swf%")==0){
		sSwf=szFile;
	}else{
		sSwf+=".swf";
	}
	CString sX=CDataXMLSaver::GetInstringPart("width=\"","\"",sPreContent);
	CString sY=CDataXMLSaver::GetInstringPart("height=\"","\"",sPreContent);
	if(bDefs){
		sX="0";
		sY="0";
	}
	CRect rt;
	::GetWindowRect(::GetDesktopWindow(),&rt);
	if(sX.Find("%")!=-1 || sY.Find("%")!=-1){
		if(sX.Find("%")!=-1){
			sX=Format("%i",int(rt.Width()*atol(sX)/100));
		}
		if(sY.Find("%")!=-1){
			sY=Format("%i",int(rt.Height()*atol(sY)/100));
		}
	}
	if(atol(sX)<=0 || atol(sY)<=0){
		int iW=0,iH=0;
		CString sFPath=sSwf;
		if(sSwf.Find(":/")==-1 && sSwf.Find(":\\")==-1){
			char szD[MAX_PATH]={0},szP[MAX_PATH]={0};
			_splitpath(szFile,szD,szP,0,0);
			sFPath=CString(szD)+szP+sSwf;
		}
		GetImageSizeFromFile(sFPath,iW,iH);
		if(iW<=0 || iH<=0){
			iW=150;
			iH=0;
		}
		if(bSmallize && iW>150){
			double d=150.0f/double(iW);
			iW=(int)(double(iW)*d);
			iH=(int)(double(iH)*d);
		}
		sX=Format("%i",iW);
		sY=Format("%i",iH);
	}
	// Добавить проверку на ява-скрипт?
	CString sClockTempl;
	ReadFile(CString(GetApplicationDir())+WP_TEMPLATE+"\\"+sTemplFile,sClockTempl);
	sClockTempl.Replace("%TITLE%",sTitle);
	sClockTempl.Replace("%SWF%",sSwf);
	sClockTempl.Replace("%LEFT%",Format("%i",(rt.Width()-atol(sX))/2));
	sClockTempl.Replace("%TOP%",Format("%i",(rt.Height()-atol(sY))/2));
	sClockTempl.Replace("%X%",sX);
	sClockTempl.Replace("%Y%",sY);
	sPreContent=sClockTempl;
	return TRUE;
}

long lOnTemplParamsFileLock=0;
BOOL AppMainDlg::OnTemplParamsFile(CWnd* pParent, CString sStartTopic, const char* sWPTemplateTitle, const char* szFile, int isWPTIndex)
{
	if(lOnTemplParamsFileLock>0){
		AfxMessageBox(_l("Another dialog is opened already. Close it first!"));
		return FALSE;
	}
	objSettings.bTemplatesInWorkChanged=1;
	BOOL bClocks=(isWPTIndex==-1);
	SimpleTracker Track(lOnTemplParamsFileLock);
	g_CurrentEditingFile=szFile;
	g_CurrentEditingClocks=bClocks;
	g_CurrentEditingIndex=isWPTIndex;
	if(bClocks){
		// Проверяем что это от часов и нету в нем наших тэгов
		CString sPreContent;
		ReadFile(g_CurrentEditingFile,sPreContent);
		if(sPreContent.Find("{"CUTLINE_TAG"}")==-1){
			// Оно, в сыром виде
			GetHTMLForClock(g_CurrentEditingFile,sPreContent);
			SaveFile(g_CurrentEditingFile,sPreContent);
		}
	}
	ReadFileWithConfig(g_CurrentEditingFile,sWPTemplateSource,objSettings.sActivePreset,TRUE);
	// Создаем диалог...
	CDLG_Options_Startup pStartupInfo;
	pStartupInfo.bTreeOnRight=TRUE;
	pStartupInfo.fpInitOptionList=AddWTemplateOptions;
	pStartupInfo.fpApplyOptionList=ApplyWOptionsFromDialog;
	pStartupInfo.iStickPix=objSettings.iStickPix;
	pStartupInfo.szRegSaveKey=PROG_REGKEY"\\Wnd_WPPreferences";
	pStartupInfo.pImageList=&theApp.MainImageList;
	pStartupInfo.szDefaultTopicTitle=sStartTopic;
	pStartupInfo.sz1stTopic=_l(bClocks?"Clock options":"Template options");
	sOptionWndTitle=Format("%s: '%s'",_l("Parameters"),sWPTemplateTitle);
	if(sWPTemplateTitle==""){
		sOptionWndTitle=_l("Wallpaper template parameters");
	}
	pStartupInfo.szWindowTitle=sOptionWndTitle;
	pStartupInfo.iWindowIcon=MAIN_ICON;
	CDLG_Options dlgOpt(pStartupInfo,pParent);
	dlgOpt.bRPaneOnly=1;
	dlgOpt.bShowFind=0;
	CString sHelpFile=CDataXMLSaver::GetInstringPart("$=<HELP-PAGE:",">",sWPTemplateSource);
	dlgOpt.sHelpPage=sHelpFile;
	if(dlgOpt.sHelpPage==""){
		dlgOpt.sHelpPage=bClocks?"dsc_clocks":"dsc_templates";
	}
	pOpenedTemplate=&dlgOpt;
	DWORD dwRes=dlgOpt.DoModal();
	aWPTOptions.FreeMemory();
	pOpenedTemplate=0;
	return TRUE;
}

BOOL AppMainDlg::OnTemplParams(CWnd* pParent, CString sWPT, CString sStartTopic)
{
	if(pParent==0){
		if(this->dlgWChanger){
			pParent=this->dlgWChanger;
		}else{
			pParent=this;
		}
	}
	return OnTemplParamsFile(pParent, sStartTopic, GetWPTTitle(sWPT), GetWPTemplateFilePath(sWPT)+".wpt",GetTemplateMainIndex(sWPT));
}

BOOL WriteSingleTemplateVar(CString wptName,CString sVar,CString sVal,const char* szPreset)
{
	CString sP=szPreset;
	if(szPreset==0){
		sP=objSettings.sActivePreset;
	}
	CString sConfigFile=GetHTMLConfigFile(wptName,sP);
	CString sWPTemplateSource;
	ReadFile(sConfigFile,sWPTemplateSource);
	CString sWPParValue=CDataXMLSaver::GetInstringPart(Format("$=<PARVAL:%s=",sVar),">",sWPTemplateSource,0);
	sWPTemplateSource.Replace(Format("$=<PARVAL:%s=%s>",sVar,sWPParValue),"");
	sWPTemplateSource+=Format("$=<PARVAL:%s=%s>",sVar,sVal);
	SaveFile(sConfigFile,sWPTemplateSource);
	return TRUE;
}


BOOL ReadSingleTemplateVar(CString wptName,CString sVar,CString& sVal,const char* szPreset,const char* szOverloadedDef)
{
	CString sP=szPreset;
	if(szPreset==0){
		sP=objSettings.sActivePreset;
	}
	CString sConfigFile=GetHTMLConfigFile(wptName,sP);
	CString sWPTemplateSource;
	ReadFile(sConfigFile,sWPTemplateSource);
	int iFrom=0;
	sVal=CDataXMLSaver::GetInstringPart(Format("$=<PARVAL:%s=",sVar),">",sWPTemplateSource,iFrom);
	if(iFrom==-1){
		if(szOverloadedDef){
			sVal=szOverloadedDef;
		}else{
			// Дефолт?
			ReadFileWithConfig(GetWPTemplateFilePath(wptName),sWPTemplateSource,objSettings.sActivePreset,TRUE);
			if(sWPTemplateSource.Find("wp:define")==-1){
				iFrom=0;
				sVal=CDataXMLSaver::GetInstringPart(Format("$=<PARDEF:%s=",sVar),">",sWPTemplateSource,iFrom);
			}else{
				sVal=_atr(Format("[%s]",sVar),sWPTemplateSource);
			}
		}
	}
	return TRUE;
}

DWORD WINAPI CallTemplatePositionDialog_InThread(LPVOID p)
{
	CWPT* wpt=(CWPT*)p;
	CallTemplatePositionDialog(wpt);
	return 1;
}

DWORD WINAPI CallTemplatePositionDialog_InThread2(LPVOID p)
{
	CWPT* wpt=(CWPT*)p;
	CallTemplatePositionDialog(wpt);
	if(pMainDialogWindow){
		pMainDialogWindow->OrderModifierForNextWallpaper=-2;
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
	}
	return 1;
}

BOOL CallTemplatePositionDialog(CWPT* wpt,CRect* pCurRect, BOOL bSilent)
{
	if(wpt==0){
		return FALSE;
	}
	if(lOnTemplParamsFileLock>0){
		if(!bSilent){
			AfxMessageBox(_l("Another dialog is opened already. Close it first!"));
		}
		return FALSE;
	}
	SimpleTracker Track(lOnTemplParamsFileLock);
	int iActionIndex=-1;
	if(!pCurRect){
		iActionIndex=aActionsStrings.Add(wpt->sActionPosition);
	}
	aWPTOptions.FreeMemory();
	CString sWPParValue;
	CString sXName=CDataXMLSaver::GetInstringPart("X:(",")",wpt->sActionPosition);
	ReadSingleTemplateVar(wpt->sWPFileName,sXName,sWPParValue);
	if(pCurRect){
		pCurRect->left=atol(sWPParValue);
	}
	aWPTOptions.AddOptionHolder_Long(sXName,atol(sWPParValue));
	CString sYName=CDataXMLSaver::GetInstringPart("Y:(",")",wpt->sActionPosition);
	ReadSingleTemplateVar(wpt->sWPFileName,sYName,sWPParValue);
	if(pCurRect){
		pCurRect->top=atol(sWPParValue);
	}
	aWPTOptions.AddOptionHolder_Long(sYName,atol(sWPParValue));
	CString sWName=CDataXMLSaver::GetInstringPart("W:(",")",wpt->sActionPosition);
	ReadSingleTemplateVar(wpt->sWPFileName,sWName,sWPParValue);
	if(pCurRect){
		pCurRect->right=pCurRect->left+atol(sWPParValue);
	}
	aWPTOptions.AddOptionHolder_Long(sWName,atol(sWPParValue));
	CString sHName=CDataXMLSaver::GetInstringPart("H:(",")",wpt->sActionPosition);
	ReadSingleTemplateVar(wpt->sWPFileName,sHName,sWPParValue);
	if(pCurRect){
		pCurRect->bottom=pCurRect->top+atol(sWPParValue);
	}
	aWPTOptions.AddOptionHolder_Long(sHName,atol(sWPParValue));
	if(iActionIndex!=-1){
		CTemplateActions_SetPos_InThread* p=new CTemplateActions_SetPos_InThread;
		p->m_iAction=iActionIndex;
		p->m_pDialog=0;
		p->pWpt=wpt;
		TemplateActions_SetPos_InThread(LPVOID(p));
		POSITION pos=aWPTOptions.aLongHolder.GetStartPosition();
		CString sKey,sTempVal;
		long lVal;
		while(pos){
			aWPTOptions.aLongHolder.GetNextAssoc(pos,sKey,lVal);
			WriteSingleTemplateVar(wpt->sWPFileName,sKey,Format("%i",lVal));
		}
	}
	aWPTOptions.FreeMemory();
	//int iActiveWpt=GetTemplateMainIndex(sCurrentWidget);
	//pMainDialogWindow->PostMessage(WM_COMMAND,WM_USER+CHANGE_TEMPL_PAR+iActiveWpt,255);
	return TRUE;
}