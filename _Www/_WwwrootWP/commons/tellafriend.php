<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "Рассказать друзьям";
	}
	return "Recommend to a Friend!";
}
function insertSection($l){
?>

<table border=0 width=90%>
<tr><td align=justify>
Enter you name, e-mail address, and one or more target e-mail addresses (separated by a comma or new line) 
and we will forward a recommendation on your behalf.
<form method="POST" action="../guestbook/tellafriend.php">
<input type="hidden" name="type" value="simple">
<input type="hidden" name="action" value="send">
<table border="0" cellpadding="0" width="100%" cellspacing="0">
<tr valign="top">
<td valign="middle">
<font face="verdana" size="2"><b>From:</b></font><br>
<img border="0" src="/img/clear.gif" width="1" height="5"><br>
<font face="verdana" size="1">your name</font><br>
<input type="text" name="from" size="25" value=""><br>
<font size="1" face="verdana">your email</font><br>
<input type="text" name="e_from" size="25" value=""><br>
<hr color="#CCCCCC" size="1" noshade><b><font face="verdana" size="2">To:</font></b><br>
<input type="text" name="e_to" size="25"></textarea><font face="verdana" size="2"><br>
</font><br>
<hr color="#CCCCCC" size="1" noshade><font face="verdana" size="2"><b>Message:<br>
</b></font><font face="verdana" size="1">subject</font><font face="verdana" size="2"><b><br>
</b></font><font color="#000000">
<input type="text" name="e_subject" size="55" value="Try this tool out!"> </font>
<font face="verdana" size="2"><b><br>
<br>
</b></font><font face="verdana" size="1">body</font><br>
<textarea wrap="true" rows="10" name="e_message" cols="45">Type your mesage here...
</textarea><hr>
<!--
Hi, I wanted to let you know about WireChanger, the wind of change for desktop.
WireChanger allows you to decorate your desktop with beautiful photos - for FREE! You can add gorgeous new photos to your desktop every day.
You can also use your desktop as a PIM with desktop sticky notes and clickable calendar!

Hi, I wanted to let you know about WireKeys, special All-in-one tool for every day.
If you're tired of installing one utility after another to fit all your needs, WireKeys is the solution you've been looking for. 

Hi, I wanted to let you know about WireNote, simple PIM with sticky notes and LAN messaging.
You can schedule Reminders to play sounds, display Notes or ToDos, send messages to other users, start programs, or even shut down your computer. You can completely orgnize your workplace with this tool.
Enjoy,
-->
<div align=center><input type="hidden" name="myRef" value="tell_a_friend"><input type="submit" name="Send" value="Send recommendation!"></div>
</td>
</tr>
</table>
</form>
</td></tr>
</table>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");

?>
