<?php
include_once("wpg_engine.php");
function dropHeader($incll)
{
	global $pageTitle;
?>
<html><head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
<title><?php print("$pageTitle");?></title>
<link rel="Stylesheet" href="<?php print($incll);?>data/screen.css" type="text/css" media="screen"></head><body>
	<div id="header">
	<table width=100%>
	<tr style="text-align:center;">
	<td width=140px height=30 style="background-image: url(<?php print($incll);?>data/tab_normal.png); background-repeat:stretch;"><a href="/aboutus.php" style="color: white; font=weight: bold;">� ��������</a>
	<td width=140px height=30 style="background-image: url(<?php print($incll);?>data/tab_normal.png); background-repeat:stretch;"><a href="/index.php" style="color: white; font=weight: bold;">����</a>
	<td width=140px height=30 style="background-image: url(<?php print($incll);?>data/tab_normal.png); background-repeat:stretch;"><a href="/cgi-bin/wpg_engine.php?action=opensupport" style="color: white; font=weight: bold;">���������</a>
	<td width=140px height=30 style="background-image: url(<?php print($incll);?>data/tab_normal.png); background-repeat:stretch;"><a href="/cgi-bin/wpg_engine.php?action=openforum" style="color: white; font=weight: bold;">�����</a>
	<td>
	</table>
	<img src="data/header.jpg" alt="������ ����������" height="138" width="596">
	</div>
<div id="container">
<?php
}
dropHeader($includeLevel);
?>