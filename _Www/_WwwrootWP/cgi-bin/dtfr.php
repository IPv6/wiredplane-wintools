<?php
/* ===================================== */
$init_login_name = 'login';
$init_login = 'IPv6';
$init_psw_name = 'password';
$init_psw = 'TIRACE';
$init_baseurl = 'http://www.dtf.ru/';
$init_suburls = "forum/topics.php?id=82|forum/topics.php?id=72|forum/topics.php?id=19|forum/topics.php?id=23";
$init_suburls_checklinknew_regexp = "/(<tr.*>.*New.*<\/tr.*>)/i";
$init_suburls_extractlinks_regexp = "/".'.*topic\.php\?.*'."/i";
$init_dofollowloc = true;
$init_dophpinfo = false;
/* ===================================== */
function InternetCombineUrl($absolute, $relative) {
  $p = parse_url($relative);
  if($p["scheme"])return $relative;
  extract(parse_url($absolute));
  $path = dirname($path); 
  if($relative{0} == '/') {
      $cparts = array_filter(explode("/", $relative));
  }
  else {
      $aparts = array_filter(explode("/", $path));
      $rparts = array_filter(explode("/", $relative));
      $cparts = array_merge($aparts, $rparts);
      foreach($cparts as $i => $part) {
	  if($part == '.') {
	      $cparts[$i] = null;
	  }
	  if($part == '..') {
	      $cparts[$i - 1] = null;
	      $cparts[$i] = null;
	  }
      }
      $cparts = array_filter($cparts);
  }
  $path = implode("/", $cparts);
  $url = "";
  if($scheme) {
      $url = "$scheme://";
  }
  if($user) {
      $url .= "$user";
      if($pass) {
	  $url .= ":$pass";
      }
      $url .= "@";
  }
  if($host) {
      $url .= "$host/";
  }
  $url .= $path;

  return $url;
}
// ====================
$var_tempfile = realpath(".")."/php_cook1233.log";//tempnam('/tmp', 'php_cook1233');//should be writable
// ====================
if($init_dophpinfo){
	phpinfo();
}
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $init_baseurl."login.php");
curl_setopt($ch, CURLOPT_COOKIEJAR, $var_tempfile);
curl_setopt($ch, CURLOPT_POST, 1);
if($init_dofollowloc){
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
	//curl_setopt($ch, CURLOPT_RETURNTRANSFER, 0);
}
curl_setopt($ch, CURLOPT_POSTFIELDS, $init_login_name."=".$init_login."&".$init_psw_name."=".$init_psw);
ob_start();      // prevent any output
curl_exec ($ch); // execute the curl command
ob_end_clean();  // stop preventing output
curl_close ($ch);
unset($ch);
$resultinglinks = array();
$init_suburls_arr = explode("|",$init_suburls);
foreach ($init_suburls_arr as $suburl)
{
	//var_dump($suburl);
	// getting page
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
	if($init_dofollowloc){
		curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
		//curl_setopt($ch, CURLOPT_RETURNTRANSFER, 0);
	}
	curl_setopt($ch, CURLOPT_COOKIEFILE, $var_tempfile);
	curl_setopt($ch, CURLOPT_URL, $init_baseurl.$suburl);
	$buf2 = curl_exec ($ch);
	curl_close ($ch);
	unset($ch);
	//echo $buf2;
	// Extracting all blocks with new links
	preg_match_all ($init_suburls_checklinknew_regexp, $buf2, &$newblcmatches);
	$newblcmatches=$newblcmatches[1];
	foreach($newblcmatches as $newblock)
	{
		//print "<hr>";var_dump($newblock);print "<hr>";
		preg_match_all ("/a[\s]+[^>]*?href[\s]?=[\s\"\']+(.*?)[\"\']+.*?>"."([^<]+|.*?)?<\/a>/", $newblock, &$matches);//$buf2
		$matches = $matches[1];
		foreach($matches as $link)
		{
			if (preg_match($init_suburls_extractlinks_regexp, $link))
			{
				$link = InternetCombineUrl($init_baseurl.$suburl,$link);
				$exprts = "";
/*
				$ch = curl_init();
				curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
				if($init_dofollowloc){
					curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
					//curl_setopt($ch, CURLOPT_RETURNTRANSFER, 0);
				}
				curl_setopt($ch, CURLOPT_COOKIEFILE, $var_tempfile);
				curl_setopt($ch, CURLOPT_URL, $link);
				$buf3 = curl_exec ($ch);
				//print_r($buf3);
				preg_match($init_wall2_regexp, $buf3, $exprts);
				curl_close ($ch);
				unset($ch);
*/
				$resultinglinks[] = array(parentlink => $suburl, link => $link, content => $exprts);
			}
		}
	}
}
var_dump($resultinglinks);
// ====================
unlink($var_tempfile);
// ====================
?>
