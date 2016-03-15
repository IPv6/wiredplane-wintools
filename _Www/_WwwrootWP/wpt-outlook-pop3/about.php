<?php 
function getTitle($l)
{
	global $language;
	return "Mapi2Pop3 - POP3/SMTP proxy for Outlook/Exchange server";
}
function insertSection($l){
?>
<div id=h1>Mapi2Pop3</div>

<?php if($l==1){?>
	Mapi2Pop3 - Шлюз между сервером Microsoft Exchange и любым приложением понимающим протокол POP3 или SMTP. 
	Если у Вас есть программа которая отсылает почту по протоколу SMTP или принимает ее по протоколу POP3, то используя Mapi2Pop3
	Вы сможете заставить ее работать через Outlook. Позволяет использовать Exchange server без необходимости использовать MS Outlook.
	<br><br>
	У Вас приложение, которое работает с почтой и Вы хотите чтобы она делала свою работу через Outlook или сервер Exchange?	Хотите вместо Outlook использовать для чтения и 
	отправки почты Ваш любимый POP3 клиент? Тогда устанавливайте Mapi2Pop3 - всю заботу о Outlook наша программа возьмет на себя!
	<br><br>Программа абсолютно бесплатна для некоммерческого использования и не содержит AdWare или SpyWare.
	Для коммерческого использования все же нужно приобрести <a href="http://www.wiredplane.com/guestbook/vendors.php?who=comuse&vendor=2&src=site">лицензию</a>
<?php }else{?>
	Mapi2Pop3 allows you to access Exchange server via <b>Pop3 protocol</b> and send messages (through Exchange) via <b>Smtp protocol</b> easily from any application you want (not just Outlook). 
	Zero configuration required!<hr>
	Wish to use your favourite POP3 client instead of Outlook? Have program that work with POP3/SMTP servers and want it to work 
	via Outlook? Mapi2Pop3 can be used with all aplications that understand Pop3 or Smtp protocol including fully functional Pop3 
	clients like <b>The Bat!</b>, <b>Eudora</b>, <b>Opera M2</b>, <b>Thunderbird</b>. With this proxy all this applications can be 
	attached to Exchange server for email messaging in the same way Outlook does<br>
	<br><br>
	This program is totally FREE for non-commersial use and does not contain adware or spyware of any kind.<br>But you can donate us if you find it helpful.
	For commersial usage you should obtain <a href="http://www.wiredplane.com/guestbook/vendors.php?who=comuse&vendor=2&src=site">license</a>
<?php }?>
<br><center><br>
<script type="text/javascript"><!--
google_ad_client = "pub-5186671154037974";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text_image";
google_ad_channel = "";
google_color_border = "000000";
google_color_bg = "EEEEEE";
google_color_link = "0000FF";
google_color_text = "000000";
google_color_url = "008000";
//-->
</script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</center>
<div width=100% align=center><a href="/download/mapi2pop3.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a></div>
<?php if($l==1){?>
	Программа также будет полезна владельцам КПК и сотовых телефонов/смартфонов с возможностью отправки почты. Не секрет что программы синхронизации для таких устройств
	в подавляющем большинстве случаев работают только с Outlook. Но что если Вы хотите читать и отправлять почту через другой почтовый клиент, к примеру TheBAT? С Mapi2Pop3 это просто!
	Достаточно лишь настроить Ваш почтовик на работу с нашим прокси: Mapi2Pop3 будет забирать синхронизированную почту из Outlook и класть отправляемую Вами почту в папку Outbox для последующей 
	отправки на мобильном устройстве или же сразу на настольном компьютере
<?php }else{?>
	This application can be useful in the following cases:
	<br><br>- You wish to use your favourite Pop3 client for email messaging but have Exchange server without Pop3/Imap support, so you forced to use Outlook
	<br><br>- You have an application that can interact with/benefit from Pop3 or Smtp services and want it to work via Outlook inbox/outbox folders
	<br><br>- You use your PPC or mobile phone for writing emails and send them at computer station, but PPC/Mobile software allows synchronization with Outlook only. 
	In this case you can leave synchronization with Outlook 'as is' and connect to local Outlook folders through this proxy
<?php }?>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
