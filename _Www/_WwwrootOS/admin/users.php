<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/* Get error messages, if set */
if (!empty($_GET['err'])) {
	$err = $_GET['err'];
} else {
	$err = '';
}
?>
			<h3><img src="icons/users.png" alt="Users" title="Users" style="vertical-align: middle;" /> Users</h3>
			<?php if (!empty($err)) { echo "\n".'<div class="box">'.$err.'</div><br />'."\n"; } ?>
			<div class="box">
				<script type="text/javascript">
					function cancel() {
						document.location.href = '<?php echo $_SERVER['PHP_SELF']; ?>?s=users';
					}
					function verifyPass() {
						p1 = document.getElementById('password').value;
						p2 = document.getElementById('password2').value;
						if (p1 == p2) {
							return true;
						} else {
							alert("Passwords do not match. Please correct this and try again.");
							return false;
						}
					}
				</script>
				<table cellpadding="2" cellspacing="0" border="0" style="width: 100%;">
<?php
/* If we don't have an action, or a user */
if (empty($_GET['a']) && empty($_GET['user'])) {
?>
					<tr><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Edit</td><td class="adminTD" style="background: #009; color: #FFF; font-weight: bold; width: 10%;">Delete</td><td style="background: #009; color: #FFF; font-weight: bold; width: 80%;">User's Name</td></tr>
<?php
	/* Get and display users - note: we always have at least 1 user (from the installation process) */
	$result =& $db->query('SELECT * FROM '.TP.'users');
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	echo "\t\t\t\t";
	$bg = ' style="background: #CCC;"';
	while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
		$bg = ($bg == ' style="background: #CCC;"' ? '' : ' style="background: #CCC;"');
		echo "\t".'<tr><td class="adminTD"'.$bg.'><a href="index.php?s=users&amp;a=edit&amp;user='.$line['userID'].'"><img src="icons/edituser.png" alt="Edit" title="Edit" /></a></td><td class="adminTD"'.$bg.'><a href="index.php?s=users&amp;a=delete&amp;user='.$line['userID'].'"><img src="icons/deleteuser.png" alt="Delete" title="Delete" /></a></td><td'.$bg.'>'.$line['userFname'].' '.$line['userLname'].'</tr>'."\n\t\t\t\t";
	}
?>
</table>
				<br /><span style="padding: 5px; font-weight: bold;"><a href="index.php?s=users&amp;a=new"><img style="vertical-align: bottom;" src="icons/newuser.png" alt="New User" title="New User" /></a> Add New User</span>
<?php
} else {
	/* Otherwise, handle the action */
	switch($_GET['a']) {
		case 'edit' :
			/* If the form has not been submitted */
			if (empty($_POST['submit'])) {
				/* Get the user's information, and display the edit form */
				$result =& $db->query('SELECT * FROM '.TP.'users WHERE userID = '.$_GET['user']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$out = "\t\t\t\t\t".'<form action="index.php?s=users&amp;a=edit&amp;user='.$_GET['user'].'" method="post" onsubmit="return verifyPass();">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Edit User</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">First Name:</td><td><input type="text" size="30" name="userFname" value="'.$line['userFname'].'" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Last Name:</td><td><input type="text" size="30" name="userLname" value="'.$line['userLname'].'" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">E-Mail:</td><td><input type="text" size="30" name="userEmail" value="'.$line['userEmail'].'" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Username:</td><td><input type="text" size="30" name="username" value="'.$line['username'].'" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Password:</td><td><input type="password" size="10" id="password" name="password" value="" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Verify:</td><td><input type="password" size="10" id="password2" name="password2" value="" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="edit"><img src="icons/addok.png" alt="Edit" title="Edit" /> Edit</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, check for password change and use appropriate query, print messages */
				if (empty($_POST['password']) && empty($_POST['password2'])) {
					$result =& $db->query('UPDATE '.TP.'users SET userFname = "'.mysql_escape_string($_POST['userFname']).'", userLname = "'.mysql_escape_string($_POST['userLname']).'", userEmail = "'.mysql_escape_string($_POST['userEmail']).'", username = "'.mysql_escape_string($_POST['username']).'" WHERE userID = '.$_GET['user']);
				} else {
					$result =& $db->query('UPDATE '.TP.'users SET userFname = "'.mysql_escape_string($_POST['userFname']).'", userLname = "'.mysql_escape_string($_POST['userLname']).'", userEmail = "'.mysql_escape_string($_POST['userEmail']).'", username = "'.mysql_escape_string($_POST['username']).'", userpass = "'.md5($_POST['password']).'" WHERE userID = '.$_GET['user']);
				}
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'Edit Successful!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=users&err='.$err.'";</script>';
				} else {
					$err = 'Edit Failed!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=users&err='.$err.'";</script>';
				}
			}
			break;
		case 'new' :
			/* If the form has not been submitted, print add user form */
			if (empty($_POST['submit'])) {
				$out = "\t\t\t\t\t".'<form action="index.php?s=users&amp;a=new" method="post" onsubmit="return verifyPass();">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Create New User</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">First Name:</td><td><input type="text" size="30" name="userFname" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Last Name:</td><td><input type="text" size="30" name="userLname" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">E-Mail:</td><td><input type="text" size="30" name="userEmail" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Username:</td><td><input type="text" size="30" name="username" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Password:</td><td><input type="password" size="10" id="password" name="password" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;">Verify:</td><td><input type="password" size="10" id="password2" name="password2" /></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="new"><img src="icons/addok.png" alt="Create" title="Create" /> Create</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, add the user, print messages */
				$result =& $db->query('INSERT INTO '.TP.'users (userID, userFname, userLname, userEmail, username, userpass) VALUES ("", "'.mysql_escape_string($_POST['userFname']).'", "'.mysql_escape_string($_POST['userLname']).'", "'.mysql_escape_string($_POST['userEmail']).'", "'.mysql_escape_string($_POST['username']).'", "'.md5($_POST['password']).'")');
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'User Created!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=users&err='.$err.'";</script>';
				} else {
					$err = 'User Creation Failed!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=users&err='.$err.'";</script>';
				}
			}
			break;
		case 'delete' :
			/* If the form has not been submitted */
			if (empty($_POST['submit'])) {
				/* Get user's information, and print confirmation form */
				$result =& $db->query('SELECT * FROM '.TP.'users WHERE userID = '.$_GET['user']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
				$out = "\t\t\t\t\t".'<form action="index.php?s=users&amp;a=delete&amp;user='.$_GET['user'].'" method="post">'."\n";
				$out .= "\t\t\t\t\t".'<tr><td colspan="2"><h4 style="margin-top: 0;">Delete User</h4></td></tr>';
				$out .= "\t\t\t\t\t".'<tr><td colspan="2">Are you sure you want to delete: <span style="font-weight: bold;">'.$line['userFname'].' '.$line['userLname'].'</span>?</td></tr>'."\n";
				$out .= "\t\t\t\t\t".'<tr><td style="text-align: right;"><button type="submit" name="submit" value="delete"><img src="icons/addok.png" alt="Delete" title="Delete" /> Delete</button></td><td><button type="reset" onclick="javascript: cancel();"><img src="icons/cancel.png" alt="Cancel" title="Cancel" /> Cancel</button></td></tr>'."\n";
				$out .= "\t\t\t\t\t".'</form>'."\n";
			} else {
				/* Otherwise, delete the user and print messages */
				$result =& $db->query('DELETE FROM '.TP.'users WHERE userID = '.$_GET['user']);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$err = 'User Deleted!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=users&err='.$err.'";</script>';
				} else {
					$err = 'User Deletion Failed!';
					echo '<script type="text/javascript">document.location.href = "index.php?s=users&err='.$err.'";</script>';
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