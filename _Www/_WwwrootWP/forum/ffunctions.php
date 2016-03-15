<?php
/* ffunctions.php - PBLang 

See the copyright-notice in docs/copyright.txt

This file contains all functions which relate to file in- and output

Updated 8 Oct 2003
*/

//Read functions **********************************************************
if ( !defined('IN_PB') )
{
	die("Hacking attempt");
}

error_reporting  (E_ERROR | E_WARNING | E_PARSE); // This will NOT report uninitialized variables
set_magic_quotes_runtime(0); // Disable magic_quotes_runtime

define('IN_PBG', "true");

function fixField($field)
{
	$res=str_replace("?","",$field);
	$res=str_replace("\"","'",$res);
	return $res;
}

function GetForumStatus($cat,$fid)
{
	include("global.php");
	include($dbpath."/cats/".$cat."_".$fid);
		if ($flock=="locked" && $fallowreply) {
			$flocked="2";
		} elseif ($flock=='locked' && !$fallowreply) {
			$flocked="1";
		}else{
			$flocked='0';
		}
	return($flocked);
}

function GetFReadStatus($cat,$fid,$user)
{
	include("global.php");
	include($dbpath."/cats/".$cat."_".$fid);
	if (strstr($fvisitor,$user)) {
		$found="1";
	}else{
		$found="0";
	}
	return($found);
}

function UpdateGuestCount($add)
{
//updated 16 December 2002
	include("global.php");
	$filename=$dbpath."/guestcount";
	if (file_exists($filename)) {
		include($filename);
	}
	if ($today==""){
		$fd=@fopen($filename,"r");
		$totalguests=@fread($fd,filesize($filename));
		@fclose($fd);
	}
	$tomorrow=$today+86400;
	if (time()>=$tomorrow){
		$todaysguests=0;
		$tod=getdate(time());
		$today=mktime(0,0,0,$tod['mon'],$tod['mday'],$tod['year']);
	}
	if ($add=="1"){
		$totalguests++;
		$todaysguests++;
	}else{
		if ($totalguests>"0"){
			$totalguests--;
		}
		if ($todaysguests>"0"){
			$todaysguests--;
		}
	}
	$content="<";
	$content.="?php\n\n";
	$content .="\$todaysguests=\"$todaysguests\";\n";
	$content .="\$totalguests=\"$totalguests\";\n";
	$content .="\$today=\"$today\";\n";
	$content .="?";
	$content .=">";

	$fp=@fopen($filename,"w");
	@fputs($fp,$content);
	@fclose($fp);
}

function GetRank($rank)
{
	global $dbpath;
	$filename = "$dbpath/status/$rank";
	$fd = fopen ($filename, "r");
	$rnk = fread ($fd, filesize ($filename));
	fclose ($fd);
	return($rnk);
}

function GetTopicStatus($cat,$fid,$pid)
{
	global $dbpath;
	$filename=$dbpath."/posts/".$cat."_".$fid."_".$pid;
	if (file_exists($filename)) {
		include ($filename);
		if ($plock=="unlocked"||$plock=="") {
			$tlocked="0";
		} elseif ($plock=="locked"||$plock=='1') {
			$tlocked="1";
		}
	} else {
		$tlocked="0";
	}
	return($tlocked);
}

//Write functions ************************************************************************

function WriteCatInfo($cat)
{
//updated 26 Dec 2002

	global $cname,$cdescription,$cforums;
	$cname=textparse(stripslashes($cname));
	$cdescription=textparse(stripslashes($cdescription));
	include("global.php");
	$content ="<"."?php\n\n";
	$content .="\$cname=\"$cname\";\n";
	$content .="\$cdescription=\"$cdescription\";\n";
	$content .="\$cforums=\"$cforums\";\n\n";
	$content .="?".">";
	$fp=fopen($dbpath."/cats/".$cat,"w");
	flock($fp,LOCK_EX);
	fputs($fp,$content);
	flock($fp,LOCK_UN);
	fclose($fp);
}

function WriteForumInfo($cid,$fid)
{
//updated 31 May 2003

	global $fname,$fdescription,$flasturl,$flastauthor,$flastpost,$freplies,$ftopics,$fusers,$fvisitors,$flock,$flastpostnumber,$fsticky,$fallowreply,$fhide;
	$fname=textparse(stripslashes($fname));
	$fdescription=textparse(stripslashes($fdescription));
	include("global.php");
	$content ="<"."?php\n\n";
	$content .="\$fname=\"$fname\";\n";
	$content .="\$fdescription=\"$fdescription\";\n";
	$content .="\$flasturl=\"$flasturl\";\n";
	$content .="\$flastauthor=\"$flastauthor\";\n";
	$content .="\$flastpost=\"$flastpost\";\n";
	$content .="\$freplies=\"$freplies\";\n";
	$content .="\$ftopics=\"$ftopics\";\n";
	$content .="\$fusers=\"$fusers\";\n";
	$content .="\$fvisitors=\"$fvisitors\";\n";
	$content .="\$flock=\"$flock\";\n";
	$content .="\$fallowreply=\"$fallowreply\";\n";
	$content .="\$fsticky=\"$fsticky\";\n";
	$content .="\$flastpostnumber=\"$flastpostnumber\";\n";
	$content .="\$fhide=\"$fhide\";\n";
	$content .="\n?".">";
	if (!$fname || (!$cid && !$fid)){
	}else{
		$filename=$dbpath.'/cats/'.$cid.'_'.$fid;
		$timestamp=@filemtime($filename);	//this is needed in case PHP accelerator is used by the host
		if ( !@touch($filename,$timestamp)){
			copy($filename, $filename.'.tmp');
			unlink($filename);
			copy($filename.'.tmp', $filename);
			unlink($filename.'.tmp');
			@chmod($filename,0666);
		}
		$fp=fopen($filename,"w");
		flock($fp,LOCK_EX);
		fputs($fp,$content);
		flock($fp,LOCK_UN);
		fclose($fp);
		@chmod($filename,0666);
		$newtimestamp=filemtime($filename);	//this is needed in case PHP accelerator is used by the host
		if ($timestamp>=$newtimestamp){
			touch($filename,$timestamp+1);
		}
	}
}

function WriteLatestPosts($cat,$fid,$pid,$rid)
{
//updated 29 Sep 2003
global $dbpath,$maxlatest,$fp1;

	//since in arrays 0 is counted, we decrease maxlatest by 1 to match the desired number of latest posts
	if ($rid){
		$latestpost=$cat.'|'.$fid.'|'.$pid.'|'.$rid;
	}else{
		$latestpost=$cat.'|'.$fid.'|'.$pid;
	}
	$filename=$dbpath."/settings/latestposts";
	$arr_latestposts=@file($filename);
	$latestpostnr=@count($arr_latestposts);
	$latestpostsstring='-'.implode("-",$arr_latestposts);
	$possiblemaxlatest=$maxlatest*4;  //we need to save more in case there are hidden or deleted messages
	if ($latestpostnr < $possiblemaxlatest){
		$possiblemaxlatest=$latestpostnr + 1;
	}
	$j=$possiblemaxlatest;
	if (strpos($latestpostsstring,$latestpost)){	//in case the message was already in the last, it just needs to be moved up.
		$i=$possiblemaxlatest+1;
		while($j>0){
			$i--;
			if ($latestpost!=trim($arr_latestposts[$i])){
				$arr_latestposts[$j]=trim($arr_latestposts[$i]);
				$j--;
			}
		}
	}else{
		$i=$possiblemaxlatest;
		while($j>0){
			$i--;
			$arr_latestposts[$j]=trim($arr_latestposts[$i]);
			$j--;
		}
	}
	$j=$possiblemaxlatest;
	$arr_latestposts[0]=$latestpost;

	for ($i=0;$i<$possiblemaxlatest;$i++){
		if (trim($arr_latestposts[$i]!='')){
			$content.=trim($arr_latestposts[$i])."\n";
		}
	}

	$fp=fopen($filename,"w");
	flock($fp,LOCK_EX);
	fputs($fp,$content);
	flock($fp,LOCK_UN);
	fclose($fp);
	@chmod($filename,0666);

}

function WritePostInfo($cid,$fid,$pid,$sticky)
{
//updated 30 Dec 2002

	global $psubject,$pcontent,$pauthor,$pdate,$plastdate,$pimage,$plastreply,$plastauthor,$preplies,$pviews,$pnotify,$plock,$pip,$pvisitors,$psticky,
		$liusername,$fp1;

	$psubcject=textparse($psubject);
	$pcontent=textparse($pcontent);
	include("global.php");
	
	$content ="<"."?php\n\n";
	$content .="\$psubject=\"$psubject\";\n";
	$content .="\$pcontent=\"$pcontent\";\n";
	$content .="\$pauthor=\"$pauthor\";\n";
	$content .="\$pdate=\"$pdate\";\n";
	$content .="\$plastdate=\"$plastdate\";\n";
	$content .="\$pimage=\"$pimage\";\n";
	$content .="\$plastreply=\"$plastreply\";\n";
	$content .="\$plastauthor=\"$plastauthor\";\n";
	$content .="\$preplies=\"$preplies\";\n";
	$content .="\$pviews=\"$pviews\";\n";
	$content .="\$pnotify=\"$pnotify\";\n";
	$content .="\$plock=\"$plock\";\n";
	$content .="\$pip=\"$pip\";\n";
	$content .="\$pvisitors=\"$pvisitors\";\n";
	$content .="\$psticky=\"$psticky\";\n";
	$content .="\n?".">";
	if ((!$psubject && !$pcontent) || (!$cid && !$fid)){
		ErrorMessage($VNoValidMessage,$pauthor);
		@unlink($dbpath.'/block');
		exit;
	//	 	WriteLog('log.txt','empty message or wrong IDs - cat: '.$cid.' fid: '.$fid.' pid: '.$pid.' user: '.$liusername. ' date: '.strftime("%D, %X"));
	}else{
		$filename=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid;
		$timestamp=@filemtime($filename);
		if ( !@touch($filename,$timestamp)){
			copy($filename, $filename.'.tmp');
			unlink($filename);
			copy($filename.'.tmp', $filename);
			unlink($filename.'.tmp');
			@chmod($filename,0666);
		}
		$fp=fopen($filename,"w");
		flock($fp,LOCK_EX);
		fputs($fp,$content);
		flock($fp,LOCK_UN);
		fclose($fp);
		@chmod($filename,0666);
		$newtimestamp=filemtime($filename);
		if ($timestamp>=$newtimestamp){
			@touch($filename,$timestamp+1);
		}
	}
}

function WriteReplyInfo($cid,$fid,$pid,$rid)
{
//updated 1 Jan 2003

	global $rsubject,$rcontent,$rauthor,$rdate,$rip,$dbpath;
	//     include("global.php");
	include_once("ffunctions.php");
	$rsubject=textparse($rsubject);
	$rcontent=textparse($rcontent);
	$content ="<"."?php\n\n";
	$content .="\$rsubject=\"$rsubject\";\n";
	$content .="\$rcontent=\"$rcontent\";\n";
	$content .="\$rauthor=\"$rauthor\";\n";
	$content .="\$rdate=\"$rdate\";\n";
	$content .="\$rip=\"$rip\";\n";
	$content .="\n?".">";
	$filename=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid.'_r_'.$rid;
	$timestamp=@filemtime($filename);
	if ( !@touch($filename,$timestamp)){
		copy($filename, $filename.'.tmp');
		unlink($filename);
		copy($filename.'.tmp', $filename);
		unlink($filename.'.tmp');
		@chmod($filename,0666);
	}
	$fp=fopen($filename,"w");
	flock($fp,LOCK_EX);
	fputs($fp,$content);
	flock($fp,LOCK_UN);
	fclose($fp);
	@chmod($filename,0666);
	$newtimestamp=filemtime($filename);
	if ($timestamp>=$newtimestamp){
		touch($filename,$timestamp+1);
	}
}

function WriteSettings(){
	@include ("global.php");
	global $emsupp,$sitetitle,$sitelogo,$changealias,$allowalias,$defaultslogan,$sitemotto,$footer,$ave,$maint,$mreason,$banreason,$adminemail;
	global $noatch,$censor,$html,$smilys,$bbcode,$timezone,$loginreq,$allowreg,$mailenabled,$cver,$maxatchsize,$maxposts,$maxppp,$confirmemail;
	global $welcomemessage,$checkip,$sitelogowidth,$sitelogoheight,$notifyadmin,$notifyadminrepl,$minspace,$avmaxwidth,$avmaxheight,$maxlatest,$charsets;
	global $maxrpp,$boardwidth,$boardbgimage,$webave,$boardalign,$csubver,$cstat,$cstatlevel,$textareawidth,$attachtypelist,$smtpmail;

	$footer=CheckSlashes($footer);
	$sitetitle=CheckSlashes($sitetitle);
	$defaultslogan=CheckSlashes($defaultslogan);
	$sitemotto=CheckSlashes($sitemotto);
	$mreason=CheckSlashes($mreason);
	$banreason=CheckSlashes($banreason);
	$welcomemessage=CheckSlashes($welcomemessage);

	$content2 = "<";
	$content2 .="?php\n\n";
	$content2 .="\$emsupp=\"$emsupp\";\n";
	$content2 .="\$sitetitle=\"$sitetitle\";\n";
	$content2 .="\$sitelogo=\"$sitelogo\";\n";
	$content2 .="\$sitelogowidth=\"$sitelogowidth\";\n";
	$content2 .="\$sitelogoheight=\"$sitelogoheight\";\n";
	$content2 .="\$boardbgimage=\"$boardbgimage\";\n";
	$content2 .="\$changealias=\"$changealias\";\n";
	$content2 .="\$allowalias=\"$allowalias\";\n";
	$content2 .="\$defaultslogan=\"$defaultslogan\";\n";
	$content2 .="\$sitemotto=\"$sitemotto\";\n";
	$content2 .="\$footer=\"$footer\";\n";
	$content2 .="\$ave=\"$ave\";\n";
	$content2 .="\$webave=\"$webave\";\n";
	$content2 .="\$avmaxheight=\"$avmaxheight\";\n";
	$content2 .="\$avmaxwidth=\"$avmaxwidth\";\n";
	$content2 .="\$maint=\"$maint\";\n";
	$content2 .="\$boardwidth=\"$boardwidth\";\n";
	$content2 .="\$boardalign=\"$boardalign\";\n";
	$content2 .="\$mreason=\"$mreason\";\n";
	$content2 .="\$banreason=\"$banreason\";\n";
	$content2 .="\$adminemail=\"$adminemail\";\n";
	$content2 .="\$noatch=\"$noatch\";\n";
	$content2 .="\$censor=\"$censor\";\n";
	$content2 .="\$html=\"$html\";\n";
	$content2 .="\$smilys=\"$smilys\";\n";
	$content2 .="\$bbcode=\"$bbcode\";\n";
	$content2 .="\$timezone=\"$timezone\";\n";
	$content2 .="\$loginreq=\"$loginreq\";\n";
	$content2 .="\$allowreg=\"$allowreg\";\n";
	$content2 .="\$mailenabled=\"$mailenabled\";\n";
	$content2 .="\$cver=\"$cver\";\n";
	$content2 .="\$csubver=\"$csubver\";\n";
	$content2 .="\$cstat=\"$cstat\";\n";
	$content2 .="\$cstatlevel=\"$cstatlevel\";\n";
	$content2 .="\$maxatchsize=\"$maxatchsize\";\n";
	$content2 .="\$minspace=\"$minspace\";\n";
	$content2 .="\$maxposts=\"$maxposts\";\n";
	$content2 .="\$maxppp=\"$maxppp\";\n";
	$content2 .="\$maxrpp=\"$maxrpp\";\n";
	$content2 .="\$maxlatest=\"$maxlatest\";\n";
	$content2 .="\$confirmemail=\"$confirmemail\";\n";
	$content2 .="\$welcomemessage=\"$welcomemessage\";\n";
	$content2 .="\$checkip=\"$checkip\";\n";
	$content2 .="\$notifyadmin=\"$notifyadmin\";\n";
	$content2 .="\$notifyadminrepl=\"$notifyadminrepl\";\n";
	$content2 .="\$charsets=\"$charsets\";\n";
	$content2 .="\$textareawidth=\"$textareawidth\";\n";
	$content2 .="\$attachtypelist=\"$attachtypelist\";\n";
	$content2 .="\$smtpmail=\"$smtpmail\";\n";
	$content2 .="\n?";
	$content2 .=">";
	if ($dbpath==""){
		$dbpath=getcwd()."/db";
	}
	$fp = fopen($dbpath."/settings.php", "w");
	fputs($fp, $content2);
	fclose($fp);
}

function WriteSendmail()
{
	global $smtp_server,$port,$mydomain,$smtpusername,$password,$dbpath;
	$content2 .="<";
	$content2 .="?php\n\n";
	$content2 .="\$smtp_server=\"$smtp_server\";\n";
	$content2 .="\$port=\"$port\";\n";
	$content2 .="\$mydomain=\"$mydomain\";\n";
	$content2 .="\$smtpusername=\"$smtpusername\";\n";
	$content2 .="\$password=\"$password\";\n";
	$content2 .="\n?";
	$content2 .=">";

	if ($dbpath==""){
		$dbpath=getcwd()."/db";
	}

	$fp = fopen($dbpath."/settings/sendmail.php", "w");
	fputs($fp, $content2);
	fclose($fp);
}

function WriteStyles()
{
	global $picturebuttons,$font,$fontcolor,$fontsize,$annfont,$annfontcolor,$annfontsize,$background,$form,$formborder,$formbordersize,$forumheaderborder;
	global $forumheaderbordersize,$forumheader,$forumheadertitle,$forumheadertitlesize,$forumheadercaption,$forumheaderwelcome,$forumheadermenu;
	global $forumheadermenufont,$datecolor,$containerborder,$containerbordersize,$containerinner,$headercolor,$headergif,$headerfont,$subheadercolor;
	global $subheaderfont,$subheadergif,$menucolor,$fmenucolor,$menufont,$forumbuttoncolor,$forumbuttonover,$forumbuttontopics,$forumbuttonreplies;
	global $forumbuttonlast,$admincolor,$modcolor,$usercolor,$linkcolor,$vlinkcolor,$hlinkcolor,$groupcolors,$group1col,$group2col,$group3col,$group4col;
	global $group5col,$group10col,$group11col,$group12col,$quotefont,$quotefontcolor,$quotefontsize,$quotebackground,$stylepref,$quoteborder;
	@include("global.php");
	$content2 .="<";
	$content2 .="?php\n\n";
	$content2 .="\$picturebuttons=\"$picturebuttons\";\n";
	$content2 .="\$font=\"$font\";\n";
	$content2 .="\$fontcolor=\"$fontcolor\";\n";
	$content2 .="\$fontsize=\"$fontsize\";\n";
	$content2 .="\$quotefont=\"$quotefont\";\n";
	$content2 .="\$quotefontcolor=\"$quotefontcolor\";\n";
	$content2 .="\$quotefontsize=\"$quotefontsize\";\n";
	$content2 .="\$quotebackground=\"$quotebackground\";\n";
	$content2 .="\$quoteborder=\"$quoteborder\";\n";
	$content2 .="\$annfont=\"$annfont\";\n";
	$content2 .="\$annfontcolor=\"$annfontcolor\";\n";
	$content2 .="\$annfontsize=\"$annfontsize\";\n";
	$content2 .="\$background=\"$background\";\n";
	$content2 .="\$form=\"$form\";\n";
	$content2 .="\$formborder=\"$formborder\";\n";
	$content2 .="\$formbordersize=\"$formbordersize\";\n";
	$content2 .="\$forumheaderborder=\"$forumheaderborder\";\n";
	$content2 .="\$forumheaderbordersize=\"$forumheaderbordersize\";\n";
	$content2 .="\$forumheader=\"$forumheader\";\n";
	$content2 .="\$forumheadertitle=\"$forumheadertitle\";\n";
	$content2 .="\$forumheadertitlesize=\"$forumheadertitlesize\";\n";
	$content2 .="\$forumheadercaption=\"$forumheadercaption\";\n";
	$content2 .="\$forumheaderwelcome=\"$forumheaderwelcome\";\n";
	$content2 .="\$forumheadermenu=\"$forumheadermenu\";\n";
	$content2 .="\$forumheadermenufont=\"$forumheadermenufont\";\n";
	$content2 .="\$datecolor=\"$datecolor\";\n";
	$content2 .="\$containerborder=\"$containerborder\";\n";
	$content2 .="\$containerbordersize=\"$containerbordersize\";\n";
	$content2 .="\$containerinner=\"$containerinner\";\n";
	$content2 .="\$headercolor=\"$headercolor\";\n";
	$content2 .="\$headergif=\"$headergif\";\n";
	$content2 .="\$headerfont=\"$headerfont\";\n";
	$content2 .="\$subheadercolor=\"$subheadercolor\";\n";
	$content2 .="\$subheadergif=\"$subheadergif\";\n";
	$content2 .="\$subheaderfont=\"$subheaderfont\";\n";
	$content2 .="\$menucolor=\"$menucolor\";\n";
	$content2 .="\$fmenucolor=\"$fmenucolor\";\n";
	$content2 .="\$menufont=\"$menufont\";\n";
	$content2 .="\$forumbuttoncolor=\"$forumbuttoncolor\";\n";
	$content2 .="\$forumbuttonover=\"$forumbuttonover\";\n";
	$content2 .="\$forumbuttontopics=\"$forumbuttontopics\";\n";
	$content2 .="\$forumbuttonreplies=\"$forumbuttonreplies\";\n";
	$content2 .="\$forumbuttonlast=\"$forumbuttonlast\";\n";
	$content2 .="\$admincolor=\"$admincolor\";\n";
	$content2 .="\$modcolor=\"$modcolor\";\n";
	$content2 .="\$usercolor=\"$usercolor\";\n";
	$content2 .="\$linkcolor=\"$linkcolor\";\n";
	$content2 .="\$vlinkcolor=\"$vlinkcolor\";\n";
	$content2 .="\$hlinkcolor=\"$hlinkcolor\";\n";
	$content2 .="\$groupcolors=\"$groupcolors\";\n";
	$content2 .="\$group1color=\"$group1col\";\n";
	$content2 .="\$group2color=\"$group2col\";\n";
	$content2 .="\$group3color=\"$group3col\";\n";
	$content2 .="\$group4color=\"$group4col\";\n";
	$content2 .="\$group5color=\"$group5col\";\n";
	$content2 .="\$group10color=\"$group10col\";\n";
	$content2 .="\$group11color=\"$group11col\";\n";
	$content2 .="\$group12color=\"$group12col\";\n";
	$content2 .="\$stylepref=\"$stylepref\";\n";
	$content2 .="\n?";
	$content2 .=">";

	if ($dbpath==""){
		$dbpath=getcwd()."/db";
	}

	$fp = fopen($dbpath."/settings/styles/styles.php", "w");
	fputs($fp, $content2);
	fclose($fp);
}

function WriteUserInfo($user)
{
	include("global.php");

	global $userid,$password,$username,$useralias,$userrealname,$useremail,$useremhide,$userip,$usersig,$userslogan,$useravatar,$usermsn;
	global $userposts,$usericq,$userqq,$userwebavatar,$userrank,$useryahoo,$useraim,$userlocation,$userjoined,$userhomepage,$useradmin;
	global $usermod,$userban,$userlastvisit,$userlastpost,$userprevvisit,$useranimsmilies,$lastaliaschange,$userfriend,$userlang,$uservisits;
	global $aliaslist,$noavatars;

	$usersig=CheckSlashes($usersig);
	$usersig=textparse($usersig);
	$userslogan=CheckSlashes($userslogan);
	$userlocation=CheckSlashes($userlocation);
	$userhomepage=CheckSlashes($userhomepage);
	$content = "<?php\n\n";
	$content .="\$userid=\"".fixField($userid)."\";\n";
	$content .="\$password=\"".fixField($password)."\";\n";
	$content .="\$username=\"".fixField($username)."\";\n";
	$content .="\$useralias=\"".fixField($useralias)."\";\n";
	$content .="\$userrealname=\"".fixField($userrealname)."\";\n";
	$content .="\$useremail=\"".fixField($useremail)."\";\n";
	$content .="\$useremhide=\"".fixField($useremhide)."\";\n";
	$content .="\$userip=\"".fixField($userip)."\";\n";
	$content .="\$usersig=\"".fixField($usersig)."\";\n";
	$content .="\$userslogan=\"".fixField($userslogan)."\";\n";
	$content .="\$useravatar=\"".fixField($useravatar)."\";\n";
	$content .="\$usermsn=\"".fixField($usermsn)."\";\n";
	$content .="\$userposts=\"".fixField($userposts)."\";\n";
	$content .="\$usericq=\"".fixField($usericq)."\";\n";
	$content .="\$userqq=\"".fixField($userqq)."\";\n";
	$userwebavatar=stripslashes($userwebavatar);
	$userwebavatar=addslashes($userwebavatar);
	$content .="\$userwebavatar=\"".fixField($userwebavatar)."\";\n";
	$content .="\$userrank=\"".fixField($userrank)."\";\n";
	$content .="\$useryahoo=\"".fixField($useryahoo)."\";\n";
	$content .="\$useraim=\"".fixField($useraim)."\";\n";
	$content .="\$userlocation=\"".fixField($userlocation)."\";\n";
	$content .="\$userjoined=\"".fixField($userjoined)."\";\n";
	$content .="\$userhomepage=\"".fixField($userhomepage)."\";\n";
	$content .="\$useradmin=\"".fixField($useradmin)."\";\n";
	$content .="\$usermod=\"".fixField($usermod)."\";\n";
	$content .="\$userban=\"".fixField($userban)."\";\n";
	$content .="\$userfriend=\"".fixField($userfriend)."\";\n";
	$content .="\$userlastvisit=\"".fixField($userlastvisit)."\";\n";
	$content .="\$userlastpost=\"".fixField($userlastpost)."\";\n";
	$content .="\$userprevvisit=\"".fixField($userprevvisit)."\";\n";
	$content .="\$useranimsmilies=\"".fixField($useranimsmilies)."\";\n";
	$content .="\$lastaliaschange=\"".fixField($lastaliaschange)."\";\n";
	$content .="\$userlang=\"".fixField($userlang)."\";\n";
	$content .="\$uservisits=\"".fixField($uservisits)."\";\n";
	$content .="\$aliaslist=\"".fixField($aliaslist)."\";\n";
	$content .="\$noavatars=\"".fixField($noavatars)."\";\n";
	$content .="\n?";
	$content .=">";

	if (!$userid && !$username && !$userjoined){
	}else{
		$filename=$dbpath.'/members/'.$user;
		$timestamp=@filemtime($filename);
		if ( !@touch($filename,$timestamp)){
			copy($filename, $filename.'.tmp');
			unlink($filename);
			copy($filename.'.tmp', $filename);
			unlink($filename.'.tmp');
			@chmod($filename,0666);
		}
		$fp = fopen($filename, "w");
		fputs($fp, $content);
		fclose($fp);
		@chmod($filename,0666);
		$newtimestamp=filemtime($filename);
		if ($timestamp>=$newtimestamp){
			@touch($filename,$timestamp+1);
		}
	}
}
//Other file-functions ********************************************

function AddGuestCount()
{
	include("global.php");
	$filename=$dbpath."/guestcount";
	$fd=@fopen($filename,"r");
	$num = @fread ($fd, filesize ($filename));
	@fclose ($fd);
	$num++;
	$fp=@fopen($filename,"w");
	@fputs($fp,$num);
	@fclose($fp);

}

function UpdateStats($uname,$ual)
{
	include("global.php");
	$filename=$dbpath."/stats";
	if (file_exists($filename)){
		include($filename);
	}
	if ($uname=="" && $ual=="guest"){
		$todaysguests++;
		$totalguests;
	}
	$totalvisits++;
	$tomorrow=$today+86400;
	if (time()>$tomorrow){
		$todaysvisits=0;
		$tod=getdate(time());
		$today=mktime(0,0,0,$tod['mon'],$tod['mday'],$tod['year']);
	}
	$todaysvisits++;
	if ($uname!=""){
		if($ual==$lastvis9 || $ual==$lastvis8 || $ual==$lastvis7 || $ual==$lastvis6 || $ual==$lastvis5 ||
			$ual==$lastvis4 || $ual==$lastvis3 || $ual==$lastvis2 || $ual==$lastvis1){
			$lastvis10=$lastvis10;
		}else{
			$lastvis10=$lastvis9;
		}
		if($ual==$lastvis8 || $ual==$lastvis7 || $ual==$lastvis6 || $ual==$lastvis5 ||
			$ual==$lastvis4 || $ual==$lastvis3 || $ual==$lastvis2 || $ual==$lastvis1){
			$lastvis9=$lastvis9;
		}else{
			$lastvis9=$lastvis8;
		}
		if($ual==$lastvis7 || $ual==$lastvis6 || $ual==$lastvis5 || $ual==$lastvis4 || $ual==$lastvis3 || $ual==$lastvis2 || $ual==$lastvis1){
			$lastvis8=$lastvis8;
		}else{
			$lastvis8=$lastvis7;
		}
		if($ual==$lastvis6 || $ual==$lastvis5 || $ual==$lastvis4 || $ual==$lastvis3 || $ual==$lastvis2 || $ual==$lastvis1){
			$lastvis7=$lastvis7;
		}else{
			$lastvis7=$lastvis6;
		}
		if($ual==$lastvis5 || $ual==$lastvis4 || $ual==$lastvis3 || $ual==$lastvis2 || $ual==$lastvis1){
			$lastvis6=$lastvis6;
		}else{
			$lastvis6=$lastvis5;
		}
		if($ual==$lastvis4 || $ual==$lastvis3 || $ual==$lastvis2 || $ual==$lastvis1){
			$lastvis5=$lastvis5;
		}else{
			$lastvis5=$lastvis4;
		}
		if($ual==$lastvis3 || $ual==$lastvis2 || $ual==$lastvis1){
			$lastvis4=$lastvis4;
		}else{
			$lastvis4=$lastvis3;
		}
		if($ual==$lastvis2 || $ual==$lastvis1){
			$lastvis3=$lastvis3;
		}else{
			$lastvis3=$lastvis2;
		}
		if($ual==$lastvis1){
			$lastvis2=$lastvis2;
		}else{
			$lastvis2=$lastvis1;
			$lastvis1=$ual;
		}
	}
	$content="<";
	$content.="?php\n\n";
	$content .="\$todaysvisits=\"$todaysvisits\";\n";
	$content .="\$totalvisits=\"$totalvisits\";\n";
	$content .="\$today=\"$today\";\n";
	$content .="\$lastvis10=\"$lastvis10\";\n";
	$content .="\$lastvis9=\"$lastvis9\";\n";
	$content .="\$lastvis8=\"$lastvis8\";\n";
	$content .="\$lastvis7=\"$lastvis7\";\n";
	$content .="\$lastvis6=\"$lastvis6\";\n";
	$content .="\$lastvis5=\"$lastvis5\";\n";
	$content .="\$lastvis4=\"$lastvis4\";\n";
	$content .="\$lastvis3=\"$lastvis3\";\n";
	$content .="\$lastvis2=\"$lastvis2\";\n";
	$content .="\$lastvis1=\"$lastvis1\";\n\n";
	$content .="?";
	$content .=">";

	$fp=@fopen($filename,"w");
	@fputs($fp,$content);
	@fclose($fp);

}
?>
