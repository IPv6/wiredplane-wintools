<?php 
function getTitle($l)
{
	if($l==1){
		return "Помощь";
	}
	return "Help and Support";
}

function insertSection($l){
?>
<div id=h1><?php if($l==1){?>Как приобрести наши продукты<?php }else{?>How to order our products<?php }?></div>
<br>
<p><a name=cd><font color="#800000"><u>How to order a software application on CD?</u></font></a> 

<blockquote> 
<p>Usually, when you order our software, you get your registration code via e-mail and can download the file from our site. 
Alternatively, you can buy a CD-ROM version for additional $15 (not recommended though as once a newer version is released, 
the version that came on the CD-rom becomes obsolete).

<p>We deliver CDs only to users who paid for the registration code. Therefore, you should:

<ol>
<li>Order the software (<a href="../commons/order.php">list of links to order our products</a>) via RegNow;</li>
<li>Check "CD" option during registration process</li>
</ol>
</blockquote>
 
<p><a name=altern><font color="#800000"><u>I don't have a credit card. Is there an alternative payment method for the product?</u></font></a> 

<blockquote> 
<p>Most of our products are sold via SWREG registration service, which lets you pay not only with a credit card, but also with:
<ul>
<li>Wire or local money transfer (including SWIFT)</li> 
<li>US or UK checks or International Money Orders</li> 
<li>PayPal</li> 
</ul>  
</blockquote>

<p><a name=when><font color="#800000"><u>If I order now, when will I get my registration code?</u></font></a> 

<blockquote>
<p>We usually send registration codes right after your order is processed. 
If you have not received your registration within two business days *, please contact us via <a href="../commons/guestbook.php">online form</a>.
<p>* if you paid via PayPal system, it can take up to 5 working days for Paypal e-checks to clear.
</blockquote>


<p><a name=nofield><font color="#800000"><u>I am going to buy the program, but I see no 
field to enter the registration code in.</u></font></a> 
<blockquote>
<p>When you buy the program, you will get instructions of how to register.
</blockquote>           


<p><a name=invoice><font color="#800000"><u>I need an invoice. How do I get it?</u></font></a> 
<blockquote>
<p>After you buy a product, you automatically get a message that contains URL for your receipt. 
Or, you will be prompted to go to your receipt URL at the end of the ordering process. The registration 
services we work with, do not send paper invoices. 

<p>Please note that the charge will appear on your bill not from "WiredPlane.com", but from the registration 
service you used for buying the software.

<p>If you misplace your receipt URL, you can find it by the following way:

<ul>
<li>Find out which registration service you used to pay for the product.<br>&nbsp;</li> 

<li>If you used <a href="http://www.swreg.org">SWREG</a> registration service, 
go to <a href="http://cardquery.com">http://cardquery.com</a>. If you visit that website, 
you can also prepare and print out a receipt. This is valid for credit card payments only.<br>&nbsp;</li> 

<li>If you used <a href="http://www.regnow.com">RegNow</a> registration service, 
go to <a href="http://www.regnow.com/cs/orderStatus">http://www.regnow.com/cs/orderStatus</a>. 
At that link, you will be asked to enter the last 5 digits of your credit card number and 
your email address to reach the receipt. If you did not pay by credit card then please contact RegNow.
</li> 
</ul> 
</blockquote> 

<p><a name=nofield><font color="#800000"><u>How can I buy using the 'Purchase Order' (Proforma Invoice) method?</u></font></a> 
<blockquote>
<p>This method is available if you pay via <a href="http://www.swreg.org">SWREG</a> registration service 
that is used for the majority of our products. Just select the "Wire or local money transfer" method 
when placing your online order, you will then be able to print an invoice and get instructions on the correct bank account to pay.
</blockquote>   

<p><a name=nofield><font color="#800000"><u>I made two orders and I am going to pay using wire transfer 
method. Is it possible for me to make only one wire transfer for these two orders?</u></font></a> 
<blockquote>
<p>You can send one wire transfer for two orders, you just need to ensure that both 
order numbers are included in the subject title.
</blockquote>


<table border=0 width=100% class="topicback"><tr><td align= align=left>&nbsp;<?php if($l==1){?>Проблемы с закачиванием программ<?php }else{?>Problems with downloading<?php }?></td></tr></table>


<p><a name=dprob><font color="#800000"><u>How to download a file?</u></font></a> 
<blockquote> 
<p>Downloading is the process of copying a program file from one computer to another across the internet. 
When you download a program from our web site, it means you are copying it from the author or publisher's 
web server to your own computer. This allows you to install and use the program on your own machine.
To download file just click on download link at <a href="../commons/download.php">download page</a> and let the 
Windows do the job for you.
</blockquote>


<p><a name=dprob><font color="#800000"><u>I have problems downloading a file.</u></font></a> 
<blockquote> 
<p>To recover broken downloads and avoid download errors, as well as to prevent various errors related 
to the use of different downloading applications, please use any download manager, for example <a href="http://www.reget.com/">this one</a>.</p>
</blockquote>


<p><a name=corrupt><font color="#800000"><u>I have downloaded a file, but when I run the setup file, 
I get the "File is corrupted" message.</u></font></a> 
 <blockquote> 
<p>Please try to download the file one more time (delete the previously downloaded corrupted file first).</p>
<p>We recommend that you use <a href="http://www.reget.com/">ReGet</a>, simple and fast download manager. 
It helps you recover broken downloads and avoid download errors. If nothing helps, please <a href="../commons/guestbook.php">contact us</a>.</p>
</blockquote>


<p><a name=respond><font color="#800000"><u>Your server does not respond.</u></font></a> 
 <blockquote> 
<p>Please check your Internet connection. Try to download a file from any other server 
and see if it works. If the connection is OK, try downloading the file from our server in 10 or 15 minutes, 
as temporary download problems may happen from time to time.</p>
</blockquote>
 
 
<p><a name=demo><font color="#800000"><u>I tried downloading the full version using a 
special link you sent me, but was unable to do it. All that I get is the demo version and can't put in my registration code.</u></font></a> 
<blockquote>
<p>Make sure that you use the link correctly. Do not type the link manually into your browser 
window. Use "Copy" and "Paste" commands. Important! - Our links are case-sensitive.</p>
</blockquote>      


<p><a name=demo><font color="#800000"><u>The program cannot be downloaded without 
getting a notice from Microsoft that you are an UNKNOWN PUBLISHER and that there may 
be risk associated with your download. </u></font></a> 
<blockquote>
<p>Windows XP/SP2 has features to warn you about downloading or opening files which might 
have been altered. When you are downloading WiredPlans.com products from the WiredPlans.com website 
no such risk arises, since no other party has had any opportunity to tamper with them. You may use them safely.</p>
</blockquote>       

<table border=0 width=100% class="topicback"><tr><td align= align=left>&nbsp;<?php if($l==1){?>Апгрейды<?php }else{?>Upgrades<?php }?></td></tr></table>
              
<p><a name=what><font color="#800000"><u>What is an upgrade?</u></font></a> 

<blockquote> 

<p>Upgrades are new versions of the program. For the majority of our programs, you can get an 
upgrade for free during a specified time period, if you are a registered user. 
Click <a href="../commons/order.php">here</a> to see the upgrade terms for every software title.

<p>Please save the file in a safe place before your free upgrade term expires. If your free 
upgrade term has expired, we cannot supply you with the newest version.
       
</blockquote>

 
<p><a name=12month><font color="#800000"><u>What does "12 months upgrade" mean?</u></font></a> 
 <blockquote> 

<p>"12 months upgrade" means, you will get new versions of the program and technical support for 
free during 12 months after you purchased the software. This term has nothing to do with using 
the software you bought - you can use it as long as you want.

<p>Registered users receive notifications about every new release, during the entire length of the 
free upgrade period. We don't send these notifications immediately after we release a new version. 
We usually do it a few weeks after the release, so our users won't have to deal with possible software problems. 

<p>If your e-mail address has changed and you had not informed us about it, you won't get any notifications 
about new releases. Use our <a href="../commons/guestbook.php">online form</a> to provide us with your new e-mail address.

<p>Please save the file in a safe place before your free upgrade term expires. If your free upgrade 
term has expired, we cannot supply you with the newest version.

</blockquote>


<p><a name=getnew><font color="#800000"><u>I am a registered user. How do I get the new version?</u></font></a> 
 <blockquote> 

<p>If your free upgrade term has not expired, you will receive notifications about every new release and instructions 
on how to get it. We don't send these notifications immediately after we release a new version. We usually do it 
a few weeks after the release, so our users won't have to deal with possible software problems. If you didn't get 
a notification within a reasonable period of time after a new version is released, this probably means that your e-mail 
address in our database is outdated. Use our <a href="../commons/guestbook.php">online form</a> to provide us with your new e-mail address.

<p>If your free upgrade term has expired but you want to get the latest version, you can buy the new version of 
the program at a discount. For the majority of our programs, we offer a 25% discount for the previously registered users. 
For more information how to get the discount, contact us via <a href="../commons/guestbook.php">online form</a>.

</blockquote>


<p><a name=expired><font color="#800000"><u>I am a registered user, but my free upgrade term has expired. 
Should I buy the software again to get the newest version?</u></font></a> 

<blockquote>

<p>If your free upgrade term has expired but you want to get the latest version, you can buy the new version of 
the program at a discount. For the majority of our programs, we offer a 25% discount for the previously registered users.
 For more information how to get the discount, contact us via <a href="../commons/guestbook.php">online form</a>.
</blockquote>

<table border=0 width=100% class="topicback"><tr><td align= align=left>&nbsp;<?php if($l==1){?>Скидки<?php }else{?>Discounts<?php }?></td></tr></table>
<p><a name=any><font color="#800000"><u>Do you have any discounts or special offers for any of your programs?</u></font></a> 
<blockquote> 
<p>Yes, we offer discounts for Educational institutions and non-profit organizations</a>.
</blockquote>


<p><a name=multip><font color="#800000"><u>Can I get a discount if I buy several licenses of the same program?</u></font></a> 
<blockquote> 
<p>Yes, you can.
<p>Multiple license discounts are available at request.
<p>If you are interested in any software title, please <a href="../commons/guestbook.php">contact us</a>.
</blockquote>


<p><a name=user><font color="#800000"><u>I bought one of your programs. Can I buy any other of your company's programs at a reduced price?</u></font></a> 
<blockquote> 
<p>In this case, we can give you a discount for some of our programs. <a href="../commons/guestbook.php">Contact us</a> for details.
</blockquote>

<p><a name=freer><font color="#800000"><u>Is there any way to get registration without money paid?</u></font></a> 
<blockquote> 
<p>You can get registration for free by contributing something useful for some of our programs, for example new macro or plugin for WireKeys or widget for WireChanger.<a href="../commons/guestbook.php">Contact us</a> for details.
</blockquote>

<p><a name=newsletter><font color="#800000"><u>How can i get information about new releases?</u></font></a> 
<blockquote> 
<p>Subscribe to WiredPlane.com FREE newsletter to become eligible for latest news, exclusive special offers, 
software giveaways, contests and much more! Our plain text newsletter, which is managed by Yahoo!Groups, 
is sent about once a month and also each time new software versions are released.<br>
<b>To subscribe</b>, type your email address in the box below and click Go.
<script language="JavaScript">
function CheckNewsletter(ThisForm)
{
	var sEmail=ThisForm.email.value;
	if (sEmail.indexOf("@")==-1){
		alert("Please enter your email first.");
		ThisForm.email.focus();
		return false;
	}
	return true;
}
</script>
<table border=0 width=1% cellspacing=0 cellpadding=0>
<form method="get" action="http://groups.yahoo.com/subscribe/wiredplane" method="post" onSubmit="return CheckNewsletter(this);">
<tr><td><input type=text name="email" size=14 value="Your email"><td>&nbsp;<input type="image" name="Go" src="/images2/gosearch.gif" alt="Go"></td></tr>
</form>
</table>
<br>
You will automatically receive an 
email message asking you to confirm your request to join the WiredPlane.com' newsletter. Reply to 
this message by pressing your email program's "Reply" button and then "Send".
If you are concerned about your email address disclosure, please read our <a href="../commons/privacy.php">Privacy Policy</a>.
</blockquote>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
