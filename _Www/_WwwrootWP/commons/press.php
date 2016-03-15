<?php 
function getTitle($l)
{
	if($l==1){
		return "Файловая зона";
	}
	return "Free downloads";
}
function insertSection($l){
?>
<?php if($l==1){?>
	Здесь находятся ссылки на материалы, изданные в электронных изданиях по разным продуктам, а также пресс-релизы наших продуктов
<?php }else{?>
	Here you can find articles, press releases and other materials about our products
<?php }?>
<br><br>

<lu>
<li> <a href="http://www.izone.ru/data/soft/article1013.htm">WireChanger review</a>, "IZone Domino computer magazine" in Russian
<li> <a href="http://www.homecomputermagazine.com/news/item-101.html">WireNote review</a>, "Home computer magazine" in English
<li> <a href="../articles/press6.php">Press release</a> about WireKeys 3.0 in Russian
<li> <a href="../articles/press5.php">Press release</a> about WireChanger 3 in English
<li> <a href="../articles/press1.php">Press release</a> about WireChanger 2.8 in Russian
<li> <a href="../articles/press2.php">Press release</a> about WireChanger 2.8 in English
<li> <a href="../articles/press3.php">Press release</a> about WireKeys 2.8 in English
</lu>
<br><br>
<div id=h1><?php if($l==1){?>Графика, баннеры и пр.<?php }else{?>Graphics, banners, etc.<?php }?></div>
<OBJECT classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"
 codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0"
 WIDTH="468" HEIGHT="60" ALIGN="">
 <PARAM NAME=movie VALUE="/images2/wirechanger/banner1.swf"> <PARAM NAME=quality VALUE=high> <PARAM NAME=bgcolor VALUE=#FFFFFF> <EMBED src="/images2/wirechanger/banner1.swf" quality=high bgcolor=#FFFFFF  WIDTH="468" HEIGHT="60" ALIGN=""
 TYPE="application/x-shockwave-flash" PLUGINSPAGE="http://www.macromedia.com/go/getflashplayer"></EMBED>
</OBJECT>
<br><br>
<OBJECT classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"
 codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0"
 WIDTH="468" HEIGHT="60" ALIGN="">
 <PARAM NAME=movie VALUE="/images2/wirekeys/banner1.swf"> <PARAM NAME=quality VALUE=high> <PARAM NAME=bgcolor VALUE=#FFFFFF> <EMBED src="/images2/wirekeys/banner1.swf" quality=high bgcolor=#FFFFFF  WIDTH="468" HEIGHT="60" ALIGN=""
 TYPE="application/x-shockwave-flash" PLUGINSPAGE="http://www.macromedia.com/go/getflashplayer"></EMBED>
</OBJECT>
<br><br>
<img src="/images2/wirekeys/banner2.gif" width=468 height=60 border=0>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
