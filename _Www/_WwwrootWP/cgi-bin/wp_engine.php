<?php
	include "_util.php";
	$who=_var("who");
	$target=_var("target");
	$what="";
	$sended=0;
	$dateDsc=date("dS-F-Y", time());
	if(stristr($target,"web")!=false){
		$what=urldecode(_var("url"));
		Header("Location: ".$what);
		$sended=1;
	}
	if(stristr($target,"search")!=false){
		$what=_var("text");
		Header("Location: http://www.google.com/custom?domains=http%3A%2F%2Fwww.wiredplane.com&client=pub-5186671154037974&forid=1&q=".$what);
		$what=urldecode($what);
		$sended=1;
	}
	if(stristr($who,"WireNote")!=false){
		if(stristr($target,"donation")!=false){
			Header("Location: http://www.wiredplane.com/en/wpt-wirenote/donate.php");
			$sended=1;
		}
		if(stristr($target,"donation-ru")!=false){
			Header("Location: http://www.wiredplane.com/ru/wpt-wirenote/donate.php");
			$sended=1;
		}
	}
	if(stristr($target,"donation")!=false && $sended==0){
		//Header("Location: https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-5&linkid=inprog");
		Header("Location: https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&c=USD&t=Donation&lnk=http://www.wiredplane.com/commons/thankyou.php&linkid=inprog");
		$sended=1;
	}
	if(stristr($target,"donation-ru")!=false && $sended==0){
		//Header("Location: https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-5&linkid=inprog");
		Header("Location: https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&c=USD&t=Donation&lnk=http://www.wiredplane.com/commons/thankyou.php&linkid=inprog");
		$sended=1;
	}
	if(stristr($who,"WJazikator")!=false){
		if(stristr($target,"vendor")!=false || stristr($target,"vendor-ru")!=false){
			$what="Rupay";
			//Header("Location: http://www.regnow.com/softsell/nph-softsell.cgi?item=10257-3&linkid=inprog");
			Header("Location: http://www.rupay.com/rupay/pay/index.php?pay_id=3884&sum_pol=10&name_service=Jazikator&order_id=WJ-ORDER-APP-".$dateDsc."&success_url=http://www.wiredplane.com/commons/thankyou.php");
			$sended=1;
		}
	}
	if(stristr($who,"WireKeys")!=false){
		if(stristr($target,"plugin")!=false){
			Header("Location: http://www.wiredplane.com/en/wirekeys/wkplugins.php");
			$sended=1;
		}	
		if(stristr($target,"macro")!=false){
			Header("Location: http://www.wiredplane.com/en/wirekeys/wkmacros.php");
			$sended=1;
		}
		if(stristr($target,"vendor")!=false){
			$what="Swreg";
			Header("Location: https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WK&v=0&d=0&q=1&c=USD&t=Single%20license%20for%20up%20to%203%20computers&lnk=http://www.wiredplane.com/commons/thankyou.php&linkid=site");
			//$what="RegNow";
			//Header("Location: https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-2&linkid=inprog");
			$sended=1;
		}
		if(stristr($target,"vendor-ru")!=false){
			$what="softkey";
			Header("Location: http://www.softkey.ru/catalog/basket.php?prodid=15658&quantity=1&clear=Y");
			$sended=1;
		}
		if(stristr($target,"board")!=false){
			$what="Forum";
			Header("Location: http://www.gamerotor.com/forum");
			$sended=1;
		}
	}
	if(stristr($who,"WireChanger")!=false || stristr($who,"PussyChanger")!=false || stristr($who,"AnimeChanger")!=false){
		if(stristr($target,"vendor")!=false){
			$what="Swreg";
			Header("Location: https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WC&v=0&d=0&q=1&c=USD&t=Single%20license%20for%20up%20to%203%20computers&lnk=http://www.wiredplane.com/commons/thankyou.php&linkid=site");
			//$what="RegNow";
			//Header("Location: https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-1&linkid=inprog");
			$sended=1;
		}
		if(stristr($target,"vendor-ru")!=false){
			$what="softkey";
			//Header("Location: http://www.rupay.com/rupay/pay/index.php?pay_id=3884&sum_pol=7&name_service=WireChanger&order_id=WC-ORDER-APP-".$dateDsc."&success_url=http://www.wiredplane.com/thankyou");
			Header("Location: http://www.softkey.ru/catalog/basket.php?prodid=5729&quantity=1&clear=Y");
			$sended=1;
		}
		if(stristr($target,"wcskins")!=false){
			$what="Skins";
			Header("Location: http://www.wiredplane.com/en/wirechanger/wcskins.php");
			$sended=1;
		}
		if(stristr($target,"wallpapers")!=false){
			Header("Location: http://www.wiredplane.com/en/wirechanger/wallpapers.php");
			$sended=1;
		}
		if(stristr($target,"wcwidgets")!=false){
			$what="Widgets";
			Header("Location: http://www.wiredplane.com/en/wirechanger/widgets.php");
			$sended=1;
		}
		if(stristr($target,"wcclocks")!=false){
			$what="Clocks";
			Header("Location: http://www.wiredplane.com/en/wirechanger/wcclocks.php");
			$sended=1;
		}
		if(stristr($target,"upload")!=false){
			$what="Unpload";
			Header("Location: http://www.wiredplane.com/guestbook/thumbview.php?action=upload_form&who=".$who);
			$sended=1;
		}
		if(stristr($target,"wcexchange")!=false){
			$what="Exchange";
			// Особый случай!!!
			print("http://outer-station.m6.net/cgi-bin/thumbview.php");
			$sended=1;
		}
		if(stristr($target,"board")!=false){
			$what="Forum";
			Header("Location: http://www.gamerotor.com/forum");
			$sended=1;
		}
		
	}
	if(stristr($target,"tellafriend")!=false){
		Header("Location: http://www.wiredplane.com/commons/tellafriend.php");
		$sended=1;
	}
	
	// Log file
	if(strstr($host,"msnbot.msn.com")==FALSE && strstr($host,"googlebot.com")==FALSE && strstr($host,"looksmart.com")==FALSE){
		if($what!=""){
			//WriteLog($who."_".$target,$what);
		}
	}
	if($sended!=1){
		Header("Location: http://www.wiredplane.com/");
		$sended=1;
	}
?>
