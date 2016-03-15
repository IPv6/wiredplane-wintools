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

	$logline="";
	{
		$artname=_var("artname");
		
		$reqDescription="";
		$reqDescription.="First name:\n"._var('FNAME')."\r\n";
		$reqDescription.="Last name:\n"._var('LNAME')."\r\n";
		$reqDescription.="Company:\n"._var('COMPANY')."\r\n";
		$reqDescription.="Phone:\n"._var('PHONE')."\r\n";
		$reqDescription.="E-mail:\n"._var('EMAIL')."\r\n";
		$reqDescription.="Confirm E-mail:\n"._var('CONFIRM_EMAIL')."\r\n";
		$reqDescription.="Address-1:\n"._var('ADD1')."\r\n";
		$reqDescription.="Address-2:\n"._var('ADD2')."\r\n";
		$reqDescription.="City:\n"._var('CITY')."\r\n";
		$reqDescription.="Zip:\n"._var('ZIP')."\r\n";
		$reqDescription.="State:\n"._var('state')."\r\n";
		$reqDescription.="Country:\n"._var('country')."\r\n";
		$reqDescription.="Picture:\n"._var('PICTURE_NAME')."\r\n";
		$reqDescription.="Comments:\n"._var('COMMENTS')."\r\n";
		
		$logline="New request for art '".$artname."':".$reqDescription;
		// Log file
		$fileName="artchanger".date("_F_Y", time()).".log";
		if(!file_exists($fileName)){
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
			$output=$output."\tHOST=".$host.";\tIP=".$ip."\n";
			fwrite($openFile3,$output);
			fclose($openFile3);
		}
		@mail("scren-art@elefun-desktops.com", "[SCREENART] New request for art", $reqDescription, "From: $FNAME $LNAME <$EMAIL>\r\nContent-Type: text/plain; charset=windows-1251\r\nContent-Transfer-Encoding: 8bit");
		@mail("contact@elefun-desktops.com", "[SCREENART] New request for art", $reqDescription, "From: $FNAME $LNAME <$EMAIL>\r\nContent-Type: text/plain; charset=windows-1251\r\nContent-Transfer-Encoding: 8bit");
		@mail("support@wiredplane.com", "[SCREENART] New request for art", $reqDescription, "From: $FNAME $LNAME <$EMAIL>\r\nContent-Type: text/plain; charset=windows-1251\r\nContent-Transfer-Encoding: 8bit");
	}
	Header("Location: http://www.elefun-desktops.com/");
?>

