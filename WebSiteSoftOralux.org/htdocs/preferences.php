<?PHP
// ----------------------------------------------------------------------------
// preferences.php
// $Id: preferences.php,v 1.1 2005/05/14 01:32:06 gcasse Exp $
// $Author: gcasse $
// Description: User's preferences
// $Date: 2005/05/14 01:32:06 $ |
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

// if the user select a language, then mylang will be found in the query string 
// e.g. preferences.php?mylang=fr
session_start(); // first line, because it sends the http header
if( isset($_POST["mylang"]))
{
  // e.g. if the posted variable mylang = en
  // then "en" is stored in the session variable $mylang 
  $_SESSION['mylang']=$_POST["mylang"];
}

// To be placed after the previous lines
require_once("include/webpage.php");

$aWebPage=new webPage("Preferences", "$Date: 2005/05/14 01:32:06 $");

// Printing the web page
$aWebPage->printTag("htm/preferences.htm");

?>
