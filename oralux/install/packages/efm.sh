#! /bin/sh
# ----------------------------------------------------------------------------
# efm.sh
# $Id: efm.sh,v 1.1 2004/09/27 20:29:59 gcasse Exp $
# $Author: gcasse $
# Description: Installing EFM (Emacspeak Festival MBROLA) from Pierre Lorenzon.
# Be aware that this script requires a fast internet connection and may take several hours to build the runtime. 
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
export SHAREDIR=/usr/local/share
export SHARE=$SHAREDIR/efm
export BINDIR=/usr/local/bin
export BIN=$BINDIR/efm
export LISP=/usr/local/share/emacs/site-lisp/efm

####
# Installing EFM in the current tree
InstallPackage()
{
#     echo "Obsolete usage"
# Pour 3.3
# mv /usr/include/c++/3.3/backward/strstream /usr/include/c++/3.3/backward/strstream.h
# Compilation d'efm avec g++ 3.2
    apt-get install libstdc++5-dev
    cd /usr/bin
    mv g++ g++.sav
    ln -s g++-3.2 g++

    cd $SHAREDIR/
    tar -zxvf $ARCH/efm-install-util.tgz 

    echo "Comparing $INSTALL_PACKAGES/efm/fst-interp.el.sav and $SHARE/emacspeak-18.0/lisp/fst-interp.el"
    sdiff $INSTALL_PACKAGES/efm/fst-interp.el.sav $SHARE/emacspeak-18.0/lisp/fst-interp.el
    echo "Press a key to copy fst-interp.el"
    read a
    cp fst-interp.el $SHARE/emacspeak-18.0/lisp/

    echo "Comparing $INSTALL_PACKAGES/efm/emacspeak-setup.el.sav $SHARE/emacspeak-18.0/lisp/"
    sdiff -s $INSTALL_PACKAGES/efm/emacspeak-setup.el.sav $SHARE/emacspeak-18.0/lisp/emacspeak-setup.el
    echo "Press a key to copy emacspeak-setup.el"
    read a
    cp emacspeak-setup.el $SHARE/emacspeak-18.0/lisp/

    echo "Press a key to copy a customized version of efm"
    read a
    cp efm $BIN

    cd efm
    make
    chmod 755 $SHARE
    chmod 755 $SHARE/mbrola
}

####
# Adding EFM to the new Oralux tree
Copy2Oralux()
{
    echo "Warning: efm is supposed to have been built in the current tree (see InstallPackage)"
    if [ ! -f $BIN ]; then
	echo "$BIN not found"
	return
    fi
    
    cp $BIN $BUILD$BINDIR
    
    mkdir -p $BUILD$SHAREDIR    
    cp -pR $SHARE $BUILD$SHAREDIR
    
    chroot $BUILD bash -c "cd $BINDIR; ln -s $SHARE/mbrola/mbrola-linux-i386 mbrola"

    mkdir -p $BUILD$LISP
    cp -R $LISP/* $BUILD$LISP

# A lighter package
    cd $BUILD$SHARE
    rm -f *.tar.gz
    find . -name "*.o" -type f | xargs rm
    find . -name "*.zip" -type f | xargs rm
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
