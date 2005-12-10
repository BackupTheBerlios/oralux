#! /bin/sh
# ----------------------------------------------------------------------------
# multispeech.sh
# $Id: multispeech.sh,v 1.8 2005/12/10 22:35:07 gcasse Exp $
# $Author: gcasse $
# Description: Installing Multispeech.
# $Date: 2005/12/10 22:35:07 $ |
# $Revision: 1.8 $ |
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

RULEX_RELEASE=0.9.22
MULTISPEECH_RELEASE=1.2.2-oralux
ARCH_RULEX=$ARCH/rulex-$RULEX_RELEASE.tar.gz 
ARCH_MULTISPEECH_SRC=$ARCH/multispeech-$MULTISPEECH_RELEASE.tar.bz2
ARCH_RU_TTS=$ARCH/ru_tts-0.4-i586-1.tgz
ARCH_LEXICON=$ARCH/freespeech-10.0-alt2.i586.rpm

echo "Previously compiled with g++-3.2"

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install libgdbm-dev

    MBROLA="/usr/local/share/mbrola"
    install -d $MBROLA

    BIN="/usr/local/bin"
    install -d $BIN

    export TMP=$tmp
#    install -d $TMP

### Rulex
    cd $TMP
    rm -rf rulex*
    tar -zxvf $ARCH_RULEX
    cd /tmp/rulex*
    make lexicon
    mkdir -p /usr/local/lib/ru_tts
    make install

### ru_tts
    cd $TMP
    mkdir ru_tts
     cd ru_tts
    tar -zxvf $ARCH_RU_TTS
    install -m 555 usr/local/bin/* $BIN

### Lexicon for English synthesis
    cd $TMP
    rm -rf freespeech*
    alien -g $ARCH_LEXICON
    SRC="freespeech-10.0.orig/usr/share/freespeech"
    install -m 444 $SRC/lexicon.dir $MBROLA/lexicon.en.dir
    install -m 444 $SRC/lexicon.dir $MBROLA/lexicon.en.pag

    SRC="freespeech-10.0.orig/usr/share/doc/freespeech-10.0"
    DOC="$BUILD/usr/local/share/doc/freespeech"
    install -d $DOC
    install -m 444 $SRC/Copying $DOC

### Multispeech
    cd $TMP
    rm -rf multispeech-*
    tar -jxvf $ARCH_MULTISPEECH_SRC 
    cd multispeech-*
    export SRC=`pwd`

    make
    cd $SRC/lisp
    make

### Installing the elisp files
    install -m 444 $SRC/lisp/*.el* /usr/share/emacs/site-lisp/emacspeak/lisp

### Installing the remaining files
    LIB="/usr/local/lib/multispeech"

    install -d $LIB/players
    install -d $LIB/tts

    for i in "br de en es fr ru"; do
	install -d $LIB/letters/$i
	install -m 444 $SRC/letters/$i/* $LIB/letters/$i
	install -m 555 $SRC/scripts/players/$i $LIB/players
	install -m 555 $SRC/scripts/tts/$i $LIB/tts
    done
    
    install -m 555 $SRC/binaries/speech_server $LIB

    install -m 555 $SRC/binaries/freephone $BIN
    install -m 555 $SRC/binaries/rawplay $BIN
    install -m 555 $SRC/binaries/tones $BIN

# Clear temporary files
    rm -rf $SRC/multispeech-$MULTISPEECH_RELEASE
    rm -rf $SRC/rulex-$RULEX_RELEASE

    cd /usr/share/emacs/site-lisp/emacspeak/servers
    echo multispeech >> .servers
    rm -f multispeech
    ln -s /usr/local/lib/multispeech/speech_server multispeech
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD apt-get install libgdbm-dev

    export MBROLA="$BUILD/usr/local/share/mbrola"
    install -d $MBROLA

    export BIN="$BUILD/usr/local/bin"
    install -d $BIN

    export TMP=$BUILD/tmp
    install -d $TMP

### Rulex
    cd $TMP
    rm -rf rulex*
    tar -zxvf $ARCH_RULEX
    chroot $BUILD bash -c "cd /tmp/rulex*; make lexicon; mkdir -p /usr/local/lib/ru_tts; make install"

### ru_tts
    cd $TMP
    mkdir ru_tts
    cd ru_tts
    tar -zxvf $ARCH_RU_TTS
    install -m 555 usr/local/bin/* $BIN

### Lexicon for English synthesis
    cd $TMP
    rm -rf freespeech*
    alien -g $ARCH_LEXICON
    SRC="freespeech-10.0.orig/usr/share/freespeech"
    install -m 444 $SRC/lexicon.dir $MBROLA/lexicon.en.dir
    install -m 444 $SRC/lexicon.dir $MBROLA/lexicon.en.pag

    SRC="freespeech-10.0.orig/usr/share/doc/freespeech-10.0"
    DOC="$BUILD/usr/local/share/doc/freespeech"
    install -d $DOC
    install -m 444 $SRC/Copying $DOC

### Multispeech
    cd $TMP
    rm -rf multispeech-*
    tar -jxvf $ARCH_MULTISPEECH_SRC 
    cd multispeech-*
    export SRC=`pwd`

    make
    cd $SRC/lisp
    make

### Installing the elisp files
    install -m 444 $SRC/lisp/*.el* $BUILD/usr/share/emacs/site-lisp/emacspeak/lisp

### Installing the remaining files
    LIB="$BUILD/usr/local/lib/multispeech"

    install -d $LIB/players
    install -d $LIB/tts

    for i in "br de en es fr ru"; do
	install -d $LIB/letters/$i
	install -m 444 $SRC/letters/$i/* $LIB/letters/$i
	install -m 555 $SRC/scripts/players/$i $LIB/players
	install -m 555 $SRC/scripts/tts/$i $LIB/tts
    done

    install -m 555 $SRC/binaries/speech_server $LIB

    install -m 555 $SRC/binaries/freephone $BIN
    install -m 555 $SRC/binaries/rawplay $BIN
    install -m 555 $SRC/binaries/tones $BIN

# Clear temporary files
    rm -rf $SRC/multispeech-$MULTISPEECH_RELEASE
    rm -rf $SRC/rulex-$RULEX_RELEASE

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