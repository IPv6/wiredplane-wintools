<?php
$includeLevel="";
include_once("cgi-bin/page-header.php");
global $g_affiliateCatalog;
$filetodownload=_var("file");
?>
    <div>
		<p class="first">Загрузка файла</p>
		<table border=0 width=100%>
		<tr><td>
		<p>Ваша загрузка начнется через несколько секунд.<br>
		Если это не произойдет, нажмите <a href="<?php print($filetodownload); ?>">сюда</a>
		</table>
		<hr>
		<!--Пока идет загрузка, <a href="<?php print $g_affiliateCatalog."?dataonly=1";?>">посмотрите интересные другие игры!</a>-->
		<span>
		<script src="http://partner.gameboss.ru/ep/?elm_out_rows=3&elm_max_text_size=100&qu-type=1%2C2%2C3%2C4&elm_out_cols=1&qu-style=1%2C2&elm_text_align=left&type_out=1&affiliate_id=31605"></script>
		</span>
	</div>
	<script>
	function RunDownload()
	{
		document.location.href="<?php print($filetodownload);?>";
	}
	setTimeout("RunDownload()",5000);
	</script>
<?php
include("cgi-bin/page-footer.php")
?>