#! /bin/sh
# ----------------------------------------------------------------------------
# general.sh
# $Id: general.sh,v 1.1 2004/09/27 20:29:59 gcasse Exp $
# $Author: gcasse $
# Description: Installing general information
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
# Replacing the original KNOPPIX file from a customized one
substituteFile()
{
    file=$1
    dir=$2
    # If the new file "dir/file" (either coming from Knoppix or already updated for Oralux) isn't equal to the saved one.
    comp=`diff $INSTALL_PACKAGES/knoppix/$file.sav $dir/$file |wc -l|awk '{print $1}'`
    # we have to compare it with the previously updated Oralux file
    # If they are distinct, we are sure the new file has to replace the saved file.
    # and then be updated with the oralux customization
    # 
    comp2=`diff $INSTALL_PACKAGES/knoppix/$file $dir/$file |wc -l|awk '{print $1}'`
    if [ "$comp" != "0" ] && [ "$comp2" != "0" ]; then
        echo "the new KNOPPIX file $dir/$file is different from our original one $INSTALL_PACKAGES/knoppix/$file.sav"
        echo "--> Press q, to quit (so that the following operations are achieved:"
	echo "1. The saved file $INSTALL_PACKAGES/knoppix/$file.sav might be equal to the new file $dir/$file from Knoppix."
	echo "2. Our customized file $INSTALL_PACKAGES/knoppix/$file might be updated with the new features from $dir/$file)."
	echo "--> Press any other key to jump this stage"
	read a

	if [ "$a" == "q" ] || [ "$a" == "Q" ]; then
	    exit 1
	fi  
    elif [ "$comp2" != "0" ]; then
        echo "the new KNOPPIX file $dir/$file is different from our customized one $INSTALL_PACKAGES/knoppix/$file"
        echo "--> Press q, to quit (so that the following operations are achieved:"
	echo "The customized file $INSTALL_PACKAGES/knoppix/$file will take in account the new features from $dir/$file)."
	echo "--> Press any other key to jump this stage"
	read a

	if [ "$a" == "q" ] || [ "$a" == "Q" ]; then
	    exit 1
	fi 
    fi
}

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
   # knoppix-autoconfig is customized to be sure that the available memory is not checked
   # In the future, this check might be restored and updated
   substituteFile knoppix-autoconfig $BUILD/etc/init.d

   # Saveconfig and mkpersistenthome: dialog boxes are removed, french translations are added
   substituteFile saveconfig $BUILD/usr/sbin
   substituteFile mkpersistenthome $BUILD/usr/sbin

   cd $INSTALL_PACKAGES
   echo "export LD_LIBRARY_PATH=/lib:/usr/local/lib:\$LD_LIBRARY_PATH" >> $BUILD/etc/profile
   cp knoppix/inittab $BUILD/etc
   chmod -w $BUILD/etc/inittab
   chroot $BUILD apt-get update

   # A simpler prompt, no colors in ls
   cp $INSTALL_PACKAGES/bashrc $BUILD/etc/bashrc

   # Default values for aumix
   cp $INSTALL_PACKAGES/aumixrc $BUILD/etc/aumixrc
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
