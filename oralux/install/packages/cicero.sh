#! /bin/sh
# ----------------------------------------------------------------------------
# cicero.sh
# $Id: cicero.sh,v 1.4 2006/03/05 18:28:57 gcasse Exp $
# $Author: gcasse $
# Description: Installing Cicero
# $Date: 2006/03/05 18:28:57 $ |
# $Revision: 1.4 $ |
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
export CICERO_RELEASE=0.6
export SERVER=/usr/share/emacs/site-lisp/emacspeak/servers
export ARCH_CICERO=cicero-${CICERO_RELEASE}.tar.gz

cd $ARCHDIR

if [ ! -e $ARCH_CICERO ]
    then
    echo "Downloading $ARCH_CICERO"
    wget http://www.cam.org/~nico/cicero/$ARCH_CICERO
fi

####
# Installing the package in the current tree
InstallPackage()
{
#    apt-get --purge remove cicero
    cd /tmp
    rm -rf cicero*
    rm -rf /usr/local/share/cicero*
    tar -zxvf $ARCHDIR/$ARCH_CICERO

    # Patch from Stephane Doyon (compatibility with the Emacspeak server)
    cd cicero-$CICERO_RELEASE
    patch < $INSTALL_PACKAGES/cicero/diff-shell-pipe-fix

    # customize config 
    cp config.py.sample config.py
    patch < $INSTALL_PACKAGES/cicero/config.patch

    # Installation
    cd ..
    mv cicero-$CICERO_RELEASE /usr/local/share/
    cd /usr/local/share/
    ln -s cicero-$CICERO_RELEASE cicero

    # Emacspeak speech server
    mkdir $SERVER/cicero-api
    cp $INSTALL_PACKAGES/cicero/Makefile $SERVER/cicero-api
    cp $INSTALL_PACKAGES/cicero/tclcicero.c $SERVER/cicero-api
    cp $INSTALL_PACKAGES/cicero/cicero $SERVER
    echo cicero >> $SERVER/.servers
    cd $SERVER/cicero-api
    make

    cd /tmp
    rm -rf cicero*
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    export INSTALL_PACKAGES=/usr/share/oralux/install/packages

#     rm -rf cicero*;\
#     wget http://www.cam.org/~nico/cicero/cicero-$CICERO_RELEASE.tar.gz;\

    cd $BUILD/tmp
    rm -rf cicero*
    rm -rf $BUILD/usr/local/share/cicero*
    tar -zxvf $ARCHDIR/$ARCH_CICERO
    chroot $BUILD  bash -c "\
    cd /tmp;\
    cd cicero-$CICERO_RELEASE;\
    patch < $INSTALL_PACKAGES/cicero/diff-shell-pipe-fix;\
    cp config.py.sample config.py;\
    patch < $INSTALL_PACKAGES/cicero/config.patch;\
    cd ..;\
    mv cicero-$CICERO_RELEASE /usr/local/share/;\
    cd /usr/local/share/;\
    ln -s cicero-$CICERO_RELEASE cicero;\
    mkdir $SERVER/cicero-api;\
    cp $INSTALL_PACKAGES/cicero/Makefile $SERVER/cicero-api;\
    cp $INSTALL_PACKAGES/cicero/tclcicero.c $SERVER/cicero-api;\
    cp $INSTALL_PACKAGES/cicero/cicero $SERVER;\
    echo cicero >> $SERVER/.servers;\
    cd $SERVER/cicero-api;\
    make"
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
