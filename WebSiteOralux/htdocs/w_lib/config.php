<?php

// Gilles Casse (2003-01-21) for the Oralux website
// 
// To be compliant with php.ini file with register_globals=off
{
  foreach ($_SERVER as $var=>$value)
    {
      $$var=preg_replace('/&PHPSESSID=.*/', '', $value);
    }
  foreach ($_REQUEST as $var=>$value)
    {
      $$var=preg_replace('/&PHPSESSID=.*/', '', $value);
    }
   foreach ($_FILES as $var=>$value)
     {
       $$var=preg_replace('/&PHPSESSID=.*/', '', $value);
     }
}

   // essential internal stuff -- skip it. Go down to Part One. There
   // are four parts to this file that interest you, all labeled Part
   // One, Two, Three and Four.

   set_magic_quotes_runtime(0);
   error_reporting(E_ALL ^ E_NOTICE);

   if (!function_exists('rcs_id')) {
      function rcs_id($id) { 
      // Gilles Casse (2003-01-21) for the Oralux website
      // 
	//echo "<!-- $id -->\n"; 
};

   }
   rcs_id('$Id: config.php,v 1.1 2004/09/28 21:48:45 gcasse Exp $'); 
   // end essential internal stuff


   /////////////////////////////////////////////////////////////////////
   // Part One:
   // Constants and settings. Edit the values below for your site.
   /////////////////////////////////////////////////////////////////////

// Gilles Casse (2003-01-21) for the Oralux website.
$FrontPage="German1";
$Menu=array("German" => array(1 => "German1",
			      2 => "German2"),
	    "Spanish" => array(1 => "Spanish1",
			       2 => "Spanish2"));

$Translate=array("Translation" => array("en"=>"Translation",
					"fr"=>"Traduction"),
		 "RecentChanges" => array("en"=>"Recent changes",
					  "fr"=>"Changements r�cents"),
		 "Diff" => array("en"=>"Comparison to the archived version of ",
				 "fr"=>"Comparaison avec la version archiv�e de "),
		 "German" => array("en"=>"German Translations",
				   "fr"=>"Traductions en allemand"),
		 "Spanish" => array("en"=>"Spanish Translations",
				    "fr"=>"Traductions en espagnol"));

function GetFullName($theName)
{
  global $Translate;
  global $Menu;
  // Translate the filename in the relevant title
  // e.g. "German1" gets "Allemand (1)" 
  // "Diff German2" gets "Comparaison avec la version archiv�e de Allemand (2)"
  $aName=$theName;
  $aLang=getLang();
  $aFullName="";
  $anArray=explode (" ", $theName);
  if (count($anArray)==2)
    {
      $aFullName=$Translate["Diff"][$aLang];
      $aName=$anArray[1];
    }
  else if ($aName=="RecentChanges")
    {
      return $Translate[$aName][$aLang];
    }

  // Check that it is an expected page
  foreach ($Menu as $i=>$aMenu)
    {
      foreach ($aMenu as $j=>$aPage)
	{
	  if ($aName==$aPage)
	    {
	      break;
	    }
	}
      if ($aName==$aPage)
	{
	  break;
	}
    }

  if ($aName != $aPage)
    { // Unknown page
      return $aName;
    }

  // no more than one digit 
  $aLength=strlen($aName);
  $aLabel=substr($aName, 0, $aLength-1);
  $aDigit=$aName[$aLength-1];

  return $aFullName.$Translate[$aLabel][$aLang]." ($aDigit)";
}

function getTemplate($theName)
{
  // Converts for example "w_templates/message.html" in "w_templates/message-fr.html"
  $aTemplate=$theName;
  $aLang=getLang();
  if ($aLang!="en")
    {
      $aArray=explode(".",$theName);
      $aTemplate=$aArray[0]."-$aLang.".$aArray[1];
    }
  return $aTemplate;
}

   // URL of w_index.php e.g. http://yoursite.com/phpwiki/w_index.php
   // you can leave this empty - it will be calculated automatically
   $ScriptUrl = "http://oralux.org/wiki.php";

   // URL of w_admin.php e.g. http://yoursite.com/phpwiki/w_admin.php
   // you can leave this empty - it will be calculated automatically
   // if you fill in $ScriptUrl you *MUST* fill in $AdminUrl as well!
   $AdminUrl = "http://oralux.org/w_admin.php";

   // Select your language - default language "C": English
   // other languages available: Dutch "nl", Spanish "es", German "de",
   // and Swedish "sv"
   $LANG="C";

   /////////////////////////////////////////////////////////////////////
   // Part Two:
   // Database section
   // set your database here and edit the according section below.
   // For PHP 4.0.4 and later you must use "dba" if you are using 
   // DBM files for storage. "dbm" uses the older deprecated interface.
   // The option 'default' will choose either dbm or dba, depending on
   // the version of PHP you are running.
   /////////////////////////////////////////////////////////////////////

   $WhichDatabase = 'mysql';
//'default'; // use one of "dbm", "dba", "mysql",
                           // "pgsql", "msql", "mssql", or "file"

   // DBM and DBA settings (default)
   if ($WhichDatabase == 'dbm' or $WhichDatabase == 'dba' or
       $WhichDatabase == 'default') {
      $DBMdir = "/tmp";
      $WikiPageStore = "wiki";
      $ArchivePageStore = "archive";
      $WikiDB['wiki']      = "$DBMdir/wikipagesdb";
      $WikiDB['archive']   = "$DBMdir/wikiarchivedb";
      $WikiDB['wikilinks'] = "$DBMdir/wikilinksdb";
      $WikiDB['hottopics'] = "$DBMdir/wikihottopicsdb";
      $WikiDB['hitcount']  = "$DBMdir/wikihitcountdb";

      // this is the type of DBM file on your system. For most Linuxen
      // 'gdbm' is fine; 'db2' is another common type. 'ndbm' appears
      // on Solaris but won't work because it won't store pages larger
      // than 1000 bytes.
      define("DBM_FILE_TYPE", 'gdbm');

      // try this many times if the dbm is unavailable
      define("MAX_DBM_ATTEMPTS", 20);

      // for PHP3 use dbmlib, else use dbalib for PHP4
      if ($WhichDatabase == 'default') {
         if ( floor(phpversion()) == 3) {
            $WhichDatabase = 'dbm';
         } else {
            $WhichDatabase = 'dba';
         }
      }

      if ($WhichDatabase == 'dbm') {
          include "w_lib/dbmlib.php"; 
      } else {
          include "w_lib/dbalib.php";
      }

   // MySQL settings -- see INSTALL.mysql for details on using MySQL
   } elseif ($WhichDatabase == 'mysql') {
      // MySQL server host:
      $mysql_server = $WIKISERVER;

      // username as used in step 2 of INSTALL.mysql:
      // GC: $WIKILOGIN comes from the prepend file
      $mysql_user = $WIKILOGIN;

      // password of above user (or leave blank if none):
      // GC: $WIKILOGIN comes from the prepend file
      $mysql_pwd = $WIKIBASEPASSWORD;

      // name of the mysql database
      //  (this used to default to 'wiki' prior to phpwiki-1.2.2)
      $mysql_db = 'oralux';

      // Names of the tables.
      // You probably don't need to change these.  If you do change
      // them you will also have to make corresponding changes in
      // w_schemas/schema.mysql before you initialize the database.
      $WikiPageStore = "wiki";
      $ArchivePageStore = "archive";
      $WikiLinksStore = "wikilinks";
      $WikiScoreStore = "wikiscore";
      $HitCountStore = "hitcount";

      include "w_lib/mysql.php";

   // PostgreSQL settings -- see INSTALL.pgsql for more details
   } elseif ($WhichDatabase == 'pgsql') {
      $pg_dbhost    = "localhost";
      $pg_dbport    = "5432";
      $WikiDataBase  = "wiki"; // name of the database in Postgresql
      $WikiPageStore = "wiki";
      $ArchivePageStore = "archive";
      $WikiLinksPageStore = "wikilinks";
      $HotTopicsPageStore = "hottopics";
      $HitCountPageStore = "hitcount";
      include "w_lib/pgsql.php";

   // MiniSQL (mSQL) settings -- see INSTALL.msql for details on using mSQL
   } elseif ($WhichDatabase == 'msql') {
      $msql_db = "wiki";
      $WikiPageStore = array();
      $ArchivePageStore = array();
      $WikiPageStore['table']         = "wiki";
      $WikiPageStore['page_table']    = "wikipages";
      $ArchivePageStore['table']      = "archive";
      $ArchivePageStore['page_table'] = "archivepages";
      // should be the same as wikipages.line
      define("MSQL_MAX_LINE_LENGTH", 128);
      include "w_lib/msql.php";

   // Filesystem DB settings
   } elseif ($WhichDatabase == 'file') {
      $DBdir = "/tmp/wiki";
      $WikiPageStore = "wiki";
      $ArchivePageStore = "archive";
      $WikiDB['wiki']      = "$DBdir/pages";
      $WikiDB['archive']   = "$DBdir/archive";
      $WikiDB['wikilinks'] = "$DBdir/links";
      $WikiDB['hottopics'] = "$DBdir/hottopics";
      $WikiDB['hitcount']  = "$DBdir/hitcount";
      include "w_lib/db_filesystem.php";

   // MS SQLServer settings
   } elseif ($WhichDatabase == 'mssql') {
      $WikiPageStore = "wiki";
      $ArchivePageStore = "archive";
      $WikiLinksStore = "wikilinks";
      $WikiScoreStore = "wikiscore";
      $HitCountStore = "hitcount";
      $mssql_server = 'servername';
      $mssql_user = '';
      $mssql_pwd = '';
      $mssql_db = '';
      include "w_lib/mssql.php";

   } else die("Invalid '\$WhichDatabase' in w_lib/config.php"); 


   /////////////////////////////////////////////////////////////////////
   // Part Three:
   // Miscellaneous
   /////////////////////////////////////////////////////////////////////

   // logo image (path relative to w_index.php)
   $logo = "";

   // Signature image which is shown after saving an edited page
   // If this is left blank (or unset), the signature will be omitted.
   $SignatureImg = "";

   // date & time formats used to display modification times, etc.
   // formats are given as format strings to PHP date() function
   $datetimeformat = "Y m d";	// may contain time of day
   $dateformat = "Y m d";	// must not contain time

   // this defines how many page names to list when displaying
   // the MostPopular pages; the default is to show the 20 most popular pages
   define("MOST_POPULAR_LIST_LENGTH", 20);

   // this defines how many page names to list when displaying related pages
   define("NUM_RELATED_PAGES", 5);

   // number of user-defined external references, i.e. "[1]"
   define("NUM_LINKS", 12);

   // allowed protocols for links - be careful not to allow "javascript:"
   // within a named link [name|uri] one more protocol is defined: phpwiki
   $AllowedProtocols = "http|https|mailto|ftp|news|gopher";

   // URLs ending with the following extension should be inlined as images
   $InlineImages = "png|jpg|gif";

   // Perl regexp for WikiNames
   // (?<!..) & (?!...) used instead of '\b' because \b matches '_' as well
   $WikiNameRegexp = "(?<![A-Za-z0-9])([A-Z][a-z]+){2,}(?![A-Za-z0-9])";



   /////////////////////////////////////////////////////////////////////
   // Part Four:
   // Original pages and layout
   /////////////////////////////////////////////////////////////////////

// Gilles Casse (2003-01-21) for the Oralux website
    if (!function_exists ('gettext')) {
      function gettext ($text) { 
         return $text;
      }
    }
   // need to define localization function first -- skip this
//    if (!function_exists ('gettext')) {
//       $lcfile = "locale/$LANG/LC_MESSAGES/phpwiki.php";
//       if (file_exists($lcfile)) { include($lcfile); }
//       else { $locale = array(); }

//       function gettext ($text) { 
//          global $locale;
//          if (!empty ($locale[$text]))
//            return $locale[$text];
//          return $text;
//       }
//    } else {
//       // This putenv() fails when safe_mode is on.
//       // I think it is unnecessary. 
//       //putenv ("LANG=$LANG");
//       bindtextdomain ("phpwiki", "./locale");
//       textdomain ("phpwiki");
//       if (!defined("LC_ALL")) {
//          // Backwards compatibility (for PHP < 4.0.5)
//          define("LC_ALL", "LC_ALL");
//       }   
//       setlocale(LC_ALL, "$LANG");
//    }
   // end of localization function

   // Template files (filenames are relative to script position)
   $templates = array(
   	"BROWSE" =>    getTemplate("w_templates/browse.html"),
	"EDITPAGE" =>  getTemplate("w_templates/editpage.html"),
	"EDITLINKS" => getTemplate("w_templates/editlinks.html"),
	"MESSAGE" =>   getTemplate("w_templates/message.html"),
	"MESSAGE2" =>  getTemplate("w_templates/message2.html")
	);

   /* WIKI_PGSRC -- specifies the source for the initial page contents
    * of the Wiki.  The setting of WIKI_PGSRC only has effect when
    * the wiki is accessed for the first time (or after clearing the
    * database.) WIKI_PGSRC can either name a directory or a zip file.
    * In either case WIKI_PGSRC is scanned for files --- one file per page.
    *
    * If the files appear to be MIME formatted messages, they are
    * scanned for application/x-phpwiki content-types.  Any suitable
    * content is added to the wiki.
    * The files can also be plain text files, in which case the page name
    * is taken from the file name.
    */

   define('WIKI_PGSRC', gettext("./pgsrc")); // Default (old) behavior.
   //define('WIKI_PGSRC', './wiki.zip'); // New style.

   // DEFAULT_WIKI_PGSRC is only used when the language is *not*
   // the default (English) and when reading from a directory:
   // in that case some English pages are inserted into the wiki as well
   // DEFAULT_WIKI_PGSRC defines where the English pages reside 
   define('DEFAULT_WIKI_PGSRC', "./pgsrc");



   //////////////////////////////////////////////////////////////////////
   // you shouldn't have to edit anyting below this line
   function compute_default_scripturl() {
      global $SERVER_PORT, $SERVER_NAME, $SCRIPT_NAME, $HTTPS;
      if (!empty($HTTPS) && $HTTPS != 'off') {
         $proto = 'https';
         $dflt_port = 443;
      }
      else {
         $proto = 'http';
         $dflt_port = 80;
      }
      $port = ($SERVER_PORT == $dflt_port) ? '' : ":$SERVER_PORT";
      return $proto . '://' . $SERVER_NAME . $port . $SCRIPT_NAME;
   }

   if (empty($ScriptUrl)) {
      $ScriptUrl = compute_default_scripturl();
   }
   if (defined('WIKI_ADMIN') && !empty($AdminUrl))
      $ScriptUrl = $AdminUrl;

   $FieldSeparator = "\263";

   if (isset($PHP_AUTH_USER)) {
        $remoteuser = $PHP_AUTH_USER;
   } else {

      // Apache won't show REMOTE_HOST unless the admin configured it
      // properly. We'll be nice and see if it's there.

      getenv('REMOTE_HOST') ? ($remoteuser = getenv('REMOTE_HOST'))
                            : ($remoteuser = getenv('REMOTE_ADDR'));
   }

   // constants used for HTML output. HTML tags may allow nesting
   // other tags always start at level 0
   define("ZERO_LEVEL", 0);
   define("NESTED_LEVEL", 1);

   // constants for flags in $pagehash
   define("FLAG_PAGE_LOCKED", 1);
?>
