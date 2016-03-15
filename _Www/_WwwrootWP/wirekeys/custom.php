<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "”правление системой с помощью гор. клавиш, полна€ автоматизаци€ работы за компьютером";
	}
	return "Automation tool for power users, Control your PC using hotkeys";
}
function insertSection($l){
?>

<p>We recognize the value of establishing partnerships with organizations and 
individuals and have tailored specific programs to meet your individual needs. 

<p>WireKeys significially improves speed and accuracy working
on a computer. Our users often ask us to add particular features to make
their productivity even better. Also many requests are initiated by
companies interested in business automation.

<p>If you are a computer manufacturer, check out our <a href="oem.shtml">Manufacturer Program</a>, 
which includes software tailoring and adaptation to specific characteristics. 
We can add your company logo into our product and customize the product for 
your other special needs. Our pricing structure is flexible.

<p>We offer custom automation solutions based on WireKeys
shortcut manager. We can update existing features according to your
requirements or build a separate version for your company with extensions
you need.

<p><h4>Advantages</h4>

<ul>
  <li><b>Short development cycle</b> - in the most cases we start working
      on your task on the same business day

  <li><b>No need for detailed specifications</b> - our managers
      can build detailed task specification based on your brief
      description and send it for your approval or corrections

  <li><b>Low cost services</b> - being located in Eastern Europe
      we offer low rates for custom development services

  <li><b>Reliability</b> - WiredPlane Labs develops automation software
      since 2001. Our experienced developers and advanced testing
      routines bring reliable solutions

  <li><b>Compatibility</b> - Solutions based on WireKeys work
      on all Windows versions since Windows 95 (95/98/ME/NT/2000/XP)
</ul>
<br>
Additional information available at request. Contact us: <a href="mailto:support@wiredplane.com">support@wiredplane.com</a>
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
