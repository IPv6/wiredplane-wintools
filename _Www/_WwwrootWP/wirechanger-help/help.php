<?php
	include("../cgi-bin/wp_layout.php");
	$section = _var("section","contents");
	if($section=="wirechanger-help"){
		$section="contents";
	}
	DoWork("wc", "en", "wirechanger-help/".$section);
?>
