#ifndef _COMMON_DEFINES
#define _COMMON_DEFINES

//////////////////////////////////////////
///// Standart defines ///////////////////
//////////////////////////////////////////
//#define USE_LITE
//#define FOR_ELEFUN
#define MAIN_ICON	IDR_MAINICO_TRAY
#ifdef ART_VERSION
	#define USE_LITE
	#define PROG_VERSION		"1.0"
	#define	BUILD_NUM			100
#else
	//---------------
	#define PROG_VERSION		"3.7.2"
	#define	BUILD_NUM			372
#endif
//---------------
#ifdef	USE_LITE
	#define LITE_VERSION	"Light"
	#define PROG_VERSION	LITE_VERSION
#endif

#define GPL
#ifdef GPL
#define PROGNAME		"WireChanger GPL"
#else
#define PROGNAME		"WireChanger"
#endif

#define WC_TRIAL			9

//#define LIC_HARDCODED_U	"vhartman100@optusnet.com.au"
//#define LIC_HARDCODED_K	"8K9G-4VER-6BBH-MXV9"

//---------------
#define MAX_WPT				100
#define MAX_CLOCKS			100
#define BUFSIZE				1024
#define DIALOG_MINXSIZE		300
#define DIALOG_MINYSIZE		200
#define DEFAULT_WPSTRLIM	100
#define DEFAULT_SUPPHTML	1
#ifdef ART_VERSION
#define DEFAULT_PERIODMIN	2
#else
#define DEFAULT_PERIODMIN	30
#endif
#define DEF_TRAYICON		1
#define LKEY_LEN			20
#define LKEY_LENS			"20"
#define LUSER_LEN			8
#define LUSER_LENS			"8"
//
#define XML_HEADER		"<?xml version='1.0' encoding='Windows-1251'?>"
#define PROG_REGKEY		"SOFTWARE\\"COMPANY_NAME"\\"PROGNAME
#define WRONG_ITEMID	"CORRUPTED ELEMENT"
#define PARENT_PROG		"WireNote"
#define LICENSE_KEY_FILE "License.key"

//////////////////////////////////////////
// My message ID`s
#define WBT_LOAD			WM_APP+2
#define WBT_EXIT			WM_APP+3
#define WBT_STOP			WM_APP+4
#define FIREALERT			WM_USER+4
#define DESKTOP_DBLC		WM_USER+5
#define DESKTOP_WALLC		WM_USER+6
#define DESKTOP_WALLC2		WM_USER+7
#define DESKTOP_MOVE		WM_USER+8
#define DESKTOP_MOVEEX		WM_USER+9
#define DESKTOP_REFRESH		WM_USER+10
#define OPEN_TEMPL_PARS		WM_USER+11
#define DESKTOP_SCLICKU		WM_USER+12
#define DESKTOP_SCLICKD		WM_USER+13
#define DESKTOP_SCMOVE		WM_USER+14
#define DESKTOP_CALLONDT	WM_USER+16
// Actions enum
#define NEXT_WALLPAPER		0
#define BRING_INTOVIEW		1
#define ORDER_NEXT			2
#define ORDER_PREV			3
#define NEW_NOTE			4
#define HIDE_ICONS			5
#define HIDE_WIDGETS		6
#define ORDER_SAME			7
#define OPEN_CALEN			8
#define MAX_HOTACTIONS		9
// ∆елательно сохран€ть возрастающий пор€док
#define USERMSG_STEP		999
#define REFRTEMPL			1
#define CHANGE_TEMPLATE		10000
#define CHANGE_TEMPL_PAR	11000
#define CHANGE_CLOCKS		12000
#define CHANGE_THEME		13000
#define OPEN_NOTE			14000
#define MOVE_TO_THEME		15000
#define ACTIVATE_PRESET		16000
#define PRESET_ON_WP		17000
#define MAX_USERMSG			30000
#define MAXROOTS_PERTEMPL	10
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
#define DEF_NSKIN			1
#define SOUND_CATALOG		"Sound\\"
#define WP_REMINDERS		"Reminders"
#define WP_TEMPLATE			"Template"
#define WP_IMPORTS			"ActiveItems"
#define WP_CLOCKS			"Clocks"
#define WP_NOTES			"Notes"
#define WP_WALLP			"Wallpapers"
#define DEFAULT_HTMLBG		"wp_backgrnd.bmp"
#define DEFAULT_HTMLBGTEMPL	PROGNAME"-Wallpaper.html"
#define REGISTRATION_TOPIC	"Registration"
#define SSAVE_TOPIC			"Screensaver"
#define DESKNOTE_TOPIC		"Desktop notes"
#define BLANK_PAGE			"about:blank"
#define DEFAULT_WPFILE		"IPv6WallPaper.bmp"
#define WALLPAPERLIST_TP	"Wallpaper generation"
#define OPTIONS_TP			"Preferences"
#define REGWPPATH			"WPTemplatePath"
#define VALID_EXTEN			"*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pcx;*.tif;*.tga;*.tiff;*.wmf;*.ico"
#define BGCOLOR			RGB(0xFA,0xEB,0xD7)
#define TXCOLOR			RGB(0,0,0)
#define DEFAULT_CPUPROC	(objSettings.bUnderWindowsNT?60:100)
#define DEFAULT_CPUPROE	(objSettings.bUnderWindowsNT?0:0)
#define DEFAULT_MEMPROE	(objSettings.bUnderWindowsNT?0:0)
#define DEFAULT_MEMPROC	(objSettings.bUnderWindowsNT?80:80)
#define DEF_HTMLTIMEOUT	10
#define DEF_PRVIEWDELAY	500
#define DEF_MORECLOCKS	"Download clocks..."
#define DEF_RNDCLOCK	"Random clock"
#define DEF_NOCLOCKS	"Without clock"
#define PLEASE_REGISTER	"Please register"
#define LOGO_W			95
#define LOGO_H			75
#define CONFIG_EXT		".cfg"
#define PROFILE_PREFIX	"Profile:"
#define UNREG_FUNC_TITLE	PROGNAME": Registration"
#define REG_BENEFITS	_l("Registered users will get more",\
		"Registered users will get more desktop note skins, additional desk-clocks\n"\
		"collections, free minor upgrades and can use "PROGNAME" without limitations.\n"\
		"Note, than single license can be used to install "PROGNAME" on up to 3 PCs.\n"\
		"See help file for details.")
#define DEFCONTES			_l("You will also get registration key in exchange for you wallpaper");
#define DEFAULTTEMPLATE_END		"</body></html>"
#define DEFAULTTEMPLATE_BEGIN	"<html>"\
	"<head><META HTTP-EQUIV='Content-Type' Content='text/html; $=<TEXTLINE(comm_l$=<LANG>.txt,CODEPAGE=)>'>"\
	"<style>$=<INCLUDE(files\\styles.css)></style></head><body SCROLL='no' bgcolor='$=<BGCOLOR>' text='$=<FGCOLOR>' style='border-style: none;'>"\
	"<table id='BGROUND' border=0 background='$=<WALLPAPER>' style='position: absolute; left: 0; top: 0; width: $=<INTEXPR($=<SCREEN_X>)>; height: $=<INTEXPR($=<SCREEN_Y>)>; z-index: -1000;'><tr><td>&nbsp;</td></tr></table>"
#define DEFICONTCOLOR	GetSysColor(COLOR_HIGHLIGHTTEXT)
#define DEFICONBGTCOLOR	GetSysColor(COLOR_APPWORKSPACE)
#define DEFNOTEICONSIZE	CSize(50,50)
#define DEFNOTEICONSIZM	CSize(16,16)
#define DEF_CHONSTART	1
#define DEF_PROCEICONS	0
#define DEFNOTEPRF		"*cIcon"
#define DEFNOTEPRFM		"*cIconMini"
#define DEFNOTEPRFB		"*cIconBg"
#define DEFNOTEPRFBM	"*cIconBgMini"
#define ROOT_WND_NAME	"WC_ROOT"
#define CUTLINE_TAG		"wp:body"
#ifdef _DEBUG
#define	RTRACE(X,Y)				{pMainDialogWindow->m_STray.PopupBaloonTooltip("","");pMainDialogWindow->m_STray.PopupBaloonTooltip(X,Y,0,100000);}
#else
#define	RTRACE(X,Y)
#endif
#define WP_SEPAR_F		":???:"
#define DEF_WARNTIME	60000
#define DEF_CLONEPOST	"_#"
#define DEF_HIDBR		"<<!<br>!>>"
#define DEF_MAKETEMPLC	"Duplicate template"
#define DEF_DOWNTEMPLC	"Download templates"
#define DEF_FDUP		"How to use 'Find duplicated images' module\n\n"\
						"1) Wait until information is gathered\n\n"\
						"2) WireChanger will show duplicated images\n"\
						"You have to press right or left 'Delete' button\n"\
						"to mark corresponding image for deletion or press\n"\
						"'Next image pair' to skip current images. You can\n"\
						"press 'Auto' to auto-remove all duplicated images\n\n"\
						"3) At the end of image queue you will be asked\n"\
						"to confirm deletion (to actually remove marked images)"
#define DEF_LITENOTICE	"You are using Lite version of WireChanger\n"\
						"This feature available in Full version only"
#define DEF_FIRSTPOPUP	"There are interactive widgets on your wallpaper like calendar and quotations. "\
						"Double-click cell to type text for a day, use Shift/Control click to popup widget options. "\
						"You can also double-click wallpaper to add new text note."
#define DEF_NOTESCATEG	"Additional elements"
#define DEF_CLOCKCATEG	"Clocks"
#define DEF_GAMESCATEG	"Games"
#define DEF_CALENCATEG	"Calendars"
#define DEF_USEFCATEG	"Entertainment"
#define DEF_MODIFCATEG	"Wallpaper modifiers"
#define DEF_NOTESID		-15
#define DEF_DOWNLOADID	-16
#define DEF_DOWNLOADCID	-17
#define DEF_APPLYID		-18
#define DEF_CURSELUNREG	_l("Warning: Several active widgets can not be\nused with unregistered version of WireChanger")+"!"
#define OPTION_UNREGF	  _l("In order to use this option, you must obtain registration first")+"\n"+_l("Register "PROGNAME" to remove this limitation")+"!"
#define OPTION_UNREG	  OPTION_UNREGF+"\n("+_l("Exchange wallpaper for free registration")+")"
#define OPTION_NOTE_UNREG _l("You already have 2 notes")+".\n"+_l("To add another one, you must obtain registration first")+"\n"+_l("Register "PROGNAME" to remove this limitation")+"!"+"\n("+_l("Exchange wallpaper for free registration")+")"
#define OPTION_WIDG_UNREG _l("You already have 4 active widgets")+".\n"+_l("To add another one, you must obtain registration first")+"\n"+_l("You can also disable one of the enabled widget")+"\n"+_l("Register "PROGNAME" to remove this limitation")+"!"+"\n("+_l("Exchange wallpaper for free registration")+")"
#define OPTION_REMD_UNREG _l("You already have active reminder")+".\n"+_l("To add another one, you must obtain registration first")+"\n"+_l("Register "PROGNAME" to remove this limitation")+"!"+"\n("+_l("Exchange wallpaper for free registration")+")"
#define OPTION_CALE_UNREG _l("This item can not be edited now")+".\n"+_l("Register "PROGNAME" to remove this limitation")+"!"+"\n("+_l("Exchange wallpaper for free registration")+")"
#define CHIDDEN			"Hidden"
/////////////////////////////////////////////////////////////////////////////
#endif