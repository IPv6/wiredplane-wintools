<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "Дополнительные макросы для WireKeys";
	}
	return "Additional macros for WireKeys";
}
function insertSection($l){
?>

<div id=ptext>
<?php if($l==1){?>
	Здесь можно скачать дополнительные макросы для WireKeys. Если Вы написали плагин или макрос который может быть полезен многим - присылайте это нам. Авторы плагинов и интересных макросов автоматически получают 
	бесплатный регистрационный ключ в подарок. Если у Вас есть вопросы, Вы можете задать их в нашем <a href="/forum/board.php?cat=3&fid=1&s=s">форуме поддержки</a>
<?php }else{?>
	Here are JavaScript and Visual Basic macros for use in WireKeys. <a href="mailto:support@wiredplane.com?subject=New%20Macro">Send us new macros by email</a>, and we will post them here. Every plugin author will be given a free registration key to WireKeys. 
	If you have any questions, you can post them in our <a href="/forum/board.php?cat=3&fid=1&s=s">support forum</a>.
<?php }?>
</div>
<br><br>
<div id=h1>List of available macros (find more at our <a href="/forum/board.php?cat=3&fid=1&s=s">forum</a>)</div>
<img src="/images2/bar_big.gif" width=100% height=1>
	<table width="590" border="0" cellspacing="1" cellpadding="0" align="center">
	<tr>
	<th width=15% class=inpage>Macro Name</th>
	<th width=55% class=inpage>Description</th>
	<th width=15% class=inpage>Download</th>
	</tr>
	
	<tr>
	<td class=inpage_center>Universal control over popular video/audio players</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет управлять любым популярным плеером (Windows Media player, Zoom player, BSPlayer, Light Alloy, WinAmp, Musicmatch Jukebox) посредством гор. клавиш из одного макроса.
	<?php }else{?>
	Control any popular media player (Windows Media player, Zoom player, BSPlayer, Light Alloy, WinAmp, Musicmatch Jukebox) via hotkeys. Access to main functions of any player from one macro.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_mediaplayers.zip">Download</a></td>
	</tr>
	
	<?php if($l==1){?>
	<tr>
	<td class=inpage_center>Подстрочный перевод выделенного слова</td>
	<td class=inpage>
	Показывает перевод выделенного слова в сплывающем облаке рядом с курсором. Использует online словари с www.rambler.ru
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_ramblertrans.zip">Download</a></td>
	</tr>
	<?php }else{?>
	<tr>
	<td class=inpage_center>Popups in-place translation of the selected word</td>
	<td class=inpage>
	Popups baloon with translation to selected word. Uses dictionaries from www.rambler.ru. English-Russian and Russian-English only.
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_ramblertrans.zip">Download</a></td>
	</tr>
	<?php }?>
		
	<tr>
	<td class=inpage_center>Convert selected text to and from transliteration form</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет нажатием клавиши транслитерировать выделенный текст или перевести из транслитерированного текста в кириллицу
	<?php }else{?>
	Using this macro you can transliterate selected text in a flash or convert text from transliterated form. 
	By default it can be used for cyrillic characters, but macro can be easily changed to support different forms of transliteration.
	Can be used as greate example for all text-conversion macros.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_translit_latrus.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Calculate md5/md4/Sha-1 hash of the selected text</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет нажатием клавиши посчитать хэш выделенного текста по одному из 3ех распространенных алгоритмов - MD5/MD4/SHA-1
	<?php }else{?>
	Using this macro you can calculate hash of the selected text using well-known algorithms: MD5/MD4/SHA-1
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_getmd5hash.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Convert selected text into Base64 encoding</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет нажатием клавиши сконвертировать выделенный текст в BASE64-представление
	<?php }else{?>
	Using this macro you can convert selected text into BASE64 encoding on-the-fly, by pressing one hotkey
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_convert2base64.zip">Download</a></td>
	</tr>

	<tr>
	<td class=inpage_center>Control opened windows by Vlad Bezden</td>
	<td class=inpage>
	<?php if($l==1){?>
	Этот VBScript-макрос позволяет минимизировать/вернуть обратно открытые окна или вызвать диалог запуска файла (вместо Win-R)
	<?php }else{?>
	Using this VBScript macro you can Minimize all windows/Undo minimize or open Run dialog by hotkey. Useful with "Win" key blocked, so you can reassign this actions to new key combinations.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_wincontrol.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Open file in pre-defined application by Walton Dell</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет открыть выделенный текст в файле в заданном в параметре приложении (Notepad по умолчанию)
	<?php }else{?>
	Open selected text in the newly created text file in the text-editor (NotePad by default)
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_opentextinapp.zip">Download</a></td>
	</tr>

	<tr>
	<td class=inpage_center>Control Explorer`s view</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет переключаться между видами окна в Explorer-е (маленькие иконки/большие иконки/список/днтали/предпросмотр) нажатием гор. клавиши
	<?php }else{?>
	Switch between small icons/large icons/list/details and thumbnails views of Explorer easiliy - by pressing hotkey
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_explorerview.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Light Alloy control</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет управлять плеером Light Alloy (аудио/видео) посредством гор. клавиш.
	<?php }else{?>
	Control Light Alloy (video/audio player) via hotkeys. Access to all main functions.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_lalloy.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Window control</td>
	<td class=inpage>
	<?php if($l==1){?>
	Позволяет двигать активное окно вверх/вниз вправо/влево/в центр горячими клавишами, позволяет перемещать окно к заданной части экрана.
	Работает только в WK, выпущенном не ранее 19 Авг. 2004 г.
	<?php }else{?>
	Move window to the right/left/top/bottom edge of the screen or by certain amount of pixels.
	Work with the WK released after 19 Aug 2004
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_windowcontrol.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Folder listing (JavaScript), updated by Walton Dell</td>
	<td class=inpage>
	<?php if($l==1){?>
	Получить список файлов в подсвеченном каталоге (подсвеченном в эксплорере или другом файл-браузере)
	<?php }else{?>
	Get listing of files in the highlighted folder (in explorer or another file manager).
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_folderlisting.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>Set of case conversion routines</td>
	<td class=inpage>
	<?php if($l==1){?>
	Набор функций по конвертации регистра текста by <b>Decil</b>:<br>
	- В нижний регистр<br>
	- В верхний регистр<br>
	- Перевод первых букв текста в верхний регистр<br>
	- Инвертирование регистра выделенного текста
	<?php }else{?>
	Set of four case conversions, written by <b>Decil</b>:<br>
	- Converts selected text to lower case.<br>
	- Converts selected text to upper case.<br>
	- Capitalizes the first letter of the selected text. Works with English and Russian languages.<br>
	- Inverts the case of the selected text. Works with English and Russian languages.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_completecase.zip">Download</a></td>
	</tr>
	
	<tr>
	<td class=inpage_center>On-line help for PHP command</td>
	<td class=inpage>
	<?php if($l==1){?>
	Открывает страницу с результатами поиска по выделенному в тексте слову-команде языка PHP<br>
	<?php }else{?>
	Searches the Internet for help pages about a PHP command that you have highlighted in any window or text editor.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_phphelp.zip">Download</a></td>
	</tr>	
	
	<tr>
	<td class=inpage_center>Random quotes from Bash.org (VBScript)</td>
	<td class=inpage>
	<?php if($l==1){?>
	Показывает случайную цитату с Bash.org
	<?php }else{?>
	Shows you random quotes from Bash.org. You must be online when you call this macro.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_getbash.zip">Download</a></td>
	</tr>	
	
	<tr>
	<td class=inpage_center>Recursive copy</td>
	<td class=inpage>
	<?php if($l==1){?>
	Копирует все файлы (включая подкаталоги) из подсвеченной директории в заданную Вами папку (после вызова макроса он спросит куда их сохранять) _без_сохранения_структуры_каталогов, т.е. все файлы в одну папку.
	<?php }else{?>
	Get all the files from the selected folder (in explorer or another file browser) and its subfolders and puts them into another folder defined by the user (without the source directory structure).
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_recursivecopy.zip">Download</a></td>
	</tr>	

	<tr>
	<td class=inpage_center>"Irfan view" automatic crop tool</td>
	<td class=inpage>
	<?php if($l==1){?>
	Вырезает и сохраняет фрагменты изображения. Запись нажатий клавиш для быстрой обработки изображений в Irfan View. Выделил кусок картинки, нажал хоткей - и макрос автоматом вырежет этот кусок, сохранит его и перейдет к следующей картинке
	<?php }else{?>
	Crop and save a sequence of images in a batch, one by one. Uses <a href="http://www.irfanview.com">IrfanView's</a> built-in keys to perform crop, save, and next image operations in sequence.
	<?php }?>
	</td>
	<td class=inpage_center><a href="/download/wkmacros/wkm_irfanbatch.zip">Download</a></td>
	</tr>	
				
	</table>
<br>

<div id=h1>To developers</div>
<div id=ptext>
<?php if($l==1){?>
	Если Вам не хватает какой-либо функции и Вы знаете как ее сделать, то можно написать макрос к WireKeys самому. Сделать это достаточно легко даже для начинающего прграммиста, взяв за основу любой из описанных
	макросов и почитав документацию. WireKeys позволяет использовать многие их своих возможностей в макросе, что облегчает создание сложных и интересных (и полезных) вещей.
	Если Вы написали макрос который может быть полезен многим - присылайте это нам. Авторы автоматически получают бесплатный регистрационный ключ в подарок. 
	Если у Вас есть вопросы, Вы можете задать их в нашем <a href="/forum/board.php?cat=3&fid=1&s=s">форуме поддержки</a>
<?php }else{?>
	If you missing some specific function and know how to implement it by yourself, you can write your own macro.
	Creating WireKeys macros is very simple, even for novice programmers. Take some time to read documenteation and other macros as examples, and you can 
	have your first macro up and running withing just a few minutes. Send us new macros by email, and we will post 
	it here. Every author will be given a free registration key to WireKeys. If you
	have any questions, you can post them in <a href="/forum/board.php?cat=3&fid=1&s=s">our support forum</a>.
<?php }?>
</div>

<br><br><div id=ptext>
remember that macros will not work without WireKeys itself (except common JavaScript/VBScript macros).
</div><br>
<a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>
