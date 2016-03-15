<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
session_start();
/*
* Include the DB class from PEAR for database access.
* You must have PEAR and the DB package installed to
* use sNaps!. If you need instructions for installing PEAR
* and the DB package, please see http://pear.php.net
*/
require_once('DB.php');

/*
* Include the configuration
*/
require_once('main.config.php');

/* Connect to the database */
$db =& DB::connect($dsn);
if (DB::isError($db)) {
    die($db->getMessage());
}

/* Retrieve configuration information from the database */
$result =& $db->query('SELECT * FROM '.TP.'config');
if (DB::isError($result)) {
	die($result->getMessage());
}
while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
	$config[$line['var']] = $line['val'];
}
/* Check to make sure we have a valid administrator session */
if ($_SESSION['loggedIn']) {
	/* Get the image ID */
	if (!empty($_GET['image'])) {
		$img = $_GET['image'];
	} else {
		$img = '';
	}
	/* If we have an image ID */
	if (!empty($img)) {
		/* Get and display the image's information */
		$result =& $db->query('SELECT * FROM '.TP.'uploads WHERE upID = '.$img);
		if (DB::isError($result)) {
			die($result->getMessage());
		}
		$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
		$filename = '../'.$config['uploadsPath'].$line['upFilename'];
		$i_imagesize = getimagesize ($filename);
		$out = $line['upName'].'. <a href="javascript:window.close();">Close window</a><br /><br />Submitted by: <a href="mailto:'.$line['upSubEmail'].'">'.$line['upSubName'].'</a> on '.date("m-d-Y", $line['upCreated']);
		$out.='; size: '.$i_imagesize[0].'x'.$i_imagesize[1];
		$out.='<br /><br /><img width=800 src="'.$filename.'">';
	} else {
		/* Otherise, print an error message */
		$out = 'Error: Invalid Upload ID';
	}
?>
<html>
<head>
<title>Snaps! Gallery Admin :: Upload Detail</title>
<link rel="stylesheet" type="text/css" href="admin.css" />
</head>
<body>
<center>
<div width=90% align=center style="width: 90%; background: #FFF; width: 100%; padding: 5px; border: 1px solid #666;">
<span style="display: block; background: #009; padding: 5px; color: #FFF; font-weight: bold;">Upload Detail.</span><br />
<?php echo $out; ?>
</div>
</center>
</body>
</html>
<?php
} else {
	/* Otherwise, fail. */
	die('Security violation.');
}
?>