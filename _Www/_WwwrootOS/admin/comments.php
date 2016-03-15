<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/* Get error messages, if set */
if (!empty($_GET['err'])) {
	$err = $_GET['err'];
} else {
	$err = '';
}
?>
			<h3><img src="icons/comments.png" alt="Comments" title="Comments" style="vertical-align: middle;" /> Comments</h3>
			<?php if (!empty($err)) { echo "\n".'<div class="box">'.$err.'</div><br />'."\n"; } ?>
			<div class="box">
				<script type="text/javascript">
					function cancel() {
						document.location.href = '<?php echo $_SERVER['PHP_SELF']; ?>?s=comments';
					}
					function comment(id) {
						window.open('cmtDetail.php?comment='+id, 'cmtDetail', 'width=330,height=240,scrollbars=yes,toolbar=no,location=no,directories=no,status=no,menubar=no');
					}
				</script>
				<table cellpadding="2" cellspacing="0" border="0" style="width: 100%;">
<?php
/* If we have no action, and no comment ID */
if (empty($_GET['a']) && empty($_GET['comment'])) {
?>
					<tr><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Delete</td><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Move</td><td style="background: #009; color: #FFF; font-weight: bold; width: 80%;">Comment</td></tr>
<?php
	/* Get the comments */
	$result =& $db->query('SELECT * FROM '.TP.'comments');
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	/* If we have comments, print them */
	if ($result->numRows() > 0) {
		echo "\t\t\t\t";
		$bg = ' style="background: #CCC;"';
		while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
			$bg = ($bg == ' style="background: #CCC;"' ? '' : ' style="background: #CCC;"');
			echo "\t".'<tr><td class="adminTD"'.$bg.'><a href="index.php?s=comments&amp;a=delete&amp;comment='.$line['commentID'].'"><img src="icons/deletecomments.png" alt="Delete" title="Delete" /></a></td><td class="adminTD"'.$bg.'><a href="index.php?s=comments&amp;a=move&amp;comment='.$line['commentID'].'"><img src="icons/movecomments.png" alt="Move" title="Move" /></a></td><td'.$bg.'>'.substr($line['commentBody'], 0, 50).'... <a href="javascript:comment('.$line['commentID'].');">More</a></td></tr>'."\n\t\t\t\t";
		}
	} else {
		/* Otherwise, print an error message */
		echo '<tr><td colspan="3" class="adminTD">There are no comments.</td></tr>';
	}
?>
</table>
<?php
} else {
	/* Otherwise, handle the action */
	switch($_GET['a']) {
		case 'delete' :
			/* If the form is not submitted */
			if (empty($_POST['submit'])) {
				/* Get the comment's information and print the confirmation form */
				$result =& $db->query('SELECT * FROM '.TP.'comments WHERE commentID = '.$_GET['comment']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$rslt =& $db->query('SELECT imageName FROM '.TP.'images WHERE imageID = '.$line['imageID']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$ln =& $rslt->fetchRow(DB_FETCHMODE_ASSOC);
				$out = "\t\t\t\t\t".'<form action="index.php?s=comments&amp;a=delete&amp;comment='.$_GET['comment'].'" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Delete Comment</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td colspan="2">Are you sure you want to delete this comment?<br /><br /><div style="margin: 0 20px; padding: 3px; border: 1px solid #666; background: #FFF; font-size: 11px;">Comment for: '.$ln['imageName'].'<br />'.$line['commentBody'].'<br />by: '.$line['commentName'].'</div><br /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="delete"><img src="icons/addok.png" alt="Delete" title="Delete" /> Delete</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, delete the comment and print messages */
				$result =& $db->query('DELETE FROM '.TP.'comments WHERE commentID = '.$_GET['comment']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'Comment Deleted.';
					echo '<script type="text/javascript">document.location.href = "index.php?s=comments&err='.$err.'";</script>';
				} else {
					$err = 'Comment Deletion Failed';
					echo '<script type="text/javascript">document.location.href = "index.php?s=comments&err='.$err.'";</script>';
				}
			}
			break;
		case 'move' :
			/* If the form is not submitted */
			if (empty($_POST['submit'])) {
				/* Get the comment's name and associated image and list of images, print form */
				$result =& $db->query('SELECT commentName, imageID FROM '.TP.'comments WHERE commentID = '.$_GET['comment']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$result =& $db->query('SELECT * FROM '.TP.'images WHERE imageID = '.$line['imageID']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$ln =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$curImage = $ln['imageName'];
				$result =& $db->query('SELECT * FROM '.TP.'images');
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$out = "\t\t\t\t\t".'<form action="index.php?s=comments&amp;a=move&amp;comment='.$_GET['comment'].'" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Move Comment</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Move From:</td><td>'.$curImage.'</td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Move To:</td><td><select name="toImage">';
				while ($ln =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
					$out .= '<option value="'.$ln['imageID'].'">'.$ln['imageName'].'</option>';
				}
				$out .= '</select></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="move"><img src="icons/addok.png" alt="Move" title="Move" /> Move</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, update the database, print messages */
				$result =& $db->query('UPDATE '.TP.'comments SET imageID = '.$_POST['toImage'].' WHERE commentID = '.$_GET['comment']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'Comment Moved!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=comments&err='.$err.'";</script>';
				} else {
					$err = 'Comment Move Failed.';
					echo '<script type="text/javascript">document.location.href = "index.php?s=comments&err='.$err.'";</script>';
				}
			}
			break;
		default :
			break;
	}
	echo $out."\t\t\t\t".'</table>'."\n";
}
?>			</div>