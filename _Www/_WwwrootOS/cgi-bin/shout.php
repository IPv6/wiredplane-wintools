<?php
	include "_util.php";
	$shout_file=_var('shout_file');
	$txtName=_var('txtName');
	$txtShout=_var('txtShout');
	$maxLen=4000;
	if($shout_file==""){
		$shout_file="shout";
	}
	$fileName="../../data/".$shout_file.".inc";
	$currHour=date("H",time());
	$currTime=date("$currHour:i", time());
	$currDate=date("dS M", time());
	$txtName=translateHtml($txtName);
	$txtShout=translateHtml($txtShout);
	// Replacing adverts
	if(stristr($txtShout,"suck")!=FALSE || stristr($txtShout," sux ")!=FALSE || stristr($txtShout,"fuck")!=FALSE || stristr($txtShout,"shit")!=FALSE || stristr($txtShout,"cheapskateszone")!=FALSE){
		ptint("Offensive shouts are prohibited. Fuck off.");
		exit;
		srand ((float) microtime() * 10000000);
		$input = array ("Very nice piece of software!", "Thank you for your wanderful software!", "You are the best!", "Just perfect!", "I just want to say: thank you!");
		$txtShout = $input[array_rand($input)];
	}
	$res="error";
	if($txtName!="" && $txtShout!=""){
		$newMsg="<!-- new shout -->\n";
		$newMsg=$newMsg."<table border=0 cellspacing=0 cellpadding=0 width=100%>";
		$newMsg=$newMsg."<tr><td bgcolor=#000000 height=1%></td></tr>";
		$newMsg=$newMsg."<tr><td bgcolor=#DDDDDD align=center><span style='font-size: 8pt;'>[".$currDate."]&nbsp;<b>".$txtName."</b></span></td></tr><td align=left><span style='font-size: 8pt;'>".$txtShout."</span></td></tr>";
		$newMsg=$newMsg."</table>\n";
		$openFile = fopen($fileName,"r");
		$newMsg = $newMsg.fread($openFile, filesize ($fileName));
		fclose($openFile);
		if(strlen($newMsg)>$maxLen){
			$pos = strpos($newMsg, "<!-- new shout -->", $maxLen);
			if (is_integer($pos)){
				$newMsg=substr($newMsg,0,$pos);
			}
		}
		writeDataInFile($newMsg,$fileName);
		@mail("support@wiredplane.com", "New Shout in ".$fileName, $txtName.": ".$txtShout."; Additionally: user-agent='".getenv("HTTP_USER_AGENT")."'", "Content-Type: text/plain; charset=windows-1251\nContent-Transfer-Encoding: 8bit");
		$res="shoutok";
	}
	$referer=getenv("HTTP_REFERER");
	if(stristr($referer,"?")==FALSE){
		$newLocation=$referer."?result=".$res;
	}else{
		$newLocation=$referer."&result=".$res;
	}
	Header("Location: ".$newLocation);
	exit;
?>
