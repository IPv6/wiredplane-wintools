<?php 
global $section;
function getTitle($l)
{
	global $language;
	return "WireNote - Lightweight PIM with Sticky notes, Reminders, LAN messenger and anti-spam filter";
}
function insertSection($l){
?>
<div id=h1>WireNote</div>
<?php if($l==1){?>
<b>Утилита для работы с Заметками/Списком ToDo/Напоминаниями/Адресной книгой и обмена сообщениями по сети (вместо WinPopup например)</b><br>
Программа предназначена для помощи в ежедневной работе за компьютером, раcполагается в трее и не требует к себе излишнего внимания. Сердцем программы является древовидная структура, в которой располагаются заметки, ToDo, напоминания и линки. 
Она в любой момент доступна либо из главного окна программы либо из меню в трее. Все это (заметки и пр.) можно создавать или одним нажатием мыши на иконку в трее или нажатием горячей клавиши (поддерживается клавиша "Win"). 
Все окна в WireNote "клеятся" друг к другу и краям экрана, что очень удобно. По каждой возможности этого приложения существует очень много настроек, доступных через удобный диалог.
<p>
Как уже упоминалось WireNote может использоваться вместо WinPopup - совместимых программ. Из-за наличия адресной книги вам не придется все время помнить имя компьютера вашего собеседника - просто занесите его в адресную книгу и пользуйтесь его именем для отсылки сообщений! Если у вашего собеседника также установлен WireNote, то Вы сможете передавать вместе с сообщениями файлы, если нет - то просто текстовые сообщения. Все сообщения логируются, также вы можете в любой момент переключится в on-line статус "Занят" или "Недоступен", соответственно в адресной книге видно кто из ваших собеседников не подключен к LAN в данный момент, а кто подключен, и готов ли он к общению :)<br>
<br><b>Заметка</b> - небольшое окно, в котором вы можете хранить текстовую информацию. Заметка может быть полупрозрачной, проявляясь только когда вы проводите над ней курсором мыши. Вы можете прикрепить заметку к какому либо окну - в этом случае заметка будет на экране только пока вы работаете с этим приложением.
<br><b>ToDo</b> - место хранения ваших идей, созданное ToDo легко доступно из меню в трее или из главного окна, не давая вам забыть о том что необходимо сделать.
<br><b>Напоминания</b> не позволят вам пропустить важную встречу или чей-то день рождения. При срабатывании напоминания WireNote может запустить программу, проиграть звук, выключить компьютер или послать сообщение по сети
<br><b>Сообщения</b> WireNote может быть использован для обмена сообщений по сети LAN, вместо Windows messaging service или WinPopup например.
Все сообщения оперируют удобными никами из адресной книги а не именами компьютеров (которые иногда довольно сложно запомнить).
WireNote автоматически следит за пользователями из адресной книги -кто в online, а кто нет. При отсылке/приеме сообщений можно использовать один из трех протоколов, можно обмениваться файлами, сообщению может быть назначена спец. иконка (Emotion), так что получивший ее сразу поймет какого рода информация содержится в сообщении еще даже не открывая его.
Кроме того вы можете создавать правила фильтрации сообщений, если например вам присылают много ненужной информации
<br> <b>Ссылки</b> Ссылки созданы для удобства доступа к часто используемым ресурсам или для совместного хранения документов вместе относящимися к ним ToDo или заметками
<br> <b>Анти-Спам (Удалятель Спама)</b> Мощный модуль по очистке Ваших почтовых ящиков от спама. Использует файлы спам-строк, совместимые с TheBAT!, с сервера скачиваются только заголовки сообщений, не сообщения целиком
(что ускоряет время проверки почты и очень полезно на слабых каналах). Убивает спам прямо на сервере. Полная поддержка различных кодировок почтовых сообщений
<br><br>
<hr><b>Возможности</b><br>
- Настраиваемый интерфейс<br>
- Все окошки "клеятся" друг к другу и краям экрана (как окна WinAmp-а)<br>
- Все элементы (заметки/ToDo) имеют свои настройки цвета текста/фона/шрифта<br>
- Удаления элементов происходит в специальную мусорную папку, из которой их потом при желании можно достать, максимальный размер папки ограничен (задается Вами)<br>
- Все введенные вами данные сохраняются между запусками программы (и даже чаще, заметки например сохраняются через 5 секунд неактивности, соответственно если ваша система вдруг упадет, вы ничего не потеряете)<br>
- Любое действие в WireNote может быть привязано к горячей клавише (с поддержкой клавиши "Win")<br>
<br><br>
<b>Меню в трее</b><br>
- В любой момент древовидная структура с заметками, напоминаниями и прочим доступна из меню у иконки в трее<br>
- Иконка настраивается, вы можете выбрать какую иконку вы хотите там видеть (организатора или центра сообщений), поверх иконки может быть расположена текущая дата или день недели и пр.<br>
- На клик с удерживанием Shift/Control/Alt может быть назначены отдельные действия (например Shift-клик - новая заметка, Alt-клик - новое напоминание и пр.)<br>
- Главное окно может автоматически показываться при наведении курсора на иконку<br>
<br><br>
<b>Главное окно</b><br>
- Поддерживает Drag&drop<br>
- у каждого элемента дерева - свое контекстное меню<br>
- Быстрая установка цветовой схемы или приоритета элемента<br> 
- Сортировка по любому столбцу<br>
- Переименование элемента можно осуществить просто дважды кликнув на него<br>
<br><br>
<b>ToDo</b><br>
- Время, потраченное на ToDo может быть записано и проанализировано, может быть получен отчет о сделанном за период<br>
- Связанные с ToDo заметки, прикрепленные к окнам, могут автоматически включать/выключать соответствующие ToDo<br>
- ToDo можно переслать по сети<br>
<br><br>
<b>Заметка</b><br>
- Заметка может быть свернута в компактный icon-mode, занимая на экране совсем немного места<br>
- Заметка может быть прикреплена к любому окну, она будет присутствовать на экране только когда вы работаете в соответствующем окне<br>
- Заметка может автоматически скрываться при проведении над ней курсора мыши (отменяется если нажат "Alt")<br>
<br><br>
<b>Напоминания</b><br>
- Напоминания могут быть повторяющимися (Например: каждый вторник, каждое второе число месяца, каждые два часа)<br>
- Напоминание может быть предварительно протестировано<br>
- Напоминание может активировать/деактивировать другие напоминания/ToDo, отсылать сообщения по сети LAN, запускать программы и т.п.<br>
- Напоминания могут быть общими между несколькими компьютерами с установленным WireNote<br>
- Напоминание можно переслать по сети<br>
<br>
<b>Сообщения</b><br>
- Можно приаттачить файл простым перетаскиванием его из других приложений<br>
- Сообщения от одного автора могут использовать одну и туже заметку, не плодя сообщений на экране<br>
- Поддерживается протокол, для работы которого Вам даже не нужно знать имя компьютера соеседника или видеть его в сети (единственное что нужно - общая сетевая папка)<br>
- WireNote может распознавать сообщения, отосланные от чужого имени<br>
<br>
<b>Центр сообщений/адресная книга</b><br>
- 4 online статуса (Online/Away/Busy/DND)<br>
- Включение статуса "Away" при запуске screensaver-а<br>
- Групповые пользователи (отсылка сообщения такому пользователю приведет к отсылке сообщения каждому члену группы)<br>
- Автоответчик (можно настроить так, что отвечать будет лишь тем пользователям, которые есть в вашей адресной книге)<br>
- Online-статусы пользователей из вашей адресной книги периодически проверяются<br>
<br>И все это совершенно бесплатно!
<?php }else{?>
This program was created to help in organization of daily work with such features as ToDo/Reminders and sticky notes service along with 
address book and LAN messenger. With WireNote, you can quickly create Sticky Notes and ToDos from the tray icon, 
the main window, or a hotkey. You can prioritize ToDos, and log and analyze the time spent on each with the ToDo report feature. 
<br>
You can schedule Reminders to play sounds, display Notes or ToDos, send messages to other users, start programs, or even shut down your computer. 
If WireNote is installed on more than one computer on a LAN, you can send Reminders to other users or store 
them in a shared folder making them visible to all users.
<p>The LAN messaging service works in conjunction with WireNote's address book. 
You can send messages through four different protocols -- Netbios, Mailslot, XPC, and Filebox. 
The system can even recognize fraudulent messages sent on behalf of other users.
<?php }?>
<div width=100% align=right><a href="/download/wirenote.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a></div>
<div id=h1>User Quotes</div>
<div id=ptext>Since I'm usually sitting in front of the computer, I needed something to actively remind me about appointments and other obligations not involving the computer. I searched for some sort of reminder program and found WireNote which fits the bill perfectly! I really love the option that will literally shake the window on the screen to get your attention.</div>
<div id=quote_author>Gary Sonnenberg</div>
<div id=ptext>I use WireNote like a virtual Post-It on my desk. It is very convenient, very easy to use. I use Note with transparency mode, in 'mini-mode'. I also use the reminder with Note, so I don`t forget anything anymore! Congratulation for this splendid software. Much better than any other software. PS: I found a small 'bug'. I told WirePlane about this issue. In less than 3 days, they "fixit". Most of the company does not even answer when told them their software gets a 'bug'. And you pay much more for something less efficient.</div>
<div id=quote_author>Woody</div>
<hr>
<table width=100%>
<tr align=center>
<td><a href='http://www.nonags.com' target=_blank><img src='/images2/organizations/nonags.gif' alt='6 of 6 at NoNags.com' border=0><br>NoNags.com - 6/6</a>
<td><a HREF='http://www.softpedia.com/' target=_blank><IMG BORDER=0 SRC='/images2/organizations/softped5.gif' alt='5 stars at SoftPedia.com'><br>SoftPedia - 5/5</a>
<td><a href='http://www.listsoft.ru/' path='?id=12461' target=_blank><img src='http://www.listsoft.ru/img/new/best.gif' width=80 height=60 border=0 alt='BEST!'><br>ListSoft - 5/5</a>
<td><A HREF='http://www.sharewareriver.com/' path='product.php?id=2297' target=_blank><IMG BORDER=0 SRC='/images2/organizations/sr5pt1.gif' WIDTH=90 HEIGHT=90 alt='5 stars at SHAREWARERiver'><br>SharewareRiver - 5/5</a>
</tr>
</table>
<hr>
<div id=h1>Special thanks</div>
Gary Sonnenberg for help in developing English version of this site,
<br>Agostino Cavallotto - official WiredPlane Italian translator,
<br>Www.rsdn.ru forum`s members for help,
<br>Bill Rucker for help with localization, 
<br>Vint, Alex Bulavin & Sergey Mushinsky for plugins, Plutonia Experiment, 2Los, AngelCode,
<br>Julia FAST for suggestions, Francisco Alvarado (Nihcap) and Timofey Kuzmenko and Jaroslav D. for helpful ideas,
<br>Wolfgang K. for German translation of WireKeys, Sergio Lanzone (Italian localization of WireChanger), Francisco Alvarado (Spanish localization of WireChanger and skins)
<br><br>
<?php 
}
?>
