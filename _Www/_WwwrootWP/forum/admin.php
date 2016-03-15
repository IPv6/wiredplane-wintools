<?php
/***************************************************************************
 *                            admin.php   -  PBLang
 *                            -------------------
 *
 *              see the file copyright.txt in the docs-folder
 *
 *             updated 27th August 2003
***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include($dbpath."/settings/styles/styles.php");

$oldcver=$cver;
$oldcsubver=$csubver;
$oldcstat=$cstat;
$oldcstatlevel=$cstatlevel;  //this ensures that the saved version-number can be checked against the current version
$cver='4';
$csubver='60';
$cstat='';		//alpha, beta or rc
$cstatlevel='';
$LF="\n";
if ($oldver != $cver || $oldcsubver!=$csubver || $oldcstat!=$cstat || $oldcstatlevel!=$cstatlevel){          //if the version that is used does not match the stored version number, the stored version number will be updated to show
	WriteSettings();
}
$username=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($username,"1","0");
if ($admin!="1" && $moder!="1") {
	echo $NoAccess;
	ErrorMessage($AccessDenied,$username);
	exit;
} elseif (($admin=="1" || $moder=="1") && $permit=="1") {
	$do=$HTTP_GET_VARS['do'];
	$lang=SetLanguage($username);
	include($temppath."/pb/language/lang_".$lang.".php");

	writeheader($newestm,0,0,0,$username,"1",$AdminCenter);
	admintop();
//	echo "</td></tr></table>";
	if ($do=="news") {
		/*****************************************************************
		Do not manipulate the following lines! They guarantee that you will see any relevant messages
		about the progress of PBLang, updated versions etc. You'll also be told if a new version is available.
		If you change the following lines (up to and including $nnews), you'll never know when a new version
		is being released!
		*****************************************************************/
		$newsfn='http://www.drmartinus.de/PBLang/latest.txt';
//		if (file_exists($newsfn)){
			$cont =@file($newsfn);
//		}
		if ($cont!=""){
			$completecontent = implode(' ',$cont);
		}else{
			$VNoNews;
		}
		$nover='';
		$verfn='http://www.drmartinus.de/PBLang/nversion.txt';
//		if (file_exists($verfn)){
			@include($verfn);
//		}else{
//			$nover=1;
//		}
		if (!$nover){
			$vver=$ncver.'.'.$ncsubver;
		}else{
			$vver='sorry';
		}
		$nnews="News and support at <a href=\"http://PBLang.drmartinus.de/\" target=\"_blank\">PBLang Project page</a>.";
		$nnews.="<br>";
		/* Here end the lines which you should never change!!! */
		echo "<td  class=\"adminform\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\" bgcolor=\"$menucolor\"><tr>";
		echo "<td bgcolor=\"$fmenucolor\"><font size=\"1\">$PBNews:</font></td></tr>";
		echo "<tr><td bgcolor=\"$fmenucolor\">$completecontent</td></tr></table><br>";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">$LF";
		echo "<tr><td bgcolor=\"$menucolor\"><font size=\"1\">$PBLSupportLink:</font></td></tr>";
		echo "<tr><td bgcolor=\"$menucolor\">$nnews</td></tr></table><br>";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td bgcolor=\"$menucolor\"><font size=\"1\">$VWhatIsThe $AdminCenter?</font></td></tr>$LF";
		echo "<tr><td height=\"20\" bgcolor=\"$menucolor\">$AdminCenterExplain</td></tr></table><br>";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td bgcolor=\"$menucolor\"><font size=\"1\">$VVersionCompare:</font></td></tr>$LF";
		echo "<tr><td height=\"20\" bgcolor=\"$menucolor\">$CompareString<br><font color=\"#FF0000\">
			$vver</font>/<font color=\"#FF0000\">$cver.$csubver$cstat$cstatlevel</font><br>";
		if ($ncver>$cver || ($ncver==$cver && $ncsubver>$csubver))
		{
			$suggupd='1';
		}
		if ($ncver==$cver && $ncsubver==$csubver && $cstat=''){
			$suggupd='0';
		}
		if ($ncver==$cver && $ncsubver==$csubver && ($cstat='rc' || $cstat=='beta' || $cstat=='alpha')){
			$suggupd='3';
		}
		if ($ncver<$cver || ($ncver==$cver && $ncsubver<$csubver) || $cstat=='rc' || $cstat=='beta' || $cstat=='alpha'){
			$suggupd='2';
		}
		if ($suggupd=='1'){
			echo $NewerRelease;
		}elseif ($suggupd=='2'){
			echo $TestRelease;
		}elseif($suggupd=='3'){
			echo $StillBeta;
		}else{
			echo $LatestRelease;
		}
		echo "</td></tr></table>";
	}elseif ($do=="") {
//		echo "</td></tr></table></td></tr></table>";
//		exit;
	}

	if ($do=="fsettings" && $admin=="1") {
		include('admsettings.php');
	} elseif ($do=="fsettings") {
		FAccessDenied();
	}

	if ($do=="predefstyles" && $admin=="1") {
	//selecting a preset style (skin)
//		echo "<td height=\"399\" bgcolor=\"$menucolor\" width=\"81%\" valign=\"top\">";
		echo "<td class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=predefstyles2\">
			<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\" bgcolor=\"$fmenucolor\">$LF";
		echo "<tr><td colspan=\"4\" bgcolor=\"$fmenucolor\"><font size=\"2\">$VPredefStyles:</font></td></tr>";
		echo "<tr><td width=\"35%\"><select name=\"defstyle\">";
		$handle=opendir("$dbpath/settings/styles");
		while($file=readdir($handle)){
			if($file!="." && $file!=".." && $file!=".htaccess" && $file!="index.html" && $file!="styles.php" && $file!="styles_base.php"){
				$stylefile=strtok($file,".");
				echo "<option value=\"$file\">$stylefile</option>";
			}
		}
		closedir($handle);
		echo "<tr><td width=\"24%\" align=\"right\">&nbsp;</td><td width=\"76%\" valign=\"bottom\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr></table></form>$LF";

	}elseif ($do=="predefstyles"){
		FAccessDenied();
	}


	if ($do=="styles" && $admin=="1") {
		include($dbpath."/settings.php");
		include($dbpath."/settings/styles/styles.php");
		include('admstyles.php');
	} elseif ($do=="styles") {
		FAccessDenied();
	}

	if ($do=="tempedit" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\" bgcolor=\"$menucolor\">";
		echo "<tr><td><font size=\"1\">$VNoTemplatesEdit</font></td></tr>";
		$te=$HTTP_GET_VARS['te'];
		$filename = "$temppath/pb/language/$te";
		$fd = fopen ($filename, "r");
		$stuff = fread ($fd, filesize ($filename));
		fclose ($fd);
		echo "<td class=\"adminform\"><form method=\"post\" action=\"admin2.php?do=temp2&te=$te\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td><font size=\"1\">$VTemplateEditor:</font></td></tr>";
		echo "<tr><td width=\"86%\"><textarea name=\"cont\" cols=\"80\" rows=\"30\">$stuff </textarea></td></tr>";
		echo "<tr><td width=\"86%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr>";
		echo "<tr><td width=\"86%\">$TemplateEditWarning</td></tr></table></form>";

	} elseif ($do=="tempedit") {
		FAccessDenied();
	}

	if ($do=="templates" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=fsettings2\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td><font size=\"1\">$VTemplates:</font></td></tr>";
		echo "<tr valign=\"bottom\"><td width=\"86%\" height=\"22\">$ChooseTemplate</td></tr>";
		echo "<tr><td width=\"86%\">";
		include("db/settings.php");
		$handle = @opendir('templates/pb/language/');
		while ($file = readdir($handle)) {
			if ($file != "." && $file != "..") {
				echo "<a href=\"admin.php?do=tempedit&te=$file\">$file</a><BR>";
			}
		}

		closedir($handle);
		echo "</td></tr></table></form>";

	} elseif ($do=="templates") {
		FAccessDenied();
	}

	if ($do=="mgroups" && $admin=="1") {
		include("$dbpath/status/ranges");
		$filename = "$dbpath/status/1";
		$fd = fopen ($filename, "r");
		$one = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/2";
		$fd = fopen ($filename, "r");
		$two = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/3";
		$fd = fopen ($filename, "r");
		$three = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/4";
		$fd = fopen ($filename, "r");
		$four = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/5";
		$fd = fopen ($filename, "r");
		$five = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/6";
		$fd = fopen ($filename, "r");
		$six = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/7";
		$fd = fopen ($filename, "r");
		$sev = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/8";
		$fd = fopen ($filename, "r");
		$ban = fread ($fd, filesize ($filename));
		fclose ($fd);
		$filename = "$dbpath/status/9";
		$fd = @fopen ($filename, "r");
		$friend = @fread ($fd, filesize ($filename));
		@fclose ($fd);
		$filename = "$dbpath/status/10";
		$fd = @fopen ($filename, "r");
		$ten = @fread ($fd, filesize ($filename));
		@fclose ($fd);
		$filename = "$dbpath/status/11";
		$fd = @fopen ($filename, "r");
		$eleven = @fread ($fd, filesize ($filename));
		@fclose ($fd);
		$filename = "$dbpath/status/12";
		$fd = @fopen ($filename, "r");
		$twelve = @fread ($fd, filesize ($filename));
		@fclose ($fd);
		$filename = "$dbpath/status/styles";
		$fd = @fopen ($filename, "r");
		$rankstyle = @fread ($fd, filesize ($filename));
		@fclose ($fd);

		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=mg2\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td colspan=\"4\"><font size=\"1\">$VMemberGroups:</font></td></tr>";
		echo "<tr valign=\"bottom\"><td height=\"22\" colspan=\"4\">$VEditMembergroups</td></tr>";
		echo "<tr><td width=\"20%\" align=\"right\">$VLessThan</td>";
		echo " <td width=\"15%\"><input type=\"text\" name=\"range1\" value=\"$range1\" size=\"10\"></td>";
		$ph = get_phrase_for_num('$VPosts', $range1,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"one\" value=\"$one\"></td></tr>";
		echo "<tr><td width=\"20%\" align=\"right\">$VLessThan</td>";
		echo " <td width=\"15%\"><input type=\"text\" name=\"range2\" value=\"$range2\" size=\"10\"></td>";
		$ph = get_phrase_for_num('$VPosts', $range2,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"two\" value=\"$two\"></td></tr>";
		echo "<tr><td width=\"20%\" align=\"right\">$VLessThan</td>";
		echo " <td width=\"15%\"><input type=\"text\" name=\"range3\" value=\"$range3\" size=\"10\"></td>";
		$ph = get_phrase_for_num('$VPosts', $range3,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"three\" value=\"$three\"></td></tr>";
		echo "<tr><td width=\"20%\" align=\"right\">$VLessThan</td>";
		echo " <td width=\"15%\"><input type=\"text\" name=\"range4\" value=\"$range4\" size=\"10\"></td>";
		$ph = get_phrase_for_num('$VPosts', $range4,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"four\" value=\"$four\"></td></tr>";

		echo "<tr><td width=\"20%\" align=\"right\">$VLessThan</td>";
		echo " <td width=\"15%\"><input type=\"text\" name=\"range5\" value=\"$range5\" size=\"10\"></td>";
		$ph = get_phrase_for_num('$VPosts', $range5,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"five\" value=\"$five\"></td></tr>";

		echo "<tr><td width=\"20%\" align=\"right\">$VLessThan</td>";
		echo " <td width=\"15%\"><input type=\"text\" name=\"range6\" value=\"$range6\" size=\"10\"></td>";
		$ph = get_phrase_for_num('$VPosts', $range6,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"ten\" value=\"$ten\"></td></tr>";

		echo "<tr><td width=\"20%\" align=\"right\">$VLessThan</td>";
		echo " <td width=\"15%\"><input type=\"text\" name=\"range7\" value=\"$range7\" size=\"10\"></td>";
		$ph = get_phrase_for_num('$VPosts', $range7,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"eleven\" value=\"$eleven\"></td></tr>";

		echo "<tr><td width=\"20%\" align=\"right\">$VMoreThan</td>";
		echo " <td width=\"15%\">$range7</td>";
		$ph = get_phrase_for_num('$VPosts', $range7,$username);
		echo "<td width=\"5%\"> $ph:</td>";
		echo "<td width=\"80%\" align=\"left\"><input type=\"text\" name=\"twelve\" value=\"$twelve\"></td></tr>";

		echo "<tr><td></td><td></td><td>$VModerator</td><td width=\"55%\" align=\"left\"><input type=\"text\" name=\"mod\" value=\"$six\"></td></tr>";
		echo "<tr><td></td><td></td><td>$VAdministrator</td><td width=\"55%\" align=\"left\"><input type=\"text\" name=\"admin\" value=\"$sev\"></td></tr>";
		echo "<tr><td></td><td></td><td>$VBanned</td><td width=\"55%\" align=\"left\"><input type=\"text\" name=\"ban\" value=\"$ban\"></td></tr>";
		echo "<tr><td></td><td></td><td>$VFriend</td><td width=\"55%\" align=\"left\"><input type=\"text\" name=\"friend\" value=\"$friend\"></td></tr>";
		echo "<tr><td colspan=\"5\">$VExplainFriend</td>";

		echo "<tr><td width=\"24%\" align=\"right\">$VRankImages: </td><td width=\"15%\"><select name=\"rankimg\">";
		if ($rankstyle=="default"){
			echo "<option value=\"default\" selected>$VDefault</option>";
		}else{
			echo "<option value=\"default\">$VDefault</option>";
		}
		if ($rankstyle=="burningflames"){
			echo "<option value=\"burningflames\" selected>$VBurningFlames</option>";
		}else{
			echo "<option value=\"burningflames\">$VBurningFlames</option>";
		}
		if ($rankstyle=="flames"){
			echo "<option value=\"flames\" selected>$VFlames</option>";
		}else{
			echo "<option value=\"flames\">$VFlames</option>";
		}
		if ($rankstyle=="stars"){
			echo "<option value=\"stars\" selected>$VStars</option>";
		}else{
			echo "<option value=\"stars\">$VStars</option>";
		}
		if ($rankstyle=="ar_grey"){
			echo "<option value=\"ar_grey\" selected>ar_grey</option>";
		}else{
			echo "<option value=\"ar_grey\">ar_grey</option>";
		}
		echo "</select>&nbsp;<img src=\"templates/pb/images/ranks/$rankstyle/rank5.gif\"></td></tr>";

		echo "<td width=\"15%\" align=\"right\">&nbsp;</td><td width=\"5%\"><br><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td>";
		echo "<td>&nbsp;</td></tr></table></form>";

	} elseif ($do=="mgroups") {
		FAccessDenied();
	}

	if ($do=="catadd" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=ca2\"><table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td colspan=\"2\"><font size=\"1\">$VAddCategory:</font></td></tr>";
		echo "<tr valign=\"bottom\"><td height=\"22\" colspan=\"2\">$ExplainAddCategory.</td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VName:</td><td width=\"69%\"><input type=\"text\" name=\"name\" size=\"75\"></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VDesc:</td><td width=\"69%\"><input type=\"text\" name=\"dec\" size=\"75\"></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">&nbsp;</td><td width=\"69%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr></table></form>";
	} elseif ($do=="catadd") {
		FAccessDenied();
	}

	if ($do=="editcat" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		$filename = $dbpath."/cats/cats";
		$fd = fopen ($filename, "r");
		$cats = fread ($fd, filesize ($filename));
		fclose ($fd);
		$i="1";
		while ($i<=$cats) {
			$filename = $dbpath."/cats/".$i;
			if (file_exists($filename)){
				include($filename);
				if ($cname!="delplz") {
					echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\"><form method=\"post\" action=\"admin2.php?do=cedit\">";
					echo "<tr><td colspan=\"2\"><font size=\"1\">$VEditCategory: $cname</font></td></tr>";
					echo "<tr><td width=\"10%\" align=\"right\">$VName:</td><td width=\"90%\"><input type=\"text\" name=\"nam\" value=\"$cname\" size=\"75\"></td></tr>";
					echo "<tr><td width=\"10%\" align=\"right\">$VDesc:</td><td width=\"90%\"><input type=\"text\" name=\"dec\" value=\"$cdescription\" size=\"75\">";
					echo "<input type=\"hidden\" name=\"cnum\" value=\"$i\"></td></tr>";
					echo "<input type=\"hidden\" name=\"cforums\" value=\"$cforums\"></td></tr>";
					echo "<tr><td width=\"10%\" align=\"right\">&nbsp;</td><td width=\"90%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr>";
					echo "<tr><td colspan=\"2\">&nbsp;</td></tr></table></form>";
				}
			}
		$i++;
		}
	} elseif ($do=="editcat") {
		FAccessDenied();
	}

	if ($do=="forumadd" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=fa2\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td colspan=\"2\"><font size=\"1\">$VAddForums:</font></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VName:</td><td width=\"69%\"><input type=\"text\" name=\"nam\" size=\"75\"></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VDesc:</td><td width=\"69%\"><input type=\"text\" name=\"dec\" size=\"75\"></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VCategory:</td><td width=\"69%\"><select name=\"cat\">";

		$filename = $dbpath."/cats/cats";
		$fd = fopen ($filename, "r");
		$cats = fread ($fd, filesize ($filename));
		fclose ($fd);
		$i="1";
		while ($i<=$cats) {
			$filename = $dbpath."/cats/".$i;
			if (file_exists($filename)){
				include($filename);
				if ($cname!='delplz'){
					print "<option value=\"$i\">$cname</option>";
				}
			}
			$i++;
		}
		echo "</select></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\" valign=\"top\">$VAllowAccess:</td><td width=\"69%\"><select multiple size=\"8\" name=\"accesscontrol[]\">";
		echo "<option value=\"all\">$VAll</option>";
		echo "<option value=\"mods\">$VModerators</option>";
		$rank=GetRank(9);
		echo "<option value=\"friends\">$rank</option>";
		GetMemberList();
		echo "</select></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\" valign=\"top\">$VLockForum:</td>
			<td width=\"69%\"><input type=\"checkbox\" name=\"lockforum\" value=\"1\">
			&nbsp;$VExplainLockedForum</td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VAllowReplyInLockedForum:</td><td width=\"69%\" valign=\"bottom\">
			<input type=\"checkbox\" name=\"allowreply\" value=\"1\"></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">&nbsp;</td><td width=\"69%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr>";
		echo "<tr><td colspan=\"2\">&nbsp;</td></tr></table></form>";
	} elseif ($do=="forumadd") {
		FAccessDenied();
	}

	if ($admin=="1" || $moder=="1") {
		if ($do=="lockpost") {
			$cat=$HTTP_GET_VARS['cat'];
			$fid=$HTTP_GET_VARS['fid'];
			$pid=$HTTP_GET_VARS['pid'];
			SetLock();
			include($dbpath."/posts/".$cat."_".$fid."_".$pid);
			$plock="locked";
			WritePostInfo($cat,$fid,$pid,"");
			@unlink($dbpath.'/block');
			include($dbpath."/cats/".$cat."_".$fid);
			$tops=$ftopics;
			$end=$tops-$maxppp;
			$start=$tops;
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=board.php?fid=$fid&cat=$cat&s=$start&e=$end\">";
		}

		if ($do=="unlockpost") {
			$cat=$HTTP_GET_VARS['cat'];
			$fid=$HTTP_GET_VARS['fid'];
			$pid=$HTTP_GET_VARS['pid'];
			SetLock();
			include($dbpath."/posts/".$cat."_".$fid."_".$pid);
			$plock="unlocked";
			WritePostInfo($cat,$fid,$pid,"");
			@unlink($dbpath.'/block');
			include($dbpath."/cats/".$cat."_".$fid);
			$tops=$ftopics;
			$end=$tops-$maxppp;
			$start=$tops;
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=board.php?fid=$fid&cat=$cat&s=$start&e=$end\">";
		}
		if ($do=="lockforum") {
			$cat=$HTTP_GET_VARS['cat'];
			$fid=$HTTP_GET_VARS['fid'];
			SetLock();
			include($dbpath."/cats/".$cat."_".$fid);
			$flock="locked";
			WriteForumInfo($cat,$fid);
			@unlink($dbpath.'/block');
			$tops=$ftopics;
			$end=$tops-$maxppp;
			$start=$tops;
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=board.php?fid=$fid&cat=$cat&s=$start&e=$end\">";
		}
		if ($do=="unlockforum" ) {
			$cat=$HTTP_GET_VARS['cat'];
			$fid=$HTTP_GET_VARS['fid'];
			SetLock();
			include($dbpath."/cats/".$cat."_".$fid);
			$flock="unlocked";
			WriteForumInfo($cat,$fid);
			@unlink($dbpath.'/block');
			$tops=$ftopics;
			$end=$tops-$maxppp;
			$start=$tops;
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=board.php?fid=$fid&cat=$cat&s=$start&e=$end\">";
		}
	} else {
		FAccessDenied();
	}

	if ($do=="editforum" && $admin=="1") {
		echo "<td height=\"399\" bgcolor=\"$menucolor\" width=\"81%\" valign=\"bottom\">";
		$filename = $dbpath."/cats/cats";
		$fd = fopen ($filename, "r");
		$totalcats = fread ($fd, filesize ($filename));
		fclose ($fd);
		$i="1";
		while ($i<=$totalcats) {
			if (file_exists($dbpath.'/cats/'.$i)){
				include($dbpath."/cats/".$i);
			}else{
				$cname='';
			}
			if ($cname!=""){
				$forumnum = $cforums;
				$e="1";
				while ($e<=$forumnum) {
					$filename=$dbpath."/cats/".$i."_".$e;
					if (!file_exists($filename)){
					}else{
						include($filename);
						if ($fname!=""){
							echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\"><form method=\"post\" action=\"admin2.php?do=fedit\">";
							echo "<tr><td colspan=\"2\">$VEditForum: $fname ($VCategory: $cname) </td></tr>";
							echo "<tr><td width=\"10%\" align=\"right\">$VName:</td><td width=\"90%\"><input type=\"text\" name=\"fname\" value=\"$fname\" size=\"75\"></td></tr>";
							echo "<tr><td width=\"10%\" align=\"right\">$VDesc:</td><td width=\"90%\"><input type=\"text\" name=\"fdesc\" value=\"$fdescription\" size=\"75\">";
							echo "<tr><td width=\"31%\" align=\"right\" valign=\"bottom\">$VAllowAccess:</td><td width=\"69%\">";
							echo "<select multiple size=\"8\" name=\"accesscontrol[]\">";
							$all="";$mods="";$friends="";
							$acclist=explode("|",$fusers);
							$rank=GetRank(9);
							for ($j=0;$j<=sizeof($acclist);$j++){
								if (trim($acclist[$j])=="all"){
									$all="1";
								}
								if (trim($acclist[$j])=="mods"){
									$mods="1";
								}
								if (trim($acclist[$j])=="friends"){
									$friends="1";
								}
							}
							if ($all=="1"){
								echo "<option value=\"all\" selected>$VAll</option>\n";
							}else{
								echo "<option value=\"all\">$VAll</option>\n";
							}
							if ($mods=="1"){
								echo "<option value=\"mods\" selected>$VModerators</option>\n";
							}else{
								echo "<option value=\"mods\">$VModerators</option>\n";
							}
							if ($friends=="1"){
								echo "<option value=\"friends\" selected>$rank</option>\n";
							}else{
								echo "<option value=\"friends\">$rank</option>\n";
							}
							$handle=opendir($dbpath."/memss/");
							while ($file=readdir($handle)){
								if ($file != "." && $file != ".." && $file != "index.html" && $file != ".htaccess") {
									for ($j=0;$j<=sizeof($acclist);$j++){
										if (trim($acclist[$j])==$file){
											$sel=" selected";
											break;
										}else{
											$sel="";
										}
									}
									echo "<option value=\"$file\"$sel>$file</option>\n";
								}
							}
							closedir($handle);

							echo "</select>";
						}
						echo "<input type=\"hidden\" name=\"cnum\" value=\"$i\"><input type=\"hidden\" name=\"fnum\" value=\"$e\"></td></tr>";
						echo "<tr><td width=\"10%\" align=\"right\">&nbsp;</td><td width=\"90%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr>";
						echo "<tr><td colspan=\"2\">&nbsp;</td></tr></table></form>";
					}    //if file_exists -> forum-info
					$e++;
				}    //end while ($e....
			}    //end if $cname...
			$i++;
		}     //end while ($i...

	} elseif ($do=="editforum") {
		FAccessDenied();
	}

	if ($do=="delforum" && $admin=="1") {
		$cnum=$HTTP_GET_VARS['cnum'];
		$fnum=$HTTP_GET_VARS['fnum'];
		include($dbpath."/cats/".$cnum."_".$fnum);
		$ftopics=$flastpostnumber;
		for ($i=1;$i<=$ftopics;$i++){
			$filename=$dbpath."/posts/".$cnum."_".$fnum."_".$i;
			if (file_exists($filename)){
				echo "<meta http-equiv=\"Refresh\" content=\"0; URL=delpost.php?cat=$cnum&fid=$fnum&pid=$i&auto=\"1\"\">";
			}
		}
		@unlink($dbpath."/cats/".$cnum."_".$fnum);

		AdminSetDone();
	} elseif ($do=="delforum") {
		FAccessDenied();
	}

	if ($do=="delcat" && $admin=="1") {
		$cid=$HTTP_GET_VARS['cnum'];
		$filename=$dbpath."/cats/".$cid;
		if (file_exists($filename)){
			include($filename);
			$foren=$cforums;
		}else{
			ErrorMessage($NoSuchFile,$username);
			exit;
		}
		for ($i=1;$i<=$foren;$i++){
			$filename="$dbpath/cats/$cid"."_$i";
			if (file_exists($filename)){
				$forbidden="1";
			}
		}
		if ($forbidden=="1"){
			ErrorMessage($VNoCatDelete,$username);
			exit;
		}else{
			@unlink($dbpath."/cats/".$cid);
		}

		AdminSetDone();
	} elseif ($do=="delcat") {
		FAccessDenied();
	}

	if ($do=="emmembers" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=emmembers2\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\"><tr><td colspan=\"2\"><font size=\"1\">$VEmailMembers:</font></td>";
		echo "</tr><tr><td width=\"31%\" align=\"right\">$VSubject:</td><td width=\"69%\"><input type=\"text\" name=\"sub\" size=\"75\"></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VContent:</td><td width=\"69%\"><textarea name=\"cont\" cols=\"75\" rows=\"5\"></textarea></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VVia:</td><td width=\"69%\"><input type=\"radio\" name=\"via\" value=\"email\">$VEmail&nbsp;&nbsp;";
		echo "<input type=\"radio\" name=\"via\" value=\"pm\" checked>$VPrivateMessage</tr>";
		echo "<tr><td width=\"31%\" align=\"right\">&nbsp;</td><td width=\"69%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr>";
		echo "<tr><td colspan=\"2\">&nbsp;</td></tr></table></form>";
	} elseif ($do=="emmembers") {
		FAccessDenied();
	}

	if ($do=="ban" && ($admin=="1" || $moder=="1")) {
		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=grantm2\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td colspan=\"2\"><font size=\"1\">$VBanMember:</font></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VUsername:</td><td width=\"69%\"><select name=\"user\">";
		GetMemberList();
		echo "</select></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\" height=\"2\">$VWhichStatus:</td><td width=\"69%\" height=\"2\"><select name=\"stat\">";
		echo "<option value=\"8\">$VBan</option>";
		echo "<option value=\"remstatus\">$VUnban</option></select></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">&nbsp;</td><td width=\"69%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr>";
		echo "<tr><td colspan=\"2\">&nbsp;</td></tr></table></form>";
	} elseif ($do=="ban") {
		FAccessDenied();
	}

	if ($do=="grantm" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=grantm2\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\"><tr><td colspan=\"2\"><font size=\"1\">$VGrantStatus:</font></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VUsername:</td><td width=\"69%\"><select name=\"user\">";
		GetMemberList();
		echo "</select></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\" height=\"2\">$VWhichStatus:</td><td width=\"69%\" height=\"2\"><select name=\"stat\">";
		$handle = opendir('db/status/');
		while ($file = readdir($handle)) {
			if ($file=="6" || $file=="7" || $file=="9") {
				$filename = "$dbpath/status/$file";
				$fd = fopen ($filename, "r");
				$name = fread ($fd, filesize ($filename));
				fclose ($fd);
				echo "<option value=\"$file\">$name</option>";
			}
		}
		echo "<option value=\"remstatus\">$VRemoveStatus</option>";
		closedir($handle);
		echo "</select></td></tr><tr><td width=\"31%\" align=\"right\">&nbsp;</td>
		<td width=\"69%\">
		<input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr><tr><td colspan=\"2\">&nbsp;</td></tr></table></form>";
	} elseif ($do=="grantm") {
		FAccessDenied();
	}

	if ($do=="remove" && $admin=="1") {
		echo "<td  class=\"adminform\">";
		echo "<form method=\"post\" action=\"admin2.php?do=remove2\">";
		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr><td colspan=\"2\"><font size=\"1\">$VRemoveMembers:</font></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">$VUsername:</td><td width=\"69%\"><select name=\"user\">";
		$handle = opendir('db/memss/');
		while ($file = readdir($handle)) {
			if ($file != "." && $file != ".." && $file != "index.html" && $file != ".htaccess") {
				$filename = "$dbpath/memss/$file";
				print "<option value=\"$file\">$file</option>";
			}
		}
		closedir($handle);
		echo "</select></td></tr>";
		echo "<tr><td width=\"31%\" align=\"right\">&nbsp;</td><td width=\"69%\"><input type=\"submit\" name=\"Submit\" value=\"$VRemove\"></td></tr>";
		echo "<tr><td colspan=\"2\">&nbsp;</td></tr></table></form>";
	} elseif ($do=="remove") {
		FAccessDenied();
	}

	echo "</td></tr></table></td></tr></table>";
	echo "</td></tr></table></td></tr></table></td></tr></table>";
	writefooter($newestm);
//	echo "</td></tr></table>";
//	echo "</td></tr></table></td></tr></table>";
}
ob_end_flush();
?>

