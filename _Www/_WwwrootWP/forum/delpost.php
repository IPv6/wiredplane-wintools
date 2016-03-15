<?php
/***************************************************************************
*                            delpost.php
*                            -------------------
*
*              see the file docs/copyright.txt for further details
*
*   last updated 3 Oct 2003
***************************************************************************/


require ('header.php');
include("global.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once("scan.php");
include($dbpath."/settings.php");

$username=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($username,"1","0");
if ($permit=="1") {
	$language=SetLanguage($username);
	include("$temppath/pb/language/lang_$language.php");
	$cat=$HTTP_GET_VARS['cat'];
	$fid=$HTTP_GET_VARS['fid'];
	$pid=$HTTP_GET_VARS['pid'];
	$type=$HTTP_GET_VARS['t'];
	$reply=$HTTP_GET_VARS['r'];
	if ($type==2 && $reply>=1) {          //if it's a reply...
		$filename = $dbpath."/posts/".$cat."_".$fid."_".$pid."_r_".$reply;
		$delreply=TRUE;
	} else {
		$filename = $dbpath."/posts/".$cat."_".$fid."_".$pid;
		$delreply=FALSE;
	}
	if (!file_exists($filename)){
		ErrorMessage($NoSuchFile." - ".$filename,$username);
		exit;
	}

	include($filename);
	if ($delreply){
		$pauth=$rauthor;
	}else{
		$pauth=$pauthor;
	}
	//determine how many replies exist in this topic
	include($dbpath."/posts/".$cat."_".$fid."_".$pid);
	$replies=$preplies;
	if ($pauth!=$username && !$admin && !$moder) {      // Only author or Admin or moderator may delete a message
		ErrorMessage($DeletePostNotAllowed,$username);
		exit;
	} elseif (!$type && $replies && !$admin) {         //if it's the first post of a thread with replies, it shouldn't be deleted unless you are admin
		ErrorMessage($DeletePostNotAllowed,$username);
		exit;
	//now the checks are done, the deletion can start:
	} else {
		if ($type==2){           //it's a reply
			include($dbpath."/posts/".$cat."_".$fid."_".$pid);
			$newreplycount=$preplies-1;
			$orgauth=$rauthor;
			//delete this reply
			unlink($filename);
			SetLock();
			if ($newreplycount=="0"){  //there is only one post left
				include($dbpath."/posts/".$cat."_".$fid."_".$pid);
				$newplastauthor=$pauthor;
				$newplastdate=$pdate;
				$newplastreply='0';
			}else{                 //there are other replies. Find out the latest and set the related info
				include($dbpath."/posts/".$cat."_".$fid."_".$pid);
				$num=$plastreply;
				$lfilename=$dbpath."/posts/".$cat."_".$fid."_".$pid."_r_";
				$filename=$lfilename.$num;
				while (!file_exists($filename)){ //walk through replies from highest to get latest reply
					$num--;
					$filename=$lfilename.$num;
				}
				include($dbpath."/posts/".$cat."_".$fid."_".$pid."_r_".$num);
				$newplastauthor=$rauthor;
				$newplastdate=$rdate;
				$newplastreply=$num;
			}
			$plastauthor=$newplastauthor;
			$plastdate=$newplastdate;
			$preplies=$newreplycount;
			$plastreply=$newplastreply;
			WritePostInfo($cat,$fid,$pid,'');
			//Now put the lastauthor, lasturl and lastdate into the forum-file in cats
			include($dbpath."/cats/".$cat."_".$fid);
			$freplies--;
			$LastPost=GetLatestForumMessage($cat,$fid,$flastpostnumber);
			$flastpost=$LastPost['date'];
			$flastauthor=$LastPost['author'];
			$flasturl='post.php?cat='.$cat.'&fid='.$fid.'&pid='.$LastPost['id'].'&page=1';
			WriteForumInfo($cat,$fid);
			Unlock();
		} else {                        //it's the first post
			SetLock();
			include($dbpath.'/cats/'.$cat.'_'.$fid);
			$forumreplies=$freplies;
			if ($replies>"0" && $admin){        //admin may delete the first post even if there are replies, it's equal to deleting the whole topic
				//delete all the replies first
				include($dbpath."/posts/".$cat."_".$fid."_".$pid);
				$allposts=$plastreply;
				for ($i=$allposts;$i>=1;$i--){
					$remfilename=$dbpath."/posts/".$cat."_".$fid."_".$pid."_r_".$i;
					if (file_exists($remfilename)){
						unlink ($remfilename);
						$forumreplies--;
					}
				}
				$replies=0;
			}elseif($replies>0){	//member isn't admin and may not delete the first message of a thread
				ErrorMessage($DeletePostNotAllowed,$username);
				exit;
			}
			include($dbpath."/posts/".$cat."_".$fid."_".$pid);
			$orgauth=$pauthor;
			@unlink($filename);
			//reduce the number of topics in the category
			$ftopics--;
			$freplies=$forumreplies;
			if ($flastpostnumber>$pid){
			}else{
				$flastpostnumber--;
			}
			WriteForumInfo($cat,$fid);
			Unlock();
			$LastPost=GetLatestForumMessage($cat,$fid,$flastpostnumber);
			SetLock();
			include($dbpath.'/cats/'.$cat.'_'.$fid);
			if ($LastPost['author']!=''){
				$flastpost=$LastPost['date'];
				$flastauthor=$LastPost['author'];
				$flasturl='post.php?cat='.$cat.'&fid='.$fid.'&pid='.$LastPost['id'].'&page=1';
				WriteForumInfo($cat,$fid);
			}else{
				$flastpost='';
				$flastauthor='';
				$flasturl='';
				WriteForumInfo($cat,$fid);
			}
			Unlock();
		}
		//reduce  author's postings-counter
		include ($dbpath."/members/".$orgauth);
		if (!$admin||$moder){			//if the admin or moderator deletes the post for space reasons, the user should still keep his/her post count
			$userposts--;
			WriteUserInfo($orgauth);
		}
		if ($userrank<"6" || $userrank>"9"){
			include ($dbpath."/members/".$orgauth);
			updatestatus($orgauth);
			WriteUserInfo($orgauth);
		}
	}
}
//return to index (can we return to the forum? Should be possible. Look into that later
echo '<meta http-equiv="Refresh" content="0; URL=index.php">';

ob_end_flush();

?>

