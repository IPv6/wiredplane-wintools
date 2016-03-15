<?php 
function getTitle($l)
{
	global $language;
	return "GameLogr - Утилита для сохранения прогресса в любой игре!";
}
function insertSection($l){
?>
<div id=h1>GameLogr</div>

Эта маленькая и простая программа предназначенна для одной цели - автоматически сохранять скриншоты прохождения Вами Ваших любимых игр.
Запустите ее и забудьте о горячих клавишах для снятия скриншота - вся прелесть в том, что GameLogr содержит огромную базу игр и сам разберется когда
Вы запускаете игру, а когда работаете. В случаве, если Вы играете, GameLogr автоматически будет делать скриншоты с определенной периодичностью
и сразу складывать в указанную Вами папку, а если Вы работаете - не будет делать ровным счетом ничего!

Конечно, возможность сделать скриншот любой программы или игры по горячей клавише также присутствует, равно как
и возможность посмотреть и отредактировать (удалить/добавить) базу игр. Изначальная база игр взята с сайта wakoopa.com (очень интересный сервис, который собирает информацию о существующих программах, в том числе и играх)

<table width=100% border=0 cellpadding=5>
<tr><td>

<br><br>Возможности:
<br>- GameLogr включается только если Вы играете во что либо!
<br>- Как только Вы запустили игру, GameLogr начнет делать скриншоты с заданной периодичностью абсолютно самостоятельно
<br>- Автосохранение скриншота на диск. Имя файла создается "на лету" и содержит дату снятия скриншота и название игры
<br>- Снятие скриншота по горячей клавише также присутсвует
<br>- Возможность посмотреть и отредактировать базу игр (добавить свои, удалить или поменять существующие)
<br><br>
Программа абсолютно бесплатна для некоммерческого использования и не содержит AdWare или SpyWare.
Для *коммерческого* использования все же нужно приобрести <a href="http://www.wiredplane.com/guestbook/vendors.php?who=comuse&vendor=2&src=site">лицензию</a>
<td width=10% align=right><img src="/images2/gamelogr/gamelogr.jpg" border=1 width=140>
</table>
<br>
<center>
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
<div width=100% align=center><a href="/download/gamelogr.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a></div>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
