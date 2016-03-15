<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "���������� �������� � ������� ���. ������, ������ ������������� ������ �� �����������";
	}
	return "Automation tool for power users, Control your PC using hotkeys";
}
function insertSection($l){
?>

<?php if($l==1){?>
<p>���� �� ����������� ���������� � ����������� ����������������� ��� ��� �������� ����������, �� ����������� � ��������� WireKeys � ����������� ��������.
��� ����� �������� ����������� � ����� ���������� ��� ���� ����� �����������!

<p>�� ����� ���������� ��� ���� ��������������:

<p>1. WireKeys ����� �������� ����������� ��� ���������� �������� ��������� � ������������� ����������.
������������:
<ul>
  <li>���������� ����� ����� �������������� ������������ ����� ������������, ������� �� ������� ������� �����������
  <li>�� ����� ������������� ���� ����������� ����������
  <li>����� ������ ������� �������������� (����������� - � ������ ���������)
  <li>�� � ��������� � �� ������� ���������� ��������� ������ ��� ���������������� � ��������� ���������� ��� ���� �����
</ul>

<p>2. WireKeys ����� �������������� �����������. ������������:

<ul>
  <li>����������� ���������� ������ WireKeys, ������� ����� �������������� ��� �������������� ������������/��������������.
  <li>����������� ����� ����� ���������� ���� ����� ��� ��������������� WireKeys � ������������ - ������, ���������, ��������, �����-������ � ��.
</ul>
<br>
�� ����� ������ ������� ��� ����� ����� �����������. ���� �� ������ ������ ������ ���
������ �������������� �����������, �������� ���: <a href="mailto:custom@wiredplane.com">custom@wiredplane.com</a>
<?php }else{?>
<p>Develop a keyboard with extended functionality? Already selling
keyboards? Add significant value to your products - include WireKeys software.

<p>Generally there are two ways we work with hardware manufacturers:

<p>1. WireKeys is a primary software for handling keyboard shortcuts.
Advantages:
<ul>
  <li>Keyboard gets additional features you can describe to buyers
  <li>No need to develop own software
  <li>Flexible licensing policy
  <li>Ability to add custom functionality
</ul>

<p>2. WireKeys is a bonus software. Advantages:

<ul>
  <li>Special FREE version of WireKeys to include without written permission.
  <li>Publisher package to minimize your time expenses by including WireKeys with keyboards. 
  The package includes icons, screen shots,  descriptions, press releases and more.
</ul>
<br>
Additional information available at request. Contact us: <a href="mailto:custom@wiredplane.com">custom@wiredplane.com</a>
<?php }?>
<br><br>
<div id=h1>More info</div>
<lu>
<li><a href="../wirekeys/advantages.php">WireKeys advantages</a>
<li><a href="../wirekeys/custom.php">Custom development</a>
<li><a href="../wirekeys/oem.php">OEM package</a>
</lu>
<br><br>
<a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>
