<?php

//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Hartmut Holzgraefe <hholzgra@php.net>                       |
// |          Christian Stocker <chregu@bitflux.ch>                       |
// +----------------------------------------------------------------------+
//
// $Id: Server.php,v 1.28 2005/04/05 22:51:09 hholzgra Exp $
//

/**
 * helper class for parsing PROPFIND request bodies
 * 
 * @package HTTP_WebDAV_Server
 * @author Hartmut Holzgraefe <hholzgra@php.net>
 * @version 0.99.1dev
 */
class _parse_propfind 
{
	/**
	 * success state flag
	 *
	 * @var bool
	 * @access public
	 */
	var $success = false;

	/**
	 * found properties are collected here
	 *
	 * @var array
	 * @access public
	 */
	var $props = false;

	/**
	 * internal tag nesting depth counter
	 *
	 * @var int
	 * @access private
	 */
	var $depth = 0;

	
	/**
	 * constructor
	 *
	 * @access public
	 */
	function _parse_propfind($path) 
	{
		// success state flag
		$this->success = true;
		
		// property storage array
		$this->props = array();

		// internal tag depth counter
		$this->depth = 0;

		// remember if any input was parsed
		$had_input = false;

		// open input stream
		$f_in = fopen($path, "r");
		if (!$f_in) {
			$this->success = false;
			return;
		}

		// create XML parser
		$xml_parser = xml_parser_create_ns("UTF-8", " ");

		// set tag and data handlers
		xml_set_element_handler($xml_parser,
								array(&$this, "_startElement"),
								array(&$this, "_endElement"));

		// we want a case sensitive parser
		xml_parser_set_option($xml_parser, 
							  XML_OPTION_CASE_FOLDING, false);


		// parse input
		while($this->success && !feof($f_in)) {
			$line = fgets($f_in);
			if (is_string($line)) {
				$had_input = true;
				$this->success &= xml_parse($xml_parser, $line, false);
			}
		} 
		
		// finish parsing
		if($had_input) {
			$this->success &= xml_parse($xml_parser, "", true);
		}

		// free parser
		xml_parser_free($xml_parser);
		
		// close input stream
		fclose($f_in);

		// if no input was parsed it was a request
		if(!count($this->props)) $this->props = "all"; // default
	}
	

	/**
	 * start tag handler
	 * 
	 * @access private
	 * @param  resource  parser
	 * @param  string    tag name
	 * @param  array     tag attributes
	 */
	function _startElement($parser, $name, $attrs) 
	{
		// name space handling
		if (strstr($name, " ")) {
			list($ns, $tag) = explode(" ", $name);
			if ($ns == "")
				$this->success = false;
		} else {
			$ns = "";
			$tag = $name;
		}

		// special tags at level 1: <allprop> and <propname>
		if ($this->depth == 1) {
			if ($tag == "allprop")
				$this->props = "all";

			if ($tag == "propname")
				$this->props = "names";
		}

		// requested properties are found at level 2
		if ($this->depth == 2) {
			$prop = array("name" => $tag);
			if ($ns)
				$prop["xmlns"] = $ns;
			$this->props[] = $prop;
		}

		// increment depth count
		$this->depth++;
	}
	

	/**
	 * end tag handler
	 * 
	 * @access private
	 * @param  resource  parser
	 * @param  string    tag name
	 */
	function _endElement($parser, $name) 
	{
		// here we only need to decrement the depth count
		$this->depth--;
	}
}


/**
 * helper class for parsing PROPPATCH request bodies
 * 
 * @package HTTP_WebDAV_Server
 * @author Hartmut Holzgraefe <hholzgra@php.net>
 * @version 0.99.1dev
 */
class _parse_proppatch 
{
    /**
     *
     * 
     * @var
     * @access
     */
    var $success;

    /**
     *
     * 
     * @var
     * @access
     */
    var $props;

    /**
     *
     * 
     * @var
     * @access
     */
    var $depth;

    /**
     *
     * 
     * @var
     * @access
     */
    var $mode;

    /**
     *
     * 
     * @var
     * @access
     */
    var $current;

    /**
     * constructor
     * 
     * @param  string  path of input stream 
     * @access public
     */
    function _parse_proppatch($path) 
    {
        $this->success = true;

        $this->depth = 0;
        $this->props = array();
        $had_input = false;

        $f_in = fopen($path, "r");
        if (!$f_in) {
            $this->success = false;
            return;
        }

        $xml_parser = xml_parser_create_ns("UTF-8", " ");

        xml_set_element_handler($xml_parser,
                                array(&$this, "_startElement"),
                                array(&$this, "_endElement"));

        xml_set_character_data_handler($xml_parser,
                                       array(&$this, "_data"));

        xml_parser_set_option($xml_parser,
                              XML_OPTION_CASE_FOLDING, false);

        while($this->success && !feof($f_in)) {
            $line = fgets($f_in);
            if (is_string($line)) {
                $had_input = true;
                $this->success &= xml_parse($xml_parser, $line, false);
            }
        } 
        
        if($had_input) {
            $this->success &= xml_parse($xml_parser, "", true);
        }

        xml_parser_free($xml_parser);

        fclose($f_in);
    }

    /**
     * tag start handler
     *
     * @param  resource  parser
     * @param  string    tag name
     * @param  array     tag attributes
     * @return void
     * @access private
     */
    function _startElement($parser, $name, $attrs) 
    {
        if (strstr($name, " ")) {
            list($ns, $tag) = explode(" ", $name);
            if ($ns == "")
                $this->success = false;
        } else {
            $ns = "";
            $tag = $name;
        }

        if ($this->depth == 1) {
            $this->mode = $tag;
        } 

        if ($this->depth == 3) {
            $prop = array("name" => $tag);
            $this->current = array("name" => $tag, "ns" => $ns, "status"=> 200);
            if ($this->mode == "set") {
                $this->current["val"] = "";     // default set val
            }
        }

        if ($this->depth >= 4) {
            $this->current["val"] .= "<$tag";
            foreach ($attr as $key => $val) {
                $this->current["val"] .= ' '.$key.'="'.str_replace('"','&quot;', $val).'"';
            }
            $this->current["val"] .= ">";
        }

        

        $this->depth++;
    }

    /**
     * tag end handler
     *
     * @param  resource  parser
     * @param  string    tag name
     * @return void
     * @access private
     */
    function _endElement($parser, $name) 
    {
        if (strstr($name, " ")) {
            list($ns, $tag) = explode(" ", $name);
            if ($ns == "")
                $this->success = false;
        } else {
            $ns = "";
            $tag = $name;
        }

        $this->depth--;

        if ($this->depth >= 4) {
            $this->current["val"] .= "</$tag>";
        }

        if ($this->depth == 3) {
            if (isset($this->current)) {
                $this->props[] = $this->current;
                unset($this->current);
            }
        }
    }

    /**
     * input data handler
     *
     * @param  resource  parser
     * @param  string    data
     * @return void
     * @access private
     */
    function _data($parser, $data) {
        if (isset($this->current)) {
            $this->current["val"] .= $data;
        }
    }
}


/**
 * helper class for parsing LOCK request bodies
 * 
 * @package HTTP_WebDAV_Server
 * @author Hartmut Holzgraefe <hholzgra@php.net>
 * @version 0.99.1dev
 */
class _parse_lockinfo 
{
	/**
	 * success state flag
	 *
	 * @var bool
	 * @access public
	 */
	var $success = false;

	/**
	 * lock type, currently only "write"
	 *
	 * @var string
	 * @access public
	 */
	var $locktype = "";

	/**
	 * lock scope, "shared" or "exclusive"
	 *
	 * @var string
	 * @access public
	 */
	var $lockscope = "";

	/**
	 * lock owner information
	 *
	 * @var string
	 * @access public
	 */
	var $owner = "";

	/**
	 * flag that is set during lock owner read
	 *
	 * @var bool
	 * @access private
	 */
	var $collect_owner = false;
	
	/**
	 * constructor
	 *
	 * @param  string  path of stream to read
	 * @access public
	 */
    function _parse_lockinfo($path) 
	{
		// we assume success unless problems occur
		$this->success = true;

		// remember if any input was parsed
		$had_input = false;
		
		// open stream
		$f_in = fopen($path, "r");
		if (!$f_in) {
			$this->success = false;
			return;
		}

		// create namespace aware parser
		$xml_parser = xml_parser_create_ns("UTF-8", " ");

		// set tag and data handlers
		xml_set_element_handler($xml_parser,
								array(&$this, "_startElement"),
								array(&$this, "_endElement"));
		xml_set_character_data_handler($xml_parser,
									   array(&$this, "_data"));

		// we want a case sensitive parser
		xml_parser_set_option($xml_parser,
							  XML_OPTION_CASE_FOLDING, false);

		// parse input
		while($this->success && !feof($f_in)) {
			$line = fgets($f_in);
			if (is_string($line)) {
				$had_input = true;
				$this->success &= xml_parse($xml_parser, $line, false);
			}
		} 

		// finish parsing
		if($had_input) {
			$this->success &= xml_parse($xml_parser, "", true);
		}

		// check if required tags where found
		$this->success &= !empty($this->locktype);
		$this->success &= !empty($this->lockscope);

		// free parser resource
		xml_parser_free($xml_parser);

		// close input stream
		fclose($f_in);		
	}
    

	/**
	 * tag start handler
	 *
	 * @param  resource  parser
	 * @param  string    tag name
	 * @param  array     tag attributes
	 * @return void
	 * @access private
	 */
    function _startElement($parser, $name, $attrs) 
    {
		// namespace handling
        if (strstr($name, " ")) {
            list($ns, $tag) = explode(" ", $name);
        } else {
            $ns = "";
            $tag = $name;
        }
		
  
        if ($this->collect_owner) {
			// everything within the <owner> tag needs to be collected
            $ns_short = "";
            $ns_attr = "";
            if ($ns) {
                if ($ns == "DAV:") {
                    $ns_short = "D:";
                } else {
                    $ns_attr = " xmlns='$ns'";
                }
            }
            $this->owner .= "<$ns_short$tag$ns_attr>";
        } else if ($ns == "DAV:") {
			// parse only the essential tags
            switch ($tag) {
			case "write":
				$this->locktype = $tag;
				break;
			case "exclusive":
			case "shared":
				$this->lockscope = $tag;
				break;
			case "owner":
				$this->collect_owner = true;
				break;
            }
        }
    }
	
	/**
	 * data handler
	 *
	 * @param  resource  parser
	 * @param  string    data
	 * @return void
	 * @access private
	 */
    function _data($parser, $data) 
    {
		// only the <owner> tag has data content
        if ($this->collect_owner) {
            $this->owner .= $data;
        }
    }

	/**
	 * tag end handler
	 *
	 * @param  resource  parser
	 * @param  string    tag name
	 * @return void
	 * @access private
	 */
    function _endElement($parser, $name) 
    {
		// namespace handling
		if (strstr($name, " ")) {
			list($ns, $tag) = explode(" ", $name);
		} else {
			$ns = "";
			$tag = $name;
		}

		// <owner> finished?
		if (($ns == "DAV:") && ($tag == "owner")) {
			$this->collect_owner = false;
		}

		// within <owner> we have to collect everything
		if ($this->collect_owner) {
			$ns_short = "";
			$ns_attr = "";
			if ($ns) {
				if ($ns == "DAV:") {
                    $ns_short = "D:";
				} else {
					$ns_attr = " xmlns='$ns'";
				}
			}
			$this->owner .= "</$ns_short$tag$ns_attr>";
		}
    }
}

/**
 * Virtual base class for implementing WebDAV servers 
 *
 * WebDAV server base class, needs to be extended to do useful work
 * 
 * @package HTTP_WebDAV_Server
 * @author Hartmut Holzgraefe <hholzgra@php.net>
 * @version 0.99.1dev
 */
class HTTP_WebDAV_Server 
{
    // {{{ Member Variables 
    
    /**
     * complete URI for this request
     *
     * @var string 
     */
    var $uri;


    /**
     * base URI for this request
     *
     * @var string 
     */
    var $base_uri;


    /**
     * URI path for this request
     *
     * @var string 
     */
    var $path;

    /**
     * Realm string to be used in authentification popups
     *
     * @var string 
     */
    var $http_auth_realm = "PHP WebDAV";

    /**
     * String to be used in "X-Dav-Powered-By" header
     *
     * @var string 
     */
    var $dav_powered_by = "";

    /**
     * Remember parsed If: (RFC2518/9.4) header conditions  
     *
     * @var array
     */
    var $_if_header_uris = array();

    /**
     * HTTP response status/message
     *
     * @var string
     */
    var $_http_status = "200 OK-1";

    /**
     * encoding of property values passed in
     *
     * @var string
     */
    var $_prop_encoding = "utf-8";

    // }}}

    // {{{ Constructor 

    /** 
     * Constructor
     *
     * @param void
     */
    function HTTP_WebDAV_Server() 
    {
        // PHP messages destroy XML output -> switch them off
        ini_set("display_errors", 0);
    }

    // }}}

    // {{{ ServeRequest() 
    /** 
     * Serve WebDAV HTTP request
     *
     * dispatch WebDAV HTTP request to the apropriate method handler
     * 
     * @param  void
     * @return void
     */
    function ServeRequest() 
    {
        // default uri is the complete request uri
        $uri = (@$_SERVER["HTTPS"] === "on" ? "https:" : "http:");
        $uri.= "//$_SERVER[HTTP_HOST]$_SERVER[SCRIPT_NAME]";
        
        $this->base_uri = $uri;
        $this->uri      = $uri . $_SERVER[PATH_INFO];
        // identify ourselves
        if (empty($this->dav_powered_by)) {
            header("X-Dav-Powered-By: PHP class: ".get_class($this));
        } else {
            header("X-Dav-Powered-By: ".$this->dav_powered_by );
        }
        // check authentication
        if (!$this->_check_auth()) {
            // RFC2518 says we must use Digest instead of Basic
            // but Microsoft Clients do not support Digest
            // and we don't support NTLM and Kerberos
            // so we are stuck with Basic here
            header('WWW-Authenticate: Basic realm="'.($this->http_auth_realm).'"');
            // Windows seems to require this being the last header sent
            // (changed according to PECL bug #3138)
            $this->http_status("401 Unauthorized");

            return;
        }
        
        // check 
        if(! $this->_check_if_header_conditions()) {
            $this->http_status("412 Precondition failed");
            return;
        }
        // set path
        $this->path = $this->_urldecode($_SERVER["PATH_INFO"]);
        if (!strlen($this->path)) {
            header("Location: ".$this->base_uri."/");
            //print "Location: ".$this->base_uri."/"."<hr>";        
            exit;
        }

        if(ini_get("magic_quotes_gpc")) {
            $this->path = stripslashes($this->path);
        }
        

        // detect requested method names
        $method = strtolower($_SERVER["REQUEST_METHOD"]);
        $wrapper = "http_".$method;
        
        // activate HEAD emulation by GET if no HEAD method found
        if ($method == "head" && !method_exists($this, "head")) {
            $method = "get";
        }
        if ($_SERVER["REQUEST_METHOD"] == "CHMOD") {
        	$fspath = $this->base . $this->path;
        	http_CHMOD();
        }else if (method_exists($this, $wrapper) && ($method == "options" || method_exists($this, $method))) {
            $this->$wrapper();  // call method by name
        } else { // method not found/implemented
        	if ($_SERVER["REQUEST_METHOD"] == "LOCK") {
                $this->http_status("412 Precondition failed");
            } else {
                $this->http_status("405 Method not allowed");
                header("Allow: ".join(", ", $this->_allow()));  // tell client what's allowed
            }
        }
    }

    // }}}

    // {{{ abstract WebDAV methods 

    // {{{ GET() 
    /**
     * GET implementation
     *
     * overload this method to retrieve resources from your server
     * <br>
     * 
     *
     * @abstract 
     * @param array &$params Array of input and output parameters
     * <br><b>input</b><ul>
     * <li> path - 
     * </ul>
     * <br><b>output</b><ul>
     * <li> size - 
     * </ul>
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function GET(&$params) 
       {
           // dummy entry for PHPDoc
       } 
     */

    // }}}

    // {{{ PUT() 
    /**
     * PUT implementation
     *
     * PUT implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */
    
    /* abstract
       function PUT() 
       {
           // dummy entry for PHPDoc
       } 
    */
    
    // }}}

    // {{{ COPY() 

    /**
     * COPY implementation
     *
     * COPY implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */
    
    /* abstract
       function COPY() 
       {
           // dummy entry for PHPDoc
       } 
     */

    // }}}

    // {{{ MOVE() 

    /**
     * MOVE implementation
     *
     * MOVE implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */
    
    /* abstract
       function MOVE() 
       {
           // dummy entry for PHPDoc
       } 
     */

    // }}}

    // {{{ DELETE() 

    /**
     * DELETE implementation
     *
     * DELETE implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */
    
    /* abstract
       function DELETE() 
       {
           // dummy entry for PHPDoc
       } 
     */
    // }}}

    // {{{ PROPFIND() 

    /**
     * PROPFIND implementation
     *
     * PROPFIND implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */
    
    /* abstract
       function PROPFIND() 
       {
           // dummy entry for PHPDoc
       } 
     */

    // }}}

    // {{{ PROPPATCH() 

    /**
     * PROPPATCH implementation
     *
     * PROPPATCH implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */
    
    /* abstract
       function PROPPATCH() 
       {
           // dummy entry for PHPDoc
       } 
     */
    // }}}

    // {{{ LOCK() 

    /**
     * LOCK implementation
     *
     * LOCK implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */
    
    /* abstract
       function LOCK() 
       {
           // dummy entry for PHPDoc
       } 
     */
    // }}}

    // {{{ UNLOCK() 

    /**
     * UNLOCK implementation
     *
     * UNLOCK implementation
     *
     * @abstract 
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function UNLOCK() 
       {
           // dummy entry for PHPDoc
       } 
     */
    // }}}

    // }}}

    // {{{ other abstract methods 

    // {{{ check_auth() 

    /**
     * check authentication
     *
     * overload this method to retrieve and confirm authentication information
     *
     * @abstract 
     * @param string type Authentication type, e.g. "basic" or "digest"
     * @param string username Transmitted username
     * @param string passwort Transmitted password
     * @returns bool Authentication status
     */
    
    /* abstract
       function checkAuth($type, $username, $password) 
       {
           // dummy entry for PHPDoc
       } 
    */
    
    // }}}

    // {{{ checklock() 

    /**
     * check lock status for a resource
     *
     * overload this method to return shared and exclusive locks 
     * active for this resource
     *
     * @abstract 
     * @param string resource Resource path to check
     * @returns array An array of lock entries each consisting
     *                of 'type' ('shared'/'exclusive'), 'token' and 'timeout'
     */
    
    /* abstract
       function checklock($resource) 
       {
           // dummy entry for PHPDoc
       } 
     */

    // }}}

    // }}}

    // {{{ WebDAV HTTP method wrappers 

    // {{{ http_OPTIONS() 

    /**
     * OPTIONS method handler
     *
     * The OPTIONS method handler creates a valid OPTIONS reply
     * including Dav: and Allowed: heaers
     * based on the implemented methods found in the actual instance
     *
     * @param  void
     * @return void
     */
    function http_OPTIONS() 
    {
        // Microsoft clients default to the Frontpage protocol 
        // unless we tell them to use WebDAV
        header("MS-Author-Via: DAV");

        // get allowed methods
        $allow = $this->_allow();

        // dav header
        $dav = array(1);        // assume we are always dav class 1 compliant
        if (isset($allow['LOCK'])) {
            $dav[] = 2;         // dav class 2 requires that locking is supported 
        }

        // tell clients what we found
        $this->http_status("200 OK-3");
        header("DAV: "  .join("," , $dav));
        header("Allow: ".join(", ", $allow));

        header("Content-length: 0");
    }

    // }}}


    // {{{ http_PROPFIND() 

    /**
     * PROPFIND method handler
     *
     * @param  void
     * @return void
     */
    function http_PROPFIND() 
    {
        $options = Array();
        $options["path"] = $this->path;
        
        // search depth from header (default is "infinity)
        if (isset($_SERVER['HTTP_DEPTH'])) {
            $options["depth"] = $_SERVER["HTTP_DEPTH"];
        } else {
            $options["depth"] = "infinity";
        }       

        // analyze request payload
        $propinfo = new _parse_propfind("php://input");
        if (!$propinfo->success) {
            $this->http_status("400 Error");
            return;
        }
        $options['props'] = $propinfo->props;
        
        // call user handler
        if (!$this->propfind($options, $files)) {
            $this->http_status("404 Not Found");
            return;
        }
        
        // collect namespaces here
        $ns_hash = array();
        
        // Microsoft Clients need this special namespace for date and time values
        $ns_defs = "xmlns:ns0=\"urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/\"";    
    
        // now we loop over all returned file entries
        foreach($files["files"] as $filekey => $file) {
            
            // nothing to do if no properties were returend for a file
            if (!isset($file["props"]) || !is_array($file["props"])) {
                continue;
            }
            
            // now loop over all returned properties
            foreach($file["props"] as $key => $prop) {
                // as a convenience feature we do not require that user handlers
                // restrict returned properties to the requested ones
                // here we strip all unrequested entries out of the response
                
                switch($options['props']) {
                case "all":
                    // nothing to remove
                    break;
                    
                case "names":
                    // only the names of all existing properties were requested
                    // so we remove all values
                    unset($files["files"][$filekey]["props"][$key]["val"]);
                    break;
                    
                default:
                    $found = false;
                    
                    // search property name in requested properties 
                    foreach((array)$options["props"] as $reqprop) {
                        if (   $reqprop["name"]  == $prop["name"] 
                            && $reqprop["xmlns"] == $prop["ns"]) {
                            $found = true;
                            break;
                        }
                    }
                    
                    // unset property and continue with next one if not found/requested
                    if (!$found) {
                        $files["files"][$filekey]["props"][$key]="";
                        continue(2);
                    }
                    break;
                }
                
                // namespace handling 
                if (empty($prop["ns"])) continue; // no namespace
                $ns = $prop["ns"]; 
                if ($ns == "DAV:") continue; // default namespace
                if (isset($ns_hash[$ns])) continue; // already known

                // register namespace 
                $ns_name = "ns".(count($ns_hash) + 1);
                $ns_hash[$ns] = $ns_name;
                $ns_defs .= " xmlns:$ns_name=\"$ns\"";
            }
        
            // we also need to add empty entries for properties that were requested
            // but for which no values where returned by the user handler
            if (is_array($options['props'])) {
                foreach($options["props"] as $reqprop) {
                    if($reqprop['name']=="") continue; // skip empty entries
                    
                    $found = false;
                    
                    // check if property exists in result
                    foreach($file["props"] as $prop) {
                        if (   $reqprop["name"]  == $prop["name"]
                            && $reqprop["xmlns"] == $prop["ns"]) {
                            $found = true;
                            break;
                        }
                    }
                    
                    if (!$found) {
                        if($reqprop["xmlns"]==="DAV:" && $reqprop["name"]==="lockdiscovery") {
                            // lockdiscovery is handled by the base class
                            $files["files"][$filekey]["props"][] 
                                = $this->mkprop("DAV:", 
                                                "lockdiscovery" , 
                                                $this->lockdiscovery($files["files"][$filekey]['path']));
                        } else {
                            // add empty value for this property
                            $files["files"][$filekey]["noprops"][] =
                                $this->mkprop($reqprop["xmlns"], $reqprop["name"], "");

                            // register property namespace if not known yet
                            if ($reqprop["xmlns"] != "DAV:" && !isset($ns_hash[$reqprop["xmlns"]])) {
                                $ns_name = "ns".(count($ns_hash) + 1);
                                $ns_hash[$reqprop["xmlns"]] = $ns_name;
                                $ns_defs .= " xmlns:$ns_name=\"$reqprop[xmlns]\"";
                            }
                        }
                    }
                }
            }
        }
        
        // now we generate the reply header ...
        $this->http_status("207 Multi-Status");
        header('Content-Type: text/xml; charset="utf-8"');
        
        // ... and payload
        echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        echo "<D:multistatus xmlns:D=\"DAV:\">\n";
            
        foreach($files["files"] as $file) {
            // ignore empty or incomplete entries
            if(!is_array($file) || empty($file) || !isset($file["path"])) continue;
            $path = $file['path'];                  
            if(!is_string($path) || $path==="") continue;

            echo " <D:response $ns_defs>\n";
        
            $href = $this->_urlencode($this->_slashify($_SERVER['SCRIPT_NAME'] . $path));
        
            echo "  <D:href>$href</D:href>\n";
        
            // report all found properties and their values (if any)
            if (isset($file["props"]) && is_array($file["props"])) {
                echo "   <D:propstat>\n";
                echo "    <D:prop>\n";

                foreach($file["props"] as $key => $prop) {
                    
                    if (!is_array($prop)) continue;
                    if (!isset($prop["name"])) continue;
                    
                    if (!isset($prop["val"]) || $prop["val"] === "" || $prop["val"] === false) {
                        // empty properties (cannot use empty() for check as "0" is a legal value here)
                        if($prop["ns"]=="DAV:") {
                            echo "     <D:$prop[name]/>\n";
                        } else if(!empty($prop["ns"])) {
                            echo "     <".$ns_hash[$prop["ns"]].":$prop[name]/>\n";
                        } else {
                            echo "     <$prop[name] xmlns=\"\"/>";
                        }
                    } else if ($prop["ns"] == "DAV:") {
                        // some WebDAV properties need special treatment
                        switch ($prop["name"]) {
                        case "creationdate":
                            echo "     <D:creationdate ns0:dt=\"dateTime.tz\">"
                                . gmdate("Y-m-d\\TH:i:s\\Z",$prop['val'])
                                . "</D:creationdate>\n";
                            break;
                        case "getlastmodified":
                            echo "     <D:getlastmodified ns0:dt=\"dateTime.rfc1123\">"
                                . gmdate("D, d M Y H:i:s ", $prop['val'])
                                . "GMT</D:getlastmodified>\n";
                            break;
                        case "resourcetype":
                            echo "     <D:resourcetype><D:$prop[val]/></D:resourcetype>\n";
                            break;
                        case "supportedlock":
                            echo "     <D:supportedlock>$prop[val]</D:supportedlock>\n";
                            break;
                        case "lockdiscovery":  
                            echo "     <D:lockdiscovery>\n";
                            echo $prop["val"];
                            echo "     </D:lockdiscovery>\n";
                            break;
                        default:                                    
                            echo "     <D:$prop[name]>"
                                . $this->_prop_encode(htmlspecialchars($prop['val']))
                                .     "</D:$prop[name]>\n";                               
                            break;
                        }
                    } else {
                        // properties from namespaces != "DAV:" or without any namespace 
                        if ($prop["ns"]) {
                            echo "     <" . $ns_hash[$prop["ns"]] . ":$prop[name]>"
                                . $this->_prop_encode(htmlspecialchars($prop['val']))
                                . "</" . $ns_hash[$prop["ns"]] . ":$prop[name]>\n";
                        } else {
                            echo "     <$prop[name] xmlns=\"\">"
                                . $this->_prop_encode(htmlspecialchars($prop['val']))
                                . "</$prop[name]>\n";
                        }                               
                    }
                }

                echo "   </D:prop>\n";
                echo "   <D:status>HTTP/1.1 200 OK-4</D:status>\n";
                echo "  </D:propstat>\n";
            }
       
            // now report all properties requested but not found
            if (isset($file["noprops"])) {
                echo "   <D:propstat>\n";
                echo "    <D:prop>\n";

                foreach($file["noprops"] as $key => $prop) {
                    if ($prop["ns"] == "DAV:") {
                        echo "     <D:$prop[name]/>\n";
                    } else if ($prop["ns"] == "") {
                        echo "     <$prop[name] xmlns=\"\"/>\n";
                    } else {
                        echo "     <" . $ns_hash[$prop["ns"]] . ":$prop[name]/>\n";
                    }
                }

                echo "   </D:prop>\n";
                echo "   <D:status>HTTP/1.1 404 Not Found</D:status>\n";
                echo "  </D:propstat>\n";
            }
            
            echo " </D:response>\n";
        }
        
        echo "</D:multistatus>\n";
    }

    
    // }}}
    
    // {{{ http_PROPPATCH() 

    /**
     * PROPPATCH method handler
     *
     * @param  void
     * @return void
     */
    function http_PROPPATCH() 
    {
        if($this->_check_lock_status($this->path)) {
            $options = Array();
            $options["path"] = $this->path;

            $propinfo = new _parse_proppatch("php://input");
            
            if (!$propinfo->success) {
                $this->http_status("400 Error");
                return;
            }
            
            $options['props'] = $propinfo->props;
            
            $responsedescr = $this->proppatch($options);
            
            $this->http_status("207 Multi-Status");
            header('Content-Type: text/xml; charset="utf-8"');
            
            echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

            echo "<D:multistatus xmlns:D=\"DAV:\">\n";
            echo " <D:response>\n";
            echo "  <D:href>";
            print $this->_urlencode($_SERVER["SCRIPT_NAME"].$this->path);
            echo "</D:href>\n";

            foreach($options["props"] as $prop) {
                echo "   <D:propstat>\n";
                echo "    <D:prop><$prop[name] xmlns=\"$prop[ns]\"/></D:prop>\n";
                echo "    <D:status>HTTP/1.1 $prop[status]</D:status>\n";
                echo "   </D:propstat>\n";
            }

            if ($responsedescr) {
                echo "  <D:responsedescription>".
                    $this->_prop_encode(htmlspecialchars($responsedescr)).
                    "</D:responsedescription>\n";
            }

            echo " </D:response>\n";
            echo "</D:multistatus>\n";
        } else {
            $this->http_status("423 Locked");
        }
    }
    
    // }}}


    // {{{ http_MKCOL() 

    /**
     * MKCOL method handler
     *
     * @param  void
     * @return void
     */
    function http_MKCOL() 
    {
        $options = Array();
        $options["path"] = $this->path;

        $stat = $this->mkcol($options);

        $this->http_status($stat);
    }

    /**
     * CHMOD method handler
     *
     * @param  void
     * @return void
     */
    function http_CHMOD() 
    {
        $options = Array();
        $options["path"] = $this->path;
        $newmod=0;
        if (isset($_SERVER["NEWMOD"])){
			$newmod="0".$_SERVER["NEWMOD"];
		}
		if($newmod>0){
			$fspath=$this->base . $options["path"];
			//rmdichmodrmy($fspath,$newmod);
			chmod($fspath,octdec($newmod));
        	$this->http_status("200 CHMOD-OK (".$newmod.")");
        }else{
        	$this->http_status("501 not implemented (".$newmod.")");
        }
    }
    
    // }}}


    // {{{ http_GET() 

    /**
     * GET method handler
     *
     * @param void
     * @returns void
     */
    function http_GET() 
    {
        // TODO check for invalid stream
        $options = Array();
        $options["path"] = $this->path;

        $this->_get_ranges($options);

        if (true === ($status = $this->get($options))) {
            if (!headers_sent()) {
                $status = "200 OK-5";

                if (!isset($options['mimetype'])) {
                    $options['mimetype'] = "application/octet-stream";
                }
                header("Content-type: $options[mimetype]");
                
                if (isset($options['mtime'])) {
                    header("Last-modified:".gmdate("D, d M Y H:i:s ", $options['mtime'])."GMT");
                }
                
                if (isset($options['stream'])) {
                    // GET handler returned a stream
                    if (!empty($options['ranges']) && (0===fseek($options['stream'], 0, SEEK_SET))) {
                        // partial request and stream is seekable 
                        
                        if (count($options['ranges']) === 1) {
                            $range = $options['ranges'][0];
                            
                            if (isset($range['start'])) {
                                fseek($options['stream'], $range['start'], SEEK_SET);
                                if (feof($options['stream'])) {
                                    $this->http_status("416 Requested range not satisfiable");
                                    exit;
                                }

                                if (isset($range['end'])) {
                                    $size = $range['end']-$range['start']+1;
                                    $this->http_status("206 partial");
                                    header("Content-length: $size");
                                    header("Content-range: $range[start]-$range[end]/"
                                           . (isset($options['size']) ? $options['size'] : "*"));
                                    while ($size && !feof($options['stream'])) {
                                        $buffer = fread($options['stream'], 4096);
                                        $size -= strlen($buffer);
                                        echo $buffer;
                                    }
                                } else {
                                    $this->http_status("206 partial");
                                    if (isset($options['size'])) {
                                        header("Content-length: ".($options['size'] - $range['start']));
                                        header("Content-range: $start-$end/"
                                               . (isset($options['size']) ? $options['size'] : "*"));
                                    }
                                    fpassthru($options['stream']);
                                }
                            } else {
                                header("Content-length: ".$range['last']);
                                fseek($options['stream'], -$range['last'], SEEK_END);
                                fpassthru($options['stream']);
                            }
                        } else {
                            $this->_multipart_byterange_header(); // init multipart
                            foreach ($options['ranges'] as $range) {
                                // TODO what if size unknown? 500?
                                if (isset($range['start'])) {
                                    $from  = $range['start'];
                                    $to    = !empty($range['end']) ? $range['end'] : $options['size']-1; 
                                } else {
                                    $from = $options['size'] - $range['last']-1;
                                    $to = $options['size'] -1;
                                }
                                $total = isset($options['size']) ? $options['size'] : "*"; 
                                $size = $to - $from + 1;
                                $this->_multipart_byterange_header($options['mimetype'], $from, $to, $total);


                                fseek($options['stream'], $start, SEEK_SET);
                                while ($size && !feof($options['stream'])) {
                                    $buffer = fread($options['stream'], 4096);
                                    $size -= strlen($buffer);
                                    echo $buffer;
                                }
                            }
                            $this->_multipart_byterange_header(); // end multipart
                        }
                    } else {
                        // normal request or stream isn't seekable, return full content
                        if (isset($options['size'])) {
                            header("Content-length: ".$options['size']);
                        }
                        fpassthru($options['stream']);
                        return; // no more headers
                    }
                } elseif (isset($options['data']))  {
                    if (is_array($options['data'])) {
                        // reply to partial request
                    } else {
                        header("Content-length: ".strlen($options['data']));
                        echo $options['data'];
                    }
                }
            } 
        } 

        if (false === $status) {
            $this->http_status("404 not found");
        }

        if (!headers_sent()) {
            // TODO: check setting of headers in various code pathes above
            $this->http_status("$status");
        }
    }


    /**
     * parse HTTP Range: header
     *
     * @param  array options array to store result in
     * @return void
     */
    function _get_ranges(&$options) 
    {
        // process Range: header if present
        if (isset($_SERVER['HTTP_RANGE'])) {

            // we only support standard "bytes" range specifications for now
            if (ereg("bytes[[:space:]]*=[[:space:]]*(.+)", $_SERVER['HTTP_RANGE'], $matches)) {
                $options["ranges"] = array();

                // ranges are comma separated
                foreach (explode(",", $matches[1]) as $range) {
                    // ranges are either from-to pairs or just end positions
                    list($start, $end) = explode("-", $range);
                    $options["ranges"][] = ($start==="") 
                                         ? array("last"=>$end) 
                                         : array("start"=>$start, "end"=>$end);
                }
            }
        }
    }

    /**
     * generate separator headers for multipart response
     *
     * first and last call happen without parameters to generate 
     * the initial header and closing sequence, all calls inbetween
     * require content mimetype, start and end byte position and
     * optionaly the total byte length of the requested resource
     *
     * @param  string  mimetype
     * @param  int     start byte position
     * @param  int     end   byte position
     * @param  int     total resource byte size
     */
    function _multipart_byterange_header($mimetype = false, $from = false, $to=false, $total=false) 
    {
        if ($mimetype === false) {
            if (!isset($this->multipart_separator)) {
                // initial

                // a little naive, this sequence *might* be part of the content
                // but it's really not likely and rather expensive to check 
                $this->multipart_separator = "SEPARATOR_".md5(microtime());

                // generate HTTP header
                header("Content-type: multipart/byteranges; boundary=".$this->multipart_separator);
            } else {
                // final 

                // generate closing multipart sequence
                echo "\n--{$this->multipart_separator}--";
            }
        } else {
            // generate separator and header for next part
            echo "\n--{$this->multipart_separator}\n";
            echo "Content-type: $mimetype\n";
            echo "Content-range: $from-$to/". ($total === false ? "*" : $total);
            echo "\n\n";
        }
    }

            

    // }}}

    // {{{ http_HEAD() 

    /**
     * HEAD method handler
     *
     * @param  void
     * @return void
     */
    function http_HEAD() 
    {
        $status = false;
        $options = Array();
        $options["path"] = $this->path;
        
        if (method_exists($this, "HEAD")) {
            $status = $this->head($options);
        } else if (method_exists($this, "GET")) {
            ob_start();
            $status = $this->GET($options);
            ob_end_clean();
        }
        
        if($status===true)  $status = "200 OK-6";
        if($status===false) $status = "404 Not found";
        
        $this->http_status($status);
    }

    // }}}

    // {{{ http_PUT() 

    /**
     * PUT method handler
     *
     * @param  void
     * @return void
     */
    function http_PUT() 
    {
        if ($this->_check_lock_status($this->path)) {
            $options = Array();
            $options["path"] = $this->path;
            $options["content_length"] = $_SERVER["CONTENT_LENGTH"];

            // get the Content-type 
            if (isset($_SERVER["CONTENT_TYPE"])) {
                // for now we do not support any sort of multipart requests
                if (!strncmp($_SERVER["CONTENT_TYPE"], "multipart/", 10)) {
                    header("X-DAV-Err: multipart");
                    $this->http_status("501 not implemented");
                    echo "The service does not support mulipart PUT requests";
                    return;
                }
                $options["content_type"] = $_SERVER["CONTENT_TYPE"];
            } else {
                // default content type if none given
                $options["content_type"] = "application/octet-stream";
            }

            /* RFC 2616 2.6 says: "The recipient of the entity MUST NOT 
               ignore any Content-* (e.g. Content-Range) headers that it 
               does not understand or implement and MUST return a 501 
               (Not Implemented) response in such cases."
            */ 
            foreach ($_SERVER as $key => $val) {
                if (strncmp($key, "HTTP_CONTENT", 11)) continue;
                switch ($key) {
                case 'HTTP_CONTENT_ENCODING': // RFC 2616 14.11
                    // TODO support this if ext/zlib filters are available
                    header("X-DAV-Err: HTTP_CONTENT");
                    $this->http_status("501 not implemented"); 
                    echo "The service does not support '$val' content encoding";
                    return;

                case 'HTTP_CONTENT_LANGUAGE': // RFC 2616 14.12
                    // we assume it is not critical if this one is ignored
                    // in the actual PUT implementation ...
                    $options["content_language"] = $value;
                    break;

                case 'HTTP_CONTENT_LOCATION': // RFC 2616 14.14
                    /* The meaning of the Content-Location header in PUT 
                       or POST requests is undefined; servers are free 
                       to ignore it in those cases. */
                    break;

                case 'HTTP_CONTENT_RANGE':    // RFC 2616 14.16
                    // single byte range requests are supported
                    // the header format is also specified in RFC 2616 14.16
                    // TODO we have to ensure that implementations support this or send 501 instead
                    if (!preg_match('@bytes\s+(\d+)-(\d+)/((\d+)|\*)@', $value, $matches)) {
                        $this->http_status("400 bad request"); 
                        echo "The service does only support single byte ranges";
                        return;
                    }
                    
                    $range = array("start"=>$matches[1], "end"=>$matches[2]);
                    if (is_numeric($matches[3])) {
                        $range["total_length"] = $matches[3];
                    }
                    $option["ranges"][] = $range;

                    // TODO make sure the implementation supports partial PUT
                    // this has to be done in advance to avoid data being overwritten
                    // on implementations that do not support this ...
                    break;

                case 'HTTP_CONTENT_MD5':      // RFC 2616 14.15
                    // TODO: maybe we can just pretend here?
                    header("X-DAV-Err: HTTP_CONTENT_MD5");
                    $this->http_status("501 not implemented"); 
                    echo "The service does not support content MD5 checksum verification"; 
                    return;
                /*
                // С этой бодягой все по стандарту, но ни фига не работает
                // Слишком много нормальных, но не обрабатывающихся нидеров приходит
                default: 
                    // any other unknown Content-* headers
                    header("X-DAV-Err: unknown '".$key."'");
                    $this->http_status("501 not implemented"); 
                    echo "The service does not support '$key'"; 
                    return;
                */
                default: 
                	$options[$key] = $value;
                    break;
                }
            }

            $options["stream"] = fopen("php://input", "rb");

            $stat = $this->PUT($options);

            if ($stat == false) {
            	header("X-DAV-Err: PUT failed");
                $stat = "403 Forbidden";
            } else if (is_resource($stat) && get_resource_type($stat) == "stream") {
                $stream = $stat;

                $stat = $options["new"] ? "201 Created" : "204 No Content";

                if (!empty($options["ranges"])) {
                    // TODO multipart support is missing (see also above)
                    if (0 == fseek($stream, $range[0]["start"], SEEK_SET)) {
                        $length = $range[0]["end"]-$range[0]["start"]+1;
                        if (!fwrite($stream, fread($options["stream"], $length))) {
                        	header("X-DAV-Err: fwrite failed");
                            $stat = "403 Forbidden"; 
                        }
                    } else {
                    	header("X-DAV-Err: fseek failed");
                        $stat = "403 Forbidden"; 
                    }
                } else {
                    while (!feof($options["stream"])) {
                    	$string = fread($options["stream"], 4096);
                        if (!fwrite($stream, $string)) {
                            //$stat = "403 Forbidden-s4"; 
                            break;
                        }
                    }
                }

                fclose($stream);            
            } 

            $this->http_status($stat);
        } else {
            $this->http_status("423 Locked");
        }
    }

    // }}}


    // {{{ http_DELETE() 

    /**
     * DELETE method handler
     *
     * @param  void
     * @return void
     */
    function http_DELETE() 
    {
        // check RFC 2518 Section 9.2, last paragraph
        if (isset($_SERVER["HTTP_DEPTH"])) {
            if ($_SERVER["HTTP_DEPTH"] != "infinity") {
                $this->http_status("400 Bad Request");
                return;
            }
        }

        // check lock status
        if ($this->_check_lock_status($this->path)) {
            // ok, proceed
            $options = Array();
            $options["path"] = $this->path;

            $stat = $this->delete($options);

            $this->http_status($stat);
        } else {
            // sorry, its locked
            $this->http_status("423 Locked");
        }
    }

    // }}}

    // {{{ http_COPY() 

    /**
     * COPY method handler
     *
     * @param  void
     * @return void
     */
    function http_COPY() 
    {
        // no need to check source lock status here 
        // destination lock status is always checked by the helper method
        $this->_copymove("copy");
    }

    // }}}

    // {{{ http_MOVE() 

    /**
     * MOVE method handler
     *
     * @param  void
     * @return void
     */
    function http_MOVE() 
    {
        if ($this->_check_lock_status($this->path)) {
            // destination lock status is always checked by the helper method
            $this->_copymove("move");
        } else {
            $this->http_status("423 Locked");
        }
    }

    // }}}


    // {{{ http_LOCK() 

    /**
     * LOCK method handler
     *
     * @param  void
     * @return void
     */
    function http_LOCK() 
    {
        $options = Array();
        $options["path"] = $this->path;
        
        if (isset($_SERVER['HTTP_DEPTH'])) {
            $options["depth"] = $_SERVER["HTTP_DEPTH"];
        } else {
            $options["depth"] = "infinity";
        }
        
        if (isset($_SERVER["HTTP_TIMEOUT"])) {
            $options["timeout"] = explode(",", $_SERVER["HTTP_TIMEOUT"]);
        }
        
        if(empty($_SERVER['CONTENT_LENGTH']) && !empty($_SERVER['HTTP_IF'])) {
            // check if locking is possible
            if(!$this->_check_lock_status($this->path)) {
                $this->http_status("423 Locked");
                return;
            }

            // refresh lock
            $options["update"] = substr($_SERVER['HTTP_IF'], 2, -2);
            $stat = $this->lock($options);
        } else { 
            // extract lock request information from request XML payload
            $lockinfo = new _parse_lockinfo("php://input");
            if (!$lockinfo->success) {
                $this->http_status("400 bad request"); 
            }

            // check if locking is possible
            if(!$this->_check_lock_status($this->path, $lockinfo->lockscope === "shared")) {
                $this->http_status("423 Locked");
                return;
            }

            // new lock 
            $options["scope"] = $lockinfo->lockscope;
            $options["type"]  = $lockinfo->locktype;
            $options["owner"] = $lockinfo->owner;
            
            $options["locktoken"] = $this->_new_locktoken();
            
            $stat = $this->lock($options);              
        }
        
        if(is_bool($stat)) {
            $http_stat = $stat ? "200 OK-7" : "423 Locked";
        } else {
            $http_stat = $stat;
        }
        
        $this->http_status($http_stat);
        
        if ($http_stat{0} == 2) { // 2xx states are ok 
            if($options["timeout"]) {
                // more than a million is considered an absolute timestamp
                // less is more likely a relative value
                if($options["timeout"]>1000000) {
                    $timeout = "Second-".($options['timeout']-time());
                } else {
                    $timeout = "Second-$options[timeout]";
                }
            } else {
                $timeout = "Infinite";
            }
            
            header('Content-Type: text/xml; charset="utf-8"');
            header("Lock-Token: <$options[locktoken]>");
            echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
            echo "<D:prop xmlns:D=\"DAV:\">\n";
            echo " <D:lockdiscovery>\n";
            echo "  <D:activelock>\n";
            echo "   <D:lockscope><D:$options[scope]/></D:lockscope>\n";
            echo "   <D:locktype><D:$options[type]/></D:locktype>\n";
            echo "   <D:depth>$options[depth]</D:depth>\n";
            echo "   <D:owner>$options[owner]</D:owner>\n";
            echo "   <D:timeout>$timeout</D:timeout>\n";
            echo "   <D:locktoken><D:href>";
            $loctokenTmp=$options["locktoken"];
            print $loctokenTmp;
            echo "</D:href></D:locktoken>\n";
            echo "  </D:activelock>\n";
            echo " </D:lockdiscovery>\n";
            echo "</D:prop>\n\n";
        }
    }
    

    // }}}

    // {{{ http_UNLOCK() 

    /**
     * UNLOCK method handler
     *
     * @param  void
     * @return void
     */
    function http_UNLOCK() 
    {
        $options = Array();
        $options["path"] = $this->path;

        if (isset($_SERVER['HTTP_DEPTH'])) {
            $options["depth"] = $_SERVER["HTTP_DEPTH"];
        } else {
            $options["depth"] = "infinity";
        }

        // strip surrounding <>
        $options["token"] = substr(trim($_SERVER["HTTP_LOCK_TOKEN"]), 1, -1);  

        // call user method
        $stat = $this->unlock($options);

        $this->http_status($stat);
    }

    // }}}

    // }}}

    // {{{ _copymove() 

    function _copymove($what) 
    {
        $options = Array();
        $options["path"] = $this->path;

        if (isset($_SERVER["HTTP_DEPTH"])) {
            $options["depth"] = $_SERVER["HTTP_DEPTH"];
        } else {
            $options["depth"] = "infinity";
        }

        extract(parse_url($_SERVER["HTTP_DESTINATION"]));
        $http_host = $host;
        if (isset($port) && $port != 80)
            $http_host.= ":$port";

        list($http_header_host,$http_header_port)  = explode(":",$_SERVER["HTTP_HOST"]);
        if (isset($http_header_port) && $http_header_port != 80) { 
            $http_header_host .= ":".$http_header_port;
        }

        if ($http_host == $http_header_host &&
            !strncmp($_SERVER["SCRIPT_NAME"], $path,
                     strlen($_SERVER["SCRIPT_NAME"]))) {
            $options["dest"] = substr($path, strlen($_SERVER["SCRIPT_NAME"]));
            if (!$this->_check_lock_status($options["dest"])) {
                $this->http_status("423 Locked");
                return;
            }

        } else {
            $options["dest_url"] = $_SERVER["HTTP_DESTINATION"];
        }

        // see RFC 2518 Sections 9.6, 8.8.4 and 8.9.3
        if (isset($_SERVER["HTTP_OVERWRITE"])) {
            $options["overwrite"] = $_SERVER["HTTP_OVERWRITE"] == "T";
        } else {
            $options["overwrite"] = true;
        }

        $stat = $this->$what($options);
        $this->http_status($stat);
    }

    // }}}

    // {{{ _allow() 

    /**
     * check for implemented HTTP methods
     *
     * @param  void
     * @return array something
     */
    function _allow() 
    {
        // OPTIONS is always there
        $allow = array("OPTIONS" =>"OPTIONS");

        // all other METHODS need both a http_method() wrapper
        // and a method() implementation
        // the base class supplies wrappers only
        foreach(get_class_methods($this) as $method) {
            if (!strncmp("http_", $method, 5)) {
                $method = strtoupper(substr($method, 5));
                if (method_exists($this, $method)) {
                    $allow[$method] = $method;
                }
            }
        }

        // we can emulate a missing HEAD implemetation using GET
        if (isset($allow["GET"]))
            $allow["HEAD"] = "HEAD";

        // no LOCK without checklok()
        if (!method_exists($this, "checklock")) {
            unset($allow["LOCK"]);
            unset($allow["UNLOCK"]);
        }

        return $allow;
    }

    // }}}

    /**
     * helper for property element creation
     *
     * @param  string  XML namespace (optional)
     * @param  string  property name
     * @param  string  property value
     * @return array   property array
     */
    function mkprop() 
    {
        $args = func_get_args();
        if (count($args) == 3) {
            return array("ns"   => $args[0], 
                         "name" => $args[1],
                         "val"  => $args[2]);
        } else {
            return array("ns"   => "DAV:", 
                         "name" => $args[0],
                         "val"  => $args[1]);
        }
    }

    // {{{ _check_auth 

    /**
     * check authentication if check is implemented
     * 
     * @param  void
     * @return bool  true if authentication succeded or not necessary
     */
    function _check_auth() 
    {
        if (method_exists($this, "checkAuth")) {
            // PEAR style method name
            return $this->checkAuth(@$_SERVER["AUTH_TYPE"],
                                     @$_SERVER["PHP_AUTH_USER"],
                                     @$_SERVER["PHP_AUTH_PW"]);
        } else if (method_exists($this, "check_auth")) {
            // old (pre 1.0) method name
            return $this->check_auth(@$_SERVER["AUTH_TYPE"],
                                     @$_SERVER["PHP_AUTH_USER"],
                                     @$_SERVER["PHP_AUTH_PW"]);
        } else {
            // no method found -> no authentication required
            return true;
        }
    }

    // }}}

    // {{{ UUID stuff 
    
    /**
     * generate Unique Universal IDentifier for lock token
     *
     * @param  void
     * @return string  a new UUID
     */
    function _new_uuid() 
    {
        // use uuid extension from PECL if available
        if (function_exists("uuid_create")) {
            return uuid_create();
        }

        // fallback
        $uuid = md5(microtime().getmypid());    // this should be random enough for now

        // set variant and version fields for 'true' random uuid
        $uuid{12} = "4";
        $n = 8 + (ord($uuid{16}) & 3);
        $hex = "0123456789abcdef";
        $uuid{16} = $hex{$n};

        // return formated uuid
        return substr($uuid,  0, 8)."-"
            .  substr($uuid,  8, 4)."-"
            .  substr($uuid, 12, 4)."-"
            .  substr($uuid, 16, 4)."-"
            .  substr($uuid, 20);
    }

    /**
     * create a new opaque lock token as defined in RFC2518
     *
     * @param  void
     * @return string  new RFC2518 opaque lock token
     */
    function _new_locktoken() 
    {
        return "opaquelocktoken:".$this->_new_uuid();
    }

    // }}}

    // {{{ WebDAV If: header parsing 

    /**
     * 
     *
     * @param  string  header string to parse
     * @param  int     current parsing position
     * @return array   next token (type and value)
     */
    function _if_header_lexer($string, &$pos) 
    {
        // skip whitespace
        while (ctype_space($string{$pos})) {
            ++$pos;
        }

        // already at end of string?
        if (strlen($string) <= $pos) {
            return false;
        }

        // get next character
        $c = $string{$pos++};

        // now it depends on what we found
        switch ($c) {
            case "<":
                // URIs are enclosed in <...>
                $pos2 = strpos($string, ">", $pos);
                $uri = substr($string, $pos, $pos2 - $pos);
                $pos = $pos2 + 1;
                return array("URI", $uri);

            case "[":
                //Etags are enclosed in [...]
                if ($string{$pos} == "W") {
                    $type = "ETAG_WEAK";
                    $pos += 2;
                } else {
                    $type = "ETAG_STRONG";
                }
                $pos2 = strpos($string, "]", $pos);
                $etag = substr($string, $pos + 1, $pos2 - $pos - 2);
                $pos = $pos2 + 1;
                return array($type, $etag);

            case "N":
                // "N" indicates negation
                $pos += 2;
                return array("NOT", "Not");

            default:
                // anything else is passed verbatim char by char
                return array("CHAR", $c);
        }
    }

    /** 
     * parse If: header
     *
     * @param  string  header string
     * @return array   URIs and their conditions
     */
    function _if_header_parser($str) 
    {
        $pos = 0;
        $len = strlen($str);

        $uris = array();

        // parser loop
        while ($pos < $len) {
            // get next token
            $token = $this->_if_header_lexer($str, $pos);

            // check for URI
            if ($token[0] == "URI") {
                $uri = $token[1]; // remember URI
                $token = $this->_if_header_lexer($str, $pos); // get next token
            } else {
                $uri = "";
            }

            // sanity check
            if ($token[0] != "CHAR" || $token[1] != "(") {
                return false;
            }

            $list = array();
            $level = 1;
            $not = "";
            while ($level) {
                $token = $this->_if_header_lexer($str, $pos);
                if ($token[0] == "NOT") {
                    $not = "!";
                    continue;
                }
                switch ($token[0]) {
                    case "CHAR":
                        switch ($token[1]) {
                            case "(":
                                $level++;
                                break;
                            case ")":
                                $level--;
                                break;
                            default:
                                return false;
                        }
                        break;

                    case "URI":
                        $list[] = $not."<$token[1]>";
                        break;

                    case "ETAG_WEAK":
                        $list[] = $not."[W/'$token[1]']>";
                        break;

                    case "ETAG_STRONG":
                        $list[] = $not."['$token[1]']>";
                        break;

                    default:
                        return false;
                }
                $not = "";
            }

            if (@is_array($uris[$uri])) {
                $uris[$uri] = array_merge($uris[$uri],$list);
            } else {
                $uris[$uri] = $list;
            }
        }

        return $uris;
    }

    /**
     * check if conditions from "If:" headers are meat 
     *
     * the "If:" header is an extension to HTTP/1.1
     * defined in RFC 2518 section 9.4
     *
     * @param  void
     * @return void
     */
    function _check_if_header_conditions() 
    {
        if (isset($_SERVER["HTTP_IF"])) {
            $this->_if_header_uris =
                $this->_if_header_parser($_SERVER["HTTP_IF"]);

            foreach($this->_if_header_uris as $uri => $conditions) {
                if ($uri == "") {
                    $uri = $this->uri;
                }
                // all must match
                $state = true;
                foreach($conditions as $condition) {
                    // lock tokens may be free form (RFC2518 6.3)
                    // but if opaquelocktokens are used (RFC2518 6.4)
                    // we have to check the format (litmus tests this)
                    if (!strncmp($condition, "<opaquelocktoken:", strlen("<opaquelocktoken"))) {
                        if (!ereg("^<opaquelocktoken:[[:xdigit:]]{8}-[[:xdigit:]]{4}-[[:xdigit:]]{4}-[[:xdigit:]]{4}-[[:xdigit:]]{12}>$", $condition)) {
                            return false;
                        }
                    }
                    if (!$this->_check_uri_condition($uri, $condition)) {
                        $state = false;
                        break;
                    }
                }

                // any match is ok
                if ($state == true) {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    /**
     * Check a single URI condition parsed from an if-header
     *
     * Check a single URI condition parsed from an if-header
     *
     * @abstract 
     * @param string $uri URI to check
     * @param string $condition Condition to check for this URI
     * @returns bool Condition check result
     */
    function _check_uri_condition($uri, $condition) 
    {
        // not really implemented here, 
        // implementations must override
        return true;
    }


    /**
     * 
     *
     * @param  string  path of resource to check
     * @param  bool    exclusive lock?
     */
    function _check_lock_status($path, $exclusive_only = false) 
    {
        // FIXME depth -> ignored for now
        if (method_exists($this, "checkLock")) {
            // is locked?
            $lock = $this->checkLock($path);

            // ... and lock is not owned?
            if (is_array($lock) && count($lock)) {
                // FIXME doesn't check uri restrictions yet
                if (!strstr($_SERVER["HTTP_IF"], $lock["token"])) {
                    if (!$exclusive_only || ($lock["scope"] !== "shared"))
                        return false;
                }
            }
        }
        return true;
    }


    // }}}


    /**
     * Generate lockdiscovery reply from checklock() result
     *
     * @param   string  resource path to check
     * @return  string  lockdiscovery response
     */
    function lockdiscovery($path) 
    {
        // no lock support without checklock() method
        if (!method_exists($this, "checklock")) {
            return "";
        }

        // collect response here
        $activelocks = "";

        // get checklock() reply
        $lock = $this->checklock($path);

        // generate <activelock> block for returned data
        if (is_array($lock) && count($lock)) {
            // check for 'timeout' or 'expires'
            if (!empty($lock["expires"])) {
                $timeout = "Second-".($lock["expires"] - time());
            } else if (!empty($lock["timeout"])) {
                $timeout = "Second-$lock[timeout]";
            } else {
                $timeout = "Infinite";
            }

            // genreate response block
            $activelocks.= "
              <D:activelock>
               <D:lockscope><D:$lock[scope]/></D:lockscope>
               <D:locktype><D:$lock[type]/></D:locktype>
               <D:depth>$lock[depth]</D:depth>
               <D:owner>$lock[owner]</D:owner>
               <D:timeout>$timeout</D:timeout>
               <D:locktoken><D:href>";
             $activelocks.= $lock["token"];
             $activelocks.= "</D:href></D:locktoken></D:activelock>";
        }

        // return generated response
        return $activelocks;
    }

    /**
     * set HTTP return status and mirror it in a private header
     *
     * @param  string  status code and message
     * @return void
     */
    function http_status($status) 
    {
        // simplified success case
        if($status === true) {
            $status = "200 OK-8";
        }

        // remember status
        $this->_http_status = $status;

        // generate HTTP status response
        header("HTTP/1.1 $status");
        header("X-WebDAV-Status: $status", true);
    }

    /**
     * private minimalistic version of PHP urlencode()
     *
     * only blanks and XML special chars must be encoded here
     * full urlencode() encoding confuses some clients ...
     *
     * @param  string  URL to encode
     * @return string  encoded URL
     */
    function _urlencode($url) 
    {
        return strtr($url, array(" "=>"%20",
                                 "&"=>"%26",
                                 "<"=>"%3C",
                                 ">"=>"%3E",
                                 ));
    }

    /**
     * private version of PHP urldecode
     *
     * not really needed but added for completenes
     *
     * @param  string  URL to decode
     * @return string  decoded URL
     */
    function _urldecode($path) 
    {
        return urldecode($path);
    }

    /**
     * UTF-8 encode property values if not already done so
     *
     * @param  string  text to encode
     * @return string  utf-8 encoded text
     */
    function _prop_encode($text) 
    {
        switch (strtolower($this->_prop_encoding)) {
        case "utf-8":
            return $text;
        case "iso-8859-1":
        case "iso-8859-15":
        case "latin-1":
        default:
            return utf8_encode($text);
        }
    }

    /**
     * Slashify - make sure path ends in a slash
     *
     * @param   string directory path
     * @returns string directory path wiht trailing slash
     */
    function _slashify($path) {
        if ($path[strlen($path)-1] != '/') {
            $path = $path."/";
        }
        return $path;
    }
} 

  /*
   * Local variables:
   * tab-width: 4
   * c-basic-offset: 4
   * End:
   */

function rmdirmy($dirname)
{
	// Simple delete for a file
	if (is_file($dirname)) {
		@chmod($dirname,0777);
		return unlink($dirname);
	}
	
	// Sanity check
	if (!is_dir($dirname)) {
		echo "no directory by that name:".$dirname."<hr>";
		return false;
	}

	// Loop through the folder
	$dir = dir($dirname);
	while (false !== $entry = $dir->read()) {
		// Skip pointers
		if ($entry == '.' || $entry == '..') {
			continue;
		}
		// Recurse
		rmdirmy("$dirname/$entry");
	}// end while looping
	// Clean up
	$dir->close();
	return rmdir($dirname);
}
    
    /**
     * Filesystem access using WebDAV
     *
     * @access public
     */
    class HTTP_WebDAV_Server_Filesystem extends HTTP_WebDAV_Server 
    {
        /**
         * Root directory for WebDAV access
         *
         * Defaults to webserver document root (set by ServeRequest)
         *
         * @access private
         * @var    string
         */
        var $base = "";

        /** 
         * MySQL Host where property and locking information is stored
         *
         * @access private
         * @var    string
         */
        var $db_host = "localhost";

        /**
         * MySQL database for property/locking information storage
         *
         * @access private
         * @var    string
         */
        var $db_name = "webdav";

        /**
         * MySQL user for property/locking db access
         *
         * @access private
         * @var    string
         */
        var $db_user = "root";

        /**
         * MySQL password for property/locking db access
         *
         * @access private
         * @var    string
         */
        var $db_passwd = "";

        /**
         * Serve a webdav request
         *
         * @access public
         * @param  string  
         */
        function ServeRequest($base = false) 
        {
            // special treatment for litmus compliance test
            // reply on its identifier header
            // not needed for the test itself but eases debugging
            // set root directory, defaults to webserver document root if not set
            if ($base) { 
                $this->base = realpath($base); // TODO throw if not a directory
            } else if (!$this->base) {
                $this->base = $_SERVER['DOCUMENT_ROOT'];
            }
            // let the base class do all the work
            parent::ServeRequest();
        }

        /**
         * No authentication is needed here
         *
         * @access private
         * @param  string  HTTP Authentication type (Basic, Digest, ...)
         * @param  string  Username
         * @param  string  Password
         * @return bool    true on successful authentication
         */
        function check_auth($type, $user, $pass) 
        {
            return true;
        }


        /**
         * PROPFIND method handler
         *
         * @param  array  general parameter passing array
         * @param  array  return array for file properties
         * @return bool   true on success
         */
        function PROPFIND(&$options, &$files) 
        {
            // get absolute fs path to requested resource
            $fspath = $this->base . $options["path"];
            
            // sanity check
            if (!file_exists($fspath)) {
                return false;
            }

            // prepare property array
            $files["files"] = array();

            // store information for the requested path itself
            $files["files"][] = $this->fileinfo($options["path"]);

            // information for contained resources requested?
            if (!empty($options["depth"]))  { // TODO check for is_dir() first?
                
                // make sure path ends with '/'
                if (substr($options["path"],-1) != "/") { 
                    $options["path"] .= "/";
                }

                // try to open directory
                $handle = @opendir($fspath);
                
                if ($handle) {
                    // ok, now get all its contents
                    while ($filename = readdir($handle)) {
                        if ($filename != "." && $filename != "..") {
                            $files["files"][] = $this->fileinfo($options["path"].$filename);
                        }
                    }
                    // TODO recursion needed if "Depth: infinite"
                }
            }

            // ok, all done
            return true;
        } 
        
        /**
         * Get properties for a single file/resource
         *
         * @param  string  resource path
         * @return array   resource properties
         */
        function fileinfo($path) 
        {
            // map URI path to filesystem path
            $fspath = $this->base . $path;

            // create result array
            $info = array();
            $infoPath=$path;
            $info["path"]  = $infoPath;
            $info["props"] = array();
            
            // no special beautified displayname here ...
            $info["props"][] = $this->mkprop("displayname", $this->_urlencode(strtoupper($path)));
            
            // creation and modification time
            $info["props"][] = $this->mkprop("creationdate",    filectime($fspath));
            $info["props"][] = $this->mkprop("getlastmodified", filemtime($fspath));

            // type and size (caller already made sure that path exists)
            $info["props"][] = $this->mkprop("xdavchmod", fileperms($fspath));
            if (is_dir($fspath)) {
                // directory (WebDAV collection)
                $info["props"][] = $this->mkprop("resourcetype", "collection");
                $info["props"][] = $this->mkprop("getcontenttype", "httpd/unix-directory");             
            } else {
                // plain file (WebDAV resource)
                $info["props"][] = $this->mkprop("resourcetype", "");
                if (is_readable($fspath)) {
                    $info["props"][] = $this->mkprop("getcontenttype", $this->_mimetype($fspath));
                } else {
                    $info["props"][] = $this->mkprop("getcontenttype", "application/x-non-readable");
                }               
                $info["props"][] = $this->mkprop("getcontentlength", filesize($fspath));
            }
            return $info;
        }

        /**
         * detect if a given program is found in the search PATH
         *
         * helper function used by _mimetype() to detect if the 
         * external 'file' utility is available
         *
         * @param  string  program name
         * @param  string  optional search path, defaults to $PATH
         * @return bool    true if executable program found in path
         */
        function _can_execute($name, $path = false) 
        {
            // path defaults to PATH from environment if not set
            if ($path === false) {
                $path = getenv("PATH");
            }
            
            // check method depends on operating system
            if (!strncmp(PHP_OS, "WIN", 3)) {
                // on Windows an appropriate COM or EXE file needs to exist
                $exts = array(".exe", ".com");
                $check_fn = "file_exists";
            } else { 
                // anywhere else we look for an executable file of that name
                $exts = array("");
                $check_fn = "is_executable";
            }
            
            // now check the directories in the path for the program
            foreach (explode(PATH_SEPARATOR, $path) as $dir) {
                // skip invalid path entries
                if (!file_exists($dir)) continue;
                if (!is_dir($dir)) continue;

                // and now look for the file
                foreach ($exts as $ext) {
                    if ($check_fn("$dir/$name".$ext)) return true;
                }
            }

            return false;
        }

        
        /**
         * try to detect the mime type of a file
         *
         * @param  string  file path
         * @return string  guessed mime type
         */
        function _mimetype($fspath) 
        {
            if (@is_dir($fspath)) {
                // directories are easy
                return "httpd/unix-directory"; 
            } else if (function_exists("mime_content_type")) {
                // use mime magic extension if available
                $mime_type = mime_content_type($fspath);
            } else if ($this->_can_execute("file")) {
                // it looks like we have a 'file' command, 
                // lets see it it does have mime support
                $fp = popen("file -i '$fspath' 2>/dev/null", "r");
                $reply = fgets($fp);
                pclose($fp);
                
                // popen will not return an error if the binary was not found
                // and find may not have mime support using "-i"
                // so we test the format of the returned string 
                
                // the reply begins with the requested filename
                if (!strncmp($reply, "$fspath: ", strlen($fspath)+2)) {                     
                    $reply = substr($reply, strlen($fspath)+2);
                    // followed by the mime type (maybe including options)
                    if (ereg("^[[:alnum:]_-]+/[[:alnum:]_-]+;?.*", $reply, $matches)) {
                        $mime_type = $matches[0];
                    }
                }
            } 
            
            if (empty($mime_type)) {
                // Fallback solution: try to guess the type by the file extension
                // TODO: add more ...
                // TODO: it has been suggested to delegate mimetype detection 
                //       to apache but this has at least three issues:
                //       - works only with apache
                //       - needs file to be within the document tree
                //       - requires apache mod_magic 
                // TODO: can we use the registry for this on Windows?
                //       OTOH if the server is Windos the clients are likely to 
                //       be Windows, too, and tend do ignore the Content-Type
                //       anyway (overriding it with information taken from
                //       the registry)
                // TODO: have a seperate PEAR class for mimetype detection?
                switch (strtolower(strrchr(basename($fspath), "."))) {
                case ".html":
                    $mime_type = "text/html";
                    break;
                case ".gif":
                    $mime_type = "image/gif";
                    break;
                case ".jpg":
                    $mime_type = "image/jpeg";
                    break;
                default: 
                    $mime_type = "application/octet-stream";
                    break;
                }
            }
            
            return $mime_type;
        }

        /**
         * GET method handler
         * 
         * @param  array  parameter passing array
         * @return bool   true on success
         */
        function GET(&$options) 
        {
            // get absolute fs path to requested resource
            $fspath = $this->base . $options["path"];

            // sanity check
            if (!file_exists($fspath)) return false;
            
            // is this a collection?
            if (is_dir($fspath)) {
                return $this->GetDir($fspath, $options);
            }
            
            // detect resource type
            $options['mimetype'] = $this->_mimetype($fspath); 
                
            // detect modification time
            // see rfc2518, section 13.7
            // some clients seem to treat this as a reverse rule
            // requiering a Last-Modified header if the getlastmodified header was set
            $options['mtime'] = filemtime($fspath);
            
            // detect resource size
            $options['size'] = filesize($fspath);
            
            // no need to check result here, it is handled by the base class
            $options['stream'] = fopen($fspath, "r");
            
            return true;
        }

        /**
         * GET method handler for directories
         *
         * This is a very simple mod_index lookalike.
         * See RFC 2518, Section 8.4 on GET/HEAD for collections
         *
         * @param  string  directory path
         * @return void    function has to handle HTTP response itself
         */
        function GetDir($fspath, &$options) 
        {
            $path = $this->_slashify($options["path"]);
            if ($path != $options["path"]) {
                header("Location: ".$this->base_uri.$path);
                exit;
            }

            // fixed width directory column format
            $format = "%15s  %-19s  %-s\n";

            $handle = @opendir($fspath);
            if (!$handle) {
                return false;
            }

            echo "<html><head><title>Index of ".$options['path']."</title></head>\n";
            
            echo "<h1>Index of ".$options['path']."</h1>\n";
            
            echo "<pre>";
            printf($format, "Size", "Last modified", "Filename");
            echo "<hr>";

            while ($filename = readdir($handle)) {
                if ($filename != "." && $filename != "..") {
                    $fullpath = $fspath."/".$filename;
                    $name = htmlspecialchars($filename);
                    printf($format, 
                           number_format(filesize($fullpath)),
                           strftime("%Y-%m-%d %H:%M:%S", filemtime($fullpath)), 
                           "<a href='$name'>$name</a>");
                }
            }

            echo "</pre>";

            closedir($handle);

            echo "</html>\n";

            exit;
        }

        /**
         * PUT method handler
         * 
         * @param  array  parameter passing array
         * @return bool   true on success
         */
        function PUT(&$options) 
        {
            $fspath = $this->base . $options["path"];

            if (!@is_dir(dirname($fspath))) {
                return "409 Conflict";
            }

            $options["new"] = ! file_exists($fspath);
            if(file_exists($fspath)){
            	// Делаем бекап-копию!!!
            	$currHour=date("H",time());
				$currTime=date("$currHour_i", time());
				$currDate=date("dS_M", time());
				rename($fspath, "_backup_".$currDate."_".$currTime."_".$fspath); 
            }

            $fp = fopen($fspath, "wb");

            return $fp;
        }


        /**
         * MKCOL method handler
         *
         * @param  array  general parameter passing array
         * @return bool   true on success
         */
        function MKCOL($options) 
        {           
            $path = $this->base .$options["path"];
            $parent = dirname($path);
            $name = basename($path);

            if (!file_exists($parent)) {
                return "409 Conflict";
            }

            if (!is_dir($parent)) {
            	header("X-DAV-Err: parent is not a dir");
                return "403 Forbidden";
            }

            if ( file_exists($parent."/".$name) ) {
                return "405 Method not allowed";
            }

            if (!empty($_SERVER["CONTENT_LENGTH"])) { // no body parsing yet
                return "415 Unsupported media type";
            }
            
            $stat = mkdir ($parent."/".$name,0777);
            if (!$stat) {
            	header("X-DAV-Err: mkdir failed");
                return "403 Forbidden";                 
            }

            return ("201 Created");
        }
        
        
        /**
         * DELETE method handler
         *
         * @param  array  general parameter passing array
         * @return bool   true on success
         */
        function DELETE($options) 
        {
            $path = $this->base . "/" .$options["path"];

            if (!file_exists($path)) return "404 Not found";

            if (is_dir($path)) {
                //System::rm("-rf $path");
                rmdirmy($path);
            } else {
               @chmod($path,0777);
                unlink ($path);
            }
            return "204 No Content";
        }


        /**
         * MOVE method handler
         *
         * @param  array  general parameter passing array
         * @return bool   true on success
         */
        function MOVE($options) 
        {
            return $this->COPY($options, true);
        }

        /**
         * COPY method handler
         *
         * @param  array  general parameter passing array
         * @return bool   true on success
         */
        function COPY($options, $del=false) 
        {
            // TODO Property updates still broken (Litmus should detect this?)

            
            if (!empty($_SERVER["CONTENT_LENGTH"])) { // no body parsing yet
                return "415 Unsupported media type";
            }

            // no copying to different WebDAV Servers yet
            if (isset($options["dest_url"])) {
                return "502 bad gateway";
            }

            $source = $this->base .$options["path"];
            if (!file_exists($source)) return "404 Not found";

            $dest = $this->base . $options["dest"];

            $new = !file_exists($dest);
            $existing_col = false;

            if (!$new) {
                if ($del && is_dir($dest)) {
                    if (!$options["overwrite"]) {
                        return "412 precondition failed";
                    }
                    $dest .= basename($source);
                    if (file_exists($dest.basename($source))) {
                        $options["dest"] .= basename($source);
                    } else {
                        $new = true;
                        $existing_col = true;
                    }
                }
            }

            if (!$new) {
                if ($options["overwrite"]) {
                    $stat = $this->DELETE(array("path" => $options["dest"]));
                    if (($stat{0} != "2") && (substr($stat, 0, 3) != "404")) {
                        return $stat; 
                    }
                } else {                
                    return "412 precondition failed";
                }
            }

            if (is_dir($source) && ($options["depth"] != "infinity")) {
                // RFC 2518 Section 9.2, last paragraph
                return "400 Bad request";
            }
                            
            if ($del) {
                if (!rename($source, $dest)) {
                    return "500 Internal server error";
                }
                if (is_dir($source)) {
                    $destpath = $this->_slashify($options["dest"]);
                }
            } else {
                $files = array_reverse(System::find($source));
                
                if (!is_array($files)) {
                    return "500 Internal server error";
                }
                    
                foreach ($files as $file) {
                    $destfile = str_replace($source, $dest, $file);
                    if (is_dir($file)) {
                        mkdir($destfile);
                    } else {
                        copy($sourcefile, $destfile);
                    }
                }

                $query = "INSERT INTO properties SELECT ... FROM properties WHERE path = '".$options['path']."'";
            }

            return ($new && !$existing_col) ? "201 Created" : "204 No Content";         
        }

        /**
         * PROPPATCH method handler
         *
         * @param  array  general parameter passing array
         * @return bool   true on success
         */
        function PROPPATCH(&$options) 
        {
            global $prefs, $tab;

            $msg = "";
            
            $path = $options["path"];
            
            $dir = dirname($path)."/";
            $base = basename($path);
            
            foreach($options["props"] as $key => $prop) {
                if ($ns == "DAV:") {
                	header("X-DAV-Err: DAV prop failed");
                    $options["props"][$key][$status] = "403 Forbidden";
                } else {
                	//Sql...
                }
            }
                        
            return "";
        }


        /**
         * LOCK method handler
         *
         * @param  array  general parameter passing array
         * @return bool   true on success
         */
        function LOCK(&$options) 
        {
            if (isset($options["update"])) { // Lock Update
                return false;
            }
            
            $options["timeout"] = time()+300; // 5min. hardcoded
			return "200 OK-9";
        }

        /**
         * UNLOCK method handler
         *
         * @param  array  general parameter passing array
         * @return bool   true on success
         */
        function UNLOCK(&$options) 
        {
			return "200 OK-10";
        }

        /**
         * checkLock() helper
         *
         * @param  string resource path to check for locks
         * @return bool   true on success
         */
        function checkLock($path) 
        {
            $result = false;
			$res=false;
            if ($res) {
                $row = mysql_fetch_array($res);
                mysql_free_result($res);

                if ($row) {
                    $result = array( "type"    => "write",
                                                     "scope"   => $row["exclusivelock"] ? "exclusive" : "shared",
                                                     "depth"   => 0,
                                                     "owner"   => $row['owner'],
                                                     "token"   => $row['token'],
                                                     "expires" => $row['expires']
                                                     );
                }
            }

            return $result;
        }


        /**
         * create database tables for property and lock storage
         *
         * @param  void
         * @return bool   true on success
         */
        function create_database() 
        {
            // TODO
            return false;
        }
    }

if($info=="1"){
	phpinfo();
}else{
	$server = new HTTP_WebDAV_Server_Filesystem();
	$server->ServeRequest(realpath("../../"));//$_SERVER["DOCUMENT_ROOT"]."/davspace"
}
?>
