#! /usr/bin/php
<?php
// ----------------------------------------------------------------------------
// getKnoppixSources.php
// $Id: getKnoppixSources.php,v 1.1 2004/11/25 22:58:43 gcasse Exp $
// $Author: gcasse $
// Description: retrieve the sources of the Knoppix packages supplied in a list 
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


# Getting index.html using wget
$aKnoppixPath="http://developer.linuxtag.net/knoppix/sources";
system("wget $aKnoppixPath");

$flog=fopen("getKnoppixSources.log", "w");
$fsource=fopen("index.html","r");

# download each url
while (!feof ($fsource))
{
  $aLine=fgets($fsource);

  // getting the url
  $aUrlPattern='/<a href=[^>]+>([^<]+)<\/a>/';
  if (preg_match( $aUrlPattern, $aLine, $regs))
    {
      $aUrl="$aKnoppixPath/".$regs[1];
  
      // getting the destination filename
      $aUrlPattern='/\/([^\/]+)$/';
      if (preg_match($aUrlPattern, $aUrl, $regs))
	{
	  echo "$aUrl\n";

	  $aFilename=$regs[1];
	  fprintf($flog,"# $aFilename\n");
	  
	  $fdest=fopen( $aFilename,"w");
	  $ch = curl_init($aUrl);
	  curl_setopt($ch, CURLOPT_FILE, $fdest);
	  curl_setopt($ch, CURLOPT_HEADER, 0);                           
	  curl_exec($ch);
	  curl_close($ch);
	  fclose($fdest);
	}
    }
}

fclose($flog);
fclose($fsource);

?>