<?php
define('IN_PB', "true");
define('IN_PBG', "true");
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once("scan.php");
include($temppath."/pb/language/lang_".$language.".php");

//backup messages into a subdir

$handle=opendir($dbpath."/cats");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.html" && $file != "cats") {
          if (!file_exists($dbpath.'/backup/cats/'.$file)){
               copy ($dbpath."/cats/".$file, $dbpath."/backup/cats/".$file);
               echo 'backing up ',$dbpath,'/cats/',$file,'<br>';
          }
     }
}
closedir($handle);

$handle=opendir($dbpath."/posts");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.html" && $file != "cats") {
          if (!file_exists($dbpath.'/backup/posts/'.$file)){
               copy ($dbpath."/posts/".$file, $dbpath."/backup/posts/".$file);
               echo 'backing up ',$dbpath,'/posts/',$file,'<br>';
          }
     }
}


?>
