<?php rcs_id('$Id: savepage.php,v 1.2 2004/12/30 00:14:16 gcasse Exp $');

/*
   All page saving events take place here.
   All page info is also taken care of here.
   This is klugey. But it works. There's probably a slicker way of
   coding it.
*/

function UpdateRecentChanges($dbi, $pagename, $isnewpage)
{
  global $remoteuser; // this is set in the config
  global $dateformat;
  global $WikiPageStore;

  $recentchanges = RetrievePage($dbi, gettext ("RecentChanges"), $WikiPageStore);

  // this shouldn't be necessary, since PhpWiki loads 
  // default pages if this is a new baby Wiki
  if ($recentchanges == -1) {
    $recentchanges = array(); 
  }

  $now = time();
  $today = date($dateformat, $now);

  if (date($dateformat, $recentchanges['lastmodified']) != $today) {
    $isNewDay = TRUE;
    $recentchanges['lastmodified'] = $now;
  } else {
    $isNewDay = FALSE;
  }

  $numlines = sizeof($recentchanges['content']);
  $newpage = array();
  $k = 0;

  // scroll through the page to the first date and break
  // dates are marked with "____" at the beginning of the line
  for ($i = 0; $i < $numlines; $i++) {
    if (preg_match("/^____/",
		   $recentchanges['content'][$i])) {
      break;
    } else {
      $newpage[$k++] = $recentchanges['content'][$i];
    }
  }

  // if it's a new date, insert it
  $newpage[$k++] = $isNewDay ? "____$today"
    : $recentchanges['content'][$i++];

  // add the updated page's name to the array
  if($isnewpage) {
    $newpage[$k++] = "\n \n* [$pagename] (new) ..... $remoteuser";
  } else {
    $diffurl = "phpwiki:?diff=" . rawurlencode($pagename);
    $newpage[$k++] = "\n \n* [$pagename] ([diff|$diffurl]) ..... $remoteuser";
  }
  if ($isNewDay)
    $newpage[$k++] = "\r";

  // copy the rest of the page into the new array
  // and skip previous entry for $pagename
  $pagename = preg_quote($pagename);
  for (; $i < $numlines; $i++) {
    if (!preg_match("|\[$pagename\]|", $recentchanges['content'][$i])) {
      $newpage[$k++] = $recentchanges['content'][$i];
    }
  }

  // copy the new page back into recentchanges, skipping empty days
  $numlines = sizeof($newpage);
  $recentchanges['content'] = array();
  $k = 0;
  for ($i = 0; $i < $numlines; $i++) {
    if ($i != $numlines-1 &&
	preg_match("/^____/", $newpage[$i]) &&
	preg_match("/^[\r\n]*$/", $newpage[$i+1])) {
      $i++;
    } else {
      $recentchanges['content'][$k++] = $newpage[$i];
    }
  }

  InsertPage($dbi, gettext ("RecentChanges"), $recentchanges);
}


function ConcurrentUpdates($pagename)
{
  if ("en" == getLang())
    {
      //     exit;
      /* xgettext only knows about c/c++ line-continuation strings
        is does not know about php's dot operator.
        We want to translate this entire paragraph as one string, of course.
      */
	 
      $html = "<p>";
      $html .= "PhpWiki is unable to save your changes, because another user edited and saved the page while you were editing the page too. If saving proceeded now changes from the previous author would be lost.";
      $html .= "</p>\n<p>";
      $html .= "In order to recover from this situation follow these steps:</p>";
      $html .= "\n<ol><li>";
      $html .= "Use your browser's <em>Back</em> button to go back to the edit page.";
      $html .= "\n</li><li>";
      $html .= "Copy your changes to the clipboard or to another temporary place (e.g. text editor).";
      $html .= "\n</li><li>";
      $html .= "<em>Reload</em> the page. You should now see the most current version of the page. Your changes are no longer there.";
      $html .= "\n</li><li>";
      $html .= "Make changes to the file again. Paste your additions from the clipboard (or text editor).";
      $html .= "\n</li><li>";
      $html .= "Press <em>Save</em> again.";
      $html .= "\n</li></ol>\n<p>";
      $html .= "Sorry for the inconvenience.";
      $html .= "</p>";
	 
      GeneratePage('MESSAGE', $html,
		   sprintf (gettext ("Problem while updating %s"), $pagename), 0);
    }
  else
    {
      $html = "<p>";
      $html .= "PhpWiki ne peut pas sauvegarder vos modifications, car un autre utilisateur modifia et sauvegarda cette m�me page pendant que vous �tiez en train de la modifier. Si vos modification �taient accept�es, celles du pr�c�dant utilisateur seraient perdues.";
      $html .= "</p>\n<p>";
      $html .= "A fin de rem�dier � cette situation suivez les �tapes suivantes :</p>";
      $html .= "\n<ol><li>";
      $html .= "Appuyer sur le bouton <em>Retour</em> de votre navigateur pour revenir � la page en cours de modification.";
      $html .= "\n</li><li>";
      $html .= "Recopier vos modifications � un autre endroit (par exemple via un �diteur de texte).";
      $html .= "\n</li><li>";
      $html .= "<em>Recharger</em> la page. Vous devriez maintenant voir apparaitre la version la plus r�cente de cette page. Vos modifications n'y figurent pas.";
      $html .= "\n</li><li>";
      $html .= "Modifier � nouveau la page en r�cup�rant vos modifications (de l'�diteur de texte par exemple).";
      $html .= "\n</li><li>";
      $html .= "Appuyer sur <em>Save</em> � nouveau.";
      $html .= "\n</li></ol>\n<p>";
      $html .= "D�sol� pour le d�rangement.";
      $html .= "</p>";
	 
      GeneratePage('MESSAGE', $html,
		   sprintf (gettext ("Probl�me pendant la mise � jour de %s"), $pagename), 0);
    }
  exit;
}


if (get_magic_quotes_gpc()) {
  $post = stripslashes($post);
}
$pagename = rawurldecode($post);
$pagehash = RetrievePage($dbi, $pagename, $WikiPageStore);

// if this page doesn't exist yet, now's the time!
if (! is_array($pagehash)) {
  $pagehash = array();
  $pagehash['version'] = 0;
  $pagehash['created'] = time();
  $pagehash['flags'] = 0;
  $newpage = 1;
} else {
  if (($pagehash['flags'] & FLAG_PAGE_LOCKED) && !defined('WIKI_ADMIN')) {
    // Gilles Casse (2003-01-21) for the Oralux website.
    // French + English
    if ("en" == getLang())
      {
	$html = "<p>" . gettext ("This page has been locked by the administrator and cannot be edited.");
	$html .= "\n<p>" . gettext ("Sorry for the inconvenience.");
	GeneratePage('MESSAGE', $html, sprintf (gettext ("Problem while editing %s"), $pagename), 0);
      }
    else
      {
	$html = "<p>" . gettext ("Cette page est verrouill�e par l'administrateur et ne peux pas �tre modifi�e.");
	$html .= "\n<p>" . gettext ("D�sol� pour le d�rangement.");
	GeneratePage('MESSAGE', $html, sprintf (gettext ("Probl�me lors de la mise � jour de %s"), $pagename), 0);
      }
    ExitWiki ("");
  }
     
  if(isset($editversion) && ($editversion != $pagehash['version'])) {
    ConcurrentUpdates($pagename);
  }
     
  // archive it if it's a new author
  if ($pagehash['author'] != $remoteuser) {
    SaveCopyToArchive($dbi, $pagename, $pagehash);
  }
  $newpage = 0;
}

// set new pageinfo
$pagehash['lastmodified'] = time();
$pagehash['version']++;
$pagehash['author'] = $remoteuser;

// create page header
$enc_url = rawurlencode($pagename);
$enc_name = htmlspecialchars($pagename);
// Gilles Casse (2003-01-21) for the Oralux website.
//    $html = sprintf(gettext("Thank you for editing %s."),
// 		   "<a href=\"$ScriptUrl?$enc_url\">$enc_name</a>");
//    $html .= "<br>\n";

if (! empty($content)) {
  //GC
  // Filter (external links,...) because of wiki abuse
  $page=preg_replace ("/<a href=\"http:\/\/(?!oralux.org|phpwiki|www.gnu.org)[^>]*>[^<]*<\/a>/i", "", $page);
  $page=preg_replace ("/\[?http:\/\/(?!oralux.org|phpwiki|www.gnu.org)[^\]]*\]?/i", "", $page);
  $page=preg_replace ("/[0-9]{4} [0-9a-zA-Z]*&[^ ]*[ ]/i", "", $page);
  $page=preg_replace ("/BAD URL -- remove all of &lt;, &gt;, &quot;/i", "", $page);
  $page=preg_replace ("/[ ]+/", " ", $page);

  // patch from Grant Morgan <grant@ryuuguu.com> for magic_quotes_gpc
  if (get_magic_quotes_gpc())
    $content = stripslashes($content);

  $pagehash['content'] = preg_split('/[ \t\r]*\n/', chop($content));

  // convert spaces to tabs at user request
  if (isset($convert)) {
    $pagehash['content'] = CookSpaces($pagehash['content']);
  }
}

for ($i = 1; $i <= NUM_LINKS; $i++) {
  if (! empty(${'r'.$i})) {
    if (preg_match("#^($AllowedProtocols):#", ${'r'.$i}))
      $pagehash['refs'][$i] = ${'r'.$i};
    else
      $html .= "<P>Link [$i]: <B>unknown protocol</B>" .
	" - use one of $AllowedProtocols - link discarded.</P>\n";
  }
}

InsertPage($dbi, $pagename, $pagehash);
UpdateRecentChanges($dbi, $pagename, $newpage);

// Gilles Casse (2003-01-21) for the Oralux website.
//    $html .= gettext ("Your careful attention to detail is much appreciated.");
//    $html .= "\n";

// fixme: no test for flat file db system
if (isset($DBMdir) && preg_match('@^/tmp\b@', $DBMdir)) {
  $html .= "<P><B>Warning: the Wiki DB files still live in the " .
    "/tmp directory. Please read the INSTALL file and move " .
    "the DBM file to a permanent location or risk losing " .
    "all the pages!</B>\n";
}

if (!empty($SignatureImg))
     $html .= "<P><img src=\"$SignatureImg\"></P>\n";

     $html .= "<hr noshade><P>";
     include('w_lib/transform.php');

     GeneratePage('BROWSE', $html, $pagename, $pagehash);
     ?>
