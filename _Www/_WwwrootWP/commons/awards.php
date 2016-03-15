<?php 
function getTitle($l)
{
	if($l==1){
		return "Награды";
	}
	return "Awards";
}
function insertSection($l){
?>
<?php if($l==1){?>
	Наши программы уже успели завоевать популярность в различных файловых архивах и у наших пользователей: 
<?php }else{?>
	Our products have earned high scores from many ratings websites as shown below. Our users also
	like our software as you can see from their testimonials:
<?php }?>

<br><br>

<a name="testimons">
<div id=h1><?php if($l==1){?>Отзывы наших пользователей<?php }else{?>User Testimonials<?php }?></div>
</a>

<table border=0 width=100% cellspacing="0" cellpadding="0">

<tr><td width=20% align=right><font color=blue>Oliver Baltz</font>&nbsp;</td><td align=left>&nbsp;WireKeys</td></tr>
<tr><td colspan=2>
As a programmer, I need to insert some snipplets of text into the source
code that I'm working on, e.g. for "tagging" changes in other people's
work. The extended clipboards of WireKeys are a great way to do that
with ease.
<br><br></td></tr>

<?php if($l==1){?>
<tr><td width=20% align=right><font color=blue>Liar</font>&nbsp;</td><td align=left>&nbsp;WireKeys</td></tr>
<tr><td colspan=2>
Наконец то я нашел именно ту, прогу для автоматизации работы за ПК, которую давно искал... Спасибо разработчикам!!! :-))
<br><br></td></tr>
<?php }?>

<tr><td width=20% align=right><font color=blue>Gary Sonnenberg</font>&nbsp;</td><td align=left>&nbsp;WireChanger</td></tr>
<tr><td colspan=2>
Changing desktop backgrounds automatically is great in and of itself,
but what really makes WireChanger special is the templates -- in
particular the calendar template. Now every time I logon, I see a
beautiful background and just a hint of a calendar overlaying it
reminding me of important (or not so important) events I've entered on
various dates this month. I love being able to set the opacity so I can
see the calendar and yet get a clear picture of the background shining through
<br><br></td></tr>

<tr><td width=20% align=right><font color=blue>Leela</font>&nbsp;</td><td align=left>&nbsp;WireChanger</td></tr>
<tr><td colspan=2>
The program is fascinating! It has everything I need on my desktop! I like changing wallpapers but WireChanger gives me so much more! It has the options I`ve never seen in any other program!
<br><br></td></tr>

<tr><td width=20% align=right><font color=blue>Roger</font>&nbsp;</td><td align=left>&nbsp;WireKeys</td></tr>
<tr><td colspan=2>
WireKeys 2.01 really rocks!!
Its power is quite amazing; I'm finding more and more uses!!
Thanks Guys!!
<br><br></td></tr>

<tr><td width=20% align=right><font color=blue>Woody</font>&nbsp;</td><td align=left>&nbsp;WireNote</td></tr>
<tr><td colspan=2>
I use WireNote like a virtual Post-It on my desk. It is very convenient, very easy to use. I use Note with transparency mode, in 'mini-mode'. I also use the reminder with Note, so I don`t forget anything anymore!
Congratulation for this splendid software. Much better than any other software. PS: I found a small 'bug'. I told WirePlane about this issue. 
In less than 3 days, they "fixit". Most of the company does not even answer when told them their software gets a 'bug'.
And you pay much more for something less efficient.
<br><br></td></tr>

<tr><td width=20% align=right><font color=blue>Gary Sonnenberg</font>&nbsp;</td><td align=left>&nbsp;WireNote</td></tr>
<tr><td colspan=2>
Since I'm usually sitting in front of the computer, I needed something
to actively remind me about appointments and other obligations not
involving the computer. I searched for some sort of reminder program
and found WireNote which fits the bill perfectly! I really love the
option that will literally shake the window on the screen to get your
attention.
<br><br></td></tr>

<tr><td width=20% align=right><font color=blue>Roger Parks</font>&nbsp;</td><td align=left>&nbsp;WireKeys Lite</td></tr>
<tr><td colspan=2>
Dear Sirs: Wirekeys is FABULOUS! Thank you for making it freely available.
<br><br></td></tr>
</table>

<br><br>

<table border=0 width=100% cellspacing="0" cellpadding="0">
<tr align=center>
<td><div id=h1>WireKeys</div></td>
<td><div id=h1>WireChanger</div></td>
<td><div id=h1>WireNote</div></td>
</tr>
<tr align=center valign=top>
<td width=33%>
<p><a HREF='http://www.softpedia.com/' target=_blank><IMG BORDER=0 SRC='/images2/organizations/softped5.gif' alt='5 stars at SoftPedia.com'><br>SoftPedia - 5/5</a></p><p><a HREF='http://www.brothersoft.com/' target=_blank><IMG BORDER=0 SRC='/images2/organizations/bsrating5.gif' alt='5 stars at BrotherSoft.com'><br>BrotherSoft - 5/5</a></p><p><a href='http://www.softnews.ro/' path='public/cat/15/2/15-2-36.shtml' target=_blank><img src='/images2/organizations/softnewstip.gif' alt='5 stars at Softnews.ro' border=0><br>SoftNews - 5/5</a></p><p><a href='http://www.softodrom.ru/' target=_blank><img src='http://www.softodrom.ru/images/star45.gif' alt='4.5 stars at softodrom.ru' border=0><br>Softodrom - 4.5/5</a></p><p><a HREF='http://www.sharewareriver.com/' path='product.php?id=2296' target=_blank><IMG BORDER=0 SRC='/images2/organizations/sr5pt1.gif' WIDTH=90 HEIGHT=90 alt='5 stars at S-River'><br>S-River - 5/5</a></p><p><a href='http://www.filetransit.com' path='/view.php?id=13251' target=_blank><img src='/images2/organizations/filetransit5.gif' width=95 height=59 border=0 alt='5/5 at FileTransit.com'><br>FileTransit - 5/5</a></p><p><a href='http://fileheaven.com/' path='WireKeys/download/2394.htm' target=_blank><img src='/images2/organizations/filehavencup_5.gif' width=91 height=50 border=0 alt='5 Stars at FileHeaven.com'><br>FileHeaven - 5/5</a></p><p><a href='http://www.listsoft.ru/' path='?id=12466' target=_blank><img src='http://www.listsoft.ru/img/new/cool.gif' width=80 height=60 border=0 alt='COOL'><br>Listsoft - 4/5</a></p><p><a href='http://www.softbox.ru/' path='?a=21&i=2036' target=_blank><img src='http://softbox.ru/top/inf/aw.php?i=2036&t=3' alt='Softbox.ru' border=0><br>Softbox - 4/5</a></p><p><a href='http://www.webtechgeek.com/' path='Free-Windows-Software33.htm#wirekeys' target=_blank><img src='/images2/organizations/best5wtg.gif' width=137 height=72 border=0 alt='5/5 at WetTechGeek.com'><br>WebTechGeek - 5/5</a></p><p><a href='http://www.biz2consumer.com/downloads/' target=_blank><img src='/images2/organizations/5_rating_biz2c.gif' alt='5 stars at Biz2Consumer.com' border=0><br>Biz2Consumer.com - 5/5</a></p><p><a href='http://www.adlenterprises.com' target=_blank><img src='http://www.adlenterprises.com/images/adlenterprises5.gif' border='0'><br>Adlenterprises - 5/5</a></p>
<br></td>
<td width=33%>
<p><a HREF='http://www.softpedia.com/' target=_blank><IMG BORDER=0 SRC='/images2/organizations/softped5.gif' alt='5 stars at SoftPedia.com'><br>SoftPedia - 5/5</a></p><p><a HREF='http://www.brothersoft.com/' target=_blank><IMG BORDER=0 SRC='/images2/organizations/bsrating5.gif' alt='5 stars at BrotherSoft.com'><br>BrotherSoft - 5/5</a></p><p><a HREF='http://www.sharewareriver.com/' path='product.php?id=2289' target=_blank><IMG BORDER=0 SRC='/images2/organizations/sr5pt1.gif' WIDTH=90 HEIGHT=90 alt='5 stars at SHAREWARERiver'><br>SharewareRiver - 5/5</a></p><p><a href='http://www.softodrom.ru/' target=_blank><img src='http://www.softodrom.ru/images/star45.gif' alt='4.5 stars at softodrom.ru' border=0><br>Softodrom - 4.5/5</a></p><p><a href='http://www.filetransit.com/' path='view.php?id=13250' target=_blank><img src='/images2/organizations/filetransit5.gif' width=95 height=59 border=0 alt='5/5 at FileTransit.com'><br>FileTransit - 5/5</a></p><p><a href='http://fileheaven.com/' path='WireChanger/download/2393.htm' target=_blank><img src='/images2/organizations/filehavencup_5.gif' width=91 height=50 border=0 alt='5 Stars at FileHeaven.com'><br>FileHeaven - 5/5</a></p><p><a href='http://www.listsoft.ru/' path='?id=12464' target=_blank><img src='http://www.listsoft.ru/img/new/good.gif' width=80 height=60 border=0 alt='GOOD'><br>ListSoft - 4/5</a></p><p><a href='http://www.softbox.ru/' path='?a=21&i=2038' target=_blank><img src='http://softbox.ru/top/inf/aw.php?i=2038&t=3' alt='Softbox.ru' border=0><br>SoftBox - 5/5</a></p><p><a href='http://www.softnews.ro/' path='public/cat/9/6/9-6-51.shtml' target=_blank><img src='/images2/organizations/softnewstip.gif' alt='5 stars at Softnews.ro' border=0><br>SoftNews - 5/5</a></p><p><a href='http://www.biz2consumer.com/downloads/' target=_blank><img src='/images2/organizations/5_rating_biz2c.gif' alt='5 stars at Biz2Consumer.com' border=0><br>Biz2Consumer.com - 5/5</a></p><p><a href='http://www.adlenterprises.com' target=_blank><img src='http://www.adlenterprises.com/images/adlenterprises5.gif' border='0'><br>Adlenterprises - 5/5</a></p><p><a href='http://www.freshshare.com/' target=_blank><img src='http://www.freshshare.com/images/fsstar_5.jpg' border='0'><br>FreshShare - 5/5</a></p>
<br></td>
<td width=33%>
<p><a href='http://www.nonags.com' target=_blank><img src='/images2/organizations/nonags6.gif' alt='6 of 6 at NoNags.com' border=0><br>NoNags.com - 6/6</a></p><p><a HREF='http://www.softpedia.com/' target=_blank><IMG BORDER=0 SRC='/images2/organizations/softped5.gif' alt='5 stars at SoftPedia.com'><br>SoftPedia - 5/5</a></p><p><a href='http://www.listsoft.ru/' path='?id=12461' target=_blank><img src='http://www.listsoft.ru/img/new/best.gif' width=80 height=60 border=0 alt='BEST!'><br>ListSoft - 5/5</a></p><p><a href='http://www.softodrom.ru/' target=_blank><img src='http://www.softodrom.ru/images/star45.gif' alt='4.5 stars at softodrom.ru' border=0><br>Softodrom - 4.5/5</a></p><p><A HREF='http://www.sharewareriver.com/' path='product.php?id=2297' target=_blank><IMG BORDER=0 SRC='/images2/organizations/sr5pt1.gif' WIDTH=90 HEIGHT=90 alt='5 stars at SHAREWARERiver'><br>SharewareRiver - 5/5</a></p><p><a href='http://www.filetransit.com/' path='view.php?id=13249' target=_blank><img src='/images2/organizations/filetransit5.gif' width=95 height=59 border=0 alt='5/5 at FileTransit.com'><br>FileTransit - 5/5</a></p><p><a href='http://www.topshareware.com/' path='WireNote-download-2392.htm' target=_blank><img src='/images2/organizations/topshareware_5.gif' alt='5 stars rating at TopShareware.com' width='130' height='80' border='0'><br>TopShareware - 5/5</a></p><p><a href='http://www.globalshareware.com/' path='Internet/Communications/WireNote.htm' target=_blank><img src='/images2/organizations/5golddisk-award-8831.gif' border=0 alt='5 Gold Disk at GlobalShareware.com'><br>GlobalShareware - 5/5</a></p><p><a href='http://fileheaven.com/' path='WireNote/download/2392.htm' target=_blank><img src='/images2/organizations/filehavencup_5.gif' width=91 height=50 border=0 alt='5 Stars at FileHeaven.com'><br>FileHeaven - 5/5</a></p><p><a href='http://www.softbox.ru/' path='?a=21&i=2039' target=_blank><img src='http://softbox.ru/top/inf/aw.php?i=2039&t=3' alt='Softbox.ru' border=0><br>Softbox - 4/5</a></p><p><a href='http://www.biz2consumer.com/downloads/' target=_blank><img src='http://www.biz2consumer.com/extras/rating/4_rating.gif' alt='4 stars at Biz2Consumer.com' border=0><br>Biz2Consumer.com - 4/5</a></p><p><a href='http://www.freetrialsoft.com/WireNote-review-2527.html' target=_blank><img src='/images2/organizations/freetrialsoft-pick.png' alt='Editor`s pick at FreeTrialSoft.com' border=0><br>FreeTrialSoft.com - 6/5</a></p>
<br></td></tr>
<tr align=center>
<td><a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" border=0 alt="Download WireKeys for free!"></a></td>
<td><a href="/download/wirechanger.zip"><img src="/images2/button_download.gif" border=0 alt="Download WireChanger for free!"></a></td>
<td><a href="/download/wirenote.zip"><img src="/images2/button_download.gif" border=0 alt="Download WireNote for free!"></a></td>
</tr>
</table>

<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
