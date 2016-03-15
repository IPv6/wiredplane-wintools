function WhichClicked(ww) {
	window.document.postmodify.waction.value = ww;
}

function submitonce(theform) {
	// if IE 4+ or NS 6+
	if (document.all || document.getElementById) {
		// hunt down "submit" and "reset"
		for (i=0;i<theform.length;i++) {
			var tempobj=theform.elements[i];
			if(tempobj.type.toLowerCase()=="submit"||tempobj.type.toLowerCase()=="reset") {
				//disable it
				tempobj.disabled=true;
			}
		}
	}
}

function storeCaret(text) { 
	if (document.selection && document.selection.createRange) {
		text.caretPos=document.selection.createRange().duplicate();
	}
}

function insertText(element,text) 
{ 
  if (element && element.caretPos) element.caretPos.text=element.caretPos.text.charAt(element.caretPos.text.length - 1) == ' ' ? text + ' ' : text; 
  else if (element && element.selectionStart+1 && element.selectionEnd+1) 
          element.value=element.value.substring(0,element.selectionStart)+text+element.value.substring(element.selectionEnd,element.value.length); 
  else if (element) element.value+=text;
  element.focus(); 
}

function AddText(pbopen, pbclose) {
	 if (document.selection && document.selection.createRange){
		sel = document.selection.createRange().text;
		if (!sel){
			insertText(document.postmodify.message, pbopen + pbclose);
			return;
		}
		else{
			document.selection.createRange().text = pbopen + sel + pbclose;
			document.postmodify.message.focus();
			return;
		}
	}
	else document.postmodify.message.value += pbopen+pbclose;  
}

function hr() {
	AddTxt="[hr]";
	AddText('',AddTxt);
}

function size() {
	AddTxt="[size=2][/size]";
	AddText(AddTxt);
}

function font() {
	AddTxt="[font=Verdana][/font]";
	AddText(AddTxt);
}

function teletype() {
	AddTxt="[tt][/tt]";
	AddText(AddTxt);
}

function right() {
	AddTxt="[right][/right]";
	AddText(AddTxt);
}

function left() {
	AddTxt="[left][/left]";
	AddText(AddTxt);
}

function superscript() {
	AddTxt="[sup][/sup]";
	AddText(AddTxt);
}

function subscript() {
	AddTxt="[sub][/sub]";
	AddText(AddTxt);
}

function image() {
	AddText('[img]', '[/img]');
}

function ftp() {
	AddTxt="[ftp][/ftp]";
	AddText(AddTxt);
}

function move() {
	AddTxt="[move]STUFF[/move]";
	AddText(AddTxt);
}

function shadow() {
	AddTxt="[shadow=red,left,300]TEXT[/shadow]";
	AddText(AddTxt);
}

function glow() {
	AddTxt="[glow=red,2,300]TEXT[/glow]";
	AddText(AddTxt);
}
function glowy() {
	AddText('[glowred]', '[/glowred]');
}

function flash() {
	AddTxt="[flash=200,200]URL[/flash]";
	AddText(AddTxt);
}

function pre() {
	AddTxt="[pre][/pre]";
	AddText(AddTxt);
}

function tcol() {
	AddTxt="[td][/td]";
	AddText(AddTxt);
}

function trow() {
	AddTxt="[tr][/tr]";
	AddText(AddTxt);
}

function table() {
	AddTxt="[table][tr][td][/td][/tr][/table]";
	AddText(AddTxt);
}

function strike() {
	AddTxt="[s][/s]";
	AddText(AddTxt);
}

function underline() {
	AddTxt="[u][/u]";
	AddText(AddTxt);
}

function emai1() {
	AddText('[email]', '[/email]');
}

function bold() {
	AddText('[b]', '[/b]');
}

function italicize() {
	AddText('[i]', '[/i]');
}

function quote() {
	AddText('[quote]','[/quote]');
}

function center() {
	AddTxt="[center][/center]";
	AddText(AddTxt);
}

function hyperlink() {
	AddText('[url]', '[/url]');
}

function showcode() {
	AddTxt="[code][/code]";
	AddText(AddTxt);
}

function list() {
	AddTxt="[list][*][*][*][/list]";
	AddText(AddTxt);
}

function showcolor(color) {
	AddTxt="[color="+color+"][/color]";
	AddText(AddTxt);
}



function smiley() {
	AddText('', ' :)');
}

function wink() {
	AddText('', ' ;)');
}

function cheesy() {
	AddTxt=" :D";
	AddText('', AddTxt);
}

function grin() {
	AddTxt=" ;D";
	AddText('', AddTxt);
}

function angry() {
	AddTxt=" :@";
	AddText('', AddTxt);
}

function sad() {
	AddTxt=" :(";
	AddText('', AddTxt);
}

function shocked() {
	AddTxt=" :o";
	AddText('', AddTxt);
}

function cool() {
	AddTxt=" 8)";
	AddText('', AddTxt);
}

function huh() {
	AddTxt=" :?";
	AddText('', AddTxt);
}

function rolleyes() {
	AddTxt=" *)";
	AddText('', AddTxt);
}

function tongue() {
	AddTxt=" :P";
	AddText('', AddTxt);
}
function evil() {
	AddTxt=" :>";
	AddText('', AddTxt);
}
function em() {
	AddTxt=" :emb(";
	AddText('', AddTxt);
}
function goof() {
	AddTxt=" :!!";
	AddText('', AddTxt);
}


function lipsrsealed() {
	AddTxt=" :-X";
	AddText('', AddTxt);
}

function embarassed() {
	AddTxt=" :-[";
	AddText('', AddTxt);
}

function undecided() {
	AddTxt=" :-/";
	AddText('', AddTxt);
}

function kiss() {
	AddTxt=" :-*";
	AddText('', AddTxt);
}

function cry() {
	AddTxt=" :'(";
	AddText('', AddTxt);
}

//new smilies

function bad() {
	AddTxt=" :bad";
	AddText('', AddTxt);
}

function bounce() {
	AddTxt=" :bounce";
	AddText('', AddTxt);
}

function dance() {
	AddTxt=" :dance";
	AddText('', AddTxt);
}

function devil() {
	AddTxt=" :devil";
	AddText('', AddTxt);
}

function duell() {
	AddTxt=" :duell";
	AddText('', AddTxt);
}

function eatthis() {
	AddTxt=" :eatthis";
	AddText('', AddTxt);
}

function einaug() {
	AddTxt=" :1eye";
	AddText('', AddTxt);
}

function qmark() {
	AddTxt=" :qmark";
	AddText('', AddTxt);
}

function glad() {
	AddTxt=" :glad";
	AddText('', AddTxt);
}

function gluehb() {
	AddTxt=" :bulb";
	AddText('', AddTxt);
}

function grins() {
	AddTxt=" :grins";
	AddText('', AddTxt);
}

function help() {
	AddTxt=" :help";
	AddText('', AddTxt);
}

function jump() {
	AddTxt=" :jump";
	AddText('', AddTxt);
}


function offtopic() {
	AddTxt=" :1offtopic";
	AddText('', AddTxt);
}

function pfeilr() {
	AddTxt=" :arrr";
	AddText('', AddTxt);
}

function redface2() {
	AddTxt=" :redface2";
	AddText('', AddTxt);
}

function rufz() {
	AddTxt=" :exclm";
	AddText('', AddTxt);
}

function sad() {
	AddTxt=" :sad";
	AddText('', AddTxt);
}

function schimpf() {
	AddTxt=" :scold";
	AddText('', AddTxt);
}

function schluck() {
	AddTxt=" :oops";
	AddText('', AddTxt);
}

function sleep() {
	AddTxt=" :sleep";
	AddText('', AddTxt);
}

function sungl() {
	AddTxt=" :sungl";
	AddText('', AddTxt);
}

function thmbdn() {
	AddTxt=" :thmbdn";
	AddText('', AddTxt);
}

function thmbup() {
	AddTxt=" :thmbup";
	AddText('', AddTxt);
}

function traene() {
	AddTxt=" :tear";
	AddText('', AddTxt);
}

function traurig() {
	AddTxt=" :sad1";
	AddText('', AddTxt);
}

function ueberr() {
	AddTxt=" :surpr";
	AddText('', AddTxt);
}

function wallbash() {
	AddTxt=" :wallbash";
	AddText('', AddTxt);
}

function zorn() {
	AddTxt=" :anger";
	AddText('', AddTxt);
}
function nextavatar() {
        var i = document.theform.avatar.selectedIndex;
 if (i==23) { i=-1; }
 document.theform.avatar.selectedIndex = ++i;
 document.images['avatarpicture'].src =
avatarpics[document.theform.avatar.selectedIndex]; }
function previousavatar() {
 var i = document.theform.avatar.selectedIndex-1;
 if (i==-1) { i=23 }
 document.theform.avatar.selectedIndex = i;
 document.images['avatarpicture'].src =
avatarpics[document.theform.avatar.selectedIndex]; }

