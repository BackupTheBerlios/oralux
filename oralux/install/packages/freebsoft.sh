#! /bin/sh
# ----------------------------------------------------------------------------
# freebsoft.sh
# $Id: freebsoft.sh,v 1.1 2006/04/17 09:11:42 gcasse Exp $
# $Author: gcasse $
# Description: packages from the Freebsoft project
# $Date: 2006/04/17 09:11:42 $ |
# $Revision: 1.1 $ |
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

#export LIST="recode libspeechd-dev speech-dispatcher"
export LIST="libspeechd-dev speech-dispatcher"
export SPEECHD_UP="speechd-up-0.3" 
export ARCH_SPEECHD_UP=$ARCHDIR/$SPEECHD_UP.tar.gz 

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install $LIST

#     # speech-dispatcher will be started on demand
#     update-rc.d -f speech-dispatcher remove

    install -m 644 $INSTALL_PACKAGES/freebsoft/speechd.conf /etc/speech-dispatcher/

    cd /tmp
    tar -zxvf $ARCH_SPEECHD_UP
    cd $SPEECHD_UP
    ./configure
    make all
    make install
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD bash -c "apt-get install $LIST"
    install -m 644 $INSTALL_PACKAGES/freebsoft/speechd.conf $BUILD/etc/speech-dispatcher/

    cd $BUILD/tmp
    tar -zxvf $ARCH_SPEECHD_UP
    chroot $BUILD bash -c 'cd /tmp/$SPEECHD_UP;./configure;make all;make install'
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
