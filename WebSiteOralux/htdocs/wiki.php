<?php
// Gilles Casse (2003-01-21) for the Oralux website.
//<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
//<!-- $Id: wiki.php,v 1.1 2004/09/28 21:48:40 gcasse Exp $ -->

// Gilles Casse (2003-01-21) for the Oralux website
require_once("include/webpage.php");
ini_set ( "output_buffering", "1");
ob_start();

/*
      The main page, i.e. the main loop.
      This file is always called first.
*/

if (!defined('WIKI_ADMIN')) { // w_index.php not included by w_admin.php?
  include "w_lib/config.php";
  include "w_lib/stdlib.php";

  // All requests require the database
  $dbi = OpenDataBase($WikiPageStore);
}

// Gilles Casse (2003-01-21) for the Oralux website.
// Limited features at the moment (for translations).

// Allow choice of submit buttons to determine type of search:
// if (isset($searchtype) && ($searchtype == 'full'))
//      $full = $searchstring;
//      elseif (isset($searchstring))     // default to title search
//      $search = $searchstring;

if (isset($edit)) {
  include "w_lib/editpage.php";
} 
// elseif (isset($links)) {
//        include "w_lib/editlinks.php";
//      } 
// elseif (isset($copy)) {
//        include "w_lib/editpage.php";
//      } 
// elseif (isset($search)) {
//        include "w_lib/search.php";
//      } 
// elseif (isset($full)) {
//        include "w_lib/fullsearch.php";
//      } 
// elseif (isset($refs)) {
//        if (function_exists('InitBackLinkSearch')) {
// 	 include "w_lib/backlinks.php";
//        }
//        else {
// 	 $full = $refs;
// 	 include "w_lib/fullsearch.php";
//        }
//      } 

elseif (isset($post)) {
  include "w_lib/savepage.php";
} 
// elseif (isset($info)) {
//        include "w_lib/pageinfo.php";
//      } 
elseif (isset($diff)) {
  include "w_lib/diff.php";
} 
else {
  include "w_lib/display.php"; // defaults to FrontPage
}

CloseDataBase($dbi);

// Gilles Casse (2003-01-21) for the Oralux website.
// Building the sub menu
unset($aPage);

if (isset($edit))
{
  $aPage=$edit;
}
else if (isset($post))
{
  $aPage=$post;
}
else if (isset($lock))
{
  $aPage=$lock;
}
else if (isset($unlock))
{
  $aPage=$unlock;
}
else if (isset($diff))
{
  $aPage=$_REQUEST["diff"];
}
else if (isset($RecentChanges))
{
  $aPage="RecentChanges";
}
else if ($_SERVER["QUERY_STRING"] != NULL)
{
  // Remove the PHPSESSID
  $anArray=explode("&",$_SERVER["QUERY_STRING"]);
  $aPage=$anArray[0];
}

//echo "<hr>page=$aPage";

$aStyle[0]="wiki.css";

if (!isset($aPage))
{
  ob_end_clean();
  $aWebPage=new webPage("Translation", "$Date: 2004/09/28 21:48:40 $", $aStyle);
  $aWebPage->printTag("htm/translation.htm");
  exit;
}

$aLang=getLang();
$a=$Translate["Translation"][$aLang];
$a_buffer = "<div>\n<a href=\"translation.php\">$a</a> &gt; ";

if ($aPage=="RecentChanges")
{
  $a_buffer .= $Translate["RecentChanges"][$aLang];
}
else
{
  // Creating the sub-menu
  unset($aIndex);
  foreach ($Menu as $i => $aMenu)
    {
      foreach( $aMenu as $j => $queryString)
	{
	  if ($queryString == $aPage)
	    {
	      $aIndex=$i;
	      break;
	    }
	}
      if (isset($aIndex))
	{
	  break;
	}
    }
  $aSeparator="";
  if (!isset($aIndex))
    {
      $a_buffer.=$aPage;
    }
  else
    {
      $aMenu=$Menu[$aIndex];
      
      foreach($aMenu as $i => $queryString)
	{
	  if ($queryString == $aPage)
	    {
	      $a_buffer .= "$aSeparator $i";
	    }
	  else
	    {
	      $a_buffer .= "$aSeparator <a href=\"$ScriptUrl?$queryString\">$i</a>";
	    }
	  $aSeparator="|";
	}
    }
}
$a_buffer .= "</div>\n";
$a_buffer.=ob_get_contents();
ob_end_clean();

//phpInfo();

$aWebPage=new webPage("Translation", "$Date: 2004/09/28 21:48:40 $", $aStyle);
$aWebPage->printBuffer( $a_buffer);

?>
