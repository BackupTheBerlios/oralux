#! /usr/bin/php
<?php
// ----------------------------------------------------------------------------
// getDebianSources.php
// $Id: getDebianSources.php,v 1.1 2004/11/25 22:58:43 gcasse Exp $
// $Author: gcasse $
// Description: retrieve the sources of the Debian packages supplied in a list 
// from http://snapshot.debian.net
// For Knoppix, see http://developer.linuxtag.net/knoppix/sources
// 
// $Date: 2004/11/25 22:58:43 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2004 Gilles Casse (gcasse@oralux.org)
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


// {{{ syntax
function syntax()
{
    echo "1. getDebianSources.php filename\n";
    echo "retrieve the URL of the packages sources listed in filename\n";
    echo "and copy these URLs in getDebianSources.log\n"; 
    echo "where filename is a file composed of such lines:\n";
    echo "List format: <status> <packageName> <version>\n";
    echo "for example:\n"; 
    echo "ii adduser 3.51\n";
    echo "ii aspell-bin 0.50.5-1\n";
    echo "\nThis list can be obtained by:\n";
    echo "COLUMNS=150 dpkg -l | awk '{print $1,$2,$3}'\n";
    echo "\n2. getDebianSources.php --download\n";
    echo "to download the URLs in getDebianSources.log\n";
}
// }}}

// {{{ getDebianUrl
// return an array of urls related to the source of the package and version supplied.
function getDebianUrl( $thePackage, $theVersion, & $theUrlArray)
{
  echo "Package=$thePackage, Version=$theVersion\n";

  $ch = curl_init("http://snapshot.debian.net/package/$thePackage");
  $fd=tmpfile();
  //  $fd=fopen("getDebianSources.html","w+");
  curl_setopt($ch, CURLOPT_FILE, $fd);
  curl_setopt($ch, CURLOPT_HEADER, 0);                           
  curl_exec($ch);
  curl_close($ch);
  fseek($fd,0);

  $aVersionPattern="<TH>$theVersion</TH>";
  //$aUrlPattern="<LI><A HREF="http://snapshot.debian.net/archive/2004/03/10/debian/pool/main/y/yasr/yasr_0.6.5-1.dsc">yasr_0.6.5-1.dsc</A></LI>"

  $aUrlPattern='/.*<LI><A HREF="(http:\/\/snapshot\.debian\.net\/archive\/.*([^d][^s][^c]))">([^<].*)<\/A><\/LI>/';
  $aEndPattern="<TR>";

  $aState=0;
  define("SearchVersion",$aState++);
  define("GetUrl",$aState++);
  define("EndVersion",$aState++);

  $aState=SearchVersion;
  while (!feof ($fd) && ($aState != EndVersion))
    {
      $aLine=fgets($fd);
      switch($aState)
	{
	case SearchVersion:
	  if (strncmp($aLine, $aVersionPattern, strlen($aVersionPattern))==0)
	    {
	      $aState=GetUrl;
	    }
	  break;
	case GetUrl:
	  if (strncmp($aLine, $aEndPattern, strlen($aEndPattern))==0)
	    {
	      $aState=EndVersion;
	    }
	  else
	    {
	      if (preg_match($aUrlPattern, $aLine, $regs))
		{
		  $theUrlArray[]=$regs[1];
		}
	    }
	  break;
	default:
	  break;
	}
    }

  fclose($fd);
}
// }}}

// {{{ listFilename

function listFilename($theFilename)
{
  $fd=fopen( $theFilename, "r");
  $fdlog=fopen("getDebianSources.log", "w");
  
  while (!feof ($fd))
    {
      $aLine=fgets($fd);
      $aWord=explode(" ", $aLine);
      
      if (count($aWord)!=3)
	{
	  continue;
	}
      
      $aUrlArray=array();
      // Remove the final return
      preg_match("/(.*)$/", $aWord[2], $regs);

      getDebianUrl($aWord[1], $regs[1], $aUrlArray);
      fprintf( $fdlog, "# ".$aWord[1]." ".$aWord[2]."\n");
      foreach( $aUrlArray as $aUrl)
	{
	  fprintf( $fdlog, "$aUrl\n");      
	}
    }

  fclose($fdlog);
  fclose($fd);
}
// }}}
// {{{ downloadUrl
function downloadUrl()
{
  $fd=fopen("getDebianSources.log", "r");
  while (!feof ($fd))
    {
      $aUrl=fgets($fd);

      $aUrlPattern='/[^#]+\/([^\/]+)/';
      if (preg_match($aUrlPattern, $aUrl, $regs))
	{
	  $fdsource=fopen($regs[1],"w");

	  echo $aUrl;
	  $ch=curl_init($aUrl);
	  curl_setopt($ch, CURLOPT_FILE, $fdsource);
	  curl_setopt($ch, CURLOPT_HEADER, 0);                           
	  curl_exec($ch);
	  curl_close($ch);
	  fclose($fdsource);
	}
    }
  fclose($fd);
}
// }}}

// {{{ main

# This script expects a list of Debian packages

if ($argc!=2) 
{
  syntax();
  exit(1);
}

if (is_readable($argv[1]))
{
  listFilename( $argv[1]);
}
else if ($argv[1]=="--download")
{
  downloadUrl();
}
else
{
  syntax();
  exit(1);
}





// }}}

?>