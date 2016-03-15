<?php

echo "<td class=\"adminform\">";
echo "<form method=\"post\" action=\"admin2.php?do=styles2\"><table width=\"100%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">";
echo "<tr><td colspan=\"4\"><font size=\"1\">$VStylesColors:</font></td></tr>";

echo "<tr><td colspan=\"4\" class=\"subheader\"><b>$VGenerell</b>: </td></tr>";
if ($picturebuttons=="Yes"){
	echo "<td width=\"24%\" align=\"right\">$Vpicturebuttons: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"picturebuttons\" value=\"Yes\" checked></td></tr>";
}else{
	echo "<td width=\"24%\" align=\"right\">$Vpicturebuttons: </td><td width=\"76%\" valign=\"bottom\">";
	echo "<input type=\"checkbox\" name=\"picturebuttons\" value=\"Yes\"></td></tr>";
}
echo "<tr><td width=\"24%\" align=\"right\" valign=\"top\">$VStylePrefix:</td>
	<td width=\"24%\" colspan=3><input type=\"text\" name=\"stylepref\" value=\"$stylepref\" size=\"20\">&nbsp;$VExplainStylePrefix</td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderFont</b>: </td></tr>";
SelectFontType("font",$font);
SelectFontColor("fontcolor",$fontcolor);
SelectFontSize("fontsize",$fontsize,8,16);

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleQuoteFont</b>: </td></tr>";
SelectFontType("quotefont",$quotefont);
SelectFontColor("quotefontcolor",$quotefontcolor);
SelectFontSize("quotefontsize",$quotefontsize,8,16);
echo "<tr><td width=\"24%\" align=\"right\">$VBackground:</td>
	<td width=\"24%\"><input type=\"text\" name=\"quotebackground\" value=\"$quotebackground\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$quotebackground\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VQuoteBorder:</td>
	<td width=\"24%\"><input type=\"text\" name=\"quoteborder\" value=\"$quoteborder\" size=\"10\"></td>
	<td width=\"15%\"></td><td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleAnnouncementFont</b>: </td></tr>";
SelectFontType("annfont",$annfont);
SelectFontColor("annfontcolor",$annfontcolor);
SelectFontSize("annfontsize",$annfontsize,8,16);

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderPage:</b> </td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VBackground:</td>
	<td width=\"24%\"><input type=\"text\" name=\"background\" value=\"$background\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$background\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VForm:</td>
	<td width=\"24%\"><input type=\"text\" name=\"form\" value=\"$form\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$form\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VFormBorder:</td>
	<td width=\"24%\"><input type=\"text\" name=\"formborder\" value=\"$formborder\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$formborder\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VFormBorderSize:</td>
	<td width=\"15%\"><input type=\"text\" name=\"formbordersize\" value=\"$formbordersize\" size=\"10\"></td>
	</tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderTitle:</b> </td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderBorder:</td>
	<td width=\"15%\"><input type=\"text\" name=\"forumheaderborder\" value=\"$forumheaderborder\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumheaderborder\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderBorderSize:</td>
	<td width=\"15%\"><input type=\"text\" name=\"forumheaderbordersize\" value=\"$forumheaderbordersize\" size=\"10\"></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VForumHeader:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumheader\" value=\"$forumheader\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumheader\">&nbsp;</td></tr></table></td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderTitle:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumheadertitle\" value=\"$forumheadertitle\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" bgcolor=\"$forumheader\"><font color=\"$forumheadertitle\"><b>$VTextColor</b></font></td></tr></table></td></tr>";


	echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderTitleSize:</td>
			<td width=\"24%\"><select name=\"forumheadertitlesize\">";
for ($i=8;$i<=36;$i++){
	if ($forumheadertitlesize==$i."px"){
		echo "<option value=\"".$i."px\" selected>".$i."px</option>\n";
	}else{
		echo "<option value=\"".$i."px\">".$i."px</option>\n";
	}
}
echo "</select></td></tr>\n";
echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderCaption:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumheadercaption\" value=\"$forumheadercaption\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" bgcolor=\"$forumheader\"><font color=\"$forumheadercaption\">$VTextColor</font></td></tr></table></td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderWelcome:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumheaderwelcome\" value=\"$forumheaderwelcome\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" bgcolor=\"$forumheader\"><font color=\"$forumheaderwelcome\">$VTextColor</font></td></tr></table></td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderMenu:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderMenu:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumheadermenu\" value=\"$forumheadermenu\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumheadermenu\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumHeaderMenuFont:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumheadermenufont\" value=\"$forumheadermenufont\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" bgcolor=\"$forumheadermenu\"><font color=\"$forumheadermenufont\">$VTextColor</font></td></tr></table></td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderDate:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VDateColor:</td>
	<td width=\"24%\"><input type=\"text\" name=\"datecolor\" value=\"$datecolor\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$datecolor\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderContainer:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VContainerBorder:</td>
	<td width=\"24%\"><input type=\"text\" name=\"containerborder\" value=\"$containerborder\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$containerborder\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VContainerInner:</td>
	<td width=\"24%\"><input type=\"text\" name=\"containerinner\" value=\"$containerinner\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$containerinner\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VContainerBorderSize:</td>
	<td width=\"24%\"><input type=\"text\" name=\"containerbordersize\" value=\"$containerbordersize\" size=\"10\"></td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderHeader:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VHeaderColor:</td>
	<td width=\"24%\"><input type=\"text\" name=\"headercolor\"  value=\"$headercolor\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$headercolor\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VHeaderGif:</td>
	<td width=\"15%\"><input type=\"text\" name=\"headergif\"  value=\"$headergif\" size=\"40\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td  width=\"100%\" height=\"25\"  background=\"$headergif\">&nbsp;</td></tr></table></td>
	</tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VHeaderFont:</td>
	<td width=\"24%\"><input type=\"text\" name=\"headerfont\" value=\"$headerfont\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" background=\"$headergif\"><font color=\"$headerfont\">$VTextColor</font></td></tr></table></td></tr>";


echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderSubheader:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VSubheaderColor:</td>
	<td width=\"24%\"><input type=\"text\" name=\"subheadercolor\" value=\"$subheadercolor\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$subheadercolor\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VSubheaderGif:</td>
	<td width=\"15%\"><input type=\"text\" name=\"subheadergif\" value=\"$subheadergif\" size=\"40\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" background=\"$subheadergif\">&nbsp;</td></tr></table></td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VSubheaderFont:</td>
	<td width=\"24%\"><input type=\"text\" name=\"subheaderfont\" value=\"$subheaderfont\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" background=\"$subheadergif\"><font color=\"$subheaderfont\">$VTextColor</font></td></tr></table></td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderMenu:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VMenuColor:</td>
	<td width=\"24%\"><input type=\"text\" name=\"menucolor\" value=\"$menucolor\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$menucolor\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VFMenuColor:</td>
	<td width=\"24%\"><input type=\"text\" name=\"fmenucolor\" value=\"$fmenucolor\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$fmenucolor\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VMenuFont:</td>
	<td width=\"24%\"><input type=\"text\" name=\"menufont\" value=\"$menufont\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr>
	<td width=\"100%\" height=\"25\" bgcolor=\"$fmenucolor\"><font color=\"$menufont\">$VTextColor</font></td></tr></table></td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderForumButton:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumButtonColor:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumbuttoncolor\" value=\"$forumbuttoncolor\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumbuttoncolor\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumButtonOver:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumbuttonover\" value=\"$forumbuttonover\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumbuttonover\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumButtonTopics:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumbuttontopics\" value=\"$forumbuttontopics\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumbuttontopics\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumButtonReplies:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumbuttonreplies\" value=\"$forumbuttonreplies\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumbuttonreplies\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VForumButtonLast:</td>
	<td width=\"24%\"><input type=\"text\" name=\"forumbuttonlast\" value=\"$forumbuttonlast\" size=\"10\"></td>
	<td width=\"15%\"><table width=\"100%\" border=\"1\"><tr><td width=\"100%\" bgcolor=\"$forumbuttonlast\">&nbsp;</td></tr></table></td>
	<td width=\"47%\">&nbsp;</td></tr>";


echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderUserColors:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VAdminColor:</td>
	<td width=\"15%\"><input type=\"text\" name=\"admincolor\" value=\"$admincolor\" size=\"10\"></td>
	<td width=\"15%\" ><font color=\"$admincolor\">Administrator</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VModColor:</td>
	<td width=\"15%\"><input type=\"text\" name=\"modcolor\" value=\"$modcolor\" size=\"10\"></td>
	<td width=\"15%\" ><font color=\"$modcolor\">Moderator</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VUserColor:</td>
	<td width=\"15%\"><input type=\"text\" name=\"usercolor\" value=\"$usercolor\" size=\"10\"></td>
	<td width=\"15%\" ><font color=\"$usercolor\">User</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderLinkColors:</b> </td></tr>";

echo "<tr><td width=\"24%\" align=\"right\">$VLinkColor:</td>
	<td width=\"15%\"><input type=\"text\" name=\"linkcolor\" value=\"$linkcolor\" size=\"10\"></td>
	<td width=\"15%\" ><font color=\"$linkcolor\">$VTextColor</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VVisitedLinkColor:</td>
	<td width=\"15%\"><input type=\"text\" name=\"vlinkcolor\" value=\"$vlinkcolor\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$vlinkcolor\">$VTextColor</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";
echo "<tr><td width=\"24%\" align=\"right\">$VHoverLinkColor:</td>
	<td width=\"15%\"><input type=\"text\" name=\"hlinkcolor\" value=\"$hlinkcolor\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$hlinkcolor\">$VTextColor</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

echo "<tr><td colspan=\"4\"  class=\"subheader\"><b>$VStyleHeaderMemgroupColors:</b> </td></tr>";

	if ($groupcolors=="1"){
		echo "<tr><td width=\"24%\" align=\"right\">$VGroupColors: </td><td width=\"76%\" valign=\"bottom\">";
		echo "<input type=\"checkbox\" name=\"groupcolors\" value=\"1\" checked></td></tr>";
	}else{
		echo "<tr><td width=\"24%\" align=\"right\">$VGroupColors: </td><td width=\"76%\" valign=\"bottom\">";
		echo "<input type=\"checkbox\" name=\"groupcolors\" value=\"1\"></td></tr>";
	}

$group=GetRank("1");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group1color\" value=\"$group1color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group1color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

$group=GetRank("2");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group2color\" value=\"$group2color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group2color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

$group=GetRank("3");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group3color\" value=\"$group3color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group3color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

$group=GetRank("4");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group4color\" value=\"$group4color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group4color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

$group=GetRank("5");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group5color\" value=\"$group5color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group5color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

$group=GetRank("10");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group10color\" value=\"$group10color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group10color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";

$group=GetRank("11");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group11color\" value=\"$group11color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group11color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";
$group=GetRank("12");
echo "<tr><td width=\"24%\" align=\"right\">$group:</td>
	<td width=\"15%\"><input type=\"text\" name=\"group12color\" value=\"$group12color\" size=\"10\"></td>
	<td width=\"15%\"><font color=\"$group12color\">$group</font></td>
	<td width=\"47%\">&nbsp;</td></tr>";
//echo "<input type=\"hidden\" name=\"stylepref\" value=\"$stylepref\">";

echo "<tr><td width=\"24%\" align=\"right\">&nbsp;</td><td width=\"76%\" valign=\"bottom\"><input type=\"submit\" name=\"Submit\" value=\"$VSubmit\"></td></tr>";
echo "</table></form>";

?>
