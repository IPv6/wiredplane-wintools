<?php
header("Content-type: text/html; charset=UTF-8");
?>
<html>
<head><meta HTTP-EQUIV="content-type" CONTENT="text/html; charset=UTF-8">
<title>Translation station</title>
</head>
<body>
<center>
<?php
print("<form action=http://babelfish.altavista.com/babelfish/tr name=mfrm accept-charset='UTF-8'>	  ");
print("<input type=hidden name=doit value=done>				    ");
print("<table width=200 border=0 cellspacing=0 cellpadding=6 bgcolor=#93b2dd><tr>   ");
print("	<th colspan=2 bgcolor=#FFFFFF><a href=http://www.altavista.com>    ");
print("	<img src=http://us.i1.yimg.com/us.yimg.com/i/us/av/i/af/box_logo.gif border=0 width=118 height=45 alt=AltaVista></a><br> ");
print("	</th></tr>								");
print("	<tr><td colspan=2><img src=http://us.i1.yimg.com/us.yimg.com/i/us/av/i/af/bfishheading.gif width=192 height=20 alt='Babel Fish Translation'><br><font size=2 face=arial,helvetica,sans-serif color=#FFFFFF>	 ");
print("	<small>Type or Paste text or Web address<br> (beginning with http://) here:<br>	</small>	");
print("	<textarea cols=20 rows=2 name=urltext></textarea>  	");
print("	</td></tr>								");
print("	<tr><td colspan=2><font face=verdana,arial,helvetica,sans-serif size=2 color=#FFFFFF><small>Translate from:<br></small></font>");
print("	<select name=lp>					");
print("	<option value=zh_en>Chinese-simp to English	");
print("	<option value=zt_en>Chinese-trad to English	");
print("	<option value=nl_en>Dutch to English		");
print("	<option value=nl_fr>Dutch to French		");
print("	<option value=en_zh>English to Chinese-simp	");
print("	<option value=en_zt>English to Chinese-trad	");
print("	<option value=en_nl>English to Dutch		");
print("	<option value=en_fr>English to French		");
print("	<option value=en_de>English to German		");
print("	<option value=en_el>English to Greek		");
print("	<option value=en_it>English to Italian		");
print("	<option value=en_ja>English to Japanese	");
print("	<option value=en_ko>English to Korean		");
print("	<option value=en_pt>English to Portuguese	");
print("	<option value=en_es>English to Spanish		");
print("	<option value=en_ru>English to Russian		");
print("	<option value=fr_nl>French to Dutch		");
print("	<option value=fr_de>French to German		");
print("	<option value=fr_el>French to Greek		");
print("	<option value=fr_it>French to Italian		");
print("	<option value=fr_pt>French to Portuguese	");
print("	<option value=fr_es>French to Spanish		");	       
print("	<option value=fr_en>French to English		");
print("	<option value=de_en>German to English		");
print("	<option value=de_fr>German to French		");
print("	<option value=el_en>Greek to English		");
print("	<option value=el_fr>Greek to French		");
print("	<option value=it_en>Italian to English		");
print("	<option value=it_fr>Italian to French		");
print("	<option value=ja_en>Japanese to English	");
print("	<option value=ko_en>Korean to English		");
print("	<option value=pt_en>Portuguese to English	");
print("	<option value=pt_fr>Portuguese to French	");
print("	<option value=ru_en>Russian to English		");
print("	<option value=es_en>Spanish to English		");	       
print("	<option value=es_fr>Spanish to French		");	       
print("	</select></font></td></tr><tr>				");
print("	<td><input type=submit value=Translate  style=font-family:sans-serif;font-weight:bold;color:#FFF;background-color:#990000;cursor:hand;margin-bottom:-1px;width:85px;></td> ");
print("	<td><font face=arial,helvetica,sans-serif size=2 color=#FFFFFF><small>Powered by Systran</small></font></td>     ");
print("</tr></form></table>	 ");
print('<script type="text/javascript">');
print('if (window.yzq_a == null) document.write("<scr" + "ipt type=text/javascript src=http://us.js2.yimg.com/us.js.yimg.com/lib/bc/bc_1.7.3.js></scr" + "ipt>");');
print('</script><script type="text/javascript">');
print("if (window.yzq_a) yzq_a('p', 'P=Qw7R5NibyJuLAwS.2hL.yQFh1VdWHUSLOKUACJ5k&T=13phicvoi%2fX%3d1149974693%2fE%3d396561044%2fR%3davus%2fK%3d5%2fV%3d1.1%2fW%3d9%2fY%3dYAHOO%2fF%3d279521683%2fS%3d1%2fJ%3dA9C89BD8');");
print('if (window.yzq_gb && window.yzq4) yzq4();else if (window.yzq_eh) yzq_eh();</script>');
?>
</center>
</body>
</html>
