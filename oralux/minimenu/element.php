<?php
// ----------------------------------------------------------------------------
// element.php
// $Id: element.php,v 1.1 2004/11/28 21:21:00 gcasse Exp $
// $Author: gcasse $
// Description: Defines the elements of the minimenu: category, program, document.
// $Date: 2004/11/28 21:21:00 $ |
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

define ("MINIMENU_CATEGORY","category");
define ("MINIMENU_PROGRAM","program");
define ("MINIMENU_DOCUMENT","document");

require_once("../main/helpEmul.php");

// {{{ class element
abstract class element
{
  abstract public function getType();
  public $name;
  public $comment;
  public $hidden=0;

  protected $myFields = array("name", "comment", "hidden");

  public function __construct( & $theArray)
    {
      ENTER("element::element",__LINE__);

      foreach ($theArray as $aKey=>$aValue)
	{
	  if (array_search($aKey, $this->myFields) !== false)
	    {
	      $this->$aKey=$aValue;
	    }
	}
    }
};
// }}}
// {{{ class category
class category extends element
{
  public function __construct( & $theArray)
    {
      ENTER("category::category",__LINE__);
      parent::__construct( $theArray);
    }

  public function getType()
    { 
      ENTER("category::getType",__LINE__);
      return MINIMENU_CATEGORY;
    } 
};
// }}}
// {{{ class program
class program extends element
{
  public $command;
  protected $myOwnFields = array("command");

  public function __construct( & $theArray)
    {
      ENTER("program::program",__LINE__);

      foreach ($theArray as $aKey=>$aValue)
	{
	  if (array_search($aKey, $this->myOwnFields) !== false)
	    {
	      $this->$aKey=$aValue;
	    }
	}

      parent::__construct( $theArray);
    }

  public function getType()
    { 
      ENTER("program::getType",__LINE__);
      return MINIMENU_PROGRAM;
    } 
};
// }}}
// {{{ class document
class document extends element
{
  public $uri;
  protected $myOwnFields = array("uri");

  public function __construct( & $theArray)
    {
      ENTER("document::document",__LINE__);

      foreach ($theArray as $aKey=>$aValue)
	{
	  if (array_search($aKey, $this->myOwnFields) !== false)
	    {
	      $this->$aKey=$aValue;
	    }
	}

      parent::__construct( $theArray);
    }

  public function getType()
    {
      ENTER("document::getType",__LINE__);
      return MINIMENU_DOCUMENT;
    } 
};
// }}}

?>