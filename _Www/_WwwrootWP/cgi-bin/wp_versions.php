<?php
include "_util.php";

if(_var("redirect")=="forum"){
	$str_low = strtolower(_var("q_app"));	
	Header("Location: http://www.gamerotor.com/forum");
	exit();
}

$q_app=_var("q_app");
$q_from=_var("q_from");
$q_id=_var("q_id");
$q_im=_var("q_im");
$q_iy=_var("q_iy");
$q_adddata=_var("q_adddata");

clearstatcache();
print "File: $q_app<br>\r\n";
$realFile=strtolower("../download/$q_app.zip");
$realFileVer=strtolower("../download/$q_app.xml");
$realFileNfo=strtolower("../download/$q_app.nfo");
$openFile2 = fopen($realFileVer,"r");
$xmlData = fread($openFile2, filesize($realFileVer));
fclose($openFile2);
//print $xmlData;
// main file info
$realFileDate=ExtractString($xmlData,"<program_release_day>","</program_release_day>").".".ExtractString($xmlData,"<program_release_month>","</program_release_month>").".".ExtractString($xmlData,"<program_release_year>","</program_release_year>");//date("d.m.y", filemtime($realFile));
print "Date: $realFileDate<br>\r\n";
$realFileSize=filesize($realFile);
print "Size: $realFileSize<br>\r\n";
// version  file info
$appver = ExtractString($xmlData,"<program_version>","</program_version>");
print "Version: $appver<br>\r\n";
// version what`s new info
$openFile3 = fopen($realFileNfo,"r");
$appnfo = fread($openFile3, filesize($realFileNfo));
fclose($openFile3);
print "WhatsNew: $appnfo<br>\r\n";
/*
// Log file
$output=$output."INSTALLED=".$q_id.".".$q_im.".".$q_iy.";";
$output=$output."ADDINFO='".$q_adddata."';";
WriteLog($q_app.$q_from."_versions",$output);
*/
?>