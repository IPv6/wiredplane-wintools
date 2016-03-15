<?php
define('IN_PB', "true");
define('IN_PBG', "true");
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once("scan.php");
include($temppath."/pb/language/lang_".$language.".php");

//update the categories
if (!$maint){
	echo "Sorry, you must set the board into maintenance mode before you can do this update process. Please go to the admin center and set
		the board to maintenance mode. Make sure no users are logged in. Then you can proceed with the update.";
	exit;
}

$handle=opendir($dbpath."/cats");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.html" && $file != "cats") {
          if (strstr($file," ") || strstr($file,"_") || $file=="100"){
          }else{
               $filename=$dbpath."/cats/".$file;
               $fd=fopen($filename,"r");
               $cname=fread($fd,filesize($filename));
               fclose($fd);
               unlink ($filename);

              $filename=$dbpath."/cats/".$file."_d";
               if (!file_exists($filename))
                         {$filename=str_replace("_"," ",$filename);}
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $cdescription=fread($fd,filesize($filename));
                    fclose($fd);
               }
               unlink ($filename);
               $filename=str_replace("_"," ",$filename);
               @unlink($filename);

               $filename=$dbpath."/cats/".$file."_f";
               if (!file_exists($filename))
                         {$filename=str_replace("_"," ",$filename);}
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $cforums=fread($fd,filesize($filename));
                    fclose($fd);
               }
               unlink ($filename);
               $filename=str_replace("_"," ",$filename);
               @unlink($filename);
               echo 'updated category: ',$cname,'<br>';
               WriteCatInfo($file);

          }
     }
}
closedir($handle);
echo '<br>';

$handle=opendir($dbpath."/cats");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.html" && $file != "cats") {
          if (strstr($file,"_f_n") || strstr($file," f n")){
               $filebase=substr($file,0,strlen($file)-4);
               $filebase1=str_replace(" ","_",$filebase);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_n';
               $filename2=$dbpath.'/cats/'.$filebase.' f n';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               $fd=fopen($filename,"r");
               $fname=stripslashes(fread($fd,filesize($filename)));
               fclose($fd);
               $fname=textparse($fname);
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_d';
               $filename2=$dbpath.'/cats/'.$filebase.' f d';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $fdescription=stripslashes(fread($fd,filesize($filename)));
                    fclose($fd);
               }else{
                    $fdescription="";
               }
               $fdescription=textparse($fdescription);
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_lock';
               $filename2=$dbpath.'/cats/'.$filebase.' f lock';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $flock=fread($fd,filesize($filename));
                    fclose($fd);
               }else{
                    $flock="";
               }
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_lp';
               $filename2=$dbpath.'/cats/'.$filebase.' f lp';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $flastpost=fread($fd,filesize($filename));
                    fclose($fd);
               }else{
                    $flastpost="";
               }
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_lpa';
               $filename2=$dbpath.'/cats/'.$filebase.' f lpa';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $flastauthor=fread($fd,filesize($filename));
                    fclose($fd);
               }else{
                    $flastauthor="";
               }
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_lpu';
               $filename2=$dbpath.'/cats/'.$filebase.' f lpu';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $flasturl=fread($fd,filesize($filename));
                    fclose($fd);
               }else{
                    $flasturl="";
               }
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/posts/'.$filebase1;
               $filename2=$dbpath.'/posts/'.$filebase;
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $flastpostnumber=fread($fd,filesize($filename));
                    fclose($fd);
               }else{
                    $flastpostnumber="";
               }
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_p';
               $filename2=$dbpath.'/cats/'.$filebase.' f p';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $freplies=fread($fd,filesize($filename));
                    fclose($fd);
               }else{
                    $freplies="";
               }
               @unlink ($filename1);
               @unlink ($filename2);

               $filename1=$dbpath.'/cats/'.$filebase1.'_f_t';
               $filename2=$dbpath.'/cats/'.$filebase.' f t';
               $filename=$filename1;
               if (!file_exists($filename)){
                    $filename=$filename2;
               }
               if (file_exists($filename)){
                    $fd=fopen($filename,"r");
                    $ftopics=fread($fd,filesize($filename));
                    fclose($fd);
               }else{
                    $ftopics="";
               }
               @unlink ($filename1);
               @unlink ($filename2);

               $filename=$dbpath.'/cats/'.$filebase1.'_f_us';
               $users=@file($filename);
               $counter=count($users);
               for ($i=0;$i<$counter;$i++){
                    $users[$i]=trim($users[$i]);
               }
               $fusers="|".@implode("|",$users);
               if ($fusers=="|"){
                    $fusers="|all";
               }
/*               $arrusers=explode("|",$fusers);
               $countarrusers=count($arrusers);
               for ($i=0;$i<$countarrusers;$i++){
                    echo $arrusers[$i]."<br>";
               }
*/
               @unlink ($filename);

               $filename=$dbpath."/cats/".$filebase1."_read";
               $visitors=@file($filename);
               $counter=count($visitors);
               for ($i=0;$i<$counter;$i++){
                    $visitors[$i]=trim($visitors[$i]);
               }
               $fvisitors="|".@implode("|",$visitors);
               $fvisitors=str_replace("0|","",$fvisitors);
               @unlink ($filename);

               if (strstr($filebase,"_")){
                    $cnum=strtok($filebase,"_");
                    $fnum=strtok("_");
               }else{
                    $cnum=strtok($filebase," ");
                    $fnum=strtok(" ");
               }

               if ($fname=="delplz"){
               }else{
                  WriteForumInfo($cnum,$fnum);
                  echo 'updated forum &quot;',$fname,'&quot;<br>';
               }
               @unlink($dbpath."/cats/$cnum"."_$fnum"."_f_p_2");
               @unlink($dbpath."/cats/$cnum"."_$fnum"."_f_t_2");
               @unlink($dbpath."/cats/$cnum"." $fnum"." f p 2");
               @unlink($dbpath."/cats/$cnum"." $fnum"." f t 2");
          }
     }
}
closedir($handle);

?>
