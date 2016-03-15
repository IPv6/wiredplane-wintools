<?php
	include "_util.php";
	$sended=0;
	$logline="";
	$src=_var('src');
	$vendor=_var('vendor');
	$who=_var('who');
	
	if(stristr($who,"comuse")!=false || stristr($who,"Mapi2Pop3")!=false || stristr($who,"ScreenCannon")!=false || stristr($who,"WPBackup")!=false || stristr($who,"WireNote")!=false){
		Header("Location: https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-7&linkid=".$src);
		$logline="Order page: ComUse-regnow";
		$sended=1;
	}
	
	if(stristr($who,"donate")!=false || stristr($who,"CloneEliminator")!=false || stristr($who,"CapsLord")!=false || stristr($who,"KaleidosWallpaper")!=false){
		Header("Location: https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&c=USD&t=Donation&lnk=http://www.wiredplane.com/thankyou.shtml&linkid=".$src);
		$logline="Order page: Donate-swreg";
		$sended=1;
	}
	
	if(stristr($who,"WireKeys")!=false){
		if($vendor=="1" || $vendor=="regnow"){
			Header("Location: https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-2&linkid=".$src);
			$logline="Order page: WireKeys-regnow";
			$sended=1;
		}else{
			Header("Location: https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WK&v=0&d=0&q=1&c=USD&t=Single%20license%20for%20up%20to%203%20computers&lnk=http://www.wiredplane.com/thankyou.shtml&linkid=".$src);
			$logline="Order page: WireKeys-swreg";
			$sended=1;
		}
	}
	if(stristr($who,"WireChanger")!=false){
		if($vendor=="1" || $vendor=="regnow"){
			Header("Location: https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-1&linkid=".$src);
			$logline="Order page: WireChanger-regnow";
			$sended=1;
		}else{
			Header("Location: https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WC&v=0&d=0&q=1&c=USD&t=Single%20license%20for%20up%20to%203%20computers&lnk=http://www.wiredplane.com/thankyou.shtml&linkid=".$src);
			$logline="Order page: WireChanger-swreg";
			$sended=1;
		}
	}
	if($sended==0){
		$logline="order page";
		Header("Location: http://www.wiredplane.com/order.shtml");
	}
	// Log file
	/*
	$fileName="vendors_".$who."_".date("_F_Y", time()).".log";
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
		$output=$output."\tHOST=".$host.";\tIP=".$ip."\tSrc=".$src."\n";
		if(strstr($host,"msnbot.msn.com")==FALSE && strstr($host,"googlebot.com")==FALSE
			&& strstr($host,"looksmart.com")==FALSE){
			fwrite($openFile3,$output);
		}
		fclose($openFile3);
	}
	*/
?>
