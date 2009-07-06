// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_)
#define AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_defines.h"
#include "..\SmartWires\SystemUtils\SystemInfo.h"
#include "ItemModif.h"
#include "AntiSpamAccount.h"

class CSettings
{
public:
	// Common functions
	void Finalize();
	void ApplySettings();
	void Load(const char* szFile="", int iLevel=-1);
	void Save();
	void SaveAll();
	void InitStuff();
	void OpenOptionsDialog(const char* szDefTopic=NULL, CWnd* pParent=NULL, int iAfterOpenType=0, LPVOID pAfData=0);
	BOOL CheckLiks(const char* szWho, const char* szOne, const char* szOther);
	CSettings();
	virtual ~CSettings();
	// Common vars
	BOOL bUnderWindows98;
	long bRunStatus;
	long bSaveAllowed;
	long bStartWithWindows;
	long bStartWithOptions;// Начинать работу проги с показа Options
	char szSystemFolder[MAX_PATH+1];
	long iStickPix;
	long bShadows;
	long bDoModalInProcess;
	CString sLikUser;
	CString sLikKey;//20-длина ключа, 1-разделитель, 10-имя пользователя
	CString sLikFile;
	long iLikStatus;// 0 - все ок, 30 дней, 1 - 30 закончились, 2 - есть ключ
	//////////////////////////
	// WireKeys
	//////////////////////////
	CIHotkey DefActionMapHK[MAX_HOTACTIONS];//горячие клавиши для действий
	// Для функций "горячего" меню эти данные должны быть уже установлены (как для кликнутости, так и гор. клавиши)
	HWND hFocusWnd;
	HWND hHotMenuWnd;
	CRect ActiveWndRect;
	//////////////////////////
	// WireNote
	//////////////////////////
	long lTrayIconsMaskO;
	long lTrayIconsMaskM;
	long lShowDateInTrayIcon;
	long lShowMsgInAddrbookIcon;
	long lShowUnreadInAddrb;
	long lAlertsOnTaskbar;
	long bNoSend;
	long bGenerateRandomNames;
	CString sNewToDo_TitleM;
	CString sNewToDo_BodyM;
	CString sNewNote_TitleM;
	CString sNewNote_BodyM;
	CString sNewMess_TitleM;// Дополнительно - %RECP, %RECP_IP
	//CString sNewMess_TitleMFROM;// Дополнительно - %RECP, %RECP_IP
	CString sNewMess_BodyM;
	CString sNewRemd_TitleM;
	CString sNewRemd_BodyM;
	long lLastReminderCheckTime;
	//
	CString sLastNewToDoPath;
	CString sLastSaveAttachDir;
	CString sLastSaveNoteTextDir;
	CString sLastFindString;
	long lAlertTextStyle;
	long lNOTEICON_DIM;
	long lShowTextInIcon;
	long iSHowTitleInIconMode;
	long iEnableAutoHideWithAlt;
	long iEnableAutoHideWithEsc;
	long bPopupNotesOnActivation;
	long iNoteOnTopByDef;
	long bCheckForDefMessenger;
	long bNoteDefaultView;
	long lDragDropBehave;
	long bToDoWordWrap;
	long bAutoCreateTodoFromMW;
	long bSWarning_ShareOK;
	long iNetbiosLanaNum;
	LANA_ENUM asEnum;
	CString sLastMessageRecipient;
	CMessageWrapper* objMessageWrapper;
	long bHourlyByHCount;
	long bRemHideToTrayDef;
	long bRemDelStrictDef;
	long lRemSubmitPreventer;
	CString sHourlyWav;
	long bShowSystemErrors;
	long dwMaxAttachSizeKB;// в килобайтах
	CString sLastGetAttachPath;
	long bSaveNoteAttachments;// Сохранять ли аттачи с messagами (может сильно увеличить размер)
	long bShowMsgProgress;
	long bAskToResendMsg;
	long bMaxTrashSize;
	long bGroupBySender;
	long bAutoPopupAdrBook;
	long bUseNickOnSend;
	long bCheckPinnedNoteForRect;
	//
	CDLG_Find* pCurrentFindDialog;
	CStringArray aProtList;
	long iProtocol_Preffered;//0 - MS, 1 - NB, 2 - XPC, 3 - FB, 4 - EMBox
	long bProtocolML_Active;
	long bProtocolNB_Active;
	long bProtocolXPC_Active;
	long bProtocolEM_Active;
	long bProtocolRS_Active;
	long lSkipSizeNotification;
	long lDisableAttachExtens;
	long lNotifyAboutFMes;
	COleDateTime dtLastSentMsgDate;
	CString sProtocolML_Slot;
	CString sProtocolNB_Alias;
	CString sFB_ID;
	CString sFBDir;
	CString sFBNick;
	long lFBDirNorm;
	CString sExcludeBillboardItems;
	BOOL bExclChanged;
	long lFBIAliveTime;
	CString sEM_p3host;
	long lEM_p3port;
	CString sEM_p3login;
	CString sEM_p3psw;
	CString sEM_smhost;
	CString sEM_smport;
	CString sEM_smlogin;
	CString sEM_smpsw;
	CString sEMmail;
	CString sEM_Ads;
	//long lEM_p3box;
	long lEMPeriod;
	long lFBCrypt;
	long lFBNotifyOfRead;
	long lReqNotifyOfRead;
	long lReqNotifyOfDeliv;
	long lShowNotInPopup;
	long lShowNotAfterTime;
	long lReceiveNotifications;
	long dwMinFBPeriod;
	long dwMidFBPeriod;
	long bOnSharedItem;
	long bOnSharedItemN;
	long bOnSharedItemT;
	long bOnSharedItemR;
	long bOnSharedItemM;
	long lFBStat_NewR;
	long lFBStat_UpdR;
	long lFBStat_NewT;
	long lFBStat_UpdT;
	long lFBStat_NewN;
	long lFBStat_UpdN;
	long lFBStat_UpdD;
	CString lFBStat_Titles;
	long iNB_LanaNum;
	long dwNB_Lana;
	long dwProtocolXPC_Port;
	long dwProtocolRSSMAxNews;
	long dwProtocolRSSOType;
	long lRSSLocUser;
	long lRSSSummar;
	//
	long iTrayIcon;
	long bShowActiveTDInTray;
	long bActivateDoneToDo;
	//long bDelMsgAlertIfNotDeliv;
	CString sRootMenuId;
	static int StatusIcons[4];
	long bSetAwayIfScrSaver;
	long bSoundOnActivation;
	long bSoundOnDone;
	long bSoundOnMail;
	long bErrorSound;
	CString sWavOnError;
	CString sWavOnActivation;
	CString sWavSoundOnDone;
	CString sWavSoundOnMail;
	long bAutoQuote;
	CRect rDesktopRectangle;
	long bAutoGetNoteText;
	//long lNoteTranspFocuFriendly;
	long bAutoDisp;
	long bDelToTrash;
	long lOnExitFromMainWnd;
	long dwGlobalCount;
	CString sFolderFile;
	CString sLinksFile;
	long bTraceToDoHist;
	long bOnDuplMessage;
	long bReceiveToDosAction;
	long bReceiveRemdsAction;
	long bNoteAHideMode;
	long bNoteCTMode;
	long iRemdStyle;
	long iAlertStyle;
	long iMainStyle;
	long bDelMsgIfDelivered;
	long bWWIncomingMsg;
	long bEscDelIncomingMsg;
	long bButtonsStyle;
	long dwButtonSize;
	long dwNoteButtonSize;
	long bHideMsgFld;
	long bAutoMoveNoteToMsg;
	long iAnimOnNoteDel;
	long iAnimOnMsgDel;
	long iAnimOnNewMsg;
	long lMsgRecentListSize;
	long bNoteDelButtonPos;
	long bNoteDelButtonWay;
	long bNoteOptButtonPos;
	long bAskDlg_DelNote;
	long lUseBloonOnNewMsg;
	long lRecentUserByDef;
	long lPreferencesLevel;
	CStringArray lToDoFRecentList;
	CStringArray lMsgRecentList;
	CStringArray lMsgQAnswers;
	long sQuotationLen;
	long bSoundOnMsgSend;
	long bSoundOnMsgDeliv;
	long bSoundOnMsgNotDeliv;
	CString sWavOnMsgSend;
	CString sWavOnMsgDeliv;
	CString sWavOnMsgNotDeliv;
	long bRemSoundByDef;
	long bRemNoteSoundByDef;
	long bMessCentreDefAction;
	long bMakeNewTDCur;
	COleDateTime dStartTime;
	long dwNetMessagingTimeout;
	long dwMaxLogFileSize;
	long dwMaxMailLogFileSize;
	long dwMailboxTimeout;
	long lStatReceivedMails;
	long lStatReceivedSpamMails;
	long lStatReceivedMessages;
	long lStatSendedMessages;
	long lStatNotesCreated;
	long lShowSpamReportInTray;
	long dwAutoSaveTime;//Автосейв всего
	long iStatusUpdateTime;// время в минутах между проверками
	long iStatusUpdateTimeLastTCount;// Время последней проверки
	CString sMessageWavFile;
	long bPlaySoundOnMessage;
	long bOnNewMessage;
	CMapStringToString aAutoAnswersRecepients;
	CString sAutoReplys[4];
	long iOnlineStatus;
	long bAutoAnswer;
	long bARForUnknownUsers;
	CString sTrayTooltip;
	CString sMCTrayTooltip;
	CString sMainWindowTitle;
	void InitFilterList();
	CArray<CMessageFilter*,CMessageFilter*> messageFilters;
	//long bMinConfToTray;
	long bNotShowInListTodo;
	long bNotShowInListNotes;
	long bUseDblClickToStart;
	long bNotShowInListRem;
	CStringArray mainViewFilter;
	long iSortColumn;
	long iSortAColumn;
	long bShowDone;
	long bShowNonActiveReminders;
	long lRemindFromSleep;
	long bAdjustNotesPosOnScr;
	long bHideDateFld;
	long bTreeNeedUpdate;
	CString sTreeNeedMoveFocus;
	long iCol0Width;
	long iCol1Width;
	long iCol2Width;
	long iColWChWidth;
	long iCol0WidthAdrb;
	long iCol1WidthAdrb;
	long iCol2WidthAdrb;
	long iFeditH;
	CArray<CItem*, CItem*> Items;
	CRITICAL_SECTION lock;
	CRITICAL_SECTION bTreeNeedUpdateLock;
	long ReminderDLGLastAction;
	CAddrBook AddrBook;
	CTaskHistory m_Tasks;
	CNotesBook m_Notes;
	CReminders m_Reminders;
	CString sLocalComputerName;
	long lNB_SupportNonStandAlias;
	long lNB_SupportLocUser;
	long SaveMesNotesByDef;
	long SaveMesNotesByDefForUser;
	CString sMessageLogDirectory;
	CNoteSettings NoteSettings;
	CNoteSettings TaskNoteSettings;
	CNoteSettings MessageNoteSettings;
	CColorScheme objSchemes;
	long iStartNoteScheme;
	long iStartTaskNoteScheme;
	long iStartMessageScheme;
	long iMenuItemsCount;
	long iLastSchemeUsed;
	CString sAddrFile;
	CString sTaskDirectory;
	CString sNotesDirectory;
	CString sRemdDirectory;
	CString sTrashDirectory;
	CString sRemFileWav;
	CString sRemNoteFileWav;
	long bNoteWordWrap;
	long APToDoList;
	long DefActionMap[MAX_HOTACTIONS];//0-сочетание клавиш для defaultа, остальное - для действий
	long UseBblClick;
	// Network sets
	long bNetSubsystemInitialized;
	long dwStrickingClockActive;
	long bCheckFraudMsg;
	//-----AntiSpam-------
	long lAntiSpamAccounts;
	long lAntiSpamAccountsFromNextStart;
	CArray<CAntiSpamAccount,CAntiSpamAccount&> antiSpamAccounts;
	long lAdditionalHeaderLines;
	long lMaxSizeToBeSpam;
	CString sSpamAddressesFile;
	CString sSpamSubjectsFile;
	CString sSpamBodyFile;
	CString sSpamHeadFile;
	CString sSPAMFileExtensions;
	long lRemoveSubjWhiteSpaces;
	long lRemoveSubjCase;
	long lSubjSeqSpaces;
	long iSpamCol0Width;
	long iSpamCol1Width;
	long iSpamCol2Width;
	long lShowWhySpam;
	long lOnCloseSpamWindow;
	long lAllowMasksInSpamStrings;
	long lCheckForUpdate;
	long lDaysBetweenChecks;
	long bFBMessageCount;
	long lBlockMailInUnknownCharset;
	CString sBlockMailsWithThisCharsets;
	long lBlockMailWithMessedChars;
	CString sUserDefinedEmailB;
	long iLastViewTab;
	//long lNoteLeaveBorder;
	CString sDefAuthor;
	CCriticalSection csSharing;
	CStringArray aSubsBB;
	long dwLastSendError;
	long iSMSCount;
	CString sLastIndexOfOpenedNote;
	long lFEditHR;
	long iDefNoteW;
	long iDefNoteH;
};
// Вызов диалога со стилями
BOOL CallStylesDialogWnd(CWnd* pWnd,int iStartStyle=-1);
// Название ящика
CString GetMailboxAccountName(int si);
void PrepareSpamFiles();
BOOL CALLBACK CallImportAB(HIROW hData=0, CDLG_Options* pDialog=0);
BOOL CALLBACK CallExportAB(HIROW hData=0, CDLG_Options* pDialog=0);
BOOL CheckSharedItem(CFileInfo& inf);
CString GetFBFolder(CString sPostFix,CString sBBPrefix="");
BOOL NotifyAboutSharedItem(CString sID,const char* szFile);
CString GetValidFileName(CItem* item,CString sPref,CString sExt, int iSection);
BOOL SaveBBExcl();
BOOL AddBBExcl(const char* szID);
CString GetFindMessageText();
void GetAllInfo(CFindedUser& pUser,CString& sStatus);
CString GetBBSectionName(CString sName);
CString GetFBFZ();
CString GetFBFolderRoot();
void AppendAdverts(CMenu* m_pMenu);
BOOL ParseAdvert(long i);
int DaysFromInstall_Reg();
#endif // !defined(AFX_SETTINGS_H__4E4F8446_F53A_4127_B458_C1C3B3FF4261__INCLUDED_)
