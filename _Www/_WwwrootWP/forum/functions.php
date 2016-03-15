<?php
if ( !defined('IN_PB') )
{
	die("Hacking attempt");
}

error_reporting  (E_ERROR | E_WARNING | E_PARSE); // This will NOT report uninitialized variables
set_magic_quotes_runtime(0); // Disable magic_quotes_runtime

define('IN_PBG', "true");

/**************************************
I began indicating when the last change was made on the 16th of December 2002.
Each function which was modified since this date contains a line stating when it was last updated.
************************************/
function AddAlias($aliasname,$oldalias)
{
	include("global.php");
	if ($oldalias!=""){
		$filename="$dbpath/aliases";
		$aliaslist=file($filename);		//get a list of aliases in use
		$aliascnt=count($aliaslist);
		$fp=fopen($dbpath."/aliases1","a");		//open the temporary file for a new alias list
		for ($i=0;$i<=$aliascnt;$i++){
			$aliasitem=trim($aliaslist[$i]);
			if ($aliasitem==$oldalias){
			}else{
					fputs($fp,$aliasitem."\n");
			}
		}
		fclose($fp);
		@unlink($filename);
		@copy("$dbpath/aliases1",$filename);
		@unlink($dbpath."/aliases1");
	}
	$fp=fopen("$dbpath/aliases","a");
	fputs($fp,$aliasname."\n");
	fclose($fp);

}

function AdminSetDone()
{
	include("global.php");
	include($dbpath."/settings.php");
	include($dbpath."/settings/styles/styles.php");
	include("$temppath/pb/language/lang_$language.php");
	echo "<td height=\"399\" bgcolor=\"$menucolor\" width=\"81%\" valign=\"top\"><table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
	echo "<tr><td><font size=\"1\">$VComplete</font></td></tr></table>";
}

function admintop() {
	global $lang,$dbpath,$sitetitle,$menucolor,$temppath;
	$orglang=$lang;
//	include("global.php");
//	include($dbpath."/settings.php");
//	include($dbpath."/settings/styles/styles.php");
	include($temppath."/pb/language/lang_".$orglang.".php");

	WriteTableTop();
	echo "<tr><td class=\"header\" colspan=\"2\">$sitetitle :: $AdminCenter</td></tr>";
	echo "<tr ><td class=\"subheader\" width=\"19%\">$VMenu :: </td>";
	echo "<td class=\"subheader\" width=\"81%\">$VView :: </td></tr>";
	echo "<tr><td height=\"399\" align=\"right\" bgcolor=\"$menucolor\" width=\"19%\" valign=\"top\"><p><u>";
	echo "$VMain</u><br>";
	echo "<a href=\"admin.php?do=news\">$PBNews $VAnd $VVersionCompare</a><br><br>";
	echo "<u>$VSettings $VAnd $VTemplates</u><br>";
	echo "<a href=\"admin.php?do=fsettings\">$VForumSettings</a><br>";
	echo "<a href=\"admin.php?do=predefstyles\">$VPredefStyles</a><br>";
	echo "<a href=\"admin.php?do=styles\">$VStylesColors</a><br>";
//	echo "<a href=\"admin.php?do=buttonstyles\">$VStyleButtonChoice</a><br>";
	echo "<a href=\"admin.php?do=templates\">$VLanguageEdit</a><br>";
	echo "<br><u>$VMembers</u><br>";
	echo "<a href=\"admin.php?do=mgroups\">$VMemberGroups</a><br>";
	echo "<a href=\"admin.php?do=ban\">$VBanMembers</a><br>";
	echo "<a href=\"admin.php?do=remove\">$VRemoveMembers</a><br>";
	echo "<a href=\"admin.php?do=emmembers\">$VEmailMembers</a><br>";
	echo "<a href=\"admin.php?do=grantm\">$VGrantStatus</a><br><br>";
	echo "<u>$VCategories</u><br>";
	echo "<a href=\"admin.php?do=catadd\">$VAddCategory</a><br>";
	echo "<a href=\"admin.php?do=editcat\">$VEditCategories</a><br>";
	echo "<br><u>$VForums</u><br>";
	$fd=fopen($dbpath."/cats/cats","r");
	$nofcats=fread($fd,filesize($dbpath."/cats/cats"));
	fclose($fd);
	if ($nofcats){
		echo "<a href=\"admin.php?do=forumadd\">$VAddForums</a><br>";
		echo "<a href=\"admin.php?do=editforum\">$VEditForums</a><br></p></td>";
	}
}

function bbottom($fid,$cat,$start,$next,$prev,$nu,$pu,$uname) {
//draws the bottom section of the board display
//updated 8 Sept 2003
	include("global.php");
	include($dbpath."/settings.php");
	include_once("ffunctions.php");
	include($dbpath."/settings/styles/styles.php");
	$language=SetLanguage($uname);
	include($temppath."/pb/language/lang_".$language.".php");
	include_once('scan.php');

	include($dbpath."/cats/".$cat."_".$fid);
	include($dbpath."/cats/".$cat);
	$prevpage="";
	$nextpage="";
	$totpages="";
	$connector="";
	$catname=replacestuff($cname);
	$forumname=replacestuff($fname);
	$stitle=replacestuff($sitetitle);
	echo "<a href=\"index.php\">$stitle</a> :: $catname :: $forumname";
	if ($prev) {
		$prevpage="<a href=\"$pu\"><-- $VPrevPage</a>";
		echo " | ".$prevpage;
	}
	if ($next=="1") {
		if ($prev=="1"){
			$connector=" :: ";
		}
		$nextpage=$connector."<a href=\"$nu\">$VNextPage --></a>";
		if ($connector==""){
			echo " | ".$nextpage."\n";
		}else{
			echo $nextpage."\n";
		}
	}
	if ($next=="0" && $prev=="0") {
		$totpages="$VPage 1 $VOf 1";
		echo " | ".$totpages."\n";
	}
	$movepages=$totpages." ".$prevpage." ".$nextpage;

}         //end function bbottom

function bpiece($subject,$icon,$author,$rep,$view,$lpd,$lpa,$fid,$cat,$pid,$lock,$loggedin,$topread,$uname,$sticky) {
	include('global.php');
	include($dbpath.'/settings.php');
	include($dbpath.'/settings/styles/styles.php');
	$language=SetLanguage($uname);
	include($temppath.'/pb/language/lang_'.$language.'.php');
	@include($dbpath.'/members/'.$author);

	$namcol=Setaliascolor($userrank);
	if (!$useralias){
		$authoralias=$author;
	}else{
		$authoralias=$useralias;
	}
	@include($dbpath.'/members/'.$lpa);
	$lnamcol=Setaliascolor($userrank);
	$lastauthoralias=$useralias;

	$image1=getimage($stylepref,'ficon4','');
	$image1a=getimage($stylepref,'ficon4_read','');
	$image2=getimage($stylepref,'ficon2','');
	$image2a=getimage($stylepref,'ficon2_read','');
	$image3=getimage($stylepref,'ficon3','');
	$image3a=getimage($stylepref,'ficon3_read','');
	$image5=getimage($stylepref,'ficon2_sticky','');
	$image5a=getimage($stylepref,'ficon2_sticky_read','');

	echo "<tr bgcolor=\"$fmenucolor\"><td align=\"center\" width=\"4%\" bgcolor=\"$forumbuttoncolor\">";
	if ($topread=="0" && $lock=="0" && $rep<20 && $sticky!='1'){
		echo "<img src=\"$image2\" alt=\"$VImage\">";
	}elseif ($topread=="1" && $lock=="0" && $rep<20 && $sticky!='1'){
		echo "<img src=\"$image2a\" alt=\"$VImage\">";
	}elseif ($topread=="0" && $lock=="0" && $rep>=20 && $sticky!='1'){
		echo "<img src=\"$image1\" alt=\"$VImage\">";
	}elseif ($topread=="1" && $lock=="0" && $rep>=20 && $sticky!='1'){
		echo "<img src=\"$image1a\" alt=\"$VImage\">";
	}elseif ($topread=="0" && $lock=="1" && $sticky!='1'){
		echo "<img src=\"$image3\" alt=\"$VImage\">";
	}elseif ($topread && $lock && !$sticky){
		echo "<img src=\"$image3a\" alt=\"$VImage\">";
	}elseif ($topread=="0" && $sticky=='1'){
		echo "<img src=\"$image5\" alt=\"$VImage\">";
	}elseif ($topread=="1" && $sticky=='1'){
		echo "<img src=\"$image5a\" alt=\"$VImage\">";
	}

	$subject=stripslashes($subject);
	$iconimage=strtok($icon,'.');
	$msgimage=GetImage($stylepref,$iconimage,'icon/');
	echo "</td><td align=\"center\" bgcolor=\"$forumbuttoncolor\" width=\"3%\"><img src=\"$msgimage\" alt=\"$VImage\"></td>\n";
	echo "<td height=\"12\"  onMouseOver=\"this.style.backgroundColor='$forumbuttonover';this.style.cursor='hand';\"";
	echo "onClick=\"document.location.href='post.php?fid=$fid&cat=$cat&pid=$pid&page=1'\"";
	echo "onMouseOut=\"this.style.backgroundColor='$forumbuttoncolor';this.style.cursor='hand';\" bgcolor=\"$forumbuttoncolor\" width=\"41%\">";
	echo "<a href=\"post.php?cat=$cat&fid=$fid&pid=$pid&page=1\">$subject</a></td>\n";
	echo "<td height=\"12\"  onMouseOver=\"this.style.backgroundColor='$forumbuttonover';this.style.cursor='hand';\"";
	echo "onClick=\"document.location.href='profile.php?u=$author'\"";
	echo "onMouseOut=\"this.style.backgroundColor='$forumbuttoncolor';this.style.cursor='hand';\" bgcolor=\"$forumbuttoncolor\" width=\"14%\">";

	if (file_exists($dbpath."/memss/".$author) && $loggedin=="1"){
		echo "<a href=\"profile.php?u=$author\" title=\"$VProfile\" name=\"$VProfile\"><font color=$namcol>$authoralias</font></a>";
	}else{
		echo "<font color=$namcol>".$authoralias."</font>";
	}

	echo "</td><td height=\"12\" bgcolor=\"$forumbuttonreplies\" width=\"8%\" align=\"center\">$rep</td>\n";
	echo "<td height=\"12\" bgcolor=\"$forumbuttonreplies\" width=\"8%\" align=\"center\">$view</td>\n";
	echo "<td height=\"12\" align=\"left\" onMouseOver=\"this.style.backgroundColor='$forumbuttonover';this.style.cursor='hand';\"";
	$filename=$dbpath."/member/".$lpa; //last author's memberdata
	echo "onClick=\"document.location.href='profile.php?u=$lpa'\"";
	echo "onMouseOut=\"this.style.backgroundColor='$forumbuttonlast';this.style.cursor='hand';\" bgcolor=\"$forumbuttonlast\" width=\"22%\">";
	echo "$VOn $lpd $VBy ";
	if (file_exists($dbpath."/memss/".$author) && $loggedin=="1"){
		echo "<a href=\"profile.php?u=$lpa\" title=\"$VProfile\" name=\"$VProfile\"><font color=$lnamcol>$lastauthoralias</font></a></td>";
	}else{
		echo "<font color=$lnamcol>".$lastauthoralias."</font>";
	}
	echo "</tr>\n";
}

function btop($fid,$cat,$start,$next,$prev,$nu,$pu,$uname) {
//draws the top section of the board display
//updated 8 Sept 2003
	include("global.php");
	include($dbpath.'/settings.php');
	include_once("ffunctions.php");
	include($dbpath.'/settings/styles/styles.php');
	$language=SetLanguage($uname);
	include_once('scan.php');
	include($temppath."/pb/language/lang_".$language.".php");

	WriteTableTop();
	echo "<tr><td class=\"header\" height=\"16\" colspan=\"7\">\n";

	include($dbpath."/cats/".$cat."_".$fid);
	include($dbpath."/cats/".$cat);
	$prevpage='';
	$nextpage='';
	$totpages='';
	$connector='';
	$catname=replacestuff($cname);
	$forumname=replacestuff($fname);
	$stitle=replacestuff($sitetitle);
	echo "<a href=\"index.php\">$stitle</a> :: $catname :: $forumname";
	if ($prev) {
		$prevpage="<a href=\"$pu\"><-- $VPrevPage</a>";
		echo " | ".$prevpage;
	}
	if ($next) {
		if ($prev){
			$connector=" :: ";
		}
		$nextpage=$connector."<a href=\"$nu\">$VNextPage --></a>";
		if ($connector==''){
			echo " | ".$nextpage."\n";
		}else{
			echo $nextpage."\n";
		}
	}
	if (!$next && !$prev) {
		$totpages="$VPage 1 $VOf 1";
		echo " | ".$totpages."\n";
	}
	$movepages=$totpages." ".$prevpage." ".$nextpage;

//     echo "</font></td></tr><tr bgcolor=\"$menucolor\"><tr bgcolor=\"$containerinner\">";
	echo "</font></td></tr><tr bgcolor=\"$containerinner\">";
	echo "<td class=\"subheader\" height=\"15\" align=\"center\" width=\"4%\">\n";
	echo "<img src=\"templates/pb/images/plusminus.gif\" BORDER=\"0\" alt=\"$VImage\"></td>\n";
	echo "<td class=\"subheader\" align=\"center\" width=\"3%\">";
	echo "<img src=\"templates/pb/images/plusminus.gif\" BORDER=\"0\" alt=\"$VImage\"></td>\n";
	echo "<td class=\"subheader\" width=\"41%\"><b>$VSubject:</b></td>\n";
	echo "<td class=\"subheader\" width=\"14%\"><b>$VAuthor:</b></td>\n";
	echo "<td class=\"subheader\" width=\"8%\"><b>$VReplies:</b></td>\n";
	echo "<td class=\"subheader\" width=\"8%\"><b>$VViews:</b></td>\n";
	echo "<td class=\"subheader\" width=\"22%\"><b>$VLastReply:</b></td></tr>\n";
	return($movepages);
}         //end function btop

function CheckAlias($aliasname)
{
	global $dbpath;
	$filename="$dbpath/aliases";
	$aliaslist=@file($filename);
	$aliascnt=count($aliaslist);
	$allow="1";
	for ($i=0;$i<=$aliascnt;$i++){
		if (trim($aliaslist[$i])==$aliasname){
			$allow="0";
			break;
		}
	}
	if ($allow=="1"){
		$userlist=@file($dbpath."/settings/users");
		$listcnt=count($userlist);
		for ($i=0;$i<=$listcnt;$i++){
			if (trim($userlist[$i])==$aliasname){
					$allow="0";
					break;
			}
		}
	}
	return ($allow);
}         //end function CheckAlias

function CheckColorCode($colorcode)
{
	if (substr($colorcode,0,1)!='#' && strlen($colorcode)>0){
		$ccode='#'.$colorcode;
	}else{
		$ccode=$colorcode;
	}
	return($ccode);
}
function CheckFiletype($extension)
{
	global $dbpath;
	$ok='';
	$filename=$dbpath.'/settings/attachtypes';
	$fd=fopen($filename,'r');
	$attachtypelist=','.trim(fread($fd,filesize($filename)));
	fclose($fd);
	if (strpos($attachtypelist,$extension)){
		$ok='1';
	}
	return ($ok);
}


function GetIPAddress()
{
	global $REMOTE_ADDR,$HTTP_X_FORWARDED_FOR;
	if ($HTTP_X_FORWARDED_FOR) { 
		$ipaddr = $HTTP_X_FORWARDED_FOR;
	} else {
		$ipaddr = $REMOTE_ADDR;
	}
	if(!$ipaddr){
		$ipaddr= $_SERVER['REMOTE_ADDR'];
	}
	return ($ipaddr);
}

function CheckLoginStatus($uname,$login,$register){
/*$uname is the username from the cookie, $login is whether the page requires the user to be logged in
$register is whether page requires visitor to be a member*/
//modified on 16 Dec 2002

	include("global.php");
	include($dbpath."/settings.php");
	global $admin,$moder,$ban,$friend,$loggedin;

	$admin="0";$moder="0";$ban="0";$friend="0";
	if ($uname=="") {
		$loggedin="0";
		$admin="0";
		if ($login=="1" && $register=="0"){
			$perm="0";
		}else{
			$perm="1";
		}
	} else {
		$filename = $dbpath."/members/".$uname;
		if (!file_exists($filename)) {
			$perm="0";
		} else {
			include($dbpath."/members/".$uname);
			$ip = GetIPAddress();
			if ($userip!=$ip && $ipcheck=="1" && $login=="1") {
					echo "<meta http-equiv=\"Refresh\" content=\"0; URL=logout.php\">";
					$perm="0";
					exit;
			} else {
					$perm="1";
			}
			$loggedin="1";
			$admin=$useradmin;
			$moder=$usermod;
			$ban=$userban;
			$friend=$userfriend;
		}
	}
	if ($ban=="1") {
		$perm="0";
		ErrorMessage($banreason,$uname);
		exit;
	} else {
		if ($maint=="1" && $admin=="0") {
			$perm="0";
			writeheader('',0,0,0,'','',$VError);
			ErrorMessage($mreason,$uname);
			exit;
		} else {
			if ($loginreq=="1" && $loggedin=="0" && $register!="1") {
				echo "<meta http-equiv=\"Refresh\" content=\"0; URL=login.php?do=req\">";
				exit;
			}
		}
	}
	if ($loggedin=="0" && $login=="1" && $register!="1") {
		echo "<meta http-equiv=\"Refresh\" content=\"0; URL=login.php\">";
		exit;
	}
	return $perm;
}

function CheckPermissions($cid,$fid,$usr)
{
//last updated 11 October 2003
	global $dbpath;
	if ($usr!=""){
		include($dbpath."/members/".$usr);
	}
	$filename="$dbpath/cats/$cid"."_$fid";
	if (file_exists($filename)){
		include ($filename);
		if (strstr($fusers,"all")){
			$allow="1";
		}else{
			if($userfriend=="1" && strstr($fusers,"friends")){
				$allow="1";
			}elseif($usermod=="1" && strstr($fusers,"mods")){
				$allow="1";
			}elseif(strstr($fusers,$usr)){
				$allow="1";
			}elseif($useradmin=="1"){
				$allow="1";
			}else{
				$allow="0";
			}
		}
		return($allow);
	}else{
		return(0);
	}
}

function CheckSlashes($teststring)
{
	if (strpos($teststring,chr(92).chr(34))){
		$result=$teststring;
	}elseif(strpos($teststring,chr(34))){
		$result=addslashes($teststring);
	}else{
		$result=$teststring;
	}
	return ($result);
}

function Delay($aotime)
{
	$now=mktime();
	$endtime=$now+$aotime;
	while(mktime()<$endtime){}
}

function ErrorMessage($MsgContent,$uname)
{
	include("global.php");
	include($dbpath."/settings.php");
	global $registering;
	$language=SetLanguage($uname);
	include($dbpath."/settings/styles/styles.php");
	include("$temppath/pb/language/lang_$language.php");
	$MsgContent=str_replace(" ","&nbsp;",$MsgContent);
	WriteTableTop();
	echo "<tr><td class=\"header\" colspan=\"2\"><a href=\"index.php\">$sitetitle</a> :: $VError</td></tr>";
	echo "<tr><td class=\"content\" colspan=\"2\">$MsgContent!<br><br>$ContactAdmin";
	echo " <a href=\"mailto:$adminemail\">$adminemail</a></td></tr></table></td></tr></table></td></tr></table>";
}

function FAccessDenied()
{
include("global.php");
include("db/settings.php");
include("$temppath/pb/language/lang_$language.php");
		echo "<td height=\"399\" bgcolor=\"$menucolor\" width=\"81%\" valign=\"top\"><table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
		echo "<tr> <td><font size=\"1\">$AccessDenied!</font></td></tr></table>";
}

function GetImage($style,$image,$subpath)
{
	global $language,$temppath;
	if(file_exists($temppath.'/pb/images/'.$subpath.$style.$image.'_'.$language.'.gif')){	//if the file exists in that style and language
		$existingimg='templates/pb/images/'.$subpath.$style.$image.'_'.$language.'.gif';
	}elseif (file_exists($temppath.'/pb/images/'.$subpath.$image.'_'.$language.'.gif')){	//if that file exists in that language
		$existingimg='templates/pb/images/'.$subpath.$image.'_'.$language.'.gif';
	}elseif (file_exists($temppath.'/pb/images/'.$subpath.$style.$image.'.gif')){	//if that file exists in that style
		$existingimg='templates/pb/images/'.$subpath.$style.$image.'.gif';
	}elseif (file_exists($temppath.'/pb/images/'.$subpath.$image.'.gif')){		//if that file exists as default
		$existingimg='templates/pb/images/'.$subpath.$image.'.gif';
	}else{
		$existingimg='';
	}
	return $existingimg;
}

function GetLatestForumMessage($cat,$fid,$lastpost)
{
	//find  the last post in this forum
	global $dbpath;
	$handle=opendir($dbpath."/posts");
	$found=FALSE;
	$TopFileTime=0;
	for ($i=1;$i<=$lastpost;$i++){
		$filename=$dbpath."/posts/".$cat."_".$fid."_".$i;
		if (file_exists($filename)){
			include($filename);
			$CFileTime=$plastdate;
			if ($CFileTime>$TopFileTime)
			{
				$TopPost['author']=$plastauthor;
				$TopPost['date']=$plastdate;
				$TopPost['id']=$i;
			}
		}
	}  //end for
	return ($TopPost);
}

function GetMemberList()
{
	include("global.php");
	$handle = opendir($dbpath.'/memss/');
	while ($file = readdir($handle)) {
			if ($file != "." && $file != ".." && $file != "index.html" && $file != ".htaccess") {
					$filename = "$dbpath/memss/$file";
					echo "<option value=\"$file\">$file</option>\n";
			}
	}
	closedir($handle);
}

function get_phrase_for_1num($var, $num,$uname) // Here we must have 1 number (0-9)
{
include("global.php");
include("db/settings.php");
$language=SetLanguage($uname);
include("$temppath/pb/language/lang_$language.php");

//  eval("global $var"."1, $var"."24, $var"."59;");
if (($num == 0) or ($num > 4))
{
	return eval("return $var"."59;");
}
else if ($num == 1)
{
	return eval("return $var"."1;");
}
else
{
	return eval("return $var"."24;");
}
}

function get_phrase_for_num($var, $num,$uname)  // This the function to call
{
// modified 20 Dec 2002
include("global.php");
include("db/settings.php");
$language=SetLanguage($uname);
include("$temppath/pb/language/lang_$language.php");

//  eval("global $var"."59;");
if (strlen($num) > 1)  // Has more then 2 numbers
{
		if (substr($num, -2, 1) == '1') // It is the second ten(10-19)
		{
				return eval("return $var"."59;");
		}
		else
		{
				if ($language != "ru")
				{
						return eval("return $var"."59;");
				}
				else
				{
						return get_phrase_for_1num($var, substr($num, -1, 1),$uname); // Send the last number
				}
		}
}
else
{
		return get_phrase_for_1num($var, $num, $uname);
}
}

function LanguageSelection($ulang)
{
	if (file_exists("global.php")){
		include("global.php");
	}else{
		$basepath=getcwd();
		$dbpath=$basepath."/db";
		$temppath=$basepath."/templates";
	}
	include($dbpath."/settings.php");
	global $langexpl;
	$filename=$temppath."/pb/language/languages.txt";
	$lnglist=file($filename);
	$lngcount=count($lnglist);
	$handle=opendir($temppath."/pb/language/");
	while ($file=readdir($handle)){
		if ($file != "." && $file != ".." && $file!="index.htm" && $file!=".htaccess" && $file!="lang_temp.php" && $file!="readme.txt" && $file!="languages.txt"){
			$teil=strtok($file,"_");
			$lang=strtok("_");
			$lang=substr($lang,0,strlen($lang)-4);  //remove extention ".php"
			for ($i=0;$i<=$lngcount;$i++){
					if ($lang==substr($lnglist[$i],0,strlen($lang))){
						$langexpl.=$lnglist[$i]." - ";
						break;
					}
			}
			if ($ulang!=""){
					if ($lang==$ulang){
						echo "<option value=\"$lang\" selected>$lang</option>";
					}else{
						echo "<option value=\"$lang\">$lang</option>";
					}
			}else{
					if ($lang==$language){
						echo "<option value=\"$lang\" selected>$lang</option>";
					}else{
						echo "<option value=\"$lang\">$lang</option>";
					}
			}
		}
	}
	$langexpl=substr($langexpl,0,-3);
	closedir($handle);
}

function loginform($do) {
//     include("global.php");
	global $dbpath,$temppath,$language;
	include($dbpath."/settings.php");
	include_once("functions.php");
	include($dbpath."/settings/styles/styles.php");
	include("$temppath/pb/language/lang_$language.php");

	WriteTableTop();
	if ($do=="req") {
		echo "<form method=\"post\" action=\"login.php?id=2&do=req\">";
	} else {
		echo "<form method=\"post\" action=\"login.php?id=2\">";
	}

	echo "<tr><td  class=\"header\" colspan=\"2\">$sitetitle :: $VLogin</td></tr>";
	echo "<tr><td height=\"16\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"45%\">
	<font color=\"$menufont\">$VUsername:</font></td>";
	echo "<td height=\"22\" bgcolor=\"$menucolor\" width=\"55%\"><input type=\"text\" name=\"user\" value='Anonymous'></td></tr>";
	echo "<tr bgcolor=\"$fmenucolor\"><td height=\"16\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"45%\"><font color=\"$menufont\">$VPassword:</font></td>";
	echo "<td height=\"22\" bgcolor=\"$menucolor\" width=\"55%\"><input type=\"password\" name=\"pass\" value='1111'></td></tr>";
	echo "<tr><td class=\"subheader\" colspan=\"2\">$VSubmitForm :: Enter your login/password or leave default values to log in as Anonimus.<hr>To be personalized at this forum, you should be <a href='/forum/register.php'>registered</a>. It`s free and fast and allows you to receive emails with answers to your posts</td></tr>";
	echo "<tr bgcolor=\"$fmenucolor\"><td height=\"16\" align=\"left\" bgcolor=\"$fmenucolor\" width=\"45%\" valign=\"middle\">&nbsp;</td>";
	echo "<td height=\"22\" bgcolor=\"$menucolor\" width=\"55%\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\">&nbsp;";
	echo "<input type=\"submit\" name=\"Submit2\" value=\"$RetrievePassword\">";
	sbot(1);
}

function mailheader(){
global $adminemail, $sitetitle,$Charset;

$headers = "From: $sitetitle <$adminemail>\r\n";
$headers .= "MIME-Version: 1.0\r\n";
$headers .= "Content-type: text; charset=$Charset\r\n";
$headers .= "Reply-To: $sitetitle <$adminemail>\r\n";
// either X-Priority or X-MSmail-Priority, not both
//$headers .= "X-Priority: 2\r\n";
$headers .= "X-MSmail-Priority: Normal\r\n";
$headers .= "X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2800.1106\r\n";
$headers .= "X-mailer: PBLang Forum\r\n\r\n";
return $headers;
}
function msg($title,$content) {
	include("global.php");
	include("db/settings.php");
	include($dbpath."/settings/styles/styles.php");

	WriteTableTop();
	echo "<tr><td class=\"header\" colspan=\"2\">$sitetitle :: $title</td></tr>";
	echo "<tr><td class=\"content\" colspan=\"2\">$content</td></tr></table></td></tr></table></td></tr></table>";
}



function postp($auth,$cont,$sub,$rank,$rank2,$pt,$p,$sig,$cm,$ip,$a,$u,$d,$c,$pa,$userrm,$uname,$qurl,$tlock) {
	include("global.php");
	include($dbpath."/settings.php");
	include($dbpath."/settings/styles/styles.php");
	include_once("ffunctions.php");
	if ($uname==""){
	}else{
		$language=SetLanguage($uname);
		include($dbpath."/members/".$uname);
	}
	include($temppath."/pb/language/lang_".$language.".php");
	$admin=$useradmin;
	$mod=$usermod;
	$dispnoav=$noavatars;
	global $loggedin;
	$profileimg=GetImage($stylepref,'profile','butt/');
	$emailimg=GetImage($stylepref,'email','butt/');
	$editimg=GetImage($stylepref,'edit','butt/');
	$sendpmimg=GetImage($stylepref,'sendpm','butt/');
	$yimimg=GetImage($stylepref,'yim','butt/');
	$aimimg=GetImage($stylepref,'aim','butt/');
	$msnimg=GetImage($stylepref,'msn','butt/');
	$icqimg=GetImage($stylepref,'icq','butt/');
	$wwwimg=GetImage($stylepref,'www','butt/');
	$replyimg=GetImage($stylepref,'reply','butt/');
	$lockedimg=GetImage($stylepref,'reply-locked','butt/');
	$qqimg=GetImage($stylepref,'qqb','butt/');
	$quoteimg=GetImage($stylepref,'quote','butt/');

	if ($userrm=="0"){
		include($dbpath."/members/".$auth);
		$av=$useravatar;
		$webav=$userwebavatar;
		$em=$useremail;
		$emhide=$useremhide;
		$aim=$useraim;
		$msn=$usermsn;
		$yim=$useryahoo;
		$icq=$usericq;
		$qq=$userqq;
		$webs=$userhomepage;
		$ualias=$useralias;
		$namcol=Setaliascolor($userrank);
	}
	if ($cm=="2") {
			$cm=$forumbuttonover;
	}
	if ($cm=="1") {
			$cm=$forumbuttoncolor;
	}

	$filet=$dbpath."/online/".$auth;
	if (file_exists($filet)) {
			$on=$VLoggedOn;
	} elseif ($userrm=="1") {
			$on=$VMemberRemoved;
	} elseif (!file_exists($filet)) {
			$on=$VLoggedOff;
	} else {
			$on=$VError;
	}

	echo "<tr bgcolor=\"$fmenucolor\"><td align=\"center\" bgcolor=\"$cm\" rowspan=\"2\" valign=\"top\">";

	if ($userrm=="0")
	{
		echo "<font color=\"$menufont\">";
		echo "<a href=\"profile.php?u=$auth\"><font style=\"color:$namcol\">$ualias</font></a>";
		if ($admin && $auth != $ualias){
			echo '<br>',$VUsername,': <font style="color:',$namcol,'">',$auth,'</font>';
		}
		echo "<br>$rank2<br><img src=\"$rank\" alt=\"$VImage\"><br>";
		if (($ave=="1" || $webave) && !$dispnoav) {
			if ($av=="webav") {
				if (!allow_url_fopen){
					if ($webave){
						echo "<img src=\"$webav\" alt=\"$VImage\">";
					}
				}elseif ($webave){
					$fexists=@fopen($webav,'rb');
					if ($fexists){
						fclose($fexists);
						$avsize=@getimagesize($webav);
						$avheight=$avsize[1];
						$avwidth=$avsize[0];
						if (($avmaxheight || $avmaxwidth) && ($avheight>$avmaxheight || $avwidth>$avmaxwidth)){
							$heightratio=$avheight/$avmaxheight;
							$widthratio=$avwidth/$avmaxwidth;
							if ($heightratio>$widthratio){
								$avheight=round($avheight/$heightratio);
								$avwidth=round($avwidth/$heightratio);
							}else{
								$avheight=round($avheight/$widthratio);
								$avwidth=round($avwidth/$widthratio);
							}
							echo "<img src=\"$webav\" alt=\"$VImage\" width=\"$avwidth\" height=\"$avheight\">";
						}else{
							echo "<img src=\"$webav\" alt=\"$VImage\">";
						}
					}else{
						echo $VNoAvatar,"<br>";
					}
				}
			} elseif (trim($av)!="" && trim($av)!="none") {
					echo "<img src=\"templates/pb/images/avatars/$av\" alt=\"$VImage\">";
			}
		echo "<br>".$pt."<br>$VPosts: $p<br>";
		}
	}else{
		echo "<font style=\"color:$namcol\">$auth</font><br>";
		echo "$on<br>";
	}

	if ($d=="1" || $a=="1") {
			echo "<a href=\"$c\">$VDeletePost</a>";
	}

	$sub=stripslashes($sub);
	echo "</font></td>\n";
	echo "<td height=\"2\" bgcolor=\"$cm\" width=\"50%\"><font color=\"$menufont\"><b>$sub</b></font>";
	echo "<font color=\"$menufont\">&nbsp;($pa)</font></td>\n";
	echo "<td height=\"2\" bgcolor=\"$cm\" width=\"25%\" align=\"right\" cellpadding=0><font color=\"$menufont\">";
	if($picturebuttons=="Yes"){
		if ($tlock!="1"){
			echo "<a href=\"$qurl\"><img src=\"$quoteimg\" border=\"0\" alt=\"$quotealt\" title=\"$quotetitle\"></a>&nbsp;";
		}
		if (($auth==$uname && $tlock!='1') || $admin=="1" || $mod=="1"){
			echo "<a href=\"$u\"><img src=\"$editimg\" border=\"0\" alt=\"$editalt\" title=\"$edittitle\"></a>";
		}
	}else{
		if ($tlock!="1"){
			echo "<a href=\"$qurl\">$quotealt</a>&nbsp;";
		}
		if (($auth==$uname && $tlock!='1') || $admin=="1" || $mod=="1"){
			echo "|&nbsp;<a href=\"$u\">$editalt</a>\n ";
		}
	}
	echo "&nbsp;</font></td></tr>\n";
	$cont=stripslashes($cont);
	echo "<tr bgcolor=\"$fmenucolor\"><td bgcolor=\"$cm\" width=\"75%\"valign=\"top\" colspan=\"2\"><font color=\"$menufont\">$cont<BR><BR>\n";

	if ($userrm=="0"){
		if ($sig!=""){
			echo "<div valign=\"baseline\"><hr size=1 color=\"#000000\" class=\"windowbg3\">$sig";
		}
		echo "</td></tr>\n";
		echo "<tr bgcolor=\"$menucolor\"><td height=\"2\" align=\"left\" bgcolor=\"$cm\" width=\"20%\" valign=\"middle\">";
		if ($on==$VLoggedOn){
			$ipimg="ip_on.gif";
		}else{
			$ipimg="ip_off.gif";
		}
		if ($a=="1") {
			echo "<a href=\"http://www.nic.com/cgi-bin/whois.php?query=$ip\" target=\"_blank\">";
			echo "<img src=\"templates/pb/images/$ipimg\" alt=\"$ip\" border=\"0\"></a>";
		} else {
			echo "<img src=\"templates/pb/images/$ipimg\" alt=\"$VIPLogged\" title=\"$VIPLogged\" border=\"0\"></a>";
		}
	echo "&nbsp;$VStatus: $on</td>\n";
		if ($picturebuttons=="Yes"){
			echo "<td height=\"2\" bgcolor=\"$cm\" width=\"75%\" colspan=\"2\"> <a href=\"profile.php?u=$auth\">";
			echo "<img src=\"$profileimg\" border=\"0\" alt=\"$profilealt\" title=\"$profiletitle\"></a>&nbsp;";
			if ($emhide!="hide" && $loggedin=="1") {
				echo "<a href=\"mailto:$em\"><img src=\"$emailimg\" border=\"0\" alt=\"$emailalt\" title=\"$emailtitle\"></a>&nbsp;";
			}

			echo "";
			if ($yim!="") {
				echo "<a href=\"http://edit.yahoo.com/config/send_webmesg?.target=$yim&.src=pg\" target=\"_blank\">";
				echo "<img src=\"$yimimg\" border=\"0\" alt=\"$yimalt\" title=\"$yimtitle\"></a>&nbsp;";
			}
			if ($aim!="") {
				echo "<a href=\"aim:goim?screenname=$aim&message=Hello+Are+you+there?\" target=\"_blank\">";
				echo "<img src=\"$aimimg\" border=\"0\" alt=\"$aimalt\" title=\"$aimtitle\"></a>&nbsp;";
			}
			if ($msn!="") {
				echo "<a href=\"profile.php?u=$auth\">";
				echo "<img src=\"$msnimg\" border=\"0\" alt=\"$msnalt\" title=\"$msntitle\"></a>&nbsp;";
			}
			if ($icq!="") {
				echo "<a href=\"http://wwp.icq.com/scripts/search.dll?to=$icq\">";
				echo "<img src=\"$icqimg\" border=\"0\" alt=\"$icqalt\" title=\"$icqtitle\"></a>&nbsp;";
			}
			if ($qq!="") {
				echo "<a href=\"profile.php?u=$auth\">";
				echo "<img src=\"$qqimg\" border=\"0\" alt=\"$qqalt\" title=\"$qqtitle\"></a>&nbsp;";
			}
			if ($webs!="" && $webs!="http://" && $webs!="http:///") {
				echo "<a href=\"$webs\" target=\"_blank\">";
				echo "<img src=\"$wwwimg\" border=\"0\" alt=\"$wwwalt\" title=\"$wwwtitle\"></a>&nbsp;";
			}
			if ($loggedin=="1"){
				echo "<a href=\"sendpm.php?to=$auth\">";
				echo "<img src=\"$sendpmimg\" border=\"0\" alt=\"$sendpmalt\" title=\"$sendpmtitle\"></a>&nbsp;";
			}else{
			}
		}else{
			echo "<td height=\"2\" bgcolor=\"$cm\" width=\"75%\" colspan=\"2\">";
			echo "<a href=\"profile.php?u=$auth\">$profilealt</a>&nbsp;|&nbsp;";
			if ($emhide!="hide" && $loggedin=="1") {
				echo "<a href=\"mailto:$em\">$emailalt</a>&nbsp;|&nbsp;";
			}
			if ($loggedin=="1"){
				echo "<a href=\"sendpm.php?to=$auth\">$sendpmalt</a>&nbsp;|&nbsp;";
			}
			if ($yim!="") {
				echo "<a href=\"http://edit.yahoo.com/config/send_webmesg?.target=$yim&.src=pg\" target=\"_blank\">$yimalt</a>&nbsp;|&nbsp;";
			}
			if ($aim!="") {
						echo "<a href=\"aim:goim?screenname=$aim&message=Hello+Are+you+there?\" target=\"_blank\">$aimalt</a>&nbsp;|&nbsp;";
			}
			if ($msn!="") {
					echo "<a href=\"profile.php?u=$auth\">$msnalt</a>&nbsp;|&nbsp;";
			}
			if ($icq!="") {
					echo "<a href=\"http://wwp.icq.com/scripts/search.dll?to=$icq\">$icqalt</a>&nbsp;|&nbsp;";
			}
			if ($qq!="") {
					echo "<a href=\"profile.php?u=$auth\">$qqalt</a>&nbsp;|&nbsp;";
			}
			if ($webs!="" && $webs!="htpp://" && $webs!="http:///") {
					echo "<a href=\"$webs\" target=\"_blank\">$wwwalt</a>&nbsp;";
			}
		}
	}
	echo " </td></tr>\n";
}         //end function postp

function PrintSmileys($anim,$adm,$ulang)
{
	include("global.php");
	include($dbpath."/settings.php");
	include($dbpath."/settings/styles/styles.php");
	include("$temppath/pb/language/lang_$ulang.php");

	if ($smilys || $adm){
		echo "<tr><td class=\"label\" width=\"19%\">$VSmilies:</td>";
		echo "<td height=\"2\" bgcolor=\"$menucolor\" width=\"81%\">";

	//non-animated smilies
		echo "<a href=javascript:smiley()><img src=\"templates/pb/images/smile/smiley.gif\" align=bottom alt=\"$VSmiley\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:cheesy()><img src=\"templates/pb/images/smile/cheesy.gif\" align=bottom alt=\"$VSmiley\" title=\"$VSmiley\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:angry()><img src=\"templates/pb/images/smile/angry.gif\" align=bottom alt=\"$VAngry\" title=\"$VAngry\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:sad()><img src=\"templates/pb/images/smile/sad.gif\" align=bottom alt=\"$VSad\" title=\"$VSad\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:shocked()><img src=\"templates/pb/images/smile/shocked.gif\" align=bottom alt=\"$VShocked\" title=\"$VShocked\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:huh()><img src=\"templates/pb/images/smile/huh.gif\" align=bottom alt=\"$VHuh\" title=\"$VHuh\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:tongue()><img src=\"templates/pb/images/smile/tongue.gif\" align=bottom alt=\"$VTongue\" title=\"$VTongue\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:wink()><img src=\"templates/pb/images/smile/wink.gif\" align=bottom alt=\"$VWink\" title=\"$VWink\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:evil()><img src=\"templates/pb/images/smile/evil.gif\" align=bottom alt=\"$VEvil\" title=\"$VEvil\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:em()><img src=\"templates/pb/images/smile/embarassed.gif\" align=bottom alt=\"$VEmbarassed\" title=\"$VEmbarassed\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:goof()><img src=\"templates/pb/images/smile/goofy.gif\" align=bottom alt=\"$VGoofy\" title=\"$VGoofy\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:rolleyes()><img src=\"templates/pb/images/smile/rolleyes.gif\" align=bottom alt=\"$VRollEyes\" title=\"$VRollEyes\" border=\"0\"></a> ";
	//     echo "</td></tr>";

		echo "<a href=javascript:glad()><img src=\"templates/pb/images/smile/glad.gif\" align=bottom alt=\"$Vgladg\" title=\"$Vgladg\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:grins()><img src=\"templates/pb/images/smile/grins.gif\" align=bottom alt=\"$Vgrinsg\" title=\"$Vgrinsg\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:bad()><img src=\"templates/pb/images/smile/bad.gif\" align=bottom alt=\"$Vbadg\" title=\"$Vbadg\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:sad()><img src=\"templates/pb/images/smile/sad1.gif\" align=bottom alt=\"$VSad\" title=\"$VSad\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:schluck()><img src=\"templates/pb/images/smile/schluck.gif\" align=bottom alt=\"$Vschluckg\" title=\"$Vschluckg\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:sungl()><img src=\"templates/pb/images/smile/sungl.gif\" align=bottom alt=\"$Vsunglg\" title=\"$Vsunglg\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:traene()><img src=\"templates/pb/images/smile/traene.gif\" align=bottom alt=\"$Vteardrop\" title=\"$Vteardrop\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:traurig()><img src=\"templates/pb/images/smile/traurig.gif\" align=bottom alt=\"$VSad\" title=\"$VSad\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:ueberr()><img src=\"templates/pb/images/smile/ueberr.gif\" align=bottom alt=\"$Vsurprised\" title=\"$Vsurprised\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:zorn()><img src=\"templates/pb/images/smile/zorn.gif\" align=bottom alt=\"$VAngry\" title=\"$VAngry\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:thmbup()><img src=\"templates/pb/images/smile/damauf.gif\" align=bottom alt=\"$Vthmbup\" title=\"$Vthmbup\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:thmbdn()><img src=\"templates/pb/images/smile/damob.gif\" align=bottom alt=\"$Vthmbdn\" title=\"$Vthmbdn\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:pfeilr()><img src=\"templates/pb/images/smile/pfeilr.gif\" align=bottom alt=\"$Vpfeilrg\" title=\"$Vpfeilrg\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:rufz()><img src=\"templates/pb/images/smile/rufz.gif\" align=bottom alt=\"$VExplanationPoint\" title=\"$VExplanationPoint\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:qmark()><img src=\"templates/pb/images/smile/fragez.gif\" align=bottom alt=\"$VQuestionMark\" title=\"$VQuestionMark\" border=\"0\"></a>&nbsp;";
		echo "<a href=javascript:gluehb()><img src=\"templates/pb/images/smile/gluehb.gif\" align=bottom alt=\"$Vbulb\" title=\"$Vbulb\" border=\"0\"></a>";
		echo "<br>";
		echo "<a href=javascript:offtopic()><img src=\"templates/pb/images/smile/offtopic.gif\" align=bottom alt=\"$Vofftopicg\" title=\"$Vofftopicg\" border=\"0\"></a>&nbsp;";
	//     echo "<BR>";

	//animated smilies
		if ($anim=="1"){
			echo "<a href=javascript:help()><img src=\"templates/pb/images/smile/help.gif\" align=bottom alt=\"$VHelpg\" title=\"$VHelpg\" border=\"0\"></a>&nbsp;";
			echo "<a href=javascript:bounce()><img src=\"templates/pb/images/smile/bounce.gif\" align=bottom alt=\"$Vbounceg\" title=\"$Vbounceg\" border=\"0\"></a>&nbsp;";
			echo "<a href=javascript:dance()><img src=\"templates/pb/images/smile/dance.gif\" align=bottom alt=\"$Vdanceg\" title=\"$Vdanceg\" border=\"0\"></a>&nbsp;";
			echo "<a href=javascript:redface2()><img src=\"templates/pb/images/smile/redface2.gif\" align=bottom alt=\"$Vredface2g\" title=\"$Vredface2g\" border=\"0\"></a>&nbsp;";
			echo "<a href=javascript:devil()><img src=\"templates/pb/images/smile/devil.gif\" align=bottom alt=\"$Vdevilg\" title=\"$Vdevilg\" border=\"0\"></a>&nbsp;";
			echo "<a href=javascript:schimpf()><img src=\"templates/pb/images/smile/schimpf.gif\" align=bottom alt=\"$Vschimpfg\" title=\"$Vschimpfg\" border=\"0\"></a>&nbsp;";
			echo "<a href=javascript:wallbash()><img src=\"templates/pb/images/smile/wallbash.gif\" align=bottom alt=\"$Vwallbashg\" title=\"$Vwallbashg\" border=\"0\"></a>&nbsp;";
			echo "<a href=javascript:sleep()><img src=\"templates/pb/images/smile/sleep.gif\" align=bottom alt=\"$Vsleeping\" title=\"$Vsleeping\" border=\"0\"></a>&nbsp;";
		}
		echo "</td></tr>";
	}

}

function PrintPBCode($adm,$ulang)
{
	include("global.php");
	include("db/settings.php");
	include($dbpath."/settings/styles/styles.php");
	include("$temppath/pb/language/lang_$ulang.php");

	if ($bbcode || $adm){
		echo "<tr><td class=\"label\">$VPBCode:</td>";
		echo "<td class=\"content\"><a href=javascript:hyperlink()>";
		echo "<img src=\"templates/pb/images/code/url.gif\" align=bottom alt=\"$VHyperlink\" title=\"$VHyperlink\" border=\"0\" width=\"20\" height=\"20\"></a>";
		echo "&nbsp;<a href=javascript:image()><img src=\"templates/pb/images/code/image.gif\" align=bottom alt=\"$VImage\" title=\"$VImage\" border=\"0\" width=\"20\" height=\"20\"></a>";
		echo "&nbsp;<a href=javascript:emai1()><img src=\"templates/pb/images/code/email.gif\" align=bottom alt=\"$VEmail\" title=\"$VEmail\" border=\"0\" width=\"20\" height=\"20\"></a>";
		echo "&nbsp;<a href=javascript:glowy()><img src=\"templates/pb/images/code/glow.gif\" align=bottom alt=\"$VGlow\" title=\"$VGlow\" border=\"0\"></a>";
		echo "&nbsp;<a href=javascript:bold()><img src=\"templates/pb/images/code/bold.gif\" align=bottom alt=\"$VBold\" title=\"$VBold\" border=\"0\"></a>";
		echo "&nbsp;<a href=javascript:italicize()><img src=\"templates/pb/images/code/italic.gif\" align=bottom alt=\"$VItalicize\" title=\"$VItalicize\" border=\"0\"></a>";
		echo "&nbsp;<a href=javascript:quote()><img src=\"templates/pb/images/code/quote.gif\" align=bottom alt=\"$quotealt\" title=\"$quotetitle\" border=\"0\"></a></td></tr>";
	}
}

function regdone($username,$type,$lang) {
	include("global.php");
	include($dbpath."/settings.php");
	include_once("functions.php");
	include($dbpath."/settings/styles/styles.php");
	$language=$lang;
	include("$temppath/pb/language/lang_$language.php");

	WriteTableTop();
	echo "<tr><td class=\"header\" colspan=\"2\">$sitetitle :: $VRegistrationComplete</td></tr>";
	echo "<tr><td class=\"content\" colspan=\"2\">$ThanksForSigning - ";
	if ($type=="1"){
		echo "$VExpectConformationMail</td>";
	}else{
		echo "$VPlease <a href=\"login.php?lang=$lang\">$VLogin</a></td>";
	}
	echo "</tr></table></td></tr></table></td></tr></table>";

}

function sbot($formsheet) {
	if ($formsheet){
		echo "</form>";
	}
	echo "</td></tr></table></td></tr></table></td></tr></table>";
}

function SelectFontColor($Title,$FColor)
{
	global $VFontColor, $VTextColor;
	echo "<tr><td width=\"24%\" align=\"right\">$VFontColor:</td>
			<td width=\"24%\"><input type=\"text\" name=\"$Title\" value=\"$FColor\" size=\"10\"></td>
			<td width=\"15%\"><font color=\"$FColor\">$VTextColor</font></td>
			<td width=\"47%\">&nbsp;</td></tr>\n";
}
function SelectFontSize($Title,$CurrentSize,$Start,$End)
{
	global $VFontsize;
	echo "<tr><td width=\"24%\" align=\"right\">$VFontsize: </td><td width=\"15%\"><select name=\"$Title\">";
	for ($i=$Start;$i<=$End;$i++){
		if ($CurrentSize==$i."px" || $CurrentSize==$i.'pt'){
			echo "<option value=\"".$i."px\" selected>".$i."px</option>";
		}else{
			echo "<option value=\"".$i."px\">".$i."px</option>";
		}
	}
	echo "</select></td></tr>";
}

function SelectFontType($Title,$CurrentType)
{
	global $VFont;
	echo "<tr><td width=\"24%\" align=\"right\">$VFont: </td><td width=\"15%\"><select name=\"$Title\">";
	if ($CurrentType=="Arial"){
			echo "<option value=\"Arial\" selected>Arial</option>";
	}else{
			echo "<option value=\"Arial\">Arial</option>";
	}
	if ($CurrentType=="Verdana"){
			echo "<option value=\"Verdana\" selected>Verdana</option>";
	}else{
			echo "<option value=\"Verdana\">Verdana</option>";
	}
	if ($CurrentType=="Times"){
			echo "<option value=\"Times\" selected>Times</option>";
	}else{
			echo "<option value=\"Times\">Times</option>";
	}
	echo "</select></td></tr>";
}

function sendmail($recipient,$subject,$content)
{

global $adminemail,$dbpath;
include ($dbpath.'/settings/sendmail.php');
$handle=fsockopen($smtp_server,$port);
fputs($handle,"EHLO $mydomain\r\n");
fputs($handle,"AUTH LOGIN\r\n");
fputs($handle,base64_encode($smtpusername)."\r\n");
fputs($handle,base64_encode($password)."\r\n");
fputs($handle,"MAIL FROM:<$adminemail>\r\n");
fputs($handle,"RCPT TO:<$recipient>\r\n");
fputs($handle,"DATA\r\n");
fputs($handle,"TO: $recipient\r\n");
fputs($handle,"Subject: $subject\r\n");
fputs($handle,"$content\r\n");
fputs($handle,"QUIT\r\n");

}

function SetLanguage($uname)
{
	include("global.php");
	include($dbpath."/settings.php");
	global $registering;
	$filename=$dbpath."/members/".$uname;
	if ($uname!=""){
		if (file_exists($filename)){
			include($dbpath."/members/".$uname);
		}
	}
	if ($userlang!=""){
		$language=$userlang;
	}
	if ($registering){
		$language=$registering;
	}
	return $language;
}

function SetLock()
{
	global $dbpath;
	$starttime=time();
	$toolong=$starttime+4;
	$filename=$dbpath.'/block';
	while(file_exists($filename)){
		if (time()>$toolong){
			break;		//if the blockfile isn't deleted for some unknown reason, this will exit the loop and continue
		}
	}
	$fp=fopen($filename,'w');
	fputs($fp,"blocked");
	fclose($fp);
	// Read back!!!
	$fp=fopen($filename,"r");
	$test=fread($fp,filesize($filename));
	fclose($fp);
	if($test!="blocked"){
		print "Serious error: Server is temporaly down!!! Please come back tomorrow :(";
		// everything is fucked
		exit;
	}
}

function Setaliascolor($urank){
	global $dbpath,$admincolor,$modcolor,$friendcolor,$usercolor;
	global $group1color,$group2color,$group3color,$group4color,$group5color,$group10color,$group11color,$group12color;
//	include ($dbpath.'/settings/styles/styles.php');
	if ($urank=='7'){
		$namcol=$admincolor;
	}elseif ($urank=='6'){
		$namcol=$modcolor;
	}elseif ($urank=='8'){
		$namcol=$usercolor;
	}elseif ($urank=='9'){
		if ($friendcolor){
			$namcol=$friendcolor;
		}else{
			$namcol=$usercolor;
		}
	}
	if ($groupcolors){
		if ($urank=='1'){
			$namcol=$group1color;
		}elseif ($urank=='2'){
			$namcol=$group2color;
		}elseif ($urank=='3'){
			$namcol=$group3color;
		}elseif ($urank=='4'){
			$namcol=$group4color;
		}elseif ($urank=='5'){
			$namcol=$group5color;
		}elseif ($urank=='10'){
			$namcol=$group10color;
		}elseif ($urank=='11'){
			$namcol=$group11color;
		}elseif ($urank=='12'){
			$namcol=$group12color;
		}
	}else{
		if ($urank < '6' || $urank > '9'){
			$namcol=$usercolor;
		}
	}
	return($namcol);
}

function textparse($textstring)
{
	$textstring=str_replace(chr(34),"&#34;",$textstring);	//"
	$textstring=str_replace(chr(36),"&#36;",$textstring);	//$
	$textstring=str_replace(chr(92),"&#92;",$textstring);	//\
	$textstring=str_replace(chr(60),"&#60;",$textstring);	//<
	$textstring=str_replace(chr(62),"&#62;",$textstring);	//>
//	$textstring=str_replace(chr(44),"&#44;",$textstring);	//,
	$textstring=str_replace(chr(39),"&#39;",$textstring);	//'
	$textstring=str_replace(chr(13),'',$textstring);				//CR
	return $textstring;
}

function textparse1($textstring)
{
//     $textstring=str_replace(chr(13),"<BR>",$textstring);
	$textstring=str_replace(chr(13).chr(10),"<BR>",$textstring);
	$textstring=str_replace(chr(10),"<BR>",$textstring);
//	$textstring=str_replace(chr(92).chr(34),chr(34).".chr(34).".chr(34),$textstring);
//	$textstring=str_replace(chr(34),chr(34).".chr(34).".chr(34),$textstring);

	return $textstring;
}

function threadpages($currentpage,$pages,$cat,$fid,$pid)
{
	global $maxrpp,$dbpath,$VPage,$VOf;
	echo $VPage,'&nbsp;', $currentpage;
	if ($pages>0){
		echo ' ',$VOf,' ',$pages;
	}
	echo ' [';
	$firstreply='0';
	$firstpageprinted='0';
	for ($j=1;$j<=$pages;$j++){
		$pageprinted='0';
		while ($noofreps<$j*$maxrpp){
			$firstreply++;
			if(file_exists($dbpath.'/posts/'.$cat.'_'.$fid.'_'.$pid.'_r_'.$firstreply)&& !$pageprinted){
				if ($firstpageprinted){
					echo '&nbsp;|';
				}
				echo '&nbsp;<a href="post.php?cat='.$cat.'&fid='.$fid.'&pid='.$pid.'&page='.$j.'&r='.$firstreply.'">'.$j.'</A>&nbsp;';
				$pageprinted='1';
				$firstpageprinted='1';
				$noofreps++;
			}elseif(file_exists($dbpath.'/posts/'.$cat.'_'.$fid.'_'.$pid.'_r_'.$firstreply)){
				$noofreps++;
			}
			if ($firstreply>=$plastreply){
				$noofreps=($j*$maxrpp)+1;
			}
		}
	}
	echo ']';

}
function ucp($user,$lang) {
	include("global.php");
	include($dbpath."/settings.php");
	include_once("functions.php");
	include_once("ffunctions.php");
	include($dbpath."/settings/styles/styles.php");
	include($temppath."/pb/language/lang_".$lang.".php");
	global $langexpl;

	include($dbpath."/members/".$user);
	$realname=$userrealname;
	$alias=$useralias;
	$loc=$userlocation;
	$msn=$usermsn;
	$aim=$useraim;
	$yah=$useryahoo;
	$em=$useremail;
	$emhide=$useremhide;
	$pt=$userslogan;
	$sig=replacestuff($usersig);
	$sig=stripslashes($sig);
	$sig=str_replace("<br>","\n",$sig);
	$uav=$useravatar;
	$uwebav=$userwebavatar;
	$web=$userhomepage;
	$icq=$usericq;
	$qq=$userqq;
	$animsm=$useranimsmilies;
	$userlng=$userlang;

	WriteTableTop();
	echo "\n";
	echo "<tr><td bgcolor=\"$headercolor\" height=\"27\" background=\"$headergif\" colspan=\"2\">";
	echo "<a href=\"index.php\"><font color=\"$headerfont\">$sitetitle</a> :: $VUserControlPanel</font>";
//     echo "<tr><td bgcolor=\"$headercolor\" height=\"27\" background=\"$headergif\" colspan=\"2\"><font color=\"$headerfont\">$sitetitle :: $VUserControlPanel</font>";
	echo "<form method=\"post\" action=\"ucp.php?id=2&user=$user\"></td></tr>\n";
	echo "<tr><td class=\"subheader\" colspan=\"2\">$VPersonal $VSettings :: </td></tr>\n";
	echo "<tr><td class=\"label\">$VNewPassword:<br></td>";
	echo "<td class=\"content\"><input type=\"password\" name=\"npass\"></td></tr>\n";
	echo "<tr><td class=\"label\">$VRepeatPassword:<br></td>";
	echo "<td class=\"content\"><input type=\"password\" name=\"npass2\"></td></tr>\n";
	echo "<tr><td class=\"label\">$VRealname:<br></td>";
	echo "<td class=\"content\"><input type=\"text\" name=\"realname\" value=\"$realname\"></td></tr>\n";
	if ($allowalias=="1" || $useradmin=="1"){
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
	echo $VHideEmail;
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
		echo "<SCRIPT LANGUAGE=\"JavaScript\">";
		echo "function avchange(URL_List)\n";
		echo "{\nvar URL = URL_List.options[URL_List.selectedIndex].value;\ndocument.avatari.src=\"templates/pb/images/avatars/\"+URL;\n";
		echo "}\n</SCRIPT>";
		echo "<tr bgcolor=\"$fmenucolor\"><td height=\"25\" width=\"20%\" align=\"right\"><font color=\"$menufont\">$VAvatar:<br></font></td>";
		echo "<td height=\"25\" width=\"80%\" bgcolor=\"$menucolor\"><select name=\"av\" OnChange=\"avchange(this);\">";
		if (trim($uav)=="" && $uwebav==""){
			echo "<option value=\"none\" selected=\"selected\">$VNoAvatar</option>";
		}else{
			echo "<option value=\"none\">$VNoAvatar</option>";
		}
		$dir="$temppath/pb/images/avatars/";
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
		if ($uwebav!=""){
			echo "<option value=\"webav\" selected>$VURLAvatar</option></select>";
		}else{
			echo "<option value=\"webav\">$VURLAvatar</option></select>";
		}
		if ($uav!="" && $uav!="none" && $uav!="webav"){
			echo "&nbsp;<img src=\"templates/pb/images/avatars/$uav\" NAME=\"$VAvatar $VImage\" alt=\"$VImage\">";
		}elseif($uwebav!=""){
			echo "&nbsp;<img src=\"$uwebav\" NAME=\"$VAvatar $VImage\" alt=\"$VImage\">";
		}
		echo "</td></tr>\n";
		echo "<tr><td class=\"label\">$VURLAvatar<br></td>";
		echo "<td class=\"content\"><input type=\"text\" name=\"webav\" value=\"$uwebav\">$AvatarURLtip</td></tr>\n";
	}


	echo "<tr bgcolor=\"$fmenucolor\"><td height=\"25\" width=\"20%\" align=\"right\"><font color=\"$menufont\">$VEnableAnimSmilies:<br></font></td>";
	echo "<td height=\"25\" width=\"80%\" bgcolor=\"$menucolor\">";
	if ($animsm=="1"){
		echo "<input type=\"checkbox\" name=\"animsm\" value=\"1\" checked>";
	}else{
		echo "<input type=\"checkbox\" name=\"animsm\" value=\"1\">";
	}
	echo "</td></tr>\n";

	echo "<tr><td class=\"label\">$VLanguageSelection:<br></td>";
	echo "<td class=\"content\"><select name=\"ulang\">";
	LanguageSelection($userlng);
	echo "</select> $langexpl</td></tr>\n";

	echo "<tr><td class=\"label\"><font color=\"$menufont\">$VSignature:</font> <BR>(<a href=\"help.php\">$PBCodeAllowed</a>)<br></td>\n";
	echo "<td class=\"content\"><textarea name=\"sig\" cols=\"60\" rows=\"4\">$sig</textarea></td></tr>\n";

	echo "<tr bgcolor=\"$subheadercolor\"><td height=\"18\" background=\"$subheadergif\" colspan=\"2\"><font color=\"$subheaderfont\">$VSubmitForm :: </font></td></tr>\n";

	echo "<tr><td class=\"label\"><br></td>";
	echo "<td class=\"content\"><input type=\"submit\" name=\"Submit2\" value=\"$VSubmit\">\n";
	sbot(1);
}         //end function ucp

function Unlock()
{
	global $dbpath;
	@unlink($dbpath.'/block');
}

function wait($seconds){
	$start=time();
	while (time()<$start+$seconds){
	}
}
function writefooter($newestm) {
	global $footer,$fontcolor,$cver,$csubver,$cstat,$cstatlevel,$VImage;
	include_once('scan.php');

	$customfooter=replacestuff(stripslashes($footer));
	echo "<BR><center><font color=\"$fontcolor\">
	<A HREF=\"http://pblang.drmartinus.de/\" NAME=\"PBLang Project\" TITLE=\"PBLang Project\" target=\"_new\">";
	//Please do not remove the following copyright notice. Doing so would be illegal. You may use the footer option in the
	//admin section to add your own information
	echo 'Software PBLang ',$cver,'.',$csubver,$cstat,$cstatlevel,'</A> &copy; 2002-2003 by Martin Senftleben<br>',"\n",$customfooter,'</font></center>';
	$exectotal=microtime()-$exectime;
	echo '<center>';
	echo "<a href=\"#top\"><img src=\"templates/pb/images/arrow_up.gif\" border=\"0\" alt=\"$VImage\"></a><br>\n";
	echo '</center></td></tr></table></td></tr></table></body></html>';
}

function writeheader($newestm,$cat,$fid,$pid,$uname,$login,$section) {

	include("global.php");
	include($dbpath."/settings.php");
	include_once("ffunctions.php");
	include($dbpath.'/settings/styles/styles.php');
	include($temppath.'/pb/language/lang_en.php');
	if ($uname!=""){
		$language=SetLanguage($uname);
		$loggedin="1";
		include($dbpath.'/members/'.$uname);
		$admin=$useradmin;
		$ualias=$useralias;
	}
	$admin=$useradmin;
	include($temppath.'/pb/language/lang_'.$language.'.php');
	$newthreadimg=GetImage($stylepref,'newthread','butt/');
	$replyimg=GetImage($stylepref,'reply','butt/');
	$lockedimg=GetImage($stylepref,'reply-locked','butt/');
	$homeimg=GetImage($stylepref,'home','top/');
	$searchimg=GetImage($stylepref,'search','top/');
	$helpimg=GetImage($stylepref,'help','top/');
	$logoutimg=GetImage($stylepref,'logout','top/');
	$ucpimg=GetImage($stylepref,'usercp','top/');
	$pmimg=GetImage($stylepref,'pm','top/');
	$loginimg=GetImage($stylepref,'login','top/');
	$registerimg=GetImage($stylepref,'register','top/');
	$adminimg=GetImage($stylepref,'admin','top/');
	$memberslistimg=GetImage($stylepref,'list','top/');
	$topbgimage=GetImage($stylepref,'topbg','top/');
	$sitelogobg=GetImage($stylepref,'sitelogo','top/');

		if ($uname!="") {
		$filename = $dbpath."/pm/".$uname."_tot";
		if (!file_exists($filename))
			{$filename=str_replace("_"," ",$filename);}
		$fd = fopen ($filename, "r");
		$totalmsg = fread ($fd, filesize ($filename));
		fclose ($fd);
		$handle=opendir("$dbpath/pm");
		$totalnew=0;
		while($file=readdir($handle)){
			if (strpos($file,"pmstat")){
				if (substr($file,0,strlen($uname))==$uname){
					$filename="$dbpath/pm/$file";
					$fd=fopen($filename,"r");
					$len=filesize($filename);
					if($len>0){
						$pmstatus=fread($fd,$len);
					}else{
						$pmstatus=0;
					}
					if ($pmstatus=="1"){
						$totalnew++;
					}
				}
			}
		}
	}
	$langid=strtoupper(substr($LangLocale,0,2));
	echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">";
	echo "<html><head>\n";
	echo "<title>$sitetitle :: $section</title>\n";
	echo "<meta name=\"description\" content=\"$sitetitle :: running with PBLang $cver.$csubver\">";
	//echo "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=$Charset\">\n";
	echo "<meta http-equiv=\"Content-language\" content=\"$language\">\n";
	echo  '<style type="text/css">';
	include ("formstyles.php");
	echo '</style>';
	echo "</head>";
//     echo "<body bgcolor=\"$background\" leftmargin=\"12\" topmargin=\"12\" marginwidth=\"12\" marginheight=\"12\">";
	if ($boardbgimage!=""){
		$backgr=' background="templates/pb/images/'.$boardbgimage.'"';
	}else{
		$backgr="";
	}
	echo '<body bgcolor="',$background,'"',$backgr,'>';
	echo "<a name=\"top\"></a>";
	setlocale(LC_TIME,$LangLocale);
	$today = strftime($DateFormat2,time()+($timezone*3600));
	if ($boardwidth > '300'){
		$bwidth=$boardwidth;
	}elseif ($boardwidth < '101'){
		$bwidth=$boardwidth."%";
	}
	echo '<table width="980" border="0" cellpadding="1" cellspacing="0" align="',$boardalign,'">';
	echo "<tr bgcolor=\"$formborder\"><td>\n";
	echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=0>";
	echo "<tr bgcolor=\"$form\" valign=\"top\"><td colspan=2>";
	echo "<table cellspacing=\"0\" cellpadding=2 width=\"100%\" align=center bgcolor=\"$forumheaderborder\">\n";
	echo "<tr><td valign=\"top\" align=\"center\">";
	echo "<A href=\"http://www.wiredplane.com\" Target=\"NEW\"><img src=\"/images2/sitelogobg-wplabs.jpg\" alt=\"$sitetitle\" width=\"980\" height=\"65\"></A>";
	echo "</td></tr><tr>";
	echo "<td align=\"right\" valign=\"top\"><font style=\"color:$forumheaderwelcome;font-size:$fontsize\">";
	if($ualias!="Anonimus"){
		if ($uname!="") {
			echo "$VHello $ualias! $VYouHave <a href=\"pm.php\"> $totalmsg ";
			$ph = get_phrase_for_num('$VMessages', $totalmsg,$uname);    //this will figure out which plural form to use
			echo "$ph</a>. $VOfTheseNew: <a href=\"pm.php\">$totalnew</a>.";
		} else {
			echo "$VPlease <a href=\"login.php\">$VLogin</a> $VOr <a href=\"register.php\">$VRegister</a>.";
		}
	}
	echo "</font>&nbsp;<font style=\"color:$datecolor;font-size:$fontsize\">$TimeDate&nbsp;'$today'</font>";
	if ($maint=="1"){
		echo "<font size=\"+1\"><b>".$VMaintenanceMode."</b></font>\n";
	}
	echo "</td><td align=\"right\">";
	//include("donation.php");
	echo "</td></tr></table>\n";
	echo "</td></tr><tr valign=\"middle\" align=\"center\" bgcolor=\"$forumheadermenu\"><td height=\"1\">\n";
	if ($picturebuttons=="Yes"){
		echo "<a href=\"index.php\"><img src=\"$homeimg\" border=\"0\" alt=\"$homealt\" title=\"$hometitle\"></a>\n";
		//echo "&nbsp;<a href=\"search.php\"><img src=\"$searchimg\" border=\"0\" alt=\"$searchalt\" title=\"$searchtitle\"></a>\n";
		echo "&nbsp;<a href=\"help.php\"><img src=\"$helpimg\" border=\"0\" alt=\"$helpalt\" title=\"$helptitle\"></a>\n";
		if ($login=="1") {
				echo "&nbsp;<a href=\"logout.php\"><img src=\"$logoutimg\" border=\"0\" alt=\"$logoutalt\" title=\"$logouttitle\"></a>\n";
				echo "&nbsp;<a href=\"ucp.php\"><img src=\"$ucpimg\" border=\"0\" alt=\"$ucpalt\" title=\"$ucptitle\"></a>\n";
				echo "&nbsp;<a href=\"pm.php\"><img src=\"$pmimg\" border=\"0\" alt=\"$pmalt\" title=\"$pmtitle\"></a>\n";
		}
		if ($login=="0") {
				echo "&nbsp;<a href=\"login.php\"><img src=\"$loginimg\" border=\"0\" alt=\"$loginalt\" title=\"$logintitle\"></a>\n";
				echo "&nbsp;<a href=\"register.php\"><img src=\"$registerimg\" border=\"0\" alt=\"$registeralt\" title=\"$registertitle\"></a>\n";
		}
		if ($admin=="1") {
				echo "&nbsp;<a href=\"admin.php\"><img src=\"$adminimg\" border=\"0\" alt=\"$adminalt\" title=\"$admintitle\"></a>\n";
		}
		echo "&nbsp;<a href=\"memberslist.php?s=name&o=a\"><img src=\"$memberslistimg\" border=\"0\" alt=\"$memberslistalt\" title=\"$memberslisttitle\"></a>";
	}else{
		echo "<div class=\"nobuttonshead\"><a href=\"index.php\">$VHome</a>";
		echo "&nbsp;|&nbsp;<a href=\"search.php\">$VSearch</a>\n";
		echo "&nbsp;|&nbsp;<a href=\"help.php\">$VHelp</a>\n";
		if ($login=="1") {
		echo "&nbsp;|&nbsp;<a href=\"logout.php\">$VLogout</a>\n";
		echo "&nbsp;|&nbsp;<a href=\"ucp.php\">$VUserCP</a>\n";
		echo "&nbsp;|&nbsp;<a href=\"pm.php\">$VPM</a>\n";
		}
		if ($login=="0") {
		echo "&nbsp;|&nbsp;<a href=\"login.php\">$VLogin</a>\n";
		echo "&nbsp;|&nbsp;<a href=\"register.php\">$VRegister</a>\n";
		}
		if ($admin=="1") {
			echo "&nbsp;|&nbsp;<a href=\"admin.php\">$VAdminPC</a>\n";
		}
		echo "&nbsp;|&nbsp;<a href=\"memberslist.php?s=name&o=a\">$MemberList</a></div></td>\n";
	}
	echo "</td>";
	echo "<form method=GET action='http://www.google.com/custom' name='searchf' id='searchf'><td valign=center>";
	echo "<input type=hidden name=sitesearch value='http://www.wiredplane.com'><input type=hidden name=domains value='http://www.wiredplane.com'><input type=hidden name=client value='pub-5186671154037974'><input type=hidden name=forid value='1'><input type=hidden name=ie value='utf-8'><input type=hidden name=oe value='utf-8'><input type=hidden name=hl value='en'><input type=hidden name=cof value='GALT:#0066CC;GL:1;DIV:#FF9900;VLC:336633;AH:center;BGC:FFFFFF;LBGC:999999;ALC:000000;LC:000000;T:0066CC;GFNT:666666;GIMP:666666;FORID:1;'><input type=text name='q' size=13 value=''>&nbsp;<a href='javascript:document.forms[\"searchf\"].submit();'>$VSearch >></a>";
	echo "</td></form>";
	echo "</tr></tbody></table>";


		//Announcement-Box
		$cont=@file($dbpath."/announce.txt");
		$ann=@implode(" ",$cont);
		$ann=stripslashes($ann);
		if (trim($ann)!="" && $newestm=="top"){
			echo "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"$formbordersize\">\n";
			echo "<tr bgcolor=\"$form\" valign=\"top\"><td height=\"25\">";
			echo "<table cellspacing=\"$forumheaderbordersize\" cellpadding=2 width=\"100%\" align=center bgcolor=\"$forumheaderborder\">";
			echo "<tbody>\n";
			echo "<tr bgcolor=\"$forumheader\"><td>";
			echo "<table cellspacing=0 cellpadding=\"1\" width=\"100%\" border=0><tbody>\n";
			echo "<tr><TD class=\"announcement\" valign=\"middle\" align=\"left\">";
			//echo "<font color=\"$annfontcolor\" size=\"$annfontsize\">$ann</font></td>";
			echo "$ann</td>";
			echo "<td></td></tr></tbody></table>\n";
			echo "</tr></tbody></table>\n";
			echo "<br></td></tr></table>\n";
		}

		echo "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">";
		echo "<tr><td width=\"48%\"><font size=\"1\" color=\"$datecolor\">";
//          echo $TimeDate." ".$today;
		echo "</font></td>\n";
		echo "<td width=\"52%\" align=\"right\">";
		if ($newestm=="showit") {
		}
		if ($s=="showit2") {
				if ($picturebuttons=="Yes"){
					echo "<a href=\"ntopic.php?fid=$fid&cat=$cat\"><img src=\"$newthreadimg\" border=\"0\" alt=\"$newthreadalt\" title=\"$newthreadtitle\"></a>\n";
					echo " <a href=\"nreply.php?fid=$fid&cat=$cat&pid=$pid\"><img src=\"$replyimg\" border=\"0\" alt=\"$replyalt\" title=\"$replytitle\"></a>\n";
				}else{
					echo "<a href=\"ntopic.php?fid=$fid&cat=$cat\">$VNewTopic</a>";
					echo "<a href=\"nreply.php?fid=$fid&cat=$cat&pid=$pid\">&nbsp;$VReply";
				}
		}
		if ($s=="showitpostlocked") {
				if ($picturebuttons=="Yes"){
					echo "<a href=\"ntopic.php?fid=$fid&cat=$cat\"><img src=\"$newthreadimg\" border=\"0\" alt=\"$newthreadalt\" title=\"$newthreadtitle\"></a>\n";
					echo "<img src=\"$lockedimg\" border=\"0\" alt=\"$lockedalt\" title=\"$lockedtitle\">";
				}else{
					echo "<a href=\"ntopic.php?fid=$fid&cat=$cat\">$VNewTopic</A> $VLocked";
				}
		}
		echo "</td></tr></table>\n";
}         //end function writeheader

function WriteLog($logfile,$text)
{
	global $dbpath;
	$filename="$dbpath/$logfile";
	@chmod($filename,0666);
	$fd=@fopen($filename,"a");
	$acttime=@getdate();
	$logdate=$acttime['mday']."/".$acttime['mon']."/".$acttime['year']." - ".$acttime['hours'].":".$acttime['minutes'];
	@fputs($fd,$logdate.": ".$text."\n");
	@fclose;
}

function WriteTableTop()
{
	global $dbpath,$containerbordersize,$containerborder,$containerinner;
	echo '<table width="100%" border="0" cellpadding="',$containerbordersize,'" cellspacing="0">';
	echo '<tr bgcolor="',$containerborder,'"><td height="2">',"\n";
	echo '<table width="100%" border="0" cellpadding="0" cellspacing="0">';
	echo '<tr bgcolor="',$containerinner,'"><td height="2">',"\n";
	echo '<table width="100%" border="0" cellpadding="7" cellspacing="1">',"\n";
}

?>

