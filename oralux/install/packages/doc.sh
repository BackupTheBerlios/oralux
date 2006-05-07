#! /bin/sh
# ----------------------------------------------------------------------------
# doc.sh
# $Id: doc.sh,v 1.6 2006/05/07 15:34:33 gcasse Exp $
# $Author: gcasse $
# Description: Installing the documentation
# $Date: 2006/05/07 15:34:33 $ |
# $Revision: 1.6 $ |
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
#rm -rf $URL

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
