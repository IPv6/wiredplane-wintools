<?php 
include "_util.php";
$smsid=_var("smsid");
echo "smsid:$smsid\n";
echo "status:reply\n";
echo "content-type:text/plan\n";
echo "\n";
echo "Thank you! Your code is 1234!\n";

$e_to="support@wiredplane.com";
$e_from="Order-ipv6@smsproxy.ru";
$e_subject="Order in Smsproxy";
$e_message=$smsid;
@mail($e_to, $e_subject, $e_message, "From: $e_from\r\nContent-Type: text/html; charset=UTF-8\r\n");

/*

http://www.domain.ru/sms.php?smsid=1174921221.133533&num=1171&operator=MTS_Moskva&user_id=891612345XX&cost=3.098&msg=xxx
*/
?>