<?php
	include "../cgi-bin/_util.php";
	$logline="";
	$from=_var("from");
	$e_to=_var("e_to");
	$e_from=_var("e_from");
	$e_subject=_var("e_subject");
	$e_message=_var("e_message");
	$formsent = 0;
	$error_text="";
	{
		$e_to = ereg_replace("\r","",$e_to);
		$e_to = ereg_replace("\t","",$e_to);
		$e_to = ereg_replace("\n",", ",$e_to);
		$logline="New tell: From=".$from."; e_from=".$e_from."; e_to=".$e_to."; e_subject=".$e_subject."; e_message=".$e_message;
		if(stristr($logline,"OK+")!=false || stristr($logline,"Received:")!=false || stristr($e_to,",")!=false || stristr($e_to,"\n")!=false || stristr($e_subject,"\n")!=false || stristr($from,"\n")!=false){
			$error_text="Invalid%20parameters";
		}
		if(stristr($logline,"href")!=false || stristr($logline,"<a")!=false || stristr($logline,"<br")!=false){
			$error_text="No%20HTML%20please!";
		}
		if($error_text==""){
			// Log file
			$fileName="tellfr".date("_F_Y", time()).".log";
			if(!file_exists($fileName)){
				$createFile = fopen($fileName, "w");
				fwrite ($createFile,"",0);
				@chmod($fileName, 0666);
				fclose($createFile);
			}
			if(file_exists($fileName) && strstr($host,"googlebot.com")==""){
				$e_message.="\r\n\r\nVisit http://www.wiredlpane.com/ for details!";
				$ip = getenv("REMOTE_ADDR");
				$host = "?";//getHostByAddr($REMOTE_ADDR);
				$openFile3 = fopen($fileName,"a");
				$currHour=date("H",time());
				$currDate=date("dS F, Y", time());
				$currTime=date("$currHour:i:s", time());
				$output="";
				$output=$output."[".$currDate." ".$currTime."] ".$logline;
				$output=$output."\tHOST=".$host.";\tIP=".$ip."\n";
				fwrite($openFile3,$output);
				fclose($openFile3);
				@mail($e_to, $e_subject, $e_message, "From: $from <$e_from>\r\nContent-Type: text/plain; charset=windows-1251\r\nContent-Transfer-Encoding: 8bit");
				@mail("support@wiredplane.com", "New tell-a-friend", $e_from."->".$e_to."\r\n".$e_subject."\r\n".$e_message, "From: admin@wiredplane.com\r\nContent-Type: text/plain; charset=windows-1251\r\nContent-Transfer-Encoding: 8bit");
				$formsent = 1;
			}
		}
	}
	if($formsent != 0){
		Header("Location: /commons/about.php?print_message=Your%20message%20has%20been%20sent!"); 
	}else{
		Header("Location: /commons/about.php?print_message=Your%20message%20has%20not%20been%20sent!%20Error:".$error_text); 
	}
?>
