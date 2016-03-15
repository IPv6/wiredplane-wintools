<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
session_start();

if (!empty($_GET['step'])) {
	$step = $_GET['step'];
} else {
	$step = '';
}
define('STR_OK', '<img src="button_ok.png" alt="OK" title="OK" />');
define('STR_ERR', '<img src="button_error.png" alt="Error" title="Error" />');
define('STR_WARN', '<img src="button_warning.png" alt="Warning" title="Warning" />');
/**
 * Get the GD Library version
 *
 * @version	1.0.0
 * @author		Dave Scott <dscott@sonicdesign.us>
 * @return		GD Version
 */
function gd_version() {
   static $gd_version_number = null;
   if ($gd_version_number === null) {
       ob_start();
       phpinfo(8);
       $module_info = ob_get_contents();
       ob_end_clean();
       if (preg_match("/\bgd\s+version\b[^\d\n\r]+?([\d\.]+)/i",
               $module_info,$matches)) {
           $gd_version_number = $matches[1];
       } else {
           $gd_version_number = 0;
       }
   }
   return $gd_version_number;
}

/**
 * Check if a file exists in the include path
 *
 * @version		1.2.0
 * @author		Aidan Lister <aidan@php.net>
 * @param		string	$file	Name of the file to look for
 * @return		bool		TRUE if the file exists, FALSE if it does not
 */
function file_exists_incpath ($file)
{
    $paths = explode(PATH_SEPARATOR, get_include_path());
 
    foreach ($paths as $path) {
        // Formulate the absolute path
        $fullpath = $path . DIRECTORY_SEPARATOR . $file;
 
        // Check it
        if (file_exists($fullpath)) {
            return true;
        }
    }
 
    return false;
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>Snaps! Gallery Installation</title>
<script type="text/javascript" src="iepngfix.js"></script>
<link rel="stylesheet" type="text/css" href="../snaps.css" />
<style type="text/css">
img {
	vertical-align: middle;
}

.btn {
	color: #000;
	text-decoration: none;
	background: #BBB;
	font-weight: bold;
	padding: 3px;
	border: 2px outset #BBB;
}

.btn:hover {
	color: #FFF;
	text-decoration: underline;
}

td {
	padding: 6px;
}

.box {
	background: #EEE;
	padding: 4px;
	border: 1px solid #666;
}
</style>
</head>
<body>
<div style="width: 700px; margin: 0 auto; text-align: center; background: #FFF; border: 1px solid #666;">
	<h1 style="float: left;"><img src="../images/snaps_logo.png" alt="Snaps! Gallery" title="Snaps! Gallery" /></h1>
	<div style="clear: both; text-align: left; padding: 10px;">
<?php
	if (empty($step)) {
?>
		<h3>Snaps! Installation - Step 1 - Server Support</h3>
		<div class="box">
			<ul>
				<li><?php echo STR_OK; ?> - Everything's good to go.</li>
				<li style="list-style: none;"><br /></li>
				<li><?php echo STR_WARN; ?> - Snaps! may still run, but the warning should be corrected for proper running of Snaps!.</li>
				<li style="list-style: none;"><br /></li>
				<li><?php echo STR_ERR; ?> - Correct the error and re-run this install script. Snaps! will not run if this is not corrected.</li>
			</ul>
		</div>
		<br />
		<div class="box">
		<table cellpadding="0" cellspacing="0" border="0" style="width: 100%;">
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">PEAR DB Installed:</td><td style="background: #CCC;">
				<?php
				if (!file_exists_incpath('DB.php')) {
					echo STR_ERR.' Snaps! requires the PEAR DB Library.';
				} else {
					echo STR_OK;
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">GD Extension:</td><td style="background: #EEE;">
				<?php
				if ($gdinfo = gd_info()) {
					if (gd_version() < 2) {
						echo STR_WARN.' GD Version is not 2.0+. Snaps! may fail to run correctly with a GD version less than 2.0.';
					} else {
						echo STR_OK;
					}
				} else {
					echo STR_ERR.' The GD library must be installed and loaded for Snaps! to work. Please correct this and then re-run this script.';
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">JPG Support:</td><td style="background: #CCC;">
				<?php
				if ($gdinfo = gd_info()) {
					if (!$gdinfo['JPG Support']) {
						echo STR_ERR.' Snaps! must have JPG Support.';
					} else {
						echo STR_OK;
					}
				} else {
					echo STR_ERR.' The GD library must be installed and loaded for Snaps! to work. Please correct this and then re-run this script.';
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">PNG Support:</td><td style="background: #EEE;">
				<?php
				if ($gdinfo = gd_info()) {
					if (!$gdinfo['PNG Support']) {
						echo STR_WARN.' You will not be able to use PNG images in your gallery.';
					} else {
						echo STR_OK;
					}
				} else {
					echo STR_ERR.' The GD library must be installed and loaded for Snaps! to work. Please correct this and then re-run this script.';
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">GIF Support:</td><td style="background: #CCC;">
				<?php
				if ($gdinfo = gd_info()) {
					if (!$gdinfo['GIF Read Support'] || !$gdinfo['GIF Create Support']) {
						echo STR_WARN.' You will not be able to use GIF images in your gallery.';
					} else if ($gdinfo['GIF Read Support'] && $gdinfo['GIF Create Support']) {
						echo STR_OK;
					} else {
						echo STR_ERR.' Partial GIF support detected. GIFs will be disabled.';
					}
				} else {
					echo STR_ERR.' The GD library must be installed and loaded for Snaps! to work. Please correct this and then re-run this script.';
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Albums Directory is Writable:</td><td style="background: #EEE;">
				<?php
				if (file_exists('../albums')) {
					if (is_writable('../albums')) {
						echo STR_OK;
					} else {
						echo STR_ERR.' The Albums directory is not writable. Please chmod the Albums directory to 777.';
					}
				} else {
					echo STR_ERR.' The Albums directory does not exist. Please create it.';
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Uploads Directory is Writable:</td><td style="background: #CCC;">
				<?php
				if (file_exists('../uploads')) {
					if (is_writable('../uploads')) {
						echo STR_OK;
					} else {
						echo STR_ERR.' The Uploads directory is not writable. Please chmod the Uploads directory to 777.';
					}
				} else {
					echo STR_ERR.' The Uploads directory does not exist. Please create it.';
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Cache Directory is Writable:</td><td style="background: #EEE;">
				<?php
				if (file_exists('../cache')) {
					if (is_writable('../cache')) {
						echo STR_OK;
					} else {
						echo STR_ERR.' The Cache directory is not writable. Please chmod the Cache directory to 777.';
					}
				} else {
					echo STR_ERR.' The Cache directory does not exist. Please create it.';
				}
				?>
				</td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Config File is Writable:</td><td style="background: #CCC;">
				<?php
				if (is_writable('admin/main.config.php')) {
						echo STR_OK;
				} else {
					echo STR_ERR.' Please chmod the config file (admin/main.config.php) to 777.';
				}
				?>
				</td>
			</tr>
		</table>
		</div>
		<br />
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%; background: #EEE;">
			<tr>
				<td style="background: #EEE; text-align: right;">If there are no <?php echo STR_ERR; ?>, click next to go to the next step. <a href="<?php echo $_SERVER['PHP_SELF']; ?>?step=2" class="btn">Next</a></td>
			</tr>
		</table>
		</div>
<?php
	} else if ($step == 2) {
?>
		<h3>Snaps! Installation - Step 2 - Snaps! Configuration</h3>
		<div class="box"><p>Please make sure the values that the install script detected are correct. In most cases they are, and you can leave them as is.</p></div><br />
		<form action="<?php echo $_SERVER['PHP_SELF']; ?>?step=3" method="post">
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Absolute Path:</td><td style="background: #EEE;"><input type="text" name="absPath" size="50" value="<?php echo str_replace('install/index.php', '', $_SERVER['SCRIPT_FILENAME']); ?>" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">URL to Snaps!:</td><td style="background: #CCC;"><input type="text" name="snapsURL" size="50" value="<?php echo 'http://'.$_SERVER['SERVER_NAME'].str_replace('install/index.php', '', $_SERVER['SCRIPT_NAME']); ?>" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Albums Directory:</td><td style="background: #EEE;"><input type="text" name="albumsPath" size="50" value="albums/" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Uploads Directory:</td><td style="background: #CCC;"><input type="text" name="uploadsPath" size="50" value="uploads/" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Cache Directory:</td><td style="background: #EEE;"><input type="text" name="cachePath" size="50" value="cache/" /></td>
			</tr>
		</table>
		</div><br />
		<div class="box"><p>Set the options below for how you want your gallery to be displayed and the features you want to use.</p></div><br />
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Gallery Title:</td><td style="background: #EEE;"><input type="text" name="title" size="50" value="Snaps! Gallery" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Albums Per Page:</td><td style="background: #CCC;"><input type="text" name="albumsPP" size="10" value="9" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Images Per Page:</td><td style="background: #EEE;"><input type="text" name="imagesPP" size="10" value="9" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Enable Comments:</td><td style="background: #CCC;"><input type="checkbox" name="allowComment" value="1" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Enable Image Submission:</td><td style="background: #EEE;"><input type="checkbox" name="allowSubmit" value="1" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Enable Caching:</td><td style="background: #CCC;"><input type="checkbox" name="enableCache" value="1" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Image Resize Method:</td><td style="background: #EEE;"><select name="resizeMethod" size="1"><option value="gd2">GD2</option><option value="im">ImageMagick</option></select></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">ImageMagick Path:</td><td style="background: #CCC;"><input type="text" name="imPath" size="50" value="" /></td>
			</tr>
		</table>
		</div>
		<br />
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%; background: #EEE;">
			<tr>
				<td style="background: #EEE; text-align: right;">If everything looks good, click next to go to the next step. <a href="#" onclick="document.forms[0].submit();" class="btn" />Next</a></td>
			</tr>
		</table>
		</div>
		</form>
<?php
} else if ($step == 3) {
	$_SESSION['configVars'] = $_POST;
	if (empty($_POST['allowComment'])) {
		$_SESSION['configVars']['allowComment'] = 0;
	}
	if (empty($_POST['allowSubmit'])) {
		$_SESSION['configVars']['allowSubmit'] = 0;
	}
	if (empty($_POST['enableCache'])) {
		$_SESSION['configVars']['enableCache'] = 0;
	}
	if (!empty($_POST['imPath'])) {
		$_SESSION['configVars']['imPath'] = str_replace('\\', '/', $_SESSION['configVars']['imPath']);
	}
?>
		<h3>Snaps! Installation - Step 3 - Database Configuration</h3>
		<form action="<?php echo $_SERVER['PHP_SELF']; ?>?step=4" method="post">
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">DB Host:</td><td style="background: #EEE;"><input type="text" name="dbHost" size="50" value="localhost" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">DB User:</td><td style="background: #CCC;"><input type="text" name="dbUser" size="50" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">DB Pass:</td><td style="background: #EEE;"><input type="text" name="dbPass" size="50" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">DB Name:</td><td style="background: #CCC;"><input type="text" name="dbName" size="50" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Table Prefix:</td><td style="background: #EEE;"><input type="text" name="tblPrefix" size="50" value="snaps_" /></td>
			</tr>
		</table>
		</div>
		<br />
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%; background: #EEE;">
			<tr>
				<td style="background: #EEE; text-align: right;">Double check your information and then click Next to install the database. <a href="#" onclick="document.forms[0].submit();" class="btn" />Next</a></td>
			</tr>
		</table>
		</div>
		</form>
<?php
} else if ($step == 4) {
	if (!file_exists('admin/main.config.php')) {
		echo '<p>'.STR_ERR.' The config file (admin/main.config.php) was not found. Please correct the error and run this script again.</p>';
	} else {
		if (is_writable('admin/main.config.php')) {
			include('snaps_db.php');
			echo '<h3>Snaps! Installation - Step 4 - Perform Setup Operations</h3><div class="box"><p>Attempting connection to the database...</p>';
			include('DB.php');
			/* Connect to the database */
			$dsn = "mysql://{$_POST['dbUser']}:{$_POST['dbPass']}@{$_POST['dbHost']}/{$_POST['dbName']}";
			$db =& DB::connect($dsn);
			if (DB::isError($db)) {
				echo ' '.STR_ERR.' '.$db->getMessage().'</p><p>Please correct the error and run this script again.</p>';
			} else {
?>
				<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
					<tr>
						<td style="background: #CCC; text-align: right; width: 30%;">Connect:</td><td style="background: #CCC;"><?php echo STR_OK; ?></td>
					</tr>
				</table>
<?php
				echo '<p>Attempting to save the configuration to the config file...</p>';
				$cfgFile = file_get_contents('admin/main.config.php');
				$cfgvars = array(	0 => 'dbHost',
												1 => 'dbUser',
												2 => 'dbPass',
												3 => 'dbName',
												4 => 'tblPrefix');
				for ($i = 0; $i < count($cfgvars); $i++) {
					$cfgFile = ereg_replace('%'.$cfgvars[$i].'%', $_POST[$cfgvars[$i]], $cfgFile);
				}
				$cfgFile = ereg_replace('%title%', $_SESSION['configVars']['title'], $cfgFile);
				$fp = fopen('admin/main.config.php', 'w');
				if (!$fp) {
					echo ' '.STR_ERR.' There was a problem opening the config file (admin/main.config.php) for writing. Please chmod the config file to 777.</p>';
				} else {
					if (fwrite($fp, $cfgFile) === FALSE) {
						echo ' '.STR_ERR.' Could not write the configuration information to the config file (admin/main.config.php). Please copy the text below into the config file.</p><br /><pre>'.$cfgFile.'</pre>';
					} else {
						$err = 0;
?>
				<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
					<tr>
						<td style="background: #CCC; text-align: right; width: 30%;">Configuration Save:</td><td style="background: #CCC;"><?php echo STR_OK; ?></td>
					</tr>
				</table>
<?php
						echo '<p>Attempting installation of the database tables...</p>';
?>
				<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
<?php
						echo '<tr><td style="background: #EEE; text-align: right; width: 30%;">Albums Table:</td>';
						$result =& $db->query($AlbumsSQL);
						if (DB::isError($result)) {
							echo '<td style="background: #EEE;">'.STR_ERR.' '.$result->getMessage().'<p>Please correct the error and run this script again.</p></td></tr>';
							$err++;
						} else {
							echo '<td style="background: #EEE;">'.STR_OK.'</td></tr>';
						}
						echo '<tr><td style="background: #CCC; text-align: right; width: 30%;">Images Table:</td>';
						$result =& $db->query($ImagesSQL);
						if (DB::isError($result)) {
							echo '<td style="background: #CCC;">'.STR_ERR.' '.$result->getMessage().'<p>Please correct the error and run this script again.</p></td></tr>';
							$err++;
						} else {
							echo '<td style="background: #CCC;">'.STR_OK.'</td></tr>';
						}
						echo '<tr><td style="background: #EEE; text-align: right; width: 30%;">Comments Table:</td>';
						$result =& $db->query($CommentsSQL);
						if (DB::isError($result)) {
							echo '<td style="background: #EEE;">'.STR_ERR.' '.$result->getMessage().'<p>Please correct the error and run this script again.</p></td></tr>';
							$err++;
						} else {
							echo '<td style="background: #EEE;">'.STR_OK.'</td></tr>';
						}
						echo '<tr><td style="background: #CCC; text-align: right; width: 30%;">Uploads Table:</td>';
						$result =& $db->query($UploadsSQL);
						if (DB::isError($result)) {
							echo '<td style="background: #CCC;">'.STR_ERR.' '.$result->getMessage().'<p>Please correct the error and run this script again.</p></td></tr>';
							$err++;
						} else {
							echo '<td style="background: #CCC;">'.STR_OK.'</td></tr>';
						}
						echo '<tr><td style="background: #EEE; text-align: right; width: 30%;">Users Table:</td>';
						$result =& $db->query($UsersSQL);
						if (DB::isError($result)) {
							echo '<td style="background: #EEE;">'.STR_ERR.' '.$result->getMessage().'<p>Please correct the error and run this script again.</p></td></tr>';
							$err++;
						} else {
							echo '<td style="background: #EEE;">'.STR_OK.'</td></tr>';
						}
						echo '<tr><td style="background: #CCC; text-align: right; width: 30%;">Config Table:</td>';
						$result =& $db->query($ConfigSQL);
						if (DB::isError($result)) {
							echo '<td style="background: #CCC;">'.STR_ERR.' '.$result->getMessage().'<p>Please correct the error and run this script again.</p></td></tr>';
							$err++;
						} else {
							echo '<td style="background: #CCC;">'.STR_OK.'</td></table>';
							echo '<p>Attempting to populate the config table...</p>';
							$cfgerr = 0;
							for ($i = 0; $i < count($ConfigVarsSQL); $i++) {
								$result =& $db->query($ConfigVarsSQL[$i]);
								if (DB::isError($result)) {
									$cfgerr++;
								}
							}
							if ($cfgerr > 0) {
								echo ' '.STR_ERR.' '.$result->getMessage().'<p>Please correct the error and run this script again.</p>';
							} else {
?>
				<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
					<tr>
						<td style="background: #CCC; text-align: right; width: 30%;">Config Population:</td><td style="background: #CCC;"><?php echo STR_OK; ?></td>
					</tr>
				</table>
				</div>
				<br />
				<div class="box">
<?php
							}
						}
						if ($err > 0) {
							echo ' '.STR_ERR.' <p>Please correct any errors and run this script again.</p>';
						} else {
							echo '<p style="text-align: right; margin: 0; padding: 3px;">If there are no '.STR_ERR.', click Next to create a user and begin using Snaps!. <a href="'.$_SERVER['PHP_SELF'].'?step=5" class="btn" />Next</a></p>';
						}
					}
				}
			}
		} else {
			echo '<p>'.STR_ERR.' The config file (admin/main.config.php) is not writable. Please chmod the config file to 777 and run this script again.</p>';
		}
	}
?>
			</div>
<?php
} else if ($step == 5) {

?>
		<script type="text/javascript">
		function validate() {
			err = 0;
			errmsg = 'There are some problems with your submission:\n';
			if (document.makeUser.userFname.value == '') {
				errmsg += " - You must enter a First Name.\n";
				err++;
			}
			if (document.makeUser.userLname.value == '') {
				errmsg += " - You must enter a Last Name.\n";
				err++;
			}
			if (document.makeUser.userEmail.value == '') {
				errmsg += " - You must enter an E-mail.\n";
				err++;
			}
			if (document.makeUser.username.value == '') {
				errmsg += " - You must enter a username.\n";
				err++;
			}
			pw1 = document.makeUser.userpass1.value;
			pw2 = document.makeUser.userpass2.value;
			if (pw1 == '' || pw2 == '') {
				errmsg += " - You must enter a password twice.\n";
				err++;
			} else {
				if (pw1 != pw2) {
					errmsg += " - Passwords do not match.\n";
					err++;
				}
			}
			if (err > 0) {
				alert(errmsg);
				return false;
			} else {
				return true;
			}
		}
		</script>
		<h3>Snaps! Installation - Step 5 - User Creation</h3>
		<form name="makeUser" action="<?php echo $_SERVER['PHP_SELF']; ?>?step=6" method="post" onsubmit="return validate()">
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">First Name:</td><td style="background: #EEE;"><input type="text" name="userFname" size="50" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Last Name:</td><td style="background: #CCC;"><input type="text" name="userLname" size="50" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">E-Mail:</td><td style="background: #EEE;"><input type="text" name="userEmail" size="50" /></td>
			</tr>
			<tr>
				<td style="background: #CCC; text-align: right; width: 30%;">Username:</td><td style="background: #CCC;"><input type="text" name="username" size="20" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Password:</td><td style="background: #EEE;"><input type="password" name="userpass1" id="userpass1" size="10" /></td>
			</tr>
			<tr>
				<td style="background: #EEE; text-align: right; width: 30%;">Verify Password:</td><td style="background: #EEE;"><input type="password" name="userpass2" id="userpass2" size="10" /></td>
			</tr>
		</table>
		</div>
		<br />
		<div class="box">
		<table cellpadding="3" cellspacing="0" border="0" style="width: 100%; background: #EEE;">
			<tr>
				<td style="background: #EEE; text-align: right;">Click Next to create your user. <input type="submit" name="submit" value="Next" class="btn" /></td>
			</tr>
		</table>
		</div>
		</form>
<?php
} else if ($step == 6) {
	echo '<h3>Snaps! Installation - Finished!</h3><div class="box">';
	include('DB.php');
	include('../admin/main.config.php');
	$db =& DB::connect($dsn);
	if (DB::isError($db)) {
		echo '<p>'.STR_ERR.' '.$db->getMessage().'</p><p>Please correct the error and run this script again.</p>';
	}
	$subUser = array(	'userID' => '',
									'userFname' => mysql_real_escape_string($_POST['userFname']),
									'userLname' => mysql_real_escape_string($_POST['userLname']),
									'userEmail' => mysql_real_escape_string($_POST['userEmail']),
									'username' => mysql_real_escape_string($_POST['username']),
									'userpass' => md5($_POST['userpass1']),
									'userLastLogin' => time());
	$result =& $db->query("INSERT INTO ".TP."users (`userID`, `userFname`, `userLname`, `userEmail`, `username`, `userpass`, `userLastLogin`) VALUES ('{$subUser['userID']}', '{$subUser['userFname']}', '{$subUser['userLname']}', '{$subUser['userEmail']}', '{$subUser['username']}', '{$subUser['userpass']}', '{$subUser['userLastLogin']}')");
	if (DB::isError($result)) {
		echo '<p>'.STR_ERR.' '.$result->getMessage().' Please correct the error and run this script again.</p>';
	} else {
		echo '<p>User Creation: '.STR_OK.'</p>';
		$_SESSION['name'] = $_POST['userFname'].' '.$_POST['userLname'];
		$_SESSION['loggedIn'] = TRUE;
?>
		<p>Congratulations! You have successfully completed the installation of Snaps!. To begin using Snaps!, click the Finish button below.</p>
		<p style="text-align: right;"><a class="btn" href="../admin/">Finish</a></p>
		</div>
<?php
	}
}
?>
	</div>
</div>
<div class="snapsCopy">Powered by <a href="http://labs.sonicdesign.us/projects/Snaps!/">Snaps! Gallery</a> v1.4.4<p class="snapsNotes">This program uses icons from the <a href="http://www.kde.org">KDE</a> Project by <a href="http://www.everaldo.com">Everaldo Coelho</a> released under the <a href="http://www.gnu.org/licenses/gpl.html">GNU GPL</a>, and the Universal PNG Enabler from <a href="http://dsandler.org/">Dan Sandler</a>, which incorporates code from <a href="http://www.youngpup.net/">Aaron Boodman</a> (inline) and <a href="http://www.allinthehead.com/">Drew McLellan</a> (background).</p></div>
</body>
</html>