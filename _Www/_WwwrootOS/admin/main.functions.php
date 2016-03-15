<?php
ini_set("include_path", 'D:\Clients\10533\outer-station.m6.net\outer-station.m6.net\wgallery\include' . PATH_SEPARATOR . ini_get("include_path"));
/*
*	File:				Snaps!.functions.php
*	Description:	Main functions file - creates crumb navigation, pagination, album list, album info, image info, and comments
*	Copyright:	©2004 The Sonic Group, LLC
*	Website:		http://labs.sonicdesign.us/projects/Snaps!/
*
*	This program is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation; either version 2 of the License, or
*	(at your option) any later version.
*/

/* Retrieve configuration information from the database */
$result =& $db->query('SELECT * FROM '.TP.'config');
if (DB::isError($result)) {
	die($result->getMessage());
}
while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
	$config[$line['var']] = $line['val'];
}

/**
* Function: crumb() - creates the breadcrumb navigation
*
* @access	public
* @var		string			$index - the index page
* @var		string/array	$album - if album is the current page, it is a string with the album's name - if an image is the current page, it is an array with the albumID and it's name
* @var		string			$imgName - the image being viewed
*/
function crumb($index, $album, $imgName) {
	global $title;
	/* If no image name */
	if (empty($imgName)) {
		/* and no album name, we are on the album list page */
		if (empty($album)) {
			$bc = 'Album List';
		/* if we have an album name, we are on the image list page */
		} else {
			$bc = '<a href="'.$_SERVER['PHP_SELF'].'">Album List</a> -&gt; '.$album;
		}
	/* otherwise, we are viewing an image */
	} else {
		$bc = '<a href="'.$_SERVER['PHP_SELF'].'">Album List</a> -&gt; <a href="'.$_SERVER['PHP_SELF'].'?album='.$album[0].'">'.$album[1].'</a> - &gt; View image';
	}
	return $bc;
}
/**
* Function: makePagination() - creates page links
*
* @access	public
* @var		string		$type - album or index
* @var		integer		$ID - the id to paginate
*/
function makePagination($type = 'album', $ID) {
	global $db, $config, $start;
	switch ($type) {
		case 'album' :
			/* Get the total number of images in this album */
			$result =& $db->query('SELECT * FROM '.TP.'images WHERE albumID ='.$ID);
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
					$out .=  '<span style="font-weight: bold;"><nobr>[ '.$i.' ]</nobr></span> ';
				/* otherwise, echo a link to the next page */
				} else {
					/* If $i is not 1 (i.e. the first page), the link is echoed with a start variable */
					if ($i != 1) {
						$out .= '<a href="'.$_SERVER['PHP_SELF'].'?album='.$ID.'&amp;start='.$config['imagesPP']*($i-1).'">'.$i.'</a> ';
					/* otherwise, the link is echoed as the first page (i.e. no start) */
					} else {
						$out .= '<a href="'.$_SERVER['PHP_SELF'].'?album='.$ID.'">'.$i.'</a> ';
					}
				}
			}
			break;
		case 'index' :
			/* Get the total number of albums in the gallery */
			$result =& $db->query('SELECT * FROM '.TP.'albums');
			if (DB::isError($result)) {
				die($result->getMessage());
			}
			$numAlbums = $result->numRows();
			$out = 'Page: ';
			/* Figure out number of pages we should have */
			$totalPages = ceil($numAlbums/$config['albumsPP']);
			/* Figure out what page we are on */
			$current = $start/$config['albumsPP']+1;
			for ($i = 1; $i <= $totalPages; $i++) {
				/* If this is the current page, echo a non-link */
				if ($i == $current) {
					$out .=  '<span style="font-weight: bold;">[ '.$i.' ]</span>&nbsp;';
				/* otherwise, echo a link to the next page */
				} else {
					/* If $i is not 1 (i.e. the first page), the link is echoed with a start variable */
					if ($i != 1) {
						$out .= '<a href="'.$_SERVER['PHP_SELF'].'?start='.$config['albumsPP']*($i-1).'">'.$i.'</a>&nbsp;';
					/* otherwise, the link is echoed as the first page (i.e. no start) */
					} else {
						$out .= '<a href="'.$_SERVER['PHP_SELF'].'">'.$i.'</a>&nbsp;';
					}
				}
			}
			break;
	}
	return $out;
}

function lastAdditions()
{
	global $db, $title, $config, $start;
	/* Get the images in the album */
	$result =& $db->query('SELECT * FROM '.TP.'images ORDER BY imageID DESC LIMIT 0,5');
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$albumImages = $result->numRows();
	/* If we have images, get their information */
	if ($albumImages > 0) {
		$i = 1;
		while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
			$image[$i][0] =  '?album='.$line['albumID'].'&amp;image='.$line['imageID'];
			$image[$i][1] = stripslashes($line['imageName']);
			if ($config['enableCache'] == 1) {
				$image[$i][2] = getImage($line['albumID'], $line['imageFilename'], $line['imageName'], 100, 'cache');
			} else {
				$image[$i][2] = getImage($line['albumID'], $line['imageFilename'], $line['imageName'], 100, 'dynamic');
			}
			$image[$i][3] = stripslashes($line['imageDesc']);
			$image[$i][4] = $line['imageCreated'];
			$image[$i][5] = $line['imageModified'];
			$image[$i][6] = $line['imageViews'];
			if ($config['allowComment'] == 1) {
				/* Get number of comments for each image */
				$rslt =& $db->query('SELECT COUNT(*) FROM '.TP.'comments WHERE imageID = '.$line['imageID']);
				if (DB::isError($rslt)) {
					die($rslt->getMessage());
				}
				$ln =& $rslt->fetchRow(DB_FETCHMODE_ASSOC);
				$image[$i][7] = ($ln['COUNT(*)'] > 1) ? $ln['COUNT(*)'].' comments' : (($ln['COUNT(*)'] == 1) ? $ln['COUNT(*)'].' comment' : 'No comments');
			}
			$i++;
		}
		/* otherwise, we have no images */
	} else {
		$image = 'There are no images in this album yet!';
	}
	return $image;
}

/**
* Function: albumList() - lists albums (index page)
*
* @access	public
*/
function albumList() {
	global $db, $start, $config;
	/* Get the number of albums */
	$result =& $db->query('SELECT * FROM '.TP.'albums LIMIT '.$start.','.$config['albumsPP']);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$numAlbums = $result->numRows();
	/* If we have albums, get their information */
	if ($numAlbums > 0) {
		$i = 1;
		while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
			$album[$i][0] =  $line['albumID'];
			$album[$i][1] = stripslashes($line['albumName']);
			$album[$i][2] = $line['albumDesc'];
			$album[$i][3] = $line['albumCount'];
			$album[$i][4] = $line['albumModified'];
			$rndnum=rand(0,$album[$i][3]-1);
			if($rndnum > 50){
				$rndnum = $rndnum%49+1;
			}
			$query2 = 'SELECT * FROM '.TP.'images WHERE albumID = '.$line['albumID'].' ORDER BY imageID DESC '.' LIMIT '.$rndnum.','.($rndnum+1);
			$result2 =& $db->query($query2);
			$numimages2 = $result2->numRows();
			if (!DB::isError($result2) && $numimages2>0) {
				$line2 =& $result2->fetchRow(DB_FETCHMODE_ASSOC);
				if ($config['enableCache'] == 1) {
					//$rndnum.', '.$query2.', '.$numimages2.'? '.
					$album[$i][6] = getImage($line2['albumID'], $line2['imageFilename'], $line2['imageName'], 100, 'cache');
				} else {
					$album[$i][6] = getImage($line2['albumID'], $line2['imageFilename'], $line2['imageName'], 100, 'dynamic');
				}
				$album[$i][5]='?album='.$line2['albumID'].'&amp;image='.$line2['imageID'];
			}else{
				$album[$i][6]="<img src='/images/album.png'>";
				$album[$i][5]="/images/album.png";
			}
			$i++;
		}
		$album[1][5] = makePagination('index', '1');

	/* otherwise, we have no albums */
	} else {
		$album = 'There are no albums yet!';
	}
	return $album;
}
/**
* Function: album() - lists images in an album
*
* @access	public
* @var		integer		$albumID - ID of album to display
*/
function album($albumID) {
	global $db, $title, $config, $start;
	/* Get the album name */
	$result =& $db->query('SELECT * FROM '.TP.'albums WHERE albumID = '.$albumID);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
	$albumTitle = stripslashes($line['albumName']);
	$result =& $db->query('SELECT * FROM '.TP.'images WHERE albumID = '.$albumID);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$numImages = $result->numRows();
	/* Get the images in the album */
	$result =& $db->query('SELECT * FROM '.TP.'images WHERE albumID = '.$albumID.' ORDER BY imageID DESC'.' LIMIT '.$start.','.$config['imagesPP']);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$albumImages = $result->numRows();
	/* If we have images, get their information */
	if ($albumImages > 0) {
		$i = 1;
		while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
			$image[$i][0] =  '?album='.$albumID.'&amp;image='.$line['imageID'];
			$image[$i][1] = stripslashes($line['imageName']);
			if ($config['enableCache'] == 1) {
				$image[$i][2] = getImage($line['albumID'], $line['imageFilename'], $line['imageName'], 100, 'cache');
			} else {
				$image[$i][2] = getImage($line['albumID'], $line['imageFilename'], $line['imageName'], 100, 'dynamic');
			}
			$image[$i][3] = stripslashes($line['imageDesc']);
			$image[$i][4] = $line['imageCreated'];
			$image[$i][5] = $line['imageModified'];
			$image[$i][6] = $line['imageViews'];
			if ($config['allowComment'] == 1) {
				/* Get number of comments for each image */
				$rslt =& $db->query('SELECT COUNT(*) FROM '.TP.'comments WHERE imageID = '.$line['imageID']);
				if (DB::isError($rslt)) {
					die($rslt->getMessage());
				}
				$ln =& $rslt->fetchRow(DB_FETCHMODE_ASSOC);
				$image[$i][7] = ($ln['COUNT(*)'] > 1) ? $ln['COUNT(*)'].' comments' : (($ln['COUNT(*)'] == 1) ? $ln['COUNT(*)'].' comment' : 'No comments');
			}
			$i++;
		}
		$image[1][9] = crumb('index', $albumTitle, '');
		if ($numImages > $config['imagesPP']) {
			$image[1][8] = makePagination('album', $albumID);
		} else {
			$image[1][8] = 'Page 1 of 1';
		}
		/* otherwise, we have no images */
	} else {
		$image = 'There are no images in this album yet!';
	}
	return $image;
}
/**
* Function: image() - retrieves information for a specific image
*
* @access	public
* @var		integer		$albumID - ID of album picture belongs to
* @var		integer		$imgID - ID of image to work with
*/
function imageraw($albumID, &$imgID, $updateviews) {
	global $db, $title, $config;
	/* Get album name */
	$result =& $db->query('SELECT * FROM '.TP.'albums WHERE albumID = '.$albumID);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
	$albumTitle[0] = $albumID;
	$albumTitle[1] = stripslashes($line['albumName']);
	if($imgID == "rnd"){
		$rndnum=rand(0,$line['albumCount']-1);
		$query2 = 'SELECT * FROM '.TP.'images WHERE albumID = '.$albumID.' LIMIT '.$rndnum.','.($rndnum+1);
		$result2 =& $db->query($query2);
		if (DB::isError($result2)) {
			die($result2->getMessage());
		}
		$line2 =& $result2->fetchRow(DB_FETCHMODE_ASSOC);
		$imgID=$line2['imageID'];
	}
	if($updateviews!=0){
		/* Update number of views for this image */
		$result =& $db->query('UPDATE '.TP.'images SET imageViews = imageViews+1 WHERE albumID = '.$albumID.' AND imageID = '.$imgID);
		if (DB::isError($result)) {
			die($result->getMessage());
		}
	}
	/* Get the image's information */
	$result =& $db->query('SELECT * FROM '.TP.'images WHERE albumID = '.$albumID.' AND imageID = '.$imgID);
	if (DB::isError($result)) {
		die($result->getMessage());
	}
	$i = 1;
	while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
		$image[$i][0] = stripslashes($line['imageName']);
		$image[$i][1] = $line['imageFilename'];
		$image[$i][2] = stripslashes($line['imageDesc']);
		$image[$i][3] = $line['imageCreated'];
		$image[$i][4] = $line['imageModified'];
		$image[$i][5] = $line['albumID'];
		if ($config['enableCache'] == 1) {
			$image[$i][6] = getImage($line['albumID'], $line['imageFilename'], $line['imageName'], 600, 'cache');
		} else {
			$image[$i][6] = getImage($line['albumID'], $line['imageFilename'], $line['imageName'], 600, 'dynamic');
		}
		$i++;
	}
	$image[1][7] = crumb('index', $albumTitle, $image[1][0]);
	$image[1][8] = $imgID;
	return $image;
}

function image($albumID, &$imgID) {
	return imageraw($albumID, $imgID, 1);
}
/**
* Function: comment() - view, create, and delete/moderate comments - called from image()
*
* @access	private
* @var		string		$action - view or create - defaults to view
* @var		integer		$imgID - ID of image to view or add comments for
* @var		integer		$albID - ID of album image belongs to
*/
function comment($action = 'view', $imgID, $albID) {
	global $db, $title, $config;
	if ($config['allowComment'] == 1) {
		switch($action) {
			default :
			case 'view' :
				/* Get all the comments for this image */
				$result =& $db->query('SELECT * FROM '.TP.'comments WHERE imageID = '.$imgID);
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				/* Get number of comments */
				$numComments = $result->numRows();
				/* If we have comments, get their information */
				if ($numComments > 0) {
					$i = 1;
					while ($line =& $result->fetchRow(DB_FETCHMODE_ASSOC)) {
						$comment[$i][0] = $line['commentName'];
						$comment[$i][1] = emailOB($line['commentEmail']);
						$comment[$i][2] = stripslashes($line['commentBody']);
						$comment[$i][3] = $line['commentCreated'];
						$comment[$i][4] = $imgID;
						$i++;
					}
				/* otherwise, there are no comments - assemble the submission form */
				} else {
					$comment = 'There are no comments for this image.<br />';
				}
				break;
			case 'create' :
				$result =& $db->query("INSERT INTO `".TP."comments` ( `commentID` , `imageID` , `commentName` , `commentEmail` , `commentBody` , `commentCreated` ) VALUES ('', '".$imgID."', '".mysql_escape_string($_POST['commentName'])."', '".mysql_escape_string($_POST['commentEmail'])."', '".mysql_escape_string($_POST['commentBody'])."', UNIX_TIMESTAMP())");
				if (DB::isError($result)) {
					die($result->getMessage());
				}
				if ($db->affectedRows() > 0) {
					$comment[0] = 'success';
					$comment[1] = 'Thank you for your comment.<br /><br /><a href="'.$_SERVER['PHP_SELF'].'?album='.$albID.'&amp;image='.$imgID.'">Back</a>';
				} else {
					$comment[0] = 'error';
					$comment[1] = 'There was a problem adding your comment.<br /><br /><a href="'.$_SERVER['PHP_SELF'].'?album='.$albID.'&amp;image='.$imgID.'">Back</a>';
				}
				break;
		}
	} else {
		$comment = 'Comments have been disabled.';
	}
	return $comment;
}
/**
* Function: getImage() - returns image as HTML <img> tag
*
* @access	private
* @var		integer		$albumID - ID of album picture belongs to
* @var		string		$imgFilename - image filename to work with
* @var		string		$imgName - name of image
* @var		integer		$size - thumbnail size
* @var		string		$mode - image generation mode (cache or dynamic)
*/
function getImage($albumID, $imgFilename, $imgName, $size, $mode) {
	global $db, $config;
	if ($config['resizeMethod'] == 'im') {
		$outPath = $config['absPath'].$config['cachePath'];
		$outFilename = md5($imgFilename.'-'.$size).'.jpg';
		$mode = 'cache';
		if (file_exists($outPath.$outFilename)) {
			return '<img width=100 src="'.$config['cachePath'].$outFilename.'" alt="'.$imgName.'" title="'.$imgName.'" />';
		} else {
			$filename = $config['absPath'].$config['albumsPath'].$albumID.'/'.$imgFilename;
			include('image.functions.php');
			return '<img width=100 src="'.$config['cachePath'].$outFilename.'" alt="'.$imgName.'" title="'.$imgName.'" />';
		}
	} else {
		switch($mode) {
			case 'cache' :
				$outPath = $config['absPath'].$config['cachePath'];
				$outFilename = md5($imgFilename.'-'.$size).'.jpg';
				if (file_exists($outPath.$outFilename)) {
					return '<img width=100 src="'.$config['cachePath'].$outFilename.'" alt="'.$imgName.'" title="'.$imgName.'" />';
				} else {
					$filename = $config['absPath'].$config['albumsPath'].$albumID.'/'.$imgFilename;
					include('image.functions.php');
					return '<img width=100 src="'.$config['cachePath'].$outFilename.'" alt="'.$imgName.'" title="'.$imgName.'" />';
				}
				break;
			case 'dynamic' :
				$filename = $config['albumsPath'].$albumID.'/'.$imgFilename;
				return '<img src="/admin/image.functions.php?filename='.$filename.'&amp;size='.$size.'" alt="'.$imgName.'" title="'.$imgName.'" />';
				break;
		}
	}
}
/**
* Function: emailOB() - obfuscate an email address
*
* @access	private
* @var		string		$email - email address to obfuscate
*/
function emailOB($email) {
	// store obfuscated email
	$obbed = '';
	//Clean whitespace
	$email = trim($email);
	//Get length
	$len = strlen($email);
	//Loop through the characters, converting them
	for ($i = 0; $i < $len; $i++) {
		$obbed .= "&#".ord($email{$i}).";";
	}
	//Return the obfuscated e-mail address
	return $obbed;
}
?>