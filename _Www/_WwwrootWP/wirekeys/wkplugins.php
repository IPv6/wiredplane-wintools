<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "Дополнительные плагины для WireKeys";
	}
	return "Additional plugins for WireKeys";
}
function insertSection($l){
?>

<div id=ptext>
<?php if($l==1){?>
	Здесь можно скачать дополнительные плагины для WireKeys. Заметьте, что наиболее важные плагины - Автотекст, Помощник по окнам, Рекордер макросов, Жесты мышью и плагин "Особые клавишные комбинации" - заранее включены в инсталлятор WireKeys.
	Если Вы написали плагин или макрос который может быть полезен многим - присылайте это нам. Авторы плагинов и интересных макросов автоматически получают 
	бесплатный регистрационный ключ в подарок. Если у Вас есть вопросы, Вы можете задать их в нашем <a href="/forum/board.php?cat=3&fid=1&s=s">форуме поддержки</a>
<?php }else{?>
	Here are plugins for use in WireKeys. Note that the most important plugins (Autotext, Window's traits, Macro recorder, Mouse gestures and Wired key combinations) are already included in the WireKeys installation.
	<a href="mailto:support@wiredplane.com?subject=New%20Plugin">Send us new plugins by email</a>, and we will post them here. Every plugin author will be given a free registration key to WireKeys. 
	If you have any questions, you can post them in our <a href="/forum/board.php?cat=3&fid=1&s=s">support forum</a>.
<?php }?>
</div>
<br><br>
<div id=h1>List of available plugins</div>
<img src="/images2/bar_big.gif" width=100% height=1>
	<table width="590" border="0" cellspacing="1" cellpadding="0" align="center">
	<tr>
	<th width=15% class=inpage>Plugin Name</th>
	<th width=55% class=inpage>Description</th>
	<th width=15% class=inpage>Download</th>
	</tr>
	
	<tr>
	<td class=inpage_center>Typing statistics</td>
	<td class=inpage>
	<?php if($l==1){?>
	Показывает статистику работы с клавиатурой - скорость печати (символов/слов в минуту), аритимию, статистику использования кнопок клавиатуры.
	<?php }else{?>
	Shows typing statistics: typing speed (symbols/words per minute), Rhythm deviation, symbols frequency and more.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_typestats.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Window puncher</td>
	<td class=inpage>
	<?php if($l==1){?>
	Плагин, позволяющий делать дырки в окнах приложений
	<?php }else{?>
	Plugin allows to make a holes in the application windows.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_winpuncher.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>MegaMouse plugin (magnifier+color picker) by <b>Pharaon</b></td>
	<td class=inpage>
	<?php if($l==1){?>
	Плагин, позволяющий прикрепить к курсору экранную лупу и показывающий в лупе цвет точки под курсором. Окошко-лупа вызывается по горячей клавише. Очень удобно. Must have for webmasters!
	<?php }else{?>
	Plugin allows to attach screen magnifier for picking color to mouse cursor and prints color of the point under cursor in that magnifier. 
	Magnifier window is called by hotkey. Very convinient.
	A must plugin for all webmaster!
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_megamouse.zip">Download</a></td>
	</tr>

	<tr>
	<td class=inpage_center>Taskbar extender</td>
	<td class=inpage>
	<?php if($l==1){?>
	Плагин позволяет перетаскивать кнопки на таскбаре в нужном Вам порядке, вытаскивать их с таскбара (при этом приложение будет либо минимизировано в трей если сбросить перетаскиваемую кнопку там, либо
	в плавающую иконку). Также ко всем кнопкам таскбара добавляется маленький крестик, позволяющий закрывать/убивать приложения одной кнопкой.
	Кроме всего этого плагин добавляет две горячие клавиши - переключиться на следующее приложения и на предыдущее. 
	Переключение идет строго в том порядке, в каком приложения располагаются на Таскбаре.
	Позволяет переключаться с помощью клавиатуры минуя необходимость считать нужное количество нажатий при использовании Alt-Tab :)
	<br> Протестирован на Win2000 и WinXP.
	<?php }else{?>
	Plugin adds ability to drag-n-drop taskbar buttons inside taskbar to sort them in your order, drag them outside taskbar to minimize application into tray (when dropped over tray)
	or into floater (when dropped over main area of desktop). Plugin also adds small "Close" icon to all buttons on taskbar to quickly close or kill windows (that are not responding for example).
	Beside that, plugin adds two more hotkeys to switch between applications in the taskbar order (next/previous window).
	<br> Tested on Win2000 and WinXP.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_alttabr.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Useless 'Start' button</td>
	<td class=inpage>
	<?php if($l==1){?>
	Заменяет текст на кнопке "Пуск" на уровень загрузки процессора или памяти
	<?php }else{?>
	Replaces text on 'Start' button with CPU usage or current memory load.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_uselessstart.zip">Download</a></td>
	</tr>

	<tr>
	<td class=inpage_center>Right-click paste/clipboard enhancer v1.1</td>
	<td class=inpage>
	<?php if($l==1){?>
	Добавляет историю буфера обмена во все менюшки с пунктом "Вставить" (вместо этого пункта). 
	Т.е. пользуясь контекстным меню в различных программах можно будет вставлять
	не только последний запомненный в буфере текст, но любой из десятка 
	(число регулируется) использованных Вами ранее
	<?php }else{?>
	 Adds history of the clipboard to all popup/conext menus in almost *all* applications, instead of 'Paste' item. 
	 You will be able to choose what text you want to paste. With this plugin you are not limited to last clip any more!
	 <?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_rclickcp.zip">Download</a></td>
	</tr>

	<tr>
	<td class=inpage_center>Close console windows by pressing Escape</td>
	<td class=inpage>
	<?php if($l==1){?>
	Плагин позволяет закрывать консольные окна двойным нажатием на Escape. Можно настроить на закрытие Escape-ом любое приложение или окною
	<?php }else{?>
	Plugin allows to close console windows by double-pressing Escape button
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_ccbyesc.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Spell checker by <b>Karcade</b></td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет проверить правильность написания выделенного в любом текстовом поле слова. При наличии нескольких вариантов правильного написания показывает меню с выбором, при выборе заменяет неверное слово на нужный вариант. 
	Использует свободно распространяемый движок от OpenOffice, в инсталляторе установлен словарь для английского языка. 
	Словарь для русского языка можно скачать здесь: <a href="/download/extras/dic-ru-1251.zip">dic-ru-1251.zip</a>
	Дополнительные словари на практически любой язык мира можно скачать на этой странице:
	<?php }else{?>
	 Allows to check spelling of any word selected in any text field. Uses free speller from OpenOffice. Installation package contain dictionary got American English. More dictionaries (for almost any language) can be found here:
	 <?php }?>
	 <a href="http://lingucomponent.openoffice.org/spell_dic.html">http://lingucomponent.openoffice.org/spell_dic.html</a>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_spellcheck.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Windows and menus enabler by <b>Alex Bulavin & Sergey Mushinsky</b></td>
	<td class=inpage>
	<?php if($l==1){?>
	Включает все кнопки, пункты меню и даже окна, недоступные из-за работы модальных диалогов - либо постоянно либо
	по горячей клавише. Используйте на свой собственный риск, не всегда элементы включенные таким образов ведут себя так, как хочется :)
	<?php }else{?>
	 Enables all buttons (grayed out, inactive, and so on) everywhere in all programs, all windows, and items in all popup menus. 
	 Very useful for applications that disable buons and menus to prevent you from accessing certain features.
	 <?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_enabler.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Any-window resizer</td>
	<td class=inpage>
	<?php if($l==1){?>
	Меняет размеры любого окна - даже такого, которое не поддерживает изменение размера. Меняет размеры всех элементов окна пропорционально. Удобно использовать со старыми приложениями, в которых
	маленькие диалоги и нету возможности изменить их размер. Также имеет функцию пропорционального увеличения/уменьшения текущего окна по горячим клавишам
	<?php }else{?>
	Resize any window - even windows that do not normally support resizing! Caution: Due to unpredictable behavior of some applications, windows resizing can have additional side effects.
	This plugin can also make current window bigger/smaller using hotkeys.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_sizewnd.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Full sound control</td>
	<td class=inpage>
	<?php if($l==1){?>
	Добавляет горячих клавиш на все доступные управлению линии звуковой карты
	<?php }else{?>
	Control all aspects of your sound card with hotkeys. Instantly access Pan (balance), Volume, and Mute of Wave, CD Audio, Line in, Microphone, and SW Synth sound lines.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_soundctr.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Text grabber</td>
	<td class=inpage>
	<?php if($l==1){?>
	Вытаскивает текст из тех окон, которые не позволяют это делать с использованием буфера обмена
	<?php }else{?>
	Grab text from Windows controls that do not normally support copy/paste operations. Get all the text from a specified window.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_textscr.zip">Download</a></td>
	</tr>	
	
	<tr>
	<td class=inpage_center>"As LINUX Move" by <b>Vint</b> v1.1</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет тащить любое окно за любое место, а не только за заголовок
	<?php }else{?>
	Quickly move any window with the mouse. Simulates LINUX style behavior in MS Windows.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_aslinux.zip">Download</a></td>
	</tr>	
	
	<tr>
	<td class=inpage_center>Instant scrolling</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет скроллить любое окно с помощью мыши, не дотягиваясь до полос прокрутки
	<?php }else{?>
	Quickly scroll any window. Looks very similar to the "As LINUX Move" plugin but scrolls the window's content instead of moving window the itself.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_iscroll.zip">Download</a></td>
	</tr>	

	<tr>
	<td class=inpage_center>Time sync</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет синхронизировать системные часы с SNTP сервером
	<?php }else{?>
	 Can sync computer clocks with SNTP server
	 <?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_timesync.zip">Download</a></td>
	</tr>	

	<tr>
	<td class=inpage_center>Desktop icon`s font</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет установить свой шрифт и цвет иконкам на десктопе
	<?php }else{?>
	Set user-defined font and color for desktop icons. Full source code included! 
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkplugins/wkp_iconsfont.zip">Download</a></td>
	</tr>	
				
	</table>
<br>

<div id=h1>To developers</div>
<div id=ptext>
<?php if($l==1){?>
	Если Вам не хватает какой-либо функции и Вы знаете как ее сделать, то можно написать плагин к WireKeys самому. Сделать это достаточно легко даже для начинающего прграммиста, взяв за основу любой из описанных
	на <a href="wksources.php">странице с исходниками</a> уже готовых плагинов и почитав коментарии в <a href="../wirekeys-help/wkplugin.h ">заголовочном файле к плагинам</a>. 
	WireKeys позволяет использовать многие их своих возможностей в плагине, что облегчает создание сложных и интересных (и полезных) вещей.
	Если Вы написали плагин или макрос который может быть полезен многим - присылайте это нам. Авторы плагинов и интересных макросов автоматически получают 
	бесплатный регистрационный ключ в подарок. Если у Вас есть вопросы, Вы можете задать их в нашем <a href="/forum/board.php?cat=3&fid=1&s=s">форуме поддержки</a>
<?php }else{?>
	If you missing some specific function and know how to implement it by yourself, you can write your own plugin.
	Creating WireKeys plugins is very simple, even for novice programmers. Take some time to read over the notes in 
	the <a href="../wirekeys-help/wkplugin.h ">plugin`s header file</a>, look into and grab <a href="wksources.php">sample plugins source code</a>, and you can 
	have your first plugin up and running withing just a few minutes. Send us new plugins by email, and we will post 
	it here. Every plugin author will be given a free registration key to WireKeys. If you
	have any questions, you can post them in <a href="/forum/board.php?cat=3&fid=1&s=s">our support forum</a>.
<?php }?>
</div>

<br><br><div id=ptext>
remember that plugins will not work without WireKeys itself. Unregistered version of WireKeys will run plugins only during trial period, after which they all will be disabled
</div><br>
<a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>
