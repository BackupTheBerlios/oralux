<?PHP
// ----------------------------------------------------------------------------
// header.php
// $Id: header.php,v 1.1 2004/09/28 21:48:44 gcasse Exp $
// $Author: gcasse $
// Description: This is the header of the web page: the horizontal banner and the navigation bar.
// TheEntry allows to know which page is displayed
// 
// $Date: 2004/09/28 21:48:44 $ |
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

require_once("nav.php");
require_once("tools.php");

// This is useful for the navigation bar. It supplies a link to go from page to page. But it doesn't supply a link for the current page.
class header
{
  var $_myNav=null;
  var $_myEntry="";

// Input variables:$theEntry. Expected value: Home, FAQ, Download
  function header($theEntry)
    {
      $this->_myEntry=$theEntry;
      $this->_myNav=new nav($theEntry);
    }

 function printTag()
   {
     // The Oralux logo is a link towards the home page.
     // This is true for any page except the homepage.
     $START_ANCHOR_TAG="";
     $END_ANCHOR_TAG="";
     if ($this->_myEntry!="Home")
     {
       $START_ANCHOR_TAG="<a href=\"index.php\">";
       $END_ANCHOR_TAG="</a>";
     }
     include(GetTranslatedFile("htm/header.htm"));
     $this->_myNav->printTag();
   } // printTag

} // class header

?>
