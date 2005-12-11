#! /bin/sh
# ----------------------------------------------------------------------------
# emacs.sh
# $Id: emacs.sh,v 1.4 2005/12/11 23:02:33 gcasse Exp $
# $Author: gcasse $
# Description: Emacs
# $Date: 2005/12/11 23:02:33 $ |
# $Revision: 1.4 $ |
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

EMACS_REL="21.4"

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install wget-el
    echo "nothing to do"
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    export URL="http://cvs.savannah.gnu.org/viewcvs/emacs/emacs/etc"
    export DEST="$BUILD/usr/share/emacs/$EMACS_REL/etc"

    chroot $BUILD apt-get install wget-el
    cd /tmp

    wget $URL/TUTORIAL.pt_BR?rev=1.8 --output-document=TUTORIAL.pt_BR
    wget $URL/TUTORIAL.ru?rev=1.13 --output-document=TUTORIAL.ru

    install -d $DEST
    install -m 444 TUTORIAL.pt_BR TUTORIAL.ru $DEST
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
