<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>{$title}</title>
{include file="header.tpl" title=WPLabs}
{if is_array($data)}
<div class="snapsCrumb">{$data[1][9]}</div>
{else}
<div class="snapsCrumb"><a href="{$smarty.server.PHP_SELF}">Album List</a></div>
{/if}
		<table cellpadding="0" cellspacing="10" border="0" style="width: 100%;">
		{if is_array($data)}
	<tr>
{* This is a loop that gives us 3 columns of images *}
{* The data returned by the album() is an array that contains (in this order):
image link [0], image Name [1], image [2], image description [3], image created timestamp [4], image modified timestamp [5], image view count [6], and number of comments [7]
for each image -> $data[element] references the image, and $data[element][0-7] references each of the pieces of information listed above. 
The array also contains 2 special elements - $data[1][8] contains the links for paginating the records, if there are more than the values configured
(9 per page by default), and $data[1][9] which contains the breadcrumb navigation for the top of the album page. *}
{assign var="col" value="0"}
{assign var="count" value="1"}
{section name=c loop=$data}
	{assign var="count" value="`$count+1`"}
{/section}
{section name=element loop=$count start=1}
{if $col == 5}
			</tr>
			<tr>
{assign var="col" value="0"}
{/if}
				<td class="snapsTable"><a href="{$smarty.server.PHP_SELF}{$data[element][0]}">{$data[element][2]}<br>Show image</a><br /><br /><div class="snapsNotes"><span style="font-style: oblique;">Viewed {$data[element][6]} times<br /></span>{$data[element][4]|date_format:"%m-%d-%Y"}<br /></div></td>
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
		
{if is_array($data)}
		<div class="snapsCrumb" style="text-align: right;">{$data[1][8]}</div><br />
{/if}

{include file="advert.tpl"}
{include file="footer.tpl"}