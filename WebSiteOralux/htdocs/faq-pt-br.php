<?PHP
// ----------------------------------------------------------------------------
// faq.php
// $Id: faq-pt-br.php,v 1.1 2006/05/06 20:59:10 gcasse Exp $
// $Author: gcasse $
// Description: Frequently asked questions
// $Date: 2006/05/06 20:59:10 $ |
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
require_once("include/tools.php");

$aStyle[0]="faq.css";
$aWebPage=new webPage("FAQ", "$Date: 2006/05/06 20:59:10 $", $aStyle, "pt-br");

// Printing the web page
$aWebPage->printTag("htm/faq.htm");

?>
