<!-- $Id: loadserial.php,v 1.1 2004/09/28 21:48:44 gcasse Exp $ -->
<?php
   /*
      Load a set of pages that have been serialized with 
      wiki_dumpserial.php.
   */
   if (!defined('WIKI_ADMIN'))
      die("You must be logged in as the administrator to load serialized pages.");

   $directory = $loadserial;
   $html = "Loading serialized pages from '$directory'.<p>\n";

   if (! file_exists($directory)) {
      echo "No such directory '$directory'.<br>\n";
      exit;
   }
   
   $handle = opendir($directory);

   while ($file = readdir($handle)) {

      if ($file[0] == ".")
         continue;

      $html .= "Reading '$file'...<br>\n";

      $data = implode("", file("$directory/$file"));
      $pagehash = unserialize($data);

      // at this point there needs to be some form of verification
      // that we are about to insert a page.

      $pagename = rawurldecode($file);
      $html .= "inserting file '".htmlspecialchars($pagename)."' into the database...<br>\n";
      InsertPage($dbi, $pagename, $pagehash);
   }
   closedir($handle); 

   $html .= "<p><b>Load complete.</b>";
   GeneratePage('MESSAGE', $html, 'Load serialized pages', 0);
   ExitWiki('');
?>
