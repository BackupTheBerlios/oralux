#! /bin/sh
# ----------------------------------------------------------------------------
# cicero.sh
# $Id: cicero.sh,v 1.1 2005/06/11 22:48:37 gcasse Exp $
# $Author: gcasse $
# Description: Installing Cicero
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
export CICERO_RELEASE=0.6
export SERVER=/usr/share/emacs/site-lisp/emacspeak/servers

####
# Installing the package in the current tree
InstallPackage()
{
#    apt-get --purge remove cicero
    cd /tmp
    rm -rf cicero*
    wget http://www.cam.org/~nico/cicero/cicero-CICERO_RELEASE.tar.gz
    tar -zxvf cicero-$CICERO_RELEASE.tar.gz

    # Patch from Stephane Doyon (compatibility with the Emacspeak server)
    cd cicero-$CICERO_RELEASE
    patch < $INSTALL_PACKAGES/cicero/diff-shell-pipe-fix

    # customize config 
    cp config.py.sample config.py
    patch < $INSTALL_PACKAGES/cicero/config.patch

    # Installation
    mv cicero-$CICERO_RELEASE /usr/local/share/
    cd /usr/local/share/
    ln -s cicero-$CICERO_RELEASE cicero

    # Emacspeak speech server
    mkdir $SERVER/cicero-api
    cp cicero/Makefile $SERVER/cicero-api
    cp cicero/tclcicero.c $SERVER/cicero-api
    cp cicero/cicero $SERVER
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

    chroot $BUILD  bash -c "\
    cd /tmp;\
    rm -rf cicero*;\
    wget http://www.cam.org/~nico/cicero/cicero-CICERO_RELEASE.tar.gz;\
    tar -zxvf cicero-$CICERO_RELEASE.tar.gz;\
    cd cicero-$CICERO_RELEASE;\
    patch < $INSTALL_PACKAGES/cicero/diff-shell-pipe-fix;\
    cp config.py.sample config.py;\
    patch < $INSTALL_PACKAGES/cicero/config.patch;\
    mv cicero-$CICERO_RELEASE /usr/local/share/;\
    cd /usr/local/share/;\
    ln -s cicero-$CICERO_RELEASE cicero;\
    mkdir $SERVER/cicero-api;\
    cp cicero/Makefile $SERVER/cicero-api;\
    cp cicero/tclcicero.c $SERVER/cicero-api;\
    cp cicero/cicero $SERVER;\
    echo cicero >> $SERVER/.servers;\
    cd $SERVER/cicero-api;\
    make;\
    cd /tmp;\
    rm -rf cicero*"
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
