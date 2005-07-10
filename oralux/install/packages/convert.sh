#! /bin/sh
# ----------------------------------------------------------------------------
# convert.sh
# $Id: convert.sh,v 1.2 2005/07/10 20:41:19 gcasse Exp $
# $Author: gcasse $
# Description: Installing format converters
# $Date: 2005/07/10 20:41:19 $ |
# $Revision: 1.2 $ |
# Copyright (C) 2004, 2005 Gilles Casse (gcasse@oralux.org)
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
set -vx

export INSTALL_PACKAGES=/usr/share/oralux/install/packages

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install pstotext

    cd /usr/bin
    for i in `ls $INSTALL_PACKAGES/convert/*.sh`; do 
	ln -s $i .
    done
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD  bash -c '\
    apt-get install pstotext;\
    cd /usr/bin;\
    for i in `ls $INSTALL_PACKAGES/convert/*.sh`; do \
	ln -s $i . ;\
    done'
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
