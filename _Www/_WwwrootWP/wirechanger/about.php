<?php 
function getTitle($l)
{
	global $language;
	return "Desktop manager with interactive desktop calendar and PIM on descktop";
}
function insertSection($l){
?>
<table width="590" border="0" cellspacing="0" cellpadding="0" align="center" >
<tr><td align=center><a href="info.php"><img src="/images2/wirechanger/splash.gif" width=560 height=180></a>
<br>WireChanger <?php insertVersion("wirechanger"); ?> Desktop/Wallpaper manager + widgets + PIM
<br><a href="/download/wirechanger.zip">Download</a>::<a href="../commons/order.php">Order</a>::<a href="../wirechanger/info.php">More info</a>::<a href="../wirechanger/screens.php">Screenshots</a>
</td></tr>
<tr><td>
<div id=h1>About</div>
<img src="/images2/bar_big.gif" width=100% height=1>
<img src="/images2/tag.gif" width=100 height=100 vspace=10 hspace=20 class=right>
<div id=ptext>WireChanger is a Wallpaper manager that includes built-in utilities, templates, calendar and the option to add active clocks to your desktop. WireChanger offers you total and complete solution to manage your own desktop wallpapers, especially If you want to see something fresh every time you minimize your applications.</div>
<div id=ptext>You may never see the same picture twice on your desktop! The main idea of this software is to set images on wallpaper with additional random effect applied to background. Animate your desktop with real clocks. Organize yourself with clickable wallpaper notes and calendar with reminders for important dates.</div>
</td></tr>
</table>
<br>
<table width="590" border="0" cellspacing="0" cellpadding="0" align="center">
<tr valign=top><td>
<img src="/images2/wirechanger/boxshot.gif" width=165 height=196>
</td><td>
<div id=h1>User Quotes</div>
<img src="/images2/bar_small.gif" width=100% height=1>
<div id=ptext>"The program is fascinating! It has everything I need on my desktop! I like changing wallpapers but WireChanger gives me so much more! It has the options I`ve never seen in any other program!"</div>
<div id=quote_author>Leela</div>
<div id=ptext>"I have probably tried just about EVERY other wallpaper manager out there, and WireChanger is easily the best I have ever seen."</div>
<div id=quote_author>Adam Poelsky</div>
	<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
	<tr><td align=center>
	<a href="/download/wirechanger.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
	<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
	</td></tr>
	</table>
</td></tr>
</table>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wc");
?>
