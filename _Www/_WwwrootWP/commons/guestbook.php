<?php 
function getTitle($l)
{
	if($l==1){
		return "Поддержка и Контакты";
	}
	return "Support and Contacts";
}

function insertSection($l){
?>
<div id=h1>Technical Questions</div>

If you have a technical question, please read information on our site, first.<br>
You can also ask your question at <a href="http://www.gamerotor.com/forum">our forum</a>.
Get a ready answer now!

<form action="../guestbook/feedback2.php" method="post">
<table border=0 width=100%><tr><td align=left>&nbsp;
<?php if($l==1){?>Задайте Ваш вопрос. Мы получим Ваше послание немедленно<?php }else{?>Ask your question<?php }?>
</td></tr></table>
<table cellpadding=2 border=0>
<tr>
<td align=left><b>Name</b>*
<td align=left><input class="input" type="text" name="name" size="20" value="" />
<tr>
<td align=left><b>E-mail</b>
<td align=left><input class="input" type="text" name="replyemail" size="20" value="" />

<tr>
<td align=left><b>Web Site</b>
<td align=left><input class="input" type="text" name="website" size="20" value="" />

<tr>
<td align=left colspan=2><b>Select the subject*</b><br />
	<input type="checkbox" name="subject_pre[]" value="Feedback" /> Feedback on the Site <br />
	<input type="checkbox" name="subject_pre[]" value="Order" /> Order product <br />
	<input type="checkbox" name="subject_pre[]" value="Suggestion" /> Suggestions <br />
	<input type="checkbox" name="subject_pre[]" value="Sales" /> Sales <br />
	<input type="checkbox" name="subject_pre[]" value="Other" /> Other <br />
	<span style="display: none;"><input type="checkbox" name="orderGet" value="orderGet" /> I am a 6ot (Do n0t check this if you are a human!) <br /></span>
	<b>If you refer to specific product, type it name</b><br /><input class="input" type="text" name="where" size="50"  value=""/><br />
<tr>
<td align=left colspan=2><b>Comments</b>*<br />
<textarea class="textarea" cols="40" rows="8" name="comments">Type your question/comment here...</textarea><br />
* = required<br />

<center>Enter text from this image (case insensitive)<br />
<img src="/images2/commons/captcha.jpg" width=200 height=70 border=0><br /><input type="text" name="fio_added" value="Type word from above*" size=30 /><br />
</center>
<br />
<tr>
<td align=center colspan=2>
<input type="hidden" name="done" value="yes" />
<input class="submit" type="submit" name="submit" value=" Send " />&nbsp;<input class="submit" type="reset" name="cancel" value="Cancel" onclick="location='http://www.wiredplane.com'" /><br />
</tr></table>
</form>
<br><br>
<div id=h1><?php if($l==1){?>Общие вопросы<?php }else{?>General Questions<?php }?></div>
<p>
<b>If you lost your registration code</b><br>
If you have lost  your registration code, please <a href="mailto:support@wiredplane.com?subject=Registration%20retrieval">Write email to us</a>. Please do <b>not</b> use the form above before having tried this first. Include some information about your order
<br><br>
<table border=0 cellspacing=0><tr><td valign="top" align="left">
<table border=0 cellspacing=0 cellpadding=3>
    <tr>
        <td style="border: 1px solid #ffffff">
            <a href="../commons/help.php">&nbsp;<b>Orders:</b></a>
        <td style="border: 1px solid #ffffff">&nbsp;
            <a href="../commons/help.php#cd">How to oder CD</a>&nbsp;|&nbsp;
            <a href="../commons/help.php#altern">I don't have a credit card</a>&nbsp;|&nbsp;
            <a href="../commons/help.php">Other questions...&nbsp;</a>

    <tr>
        <td style="border: 1px solid #ffffff">
            <a href="../commons/help.php">&nbsp;<b>Download:</b></a>
        <td style="border: 1px solid #ffffff">&nbsp;
            <a href="../commons/help.php#dprob">Problems with downloading</a>&nbsp;|&nbsp;
            <a href="../commons/help.php">Other questions...</a>

    <tr>
        <td style="border: 1px solid #ffffff">
            <a href="../commons/help.php">&nbsp;<b>Upgrades:</b></a>
        <td style="border: 1px solid #ffffff">&nbsp;
        <a href="../commons/help.php#what">What is an upgrade?</a>&nbsp;|&nbsp;
        <a href="../commons/help.php#getnew">How do I get the new version?</a>&nbsp;|&nbsp;
        <a href="../commons/help.php">Other questions...</a>

    <tr>
        <td style="border: 1px solid #ffffff">
            <a href="../commons/help.php">&nbsp;<b>Discounts:</b></a>
        <td style="border: 1px solid #ffffff">&nbsp;
            <a href="../commons/help.php#any">Are there any special offers?</a>&nbsp;|&nbsp;
            <a href="../commons/help.php">Other questions...</a>
</table>
</td></tr>
</table>
<hr>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
