<?php
/***************************************************************************
 *                            search.php  - PBLang
 *                            -------------------
 *
 *                  see copyright.txt in the docs-folder
 *             modified on 7 Oct 2003
 ***************************************************************************/

require ('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("scan.php");
include_once("functions.php");
global $loggedin;

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"0","0");
if ($permit=="1") {
	$language=SetLanguage($liusername);
	include($temppath."/pb/language/lang_".$language.".php");
	setlocale(LC_ALL,$LangLocale);
	$reg=$HTTP_GET_VARS['id'];
	writeheader($newestm,0,0,0,$liusername,$loggedin,$VSearch);
	if ($reg!="2") {              //this means the form has to be presented
		WriteTableTop();
		echo "<form method=\"post\" action=\"search.php?id=2\">";
		echo "<tr><td class=\"header\" colspan=\"2\"><font color=\"$headerfont\">$sitetitle :: $VSearch</font></td></tr>";
		echo "<tr><td class=\"subheader\" colspan=2>
			<font color=\"$menufont\">$VSearchTerms: </font><input type=\"text\" name=\"s\">&nbsp;</td></tr>";
		echo "<tr><td class=\"subheader\" colspan=\"2\">$VSubmitForm :</td></tr>";
		echo "<tr><td height=\"18\" align=\"center\" bgcolor=\"$fmenucolor\" width=\"45%\" colspan=2>";
		echo "<input type=\"submit\" name=\"Submit\" value=\"$VSubmit\">";
		sbot(1);
		writefooter($newestm);

	} else {                      // reg is "2" - this means the results have to be presented

		$input=stripslashes($HTTP_POST_VARS['s']);
/*		if (!$input){
			echo "<meta http-equiv=\"Refresh\" content=\"0; URL=search.php\">";
		}else{
*/			WriteTableTop();
			echo "<tr><td bgcolor=\"$headercolor\" height=\"15\" background=\"$headergif\">$sitetitle :: $VSearchResults</td></tr>\n";
			echo "<tr bgcolor=\"$subheadercolor\"><td height=\"18\" background=\"$subheadergif\">$VTheSearchTerm &quot;".$input."&quot; $VFoundResults:</td></tr>\n";
			echo "<tr bgcolor=\"$subheadercolor\" valign=\"top\"><td height=\"125\" bgcolor=\"$menucolor\" width=\"55%\">\n";
			$spath=opendir($dbpath."/posts");
			$counter=0;
			while ($file = readdir($spath)){
				if ($file != "." && $file != ".." && $file !=".htaccess" && $file!="index.html" && $file!=".part") {
					$pcontent="";
					$rcontent="";
					$cont="";
					include($dbpath."/posts/".$file);
					if ($pcontent!=""){
						$cont=" ".$pcontent;
						$sub=" ".$psubject;
					}elseif($rcontent!=""){
						$cont=" ".$rcontent;
						$sub=" ".$rsubject;
					}
					if ($cont!=""){
						$yes=@strstr($cont,$input);
						if (!$yes){
							$yes=@strstr($sub,$input);
						}
						if($yes){
							$pieces = explode("_",$file);
							$accallowed=CheckPermissions($pieces[0],$pieces[1],$liusername);
							if ($accallowed=="1"){
								$name=str_replace($input, "<b>".$input."</b>",$sub);
								$found=textparse1(str_replace($input, "<b>".$input."</b>",$cont));
								$found=replacestuff($found);
								echo "<a href=\"post.php?cat=$pieces[0]&fid=$pieces[1]&pid=$pieces[2]&page=1\"><b>$name</b></a><br>$found<br><hr>\n";
							}
							$counter++;
							if ($counter==$maxsearch){
								break 4;
							}
						}    //end if ($blah)...
					}
				}    //end if ($file....
			}    //end while
			closedir($spath);

			if (!$counter) {
				echo "$NoResults.";
			}
			sbot(0);
			writefooter($newestm);
//		}
	}
}
ob_end_flush();
?>
