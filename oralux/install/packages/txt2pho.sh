#! /bin/sh
# ----------------------------------------------------------------------------
# txt2pho.sh
# $Id: txt2pho.sh,v 1.2 2005/12/10 22:35:07 gcasse Exp $
# $Author: gcasse $
# Description: Installing txt2pho, Spanish phonetizer
# $Date: 2005/12/10 22:35:07 $ |
# $Revision: 1.2 $ |
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

URL="http://www.ikp.uni-bonn.de/dt/forsch/phonetik/hadifix/txt2pho.zip"

####
# Replacing the original file by a customized one
substituteFile()
{
    file=$1
    dir=$2
    # If the new file "dir/file" (either coming from Txt2pho or already updated for Oralux) isn't equal to the saved one.
    comp=`diff $INSTALL_PACKAGES/txt2pho/$file.sav $dir/$file |wc -l|awk '{print $1}'`
    # we have to compare it with the previously updated Oralux file
    # If they are distinct, we are sure the new file has to replace the saved file.
    # and then be updated with the oralux customization
    # 
    comp2=`diff $INSTALL_PACKAGES/txt2pho/$file $dir/$file |wc -l|awk '{print $1}'`
    if [ "$comp" != "0" ] && [ "$comp2" != "0" ]; then
        echo "the new TXT2PHO file $dir/$file is different from our original one $INSTALL_PACKAGES/txt2pho/$file.sav"
        echo "--> Press q, to quit (so that the following operations are achieved:"
	echo "1. The saved file $INSTALL_PACKAGES/txt2pho/$file.sav might be equal to the new file $dir/$file from Txt2pho."
	echo "2. Our customized file $INSTALL_PACKAGES/txt2pho/$file might be updated with the new features from $dir/$file)."
	echo "--> Press any other key to jump this stage"
	read a

	if [ "$a" == "q" ] || [ "$a" == "Q" ]; then
	    exit 1
	fi  
    elif [ "$comp2" != "0" ]; then
        echo "the new TXT2PHO file $dir/$file is different from our customized one $INSTALL_PACKAGES/txt2pho/$file"
        echo "--> Press q, to quit (so that the following operations are achieved:"
	echo "The customized file $INSTALL_PACKAGES/txt2pho/$file will take in account the new features from $dir/$file)."
	echo "--> Press any other key to jump this stage"
	read a

	if [ "$a" == "q" ] || [ "$a" == "Q" ]; then
	    exit 1
	fi 
    fi
}


####
# Installing the package in the current tree
InstallPackage()
{
    TMP="$BUILD/tmp"
    install -d $TMP

    cd $TMP
    wget $URL
    unzip txt2pho.zip
    cd txt2pho

    BIN="$BUILD/usr/local/bin"
    install -d $BIN
    install -m 555 txt2pho $BIN

    DATA="$BUILD/usr/local/share/txt2pho"
    install -d $DATA
    install -m 444 data/* $DATA

    DOC="$BUILD/usr/local/share/doc/txt2pho"
    install -d $DOC
    install -m 444 README $DOC

    ETC="$BUILD/etc"
    install -d $ETC

    INSTALL="$INSTALL_PACKAGES/txt2pho"
    install -d $INSTALL
    if [ -e $ETC/txt2pho ]; then
	cp $ETC/txt2pho $TMP/txt2phorc
	substituteFile txt2phorc $TMP
	rm $TMP/txt2phorc
    fi

    install -m 444 $INSTALL/txt2phorc $BUILD/etc/txt2pho
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

