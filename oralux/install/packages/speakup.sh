#! /bin/sh
# ----------------------------------------------------------------------------
# speakup.sh
# $Id: speakup.sh,v 1.2 2006/04/17 22:38:19 gcasse Exp $
# $Author: gcasse $
# Description: Installing the user space softwares related to Speakup.
# $Date: 2006/04/17 22:38:19 $ |
# $Revision: 1.2 $ |
# Copyright (C) 2006 Gilles Casse (gcasse@oralux.org)
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
#set -vx
source ../oralux.conf

export M_ORIG=$ORALUX/multispeech-up

####
# Installing the package in the current tree
InstallPackage()
{
    install -m 755 $INSTALL_PACKAGES/speakup/speakupconf /usr/local/bin

# Installing multispeech-up
    cd $M_ORIG
    make
    make install
    make clean
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    install -m 755 $INSTALL_PACKAGES/speakup/speakupconf $BUILD/usr/local/bin

# Installing multispeech-up
    export M_BUILD=/usr/share/oralux/multispeech-up

    install -d $BUILD$M_BUILD
    cp -pR $M_ORIG/* $BUILD$M_BUILD

    chroot $BUILD bash -c 'cd $M_BUILD;\
     make;\
     make install;\
     make clean'
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