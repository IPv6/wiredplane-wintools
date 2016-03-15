<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/* Get error messages, if set */
if (!empty($_GET['err'])) {
	$err = $_GET['err'];
} else {
	$err = '';
}
?>
			<h3><img src="icons/config.png" alt="Settings" title="Settings" style="vertical-align: middle;" /> Settings</h3>
			<?php if (!empty($err)) { echo "\n".'<div class="box">'.$err.'</div><br />'."\n"; } ?>
			<div class="box">
				<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
<?php
/* If the form was submitted */
if (!empty($_POST['submit'])) {
	/* Set values from checkboxes. The form does not send a value if the checkbox is unchecked, so we must manually set the value for the query if unchecked */
	if (empty($_POST['allowComment'])) {
		$aC = 0;
	} else {
		$aC = $_POST['allowComment'];
	}
	if (empty($_POST['allowSubmit'])) {
		$aS = 0;
	} else {
		$aS = $_POST['allowSubmit'];
	}
	if (empty($_POST['enableCache'])) {
		$eC = 0;
	} else {
		$eC = $_POST['enableCache'];
	}
	if (!empty($_POST['imPath'])) {
		$imP = str_replace('\\', '/', $_POST['imPath']);
	} else {
		$imP = '';
	}
	/* Update the database and print messages */
	$sql = 'REPLACE INTO '.TP.'config (var, val) VALUES ("absPath", "'.$_POST['absPath'].'"), ("albumsPath", "'.$_POST['albumsPath'].'"), ("cachePath", "'.$_POST['cachePath'].'"), ("uploadsPath", "'.$_POST['uploadsPath'].'"), ("snapsURL", "'.$_POST['snapsURL'].'"), ("albumsPP", '.$_POST['albumsPP'].'), ("imagesPP", '.$_POST['imagesPP'].'), ("allowComment", '.$aC.'), ("allowSubmit", '.$aS.'), ("enableCache", '.$eC.'), ("resizeMethod", "'.$_POST['resizeMethod'].'"), ("imPath", "'.$imP.'")';
	$result =& $db->query($sql);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	if ($db->affectedRows() > 0) {
		$err = 'Settings Updated!';
		echo '<script type="text/javascript">document.location.href = "index.php?s=config&err='.$err.'";</script>';
	} else {
		$err = 'Update Failed!';
		echo '<script type="text/javascript">document.location.href = "index.php?s=config&err='.$err.'";</script>';
	}
}
?>
					<form action="index.php?s=config" method="post">
					<tr><td style="width: 30%; text-align: right;">Absolute Path:</td><td colspan="2" style="text-align: left;"><input type="text" name="absPath" size="30" value="<?php echo $config['absPath']; ?>" /></td></tr>
					<tr><td style="background: #CCC; width: 30%; text-align: right;">Albums Path:</td><td style="background: #CCC; text-align: left;"><input type="text" name="albumsPath" size="10" value="<?php echo $config['albumsPath']; ?>" /></td><td rowspan="3">Albums, Cache, and Uploads Paths are relative to the Absolute Path defined above.</td></tr>
					<tr><td style="width: 30%; text-align: right;">Cache Path:</td><td style="text-align: left;"><input type="text" name="cachePath" size="10" value="<?php echo $config['cachePath']; ?>" /></td></tr>
					<tr><td style="background: #CCC; width: 30%; text-align: right;">Uploads Path:</td><td style="background: #CCC; text-align: left;"><input type="text" name="uploadsPath" size="10" value="<?php echo $config['uploadsPath']; ?>" /></td></tr>
					<tr><td style="width: 30%; text-align: right;">Snaps! URL:</td><td colspan="2" style="text-align: left;"><input type="text" name="snapsURL" size="30" value="<?php echo $config['snapsURL']; ?>" /></td></tr>
					<tr><td style="background: #CCC; width: 30%; text-align: right;">Albums Per Page:</td><td colspan="2" style="background: #CCC; text-align: left;"><input type="text" name="albumsPP" size="10" value="<?php echo $config['albumsPP']; ?>" /></td></tr>
					<tr><td style="width: 30%; text-align: right;">Images Per Page:</td><td colspan="2" style="text-align: left;"><input type="text" name="imagesPP" size="10" value="<?php echo $config['imagesPP']; ?>" /></td></tr>
					<tr><td style="background: #CCC; width: 30%; text-align: right;">Allow Comments:</td><td colspan="2" style="background: #CCC; text-align: left;"><input type="checkbox" name="allowComment" value="1"<?php echo ($config['allowComment'] == 1) ? ' checked="checked"' : ''; ?> /></td></tr>
					<tr><td style="width: 30%; text-align: right;">Allow Image Submission:</td><td colspan="2" style="text-align: left;"><input type="checkbox" name="allowSubmit" value="1"<?php echo ($config['allowSubmit'] == 1) ? ' checked="checked"' : ''; ?> /></td></tr>
					<tr><td style="background: #CCC; width: 30%; text-align: right;">Enable Cache:</td><td colspan="2" style="background: #CCC; text-align: left;"><input type="checkbox" name="enableCache" value="1"<?php echo ($config['enableCache'] == 1) ? ' checked="checked"' : ''; ?> /></td></tr>
					<tr><td style="width: 30%; text-align: right;">Resize Method:</td><td colspan="2" style="text-align: left;"><select name="resizeMethod" size="1"><option value="gd2"<?php echo ($config['resizeMethod'] == 'gd2') ? ' selected="selected"' : ''; ?>>GD2</option><option value="im"<?php echo ($config['resizeMethod'] == 'im') ? ' selected="selected"' : ''; ?>>ImageMagick</option></select></td></tr>
					<tr><td style="background: #CCC; width: 30%; text-align: right;">ImageMagick Path:</td><td colspan="2" style="background: #CCC; text-align: left;"><input type="text" name="imPath" size="50" value="<?php echo $config['imPath']; ?>" /></td></tr>
					<tr><td colspan="3" style="padding-left: 120px;"><button type="submit" name="submit" value="edit"><img src="icons/addok.png" alt="Edit" title="Edit" /> Edit</button></td></tr>
					</form>
				</table>
			</div>