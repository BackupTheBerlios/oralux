<?php
// ----------------------------------------------------------------------------
// vmConfig.php
// $Id: vmConfig.php,v 1.2 2004/11/07 21:19:14 gcasse Exp $
// $Author: gcasse $
// Description: VM settings (php5)
// $Date: 2004/11/07 21:19:14 $ |
// $Revision: 1.2 $ |
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

require_once("mailConfig.php");

class vmConfig
{
  protected $_myFilename;
  protected $_myPathname;
  protected $_myTempFilename;
  protected $_myMailConfig;
  protected $_myBeginInsertionRegexp="/[ \t]*;[ \t]*BeginOraluxInsertion.*/";
  protected $_myBeginInsertionMark=";BeginOraluxInsertion";
  protected $_myEndInsertionRegexp="/[ \t]*;[ \t]*EndOraluxInsertion.*/";
  protected $_myEndInsertionMark=";EndOraluxInsertion";

  function vmConfig( & $theMailConfig)
    {
      $this->_myMailConfig= & $theMailConfig;
      $this->_myPathname="/home/".$theMailConfig->getUser();
      $this->_myFilename=$this->_myPathname."/.vm";
      $this->_myTempFilename=$this->_myPathname."/oralux_vm".rand(1,9999);
      if (touch($this->_myTempFilename)==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myTempFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}
      if (chmod ( $this->_myTempFilename, 0600)==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myTempFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}
      if (chown ( $this->_myTempFilename, $this->_myMailConfig->getUser())==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myTempFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}
      if (chgrp ( $this->_myTempFilename, $this->_myMailConfig->getUser())==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myTempFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}
    }

  // input: file descriptors
  protected function _copyUntilMark( $theRegexpMark, $theSource, $theDestination=NULL )
    {
      $aMarkIsExpected=true;

      while( $aMarkIsExpected && !feof( $theSource))
	{
	  $aLine=fgets($theSource);
	  
	  if (($theRegexpMark!=NULL) && preg_match( $theRegexpMark, $aLine, $aResult))
	    {
	      $aMarkIsExpected=false;
	    }
	  else if ($theDestination)
	    {
	      fprintf( $theDestination, $aLine);
	    }
	}
    }

  protected function _copyNewData( & $theFileDescriptor)
    {
      fprintf($theFileDescriptor, "(setq vm-url-browser 'w3m-browse-url)\n");
      fprintf($theFileDescriptor, "(setq vm-reply-subject-prefix \"Re: \")\n");
      fprintf($theFileDescriptor, "(setq vm-folder-directory \"~/mail/\")\n");
      fprintf($theFileDescriptor, 
	      sprintf("(setq vm-auto-get-new-mail %d)\n", $this->_myMailConfig->getValue("delay")));

      if ($this->_myMailConfig->getNumberOfMailSource())
	{
	  fprintf($theFileDescriptor, "(setq vm-spool-files '(\n");
	  $anArray=array();
	  $aIdentifier=0;
	  $this->_myMailConfig->initMailSourceIndex();
	  $aKeepToServerIsSelected=false;
	  while ($this->_myMailConfig->getNextMailSource( $anArray, $aIdentifier))
	    {
	      // port vide => 110
	      fprintf($theFileDescriptor, 
		      "\"pop:%s:%s:pass:%s:%s\"\n",
		      $anArray["host"],
		      $anArray["port"],
		      $anArray["login"],
		      $anArray["password"]
		      );
	      if ($anArray["keep"])
		{
		  $aKeepToServerIsSelected=true;
		}
	    }
	  fprintf($theFileDescriptor, "))\n");

	  if ($aKeepToServerIsSelected)
	    {
	      unset($anArray);
	      fprintf($theFileDescriptor, "(setq vm-pop-auto-expunge-alist '(\n");
	      $anArray=array();
	      $aIdentifier=0;
	      $this->_myMailConfig->initMailSourceIndex();
	      
	      while ($this->_myMailConfig->getNextMailSource( $anArray, $aIdentifier))
		{
		  if ($anArray["keep"])
		    {
		      fprintf($theFileDescriptor, 
			      "(\"%s:%s:pass:%s:*\" . nil)\n",
			      $anArray["host"],
			      $anArray["port"],
			      $anArray["login"]
			      );
		    }
		}
	      fprintf($theFileDescriptor, "))\n");
	    }
	}

      // Russian stuff
      fprintf($theFileDescriptor, "(cond ((string-match \"ru\" (getenv \"LANGUAGE\"))\n");
      fprintf($theFileDescriptor, "(print \"Russian environment\")\n");
      fprintf($theFileDescriptor, ";; Begin Vm Russian stuff\n");
      fprintf($theFileDescriptor, "(setq vm-mime-8bit-text-transfer-encoding '8bit)\n");
      fprintf($theFileDescriptor, "(setq vm-mime-8bit-composition-charset \"koi8-r\")\n");
      fprintf($theFileDescriptor, ";; Map the MULE charset to MIME charset\n");
      fprintf($theFileDescriptor, "(add-to-list 'vm-mime-mule-charset-to-charset-alist\n");
      fprintf($theFileDescriptor, "'(cyrillic-iso8859-5 \"koi8-r\"))\n");
      fprintf($theFileDescriptor, "(add-to-list 'vm-mime-mule-charset-to-coding-alist\n");
      fprintf($theFileDescriptor, "'(\"koi8-r\" cyrillic-koi8))\n");
      fprintf($theFileDescriptor, "(add-to-list 'vm-mime-mule-charset-to-coding-alist\n");
      fprintf($theFileDescriptor, "'(\"windows-1251\" cp1251))\n");
      fprintf($theFileDescriptor, "(add-to-list 'vm-mime-mule-coding-to-charset-alist\n");
      fprintf($theFileDescriptor, "'(cyrillic-koi8 \"koi8-r\"))\n");
      fprintf($theFileDescriptor, "(add-to-list 'vm-mime-mule-coding-to-charset-alist\n");
      fprintf($theFileDescriptor, "'(cp1251 \"windows-1251\"))\n");
      fprintf($theFileDescriptor, "(add-to-list 'vm-mime-mule-coding-to-charset-alist\n");
      fprintf($theFileDescriptor, "'(windows-1251 \"windows-1251\"))\n");
      fprintf($theFileDescriptor, ";; end of Vm Russian stuff.\n");
      fprintf($theFileDescriptor, "))\n");

      // Personal crisis
      fprintf($theFileDescriptor, "(load \"vm-pcrisis\")\n");
    }

  protected function _putMark( $theFileDescriptor, $theMark)
    {
      fprintf( $theFileDescriptor, "%s\n", $theMark);
    }

  protected function _putEndInsertionMark( $theFileDescriptor)
    {
      fprintf($theFileDescriptor, "%s\n", $this->_myEndInsertionMark);
    }


  function save()
    {
      if (!is_file($this->_myFilename))
	{
	  if (touch($this->_myFilename)==FALSE)
	    {
	      $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	      ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	      return;
	    }
	}

      $aSource=fopen($this->_myFilename,"r");
      if ($aSource==NULL)
	{
	  $aError=sprintf("Error: can't open file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}

      $aDestination=fopen($this->_myTempFilename,"w");
      
      if ($aDestination==NULL)
	{
	  $aError=sprintf("Error: can't open file: %s\n", $this->_myTempFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}

      // Copy the original data until the previously inserted area:
      $this->_copyUntilMark( $this->_myBeginInsertionRegexp, $aSource, $aDestination);
      $this->_putMark( $aDestination, $this->_myBeginInsertionMark);

      $this->_copyNewdata( $aDestination);
	  
      // Filter the previously inserted data
      $this->_copyUntilMark( $this->_myEndInsertionRegexp, $aSource);
      $this->_putMark( $aDestination, $this->_myEndInsertionMark);
	  
      // Copy the original remaining data
      $this->_copyUntilMark( NULL, $aSource, $aDestination);
	  
      fclose($aSource);
      fclose($aDestination);

      if (rename($this->_myTempFilename, $this->_myFilename)==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}

      if (chmod ( $this->_myFilename, 0600)==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}

      if (chown ( $this->_myFilename, $this->_myMailConfig->getUser())==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}

      if (chgrp ( $this->_myFilename, $this->_myMailConfig->getUser())==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  return;
	}
    }
}

?>