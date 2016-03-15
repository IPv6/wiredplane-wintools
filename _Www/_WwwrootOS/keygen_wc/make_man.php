<html>
<body>
<?php 
include("config.php");
include("funcs.php");

$act=_var("act");
if($act==""){
?>
<h1>KeyGen for <?php print $appName; ?>
<form method=post action="<?php print $_SERVER['PHP_SELF']; ?>">
<input type=hidden name="act" value="generate">
<table>
<tr><td>User FIO:<td><input name=regfio>
<tr><td>Registration name:<td><input name=regname> Leave blank to use Email
<tr><td>Email:<td><input name=email> <input type=checkbox name="nsendeml" value="1">Do not send email
<tr><td>Full order info:<td><textarea name=fulldata rows=20 cols=70></textarea>
<tr><td colspan=2><tr><td>Password:<td><input name=psw>&nbsp;<input type=submit>
</form>

<?php 
}else if($act=="generate"){
	$email=_var("email");
	$fio=_var("regfio");
	$fulldata=_var("fulldata");
	$psw=_var("psw");
	$nsendeml=_var("nsendeml");
	$regname=_var("regname");
	if(strlen($regname)==0){
		$regname=$email;
	}
	print "Generating key for '".$regname."'";
	$key=getKey($psw,$salt,$regname, $email, $fio, $appName, $fulldata,$emailsent,$nsendeml,1);
	print "<hr><pre>";
	print "<br>";
	print "Registration Name:      ".$email;
	print "<br>";
	print "Serial number:          ".$key;
	print "<br><hr>";
	print $emailsent;
	print "</pre>";
}else{
	print "<hr>Parameter error! act=".$act;
}
?>
</body>
</html>