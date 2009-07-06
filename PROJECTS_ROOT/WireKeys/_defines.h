#ifndef _COMMON_DEFINES
#define _COMMON_DEFINES
//////////////////////////////////////////
///// Standart defines ///////////////////
//////////////////////////////////////////
#define VERSION_MAJOR	3
#define VERSION_MINOR   7
#define VERSION_BUILD   1
#define	MINOR_VALUE		""
#define PLUGINAPI_VER	371
#define PROG_VERSION	GetAppVersion()

#define GPL
#ifdef GPL
#define PROGNAME		"WireKeys GPL"
#else
#define PROGNAME		"WireKeys"
#endif

#define WK_DEPRECATED

//
#define IIDSIZE	20
#define DIALOG_MINXSIZE	300
#define DIALOG_MINYSIZE	200
#define TRAY_ICONS	7
//
#define LICENSE_KEY_FILE "License.key"
#define XML_HEADER	"<?xml version='1.0' encoding='Windows-1251'?>"
#define PROG_REGKEY "SOFTWARE\\"COMPANY_NAME"\\"PROGNAME
#define WRONG_ITEMID "CORRUPTED ELEMENT"
#define PARENT_PROG "WireNote"

//////////////////////////////////////////
// My message ID`s
#define CREATE_NEWNOTE		WM_USER+1
#define NEEDORGREACTION		WM_USER+2
#define FIREHOTMENU			WM_USER+3
#define FIREALERT			WM_USER+4
#define ADDTOQUICKRUNS		WM_USER+5
#define TRAYICO_DBCL		WM_USER+6
#define CHANGEICON			WM_USER+7
#define LANG_CHANGE			WM_USER+8
#define OPT_TOPIC			WM_USER+9
#define UPDATE_ICON			WM_USER+10
#define STOPMACRO			WM_USER+11
#define SHOWCURBALOON		WM_USER+12
#define SETTRAYICON			WM_USER+13
#define EXTERNSTUFF			WM_USER+14
#define EDITTEXT			WM_USER+15
#define LOADPLUGIN			WM_USER+16
#define RCLICKHLP			WM_USER+17


#define MAX_CLIPS			99
#define MAX_DESKTOPS		99
#define MAX_LAYOUTS			99
#define MAX_QRUNAPPLICATION	999
#define MAX_EXTRAMAC		999

#define MAX_PLUGFUNCS		999
// Желательно сохранять возрастающий порядок
#define USERMSG_STEP		999
#define USERMSG_STEP_HALF	499
#define EXPAND_FILES_MAX	1999
#define PLUGINTP_OFFSET		10000
#define SWITCH_TO			11000
#define SEND_TO_OFFSET		12000
#define CHANGE_NOTE_STYLE	13000
#define SWITCHLAY			14000
#define KILLPROCESS			15000
#define ATTACHMENT_SAVE		16000
#define ATTACHMENT_REM		17000
#define HIDEDWINDOWS		18000
#define QRUNAPPLICATION		19000
#define QRUNAPP_EDIT		19500
#define ID_COPYTOCLIP		20000
#define ID_PASTECLIP		21000
#define ID_CLIPHISTORY		22000
#define ACTIVEAPP_PRIOR		23000
#define SWITCH_DESKTOP		24000
#define PLUGIN_FUNCTION		25000
#define SYSMENU_FUNCTION	26000
#define SET_DMODE			27000
#define EXPAND_FILES		28000
#define QRUNAPP_EXTRA		30000
#define HK_HISTCL			500
#define HK_SWITBASE			600
#define HK_LAYOBASE			700
#define HK_HISTCLAPP		800
#define HK_PLUGBASE			1000
#define HK_EXTRAMACRO		(HK_PLUGBASE+MAX_PLUGFUNCS+1)
#define HK_MACRO			(HK_EXTRAMACRO+MAX_EXTRAMAC+1)
// Все, дальше-только ужимать интервалы!
#define MAX_USERMSG			30000
#define MAX_USERMSG2		40000
// Другое
#define CLIP_COPY			"$CLIPCOPY$"
#define CLIP_PASTE			"$CLIPPASTE$"
#define CONV_TEXT			"$CONVTEXT$"
#define TAKE_SSHOT			"$SCREENSHOT$"
#define CALC_TEXT			"$CALCTEXT$"
#define HK_TEXT				"$HKTRIG$"
#define SND_ERROR			"$ERROR$"
//
#define ID_TIMER			101
#define PRIOR_ICONS_OFFSET	11
#define NOTE_ICON_NUM		6
#define TR_ICONS_OFFSET		28
#define REM_ICONS_OFFSET	34
#define REG_ICONS_OFFSET	42
#define APP_ICONS_OFFSET	33
#define EMPTY_ICON_OFFSET	13
#define SCLOCK_ICONS_OFFSET	35
#define	IMAGELIST_SIZE		16
/////////////////////////////////////////////////////////////////////////////
#define SOUND_CATALOG		"Sound"
#define REMINDER_SOUND		"Sound\\remind.wav"
#define REMDNOTE_SOUND		"Sound\\remind_note.wav"
#define DEFAULT_TRASHFOLDER	"Trash"
#define DEFAULT_ONERRSOUND	"Sound\\error.wav"
#define DEFAULT_ONCCOPYSND	"Sound\\clipcopy.wav"
#define DEFAULT_ONCPASTESND	"Sound\\clippaste.wav"
#define DEFAULT_ONLCONV		"Sound\\lconv.wav"
#define DEFAULT_SCRSHOT		"Sound\\sshot.wav"
#define DEFAULT_CALCSOUND	"Sound\\calc.wav"
#define DEFAULT_HKSOUND		"Sound\\hk.wav"
#define DEFAULT_MAXLOGKB	5
#define DEFAULT_TEXTSUBMENU	1
#define DEFAULT_SYSSUBMENU	2
#define DEFAULT_SCRSUBMENU	0
#define DEFAULT_QRUNSUBMENU	0
#define DEFAULT_MACRSUBMENU	1
#define DEFAULT_VOLUMEMENU	0
#define DEFAULT_SCRFILE		"Scr%YEAR%MONTH%DAY_%HOUR%MINUTE_#%COUNT_%TITLE"
#define DEFAULT_SCRSIGN		"%DATE %TIME"
#define DEF_TRANSMITAPP		"*** Timestamp: [%DATE %TIME] ***"
#define DEFAULT_SHOWHMENU	1
#define DEFAULT_XRES		800
#define DEFAULT_YRES		600
#define PLUGINS_TOPIC		"Plugins set-up"
#define SUPPORT_OWONDRDEF	1
// Info-windows codes
#define ID_INFOWND_OPENPR	WM_USER+1
#define ID_INFOWND_CLSEPR	WM_USER+2
#define ID_INFOWND_DELETE	WM_USER+3
#define ID_INFOWND_TERM		WM_USER+4
#define ID_INFOWND_DETACH	WM_USER+5
#define ID_INFOWND_EDITQ	WM_USER+6
#define ID_INFOWND_SETHK	WM_USER+7
#define ID_INFOWND_SET_TITL	WM_USER+8
// Это значение должно быть больше чем ID_INFOWND_... чтобы не перекрываться
#define INFOWNDOFFSET		10
//
#define BGCOLOR				RGB(0xFA,0xEB,0xD7)
#define TXCOLOR				RGB(0,0,0)
#define INFOWND_TIMERID		1
#define	DEF_STARTUPTIME		1
#define	DEF_STARTUPMSGT		1
#define	DEF_BOOSTPRIOR		1
#define	DEF_OSDSECS			1
#define	DEF_OSDCHARS		30
#define	DEF_CDCLOSETIME		6
#define DEF_SPLASHTEXTCOL	32768
#define DEF_SPLASHBGCOL		65408
#define ABOVE_NORMAL_PRIORITY_CLASS	0x00008000
#define DEF_JPGQUALITY		80
#define DEF_MAXCLIPSIZE		5
#define DEF_DESKNAMESIZE	256
#define	DEF_CLIPHIST		30
#define TRAYAPP_FOLDER		"TrayAppConfigs"
#define MACROS_FOLDER		"Macros"
#define DEF_MACROSTEMPLATE	"// Type basic information about this macro in the next lines\r\n"\
							"// Title: My macro\r\n"\
							"// Macro parameter type (string/file/folder/none)\r\n"\
							"// ParamType: String\r\n"\
							"// Description: No description yet\r\n"\
							"\r\n"\
							"// You can pre-create some ActiveX objects, that can be used in macro\r\n"\
							"// var WshFSO = new ActiveXObject('Scripting.FileSystemObject');\r\n"\
							"// var WshShell = new ActiveXObject('WScript.Shell');\r\n"\
							"\r\n"\
							"// This is the main function. WireKeys will call this function with \r\n"\
							"// selected text and macro parameter. Remove parameters from function definition if you don`t need them.\r\n"\
							"function wkMain(selectedText,macroParameter)\r\n"\
							"{\r\n"\
							"	var OUTPUT=selectedText;\r\n"\
							"	// Do whatever is needed. You can use patterns (see Help for details)\r\n"\
							"	// Remember that patterns are substituted BEFORE macro starts to work\r\n"\
							"	OUTPUT+='Today date is %MONTH.%DAY.%YEAR, time: %HOUR:%MINUTE:%SECOND\\r\\n';\r\n"\
							"	OUTPUT+='Macro parameter: '+macroParameter+'\\r\\n';\r\n"\
							"	for(i=0;i<5;i++){\r\n"\
							"		OUTPUT+='Line '+(i+1)+': Hello world!\\r\\n';\r\n"\
							"	};\r\n"\
							"	// Returning result back into active application (if not empty)\r\n"\
							"	return OUTPUT;\r\n"\
							"};\r\n\r\n"\
							"// More help can be found here:\r\n"\
							"// http://www.wiredplane.com/en/wirekeys-help/help.php?section=macros\r\n"\
							"// http://www.wiredplane.com/en/wirekeys-help/help.php?section=api\r\n"\
							"// http://www.wiredplane.com/forum/board.php?cat=3&fid=1&s=s\r\n"\
							"\r\n"\
							"// Macro can have several entry points at the same time (wkMain2,wkMain3,...)\r\n"\
							"// Each entry point can be called with separate hotkey\r\n"\
							"// Entry point #2 description\r\n"\
							"// function wkMain2(){};\r\n"

#define DEF_BROWSEICON		"INETBROWSER"
const char* GetAppVersion();
#define	UNREGDAY_MENU			30
#define	UNREGDAY_SCR			40
#define	UNREGDAY_PUT			50
#define	UNREGDAY_ALERT			35
#define TUNE_UP_ITEM2(menu,txt)	{if(!objSettings.lDisableSCHelp){(menu).AppendMenu(MF_SEPARATOR, 0, "");AddMenuString(&(menu),RCLICKHLP,_l(txt));};};
#define OPTIONS_TP				"Preferences"
#define DEFQRUN_TOPIC			_l("Quick launch")+" "+_l("and macros")
#define DEFAPPLACTIONS_TOPIC	_l("Quick launch")+": "+_l("Actions")
#define DEFQMAC_TOPIC			DEFQRUN_TOPIC/*"Keyboard and application macros"*/
#define SCRSHOT_TOPIC			"Screenshots parameters"
#define REGISTRATION_TOPIC		"Registration"
#define	VOL_TOPIC				_l("Sound volume control")
#define SCRSAVE_TOPIC			_l("Screen saver")
#define	SHUTDOWN_TOPIC			_l("Quick shutdown/restart")
#define	BOSS_TOPIC				_l("Boss key")
#define	CDEJECT_TOPIC			_l("Quick CDROM Eject/Load")
#define	WINCONT_TOPIC			_l("Active window and processes")
#define	SAVEICONS_TOPIC			_l("Desktops icons")
#define	ADDDESK_TOPIC			_l("Additional desktops")
#define	PROCLIST_TOPIC			_l("Processes list")
#define	SELTXT_TOPIC			_l("Selected text operations")
#define	GSTARTED_TOPIC			_l("Miscellaneous")
#define	INFOWNDHK_TOPIC			_l("Define activation hotkey")
#define DEFAULT_REGNOTICE2		_l("Note: This program is still unregistered",\
								"Note: This program is still unregistered!\n"\
								"\nYou can freely use this application for 14 days."\
								"\nIf you wish to continue to use WireKeys"\
								"\nafter that period, you have to register it."\
								"\nVisit http://www.WiredPlane.com for details")+"\n\n"+\
								_l("Unregistered version will not save your changes")+"."
#define DEFAULT_REGNOTICE3		_l("Trial period is over",\
								"Trial period is over. Your changes will not be saved!"\
								"\nRegister WireKeys to unlock all features."\
								"\nVisit http://www.WiredPlane.com for details")

#define	SCRSEQCAUTION			_l("Caution: To stop sequenced screenshooting","Caution: To stop sequenced screenshooting, you MUST press 'Make Screenshot' hotkey AGAIN")+"!"
#define REG_DSC					CString("*** ")+_l("Register "PROGNAME)+"! ***\t"+_l("Please, register "PROGNAME,"Please, register "PROGNAME". Registered users will get more useful plugins, can request special features to be implemented in the minor upgrades. Registrations will be used to make this application more powerful, useful and easy-to-use")
#define EXPAND_FOLDER_ID		"*** EXPAND:"
#define EXPAND_FOLDER_MASK		"*** MASK:"
#ifdef _DEBUG
#define	RTRACE(X,Y)				{pMainDialogWindow->ShowPopup(X,Y,1000);}
#else
#define	RTRACE(X,Y)
#endif
#define RUNHIST_NAME			"CSRUN-HIST"
#define MAX_HKL					10
#define MAX_BMPCLIP_SIZE		5000000
#define MAX_BMPCLIP_SIZE_SAVE	500000
#define ADD_MACRO_PREF			"ADD:"
#define SCRIPT_MACRO_PREF		"javascript:"
//wp_macrorec wp_mgestures wp_schedule 
#define DEF_PLUGINS				" wp_keymaster wp_wtraits "
#endif