<?php 
include("config.php");
include("funcs.php");

	$email=_var("email");
	$fio=_var("fname")." "._var("lname");
	$fulldata="Order:"._var("orderid")."\n";
	$fulldata.="Client:"._var("fname")." "._var("lname")."\n";
	$fulldata.="Company:"._var("company")."\n";
	$fulldata.="Address:"._var("country")." "._var("zip")." "._var("state")." "._var("city")." "._var("add1")." "._var("add2")."\n";
	$fulldata.="Phone:"._var("phone")."\n";
	
	$psw=$kgpsw;
	// В качестве регключа используем email
	$regname=$email;// Если нужно использовать что-то  другое, то менять тут
	// Генерим ключ, записываем данные в файл и отсылаем письмо клиенту (с копией нам самим)
	$key=getKey($psw,$salt,$regname, $email, $fio, $appName, $fulldata,$emailsent,0,0);
	print $key;
?>
