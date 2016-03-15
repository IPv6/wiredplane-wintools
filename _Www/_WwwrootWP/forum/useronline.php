<?php
/***************************************************************************
 *                            useronline.php
 *                            -------------------
 *
 *             see docs/copyright.txt for more details
 *
 *			Last modified 10 October 2003
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");

$username=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($username,"0","0");
if ($permit=="1") {
	$language=SetLanguage($username);
	include($temppath."/pb/language/lang_".$language.".php");
	$mem=$HTTP_COOKIE_VARS[$cookieid];
	if ($mem=="") {
	} else {
		$time = @time();
		$a = fopen($dbpath."/online/".$mem, "w");
		fputs($a, $time);
		fclose($a);
	}
	$count=0;
	$handle = opendir($dbpath.'/online');
	while ($file = readdir($handle)) {
		if ($file != "." && $file != ".." && $file != ".htaccess" && $file != "index.html") {
			$count++;
		}
	}
	closedir($handle);
	if ($count>1 || $count==0) {
		echo "<table cellspacing=0 cellpadding=0><tr><td width=\"30%\" valign=\"top\">$VThereAre $count ";
		$ph = get_phrase_for_num('$VMembers', $count,$username);    //this will figure out which plural form to use
		echo "$ph ";
	} else {
		echo "<table cellspacing=0 cellpadding=0><tr><td width=\"30%\">$VThereIs $count ";
		$ph = get_phrase_for_num('$VMembers', $count,$username);    //this will return the singular form
		echo "$ph ";
	}
	if ($groupcolors=="1"){
		$group1=GetRank("1");
		$group2=GetRank("2");
		$group3=GetRank("3");
		$group4=GetRank("4");
		$group5=GetRank("5");
		$group6=GetRank("10");
		$group7=GetRank("11");
		$group8=GetRank("12");
		echo "$VLoggedIn:</td><td width=\"70%\"> [ <font color=\"$group1color\">$group1</font> ] [ <font color=\"$group2color\">$group2</font> ]";
		echo " [ <font color=\"$group3color\">$group3</font> ] [ <font color=\"$group4color\">$group4</font> ]";
		echo " [ <font color=\"$group5color\">$group5</font> ]<BR>[ <font color=\"$group10color\">$group6</font> ]";
		echo " [ <font color=\"$group11color\">$group7</font> ] [ <font color=\"$group12color\">$group8</font> ] [ <font color=\"$modcolor\">$VModerator</font> ]";
		echo " [ <font color=\"$admincolor\">$VAdministrator</font> ]</td></tr></table>";
	}else{
		echo "$VLoggedIn:</td><td width=\"50%\"> [ <font color=\"$usercolor\">$VUser</font> ] [ <font color=\"$modcolor\">$VModerator</font> ]
			[ <font color=\"$admincolor\">$VAdministrator</font> ]</td></tr></table>";
	}
	$thiscount=0;
	$handle = opendir($dbpath.'/online');
	while ($file = readdir($handle)) {
		if ($file != "." && $file != ".." && $file!=".htaccess" && $file!="index.htm") {
			include("$dbpath/members/$file");
			if ($loggedin){               //don't allow profile access when not logged in
					echo "<a href=\"profile.php?u=$file\">";
			}
			if ($usermod=="1") {
				echo "<font color=\"$modcolor\">";
			}
			if ($useradmin=="1") {
				echo "<font color=\"$admincolor\">";
			}
			if ($groupcolors=="1"){
				if ($userrank=="1") {
					echo "<font color=\"$group1color\">";
				}
				if ($userrank=="2") {
					echo "<font color=\"$group2color\">";
				}
				if ($userrank=="3") {
					echo "<font color=\"$group3color\">";
				}
				if ($userrank=="4") {
					echo "<font color=\"$group4color\">";
				}
				if ($userrank=="5") {
					echo "<font color=\"$group5color\">";
				}
				if ($userrank=="10") {
					echo "<font color=\"$group10color\">";
				}
				if ($userrank=="11") {
					echo "<font color=\"$group11color\">";
				}
				if ($userrank=="12") {
					echo "<font color=\"$group12color\">";
				}
			}elseif ($useradmin!="1" && $usermod!="1"){
				echo "<font color=\"$usercolor\">";
			}
			include($dbpath."/members/".$file);
			echo "$useralias</font>";
			if ($loggedin){
				echo "</a>";
			}
			$thiscount++;
			if ($count>1 && $count !=$thiscount){        //no comma after the last logged on member
				echo ", ";
			}
		}
	}
	closedir($handle);

	$handle = opendir($dbpath.'/online');
	while ($file = readdir($handle)) {
		if ($file != "." && $file != "..") {
			$timer=5;
			$timeout = time()-(60*$timer);
			$filename = $dbpath."/online/".$file;
			$fd = fopen ($filename, "r");
			$ct = fread ($fd, filesize ($filename));
			fclose ($fd);
			if ($ct < $timeout) {
				unlink($dbpath."/online/".$file);
			}
		}
	}
	closedir($handle);
}
ob_end_flush();
?>


