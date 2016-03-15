<?php 
/*
Набор скриптов для использования кодогенератора ключей ExeCryptor в ручном режиме и для регистраторов SWReg и RegNow

Разное:
- тут нет поддержки создания нескольких ключей за раз. У SWRegа есть опция "вызывать генератор для каждой купленной лицензии", для RegNow необходимо дорабатывать самостоятельно
- регистрационное имя автоматически переводится в нижний регистр перед отсылкой генератору. 
- В качестве регистрационного имени используется EMAIL клиента, как наиболее формализованное данное при любых ситуациях. Если Вы хотите использовать другие рег. имена, поправьте помеченные строки в соотв. файлах.

Настройки кодогенератора на http://www.strongbit.com/
- кодогенератор должен быть настроен так:Registration Name 1=%regname% Required fields=%regname% Reply template=%!SERNUM%
- по умолчанию ключи (копии, на всякий случай) сохраняются в файл в доступном из web каталоге, необходимо или его защитить на уровне сервера или прописать пути за пределы доступного из web пространства
- сгенеренный ключ сверяется со строкой ERROR на предмет неудачи генерации, поэтому нужно чтобы кодогенератор в случае ошибки возвращал именно эту строку. Для этого нужно в настройках лицензии выбрать "код 200" при ошибке и в поле ввода написать ERROR

Настройки регистраторов
- Для СВРега и Регнау используются не все, но самые информативные поля. За подробностями того, какую информацию могут передавать кодогенератору см. соответсвующие хелпы
- у СВРЕГА путь к кейгену прописывается в настройках доставки продукта (за кнопкой редактирования метода доставки и нажатия на edit email). Путь к кейгену будет выглядеть так: http://www.yoursite.com/keygen_app/make_swreg.php
- у РегНау путь к кодогенератору проще переписать через XML файл настройки продукта: XML Files->Настройка продукта (из списка кодов ваших продуктов)->Секцию <delivery>...</delivery> заменяете на
<delivery>
 <deliver-license>
  <generate-license-code>
   <script>
   <define-parameter name="license_name"/>
   <return>
   <remote.connect method="post" url="http://www.yoursite.com/keygen_app/make_regnow.php"> 
    <remote.php-value name="what"><static value="wkcode"/></remote.php-value>
    <remote.php-value name="date"><date format="mm/dd/yyyy"/></remote.php-value>
    <remote.php-value name="orderid"><value-of class="orderitem" field="id"/></remote.php-value>
    <remote.php-value name="item"><value-of class="orderitem" field="product"/></remote.php-value>
    <remote.php-value name="quantity"><value-of class="orderitem" field="quantity"/></remote.php-value>
    <remote.php-value name="fname"><value-of class="recipient" field="fname"/></remote.php-value>
    <remote.php-value name="lname"><value-of class="recipient" field="lname"/></remote.php-value>
    <remote.php-value name="company"><value-of class="recipient" field="company"/></remote.php-value>
    <remote.php-value name="add1"><value-of class="recipient" field="add1"/></remote.php-value>
    <remote.php-value name="add2"><value-of class="recipient" field="add2"/></remote.php-value>
    <remote.php-value name="city"><value-of class="recipient" field="city"/></remote.php-value>
    <remote.php-value name="state"><value-of class="recipient" field="state"/></remote.php-value>
    <remote.php-value name="zip"><value-of class="recipient" field="zip"/></remote.php-value>
    <remote.php-value name="country"><value-of class="recipient" field="country"/></remote.php-value>
    <remote.php-value name="phone"><value-of class="recipient" field="phone"/></remote.php-value>
    <remote.php-value name="email"><value-of class="recipient" field="email"/></remote.php-value>
    <remote.php-value name="total"><value-of class="orderitem" field="total"/></remote.php-value>
   </remote.connect>
   </return>
   </script>
  </generate-license-code> 
 </deliver-license>
</delivery>
соответсвенно правите в нем УРЛ
Путь к кейгену будет выглядеть так:
http://www.yoursite.com/keygen_app/make_regnow.php

- для ручной генерации ключа путь к форме будет выглядеть так:
http://www.yoursite.com/keygen_app/make.php
Учтите, что все равно придется вводить пароль (прописанный в лицензии на http://www.strongbit.com/), в целях безопасности
*/

// Настройки
// Название программы
$appName="WireChanger";
// Путь куда сохранять файлы с информацией о сгенерированных ключах (со слешем на конце!)
$savekeysto="";
// Путь к шаблону emailа клиенту
$emailtempl="kg_emailtempl.txt";
// Посылать ли копию письма клиенту еще куда нибудь (Вам в частности). Если пусто - не посылать
$sendcopyto="support@wiredplane.com";
// Какой адрес подставлять в replyto письма клиенту (чтобы при ответе оно отсылалось куда надоб вам в саппорт)
$replyto="support@wiredplane.com";
// Путь к кейгену. Пароль и параметр запроса "regname" добавяться автоматическиб тут их указывать не надо
$keygenurl="http://www.strongbit.com/keygen/getkey.asp?_Login=Razinkov%20Ilya&_License=WireChanger.Main.lic";
// Специальная строка, добавляемая к регистрационному имени (соответсвующую строку нужно прибвлять и перед проверкой  рег. ключа)
// Сделано на всякий случай, в большинстве случаев проще оставить пустым
$salt="WC5";
// Пароль, передаваемый кодогенератору (тот, что устанавливается в настройках лицензии). При ручной генерации он все равно вводится самостоятельно
$kgpsw="imurkola";

?>