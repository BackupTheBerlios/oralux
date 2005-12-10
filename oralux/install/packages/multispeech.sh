#! /bin/sh
# ----------------------------------------------------------------------------
# multispeech.sh
# $Id: multispeech.sh,v 1.7 2005/12/10 14:37:45 gcasse Exp $
# $Author: gcasse $
# Description: Installing Multispeech.
# $Date: 2005/12/10 14:37:45 $ |
# $Revision: 1.7 $ |
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

DIR=$INSTALL_PACKAGES/multispeech

RULEX_RELEASE=0.9.22
MULTISPEECH_RELEASE=1.2.2-oralux
ARCH_RULEX=$ARCH/rulex-$RULEX_RELEASE.tar.gz 
ARCH_MULTISPEECH_BIN=$ARCH/multispeech-1.2-i586-1.tgz
ARCH_MULTISPEECH_SRC=$DIR/multispeech-$MULTISPEECH_RELEASE.tar.bz2
ARCH_RU_TTS=$ARCH/ru_tts-0.4-i586-1.tgz
ARCH_LEXICON=$ARCH/freespeech-10.0-alt2.i586.rpm



echo "Previously compiled with g++-3.2"

####
# Installing the package in the current tree
InstallPackage()
{
### Rulex
    cd /tmp
    rm -rf rulex*
    tar -zxvf $ARCH_RULEX
    cd rulex*
    make lexicon
    mkdir -p /usr/local/lib/ru_tts
    make install

### ru_tts
    cd /tmp
    mkdir ru_tts
     cd ru_tts
    tar -zxvf $ARCH_RU_TTS
    cp usr/local/bin/* /usr/local/bin

    mkdir -p /usr/local/share/mbrola

### Lexicon for English synthesis
    cd /tmp
    rm -rf freespeech*
    alien -g $ARCH_LEXICON
    cp freespeech-10.0.orig/usr/share/freespeech/lexicon.dir /usr/local/share/mbrola/lexicon.en.dir
    cp freespeech-10.0.orig/usr/share/freespeech/lexicon.dir /usr/local/share/mbrola/lexicon.en.pag
    mkdir -p /usr/share/oralux/doc/license/freespeech
    cp freespeech-10.0.orig/usr/share/doc/freespeech-10.0/Copying /usr/share/oralux/doc/license/freespeech

### Multispeech
    cd /tmp
    rm -rf multispeech-*
    tar -jxvf $ARCH_MULTISPEECH_SRC 
    cd multispeech-*
    export TMP=`pwd`

    apt-get install libgdbm-dev

### Compiling Multispeech
    make
    cd $TMP/lisp
    make

### Installing the elisp files
    cp -f $TMP/lisp/*.el* /usr/share/emacs/site-lisp/emacspeak/lisp

### Installing the remaining files
    mkdir -p /usr/local/lib/multispeech
    cd /usr/local/lib/multispeech
    cp -pR $TMP/letters .
    cp -pR $TMP/scripts/players .
    cp -pR $TMP/scripts/tts .
    cp $TMP/binaries/speech_server .

    cd /usr/local/bin
    cp $TMP/binaries/freephone .
    cp $TMP/binaries/rawplay .
    cp $TMP/binaries/tones .

# Clear temporary files
    rm -rf $TMP/multispeech-$MULTISPEECH_RELEASE
    rm -rf $TMP/rulex-$RULEX_RELEASE

    cd /usr/share/emacs/site-lisp/emacspeak/servers
    echo multispeech >> .servers
    rm -f multispeech
    ln -s /usr/local/lib/multispeech/speech_server multispeech
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
### Rulex
    cd $BUILD/tmp
    rm -rf rulex*
    tar -zxvf $ARCH_RULEX
    
    chroot $BUILD bash -c "cd /tmp/rulex*; make lexicon; mkdir -p /usr/local/lib/ru_tts; make install"

### ru_tts
    cd $BUILD/tmp
    mkdir ru_tts
    cd ru_tts
    tar -zxvf $ARCH_RU_TTS
    cp usr/local/bin/* $BUILD/usr/local/bin

    mkdir -p $BUILD/usr/local/share/mbrola

### Lexicon for English synthesis
    cd $BUILD/tmp
    rm -rf freespeech*
    alien -g $ARCH_LEXICON
    cp freespeech-10.0.orig/usr/share/freespeech/lexicon.dir $BUILD/usr/local/share/mbrola/lexicon.en.dir
    cp freespeech-10.0.orig/usr/share/freespeech/lexicon.dir $BUILD/usr/local/share/mbrola/lexicon.en.pag
    mkdir -p $BUILD/usr/share/oralux/doc/license/freespeech
    cp freespeech-10.0.orig/usr/share/doc/freespeech-10.0/Copying $BUILD/usr/share/oralux/doc/license/freespeech

### Multispeech
    cd $BUILD/tmp
    rm -rf multispeech-*
    tar -jxvf $ARCH_MULTISPEECH_SRC 
    cd multispeech-*
    export TMP=`pwd`

    chroot $BUILD apt-get install libgdbm-dev

### Compiling Multispeech
    make
    cd $TMP/lisp
    make

### Installing the elisp files
    cp -f $TMP/lisp/Russian-spelling.el $BUILD/usr/share/emacs/site-lisp/emacspeak/lisp

### Installing the remaining files
    mkdir -p $BUILD/usr/local/lib/multispeech
    cd $BUILD/usr/local/lib/multispeech
    cp -pR $TMP/letters .
    cp -pR $TMP/scripts/players .
    cp -pR $TMP/scripts/tts .
    cp $TMP/binaries/speech_server .

    cd $BUILD/usr/local/bin
    cp $TMP/binaries/freephone .
    cp $TMP/binaries/rawplay .
    cp $TMP/binaries/tones .

# Clear temporary files
    rm -rf $TMP/multispeech-$MULTISPEECH_RELEASE
    rm -rf $TMP/rulex-$RULEX_RELEASE

    chroot $BUILD bash -c "cd /usr/share/emacs/site-lisp/emacspeak/servers; echo multispeech >> .servers; rm -f multispeech; ln -s /usr/local/lib/multispeech/speech_server multispeech"
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