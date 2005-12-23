#! /bin/sh
# ----------------------------------------------------------------------------
# yasr.sh
# $Id: yasr.sh,v 1.5 2005/12/23 20:13:22 gcasse Exp $
# $Author: gcasse $
# Description: Installing Yasr
# $Date: 2005/12/23 20:13:22 $ |
# $Revision: 1.5 $ |
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
export YASR_RELEASE=0.6.7
cd $ARCH
#wget http://ovh.dl.sourceforge.net/sourceforge/yasr/yasr-$YASR_RELEASE.tar.gz


####
# Installing the package in the current tree
InstallPackage()
{
    apt-get --purge remove yasr
    rm -rf /etc/yasr
    cd /tmp
    rm -rf yasr*
    cp $ARCH/yasr-$YASR_RELEASE.tar.gz .
    tar -zxvf yasr-$YASR_RELEASE.tar.gz

#     cd yasr*
#     patch -p0 yasr/main.c < $INSTALL_PACKAGES/yasr/main.c.patch

    # avoid crash at launch time, speech server reinit, say word
    patch -p0 < $INSTALL_PACKAGES/yasr/yasr.patch
    patch -p0 < $INSTALL_PACKAGES/yasr/arg.patch
    # special escape key
    patch -p0 < $INSTALL_PACKAGES/yasr/test-tengoo.patch

    cd yasr-$YASR_RELEASE
    ./configure --prefix=/usr
    make
    make install
    cd /tmp
    rm -rf yasr*
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    export NEW_INSTALL_PACKAGES=/usr/share/oralux/install/packages
    export SRC=$INSTALL_PACKAGES/yasr
    export DEST=$BUILD/$NEW_INSTALL_PACKAGES/yasr

    install -d $DEST
    install -m 555 $SRC/yasr.patch $DEST 
    install -m 555 $SRC/arg.patch $DEST
    install -m 555 $SRC/test-tengoo.patch $DEST

    rm -rf $BUILD/etc/yasr
    cd $BUILD/tmp
    rm -rf yasr*
    cp $ARCH/yasr-$YASR_RELEASE.tar.gz .
    tar -zxvf yasr-$YASR_RELEASE.tar.gz

    chroot $BUILD  bash -c "\
    apt-get --purge remove yasr;\
    cd /tmp;\
    patch -p0 < $NEW_INSTALL_PACKAGES/yasr/yasr.patch;\
    patch -p0 < $NEW_INSTALL_PACKAGES/yasr/arg.patch;\
    patch -p0 < $NEW_INSTALL_PACKAGES/yasr/test-tengoo.patch;\
    cd yasr-$YASR_RELEASE;\
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
