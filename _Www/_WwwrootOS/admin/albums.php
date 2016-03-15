<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/* Get error messages, if set */
if (!empty($_GET['err'])) {
	$err = $_GET['err'];
} else {
	$err = '';
}
?>
			<h3><img src="icons/albums.png" alt="Albums" title="Albums" style="vertical-align: middle;" /> Albums</h3>
			<?php if (!empty($err)) { echo "\n".'<div class="box">'.$err.'</div><br />'."\n"; } ?>
			<div class="box">
				<script type="text/javascript">
					function cancel() {
						document.location.href = '<?php echo $_SERVER['PHP_SELF']; ?>?s=albums';
					}
				</script>
				<table cellpadding="2" cellspacing="0" border="0" style="width: 100%;">
<?php
/* If we don't have an action, or an album ID, list the albums */
if (empty($_GET['a']) && empty($_GET['album'])) {
?>
					<tr><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Edit</td><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Delete</td><td style="background: #009; color: #FFF; font-weight: bold; width: 80%;">Album</td></tr>
<?php
	/* Get the albums */
	$result =& $db->query('SELECT * FROM '.TP.'albums');
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	/* If we have albums, display the list */
	if ($result->numRows() > 0) {
		echo "\t\t\t\t";
		$bg = ' style="background: #CCC;"';
		while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
			$bg = ($bg == ' style="background: #CCC;"' ? '' : ' style="background: #CCC;"');
			echo "\t".'<tr><td class="adminTD"'.$bg.'><a href="index.php?s=albums&amp;a=edit&amp;album='.$line['albumID'].'"><img src="icons/editalbum.png" alt="Edit" title="Edit" /></a></td><td class="adminTD"'.$bg.'><a href="index.php?s=albums&amp;a=delete&amp;album='.$line['albumID'].'"><img src="icons/deletealbum.png" alt="Delete" title="Delete" /></a></td><td'.$bg.'>'.$line['albumName'].'</td></tr>'."\n\t\t\t\t";
		}
	} else {
		/* Otherwise, print an error message */
		echo '<tr><td colspan="3" class="adminTD">There are no albums.</td></tr>';
	}
?>
</table>
				<br /><span style="padding: 5px; font-weight: bold;"><a href="index.php?s=albums&amp;a=new"><img style="vertical-align: bottom;" src="icons/newalbum.png" alt="New Album" title="New Album" /></a> Create New Album</span>
<?php
} else {
	/* Otherwise, handle the action */
	switch($_GET['a']) {
		case 'edit' :
			/* If the form is not submitted, display the edit form */
			if (empty($_POST['submit'])) {
				$result =& $db->query('SELECT * FROM '.TP.'albums WHERE albumID = '.$_GET['album']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$out = "\t\t\t\t\t".'<form action="index.php?s=albums&amp;a=edit&amp;album='.$_GET['album'].'" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Edit Album</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Album Name:</td><td><input type="text" size="30" name="albumName" value="'.$line['albumName'].'" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right; vertical-align: top;">Album Description:</td><td><textarea cols="50" rows="5" name="albumDesc">'.$line['albumDesc'].'</textarea></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="edit"><img src="icons/addok.png" alt="Edit" title="Edit" /> Edit</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, handle the edit, print messages */
				$result =& $db->query('UPDATE '.TP.'albums SET albumName = "'.mysql_escape_string($_POST['albumName']).'", albumDesc = "'.mysql_escape_string($_POST['albumDesc']).'", albumModified = UNIX_TIMESTAMP() WHERE albumID = '.$_GET['album']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'Edit Successful!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
				} else {
					$err = 'Edit Failed!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
				}
			}
			break;
		case 'new' :
			/* If the form is not submitted, display the form */
			if (empty($_POST['submit'])) {
				$out = "\t\t\t\t\t".'<form action="index.php?s=albums&amp;a=new" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Create New Album</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Album Name:</td><td><input type="text" size="30" name="albumName" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right; vertical-align: top;">Album Description:</td><td><textarea cols="50" rows="5" name="albumDesc"></textarea></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="create"><img src="icons/addok.png" alt="Create" title="Create" /> Create</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, handle the add - add album to database, create album folder, print messages */
				$result =& $db->query('INSERT INTO '.TP.'albums (albumID, albumName, albumDesc, albumCount, albumCreated, albumModified) VALUES ("", "'.mysql_escape_string($_POST['albumName']).'", "'.mysql_escape_string($_POST['albumDesc']).'", "", UNIX_TIMESTAMP(), UNIX_TIMESTAMP())');
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$result =& $db->query('SELECT albumID FROM '.TP.'albums WHERE albumName = "'.mysql_escape_string($_POST['albumName']).'" AND albumDesc = "'.mysql_escape_string($_POST['albumDesc']).'"');
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
					$test = $config['absPath'].$config['albumsPath'].$line['albumID'];
					print "Creating folder: '$test'<br>";
					if (mkdir($test)) {
						$err = 'Album Created!';
						echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
					} else {
						$err = 'Album created in database, but folder creation failed. Please create a folder called "'.$line['albumID'].'" (without quotes) before adding images to this album.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
					}
				} else {
					$err = 'Album Creation Failed!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
				}
			}
			break;
		case 'delete' :
			/* If the form is not submitted */
			if (empty($_POST['submit'])) {
				/* Get the album's information */
				$result =& $db->query('SELECT * FROM '.TP.'albums WHERE albumID = '.$_GET['album']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$albumDir = $config['absPath'].$config['albumsPath'].$line['albumID'];
				$files = 0;
				/* Get a count of the number of files in the directory (check to verify it is empty) */
				if (file_exists($albumDir)) {
					$dir = dir($albumDir);
					while (false !== $entry = $dir->read()) {
						if ($entry == '.' || $entry == '..') {
							continue;
						}
						$files++;
					}
					/* If the folder is empty, print form/confirmation message */
					if ($files == 0) {
						$out = "\t\t\t\t\t".'<form action="index.php?s=albums&amp;a=delete&amp;album='.$_GET['album'].'" method="post">'."\n";
						$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Delete Album</h4></td></tr>';
						$out .= "\t\t\t\t\t".'<tr><td colspan="2">Are you sure you want to delete: <span style="font-weight: bold;">'.$line['albumName'].'</span>?</td></tr>'."\n";
						$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="delete"><img src="icons/addok.png" alt="Delete" title="Delete" /> Delete</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
						$out .= "\t\t\t\t\t".'</form>'."\n";
					} else {
						/* Otherwise, print error message */
						$out = "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Delete Album</h4></td></tr>';
						$out .= "\t\t\t\t\t".'<tr><td colspan="2">Cannot delete <span style="font-weight: bold;">'.$line['albumName'].'</span> because it contains images. Please remove the images first.</td></tr>';
					}
				}
			} else {
				/* Otherwise, handle the deletion - from database, delete physical folder, print messages */
				$result =& $db->query('DELETE FROM '.TP.'albums WHERE albumID = '.$_GET['album']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$albumDir = $config['absPath'].$config['albumsPath'].$_GET['album'];
					if (rmdir($albumDir)) {
						$err = 'Album deleted.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
					} else {
						$err = 'Album deleted from database, but could not delete album folder. Please remove "'.$albumDir.'" manually.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
					}
				} else {
					$err = 'Album could not be deleted.';
					echo '<script type="text/javascript">document.location.href = "index.php?s=albums&err='.$err.'";</script>';
				}
			}
			break;
		default :
			break;
	}
	echo $out."\t\t\t\t".'</table>'."\n";
}
?>
			</div>