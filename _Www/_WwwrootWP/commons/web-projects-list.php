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
Наши Web-проекты. Часть из них пока находится в закрытом бета-тестировании
<hr>
<br>&nbsp;-&nbsp;<a href="../commons/web-projects-sms2odnoklassniki.php" class=menu>Общение в Odnoklassniky.ru через SMS</a>
<br>&nbsp;-&nbsp;<a href="../commons/web-projects-icq-for-lineage.php" class=menu>Общение с Online-миром LineAge2 через ICQ</a>
</td></tr>
</table>
<hr>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
