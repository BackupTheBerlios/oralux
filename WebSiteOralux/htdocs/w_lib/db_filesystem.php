<?php  rcs_id('$Id: db_filesystem.php,v 1.1 2004/09/28 21:48:45 gcasse Exp $');
   /*
      Database functions:

      OpenDataBase($table)
      CloseDataBase($dbi)
      RetrievePage($dbi, $pagename, $pagestore)
      InsertPage($dbi, $pagename, $pagehash)
      SaveCopyToArchive($dbi, $pagename, $pagehash) 
      IsWikiPage($dbi, $pagename)
      InitTitleSearch($dbi, $search)
      TitleSearchNextMatch($dbi, $res)
      InitFullSearch($dbi, $search)
      FullSearchNextMatch($dbi, $res)
      MakeBackLinkSearchRegexp($pagename)
      InitBackLinkSearch($dbi, $pagename) 
      BackLinkSearchNextMatch($dbi, &$pos) 
      IncreaseHitCount($dbi, $pagename)
      GetHitCount($dbi, $pagename)
      InitMostPopular($dbi, $limit)
      MostPopularNextMatch($dbi, $res)
   */


   // open a database and return the handle
   // loop until we get a handle; php has its own
   // locking mechanism, thank god.
   // Suppress ugly error message with @.

   function OpenDataBase($dbname) {
      global $WikiDB;

      ksort($WikiDB);
      reset($WikiDB);

      return $WikiDB;
   }


   function CloseDataBase($dbi) {
      return;
   }

   // Sort of urlencode() the pagename.
   // We only encode a limited set of characters to minimize breakage
   // of existing databases.  The encoded names can be decoded with
   // urldecode.
   function EncodePagename($pagename) {
      if ($pagename == '.')
        return '%2e';
      else if ($pagename == '..')
        return '%2e.';
      
      $bad_chars = '%/\\:'; // '%' must be first!
      for ($i = 0; $i < strlen($bad_chars); $i++) {
	 $pagename = str_replace($bad_chars[$i],
	                         rawurlencode($bad_chars[$i]), $pagename);
      }
        
      return $pagename;
   }

   // Return hash of page + attributes or default
   function RetrievePage($dbi, $pagename, $pagestore) {
      $filename = $dbi[$pagestore] . "/" . EncodePagename($pagename);
      if ($fd = @fopen($filename, "rb")) {
         $locked = flock($fd, 1); # Read lock
         if (!$locked) { 
            ExitWiki("Timeout while obtaining lock. Please try again"); 
         }
         if ($data = fread($fd, filesize($filename))) {
            // unserialize $data into a hash
            $pagehash = unserialize($data);
            $pagehash['pagename'] = $pagename;
	    if (!is_array($pagehash))
		ExitWiki(sprintf(gettext("'%s': corrupt file"),
				 htmlspecialchars($filename)));
	 }	
	 fclose($fd);
	 if ($data) {
	    return $pagehash;
	 }
      } else {
         return -1;
      }
   }


   // Either insert or replace a key/value (a page)
   function Filesystem_WritePage($dbi, $pagename, $pagehash) {
      global $WikiPageStore;
      $pagedata = serialize($pagehash);

      if (!file_exists($dbi)) {
	     $d = split("/", $dbi);
		 $dt = "";
		 while(list($key, $val) = each($d)) {
			$dt = $dt.$val."/";
		    @mkdir($dt, 0755);
		 }
	  }

      $filename = $dbi . "/" . EncodePagename($pagename);
      if($fd = fopen($filename, 'a+b')) { 
         $locked = flock($fd,2); #Exclusive blocking lock 
         if (!$locked) { 
            ExitWiki("Timeout while obtaining lock. Please try again"); 
         }
	 

         #Second (actually used) filehandle 
         #$fdsafe = fopen($filename, 'wb'); 
         #fwrite($fdsafe, $pagedata); 
         #fclose($fdsafe);

	 rewind($fd);
	 ftruncate($fd, 0);
         fwrite($fd, $pagedata); 
         fclose($fd);
      } else {
         ExitWiki("Error while writing page '$pagename'");
      }
   }

   function InsertPage($dbi, $pagename, $pagehash) {
      return Filesystem_WritePage($dbi['wiki'], $pagename, $pagehash);
   }

   // for archiving pages to a seperate dbm
   function SaveCopyToArchive($dbi, $pagename, $pagehash) {
      global $ArchivePageStore;
      return Filesystem_WritePage($dbi[$ArchivePageStore], $pagename, $pagehash);
   }


   function IsWikiPage($dbi, $pagename) {
      return file_exists($dbi['wiki'] . "/" . EncodePagename($pagename));
   }


   function IsInArchive($dbi, $pagename) {
      return file_exists($dbi['archive'] . "/" . EncodePagename($pagename));
   }


   // setup for title-search
   function InitTitleSearch($dbi, $search) { 
      $pos['search'] = '=' . preg_quote($search) . '=i';
      $pos['data'] = GetAllWikiPageNames($dbi['wiki']);

      return $pos;
   }

   // iterating through database
   function TitleSearchNextMatch($dbi, &$pos) { 
      while (list($key, $page) = each($pos['data'])) {
         if (preg_match($pos['search'], $page)) {
            return $page;
         }
      }
      return 0;
   }

   // setup for full-text search
   function InitFullSearch($dbi, $search) { 
      return InitTitleSearch($dbi, $search);
   }

   //iterating through database
   function FullSearchNextMatch($dbi, &$pos) { 
      global $WikiPageStore;
      while (list($key, $page) = each($pos['data'])) {
         $pagedata = RetrievePage($dbi, $page, $WikiPageStore);
         if (preg_match($pos['search'], serialize($pagedata))) {
	        return $pagedata;
         }
      }
      return 0;
   }

   ////////////////////////
   // new database features

   // Compute PCRE suitable for searching for links to the given page.
   function MakeBackLinkSearchRegexp($pagename) {
      global $WikiNameRegexp;

      // Note that in (at least some) PHP 3.x's, preg_quote only takes
      // (at most) one argument.  Also it doesn't quote '/'s.
      // It does quote '='s, so we'll use that for the delimeter.
      $quoted_pagename = preg_quote($pagename);
      if (preg_match("/^$WikiNameRegexp\$/", $pagename)) {
	 # FIXME: This may need modification for non-standard (non-english) $WikiNameRegexp.
	 return "=(?<![A-Za-z0-9!])$quoted_pagename(?![A-Za-z0-9])=";
      }
      else {
	 // Note from author: Sorry. :-/
	 return ( '='
		  . '(?<!\[)\[(?!\[)' // Single, isolated '['
		  . '([^]|]*\|)?'     // Optional stuff followed by '|'
	          . '\s*'             // Optional space
		  . $quoted_pagename  // Pagename
		  . '\s*\]=' );	      // Optional space, followed by ']'
	 // FIXME: the above regexp is still not quite right.
	 // Consider the text: " [ [ test page ]".  This is a link to a page
	 // named '[ test page'.  The above regexp will recognize this
	 // as a link either to '[ test page' (good) or to 'test page' (wrong).
      } 
   }

   // setup for back-link search
   function InitBackLinkSearch($dbi, $pagename) {
      $pos['search'] = MakeBackLinkSearchRegexp($pagename);
      $pos['data'] = GetAllWikiPageNames($dbi['wiki']);

      return $pos;
   }

   // iterating through back-links
   function BackLinkSearchNextMatch($dbi, &$pos) {
      global $WikiPageStore;
      while (list($key, $page) = each($pos['data'])) {
         $pagedata = RetrievePage($dbi, $page, $WikiPageStore);
	 if (!is_array($pagedata)) {
	    printf(gettext("%s: bad data<br>\n"), htmlspecialchars($page));
	    continue;
	 }
	 
	 while (list($i, $line) = each($pagedata['content'])) {
	    if (preg_match($pos['search'], $line))
	       return $page;
	 }
      }
      return 0;
   }

   function IncreaseHitCount($dbi, $pagename) {
      return;
return;
      // kluge: we ignore the $dbi for hit counting
      global $WikiDB;

      $hcdb = OpenDataBase($WikiDB['hitcount']);

      if (dbmexists($hcdb['active'], $pagename)) {
         // increase the hit count
         $count = dbmfetch($hcdb['active'], $pagename);
         $count++;
         dbmreplace($hcdb['active'], $pagename, $count);
      } else {
         // add it, set the hit count to one
         $count = 1;
         dbminsert($hcdb['active'], $pagename, $count);
      }

      CloseDataBase($hcdb);
   }

   function GetHitCount($dbi, $pagename) {
      return;
      // kluge: we ignore the $dbi for hit counting
      global $WikiDB;

      $hcdb = OpenDataBase($WikiDB['hitcount']);
      if (dbmexists($hcdb['active'], $pagename)) {
         // increase the hit count
         $count = dbmfetch($hcdb['active'], $pagename);
         return $count;
      } else {
         return 0;
      }

      CloseDataBase($hcdb);
   }


   function InitMostPopular($dbi, $limit) {
     return;
      $pagename = dbmfirstkey($dbi['hitcount']);
      $res[$pagename] = dbmfetch($dbi['hitcount'], $pagename);
      while ($pagename = dbmnextkey($dbi['hitcount'], $pagename)) {
         $res[$pagename] = dbmfetch($dbi['hitcount'], $pagename);
         echo "got $pagename with value " . $res[$pagename] . "<br>\n";
      }

      rsort($res);
      reset($res);
      return($res);
   }

   function MostPopularNextMatch($dbi, $res) {
      return;
      // the return result is a two element array with 'hits'
      // and 'pagename' as the keys

      if (list($index1, $index2, $pagename, $hits) = each($res)) {
         echo "most popular next match called<br>\n";
         echo "got $pagename, $hits back<br>\n";
         $nextpage = array(
            "hits" => $hits,
            "pagename" => $pagename
         );
         return $nextpage;
      } else {
         return 0;
      }
   } 

   function GetAllWikiPagenames($dbi) {
      $namelist = array();
      $d = opendir($dbi);
      while($entry = readdir($d)) {
	 if ($entry == '.' || $entry == '..')
	    continue;
	 $pagename = rawurldecode($entry);
	 if ($entry != EncodePagename($pagename)) {
	    printf(gettext("%s: Bad filename in database<br>\n"),
		   htmlspecialchars("$dbi/$entry"));
	    continue;
	 }
	 $namelist[] = $pagename;
      }

      return $namelist;
   }
?>
