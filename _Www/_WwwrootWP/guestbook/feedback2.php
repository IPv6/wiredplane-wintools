<?

include "../cgi-bin/_util.php";

# CONFIGURATION
# Title of the Page
$title = 'Contact WiredPlane.com';

# Subject prefix for the mail arriving in your Mail Box... eg. [hpnadig.com-feedback]
$sub = '[WWW] ';

# The Mail address you want the mail to be sent to...
$address = 'support@wiredplane.com';

# The Mail address(es) you want to be included in the BCC list...
//$ccmail = 'alternative@wiredplane.com'; 
$ccmail = ''; //if you dont want use CC

# The Mail address(es) you want to be included in the BCC list...
//$bccmail = 'alternative2@wiredplane.com'; // 
$bccmail = ''; //if you dont want use BCC

# Error Messages... edit them to your needs :)
$errors['name'] = "No Name Entered!";
$errors['email'] = "Need proper mail - id";
$errors['website'] = "Need proper website address";
$errors['comments'] = "Please type in your comments!";
$errors['subject'] = "No Subject Selected!";

# input value cookies
if($submit) {
	SetCookie ("namec", $name, time()+2592000);
	SetCookie ("emailc", $replyemail, time()+2592000);
	SetCookie ("websitec", $website, time()+2592000);
	SetCookie ("wherec", $where, time()+2592000);
	SetCookie ("findc", $find, time()+2592000);
	SetCookie ("commentsc", $comments, time()+2592000);
	$namec = $name;
	$emailc = $replyemail;
	$websitec = $website;
	$wherec = $where;
	$findc = $find;
	$commentsc = $comments;
	SetCookie ("namec", $name, time());
	SetCookie ("emailc", $replyemail, time());
	SetCookie ("websitec", $website, time());
	SetCookie ("wherec", $where, time());
	SetCookie ("findc", $find, time());
	SetCookie ("commentsc", $comments, time());
}

$error_text="";

# Checks for a valid mail address...
function do_check_email($address) 
{
	return preg_match('/^([a-zA-Z0-9_\-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([a-zA-Z0-9\-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?)$/',$address);
}

# Checks for a valid website address...
function do_check_website($website) 
{
	return eregi('^([:/~a-zA-Z0-9_\-\.]+)\.([:/~a-zA-Z0-9]+)$',$website);
}

// Check for the fields.. yet to be customized.....
if($_POST['done'] == "yes")
{
	$error_text="";
	$formsent=false;
	if ($_POST['subject_pre']) {
		$iambot = protectInput(_var('orderGet'));
		if(strlen($iambot)>0){
			// Спаммеры
			$error_text="Required fields are not filled";
			$formsent=FALSE;
		}
		$captha=protectInput(_var('fio_added'));
		$captha=strtolower($captha);
		if($captha!="swell"){
			$error_text="Wrong word from the image";
			$formsent=FALSE;
		}
		$name = protectInput(_var('name'));
		$replyemail = protectInput(_var('replyemail'));
		$website = protectInput(_var('website'));
		$where = protectInput(_var('where'));
		$find = protectInput(_var('find'));
		$comments = protectInput(_var('comments'));
		$subject_pre = protectInput(join(',',$_POST['subject_pre']));
		$totalTest=$comments.$name.$replyemail.$website.$where.$find.$subject_pre;
		if(strlen($comments)==0 || strlen($name)==0 || stristr($totalTest,"+OK")!=FALSE  || stristr($totalTest,"Received:")!=FALSE){
			// Нас ломают!!!
			$error_text="Required fields are missing";
			$formsent=FALSE;
		}
		if(stristr($comments,'href=')!=false){
			$error_text="HTML links are not allowed!";
			$formsent=FALSE;
		}
		if($error_text==""){
			$subject = $sub.$subject_pre;
			
			$extra_hdr_str = "From: $name <$replyemail> \r\ncc: $ccmail \r\nbcc: $bccmail \r\nContent-type: text/html\r\n";
			
			$date = strftime('%c (%Z)');

			// if there is no 'http://' on website field
			if ( substr ($website, 0, 7) != "http://") {
				$url = "http://" . $website;
			}else {
				$url = $website;
			} // done!

			$body = "<p align='center'>$date Message from '$name' &lt;$replyemail&gt; <a href='$url' target='_blank'>$website</a>:<br> $comments <br /><br /> Location: '$where' / Find: '$find'</p>";
			$formsent = mail($address,$subject,$body,$extra_hdr_str); // BUILT IN PHP FUNCTION mail() TO SENT EMAIL
		}
	}else{
		$error_text="Choose message subject first";
	}
}
// end of checking

$language=_var("language","en");
if(!$formsent) { // if the email has not been sent, will display message above and form below
	Header("Location: /".$language."/commons/guestbook.php?print_message=Your%20message%20has%20not%20been%20delivered!%20".$error_text); 
}else{
	Header("Location: /".$language."/commons/about.php?print_message=Your%20message%20has%20been%20sent!"); 
}
?>
