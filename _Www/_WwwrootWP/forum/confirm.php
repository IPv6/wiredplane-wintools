<?php
/***************************************************************************
 *                            confirm.php  - PBLang
 *                            -------------------
 *              see the file db/copyright.txt for more details
 *
 *		last modified 7 October 2003
***************************************************************************/


require ('header.php');
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include($temppath."/pb/language/lang_".$language.".php");

$username=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($username,"0","1");

if ($loggedin=="1") {
     writeheader($newestm,0,0,0,$username,"1",$VConfirmation);
     ErrorMessage($AlreadyLoggedIn,$username);
     exit;
} else {
     if ($allowreg!="1") {
          writeheader($newestm,0,0,0,$username,"0",$VConfirmation);
          ErrorMessage($NoNewUsers,$username);
          exit;
     //Here begins the action!
     } else {
          $regcode=$HTTP_GET_VARS['code'];
          $user=$HTTP_GET_VARS['user'];
          writeheader($newestm,0,0,0,$username,"0",$VConfirmation);
          $filename=$dbpath."/members/pending/".$regcode;
          if (!file_exists($filename)){
               ErrorMessage($VWrongConfirmationCode,$username);
               exit;
          } else {
               include($filename);
               if ($username==$user){
                    copy($filename,$dbpath."/members/".$user);
                    @unlink ($filename);

                    $filename = $dbpath."/mems";
                    $fd = fopen ($filename, "r");
                    $n = fread ($fd, filesize ($filename));
                    fclose ($fd);
                    $n++;
                    $fp = fopen($dbpath."/mems", "w");
                    fputs($fp, $n);
                    fclose($fp);

                    $zero="0";
                    $fp = fopen($dbpath."/pm/".$user, "w");
                    fputs($fp, $zero);
                    fclose($fp);
                    $fp = fopen($dbpath."/pm/".$user."_tot", "w");
                    fputs($fp, $zero);
                    fclose($fp);

                    $fp = fopen($dbpath."/settings/newestm", "w");
                    fputs($fp, $user);
                    fclose($fp);

                    $fp = fopen($dbpath."/memss/".$user, "w");
                    fputs($fp, $zero);
                    fclose($fp);

                    regdone($user,0,$userlang);
               } else {
                    writeheader($newestm,0,0,0,$username,"0",$VConfirmation);
                    ErrorMessage($VConfirmationError,$username);
               }
          }
     }
}
writefooter($newestm);


ob_end_flush();
?>
