<?php 
include("../cgi-bin/wp_layout.php");
function getTitle($l)
{
	global $language;
	if($l==1){
		return "–егистраци€ в сервисе";
	}
	return "Registration";
}
function insertSection($l){
	$prod=_var("Prod","");
	if($prod==""){
		$who=_var("who","none");
		$prod=$who;
	}
?>

<center>

<div style="text-align:center;">
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
<!-- ****************************************************** -->
<table width="98%"><tr><td align="center">
<font size=+1>  сожалению, пока данный серсис находитс€ в стадии бета-тестировани€</font>
<br>
<br> –егистраци€ новых пользователей работает лишь по приглашени€м. ќставьте свой email и мы вышлем приглашение на регистрацию в серисве когда по€витс€ така€ возможность и сервис станет доступным дл€ всех!
<br>
<br>
<form method=post action="../guestbook/uninstall.php">
<input type="hidden" name="action" value="send">
<input type="hidden" name="who" value="<?php print $prod; ?>">
<input type="hidden" name="opinion" value="Test for <?php print $prod; ?>">
<br>¬аш email: <INPUT type=text value="" name=email size=20>
<br>
<br><input type=submit value="ќтправить запрос на регистрацию">
</form>
</table>
</center>
<?php 
}
printLayout("common");
?>
