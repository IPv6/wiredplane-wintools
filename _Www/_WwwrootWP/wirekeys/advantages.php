<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "Управление системой с помощью гор. клавиш, полная автоматизация работы за компьютером";
	}
	return "Automation tool for power users, Control your PC using hotkeys";
}
function insertSection($l){
?>
<?php if($l==1){?>
Эта программа из класса "Все-в-одном", и способна заменить собой целый полк 
утилит установленных на Вашем компьютере. В отличие от других программ подобного направления, 
WireKeys вобрала в себя не бессвязный набор возможностей, а так или иначе используемые, но не 
реализованные в операционной системе, функциональности. Также, в отличии от других менеджеров клавиатру, 
возможности WireKeys взаимодействуют друг с другом и дополняют друг друга. К примеру поддержка 
мультимедийных клавиатур распространяется на все горячие клавиши, которые можно задать через WireKeys.
Кроме того в этой программе реализовано несколько идей, <b>упрощающих</b> работу с клавиатурой. 
К примеру многие действия допускают двойное нажатие на вызвающую их горячую клавишу - при этом действие 
будет выполнено с альтернативными настройками. Если при нажатии на кнопку снятия скриншота по-умолчанию 
изображение сохраняется в определенную папку, то при двойном нажатии на туже горячую клавишу изображение будет 
дополнительно открыто в графическом редакторе или сохранено в другой каталог или передано в буфер обмена.
<?php }else{?>
WireKeys is all-inclusive MS Windows enhancement software allowing PC 
users take full control over the operating system interface and performance. 
WireKeys adds unlimited additional clipboards as well as additional virtual 
desktops for uncluttered data representation. Besides that, the application 
includes inline calculator capable of calculating math formulae from '2+2=' 
to complex JS-syntax expressions, text case and codepage\layout changer and 
windows management (including window opacity, automatic hiding to TaskTray, 
setting process priority and killing a process). With WireKeys it is easy 
to preserve icons positions on desktop, set the desired sound volume system-wide, 
restrict access to a PC as well as turn it on\off and optimize memory usage. 
<p>Additional tools allow for CD\DVD drive(s) control, screen resolution settings 
adjusting and processes management. WireKeys makes it easy to record keystrokes that you frequently use. Once you have recorded your frequently-typed
information, WireKeys makes it quick and easy to fill out forms on the web, enter logins and passwords, and send
routine notes to customers and colleagues.
<p>WireKeys is intended for a broad range of Windows users who would like to take 
full control over operating system casual and hidden features by using customizable 
keyboard shortcuts and context menus. WireKeys is available in English, Spanish, 
Italian, Russian and Ukrainian. The application functionality can be further 
expanded by installing plug-ins available both from WiredPlane Labs and third 
party developers.
<?php }?>
<?php if($l==1){?>
<br>
<u>Эта программа предназначена для всех кто:</u>
<br>- имеет мультимедийную клавиатуру, но стандартные программы не позволяют использовать ее возможности в полной мере;
<br>- занимается за компьютером несколькими делами одновременно;
<br>- часто работает с большим количеством окон на одном экране;
<br>- часто компанует документы из множества различных источников в различных приложениях;
<br>- помногу и однотипным образом обрабатывает файлы и тексты документов;
<br>- желает просто и быстро автоматизировать свою работу с помощью легко доступных и понятных средств - используя универсальные макросы;
<br>- устал от утилит разряда "one-trick pony". Одновременное использование множества таких утилит сильно захламляет рабочее место и вместо экономии времени затрудняет их использование;
<br>- хочет использовать нестандартные комбинации для горячих клавиш. Пример: раздельно правые и левые клавиши-модификаторы (например назначив на 'Left Alt-O' одно действие и на 'Right Alt-O' - другое)
<?php }else{?>
<p>The all-in-one architecture of this software not only 
means better integrity but also allows for cutting additional software investments 
at least twice.
<?php }?>
<hr>
<table border=0 width=100% class="topicback"><tr><td align= align=left>&nbsp;<b><b>Key Features:</b>: <a href="#lnc">Launcher,</a> <a href="#mac">Macros,</a> <a href="#txt">Text processings,</a> <a href="#win">Windows,</a> <a href="#sys">Miscellaneous</a></b></td></tr></table>
<table width="100%" border="0" cellpadding="3" cellspacing="0">
<tr>
	<td bgcolor="#EEEEEE" colspan=2>
		<a name="lnc"><u><b>Unique characteristics:</b></u></a><br>
		<lu>
		<li>Full support for all popular multimedia keyboards 
		<li>Full support for TV tuner remote controls (<a href="http://winlirc.sourceforge.net/">WinLIRC</a> is supported as well as AverMedia remotes)
		<li>Tracking of all recently installed applications for quick access to their uninstall programs
		<li>Tray icon can show CPU/memory usage as a histogram
		<li>Automatically eject a CD at shutdown
		<li>Control the system sound volume (0% to 100%)
		<li>Change your screen resolution in one click 
		<li>Hide the mouse cursor when typing text or scrolling any window using the mouse wheel (requires
plugin)
		<li>Advanced WinAmp (2.x, 3, 5) support. WireKeys can automatically pause WinAmp when you lock your
workstation and resume it when you unlock. WireKeys can show the composition "Now playing" in the
onscreen display.
		</lu>
	</td>
</tr>
<tr>
	<td bgcolor="#EEEEEE" colspan=2>
		<a name="lnc"><u><b>Launcher. With the WireKeys Launcher, you can...</b></u></a><br>
		<lu>
		<li>WireKeys lets you execute any program by pressing a hotkey and specify the window's position or minimize or
maximize it after start. With a single keystroke, you can launch your web browser
and visit your favorite web site. By letting you run programs with hotkeys, WireKeys saves your desktop from icon
clutter, and lets you hide icons from your coworkers' prying eyes, because only you know the hotkey.
		<li>Run and enable/disable your screensaver. Before running the screensaver, WireKeys can
automatically stop WinAmp or shuffle the current saver.
		<li>Quickly run the Registry editor, Services panel, and other items from Control panel (available
in Windows NT/2000/XP/2003 only).
		<li>Add quick-run from any window's hot menu or the system menu.
		<li>Add special icons to your tray or a floater to your display to quickly launch your favorite
applications.
		<li>Record keystrokes to replay them later by hotkey (requires plugin).
		<li>Use any one of five methods to add hotkeys to an application without starting the program
itself. 
		<li>List and expand your favorite folders directly from the tray. WireKeys can filter folder
content and show matched files only. Right-clicking a file in the tray menu will popup Explorer's
context menu for the file. Your favorite folder can be converted into A convenient floater. You
can simply drag-and-drop files onto the folder's floater window to copy them into the folder.
		</lu>
	</td>
</tr>
<tr>
	<td bgcolor="#EEEEEE" colspan=2>
		<a name="mac"><u><b>JScript and VBScript macros:</b></u></a><br>
		<lu>
		<li>WireKeys makes it easy to record keystrokes that you frequently use. Once you have recorded your frequently-typed
information, WireKeys makes it quick and easy to fill out forms on the web, enter logins and passwords, and send
routine notes to customers and colleagues.
		<li>Create and start macros written in JavaScript or VBScript by hotkey. Result of the script execution can be inserted into active window automatically - so its very easy to create your own special text processing for everyday use.</li>
		<li>Record and playback macros will allow you to perform multiple operations (picture editing, applying different actions to the active document in a flash) in one keypress. Macro playback speed is adjustable.</li>
		<li>Application specific macros - the same trigger can start different macro in different applications. This helps to unify computer environment.</li>
		<li>Shared macros allows user groups to share the same actions in corporate network.</li>
		</lu>
	</td>
</tr>

<tr>
	<td bgcolor="#EEEEEE" colspan=2>
		<a name="txt"><u><b>Automated text processing:</b></u></a><br>
		<lu>
		<li>Add history of use for standard clipboard. Now you can always paste text you had sometime before!</li>
		<li>Insert text or text file into active application by hotkey press</li>
		<li>Use inline calculator with full functions support (based on JScript)</li>
		<li><a href="#AST">**</a> Add multiple text clipboards (unlimited clips plus history of use)</li>
		<li><a href="#AST">****</a> WireKeys text autoreplace facility will automatically expand short words or phrases into long text strings.</li>
		<li>Forward selected strings in any application to one specific opened document to quicly collect data from different sources.</li>
		<li>Run selected text as from command string (Run ...). For example type 'Notepad' in any application and press hotkey to start Windows Notepad.</li>
		<li>Convert selected text entered from wrong keyboard layout. For example from Cyrillic to Western</li>
		<li>Quickly sort lines of selected text in any text-based application</li>
		<li>Change case of selected text</li>
		<li>Many more can be done using macros and plugins...</li>
		</lu>
	</td>
</tr>
<tr>
	<td bgcolor="#EEEEEE" colspan=2>
		<a name="win"><u><b>Control over windows:</b></u></a><br>
		<lu>
		<li><a href="#AST">***</a> Minimize any application to tray (with ability to browse application main menu from icon-in-tray menu)</li>
		<li>Quickly move off-screen windows back into view, change window's "OnTop" state ("Pin window"), dock window to screen edge, expand window horizontally or vertically and more.</li>
		<li>Minimize any window into handy floater - small icon on your screen which remembers its position. With this feature don`t have move your mouse to tray to open hidden window</li>
		<li>Make all windows in your system sticky, hide windows to tray by right-clicking minimize window button, add useful items into window`s system menu.</li>
		<li>Automatically hide/minimize/minimize to tray inactive window after specified period of disuse.</li>
		<li>Create shortcut to active application on desktop or Start menu or add hotkey in one click.</li>
		<li>Quick access to all window`s parameters - size, position, start-up information, opened files (even hidden) and more</li>
		<li>Open special customized menu by right-clicking 'Close window' button with all relevant actions</li>
		<li>Close/hide all shady windows by hotkey ('Boss' key or 'Emergency' key).</li>
		<li>Rollup window to title by hotkey or mouse gesture.</li>
		</lu>
	</td>
</tr>
<tr>
	<td bgcolor="#EEEEEE" colspan=2>
		<a name="sys"><u><b>Miscellaneous:</b></u></a><br>
		<lu>
		<li>View started processes and kill non-replying tasks (even system processes if you have admin rights), can be used as quick and fast replacement for "Task manager"</li>
		<li>Make screenshots conveniently (WireKeys can automatically save image into file in bmp or jpg formats). WireKeys can even save screenshots periodically.</li>
		<li><a href="#AST">*</a> Limit screenshot to a specified area - after pressing hotkey just hold mouse button and drag the rectangle of interest.</li>
		<li>Quickly shutdown/restart/logoff/lock your computer. WireKeys can automatically pause WinAmp at lock and unpause it again after unlock.</li>
		<li><a href="#AST">*</a> Switch between multiple desktops (desktop switcher). Each desktop can have separate icons layout, screen resolution and even different shell.</li>
		<li><a href="#AST">****</a> Hide mouse cursor when typing text or scrolling any window using mouse wheel</li>
		<li>Tracking of all installed applications. You don`t have to open slow Windows` 'Installed applications' dialog - just choose one of the recently installed applications from tray menu.</li>
		<li>Control sound volume and CD drive via hotkeys with ease</li>
		<li>Control WinAmp (2.x,3,5) via hotkeys</li>
		<li>Save/restore desktop icon positions (layout)</li>
		<li>Change your screen resolution in one click</li>
		</lu>
	</td>
</tr>
</table>

<br><br><a name="AST">Features marked with:
<br>'*'   Available in Windows NT/2000/XP/2003 only
<br>'**'  Subject to system limits and available storage
<br>'***' Most applications
<br>'****' Via additional plugins
<br><br>
<h2>More detailed description</h2>
<br>
<table>
<tr><td bgcolor="AAAAAA"><b>Additional clipboards + history of use</b></td>
<tr><td>
Helps in work with any application by providing additional text clipboards.
Very convinient for combining dicument from different text pieces, for example.
You will be able to copy and paste selected text to/from clips using hotkeys only.
You can check the content of any clipboard at any time in the hot menu.
Every clipboard have its own history of use. History of use is used to quickly insert one of the recently copied string (but not the last one).
WireKeys add clipboard history for standard clipboard too. Additionaly, you can enable transmit mode
(by choosing 'Transmit mode' in hot menu for target window) - any copy to additional clip will be automatically transmited (pasted) into target window.
Clipboards content/history are saved between shutdowns. You can also pin up pieces in clipboards history. Pinned pieces will not be pushed out from history list by other items. 
To pin up piece just hold SHIFT key while choosing item from history menu
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Screenshots</b></td>
<tr><td>
You can automatically save screenshots in bmp or jpg formats. WireKeys can even save screenshots
periodically. WireKeys gives you the ability to make screenshots of the whole screen or the
active application only or a rectangular area of interest. Press a hotkey, hold the mouse button,
and drag the rectangle. You can also define a different behavior for double-pressing the hotkey.
For example, if you are using 'Alt-F12' as your screenshot key to get the whole display, you can
set the 'Active window only' behavior to the automatically handled hotkey 'Alt-F12-F12' (press
'F12' twice while holding 'Alt'). The screenshot will be automatically saved in a file, and can
be signed; that is, the current date and time will be included on the screenshot. (The file name
and sign string support parametrization). There is an option to automatically open a created
screenshot in the image editor and an option to show the snapshot's area and position while
dragging the mouse. During rectangular snapping, you can right-click the mouse to send the cursor
to the opposite corner of the rectangle. Also, you can make screenshots of video windows, if you
enable special support for such windows in the screenshot's preferences.<br>
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Keyboard (or text) macros</b></td>
<tr><td>
Powerful feature. Macro can be called by hotkey or from quick-run menu and can be written in JScript or VBScript.
<br>One type of macro will call any application with substituted parameters. Using this type of macro you can, for example,
select email address in any application and create a new email message with  the selected address as recipient by pressing hotkey
(in this example macro must have 'application path' set to 'mailto:' and 'parameters' set to '%SELECTED').
Or select a word and open search results for this word in Google by pressing another hotkey.
<br>Second type of macro will execute macro code with selected text as parameter and put the result 
back in the active application. See preinstalled macros to understand how it works.
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Offscreen windows</b></td>
<tr><td>
There are hotkeys to quickly bring back into screen windows, that are placed outside the screen boundaries.
"Windows traits" plugin can prevent windows from being moved beyond screen edges completely.
Useful, for example, for windows which title bar is used to be hidden by monitor`s upper border.
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Text layout conversion</b></td>
<tr><td>
Useful when you type some text in wrong keyboard layout (in any application) and don`t want (and now you don`t have to)
retype it in correct layout
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Process killer</b></td>
<tr><td>
You can list all started processes and their memory usage through the main menu or a hot menu and
kill a 'not responding' process (if you have admin rights to do so). You can also change the
process priority (and more) by right-clicking a process name in the processes submenu.  
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Tray minimizer</b></td>
<tr><td>
WireKeys can minimize any window to tray or hide it completely. Hidden window will be reachable through tray or main WireKeys menu.
The main difference from other competitor with such possibility: minimized apllication`s main menu will be still reachable through icon-in-tray context menu. 
And if you select an item from tray submenu, it will be added to the top menu level too for quick access in the future. 
Such moved items will be saved between WireKeys startups, so you haven`t to redefine them again and again. 
Though this may not work with some application. You can view and use system menu of the hidden to tray window also - 
just hold "Shift" key while opening tray menu. Additional "Windows traits" plugin can minimize window to tray 
by right-clicking its minimize button. Hidden to tray application can be detached from tray into small floating window.
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Run selected as from command string / Inline calculator</b></td>
<tr><td>
As simple as it`s sounds.
Extremely useful when you receive link to some document. With this feature you don`t need to open you file browser and drill down to desired location - just select link and press a hotkey, 
and corresponding application will be executed for you! If you double-press this hotkey, an history menu will be popuped, so you don`t have to remember your last actions. This operation detects console applications and start them in separate "cmd.exe" session, so you can check out the results. Try to select and 'Run as from command string' 
this text: "ping www.wiredplane.com". You can also 'run' arithmetical expressions, in this case the result will be inserted at the cursor position automatically. To be treated as expression selected string must end on '=' char.
You can even use trigonometrical functions as Math.sin(...) / Math.cos(...) and so on! Calculator based on Java-Script engine. 
You can also perform simple conversions between different scales of notation (conversion between arbitary bases): try to "run" following strings 
- '111(2->10)=', '7(10->2)=', 'FF(16->10)=', '255(10->16)='. This convertor also supports character base designators (B, O, D, H), so you can use expressions like "(100+99)(D to H)=" (this mean "calculate 100+99 and convert from decimal to hex").
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Quick run</b></td>
<tr><td>
With this feature you can run any application or open folder or internet link by simply pressing a hotkey.
You can also hide newly started application to tray or maximize/minimize it automatically.
There is option to popup already started application instead of starting another one.
Quick-run application can be automatically positioned, sized (user-defined size and position) 
and pinned after start. It can be also started from specified directory. 
You can also add additional icon to tray. Double click this icon to start corresponding application. If you detach this icon into floater, you will be able to browse startup directory from tray/floater menu.
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Quick shutdown</b></td>
<tr><td>
Shutdown with auto-power off can be cancelled approximately 2 seconds after initialization.
(Note: 'power off' requires special hardware support.) WireKeys can automatically kill some
applications before shutdown. Shutdown can be converted into a restart sequence if the shutdown
hotkey is pressed twice. Note that "Extreme" shutdown speed will not leave
any chance for applications to save their data. The main actions performed by Windows during
"Extreme" shutdown are flushing the file cache and saving personal
settings. 
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Volume control</b></td>
<tr><td>
Just hotkeys to change system sound volume up and down from 0% to 100%
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Boss key</b></td>
<tr><td>
With this key you can hide various windows from the desktop very quickly. This is a very useful
feature if you don't want your boss or colleagues to spy on your activity. Just press the hotkey
and your favorite game (or Internet browser, or any other program) instantly disappears from the
desktop and Ctrl+Alt+Del list. Later, you just press another hotkey and the hidden program
appears on the desktop again. Additionally, you can overlay your screen with your most recently
created screenshot. 
</td></tr>
<tr><td bgcolor="AAAAAA"><b>Desktop switcher (Win NT/2000/XP only)</b></td>
<tr><td>
You can dynamically create additional desktops and use them in a flash. WireKeys remembers the
desktop layout for each desktop and sets screen resolution for each desktop independently - very
useful for designers.
</td></tr>
</table>
<br><br>
<div id=h1>More info</div>
<lu>
<li><a href="../wirekeys/advantages.php">WireKeys advantages</a>
<li><a href="../wirekeys/custom.php">Custom development</a>
<li><a href="../wirekeys/oem.php">OEM package</a>
</lu>
<br><br>
<a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>
