<?php 
include("config.php");
include("funcs.php");

	$email=_var("email");
	$fio=_var("initals")." "._var("name");
	$fulldata="Order:"._var("o_no")."\n";
	$fulldata.="Client:"._var("title")." "._var("initals")." "._var("name")."\n";
	$fulldata.="Company:"._var("co_name")."\n";
	$fulldata.="Address:"._var("add1")." "._var("add2")." "._var("add3")." "._var("add4")." "._var("add5")." "._var("add6")."\n";
	$fulldata.="DClient:"._var("d_title")." "._var("d_initals")." "._var("d_name")."\n";
	$fulldata.="DCompany:"._var("d_co_name")."\n";
	$fulldata.="DAddress:"._var("d_add1")." "._var("d_add2")." "._var("d_add3")." "._var("d_add4")." "._var("d_add5")." "._var("d_add6")."\n";
	$fulldata.="Phone:"._var("phone")."\n";
	$fulldata.="UText:"._var("user_text")."\n";
	$fulldata.="IP:"._var("ip")."\n";
	
	$psw=$kgpsw;
	// В качестве регключа используем email
	$regname=$email;// Если нужно использовать что-то  другое, то менять тут
	// Генерим ключ, записываем данные в файл и отсылаем письмо клиенту (с копией нам самим)
	$key=getKey($psw,$salt,$regname, $email, $fio, $appName, $fulldata,$emailsent,0,0);
	print "<softshop>";
	print $key;
	print "</softshop>";
?>
