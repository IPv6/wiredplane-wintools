<?php 
function getTitle($l)
{
	if($l==1){
		return "���������� �� Webmoney/PayPal";
	}
	return "Order with WebMoney/PayPal";
}
function insertSection($l){
?>
	<?php if($l==1){?>
	���� � ������ ��� ������� ������� ����: WireChanger - <b>200 ������</b>, WireKeys - <b>200 ������</b>, ��������� - �� ������� :) ������� ��������� �������������� �������������!!! :) ��������� �� ��������� �� ������� ����� ����� �� ���� ��������, �������� ���������� ��� ��� �����.
	�� ���� ������ �������� ��������� � ������� ������ ��� �� ������ <a href="mailto:banking@wiredplane.com">banking@wiredplane.com</a>
	<?php }else{?>
	Prices for Ex-USSR users: WireChanger - 200 rub ($7), WireKeys - 200 rub ($7)<br>The rest - as much as you can afford. There are two payment options available, choose convinient way below.
	If you have any questions send an email to <a href="mailto:banking@wiredplane.com">banking@wiredplane.com</a>.
	<?php }?>

<br><br>
<div id=h1>RuPay</div>
<?php if($l==1){?>
	���� �� ������ �������� ����� <b>��������</b> ��� � �������������� ������ <b>PayPal, RuPay, e-gold, Yandex-������, moneybookers � ��.</b>,
	�������� ��������� ��� ������, ����� �� ���� �� ������.
	������� <a href="http://www.rupay.ru">RuPay</a> ������������� ����� 20 �������� ������ ��� � ������ � �� �������, ��� � � ������ �������.
	����� ��������� ���� ������� ���� ����� ������ ��� � ������� 2�� ����. ��������! �������� ��� �� ����������� ����� ����� ������ ����� (� ��������� ��� ������) ��� ��������� ��������� ������ ���. �����.
	<br><br>
	<table border=0 width=100%>
	<tr><td align=center>
	<form action="http://www.rupay.com/rupay/pay/index.php" name="pay" method="POST">
	<input type="hidden" name="pay_id" value="3884">
	<input type="hidden" name="sum_pol" value="7">
	<input type="hidden" name="name_service" value="��������� WireKeys">
	<input type="hidden" name="order_id" value="WK-ORDER-<?php insertDate(""); ?>">
	<!--input type="hidden" name="success_url" value="http://www.wiredplane.com/thankyou.shtml"-->
	<input type="submit" name="button" value=" WireKeys::�������� ($7) " style="font-family:Verdana, Arial, sans-serif; font-size : 11px;">
	</form>
	<td align=center>
	<form action="http://www.rupay.com/rupay/pay/index.php" name="pay" method="POST">
	<input type="hidden" name="pay_id" value="3884">
	<input type="hidden" name="sum_pol" value="7">
	<input type="hidden" name="name_service" value="��������� WireChanger">
	<input type="hidden" name="order_id" value="WC-ORDER-<?php insertDate(""); ?>">
	<!--input type="hidden" name="success_url" value="http://www.wiredplane.com/thankyou.shtml"-->
	<input type="submit" name="button" value=" WireChanger::�������� ($7) " style="font-family:Verdana, Arial, sans-serif; font-size : 11px;">
	</form>
	<td align=center><a href="http://www.rupay.ru"><img src="/images2/payments/rupay_8831_2-8.gif" width=88 height=31></a>
	<img src="/images2/payments/rupay_8831_2-sbr.gif" width=31 height=31>
	</tr>
	</table>
<?php }else{?>
	If you want to pay via <b>Sberbank</b> or use other electronic payments systems like <b>PayPal, RuPay, e-gold, Yandex-money, moneybookers and others</b>, choose registration link below. 
	Just click on button with the name of desired product to pay for it on-line. <a href="http://www.rupay.ru">RuPay</a> have more than 20 commonly-used payment options in Russia and Ukrain and other countries.
	After we receive your payment, your key will be sent to you in 2 business days. Attention! Please email to us when you finish the order with order number to get your registration code immediately.
	<br><br>
	<table border=0 width=100%>
	<tr><td align=center>
	<form action="http://www.rupay.com/rupay/pay/index.php" name="pay" method="POST">
	<input type="hidden" name="pay_id" value="3884">
	<input type="hidden" name="sum_pol" value="7">
	<input type="hidden" name="name_service" value="WireKeys License">
	<input type="hidden" name="order_id" value="WK-ORDER-<?php insertDate(""); ?>">
	<input type="submit" name="button" value=" WireKeys::Order now ($7) " style="font-family:Verdana, Arial, sans-serif; font-size : 11px;">
	</form>
	<td align=center>
	<form action="http://www.rupay.com/rupay/pay/index.php" name="pay" method="POST">
	<input type="hidden" name="pay_id" value="3884">
	<input type="hidden" name="sum_pol" value="7">
	<input type="hidden" name="name_service" value="WireChanger License">
	<input type="hidden" name="order_id" value="WC-ORDER-<?php insertDate(""); ?>">
	<input type="submit" name="button" value=" WireChanger::Order now ($7) " style="font-family:Verdana, Arial, sans-serif; font-size : 11px;">
	</form>
	<td align=center><a href="http://www.rupay.ru"><img src="/images2/payments/rupay_8831_2-8.gif" width=88 height=31></a>
	<img src="/images2/payments/rupay_8831_2-sbr.gif" width=31 height=31>
	</tr>
	</table>
<?php }?>
<br><br>
<div id=h1>WebMoney</div>
<?php if($l==1){?>
	������ ����� ����������� �������� �� ������� WebMoney (<a href="http://www.webmoney.ru">WebMoney.ru</a>). ����� �������� �� ������� ������� ���������� ����� (200���.): <b>R485517162176</b>.
	���� ���� ������� ������� � US Dollars, �� ����� ��������� <b>$7</b> �� ������� <b>Z998320527287</b>. ����� ��������� ���� ������� ���� ����� ������ ��� � ������� 2�� ���� (�� �������� ��������� � ���� �� ������ ���� ��������!)
	<br><br>
	<b>�����:</b> <b>����� ��������� �������</b> ����������� ��������� � ���� �� ����� � �������
	����� email �� ������� ���� ����� ������ ��������������� ���� � ��������������� ���.
<?php }else{?>
	You can directly order registration key by transfering registration fee to our WebMoney wallet (<a href="http://www.webmoney.ru">WebMoney.ru</a>).
	Our wallet number is <b>R485517162176</b> for roubles (200rub.). To pay in US Dollars, transfer $7 to wallet number <b>Z998320527287</b>.
	After we receive your payment, your key will be sent to you in 2 business days. 
	<br><br>
	<b>Important:</b> Please send your registration name to us by <a href="mailto:banking@wiredplane.com">email</a>
	<b>before</b> making payment. We will use this information to generate and send back to you your registration key. 
<?php }?>
<br><br>
<?php if($l==1){?>
	���� �� ����������� WM Keeper Classic, �� ������ ��������� ����� ������ 
	������ ������� �� ���� �� ���������� ��� ������. �� �������� ������� ��� email 
	� ������� ������ ������� �������� ����� �������!
<?php }else{?>
	If you are using WM Keeper Classic, you can by our products right now.
	Just click one of the following links. Don't forget to type your email
	address in the payment dialog box! 
<?php }?>
<table border=0 width=100%>
<tr align=center><td>&nbsp;<a href="wmk:PayTo?Purse=Z998320527287&Amount=7&Desc=I%20want%20to%20order%20WireKeys.%20My%20Email%20is%20email@domain.com&BringToFront=y"><?php if($l==1){?>��������� $7 ��<?php }else{?>Pay $7 for<?php }?> WireKeys</a>
<td>&nbsp;<a href="wmk:PayTo?Purse=R485517162176&Amount=200&Desc=I%20want%20to%20order%20WireKeys.%20My%20Email%20is%20email@domain.com&BringToFront=y"><?php if($l==1){?>��������� 200��� ��<?php }else{?>Pay 200rub for<?php }?> WireKeys</a></tr>
<tr align=center><td>&nbsp;<a href="wmk:PayTo?Purse=Z998320527287&Amount=7&Desc=I%20want%20to%20order%20WireChanger.%20My%20Email%20is%20email@domain.com&BringToFront=y"><?php if($l==1){?>��������� $7 ��<?php }else{?>Pay $7 for<?php }?> WireChanger</a>
<td>&nbsp;<a href="wmk:PayTo?Purse=R485517162176&Amount=200&Desc=I%20want%20to%20order%20WireChanger.%20My%20Email%20is%20email@domain.com&BringToFront=y"><?php if($l==1){?>��������� 200��� ��<?php }else{?>Pay 200rub for<?php }?> WireChanger</a></tr>
</table>
<br>
<?php if($l==1){?>
	�������� ��������: ���� WM Keeper Classic �� �������, �� �� ����� ������� �������������.
	<br>��� ������ �������� ������ � Internet Explorer ������ 5 � ����.
<?php }else{?>
	Note: If WM Keeper Classic is not running yet, it will be started automatically. 
	These links will work in Internet Explorer v5 and higher only.
<?php }?>

<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
