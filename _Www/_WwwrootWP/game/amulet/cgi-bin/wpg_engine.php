<?
$email="support@wiredplane.com";
$module="amulet";
include("../../cgi-bin/wpg_engine.php");

$action=_var("action");
$handled=0;
if($action=="acceptkey"){
	$handled=1;
	$emailbody="Module registration detected: ".$module."\r\n";
	$emailbody.="Email: "._var("email")."\r\n";
	$emailbody.="Key: "._var("key")."\r\n";
	$emailtitile="Module registration:".$module;
	$replyto="support@wiredplane.com";
    $headers="From: ".$replyto."\r\n" ."Reply-To: ".$replyto."\r\n";
	@mail($email,$emailtitile,$emailbody,$headers);
	print("Registration accepted");
}
if($action=="opensmsinfo"){
	$handled=1;
	//Header("Location: http://smszamok.ru/client/izamok.php?1635");
	Header("Location: /amulet/smspage.php");
}
if($action=="opensupport"){
	$handled=1;
	redirectToSupport();
}
if($action=="opencatalog"){
	$handled=1;
	redirectToCatalog();
}
if($action=="opendownloadpage"){
	$handled=1;
	//redirectToDownload("/amulet/download/amuletsetup.zip");
	redirectToDownload("http://files2.gamerotor.com/game_amulet/amuletsetup_ru.zip");
	//print("?");
}
if(!$handled){
	redirectToCatalog();
}
?>
