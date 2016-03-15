<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "Управление системой с помощью гор. клавиш, полная автоматизация работы за компьютером";
	}
	return "Automation tool for power users, Control your PC using hotkeys";
}
function insertSection($l){
?>

<?php if($l==1){?>
<p>Если Вы разработали клавиатуру с расширенной функциональностью или уже продаете клавиатуры, то задумайтесь о включении WireKeys в стандартную поставку.
Это будет отличным дополнением к новой клавиатуре для всех Ваших покупателей!

<p>Мы можем предложить два пути сотрудничества:

<p>1. WireKeys будет основным приложением для управления горячими клавишами и возможностями клавиатуры.
Преимущества:
<ul>
  <li>Клавиатура будет иметь дополнительные преимущества перед конкурентами, которые Вы сможете описать покупателям
  <li>Не нужно разрабатывать свое собственное приложение
  <li>Очень гибкая система лицензирования (подробности - в личной переписке)
  <li>Мы с легкостью и по первому требованию добавляем нужные Вам функциональности и подгоняем приложение под Ваши нужды
</ul>

<p>2. WireKeys будет дополнительным приложением. Преимущества:

<ul>
  <li>Специальная бесплатная версия WireKeys, которую можно распространять без дополнительных согласований/лицензирования.
  <li>Специальный набор чтобы сэкономить Ваше время для распространения WireKeys с клавиатурами - иконки, скриншоты, описания, пресс-релизы и др.
</ul>
<br>
Мы также всегда открыты для любых Ваших предложений. Если Вы хотите задать вопрос или
узнать дополнительные подробности, напишите нам: <a href="mailto:custom@wiredplane.com">custom@wiredplane.com</a>
<?php }else{?>
<p>Develop a keyboard with extended functionality? Already selling
keyboards? Add significant value to your products - include WireKeys software.

<p>Generally there are two ways we work with hardware manufacturers:

<p>1. WireKeys is a primary software for handling keyboard shortcuts.
Advantages:
<ul>
  <li>Keyboard gets additional features you can describe to buyers
  <li>No need to develop own software
  <li>Flexible licensing policy
  <li>Ability to add custom functionality
</ul>

<p>2. WireKeys is a bonus software. Advantages:

<ul>
  <li>Special FREE version of WireKeys to include without written permission.
  <li>Publisher package to minimize your time expenses by including WireKeys with keyboards. 
  The package includes icons, screen shots,  descriptions, press releases and more.
</ul>
<br>
Additional information available at request. Contact us: <a href="mailto:custom@wiredplane.com">custom@wiredplane.com</a>
<?php }?>
<br><br>
<div id=h1>More info</div>
<lu>
<li><a href="../wirekeys/advantages.php">WireKeys advantages</a>
<li><a href="../wirekeys/custom.php">Custom development</a>
<li><a href="../wirekeys/oem.php">OEM package</a>
</lu>
<br><br>
<a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>
