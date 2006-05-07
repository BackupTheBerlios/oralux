#!/usr/bin/php -q
<?PHP 

// ----------------------------------------------------------------------------
// main.php
// $Id: dtkInstallerMain.php,v 1.3 2006/05/07 15:36:39 gcasse Exp $
// $Author: gcasse $
// Description: Defining the common constants. 
// $Date: 2006/05/07 15:36:39 $ |
// $Revision: 1.3 $ |
// Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
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

require_once("../main/Constants.php");
require_once("../text2speechinstaller/dtkInstaller.php");

function printStatus($theStatus, $theFoundPath, $theLanguage)
{
  $fd=fopen("/tmp/phpstatus","w");
  fwrite($fd, "$theStatus\n$theFoundPath\n$theLanguage\n");
  fclose($fd);
}

$aTextToSpeechLanguage=ENGLISH_TTS;
$aTextToSpeech=GENERIC_TTS;

if (FALSE==LookForDirectory("dtk", $aFoundPath))
{
  // For DOS
  if (LookForDirectory("DTK", $aFoundPath))
    {
      // For DOS
      system("mv $aFoundPath/DTK $aFoundPath/dtk");
    }
  else
    {
      printStatus(DTK_DIRECTORY_NOT_FOUND, $aFoundPath);
    }
  return;
}

$anInstaller=new DECtalkInstaller1("$aFoundPath/dtk");

if ($anInstaller->myStatus==OK)
{
  $anInstaller->placeInRamdisk();
}

// Today, we may install two languages.
// The last one in the attribute is the optional language (e.g french)
$aLanguage=$anInstaller->myTextToSpeechLanguage[count($anInstaller->myTextToSpeechLanguage)-1];

printStatus($anInstaller->myStatus, $aFoundPath, $aLanguage);

?>
