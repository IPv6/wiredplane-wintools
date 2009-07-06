#ifndef _COMMON_DEFINES
#define _COMMON_DEFINES
//////////////////////////////////////////
///// Standart defines ///////////////////
//////////////////////////////////////////
//#define SEND_ADMINS_ONLY
#define PROG_VERSION	"3.5.3"
#define PROGNAME		"WireNote GPL"
#define SUPPORT_EMAIL	"support@wiredplane.com"
#define DOWNLOAD_URL	"www.wiredplane.com"
#define REGNAG_TEXT		((CString)(objSettings.iLikStatus==1?("\n---\n"+_l("Trial period is over")+".\n"+_l("Please register WireNote")+"!"):""))
#define GPL
//
#define BUFSIZE	1024
#define IIDSIZE	20
#define DIALOG_MINXSIZEW	330
#define DIALOG_MINYSIZEW	200
#define TRAY_ICONS	7
//
#define LKEY_LEN		20
#define LKEY_LENS		"20"
#define LUSER_LEN		8
#define LUSER_LENS		"8"
#define	COMPANY_NAME	"WiredPlane"
#define SENDTEXT_ONLINE "WireNote.Message"
#define EXCHSTATUS_ONLN	"WireNote.StatusExchange"
#define LOGTIMEFRMT		"%d.%m.%Y %H:%M:%S"
#define MAILTIMEFRMT	"%H:%M"
#define XML_HEADER		"<?xml version='1.0' encoding='Windows-1251'?>"
#define PROG_REGKEY		"SOFTWARE\\"COMPANY_NAME"\\WireNote"
#define PROG_REGKEYWP	"SOFTWARE\\"COMPANY_NAME"\\WireChanger"
#define WP_TEMPLATE		"Template"
#define WRONG_ITEMID	"CORRUPTED ELEMENT"
#define LICENSE_KEY_FILE "License.key"

//////////////////////////////////////////
// My message ID`s
#define CREATE_NEWNOTE		WM_USER+1
#define NEEDORGREACTION		WM_USER+2
#define DELNOTE				WM_USER+3
#define FIREALERT			WM_USER+4
#define UPDATERECLIST		WM_USER+5
#define CREATE_NEWTODO		WM_USER+6
#define REMOVE_REMD			WM_USER+7
#define SETWAITT			WM_USER+8
#define SETCOLW				WM_USER+9
#define OPENPREFS			WM_USER+10
// Actions enum
#define DEF_ACTION			0
#define FIRST_HKEYNUM		100
#define MAIL_HKEYNUM		200
#define NOTES_HKEYNUM		300
#define NEW_NOTE			1
#define NEW_TODO			2
#define NEW_REMD			3
#define NEW_MSG				4
#define OPEN_TODOLIST		5
#define SHOW_ALLNOTES		6
#define OPEN_ADDRBOOK		7
#define OPEN_LASTUNREAD		8
#define HIDE_ALLNOTES		9
#define CHECKALLACC			10
#define NEXT_NOTE			11
#define MAX_HOTACTIONS		12
// Желательно сохранять возрастающий порядок
#define USERMSG_STEP		999
#define TASK_NOTE_OFFSET	10000
#define SWITCH_TO			11000
#define SEND_TO_OFFSET		12000
#define CHANGE_NOTE_STYLE	13000
#define PINTOWINDOW			14000
#define KILLPROCESS			15000
#define ATTACHMENT_SAVE		16000
#define ATTACHMENT_REM		17000
#define SHOW_ANTISPAM		18000
#define BB_SECTION			19000
#define ADV_BASE			20000
#define SMILS_BASE			21000
#define EXTERNS_BASE		29000
#define MAX_USERMSG			30000
// Другое
#define NOTEID_PREFX		"NoteID:"
#define ALERTSTYLE_PREFX	"VStyle:"
#define DELETEIFOK_PREFX	"DelIfOk"
#define SOUND_MSGSEND		"$MSGSEND$"
#define SOUND_MSGDELIV		"$MSGDELIV$"
#define SOUND_MSGNOTDELIV	"$MSGNOTDELIV$"
#define SOUND_ONMSG			"$ONNEWMSG$"
#define SOUND_ONMAIL		"$ONNEWMAIL$"
#define SOUND_ONDONE		"$ONTODODONE$"
#define SOUND_ONACTIVATION	"$ONACTIVATION$"
#define ERROR_SOUND			"$ERROR$"
#define CLIP_COPY			"$CLIPCOPY$"
#define CLIP_PASTE			"$CLIPPASTE$"
#define CONV_TEXT			"$CONVTEXT$"
//
#define ID_TIMER			101
#define DEF_BIGBMPSIZE		100
#define PRIOR_ICONS_OFFSET	11
#define NOTE_ICON_NUM		6
#define TR_ICONS_OFFSET		28
#define REM_ICONS_OFFSET	34
#define REG_ICONS_OFFSET	42
#define APP_ICONS_OFFSET	33
#define EMPTY_ICON_OFFSET	13
#define SCLOCK_ICONS_OFFSET	35
#define NOTE_PATH			"Notes"
#define NOTE_EXTENSION		"note"
#define NOTE_TODOPATH		"ToDo-Notes"
#define TODO_PATH			"ToDo"
#define TODO_EXTENSION		"todo"
#define MESS_PATH			"Messages"
#define REMD_PATH			"Reminders"
#define REMD_EXTENSION		"remd"
#define REMS_PATH			"System"
#define BB_PATH				"Billboard"
#define REMSHARE_PATH		"Shared"
#define LINK_PATH			"Links"
#define REMSYS_ID			"REMSYS_ID"
#define BILLBFOL_ID			"BILLBFOL_ID"
#define REGTODO_ID			"REGISTRATIONTODO"
/////////////////////////////////////////////////////////////////////////////
// Миллисекунд на основной таймер программы
#define TIMER_SETTIMER_TICK			500
// Через сколько срабатываний таймера проверять напоминания
#define TIMER_CHECKREMINDERS		20
// Через сколько срабатываний таймера проверять обновление окна организатора
#define TIMER_CHECKTREEUPDATE		1
// Через сколько срабатываний таймера проверять screen saver
#define TIMER_CHECKAUTOAWAY			5
// Default values
#define SOUND_CATALOG		"Sound\\"
#define REMINDER_SOUND		"Sound\\remind.wav"
#define REMDNOTE_SOUND		"Sound\\remind_note.wav"
#define DEFAULT_TRASHFOLDER	"Trash"
#define DEFAULT_ONERRSOUND	"Sound\\error.wav"
#define DEFAULT_ONCCOPYSND	"Sound\\clipcopy.wav"
#define DEFAULT_ONCPASTESND	"Sound\\clippaste.wav"
#define DEFAULT_ONLCONV		"Sound\\lconv.wav"
#define DEFAULT_ONMAIL		"Sound\\mail.wav"
#define DEFAULT_TODOACT		"Sound\\tdactiv.wav"
#define DEFAULT_TODODONE	"Sound\\tddone.wav"
#define DEFAULT_WPFILE		"IPv6WallPaper.bmp"
#define DEFAULT_ABOOKFILE	"addressbook.xml"
#define DEFAULT_MSLOT		"messngr"
#define DEFAULT_NETTO		10000
#define DEFAULT_MAXLOGKB	25
#define DEFAULT_MAXMLLOGKB	20
#define DEFAULT_MBTIMEOUT	20
#define DEFAULT_MSGLOG		"Messages"
#define DEFAULT_UPDSATMIN	60
#define DEFAULT_MAXATTSIZE	1024
#define DEFAULT_AASTATUS0	_l("Sorry, i am busy, Please contact me later")
#define DEFAULT_AASTATUS1	_l("I am currently away, Please contact me later")
#define DEFAULT_AASTATUS2	_l("I am currently busy, Please contact me later")
#define DEFAULT_AASTATUS3	_l("Please, do not disturb me, i am very busy")
#define DEFAULT_SENDWAV		"Sound\\msgsend.wav"
#define DEFAULT_DELIVWAV	"Sound\\msgdeliv.wav"
#define DEFAULT_NDELIVWAV	"Sound\\msgnotdeliv.wav"
#define DEFAULT_RECIVWAV	"Sound\\message.wav"
#define MIN_TRASH_SIZE		10
#define XPC_PORTDEF			1709
#define DEFAULT_LOGEXT		".log"
#define DEFAULT_GENERALLOG	"messages_log"
#define DEFAULT_ANTISPAMLOG	"spamcheck"
#define DEFAULT_PERSONLOG	"_log"
#define BLANK_PAGE			"about:blank"
#define DEFAULT_HOURLY		"Sound\\hourly.wav"
#define DEFAULT_CHKFRAUD	0
#define DEFAULT_TEXTSUBMENU	1
#define DEFAULT_SYSSUBMENU	1
#define WBT_INIT			WM_APP+1
#define WBT_LOAD			WM_APP+2
#define WBT_EXIT			WM_APP+3
#define REGISTRATION_TOPIC	"Registration"
#define MESSAGING_TOPIC		"Messaging"
#define MAINFEAT_TOPIC		"Main Features"
#define MESFILTER_TITLE		"Message filters"
#define TRIALMAX_DAYS		7
#define DEFAULT_REGNOTICE	_l("Note: You can freely use this program during test period of 30 days",\
		"Note: You can freely use this program during test period of 30 days\n"\
		"\nFollowing this test period of 30 days (or less)"\
		"\nif you wish to continue to use WireNote, you must register!"\
		"\nSee help file for details.")
#define OPTIONS_TPM			"Options"
#define REGWPPATH			"WPTemplatePath"
#define INVALIDUSERNAME		"/nb/INVALIDNAME/nb/"
#define DEFANIM_NOTEDEL		2
#define DEFANIM_MSGNEW		2
#define DEFAULT_ONMESSAGE	3
#define BIGLIST_ID			"BIG"
//
#define DEFNewNote_TitleM	"Note '%RNDNAME' [%DAY.%MONTH %HOUR:%MINUTE]"
#define DEFNewNote_BodyM	"%CLIP"
#define DEFNewToDo_TitleM	"ToDo '%RNDNAME' [%DAY.%MONTH %HOUR:%MINUTE]"
#define DEFNewToDo_BodyM	"%CLIP"
#define DEFNewRemd_TitleM	"'%RNDNAME' Reminder"
#define DEFNewRemd_BodyM	"%HOUR:%MINUTE Don`t forget about "
#define DEFNewMsg_TitleM	"%HOUR:%MINUTE Message to %RECP"
#define DEFNewMsg_BodyM		""
#define UNREG_FUNC_TEXT		"You must register WireNote\nin order to use this functionality"
#define UNREG_FUNC_TITLE	"WireNote: Registration"
#define WNUSAGE_TITLE		"How to use WireNote - short info"
#define DEFAULT_NETPROT		1
#define DEFAULT_READTIME	5000
#define DEFAULT_ATTWARN		"Note: Attachments/Emoticons are not supported\nthrough this protocol!"
#define DEFAULT_NOPROTWARN	"Protocol disabled or failed to start"
#define DEFAULT_NOTEVIEW	0
#define DEFAULT_BALOONTIP	0
#define DEFAULT_WORDWRAP	1
#define NOTEWINDOW_CLASS	"IPV6_NOTE"
#define DEF_OKBLOCKTIME		2000
#define DEF_SUBMITPREVENTER	1
#define DEF_MENUTITLELEN	15
#define DEFAULT_REMACTCOUNT	14
#define DEFAULT_DELBUTTONP	1
#define DEFAULT_OTHBUTTONP	0
#define NETSETS_TOPIC		"Network features"
#define MSGFILTER_TOPIC		MESFILTER_TITLE
#define MSGSETS_TOPIC		MESSAGING_TOPIC
#define DEF_ANTISPAMTOPIC	"Mail checker"
#define NOTESETS_TOPIC		"Notes"
#define TODOSETS_TOPIC		"ToDos"
#define REMDSETS_TOPIC		"Reminders"
#define BBRDSETS_TOPIC		"Billboard"
#define DEF_CHECKSHAREREMDS	60
#define DEF_SPAMADDRFILE	CString(GetApplicationDir())+"spam-addr.txt"
#define DEF_SPAMSUBJFILE	CString(GetApplicationDir())+"spam-subj.txt"
#define DEF_SPAMBODYFILE	CString(GetApplicationDir())+"spam-body.txt"
#define DEF_SPAMHEADFILE	CString(GetApplicationDir())+"spam-head.txt"
#define DEFPOP3SERVER		_l("<POP server>")
#define DEFACCLOGIN			_l("<your login>")
#define DEFSPAMMAILLINES	30
#define DEFSPAMMAILSIZE		20000000
#define DEFBILLB_EXLSIZE	256*1024
#define DEF_SPAMFILEEXT		"*.exe;*.com;*.pif;*.scr;*.bat;*.cmd"
#define USAGE_ID			"WNUSAGETODO"
#define DEFAULT_WNUSAGE		_l("WireNote can be used in varous ways",\
		"WireNote can be used in various ways: as organizer and LAN messenger"\
		"\n\n As organizer WireNote can keep track of your ToDos, sticky notes and can remind your about your important events."\
		"\n To create new ToDo, note or reminder click on the corresponding button on the right side of the main window"\
		" or press predefined hotkeys 'Ctrl-Shift-T'/'Ctrl-Shift-N'/'Ctrl-Shift-R'. Or you can choose corresponding item in the tray menu."\
		" All ToDos/notes/reminders can be archived at any time through WireNote tray menu"\
		"\n\n As messenger WireNote can communicate with standard WinPopup application, other WireNote application or standard messenger service on Win NT/2000/XP."\
		" To create message press predefined hotkey 'Ctrl-Shift-M' or choose corresponding item in the tray menu. Then type message text, the recipient address and press 'Ctrl-Enter' to send message."\
		" You can save most used addresses in the address book - WireNote will detect messages from address book users automatically and will keep track of their online availability."\
		" You can setup almost any aspect of the messaging in the preferences (for example 'How to show new messages' - immediately or keep them in tray - or something else)"\
		"\n See help file for details.")

#define NO_SEND_BODY		"Sorry, you can`t send messages. This feature can be used by Administrators only"
#define NO_SEND_TITLE		"Restriction"
#define MSG_WHITE_PART		"(!)"
#define DEF_ATTACHPREF		"AEXT_"
#define DEF_OPENTYPE		DEF_ATTACHPREF"OTYP"
#define DEF_DELIVERYNOTIFY	DEF_ATTACHPREF"DNOT"
#define DEF_DELIVERYREQUEST	DEF_ATTACHPREF"DREQ"
#define DEF_READREQUEST		DEF_ATTACHPREF"RREQ"
#define DEF_NOTTIMESTAMP	DEF_ATTACHPREF"STSP"
#define DEF_NOTID			DEF_ATTACHPREF"SMID"
#define DEF_NOTTITLE		DEF_ATTACHPREF"NTIT"
#define DEF_AANSM			DEF_ATTACHPREF"AAMM"

#define DEF_EMAIL_SUBJ		"[WireNote]"
#define DEF_NAH				"Note activation hotkey"
#define DEF_BB_STIMARK		"_sticked"
#define DEF_COMMON			"Common"
#define DEF_BB_NDIR			"NOTE"
#define DEF_BB_RDIR			"REMD"
#define DEF_BB_TDIR			"TODO"
#define FBMSG_EXT			"fbm"
#define FBSTATUS			"fbstatus.def"
#define FBUSER_PR			"FBU"
#define FBBB_PR				"BBI_"
#define FBBB_PR0			"BILLBOARD_"
#define	FB_BBDSC_FILE		"bbinfo.xml"
#define DEF_ACTIVEPROTS		"Active protocols"
#define SMS_TEXT			"To register send SMS"
#endif
