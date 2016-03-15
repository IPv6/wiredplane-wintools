<?php
/***************************************************************************
 *                            profile.php  - PBLang
 *                            -------------------
 *
 *             read the file copyright.txt in the docs-folder
 *		updated 21 May 2003
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("scan.php");
include_once("functions.php");
include_once("ffunctions.php");
$sendpmimg=GetImage($stylepref,'sendpm','butt/');

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1") {
	$language=SetLanguage($liusername);
	$sendpmimg=GetImage($stylepref,'sendpm','butt/');
	include($temppath."/pb/language/lang_".$language.".php");
	$user=$HTTP_GET_VARS['u'];
	if (!$user)
	{
		ErrorMessage($AccessDenied,$user);
		exit;
	}
	writeheader($newestm,0,0,0,$liusername,$loggedin,$VProfile);
	include($dbpath."/members/".$user);

	$realname=$userrealname;
	$alias=$useralias;
	$posts=$userposts;
	$pt=replacestuff($userslogan);
	$msn=$usermsn;
	$yah=$useryahoo;
	$aim=$useraim;
	$icq=$usericq;
	$qq=$userqq;
	$loc=$userlocation;
	$sig=replacestuff($usersig);
	$av=$useravatar;
	$type=$userrank;
	$em=$useremail;
	$emhide=$useremhide;
	$web=$userhomepage;
	$webav=$userwebavatar;
	$statn=GetRank($type);

	$filet="$dbpath/online/$user";
	if (file_exists($filet)) {
		$on="$VLoggedOn";
	} else {
		$on="$VLoggedOff";
	}
	if ($type=="6") {
		$type="12";
	}
	if ($type=="7") {
		$type="12";
	}
	if ($type=="8") {
		$type="8";
	}
	$stati="rank$type.gif";

	WriteTableTop();
	echo "<form method=\"post\" action=\"\">";
	echo "<tr><td class=\"header\" colspan=\"3\">$sitetitle :: $VProfileFor $alias";
	if ($admin=="1" || $liusername==$user) {
		print " :: <a href=\"ucp.php?te=$user\">$VModify</a>\n";
	}
	echo "</td></tr>";
	echo "<tr ><td class=\"subheader\" width=\"20%\">$VAvatar :: ";
	echo "</td>\n<td class=\"subheader\" width = \"80%\" colspan=2>$VSettings :: </td></tr>";
	echo "<tr><td class=\"label\" style=\"vertical-align:bottom;text-align:center;\" rowspan=11>";
	echo "<p>&nbsp;</p>";
	if ($ave=="1") {
		if ($av=="webav") {
				$avsize=@getimagesize($webav);
				$avheight=$avsize[1];
				$avwidth=$avsize[0];
				if (($avmaxheight || $avmaxwidth) && ($avheight>$avmaxheight || $avwidth>$avmaxwidth)){
					$heightratio=$avheight/$avmaxheight;
					$widthratio=$avwidth/$avmaxwidth;
					if ($heightratio>$widthratio){
						$avheight=round($avheight/$heightratio);
						$avwidth=round($avwidth/$heightratio);
					}else{
						$avheight=round($avheight/$widthratio);
						$avwidth=round($avwidth/$widthratio);
					}
					echo "<img src=\"$webav\" alt=\"$VImage\" width=\"$avwidth\" height=\"$avheight\">";
				}else{
					echo "<img src=\"$webav\" alt=\"$VImage\">";
				}
		} elseif (trim($av)!="" && $av!="none") {
				echo "<img src=\"templates/pb/images/avatars/$av\" alt=\"$VImage\">";
		}else{
			echo $VNoAvatar;
		}
	} else {
		print $NoAvatars;
	}
	echo "<p>$pt</p></td>\n"; //<td height=\"42\" bgcolor=\"$fmenucolor\" width=\"70%\" valign=\"top\">";
	echo "<td class=\"label\">$VRealname:</td>
		<td class=\"content\">$realname</td></tr>\n";
	echo "<tr><td class=\"label\">$VAlias:</td>
		<td class=\"content\">$alias</td></tr>\n";
	echo "<tr><td class=\"label\">$VLocation:</td>
		<td class=\"content\">$loc</td></tr>\n";
	echo "<tr><td class=\"label\">MSN:</td>
		<td class=\"content\">$msn</td></tr>\n";
	echo "<tr><td class=\"label\">AIM:</td>
		<td class=\"content\"><a href=\"aim:goim?screenname=$aim&message=Hello+Are+you+there?\">$aim</a></td></tr>\n";
	echo "<tr><td class=\"label\">ICQ #:</td>
		<td class=\"content\"><a href=\"http://wwp.icq.com/scripts/search.dll?to=$icq\">$icq</a></td></tr>\n";
	echo "<tr><td class=\"label\">QQ:</td>
		<td class=\"content\">$qq</td></tr>\n";
	echo "<tr><td class=\"label\">$VWebsite: </td><td class=\"content\">";
	if ($web!="http://" && trim($web)!="" && $web!="http:///") {
		echo "<a href=\"$web\" target=\"_blank\">$web</a>";
	}
	echo "</td></tr><tr><td class=\"label\">Yahoo:</td><td class=\"content\">";
	if (trim($yah)!="") {
		echo "<a href=\"http://edit.yahoo.com/config/send_webmesg?.target=$yah&.src=pg\">$yah</a>";
	}
	echo "</td></tr>";
	if (($emhide=="hide" && $admin!="1") || $loggedin=="0"){
		echo "<tr><td class=\"label\">$VEmail:</td>
			<td class=\"content\">$VHidden</td></tr>\n";
	}else{
		echo "<tr><td class=\"label\">$VEmail:</td>
			<td class=\"content\"><a href=\"mailto:$em\">$em</a></td></tr>\n";
	}
	echo "<tr><td class=\"label\">$VStatus:</td>
		<td class=\"content\">$on</td></tr>\n";
	echo "</td></tr><tr>";
	echo "<td class=\"subheader\" width=\"20%\">$VSignature :: </td>";
	echo "<td class=\"subheader\" width=\"80%\" colspan=2>$VPosts $VAnd $VStatus :: </td></tr>\n";
	echo "<tr><td class=\"label\" style=\"vertical-align:middle;text-align:center;\">$sig</td>";
	echo "<td class=\"content\" style=\"vertical-align:middle;\" colspan=2>$statn<br>\n";
	echo "<img src=\"templates/pb/images/ranks/$stati\"><br>";
	echo "$VPosts: $posts";
	if ($picturebuttons=="Yes"){
		if ($loggedin=="1"){
			echo "<br><BR><a href=\"sendpm.php?to=$user\"><img src=\"$sendpmimg\" border=\"0\" alt=\"$sendpmalt\" title=\"$sendpmtitle\"></a>";
		}
	}else{
		if ($loggedin=="1"){
			echo "<br><br><a href=\"sendpm.php?to=$user\">$Vsendpm</a>";
		}
	}
	echo "<br>";
	sbot(0);

	writefooter($newestm);
}
ob_end_flush();
?>
