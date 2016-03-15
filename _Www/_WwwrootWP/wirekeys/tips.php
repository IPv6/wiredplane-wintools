<?php 
function getTitle($l)
{
	global $language;
	if($l==1){
		return "Примеры использования WireKeys";
	}
	return "Examples of WireKeys usage";
}
function insertSection($l){
?>

<?php if($l==1){?>
<p>Пока еще не написано...
<?php }else{?>
<p>Under construction...
<?php }?>
<br><br>
<a href="/download/wirekeys.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wk");
?>
