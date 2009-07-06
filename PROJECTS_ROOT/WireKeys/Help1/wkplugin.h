//<pre>
#if !defined(WKPLUGIN_HEADER__INCLUDED_)
#define WKPLUGIN_HEADER__INCLUDED_
#include <stdlib.h>
#include <windows.h>
/*
    WireKeys plugin header file for VC++
    ------------------------------------------
    WireKeys plugin must be DLL renamed to '*.wkp' or standalone application ('*.exe'),
    placed in the "Plugins" directory of the WireKeys home catalog.
    WireKeys enums this folder at startup.
    ------------------------------------------
    Here is the plugin`s life-circle 
    (the order in which WireKeys will call extern function from plugin):

    a) Start-up functions
    -----------------------------------------------------------------------------------------------------
        Plugin`s Function name          What function expected to do
    -----------------------------------------------------------------------------------------------------
    a.1.WKPluginStart                   Basic initializations. This must be quick and easy function!!!
    a.2.GetPluginDsc                    Basic information about plugin: plugin name, icon, description and so on
    -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
    ** If plugin was previously disabled by user, WireKeys will go right to the section 'c' from here **
    -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
    a.3.WKPluginOptionsManager          Here plugin can add plugin-driven options into WireKeys preferences dialog.
                                        Plugin don`t need to save/restore them, this will be done automatically
    a.4.WKGetPluginFunctionCommonDesc   Getting plugins func`s description. 
                                        This function will be called in cycle with the parameter from 0 to ... (max 100) until plugin will not return 0 in reply.
                                        By this function WireKeys enums plugin`s actions. Every action can be called by hotkey or tray menu.
                                        It is WireKeys resposibility to remember hotkeys and other stuff about every action
    a.5.WKPluginInit                    The rest of the plugin`s initialization code (hook installations, etc).
                                        This function will be called from separate thread and can be as lenghty as needed.
										WARNING: This function will be called for RESIDENT plugin only!
    -----------------------------------------------------------------------------------------------------
    b) Functions, that can be called multiple times
    -----------------------------------------------------------------------------------------------------
    b.1.WKGetPluginFunctionActualDesc   This will be called right before showing popup menu with items, belonging to this plugin.
                                        Plugin can check/disable/temporarily rename menu item according to internal state
    b.2.WKCallPluginFunction            This is the main function :) This function will be called every time user choose corresponding menu item 
                                        or press corresponding hotkey to call plugin action
    b.3.WKPluginOptions                 This will be called when user wants to change plugin`s preferences (special button in the WireKeys preferences dialog)
    b.4.WKPluginOptionsManager          When user change WireKeys settings, plugin will be notified about this event by call to this function.
                                        Here plugin can retrieve plugin-driven options and change behaviour accordingly.
    b.5.WKPluginPause                   This will be called when WireKeys needs to temporarily shutdown plugin,
                                        for example to allow user switch main desktop (where second instance of the WireKeys with this plugin will be started) and so on
										This is important function, especially for plugins with system-wide hooks,
										because WireKeys can work in different desktops with the same plugin in the same time
										If this function is absent, plugin will be notified about desktop switching at all and will not be loaded in second instances of WireKeys
	b.6.WKDescktopSwitching				Called on switching desktop in WireKeys. Important for plugins that used hooks, because they are dropped by the system when desktop is changing
	b.7.WKTrayMenuPopup					Called on opening WK tray menu. Use this method to add custom submenu/items to WK tray/hot menu
    -----------------------------------------------------------------------------------------------------
    c) Shutdown functions
    -----------------------------------------------------------------------------------------------------
    c.1.WKPluginStop                    Pre-unload function. Cleanup, etc.
    -----------------------------------------------------------------------------------------------------
    If you made a cool plugin and what him to be shared with other WireKeys users, you can send plugin to 
    plugins@wiredplane.com in exchange for free registration key
    ------------------------------------------
    WiredPlane.com @2002-2008. All rights reserved
*/
//--------------------------------------------
// Defines
//--------------------------------------------
// Misc
#define WKERRORMSG_SIZE             256
#define WKITEMDSC_SIZE              256
#define WKPLUGTITLE_SIZE            256

// Menu Item position
// Using this constants you can set places
// where user will see plugin`s menu items
#define WKITEMPOSITION_PLUGIN       0x00000001
#define WKITEMPOSITION_DESK         0x00000002
#define WKITEMPOSITION_QRUN         0x00000004
#define WKITEMPOSITION_SELTEXT      0x00000008
#define WKITEMPOSITION_MISC         0x00000010
#define WKITEMPOSITION_OPTFLOATS    0x00000020
// This positions will be added irrespective of
// tray/hotmenu position modificators
#define WKITEMPOSITION_MACRO        0x00000040
#define WKITEMPOSITION_TRAY         0x10000000
#define WKITEMPOSITION_HOTMENU      0x20000000
#define WKITEMPOSITION_DBLHOTMENU   0x40000000
// Actions enum
// This is a list of standalone actions,
// that can be called by hotkey press
// Plugin can call them by calling 
// corresponding method from callback object
#define HOT_MENU            0
#define HIDE_TOTRAY         1
#define BRING_INTOVIEW      2
#define SWITCH_ONTOP        3
#define CONVERT_SELECTED    4
#define TAKE_SCREENSHOT     5
#define RUN_ASCOMMAND       6
#define HIDE_COMPLIT        7
#define RUN_SCREENSAVER     8
#define FAST_SHUTDOWN       9
#define VOLUME_UP           10
#define VOLUME_DOWN         11
#define RUN_DEFBROWSER      12
#define RUN_DEFEMAILCL      13
#define BOSS_KEY            14
#define OPEN_CD             15
#define WINAMP_NEXTS        16
#define WINAMP_PREVS        17
#define WINAMP_STOP         18
#define WINAMP_PLAY         19
#define CHANGE_CASE         20
#define WINDOW_RESTORE      21
#define WINDOW_MINIMIZE     22
#define SOUND_MUTE          23
#define LOCK_STATION        24
#define LAST_HIDDEN         25
#define ROLLUP_TOTITLE      26
#define WINAMP_NEXT5S       27
#define WINAMP_PREV5S       28
#define HIDE_TOFLOAT        29
#define SCROLL_UP           30
#define SCROLL_DOWN         31
#define SCROLL_LEFT         32
#define SCROLL_RIGHT        33
#define SHOW_OPTIONS        34
#define FREE_MEMORY         35
#define KILL_PROCLIST       36
#define KILL_ACTWND         37
#define WINAMP_SHOWTITL     38
#define WINAMP_VOLUP        39
#define WINAMP_VOLDOWN      40
#define WINAMP_SHUFFLING    41
#define ADDDESK_PREV	    42
#define ADDDESK_NEXT	    43
#define SHOWCLOCKS		    44
#define SWITCHLAYOUT	    45
#define SHOWPROCLIST	    46
#define CLEARHISTORIES	    47
#define SCROLL_PAGEUP	    48
#define SCROLL_PAGEDN	    49
#define ICONS_SAVE	        50
#define ICONS_RESTORE	    51
#define MONITOR_STANDBY	    52
#define WINAMP_SHOWHIDE	    53
#define CONVERT_LASTWORD    54
#define WINDOW_TRANS        55
#define EXTREM_SHUTD        56
#define HOTSEL_MENU         57
#define WINAMP_VISPLUGIN    58
#define DIALOG_SHUTDOWN     59
#define WS_SHOWSTART		60
#define WS_SYSPROPS			61
#define WS_SHOWDESK			62
#define WS_MINIMALL			63
#define WS_SHOWMYCOMP		64
#define WS_SEARCHFILE		65
#define WS_SEARCHCOMP		66
#define WS_SHOWWHELP		67
#define WS_LOCKKEYB			68
#define WS_SHOWRUND			69
#define WS_SHOWUTILM		70

// Total
#define LAST_ACTION_CODE    70

// WireKeys options that can be retrieved via getOption function
// Extended hotkeys processing
#define WKOPT_EXTHKPROC		0x0001
#define WKOPT_ISSHUTDOWN	0x0002
#define WKOPT_AUTOTEXT		0x0003
#define WKOPT_WKINITIALIZED	0x0004

// Events that can be monitored by plugins using
// SubmitToSystemEventNotification function
#define WKSYSEVENTTYPE_MOUSEMOVE	1
#define WKSYSEVENTTYPE_CHAR			2
#define WKSYSEVENTTYPE_REBUILDHK	3
#define WKSYSEVENTTYPE_KEYPRESS		4
#define WKSYSEVENTTYPE_KEYUNPRESS	5
#define WKSYSEVENTTYPE_COPYTOCLIPB	6
#define WKSYSEVENTTYPE_COPYCLIPIMG	7

// Callback functions definitions and structs
struct CWKSYSEvent
{
	CWKSYSEvent()
	{
		memset(this,0,sizeof(CWKSYSEvent));
		cbSize=sizeof(CWKSYSEvent);
	}
	size_t cbSize;
	int iEventType;
	int iCurPosX;
	int iCurPosY;
	LPVOID pData;
};
struct WKPluginFunctionStuff;
typedef int (WINAPI *FPONEVENTADD)(char const* szEventID);
typedef int (WINAPI *FPONEVENTREMOVE)(char const* szEventID);
typedef int (WINAPI *FPONSYSTEMEVENT)(CWKSYSEvent* pEvent);
//--------------------------------------------
// Struct definitions
//--------------------------------------------
//  Class with special WireKeys operations
//  such as get/put selected text and so on
class WKCallbackInterface
{
public:

    // Get selected text
    virtual size_t GetSelectedText(char* szText, size_t dwTextSize)=0;

    // Put text in active window.
    virtual BOOL PutText(const char* szText)=0;

    // Get count of the additional clips
    virtual DWORD GetClipCount()=0;

    // Get additional clipboard content
    virtual size_t GetClipContent(DWORD dwClipNum, char* szText, size_t dwTextSize)=0;

    // Set additional clipboard content. if ((int)dwClipNum<0) - it will set standart clipboard value
    virtual BOOL SetClipContent(DWORD dwClipNum, const char* szContent)=0;

    // Show alert with text and title
    virtual BOOL ShowAlert(const char* szText, const char* szTitle)=0;

    // Get WireKeys preferences folder
    // Plugin must save his options into this folder
    virtual size_t GetPreferencesFolder(char* szText, size_t dwTextSize)=0;

    // Get text in current language. If string is not present
    // the langfile, string will be automatically added
	// szText must be szTextSize in size (at least)
    virtual size_t GetTranslation(const char* szToTranslate,char* szText, size_t dwTextSize)=0;

    // Operation over window: hide to tray or completely, bring into view, etc
    // iOperation==0: Bring into view
    // iOperation==1: Switch On-top state
    // iOperation==2: Hide to tray
    // iOperation==3: Hide completely
    // iOperation==4: Maximize/Restore
    // iOperation==5: Minimize
    // iOperation==6: Maximize
    // iOperation==7: Rollup (title bar and app. menu)/Unroll
    // iOperation==8: Rollup (title bar only)/Unroll
	// iOperation==9: Hide detached
	// iOperation==10: Close window
	// iOperation==11: Create shortcut to app
	// iOperation==12: Kill app
    virtual BOOL ProcessWindow(HWND hWnd, int iOperation)=0;

    // Show WireKeys-generated menu
    virtual BOOL ShowWKMenu(HWND hWnd)=0;

    // Get WireKeys version (for example "1.9.7")
    virtual BOOL GetWKVersion(char szVersion[32])=0;

    // Return current WK state
    // 0 - everything is ok
    // 1 - tray menu opened
    // 2 - hot menu opened
    // 3 - preferences opened
    virtual int GetWKState()=0;

    // Return WireKeys main window
    virtual HWND GetWKMainWindow()=0;

    // Action hotkey, basic hotkeys only (no sequence hotkeys, no multi character hotkeys)
    virtual DWORD GetWKActionHKCode(int iAction)=0;

    // Action hotkey description
    virtual BOOL GetWKActionHKDesc(int iAction, char szAsString[32])=0;
    
    // Call WireKeys Action
    virtual BOOL CallWKAction(int iAction)=0;

    // Show WireKeys-generated application-info
    virtual BOOL ShowAppInfo(HWND hWnd)=0;

    // Show tray-icon popup with text and title
    virtual BOOL ShowPopup(const char* szText, const char* szTitle, DWORD dwTimeOut)=0;

    // Returns Plugin directory
    virtual size_t GetWKPluginsDirectory(char* szPath, size_t dwPathSize)=0;

    // Add qrun item from HWnd
    virtual BOOL AddQrunFromHWND(HWND hwnd)=0;

    // Allows plugin to set options back into WireKeys. When this function is called,
    // WireKeys will call (in reply) plugin`s function WKPluginOptionsManager
    // with 'OM_OPTIONS_GET' as first parameter and dwReason as second parameter
    virtual BOOL PushMyOptions(DWORD dwReason)=0;

	// v2.1.0
	// Substitute patterns in szValue, result will be copied back into szValue
	// To detect 'buffer-overflow' case, use return size_t value:
	// if result is bigger that length of szValue, than substituted string was truncated
	virtual size_t SubstPatterns(char* szValue, size_t dwValueSize)=0;

	// v2.1.5
	// Show plugin-defined OSD string, dwTimeToShow - in milliseconds
	virtual BOOL ShowOSD(const char* szText,DWORD dwTimeToShow)=0;

	// Starts quick-run item with specified name. szAdditionalParameter can be null 
	// (in fact it`s value will be added to the actual parameters of the quick-run item)
	virtual BOOL StartQuickRunByName(const char* szQRunTitle, const char* szAdditionalParameter)=0;

	// v2.3
	// Add qrun item from file (if this file has 'wkm' extension, macro will be added)
    virtual BOOL AddQrunFromFile(const char* szQRunFile)=0;

	// v2.6
	// Register hotkey-like event. Registered event can be added to any hotkey via hotkey`s context menu
	// Corresponding submenu will be named from szEventClassName
	// szEventID must be unique through all plugins. It is recommended to use 2 first characters of szEventID as abbreviation of plugin
	virtual BOOL RegisterEvent(char szEventID[5], char szShortDescription[64], char szEventClassName[64])=0;
	
	// Unregister hotkey-like event by it's ID
	virtual BOOL UnRegisterEvent(char szEventID[5])=0;

	// Plugin must call this function when plugin-defined event is started
	// For example Remote Control plugin have to call this function when users press remote key
	virtual BOOL NotifyEventStarted(char szEventID[5])=0;

	// Plugin must call this function when plugin-defined event is finished
	// For example Remote Control plugin have to call this function when users unpress remote key
	virtual BOOL NotifyEventFinished(char szEventID[5])=0;

	// This function will wait until user releases last pressed hotkey
	virtual BOOL WaitWhileAllKeysAreReleased()=0;

	// v2.6.3
	virtual BOOL getOption(DWORD dwOption)=0;

	// v2.7
	virtual BOOL ChangeItemIcon(const char* szItemPath)=0;
	virtual BOOL triggerTweak(DWORD dwOption)=0;

	// v2.7.1
	virtual BOOL ShowBaloon(const char* szText,const char* szTitle,DWORD dwTimeToShow)=0;

	// v2.7.7
	// iSoundType==0 Error sound
	virtual BOOL Sound(int iSoundType)=0;

	// v2.8.1
	virtual BOOL ShowPluginPrefernces()=0;

	// v2.8.2
	// Return TRUE if one of the started application match the mask. Test is case-insensitive!
	virtual BOOL TestRunningApplicationsByMask(const char* szMask)=0;

	// v3.0
	// Registering callback for adding/removing events/hotkeys to/from WK hotkey (in preferences)
	virtual BOOL RegisterOnAddRemoveEventCallback(FPONEVENTADD fpAdd,FPONEVENTREMOVE fpRemove)=0;

	// Registering callback for different system events like keypresses and mouse movemens
	// Warning: if any of callbacks will work more that 1 sec, all callbacks will be disabled
	// If you should do lengthy processing here, do them in separate thread please
	virtual BOOL SubmitToSystemEventNotification(int iEventType, FPONSYSTEMEVENT fp)=0;

	// UnRegister previous registered callback for different system events
	// It is important to unregister callbacks when you don`t need them, because 
	// callback execution ofr certain events can cause great overhead on CPU usage!!!
	virtual BOOL UnSubmitToSystemEventNotification(int iEventType, FPONSYSTEMEVENT fp)=0;

	// Notify about system event. Plugin can notify WK about various system event (and all subscribed plugins will receive callback calls).
	// You are free to delete pEvent after call returns
	virtual BOOL NotifyAboutSystemEvent(CWKSYSEvent* pEvent)=0;

	// Something special can be returned
	virtual LPVOID getOptionEx(DWORD dwOption,DWORD dwParameter)=0;
	virtual LPVOID setOptionEx(DWORD dwOption,DWORD dwParameter)=0;

	// Correctly return keyboard focus to active application
	virtual void ReturnKeyboardFocusToActiveApp(WKPluginFunctionStuff*)=0;

	// v3.3.4
	// Get named setting from registry (per user)
	virtual BOOL GetRegSetting(const char* szRegSettingsName, char* szOut, size_t dwOutSize)=0;

	// Set named setting to registry (per user)
	virtual BOOL SetRegSetting(const char* szRegSettingsName, char* szOut)=0;

	// v3.3.5
	// Plays sound according to WK settings
	virtual BOOL PlayWavSound(const char* szWav)=0;

	// v3.4.0
	// Starts quick-run item with specified name, parameter and substitution to "selected" text
	virtual BOOL StartQuickRunByNameX(const char* szQRunTitle, const char* szAdditionalParameter, const char* szSelectionSubstitution, int iEntryNum)=0;

	// Call WireKeys Action with parameter
    virtual BOOL CallWKActionX(int iAction, const char* szParameter)=0;
    
    // v3.4.3
    // Pause WireKeys completely (with all hooks, etc)
    virtual BOOL PauseWKCompletely(BOOL bPause)=0;
    
    // Opens main WK`s window on specified topic
    virtual BOOL ShowPreferencesTopic(const char* szTitle_EN)=0;
    
    // v3.5
    // Returns handle to plugin`s dll
    virtual HINSTANCE GetPluginDllInstance()=0;

	// Save current screenshot to specified file
	virtual void SaveScreenshotTo(char* szPath, int iType)=0;

	// v3.6
	// Get MD5 of the byte stream
	virtual void getMD5(char* szOut, BYTE* pFrom, size_t size)=0;

	// v3.6.4
	// Executes script by its text
	virtual BOOL ExecuteScript(const char* szText)=0;

	// translation from WireKeys terms file
	virtual size_t GetBaseTranslation(const char* szToTranslate,char* szText, size_t dwTextSize)=0;
};

#define OL_HOME                 0
#define OL_ADDOPTIONS           1
#define OL_COMMONOPT            2
#define OL_ADDCLIPS             3
#define OL_SELTEXTOPERATIONS    4
#define OL_QUICKRUNS            5
#define OL_MACROS               6
#define OL_WINAMP               7
#define OL_CDEJECT              8
#define OL_VOLUME               9
#define OL_QUICKSHUTDOWN        10
#define OL_PROCLIST             11
#define OL_BOSSKEY              12
#define OL_SCRSHOTS             13
#define OL_ACTIVEWND            14
#define OL_HIDDENAPPS           15
#define OL_ADDDESKTOPS          16
#define OL_DESKICONS            17
#define OL_SCRSAVER             18
#define OL_KEYBOARD             19
#define OL_HIDDEN               20
#define OL_EXPLORERHK           21
// This class is used to add/get/set options into WireKeys preferences dialog (into any subfolder, defined in OL_* enum)
// Every plugin-driven option must have unique name and plugin can get/set corresponding option using this name
// Important: WireKeys will handle plugin`s options by himself! 
// Plugin don`t need to save/restore them, this will be done automatically!
class WKOptionsCallbackInterface
{
public:
    // -------------
    // Add section
    // Note, that dwOptionPosition is the parameter determining location of the option (OL_*)
    // I.e. subfolder in the preferences dialog. If dwOptionPosition==0 then option will be placed in the plugin`s "home" subfolder
    // Note, that using this functions you can overwrite previously added options by adding them again (i.e. with the same name)
    // -------------
    // Yes/No option
    virtual BOOL AddBoolOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long bDefaultValue=TRUE, DWORD dwOptionPosition=0)=0;
    // String option. if dwHint==1 this option will be powered with "Browse for file" dialog. if dwHint==2 this option will be powered with "Browse for folder" dialog. if dwHint==3, item will be used as color picker
    virtual BOOL AddStringOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szDefaultValue="", DWORD dwHint=0, DWORD dwOptionPosition=0)=0;
    // Number option. You can define Min/Max values
    virtual BOOL AddNumberOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long lDefaultValue=0, long lMinValue=-999999, long lMaxValue=999999, DWORD dwOptionPosition=0)=0;
    // ListBox option. szTabDelimitedList is the string with tabs between list lines, for example "First line\tSecond line\tThird line". lDefaultPosition is the zero-based start-up position in the list
    virtual BOOL AddListOption(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szTabDelimitedList="", long lDefaultPosition=0, DWORD dwOptionPosition=0)=0;
    // Action option - simple button. When user clicks the button, WKCallPluginFunction will be called with corresponding function number
    // If iPluginFunction<0, plugin options will be called (instead of external function)
    virtual BOOL AddActionOption(const char* szOptionName, const char* szOptionTitle, long iPluginFunction, DWORD dwOptionPosition=0)=0;
    // -------------
    // Get section
    // -------------
    // Yes/No option
    virtual long GetBoolOption(const char* szOptionName)=0;
    // String option. Result is the total lenght of the corresponfing option.
    virtual size_t GetStringOption(const char* szOptionName, char* szOptionBuffer, size_t dwOptionBufferSize)=0;
    // Number option.
    virtual long GetNumberOption(const char* szOptionName)=0;
    // ListBox option.
    virtual long GetListOption(const char* szOptionName)=0;
    // -------------
    // Set section
    // -------------
    // Yes/No option
    virtual BOOL SetBoolOption(const char* szOptionName, long lNewValue)=0;
    // String option.
    virtual BOOL SetStringOption(const char* szOptionName, const char* szNewValue)=0;
    // Number option.
    virtual BOOL SetNumberOption(const char* szOptionName, long lNewValue)=0;
    // ListBox option.
    virtual BOOL SetListOption(const char* szOptionName, long lNewValue)=0;
    // -------------
    // Other
    // -------------
    // Remove named option (of any type)
    virtual BOOL RemoveOption(const char* szOptionName)=0;
    // -------------
    // Additionally
    // -------------
	// Any binary data
	virtual BOOL AddBinOption(const char* szOptionName)=0;
	// Any binary data
	virtual long GetBinOption(const char* szOptionName, LPVOID pData, DWORD dwDataSize)=0;
	// Any binary data
	virtual BOOL SetBinOption(const char* szOptionName, LPVOID pData, DWORD dwDataSize)=0;
	// Adds tip to plugin`s preferences
	virtual BOOL AddTip(const char* szOptionName, const char* szTip, DWORD dwOptionPosition)=0;
	// -------------
    // Additionally - 2, for hierarhic menus
	// To make hierarchy, szParent must contain szOptionName of the parent item
    // -------------
    // Yes/No option
    virtual int AddBoolOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long bDefaultValue=TRUE, DWORD dwOptionPosition=0, const char* szParent=0)=0;
    // String option. if dwHint==1 this option will be powered with "Browse for file" dialog. if dwHint==2 this option will be powered with "Browse for folder" dialog
    virtual int AddStringOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szDefaultValue="", DWORD dwHint=0, DWORD dwOptionPosition=0, const char* szParent=0)=0;
    // Number option. You can define Min/Max values
    virtual int AddNumberOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, long lDefaultValue=0, long lMinValue=-999999, long lMaxValue=999999, DWORD dwOptionPosition=0, const char* szParent=0)=0;
    // ListBox option. szTabDelimitedList is the string with tabs between list lines, for example "First line\tSecond line\tThird line". lDefaultPosition is the zero-based start-up position in the list
    virtual int AddListOptionEx(const char* szOptionName, const char* szOptionTitle, const char* szOptionDescription, const char* szTabDelimitedList="", long lDefaultPosition=0, DWORD dwOptionPosition=0, const char* szParent=0)=0;
    // Action option - simple button. When user clicks the button, WKCallPluginFunction will be called with corresponding function number
    // If iPluginFunction<0, plugin options will be called (instead of external function)
    virtual int AddActionOptionEx(const char* szOptionName, const char* szOptionTitle, long iPluginFunction, DWORD dwOptionPosition=0, const char* szParent=0)=0;
	// Adds tip to plugin`s preferences
	virtual int AddTipEx(const char* szOptionName, const char* szTip, DWORD dwOptionPosition, const char* szParent=0)=0;
	// Add action hotkey. Works for actions with WKITEMPOSITION_OPTFLOATS flag set
	virtual int AddActionHotkey(const char* szOptionName, const char* szActionTitle, const char* szOptionDescription, DWORD dwActionNumber, DWORD dwOptionPosition=0, const char* szParent=0)=0;
	// Add color picker
	virtual int AddColorOption(const char* szOptionName, const char* szActionTitle, const char* szOptionDescription, DWORD dwDefaultColor, DWORD dwOptionPosition=0, const char* szParent=0)=0;
	
};

// Struct with common plugin parameters
struct WKPluginDsc
{
    WKPluginDsc()
    {
        cbsize=sizeof(WKPluginDsc);
        bLoadPluginByDefault=0;
        szTitle[0]=0;
        szDesk[0]=0;
        hPluginBmp=NULL;
        bResidentPlugin=FALSE;
		szAuthors[0]=0;
		dwVersionHi=0;
		dwVersionLo=0;
    };
    // Size of this structure
    size_t                  cbsize;
    //Plugin name/title
    char                    szTitle[WKPLUGTITLE_SIZE];
    //Load plugin by default (Warning: ignored from v3.5.0)
    BOOL                    bLoadPluginByDefault;
    //Plugin short description
    char                    szDesk[WKPLUGTITLE_SIZE];
    // Icon for Preferences menu, NULL for none. WireKeys will copy this bitmap into internal buffer
    HBITMAP                 hPluginBmp;
    // "Resident" flag
    // If TRUE, plugin will be loaded at WK startup and unloaded on WK exit
    // If FALSE, plugin will be loaded for every single call and then unloaded
    BOOL                    bResidentPlugin;
	// Authors of this plugin
	char					szAuthors[WKPLUGTITLE_SIZE];
	// Plugin`s version
	DWORD					dwVersionHi;
	DWORD					dwVersionLo;
};

//  Object of this class must be filled for every
//  Plugin operation. WireKeys will show plugin items
//  according to this structure
struct WKPluginFunctionDsc
{
    WKPluginFunctionDsc()
    {
        cbsize=sizeof(WKPluginFunctionDsc);
        dwItemMenuPosition=WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN;
        dwDefaultHotkey=0;//-1;
        hItemBmp=NULL;
        strcpy(szItemName,"");
		pInternalData=0;
    };
    // Size of this structure
    size_t                  cbsize;
    // Icon for tray menu, NULL for none. WireKeys will copy this bitmap
    HBITMAP                 hItemBmp;
    // Text to be displayed in the tray menu item (can be overwriten later)
    char                    szItemName[WKITEMDSC_SIZE];
    // Default hotkey, 0 for none, DWORD(-1) if hotkey is not needed at all
    long                    dwDefaultHotkey;
    // Bitmask for tray submenus, where item must appear. Must contain WKITEMPOSITION_TRAY or WKITEMPOSITION_HOTMENU. Default WKITEMPOSITION_TRAY|WKITEMPOSITION_PLUGIN
    DWORD                   dwItemMenuPosition;
	// Reserved (must be NULL)
	void*					pInternalData;
};

// Object of this class must be filled for every
// Plugin function`s item and must define properties 
// of the corresponding menu item
struct WKPluginFunctionActualDsc
{
    WKPluginFunctionActualDsc()
    {
        bVisible=TRUE;
        bDisabled=bChecked=bGrayedout=FALSE;
        szNonDefaultItemText[0]=0;
    };
    BOOL bVisible;
    BOOL bDisabled;
    BOOL bChecked;
    BOOL bGrayedout;
    char szNonDefaultItemText[32];
};

// This struct will be filled for every call to any plugin operation
struct WKPluginFunctionStuff
{
    WKPluginFunctionStuff()
    {
        szError[0]=0;
        cbsize=sizeof(WKPluginFunctionStuff);
        hCurrentFocusWnd=NULL;
        hMainWKWindow=NULL;
        wireKeysInt=0;
		pSpecialHotkeyDsc=NULL;
		hForegroundWindow=NULL;
		bOnUnpress=0;
    };
    // Size of this structure
    size_t                  cbsize;
    // Error message, WireKeys alert this error if needed
    char                    szError[WKERRORMSG_SIZE];
    // Pointer to object, which can be used to call special WireKeys operations
    WKCallbackInterface*    wireKeysInt;
    // Handle to the window with focus, may be NULL
    HWND                    hCurrentFocusWnd;
    // Handle to main WireKeys window to use as parent for plugin dialog
    HWND                    hMainWKWindow;
	// Additional pointer to hotkey
	void*					pSpecialHotkeyDsc;
	// Handle to the foreground window
    HWND                    hForegroundWindow;
	// Notification if the action were raised by UnperssingHotkey
	BOOL					bOnUnpress;
};

//--------------------------------------------
// Functions definitions. 
// All required functions MUST be exported (throught .DEF file)
//--------------------------------------------
/*
    Function must fill szTitle with Plugin name
    and some other options. Result is ignored
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINCOMDSCFUNCTION)(WKPluginDsc*);
int WINAPI GetPluginDsc(WKPluginDsc* stuff);

/*
    This function will be called when plugin is loaded
    MUST NOT invlolve any operations with long duration
    (use WKPluginInit for such initializations)
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINSTARTFUNCTION)();
int WINAPI WKPluginStart();

/*
    This function will be called for resident plugin at plugin load,
    but in secondary thread. (WKCallbackInterface*) pointer 
    can be saved for later use. This method may involve 
    long-duration operations and initializations
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGININITFUNCTION)(WKCallbackInterface*);
int WINAPI WKPluginInit(WKCallbackInterface*);

/*
    This function will be called when plugin is unloaded
    MUST NOT invlolve any oprations with long duration
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINSTOPFUNCTION)();
int WINAPI WKPluginStop();

/*
    This function will be called when plugin is paused
    or unpaused. This may happen when user switches to
    another desktop (in which case second copy of WireKeys 
    will be started). If this function is absent, plugin will not be loaded
	in second desktops
    Plugin must _not_ do something in paused mode,
    must turn off all hooks and background processing
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINPAUSEFUNCTION)(BOOL);
int WINAPI WKPluginPause(BOOL bPaused);


/*
    This function will be called when plugin is paused
    or unpaused. This may happen when user switches to
    another desktop (in wich second copy of WireKeys 
    will be started).
    Plugin must _not_ do something in paused mode,
    must turn off all hooks and background processing
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINDSWITCHFUNCTION)(BOOL);
int WINAPI WKDesktopSwitching(BOOL bMovingToAnotherDesktop);

/*
	This function called every time user opens tray/hot menu with iActionType=TM_ADDCUSTOMITEM.
	Using this method you can add custom submenus/items to WK menu
	When user clicks on your item, WK will call this method again with 
	proper iActionType(TM_HANDLECUSTOMITEM). iItemId will contain choosen item in this case.
	In case of TM_ADDCUSTOMITEM you should set item`s ID in for of iItemId+0, iItemId+1, ... 
	and set iItemId above the code of the last item you added
*/
#define TM_ADDCUSTOMITEM  0
#define TM_HANDLECUSTOMITEM  1
typedef int (WINAPI *FPLUGINDPOPUPMENUFUNCTION)(int, int&, HMENU);
int WINAPI WKTrayMenuPopup(int iActionType, int& iItemId, HMENU hMenu);

/*
    This function will be called when user wants to change Plugin preferences
    Result must be 1 if everithing is OK
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINOPTIONSFUNCTION)(HWND);
int WINAPI WKPluginOptions(HWND hParent=NULL);

/*
    This function can be called in 3 cases:
    1) When plugin is loaded at first time, this function will be called with 'OM_STARTUP_ADD' as first parameter
    and plugin can build his options into WireKeys preferences using callback pointer
    2) When user finished to change WireKeys settings, this function will be called with 'OM_OPTIONS_SET' as first parameter
    and plugin can retrieve his options (installed previously in section (1) using callback pointer and change behaviour/internal structures accordingly
    3) Sometimes plugin may want to push options back into WireKeys (for example when user change some of the through plugin`s dialogs)
    In this case plugin can call "PushMyOptions" from WireKeys callback object (look at WKPluginInit) and WKPluginOptionsManager will be called with 'OM_OPTIONS_GET' as first parameter
    dwParameter is equals to zero in most cases, except calls from 'PushMyOptions' function (in this case 'pushMyOptions' will be trasferred to dwParameter)
    Result must be 1 if everithing is OK (but result is ignored for this time)
    -------------------------------------------------
    This is not required function
*/
#define OM_STARTUP_ADD  0
#define OM_OPTIONS_SET  1
#define OM_OPTIONS_GET  2
typedef int (WINAPI *FPLUGINOPTMANSFUNCTION)(int, WKOptionsCallbackInterface*,DWORD);
int WINAPI WKPluginOptionsManager(int, WKOptionsCallbackInterface*, DWORD dwParameter);

/*
    WireKeys detect plugins by presence of this function
    At start up WireKeys will call this function 
    with iPluginFunction from 0 to '...' while WKGetPluginFunctionDescription(..)!=0
    Plugin must fill WKPluginFunctionDsc for every supported function
    Result must be:
    -------------------------------------------------
    0   Function with this iPluginFunction is not supported by plugin
    1   Function with this iPluginFunction supported by plugin
    -------------------------------------------------
    This is required function
*/
typedef int (WINAPI *FPLUGINDSCFUNCTION)(long, WKPluginFunctionDsc*);
int WINAPI WKGetPluginFunctionCommonDesc(long iPluginFunction, WKPluginFunctionDsc* stuff);

/*
    This function will be called for every plugin menu item,
    So Plugin can temporaly disable/gray out/check/uncheck
    his own menu items at run-time (just before they will be shown)
    Result must be 1.
    -------------------------------------------------
    This is not required function
*/
typedef int (WINAPI *FPLUGINACTDSCFUNCTION)(long, WKPluginFunctionActualDsc*);
int WINAPI WKGetPluginFunctionActualDesc(long iPluginFunction, WKPluginFunctionActualDsc* stuff);

/*
    This function will be called from WireKeys 
    when user will choose appropriate menu item/hotkey
    -------------------------------------------------
    Result must be:
    0   Error occured, If szError[0]!=NULL, alert will be shown to the user
    >0  Everything OK
    <0  Reserved
    -------------------------------------------------
    This is required function
*/
typedef int (WINAPI *FPLUGINCALLFUNCTION)(long, WKPluginFunctionStuff*);
int WINAPI WKCallPluginFunction(long iPluginFunction, WKPluginFunctionStuff* stuff=NULL);

/*
	This is prototype of the function that can be called from JS/VB macros
	using WireKeys.CallDLL method
*/
typedef int (WINAPI *FPLUGINEXTFUNCTION)(const char* szParameters_in, char* szResult_out, size_t dwResultSize_in);

// Some special additional functions
// This function intended to be used in system-wide hooks
// It is important to skip hooks when isWKUpAndRunning() returns FALSE
// otherwise you can expirience weird behaviour in secondary desktops 
// or after switching from them!
#include <stdio.h>
class WKUtils
{
public:
	static BOOL isWKUpAndRunning()
	{
		return TRUE;
		/*Old code... may be useful may be not
		DWORD d=GetLastError();
		// This search is 1.5 faster 
		// then isWKUpAndRunningByAtom() (GetUserObjectInformation+GlobalFindAtom)
		HWND hMain=FindWindow("WK_MAIN",0);
		SetLastError(d);
		return (hMain!=0)?TRUE:FALSE;
		*/
	}

	static BOOL isWKUpAndRunningByAtom()
	{
		return TRUE;
		/*Old code... may be useful may be not
		DWORD d=GetLastError();
		char s[64]={0};
		GetCurrentDesktopAtom(s);
		ATOM h=GlobalFindAtom(s);
		SetLastError(d);
		return (h!=0)?TRUE:FALSE;
		*/
	}


	static void GetCurrentDesktopAtom(char s[64])
	{
		s[0]='W';
		s[1]='p';
		s[2]='D';
		DWORD dwLen=0;
		DWORD dwErr=GetUserObjectInformation(GetThreadDesktop(GetCurrentThreadId()), UOI_NAME, s+3, sizeof(s)-4, &dwLen);
	}

 };

#endif
//</pre>
