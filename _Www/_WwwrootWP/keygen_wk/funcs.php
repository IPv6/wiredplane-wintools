<?php 
error_reporting  (E_ERROR | E_PARSE);
set_magic_quotes_runtime(0); // Disable magic_quotes_runtime

// Общие функции
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
	if($res==""){
		$res=$def;
	}
	$res=trim($res);
	return $res;
}

function writeDataInFile($fileData, $fileName)
{
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

function getKey($psw, $salt, $regname, $email, $fio, $appName, $fulldata,&$emailsent,$nsendeml,$printdebugs)
{
	global $emailtempl,$savekeysto,$keygenurl,$sendcopyto,$replyto;
	$newMsg="";
	$emailfile=realpath($emailtempl);//realpath(".")."/".
	if($printdebugs!=0){
		print "<br>".$emailfile;
	}
	$openFile2 = fopen($emailfile,"r");
	$newMsg = $newMsg.fread($openFile2, filesize ($emailfile));
	fclose($openFile2);
	if($newMsg == "ERROR"){
		// Ошибка!!!
		if($printdebugs!=0){
			print "Failed to loade mail template. Check config.php";
			print "<br>".$emailfile;
		}
		die();
	}
	$regname=strtolower($regname);
	$appGenpath=$keygenurl."&_Pswd=".$psw."&regname=";
	$appGenpath=$appGenpath.$salt.$regname;
	$openFile = fopen($appGenpath,"r");
	if($printdebugs!=0){
		print "<hr>Looking for ".$appGenpath;
	}
	if($openFile == FALSE){
		return "ERROR";
	}
	$contents = '';
	while (!feof($openFile)) {
  		$contents .= fread($openFile, 8192);
	}
	fclose($openFile);
	if($contents == "ERROR"){
		if($printdebugs!=0){
			// Ошибка!!!
			print "Failed to generate key. Check config.php, password and license settings at http://www.strongbit.com/keygen/main.asp";
		}
		die();
	}
	// Генерим шаблон
	$newMsg=str_replace("<%PRODUCT%>",$appName,$newMsg);
	$newMsg=str_replace("<%FIO%>",$fio,$newMsg);
	$newMsg=str_replace("<%REGNAME%>",$regname,$newMsg);
	$newMsg=str_replace("<%REGKEY%>",$contents,$newMsg);
	$emailsent=$newMsg;
	$emailtitile=$appName." registration key";
    $headers="From: ".$replyto."\r\n" ."Reply-To: ".$replyto."\r\n";
	if($nsendeml!="1"){
		if(!mail($email,$emailtitile,$newMsg,$headers)){
			$emailsent="EMAIL WERE NOT SENT TO CUSTOMER!<hr>".$emailsent;
		}else{
			$emailsent="EMAIL sent succsessfully!<hr>".$emailsent;
		}
	}else{
		$emailsent="SKIPPING REGISTRATION EMAIL...<hr>".$emailsent;
		$newMsg.="\r\nREGISTRATION EMAIL SKIPPED!";
	}
	if(strlen($sendcopyto)>0){
		mail($sendcopyto,"[SERIAL] ".$emailtitile." for ".$regname,"Created serial for ".$appName."\nRegistration name:".$regname."\nSerial number:".$contents."\n=========================\n*** Additional info: ***\n\n".$fulldata."\n\n=========================\n*** Copy of email to ".$email." ***\n\n".$newMsg,$headers);
	}
	$savekeypath=realpath($savekeysto);
	$currDate=date("dS_F_Y", time());
	$savekeypath=$savekeypath."/".$appName."_".$regname."_(".$currDate."_".rand().").key";
	$keyfile="";
	$keyfile.="App:".$appName;
	$keyfile.="\r\n";
	$keyfile.="FIO:".$fio;
	$keyfile.="\r\n";
	$keyfile.="Regname:".$regname;
	$keyfile.="\r\n";
	$keyfile.="Email:".$email;
	$keyfile.="\r\n";
	$keyfile.="GKey:".$contents;
	$keyfile.="\r\n";
	$keyfile.="Full info:\r\n".$fulldata;
	$keyfile.="\r\n";
	//$openFile3 = fopen($savekeypath,"a");
	//fwrite($openFile3,$keyfile);
	//fclose($openFile3);
	return $contents;
}
?>