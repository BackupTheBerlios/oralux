<?PHP
// ----------------------------------------------------------------------------
// head.php
// $Id: head.php,v 1.2 2005/08/14 23:35:22 gcasse Exp $
// $Author: gcasse $
// Description: The content of the html head
// $Date: 2005/08/14 23:35:22 $ |
// $Revision: 1.2 $ |
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

class head
{
  // _myTitle
  // This is the title of the web page. It is also the entry in the navigation bar
  // Type: String. 
  var $_myTitle=""; 

  // _myStyles
  // The style sheets (e.g. "file.css")
  // Type: array of strings
  var $_myStyles=null;

  var $_myCharset=null;

  function head($theTitle, $theDate, $theCharset, $theStyles=null)
    {
      global $TheTitleTranslation;
      global $TheSubTitleTranslation;
      
      // Is it a title from the menu (as "Home", "FAQ") or a 'sub menu' (as "Accessibility")?

      if (isset($TheTitleTranslation[$theTitle]))
      {
        $aTitle=$TheTitleTranslation[$theTitle];
      }
      else if (isset($TheSubTitleTranslation[$theTitle]))
      {
        $aTitle=$TheSubTitleTranslation[$theTitle];
      }
      else
      {
        die("Unexpected title ($theTitle)!");
      }
      $this->_myTitle=$aTitle;
      $this->_myStyles=$theStyles;
      $this->_myDate=$theDate;
      $this->_myCharset=$theCharset;
    }

  function printTag()
    {
      $TITLE=$this->_myTitle;
      $DATE=$this->_myDate;
      $CHARSET=$this->_myCharset;
      include(GetTranslatedFile("htm/head.htm"));

      // Adding the supplied style sheets
      if (null!=$this->_myStyles)
      {
        foreach($this->_myStyles as $STYLE)
          {
            include("htm/head2.htm"); // No translated file for head2
          }
      }
    } // printTag
} // class head

?>

