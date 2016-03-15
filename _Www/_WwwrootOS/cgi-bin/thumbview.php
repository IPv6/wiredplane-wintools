<?php
	include "_util.php";
	$action=_var('action');
	$image=_var('image');
	$randseed=_var('randseed');
	$contest=_var('contest');
	$title=_var('title');
	$contrib=_var('contrib');
	$comments=_var('comments');
	$who=_var('who');
	$uname=_var('uname');
	if($action=="info"){
		phpinfo();
		exit();
	}
	// Инициализация закончена
	srand();
	function writeImageData($image, $i_title, $i_contributor, $i_contrdate, $i_counter, $i_hash, $i_comment)
	{
		$fileName="../../data/gallery/".$image.".inf";
		if(!file_exists($fileName)){
			$createFile = fopen($fileName, "w");
			fwrite ($createFile,"",0);
			fclose($createFile);
		}
		chmod($fileName, 0666);
		$fileData=$i_title."<!-- ### -->".$i_contributor."<!-- ### -->".$i_contrdate."<!-- ### -->".$i_counter."<!-- ### -->".$i_hash."<!-- ### -->".$i_comment;
		$openFile = fopen($fileName,"w+");
		if($openFile){
			fwrite($openFile,$fileData);
			fclose($openFile);
		}
	}
	function readImageData($image, &$i_title, &$i_contributor, &$i_contrdate, &$i_counter, &$i_hash, &$i_imagesize, &$i_comment)
	{
		$ires=1;
		$i_imagesize = getimagesize ("../../data/gallery/".$image.".jpg");
		$fileName="../../data/gallery/".$image.".inf";
		if(!file_exists($fileName)){
			$createFile = fopen($fileName, "w");
			fwrite ($createFile,"",0);
			fclose($createFile);
			$ires=0;
		}
		chmod($fileName, 0666);
		$openFile = fopen($fileName,"r");
		$fileData = fread($openFile, filesize ($fileName)); 
		fclose($openFile);
		$str = explode("<!-- ### -->",$fileData);
		if($str[0]!=""){
			$i_title=$str[0];
		}else{
			$i_title=$image;
			if($ires==0){
				// Mark that file was newly created
				$i_title=$i_title."!";
			}
			$ires=0;
		}
		if($str[1]!=""){
			$i_contributor=$str[1];
		}else{
			$ires=0;
			$i_contributor="WiredPlane Labs";
		}
		if($str[2]!=""){
			$i_contrdate=$str[2];
		}else{
			$ires=0;
			$currHour=date("H",time());
			$currTime=date("$currHour:i", time());
			$i_contrdate=date("dS M", time());
		}
		if($str[3]!=""){
			$i_counter=$str[3];
		}else{
			$ires=0;
			$i_counter=0;
		}
		if($str[4]!=""){
			$i_hash=$str[4];
		}else{
			$ires=0;
			$i_hash="0";
		}
		if(sizeof($str)>5 && $str[5]!=""){
			$i_comment=$str[5];
		}else{
			$i_comment="";
		}
		return $ires;
	}

	function WriteImageBlock($imageName, &$form)
	{
		$i_title = "";
		$i_contributor = "";
		$i_contrdate = "";
		$i_counter = 0;
		$i_hash = "0";
		$i_imagesize = 0;
		$i_comment = "";
		readImageData($imageName, $i_title, $i_contributor, $i_contrdate, $i_counter, $i_hash, $i_imagesize, $i_comment);
		if($form=="yes"){
			print "<td align=center bgcolor=gray><input type=radio name=img_check value='$imageName' onclick=\"setOnlineImage('$imageName');\">";
		}
		print "<td width=10% align=center valign=top><a href='/cgi-bin/thumbview.php?action=fullv&image=".$imageName."&randseed=".rand()."' target='_blank'><img name='img_".$imageName."' src='/cgi-bin/thumbview.php?action=thumb&image=".$imageName."' border=0 width=140 height=105></a></td><td valign=top name='".$imageName."_td' id='".$imageName."_td'><b>$i_title</b><br>$i_contributor<br>Date: $i_contrdate<br>Resolution: $i_imagesize[0]x$i_imagesize[1]<br>Views: $i_counter<br><a href='/cgi-bin/thumbview.php?action=fullv&image=".$imageName."&randseed=".rand()."' target='_blank'>Open image</a></td>";
	}
?>
<?php 
	$form=_var("form");
	if($form==""){
		$form="no";
	}
	if($action=="upload_form"){
		$action="preview";
		$form="yes";
	}
	if($action=="preview"){
		if($form=="yes"){
?>
<html><head><title>WireChanger`s Wallpaper exchange service</title>
<link rel="stylesheet" href="/images/styles.css" TYPE="text/css"></head>
<body bgcolor="#EEEEEE">
<div align="center">
<script language=JavaScript>
var bInProgress=0;
var lastBgChanged="";
var lastExhangedFile="";
function setOnlineImageByIndex(imageInd)
{
	ThisForm=document.forms['upform'];
	ThisForm.img_check[imageInd].checked=true;
	ThisForm.img.selectedIndex=imageInd;
	ThisForm=document.forms['upform'];
	document.images['rf_preview'].src=document.images['img_image'+(imageInd+1)].src;
	if(document.all){
		var newBgChanged='image'+(imageInd+1)+'_td';
		document.all[newBgChanged].style.backgroundColor='darkgray';
		if(lastBgChanged!=""){
			document.all[lastBgChanged].style.backgroundColor='';
		}
		lastBgChanged=newBgChanged;
	}
}

function setOnlineImage(imageName)
{
	var index=imageName.slice(5)-'0';
	setOnlineImageByIndex(index-1);
}

function CheckSubmit()
{
	ThisForm=document.forms['upform'];
	if (bInProgress!=0){
		alert("Please, wait until exchange will finish\n"+lastExhangedFile+"\nTo exchange another wallpaper please reopen this window.");
		return false;
	}
	if (ThisForm.title.value == ""){
		alert("Please enter image title.");
		ThisForm.title.focus();
		return false;
	}
	if (ThisForm.contrib.value.length<4 ){
		alert("Please enter your name (at least 4 characters).");
		ThisForm.contrib.focus();
		return false;
	}
	var userFile=ThisForm.userfile.value;
	if (userFile == ""){
		alert("Please enter image file path.");
		ThisForm.userfile.focus();
		return false;
	}
	lastExhangedFile=userFile;
	var bRes=confirm("Starting upload of your image\nin exchange for choosen one\nPress OK to continue!");
	if(bRes){
		bInProgress=1;
		if(document.all){
			document.all['H2_Instructions'].innerHTML="Uploading of your image started. Please wait, this may take several minutes depending on your Internet connection speed.";
		}
		return true;
	}
	return false;
}

function setPreview()
{
	ThisForm=document.forms['upform'];
	var newPath=ThisForm.userfile.value;
	document.images['lf_preview'].src='file:///'+newPath;
}
</script>
<form action='/cgi-bin/thumbview.php?rndseed=<?php print rand(); ?>' name='upform' id='upform' method=post enctype='multipart/form-data' onsubmit='return CheckSubmit();'>
<h1>WireChanger`s Wallpaper exchange service</h1><hr>
<input type=hidden	name=action value='upload'>
<input type=hidden	name=hash value='none'>
<?php
	if(array_key_exists("contest",$_REQUEST) && $_REQUEST["contest"]=="yes"){
		print "<input type=hidden	name=contest value='$contest'>";
		print "<b><h2><font color=red>Using this page you can register WireChanger for FREE. You just have to upload one of your wallpaper in exchange for another image.";
		print " Follow instructions below and Your registration key will be generated on the next screen</font></h2></b><br>";
	}
	print "<input type=hidden name=who value='".$who."'>";
?>
<center>
<div id="SEND_DATA" name="SEND_DATA">
<table border=0 width=80%>
<tr>
<td rowspan=5 valign=top><img src="/images/browseimg.jpg" width=140 name="rf_preview" border=1></td>
<td rowspan=5 valign=top><img src="/images/browseimg.jpg" width=140 name="lf_preview" border=1 _on_Click_="document.forms['upform'].userfile.click();"></td>
<td colspan=2>Exchange your image with:
<select name='img' onchange='setOnlineImageByIndex(this.selectedIndex);'>
<option value='image1'>Image #1</option>
<option value='image2'>Image #2</option>
<option value='image3'>Image #3</option>
<option value='image4'>Image #4</option>
<option value='image5'>Image #5</option>
<option value='image6'>Image #6</option>
<option value='image7'>Image #7</option>
<option value='image8'>Image #8</option>
<option value='image9'>Image #9</option>
<option value='image10'>Image #10</option>
</select>
</td>
</tr>
<tr>
<td><b>Your name</b>:</td><td><input type=text name=contrib size=40></td>
</tr>
<tr>
<td><b>Image title</b>:</td><td><input type=text name=title size=40></td>
</tr>
<tr>
<td>Comments:</td><td><input type=text name=comments size=40></td>
</tr>
<tr>
<td><b>Your image</b>:</td><td colspan=3><input type=file size=40 name=userfile onChange="setPreview();"></td>
</tr>
<tr>
<td colspan=4 align=center>
<h2 id="H2_Instructions" name="H2_Instructions">Choose image you want to download from the list below<br>Type your name and image title and click <input type='submit' value='Exchange!'> button.</h2>
Note: Image resolution must 640*480 pixels or higher; image must be not bigger than <b>1000Kb</b> and must be in <b>jpg, gif or png</b> format.
</td>
</tr>
</table>
</div>
</td></tr></table>
<hr>
<center>
<?php
		}else if($form!="nobody"){
?>
<html><head><title>Full Size view - '<?php print "$image"; ?>'</title><link rel="stylesheet" href="/images/styles.css" TYPE="text/css"></head>
<body bgcolor="#FFFFFF">
<?php
		}
?>
<table border=0 width=100% cellspacing="2" cellpadding="2">
<tr><td colspan=7>
<table border=0 background="http://outer-station.m6.net/images/outer-station-logo.jpg" width=100%>
<tr><td align=center valign=center><font color=white size=+2 face=Verdana><b><a href="http://outer-station.m6.net/" style="text-decoration: none; color:white;" target=_top>OUTER-STATION::Endless wallpapers</a></b></font></tr></table>
</td></tr>
<?php 
		print "<tr>\n";
		WriteImageBlock("image1",$form);
		WriteImageBlock("image2",$form);
		print "\n</tr>";
		print "<tr>\n";
		WriteImageBlock("image3",$form);
		WriteImageBlock("image4",$form);
		print "\n</tr>";
		print "<tr>\n";
		WriteImageBlock("image5",$form);
		WriteImageBlock("image6",$form);
		print "\n</tr>";
		WriteImageBlock("image7",$form);
		WriteImageBlock("image8",$form);
		print "\n</tr>";
		WriteImageBlock("image9",$form);
		WriteImageBlock("image10",$form);
		print "\n</tr>";
?>
</table>
<div align=center><a href="http://outer-station.m6.net" target="_blank">
<img border=0 src="http://outer-station.m6.net/images/banners/more-wallpapers.jpg"></a></div>
<hr>
<?php
		if($form=="yes"){
			print "<script language=JavaScript>setOnlineImageByIndex(Math.ceil(9*Math.random()));</script></form>";
		}
		if($form!="nobody" || $form=="yes"){
			print "</body></html>";
		}
	}else if($action=="thumb"){
		if($image=="rnd"){
			$image="image".rand(1,10);
		}
		loadGD();
		$image="../../data/gallery/".$image.".jpg"; //исходное файл
		$src = imagecreatefromjpeg($image); 
		$bw = imagesx($src);
		$bh = imagesy($src);
		$needx=140;
		// Маштабим до $needx по X
		if($bw/$needx > $bh/$needx){
			$cq=$needx/$bw;
		}else{
			$cq=$needx/$bh;
		}
		$w=$bw*$cq;
		$h=$bh*$cq;
		$dst = ImageCreateTrueColor($w,$h);
		ImageCopyResized($dst,$src,0,0,0,0,$w,$h,$bw,$bh);
		header("Content-type: image/jpeg"); 
		ImageJpeg($dst, '', 70);// подогнать компрессию
		ImageDestroy($src); 
		ImageDestroy($dst);
	}else if($action=="upload"){
		loadGD();
		function WriteError($error)
		{
?>
<html><head><title>WireChanger`s Wallpaper exchange service</title><link rel="stylesheet" href="/images/styles.css" TYPE="text/css"></head>
<body bgcolor="#EEEEEE"><div align="center">
<h1><?php print $error; ?></h1>
<hr>
</body>
</html>
<?php
		}
		$img=_var("img");
		if($img==""){
			$img=getOldestImage("../../data/gallery/");
		}
		if($img==""){
			WriteError("Error: Target image must be filled out!!!");
		}else if($title==""){
			WriteError("Error: Image title must be filled out!!!");
		}else if($contrib==""){
			WriteError("Error: Your name must be filled out!!!");
		}else if(!array_key_exists('userfile',$_FILES)){
			WriteError("Error: File can not be uploaded!!!");
		}else if($_FILES['userfile']['size']>1000*1024){
			WriteError("Error: Image file is too big (".$_FILES['userfile']['size']."b)! Try another image.");
		}else if($_FILES['userfile']['error']!=0){
			WriteError("Error: Image file was not properly uploaded! Try another image.");
		}else{
			$i_type=$_FILES['userfile']['type'];
			$imageFile="../../data/gallery/".$img.".jpg";
			$imageFileOut="../../data/gallery/exchanged.jpg";
			if($i_type!="image/jpeg" && $i_type!="image/pjpeg" && $i_type!="image/jpg" && $i_type!="image/png" && $i_type!="image/gif"){
				WriteError("Error: Image is in the unsupported image type! Try another image.");
				exit();
			}
			$i_title2 = "";
			$i_contributor2 = "";
			$i_contrdate2 = "";
			$i_counter2 = "";
			$i_hash2 = "";
			$i_imagesize2 = 0;
			$i_comment2 = "";
			// Checking md5...
			$i_hash=md5_file($_FILES['userfile']['tmp_name']);
			{// Пишем в md5store
				$fileNameMD5="../../data/md5store_".date("_F_Y", time()).".inc";
				if(file_exists($fileNameMD5)){
					$openFileMD5 = fopen($fileNameMD5, "r");
					$size = filesize ($fileNameMD5);
					$newHashStore = fread($openFileMD5, $size);
					fclose($openFileMD5);
					if(strstr($newHashStore,$i_hash)!=FALSE){
						WriteError("Error: Image is identical to one of the recently uploaded images. Please upload another image.");
						exit();
					}
				}
				$openFileMD5 = fopen($fileNameMD5, "w");
				if(strlen($newHashStore)>10000){
					$newHashStore=substr($newHashStore,0,10000);
				}
				$newHashStore = $i_hash."\n".$newHashStore;
				fwrite($openFileMD5,$newHashStore);
				fclose($openFileMD5);
			}
			// Sending New image...
			$cancopy=0;
			$newsrc = 0;
			if($i_type=="image/jpeg" || $i_type=="image/pjpeg" || $i_type=="image/jpg"){
				$newsrc = imagecreatefromjpeg($_FILES['userfile']['tmp_name']);
				$cancopy = 1;
			}
			if($i_type=="image/png"){
				$newsrc = imagecreatefrompng($_FILES['userfile']['tmp_name']);
			}
			if($i_type=="image/gif"){
				$newsrc = imagecreatefromgif($_FILES['userfile']['tmp_name']);
			}
			if(!$newsrc){
				WriteError("Error: Image is in the unsupported image type! Try another image.");
				exit();	
			}
			$bw = ImageSX($newsrc);
			$bh = ImageSY($newsrc);
			if(abs($bw/$bh-1024/768)>0.2 || abs($bw/$bh-640/480)>0.2){
				WriteError("Error: Image is disproportional too much ($bw x $bh)! Try another image.");
			}else if($bw<640 || $bh<480){
				WriteError("Error: Image is too small! Try another image (at least 640*480).");
			}else{
				$i_title = translateHtml($title);
				$i_contributor = translateHtml($contrib);
				$cOK=(strlen($i_contributor)>0);
				for($icok=0;$icok<strlen($i_contributor);$icok++){
					if(!($i_contributor[$icok]<='z') && !($i_contributor[$icok]>='A' && $i_contributor[$icok]<='Z') && !($i_contributor[$icok]>='0' && $i_contributor[$icok]<='9')){
						$cOK=0;
					}
					if($i_contributor[$icok]=='+' || $i_contributor[$icok]=='?' || $i_contributor[$icok]=='<' || $i_contributor[$icok]=='>' || $i_contributor[$icok]=='*' || $i_contributor[$icok]=='\"' || $i_contributor[$icok]=='\'' || $i_contributor[$icok]=='\\' || $i_contributor[$icok]=='/' || $i_contributor[$icok]==':'){
						$cOK=0;
					}
				}
				if(!$cOK){
					WriteError("Error: Contributir name is incorrect. Only latin characters and numbers allowed (a-z, A-Z, 0-9)");
					exit();
				}
				$i_comment = translateHtml($comments);
				$currHour=date("H",time());
				$currTime=date("$currHour:i", time());
				$i_contrdate=date("dS M", time());
				$i_counter = 0;
				// Old image
				$ires=readImageData($img, $i_title2, $i_contributor2, $i_contrdate2, $i_counter2, $i_hash2, $i_imagesize2, $i_comment2);
				unlink($imageFileOut);
				if(!rename($imageFile,$imageFileOut)){
					WriteError("Error: Can`t set-up temporary file! Notify admin about this problem!");
					exit();	
				}
				writeImageData("exchanged", $i_title2, $i_contributor2, $i_contrdate2, $i_counter2, $i_hash2, $i_comment2);
				$adddata="";
				if(array_key_exists("contest",$_REQUEST) && $_REQUEST["contest"]=="yes"){
					$adddata="contest=yes&uname=$i_contributor";
				}
				// New image
				if($cancopy==1){
					rename($_FILES['userfile']['tmp_name'],$imageFile);
				}else{
					ImageJpeg($newsrc, $imageFile, 90);
				}
				chmod($imageFile, 0666);
				writeImageData($img, $i_title, $i_contributor, $i_contrdate, $i_counter, $i_hash, $i_comment);
				{// Пишем в галлерею
					$currTimeWG=mt_rand();
					$i_contrdateWG=date("dS_M", time());
					$wgname="../uploads/extern/".$i_contrdateWG."_".$currTimeWG."_".$i_contributor.".jpg";
					copy($imageFile, $wgname);
				}
				{// Пишем в shout
					$currHour=date("H",time());
					$currTime=date("$currHour:i", time());
					$currDate=date("dS M", time());
					$fileName="../../data/shout_".$img.".inc";
					$newMsg="<!-- new shout -->\n";
					$newMsg=$newMsg."<table border=0 cellspacing=0 cellpadding=0 width=100%>";
					$newMsg=$newMsg."<tr><td bgcolor=#000000 height=1%></td></tr>";
					$newMsg=$newMsg."<tr><td bgcolor=#DDDDDD align=left><span style='font-size: 8pt;'>[".$currDate."] <b>";
					$newMsg=$newMsg."New image '".$i_title."', posted by ".$i_contributor;
					$newMsg=$newMsg."</b></span></td></tr>";
					$newMsg=$newMsg."</table>\n";
					$openFile = fopen($fileName,"r");
					$newMsg = $newMsg.fread($openFile, filesize ($fileName));
					fclose($openFile);
					writeDataInFile($newMsg,$fileName);
				}
				Header("Location: http://outer-station.m6.net/cgi-bin/thumbview.php?rndseed=".rand()."&action=fullv&".$adddata."&"."image"."=exchanged&who=".$who);
			}
			ImageDestroy($newsrc); 
			exit();
		}
	}else if($action=="src"){
		Header("Content-Type: image/jpeg");
		$image="../../data/gallery/".$image.".jpg"; //исходное файл
		readfile($image);
		exit();
	}else if($action=="fullv"){
?>
<html><head><title>Full Size view - '<?php print "$image"; ?>'</title><link rel="stylesheet" href="/images/styles.css" TYPE="text/css"></head>
<body bgcolor="#FFFFFF">
<div align="center">
<?php
	$i_title = "";
	$i_contributor = "";
	$i_contrdate = "";
	$i_counter = 0;
	$i_hash = "0";
	$i_imagesize = 0;
	$i_comment = "";
	readImageData($image, $i_title, $i_contributor, $i_contrdate, $i_counter, $i_hash, $i_imagesize, $i_comment);
	if(array_key_exists("contest",$_REQUEST) && $_REQUEST["contest"]=="yes"){
		$afterWeek = mktime(0,0,0,date("m"),date("d")+7,date("Y"));
		$currDate=date("dmy", $afterWeek);
		$toHash="WIRECHANGER".$currDate."ASTON-PISTON-DRUBEL".$uname."MARTIN-CARTIN-GIGARTESK";
		$newKey="WPK".$currDate.md5($toHash);
		$fileName="../../data/tempkeys".date("_F_Y", time()).".log";
		appendDataInFile($fileName,$uname."/".$newKey,$who);
		// Формат менять нельзя - не прочтется потом
		print "<hr><div align=center>Thank you for contributing your wallpaper! ";
		print "<table border=0 width=90%>";
		print "<tr><td colspan=2>";
		print "<b>Attention! <a href='http://www.wiredplane.com/cgi-bin/wp_engine.php?target=vendor&who=WireChanger_Exchange'>Order full license</a></b> to unlock advanced features of WireChanger: additional collections of clocks and widgets, duplicated images remover, 'print note' option, reminders, option to pack theme into archive, image sorting options and other features.";
		print " You can also <a href='https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-1&cdrom=yes'><b>buy CD</b></a> with a hundreds of beautiful images, packed with fully-registered WireChanger. By ordering WireChanger you will help us to improve this software";
		//print "<br>Enter following registration information into <a href='http://www.wiredplane.com/wirechanger/info.shtml'>WireChanger</a> to became registered for another week";
		print "<hr>";
		print "</td></tr>";
		print "<tr bgcolor='#ddddff'>";
		print "<td align=right width=40% style='font-size: 14px;'>Registration name</td><td align=left style='font-size: 18px;'><b>".$uname."</b></td></tr>";
		print "<tr bgcolor='#ddddff'>";
		print "<td align=right width=40% style='font-size: 14px;'>Registration key</td><td align=left style='font-size: 18px;'><b>".$newKey."</b></td></tr>";
		print "<tr bgcolor='#ddddff'>";
		print "<td align=center width=40% colspan=2>";
		print "To register WireChanger: choose 'Enter key' from tray menu OR open preferences and click on 'Registration'.<br>Enter Name (as registration email) and Key and click 'Ok' to apply new registration information";
		print "<br>If you have any questions, feel free to contact us by using <a href='http://www.wiredplane.com/guestbook.shtml'>this contact form</a></td></tr>";
		print "</table></div><hr>";
	}
	if(!array_key_exists("result",$_REQUEST) || $_REQUEST["result"]!="shoutok"){
		$i_counter = 1+$i_counter;
		writeImageData($image, $i_title, $i_contributor, $i_contrdate, $i_counter, $i_hash, $i_comment);
	}
	$imageDesk="Picture: $i_title ($i_contributor) ";
	if($i_comment!=""){
		//print "Comments from contributor: ";
		$imageDesk.="'".$i_comment."'";
	}
	$imageDesk.="; Viewed: $i_counter time(s) so far. Real resolution: $i_imagesize[0]x$i_imagesize[1]. ";
?>

<table border="0" cellpadding="0" cellspacing="5" align="center" width=810>
<tr><td align=center valign=center>
<b>To save this picture, right-click on the image and select 'Save Picture As...'</b>. 
<a href="http://www.wiredplane.com/wirechanger/">WireChanger</a> users can 
<br>right-click on the image and select 'Set as background' to automatically add this picture to local image gallery
</td><td align=right valign=top><a href='http://www.m6.net'><img src='../images/m6hosted.gif' width=90 border=1></a></td></tr>
<tr><td align="center" colspan=2><img src="/cgi-bin/thumbview.php?action=src&rndseed=<?php print rand(); ?>&image=<?php print $image; ?>" width="800"></td></tr>
<tr><td align="center" colspan=2>
<?php print $imageDesk; ?>
<br>All pictures (wallpapers) displayed here are copyrighted by their respective owners.
<br>Pictures can be removed at the request of the author. Feel free to contact us by using <a href='http://www.wiredplane.com/guestbook.shtml'>this contact form</a>
</td></tr>
</table>

<hr>
<table width="590" border="0" cellspacing="0" cellpadding="0" align="center" >
<tr><td align=center><a href="http://www.wiredplane.com/wirechanger/info.php"><img src="http://www.wiredplane.com/images2/wirechanger/splash.gif" width=560 height=180></a>
<br>WireChanger <a href="http://www.wiredplane.com/download/wirechanger.zip">Download</a>::<a href="http://www.wiredplane.com/commons/order.php">Order</a>::<a href="http://www.wiredplane.com/wirechanger/info.php">More info</a>::<a href="http://www.wiredplane.com/wirechanger/screens.php">Screenshots</a>
</td></tr>
</table>
</div>
</body>
</html>
<?php
	}else{
		print "Unknown parameters!";
	}
?>
