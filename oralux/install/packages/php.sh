#! /bin/sh
# ----------------------------------------------------------------------------
# php.sh
# $Id: php.sh,v 1.1 2004/09/27 20:30:00 gcasse Exp $
# $Author: gcasse $
# Description: Installing PHP
# $Date: 2004/09/27 20:30:00 $ |
# $Revision: 1.1 $ |
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
export OPT_CONF="--with-libxml-dir --disable-cgi --with-gettext --enable-dio"

####
# Installing the package in the current tree
InstallPackage()
{
#    apt-get install php4-cgi
    
    # php5
    apt-get install libxml2-dev
    cd /tmp
    rm -rf /tmp/php-5*
    wget http://www.php.net/get/php-5.0.0.tar.bz2/from/fr.php.net/mirror
    tar -jxvf php-5.0.0.tar.bz2
    cd php-5.0.0
    cp $INSTALL_PACKAGES/php/dio.c ext/dio
    cp $INSTALL_PACKAGES/php/php_dio.h ext/dio
    ./configure $OPT_CONF
    make
    make install
    cd /usr/bin 
    ln -s /usr/local/bin/php php5

    cd /etc/alternatives;rm -f php;ln -s /usr/bin/php5 php

    rm -rf /tmp/php-5*
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
#    chroot $BUILD apt-get install php4-cgi

    # php5
    cd $BUILD/var/tmp
    rm -rf $BUILD/var/tmp/php-5*
    wget http://www.php.net/get/php-5.0.0.tar.bz2/from/fr.php.net/mirror
    tar -jxvf php-5.0.0.tar.bz2
    cp $INSTALL_PACKAGES/php/dio.c php-5.0.0/ext/dio
    cp $INSTALL_PACKAGES/php/php_dio.h php-5.0.0/ext/dio

    chroot $BUILD  bash -c "apt-get install libxml2-dev; cd /var/tmp/php-5.0.0;./configure $OPT_CONF;make;make install;cd /usr/bin; ln -s /usr/local/bin/php php5;cd /etc/alternatives;rm -f php;ln -s /usr/bin/php5 php"
    rm -rf $BUILD/var/tmp/php-5*
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
