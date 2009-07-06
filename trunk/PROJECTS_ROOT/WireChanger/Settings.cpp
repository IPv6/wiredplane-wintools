// Settings.cpp: implementation of the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#include "DLG_Chooser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL bAddOnStartCal=1;
BOOL bAddOnStartClo=0;

CString sLangsNames="English\tRussian";
int GetBuffersLang(CStringArray& aLangBufferNames, int& iDefLang)
{
	int iLang=0;
	CFileInfoArray dir;
	aLangBufferNames.RemoveAll();
	CString sStartDir=GetApplicationDir();
	dir.AddDir(sStartDir,"*.lng",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	int iSize=dir.GetSize();
	if(iSize<=1){
		iSize=1;
		iDefLang=0;
		aLangBufferNames.Add("English");
		sLangsNames="English";
	}else if(iSize>1){
		int i;
		for(i=0;i<iSize;i++){
			CString sLangName=GetLangName(i);
			aLangBufferNames.Add(sLangName);
		}
		sLangsNames="";
		for(i=0;i<aLangBufferNames.GetSize();i++){
			sLangsNames+=aLangBufferNames[i];
			sLangsNames+="\t";
		}
		sLangsNames.TrimRight();
	}
	return iSize;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void ClearWPTs()
{
	objSettings.aLoadedWPTs.RemoveAll();
	CWPT* pVal=0;
	CString sKey;
	POSITION p=objSettings.aLoadedWPTsData.GetStartPosition();
	while(p)
	{
		objSettings.aLoadedWPTsData.GetNextAssoc(p,sKey,pVal);
		delete pVal;
	}
	objSettings.aLoadedWPTsData.RemoveAll();
};

void AssignPreview(CString& sPreview,CString sDefName)
{
	if(sPreview!=""){
		sPreview=Format("%s"WP_TEMPLATE"\\%s",GetApplicationDir(),sPreview);
	}else if(isFileExist(Format("%s"WP_TEMPLATE"\\Preview\\%s.gif",GetApplicationDir(),sDefName))){
		sPreview=Format("%s"WP_TEMPLATE"\\Preview\\%s.gif",GetApplicationDir(),sDefName);
	}
}

void LoadTemplates(CString sStartDir)
{
	if(sStartDir==""){
		sStartDir=GetApplicationDir();
		sStartDir+=WP_TEMPLATE"\\";
	}
	CStringArray aLoadedWPTsSort;
	// Вытаскиваем шаблоны...
	ClearWPTs();
	CFileInfoArray dir;
#ifndef ART_VERSION
	CString sImports=GetUserFolder()+WP_IMPORTS"\\";
	CString sImportsOrigin=GetAppFolder()+WP_TEMPLATE"\\"WP_IMPORTS"\\";
	CreateDirIfNotExist(sImports);
	// Импортируем если чего не хватает
	CFileInfoArray dirImprots;
	dirImprots.AddDir(sImportsOrigin,"*.wcclock",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	for (int j=0;j<dirImprots.GetSize();j++){
		CString sFile=dirImprots[j].GetFilePath();
		CString sFileWpt=sImports+dirImprots[j].GetFileTitle()+".wpt";
		if(!isFileExist(sFileWpt)){
			CString sTemplate;
			AppMainDlg::GetHTMLForClock(sFile,sTemplate,"import_wcclock.templ",1);
			SaveFile(sFileWpt,sTemplate);
		}
	}
	// Читаем шаблоны
	dir.AddDir(sStartDir,"*.wpt",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	dir.AddDir(sImports,"*.wpt",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
#else
	dir.AddFile(sStartDir+"ArtInfo.wpt",0);
#endif
	for (int i=0;i<dir.GetSize();i++){
		if(i>=MAX_WPT){
			break;
		}
		CString sFile=dir[i].GetFileTitle();
		if(sFile!=""){
			CString sFileFullPath=dir[i].GetFilePath();
			//int iWptIndex
			int iNI=objSettings.aLoadedWPTs.Add(sFile);
			CWPT* pWPT=new CWPT;
			objSettings.aLoadedWPTsData.SetAt(sFile,pWPT);
			CString sWPTemplateSource;
			ReadFile(GetWPTemplateFilePath(sFile+".wpt"),sWPTemplateSource);
			{// Общая часть
				pWPT->sActionTransparency=_atr("tray-transparency",sWPTemplateSource);
				pWPT->sActionPosition=_atr("tray-position",sWPTemplateSource);
			}
			if(sWPTemplateSource.Find("wp:head")!=-1){
				pWPT->sHash=_atr("hash",sWPTemplateSource); 
				int iFrom=0,iFromPar=0,iCount=0;
				extTag("wp:head",sWPTemplateSource,&sWPTemplateSource);
				CString sWPTTitle=_atr("title",sWPTemplateSource);
				if(sWPTTitle==""){
					sWPTTitle=sFile;
				}
				CString sSortID=_atr("sort-id",sWPTemplateSource);
				if(sSortID.GetLength()==0){
					sSortID="z";
				}
				if(sSortID.GetLength()<5){
					sSortID+=CString('.',5-sSortID.GetLength());
				}
				CString sCSortID=_atr("sortc-id",sWPTemplateSource);
				if(sCSortID.GetLength()==0){
					sCSortID="z";
				}
				if(sCSortID.GetLength()<5){
					sCSortID+=CString('.',5-sCSortID.GetLength());
				}
				CString sDescription=_atr("description",sWPTemplateSource);
				sDescription.TrimLeft();
				sDescription.TrimRight();
				CString sCateg=_atr("category",sWPTemplateSource);
				CString sPostFix="";
				int iCloneLabelPos=sFile.Find(DEF_CLONEPOST);
				if(iCloneLabelPos!=-1){
					sPostFix=Format(" (#%i)",atol(sFile.Mid(iCloneLabelPos+strlen(DEF_CLONEPOST))));
				}
				if(sPostFix!=""){
					CString sTemp=_l(sWPTTitle+sPostFix,_l(sWPTTitle)+sPostFix);
				}
				sWPTTitle+=sPostFix;
				pWPT->sADType=_atr("render",sWPTemplateSource);
				pWPT->sADTypeAlt=_atr("alt-render",sWPTemplateSource);
				pWPT->sHints=_atr("hints",sWPTemplateSource);
				pWPT->sTitle=sWPTTitle;
				pWPT->sCategs=sCateg;
				pWPT->sDsc=sDescription;
				pWPT->sWPFileName=GetPathPart(sFileFullPath,0,0,1,1);
				pWPT->sTemplName=sFile;
				pWPT->sExcCateg=_atr("internal-category",sWPTemplateSource);
				pWPT->bClonable=(isTrue(_atr("clonable",sWPTemplateSource)) && sFile.Find(DEF_CLONEPOST)==-1);
				pWPT->bScrollBarsAware=(isTrue(_atr("scrollbars",sWPTemplateSource)));
				pWPT->sPreview=_atr("preview",sWPTemplateSource);
				AssignPreview(pWPT->sPreview,sFile);
				// Парсим на предмет действий доступных из меню в трее
				iFrom=0;
				CString sWPInTray=_atr("tray-items",sWPTemplateSource);
				CStringArray sItemsData;
				ConvertComboDataToArray(sWPInTray,sItemsData,',');
				for(int j=0;j<sItemsData.GetSize();j++){
					CString sWPPar=sItemsData[j];
					if(sWPPar!="" && sWPPar[0]=='!'){
						sWPPar=sWPPar.Mid(1);
						pWPT->aLoadedWPTsActionsFlags.Add(2);
					}else if(sWPPar!="" && sWPPar[0]=='*'){
						sWPPar=sWPPar.Mid(1);
						pWPT->aLoadedWPTsActionsFlags.Add(1);
					}else{
						pWPT->aLoadedWPTsActionsFlags.Add(0);
					}
					CString sTagValue;
					int iPos=sWPTemplateSource.Find(Format("[%s]",sWPPar));
					CString sWPParDsc;
					if(iPos!=-1){
						int iPos2=sWPTemplateSource.Find("{/wp:define",iPos);
						if(iPos2!=-1){
							// Описание пункта
							sWPParDsc=_atr("title",sWPTemplateSource.Mid(iPos,iPos2-iPos));
						}
					}
					if(sWPParDsc.IsEmpty()){
						sWPParDsc=sWPPar;
					}
					if(sPostFix!=""){
						//Обращаемся
						CString sTemp=_l(sWPParDsc+sPostFix,_l(sWPParDsc)+sPostFix);
					}
					pWPT->aLoadedWPTsActionsDsc.Add(sWPParDsc+sPostFix);
					// Имя файла для прямого редактирования
					CString sWPParFile=_atr("action",sTagValue);
					pWPT->aLoadedWPTsActionsFile.Add(sWPParFile);
				}
				aLoadedWPTsSort.SetAtGrow(iNI,sCSortID+"."+_l(sCateg)+"."+sSortID+"."+_l(sWPTTitle));
			}else{
				int iFrom=0,iFromPar=0,iCount=0;
				CString sWPTTitle=CDataXMLSaver::GetInstringPart("$=<TITLE:",">",sWPTemplateSource,iFrom);
				if(sWPTTitle==""){
					sWPTTitle=sFile;
				}
				pWPT->sHash=CDataXMLSaver::GetInstringPart("$=<HASH:",">",sWPTemplateSource);
				CString sSortID=CDataXMLSaver::GetInstringPart("$=<SORTID:",">",sWPTemplateSource);
				if(sSortID.GetLength()==0){
					sSortID="z";
				}
				if(sSortID.GetLength()<5){
					sSortID+=CString('.',5-sSortID.GetLength());
				}
				CString sCSortID=CDataXMLSaver::GetInstringPart("$=<SORTCID:",">",sWPTemplateSource);
				if(sCSortID.GetLength()==0){
					sCSortID="z";
				}
				if(sCSortID.GetLength()<5){
					sCSortID+=CString('.',5-sCSortID.GetLength());
				}
				CString sDescription=CDataXMLSaver::GetInstringPart("$=<DESCRIPTION:",">",sWPTemplateSource);
				sDescription.TrimLeft();
				sDescription.TrimRight();
				CString sCateg=CDataXMLSaver::GetInstringPart("$=<CATEG:",">",sWPTemplateSource);
				CString sPostFix="";
				int iCloneLabelPos=sFile.Find(DEF_CLONEPOST);
				if(iCloneLabelPos!=-1){
					sPostFix=Format(" (#%i)",atol(sFile.Mid(iCloneLabelPos+strlen(DEF_CLONEPOST))));
				}
				if(sPostFix!=""){
					CString sTemp=_l(sWPTTitle+sPostFix,_l(sWPTTitle)+sPostFix);
				}
				sWPTTitle+=sPostFix;
				pWPT->sADType=CDataXMLSaver::GetInstringPart("$=<RENDER:",">",sWPTemplateSource);
				pWPT->sADTypeAlt=CDataXMLSaver::GetInstringPart("$=<ALTRENDER:",">",sWPTemplateSource);
				pWPT->sHints=CDataXMLSaver::GetInstringPart("$=<HINTS:",">",sWPTemplateSource);
				pWPT->sTitle=sWPTTitle;
				pWPT->sCategs=sCateg;
				pWPT->sDsc=sDescription;
				pWPT->sWPFileName=GetPathPart(sFileFullPath,0,0,1,1);
				pWPT->sTemplName=sFile;
				pWPT->sExcCateg=CDataXMLSaver::GetInstringPart("$=<INTERNAL-CATEG:",">",sWPTemplateSource);
				pWPT->bClonable=(sWPTemplateSource.Find("NO-DUPS")==-1 && sFile.Find(DEF_CLONEPOST)==-1);
				pWPT->bScrollBarsAware=(sWPTemplateSource.Find("SCROLLBARS")!=-1);
				pWPT->sPreview=CDataXMLSaver::GetInstringPart("$=<PREVIEW:",">",sWPTemplateSource);
				AssignPreview(pWPT->sPreview,sFile);
				// Парсим на предмет действий доступных из меню в трее
				iFrom=0;
				CString sWPInTray=CDataXMLSaver::GetInstringPart("$=<SHOWINTRAY:",">",sWPTemplateSource,iFrom);
				CStringArray sItemsData;
				ConvertComboDataToArray(sWPInTray,sItemsData,',');
				for(int j=0;j<sItemsData.GetSize();j++){
					CString sWPPar=sItemsData[j];
					if(sWPPar!="" && sWPPar[0]=='!'){
						sWPPar=sWPPar.Mid(1);
						pWPT->aLoadedWPTsActionsFlags.Add(2);
					}else if(sWPPar!="" && sWPPar[0]=='*'){
						sWPPar=sWPPar.Mid(1);
						pWPT->aLoadedWPTsActionsFlags.Add(1);
					}else{
						pWPT->aLoadedWPTsActionsFlags.Add(0);
					}
					// Описание пункта
					CString sWPParDsc=CDataXMLSaver::GetInstringPart(Format("$=<PARDSC:%s=",sWPPar),">",sWPTemplateSource,0);
					if(sWPParDsc.IsEmpty()){
						sWPParDsc=sWPPar;
					}
					if(sPostFix!=""){
						CString sTemp=_l(sWPParDsc+sPostFix,_l(sWPParDsc)+sPostFix);
					}
					pWPT->aLoadedWPTsActionsDsc.Add(sWPParDsc+sPostFix);
					// Имя файла для прямого редактирования
					CString sWPParFile=CDataXMLSaver::GetInstringPart(Format("$=<PARACTION:%s=",sWPPar),"/>",sWPTemplateSource,0);
					pWPT->aLoadedWPTsActionsFile.Add(sWPParFile);
				}
				aLoadedWPTsSort.SetAtGrow(iNI,sCSortID+"."+_l(sCateg)+"."+sSortID+"."+_l(sWPTTitle));
			}
		}
	}
	// Сортируем
	int iSize=objSettings.aLoadedWPTs.GetSize();
	//CString s=DumpCAr(objSettings.aLoadedWPTs,&aLoadedWPTsSort);
	for(int si=0;si<iSize-1;si++){
		for(int sj=si+1;sj<iSize;sj++){
			if(aLoadedWPTsSort[sj]<aLoadedWPTsSort[si]){
				CString sTmp=aLoadedWPTsSort[sj];
				aLoadedWPTsSort[sj]=aLoadedWPTsSort[si];
				aLoadedWPTsSort[si]=sTmp;
				CString sTmp2=objSettings.aLoadedWPTs[sj];
				objSettings.aLoadedWPTs[sj]=objSettings.aLoadedWPTs[si];
				objSettings.aLoadedWPTs[si]=sTmp2;
			}
		}
	}
}

BOOL LoadNoteSkin(int iSkin,CString szSkinDir, BOOL bWithIcons)
{
	if(iSkin<=objSettings.aNoteSkin.GetSize()){// Описание скина
		CNoteSkin skin;
		skin.sFolder=szSkinDir;
		CString sSkinDef;
		ReadFile(Format("%sskin.def",szSkinDir),sSkinDef);
		sSkinDef+="\n";
		skin.sTitle=CDataXMLSaver::GetInstringPart("TITLE=","\n",sSkinDef);
		skin.sTitle.TrimLeft();
		skin.sTitle.TrimRight();
		if(skin.sTitle.GetLength()==0){
			return 0;
		}
		skin.sAuthor=CDataXMLSaver::GetInstringPart("AUTHOR=","\n",sSkinDef);
		skin.sAuthor.TrimLeft();
		skin.sAuthor.TrimRight();
		if(skin.sTitle==""){
			skin.sTitle=Format("%s #%lu",_l("Skin"),iSkin);
		}
		skin.dwTXColor=atol(CDataXMLSaver::GetInstringPart("TCOLOR=","\n",sSkinDef));
		skin.dwBGColor=atol(CDataXMLSaver::GetInstringPart("BCOLOR=","\n",sSkinDef));
		skin.bXPos=atol(CDataXMLSaver::GetInstringPart("ICON_LALIGN=","\n",sSkinDef));
		skin.bYPos=atol(CDataXMLSaver::GetInstringPart("ICON_TALIGN=","\n",sSkinDef));
		skin.dXDelta=atof(CDataXMLSaver::GetInstringPart("ICON_XDELTA=","\n",sSkinDef));
		skin.dYDelta=atof(CDataXMLSaver::GetInstringPart("ICON_YDELTA=","\n",sSkinDef));

		skin.sHTML_Full=CDataXMLSaver::GetInstringPart("<template-full>","</template-full>",sSkinDef);
		skin.sHTML_Titleonly=CDataXMLSaver::GetInstringPart("<template-titleonly>","</template-titleonly>",sSkinDef);
		skin.sHTML_Iconic=CDataXMLSaver::GetInstringPart("<template-iconic>","</template-iconic>",sSkinDef);
		skin.sHTML_Text=CDataXMLSaver::GetInstringPart("<template-textonly>","</template-textonly>",sSkinDef);
		skin.sHTML_Css=CDataXMLSaver::GetInstringPart("<template-css>","</template-css>",sSkinDef);
		
		skin.bIsHTML=skin.sHTML_Full.GetLength()+skin.sHTML_Titleonly.GetLength()+skin.sHTML_Iconic.GetLength()+skin.sHTML_Text.GetLength();
		int iIconTitleNum=0;
		while(1){
			CString sIconTitle;
			sIconTitle=CDataXMLSaver::GetInstringPart(Format("ICON%lu_TITLE=",iIconTitleNum),"\n",sSkinDef);
			if(sIconTitle==""){
				break;
			}
			sIconTitle.TrimLeft();
			sIconTitle.TrimRight();
			if(sIconTitle=="#"){
				sIconTitle=Format("ICON%lu_TITLE",iIconTitleNum);
			}
			skin.aIconsTitles.SetAtGrow(iIconTitleNum,sIconTitle);
			iIconTitleNum++;
		}
		objSettings.aNoteSkin.SetAtGrow(iSkin,skin);
	}
	if(bWithIcons && !objSettings.aNoteSkin[iSkin].bIconsLoaded){// Грузим иконки
		objSettings.aNoteSkin[iSkin].bIconsLoaded=TRUE;
		int iNoteIcon=0;
		CBitmap* bmpIco=NULL;
		do{
			CString sIcoPath=Format("%sicon%lu",szSkinDir,iNoteIcon);
			bmpIco=_bmp().LoadBmpFromPathExt(sIcoPath);
			if(bmpIco){
				//if(isFileExist(sIcoPath+".png")){PngTransp2White(bmpIco);}
				CBitmap* bmpIcoCopy1=_bmp().AddBmp(GetNSkinBmp(iSkin,"ICO_TN",iNoteIcon),bmpIco);
				CBitmap* bmpIcoCopy2=_bmp().AddBmpRaw(GetNSkinBmp(iSkin,"ICO",iNoteIcon),bmpIco,GetBitmapSize(bmpIco));
				delete bmpIco;
				iNoteIcon++;
			}
		}while(bmpIco!=NULL);
		CBitmap* bmTi=_bmp().LoadBmpFromPathExt(szSkinDir+"titlebg");
		if(bmTi){
			_bmp().AddBmpRaw(GetNSkinBmp(iSkin,"TITLE"),bmTi,GetBitmapSize(bmTi));
			delete bmTi;
		}
		CBitmap* bmTe=_bmp().LoadBmpFromPathExt(szSkinDir+"textbg");
		if(bmTe){
			_bmp().AddBmpRaw(GetNSkinBmp(iSkin,"TEXT"),bmTe,GetBitmapSize(bmTe));
			delete bmTe;
		}
		objSettings.aNoteSkin[iSkin].iMaxIcons=iNoteIcon;
	}
	return 1;
}

BOOL isTempKeyRnd()
{
	if(GetWindowDays()>0){
		return FALSE;
	}
	if(isTempKey()){
		static BOOL b=0;
		int i=rnd(0,100);
		if(b<10 || i>50){
			b++;
			return TRUE;
		}
	}
	return FALSE;
}

void SaveNotes()
{
#if	defined(USE_LITE) || defined(ART_VERSION)
	return;
#endif
	CSmartLock lN(&objSettings.cNoteLock,TRUE,-1);
	for(int i=0;i<objSettings.aDesktopNotes.GetSize();i++){
		CNote& nt=objSettings.aDesktopNotes[i];
		nt.SaveToFile();
	}
}


void LoadNotes(CString sStartDir)
{
#if	defined(USE_LITE) || defined(ART_VERSION)
	return;
#endif
	if(sStartDir==""){
		sStartDir=GetNotesFolder();
	}
	// Вытаскиваем шаблоны...
	CFileInfoArray dir;
	CSmartLock lN(&objSettings.cNoteLock,TRUE,-1);
	dir.AddDir(sStartDir,"*.wnt",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	objSettings.aDesktopNotes.RemoveAll();
	for (int i=0;i<dir.GetSize();i++){
		CString sFile=dir[i].GetFilePath();
		if(sFile!=""){
			CNote nt;
			nt.sNoteFile=GetPathPart(sFile,0,0,1,1);
			if(nt.LoadFromFile()){
				if(objSettings.iLikStatus<2 && dir.GetSize()>=2){
					// Не грузим больше двух
					CString sUnregRem=_l("Don`t forget to\r\nregister "+AppName())+"!";
					CString sContent=nt.GetRealTextFromNote();
					if(strstr(sContent,sUnregRem)==0)
					{
						sContent+="\r\n--\r\n";
						sContent+=sUnregRem;
						nt.SetRealTextFromNote(sContent);
					}
				}
				objSettings.aDesktopNotes.Add(nt);
			}
		}
	}
	CString sSkinDirBegin=Format("%sSkins\\",GetApplicationDir());
	if(objSettings.sIcoInitDir==""){
		objSettings.sIcoInitDir=sSkinDirBegin;
	}
	objSettings.aNoteSkin.RemoveAll();
	int iSkin=0;
	CString sSkinDir;
	CFileInfoArray dirS;
	dirS.AddDir(sSkinDirBegin,"Note*.*",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,TRUE);
	for (int j=0;j<dirS.GetSize();j++){
		sSkinDir=dirS[j].GetFilePath()+"\\";
		if(isFileExist(sSkinDir)){
			if(LoadNoteSkin(iSkin,sSkinDir,(iSkin==objSettings.lNoteSkin)?TRUE:FALSE)){
				iSkin++;
			}
		}
	}
}

BOOL WINAPI ReminderMody(CWPReminder& rem)
{
	CString sUnregRem=_l("Don`t forget to\r\nregister "+AppName())+"!";
	if(isTempKeyRnd() && strstr(rem.szText,sUnregRem)==0)
	{
		strcat(rem.szText,"\r\n--\r\n");
		strcat(rem.szText,sUnregRem);
	}
	return FALSE;
}

int GetStrMiniHash(const char* szName)
{
	int iHash=0;
	char const* szPos=szName;
	//-----------------------
	while(*szPos){
		iHash+=*szPos;
		szPos++;
	}
	iHash=iHash%20;
	return iHash;
}

BOOL CSettings::bWPEngineInitialized=FALSE;
CSettings::CSettings()
{
	isLight=0;
	lEnableDebug=0;
	bBlockAlerts=0;
	hRemin=0;
	hHookLib=0;
	lTempDisableAllWidgets=0;
	lUseGlobalRandomWPTLevel=1;
	lDefMWTab=0;
	bTemplatesInWorkChanged=0;
	IsScreenBordersAreFAT()=0;
	lSkipMorthOnGeneration=0;
	lShowAnimationItems=0;
	noWallpaperChangesYet=TRUE;
	noDesktopHookProcessing=0;
	lConfModeIsON=0;
	lAdForPrevModeIsON=0;
	lRefreshOnNewDay=0;
	bSaveAllowed=0;
	lNoExitWarning=FALSE;
	sIniFile=getFileFullName(GetDesktopPrefix()+AppName()+".conf");
	hGlobalTimerEvent=CreateEvent(NULL,FALSE,FALSE,"");
	//
	CString sOS;
	bUnderWindowsNT=0;
	lMainWindowOpened=0;
	GetOSVersion(sOS,NULL,&bUnderWindowsNT);
	browser=NULL;
	bNeedFirstTimeChange=0;
	lLastTimeChange=GetTickCount();
	dtStartTime=GETCURTIME;//COleDateTime::GetCurrentTime();
	lChangePriodically=1;
	lChangePeriod=DEFAULT_PERIODMIN;
	dwAutoStretchLim=DEFAULT_WPSTRLIM;
	bLoadDirsRecursively=2;
	iLikStatus=1;
	sLikKey="";
	sLikUser="";
	lAlertsOnTaskbar=FALSE;
	WPInProgress=FALSE;
	hHotMenuWnd=NULL;
	hFocusWnd=NULL;
	ActiveWndRect.SetRect(0,0,0,0);
	bRunStatus=0;
	lOnExitFromMainWnd=0;
	sMainWindowTitle=AppName();
	sTrayTooltip=sMainWindowTitle;
	pNoteFont=NULL;
	pNoteTFont=0;
	lUseNoteTitleShadow=1;
	lUseNoreDirectPositioning=0;
	// Имя компа
	char szCompName[MAX_COMPUTERNAME_LENGTH + 1]="";
	DWORD dwLen=sizeof(szCompName);
	DWORD dwRes=GetComputerName(szCompName,&dwLen);
	szSystemFolder[0]=0;
	GetSpecialFolderPath(szSystemFolder,CSIDL_PERSONAL);
	bDoModalInProcess=0;
	bStartWithOptions=FALSE;
	lVeryFirstStart=0;
	lSetWallaperLock=0;
	lSetWallaperLockDesc="";
	{//Wchr
		sHTMLWrapperFile=GetUserFolder()+DEFAULT_HTMLBGTEMPL;
	}
	hEnliver=0;
	hHookLib=0;
	hRemin=0;
#if	!defined(USE_LITE) || defined(ART_VERSION)
	hHookLib=LoadLibrary("WCDesknote.cml");
#endif
#if	!defined(USE_LITE)
	hRemin=LoadLibrary("WCRemind.cml");
	if(hRemin){
		__bmpX fp1=(__bmpX)GetProcAddress(hRemin,"_bmpX");
		if(fp1){
			(*fp1)()=&_bmp();
		}
		__lngX fp2=(__lngX)GetProcAddress(hRemin,"_lngX");
		if(fp2){
			(*fp2)()=CUITranslationImpl::getInstance();
		}
		_InitDLL fp0=(_InitDLL)GetProcAddress(hRemin,"InitDLL");
		if(fp0){
			CString sRemDir=GetUserFolder()+WP_REMINDERS"\\";
			CreateDirIfNotExist(sRemDir);
			int ihash=0;
			(*fp0)(ihash, sRemDir, ReminderMody);
		}
	}
	hEnliver=LoadLibrary("WCEnliver.cml");
	_StartEnliver fp=(_StartEnliver)GetProcAddress(hEnliver,"StartEnliver");
	if(fp){
		(*fp)();
	}
	hUniquer=LoadLibrary("Uniquer.cml");
	if(hUniquer){
		__bmpX fp1=(__bmpX)GetProcAddress(hUniquer,"_bmpX");
		if(fp1){
			(*fp1)()=&_bmp();
		}
		_Handshake fp=(_Handshake)GetProcAddress(hUniquer,"Handshake");
		if(fp){
			CString sA=Format("%lu-WC",GetTickCount());
			(*fp)(sA,GetStrMiniHash(sA));
		}
	}
#endif
	bRunStatus=1;
}

DWORD WINAPI WhatsNew(LPVOID data)
{
	static long bWas=0;
	if(bWas){
		return 0;
	}
	bWas=1;
	if(AfxMessageBox(_l("Do you want to see 'What`s new' in this version of")+" "+AppName()+"?",MB_YESNO|MB_ICONQUESTION)==IDYES){
		ShowHelp("history");
	}
	return 0;
}

DWORD DefActionMapHK_Preinit[MAX_HOTACTIONS]={262231,0};//393303-CtrlAltW
long lRestoreOnResChangeDef=0;
void CSettings::LoadActiveWPT(const char* szPreset, CStringArray& aWPTs, BOOL bLoadDefaults)
{
	CString sActWpt=GetHTMLConfigFile("actives"CONFIG_EXT,szPreset);
	if(!isFileExist(sActWpt)){
		sActWpt=sIniFile;
	}
	CDataXMLSaver IniFile(sActWpt,"application-config",TRUE);
	int k=0;
	CString sWPT;
	aWPTs.RemoveAll();
	while(IniFile.getValue(Format("wchr-HTML_WPT%lu",k),	sWPT)){
		aWPTs.SetAtGrow(k++,sWPT);
		if(objSettings.iLikStatus<2){
			if(aWPTs.GetSize()>=4){
				break;
			}
		}
	}
	/*
	CString aWPTsFreq;
	IniFile.getValue("wchr-wptsFreq",aWPTsFreq);
	// Тут грязный хак :((( к сожалению пока так
	for(int h=0;h<objSettings.aLoadedWPTs.GetSize();h++){
		CWPT* wpt=GetWPT(objSettings.aLoadedWPTs[h]);
		if(wpt){
			wpt->SetWeight(atol(_atr(wpt->sFileName,aWPTsFreq,"100")));
		}
	}*/
	if(bLoadDefaults){
		IniFile.getValue("wchr-disableNotes",lDisableNotes);
	}
}

void CSettings::LoadActiveWPT()
{
#ifdef ART_VERSION
	objSettings.TemplatesInWork.RemoveAll();
	objSettings.TemplatesInWork.Add("ArtInfo");
#else
	bTemplatesInWorkChanged=1;
	LoadActiveWPT(objSettings.sActivePreset,objSettings.TemplatesInWork, 1);
	if(bStartWithOptions && objSettings.TemplatesInWork.GetSize()==0){
		int k=0;
		TemplatesInWork.SetAtGrow(k++,"Citations");
		TemplatesInWork.SetAtGrow(k++,"Second Picture");
		TemplatesInWork.SetAtGrow(k++,"Controls");
		//if(bAddOnStartCal){
			TemplatesInWork.SetAtGrow(k++,"Calendar");
		//}
		if(bAddOnStartClo){
			TemplatesInWork.SetAtGrow(k++,"Flip clock");
		}
	}
#endif
}

CCriticalSection csGPL;
void GetPresetList(CStringArray& aPres)
{
	SimpleLocker l(&csGPL);
	aPres.RemoveAll();
	aPres.Add("");
	CFileInfoArray dir;
	CString sPRoot=GetPresetsFolder("?");
	CString sFileF="actives"CONFIG_EXT;
	dir.AddDir(sPRoot,sFileF,TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,TRUE);
	for (int i=0;i<dir.GetSize();i++){
		CString sFile=dir[i].GetFileRoot();
		sFile.Replace(sPRoot,"");
		sFile.TrimRight("\\/");
		if(sFile!=""){
			aPres.Add(sFile);
		}
	}
}

void CSettings::SaveActiveWPT()
{
	CString sActWpt=GetHTMLConfigFile("actives"CONFIG_EXT,objSettings.sActivePreset);
	CDataXMLSaver IniFile(sActWpt,"application-config",FALSE);
	int iWPTNum=0;
	for(int k=0;k<TemplatesInWork.GetSize();k++){
		if(GetWPT(TemplatesInWork[k])){
			IniFile.putValue(Format("wchr-HTML_WPT%lu",iWPTNum++),TemplatesInWork[k]);
		}
	}
	/*
	CString aWPTsFreq;
	for(int h=0;h<objSettings.aLoadedWPTs.GetSize();h++){
		CWPT* wpt=GetWPT(objSettings.aLoadedWPTs[h]);
		if(wpt){
			aWPTsFreq+=Format("%s{%i};",wpt->sFileName,wpt->GetWeight());
		}
	}
	IniFile.putValue("wchr-wptsFreq",aWPTsFreq);
	*/
	IniFile.putValue("wchr-disableNotes",lDisableNotes);
}

void CSettings::Load()
{
	if(USE_ELEFUN){
		long lRestoreOnResChangeDef=1;
	}
	//Читаем настройки
	CDataXMLSaver IniFile(sIniFile,"application-config",TRUE);
	//
	if(!IniFile.IsFileLoaded()){
		bStartWithOptions=TRUE;
		lVeryFirstStart=TRUE;
	}
	IniFile.getValue("uint-language",				GetApplicationLang(),0);
	if(bStartWithOptions){
		int iLang=0;
		CStringArray aLangBufferNames;
		int iLangsCount=GetBuffersLang(aLangBufferNames,iLang);
		// Нет главной программы. спрашиваем стартовый язык пользователя!!!
		if(iLangsCount>1){
			CDLG_Chooser dlgChooser(CString(AppName()+" - ")+_l("Select language"),_l("Choose your language"),&aLangBufferNames, GetUserDefaultLangID()==0x0419?1:0, MAIN_ICON,NULL);
			iLang=dlgChooser.DoModal();
		}
		if(iLang>=0){
			GetApplicationLang()=iLang;
		}
	}
	CString sVer;
	IniFile.getValue("cur-version",					sVer);
	if(sVer!=PROG_VERSION && !bStartWithOptions){
		FORK(WhatsNew,NULL);
	}
	//
	IniFile.getValue("uint-changeWPInScrSaverMode",	bChangetWPIfScrSaver);
	IniFile.getValue("uint-skipChangeInMemDefic",	lChangeIfMemoE,DEFAULT_MEMPROE);
	IniFile.getValue("uint-skipChangeInMemDeficP",	lChangeIfMemoP,DEFAULT_MEMPROC);
	IniFile.getValue("uint-skipChangeInCPUDefic",	lChangeIfCPUE,DEFAULT_CPUPROE);
	IniFile.getValue("uint-skipChangeInCPUDeficP",	lChangeIfCPUP,DEFAULT_CPUPROC);
	IniFile.getValue("uint-skipChangeInWndPresent",	sStopTitles);
	IniFile.getValue("uint-themeSyncCount",			lSyncCount);
	IniFile.getValue("uint-themeSyncRemoveBroken",	lRemBrokenOnSync,1);
	IniFile.getValue("uint-skipHelpToDup",			lShowDupHelp);
	IniFile.getValue("uint-stickWindowsThreshold",	iStickPix,10);
	IniFile.getValue("uint-onExitFromMainWnd",		lOnExitFromMainWnd);
	IniFile.getValue("uint-onWallpaperExchange",	lOnWallExchange);
	IniFile.getValue("uint-loadDirsRecursively",	bLoadDirsRecursively,2);
	IniFile.getValue("uint-appStarts",				lStartsCount);
	IniFile.getValue("uint-appWPChanges",			lChangesCount);
	IniFile.getValue("uint-setDefUserWp",			bSetLastWallpaperForDefUserToo);
	IniFile.getValue("uint-checkForUpdates",		lCheckForUpdate,1);
	IniFile.getValue("uint-checkForUpdDays",		lDaysBetweenChecks,20);
	IniFile.getValue("uint-showPopupsOnErr",		lShowPopupsOnErr,1);
	IniFile.getValue("uint-showPopupsOnTBar",		lAlertsOnTaskbar,1);
	IniFile.getValue("uint-treatUnwthemesRight",	iTreatUnwtRight,1);
	IniFile.getValue("uint-reuseBrowser",			lReuseBrowser);
	IniFile.getValue("uint-watchWallpaper",			lTrackWallpaperChanges,1);
	IniFile.getValue("uint-actOnDblClick",			lActOnDblClick,2);
	IniFile.getValue("uint-enableSimpleMoving",		lenableSimpleMoving,1);
#ifdef ART_VERSION
	lActOnDblClick=1;
	lenableSimpleMoving=1;
#endif
	IniFile.getValue("uint-watchWallpaperSaveJpg",	lTrackWallpaperSaveJpg,1);
	IniFile.getValue("uint-watchWallpaperSaveJpQ",	lTrackWallpaperSaveJpgQ,90);
	IniFile.getValue("uint-changeInLowP",			lChangeInLowP);
	IniFile.getValue("uint-changeOnResChange",		lRestoreOnResChange,lRestoreOnResChangeDef);
	IniFile.getValue("uint-disableNotesBeforeSD",	lDisableNotesBeforeExit,1);
#ifdef ART_VERSION
	lDisableNotesBeforeExit=2;
#endif
	IniFile.getValue("wchr-makeDesktopIconsTrans",	bTrasparDeskIcons,DEF_PROCEICONS);
	IniFile.getValue("wchr-autoAddExtWP",			lAutoAddExternalWP);
	IniFile.getValue("wchr-changeOnStart",			bChangeWPaperOnStart,DEF_CHONSTART);
	IniFile.getValue("wchr-changeOnStartRegen",		bOnStartRegerate);
	IniFile.getValue("wchr-previewImage",			bUsePreview,1);
	IniFile.getValue("wchr-previewDelay",			bEnablePreviewDelay);
	IniFile.getValue("wchr-previewDelayMs",			bEnablePreviewDelayMs,DEF_PRVIEWDELAY);
	IniFile.getValue("wchr-previewSize",			bPreviewSize);
	IniFile.getValue("wchr-previewDblToRename",		lDblToRename);
	IniFile.getValue("wchr-column0Width",			iColWChWidth,300);
	IniFile.getValue("wchr-supportHTML",			bSupportHTMLWP,DEFAULT_SUPPHTML);
	IniFile.getValue("wchr-changeTrayIconOnWP",		bChangeTrayIconOnWP,1);
	IniFile.getValue("wchr-showTrayIconAsWP",		bShowTrayIconAsWP,DEF_TRAYICON);
	IniFile.getValue("wchr-HTMLTimeout",			iHTMLTimeout,DEF_HTMLTIMEOUT);
	IniFile.getValue("wchr-activePreset",			sActivePreset);
	CString sDefIcoFont;
	IniFile.getValue("wchr-SaveTo",					sWChangerFile,"wchanger.xml");
	IniFile.getValue("wchr-reminderWav",			sReminderWav,"reminder.wav");
	IniFile.getValue("wchr-reminderDefView",		bDefViewCalendar);
	IniFile.getValue("wchr-reminderDefShowWNums",	bDefViewWNums);
	
	IniFile.getValue("wchr-reminderWavLoop",		lLoopRemidersSound);
	IniFile.getValue("wchr-showRIconOnNotes",		lShowRIconOnNotes,1);
	IniFile.getValue("wchr-wallpaperSystemFile",	sWallpaperFile,DEFAULT_WPFILE);
	IniFile.getValue("wchr-openDir",				sPictInitDir,szSystemFolder);
	IniFile.getValue("wchr-openDir2",				sIcoInitDir);
	IniFile.getValue("wchr-autoStretchLim",			dwAutoStretchLim,DEFAULT_WPSTRLIM);
	IniFile.getValue("wchr-changePeriodically",		lChangePriodically,1);
	IniFile.getValue("wchr-hideConfidDataInLock",	lHideConfDataInLock);
	IniFile.getValue("wchr-hideConBmpDataInLock2",	sHideConfDataInLock2);
	IniFile.getValue("wchr-changePeriodMin",		lChangePeriod,DEFAULT_PERIODMIN);
	IniFile.getValue("wchr-wallpaperBgColor",		lWPBGColor,GetSysColor(COLOR_BACKGROUND));
	IniFile.getValue("wchr-wallpaperTxColor",		lWPTXColor,DEFICONTCOLOR);
	IniFile.getValue("wchr-wallpaperBgiColor",		lWPIBGColor,GetSysColor(COLOR_BACKGROUND));
	IniFile.getValue("wchr-changeDicoFont",			lChangeDicoFont);
	IniFile.getValue("wchr-defIconsFont",			sDefIcoFont);
	if(pDicoFont){
		delete pDicoFont;
		pDicoFont=NULL;
	}
	pDicoFont=CreateFontFromStr(sDefIcoFont,		NULL, NULL, NULL, NULL);
	IniFile.getValue("wchr-previewQuality",			bPreviewQuality);
	IniFile.getValue("wchr-trimImagePaths",			bUsePathEllipses,1);
	IniFile.getValue("wchr-cycleType",				bChangeWPaperCycleType);
	IniFile.getValue("wchr-cycleTypeMM",			iCurrentMultyOrderStyle);
	IniFile.getValue("wchr-tileOrCenter",			bTileOrCenter,1);
	IniFile.getValue("wchr-dotagivatBG",			bDotajkaImages,0);
	IniFile.getValue("wchr-randBGDot",				bDotajkaRand,1);
	IniFile.getValue("wchr-randBGDotBrd",			bDotajkaRandImgBrd,1);
	IniFile.getValue("wchr-enableEnliver",			lEnliverON);
	IniFile.getValue("wchr-showEnliver",			lShowAnimationItems);
	IniFile.getValue("wchr-desktopTooltips",		lDeskTooltips,1);
	IniFile.getValue("wchr-lastWallpaperNumber",	lCurPicture);
	IniFile.getValue("wchr-stretchingQuality",		bFineStretching,1);
	IniFile.getValue("wchr-reportBrokenLinks",		bReportBrokenLinks);
	IniFile.getValue("wchr-reportAddWPNow",			lReportAddWPNow);
	IniFile.getValue("wchr-reportActiveDeskChange",	bReportActiveDesktopChange);
	IniFile.getValue("wchr-reportLongImProc",		bReportLongImageProcs);
	IniFile.getValue("wchr-applyingChanges",		lApplyingChangesWarn);
	IniFile.getValue("wchr-delThemeWarn",			lDelThemeWarn);
	IniFile.getValue("wchr-delNoteWarn",			lDelNoteWarn);
	IniFile.getValue("wchr-delImageWarn",			lDelImageWarn);
	IniFile.getValue("wchr-delClockWarn",			lDelClockWarn);
	IniFile.getValue("wchr-reportInfAlerts",		bReportInformAlerts);
	IniFile.getValue("wchr-exitAlert",				bExitAlert);
	IniFile.getValue("wchr-reportraHit",			bReportTraHit);
	IniFile.getValue("wchr-lasUsrWall",				sLastWallpaperFromUser);
	IniFile.getValue("wchr-lasUsrWallStr",			iLastStretchingFromUser);
	IniFile.getValue("wchr-lasUsrWallTile",			iTile);
	IniFile.getValue("wchr-scrTimeOut",				dwScrTimeout,10);
	IniFile.getValue("wchr-scrPreset",				sScrEffect);
	IniFile.getValue("wchr-scrPreset_Minilendar",	iScrEffect_ML,1);
	IniFile.getValue("wchr-scrPreset_Timestamp",	iScrEffect_TS,1);
	IniFile.getValue("wchr-scrExcTas",				lExcludeTaskbar);
	IniFile.getValue("uint-lEnableDebug",			lEnableDebug);
#ifdef ART_VERSION
	lExcludeTaskbar=1;
	bDotajkaImages=0;
	bDotajkaRand=0;
#endif
	{// Настройки заметок
		IniFile.getValue("wchr-notesFolder",		sNotesFolder);
		IniFile.getValue("wchr-warnFirstTiJE",		bFirstJustEditNote,1);
		IniFile.getValue("wchr-warnFirstMini",		bFirstMiniNote,1);
		IniFile.getValue("wchr-defNoteSkin",		lNoteSkin,DEF_NSKIN);
#ifdef ART_VERSION
		lNoteSkin=0;
#endif
		IniFile.getValue("wchr-defNoteIcon",		lDefaultNoteIcon,1);
#ifdef ART_VERSION
		lDefaultNoteIcon=0;
#endif
		IniFile.getValue("wchr-defNoteView",		lNoteView);
		#ifdef ART_VERSION
		objSettings.lNoteView=1;
		#endif
		IniFile.getValue("wchr-defNoteModif",		lDefNoteModifier);
		IniFile.getValue("wchr-defNoteType",		lDefautNoteType);
		IniFile.getValue("wchr-defNoteAWrap",		lAutoWrapNote,1);
		IniFile.getValue("wchr-autoSaveNote",		lAutoSaveNote);
		IniFile.getValue("wchr-customsLong",		iCustomsBuf,10);
		IniFile.getValue("wchr-hideIcons",			lHideIconsByDefault);
		IniFile.getValue("wchr-useTitleShadow",		lUseNoteTitleShadow,1);
		IniFile.getValue("wchr-useDirPosit",		lUseNoreDirectPositioning);
		
		
		int k=0;
		CString sCustomIcon;
		while(IniFile.getValue(Format("wchr-custIcon%lu",k),	sCustomIcon)){
			aCustomIcons.SetAtGrow(k++,sCustomIcon);
		}
		int k3=0;
		CString sCustomBg;
		while(IniFile.getValue(Format("wchr-custBg%lu",k3),	sCustomBg)){
			aCustomBg.SetAtGrow(k3++,sCustomBg);
		}
		// Шрифт заметок
		CString sDefNoteFont,sFontName="notes-font";
		IniFile.getValue("wchr-defNoteFont",		sDefNoteFont,"<name>notes-font</name><alpha value=\"50\"/><fontname>Arial</fontname><size value=\"-15\"/><weight value=\"400\"/>");
		int k2=0;
		CString sCustomFont;
		while(IniFile.getValue(Format("wchr-custFont%lu",k2),	sCustomFont)){
			aCustomFonts.SetAtGrow(k2++,sCustomFont);
		}
		// Другое
		DWORD dwOpacity=50;
		if(pNoteFont){
			delete pNoteFont;
			pNoteFont=NULL;
		}
		pNoteFont=CreateFontFromStr(sDefNoteFont, &sFontName, NULL, NULL, &dwOpacity);
		if(pNoteTFont){
			delete pNoteTFont;
			pNoteTFont=NULL;
		}
		DWORD dwOpacityT=50;
		CString sDefNoteTFont,sFontTName="notes-t-font";
		IniFile.getValue("wchr-defNoteTFont",		sDefNoteTFont, "<name>notes-t-font</name><alpha value=\"20\"/><autotrans value=\"0\"/><txtcolor value=\"0\"/><bgcolor value=\"0\"/><fontname>Tahoma</fontname><size value=\"-13\"/><weight value=\"700\"/><charset value=\"204\"/>");
		pNoteTFont=CreateFontFromStr(sDefNoteTFont, &sFontTName, NULL, NULL, &dwOpacityT);
		dwDefNoteOpacity=dwOpacity;
	}
	// Горячие клавиши
	for(int j=0;j<(sizeof(DefActionMapHK)/sizeof(DefActionMapHK[0]));j++){
		DefActionMapHK[j].Reset();
		IniFile.getValue(Format("uint-hotkey%i",j),	DefActionMapHK[j],DefActionMapHK_Preinit[j]);
	}
	//
	long lPreservedTimeout=0;//objSettings.lNextTimeChange
	IniFile.getValue("wchr-preservedTimeout",			lPreservedTimeout);
	if(bChangeWPaperOnStart){
		DWORD dwCurTick=GetTickCount();
		if(bChangeWPaperOnStart==5 && lPreservedTimeout>=0){
			lLastTimeChange=dwCurTick-(lChangePeriod*60*1000)+lPreservedTimeout;
		}else if(bChangeWPaperOnStart==3 || bChangeWPaperOnStart==4){
			lLastTimeChange=dwCurTick-(lChangePeriod*60*1000)+5*1000;
		}else{
			lLastTimeChange=dwCurTick-(lChangePeriod*60*1000)-5*1000;
		}
		bNeedFirstTimeChange=1;
	}
	// Вытаскиваем заметки
	LoadNotes("");
	// Все шаблоны....
	LoadTemplates("");
	// Параметры шаблонов
	LoadActiveWPT();
	// Активные зоны
	LoadZones();
	// Уже почти все сделано
	bSaveAllowed=1;
	// Сбрасываем настройки обоев в системе
	SetWallpaperStyle(1, 0);
	//--------------
	ApplySettings();
	lStartsCount++;
}

CString GetNotesFolder()
{
	CString sTargetDir=objSettings.sNotesFolder;
	sTargetDir.TrimLeft();
	sTargetDir.TrimRight();
	if(sTargetDir==""){
		sTargetDir=GetUserFolder()+WP_NOTES;
	}
	sTargetDir.TrimRight("\\");
	sTargetDir+="\\";
	CreateDirIfNotExist(sTargetDir);
	return sTargetDir;
}

CString GetPresetsFolder(CString sName)
{
	if(sName==""){
		return GetUserFolder();
	}else{
		CString sTargetDir=GetUserFolder()+"Presets";
		sTargetDir.TrimRight("\\");
		sTargetDir+="\\";
		CreateDirIfNotExist(sTargetDir);
		if(sName!="?"){
			sTargetDir+=sName;
			sTargetDir+="\\";
			CreateDirIfNotExist(sTargetDir);
		}
		return sTargetDir;
	}
}


void CSettings::Save()
{
	//Пишем настройки
	CDataXMLSaver IniFile(sIniFile,"application-config",FALSE);
	// UINT
	IniFile.putValue("uint-language",				GetApplicationLang());
	IniFile.putValue("cur-version",					PROG_VERSION);
	//
	IniFile.putValue("uint-changeWPInScrSaverMode",	bChangetWPIfScrSaver);
	IniFile.putValue("uint-loadDirsRecursively",	bLoadDirsRecursively);
	IniFile.putValue("uint-skipChangeInMemDefic",	lChangeIfMemoE);
	IniFile.putValue("uint-skipChangeInMemDeficP",	lChangeIfMemoP);
	IniFile.putValue("uint-skipChangeInCPUDefic",	lChangeIfCPUE);
	IniFile.putValue("uint-skipChangeInWndPresent",	sStopTitles);
	IniFile.putValue("uint-skipChangeInCPUDeficP",	lChangeIfCPUP);
	IniFile.putValue("uint-themeSyncCount",			lSyncCount);
	IniFile.putValue("uint-themeSyncRemoveBroken",	lRemBrokenOnSync);
	IniFile.putValue("uint-skipHelpToDup",			lShowDupHelp);
	IniFile.putValue("uint-appStarts",				lStartsCount);
	IniFile.putValue("uint-appWPChanges",			lChangesCount);
	IniFile.putValue("uint-stickWindowsThreshold",	iStickPix);
	IniFile.putValue("uint-onExitFromMainWnd",		lOnExitFromMainWnd);
	IniFile.putValue("uint-onWallpaperExchange",	lOnWallExchange);
	IniFile.putValue("uint-setDefUserWp",			bSetLastWallpaperForDefUserToo);
	IniFile.putValue("uint-checkForUpdates",		lCheckForUpdate);
	IniFile.putValue("uint-checkForUpdDays",		lDaysBetweenChecks);
	IniFile.putValue("uint-showPopupsOnErr",		lShowPopupsOnErr);
	IniFile.putValue("uint-showPopupsOnTBar",		lAlertsOnTaskbar);
	IniFile.putValue("uint-treatUnwthemesRight",	iTreatUnwtRight);
	IniFile.putValue("uint-reuseBrowser",			lReuseBrowser);
	IniFile.putValue("uint-watchWallpaper",			lTrackWallpaperChanges);
	IniFile.putValue("uint-actOnDblClick",			lActOnDblClick);
	IniFile.putValue("uint-enableSimpleMoving",		lenableSimpleMoving);
	IniFile.putValue("uint-watchWallpaperSaveJpg",	lTrackWallpaperSaveJpg);
	IniFile.putValue("uint-watchWallpaperSaveJpQ",	lTrackWallpaperSaveJpgQ);
	IniFile.putValue("uint-changeInLowP",			lChangeInLowP);
	IniFile.putValue("uint-changeOnResChange",		lRestoreOnResChange);
	IniFile.putValue("uint-disableNotesBeforeSD",	lDisableNotesBeforeExit);

	for(int j=0;j<(sizeof(DefActionMapHK)/sizeof(DefActionMapHK[0]));j++){
		IniFile.putValue(Format("uint-hotkey%i",j),	DefActionMapHK[j]);
	}
	// WCHR
	IniFile.putValue("wchr-autoAddExtWP",			lAutoAddExternalWP);
	IniFile.putValue("wchr-makeDesktopIconsTrans",	bTrasparDeskIcons);
	IniFile.putValue("wchr-SaveTo",					sWChangerFile);
	IniFile.putValue("wchr-reminderWav",			sReminderWav);
	IniFile.putValue("wchr-reminderDefView",		bDefViewCalendar);
	IniFile.putValue("wchr-reminderDefShowWNums",	bDefViewWNums);
	IniFile.putValue("wchr-reminderWavLoop",		lLoopRemidersSound);
	IniFile.putValue("wchr-showRIconOnNotes",		lShowRIconOnNotes);
	IniFile.putValue("wchr-column0Width",			iColWChWidth);
	IniFile.putValue("wchr-previewImage",			bUsePreview);
	IniFile.putValue("wchr-previewDelay",			bEnablePreviewDelay);
	IniFile.putValue("wchr-previewDelayMs",			bEnablePreviewDelayMs);
	IniFile.putValue("wchr-previewSize",			bPreviewSize);
	IniFile.putValue("wchr-previewDblToRename",		lDblToRename);
	IniFile.putValue("wchr-changeOnStart",			bChangeWPaperOnStart);
	IniFile.putValue("wchr-changeOnStartRegen",		bOnStartRegerate);
	IniFile.putValue("wchr-openDir",				sPictInitDir);
	IniFile.putValue("wchr-openDir2",				sIcoInitDir);
	IniFile.putValue("wchr-autoStretchLim",			dwAutoStretchLim);
	IniFile.putValue("wchr-wallpaperSystemFile",	sWallpaperFile);
	IniFile.putValue("wchr-supportHTML",			bSupportHTMLWP);
	IniFile.putValue("wchr-changeTrayIconOnWP",		bChangeTrayIconOnWP);
	IniFile.putValue("wchr-showTrayIconAsWP",		bShowTrayIconAsWP);
	IniFile.putValue("wchr-HTMLTimeout",			iHTMLTimeout);
	IniFile.putValue("wchr-activePreset",			sActivePreset);
	SaveActiveWPT();
	IniFile.putValue("wchr-changePeriodically",		lChangePriodically);
	IniFile.putValue("wchr-hideConfidDataInLock",	lHideConfDataInLock);
	IniFile.putValue("wchr-hideConBmpDataInLock2",	sHideConfDataInLock2);
	IniFile.putValue("wchr-changePeriodMin",		lChangePeriod);
	IniFile.putValue("wchr-wallpaperBgColor",		lWPBGColor);
	IniFile.putValue("wchr-wallpaperTxColor",		lWPTXColor);
	IniFile.putValue("wchr-wallpaperBgiColor",		lWPIBGColor);
	IniFile.putValue("wchr-defIconsFont",			CreateStrFromFont(pDicoFont,"icons-font",0,0,100),XMLNoConversion);
	IniFile.putValue("wchr-changeDicoFont",			lChangeDicoFont);
	IniFile.putValue("wchr-previewQuality",			bPreviewQuality);
	IniFile.putValue("wchr-trimImagePaths",			bUsePathEllipses);
	IniFile.putValue("wchr-cycleType",				bChangeWPaperCycleType);
	IniFile.putValue("wchr-cycleTypeMM",			iCurrentMultyOrderStyle);
	IniFile.putValue("wchr-tileOrCenter",			bTileOrCenter);
	IniFile.putValue("wchr-dotagivatBG",			bDotajkaImages);
	IniFile.putValue("wchr-randBGDot",				bDotajkaRand);
	IniFile.putValue("wchr-randBGDotBrd",			bDotajkaRandImgBrd);
	IniFile.putValue("wchr-enableEnliver",			lEnliverON);
	IniFile.putValue("wchr-showEnliver",			lShowAnimationItems);
	IniFile.putValue("wchr-desktopTooltips",		lDeskTooltips);
	IniFile.putValue("wchr-lastWallpaperNumber",	lCurPicture);
	IniFile.putValue("wchr-stretchingQuality",		bFineStretching);
	IniFile.putValue("wchr-reportBrokenLinks",		bReportBrokenLinks);
	IniFile.putValue("wchr-reportAddWPNow",			lReportAddWPNow);
	IniFile.putValue("wchr-reportActiveDeskChange",	bReportActiveDesktopChange);
	IniFile.putValue("wchr-reportLongImProc",		bReportLongImageProcs);
	IniFile.putValue("wchr-applyingChanges",		lApplyingChangesWarn);
	IniFile.putValue("wchr-delThemeWarn",			lDelThemeWarn);
	IniFile.putValue("wchr-delNoteWarn",			lDelNoteWarn);
	IniFile.putValue("wchr-delImageWarn",			lDelImageWarn);
	IniFile.putValue("wchr-delClockWarn",			lDelClockWarn);
	IniFile.putValue("wchr-reportInfAlerts",		bReportInformAlerts);
	IniFile.putValue("wchr-exitAlert",				bExitAlert);
	IniFile.putValue("wchr-reportraHit",			bReportTraHit);
	IniFile.putValue("wchr-lasUsrWall",				sLastWallpaperFromUser);
	IniFile.putValue("wchr-lasUsrWallStr",			iLastStretchingFromUser);
	IniFile.putValue("wchr-lasUsrWallTile",			iTile);
	IniFile.putValue("wchr-scrTimeOut",				dwScrTimeout);
	IniFile.putValue("wchr-scrPreset",				sScrEffect);
	IniFile.putValue("wchr-scrPreset_Minilendar",	iScrEffect_ML);
	IniFile.putValue("wchr-scrPreset_Timestamp",	iScrEffect_TS);
	IniFile.putValue("wchr-scrExcTas",				lExcludeTaskbar);
	IniFile.putValue("uint-lEnableDebug",			lEnableDebug);
	IniFile.putValue("wchr-notesFolder",			sNotesFolder);
	IniFile.putValue("wchr-warnFirstTiJE",			bFirstJustEditNote);
	IniFile.putValue("wchr-warnFirstMini",			bFirstMiniNote);
	IniFile.putValue("wchr-defNoteSkin",			lNoteSkin);
	IniFile.putValue("wchr-defNoteIcon",			lDefaultNoteIcon);
	IniFile.putValue("wchr-defNoteView",			lNoteView);
	IniFile.putValue("wchr-defNoteModif",			lDefNoteModifier);
	IniFile.putValue("wchr-defNoteType",			lDefautNoteType);
	IniFile.putValue("wchr-useTitleShadow",			lUseNoteTitleShadow);
	IniFile.putValue("wchr-useDirPosit",			lUseNoreDirectPositioning);
	IniFile.putValue("wchr-defNoteFont",			CreateStrFromFont(pNoteFont,"notes-font",0,0,dwDefNoteOpacity),XMLNoConversion);
	IniFile.putValue("wchr-defNoteTFont",			CreateStrFromFont(pNoteTFont,"notes-t-font",0,0,dwDefNoteOpacity),XMLNoConversion);
	IniFile.putValue("wchr-defNoteAWrap",			lAutoWrapNote);
	IniFile.putValue("wchr-autoSaveNote",			lAutoSaveNote);
	IniFile.putValue("wchr-customsLong",			iCustomsBuf);
	IniFile.putValue("wchr-hideIcons",				lHideIconsByDefault);
	long lPreservedTimeout=objSettings.lNextTimeChange-GetTickCount();
	IniFile.putValue("wchr-preservedTimeout",			lPreservedTimeout);
	for(int k2=0;k2<aCustomIcons.GetSize();k2++){
		IniFile.putValue(Format("wchr-custIcon%lu",k2),	aCustomIcons[k2]);
	}
	for(int k4=0;k4<aCustomBg.GetSize();k4++){
		IniFile.putValue(Format("wchr-custBg%lu",k4),	aCustomBg[k4]);
	}
	for(int k3=0;k3<aCustomFonts.GetSize();k3++){
		IniFile.putValue(Format("wchr-custFont%lu",k3),	aCustomFonts[k3]);
	}
	SaveZones();
}

extern CCriticalSection csActZones;
void CSettings::SaveZones()
{
	//Пишем настройки
	CSmartLock lActZones(&csActZones,TRUE);
	CString sZonesIni=getFileFullName(GetDesktopPrefix()+"zonestmp.conf");
	CDataXMLSaver IniFile(sZonesIni,"active-zones",FALSE);
	// Сохраняем если небыло опции "скрывать конфидент"
	for(int i=0;i<aActiveZonesRect.GetSize();i++){
		IniFile.putValue(Format("zone%i-l",i),aActiveZonesRect[i].left);
		IniFile.putValue(Format("zone%i-t",i),aActiveZonesRect[i].top);
		IniFile.putValue(Format("zone%i-r",i),aActiveZonesRect[i].right);
		IniFile.putValue(Format("zone%i-b",i),aActiveZonesRect[i].bottom);
		IniFile.putValue(Format("zone%i-t",i),aActiveZonesActions[i]);
	}
}

void CSettings::LoadZones()
{
	//Пишем настройки
	CString sZonesIni=getFileFullName(GetDesktopPrefix()+"zonestmp.conf");
	CDataXMLSaver IniFile(sZonesIni,"active-zones",TRUE);
	// UINT
	int i=0;
	while(i>=0 && i<100){
		CString sText;
		CRect rZone(0,0,0,0);
		IniFile.getValue(Format("zone%i-l",i),rZone.left);
		IniFile.getValue(Format("zone%i-t",i),rZone.top);
		IniFile.getValue(Format("zone%i-r",i),rZone.right);
		IniFile.getValue(Format("zone%i-b",i),rZone.bottom);
		IniFile.getValue(Format("zone%i-t",i),sText);
		if(!rZone.IsRectEmpty() && sText!=""){
			CSmartLock lActZones(&csActZones,TRUE);
			aActiveZonesRect.SetAtGrow(i,rZone);
			aActiveZonesActions.SetAtGrow(i,sText);
		}else{
			break;
		}
		i++;
	}
}

void CSettings::Finalize()
{
	static BOOL bFinalized=0;
	if(bFinalized){
		return;
	}
	bFinalized=1;
	if(hEnliver){
		_StopEnliver fp=(_StopEnliver)GetProcAddress(hEnliver,"StopEnliver");
		if(fp){
			(*fp)();
		}
		FreeLibrary(hEnliver);
	}
	if(hUniquer){
		FreeLibrary(hUniquer);
	}
	if(hRemin){
		_StopDLL fp0=(_StopDLL)GetProcAddress(hRemin,"StopDLL");
		if(fp0){
			(*fp0)();
		}
		FreeLibrary(hRemin);
	}
	if(hHookLib){
		FreeLibrary(hHookLib);
	}
	
	if(browser){
		// На всякий случай...
		OleInitialize(NULL);
		delete browser;
		browser=NULL;
	}
	if(bRunStatus!=-1){
		bRunStatus=-1;
		SaveAll();
	}
	if(pNoteFont){
		delete pNoteFont;
		pNoteFont=NULL;
	}
	if(pNoteTFont){
		delete pNoteTFont;
		pNoteTFont=NULL;
	}
	if(pDicoFont){
		delete pDicoFont;
		pDicoFont=NULL;
	}
	ClearWPTs();
}

CSettings::~CSettings()
{
	Finalize();
};

void CSettings::ApplySettings()
{
	objSettings.sBMPFileName="";// Чтобы перечиталось при след. запросе
	return;
}

void CSettings::SaveAll()
{
	if(bSaveAllowed==0){
		return;
	}
	HCURSOR hCur=SetCursor(theApp.LoadCursor(IDC_CURSORSAVE));
	//m_WChanger.Save();// Привыходеиз окна
	this->Save();
	SetCursor(hCur);
}

BOOL bFileDialogOpened=FALSE;
BOOL OpenFileDialog(const char* szExtension, LPVOID lpData, CDLG_Options* pDialog, BOOL bSkipFileCheck, BOOL bWithCString, CWnd* pParent, BOOL bOpenDialog, CStringArray* pMultyOut)
{
	if(bFileDialogOpened){
		return FALSE;
	}
	if(lpData==NULL){
		return FALSE;
	}
	CString sFile;
	HIROW hData=NULL;
	CString* sOutFileName=NULL;
	if(bWithCString){
		sOutFileName=(CString*)lpData;
		sFile=*sOutFileName;
	}else{
		hData=(HIROW)lpData;
		sFile=pDialog->m_OptionsList.GetIItemText(hData,1);
	}
	BOOL bRes=FALSE;
	bFileDialogOpened=TRUE;
	char szFileNameBuffer[32*1024]="";
	strcpy(szFileNameBuffer,bSkipFileCheck?(""):(sFile));
	CFileDialog dlg(bOpenDialog, NULL, szFileNameBuffer, OFN_NODEREFERENCELINKS|(pMultyOut!=0?OFN_ALLOWMULTISELECT:0), szExtension, pDialog?pDialog:pParent);
	if(pMultyOut!=0){
		dlg.m_ofn.lpstrFile=szFileNameBuffer;
		dlg.m_ofn.lpstrInitialDir=objSettings.sPictInitDir;
		dlg.m_ofn.nMaxFile=sizeof(szFileNameBuffer);
	}
	SimpleTracker Track(objSettings.bDoModalInProcess);
	if(dlg.DoModal()==IDOK){
		bRes=TRUE;
		CString sNewFile=dlg.GetPathName();
		if(pDialog && hData){
			pDialog->m_OptionsList.SetIItemText(hData,1,sNewFile);
			FLNM_ROWSTATE flnmRowState;
			flnmRowState.bCollapsed= FALSE;
			flnmRowState.dwData= 0;
			flnmRowState.strText=sNewFile;
			flnmRowState.hIRow= hData;
			flnmRowState.iIItem=1;
			LRESULT result;
			pDialog->OnEndEdit((NMHDR*)(&flnmRowState), &result);
		}else{
			if(pMultyOut){
				POSITION p=dlg.GetStartPosition();
				while(p){
					pMultyOut->Add(dlg.GetNextPathName(p));
				}
			}
			if(sOutFileName){
				*sOutFileName=sNewFile;
			}
		}
	}
	bFileDialogOpened=FALSE;
	if(!bRes && !bSkipFileCheck){
		// Пробуем без стартового имени файла...
		DWORD dwErr=CommDlgExtendedError();
		if(dwErr==12290){
			return OpenFileDialog(szExtension, lpData, pDialog, TRUE, bWithCString, pParent, bOpenDialog, pMultyOut);
		}
	}
	return bRes;
}

#define OC_LANG		1
#define OC_NETMS	2
#define OC_NETNB	3
#define OC_NETXPC	4
#define OC_NETONLINESTATUS 5
#define OC_SORT		6
#define OC_ICON		7
#define OC_FILTER	8
#define OC_LICDATA	9
#define OC_STARTUP	10
#define OC_NSKIN	11
#define OC_TACTIVE	12
#define OC_CLOCKS	13
#define OC_TEMPLS	14
#define OC_DESK		15
#define OC_SCR		16
#define OC_WPSFILE	17
#define OC_NOTFOL	18
#define OC_SCRPRES	19
BOOL CALLBACK ChooseAnyFile(HIROW hData, CDLG_Options* pDialog)
{
	return OpenFileDialog("Any files (*.*)|*.*||",hData,pDialog);
}

BOOL CALLBACK ChooseAnyIFile(HIROW hData, CDLG_Options* pDialog)
{
	// С предпросмотром
	CPreviewImage PreviewWnd;
	CPreviewFileDialog dlg(&PreviewWnd, TRUE, NULL, NULL, OFN_FILEMUSTEXIST, "Bitmaps (*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx;*.tif;*.tga;*.tiff;*.ico)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx;*.tif;*.tga;*.tiff;*.ico|Vector graphics (*.wmf)|*.wmf|Html pages (*.htm;*.html)|*.htm;*.html|All Files (*.*)|*.*||", pDialog);
	char szFileNameBuffer[32*1024]="";
	dlg.m_ofn.lpstrFile=szFileNameBuffer;
	dlg.m_ofn.lpstrInitialDir=objSettings.sPictInitDir;
	dlg.m_ofn.nMaxFile=sizeof(szFileNameBuffer);
	SimpleTracker Track(objSettings.bDoModalInProcess);
	int iCounter=0;
	if(dlg.DoModal()!=IDOK){
		return FALSE;
	}else{
		objSettings.sPictInitDir=dlg.GetPathName();
		pDialog->m_OptionsList.SetIItemText(hData,1,objSettings.sPictInitDir);
		FLNM_ROWSTATE flnmRowState;
		flnmRowState.bCollapsed= FALSE;
		flnmRowState.dwData= 0;
		flnmRowState.strText=objSettings.sPictInitDir;
		flnmRowState.hIRow= hData;
		flnmRowState.iIItem=1;
		LRESULT result;
		pDialog->OnEndEdit((NMHDR*)(&flnmRowState), &result);
	}
	return TRUE;
};

BOOL CALLBACK ChooseDir(HIROW hData, CDLG_Options* pDialog)
{
	if(bFileDialogOpened){
		return FALSE;
	}
	if(!pDialog){
		return FALSE;
	}
	BOOL bRes=FALSE;
	bFileDialogOpened=TRUE;
	char szDir[MAX_PATH]="";
	char szFolderBuffer[MAX_PATH]="";
	GetSpecialFolderPath(szFolderBuffer,CSIDL_DESKTOP);
	strcpy(szDir,szFolderBuffer);
	if(GetFolder(_l("Choose directory"), szDir, NULL, pDialog?pDialog->GetSafeHwnd():NULL)){
		if(szDir[strlen(szDir)-1]!='\\'){
			strcat(szDir,"\\");
		}
		if(pDialog){
			pDialog->m_OptionsList.SetIItemText(hData,1,szDir);
			FLNM_ROWSTATE flnmRowState;
			flnmRowState.bCollapsed= FALSE;
			flnmRowState.dwData= 0;
			flnmRowState.strText=szDir;
			flnmRowState.hIRow= hData;
			flnmRowState.iIItem=1;
			LRESULT result;
			pDialog->OnEndEdit((NMHDR*)(&flnmRowState), &result);
			bRes=TRUE;
		}
	}
	bFileDialogOpened=FALSE;
	return bRes;
}

BOOL CALLBACK ChooseWavFile(HIROW hData, CDLG_Options* pDialog)
{
	return OpenFileDialog("Wav files (*.wav)|*.wav||",hData,pDialog);
}

BOOL CALLBACK TestSound(HIROW hData, CDLG_Options* pDialog)
{
	if(!pDialog){
		return FALSE;
	}
	CString sFile=pDialog->m_OptionsList.GetIItemText(hData,1);
	if(sFile!=""){
		AsyncPlaySound(sFile);
	}
	return TRUE;
}

long lLikKeysPasted=0;
void GetRegInfoFromText(CString &sText,CString& sLikUser,CString& sLikKey, const char* szSalt);
BOOL CALLBACK SubmitLikKeys(CString& sText, HIROW hData, CDLG_Options* pDialog)
{
	BOOL bRes=FALSE;
	if(!sText.IsEmpty()){
		CString sUser,sKey;
		GetRegInfoFromText(sText,sUser,sKey,0);
		if(sKey.IsEmpty() || sUser.IsEmpty()){
			bRes=FALSE;
		}else{
			bRes=TRUE;
			objSettings.sLikUser=sUser;
			objSettings.sLikKey=sKey;
			if(pDialog){
				pDialog->PushSomeDataOnScreen(OC_LICDATA);
			}
		}
	}
	if(pDialog){
		if(!bRes){
			Alert(_l("No registration info found")+"!\n"+_l("Please, enter registration data\n(registration email address and serial key)\nmanually"),_l("Registration"));
		}else{
			Alert(_l("Registration info pasted successfully")+"!",_l("Registration"));
		}
	}
	if(bRes){
		lLikKeysPasted=1;
	}
	return bRes;
}

BOOL CALLBACK OpenNoteSkins(HIROW hData, CDLG_Options* pDialog)
{
	static long lDWPLock=0;
	if(lDWPLock>0){
		return 0;
	}
	SimpleTracker Track(lDWPLock);
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wcskins&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	return TRUE;
}

BOOL CALLBACK DownloadWP(HIROW hData, CDLG_Options* pDialog)
{
	static long lDWPLock=0;
	if(lDWPLock>0){
		return 0;
	}
	SimpleTracker Track(lDWPLock);
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wallpapers&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	return TRUE;
}

BOOL CALLBACK DownloadWPT(HIROW hData, CDLG_Options* pDialog)
{
	static long lDWPLock=0;
	if(lDWPLock>0){
		return 0;
	}
	SimpleTracker Track(lDWPLock);
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wcwidgets&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	return TRUE;
}

BOOL CALLBACK DownloadWC(HIROW hData, CDLG_Options* pDialog)
{
	static long lDWPLock=0;
	if(lDWPLock>0){
		return 0;
	}
	SimpleTracker Track(lDWPLock);
	ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wcclocks&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	return TRUE;
}

BOOL CloneTemplate(int iTemplNum)
{
	objSettings.bTemplatesInWorkChanged=1;
	CString sSourceFile=GetApplicationDir();
	sSourceFile+=WP_TEMPLATE"\\";
	sSourceFile+=objSettings.aLoadedWPTs[iTemplNum];
	sSourceFile+=".wpt";
	CString sSrc;
	ReadFile(sSourceFile,sSrc);
	if(sSrc=="" || sSrc.Find("NO-DUPS")!=-1){
		Alert(_l("Sorry, this template can not be cloned.\nTry another one"),_l("Error"));
		return FALSE;
	}
	Alert(GetWPTTitle(iTemplNum)+" "+_l("cloned")+"!",6000);
	char szDisk[MAX_PATH]="C:\\", szPath[MAX_PATH]="", szName[MAX_PATH]="", szExt[MAX_PATH]="";
	_splitpath(sSourceFile, szDisk, szPath, szName, szExt);
	int iCount=0;
	CString sTarget;
	while(iCount<10){
		CString sTargetTempl=Format("%s"WP_IMPORTS"\\%s%s%i%s",GetUserFolder(), szName, DEF_CLONEPOST, iCount+2, szExt);
		if(isFileExist(sTargetTempl)){
			iCount++;
		}else{
			sTarget=sTargetTempl;
			break;
		}
	}
	if(sTarget){
		// Копируем...
		::CopyFile(sSourceFile,sTarget,FALSE);
		CString sPrep=Format(" #%i",iCount+2);
		// Название корректируем...
		LoadTemplates("");
	}
	if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
		pMainDialogWindow->dlgWChanger->PostMessage(WM_COMMAND,WM_USER+REFRTEMPL,LPARAM(-1));
	}
	return TRUE;
}

BOOL RemoveTemplate(int iCurSel)
{
	objSettings.bTemplatesInWorkChanged=1;
	int iRes=0;
	CString sWidget=objSettings.aLoadedWPTs[iCurSel];
	CString sWidgetName=GetWPTTitle(iCurSel);

	CString sSourceFile=GetApplicationDir();
	sSourceFile+=WP_TEMPLATE"\\";
	sSourceFile+=sWidget;
	sSourceFile+=".wpt";
	iRes+=::DeleteFile(sSourceFile);

	CString sImports=GetUserFolder()+WP_IMPORTS"\\";
	sImports+=sWidget;
	sImports+=".wpt";
	iRes+=::DeleteFile(sImports);

	if(iRes>0){
		LoadTemplates("");
		if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
			pMainDialogWindow->dlgWChanger->PostMessage(WM_COMMAND,WM_USER+REFRTEMPL,LPARAM(-1));
		}
		Alert(_l("Widget removed")+": "+sWidgetName,6000);
	}
	return TRUE;
}

BOOL CALLBACK DeleteTempl(HIROW hData, CDLG_Options* pDialog)
{
	if(AfxMessageBox(_l("Do you really want to delete this template?\nTemplate file will be removed from your disk too"),MB_YESNO|MB_ICONQUESTION)!=IDYES){
		return FALSE;
	}
	RemoveTemplate(int(hData));
	pDialog->AddChanged(OC_TEMPLS);
	pDialog->PostMessage(WM_COMMAND,IDAPPLY,0);
	return TRUE;
}

BOOL CALLBACK CloneTempl(HIROW hData, CDLG_Options* pDialog)
{
	CloneTemplate(int(hData));
	pDialog->AddChanged(OC_TEMPLS);
	pDialog->PostMessage(WM_COMMAND,IDAPPLY,0);
	return TRUE;
}

BOOL CALLBACK ChangeWTemplSetts(HIROW hData, CDLG_Options* pDialog)
{
	static long lDWPLock=0;
	if(lDWPLock>0 || !pMainDialogWindow){
		return 0;
	}
	SimpleTracker Track(lDWPLock);
	int iWptIndex=((int)hData);
	pMainDialogWindow->OnTemplParams(pDialog,objSettings.aLoadedWPTs[iWptIndex],"");
	return TRUE;
}

BOOL CALLBACK BuyWC(HIROW hData, CDLG_Options* pDialog)
{
	static long lDWPLock=0;
	if(lDWPLock>0){
		return 0;
	}
	int iRes=0;
	if(objSettings.iLikStatus==2 && !isTempKey()){
		iRes=(int)ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open","http://www.wiredplane.com/cgi-bin/wp_engine.php?target=none&who="+AppName()+""PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	}else{
		CString sURL=regGetBuyURL("WiredPlane",AppName(),"3");
		if(sURL==""){
			if(GetApplicationLang()==1){
				// Наши!!!
				sURL="http://www.wiredplane.com/cgi-bin/wp_engine.php?target=vendor-ru&who="+AppName()+""PROG_VERSION;
			}else{
				sURL="http://www.wiredplane.com/cgi-bin/wp_engine.php?target=vendor&who="+AppName()+""PROG_VERSION;
			}
		}
		iRes=(int)ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open",sURL,NULL,NULL,SW_SHOWNORMAL);
	}
	return TRUE;
}

BOOL CALLBACK OpenAnimProps(HIROW hData, CDLG_Options* pDialog)
{
	if(objSettings.hEnliver){
		_EnliverOptions fp=(_EnliverOptions)GetProcAddress(objSettings.hEnliver,"EnliverOptions");
		if(fp){
			(*fp)(hData!=NULL?(HWND)hData:pDialog->GetSafeHwnd());
		}
	}
	return TRUE;
}

BOOL CALLBACK PasteLikKeysFromFile(HIROW hData, CDLG_Options* pDialog)
{
	static long bLikPaste=0;
	if(bLikPaste>0){
		return FALSE;
	}
	SimpleTracker TrackLik(bLikPaste);
	if(!OpenFileDialog("Any files (*.*)|*.*||",&objSettings.sLikFile,pDialog,FALSE,TRUE)){
		return FALSE;
	}
	CString sText;
	if(objSettings.sLikFile!=""){
		ReadFile(objSettings.sLikFile,sText);
	}
	return SubmitLikKeys(sText,hData, pDialog);
}

BOOL CALLBACK PasteLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	static long bLikPaste=0;
	if(bLikPaste>0){
		return FALSE;
	}
	SimpleTracker TrackLik(bLikPaste);
	CString sText=GetClipboardText();
	return SubmitLikKeys(sText,hData, pDialog);
}

#define KSLEEP 25
BOOL CALLBACK CheckLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	static long bLikKDialog=0;
	if(bLikKDialog>0){
		return FALSE;
	}
	SimpleTracker Track(objSettings.bDoModalInProcess);
	SimpleTracker TrackLik(bLikKDialog);
	if(pDialog){
		pDialog->CommitData();
	}
	objSettings.sLikKey.TrimLeft();
	objSettings.sLikKey.TrimRight();
	objSettings.sLikUser.TrimLeft();
	objSettings.sLikUser.TrimRight();
	BOOL bRestart=0;
	if(!objSettings.sLikKey.IsEmpty() && !objSettings.sLikUser.IsEmpty()){
		BOOL bCheck=1;
		CString sError,sUsrName=objSettings.sLikUser;
		if(!USE_ELEFUN){
			if(bCheck && !isUserNameValid(sUsrName,sError)){
				Sleep(KSLEEP);
				Alert(_l("Invalid user name")+": "+_l(sError)+"!");
				return TRUE;// becouse of commit
			}
			if(bCheck && !isKeyValid(objSettings.sLikKey)){
				Sleep(KSLEEP);
				AlertBox(_l("Registration error"),_l("Invalid registration key")+"!",9000);
			}else{
				Sleep(KSLEEP);
				//Alert(_l("Application restarting to apply changes"),_l("Registration key accepted"));
				bRestart=1;
			}
		}
	}
	CString szKey;
	szKey="user:[";
	szKey+=objSettings.sLikUser;
	szKey+="],";
	szKey+="key:[";
	szKey+=objSettings.sLikKey;
	szKey+="]";
	/*
	szKey+="key:{";
	char szK[256]={0};
	EXECryptor_EncryptStr(objSettings.sLikKey,szK);
	szKey+=szK;
	szKey+="}";
	*/
	DWORD dwWritten=0;
	CString sLikFile=GetUserFolder()+LICENSE_KEY_FILE;
	DWORD dwLikOpenType=CREATE_NEW;
	if(isFileExist(sLikFile)){
		dwLikOpenType=TRUNCATE_EXISTING;
	}
	HANDLE hLicenseFile=::CreateFile(sLikFile, GENERIC_WRITE, 0, NULL, dwLikOpenType, 0, NULL);
	if (hLicenseFile && hLicenseFile!=INVALID_HANDLE_VALUE){
		::WriteFile(hLicenseFile,szKey,strlen(szKey),&dwWritten,NULL);
		::CloseHandle(hLicenseFile);
	}
	if(bRestart && pMainDialogWindow){
		RestartWC();
	}
	return TRUE;
}

void RestartWC()
{
	objSettings.lNoExitWarning=1;
	CString sExe=GetApplicationDir();
	sExe+=GetApplicationName();
	sExe+=".exe";
	int iRes=(int)::ShellExecute(0,"open",sExe,"-wait=yes",NULL,SW_SHOWNORMAL);
	pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
}

BOOL CALLBACK HowGetLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	ShowHelp("Registration");
	return TRUE;
}

typedef BOOL (WINAPI *_SetDeskFont)(LOGFONT&,DWORD dwText, DWORD dwBG); 
void ApplyDeskIconsOptions()
{
	if(objSettings.lChangeDicoFont && objSettings.bTrasparDeskIcons==0){
	    objSettings.bTrasparDeskIcons=1;
	}
	if(objSettings.bTrasparDeskIcons)
	{
		DWORD dwBgColor=objSettings.lWPIBGColor;
		SetDesktopIconsBg(objSettings.bTrasparDeskIcons>1,dwBgColor,objSettings.lWPTXColor);
		if(objSettings.hHookLib){
			_SetDeskFont fp=(_SetDeskFont)GetProcAddress(objSettings.hHookLib,"SetDeskFont");
			if(fp){
				LOGFONT logfont;
				memset(&logfont,0,sizeof(logfont));
				if(objSettings.lChangeDicoFont && objSettings.pDicoFont){
					objSettings.pDicoFont->GetLogFont(&logfont);
				}
				fp(logfont,objSettings.lWPTXColor,dwBgColor);
			}
		}
	}
	HideDeskIcons(objSettings.lHideIconsByDefault);
}

long iScrEffect=0;
BOOL CALLBACK ApplyOptionsFromDialog(BOOL bSaveAndClose,DWORD dwRes,CDLG_Options* dlgOpt)
{
	BOOL bRes=FALSE;
	if(dwRes==IDOK){
		// Обрабатываем
		if(dlgOpt->isOptionClassChanged(OC_SCRPRES)){
			CStringArray aPres;
			GetPresetList(aPres);
			if(iScrEffect>=0 && iScrEffect<=aPres.GetSize()){
				objSettings.sScrEffect=aPres[iScrEffect];
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_STARTUP)){
			StartupApplicationWithWindows(objSettings.bStartWithWindows);
		}
		if(dlgOpt->isOptionClassChanged(OC_SCR)){
			SetScreensaver(0);
		}
		if(dlgOpt->isOptionClassChanged(OC_DESK)){
			if(objSettings.bTrasparDeskIcons==2){
				CoInitialize(NULL);
				BOOL bActiveDEnabled=IsActiveDesctopEnable();
				if(bActiveDEnabled){
					SmartWarning(_l("Warning: Transparent icons with Active Desktop enabled can cause\nserious CPU hit on some systems. If this is happening\non your computer, please consider to disable desktop icons transparency"),AppName(),&objSettings.bReportTraHit,DEF_WARNTIME);
				}
				CoUninitialize();
			}
			ApplyDeskIconsOptions();
		}
		if(dlgOpt->isOptionClassChanged(OC_WPSFILE)){
			objSettings.m_WChanger.Save();
		}
		/*if(dlgOpt->isOptionClassChanged(OC_TACTIVE)){
			BOOL bChanged=0;
			for(int i=0;i<aActiveTemplates.GetSize();i++){
				BOOL bNew=aActiveTemplates[i];
				BOOL bOld=IsWPTActive(objSettings.aLoadedWPTs[i]);
				if(bNew!=bOld){
					bChanged=1;
					SwitchSingleTemplate(objSettings.aLoadedWPTs[i]);
				}
			}
			if(bChanged && pMainDialogWindow){
				pMainDialogWindow->OrderModifierForNextWallpaper=-2;
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
			}
		}*/
		if(dlgOpt->isOptionClassChanged(OC_ICON)){
			if(objSettings.bShowTrayIconAsWP==2 && !objSettings.DefActionMapHK[BRING_INTOVIEW].Present()){
				Alert(_l("You choose to hide tray icon without special hotkey defined.\nYou can press Ctrl-Alt-W to open main window"),_l(AppName()+": Warning"));
				objSettings.DefActionMapHK[BRING_INTOVIEW]=CIHotkey(393303);//Ctrl-Alt-W
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_NOTFOL)){
			SaveNotes();
		}
		if(dlgOpt->isOptionClassChanged(OC_NSKIN)){
			// Подгружаем скин если еще не...
			LoadNoteSkin(objSettings.lNoteSkin,objSettings.aNoteSkin[objSettings.lNoteSkin].sFolder,TRUE);
			// Обновляем все!
			if(pMainDialogWindow && objSettings.aDesktopNotes.GetSize()>0){
				pMainDialogWindow->OrderModifierForNextWallpaper=-2;
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_TEMPLS)){
			bRes=TRUE;
		}
		if(dlgOpt->isOptionClassChanged(OC_LANG)){
			bRes=TRUE;
		}
		objSettings.ApplySettings();
		if(dlgOpt->isOptionClassChanged(OC_LICDATA) || lLikKeysPasted){
			CheckLikKeys(NULL,NULL);
		}
		objSettings.Save();
	}
	return bRes;
}

BOOL SetScreensaver(BOOL bUI)
{
	if(AddDefsInteract()){
		CString sExe=GetApplicationDir();
		sExe+="Wired_Screensaver.exe";
		CString sPar=CString("/i /t=")+Format("%i",objSettings.dwScrTimeout)+(bUI?"":" /d");
		int iRes=(int)ShellExecute(0,"open",sExe,sPar,GetApplicationDir(),SW_SHOWNORMAL);
		//ERROR_FILE_NOT_FOUND
	}
	return 1;
}

BOOL CALLBACK SetScreensaver(HIROW hData, CDLG_Options* pDialog)
{
	SetScreensaver(1);
	return 0;
}

BOOL CALLBACK AddOptionsToDialog(CDLG_Options* pData)
{
	CDLG_Options* pDialog=pData;
	if(!pDialog){
		return FALSE;
	}
	pDialog->m_OptionsList.m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
	pDialog->m_OptionsList.m_crIRowBackground=GetSysColor(COLOR_BTNFACE);
	pDialog->m_OptionsList.SetBkColor(GetSysColor(COLOR_BTNFACE));
	pDialog->m_OptionsList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);
	pDialog->m_LList.m_crSelectedIRowBackground=GetSysColor(COLOR_BTNFACE);
	pDialog->m_LList.m_crSelectedIItemBackground=GetSysColor(COLOR_BTNFACE);

	// Чтобя перечитать все с новым языком
	CStringArray aLangBufferNames;
	int iTempLang=GetApplicationLang();
	GetBuffersLang(aLangBufferNames,iTempLang);
	////////////////////////
	// Задаем список опций
	////////////////////////
	// UINT
	pDialog->Option_AddMenuAction(_l("Download wallpapers"), DownloadWP, 0, 0);
	pDialog->Option_AddMenuAction(_l("Download templates"), DownloadWPT, 0, 0);
	pDialog->Option_AddMenuAction(_l("Download more clocks"), DownloadWC, 0, 0);
	pDialog->Option_AddMenuAction(_l("Download more skins"), OpenNoteSkins, 0, 0);
#ifndef GPL
	if(objSettings.iLikStatus<2 || isTempKey()){
		pDialog->Option_AddMenuAction(_l("Order "+AppName()+" now")+"!", BuyWC, 0, 0);
	}
#endif
	HIROW hCommon=pDialog->Option_AddHeader(FL_ROOT, _l("Common")+"\t"+_l("Interface options, tray icon and other common stuff"),61);
	{
		pDialog->Option_AddBOOL(hCommon,_l("Start with Windows"),&objSettings.bStartWithWindows, 1, 0, OC_STARTUP);
		pDialog->Option_AddCombo(hCommon,_l("User interface language"),&GetApplicationLang(),GetApplicationLang(),sLangsNames,OC_LANG);
		HIROW bRegen=pDialog->Option_AddCombo(hCommon,_l("What to do after start")+"\t"+_l("What to do after program start","What to do after program start - change wallpaper, change and exit and so on"),&objSettings.bChangeWPaperOnStart, DEF_CHONSTART, _l("Do nothing")+"\t"+_l("Change")+"\t"+_l("Change and Exit")+"\t"+_l("Wait and Change")+"\t"+_l("Wait, Change and Exit")+"\t"+_l("Change (keep period between restarts)"));
		pDialog->Option_AddBOOL(bRegen,_l("Regenerate existing wallpaper"),&objSettings.bOnStartRegerate, 0);
		pDialog->Option_AddCombo(hCommon,_l("Tray icon appearance")+"\t"+_l("What to show in tray","What to show in tray while "+AppName()+" is running - icon, minimized wallpaper or nothing"),&objSettings.bShowTrayIconAsWP, DEF_TRAYICON, _l("Simple icon")+"\t"+_l("Minimized wallpaper")+"\t"+_l("No icon at all"),OC_ICON);
		pDialog->Option_AddCombo(hCommon,_l("Action to perform on 'Close' button")+"\t"+_l("What to do when you press 'X' button","What to do when you press 'X' button (small button in the top-right corner of the main window)"),&objSettings.lOnExitFromMainWnd, 0, _l("Ask user")+"\t"+_l("Close program")+"\t"+_l("Minimize to tray"));
		pDialog->Option_AddBOOL(hCommon,_l("Blink tray icon while processing wallpaper")+"\t"+_l("Useful if you want to be informed","Useful if you want to be informed that new wallpaper is coming"),&objSettings.bChangeTrayIconOnWP, 1);
		{
			HIROW hHotKeys=pDialog->Option_AddHeader(hCommon,	_l("Hotkeys"),55);
			pDialog->Option_AddHotKey(hHotKeys,_l("Change wallpaper"),&objSettings.DefActionMapHK[NEXT_WALLPAPER],DefActionMapHK_Preinit[NEXT_WALLPAPER]);
#ifndef ART_VERSION
			pDialog->Option_AddHotKey(hHotKeys,_l("New desktop note"),&objSettings.DefActionMapHK[NEW_NOTE],DefActionMapHK_Preinit[NEW_NOTE]);
#endif
			pDialog->Option_AddHotKey(hHotKeys,_l("Show list of images")+"/"+_l("Widgets"),&objSettings.DefActionMapHK[BRING_INTOVIEW],DefActionMapHK_Preinit[BRING_INTOVIEW]);
			pDialog->Option_AddHotKey(hHotKeys,_l("Next wallpaper"),&objSettings.DefActionMapHK[ORDER_NEXT],DefActionMapHK_Preinit[ORDER_NEXT]);
			pDialog->Option_AddHotKey(hHotKeys,_l("Prev wallpaper"),&objSettings.DefActionMapHK[ORDER_PREV],DefActionMapHK_Preinit[ORDER_PREV]);
#ifndef ART_VERSION
			pDialog->Option_AddHotKey(hHotKeys,_l("Refresh current wallpaper"),&objSettings.DefActionMapHK[ORDER_SAME],DefActionMapHK_Preinit[NEXT_WALLPAPER]);
			pDialog->Option_AddHotKey(hHotKeys,_l("Hide/Show desktop icons"),&objSettings.DefActionMapHK[HIDE_ICONS],DefActionMapHK_Preinit[HIDE_ICONS]);
			pDialog->Option_AddHotKey(hHotKeys,_l("Hide/Show desktop widgets"),&objSettings.DefActionMapHK[HIDE_WIDGETS],DefActionMapHK_Preinit[HIDE_WIDGETS]);
			pDialog->Option_AddHotKey(hHotKeys,_l("Show calendar"),&objSettings.DefActionMapHK[OPEN_CALEN],DefActionMapHK_Preinit[OPEN_CALEN]);
#endif
		} 
#ifndef ART_VERSION
		{// Exceptions
			HIROW hWchrExc=pDialog->Option_AddHeader(hCommon, _l("Privacy")+"/"+_l("Exceptions")+"\t"+_l("Special conditions to temporarily stop image rotation"),60);
			pDialog->Option_AddCombo(hWchrExc,_l("When screensaver is running or computer is locked"),&objSettings.bChangetWPIfScrSaver, 0, _l("Do not change wallpaper")+"\t"+_l("Change in normal way")+"\t"+_l("Change only once"));
			HIROW hWchrMExc=pDialog->Option_AddBOOL(hWchrExc,_l("Don`t change if Memory usage threshold exceeded (percents)"),&objSettings.lChangeIfMemoE, DEFAULT_MEMPROE);
			pDialog->Option_AddNSlider(hWchrMExc,"",&objSettings.lChangeIfMemoP, DEFAULT_MEMPROC,0,100);
			HIROW hWchrCExc=pDialog->Option_AddBOOL(hWchrExc,_l("Don`t change if CPU usage threshold exceeded (percents)"),&objSettings.lChangeIfCPUE, DEFAULT_CPUPROE);
			pDialog->Option_AddNSlider(hWchrCExc,"",&objSettings.lChangeIfCPUP, DEFAULT_CPUPROC, 0, 100);
			pDialog->Option_AddString(hWchrExc,_l("Don`t change if matched windows are present")+"\t"+_l("Here you can enter titles of some windows. "+AppName()+" will not change wallpaper while those windows are present. Use ';' as delimiter, and '*' to mask title substrings. Example: *Explorer;*Media"),&objSettings.sStopTitles, "", 0);
			pDialog->Option_AddBOOL(hWchrExc,_l("Hide notes and other data on exit")+" "+_l("or shutdown"),&objSettings.lDisableNotesBeforeExit, 1);
			pDialog->Option_AddBOOL(hWchrExc,_l("Hide notes and other data while workstation is locked (Windows NT and higher only)"),&objSettings.lHideConfDataInLock, 0);
			HIROW hUBmp_f=pDialog->Option_AddString(hWchrExc,_l("Set my wallpaper while workstation is locked (Windows NT and higher only)")+"\t"+_l("You can specify image to use in case of hiding personal data in locked state. Leave this field empty to use last used wallpaper"),&objSettings.sHideConfDataInLock2, "", 0);
			pDialog->Option_AddActionToEdit(hUBmp_f,ChooseAnyIFile,hUBmp_f,0,70);
		}
#endif
		{
			HIROW hMisc=pDialog->Option_AddHeader(hCommon,	_l("System options"),71);
			pDialog->Option_AddBOOL(hMisc,_l("Show tooltip over interactive desktop elements"),&objSettings.lDeskTooltips, 1);
#ifndef ART_VERSION
			pDialog->Option_AddBOOL(hMisc,_l("Show desktop wallpaper on Windows Startup screen"),&objSettings.bSetLastWallpaperForDefUserToo, 0);
			pDialog->Option_AddBOOL(hMisc,_l("Remove broken links when synchronizing themes"),&objSettings.lRemBrokenOnSync, 1);
			pDialog->Option_AddBOOL(hMisc,_l("Track wallpaper changes and apply notes automatically")+", "+_l("Apply active templates also"),&objSettings.lTrackWallpaperChanges, 1);
			HIROW hJpgQ=pDialog->Option_AddBOOL(hMisc,_l("Save added images in JPG format. Quality level"),&objSettings.lTrackWallpaperSaveJpg, 1);
			pDialog->Option_AddNString(hJpgQ,"",&objSettings.lTrackWallpaperSaveJpgQ, 90, 1, 100);
#endif
#ifndef USE_LITE
			pDialog->Option_AddCombo(hMisc,_l("How often synchronize themes")+"\t"+_l("To use 'Folder modification time' option, image folder must be placed on NTFS drive"),&objSettings.lSyncCount,0,_l("At every start")+"\t"+_l("At every 3rd start")+"\t"+_l("At every 6th start")+(isWin98()?"":(CString("\t")+_l("Use folder modification time"))));
			pDialog->Option_AddCombo(hMisc,_l("Desktop: mouse click type to Edit notes/Activate widgets"),&objSettings.lActOnDblClick,2,_l("Disable")+"\t"+_l("Single click")+"\t"+_l("Double click"));
			pDialog->Option_AddCombo(hMisc,_l("Desktop: Enable simple drag-n-drop to move notes/widgets"),&objSettings.lenableSimpleMoving,1,_l("Disable")+"\t"+_l("Enable"));
#endif
			pDialog->Option_AddNString(hMisc,_l("Distance to stick windows together (pix)"),&objSettings.iStickPix, 10, 0, 30);
#ifndef ART_VERSION
			if(!USE_ELEFUN){
				HIROW hUpdates=pDialog->Option_AddBOOL(hMisc,_l("Check for updates every ... day(s)"),&objSettings.lCheckForUpdate, 1);
				pDialog->Option_AddNString(hUpdates,"",&objSettings.lDaysBetweenChecks, 20, 0, 300);
			}
			//pDialog->Option_AddBOOL(hMisc,_l("Allow desktop animations")+"\t"+_l("Funny animations for your desktop. This mode is still in beta stage of development")+". "+_l("Changes in this option ")+" "+_l("Effective after restart"),&objSettings.lShowAnimationItems, 0);
#endif
			HIROW hWPFile=pDialog->Option_AddString(hMisc,_l("Save wallpaper image into this file"),&objSettings.sWallpaperFile, DEFAULT_WPFILE);
			pDialog->Option_AddActionToEdit(hWPFile,ChooseAnyFile,hWPFile,0,70);
			HIROW hWPFileList=pDialog->Option_AddString(hMisc,_l("Save list of images into this file"),&objSettings.sWChangerFile, "wchanger.xml", OC_WPSFILE);
			pDialog->Option_AddActionToEdit(hWPFileList,ChooseAnyFile,hWPFileList,0,70);
#ifdef ART_VERSION
			pDialog->Option_AddNString(hMisc,_l("Change images in screen saver every ... seconds"),&objSettings.dwScrTimeout, 10, 0, 3000, OC_SCR);
#endif
#ifndef ART_VERSION
			pDialog->Option_AddBOOL(hMisc,_l("Log debug messages to disk")+"\t"+_l("Changes in this option ")+" "+_l("Effective after restart"),&objSettings.lEnableDebug, 0);
#endif
		}
		{
			HIROW hWarnings=pDialog->Option_AddHeader(hCommon, _l("Confirmations"),66);
#ifndef ART_VERSION
			if(AddDefsInteract()){
				pDialog->Option_AddCombo(hWarnings,_l("Wallpaper exchange service"),&objSettings.lOnWallExchange, 0, _l("Suggest exchange sometimes")+"\t"+_l("Approve wallpaper exchange")+"\t"+_l("Turn this service off"));
			}
			pDialog->Option_AddCombo(hWarnings,_l("Wallpapers installed from other applications"),&objSettings.lAutoAddExternalWP, 0, _l("Suggest to add them to your collection")+"\t"+_l("Add them automatically")+"\t"+_l("Do not add them"));
			pDialog->Option_AddBOOL(hWarnings,_l("Warning window: Place button on taskbar instead of tray icon"),&objSettings.lAlertsOnTaskbar, 1);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning on exit"),&objSettings.bExitAlert, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip information warnings"),&objSettings.bReportInformAlerts, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about Active Desktop enable/disable"),&objSettings.bReportActiveDesktopChange, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about deleting theme"),&objSettings.lDelThemeWarn, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about deleting note"),&objSettings.lDelNoteWarn, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about deleting clock"),&objSettings.lDelClockWarn, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip hints for 'Find duplicated images' module"),&objSettings.lShowDupHelp, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Show tray popup in difficult situation"),&objSettings.lShowPopupsOnErr, 1);
#endif
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about deleting image file"),&objSettings.lDelImageWarn, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about CPU hit on icon transparency"),&objSettings.bReportTraHit, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about broken image link"),&objSettings.bReportBrokenLinks, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about refreshing image list"),&objSettings.bReportLongImageProcs, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about applying changes"),&objSettings.lApplyingChangesWarn, 0);
			pDialog->Option_AddBOOL(hWarnings,_l("Skip warning about adding wallpaper"),&objSettings.lReportAddWPNow, 0);
		}
	}
	{//Wallpaper changer
		HIROW hWchr=pDialog->Option_AddHeader(FL_ROOT, _l(WALLPAPERLIST_TP),68);
		HIROW hChPer=pDialog->Option_AddBOOL(hWchr,_l("Change wallpaper periodically")+", "+_l("Generate wallpaper every ... min")+":",&objSettings.lChangePriodically, 1, 0);
		pDialog->Option_AddNString(hChPer,"",&objSettings.lChangePeriod, DEFAULT_PERIODMIN, 1, 10000);
		HIROW hSupHTML=pDialog->Option_AddBOOL(hWchr,_l("Render HTML wallpapers and URL links")+", "+_l("Max processing time (sec)")+":\t"+_l("Ability to load HTMLs and use templates","Ability to load HTMLs and use templates (slightly increases memory usage)")+", "+_l("When checked, HTMLs or images that are accessed by a URL link will be processed only if the maximum time you allot is not exceeded. If image generation takes longer than your limit, the wallpaper will not be changed."),&objSettings.bSupportHTMLWP, DEFAULT_SUPPHTML);
		pDialog->Option_AddNString(hSupHTML,"",&objSettings.iHTMLTimeout, DEF_HTMLTIMEOUT, 1, 9999);
		pDialog->Option_AddCombo(hWchr,_l("How to get next wallpaper"),&objSettings.bChangeWPaperCycleType,0,_l("Random")+"\t"+_l("Next in order"));
		if(GetNumberOfMons()>1){
			pDialog->Option_AddCombo(hWchr,_l("How to get next wallpaper for secondary monitor"),&objSettings.iCurrentMultyOrderStyle,0,_l("From main monitor")+"\t"+_l("Random")+"\t"+_l("Next in order"));
		}
		pDialog->Option_AddCombo(hWchr,_l("Quality of image resizing")+"\t"+_l("Image stretching quality","Image stretching quality, 'Fine' is the best but slow"),&objSettings.bFineStretching,1,_l("Normal")+"\t"+_l("Fine"));
		pDialog->Option_AddColor(hWchr,_l("Wallpaper background color"),&objSettings.lWPBGColor, GetSysColor(COLOR_BACKGROUND), 0, 67);
		pDialog->Option_AddCombo(hWchr,_l("Add folders with images")+": "+_l("Load with subfolders"),&objSettings.bLoadDirsRecursively, 2, _l("Ignore subfolders")+"\t"+_l("Check subfolders too")+"\t"+_l("Ask user"));
#ifndef ART_VERSION
		pDialog->Option_AddBOOL(hWchr,_l("Regenerate wallpaper on resolution change"),&objSettings.lRestoreOnResChange, lRestoreOnResChangeDef);
		pDialog->Option_AddBOOL(hWchr,_l("Generate wallpapers in low priority"),&objSettings.lChangeInLowP, 0);
		pDialog->Option_AddNString(hWchr,_l("Maximum stretching level for 'Auto-fit proportional' method of image resizing (percents)")+"\t"+_l("All images with higher stretching rate will not be stretched at all"),&objSettings.dwAutoStretchLim, DEFAULT_WPSTRLIM, 1, 1000);
		{
			HIROW hDotajka=pDialog->Option_AddHeader(hWchr, _l("Wallpaper gaps"),62);
			pDialog->Option_AddBOOL(hDotajka,_l("Exclude taskbar from wallpaper"),&objSettings.lExcludeTaskbar, 0);
			pDialog->Option_AddCombo(hDotajka,_l("How to fill borders around images"),&objSettings.bDotajkaImages, 0, _l("Fill with background color")+"\t"+_l("Use image edges infinitely")+"\t"+_l("Use mirrored image infinitely")+"\t"+_l("Use image edges in 3D")+"\t"+_l("Use mirrored image in 3D"));
			pDialog->Option_AddCombo(hDotajka,_l("Additional mixing in image borders"),&objSettings.bDotajkaRand,1,_l("None")+"\t"+_l("Water pattern")+"\t"+_l("Blend with background")+"\t"+_l("Blend with background using Water pattern"));
			pDialog->Option_AddCombo(hDotajka,_l("How to fill desktop with patterns")+" ("+_l("When applicable")+")",&objSettings.bTileOrCenter,1,_l("Center/Stretch")+"\t"+_l("Tile")+"\t"+_l("Mirror tiling")+"\t"+_l("Center only"));
			pDialog->Option_AddBOOL(hDotajka,_l("Mirroring: Use random part of image"),&objSettings.bDotajkaRandImgBrd, 1);
		}
#endif
		{// Preview options
			HIROW hPreview=pDialog->Option_AddHeader(hWchr, _l("Wallpapers list")+"\t"+_l("Main window options")+", "+_l("Images preview preferences"),62);
			pDialog->Option_AddBOOL(hPreview,_l("Image list: Trim image path and preserve file name")+"\t"+_l("How to show image files paths, reopen main window to see changes"),&objSettings.bUsePathEllipses, 1);
			pDialog->Option_AddBOOL(hPreview,_l("Image list, Double click: Rename image file (instead of setting as wallpaper) "),&objSettings.lDblToRename, 0);
#ifndef ART_VERSION
			pDialog->Option_AddBOOL(hPreview,_l("Non-weighted themes: assign weight according to the number of active images"),&objSettings.iTreatUnwtRight, 1);
#endif
			pDialog->Option_AddBOOL(hPreview,_l("Show selected image in the preview window"),&objSettings.bUsePreview, 1);
			HIROW hPreviewDel=pDialog->Option_AddBOOL(hPreview,_l("Delay loading preview")+" ("+_l("ms")+")",&objSettings.bEnablePreviewDelay, 0);
			pDialog->Option_AddNString(hPreviewDel,"",&objSettings.bEnablePreviewDelayMs, DEF_PRVIEWDELAY, 50, 5000);
			pDialog->Option_AddCombo(hPreview,_l("Preview generation method"),&objSettings.bPreviewQuality,0,_l("Fast (just image, no HTML)")+"\t"+_l("Normal (image or HTML)")+"\t"+_l("Full (with current template)"));
		}
	}
/*
	if(objSettings.aLoadedWPTs.GetSize()>0){// Templates
		aActiveTemplates.RemoveAll();
		HIROW hTempl=pDialog->Option_AddHeader(FL_ROOT, _l("Wallpaper templates")+"\t"+_l("Template`s options and settings"),74);
		int iCount=0;
		aActiveTemplates.SetSize(objSettings.aLoadedWPTs.GetSize());
		for (int i=0;i<objSettings.aLoadedWPTs.GetSize();i++){
			CString sTitle=GetWPTTitle(i);
			long lIsActive=IsWPTActive(objSettings.aLoadedWPTs[i]);
			aActiveTemplates[i]=lIsActive;
			pDialog->Option_AddBOOL(hTempl,sTitle, &aActiveTemplates[i], lIsActive, 0, OC_TACTIVE);
			pDialog->Option_AddAction(hTempl,_l("Change settings of")+" '"+sTitle+"'", ChangeWTemplSetts, HIROW(i), 0);
			if(objSettings.aLoadedWPTs[i].Find(DEF_CLONEPOST)==-1){
				pDialog->Option_AddAction(hTempl,_l("Make a copy of")+" '"+sTitle+"'", CloneTempl, HIROW(i), 0);
			}
			pDialog->Option_AddAction(hTempl,_l("Remove")+" '"+sTitle+"'", DeleteTempl, HIROW(i), 0);
		}
	}
*/
#if	!defined(USE_LITE) && !defined(ART_VERSION)
	{// Notes
		HIROW hDNotes=pDialog->Option_AddHeader(FL_ROOT, _l(DESKNOTE_TOPIC)+"\t"+_l("Quick notes, can be added simply by double-clicking desktop wallpaper"),64);
		if(objSettings.lNoteSkin>=objSettings.aNoteSkin.GetSize()){
			objSettings.lNoteSkin=0;
		}
		if(objSettings.aNoteSkin.GetSize()==0){
			CNoteSkin skin;
			skin.sTitle=_l("Default");
			objSettings.aNoteSkin.Add(skin);
		}
		CNoteSkin& skin0=objSettings.aNoteSkin[0];
		CString sNoteSkins=_getLngString(skin0.sTitle,NULL,skin0.sFolder,"strings")+((skin0.sAuthor!="")?(CString(", ")+_l("Author")+": "+skin0.sAuthor):"");
		if(objSettings.aNoteSkin.GetSize()>1){
			for(int i=1;i<objSettings.aNoteSkin.GetSize();i++){
				sNoteSkins+="\t";
				sNoteSkins+=_getLngString(objSettings.aNoteSkin[i].sTitle,NULL,objSettings.aNoteSkin[i].sFolder,"strings")+((objSettings.aNoteSkin[i].sAuthor!="")?(CString(", ")+_l("Author")+": "+objSettings.aNoteSkin[i].sAuthor):"");
			}
		}
		CString sNoteIcons=_l("None");
		CNoteSkin& skinN=objSettings.aNoteSkin[objSettings.lNoteSkin];
		if(skinN.aIconsTitles.GetSize()>0){
			CString sNoteIcons=_getLngString(skinN.aIconsTitles[0],NULL,skinN.sFolder,"strings");
			if(objSettings.aNoteSkin.GetSize()>1){
				for(int i=1;i<skinN.aIconsTitles.GetSize();i++){
					sNoteIcons+="\t";
					sNoteIcons+=_getLngString(skinN.aIconsTitles[i],NULL,skinN.sFolder,"strings");
				}
				sNoteIcons+="\t";
				sNoteIcons+=_l("Random");
			}
		}
		pDialog->Option_AddCombo(hDNotes,_l("Default note icon"),&objSettings.lDefaultNoteIcon, 1, sNoteIcons, 0);
		pDialog->Option_AddCombo(hDNotes,_l("Notes skin"),&objSettings.lNoteSkin, DEF_NSKIN, sNoteSkins, OC_NSKIN);
		pDialog->Option_AddCombo(hDNotes,_l("Layout of the 'Edit note' window"),&objSettings.lNoteView, 0, _l("Normal (with buttons)")+"\t"+_l("Text and title only"));
		pDialog->Option_AddCombo(hDNotes,_l("Default type of the new note"),&objSettings.lDefautNoteType, 0, _l("Normal (title and text)")+"\t"+_l("Title only")+"\t"+_l("Icon only")+"\t"+_l("Text only"));
		pDialog->Option_AddCombo(hDNotes,_l("Activate note on wallpaper"),&objSettings.lDefNoteModifier, 0, _l("Double-Click")+"\t"+_l("Shift + Double-Click")+"\t"+_l("Control + Double-Click")+"\t"+_l("Alt + Double-Click"));
		pDialog->Option_AddFont(hDNotes,_l("Note text font"),&objSettings.pNoteFont, OC_NSKIN, 69);
		pDialog->Option_AddFont(hDNotes,_l("Note title font"),&objSettings.pNoteTFont, OC_NSKIN, 69);
		pDialog->Option_AddBOOL(hDNotes,_l("Show note title with shadow"),&objSettings.lUseNoteTitleShadow, 1, 0, OC_NSKIN);
		pDialog->Option_AddNSlider(hDNotes,_l("Note opacity")+" ("+_l("percents")+")",&objSettings.dwDefNoteOpacity, 50, 1, 100, OC_NSKIN);
		pDialog->Option_AddNString(hDNotes,_l("How many items to remember in custom histories"),&objSettings.iCustomsBuf, 10, 1, 100, OC_NSKIN);
		HIROW hNOtFolder=pDialog->Option_AddString(hDNotes,_l("Notes folder")+"\t"+_l("Leave blank to use default value"),&objSettings.sNotesFolder, "",OC_NOTFOL);
		pDialog->Option_AddActionToEdit(hNOtFolder,ChooseDir,hNOtFolder,0,70);
		pDialog->Option_AddBOOL(hDNotes,_l("Wrap note text"),&objSettings.lAutoWrapNote, 1, 0, OC_NSKIN);
		pDialog->Option_AddBOOL(hDNotes,_l("Automatically save note when switching to another application"),&objSettings.lAutoSaveNote, 0);
		pDialog->Option_AddBOOL(hDNotes,_l("Enable precise note positioning"),&objSettings.lUseNoreDirectPositioning, 0, 0, OC_NSKIN);
		pDialog->Option_AddBOOL(hDNotes,_l("Disable desktop notes")+"\t"+_l("When disabled, all notes are removed from the desktop, but not destroyed - you can always reach them via tray menu"),&objSettings.lDisableNotes, 0);
		pDialog->Option_AddAction(hDNotes,_l("Download new note skins"), OpenNoteSkins, NULL);
	}
#ifndef ART_VERSION
		if(objSettings.hRemin){
			HIROW hReminders=pDialog->Option_AddHeader(FL_ROOT, _l("Calendar")+"/"+_l("Reminders"),34);
			pDialog->Option_AddCombo(hReminders,_l("Default tray calendar view"),&objSettings.bDefViewCalendar, 0, _l("Month")+"\t"+_l("Week"));
			pDialog->Option_AddBOOL(hReminders,_l("Show week numbers"),&objSettings.bDefViewWNums, 0);
			pDialog->Option_AddBOOL(hReminders,_l("Note reminder: show reminder icon over desktop note"),&objSettings.lShowRIconOnNotes, 1);
			HIROW hWPWav=pDialog->Option_AddString(hReminders,_l("Default reminder sound"),&objSettings.sReminderWav, "reminder.wav");
			pDialog->Option_AddActionToEdit(hWPWav,ChooseAnyFile,hWPWav,0,70);
			pDialog->Option_AddBOOL(hReminders,_l("Loop sound until reminder is dismissed"),&objSettings.lLoopRemidersSound, 0);
		}
#endif
	{
		HIROW hDicos=pDialog->Option_AddHeader(FL_ROOT,	_l("Desktop icons"),23);
		pDialog->Option_AddCombo(hDicos,_l("Process desktop icons"),&objSettings.bTrasparDeskIcons, DEF_PROCEICONS, _l("Do not process icons")+"\t"+_l("Change colors only")+"\t"+_l("Transparent background"), OC_DESK);
		pDialog->Option_AddColor(hDicos,_l("Icons text color"),&objSettings.lWPTXColor, DEFICONTCOLOR, OC_DESK, 67);
		pDialog->Option_AddColor(hDicos,_l("Icons background color"),&objSettings.lWPIBGColor, DEFICONBGTCOLOR, OC_DESK, 67);
		pDialog->Option_AddBOOL(hDicos,_l("Change desktop icons font"),&objSettings.lChangeDicoFont, 0, 0, OC_DESK);
		pDialog->Option_AddFont(hDicos,_l("Icons text font"),&objSettings.pDicoFont, OC_DESK, 69);
		pDialog->Option_AddBOOL(hDicos,_l("Hide desktop icons"),&objSettings.lHideIconsByDefault, 0, 0, OC_DESK);
	}
	{
		HIROW hSSave=pDialog->Option_AddHeader(FL_ROOT,	_l(SSAVE_TOPIC),24);
		pDialog->Option_AddNString(hSSave,_l("Change images every ... seconds"),&objSettings.dwScrTimeout, 10, 0, 3000, OC_SCR);
		CStringArray aPres;
		GetPresetList(aPres);
		CString sPresets=_l("Default");
		for(int i=1;i<aPres.GetSize();i++){
			sPresets+="\t";
			sPresets+=aPres[i];
			if(objSettings.sScrEffect==aPres[i]){
				iScrEffect=i;
			}
		}
		pDialog->Option_AddCombo(hSSave,_l("Use following preset"),&iScrEffect, 0, sPresets, OC_SCRPRES);
		
		pDialog->Option_AddBOOL(hSSave,_l("Add mini-calendar"),&objSettings.iScrEffect_ML, 1);
		pDialog->Option_AddBOOL(hSSave,_l("Add current time"),&objSettings.iScrEffect_TS, 1);
		if(AddDefsInteract()){
			pDialog->Option_AddAction(hSSave,_l("Install screensaver"), SetScreensaver, 0, 0);
		}
	}
	if(objSettings.hEnliver!=NULL && objSettings.lShowAnimationItems){// Enliver
		HIROW hAnim=pDialog->Option_AddHeader(FL_ROOT, _l("Desktop animation")+"\t"+_l("Different animations based of DirectX features"),76);
		pDialog->Option_AddBOOL(hAnim,_l("Enlive desktop")+" "+_l("with different animation effects")+" - "+_l("Very experimental at this time")+"\t"+_l("Add animation to your desktop. This option requires DirectX 7 installed"),&objSettings.lEnliverON, 0);
		pDialog->Option_AddAction(hAnim,_l("Open animation properties"), OpenAnimProps, NULL);
	}
#ifndef GPL
	if(!USE_ELEFUN){
		HIROW hRegistration=pDialog->Option_AddHeader(FL_ROOT,_l(REGISTRATION_TOPIC)+"\t"+_l("Registration information"),59);
		{
			if(objSettings.iLikStatus<2 || isTempKey()){
				CString sBenefits=REG_BENEFITS;
				sBenefits.Replace("\n"," ");
				pDialog->Option_AddAction(hRegistration,CString("*** ")+_l("Order now")+"! ***\t"+sBenefits+" "+_l("Click here to")+" "+_l("Start secure transaction immediately"), BuyWC, NULL, 0);
			}
			pDialog->Option_AddAction(hRegistration,_l("Insert from clipboard")+"\t"+_l("Use this to quickly register application")+": "+_l("Copy email with your registration info into clipboard and press this button"), PasteLikKeys, NULL, OC_LICDATA);
			pDialog->Option_AddAction(hRegistration,_l("Insert from file")+"\t"+_l("Use this to quickly register application")+": "+_l("Save email with your registration info into file and press this button"), PasteLikKeysFromFile, NULL, OC_LICDATA);
			pDialog->Option_AddString(hRegistration,_l("Registration email"),&objSettings.sLikUser, objSettings.sLikUser, OC_LICDATA);
			pDialog->Option_AddString(hRegistration,_l("Serial number"),&objSettings.sLikKey, objSettings.sLikKey, OC_LICDATA);
		}
	}
#endif
#endif
	if(isTempKey()){
		pDialog->m_pStartupInfo->szTooltipPostfix="\n---\n P.S.:"+_l("This copy is registered with time-limited key")+". "+_l("To get rid of all limitations, order full registration. See help for details.");
	}
	return TRUE;
}

CDLG_Options* g_dlgOptions=0;
void CSettings::OpenOptionsDialog(const char* szDefTopic, CWnd* pParentWnd)
{
	static long lOptionsDialogOpened=0;
	if(lOptionsDialogOpened){
		if(g_dlgOptions){
			SwitchToWindow(g_dlgOptions->GetSafeHwnd());
		}
		return;
	}
	SimpleTracker Track(lOptionsDialogOpened);
	SimpleTracker Track2(objSettings.bDoModalInProcess);
	if(pMainDialogWindow){
		pMainDialogWindow->UnRegisterMyHotKeys();
	}
	//
	int iLang=0;
	CStringArray aLangBufferNames;
	int iLangsCount=GetBuffersLang(aLangBufferNames,iLang);
	if(pMainDialogWindow){
		if(!pParentWnd){
			pParentWnd=pMainDialogWindow;
		}
	}else{
		// Нет главной программы. спрашиваем стартовый язык пользователя!!!
		if(iLangsCount>1){
			CDLG_Chooser dlgChooser(_l("Select language"),_l("Choose your language"),&aLangBufferNames, GetUserDefaultLangID()==0x0419?1:0, MAIN_ICON,NULL);
			iLang=dlgChooser.DoModal();
		}
		if(iLang>=0){
			GetApplicationLang()=iLang;
			return;
		}
	}
	CDLG_Options_Startup pStartupInfo;
	pStartupInfo.fpInitOptionList=AddOptionsToDialog;
	pStartupInfo.fpApplyOptionList=ApplyOptionsFromDialog;
	//pStartupInfo.fpUpMenuAction=ProcessUpMenu;???
	pStartupInfo.iStickPix=objSettings.iStickPix;
	pStartupInfo.szRegSaveKey=PROG_REGKEY"\\Wnd_Preferences";
	pStartupInfo.pImageList=&theApp.MainImageList;
	CString sOptionWndTitle=_l(OPTIONS_TP)+" - "+objSettings.sMainWindowTitle;
	pStartupInfo.szWindowTitle=sOptionWndTitle;
	pStartupInfo.iWindowIcon=MAIN_ICON;
	pStartupInfo.szDefaultTopicTitle=szDefTopic;
	pStartupInfo.sz1stTopic=_l("Options");
	pStartupInfo.bExpandByDef=TRUE;
	CDLG_Options dlgOpt(pStartupInfo,pParentWnd,1);
	dlgOpt.bShowFind=0;
	dlgOpt.m_bSelInBold=0;
	if(objSettings.bStartWithOptions){
		dlgOpt.bBlockCancel=1;
	}
	g_dlgOptions=&dlgOpt;
	objSettings.bStartWithWindows=IsStartupWithWindows();
	lLikKeysPasted=0;
	// Вызываем
	DWORD dwRes=dlgOpt.DoModal();
	if(pMainDialogWindow){
		pMainDialogWindow->RegisterMyHotKeys();
	}
	g_dlgOptions=0; 
}

#define NAGTIME	(30*1000)
DWORD dwLastMsgLeakNagTime=0;
BOOL CSettings::CheckLiks(const char* szWho, const char* szOne, const char* szOther)
{
	// Если  хэшировать со сжатием - не будет работать
	if(szOther && szOne && strlen(szOne)>=LKEY_LEN+LUSER_LEN && strlen(szOther)>=LKEY_LEN+LUSER_LEN && strcmp(szOne,szOther)==0){
		if(GetTickCount()-dwLastMsgLeakNagTime>NAGTIME){
			dwLastMsgLeakNagTime=GetTickCount();
			CString sStr="'";
			sStr+=szWho;
			sStr+="' ";
			sStr+=_l("uses this program\nwith the same registration key as you");
			sStr+="\n";
			sStr+=_l("In this case license agreement is not valid any more!\nPlease contact us at www.WiredPlane.com");
			Alert(sStr,_l("Security leak found"));
			return TRUE;
		}
	}
	return FALSE;
}

CString GetNSkinBmp(int iSkin, const char* szBmp, int iNum)
{
	return Format("%s_s%lu_%lu",szBmp,iSkin,iNum);
}

BOOL CALLBACK RegNow(void* Param, int iButtonNum, HWND hThis)
{
	if(iButtonNum!=IDCANCEL){
		BuyWC(NULL,NULL);
	}
	return TRUE;
}

HWND hPrevUnreg=0;
void UnregAlert(const char* szText)
{
	if(hPrevUnreg && IsWindow(hPrevUnreg)){
		// Уже висит одно.... не наглеем
		return;
	}
	if(objSettings.iLikStatus<2){
		hPrevUnreg=0;
		InfoWndStruct* info=new InfoWndStruct();
		info->pParent=NULL;
		info->sText=szText;
		info->sTitle=_l(UNREG_FUNC_TITLE);
		info->bCenterUnderMouse=TRUE;
		info->dwStaticPars=DT_CENTER|DT_VCENTER;
		info->sCancelButtonText=_l("Cancel");
		info->sOkButtonText=_l("Order now")+"!";
		info->dwBlockCancel=4000;
		InfoWnd* RegAlert=Alert(info);
		if(RegAlert){
			hPrevUnreg=RegAlert->GetSafeHwnd();
			RegAlert->SetAutoCloseTime(info->dwBlockCancel);
			RegAlert->SetCallback(RegNow,0);
		}
	}
}

void PerformUnregActions()
{
	if(objSettings.sLikKey.GetLength()>20 && objSettings.sLikKey.Find("WPK")==-1){
		Alert(_l("Warning: You should upgrade your registration key in order to use "+AppName()+" 3!\n\nPlease, write an email to support@wiredplane.com and ask for new key.\nThis is absoluetly FREE for registered users.\nWe are forced to reissue keys because of hackers activity")+"\n"+_l("Sorry for inconvinience"), _l(UNREG_FUNC_TITLE));
	}
	if(objSettings.TemplatesInWork.GetSize()<=2 && objSettings.aDesktopNotes.GetSize()<=1){
		// Пожалеем юзера....
		return;
	}
	if(isTempKeyInvalid()){
		UnregAlert(_l(
			"Your temporarily registration key is no longer valid. You can contribute another\n"
			" picture using our Wallpaper Exchange service to get registration for another week, or\n"
			" continue to use unregistered version with serious limitations. Or you can buy full\n"
			" copy of this piece of software to get even more fun with "+AppName())+"!");
	}else {
		if(isTempKey() && isTempKeyDate().GetStatus()!=COleDateTime::invalid){
			CString sTemp=_l("All trial limitations are removed because this\nprogram is registered with free time-limited key");
			sTemp+=".\r\n";
			sTemp+=_l("But if you don`t want to see this message again,\nplease buy full license at");
			sTemp+=" http://www.wiredplane.com/\r\n";
			sTemp+=_l("Expiration date")+": "+isTempKeyDate().Format("%d %B %Y");
			pMainDialogWindow->ShowBaloon(sTemp,_l(UNREG_FUNC_TITLE));
		}else if(objSettings.iLikStatus<2){
			CString sTemp=_l("You are using demo version of "+AppName());
			sTemp+=".\r\n";
			sTemp+=_l("But if you don`t want to see this message again,\nplease buy full license at");
			sTemp+=" http://www.wiredplane.com/\r\n";
			pMainDialogWindow->ShowBaloon(sTemp,_l(UNREG_FUNC_TITLE));
		}
	}
}

void BackupAllSets()
{
	CString sFile;
	if(!OpenFileDialog("Settings archive (*.wcb)|*.wcb||",&sFile,0,FALSE,TRUE,pMainDialogWindow?(CWnd*)pMainDialogWindow:0)){
		return;
	}
	CString sZipFile=AddPathExtension(sFile,".wcb");
	if(isFileExist(sZipFile)){
		if(AfxMessageBox(Format("%s ('%s')?",_l("Overwrite existing backup"),TrimMessage(sZipFile,40,3)),MB_YESNO|MB_ICONQUESTION)!=IDYES){
			return;
		}
	}
	objSettings.Save();
	CCreateZipArchive zpBackup;
	if(zpBackup.Create(sZipFile,"",0)!=ZIPRES_OK){
		AfxMessageBox(_l("Can`t create backup file"));
		return;
	}
	int iCount=1;
	CFileInfoArray dir;
	CString sIniPath=GetPathPart(objSettings.sIniFile,1,1,0,0);
	dir.AddDir(sIniPath,"*.*",TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	for (int i=0;i<dir.GetSize();i++){
		CString sFile=dir[i].GetFilePath();
		if(sFile.Find(".bmp")==-1){
			CString sFileOnArch=sFile.Mid(strlen(sIniPath));
			zpBackup.Add(sFile,sFileOnArch);
		}
	}
	zpBackup.Close();
	AfxMessageBox(_l("Backup created successfully")+"!");
}

void RestoreAllSets()
{
	CString sFile;
	if(!OpenFileDialog("Settings archive (*.wcb)|*.wcb||",&sFile,0,FALSE,TRUE,0)){
		return;
	}
	sFile=AddPathExtension(sFile,".wcb");
	if(!isFileExist(sFile)){
		return;
	}
	objSettings.lNoExitWarning=1;
	CString sExe=GetApplicationDir();
	sExe+=GetApplicationName();
	sExe+=".exe";
	int iRes=(int)::ShellExecute(0,"open",sExe,Format("-wait=yes -restore=\"%s\"",sFile),NULL,SW_SHOWNORMAL);
	pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
}

CWPT* GetWPT(const char* szWPT)
{
	if(!szWPT || szWPT[0]==0){
		return 0;
	}
	CWPT* pOut=0;
	objSettings.aLoadedWPTsData.Lookup(szWPT,pOut);
	return pOut;
}

CWPT* GetWPT(int i)
{
	if(i<0 || i>=objSettings.aLoadedWPTs.GetSize()){
		return 0;
	}
	return GetWPT(objSettings.aLoadedWPTs[i]);
}

CString GetWPTTitle(const char* szWPT)
{
	CWPT* pOut=GetWPT(szWPT);
	if(!pOut){
		return "Unknown";
	}
	return _l(pOut->sTitle);
}

BOOL GetWPTClonable(int i)
{
	CWPT* wpt=GetWPT(objSettings.aLoadedWPTs[i]);
	if(wpt){
		return 0;
	}
	return wpt->bClonable;
}

CString GetWPTTitle(int i)
{
	return GetWPTTitle(objSettings.aLoadedWPTs[i]);
}

int GetWPTIndex(const char* szWPT)
{
	for(int i=0;i<objSettings.aLoadedWPTs.GetSize();i++){
		if(objSettings.aLoadedWPTs[i]==szWPT){
			return i;
		}
	}
	return -1;
}

BOOL DeleteFilesByMask(CString sDir, CString sMask, BOOL bDelRoot)
{
	CFileInfoArray dir;
	dir.AddDir(sDir,sMask,FALSE,0,FALSE);
	for (int i=0;i<dir.GetSize();i++){
		CString sFile=dir[i].GetFilePath();
		DeleteFile(sFile);
	}
	if(bDelRoot){
		RemoveDirectory(sDir);
	}
	return dir.GetSize();
}

BOOL isFolderEmpty(CString sDir, CString sMask)
{
	CFileInfoArray dir;
	dir.AddDir(sDir,sMask,FALSE,0,FALSE);
	return dir.GetSize();
}

int& GetWindowDays()
{
	static int iDays=-999;
	return iDays;
}

int& GetWindowDaysFromInstall()
{
	static int iDays=-1;
	return iDays;
}

int& isTempKeyInvalid()
{
	static int isTempKey=0;
	return isTempKey;
}

int& isTempKey()
{
	static int isTempKey=0;
	return isTempKey;
}

COleDateTime& isTempKeyDate()
{
	static COleDateTime isTempKey(2004,1,1,0,0,0);
	return isTempKey;
}

BOOL isKeyValid(const char* szKey)
{
	if(szKey[0]=='W' && szKey[1]=='P' && szKey[2]=='K'){
		return 1;
	}
	if(szKey[0]=='W' && szKey[1]=='P' && szKey[2]=='T'){
		return 1;
	}
	return strlen(szKey)>10;
}

BOOL isUserNameValid(CString& sUser, CString& sError)
{
	if(strlen(sUser)>66){
		sError="Maximum 66 characters";
		return FALSE;
	}
	if(strlen(sUser)<3){
		sError="Minimum 3 characters";
		return FALSE;
	}
	char szUserBuf[128]="";
	memset(szUserBuf,0,sizeof(szUserBuf));
	strcpy(szUserBuf,sUser);
	strupr(szUserBuf);
	char* szPos=szUserBuf;
	while(*szPos){
		if(BYTE(*szPos)<BYTE(' ')){
			sError="Invalid character";
			return FALSE;
		}
		/*
		if(!(BYTE(*szPos)>=BYTE('A') && BYTE(*szPos)<=BYTE('Z'))){
		*szPos='A'+(*szPos)%('Z'-'A');
		}
		*/
		szPos++;
	}
	sUser=szUserBuf;
	return TRUE;
}