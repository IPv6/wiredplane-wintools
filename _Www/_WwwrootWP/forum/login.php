<?php
/***************************************************************************
 *                            login.php  - PBLang
 *
 *                  see the file copyright.txt  in the docs-folder!!
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include($dbpath."/settings/styles/styles.php");
$do=$HTTP_GET_VARS['do'];
$getlang=$HTTP_GET_VARS['lang'];
if ($getlang!=""){
     $language=$getlang;
}
include_once("$temppath/pb/language/lang_$language.php");
setlocale (LC_TIME,$LangLocale);

$username=$HTTP_COOKIE_VARS[$cookieid];
if ($username=="") {
		$loggedin="0";
		$admin="0";
		$allow="1";
} else {
	$filename = "$dbpath/members/$username";
	if (!file_exists($filename)) {
		$loggedin="0";
	} else {
		include($filename);
		$ip = GetIPAddress();
		if ($userip==$ip) {
			$allow="1";
		} else {
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=logout.php\">";
			$allow="0";
			exit;
		}
		$loggedin="1";

		$admin=$useradmin;
		$moder=$usermod;
		$ban=$userban;
	}
}
//writeheader($newestm,0,0,0,$username,$loggedin,$VLogin);
writeheader('top',0,0,0,$username,$loggedin,$VLogin);

if ($ban=="1") {
	setcookie ($cookieid, "", time() - 3600, "/");
	ErrorMessage($banreason,$username);
	exit;
} else {
	if ($allow=="1") {
		if ($loggedin=="1") {
			ErrorMessage($AlreadyLoggedIn,$username);
		} else {
			$idnum=$HTTP_GET_VARS['id'];
			$sub=$HTTP_POST_VARS['Submit2'];
			if ($idnum=="" && $do=="" && $loginreq!="1") {
				loginform("");
			} else {
				if ($idnum==""){
					msg($VNotice,"$RegistrationRequired!");
					loginform("req");
				}
			}

			if ($idnum=="2") {		//interpreting the data submitted by the login form
				$u=$HTTP_POST_VARS['user'];
				$p=md5($HTTP_POST_VARS['pass']);
				$filet=$dbpath.'/members/'.$u;
				if (!file_exists($filet)) {
					ErrorMessage($NonExistingUser,$username);
					writefooter($newestm);
					exit;
				} elseif (trim($u)=="") {
					ErrorMessage($UsernameRequired,$username);
					writefooter($newestm);
					exit;
				} elseif (trim($p)=="" && $sub!=$RetrievePassword) {
					ErrorMessage($PasswordRequired,$username);
					writefooter($newestm);
					exit;
				} else {
					if ($sub==$RetrievePassword) {
						$mail=$PasswordMessage.".\n\n".$ContactAdmin." <a href=\"mailto:".$adminemail."\">".$adminemail."</a>\n";
						msg($VSent,"$mail");
					} else {
						$content .='<'.'?php'."\n";
						$content .="\$u=\"$u\";\n";
						$content .="\$p=\"$p\";\n?".'>';
						$fp=fopen($dbpath.'/'.$u.'temp','w');
						fputs($fp,$content);
						echo "<meta http-equiv=\"Refresh\" content=\"0; URL=setcookie.php?u=$u\">";
					}
				}
			}
			writefooter($newestm);
		}
	}
}
ob_end_flush();
?>

