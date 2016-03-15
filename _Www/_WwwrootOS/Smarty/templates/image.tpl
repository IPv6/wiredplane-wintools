<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>{$title}</title>
{include file="header.tpl" title=WPLabs}
<div class="snapsCrumb">{$data[1][7]}
{if $admin eq 1}
<a href="admin/index.php?s=images&a=edit&image={$data[1][8]}"><img src="icons/editimage.png" alt="Edit" title="Edit" /></a>|
<a href="admin/index.php?s=images&a=delete&image={$data[1][8]}"><img src="icons/deleteimage.png" alt="Delete" title="Delete" /></a>|
<a href="admin/index.php?s=images&a=move&image={$data[1][8]}"><img src="icons/moveimage.png" alt="Move" title="Move" /></a>
{/if}
</div>
		<table cellpadding="0" cellspacing="10" border="0" style="width: 100%;">
			<tr>
{* The data returned by the image() is an array that contains (in this order):
image name [0], image filename [1], image description [2], image created timestamp [3], image modified timestamp [4], album ID [5], and image [6]
$data[1][0-6] references each of the pieces of information listed above. 
The array also contains 2 special elements -  $data[1][7] which contains the breadcrumb navigation for the top of the album page, and
$data[1][8] contains the comments and comment form. *}
				<td class="snapsTable">
				<b>To save this picture, right-click on the image and select 'Save Picture As...'</b>. 
				<br><span class="snapsNotes">Created: {$data[1][3]|date_format:"%m-%d-%Y"}&nbsp;|&nbsp;<!--Last Modified: {$data[1][4]|date_format:"%m-%d-%Y"}-->Picture will be downloaded in its original resolution</span><br>
				<img width=640 src="{$config.albumsPath}{$data[1][5]}/{$data[1][1]}">
				<!--Send to mobile&nbsp;|&nbsp;Order print&nbsp;|&nbsp;Order poster, gift or souvenir with this image<br>-->
				<table width=90%>
				<form action="wgallery/phpThumb.php" method=get>
				<input type=hidden name="src" value="../{$config.albumsPath}{$data[1][5]}/{$data[1][1]}">
				<input type=hidden name="f" value="jpeg">
				<input type=hidden name="down" value="ostation_{$data[1][1]}">
				<input type=hidden name="fltr[]" value="wmt|Outer-station wallpapers|5|TL|FFFFFF||50|1|0">
				<tr valign=center>
				<td align=center colspan=2><a href="http://www.wiredplane.com/wirechanger/info.shtml">WireChanger</a> users can right-click on the image and select 'Set as background'<br>to automatically add this picture into image gallery
				<tr valign=center>
				<td align=center colspan=2>
				
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
google_ui_features = "rc:10";
//-->
</script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
				
				<tr valign=center>
				<td align=left width=1%>
				<!--a href="#" onClick="javascript:window.open('http://mpush.txtnation.com/req.php?account=wiredplane&image=http://outer-station.m6.net/{$config.albumsPath}{$data[1][5]}/{$data[1][1]}&caption=Bought+to+you+by+Outer-station.m6.net+Enjoy','_MPUSH','width=640,height=420,titlebar=1,resizable=1,scrollbars=0');return(false);">Send to mobile</a-->
				<td align=center>
				Resolution: <select name="w"><option value="800">800*600</option><option value="1024">1024*768</option><option value="1280">1280*1024</option></select>
				<select name="fltr[]"><option value="">Without any effect</option><option value="ds|100">Make Black and white</option><option value="blur|3">Add blur</option></select>
				<!--<input type=checkbox name="fltr[]" value="wmi|wmark/cal_{$date_year}_{$date_month}|BR|50|1">Add calendar-->
				<input type=submit value="Download!">
				</table>
				</form>
				<hr>All pictures (wallpapers) displayed here are copyrighted by their respective owners. Commercial use of these pictures is strictly forbidden without the prior agreement of the author. Pictures can be removed at the request of the author.
				</td>
			</tr>
		</table>
		<div class="snapsCrumb">
			{$data[1][2]}
			<h3>Comments</h3>
{if $config.allowComment eq 1}
{if is_array($comment)}
			<table cellpadding="0" cellspacing="10" border="0" style="width: 100%;">
{assign var="count" value="1"}
{section name=c loop=$comment}
	{assign var="count" value="`$count+1`"}
{/section}
{section name=el loop=$count  start=1}
				<tr>
					<td class="snapsTable" style="text-align: left;"><span class="snapsNotes">By: <a href="mailto:{$comment[el][1]}">{$comment[el][0]}</a><br />Posted: {$comment[el][3]|date_format:"%m-%d-%Y @ %I:%M:%S %p"}</span><br /><br />{$comment[el][2]}</td>
				</tr>
{/section}
			</table>
{else}
			{$comment}
{/if}
			<form name="comment" action="{$smarty.server.PHP_SELF}?album={$data[1][5]}&amp;image={$data[1][8]}" method="post">
			<h3>Post a comment</h3>
			<div style="width: 500px; height: 250px;">
				<div style="clear: both; height: 24px;"><span style="width: 100px; float: left; text-align: right;">Name:</span><span style="width: 390px; float: right; text-align: left;"><input type="text" name="commentName" id="commentName" size="30" /></span></div>
				<div style="clear: both; height: 24px;"><span style="width: 100px; float: left; text-align: right;">*Email:</span><span style="width: 390px; float: right; text-align: left;"><input type="text" name="commentEmail" id="commentEmail" size="30" /></span></div>
				<div style="clear: both;"><span style="width: 100px; float: left; text-align: right;">Comment:</span><span style="width: 390px; float: right; text-align: left;"><textarea cols="60" rows="10" name="commentBody" id="commentBody"></textarea></span></div>
				<div style="clear: both; padding-top: 10px; height: 30px;"><span style="width: 100px; float: left; text-align: right;">&nbsp;</span><span style="width: 390px; float: right;"><input type="reset" name="reset" value="Clear" /> <input type="submit" name="submit" value="Post Comment" /></span></div>
				<div style="clear: both; padding-top: 10px; height: 30px; font-size: 9px;">*Your email will be obfuscated to protect against spam harvesters.</div>
			</div>
			</form>
{else}
			{$comment}
{/if}
		</div><br />
{include file="footer.tpl"}
