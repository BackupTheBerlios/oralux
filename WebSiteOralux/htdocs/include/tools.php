<?PHP
// ----------------------------------------------------------------------------
// tools.php
// $Id: tools.php,v 1.1 2004/09/28 21:48:44 gcasse Exp $
// $Author: gcasse $
// Description: A few helpful tools
// $Date: 2004/09/28 21:48:44 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2003 Gilles Casse (gcasse@oralux.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ----------------------------------------------------------------------------

function getLang()
{

//  echo phpInfo().
//echo "<hr>getLang() : $mylang";
  if (isset($_SESSION['mylang']))
  {
    $mylang = $_SESSION['mylang'];
  }
  else
  {
    $mylang = trim( $_SERVER["HTTP_ACCEPT_LANGUAGE"] );
  }

// The following code was written by Brion VIBBER and copied from 
// http://mail.wikipedia.org/pipermail/wikitech-l/2002-October/001068.html
//
// Its aim is to determine the required language from the Accept-Language variable.
//
// We run this code even if the language comes from an url or the session
// because the following code checks that the value belongs to 
// the expected languages

  $knownlangs = array( "en", "fr" ); #etc
  $lang = "en";
  $lastquality = 0.0;

// Note HTTP reference RFC 2616 - ftp://ftp.isi.edu/in-notes/rfc2616.txt

  $langtag = '((?:[a-zA-Z]{1,8})(?:-[a-zA-Z]{1,8})*)';
  $qvalue ='(0(?:\.[0-9]{1,3})?|1(?:\.0{1,3}))';
  $eachbit = '^' . $langtag . '(?:;q=' . $qvalue . ')?(?:,\s*)?(.*)$';

//echo "<hr>mylang=$mylang<br>\n\n";

  while(strlen($mylang))
  {
    if( preg_match( "/$eachbit/", $mylang, $m )) 
    {
      $tag = $m[1];
      $quality = $m[2];
      if(strlen($quality) == 0) 
        {
          $quality = 1;
        }
      $mylang = $m[3];
    	
    	//echo "language '$tag' quality '$quality'\n";
    	
    	if(in_array($tag, $knownlangs) and $quality > $lastquality) 
      {
    		$lang = $tag;
    		$lastquality = $quality;
    	}
    } 
    else 
    {
      break;
    }
  }
  return $lang;
}

// GetTranslatedFile
// Return the right filename according to the current language
// if the filename is "index.htm"
// 1. lang=="en", then nothing is done, and index.htm is returned
// 2. lang=="fr" then the returned name is "index-fr.htm"
function GetTranslatedFile($theFile)
{
  $aLanguage=getLang();
  if ("en"==$aLanguage)
  {
    $aFile=$theFile;
  }
  else
  {
    $a=split("\.",$theFile);
    if (2!=count($a))
    {
      die("Unexpected name \"$theFile\" in function tools.php/translateFile");
    }
    $aFile=$a[0]."-$aLanguage.".$a[1];
    
    // If the translation doesn't exist, we choose the default file
    if (!is_file($aFile))
    {
      $aFile=$theFile;
    }
  }

//  echo("<hr>GetTranslatedFile=$aFile");
  return $aFile;
}

$TheH1Num=0;
$TheH2Num=0;
$TheH3Num=0;
$TheH4Num=0;
$TheH5Num=0;
$TheH6Num=0;


// Useful to numbered the distinct headers 
// e.g.first h1 could be 1. A title
// third h2: 1.3 A title
// and so on

function getNumH1()
{
  global $TheH1Num;
  global $TheH2Num;
  global $TheH3Num;
  global $TheH4Num;
  global $TheH5Num;
  global $TheH6Num;
  ++$TheH1Num;
  echo "$TheH1Num.";
  $TheH2Num=0;
  $TheH3Num=0;
  $TheH4Num=0;
  $TheH5Num=0;
  $TheH6Num=0;
}

// $theNumberIsDisplayed=true => the number is displayed.
// It is useful to supply false, just to reset the h3, h4,... counters
function getNumH2($theNumberIsDisplayed=true)
{
  global $TheH1Num;
  global $TheH2Num;
  global $TheH3Num;
  global $TheH4Num;
  global $TheH5Num;
  global $TheH6Num;
  $aPrefix="";
  if (0!=$TheH1Num)
  {
    $aPrefix="$TheH1Num.";
  }
  if ($theNumberIsDisplayed)
  {
    ++$TheH2Num;
    echo "$aPrefix$TheH2Num";
  }
  $TheH3Num=0;
  $TheH4Num=0;
  $TheH5Num=0;
  $TheH6Num=0;
}

function getNumH3()
{
  global $TheH1Num;
  global $TheH2Num;
  global $TheH3Num;
  global $TheH4Num;
  global $TheH5Num;
  global $TheH6Num;
  $aPrefix="";
  if (0!=$TheH1Num)
  {
    $aPrefix="$TheH1Num.";
  }
  if (0!=$TheH2Num)
  {
    $aPrefix.="$TheH2Num.";
  }
  ++$TheH3Num;
  echo "$aPrefix$TheH3Num";
  $TheH4Num=0;
  $TheH5Num=0;
  $TheH6Num=0;
}

function getNumH4()
{
  global $TheH1Num;
  global $TheH2Num;
  global $TheH3Num;
  global $TheH4Num;
  global $TheH5Num;
  global $TheH6Num;
  $aPrefix="";
  if (0!=$TheH1Num)
  {
    $aPrefix="$TheH1Num.";
  }
  if (0!=$TheH2Num)
  {
    $aPrefix.="$TheH2Num.";
  }
  if (0!=$TheH3Num)
  {
    $aPrefix.="$TheH3Num.";
  }
  $TheH4Num++;
  echo "$aPrefix$TheH4Num";
  $TheH5Num=0;
  $TheH6Num=0;
}

function getNumH5()
{
  global $TheH1Num;
  global $TheH2Num;
  global $TheH3Num;
  global $TheH4Num;
  global $TheH5Num;
  global $TheH6Num;
  $aPrefix="";
  if (0!=$TheH1Num)
  {
    $aPrefix="$TheH1Num.";
  }
  if (0!=$TheH2Num)
  {
    $aPrefix.="$TheH2Num.";
  }
  if (0!=$TheH3Num)
  {
    $aPrefix.="$TheH3Num.";
  }
  if (0!=$TheH4Num)
  {
    $aPrefix.="$TheH4Num.";
  }
  $TheH5Num++;
  echo "$aPrefix$TheH5Num";
  $TheH6Num=0;
}

function getNumH6()
{
  global $TheH1Num;
  global $TheH2Num;
  global $TheH3Num;
  global $TheH4Num;
  global $TheH5Num;
  global $TheH6Num;
  $aPrefix="";
  if (0!=$TheH1Num)
  {
    $aPrefix="$TheH1Num.";
  }
  if (0!=$TheH2Num)
  {
    $aPrefix.="$TheH2Num.";
  }
  if (0!=$TheH3Num)
  {
    $aPrefix.="$TheH3Num.";
  }
  if (0!=$TheH4Num)
  {
    $aPrefix.="$TheH4Num.";
  }
  if (0!=$TheH5Num)
  {
    $aPrefix.="$TheH5Num.";
  }
  $TheH6Num++;
  echo "$aPrefix$TheH6Num";
}

?>