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
<!--<table border=0 width=98%><tr>
<td align=center><a href="screens.php"><img src="/images2/wirekeys/screens/th_wkeys3.gif" width=150 style="float: left; margin-right: 0.5em;"></a></td>
<td align=center><a href="screens.php"><img src="/images2/wirekeys/screens/th_wkeys2.gif" width=150></a></td>
<td align=center><a href="screens.php"><img src="/images2/wirekeys/screens/th_wkeys1.gif" width=150></a></td>
</tr></table><br>-->
<a href="screens.php"><img src="/images2/wirekeys/screens/th_wkeys3.gif" width=150 style="float: left; margin-right: 0.5em;"></a>
<?php if($l==1){?>
<div id=h1>WireKeys - Что угодно одной кнопкой!</div>
Это многофункциональная утилита, ориентированная на использование горячих клавиш.
Вместе с WireKeys в системе появляются доп. буферы обмена (до 99), по каждому из которых (и по стандартному тоже) ведется история копирования, 
так что Вы всегда можете вставить то, что было в буфере "два копирования назад".
Кроме этого появляются другие возможности по работе с выделенным текстом (например выделенный в любой 
программе текст можно запустить как из командной строки, посчитать как на калькуляторе, поменять раскладку текста, обработать VB/JScript макросом). 
Также удобно управлять списком активных процессов с возможностью мгновенно закрыть любой из них.
Добавляются также широкие возможности по управлению открытыми окнами с клавиатуры: минимизация приложений в трей с сохранением 
главного меню программы (оно будет доступно в меню у иконки в трее), смена основных состояний окна нажатием на кнопки заголовка окна и т.д.
Также есть часто необходимая возможность записать и воспроизвести последовательность нажатий клавиатуры или действий мыши,
 делать снимки экрана с автоматическим сохранением на диск и подписью, управлять различными медиа-плеерами и другими программами и многое другое.
&nbsp;<b>Возможности - неограничены!</b>&nbsp;&nbsp;Используйте WireKeys
чтобы сэкономить на выполнении обычных действий на компьютере самую ценную вещь на свете - <b>Ваше время</b>.
<br>Горячие клавиши допускают использование в них кнопок мыши, краев экрана, 
раздельно правых и левых клавиш Shift/Alt/и т.д., клавиш мультимедиа-клавиатур, кнопок пультов управления TV-тюнером (Поддерживается <a href="http://winlirc.sourceforge.net/">WinLIRC</a> и пульты тюнеров AverTV).
На каждое действие можно назначить до трех разных комбинаций.
Функциональность WireKeys может быть легко расширена за счет <a href="/wkplugins.shtml">подключаемых модулей - плагинов</a>. 5 самых полезных плагина (помощник по окнам, автотекст, 
поддержка специальных горячих клавиш, улучшатель стандартных диалогов Save As/Open и записыватель макросов) включены в дистрибутив WireKeys.
<?php }else{?>
<div id=h1>WireKeys - Automation tool for power users<br>Real keys to your keyboard!</div>
This application is a Windows NT4/2000/XP utility that speeds up routine tasks by reducing
the number of keystrokes required to perform them. If you're tired of installing one utility after another to fit all 
your needs, WireKeys is the solution you've been looking for. Now you can manage *all* your hotkeys from one place instead 
of remembering preferences of tens applications. And hotkey in WireKeys can be much more useful and effective than in any other application.
You will be able to redefine standard Windows hotkeys like Win-R, Win-F and others.
<p><font size=+1>Automate all aspects</font> of everyday working with your computer. Control file 
and directory manipulation, text replacement, text processing, file downloads and uploads. Use an inline calculator. 
Create additional desktops. Use your own intuitive hotkeys in combination with your mouse and screen edges. 
Allow one key to execute multiple actions simultaneously. Hit the 'Boss Key' to hide windows of your choice in an emergency. 
With WireKeys you can hide windows to the tray, minimize them to the taskbar or into a floater, roll them up to the titlebar, 
change a window's 'On-top' state, move it back into view, change its opacity, create a shortcut or hotkey for quick access, 
and much more. All these actions can be done using hotkeys, remotes (<a href="http://winlirc.sourceforge.net/">WinLIRC</a> is supported as well as AverMedia remotes) or mouse gestures or can be done almost automatically by repetitive schedule.
<p><font size=+1>Eliminate repetitive tasks</font> using <b>JavaScript-compatible</b> macro language.
For added convenience, all of your keyboard macros and shortcuts can be organized into menus, 
so you won't have to memorize your hotkeys, regardless of how many time-saving macros you create.
WireKeys text <b>autoreplace</b> facility will automatically expand short words 
or phrases into long text strings so you don`t have to type them again.
<?php }?>

<?php if($l==1){?>
<p>Используя эту программу Вы сможете <b>управлять *всеми* горячими клавишами</b> (и даже переопределять стандартные сочетания Windows - Win-R, Win-F и др.) в Вашей системе из одного приложения! Вам больше не придется 
вспоминать "В какой-же утилите можно задать кнопку для действия ***?" - в WireKeys есть все что только может понадобиться!
<br><br>
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
<p><font size=+1>Record keystrokes that you frequently use</font>. Once you have recorded your frequently-typed
information, WireKeys makes it quick and easy to fill out forms on the web, enter logins and passwords, and send
routine notes to customers and colleagues. 

<p><font size=+1>Speed up typing with <b>additional clipboards</b></font> assigned to separate hotkeys. 
Normally, you have only one clipboard, and when you copy text into it, all previous data is lost. With WireKeys you can copy 
snippets into different clipboards at the same time and hold them until you need them. Data copied to additional clipboards 
is NOT copied to the standard clipboard, these clipboards can be used in addition to the standard clipboard.
This features works with most windows programs. Below is a partial list of supported applications:</p>
<table width="100%%" border="0" cellpadding="0" cellspacing="0" >
    <tr>
      <td width="5%" height="20">&nbsp;</td>
      <td width="42%"><li>Yahoo Messenger </td>
      <td width="50%"><li>MSN Messenger </td>
      <td width="3%">&nbsp;</td>
    </tr>
    <tr>
      <td height="20">&nbsp;</td>
      <td><li>ICQ Messenger </td>
      <td><li>Outlook Express </td>
      <td rowspan="6">&nbsp;</td>
    </tr>
    <tr>
      <td height="20">&nbsp;</td>
      <td><li>Microsoft Word</td>
      <td><li>Microsoft PowerPoint</td>
    </tr>
    <tr>
      <td height="20">&nbsp;</td>
      <td><li>Microsoft Outlook</td>
      <td><li>Microsoft Excel </td>
    </tr>
    <tr>
      <td height="20">&nbsp;</td>
      <td><li>UltraEdit-32</td>
      <td><li>Macromedia Dreamweaver </td>
    </tr>
    <tr>
      <td height="20">&nbsp;</td>
      <td><li>Adobe Acrobat </td>
      <td><li>Adobe PageMaker </td>
    </tr>
    <tr>
      <td height="20">&nbsp;</td>
      <td><li>Notepad </td>
      <td><li>WordPad </td>
    </tr>
    <tr>
      <td height="20">&nbsp;</td>
      <td><li>ACT! </td>
      <td><li>GoldMine </td>
      <td>&nbsp;</td>
    </tr>
</table>
<br>And many more!
<p><font size=+1>Install plugins</font> to increase possibilities of this hotkey manager. Look at available <a href="/wkplugins.shtml">plugins</a> 
and <a href="/wkmacros.shtml">macros</a> for even more control over windows, sound, text case, fonts, graphics, and more.
Some of them are preinstalled with <a href="/download/wirekeys.zip">installation package</a>. For example after installing 
WireKeys you will get great shell enhancement plugin that gives you quick jump to your favorite folders in Common Open/Save 
dialogs (including dialogs of the <b>Microsoft Office</b> applications like Word, Excel, Outlook),in Windows Explorer 
and Total Commander.
<?php }?>
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
