<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>{$title}</title>
{include file="header.tpl" title=WPLabs}
<div class="snapsCrumb">Recent additions
&nbsp;&nbsp;
<script type="text/javascript"><!--
google_ad_client = "pub-5186671154037974";
google_ad_width = 468;
google_ad_height = 15;
google_ad_format = "468x15_0ads_al_s";
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
<table cellpadding="0" cellspacing="10" border="0" style="width: 100%;">
		{if is_array($dataLast)}
	<tr>
{* This is a loop that gives us 3 columns of images *}
{* The data returned by the album() is an array that contains (in this order):
image link [0], image Name [1], image [2], image description [3], image created timestamp [4], image modified timestamp [5], image view count [6], and number of comments [7]
for each image -> $dataLast[element] references the image, and $dataLast[element][0-7] references each of the pieces of information listed above. 
The array also contains 2 special elements - $dataLast[1][8] contains the links for paginating the records, if there are more than the values configured
(9 per page by default), and $dataLast[1][9] which contains the breadcrumb navigation for the top of the album page. *}
{assign var="col" value="0"}
{assign var="count" value="1"}
{section name=c loop=$dataLast}
	{assign var="count" value="`$count+1`"}
{/section}
{section name=element loop=$count start=1}
{if $col == 5}
			</tr>
			<tr>
{assign var="col" value="0"}
{/if}
				<td class="snapsTable"><a href="{$smarty.server.PHP_SELF}{$dataLast[element][0]}">{$dataLast[element][2]}<br>Show image</a><br /><br /><div class="snapsNotes"><span style="font-style: oblique;">Viewed {$dataLast[element][6]} times<br /></span>{$dataLast[element][4]|date_format:"%m-%d-%Y"}<!--br />Last modified: {$dataLast[element][5]|date_format:"%m-%d-%Y"}--></div></td>
{assign var="col" value="`$col+1`"}
{/section}
{assign var="remainder" value="`3-$col`"}
{section name=emptyElement loop=$remainder}
				<td class="snapsTable">&nbsp;</td>
{/section}
			</tr>
{else}
			<tr><td class="snapsTable">{$dataLast}</td></tr>
{/if}
		</table>

<div class="snapsCrumb">Wallpaper Categories</div>
<table cellpadding="0" cellspacing="10" border="0" style="width: 100%;">
		{if is_array($data)}
	<tr>
{* This is a loop that gives us 3 columns of albums *}
{* The data returned by the albumList() is an array that contains (in this order):
album ID [0], album Name [1], album Description [2], album image count [3], and album last modified timestamp [4]
for each album -> $data[element] references the album, and $data[element][0-4] references each of the
pieces of information listed above. *}
{assign var="col" value="0"}
{assign var="count" value="1"}
{section name=c loop=$data}
	{assign var="count" value="`$count+1`"}
{/section}
{section name=element loop=$count start=1}
{if $col == 3}
			</tr>
			<tr>
{assign var="col" value="0"}
{/if}
				<td class="snapsTable"><a href="{$smarty.server.PHP_SELF}?album={$data[element][0]}">{$data[element][6]}</a><br />
				<a href="{$smarty.server.PHP_SELF}?album={$data[element][0]}">{$data[element][1]}</a><br />{$data[element][2]}<br /><div class="snapsNotes">({$data[element][3]} items in this album)<br />Last modified: {$data[element][4]|date_format:"%m-%d-%Y"}</div></td>
{assign var="col" value="`$col+1`"}
{/section}
{assign var="remainder" value="`3-$col`"}
{section name=emptyElement loop=$remainder}
				<td class="snapsTable">&nbsp;</td>
{/section}
			</tr>
{else}
			<tr><td class="snapsTable">{$data}</td></tr>
{/if}
		</table>
<!--div class="snapsCrumb" style="text-align: right;">{$data[1][5]}</div-->
<div>
<script type="text/javascript"><!--
google_ad_client = "pub-5186671154037974";
google_ad_width = 728;
google_ad_height = 90;
google_ad_format = "728x90_as";
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
</div>

{include file="advert.tpl"}
{include file="footer.tpl"}
