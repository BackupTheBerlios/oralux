#! /bin/sh
# ----------------------------------------------------------------------------
# yasr.sh
# $Id: yasr.sh,v 1.2 2005/01/30 21:43:51 gcasse Exp $
# $Author: gcasse $
# Description: Installing BRLTTY
# $Date: 2005/01/30 21:43:51 $ |
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
export YASR_RELEASE=0.6.5

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get --purge remove yasr
    rm -rf /etc/yasr
    cd /tmp
    rm -rf yasr*
    wget http://ovh.dl.sourceforge.net/sourceforge/yasr/yasr-$YASR_RELEASE.tar.gz
    tar -zxvf yasr*
    cd yasr*
    patch -p0 yasr/main.c < $INSTALL_PACKAGES/yasr/main.c.patch
    ./configure --prefix=/usr
    make
    make install

    rm -rf /tmp/yasr*
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    export INSTALL_PACKAGES=/usr/share/oralux/install/packages

    chroot $BUILD  bash -c "\
    apt-get --purge remove yasr;\
    rm -rf /etc/yasr;\
    cd /tmp;\
    rm -rf yasr*;\
    wget http://ovh.dl.sourceforge.net/sourceforge/yasr/yasr-$YASR_RELEASE.tar.gz;\
    tar -zxvf yasr*;\
    cd yasr*;\
    patch -p0 yasr/main.c < $INSTALL_PACKAGES/yasr/main.c.patch;\
    ./configure --prefix=/usr;\
    make;\
    make install"
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
