<?PHP
// ----------------------------------------------------------------------------
// skeleton.php
// $Id: skeleton.php,v 1.1 2004/09/28 21:48:39 gcasse Exp $
// $Author: gcasse $
// Description: How creating a new web page
// $Date: 2004/09/28 21:48:39 $ |
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

require_once("include/webpage.php");

// Adding a new page in 4 stages

// ###
// 1. First of all, chose a title (in english) for the new page, just one word if possible. The current titles are today (May 2003): Home, FAQ, Download.
// Place your title in the following line:

$aWebPage=new webPage("titleOfMyPage", "$Date: 2004/09/28 21:48:39 $");

// if you need to add a new stylesheet - say newStyleSheet.css - please, place it in the directory "styles"
// You will replace the previous php line by:
// $aStyle[0]="newStyleSheet.css";
// $aWebPage=new webPage("titleOfMyPage", "$Date: 2004/09/28 21:48:39 $", $aStyle);

// ###
// 2. Updating the title of the page and the navigation bar
// Update the file htm/title.htm with your new title. Exactly the same title that you previously supplied in stage 1
// If possible, inform the translaters so that e.g. htm/title-fr.htm be updated too.

// ###
// 3. The content (the information) of your page is included in a file written in xhtml
// Well, this is not a complete html file (with doctype, head, body) since the structure is built by the webpage class.
// The html files are stored in directory htm 
// You will find as example 
// - A one row layout: htm/skeleton1.htm
// - A two rows layout: the first div (main) is on the left and takes 70% of the width, the second div is named (sidebar).

// ###
// 4. Update file include/nav.php for navigation.


// Printing the web page
$aWebPage->printTag("htm/new_XHTML_File.htm");

?>
