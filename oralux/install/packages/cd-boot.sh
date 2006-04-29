#! /bin/sh
# ----------------------------------------------------------------------------
# cdboot.sh
# $Id: cd-boot.sh,v 1.1 2006/04/29 14:45:48 gcasse Exp $
# $Author: gcasse $
# Description: Installing stuff in the CD.
# $Date: 2006/04/29 14:45:48 $ |
# $Revision: 1.1 $ |
# Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# ----------------------------------------------------------------------------
####
source ../oralux.conf

export CDBOOT_SRC="$INSTALL_PACKAGES/cd-boot"
export TOOLS="$NEW_ORALUX/tools"

install -d "$TOOLS/cd-boot"

cp -R $CDBOOT_SRC $TOOLS
chown -R root.root $TOOLS
