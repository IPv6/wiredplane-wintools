<?php 
function getTitle($l)
{
	global $language;
	return "SMS for Odnoklassniki.ru";
}
function insertSection($l){
?>
<table border=0 width=90%>
<tr><td align=center>
<h2>�������� ��������� ����� ������� �� ����� Odnoklassniki.ru ����� ���</h2>
<hr>
������ ��������� ������� ��������� ������ ����� ��� ������� ��� ����� �������� �� �� ������?
��������� ��� ����� ���� Odnoklassniki.ru!

<p>������� ��� � ������ � ��� ��� ������ ����� (��� �������) �� ������ ���� ������� ��� ��/���, ����� �� Odnoklassniki.ru,
��� �� ������� ���� ���������! �������, ������ � ������!
<br>
<br><form method="get" action="../commons/register-wp.php" class=menu>
<input type="hidden" name="who" value="sms2odnoklassniki">
<input type="submit" value="������������������">
</form>
<hr>
</td></tr>
</table>
<div style="clear: both; text-align:center;">
	<br>
	<script type="text/javascript"><!--
	google_ad_client = "pub-5186671154037974";
	google_ad_width = 468;
	google_ad_height = 60;
	google_ad_format = "468x60_as";
	google_ad_type = "text_image";
	google_ad_channel = "";
	google_color_border = "000000";
	google_color_bg = "EEEEEE";
	google_color_link = "0000FF";
	google_color_text = "000000";
	google_color_url = "008000";
	//-->
	</script>
	<script type="text/javascript"
	  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
	</script>
</div>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
