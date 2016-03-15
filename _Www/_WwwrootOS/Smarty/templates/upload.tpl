<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>{$title}</title>
{include file="header.tpl" title=WPLabs}
		<div class="box" style="text-align: left;">
			<table cellpadding="3" cellspacing="0" border="0">
				<form enctype="multipart/form-data" action="{$smarty.server.PHP_SELF}?action=submit" method="post">
				<tr><td colspan="2"><h4 style="margin-top: 0;">Upload New Image</h4></td></tr>
				<tr><td style="text-align: right;">Your Name:</td><td><input type="text" size="30" name="upSubName" /></td></tr>
				<tr><td style="text-align: right;">Your Email:</td><td><input type="text" size="30" name="upSubEmail" /></td></tr>
				<tr><td style="text-align: right;">Image Name:</td><td><input type="text" size="30" name="upName" /></td></tr>
				<tr><td style="text-align: right; vertical-align: top;">Image Description:</td><td><textarea cols="50" rows="5" name="upDesc"></textarea></td></tr>
				<tr><td style="text-align: right;">File:</td><td><input type="hidden" name="MAX_FILE_SIZE" value="2000000" /><input name="userfile" type="file" /></td></tr>
				<tr><td style="text-align: right; vertical-align: top;" colspan=2><span class="snapsNotes">Valid images are jpg, gif and png. You can also upload zip archive with images. Max file size is 1.7Mb</span></td></tr>
				<tr><td style="text-align: right;"></td><td><input type="submit" name="submit" value="Submit image(s)" /></td></tr>
				</form>
			</table>
		</div><br />

{literal}
<script language=JavaScript>
function onSub()
{
        mywin=open('about:blank',"RES","width=700,height=400,scrollbars=yes,status=no,toolbar=no,menubar=no,titlebar=no,resizable=1");
        links=document.forms['URLUpload'].src.value;
        if(links.length==0){
            return false;
        }
{/literal}
		var filename='{$fname}';
{literal}
        arr=links.split("\n");
        mywin.document.open();
        mywin.document.write("<ht"+"ml>"+"<he"+"ad>");
        mywin.document.write("<link rel='stylesheet' type='text/css' href='snaps.css' />");
        mywin.document.write("</he"+"ad>"+"<bo"+"dy>");
        mywin.document.write("Retrieving images...<hr>");
        for(i=0;i<arr.length;i++){
                arr[i] = arr[i].replace(/\r/ig,"");
        fname=escape("uploads/extern/"+filename+"_"+i)+"_"+Math.ceil(Math.random()*1000)+".jpg";
                mywin.document.write("<div  class='box' align=center>Uploading "+arr[i]+":<br>");
                mywin.document.write("<img src='");
                mywin.document.write("http://outer-station.m6.net/wgallery/phpThumb.php?file=");
                mywin.document.write("../"+fname);
                mywin.document.write("&goto="+escape("http://outer-station.m6.net/")+fname);
                mywin.document.write("&src="+escape(arr[i]));
                mywin.document.write("' width=240></div>");
                mywin.document.write("<hr>");
        }
        mywin.document.write("Done!...<hr>");
        mywin.document.write("</bo"+"dy>"+"</ht"+"ml>");
        mywin.document.close();
        document.forms['URLUpload'].src.value="";
        return false;
}
</script>
{/literal}
		<div class="box" style="text-align: left;">
			<table cellpadding="3" cellspacing="0" border="0">
				<form name="URLUpload" action="http://outer-station.m6.net" method=get onSubmit="return onSub();">
				<tr><td colspan="2"><h4 style="margin-top: 0;">Upload New Images from URLs</h4></td></tr>
				<tr><td style="text-align: right;"></td><td>
				Image URLs (one per line):<br><textarea rows=10 cols=80 wrap=off name="src" /></textarea>
				</td></tr>
				<tr><td style="text-align: right;"></td><td><input type="submit" value="Submit image(s)" /></td></tr>
				</form>
			</table>
		</div><br />
{include file="footer.tpl"}
