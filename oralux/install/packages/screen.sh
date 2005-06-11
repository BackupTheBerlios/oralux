#! /bin/sh
# ----------------------------------------------------------------------------
# screen.sh
# $Id: screen.sh,v 1.1 2005/06/11 22:48:37 gcasse Exp $
# $Author: gcasse $
# Description: Installing Screen
# $Date: 2005/06/11 22:48:37 $ |
# $Revision: 1.1 $ |
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
export SCREEN_RELEASE=0.4.2

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get --purge remove screen
    cd /tmp
    rm -rf screen*
    wget ftp://ftp.cs.univ-paris8.fr/mirrors/ftp.gnu.org/screen/screen-$SCREEN_RELEASE.tar.gz
    tar -zxvf screen-$SCREEN_RELEASE.tar.gz

#     cd screen*
#     patch -p0 screen/main.c < $INSTALL_PACKAGES/screen/main.c.patch

    # avoid crash at launch time, speech server reinit, say word
    patch -p0 < $INSTALL_PACKAGES/screen/screen.patch

    cd screen-$SCREEN_RELEASE
    ./configure --prefix=/usr
    make
    make install
    echo "autodetach on" >> /etc/screenrc

    cd /tmp
    rm -rf screen*
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    export INSTALL_PACKAGES=/usr/share/oralux/install/packages

    chroot $BUILD  bash -c "\
    apt-get --purge remove screen;\
    cd /tmp;\
    rm -rf screen*;\
    wget ftp://ftp.cs.univ-paris8.fr/mirrors/ftp.gnu.org/screen/screen-$SCREEN_RELEASE.tar.gz\
    tar -zxvf screen-$SCREEN_RELEASE.tar.gz;\
    patch -p0 < $INSTALL_PACKAGES/screen/screen.patch
    cd screen-$SCREEN_RELEASE;\
    ./configure --prefix=/usr;\
    make;\
    make install;\
    echo \"autodetach on\" >> /etc/screenrc"
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
