<?PHP
// ----------------------------------------------------------------------------
// footer.php
// $Id: footer.php,v 1.1 2004/09/28 21:48:43 gcasse Exp $
// $Author: gcasse $
// Description: This is the footer of the web page: the navigation bar and the legal banner.
// TheEntry allows to know which page is displayed
// 
// $Date: 2004/09/28 21:48:43 $ |
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
class footer
{
  var $_myNav=null;

// Input variables:$theEntry. Expected value: Home, FAQ, Download
  function footer($theEntry)
    {
      $this->_myNav=new nav($theEntry);
    }

 function printTag()
   {
     include(GetTranslatedFile("htm/footer.htm"));
   } // printTag

} // class header


?>
