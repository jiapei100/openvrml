<?php // -*-php-*-
rcs_id('$Id: _WikiTranslation.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/*
 Copyright 2004 $ThePhpWikiProgrammingTeam

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

/**
 * _WikiTranslation:  Display pagenames and other strings in various languages.
 * Can be used to let a favorite translation service translate a whole page. 
 * Current favorite: translate.google.com if from_lang = en or fr
 *
 * Examples:
 *  <?plugin _WikiTranslation page=HomePage languages=fr ?>
 *     Translation service for HomePage into french (redirect to translate.google.com)
 *  <?plugin _WikiTranslation what=pages ?>
 *     Translation matrix of all pages with proper translations (all in pgsrc)
 *  <?plugin _WikiTranslation what=wikiwords match="W*" limit=20 ?>
 *     Translation matrix of the first 20 wikiwords matching "W*"
 *  <?plugin _WikiTranslation string=HomePage languages=fr,de,sv ?>
 *     Translation matrix for all given languages
 *  <?plugin _WikiTranslation string=HomePage ?>
 *     Translation matrix for all supported languages
 *  <?plugin _WikiTranslation string=HomePage languages=fr ?>
 *     Just return the translated string for this language.
 *
 * @author:  Reini Urban
 */

/* Container for untranslated pagenames. Needed to show up in locale/po/phpwiki.pot */
$pgsrc_container = 
    _("AddCommentPlugin")  .','.
    _("AddingPages")  .','.
    _("AuthorHistoryPlugin") .','.
    _("BackLinks") .','.
    _("CalendarListPlugin") .','.
    _("CalendarPlugin") .','.
    _("CategoryCategory")  .','.
    _("CategoryHomePages")  .','.
    _("CommentPlugin")  .','.
    _("CreateTocPlugin")  .','.
    _("DebugInfo") .','.
    _("EditMetaDataPlugin") .','.
    _("ExternalSearchPlugin") .','.
    _("FindPage") .','.
    _("FrameIncludePlugin") .','.
    _("FullRecentChanges") .','.
    _("HelloWorldPlugin") .','.
    _("HomePageAlias") .','.
    _("IncludePagePlugin") .','.
    _("InterWiki") .','.
    _("LinkIcons") .','.
    _("MagicPhpWikiURLs") .','.
    _("MoreAboutMechanics") .','.
    _("NewMarkupTestPage") .','.
    _("OldMarkupTestPage") .','.
    _("OldStyleTablePlugin") .','.
    _("PageDump") .','.
    _("PageGroupTest") .','.
    _("PageGroupTestFour") .','.
    _("PageGroupTestOne") .','.
    _("PageGroupTestThree") .','.
    _("PageGroupTestTwo") .','.
    _("PgsrcTranslation") .','.
    _("PhotoAlbumPlugin") .','.
    _("PhpHighlightPlugin") .','.
    _("PhpWeatherPlugin") .','.
    _("PhpWiki") .','.
    _("PhpWikiAdministration/Chmod") .','.
    _("PhpWikiAdministration/Remove") .','.
    _("PhpWikiAdministration/Rename") .','.
    _("PhpWikiAdministration/Replace") .','.
    _("PhpWikiDocumentation") .','.
    _("PhpWikiPoll") .','.
    _("RawHtmlPlugin") .','.
    _("RecentVisitors") .','.
    _("RedirectToPlugin") .','.
    _("ReleaseNotes") .','.
    _("RichTablePlugin") .','.
    _("SteveWainstead") .','.
    _("SystemInfoPlugin") .','.
    _("TranscludePlugin") .','.
    _("TranslateText") .','.
    _("UnfoldSubpagesPlugin") .','.
    _("UpLoad") .','.
    _("WabiSabi") .','.
    _("WikiBlogPlugin") .','.
    _("WikiPlugin") .','.
    _("WikiWikiWeb");
 
require_once('lib/PageList.php');

class WikiPlugin__WikiTranslation
extends WikiPlugin
{

    function getName() {
        return _("_WikiTranslation");
    }

    function getDescription() {
        return _("Show translations of various words or pages");
    }

    function getVersion() {
        return preg_replace("/[Revision: $]/", '',
                            "\$Revision: 1.1.1.1 $");
    }

    function getDefaultArguments() {
        return 
            array( 'languages'  => '',  // comma delimited string of de,en,sv,...
                   'string'     => '',  
                   'page'       => '',  // use a translation service
                   'what'       => 'pages', // or 'buttons', 'plugins' or 'wikiwords'

                   'match'         => '*',
                   'from_lang'     => false,
                   'include_empty' => false,
                   'exclude'       => '',
                   'sortby'        => '',
                   'limit'         => 0,
                   'debug'         => false
                 );
    }

    function init_locale($lang) {
        if ($lang != $this->lang)
            update_locale($lang);
        // gettext module loaded: must load the LC_MESSAGES php hash
        if (function_exists ('bindtextdomain')) {
            include (FindLocalizedFile("LC_MESSAGES/phpwiki.php", 0,'reinit'));
        } elseif ($lang == 'en') {
            //hack alert! we need hash for stepping through it, even if it's in the wrong language
            include (FindFile("locale/de/LC_MESSAGES/phpwiki.php", 0,'reinit'));
            foreach ($locale as $en => $de) {
            	$locale[$en] = $en;
            }
        // we already have a $locale, but maybe it's in the wrong language
        } elseif ($lang != $this->lang or empty($GLOBALS['locale'])) {
            include (FindFile("LC_MESSAGES/phpwiki.php", 0,'reinit'));
        } else {
           $locale = & $GLOBALS['locale'];
        }
        $this->_locales[$lang] = $locale;
    }

    // reverse translation: 
    function translate_to_en($text,$lang=false) {
        if (!$lang) $lang = $this->lang; // current locale
        if ($lang == 'en') return $text;

        $this->_locales = array();
        $this->_reverse_locales = array();

        if (!isset($this->_locales[$lang])) {
            $this->init_locale($lang);
        }
        assert(!empty($this->_locales[$lang]));
        if (!isset($this->_reverse_locales[$lang])) {
            // and now do a reverse lookup in the $locale hash
            $this->_reverse_locales[$lang] = array_flip($this->_locales[$lang]);
        }
        if (!empty($this->_reverse_locales[$lang][$text])) {
            return $this->_reverse_locales[$lang][$text];
        } else {
            return $text;
        }
    }

    function translate($text,$to_lang,$from_lang=false) {
        if (!$from_lang) $from_lang = $this->lang; // current locale
        if ($from_lang == $to_lang) return $text;
        // speed up hash lookup. not needed for gettext module
        if (!isset($this->_locales[$from_lang]) and !function_exists('bindtextdomain')) {
            $this->init_locale($from_lang);
        }
        if ($from_lang != 'en') {
            // get reverse gettext: translate to english
            $en = $this->translate_to_en($text,$from_lang);
            // and then to target
            update_locale($to_lang);
            $result = gettext($en);
            update_locale($from_lang);
        } else {
            if ($from_lang != $to_lang) {
                update_locale($to_lang);
            }
            $result = gettext($text);
            if ($from_lang != $to_lang) {
                update_locale($from_lang);
            }
        }
        return $result;
    }
                
    function run($dbi, $argstr, $request, $basepage) {
        extract($this->getArgs($argstr, $request));
        $this->request = &$request;
        if (!$from_lang) $from_lang = $request->getPref('lang');
        if (!$from_lang) $from_lang = $GLOBALS['LANG'];
        $this->lang = $from_lang;

        if (empty($languages)) {
            // from lib/plugin/UserPreferences.php
            $available_languages = array();
            if ($from_lang != 'en')
                array_push($available_languages, 'en');
            $dir_root = 'locale/';
            if (defined('PHPWIKI_DIR'))
                $dir_root = PHPWIKI_DIR . "/$dir_root";
            $dir = dir($dir_root);
            if ($dir) {
                while ($entry = $dir->read()) {
                    if (is_dir($dir_root.$entry)
                        && (substr($entry,0,1) != '.')
                        && $entry != 'po'
                        && $entry != $from_lang
                        && $entry != 'CVS') {
                        array_push($available_languages, $entry);
                    }
                }
                $dir->close();
            }
            if (in_array($from_lang,$available_languages))
                $languages = $available_languages;
            else
            	$languages = array_merge($available_languages,array($from_lang));
        } elseif (strstr($languages,',')) {
            $languages = explode(',',$languages);
        } else {
            $languages = array($languages);
        }
        $to_lang = $languages[0];
        if (!empty($string) and count($languages)==1) {
            return $this->translate($string,$to_lang,$from_lang);
        }
        if (!empty($page)) {
            $pagename = $page;
            if ($dbi->isWikiPage($pagename)) {
            	$url = '';
            	// google can only translate from english and french
            	if (in_array($from_lang,array('en','fr'))) {
                    $url = "http://translate.google.com/translate";
                    $url .= "?langpair=" . urlencode($from_lang."|".$to_lang);
                    $url .= "&u=" . urlencode(WikiUrl($pagename,false,true));
            	}
            	// redirect or transclude?
	        if ($url)
	            return $request->redirect($url);
            	return HTML(fmt("TODO: Google can only translate from english and french. Find a translation service for %s to language %s",
            	                WikiUrl($pagename,false,true),
            	                $to_lang));
            } else
                return $this->error(fmt("%s is empty",$pagename));
        }
        
        $pagelist = new PageList('', $exclude, $this->getArgs($argstr, $request));
        $pagelist->_columns[0]->_heading = "$from_lang";
        foreach ($languages as $lang) {
            if ($lang == $from_lang) continue;
            $field = "custom:$lang";
            $column = new _PageList_Column_custom($field,$from_lang,$this);
            $pagelist->_types["custom"] = $column;
            $pagelist->_addColumn($field);
        }
        if (!empty($string)) {
            $pagelist->addPage( $string );
            return $pagelist;
        }
        switch ($what) {
        case 'allpages':
            $pagelist->addPages( $dbi->getAllPages($include_empty, $sortby, $limit) );
            break;
        case 'pages':
            // not all pages, only the pgsrc pages
            if (!is_array($exclude))
                $exclude = $pagelist->explodePageList($exclude,false,$sortby,$limit);
            $path = FindLocalizedFile(WIKI_PGSRC);
            $pgsrc = new fileSet($path);
            foreach ($pgsrc->getFiles($exclude,$sortby,$limit) as $pagename) {
                $pagename = urldecode($pagename);
                if (substr($pagename,-1,1) == '~') continue;
	        if (in_array($pagename, $exclude))
        	    continue;             // exclude page.
                if ($match != '*' and !glob_match($match,$pagename))
                    continue;
                $page_handle = $dbi->getPage($pagename);
                $pagelist->addPage( $page_handle );
            }
            break;
        case 'wikiwords':
            if (!isset($this->_locales[$from_lang])) {
                $this->init_locale($from_lang);
            }
            $locale = & $this->_locales[$from_lang];
            if (is_array($locale)) {
                $count = 0;
                foreach ($locale as $from => $to) {
                    if ($match != '*' and !glob_match($match,$from))
                        continue;
                    if (isWikiWord($from)) {
                        $count++;
                        $pagelist->addPage( $from );
                        if ($limit and $count > $limit) break;
                    }
                }
            }
            break;
        }
        return $pagelist;
    }
};

class _PageList_Column_custom extends _PageList_Column {
    function _PageList_Column_custom($field, $from_lang, $plugin) {
        $this->_field = $field;
        $this->_from_lang = $from_lang;
        $this->_plugin =& $plugin;
        $this->_iscustom = substr($field, 0, 7) == 'custom:';
        if ($this->_iscustom)
            $this->_field = substr($field, 7);
        $heading = $field;
        $this->dbi = &$GLOBALS['request']->getDbh();
        $this->_PageList_Column_base($field);
    }
    
    function _getValue($page, &$revision_handle) {
    	if (is_object($page)) $text = $page->getName();
        else $text = $page;
    	$trans = $this->_plugin->translate($text, $this->_field, $this->_from_lang);
        // how to markup untranslated words and not existing pages?
        // untranslated: (TODO) link to translation editor
        if ($trans == $text or // untranslated
            (($this->_from_lang != 'en') and 
             ($this->_field != 'en') and
             ($trans == $this->_plugin->translate($text, 'en', $this->_from_lang))
             ))
        {    
            global $Theme;
            $link = $Theme->linkUnknownWikiWord($trans);
            if ($this->dbi->isWikiPage($trans)) {
                $url = WikiURL($trans, array('action' => 'TranslateText','lang' => $this->_field));
                $button = $Theme->makeButton('T', $url);
                $button->addTooltip(sprintf(_("Define the translation for %s in %s"), $trans, $this->_field));
                $link = HTML::span($button);
                $link->setAttr('class', 'wikiunknown');
                $text = HTML::span($Theme->maybeSplitWikiWord($trans));
                $text->setAttr('style', 'text-decoration:line-through');
                $link->pushContent($text);
                return $link;
            } elseif (is_object($page))
                return '';
            else                        // not existing: empty
                return '';
        } elseif (is_object($page)) {
            return WikiLink($trans,'auto');
        } else {
            return $trans;
        }
    }
}

// $Log: not supported by cvs2svn $
// Revision 1.5  2004/03/17 15:38:03  rurban
// more translations
//
// Revision 1.4  2004/03/17 13:20:31  rurban
// Placeholder for all yet untranslated pgsrc pagenames. Add german translations of these.
//
// Revision 1.3  2004/03/16 20:22:32  rurban
// added link to TranslateText action
//
// Revision 1.2  2004/03/16 15:47:27  rurban
// added match, fixed reverse translation, added page=, what=allpages, what=wikiwords, fixed what=pages, simplified _PageList_Column_custom
//
// Revision 1.1  2004/03/14 16:45:10  rurban
// Just the page matrix for now.
// doesn't work yet, if the default langauge != en
//
//

// For emacs users
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>