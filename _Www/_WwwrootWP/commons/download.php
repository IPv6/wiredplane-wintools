<?php 
function getTitle($l)
{
	if($l==1){
		return "Файловая зона";
	}
	return "Free downloads";
}
function insertSection($l){
?>
<?php if($l==1){?>
	Здесь находятся дистрибутивы наших продуктов, доступные для свободного скачивания и инсталляции. Инструкция по установке: Распакуйте полученный файл в любую директорию и запустите инсталлятор, далее следуйте инструкциям на экране.
<?php }else{?>
	Here you can download the latest versions of our products. To install a product, click the appropriate link which will 
	download a zip file to your computer. If you already have a program (such as <a href="http://www.winzip.com">Win Zip</a>) 
	that can unzip such files, find the file you downloaded to your hard drive and click it to begin the installation process. Windows XP has such an unzip program built into the system.
<?php }?>
<br><br>
<?php if (!defined('FREEWARE_ONLY')){?>
<div id=h1><?php if($l==1){?>Программы с оценочным сроком использования<?php }else{?>Trialware utilities<?php }?></div>
<table border="0" width="100%" cellspacing="0" cellpadding="2" class="grtb">
<tr align="center" bgcolor="#dddddd">
	<th class=inpage width=25%>Product</td>
	<th class=inpage width=20%>Release date</td>
	<th class=inpage width=20%>Product type</td>
	<th class=inpage width=15%>File size</td>
	<th class=inpage width=20%>Download link</td>
</tr>

<tr align="center">
	<td class=inpage_center><a href="../wirekeys/about.php">WireKeys</a><br><?php insertVersion("wirekeys"); ?>
	<td class=inpage_center><?php insertDate("wirekeys"); ?></td>
	<td class=inpage_center>Trialware,<br>14 days to try</td>
	<td class=inpage_center><?php insertSize("wirekeys"); ?></td>
	<td class=inpage_center><b>&nbsp;&nbsp;<a href="/download/wirekeys.zip">Download&nbsp;WireKeys!</a>&nbsp;&nbsp;</b>
	<br><small>(Includes&nbsp;5&nbsp;plugins)</small><br><small>See also <a href="../wirekeys/wkplugins.php">plugins</a> and <a href="../wirekeys/wkmacros.php">macros</a></small>
	</td>
</tr>

<tr align="center">
	<td class=inpage_center><a href="../wirechanger/about.php">WireChanger</a><br><?php insertVersion("wirechanger"); ?>
	<td class=inpage_center><?php insertDate("wirechanger"); ?></td>
	<td class=inpage_center>Trialware,<br>unlimited trial</td>
	<td class=inpage_center><?php insertSize("wirechanger"); ?></td>
	<td class=inpage_center><b>&nbsp;&nbsp;<a href="/download/wirechanger.zip">Download&nbsp;WireChanger!</a>&nbsp;&nbsp;</b>
	<br><small>See also <a href="../wirechanger/wcclocks.php">Clocks</a> and <a href="../wirechanger/widgets.php">Widgets</a></small>
	</td>
</tr>
</table>
<br><br>
<?php }?>
<a name=freeware>
<div id=h1><?php if($l==1){?>Бесплатные для некоммерческого использования программы<?php }else{?>Free for non-commersial use<?php }?></div>
</a>
<table border="0" width="100%" cellspacing="0" cellpadding="2">
<tr align="center">
	<th class=inpage width=20%>Product name</td>
	<th class=inpage width=40%>Description</td>
	<th class=inpage>File size</td>
	<th class=inpage width=30%>Download link</td>
</tr>

<tr align="center">
	<td class=inpage_center valign=top><a href="../wpt-wirenote/about.php">WireNote <?php insertVersion("wirenote"); ?></a>
	<td class=inpage_center><?php if($l==1){?>Удобный организатор, позволяющий вести список дел и напоминаний. Может создавать окна-заметки, которые приклеиваются к конкретному окну. Имеет встроенные возможности общения по сети (с WinPopup/Messenger service), включая пересылку файлов. Также есть модуль борьбы со спамом в POP3 ящиках (без скачивания сообщений с сервера)<?php }else{?>Smart personal data manager and messaging application. Attach Sticky Notes and ToDos to the desktop or to any other application's window. Schedule Reminders to perform one or more actions at a set date and time. Use WireNote's high-quality LAN messaging (including send/receive files and message filtering) instead of WinPopup or Windows messaging service<?php }?></td>
	<td class=inpage_center><?php insertSize("wirenote"); ?></td>
	<td class=inpage_center><a href="/download/wirenote.zip">Download<br>WireNote!</a></td>
</tr>

<tr align="center">
	<td class=inpage_center valign=top><a href="../wpt-kaleidoswallpaper/about.php">KaleidosWallpaper</a>
	<td class=inpage_center><?php if($l==1){?>Генератор обоев. Для работы достаточно задать в настройках каталог с картинками и программа, используя эффект калейдоскопа, будет регулярно создавать красивые коллажи на их основе <?php }else{?>Simple and funny wallpaper generator. It can generate futuristic wallpapers and put them on desktop automatically<?php }?></td>
	<td class=inpage_center><?php insertSize("kaleidoswallpaper"); ?></td>
	<td class=inpage_center><a href="/download/kaleidoswallpaper.zip">Download<br>KaleidosWallpaper!</a></td>
</tr>

<tr align="center">
	<td class=inpage_center valign=top><a href="../wpt-backup/about.php">WPBackup</a>
	<td class=inpage_center><?php if($l==1){?>Простая и надежная утилита для инкрементального бэкапа. Утилита запоминает CRC файлов и при создании бэкапа сохраняет файлы с измененным CRC.<?php }else{?>Snapshot-based incremental backup utility. Simple command-line tool for incremental backup, based on snapshot of the folder content (files CRCs are saved)<?php }?></td>
	<td class=inpage_center><?php insertSize("wpbackup"); ?></td>
	<td class=inpage_center><a href="/download/wpbackup.zip">Download WPBackup</a>*</td>
</tr>

<tr align="center">
	<td class=inpage_center valign=top><a href="../wpt-gamelogr/about.php">GameLogr</a>
	<td class=inpage_center><?php if($l==1){?> Автоматический скриншотер для сохранения прогресса в играх <?php }else{?>Screenshot utility which automatically works when you playing PC games<?php }?></td>
	<td class=inpage_center><?php insertSize("gamelogr"); ?></td>
	<td class=inpage_center><a href="/download/gamelogr.zip">Download<br>GameLogr!</a></td>
</tr>

<tr align="center">
	<td class=inpage_center valign=top><a href="../wpt-outlook-pop3/about.php">Mapi2Pop3</a><br>Open source
	<td class=inpage_center><?php if($l==1){?>Шлюз между сервером Microsoft Exchange и любым приложением понимающим протокол POP3 или SMTP. Если у Вас есть программа которая отсылает почту по протоколу SMTP или принимает ее по протоколу POP3, то используя Mapi2Pop3 Вы сможете заставить ее работать через Outlook. Позволяет использовать Exchange server без необходимости использовать MS Outlook.<?php }else{?>Proxy component to use Outlook through any Pop3 client<?php }?></td>
	<td class=inpage_center><?php insertSize("mapi2pop3"); ?></td>
	<td class=inpage_center><a href="/download/mapi2pop3.zip">Download<br>Mapi2Pop3!</a>*</td>
</tr>

<tr align="center">
	<td class=inpage_center valign=top><a href="../wpt-screencannon/about.php">ScreenCannon</a>
	<td class=inpage_center><?php if($l==1){?>Простая но мощная утилита для снятия скриншотов с чего угодно.<?php }else{?>Small but powerfull utility for taking screenshots in any applications<?php }?></td>
	<td class=inpage_center><?php insertSize("screencannon"); ?></td>
	<td class=inpage_center><a href="/download/screencannon.zip">Download ScreenCannon</a></td>
</tr>

<tr align="center">
	<td class=inpage_center valign=top>ProofGoogling
	<td class=inpage_center>Checks the orthography of text in a foreign language using Google or other search system</td>
	<td class=inpage_center><?php insertSize("proofgoogling"); ?></td>
	<td class=inpage_center><a href="/download/proofgoogling.zip">Download<br>ProofGoogling!</a>*<br></td>
</tr>

<tr align="center">
	<td colspan="4" class=inpage_center>&nbsp;*This download does not include an installation program. Just unzip and click the program file to start it.</td>
</tr>
</table>

<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
