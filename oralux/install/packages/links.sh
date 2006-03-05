#! /bin/sh
# This file is a template to quickly create an Oralux script.
# To update the relevant fields with your data, search the keyword: [INPUT 
# ----------------------------------------------------------------------------
# links.sh
# $Id: links.sh,v 1.5 2006/03/05 18:28:57 gcasse Exp $
# $Author: gcasse $
# Description: installing links
# $Date: 2006/03/05 18:28:57 $ |
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
export DIR=links-2.1pre20
export TARBALL=$DIR.tar.bz2
cd $ARCHDIR
#wget http://links.twibright.com/download/$DIR.tar.bz2

####
# Installing the package in the current tree
InstallPackage()
{
    cd /tmp
    rm -f links*bz2
    tar -jxvf $ARCHDIR/$TARBALL
    cd $DIR
    ./configure --enable-javascript --with-ssl=/usr/include/openssl
    make
    rm -f /usr/bin/links
    cp links /usr/bin/links
    rm -rf /tmp/links*
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    cd $BUILD/tmp
    rm -rf links*
    tar -jxvf $ARCHDIR/$TARBALL
    chroot $BUILD bash -c "cd /tmp/$DIR; ./configure --enable-javascript --with-ssl=/usr/include/openssl; make; rm -f /usr/bin/links; cp links /usr/bin/links"
    rm -rf $BUILD/tmp/links*
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
