<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "”правление системой с помощью гор. клавиш, полна€ автоматизаци€ работы за компьютером";
	}
	return "Automation tool for power users, Control your PC using hotkeys";
}
function insertSection($l){
?>

	<table border=0 cellspacing=0 cellpadding=0 width=100%>
	<tr><td><br><b>Preferences/Main window<hr></td></tr>
	<tr><td><img src="/images2/wirekeys/screens/wkeys3.gif"></td></tr>
	<tr><td><br><b>Tray menu<hr></td></tr>
	<tr><td><img src="/images2/wirekeys/screens/wkeys2.gif"></td></tr>
	<tr><td><hr><br><b>Application-specific Hot menu<hr></td></tr>
	<tr><td><img src="/images2/wirekeys/screens/wkeys4.gif"></td></tr>
	<tr><td><hr><br><b>'Kill any process' submenu<hr></td></tr>
	<tr><td><img src="/images2/wirekeys/screens/wkeys5.gif"></td></tr>
	<tr><td><hr><br><b>Tray icons menu of hidden to tray application (with application main menu)<hr></td></tr>
	<tr><td><img src="/images2/wirekeys/screens/wkeys1.gif"></td></tr>
	</table>

<br><br>
<a href="/download/wirechanger.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>

<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>