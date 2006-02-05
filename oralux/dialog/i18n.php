<?php
// ----------------------------------------------------------------------------
// i18n.php
// $Id: i18n.php,v 1.3 2006/02/05 21:03:37 gcasse Exp $
// $Author: gcasse $
// Description: set domain
// $Date: 2006/02/05 21:03:37 $ |
// $Revision: 1.3 $ |
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

setlocale(LC_ALL, getenv("LANG")); 
bindtextdomain('oralux', '/usr/share/oralux/locale');
textdomain('oralux');                           

?>