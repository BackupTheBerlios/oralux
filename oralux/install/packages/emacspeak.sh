#! /bin/sh
# ----------------------------------------------------------------------------
# emacspeak.sh
# $Id: emacspeak.sh,v 1.4 2005/03/13 22:09:54 gcasse Exp $
# $Author: gcasse $
# Description: Installing emacspeak. Thanks to the Nath's howto: 
# emacspeak-dtk-soft-debinst-howto.htm
# $Date: 2005/03/13 22:09:54 $ |
# $Revision: 1.4 $ |
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
EMACSPEAK=emacspeak-21.tar.bz2
ARCH_EMACSPEAK=$ARCH/$EMACSPEAK
RELEASE=21

####
# Replacing the original EMACSPEAK file from a customized one
substituteFile()
{
    file=$1
    dir=$2
    # If the new file "dir/file" (either coming from Emacspeak or already updated for Oralux) isn't equal to the saved one.
    comp=`diff $INSTALL_PACKAGES/emacspeak/$file.sav $dir/$file |wc -l|awk '{print $1}'`
    # we have to compare it with the previously updated Oralux file
    # If they are distinct, we are sure the new file has to replace the saved file.
    # and then be updated with the oralux customization
    # 
    comp2=`diff $INSTALL_PACKAGES/emacspeak/$file $dir/$file |wc -l|awk '{print $1}'`
    if [ "$comp" != "0" ] && [ "$comp2" != "0" ]; then
        echo "the new EMACSPEAK file $dir/$file is different from our original one $INSTALL_PACKAGES/emacspeak/$file.sav"
        echo "--> Press q, to quit (so that the following operations are achieved:"
	echo "1. The saved file $INSTALL_PACKAGES/emacspeak/$file.sav might be equal to the new file $dir/$file from Emacspeak."
	echo "2. Our customized file $INSTALL_PACKAGES/emacspeak/$file might be updated with the new features from $dir/$file)."
	echo "--> Press any other key to jump this stage"
	read a

	if [ "$a" == "q" ] || [ "$a" == "Q" ]; then
	    exit 1
	fi  
    elif [ "$comp2" != "0" ]; then
        echo "the new EMACSPEAK file $dir/$file is different from our customized one $INSTALL_PACKAGES/emacspeak/$file"
        echo "--> Press q, to quit (so that the following operations are achieved:"
	echo "The customized file $INSTALL_PACKAGES/emacspeak/$file will take in account the new features from $dir/$file)."
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
    cd /tmp
    rm -rf emacspeak*

    if [ $method = "TARGZ" ]
        then
        tar -zxvf $ARCH_EMACSPEAK
    fi
    if [ $method = "TARBZ2" ]
        then
        tar -jxvf $ARCH_EMACSPEAK
    fi

    # Installing emacspeak
    cd /tmp/emacspeak*; 
    
    # Patch by Igor B. Poretsky (multispeech integration, and more)
    patch -p0 -i emacspeak-$RELEASE.patch
    make config; make; make install

    EMACSPEAK_DIR="/usr/share/emacs/site-lisp/emacspeak"
    echo "export EMACSPEAK_DIR=$EMACSPEAK_DIR" >> /etc/profile
    echo "export LD_LIBRARY_PATH=/ramdisk/dtk/lib:\$LD_LIBRARY_PATH" >> /etc/profile
    export DTK_PROGRAM=/usr/local/bin/eflite

    echo "Do you want to use DECtalk (y or n) ?"
    read a
    if [ "$a" == "y" ]
	then
	echo "DECtalk might be installed"
	echo "export DTK_PROGRAM=dtk-soft" >> /etc/profile
	echo "unset DTK_TCL" >> /etc/profile
	cd $EMACSPEAK_DIR/servers/software-dtk
	rm -f tcldtk.o
	rm -f tcldtk.so
	rm -f Makefile
	cp $INSTALL_PACKAGES/emacspeak/Makefile .
	replace /ramdisk/dtk/include /usr/local/include/dtk/ -- Makefile
	echo "Update the software-dtk Makefile according the optional languages (fr, sp, gr)"
	cp $INSTALL_PACKAGES/emacspeak/tcldtk.c .
	cp $INSTALL_PACKAGES/emacspeak/dtk-soft ..
	make
    fi

    # Installing emacspeak.info
    install-info /usr/share/info/emacspeak.info
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    a=`echo $ARCH_EMACSPEAK|grep tar.gz|wc -c|sed "s/ //g"`
    if [ $a != 0 ]
        then
        method="TARGZ"
    fi

    a=`echo $ARCH_EMACSPEAK|grep tar.bz2|wc -c|sed "s/ //g"`
    if [ $a != 0 ]
        then
        method="TARBZ2"
    fi

    cd $BUILD/var/tmp
    rm -rf emacspeak*

    if [ $method = "TARGZ" ]
        then
        tar -zxvf $ARCH_EMACSPEAK
    fi
    if [ $method = "TARBZ2" ]
        then
        tar -jxvf $ARCH_EMACSPEAK
    fi

    # For emacspeak-amphetadesk.el (emacspeak 19.0)
    chroot $BUILD apt-get install w3-el-e21

    # Installing emacspeak
    chroot $BUILD bash -c "rm -f /usr/share/emacs/site-lisp/emacspeak/servers/software-dtk/Makefile; rm -f /usr/share/emacs/site-lisp/emacspeak/servers/software-dtk/tcldtk.*; cd /var/tmp/emacspeak*; patch -p0 -i emacspeak-$RELEASE.patch;make config; make; make install"

    cd $BUILD/var/tmp
    rm -rf emacspeak*

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

    # Installing emacspeak.info
    cd
    chroot $BUILD install-info /usr/share/info/emacspeak.info

    # Installing the customized emacspeak scripts
#     substituteFile emacspeak $BUILD/usr/bin
#     substituteFile emacspeak-setup.el $BUILD/usr/bin
    cp $INSTALL_PACKAGES/emacspeak/emacspeak $BUILD/usr/bin
    cp $INSTALL_PACKAGES/emacspeak/emacspeak-setup.el $BUILD/usr/bin
}

if [ ! -e $ARCH_EMACSPEAK ]
    then
    cd $ARCH
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
