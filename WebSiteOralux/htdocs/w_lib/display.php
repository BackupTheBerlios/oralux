<?php
   // display.php: fetch page or get default content
   // calls transform.php for actual transformation of wiki markup to HTML
   rcs_id('$Id: display.php,v 1.1 2004/09/28 21:48:46 gcasse Exp $');
 
   // if we got GET data, the first item is always a page name
   // if it wasn't this file would not have been included

   if (empty($QUERY_STRING) && isset($argv[0]))
      $QUERY_STRING = $argv[0];
  
   if (isset($QUERY_STRING) && preg_match('/^[-_.+%\w]+$/', $QUERY_STRING)) {
      $pagename = urldecode($QUERY_STRING);
   } else { 
// Gilles Casse (2003-01-21) for the Oralux website.
$pagename = $FrontPage;

//       $pagename = gettext("FrontPage");
//       // if there is no FrontPage, create a basic set of Wiki pages
//       if (! IsWikiPage($dbi, $pagename)) {
//          include "w_lib/setupwiki.php";
//       }
   }

   $html = "";
   $enc_name = rawurlencode($pagename);
   $pagehash = RetrievePage($dbi, $pagename, $WikiPageStore);

// Gilles Casse (2003-01-21) for the Oralux website.
// Sorry, no new page at the moment (translations).
if (!is_array($pagehash)) {
  echo ("Error...");
  return;
}

// transform.php returns $html containing all the HTML markup
include("w_lib/transform.php");

   // we render the page if it exists, else ask the user to write one.
//    if (is_array($pagehash)) {
//       // transform.php returns $html containing all the HTML markup
//       include("w_lib/transform.php");
//    } else {
//       $html .= sprintf(gettext("Describe %s here."),
// 		       "$pagename<a href='$ScriptUrl?edit=$enc_name'>?</a>");
//    }

   GeneratePage('BROWSE', $html, $pagename, $pagehash);
   flush();

   IncreaseHitCount($dbi, $pagename);
?>
