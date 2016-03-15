<?php
define('IN_PB', "true");
define('IN_PBG', "true");
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once("scan.php");
@include ($dbpath.'/updateprocess.txt');
include($temppath."/pb/language/lang_".$language.".php");

if (!$maint){
	echo "Sorry, you must set the board into maintenance mode before you can do this update process. Please go to the admin center and set
		the board to maintenance mode. Make sure no users are logged in. Then you can proceed with the update.";
	exit;
}

$catcount=$HTTP_GET_VARS['cc'];
if (!$catcount){
     $filename=$dbpath.'/cats/cats';
     $fd=fopen($filename,'r');
     $catcount=fread($fd,filesize($filename));
     fclose($fd);
     for($i=1;$i<=$catcount;$i++){
          @include($dbpath.'/cats/'.$i);
          for($j=1;$j<=$cforums;$j++){
               @include($dbpath.'/cats/'.$i.'_'.$j);
               $totalposts=$fposts+$freplies;
          }
     }
     if ($totalposts>1000){
        $onego=0;
        $nextcat=$currentcat+1;
        if ($nextcat>$catcount){
                echo '<br>ok, I&#39;m done!<br>';
                unlink ($dbpath.'/updateprocess.txt');
                exit;
        }
        $content='<'.'?php'."\n\n";
        $content .="\$onego=\"$onego\";\n";
        $content .="\$currentcat=\"$currentcat\";\n\n";
        $content .='?'.'>';
        $fp=@fopen($dbpath.'/updateprocess.txt','w');
        fputs($fp,$content);
        fclose($fp);
     }else{
        $onego=1;
        $currentcat=0;
     }
}

$handle=opendir($dbpath."/posts");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.html" && $file != "cats") {
          if ($onego=="0"){
               if (substr($file,0,1)==$nextcat){
                    if (strstr($file,"_n") || strstr($file," n")){
                         $filebase=substr($file,0,strlen($file)-2);
                         if (strstr($filebase,"_")){
                              $filebase=str_replace("_"," ",$filebase);
                         }
                         $filebase1=str_replace(" ","_",$filebase);

                         $filename1=$dbpath.'/posts/'.$filebase.' n';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_n';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         $fd=fopen($filename,"r");
                         $psubject=stripslashes(fread($fd,filesize($filename)));
                         fclose($fd);
                         $psubject=textparse($psubject);
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' pa';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_pa';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $pdate=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $$pauthor="";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' a';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_a';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $pauthor=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $$pauthor="";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' a_er';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_a_er';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $emnotify=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $emnotify="";
                         }
                         if ($emnotify=="1"){
                              $pnotify="|".$pauthor;
                         }else{
                              $pnotify="";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' c';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_c';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $pcontent=stripslashes(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $pcontent="";
                         }
                         $pcontent=textparse($pcontent);
                         $pcontent=str_replace("/n",chr(10),$pcontent);
                         $pcontent=str_replace("\n",chr(10),$pcontent);
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' i';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_i';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $pimage=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $pimage="";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' r';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_r';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $preplies=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $preplies="0";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' last';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_last';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $plastreply=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $plastreply=$preplies;
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' lpa';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_lpa';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $plastauthor=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $plastauthor="";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' lpd';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_lpd';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $plastdate=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $plastdate="";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' v';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_v';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $pviews=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $pviews="";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename1=$dbpath.'/posts/'.$filebase.' lock';
                         $filename2=$dbpath.'/posts/'.$filebase1.'_lock';
                         if (file_exists($filename2)){
                              $filename=$filename2;
                         }else{
                              $filename=$filename1;
                         }
                         if (file_exists($filename)){
                              $fd=fopen($filename,"r");
                              $plock=trim(fread($fd,filesize($filename)));
                              fclose($fd);
                         }else{
                              $plock="unlocked";
                         }
                         @unlink ($filename1);
                         @unlink ($filename2);

                         $filename=$dbpath.'/posts/'.$filebase1.'_read';
                         if (file_exists($filename)){
                              $users=@file($filename);
                              $counter=@count($users);
                              for ($i=0;$i<$counter;$i++){
                                   $users[$i]=trim($users[$i]);
                              }
                              $pvisitors="|".@implode("|",$users);
                              if ($pvisitors=="|"){
                                   $pvisitors="";
                              }
                         }
                         @unlink ($filename);


          /*               if (strstr($filebase,"_")){
                              $cid=strtok($filebase,"_");
                              $fid=strtok("_");
                              $pid=strtok("_");
                         }else{
          */
                         $cid=strtok($filebase," ");
                         $fid=strtok(" ");
                         $pid=strtok(" ");

                         if ($plastreply){
                              $preplies="0";
                              for ($i=1;$i<=$plastreply;$i++){
                                   $filename1=$dbpath."/posts/".$cid." ".$fid." ".$pid." r ".$i." a";
                                   $filename2=$dbpath."/posts/".$cid."_".$fid."_".$pid."_r_".$i."_a";
                                   if (!file_exists($filename2)){
                                        $filename=$filename1;
                                   }else{
                                        $filename=$filename2;
                                   }
                                   if (file_exists($filename)){
                                        $found="1";
                                        $preplies++;
                                   }else{
                                        $found="0";
                                   }

                                   if ($found=="1"){
                                        $fd = fopen ($filename, "r");
                                        $rauthor = trim(fread ($fd, filesize ($filename)));
                                        fclose ($fd);
                                        @unlink ($filename1);
                                        @unlink ($filename2);

                                        $filename1=$dbpath.'/posts/'.$cid.' '.$fid.' '.$pid.' r '.$i;
                                        $filename2=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid.'_r_'.$i;
                                        if (!file_exists($filename2)){
                                             $filename=$filename1;
                                        }else{
                                             $filename=$filename2;
                                        }
                                        $fd = fopen ($filename, "r");
                                        $rsubject = trim(fread ($fd, filesize ($filename)));
                                        fclose ($fd);
                                        @unlink ($filename1);
                                        @unlink ($filename2);

                                        $filename1=$dbpath.'/posts/'.$cid.' '.$fid.' '.$pid.' r '.$i.' c';
                                        $filename2=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid.'_r_'.$i.'_c';
                                        if (!file_exists($filename2)){
                                             $filename=$filename1;
                                        }else{
                                             $filename=$filename2;
                                        }
                                        $fd = fopen ($filename, "r");
                                        $rcontent = trim(fread ($fd, filesize ($filename)));
                                        fclose ($fd);
                                        @unlink ($filename1);
                                        @unlink ($filename2);

                                        $filename1=$dbpath.'/posts/'.$cid.' '.$fid.' '.$pid.' r '.$i.' pa';
                                        $filename2=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid.'_r_'.$i.'_pa';
                                        if (!file_exists($filename2)){
                                             $filename=$filename1;
                                        }else{
                                             $filename=$filename2;
                                        }
                                        $fd = fopen ($filename, "r");
                                        $rdate = trim(fread ($fd, filesize ($filename)));
                                        fclose ($fd);
                                        @unlink ($filename1);
                                        @unlink ($filename2);

                                        $rip="";
                                        if ($rname=='delplz'){
                                        }else{
                                                WriteReplyInfo($cid,$fid,$pid,$i);
                                                echo '   updated reply &quot;',$rsubject,'&quot;<br>';
                                        }
                                   }
                              }
                         }
                         if ($pname=='delplz' || $pname=='delplz'){
                         }else{
                         WritePostInfo($cid,$fid,$pid,"");
                              echo 'updated message &quot;',$psubject,'&quot;<br>';
                         }
                    }
               }
          }else{
               if (strstr($file,"_n") || strstr($file," n")){
                    $filebase=substr($file,0,strlen($file)-2);
                    if (strstr($filebase,"_")){
                         $filebase=str_replace("_"," ",$filebase);
                    }
                    $filebase1=str_replace(" ","_",$filebase);

                    $filename1=$dbpath.'/posts/'.$filebase.' n';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_n';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    $fd=fopen($filename,"r");
                    $psubject=stripslashes(fread($fd,filesize($filename)));
                    fclose($fd);
                    $psubject=textparse($psubject);
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' pa';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_pa';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $pdate=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $$pauthor="";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' a';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_a';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $pauthor=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $$pauthor="";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' a_er';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_a_er';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $emnotify=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $emnotify="";
                    }
                    if ($emnotify=="1"){
                         $pnotify="|".$pauthor;
                    }else{
                         $pnotify="";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' c';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_c';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $pcontent=stripslashes(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $pcontent="";
                    }
                    $pcontent=textparse($pcontent);
                    $pcontent=str_replace("/n",chr(10),$pcontent);
                    $pcontent=str_replace("\n",chr(10),$pcontent);
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' i';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_i';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $pimage=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $pimage="";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' r';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_r';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $preplies=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $preplies="0";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' last';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_last';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $plastreply=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $plastreply=$preplies;
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' lpa';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_lpa';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $plastauthor=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $plastauthor="";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' lpd';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_lpd';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $plastdate=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $plastdate="";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' v';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_v';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $pviews=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $pviews="";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename1=$dbpath.'/posts/'.$filebase.' lock';
                    $filename2=$dbpath.'/posts/'.$filebase1.'_lock';
                    if (file_exists($filename2)){
                         $filename=$filename2;
                    }else{
                         $filename=$filename1;
                    }
                    if (file_exists($filename)){
                         $fd=fopen($filename,"r");
                         $plock=trim(fread($fd,filesize($filename)));
                         fclose($fd);
                    }else{
                         $plock="unlocked";
                    }
                    @unlink ($filename1);
                    @unlink ($filename2);

                    $filename=$dbpath.'/posts/'.$filebase1.'_read';
                    if (file_exists($filename)){
                         $users=@file($filename);
                         $counter=@count($users);
                         for ($i=0;$i<$counter;$i++){
                              $users[$i]=trim($users[$i]);
                         }
                         $pvisitors="|".@implode("|",$users);
                         if ($pvisitors=="|"){
                              $pvisitors="";
                         }
                    }
                    @unlink ($filename);


     /*               if (strstr($filebase,"_")){
                         $cid=strtok($filebase,"_");
                         $fid=strtok("_");
                         $pid=strtok("_");
                    }else{
     */
                    $cid=strtok($filebase," ");
                    $fid=strtok(" ");
                    $pid=strtok(" ");

                    if ($plastreply){
                         $preplies="0";
                         for ($i=1;$i<=$plastreply;$i++){
                              $filename1=$dbpath."/posts/".$cid." ".$fid." ".$pid." r ".$i." a";
                              $filename2=$dbpath."/posts/".$cid."_".$fid."_".$pid."_r_".$i."_a";
                              if (!file_exists($filename2)){
                                   $filename=$filename1;
                              }else{
                                   $filename=$filename2;
                              }
                              if (file_exists($filename)){
                                   $found="1";
                                   $preplies++;
                              }else{
                                   $found="0";
                              }

                              if ($found=="1"){
                                   $fd = fopen ($filename, "r");
                                   $rauthor = trim(fread ($fd, filesize ($filename)));
                                   fclose ($fd);
                                   @unlink ($filename1);
                                   @unlink ($filename2);

                                   $filename1=$dbpath.'/posts/'.$cid.' '.$fid.' '.$pid.' r '.$i;
                                   $filename2=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid.'_r_'.$i;
                                   if (!file_exists($filename2)){
                                        $filename=$filename1;
                                   }else{
                                        $filename=$filename2;
                                   }
                                   $fd = fopen ($filename, "r");
                                   $rsubject = trim(fread ($fd, filesize ($filename)));
                                   fclose ($fd);
                                   @unlink ($filename1);
                                   @unlink ($filename2);

                                   $filename1=$dbpath.'/posts/'.$cid.' '.$fid.' '.$pid.' r '.$i.' c';
                                   $filename2=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid.'_r_'.$i.'_c';
                                   if (!file_exists($filename2)){
                                        $filename=$filename1;
                                   }else{
                                        $filename=$filename2;
                                   }
                                   $fd = fopen ($filename, "r");
                                   $rcontent = trim(fread ($fd, filesize ($filename)));
                                   fclose ($fd);
                                   @unlink ($filename1);
                                   @unlink ($filename2);

                                   $filename1=$dbpath.'/posts/'.$cid.' '.$fid.' '.$pid.' r '.$i.' pa';
                                   $filename2=$dbpath.'/posts/'.$cid.'_'.$fid.'_'.$pid.'_r_'.$i.'_pa';
                                   if (!file_exists($filename2)){
                                        $filename=$filename1;
                                   }else{
                                        $filename=$filename2;
                                   }
                                   $fd = fopen ($filename, "r");
                                   $rdate = trim(fread ($fd, filesize ($filename)));
                                   fclose ($fd);
                                   @unlink ($filename1);
                                   @unlink ($filename2);

                                   $rip="";

                                        if ($rname=='delplz' || $pname=='delplz'){
                                        }else{
                                                WriteReplyInfo($cid,$fid,$pid,$i);
                                                echo '   updated reply &quot;',$rsubject,'&quot;<br>';
                                        }
                              }
                         }
                    }
                    if ($pname=="delplz"){
                    }else{
                    WritePostInfo($cid,$fid,$pid,"");
                         echo 'updated message &quot;',$psubject,'&quot;<br>';
                    }
               }
          }
     }
}
closedir($handle);
if ($onego=='0'){
        echo "<meta http-equiv=\"Refresh\" content=\"0; URL=update-2.php?cc=$catcount\">";
}

?>


