<?PHP
// ----------------------------------------------------------------------------
// demopwd.php
// Copyright (C) 2001, 2002 Gilles Casse (gcasse@helis.org)
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

// This file is useful for demo mode (if the user is anonymous)
// - Put this file in your include directory so that users can not read it...
// - Update "demo_user", "demo_pwd",... according to your demo environment

define ("DEMO_USER", "helis");
define ("DEMO_PWD", "gsx750");
define ("DEMO_SERVER", "db.tf-data.net");
define ("DEMO_BASE", "Helis");
define ("WITH_SESSION", TRUE);

?>
