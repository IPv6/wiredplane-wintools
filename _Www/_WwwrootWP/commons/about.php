<?php 
function getTitle($l)
{
	global $language;
	return "Automation software for advanced users (macros, desktop, clocks, calendars)";
}
function insertSection($l){
?>
<table width="590" border="0" cellspacing="0" cellpadding="0" align="center" >
<?php
if($l==1){
?>
<tr><td align=center><a href="/download/AmuletSetup.zip"><img src="/images2/amulet/header.jpg" width=560></a>
<br><b>Амулет трехцветья. Логическая игра</b>
<br><a href="/download/AmuletSetup.zip">Download</a><br><br>
</td></tr>
<?php
}else{
?>
<tr><td align=center><a href="/download/AmuletSetup.zip"><img src="/images2/amulet/header-en.jpg" width=560></a>
<br><b>Amulet of tricolor. Puzzle</b>
<br><a href="/download/AmuletSetup.zip">Download</a><br><br>
</td></tr>
<?php
}
?>
<tr><td align=center><a href="../wirekeys/"><img src="/images2/wirekeys/splash.gif" width=560 height=180></a>
<br><b>WireKeys <?php insertVersion("wirekeys"); ?> Automation tool for power users</b>
<br><a href="/download/wirekeys.zip">Download</a>::<a href="../commons/order.php">Order</a>::<a href="../wirekeys/about.php">More info</a><br><br>
</td></tr>
<tr><td align=center><a href="../wirechanger/"><img src="/images2/wirechanger/splash.gif" width=560 height=180></a>
<br><b>WireChanger <?php insertVersion("wirechanger"); ?> Desktop/Wallpaper manager + widgets + PIM</b>
<br><a href="/download/wirechanger.zip">Download</a>::<a href="../commons/order.php">Order</a>::<a href="../wirechanger/about.php">More info</a><br><br>
</td></tr>
</table>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
