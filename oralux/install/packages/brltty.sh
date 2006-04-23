#! /bin/sh
# ----------------------------------------------------------------------------
# brltty.sh
# $Id: brltty.sh,v 1.10 2006/04/23 23:14:04 gcasse Exp $
# $Author: gcasse $
# Description: Installing BRLTTY
# $Date: 2006/04/23 23:14:04 $ |
# $Revision: 1.10 $ |
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

export URL="http://www.mielke.cc/brltty/releases"
export BRLTTY=brltty-3.7.2
export BRLAPI_RPM=brlapi-0.4.1-1.i386.rpm
#export BRLAPI_DEVEL_RPM=brlapi-devel-0.4.1-1.i386.rpm
export ARCH_BRLTTY=$ARCHDIR/$BRLTTY.tar.gz


cd $ARCHDIR

if [ ! -e $ARCH_BRLTTY ]
    then
    echo "Downloading $BRLTTY"
    wget $URL/$BRLTTY.tar.gz
fi

# i=$BRLAPI_DEVEL_RPM
# if [ ! -e $ARCHDIR/$i ]
#     then
#     echo "Downloading $i"
#     wget $URL/$i
# fi

i=$BRLAPI_RPM
if [ ! -e $ARCHDIR/$i ]
    then
    echo "Downloading $i"
    wget $URL/$i
fi


####
# Installing the package in the current tree
 InstallPackage()
{
    apt-get remove --purge brltty libbrlapi libbrlapi1
    rm -rf /etc/brltty

    apt-get install bison
    touch /usr/include/linux/autoconf.h

    cd /tmp
    rm -rf /tmp/brl*
    tar -zxvf $ARCH_BRLTTY
    cd $BRLTTY
    ./configure
    make
    make install

#    rm -f /etc/brltty.conf

#    alien $ARCHDIR/$BRLAPI_DEVEL_RPM
    alien --script -i $ARCHDIR/$BRLAPI_RPM

}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    cd $BUILD/tmp
    rm -rf brl*
    cp $ARCH_BRLTTY .
    cp $ARCHDIR/$BRLAPI_RPM .

    cp $INSTALL_PACKAGES/brltty/brltty $BUILD/etc/init.d
    rm $BUILD/sbin/brltty*
    rm $BUILD/bin/brltty*

    chroot $BUILD  bash -c "apt-get remove --purge brltty libbrlapi libbrlapi1;\
    rm -rf /etc/brltty;\
    apt-get install bison;\
    touch /usr/include/linux/autoconf.h;\
    cd /tmp;\
    tar -zxvf $BRLTTY.tar.gz;\
    cd $BRLTTY;\
    ./configure;\
    make;\
    make install;\
    cd /tmp;\
    alien --script -i $BRLAPI_RPM"
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
