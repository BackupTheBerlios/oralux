#! /bin/sh
# ----------------------------------------------------------------------------
# tengoo.sh
# $Id: tengoo.sh,v 1.2 2006/01/22 15:19:45 gcasse Exp $
# $Author: gcasse $
# Description: Installing tengoo
# $Date: 2006/01/22 15:19:45 $ |
# $Revision: 1.2 $ |
# Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
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

export LIB_DEST=/usr/lib
cd $ARCH

####
# Installing the package in the current tree
InstallPackage()
{
    export TENGOO_BUILD=$ORALUX/tengoo
    apt-get install libglib2.0-dev scons
    cd $TENGOO_BUILD
    scons
    install -m 555 build/main/test-tengoo /usr/bin
    strip /usr/bin/test-tengoo

    cd build/plugin
    for i in *so.*; do
	install -m 555 $i $LIB_DEST
	pushd `pwd`
	cd $LIB_DEST
	ln -s $i $(echo $i| sed 's/so.*/so/')
	popd
    done

    ldconfig -n $LIB_DEST
#    adduser $USER tty
    for i in vcsa0 tty0 tty1 tty2 tty3 tty4; do
	chgrp tty /dev/$i
	chmod g+rw /dev/$i
    done
    rm -rf $TENGOO_BUILD/build
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    export TENGOO_BUILD=/usr/share/oralux/tengoo
    export TENGOO_ORIG=$ORALUX/tengoo
    echo "Check the env variable in SConstruct"
    grep env $TENGOO_ORIG/SConstruct
    echo "Press a key, if OK"
    read a

    install -d $BUILD$TENGOO_BUILD
    cp -pR $TENGOO_ORIG/* $BUILD$TENGOO_BUILD
    rm -rf $BUILD$TENGOO_BUILD/build

    chroot $BUILD bash -c 'apt-get install libglib2.0-dev scons;\
    cd $TENGOO_BUILD;\
    scons;\
    install -m 555 build/main/test-tengoo /usr/bin;\
    strip /usr/bin/test-tengoo;\
    cd build/plugin;\
    for i in *so.*; do \
	install -m 555 $i $LIB_DEST;\
	pushd `pwd`;\
	cd $LIB_DEST;\
	ln -s $i $(echo $i| sed 's/so.*/so/');\
	popd;\
    done;\
    ldconfig -n $LIB_DEST;\
    adduser knoppix tty;\
    for i in vcsa0 tty0 tty1 tty2 tty3 tty4; do \
	chgrp tty /dev/$i;\
	chmod g+rw /dev/$i;\
    done;\
    rm -rf $TENGOO_BUILD/build'
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
