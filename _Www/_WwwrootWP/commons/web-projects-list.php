<?php 
function getTitle($l)
{
	global $language;
	return "Web-projects";
}
function insertSection($l){
?>
<table border=0 width=90%>
<tr><td align=left>
���� Web-�������. ����� �� ��� ���� ��������� � �������� ����-������������
<hr>
<br>&nbsp;-&nbsp;<a href="../commons/web-projects-sms2odnoklassniki.php" class=menu>������� � Odnoklassniky.ru ����� SMS</a>
<br>&nbsp;-&nbsp;<a href="../commons/web-projects-icq-for-lineage.php" class=menu>������� � Online-����� LineAge2 ����� ICQ</a>
</td></tr>
</table>
<hr>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
