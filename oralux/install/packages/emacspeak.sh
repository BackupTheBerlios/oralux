#! /bin/sh
# ----------------------------------------------------------------------------
# emacspeak.sh
# $Id: emacspeak.sh,v 1.10 2006/03/05 18:28:57 gcasse Exp $
# $Author: gcasse $
# Description: Installing emacspeak. Thanks to the Nath's howto: 
# emacspeak-dtk-soft-debinst-howto.htm
# $Date: 2006/03/05 18:28:57 $ |
# $Revision: 1.10 $ |
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
EMACSPEAK=emacspeak-23.0.tar.bz2
ARCH_EMACSPEAK=$ARCHDIR/$EMACSPEAK
LIB_LIST="debian-el edb emacs-goodies-el emacs-wiki muse-el nxml-mode planner-el texinfo w3-el-e21 w3m-el xsltproc"

# Patch by Igor B. Poretsky (multispeech integration, and more)
PATCH=emacspeak-23-2005_12_16.patch

cd $ARCHDIR

if [ ! -e $ARCH_EMACSPEAK ]
    then
    echo "Downloading $EMACSPEAK"
#    wget http://switch.dl.sourceforge.net/sourceforge/emacspeak/$EMACSPEAK
fi

if [ ! -e $ARCHDIR/$PATCH.bz2 ]
    then
    echo "Downloading $PATCH"
#    wget ftp://ftp.rakurs.spb.ru/pub/Goga/projects/speech-interface/patches/emacspeak/emacspeak-23.patch
fi

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install $LIB_LIST

    cd /tmp
    rm -rf emacspeak*
    rm -rf /usr/share/emacs/site-lisp/emacspeak
    tar -jxvf $ARCH_EMACSPEAK
    cp $INSTALL_PACKAGES/emacspeak/$PATCH.bz2 .
    bunzip2 $PATCH.bz2
    cd emacspeak-??.[^p]*    
    patch -p0 -i ../$PATCH
    cp $INSTALL_PACKAGES/emacspeak/*el lisp

#Comment for 0.6b
#     cp $INSTALL_PACKAGES/emacspeak/MakefileECI servers/linux-outloud/Makefile
#     cp $INSTALL_PACKAGES/emacspeak/tcleci.cpp servers/linux-outloud
#     cp $INSTALL_PACKAGES/emacspeak/outloud servers
#     cd ..
#     cp $INSTALL_PACKAGES/vv/outloud .

    make config; make; make install

    EMACSPEAK_DIR="/usr/share/emacs/site-lisp/emacspeak"
    echo "export EMACSPEAK_DIR=$EMACSPEAK_DIR" >> /etc/profile
    echo "export LD_LIBRARY_PATH=/ramdisk/dtk/lib:\$LD_LIBRARY_PATH" >> /etc/profile
    export DTK_PROGRAM=/usr/bin/eflite

    echo "Do you want to use DECtalk (y or n) ?"
    read a
    if [ "$a" == "y" ]
	then
	echo "DECtalk might be installed"
	echo "export DTK_PROGRAM=dtk-soft" >> /etc/profile
	echo "unset DTK_TCL" >> /etc/profile
	cd $EMACSPEAK_DIR/servers/software-dtk
	rm -f tcldtk.o tcldtk.so Makefile
	cp $INSTALL_PACKAGES/emacspeak/Makefile .
#	replace /ramdisk/dtk/include /usr/local/include/dtk/ -- Makefile
	echo "Update the software-dtk Makefile according the optional languages (fr, sp, gr)"
	cp $INSTALL_PACKAGES/emacspeak/tcldtk.c .
	cp $INSTALL_PACKAGES/emacspeak/dtk-soft ..
	make
    fi

    # Installing emacspeak.info
    install-info /usr/share/info/emacspeak.info
    cp $INSTALL_PACKAGES/emacspeak/emacspeak /usr/bin
    cp $INSTALL_PACKAGES/emacspeak/emacspeak.conf2 /etc/emacspeak.conf
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD apt-get install $LIB_LIST

    cd $BUILD/tmp
    rm -rf emacspeak*
    rm -rf $BUILD/usr/share/emacs/site-lisp/emacspeak
    tar -jxvf $ARCH_EMACSPEAK
    cp $INSTALL_PACKAGES/emacspeak/$PATCH.bz2 .
    bunzip2 $PATCH.bz2
    cd emacspeak-??.[^p]*    
    patch -p0 -i ../$PATCH
    cp $INSTALL_PACKAGES/emacspeak/*el lisp

    chroot $BUILD bash -c 'cd /tmp/emacspeak-??.[^p]*; make config; make; make install'

    EMACSPEAK_DIR="/usr/share/emacs/site-lisp/emacspeak"
    echo "export EMACSPEAK_DIR=$EMACSPEAK_DIR" >> $BUILD/etc/profile
    echo "export LD_LIBRARY_PATH=/ramdisk/dtk/lib:\$LD_LIBRARY_PATH" >> $BUILD/etc/profile

#     # Useful for the hd install (test environment)
#     if [ ! -d $DTK_EMACSPEAK ]
#         then
#         mkdir -p $DTK_EMACSPEAK
#     fi

    cd $BUILD/etc
    rm -f emacspeak.conf
    ln -s $DTK_EMACSPEAK/emacspeak.conf .

    rm -f DECtalk.conf
    ln -s $DTK_BASE/DECtalk.conf .

    # Environment for compiling libdtk
    # tcldtk.so will be compiled when the user will install the dtk software.
    # And two files will be built: tcldtk.o and tcldtk.so.
    # Two symbolic links are just added to the ramdisk.
    cd $BUILD/$EMACSPEAK_DIR/servers/software-dtk

    rm -f tcldtk.o
    ln -s $DTK_EMACSPEAK/tcldtk.o .

    rm -f tcldtk.so
    ln -s $DTK_EMACSPEAK/tcldtk.so .

    # The Makefile may be updated according to the optional tts library  
    rm -f Makefile
    ln -s $DTK_EMACSPEAK/Makefile .

    # The original file tcldtk.c is replaced to be compliant with the beta french release
    cp $INSTALL_PACKAGES/emacspeak/tcldtk.c .
    cp $INSTALL_PACKAGES/emacspeak/dtk-soft ..

#Comment for 0.6b
#     cp $INSTALL_PACKAGES/emacspeak/outloud ..

    # Installing emacspeak.info
    cd
    chroot $BUILD install-info /usr/share/info/emacspeak.info

    # Installing the customized emacspeak scripts
    cp $INSTALL_PACKAGES/emacspeak/emacspeak $BUILD/usr/bin
}

if [ ! -e $ARCH_EMACSPEAK ]
    then
    cd $ARCHDIR
    echo "Downloading $EMACSPEAK"
    wget -nd http://unc.dl.sourceforge.net/sourceforge/emacspeak/$EMACSPEAK
fi

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
