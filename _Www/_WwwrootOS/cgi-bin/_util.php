<?php

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

function writeDataInFile($fileData, $fileName)
{
	$fileName=$fileName;
	if(!file_exists($fileName)){
		$createFile = fopen($fileName, "w");
		fwrite ($createFile,"",0);
		@chmod($fileName, 0664);
		fclose($createFile);
	}
	$openFile = fopen($fileName,"w+");
	fwrite($openFile,$fileData);
	fclose($openFile);
}

function translateHtml($content)
{
	$content = preg_replace("/(\<a href=\\\\\")(.*)(\\\\\">)(.*)(\<\/a\>)/U","\[a href=\\2\]\\4\[/a\]", $content);
	$content = preg_replace("/(\<b\>)(.*)(\<\/b\>)/U","[b]\\2[/b]", $content);
	$content = preg_replace("/(\<i\>)(.*)(\<\/i\>)/U","[i]\\2[/i]", $content);		
	$content = nl2br($content);
	$content = ereg_replace("\n","",$content);
	$content = ereg_replace("\(","&#040;",$content);
	$content = ereg_replace("\)","&#041;",$content);
	$content = ereg_replace("\\$","&#036;",$content);
	$content = stripslashes($content);
	$content = preg_replace("/(\[a href=)(.*)(\])(.*)(\[\/a\])/U","<a href=\\2>\\4</a>", $content);
	$content = preg_replace("/(\[b\])(.*)(\[\/b\])/U","<b>\\2</b>", $content);
	$content = preg_replace("/(\[i\])(.*)(\[\/i\])/U","<i>\\2</i>", $content);
	return $content;
}

function appendDataInFile($fileName, $fileData, $reswho)
{
	if(!file_exists($fileName)){
		$createFile = fopen($fileName, "w");
		fwrite ($createFile,"",0);
		fclose($createFile);
	}
	chmod($fileName, 0664);
	$openFile = fopen($fileName,"a");
	$ip = getenv("REMOTE_ADDR");
	$currHour=date("H",time());
	$currDate=date("dS F, Y", time());
	$currTime=date("$currHour:i:s", time());
	$output="";
	$output=$output."[".$currDate." ".$currTime."] ".$fileData;
	$output=$output."\twho=".$reswho.";\t";
	$output=$output."\tIP=".$ip."\n";
	fwrite($openFile,$output);
	fclose($openFile);
}


function getOldestImage($directory) { 
    if ($handle = opendir($directory)) { 
        while (false !== ($file = readdir($handle))) { 
            if (is_file($directory.$file) && stristr($directory.$file,".jpg")!=false) { // add only files to the array (ver. 1.01) 
                $files[] = $file; 
            } 
        } 
            foreach ($files as $val) { 
                if (is_file($directory.$val)) { 
                    $file_date[$val] = filemtime($directory.$val); 
                } 
            } 
    } 
    closedir($handle); 
    asort($file_date, SORT_NUMERIC); 
    reset($file_date); 
    $oldest = key($file_date); 
    return $oldest; 
} 
?>