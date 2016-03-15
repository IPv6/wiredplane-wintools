<?php

/**************************************************
PBLang - addon to display latest posts of a forum on a separate page
call:

GetLatestMessage(str URL_to_forum_without_http://,str document_path_of_the_server,int Number_of_posts_to_display);

Note that the page on which the latest posts shall be displayed must be hosted on the same server
This file should be in the folder of the page where the list shall be posted

-------------------------------------
Usage example:

include('GetLatestMessage.php');
$LastMsg=GetLatestMessage('www.your_domain.com/forum','/home/www/userid/html/forum',5);
for ($i=0;$i<$LastMsg['count'];$i++){
	echo "<a href=\"http://",$LastMsg['URL'][$i],"\" target=\"_TOP\"><font face=\"verdana\" size=\"2\">",$LastMsg['subj'][$i],"</a> by ",
	$LastMsg['auth'][$i]," on ",strftime("%d.%m.%y - %H:%Mh",$LastMsg['date'][$i]),"</font><br>";
}

----------------------------------------

Last modified: 11 October 2003
This file is entirely programmed by Dr. Martinus (Martin Senftleben) and does not base on any other person's code
Released under the GPL

see docs/copyright.txt for further details

***************************************************/

function GetLatestMessage($URL,$basedir,$count)
{
	$counter=0;
	$listlatestposts=@file($basedir."/db/settings/latestposts");
	$countlatestposts=count($listlatestposts);
	for ($j=0;$j<=$countlatestposts;$j++){
		$catid=strtok($listlatestposts[$j],"|");
		$forid=strtok("|");
		$postid=trim(strtok("|"));
		if (substr_count($listlatestposts[$j],'|')==3){
			$repid=trim(strtok('|'));
			$filename=$basedir.'/db/posts/'.$catid.'_'.$forid.'_'.$postid.'_r_'.$repid;
			$LatestMsg['URL'][$counter]=$URL.'/post.php?cat='.$catid.'&fid='.$forid.'&pid='.$postid.'&page=1';
			$isreply='1';
		}else{
			$filename=$basedir.'/db/posts/'.$catid.'_'.$forid.'_'.$postid;
			$LatestMsg['URL'][$counter]=$URL.'/post.php?cat='.$catid.'&fid='.$forid.'&pid='.$postid.'&page=1';
			$isreply='0';
		}
		if (file_exists($filename)){
			include($filename);
			include($basedir.'/db/cats/'.$catid.'_'.$forid);
			if(strpos($fusers,'all')){
				if ($isreply){
					if (file_exists($basedir.'/db/members/'.$rauthor)){
						include($basedir.'/db/members/'.$rauthor);
						$LatestMsg['auth'][$counter]=$useralias;
					}else{
						$LatestMsg['auth'][$counter]='';
					}
					$LatestMsg['subj'][$counter]=$rsubject;
					$LatestMsg['date'][$counter]=$rdate;
				}else{
					if (file_exists($basedir.'/db/members/'.$pauthor)){
						include($basedir.'/db/members/'.$pauthor);
						$LatestMsg['auth'][$counter]=$useralias;
					}else{
						$LatestMsg['auth'][$counter]='';
					}
					$LatestMsg['subj'][$counter]=$psubject;
					$LatestMsg['date'][$counter]=$pdate;
				}
				$counter++;
			}
			if ($counter==$count){
				break;
			}
		}
	}
	$LatestMsg['cnt']=$counter;
	return ($LatestMsg);
}
?>
