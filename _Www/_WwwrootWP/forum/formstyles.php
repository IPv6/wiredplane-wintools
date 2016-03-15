<?php
include ("global.php");
include ($dbpath."/settings/styles/styles.php");

?>

A {
     font-family= <?php echo $font; ?>;
}

P {
     font-family= <?php echo $font; ?>;
}

TD {
          font-size: <?php echo $fontsize; ?>;
          color: <?php echo $fontcolor; ?>;
          font-family: <?php echo $font; ?>;
}

.adminform{
	width:81%;
	height:399;
	background-color:<?php echo $menucolor; ?>;
	vertical-align: top;
}

.nobuttonshead{
	font-size: <?php echo $fontsize; ?>;
	color: <?php echo $forumheadermenufont; ?>;
	font-family: <?php echo $font; ?>;
}

.indexleft{
     width:5%;
     height:15px;
     background-color:<?php echo $fmenucolor; ?>;
     text-align:center;
     vertical-align:middle
}

.quote{
	background-color:<?php echo $quotebackground; ?>;
	text-align:left;
	vertical-align:middle;
	color: <?php echo $quotefontcolor; ?>;
	font-size: <?php echo $quotefontsize; ?>;
}

.subheader{
	background-image: url(<?php echo $subheadergif; ?>);
	background-color: <?php echo $subheadercolor; ?>;
	color: <?php echo $subheaderfont; ?>;
	vertical-align:middle;
	line-height: 12px;
}

.header{
	background-image: url(<?php echo $headergif; ?>);
	background-color: <?php echo $headercolor; ?>;
	vertical-align:middle;
	color: <?php echo $headerfont; ?>;
	font-weight: bold;
	font-size:  <?php echo $fontsize; ?>;
	line-height: 15px;
}

.label{
	background-color: <?php echo $fmenucolor; ?>;
	vertical-align:top;
	text-align: right;
	color: <?php echo $menufont; ?>;
	font-weight: normal;
	font-size:  <?php echo $fontsize; ?>;
	line-height: 12px;
	width:19%;
}

.content{
	background-color: <?php echo $menucolor; ?>;
	vertical-align:top;
	text-align: left;
	color: <?php echo $menufont; ?>;
	font-weight: normal;
	font-size:  <?php echo $fontsize; ?>;
	line-height: 12px;
	width:60%;
}

IMG{
	margin-left:auto;
	margin-right:auto;
	border-style: none;
}

.announcement{
     font-size: <?php echo $annfontsize; ?>;
     color: <?php echo $annfontcolor; ?>;
     font-family: <?php echo $annfont; ?>;
     vertical-align:middle;
     text-align:left;
}

A:link{
     text-decoration: none;
     color: <?php echo $linkcolor; ?>;
}

A:visited{
     text-decoration: none;
     color: <?php echo $vlinkcolor; ?>;
}

A:hover{
     text-decoration: none;
     color: <?php echo $hlinkcolor; ?>;
     font-style: normal;
     background-color: transparent;
     text-decoration: underline;
}

A.Buttoni:hover, A.Buttoni:link, A.Buttoni:visited, A.Buttoni:active {
FONT-WEIGHT: bold;
FONT-SIZE: 10px;
line-height: 2em;
vertical-align: middle;
COLOR: #FFFFFF;
FONT-FAMILY: Verdana, Arial, Helvetica, sans-serif;
//background: <?php echo $menucolor; ?>;
background: url(arg.gif) #778FA8 50%;
text-decoration:none;
TEXT-ALIGN: right;
padding:2px 8px;
border: 1px solid #000000;
white-space: nowrap;
cursor: hand, pointer;
}

