<?php
   // Title search: returns pages having a name matching the search term
   rcs_id('$Id: search.php,v 1.1 2004/09/28 21:48:48 gcasse Exp $');

   if(get_magic_quotes_gpc())
      $search = stripslashes($search);

   $html = "<P><B>"
	   . sprintf(gettext ("Searching for \"%s\" ....."),
		     htmlspecialchars($search))
	   . "</B></P>\n";

   // quote regexp chars (backends should do this...) 
   //$search = preg_quote($search);

   // search matching pages
   $found = 0;
   if (strlen($search)) {           
      $query = InitTitleSearch($dbi, $search);
      while ($page = TitleSearchNextMatch($dbi, $query)) {
         $found++;
         $html .= LinkExistingWikiWord($page) . "<br>\n";
      }
   }
   else {
      $html .= gettext("(You entered an empty search string)") . "<br>\n";
   }   
     
   $html .= "<hr noshade>\n"
	    . sprintf(gettext ("%d pages match your query."), $found)
	    . "\n";

   GeneratePage('MESSAGE', $html, gettext ("Title Search Results"), 0);
?>
