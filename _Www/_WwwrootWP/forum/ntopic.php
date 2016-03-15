<?php
/***************************************************************************
 *                            ntopic.php
 *                            -------------------
 *
 *                  see copyright.txt in the docs-folder!!
 *
 *        updated 7 October 2003
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("scan.php");
include_once("functions.php");
include_once("ffunctions.php");
$boardlang=$language;

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1") {
	$language=SetLanguage($liusername);
	include($temppath."/pb/language/lang_".$language.".php");
	setlocale(LC_TIME,$LangLocale);
	$idnum=$HTTP_GET_VARS['idnum'];
	writeheader($newestm,0,0,0,$liusername,"1",$VNewTopic);

	if (trim($idnum)=='') {
		$fid=$HTTP_GET_VARS['fid'];
		$cat=$HTTP_GET_VARS['cat'];
		include($dbpath.'/members/'.$liusername);
		$admin=$useradmin;
//          ntopic($fid,$cat,$HTTP_COOKIE_VARS[$cookieid]);
		$lockedforum=GetForumStatus($cat,$fid);
		if ($lockedforum=="1" && $admin!="1") {
			ErrorMessage($NoNewTopicInLockedForum,$liusername);
		} elseif ($lockedforum=="0" || $admin=="1") {
			include($dbpath."/cats/".$cat."_".$fid);
			$forumname=replacestuff($fname);
			include($dbpath."/cats/".$cat);
			$catname=replacestuff($cname);
			$stitle=replacestuff($sitetitle);

			echo "<script language=\"JavaScript1.2\" src=\"ubbc.js\" type=\"text/javascript\"></script>";
			WriteTableTop();
			echo "\n<form enctype=\"multipart/form-data\" method=\"post\" action=\"ntopic.php?idnum=2\" name=\"postmodify\" onSubmit=\"submitonce(this);\">";
			echo "<tr><td class=\"header\" colspan=\"2\">";
			echo "<a href=\"index.php\">$stitle</a> :: $catname :: $forumname :: $VNewTopic</td></tr>\n";
//			echo "<tr><td class=\"subheader\" colspan=\"2\">$VPost $VOptions :: </td></tr>";
			echo "<tr><td class=\"label\"><font color=\"$menufont\">$VUsername:</font></td>";
			echo "<td class=\"content\"> $liusername</td></tr>";
			echo "<tr><td class=\"label\"><font color=\"$menufont\">$SubjectIcon:</font></td>";
			echo "<td  class=\"content\">\n";
			echo "<input type=\"text\" name=\"subject\" size=\"$textareawidth\">";
			echo "<input type=\"hidden\" name=\"fid\" value=\"$fid\" size=\"40\">";
			echo "<input type=\"hidden\" name=\"cat\" value=\"$cat\" size=\"40\">";

			$iconimage1=GetImage($stylepref,'1','icon/');
			$iconimage2=GetImage($stylepref,'2','icon/');
			$iconimage3=GetImage($stylepref,'3','icon/');
			$iconimage4=GetImage($stylepref,'4','icon/');
			$iconimage5=GetImage($stylepref,'5','icon/');
			$iconimage6=GetImage($stylepref,'6','icon/');
			$iconimage7=GetImage($stylepref,'7','icon/');

			echo "<BR><input type=\"radio\" name=\"topicicon\" value=\"1.gif\" CHECKED>&nbsp;";
			echo "<IMG SRC=\"$iconimage1\" ALIGN=\"bottom\" BORDER=0 alt=\"$VStandard\">&nbsp; ";
			echo "<input type=\"radio\" name=\"topicicon\" value=\"2.gif\">&nbsp;";
			echo "<IMG SRC=\"$iconimage2\" ALIGN=\"bottom\" BORDER=0 alt=\"$VThumbsUp\">&nbsp; ";
			echo "<input type=\"radio\" name=\"topicicon\" value=\"3.gif\">&nbsp;";
			echo "<IMG SRC=\"$iconimage3\" ALIGN=\"bottom\" BORDER=0 alt=\"$VThumbsDown\">&nbsp; ";
			echo "<input type=\"radio\" name=\"topicicon\" value=\"4.gif\">&nbsp;";
			echo "<IMG SRC=\"$iconimage4\" ALIGN=\"bottom\" BORDER=0 alt =\"$VExplanationPoint\">&nbsp; ";
			echo "<input type=\"radio\" name=\"topicicon\" value=\"5.gif\">&nbsp;";
			echo "<IMG SRC=\"$iconimage5\" ALIGN=\"bottom\" BORDER=0 alt=\"$VQuestionMark\">&nbsp; ";
			echo "<input type=\"radio\" name=\"topicicon\" value=\"6.gif\">&nbsp;";
			echo "<IMG SRC=\"$iconimage6\" ALIGN=\"bottom\" BORDER=0 alt=\"$VSmiley\">&nbsp; ";
			echo "<input type=\"radio\" name=\"topicicon\" value=\"7.gif\">&nbsp;";
			echo "<IMG SRC=\"$iconimage7\" ALIGN=\"bottom\" BORDER=0 alt=\"$VAngry\">&nbsp; ";

			PrintSmileys($useranimsmilies,$admin,$language);
			PrintPBCode($admin,$language);

			echo "<tr><td class=\"label\">$VContent:</td>";
			echo "<td class=\"content\">";
			echo "<textarea name=\"message\" rows=\"12\" cols=\"$textareawidth\" onselect=\"storeCaret(this)\" onclick=\"storeCaret(this)\" onkeyup=\"storeCaret(this)\"></textarea></td></tr>";
			if ($emsupp=="1"||$smtpmail){
				echo "<tr><td  class=\"label\">$EmailNotification: </td>";
				echo "<td  class=\"content\"><input type=\"checkbox\" name=\"EMNotify\">&nbsp;$VNotifyByEmail?</td></tr>";
			}
			if ($admin=="1"){
				echo "<tr><td  class=\"label\">$VStickyMessage: </td>";
				echo "<td  class=\"content\"><input type=\"checkbox\" name=\"sticky\">&nbsp;$VStickyMessageExplain</td></tr>";
			}
			echo "<tr><td class=\"subheader\" colspan=\"2\">$VSubmitForm :: </td></tr>";
			echo "<tr><td class=\"label\">&nbsp;</td>";
			echo "<td class=\"content\">";
			echo "<input type=\"submit\" name=\"Submit\" value=\"$VSubmit\">";//&nbsp;<input type=\"reset\" name=\"Submit2\" value=\"$VReset\">
			sbot(1);
		}
	}

	if ($idnum=="2") {
		$user=trim($HTTP_COOKIE_VARS[$cookieid]);
		$sub=stripslashes($HTTP_POST_VARS['subject']);
		$psubject=textparse($sub);
		$ticon=$HTTP_POST_VARS['topicicon'];
		$msg=stripslashes($HTTP_POST_VARS['message']);
		$fid=$HTTP_POST_VARS['fid'];
		$cat=$HTTP_POST_VARS['cat'];
		$EMnotify=$HTTP_POST_VARS['EMNotify'];
		$stickymessage=$HTTP_POST_VARS['sticky'];

		$psubject=iconv('utf-8','windows-1251',$psubject);
		$msg=iconv('utf-8','windows-1251',$msg);


		$lockedforum=GetForumStatus($cat,$fid);
		if ($lockedforum=="1" && $admin!="1") {
			ErrorMessage($NoNewTopicInLockedForum,$liusername);
			exit;
		} elseif ($lockedforum=="0" || $admin=="1") {
//Attachment section
			if ($fileupload_name != "") {
				if ($noatch=="1" && $admin=="0") {
					ErrorMessage($UploadsNotAllowed,$liusername);
					exit;
				} else {
					$alreadythere=1;
					$fileupldname=trim($fileupload_name);
					$fext=substr($fileupload_name,strlen($fileupload_name)-3);
					if (diskfreespace($mainpath."/attachments")>$minspace){
						$typeallowed=Checkfiletype($fext);
						if ($typeallowed || !strstr($fileupload_name,".")){
							while($alreadythere){
								if (file_exists("$mainpath/attachments/$fileupldname")){
									$counter++;
									$fileupldname=$counter.$fileupload_name;
								}else{
									$alreadythere=0;
								}
							}
							$target="$mainpath/attachments/$fileupldname";
							copy($fileupload, $target);
							if (filesize($target)>$maxatchsize && $maxatchsize!="0"){
									ErrorMessage($VFileTooBig,$liusername);
									unlink($target);
									exit;
							}else{
								$msg .="\n\n[b] ".$VAttachment."[/b]: [att]".$fileupldname."[/att]";
							}
						}else{
							ErrorMessage($VForbiddenType,$liusername);
							exit;
						}
					}else{
						ErrorMessage($VQuotaExceeded,$liusername);
						exit;
					}
				}
			}

//creating new message
			$zero="0";

			$postdat =time()+($timezone*3600);
			$pvisitors="|".$user;
			$pauthor=$user;
			$pdate=$postdat;
			$plastreply=0;
			$pcontent=textparse($msg);
			$preplies=0;
			$pimage=$ticon;
			if (rtrim($psubject)==""){
				$subrepl=@substr($pcontent,0,@strpos($pcontent,' ',41));
				if ($subrepl==''){
					$subrepl=$pcontent;
				}
				$psubject=$subrepl;
			}
			$pviews=0;
			$plastdate=$postdat;
			$plastauthor=$user;
			$pip=$userip;

			if ($EMnotify=="on" && !strpos($pnotify,$user)){
				$pnotify="|".$user;
			}else{
				$pnotify="";
			}
			SetLock();
			include($dbpath."/cats/".$cat."_".$fid);
			$ftopics++;
			$flastpostnumber++;
			if ($stickymessage=="on"){
				$fsticky++;
				$psticky="1";
				$postid=chr(96+$fsticky);
			}else{
				$postid=$flastpostnumber;
			}
			$flasturl="post.php?cat=$cat&fid=$fid&pid=$postid&page=1";
			$flastpost=$postdat;
			$flastauthor=$user;
			$fvisitors="|".$user;

			WritePostInfo($cat,$fid,$postid,$stickymessage);	//This doesn't need to be locked, since nobody knows yet that this file exists.
//If the subject and content are empty, the function will exit here to an error message.

			WriteForumInfo($cat,$fid);
			@unlink($dbpath.'/block');

			include($dbpath."/members/".$user);
			$userip = GetIPAddress();
			$pip=$userip;
			$userlastpost=$postdat;
			$userposts++;
			WriteUserInfo($user);
			if ($userrank!="6" && $userrank!="7" && $userrank!="8" && $userrank !="9"){
				include($dbpath."/members/".$user);
				updatestatus($user);
				WriteUserInfo($user);
			}

			SetLock();
			WriteLatestPosts($cat,$fid,$postid,'');
			unlink($dbpath.'/block');
			if ($notifyadmin && ($emsupp || $smtpmail)){
				include($dbpath."/cats/".$cat);
				include($dbpath."/cats/".$cat."_".$fid);
				include($temppath."/pb/language/lang_".$boardlang.".php");
				$purl="$homeurl/post.php?cat=$cat&fid=$fid&pid=$postid&page=1";
				$header=mailheader();
				if ($emailsupp){
					mail($adminemail,"$VNewTopic $VIn $sitetitle","$VNewTopic \n\n$purl - $psubject\n\n $VIn: $cname :: $fname $VBy $user",$header);
				}elseif($smtpmail){
					SendMail($adminemail,"$VNewTopic $VIn $sitetitle","$VNewTopic \n\n$purl - $psubject\n\n $VIn: $cname :: $fname $VBy $user");
				}
				include($temppath."/pb/language/lang_".$language.".php");
			}
			msg("$VPosted",$PostWait);
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=post.php?cat=$cat&fid=$fid&pid=$postid&page=1\">";
		}
	}
	writefooter($newestm);
}
ob_end_flush();
?>


