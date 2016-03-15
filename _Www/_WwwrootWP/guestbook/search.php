<?php

	$sended=0;
	$searchstr="http://www.google.com/search?sourceid=opera&num=0&ie=utf-8&q=".$question;
	if($qtype!="gg"){
		$searchstr="http://www.google.com/search?sourceid=opera&num=0&ie=utf-8&as_sitesearch=wiredplane.com&as_q=".$question;
	}
	Header("Location: ".$searchstr);
?>
