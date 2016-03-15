<?php
/***************************************************************************
 *                            sendpm.php  - PBLang
 *                            -------------------
 *
 *                  see docs/copyright.txt for more details
 *
 *			Last modified 10 Oct 2003
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("functions.php");

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1") {
     $language=SetLanguage($liusername);
     include($temppath."/pb/language/lang_".$language.".php");
     $reg=$HTTP_GET_VARS['id'];
     writeheader($newestm,0,0,0,$liusername,"1",$SendPM);
     if ($reg!="2") {
          $num=$HTTP_GET_VARS['num'];
          $to=$HTTP_GET_VARS['to'];
          $subj=stripslashes($HTTP_GET_VARS['subj']);
          $msg=stripslashes($HTTP_GET_VARS['msg']);
          $orgp=stripslashes($HTTP_GET_VARS['orig']);
          $pmstat="3";
          $fp = fopen($dbpath."/pm/".$liusername."_".$num."_pmstat", "w");
          fputs($fp, $pmstat);
          fclose($fp);
//          sendpm($to,$subj,$msg,$orgp,$liusername);
          $filename=$orgp;
          $fd=@fopen($filename,"r");
          $rmsg=@fread($fd,filesize($filename));
          @fclose($fd);
          WriteTableTop();
          echo "<form method=\"post\" action=\"sendpm.php?id=2\">";
          echo "<tr><td bgcolor=\"$headercolor\" height=\"15\" background=\"$headergif\" colspan=\"2\">";
          echo "<a href=\"index.php\">$sitetitle</a> :: <a href=\"sendpm.php\">$PrivateMessaging</a> :: $VSendMessage</td></tr>";
          echo "<tr><td class=\"subheader\" colspan=\"2\"><a href=\"pm.php\">$VInbox</a>";
          echo " :: <a href=\"sendpm.php\">$VSendPM</a></td></tr>";
          if ($rmsg!=""){
               echo "<tr><td bgcolor=\"$fmenucolor\" height=\"15\" colspan=\"2\">";
               echo "$VOriginalPM $VFrom <b>$to</b>:<br><br>$rmsg<br></td></tr>";
               echo "<tr>";
          }
          echo "<td height=\"5\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"18%\"><p>$VTo:</p></td>";
          echo "<td height=\"5\" bgcolor=\"$menucolor\" width=\"82%\"><input type=\"text\" name=\"to\" value=\"$to\"></td></tr>";
          echo "<tr bgcolor=\"$subheadercolor\" valign=\"top\"><td height=\"5\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"18%\"><p>$VSubject:</p></td>";
          echo "<td height=\"5\" bgcolor=\"$menucolor\" width=\"82%\"><input type=\"text\" name=\"subj\" value=\"$subj\"></td></tr>";
          echo "<tr bgcolor=\"$subheadercolor\" valign=\"top\"><td height=\"15\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"18%\">$VMessage:</td>";
          echo "<td height=\"15\" bgcolor=\"$menucolor\" width=\"82%\"><textarea name=\"msg\" cols=\"60\" rows=\"10\">$msg</textarea></td></tr>";
          echo "<tr bgcolor=\"$subheadercolor\" valign=\"top\"><td height=\"15\" align=\"right\" bgcolor=\"$fmenucolor\" width=\"18%\">&nbsp;</td>";
          echo "<td height=\"15\" bgcolor=\"$menucolor\" width=\"82%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\">";
          sbot(1);
     } else {
          $to=$HTTP_POST_VARS['to'];
          $msg=stripslashes($HTTP_POST_VARS['msg']);
          $subj=stripslashes($HTTP_POST_VARS['subj']);
          $pmstat="1";
          if (trim($to)=="") {
               ErrorMessage($AuthorRequired,$liusername);
          } else {
               $filet=$dbpath."/pm/".$to;
               if (file_exists($filet)) {
                    $filename = "$dbpath/pm/$to"."_tot";
                    if (!file_exists($filename))
                         {$filename=str_replace("_"," ",$filename);}
                    $fd = fopen ($filename, "r");
                    $num = fread ($fd, filesize ($filename));
                    fclose ($fd);
                    $num++;
                    $fp = fopen("$dbpath/pm/$to"."_tot", "w");
                    fputs($fp, $num);
                    fclose($fp);
                    $filename = "$dbpath/pm/$to";
                    $fd = fopen ($filename, "r");
                    $num = fread ($fd, filesize ($filename));
                    fclose ($fd);
                    $num++;
                    $fp = fopen("$dbpath/pm/$to", "w");
                    fputs($fp, $num);
                    fclose($fp);

                    setlocale(LC_TIME,$LangLocale);
                    $date = time()+($timezone*3600);
                    $fp = fopen("$dbpath/pm/$to"."_$num"."_a", "w");
                    fputs($fp, $liusername);
                    fclose($fp);
                    $fp = fopen("$dbpath/pm/$to"."_$num"."_d", "w");
                    fputs($fp, $date);
                    fclose($fp);
                    $fp = fopen("$dbpath/pm/$to"."_$num"."_c", "w");
                    fputs($fp, $msg);
                    fclose($fp);
                    $fp = fopen("$dbpath/pm/$to"."_$num"."_s", "w");
                    fputs($fp, $subj);
                    fclose($fp);
                    $fp = fopen("$dbpath/pm/$to"."_$num"."_pmstat", "w");
                    fputs($fp, $pmstat);
                    fclose($fp);
                    msg($VSent,"$MessageSent!");
                    echo "<meta http-equiv=\"Refresh\" content=\"0; URL=pm.php\">";
               } else {
                    ErrorMessage($InvalidUsername,$liusername);
               }
          }
     }
     writefooter($newestm);
}else{
     include("login.php");
}
ob_end_flush();
?>
