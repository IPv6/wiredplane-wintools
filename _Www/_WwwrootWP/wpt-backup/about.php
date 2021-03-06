<?php 
function getTitle($l)
{
	global $language;
	return "WPBackup - Snapshot-based backup utility";
}
function insertSection($l){
?>
<div id=h1>WPBackup</div>
Command line tool for backuping folders from your hard drive. 
Unlike other programs in this field, WPBackup uses folder snapshot to detect changed 
files (using CRC of every file in the folder). In other words, WPBackup will archive 
only changed files regardless to file modification date (though this option available too). 
When updating from backup, WPBackup can automatically remove deleted files.
<br><br>
This program is totally FREE for non-commersial use and does not contain adware or spyware of any kind.<br>But you can donate us if you find it helpful.
For commersial usage you should obtain <a href="http://www.wiredplane.com/guestbook/vendors.php?who=comuse&vendor=2&src=site">license</a>
<br>
<center><br>
<script type="text/javascript"><!--
google_ad_client = "pub-5186671154037974";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text_image";
google_ad_channel = "";
google_color_border = "000000";
google_color_bg = "EEEEEE";
google_color_link = "0000FF";
google_color_text = "000000";
google_color_url = "008000";
//-->
</script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</center>
<div width=100% align=center><a href="/download/wpbackup.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a></div>
<pre>
wpbackup [command] [options] -f=[from folder/archive] -t=[to folder/archive]

Usage examples:
Backup current folder into backup.zip:    wpbackup -b -f=. -t=backup.zip
Unpack backup.zip into current folder:    wpbackup -u -f=backup.zip -t=.
Update snapshot of the 'C:\PROJECTS':    wpbackup -r -f="C:\PROJECTS"
List changed files in the 'C:\PROJECTS': wpbackup -c -f="C:\PROJECTS"

Commands:
---------
-b	Make backup of the folder (pack changed files into single zip archive)
-u	Unpack backup file into the folder and delete removed files/folders.
-r	Refresh snapshot of the folder ('folder_snapshot.info' file by default)
-c	List new/changed/deleted files

Options
---------

-f=[Folder/file path]
Folder (for 'b','r' and 'c' commands) or backup archive (for 'u' command)
You can use '.' to get current folder/autogenerated archive name
Obligatory parameter, no default value

-t=[Folder/file path]
Archive name (for 'b' command) or source folder (for 'u' command)
You can use '.' to get current folder/most recent archive file
Obligatory parameter for 'b' and 'u' commands, no default value

-e=[List of patterns]
';'-Separated list of patterns. Only matched files will be
involved into pack/unpack operations. Example: -e=*.h;*.cpp;*.html
Default is '*.*'

-x=[List of exclusions]
';'-Separated list of exclusions. Matched files will NOT be
involved into pack/unpack operations. Example: -x=backup_*.zip
No default value

-p=[Archive password]
Backup archive password

-m=[c/d/cd/a]
How to determine that file was changed. 'cd' - Compare file CRC AND file 
modification time, 'c' - Compare CRC only (file modification time is ignored),
'd' - Compare file modification time (CRC is ignored).
'a' - all files will be treated as changed (full backup will be created)
Default is 'c'

-dr=[y/n]
Delete removed files/folders after unpacking backup archive.
Use with 'u' command only
Default is 'y'

-sf=[Snapshot file]
This file is used to hold snapshot data.
Using this option, you can work with different snapshots of the same folder.
In other cases you can omit it. Default is 'folder_snapshot.info'

-us=[y/n]
Update folder snapshot after all.
Default is 'y'(yes)

-rf=[File name]
Name of the revision counter file. Will be created/updated 
in each folder with changed files

Hints
---------
To load option value from text file (by default, values are retrieved 
directly from command line), enter file name prefixed with '@' after '=' character.
 Example: -x=@exclusions.txt
</pre>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
