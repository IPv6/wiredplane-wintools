<?php
	include("_util.php");
	//header("Content-type: text/html; charset=UTF-8");
	//ob_start("str2UTF8");
	print '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">';
	function DoWork($type, $languageIn, $insType){
		global $language;
		global $section;
		global $languageNumber;
		if($languageIn==""){
			$languageIn = _var("language","en");
		}
		$language=$languageIn;
		// Для обратной совместимости
		if($language == "eng"){
			$language = "en";
		}
		if($language == "rus"){
			$language = "ru";
		}
		
		// язык
		$languageNumber=($language == "ru")?1:0;
		print "<!-- QUERY: ".getenv("QUERY_STRING")." -->\r\n";
		print "<!-- LANGUAGE: ".$language." -->\r\n";
	?>
	
	<html>
	<head>
	<title><?php 
	if($type=="wc"){
		print "WireChanger :: ";
	}else if($type=="wk"){
		print "WireKeys :: ";
	}else{
		print "WiredPlane.com :: ";
	}
	if($insType!=""){
		print "Help";
	}else{
		print getTitle($languageNumber);
	}
	?></title>
	<?php 
	if($language=="ru"){
		print "<meta http-equiv='Content-Type' content='text/html; charset=windows-1251'>\r\n";
	}
	//print "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>\r\n";
	?>
	<link href= "/images2/styles.css" rel="stylesheet" type="text/css">
	<meta name="copyright" content="Copyright © 2002-2006 WiredPlane.com, Razinkov Ilja">
	<meta http-equiv="author" content="WiredPlane.com, Razinkov Ilja">
	<meta http-equiv="url" content="http://www.wiredplane.com">
	<meta name="doc-publisher" content="WiredPlane.com">
	<meta name="classification" content="Software">
	<meta name="rating" content="General">
	<meta name="description" content="<?php 
	if($type=="wc"){
		print "WireChanger - Wind of change for your desktop. Add widgets, notes, interactive calendar and clocks on your desktop in a single click. Set reminders for future dates. Turn desktop into convinient PIM.";
	}else if($type=="wk"){
		print "WireKeys - Keys to your keyboard. Automation tool for managing common tasks with application, files and windows. Record macros, assign unusual hotkeys and many more. Works in all applications!";
	}else{
		print "Home for WireKeys, WireChanger and a lot of free tools. WireKeys: automation tool for managing common tasks with application, files and windows. WireChanger: wallpaper manager that turns your desktop into PIM with widgets and magical effects";
	}
	?>">
	<LINK REL="SHORTCUT ICON" HREF="/favicon.ico">
	</head>
	
	<body>
	
	<table width="980" border="0" cellspacing="0" cellpadding="0" align="center">
	<tr valign=top>
	<td width="41" background="/images2/backshadow_left.gif" valign=bottom></td>
	<td width="900" bgcolor="FFFFFF">
	
		<table width="900" height=100% border="0" cellspacing="0" cellpadding="0" align="center" class=table_mainpage>
		<tr><td><?php 
		if($type=="wk"){
			includeFile("wirekeys/header");
		}else if($type=="wc"){
			includeFile("wirechanger/header");
		}else{
			includeFile("commons/header");
		}
		?></td></tr>
		<tr><td width="900">
	
			<table width="900" height=100% border="0" cellspacing="0" cellpadding="0" align="center">
			<tr valign=top>
			<td width=150><?php includeFile("commons/vmenu");?></td>
			<td width=600><div id=the_page>
			<?php 
			$print_message=_var("print_message");
			if($print_message!=""){
				print "<div id=h1 align=center>".$print_message."</div><hr>";
			}
			if($insType==""){
				insertSection($languageNumber);
			}else{
				$vfile=getenv("DOCUMENT_ROOT")."/".$insType.".shtml";
				$openFile = fopen($vfile,"r");
				$filecont = fread($openFile, filesize($vfile));
				fclose($openFile);
				print $filecont;
			}
			?>
			<br>
			</div></td>
			<td width=150 valign=top><?php includeFile("commons/ads");?></td>
			</tr>
			</table>
	
		</td></tr>
		</table>
	
	</td>
	<td width="39" background="/images2/backshadow_right.gif" valign=bottom>
		<img src="/images2/backshadow_rightspacer.gif"></td>
	</tr>
	<tr>
		<td width="41" height="22"><img src="/images2/backshadow_bottomleft.gif" width=41 height=22></td>
		<td width="900" height="22"><img src="/images2/backshadow_bottom.gif" width=900 height=22></td>
		<td width="39" height="22"><img src="/images2/backshadow_bottomright.gif" width=39 height=22></td>
	</tr>
	</table>
	<center class="copyright">
	© 2002-2006, WiredPlane.com. All rights reserved. 
	<a href="../commons/guestbook.php">Contact Support</a> | <a href="../commons/privacy.php">Privacy</a>
	 | <a href="../commons/copyright.php">Copyright</a>
	 | <a href="../commons/help.php#newsletter"><?php if($l==1){?>Новостная рассылка<?php }else{?>Newsletter<?php }?></a>.
	<!--LiveInternet counter-->
	<script language="JavaScript"><!--
	document.write('<a href="http://www.liveinternet.ru/click" '+
	'target=_blank><img src="http://counter.yadro.ru/hit?t23.15;r'+
	escape(document.referrer)+((typeof(screen)=='undefined')?'':
	';s'+screen.width+'*'+screen.height+'*'+(screen.colorDepth?
	screen.colorDepth:screen.pixelDepth))+';u'+escape(document.URL)+
	';i'+escape('Жж'+document.title.substring(0,80))+';'+Math.random()+
	'" title="LiveInternet: показано число посетителей за сегодн\я" '+
	'border=0 width=88 height=15></a>')//--></script>
	<!--/LiveInternet-->
<?php 
define('_SAPE_USER', '6cce80cdb6d666c984f5d5f2b8231826'); 
require_once($_SERVER['DOCUMENT_ROOT'].'/'._SAPE_USER.'/sape.php'); 
$o['request_uri'] =$_SERVER['REQUEST_URI'];
print("<!--");
print("uri=".$o['request_uri']);
print("--><hr><font size=-1>");
$sape = new SAPE_client();
echo $sape->return_links();
print("</font>");
?>
</center>
</body>
</html>
<?php
	}

	function printLayout($type){
		return DoWork($type,"","");
	}
	
	//ob_end_flush();
?>