<?php

rcs_id( '$Id: cvs.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $' );

require_once( 'lib/WikiDB.php' );
require_once( 'lib/WikiDB/backend/cvs.php' );

/**
 * Wrapper class for the cvs backend.
 *
 * Author: Gerrit Riessen, gerrit.riessen@open-source-consultants.de
 */
class WikiDB_cvs
extends WikiDB
{  
    var $_backend;

    /**
     * Constructor requires the DB parameters. 
     */
    function WikiDB_cvs( $dbparams ) 
    {
        $this->_backend = new WikiDB_backend_cvs( $dbparams );
    }
}
?>