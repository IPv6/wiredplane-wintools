<?php 
	global $languageNumber;
	$l=$languageNumber;
?>
<table width=122 cellspacing="0" cellpadding="3" border="0">
<tr><td colspan=2 align=center>

<?php if($l==1){
?>

<script type="text/javascript">
google_ad_client = "pub-5186671154037974";
google_alternate_ad_url = "http://outer-station.m6.net/";
google_alternate_color = "CCCCCC";
google_ad_width = 120;
google_ad_height = 240;
google_ad_format = "120x240_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "F5F5F5";
google_color_bg = "FFFFFF";
google_color_link = "3399ff";
google_color_url = "3399ff";
google_color_text = "3399ff";
</script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
></script>

<?php
 }else{
 ?>

<script type="text/javascript">
google_ad_client = "pub-5186671154037974";
google_alternate_ad_url = "http://outer-station.m6.net/";
google_alternate_color = "CCCCCC";
google_ad_width = 120;
google_ad_height = 240;
google_ad_format = "120x240_as";
google_ad_type = "text_image";
google_ad_channel ="";
google_color_border = "F5F5F5";
google_color_bg = "FFFFFF";
google_color_link = "3399ff";
google_color_url = "3399ff";
google_color_text = "3399ff";
</script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
></script>

 
<?php
}
?>
<img src='/images2/vmenu_line.gif' width=120 height=7>
</td></tr>
<form method=GET action='http://www.google.com/custom'>
	<input type=hidden name=sitesearch value='http://www.wiredplane.com'>
	<input type=hidden name=domains value='http://www.wiredplane.com'>
	<input type=hidden name=client value='pub-5186671154037974'>
	<input type=hidden name=forid value='1'>
	<?php if($l==1){?>
	<input type=hidden name=ie value='Windows-1251'>
	<input type=hidden name=oe value='Windows-1251'>
	<?php }else{?>
	<input type=hidden name=ie value='ISO-8859-1'>
	<input type=hidden name=oe value='ISO-8859-1'>
	<?php }?>
	<input type=hidden name=hl value='en'>
	<input type=hidden name=cof value='GALT:#0066CC;GL:1;DIV:#FF9900;VLC:336633;AH:center;BGC:FFFFFF;LBGC:999999;ALC:000000;LC:000000;T:0066CC;GFNT:666666;GIMP:666666;FORID:1;'>
<tr style="margin: 3px; padding: 3px;">
	<td height=1% valign=middle align=right><input type=text name='q' size=10 value="" style="width: 100%;"><td height=1%  valign=middle><input type="image" name="Go" src="/images2/gosearch.gif" alt="Go"></td>
</tr>
</form>
<tr><td colspan=2 align=center>
<img src='/images2/vmenu_line.gif' width=120 height=7>
<?php
include "../guestbook/rss_feed_ml.php"
?>
</td></tr>
</table>
