#! /bin/sh
# This file is a template to quickly create an Oralux script.
# To update the relevant fields with your data, search the keyword: [INPUT 
# ----------------------------------------------------------------------------
# skeleton.sh INPUT: replace skeleton.sh by the name of your file
# $Id: skeleton.sh,v 1.1 2004/09/27 20:30:00 gcasse Exp $
# $Author: gcasse $
# Description: INPUT: a short description]
# $Date: 2004/09/27 20:30:00 $ |
# $Revision: 1.1 $ |
# Copyright (C) 2004 INPUT: for example: Victor Hugo (vhugo@pantheon.fr)
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
# INPUT: 
# the required sequence to install the packages for your running distro
# for example, to install cdtool, it is:
#    apt-get install cdtool 
#    cd /usr/bin
#    ln -s ./cdtool cdstart 
#
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
# INPUT: 
# the required sequence to install the packages to the Oralux build directory
#
# for example, to copy documentation to the Oralux build directory, it is:
#    cp -pR myDocumentation.txt $BUILD/usr/share/oralux/doc
#
# Another example, to install cdtool, it is:
#    chroot $BUILD bash -c "apt-get install cdtool; cd /usr/bin; ln -s ./cdtool cdstart"
#
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
