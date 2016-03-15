<?php
include "_util.php";
 // http://www.wiredplane.com/cgi-bin/email2rss_proxy.php?user=wplabs.rss2@gmail.com&psw=jncnfymnt&host=pop.gmail.com:995/pop3/ssl/novalidate-cert
 // http://www.wiredplane.com/cgi-bin/email2rss_proxy.php?user=wplabs.rss2@gmail.com&psw=jncnfymnt
 ///////////////////////////////////////////////////////////////////////////////

 //  email2rss_proxy.php is a script  that reads email from a  mail server and outputs
 //  it in RSS format that can be displayed in a browser (as XML, using included
 //  XSL/CSS) or RSS aggregator or as WML for use on WAP devices (mobile phones/
 //  PDAs). The output has been tested and found to be valid XSL/CSS, valid XML/
 //  RSS and valid WML.
 // Параметры:
	// "host"-хост для POP3. Для gmailа (user вида ...@gmail.com можно не указывать)
	// "user"-имя юзера
	// "psw"-пароль юзера

///////////////////////////////////////////////////////////////////////////////
if(_var("info")==1){
	phpinfo();
	exit();
}
email2rss_proxy (_var("host"),_var("user"),_var("psw"));
exit();

///////////////////////////////////////////////////////////////////////////////
function email2rss_proxy($server,$user,$pass)
{
	// Defaults ///////////////////////////////////////////////////////////////////
	// Configuration //////////////////////////////////////////////////////////////
	//define ("PROTOCOL","{pop.gmail.com:995/pop3/ssl/novalidate-cert}INBOX");  //"143/NOTLS" // Mail protocol (POP3: 110/POP3)
	define ("PROTOCOL","110/POP3");  //"143/NOTLS" // Mail protocol (POP3: 110/POP3)
	define ("MBXNAME","INBOX");       // Mailbox to use
	define ("GMTOFFSET","+3");        // GMT offset in hours
	define ("NUMMSGSRSS","20");       // Number of messages in RSS output
	define ("MSGLENRSS","256");       // Length of messages in RSS output
	define ("MSGLENRSSFULL","3000"); // Length of messages in RSS output
	define ("RSS_SEND_FULL",1);

	// Get variables ///////////////////////////////////////////////////////////
	$cache_file=realpath(".")."/email2rss_proxy_".$user.".log";
	$getmsg=_var("msg");
	$addDebug=_var("debug");
	$forceGmail=_var("gmail");
	if (strstr($getmsg,"\.")){
		list($getmsg,$devnull)=split("\.",$getmsg,2);
	}

	// GMT offset //////////////////////////////////////////////////////////////
	$currdate=time()+(3600*GMTOFFSET)-date("Z")+3600*(1-date("I"));
	$tzone=sprintf('%s%02d',(GMTOFFSET<0)?'-':'+',abs(GMTOFFSET));
	$rdate=date("D, d M Y H:i:s",$currdate)." ".$tzone."00";
	//$Diff = (time() - filectime("$path/$file"))/60/60/24;
	if($addDebug!="1" && file_exists($cache_file) && ((time() - filectime($cache_file))/60/60) <2 ){
		$output=file_get_contents($cache_file);
		$Diff = ((time() - filectime($cache_file))/60/60);
		$output.="<!-- rss-cache-info>Cached. Diff is ".$Diff." hours</rss-cache-info -->";
	}else
	{
		// Server detection
		$total=0;
		$sorted=0;
		if(strstr($user,"gmail")!=false || $forceGmail=="1"){
			$server="imap.gmail.com:993/imap/ssl";//"pop.gmail.com:995/pop3/ssl/novalidate-cert";
		}else if(strstr($user,":")!=false){
			$server=$server.":".PROTOCOL;
		}

		// RSS header //////////////////////////////////////////////////////////////
		$rss_encoding="UTF-8";//"Win-1251";
		$output="";
		$output.="<?xml version=\"1.0\" encoding=\"$rss_encoding\"?>\n";
		$output.="<rss version=\"2.0\">\n";
		$output.="<channel>\n";
		$output.="<title>e2rss[$user]</title>\n";
		//$output.="<title>email2rss_proxy.php '$user@$server'</title>\n";
		$output.="<link>".$_SERVER['HTTP_HOST']."</link>\n";
		$output.="<pubDate>$rdate</pubDate>\n";
		$output.="<ttl>60</ttl>\n";
		$output.="<description>$user@$server</description>\n";
		$output.="<lastBuildDate>$rdate</lastBuildDate>\n";
		
		// Open mailbox ////////////////////////////////////////////////////////////
		$bCacheContent=1;
		$errinfo="Check mailbox later!";
		$inbox=@imap_open("{".$server."}".MBXNAME,$user,$pass);
		$wasError=0;
		if(!$inbox){
			$errinfo="Error opening mailbox [".imap_last_error()."]";//. ["."{".$server."}".MBXNAME.":".$user.":".$pass."])";
			$bCacheContent=0;
			$wasError=1;
		}else{
			$sorted=imap_sort($inbox,SORTARRIVAL,1);
			$total=imap_num_msg($inbox);
		}
		$maxmsg=1+_var("maxmsg");
		if($maxmsg<3){
			$maxmsg=NUMMSGSRSS;
		}
		//$output.="<logon-info>$user:$pass, messages: $total</logon-info>\n";
		if ($total>0)
		{
			// Get messages to show ///////////////////////////////////////////////////
			if($maxmsg>=$total)
			{
				$maxmsg=$total-1;
			}
			$limit=$total-($maxmsg-1);
			if ($limit<1) {$limit=1;}
			if ($getmsg) {$total=(int)$getmsg;$limit=$total;}

			// Loop ///////////////////////////////////////////////////////////////////
			//for($i=$total-1;$i>=$limit;$i--)
			for($i=0;$i<=$maxmsg;$i++)
			{
				$headers=imap_header($inbox,$sorted[$i]);

				// Date //////////////////////////////////////////////////////////////////
				unset ($hmaildate);unset ($hrssmaildate);
				$thisdate=$headers->udate+(3600*GMTOFFSET)-date("Z")+3600*(1-date("I"));
				$hmaildate=date("d-m-y H:i",$thisdate);
				$hrssmaildate=date("D, d M Y H:i:s",$thisdate)." ".$tzone."00";

				// Subject ///////////////////////////////////////////////////////////////
				unset ($hsubject);
				//$hsubject=mb_decode_mimeheader($headers->Subject);$hsubject=eregi_replace("[^ -!#-%'-;a-~A-_0-9?-@]"," ",$hsubject);if (strlen($hsubject)>50) {$hsubject=substr($hsubject,0,47)."...";}
				$hsubject=imap_utf8($headers->Subject);

				// Author ////////////////////////////////////////////////////////////////
				unset ($hauthorname);unset ($hauthoremail);
				//$hauthoremail=mb_decode_mimeheader($headers->fromaddress);
				$hauthoremail=imap_utf8($headers->fromaddress);
				if (strstr($hauthoremail,"<")) {list($hauthorname,$devnull)=split(" <",$hauthoremail,2);}
				else {$hauthorname=$hauthoremail;}
				//$hauthorname=eregi_replace("[^ -!#-%'-;a-~A-_0-9?-@]"," ",$hauthorname);
				if (strlen($hauthorname)>25) {$hauthorname=substr($hauthorname,0,22)."...";}
				if (strstr($hauthoremail,"<")) {list ($devnull,$hauthoremail)=split("<",$hauthoremail,2);}
				if (strstr($hauthoremail,">")) {list ($hauthoremail,$devnull)=split(">",$hauthoremail,2);}
				$hauthoremail=eregi_replace("[^ -!#-%'-;a-~A-_0-9?-@]","",$hauthoremail);

				// Generate RSS output ///////////////////////////////////////////////////
				$output.="\n\n\n<item>\n";
				$guid =strtolower(eregi_replace("[^a-z]","",$hauthorname)).".";
				$guid.=md5($thisdate.strtolower(eregi_replace("[^a-z]","",$hauthoremail.$hsubject)));
				//? $hsubject = utf8_encode($hsubject);
				$output.="<title><![CDATA[$hsubject]]></title>\n";
				if ($getmsg){
					$output.="<link>http://".$_SERVER['HTTP_HOST'].$_SERVER['PHP_SELF']."</link>\n";
				}else
				{
					$output.="<link>http://".$_SERVER['HTTP_HOST'].$_SERVER['PHP_SELF']."?msg=".$sorted[$i].".$guid</link>\n";
				}
				$output.="<pubDate>$hrssmaildate</pubDate>\n";
				$output.="<guid isPermaLink=\"false\">$guid</guid>\n";
				$authorLine="$hauthorname $hauthoremail";
				//? $authorLine = utf8_encode($authorLine);
				$output.="<author><![CDATA[$authorLine]]></author>\n";
				
				// Body //////////////////////////////////////////////////////////////////
				if(0){
					$part=-1;
					$hbodypart=$hsubject;
					$type = 0; 
					$encoding = 0;
					$struckture = "";
				}else{
					unset ($hbodypart);
					$struckture = imap_fetchstructure($inbox,$sorted[$i]); 
					$part=0;
					if($headers->Size>10000){
						$part=-1;
						$hbodypart="Too big message";
						$type = 0; 
						$encoding = 0;
					}else if(isset($struckture->parts) && $struckture->type==1){
						$part=1;
						if($struckture->parts[$part]->subtype!="HTML"){
							$part=0;
						}
						$hbodypart=imap_fetchbody($inbox,$sorted[$i],$part,FT_PEEK);//|| FT_UID
						$type = $struckture->parts[$part]->type; 
						$encoding = $struckture->parts[$part]->encoding;
					}else{
						$part=-1;
						$hbodypart=imap_body($inbox,$sorted[$i],FT_PEEK);
						$type = $struckture->type; 
						$encoding = $struckture->encoding;
					}
					if ($encoding == 1)
					{
					    //$hbodypart = imap_8bit($hbodypart);
						$hbodypart = quoted_printable_decode($hbodypart);
					}
					elseif ($encoding == 2)
					{
					    $hbodypart = imap_binary($hbodypart);
					}
					elseif ($encoding == 3)
					{
					    $hbodypart = imap_base64($hbodypart);
					}
					elseif ($encoding == 4)
					{
						//$hbodypart = quoted_printable($hbodypart);
						$hbodypart = quoted_printable_decode($hbodypart);
					}else{
						$hbodypart = ($hbodypart);
					}
					$hbodypartlimit=MSGLENRSS;
					if ($getmsg){
						$hbodypartlimit=MSGLENRSSFULL;
					}
					if(RSS_SEND_FULL){
						$hbodypartlimit=MSGLENRSSFULL;
					}
					$hbodypart=substr($hbodypart,0,$hbodypartlimit);
					
					//? $hbodypart = utf8_encode($hbodypart);
					
					//$hbodypart=htmlentities($hbodypart);
					//$hbodypart=eregi_replace("[^ -!#-%'-;a-~A-_0-9?-@]"," ",$hbodypart);
					
					if($addDebug=="1"){
						//==== Debug ====
						//$output.="<debug-headers><![CDATA[".$i.": ".var_export($headers, true)."']]></debug-headers>\n";
						$output.="<debug>[".$i."] Date=".$headers->Date."; Size=".$headers->Size."; part='".$part."'; msg-type='".$type."'; msg-encoding='".$encoding."'. structure='".var_export($struckture, true)."'</debug>\n";
					}
				}
				// Generate RSS item body ///////////////////////////////////////////////////
				$output.="<description><![CDATA[$hbodypart]]></description>\n";
				$output.="</item>\n";
			}
		}else{
			$bCacheContent=0;
			if($wasError==0){
				$output.="\n\n<item><title>No new items</title><description>There are no new items</description></item>\n";
			}else{
				$output.="\n\n<item><title>No new items</title><description>Login failed: $errinfo</description></item>\n";
			}
		}
		// Close mailbox ///////////////////////////////////////////////////////////
		if($inbox){
			imap_close($inbox);
		}
		// Content /////////////////////////////////////////////////////////////////
		$output.="\n</channel>\n</rss>\n";
		if($bCacheContent){
			$cfile = @fopen($cache_file, "w+");
			flock($cfile, LOCK_EX);
	        fputs($cfile, $output);
	        flock($cfile, LOCK_UN);
	        fclose($cfile);
		}
	}
	header ("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
	header ("Last-Modified: ".$rdate);
	header ("Cache-Control: no-cache, must-revalidate");
	header ("Pragma: no-cache");
	header ("Content-Type:·text/xml;charset=$rss_encoding");
	echo $output;
}

?>
