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
/* Make sure we have a valid administrator session */
if ($_SESSION['loggedIn']) {
	/* Get the comment ID */
	if (!empty($_GET['comment'])) {
		$com = $_GET['comment'];
	} else {
		$com = '';
	}
	/* If we have a comment ID */
	if (!empty($com)) {
		/* Get the comment and display it */
		$result =& $db->query('SELECT * FROM '.TP.'comments WHERE commentID = '.$com);
		if (DB::isError($result)) {
			die($result->getMessage());
		}
		$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
		$out = $line['commentBody'].'<br /><br />Posted by: '.$line['commentName'].' on '.date("m-d-Y", $line['commentCreated']);
	} else {
		/* Otherwise, print an error message */
		$out = 'Error: Invalid Comment ID';
	}
?>
<html>
<head>
<title>Snaps! Gallery Admin :: Comment Detail</title>
<link rel="stylesheet" type="text/css" href="admin.css" />
</head>
<body>
<div style="background: #FFF; width: 99%; padding: 5px; border: 1px solid #666;">
<span style="display: block; background: #009; padding: 5px; color: #FFF; font-weight: bold;">Comment Detail</span><br />
<?php echo $out; ?>
</div><br />
<div style="background: #FFF; width: 60px; float: right; text-align: center; padding: 5px; border: 1px solid #666;">
<a href="javascript:window.close();">Close</a>
</div>
</body>
</html>
<?php
} else {
	/* Otherwise, fail. */
	die('Security violation.');
}
?>