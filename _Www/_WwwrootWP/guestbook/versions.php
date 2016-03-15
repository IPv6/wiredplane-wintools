<?php
function _var($vname,$def="",$fieldname="")
{
	$res="";
	if(strstr($vname,"[]")!=false){
		$arname=trim($vname,"[]");
		if(isset($_REQUEST[$arname]) && is_array($_REQUEST[$arname])){
			$i="";
			foreach($_REQUEST[$arname] as $i){
				$res.=",".addslashes($i)."";
			}
		}
		$res=trim($res,",");
	}else if(isset($_REQUEST[$vname])){
		$res=addslashes($_REQUEST[$vname]);
	}
	if(strstr($def,"NOT_EMPTY")!=false && $res==""){
		print("Please, fill in '".$fieldname."' first");
		exit();
	}
	if($res==""){
		$res=$def;
	}
	$res=trim($res);
	return $res;
}

if(_var("redirect")==1){
	if(_var("q_app")=="wirekeys"){
		Header("Location: http://www.wiredplane.com/forum/post.php?cat=2&fid=1&pid=76&page=1&order=fl");
	}else if(_var("q_app")=="wirenote"){
		Header("Location: http://www.wiredplane.com/forum/board.php?cat=2&fid=3&s=s");
	}else if(_var("q_app")=="wirechanger"){
		Header("Location: http://www.wiredplane.com/forum/post.php?cat=2&fid=2&pid=35&page=1&order=fl");
	}else{
		Header("Location: http://www.wiredplane.com/forum/index.php");
	}
	exit();
}

function ExtractString($str, $start, $end)
{
   $str_low = strtolower($str);
   $pos_start = strpos($str_low, $start);
   $pos_end = strpos($str_low, $end, ($pos_start + strlen($start)));
   if ( ($pos_start !== false) && ($pos_end !== false) )
   {
       $pos1 = $pos_start + strlen($start);
       $pos2 = $pos_end - $pos1;
       return substr($str, $pos1, $pos2);
   }else{
   		return "NULL";
   }
}

clearstatcache();
$q_app=_var("q_app");
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

// Log file
$currHour=date("H",time());
$currDate=date("dS F, Y", time());
$currTime=date("$currHour:i:s", time());
$fileName="versions_".$q_app."_".date("_F_Y", time()).".log";
if(!file_exists($fileName)){
	$createFile = fopen($fileName, "w");
	fwrite ($createFile,"",0);
	@chmod($fileName, 0666);
	fclose($createFile);
}
$ip = getenv("REMOTE_ADDR");
$host = getHostByAddr($REMOTE_ADDR);
$openFile3 = fopen($fileName,"a");
$output="";//fread($openFile3, filesize($fileName));
$output=$output."[".$currDate." ".$currTime."] ".$q_app."\t".$q_from.";";
$output=$output." ADDINFO='".$q_adddata."';";
$output=$output."\tINSTALLED=".$q_id.".".$q_im.".".$q_iy.";";
$output=$output."\tHOST=".$host.";\tIP=".$ip."\n";
fwrite($openFile3,$output);
fclose($openFile3);
?>