<?php 
/*
����� �������� ��� ������������� �������������� ������ ExeCryptor � ������ ������ � ��� ������������� SWReg � RegNow

������:
- ��� ��� ��������� �������� ���������� ������ �� ���. � SWReg� ���� ����� "�������� ��������� ��� ������ ��������� ��������", ��� RegNow ���������� ������������ ��������������
- ��������������� ��� ������������� ����������� � ������ ������� ����� �������� ����������. 
- � �������� ���������������� ����� ������������ EMAIL �������, ��� �������� ��������������� ������ ��� ����� ���������. ���� �� ������ ������������ ������ ���. �����, ��������� ���������� ������ � �����. ������.

��������� �������������� �� http://www.strongbit.com/
- ������������� ������ ���� �������� ���:Registration Name 1=%regname% Required fields=%regname% Reply template=%!SERNUM%
- �� ��������� ����� (�����, �� ������ ������) ����������� � ���� � ��������� �� web ��������, ���������� ��� ��� �������� �� ������ ������� ��� ��������� ���� �� ������� ���������� �� web ������������
- ����������� ���� ��������� �� ������� ERROR �� ������� ������� ���������, ������� ����� ����� ������������� � ������ ������ ��������� ������ ��� ������. ��� ����� ����� � ���������� �������� ������� "��� 200" ��� ������ � � ���� ����� �������� ERROR

��������� �������������
- ��� ������ � ������ ������������ �� ���, �� ����� ������������� ����. �� ������������� ����, ����� ���������� ����� ���������� �������������� ��. �������������� �����
- � ������ ���� � ������� ������������� � ���������� �������� �������� (�� ������� �������������� ������ �������� � ������� �� edit email). ���� � ������� ����� ��������� ���: http://www.yoursite.com/keygen_app/make_swreg.php
- � ������ ���� � �������������� ����� ���������� ����� XML ���� ��������� ��������: XML Files->��������� �������� (�� ������ ����� ����� ���������)->������ <delivery>...</delivery> ��������� ��
<delivery>
 <deliver-license>
  <generate-license-code>
   <script>
   <define-parameter name="license_name"/>
   <return>
   <remote.connect method="post" url="http://www.yoursite.com/keygen_app/make_regnow.php"> 
    <remote.php-value name="what"><static value="wkcode"/></remote.php-value>
    <remote.php-value name="date"><date format="mm/dd/yyyy"/></remote.php-value>
    <remote.php-value name="orderid"><value-of class="orderitem" field="id"/></remote.php-value>
    <remote.php-value name="item"><value-of class="orderitem" field="product"/></remote.php-value>
    <remote.php-value name="quantity"><value-of class="orderitem" field="quantity"/></remote.php-value>
    <remote.php-value name="fname"><value-of class="recipient" field="fname"/></remote.php-value>
    <remote.php-value name="lname"><value-of class="recipient" field="lname"/></remote.php-value>
    <remote.php-value name="company"><value-of class="recipient" field="company"/></remote.php-value>
    <remote.php-value name="add1"><value-of class="recipient" field="add1"/></remote.php-value>
    <remote.php-value name="add2"><value-of class="recipient" field="add2"/></remote.php-value>
    <remote.php-value name="city"><value-of class="recipient" field="city"/></remote.php-value>
    <remote.php-value name="state"><value-of class="recipient" field="state"/></remote.php-value>
    <remote.php-value name="zip"><value-of class="recipient" field="zip"/></remote.php-value>
    <remote.php-value name="country"><value-of class="recipient" field="country"/></remote.php-value>
    <remote.php-value name="phone"><value-of class="recipient" field="phone"/></remote.php-value>
    <remote.php-value name="email"><value-of class="recipient" field="email"/></remote.php-value>
    <remote.php-value name="total"><value-of class="orderitem" field="total"/></remote.php-value>
   </remote.connect>
   </return>
   </script>
  </generate-license-code> 
 </deliver-license>
</delivery>
������������� ������� � ��� ���
���� � ������� ����� ��������� ���:
http://www.yoursite.com/keygen_app/make_regnow.php

- ��� ������ ��������� ����� ���� � ����� ����� ��������� ���:
http://www.yoursite.com/keygen_app/make.php
������, ��� ��� ����� �������� ������� ������ (����������� � �������� �� http://www.strongbit.com/), � ����� ������������
*/

// ���������
// �������� ���������
$appName="WireChanger";
// ���� ���� ��������� ����� � ����������� � ��������������� ������ (�� ������ �� �����!)
$savekeysto="";
// ���� � ������� email� �������
$emailtempl="kg_emailtempl.txt";
// �������� �� ����� ������ ������� ��� ���� ������ (��� � ���������). ���� ����� - �� ��������
$sendcopyto="support@wiredplane.com";
// ����� ����� ����������� � replyto ������ ������� (����� ��� ������ ��� ���������� ���� ����� ��� � �������)
$replyto="support@wiredplane.com";
// ���� � �������. ������ � �������� ������� "regname" ���������� �������������� ��� �� ��������� �� ����
$keygenurl="http://www.strongbit.com/keygen/getkey.asp?_Login=Razinkov%20Ilya&_License=WireChanger.Main.lic";
// ����������� ������, ����������� � ���������������� ����� (�������������� ������ ����� ��������� � ����� ���������  ���. �����)
// ������� �� ������ ������, � ����������� ������� ����� �������� ������
$salt="WC5";
// ������, ������������ �������������� (���, ��� ��������������� � ���������� ��������). ��� ������ ��������� �� ��� ����� �������� ��������������
$kgpsw="imurkola";

?>