<? $site=''.$_REQUEST["site"];?>
<? if(!empty($site)){include ("$site");} ?>
<hr color="#008000">
<p align="center">
<font size="1" face="Arial" color="#008000">Type in the URL of the page below</font></p>
<form method="POST" action="pump.php?">
	<p align="center">
	<input type="text" name="site" size="44" value="">  <input type="submit" value="Submit" name="submit"></p>
	<p align="center">
	<b><font color="#008000" size="2" face="Arial">Currently Showing: <? print"$site";?></font></b></p>
</form>


