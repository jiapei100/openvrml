<?php // -*-php-*-
rcs_id('$Id: PluginManager.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/**
 Copyright 1999, 2000, 2001, 2002 $ThePhpWikiProgrammingTeam

 This file is part of PhpWiki.

 PhpWiki is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 PhpWiki is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with PhpWiki; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Set this to true if you don't want regular users to view this page.
// So far there are no known security issues.
define('REQUIRE_ADMIN', false);

class WikiPlugin_PluginManager
extends WikiPlugin
{
    function getName () {
        return _("PluginManager");
    }

    function getDescription () {
        return _("Description: Provides a list of plugins on this wiki.");
    }

    function getVersion() {
        return preg_replace("/[Revision: $]/", '',
                            "\$Revision: 1.1.1.1 $");
    }

    function getDefaultArguments() {
        return array('info' => 'args');
    }

    function run($dbi, $argstr, &$request, $basepage) {
        extract($this->getArgs($argstr, $request));

        $h = HTML();
        $this->_generatePageheader($info, $h);

        if (! REQUIRE_ADMIN || $request->_user->isadmin()) {
            $h->pushContent(HTML::h2(_("Plugins")));

            $table = HTML::table(array('class' => "pagelist"));
            $this->_generateColgroups($info, $table);
            $this->_generateColheadings($info, $table);
            $this->_generateTableBody($info, $dbi, $request, $table);
            $h->pushContent($table);

            //$h->pushContent(HTML::h2(_("Disabled Plugins")));
        }
        else {
            $h->pushContent(fmt("You must be an administrator to %s.",
                                _("use this plugin")));
        }
        return $h;
    }

    function _generatePageheader(&$info, &$html) {
        $html->pushContent(HTML::p($this->getDescription()));
    }

    function _generateColgroups(&$info, &$table) {
        // specify last two column widths
        $colgroup = HTML::colgroup();
        $colgroup->pushContent(HTML::col(array('width' => '0*')));
        $colgroup->pushContent(HTML::col(array('width' => '0*',
                                               'align' => 'right')));
        $colgroup->pushContent(HTML::col(array('width' => '9*')));
        if ($info == 'args')
            $colgroup->pushContent(HTML::col(array('width' => '2*')));
        $table->pushcontent($colgroup);
    }

    function _generateColheadings(&$info, &$table) {
        // table headings
        $tr = HTML::tr();
        $headings = array(_("Plugin"), _("Version"), _("Description"));
        if ($info == 'args')
            $headings []= _("Arguments");
        foreach ($headings as $title) {
            $tr->pushContent(HTML::td($title));
        }
        $table->pushContent(HTML::thead($tr));
    }

    function _generateTableBody(&$info, &$dbi, &$request, &$table) {
        $plugin_dir = 'lib/plugin';
        if (defined('PHPWIKI_DIR'))
            $plugin_dir = PHPWIKI_DIR . "/$plugin_dir";
        $pd = new fileSet($plugin_dir, '*.php');
        $plugins = $pd->getFiles();
        unset($pd);
        sort($plugins);

        // table body
        $tbody = HTML::tbody();
        $row_no = 0;

        $w = new WikiPluginLoader;
        foreach($plugins as $pluginName) {
            // instantiate a plugin
            $pluginName = str_replace(".php", "", $pluginName);
            $temppluginclass = "<? plugin $pluginName ?>"; // hackish
            $p = $w->getPlugin($pluginName, false); // second arg?
            // trap php files which aren't WikiPlugin~s
            if (!substr(get_parent_class($p), 0, 10) == 'wikiplugin') {
                // Security: Hide names of extraneous files within
                // plugin dir from non-admins.
                if ($request->_user->isadmin())
                    trigger_error(sprintf(_("%s does not appear to be a WikiPlugin."),
                                          $pluginName . ".php"));
                continue; // skip this non WikiPlugin file
            }
            $desc = $p->getDescription();
            $ver = $p->getVersion();
            $args = $p->getDefaultArguments();
            unset($p); //done querying plugin object, release from memory

            $arguments = HTML();
            foreach ($args as $arg => $default) {
                // Word around UserPreferences plugin to avoid error
                if ((is_array($default))) {
                    $default = '(array)';
                    // This is a bit flawed with UserPreferences object
                    //$default = sprintf("array('%s')",
                    //                   implode("', '", array_keys($default)));
                }
                else
                    if (stristr($default, ' '))
                        $default = "'$default'";
                $arguments->pushcontent("$arg=$default", HTML::br());
            }
            // make a link if an actionpage exists
            $pluginNamelink = $pluginName;
            $pluginDocPageNamelink = false;
            // Also look for pages in the current locale
            // Maybe FIXME? warn about case language != en and _(p) == "p"?
            if (_($pluginName) != $pluginName) {
                $localizedPluginName = _($pluginName);
            }
            else
                $localizedPluginName = '';
            $pluginNamelink = WikiLink($pluginName, 'if_known');
            // make another link for the localized plugin description
            // page if it exists
            $pluginDocPageName = $pluginName . "Plugin";
            // Also look for pages in the current locale
            if (_($pluginDocPageName) != $pluginDocPageName) {
                $localizedPluginDocPageName = _($pluginDocPageName);
            }
            else
                $localizedPluginDocPageName = '';

            if (isWikiWord($pluginDocPageName) && $dbi->isWikiPage($pluginDocPageName))
                {
                $pluginDocPageNamelink = HTML(WikiLink($pluginDocPageName));
            }
            else {
                // don't link to actionpages and plugins starting with
                // an _ from page list
                if (!preg_match("/^_/", $pluginName)
                    //&& !(@$request->isActionPage($pluginName)) //FIXME?
                    ) {
                    $pluginDocPageNamelink = WikiLink($pluginDocPageName,
                                                      'unknown');
                }
                else
                    $pluginDocPageNamelink = false;
            }
            // insert any found locale-specific pages at the bottom of
            // the td
            if ($localizedPluginName || $localizedPluginDocPageName) {
                $par = HTML::p();
                if ($localizedPluginName) {
                    // Don't offer to create a link to a non-wikiword
                    // localized plugin page but show those that
                    // already exist (Calendar, Comment, etc.)  (Non
                    // non-wikiword plugins are okay, they just can't
                    // become actionPages.)
                    if (isWikiWord($localizedPluginName)
                        || $dbi->isWikiPage($localizedPluginName))
                        {
                        $par->pushContent(WikiLink($localizedPluginName,
                                                   'auto'));
                    }
                    else {
                        // probably incorrectly translated, so no page
                        // link
                        $par->pushContent($localizedPluginName, ' '
                                          . _("(Not a WikiWord)"));
                    }
                }
                if ($localizedPluginName && $localizedPluginDocPageName)
                    $par->pushContent(HTML::br());
                if ($localizedPluginDocPageName) {
                    if (isWikiWord($localizedPluginDocPageName)
                        || $dbi->isWikiPage($localizedPluginDocPageName))
                        {
                        $par->pushContent(WikiLink($localizedPluginDocPageName,
                                                   'auto'));
                    }
                    else {
                        // probably incorrectly translated, so no page
                        // link
                        $par->pushContent($localizedPluginDocPageName, ' '
                                          . _("(Not a WikiWord)"));
                    }
                }
                $pluginDocPageNamelink->pushContent($par);
            }

            // highlight alternate rows
            $row_no++;
            $group = (int)($row_no / 1); //_group_rows
            $class = ($group % 2) ? 'evenrow' : 'oddrow';
            // generate table row
            $tr = HTML::tr(array('class' => $class));
            if ($pluginDocPageNamelink) {
                // plugin has a description page 'PluginName' . 'Plugin'
                $tr->pushContent(HTML::td($pluginNamelink, HTML::br(),
                                          $pluginDocPageNamelink));
                $pluginDocPageNamelink = false;
                //$row_no++;
            }
            else {
                // plugin just has an actionpage
                $tr->pushContent(HTML::td($pluginNamelink));
            }
            $tr->pushContent(HTML::td($ver), HTML::td($desc));
            if ($info == 'args') {
                // add Arguments column
                $style = array('style'
                               => 'font-family:monospace;font-size:smaller');
                $tr->pushContent(HTML::td($style, $arguments));
            }
            $tbody->pushContent($tr);
        }
        $table->pushContent($tbody);
    }
};

// $Log: not supported by cvs2svn $
// Revision 1.14  2004/02/17 12:11:36  rurban
// added missing 4th basepage arg at plugin->run() to almost all plugins. This caused no harm so far, because it was silently dropped on normal usage. However on plugin internal ->run invocations it failed. (InterWikiSearch, IncludeSiteMap, ...)
//
// Revision 1.13  2004/01/25 03:58:44  rurban
// use stdlib:isWikiWord()
//
// Revision 1.12  2004/01/04 18:14:49  wainstead
// Added "Description:" to the beginning of the description string, so
// the plugin plays nice with surrounding text.
//
// Revision 1.11  2003/12/10 01:01:24  carstenklapp
// New features: Also show plugin pages for localized variants.
// Gracefully handle broken plugins in the plugins folder (such as other
// lingering php files).
//
// Bugfix: Cleaned up Php warnings related to oddities of UserPreference
// plugin (whose default value contains an array).
//
// Internal changes: Gave GoodVariableNames to the nightmarish
// ones. Simplified some code with WikiLink 'if_known'.
//
// Revision 1.10  2003/11/30 18:23:48  carstenklapp
// Code housekeeping: PEAR coding standards reformatting only.
//
// Revision 1.9  2003/11/19 00:02:42  carstenklapp
// Include found locale-specific pages for the current (non-English)
// locale.
//
// Revision 1.8  2003/11/15 21:53:53  wainstead
// Minor change: list plugins in asciibetical order. It'd be better if
// they were alphabetical.
//
// Revision 1.7  2003/02/24 01:36:25  dairiki
// Don't use PHPWIKI_DIR unless it's defined.
// (Also typo/bugfix in SystemInfo plugin.)
//
// Revision 1.6  2003/02/24 00:56:53  carstenklapp
// Updated to work with recent changes to WikiLink function (fix
// "==Object(wikipagename)==" for unknown wiki links).
//
// Revision 1.5  2003/02/22 20:49:56  dairiki
// Fixes for "Call-time pass by reference has been deprecated" errors.
//
// Revision 1.4  2003/02/20 18:13:38  carstenklapp
// Workaround for recent changes to WikiPlugin->getPlugin.
// Made admin restriction for viewing this page optional.
// Now defaults to any user may view this page (mainly for PhpWiki Demo site).
// Minor code changes & reformatting.
//
// Revision 1.3  2003/01/04 02:30:12  carstenklapp
// Added 'info' argument to show / hide plugin "Arguments"
// column. Improved row highlighting and error message when viewed by
// non-admin user. Code refactored. Added copyleft.

// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>