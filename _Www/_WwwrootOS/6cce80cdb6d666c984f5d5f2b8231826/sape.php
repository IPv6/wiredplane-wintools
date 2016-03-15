<?php
/*
 * SAPE.ru -- Интеллектуальная система купли-продажи ссылок
 * 
 * PHP-клиент, версия 0.6 от 04.06.2007
 *
 * По всем вопросам обращайтесь на support@sape.ru
 *
 * Вебмастеры! Не нужно ничего менять в этом файле! Все настройки - через параметры при вызове кода.
 * Читайте: http://www.sape.ru/faq.php
 *
 */

class SAPE_client {
    
    var $_version           = '0.6';
    
    var $_verbose           = false;
    
    var $_charset           = '';               // http://www.php.net/manual/en/function.iconv.php
    
    var $_server            = 'dispenser-01.sape.ru';
    
    var $_cache_lifetime    = 3600;             // Пожалейте наш сервер :о)
    
    // Если скачать базу ссылок не удалось, то следующая попытка будет через столько секунд
    var $_cache_reloadtime  = 600;
    
    var $_links_db_file     = '';
    
    var $_links             = array();
    
    var $_links_page        = array();
    
    var $_links_delimiter   = '';
    
    var $_error             = '';
    
    var $_host              = '';
    
    var $_request_uri       = '';
    
    var $_multi_site        = false;
    
    var $_fetch_remote_type = '';              // Способ подключения к удалённому серверу [file_get_contents|curl|socket]
    
    var $_socket_timeout    = 6;               // Сколько ждать ответа
    
    
    function SAPE_client($options = null) {
        
        // Поехали :o)
        
        $host = '';
        
        if (is_array($options)) {
            if (isset($options['host'])) {
                $host = $options['host'];
            }
        } elseif (strlen($options)) {
            $host = $options;
            $options = array();
        } else {
            $options = array();
        }
        
        // Какой сайт?
        if (strlen($host)) {
            $this->_host = $host;
        } else {
            $this->_host = $_SERVER['HTTP_HOST'];
        }
        
        $this->_host = preg_replace('/^http:\/\//', '', $this->_host);
        $this->_host = preg_replace('/^www\./', '', $this->_host);
        
        // Какая страница?
        if (isset($options['request_uri']) && strlen($options['request_uri'])) {
            $this->_request_uri = $options['request_uri'];
        } else {
            $this->_request_uri = $_SERVER['REQUEST_URI'];
        }
        
        // На случай, если хочется много сайтов в одной папке
        if (isset($options['multi_site']) && $options['multi_site'] == true) {
            $this->_multi_site = true;
        }
        
        // Сообщать об ошибках
        if (isset($options['verbose']) && $options['verbose'] == true) {
            $this->_verbose = true;
        }
        
        // Кодировка
        if (isset($options['charset']) && strlen($options['charset'])) {
            $this->_charset = $options['charset'];
        }
        
        if (isset($options['fetch_remote_type']) && strlen($options['fetch_remote_type'])) {
            $this->_fetch_remote_type = $options['fetch_remote_type'];
        }
        
        if (isset($options['socket_timeout']) && is_numeric($options['socket_timeout']) && $options['socket_timeout'] > 0) {
            $this->_socket_timeout = $options['socket_timeout'];
        }
        
        if (!defined('_SAPE_USER')) {
            return $this->raise_error('Не задана константа _SAPE_USER');
        }
        
        $this->load_links();
    }
    
    
    function load_links() {
        
        // Для сайтов, которые лежат в одной папке
        if ($this->_multi_site) {
            $this->_links_db_file = dirname(__FILE__) . '/' . $this->_host . '.links.db';
        } else {
            $this->_links_db_file = dirname(__FILE__) . '/links.db';
        }
        
        if (!is_file($this->_links_db_file)) {
            // Пытаемся создать файл.
            if (@touch($this->_links_db_file)) {
                @chmod($this->_links_db_file, 0666);    // Права доступа
            } else {
                return $this->raise_error('Нет файла ' . $this->_links_db_file . '. Создать не удалось. Выставите права 777 на папку.');
            }
        } 
        
        if (!is_writable($this->_links_db_file)) {
            return $this->raise_error('Нет доступа на запись к файлу: ' . $this->_links_db_file . '! Выставите права 777 на папку.');
        }
        
        @clearstatcache();
        
        if (filemtime($this->_links_db_file) < (time()-$this->_cache_lifetime) || filesize($this->_links_db_file) == 0) {
            
            // Чтобы не повесить площадку клиента и чтобы не было одновременных запросов
            @touch($this->_links_db_file, (time() - $this->_cache_lifetime + $this->_cache_reloadtime));
            
            $path = '/code.php?user=' . _SAPE_USER . '&host=' . $this->_host;
            if (strlen($this->_charset)) {
                $path .= '&charset=' . $this->_charset;
            }
            
            if ($links = $this->fetch_remote_file($this->_server, $path)) {
                if (substr($links, 0, 12) == 'FATAL ERROR:') {
                    $this->raise_error($links);
                } else {
                    // [псевдо]проверка целостности:
                    if (@unserialize($links) != false) {
                        $this->_write($this->_links_db_file, $links);
                    }
                }
            }
        }
        
        if ($links = $this->_read($this->_links_db_file)) {
            if (!$this->_links = @unserialize($links)) {
                $this->_links = array();
            }
        } else {
            $this->_links = array();
        }
        
        // Убиваем PHPSESSID
        if (strlen(session_id())) {
            $session = session_name() . '=' . session_id();
            $this->_request_uri = str_replace(array('?'.$session,'&'.$session), '', $this->_request_uri);
        }
            
        if (isset($this->_links['__sape_delimiter__'])) {
            $this->_links_delimiter = $this->_links['__sape_delimiter__'];
        }
        
        if (array_key_exists($this->_request_uri, $this->_links) && is_array($this->_links[$this->_request_uri])) {
            $this->_links_page = $this->_links[$this->_request_uri];
        } else {
            if (isset($this->_links['__sape_new_url__']) && strlen($this->_links['__sape_new_url__'])) {
                $this->_links_page = $this->_links['__sape_new_url__'];
            }
        }
    }
    
    /*
     * Ccылки можно показывать по частям
     *
     */
    function return_links($n = null) {
        
        if (is_array($this->_links_page)) {
            
            $total_page_links = count($this->_links_page);
            
            if (!is_numeric($n) || $n > $total_page_links) {
                $n = $total_page_links;
            }
            
            $links = array();
            
            for ($i = 1; $i <= $n; $i++) {
                $links[] = array_shift($this->_links_page);
            }
            
            $html = join($this->_links_delimiter, $links);
            
            return $html;
            
        } else {
            return $this->_links_page;
        }
        
    }
    
    /*
     * Функция для подключения к удалённому серверу
     */
    function fetch_remote_file($host, $path) {
        
        $user_agent = 'SAPE_Client PHP ' . $this->_version;
        
        @ini_set('allow_url_fopen',          1);
        @ini_set('default_socket_timeout',   $this->_socket_timeout);
        @ini_set('user_agent',               $user_agent);
        
        if (
            $this->_fetch_remote_type == 'file_get_contents'
            ||
            (
                $this->_fetch_remote_type == ''
                &&
                function_exists('file_get_contents')
                &&
                ini_get('allow_url_fopen') == 1
            )
        ) {
            
            if ($data = @file_get_contents('http://' . $host . $path)) {
                return $data;
            }
        
        } elseif (
            $this->_fetch_remote_type == 'curl' 
            ||
            (
                $this->_fetch_remote_type == ''
                &&
                function_exists('curl_init')
            )
        ) {
            
            if ($ch = @curl_init()) {

                @curl_setopt($ch, CURLOPT_URL,              'http://' . $host . $path);
                @curl_setopt($ch, CURLOPT_HEADER,           false);
                @curl_setopt($ch, CURLOPT_RETURNTRANSFER,   true);
                @curl_setopt($ch, CURLOPT_CONNECTTIMEOUT,   $this->_socket_timeout);
                @curl_setopt($ch, CURLOPT_USERAGENT,        $user_agent);
                
                if ($data = @curl_exec($ch)) {
                    return $data;
                }
                
                @curl_close($ch);
            }
            
        } else {
            
            $buff = '';
            $fp = @fsockopen($this->_server, 80, $errno, $errstr, $this->_socket_timeout);
            if ($fp) {
                @fputs($fp, "GET {$path} HTTP/1.0\r\nHost: {$host}\r\n");
                @fputs($fp, "User-Agent: {$user_agent}\r\n\r\n");
                while (!@feof($fp)) {
                    $buff .= @fgets($fp, 128);
                }
                @fclose($fp);
                
                $page = explode("\r\n\r\n", $buff);
            
                return $page[1];
            }
            
        }
        
        return $this->raise_error('Не могу подключиться к серверу: ' . $host . $path);
    }
    
    /*
     * Функция чтения из локального файла
     */
    function _read($filename) {
        
        $fp = @fopen($filename, 'rb');
        @flock($fp, LOCK_SH);
        if ($fp) {
            clearstatcache();
            $length = @filesize($filename);
            $mqr = get_magic_quotes_runtime();
            set_magic_quotes_runtime(0);
            if ($length) {
                $data = @fread($fp, $length);
            } else {
                $data = '';
            }
            set_magic_quotes_runtime($mqr);
            @flock($fp, LOCK_UN);
            @fclose($fp);
            
            return $data;
        }
        
        return $this->raise_error('Не могу считать данные из файла: ' . $filename);  
    }
    
    /*
     * Функция записи в локальный файл
     */
    function _write($filename, $data) {
        
        $fp = @fopen($filename, 'wb');
        if ($fp) {
            @flock($fp, LOCK_EX);
            $length = strlen($data);
            @fwrite($fp, $data, $length);
            @flock($fp, LOCK_UN);
            @fclose($fp);
            
            if (md5($this->_read($filename)) != md5($data)) {
                return $this->raise_error('Нарушена целостность данных при записи в файл: ' . $filename); 
            }
            
            return true;
        }
        
        return $this->raise_error('Не могу записать данные в файл: ' . $filename); 
    }
    
    /*
     * Функция обработки ошибок
     */
    function raise_error($e) {
        
        $this->_error = '<p style="color: red; font-weight: bold;">SAPE ERROR: ' . $e . '</p>';
        
        if ($this->_verbose == true) {
            print $this->_error;
        }
        
        return false;
    }
}

?>