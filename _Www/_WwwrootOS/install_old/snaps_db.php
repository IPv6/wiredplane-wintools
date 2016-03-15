<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
$AlbumsSQL = "CREATE TABLE `{$_POST['tblPrefix']}albums` (
  `albumID` int(11) NOT NULL auto_increment,
  `albumName` varchar(255) NOT NULL default '',
  `albumDesc` text NOT NULL,
  `albumCount` int(11) NOT NULL default '0',
  `albumCreated` int(11) NOT NULL default '0',
  `albumModified` int(11) NOT NULL default '0',
  PRIMARY KEY  (`albumID`)
) TYPE=MyISAM";

$CommentsSQL = "CREATE TABLE `{$_POST['tblPrefix']}comments` (
  `commentID` int(11) NOT NULL auto_increment,
  `imageID` int(11) NOT NULL default '0',
  `commentName` varchar(255) NOT NULL default '',
  `commentEmail` varchar(255) NOT NULL default '',
  `commentBody` text NOT NULL,
  `commentCreated` int(11) NOT NULL default '0',
  PRIMARY KEY  (`commentID`)
) TYPE=MyISAM";

$ConfigSQL = "CREATE TABLE `{$_POST['tblPrefix']}config` (
  `var` varchar(255) NOT NULL default '',
  `val` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`var`)
) TYPE=MyISAM";

$ConfigVarsSQL = array(	0 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('version', '1.4.4')",
											1 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('absPath', '{$_SESSION['configVars']['absPath']}')",
											2 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('albumsPath', '{$_SESSION['configVars']['albumsPath']}')",
											3 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('cachePath', '{$_SESSION['configVars']['cachePath']}')",
											4 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('uploadsPath', '{$_SESSION['configVars']['uploadsPath']}')",
											5 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('snapsURL', '{$_SESSION['configVars']['snapsURL']}')",
											6 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('albumsPP', '{$_SESSION['configVars']['albumsPP']}')",
											7 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('imagesPP', '{$_SESSION['configVars']['imagesPP']}')",
											8 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('allowComment', '{$_SESSION['configVars']['allowComment']}')",
											9 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('allowSubmit', '{$_SESSION['configVars']['allowSubmit']}')",
											10 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('enableCache', '{$_SESSION['configVars']['enableCache']}')",
											11 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('resizeMethod', '{$_SESSION['configVars']['resizeMethod']}')",
											12 => "INSERT INTO `{$_POST['tblPrefix']}config` (`var`, `val`) VALUES ('imPath', '{$_SESSION['configVars']['imPath']}')");

$ImagesSQL = "CREATE TABLE `{$_POST['tblPrefix']}images` (
  `imageID` int(11) NOT NULL auto_increment,
  `albumID` int(11) NOT NULL default '0',
  `imageName` varchar(255) NOT NULL default '',
  `imageDesc` text NOT NULL,
  `imageFilename` varchar(255) NOT NULL default '',
  `imageSubName` varchar(255) NOT NULL default '',
  `imageSubEmail` varchar(255) NOT NULL default '',
  `imageCreated` int(11) NOT NULL default '0',
  `imageModified` int(11) NOT NULL default '0',
  `imageViews` int(11) NOT NULL default '0',
  PRIMARY KEY  (`imageID`)
) TYPE=MyISAM";

$UploadsSQL = "CREATE TABLE `{$_POST['tblPrefix']}uploads` (
  `upID` int(11) NOT NULL auto_increment,
  `upSubName` varchar(255) NOT NULL default '',
  `upSubEmail` varchar(255) NOT NULL default '',
  `upName` varchar(255) NOT NULL default '',
  `upDesc` text NOT NULL,
  `upFilename` varchar(255) NOT NULL default '',
  `upCreated` int(11) NOT NULL default '0',
  PRIMARY KEY  (`upID`)
) TYPE=MyISAM";

$UsersSQL = "CREATE TABLE `{$_POST['tblPrefix']}users` (
  `userID` int(11) NOT NULL auto_increment,
  `userFname` varchar(255) NOT NULL default '',
  `userLname` varchar(255) NOT NULL default '',
  `userEmail` varchar(255) NOT NULL default '',
  `username` varchar(255) NOT NULL default '',
  `userpass` varchar(32) NOT NULL default '',
  `userLastLogin` int(11) NOT NULL default '0',
  PRIMARY KEY  (`userID`)
) TYPE=MyISAM";
?>