<?php
	include "_util.php";
	$logline="";
	$e_to=stripslashes($_REQUEST["e_to"]);
	$e_from=stripslashes($_REQUEST["e_from"]);
	$e_subject=stripslashes($_REQUEST["e_subject"]);
	$e_message=stripslashes($_REQUEST["e_message"]);
	$e_hash=stripslashes($_REQUEST["e_hash"]);
	$hash_src="WN-CHECk-vaLID-$e_message-$e_to-$e_from-PAKA-PAKA!";
	$hash_here=md5($hash_src);
	$hash_here=strtolower($hash_here);
	if($hash_here==$e_hash)
	{
		$e_to = ereg_replace("\r","",$e_to);
		$e_to = ereg_replace("\t","",$e_to);
		$e_to = ereg_replace("\n",", ",$e_to);
		
		$logline="New email: e_from=".$e_from."; e_to=".$e_to."; e_subject=".$e_subject."; e_message=".$e_message;
		WriteLog("WireNote_mails",$logline);

		if(@mail($e_to, $e_subject, $e_message, "From: $e_from\r\nContent-Type: text/html; charset=UTF-8\r\n")){//Content-Transfer-Encoding: 8bit\r\n\r\n
			print "TRUE";
			exit();
		}
		print "FALSE-send-failed";
		exit();
	}
	$res="FALSE-wrong-md5 :$hash_here of $hash_src";
	print $res;
	exit();
?>
