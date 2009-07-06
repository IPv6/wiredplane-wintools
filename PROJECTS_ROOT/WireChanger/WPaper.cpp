// WPaper.cpp: implementation of the WPaper class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
WChanger::WChanger()
{
	bUsedListsInvalidated=FALSE;
	lLastValidPosition=0;
}

WChanger::~WChanger()
{
	ClearWPList();
}

BOOL WChanger::ClearWPList()
{
	bUsedListsInvalidated=TRUE;
	for(int i=0;i<aWPapersAndThemes.GetSize();i++){
		if(aWPapersAndThemes[i]){
			delete aWPapersAndThemes[i];
		}
	}
	aWPapersAndThemes.SetSize(0,100);
	return TRUE;
}

void WChanger::Save(const char* szFile)
{
	static long lSaveLock=0;
	if(lSaveLock>0){
		return;
	}
	if(pMainDialogWindow){
		pMainDialogWindow->StartAnimation(-1);
		pMainDialogWindow->m_STray.SetTooltipText(_l("Saving themes")+"...");
	}
	SimpleTracker Track(lSaveLock);
	//
	CString sTarget=szFile;
	if(sTarget==""){
		if(objSettings.sWChangerFile==""){
			objSettings.sWChangerFile="wchanger.xml";
		}
		sTarget=getFileFullName(objSettings.sWChangerFile);
	}
	CDataXMLSaver xmlFile(sTarget,"w-changer",FALSE);
	int iCount=0;
	for(int i=0;i<aUsedThemes.GetSize();i++){
		int iThemeNum=aUsedThemes[i];
		CDataXMLSaver xmlPictures;
		xmlPictures.putValue("title",aWPapersAndThemes[iThemeNum]->sTitle);
		xmlPictures.putValue("use",aWPapersAndThemes[iThemeNum]->dwStatus);
		xmlPictures.putValue("f-sync",aWPapersAndThemes[iThemeNum]->dwFolderSync);
		xmlPictures.putValue("add-param",aWPapersAndThemes[iThemeNum]->GetWPar(0));
		xmlPictures.putValue("resampling-style",aWPapersAndThemes[iThemeNum]->dwSampling);
		xmlPictures.putValue("weight",aWPapersAndThemes[iThemeNum]->dwTheme);
		CString sFiles="\n";
		int iFD=aUsedThemesFD[i];
		int iTD=aUsedThemesTD_Total[i];
		for(int j=iFD;j<=iTD;j++){
			int iWPIndex=aImagesInRotation[j];
			if(!aWPapersAndThemes[iWPIndex]){
				continue;
			}
			CString sSeparator,sAddData;
			if(aWPapersAndThemes[iWPIndex]->dwStatus){
				sAddData+="[B]";
			}
			if(aWPapersAndThemes[iWPIndex]->GetWPar("PRESET")!=""){
				sAddData+=Format("[PRESET:%s]",aWPapersAndThemes[iWPIndex]->GetWPar("PRESET"));
			}
			if(sAddData!=""){
				sFiles+=sAddData+WP_SEPAR_F;
			}
			sFiles+=aWPapersAndThemes[iWPIndex]->sTitle;
			sFiles+="\n";
		}
		xmlPictures.putValue("files",sFiles);
		xmlFile.putValue(Format("theme%i",iCount++),xmlPictures.GetResult(),XMLNoConversion);
	}
	if(pMainDialogWindow){
		pMainDialogWindow->StopAnimation(0);
	}
}

BOOL WChanger::IsSuchWPExist(const char* sFile, DWORD dwTheme)
{
	BOOL bDuplication=FALSE;
	for (int j=0;j<objSettings.m_WChanger.aWPapersAndThemes.GetSize();j++){
		WPaper* pWP=objSettings.m_WChanger.aWPapersAndThemes[j];
		if(!pWP || pWP->dwType!=1 || (dwTheme!=DWORD(-1) && int(pWP->dwTheme)!=dwTheme)){
			continue;
		}
		if(stricmp(sFile,pWP->sTitle)==0){
			bDuplication=TRUE;
			break;
		}
	}
	return bDuplication;
}

BOOL WChanger::AddFile(CString& sWPTitle, DWORD iTheme)
{
	if(strlen(sWPTitle)>0){
		WPaper* wp=new WPaper();
		sWPTitle.TrimLeft();
		sWPTitle.TrimRight();
		int iSeparation=-1;
		if((iSeparation=sWPTitle.Find(WP_SEPAR_F))!=-1){
			// Что-то еще!!!
			CString sAddData=sWPTitle.Left(iSeparation);
			if(sAddData.Find("[B]")!=-1){
				wp->SetStatus(1);
			}
			CString sPreset=CDataXMLSaver::GetInstringPart("[PRESET:","]",sAddData);
			if(sPreset!=""){
				wp->SetWPar("PRESET",sPreset);
			}
			sWPTitle=sWPTitle.Mid(iSeparation+strlen(WP_SEPAR_F));
		}
		wp->sTitle=sWPTitle;
		wp->dwType=1;
		wp->dwTheme=iTheme;
		objSettings.m_WChanger.aWPapersAndThemes.Add(wp);
		return TRUE;
	}
	return FALSE;
}

BOOL IsWallpaperExist(const char* szFile);
void WChanger::Load(const char* szFile)
{
	if(pMainDialogWindow){
		pMainDialogWindow->StartAnimation(-1);
		pMainDialogWindow->m_STray.SetTooltipText(_l("Loading and synchronizing themes")+"...");
	}
	ClearWPList();
	CString sTarget=szFile;
	if(sTarget==""){
		if(objSettings.sWChangerFile==""){
			objSettings.sWChangerFile="wchanger.xml";
		}
		sTarget=getFileFullName(objSettings.sWChangerFile);//GetDesktopPrefix()+
	}
	CDataXMLSaver xmlFile(sTarget,"w-changer",TRUE);
	int iCount=0,iSynced=0;
	CString sTheme,sTmpAddPars;
	while(xmlFile.getValue(Format("theme%i",iCount++),sTheme)){
		CDataXMLSaver xmlTheme(sTheme);
		WPaper* wp=new WPaper();
		xmlTheme.getValue("title",wp->sTitle);
		xmlTheme.getValue("use",wp->dwStatus);
		xmlTheme.getValue("f-sync",wp->dwFolderSync);
		xmlTheme.getValue("add-param",sTmpAddPars);
		xmlTheme.getValue("resampling-style",wp->dwSampling);
		xmlTheme.getValue("weight",wp->dwTheme);
		wp->SetWPar(0,sTmpAddPars);
		wp->dwType=0;
		BOOL bSyncThisTheme=(wp->dwFolderSync>0);
		if(bSyncThisTheme){
			if(objSettings.lSyncCount==3){
				// Количество секунд?
				FILETIME lpCheck;
				CString sFolder=wp->GetWPar("SYNC");
				sFolder.TrimRight("\\/");
				if(GetFileDateTime(sFolder,&lpCheck,TRUE,TRUE)){
					//SYSTEMTIME lpCheckSys;
					//FileTimeToSystemTime(&lpCheck,&lpCheckSys);
					if(lpCheck.dwLowDateTime!=0 || lpCheck.dwHighDateTime!=0){
						// Считаем секунды
						COleDateTime tm(lpCheck);
						COleDateTime tmS(2000,01,01,00,00,00);
						if(tm.GetStatus()==COleDateTime::valid){
#ifdef _DEBUG
							CString s1=tm.Format("%x %X");
							CString s21=tmS.Format("%x %X");
#endif
							COleDateTimeSpan ps;
							ps=tm-tmS;
							long lSecs=long(ps.GetTotalSeconds());
							long lFolderLastCheck=atol(wp->GetWPar("LAST-CHECK"));
							if(lSecs>lFolderLastCheck){
								bSyncThisTheme=1;
							}
							wp->SetWPar("LAST-CHECK",Format("%i",lSecs));
						}
					}
				}
			}else{
				bSyncThisTheme=(objSettings.lSyncCount==0 || (objSettings.lSyncCount!=0 && objSettings.lStartsCount && (objSettings.lStartsCount%(objSettings.lSyncCount*3))==0));
			}
		}
		int iTheme=objSettings.m_WChanger.aWPapersAndThemes.Add(wp);
		// Определяем файлы темы...
		CString sFiles;
		xmlTheme.getValue("files",sFiles);
		CString sFirstImagePath;
		while(sFiles.GetLength()>0){
			CString sWPTitle=sFiles.SpanExcluding("\n");
			if(strlen(sWPTitle)!=strlen(sFiles)){
				sFiles=sFiles.Mid(strlen(sWPTitle)+1);
			}else{
				sFiles="";
			}
			if(sWPTitle!=""){
				if(!bSyncThisTheme || (!objSettings.lRemBrokenOnSync || IsWallpaperExist(sWPTitle))){
					if(AddFile(sWPTitle,iTheme)){
						if(sFirstImagePath.IsEmpty() && !IsFileHTML(sWPTitle)){
							sFirstImagePath=sWPTitle;
						}
					}
				}
			}
		}
		if(wp->GetWPar("SYNC")!=""){
			sFirstImagePath=wp->GetWPar("SYNC");
		}
		if(sFirstImagePath!="" && bSyncThisTheme){
			CFileInfoArray dir;
			char szD[MAX_PATH]="",szP[MAX_PATH]="";
			_splitpath(sFirstImagePath,szD,szP,NULL,NULL);
			sFirstImagePath=szD;
			sFirstImagePath+=szP;
			dir.AddDir(sFirstImagePath,VALID_EXTEN,objSettings.bLoadDirsRecursively,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
			for (int i=0;i<dir.GetSize();i++){
				CString sFile=dir[i].GetFilePath();
				if(sFile!=""){
					if(IsSuchWPExist(sFile,iTheme)){
						continue;
					}
					iSynced++;
					AddFile(sFile,iTheme);
				}
			}
		}
	}
	ReinitWPaperShuffle("",TRUE);
	if(pMainDialogWindow){
		pMainDialogWindow->StopAnimation(0);
	}
	if(iSynced>0){
		pMainDialogWindow->ShowBaloon(Format("%s: %i",_l("New images added"),iSynced),_l("Theme Synchronization"));
		// Сохраняем!
		objSettings.m_WChanger.Save();
	}
}

long WChanger::GetLastActivePictureNum()
{
	return lLastValidPosition;
}

void WChanger::AddThemeToPicturesByStatus(int iStatus, int iTheme, int& iThemeImagesCount, const char* szStartPict)
{
	for(int j=0;j<objSettings.m_WChanger.aWPapersAndThemes.GetSize();j++){
		WPaper* wpF=objSettings.m_WChanger.aWPapersAndThemes[j];
		if(!wpF || wpF->dwType==0 || (int)wpF->dwTheme!=iTheme){
			continue;
		}
		if(iStatus==-1 || int(wpF->dwStatus)==iStatus){
			iThemeImagesCount++;
			int iIndex=aImagesInRotation.Add(j);
			if(szStartPict && wpF->sTitle==szStartPict){
				objSettings.lCurPicture=iIndex;
			}
		}
	}
}

void WChanger::AddThemeToPictures(int iThemeStatus, const char* szStartPict)
{
	for(int i=0;i<objSettings.m_WChanger.aWPapersAndThemes.GetSize();i++){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[i];
		if(!wp){
			continue;
		}
		if(wp->dwType==0 && int(wp->dwStatus)==iThemeStatus){
			int iThemeNum=objSettings.m_WChanger.aUsedThemes.Add(i);
			if(iThemeStatus==1 && wp->dwTheme==0){
				// Невесомая тема...
				aUsedThemes_Passive.Add(iThemeNum);
			}
			DWORD dwFD=aImagesInRotation.GetSize();
			aUsedThemesFD.Add(dwFD);
			int iThemeImagesCount=0;
			int iThemeActiveImagesCount=0;
			//Заносим подряд все обои относящиеся к найденной теме
			AddThemeToPicturesByStatus((iThemeStatus!=0?0:-1),i,iThemeImagesCount,szStartPict);
			if(iThemeStatus==1){
				// Считаем только для активных тем...
				iThemeActiveImagesCount+=iThemeImagesCount;
			}
			DWORD dwTD=aImagesInRotation.GetSize()-1;
			aUsedThemesTD.Add(dwTD);
			aUsedThemesFDOff.Add(dwTD-dwFD);
			if(iThemeStatus!=0){
				// Занесем неиспользуемые обои в промежуточную зону
				AddThemeToPicturesByStatus(1,i,iThemeImagesCount,NULL);
			}
			DWORD dwTD_Total=aImagesInRotation.GetSize()-1;
			aUsedThemesTD_Total.Add(dwTD_Total);
			wp->dwImagesCount=iThemeImagesCount;
			wp->dwActiveImagesCount=iThemeActiveImagesCount;
		}
	}
}

void WChanger::ReinitWPaperShuffle(CString szStartPic,BOOL bInLowP)
{
//	EXECryptor_ProtectImport(); 
	if(pMainDialogWindow){
		pMainDialogWindow->StartAnimation(-1);
		pMainDialogWindow->m_STray.SetTooltipText(_l("Refreshing list of images")+"...");
	}
	if(bInLowP){
		SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_IDLE);
	}
	CSmartLock lslock(cShufflingLock,TRUE);
	aUsedThemes_Passive.RemoveAll();
	aUsedThemes.RemoveAll();
	aUsedThemesFD.RemoveAll();
	aUsedThemesTD.RemoveAll();
	aUsedThemesTD_Total.RemoveAll();
	aUsedThemesFDOff.RemoveAll();
	aImagesInRotation.RemoveAll();
	lLastValidPosition=0;
	AddThemeToPictures(1,szStartPic);// Добавили активные темы
	lLastValidPosition=aImagesInRotation.GetSize();// Теперь эта штука указывает на последний элемент активной темы +1.
	AddThemeToPictures(0,0);// Добавили неактивные темы...
	if(bInLowP){
		SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}
	bUsedListsInvalidated=FALSE;
	if(pMainDialogWindow){
		pMainDialogWindow->StopAnimation(0);
	}
}

long WChanger::GetNextWPNumber(int iSelectionType)
{
	if(bUsedListsInvalidated){
		return -2;
	}
	CSmartLock lslock(objSettings.m_WChanger.cShufflingLock,TRUE);
	long lRes=objSettings.lCurPicture;
	if(GetLastActivePictureNum()==0 || aImagesInRotation.GetSize()==0){
		return -1;
	}
	if(lRes<0){
		lRes=0;
	}
	if(lRes>=aImagesInRotation.GetSize()){
		lRes=aImagesInRotation.GetSize();
	}
	int iChoosedThemeNum=-1;
	if(iSelectionType==1){
		long dwNewNum=aImagesInRotation[lRes];
		long lMinNewNum=-1;
		BOOL bOk=0;
		for(int i=0;i<GetLastActivePictureNum();i++){
			long lID=aImagesInRotation[i];
			if(lID>dwNewNum && (lMinNewNum==-1 || lID<lMinNewNum)){
				lMinNewNum=lID;
				lRes=i;
				bOk=1;
			}
		}
		if(bOk==0){
			lRes=0;
		}
	}else if(iSelectionType==-1){
		long dwNewNum=aImagesInRotation[lRes];
		long lMinNewNum=-1;
		BOOL bOk=0;
		for(int i=GetLastActivePictureNum()-1;i>=0;i--){
			long lID=aImagesInRotation[i];
			if(lID<dwNewNum && (lMinNewNum==-1 || lID>lMinNewNum)){
				lMinNewNum=lID;
				lRes=i;
				bOk=1;
			}
		}
		if(bOk==0){
			lRes=GetLastActivePictureNum()-1;
		}
	}else if(iSelectionType==0){
		// Это по старому...
		lRes=rnd(0,GetLastActivePictureNum()-1);
		// А теперь по новому - С учетом веса обоев!!
		// Темы с нулевым весом, которые проверяются в случае когда никаких других нет
		CDWordArray aPassiveThemes;
		int iRandom=rnd(0,99);
		int iStartSeed=0;
		CDWordArray aRealActiveThemes;
		for(int i=0;i<aUsedThemes.GetSize();i++){
			WPaper* wp=aWPapersAndThemes[aUsedThemes[i]];
			if(!wp){
				break;
			}
			if(wp->dwStatus==0){
				continue;
			}
			aRealActiveThemes.Add(i);
			DWORD dwWeight=wp->dwTheme;
			if(dwWeight>0){
				iStartSeed+=dwWeight;
				if(iRandom<=iStartSeed){
					//используем эту тему!
					iChoosedThemeNum=i;
					break;
				}
			}
		}
		if(iChoosedThemeNum==-1 && aUsedThemes_Passive.GetSize()>0){
			// недобор! Выбираем произвольную тему из пассивных...
			DWORD iMaxRandom=0;
			int iMaxActiveNum=-1;
			// ищем только среди активных пассивных тем
			for(int i=0;i<aUsedThemes_Passive.GetSize();i++){
				DWORD dwTheme=aUsedThemes[aUsedThemes_Passive[i]];
				if(!aWPapersAndThemes[dwTheme]){
					continue;
				}
				WPaper* wpt=aWPapersAndThemes[dwTheme];
				if(wpt->dwStatus){
					iMaxRandom+=wpt->dwActiveImagesCount;
					iMaxActiveNum=i;
				}else{
					break;
				}
			}
			if(iMaxActiveNum!=-1){
				// Отсчитываем обратно чтобы определить тему
				// пропорционально количеству картинок в ней
				int iRnd=rnd(0,iMaxActiveNum);
				if(objSettings.iTreatUnwtRight){
					DWORD iRandomBalanced=rnd(0,iMaxRandom);
					for(int i=0;i<=iMaxActiveNum;i++){
						DWORD dwTheme=aUsedThemes[aUsedThemes_Passive[i]];
						if(!aWPapersAndThemes[dwTheme]){
							continue;
						}
						iMaxRandom-=aWPapersAndThemes[dwTheme]->dwActiveImagesCount;
						if(iRandomBalanced>iMaxRandom){
							iRnd=i;
							break;
						}
					}
				}
				iChoosedThemeNum=aUsedThemes_Passive[iRnd];
			}
		}
		if(iChoosedThemeNum==-1 && aRealActiveThemes.GetSize()>0){
			// Опять недобор! Выбираем _любую_из_активных_
			iChoosedThemeNum=aRealActiveThemes[rnd(0,aRealActiveThemes.GetSize()-1)];
		}
	}
	if(iChoosedThemeNum!=-1){
		DWORD dwFD=aUsedThemesFD[iChoosedThemeNum];
		DWORD dwTD=aUsedThemesTD[iChoosedThemeNum];
		int dwOffset=0;
		if(dwFD==dwTD){
			lRes=dwFD;
		}else{
			dwOffset=aUsedThemesFDOff[iChoosedThemeNum];
			// Выбираем с чем менять
			DWORD dwLeftLim=dwFD+(dwOffset==int(dwTD-dwFD)?1:0);
			DWORD dwRightLim=dwFD+dwOffset;
			if(dwLeftLim>dwRightLim){
				DWORD dwTmp=dwRightLim;
				dwRightLim=dwLeftLim;
				dwLeftLim=dwTmp;
			}
			lRes=rnd(dwLeftLim,dwRightLim);
		}
		// Меняем с номером lRes и dwFD+dwOffset
		if(lRes>=GetLastActivePictureNum() || long(dwFD+dwOffset)>=GetLastActivePictureNum()){
			return -1;
		}
		if(lRes<0 || lRes>=aImagesInRotation.GetSize() || long(dwFD+dwOffset)<0 || long(dwFD+dwOffset)>=aImagesInRotation.GetSize()){
			return -1;
		}
		if(dwFD!=dwTD){
			DWORD dwNewNum=aImagesInRotation[lRes];
			DWORD dwNewNum2=aImagesInRotation[dwFD+dwOffset];
			aImagesInRotation[dwFD+dwOffset]=dwNewNum;
			aImagesInRotation[lRes]=dwNewNum2;
			lRes=dwFD+dwOffset;// Теперь lres указывает на картинку, которая не скоро попадет в ротацию...
			// Уменьшаем на 1 смещение ..
			dwOffset--;
			if(dwOffset<0){
				dwOffset=dwTD-dwFD;
			}
			aUsedThemesFDOff[iChoosedThemeNum]=dwOffset;
		}
#ifdef _DEBUG
		DWORD dwRealNum=objSettings.m_WChanger.aImagesInRotation[lRes];
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[dwRealNum];
		if(wp){
			if(wp->dwStatus==1){
				DebugBreak();
			}
		}
#endif
	}
	if(lRes>=GetLastActivePictureNum()){
		lRes=0;
	}
	if(lRes<0){
		lRes=GetLastActivePictureNum()-1;
	}
	return lRes;
}

// Индекс из aWPapersAndThemes!!!
void WChanger::GetWPTitleAndSampling(int iIndex, CString& sPath, CString& sThemeName, DWORD& dwSampling)
{
	sPath="";
	sThemeName="Common";
	dwSampling=0;
	if(iIndex>=0 && (int)iIndex<objSettings.m_WChanger.aWPapersAndThemes.GetSize()){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iIndex];
		int iThemeIndex=iIndex;
		WPaper* wpTh=GetWPTheme(iThemeIndex);
		if(wp){
			dwSampling=wp->dwSampling;
			if(dwSampling==-1){
				dwSampling=objSettings.m_WChanger.aWPapersAndThemes[wp->dwTheme]->dwSampling;
			}
			sPath=wp->sTitle;
			if(wpTh){
				sThemeName=wpTh->sTitle;
			}
			return;
		}
	}
}

BOOL WChanger::SetWallPaperByIndex(int dwIndex)
{
	CString sPath="",sThemeName="";
	DWORD dwSampling=0;
	objSettings.iLikStatus--;
	objSettings.iLikStatus--;
	objSettings.iLikStatus--;
	objSettings.iLikStatus--;
	CRYPT_START
	if(objSettings.iLikStatus>0){
		objSettings.iLikStatus=0;
		return FALSE;
	}
	objSettings.iLikStatus+=4;
	CString sPreset=objSettings.sActivePreset;
	GetWPTitleAndSampling(dwIndex, sPath, sThemeName, dwSampling);
	if(sPath!=""){
		objSettings.sLastWallpaperFromUser=sPath;
		objSettings.iLastStretchingFromUser=dwSampling;
		return SetWallPaper(sPath, dwSampling, sThemeName,sPreset);
	}
	CRYPT_END
	return FALSE;
}

WPaper* GetWPTheme(int& iIndex)
{
	if(iIndex<0){
		return NULL;
	}
	WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iIndex];
	if(!wp){
		return NULL;
	}
	WPaper* wpTheme=wp;
	if(wpTheme->dwType!=0){
		iIndex=wp->dwTheme;
		wpTheme=objSettings.m_WChanger.aWPapersAndThemes[iIndex];
		if(!wpTheme){
			return NULL;
		}
	}
	return wpTheme;
}

WPaper::WPaper()
{
	dwSampling=-1;sTitle="";hRow=NULL;dwActiveImagesCount=dwImagesCount=dwFolderSync=dwType=dwTheme=0;dwStatus=0;
};

DWORD WPaper::SetStatus(DWORD lNewVal)
{
	if(dwStatus!=lNewVal){
		if(dwType==0 && pMainDialogWindow && pMainDialogWindow->dlgWChanger){
			if(dwStatus==0 && lNewVal!=0){
				pMainDialogWindow->dlgWChanger->lThemesWTotal+=dwTheme;
			}else{
				pMainDialogWindow->dlgWChanger->lThemesWTotal-=dwTheme;
			}
		}
		dwStatus=lNewVal;
	}
	return dwStatus;
}

WPaper::~WPaper()
{
};
