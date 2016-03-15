<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "Спасибо за покупку!";
	}
	return "Registration successfull!";
}
function insertSection($l){
?>

<SCRIPT LANGUAGE="JavaScript">
<!-- Overture Services Inc. 07/15/2003
var cc_tagVersion = "1.0";
var cc_accountID = "1022155428";
var cc_marketID =  "0";
var cc_protocol="http";
var cc_subdomain = "convctr";
if(location.protocol == "https:")
{
    cc_protocol="https";
     cc_subdomain="convctrs";
}
var cc_queryStr = "?" + "ver=" + cc_tagVersion + "&aID=" + cc_accountID + "&mkt=" + cc_marketID +"&ref=" + escape(document.referrer);
var cc_imageUrl = cc_protocol + "://" + cc_subdomain + ".overture.com/images/cc/cc.gif" + cc_queryStr;
var cc_imageObject = new Image();
cc_imageObject.src = cc_imageUrl;
// -->
</SCRIPT>

<center>
Thank you for your interest to our product!<hr>
You order is placed for processing. You will receive your registration key soon.<br>
Please check mailbox used in registration process. Be sure to check susppicious or spam 
folders at your mailbox as our email can be falsly detected as spam.
<hr>
<form method=post action="../guestbook/uninstall.php">
We would be happy if you could answer 3 questions. If you have a couple of minutes,<br>
please fill out this form to help us create a more relevant and useful product. Thank you.<br>
<br>
<table border=0 width=60%>
<input type="hidden" name="action" value="send">
<INPUT type=hidden name="logtype" value="order-survey">
<tr><td align=left>- Where did you first hear/read about our product?
<tr><td align=left>- The main reasons why you finally registered it.
<tr><td align=left>- The main features of our program that made you hot for it.
</table>
<br><textarea cols=60 rows=10 name="opinion"></textarea>
<br><input type=submit value="Send your opinion to WiredPlane.com">
</form>

<!-- Google Code for Purchase Conversion Page -->
<script language="JavaScript" type="text/javascript">
<!--
var google_conversion_id = 1070265966;
var google_conversion_language = "en_US";
var google_conversion_format = "2";
var google_conversion_color = "CCCCCC";
if (1) {
  var google_conversion_value = 1;
}
var google_conversion_label = "Purchase";
//-->
</script>
<script language="JavaScript" src="http://www.googleadservices.com/pagead/conversion.js">
</script>
<noscript>
<img height=1 width=1 border=0 src="http://www.googleadservices.com/pagead/conversion/1070265966/?value=1&label=Purchase&script=0">
</noscript>
</center>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
