<?php // $Id: w_admin.php,v 1.1 2004/09/28 21:48:40 gcasse Exp $

   function rcs_id($id) {}   // otherwise this gets in the way

   define('WIKI_ADMIN', true);	// has to be before includes

// set these to your preferences. For heaven's sake
// pick a good password!

   $wikiadmin   = $WIKILOGIN;
   $adminpasswd = $WIKIADMINPASSWORD;

   // Do not tolerate sloppy systems administration
   if (empty($wikiadmin) || empty($adminpasswd)) {
      echo "Set the administrator account and password first.\n";
      exit;
   }

   // from the manual, Chapter 16
   if (($_SERVER["PHP_AUTH_USER"] != $wikiadmin  )  ||
       ($_SERVER["PHP_AUTH_PW"]   != $adminpasswd)) {
     //     header("HTTP/1.0 401 Unauthorized");
     // For TuxFamily
     header("status: 401 Unauthorized");

     header("WWW-Authenticate: Basic realm=\"PhpWiki\"");
      echo "Invalid login or password.";
      exit;
   }

// Gilles Casse (2003-01-23) for the Oralux website
// After the Basic Authentification:
require_once("include/webpage.php");
   include("w_lib/config.php");
   include("w_lib/stdlib.php");



   // All requests require the database
   $dbi = OpenDataBase($WikiPageStore);

   if (isset($lock) || isset($unlock)) {
      include ('w_admin/lockpage.php');
   } elseif (isset($zip)) {
      include ('w_lib/ziplib.php');
      include ('w_admin/zip.php');
      ExitWiki('');
   } elseif (isset($dumpserial)) {
      include ('w_admin/dumpserial.php');
   } elseif (isset($loadserial)) {
      include ('w_admin/loadserial.php');
   } elseif (isset($remove)) {
      if (get_magic_quotes_gpc())
         $remove = stripslashes($remove);
      if (function_exists('RemovePage')) {
         $html .= sprintf(gettext ("You are about to remove '%s' permanently!"), htmlspecialchars($remove));
	 $html .= "\n<P>";
	 $url = rawurlencode($remove);
	 $html .= sprintf(gettext ("Click %shere%s to remove the page now."),
		  "<A HREF=\"$ScriptUrl?removeok=$url\">", "</A>");
	 $html .= "\n<P>";
	 $html .= gettext ("Otherwise press the \"Back\" button of your browser.");
      } else {
         $html = gettext ("Function not yet implemented.");
      }
      GeneratePage('MESSAGE', $html, gettext ("Remove page"), 0);
      ExitWiki('');
   } elseif (isset($removeok)) {
      if (get_magic_quotes_gpc())
	 $removeok = stripslashes($removeok);
      RemovePage($dbi, $removeok);
      $html = sprintf(gettext ("Removed page '%s' succesfully."),
		      htmlspecialchars($removeok));
      GeneratePage('MESSAGE', $html, gettext ("Remove page"), 0);
      ExitWiki('');
   }

   include('wiki.php');
?>
