#! /bin/sh
# ----------------------------------------------------------------------------
# menu.sh
# $Id: menu.sh,v 1.1 2004/09/27 20:29:59 gcasse Exp $
# $Author: gcasse $
# Description: Installing the audio menu
# $Date: 2004/09/27 20:29:59 $ |
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
# Installing in the current tree
InstallPackage()
{
    echo disabled
}


####
# Adding the information to the new Oralux tree
Copy2Oralux()
{
   # WARNING
   # We can't embedded any DECtalk file in the CD.
   # As getvoice is built from dtk (dtk/ttsapi.h)
   # we have to delete the related objects
   cd $MAIN
   make oralux
   make minimenu
   make clean.getvoice 2>/dev/null
   rm -f *.o

   # Installing the oralux runtime
   rm -rf $BUILD/usr/share/oralux

   cd $ORALUX/..
   tar --dereference --exclude factory --exclude arch --exclude CVS --exclude "*~" -cf - oralux | tar -C $BUILD/usr/share -xf -
   chroot $BUILD bash -c "cd /usr/bin; rm -f oralux.sh; ln -s /usr/share/oralux/main/oralux.sh ."

   # Installing the text based dialog
   chroot $BUILD bash -c "cd /usr/bin; rm -f dialog; ln -s /usr/share/oralux/main/dialog.php dialog"
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
