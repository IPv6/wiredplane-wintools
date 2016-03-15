<?php
/***************************************************************************
 *                            admin2.php   -  PBLang
 *                            -------------------
 *
 *              see the file copyright.txt in the docs-folder
 *
 *        updated: 16 May 2003
***************************************************************************/


error_reporting  (E_ERROR | E_WARNING | E_PARSE);
//error_reporting  (E_ALL);
set_magic_quotes_runtime(0); // Disable magic_quotes_runtime

ob_start();

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
                                                     // always modified
header("Cache-Control: no-store, no-cache, must-revalidate");  // HTTP/1.1
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");

define('IN_PB', "true");
define('IN_PBG', "true");

include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once("scan.php");
include("$temppath/pb/language/lang_$language.php");

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1"){
	if ($admin!="1" && $moder!="1") {
		echo $NoAccess;
		ErrorMessage($AccessDenied,$liusername);
		exit;
	} elseif ($admin=="1" || $moder=="1") {
		$do=$HTTP_GET_VARS['do'];
		if ($do=="") {
			FAccessDenied();
			exit;
		}

		if ($do=="fsettings2" && $admin=="1") {                 //edit forum settings  - action
			$emsupp=$HTTP_POST_VARS['emsupp'];
			$sitetitle=($HTTP_POST_VARS['title']);
			$sitelogo=trim($HTTP_POST_VARS['logo']);
			$boardbgimage=trim($HTTP_POST_VARS['bbgimage']);
			$sitelogowidth=trim($HTTP_POST_VARS['sitelogowidth']);
			$sitelogoheight=trim($HTTP_POST_VARS['sitelogoheight']);
			$defaultslogan=($HTTP_POST_VARS['defslogan']);
			$sitemotto=($HTTP_POST_VARS['motto']);
			$footer=($HTTP_POST_VARS['footer']);
			$boardwidth=$HTTP_POST_VARS['boardwidth'];
			$boardalign=$HTTP_POST_VARS['boardalign'];
			$html='1';
			$changealias=$HTTP_POST_VARS['chalias'];
			$allowalias=$HTTP_POST_VARS['allalias'];
			$smilys=$HTTP_POST_VARS['smilys'];
			$bbcode=$HTTP_POST_VARS['bbcode'];
			$censor=$HTTP_POST_VARS['censor'];
			$timezone=$HTTP_POST_VARS['timezone'];
			$loginreq=$HTTP_POST_VARS['logreq'];
			$maint=$HTTP_POST_VARS['maint'];
			$ave=$HTTP_POST_VARS['av'];
			$webave=$HTTP_POST_VARS['webav'];
			$avmaxwidth=trim($HTTP_POST_VARS['avmaxwidth']);
			$avmaxheight=trim($HTTP_POST_VARS['avmaxheight']);
			$mreason=$HTTP_POST_VARS['mr'];
			$noatch=$HTTP_POST_VARS['noatch'];
			$allowreg=$HTTP_POST_VARS['allowreg'];
			$mailenabled=$HTTP_POST_VARS['allowem'];
			$banreason=$HTTP_POST_VARS['banreason'];
			$adminemail=$HTTP_POST_VARS['adminemail'];
			$maxatchsize=$HTTP_POST_VARS['maxatchsize'];
			$minspace=$HTTP_POST_VARS['minspace'];
			$maxposts=trim($HTTP_POST_VARS['maxposts']);
			$maxppp=trim($HTTP_POST_VARS['maxppp']);
			$maxrpp=trim($HTTP_POST_VARS['maxrpp']);
			$maxlatest=trim($HTTP_POST_VARS['maxlatest']);
			$confirmemail=$HTTP_POST_VARS['confirmemail'];
			$welcomemessage=$HTTP_POST_VARS['welcmess'];
			$checkip=$HTTP_POST_VARS['chckip'];
			$notifyadmin=$HTTP_POST_VARS['notifyadm'];
			$notifyadminrepl=$HTTP_POST_VARS['notifyadmrepl'];
			$charsets=$HTTP_POST_VARS['charsets'];
			$textareawidth=$HTTP_POST_VARS['textareawidth'];
			$attachtypelist=$HTTP_POST_VARS['attachtypes'];
			$smtpmail=$HTTP_POST_VARS['smtpmail'];
			WriteSettings();
			$announce=$HTTP_POST_VARS['announce'];
			$fp = @fopen($dbpath."/announce.txt", "w");
			@fputs($fp, $announce);
			@fclose($fp);
			$sprache=$HTTP_POST_VARS['language'];
			$mainpath=addslashes($mainpath);
			$dbpath=addslashes($dbpath);
			$temppath=addslashes($temppath);
			$homeurl=addslashes($homeurl);
			$content2 = "<?php\n\n";
			$content2 .="\$mainpath=\"$mainpath\";\n";
			$content2 .="\$dbpath=\"$dbpath\";\n";
			$content2 .="\$temppath=\"$temppath\";\n";
			$content2 .="\$language=\"$sprache\";\n";
			$content2 .="\$homeurl=\"$homeurl\";\n";
			$content2 .="\$cookieid=\"$cookieid\";\n\n?";
			$content2 .=">";
			$fp = fopen("$mainpath/global.php", "w");
			fputs($fp, $content2);
			fclose($fp);
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=fsettings\">";
		} elseif ($do=="fsettings2") {
			FAccessDenied();
		}

		if ($do=="predefstyles2" && $admin=="1") {                    //get a set of predefined styles and install them - action
			$stylefname=$HTTP_POST_VARS['defstyle'];
			$filename=$dbpath."/settings/styles/styles.php";
			if (!is_writeable($filename)){
				ErrorMessage($VNotWriteable,$liusername);
			}else{
				copy("$dbpath/settings/styles/$stylefname",$filename);
				echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=predefstyles\">";
			}
		}elseif($do=="predefstyles2"){
			FAccessDenied();
		}

		if ($do=="styles2" && $admin=="1") {                    //edit styles - action
			$picturebuttons=$HTTP_POST_VARS['picturebuttons'];
			$font=$HTTP_POST_VARS['font'];
			$fontcolor=CheckColorCode($HTTP_POST_VARS['fontcolor']);
			$fontsize=$HTTP_POST_VARS['fontsize'];
			$quotefont=$HTTP_POST_VARS['quotefont'];
			$quotefontcolor=CheckColorCode($HTTP_POST_VARS['quotefontcolor']);
			$quotefontsize=$HTTP_POST_VARS['quotefontsize'];
			$quotebackground=CheckColorCode($HTTP_POST_VARS['quotebackground']);
			$quoteborder=$HTTP_POST_VARS['quoteborder'];
			$annfont=$HTTP_POST_VARS['annfont'];
			$annfontcolor=CheckColorCode($HTTP_POST_VARS['annfontcolor']);
			$annfontsize=$HTTP_POST_VARS['annfontsize'];
			$background=CheckColorCode($HTTP_POST_VARS['background']);
			$form=$HTTP_POST_VARS['form'];
			$formborder=$HTTP_POST_VARS['formborder'];
			$formbordersize=$HTTP_POST_VARS['formbordersize'];
			$forumheaderborder=CheckColorCode($HTTP_POST_VARS['forumheaderborder']);
			$forumheaderbordersize=$HTTP_POST_VARS['forumheaderbordersize'];
			$forumheader=CheckColorCode($HTTP_POST_VARS['forumheader']);
			$forumheadertitle=CheckColorCode($HTTP_POST_VARS['forumheadertitle']);
			$forumheadertitlesize=$HTTP_POST_VARS['forumheadertitlesize'];
			$forumheadercaption=CheckColorCode($HTTP_POST_VARS['forumheadercaption']);
			$forumheaderwelcome=CheckColorCode($HTTP_POST_VARS['forumheaderwelcome']);
			$forumheadermenu=CheckColorCode($HTTP_POST_VARS['forumheadermenu']);
			$forumheadermenufont=$HTTP_POST_VARS['forumheadermenufont'];
			$datecolor=CheckColorCode($HTTP_POST_VARS['datecolor']);
			$containerborder=CheckColorCode($HTTP_POST_VARS['containerborder']);
			$containerbordersize=$HTTP_POST_VARS['containerbordersize'];
			$containerinner=CheckColorCode($HTTP_POST_VARS['containerinner']);
			$headercolor=CheckColorCode($HTTP_POST_VARS['headercolor']);
			$headergif=$HTTP_POST_VARS['headergif'];
			$headerfont=CheckColorCode($HTTP_POST_VARS['headerfont']);
			$subheadercolor=CheckColorCode($HTTP_POST_VARS['subheadercolor']);
			$subheadergif=$HTTP_POST_VARS['subheadergif'];
			$subheaderfont=$HTTP_POST_VARS['subheaderfont'];
			$menucolor=CheckColorCode($HTTP_POST_VARS['menucolor']);
			$fmenucolor=CheckColorCode($HTTP_POST_VARS['fmenucolor']);
			$menufont=$HTTP_POST_VARS['menufont'];
			$forumbuttoncolor=CheckColorCode($HTTP_POST_VARS['forumbuttoncolor']);
			$forumbuttonover=CheckColorCode($HTTP_POST_VARS['forumbuttonover']);
			$forumbuttontopics=CheckColorCode($HTTP_POST_VARS['forumbuttontopics']);
			$forumbuttonreplies=CheckColorCode($HTTP_POST_VARS['forumbuttonreplies']);
			$forumbuttonlast=CheckColorCode($HTTP_POST_VARS['forumbuttonlast']);
			$admincolor=CheckColorCode($HTTP_POST_VARS['admincolor']);
			$modcolor=CheckColorCode($HTTP_POST_VARS['modcolor']);
			$usercolor=CheckColorCode($HTTP_POST_VARS['usercolor']);
			$linkcolor=CheckColorCode($HTTP_POST_VARS['linkcolor']);
			$vlinkcolor=CheckColorCode($HTTP_POST_VARS['vlinkcolor']);
			$hlinkcolor=CheckColorCode($HTTP_POST_VARS['hlinkcolor']);
			$buttonchoice=CheckColorCode($HTTP_POST_VARS['buttonchoice']);
			$groupcolors=$HTTP_POST_VARS['groupcolors'];
			$group1col=CheckColorCode($HTTP_POST_VARS['group1color']);
			$group2col=CheckColorCode($HTTP_POST_VARS['group2color']);
			$group3col=CheckColorCode($HTTP_POST_VARS['group3color']);
			$group4col=CheckColorCode($HTTP_POST_VARS['group4color']);
			$group5col=CheckColorCode($HTTP_POST_VARS['group5color']);
			$group10col=CheckColorCode($HTTP_POST_VARS['group10color']);
			$group11col=CheckColorCode($HTTP_POST_VARS['group11color']);
			$group12col=CheckColorCode($HTTP_POST_VARS['group12color']);
			$stylepref=$HTTP_POST_VARS['stylepref'];
			WriteStyles();
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=styles\">";
		} elseif ($do=="styles2") {
			FAccessDenied();
		}

		if ($do=="temp2" && $admin=="1") {              //edit language files - action
			$te=$HTTP_GET_VARS['te'];
			$cont=$HTTP_POST_VARS['cont'];
			$cont=stripslashes($cont);
			$fp = fopen("$mainpath/templates/pb/language/$te", "w");
			fputs($fp, $cont);
			fclose($fp);
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=templates\">";
		} elseif ($do=="temp2") {
			FAccessDenied();
		}

		if ($do=="mg2" && $admin=="1") {                //edit member groups
			$range1=$HTTP_POST_VARS['range1'];
			$range2=$HTTP_POST_VARS['range2'];
			$range3=$HTTP_POST_VARS['range3'];
			$range4=$HTTP_POST_VARS['range4'];
			$range5=$HTTP_POST_VARS['range5'];
			$range6=$HTTP_POST_VARS['range6'];
			$range7=$HTTP_POST_VARS['range7'];
			$one=$HTTP_POST_VARS['one'];
			$two=$HTTP_POST_VARS['two'];
			$three=$HTTP_POST_VARS['three'];
			$four=$HTTP_POST_VARS['four'];
			$five=$HTTP_POST_VARS['five'];
			$ten=$HTTP_POST_VARS['ten'];
			$eleven=$HTTP_POST_VARS['eleven'];
			$twelve=$HTTP_POST_VARS['twelve'];
			$mod=$HTTP_POST_VARS['mod'];
			$adm=$HTTP_POST_VARS['admin'];
			$ban=$HTTP_POST_VARS['ban'];
			$friend=$HTTP_POST_VARS['friend'];
			$rankstyle=$HTTP_POST_VARS['rankimg'];
			$fp = fopen("$dbpath/status/1", "w");
			fputs($fp, $one);
			fclose($fp);
			$fp = fopen("$dbpath/status/2", "w");
			fputs($fp, $two);
			fclose($fp);
			$fp = fopen("$dbpath/status/3", "w");
			fputs($fp, $three);
			fclose($fp);
			$fp = fopen("$dbpath/status/4", "w");
			fputs($fp, $four);
			fclose($fp);
			$fp = fopen("$dbpath/status/5", "w");
			fputs($fp, $five);
			fclose($fp);
			$fp = fopen("$dbpath/status/6", "w");
			fputs($fp, $mod);
			fclose($fp);
			$fp = fopen("$dbpath/status/7", "w");
			fputs($fp, $adm);
			fclose($fp);
			$fp = fopen("$dbpath/status/8", "w");
			fputs($fp, $ban);
			fclose($fp);
			$fp = fopen("$dbpath/status/9", "w");
			fputs($fp, $friend);
			fclose($fp);
			$fp = @fopen("$dbpath/status/10", "w");
			fputs($fp, $ten);
			fclose($fp);
			$fp = @fopen("$dbpath/status/11", "w");
			fputs($fp, $eleven);
			fclose($fp);
			$fp = @fopen("$dbpath/status/12", "w");
			fputs($fp, $twelve);
			fclose($fp);
			$fp = @fopen("$dbpath/status/ranges", "w");
			$ranges .= "<?php\n\n";
			$ranges .= "\$range1=$range1;\n";
			$ranges .= "\$range2=$range2;\n";
			$ranges .= "\$range3=$range3;\n";
			$ranges .= "\$range4=$range4;\n";
			$ranges .= "\$range5=$range5;\n";
			$ranges .= "\$range6=$range6;\n";
			$ranges .= "\$range7=$range7;\n";
			$ranges .= "\n?";
			$ranges .= ">";
			fputs($fp, $ranges);
			fclose($fp);
			$fp = @fopen("$dbpath/status/styles", "w");
			fputs($fp, $rankstyle);
			fclose($fp);
			copy("$temppath/pb/images/ranks/$rankstyle/rank1.gif","$temppath/pb/images/ranks/rank1.gif");
			copy("$temppath/pb/images/ranks/$rankstyle/rank2.gif","$temppath/pb/images/ranks/rank2.gif");
			copy("$temppath/pb/images/ranks/$rankstyle/rank3.gif","$temppath/pb/images/ranks/rank3.gif");
			copy("$temppath/pb/images/ranks/$rankstyle/rank4.gif","$temppath/pb/images/ranks/rank4.gif");
			copy("$temppath/pb/images/ranks/$rankstyle/rank5.gif","$temppath/pb/images/ranks/rank5.gif");
			copy("$temppath/pb/images/ranks/$rankstyle/rank10.gif","$temppath/pb/images/ranks/rank10.gif");
			copy("$temppath/pb/images/ranks/$rankstyle/rank11.gif","$temppath/pb/images/ranks/rank11.gif");
			copy("$temppath/pb/images/ranks/$rankstyle/rank12.gif","$temppath/pb/images/ranks/rank12.gif");
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=mgroups\">";
		} elseif ($do=="mg2") {
			FAccessDenied();
		}

		if ($do=="ca2" && $admin=="1") {                        //add a category - action
			$cname=stripslashes($HTTP_POST_VARS['name']);
			$cdescription=stripslashes($HTTP_POST_VARS['dec']);
			$filename = "$dbpath/cats/cats";
			$fd = fopen ($filename, "r");
			$cats = fread ($fd, filesize ($filename));
			fclose ($fd);
			$cats++;
			$fp = fopen("$dbpath/cats/cats", "w");
			fputs($fp, $cats);
			fclose($fp);
			$cforums=0;
			$cname=textparse($cname);
			$cdescription=textparse($cdescription);
			WriteCatInfo($cats);
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=catadd\">";
		} elseif ($do=="ca2") {
			FAccessDenied();
		}

		if ($do=="cedit" && $admin=="1") {              //edit a category - action
			$cname=$HTTP_POST_VARS['nam'];
			$cdescription=$HTTP_POST_VARS['dec'];
			$cforums=$HTTP_POST_VARS['cforums'];
			$cnum=$HTTP_POST_VARS['cnum'];
			WriteCatInfo($cnum);
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=editcat\">";

		} elseif ($do=="cedit") {
			FAccessDenied();
		}

		if ($do=="fedit" && $admin=="1") {
			$cnum=$HTTP_POST_VARS['cnum'];
			$fnum=$HTTP_POST_VARS['fnum'];
			SetLock();
			include($dbpath."/cats/".$cnum."_".$fnum);
			$fname=stripslashes($HTTP_POST_VARS['fname']);
			$fdescription=stripslashes($HTTP_POST_VARS['fdesc']);
			$users=$HTTP_POST_VARS['accesscontrol'];
			$usersnr=count($users);
			if ($usersnr=="0" && trim($users[0])==""){
				$fusers="|all";
			}else{
				$fusers="|".@implode("|",$users);
			}
			$fname=textparse($fname);
			$fdescription=textparse($fdescription);

			WriteForumInfo($cnum,$fnum);
			@unlink ($dbpath.'/block');

			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=editforum\">";

		} elseif ($do=="fedit") {
			FAccessDenied();
		}

		if ($do=="fa2" && $admin=="1") {                //add a forum - action
			$fname=stripslashes($HTTP_POST_VARS['nam']);
			$fdescription=stripslashes($HTTP_POST_VARS['dec']);
			$cat=$HTTP_POST_VARS['cat'];
			$users=$HTTP_POST_VARS['accesscontrol'];
			$lockforum=$HTTP_POST_VARS['lockforum'];
			$allowrep=$HTTP_POST_VARS['allowreply'];
			$usersnr=count($users);
			if ($usersnr=="0" && trim($users[0])==""){
				$fusers="|all";
			}else{
				$fusers="|".@implode("|",$users);
			}

			$fname=textparse($fname);
			$fdescription=textparse($fdescription);
			if ($lockforum){
				$flock="locked";
			}else{
				$flock="unlocked";
			}
			if ($allowrep){
				$fallowreply="1";
			}else{
				$fallowreply="0";
			}
			$flastpost="";
			$flastauthor="";
			$flasturl="";
			$freplies=0;
			$ftopics=0;
			$fvisitors="";
			$flastpostnumber=0;

			$filename = $dbpath."/cats/".$cat;
			include($filename);
			$cforums++;
			WriteCatInfo($cat);

			WriteForumInfo($cat,$cforums);
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=forumadd\">";
		} elseif ($do=="fa2") {
			FAccessDenied();
		}

		if ($do=="emmembers2" && $admin=="1") {         //email members - action
			$sub=$HTTP_POST_VARS['sub'];
			$cont=$HTTP_POST_VARS['cont'];
			$via=$HTTP_POST_VARS['via'];
			$handle = opendir('db/memss/');
			while ($file = readdir($handle)) {
				if ($file != "." && $file != ".." && $file != ".htaccess") {
					if ($via=="pm") {
						$to=$file;
						$filename = "$dbpath/pm/$to"."_tot";
						if (!file_exists($filename)){
							$filename=str_replace("_"," ",$filename);
						}
						$fd = fopen ($filename, "r");
						$num = fread ($fd, filesize ($filename));
						fclose ($fd);
						$num++;
						$filename=$dbpath."/pm/".$to."_tot";
						$fp = fopen($filename, "w");
						fputs($fp, $num);
						fclose($fp);
						$filename = $dbpath."/pm/".$to;
						$fd = fopen ($filename, "r");
						$num = fread ($fd, filesize ($filename));
						fclose ($fd);
						$num++;

						$fp = fopen($filename, "w");
						fputs($fp, $num);
						fclose($fp);

						setlocale(LC_TIME,$LangLocale);
						$date = time()+($timezone*3600);
						$fp = fopen($dbpath.'/pm/'.$to.'_'.$num.'_s', "w");
						fputs($fp, $sub);
						fclose($fp);
						$fp = fopen($dbpath.'/pm/'.$to.'_'.$num.'_a', "w");
						fputs($fp, $liusername);
						fclose($fp);
						$fp = fopen($dbpath.'/pm/'.$to.'_'.$num.'_d', "w");
						fputs($fp, $date);
						fclose($fp);
						$fp = fopen($dbpath.'/pm/'.$to.'_'.$num.'_c', "w");
						fputs($fp, $cont);
						fclose($fp);
						$fp = fopen($dbpath.'/pm/'.$to.'_'.$num.'_pmstat', "w");
						fputs($fp, "1");
						fclose($fp);
					}
					if ($via=="email") {
						include($dbpath."/members/".$file);
						$cont=stripslashes($cont);
						if ($emsupp=="1"){
							$header=mailheader();
							mail($useremail,$sub,$cont,$header);
						}elseif($smtpmail){
							SendMail($useremail,$sub,$cont);
						}
					}
				}
			}
			closedir($handle);
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php\">";
		} elseif ($do=="emmembers2") {
			FAccessDenied();
		}

		if ($do=="remove2" && $admin=="1") {                    //remove members - action
			$olduser=$HTTP_POST_VARS['user'];
			$success=0;
			$filename="$dbpath/online/$olduser";
			if (file_exists($filename)){
				ErrorMessage($VUserIsOnline,$liusername);
				exit;
			}
			$filename="$dbpath/members/$olduser";
			if (file_exists($filename))
				{unlink ($filename);
				$success=1;
			}
			$filename="$dbpath/memss/$olduser";
			if (file_exists($filename))
				{unlink ($filename);
			}
			$filename = "$dbpath/mems";
			$fd = fopen ($filename, "r");
			$n = fread ($fd, filesize ($filename));
			fclose ($fd);
			$n--;
			$fp = fopen("$dbpath/mems", "w");
			fputs($fp, $n);
			fclose($fp);
				$handle = opendir('db/pm/');		//remove all data related to private messages for this member
				while ($file = readdir($handle)) {
					if ($olduser==substr($file,0,strlen($olduser))) {
						unlink($dbpath.'/pm/'.$file);
					}
				}
				closedir($handle);
			$filename="$dbpath/settings/newestm";
			$fd=fopen($filename,"r");
			$lastuser=fread($fd,filesize($filename));
			fclose($fd);
			if ($olduser==$lastuser){          //this needs to be done better once it's easier to figure out who was the last user joined
				$handle = opendir('db/members/');
				while ($file = readdir($handle)) {
					if ($file != "." && $file != ".." && $file != "index.html" && $file != ".htaccess" && $file !="backup" && $file !="pending") {
						include("$dbpath/members/$file");
						if ($userjoined>$uoldjoined){
							$newestmember=$username;
							$uoldjoined=$userjoined;
						}
					}
				}
				closedir($handle);
				$fp=fopen("$dbpath/settings/newestm","w");
				fputs($fp,$newestmember);
				fclose($fp);
			}
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=remove\">";
		} elseif ($do=="remove2") {
			FAccessDenied();
		}

		if ($do=="grantm2" && $admin=="1") {           //only admin may grant moderator or admin status
			$u=$HTTP_POST_VARS['user'];
			$s=$HTTP_POST_VARS['stat'];
			$filet=$dbpath."/members/".$u;
			if (file_exists($filet) && $u!="") {
				if ($s=="remstatus"){
					include($filet);
					updatestatus($u);
					$usermod="0";
					$useradmin="0";
					$userban="0";
					$userfriend="0";
				}else{
					include($filet);
					$userrank=$s;
					switch ($userrank){
						case "6":
							$usermod="1";$useradmin="0";$userban="0";$userfriend="0";
							break;
						case "7":
							$usermod="0";$useradmin="1";$userban="0";$userfriend="0";
							break;
						case "8":
							$usermod="0";$useradmin="0";$userban="1";$userfriend="0";
							break;
						case "9":
							$usermod="0";$useradmin="0";$userban="0";$userfriend="1";
							break;
					}
				}
				WriteUserInfo($u);
				echo "<meta http-equiv=\"Refresh\" content=\"0; URL=admin.php?do=grantm\">";
			} else {
				echo "<td height=\"399\" bgcolor=\"$color1\" width=\"81%\" valign=\"top\"><table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
				echo "<tr> <td><font size=\"1\">$InvalidUsername!</font></td></tr></table>";
			}
		} elseif ($do=="grantm2") {
			FAccessDenied();
		}

		echo "</td></tr></table></td></tr></table></td></tr></table>";
		writefooter($newestm);
	}
}
ob_end_flush();
?>

