<?php 
function getTitle($l)
{
	global $language;
	return "ICQ for LineAge2";
}
function insertSection($l){
?>
<table border=0 width=90%>
<tr><td align=center>
<h2>Общение с Вашими друзьями в LineAge2 через ICQ</h2>
<hr>
Хотите общаться с Вашими друзьями в LineAge2 даже на работе, когда нельзя запустить игру?
<br>Тогда этот сервис - для Вас!
<p>Мы делаем перенаправление из внутреннего чата LineAge2 на Ваш номер ICQ и обратно.
<br>Просто добавьте в Ваш список контактов бота-посредника и все отправляемые ему сообщения будут перенаправляться в чат Вашим друзъям в LineAge2, а все полученные им ответы - обратно Вам в аську. 
<p>Это просто и удобно!
<br>
<br><form method="get" action="../commons/register-wp.php" class=menu>
<input type="hidden" name="who" value="icq4lineage">
<input type="submit" value="Зарегистрироваться">
</form>
<hr>
</td></tr>
</table>
<div style="clear: both; text-align:center;">
	<br>
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
</div>

<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
