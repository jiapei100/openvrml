<?php // -*-php-*-
rcs_id('$Id: ADODB.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');

require_once('lib/WikiDB.php');

/**
 * WikiDB layer for ADODB, which does nothing more than calling the mysql-specific ADODB backend.
 * Support for a newer adodb library, the adodb extension library 
 * and more databases will come with PhpWiki v1.3.9
 *
 * @author: Lawrence Akka
 */
class WikiDB_ADODB extends WikiDB
{
    function WikiDB_ADODB ($dbparams) {
        $backend_type = 'mysql';   // default value.  FIXME
        /* if (preg_match('/^(\w+):/', $dbparams['dsn'], $m))
             $backend_type = $m[1];  */
        include_once("lib/WikiDB/backend/ADODB_$backend_type.php");
        $backend_class = "WikiDB_backend_ADODB_$backend_type";
        $backend = new $backend_class($dbparams);

        $this->WikiDB($backend, $dbparams);
    }
    
    
    /**
     * Determine whether page exists (in non-default form).
     * @see WikiDB::isWikiPage
     */
    function isWikiPage ($pagename) {
        /*
        if (empty($this->_iwpcache))
            $this->_iwpcache = array_flip($this->_backend->get_all_pagenames());
        return isset($this->_iwpcache[$pagename]);
        */

        if (!isset($this->_iwpcache[$pagename]))
            $this->_iwpcache[$pagename] = $this->_backend->is_wiki_page($pagename);
        return $this->_iwpcache[$pagename];
        
        // Talk to the backend directly for max speed.
        /*
        $pagedata = $this->_cache->get_pagedata($pagename);
        return !empty($pagedata[':non_default']);
        */
    }
};

  
// For emacs users
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:

?>
