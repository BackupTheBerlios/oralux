<?php
// ----------------------------------------------------------------------------
// emacsConfig.php
// $Id: emacsConfig.php,v 1.4 2004/11/12 21:46:13 gcasse Exp $
// $Author: gcasse $
// Description: Emacs settings (php5)
// $Date: 2004/11/12 21:46:13 $ |
// $Revision: 1.4 $ |
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

class emacsConfig
{
  protected $_myFilename;
  protected $_myPathname;
  protected $_myTempFilename;
  protected $_myMailConfig;

  function emacsConfig( & $theMailConfig)
    {
      $this->_myMailConfig= & $theMailConfig;
      $this->_myPathname="/home/".$theMailConfig->getUser();
      $this->_myFilename=$this->_myPathname."/.emacs";
      $this->_myTempFilename=$this->_myPathname."/oralux_emacs".rand(1,9999);
    }

  function save()
    {
      $aSource=fopen($this->_myFilename,"r");
      if ($aSource==NULL)
	{
	  $aError=sprintf("Error: can't open file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.4 $');
	  return FALSE;
	}

      $aDestination=fopen($this->_myTempFilename,"w");
      if ($aDestination==NULL)
	{
	  $aError=sprintf("Error: can't open file: %s\n", $this->_myTempFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.4 $');
	  return FALSE;
	}
      
      while (!feof ($aSource))
	{
	  $aLine=fgets($aSource);

	  $aTemplatePattern="/[ \t]*\([ \t]*setq[ \t]+%s[ \t]+\"[^\"]*\"[ \t]*\)[ \t]*/";
	  $aTemplateReplacement="(setq %s \"%s\")";
	  
	  $aPattern[0]=sprintf( $aTemplatePattern, "user-mail-address");
	  $aReplacement[0]=sprintf( $aTemplateReplacement, "user-mail-address", $this->_myMailConfig->getValue( "email"));
	  
	  $aPattern[1]=sprintf($aTemplatePattern, "user-full-name");
	  $aReplacement[1]=sprintf($aTemplateReplacement,  "user-full-name", $this->_myMailConfig->getValue( "name"));
	  
	  $aResult=preg_replace($aPattern, $aReplacement, $aLine);

	  fprintf($aDestination, $aResult);
	}

      if ($aSource==NULL)
	{
	  $aError=sprintf("Error: can't open file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.4 $');
	  return FALSE;
	}

      fclose($aDestination);
      fclose($aSource);
      
      if ((chown ( $this->_myTempFilename, $this->_myMailConfig->getUser())==FALSE)
	  || (chgrp ( $this->_myTempFilename, $this->_myMailConfig->getUser())==FALSE)
	  || (chmod ( $this->_myTempFilename, 0600)==FALSE)
	  || (rename($this->_myTempFilename, $this->_myFilename)==FALSE)
	  || (chmod ( $this->_myFilename, 0600)==FALSE)
	  || (chown ( $this->_myFilename, $this->_myMailConfig->getUser())==FALSE)
	  || (chgrp ( $this->_myFilename, $this->_myMailConfig->getUser())==FALSE))
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myTempFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.4 $');
	  return FALSE;
	}
      return TRUE;
    }
}

?>