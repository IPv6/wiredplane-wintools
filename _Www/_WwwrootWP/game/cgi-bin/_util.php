<?php

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

function loadGD()
{
	if(!defined('GD_LOADED')){
		define('GD_LOADED',true);
		//dl("php_gd2.dll");
	}
}

function _var($vname,$def="",$fieldname="")
{
	$res="";
	if(strstr($vname,"[]")!=false){
		$arname=trim($vname,"[]");
		if(isset($_REQUEST[$arname]) && is_array($_REQUEST[$arname])){
			$i="";
			foreach($_REQUEST[$arname] as $i){
				$res.=",".stripslashes($i)."";
			}
		}
		$res=trim($res,",");
	}else if(isset($_REQUEST[$vname])){
		$res=stripslashes($_REQUEST[$vname]);
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

function WriteLog($fileBase,$logline)
{
	//$fileBase = preg_replace(".","", $fileBase);
	$fileName=($fileBase."-".date("F_Y", time()).".log");
	//print ($fileBase."->'".$fileName."'");
	$fileName = strtolower($fileName);
	if(!file_exists($fileName)){
		$createFile = fopen($fileName, "w");
		fwrite ($createFile,"",0);
		@chmod($fileName, 0666);
		fclose($createFile);
	}
	if(file_exists($fileName)){
		$ip = getenv("REMOTE_ADDR");
		$host = getHostByAddr($ip);
		if(stristr($host,"googlebot")==false && stristr($host,"msnbot")==false ){
			$openFile3 = fopen($fileName,"a");
			$currHour=date("H",time());
			$currDate=date("dS F, Y", time());
			$currTime=date("$currHour:i:s", time());
			$output="";
			$output=$output."[".$currDate." ".$currTime."] ".$logline;
			$output=$output."\tIP=".$ip.";\thost=".$host."\n";
			fwrite($openFile3,$output);
			fclose($openFile3);
		}
	}
}

function includeFileX($fname, $norec)
{
	global $language;
	$templatename = getenv("DOCUMENT_ROOT")."/".$fname."_".$language.".inc";
	if(file_exists($templatename)){
		include($templatename);
		return true;
	}
	$templatename = getenv("DOCUMENT_ROOT")."/".$fname.".inc";
	if(file_exists($templatename)){
		include($templatename);
		return true;
	}
	if($norec!=1 && includeFileX($fname."about",1)){
		return true;
	}
	if($norec!=1){
		print "\r\n\r\n<h1> WARNING! file not found: ".$templatename."</h1>\r\n\r\n";
	}
	return false;
}

function includeFile($fname)
{
	return includeFileX($fname,0);
}

function insertVersion($fname)
{
	$vfile=getenv("DOCUMENT_ROOT")."/".("download/".$fname.".ver");
	if(file_exists($vfile)){
		$openFile = fopen($vfile,"r");
		$appnfo = fread($openFile, filesize($vfile));
		fclose($openFile);
		print "v.";
		print $appnfo;
	}else
	{
		print $vfile;
		print " not found";
	}
}

function insertSize($fname)
{
	$vfile=getenv("DOCUMENT_ROOT")."/".("download/".$fname.".zip");
	if(file_exists($vfile)){
		print sprintf("%.1f Mb",filesize($vfile)/1024/1024);
		//floor(filesize("manual.zip")/1024);
		//filesize($vfile);
	}
}

function insertDate($fname)
{
	$vfile=getenv("DOCUMENT_ROOT")."/".("download/".$fname.".zip");
	if(file_exists($vfile)){
		print date("m/d/y",filemtime($vfile));//dS M
	}else{
		$currHour=date("H",time());
		$currTime=date("$currHour:i", time());
		$currDate=date("m/d/y", time());
		print $currDate;
	}
}

function protectInput($content)
{
	$content = str_replace('"','&quot;', "".$content);
	$content = str_replace('<','&lt;', "".$content);
	$content = str_replace('>','&gt;', "".$content);
	return $content;
}

function str2UTF8($str)
{
	//$str = iconv("windows-1251", "utf-8", $str);
	$str=utf8_encode($str);
	return "!!!!!!!!!abc!!!!!!!!!!!!".$str."!!!!!!!!!abc!!!!!!!!!!!!";
}

?>