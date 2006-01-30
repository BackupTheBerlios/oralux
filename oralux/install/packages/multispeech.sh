#! /bin/sh
# ----------------------------------------------------------------------------
# multispeech.sh
# $Id: multispeech.sh,v 1.14 2006/01/30 22:49:38 gcasse Exp $
# $Author: gcasse $
# Description: Installing Multispeech.
# $Date: 2006/01/30 22:49:38 $ |
# $Revision: 1.14 $ |
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
set -vx
source ../oralux.conf

export RULEX_RELEASE=1.0
export MULTISPEECH_RELEASE=oralux-2006-01-29
export ARCH_RULEX=$ARCH/rulex-$RULEX_RELEASE.tar.gz 
export ARCH_MULTISPEECH_SRC=$ARCH/multispeech-$MULTISPEECH_RELEASE.tar.bz2
#export ARCH_RU_TTS=$ARCH/ru_tts.bz2
export ARCH_LEXICON=$ARCH/enlex.tar.bz2
#export DOC="/usr/local/share/doc"
export BIN="/usr/local/bin"
export MBROLA="/usr/local/share/mbrola"


cd $ARCH
wget http://oralux.org/tmp/multispeech-$MULTISPEECH_RELEASE.tar.bz2
# wget ftp://ftp.rakurs.spb.ru/pub/Goga/projects/speech-interface/rulex/rulex-$RULEX_RELEASE.tar.gz
# wget ftp://ftp.rakurs.spb.ru/pub/Goga/projects/speech-interface/ru_tts/binaries/ru_tts.bz2
# wget ftp://ftp.rakurs.spb.ru/pub/Goga/projects/speech-interface/current/enlex.tar.bz2

echo "Previously compiled with g++-3.3"

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install libdb3-dev

    install -d $MBROLA
    install -d $BIN

    export TMP=$tmp
#    install -d $TMP

### Rulex
    cd $TMP
    rm -rf rulex*
    tar -zxvf $ARCH_RULEX
    cd /tmp/rulex*
    make lexicon
    install -d /usr/local/lib/ru_tts
    make install

### ru_tts
    cd $TMP
    cp $ARCH/ru_tts.bz2 .
    bunzip2 ru_tts.bz2
    install -m 555 ru_tts $BIN

### Lexicon for English synthesis
    cd $TMP
    cp $ARCH/enlex.tar.bz2 .
    tar -jxvf enlex.tar.bz2
    cd enlex
    make
    install -m 444 lexicon $MBROLA/lexicon.en

### Multispeech
    cd $TMP
    rm -rf multispeech-*
    tar -jxvf $ARCH_MULTISPEECH_SRC 
    cd multispeech-*
    export SRC=`pwd`

    echo "TODO: make letters_de and letters_fr (de, fr) must be run as current user"
    read a

    make

### Installing the elisp files
#    install -m 444 $SRC/lisp/*.el* /usr/share/emacs/site-lisp/emacspeak/lisp

### Installing the remaining files
    LIB="/usr/local/lib/multispeech"

    install -d $LIB/players
    install -d $LIB/tts

    for i in pt_BR de en es fr ru; do
	install -d $LIB/letters/$i
	install -m 444 $SRC/letters/$i/* $LIB/letters/$i
	install -m 555 $SRC/scripts/players/$i $LIB/players
	install -m 555 $SRC/scripts/tts/$i $LIB/tts
    done
    
    install -m 555 $SRC/src/multilingual/speech_server $LIB
    install -m 555 $SRC/src/freephone/freephone $BIN
    install -m 555 $SRC/src/tones/tones $BIN

# Clear temporary files
#    rm -rf $TMP/multispeech-$MULTISPEECH_RELEASE
#    rm -rf $TMP/rulex-$RULEX_RELEASE

    cd /usr/share/emacs/site-lisp/emacspeak/servers
    echo multispeech >> .servers
    rm -f multispeech
    ln -s /usr/local/lib/multispeech/speech_server multispeech
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD apt-get install libdb3-dev

    export MBROLA="$BUILD/$MBROLA"
    install -d $MBROLA

    export BIN="$BUILD/$BIN"
    install -d $BIN

    export TMP=$BUILD/tmp
    install -d $TMP

### Rulex
    cd $TMP
    rm -rf rulex*
    tar -zxvf $ARCH_RULEX
    chroot $BUILD bash -c "cd /tmp/rulex*; make lexicon; install -d /usr/local/lib/ru_tts; make install"

### ru_tts
    cd $TMP
    cp $ARCH/ru_tts.bz2 .
    bunzip2 ru_tts.bz2
    install -m 555 ru_tts $BIN

### Lexicon for English synthesis
    cd $TMP
    cp $ARCH/enlex.tar.bz2 .
    tar -jxvf enlex.tar.bz2
    cd enlex
    make
    install -m 444 lexicon $MBROLA/lexicon.en

### Multispeech
    cd $TMP
    rm -rf multispeech-*
    tar -jxvf $ARCH_MULTISPEECH_SRC 
    cd multispeech-*
    export SRC=`pwd`

    echo "TODO: make letters_de and letters_fr (de, fr) must be run as current user"
    read a
    make
 
### Installing the remaining files
    LIB="$BUILD/usr/local/lib/multispeech"

    install -d $LIB/players
    install -d $LIB/tts

    for i in pt_BR de en es fr ru; do
	install -d $LIB/letters/$i
	install -m 444 $SRC/letters/$i/* $LIB/letters/$i
	install -m 555 $SRC/scripts/players/$i $LIB/players
	install -m 555 $SRC/scripts/tts/$i $LIB/tts
    done

    install -m 555 $SRC/src/multilingual/speech_server $LIB
    install -m 555 $SRC/src/freephone/freephone $BIN
    install -m 555 $SRC/src/tones/tones $BIN

# Clear temporary files
#    rm -rf $SRC/multispeech-$MULTISPEECH_RELEASE
#    rm -rf $SRC/rulex-$RULEX_RELEASE

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