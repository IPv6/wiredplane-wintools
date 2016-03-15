<?php
/***************************************************************************
 *                            register.php  - PBLang
 *                            -------------------
 *              see docs/copyright.txt for more details
 *
 * 			Modified: 10 October 2003
***************************************************************************/


require('header.php');
include("global.php");
include($dbpath."/settings.php");
include_once("functions.php");
include_once("ffunctions.php");
include($dbpath."/settings/styles/styles.php");
include($temppath."/pb/language/lang_".$language.".php");
$orglang=$language;

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"0","1");
if ($permit=="1") {
	if ($loggedin=="1" && $admin=="0") {
		ErrorMessage($AlreadyMember,$liusername);
		exit;
	} else {
		if ($allowreg!="1" && $admin=="0") {
			ErrorMessage($NoNewUsers,$liusername);
			exit;
		} else {
			$reg=$HTTP_GET_VARS['reg'];
			writeheader($newestm,0,0,0,$liusername,$loggedin,$VRegister);
			if ($reg=="1") {
				$lang=trim($HTTP_POST_VARS['lang']);
				include($temppath."/pb/language/lang_".$lang.".php");
				//                    register($admin,$lang);
				WriteTableTop();
				echo "<form method=\"post\" action=\"register.php?reg=2\"><tr>";
				echo "<td class=\"header\" colspan=\"2\">$sitetitle :: $VRegister</td></tr>";
				echo "<tr><td class=\"subheader\" colspan=\"2\">$VUser $VOptions :: </td></tr>";

				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\"><font color=\"red\">*</font>
					<font color=\"$menufont\">$VUsername:</font></td>";
				echo "<td height=\"25\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"user\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\"><font color=\"red\">*</font>
					<font color=\"$menufont\">$VPassword:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"password\" name=\"pass\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\"><font color=\"red\">*</font>
					<font color=\"$menufont\">$VPassword $VAgain:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"password\" name=\"pass2\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\"><font color=\"red\">*</font>
					<font color=\"$menufont\">$VEmail:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"em\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">$VHideEmail:</td>
				<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"checkbox\" name=\"emhide\" value=\"hide\"></td></tr>";

				echo "<tr><td class=\"subheader\" colspan=\"2\">$VProfile $VOptions :: </td></tr>";

				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">$VRealname:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"realname\" size=\"30\"></td></tr>";
				if ($allowalias=="1"){
					echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
						<font color=\"$menufont\">$VAlias:</font></td>";
					echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"alias\" size=\"20\">&nbsp;$VExplainAlias</td></tr>";
				}
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">MSN Messenger:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"msn\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">ICQ $VNumber:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"icq\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">AIM Nickname:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"aim\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">Yahoo Messenger:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"yahoo\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\"><font color=\"$menufont\">QQ:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"qq\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">$VWebsite:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"web\" value=\"http://\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">$VLocation:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"loc\"></td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">$VSlogan:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"text\" name=\"pt\" value=\"$defaultslogan\" size=\"30\">&nbsp;$VExplainSlogan</td></tr>";
				if ($ave=="1") {
					echo "\n<SCRIPT LANGUAGE=\"JavaScript\" type=\"text/javascript\">\n";
					echo "function avchange(URL_List)\n";
					echo "{\n","var URL = URL_List.options[URL_List.selectedIndex].value;\n","document.avatari.src=\"templates/pb/images/avatars/\"+URL;\n";
					echo "}\n</SCRIPT>\n";
					echo "<tr bgcolor=\"$fmenucolor\"><td height=\"25\" width=\"20%\"><font color=$menufont>$VAvatar:<br></font></td>\n";
					$uav="none";
					echo "<td height=\"25\" width=\"80%\" bgcolor=$menucolor><select name=\"av\" OnChange=\"avchange(this);\">";
					echo "<option value=\"none\" selected=\"selected\">$VNoAvatar</option>";
					$dir=$temppath."/pb/images/avatars/";
					if (file_exists($dir)){
						$handle = opendir("$dir");
						while ($file = readdir($handle)) {
							if ($file != "." && $file != "..") {
								echo "<option value=\"$file\">$file</option>\n";
							}
						}
						closedir($handle);
					}
					if ($webave){
						echo "<option value=\"webav\">$VURLAvatar</option>";
					}
					echo "</select>";
					echo "&nbsp;&nbsp;<img src=\"templates/pb/images/avatars/$uav\" NAME=\"avatari\" alt=\"$VImage\"></td></tr>";
/*					if ($uav!="" && $uav!="none" && $uav!="webav"){
						echo "&nbsp;&nbsp;<img src=\"templates/pb/images/avatars/$uav\" NAME=\"avatari\" alt=\"$VImage\"></td></tr>";
					}
*/					if ($webave){
						echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" width=\"20%\"><font color=\"$menufont\">$VURLAvatar:<br></font></td>\n";
						echo "<td height=\"18\" width=\"80%\" bgcolor=\"$menucolor\"><input type=\"text\" name=\"webav\" value=\"$uwebav\">$AvatarURLtip</td></tr>";
					}
				}
				/*echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">$VSignature:</font></td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><textarea name=\"sig\" cols=\"30\" rows=\"4\"></textarea></td></tr>";*/
				$regcode=time();
				echo "<input type=\"hidden\" name=\"regcode\" value=\"$regcode\">";
				echo "<input type=\"hidden\" name=\"lang\" value=\"$lang\">";

				echo "<tr bgcolor=\"$fmenucolor\"><td class=\"subheader\" colspan=\"2\">$VRules :: </td></tr>";
				if (file_exists("termsofuse"."_$lang".".txt")){
					$termsfile="termsofuse"."_$lang".".txt";
				}else{
					$termsfile="termsofuse_en.txt";
				}
				$ruleset=file($termsfile);
				$rules=implode(" ",$ruleset);
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\" colspan=\"2\">
					<font color=\"$menufont\">$rules</font><br>
					</td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">
					<font color=\"$menufont\">$VAcceptRules:</td>
					<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"checkbox\" name=\"accept\" value=\"1\"></td></tr>";
				echo "<tr><td  class=\"subheader\" colspan=\"2\">$VSubmitForm :: </td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">&nbsp;</td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\">&nbsp;";
				echo "<input type=\"reset\" name=\"Submit2\" value=\"$VReset\"></td></tr></form></table></td></tr></table></td></tr></table>";
				echo "<BR><font color=\"red\">* = $VRequired</font>";
			}elseif($reg!="2" && $reg!="1") {
				WriteTableTop();
				echo "<form method=\"post\" action=\"register.php?reg=1\"><tr>";
				echo "<td class=\"header\" colspan=\"2\">$sitetitle :: $VRegister</td></tr>";

				echo "<tr><td class=\"subheader\" colspan=\"2\">$VUser $VOptions :: </td></tr>";

				echo "<tr><td class=\"label\">Select your language/$VLanguageSelection:<br></td>";
				echo "<td class=\"content\"><select name=\"lang\">";
				LanguageSelection($userlng);
				echo "</select> $langexpl</td></tr>\n";

				echo "<tr bgcolor=\"$fmenucolor\"><td class=\"subheader\" colspan=\"2\">$VSubmitForm :: </td></tr>";
				echo "<tr bgcolor=\"$fmenucolor\"><td height=\"18\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"30%\" valign=\"middle\">&nbsp;</td>";
				echo "<td height=\"18\" bgcolor=\"$menucolor\" width=\"70%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\">&nbsp;";
				echo "<input type=\"reset\" name=\"Submit2\" value=\"$VReset\"></td></tr></form></table></td></tr></table></td></tr></table>";

			} else {
				$user=fixField(trim($HTTP_POST_VARS['user']));
				$pass=fixField(trim($HTTP_POST_VARS['pass']));
				$pass2=fixField(trim($HTTP_POST_VARS['pass2']));
				$ualias=fixField(textparse(trim($HTTP_POST_VARS['alias'])));
				$urealname=fixField(trim($HTTP_POST_VARS['realname']));
				$sig=fixField(textparse(stripslashes(trim($HTTP_POST_VARS['sig']))));
				$loc=fixField(trim($HTTP_POST_VARS['loc']));
				$msn=fixField(trim($HTTP_POST_VARS['msn']));
				$aim=fixField(trim($HTTP_POST_VARS['aim']));
				$yah=fixField(trim($HTTP_POST_VARS['yahoo']));
				$qq=fixField(trim($HTTP_POST_VARS['qq']));
				$em=fixField(trim($HTTP_POST_VARS['em']));
				$emhide=fixField(trim($HTTP_POST_VARS['emhide']));
				$web=fixField(trim($HTTP_POST_VARS['web']));
				$icq=fixField(trim($HTTP_POST_VARS['icq']));
				$pt=fixField(textparse(stripslashes(trim($HTTP_POST_VARS['pt']))));
				$image=fixField(trim($HTTP_POST_VARS['av']));
				$userlang=fixField($HTTP_POST_VARS['lang']);
				$accepted=fixField($HTTP_POST_VARS['accept']);
				$registering=fixField($userlang);
				include($temppath."/pb/language/lang_".$userlang.".php");

				if (trim($ualias)==""){
					$ualias=$user;
				}
				if ($image=="none"){
					$image="";
					$webav="";
				}elseif ($image=="webav"){
					$webav=fixField(trim($HTTP_POST_VARS['webav']));
				}else{
					$webav="";
				}
				$regcodeno=fixField(trim($HTTP_POST_VARS['regcode']));

				$mailRegex="^[_a-zA-Z0-9-]+(\.[_a-zA-Z0-9-]+)*@[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)+$";
				$usernameRegex = '^[a-zA-Z0-9]+$';
				$filename=$dbpath."/settings/users";
				$userlist=@file($filename);
				$userlst=@implode(" - ",$userlist);
				if (!$accepted){
					ErrorMessage($VRulesMustBeAccepted,$liusername);
					exit;
				}
				if (strlen($user)>25 || strlen($user)<3){
					ErrorMessage($VUsernameTooLong,$liusername);
					exit;
				}
				if (strchr(strtoupper($userlst),strtoupper($user))){
					ErrorMessage($VUsernameGiven,$liusername);
					exit;
				}
				if ($pass=="" || $em=="" || $user=="" || $pass2=="") {
					ErrorMessage($PleaseComplete,$liusername);
					exit;
				}
				if (!eregi($mailRegex, $em)) {
					ErrorMessage($WrongEmailAddress,$liusername);
					exit;
				}
				if (!eregi($usernameRegex,$user)){
					ErrorMessage($WrongUsername,$liusername);
					exit;
				}
				if (strtolower($user)=="admin" || strtolower($user)=="mods" || strtolower($user)=="friends" || strtolower($user)=="moderator" || strtolower($user)=="friend"){
					ErrorMessage($WrongUsername,$liusername);
					exit;
				}
				if ($pass!=$pass2){
					ErrorMessage($PWNoMatch,$liusername);
					exit;
				} else {
					if ($ualias!=$user){
						if ($allowalias){
							$allowaliaschange=CheckAlias($ualias);
							if ($allowaliaschange=="1"){
									$useralias=$ualias;
									AddAlias($ualias,"");
							}else{
									ErrorMessage($AliasAlreadyInUse,$liusername);
									exit;
							}
						}
					}
					$lastaliaschange=time();
					$filet=$dbpath."/members/".$user;
					if (!file_exists($filet)) {
						$ip = GetIPAddress();
						if ($mailenabled=="1" && ($emsupp=="1" || $smtpmail)) {
							$mail="$VHello $user!\n$WelcomeMessage $sitetitle"."$WelcomeMessage1!\n";
							if ($welcomemessage!=""){
									$mail.=$welcomemessage."\n";
							}
							$mail.="$ThanksMessage!";
							$header=mailheader();
							if ($confirmemail=="1"){
								$mail.=" $VPleaseConfirm\n\n";
								$mail.="$homeurl/confirm.php?code=$regcodeno&user=$user\n\n";
								if ($emsupp){
									mail($em, "$WelcomeMessage $sitetitle", $mail,$header);
								}elseif($smtpmail){
									SendMail($em, "$WelcomeMessage $sitetitle", $mail);
								}
/*								$mail1="This has been sent to $em:\n\n".$mail;
								mail($adminemail, "$WelcomeMessage $sitetitle", $mail1,$header);
*/							}else{
								if ($emsupp){
									mail($em, "$WelcomeMessage $sitetitle", $mail,$header);
								}elseif($smtpmail){
									SendMail($em, "$WelcomeMessage $sitetitle", $mail);
								}
							}
						}
						if ($emsupp=="1"){
							if ($userlang!=$orglang){
								include($temppath."/pb/language/lang_".$orglang.".php");
							}
							$header=mailheader();
							if ($emsupp){
								mail($adminemail, "$VNewMember", "$VNewMember:\n\n $user $VWith EMail $em and IP-address $ip",$header);
							}elseif($smtpmail){
								SendMail($adminemail, "$VNewMember", "$VNewMember:\n\n $user $VWith EMail $em and IP-address $ip");
							}
							if ($userlang!=$orglang){
								include($temppath."/pb/language/lang_".$userlang.".php");
							}
						}

						$pass=md5($pass);

						$filename=$dbpath."/settings/userid";
						$fd=@fopen($filename,"r");
						$userid=@fread($fd,filesize($filename));
						@fclose($fd);
						$userid++;
						$fp=fopen($dbpath."/settings/userid","w");
						fputs($fp,$userid);
						$fclose;
						$joineddate=time()+($timezone*3600);

						$content = "<?php\n\n";
						$content .="\$userid=\"$userid\";\n";
						$content .="\$password=\"$pass\";\n";
						$content .="\$username=\"$user\";\n";
						$content .="\$useralias=\"$ualias\";\n";
						$content .="\$userrealname=\"$realname\";\n";
						$content .="\$useremail=\"$em\";\n";
						$content .="\$useremhide=\"$emhide\";\n";
						$content .="\$userip=\"$ip\";\n";
						$content .="\$usersig=\"$sig\";\n";
						$content .="\$userslogan=\"$pt\";\n";
						$content .="\$useravatar=\"$image\";\n";
						$content .="\$usermsn=\"$msn\";\n";
						$content .="\$userposts=\"0\";\n";
						$content .="\$usericq=\"$icq\";\n";
						$content .="\$userqq=\"$qq\";\n";
						$content .="\$userwebavatar=\"$webav\";\n";
						if ($userid=="1") {
									$type="7";
									$useradm="1";
						} else {
									$type="1";
									$useradm="0";
						}
						$content .="\$userrank=\"$type\";\n";
						$content .="\$useryahoo=\"$yah\";\n";
						$content .="\$useraim=\"$aim\";\n";
						$content .="\$userlocation=\"$loc\";\n";
						$content .="\$userjoined=\"$joineddate\";\n";
						$content .="\$userhomepage=\"$web\";\n";
						$content .="\$useradmin=\"$useradm\";\n";
						$content .="\$usermod=\"0\";\n";
						$content .="\$userban=\"0\";\n";
						$content .="\$userlastvisit=\"$joineddate\";\n";
						$content .="\$userlastpost=\"$joineddate\";\n";
						$content .="\$userprevvisit=\"$joineddate\";\n";
						$content .="\$useranimsmilies=\"$useranimsmilies\";\n";
						$content .="\$lastaliaschange=\"$lastaliaschange\";\n";
						$content .="\$userlang=\"$userlang\";\n";
						$content .="\n?";
						$content .=">";

						if ($confirmemail=="1"){
							$fp = fopen($dbpath."/members/pending/".$regcodeno, "w");
							fputs($fp, $content);
							fclose($fp);
							chmod($dbpath."/members/pending/".$regcodeno,0666);
							$fp=fopen($dbpath."/settings/users","a");
							fputs($fp,"$user\n");
							fclose($fp);
							regdone($user,1,$userlang);
						}else{
							$fp = fopen($dbpath."/memss/".$user, "w");
							fputs($fp, $userid);
							fclose($fp);

							$fp = fopen($dbpath."/members/".$user, "w");
							fputs($fp, $content);
							fclose($fp);

							$fp=fopen($dbpath."/settings/users","a");
							fputs($fp,"$user\n");
							fclose($fp);

							$filename = $dbpath."/mems";
							$fd = fopen ($filename, "r");
							$n = fread ($fd, filesize ($filename));
							fclose ($fd);
							$n++;
							$fp = fopen($dbpath."/mems", "w");
							fputs($fp, $n);
							fclose($fp);

							$zero="0";
							$fp = fopen($dbpath."/pm/".$user, "w");
							fputs($fp, $zero);
							fclose($fp);
							$fp = fopen($dbpath."/pm/".$user."_tot", "w");
							fputs($fp, $zero);
							fclose($fp);

							$fp = fopen($dbpath."/settings/newestm", "w");
							fputs($fp, $user);
							fclose($fp);

							regdone($user,0,$userlang);
						}
					} else {
						ErrorMessage($UsernameTaken,$liusername);
					}
				}
			}
		}
		writefooter($newestm);
	}
}
ob_end_flush();
?>
