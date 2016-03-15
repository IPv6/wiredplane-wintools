<?php 
function getTitle($l)
{
	global $language;
	return "Wind of change for your desktop. Add widgets, notes, interactive calendar and clocks on your desktop in a single click. Set reminders for future dates. Turn desktop into convinient PIM.";
}
function insertSection($l){
?>
<!--table border=0 width=98%><tr>
<td align=center><a href="screens.php"><img src="/images2/wirechanger/screens/wchanger6.jpg" width=150></a></td>
<td align=center><a href="screens.php"><img src="/images2/wirechanger/screens/wchanger7.jpg" width=150></a></td>
<td align=center><a href="screens.php"><img src="/images2/wirechanger/screens/wchanger10.jpg" width=150></a></td>
</tr></table><br>-->
<a href="screens.php"><img src="/images2/wirechanger/screens/wchanger10.jpg" width=150 style="float: left; margin-right: 0.5em;"></a>
<?php if($l==1){?>
<div id=h1>WireChanger - ветер перемен для Вашего десктопа!</div>
Если у Вас скопилась коллекция обоев, которой Вы хотите разнообразить свой десктоп, воспользуйтесь нашим десктоп-органайзером с возможностью наложения на изображения нетипичных эффектов!
В процессе развития программа приобрела множество дополнительных и полезных функций, 
таких как <b>календарь, заметки, часы</b>, показывающие реальное время и многое другое (см. далее).
Кроме стандартных типов файлов картинок (bmp, jpg, gif, анимированный gif и т.п.), программа работает с форматами TIFF, TGA, видео-файлами и с HTML страницами и адресами интернет 
(т.е. вместо картинки можно поместить скриншот интересующего Вас сайта, вся информация с которого берется в реальном времени).
WireChanger также может <b>работать с архивами</b> - Вы можете держать картинки в большом zip архиве и Вам не нужно его регулярно распаковывать 
для установки картинки на десктоп
<p>WireChanger также поможет Вам в повседневной работе - поверх обоев можно разместить полупрозрачный <b>календарь</b> (несколько внешних видов) 
и простым кликом на нем добавлять пометки к дням месяца, а также <b>напоминания</b> о важных датах, днях рождения и пр. 
На десктоп можно добавлять различные <b>Виджеты</b> - прогноз погоды для Вашего города, случайные 
афоризмы и анекдоты, свежие новости Ваших любимых новостных лент (<a href="/wirechanger/widgets_main.shtml">см. Шаблоны</a>). 
В отличие от аналогичных программ, виджеты WireChanger-а <b>не используют ресурсов процессора и памяти компьютера</b>!!!
<p>С помощью WireChanger вы сможете избавится от бумажных записей - программа позволяет добавлять забавные <b>заметки</b> 
прямо на десктоп простым двойным нажатием на свободное место картинки. 
Не нужно заходить в саму программу или нажимать специальную горячую клавишу - для добавления просто кликните 
дважды на том месте обоев, где бы вы хотели увидеть новую заметку. Такую заметку невозможно потерять и про нее 
невозможно забыть, так как она находится в легкодоступном месте. К заметке можно прикрепить <b>напоминание</b> 
и в указанное время программа сообщит Вам о предстоящем деле.
<br>
<?php }else{?>
<div id=h1>WireChanger - Wallpaper manager + widgets + PIM. Real wind of change for your desktop!</div>
This application is a wallpaper organizer that begins as a wallpaper changer but becomes 
much more with built-in utilities, templates, calendar and the option to add active clocks to 
your desktop. WireChanger offers you total and complete solution to 
manage your own desktop wallpapers, especially If you want to see something fresh every time 
you minimize your applications. You may never see the same picture twice on your desktop! 
The main idea of this software is to set images on wallpaper with additional 
random effect applied to background.
<p><font size=+1>All the major image formats supported</font> (gif, animated gif, jpg, bmp, png, wmf, ico, tiff, tga, video formats) and <b>image archives</b>. 
You can apply <b>special effects</b> to an image (for example, kaleidoscope, mosaic, emboss, water 
or color effects) on the fly, merge two images into one, artistically created from both. 
An additional great feature is the ability to show HTML pages or online URLs. And you can 
always get free wallpaper from our <a href="/wallpapers.shtml">ever-growing image gallery</a>. 
<p><font size=+1>Special widgets</font> can be used to color and alter your wallpaper. Widgets overlay your background with useful information 
such as a <b>desktop calendar</b>, the <b>weather forecast</b>, or <b>famous quotations</b>. The calendar widget transforms part 
or all of your background into a handy desktop calendar for two weeks or a current month to help you organize yourself. 
Just <b>double-click date</b>'s cell to add some notes for that day. You don't have to search icons in your tray or remember 
special hotkeys to popup a main window; just double-click your calendar wallpaper! 
You can also put customizable <b>post it notes</b> on your desktop with WireChanger. 
Calendar and notes can have <b>full-featured reminders</b> attached, 
use them to remind yourself about important dates, birthdays, etc. 
Unlike desktop widgets in similar programs (Konfabulator, Samurize, etc), our widgets 
are <b>not using precious CPU and memory resources</b> at all!
<br><br>
<?php }?>

<?php if($l==1){?>
<p>Кроме интерактивного календаря и заметок, Вам помогут организовывать Ваше время идущие в 
реальном времени <b>часы</b> (дополнительные часы могут быть загружены с галереи <a href="http://www.adamdorman.com/">AdamDorman</a>). 
Вы также можете создать интерактивный анимированный десктоп из Ваших Flash файлов используя встроенный в программу импорт Flash. 
Кроме этого программа может использоваться как скринсейвер, показывая Ваши картинки в режиме слайд-шоу.
<p>С программой удобно работать. К примеру есть возможность добавления <b>целиком каталога с картинками</b>. 
Можно добавить каталог не целиком, а только новые изображения. Добавленные картинки можно отсортировать по разрешению, размеру, имени, типу и пр.
Картинки автоматически группируются в темы, каждая тема имеет вес, который указывает как часто картинки этой темы будут использованы в смене обоев.
<p>WireChanger может автоматически синхронизировать содержимое темы с содержимым каталога на вашем диске - Вам не придется вручную 
добавлять новые поступления в Вашу коллекцию. Спецэффекты над картинками Вашей коллекции (например смешивание двух картинок из разных тем по разнообразным алгоритмам) еще больше разнообразят Ваш десктоп.
Эта программа специально предназначена для <b>больших коллекций</b> обоев. Для обладателей больших коллекций есть встроенная возможность поиска и удаления одинаковых картинок по содержанию (т.е. независимо от формата и разрешения). 
Теперь, глядя на десктоп, Вы всегда будете видеть что-то новое и интересное. Убедитесь сами!
<?php }else{?>
<font size=+1>Flash clocks</font>. WireChanger can also add clocks on top of your wallpaper 
from the biggest <a href="http://www.colclocks.com">ColClocks</a> collection or <a href="http://www.adamdorman.com/">AdamDorman</a>`s collection.
You can directly import any Flash file and use it as a nifty widget on your wallpaper. You can even use WireChanger as <b>Slide-show screensaver</b> and watch 
your favourite images (possibly with calendar or additional effects) endlessly.
<p><font size=+1>REALLY BIG collections</font>. This application was designed to handle huge collections of images, 
so it has all options needed, even handy "Duplicated Images finder and remover".
While most applications can only locate duplicate images of the same format, 
WireChanger will identify the same images even if one is a JPEG and another is a Bitmap image. 
Furthermore, WireChanger can identify duplicate images even if they have different sizes and 
resolution settings. Program's unique algorithm can even identify similar images.
<?php }?>
<br><br>
<a href="/download/wirechanger.zip"><img src="/images2/button_download.gif" width=148 height=40 vspace=10 hspace=10></a>
<a href="../commons/order.php"><img src="/images2/button_buy.gif" width=148 height=40 vspace=10 hspace=10></a>
<br><br>

<?php 
}
include("../cgi-bin/wp_layout.php");
printLayout("wc");
?>