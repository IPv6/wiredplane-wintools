<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/*
*	File:				index.php
*	Description:	Handles display, login, and image submission
*	Copyright:	©2004 The Sonic Group, LLC
*	Website:		http://labs.sonicdesign.us/projects/Snaps!/
*
*	This program is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation; either version 2 of the License, or
*	(at your option) any later version.
*/

session_start();
if (!file_exists('install/')) {
/*
* Include the DB class from PEAR for database access.
* You must have PEAR and the DB package installed to
* use Snaps!. If you need instructions for installing PEAR
* and the DB package, please see http://pear.php.net
*/
require_once('DB.php');

/*
* Include the Snaps! configuration file
*/
require_once('admin/main.config.php');

function unzip($file,$zroot)
{
	global $rr,$base,$modtree,$modcurr;
	//echo $file."<br>";
	$z=new readzip($file);
	if(empty($z->ctrl_dir['entries']))
		return(T("Empty, corrupted or invalid zip file"));
	$zipList=$z->contentlist;
	usort($zipList,"sortZip");
	foreach($zipList as $elem)
	{
		$name=$zroot."/".basename($elem["filename"]);
		/*if(substr($elem["filename"],0,1)=='/')	$name=$elem["filename"];	else	$name=$zroot."/".$elem["filename"];
		if($elem["type"]=="folder") {
			if(!is_dir($name))
				if(!mkdir($name,0777))
					return (T($elem["filename"]." (Cannot make directory)"));
		}*/
		if($elem["type"]=="file") {
			$t=$z->writetmpfile($elem["index"],$zroot);
			if($t<0) return(ziperror($t));
			$r=$z->unziptmp($elem["index"],$t,$name);
			/*print "<hr>";
			print $t.'->'.$name;
			if(is_file($name)){
			    print "!!!";
			}*/
			if($r<0) return(ziperror($r));
			unlink($t);
		}
	}
	return("");
}

/* Connect to the database */
$db =& DB::connect($dsn);
if (DB::isError($db)) {
    die($db->getMessage());
}

/* Check and get the URL variables */
if (!empty($_GET['action'])) {
	$action = $_GET['action'];
} else {
	$action = '';
}

if (!empty($_GET['album'])) {
	$album = $_GET['album'];
} else {
	$album = '';
}

if (!empty($_GET['image'])) {
	$image = $_GET['image'];
} else {
	$image = '';
}

if (!empty($_GET['start'])) {
	$start = $_GET['start'];
} else {
	$start = '0';
}

if (!empty($_GET['w'])) {
	$w = $_GET['w'];
} else {
	$w = '';
}

if (!empty($_GET['h'])) {
	$h = $_GET['h'];
} else {
	$h = '';
}

/* Include the Snaps! functions */
require_once('./admin/main.functions.php');

/* Include Smarty class */
define('SMARTY_DIR', $config['absPath'].'Smarty/libs/');
include(SMARTY_DIR.'Smarty.class.php');

/* Instantiate Smarty and set output values */
$smarty = new Smarty;
$smarty->template_dir = SMARTY_DIR.'../templates/';
$smarty->compile_dir = SMARTY_DIR.'../templates_c/';
$smarty->config_dir = SMARTY_DIR.'../config/';
$smarty->cache_dir = SMARTY_DIR.'../cache/';

$smarty->assign('title', $title);
$smarty->assign('action', $action);
$smarty->assign('config', $config);
if (!empty($action)) {
	switch($action) {
		case 'login' :
			if (empty($_POST['submit'])) {
				$smarty->display('login.tpl');
			} else {
				$result =& $db->query('SELECT * FROM '.TP.'users WHERE username = \''.$_POST['username'].'\' AND userpass = \''.md5($_POST['password']).'\'');
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				if ($result->numRows() == 0) {
					$smarty->assign('error', 'Username or Password incorrect.');
					$smarty->display('login.tpl');
				} else {
					$_SESSION['name'] = $line['userFname'].' '.$line['userLname'];
					$_SESSION['userID'] = $line['userID'];
					$_SESSION['loggedIn'] = TRUE;
					$result =& $db->query('UPDATE '.TP.'users SET userLastLogin = UNIX_TIMESTAMP() WHERE userID = '.$_SESSION['userID']);
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					header("Location: admin/index.php");
				}
			}
			break;
		case 'submit' :
			if ($config['allowSubmit'] == 1) {
				if (empty($_GET['err'])) {
					if (empty($_POST['submit'])) {
						$smarty->assign('fname', date("Y", time())."_".date("d", time())."_".mt_rand());
						$smarty->display('upload.tpl');
					} else {
						$uploaddir = $config['absPath'].$config['uploadsPath'];
						$fileIntName=$_FILES['userfile']['name'];
						$fnamePost=$_POST['upName'];
						if($fnamePost==""){
							$fileIntName = mt_rand().'_'.$fileIntName;
							$fnamePost = $fileIntName;
						}
						$uploadfile = $uploaddir . basename($fileIntName);
						$uploadfileEx = $uploaddir . 'extern/'. basename($fileIntName);
						if(is_file($uploadfile)){
							unlink($uploadfile);
						}
						if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
							// Zip?
							if (stristr($uploadfile,".zip")!=false){
								require_once('admin/zipclass.php');
								if(is_file($uploadfileEx)){
									unlink($uploadfileEx);
								}
								rename($uploadfile,$uploadfileEx);
								if(is_file($uploadfile)){
									unlink($uploadfile);
								}
								$r=unzip($uploadfileEx,$uploaddir . 'extern');
								if(is_file($uploadfileEx)){
									unlink($uploadfileEx);
								}
								if($r!=""){
									$smarty->assign('etype', 'error');
									$smarty->assign('error', 'Bad archive?');
									$smarty->display('message.tpl');
								}else{
									$smarty->assign('etype', 'success');
									$smarty->assign('error', 'Archive successfully uploaded. The administrator will review your submission.');
									$smarty->display('message.tpl');
								}
							}else if (stristr($uploadfile,".jpg")!=false || stristr($uploadfile,".jpeg")!=false || stristr($uploadfile,".gif")!=false || stristr($uploadfile,".png")!=false){
								$result =& $db->query('INSERT INTO '.TP.'uploads (upID, upSubName, upSubEmail, upName, upDesc, upFilename, upCreated) VALUES ("", "'.mysql_escape_string($_POST['upSubName']).'", "'.mysql_escape_string($_POST['upSubEmail']).'", "'.mysql_escape_string($fnamePost).'", "'.mysql_escape_string($_POST['upDesc']).'", "'.basename($fileIntName).'", UNIX_TIMESTAMP())');
								if (DB::isError($result)) {
									die($result->getMessage());
								}
								if ($db->affectedRows() > 0) {
									$smarty->assign('etype', 'success');
									$smarty->assign('error', 'Image successfully uploaded. The administrator will review your submission.');
									$smarty->display('message.tpl');
								} else {
									$smarty->assign('etype', 'partial');
									$smarty->assign('error', 'Image successfully uploaded, but could not be added to database.');
									$smarty->display('message.tpl');
								}
							}else {
								$smarty->assign('etype', 'error');
								$smarty->assign('error', 'Unknown file type!');
								$smarty->display('message.tpl');
							}
						} else {
							$smarty->assign('etype', 'error');
							$smarty->assign('error', 'Possible file upload attack!');
							$smarty->display('message.tpl');
						}
					}
				}
			} else {
				$smarty->assign('etype', 'error');
				$smarty->assign('error', 'Image Submission has been disabled by the administrator.');
				$smarty->display('message.tpl');
			}
			break;
		case 'mn_help':
			$smarty->display('mn_help.tpl');
			break;
		case 'mn_privacy':
			$smarty->display('mn_privacy.tpl');
			break;
		case 'mn_links':
			$smarty->display('mn_links.tpl');
			break;
		case 'mn_about':
			$smarty->display('mn_about.tpl');
			break;
		case 'mn_disclaimer':
			$smarty->display('mn_disclaimer.tpl');
			break;
		case 'i':
			phpinfo();
			break;
		default:
			$smarty->display('mn_'.$action.'.tpl');
			break;
	}
} else if (!empty($_POST['submit'])) {
	if ($_POST['submit'] == 'Post Comment') {
		if ($config['allowComment'] == 1) {
			If (!empty($_POST['commentName']) && !empty($_POST['commentBody'])) {
				$smarty->assign('comment', comment('create', $image, $album));
				$smarty->display('cMessage.tpl');
			} else {
				$smarty->assign('etype', 'error');
				$smarty->assign('error', 'You must provide at least your name and a comment.');
				$smarty->display('message.tpl');
			}
		} else {
			$smarty->assign('etype', 'error');
			$smarty->assign('error', 'Commenting has been disabled by the administrator.');
			$smarty->display('message.tpl');
		}
	}
} else {
	if (empty($album)) {
		$smarty->assign('data', albumList());
		$smarty->assign('dataLast', lastAdditions());
		$smarty->display('albumList.tpl');
	} else {
		if (empty($image)) {
			$smarty->assign('data', album($album));
			$smarty->display('album.tpl');
		} else {
			if ($image!="rnd") 
			{
				$smarty->assign('data', image($album, $image));
				$smarty->assign('comment', comment('view', $image, $album));
				$smarty->assign('date_year', date("Y", time()));
				$smarty->assign('date_month', date("d", time()));
				if ($_SESSION['loggedIn']) {
					$smarty->assign('admin', '1');
				}
				$smarty->display('image.tpl');
			}else{
				$imagedata=image($album, $image);
				Header("Content-Type: image/jpeg");
				$image=$config['absPath'].$config['albumsPath'].$imagedata[1][5].'/'.$imagedata[1][1];
				if(empty($w)){
					readfile($image);
				}else{
					//В заданном размере...
					$src = imagecreatefromjpeg($image);
					$dst = ImageCreateTrueColor($w,$h);
					ImageCopyResized($dst,$src,0,0,0,0,$w,$h,imagesx($src),imagesy($src));
					ImageJpeg($dst, '', 70);
					ImageDestroy($src); 
					ImageDestroy($dst);
				}
				exit();
			}
		}
	}
}
} else {
	die('<html><head><title>Snaps! Gallery :: Security Violation</title></head><body><div style="width: 600px; margin: 0 auto; padding: 5px; text-align: center; background: #EEE; border: 1px solid #666;"><h2 style="text-align: left; font-family: Verdana, Arial, Helvetica, sans-serif; padding: 5px; margin: 0;">Security Violation!</h2><p style="text-align: left; font-size: 11px; font-family: Verdana, Arial, Helvetica, sans-serif; font-weight: bold;">If you are seeing this message, one of the following has occured:</p><p style="padding-left: 10px; color: #F00; text-align: left; font-size: 11px; font-family: Verdana, Arial, Helvetica, sans-serif; font-weight: bold;">&bull; Snaps! has not been installed<br />&bull; You have not deleted the "install" directory after installing Snaps!<br />&bull; A security breach</p></div><br /></body></html>');
}
?>