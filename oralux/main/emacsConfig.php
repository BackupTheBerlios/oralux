<?php
// ----------------------------------------------------------------------------
// emacsConfig.php
// $Id: emacsConfig.php,v 1.1 2004/09/27 20:30:30 gcasse Exp $
// $Author: gcasse $
// Description: Emacs settings (php5)
// $Date: 2004/09/27 20:30:30 $ |
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
      $aDestination=fopen($this->_myTempFilename,"w");
      
      while ($aLine=fgets($aSource))
	{
	  $aTemplatePattern="/[ \t]*\([ \t]*setq[ \t]+%s[ \t]+\"[^\"]*\"[ \t]*\)[ \t]*/";
	  $aTemplateReplacement="(setq %s \"%s\")";

	  $aPattern[0]=sprintf( $aTemplatePattern, "user-mail-address");
	  $aReplacement[0]=sprintf( $aTemplateReplacement,  "user-mail-address", $this->_myMailConfig->getValue( "email"));

	  $aPattern[1]=sprintf($aTemplatePattern, "user-full-name");
	  $aReplacement[1]=sprintf($aTemplateReplacement,  "user-full-name", $this->_myMailConfig->getValue( "name"));

  	  $aResult=preg_replace($aPattern, $aReplacement, $aLine);
	  fprintf($aDestination, $aResult);
	}

      fclose($aSource);
      fclose($aDestination);
      chown ( $this->_myTempFilename, $this->_myMailConfig->getUser());
      chgrp ( $this->_myTempFilename, $this->_myMailConfig->getUser());
      chmod ( $this->_myTempFilename, 0600);

      rename($this->_myTempFilename, $this->_myFilename);
      chmod ( $this->_myFilename, 0600);
      chown ( $this->_myFilename, $this->_myMailConfig->getUser());
      chgrp ( $this->_myFilename, $this->_myMailConfig->getUser());

    }
}

?>