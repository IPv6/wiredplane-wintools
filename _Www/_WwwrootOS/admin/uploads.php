<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/* Get error messages, if set */
if (!empty($_GET['err'])) {
	$err = $_GET['err'];
} else {
	$err = '';
}

function md5_xfile($image,$tmp)
{
	$w=30;
	$h=30;
	$src = imagecreatefromjpeg($image);
	$dst = ImageCreateTrueColor($w,$h);
	@ImageCopyResized($dst,$src,0,0,0,0,$w,$h,@imagesx($src),@imagesy($src));
	$randomfname=$tmp."/tmp_"."_".mt_rand()."_".mt_rand();
	imagegif($dst, $randomfname);
	//echo($randomfname);
	$res=md5_file($randomfname);
	//echo("md5=".$res."size=".filesize($randomfname)."<hr>");
	@ImageDestroy($src); 
	@ImageDestroy($dst);
	Unlink($randomfname);
	return $res;
}
?>
			<h3><img src="icons/uploads.png" alt="Uploads" title="Uploads" style="vertical-align: middle;" /> Uploads</h3>
			<?php if (!empty($err)) { echo "\n".'<div class="box">'.$err.'</div><br />'."\n"; } ?>
			<div class="box" width=99%>
				<script type="text/javascript">
					function cancel() {
						document.location.href = '<?php echo $_SERVER['PHP_SELF']; ?>?s=uploads';
					}
					function picture(id) {
						window.open('picDetail.php?image='+id, 'picDetail', 'width=1000,height=750,scrollbars=yes,toolbar=yes,location=yes,directories=yes,status=yes,menubar=yes');
					}
				</script>
				<table cellpadding="2" cellspacing="0" border="0" style="width: 100%;">
<?php
/* If we don't have an action, or an image */
if (empty($_GET['a']) && empty($_GET['image'])) {
?>
					<tr><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Edit</td><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Delete</td><td style="background: #009; color: #FFF; font-weight: bold; width: 10%; text-align: center;">Add</td><td style="background: #009; color: #FFF; font-weight: bold; width: 30%;">Image Name</td><td style="background: #009; color: #FFF; font-weight: bold; width: 40%;">Image Description</td></tr>
<?php
	/* preparing albums list */
	$result =& $db->query('SELECT * FROM '.TP.'albums');
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$before = "\t\t\t\t\t".'<form target=_blank action="index.php?s=uploads&amp;a=move&amp;confirm=no&amp;image=';
	$after = '" method="post">'."\n";
	$after .= "\t\t\t\t\t".'<select name="toAlbum">';
	$after .= '<option value=""></option>';
	$after .= '<option value="-1">Delete image</option>';
	while ($ln =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
		$after .= '<option value="'.$ln['albumID'].'">'.$ln['albumName'].'</option>';
	}
	$after .= '</select>'."\n\t\t\t\t\t".'<button type="submit" name="submit" value="move"><img src="icons/addok.png" alt="Add" title="Add" /> Add</button></form>'."\n";			

	/* Get uploads */
	$result =& $db->query('SELECT * FROM '.TP.'uploads');
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$md5folder=$config['absPath']."cache/md5";
	//$md5folder=realpath("../".$md5folder);
	/* If we have uploads, display list */
	if ($result->numRows() > 0) {
		echo "\t\t\t\t";
		$bg = ' style="background: #CCC;"';
		$imgCount=0;
		while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
			$imageFilePath=$config['absPath'].'uploads\\'.$line['upFilename'];
			if(!is_file($imageFilePath)){
					copy(realpath("icons/nowallpaper.jpg"),$imageFilePath);
			}
			// Не дубликат ли???
			$conflafter="";
			$md5raw=md5_xfile($imageFilePath,$md5folder);
			$md5=$md5folder."/h-".$md5raw.".md5";
			$bg = ($bg == ' style="background: #CCC;"' ? '' : ' style="background: #CCC;"');
			if(file_exists($md5)){
				// Конфликт!!!
				$openFileMD5 = fopen($md5, "r");
				$prefile=fread($openFileMD5,filesize($md5));
				fclose($openFileMD5);
				$conflafter='</td></tr><tr><td class="adminTD"'.$bg.' colspan=10><span title="'.$prefile.'">Conflict found!!!</span></td>';
				$bg=' style="background: #FF0000;"';
			}
			$imgCount++;
			echo '<tr><td class="adminTD"'.$bg.'>'.$imgCount.".";
			echo '<a href="index.php?s=uploads&amp;a=edit&amp;image='.$line['upID'].'"><img src="icons/editimage.png" alt="Edit" title="Edit" /></a></td><td class="adminTD"'.$bg.'><a href="index.php?s=uploads&amp;a=delete&amp;submit=yes&amp;image='.$line['upID'].'"><img src="icons/deleteimage.png" alt="Delete" title="Delete" /></a></td><td class="adminTD"'.$bg.'><a href="index.php?s=uploads&amp;a=move&amp;image='.$line['upID'].'"><img src="icons/moveimage.png" alt="Add to Album" title="Add to Album" /></a>';
			echo '</td><td'.$bg.' align=center><a href="picDetail.php?image='.$line['upID'].'" target=_blank>';
			$img = $config['absPath'].$config['uploadsPath'].$line['upFilename'];
			echo '<br><img src="/wgallery/phpThumb.php?w=100&f=jpeg&src=/uploads/'.$line['upFilename'].'" width=100>';
			echo $line['upName'];
			echo '</a></td>';
			echo '<td'.$bg.' class="snapsNotes" align=center>'.$line['upDesc'];
			$i_imagesize = getimagesize ($imageFilePath);
			echo "&nbsp;(".$i_imagesize[0]."x".$i_imagesize[1].")";
			echo $before;
			echo $line['upID'];
			echo $after;
			echo $conflafter;
			echo '</td></tr>'."\n\t\t\t\t";
			if($imgCount>=20){
				echo '<tr><td colspan="5" class="adminTD">More images (total='.$result->numRows().') found but not displayed. Dispatch images above first!</td></tr>';
				break;
			}
		}
	} else {
		/* Otherwise, display error message */
		echo '<tr><td colspan="5" class="adminTD">There are no uploads.</td></tr>';
	}
	echo '<tr><td colspan="5" class="adminTD"><hr><a href="index.php?s=uploads&a=ex">Check extern uploads</a>, ';
	{
			$count =0;
			$extcatalog=$config['absPath']."uploads\\extern\\";
			//print $extcatalog;print "<hr>";
			$dir_handle=opendir($extcatalog); 
			while($file=readdir($dir_handle)) 
  			{ if($file!="." && $file!=".."){  $count++; }}
			closedir($dir_handle); 
			print $count." image(s) waiting for approval";
	}
	echo '</td></tr></table>';
} else {
	/* Otherwise, handle the action */
	switch($_GET['a']) {
		case 'edit' :
			/* If the form has not been submitted */
			if (empty($_POST['submit'])) {
				/* Get the upload's information, display edit form */
				$result =& $db->query('SELECT * FROM '.TP.'uploads WHERE upID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$out = "\t\t\t\t\t".'<form action="index.php?s=uploads&amp;a=edit&amp;image='.$_GET['image'].'" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Edit Uploaded Image</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Image Name:</td><td><input type="text" size="30" name="upName" value="'.$line['upName'].'" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right; vertical-align: top;">Image Description:</td><td><textarea cols="50" rows="5" name="upDesc">'.$line['upDesc'].'</textarea></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="edit"><img src="icons/addok.png" alt="Edit" title="Edit" /> Edit</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, update the image, print messages */
				$result =& $db->query('UPDATE '.TP.'uploads SET upName = "'.mysql_escape_string($_POST['upName']).'", upDesc = "'.mysql_escape_string($_POST['upDesc']).'" WHERE upID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'Edit Successful!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
				} else {
					$err = 'Edit Failed!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
				}
			}
			break;
		case 'ex' :
			{
				$etext="";
				$dirsource=$config['absPath']."uploads/extern";
				$thiscatalog=$config['absPath']."uploads";
				if(is_dir($dirsource))
				  	  $dir_handle=opendir($dirsource); 
				 $c=0;$ic=0;$d=0;
				 while($file=readdir($dir_handle)) 
				 { 
					    if($file!="." && $file!="..") 
					    { 
					      if (stristr($file,".jpg")!=false || stristr($file,".jpeg")!=false || stristr($file,".gif")!=false || stristr($file,".png")!=false){
					      if(!is_dir($dirsource."/".$file))
					      	{
					      	$ic++;
					      	$uploadfile=$dirsource."/".$file;
					      	$thisupl=$thiscatalog."/".$file;
					      	if(is_file($thisupl)){
								$file = mt_rand().'_'.microtime().'_'.$file;
								$thisupl=$thiscatalog."/".$file;
							}
					      	rename($uploadfile,$thisupl);
					      	$f_dsc="";
					      	$f_name=$file;
					      	$f_nameu=$file;
					      	$f_email="support@wiredplane.com";
					      	$f_names=$file;
					      	$res='INSERT INTO '.TP.'uploads (upID, upSubName, upSubEmail, upName, upDesc, upFilename, upCreated) VALUES ("", "'.mysql_escape_string($f_names).'", "'.mysql_escape_string($f_email).'", "'.mysql_escape_string($f_nameu).'", "'.mysql_escape_string($f_dsc).'", "'.mysql_escape_string(basename($f_name)).'", UNIX_TIMESTAMP())';
							$result =& $db->query($res);
							if (DB::isError($result)) {
								die($result->getMessage());
							}
							if ($db->affectedRows() > 0) {
									$etext.="<br>Image copy ok: ".$file;
									$c++;
								} else {
									$etext.="<br>Image copy ERROR: ".$file;
								}
					      	}
					      }else{
					    	unlink($file);
					    	$d++;
					      } 
					    }
				} 
				closedir($dir_handle); 
				if($ic==0){
					$etext="No images found";
				}
				echo $etext;
				print "<hr>Successfully added:". $c;
				if($d>0){
					print "<hr>Trash deleted:". $d;
				}
				break;
			}
		case 'move' :
			if (!empty($_POST['toAlbum']) && $_POST['toAlbum']=='-1'){
				// Moving to delete
			}else{
				/* If the form has not been submitted */
				if (empty($_POST['submit']) || empty($_POST['toAlbum'])) {
					/* Get the image's information and list of albums, display move/add to album form */
					$result =& $db->query('SELECT * FROM '.TP.'uploads WHERE upID = '.$_GET['image']);
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
					$result =& $db->query('SELECT * FROM '.TP.'albums');
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					$out = "\t\t\t\t\t".'<form action="index.php?s=uploads&amp;a=move&amp;image='.$_GET['image'].'" method="post">'."\n";
					$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Add Image ('.$line['upName'].') to Album</h4></td></tr>';
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Add To:</td><td><select name="toAlbum">';
					while ($ln =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
						$out .= '<option value="'.$ln['albumID'].'">'.$ln['albumName'].'</option>';
					}
					$out .= '</select></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="move"><img src="icons/addok.png" alt="Add" title="Add" /> Add</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'</form>'."\n";
				} else {
					/* Otherwise, get image's information, update album image count, add the image to database, move physical image, delete from uploads table, print messages */
					$result =& $db->query('SELECT * FROM '.TP.'uploads WHERE upID = '.$_GET['image']);
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
					if($line['upFilename']==""){
						$line['upFilename']=mt_rand().'_'.microtime().'_misc';
					}
					$src_img = $config['absPath'].$config['uploadsPath'].$line['upFilename'];
					$dst_img = $config['absPath'].$config['albumsPath'].$_POST['toAlbum'].'/'.$line['upFilename'];
					if(is_file($dst_img)){
						$line['upFilename'] = mt_rand().'_'.microtime().'_'.$line['upFilename'];
						$dst_img = $config['absPath'].$config['albumsPath'].$_POST['toAlbum'].'/'.$line['upFilename'];
					}
					$result =& $db->query('UPDATE '.TP.'albums SET albumCount = albumCount+1, albumModified = UNIX_TIMESTAMP() WHERE albumID = '.$_POST['toAlbum']);
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					$result =& $db->query('INSERT INTO '.TP.'images (`imageID`, `albumID`, `imageName`, `imageDesc`, `imageFilename`, `imageSubName`, `imageSubEmail`, `imageCreated`, `imageModified`, `imageViews`) VALUES ("", "'.$_POST['toAlbum'].'", "'.$line['upName'].'", "'.$line['upDesc'].'", "'.$line['upFilename'].'", "'.$line['upSubName'].'", "'.$line['upSubEmail'].'", "'.$line['upCreated'].'", UNIX_TIMESTAMP(), 0)');
					if (DB::isError($result)) {
						die($result->getMessage());
					}
					if ($db->affectedRows() > 0) {
						if (rename($src_img, $dst_img)) {
							$result =& $db->query('DELETE FROM '.TP.'uploads WHERE upID = '.$_GET['image']);
							if (DB::isError($result)) {
								die($result->getMessage());
							}
							$err = 'Successfully added image.';
							if (empty($_GET['confirm'])) {
								echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
							}else{
								echo $err;
								echo '<script type="text/javascript">window.close();</script>';
							}
							// Вписываем мд5
							$md5folder=$config['absPath']."cache/md5";
							$md5raw=md5_xfile($dst_img,$md5folder);
							$md5=$md5folder."/h-".$md5raw.".md5";
							$openFileMD5 = fopen($md5, "w");
							fwrite($openFileMD5,$dst_img);
							fclose($openFileMD5);
						} else {
							$err = 'Added image to database, but physical move failed. Please move "'.$src_img.'" to "'.$dst_img.'".';
							echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
						}
					} else {
						$err = 'Add failed. Please make sure the albums directories are chmod 755 or 777.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
					}
				}
				break;
			};
		case 'delete' :
			/* If the form has not been submitted */
			if (empty($_POST['submit'])) {
				/* Get the image's information */
				$result =& $db->query('SELECT * FROM '.TP.'uploads WHERE upID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$img = $config['absPath'].$config['uploadsPath'].$line['upFilename'];
				/* If the file exists, display the confirmation form */
				if (file_exists($img)) {
					$out = "\t\t\t\t\t".'<form action="index.php?s=uploads&amp;a=delete&amp;image='.$_GET['image'].'" method="post">'."\n";
					$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Delete Uploaded Image ('.$line['upName'].')</h4></td></tr>';
					$out .= "\t\t\t\t\t".'<tr><td colspan="2">Are you sure you want to delete: <span style="font-weight: bold;">'.$line['upName'].'</span>?</td></tr>'."\n";
					$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="delete"><img src="icons/addok.png" alt="Delete" title="Delete" /> Delete</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
					$out .= "\t\t\t\t\t".'</form>'."\n";
				} else {
					/* Otherwise, print error message */
					$result =& $db->query('DELETE FROM snaps_uploads WHERE upID = '.$_GET['image']);
					$err = 'File does not exist!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
				}
			} else {
				/* Otherwise, delete the image fomr the database, and physical file from uploads folder, print messages */
				$result =& $db->query('SELECT * FROM '.TP.'uploads WHERE upID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$img = $config['absPath'].$config['uploadsPath'].$line['upFilename'];
				$result =& $db->query('DELETE FROM snaps_uploads WHERE upID = '.$_GET['image']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					if (unlink($img)) {
						$err = 'Image Deleted.';
						//echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
						if (empty($_GET['confirm'])) {
							echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
						}else{
							echo $err;
							echo '<script type="text/javascript">window.close();</script>';
						}
					} else {
						$err = 'Image deleted from database, but could not delete image file. Please remove "'.$img.'" manually.';
						echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
					}
				} else {
					$err = 'Image could not be deleted.';
					echo '<script type="text/javascript">document.location.href = "index.php?s=uploads&err='.$err.'";</script>';
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