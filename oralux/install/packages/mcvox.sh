#! /bin/sh
# ----------------------------------------------------------------------------
# mcvox.sh
# $Id: mcvox.sh,v 1.6 2006/03/05 18:28:57 gcasse Exp $
# $Author: gcasse $
# Description: Installing mcvox
# $Date: 2006/03/05 18:28:57 $ |
# $Revision: 1.6 $ |
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
export MCVOX_RELEASE=0.5
export MCVOX=mcvox-$MCVOX_RELEASE.tgz

cd $ARCHDIR
if [ ! -e $ARCHDIR/$MCVOX ]
    then
    echo "Downloading $MCVOX"
    wget http://soft.oralux.org/download/mcvox-$MCVOX_RELEASE.tgz
fi

####
# Installing the package in the current tree
InstallPackage()
{
#    apt-get remove --purge mc
    cd /tmp
    rm -rf mcvox*
    cp $ARCHDIR/$MCVOX .
    tar -zxvf $ARCHDIR/$MCVOX
    cd mcvox*
    ./configure --prefix=/usr --with-screen=ncurses
    make
    make install
#    cp $INSTALL_PACKAGES/mcvox/menu /usr/share/mcvox/mc.menu 
    install -m 644 $INSTALL_PACKAGES/mcvox/mc.ext /etc/mc/mc.ext
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    cd $BUILD/tmp
    rm -rf mcvox*
    cp $ARCHDIR/$MCVOX .

    chroot $BUILD  bash -c "\
    cd /tmp;\
    tar -zxvf mcvox*;\
    cd mcvox*;\
    ./configure --prefix=/usr --with-screen=ncurses;\
    make;\
    make install"
    install -m 644 $INSTALL_PACKAGES/mcvox/mc.ext $BUILD/etc/mc/mc.ext
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
