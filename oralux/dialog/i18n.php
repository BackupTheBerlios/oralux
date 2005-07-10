#!/usr/bin/php
<?php
// ----------------------------------------------------------------------------
// dialog.php
// $Id: i18n.php,v 1.2 2005/07/10 20:41:19 gcasse Exp $
// $Author: gcasse $
// Description: command line based dialog (menu, yes/no question, dialog box,...)
// $Date: 2005/07/10 20:41:19 $ |
// $Revision: 1.2 $ |
// Copyright (C) 2004, 2005 Gilles Casse (gcasse@oralux.org)
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

setlocale(LC_MESSAGES, getenv("ORALUXTTSLANG")); 
// setlocale(LC_MESSAGES, getenv("LANG"));
bindtextdomain('oralux', '/usr/share/oralux/locale');
textdomain('oralux');                           

?>