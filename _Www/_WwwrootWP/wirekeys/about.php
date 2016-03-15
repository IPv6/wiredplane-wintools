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
<table width="590" border="0" cellspacing="0" cellpadding="0" align="center" >
<tr><td align=center><a href="info.php"><img src="/images2/wirekeys/splash.gif" width=560 height=180></a>
<br>WireKeys <?php insertVersion("wirekeys"); ?> Automation tool for power users
<br><a href="/download/wirekeys.zip">Download</a>::<a href="../commons/order.php">Order</a>::<a href="../wirekeys/info.php">More info</a>::<a href="../wirekeys/screens.php">Screenshots</a>
</td></tr>
<tr><td>
<div id=h1>About</div>
<img src="/images2/bar_big.gif" width=100% height=1>
<img src="/images2/tag.gif" width=100 height=100 vspace=10 hspace=20 class=right>
<div id=ptext><font>WireKeys</font> is a Windows NT4/2000/XP utility that speeds up routine tasks by reducing
the number of keystrokes required to perform them. If you're tired of installing one utility after another to fit all 
your needs, WireKeys is the solution you've been looking for. Now you can manage *all* your hotkeys from one place instead 
of remembering preferences of tens applications. And hotkey in WireKeys can be much more useful and effective than in any other application.
</div>
<div id=ptext><font>Automate all aspects</font> of everyday working with your computer. Control file 
and directory manipulation, text replacement, text processing, file downloads and uploads. Use an inline calculator. 
Create additional desktops. <font>Eliminate repetitive tasks</font> using <b>JavaScript-compatible</b> macro language.
Use your own intuitive hotkeys in combination with your mouse and screen edges. 
</div>
</td></tr>
</table>
<br>
<table width="590" border="0" cellspacing="0" cellpadding="0" align="center">
<tr valign=top><td>
<img src="/images2/wirekeys/boxshot.gif" width=165 height=196>
</td><td>
<div id=h1>User Quotes</div>
<img src="/images2/bar_small.gif" width=100% height=1>
<div id=ptext>"WireKeys is a dream come true! I can not say enough good things about it.  I use my computer constantly and know I will find hundreds and hundreds of uses for this magnificent program. Thank you!"</div>
<div id=quote_author>Martin Perelly</div>
<div id=ptext>"As a programmer, I need to insert some snipplets of text into the source code that I'm working on, e.g. for "tagging" changes in other people's work. The extended clipboards of WireKeys are a great way to do that with ease."</div>
<div id=quote_author>Oliver Baltz</div>
	<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
	<tr><td align=center>
	<a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
	<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
	</td></tr>
	</table>
</td></tr>
</table>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>
