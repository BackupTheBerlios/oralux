<?PHP
// ----------------------------------------------------------------------------
// dtkInstaller1.php
// $Id: dtkInstaller.php,v 1.3 2006/05/07 15:36:39 gcasse Exp $
// $Author: gcasse $
// Description: placing the DECtalk software in ramdisk 
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
require_once("../text2speechinstaller/Files.php");

// Install tree
// Where most of the DECtalk directories are dynamically copied
define ("INSTALL_TREE", RAMDISK."/dtk");

// Where the emacspeak files for the DECtalk speech server are built (Makefile, tcldtk.o, tcldtk.so)
// Warning: defined in oralux.conf too
define("DTK_EMACSPEAK",INSTALL_TREE."/emacspeak");

// This file lists the installed tarballs and their features (name, size, last modifcation).
// This is for a future use
// It will be useful to identify a new DECtall tarball (a new language or an update).
//  
define ("DTK_CONF","oralux_dtk.conf");

// Where DECtalk libraries are stored
define ("DTK_PATH_LIB", "/ramdisk/dtk/lib");

class DECtalkInstaller1
{
  // Public attribute
  var $myTextToSpeechLanguage=array("ENGLISH_TTS");
  var $myStatus=OK;

  // Private attributes

  // _myInstallationIsPossible equals TRUE if the DECtalk packages were found.
  var $_myInstallationIsPossible=FALSE;

  // if _isFirstInstallation equals TRUE, it means that this is the first installation
  // Then, the DECtalk installer must be called.
  // if FALSE, the files are already extracted from the tarballs.  
  var $_isFirstInstallation=TRUE;

  // _myOptionalDirectory is an array including the distinct optional directories (e.g. french,...) 
  var $_myOptionalDirectory=null;

  // _myStorageDirectory is the directory where the tarbals and the DECtalk files are stored
  var $_myStorageDirectory="";

  var $_myDECtalkLibrary=array(
  "tts_us", 
  "tts_fr",
  "tts_gr",
  "tts_sp",
  );

  var $_myTTSIdentifier=array(
  ENGLISH_TTS, 
  FRENCH_TTS,
  GERMAN_TTS,
  SPANISH_TTS,
  );

  // Public methods
// Constructor
// This method copies the DECtalk files from the storage directory to the RAMDISK.
//
// If the files are still in the tarball, they are extracted from.
//
// The detailed scenario is:
// First, It looks for the DECtalk tarballs (tar.gz) or the related directories.
// 
// If the DECtalk directory is present, the method supposes that the packages
// were already installed.
// If the DECtalk directory is not present, the tarballs are 'untared'
//
// Finaly the DECtalk directories are copied to our ramdisk
//
// Input:
// $theSourceDirectory: the directory where the packages must be found
//
// Output: 
// $theOptionalDirectory: an array of the directories for the optional languages
// e.g: 0 => french
//
// Returned value:
// true if the DECtalk directory is found
//
// Notice:
// If the DECtalk directory is already present, the optional directories are just returned
//
  function DECtalkInstaller1( $theStorageDirectory)
    { 
      ENTER("DECtalkInstaller1");
      Debug("Input: $theStorageDirectory");
      unset($this->_myOptionalDirectory); // to 'clean' this variable
      $this->_isFirstInstallation=FALSE;
      $this->_myStorageDirectory=$theStorageDirectory;

      // We check the present directories
      if (TRUE==$this->_LookForMyDirectory( $theStorageDirectory, $aTarBall, $this->_myOptionalDirectory))
      {
        // We copy the DECtalk directory and the optional directories from $theStorageDirectory to our ramdisk
	if (is_array($this->_myOptionalDirectory))
  	{
          array_push( $this->_myOptionalDirectory, "DECtalk"); //Adding "DECtalk" to the list!
	}
        CopyFiles( $theStorageDirectory, RAMDISK, $this->_myOptionalDirectory);
	if (is_array($this->_myOptionalDirectory))
  	{
	  array_pop( $this->_myOptionalDirectory); //Removing "DECtalk" from the list
	}
        return;
      }
      else if (0==count($aTarBall))
      { // No tarball!
          $this->myStatus=DECTALK_CANT_BE_FOUND;
          return;
      }

      // We extract the files from the tarballs and copy them to RAMDISK
      // The name of the tarball, its features are copied in a file (if possible) for a future use
      $aConf=fopen("$theStorageDirectory/".DTK_CONF,"w");
      foreach($aTarBall as $aFile)
        {
          if ($aConf)
          {
            // Storing the features of the file
            $aStat=stat("$theStorageDirectory/$aFile");
            // Name
            fwrite($aConf, $aFile);
            // Size
            fwrite($aConf, ",".$aStat[7]);
            // Last modification
            fwrite($aConf, ",".$aStat[9]."\n");
          }
            
          // Restoring the archive
          $aCommand="tar --directory ".RAMDISK." -zxf $theStorageDirectory/$aFile";
          Debug($aCommand);
          system($aCommand);
        }
      
      if ($aConf)
      {
        fclose($aConf);
      }

      // We check once more the present directories
      unset($this->_myOptionalDirectory);
      if (FALSE==$this->_LookForMyDirectory( RAMDISK, $aTarBall, $this->_myOptionalDirectory))
      {
          $this->myStatus=DECTALK_CANT_BE_FOUND;
          return;
      }

      $this->_isFirstInstallation=TRUE;
    }

// placeInRamdisk
// This method calls the DECtalk installer which is expected to place the DECtalk files in ramdisk
//
// This method will customize the BASE_DIR variable (in files.sh or installer)
// so that BASE_DIR=INSTALL_TREE
//
// Status:
// OK: DECtalk is useable
// SERIAL_NUMBER_IS_EXPECTED: this is the first installation
// Other status for fatal errors
//
// If Status==OK, myTextToSpeechLanguage is updated
//

  function placeInRamdisk()
    {
      ENTER("placeInRamdisk");
      // By default the generic tts is used
      $this->myTextToSpeechLanguage[0]=ENGLISH_TTS;

      if (OK!=$this->_myStatus)
      {
        return;
      }

      // Creating the new tree into the ramdisk
      $aCommand="mkdir -p ".INSTALL_TREE;
      Debug($aCommand);
      system($aCommand);
 
      // Copying the Makefile for tcdtk.so (emacspeak)
      Debug("Copying the Makefile");
      $aMakefile=ORALUX_RUNTIME_EMACSPEAK."/Makefile";
      if (@is_file($aMakefile))
      {
        unset($aFile);
        $aFile[0]="Makefile";
        CopyFiles( ORALUX_RUNTIME_EMACSPEAK, DTK_EMACSPEAK, $aFile);
      }
      else
      {
        $this->myStatus=MAKEFILE_CANT_BE_FOUND;
        return;          
      }

      if (FALSE==$this->_isFirstInstallation)
      {
        // This is not the first installation
        // We have just to copy the DECtalk files from the storage directory to the good place
        $oldTree=$this->_myStorageDirectory."/DECtalk";

        unset($aFile);
        $aFile[0]="libtts*.so";
        CopyFiles( $oldTree, INSTALL_TREE."/lib", $aFile);

        $aFile[0]="*.dic";
        CopyFiles( $oldTree, INSTALL_TREE."/lib/DECtalk", $aFile);

        $aFile[0]="say";
        CopyFiles( $oldTree, INSTALL_TREE."/bin", $aFile);

        $aFile[0]="*.h";
        CopyFiles( $oldTree, INSTALL_TREE."/include/dtk", $aFile);

        $aFile[0]="DECtalk.conf";
        CopyFiles( $oldTree, RAMDISK."/dtk", $aFile);

        // Copying the optional tts if present
        $anOptionalDirectory=0;
        for ($i=1; $i<count($this->_myTTSIdentifier); $i++)
        {
          $oldTree=$this->_myStorageDirectory."/".$this->_myTTSIdentifier[$i];
	  Debug("Is dir?");
	  Debug($oldTree);
          if (@is_dir($oldTree))
          {
            $anOptionalDirectory=$i;
            break;
          }
        }

        if ($anOptionalDirectory)
        {
          unset($aFile);
          $aFile[0]="libtts*.so";
          CopyFiles( $oldTree, INSTALL_TREE."/lib", $aFile);
          
          $aFile[0]="*.dic";
          CopyFiles( $oldTree, INSTALL_TREE."/lib/DECtalk", $aFile);

          // Updating the Makefile (emacspeak) so that the optional library is used
          $aCommand="sed 's+ltts_us+ltts_us -l".$this->_myDECtalkLibrary[$anOptionalDirectory]."+g' ".DTK_EMACSPEAK."/Makefile";

	  //replace 'ltts_us' 'ltts_us -l".$this->_myDECtalkLibrary[$anOptionalDirectory]."' -- ".DTK_EMACSPEAK."/Makefile";

          Debug($aCommand);
          system($aCommand);

          $this->myTextToSpeechLanguage[1] = $this->_myTTSIdentifier[ $anOptionalDirectory ];
        }
      }
      else
      {
        // The BASE_DIR variable must be customized
        $oldTree="/usr/local";

	$aCommand="sed 's+BASE_DIR=\"$oldTree\"+BASE_DIR=\"".INSTALL_TREE."\"+g' ".RAMDISK."/DECtalk/files.sh";

//         $aCommand="replace".
//           " 'BASE_DIR=\"$oldTree\"'".
//           " 'BASE_DIR=\"".INSTALL_TREE."\"'".
//           " -- ".
//           RAMDISK."/DECtalk/files.sh";
        Debug($aCommand);
        system($aCommand);

        // Idem for an optional package if present
        $anOptionalPackageIsPresent=0;
	  Debug("count _myTTSIdentifier=".count($this->_myTTSIdentifier));
        for ($i=1; $i<count($this->_myTTSIdentifier); $i++)
        {
	  Debug("Is dir?");
	  Debug(RAMDISK."/".$this->_myTTSIdentifier[$i]);
          if (@is_dir(RAMDISK."/".$this->_myTTSIdentifier[$i]))
          {
            $anOptionalPackageIsPresent=$i;
            break;
          }
        }

        if ($anOptionalPackageIsPresent)
        {
	  $aCommand="sed 's+BASE_DIR=\"$oldTree\"+BASE_DIR=\"".INSTALL_TREE."\"+g' ".RAMDISK."/".$this->_myTTSIdentifier[$anOptionalPackageIsPresent]."/installer";

//           $aCommand="replace".
//             " 'BASE_DIR=\"$oldTree\"'".
//             " 'BASE_DIR=\"".INSTALL_TREE."\"'".
//             " -- ".
//             RAMDISK."/".$this->_myTTSIdentifier[$anOptionalPackageIsPresent]."/installer";

          Debug($aCommand);
          system($aCommand);
          $this->myTextToSpeechLanguage[1]=$this->_myTTSIdentifier[$anOptionalPackageIsPresent];
        }

        $this->myStatus=SERIAL_NUMBER_IS_EXPECTED;
      }
    }

  // Private methods
// _LookForMyDirectory
// A short method which looks for the DECtalk tarballs and directories
//
// It supposes that:
// - the DECtalk tarballs are named linux...tar.gz
// - the main DECtalk directory is named "DECtalk"
// - the other directories concerned optional languages (french,...)
//
// Input:
// $thePath: The path of the directory where the packages might be found
//
// Output:
// $theTarBalls : an array of tarballs.
// $theOptionalDirectory: an array of the directories for the optional languages
// e.g: 0 => french
//
// Returned value:
// true if the DECtalk directory is found
//
  function _LookForMyDirectory($thePath, &$theTarBalls, &$theOptionalDirectory)
    {
      ENTER("_LookForMyDirectory");
      Debug("Input: $thePath");
      $isDECtalkFound=FALSE;
      $aDirectoryPointer=opendir($thePath);  
      while($aFilename=readdir($aDirectoryPointer))
      {
        if (@is_dir("$thePath/$aFilename"))
        {
          if ($aFilename=="DECtalk")
          {
            Debug("Dir = DECtalk");
            $isDECtalkFound=TRUE;
          }
          else if (($aFilename!=".")
                   &&($aFilename!="..")
                   &&($aFilename!="emacspeak")
                   &&($aFilename!="oralux")
                   &&($aFilename!="dtk")
                   &&($aFilename!="usr")
                   &&($aFilename!="home")
                   &&($aFilename!="var"))
          { // This might be a directory related to an optional language
            Debug("Optional Dir = $aFilename");
            $theOptionalDirectory[]=$aFilename;
          }
        }
	else if (preg_match("/[lL][iI][nN][uU][xX].*.[gG][zZ]/",$aFilename,$b))
        { // Yes, this looks like a DECtalk tarball!
          Debug("tarball=$aFilename");
          $theTarBalls[]=$aFilename;
        }
      }
      closedir($aDirectoryPointer);

      Debug("theTarBalls=");
      Debug($theTarBalls);
      Debug("theOptionalDirectory=");
      Debug($theOptionalDirectory);

      return $isDECtalkFound;
    }
}


?>