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
* Include the Snaps! configuration file
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
if (isset($_SESSION['loggedIn'])) {
	/* Get section, if set */
	if (!empty($_GET['s'])) {
		$section = $_GET['s'];
	} else {
		$section = '';
}

function rmdirmy($dirname)
{
	// Simple delete for a file
	if (is_file($dirname)) {
		return unlink($dirname);
	}
	
	// Sanity check
	if (!is_dir($dirname)) {
		echo "no directory by that name:".$dirname."<hr>";
		return false;
	}

	// Loop through the folder
	$dir = dir($dirname);
	while (false !== $entry = $dir->read()) {
		// Skip pointers
		if ($entry == '.' || $entry == '..') {
			continue;
		}
		// Recurse
		rmdirmy("$dirname/$entry");
	}// end while looping
	// Clean up
	$dir->close();
	return rmdir($dirname);
}

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>Snaps! Gallery Admin</title>
<link rel="stylesheet" type="text/css" href="admin.css" />
<script type="text/javascript" src="iepngfix.js"></script>
</head>
<body>
<div style="width: 700px; margin: 0 auto; text-align: center; background: #FFF; border: 1px solid #666;"><div style="float: right; padding: 5px;">Logged In as: <?php echo $_SESSION['name']; ?> | <a href="index.php?action=logout">Log Out</a></div>
	<h1 style="float: left;"><a href="index.php"><img src="../images/snaps_logo.png" alt="Snaps! Gallery" title="Snaps! Gallery" /></a></h1>
	<h3 style="clear: both; text-indent: 10px; text-align: left;">Snaps! Administration</h3>
	<table cellpadding="3" cellspacing="3" border="0" style="padding: 10px;">
		<tr>
			<td style="width: 180px; vertical-align: top;">
			<ul id="adminav">
				<li><a <?php if ($section == 'albums') { echo 'class="cur" '; } ?>href="index.php?s=albums"><img src="icons/albums.png" alt="Albums" title="Albums" style="vertical-align: middle;" /> Albums</a></li>
				<li><a <?php if ($section == 'images') { echo 'class="cur" '; } ?>href="index.php?s=images"><img src="icons/camera.png" alt="Pictures" title="Pictures" style="vertical-align: middle;" /> Pictures</a></li>
				<li><a <?php if ($section == 'users') { echo 'class="cur" '; } ?>href="index.php?s=users"><img src="icons/users.png" alt="Users" title="Users" style="vertical-align: middle;" /> Users</a></li>
				<li><a <?php if ($section == 'comments') { echo 'class="cur" '; } ?>href="index.php?s=comments"><img src="icons/comments.png" alt="Comments" title="Comments" style="vertical-align: middle;" /> Comments</a></li>
				<li><a <?php if ($section == 'uploads') { echo 'class="cur" '; } ?>href="index.php?s=uploads"><img src="icons/uploads.png" alt="Uploads" title="Uploads" style="vertical-align: middle;" /> Uploads</a></li>
				<!--li><a <?php if ($section == 'uploads') { echo 'class="cur" '; } ?>href="index.php?s=uploads&a=ex"><img src="icons/uploads.png" alt="Uploads" title="Uploads" style="vertical-align: middle;" /> UploadsEx</a></li-->
				<li><a <?php if ($section == 'config') { echo 'class="cur" '; } ?>href="index.php?s=config"><img src="icons/config.png" alt="Settings" title="Settings" style="vertical-align: middle;" /> Settings</a></li>
				<li><a href="../index.php"><img src="icons/gallery.png" alt="Gallery" title="Gallery" style="vertical-align: middle;" /> My Gallery</a></li>
			</ul>
			</td>
			<td style="vertical-align: top;" id="content">
<?php
	/* If we have an action */
	if(isset($_GET['action'])) {
		/* If the action is logout, log the user out, print messages */
		if ($_GET['action'] == 'logout') {
			session_unset();
			session_destroy();
			if(!session_is_registered('userID')){
				echo '<span style="font-weight: bold; color: #900;">You have been logged out.</span><br /><br /><a href="../index.php">Gallery</a> index.<br /><br /><a href="../index.php?action=login">Log In</a> again.';
			}
		} else if ($_GET['action'] == 'optimize') {
			/* Otherwise, if the action is optimize, optimize the tables, print messages */
			$result =& $db->query('OPTIMIZE TABLE `'.TP.'albums` , `'.TP.'comments` , `'.TP.'config` , `'.TP.'images` , `'.TP.'uploads` , `'.TP.'users`');
			if (DB::isError($result)) {
				die($result->getMessage());
			}
			echo '<h3><img src="icons/utilities.png" alt="Utilities" title="Utilities" style="vertical-align: middle;" /> Utilities</h3><div class="box"><h4>Optimize Tables</h4>';
			while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
				$tname = explode('.', $line['Table']);
				echo $tname[1].' - '.$line['Msg_text'].'<br />';
			}
			echo '</div>';
		} else if ($_GET['action'] == 'recreatemd5') {
			/* Creating MD5 hashes */
			require_once('zipclass.php');
			echo '<h3><img src="icons/utilities.png" alt="Utilities" title="Utilities" style="vertical-align: middle;" /> Utilities</h3><div class="box"><h4>Recreating md5. List of conflicts</h4>';
			$count=0;
			$countall=0;
			$dirsource=$config['absPath']."albums";
			$md5folder=$config['absPath']."cache/md5";
			$md5folder=realpath("../".$md5folder);
			$dirsource=realpath("../".$dirsource);
			echo 'Albums dir='.$dirsource."<br>";
			echo 'MD5 dir='.$md5folder."<br>";
			$toskip=$_GET['skip'];
			if($toskip+0==0){
				echo 'Removing old md5...<br>';
				rmdirmy($md5folder);
				mkdir($md5folder,0777,true);
			}
			echo 'Generating new ones...<br><br>';
			processMD5In($db, $dirsource, $count, $toskip, $countall);
			if($count==0){
				echo 'No conflicts found!!!';
			}
			echo '</div>';
		}
	} else {
		/* Otherwise, if the section is NULL, include the main page (statistics), otherwise, include the appropriate section page */
		if ($section == '') {
			include('main.php');
		} else {
			include($section.'.php');
		}
	}
?>

		</td>
		</tr>
	</table>
</div>
<div class="snapsCopy">Powered by <a href="http://labs.sonicdesign.us/projects/Snaps!/">Snaps!</a> v<?php echo $config['version']; ?><p class="snapsNotes">This program uses icons from the <a href="http://www.kde.org">KDE</a> Project by <a href="http://www.everaldo.com">Everaldo Coelho</a> released under the <a href="http://www.gnu.org/licenses/gpl.html">GNU GPL</a>, and the Universal PNG Enabler from <a href="http://dsandler.org/">Dan Sandler</a>, which incorporates code from <a href="http://www.youngpup.net/">Aaron Boodman</a> (inline) and <a href="http://www.allinthehead.com/">Drew McLellan</a> (background).</p></div>
</body>
</html>
<?php
} else {
	die('Security violation.');
}
?>