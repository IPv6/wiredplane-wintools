<?php
	include "../cgi-bin/_util.php";
	
	function do_processing($backurl)
	{
		//$myhost="http://localhost/";
		$myhost="http://www.wiredplane.com/";
		$prefix="Uninstalling product";
		$action=_var("action");
		$indate=_var("indate");
		$affdat=_var("affdat");
		$opinion=_var("opinion");
		$logtype=_var("logtype");
		$who=_var("who");
		$ver=_var("ver");
		$other=_var("other");
		$features=_var("features");
		$complicated=_var("complicated");
		$email=_var("email");
		$unstable=_var("unstable");
		$language=_var("language","en");
		$language.="/";
		$fileName="uninst_".$who."_".date("_F_Y", time()).".log";
		$logline="Uninstall: ".$who."; Ver=".$ver."; INDATE=".$indate."; AFFDAT=".$affdat;
		if($action==""){
			Header("Location: ".$myhost.$language."commons/uninstall.php?Prod=".$who."&Ver=".$ver."&indate=".$indate."&affdat=".$affdat);
		}else{
			if($action=="send"){
				// Log file
				if($logtype=="order-survey"){
					$prefix="Order product surv";
					$fileName="order_surv_".$who."_".date("_F_Y", time()).".log";
					$prefix="Registration survey";
				}else if($logtype=="order-done"){
					$prefix="Order product done";
					$fileName="order-done_".$who."_".date("_F_Y", time()).".log";
					$opinion=getenv("HTTP_REFERER");
					$prefix="Registration done";
				}
			}
			if($opinion!=""){
				@mail("support@wiredplane.com", $prefix, $opinion."; Additionally: other='".$other."'; features='".$features."'; complicated='".$complicated."'; unstable='".$unstable."'; later='".$later."'; email='".$email."'; user-agent='".getenv("HTTP_USER_AGENT")."'; ".$who, "Content-Type: text/plain; charset=windows-1251\nContent-Transfer-Encoding: 8bit");
			}
			if($action=="send"){
				Header("Location: ".$myhost.$language."commons/about.php?print_message=Thank%20you%20for%20your%20message!");
			}else{
				Header("Location: ".$myhost.$language.$backurl);
			}
		}
		/*if(!file_exists($fileName)){
			$createFile = fopen($fileName, "w");
			fwrite ($createFile,"",0);
			@chmod($fileName, 0666);
			fclose($createFile);
		}
		if(file_exists($fileName) && strstr($host,"googlebot.com")==""){
			$ip = getenv("REMOTE_ADDR");
			$host = getHostByAddr($REMOTE_ADDR);
			$openFile3 = fopen($fileName,"a");
			$currHour=date("H",time());
			$currDate=date("dS F, Y", time());
			$currTime=date("$currHour:i:s", time());
			$output="";
			$output=$output."[".$currDate." ".$currTime."] ".$logline;
			$output=$output."\tHOST=".$host.";\tIP=".$ip."\tuser-agent=".getenv("HTTP_USER_AGENT")."\n";
			fwrite($openFile3,$output);
			fclose($openFile3);
		}*/
	}
	
	do_processing("/commons/uninstall.php");
?>
