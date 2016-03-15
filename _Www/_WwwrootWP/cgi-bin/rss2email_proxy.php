<?php
include "_util.php";
// Usage from pipe:
// http://pipes.yahoo.com/pipes/pipe.info?_id=JlKq55ic3BGfcOJ7JhOy0Q
// http://www.wiredplane.com/cgi-bin/rss2email_proxy.php?cc2email=wplabs@gmail.com&rsslink=http%3A//pipes.yahoo.com/pipes/pipe.run%3F_id%3DJlKq55ic3BGfcOJ7JhOy0Q%26_render%3Drss


$guid_cache_maxsize=10000;
$default_cc_from="rss2email_proxy@dev.null";
$url = _var("rsslink");
$cc2email = _var("cc2email");
$html = "rss";//"<table width=120 cellspacing=0 cellpadding=1><tr><td><a href='#{link}' target='_new'>#{title}</a></td></tr></table><img src='/images2/vmenu_line.gif' width=120 height=7>"; 
$show = 10;
$update = 1;
$hideAhowAddInfo=_var("noattaches");

class rss_parser {
  
  /* How often to fetch the rss file
     A cached version will be used between updates    */
  var $update_interval = 60;    
	 
  /* Where to store the rss data from the feeds
     Note: an absolute path is better than a relative path here
     unless you plan on keeping the script to display the feeds
     in the same folder as this file and the feeds.   */
  var $data_directory_base = ".";
  var $data_directory_subdircache = "rss2emailCache";

  /* NO NEED TO EDIT BELOW HERE UNLESS YOU KNOW WHAT YOU ARE DOING  */
  var $rss_url;
  var $cc2email;
  var $cc2emailFrom;
  var $data_directory;
  var $num_to_show;
  var $do_update;
  var $tags = array();
  var $content;
  var $rss = array();

  var $feed_title;
  var $feed_link;
  var $feed_description;

  var $my_html;
  
  var $cachefname;

  function rss_parser($url, $numtoshow = 10, $html = "", $update = 0, $cc2e = "", $cc2f = "")
  {
	global $default_cc_from;
    $this->rss_url = $url;
	$this->cc2email = $cc2e;
	$this->cc2emailFrom = $cc2f;
	if($this->cc2emailFrom==""){
		$this->cc2emailFrom=$default_cc_from;
	}
    $this->num_to_show = $numtoshow;
    $this->do_update = $update;
    $this->my_html = preg_replace("/(#{.*?):(.*?})/", "\\1__\\2", $html); //xx:xx tag workaround

    $this->content = $this->fetch_feed();
    $this->parse_feed();
    $this->show();
  }


  /* string */
  function fetch_feed()
  { 
    $url_parts = parse_url($this->rss_url);

	$this->cachefname=$data_directory_subdircache . "rss2email_proxy_" . $url_parts['host'] . str_replace("/", ",", $url_parts['path']) . "_" . @$url_parts['query'] .".rss";
    $filename = $this->cachefname;
    $this->data_directory = realpath($this->data_directory_base);
    if(file_exists($this->data_directory . "/$filename")) {
      $last = filemtime($this->data_directory . "/$filename");
      if(time() - $last > $this->update_interval * 60 || $this->update_interval == 0) {
        $update = 1;
      }
    } else {
        $create= 1;
    }

    if($create == 1 || ($this->do_update == TRUE && $update == 1)) {
      $fp = @fsockopen($url_parts['host'], 80, $errno, $errstr, 5);
      if (!$fp) {
        echo "Couldn't open rss feed {$this->feed_url} in {$_SERVER['PHP_SELF']}<br />\n";
        return;
      }

      fputs($fp, "GET " . $url_parts['path'] . "?" . @$url_parts['query'] . " HTTP/1.0\r\n"
                ."Host: " . $url_parts['host'] . "\r\n"
                ."User-Agent: Drew's RSS Reader 0.1\r\n"
                ."Connection: Close\r\n\r\n");

      while(!feof($fp)) {
        $rss_data .= @fgets($fp, 1024);
      }

      list(, $rss_data) = explode("\r\n\r\n", $rss_data, 2);

      $output = @fopen($this->data_directory . "/$filename", "w+");
      if(!$output) {
        return $rss_data;
      } else {
        flock($output, LOCK_EX);
        fputs($output, $rss_data);
        flock($output, LOCK_UN);
        fclose($output);
      }
    } //update

    return file_get_contents($this->data_directory . "/$filename");
  }

  /* void */
  function parse_feed()
  {
    preg_match("/<title>(.*?)<\/title>/", $this->content, $title);
    $this->feed_title = @$title[1];

    preg_match("/<link>(.*?)<\/link>/", $this->content, $link);
    $this->feed_link = @$link[1];

    preg_match("/<description>(.*?)<\/description>/", $this->content, $description);
    $this->feed_description = @$description[1];
	
	preg_match("/<pubDate>(.*?)<\/pubDate>/", $this->content, $pudate);
    $this->feed_pubDate = @$pudate[1];

	preg_match("/<lastBuildDate>(.*?)<\/lastBuildDate>/", $this->content, $lastBuildDate);
    $this->feed_lastBuildDate = @$lastBuildDate[1];

    preg_match_all("/<item[^>]*>(.*?)<\/item>/s", $this->content, $items);
    
    for($i = 0; $i < sizeof($items[0]); ++$i) {
      preg_match_all("/(?:<([\w:]*)[^>]*>\s*(?:<!\[CDATA\[)?(.*?)(?:]]>)?\s*<\/\\1>)+?/si", preg_replace("/<item[^>]*>/", "", $items[0][$i]), $elements);
      for($j = 0; $j < sizeof($elements[0]); ++$j) {
        $elements[1][$j] = str_replace(":", "__", $elements[1][$j]);  //regex fix for items with : like dc:date
        $this->rss[$i][$elements[1][$j]] = trim(html_entity_decode($elements[2][$j]));
      }
    }
  }
  
  /* void */
  function show()
  {
	if($this->my_html == "rss") {
		$this->show_rss();
    }else if($this->my_html == "") {
      $this->show_html();
    } else {
      $this->show_user_html();
    }
  }

  function show_rss()
  {
	global $guid_cache_maxsize;
	$sendedguids = "";
	$filenameRssCache=$this->data_directory . "/". $this->cachefname;
	$filename = $filenameRssCache. "email.log";
	if(file_exists($filename)){
		$openFile = fopen($filename,"r");
		$sendedguids = fread($openFile, min(filesize($filename),$guid_cache_maxsize));
		fclose($openFile);
	}
  
	header("Content-Type:·text/xml;charset=utf-8");
	echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
	echo "<rss version=\"2.0\">\r\n\r\n";
	echo "<channel>\r\n";
	echo "<!-- Rss from ".$this->rss_url." -->\r\n";
	if($this->cc2email != ""){
		echo "<br> Duplicating ".$this->rss_url." to email '".$this->cc2email."' \r\n";
		if($this->cc2emailFrom != ""){
			echo "<br> on behalf of '".$this->cc2emailFrom."' \r\n";
		}
		echo "<br> rss cache placed here: '".$filenameRssCache."' \r\n";
		echo "<br> guid cache placed here: '".$filename."' \r\n";
		echo "<hr>";
	}
	echo "<title><![CDATA[".$this->feed_title."]]></title>\r\n";
	echo "<link><![CDATA[".$this->feed_link."]]></link>\r\n";
	echo "<description><![CDATA[".$this->feed_description."]]></description>\r\n";
	echo "<pubDate>".$this->feed_pubDate."</pubDate>\r\n";
	echo "<lastBuildDate>".$this->feed_lastBuildDate."</lastBuildDate>\r\n";
	echo "\r\n\r\n";
  
    $show = (sizeof($this->rss)  > $this->num_to_show ? $this->num_to_show : sizeof($this->rss));
    for($i = 0; $i < $show; ++$i) {
		$itemguid=$this->rss[$i]['guid'];
		$itemdsc=$this->rss[$i]['description'];
		$itemtlt=$this->rss[$i]['title'];
		echo "<item>\r\n";
		if($this->cc2email != "" && $itemguid != "")
		{
			// Дублируем на почту!!!
			$itemguid2 = "|".$itemguid."|";
			if(stristr($sendedguids,$itemguid2)==false){
				$sendedguids = $itemguid2.$sendedguids;
				$e_to=stripslashes($this->cc2email);
				$e_from=stripslashes($this->cc2emailFrom);
				$e_subject=stripslashes($itemtlt);
				$e_message=stripslashes($itemdsc);
				//$e_message=$e_message."\n\nrequestjson=".json_encode($_REQUEST)."\n\n_SERVER=".json_encode($_SERVER);
				if(!@mail($e_to, $e_subject, $e_message, "From: $e_from\r\nContent-Type: text/html; charset=UTF-8\r\n")){
					if($hideAhowAddInfo!=1){
						$itemdsc.="\r\ncc2email:Failed to send this item!\r\n";
					}
				}else{
					if($hideAhowAddInfo!=1){
						$itemdsc.="\r\ncc2email:Item sent successfully!\r\n";
					}
				}
			}else{
				if($hideAhowAddInfo!=1){
					$itemdsc.="\r\ncc2email:Item was already sent!\r\n";
				}
			}
		}

		echo " <title><![CDATA[".$itemtlt."]]></title>\r\n";
		echo " <link>".$this->rss[$i]['link']."</link>\r\n";
		echo " <description type=\"text/html\"><![CDATA[".$itemdsc."]]></description>\r\n";
		echo " <guid>".$itemguid."</guid>\r\n";
		echo " <pubDate>".$this->rss[$i]['pubDate']."</pubDate>\r\n";
		echo "</item>\r\n";
		echo "\r\n";
    }
	echo "</channel>\r\n";
	echo "</rss>";
	if($sendedguids!=""){
		if(!file_exists($filename)){
			$createFile = fopen($filename, "w");
			fwrite ($createFile,"",0);
			@chmod($filename, 0666);
			fclose($createFile);
		}
		$openFile = fopen($filename,"w");
		fwrite($openFile,$sendedguids);
		fclose($openFile);
	}
  }
  
  function show_html()
  {
    $show = (sizeof($this->rss)  > $this->num_to_show ? $this->num_to_show : sizeof($this->rss));
    for($i = 0; $i < $show; ++$i) {
      echo "- <a href=\"{$this->rss[$i]['link']}\" target=\"_new\">{$this->rss[$i]['title']}</a><br />\n";
    }
  }

  function show_user_html()
  {
    $show = (sizeof($this->rss) > $this->num_to_show ? $this->num_to_show : sizeof($this->rss));
    for($i = 0; $i < $show; ++$i) {
		extract($this->rss[$i]);
		$item = preg_replace("/#\{([^}]+)}/e", "$\\1", $this->my_html);
		echo $item;
    }
  }
} // end class

// Запускаем чтение rss
$rss = new rss_parser($url, $show, $html, $update, $cc2email); 
?>



