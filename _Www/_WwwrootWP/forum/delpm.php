<?php
/***************************************************************************
 *                            delpm.php
 *                            -------------------
 *
 *             see docs/copyright.txt for more details
 *
 *		Last modified 7 October 2003
 ***************************************************************************/


require('header.php');
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");

$username=$HTTP_COOKIE_VARS[$cookieid];
if($username=="Anonimus"){
	echo '<meta http-equiv="Refresh" content="0; URL=index.php">';
	

ob_end_flush();
	exit();
}
$permit=CheckLoginStatus($username,"1","0");
if ($permit=="1") {
     $language=SetLanguage($username);
     include("$temppath/pb/language/lang_$language.php");
     $id=$HTTP_GET_VARS['id'];
     $a=$HTTP_GET_VARS['a'];
     $filename = "$dbpath/pm/$a"."_$id"."_c";
     if (!file_exists($filename)){
	 	$filename=str_replace("_"," ",$filename);
	}
     $fd = fopen ($filename, "r");
     $tease = fread ($fd, filesize ($filename));
     fclose ($fd);
     if ($tease=="delplz") {
          ErrorMessage($PMAlreadyDeleted,$username);
     } else {
          $des="delplz";
          $fp = fopen($dbpath."/pm/".$a."_".$id."_c", "w");
          fputs($fp, $des);
          fclose($fp);

          $filename = $dbpath."/pm/".$a."_tot";
          if (!file_exists($filename)){
               $filename=str_replace("_"," ",$filename);
		}
          $fd = fopen ($filename, "r");
          $num = fread ($fd, filesize ($filename));
          fclose ($fd);
          $num--;
          $fp = fopen("$dbpath/pm/$a"."_tot", "w");
          fputs($fp, $num);
          fclose($fp);
          echo "<meta http-equiv=\"Refresh\" content=\"0; URL=pm.php\">";
     }
}
ob_end_flush();
?>
