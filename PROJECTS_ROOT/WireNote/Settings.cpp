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

BOOL& IsEmbedded();
CString sResTsc("* ");
CString sLangsNames="English\tRussian";
extern BOOL bAdvertLoaded;
BOOL bOnlyNotesOptions=0;
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
		sLangsNames="English\tRussian";
	}else if(iSize>1){
		for(int i=0;i<iSize;i++){
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
int CSettings::StatusIcons[4]={IDI_ADDRBOOK,IDI_ADDRBOOK_AWAY,IDI_ADDRBOOK_BUSY,IDI_ADDRBOOK_DND};
CSettings::CSettings()
{
	sAddrFile=DEFAULT_ABOOKFILE;// Для поддержки параметров комстроки
	iDefNoteW=0;
	iDefNoteH=0;
	lShowUnreadInAddrb=1;
	bAutoQuote=0;
	lFBNotifyOfRead=1;
	iSMSCount=0;
	bProtocolRS_Active=1;
	bExclChanged=0;
	bAutoCreateTodoFromMW=0;
	bUseNickOnSend=1;
	bGroupBySender=0;
	sLastIndexOfOpenedNote="";
	lFBDirNorm=0;
	dwLastSendError=0;
	bNoSend=0;
	bSaveAllowed=0;
	CString sOS;
	bUnderWindows98=0;
	DWORD dwMajor=0;
	long bUnderWindowsNT=0;
	GetOSVersion(sOS,&dwMajor,&bUnderWindowsNT);
	bUnderWindows98=!bUnderWindowsNT;
	//
	lTrayIconsMaskO=1;
	lTrayIconsMaskM=1;
	iSHowTitleInIconMode=1;
	lShowMsgInAddrbookIcon=1;
	iEnableAutoHideWithEsc=iEnableAutoHideWithAlt=0;
	bSWarning_ShareOK=0;
	iLikStatus=2;//!!! registered from the start
	sLikKey="";
	sLikUser="";
	Items.SetSize(0,100);
	iTrayIcon=0;
	lNOTEICON_DIM=24;
	bNetSubsystemInitialized=FALSE;
	hHotMenuWnd=NULL;
	hFocusWnd=NULL;
	ActiveWndRect.SetRect(0,0,0,0);
	dStartTime=COleDateTime::GetCurrentTime();
	lLastReminderCheckTime=GetTickCount();
	bRunStatus=0;
	dwButtonSize=20;
	dwNoteButtonSize=16;
	InitializeCriticalSection(&lock);
	InitializeCriticalSection(&bTreeNeedUpdateLock);
	ReminderDLGLastAction=0;
	// Имя компа
	char szCompName[MAX_COMPUTERNAME_LENGTH + 1]="";
	DWORD dwLen=sizeof(szCompName);
	DWORD dwRes=GetComputerName(szCompName,&dwLen);
	sLocalComputerName=szCompName;
	sLocalComputerName.MakeUpper();
	sDefAuthor=szCompName;
	szSystemFolder[0]=0;
	GetSpecialFolderPath(szSystemFolder,CSIDL_PERSONAL);
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rDesktopRectangle,0);
	bDoModalInProcess=0;
	iLastSchemeUsed=0;
	bTreeNeedUpdate=FALSE;
	bStartWithOptions=FALSE;
	iStatusUpdateTimeLastTCount=GetTickCount();
	bCheckPinnedNoteForRect=1;
	// Перенумеруем сетевые адаптеры...
	dwNB_Lana=0;
	iNB_LanaNum=0;
	CNCB::EnumerateAdapters(&asEnum);
	// Названия сокращений протоколов...
	aProtList.Add("ms");
	aProtList.Add("nb");
	aProtList.Add("xpc");
	aProtList.Add("fb");
	aProtList.Add("em");
	aProtList.Add("sms");
	aProtList.Add("rss");
	// Запускаем мессаджинг
	objMessageWrapper=new CMessageWrapper;
	bRunStatus=1;
}


DWORD WINAPI WhatsNew(LPVOID data)
{
	if(IsEmbedded()){
		return 0;
	}
	static long bWas=0;
	if(bWas){
		return 0;
	}
	bWas=1;
	if(AfxMessageBox(_l("Do you want to see 'What`s new' in this version of")+" "+PROGNAME+"?",MB_YESNO|MB_ICONQUESTION)==IDYES){
		ShowHelp("history");
	}
	return 0;
}

DWORD DefActionMap_Preinit[MAX_HOTACTIONS]={0,0,0,0,0,0,0,0};
DWORD DefActionMapHK_Preinit[MAX_HOTACTIONS]={0,196686,196692,196690,196685,393300,393294,393281,262220};
void CSettings::Load(const char* szFile, int iLevel)
{
	//Читаем настройки
	CString sIniFile=szFile;
	if(sIniFile==""){
		sIniFile=getFileFullName(CString(PROGNAME)+".conf");
	}
	CDataXMLSaver IniFile(sIniFile,"application-config",TRUE);
	if(!IniFile.IsFileLoaded()){
		bStartWithOptions=TRUE;
	}
	if(iLevel==0 && !IsEmbedded()){
		CRegKey key;
		if(key.Open(HKEY_CURRENT_USER, PROG_REGKEY)!=ERROR_SUCCESS){
			key.Create(HKEY_CURRENT_USER, PROG_REGKEY);
		}
		if(key.m_hKey!=NULL){
			CString sSource=IniFile.GetSourceFile();
			DWORD dwType=REG_SZ,dwSize=0;
			RegSetValueEx(key.m_hKey,"ConfigFile",0,REG_SZ,(BYTE*)(LPCSTR)sSource,lstrlen(sSource)+1);
			char szDisk[MAX_PATH]="C://", szPath[MAX_PATH]="";
			_splitpath(sSource, szDisk, szPath, NULL, NULL);
			CString sConfPath=szDisk;
			sConfPath+=szPath;
			RegSetValueEx(key.m_hKey,"UserData",0,REG_SZ,(BYTE*)(LPCSTR)sConfPath,lstrlen(sConfPath)+1);
			if(bStartWithOptions && !IsEmbedded()){
				sConfPath=COleDateTime::GetCurrentTime().Format("%d.%m.%Y");
				RegSetValueEx(key.m_hKey,"InstallDate",0,REG_SZ,(BYTE*)(LPCSTR)sConfPath,lstrlen(sConfPath)+1);
				sConfPath="";
				ReadFile(CString(GetApplicationDir())+"affinfo.txt",sConfPath);
				RegSetValueEx(key.m_hKey,"AffData",0,REG_SZ,(BYTE*)(LPCSTR)sConfPath,lstrlen(sConfPath)+1);
			}
		}
	}
	IniFile.getValue("uint-language",				GetApplicationLang(),0);
	if(IsEmbedded()){
		// Вычисляем...
		CString sEmbName;
		GetCommandLineParameter("langByName",sEmbName);
		//AfxMessageBox(sEmbName);
		for(int i=0;i<50;i++){
			int iDef=GetApplicationLang();
			GetApplicationLang()=i;
			CString sHere=_l(Format("LangName%i",i));
			GetApplicationLang()=iDef;
			if(sEmbName==sHere){
				GetApplicationLang()=i;
				break;
			}
			if(sHere.Find("LangName")!=-1){
				// Не нашли языка...
				break;
			}
		}
	}
	CString sVer;
	IniFile.getValue("cur-version",					sVer);
	if(sVer!=PROG_VERSION && !bStartWithOptions && !IsEmbedded()){
		FORK(WhatsNew,NULL);
	}
	// только на эту машину....
	if(iLevel==0 || iLevel==-1){
		IniFile.getValue("uint-globalSIDCounter",		dwGlobalCount);
		IniFile.getValue("uint-trayMenuMode",			sRootMenuId);
		IniFile.getValue("uint-soundOnErrorWav",		sWavOnError,DEFAULT_ONERRSOUND);
		IniFile.getValue("uint-trashDirectory",			sTrashDirectory,getFileFullName(DEFAULT_TRASHFOLDER,TRUE));
		IniFile.getValue("uint-lastFindString",			sLastFindString);
		IniFile.getValue("stat-sendSMS",				iSMSCount);
		IniFile.getValue("stat-receivedMails",			lStatReceivedMails);
		IniFile.getValue("stat-receivedSpamMails",		lStatReceivedSpamMails);
		IniFile.getValue("stat-receivedMsg",			lStatReceivedMessages);
		IniFile.getValue("stat-sendedMsg",				lStatSendedMessages);
		IniFile.getValue("stat-notesCreated",			lStatNotesCreated);
		IniFile.getValue("msge-NBAlias",				sProtocolNB_Alias,sLocalComputerName);
		IniFile.getValue("msge-FBNick",					sFBNick,sLocalComputerName);
		if(sFBNick!=""){
			sDefAuthor=sFBNick+" ("+sLocalComputerName+")";
		}
		IniFile.getValue("msge-FBID",					sFB_ID,GenerateNewId("",sLocalComputerName));
		IniFile.getValue("msge-EM-ADS",					sEM_Ads);
		IniFile.getValue("msge-EM-P3-HOST",				sEM_p3host);
		IniFile.getValue("msge-EM-P3-PORT",				lEM_p3port,110);
		IniFile.getValue("msge-EM-P3-LOGN",				sEM_p3login);
		IniFile.getValue("msge-EM-P3-PASW",				sEM_p3psw);
		IniFile.getValue("msge-EM-SM-HOST",				sEM_smhost);
		IniFile.getValue("msge-EM-SM-PORT",				sEM_smport,"25");
		IniFile.getValue("msge-EM-SM-LOGN",				sEM_smlogin);
		IniFile.getValue("msge-EM-SM-PASW",				sEM_smpsw);
		IniFile.getValue("msge-EM-EMAIL",				sEMmail);

		IniFile.getValue("msge-soundOnSendWav",			sWavOnMsgSend,		DEFAULT_SENDWAV);
		IniFile.getValue("msge-soundOnDeliveryWav",		sWavOnMsgDeliv,		DEFAULT_DELIVWAV);
		IniFile.getValue("msge-soundOnFailedDeliveryWav",sWavOnMsgNotDeliv,	DEFAULT_NDELIVWAV);
		IniFile.getValue("msge-soundFile",				sMessageWavFile,	DEFAULT_RECIVWAV);

		IniFile.getValue("msge-actionOnIncomingMsg",	bOnNewMessage,DEFAULT_ONMESSAGE);
		IniFile.getValue("msge-currentOnlineStatus",	iOnlineStatus);
		if(iOnlineStatus<0 || iOnlineStatus>3){
			iOnlineStatus=0;
		}
		IniFile.getValue("msge-logFileDirectory",		sMessageLogDirectory,getFileFullName(DEFAULT_MSGLOG,TRUE));
		{
			int m=0;
			CString sRecentName;
			while(sRecentName!=INVALIDUSERNAME){
				IniFile.getValue(Format("msge-recentListVal%i",m++), sRecentName,INVALIDUSERNAME);
				if(sRecentName==INVALIDUSERNAME){
					break;
				}
				lMsgRecentList.Add(sRecentName);
			}
		}
		//
		{
			int m=0;
			CString sRecentQAnswer;
			while(sRecentQAnswer!=INVALIDUSERNAME){
				IniFile.getValue(Format("msge-recentQAnswer%i",m++), sRecentQAnswer,INVALIDUSERNAME);
				if(sRecentQAnswer==INVALIDUSERNAME){
					break;
				}
				lMsgQAnswers.Add(sRecentQAnswer);
			}
		}
		IniFile.getValue("link-SaveTo",					sLinksFile,"links.xml");
		IniFile.getValue("fold-SaveTo",					sFolderFile,"folders.xml");
		IniFile.getValue("todo-soundOnActivationWav",	sWavOnActivation,DEFAULT_TODOACT);
		IniFile.getValue("todo-soundOnDoneWav",			sWavSoundOnDone,DEFAULT_TODODONE);
		IniFile.getValue("todo-SaveTo",					sTaskDirectory,"Todo");
		{
			int m=0;
			CString sRecentToDoFolder;
			while(sRecentToDoFolder!=INVALIDUSERNAME){
				IniFile.getValue(Format("todo-recentFolder%i",m++), sRecentToDoFolder,INVALIDUSERNAME);
				if(sRecentToDoFolder==INVALIDUSERNAME){
					break;
				}
				lToDoFRecentList.Add(sRecentToDoFolder);
			}
		}
		IniFile.getValue("remd-soundFile",				sRemFileWav, REMINDER_SOUND);
		IniFile.getValue("remd-soundFileForNote",		sRemNoteFileWav, REMDNOTE_SOUND);
		IniFile.getValue("spam-accountCount",			lAntiSpamAccounts,1);
		IniFile.getValue("spam-addrFile",				sSpamAddressesFile,DEF_SPAMADDRFILE);
		IniFile.getValue("spam-subjFile",				sSpamSubjectsFile,DEF_SPAMSUBJFILE);
		IniFile.getValue("spam-bodyFile",				sSpamBodyFile,DEF_SPAMBODYFILE);
		IniFile.getValue("spam-headFile",				sSpamHeadFile,DEF_SPAMHEADFILE);
		IniFile.getValue("spam-removeWhireSpaces",		lRemoveSubjWhiteSpaces,1);
		IniFile.getValue("spam-ignoreCase",				lRemoveSubjCase,1);
		IniFile.getValue("spam-useMaskInSStr",			lAllowMasksInSpamStrings);
		IniFile.getValue("spam-spam5SqeSpaces",			lSubjSeqSpaces,1);
		IniFile.getValue("spam-showWhySpam",			lShowWhySpam,1);
		IniFile.getValue("spam-col0width",				iSpamCol0Width,50);
		IniFile.getValue("spam-col1width",				iSpamCol1Width,100);
		IniFile.getValue("spam-col2width",				iSpamCol2Width,100);
		IniFile.getValue("spam-askForKill",				lOnCloseSpamWindow);
		IniFile.getValue("spam-soundOnMail",			bSoundOnMail,1);
		IniFile.getValue("spam-soundOnMailWav",			sWavSoundOnMail,DEFAULT_ONMAIL);
		IniFile.getValue("spam-logFileMaxSize",			dwMaxMailLogFileSize,DEFAULT_MAXMLLOGKB);
		IniFile.getValue("spam-timeout",				dwMailboxTimeout,DEFAULT_MBTIMEOUT);
		IniFile.getValue("spam-reportInTray",			lShowSpamReportInTray,1);
		IniFile.getValue("spam-linesToLoad",			objSettings.lAdditionalHeaderLines,DEFSPAMMAILLINES);
		IniFile.getValue("spam-maxNotSpamSize",			objSettings.lMaxSizeToBeSpam,DEFSPAMMAILSIZE);
		IniFile.getValue("spam-spamFileExtensions",		sSPAMFileExtensions, DEF_SPAMFILEEXT);
		IniFile.getValue("spam-noUnkCharsets",			lBlockMailInUnknownCharset,1);
		IniFile.getValue("spam-noMessedChars",			lBlockMailWithMessedChars,1);
		IniFile.getValue("spam-noThisCharsets",			sBlockMailsWithThisCharsets);
		IniFile.getValue("spam-userDefinedEmCl",		sUserDefinedEmailB);

		if(objSettings.iLikStatus<2 && lAntiSpamAccounts>1){
			lAntiSpamAccounts=1;
		}
		lAntiSpamAccountsFromNextStart=lAntiSpamAccounts;
		antiSpamAccounts.SetSize(lAntiSpamAccounts);
		for(int si=0;si<lAntiSpamAccounts;si++){
			CString sData;
			IniFile.getValue(Format("spam-account%i",si),sData);
			antiSpamAccounts[si].lIndex=si;
			antiSpamAccounts[si].DeserializeFromString(sData);
		}
		PrepareSpamFiles();
		ReadFile(getFileFullName("BillboardItems")+".exc",sExcludeBillboardItems);
	}
	if(iLevel==1 || iLevel==-1){
		IniFile.getValue("uint-styleMain",				iMainStyle,0);
		IniFile.getValue("uint-stylePrefs",				lPreferencesLevel);
		IniFile.getValue("uint-showSystemErrors",		bShowSystemErrors,0);
		IniFile.getValue("uint-autoGetNoteTextInterval",bAutoGetNoteText,5000);
		IniFile.getValue("uint-renameInsteadOfDelete",	bDelToTrash,1);
		IniFile.getValue("uint-whattodoOnXButton",		lOnExitFromMainWnd);
		IniFile.getValue("uint-askToResendMsg",			bAskToResendMsg);
		IniFile.getValue("uint-maxTrashSize",			bMaxTrashSize,MIN_TRASH_SIZE);
		IniFile.getValue("uint-buttons",				bButtonsStyle);
		IniFile.getValue("uint-animOnNoteDel",			iAnimOnNoteDel,DEFANIM_NOTEDEL);
		IniFile.getValue("uint-animOnNewMsg",			iAnimOnNewMsg,DEFANIM_MSGNEW);
		IniFile.getValue("uint-lastSortedColumn",		iSortColumn,1);
		IniFile.getValue("uint-lastASortedColumn",		iSortAColumn,1);
		IniFile.getValue("uint-trayIcon",				iTrayIcon);
		IniFile.getValue("uint-soundOnError",			bErrorSound,1);
		IniFile.getValue("uint-autoSaveTime",			dwAutoSaveTime,20);
		IniFile.getValue("uint-messageCentreDefAction",	bMessCentreDefAction,0);	
		IniFile.getValue("uint-column0Width",			iCol0Width,30);
		IniFile.getValue("uint-column1Width",			iCol1Width,250);
		IniFile.getValue("uint-column2Width",			iCol2Width,120);
		IniFile.getValue("uint-feditH",					iFeditH);
		IniFile.getValue("uint-feditHR",				lFEditHR);

		IniFile.getValue("uint-generateRandomNames",	bGenerateRandomNames,1);
		IniFile.getValue("uint-itemsInTrayMenu",		iMenuItemsCount,10);
		IniFile.getValue("uint-stickWindowsThreshold",	iStickPix,10);
		IniFile.getValue("uint-wndshadows",				bShadows,1);
		IniFile.getValue("uint-autoPopupWindow",		APToDoList);
		IniFile.getValue("uint-doubleClickTray",		UseBblClick,0);
		IniFile.getValue("uint-showDateInTrayIcon",		lShowDateInTrayIcon,1);
		IniFile.getValue("uint-trayIconsMaskO",			lTrayIconsMaskO,1);
		IniFile.getValue("uint-trayIconsMaskM",			lTrayIconsMaskM);
		IniFile.getValue("uint-hideDateFld",			bHideDateFld);
		IniFile.getValue("uint-alertToTaskbar",			lAlertsOnTaskbar,1);
		IniFile.getValue("uint-checkForUpdates",		lCheckForUpdate,1);
		IniFile.getValue("uint-checkForUpdDays",		lDaysBetweenChecks,20);

		IniFile.getValue("msge-sProxyURL",				CWebWorld::sProxyURL);
		IniFile.getValue("msge-lProxyPort",				CWebWorld::lProxyPort);
		IniFile.getValue("msge-sProxyUser",				CWebWorld::sProxyUser);
		IniFile.getValue("msge-sProxyPsw",				CWebWorld::sProxyPsw);
		CWebWorld::SetProxyInfo();

		IniFile.getValue("msge-defNoteW",				iDefNoteW);
		IniFile.getValue("msge-defNoteH",				iDefNoteH);
		IniFile.getValue("msge-prefferedProt",			iProtocol_Preffered,DEFAULT_NETPROT);
		IniFile.getValue("msge-checkForFraud",			bCheckFraudMsg,DEFAULT_CHKFRAUD);
		IniFile.getValue("msge-lSendDate",				dtLastSentMsgDate);
		IniFile.getValue("msge-MLActive",				bProtocolML_Active,1);
		IniFile.getValue("msge-MLSlot",					sProtocolML_Slot,DEFAULT_MSLOT);
		IniFile.getValue("msge-MLSkipSizeNot",			lSkipSizeNotification);
		IniFile.getValue("msge-MLDisableAExt",			lDisableAttachExtens,1);
		IniFile.getValue("msge-lNotifyAboutFMes",		lNotifyAboutFMes,1);
		IniFile.getValue("msge-NBActive",				bProtocolNB_Active,1);

		IniFile.getValue("msge-NBLocUser",				lNB_SupportLocUser,1);
		IniFile.getValue("msge-NBNonStandAlias",		lNB_SupportNonStandAlias);
		IniFile.getValue("msge-NBLanaNum",				iNB_LanaNum,0);
		IniFile.getValue("msge-XPCActive",				bProtocolXPC_Active,1);
		IniFile.getValue("msge-XPCPort",				dwProtocolXPC_Port,XPC_PORTDEF);
		IniFile.getValue("msge-FBDir",					sFBDir);
		IniFile.getValue("msge-FBItemAliveDays",		lFBIAliveTime,90);

		CString sSubscr;
		IniFile.getValue("msge-FBSubscribed",			sSubscr,"MAIN");
		ConvertComboDataToArray(sSubscr,aSubsBB,'\\');
		IniFile.getValue("msge-FBCrypt",				lFBCrypt,bStartWithOptions?1:0);
		IniFile.getValue("msge-FBMinPeriod",			dwMinFBPeriod,7);
		IniFile.getValue("msge-FBMidPeriod",			dwMidFBPeriod,100);
		IniFile.getValue("msge-FBOnSharedItem",			bOnSharedItem,1);
		IniFile.getValue("msge-FBOnSharedItemR",		bOnSharedItemR,1);
		IniFile.getValue("msge-FBOnSharedItemT",		bOnSharedItemT,1);
		IniFile.getValue("msge-FBOnSharedItemN",		bOnSharedItemN,1);
		IniFile.getValue("msge-FBOnSharedItemM",		bOnSharedItemM,1);
		IniFile.getValue("msge-FBMsgCount",				bFBMessageCount);
		IniFile.getValue("msge-RSActive",				bProtocolRS_Active,1);

		IniFile.getValue("msge-RSSSummar",				lRSSSummar);
		IniFile.getValue("msge-RSSLocUser",				lRSSLocUser,1);
		IniFile.getValue("msge-RSSMaxNews",				dwProtocolRSSMAxNews,10);
		IniFile.getValue("msge-RSSOType",				dwProtocolRSSOType,2);
		IniFile.getValue("msge-EMActive",				bProtocolEM_Active,1);
		IniFile.getValue("msge-EM-PERIOD",				lEMPeriod,100);
		//IniFile.getValue("msge-NotifyOfRead",			lFBNotifyOfRead,1);
		IniFile.getValue("msge-ReqNotifyOfRead",		lReqNotifyOfRead);
		IniFile.getValue("msge-ReqNotifyOfDeliv",		lReqNotifyOfDeliv);
		IniFile.getValue("msge-showNotifInPopup",		lShowNotInPopup,1);
		IniFile.getValue("msge-showNotifAfterTime",		lShowNotAfterTime);
		IniFile.getValue("msge-AutoPopupAdrBook",		bAutoPopupAdrBook);
		//IniFile.getValue("msge-oneNoteForOneSender",	bGroupBySender);
		IniFile.getValue("msge-autoQuote",				bAutoQuote);
		IniFile.getValue("msge-onDuplMessage",			bOnDuplMessage);
		IniFile.getValue("msge-awayOnScrSaver",			bSetAwayIfScrSaver,0);
		IniFile.getValue("msge-askToResendMsg",			bAskToResendMsg);
		IniFile.getValue("msge-delIfDelivered",			bDelMsgIfDelivered,0);
		IniFile.getValue("msge-wordWrapIncoming",		bWWIncomingMsg,1);
		IniFile.getValue("msge-escDeletIncoming",		bEscDelIncomingMsg,1);
		IniFile.getValue("msge-saveAttachments",		bSaveNoteAttachments);
		IniFile.getValue("msge-showMsgProgress",		bShowMsgProgress);
		IniFile.getValue("msge-hideFolder",				bHideMsgFld,0);
		IniFile.getValue("msge-autoMoveNoteToMsg",		bAutoMoveNoteToMsg);
		IniFile.getValue("msge-soundOnSend",			bSoundOnMsgSend,1);
		IniFile.getValue("msge-soundOnDelivery",		bSoundOnMsgDeliv,1);
		IniFile.getValue("msge-soundOnFailedDelivery",	bSoundOnMsgNotDeliv,1);

		IniFile.getValue("msge-receiveToDos",			bReceiveToDosAction);
		IniFile.getValue("msge-receiveRemds",			bReceiveRemdsAction);
		IniFile.getValue("msge-quotationLen",			sQuotationLen,150);
		bAutoAnswer=0;//IniFile.getValue("msge-autoAnswer",	bAutoAnswer);

		IniFile.getValue("msge-lastSaveAttachPath",		sLastSaveAttachDir,	"C:\\");
		IniFile.getValue("msge-autoAnswerToUnknown",	bARForUnknownUsers);
		IniFile.getValue("msge-autoAnswerInOnline",		sAutoReplys[0],DEFAULT_AASTATUS0);
		IniFile.getValue("msge-autoAnswerInAway",		sAutoReplys[1],DEFAULT_AASTATUS1);
		IniFile.getValue("msge-autoAnswerInBusy",		sAutoReplys[2],DEFAULT_AASTATUS2);
		IniFile.getValue("msge-autoAnswerInDND",		sAutoReplys[3],DEFAULT_AASTATUS3);
		IniFile.getValue("msge-soundOnIncomingMsg",		bPlaySoundOnMessage,1);

		//IniFile.getValue("msge-deleteAlertIfNotDeliv",	bDelMsgAlertIfNotDeliv,0);
		IniFile.getValue("msge-lastAttachSavePath",		sLastGetAttachPath,"C:\\");
		IniFile.getValue("msge-maxAttachSize_KB",		dwMaxAttachSizeKB,DEFAULT_MAXATTSIZE);
		IniFile.getValue("msge-notCheckForDefMsgnr",	bCheckForDefMessenger);

		//IniFile.getValue("msge-useNickAsRecipient",		bUseNickOnSend,1);
		IniFile.getValue("msge-statusUpdateTime",		iStatusUpdateTime,DEFAULT_UPDSATMIN);
		IniFile.getValue("msge-showTitleInIconMode",	iSHowTitleInIconMode,1);
		IniFile.getValue("msge-newMsg_TitleM",			sNewMess_TitleM, DEFNewMsg_TitleM);
		IniFile.getValue("msge-newMsg_BodyM",			sNewMess_BodyM, DEFNewMsg_BodyM);
		IniFile.getValue("msge-newMsg_Baloon",			lUseBloonOnNewMsg, DEFAULT_BALOONTIP);

		IniFile.getValue("msge-netTimeout",				dwNetMessagingTimeout,DEFAULT_NETTO);
		if(dwNetMessagingTimeout<1000){
			dwNetMessagingTimeout=1000;
		}
		//IniFile.getValue("msge-minimizeAlertWhileSend",	bMinConfToTray,1);
		IniFile.getValue("msge-logToFile",				SaveMesNotesByDef,1);
		IniFile.getValue("msge-logToFileByUser",		SaveMesNotesByDefForUser,1);
		IniFile.getValue("msge-logFileMaxSize",			dwMaxLogFileSize,DEFAULT_MAXLOGKB);
		IniFile.getValue("msge-recentUserByDef",		lRecentUserByDef,1);
		IniFile.getValue("msge-recentListSize",			lMsgRecentListSize,10);
		//
		int i=0;
		messageFilters.RemoveAll();
		while(1){
			CString sFilterValue;
			IniFile.getValue(Format("msge-filters%i",i),sFilterValue);
			if(sFilterValue.GetLength()==0){
				break;
			}
			CDataXMLSaver FilterData(sFilterValue);
			CString sSearchString="";
			FilterData.getValue("searchFor",sSearchString);
			DWORD dwSearchWhere=0;
			FilterData.getValue("searchWhere",dwSearchWhere);
			DWORD dwDoWhat=0;
			FilterData.getValue("doWhat",dwDoWhat);
			messageFilters.SetAtGrow(i,new CMessageFilter(sSearchString,dwSearchWhere,dwDoWhat));
			i++;
		}

		IniFile.getValue("todo-traceHistory",			bTraceToDoHist,1);
		IniFile.getValue("todo-showActiveAsTooltip",	bShowActiveTDInTray,1);
		IniFile.getValue("todo-activateEvenIfDone",		bActivateDoneToDo);
		IniFile.getValue("todo-soundOnActivation",		bSoundOnActivation,1);
		IniFile.getValue("todo-soundOnDone",			bSoundOnDone,1);
		IniFile.getValue("todo-switchCurToNewToDo",		bMakeNewTDCur,0);
		IniFile.getValue("todo-showEvenIfDone",			bShowDone,1);
		IniFile.getValue("todo-hideFolder",				bNotShowInListTodo,0);
		IniFile.getValue("todo-newToDo_TitleM",			sNewToDo_TitleM,DEFNewToDo_TitleM);
		IniFile.getValue("todo-newToDo_BodyM",			sNewToDo_BodyM,DEFNewToDo_BodyM);
		IniFile.getValue("todo-lastNewToDoPath",		sLastNewToDoPath);
		IniFile.getValue("todo-wordWrapContent",		bToDoWordWrap,1);
		//IniFile.getValue("todo-autoCreateTodoFromMW",	bAutoCreateTodoFromMW,1);

		IniFile.getValue("adrb-SaveTo",					sAddrFile,DEFAULT_ABOOKFILE);
		IniFile.getValue("adrb-column0Width",			iCol0WidthAdrb,110);
		IniFile.getValue("adrb-column1Width",			iCol1WidthAdrb,150);
		IniFile.getValue("adrb-column2Width",			iCol2WidthAdrb,210);
		IniFile.getValue("adrb-msgCountInTray",			lShowMsgInAddrbookIcon,1);
		IniFile.getValue("adrb-showUnreadOnPersonName",	lShowUnreadInAddrb,1);
		IniFile.getValue("adrb-lastOpenedTab",			iLastViewTab);

		IniFile.getValue("remd-SaveTo",					sRemdDirectory,"Reminders");
		IniFile.getValue("remd-activateFromSleep",		lRemindFromSleep);
		IniFile.getValue("remd-showEvenIfInactive",		bShowNonActiveReminders,1);
		IniFile.getValue("remd-hideFolder",				bNotShowInListRem,0);
		IniFile.getValue("remd-soundWhenTriggered",		bRemSoundByDef,1);
		IniFile.getValue("remd-soundWhenNoteTriggered",	bRemNoteSoundByDef,1);
		IniFile.getValue("remd-soundFileForHourly",		sHourlyWav,DEFAULT_HOURLY);
		IniFile.getValue("remd-soundHourlyAsHours",		bHourlyByHCount,0);
		IniFile.getValue("remd-hideToTrayDef",			bRemHideToTrayDef,0);
		IniFile.getValue("remd-delStrictAfter",			bRemDelStrictDef,0);
		IniFile.getValue("remd-newRemd_TitleM",			sNewRemd_TitleM, DEFNewRemd_TitleM);
		IniFile.getValue("remd-newRemd_BodyM",			sNewRemd_BodyM, DEFNewRemd_BodyM);
		IniFile.getValue("remd-alertTextStyle",			lAlertTextStyle);
		IniFile.getValue("remd-preventAccSubmit",		lRemSubmitPreventer, DEF_SUBMITPREVENTER);

		IniFile.getValue("note-hideFolder",				bNotShowInListNotes,0);
		IniFile.getValue("note-dblClick",				bUseDblClickToStart,0);
		IniFile.getValue("note-SaveTo",					sNotesDirectory,"Notes");
		IniFile.getValue("note-wordWrap",				bNoteWordWrap,DEFAULT_WORDWRAP);
		IniFile.getValue("note-warningShareOK",			bSWarning_ShareOK);
		IniFile.getValue("note-defaultView",			bNoteDefaultView,DEFAULT_NOTEVIEW);
		IniFile.getValue("note-onTopByDef",				iNoteOnTopByDef,1);
		IniFile.getValue("note-enableAutohideOnAlt",	iEnableAutoHideWithAlt);
		IniFile.getValue("note-enableAutohideOnEsc",	iEnableAutoHideWithEsc);
		IniFile.getValue("note-popupOnAppActivation",	bPopupNotesOnActivation);
		IniFile.getValue("note-newNote_TitleM",			sNewNote_TitleM, DEFNewNote_TitleM);
		IniFile.getValue("note-newNote_BodyM",			sNewNote_BodyM, DEFNewNote_BodyM);
		IniFile.getValue("note-iconicSize",				lNOTEICON_DIM, 2*GetSystemMetrics(SM_CYMENU)+2);
		IniFile.getValue("note-showTextInIcon",			lShowTextInIcon,1);
		IniFile.getValue("note-lastSaveNoteTextPath",	sLastSaveNoteTextDir,	"C:\\");
		IniFile.getValue("note-checkPinnedNoteForRect",	bCheckPinnedNoteForRect, 1);
		IniFile.getValue("note-standDelPosition",		bNoteDelButtonPos,DEFAULT_DELBUTTONP);
		IniFile.getValue("note-standDelWay",			bNoteDelButtonWay,1);
		IniFile.getValue("note-standOptPosition",		bNoteOptButtonPos,DEFAULT_OTHBUTTONP);
		IniFile.getValue("note-askForDel",				bAskDlg_DelNote);
		IniFile.getValue("note-autoHideWarn",			bNoteAHideMode);
		IniFile.getValue("note-noteCTMode",				bNoteCTMode);
		IniFile.getValue("note-autoDisp",				bAutoDisp,1);
		IniFile.getValue("note-adjustOnResC",			bAdjustNotesPosOnScr,1);
		//	
		memset(DefActionMap,0,sizeof(DefActionMap));
		for(int k=0;k<(sizeof(DefActionMap)/sizeof(DefActionMap[0]));k++){
			IniFile.getValue(Format("uint-trayActionMap%i",k),	DefActionMap[k],DefActionMap_Preinit[k]);
		}
		//
		memset(DefActionMapHK,0,sizeof(DefActionMapHK));
		for(int j=0;j<(sizeof(DefActionMapHK)/sizeof(DefActionMapHK[0]));j++){
			IniFile.getValue(Format("uint-hotkey%i",j),	DefActionMapHK[j],DefActionMapHK_Preinit[j]);
		}
		if(objSettings.bNoSend){
			DefActionMapHK[NEW_MSG]=CIHotkey(0);
			DefActionMapHK_Preinit[NEW_MSG]=0;
			DefActionMap[NEW_MSG]=0;
		}
		//
		i=1;
		while(1){
			CString sSchValue;
			IniFile.getValue(Format("uint-colorScheme%i",i),sSchValue);
			if(sSchValue.GetLength()==0){
				break;
			}
			CString sName=Format("style%i",i);
			DWORD dwBgColor,dwTxtColor,dwAlpha,dwAutoTrans;
			CFont* font=CreateFontFromStr(sSchValue,&sName,&dwBgColor,&dwTxtColor,&dwAlpha,&dwAutoTrans);
			objSchemes.Add(sName,dwBgColor,dwTxtColor,font,dwAlpha,dwAutoTrans);
			i++;
		}
		// Стили по умолчанию...
		if(objSchemes.GetSize()<=1){
			long iTmp=0;
			objSchemes.getName(iTmp)=_l("System");
			objSchemes.Add(_l("White"),RGB(250,250,250),RGB(0,0,0),100);
			objSchemes.Add(_l("Antique"),RGB(0xFA,0xEB,0xD7),RGB(0,0,0),100);
			objSchemes.Add(_l("Black"),RGB(0,0,0),RGB(255,255,255),100);
			objSchemes.Add(_l("Grey"),RGB(153,153,153),RGB(255,255,255),100);
			objSchemes.Add(_l("Navy"),RGB(0,0,0x80),RGB(255,255,255),100);
			objSchemes.Add(_l("Blue"),RGB(0,0,0xFF),RGB(255,255,255),100);
			objSchemes.Add(_l("Yellow"),RGB(255,255,102),RGB(0,0,0),50,0,1);
			objSchemes.Add(_l("Gold"),RGB(0xFF,0xD7,0),RGB(0,0,0),100);
			objSchemes.Add(_l("Green"),RGB(51,255,0),RGB(0,0,0),100);
			objSchemes.Add(_l("Greeny"),RGB(0xAD,0xFF,0x2F),RGB(0,0,0),100);
			objSchemes.Add(_l("Olive"),RGB(0x55,0x6B,0x2F),RGB(255,255,255),100);
			objSchemes.Add(_l("Orchid"),RGB(0x99,0x32,0xCC),RGB(0,0,0),100);
			objSchemes.Add(_l("Pink"),RGB(255,102,153),RGB(0,0,0),100);
			objSchemes.Add(_l("Brick"),RGB(0xB2,0x22,0x22),RGB(255,255,255),100);
			objSchemes.Add(_l("Alerts"),RGB(0xFA,0xEB,0xD7),RGB(0,0,0),100);
		}
		// Обновляем кому какие схемы после загрузки схем
		IniFile.getValue("todo-style",	iStartTaskNoteScheme,4);
		IniFile.getValue("msge-style",	iStartMessageScheme,4);
		IniFile.getValue("note-style",	iStartNoteScheme,7);
		IniFile.getValue("remd-style",	iRemdStyle,15);
		IniFile.getValue("alrt-style",	iAlertStyle,15);
		//
		if(iNB_LanaNum<asEnum.length){
			dwNB_Lana=asEnum.lana[iNB_LanaNum];
		}else{
			dwNB_Lana=0;
		}
		//
	}
	if(IsEmbedded()){
		//iTrayIcon=0;
	}
	ApplySettings();
	bSaveAllowed=1;
}

void CSettings::Save()
{
	CDataXMLSaver IniFile(getFileFullName(CString(PROGNAME)+".conf"),"application-config",FALSE);
	int m=0,m1=0;
	// UINT
	IniFile.putValue("uint-language",				GetApplicationLang());
	IniFile.putValue("cur-version",					PROG_VERSION);
	//
	IniFile.putValue("uint-styleMain",				iMainStyle);
	IniFile.putValue("uint-stylePrefs",				lPreferencesLevel);
	IniFile.putValue("uint-trayIcon",				iTrayIcon);
	IniFile.putValue("uint-showSystemErrors",		bShowSystemErrors);
	IniFile.putValue("uint-soundOnError",			bErrorSound);
	IniFile.putValue("uint-soundOnErrorWav",		sWavOnError);
	IniFile.putValue("uint-autoGetNoteTextInterval",bAutoGetNoteText);
	IniFile.putValue("uint-renameInsteadOfDelete",	bDelToTrash);
	IniFile.putValue("uint-whattodoOnXButton",		lOnExitFromMainWnd);
	IniFile.putValue("uint-maxTrashSize",			bMaxTrashSize);
	IniFile.putValue("uint-globalSIDCounter",		dwGlobalCount);
	IniFile.putValue("uint-autoSaveTime",			dwAutoSaveTime);
	IniFile.putValue("uint-buttons",				bButtonsStyle);
	IniFile.putValue("uint-animOnNoteDel",			iAnimOnNoteDel);
	IniFile.putValue("uint-animOnNewMsg",			iAnimOnNewMsg);
	IniFile.putValue("uint-messageCentreDefAction",	bMessCentreDefAction);
	IniFile.putValue("uint-lastSortedColumn",		iSortColumn);
	IniFile.putValue("uint-lastASortedColumn",		iSortAColumn);
	IniFile.putValue("uint-column0Width",			iCol0Width);
	IniFile.putValue("uint-column1Width",			iCol1Width);
	IniFile.putValue("uint-column2Width",			iCol2Width);
	IniFile.putValue("uint-feditH",					iFeditH);
	IniFile.putValue("uint-feditHR",				lFEditHR);
	IniFile.putValue("uint-itemsInTrayMenu",		iMenuItemsCount);
	IniFile.putValue("uint-stickWindowsThreshold",	iStickPix);
	IniFile.putValue("uint-wndshadows",				bShadows);
	IniFile.putValue("uint-autoPopupWindow",		APToDoList);
	IniFile.putValue("uint-trashDirectory",			sTrashDirectory);
	IniFile.putValue("uint-generateRandomNames",	bGenerateRandomNames);
	IniFile.putValue("uint-trayMenuMode",			sRootMenuId);
	IniFile.putValue("uint-showDateInTrayIcon",		lShowDateInTrayIcon);
	IniFile.putValue("uint-trayIconsMaskO",			lTrayIconsMaskO);
	IniFile.putValue("uint-trayIconsMaskM",			lTrayIconsMaskM);
	IniFile.putValue("uint-lastFindString",			sLastFindString);
	IniFile.putValue("uint-hideDateFld",			bHideDateFld);
	IniFile.putValue("uint-doubleClickTray",		UseBblClick);
	IniFile.putValue("uint-alertToTaskbar",			lAlertsOnTaskbar);
	IniFile.putValue("uint-checkForUpdates",		lCheckForUpdate);
	IniFile.putValue("uint-checkForUpdDays",		lDaysBetweenChecks);
	for(int k=0;k<(sizeof(DefActionMap)/sizeof(DefActionMap[0]));k++){
		IniFile.putValue(Format("uint-trayActionMap%i",k),	DefActionMap[k]);
	}
	for(int j=0;j<(sizeof(DefActionMapHK)/sizeof(DefActionMapHK[0]));j++){
		IniFile.putValue(Format("uint-hotkey%i",j),	DefActionMapHK[j]);
	}
	IniFile.putValue("stat-sendSMS",				iSMSCount);
	IniFile.putValue("stat-receivedMails",			lStatReceivedMails);
	IniFile.putValue("stat-receivedSpamMails",		lStatReceivedSpamMails);
	IniFile.putValue("stat-receivedMsg",			lStatReceivedMessages);
	IniFile.putValue("stat-sendedMsg",				lStatSendedMessages);
	IniFile.putValue("stat-notesCreated",			lStatNotesCreated);

	// TODO
	IniFile.putValue("todo-SaveTo",					sTaskDirectory);
	IniFile.putValue("todo-style",					iStartTaskNoteScheme);
	IniFile.putValue("todo-showActiveAsTooltip",	bShowActiveTDInTray);
	IniFile.putValue("todo-activateEvenIfDone",		bActivateDoneToDo);
	IniFile.putValue("todo-soundOnActivation",		bSoundOnActivation);
	IniFile.putValue("todo-soundOnDone",			bSoundOnDone);
	IniFile.putValue("todo-soundOnActivationWav",	sWavOnActivation);
	IniFile.putValue("todo-soundOnDoneWav",			sWavSoundOnDone);
	IniFile.putValue("todo-traceHistory",			bTraceToDoHist);
	IniFile.putValue("todo-switchCurToNewToDo",		bMakeNewTDCur);
	IniFile.putValue("todo-showEvenIfDone",			bShowDone);
	IniFile.putValue("todo-hideFolder",				bNotShowInListTodo);
	IniFile.putValue("todo-newToDo_TitleM",			sNewToDo_TitleM);
	IniFile.putValue("todo-newToDo_BodyM",			sNewToDo_BodyM);
	IniFile.putValue("todo-lastNewToDoPath",		sLastNewToDoPath);
	IniFile.putValue("todo-wordWrapContent",		bToDoWordWrap);
	IniFile.putValue("todo-autoCreateTodoFromMW",	bAutoCreateTodoFromMW);
	for(m=0;m<lToDoFRecentList.GetSize();m++){
		IniFile.putValue(Format("todo-recentFolder%i",m), lToDoFRecentList[m]);
	}

	// MSGE
	IniFile.putValue("msge-sProxyURL",				CWebWorld::sProxyURL);
	IniFile.putValue("msge-lProxyPort",				CWebWorld::lProxyPort);
	IniFile.putValue("msge-sProxyUser",				CWebWorld::sProxyUser);
	IniFile.putValue("msge-sProxyPsw",				CWebWorld::sProxyPsw);
	IniFile.putValue("msge-defNoteW",				iDefNoteW);
	IniFile.putValue("msge-defNoteH",				iDefNoteH);
	IniFile.putValue("msge-prefferedProt",			iProtocol_Preffered);
	IniFile.putValue("msge-checkForFraud",			bCheckFraudMsg);
	IniFile.putValue("msge-lSendDate",				dtLastSentMsgDate);
	IniFile.putValue("msge-MLActive",				bProtocolML_Active);
	IniFile.putValue("msge-MLSlot",					sProtocolML_Slot);
	IniFile.putValue("msge-MLSkipSizeNot",			lSkipSizeNotification);
	IniFile.putValue("msge-lNotifyAboutFMes",		lNotifyAboutFMes);
	IniFile.putValue("msge-MLDisableAExt",			lDisableAttachExtens);
	IniFile.putValue("msge-NBActive",				bProtocolNB_Active);
	IniFile.putValue("msge-NBAlias",				sProtocolNB_Alias);
	IniFile.putValue("msge-NBLocUser",				lNB_SupportLocUser);
	IniFile.putValue("msge-NBNonStandAlias",		lNB_SupportNonStandAlias);
	IniFile.putValue("msge-NBLanaNum",				iNB_LanaNum);
	IniFile.putValue("msge-XPCActive",				bProtocolXPC_Active);
	IniFile.putValue("msge-XPCPort",				dwProtocolXPC_Port);
	IniFile.putValue("msge-FBDir",					sFBDir);
	IniFile.putValue("msge-FBItemAliveDays",		lFBIAliveTime);
	IniFile.putValue("msge-FBNick",					sFBNick);
	IniFile.putValue("msge-FBSubscribed",			ConvertArrayToString(aSubsBB,'\\'));
	IniFile.putValue("msge-FBCrypt",				lFBCrypt);
	IniFile.putValue("msge-EMActive",				bProtocolEM_Active);
	IniFile.putValue("msge-RSActive",				bProtocolRS_Active);
	IniFile.putValue("msge-RSSSummar",				lRSSSummar);
	IniFile.putValue("msge-RSSLocUser",				lRSSLocUser);
	IniFile.putValue("msge-RSSMaxNews",				dwProtocolRSSMAxNews);
	IniFile.putValue("msge-RSSOType",				dwProtocolRSSOType);
	IniFile.putValue("msge-EM-ADS",					sEM_Ads);
	IniFile.putValue("msge-EM-P3-HOST",				sEM_p3host);
	IniFile.putValue("msge-EM-P3-PORT",				lEM_p3port);
	IniFile.putValue("msge-EM-P3-LOGN",				sEM_p3login);
	IniFile.putValue("msge-EM-P3-PASW",				sEM_p3psw);
	//IniFile.putValue("msge-EM-P3-BOX",				lEM_p3box);
	IniFile.putValue("msge-EM-SM-HOST",				sEM_smhost);
	IniFile.putValue("msge-EM-SM-PORT",				sEM_smport);
	IniFile.putValue("msge-EM-SM-LOGN",				sEM_smlogin);
	IniFile.putValue("msge-EM-SM-PASW",				sEM_smpsw);
	IniFile.putValue("msge-EM-EMAIL",				sEMmail);
	IniFile.putValue("msge-EM-PERIOD",				lEMPeriod);
	//IniFile.putValue("msge-NotifyOfRead",			lFBNotifyOfRead);
	IniFile.putValue("msge-ReqNotifyOfRead",		lReqNotifyOfRead);
	IniFile.putValue("msge-ReqNotifyOfDeliv",		lReqNotifyOfDeliv);
	IniFile.putValue("msge-showNotifInPopup",		lShowNotInPopup);
	IniFile.putValue("msge-showNotifAfterTime",		lShowNotAfterTime);
	IniFile.putValue("msge-FBID",					sFB_ID);
	IniFile.putValue("msge-FBMinPeriod",			dwMinFBPeriod);
	IniFile.putValue("msge-FBMidPeriod",			dwMidFBPeriod);
	IniFile.putValue("msge-FBOnSharedItem",			bOnSharedItem);
	IniFile.putValue("msge-FBOnSharedItemR",		bOnSharedItemR);
	IniFile.putValue("msge-FBOnSharedItemT",		bOnSharedItemT);
	IniFile.putValue("msge-FBOnSharedItemN",		bOnSharedItemN);
	IniFile.putValue("msge-FBOnSharedItemM",		bOnSharedItemM);
	IniFile.putValue("msge-FBMsgCount",				bFBMessageCount);
	IniFile.putValue("msge-netTimeout",				dwNetMessagingTimeout);
	IniFile.putValue("msge-logToFile",				SaveMesNotesByDef);
	IniFile.putValue("msge-logToFileByUser",		SaveMesNotesByDefForUser);
	IniFile.putValue("msge-logFileDirectory",		sMessageLogDirectory);
	IniFile.putValue("msge-logFileMaxSize",			dwMaxLogFileSize);
	IniFile.putValue("msge-style",					iStartMessageScheme);
	IniFile.putValue("msge-onDuplMessage",			bOnDuplMessage);
	IniFile.putValue("msge-autoQuote",				bAutoQuote);
	IniFile.putValue("msge-delIfDelivered",			bDelMsgIfDelivered);
	IniFile.putValue("msge-wordWrapIncoming",		bWWIncomingMsg);
	IniFile.putValue("msge-escDeletIncoming",		bEscDelIncomingMsg);
	IniFile.putValue("msge-saveAttachments",		bSaveNoteAttachments);
	IniFile.putValue("msge-showMsgProgress",		bShowMsgProgress);
	IniFile.putValue("msge-hideFolder",				bHideMsgFld);
	IniFile.putValue("msge-autoMoveNoteToMsg",		bAutoMoveNoteToMsg);
	IniFile.putValue("msge-soundOnSend",			bSoundOnMsgSend);
	IniFile.putValue("msge-soundOnDelivery",		bSoundOnMsgDeliv);
	IniFile.putValue("msge-soundOnFailedDelivery",	bSoundOnMsgNotDeliv);
	IniFile.putValue("msge-soundOnSendWav",			sWavOnMsgSend);
	IniFile.putValue("msge-soundOnDeliveryWav",		sWavOnMsgDeliv);
	IniFile.putValue("msge-soundOnFailedDeliveryWav",sWavOnMsgNotDeliv);
	IniFile.putValue("msge-awayOnScrSaver",			bSetAwayIfScrSaver);
	IniFile.putValue("msge-askToResendMsg",			bAskToResendMsg);
	//IniFile.putValue("msge-deleteAlertIfNotDeliv",	bDelMsgAlertIfNotDeliv);
	IniFile.putValue("msge-statusUpdateTime",		iStatusUpdateTime);
	IniFile.putValue("msge-showTitleInIconMode",	iSHowTitleInIconMode);
	IniFile.putValue("msge-currentOnlineStatus",	iOnlineStatus);
	//IniFile.putValue("msge-minimizeAlertWhileSend",	bMinConfToTray);
	IniFile.putValue("msge-soundFile",				sMessageWavFile);
	IniFile.putValue("msge-lastSaveAttachPath",		sLastSaveAttachDir);
	IniFile.putValue("msge-autoAnswer",				bAutoAnswer);
	IniFile.putValue("msge-autoAnswerToUnknown",	bARForUnknownUsers);
	IniFile.putValue("msge-autoAnswerInOnline",		sAutoReplys[0]);
	IniFile.putValue("msge-autoAnswerInAway",		sAutoReplys[1]);
	IniFile.putValue("msge-autoAnswerInBusy",		sAutoReplys[2]);
	IniFile.putValue("msge-autoAnswerInDND",		sAutoReplys[3]);
	IniFile.putValue("msge-soundOnIncomingMsg",		bPlaySoundOnMessage);
	IniFile.putValue("msge-actionOnIncomingMsg",	bOnNewMessage);
	//IniFile.putValue("msge-useNickAsRecipient",		bUseNickOnSend);
	IniFile.putValue("msge-AutoPopupAdrBook",		bAutoPopupAdrBook);
	//IniFile.putValue("msge-oneNoteForOneSender",	bGroupBySender);
	IniFile.putValue("msge-lastAttachSavePath",		sLastGetAttachPath);
	IniFile.putValue("msge-maxAttachSize_KB",		dwMaxAttachSizeKB);
	IniFile.putValue("msge-notCheckForDefMsgnr",	bCheckForDefMessenger);
	IniFile.putValue("msge-recentUserByDef",		lRecentUserByDef);
	IniFile.putValue("msge-newMsg_TitleM",			sNewMess_TitleM);
	IniFile.putValue("msge-newMsg_BodyM",			sNewMess_BodyM);
	IniFile.putValue("msge-newMsg_Baloon",			lUseBloonOnNewMsg);
	IniFile.putValue("msge-recentListSize",			lMsgRecentListSize);
	IniFile.putValue("msge-receiveToDos",			bReceiveToDosAction);
	// Юыстрые ответы/последние полчатели
	IniFile.putValue("msge-receiveRemds",			bReceiveRemdsAction);
	IniFile.putValue("msge-quotationLen",			sQuotationLen);
	for(m=0,m1=0;m<lMsgRecentList.GetSize();m++){
		if(lMsgRecentList[m].GetLength()>0){
			IniFile.putValue(Format("msge-recentListVal%i",m1++), lMsgRecentList[m]);
		}
	}
	for(m=0,m1=0;m<lMsgQAnswers.GetSize();m++){
		IniFile.putValue(Format("msge-recentQAnswer%i",m1++), lMsgQAnswers[m]);
	}
	//
	int jkC=0;
	for(int jk=0;jk<messageFilters.GetSize();jk++){
		CString sFilterValue;
		if(messageFilters[jk]){
			CDataXMLSaver FilterData(sFilterValue);
			FilterData.putValue("searchFor",messageFilters[jk]->sSearchString);
			FilterData.putValue("searchWhere",messageFilters[jk]->dwSearchWhere);
			FilterData.putValue("doWhat",messageFilters[jk]->dwDoWhat);
			IniFile.putValue(Format("msge-filters%i",jkC),FilterData.GetResult(),XMLNoConversion);
			jkC++;
		}
	}
	// REMD
	IniFile.putValue("remd-SaveTo",					sRemdDirectory);
	IniFile.putValue("remd-style",					iRemdStyle);
	IniFile.putValue("remd-soundWhenTriggered",		bRemSoundByDef);
	IniFile.putValue("remd-soundWhenNoteTriggered",	bRemNoteSoundByDef);
	IniFile.putValue("remd-showEvenIfInactive",		bShowNonActiveReminders);
	IniFile.putValue("remd-activateFromSleep",		lRemindFromSleep);
	IniFile.putValue("remd-hideFolder",				bNotShowInListRem);
	IniFile.putValue("remd-soundFile",				sRemFileWav);
	IniFile.putValue("remd-soundFileForNote",		sRemNoteFileWav);
	IniFile.putValue("remd-soundFileForHourly",		sHourlyWav);
	IniFile.putValue("remd-soundHourlyAsHours",		bHourlyByHCount);
	IniFile.putValue("remd-hideToTrayDef",			bRemHideToTrayDef);
	IniFile.putValue("remd-delStrictAfter",			bRemDelStrictDef);
	IniFile.putValue("remd-newRemd_TitleM",			sNewRemd_TitleM);
	IniFile.putValue("remd-newRemd_BodyM",			sNewRemd_BodyM);
	IniFile.putValue("remd-alertTextStyle",			lAlertTextStyle);
	IniFile.putValue("remd-preventAccSubmit",		lRemSubmitPreventer);

	//NOTE
	IniFile.putValue("note-SaveTo",					sNotesDirectory);
	IniFile.putValue("note-style",					iStartNoteScheme);
	IniFile.putValue("note-hideFolder",				bNotShowInListNotes);
	IniFile.putValue("note-dblClick",				bUseDblClickToStart);
	IniFile.putValue("note-wordWrap",				bNoteWordWrap);
	IniFile.putValue("note-warningShareOK",			bSWarning_ShareOK);
	IniFile.putValue("note-defaultView",			bNoteDefaultView);
	IniFile.putValue("note-onTopByDef",				iNoteOnTopByDef);
	IniFile.putValue("note-enableAutohideOnAlt",	iEnableAutoHideWithAlt);
	IniFile.putValue("note-enableAutohideOnEsc",	iEnableAutoHideWithEsc);
	IniFile.putValue("note-popupOnAppActivation",	bPopupNotesOnActivation);
	IniFile.putValue("note-newNote_TitleM",			sNewNote_TitleM);
	IniFile.putValue("note-newNote_BodyM",			sNewNote_BodyM);
	IniFile.putValue("note-iconicSize",				lNOTEICON_DIM);
	IniFile.putValue("note-showTextInIcon",			lShowTextInIcon);
	IniFile.putValue("note-lastSaveNoteTextPath",	sLastSaveNoteTextDir);
	IniFile.putValue("note-checkPinnedNoteForRect",	bCheckPinnedNoteForRect);
	IniFile.putValue("note-standDelPosition",		bNoteDelButtonPos);
	IniFile.putValue("note-standDelWay",			bNoteDelButtonWay);
	IniFile.putValue("note-standOptPosition",		bNoteOptButtonPos);
	IniFile.putValue("note-askForDel",				bAskDlg_DelNote);
	IniFile.putValue("note-autoHideWarn",			bNoteAHideMode);
	IniFile.putValue("note-noteCTMode",				bNoteCTMode);
	IniFile.putValue("note-autoDisp",				bAutoDisp);
	IniFile.putValue("note-adjustOnResC",			bAdjustNotesPosOnScr);
	//
	IniFile.putValue("spam-accountCount",			lAntiSpamAccountsFromNextStart);
	IniFile.putValue("spam-addrFile",				sSpamAddressesFile);
	IniFile.putValue("spam-subjFile",				sSpamSubjectsFile);
	IniFile.putValue("spam-bodyFile",				sSpamBodyFile);
	IniFile.putValue("spam-headFile",				sSpamHeadFile);
	IniFile.putValue("spam-removeWhireSpaces",		lRemoveSubjWhiteSpaces);
	IniFile.putValue("spam-ignoreCase",				lRemoveSubjCase);
	IniFile.putValue("spam-useMaskInSStr",			lAllowMasksInSpamStrings);
	IniFile.putValue("spam-spam5SqeSpaces",			lSubjSeqSpaces);
	IniFile.putValue("spam-showWhySpam",			lShowWhySpam);
	IniFile.putValue("spam-col0width",				iSpamCol0Width);
	IniFile.putValue("spam-col1width",				iSpamCol1Width);
	IniFile.putValue("spam-col2width",				iSpamCol2Width);
	IniFile.putValue("spam-askForKill",				lOnCloseSpamWindow);
	IniFile.putValue("spam-soundOnMail",			bSoundOnMail);
	IniFile.putValue("spam-soundOnMailWav",			sWavSoundOnMail);
	IniFile.putValue("spam-logFileMaxSize",			dwMaxMailLogFileSize);
	IniFile.putValue("spam-timeout",				dwMailboxTimeout);
	IniFile.putValue("spam-reportInTray",			lShowSpamReportInTray);
	IniFile.putValue("spam-linesToLoad",			objSettings.lAdditionalHeaderLines);
	IniFile.putValue("spam-maxNotSpamSize",			objSettings.lMaxSizeToBeSpam);
	IniFile.putValue("spam-spamFileExtensions",		sSPAMFileExtensions);
	IniFile.putValue("spam-noUnkCharsets",			lBlockMailInUnknownCharset);
	IniFile.putValue("spam-noMessedChars",			lBlockMailWithMessedChars);
	IniFile.putValue("spam-noThisCharsets",			sBlockMailsWithThisCharsets);
	IniFile.putValue("spam-userDefinedEmCl",		sUserDefinedEmailB);

	for(int si=0;si<lAntiSpamAccounts;si++){
		IniFile.putValue(Format("spam-account%i",si),antiSpamAccounts[si].SerializeToString(),XMLNoConversion);
	}
	// ADBR
	IniFile.putValue("adrb-SaveTo",					sAddrFile);
	IniFile.putValue("adrb-column0Width",			iCol0WidthAdrb);
	IniFile.putValue("adrb-column1Width",			iCol1WidthAdrb);
	IniFile.putValue("adrb-column2Width",			iCol2WidthAdrb);
	IniFile.putValue("adrb-msgCountInTray",			lShowMsgInAddrbookIcon);
	IniFile.putValue("adrb-showUnreadOnPersonName",	lShowUnreadInAddrb);
	IniFile.putValue("adrb-lastOpenedTab",			iLastViewTab);
	// Other
	IniFile.putValue("link-SaveTo",					sLinksFile);
	IniFile.putValue("fold-SaveTo",					sFolderFile);
	IniFile.putValue("alrt-style",					iAlertStyle);
	//
	for(long i=1;i<objSchemes.GetSize();i++){
		IniFile.putValue(Format("uint-colorScheme%i",i),CreateStrFromFont(objSchemes.getFont(i),objSchemes.getName(i), objSchemes.getBgColor(i), objSchemes.getTxtColor(i), objSchemes.getAlpha(i), 0), XMLNoConversion);
	}
	SaveBBExcl();
}


BOOL AddBBExcl(const char* szID)
{
	objSettings.bExclChanged=1;
	objSettings.sExcludeBillboardItems+=szID;
	objSettings.sExcludeBillboardItems+=";";
	return 1;
}

BOOL SaveBBExcl()
{
	if(objSettings.bExclChanged){
		objSettings.bExclChanged=0;
		long lExLen=objSettings.sExcludeBillboardItems.GetLength();
		if(lExLen>DEFBILLB_EXLSIZE){
			objSettings.sExcludeBillboardItems=objSettings.sExcludeBillboardItems.Mid(lExLen-int(DEFBILLB_EXLSIZE*0.8));
		}
		SaveFile(getFileFullName("BillboardItems")+".exc",objSettings.sExcludeBillboardItems);
	}
	return 1;
}

void CSettings::Finalize()
{
	if(bRunStatus!=-1){
		bRunStatus=-1;
		SaveAll();
		m_Notes.DeleteAllNotes();
		if(objMessageWrapper!=NULL){
			delete objMessageWrapper;
			objMessageWrapper=NULL;
		}
		for(int p=0;p<messageFilters.GetSize();p++){
			if(messageFilters[p]){
				delete messageFilters[p];
				messageFilters[p]=NULL;
			}
		}
		messageFilters.RemoveAll();
	}
}

CSettings::~CSettings()
{
	Finalize();
	DeleteCriticalSection(&lock);
	DeleteCriticalSection(&bTreeNeedUpdateLock);
};

void CSettings::ApplySettings()
{
	NoteSettings.SetColorCode(iStartNoteScheme);
	TaskNoteSettings.SetColorCode(iStartTaskNoteScheme);
	MessageNoteSettings.SetColorCode(iStartMessageScheme);
	//
	MessageNoteSettings.SetTitle(_l("Message"));
	MessageNoteSettings.SetPath(MESS_PATH);
	MessageNoteSettings.bSaveOnExit=TRUE;
	MessageNoteSettings.bTopMost=TRUE;
	MessageNoteSettings.AutoResize=TRUE;
	MessageNoteSettings.iViewMode=2;
	MessageNoteSettings.iIconMode=(objSettings.bOnNewMessage==3);
	MessageNoteSettings.bSelectTextOnStart=TRUE;
	MessageNoteSettings.bWordWrap=bWWIncomingMsg;
	//
	NoteSettings.bTopMost=iNoteOnTopByDef;
	NoteSettings.bSaveOnExit=TRUE;
	NoteSettings.bSelectTextOnStart=TRUE;
	NoteSettings.iViewMode=objSettings.bNoteDefaultView?1:0;
	//
	TaskNoteSettings.bSaveOnExit=TRUE;
	TaskNoteSettings.iViewMode=1;
	//Common
	NoteSettings.ReplyButton=TaskNoteSettings.ReplyButton=MessageNoteSettings.ReplyButton=FALSE;
	NoteSettings.iXSize=TaskNoteSettings.iXSize=MessageNoteSettings.iXSize=-1;
	NoteSettings.bWordWrap=TaskNoteSettings.bWordWrap=bNoteWordWrap;
	InitFilterList();
	// Styles
	if(iStartNoteScheme<0 || iStartNoteScheme>objSettings.objSchemes.GetSize()){
		iStartNoteScheme=0;
	}
	if(iStartTaskNoteScheme<0 || iStartTaskNoteScheme>objSettings.objSchemes.GetSize()){
		iStartTaskNoteScheme=0;
	}
	if(iStartMessageScheme<0 || iStartMessageScheme>objSettings.objSchemes.GetSize()){
		iStartMessageScheme=0;
	}
	if(iRemdStyle<0 || iRemdStyle>objSettings.objSchemes.GetSize()){
		iRemdStyle=0;
	}
	if(iAlertStyle<0 || iAlertStyle>objSettings.objSchemes.GetSize()){
		iAlertStyle=0;
	}
	if(iMainStyle<0 || iMainStyle>objSettings.objSchemes.GetSize()){
		iMainStyle=0;
	}
	//
	sMainWindowTitle=PROGNAME;
	if(objSettings.iLikStatus<2){
		sMainWindowTitle=sMainWindowTitle+" - "+_l("Unregistered");
	}else{
		sMainWindowTitle=sMainWindowTitle;
	}
	sTrayTooltip=sMainWindowTitle;
	// Создаем папку для мусора...
	CreateDirIfNotExist(getFileFullName(sTrashDirectory,TRUE));
	// Создаем папку для лога сообщений...
	CreateDirIfNotExist(getFileFullName(sMessageLogDirectory,TRUE));
}

void CSettings::SaveAll()
{
	if(bSaveAllowed==0){
		return;
	}
	HCURSOR hCur=SetCursor(theApp.LoadCursor(IDC_CURSORSAVE));
	this->Save();
	if(pMainDialogWindow){
		pMainDialogWindow->SaveFolders();
		pMainDialogWindow->SaveLinks();
	}
	objLog.FlushAllToDisk();
	AddrBook.SaveToFile();
	// Записки сохраняем раньше т.к. они могут менять ToDo (если связаны)
	m_Notes.SaveNotes();
	m_Tasks.SaveTasks();
	m_Reminders.Save();
	//objSettings.m_WChanger.Save();// Сохраняется при выходе из диалога
	SetCursor(hCur);
}

void CSettings::InitFilterList()
{
	mainViewFilter.RemoveAll();
	if(bNotShowInListTodo)
		mainViewFilter.Add(TODO_PATH);
	if(bNotShowInListNotes)
		mainViewFilter.Add(NOTE_PATH);
	if(bNotShowInListRem)
		mainViewFilter.Add(REMD_PATH);
	if(bHideMsgFld)
		mainViewFilter.Add(MESS_PATH);
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
#define OC_STYLES	10
#define OC_LIFETIME	11
#define OC_ICONAB	12
#define OC_ICONMSK	13
#define OC_STARTUP	14
#define OC_DATECOL	15
#define OC_EMAILCNT	16
#define OC_MAILTIME	17
#define OC_NETFB	18
#define OC_NETEM	19
//
#define OC_FILTERS	1000

BOOL bFileDialogOpened=FALSE;
BOOL OpenFileDialog(const char* szExtension, HIROW hData, CDLG_Options* pDialog, BOOL bSkipFileCheck=FALSE, BOOL bSaveDialog=FALSE)
{
	if(bFileDialogOpened){
		return FALSE;
	}
	BOOL bRes=FALSE;
	bFileDialogOpened=TRUE;
	CString sFile=objSettings.sLastGetAttachPath;
	if(hData){
		sFile=pDialog->m_OptionsList.GetIItemText(hData,1);
	}
	CFileDialog dlg(!bSaveDialog, NULL, bSkipFileCheck?((const char*)NULL):(sFile), OFN_NODEREFERENCELINKS, szExtension, pDialog);
	SimpleTracker Track(objSettings.bDoModalInProcess);
	if(dlg.DoModal()==IDOK){
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
			objSettings.sLastGetAttachPath=sNewFile;
		}
		bRes=TRUE;
	}
	bFileDialogOpened=FALSE;
	if(!bRes && !bSkipFileCheck){
		// Пробуем без стартового имени файла...
		DWORD dwErr=CommDlgExtendedError();
		if(dwErr==12290){
			return OpenFileDialog(szExtension, hData, pDialog, TRUE, bSaveDialog);
		}
	}
	return bRes;
}

CString GetMailboxAccountName(int si)
{
	return _l("Mailbox")+Format(" #%lu",si+1);
}

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
	strcpy(szDir,objSettings.sFBDir);
	if(GetFolder (_l("Choose directory"), szDir, NULL, pDialog?pDialog->GetSafeHwnd():NULL)){
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

HIROW hSpamBoxesPos=0;
BOOL CALLBACK JumpToSpamt(HIROW hData, CDLG_Options* pDialog)
{
	if(hSpamBoxesPos!=0){
		pDialog->MoveGlobalFocus(pDialog->GetAliasHIROW(hSpamBoxesPos),TRUE);
	}
	return TRUE;
}

BOOL CALLBACK ShowProtHelp(HIROW hData, CDLG_Options* pDialog)
{
	ShowHelp("dsc_network");
	return TRUE;
}

BOOL CALLBACK ChooseAnyFile(HIROW hData, CDLG_Options* pDialog)
{
	return OpenFileDialog("All files (*.*)|*.*||",hData,pDialog);
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

BOOL CallStylesDialogWnd(CWnd* pWnd,int iStartStyle)
{
	static long bStylesDialogOpened=0;
	if(bStylesDialogOpened>0){
		return FALSE;
	}
	SimpleTracker Track(bStylesDialogOpened);
	if(iStartStyle!=-1){
		objSettings.iLastSchemeUsed=iStartStyle;
	}
	COptionPage_Styles dlg(pWnd);
	dlg.DoModal();
	return TRUE;
}

BOOL CALLBACK CallImportAB(HIROW hData, CDLG_Options* pDialog)
{
	OpenFileDialog("Address book file (*.wna)|*.wna||", 0, 0);
	CString sFile=objSettings.sLastGetAttachPath;
	if(!isFileExist(sFile)){
		sFile=AddPathExtension(sFile,".wna");
	}
	if(isFileExist(sFile)){
		if(pMainDialogWindow && pMainDialogWindow->dlgAddBook){
			pMainDialogWindow->dlgAddBook->DestroyWindow();
			pMainDialogWindow->dlgAddBook=NULL;
		}
		objSettings.AddrBook.aPersons.RemoveAll();
		objSettings.AddrBook.aAddrBookTabs.RemoveAll();
		objSettings.AddrBook.LoadFromFile(sFile);
		ExchangeOnlineStatuses();
		if(pMainDialogWindow){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_ADDRESSBOOK,0);
		}
		Alert(_l("Address book imported successfully")+"!");
	}
	return TRUE;
}

BOOL CALLBACK CallExportAB(HIROW hData, CDLG_Options* pDialog)
{
	OpenFileDialog("Address book file (*.wna)|*.wna||", 0, 0, 0, 1);
	CString sFile=objSettings.sLastGetAttachPath;
	if(sFile.Find("debug")!=-1){
		Alert(getEmailExePath(),"Debug line");
	}
	sFile=AddPathExtension(sFile,".wna");
	objSettings.AddrBook.SaveToFile(sFile);
	Alert(_l("Address book exported successfully")+"!");
	return TRUE;
}

BOOL CALLBACK CallStylesDialog(HIROW hData, CDLG_Options* pDialog)
{
	return CallStylesDialogWnd((CWnd*)pDialog);
}

BOOL CALLBACK CallSetFolder(HIROW hData, CDLG_Options* pDialog)
{
	::ShellExecute(NULL,"open",GetUserFolder(),NULL,NULL,SW_SHOWNORMAL);
	return FALSE;
}

BOOL CALLBACK CallTFolder(HIROW hData, CDLG_Options* pDialog)
{
	::ShellExecute(NULL,"open",getFileFullName(objSettings.sTrashDirectory,TRUE),NULL,NULL,SW_SHOWNORMAL);
	return FALSE;
}

long lLikKeysPasted=0;
void GetRegInfoFromText(CString &sText,CString& sLikUser,CString& sLikKey, const char* szSalt);
BOOL CALLBACK PasteLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	static long bLikPaste=0;
	if(bLikPaste>0){
		return FALSE;
	}
	SimpleTracker TrackLik(bLikPaste);
	CString sText;
	if(pDialog){
		pDialog->CommitData(OC_LICDATA);
	}
	if(objSettings.sLikFile!=""){
		ReadFile(objSettings.sLikFile,sText);
	}
	if(sText==""){
		sText=GetClipboardText();
	}
	BOOL bRes=FALSE;
	if(!sText.IsEmpty()){
		CString sUser,sKey;
		GetRegInfoFromText(sText,sUser,sKey,"WN");
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
	if(!bRes){
		Alert(_l("No registration info found")+"!",_l("Clipboard"));
	}else{
		Alert(_l("Registration info pasted successfully")+"!",_l("Clipboard"));
	}
	if(bRes){
		lLikKeysPasted=1;
	}
	return bRes;
}

#define KSLEEP 1432
void SaveKeysInfo()
{
	CString szKey;
	szKey="user:[";
	szKey+=objSettings.sLikUser;
	szKey+="],key:[";
	szKey+=objSettings.sLikKey;
	szKey+="]";
	DWORD dwWritten=0;
	CString sLikFile=GetUserFolder()+LICENSE_KEY_FILE;
	DWORD dwLikOpenType=CREATE_NEW;
	if(isFileExist(sLikFile)){
		dwLikOpenType=TRUNCATE_EXISTING;
	}
	HANDLE hLicenseFile=::CreateFile(sLikFile, GENERIC_WRITE, 0, NULL, dwLikOpenType, 0, NULL);
	if (hLicenseFile && hLicenseFile!=INVALID_HANDLE_VALUE){
		//::SetFilePointer(hLicenseFile,0,NULL,FILE_BEGIN);
		::WriteFile(hLicenseFile,szKey,strlen(szKey),&dwWritten,NULL);
		//::SetEndOfFile(hLicenseFile);
		::CloseHandle(hLicenseFile);
	}
}

BOOL CALLBACK CheckLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	static long bLikKDialog=0;
	if(bLikKDialog>0){
		return FALSE;
	}
	SimpleTracker TrackLik(bLikKDialog);
	SimpleTracker TrackCom(objSettings.bDoModalInProcess);
	if(pDialog){
		pDialog->CommitData();
	}
	objSettings.sLikKey.TrimLeft();
	objSettings.sLikKey.TrimRight();
	objSettings.sLikUser.TrimLeft();
	objSettings.sLikUser.TrimRight();
	if(objSettings.sLikKey.IsEmpty() || objSettings.sLikUser.IsEmpty()){
		return FALSE;
	}
	CString sError,sUsrName=objSettings.sLikUser;
	if(!isUserNameValid(sUsrName,sError)){
		Sleep(KSLEEP);
		Alert(_l("Invalid user name")+": "+_l(sError)+"!");
		return TRUE;// becouse of commit
	}
	if(!isKeyValid(objSettings.sLikKey)){
		Sleep(KSLEEP);
		Alert(_l("Invalid registration key")+"!");

	}else{
		Sleep(KSLEEP);
		Alert(_l("Restart program to see changes"),_l("Registration key accepted"));
	}
	SaveKeysInfo();
	return TRUE;
}

BOOL CALLBACK HowGetLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	ShowHelp("Registration");
	return TRUE;
}

HIROW hFilters=NULL;
BOOL CALLBACK RemoveMFilter(HIROW hData, CDLG_Options* pDialog)
{
	static long lRemItem=0;
	if(lRemItem){
		return FALSE;
	}
	SimpleTracker Track(lRemItem);
	//
	if(!pDialog){
		return FALSE;
	}
	long lOptClassToDel=-1,lIDToDel=-1;
	long lID=long(hData);
	if(lID>=0 && lID<objSettings.messageFilters.GetSize()){
		CMessageFilter* pItem=objSettings.messageFilters[lID];
		pDialog->MoveGlobalFocus(hFilters,TRUE);//pDialog->GetAliasHIROW???
		if(pItem){
			pDialog->PopSomeDataFromScreen(OC_FILTERS+lID);
			objSettings.messageFilters[lID]=NULL;
			pDialog->Invalidate();
			delete pItem;
		}
	}
	// Чтобы вызывающий код не придрался
	return FALSE;
}

int iFilterCount=0;
BOOL CALLBACK AddMFilterEx(HIROW hData, CDLG_Options* pDialog, int iFilterNum=-1)
{
	if(!pDialog){
		return FALSE;
	}
	static long lAddItem=0;
	if(lAddItem){
		return FALSE;
	}
	SimpleTracker Track(lAddItem);
	if(iFilterNum<0){
		CMessageFilter* pTemplate=new CMessageFilter("<"+_l("Filter Text")+">",0,0);
		iFilterNum=objSettings.messageFilters.Add(pTemplate);
	}
	CMessageFilter* pTemplate=objSettings.messageFilters[iFilterNum];
	if(!pTemplate){
		return FALSE;
	}
	CString sFilterTitle=Format(" #%i",++iFilterCount);
	HIROW hNewItemTitle=pDialog->Option_AddBOOL(hData,_l("Message filter")+sFilterTitle,&(pTemplate->dwActive), pTemplate->dwActive, 5, OC_FILTERS+iFilterNum);
	pDialog->Option_AddString(hNewItemTitle,_l("Filter text"),&(pTemplate->sSearchString), pTemplate->sSearchString,OC_FILTERS+iFilterNum);
	pDialog->Option_AddCombo(hNewItemTitle,_l("Search where"),&(pTemplate->dwSearchWhere), pTemplate->dwSearchWhere,_l("Anywhere")+"\t"+_l("Sender nick only")+"\t"+_l("Message text only"),OC_FILTERS+iFilterNum);
	pDialog->Option_AddCombo(hNewItemTitle,_l("Do what"),&(pTemplate->dwDoWhat), pTemplate->dwDoWhat,_l("Nothing")+"\t"+_l("Delete")+"\t"+_l("Open always")+"\t"+_l("Notify"),OC_FILTERS+iFilterNum);
	pDialog->Option_AddActionEx(hNewItemTitle,_l("Delete filter"),RemoveMFilter, 0, HIROW(iFilterNum), 62, OC_FILTERS+iFilterNum);
	pDialog->m_OptionsList.Expand(hNewItemTitle);
	pDialog->Invalidate();
	return TRUE;
}

BOOL CALLBACK AddMFilter(HIROW hData, CDLG_Options* pDialog)
{
	return AddMFilterEx(hData, pDialog, -1);
}

int bGlobalBBChanges=0;
BOOL CALLBACK UnSubcrBB(HIROW hData, CDLG_Options* pDialog)
{
	if(objSettings.sFBDir=="" || !objSettings.objMessageWrapper->pFBNet){
		Alert(_l("Error: Billboard is not started yet"));
		return 0;
	}
	int i=(int)hData;
	if(i>=0 && i<objSettings.aSubsBB.GetSize()){
		DeleteFile(GetFBFolder("",objSettings.aSubsBB[i])+objSettings.objMessageWrapper->pFBNet->m_sFB_ID+"_"+FBSTATUS);
		objSettings.aSubsBB.RemoveAt(i);
		bGlobalBBChanges=1;
		pDialog->PostMessage(WM_COMMAND,IDAPPLY,0);
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK SubcrBB(HIROW hData, CDLG_Options* pDialog)
{
	if(objSettings.sFBDir=="" || !objSettings.objMessageWrapper->pFBNet){
		Alert(_l("Error: Billboard is not started yet"));
		return 0;
	}
	int iSecNum=(int)hData;
	if(iSecNum==0xFFFF && pDialog){
		// Смотрим какие есть...
		CString sDirectoryPath=GetFBFolderRoot();
		CFileInfoArray dir;
		dir.AddDir(sDirectoryPath,FBBB_PR0"*.",FALSE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTDESCENDING,TRUE);
		int iChoosen=0,iCount=0;
		CMenu mn;
		mn.CreatePopupMenu();
		CStringArray aBBs;
		for (int i=0;i<dir.GetSize();i++){
			CString sBB=dir[i].GetFileName().SpanExcluding(".");
			if(sBB.Replace(FBBB_PR0,"")==1){
				if(sBB.Find("FILEZONE")==-1){
					iCount++;
					AddMenuString(&mn,aBBs.Add(sBB)+1,TrimMessage(GetBBSectionName(sBB),30,5));
				}
			}
		}
		if(iCount){
			::SetMenuDefaultItem(mn.m_hMenu, 0, FALSE);
			CPoint pc;
			GetCursorPos(&pc);
			iChoosen=::TrackPopupMenu(mn.m_hMenu, TPM_RETURNCMD, pc.x, pc.y, 0, pDialog->GetSafeHwnd(), NULL);
		}
		if(iChoosen==0){
			if(iCount==0){
				Alert(_l("Error: There are no active sections found")+"!");
			}
			return 0;
		}
		if(iChoosen!=0){
			CString sSecName=aBBs[iChoosen-1];
			for(int i=0;i<objSettings.aSubsBB.GetSize();i++){
				if(objSettings.aSubsBB[i]==sSecName){
					AlertBox(_l("Billboard: Subscribe"),_l("Error: You are already subscribed\nto this section")+"!",8000);
					return 0;
				}
			}
			iSecNum=objSettings.aSubsBB.Add(sSecName);
		}
	}
	if(iSecNum>=0 && iSecNum<objSettings.aSubsBB.GetSize()){
		bGlobalBBChanges=1;
		objSettings.objMessageWrapper->pFBNet->WriteStatusInfo(GetFBFolder("",objSettings.aSubsBB[iSecNum]),objSettings.objMessageWrapper->pFBNet->m_sFB_ID);
		if(pDialog){
			pDialog->PostMessage(WM_COMMAND,IDAPPLY,0);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK CreateBB(HIROW hData, CDLG_Options* pDialog)
{
	if(objSettings.sFBDir=="" || !objSettings.objMessageWrapper->pFBNet){
		Alert(_l("Error: Billboard is not started yet"));
		return 0;
	}
	static long lAlread=0;
	if(lAlread){
		return 0;
	}
	SimpleTracker Lock(lAlread);
	CDlgSimpleEdit ed(pDialog);
	ed.iStyle=2;
	CString sTitDef=_l("Type section name (LAT characters only!)");
	CString sTexDef=_l("Type short section description");
	ed.m_EditTitle=sTitDef;
	ed.m_Edit=sTexDef;
	if(ed.DoModal()==IDOK){
		BOOL bLatOnly=1;
		ed.m_EditTitle.MakeUpper();
		for(int j=0;j<ed.m_EditTitle.GetLength();j++){
			if(ed.m_EditTitle.Find(sTitDef)!=-1){
				Alert(_l("You must specify valid section name")+"!");
				return FALSE;
			}
			if(ed.m_Edit.Find(sTexDef)!=-1){
				Alert(_l("You must specify valid section description")+"!");
				return FALSE;
			}
			if(!(ed.m_EditTitle[j]>='A' && ed.m_EditTitle[j]<='Z')
				&& !(ed.m_EditTitle[j]>='0' && ed.m_EditTitle[j]<='9')
				&& ed.m_EditTitle[j]!='_'){
				Alert(_l("Error: Wrong characters in Billboard section name")+"!");
				return FALSE;
			}
		}
		for(int i=0;i<objSettings.aSubsBB.GetSize();i++){
			if(objSettings.aSubsBB[i]==ed.m_EditTitle){
				Alert(_l("Error")+": "+_l("This section is already created")+"!");
				return FALSE;
			}
		}
		int iIndex=objSettings.aSubsBB.Add(ed.m_EditTitle);
		CString sFolder=GetFBFolder("",ed.m_EditTitle);
		if(sFolder==""){
			Alert(_l("Error")+": "+_l("Access denied")+"!");
			return FALSE;
		}
		CDataXMLSaver::Str2Xml(ed.m_Edit);
		CString sDsc=Format(XML_HEADER"<bbinfo><desc>%s</desc></bbinfo>",(ed.m_Edit));
		SaveFile(sFolder+FB_BBDSC_FILE,sDsc);
		Alert(_l("Created successfully")+"!");
		SubcrBB(HIROW(iIndex),0);
		pDialog->PostMessage(WM_COMMAND,IDAPPLY,0);
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK CheckBB(HIROW hData, CDLG_Options* pDialog)
{
	pDialog->SendMessage(WM_COMMAND,IDAPPLY,0);
	objSettings.objMessageWrapper->RecreateFBModule();
	if(objSettings.sFBDir=="" || objSettings.objMessageWrapper->pFBNet==NULL || objSettings.objMessageWrapper->pFBNet->bStarted==0){
		Alert(_l("Failed to start Billboard submodule\nCheck protocol settings, protocol disabled"),_l("Billboard"));
		return 0;
	}
	CDLG_Find dlg(pDialog);
	if(!dlg.bAlreadyExist){
		dlg.bSearchForPersons=TRUE;
		dlg.bLimit2FB=TRUE;
		dlg.DoModal();
	}
	return 1;
}

BOOL CALLBACK ApplyOptionsFromDialog(BOOL bSaveAndClose,DWORD dwRes,CDLG_Options* dlgOpt)
{
	BOOL bRes=FALSE;
	if(bGlobalBBChanges){
		bGlobalBBChanges=0;
		bRes=TRUE;
	}
	if(dwRes==IDOK){
		// Обрабатываем
		if(dlgOpt->isOptionClassChanged(OC_DATECOL)){//Требует полной перерисовки...
			CItem::SetModify(TRUE);
		}
		if(!IsEmbedded()){
			if(dlgOpt->isOptionClassChanged(OC_STARTUP)){//Автозапуск...
				StartupApplicationWithWindows(objSettings.bStartWithWindows);
			}
		}else{
			StartupApplicationWithWindows(FALSE);
		}
		objSettings.ApplySettings();
		if(dlgOpt->isOptionClassChanged(OC_LICDATA)){
			if(!lLikKeysPasted){
				PasteLikKeys(NULL,NULL);
			}
			CheckLikKeys(NULL,NULL);
		}
		if(dlgOpt->isOptionClassChanged(OC_MAILTIME)){
			for(int i=0;i<objSettings.antiSpamAccounts.GetSize();i++){
				if(objSettings.antiSpamAccounts[i].m_pcPOP3){
					objSettings.antiSpamAccounts[i].m_pcPOP3->SetTimeout(objSettings.dwMailboxTimeout*1000);
				}
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_STYLES)){
			// Обновляем все что можно...
			if(pMainDialogWindow){
				pMainDialogWindow->SetColorStyle();
				pMainDialogWindow->Invalidate();
				if(pMainDialogWindow->dlgAddBook){
					pMainDialogWindow->dlgAddBook->SetColorStyle();
					pMainDialogWindow->dlgAddBook->Invalidate();
				}
			}
			for(int i=0;i<objSettings.m_Notes.GetSize();i++){
				CDLG_Note* pNote=objSettings.m_Notes.GetNote(i);
				if(!pNote){
					continue;
				}
				objSettings.m_Notes.GetNote(i)->ActivateNoteParams(pNote->IsWindowVisible());
				objSettings.m_Notes.GetNote(i)->Invalidate();
			}
		}
		if(pMainDialogWindow){
			if(dlgOpt->isOptionClassChanged(OC_EMAILCNT)){
				if(objSettings.iLikStatus<2 && objSettings.lAntiSpamAccountsFromNextStart>1){
					objSettings.lAntiSpamAccountsFromNextStart=1;
					Alert(_l("In order to work with more than one POP3 account you must register WireNote!\nIn unregistered version, you can work with one POP3 account only"),_l("Limitations of unregistered version"));
				}
			}
			if(dlgOpt->isOptionClassChanged(OC_ICONMSK)){
				if(!objSettings.lTrayIconsMaskO && !objSettings.lTrayIconsMaskM){
					// По кнопке X спрашиваем закрывать ли прогу
					objSettings.lOnExitFromMainWnd=0;
					// Предупреждаем
					Alert(_l("WireNote is in the invisible mode\nYou can press")+Format(" '%s' ",getActionHotKeyDsc(OPEN_TODOLIST,""))+_l("to open main window"),_l("WireNote: Warning"));
				}
				pMainDialogWindow->SetWindowIcon();
				if(pMainDialogWindow->dlgAddBook){
					pMainDialogWindow->dlgAddBook->SetWindowIcon();
				}
			}else{// Иначе может нужно обновить отдельную иконку...
				if(dlgOpt->isOptionClassChanged(OC_ICON) || dlgOpt->isOptionClassChanged(OC_LANG)){
					pMainDialogWindow->SetWindowIcon();
				}
				if(dlgOpt->isOptionClassChanged(OC_ICONAB) && pMainDialogWindow->dlgAddBook){
					pMainDialogWindow->dlgAddBook->SetWindowIcon();
				}
			}
			if(dlgOpt->isOptionClassChanged(OC_SORT)){
				pMainDialogWindow->SortList(objSettings.iSortColumn);
			}
			if(dlgOpt->isOptionClassChanged(OC_NETONLINESTATUS)){
				objSettings.aAutoAnswersRecepients.RemoveAll();
				ExchangeOnlineStatuses();
			}
			if(dlgOpt->isOptionClassChanged(OC_FILTER)){
				pMainDialogWindow->ReattachAccordingFilter();
			}
			//pMainDialogWindow->Sizer.SetGotoRule(pMainDialogWindow->MainRulePos,objSettings.bButtonsStyle);
			pMainDialogWindow->Sizer.ApplyLayout();
			if(pMainDialogWindow->dlgAddBook){
				pMainDialogWindow->dlgAddBook->Sizer.ApplyLayout();
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_NETMS)){
			objSettings.objMessageWrapper->RecreateMLModule();
		}
		if(dlgOpt->isOptionClassChanged(OC_NETNB)){
			if(objSettings.asEnum.length!=0){
				objSettings.dwNB_Lana=objSettings.asEnum.lana[objSettings.iNB_LanaNum];
			}else{
				objSettings.dwNB_Lana=0;
			}
			objSettings.objMessageWrapper->RecreateNBModule();
		}
		if(dlgOpt->isOptionClassChanged(OC_NETXPC)){
			objSettings.objMessageWrapper->RecreateXPCModule();
		}
		if(dlgOpt->isOptionClassChanged(OC_NETFB)){
			if(pMainDialogWindow){
				pMainDialogWindow->SetDefaultsListParams(1);
			}
			objSettings.objMessageWrapper->RecreateFBModule();
			if(objSettings.objMessageWrapper->pFBNet==NULL || objSettings.objMessageWrapper->pFBNet->bStarted==0){
				Alert(_l("Failed to start Billboard submodule\nCheck protocol settings, protocol disabled"),_l("Billboard"));
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_NETEM)){
			objSettings.objMessageWrapper->RecreateEMModule();
		}
		if(dlgOpt->isOptionClassChanged(OC_LANG)){
			bAdvertLoaded=0;
			bRes=TRUE;
		}
		objSettings.m_Reminders.SetHourlyAsOptions();
		objSettings.Save();
	}
	return bRes;
}

BOOL g_bAllOptions=FALSE;
int g_iAfterOpenType=0;
LPVOID g_hAfData=0;
BOOL CALLBACK AddOptionsToDialog(CDLG_Options* pData)
{
	CDLG_Options* pDialog=pData;
	if(!pDialog){
		return FALSE;
	}
	// Устанавливаем цвета в соотвествии со стилем - без прозрачности!!!
	/*
	pDialog->m_OptionsList.m_crIItemText=objSettings.objSchemes.getTxtColor(objSettings.iMainStyle);
	pDialog->m_OptionsList.m_crIRowBackground=objSettings.objSchemes.getBgColor(objSettings.iMainStyle);
	pDialog->m_OptionsList.SetBkColor(objSettings.objSchemes.getBgColor(objSettings.iMainStyle));
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

	if(pMainDialogWindow){// Шрифт
		CFont* font=pMainDialogWindow->font;
		if(font){
			pDialog->m_OptionsList.SetFont(font,FALSE);
		}
	}
	CString sTrayModCombo=_l("None")+"\t"+_l("Shift")+"\t"+_l("Ctrl")+"\t"+_l("Shift+Ctrl")+"\t"+_l("Alt")+"\t"+_l("Shift+Alt")+"\t"+_l("Ctrl+Alt")+"\t"+_l("Shift+Ctrl")+"\t"+_l("Shift+Ctrl+Alt");
	CString sVisualClues=_l("Hide")+"\t"+_l("Strike")+"\t"+_l("Pale");
	CString sLanaEnumValues;
	if(objSettings.asEnum.length!=0){
		for(int j=0;j<objSettings.asEnum.length;j++){
			sLanaEnumValues+=Format("%i. LANA #%lu\t",j+1,objSettings.asEnum.lana[j]);
		}
		sLanaEnumValues.TrimRight("\t");
	}else{
		sLanaEnumValues="1. LANA #0";// 0 - значение по умолчанию
	}
	CString sColorStyles="";
	for(long i=0;i<objSettings.objSchemes.GetSize();i++){
		sColorStyles+=objSettings.objSchemes.getName(i);
		sColorStyles+="\t";
	}
	sColorStyles.TrimRight();
	////////////////////////
	// Задаем список опций
	////////////////////////
	// UINT
	HIROW hFeatures=FL_ROOT;
	if(!bOnlyNotesOptions){
		if(!IsEmbedded()){
			HIROW hCommon=pDialog->Option_AddHeader(FL_ROOT,	_l(DEF_COMMON)+"\t"+_l("Interface, tray icon and other common stuff"));
			{
				pDialog->Option_AddBOOL(hCommon,_l("Start with windows"),&objSettings.bStartWithWindows, 1, 0, OC_STARTUP);
				pDialog->Option_AddCombo(hCommon,_l("User interface language"),&GetApplicationLang(),GetApplicationLang(),sLangsNames,OC_LANG);
				pDialog->Option_AddCombo(hCommon,_l("Action to perform on 'X' button ('Close window')")+"\t"+_l("What to do when you press 'X' button","What to do when you press 'X' button (small button in the top-right corner of the window)"),&objSettings.lOnExitFromMainWnd, 0, _l("Ask")+"\t"+_l("Close program")+"\t"+_l("Minimize to tray"));
				pDialog->Option_AddCombo(hCommon,_l("Preferences level"),&objSettings.lPreferencesLevel, 0, _l("All options")+"\t"+_l("Organizer`s related")+"\t"+_l("Messaging`s related"));
				{// Tray-icon
					pDialog->Option_AddTitle(hCommon,	_l("Tray icon")+"\t"+_l("Icon image, icon text and default tray actions"), 63);
					HIROW hTrayIcon=hCommon;
					//pDialog->Option_AddCombo(hCommon,_l("Icons in tray"),&objSettings.lTrayIconsMask, 0, _l("Organizer")+"\t"+_l("Message center")+"\t"+_l("Both icons")+"\t"+_l("Without any icons"), OC_ICONMSK);???
					pDialog->Option_AddBOOL(hTrayIcon,_l("Show Organizer icon"),&objSettings.lTrayIconsMaskO, 1, 0, OC_ICONMSK);
					pDialog->Option_AddBOOL(hTrayIcon,_l("Show Messenger icon"),&objSettings.lTrayIconsMaskM, 0, 0, OC_ICONMSK);
					pDialog->Option_AddCombo(hTrayIcon,_l("Organizer default action"),&objSettings.DefActionMap[DEF_ACTION], 1, _l("None")+"\t"+_l("New Note")+"\t"+_l("New ToDo")+"\t"+_l("New Reminder")+"\t"+_l("New Message")+"\t"+_l("Open organizer")+"\t"+_l("Show all notes")+"\t"+_l("Show address book"));
					pDialog->Option_AddCombo(hTrayIcon,_l("Organizer icon"),&objSettings.iTrayIcon, 0, _l("Icon 1")+"\t"+_l("Icon 2")+"\t"+_l("Icon 3")+"\t"+_l("Icon 4")+"\t"+_l("Icon 5")+"\t"+_l("Icon 6")+"\t"+_l("Icon 7"), OC_ICON);
					pDialog->Option_AddCombo(hTrayIcon,_l("Organizer icon text"),&objSettings.lShowDateInTrayIcon, 1, _l("None")+"\t"+_l("Current date")+"\t"+_l("Current day of week"), OC_ICON);
					pDialog->Option_AddCombo(hTrayIcon,_l("Message center icon text"),&objSettings.lShowMsgInAddrbookIcon, 1, _l("None")+"\t"+_l("Number of unread messages")+"\t"+_l("Number of incoming messages"), OC_ICONAB);
					pDialog->Option_AddCombo(hTrayIcon,_l("Message center default action"),&objSettings.bMessCentreDefAction,0,_l("Open address book")+"\t"+_l("New Message"));
				}
				{// Alerts
					HIROW hAlerts=pDialog->Option_AddHeader(hCommon,	_l("Alerts"));
					pDialog->Option_AddBOOL(hAlerts,_l("Prevent alerts from accidental submit"),&objSettings.lRemSubmitPreventer, DEF_SUBMITPREVENTER);
					pDialog->Option_AddBOOL(hAlerts,_l("Place alerts on taskbar instead of tray"), &objSettings.lAlertsOnTaskbar, 1);
					pDialog->Option_AddBOOL(hAlerts,_l("Hide reminder alerts to tray by default"),&objSettings.bRemHideToTrayDef, 0);
					pDialog->Option_AddCombo(hAlerts,_l("Alert: text style"),&objSettings.lAlertTextStyle, 0, _l("None")+"\t"+_l("Shadow")+"\t"+_l("Shadow 2"));
					pDialog->Option_AddCombo(hAlerts,_l("Alerts")+": "+_l("Default color style"),&objSettings.iAlertStyle,objSettings.iAlertStyle,sColorStyles);
				}
				{// Разное
					HIROW hMisc=pDialog->Option_AddHeader(hCommon, _l("Miscellaneous"));
					pDialog->Option_AddCombo(hMisc,_l("Tray-click style"),&objSettings.UseBblClick, 1, _l("Single-click")+"\t"+_l("Double-click"));
					if(!IsVista()){
						pDialog->Option_AddBOOL(hMisc,sResTsc+_l("Add shadows to WireNote windows")+"\t"+_l("Restart program to see changes"),&objSettings.bShadows, 1);
					}
					//pDialog->Option_AddBOOL(hMisc,_l("Generate random titles for new items"),&objSettings.bGenerateRandomNames, 1);
					pDialog->Option_AddNString(hMisc,_l("Distance to stick windows together (pix)"),&objSettings.iStickPix, 10, 0, 30);
					pDialog->Option_AddNString(hMisc,_l("How many items show in popup submenus"),&objSettings.iMenuItemsCount, 10, 2, 1000);
					pDialog->Option_AddNString(hMisc,_l("Auto-save all data period (min)"),&objSettings.dwAutoSaveTime, 30, 10, 1200);
					HIROW hUpdates=pDialog->Option_AddBOOL(hMisc,_l("Check for updates every ... day(s)"),&objSettings.lCheckForUpdate, 1);
					pDialog->Option_AddNString(hUpdates,"",&objSettings.lDaysBetweenChecks, 20, 0, 300);
					{// Trash
						HIROW hTrash=pDialog->Option_AddBOOL(hMisc,_l("Delete items to trash folder"),&objSettings.bDelToTrash, 1,5);
						pDialog->Option_AddNString(hTrash,_l("Maximum trash folder size (Kb)"),&objSettings.bMaxTrashSize, MIN_TRASH_SIZE, MIN_TRASH_SIZE, MIN_TRASH_SIZE*100);
						pDialog->Option_AddString(hTrash,_l("Trash folder"),&objSettings.sTrashDirectory, getFileFullName(DEFAULT_TRASHFOLDER,TRUE));
						pDialog->Option_AddAction(hTrash,_l("Open trash folder"),CallTFolder,NULL,0);
					}
					pDialog->Option_AddAction(hMisc,_l("Open folder with all saved settings, notes, ToDos, etc."),CallSetFolder,NULL,0);
				}
				pDialog->Option_AddAction(hCommon,_l("Edit fonts and colors"),CallStylesDialog,NULL,OC_STYLES);
			}
		}
		hFeatures=0;
		if(g_bAllOptions || objSettings.lPreferencesLevel==0 || objSettings.lPreferencesLevel==1){
			hFeatures=pDialog->Option_AddHeader(FL_ROOT, _l(MAINFEAT_TOPIC)+"\t"+_l("Customizations for ToDo, ToDo list, Note, Reminder, Address book"));
			if(g_iAfterOpenType==1){
				HIROW hNoteNAH=pDialog->Option_AddHeader(hFeatures,	_l(DEF_NAH));
				CIHotkey* lpH=(CIHotkey*)g_hAfData;
				pDialog->Option_AddHotKey(hNoteNAH,_l("Activation hotkey"),lpH,*lpH);
			}
			{// ToDo
				HIROW hToDo=pDialog->Option_AddHeader(hFeatures,	_l(TODOSETS_TOPIC));
				pDialog->Option_AddHotKey(hToDo,_l("New ToDo: Hotkey"),&objSettings.DefActionMapHK[NEW_TODO],DefActionMapHK_Preinit[NEW_TODO]);
				pDialog->Option_AddBOOL(hToDo,_l("New ToDo: Activate immediately after creation"),&objSettings.bMakeNewTDCur, 0);
				//pDialog->Option_AddBOOL(hToDo,_l("New ToDo: Do not open separate dialog when created from main window"),&objSettings.bAutoCreateTodoFromMW, 1);
				pDialog->Option_AddBOOL(hToDo,_l("Activate ToDo even if it is done already"),&objSettings.bActivateDoneToDo, 1);
				pDialog->Option_AddBOOL(hToDo,_l("Show active ToDos in tray`s tooltip"),&objSettings.bShowActiveTDInTray, 1);
				pDialog->Option_AddBOOL(hToDo,_l("Word-wrap ToDo content by default"),&objSettings.bToDoWordWrap, 1);
				pDialog->Option_AddCombo(hToDo,_l("ToDo")+": "+_l("Default color style"),&objSettings.iStartTaskNoteScheme,objSettings.iStartTaskNoteScheme,sColorStyles);
				pDialog->Option_AddCombo(hToDo,_l("How to log ToDo history"),&objSettings.bTraceToDoHist,1,_l("Do not log anything")+"\t"+_l("Summary")+"\t"+_l("Detailed"));
				pDialog->Option_AddString(hToDo,_l("New ToDo: title pattern"),&objSettings.sNewToDo_TitleM, DEFNewToDo_TitleM);
				pDialog->Option_AddString(hToDo,_l("New ToDo: body pattern"),&objSettings.sNewToDo_BodyM, DEFNewToDo_BodyM);
				pDialog->Option_AddCombo(hToDo,_l("New ToDo: Modifier for tray-click"),&objSettings.DefActionMap[NEW_TODO], 1, sTrayModCombo);
				pDialog->Option_AddAction(hToDo,_l("Set fonts and colors"),CallStylesDialog,NULL,OC_STYLES);
			}
		}
	}
	HIROW hNote=0;
	{// Note
		hNote=pDialog->Option_AddHeader(hFeatures,	_l(NOTESETS_TOPIC));
		if(!IsEmbedded()){
			pDialog->Option_AddHotKey(hNote,_l("New note: Hotkey"),&objSettings.DefActionMapHK[NEW_NOTE],DefActionMapHK_Preinit[NEW_NOTE]);
			pDialog->Option_AddHotKey(hNote,_l("Show all notes: Hotkey"),&objSettings.DefActionMapHK[SHOW_ALLNOTES],DefActionMapHK_Preinit[SHOW_ALLNOTES]);
			pDialog->Option_AddHotKey(hNote,_l("Hide all notes: Hotkey"),&objSettings.DefActionMapHK[HIDE_ALLNOTES],DefActionMapHK_Preinit[HIDE_ALLNOTES]);
			pDialog->Option_AddHotKey(hNote,_l("Cycle through notes"),&objSettings.DefActionMapHK[NEXT_NOTE],DefActionMapHK_Preinit[NEXT_NOTE]);
		}
		pDialog->Option_AddBOOL(hNote,_l("Dynamic transparency on semi-transparent notes"),&objSettings.bAutoDisp, 1);
		pDialog->Option_AddBOOL(hNote,_l("New note: Word wrap by default"),&objSettings.bNoteWordWrap, DEFAULT_WORDWRAP);
		pDialog->Option_AddBOOL(hNote,_l("New note: Topmost by default"),&objSettings.iNoteOnTopByDef, 1);
		pDialog->Option_AddBOOL(hNote,_l("Hide note on ESC key"),&objSettings.iEnableAutoHideWithEsc, 0);
		pDialog->Option_AddBOOL(hNote,_l("Hide note when ALT key is down"),&objSettings.iEnableAutoHideWithAlt, 0);
		pDialog->Option_AddBOOL(hNote,_l("Popup all notes with WireNote activation"),&objSettings.bPopupNotesOnActivation, 0);
		pDialog->Option_AddBOOL(hNote,_l("Adjust note positions on resolution change"),&objSettings.bAdjustNotesPosOnScr, 1);
		pDialog->Option_AddBOOL(hNote,_l("New note: Place 'Delete' button as usual"),&objSettings.bNoteDelButtonPos,DEFAULT_DELBUTTONP);
		pDialog->Option_AddBOOL(hNote,_l("New note: Place other buttons in title"),&objSettings.bNoteOptButtonPos,DEFAULT_OTHBUTTONP);
		pDialog->Option_AddBOOL(hNote,_l("Do not show 'auto-hide' warning"),&objSettings.bNoteAHideMode,0);
		pDialog->Option_AddBOOL(hNote,_l("Do not show 'click-through' warning"),&objSettings.bNoteCTMode,0);
		pDialog->Option_AddBOOL(hNote,sResTsc+_l("Show note title in icon mode")+" ("+_l("need restart")+")",&objSettings.iSHowTitleInIconMode, 1);			
		pDialog->Option_AddBOOL(hNote,_l("Show note text in icon mode"),&objSettings.lShowTextInIcon, 1);
		pDialog->Option_AddNString(hNote,_l("Note size in icon mode (pixels)"),&objSettings.lNOTEICON_DIM, 2*GetSystemMetrics(SM_CYMENU)+2, 20, 100);
		pDialog->Option_AddCombo(hNote,_l("Note")+": "+_l("Default color style"),&objSettings.iStartNoteScheme,objSettings.iStartNoteScheme,sColorStyles);
		pDialog->Option_AddCombo(hNote,_l("New note: View by default"),&objSettings.bNoteDefaultView, DEFAULT_NOTEVIEW, _l("Minimal")+"\t"+_l("Buttons"));
		pDialog->Option_AddCombo(hNote,_l("Delete note: Animation"),&objSettings.iAnimOnNoteDel, DEFANIM_NOTEDEL, _l("No animation")+"\t"+_l("Fade effect")+"\t"+_l("Bubble effect"));
		pDialog->Option_AddCombo(hNote,_l("Delete note confirmation"),&objSettings.bAskDlg_DelNote, 0, _l("Ask for confirmation")+"\t"+_l("Delete note immediately"));
		pDialog->Option_AddString(hNote,_l("New note: title pattern"),&objSettings.sNewNote_TitleM, DEFNewNote_TitleM);
		pDialog->Option_AddString(hNote,_l("New note: body pattern"),&objSettings.sNewNote_BodyM, DEFNewNote_BodyM);
		pDialog->Option_AddCombo(hNote,_l("What to do when clicking 'Delete' button"),&objSettings.bNoteDelButtonWay,1,_l("Hide note")+"\t"+_l("Delete note"));
		pDialog->Option_AddCombo(hNote,_l("Sticky note: hide behaviour on application switching"),&objSettings.bCheckPinnedNoteForRect, 1, _l("Always hide")+"\t"+_l("Interfering intersection"));
		if(IsEmbedded()){
			// Упрощенное - для встроенного режима
			pDialog->Option_AddString(hNote,_l("Your email")+"\t"+_l("This email will be used as 'Sender' of your messages"),&objSettings.sEMmail, objSettings.sEMmail);
		}
		pDialog->Option_AddAction(hNote,_l("Set fonts and colors"),CallStylesDialog,NULL,OC_STYLES);
		if(!IsEmbedded()){
			pDialog->Option_AddCombo(hNote,_l("New note: Modifier for tray-click"),&objSettings.DefActionMap[NEW_NOTE], 0, sTrayModCombo);
			pDialog->Option_AddCombo(hNote,_l("Show all notes: Modifier for tray-click"),&objSettings.DefActionMap[SHOW_ALLNOTES], 0, sTrayModCombo);
		}
	}
	if(!bOnlyNotesOptions){
		{// ToDo list
			HIROW hTDList=pDialog->Option_AddHeader(hFeatures, _l("Organizer"));
			pDialog->Option_AddHotKey(hTDList,_l("Open organizer: Hotkey"),&objSettings.DefActionMapHK[OPEN_TODOLIST],DefActionMapHK_Preinit[OPEN_TODOLIST]);
			pDialog->Option_AddBOOL(hTDList,_l("Auto-popup organizer")+"\t"+_l("Window will be automatically activated as you move your mouse  over icon"),&objSettings.APToDoList, 0);
			//pDialog->Option_AddBOOL(hTDList,_l("Hide button panel"),&objSettings.bButtonsStyle, 0);
			pDialog->Option_AddBOOL(hTDList,_l("Hide ToDo folder"),&objSettings.bNotShowInListTodo, 0);
			pDialog->Option_AddBOOL(hTDList,_l("Hide Notes folder"),&objSettings.bNotShowInListNotes, 0);
			pDialog->Option_AddBOOL(hTDList,_l("Hide Reminders folder"),&objSettings.bNotShowInListRem, 0);
			pDialog->Option_AddBOOL(hTDList,_l("Hide Messages folder"),&objSettings.bHideMsgFld, 0);
			pDialog->Option_AddBOOL(hTDList,sResTsc+_l("Hide 'Date' column")+" ("+_l("need restart")+")",&objSettings.bHideDateFld, 0, 0, OC_DATECOL);
			pDialog->Option_AddBOOL(hTDList,sResTsc+_l("Use double-click to activate item under cursor")+"\t"+_l("By default, double click will rename item under cursor. To change this behaviour, set this checkbox. Warning: you have to restart WireNote in order to activate this feature!"),&objSettings.bUseDblClickToStart, 0);
			if(objSettings.iSortColumn<0){
				objSettings.iSortColumn=-objSettings.iSortColumn;
			}
			pDialog->Option_AddCombo(hTDList,sResTsc+_l("Main window")+": "+_l("Default color style")+". "+_l("Restart program to see changes"),&objSettings.iMainStyle,objSettings.iMainStyle,sColorStyles);
			pDialog->Option_AddCombo(hTDList,_l("Sort item in main window by"),&objSettings.iSortColumn,0,_l("None")+"\t"+_l("Priority")+"\t"+_l("Title")+"\t"+_l("Creation date"),OC_SORT);
			pDialog->Option_AddCombo(hTDList,_l("How to emphasize done ToDo"),&objSettings.bShowDone,1,sVisualClues,OC_FILTER);
			pDialog->Option_AddCombo(hTDList,_l("How to emphasize inactive Reminder"),&objSettings.bShowNonActiveReminders,1,sVisualClues,OC_FILTER);
			pDialog->Option_AddCombo(hTDList,_l("Open organizer: Modifier for tray-click"),&objSettings.DefActionMap[OPEN_TODOLIST], 0, sTrayModCombo);
			pDialog->Option_AddAction(hTDList,_l("Set fonts and colors"),CallStylesDialog,NULL,OC_STYLES);
		}
		{// Address book
			HIROW hABook=pDialog->Option_AddHeader(hFeatures,_l("Address book"));
			pDialog->Option_AddHotKey(hABook,_l("Show address book: Hotkey"),&objSettings.DefActionMapHK[OPEN_ADDRBOOK],DefActionMapHK_Preinit[OPEN_ADDRBOOK]);
			pDialog->Option_AddHotKey(hABook,_l("Show last unread message: Hotkey"),&objSettings.DefActionMapHK[OPEN_LASTUNREAD],DefActionMapHK_Preinit[OPEN_LASTUNREAD]);
			pDialog->Option_AddBOOL(hABook,_l("Auto-popup address book window")+"\t"+_l("Window will be automatically activated as you move your mouse  over icon"),&objSettings.bAutoPopupAdrBook, 0);
			//pDialog->Option_AddBOOL(hABook,_l("Count unread messages in address book window"),&objSettings.lShowUnreadInAddrb, 1);
			pDialog->Option_AddNString(hABook,_l("Update online status every ... min"),&objSettings.iStatusUpdateTime, DEFAULT_UPDSATMIN,0,9999);
			pDialog->Option_AddBOOL(hABook,_l("Set Away status when screen saver is running"),&objSettings.bSetAwayIfScrSaver, 1);
			pDialog->Option_AddCombo(hABook,_l("Show address book: Modifier for tray-click"),&objSettings.DefActionMap[OPEN_ADDRBOOK], 0, sTrayModCombo);
			pDialog->Option_AddAction(hABook,_l("Set fonts and colors"),CallStylesDialog,NULL,OC_STYLES);
			pDialog->Option_AddAction(hABook,_l("Import address book"),CallImportAB,NULL,0);
			pDialog->Option_AddAction(hABook,_l("Export address book"),CallExportAB,NULL,0);
		}
		{// Remd
			HIROW hRemd=pDialog->Option_AddHeader(hFeatures,	_l(REMDSETS_TOPIC));
			pDialog->Option_AddHotKey(hRemd,_l("New reminder: Hotkey"),&objSettings.DefActionMapHK[NEW_REMD],DefActionMapHK_Preinit[NEW_REMD]);
			pDialog->Option_AddString(hRemd,_l("New reminder: title pattern"),&objSettings.sNewRemd_TitleM, DEFNewRemd_TitleM);
			pDialog->Option_AddString(hRemd,_l("New reminder: body pattern"),&objSettings.sNewRemd_BodyM, DEFNewRemd_BodyM);
			pDialog->Option_AddCombo(hRemd,_l("New reminder: Modifier for tray-click"),&objSettings.DefActionMap[NEW_REMD], 2, sTrayModCombo);
			pDialog->Option_AddCombo(hRemd,_l("Reminder alert")+": "+_l("Default color style"),&objSettings.iRemdStyle,objSettings.iRemdStyle,sColorStyles);
			pDialog->Option_AddBOOL(hRemd,_l("Delete reminder when used"),&objSettings.bRemDelStrictDef, 0);
			pDialog->Option_AddBOOL(hRemd,_l("Allow reminder to wake up computer from Sleep state"),&objSettings.lRemindFromSleep, 0);
			{// Stricking Clock
				HIROW hStrickClock=pDialog->Option_AddBOOL(hRemd,_l("Enable striking clock"),&objSettings.dwStrickingClockActive, 0, 5);
				HIROW hSoundsOnStrClockWav=pDialog->Option_AddString(hStrickClock,_l("Wav file"),&objSettings.sHourlyWav, DEFAULT_HOURLY);
				pDialog->Option_AddActionToEdit(hSoundsOnStrClockWav,ChooseWavFile);
				pDialog->Option_AddAction(hStrickClock,_l("Test sound"),TestSound, hSoundsOnStrClockWav);
				pDialog->Option_AddBOOL(hStrickClock,_l("Strike by hours count"),&objSettings.bHourlyByHCount, 0);
			}
			pDialog->Option_AddAction(hRemd,_l("Set fonts and colors"),CallStylesDialog,NULL,OC_STYLES);
		}
		if(g_bAllOptions || objSettings.lPreferencesLevel==0 || objSettings.lPreferencesLevel==2){
			HIROW hProtocolsRaw=pDialog->Option_AddHeader(FL_ROOT,_l(NETSETS_TOPIC)+"\t"+_l("Common network options, protocols preferences"));
			{
				HIROW hFeatures3=pDialog->Option_AddHeader(hProtocolsRaw?hProtocolsRaw:FL_ROOT,_l(MESSAGING_TOPIC)+"\t"+_l("Incoming/Outcoming messages and network options"));
				{// MSGE
					if(!objSettings.bNoSend){
						HIROW hMessages=hFeatures3;
						pDialog->Option_AddTitle(hFeatures3,_l("Specific to New messages")/*+"\t"+_l("Customizations for New messages")*/,63);
						pDialog->Option_AddHotKey(hMessages,_l("New message hotkey"),&objSettings.DefActionMapHK[NEW_MSG],DefActionMapHK_Preinit[NEW_MSG]);
						pDialog->Option_AddBOOL(hMessages,_l("New message: auto-set most recent recipient by default"),&objSettings.lRecentUserByDef, 1);
						pDialog->Option_AddNString(hMessages,_l("Recent recipients list size"),&objSettings.lMsgRecentListSize, 10,3,50);
						pDialog->Option_AddString(hMessages,_l("New message: title pattern"),&objSettings.sNewMess_TitleM, DEFNewMsg_TitleM);
						pDialog->Option_AddString(hMessages,_l("New message: body pattern"),&objSettings.sNewMess_BodyM, DEFNewMsg_BodyM);
						pDialog->Option_AddCombo(hMessages,_l("New message")+": "+_l("Default color style"),&objSettings.iStartMessageScheme,objSettings.iStartMessageScheme,sColorStyles);
						pDialog->Option_AddCombo(hMessages,_l("New message: Modifier for tray-click"),&objSettings.DefActionMap[NEW_MSG], 4, sTrayModCombo);
					}
					{
						HIROW hInMessages=hFeatures3;
						pDialog->Option_AddTitle(hFeatures3,_l("Specific to Incoming messages")/*+"\t"+_l("Customizations for Incoming messages")*/,63);
						pDialog->Option_AddCombo(hInMessages,_l("Incoming message")+"\t"+_l("How to represent incoming message"),&objSettings.bOnNewMessage, DEFAULT_ONMESSAGE, _l("Blink in tray")+"\t"+_l("Put in tray as icon")+"\t"+_l("Open note")+"\t"+_l("Open iconic note"));
						pDialog->Option_AddBOOL(hInMessages,_l("Popup baloon tooltip on incoming event"),&objSettings.lUseBloonOnNewMsg, DEFAULT_BALOONTIP);
						//pDialog->Option_AddBOOL(hInMessages,_l("Auto-quote icon messages"),&objSettings.bAutoQuote, 0);
						pDialog->Option_AddCombo(hInMessages,_l("Animate incoming message"),&objSettings.iAnimOnNewMsg, DEFANIM_MSGNEW, _l("No animation")+"\t"+_l("Fade effect")+"\t"+_l("Slide effect"));
						//pDialog->Option_AddBOOL(hInMessages,_l("Use one note for one sender")+"\t"+_l("Using this option it is possible to create chat-like conversations, when answers will appear in the same note/message"),&objSettings.bGroupBySender, 0);
						pDialog->Option_AddCombo(hInMessages,_l("Duplicated messages"),&objSettings.bOnDuplMessage,2,_l("Warn")+"\t"+_l("Dismiss")+"\t"+_l("Open"));
						//pDialog->Option_AddString(hInMessages,_l("Incoming message: title pattern"),&objSettings.sNewMess_TitleMFROM, DEFNewMsg_TitleMFRM);
						pDialog->Option_AddCombo(hInMessages,_l("If message has attached ToDo"),&objSettings.bReceiveToDosAction,0,_l("Ask what to do with message")+"\t"+_l("Do nothing, open message")+"\t"+_l("Accept ToDos from known senders only")+"\t"+_l("Accept ToDos from any sender"));
						pDialog->Option_AddCombo(hInMessages,_l("If message has attached Reminder"),&objSettings.bReceiveRemdsAction,0,_l("Ask what to do with message")+"\t"+_l("Do nothing, open message")+"\t"+_l("Accept reminders from known senders only")+"\t"+_l("Accept reminders from any sender"));
						pDialog->Option_AddBOOL(hInMessages,_l("Check messages for fraud (real sender name)")+"\t"+_l("When checked, WireNote will search for the real sender of each incoming message and will notify you if the real sender is different"),&objSettings.bCheckFraudMsg, DEFAULT_CHKFRAUD);
					}
					pDialog->Option_AddTitle(hFeatures3,_l("Common options"),63);
					pDialog->Option_AddBOOL(hFeatures3,_l("Delete message with ESC key"),&objSettings.bEscDelIncomingMsg, 1);
					pDialog->Option_AddBOOL(hFeatures3,_l("Word-wrap message text"),&objSettings.bWWIncomingMsg, 1);
					pDialog->Option_AddNString(hFeatures3,_l("'Quote message': How many characters leave from the current text"),&objSettings.sQuotationLen, 150,0,9999);
					pDialog->Option_AddBOOL(hFeatures3,_l("Save message attachments with note"),&objSettings.bSaveNoteAttachments, 0);
					if(!objSettings.bNoSend){
						HIROW hOutMessages=hFeatures3;//pDialog->Option_AddHeader(hFeatures3,_l("Specific to Outcoming messages")+"\t"+_l("Customizations for Outcoming messages"));
						//pDialog->Option_AddBOOL(hOutMessages,_l("Auto-close alert if message is not delivered"),&objSettings.bDelMsgAlertIfNotDeliv, 1);
						pDialog->Option_AddBOOL(hFeatures3,_l("Show sending progress in separate window"),&objSettings.bShowMsgProgress, 0);
						//pDialog->Option_AddBOOL(hOutMessages,_l("Minimize 'Sending' alert to tray"),&objSettings.bMinConfToTray, 1);
						pDialog->Option_AddBOOL(hOutMessages,_l("Automatically delete delivered message"),&objSettings.bDelMsgIfDelivered, 0);
						//pDialog->Option_AddBOOL(hOutMessages,_l("Recipients: Use nicknames instead of computer names"),&objSettings.bUseNickOnSend, 1);
						pDialog->Option_AddCombo(hOutMessages,_l("Automatically resend undelivered message after a while"),&objSettings.bAskToResendMsg, 0, _l("Ask")+"\t"+_l("Resend")+"\t"+_l("Do not resend"));
					}
					{//Auto answer
						HIROW hAAnswer=pDialog->Option_AddBOOL(hFeatures3,_l("Auto answer on incoming messages")+"\t"+_l("Click twice to define 'auto answer' strings"),&objSettings.bAutoAnswer, 0, 5);
						pDialog->Option_AddBOOL(hAAnswer,_l("Auto answer even to unknown senders"),&objSettings.bARForUnknownUsers, 0);
						pDialog->Option_AddString(hAAnswer,_l("Auto answer when my status is online"),&objSettings.sAutoReplys[0], DEFAULT_AASTATUS0);
						pDialog->Option_AddString(hAAnswer,_l("Auto answer when my status is away"),&objSettings.sAutoReplys[1], DEFAULT_AASTATUS1);
						pDialog->Option_AddString(hAAnswer,_l("Auto answer when my status is busy"),&objSettings.sAutoReplys[2], DEFAULT_AASTATUS2);
						pDialog->Option_AddString(hAAnswer,_l("Auto answer when my status is DND"),&objSettings.sAutoReplys[3], DEFAULT_AASTATUS3);
					}
					HIROW hLog=pDialog->Option_AddBOOL(hFeatures3,_l("Message history")+"\t"+_l("Save all incoming events to log file"),&objSettings.SaveMesNotesByDef, 1, 5);
					pDialog->Option_AddBOOL(hLog,_l("Log history per user"),&objSettings.SaveMesNotesByDefForUser, 1);
					pDialog->Option_AddNString(hLog,_l("Maximum log file size (Kb)"),&objSettings.dwMaxLogFileSize, DEFAULT_MAXLOGKB,3,10000);
					//pDialog->Option_AddString(hLog,_l("Log directory"),&objSettings.sMessageLogDirectory, getFileFullName(DEFAULT_MSGLOG,TRUE));
					pDialog->Option_AddAction(hFeatures3,_l("Set fonts and colors"),CallStylesDialog,NULL,OC_STYLES);
					//HIROW hMessagesCommon=pDialog->Option_AddHeader(hFeatures3,_l("Message options")+"\t"+_l("Common customizations"));
				}
				HIROW hMessagesFilter=pDialog->Option_AddHeader(hFeatures3,_l(MESFILTER_TITLE)+"\t"+_l("Place to create new filter or edit and remove old one"));
				hFilters=pDialog->GetAliasHIROW(hMessagesFilter);
				pDialog->Option_AddBOOL(hMessagesFilter,_l("Notify about filtered messages"),&objSettings.lNotifyAboutFMes, 1);
				pDialog->Option_AddAction(hMessagesFilter,_l("Add message filter"), AddMFilter, hMessagesFilter);
				iFilterCount=0;
				for(int i=0;i<objSettings.messageFilters.GetSize();i++){
					AddMFilterEx(hMessagesFilter,pDialog,i);
				}
				{
					HIROW hNot=pDialog->Option_AddHeader(hFeatures3,_l("Delivery notification")+"\t"+_l("How to handle notifications requests, etc"));
					pDialog->Option_AddTitle(hNot,_l("important: Notifications will affect Xpc, Netbios and Filebox messages only"),63);
					pDialog->Option_AddBOOL(hNot,_l("Disable all notifications and additional network functionality")+"\t"+_l("Disable all extensions that are not supported by previous versions of WireNote and third-party LAN applications"),&objSettings.lDisableAttachExtens, 1, 0, 0);
					//pDialog->Option_AddBOOL(hNot,_l("Send delivery notification when requested"),&objSettings.lFBNotifyOfRead, 1, 0, 0);
					pDialog->Option_AddBOOL(hNot,_l("Request 'message read' notification"),&objSettings.lReqNotifyOfRead, 0, 0, 0);
					pDialog->Option_AddBOOL(hNot,_l("Request 'message delivered' notification"),&objSettings.lReqNotifyOfDeliv, 0, 0, 0);
					pDialog->Option_AddCombo(hNot,_l("Receive notification"),&objSettings.lShowNotInPopup,1,_l("Do not receive")+"\t"+_l("Receive as popup")+"\t"+_l("Receive as separate message"));
					pDialog->Option_AddBOOL(hNot,_l("Block immediate notifications (shorter than 5 minutes from send)"),&objSettings.lShowNotAfterTime, 0, 0, 0);
				}
			}
			{
				HIROW hProtocolsRaw2=pDialog->Option_AddHeader(hProtocolsRaw,	_l(DEF_ACTIVEPROTS));
				pDialog->Option_AddCombo(hProtocolsRaw2,_l("Current online status"),&objSettings.iOnlineStatus,0,_l("Online")+"\t"+_l("Away")+"\t"+_l("Busy")+"\t"+_l("DND"), OC_NETONLINESTATUS);
				// Общие настройки 
				pDialog->Option_AddCombo(hProtocolsRaw2,_l("Default net protocol"),&objSettings.iProtocol_Preffered,DEFAULT_NETPROT,_l("Mailslot")+"\t"+_l("WinPopup")+"\t"+_l("WireNote")+"\t"+_l("Billboard")+"\t"+_l("Email"));
				pDialog->Option_AddNString(hProtocolsRaw2,_l("Timeout for net operations (milliseconds)"),&objSettings.dwNetMessagingTimeout, DEFAULT_NETTO,3,100000);
				if(!objSettings.bUnderWindows98){
					pDialog->Option_AddCombo(hProtocolsRaw2,_l("Messenger service detection"),&objSettings.bCheckForDefMessenger,0,_l("Ask")+"\t"+_l("Stop service")+"\t"+_l("Leave service"));
				}
				pDialog->Option_AddAction(hProtocolsRaw2,_l("Open help page"),ShowProtHelp, NULL);
				HIROW hSupML=pDialog->Option_AddBOOL(hProtocolsRaw2,_l("Maintain Mailslot protocol ('/ms')"),&objSettings.bProtocolML_Active, 1, 5,OC_NETMS);
				pDialog->Option_AddString(hSupML,_l("Mailslot name"),&objSettings.sProtocolML_Slot, DEFAULT_MSLOT, OC_NETMS);
				HIROW hSupNB=pDialog->Option_AddBOOL(hProtocolsRaw2,_l("Maintain WinPopup protocol ('/nb')"),&objSettings.bProtocolNB_Active, 1, 5,OC_NETNB);
				pDialog->Option_AddBOOL(hSupNB,_l("Receive messages to local user"),&objSettings.lNB_SupportLocUser, 1, 0, OC_NETNB);
				HIROW hLocComp=pDialog->Option_AddBOOL(hSupNB,_l("Use non-usual computer alias"),&objSettings.lNB_SupportNonStandAlias, 0, 0, OC_NETNB);
				pDialog->Option_AddString(hLocComp,"",&objSettings.sProtocolNB_Alias, objSettings.sLocalComputerName,OC_NETNB);
				pDialog->Option_AddCombo(hSupNB,_l("Network LANA")+"\t"+_l("'0' is the most common value"),&objSettings.iNB_LanaNum,0,sLanaEnumValues,OC_NETNB);
				HIROW hSupXPC=pDialog->Option_AddBOOL(hProtocolsRaw2,_l("Maintain WireNote protocol ('/xpc')"),&objSettings.bProtocolXPC_Active, 1, 5, OC_NETXPC);
				pDialog->Option_AddNString(hSupXPC,_l("Tcp-ip port"),&objSettings.dwProtocolXPC_Port, XPC_PORTDEF,0, 9999, OC_NETXPC);
				pDialog->Option_AddNString(hSupXPC,_l("Maximum attachment size (Kb)"),&objSettings.dwMaxAttachSizeKB, DEFAULT_MAXATTSIZE,0,9999);

				HIROW hSupRSS=hProtocolsRaw2;
				pDialog->Option_AddTitle(hProtocolsRaw2,_l("Rss options"),63);//bProtocolRS_Active
				pDialog->Option_AddNString(hSupRSS,_l("Maximum number of RSS news for one checking"),&objSettings.dwProtocolRSSMAxNews, 10,1, 9999);
				pDialog->Option_AddCombo(hSupRSS,_l("How to open RSS messages"),&objSettings.dwProtocolRSSOType, 2, _l("As incoming message")+"\t"+_l("Put in tray as icon")+"\t"+_l("Open note")+"\t"+_l("Open iconic note"));
				pDialog->Option_AddBOOL(hSupRSS,_l("Do not check news when computer is locked or screensaver is active"),&objSettings.lRSSLocUser, 1, 0, OC_NETNB);
				pDialog->Option_AddBOOL(hSupRSS,_l("Show feed as digect, do not open each item in separate note"),&objSettings.lRSSSummar, 0, 0, OC_NETNB);


				{// Billboard
					HIROW hBBoard=pDialog->Option_AddHeader(hProtocolsRaw2,	_l(BBRDSETS_TOPIC));
					HIROW hFDShare=pDialog->Option_AddString(hBBoard,_l("Shared folder to host billboard data")+"\t"+_l("Leave blank to disable Billboard support")+". "+_l("Note: you must have read/write access to this folder"),&objSettings.sFBDir, objSettings.sFBDir, OC_NETFB);
					pDialog->Option_AddActionToEdit(hFDShare, ChooseDir);
					pDialog->Option_AddAction(hBBoard,_l("Check billboard and view connected users"),CheckBB,NULL,OC_STYLES);
					pDialog->Option_AddTitle(hBBoard,_l("Common settings, used for all billboards"),63);
					pDialog->Option_AddString(hBBoard,_l("Billboard nickname"),&objSettings.sFBNick, objSettings.sLocalComputerName, OC_NETFB);
					pDialog->Option_AddString(hBBoard,sResTsc+_l("Your unique billboard ID")+"\t"+_l("Your unique ID between all billboard users. In most cases you don`t have to change it. Effective after restart only"),&objSettings.sFB_ID, objSettings.sFB_ID, OC_NETFB);
					pDialog->Option_AddNString(hBBoard,_l("Check billboard for new items every")+"... "+_l("seconds"),&objSettings.dwMidFBPeriod, 100, 5,100000);
					pDialog->Option_AddBOOL(hBBoard,_l("Check for new Notes"),&objSettings.bOnSharedItemN, 1, 0, 0);
					pDialog->Option_AddBOOL(hBBoard,_l("Check for new ToDos"),&objSettings.bOnSharedItemT, 1, 0, 0);
					pDialog->Option_AddBOOL(hBBoard,_l("Check for new Reminders"),&objSettings.bOnSharedItemR, 1, 0, 0);
					pDialog->Option_AddBOOL(hBBoard,_l("Check for new Private messages"),&objSettings.bOnSharedItemM, 1, 0, 0);
					pDialog->Option_AddCombo(hBBoard,_l("When new item arrive"),&objSettings.bOnSharedItem,1,_l("Do nothing")+"\t"+_l("Show notification in tray"));//+"\t"+_l("Activate received item")
					pDialog->Option_AddNString(hBBoard,_l("After sending private messages")+": "+_l("Check billboard every")+"... "+_l("seconds")+" "+_l("for awhile"),&objSettings.dwMinFBPeriod, 7, 5,100000);
					pDialog->Option_AddNString(hBBoard,_l("Remove items that are older than")+"... "+_l("days"),&objSettings.lFBIAliveTime, 90, 90,100000);
					pDialog->Option_AddBOOL(hBBoard,_l("Crypt your data in the shared folder")+"\t"+_l("Check this option to hide your messages/items from persons outside of WireNote"),&objSettings.lFBCrypt, 1, 0, 0);
					pDialog->Option_AddBOOL(hBBoard,_l("Do not notify about successfull publishing"),&objSettings.bSWarning_ShareOK, 0);
					if(objSettings.aSubsBB.GetSize()>0){
						pDialog->Option_AddTitle(hBBoard,_l("You are subscribed to the following sections"),63);
						for(int i=0;i<objSettings.aSubsBB.GetSize();i++){
							pDialog->Option_AddAction(hBBoard,TrimMessage(GetBBSectionName(objSettings.aSubsBB[i]),60,1)+": "+_l("Unsubscribe"),UnSubcrBB,HIROW(i),OC_STYLES);
						}
					}else{
						pDialog->Option_AddTitle(hBBoard,_l("Billboard sections information"),63);
					}
					pDialog->Option_AddAction(hBBoard,_l("Subscribe to new section"),SubcrBB,HIROW(0xFFFF),OC_STYLES);
					pDialog->Option_AddAction(hBBoard,_l("Create new section"),CreateBB,NULL,OC_STYLES);
				}
				{
					HIROW hSupEM=pDialog->Option_AddHeader(hProtocolsRaw2,_l("Email"));
					pDialog->Option_AddBOOL(hSupEM,_l("Allow to send messages via Email protocol ('/em')"),&objSettings.bProtocolEM_Active, 1, 0, OC_NETEM);
					HIROW hEmEmail=pDialog->Option_AddString(hSupEM,_l("Your email")+"\t"+_l("This email will be used as 'Sender' of your messages"),&objSettings.sEMmail, objSettings.sEMmail);
					pDialog->Option_AddTitle(hSupEM,_l("Select mailbox to watch new messages for you")+". "+_l("Leave 'Login' field empty to disable this feature"),63);
					pDialog->Option_AddString(hSupEM,_l("POP3 Server"),&objSettings.sEM_p3host, objSettings.sEM_p3host);
					pDialog->Option_AddNString(hSupEM,_l("POP3 port"),&objSettings.lEM_p3port, objSettings.lEM_p3port, 0, 65535);
					pDialog->Option_AddString(hSupEM,_l("Login"),&objSettings.sEM_p3login, objSettings.sEM_p3login);
					pDialog->Option_AddPassword(hSupEM,_l("Password"),&objSettings.sEM_p3psw, objSettings.sEM_p3psw);
					pDialog->Option_AddNString(hSupEM,_l("Check new messages every ... minutes"),&objSettings.lEMPeriod, 100, 2,100000);
					pDialog->Option_AddString(hSupEM,_l("Additional subjects to watch")+"\t"+_l("All messages with matched subject will be opened as incoming message")+". "+_l("Use '*' as a wildcard for partial matches, delimit masks with ';'"),&objSettings.sEM_Ads, objSettings.sEM_Ads);
					pDialog->Option_AddTitle(hSupEM,_l("This server will be used to send messages. Leave blank to use your default email client instead"),63);
					pDialog->Option_AddString(hSupEM,_l("SMTP server"),&objSettings.sEM_smhost, objSettings.sEM_smhost, OC_NETEM);
					pDialog->Option_AddString(hSupEM,_l("SMTP port"),&objSettings.sEM_smport, "25", OC_NETEM);
					pDialog->Option_AddString(hSupEM,_l("Login"),&objSettings.sEM_smlogin, objSettings.sEM_smlogin, OC_NETEM);
					pDialog->Option_AddPassword(hSupEM,_l("Password"),&objSettings.sEM_smpsw, objSettings.sEM_smpsw, OC_NETEM);
				}
			}
			if(!IsEmbedded())
			{// Spam checker
				{
					HIROW hSpam=pDialog->Option_AddHeader(hProtocolsRaw,_l(DEF_ANTISPAMTOPIC));
					hSpamBoxesPos=hSpam;
					pDialog->Option_AddHotKey(hSpam,_l("Check all accounts"),&objSettings.DefActionMapHK[CHECKALLACC],DefActionMapHK_Preinit[CHECKALLACC]);
					pDialog->Option_AddNString(hSpam,sResTsc+_l("How many accounts to handle","How many accounts to handle (restart to see changes)")+"\t"+_l("Use '0' to disable Anti-SPAM"),&objSettings.lAntiSpamAccountsFromNextStart, objSettings.lAntiSpamAccounts,0,100,OC_EMAILCNT);
					pDialog->Option_AddCombo(hSpam,_l("What to do when you closing Anti-SPAM window"),&objSettings.lOnCloseSpamWindow, 0, _l("Ask")+"\t"+_l("Kill marked messages")+"\t"+_l("Do not kill messages"));
					pDialog->Option_AddNString(hSpam,_l("Number of body lines to download with email header"),&objSettings.lAdditionalHeaderLines, DEFSPAMMAILLINES,0,10000);
					pDialog->Option_AddNString(hSpam,_l("Timeout for network operations (sec)"),&objSettings.dwMailboxTimeout, DEFAULT_MBTIMEOUT,3,1000,OC_MAILTIME);
					pDialog->Option_AddNString(hSpam,_l("Maximum log file size (Kb)"),&objSettings.dwMaxMailLogFileSize, DEFAULT_MAXMLLOGKB,3,1000);
					pDialog->Option_AddBOOL(hSpam,_l("Show check results in tray"),&objSettings.lShowSpamReportInTray, 1);
					HIROW hECD=pDialog->Option_AddString(hSpam,_l("Use this email client instead of default")+"\t"+_l("Leave blank to use default email client. Fill in to use custom email client"),&objSettings.sUserDefinedEmailB, "");
					pDialog->Option_AddActionToEdit(hECD,ChooseAnyFile);
					{
						pDialog->Option_AddTitle(hSpam,	_l("Spam detection rules"),63);
						HIROW hSpamSpecs=hSpam;
						pDialog->Option_AddBOOL(hSpamSpecs,_l("Ignore whitespaces in search strings"),&objSettings.lRemoveSubjWhiteSpaces, 1);
						pDialog->Option_AddBOOL(hSpamSpecs,_l("Ignore characters case in search strings"),&objSettings.lRemoveSubjCase, 1);
						pDialog->Option_AddBOOL(hSpamSpecs,_l("Automatically treat search strings as masks")+"\t"+_l("When checked, WireNote will treat search strings with '*' and '?' characters as masks"),&objSettings.lAllowMasksInSpamStrings, 0);
						pDialog->Option_AddBOOL(hSpamSpecs,_l("Tooltip: Show why the message was classified as SPAM"),&objSettings.lShowWhySpam, 1);
						pDialog->Option_AddBOOL(hSpamSpecs,_l("Block messages in unknown charset")+"\t"+_l("You can use mask char '*' and delimit different charsets with ';'"),&objSettings.lBlockMailInUnknownCharset, 1);
						pDialog->Option_AddBOOL(hSpamSpecs,_l("Block messages with charset-messed chars in the title"),&objSettings.lBlockMailWithMessedChars, 1);
						pDialog->Option_AddBOOL(hSpamSpecs,_l("Block messages with 5 sequential spaces or dots in subject"),&objSettings.lSubjSeqSpaces, 1);
						pDialog->Option_AddNString(hSpamSpecs,_l("Block messages with size larger then"),&objSettings.lMaxSizeToBeSpam, DEFSPAMMAILSIZE, 0, MAXLONG);
						pDialog->Option_AddString(hSpamSpecs,_l("Block messages with attachments of certain type"),&objSettings.sSPAMFileExtensions, DEF_SPAMFILEEXT);
						pDialog->Option_AddString(hSpamSpecs,_l("Block messages in the following charsets"),&objSettings.sBlockMailsWithThisCharsets);
						HIROW hSubjFile=pDialog->Option_AddString(hSpamSpecs,_l("'Subject' spam-strings file"),&objSettings.sSpamSubjectsFile, DEF_SPAMSUBJFILE);
						pDialog->Option_AddActionToEdit(hSubjFile,ChooseAnyFile);
						HIROW hAddrFile=pDialog->Option_AddString(hSpamSpecs,_l("'Address' spam-strings file"),&objSettings.sSpamAddressesFile, DEF_SPAMADDRFILE);
						pDialog->Option_AddActionToEdit(hAddrFile,ChooseAnyFile);
						HIROW hBodyFile=pDialog->Option_AddString(hSpamSpecs,_l("'Body' spam-strings file"),&objSettings.sSpamBodyFile, DEF_SPAMBODYFILE);
						pDialog->Option_AddActionToEdit(hBodyFile,ChooseAnyFile);
						HIROW hHeadFile=pDialog->Option_AddString(hSpamSpecs,_l("'Headers' spam-strings file"),&objSettings.sSpamHeadFile, DEF_SPAMHEADFILE);
						pDialog->Option_AddActionToEdit(hHeadFile,ChooseAnyFile);
					}
					for(int si=0;si<objSettings.lAntiSpamAccounts;si++){
						CString sAccName=objSettings.antiSpamAccounts[si].GetAccountName();
						CString sAccDsc=objSettings.antiSpamAccounts[si].GetAccountDsc();
						HIROW hSpamAccount=pDialog->Option_AddHeader(hSpam,_l("Mailbox")+": "+sAccName+"\t"+sAccDsc);//,44
						pDialog->Option_AddString(hSpamAccount,_l("Account name"),&objSettings.antiSpamAccounts[si].sAccountName, objSettings.antiSpamAccounts[si].GetAccountName());
						pDialog->Option_AddCombo(hSpamAccount,_l("What to do after check"),&objSettings.antiSpamAccounts[si].lAfterCheck,objSettings.antiSpamAccounts[si].lAfterCheck,_l("Show received messages")+"\t"+_l("Kill spam & run email client")+"\t"+_l("Kill spam & show report")+"\t"+_l("Kill spam"));
						pDialog->Option_AddBOOL(hSpamAccount,_l("After check: Run email client to receive residual messages"),&objSettings.antiSpamAccounts[si].lRunEmailAfterUserCheck, 1);
						pDialog->Option_AddBOOL(hSpamAccount,_l("Automatically start check when open mail window"),&objSettings.antiSpamAccounts[si].lStartCheckAuto, 0);
						HIROW hSpamCheck=pDialog->Option_AddBOOL(hSpamAccount,_l("Check mailbox every ... min"),&objSettings.antiSpamAccounts[si].lActive, 1);
						pDialog->Option_AddNString(hSpamCheck,"",&objSettings.antiSpamAccounts[si].dwCheckingPeriod, objSettings.antiSpamAccounts[si].dwCheckingPeriod,0,24*60);
						pDialog->Option_AddHotKey(hSpamAccount,_l("Account hotkey"),&objSettings.antiSpamAccounts[si].dwHotkey,objSettings.antiSpamAccounts[si].dwHotkey);
						{// Pop3 sets
							pDialog->Option_AddTitle(hSpamAccount,_l("POP3 Settings"),63);
							HIROW hSpamAccountPop3=hSpamAccount;
							pDialog->Option_AddString(hSpamAccountPop3,_l("POP3 Server"),&objSettings.antiSpamAccounts[si].sPOP3Server, objSettings.antiSpamAccounts[si].sPOP3Server);
							pDialog->Option_AddNString(hSpamAccountPop3,_l("POP3 port"),&objSettings.antiSpamAccounts[si].dwPOP3Port, objSettings.antiSpamAccounts[si].dwPOP3Port, 0, 65535);
							pDialog->Option_AddString(hSpamAccountPop3,_l("Login"),&objSettings.antiSpamAccounts[si].sAccount, objSettings.antiSpamAccounts[si].sAccount);
							pDialog->Option_AddPassword(hSpamAccountPop3,_l("Password"),&objSettings.antiSpamAccounts[si].sAccountPsw, objSettings.antiSpamAccounts[si].sAccountPsw);
						}
					}
				}
			}
			{
				HIROW hProxy=pDialog->Option_AddHeader(hProtocolsRaw,_l("HTTP Proxy")+"\t"+_l("Proxy settings (if needed)"));
				pDialog->Option_AddString(hProxy,_l("URL"),&CWebWorld::sProxyURL, CWebWorld::sProxyURL);
				pDialog->Option_AddNString(hProxy,_l("Port"),&CWebWorld::lProxyPort, CWebWorld::lProxyPort, 0, 65543);
				pDialog->Option_AddString(hProxy,_l("User"),&CWebWorld::sProxyUser, CWebWorld::sProxyUser);
				pDialog->Option_AddString(hProxy,_l("Password"),&CWebWorld::sProxyPsw, CWebWorld::sProxyPsw);
			}
		}
	}
	if(!bOnlyNotesOptions){// Sounds
		HIROW hSounds=pDialog->Option_AddHeader(FL_ROOT,	_l("Sounds"));
		HIROW hSoundsOnTActive=pDialog->Option_AddBOOL(hSounds,_l("ToDo activated"),&objSettings.bSoundOnActivation, 1, 53);
		HIROW hSoundsOnTActiveWav=pDialog->Option_AddString(hSoundsOnTActive,_l("Wav file"),&objSettings.sWavOnActivation, DEFAULT_TODOACT);
		pDialog->Option_AddActionToEdit(hSoundsOnTActiveWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnTActive,_l("Test sound"),TestSound, hSoundsOnTActiveWav);

		HIROW hSoundsOnTDone=pDialog->Option_AddBOOL(hSounds,_l("ToDo done"),&objSettings.bSoundOnDone, 1, 53);
		HIROW hSoundsOnTDoneWav=pDialog->Option_AddString(hSoundsOnTDone,_l("Wav file"),&objSettings.sWavSoundOnDone, DEFAULT_TODODONE);
		pDialog->Option_AddActionToEdit(hSoundsOnTDoneWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnTDone,_l("Test sound"),TestSound, hSoundsOnTDoneWav);

		HIROW hSoundsOnRemd=pDialog->Option_AddBOOL(hSounds,_l("Reminder triggered"),&objSettings.bRemSoundByDef, 1, 53);
		HIROW hSoundsOnRemdWav=pDialog->Option_AddString(hSoundsOnRemd,_l("Wav file"),&objSettings.sRemFileWav, REMINDER_SOUND);
		pDialog->Option_AddActionToEdit(hSoundsOnRemdWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnRemd,_l("Test sound"),TestSound, hSoundsOnRemdWav);

		HIROW hSoundsOnRemdNote=pDialog->Option_AddBOOL(hSounds,_l("Note reminder triggered"),&objSettings.bRemNoteSoundByDef, 1, 53);
		HIROW hSoundsOnRemdNoteWav=pDialog->Option_AddString(hSoundsOnRemdNote,_l("Wav file"),&objSettings.sRemNoteFileWav, REMDNOTE_SOUND);
		pDialog->Option_AddActionToEdit(hSoundsOnRemdNoteWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnRemdNote,_l("Test sound"),TestSound, hSoundsOnRemdNoteWav);

		HIROW hSoundsOnSend=pDialog->Option_AddBOOL(hSounds,_l("Sending message"),&objSettings.bSoundOnMsgSend, 1, 53);
		HIROW hSoundsOnSendWav=pDialog->Option_AddString(hSoundsOnSend,_l("Wav file"),&objSettings.sWavOnMsgSend, DEFAULT_SENDWAV);
		pDialog->Option_AddActionToEdit(hSoundsOnSendWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnSend,_l("Test sound"),TestSound, hSoundsOnSendWav);

		HIROW hSoundsOnDeliv=pDialog->Option_AddBOOL(hSounds,_l("Message delivered"),&objSettings.bSoundOnMsgDeliv, 1, 53);
		HIROW hSoundsOnDelivWav=pDialog->Option_AddString(hSoundsOnDeliv,_l("Wav file"),&objSettings.sWavOnMsgDeliv, DEFAULT_DELIVWAV);
		pDialog->Option_AddActionToEdit(hSoundsOnDelivWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnDeliv,_l("Test sound"),TestSound, hSoundsOnDelivWav);

		HIROW hSoundsOnNDeliv=pDialog->Option_AddBOOL(hSounds,_l("Message not delivered"),&objSettings.bSoundOnMsgNotDeliv, 1, 53);
		HIROW hSoundsOnNDelivWav=pDialog->Option_AddString(hSoundsOnNDeliv,_l("Wav file"),&objSettings.sWavOnMsgNotDeliv, DEFAULT_NDELIVWAV);
		pDialog->Option_AddActionToEdit(hSoundsOnNDelivWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnNDeliv,_l("Test sound"),TestSound, hSoundsOnNDelivWav);

		HIROW hSoundsOnReciv=pDialog->Option_AddBOOL(hSounds,_l("Recieved new message"),&objSettings.bPlaySoundOnMessage, 1, 53);
		HIROW hSoundsOnRecivWav=pDialog->Option_AddString(hSoundsOnReciv,_l("Wav file"),&objSettings.sMessageWavFile, DEFAULT_RECIVWAV);
		pDialog->Option_AddActionToEdit(hSoundsOnRecivWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnReciv,_l("Test sound"),TestSound, hSoundsOnRecivWav);

		HIROW hSoundsOnMail=pDialog->Option_AddBOOL(hSounds,_l("New emails"),&objSettings.bSoundOnMail, 1, 53);
		HIROW hSoundsOnMailWav=pDialog->Option_AddString(hSoundsOnMail,_l("Wav file"),&objSettings.sWavSoundOnMail, DEFAULT_ONMAIL);
		pDialog->Option_AddActionToEdit(hSoundsOnMailWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnMail,_l("Test sound"),TestSound, hSoundsOnMailWav);

		HIROW hSoundsOnError=pDialog->Option_AddBOOL(hSounds,_l("Any error"),&objSettings.bErrorSound, 1, 53);
		HIROW hSoundOnErrorWav=pDialog->Option_AddString(hSoundsOnError,_l("Wav file"),&objSettings.sWavOnError, DEFAULT_ONERRSOUND);
		pDialog->Option_AddActionToEdit(hSoundOnErrorWav,ChooseWavFile);
		pDialog->Option_AddAction(hSoundsOnError,_l("Test sound"),TestSound, hSoundOnErrorWav);
	}
	bOnlyNotesOptions=0;
	return TRUE;
}

BOOL WINAPI ExternalEnterFind(const char* szTitle, CString& sText,CWnd* pDialog)
{
	static long lAlread=0;
	if(lAlread){
		return 0;
	}
	SimpleTracker Lock(lAlread);
	CDlgSimpleEdit ed(pDialog);
	ed.m_Edit=szTitle;
	ed.m_EditTitle=sText;
	ed.iStyle=3;
	if(ed.DoModal()==IDOK){
		sText=ed.m_EditTitle;
		return 1;
	}
	return 0;
}

void CSettings::InitStuff()
{
	long iTmp=0;
	objSettings.objSchemes.getName(iTmp)=_l("System");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("White");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Antique");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Black");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Grey");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Navy");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Blue");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Yellow");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Gold");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Green");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Greeny");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Olive");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Orchid");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Pink");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Brick");iTmp++;
	objSettings.objSchemes.getName(iTmp)=_l("Alerts");iTmp++;
	objSettings.sNewNote_TitleM=_l(DEFNewNote_TitleM);
	objSettings.sNewToDo_TitleM=_l(DEFNewToDo_TitleM);
	objSettings.sNewRemd_TitleM=_l(DEFNewRemd_TitleM);
	objSettings.sNewRemd_BodyM=_l(DEFNewRemd_BodyM);
	objSettings.sNewMess_TitleM=_l(DEFNewMsg_TitleM);
	objSettings.sAutoReplys[0]=DEFAULT_AASTATUS0;
	objSettings.sAutoReplys[1]=DEFAULT_AASTATUS1;
	objSettings.sAutoReplys[2]=DEFAULT_AASTATUS2;
	objSettings.sAutoReplys[3]=DEFAULT_AASTATUS3;
}

void ClanchimDengi();
void CSettings::OpenOptionsDialog(const char* szDefTopic_i, CWnd* pParentWnd, int iAfterOpenType, LPVOID pAfData)
{
	static long lOptionsDialogOpened=0;
	if(lOptionsDialogOpened>0){
		return;
	}
	ClanchimDengi();
	CString szDefTopic=szDefTopic_i;
	if(szDefTopic==""){
		//AfxMessageBox("emp");
		szDefTopic=_l(DEF_COMMON);
	}
	if(IsEmbedded() && bStartWithOptions){
		//AfxMessageBox("bast");
		szDefTopic=_l(DEF_COMMON);
		InitStuff();
	}
	//AfxMessageBox(Format("%s-%s",szDefTopic,_l(NOTESETS_TOPIC)));
	if(szDefTopic==_l(NOTESETS_TOPIC)){
		bOnlyNotesOptions=1;
	}
	SimpleTracker Track(lOptionsDialogOpened);
	SimpleTracker Track2(objSettings.bDoModalInProcess);
	if(pMainDialogWindow){
		pMainDialogWindow->UnRegisterMyHotKeys();
	}
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
			CDLG_Chooser dlgChooser(_l("Select language"),_l("Choose your language"),&aLangBufferNames, GetUserDefaultLangID()==0x0419?1:0, IDR_MAINICO,NULL);
			iLang=dlgChooser.DoModal();
		}
		if(iLang>=0){
			GetApplicationLang()=iLang;
			InitStuff();
		}
		szDefTopic=_l(DEF_COMMON);//Перезадаем, так как этот пункт языка-зависимый
		// Спрашиваем основное назначение WireNote
		CStringArray aUseBufferNames;
		aUseBufferNames.Add(_l("Stickies & Reminders"));
		aUseBufferNames.Add(_l("LAN messenger"));
		aUseBufferNames.Add(_l("Both ways"));
		CDLG_Chooser dlgChooser(_l("Select purpose"),_l("Select primary WireNote purpose"), &aUseBufferNames, 0, theApp.aAppIcons[objSettings.iTrayIcon],NULL);
		int iRes=dlgChooser.DoModal();
		objSettings.lTrayIconsMaskO=1;
		objSettings.lTrayIconsMaskM=1;
		if(iRes==0){
			objSettings.lTrayIconsMaskO=1;
			objSettings.lTrayIconsMaskM=0;
			objSettings.lPreferencesLevel=1;
			objSettings.bProtocolXPC_Active=0;
			objSettings.bProtocolNB_Active=0;
			objSettings.bProtocolML_Active=0;
			objSettings.bProtocolEM_Active=0;
			objSettings.DefActionMapHK[NEW_MSG]=CIHotkey();
			objSettings.sFBDir="";
		}
		if(iRes==1){
			objSettings.lTrayIconsMaskO=0;
			objSettings.lTrayIconsMaskM=1;
			objSettings.lPreferencesLevel=2;
		}
	}
	// Чтобя перечитать все с новым языком
	GetBuffersLang(aLangBufferNames,iLang);
	//
	CDLG_Options_Startup pStartupInfo;
	pStartupInfo.fpInitOptionList=AddOptionsToDialog;
	pStartupInfo.fpApplyOptionList=ApplyOptionsFromDialog;
	pStartupInfo.iStickPix=objSettings.iStickPix;
	pStartupInfo.szRegSaveKey=IsEmbedded()?(PROG_REGKEY"\\Wnd_OptionsExWk"):(PROG_REGKEY"\\Wnd_OptionsEx");
	pStartupInfo.pImageList=&theApp.MainImageList;
	pStartupInfo.bExpandByDef=2;
	pStartupInfo.sz1stTopic=_l("Options");
	pStartupInfo.iWindowIcon=theApp.aAppIcons[objSettings.iTrayIcon];
	pStartupInfo.szDefaultTopicTitle=szDefTopic;
	pStartupInfo.szQHelpByDefault=_l("Choose topic from the left pane. Double-click option value in the right pane to change it");
	CDLG_Options dlgOpt(pStartupInfo,pParentWnd);
	dlgOpt.bShowFind=3;
	CString sOptionWndTitle=_l(OPTIONS_TP)+" - "+objSettings.sMainWindowTitle;
	if(IsEmbedded()){
		dlgOpt.bShowFind=0;
		dlgOpt.bHHideAddit=1;
		dlgOpt.bHHideHelp=1;
		sOptionWndTitle=_l(OPTIONS_TP);
	}
	pStartupInfo.szWindowTitle=sOptionWndTitle;
	dlgOpt.fpExternalSearch=ExternalEnterFind;
	dlgOpt.m_bSelInBold=1;
	if(objSettings.bStartWithOptions && !IsEmbedded()){
		dlgOpt.bBlockCancel=1;
	}
	g_bAllOptions=(szDefTopic!="");
	if(szDefTopic==""){
		pStartupInfo.szDefaultTopicTitle=_l(DEF_COMMON);
	}
	// Подготавливаем...
	Reminder* rem=objSettings.m_Reminders.GetHourTicker(FALSE);
	if(rem){
		objSettings.dwStrickingClockActive=rem->GetState();
	}else{
		objSettings.dwStrickingClockActive=0;
	}
	objSettings.bStartWithWindows=IsStartupWithWindows();
	if(IsEmbedded() && objSettings.bStartWithWindows){
		objSettings.bStartWithWindows=0;
		StartupApplicationWithWindows(FALSE);
	}
	lLikKeysPasted=0;
	g_iAfterOpenType=iAfterOpenType;
	g_hAfData=pAfData;
	// Вызываем
	DWORD dwRes=dlgOpt.DoModal();
	if(pMainDialogWindow){
		pMainDialogWindow->RegisterMyHotKeys();
	}
}

#define NAGTIME	(30*1000)
DWORD dwLastMsgLeakNagTime=0;
BOOL CSettings::CheckLiks(const char* szWho, const char* szOne, const char* szOther)
{
	#pragma message("Online registration check disabled...")
	return FALSE;
	if(objMessageWrapper->pNBNet && stricmp(szWho,objMessageWrapper->pNBNet->m_sAlias)==0){
		return FALSE;
	}
	if(*szOne=='/' || *szOther=='/'){
		return FALSE;
	}
	int iOneLen=strlen(szOne);
	int iOtherLen=strlen(szOther);
	int iCompare=stricmp(szOne,szOther);
	if(szOther && szOne && iCompare==0 && iOneLen>0 && iOtherLen>0){//&& iOneLen>=LKEY_LEN+LUSER_LEN && iOtherLen>=LKEY_LEN+LUSER_LEN &&// Если  хэшировать со сжатием - не будет работать
		if(GetTickCount()-dwLastMsgLeakNagTime>NAGTIME){
			dwLastMsgLeakNagTime=GetTickCount();
			CString sStr="'";
			sStr+=szWho;
			sStr+="' ";
			sStr+=_l("uses this program\nwith the same registration key as you");
			sStr+="\n";
			sStr+=_l("In this case license agreement is not valid any more!\nPlease contact us at www.WiredPlane.com");
			Alert(sStr,_l("Security leak found"));
			objSettings.sLikKey="";
			SaveKeysInfo();
			return TRUE;
		}
	}
	return FALSE;
}

void PrepareSpamFiles()
{
	if(IsEmbedded()){
		return;
	}
	objSettings.sSpamSubjectsFile=getFileFullName(objSettings.sSpamSubjectsFile);
	if(!isFileExist(objSettings.sSpamSubjectsFile)){
		if(!CopyFile(CString(GetApplicationDir())+"spam-subj.def",objSettings.sSpamSubjectsFile,TRUE)){
			SaveFile(objSettings.sSpamSubjectsFile,"");
		}
	}
	objSettings.sSpamAddressesFile=getFileFullName(objSettings.sSpamAddressesFile);
	if(!isFileExist(objSettings.sSpamAddressesFile)){
		if(!CopyFile(CString(GetApplicationDir())+"spam-addr.def",objSettings.sSpamAddressesFile,TRUE)){
			SaveFile(objSettings.sSpamAddressesFile,"");
		}
	}
	objSettings.sSpamBodyFile=getFileFullName(objSettings.sSpamBodyFile);
	if(!isFileExist(objSettings.sSpamBodyFile)){
		if(!CopyFile(CString(GetApplicationDir())+"spam-body.def",objSettings.sSpamBodyFile,TRUE)){
			SaveFile(objSettings.sSpamBodyFile,"");
		}
	}
	objSettings.sSpamHeadFile=getFileFullName(objSettings.sSpamHeadFile);
	if(!isFileExist(objSettings.sSpamHeadFile)){
		if(!CopyFile(CString(GetApplicationDir())+"spam-head.def",objSettings.sSpamHeadFile,TRUE)){
			SaveFile(objSettings.sSpamHeadFile,"");
		}
	}
}

BOOL CheckSharedItem(CFileInfo& inf)
{
	if(inf.GetFileName().Find(DEF_BB_STIMARK)==-1){
		CTime cBBItemTime=inf.GetCreationTime();
		CTime cCurrentTime=CTime::GetCurrentTime();
		CTimeSpan cS=cCurrentTime-cBBItemTime;
		if(cS.GetDays()<0){
			// Будущей датой...
			return 0;
		}
		if(objSettings.lFBIAliveTime<15){
			objSettings.lFBIAliveTime=15;
		}
		if(cS.GetDays()>objSettings.lFBIAliveTime){
			objSettings.lFBStat_UpdD++;
			::DeleteFile(inf.GetFilePath());
			return 0;
		}
	}
	if(objSettings.sExcludeBillboardItems.Find(inf.GetFileName()+";")!=-1){
		return 0;
	}
	return 1;
}

CString GetFBFolderRoot()
{
	if(objSettings.lFBDirNorm==0){
		objSettings.lFBDirNorm=1;
		objSettings.sFBDir.TrimLeft();
		objSettings.sFBDir.TrimRight();
		objSettings.sFBDir.TrimRight("\\/");
		objSettings.sFBDir+="\\";
		if(!CreateDirIfNotExist(objSettings.sFBDir)){
			objSettings.lFBDirNorm=0;
			return "";
		}
	}
	CreateDirIfNotExist(objSettings.sFBDir);
	return objSettings.sFBDir;
}

CString GetFBFolder(CString sPostFix,CString sBBPrefix)
{
	CString sDir=GetFBFolderRoot();
	if(sBBPrefix!=""){
		sDir+=FBBB_PR0;
		sDir+=sBBPrefix;//"MAIN";
		sDir+="\\";
		if(!CreateDirIfNotExist(sDir)){
			return "";
		}
		if(sPostFix!=""){
			sDir+=FBBB_PR;
		}
	}else{
		sDir+=FBUSER_PR;
	}
	if(sPostFix!=""){
		sDir+=sPostFix;
		sDir+="\\";
	}
	if(!CreateDirIfNotExist(sDir)){
		return "";
	}
	return sDir;
}

BOOL NotifyAboutSharedItem(CString sID,const char* szFile)
{
	if(sID==""){
		BOOL bNew=0;
		CString sNotification=_l("Checking billboard for new items. Results")+":\n";
		if(objSettings.lFBStat_NewN || objSettings.lFBStat_UpdN){
			sNotification+=Format("%s (%s/%s): %i/%i\n",_l("Notes"),_l("Bbnew","New"),_l("Bbupdated","Updated"),objSettings.lFBStat_NewN,objSettings.lFBStat_UpdN);
			bNew=1;
		}
		if(objSettings.lFBStat_NewT || objSettings.lFBStat_UpdT){
			sNotification+=Format("%s (%s/%s): %i/%i\n",_l("ToDos"),_l("Bbnew","New"),_l("Bbupdated","Updated"),objSettings.lFBStat_NewT,objSettings.lFBStat_UpdT);
			bNew=1;
		}
		if(objSettings.lFBStat_NewR || objSettings.lFBStat_UpdR){
			sNotification+=Format("%s (%s/%s): %i/%i\n",_l("Reminders"),_l("Bbnew","New"),_l("Bbupdated","Updated"),objSettings.lFBStat_NewR,objSettings.lFBStat_UpdR);
			bNew=1;
		}
		if(objSettings.lFBStat_UpdD){
			sNotification+=Format("%s: %i\n",_l("Old items deleted"),objSettings.lFBStat_UpdD);
			bNew=1;
		}
		sNotification.TrimRight();
		objSettings.lFBStat_NewR=0;
		objSettings.lFBStat_UpdR=0;
		objSettings.lFBStat_NewT=0;
		objSettings.lFBStat_UpdT=0;
		objSettings.lFBStat_NewN=0;
		objSettings.lFBStat_UpdN=0;
		objSettings.lFBStat_UpdD=0;
		if(bNew){
			objSettings.lFBStat_Titles.TrimRight();
			objSettings.lFBStat_Titles.TrimRight(",; ");
			if(objSettings.lFBStat_Titles!=""){
				sNotification+="\n-------\n"+objSettings.lFBStat_Titles;
			}
			pMainDialogWindow->ShowBaloon( _l("Billboard updated"),sNotification);
		}
	}else{
		AddBBExcl(GetPathPart(szFile,0,0,1,1));
		CItem* pItem=CItem::GetItemByID(sID);
		if(pItem){// Его может и не быть!!!
			objSettings.lFBStat_Titles+="'";
			objSettings.lFBStat_Titles+=TrimMessage(pItem->GetTitle(),25)+"'; ";
			pItem->SetModify();
			pItem->ItemChanged();
			pItem->ActivateChanges();
		}
	}
	return FALSE;
}

CString GetValidFileName(CItem* item,CString sPref,CString sExt, int iSection)
{
	if(!item){
		return "";
	}
	if(iSection<0 || iSection>=objSettings.aSubsBB.GetSize()){
		Alert(_l("You are not subscribed to billboard sections\nYou must subscribe or create at least one of them"));
		return "";
	}
	CString sRes=GetFBFolder(sPref,objSettings.aSubsBB[iSection]);
	if(sRes==""){
		return "";
	}
	sRes+=item->GetID();
	int iCount=item->GetRevision()+1;
	if(iCount>51){
		Alert(_l("This item were changed more than 50 times\nIt is better to create separate item with the same content\nThis item can`t be published any more"));
		return "";
	}
	CString sFullFile=((iCount==0)?(sRes+"."+sExt):Format("%s[%i].%s",sRes,iCount,sExt));
	if(isFileExist(sFullFile)){
		DWORD dwReply=AskYesNo(_l("Billboard contain more recent version of this item\nDo you want to continue and overwrite it?\nClick 'No' to receive recent version from billboard later"),0,pMainDialogWindow);
		if(dwReply!=IDYES){
			return "";
		}
	}
	while(isFileExist(sFullFile)){
		sFullFile=Format("%s[%i].%s",sRes,++iCount,sExt);
	}
	return sFullFile;
}

CString GetBBSectionName(CString sName)
{
	if(sName=="MAIN"){
		return _l("Main");
	}
	CString sBBFolder=GetFBFolder("",sName);
	if(sBBFolder==""){
		return Format("%s: unaccesible",sName);
	}
	CString sDesc;
	ReadFile(sBBFolder+FB_BBDSC_FILE,sDesc);
	CString sNameOut=(CDataXMLSaver::GetInstringPart("<desc>","</desc>",sDesc));
	CDataXMLSaver::Xml2Str(sNameOut);
	if(sNameOut==""){
		sNameOut=sName;
	}
	return sNameOut;
}

CString GetFBFZ()
{
	CString sFileZone=GetFBFolder("","FILEZONE");
	return sFileZone;
}


int DaysFromInstall_Reg()
{
	CRegKey hKey;
	hKey.Open(HKEY_CURRENT_USER, PROG_REGKEY);
	CString sK=CString("InstallDate");
	CString sNow=GetRegString(hKey.m_hKey,sK,100);
	COleDateTime dtNow=COleDateTime::GetCurrentTime();
	COleDateTime dt=CDataXMLSaver::Str2OleDate(sNow);
	if(dt.GetStatus()!=COleDateTime::valid){
		SetRegString(hKey.m_hKey,sK,CDataXMLSaver::OleDate2Str(dtNow));
		return 0;
	}
	return int((dtNow-dt).GetTotalDays());
}