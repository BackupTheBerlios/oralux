<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">
<head>
 <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
 <meta name="description" content="Archives de la mailing liste">
 <meta name="KeyWords" content="archives, mailing-list, liste, diffusion">
 <meta name="Author" content="Jean-Christophe 'Jice' Cardot (taz98 chez altern.org)">
 <meta name="LICENSE" content="GPL (GNU Public License)">
 <meta name="Version" content="1.1">
 <meta name="created" content="20020628;00000000">
 <meta name="changedby" content="Jice">
 <meta name="changed" content="20020901;00000000">
 <meta name="GENERATOR" content="explore2.php3">
 <title>Archives</title>
</head>
<body>
<?
// RAF GC: penser a creer le repertoire .cache
/* Ce programme récupère les pages générées par http://listes.tuxfamily.org
   et les travaille pour les rendre propres et navigables (décodage mime,
   décodage ISO, encodage anti-spam des adresses de courriel, colorisation,
   traduction en français, extraction des fils de discussion et navigation
   dedans, affichage à plat d'un fil, etc.)
   A l'origine, on a :
   http://listes.tuxfamily.org/?A=LIST&L=aide_lea-linux.org
     -> affichage de la liste des mois de l'archive
   http://listes.tuxfamily.org/?L=aide_lea-linux.org&Y=2002&M=6
     -> affichage de la liste des messages d'un mois
   http://listes.tuxfamily.org/?A=READ&L=aide_lea-linux.org/2002/7/30/12
     -> affichage d'un message
*/

global $M, $Y, $A, $L; // les variables de listes.tuxfamily.org
global $PHP_SELF, $thread, $aplat; // nos variables

global $serveur_listes, $liste, $liste_prefix, $fichier_cache_prefix, $couleur;
global $thread_sujet, $thread_de, $thread_date;

// constantes utilisateur /////////////////////////////////////////////////////////////////////

$serveur_listes = "listes.lost-oasis.net/" ;
$liste = "ml_oralux.org"; //"aide_lea-linux.org";
$liste_prefix = "[Oralux]";
$fichier_cache_prefix = ".cache/archives-ml_";
$titre = "<p class='leatitre'>Forum</p>";

// fin des modifications utilisateurs /////////////////////////////////////////////////////////

echo $titre;

// RAF GC: LC_ALL au lieu de "LC_ALL"
setlocale(LC_ALL, "fr_FR"); // pour que '\b' dans les regex ne matche pas sur les caractères accentués

$pathinitial = "A=LIST&L=$liste";

$mois[1] = "Janvier";
$mois[2] = "Février";
$mois[3] = "Mars";
$mois[4] = "Avril";
$mois[5] = "Mai";
$mois[6] = "Juin";
$mois[7] = "Juillet";
$mois[8] = "Août";
$mois[9] = "Septembre";
$mois[10] = "Octobre";
$mois[11] = "Novembre";
$mois[12] = "Décembre";


function unmime($s) { // decodage mime (=?iso-8859-1?Q?l=E9a?= et autres =?iso-8859-1?B?bOlh?=)
  $elements = imap_mime_header_decode($s);
  $s = "";
  for($i=0 ; $i<count($elements) ; $i++) {
    $s .= $elements[$i]->text;
  }
  return $s;
}


function unboubouge($s) { // decodage mime supplémentaire (superflu ?)
  $s = str_replace("_", " ", $s);
  $s = preg_replace("/\=([0-9a-f]{2})/ei", "chr(hexdec('$1'))", $s);
  return $s;
}


function my_trim($s) { // renvoie le sujet d'un thread à partir du sujet d'un message
  global $liste_prefix;
  return trim(preg_replace(array("/\<[^\>]+\>/","/\bre\:/i","/\btr\:/i","/\bfwd\:/i","/".preg_quote($liste_prefix)."/i","/\s+/"), array("","","","",""," "), $s));
}


function get_threads($Y, $M) { // charge les fils de discussion d'apres le cache
                               // ou le (ré)génère si besoin.
  global $serveur_listes, $liste, $fichier_cache_prefix;
  global $thread_sujet, $thread_de, $thread_date;
  $now = time();
  $mois_en_cours = strftime("%m", $now); // mois consulté : $M
  $fichier_cache = sprintf("$fichier_cache_prefix%04d%02d.cache", $Y, $M); // format : archives-ml_200207.cache pour juillet 2002
  // cas où on va générer le cache
  if (   !file_exists($fichier_cache) // si le fichier n'existe pas
      || !filesize($fichier_cache)    // ou existe mais est vide
      || (   ($M < $mois_en_cours)    // ou on consulte un mois précédent et
          && (strftime("%m", filemtime($fichier_cache)) == $M) // le fichier date encore de ce mois
         )                            // (dernière génération pour ce mois là)
      || (   ($M == $mois_en_cours)   // ou on consulte le mois en cours et
          && ($now - filemtime($fichier_cache) > 3600) // le fichier n'est plus à jour
         )
     ) { // alors on génère le cache
    $page = file("http://$serveur_listes?L=$liste&Y=$Y&M=$M");
    if ($page) {
      $page = explode("\n", unmime(join("", $page))); // violent, mais nécessaire ;)
      $page = preg_replace("/^\t\t\t/m", "", $page);
      $fpcache = fopen($fichier_cache, "w");
      if (flock($fpcache, 2)) {
        fwrite($fpcache, "<? // fichier cache généré, ne pas modifier\n\n");
        for ($i=16 ; $i<count($page) ; $i+=12) {
          $sujet = $page[$i+3];
          $de = $page[$i+6];
          $date = $page[$i+9];
          if ($sujet) {
            preg_match("/\&L\=([^\"]*?)\"/", $sujet, $m);
            list ($sujet_post,) = get_post($m[1]);
            $sujet_thread = my_trim($sujet_post);
            $thread_sujet[$sujet_thread][] = $sujet;
            $thread_de[$sujet_thread][] = $de;
            $thread_date[$sujet_thread][] = $date;
            fwrite($fpcache, '$thread_sujet["' . addslashes($sujet_thread) . '"][]="' . addslashes($sujet) . "\";\n");
            fwrite($fpcache, '$thread_de["' . addslashes($sujet_thread) . '"][]="' . addslashes($de) . "\";\n");
            fwrite($fpcache, '$thread_date["' . addslashes($sujet_thread) . '"][]="' . addslashes($date) . "\";\n\n");
          }
        }
        fwrite($fpcache, "?>");
        fclose($fpcache);
      } else { // on n'a pas eu le lock
        //BUG: on fait quoi ?
        echo "<p>1:Impossible de charger la page !</p>";
      }
    } else { // on n'a pas eu la page http://$serveur?$path
      //BUG: on fait quoi ?
      echo "<p>2:Impossible de charger la page !</p>";
    }
  } else { // sinon on utilise le cache
    include $fichier_cache;
  }
} // get_threads()


// codes couleur : http://www.htmlgoodies.com/tutors/colors.html
$couleur['&gt;']='red';
$couleur['&gt;&gt;']='green';
$couleur['&gt;&gt;&gt;']='darkviolet';
$couleur['&gt;&gt;&gt;&gt;']='brown';
$couleur['&gt;&gt;&gt;&gt;&gt;']='darkorange';
$couleur['&gt;&gt;&gt;&gt;&gt;&gt;']='crimson';
$couleur['&gt;&gt;&gt;&gt;&gt;&gt;&gt;']='indigo';
function couleur_reponse($prefix) {
  // renvoie la couleur qui sert à coloriser les différents niveaux
  // de réponse dans un message.
  global $couleur;
  $prefix = str_replace(" ", "", $prefix);
  if ($couleur[$prefix]) { return $couleur[$prefix]; }
  else { return 'black'; }
}

function get_post($L) { // charge un message ($L de la forme 'aide_lea-linux.org/2002/7/31/3') + cache
  global $fichier_cache_prefix;
  preg_match("~/(\d\d\d\d)/(\d\d?)/(\d\d?)/(\d+)$~", $L, $m);
  $fichier_cache = sprintf("$fichier_cache_prefix%04d%02d%02d-%d.cache", $m[1], $m[2], $m[3], $m[4]); // format : archives-ml_20020805-12.cache pour le 12ème message du 5 août 2002
  if (   !file_exists($fichier_cache) // si le fichier n'existe pas
      || !filesize($fichier_cache)    // ou existe mais est vide
     ) {
    global $serveur_listes;
    if ($post = file("http://$serveur_listes?A=READ&L=$L")) {
      $post = unmime(join("", $post));
      preg_match("/Subject\:(.*?)\n/", $post, $matches);
      $sujet_post = $matches[1];
      $searches = array(
        "/^.*?\<BODY[^\>]*?\>/is",      // supprime tout jusqu'à <body> compris
        "~\</BODY\>.*$~is",             // supprime </body> et la suite
        //"/\<PRE\>\-\-(.*?)\n\-\-/es", // pour decoder l'encodage mime du message
        //"/\=\?iso\-8859\-15?\?Q\?(.*?)\?\=/sie", // décode l'encodage mime
//        "~</PRE><A\sHREF\=\".*?\"\>Attachment\:\s\nContent\-Type\:\s[^\;]+?\;\n\s+charset\=[^\n]+?\nContent\-Transfer\-Encoding\:\s[^\<]+?\</A\>\<BR\>\</P\>~", // supprime les attachements
        "~</PRE><A\sHREF\=\".*?\"\>Attachment\:[^\<]+?\</A\>\<BR\>\</P\>~", // supprime les attachements
        "/\=\n/", // idem (' =' en fin de ligne)
        "~\</?PRE\>~",                  // supprime <PRE> et </PRE>
        "/^\n\s*Date\:(.*?)\nFrom\:(.*?)\nTo\:(.*?)\n(cc\:.*?\n)?Subject\:(.*?)\n/", // traduction de l'entête
        "/^\s*((?:\&gt\;\ *)+)(.*)$/me",   // colorisation des niveaux de réponse
        "/\n/",                         // remplace \n par <br>
      );
      $replaces = array(
        "",
        "",
        //"\"<PRE>--\".unboubouge('$1').\"\n--\"",
        //"unboubouge('$1')",
        "",
        "",
        "",
        "&nbsp;<b>Date</b>&nbsp;:$1\n&nbsp;&nbsp;&nbsp;<b>De</b>&nbsp;:$2\n&nbsp;&nbsp;&nbsp;&nbsp;<b>&Agrave;</b>&nbsp;:$3\n$4<b>Sujet</b>&nbsp;:$5<hr />",
        "\"<font color='\".couleur_reponse('$1').\"'>$1\".stripslashes('$2').\"</font>\"",
        "\n<br />",
      );
      $post = preg_replace($searches, $replaces, $post);
      $post = unboubouge($post);
      // on écrit le cache
      $fpcache = fopen($fichier_cache, "w");
      if (flock($fpcache, 2)) {
        fwrite($fpcache, "<? // fichier cache généré, ne pas modifier\n");
        fwrite($fpcache, '$sujet_post=\''. str_replace('\'', '\\\'', $sujet_post) ."';\n");
        fwrite($fpcache, '$post=\''. str_replace('\'', '\\\'', $post) ."';\n");
        fwrite($fpcache, "?>");
        fclose($fpcache);
      } else { // BUG: on n'a pas eu le lock, on fait quoi ?
      }
    } else {
      echo "<p>3:Impossible de charger la page !</p>";
      $post = "";
      $sujet_post = "";
    }
  } else { // sinon on utilise le cache
    include $fichier_cache;
  }
  return array($sujet_post, $post);
}

//

if ($thread == 1) { // affichage par fils de discussion de la liste des messages
  // 1. on charge les threads (depuis le cache ou regénération)
  get_threads($Y, $M);
  // 2. on affiche la liste des messages par fil de discussion
  $page = "<TABLE WIDTH='100%' BORDER='0'>\n<TR>\n<TH ALIGN='CENTER'>Sujet</TH>\n<TH ALIGN='CENTER'>Auteur</TH>\n<TH ALIGN='CENTER'>Date</TH>\n</TR>\n";
  reset($thread_sujet);
  while(list($st,$v) = each($thread_sujet)) {
    preg_match("/\&L\=([^\"]*?)\"/", $thread_sujet[$st][0], $matches);
    $page .= "<TR><TD COLSPAN='3'>[<i><a href='$PHP_SELF?A=READ&L=" . $matches[1] . "&aplat=1'>Affichage à plat</a></i>]</TD></TR>\n";
    while(list($clef,$sujet) = each($thread_sujet[$st])) {
      $page .= "<TR>\n<TD ALIGN='LEFT'>"
            . stripslashes($sujet) . "</TD>\n<TD ALIGN='LEFT'>"
            . stripslashes($thread_de[$st][$clef]) . "</TD>\n<TD ALIGN='LEFT'>"
            . $thread_date[$st][$clef] . "</TD>\n</TR>\n\n";
    }
    $page .= "<TR><TD COLSPAN='3'><HR></TD></TR>\n\n";
  }
  $page .= "</TABLE>";


} else if ($A == "READ") { // affichage d'un message ou d'un fil
  // on récupère les fils
  preg_match("~/(\d\d\d\d)/(\d\d?)/~", $L, $matches);
  get_threads($matches[1], $matches[2]);

  if ($aplat == 1) { // affichage à plat d'un fil de discussion
    list($sujet_init, $page_init) = get_post($L);
    $sujet_thread = my_trim($sujet_init);
    $page = "<p>Affichage du fil de discussion : <b>$sujet_thread</b></p>\n<hr>\n";
    $sujet_thread = addslashes($sujet_thread);
    while(list($clef,$sujet) = each($thread_sujet[$sujet_thread])) {
      if (preg_match("~".preg_quote($L)."~", $sujet)) {
        $page .= $page_init . "<hr>\n";
      } else {
        preg_match("/\&L\=([^\"]*?)\"/", $sujet, $matches);
        list($sujet, $message) = get_post($matches[1]);
        $page .= $message . "<hr>\n";
      }
    }
    $page2 = "<p>[<a href='$PHP_SELF?A=READ&L=$L'>Affichage replié</a>]</p>";

  } else { // affichage d'un seul message, suivi de la liste des messages du fil
    list($sujet_message, $page) = get_post($L);
    // affichage de la liste des messages du thread
    $page2 = "<hr>\n<p><b>Fil de discussion :</b> [<i><a href='$PHP_SELF?A=READ&L=$L&aplat=1'>Affichage à plat</a></i>]</p>\n"
           . "<TABLE WIDTH='100%' BORDER='0'>\n<TR>\n<TH ALIGN='CENTER'>Sujet</TH>\n<TH ALIGN='CENTER'>Auteur</TH>\n<TH ALIGN='CENTER'>Date</TH>\n</TR>\n";
    $sujet_thread = addslashes(my_trim($sujet_message));
/*
echo "<br>sujet_thread='$sujet_thread'";
if ($thread_sujet[$sujet_thread])
 echo "<br>le thread existe";
else
 echo "<br>le thread n'existe pas !";
*/
    if (is_array($thread_sujet))
    {
      while(list($clef,$sujet) = each($thread_sujet[$sujet_thread])) {
        $page2 .= "<TR>\n<TD ALIGN='LEFT'>"
          . stripslashes($sujet) . "</TD>\n<TD ALIGN='LEFT'>"
          . stripslashes($thread_de[$sujet_thread][$clef]) . "</TD>\n<TD ALIGN='LEFT'>"
          . $thread_date[$sujet_thread][$clef] . "</TD>\n</TR>\n\n";
      }
    }
    $page2 .= "</TABLE>\n";
  }


} else { // affichage "normal" d'une page (i.e. non spécialement traitée)
         // -> notamment le cas de la liste des messages non triés
  if ($argv[0] && ($page = file("http://$serveur_listes?$argv[0]"))) {
    $page = unmime(join("", $page));
  } else {
    $page = "";
  }
}


$server = "(?:[a-z0-9_-]+\.)+[a-z0-9-]{1,3}(?:\:\d+)?" ;
$user = "[a-z0-9_\.-]*?[a-z0-9-]+" ;

// remplacements qui ne seront effectués que dans la liste des mois de l'archive
$searches0 = array (
  "/^.*?\<BODY[^\>]*?\>/is",              // supprime tt jusqu'à <body> compris
  "~\</BODY\>.*$~is",                     // supprime </body> et la suite
  "/\<A\sHREF\=\"\/\?([^\"]*)\"\>(.*?)\<\/A\>/i",     // modifie les urls pour pointer sur ce script
  "/\bJan\b/i", "/\bFeb\b/i", "/\bMar\b/i", "/\bApr\b/i", "/\bMay\b/i", "/\bJun\b/i",
  "/\bJul\b/i", "/\bAug\b/i", "/\bSep\b/i", "/\bOct\b/i", "/\bNov\b/i", "/\bDec\b/i",
);
$replaces0 = array (
  "",
  "",
  "<A HREF='$PHP_SELF?$1'>$2</A>&nbsp;<A HREF='$PHP_SELF?$1&thread=1'>[T]</A> | ",
  "Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
  "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre",
);

// remplacements effectués dans toutes les autres pages (liste des messages, messages)
$searches = array (
  "/^.*?\<BODY[^\>]*?\>/is",              // supprime tt jusqu'à <body> compris
  "~\</BODY\>.*$~is",                     // supprime </body> et la suite
  "/\<A\sHREF\=\"\/\?([^\"]*)\"\>/i",     // modifie les urls pour pointer sur ce script
  "/($user)\@($server)/i",                // modifie les emails (anti spam)
  "/\bJan\ /i", "/\bFeb\ /i", "/\bMar\ /i", "/\bApr\ /i", "/\bMay\ /i", "/\bJun\ /i",
  "/\bJul\ /i", "/\bAug\ /i", "/\bSep\ /i", "/\bOct\ /i", "/\bNov\ /i", "/\bDec\ /i",
  "/\bMon\,/i", "/\bTue\,/i", "/\bWed\,/i", "/\bThu\,/i", "/\bFri\,/i", "/\bSat\,/i", "/\bSun\,/i",
);
$replaces = array (

  "",
  "",
  "<A HREF='$PHP_SELF?$1'>",
  "$1 at $2",
  "Janvier ", "Février ", "Mars ", "Avril ", "Mai ", "Juin ",
  "Juillet ", "Août ", "Septembre ", "Octobre ", "Novembre ", "Décembre ",
  "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam", "Dim",
);

// affichage de la liste des mois de l'archive
echo preg_replace($searches0, $replaces0, join("", file("http://$serveur_listes?$pathinitial")));
if (!$argv[0]) {
  echo "<p>Cliquez sur les [T] pour une vision triée par fil de discussion...</p>";
}

// affichage de la page
if ($argv[0]) {
  echo "<hr>\n";
  if (($A != "READ") && $M && $Y) { // si liste des messages => on affiche le mois
    echo "<h"."2 align='center'>".$mois[$M]." $Y</h"."2>\n";
    if (!$thread) {
      echo "<p align='center'>(<i><a href='$PHP_SELF?$argv[0]&thread=1'>Afficher par fils de discussion</a></i>)</p>\n";
    }
  }
  $page = preg_replace($searches, $replaces, $page);
  echo "<tt>" . $page . "</tt>\n";
  if ($page2) { // page2 = fil de discussion dans l'affichage d'un message
    $searches2 = array (
      "/\<A\sHREF\=\"\/\?([^\"]*)\"\>/i",     // modifie les urls pour pointer sur ce script
      "/($user)\@($server)/i",                // modifie les emails (anti spam)

    );
    $replaces2 = array (
      "<A HREF='$PHP_SELF?$1'>",
      "$1 at $2",
    );
    echo preg_replace($searches2, $replaces2, $page2);
  }
}
?>
<p><em>Les propos tenus dans cette liste de diffusion, dont vous disposez ici de l'archive, n'engagent que leurs auteurs respectifs.</em></p>
</body>
</html>