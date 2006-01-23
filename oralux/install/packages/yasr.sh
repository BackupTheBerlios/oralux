#! /bin/sh
# ----------------------------------------------------------------------------
# yasr.sh
# $Id: yasr.sh,v 1.9 2006/01/23 22:08:36 gcasse Exp $
# $Author: gcasse $
# Description: Installing Yasr
# $Date: 2006/01/23 22:08:36 $ |
# $Revision: 1.9 $ |
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
    # special escape key (tengoo)
    patch -p0 < $INSTALL_PACKAGES/yasr/test-tengoo.patch
    # distinct voices (tengoo)
    patch -p0 < $INSTALL_PACKAGES/yasr/voice.patch
    # any char is speakable (multispeech)
#    patch -p0 < $INSTALL_PACKAGES/yasr/unspeakable.patch

    cd yasr-$YASR_RELEASE
    ./configure --prefix=/usr
    make
    make install
    cd /tmp
    rm -rf yasr*

    install -m 644 $INSTALL_PACKAGES/yasr/fr.mo /usr/share/locale/fr/LC_MESSAGES/yasr.mo 
    install -m 644 $INSTALL_PACKAGES/yasr/es.mo /usr/share/locale/es/LC_MESSAGES/yasr.mo 
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    export NEW_INSTALL_PACKAGES=/usr/share/oralux/install/packages
    export SRC=$INSTALL_PACKAGES/yasr
    export DEST=$BUILD/$NEW_INSTALL_PACKAGES/yasr

    install -d $DEST
    install -m 555 $SRC/* $DEST 

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
    patch -p0 < $NEW_INSTALL_PACKAGES/yasr/voice.patch;\
    cd yasr-$YASR_RELEASE;\
    ./configure --prefix=/usr;\
    make;\
    make install;
    install -m 644 $NEW_INSTALL_PACKAGES/yasr/fr.mo /usr/share/locale/fr/LC_MESSAGES/yasr.mo;\
    install -m 644 $NEW_INSTALL_PACKAGES/yasr/es.mo /usr/share/locale/es/LC_MESSAGES/yasr.mo"
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
