<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>{$title}</title>
{include file="header.tpl" title=WPLabs}
<div class="box" style="text-align: center; font-weight: bold;">{if $etype eq "success"}<span style="color: #090;">{$error}</span>{elseif $etype eq "partial"}<span style="color: #990;">{$error}</span>{else}<span style="color: #900;">{$error}</span>{/if}</div><br />
{include file="footer.tpl"}
