<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/* Get error messages, if set */
if (!empty($_GET['err'])) {
	$err = $_GET['err'];
} else {
	$err = '';
}
/**
* Function: makePagination() - creates page links
*
* @access public
* @var integer $albumID - the album to paginate
*/
function makePagination($start) {
	global $db, $config;
	/* Get the total number of images in this album */
	$result =& $db->query('SELECT * FROM '.TP.'images');
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$numImages = $result->numRows();
	$out = 'Page: ';
	/* Figure out number of pages we should have */
	$totalPages = ceil($numImages/$config['imagesPP']);
	/* Figure out what page we are on */
	$current = $start/$config['imagesPP']+1;
	for ($i = 1; $i <= $totalPages; $i++) {
		/* If this is the current page, echo a non-link */
		if ($i == $current) {
			$out .=  '<span style="font-weight: bold;">[ '.$i.' ]</span>&nbsp;';
		/* otherwise, echo a link to the next page */
		} else {
			/* If $i is not 1 (i.e. the first page), the link is echoed with a start variable */
			if ($i != 1) {
				$out .= '<a href="'.$_SERVER['PHP_SELF'].'?s=images&amp;start='.$config['imagesPP']*($i-1).'">'.$i.'</a>&nbsp;';
			/* otherwise, the link is echoed as the first page (i.e. no start) */
			} else {
				$out .= '<a href="'.$_SERVER['PHP_SELF'].'?s=images">'.$i.'</a>&nbsp;';
			}
		}
	}
	return $out;
}
?>
			<h3><img src="icons/camera.png" alt="Pictures" title="Pictures" style="vertical-align: middle;" /> Pictures</h3>
			<?php if (!empty($err)) { echo "\n".'<div class="box">'.$err.'</div><br />'."\n"; } ?>
			<div class="box">
				<script type="text/javascript">
					function cancel() {
						document.location.href = '<?php echo $_SERVER['PHP_SELF']; ?>?s=images';
					}
					function picture(pic) {
						window.open(pic, 'picDetail', 'width=1000,height=750,scrollbars=yes,toolbar=yes,location=yes,directories=yes,status=yes,menubar=yes');
					}
				</script>				
				<table cellpadding="2" cellspacing="0" border="0" style="width: 100%;">
<?php
/* If no action is set, and we don't have an image ID, display the list of images */
if (empty($_GET['a']) && empty($_GET['image'])) {
?>
					<tr><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Edit</td><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Delete</td><td style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Move</td><td style="background: #009; color: #FFF; font-weight: bold; width: 30%;">File Name</td><td style="background: #009; color: #FFF; font-weight: bold; width: 40%;">Image Name</td></tr>
<?php
	/* Get the start value for the list of images */
	if (!empty($_GET['start'])) {
		$start = $_GET['start'];
	} else {
		$start = 0;
	}
	/* Get the images */
	$result =& $db->query('SELECT * FROM '.TP.'images order by imageID DESC LIMIT '.$start.','.$config['imagesPP']);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	/* If we have rows, there are images, echo them */
	if ($result->numRows() > 0) {
		echo "\t\t\t\t";
		$bg = ' style="background: #CCC;"';
		while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
			$bg = ($bg == ' style="background: #CCC;"' ? '' : ' style="background: #CCC;"');
			$iurl=$config['snapsURL'].$config['albumsPath'].$line['albumID'].'/'.$line['imageFilename'];
			echo "\t".'<tr><td class="adminTD"'.$bg.'><a href="index.php?s=images&amp;a=edit&amp;image='.$line['imageID'].'"><img src="icons/editimage.png" alt="Edit" title="Edit" /></a></td><td class="adminTD"'.$bg.'><a href="index.php?s=images&amp;a=delete&amp;image='.$line['imageID'].'"><img src="icons/deleteimage.png" alt="Delete" title="Delete" /></a></td><td class="adminTD"'.$bg.'><a href="index.php?s=images&amp;a=move&amp;image='.$line['imageID'];
			echo '"><img src="icons/moveimage.png" alt="Move" title="Move" /></a></td><td'.$bg.'><a href="javascript:picture(\''.$iurl.'\');">'.$line['imageFilename'].'</a></td><td'.$bg.'>'.$line['imageName'];
			echo '<br><img src="/wgallery/phpThumb.php?w=100&f=jpeg&src='.$iurl.'" width=100>';
			echo '</tr>'."\n\t\t\t\t";
		}
		echo "\t".'<tr><td class="adminTD" colspan="5" style="text-align: right;">'.makePagination($start).'</td></tr>';
	} else {
		/* No images defined */
		echo '<tr><td colspan="5" class="adminTD">There are no images.</td></tr><tr>';
	}
?>
</table>
			<br /><span style="padding: 5px; font-weight: bold;"><a href="index.php?s=images&amp;a=new"><img style="vertical-align: bottom;" src="icons/newimage.png" alt="New Image" title="New Image" /></a> Add New Image</span>
<?php
} else {
	/* Otherwise, handle the action */
	switch($_GET['a']) {
		case 'edit' :
			/* If the form has not been submitted */
			if (empty($_POST['submit'])) {
				/* Get the image's information and display the form to edit */
				$result =& $db->query('SELECT * FROM '.TP.'images WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$out = "\t\t\t\t\t".'<form action="index.php?s=images&amp;a=edit&amp;image='.$_GET['image'].'" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Edit Image</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Image Name:</td><td><input type="text" size="30" name="imageName" value="'.$line['imageName'].'" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right; vertical-align: top;">Image Description:</td><td><textarea cols="50" rows="5" name="imageDesc">'.$line['imageDesc'].'</textarea></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="edit"><img src="icons/addok.png" alt="Edit" title="Edit" /> Edit</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, update the information in the database and display messages */
				$result =& $db->query('UPDATE '.TP.'images SET imageName = "'.mysql_escape_string($_POST['imageName']).'", imageDesc = "'.mysql_escape_string($_POST['imageDesc']).'", imageModified = UNIX_TIMESTAMP() WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'Edit Successful!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
				} else {
					$err = 'Edit Failed!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
				}
			}
			break;
		case 'new' :
			/* If the form has not been submitted */
			if (empty($_POST['submit'])) {
				/* Check to make sure there are albums to add images to */
				$result =& $db->query('SELECT * FROM '.TP.'albums');
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				/* If there are no albums, print an error message */
				if ($result->numRows() == 0) {
					$out = "\t\t\t\t\t".'<tr><td colspan="2">You have not created any albums yet!</td></tr>'."\n";
				} else {
					/* Otherwise, display the add form */
					$out = "\t\t\t\t\t".'<form enctype="multipart/form-data" action="index.php?s=images&amp;a=new" method="post">'."\n";
					$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Upload New Image</h4></td></tr>';
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Image Name:</td><td><input type="text" size="30" name="imageName" /></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right; vertical-align: top;">Image Description:</td><td><textarea cols="50" rows="5" name="imageDesc"></textarea></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Album:</td><td><select name="albumID">';
					while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
						$out .= '<option value="'.$line['albumID'].'">'.$line['albumName'].'</option>';
					}
					$out .= '</select></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">File:</td><td><input type="hidden" name="MAX_FILE_SIZE" value="2000000" /><input name="userfile" type="file" /></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="new"><img src="icons/addok.png" alt="Add" title="Add" /> Add</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'</form>'."\n";
				}
			} else {
				/* Otherwise, handle the file upload and add to database, print messages */
				$uploaddir = $config['absPath'].$config['albumsPath'].$_POST['albumID'].'/';
				$uploadfile = $uploaddir . basename($_FILES['userfile']['name']);
				if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
					$result =& $db->query('INSERT INTO '.TP.'images (imageID, albumID, imageName, imageDesc, imageFilename, imageCreated, imageModified) VALUES ("", "'.$_POST['albumID'].'", "'.mysql_escape_string($_POST['imageName']).'", "'.mysql_escape_string($_POST['imageDesc']).'", "'.basename($_FILES['userfile']['name']).'", UNIX_TIMESTAMP(), UNIX_TIMESTAMP())');
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					if ($db->affectedRows() > 0) {
						$result =& $db->query('UPDATE '.TP.'albums SET albumCount = albumCount+1, albumModified = UNIX_TIMESTAMP() WHERE albumID = '.$_POST['albumID']);
						if (DB::isError($result)) {
							die($result->getMessage());
						}
						$err = 'Image successfully uploaded and added to database.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
					} else {
						$err = 'Image successfully uploaded, but could not be added to database.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
					}
				} else {
					$err = 'Possible file upload attack!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
				}
			}
			break;
		case 'delete' :
			/* If the form has not been submitted */
			if (empty($_POST['submit'])) {
				/* Get the image's information */
				$result =& $db->query('SELECT * FROM '.TP.'images WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$img = $config['absPath'].$config['albumsPath'].$line['albumID'].'/'.$line['imageFilename'];
				/* If the file actually exists print the image's information and confirmation request */
				if (file_exists($img)) {
					$out = "\t\t\t\t\t".'<form action="index.php?s=images&amp;a=delete&amp;image='.$_GET['image'].'" method="post">'."\n";
					$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Delete Image ('.$line['imageName'].')</h4></td></tr>';
					$out .= "\t\t\t\t\t".'<tr><td colspan="2"><input type="hidden" name="album" value="'.$line['albumID'].'" />Are you sure you want to delete: <span style="font-weight: bold;">'.$line['imageName'].'</span>?</td></tr>'."\n";
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="delete"><img src="icons/addok.png" alt="Delete" title="Delete" /> Delete</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'</form>'."\n";
				} else {
					/* Otherwise, print an error */
					$err = 'File does not exist!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
				}
			} else {
				/* Otherwise, handle the deletion from the database and of the file, print messages */
				$result =& $db->query('SELECT * FROM '.TP.'images WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$img = $config['absPath'].$config['albumsPath'].$line['albumID'].'/'.$line['imageFilename'];
				$result =& $db->query('DELETE FROM '.TP.'images WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$result =& $db->query('UPDATE '.TP.'albums SET albumCount = albumCount-1, albumModified = UNIX_TIMESTAMP() WHERE albumID = '.$_POST['album']);
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					if (unlink($img)) {
						$err = 'Image Deleted.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
					} else {
						$err = 'Image deleted from database, but could not delete image file. Please remove "'.$img.'" manually.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
					}
				} else {
					$err = 'Image could not be deleted.';
					echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
				}
			}
			break;
		case 'move' :
			/* If the form is not submitted */
			if (empty($_POST['submit'])) {
				/* Get the image's name and album, the album information, and a list of all albums, display the form */
				$result =& $db->query('SELECT imageName, albumID FROM '.TP.'images WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$result =& $db->query('SELECT * FROM '.TP.'albums WHERE albumID = '.$line['albumID']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$ln =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$curAlbum = $ln['albumName'];
				$result =& $db->query('SELECT * FROM '.TP.'albums');
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$out = "\t\t\t\t\t".'<form action="index.php?s=images&amp;a=move&amp;image='.$_GET['image'].'" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Move Image ('.$line['imageName'].')</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Move From:</td><td><input type="hidden" name="fromAlbum" value="'.$line['albumID'].'" />'.$curAlbum.'</td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Move To:</td><td><select name="toAlbum">';
				while ($ln =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
					$out .= '<option value="'.$ln['albumID'].'">'.$ln['albumName'].'</option>';
				}
				$out .= '</select></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="move"><img src="icons/addok.png" alt="Move" title="Move" /> Move</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, handle the move - update database, move physical file, update database records for image counts and modification times, print messages */
				$result =& $db->query('SELECT * FROM '.TP.'images WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$img['album'] = $line['albumID'];
				$img['imageFilename'] = $line['imageFilename'];
				$src_img = $config['absPath'].$config['albumsPath'].$img['album'].'/'.$img['imageFilename'];
				$dst_img = $config['absPath'].$config['albumsPath'].$_POST['toAlbum'].'/'.$img['imageFilename'];
				$result =& $db->query('UPDATE '.TP.'albums SET albumCount = albumCount+1, albumModified = UNIX_TIMESTAMP() WHERE albumID = '.$_POST['toAlbum']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$result =& $db->query('UPDATE '.TP.'albums SET albumCount = albumCount-1, albumModified = UNIX_TIMESTAMP() WHERE albumID = '.$_POST['fromAlbum']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$result =& $db->query('UPDATE '.TP.'images SET albumID = '.$_POST['toAlbum'].', imageModified = UNIX_TIMESTAMP() WHERE imageID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					if (rename($src_img, $dst_img)) {
						$err = 'Successfully moved image.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
					} else {
						$err = 'Moved image in database, but physical move failed. Please move "'.$src_img.'" to "'.$dst_img.'".';
						echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
					}
				} else {
					$err = 'Copy failed. Please make sure the albums directories are chmod 755 or 777.';
					echo '<script type="text/javascript">document.location.href = "index.php?s=images&err='.$err.'";</script>';
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