<!-- $Id: lockpage.php,v 1.1 2004/09/28 21:48:44 gcasse Exp $ -->
<?php
   if(isset($lock)) $page = $lock;
   elseif(isset($unlock)) $page = $unlock;

   $argv[0] = $page;  // necessary for displaying the page afterwards
   $pagename = rawurldecode($page);

   $pagehash = RetrievePage($dbi, $pagename, $WikiPageStore);
   if (! is_array($pagehash))
      ExitWiki("Unknown page '".htmlspecialchars($pagename)."'\n");

   if (isset($lock)) {
      $pagehash['flags'] |= FLAG_PAGE_LOCKED;
      InsertPage($dbi, $pagename, $pagehash);
      // echo htmlspecialchars($page) . " locked\n";
   } elseif(isset($unlock)) {
      $pagehash['flags'] &= ~FLAG_PAGE_LOCKED;
      InsertPage($dbi, $pagename, $pagehash);
      // echo htmlspecialchars($page) . " unlocked\n";
   }
?>
