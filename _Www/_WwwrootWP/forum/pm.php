<?php
/***************************************************************************
 *                            pm.php  - PBLang
 *                            -------------------
 *
 *                  see docs/coypright.txt for more details
 *
 *			Last modified 10 October 2003
 ***************************************************************************/


require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("scan.php");
include_once("functions.php");
include_once("ffunctions.php");

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1") {
	$language=SetLanguage($liusername);
	include("$temppath/pb/language/lang_$language.php");
	writeheader($newestm,0,0,0,$liusername,"1",$PrivateMessaging);
		//     pmtop($liusername);
	WriteTableTop();
	echo "<tr><td bgcolor=\"$headercolor\" height=\"15\" background=\"$headergif\" colspan=\"4\">";
	echo "<a href=\"index.php\"><font color=\"$headerfont\">$sitetitle</font></a><font color=\"$headerfont\"> :: $PrivateMessaging :: $VInbox</font></td></tr>";
	echo "<tr><td class=\"subheader\" colspan=\"4\"><a href=\"pm.php\">$VInbox</a> :: <a href=\"sendpm.php\">$VSendPM</a></td></tr>";
	$filename = $dbpath."/pm/".$liusername;
	$fd = fopen ($filename, "r");
	$num = fread ($fd, filesize ($filename));
	fclose ($fd);
	$filename = $dbpath."/pm/".$liusername."_tot";
	if (!file_exists($filename))
		{$filename=str_replace("_"," ",$filename);}
	$fd = fopen ($filename, "r");
	$num2 = fread ($fd, filesize ($filename));
	fclose ($fd);
	if ($num=="0" || $num2=="0") {
		echo "<tr bgcolor=\"$menucolor\" valign=\"top\"><td height=\"0\" bgcolor=\"$fmenucolor\" width=\"100%\">$NoMessages!</td>";
	}else{
		echo "</table>";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"7\" cellspacing=\"1\">";
		echo "<tr bgcolor=\"$subheadercolor\" valign=\"middle\"> ";
		echo "<td height=\"10\" align=\"left\" bgcolor=\"$forumbuttoncolor\" width=\"10%\">";
		echo "<p>$VFrom</td>";
		echo "<td height=\"10\" align=\"left\" bgcolor=\"$forumbuttoncolor\" width=\"30%\">";
		echo "<p>$VSubject</td>";
		echo "<td height=\"10\" align=\"left\" bgcolor=\"$forumbuttoncolor\" width=\"10%\">";
		echo "<p>$VDate</td>";
		echo "<td height=\"10\" align=\"left\" bgcolor=\"$forumbuttoncolor\" >";
		echo "<p>$VStatus</td>";
	}

	$i=$num;
	while ($i<=$num && $i>0) {
			$filename = "$dbpath/pm/$liusername"."_$i"."_c";
			if (!file_exists($filename))
				{$filename=str_replace("_"," ",$filename);}
			if (file_exists($filename)){
				$fd = fopen ($filename, "r");
				$msg = fread ($fd, filesize ($filename));
				fclose ($fd);

				if($msg!="delplz") {
					$filename = "$dbpath/pm/$liusername"."_$i"."_d";
					if (!file_exists($filename))
						{$filename=str_replace("_"," ",$filename);}
					$fd = fopen ($filename, "r");
					$date = fread ($fd, filesize ($filename));
					fclose ($fd);

					$filename = "$dbpath/pm/$liusername"."_$i"."_a";
					if (!file_exists($filename))
						{$filename=str_replace("_"," ",$filename);}
					$fd = fopen ($filename, "r");
					$auth = fread ($fd, filesize ($filename));
					fclose ($fd);

					$filename = "$dbpath/pm/$liusername"."_$i"."_s";
					$fd = @fopen ($filename, "r");
					$subj = @fread ($fd, filesize ($filename));
					@fclose ($fd);

					$filename = "$dbpath/pm/$liusername"."_$i"."_pmstat";
					$fd = @fopen ($filename, "r");
					$pmstat = @fread ($fd, filesize ($filename));
					@fclose ($fd);

					if ($censor=="1"){
						$msg=wcensor($msg,$liusername);
					}
					$msg=replacestuff($msg);
					$msg=stripslashes($msg);
					//               pmp($auth,$msg,$date,$subj,$pmstat,$liusername,$i);
					if ($pmstat=="1"){
						$bold="<b>";
						$ebold="</b>";
					}else{
						$bold="";
						$ebold="";
					}
					$date=strftime($DateFormat1,$date);
					echo "<tr bgcolor=\"$subheadercolor\" valign=\"middle\"> ";
					echo "<td height=\"10\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"10%\">";
					$filename=$dbpath."/members/".$auth;
					if (file_exists($filename)){
						include ($filename);
						$author=$useralias;
					}else{
						$author=$VRemoved;
					}
					echo "<p><font color=\"$menufont\">$bold<a href=\"profile.php?u=$auth\" title=\"$VProfile $useralias\">$author</a>$ebold</font></td>";
					echo "<td height=\"10\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"50%\">";
					if (trim($subj)=="")
						{$subj="--- $VNoSubject ---";}
					echo "<p><a href=\"pmpshow.php?num=$i\">$bold$subj$ebold</a></td>";
					echo "<td height=\"10\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"10%\">";
					echo "<p>$bold$date$ebold</td>";
					echo "<td height=\"10\" align=\"left\" bgcolor=\"$fmenucolor\"  width=\"15%\">";
					if ($pmstat=="1"){       //PM is new
						echo "<p><b>$VNew</b></td></tr>";
					}elseif ($pmstat=="2"){
						echo "<p>$VRead</td></tr>";
					}elseif ($pmstat=="3"){
						echo "<p>$VReplied</td></tr>";
					}
				}
			}
			$i--;
		}
		echo "</table></td></tr></table></td></tr></table>";
		writefooter($newestm);
	}else{
		writeheader($newestm,0,0,0,$liusername,"",$PrivateMessaging);
		include ("login.php");
	}

ob_end_flush();
?>


