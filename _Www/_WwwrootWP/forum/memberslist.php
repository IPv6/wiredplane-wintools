<?php
/***************************************************************************
 *                            memberlist.php  - PBLang
 *                            -------------------
 *
 *             See the file docs/copyright.txt for more details!
 *
 *		Last modified: 10 October 2003
 ***************************************************************************/


require('header.php');
include("global.php");
include($dbpath."/settings.php");
include($dbpath."/settings/styles/styles.php");
include_once("functions.php");
include_once("ffunctions.php");

$liusername=$HTTP_COOKIE_VARS[$cookieid];
$permit=CheckLoginStatus($liusername,"1","0");
if ($permit=="1") {
     $s=$HTTP_GET_VARS['s'];
     $o=$HTTP_GET_VARS['o'];
     $start=$HTTP_GET_VARS['st'];
     $end=$HTTP_GET_VARS['en'];
     $language=SetLanguage($liusername);
     include($temppath."/pb/language/lang_".$language.".php");
     include ($dbpath."/members/".$liusername);
     $valias=$useralias;
     writeheader($newestm,0,0,0,$liusername,"1",$MemberList);
     WriteTableTop();
     echo "<tr><td class=\"header\" colspan=\"8\">";
     echo "<a href=\"index.php\"><font color=\"$headerfont\">$sitetitle</a> :: $MemberList</font></td></tr>\n";
     echo "<tr>";
     echo "<td  class=\"subheader\" width=\"12%\">";
     if ($s=="name" && $o=="a"){
          echo "<a href=\"memberslist.php?s=name&o=d\" title=\"$VSortDescending\">$VAlias:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=name&o=a\" title=\"$VSortAscending\">$VAlias:</A>";
     }
     echo "<td class=\"subheader\" width=\"8%\">";
     if ($s=="posts" && $o=="d"){
          echo "<a href=\"memberslist.php?s=posts&o=a\" title=\"$VSortDescending\">$VPosts:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=posts&o=d\" title=\"$VSortAscending\">$VPosts:</A>";
     }
     echo "<td class=\"subheader\" width=\"12%\">";
     if ($s=="email" && $o=="a"){
          echo "<a href=\"memberslist.php?s=email&o=d\" title=\"$VSortDescending\">$VEmail:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=email&o=a\" title=\"$VSortAscending\">$VEmail:</A>";
     }
     echo "<td class=\"subheader\" width=\"12%\">";
     if ($s=="position" && $o=="d"){
          echo "<a href=\"memberslist.php?s=position&o=a\" title=\"$VSortDescending\">$VPosition:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=position&o=d\" title=\"$VSortAscending\">$VPosition:</A>";
     }
     echo "<td class=\"subheader\" width=\"12%\">";
     if ($s=="joined" && $o=="d"){
          echo "<a href=\"memberslist.php?s=joined&o=a\" title=\"$VSortDescending\">$VDateJoined:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=joined&o=d\" title=\"$VSortAscending\">$VDateJoined:</A>";
     }
     echo "<td class=\"subheader\" width=\"12%\">";
     if ($s=="lastvis" && $o=="d"){
          echo "<a href=\"memberslist.php?s=lastvis&o=a\" title=\"$VSortDescending\">$VLastVisit:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=lastvis&o=d\" title=\"$VSortAscending\">$VLastVisit:</A>";
     }
     echo "<td class=\"subheader\" width=\"12%\">";
     if ($s=="lastpost" && $o=="d"){
          echo "<a href=\"memberslist.php?s=lastpost&o=a\" title=\"$VSortDescending\">$VLastPost:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=lastpost&o=d\" title=\"$VSortAscending\">$VLastPost:</A>";
     }
     echo "<td class=\"subheader\" width=\"12%\">";
     if ($s=="visits" && $o=="d"){
          echo "<a href=\"memberslist.php?s=visits&o=a\" title=\"$VSortDescending\">$VTotalVisits:</A>";
     }else{
          echo "<a href=\"memberslist.php?s=visits&o=d\" title=\"$VSortAscending\">$VTotalVisits:</A>";
     }
     echo "</tr>\n";
     $n=0;
     $handle = opendir($dbpath.'/memss');
     while ($file = readdir($handle)) {
          if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.html") {
//               $username="";
		   		$uservisits=0;
               include($dbpath."/members/".$file);
               if ($username==$file){
                    $ulist[name][$n]=$username;
                    $ulist[posts][$n]=$userposts;
                    $ulist[email][$n]=strtolower($useremail);
                    $ulist[emhide][$n]=$useremhide;
                    $ulist[status][$n]=$userrank;
                    $ulist[joined][$n]=$userjoined;
                    $ulist[lastvis][$n]=$userlastvisit;
                    $ulist[lastpost][$n]=$userlastpost;
                    $ulist[visits][$n]=$uservisits;
                    $ulist[alias][$n]=$useralias;
					$ulist[cal][$n]=strtoupper($useralias);

					$fname=$dbpath."/settings/users";
					$userlist=@file($fname);
					$userlst=@implode(" - ",$userlist);
					if (!strpos(strtoupper($userlst),strtoupper($username))){
						$fp=fopen($fname,"a");
						fputs($fp,"$username\n");
						fclose($fp);
					}
					$n++;

               }
          }
     } //end while ($file...
     closedir($handle);
     switch ($s){
          case "name":
				if ($o==a){
					array_multisort($ulist[cal],SORT_ASC,SORT_REGULAR,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias]);
				}else{
					array_multisort($ulist[cal],SORT_DESC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias]);
				}
               break;
          case "posts":
				if ($o==a){
					array_multisort($ulist[posts],SORT_ASC,$ulist[name],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}else{
					array_multisort($ulist[posts],SORT_DESC,$ulist[name],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}
               break;
          case "email":
				if ($o==a){
					array_multisort($ulist[email],SORT_ASC,$ulist[name],$ulist[posts],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}else{
					array_multisort($ulist[email],SORT_DESC,$ulist[name],$ulist[posts],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}
               break;
          case "position":
				if ($o==a){
					array_multisort($ulist[status],SORT_ASC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}else{
					array_multisort($ulist[status],SORT_DESC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}
               break;
          case "joined":
				if ($o==a){
					array_multisort($ulist[joined],SORT_ASC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}else{
					array_multisort($ulist[joined],SORT_DESC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[lastvis],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}
               break;
          case "lastvis":
				if ($o==a){
					array_multisort($ulist[lastvis],SORT_ASC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}else{
					array_multisort($ulist[lastvis],SORT_DESC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastpost],$ulist[visits],$ulist[alias],$ulist[cal]);
				}
               break;
          case "lastpost":
				if ($o==a){
					array_multisort($ulist[lastpost],SORT_ASC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[visits],$ulist[alias],$ulist[cal]);
				}else{
					array_multisort($ulist[lastpost],SORT_DESC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[visits],$ulist[alias],$ulist[cal]);
				}
               break;
          case "visits":
				if ($o==a){
					array_multisort($ulist[visits],SORT_ASC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[alias],$ulist[cal]);
				}else{
					array_multisort($ulist[visits],SORT_DESC,$ulist[name],$ulist[posts],$ulist[email],$ulist[emhide],$ulist[status],$ulist[joined],$ulist[lastvis],$ulist[lastpost],$ulist[alias],$ulist[cal]);
				}
               break;
     }

     for ($i=0;$i<=($n-1);$i++){
          $pos=GetRank($ulist[status][$i]);
          $ujoined=strftime($DateFormat1,$ulist[joined][$i]);
          $lastvisit=strftime($DateFormat1,$ulist[lastvis][$i]);
          $lastpost=strftime($DateFormat1,$ulist[lastpost][$i]);
          echo "<tr bgcolor=\"$menucolor\">";
          echo '<td height="15" align="left" width="12%"><a href="profile.php?u=',$ulist[name][$i],'" alt="',$VProfileFor,' ',$ulist[alias][$i],'" title="',$VProfileFor,' ',$ulist[alias][$i],'">';
          echo $ulist[alias][$i]."</a>";
          if ($admin=="1"){
               echo "<form method=\"post\" action=\"admin2.php?do=remove2\">";
               echo '<input type="hidden" name="user" value="',$ulist[name][$i],'">';
               echo "<input type=\"submit\" name=\"Submit\" value=\"$VRemove\"></form>";
          }
          echo "</td>\n";
          echo "<td height=\"15\" width=\"8%\" valign=\"middle\" align=\"center\">".$ulist[posts][$i]."</td>";
          echo "<td height=\"15\" width=\"12%\">";

          if ($ulist[emhide][$i]=="hide" && !$admin){
               echo "$VHidden</td>";
          }else{
               echo "<a href=\"mailto:".$ulist[email][$i];
               if ($loggedin=="1") {
                         echo "?subject=E-mail from ".$valias;
               }
               echo "\">".$ulist[email][$i]."</a>";
               if ($admin && $ulist[emhide][$i]=="hide")
               {
                         echo " - $VHidden";
               }
               echo "</td>\n";
          }
          echo "<td height=\"15\" width=\"10%\">$pos</td><td height=\"15\" width=\"10%\" align=\"center\">$ujoined</td>\n
          <td height=\"15\" width=\"10%\" align=\"center\">$lastvisit</td><td height=\"15\" width=\"10%\" align=\"center\">$lastpost</td>
          <td height=\"15\" width=\"10%\" align=\"center\">".$ulist[visits][$i]."</td></tr>\n";
     }
     echo "</table></td></tr></table></td></tr></table>";
     writefooter($newestm);
}else{
     include("login.php");
}
ob_end_flush();
?>
