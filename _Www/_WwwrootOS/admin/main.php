			<h3><img src="icons/stats.png" alt="Statistics" title="Statistics" style="vertical-align: middle;" /> Statistics</h3>
			<div class="box">
				<table cellpadding="3" cellspacing="0" border="0" style="width: 100%;">
					<tr><td style="text-align: right;">Albums:</td><td>
<?php
/* Get number of albums */
$result =& $db->query('SELECT COUNT(*) FROM '.TP.'albums');
if (DB::isError($result)) {
	die($result->getMessage());
}
$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
echo $line['COUNT(*)'].'</td></tr>';
?>
					<tr><td style="background: #CCC; text-align: right;">Images:</td><td style="background: #CCC;">
<?php
/* Get number of images */
$result =& $db->query('SELECT COUNT(*) FROM '.TP.'images');
if (DB::isError($result)) {
	die($result->getMessage());
}
$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
echo $line['COUNT(*)'].'</td></tr>';
?>
					<tr><td style="text-align: right;">Users:</td><td>
<?php
/* Get number of administrators */
$result =& $db->query('SELECT COUNT(*) FROM '.TP.'users');
if (DB::isError($result)) {
	die($result->getMessage());
}
$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
echo $line['COUNT(*)'].'</td></tr>';
?>
					<tr><td style="background: #CCC; text-align: right;">Comments:</td><td style="background: #CCC;">
<?php
/* Get number of comments */
$result =& $db->query('SELECT COUNT(*) FROM '.TP.'comments');
if (DB::isError($result)) {
	die($result->getMessage());
}
$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
echo $line['COUNT(*)'].'</td></tr>';
?>
					<tr><td style="text-align: right;">Most Viewed:</td><td>
<?php
/* Get information for most viewed image */
$result =& $db->query('SELECT * FROM '.TP.'images ORDER BY imageViews DESC');
if (DB::isError($result)) {
	die($result->getMessage());
}
$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
echo $line['imageName'].' ('.$line['imageFilename'].') ['.$line['imageViews'].' views]</td></tr>';
?>
			</table>
		</div><br />
		<h3><img src="icons/utilities.png" alt="Utilities" title="Utilities" style="vertical-align: middle;" /> Utilities</h3>
		<div class="box">
			<a href="index.php?action=optimize">Optimize Database Tables</a> - <span class="snapsNotes">Recommended: Perform this at least once per week.</span>
		</div>
		<div class="box">
			<a href="index.php?action=recreatemd5">Recreate MD5</a> - <span class="snapsNotes">To keep off duplications.</span>
		</div>
		<div class="box">
			<a href="index.php?s=uploads&a=ex"><img src="icons/uploads.png" alt="Uploads" title="Uploads" style="vertical-align: middle;" /> Uploads - Extern</a>:
			<?php 
			$count =0;
			$extcatalog=$config['absPath']."uploads\\extern\\";
			//print $extcatalog;print "<hr>";
			$dir_handle=opendir($extcatalog); 
			while($file=readdir($dir_handle)) 
  			{ if($file!="." && $file!=".."){  $count++; }}
			closedir($dir_handle); 
			print $count." image(s) waiting for approval";
			?>
		</div>
