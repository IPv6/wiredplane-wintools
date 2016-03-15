<?php
/***************************************************************************
 *                            ucp.php - PBLang
 *                            -------------------
*              see docs/copyright.txt for more details
*
*			Last modified 10 October 2003
 ***************************************************************************/

require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once("scan.php");

$liusername=$HTTP_COOKIE_VARS[$cookieid];
if($liusername=="Anonimus" || $liusername=="Anonymous"){
	echo '<meta http-equiv="Refresh" content="0; URL=index.php">';
	ob_end_flush(); 
	exit();
}
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1") {
	$language=SetLanguage($liusername);
	include($temppath.'/pb/language/lang_'.$language.'.php');

	$reg=$HTTP_GET_VARS['id'];
	$te=$HTTP_GET_VARS['te'];
	writeheader($newestm,0,0,0,$liusername,$loggedin,$VUserCP);
	$uname=$liusername;
	if ($reg!="2") {
		if (trim($te)=="") {
		} else {
			if ($admin=="1") {
					$uname=$te;
			}
		}
		if ($loggedin=="0") {
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=login.php\">";
			exit;
		} else {
//               ucp($uname,$language);
			include($dbpath."/members/".$uname);
			$realname=$userrealname;
			$alias=$useralias;
			$loc=$userlocation;
			$msn=$usermsn;
			$aim=$useraim;
			$yah=$useryahoo;
			$em=$useremail;
			$emhide=$useremhide;
			$pt=str_replace("<br>","\n",$userslogan);
			$pt=str_replace("<BR>","\n",$userslogan);
			$sig=$usersig;
//			$sig=stripslashes($sig);
//			$sig=textparse(textparse1($sig));
//			$sig=replacestuff($sig);
			$sig=str_replace("<br>","\n",$sig);
			$sig=str_replace("<BR>","\n",$sig);
			$uav=$useravatar;
			$uwebav=$userwebavatar;
			$web=$userhomepage;
			$icq=$usericq;
			$qq=$userqq;
			$animsm=$useranimsmilies;
			$userlng=$userlang;

			echo "<script language=\"JavaScript1.2\" src=\"ubbc.js\" type=\"text/javascript\"></script>";
			WriteTableTop();
			echo "\n";
			echo "<tr><td class=\"header\" colspan=\"2\">";
			echo "<a href=\"index.php\">$sitetitle</a> :: $VUserControlPanel";
			echo "</td></tr>\n<form method=\"post\" action=\"ucp.php?id=2&user=$uname\">";
			echo "<tr>";
			echo "<td class=\"subheader\" colspan=\"2\">$VPersonal $VSettings :: </td></tr>\n";
			echo "<tr><td class=\"label\">$VNewPassword:<br></td>";
			echo "<td class=\"content\"><input type=\"password\" name=\"npass\"></td></tr>\n";
			echo "<tr><td class=\"label\">$VRepeatPassword:<br></td>";
			echo "<td class=\"content\"><input type=\"password\" name=\"npass2\"></td></tr>\n";
			echo "<tr><td class=\"label\">$VRealname:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"realname\" value=\"$realname\"></td></tr>\n";
			if ($allowalias=="1" || $admin=="1"){
				echo "<tr><td class=\"label\">$VAlias:<br></td>";
				echo "<td class=\"content\"><input type=\"text\" name=\"alias\" value=\"$alias\">&nbsp;$VExplainAlias</td></tr>\n";
			}
			echo "<tr><td class=\"label\">$VLocation:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"loc\" value=\"$loc\"></td></tr>\n";
			echo "<tr><td class=\"label\">MSN:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"msn\" value=\"$msn\"></td></tr>\n";
			echo "<tr><td class=\"label\">ICQ $VNumber:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"icq\" value=\"$icq\"></td></tr>\n";
			echo "<tr><td class=\"label\">AIM:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"aim\" value=\"$aim\"></td></tr>\n";
			echo "<tr><td class=\"label\">Yahoo:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"yah\" value=\"$yah\"></td></tr>\n";
			echo "<tr><td class=\"label\">QQ:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"qq\" value=\"$qq\"></td></tr>\n";
			echo "<tr><td class=\"label\">$VWebsite:<br></td>";
			echo "<td class=\"content\"><input type=\"text\" name=\"web\" value=\"$web\"></td></tr>\n";
			echo "<tr><td class=\"label\">$VEmail:<br>";
			echo $VHideEmail."</font>";
			if ($emhide=="hide"){
				echo "<input type=\"checkbox\" name=\"emhide\" value=\"hide\" checked>";
			}else{
				echo "<input type=\"checkbox\" name=\"emhide\" value=\"hide\">";
			}

			echo "</td><td class=\"content\"><input type=\"text\" name=\"em\" value=\"$em\"></td></tr>\n";
			echo "<tr><td class=\"subheader\" colspan=\"2\">$VUser $VOptions :: </td></tr>\n";
			echo "<tr><td class=\"label\">$VPersonalText:<br></td>
					<td class=\"content\"><input type=\"text\" name=\"pt\" value=\"$pt\"></td></tr>\n";
			if ($ave=="1") {
				echo "\n<SCRIPT LANGUAGE=\"JavaScript\">\n";
				echo "function avchange(URL_List)\n";
				echo "{\n","var URL = URL_List.options[URL_List.selectedIndex].value;\n","document.avatari.src=\"templates/pb/images/avatars/\"+URL;\n";
				echo "}\n","</SCRIPT>\n";
				echo "<tr><td class=\"label\">$VAvatar:<br></td>";
				echo "<td height=\"25\" width=\"80%\" bgcolor=\"$menucolor\"><select name=\"av\" OnChange=\"avchange(this);\">";
				if (trim($uav)=="" && $uwebav==""){
					echo "<option value=\"none\" selected=\"selected\">$VNoAvatar</option>";
				}else{
					echo "<option value=\"none\">$VNoAvatar</option>";
				}
				$dir=$temppath."/pb/images/avatars/";
				if (file_exists($dir)){
					$handle = opendir("$dir");
					while ($file = readdir($handle)) {
						if ($file != "." && $file != "..") {
							echo "<option value=\"$file\"";
							if ($file==$uav) {
								echo "selected=\"selected\"";
							}
						echo ">$file</option>\n";
						}
					}
					closedir($handle);
				}
				if ($webave){
					if ($uwebav!=""){
							echo "<option value=\"webav\" selected>$VURLAvatar</option>";
					}else{
							echo "<option value=\"webav\">$VURLAvatar</option>";
					}
				}
				echo '</select>';
				if ($uav!="" && $uav!="none" && $uav!="webav"){
					echo "&nbsp;<img src=\"templates/pb/images/avatars/$uav\" NAME=\"avatari\" alt=\"$VImage\">";
				}elseif($uwebav!=""){
					echo "&nbsp;<img src=\"$uwebav\" NAME=\"$VAvatar $VImage\" alt=\"$VImage\">";
				}
				echo "</td></tr>\n";
				if ($webave){
					echo "<tr><td class=\"label\">$VURLAvatar:<br></td>";
					echo "<td class=\"content\"><input type=\"text\" name=\"webav\" value=\"$uwebav\">$AvatarURLtip</td></tr>\n";
				}
			}


			echo "<tr><td class=\"label\">$VEnableAnimSmilies:<br></td>";
			echo "<td height=\"25\" width=\"80%\" bgcolor=\"$menucolor\">";
			if ($animsm=="1"){
				echo "<input type=\"checkbox\" name=\"animsm\" value=\"1\" checked>";
			}else{
				echo "<input type=\"checkbox\" name=\"animsm\" value=\"1\">";
			}
			echo "</td></tr>\n";

			echo "<tr><td class=\"label\">$VDisableAvatars:<br></td>";
			echo "<td height=\"25\" width=\"80%\" bgcolor=\"$menucolor\">";
			if ($noavatars=="1"){
				echo "<input type=\"checkbox\" name=\"noavatars\" value=\"1\" checked>";
			}else{
				echo "<input type=\"checkbox\" name=\"noavatars\" value=\"1\">";
			}
			echo "</td></tr>\n";

			echo "<tr><td class=\"label\">$VLanguageSelection:<br></td>";
			echo "<td height=\"25\" width=\"80%\" bgcolor=\"$menucolor\"><select name=\"ulang\">";
			LanguageSelection($userlng);
			echo "</select> $langexpl</td></tr>\n";

			echo "<tr>";
			/*
			echo "<td class=\"label\">$VSignature: <BR>
			(<a href=\"help.php\">$PBCodeAllowed</a>)<br></td>\n";
			echo "<td class=\"content\"><textarea name=\"sig\" cols=\"60\" rows=\"4\">$sig</textarea></td></tr>\n";
			*/
			echo "<tr><td class=\"subheader\" colspan=\"2\">$VSubmitForm :: </td></tr>\n";

			echo "<tr><td class=\"label\"><br></td>";
			echo "<td class=\"content\"><input type=\"submit\" name=\"Submit2\" value=\"$VSubmit\">\n";
			sbot(1);

		}
	} else {
		$user=$HTTP_GET_VARS['user'];
		if($user=="Anonimus" || $user=="Anonymous"){
			echo '<meta http-equiv="Refresh" content="0; URL=index.php">';
			ob_end_flush(); 
			exit();
		}		
		include($dbpath."/members/".$user);
		$userrealname=$HTTP_POST_VARS['realname'];
		$alias=$HTTP_POST_VARS['alias'];
		$pass=$HTTP_POST_VARS['npass'];
		$pass2=$HTTP_POST_VARS['npass2'];
		if ($pass!=$pass2){
			ErrorMessage($PWNoMatch,$liusername);
			exit;
		}elseif (trim($pass!="")) {
			$password=md5($pass);
		}
		$usersig=textparse(stripslashes($HTTP_POST_VARS['sig']));
		$userlocation=stripslashes($HTTP_POST_VARS['loc']);
		$usermsn=$HTTP_POST_VARS['msn'];
		$useraim=$HTTP_POST_VARS['aim'];
		$useryahoo=$HTTP_POST_VARS['yah'];
		$userhomepage=$HTTP_POST_VARS['web'];
		$userqq=$HTTP_POST_VARS['qq'];
		$usericq=$HTTP_POST_VARS['icq'];
		$userwebavatar=$HTTP_POST_VARS['webav'];
		$useremail=$HTTP_POST_VARS['em'];
		$useremhide=$HTTP_POST_VARS['emhide'];
		$av=$HTTP_POST_VARS['av'];
		$noavatars=$HTTP_POST_VARS['noavatars'];
		if ($av==$userwebavavatar){
			$useravatar="webav";
		}else{
			$useravatar=$av;
		}
		$userlang=$HTTP_POST_VARS['ulang'];
		$useranimsmilies=$HTTP_POST_VARS['animsm'];
		$userslogan=stripslashes($HTTP_POST_VARS['pt']);
		$userslogan=textparse($userslogan);

		if ($alias!=$useralias){
			if ($allowalias){
				$mindate=$lastaliaschange+($changealias*86400);
				if (time()<$mindate && $admin!="1"){
					ErrorMessage($ChangeAliasNotAllowed,$liusername);
					exit;
				}else{
					if (strpos($aliaslist,$liusername)){
						$allowaliaschange='1';
						$usedalias='1';
					}else{
						$allowaliaschange=CheckAlias($alias);
					}
					if ($allowaliaschange=="1"){
						$useralias=$alias;
						$lastaliaschange=time();
						AddAlias($alias,$useralias);
						if (!$usedalias){
							$aliaslist=$aliaslist.'|'.$useralias;
						}
					}else{
						ErrorMessage($AliasAlreadyInUse,$liusername);
						exit;
					}
				}
			}
		}
		WriteUserInfo($user);
		echo "<meta http-equiv=\"Refresh\" content=\"0; URL=profile.php?u=$user\">";
		msg("$VUpdated","$user $ProfileUpdated!");
	}
	writefooter($newestm);
}
ob_end_flush();
?>


