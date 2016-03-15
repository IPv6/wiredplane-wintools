<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "KaleidosWallpaper -  алейдоскоп - генератор обоев";
	}
	return "KaleidosWallpaper - Kaleidoscopic wallpaper generator";
}
function insertSection($l){
?>
<div id=h1>KaleidosWallpaper</div>
<table border=0 width=100% cellspacing=0 cellpadding=6>
<tr>
<td width=80% valign=top>
<?php if($l==1){?>
Ёта маленька€ и проста€ программа позвол€ет генерировать удивительные и красивые обои из любого набора картинокю ѕросто задайте в настройках каталог с картинками и через заданные промежутки времени KaleidosWallpaper создаст на десктопе новое интересное изображение.
<br><br>ѕрограмма абсолютно бесплатна во всех отношени€х и не содержит AdWare или SpyWare.
<br><br>¬ы также можете помочь нам в дальнейшем развитии этой программы, <a href="/guestbook/vendors.php?who=KaleidosWallpaper&vendor=2&src=site">пожертвовав небольшую сумму денег</a>
<?php }else{?>
This is small wallpaper generator based on Kaleidoscope effect. Just point it to the folder with random images and it will set interesting masterpiece as wallpaper after specified amount of time.
<br><br>This program is totally FREE and does not contain adware or spyware of any kind.
<br><br>But you can <a href="/guestbook/vendors.php?who=KaleidosWallpaper&vendor=2&src=site">donate us</a> if you find it interesting
<?php }?>
<td align=right>
<img src="/images2/kaleidoswallpaper/kaleidoswallpaper3.jpg" border=1 width=240>
<tr><td colspan=2 align=center>
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
<a href="/download/kaleidoswallpaper.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
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
<tr>
<td align=left width=80%>
<img src="/images2/kaleidoswallpaper/kaleidoswallpaper1.jpg" border=1 width=240>
<td align=right>
<img src="/images2/kaleidoswallpaper/kaleidoswallpaper2.jpg" border=1 width=240>
</table>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
