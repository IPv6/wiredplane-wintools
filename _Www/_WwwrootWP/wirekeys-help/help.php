<?php
	include("../cgi-bin/wp_layout.php");
	$section = _var("section","contents");
	if($section=="wirekeys-help"){
		$section="contents";
	}
	DoWork("wk", "en", "wirekeys-help/".$section);
?>
