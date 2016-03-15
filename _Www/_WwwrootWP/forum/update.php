<?php
define('IN_PB', "true");
define('IN_PBG', "true");
include("db/settings.php");
include("global.php");
include_once("functions.php");
include_once("ffunctions.php");
include_once("scan.php");
include("$temppath/pb/language/lang_$language.php");

function GetAIM($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_aim";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     if (file_exists($filename)){
          $fd = fopen ($filename, "r");
          $aim = trim(fread ($fd, filesize ($filename)));
          fclose ($fd);
     }else{
          $aim="";
     }
     return($aim);
}

function GetAuthorsPosts($auth)
{
     include("global.php");
     $filename="$dbpath/members/$auth"."_p";
     if (!file_exists($filename))            //test if filename with spaces - if so call that up
               {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $count = trim(fread ($fd, filesize ($filename)));
     fclose ($fd);
     return($count);
}

function GetAvatar($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_av";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     $fd = @fopen ($filename, "r");
     $avatar = @fread ($fd, filesize ($filename));
     @fclose ($fd);
     return($avatar);
}

function GetEmail($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_em";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $email = fread ($fd, filesize ($filename));
     fclose ($fd);
     return($email);
}

function GetEmailHide($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_emhide";
     $fd = @fopen ($filename, "r");
     $emailhide = @fread ($fd, filesize ($filename));
     @fclose ($fd);
     return($emailhide);
}

function GetHomepage($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_web";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     if (file_exists($filename)){
          $fd = @fopen($filename, "r");
          $homepage = trim(@fread ($fd, filesize ($filename)));
          @fclose ($fd);
     }else{
          $homepage="";
     }
     return($homepage);
}

function GetICQ($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_icq";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     if (file_exists($filename)){
          $fd = @fopen ($filename, "r");
          $ficq = trim(@fread ($fd, filesize ($filename)));
          @fclose ($fd);
     }else{
          $ficq="";
     }
     return($ficq);
}

function GetMemberIP($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_ip";
     if (!file_exists($filename))
     {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $mip = fread ($fd, filesize ($filename));
     fclose ($fd);
     return($mip);
}

function GetMemberSlogan($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_pt";
     if (!file_exists($filename))
               {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $mline = fread ($fd, filesize ($filename));
     fclose ($fd);
     return($mline);
}

function GetMemberLoc($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_loc";
     if (!file_exists($filename))
               {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $mloc = fread ($fd, filesize ($filename));
     fclose ($fd);
     return($mloc);
}

function GetMemberPosts($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_p";
     if (!file_exists($filename))
               {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $mposts = fread ($fd, filesize ($filename));
     fclose ($fd);
     return($mposts);
}

function GetMemberRank($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_t";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $rnk = fread ($fd, filesize ($filename));
     fclose ($fd);
     return($rnk);
}

function GetMemberSignature($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_sig";
     if (!file_exists($filename))
               {$filename=str_replace("_"," ",$filename);}
     $fd = fopen ($filename, "r");
     $msig = fread ($fd, filesize ($filename));
     fclose ($fd);
     return($msig);
}

function GetMSN($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_msn";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     $fd = @fopen ($filename, "r");
     $msn = @fread ($fd, filesize ($filename));
     @fclose ($fd);
     return($msn);
}

function GetQQ($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_qq";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     if (file_exists($filename)){
          $fd = fopen ($filename, "r");
          $qq = trim(fread ($fd, filesize ($filename)));
          fclose ($fd);
     }else{
          $qq="";
     }
     return($qq);
}

function GetWebAvatar($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_webav";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     $fd = @fopen ($filename, "r");
     $webavatar = @fread ($fd, filesize ($filename));
     @fclose ($fd);
     return($webavatar);
}

function GetYahoo($user)
{
     include("global.php");
     $filename = "$dbpath/members/$user"."_yah";
     if (!file_exists($filename))
          {$filename=str_replace("_"," ",$filename);}
     $fd = @fopen ($filename, "r");
     $yahooim = @fread ($fd, filesize ($filename));
     @fclose ($fd);
     return($yahooim);
}

function WriteUserInfoa($user)
{
include("global.php");
     global $userid,$password,$username,$useralias,$userrealname,$useremail,$useremhide,$userip,$usersig,$userslogan,$useravatar,$usermsn;
     global $userposts,$usericq,$userqq,$userwebavatar,$userrank,$useryahoo,$useraim,$userlocation,$userjoined,$userhomepage,$useradmin;
     global $usermod,$userban,$userlastvisit,$userlastpost,$userprevvisit,$useranimsmilies,$lastaliaschange,$userfriend,$userlang,$uservisits;
     $content = "<?php\n\n";
     $content .="\$userid=\"$userid\";\n";
     $content .="\$password=\"$password\";\n";
     $content .="\$username=\"$username\";\n";
     $content .="\$useralias=\"$useralias\";\n";
     $content .="\$userrealname=\"$userrealname\";\n";
     $content .="\$useremail=\"$useremail\";\n";
     $content .="\$useremhide=\"$useremhide\";\n";
     $content .="\$userip=\"$userip\";\n";
     $content .="\$usersig=\"$usersig\";\n";
     $content .="\$userslogan=\"$userslogan\";\n";
     $content .="\$useravatar=\"$useravatar\";\n";
     $content .="\$usermsn=\"$usermsn\";\n";
     $content .="\$userposts=\"$userposts\";\n";
     $content .="\$usericq=\"$usericq\";\n";
     $content .="\$userqq=\"$userqq\";\n";
     $content .="\$userwebavatar=\"$userwebavatar\";\n";
     $content .="\$userrank=\"$userrank\";\n";
     $content .="\$useryahoo=\"$useryahoo\";\n";
     $content .="\$useraim=\"$useraim\";\n";
     $content .="\$userlocation=\"$userlocation\";\n";
     $content .="\$userjoined=\"$userjoined\";\n";
     $content .="\$userhomepage=\"$userhomepage\";\n";
     $content .="\$useradmin=\"$useradmin\";\n";
     $content .="\$usermod=\"$usermod\";\n";
     $content .="\$userban=\"$userban\";\n";
     $content .="\$userfriend=\"$userfriend\";\n";
     $content .="\$userlastvisit=\"$userlastvisit\";\n";
     $content .="\$userlastpost=\"$userlastpost\";\n";
     $content .="\$userprevvisit=\"$userprevvisit\";\n";
     $content .="\$useranimsmilies=\"$useranimsmilies\";\n";
     $content .="\$lastaliaschange=\"$lastaliaschange\";\n";
     $content .="\$userlang=\"$userlang\";\n";
     $content .="\$uservisits=\"$uservisits\";\n";
     $content .="\n?";
     $content .=">";

     $fp = fopen("$dbpath/members/$user", "w");
     fputs($fp, $content);
     fclose($fp);
     $fp=fopen("$dbpath/aliases","a");
     fputs($fp,$useralias."\n");
     fclose($fp);
}


$handle=opendir("db/memss");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.html") {

          $filename="$dbpath/settings/userid";
          $fd=@fopen($filename,"r");
          $userid=@fread($fd,filesize($filename));
          @fclose($fd);
          $userid++;
          $fp=fopen("$dbpath/settings/userid","w");
          fputs($fp,$userid);
          $fclose;

          $u=$file;
          $filename="$dbpath/members/$u";
          $username="";
          $useremail="";
          $useralias="";
          $userjoined="";
          $fd=fopen($filename,"r");
          $memtype=fread($fd,5);
          if ($memtype=="<?php"){
               echo "$VAlreadyUpdated!";
               exit;
               fclose($fd);
          }else{
               fclose($fd);
               $fd=fopen($filename,"r");
               $pass=fread($fd,filesize($filename));
               fclose($fd);
               $password=md5($pass);

               $username=$u;
               if ($useralias==""){
                    $useralias=$u;
               }
               if ($userjoined==""){
                    $userjoined=filemtime($filename);
                    if (!$userjoined){
                         $userjoined=time();
                    }
               }
               $userip=GetMemberIP($u);
               $userrank=GetMemberRank($u);
               if ($userrank=="7"){
                    $useradmin="1";
               }else{
                    $useradmin="0";
               }
               if ($userrank=="6"){
                    $usermod="1";
               }else{
                    $usermod="0";
               }
               if ($userrank=="8"){
                    $userban="1";
               }else{
                    $userban="0";
               }
               $userposts=GetMemberPosts($u);
               $slogan=stripslashes(GetMemberSlogan($u));
               $userslogan=addslashes($slogan);
               $usermsn=GetMSN($u);
               $useryahoo=GetYahoo($u);
               $useraim=GetAIM($u);
               $usericq=GetICQ($u);
               $userqq=GetQQ($u);
               $userlocation=GetMemberLoc($u);
               $sig=stripslashes(GetMemberSignature($u));
               $usersig=addslashes($sig);
               $useravatar=GetAvatar($u);
               $useremail=GetEmail($u);
               $useremhide=GetEmailHide($u);
               $homepage=stripslashes(GetHomepage($u));
               $userhomepage=addslashes($homepage);
               $webavatar=stripslashes(GetWebAvatar($u));
               $userwebavatar=addslashes($webavatar);
               $ulastvisit=filemtime($dbpath."/members/".$u."_ip");
               if ($ulastvisit!=""){
                    $userlastvisit=$ulastvisit;
               }else{
                    $userlastvisit=time();
               }
               $ulastpost=filemtime($dbpath."/members/".$u."_p");
               if ($ulastpost!=""){
                    $userlastpost=$ulastpost;
               }else{
                    $userlastpost=time();
               }
               $userrealname="";
               $userlang=$language;
               $uservisits=0;
               WriteUserInfoa($u);
               @unlink("$dbpath/members/$u"."_aim");
               @unlink("$dbpath/members/$u"."_av");
               @unlink("$dbpath/members/$u"."_em");
               @unlink("$dbpath/members/$u"."_emhide");
               @unlink("$dbpath/members/$u"."_icq");
               @unlink("$dbpath/members/$u"."_ip");
               @unlink("$dbpath/members/$u"."_loc");
               @unlink("$dbpath/members/$u"."_msn");
               @unlink("$dbpath/members/$u"."_p");
               @unlink("$dbpath/members/$u"."_pt");
               @unlink("$dbpath/members/$u"."_qq");
               @unlink("$dbpath/members/$u"."_sig");
               @unlink("$dbpath/members/$u"."_t");
               @unlink("$dbpath/members/$u"."_web");
               @unlink("$dbpath/members/$u"."_webav");
               @unlink("$dbpath/members/$u"."_yah");
               @unlink("$dbpath/members/$u"." aim");
               @unlink("$dbpath/members/$u"." av");
               @unlink("$dbpath/members/$u"." em");
               @unlink("$dbpath/members/$u"." emhide");
               @unlink("$dbpath/members/$u"." icq");
               @unlink("$dbpath/members/$u"." ip");
               @unlink("$dbpath/members/$u"." loc");
               @unlink("$dbpath/members/$u"." msn");
               @unlink("$dbpath/members/$u"." p");
               @unlink("$dbpath/members/$u"." pt");
               @unlink("$dbpath/members/$u"." qq");
               @unlink("$dbpath/members/$u"." sig");
               @unlink("$dbpath/members/$u"." t");
               @unlink("$dbpath/members/$u"." web");
               @unlink("$dbpath/members/$u"." webav");
               @unlink("$dbpath/members/$u"." yah");
               echo "updated $u<BR>";
          }
     }
}
closedir($handle);
echo "Done updating members!<br>";

//this updates the dates in messages and categories

$handle=opendir($dbpath."/cats");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.htm") {
          if (substr($file,strlen($file)-3,3)=="_lp" || substr($file,strlen($file)-3,3)==" lp"){
               $filename="$dbpath/cats/$file";
               $fd=fopen($filename,"r");
               $fdate=fread($fd,filesize($filename));
               fclose($fd);
               //Could be "$VNever!!!! - file is empty, or contains the word $VNever
               if ($fdate=="" || $fdate==$VNever){
                    $newdate="";
               }else{
                    $newdate=filemtime($filename);
               }
               $fp=fopen($filename,"w");
               fputs($fp,$newdate);
               fclose($fp);
//               $prdate=date("d. F Y, G:i",$newdate);
//               echo "$file: $fdate - written: $newdate = $prdate<br>";
          }
     }
}
closedir($handle);
//updating dates of posts
$handle=opendir($dbpath."/posts");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.htm") {
          if (substr($file,strlen($file)-4,4)=="_lpd" || substr($file,strlen($file)-3,3)=="_pa" || substr($file,strlen($file)-4,4)==" lpd" || substr($file,strlen($file)-3,3)==" pa" ){
               $filename="$dbpath/posts/$file";
               $fd=fopen($filename,"r");
               $fdate=fread($fd,filesize($filename));
               fclose($fd);

               $newdate=filemtime($filename);

               $fp=fopen($filename,"w");
               fputs($fp,$newdate);
               fclose($fp);
//               $prdate=date("d. F Y, G:i",$newdate);
//               echo "$file: $fdate - written: $newdate = $prdate<br>";
          }
     }
}
closedir($handle);
// -- update the date of the pms
$handle=opendir($dbpath."/pm");
while ($file = readdir($handle)) {
     if ($file != "." && $file != ".." && $file !=".htaccess" && $file !="index.htm") {
          if (substr($file,strlen($file)-2,2)=="_d"){
               $filename="$dbpath/pm/$file";

               $newdate=filemtime($filename);

               $fp=fopen($filename,"w");
               fputs($fp,$newdate);
               fclose($fp);
//               $prdate=date("d. F Y, G:i",$newdate);
//               echo "$file: $fdate - written: $newdate = $prdate<br>";
          }
     }
}
closedir($handle);
echo "Done updating dates!<br>";
?>
