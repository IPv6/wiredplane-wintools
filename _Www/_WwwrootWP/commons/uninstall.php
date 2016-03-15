<?php 
include("../cgi-bin/wp_layout.php");
function getTitle($l)
{
	global $language;
	if($l==1){
		return "ƒеинсталл€ци€";
	}
	return "Uninstall";
}
function insertSection($l){
	$prod=_var("Prod","");
	if($prod==""){
		$who=_var("who","none");
		$prod=$who;
	}
?>

<center>

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
<!-- ****************************************************** -->
<?php if(strtolower($prod)=='wirekeys'){ ?>
<hr>
<p><center><font size=+1><font color=red>NEW!</font> Special promotion!<br> Right now, You can get WireKeys
<br>completely <font color=blue><b>FREE of charge</b></font>!
<br><b>Just <font color=red>follow this link</font>:</b><br>
<a target="_blank" href="http://www.trialpay.com/productpage/?c=1qlrh5"><img border="0" src="http://images.trialpay.com/buttons/buttons_08.gif"></a>
</font></center></p>
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
<hr>
<?php } ?>
<!-- ****************************************************** -->
<?php if(strtolower($prod)=='wirechanger'){ ?>
<hr>
<p><center><font size=+1><font color=red>NEW!</font> Special promotion!<br> Right now, You can get WireChanger
<br>completely <font color=blue><b>FREE of charge</b></font>!
<br><b>Just <font color=red>follow this link</font>:</b><br>
<a target="_blank" href="http://www.trialpay.com/productpage/?c=4e73121"><img border="0" src="http://images.trialpay.com/buttons/buttons_08.gif"></a>
</font></center></p>
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
<hr>
<?php } ?>
<!-- ****************************************************** -->
Thank you for your interest to our product! Perhaps you had a reason to delete it from your computer.
We are sorry to see you leave. Please help us improve. Your opinion matters. If you have a couple of minutes, 
please fill out this form to help us create a more relevant and useful product. Thank you in advance!
<hr>
<form method=post action="../guestbook/uninstall.php">
<input type="hidden" name="action" value="send">
<input type="hidden" name="who" value="<?php print $prod; ?>">
We would be happy if you could answer 3 questions. If you have a couple of minutes,<br>
please fill out this form to help us create a more relevant and useful product. Thank you.<br>
<br>
<table border=0 width=60%>
<tr><td align=left>I am uninstalling because...
<tr><td align=left><INPUT type=checkbox value=yes name=other> I will use other products or services
<tr><td align=left><INPUT type=checkbox value=yes name=features> Some features I need are missing
<tr><td align=left><INPUT type=checkbox value=yes name=complicated> Too complicated <INPUT type=checkbox value=yes name=unstable> Unstable
<tr><td align=left><INPUT type=checkbox value=yes name=later> I like the program, but I'll come to a decision later
</table>
<br>If you wish to receive an answer, leave your email: <INPUT type=text value="" name=email size=20>
<br><B><INPUT type=checkbox value=yes name=hint> Other (If possible, give us a reason in the comment field below.)</B>
<br><textarea cols=60 rows=10 name="opinion"></textarea>
<br><input type=submit value="Send your opinion to WiredPlane Labs">
</form>
</center>
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
printLayout("common");
?>
