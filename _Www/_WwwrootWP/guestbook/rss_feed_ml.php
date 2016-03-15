<?php 
	global $languageNumber;
	$l=$languageNumber;
?>
<?php
include "rss_fetch.php"; 
if($l==1){
$rsses[1]="http://feeds.feedburner.com/game-budizm";
$ranNum = 1;//rand(1, 16); 
}else{
$rsses[1]="http://rss.news.yahoo.com/rss/tech";
$rsses[2]="http://rss.news.yahoo.com/rss/topstories";
$rsses[3]="http://rss.cnn.com/rss/cnn_space.rss";
$rsses[4]="http://rss.cnn.com/rss/cnn_tech.rss";
$rsses[5]="http://rss.cnn.com/rss/cnn_world.rss";
$rsses[6]="http://www.wired.com/news/feeds/rss2/0,2610,3,00.xml";
$rsses[7]="http://www.wired.com/news/feeds/rss2/0,2610,,00.xml";
$rsses[8]="http://rss.pcworld.com/rss/latestnews.rss";
$rsses[9]="http://www.2space.net/rss/latestarticles.xml";
$rsses[10]="http://news.google.com/nwshp?gl=us&ned=us&topic=t&output=rss";
$rsses[11]="http://news.google.com/nwshp?gl=us&ned=us&output=rss";
$rsses[12]="http://news.google.com/nwshp?gl=us&ned=us&topic=w&output=rss";
$rsses[13]="http://news.google.com/nwshp?gl=us&ned=us&topic=b&output=rss";
$rsses[14]="http://news.google.com/nwshp?gl=us&ned=us&topic=s&output=rss";
$rsses[15]="http://news.google.com/nwshp?gl=us&ned=us&topic=e&output=rss";
$rsses[16]="http://news.google.com/nwshp?gl=us&ned=us&topic=m&output=rss";
$ranNum = rand(1, 16); 
}

$url = $rsses[$ranNum];
$show = 4;
$html = "<table width=120 cellspacing=0 cellpadding=1><tr><td><a href='#{link}' target='_new'>#{title}</a></td></tr></table><img src='/images2/vmenu_line.gif' width=120 height=7>"; 
$update = 1;
$rss = new rss_parser($url, $show, $html, $update);  
?> 