<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>{$title}</title>
{include file="header.tpl" title=WPLabs}
		<div class="snapsCrumb">
			<div style="width: 300px; margin: 0 auto; text-align: center;">
				<form action="{$smarty.server.PHP_SELF}?action=login" method="post">
				<div style="height: 24px;"><span style="width: 100px; float: left; text-align: right;">Username:</span><span style="width: 190px; float: right; text-align: left;"><input type="text" name="username" id="username" size="15" /></span></div>
				<div style="height: 24px;"><span style="width: 100px; float: left; text-align: right;">Password:</span><span style="width: 190px; float: right; text-align: left;"><input type="password" name="password" id="password" size="15" /></span></div>
				<div style="height: 24px;"><span style="width: 100px; float: left; text-align: right;"><input style="font-size: 13px;" type="reset" name="reset" value="Clear" /></span><span style="width: 190px; float: right; text-align: left;"><input style="font-size: 13px;" type="submit" name="submit" value="Log In" /></span></div>
				</form>
			</div>
		</div><br />
{literal}
<script type="text/javascript">
document.getElementById('username').focus();
</script>
{/literal}
{include file="footer.tpl"}
