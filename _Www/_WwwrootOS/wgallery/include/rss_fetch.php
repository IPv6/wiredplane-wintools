<?php
/************************************************************
RSS Fetch 0.4.2
RSS Feed Reader
Author: Drew Phillips
www.neoprogrammers.com
Copyright 2005 Drew Phillips

Please rate this script at http://www.hotscripts.com/rate/48456.html
Then it will remain popular and others will be able to find it easier.
Thanks

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.



************************************************************/



class rss_parser {
  var $update_interval = 60;    
  /* How often to fetch the rss file
     A cached version will be used between updates    */

  var $data_directory = "cep";
  /* Where to store the rss data from the feeds
     Note: an absolute path is better than a relative path here
     unless you plan on keeping the script to display the feeds
     in the same folder as this file and the feeds.   */



  /* NO NEED TO EDIT BELOW HERE UNLESS YOU KNOW WHAT YOU ARE DOING  */


  var $rss_url;
  var $num_to_show;
  var $do_update;
  var $tags = array();
  var $content;
  var $rss = array();

  var $feed_title;
  var $feed_link;
  var $feed_description;

  var $my_html;

  function rss_parser($url, $numtoshow = 10, $html = "", $update = 0)
  {
    $this->rss_url = $url;
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

    $filename = $url_parts['host'] . str_replace("/", ",", $url_parts['path']) . "_" . @$url_parts['query'];
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

    preg_match_all("/<item[^>]*>(.*?)<\/item>/s", $this->content, $items);
    if (sizeof($items[0]) == 0) {
      echo "No item elements found in rss feed.<br />\n";
    }

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
    if($this->my_html == "") {
      $this->show_html();
    } else {
      $this->show_user_html();
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
		/*
		if($i==2){		
			echo '<script type="text/javascript">';
			echo 'google_ad_client = "pub-5186671154037974";';
			echo 'google_ad_width = 120;';
			echo 'google_ad_height = 90;';
			echo 'google_ad_format = "120x90_0ads_al_s";';
			echo 'google_ad_channel ="";';
			echo 'google_color_border = "000000";';
			echo 'google_color_bg = "EEEEEE";';
			echo 'google_color_link = "0000FF";';
			echo 'google_color_url = "008000";';
			echo 'google_color_text = "000000";';
			echo '</script>';
			echo '<script type="text/javascript"';
			echo '  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">';
			echo '</script><hr>';
		}
		if($i==5){
			echo '<script type="text/javascript">';
			echo 'google_ad_client = "pub-5186671154037974";';
			//echo 'google_alternate_ad_url = "http://outer-station.m6.net/";';
			//echo 'google_alternate_color = "CCCCCC";';
			echo 'google_ad_width = 125;';
			echo 'google_ad_height = 125;';
			echo 'google_ad_format = "125x125_as";';
			echo 'google_ad_type = "text_image";';
			echo 'google_ad_channel ="";';
			echo 'google_color_border = "000000";';
			echo 'google_color_bg = "EEEEEE";';
			echo 'google_color_link = "0000FF";';
			echo 'google_color_url = "008000";';
			echo 'google_color_text = "000000";';
			echo '</script>';
			echo '<script type="text/javascript"';
			echo '  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">';
			echo '></script><hr>';
		}	
		*/
    }
  }
} // end class


?>



