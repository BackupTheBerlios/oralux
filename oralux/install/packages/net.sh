#! /bin/sh
# ----------------------------------------------------------------------------
# net.sh
# $Id: net.sh,v 1.2 2004/10/24 21:34:34 gcasse Exp $
# $Author: gcasse $
# Description: ppp configuration tool, adsl drivers,...
# $Date: 2004/10/24 21:34:34 $ |
# $Revision: 1.2 $ |
# Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
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

####
# Installing the package in the current tree
InstallPackage()
{
    echo "nothing to do"
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    # pppconfig
    cp $INSTALL_PACKAGES/net/pppconfig $BUILD/usr/sbin

    # dhclient (in fact pump), useful for eagle-usb
    cp $INSTALL_PACKAGES/net/dhclient $BUILD/sbin/dhclient

    # eagle-usb
    cp $INSTALL_PACKAGES/net/startadsl $BUILD/usr/bin
    cp $INSTALL_PACKAGES/net/stopadsl $BUILD/usr/bin

    cd $BUILD/var/tmp
    rm -rf eagle-usb*
    wget http://switch.dl.sourceforge.net/sourceforge/eagle-usb/eagle-usb-1.9.6.tar.bz2
    tar -jxvf eagle-usb-1.9.6.tar.bz2 
    cd eagle-usb*
    //
    echo "The kernel has been compiled using the following release of gcc:"
    cat /proc/version | awk '{print $5,$6,$7}'
    export CC=gcc-2.95
    echo "CC is currently set to $CC"
    echo "If this is correct, press RETURN otherwise press Control C"
    read a

    # USE_IFUPDOWN=0: useful for 'Free Degroupe' to force dhclient

    chroot $BUILD bash -c "cd /var/tmp/eagle-usb*;\
	export CC=gcc-2.95;\
	export USE_IFUPDOWN=0;\
	./configure --prefix=/usr/local/eagle-usb --exec-prefix=/usr/local/eagle-usb;\
	make uninstall;\
	make clean;\
	make;\
	make install;\
	depmod -a;\
	mkdir -p /etc/eagle-usb/hotplug;\
	cd /usr/lib/hotplug;ln -s /etc/eagle-usb/hotplug eagle-usb;"

    cp $INSTALL_PACKAGES/net/eagleconfig $BUILD/usr/local/eagle-usb/sbin
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
