#! /bin/sh
# ----------------------------------------------------------------------------
# mbrola.sh
# $Id: mbrola.sh,v 1.4 2005/12/04 22:42:27 gcasse Exp $
# $Author: gcasse $
# Description: Installing mbrola
# $Date: 2005/12/04 22:42:27 $ |
# $Revision: 1.4 $ |
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

####
# Installing the package in the current tree
InstallPackage()
{
    MBROLA=/tmp/mbrola
    mkdir $MBROLA

    VOICES=/usr/local/share/mbrola/voices
    mkdir -p $VOICES

    # mbrola binary: mbrola-linux-i386
    cd $MBROLA

    wget http://tcts.fpms.ac.be/synthesis/mbrola/bin/pclinux/mbr301h.zip
    unzip mbr301h.zip
    rm -f /usr/local/bin/mbrola
    cp /tmp/mbrola/mbrola-linux-i386 /usr/local/bin/mbrola

    mkdir -p /usr/share/oralux/doc/license/mbrola

    # fr3 (ParleMax)
    cd $MBROLA
    wget http://tcts.fpms.ac.be/synthesis/mbrola/dba/fr3/fr3-990324.zip
    unzip fr3*zip
    mv $MBROLA/fr3/fr3 $VOICES/fr3
    mv $MBROLA/fr3/fr3.txt $BUILD/usr/share/oralux/doc/license/mbrola
    mv $MBROLA/fr3/license.txt $BUILD/usr/share/oralux/doc/license/mbrola/license_fr3.txt

    # fr4 (Cicero)
    cd $MBROLA
    wget http://www.tcts.fpms.ac.be/synthesis/mbrola/dba/fr4/fr4-990521.zip
    unzip fr4*zip
    mv $MBROLA/fr4 $VOICES/fr4
    mv $MBROLA/fr4.txt $BUILD/usr/share/oralux/doc/license/mbrola
    mv $MBROLA/license.txt $BUILD/usr/share/oralux/doc/license/mbrola/license_fr4.txt

    # en1 (multispeech)
    cd $MBROLA
    wget http://tcts.fpms.ac.be/synthesis/mbrola/dba/en1/en1-980910.zip
    unzip en1*zip
    mv $MBROLA/en1/en1 $VOICES/en1
    mv $MBROLA/en1/en1.txt $BUILD/usr/share/oralux/doc/license/mbrola
    
    #removing unused files
    cd /tmp
    rm -rf mbrola
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    MBROLA=$BUILD/tmp/mbrola
    mkdir $MBROLA

    VOICES=$BUILD/usr/local/share/mbrola/voices
    mkdir -p $VOICES

    # mbrola binary: mbrola-linux-i386
    cd $MBROLA

    wget http://tcts.fpms.ac.be/synthesis/mbrola/bin/pclinux/mbr301h.zip
    unzip mbr301h.zip
    rm - f $BUILD/usr/local/bin/mbrola
    cp $BUILD/tmp/mbrola/mbrola-linux-i386 $BUILD/usr/local/bin/mbrola
    
    mkdir -p /usr/share/oralux/doc/license/mbrola

    # fr3 (ParleMax)
    cd $MBROLA
    wget http://tcts.fpms.ac.be/synthesis/mbrola/dba/fr3/fr3-990324.zip
    unzip fr3*zip
    mv $MBROLA/fr3/fr3 $VOICES/fr3
    mv $MBROLA/fr3/fr3.txt $BUILD/usr/share/oralux/doc/license/mbrola
    mv $MBROLA/fr3/license.txt $BUILD/usr/share/oralux/doc/license/mbrola/license_fr3.txt

    # fr4 (Cicero)
    cd $MBROLA
    wget http://www.tcts.fpms.ac.be/synthesis/mbrola/dba/fr4/fr4-990521.zip
    unzip fr4*zip
    mv $MBROLA/fr4 $VOICES/fr4
    mv $MBROLA/fr4.txt $BUILD/usr/share/oralux/doc/license/mbrola
    mv $MBROLA/license.txt $BUILD/usr/share/oralux/doc/license/mbrola/license_fr4.txt

    # en1 (multispeech)
    cd $MBROLA
    wget http://tcts.fpms.ac.be/synthesis/mbrola/dba/en1/en1-980910.zip
    unzip en1*zip
    mv $MBROLA/en1/en1 $VOICES/en1
    mv $MBROLA/en1/en1.txt $BUILD/usr/share/oralux/doc/license/mbrola

    #removing unused files
    cd $BUILD/tmp
    rm -rf mbrola
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


