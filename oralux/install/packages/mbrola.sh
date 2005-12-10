#! /bin/sh
# ----------------------------------------------------------------------------
# mbrola.sh
# $Id: mbrola.sh,v 1.5 2005/12/10 14:37:45 gcasse Exp $
# $Author: gcasse $
# Description: Installing mbrola
# $Date: 2005/12/10 14:37:45 $ |
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

export MBROLA_URL="http://tcts.fpms.ac.be/synthesis/mbrola"

####
# Installing the package in the current tree
InstallPackage()
{
    MBROLA=$BUILD/tmp/mbrola
    mkdir $MBROLA

    VOICES=$BUILD/usr/local/share/mbrola/voices
    mkdir -p $VOICES

    # mbrola binary: mbrola-linux-i386
    cd $MBROLA
    wget ${MBROLA_URL}/bin/pclinux/mbr301h.zip
    unzip mbr301h.zip
    rm -f $BUILD/usr/local/bin/mbrola
    cp $BUILD/tmp/mbrola/mbrola-linux-i386 $BUILD/usr/local/bin/mbrola

    mkdir -p $BUILD/usr/share/oralux/doc/license/mbrola

    # br1
    cd $MBROLA
    wget ${MBROLA_URL}/dba/br1/br1-971105.zip
    unzip br1*zip
    mv $MBROLA/br1/br1 $VOICES
    # TODO: add br1.txt to doc
    mv $MBROLA/br1/license.txt $BUILD/usr/share/oralux/doc/license/mbrola/br1.txt
    # de6
    cd $MBROLA
    wget ${MBROLA_URL}/dba/de6/de6.zip
    unzip de6.zip
    mv $MBROLA/de6/de6 $VOICES
    # TODO: add de6.txt to doc
    mv $MBROLA/de6/license.txt $BUILD/usr/share/oralux/doc/license/mbrola/de6.txt

    # en1
    cd $MBROLA
    wget ${MBROLA_URL}/dba/en1/en1-980910.zip
    unzip en1*zip
    mv $MBROLA/en1/en1 $VOICES/en1
    mv $MBROLA/en1/en1.txt $BUILD/usr/share/oralux/doc/license/mbrola

    # es1
    cd $MBROLA
    wget ${MBROLA_URL}/dba/es1/es1-980610.zip
    unzip es1*zip
    mv $MBROLA/es1/es1 $VOICES
    # TODO: add es1.txt to doc
    mv $MBROLA/es1/license.txt $BUILD/usr/share/oralux/doc/license/mbrola

    # fr4
    cd $MBROLA
    wget ${MBROLA_URL}/dba/fr4/fr4-990521.zip
    unzip fr4*zip
    mv $MBROLA/fr4 $VOICES/fr4
    # TODO: add fr4.txt to doc
    mv $MBROLA/license.txt $BUILD/usr/share/oralux/doc/license/mbrola/license_fr4.txt
    
    #removing unused files
    cd $BUILD/tmp
    rm -rf mbrola
}

####
case $1 in
    i|I)
	BUILD=""
        InstallPackage
        ;;
    b|B)
	# copy to the new Oralux tree
        InstallPackage
        ;;
    *)
        echo "I (install) or B(new tree) is expected"
        ;;
esac

exit 0

