<?php
/***************************************************************************
 *                            post.php   -  PBLang
 *                            -------------------
 *
*              see the file docs/copyright.txt for more details
*
*		modified on 10 October 2003
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("scan.php");
include_once("functions.php");
include_once("ffunctions.php");
global $loggedin;
$newthreadimg=GetImage($stylepref,'newthread','butt/');
$replyimg=GetImage($stylepref,'reply','butt/');
$lockedimg=GetImage($stylepref,'reply-locked','butt/');

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"0","0");
if ($permit=="1" || (!$loginreq && $permit=="0")) {
	$language=SetLanguage($liusername);
	include($temppath."/pb/language/lang_".$language.".php");
	$newthreadimg=GetImage($stylepref,'newthread','butt/');
	$replyimg=GetImage($stylepref,'reply','butt/');
	$lockedimg=GetImage($stylepref,'reply-locked','butt/');
	$cat=$HTTP_GET_VARS['cat'];
	$fid=$HTTP_GET_VARS['fid'];
	$pid=$HTTP_GET_VARS['pid'];
	$order=$HTTP_GET_VARS['order'];
	$page=$HTTP_GET_VARS['page'];
	$incrview=$HTTP_GET_VARS['v'];
	$allow=CheckPermissions($cat,$fid,$liusername);
	if (!$allow){
		$language=SetLanguage($liusername);
		include($temppath."/pb/language/lang_".$language.".php");
		writeheader("",0,0,0,$liusername,"1","");
		ErrorMessage($NoAccess,$liusername,$liusername);
		exit;
	}
	setlocale(LC_TIME,$LangLocale);
	$filename = $dbpath."/cats/".$cat."_".$fid;
	if (!file_exists($filename)) {
		WriteHeader("",0,0,0,$liusername,$loggedin,$VError);
		ErrorMessage($NoSuchForum,$liusername);
		exit;
	}
	$lockedforum=GetForumStatus($cat,$fid);
	SetLock();
	include($filename);
	$fpage=ceil($ftopics/$maxppp);
	if (file_exists($dbpath."/posts/".$cat."_".$fid."_".$pid)){
		include ($dbpath."/posts/".$cat."_".$fid."_".$pid);
	}else{
		writeheader("",0,0,0,$liusername,"1","");
		ErrorMessage($NoSuchFile,$liusername);
		exit;
	}
	if ($incrview!="0"){
		$pviews++;
		WritePostInfo($cat,$fid,$pid,"");
	}
	@unlink($dbpath.'/block');
	$locked=GetTopicStatus($cat,$fid,$pid);
	if ($liusername!=""){
		SetLock();
		include ($dbpath."/posts/".$cat."_".$fid."_".$pid);
		if (!strpos($pvisitors,$liusername)){
			$pvisitors.="|".$liusername;
			WritePostInfo($cat,$fid,$pid,"");
		}
		if (!strpos($fvisitors,$liusername)){
			$fvisitors=$fvisitors."|".$liusername;       //add visitor to readerlist of forum
			WriteForumInfo($cat,$fid);
		}
		unlink(  $dbpath.'/block');
	}
	$pname=replacestuff($psubject);
	$pauth=$pauthor;
	$pcont=textparse1($pcontent);
	$pcont=replacestuff($pcont);
	$ppa=$pdate;
	setlocale(LC_TIME,$LangLocale);
	$ppa=strftime($DateFormat2,$ppa);

	if (($locked=="1" || $locked=='locked')&&$admin!='1') {
			$s="showitpostlocked";
	} else {
			$s="showit2";
	}
	writeheader($s,$cat,$fid,$pid,$liusername,$loggedin,$ViewPost." :: ".$pname);

	$filename=$dbpath."/members/".$liusername;
	if (file_exists($filename) && $loggedin=="1"){
		include($filename);
	}
	$admin=$useradmin;
	$moder=$usermod;
	$ban=$userban;
	if ($ban=="1") {
		ErrorMessage($banreason,$liusername);
		exit;
	} else {
		if ($maint=="1" && $admin=="0") {
			ErrorMessage($mreason,$liusername);
			exit;
		}
	}
	clearstatcache();
	$lockedforum=GetForumStatus($cat,$fid);
	include($dbpath."/cats/".$cat);
	include($dbpath."/cats/".$cat."_".$fid);
	$tops=$ftopics;
	$end=$tops-$maxppp;
	if ($maxrpp){
		$noofpages=ceil($preplies/$maxrpp);
	}
	if($noofpages==0){
		$noofpages="1";
	}
	if(strlen($page)==0){
		$page="last";
	}
	if($page=="last" || $page>$noofpages){
		$page=$noofpages;
	}
	$start=$tops;
	echo "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">";
	echo "<tr><td width=\"48%\"><font size=\"1\"></font></td><td width=\"52%\" align=\"right\">";
	if ($picturebuttons=="Yes"){
		if ($locked=="0" && $lockedforum=="0" || $admin=="1") {
			echo "<a href=\"ntopic.php?cat=$cat&fid=$fid\"><img src=\"$newthreadimg\" border=\"0\" alt=\"$newthreadalt\" title=\"$newthreadtitle\"></a> ";
		}
		if (($locked=="0" && ($lockedforum=="0" || $lockedforum=='2')) || $admin=="1") {
			echo " <a href=\"nreply.php?cat=$cat&fid=$fid&pid=$pid&page=$noofpages\"><img src=\"$replyimg\" border=\"0\" alt=\"$replyalt\" title=\"$replytitle\"></a>";
		}
		if ($locked=="1" && $lockedforum=="0" && $admin=="0") {
			echo "<a href=\"ntopic.php?cat=$cat&fid=$fid\"><img src=\"$newthreadimg\" border=\"0\" alt=\"$newthreadalt\" title=\"$newthreadtitle\"></A> ";
			echo "<img src=\"$lockedimg\" border=\"0\" alt=\"$lockedalt\" title=\"$lockedtitle\">";
		}
		if ($lockedforum=="1" && $admin=="0") {
			echo "<img src=\"$lockedimg\" border=\"0\" alt=\"$lockedalt\" title=\"$lockedtitle\">";
		}
	}else{
		if ($locked=="0" && $lockedforum=="0" || $admin=="1") {
			echo "<a href=\"ntopic.php?cat=$cat&fid=$fid\">$VNewTopic</a> | ";
			echo " <a href=\"nreply.php?cat=$cat&fid=$fid&pid=$pid&page=$noofpages\">$VReply</a>";
		}
		if ($locked=="0" && $lockedforum=='2') {
			echo " <a href=\"nreply.php?cat=$cat&fid=$fid&pid=$pid&page=$noofpages\">$VReply</a>";
		}
		if ($locked=="1" && $lockedforum=="0" && $admin=="0") {
			echo "<a href=\"ntopic.php?cat=$cat&fid=$fid\">$VNewTopic</A> | $VLocked";
		}
		if ($lockedforum=="1" && $admin=="0") {
			echo "$VLocked";
		}
	}
	echo "</td></tr></table>";
	echo "<table width=\"100%\" border=\"0\" cellpadding=\"$containerbordersize\" cellspacing=\"0\">";
	echo "<tr bgcolor=\"$containerborder\"><td height=\"2\">";
	echo "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr bgcolor=\"$containerinner\"><td height=\"2\">";
	echo "<table width=\"100%\" border=\"0\" cellpadding=\"3\" cellspacing=\"1\">";
	$forumname=replacestuff(stripslashes($fname));
	$catname=replacestuff(stripslashes($cname));
	$stitle=replacestuff(stripslashes($sitetitle));
	$pname=stripslashes($pname);
	echo "<tr><td class=\"header\" colspan=\"3\"><a href=\"index.php\">";
	echo "$stitle</a> :: $catname :: <a href=\"board.php?fid=$fid&cat=$cat&s=$start&e=$end\">";
	echo $forumname,'</a> :: ',$pname,'&nbsp; :: ';
	if ($noofpages>1){
		if($page=='last'){
			$page=$noofpages;
		}
		$currpage=$page;
		threadpages($currpage,$noofpages,$cat,$fid,$pid);
	}
	echo '</td></tr>';

	$filename=$dbpath.'/memss/'.$pauth;
	if (!file_exists($filename)){
		$userrem='1';
	}else{
		$userrem='0';
		include($dbpath.'/members/'.$pauth);
		$rnk=$userrank;
		$rank2=GetRank($rnk);
		if (($rnk=='6' && !file_exists($temppath.'/pb/images/ranks/rank6.gif')) || ($rnk=='7' && !file_exists($temppath.'/pb/images/ranks/rank7.gif')) || ($rnk=='9' && !file_exists($temppath.'/pb/images/ranks/rank9.gif'))) {
			$rnk='12';
		}
		$done='0';
		$rank1="rank$rnk";
		$rank=GetImage($stylepref,$rank1,'ranks/');
		$pt=replacestuff($userslogan);
		$posts=$userposts;
		$sig=replacestuff($usersig);
		$ip=$userip;
	}
	$ctuu=1;

	$u="editpost.php?cat=$cat&fid=$fid&pid=$pid&t=1&page=$noofpages";
	$qurl="nreply.php?cat=$cat&fid=$fid&pid=$pid&page=$noofpages&q=0";

	if ($HTTP_COOKIE_VARS[$cookieid]==trim($pauth) && !$preplies) {
		$don="1";
	} else {
		$don="0";
	}
	$u2u="delpost.php?cat=$cat&fid=$fid&pid=$pid";
	if ($censor=="1") {
		$sig=wcensor($sig,$username);
		$pcont=wcensor($pcont,$username);
	}
	$sig=replacestuff($sig);
//	$sig=stripslashes($sig);
	$pname=stripslashes($pname);

	if ($page=="1"){
		//Display the first post of a thread
		postp($pauth,$pcont,$pname,$rank,$rank2,$pt,$posts,$sig,$ctuu,$ip,$admin,$u,$don,$u2u,$ppa,$userrem,$liusername,$qurl,$locked);
	}

	include($dbpath."/posts/".$cat."_".$fid."_".$pid);
	$rnum = $plastreply;
	if ($rnum>"1"){
		echo "<tr><td bgcolor=\"$menucolor\" height=\"12\" colspan=\"3\"><font style=\"color:$menufont\" color=\"$menufont\">$VOrderOfReplies:&nbsp;</font>";
		echo "<a href=\"post.php?cat=$cat&fid=$fid&pid=$pid&order=fl&page=1\">$VFirstReplyLast</a> :: ";
		echo "<a href=\"post.php?cat=$cat&fid=$fid&pid=$pid&order=ff&page=1\">$VFirstReplyFirst</a></td></tr>";
	}
	if ($order=='fl'){
		$end=$rnum-($page*$maxrpp);
		$start=$rnum-($page-1)*$maxrpp;
		if ($start < '1'){
			$start='1';
		}
		if ($end < '1'){
			$end='1';
		}
		$i=$start;
		while ($i>=$end) {
			$rsubject='';
			$rauthor='';
			@include($dbpath.'/posts/'.$cat.'_'.$fid.'_'.$pid.'_r_'.$i);
			$rsub=$rsubject;
			$rauth=$rauthor;
			$rpa=$rdate;
			$rpa=strftime($DateFormat2,$rpa);

			if ($rauth=='')
			{
			}else{
				$filename=$dbpath.'/memss/'.$rauth;
				if (!file_exists($filename)){
					$userrem='1';
				}else{
					$userrem='0';
					include($dbpath.'/members/'.$rauth);
					$pt=replacestuff($userslogan);
					$post=$userposts;
					$sig=replacestuff($usersig);
					$ip=$userip;
					$rnk=$userrank;
					$rank2=GetRank($rnk);
					if (($rnk=='6' && !file_exists($temppath.'/pb/images/ranks/rank6.gif')) || ($rnk=='7' && !file_exists($temppath.'/pb/images/ranks/rank7.gif')) || ($rnk=='9' && !file_exists($temppath.'/pb/images/ranks/rank9.gif'))) {
						$rnk='12';
					}
					$rank1="rank$rnk";
					$rank=GetImage($stylepref,$rank1,'ranks/');
				}

				$rcont=textparse1($rcontent);
				$rcont=replacestuff($rcont);
				if ($ctuu=='1') {
					$ctuu=2;
					$done='1';
				}
				if ($ctuu=='2' && $done=='0') {
					$ctuu=1;
				}
				$done='0';
				$u="editpost.php?cat=$cat&fid=$fid&pid=$pid&t=2&r=$i&page=$noofpages";
				$qurl="nreply.php?cat=$cat&fid=$fid&pid=$pid&page=$noofpages&q=$i";
				if ($HTTP_COOKIE_VARS[$cookieid]==trim($rauth)) {
					$del='1';
				} else {
					$del='0';
				}
				$u2u="delpost.php?cat=$cat&fid=$fid&pid=$pid&t=2&r=$i";          //delete URL
				if ($censor=='1')
				{
					$sig=wcensor($sig,$username);
					$rcont=wcensor($rcont,$username);
				}
				$sig=replacestuff($sig);
				$sig=stripslashes($sig);

				postp($rauth,$rcont,$rsub,$rank,$rank2,$pt,$post,$sig,$ctuu,$ip,$admin,$u,$del,$u2u,$rpa,$userrem,$liusername,$qurl,$locked);
			}
			$i--;
		}
		if ($i<1){$i=1;}
	}else{
		$end=$page*$maxrpp;
		$start=(($page-1)*$maxrpp) + 1;
		if ($start > $rnum){
			$start=$rnum;
		}
		if ($end > $rnum){
			$end=$rnum;
		}
		$i=$start;
		while ($i<=$end) {
			$rsubject='';
			$rauthor='';
			@include($dbpath.'/posts/'.$cat.'_'.$fid.'_'.$pid.'_r_'.$i);
			$rsub=$rsubject;
			$rauth=$rauthor;
			$rpa=$rdate;
			$rpa=strftime($DateFormat2,$rpa);

			if ($rauth==''){
			}else{
				$filename=$dbpath.'/memss/'.$rauth;
				if (!file_exists($filename)){
					$userrem='1';
				}else{
					$userrem='0';
					include($dbpath.'/members/'.$rauth);
					$pt=replacestuff($userslogan);
					$post=$userposts;
					$sig=replacestuff($usersig);
					$ip=$userip;
					$rnk=$userrank;
					$rank2=GetRank($rnk);
					if (($rnk=='6' && !file_exists($temppath.'/pb/images/ranks/rank6.gif')) || ($rnk=='7' && !file_exists($temppath.'/pb/images/ranks/rank7.gif')) || ($rnk=='9' && !file_exists($temppath.'/pb/images/ranks/rank9.gif'))) {
							$rnk='12';
					}
					$rank1='rank'.$rnk;
					$rank=GetImage($stylepref,$rank1,'ranks/');
				}

				$rcont=textparse1($rcontent);
				$rcont=replacestuff($rcont);
				if ($ctuu=='1') {
						$ctuu=2;
						$done='1';
				}
				if ($ctuu=='2' && $done=='0') {
						$ctuu=1;
				}
				$done='0';
				$u="editpost.php?cat=$cat&fid=$fid&pid=$pid&t=2&r=$i";
				$qurl="nreply.php?fid=$fid&cat=$cat&pid=$pid&page=$noofpages&q=$i";
				if ($HTTP_COOKIE_VARS[$cookieid]==trim($rauth)) {
						$del='1';
				} else {
					$del='0';
				}
				$u2u="delpost.php?cat=$cat&fid=$fid&pid=$pid&t=2&r=$i";          //delete URL
				if ($censor)
				{
					$sig=wcensor($sig,$username);
					$rcont=wcensor($rcont,$username);
				}
				$sig=replacestuff($sig);
				$sig=stripslashes($sig);

				postp($rauth,$rcont,$rsub,$rank,$rank2,$pt,$post,$sig,$ctuu,$ip,$admin,$u,$del,$u2u,$rpa,$userrem,$liusername,$qurl,$locked);
			}
			$i++;
		}
		if ($i>$rnum){$i=$rnum;}
	}

echo '</table></td></tr></table></td></tr></table>';
	if ($picturebuttons=='Yes'){
		if ($lockedforum=='0' || $admin=='1' || $lockedforum=='2') {
			if ($s=='showit2') {
				echo '<table width="100%" border="0" cellpadding="2" cellspacing="0">';
				echo "<tr><td width=\"48%\">";
				if ($noofpages>1){
					threadpages($currpage,$noofpages,$cat,$fid,$pid);
				}
				echo "</td>";
				echo "<td width=\"52%\" align=\"right\">";
				if ($lockedforum=='0' || $admin=='1'){
					echo "<a href=\"ntopic.php?cat=$cat&fid=$fid\"><img src=\"$newthreadimg\" border=\"0\" alt=\"$newthreadalt\" title=\"$newthreadtitle\"></a>";
				}
				if ($lockedforum=='0' || $lockedforum=='2' || $admin=='1'){
					echo " <a href=\"nreply.php?cat=$cat&fid=$fid&pid=$pid&page=$noofpages\">
						<img src=\"$replyimg\" border=\"0\" alt=\"$replyalt\" title=\"$replytitle\"></a>";
				}
				echo '</td></tr></table>';
			} elseif ($s=='showitpostlocked') {
				echo "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">";
				echo "<tr><td width=\"48%\">$links</td>";
				echo "<td width=\"52%\" align=\"right\"><a href=\"ntopic.php?cat=$cat&fid=$fid\">";
				echo "<img src=\"$newthreadimg\" border=\"0\" alt=\"$newthreadalt\" title=\"$newthreadtitle\"></A>";
				echo " <img src=\"$lockedimg\" border=\"0\" alt=\"$lockedalt\" title=\"$lockedtitle\"></td></tr></table>";
			}
		}
	}else{
		if ($lockedforum=="0" || $admin=="1" || $lockedforum=='2') {
			if ($s=="showit2") {
				echo "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">";
				echo "<tr><td width=\"48%\">$links</td>";
				if ($lockedforum=='0' || $admin=='1'){
					echo "<td width=\"52%\" align=\"right\"><a href=\"ntopic.php?cat=$cat&fid=$fid\">$VNewTopic</a> | ";
				}
				if ($lockedforum=='0' || $lockedforum=='2' || $admin=='1'){
					echo "<a href=\"nreply.php?cat=$cat&fid=$fid&pid=$pid&page=$noofpages\">$VReply</a></td></tr></table>";
				}
			} elseif ($s=="showitpostlocked") {
				echo "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">";
				echo "<tr><td width=\"48%\">$links</td>";
				echo "<td width=\"52%\" align=\"right\"><a href=\"ntopic.php?cat=$cat&fid=$fid\">$VNewTopic</A> - $VLocked</td></tr></table>";
			}
		}
	}
	if ($admin=="1" || $moder=="1") {
		echo "<br><div align=\"right\">$VAdministrativeFunctions: ";
		$deletetopicimg=GetImage($stylepref,'deletetopic','');
		if ($picturebuttons=='Yes' && $deletetopicimg){
			echo "<a href=\"delpost.php?cat=$cat&fid=$fid&pid=$pid\"><img src=\"$deletetopicimg\" border=\"0\" alt=\"$DeleteTopic\" title=\"$DeleteTopic\"></a> ";
		}else{
			echo "<a href=\"delpost.php?cat=$cat&fid=$fid&pid=$pid\">$DeleteTopic</a> | ";
		}
		if ($locked=="0") {
			$locktopicimg=GetImage($stylepref,'locktopic','');
			if ($picturebuttons=='Yes' && $locktopicimg){
				echo "<a href=\"admin.php?do=lockpost&cat=$cat&fid=$fid&pid=$pid\">
				<img src=\"$locktopicimg\" border=\"0\" alt=\"$LockTopic\" title=\"$LockTopic\"></a>";
			}else{
				echo "<a href=\"admin.php?do=lockpost&cat=$cat&fid=$fid&pid=$pid\">$LockTopic</a>";
			}
		} elseif ($locked=="1") {
			$unlocktopicimg=GetImage($stylepref,'unlocktopic','');
			if ($picturebuttons=='Yes' && $unlocktopicimg){
				echo "<a href=\"admin.php?do=unlockpost&cat=$cat&fid=$fid&pid=$pid\">
				<img src=\"$unlocktopicimg\" border=\"0\" alt=\"$UnlockTopic\" title=\"$UnlockTopic\"></a>";
			}else{
				echo "<a href=\"admin.php?do=unlockpost&cat=$cat&fid=$fid&pid=$pid\">$UnlockTopic</a>";
			}
		}
		echo "</div>";

		$s=$newestm;
	}
	writefooter($newestm);
}
ob_end_flush();
?>

