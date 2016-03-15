<?php
$config=Array();
$config['basedir']=".";

function sortZip($a,$b) {
$valtype=array("folder"=>0,"file"=>1);
if($a["type"]==$b["type"]) return(strcmp($a["filename"],$b["filename"]));
return($valtype[$a["type"]]-$valtype[$b["type"]]); }

class readzip
{
var $ctrl_dir = array();
var $contentlist = array();
var $filename = "";
var $read = 0;
var $filehandle = "";

function readzip($filename)
{
if(!file_exists($filename)) $this->error("file not found");
//if(!extension_loaded("zlib")) $this->error("zlib library required");
$this->filename=$filename;
$this->open();
$this->readcontent();
$this->close();
}

function readcontent()
{
$this->contentlist=array();
$this->read_ctrl_dir();
@fseek($this->filehandle,$this->ctrl_dir['offset']);

for($i=0;$i<$this->ctrl_dir['entries'];$i++)
{
$this->contentlist[$i]=$this->read_centralfileheader();
$this->contentlist[$i]['index']=$i;
}

$this->read=1;
}

function unzip($index,$filename="")
{
if($this->read==0) $this->readcontent();
if(!isset($this->contentlist[$index])) $this->error("cannot find file in archive");
if($this->contentlist[$index]['type']=="folder") $this->error("cannot unzip a folder, only files");
if(!$filename) $filename="./".$this->contentlist[$index]['filename'];

$fh=@fopen($filename,"wb");
if(!$fh) $this->error("cannot open destiny file $filename for write");
fwrite($fh,$this->extract($index));
@fclose($fh);
@touch($filename,$this->contentlist[$index]['mtime']);
}

function writetmpfile($index,$zroot)
{
global $config;

$this->open();
if($this->read==0) $this->readcontent();
if(!isset($this->contentlist[$index])) return(-1); //$this->error("cannot find file in archive");
if($this->contentlist[$index]['type']=="folder") return(-2); //$this->error("cannot unzip a folder, only files");
fseek($this->filehandle,($this->contentlist[$index]['offset']+$this->contentlist[$index]['filename_len']+$this->contentlist[$index]['extra_len']+30));

#if($this->contentlist[$index]['compressed_size']==$this->contentlist[$index]['size'])
if($this->contentlist[$index]['compression']==0)
{
$tmpfile=$zroot."/uncompress_".$index."_".md5(uniqid(rand()));
while(file_exists($tmpfile)) $tmpfile=$zroot."/uncompress_".$index."_".md5(uniqid(rand()));
}
else
{
$tmpfile=$zroot."/uncompress_".$index."_".md5(uniqid(rand())).".gz";
while(file_exists($tmpfile)) $tmpfile=$zroot."/uncompress_".$index."_".md5(uniqid(rand())).".gz";
}

$tmp_fh=@fopen($tmpfile,"wb");
if(!$tmp_fh) return(-8); //$this->error("cannot open temporary file $tmpfile for write");

// uncompressed
#if($this->contentlist[$index]['compressed_size']==$this->contentlist[$index]['size'])
if($this->contentlist[$index]['compression']==0)
{
$size = $this->contentlist[$index]['compressed_size'];
while($size)
{
$read_size = ($size < 2048 ? $size : 2048);
@fwrite($tmp_fh,pack('a'.$read_size,@fread($this->filehandle, $read_size)),$read_size);
$size -= $read_size;
}
}
// compressed
else
{
$binary_data = pack('va1a1Va1a1', 0x8b1f, Chr($this->contentlist[$index]['compression']), Chr(0x00), time(), Chr(0x00), Chr(3));
@fwrite($tmp_fh, $binary_data, 10);
$size = $this->contentlist[$index]['compressed_size'];
while($size)
{
$read_size = ($size < 2048 ? $size : 2048);
@fwrite($tmp_fh,pack('a'.$read_size,@fread($this->filehandle, $read_size)),$read_size);
$size -= $read_size;
}
@fwrite($tmp_fh,pack('VV', $this->contentlist[$index]['crc'], $this->contentlist[$index]['size']), 8);
}

@fclose($tmp_fh);
@chmod($tmpfile,0777);
$this->close();
return $tmpfile;
}

function unziptmp($index,$tmpfile,$dest)
{
$this->open();
if($this->read==0) $this->readcontent();
if(!isset($this->contentlist[$index])) return(-1); //$this->error("cannot find file in archive");
if($this->contentlist[$index]['type']=="folder") return(-2); //$this->error("cannot unzip a folder, only files");
if(!file_exists($tmpfile)) return(-3); //$this->error("cannot find temporary file $tmpfile for read");

// uncompressed
#if($this->contentlist[$index]['compressed_size']==$this->contentlist[$index]['size'])
if($this->contentlist[$index]['compression']==0)
{
$copy=@copy($tmpfile,$dest);
if(!$copy) return(-4); //$this->error("cannot copy temorary file $tmpfile to $dest");
}
// compressed
else
{
// die if zlib library not loaded
if(!extension_loaded("zlib") || !function_exists("gzopen")) return(-5); //$this->error("zlib library required");

$tmp_fh=@gzopen($tmpfile,"rb");
if(!$tmp_fh) return(-6); //$this->error("cannot open temporary file $tmpfile for read");
$fh=@fopen($dest,"wb");
if(!$fh) return(-7); //$this->error("cannot open file $dest for write");
$size = $this->contentlist[$index]['size'];
while($size)
{
$read_size = ($size < 2048 ? $size : 2048);
@fwrite($fh,pack('a'.$read_size,@gzread($tmp_fh, $read_size)),$read_size);
$size -= $read_size;
}
@gzclose($tmp_fh);
@fclose($fh);
}
@chmod($dest,0777);
}

function read_centralfileheader()
{
$binary_data = @fread($this->filehandle, 4);
$data = unpack('Vid', $binary_data);
$binary_data = @fread($this->filehandle,42);
$header = unpack('vversion/vversion_extracted/vflag/vcompression/vmtime/vmdate/Vcrc/Vcompressed_size/Vsize/vfilename_len/vextra_len/vcomment_len/vdisk/vinternal/Vexternal/Voffset', $binary_data);
if($header['filename_len']) $header['filename'] = fread($this->filehandle,$header['filename_len']);
else $header['filename'] = "";
if($header['extra_len']) $header['extra'] = fread($this->filehandle,$header['extra_len']);
else $header['extra'] = "";
if($header['comment_len']) $header['comment'] = fread($this->filehandle,$header['comment_len']);
else $header['comment'] = "";
if ($header['mdate'] && $header['mtime'])
{
$hour = ($header['mtime'] & 0xF800) >> 11;
$minute = ($header['mtime'] & 0x07E0) >> 5;
$second = ($header['mtime'] & 0x001F)*2;
$year = (($header['mdate'] & 0xFE00) >> 9) + 1980;
$month = ($header['mdate'] & 0x01E0) >> 5;
$day = $header['mdate'] & 0x001F;
$header['mtime'] = mktime($hour, $minute, $second, $month, $day, $year);
}
else $header['mtime'] = time();
if(substr($header['filename'], -1) == '/') $header['external'] = 0x41FF0010;
if($header['external']==0x41FF0010) $header['type']="folder";
else $header['type']="file";

return $header;
}

function read_ctrl_dir()
{
$this->ctrl_dir=array();
$size=@filesize($this->filename);
$maximum_size=277;
if($maximum_size>$size) $maximum_size=$size;
@fseek($this->filehandle,($size-$maximum_size));
$pos = @ftell($this->filehandle);
$bytes = 0x00000000;
while ($pos < $size)
{
$pos++;
$byte = fread($this->filehandle, 1);
$bytes = ($bytes << 8) | Ord($byte);
if($bytes==0x504b0506) break;
}

$binary_data = @fread($this->filehandle,18);
$data = unpack('vdisk/vdisk_start/vdisk_entries/ventries/Vsize/Voffset/vcomment_size', $binary_data);

if($data['comment_size'] != 0) $this->ctrl_dir['comment'] = @fread($this->filehandle, $data['comment_size']);
else $this->ctrl_dir['comment'] = '';
$this->ctrl_dir['entries'] = $data['entries'];
$this->ctrl_dir['disk_entries'] = $data['disk_entries'];
$this->ctrl_dir['offset'] = $data['offset'];
$this->ctrl_dir['size'] = $data['size'];
$this->ctrl_dir['disk'] = $data['disk'];
$this->ctrl_dir['disk_start'] = $data['disk_start'];
}

function error($msg)
{
$this->close();
die("<b>Fatal error:</b> ".$msg);
}
function open()
{
$this->filehandle = @fopen($this->filename,"rb");
if(!$this->filehandle) $this->error("cannot open archive ".$this->filename);
}
function close()
{
@fclose($this->filehandle);
unset($this->filehandle);
}

}








class writezip
{
var $datasec = array(); // array to store compressed data 
var $ctrl_dir = array(); // central directory 
var $eof_ctrl_dir = "x50x4bx05x06x00x00x00x00"; //end of Central directory record 
var $old_offset = 0; 
var $filename="";
var $filehandle="";
var $opened=0;
var $compression_level=-1;
var $write_directly=1;
var $datastring="";

function writezip($filename, $compression_level=-1,$write_directly=1)
{
$this->opened=0;
$this->filename=$filename;
$this->datasec=array();
$this->ctrl_dir=array();
$this->ctrl_dir_count=0;
$this->old_offset=0;
$this->compression_level=$compression_level;
$this->write_directly=$write_directly;
$this->datastring="";
if(!extension_loaded("zlib") || !function_exists("gzcompress")) $this->compression_level=-1;
}

function open()
{
if($this->opened==0 && $this->write_directly==1)
{
$this->filehandle=@fopen($this->filename,"wb");
@rewind($this->filehandle);
$this->opened=1;
}
}
function close()
{
if($this->opened==1 && $this->write_directly==1)
{
fclose($this->filehandle);
$this->opened=0;
}
}
function error($msg)
{
die("Fatal error: ".$msg);
}


function getDosDatetime($date=0)
{
$date = date("Y-m-d H:i:s",$date);

$regexp = "([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})";
$date = ereg_replace($regexp, "\1-\2-\3-\4-\5-\6", $date);
$date_array = split("-", $date);

$date_array[0] = $date_array[0]-1980;

$date_day = $date_array[2];
$date_month = $date_array[1];
$date_year = $date_array[0];
$date_hour = $date_array[3];
$date_minute = $date_array[4];
$date_second = $date_array[5];

$my_time = $date_hour;
$my_time = ($my_time << 6) + $date_minute;
$my_time = ($my_time << 5) + number_format($date_second / 2, 0);
$time_right = $my_time >> 8;
$time_left = $my_time - ($time_right << 8);

$my_date = $date_year;
$my_date = ($my_date << 4) + $date_month;
$my_date = ($my_date << 5) + $date_day;
$date_right = $my_date >> 8;
$date_left = $my_date - ($date_right << 8);

$time_left = sprintf("%02x", $time_left);
$time_right = sprintf("%02x", $time_right);
$date_left = sprintf("%02x", $date_left);
$date_right = sprintf("%02x", $date_right);

return pack("H*H*H*H*", $time_left, $time_right, $date_left, $date_right);
}


// adds "directory" to archive - do this before putting any files in directory!
// $name - name of directory... like this: "path/"
// ...then you can add files using add_file with names like "path/file.txt"
function add_dir($name,$date="")
{
if(!$date) $date=time();
$name = str_replace("\\", "/", $name);
if(substr($name,-1)!="/") $name.="/";
$fr = "x50x4bx03x04";
$fr .= "x0ax00"; // ver needed to extract
$fr .= "x00x00"; // gen purpose bit flag
$fr .= "x00x00"; // compression method
//$fr .= "x00x00x00x00"; // last mod time and date
$fr .= $this->getDosDatetime($date); // last mod time & date 

$fr .= pack("V",0); // crc32
$fr .= pack("V",0); //compressed filesize
$fr .= pack("V",0); //uncompressed filesize
$fr .= pack("v", strlen($name) ); //length of pathname
$fr .= pack("v", 0 ); //extra field length
$fr .= $name;
// end of "local file header" segment

// no "file data" segment for path

// "data descriptor" segment (optional but necessary if archive is not served as file)
$fr .= pack("V",$crc); //crc32
$fr .= pack("V",$c_len); //compressed filesize
$fr .= pack("V",$unc_len); //uncompressed filesize

// add this entry to array
if($this->write_directly==1)
{
@fwrite($this->filehandle,$fr);
$new_offset=ftell($this->filehandle);
}
else
{
$this->datastring.=$fr;
$new_offset=strlen($this->datastring);
}


// ext. file attributes mirrors MS-DOS directory attr byte, detailed
// at http://support.microsoft.com/support/kb/articles/Q125/0/19.asp

// now add to central record
$cdrec = "x50x4bx01x02";
$cdrec .="x00x00"; // version made by
$cdrec .="x0ax00"; // version needed to extract
$cdrec .="x00x00"; // gen purpose bit flag
$cdrec .="x00x00"; // compression method
$cdrec .= $this->getDosDatetime($date); // last mod time & date 
//$cdrec .="x00x00x00x00"; // last mod time & date
$cdrec .= pack("V",0); // crc32
$cdrec .= pack("V",0); //compressed filesize
$cdrec .= pack("V",0); //uncompressed filesize
$cdrec .= pack("v", strlen($name) ); //length of filename
$cdrec .= pack("v", 0 ); //extra field length
$cdrec .= pack("v", 0 ); //file comment length
$cdrec .= pack("v", 0 ); //disk number start
$cdrec .= pack("v", 0 ); //internal file attributes
$ext = "x00x00x10x00";
$ext = "xffxffxffxff";
$cdrec .= pack("V", 16 ); //external file attributes - 'directory' bit set

$cdrec .= pack("V", $this -> old_offset ); //relative offset of local header
$this -> old_offset = $new_offset;

$cdrec .= $name;
// optional extra field, file comment goes here
// save to array
$this -> ctrl_dir[] = $cdrec;
#$this->ctrl_dir.=$cdrec;
#$this->ctrl_dir_count++;
}


// adds "file" to archive
// $data - file contents
// $name - name of file in archive. Add path if your want
function add_data($data, $name,$date="")
{
if(!$date) $date=time();

$name = str_replace("\\", "/", $name);
//$name = str_replace("\", "\\", $name);

$fr = "x50x4bx03x04";
$fr .= "x14x00"; // ver needed to extract
$fr .= "x00x00"; // gen purpose bit flag
if($this->compression_level>=0)
{
$fr .= "x08x00"; // compression method
}
elseif($this->compression_level==-1)
{
$fr .= "x00x00"; // compression method: uncompressed
}
//$fr .= "x00x00x00x00"; // last mod time and date
$fr .= $this->getDosDatetime($date); // last mod time & date 

$unc_len = strlen($data);
$crc = crc32($data);

if($this->compression_level>=0)
{
$zdata = gzcompress($data,$this->compression_level);
$zdata = substr( substr($zdata, 0, strlen($zdata) - 4), 2); // fix crc bug
}
elseif($this->compression_level==-1)
{
$zdata = $data; // save data uncompressed => faster
}

$c_len = strlen($zdata);
$fr .= pack("V",$crc); // crc32
$fr .= pack("V",$c_len); //compressed filesize
$fr .= pack("V",$unc_len); //uncompressed filesize
$fr .= pack("v", strlen($name) ); //length of filename
$fr .= pack("v", 0 ); //extra field length
$fr .= $name;
// end of "local file header" segment

// "file data" segment
$fr .= $zdata;

// "data descriptor" segment (optional but necessary if archive is not served as file)
$fr .= pack("V",$crc); //crc32
$fr .= pack("V",$c_len); //compressed filesize
$fr .= pack("V",$unc_len); //uncompressed filesize

// add this entry to array
if($this->write_directly==1)
{
@fwrite($this->filehandle,$fr);
$new_offset = ftell($this->filehandle);
}
else
{
$this->datastring.=$fr;
$new_offset=strlen($this->datastring);
}


// now add to central directory record
$cdrec = "x50x4bx01x02";
$cdrec .="x00x00"; // version made by
$cdrec .="x14x00"; // version needed to extract
$cdrec .="x00x00"; // gen purpose bit flag
$cdrec .="x08x00"; // compression method
$cdrec .= $this->getDosDatetime($date); // last mod time & date 
//$cdrec .="x00x00x00x00"; // last mod time & date
$cdrec .= pack("V",$crc); // crc32
$cdrec .= pack("V",$c_len); //compressed filesize
$cdrec .= pack("V",$unc_len); //uncompressed filesize
$cdrec .= pack("v", strlen($name) ); //length of filename
$cdrec .= pack("v", 0 ); //extra field length
$cdrec .= pack("v", 0 ); //file comment length
$cdrec .= pack("v", 0 ); //disk number start
$cdrec .= pack("v", 0 ); //internal file attributes
$cdrec .= pack("V", 32 ); //external file attributes - 'archive' bit set

$cdrec .= pack("V", $this -> old_offset ); //relative offset of local header
$this -> old_offset = $new_offset;

$cdrec .= $name;
// optional extra field, file comment goes here
// save to central directory
$this -> ctrl_dir[] = $cdrec;
#$this->ctrl_dir.=$cdrec;
#$this->ctrl_dir_count++;
}

function file()
{
#$data = implode("", $this -> datasec);
#return $data.$ctrldir.$this -> eof_ctrl_dir.pack("v", sizeof($this -> ctrl_dir)).pack("v", sizeof($this -> ctrl_dir)).pack("V", strlen($ctrldir)).pack("V", strlen($data))."x00x00";
#return $ctrldir.$this -> eof_ctrl_dir.pack("v", $this -> ctrl_dir_count).pack("v", $this -> ctrl_dir_count).pack("V", strlen($ctrldir)).pack("V", ftell($this->filehandle))."x00x00";
if($this->write_directly==1)
{
$ctrldir = implode("", $this -> ctrl_dir);
return $ctrldir.$this -> eof_ctrl_dir.pack("v", sizeof($this -> ctrl_dir)).pack("v", sizeof($this -> ctrl_dir)).pack("V", strlen($ctrldir)).pack("V", ftell($this->filehandle))."x00x00";
}
else
{
$ctrldir = implode("", $this -> ctrl_dir);
return $this->datastring.$ctrldir.$this -> eof_ctrl_dir.pack("v", sizeof($this -> ctrl_dir)).pack("v", sizeof($this -> ctrl_dir)).pack("V", strlen($ctrldir)).pack("V", strlen($this->datastring))."x00x00";
}
}

function create()
{
if($this->write_directly==1) @fwrite($this->filehandle,$this->file());
else return $this->file();
}

function add_file($src,$name,$date="")
{
$fh=@fopen($src,"rb");
if(!$fh) $this->error("cannot open $src");
#$data=@fread($fh,@filesize($src));
#$this->add_data($data,$name);
$this->add_data(@fread($fh,@filesize($src)),$name,$date);
#$this->add_data("dump",$name);
@fclose($fh);
}

function add_tree($dir)
{
$dir=str_replace("\\","/",$dir);
if(substr($dir,-1)=="/") $dir=substr($dir,0,strlen($dir)-1);
if(!file_exists($dir)) $this->error("directory $dir does not exist.");

$dirs=array();
$files=array();
$this->get_tree($dir,$dirs,$files);

if(count($dirs) || count($files))
{
foreach($dirs as $dirname)
{
if($dirname!=$dir && strlen($dirname)>strlen($dir))
{
#echo "Adding directory ".substr($dirname,strlen($dir)+1)."<br>";
$this->add_dir(substr($dirname,strlen($dir)+1));
}
}
foreach($files as $filename)
{
#echo "Adding file ".substr($filename,strlen($dir)+1)."<br>";
$this->add_file($filename,substr($filename,strlen($dir)+1));
}
}
return $files;
}

function get_tree($dir,&$dirs,&$files)
{
$dirs[]=$dir;
$dirh=@opendir($dir);
if(!$dirh) $this->error("Cannot open directory $dir for reading.");
while($filename=readdir($dirh))
{
if($filename=="." || $filename=="..") continue;
elseif(is_dir($dir."/".$filename)) $this->get_tree($dir."/".$filename,$dirs,$files);
elseif(is_file($dir."/".$filename)) $files[]=$dir."/".$filename;
}
@closedir($dirh);
}
}

function ziperror($r) {
switch($r) {
case -1: return(("Cannot find file in archive"));
case -2: return(("Cannot unzip a folder, only files"));
case -3: return(("Cannot find temporary file for read"));
case -4: return(("Cannot copy temporary file to destination"));
case -5: return(("Zlib library required"));
case -6: return(("Cannot open temporary file for read"));
case -7: return(("Cannot open file for write"));
case -8: return(("Cannot open temporary file for write"));
} }

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

function processMD5In(&$db, $dirsource, &$count, &$toskip,&$countall)
{
	//require_once('DB.php');
	//require_once('main.config.php');
	@chmod($dirsource, octdec("0666"));
	if(is_dir($dirsource)){
	  	  $dir_handle=opendir($dirsource); 
	}
	$md5folder=$config['absPath']."cache/md5";
	$md5folder=realpath("../".$md5folder);
	while($file=readdir($dir_handle)){ 
		if($file!="." && $file!=".." && $file!="_vti_cnf"){ 
			$fpath=$dirsource."/".$file;
			if(!is_dir($fpath)){
				$countall++;
				if($toskip>0){
					$toskip--;
					continue;
				}
				if(($countall%20)==0){
					echo "<a href='http://outer-station.m6.net/admin/index.php?action=recreatemd5&skip=$countall'>$countall</a>... ";
				}
				@chmod($fpath, octdec("0666"));
				// Сохраняем md5
				$md5raw=md5_xfile($fpath,$md5folder);
				$md5=$md5folder."/h-".$md5raw.".md5";
				if(file_exists($md5)){
					// Конфликт!!!
					$count++;
					$openFileMD5 = fopen($md5, "r");
					$prefile=fread($openFileMD5,filesize($md5));
					fclose($openFileMD5);
					if($fpath!=$prefile){
						echo '<br>Conflict:<br>'.$fpath.'<br>'.$prefile.'<br>md5='.$md5raw;
						// Ищем файл по имени
						$selectline='SELECT * FROM '.TP.'images WHERE imageFilename = "'.mysql_escape_string($file).'"';
						$result =& $db->query($selectline);
						if (!DB::isError($result)) {
							$line =& $result->fetchRow(DB_FETCHMODE_ASSOC);
							//echo ' ImageID='.$line['imageID'].', album='.$line['albumID'];
							//http://outer-station.m6.net/admin/index.php?s=images&a=delete&image=943
							echo '<br>';
							echo '<a href="/admin/index.php?s=images&a=delete&image='.$line['imageID'].'">Delete image '.$line['albumID'].'/'.$line['imageID'].'</a>';
						}else{
							die($result->getMessage());
						}
						echo '<hr>';
					}
				}else{
					$openFileMD5 = fopen($md5, "w");
					fwrite($openFileMD5,$fpath);
					fclose($openFileMD5);
				}
			}else{
				processMD5In($db,$fpath, $count, $toskip,$countall);
			}
		}
	} 
	closedir($dir_handle); 
}
?>