<?php 
function getTitle($l)
{
	if($l==1){
		return "Пресс-релизы";
	}
	return "Press releases";
}
function insertSection($l){
?>

October , 2004
<br><br>
For Immediate Release
<br><br>
WiredPlane.com Release WireChanger 2.8
<br><br>
Turning PC desktop into a productivity booster
<br><br>
At first glance, WireChanger 2.8 is merely a wallpaper manager.  But, the fact is, this program offers quite a lot of nifty features to people who aren't interested in desktop beautifiers at all.  The most in demand feature in WireChanger is 'desktop sticky notes'.  Post-It notes and such can be found in every office.  With WireChanger any person can get the electronic analog that can be used on a PC desktop.  It's simple, it's effective and it's very helpful in everyday office routine.
<br>
Second, the program can install a calendar as a wallpaper.  If necessary, users can add a list of tasks they need to do on specified days or during specified time period.  Or mark friends and colleagues' birthdays. Plus, WireChanger even comes with a weather forecast template that is a must have for any company or individual whose business depends of weather conditions.  To have real-time weather updates, one needs to have Internet connection established
<br>
Folks who aren't looking for productivity enhancers but rather need a simple tool to manage their ever growing wallpaper collection are likely to fall in love with WireChanger as well. All wallpaper images can be split into different groups and rated so their appearance is not random, but rather driven by user likes and dislikes.  In addition, WireChanger supports a famous collection of clocks and time pieces from ColClocks.   These are real digital 'replicas' of Rolex, Cartier and other famous clock makers.
<br>
Another WireChanger unique feature is artistic special effects for wallpaper.  Things like morphing, image merging, smart picture enhancements many not add anything to productivity, but sure look cool on the desktop.  Plus, there are tons of other features that are common for most wallpaper managers - picture resizing, animated desktop support and so on.
<br>
WireChanger 2.8 is distributed electronically over the Internet; free demo version is available at www.wiredplane.com for evaluation.  
The price of a single copy is 20.00 US Dollars. 
<br><br>
If you have any questions, would like to request editor's copy, want to inquire about special prices for volume buyers/software resellers, or have a business proposal, please contact WirePlane.com staff at press@wiredplane.com<br>
System Requirements:<br>
·	Operating system: Windows 98/Me/2000/XP/2003<br>
·	Hard Disk:  4 Mb<br>
·	Display:  at least 800x600, 16b color<br>
<br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wc");
?>
