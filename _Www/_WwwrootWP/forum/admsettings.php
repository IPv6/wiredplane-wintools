<?php

echo "<td class=\"adminform\">";
echo "<form method=\"post\" action=\"admin2.php?do=fsettings2\">";
//		echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\" bgcolor=\"$menucolor\">";
echo "<table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
echo "<tr><td colspan=\"2\">$VForumSettings:</td></tr>$LF";

if ($maint=="1") {
echo "<tr><td width=\"24%\" align=\"right\">$VMaintenanceMode: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"maint\" value=\"1\" checked> ($VOnlyAdmins)</td></tr>";
} else {
echo "<tr><td width=\"24%\" align=\"right\">$VMaintenanceMode: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"maint\" value=\"1\"> ($VOnlyAdmins)</td></tr>";
}
echo "<tr><td colspan=\"4\" class=\"subheader\"><b>$VBoardidentity:</b> </td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VTitle:</td><td width=\"76%\" valign=\"bottom\"> ";
echo '<input type="text" size="70" name="title" value="',stripslashes($sitetitle),'"></td></tr>';

//echo "<input type=\"hidden\" name=\"template\" value=\"pb\">";

echo "<input type=\"hidden\" name=\"charsets\" value=\"$charsets\">";
echo "<tr><td width=\"24%\" align=\"right\">$VSiteMotto:</td><td width=\"76%\" valign=\"bottom\">";
echo '<input type="text" size="70" name="motto" value="',stripslashes($sitemotto),'"></td></tr>';
echo "<tr><td width=\"24%\" align=\"right\" valign=\"top\">$VSiteLogo:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"40\" name=\"logo\" value=\"",$sitelogo,"\"><BR>$VSiteLogoExplain</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\" valign=\"bottom\">$VSitelogoWidth:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"10\" name=\"sitelogowidth\" value=\"$sitelogowidth\"><BR></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\" valign=\"bottom\">$VSitelogoHeight:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"10\" name=\"sitelogoheight\" value=\"$sitelogoheight\"><BR></td></tr>";
$footer1=textparse(stripslashes($footer));
echo "<tr><td width=\"24%\" align=\"right\">$VFooter:</td><td width=\"76%\" valign=\"bottom\">";
echo '<input type="text" size="70" name="footer" value="',$footer1,'"></td></tr>';

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VBoardappearance:</b> </td></tr>";
echo "<tr><td width=\"24%\" align=\"right\" valign=\"bottom\">$VBackgroundimage:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"40\" name=\"bbgimage\" value=\"$boardbgimage\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VBoardwidth:</td><td width=\"76%\" valign=\"bottom\"><input type=\"text\" size=\"3\" name=\"boardwidth\" value=\"$boardwidth\"> %&nbsp;";
if ($boardalign=="left"){
	echo " - $VAlign:&nbsp;<input type=\"radio\" name=\"boardalign\" value=\"left\" checked>&nbsp;$VLeft&nbsp;&nbsp;";
}else{
	echo " - $VAlign:&nbsp;<input type=\"radio\" name=\"boardalign\" value=\"left\">&nbsp;$VLeft&nbsp;&nbsp;";
}
if ($boardalign=="center"){
	echo "&nbsp;<input type=\"radio\" name=\"boardalign\" value=\"center\" checked>&nbsp;$VCenter&nbsp;&nbsp;";
}else{
	echo "&nbsp;<input type=\"radio\" name=\"boardalign\" value=\"center\">&nbsp;$VCenter&nbsp;&nbsp;";
}
if ($boardalign=="right"){
	echo "&nbsp;<input type=\"radio\" name=\"boardalign\" value=\"right\" checked>&nbsp;$VRight&nbsp;&nbsp;</td></tr>";
}else{
	echo "&nbsp;<input type=\"radio\" name=\"boardalign\" value=\"right\">&nbsp;$VRight&nbsp;&nbsp;</td></tr>";
}

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VBoardsettings:</b> </td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VMaxPostsReply:</td><td width=\"76%\" valign=\"bottom\"><input type=\"text\" size=\"5\" name=\"maxposts\" value=\"$maxposts\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VMaxPPP:</td><td width=\"76%\" valign=\"bottom\"><input type=\"text\" size=\"5\" name=\"maxppp\" value=\"$maxppp\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VMaxRPP:</td><td width=\"76%\" valign=\"bottom\"><input type=\"text\" size=\"5\" name=\"maxrpp\" value=\"$maxrpp\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VMaxLastPosts:</td><td width=\"76%\" valign=\"bottom\"><input type=\"text\" size=\"5\" name=\"maxlatest\" value=\"$maxlatest\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\" valign=\"top\">$VTextareawidth:</td>
	<td width=\"76%\" valign=\"bottom\"><input type=\"text\" size=\"5\" name=\"textareawidth\" value=\"$textareawidth\">&nbsp;$VExplainTextareawidth</td></tr>";
if ($mailenabled=="1") {
echo "<tr><td width=\"24%\" align=\"right\">$VAllowEmail: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"allowem\" value=\"1\" checked></td></tr>";
} else {
echo "<tr><td width=\"24%\" align=\"right\">$VAllowEmail: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"allowem\" value=\"1\"></td></tr>";
}
if ($notifyadmin=="1") {
echo "<tr><td width=\"24%\" align=\"right\">$VNotifyAdmin: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"notifyadm\" value=\"1\" checked></td></tr>";
} else {
echo "<tr><td width=\"24%\" align=\"right\">$VNotifyAdmin: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"notifyadm\" value=\"1\"></td></tr>";
}
if ($notifyadminrepl=="1") {
echo "<tr><td width=\"24%\" align=\"right\">$VNotifyAdminReplies: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"notifyadmrepl\" value=\"1\" checked></td></tr>";
} else {
echo "<tr><td width=\"24%\" align=\"right\">$VNotifyAdminReplies: </td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"notifyadmrepl\" value=\"1\"></td></tr>";
}
echo "<tr><td width=\"24%\" valign=\"top\" align=\"right\">$VLanguageSelection:</td>";
echo "<td width=\"76%\" valign=\"bottom\">";
echo "<select name=\"language\">";
LanguageSelection($language);
echo "</select>&nbsp;($langexpl)</td></tr>";

echo "<tr><td width=\"24%\" valign=\"top\" align=\"right\">$VTimeZone:</td>";
echo "<td width=\"76%\" valign=\"bottom\">";
if ($timezone==""){$timezone="0";};

echo "<select name=\"timezone\">";
for ($i=23;$i>=1;$i--){
if ($timezone=="+".$i)
	{echo "<option label=\"+".$i."\" selected>+".$i."</option>";}
else
	{echo "<option label=\"+".$i."\">+".$i."</option>";}
}
if ($timezone=="0")
	{echo "<option label=\"0\" selected>0</option>";}
else
	{echo "<option label=\"0\">0</option>";}
for ($i=1;$i<=23;$i++){
if ($timezone=="-".$i)
	{echo "<option label=\"-".$i."\" selected>-".$i."</option>";}
else
	{echo "<option label=\"-".$i."\">-".$i."</option>";}
}
echo "</select><br>";
$servertime=strftime($DateFormat2);
$localtime=strftime($DateFormat2,time()+$timezone*3600);
echo "$VServerTimezone: $servertime<br>$VLocalTimezone: $localtime<br>$VExplainTimezone</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VMaintReason:</td><td width=\"76%\" valign=\"top\">";
echo "<input type=\"text\" name=\"mr\" size=\"70\" value=\"$mreason\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VBannedReason:</td><td width=\"76%\" valign=\"top\">";
echo "<input type=\"text\" name=\"banreason\" size=\"70\" value=\"$banreason\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VDefaultSlogan:</td><td width=\"76%\" valign=\"top\" valign=\"bottom\">";
echo "<input type=\"text\" name=\"defslogan\" size=\"70\" value=\"$defaultslogan\"><br>($VExplainSlogan)</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$AdminEmail:</td><td width=\"76%\" valign=\"top\">";
echo "<input type=\"text\" name=\"adminemail\" size=\"70\" value=\"$adminemail\"><br>($AdminEmailReason)</td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VNotificationsettings:</b> </td></tr>";
echo "<tr><td width=\"24%\" align=\"right\" valign=\"top\">$VAnnounce:</td><td width=\"76%\" valign=\"bottom\">";
$filename=$dbpath."/announce.txt";
$fd=@fopen($filename,"r");
$announce=@fread($fd,@filesize($filename));
@fclose($fd);
$announce=stripslashes($announce);
//echo "<textarea cols=\"50\" rows=\"6\" name=\"announce\" value=\"$announce\">$announce</textarea></td></tr>";
echo "<textarea cols=\"50\" rows=\"6\" name=\"announce\">$announce</textarea></td></tr>";
if ($emsupp=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VEMSupport:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"emsupp\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VEMSupport:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"emsupp\" value=\"1\"></td></tr>";
}
echo "<input type=\"hidden\" name=\"smtpmail\" value=\"0\">";

echo "<tr><td width=\"24%\" align=\"right\">$VWelcomeMessage:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"70\" name=\"welcmess\" value=\"$welcomemessage\"></td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VAliassettings:</b> </td></tr>";
if ($allowalias=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VAllowAlias:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"allalias\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VAllowAlias:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"allalias\" value=\"1\"></td></tr>";
}
echo "<tr><td width=\"24%\" align=\"right\">$VChangeAlias:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"5\" name=\"chalias\" value=\"$changealias\">&nbsp;$VDays</td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VAvatarsettings:</b> </td></tr>";
if ($ave=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableAvatars:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"av\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableAvatars:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"av\" value=\"1\"></td></tr>";
}
if ($webave=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableWebAvatars:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"webav\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableWebAvatars:</td><td width=\"76%\" valign=\"bottom\"><input type=\"checkbox\" name=\"webav\" value=\"1\"></td></tr>";
}

echo "<tr><td width=\"24%\" align=\"right\" valign=\"bottom\">$VMaxAvatarWidth:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"10\" name=\"avmaxwidth\" value=\"$avmaxwidth\"><BR></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\" valign=\"bottom\">$VMaxAvatarHeight:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"10\" name=\"avmaxheight\" value=\"$avmaxheight\"><BR></td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VPostsettings:</b> </td></tr>";
if ($noatch=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VDisableAttachments: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"noatch\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VDisableAttachments: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"noatch\" value=\"1\"></td></tr>";
}
if ($attachtypelist==''){
	$filename=$dbpath.'/settings/attachtypes';
	$fd=fopen($filename,'r');
	$attachtypelist=trim(fread($fd,filesize($filename)));
	fclose($fd);
}
if (!$noatch){
	echo "<tr><td width=\"24%\" align=\"right\">$VAttachmenttypes:</td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"text\" size=\"70\" name=\"attachtypes\" value=\"$attachtypelist\"></td></tr>";
}else{
	echo "<input type=\"hidden\" name=\"attachtypes\" value=\"$attachtypelist\">";
}
echo "<tr><td width=\"24%\" align=\"right\">$VMaxAtchSize:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"5\" name=\"maxatchsize\" value=\"$maxatchsize\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VMinimumSpace:</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"text\" size=\"5\" name=\"minspace\" value=\"$minspace\"></td></tr>";
if ($censor=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VCensorWords: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"censor\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VCensorWords: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"censor\" value=\"1\"></td></tr>";
}
if ($bbcode=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableBBCode: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"bbcode\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableBBCode: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"bbcode\" value=\"1\"></td></tr>";
}
if ($smilys) {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableSmilies: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"smilys\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VEnableSmilies: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"smilys\" value=\"1\"></td></tr>";
}

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VMembershipsettings:</b> </td></tr>";
if ($allowreg=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VAllowNewUsers: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"allowreg\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VAllowNewUsers: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"allowreg\" value=\"1\"></td></tr>";
}
if ($confirmemail=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VRequestConfirmation: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"confirmemail\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VRequestConfirmation: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"confirmemail\" value=\"1\"></td></tr>";
}

if ($checkip=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$VCheckIP: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"chckip\" value=\"1\" checked></td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$VCheckIP: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"chckip\" value=\"1\"></td></tr>";
}
if ($loginreq=="1") {
	echo "<tr><td width=\"24%\" align=\"right\">$SetRequireLogin: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"logreq\" value=\"1\" checked> ($RequireLogin $VToViewForum)</td></tr>";
} else {
	echo "<tr><td width=\"24%\" align=\"right\">$SetRequireLogin: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"logreq\" value=\"1\"> ($RequireLogin $VToViewForum)</td></tr>";
}

echo "<tr><td width=\"24%\" align=\"right\">&nbsp;</td><td width=\"76%\" valign=\"bottom\">";
echo "<input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr></table></form>";

?>
