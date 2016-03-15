<?php
function xap_code($login)
 {
 $path = ''; $file = ''; $site = str_replace('www.', '', $_SERVER["HTTP_HOST"]);
 if (strlen($_SERVER["REQUEST_URI"]) > 180) return; if ($_SERVER["REQUEST_URI"] == '') $_SERVER["REQUEST_URI"] = '/';
 $file = base64_encode("$_SERVER[REQUEST_URI]");
 $path_code = md5($file); $user_pref = substr($login, 0, 2);
 $path = substr($path_code, 0, 1).'/'.substr($path_code, 1, 2).'/';
 $domain = "$login.tnx.net";
 $path = "/users/$user_pref/$login/$site/$path$file.txt";
 if ($fp = fsockopen ("$domain", 80, $errno, $errstr, 7))
   {
   fputs ($fp, "GET $path HTTP/1.0\r\nhost: $domain\r\n\r\n");
   $fl = 0; while (!feof($fp)) {
       $str = trim(fgets($fp,4096));
       if ($str == 'HTTP/1.1 404 Not Found') return;
       if ($fl == 1) echo $str;
       if ($str == "") $fl = 1;
       }
   fclose ($fp);
   }
 }

 print("<center>Related links:<br>");
 print("<a href='http://www.gamerotor.com/'>Casual games</s><br>");
 print("</center>");
 //xap_code(strtolower("ipv6"));
?>

<?php 
/*print("<hr><center>");
define('_SAPE_USER', '6cce80cdb6d666c984f5d5f2b8231826'); 
if (phpversion() < '4.1.0') {
	$_SERVER = $HTTP_SERVER_VARS;
	$_GET    = $HTTP_GET_VARS;
}

require_once(realpath($_SERVER['DOCUMENT_ROOT']._SAPE_USER.'/sape.php')); 
$o['host'] = "outer-station.m6.net";
$o['request_uri'] =$_SERVER['SCRIPT_NAME']; //$_SERVER['REQUEST_URI'];
//$o['verbose']=true;
if($_SERVER['QUERY_STRING']!=""){
	$o['request_uri'].='?'.$_SERVER['QUERY_STRING'];
}
$sape = new SAPE_client($o);
echo $sape->return_links();

print("<!--");
//var_dump($_SERVER);//REQUEST_URI нету! :(
var_dump($o);
print("--><font size=-1>");
print("</font></center>");

unset($o);
*/
?>