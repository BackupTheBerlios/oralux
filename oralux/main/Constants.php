<?PHP

// ----------------------------------------------------------------------------
// Constants.php
// $Id: Constants.php,v 1.1 2004/09/27 20:30:24 gcasse Exp $
// $Author: gcasse $
// Description: Defining the common constants. 
// $Date: 2004/09/27 20:30:24 $ |
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

// Ramdisk
// The ramdisk name must match with the ramdisk directory required by Knoppix.
// Warning: defined in oralux.conf too
define ("RAMDISK","/ramdisk");

// Oralux Directory at runtime (in the CD)
define ("ORALUX_RUNTIME","/usr/share/oralux");

// Where the emacspeak Makefile is stored useful to build e.g. the tcldtk.so shared library
define ("ORALUX_RUNTIME_EMACSPEAK",ORALUX_RUNTIME."/install/packages/emacspeak");

// Directory reserved to mount the yet unmounted file system e.g. for the partition 
// where an non free TTS is stored
define ("DIR_MOUNT", "/mnt/oralux_guest"); 

// Name of the known text to speech
define("FLITE_TTS", "flite");
define("DTK_TTS", "DECtalk");

// The default text to speech
define("GENERIC_TTS", FLITE_TTS);

// The distinct languages for the text to speech software
// WARNING
// The string is processed by main.c
define ("ENGLISH_TTS","english");
define ("FRENCH_TTS","french");
define ("GERMAN_TTS","german");
define ("SPANISH_TTS","spanish");

// Returned Status 
// WARNING
// The numerical value is processed by main.c
// And must match with the enumerate phpStatus 
define("OK",0);
define("DECTALK_CANT_BE_FOUND",1);
define("MAKEFILE_CANT_BE_FOUND",2);
define("TCLDTK_LIB_NOT_BUILT",3);
define("EMACSPEAK_NOT_DEFINED",4);
define("SERIAL_NUMBER_IS_EXPECTED",5);
define("SOFTWARE_DTK_NOT_FOUND",6);
define("DTK_DIRECTORY_NOT_FOUND",7);

// A few functions for tests
function ENTER($the_data)
{
  Debug("== Function $the_data");
}

function Debug($the_data)
{
  return;
  $file=FALSE;
  $html=FALSE;

  if ($html)
  {
    $ul="<ul>";
    $li="<li>";
    $il="</li>";
    $lu="</ul>";
  }
  else
  {
    $ul="\n";
    $li="\n- ";
    $il="";
    $lu="";
  }

  $a=$ul;

  if (is_array($the_data))
  {
    $a.=$ul;
    foreach ($the_data as $k=>$v)
      {
        $a.=$li; // Start of the <li> element
        $a.="key=\"$k\"";
        if($file)
        {
          $fd=fopen("log.html","a");
          if ($fd)
          {
            fwrite($fd,$a);
            fclose($fd);
          }
          else
          {
            echo $a;
          }
        }
        else
        {
          echo $a;
        }
        Debug($v);
        $a=$il; // End of the </li> element
      }
    $a.=$lu;
  }
  else
  {
    $a.=$li;
    $a.=$the_data;
    $a.=$il;
  }
  $a.=$lu;
  if($file && ($fd=fopen("log.html","a")))
  {
    fwrite($fd,$a);
    fclose($fd);
  }
  else
  {
    echo $a;
  }
}    

?>