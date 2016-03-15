<?php 
function getTitle($l)
{
	global $language;
	return "Wind of change for your desktop. Add widgets, notes, interactive calendar and clocks on your desktop in a single click. Set reminders for future dates. Turn desktop into convinient PIM.";
}
function insertSection($l){
?>

	<table border=0 cellspacing=0 cellpadding=0 width=100%>
	<tr><td><br><b>Wallpaper examples<hr></td></tr>
	<tr><td align=center colspan=2>
	<img src="/images2/wirechanger/screens/prescr.gif"><br>
	<small>What you get with WireChanger</small><br>
	<tr><td><table border=0 width=100%><tr><td align=center>
	<img src="/images2/wirechanger/screens/wchanger5.jpg"><br>
	<small>Full color background with Memo Pad</small><br>
	</td><td align=center>
	<img src="/images2/wirechanger/screens/wchanger6.jpg"><br>
	<small>Grayscale background</small><br>
	</td></tr></table></td></tr>
	<tr><td><table border=0 width=100%><tr><td align=center>
	<img src="/images2/wirechanger/screens/wchanger7.jpg"><br>
	<small>Image mixer in action</small><br>
	</td><td align=center>
	<img src="/images2/wirechanger/screens/wchanger8.jpg"><br>
	<small>Calendar with notes and icons in cells</small><br>
	</td></tr></table></td></tr>
	<tr><td><table border=0 width=100%><tr><td align=center>
	<img src="/images2/wirechanger/screens/wchanger9.jpg"><br>
	<small>Clock, monthly calendar and sticky note</small><br>
	</td><td align=center><img src="/images2/wirechanger/screens/wchanger10.jpg"><br>
	<small>Kaleidoscope effect with minilendar</small><br>
	</td></tr></table></td></tr>
	<tr><td><table border=0 width=100%><tr><td align=center>
	<img src="/images2/wirechanger/screens/wchanger12.jpg"><br>
	<small>Minilendar with icons and tooltip</small><br>
	</td><td align=center><img src="/images2/wirechanger/screens/wchanger13.jpg"><br>
	<small>Note and weather report</small><br>
	</td></tr></table></td></tr>
	<tr><td align=center><hr><br><b>WireChanger main window<hr></td></tr>
	<tr><td align=center><img src="/images2/wirechanger/screens/wchanger1.gif"></td></tr>
	<tr><td align=center><hr><br><b>Installed Widgets window<hr></td></tr>
	<tr><td align=center><img src="/images2/wirechanger/screens/wchanger2.gif"></td></tr>
	<tr><td align=center><hr><br><b>WireChanger tray popup menu and desktop note<hr></td></tr>
	<tr><td align=center><img src="/images2/wirechanger/screens/wchanger5.gif"></td></tr>
	<tr><td align=center><hr><br><b>Calendar template parameters window<hr></td></tr>
	<tr><td align=center><img src="/images2/wirechanger/screens/wchanger3.gif"></td></tr>
	<tr><td align=center><hr><br><b>Reminder window<hr></td></tr>
	<tr><td align=center><img src="/images2/wirechanger/screens/wchanger6.gif"</td></tr>
	</table>

<br><br>
<a href="/download/wirechanger.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>

<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wc");
?>