<?php
/***************************************************************************
 *                            editpost.php  - PBLang
 *                            -------------------
 *        see copyrights.txt in the docs-folder
 *
 *        updated 7 October 2003
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("functions.php");
include_once("ffunctions.php");

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1") {
	$language=SetLanguage($liusername);
	include($temppath."/pb/language/lang_".$language.".php");
	$idnum=$HTTP_GET_VARS['idnum'];
	$fid=$HTTP_GET_VARS['fid'];
	$cat=$HTTP_GET_VARS['cat'];
	$pid=$HTTP_GET_VARS['pid'];
	$type=$HTTP_GET_VARS['t'];
	$rep=$HTTP_GET_VARS['r'];
	$currpage=$HTTP_GET_VARS['page'];
	$msg=stripslashes($HTTP_POST_VARS['message']);
	$subj=stripslashes($HTTP_POST_VARS['subject']);

	$subj=iconv('utf-8','windows-1251',$subj);
	$msg=iconv('utf-8','windows-1251',$msg);

	
	$emnotify=$HTTP_POST_VARS['EMNotify'];
	writeheader($newestm,0,0,0,$liusername,"1",$EditPost);
	include($dbpath."/members/".$liusername);

	$lockedforum=GetForumStatus($cat,$fid);
	if ($lockedforum=="1" && !$admin=="1") {
			ErrorMessage($NoEditInLockedForum,$liusername);
	} elseif ($lockedforum=="0" || $admin=="1" || $lockedforum=='2') {
		$filename=$dbpath."/posts/".$cat."_".$fid."_".$pid;
		include ($filename);
		if ($plock=="unlocked" || $plock=="") {
			$locked="0";
		} elseif ($plock=="locked") {
			$locked="1";
			if ($admin!="1") {
					ErrorMessage($NoEditInLockedForum,$liusername);
			}
		}
		if ($locked=="0" || $admin=="1") {
			if (trim($idnum)=="") {
				if ($type=="1") {
					include($dbpath."/posts/".$cat."_".$fid."_".$pid);
					$pname=$psubject;
					$pauth=$pauthor;
					$pcont=$pcontent;
				}
				if ($type=="2") {
					include($dbpath."/posts/".$cat."_".$fid."_".$pid."_r_".$rep);
					$pname=$rsubject;
					$pauth=$rauthor;
					$pcont=$rcontent;
				}
				if ($pauth==$liusername || $admin=="1" || $moder=="1") {
					$lockedforum=GetForumStatus($cat,$fid);
					if ($lockedforum=="1" && $admin!="1") {
						ErrorMessage($NoEditInLockedForum,$liusername);
						exit;
					} elseif ($lockedforum=="0" || $admin=="1" || $lockedforum=='2') {
						echo "<script language=\"JavaScript1.2\" src=\"ubbc.js\" type=\"text/javascript\"></script>";
						WriteTableTop();
						echo '<form method="post" action="editpost.php?idnum=2&cat=',$cat,'&pid=',$pid,'&fid=',$fid,'&t=',$type,'&r=',$rep,'" ';
						echo 'name="postmodify" enctype="multipart/form-data" onSubmit="submitonce(this);">';
						echo "<tr>";
						echo "<td class=\"header\" colspan=\"2\"><a href=\"index.php\">$sitetitle</a> :: $VEditPost</td></tr>";
						echo "<tr bgcolor=\"$fmenucolor\"><td height=\"20\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"19%\"><font color=\"$menufont\">$VUsername:</font></td>";
						echo "<td height=\"20\" bgcolor=\"$menucolor\" width=\"81%\"> $pauth</td></tr>";
						echo "<tr bgcolor=\"$fmenucolor\"><td height=\"20\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"19%\"><font color=\"$menufont\">$SubjectIcon:</font></td>";
						echo "<td height=\"20\" bgcolor=\"$menucolor\" width=\"81%\">\n";
						echo "<input type=\"text\" name=\"subject\" size=\"$textareawidth\" value=\"$pname\"></td></tr>";
						PrintSmileys($useranimsmilies,$admin,$language);
						PrintPBCode($admin,$language);
						echo "<tr bgcolor=\"$fmenucolor\"><td height=\"20\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"19%\" valign=\"top\">\n
						<font color=\"$menufont\">$VContent:</font></td>";
						echo "<td height=\"20\" bgcolor=\"$menucolor\" width=\"81%\">";
						echo "<textarea name=\"message\" rows=\"12\" cols=\"$textareawidth\" onselect=\"storeCaret(this)\" onclick=\"storeCaret(this)\" onkeyup=\"storeCaret(this)\">$pcont</textarea></td></tr>";
						if ($emsupp=="1"){
							echo "<tr><td  class=\"label\">$EmailNotification: </td>";
							echo "<td  class=\"content\"><input type=\"checkbox\" name=\"EMNotify\"";
							if (strpos($pnotify,$liusername)){
								echo " checked";
							}
							echo ">&nbsp;$VNotifyByEmail?</td></tr>";
						}
						echo "<tr bgcolor=\"$menucolor\"><td height=\"20\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"19%\" valign=\"middle\">$VSubmitForm:</td>";
						echo "<td height=\"20\" bgcolor=\"$menucolor\" width=\"81%\">";
						echo "<input type=\"submit\" name=\"Submit\" value=\"$VSubmit\">";//&nbsp;&nbsp;<input type=\"reset\" name=\"Submit2\" value=\"$VReset\">
						sbot(1);
					} else {
						ErrorMessage($AccessDenied,$liusername);
						exit;
					}
				}
			}
			if ($idnum=="2") {
				if ($fileupload_name != "") {
					if ($noatch=="1" && $admin=="0") {
						ErrorMessage($UploadsNotAllowed,$liusername);
						exit;
					} else {
						$alreadythere=1;
						$fileupldname=$fileupload_name;
						$fext=substr($fileupload_name,strlen($fileupload_name)-3);
						if (diskfreespace($mainpath."/attachments")>$minspace){
							$typeallowed=Checkfiletype($fext);
							if ($typeallowed || !strstr($fileupload_name,".")){
//							if ($fext=="txt" || $fext=="gif" || $fext=="jpg" || $fext=="tif" || $fext=="zip" || $fext=="pdf" || $fext=="rar" || !strstr($fileupload_name,".")){
								while($alreadythere){
									if (file_exists($mainpath."/attachments/".$fileupldname)){
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
									$msg .="\n\n[b] ".$VAttachment."[/b]:[att]".$fileupldname."[/att]";
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
				$postdat=time()+($timezone*3600);
				if ($type=="1") {
					$filename=$dbpath."/posts/".$cat."_".$fid."_".$pid;
					SetLock();
					include($filename);
					$pcontent=textparse($msg);
					$psubject=textparse($subj);
					$pdate=$postdat;
					if ($emnotify=='on'){
						if (!strpos($pnotify,$liusername)){
							$pnotify.='|'.$liusername;
						}
					}else{
						if(strpos($pnotify,$liusername)){
							$pnotify=str_replace('|'.$liusername,'',$pnotify);
						}
					}
					if ($plastdate<$postdat){
						$plastdate=$postdat;
					}
					WritePostInfo($cat,$fid,$pid,$psticky);
					WriteLatestPosts($cat,$fid,$pid,'');
					@unlink(  $dbpath.'/block');
				}elseif($type=="2"){
					$filename=$dbpath."/posts/".$cat."_".$fid."_".$pid."_r_".$rep;
					include($filename);
					$rcontent=textparse($msg);
					$rsubject=textparse($subj);
					$rdate=$postdat;
					WriteReplyInfo($cat,$fid,$pid,$rep);
					WriteLatestPosts($cat,$fid,$pid,$rep);
				}
				SetLock();
				include($dbpath."/posts/".$cat."_".$fid."_".$pid);
				$pvisitors="|".$liusername;
				WritePostInfo($cat,$fid,$pid,"");
				@unlink($dbpath.'/block');

				SetLock();
				include($dbpath."/cats/".$cat."_".$fid);
				$flastpost=$postdat;
				$flastauthor=$liusername;
				$flasturl="post.php?cat=$cat&fid=$fid&pid=$pid&page=1";
				$fvisitors="|".$liusername;
				WriteForumInfo($cat,$fid);
				@unlink($dbpath.'/block');

				msg("$VEdited","$PostWait");
				echo "<meta http-equiv=\"Refresh\" content=\"0; URL=post.php?cat=$cat&fid=$fid&pid=$pid&page=$currpage&v=0\">";
			}
		}
		writefooter($newestm);
	}
}
ob_end_flush();
?>

