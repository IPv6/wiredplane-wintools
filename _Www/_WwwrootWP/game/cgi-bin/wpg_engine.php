<?php
$g_affiliateCatalog="http://gameboss.ru/games/id-31605/";

include_once("_util.php");
function redirectToCatalog()
{
	global $g_affiliateCatalog;
	Header("Location: ".$g_affiliateCatalog);
}
function redirectToSupport()
{
	Header("Location: http://www.wiredplane.com/en/commons/guestbook.php");
}
function redirectToForum()
{
	Header("Location: http://www.wiredplane.com/forum/index.php");
}
function redirectToDownload($filetoload)
{
	Header("Location: /download.php?file=".$filetoload);
}
// Общее
$action=_var("action");
if($action=="opencatalog"){
	$handled=1;
	redirectToCatalog();
}

if($action=="opensupport"){
	$handled=1;
	redirectToSupport();
}

if($action=="openforum"){
	$handled=1;
	redirectToForum();
}
?>