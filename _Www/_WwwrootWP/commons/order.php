<?php 
function getTitle($l)
{
	if($l==1){
		return "��� ����������";
	}
	return "How to Order";
}
function insertSection($l){
?>
	<?php if($l==1){?>
		����� �� ������� ���������� � ��� ��� ����� ���������� ���� ��������. ������������������ ������������� ��������� 
		�������� ��� ���������� ���������� ������������������� ��������.<br>��� ������ WiredPlane.com ���������� ����� 
		������������ � ������ ��������� �������� SWReg � RegNow � �������� ����������� ������� �����������. ����� 
		��������� ���� �������, ��� ����� ������ ��������������� ���� ��� ��������� �������������� ���� ��������.
	<?php }else{?>
		Trial versions of our products will not work longer than 14 days. If you like the program, please register your copy. 
		Registered users can install all future minor updates for free. Once you complete your registration, within 48 hours you 
		will receive an email with your serial number and registration instructions.
	<?php }?>
<br><br>

<table width="100%" cellspacing="0" cellpadding="1">

<tr align="center">
	<th class=inpage rowspan=2 width=15%><?php if($l==1){?>�������<?php }else{?>Product<?php }?></td>
	<th class=inpage rowspan=2><?php if($l==1){?>����<?php }else{?>Price<?php }?></td>
	</tr>
	<tr align="center">
	<th class=inpage width=20%><?php if($l==1){?>���, ������<?php }else{?>US and Canada<?php }?></td>
	<th class=inpage width=20%><?php if($l==1){?>������<?php }else{?>Europe<br><small>outside Russia</small><?php }?></td>
	<th class=inpage width=20%><?php if($l==1){?>������, ������<?php }else{?>Europe<br>Russia<?php }?></td>
</tr>

<tr align="center">
	<td colspan=6>
	<span id=h1><?php if($l==1){?>Online-�����������<?php }else{?>Order online<?php }?></span> (<font color=red><b>Instant key delivery!</b></font>)
	</td>
</tr>

<tr align="center">
	<td class=inpage_center>
		<a href="../wirekeys/about.php">WireKeys</a><br><?php insertVersion("wirekeys"); ?><br>Private User
	</td>
	<td class=inpage_center>$29.99</td>
	<td class=inpage_center>
		<a href="/guestbook/vendors.php?who=WireKeys&vendor=1&src=site"><?php if($l==1){?>����������<?php }else{?>Register<?php }?>!</a>
	</td>
	<td class=inpage_center>
		<a href="/guestbook/vendors.php?who=WireKeys&vendor=2&src=site"><?php if($l==1){?>����������<?php }else{?>Register<?php }?>!</a>
	</td>
	<td class=inpage_center>&nbsp;<a href="http://www.softkey.ru/catalog/basket.php?prodid=15658&quantity=1&clear=Y"><?php if($l==1){?>����������<?php }else{?>Register<?php }?>!</a></td>
</tr>
<tr>
<td class=inpage_center colspan=5><font color=red>NEW!</font> You can also get WireKeys FREE - <a target="_blank" href="http://www.trialpay.com/productpage/?c=1qlrh5">Click here!</a><br><font size="-2">Provided by our partners TrialPay</font></td>
</tr>

<tr align="center">
	<td class=inpage_center>
		<a href="../wirechanger/about.php">WireChanger</a><br><?php insertVersion("wirechanger"); ?>
	</td>
	<td class=inpage_center>$19.99</td>
	<td class=inpage_center>
		<a href="/guestbook/vendors.php?who=WireChanger&vendor=1&src=site"><?php if($l==1){?>����������<?php }else{?>Register<?php }?>!</a>
	</td>
	<td class=inpage_center>
		<a href="/guestbook/vendors.php?who=WireChanger&vendor=2&src=site"><?php if($l==1){?>����������<?php }else{?>Register<?php }?>!</a>
	</td>
	<td class=inpage_center>&nbsp;<a href="http://www.softkey.ru/catalog/basket.php?prodid=5729&quantity=1&clear=Y"><?php if($l==1){?>����������<?php }else{?>Register<?php }?>!</a></td>
</tr>

<tr align="center">
	<td class=inpage_center><a href="../wirekeys/about.php">WireKeys</a><br><?php insertVersion("wirekeys"); ?><br>Business user</td>
	<td class=inpage_center>&nbsp;$49.99&nbsp;</td>
	<td class=inpage_center colspan=3>
		<a href="https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-4"><?php if($l==1){?>����������<?php }else{?>Register<?php }?>!</a>
	</td>
</tr>
<!--
<tr align="center">
	<td colspan=6>
	<div id=h1><?php if($l==1){?>����������� �����������<?php }else{?>Special suggestions<?php }?></div>
	</td>
</tr>

<tr align="center">
	<td class=inpage_center>
		<a href="../wirechanger/about.php">WireChanger</a><br><?php insertVersion("wirechanger"); ?><br><b>on CD with Collection of images</b>
	</td>
	<td class=inpage_center>&nbsp;$35.00&nbsp;</td>
	<td class=inpage_center colspan=3>
		<a href="https://www.regnow.com/softsell/nph-softsell.cgi?item=10257-1&cdrom=yes">Register!</a>
	</td>
</tr>
-->
<tr align="center">
	<td colspan=6>
		<div id=h1><?php if($l==1){?>�������<?php }else{?>Utilities<?php }?></div>
	</td>
</tr>

<tr align="center">
	<td class=inpage_center>
		Commersial license for all freeware tools
	</td>
	<td class=inpage_center>&nbsp;$20.00&nbsp;</td>
	<td class=inpage_center colspan=3>
		<a href="/guestbook/vendors.php?who=comuse&vendor=2&src=site">Register!</a>
	</td>
</tr>

<tr align="center">
	<td class=inpage_center>Other utilities</td>
	<td class=inpage_center>&nbsp;Freeware&nbsp;</td>
	<td class=inpage_center colspan=3>
		<a href="/guestbook/vendors.php?who=donate&vendor=2&src=site">Donate!</a>
	</td>
</tr>

</table>

<!--table border=0 width=100%>
<tr>
	<td width=75%>
	<?php if($l==1){?>
		������������ �� ��� � ������ ������ ���������������� �� ������ RegNow.<br>
		������������ �� ������ ������ ���������������� �� ������ SWReg.<br>
		������������ �� ������ � ����� ������� ���� ������ ���������������� �� ������ WebMoney.<br>
	<?php }else{?>
		Users in the <b>US and Canada</b> should click the Register (RegNow) link.<br>
		Users in <b>Europe</b> or<b>PayPal</b> users should click the Register (SWReg) link.<br>
		Users in <b>Russia</b> should click the WebMoney link.<br>
	<?php }?>
	</td>
</table-->

<br>
<table width=99% border=0><tr>
<td valign=top width=50%>
	<?php if($l==1){?>
		�� ������ �������� ������� ��������� ������, ����� ��� �� ��������. ��� ����� ����� ����������� ����� ��������� ���������� ����������, 
		��������� ����� ��� �������� ���������������� ���������� ��� ���� �������� � ��������. 
		���� �� ������ �������� ������������ �������� �� ����� ������ ��� RuPay, e-gold, Yandex-������, 
		moneybookers � ��., �� ����� ��������� ���������� ����� ����� �� <a href="../commons/order-ru.php">����</a> ��������
	<?php }else{?>
		You can pay for product registration by credit card, money order, wire transfer, or check. You may also order by phone or fax. 
		Registering online is the fastest and easiest way to order our products. Your order will be processed in real-time and the registration email will be sent to you immediately. All data is encrypted to provide a secure transaction. 
		If you wish to pay via e-money from such systems like RuPay, e-gold, Yandex-money, moneybookers and others, visit <a href="../commons/order-ru.php">this page</a>
	<?php }?>
<td align=center width=50%><img border=0 src="/images2/payments/cc_visa.gif" height=38><img border=0 src="/images2/payments/cc_mastercard.gif" height=38>
<img border=0 src="/images2/payments/cc_discover.gif" height=38><br><img border=0 src="/images2/payments/cc_paypal.gif" height=28>
<br><span style="font-size: 10px;">Visa, Mastercard, American Express, Discover, Eurocard, JCB, Diners Club, PayPal, others. Instant key delivery.</span>
</tr>
</table>

<br>
<div id=h1><?php if($l==1){?>������ ��� ������� ���������� ��������<?php }else{?>Site license and volume discounts<?php }?></div>
<?php if($l==1){?>
	��� ����������� ����� 5� �������� ��� �������������� �������� �� ������������� ����� ��������� (site-license) ������������� ��������� ���� � ������.<a href="mailto:support@wiredplane.com">�������� ���</a> ����� ������ � ���
<?php }else{?>
	We offer a flexible system of discounts for purchasing more than 5 licenses at the same time, an unlimited site license, or for educational organizations. 
	Purchasing multi-user site licenses offers you a cost saving when you implement WiredPlane.com products throughout your organization, institution or business.
	Site license (One license for all staff in organization at a single site (single building or group of buildings within a 10 km range)) prices are discounted for volume purchases. We welcome your inquiries on the availability and arrangements of multi-user site licenses for our software products.
	Please <a href="mailto:support@wiredplane.com">contact us</a> to get more information about this discounts.
<?php }?>
<hr>

<div id=h1><?php if($l==1){?>��� ���� �����������<?php }else{?>Registration Benefits<?php }?></div>
<?php if($l==1){?>����� ����������� ����� ����� ����������� ��������� ������;&nbsp; ����� �������� �������������� ������ (��������� �� ����� �����) � ����������;&nbsp; ������������ ����������� ���������; &nbsp;
 ���� �������� ��������� ���������� ��������� �� 3 ����������, � �� �� ���� ��� ��� ������ ������;&nbsp; ��� ������� minor updates - ��������� ���������! ���������� � ��� ����������� � ����� ����������, �� ���������� ��������� ������ � �������� ��������
<?php }else{?>After registration trial limitations will be removed;&nbsp; Additional modules from this site will be available for free download;&nbsp;
 Prioritative technical support;&nbsp; Free minor upgrades;&nbsp; 
 Single license allow you to install application on up to 3 computers;&nbsp; By ordering registration key you are supporting futher development<?php }?>
<hr>

<div id=h1><?php if($l==1){?>���� ��������<?php }else{?>About our partners<?php }?>.</div>
<a href="http://www.regnow.com">RegNow</a>: <?php if($l==1){?>- ��������� ������, �������� ������� ���������� ����� �������� ��� AutoDesk, Symantec, McAfee, Novell, Metrowerks<?php }else{?>is a part of the Digital River, which is used by such companies as McAfee, Symantec, Novell, AutoDesk, Metrowerks.<?php }?>
<br>
<a href="http://www.swreg.org">SWReg</a>: <?php if($l==1){?>- ���� ��������� ������, ������������������ �� �������������� ����� � ������ � ���������� ��������. �������� SWReg ����� ���������� ������ ��������� ��������<?php }else{?>is a well-known European/UK registration service, which is used by many other well-known companies.<?php }?>
<br>
<?php if($l==1){?>RegNow � SWReg ���������������� �� ���������� ������ ��������� ����� ��������. �� (WiredPlane.com) �� �������� ������� ���������� � ����� ������������ ������ �� ���� ��������<?php }else{?>RegNow and SWReg allow products to be securely registered or purchased over the Internet. All RegNow and SWReg transactions are protected by an extremely high level of encryption -- the highest allowed by U.S. (RegNow) and UK (SWReg) law. WiredPlane.com does not receive any information about your credit card and personal data (if any) from RegNow and SWReg.<?php }?>

<br><br>
<?php if($l==1){?>
	��������: SWReg! � RegNow ���������� ������������� ������� �������� �  �� ������������� ����������� ���������. ��� ���. ��������� ����������� � ��� - <a href="mailto:support@wiredplane.com">support@wiredplane.com</a>
<?php }else{?>
	IMPORTANT: RegNow and SWReg only process registrations and do not supply technical support. For technical support please <a href="mailto:support@wiredplane.com">contact us</a>.
<?php }?>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
