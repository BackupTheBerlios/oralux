#! /bin/sh
# ses.sh 
# $Id: ses.sh,v 1.1 2004/09/27 20:30:00 gcasse Exp $
# $Author: gcasse $
# Description: installing SES Simple Emacs Spreadsheet
# $Date: 2004/09/27 20:30:00 $ |
# $Revision: 1.1 $ |
# Copyright (C) 2004 Sylvie Xhaard (sxhaard@free.fr)
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
# Installing SES
cd /tmp
wget http://mywebpages.comcast.net/jyavner/ses/ses21-031130.tgz
tar -xvzf ses21-031130.tgz
mkdir /usr/share/emacs/site-lisp/ses
cd ses21-031130
cp * /usr/share/emacs/site-lisp/ses
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
# Installing SES
# the required sequence to install the packages to the Oralux build directory
cd $BUILD/var/tmp
wget http://mywebpages.comcast.net/jyavner/ses/ses21-031130.tgz
tar -xvzf ses21-031130.tgz
mkdir $BUILD/usr/share/emacs/site-lisp/ses
cd $BUILD/var/tmp/ses21-031130
cp * $BUILD/usr/share/emacs/site-lisp/ses
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
