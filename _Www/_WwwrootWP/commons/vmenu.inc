<?php 
	global $languageNumber;
	$l=$languageNumber;
?>
<div id=vertical_menu>
<table width="135" border="0" cellspacing="0" cellpadding="0" align="center" >
<tr><td><div id=vertical_menu_section><a href="../commons/about.php" class=menu><img src="/images2/vmenu_products.gif" width=133 height=14></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../wirekeys/" class=menu>
	WireKeys</a>
	<br>&nbsp;-&nbsp;<a href="../wirekeys/info.php" class=menu>Info</a>
	<br>&nbsp;-&nbsp;<a href="../wirekeys/wkplugins.php" class=menu>Plugins</a>
	<br>&nbsp;-&nbsp;<a href="../wirekeys/wkmacros.php" class=menu>Macros</a>
	</div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../wirechanger/" class=menu>
	WireChanger</a>
	<br>&nbsp;-&nbsp;<a href="../wirechanger/info.php" class=menu>Info</a>
	<br>&nbsp;-&nbsp;<a href="../wirechanger/wallpapers.php" class=menu>Free wallpapers</a>
	<br>&nbsp;-&nbsp;<a href="../wirechanger/widgets.php" class=menu>Desktop widgets</a>
	<br>&nbsp;-&nbsp;<a href="../wirechanger/wcclocks.php" class=menu>Desktop clocks</a>
	</div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../commons/freeware.php" class=menu>
	Freeware</a>
	<br>&nbsp;-&nbsp;<a href="../wpt-wirenote/about.php" class=menu>WireNote</a>
	<br>&nbsp;-&nbsp;<a href="../wpt-kaleidoswallpaper/about.php" class=menu>KaleidosWallpaperr</a>
	<br>&nbsp;-&nbsp;<a href="../wpt-backup/about.php" class=menu>WPBackup</a>
	<br>&nbsp;-&nbsp;<a href="../wpt-outlook-pop3/about.php" class=menu>Mapi2Pop3</a>
	<?php if($l==1){?>
	<br>&nbsp;-&nbsp;<a href="../wpt-gamelogr/" class=menu>GameLogr</a>
	<br>&nbsp;-&nbsp;<a href="../commons/annotation-builder2.php" class=menu>Annotation builder</a>
	<br>&nbsp;-&nbsp;<a href="../wpt-jazikator/" class=menu>Языкатор</a>
	<br>
	<br>&nbsp;<a href="http://gamebudizm.blogspot.com/" class=menu>Game-будизм: блог</a>
	<br>&nbsp;<a href="../commons/web-projects-list.php" class=menu>Web-проекты</a>
	<?php }else{?>
	<br>
	<br>&nbsp;<a href="http://www.gamerotor.com/" class=menu>Casual games</a>
	<?php } ?>
	</div></td></tr>
<tr><td><div id=vertical_menu_section><img src="/images2/vmenu_supportnews.gif" width=133 height=14></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../commons/order.php" class=menu>
	<?php if($l==1){?>Купить<?php }else{?>Order Now<?php }?></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../commons/download.php" class=menu>
	<?php if($l==1){?>Скачать<?php }else{?>Download<?php }?></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="http://www.gamerotor.com/forum" class=menu>
	<?php if($l==1){?>Форумы<?php }else{?>Forums<?php }?></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../commons/guestbook.php" class=menu>
	<?php if($l==1){?>Контакты<?php }else{?>Contact Us<?php }?></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../commons/press.php" class=menu>
	<?php if($l==1){?>Для прессы<?php }else{?>For Press<?php }?></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../commons/help.php#newsletter" class=menu>
	<?php if($l==1){?>Рассылка<?php }else{?>Newsletter<?php }?></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="../commons/company.php" class=menu>
	<?php if($l==1){?>О компании<?php }else{?>About Us<?php }?></a></div></td></tr>
	
<tr><td><div id=vertical_menu_section><img src="/images2/vmenu_language.gif" width=133 height=14></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="/en/commons/about.php" class=menu>
	<?php if($l==1){?>Английский<?php }else{?>English<?php }?></a></div></td></tr>
<tr><td><div id=vertical_menu_item><a href="/ru/commons/about.php" class=menu>
	<?php if($l==1){?>Русский<?php }else{?>Russian<?php }?></a></div></td></tr>
</table>
</div>
