<?php
// ----------------------------------------------------------------------------
// minimenu.php
// $Id: minimenu.php,v 1.2 2004/12/03 22:59:28 gcasse Exp $
// $Author: gcasse $
// Description: menu to easily launch and manage text-based programs.
// $Date: 2004/12/03 22:59:28 $ |
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

require_once("../main/helpEmul.php");
require_once("../dialog/cliDialog.php");
require_once("../dialog/terminal.php");
require_once("config.php");

// {{{ class element
class minimenu
{
  public function __construct( & $theArray)
    {
      ENTER("minimenu::minimenu",__LINE__);

      $a=new config("gilles", $anElement);

      unset($anOption);
      foreach ($anElement as $a)
	{
	  switch( $a->getType())
	    {
	    case MINIMENU_CATEGORY:
	      $anOption[]="+ ".$a->name;
	      break;
	    case MINIMENU_PROGRAM:
	      $anOption[]="* ".$a->name;
	      break;
	    case MINIMENU_DOCUMENT:
	      $anOption[]=$a->name;
	      break;	      
	    }
	} 


      $this->_myTerminal=new enhancedTerminal();
      if (!$this->_myTerminal->isBuild())
	{
	  $this->_myTerminal=new dumbTerminal($theTerminal);
	}

      $this->myDialog=new cliDialog($this->_myTerminal, false);
      $aTitle="";
      $theText=NULL; 
      $theSelectedOption=NULL; 
      $theKeyIsDisplayed=false;
      $this->myDialog->menu($aTitle, $anOption, & $aResult, 
			    NULL, // no text
			    NULL, // no selected option
			    false // the keys are not displayed (no number for example before the label).
			    );
    }
};
// }}}

$a=new minimenu();

?>