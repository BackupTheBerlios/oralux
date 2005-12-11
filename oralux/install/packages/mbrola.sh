#! /bin/sh
# ----------------------------------------------------------------------------
# mbrola.sh
# $Id: mbrola.sh,v 1.7 2005/12/11 23:02:33 gcasse Exp $
# $Author: gcasse $
# Description: Installing mbrola
# $Date: 2005/12/11 23:02:33 $ |
# $Revision: 1.7 $ |
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

    DOC=$BUILD/usr/local/share/doc/mbrola
    install -d $DOC

    VOICES=$BUILD/usr/local/share/mbrola/voices
    install -d $VOICES

    # mbrola binary: mbrola-linux-i386
    cd $MBROLA
    wget ${MBROLA_URL}/bin/pclinux/mbr301h.zip
    unzip mbr301h.zip
    rm -f $BUILD/usr/local/bin/mbrola
    install -m 555 $BUILD/tmp/mbrola/mbrola-linux-i386 $BUILD/usr/local/bin/mbrola

    # br1
    cd $MBROLA
    wget ${MBROLA_URL}/dba/br1/br1-971105.zip
    unzip br1*zip
    install -m 444 $MBROLA/br1/br1 $VOICES
    install -d $DOC/br1
    install -m 444 $MBROLA/br1/license.txt $DOC/br1
    install -m 444 $MBROLA/br1/br1.txt $DOC/br1

    # de6
    cd $MBROLA
    wget ${MBROLA_URL}/dba/de6/de6.zip
    unzip de6.zip
    install -m 444 $MBROLA/de6/de6 $VOICES
    install -d $DOC/de6
    install -m 444 $MBROLA/de6/license.txt $DOC/de6
    install -m 444 $MBROLA/de6/de6.txt $DOC/de6

    # en1
    cd $MBROLA
    wget ${MBROLA_URL}/dba/en1/en1-980910.zip
    unzip en1*zip
    install -m 444 $MBROLA/en1/en1 $VOICES
    install -d $DOC/en1
    install -m 444 $MBROLA/en1/en1.txt $DOC/en1

    # es1
    cd $MBROLA
    wget ${MBROLA_URL}/dba/es1/es1-980610.zip
    unzip es1*zip
    install -m 444 $MBROLA/es1/es1 $VOICES
    install -d $DOC/es1
    install -m 444 $MBROLA/es1/license.txt $DOC/es1
    install -m 444 $MBROLA/es1/es1.txt $DOC/es1

    # fr4
    cd $MBROLA
    wget ${MBROLA_URL}/dba/fr4/fr4-990521.zip
    unzip fr4*zip
    install -m 444 $MBROLA/fr4 $VOICES
    install -d $DOC/fr4
    install -m 444 $MBROLA/license.txt $DOC/fr4
    install -m 444 $MBROLA/fr4.txt $DOC/fr4

    #removing unused files
    cd $BUILD/tmp
    rm -rf $MBROLA
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

