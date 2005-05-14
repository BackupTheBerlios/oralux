<?PHP
// ----------------------------------------------------------------------------
// nav.php
// $Id: nav.php,v 1.1 2005/05/14 01:32:08 gcasse Exp $
// $Author: gcasse $
// Description: Printing the navigation bar
// This is useful for the navigation bar. It supplies a link to go from page to page. But it doesn't supply a link for the current page.
// 
// $Date: 2005/05/14 01:32:08 $ |
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

require_once("tools.php");
require_once(GetTranslatedFile("htm/title.htm"));

// If you have to add a new page, please read skeleton.php 
// If you come from skeleton.php, just update the definition of the _myEntries array (in english please).

class nav
{
  var $_myCurrentEntry="";
  var $_myEntries=array("mcvox"=>"index.php",
                        "Preferences"=>"preferences.php",
                        );

  var $_myStartAnchorTag=null;
  var $_myStopAnchorTag=null;

// Input variables:$theEntry. Expected value: Home, FAQ, Download
  function nav($theEntry)
    {
      $this->_myCurrentEntry=$theEntry;

// No link for the current entry
      $this->_myStartAnchorTag[$theEntry]="";
      $this->_myFinalAnchorTag[$theEntry]="";

// Supplying the links for the navigation bar
      foreach($this->_myEntries as $anEntry=>$aPage)
        {
          if ($anEntry!=$theEntry)
          {
            // For the other entries, the link is required
            $this->_myStartAnchorTag[$anEntry]="<a href=\"$aPage\">";
            $this->_myFinalAnchorTag[$anEntry]="</a>";
          }
        }
    }

// theEntry indicates which page is displayed
  function printEntry($theEntry, $theTranslation)
    {
      return ($this->_myStartAnchorTag[$theEntry].
            $theTranslation.
            $this->_myFinalAnchorTag[$theEntry]);
    }
  
  function printTag()
    {
      global $TheTitleTranslation;
      include(GetTranslatedFile("htm/nav.htm"));
    }

} // class nav


?>
