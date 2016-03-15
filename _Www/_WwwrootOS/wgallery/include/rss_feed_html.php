<?php
include "rss_fetch.php"; 
$rsses[1]="http://rss.news.yahoo.com/rss/tech";
$rsses[2]="http://rss.news.yahoo.com/rss/topstories";
$rsses[3]="http://rss.cnn.com/rss/cnn_space.rss";
$rsses[4]="http://rss.cnn.com/rss/cnn_tech.rss";
$rsses[5]="http://rss.cnn.com/rss/cnn_world.rss";
$rsses[6]="http://www.wired.com/news/feeds/rss2/0,2610,3,00.xml";
$rsses[7]="http://www.wired.com/news/feeds/rss2/0,2610,,00.xml";
$rsses[8]="http://digg.com/rss/containergaming.xml";
$rsses[9]="http://digg.com/rss/containerentertainment.xml";
$rsses[10]="http://digg.com/rss/containerscience.xml";
$rsses[11]="http://digg.com/rss/containertechnology.xml";

$ranNum = rand(1, 11); 
$url = $rsses[$ranNum];
$show = 10;   
$html = "<a href='#{link}' target='_new'>#{title}</a><hr />"; 
$update = 1;  
print "<hr>";
$rss = new rss_parser($url, $show, $html, $update);  
?> 