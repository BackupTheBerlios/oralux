#! /bin/sh
# ----------------------------------------------------------------------------
# general.sh
# $Id: general.sh,v 1.6 2006/01/30 22:49:38 gcasse Exp $
# $Author: gcasse $
# Description: Installing general information
# $Date: 2006/01/30 22:49:38 $ |
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
# Installing in the current tree
InstallPackage()
{
    echo disabled
}


####
# Adding the information to the new Oralux tree
Copy2Oralux()
{
   cd $INSTALL_PACKAGES/knoppix
   install -m 755 knoppix-autoconfig $BUILD/etc/init.d
   install -m 755 saveconfig knoppix-image knoppix-mkimage $BUILD/usr/sbin
   install -m 644 inittab $BUILD/etc
   install -m 444 minirt.gz $NEW_ORALUX/boot/isolinux 

   cd $INSTALL_PACKAGES
   echo "export LD_LIBRARY_PATH=/lib:/usr/local/lib:\$LD_LIBRARY_PATH" >> $BUILD/etc/profile

   chroot $BUILD bash -c "wget http://ftp-master.debian.org/ziyi_key_2005.asc -O - | sudo apt-key add -; wget http://ftp-master.debian.org/ziyi_key_2006.asc -O - | sudo apt-key add -"

   chroot $BUILD apt-get update
   install -m 644 bashrc aumixrc $BUILD/etc

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
