<?php 
include "_util.php";
echo "true\n";
$e_to="support@wiredplane.com";
$e_from="Order-ipv6@smsproxy.ru";
$e_subject="WC key check! in Smsproxy";
$e_message=_var("code");
@mail($e_to, $e_subject, $e_message, "From: $e_from\r\nContent-Type: text/html; charset=UTF-8\r\n");

/*
http://www.domain.ru/sms.php?smsid=1174921221.133533&num=1171&operator=MTS_Moskva&user_id=891612345XX&cost=3.098&msg=xxx
*/
?>