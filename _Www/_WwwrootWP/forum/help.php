<?php
/***************************************************************************
 *                            help.php
 *                            -------------------
 *             see copyright.txt in the docs-folder
 *
 *	last modified 7 October 2003
 **************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once($dbpath."/settings/styles/styles.php");

$username=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($username,"0","0");

if ($permit=="1") {
     $language=SetLanguage($username);
     include("$temppath/pb/language/lang_$language.php");
     $new="nope";
     writeheader($new,0,0,0,$username,$loggedin,$VHelp);
     WriteTableTop();
     echo "<tr>";
     echo "<td bgcolor=\"$headercolor\" height=\"27\" background=\"$headergif\">$sitetitle :: $VHelp</td></tr>";
     echo "<tr bgcolor=\"$subheadercolor\"><td height=\"36\" align=\"left\" bgcolor=\"$menucolor\" width=\"45%\" valign=\"middle\">";
     echo "<p>PBLCode $VExplanation:</p><p>[img][/img] $VHelpIMG.<br>";
     echo "<font color=\"$fontcolor\"><br>$VUsage: [img]http://www.yourdomain.com/PBLang/imagespath/help.gif[/img]</font> <br>";
     echo "<font color=\"$fontcolor\">$VDisplays: <img src=\"templates/pb/images/top/help.gif\"></font><br>";
     echo "<br><font color=\"$fontcolor\"><br>[url][/url] $VHelpURL.<br><br>
	 	$VUsage: [url]http://www.yourdomain.com[/url]</font><br>";
     echo "$VDisplays: <a href=\"http://yourdomain.com\">www.yourdomain.com</a><br><br><br>";
     echo "[email][/email] $VHelpEMAIL.<br><br>$VUsage: [email]yourname@yourdomain.com[/email]<br>";
     echo "$VDisplays: <a href=\"mailto:yourname@yourdomain.com\">yourname@yourdomain.com</a><br><br><br>";
     echo "[glowCOLOR][/glowCOLOR] ".$VHelpGLOW."<br><br>".$VUsage.": [glowred]".$GlowText."[/glowred]<br>";
     echo "$VDisplays</font></font></p><table style=\"Filter: Glow(Color=red, Strength=2)\" width=300>$GlowText</table><br>";
     echo "[i][/i] ".$VHelpITALICS.".<br><br>".$VUsage.": [i]".$ItalicText."[/i] <br><font color=\"$fontcolor\">";
     echo "$VDisplays: <i>$ItalicText</i><br><br>";
     echo "[b][/b] ".$VHelpBold.".<br><br>".$VUsage.": [b]".$BoldText."[/b]<br>".$VDisplays.":<b> ".$BoldText."</b><br> </font><br>";
     sbot(0);
     writefooter($newestm);
}
ob_end_flush();
?>
