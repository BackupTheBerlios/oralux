#! /bin/sh
# ----------------------------------------------------------------------------
# tcl
# $Id: tcl.sh,v 1.1 2004/09/27 20:29:59 gcasse Exp $
# $Author: gcasse $
# Description: Installing TCL
# $Date: 2004/09/27 20:29:59 $ |
# $Revision: 1.1 $ |
# Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
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

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install tcl8.3-dev 
    apt-get install tclx8.3-dev 

#    cd /usr/local/lib
#    ln -s libtcl?.?.so libtcl.so
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD apt-get install tcl8.3-dev 
    chroot $BUILD apt-get install tclx8.3-dev 

#    cd $BUILD/usr/local/lib
#    cp -p /usr/local/lib/libtcl?.?.so libtcl.so
#    cp -p /usr/local/lib/libtclstub?.?.a .
}

case $1 in
    i|I)
        InstallPackage
        ;;
    b|B)
        Copy2Oralux
        ;;
    *)
        echo "I (install) or B(new tree) is expected"
        ;;
esac


exit 0
