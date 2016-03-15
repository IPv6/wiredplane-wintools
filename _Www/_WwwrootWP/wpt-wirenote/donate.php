<?php 
function getTitle($l)
{
	global $language;
	return "WireNote - Lightweight PIM with Sticky notes, Reminders, LAN messenger and anti-spam filter";
}
function insertSection($l){
?>
<?php if($l==1){?>
<div id=h1>WireNote помогает природе! <a href="http://smskopilka.ru/?info&id=9527" title="На помощь Природе - в защиту леса! Откроется в новом окне" target="_blank">Помогите</a> нам в развитии программы!</div>
<br>
Используя эту программу для хранения записей и отправки сообщений по сети, Вы экономите бумагу на которой Вам пришлось бы писать то, что можно хранить в электронном виде.
Экономя бумагу Вы сохраняете лес, из которого ее делают. Не думайте что такой вклад невелик чтобы быть заметным - используя программу всего лишь несколько месяцев, Вы тем самым спасаете от вырубки новое молодое деревце!
Вы также можете порекомендовать программу своим друзьям и сослуживцам - каждый человек использующий WireNote для ведения заметок помогает истинным "легким" нашей природы - деревьям!
<br><br>Помогите и нам в дальнейшем развитии и распространении этой программы, <a href="http://smskopilka.ru/?info&id=9527" title="На помощь Природе - в защиту леса! Откроется в новом окне" target="_blank">просто послав смс</a>. 
Для этого нажмите на ссылку с копилкой и следуйте иструкциям на экране.
Для Вас эта смс будет стоить всего от $0.3 до $3 (на выбор) - в любом случае это весьма незначительная сумма 
для такого дела, как возможность помочь лесам и посильно (пусть и косвенно) защитить их от вырубки!
Если Вы хотите помочь другим способом оплаты, кликните по <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">этой ссылке</a>.
<br><br>
<a href="http://smskopilka.ru/?info&id=9527" title="На помощь Природе - в защиту леса! Откроется в новом окне" target="_blank"><img src="http://smskopilka.ru/iclient/9527/smskopilka.gif" border="0" alt="sms.копилка На помощь Природе - в защиту леса!"></a>
<br><br>
<?php }else{?>
<div id=h1>WireNote helps the trees! <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">Help</a> us too!</div>
<br>Using this program for storing of records and sending of messages over network, 
you economize paper on which it would be necessary to write otherwise.
Economizing paper you you preserve the forest, from which it makes. 
Do not think that this contribution is small in order to be noticeable - using a program only several months, 
you thus save from the cutting down new young sapling!
You also can recommend program to your friends and colleagues - each person treating WireNote 
for conducting the notes helps the true "lungs" of our nature - trees!
<br><br>
If you like WireNote and wish us to continue development of this product, please help us and 
consider to <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">donate</a> our team with a small amout of money. Donations can be done here: <a href="https://usd.swreg.org/cgi-bin/s.cgi?s=33087&p=33087WN&v=0&d=0&q=1&t=">Visa, PayPal, other options available</a>
<br>Your aid will actually help nature (Indirectly, but your aid IS real!)
<br><br>
<?php }?>
<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("common");
?>
