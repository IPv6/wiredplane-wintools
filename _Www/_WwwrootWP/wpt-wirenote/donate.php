<?php 
function getTitle($l)
{
	global $language;
	return "WireNote - Lightweight PIM with Sticky notes, Reminders, LAN messenger and anti-spam filter";
}
function insertSection($l){
?>
<?php if($l==1){?>
<div id=h1>WireNote �������� �������! <a href="http://smskopilka.ru/?info&id=9527" title="�� ������ ������� - � ������ ����! ��������� � ����� ����" target="_blank">��������</a> ��� � �������� ���������!</div>
<br>
��������� ��� ��������� ��� �������� ������� � �������� ��������� �� ����, �� ��������� ������ �� ������� ��� �������� �� ������ ��, ��� ����� ������� � ����������� ����.
������� ������ �� ���������� ���, �� �������� �� ������. �� ������� ��� ����� ����� ������� ����� ���� �������� - ��������� ��������� ����� ���� ��������� �������, �� ��� ����� �������� �� ������� ����� ������� �������!
�� ����� ������ ��������������� ��������� ����� ������� � ����������� - ������ ������� ������������ WireNote ��� ������� ������� �������� �������� "������" ����� ������� - ��������!
<br><br>�������� � ��� � ���������� �������� � ��������������� ���� ���������, <a href="http://smskopilka.ru/?info&id=9527" title="�� ������ ������� - � ������ ����! ��������� � ����� ����" target="_blank">������ ������ ���</a>. 
��� ����� ������� �� ������ � �������� � �������� ���������� �� ������.
��� ��� ��� ��� ����� ������ ����� �� $0.3 �� $3 (�� �����) - � ����� ������ ��� ������ �������������� ����� 
��� ������ ����, ��� ����������� ������ ����� � �������� (����� � ��������) �������� �� �� �������!
���� �� ������ ������ ������ �������� ������, �������� �� <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">���� ������</a>.
<br><br>
<a href="http://smskopilka.ru/?info&id=9527" title="�� ������ ������� - � ������ ����! ��������� � ����� ����" target="_blank"><img src="http://smskopilka.ru/iclient/9527/smskopilka.gif" border="0" alt="sms.������� �� ������ ������� - � ������ ����!"></a>
<br><br>
<?php }else{?>
<div id=h1>WireNote helps the trees! <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">Help</a> us too!</div>
<br>Using this program for storing of records and sending of messages over network, 
you economize paper on which it would be necessary to write otherwise.
Economizing paper you you preserve the forest, from which it makes. 
Do not think that this contribution is small in order to be noticeable - using a program only several months, 
you thus save from the cutting down new young sapling!
You also can recommend program to your friends and colleagues - each person treating WireNote 
for conducting the notes helps the true "lungs" of our nature - trees!
<br><br>
If you like WireNote and wish us to continue development of this product, please help us and 
consider to <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">donate</a> our team with a small amout of money. Donations can be done here: <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">Visa, PayPal, other options available</a>
<br>Your aid will actually help nature (Indirectly, but your aid IS real!)
<br><br>
<?php }?>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
