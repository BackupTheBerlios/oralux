#! /bin/sh
# ----------------------------------------------------------------------------
# doc.sh
# $Id: doc.sh,v 1.1 2004/09/27 20:30:00 gcasse Exp $
# $Author: gcasse $
# Description: Installing the documentation
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
BUILDDOC=$BUILD/usr/share/oralux/doc

# French Emacspeak tutorial from Pierre Lorenzon
URL=www.pollock-nageoire.net
cd /tmp
rm -rf $URL

wget -r -k http://$URL/emacspeak/index.html
mkdir $BUILDDOC/pl
mv $URL/emacspeak $BUILDDOC/pl
rm -rf $URL

# Emacspeak introduction
cp $ORALUX/doc/intro*.txt $BUILDDOC

# Html files
cd $ORALUX/doc/htm
tar --dereference --exclude CVS --exclude "*.sav" --exclude "*~" -cf - * | tar -C $NEW_ORALUX/KNOPPIX -xf -

# Emacspeak's tutorials: under /usr/share/oralux/doc/tutorials and are installed - by side effect - when the oralux files are copied.

# French files from Carrefour Blinux
cp -R $ARCH/cblx $BUILDDOC

# TUX&GNU@school
cp -R $ARCH/tgs $BUILDDOC

#    chroot $BUILD apt-get -t unstable install eflite
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
