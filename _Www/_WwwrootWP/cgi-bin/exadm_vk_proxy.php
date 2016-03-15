<?php
/* ===================================== */
$init_login_name = 'login';
$init_login = 'IPv6';
$init_psw_name = 'password';
$init_psw = 'TIRACE';
$init_baseurl = 'http://62.213.68.226/test/ex/stats.php?raw=1';//'http://62.213.68.226/adm/?admpage=admin_stats_tdy&admauth=ipv6:imurkola_ex&admpview=1';
$init_dophpinfo = false;
$init_title = "ExAdmVk";
$init_cc_from="rss2email_proxy_vk@dev.null";
$init_cc2email="razinkov@gmail.com";
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
$var_tempfile = realpath(".")."/php_cook1213.log";//tempnam('/tmp', 'php_cook1233');//should be writable
// ====================
if($init_dophpinfo){
	phpinfo();
}
$currdate=time()+(3600*GMTOFFSET)-date("Z")+3600*(1-date("I"));
$tzone=sprintf('%s%02d',(GMTOFFSET<0)?'-':'+',abs(GMTOFFSET));
$rdate=date("D, d M Y H:i:s",$currdate)." ".$tzone."00";
$rss_encoding="utf-8";//"Win-1251";

$output="";
header("Content-Type:·text/xml;charset=$rss_encoding");
$output.="<?xml version=\"1.0\" encoding=\"$rss_encoding\"?>\n";
$output.="<rss version=\"2.0\">\n";
$output.="<channel>\n";
$output.="<title>".$init_title."</title>\n";
$output.="<link></link>\n";
$output.="<pubDate>$rdate</pubDate>\n";
$output.="<ttl>50</ttl>\n";
$output.="<description></description>\n";
$output.="<lastBuildDate>$rdate</lastBuildDate>\n";

	$hour = date("G");
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $init_baseurl);
	curl_setopt($ch, CURLOPT_COOKIEJAR, $var_tempfile);
	//curl_setopt($ch, CURLOPT_POST, 1);
	//curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
	$buf = curl_exec ($ch); // execute the curl command
	curl_close ($ch);
	unset($ch);

	$output.="<item>\n";
	$output.="<title><![CDATA[Stats (".$rdate."]]></title>\n";
	$output.="<link></link>\n";
	$output.="<pubDate>$rdate</pubDate>\n";
	$output.="<guid isPermaLink=\"false\">$rdate</guid>\n";
	$output.="<description><![CDATA[".$buf."]]></description>\n";
	$output.="</item>\n";

//if($hour == "21" || $hour == "22" || $hour == "23")
//{
//	@mail($init_cc2email, "[JRD] ".$init_title, $buf, "From: $init_cc_from\r\nContent-Type: text/html; charset=UTF-8\r\n");
//}

$output.="</channel>\n";
$output.="</rss>\n";
echo $output;
// ====================
@unlink($var_tempfile);
// ====================
?>
