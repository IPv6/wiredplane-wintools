<?php
include "../cgi-bin/_util.php";
$who=_var("who");
@mail("support@wiredplane.com", "Registration detected", "New registration from ".$who, "From: admin@wiredplane.com\r\nContent-Type: text/plain; charset=windows-1251\r\nContent-Transfer-Encoding: 8bit");
?>
