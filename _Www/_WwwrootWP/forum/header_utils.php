<?php
function str2UTF8($str)
{
	$str = iconv("windows-1251", "utf-8", $str);
	//$str=utf8_encode($str);
	return $str;
}
?>
